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

#ifndef __DNS_SERVICE_DISCOVERY_H
#define __DNS_SERVICE_DISCOVERY_H

#include <mach/mach_types.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

typedef union { unsigned char b[2]; u_int16_t NotAnInteger; } Opaque16;

/* Opaque internal data type */
typedef struct _dns_service_discovery_t * dns_service_discovery_ref;

/* possible reply flags values */

enum {
    kDNSServiceDiscoveryNoFlags			= 0,
    kDNSServiceDiscoveryMoreRepliesImmediately	= 1 << 0,
};


/* possible error code values */
enum
{
    kDNSServiceDiscoveryWaiting     = 1,
    kDNSServiceDiscoveryNoError     = 0,
      // mDNS Error codes are in the range
      // FFFE FF00 (-65792) to FFFE FFFF (-65537)
    kDNSServiceDiscoveryUnknownErr        = -65537,       // 0xFFFE FFFF
    kDNSServiceDiscoveryNoSuchNameErr     = -65538,
    kDNSServiceDiscoveryNoMemoryErr       = -65539,
    kDNSServiceDiscoveryBadParamErr       = -65540,
    kDNSServiceDiscoveryBadReferenceErr   = -65541,
    kDNSServiceDiscoveryBadStateErr       = -65542,
    kDNSServiceDiscoveryBadFlagsErr       = -65543,
    kDNSServiceDiscoveryUnsupportedErr    = -65544,
    kDNSServiceDiscoveryNotInitializedErr = -65545,
    kDNSServiceDiscoveryNoCache           = -65546,
    kDNSServiceDiscoveryAlreadyRegistered = -65547,
    kDNSServiceDiscoveryNameConflict      = -65548,
    kDNSServiceDiscoveryInvalid           = -65549,
    kDNSServiceDiscoveryMemFree           = -65792        // 0xFFFE FF00
};

/*!
@function DNSServiceResolver_handleReply
 @description This function should be called with the Mach message sent
 to the port returned by the call to DNSServiceResolverResolve.
 The reply message will be interpreted and will result in a
 call to the specified callout function.
 @param replyMsg The Mach message.
 */
void DNSServiceDiscovery_handleReply(void *replyMsg);

/* Service Registration */

typedef void (*DNSServiceRegistrationReply) (
    int 		errorCode,
    void		*context
);

/*!
@function DNSServiceRegistrationCreate
    @description Register a named service with DNS Service Discovery
    @param name The name of this service instance (e.g. "Steve's Printer")
    @param regtype The service type (e.g. "_printer._tcp." -- see
        RFC 2782 (DNS SRV) and <http://www.iana.org/assignments/port-numbers>)
    @param domain The domain in which to register the service (e.g. "apple.com.")
    @param port The local port on which this service is being offered
    @param txtRecord Optional protocol-specific additional information
    @param callBack The DNSServiceRegistrationReply function to be called
    @param context A user specified context which will be passed to the callout function.
    @result A dns_registration_t
*/
dns_service_discovery_ref DNSServiceRegistrationCreate
(
    char 		*name,
    char 		*regtype,
    char 		*domain,
    Opaque16		port,
    char 		*txtRecord,
    DNSServiceRegistrationReply callBack,
    void		*context
);

/***************************************************************************/
/*   DNS Domain Enumeration   */

typedef enum
{
    DNSServiceDomainEnumerationReplyAddDomain,			// Domain found
    DNSServiceDomainEnumerationReplyAddDomainDefault,		// Domain found (and should be selected by default)
    DNSServiceDomainEnumerationReplyRemoveDomain,			// Domain has been removed from network
} DNSServiceDomainEnumerationReplyResultType;

typedef void (*DNSServiceDomainEnumerationReply) (
    int 		resultType,		// One of DNSServiceDomainEnumerationReplyResultType
    char  		*replyDomain,
    int 		flags,			// DNS Service Discovery reply flags information
    void		*context		
);

