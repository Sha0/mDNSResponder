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
#include <netdb.h>

#include <DNSServiceDiscovery/DNSServiceDiscovery.h>

#include "DNSServiceDiscovery/DNSServiceDiscoveryDefines.h"

#include "DNSServiceDiscoveryRequestServer.h"
#include "DNSServiceDiscoveryReply.h"

#include "mDNSEnvironment.h"
#include "mDNSClientAPI.h"				// Defines the interface to the client layer above

// Globals
static mDNS m;
static mDNS_PlatformSupport p;
#define RR_CACHE_SIZE 500
static ResourceRecord rrcachestorage[RR_CACHE_SIZE];

static int debug_mode = 0;

local void FoundService(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, void *context)
	{
	#pragma unused (question)
	mach_port_t client = (mach_port_t)context;
	int resultType = DNSServiceBrowserReplyAddInstance;

	domainlabel name;
	domainname type, domain;
	char c_name[256], c_type[256], c_dom[256];
	
	if (answer->rrtype != kDNSType_PTR) return;

	debugf("FoundService %##s", answer->rdata.name.c);

	DeconstructServiceName(&answer->rdata.name, &name, &type, &domain);
	ConvertDomainLabelToCString_unescaped(&name, c_name);
	ConvertDomainNameToCString(&type, c_type);
	ConvertDomainNameToCString(&domain, c_dom);

	if (answer->rrremainingttl == 0) resultType = DNSServiceBrowserReplyRemoveInstance;

	debugf("DNSServiceBrowserReply_rpc sending reply for %s", c_name);

	DNSServiceBrowserReply_rpc(client, resultType, c_name, c_type, c_dom, 0);
	}

kern_return_t provide_DNSServiceBrowserCreate_rpc
(
	mach_port_t server,
	mach_port_t client,
	DNSCString regtype,
	DNSCString domain
 )
{
	mStatus err;
    DNSQuestion *q = malloc(sizeof(DNSQuestion));
    if (!q) { debugf("provide_DNSServiceBrowserCreate_rpc: No memory!"); return(-1); }

    if (debug_mode) {
        printf("Create a browser, %s, %s\n", regtype, domain);
    }
    err = mDNS_StartBrowse(&m, q, regtype, domain, zeroIPAddr, FoundService, (void*)client);
    if (err) { debugf("provide_DNSServiceBrowserCreate_rpc: mDNS_StartBrowse failed"); return(err); }

    // reply
    // 
    return 0;
}


kern_return_t provide_DNSServiceDomainEnumerationCreate_rpc
(
	mach_port_t server,
	mach_port_t client,
	int registrationDomains
 )
{
    if (debug_mode) {
        printf("Enumerate %s domains\n", registrationDomains ? "registration" : "browsing");
    }
    DNSServiceDomainEnumerationReply_rpc(client, DNSServiceDomainEnumerationReplyAddDomainDefault, "local.arpa.", 0);
	if (!registrationDomains)
		DNSServiceDomainEnumerationReply_rpc(client, DNSServiceDomainEnumerationReplyAddDomain, "apple.com.", 0);
    return 0;
}

kern_return_t provide_DNSServiceRegistrationCreate_rpc
(
	mach_port_t server,
	mach_port_t client,
	DNSCString name,
	DNSCString regtype,
	DNSCString domain,
	int notAnIntPort,
	DNSCString txtRecord
 )
{
    char buffer[256];
    domainlabel n;
    domainname t, d;
    IPPort port;
    ServiceRecordSet *srs = malloc(sizeof(ServiceRecordSet));
    if (!srs) { debugf("DNSServiceRegistrationRegister: No memory!"); return(-1); }

    if (debug_mode) {

        printf("Received Service Registration from local client\n");
        printf("Name    = %s\n", name);
        printf("RegType = %s\n", regtype);
        printf("Domain  = %s\n", domain);
        printf("Port    = %d\n", notAnIntPort);
        if (txtRecord && txtRecord[0]) printf("txtRecord   = %s\n", txtRecord);
    }

    ConvertCStringToDomainLabel(name, &n);
    ConvertCStringToDomainName(regtype, &t);
    ConvertCStringToDomainName(domain, &d);
    port.NotAnInteger = notAnIntPort;
    mDNS_RegisterService(&m, srs, port, txtRecord, &n, &t, &d);
    ConvertDomainNameToCString_unescaped(&srs->RR_SRV.name, buffer);
    if (debug_mode) {
        printf("Made Service Record Set for %s\n", buffer);
    }

	// For now, return immediate success to client
    DNSServiceRegistrationReply_rpc(client, 0);
    return 0;

}

