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

#ifndef _DNS_SD_H
#define _DNS_SD_H

#include <sys/types.h>
#include <sys/socket.h>
#include <inttypes.h>
#include <netinet/in.h>


/* DNSServiceDiscoveryRef, DNSRecordRef
 * 
 * Opaque internal data types.  
 * Note: client is responsible for serializing access to these structures if 
 * they are shared between concurrent threads.
 */

typedef struct _DNSServiceDiscoveryRef_t *DNSServiceDiscoveryRef;
typedef struct _DNSRecordRef_t *DNSRecordRef;

/* General flags used in functions defined below */
enum
    {
    kDNSServiceDiscoveryFlagsFinished    = 0,
    kDNSServiceDiscoveryFlagsMoreComing  = 1,
    /* MoreComing indicates to a Browse callback that another result is
     * queued.  Applications should defer updating their UI to display browse 
     * results until the Finished flag is set in the callback. */

    kDNSServiceDiscoveryFlagsRemove      = 0,
    kDNSServiceDiscoveryFlagsAdd         = 2,
    kDNSServiceDiscoveryFlagsDefault     = 4,
    /* Flags for domain enumeration reply callbacks.
     * "Default" is only valid in conjuction with "Add" */

    kDNSServiceDiscoveryFlagsShared      = 0,
    kDNSServiceDiscoveryFlagsUnique      = 8,
    
    kDNSServiceDiscoverFlagsRenameOnConflict    = 0,
    kDNSServiceDiscoveryFlagsNoRenameOnConflict = 16
    /* Flags for specifying renaming behavior on name conflict when registering 
     * non-shared records. NoAutorename is only valid if a name is explicitly 
     * specified when registering a service (ie the default name is not used.)
     */
    };

/* possible error code values */
enum
    {
    kDNSServiceDiscoveryErr_NoError	   	= 0,
    kDNSServiceDiscoveryErr_Unknown        	= -65537,	/* 0xFFFE FFFF */
    kDNSServiceDiscoveryErr_NoSuchName     	= -65538,
    kDNSServiceDiscoveryErr_NoMemory       	= -65539,
    kDNSServiceDiscoveryErr_BadParam       	= -65540,
    kDNSServiceDiscoveryErr_BadReference   	= -65541,
    kDNSServiceDiscoveryErr_BadState       	= -65542,
    kDNSServiceDiscoveryErr_BadFlags       	= -65543,
    kDNSServiceDiscoveryErr_Unsupported    	= -65544,
    kDNSServiceDiscoveryErr_NotInitialized 	= -65545,
    kDNSServiceDiscoveryErr_AlreadyRegistered	= -65547,
    kDNSServiceDiscoveryErr_NameConflict      	= -65548,
    kDNSServiceDiscoveryErr_Invalid           	= -65549,
    kDNSServiceDiscoveryErr_Incompatible	= -65551	/* client library incompatible with daemon */
    /* mDNS Error codes are in the range 
     * FFFE FF00 (-65792) to FFFE FFFF (-65537) */
    }; 

typedef uint32_t DNSServiceDiscoveryFlags;
typedef uint32_t DNSServiceReplyErrorType;


/*********************************************************************************************
 * General Parameter/Return Type Descriptions
 * (Apply to all of the following calls, unless stated otherwise)
 *
 * value-result parameters (DNSServiceDiscoveryRef* and DNSRecordRef*) must point to
 * an uninitialized reference, and must NOT be used in subsequent calls if the call 
 * that initializes them returns a non-zero error.
 * 
 * C-String parameters (such as "name" in DNSServiceRegister()) are interpreted literally, i.e.
 * a dot (".") is a literal dot, and not a DNS label separator.  DNSNameStrings (such as 
 * "regtype" in DNSServiceRegister()) are interpreted using DNS naming conventions - that is,
 * an unescaped dot is a label separator, literal dots must be escaped with a backslash ("\."),
 * and literal backslashes must be escaped with a second backslash ("\\").
 *
 * interfaceIndex is the index returned by the if_nametoindex() family of calls 
 * that is to be searched or registered on (most applications will pass 0 to 
 * search/register on all interfaces.)  
 *
 * txtData is txt record rData in standard txt data format - there is no ASCII
 * 1 delimiter as there is in the older DNSServiceDiscovery.h API.
 *
 * callBack is the function to be called when there is a reply from the daemon.
 *
 * context is a user specified context pointer which is passed to the callback function.
 *
 * return value: DNSServiceReplyErrorType will be kDNSServiceDiscoveryErr_NoError on
 * success, and will otherwise indicate the failure that occurred.  Even if the call
 * returns successfully, the callback must check for an error from the daemon.  Arguments
 * to the callback are undefined if the error code is not  kDNSServiceDiscoveryErr_NoError.
 */