/*!
    @function DNSServiceDomainEnumerationCreate
    @description Asynchronously create a DNS Domain Enumerator
    @param registrationDomains A boolean indicating whether you are looking
        for recommended registration domains
        (e.g. equivalent to the AppleTalk zone list in the AppleTalk Control Panel)
        or recommended browsing domains
        (e.g. equivalent to the AppleTalk zone list in the Chooser).
    @param callBack The function to be called when domains are found or removed
    @param context A user specified context which will be passed to the callout function.
    @result A dns_registration_t
*/
dns_service_discovery_ref DNSServiceDomainEnumerationCreate
(
    int 		registrationDomains,
    DNSServiceDomainEnumerationReply	callBack,
    void		*context
);

/***************************************************************************/
/*   DNS Service Browser   */

typedef enum
{
    DNSServiceBrowserReplyAddInstance,	// Instance of service found
    DNSServiceBrowserReplyRemoveInstance	// Instance has been removed from network
} DNSServiceBrowserReplyResultType;

typedef void (*DNSServiceBrowserReply) (
    int 		resultType,		// One of DNSServiceBrowserReplyResultType
    char  		*replyName,
    char  		*replyType,
    char  		*replyDomain,
    int 		flags,			// DNS Service Discovery reply flags information
    void		*context
);

/*!
    @function DNSServiceBrowserCreate
    @description Asynchronously create a DNS Service browser
    @param regtype The type of service
    @param domain The domain in which to find the service
    @param callBack The function to be called when service instances are found or removed
    @param context A user specified context which will be passed to the callout function.
    @result A dns_registration_t
*/
dns_service_discovery_ref DNSServiceBrowserCreate
(
    char 		*regtype,
    char 		*domain,
    DNSServiceBrowserReply	callBack,
    void		*context
);

/***************************************************************************/
/* Resolver requests */

typedef void (*DNSServiceResolverReply) (
    struct sockaddr 	*interface,		// Needed for scoped addresses like link-local
    struct sockaddr 	*address,
    char 		*txtRecord,
    int 		flags,			// DNS Service Discovery reply flags information
    void		*context
);

/*!
@function DNSServiceResolverResolve
    @description Resolved a named instance of a service to its address, port, and
        (optionally) other demultiplexing information contained in the TXT record.
    @param name The name of the service instance
    @param regtype The type of service
    @param domain The domain in which to find the service
    @param callBack The DNSServiceResolverReply function to be called when the specified
        address has been resolved.
    @param context A user specified context which will be passed to the callout function.
    @result A dns_registration_t
*/

dns_service_discovery_ref DNSServiceResolverResolve
(
    char 		*name,
    char 		*regtype,
    char 		*domain,
    DNSServiceResolverReply callBack,
    void		*context
);

/***************************************************************************/
/* Mach port accessor and deallocation */

/*!
    @function DNSServiceDiscoveryMachPort
    @description Returns the mach port for a dns_service_discovery_ref
    @param registration A dns_service_discovery_ref as returned from DNSServiceRegistrationCreate
    @result A mach reply port which will be sent messages as appropriate.
        These messages should be passed to the DNSServiceDiscovery_handleReply
        function.  A NULL value indicates that no address was
        specified or some other error occurred which prevented the
        resolution from being started.
*/
mach_port_t DNSServiceDiscoveryMachPort(dns_service_discovery_ref dnsServiceDiscovery);

/*!
    @function DNSServiceDiscoveryDeallocate
    @description Deallocates the DNS Service Discovery type / closes the connection to the server
    @param dnsServiceDiscovery A dns_service_discovery_ref as returned from a creation or enumeration call
    @result void
*/
void DNSServiceDiscoveryDeallocate(dns_service_discovery_ref dnsServiceDiscovery);

#endif	/* __DNS_SERVICE_DISCOVERY_H */