kern_return_t provide_DNSServiceResolverResolve_rpc
(
	mach_port_t server,
	mach_port_t client,
	DNSCString name,
	DNSCString regtype,
	DNSCString domain
 )
{
    struct sockaddr_in	return_address;
    struct sockaddr_in	return_interface;

    char *buffer1;
    char *buffer2;

    struct hostent *h;

    if (debug_mode) {
        printf("Resolve a request, %s, %s, %s\n", name, regtype, domain);
    }

    h = gethostbyname("www.apple.com");
    return_address.sin_family = h->h_addrtype;
    memcpy((char *)&return_address.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
    return_address.sin_port = htons(80);

    h = gethostbyname("www.epicware.com");
    return_interface.sin_family = h->h_addrtype;
    memcpy((char *)&return_interface.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
    return_interface.sin_port = htons(2000);

    buffer1 = malloc(return_address.sin_len);
    buffer2 = malloc(return_interface.sin_len);

    bzero(buffer1, return_address.sin_len);
    bzero(buffer2, return_interface.sin_len);

    bcopy(&return_address, buffer1, return_address.sin_len);
    bcopy(&return_interface, buffer2, return_interface.sin_len);

    // reply
    // DNSServiceResolverReply_rpc(client, buffer1, buffer2, "Resolver data for apple.com, epicware.com", 0);
    return 0;
}

void
dnsserverCallback(CFMachPortRef port, void *msg, CFIndex size, void *info)
{
    mig_reply_error_t		*request = msg;
    mig_reply_error_t		*reply;
    mach_msg_return_t		mr;
    int				options;

    /* allocate a reply buffer */
    reply = CFAllocatorAllocate(NULL, provide_DNSServiceDiscoveryRequest_subsystem.maxsize, 0);

    /* call the MiG server routine */
    (void) DNSServiceDiscoveryRequest_server(&request->Head, &reply->Head);

    if (!(reply->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) && (reply->RetCode != KERN_SUCCESS)) {

        if (reply->RetCode == MIG_NO_REPLY) {
            /*
             * This return code is a little tricky -- it appears that the
             * demux routine found an error of some sort, but since that
             * error would not normally get returned either to the local
             * user or the remote one, we pretend it's ok.
             */
            CFAllocatorDeallocate(NULL, reply);
            return;
        }

        /*
         * destroy any out-of-line data in the request buffer but don't destroy
         * the reply port right (since we need that to send an error message).
         */
        request->Head.msgh_remote_port = MACH_PORT_NULL;
        mach_msg_destroy(&request->Head);
    }

    if (reply->Head.msgh_remote_port == MACH_PORT_NULL) {
        /* no reply port, so destroy the reply */
        if (reply->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) {
            mach_msg_destroy(&reply->Head);
        }
        CFAllocatorDeallocate(NULL, reply);
        return;
    }

    /*
     * send reply.
     *
     * We don't want to block indefinitely because the client
     * isn't receiving messages from the reply port.
     * If we have a send-once right for the reply port, then
     * this isn't a concern because the send won't block.
     * If we have a send right, we need to use MACH_SEND_TIMEOUT.
     * To avoid falling off the kernel's fast RPC path unnecessarily,
     * we only supply MACH_SEND_TIMEOUT when absolutely necessary.
     */

    options = MACH_SEND_MSG;
    if (MACH_MSGH_BITS_REMOTE(reply->Head.msgh_bits) == MACH_MSG_TYPE_MOVE_SEND_ONCE) {
        options |= MACH_SEND_TIMEOUT;
    }
    mr = mach_msg(&reply->Head,		/* msg */
		      options,			/* option */
		      reply->Head.msgh_size,	/* send_size */
		      0,			/* rcv_size */
		      MACH_PORT_NULL,		/* rcv_name */
		      MACH_MSG_TIMEOUT_NONE,	/* timeout */
		      MACH_PORT_NULL);		/* notify */


    /* Has a message error occurred? */
    switch (mr) {
        case MACH_SEND_INVALID_DEST:
        case MACH_SEND_TIMED_OUT:
            /* the reply can't be delivered, so destroy it */
            mach_msg_destroy(&reply->Head);
            break;

        default :
            /* Includes success case.  */
            break;
    }

    CFAllocatorDeallocate(NULL, reply);
}

void
start(const char *bundleName, const char *bundleDir)
{
    kern_return_t 		status = KERN_SUCCESS;
    mach_port_t			host_priv;
    CFMachPortRef		server_port;
    CFRunLoopSourceRef	rls;

	mStatus err = mDNS_Init(&m, &p, rrcachestorage, RR_CACHE_SIZE, NULL);
	if (err) { debugf("Daemon start: mDNS_Init failed"); return; }

    /* Getting host_priv port */
    host_priv = mach_host_self();

    if (status != KERN_SUCCESS) {
        return;
    }

    /* Create the primary / new connection port */
    server_port = CFMachPortCreate(NULL, dnsserverCallback, NULL, NULL);

    /* Create and add a run loop source for the port */
    rls = CFMachPortCreateRunLoopSource(NULL, server_port, 0);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), rls, kCFRunLoopDefaultMode);
    CFRelease(rls);

    // Register the server port with the bootstrap server so clients can find it

    status = bootstrap_register(bootstrap_port, DNS_SERVICE_DISCOVERY_SERVER, CFMachPortGetPort(server_port));
    if (status != KERN_SUCCESS)
    {
        return;
    }

    (void)mach_port_deallocate(mach_task_self(), host_priv);

    switch (status) {
        case KERN_SUCCESS :
            if (debug_mode) {
                printf("service registered with port %d\n", CFMachPortGetPort(server_port));
            }

            /* service not currently registered, "a good thing" (tm) */
            break;
        default :
            printf("bootstrap_register failed(): %s", mach_error_string(status));
            return;
    }

    return;
}

int
main(int argc, char ** argv)
{
    if (argc > 1) {
        // command line arguments
        if (!strcmp(argv[1], "-d")) {
            debug_mode = 1;
        }
    }

    if (!debug_mode) {
        daemon(0,0);
    }

    start(NULL, NULL);
    CFRunLoopRun();
    /* not reached */
    exit(0);
    return 0;
}
