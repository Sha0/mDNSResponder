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

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <mach/mach.h>
#include <net/if_types.h>
#include <libc.h>
#include <mach/mach_error.h>
#include <mach/message.h>
#include <servers/bootstrap.h>
#include <mach/mach_error.h>
#include <pthread.h>
#include <assert.h>

#include <SystemConfiguration/SystemConfiguration.h>
#include <SystemConfiguration/SCDPlugin.h>
#include <SystemConfiguration/SCPrivate.h>	// for SCLog()

#include <CoreFoundation/CoreFoundation.h>

#include <DNSServiceDiscovery/DNSServiceDiscovery.h>

void MyHandleMachMessage ( CFMachPortRef port, void * msg, CFIndex size, void * info )
{
    DNSServiceDiscovery_handleReply(msg);
}

void reg_reply(int errorCode, void *context)
{
    printf("Got a reply from the server with error %d\n", errorCode);
}

void browse_reply(int resultType, char *replyName, char *replyType, char *replyDomain, int 	flags, void	*context)
{
    printf("Got a reply from the server %d %s %s %s %d\n", resultType, replyName, replyType, replyDomain, flags);
}

void enum_reply(int resultType, char *replyDomain, int flags, void *context)
{
	char *def = "";
	if (resultType == DNSServiceDomainEnumerationReplyAddDomainDefault) def = "(default)";
    printf("Got a reply from the server %d %s %d %s\n", resultType, replyDomain, flags, def);
}

void resolve_reply(struct sockaddr *interface, struct sockaddr *address, char *txtRecord, int flags, void *context)
{
    printf("Got a reply from the server %s, %d\n", txtRecord, flags);
    //printf("interface length = %d, port = %d, family = %d, address = %s\n", ((struct sockaddr_in *)interface)->sin_len, ((struct sockaddr_in *)interface)->sin_port, ((struct sockaddr_in *)interface)->sin_family, inet_ntoa(((struct in_addr)((struct sockaddr_in *)interface)->sin_addr)));
    //printf("address length = %d, port = %d, family = %d, address = %s\n", ((struct sockaddr_in *)address)->sin_len, ((struct sockaddr_in *)address)->sin_port, ((struct sockaddr_in *)address)->sin_family, inet_ntoa(((struct in_addr)((struct sockaddr_in *)address)->sin_addr)));
}

int
main(int argc, char ** argv)
{
    CFMachPortRef           cfMachPort;
    CFMachPortContext       context;
    Boolean                 shouldFreeInfo;

    char            ch;
    mach_port_t port;
    dns_service_discovery_ref 	dns_client;

    context.version                 = 1;
    context.info                    = 0;
    context.retain                  = NULL;
    context.release                 = NULL;
    context.copyDescription 	    = NULL;
    
    if (argc < 2) goto Fail;		// Minimum command line is the command name and one argument

    while ((ch = getopt(argc, (char * const *)argv, "ERFBL")) != -1)
    {
        CFRunLoopSourceRef	rls;

        switch (ch) {

            case 'E':	dns_client =  DNSServiceDomainEnumerationCreate(1, enum_reply, nil); break;

            case 'R':	{
						char 	*demux = "";
						Opaque16	registerPort;
						UInt16 portasnum;
						if (argc < optind+4) goto Fail;
						if (argc > optind+4) demux = argv[optind+4];
						printf("Registering Service %s.%s%s port %s %s\n", argv[optind+0], argv[optind+1], argv[optind+2], argv[optind+3], demux);
						portasnum = atoi(argv[optind+3]);
						registerPort.b[0] = portasnum >> 8;
						registerPort.b[1] = portasnum & 0xFF;
						dns_client = DNSServiceRegistrationCreate(argv[optind+0], argv[optind+1], argv[optind+2], registerPort, demux, reg_reply, nil);
						break;
						}

            case 'F':	dns_client =  DNSServiceDomainEnumerationCreate(0, enum_reply, nil); break;

            case 'B':	if (argc < optind+2) goto Fail;
						printf("Browsing for %s%s\n", argv[optind+0], argv[optind+1]);
						dns_client = DNSServiceBrowserCreate(argv[optind+0], argv[optind+1], browse_reply, nil);
						break;

            case 'L':	if (argc < optind+3) goto Fail;
						printf("Lookup %s.%s%s\n", argv[optind+0], argv[optind+1], argv[optind+2]);
						dns_client = DNSServiceResolverResolve(argv[optind+0], argv[optind+1], argv[optind+2], resolve_reply, nil);
						break;

            default:
                goto Exit;
        }
        port = DNSServiceDiscoveryMachPort(dns_client);

        if (port) {

            printf("port is %d\n", port);

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
	fprintf(stderr, "%s -E                                       (Enumerate recommended registration domains) \n", argv[0]);
	fprintf(stderr, "%s -R <Name> <Type> <Domain> <Port> <Demux> (Register a Service)\n", argv[0]);
	fprintf(stderr, "%s -F                                       (Enumerate recommended browsing domains)\n", argv[0]);
	fprintf(stderr, "%s -B        <Type> <Domain>                (Browse for Services Instances -- not yet implemented)\n", argv[0]);
	fprintf(stderr, "%s -L <Name> <Type> <Domain>                (Look Up a Service Instance)\n", argv[0]);
    return 0;
}

