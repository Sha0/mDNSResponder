/*
 * Copyright (c) 2002 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 *
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 1.1 (the
 * "License").  You may not use this file except in compliance with the
 * License.  Please obtain a copy of the License at
 * http://www.apple.com/publicsource and read it before using this file.
 *
 * This Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 *
 * @APPLE_LICENSE_HEADER_END@
 */

#include <libc.h>
#include <CoreFoundation/CoreFoundation.h>
#include <DNSServiceDiscovery/DNSServiceDiscovery.h>

#define DomainMsg(X) ((X) == DNSServiceDomainEnumerationReplyAddDomain        ? "Added"     :          \
                      (X) == DNSServiceDomainEnumerationReplyAddDomainDefault ? "(Default)" :          \
                      (X) == DNSServiceDomainEnumerationReplyRemoveDomain     ? "Removed"   : "Unknown")

void regdom_reply(int resultType, char *replyDomain, int flags, void *context)
	{
	printf("Recommended Registration Domain %s %s", replyDomain, DomainMsg(resultType));
	if (flags) printf(" Flags: %X", flags);
	printf("\n");
	}

void browsedom_reply(int resultType, char *replyDomain, int flags, void *context)
	{
	printf("Recommended Browsing Domain %s %s", replyDomain, DomainMsg(resultType));
	if (flags) printf(" Flags: %X", flags);
	printf("\n");
	}

void browse_reply(int resultType, char *replyName, char *replyType, char *replyDomain, int flags, void *context)
	{
	char *op = (resultType == DNSServiceBrowserReplyAddInstance) ? "Found" : "Removed";
	printf("Service \"%s\", type \"%s\", domain \"%s\" %s", replyName, replyType, replyDomain, op);
	if (flags) printf(" Flags: %X", flags);
	printf("\n");
	}

void resolve_reply(struct sockaddr *interface, struct sockaddr *address, char *txtRecord, int flags, void *context)
	{
	if (address->sa_family != AF_INET)
		printf("Unknown address family %d\n", address->sa_family);
	else
		{
		struct sockaddr_in *ip = (struct sockaddr_in *)address;
		union { uint32_t l; u_char b[4]; } addr = { ip->sin_addr.s_addr };
		union { uint16_t s; u_char b[2]; } port = { ip->sin_port };
		uint16_t PortAsNumber = ((uint16_t)port.b[0]) << 8 | port.b[1];
		printf("Service can be reached at %d.%d.%d.%d:%u %s",
			addr.b[0], addr.b[1], addr.b[2], addr.b[3], PortAsNumber, txtRecord);
		if (flags) printf(" Flags: %X", flags);
		printf("\n");
		}
	}

void reg_reply(int errorCode, void *context)
	{
	if (errorCode)
		printf("Got a reply from the server with error %d\n", errorCode);
	else
		printf("Got a reply from the server: Name now registered and active\n");
	}

void MyHandleMachMessage(CFMachPortRef port, void *msg, CFIndex size, void *info)
	{
	DNSServiceDiscovery_handleReply(msg);
	}

int main(int argc, char ** argv)
	{
	char ch;
	char *dom;
	dns_service_discovery_ref client = NULL;
	mach_port_t               port   = NULL;

	if (argc < 2) goto Fail;		// Minimum command line is the command name and one argument

	while ((ch = getopt(argc, (char * const *)argv, "ERFBL")) != -1)
		{
		switch (ch)
			{
			case 'E':	printf("Looking for recommended registration domains:\n");
						client = DNSServiceDomainEnumerationCreate(1, regdom_reply, nil);
						break;

			case 'F':	printf("Looking for recommended browsing domains:\n");
						client = DNSServiceDomainEnumerationCreate(0, browsedom_reply, nil);
						break;

			case 'B':	if (argc < optind+1) goto Fail;
						dom = (argc < optind+2) ? "" : argv[optind+1];
						printf("Browsing for %s%s\n", argv[optind+0], dom);
						client = DNSServiceBrowserCreate(argv[optind+0], dom, browse_reply, nil);
						break;

			case 'L':	if (argc < optind+2) goto Fail;
						dom = (argc < optind+3) ? "" : argv[optind+2];
						printf("Lookup %s.%s%s\n", argv[optind+0], argv[optind+1], dom);
						client = DNSServiceResolverResolve(argv[optind+0], argv[optind+1], dom, resolve_reply, nil);
						break;

			case 'R':	if (argc < optind+4) goto Fail;
						{
						uint16_t PortAsNumber = atoi(argv[optind+3]);
						Opaque16 registerPort = { { PortAsNumber >> 8, PortAsNumber & 0xFF } };
						char *txt = (argc > optind+4) ? argv[optind+4] : "";
						printf("Registering Service %s.%s%s port %s %s\n", argv[optind+0], argv[optind+1], argv[optind+2], argv[optind+3], txt);
						client = DNSServiceRegistrationCreate(argv[optind+0], argv[optind+1], argv[optind+2], registerPort, txt, reg_reply, nil);
						break;
						}

			default: goto Exit;
			}

		if (client) port = DNSServiceDiscoveryMachPort(client);
		if (!port)
			fprintf(stderr, "Could not connect to mDNSResponder daemon\n");
		else
			{
			CFMachPortContext context = { 0, 0, NULL, NULL, NULL };
			Boolean           shouldFreeInfo;
			CFMachPortRef cfMachPort = CFMachPortCreateWithPort(kCFAllocatorDefault, port, MyHandleMachMessage, &context, &shouldFreeInfo);
			CFRunLoopSourceRef rls = CFMachPortCreateRunLoopSource(NULL, cfMachPort, 0);
			CFRunLoopAddSource(CFRunLoopGetCurrent(), rls, kCFRunLoopDefaultMode);
			CFRelease(rls);
			printf("Talking to DNS SD Daemon at Mach port %d\n", port);
			}
		}
	CFRunLoopRun();
Exit:
	return 0;

Fail:
	fprintf(stderr, "%s -E             (Enumerate recommended registration domains)\n", argv[0]);
	fprintf(stderr, "%s -F                 (Enumerate recommended browsing domains)\n", argv[0]);
	fprintf(stderr, "%s -B        <Type> <Domain>   (Browse for Services Instances)\n", argv[0]);
	fprintf(stderr, "%s -L <Name> <Type> <Domain>      (Look Up a Service Instance)\n", argv[0]);
	fprintf(stderr, "%s -R <Name> <Type> <Domain> <Port> <TXT> (Register a Service)\n", argv[0]);
	return 0;
	}
