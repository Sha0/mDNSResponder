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

void regdom_reply(int resultType, char *replyDomain, int flags, void *context)
	{
    char *msg = (resultType == DNSServiceDomainEnumerationReplyAddDomain)        ? "Added" :
                (resultType == DNSServiceDomainEnumerationReplyAddDomainDefault) ? "(Default)" :
                (resultType == DNSServiceDomainEnumerationReplyRemoveDomain)     ? "Removed" : "Unknown";
    printf("Recommended Registration Domain %s %s", replyDomain, msg);
	if (flags) printf(" Flags: %X", flags);
	printf("\n");
	}

void browsedom_reply(int resultType, char *replyDomain, int flags, void *context)
	{
    char *msg = (resultType == DNSServiceDomainEnumerationReplyAddDomain)        ? "Added" :
                (resultType == DNSServiceDomainEnumerationReplyAddDomainDefault) ? "(Default)" :
                (resultType == DNSServiceDomainEnumerationReplyRemoveDomain)     ? "Removed" : "Unknown";
    printf("Recommended Browsing Domain %s %s", replyDomain, msg);
	if (flags) printf("flags: %X", flags);
	printf("\n");
	}

void browse_reply(int resultType, char *replyName, char *replyType, char *replyDomain, int 	flags, void	*context)
	{
	char *op = (resultType == DNSServiceBrowserReplyAddInstance) ? "Found" : "Removed";
	printf("Service \"%s\", type \"%s\", domain \"%s\" %s", replyName, replyType, replyDomain, op);
	if (flags) printf(" Flags: %X", flags);
	printf("\n");
	}

void resolve_reply(struct sockaddr *interface, struct sockaddr *address, char *txtRecord, int flags, void *context)
	{
    printf("resolve_reply interface %p, address %p\n", interface, address);
	if (0 && address->sa_family != AF_INET)
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
        // For this demo code we just get one answer and exit
        // A real application might wait to see if there are more addresses coming
        exit(0);
		}
	}

void reg_reply(int errorCode, void *context)
{
    if (errorCode)
        printf("Got a reply from the server with error %d\n", errorCode);
    else
        printf("Got a reply from the server: Name now registered and active\n");
}

void MyHandleMachMessage ( CFMachPortRef port, void * msg, CFIndex size, void * info )
{
    DNSServiceDiscovery_handleReply(msg);
}

int main(int argc, char ** argv)
{
    CFMachPortRef           cfMachPort;
    CFMachPortContext       context;
    Boolean                 shouldFreeInfo;
    char            ch;
    mach_port_t port;
    dns_service_discovery_ref 	dns_client;

    if (argc < 2) goto Fail;		// Minimum command line is the command name and one argument

    context.version                 = 1;
    context.info                    = 0;
    context.retain                  = NULL;
    context.release                 = NULL;
    context.copyDescription 	    = NULL;
    
    while ((ch = getopt(argc, (char * const *)argv, "ERFBL")) != -1)
    {
        switch (ch) {

            case 'E':	dns_client =  DNSServiceDomainEnumerationCreate(1, regdom_reply, nil);
						printf("Looking for recommended registration domains:\n");
						break;

            case 'F':	dns_client =  DNSServiceDomainEnumerationCreate(0, browsedom_reply, nil);
						printf("Looking for recommended browsing domains:\n");
						break;

            case 'B':	if (argc < optind+2) goto Fail;
						printf("Browsing for %s%s\n", argv[optind+0], argv[optind+1]);
						dns_client = DNSServiceBrowserCreate(argv[optind+0], argv[optind+1], browse_reply, nil);
						break;

            case 'L':	if (argc < optind+3) goto Fail;
						printf("Lookup %s.%s%s\n", argv[optind+0], argv[optind+1], argv[optind+2]);
						dns_client = DNSServiceResolverResolve(argv[optind+0], argv[optind+1], argv[optind+2], resolve_reply, nil);
						break;

            case 'R':	{
						char 	*demux = "";
						Opaque16 registerPort;
						if (argc < optind+4) goto Fail;
						if (argc > optind+4) demux = argv[optind+4];
						printf("Registering Service %s.%s%s port %s %s\n", argv[optind+0], argv[optind+1], argv[optind+2], argv[optind+3], demux);
						registerPort.b[0] = atoi(argv[optind+3]) >> 8;
						registerPort.b[1] = atoi(argv[optind+3]) & 0xFF;
						dns_client = DNSServiceRegistrationCreate(argv[optind+0], argv[optind+1], argv[optind+2], registerPort, demux, reg_reply, nil);
						break;
						}

            default:
                goto Exit;
        }
        port = DNSServiceDiscoveryMachPort(dns_client);

        if (port) {
			CFRunLoopSourceRef	rls;
            printf("Talking to DNS SD Daemon at Mach port %d\n", port);
            cfMachPort = CFMachPortCreateWithPort ( kCFAllocatorDefault, port, ( CFMachPortCallBack ) MyHandleMachMessage,&context,&shouldFreeInfo );
            /* Create and add a run loop source for the port */
            rls = CFMachPortCreateRunLoopSource(NULL, cfMachPort, 0);
            CFRunLoopAddSource(CFRunLoopGetCurrent(), rls, kCFRunLoopDefaultMode);
            CFRelease(rls);
        } else {
            printf("Could not obtain client port\n");
            goto Exit;
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