/*********************************************************************************************
 * Unix Domain Socket access, DNSServiceDiscoveryRef deallocation, and data processing functions
 */

/* DNSServiceDiscoverySockFD()
 * 
 * Access underlying Unix domain socket for an initialized DNSServiceDiscoveryRef from which 
 * the client reads a reply from the daemon.  The caller should NOT directly manipulate 
 * the socket, but should instead call DNSServiceDiscoveryProcessResult().  Socket may be checked
 * for new data in a select() loop, or may be used as a RunLoop source.   Returns -1 on error.
 */
int DNSServiceDiscoverySockFD(DNSServiceDiscoveryRef DNSServiceRef);

/* DNSServiceDiscoveryProcessResult()
 *
 * Read a reply from the daemon, calling the application callback.  Call will block if there
 * is no data on the DNSServiceRef's underlying socket, or if the data from the server is incomplete.
 */

DNSServiceReplyErrorType DNSServiceDiscoveryProcessResult(DNSServiceDiscoveryRef DNSServiceRef);

/* DNSServiceDiscoveryRefDeallocate()
 * 
 * Deallocate memory associated with a DNSServiceDiscoveryRef that is initialized by successful
 * completion of a call that takes a reference as a value-result parameter.  Deallocation terminates 
 * the connection with the daemon, aborting any operations.  If the DNSServiceRef's underlying socket is 
 * in a select() loop or is a RunLoop source, it must be removed BEFORE the reference is deallocated.
 */
 
void DNSServiceDiscoveryRefDeallocate(DNSServiceDiscoveryRef DNSServiceRef);


/*********************************************************************************************
 * Domain Enumeration, Service Registration and Discovery functions
 *
 */   

/* DNSServiceEnumerateDomains()
 * 
 * Asynchronously enumerate domains available for browsing and registration.
 * flags are currently ignored and are reserved for future use.
 * registrationDomains should be set to 0 to get the list of domains recommended 
 * for browsing, or 1 for the list of domains recommended for registration.
 */
 
typedef void (*DNSServiceDomainEnumReply)
    (
    const DNSServiceDiscoveryRef 	DNSServiceRef,
    const DNSServiceDiscoveryFlags      flags,
    const uint32_t                      interfaceIndex,
    const DNSServiceReplyErrorType      errorCode,
    const char                          *replyDomain,
    void                                *context
    );

DNSServiceReplyErrorType DNSServiceEnumerateDomains
    (
    DNSServiceDiscoveryRef          	*DNSServiceRef,
    const DNSServiceDiscoveryFlags     	flags,
    const uint32_t                    	interfaceIndex,
    const uint32_t                    	registrationDomains,
    const DNSServiceDomainEnumReply   	callBack,
    void                               	*context
    );

/* DNSServiceRegister()
 * 
 * Register a new service to be discovered by other clients via Browse/Resolve.
 * flags indicate the renaming behavior on name conflict.
 * If non-null, name specifies the service name to be registered.  Most applications will
 * not specify a name, instead using the system's default name.
 * regtype is the service type followed by the protocol (a DNS name string, e.g. "_ftp._tcp") - the protocol 
 * must either by "_tcp" or "_udp".
 * domain is the domain on which to advertise the service (a DNS name string, e.g. ".local.").  
 * port is the port on which the service accepts connections.  Pass 0 for a "placeholder" service
 * (e.g. a service that will not be discovered by browsing, but will cause a name conflict if
 * another client tries to register that same name.)
 * txtLen is the length of the txtRecord, in bytes
 * txtTTL is the resource record TTL of the text record.
 * callBack is the function to be called with the result of the service registration (indicating the
 * the name registered when no name was specified by the caller.)
 */

typedef void (*DNSServiceRegisterReply)
    (
    const DNSServiceDiscoveryRef	DNSServiceRef,
    const DNSServiceDiscoveryFlags      flags,
    const uint32_t                     	interfaceIndex,
    const DNSServiceReplyErrorType      errorCode,
    const char                          *name,
    const char                          *regtype,
    const char                          *domain,
    void                                *context
    );

DNSServiceReplyErrorType DNSServiceRegister
    (
    DNSServiceDiscoveryRef           	*DNSServiceRef,
    const DNSServiceDiscoveryFlags      flags,
    const uint32_t                     	interfaceIndex,
    const char                          *name,
    const char                          *regtype,
    const char                          *domain,
    const uint16_t                     	port,
    const uint16_t                     	txtLen,
    const void                          *txtRecord,
    const uint32_t                     	txtTTL,
    const DNSServiceRegisterReply       callBack,
    void                                *context
    );

