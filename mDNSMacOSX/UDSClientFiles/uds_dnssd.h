/*
 * Copyright (c) 2003 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 *
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 1.2 (the
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


/* NOTE: This is a preliminary release of the Unix domain sockets based
 * Rendezvous APIs, and is subject to change without notice.
 */

#ifndef UDS_DNSSD_H
#define UDS_DNSSD_H

#include <sys/types.h>
#include <sys/socket.h>
#include <CoreFoundation/CoreFoundation.h>

#include <netinet/in.h>

typedef struct _DNSServiceDiscoveryRef_t *DNSServiceDiscoveryRef;
typedef struct _DNSRecordReference_t *DNSRecordReference;

// General flags used in functions defined below.

enum
    {
    kDNSServiceDiscoveryFlagsFinished    = 0,
    kDNSServiceDiscoveryFlagsMoreComing  = 1,

    kDNSServiceDiscoveryFlagsRemove      = 0,
    kDNSServiceDiscoveryFlagsAdd         = 2,
    kDNSServiceDiscoveryFlagsDefault     = 4,
    // "Default" is only valid in conjuction with "Add" 

    kDNSServiceDiscoveryFlagsShared      = 0,
    kDNSServiceDiscoveryFlagsUnique      = 8

    };


enum
{
    kDNSServiceDiscoveryNoErr            = 0,
    kDNSServiceDiscoveryNameConflictErr  = 1,
    kDNSServiceDiscoveryUnknownErr       = 2
};


typedef u_int32_t DNSServiceDiscoveryFlags;
typedef u_int32_t DNSServiceReplyErrorType;


//----------

//Improved API call:

typedef void (*DNSServiceDomainEnumerationReply)
    (
    const DNSServiceDiscoveryRef        DNSServiceRef,
    const DNSServiceDiscoveryFlags      flags,
    const u_int32_t                      interfaceIndex,
    const DNSServiceReplyErrorType      errorCode,
    const char                         *replyDomain,
    void                               *context
    );

int DNSServiceEnumerateDomains
    (
    DNSServiceDiscoveryRef             	*DNSServiceRef,
    const DNSServiceDiscoveryFlags      flags,
    const u_int32_t                      interfaceIndex,
    const u_int32_t                      registrationDomains,
    const DNSServiceDomainEnumerationReply       callBack,
    void                               *context
    );

//Pass "0" for registrationDomains to get the list of domains recommended 
//for browsing, or "1" for the list of domains recommended for registration.

//----------


/*
Improved API call:

DNSServiceRegister allows the client to (optionally) specify an 
interfaceIndex, for cases where the client only wants to advertise its 
service on one particular interface. (For example, Xserve RAID uses 
interface-specific registrations, to advertise internal services that are 
only accessible to other modules on the internal bus. This change will 
bring OS X up to parity with what is already available in Rendezvous for 
Linux, Rendezvous for VxWorks, etc.)

DNSServiceRegister takes standard-format TXT data (instead of the 
previous hack using ASCII 1 as the string delimiter).

You can also register a placeholder service name. If you call 
DNSServiceRegister() with a port number of zero, then your service will 
not show up when people browse, but anyone else trying to register a 
service with that name will get a name conflict. For example, suppose you 
offer LPR printing service under a certain name, and you don't want 
another device to offer IPP printing service under the same name, because 
that would be confusing to users. You can register a real LPR service, 
and an IPP placeholder, which won't show up to people browsing for IPP 
printers, but *will* prevent an IPP printing service from registering 
using that name.

The DNSServiceRegisterReply callback indicates the advertised name. In 
the case where the client passes "" for the name, this is useful to tell 
the client what name was automatically selected on its behalf. (Some 
clients want to know this so they can filter themselves out of UI lists. 
If they don't know what name was picked for them, they can't do that.)
*/


typedef void (*DNSServiceRegisterReply)
    (
    const DNSServiceDiscoveryRef        DNSServiceRef,
    const DNSServiceDiscoveryFlags      flags,
    const u_int32_t                      interfaceIndex,
    const DNSServiceReplyErrorType      errorCode,
    const char                         *name,
    const char                         *regtype,
    const char                         *domain,
    void                               *context
    );