/* DNSServiceBrowse
 *
 * Browse for instances of a given service type.
 * flags are currently ignored, and are reserved for future use.
 * regtype is the service type followed by the protocol (a DNS name string, e.g. "_ftp._tcp") - the protocol 
 * must either by "_tcp" or "_udp".
 * domain is the domain on which to browse for services (a DNS name string, e.g. ".local.").  
 * the interfaceIndex parameter passed to the DNSServiceBrowseReply callback indicates the interface on which 
 * the service was advertised,  should be passed back to DNSServiceResolve() when resolving the service
 * name to a target or txt data.
 */

typedef void (*DNSServiceBrowseReply)
    (
    const DNSServiceDiscoveryRef     	DNSServiceRef,
    const DNSServiceDiscoveryFlags      flags,
    const uint32_t                     	interfaceIndex,
    const DNSServiceReplyErrorType      errorCode,
    const char                          *replyName,
    const char                         	*replyType,
    const char                         	*replyDomain,
    void                               	*context
    );

DNSServiceReplyErrorType DNSServiceBrowse
    (
    DNSServiceDiscoveryRef           	*DNSServiceRef,
    const DNSServiceDiscoveryFlags      flags,
    const uint32_t                     	interfaceIndex,
    const char                         	*regtype,
    const char                         	*domain,
    const DNSServiceBrowseReply         callBack,
    void                               	*context
    );
    
/* DNSServiceResolve()
 *
 * Resolve a service name discovered via a Browse() call to a target or txt data.
 * flags are currently ignored, and are reserved for future use.
 * name is the name of the service (a literal C-string).
 * regtype is the service type followed by a protocol (a DNS name string, e.g. "_ftp._tcp") 
 * domain is the domain on which the service was advertised (a DNS name string, e.g. ".local.").  
 * rrtype specifies whether the name is to be resolved to an SRV record, or a TXT record
 * (numerical constants for these types are defined in nameser.h)
 * 
 * callback returns all resource records in standard DNS format, including TXT data (ASCII 1 
 * string delimiters are not used.)
 */

typedef void (*DNSServiceQueryReply)
    (
    const DNSServiceDiscoveryRef	DNSServiceRef,
    const DNSServiceDiscoveryFlags      flags,
    const uint32_t                     	interfaceIndex,
    const DNSServiceReplyErrorType      errorCode,
    const char                         	*name,
    const uint16_t                     	rrtype,
    const uint16_t                     	rrclass,
    const uint16_t                     	rdlen,
    const char                         	*rdata,
    const uint32_t                     	ttl,
    void                               	*context
    );

DNSServiceReplyErrorType DNSServiceResolve
    (
    DNSServiceDiscoveryRef           	*DNSServiceRef,
    const DNSServiceDiscoveryFlags      flags,
    const uint32_t                     	interfaceIndex,
    const char                         	*name,
    const char                         	*regtype,
    const char                         	*domain,
    const uint16_t                     	rrtype,
    const DNSServiceQueryReply          callBack,
    void                               	*context
    );

/* DNSServiceAddRecord()
 *
 * Add a record to a registered service.
 * DNSServiceRef is a DNSServiceDiscoveryRef initialized by DNSServiceRegister().
 * Upon succesfull completion of this call, DNSRecordRef may be passed to 
 * DNSServiceUpdateRecord() or DNSServiceRemoveRecord()).
 * rrtype is the resource record type, numerically defined in nameser.h.
 * rdlen, rdata, and ttl are values that are to appear literally in the resource record.
 */
 
DNSServiceReplyErrorType DNSServiceAddRecord
    (
    const DNSServiceDiscoveryRef	DNSServiceRef,
    DNSRecordRef			*RecordRef,
    const uint16_t			rrtype,
    const uint16_t			rdlen,
    const char				*rdata,
    const uint32_t			ttl
    );

/* DNSServiceUpdateRecord
 * 
 * Update a record in a registered service.
 * DNSServiceRef is a DNSServiceDiscoveryRef that is initialized by DNSServiceRegister().
 * RecordRef is a DNSRecordRef initialized by DNSServiceAddRecord, or NULL for the
 * service's primary txt record.
 * rdlen, rdata, and ttl are the (potentially new) fields that are to appear in the updated record.
 */
 
DNSServiceReplyErrorType DNSServiceUpdateRecord
    (
    const DNSServiceDiscoveryRef	DNSServiceRef,
    DNSRecordRef			RecordRef,
    const uint16_t			rdlen,
    const char				*rdata,
    const uint32_t			ttl
    );