int DNSServiceRegister
    (
    DNSServiceDiscoveryRef             *DNSServiceRef,
    const DNSServiceDiscoveryFlags      flags,
    const u_int32_t                      interfaceIndex,
    const char                         *name,
    const char                         *regtype,
    const char                         *domain,
    const u_int16_t                      port,
    const u_int16_t                      txtLen,
    const void                         *txtRecord,
    const u_int32_t                      txtTTL,
    const DNSServiceRegisterReply       callBack,
    void                               *context
    );


/*
----------

Improved API call:

DNSServiceBrowse() allows the client to (optionally) specify an 
interfaceIndex, for cases where the client only wants to browse on one 
particular interface.

The DNSServiceBrowseReply callback returns the interfaceIndex of the 
interface where the service was found. This interfaceIndex should be 
passed back when resolving a service name to targethost/port/txt.
*/


typedef void (*DNSServiceBrowseReply)
    (
    const DNSServiceDiscoveryRef        DNSServiceRef,
    const DNSServiceDiscoveryFlags      flags,
    const u_int32_t                      interfaceIndex,
    const DNSServiceReplyErrorType      errorCode,
    const char                         *replyName,
    const char                         *replyType,
    const char                         *replyDomain,
    void                               *context
    );

int DNSServiceBrowse
    (
    DNSServiceDiscoveryRef             *DNSServiceRef,
    const DNSServiceDiscoveryFlags      flags,
    const u_int32_t                      interfaceIndex,
    const char                         *regtype,
    const char                         *domain,
    const DNSServiceBrowseReply         callBack,
    void                               *context
    );
    
    
/*
----------

Improved API call:

The old DNSServiceResolverResolve call has been replaced by 
DNSServiceResolve. The old DNSServiceResolverResolve call returns IP 
address(es), port number, and TXT record(s). For a client only wanting 
one of the three, this is inefficient.

Now a client uses the rrtype parameter to indicate whether it wants to 
get the TXT record, or the SRV record (target host name and port number), 
as appropriate. If the client needs to translate from target host name to 
IP address(es) then standard calls like getaddrinfo() should be used.

Ideally, a client should have to use neither getaddrinfo() nor even 
retrieve the SRV record -- it should just pass the service name directly 
to some future connect-by-name API which handles all these details 
automatically.

The DNSServiceQueryReply callback returns all resource records in 
standard DNS format, including TXT data (instead of the previous hack 
using ASCII 1 as the string delimiter).
*/

typedef void (*DNSServiceQueryReply)
    (
    const DNSServiceDiscoveryRef        DNSServiceRef,
    const DNSServiceDiscoveryFlags      flags,
    const u_int32_t                      interfaceIndex,
    const DNSServiceReplyErrorType      errorCode,
    const char                         *name,
    const u_int16_t                      rrtype,
    const u_int16_t                      rrclass,
    const u_int16_t                      rdlen,
    const char                         *rdata,
    const u_int32_t                      ttl,
    void                               *context
    );


int DNSServiceResolve
    (
    DNSServiceDiscoveryRef             *DNSServiceRef,
    const DNSServiceDiscoveryFlags      flags,
    const u_int32_t                      interfaceIndex,
    const char                         *name,
    const char                         *regtype,
    const char                         *domain,
    const u_int16_t                      rrtype,
    const DNSServiceQueryReply          callBack,
    void                               *context
    );

/* deallocate a DNSServiceDiscoveryRef, terminating the connection with the client */

int DNSServiceDiscoveryRefDeallocate(DNSServiceDiscoveryRef DNSServiceRef);




/*
----------

New API calls:

Most clients will not need these, but they are provided for clients with 
special needs not met by the standard API.

DNSServiceConnect() creates a connection to the daemon to allow 
registration of individual records.

This call exists because it is expected that the (rare) clients that use 
this API may want to register a very large number of records, and using a 
Mach port per record would be inefficient.

DNSServiceRegisterRecord() uses a connection reference returned from 
DNSServiceConnect() and registers a single DNS record. Name conflicts, if 
any, or other failures, are reported by calls back to the 
DNSServiceRegisterRecordReply() function.

To remove a registered record, use DNSServiceRegistrationRemoveRecord() 
in the usual way. To do a wholesale removal of all records associated 
with this DNSServiceConnect(), use DNSServiceDiscoveryRefDeallocate() in the 
usual way.
*/