/* DNSServiceRemoveRecord
 *
 * Remove a record from a service record set, or deregister an individually-registered
 * record.
 * DNSServiceRef must have been initialized by DNSServiceRegister() and RecordRef by 
 * DNSServiceAddRecord() to remove a record from a service record set, 
 * or DNSServiceRef must have been initialized by DNSServiceConnect() and RecordRef by 
 * DNSRegisterRecord() to remove an individually registered record.
 * flags is unused.
 */

DNSServiceReplyErrorType DNSServiceRemoveRecord
    (
    const DNSServiceDiscoveryRef     	DNSServiceRef,
    const DNSRecordRef            	RecordRef,
    const DNSServiceDiscoveryFlags      flags
    );


/*********************************************************************************************
 * Special Purpose Calls
 */
 



/*




To remove a registered record, use DNSServiceRegistrationRemoveRecord() 
in the usual way. To do a wholesale removal of all records associated 
with this DNSServiceConnect(), use DNSServiceDiscoveryRefDeallocate() in the 
usual way.
*/

/* DNSServiceConnect()
 * 
 * Create a connection to the daemon allowing efficient registration of 
 * multiple individual records.  
 */

DNSServiceReplyErrorType DNSServiceConnect(DNSServiceDiscoveryRef *DNSServiceRef);

/* DNSServiceRegisterRecord
 *
 * Register an individual resource record.
 * DNSServiceReference is a DNSServiceDiscoveryRef initialized by DNSServiceConnect().
 * RecordRef is a pointer to an uninitialized DNSRecordRef (may be passed to 
 * DNSServiceUpdateRecord() or DNSServiceRemoveRecord following successful completion.)
 * flags specifies whether the resource record is to be shared or unique.
 * name is the record's full domain name (a DNS name string).
 * rrtype, rrclass, rdlen, rdata, and ttl are all the literal values that are to be
 * in the resource record.
 * Name conflicts and any other asynchronous registration failures are reported to the callback.
 * Successfully registered records can be removed individually by DNSServiceRemoveRecord(), or 
 * all records registered on a connection can be removed by deallocating the connected 
 * DNSServiceDiscoveryRef (i.e. via DNSServiceDiscoveryRefDeallcate).
 */
 
typedef void (*DNSServiceRegisterRecordReply)
    (
    const DNSServiceDiscoveryRef     	DNSServiceRef,
    const DNSRecordRef            	RecordRef,
    const DNSServiceDiscoveryFlags      flags,
    const uint32_t                     	interfaceIndex,
    const DNSServiceReplyErrorType      errorCode,
    void                               	*context
    );

DNSServiceReplyErrorType DNSServiceRegisterRecord
    (
    const DNSServiceDiscoveryRef     	DNSServiceRef,
    DNSRecordRef                 	*RecordRef,
    const DNSServiceDiscoveryFlags      flags,
    const uint32_t                     	interfaceIndex,
    const char                         	*name,
    const uint16_t                     	rrtype,
    const uint16_t                     	rrclass,
    const uint16_t                     	rdlen,
    const char                         	*rdata,
    const uint32_t                     	ttl,
    const DNSServiceRegisterRecordReply callBack,
    void                               	*context
    );

/* DNSServiceQuery
 * 
 * Query for an arbitrary DNS record.
 * flags are unused.
 * name is the full service name to be queried for (a DNS name string)
 * rrtype and rrclass are the literal types to be queried for (numerical values
 * are defined in nameser.h)
 */
 
DNSServiceReplyErrorType DNSServiceQuery
    (
    DNSServiceDiscoveryRef       	*DNSServiceRef,
    const DNSServiceDiscoveryFlags      flags,
    const uint32_t                     	interfaceIndex,
    const char                   	*name,
    const uint16_t                     	rrtype,
    const uint16_t                     	rrclass,
    const DNSServiceQueryReply          callBack,
    void                               	*context
    );

/* DNSServiceDiscoveryReconfirmRecord
 * 
 * Instruct the daemon to verify the validity of a resource record.
 * name, rrtype, rrclass, rdlen, and rdata are values returned by the
 * daemon via a callback that appear to be out of date (e.g. because tcp 
 * connection to a service's target failed.)  Causes the record to be 
 * flushed from the daemon's cache (as well as all other daemons' caches
 * on the network) if the record is no longer valid.
 */
 
void DNSServiceReconfirmRecord
    (
    const DNSServiceDiscoveryFlags     flags,
    const uint32_t                     interfaceIndex,
    const char                         *name,
    const uint16_t                     rrtype,
    const uint16_t                     rrclass,
    const uint16_t                     rdlen,
    const char                         *rdata
    );


#endif  // _DNS_SD_H