int DNSServiceConnect(DNSServiceDiscoveryRef *DNSServiceRef);

typedef void (*DNSServiceRegisterRecordReply)
    (
    const DNSServiceDiscoveryRef        DNSServiceRef,
    const DNSRecordReference            reference,
    const DNSServiceDiscoveryFlags      flags,
    const u_int32_t                     interfaceIndex,
    const DNSServiceReplyErrorType      errorCode,
    void                               *context
    );

int DNSServiceRegisterRecord
    (
    const DNSServiceDiscoveryRef        DNSServiceRef,
    DNSRecordReference                 *DNSRecordRef,
    const DNSServiceDiscoveryFlags      flags,
    const u_int32_t                      interfaceIndex,
    const char                         *name,
    const u_int16_t                      rrtype,
    const u_int16_t                      rrclass,
    const u_int16_t                      rdlen,
    const char                         *rdata,
    const u_int32_t                      ttl,
    const DNSServiceRegisterRecordReply  callBack,
    void                               *context
    );

int DNSServiceRemoveRecord
    (
    const DNSServiceDiscoveryRef        DNSServiceRef,
    const DNSRecordReference            DNSRecordRef,
    const DNSServiceDiscoveryFlags      flags
    );


/*
----------

New API call:

DNSServiceQuery() queries for an arbitrary mDNS record. Most clients will 
not need this, but it is provided for clients with special needs not met 
by the standard API.

It is expected that lookupd should eventually use this API for "dot 
local" names, to benefit from mDNSResponder's distributed caching of 
multicast records.

*/

int DNSServiceQuery
    (
    DNSServiceDiscoveryRef       *DNSServiceRef,
    const DNSServiceDiscoveryFlags      flags,
    const u_int32_t                      interfaceIndex,
    const char                   *name,
    const u_int16_t                      rrtype,
    const u_int16_t                      rrclass,
    const DNSServiceQueryReply          callBack,
    void                               *context
    );

/*

----------

New API call:

DNSServiceReconfirmRecord() is used to tell mDNSResponder when the result 
returned from one of its calls appears to be bad.

The mDNSResponder daemon uses aggressive caching to reduce network 
traffic. The necessary consequence of this is that sometimes data in its 
cache may be out-of-date. For example, mDNSResponder may cache the IP 
address corresponding with a given "dot local" host name, but if that 
host has crashed or been disconnected, then when the client attempts to 
open a TCP connection to that IP address, it will fail. When this 
happens, the client may use the DNSServiceReconfirmRecord() API to give a 
hint to mDNSResponder that the data may be stale. When mDNSResponder gets 
this hint, it will re-query for the data, and if no answer is received 
within a few seconds, the data will be deleted from the cache. By virtue 
of the distributed cache management protocol, all other hosts on the 
network will also delete the stale data from their caches too.

The required parameters to this call constitute the necessary data to 
locate the questionable cache data.

*/

void DNSServiceReconfirmRecord
    (
     const DNSServiceDiscoveryFlags      flags,
    const u_int32_t                      interfaceIndex,
    const char                         *name,
    const u_int16_t                      rrtype,
    const u_int16_t                      rrclass,
    const u_int16_t                      rdlen,
    const char                         *rdata
    );

/*
----------

New API calls:

Little helper routine to make a CFRunLoopSourceRef in a single simple 
call.
*/



CFRunLoopSourceRef DNSServiceCreateRunLoopSource
    (
    CFAllocatorRef                      allocator,
    DNSServiceDiscoveryRef              DNSServiceRef,
    CFIndex                             order
    );

int DNSServiceGetReadFDSet
    (
    DNSServiceDiscoveryRef              DNSServiceRef,
    int                                *nfds,
    fd_set                             *readfds
    );

int DNSServiceProcessFDSet
     (
      int                                 nfds,
      fd_set                             *readfds
      );

#endif  // UDS_DSNSD_H
