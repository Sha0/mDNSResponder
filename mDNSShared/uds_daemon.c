/* -*- Mode: C; tab-width: 4 -*-
 *
 * Copyright (c) 2003-2006 Apple Computer, Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.

    Change History (most recent first):

$Log: uds_daemon.c,v $
Revision 1.233  2007/01/10 20:49:37  cheshire
Remove unnecessary setting of q->Private fields

Revision 1.232  2007/01/09 00:03:23  cheshire
Call udsserver_handle_configchange() once at the end of udsserver_init()
to set up the automatic registration and browsing domains.

Revision 1.231  2007/01/06 02:50:19  cheshire
<rdar://problem/4632919> Instead of copying SRV and TXT record data, just store pointers to cache entities

Revision 1.230  2007/01/06 01:00:35  cheshire
Improved SIGINFO output

Revision 1.229  2007/01/05 08:30:56  cheshire
Trim excessive "$Log" checkin history from before 2006
(checkin history still available via "cvs log ..." of course)

Revision 1.228  2007/01/05 08:09:05  cheshire
Reorder code into functional sections, with "#pragma mark" headers

Revision 1.227  2007/01/05 07:04:24  cheshire
Minor code tidying

Revision 1.226  2007/01/05 05:44:35  cheshire
Move automatic browse/registration management from uDNS.c to mDNSShared/uds_daemon.c,
so that mDNSPosix embedded clients will compile again

Revision 1.225  2007/01/04 23:11:15  cheshire
<rdar://problem/4720673> uDNS: Need to start caching unicast records
When an automatic browsing domain is removed, generate appropriate "remove" events for legacy queries

Revision 1.224  2007/01/04 20:57:49  cheshire
Rename ReturnCNAME to ReturnIntermed (for ReturnIntermediates)

Revision 1.223  2006/12/21 01:25:49  cheshire
Tidy up SIGINFO state log

Revision 1.222  2006/12/21 00:15:22  cheshire
Get rid of gmDNS macro; fixed a crash in udsserver_info()

Revision 1.221  2006/12/20 04:07:38  cheshire
Remove uDNS_info substructure from AuthRecord_struct

Revision 1.220  2006/12/19 22:49:25  cheshire
Remove uDNS_info substructure from ServiceRecordSet_struct

Revision 1.219  2006/12/14 03:02:38  cheshire
<rdar://problem/4838433> Tools: dns-sd -G 0 only returns IPv6 when you have a routable IPv6 address

Revision 1.218  2006/11/18 05:01:33  cheshire
Preliminary support for unifying the uDNS and mDNS code,
including caching of uDNS answers

Revision 1.217  2006/11/15 19:27:53  mkrochma
<rdar://problem/4838433> Tools: dns-sd -G 0 only returns IPv6 when you have a routable IPv6 address

Revision 1.216  2006/11/10 00:54:16  cheshire
<rdar://problem/4816598> Changing case of Computer Name doesn't work

Revision 1.215  2006/10/27 01:30:23  cheshire
Need explicitly to set ReturnIntermed = mDNSfalse

Revision 1.214  2006/10/20 05:37:23  herscher
Display question list information in udsserver_info()

Revision 1.213  2006/10/05 03:54:31  herscher
Remove embedded uDNS_info struct from DNSQuestion_struct

Revision 1.212  2006/09/30 01:22:35  cheshire
Put back UTF-8 curly quotes in log messages

Revision 1.211  2006/09/27 00:44:55  herscher
<rdar://problem/4249761> API: Need DNSServiceGetAddrInfo()

Revision 1.210  2006/09/26 01:52:41  herscher
<rdar://problem/4245016> NAT Port Mapping API (for both NAT-PMP and UPnP Gateway Protocol)

Revision 1.209  2006/09/21 21:34:09  cheshire
<rdar://problem/4100000> Allow empty string name when using kDNSServiceFlagsNoAutoRename

Revision 1.208  2006/09/21 21:28:24  cheshire
Code cleanup to make it consistent with daemon.c: change rename_on_memfree to renameonmemfree

Revision 1.207  2006/09/15 21:20:16  cheshire
Remove uDNS_info substructure from mDNS_struct

Revision 1.206  2006/08/14 23:24:56  cheshire
Re-licensed mDNSResponder daemon source code under Apache License, Version 2.0

Revision 1.205  2006/07/20 22:07:30  mkrochma
<rdar://problem/4633196> Wide-area browsing is currently broken in TOT
More fixes for uninitialized variables

Revision 1.204  2006/07/15 02:01:33  cheshire
<rdar://problem/4472014> Add Private DNS client functionality to mDNSResponder
Fix broken "empty string" browsing

Revision 1.203  2006/07/07 01:09:13  cheshire
<rdar://problem/4472013> Add Private DNS server functionality to dnsextd
Only use mallocL/freeL debugging routines when building mDNSResponder, not dnsextd

Revision 1.202  2006/07/05 22:00:10  cheshire
Wide-area cleanup: Rename mDNSPlatformGetRegDomainList() to uDNS_GetDefaultRegDomainList()

Revision 1.201  2006/06/29 03:02:47  cheshire
<rdar://problem/4607042> mDNSResponder NXDOMAIN and CNAME support

Revision 1.200  2006/06/28 08:56:26  cheshire
Added "_op" to the end of the operation code enum values,
to differentiate them from the routines with the same names

Revision 1.199  2006/06/28 08:53:39  cheshire
Added (commented out) debugging messages

Revision 1.198  2006/06/27 20:16:07  cheshire
Fix code layout

Revision 1.197  2006/05/18 01:32:35  cheshire
<rdar://problem/4472706> iChat: Lost connection with Bonjour
(mDNSResponder insufficiently defensive against malformed browsing PTR responses)

Revision 1.196  2006/05/05 07:07:13  cheshire
<rdar://problem/4538206> mDNSResponder fails when UDS reads deliver partial data

Revision 1.195  2006/04/25 20:56:28  mkrochma
Added comment about previous checkin

Revision 1.194  2006/04/25 18:29:36  mkrochma
Workaround for warning: unused variable 'status' when building mDNSPosix

Revision 1.193  2006/03/19 17:14:38  cheshire
<rdar://problem/4483117> Need faster purging of stale records
read_rr_from_ipc_msg was not setting namehash and rdatahash

Revision 1.192  2006/03/18 20:58:32  cheshire
Misplaced curly brace

Revision 1.191  2006/03/10 22:19:43  cheshire
Update debugging message in resolve_result_callback() to indicate whether event is ADD or RMV

Revision 1.190  2006/03/10 21:56:12  cheshire
<rdar://problem/4111464> After record update, old record sometimes remains in cache
When service TXT and SRV record both change, clients with active resolve calls get *two* callbacks, one
when the TXT data changes, and then immediately afterwards a second callback with the new port number
This change suppresses the first unneccessary (and confusing) callback

Revision 1.189  2006/01/06 00:56:31  cheshire
<rdar://problem/4400573> Should remove PID file on exit

*/

#if defined(_WIN32)
#include <process.h>
#define dnssd_strerror(X)	win32_strerror(X)
#define usleep(X)				Sleep(((X)+999)/1000)
mDNSlocal char * win32_strerror(int inErrorCode)
	{
	static char buffer[1024];
	DWORD       n;
	memset(buffer, 0, sizeof(buffer));
	n = FormatMessageA(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			(DWORD) inErrorCode,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			buffer,
			sizeof(buffer),
			NULL);
	if (n > 0)
		{
		// Remove any trailing CR's or LF's since some messages have them.
		while((n > 0) && isspace(((unsigned char *) buffer)[n - 1]))
			buffer[--n] = '\0';
		}
	return buffer;
	}
#else
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#define dnssd_strerror(X)	strerror(X)
#endif

#include <stdlib.h>
#include <stdio.h>
#include "mDNSEmbeddedAPI.h"
#include "DNSCommon.h"
#include "uDNS.h"
#include "uds_daemon.h"
#include "dns_sd.h"
#include "dnssd_ipc.h"

// Apple specific configuration functionality, not required for other platforms
#if APPLE_OSX_mDNSResponder
#include <sys/ucred.h>
#ifndef LOCAL_PEERCRED
#define LOCAL_PEERCRED 0x001 /* retrieve peer credentials */
#endif // LOCAL_PEERCRED
#endif // APPLE_OSX_mDNSResponder

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - Types and Data Structures
#endif

typedef enum
    {
    t_uninitialized,
    t_morecoming,
    t_complete,
    t_error,
    t_terminated
    } transfer_state;

typedef void (*req_termination_fn)(void *);

typedef struct registered_record_entry
    {
    uint32_t key;
    AuthRecord *rr;
    struct registered_record_entry *next;
    client_context_t client_context;
    struct request_state *rstate;
    } registered_record_entry;

// A single registered service: ServiceRecordSet + bookkeeping
// Note that we duplicate some fields from parent service_info object
// to facilitate cleanup, when instances and parent may be deallocated at different times.
typedef struct service_instance
    {
    struct service_instance *next;
    mDNSBool autoname;				// Set if this name is tied to the Computer Name
    mDNSBool autorename;			// Set if this client wants us to automatically rename on conflict
    mDNSBool allowremotequery;		// Respond to unicast queries from outside the local link?
    mDNSBool renameonmemfree;  		// Set on config change when we deregister original name
    domainlabel name;
    domainname domain;
    mDNSBool default_local;			// is this the "local." from an empty-string registration?
    struct request_state *request;
    dnssd_sock_t sd;
    AuthRecord *subtypes;
    ServiceRecordSet srs; // note - must be last field in struct
    } service_instance;

// A client-created service.  May reference several service_info objects if default
// settings cause registration in multiple domains.
typedef struct
	{
    uint16_t txtlen;
    void *txtdata;
    mDNSIPPort port;
    domainlabel name;
    char type_as_string[MAX_ESCAPED_DOMAIN_NAME];
    domainname type;
    mDNSBool default_domain;
    domainname host;
    mDNSBool autoname;				// Set if this name is tied to the Computer Name
    mDNSBool autorename;			// Set if this client wants us to automatically rename on conflict
    mDNSBool allowremotequery;		// Respond to unicast queries from outside the local link?
    int num_subtypes;
    mDNSInterfaceID InterfaceID;
    service_instance *instances;
    struct request_state *request;
	} service_info;

// for multi-domain default browsing
typedef struct browser_t
	{
    DNSQuestion q;
    domainname domain;
    struct browser_t *next;
	} browser_t;

// parent struct for browser instances: list pointer plus metadata
typedef struct
	{
    mDNSBool default_domain;
    mDNSBool ForceMCast;
    domainname regtype;
    mDNSInterfaceID interface_id;
    struct request_state *rstate;
    browser_t *browsers;
	} browser_info_t;

typedef struct
	{
	mDNSInterfaceID       interface_id;
	uint8_t               protocol;
	mDNSIPPort            privatePort;
	mDNSIPPort            requestedPublicPort;
	mDNSIPPort            receivedPublicPort;
	uint32_t              requestedTTL;
	uint32_t              receivedTTL;
	mDNSAddr		      addr;
	NATTraversalInfo     *NATAddrinfo;
	NATTraversalInfo     *NATMapinfo;
	struct request_state *rstate;
	} port_mapping_info_t;

typedef struct
	{
	uint32_t              flags;
	mDNSInterfaceID       interface_id;
	uint32_t              protocol;
	DNSQuestion           q4;
	DNSQuestion           q6;
    struct request_state *rstate;
	} addrinfo_info_t;

typedef struct
    {
    mStatus err;		// Note: This field is in NETWORK byte order
    int nwritten;
    dnssd_sock_t sd;
    } undelivered_error_t;

typedef struct request_state
    {
    // connection structures
    dnssd_sock_t sd;

    // state of read (in case message is read over several recv() calls)
    transfer_state ts;
    uint32_t hdr_bytes;		// bytes of header already read
    ipc_msg_hdr hdr;
    uint32_t data_bytes;	// bytes of message data already read
    char *msgbuf;		// pointer to data storage to pass to free()
    char *msgdata;		// pointer to data to be read from (may be modified)
    int bufsize;		// size of data storage

    // reply, termination, error, and client context info
    int no_reply;		// don't send asynchronous replies to client
    int time_blocked;           // record time of a blocked client
    void *client_context;	// don't touch this - pointer only valid in client's addr space
    struct reply_state *replies;  // corresponding (active) reply list
    undelivered_error_t *u_err;
    void *termination_context;
    req_termination_fn terminate;

    //!!!KRS toss these pointers in a union
    // registration context associated with this request (null if not applicable)
    registered_record_entry *reg_recs;  // muliple registrations for a connection-oriented request
    service_info *service_registration;
    browser_info_t *browser_info;
	port_mapping_info_t * port_mapping_create_info;
	addrinfo_info_t     * addrinfo_info;
    struct request_state *next;
    } request_state;

// struct physically sits between ipc message header and call-specific fields in the message buffer
typedef struct
    {
    DNSServiceFlags flags;			// Note: This field is in NETWORK byte order
    uint32_t ifi;					// Note: This field is in NETWORK byte order
    DNSServiceErrorType error;		// Note: This field is in NETWORK byte order
    } reply_hdr;

typedef struct reply_state
    {
    // state of the transmission
    dnssd_sock_t sd;
    transfer_state ts;
    uint32_t nwriten;
    uint32_t len;
    // context of the reply
    struct request_state *request;  // the request that this answers
    struct reply_state *next;   // if there are multiple unsent replies
    // pointer into message buffer - allows fields to be changed after message is formatted
    ipc_msg_hdr *mhdr;
    reply_hdr *rhdr;
    char *sdata;  // pointer to start of call-specific data
    // pointer to malloc'd buffer
    char *msgbuf;
    } reply_state;

// domain enumeration and resolv calls require 2 mDNSCore calls, so we need separate interconnected
// structures to handle callbacks
typedef struct
    {
    DNSQuestion question;
    mDNS_DomainType type;
    request_state *rstate;
    } domain_enum_t;

typedef struct
    {
    domain_enum_t *all;
    domain_enum_t *def;
    request_state *rstate;
    } enum_termination_t;

typedef struct
    {
    request_state *rstate;
    DNSQuestion qtxt;
    DNSQuestion qsrv;
    const ResourceRecord *txt;
    const ResourceRecord *srv;
    } resolve_termination_t;

#ifdef _HAVE_SETDOMAIN_SUPPORT_
typedef struct default_browse_list_t
	{
    struct default_browse_list_t *next;
    uid_t uid;
    AuthRecord ptr_rec;
	} default_browse_list_t;

static default_browse_list_t *default_browse_list = NULL;
#endif // _HAVE_SETDOMAIN_SUPPORT_

// globals
mDNSexport mDNS mDNSStorage;

static dnssd_sock_t			listenfd		=	dnssd_InvalidSocket;
static request_state	*	all_requests	=	NULL;

#define MAX_TIME_BLOCKED 60 * mDNSPlatformOneSecond  // try to send data to a blocked client for 60 seconds before
                                                     // terminating connection
#define MSG_PAD_BYTES 5                              // pad message buffer (read from client) with n zero'd bytes to guarantee
                                                     // n get_string() calls w/o buffer overrun
// initialization, setup/teardown functions

// If a platform specifies its own PID file name, we use that
#ifndef PID_FILE
#define PID_FILE "/var/run/mDNSResponder.pid"
#endif

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - General Utility Functions
#endif

mDNSlocal void FatalError(char *errmsg)
	{
	LogMsg("%s: %s", errmsg, dnssd_strerror(dnssd_errno()));
	*(long*)0 = 0;	// On OS X abort() doesn't generate a crash log, but writing to zero does
	abort();		// On platforms where writing to zero doesn't generate an exception, abort instead
	}
	
mDNSlocal uint32_t dnssd_htonl(uint32_t l)
	{
	uint32_t 	ret;
	char	*	data = (char*) &ret;
	put_long(l, &data);
	return ret;
	}

// hack to search-replace perror's to LogMsg's
mDNSlocal void my_perror(char *errmsg)
    {
    LogMsg("%s: %s", errmsg, dnssd_strerror(dnssd_errno()));
    }

mDNSlocal mDNSBool is_routeable_v4(mDNSAddr * addr)
	{
	mDNSu8 * b;

	if (addr->type != mDNSAddrType_IPv4) return mDNSfalse;
		
	b = addr->ip.v4.b;
		
	if (((b[0] == 169) && (b[1] == 254))            ||
		((b[0] == 127))                             ||
		((b[0] == 10))                              ||
		((b[0] == 172) && (b[1] > 15 && b[1] < 32)) ||
		((b[0] == 192) && (b[1] == 168)))
		{
		return mDNSfalse;
		}
		
	return mDNStrue;
	}

// If there's a comma followed by another character,
// FindFirstSubType overwrites the comma with a nul and returns the pointer to the next character.
// Otherwise, it returns a pointer to the final nul at the end of the string
mDNSlocal char *FindFirstSubType(char *p)
	{
	while (*p)
		{
		if (p[0] == '\\' && p[1]) p += 2;
		else if (p[0] == ',' && p[1]) { *p++ = 0; return(p); }
		else p++;
		}
	return(p);
	}

// If there's a comma followed by another character,
// FindNextSubType overwrites the comma with a nul and returns the pointer to the next character.
// If it finds an illegal unescaped dot in the subtype name, it returns mDNSNULL
// Otherwise, it returns a pointer to the final nul at the end of the string
mDNSlocal char *FindNextSubType(char *p)
	{
	while (*p)
		{
		if (p[0] == '\\' && p[1])		// If escape character
			p += 2;						// ignore following character
		else if (p[0] == ',')			// If we found a comma
			{
			if (p[1]) *p++ = 0;
			return(p);
			}
		else if (p[0] == '.')
			return(mDNSNULL);
		else p++;
		}
	return(p);
	}

// Returns -1 if illegal subtype found
mDNSexport mDNSs32 ChopSubTypes(char *regtype)
	{
	mDNSs32 NumSubTypes = 0;
	char *stp = FindFirstSubType(regtype);
	while (stp && *stp)					// If we found a comma...
		{
		if (*stp == ',') return(-1);
		NumSubTypes++;
		stp = FindNextSubType(stp);
		}
	if (!stp) return(-1);
	return(NumSubTypes);
	}

mDNSexport AuthRecord *AllocateSubTypes(mDNSs32 NumSubTypes, char *p)
	{
	AuthRecord *st = mDNSNULL;
	if (NumSubTypes)
		{
		mDNSs32 i;
		st = mallocL("ServiceSubTypes", NumSubTypes * sizeof(AuthRecord));
		if (!st) return(mDNSNULL);
		for (i = 0; i < NumSubTypes; i++)
			{
			mDNS_SetupResourceRecord(&st[i], mDNSNULL, mDNSInterface_Any, kDNSQType_ANY, kStandardTTL, 0, mDNSNULL, mDNSNULL);
			while (*p) p++;
			p++;
			if (!MakeDomainNameFromDNSNameString(st[i].resrec.name, p))
				{ freeL("ServiceSubTypes", st); return(mDNSNULL); }
			}
		}
	return(st);
	}

// returns 0 on success, -1 if send is incomplete, or on terminal failure (request is aborted)
mDNSlocal transfer_state send_undelivered_error(request_state *rs)
	{
	int nwritten;
	
	nwritten = send(rs->u_err->sd, (char *)(&rs->u_err->err) + rs->u_err->nwritten, sizeof(mStatus) - rs->u_err->nwritten, 0);
	if (nwritten < 0)
		{
		if (dnssd_errno() == dnssd_EINTR || dnssd_errno() == dnssd_EWOULDBLOCK)
			nwritten = 0;
		else
			{
			my_perror("ERROR: send - unable to deliver error to client\n");
			return t_error;
			}
		}
	if ((unsigned int)(nwritten + rs->u_err->nwritten) >= sizeof(mStatus))
		{
		freeL("send_undelivered_error", rs->u_err);
		rs->u_err = NULL;
		return t_complete;
		}
	rs->u_err->nwritten += nwritten;
	return t_morecoming;
	}

mDNSlocal int send_msg(reply_state *rs)
    {
    ssize_t nwriten;
    
    if (!rs->msgbuf)
        {
        LogMsg("ERROR: send_msg called with NULL message buffer");
        return t_error;
        }
    
    if (rs->request->no_reply)	//!!!KRS this behavior should be optimized if it becomes more common
        {
        rs->ts = t_complete;
        freeL("send_msg", rs->msgbuf);
        return t_complete;
        }

	ConvertHeaderBytes(rs->mhdr);
    nwriten = send(rs->sd, rs->msgbuf + rs->nwriten, rs->len - rs->nwriten, 0);
	ConvertHeaderBytes(rs->mhdr);
    if (nwriten < 0)
    	{
        if (dnssd_errno() == dnssd_EINTR || dnssd_errno() == dnssd_EWOULDBLOCK) nwriten = 0;
        else
            {
#if !defined(PLATFORM_NO_EPIPE)
            if (dnssd_errno() == EPIPE)
            	{
                debugf("%3d: broken pipe", rs->sd);
                rs->ts = t_terminated;
                rs->request->ts = t_terminated;
                return t_terminated;
            	}
            else
#endif
            	{
                my_perror("ERROR: send\n");
                rs->ts = t_error;
                return t_error;
            	}
            }
        }
    rs->nwriten += nwriten;

    if (rs->nwriten == rs->len)
    	{
        rs->ts = t_complete;
        freeL("send_msg", rs->msgbuf);
    	}
    return rs->ts;
    }

mDNSlocal void abort_request(request_state *rs)
    {
    reply_state *rep, *ptr;

    if (rs->terminate) rs->terminate(rs->termination_context);  // terminate field may not be set yet
    if (rs->msgbuf) freeL("abort_request", rs->msgbuf);
	LogOperation("%3d: Removing FD", rs->sd);
    udsSupportRemoveFDFromEventLoop(rs->sd);					// Note: This also closes file descriptor rs->sd for us

	// Don't use dnssd_InvalidSocket (-1) because that's the sentinel value MACOSX_MDNS_MALLOC_DEBUGGING uses
	// for detecting when the memory for an object is inadvertently freed while the object is still on some list
    rs->sd = -2;

    // free pending replies
    rep = rs->replies;
    while(rep)
    	{
        if (rep->msgbuf) freeL("abort_request", rep->msgbuf);
        ptr = rep;
        rep = rep->next;
        freeL("abort_request", ptr);
    	}
    
    if (rs->u_err)
        {
        freeL("abort_request", rs->u_err);
        rs->u_err = NULL;
        }
    }

#if APPLE_OSX_mDNSResponder && MACOSX_MDNS_MALLOC_DEBUGGING
mDNSexport void uds_validatelists(void)
	{
	request_state *req;
	for (req = all_requests; req; req=req->next)
		if (req->sd < 0 && req->sd != -2)
			LogMemCorruption("UDS request list: %p is garbage (%X)", req, req->sd);
	}
#endif

mDNSlocal void unlink_request(request_state *rs)
    {
    request_state *ptr;
    
    if (rs == all_requests)
        {
        all_requests = all_requests->next;
        freeL("unlink_request", rs);
        return;
        }
    for (ptr = all_requests; ptr->next; ptr = ptr->next)
        if (ptr->next == rs)
            {
            ptr->next = rs->next;
            freeL("unlink_request", rs);
            return;
        }
    }

mDNSlocal reply_state *create_reply(reply_op_t op, size_t datalen, request_state *request)
	{
    reply_state *reply;
    int totallen;

    if ((unsigned)datalen < sizeof(reply_hdr))
        {
        LogMsg("ERROR: create_reply - data length less than lenght of required fields");
        return NULL;
        }
    
    totallen = (int) (datalen + sizeof(ipc_msg_hdr));
    reply = mallocL("create_reply", sizeof(reply_state));
    if (!reply) FatalError("ERROR: malloc");
    mDNSPlatformMemZero(reply, sizeof(reply_state));
    reply->ts = t_morecoming;
    reply->sd = request->sd;
    reply->request = request;
    reply->len = totallen;
    reply->msgbuf = mallocL("create_reply", totallen);
    if (!reply->msgbuf) FatalError("ERROR: malloc");
    mDNSPlatformMemZero(reply->msgbuf, totallen);
    reply->mhdr = (ipc_msg_hdr *)reply->msgbuf;
    reply->rhdr = (reply_hdr *)(reply->msgbuf + sizeof(ipc_msg_hdr));
    reply->sdata = reply->msgbuf + sizeof(ipc_msg_hdr) + sizeof(reply_hdr);
    reply->mhdr->version = VERSION;
    reply->mhdr->op = op;
    reply->mhdr->datalen = totallen - sizeof(ipc_msg_hdr);
    return reply;
    }

// append a reply to the list in a request object
mDNSlocal void append_reply(request_state *req, reply_state *rep)
    {
    reply_state *ptr;

    if (!req->replies) req->replies = rep;
    else
    	{
        ptr = req->replies;
        while (ptr->next) ptr = ptr->next;
        ptr->next = rep;
    	}
    rep->next = NULL;
    }

// send bogus data along with an error code to the app callback
// returns 0 on success (linking reply into list of not fully delivered),
// -1 on failure (request should be aborted)
mDNSlocal int deliver_async_error(request_state *rs, reply_op_t op, mStatus err)
    {
    int len;
    reply_state *reply;
    transfer_state ts;
    
    if (rs->no_reply) return 0;
    len = 256;		// long enough for any reply handler to read all args w/o buffer overrun
    reply = create_reply(op, len, rs);
    reply->rhdr->error = dnssd_htonl(err);
    ts = send_msg(reply);
    if (ts == t_error || ts == t_terminated)
        {
        freeL("deliver_async_error", reply);
        return -1;
        }
    else if (ts == t_complete) freeL("deliver_async_error", reply);
    else if (ts == t_morecoming) append_reply(rs, reply);   // client is blocked, link reply into list
    return 0;
    }

// Generates a response message giving name, type, domain, plus interface index,
// suitable for a browse result or service registration result.
// On successful completion rep is set to point to a malloc'd reply_state struct
mDNSlocal mStatus GenerateNTDResponse(domainname *servicename, mDNSInterfaceID id, request_state *request, reply_state **rep)
	{
	domainlabel name;
	domainname type, dom;
	*rep = NULL;
	if (!DeconstructServiceName(servicename, &name, &type, &dom))
		return kDNSServiceErr_Invalid;
	else
		{
		char namestr[MAX_DOMAIN_LABEL+1];
		char typestr[MAX_ESCAPED_DOMAIN_NAME];
		char domstr [MAX_ESCAPED_DOMAIN_NAME];
		int len;
		char *data;
	
		ConvertDomainLabelToCString_unescaped(&name, namestr);
		ConvertDomainNameToCString(&type, typestr);
		ConvertDomainNameToCString(&dom, domstr);
		
		// Calculate reply data length
		len = sizeof(DNSServiceFlags);
		len += sizeof(uint32_t);  // if index
		len += sizeof(DNSServiceErrorType);
		len += (int) (strlen(namestr) + 1);
		len += (int) (strlen(typestr) + 1);
		len += (int) (strlen(domstr) + 1);
		
		// Build reply header
		*rep = create_reply(query_reply_op, len, request);
		(*rep)->rhdr->flags = dnssd_htonl(0);
		(*rep)->rhdr->ifi   = dnssd_htonl(mDNSPlatformInterfaceIndexfromInterfaceID(&mDNSStorage, id));
		(*rep)->rhdr->error = dnssd_htonl(kDNSServiceErr_NoError);
		
		// Build reply body
		data = (*rep)->sdata;
		put_string(namestr, &data);
		put_string(typestr, &data);
		put_string(domstr, &data);

		return mStatus_NoError;
		}
	}

mDNSlocal int deliver_error(request_state *rstate, mStatus err)
	{
	int nwritten = -1;
	undelivered_error_t *undeliv;
	
	err = dnssd_htonl(err);
	nwritten = send(rstate->sd, (dnssd_sockbuf_t) &err, sizeof(mStatus), 0);
	if (nwritten < (int)sizeof(mStatus))
		{
		if (dnssd_errno() == dnssd_EINTR || dnssd_errno() == dnssd_EWOULDBLOCK)
			nwritten = 0;
		if (nwritten < 0)
			{
			my_perror("ERROR: send - unable to deliver error to client");
			return(-1);
			}
		else
			{
			//client blocked - store result and come backr
			undeliv = mallocL("deliver_error", sizeof(undelivered_error_t));
			if (!undeliv) FatalError("ERROR: malloc");
			undeliv->err      = err;
			undeliv->nwritten = nwritten;
			undeliv->sd       = rstate->sd;
			rstate->u_err     = undeliv;
			return 0;
			}
		}
	return 0;
	}

// read_msg may be called any time when the transfer state (rs->ts) is t_morecoming.
// returns the current state of the request (morecoming, error, complete, terminated.)
// if there is no data on the socket, the socket will be closed and t_terminated will be returned
mDNSlocal int read_msg(request_state *rs)
    {
    uint32_t nleft;
    int nread;
    char buf[4];   // dummy for death notification
    
    if (rs->ts == t_terminated || rs->ts == t_error)
        {
        LogMsg("ERROR: read_msg called with transfer state terminated or error");
        rs->ts = t_error;
        return t_error;
        }
        
    if (rs->ts == t_complete)
    	{  // this must be death or something is wrong
        nread = recv(rs->sd, buf, 4, 0);
        if (!nread) 	{  rs->ts = t_terminated;  return t_terminated;  	}
        if (nread < 0) goto rerror;
        LogMsg("ERROR: read data from a completed request.");
        rs->ts = t_error;
        return t_error;
    	}

    if (rs->ts != t_morecoming)
        {
        LogMsg("ERROR: read_msg called with invalid transfer state (%d)", rs->ts);
        rs->ts = t_error;
        return t_error;
        }
        
    if (rs->hdr_bytes < sizeof(ipc_msg_hdr))
    	{
        nleft = sizeof(ipc_msg_hdr) - rs->hdr_bytes;
        nread = recv(rs->sd, (char *)&rs->hdr + rs->hdr_bytes, nleft, 0);
        if (nread == 0)  	{ rs->ts = t_terminated;  return t_terminated;  	}
        if (nread < 0) goto rerror;
        rs->hdr_bytes += nread;
        if (rs->hdr_bytes == sizeof(ipc_msg_hdr))
        	{
        	ConvertHeaderBytes(&rs->hdr);
			if (rs->hdr.version != VERSION)
				{
				LogMsg("ERROR: read_msg - client version 0x%08X does not match daemon version 0x%08X", rs->hdr.version, VERSION);
				rs->ts = t_error;
				return t_error;
				}
			}
        if (rs->hdr_bytes > sizeof(ipc_msg_hdr))
            {
            LogMsg("ERROR: read_msg - read too many header bytes");
            rs->ts = t_error;
            return t_error;
            }
    	}

    // only read data if header is complete
    if (rs->hdr_bytes == sizeof(ipc_msg_hdr))
    	{
        if (rs->hdr.datalen == 0)  // ok in removerecord requests
            {
            rs->ts = t_complete;
            rs->msgbuf = NULL;
            return t_complete;
            }
        
        if (!rs->msgbuf)  // allocate the buffer first time through
            {
            rs->msgbuf = mallocL("read_msg", rs->hdr.datalen + MSG_PAD_BYTES);
            if (!rs->msgbuf)
            	{
                my_perror("ERROR: malloc");
                rs->ts = t_error;
                return t_error;
            	}
            rs->msgdata = rs->msgbuf;
            mDNSPlatformMemZero(rs->msgbuf, rs->hdr.datalen + MSG_PAD_BYTES);
            }
        nleft = rs->hdr.datalen - rs->data_bytes;
        nread = recv(rs->sd, rs->msgbuf + rs->data_bytes, nleft, 0);
        if (nread == 0)  	{ rs->ts = t_terminated;  return t_terminated; 	}
        if (nread < 0)	goto rerror;
        rs->data_bytes += nread;
        if (rs->data_bytes > rs->hdr.datalen)
            {
            LogMsg("ERROR: read_msg - read too many data bytes");
            rs->ts = t_error;
            return t_error;
            }
        }

    if (rs->hdr_bytes == sizeof(ipc_msg_hdr) && rs->data_bytes == rs->hdr.datalen)
        rs->ts = t_complete;
    else rs->ts = t_morecoming;

    return rs->ts;

rerror:
    if (dnssd_errno() == dnssd_EWOULDBLOCK || dnssd_errno() == dnssd_EINTR) return t_morecoming;
    my_perror("ERROR: read_msg");
    rs->ts = t_error;
    return t_error;
    }

// check that the message delivered by the client is sufficiently long to extract the required data from the buffer
// without overrunning it.
// returns 0 on success, -1 on error.

mDNSlocal int validate_message(request_state *rstate)
    {
    uint32_t min_size;
    
    switch(rstate->hdr.op)
    	{
        case resolve_request: min_size = 	sizeof(DNSServiceFlags) +	// flags
						sizeof(uint32_t) + 		// interface
						(3 * sizeof(char));           	// name, regtype, domain
						break;
        case query_request: min_size = 		sizeof(DNSServiceFlags) + 	// flags
						sizeof(uint32_t) +		// interface
						sizeof(char) + 			// fullname
						(2 * sizeof(uint16_t)); 	// type, class
						break;
        case browse_request: min_size = 	sizeof(DNSServiceFlags) +	// flags
						sizeof(uint32_t) +		// interface
						(2 * sizeof(char)); 		// regtype, domain
						break;
        case reg_service_request: min_size = 	sizeof(DNSServiceFlags) +	// flags
						sizeof(uint32_t) +		// interface
						(4 * sizeof(char)) + 		// name, type, domain, host
						(2 * sizeof(uint16_t));		// port, textlen
						break;
        case enumeration_request: min_size =	sizeof(DNSServiceFlags) +	// flags
						sizeof(uint32_t); 		// interface
						break;
        case reg_record_request: min_size = 	sizeof(DNSServiceFlags) +	// flags
						sizeof(uint32_t) + 		// interface
						sizeof(char) + 			// fullname
						(3 * sizeof(uint16_t)) +	// type, class, rdlen
						sizeof(uint32_t);		// ttl
						break;
        case add_record_request: min_size = 	sizeof(DNSServiceFlags) +	// flags
						(2 * sizeof(uint16_t)) + 	// type, rdlen
						sizeof(uint32_t);		// ttl
						break;
        case update_record_request: min_size =	sizeof(DNSServiceFlags) +	// flags
						sizeof(uint16_t) +		// rdlen
						sizeof(uint32_t); 		// ttl
						break;
        case remove_record_request: min_size =	sizeof(DNSServiceFlags);	// flags
						break;
        case reconfirm_record_request: min_size=sizeof(DNSServiceFlags) +	// flags
						sizeof(uint32_t) + 		// interface
						sizeof(char) + 			// fullname
						(3 * sizeof(uint16_t));		// type, class, rdlen
			            break;
		case setdomain_request: min_size = sizeof(DNSServiceFlags) + sizeof(char);  // flags + domain
			break;
		case port_mapping_create_request: min_size = sizeof(DNSServiceFlags) + 	// flags
		                sizeof(uint32_t) +             // interface
		                sizeof(uint8_t)  +             // protocol
		                sizeof(uint16_t) +             // private port
		                sizeof(uint16_t) +             // public port
		                sizeof(uint32_t);
		    break;
		case addrinfo_request: min_size = sizeof(DNSServiceFlags) + // flags
		                sizeof(uint32_t) +             // interface
						sizeof(uint32_t) +             // protocol
						sizeof(char);                  // hostname
			break;
		default:
            LogMsg("ERROR: validate_message - unsupported request type: %d", rstate->hdr.op);
	    return -1;
	}
    
	return (rstate->data_bytes >= min_size ? 0 : -1);
    }

// returns a resource record (allocated w/ malloc) containing the data found in an IPC message
// data must be in format flags, interfaceIndex, name, rrtype, rrclass, rdlen, rdata, (optional)ttl
// (ttl only extracted/set if ttl argument is non-zero).  returns NULL for a bad-parameter error
mDNSlocal AuthRecord *read_rr_from_ipc_msg(char *msgbuf, int GetTTL, int validate_flags)
    {
    char *rdata, name[256];
    AuthRecord *rr;
    DNSServiceFlags flags;
    uint32_t interfaceIndex;
    uint16_t type, class, rdlen;
    int storage_size;

    flags = get_flags(&msgbuf);
	if (validate_flags &&
		!((flags & kDNSServiceFlagsShared) == kDNSServiceFlagsShared) &&
		!((flags & kDNSServiceFlagsUnique) == kDNSServiceFlagsUnique))
		{
		LogMsg("ERROR: Bad resource record flags (must be kDNSServiceFlagsShared or kDNSServiceFlagsUnique)");
		return NULL;
		}
	
	interfaceIndex = get_long(&msgbuf);
    if (get_string(&msgbuf, name, 256) < 0)
        {
        LogMsg("ERROR: read_rr_from_ipc_msg - get_string");
        return NULL;
        }
    type = get_short(&msgbuf);
    class = get_short(&msgbuf);
    rdlen = get_short(&msgbuf);

    if (rdlen > sizeof(RDataBody)) storage_size = rdlen;
    else storage_size = sizeof(RDataBody);
    
    rr = mallocL("read_rr_from_ipc_msg", sizeof(AuthRecord) - sizeof(RDataBody) + storage_size);
    if (!rr) FatalError("ERROR: malloc");
    mDNS_SetupResourceRecord(rr, mDNSNULL, mDNSPlatformInterfaceIDfromInterfaceIndex(&mDNSStorage, interfaceIndex),
		type, 0, (mDNSu8) ((flags & kDNSServiceFlagsShared) ? kDNSRecordTypeShared : kDNSRecordTypeUnique), mDNSNULL, mDNSNULL);
    
    if (!MakeDomainNameFromDNSNameString(rr->resrec.name, name))
    	{
        LogMsg("ERROR: bad name: %s", name);
        freeL("read_rr_from_ipc_msg", rr);
        return NULL;
    	}
    
    if (flags & kDNSServiceFlagsAllowRemoteQuery) rr->AllowRemoteQuery  = mDNStrue;
    rr->resrec.rrclass = class;
    rr->resrec.rdlength = rdlen;
    rr->resrec.rdata->MaxRDLength = rdlen;
    rdata = get_rdata(&msgbuf, rdlen);
    memcpy(rr->resrec.rdata->u.data, rdata, rdlen);
    if (GetTTL) rr->resrec.rroriginalttl = get_long(&msgbuf);
    rr->resrec.namehash = DomainNameHashValue(rr->resrec.name);
    SetNewRData(&rr->resrec, mDNSNULL, 0);	// Sets rr->rdatahash for us
    return rr;
    }

// setup rstate to accept new reg/dereg requests
mDNSlocal void reset_connected_rstate(request_state *rstate)
    {
    rstate->ts = t_morecoming;
    rstate->hdr_bytes = 0;
    rstate->data_bytes = 0;
    if (rstate->msgbuf) freeL("reset_connected_rstate", rstate->msgbuf);
    rstate->msgbuf = NULL;
    rstate->bufsize = 0;
    }

mDNSlocal int build_domainname_from_strings(domainname *srv, char *name, char *regtype, char *domain)
    {
    domainlabel n;
    domainname d, t;

    if (!MakeDomainLabelFromLiteralString(&n, name)) return -1;
    if (!MakeDomainNameFromDNSNameString(&t, regtype)) return -1;
    if (!MakeDomainNameFromDNSNameString(&d, domain)) return -1;
    if (!ConstructServiceName(srv, &n, &t, &d)) return -1;
    return 0;
    }

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - Handle DNSServiceRegister Requests
#endif

mDNSexport void FreeExtraRR(mDNS *const m, AuthRecord *const rr, mStatus result)
	{
	ExtraResourceRecord *extra = (ExtraResourceRecord *)rr->RecordContext;
	(void)m;  //unused
	
	if (result != mStatus_MemFree) { LogMsg("Error: FreeExtraRR invoked with unexpected error %d", result); return; }
	
	debugf("%##s: MemFree", rr->resrec.name->c);
	if (rr->resrec.rdata != &rr->rdatastorage)
		freeL("Extra RData", rr->resrec.rdata);
	freeL("ExtraResourceRecord", extra);
	}

mDNSlocal void free_service_instance(service_instance *srv)
	{
	request_state *rstate = srv->request;
	ExtraResourceRecord *e = srv->srs.Extras, *tmp;
	
	// clear pointers from parent struct
	if (rstate)
		{
		service_instance *ptr = rstate->service_registration->instances, *prev = NULL;
		while (ptr)
			{
			if (ptr == srv)
				{
				if (prev) prev->next = ptr->next;
				else rstate->service_registration->instances = ptr->next;
				break;
				}
			prev = ptr;
			ptr = ptr->next;
			}
		}
	
	while(e)
		{
		e->r.RecordContext = e;
		tmp = e;
		e = e->next;
		FreeExtraRR(&mDNSStorage, &tmp->r, mStatus_MemFree);
		}
	
	if (srv->subtypes) { freeL("regservice_callback", srv->subtypes); srv->subtypes = NULL; }
	freeL("regservice_callback", srv);
	}

// Count how many other service records we have locally with the same name, but different rdata.
// For auto-named services, we can have at most one per machine -- if we allowed two auto-named services of
// the same type on the same machine, we'd get into an infinite autoimmune-response loop of continuous renaming.
mDNSexport int CountPeerRegistrations(mDNS *const m, ServiceRecordSet *const srs)
	{
	int count = 0;
	ResourceRecord *r = &srs->RR_SRV.resrec;
	AuthRecord *rr;
	ServiceRecordSet *s;
	
	for (rr = m->ResourceRecords; rr; rr=rr->next)
		if (rr->resrec.rrtype == kDNSType_SRV && SameDomainName(rr->resrec.name, r->name) && !SameRData(&rr->resrec, r))
			count++;

	for (rr = m->RecordRegistrations; rr; rr=rr->next)
		if (rr->state != regState_Unregistered && rr->resrec.rrtype == kDNSType_SRV && SameDomainName(rr->resrec.name, r->name) && !SameRData(&rr->resrec, r))
			count++;

	for (s = m->ServiceRegistrations; s; s = s->next)
		if (s->state != regState_Unregistered && SameDomainName(s->RR_SRV.resrec.name, r->name) && !SameRData(&s->RR_SRV.resrec, r))
			count++;
		
	verbosedebugf("%d peer registrations for %##s", count, r->name->c);
	return(count);
	}

mDNSexport int CountExistingRegistrations(domainname *srv, mDNSIPPort port)
	{
	int count = 0;
	AuthRecord *rr;
	for (rr = mDNSStorage.ResourceRecords; rr; rr=rr->next)
		if (rr->resrec.rrtype == kDNSType_SRV &&
			rr->resrec.rdata->u.srv.port.NotAnInteger == port.NotAnInteger &&
			SameDomainName(rr->resrec.name, srv))
			count++;
	return(count);
	}

// service registration callback performs three duties - frees memory for deregistered services,
// handles name conflicts, and delivers completed registration information to the client (via
// process_service_registraion())
mDNSlocal void regservice_callback(mDNS *const m, ServiceRecordSet *const srs, mStatus result)
	{
	mStatus err;
	mDNSBool SuppressError = mDNSfalse;
	service_instance *instance = srs->ServiceContext;
	(void)m; // Unused
	if (!srs)      { LogMsg("regservice_callback: srs is NULL %d",                 result); return; }
	if (!instance) { LogMsg("regservice_callback: srs->ServiceContext is NULL %d", result); return; }

	// don't send errors up to client for wide-area, empty-string registrations
	if (instance->request &&
		instance->request->service_registration &&
		instance->request->service_registration->default_domain &&
		!instance->default_local)
			SuppressError = mDNStrue;

	if (result == mStatus_NoError)
		LogOperation("%3d: DNSServiceRegister(%##s, %u) REGISTERED  ",  instance->sd, srs->RR_SRV.resrec.name->c, mDNSVal16(srs->RR_SRV.resrec.rdata->u.srv.port));
	else if (result == mStatus_MemFree)
		LogOperation("%3d: DNSServiceRegister(%##s, %u) DEREGISTERED",  instance->sd, srs->RR_SRV.resrec.name->c, mDNSVal16(srs->RR_SRV.resrec.rdata->u.srv.port));
	else if (result == mStatus_NameConflict)
		LogOperation("%3d: DNSServiceRegister(%##s, %u) NAME CONFLICT", instance->sd, srs->RR_SRV.resrec.name->c, mDNSVal16(srs->RR_SRV.resrec.rdata->u.srv.port));
	else
		LogOperation("%3d: DNSServiceRegister(%##s, %u) CALLBACK %d",   instance->sd, srs->RR_SRV.resrec.name->c, mDNSVal16(srs->RR_SRV.resrec.rdata->u.srv.port), result);

	if (result == mStatus_NoError)
		{
		request_state *req = instance->request;
		if (instance->allowremotequery)
			{
			ExtraResourceRecord *e;
			srs->RR_ADV.AllowRemoteQuery = mDNStrue;
			srs->RR_PTR.AllowRemoteQuery = mDNStrue;
			srs->RR_SRV.AllowRemoteQuery = mDNStrue;
			srs->RR_TXT.AllowRemoteQuery = mDNStrue;
			for (e = instance->srs.Extras; e; e = e->next) e->r.AllowRemoteQuery = mDNStrue;
			}

		if (!req) LogMsg("ERROR: regservice_callback - null request object");
		else
			{
			reply_state *rep;
			if (GenerateNTDResponse(srs->RR_SRV.resrec.name, srs->RR_SRV.resrec.InterfaceID, req, &rep) != mStatus_NoError)
				LogMsg("%3d: regservice_callback: %##s is not valid DNS-SD SRV name", req->sd, srs->RR_SRV.resrec.name->c);
			else
				{
				transfer_state send_result = send_msg(rep);
				if (send_result == t_error || send_result == t_terminated)
					{ abort_request(req); unlink_request(req); freeL("reply_state", rep); }
				else if (send_result == t_complete) freeL("regservice_callback", rep);
				else append_reply(req, rep);
				}
			}
		if (instance->autoname && CountPeerRegistrations(m, srs) == 0)
			RecordUpdatedNiceLabel(m, 0);	// Successfully got new name, tell user immediately
		}
	else if (result == mStatus_MemFree)
		{
		if (instance->renameonmemfree)
			{
			instance->renameonmemfree = 0;
			instance->name = mDNSStorage.nicelabel;
			err = mDNS_RenameAndReregisterService(&mDNSStorage, srs, &instance->name);
			if (err) LogMsg("ERROR: regservice_callback - RenameAndReregisterService returned %ld", err);
			// error should never happen - safest to log and continue
			}
		else
			free_service_instance(instance);
		}
	else if (result == mStatus_NameConflict)
		{
		if (instance->autorename)
			{
			if (instance->autoname && CountPeerRegistrations(m, srs) == 0)
				{
				// On conflict for an autoname service, rename and reregister *all* autoname services
				IncrementLabelSuffix(&m->nicelabel, mDNStrue);
				m->MainCallback(m, mStatus_ConfigChanged);
				}
			else	// On conflict for a non-autoname service, rename and reregister just that one service
				mDNS_RenameAndReregisterService(&mDNSStorage, srs, mDNSNULL);
			}
		else
			{
			request_state *rs = instance->request;
			if (!rs) { LogMsg("ERROR: regservice_callback: received result %ld with a NULL request pointer", result); return; }
			free_service_instance(instance);
			if (!SuppressError && deliver_async_error(rs, reg_service_reply_op, result) < 0)
				{
				abort_request(rs);
				unlink_request(rs);
				}
			}
		}
	else
		{
		request_state *rs = instance->request;
		if (!rs) { LogMsg("ERROR: regservice_callback: received result %ld with a NULL request pointer", result); return; }
		if (result != mStatus_NATTraversal) LogMsg("ERROR: unknown result in regservice_callback: %ld", result);
		free_service_instance(instance);
		if (!SuppressError && deliver_async_error(rs, reg_service_reply_op, result) < 0)
			{
			abort_request(rs);
			unlink_request(rs);
			}
		}
	}

mDNSlocal void rename_service(service_instance *srv)
	{
	if (srv->autoname && !SameDomainLabelCS(srv->name.c, mDNSStorage.nicelabel.c))
		{
		srv->renameonmemfree = 1;
		if (mDNS_DeregisterService(&mDNSStorage, &srv->srs))	// If service deregistered already, we can re-register immediately
			regservice_callback(&mDNSStorage, &srv->srs, mStatus_MemFree);
		}
	}

mDNSlocal void AddDefaultRegDomain(mDNS *const m, domainname *d)
	{
	DNameListElem *newelem = mDNSNULL, *ptr;

	// make sure name not already in list
	for (ptr = m->DefRegList; ptr; ptr = ptr->next)
		{
		if (SameDomainName(&ptr->name, d))
			{ debugf("duplicate addition of default reg domain %##s", d->c); return; }
		}

	newelem = mDNSPlatformMemAllocate(sizeof(*newelem));
	if (!newelem) { LogMsg("Error - malloc"); return; }
	AssignDomainName(&newelem->name, d);
	newelem->next = m->DefRegList;
	m->DefRegList = newelem;

	mDNSPlatformDefaultRegDomainChanged(d, mDNStrue);
	}

mDNSlocal void regrecord_callback(mDNS *const m, AuthRecord * rr, mStatus result)
    {
    registered_record_entry *re = rr->RecordContext;
	request_state *rstate = re ? re->rstate : NULL;
    int len;
    reply_state *reply;
    transfer_state ts;
    (void)m; // Unused

	if (!re)
		{
		// parent struct alreadt freed by termination callback
		if (!result) LogMsg("Error: regrecord_callback: successful registration of orphaned record");
		else
			{
			if (result != mStatus_MemFree) LogMsg("regrecord_callback: error %d received after parent termination", result);
			freeL("regrecord_callback", rr);
			}
		return;
		}
	
    // format result, add to the list for the request, including the client context in the header
    len = sizeof(DNSServiceFlags);
    len += sizeof(uint32_t);                //interfaceIndex
    len += sizeof(DNSServiceErrorType);
    
    reply = create_reply(reg_record_reply_op, len, rstate);
    reply->mhdr->client_context = re->client_context;
    reply->rhdr->flags = dnssd_htonl(0);
    reply->rhdr->ifi = dnssd_htonl(mDNSPlatformInterfaceIndexfromInterfaceID(&mDNSStorage, rr->resrec.InterfaceID));
    reply->rhdr->error = dnssd_htonl(result);

	if (result)
		{
		// unlink from list, free memory
		registered_record_entry **ptr = &re->rstate->reg_recs;
		while (*ptr && (*ptr) != re) ptr = &(*ptr)->next;
		if (!*ptr) { LogMsg("regrecord_callback - record not in list!"); return; }
		*ptr = (*ptr)->next;
		freeL("regrecord_callback", re->rr);
		re->rr = rr = NULL;
		freeL("regrecord_callback", re);
		re = NULL;
		}
	
    ts = send_msg(reply);
	
    if (ts == t_error || ts == t_terminated) { abort_request(rstate); unlink_request(rstate); }
    else if (ts == t_complete) freeL("regrecord_callback", reply);
    else if (ts == t_morecoming) append_reply(rstate, reply);   // client is blocked, link reply into list
	}

mDNSlocal void connected_registration_termination(void *context)
    {
    int shared;
    registered_record_entry *fptr, *ptr = ((request_state *)context)->reg_recs;
    while(ptr)
        {
        fptr = ptr;
        ptr = ptr->next;
        shared = fptr->rr->resrec.RecordType == kDNSRecordTypeShared;
		fptr->rr->RecordContext = NULL;
        mDNS_Deregister(&mDNSStorage, fptr->rr);
        freeL("connected_registration_termination", fptr);
		}
	}
    
mDNSlocal mStatus handle_regrecord_request(request_state *rstate)
    {
    AuthRecord *rr;
    registered_record_entry *re;
    mStatus result;
    
    if (rstate->ts != t_complete)
        {
        LogMsg("ERROR: handle_regrecord_request - transfer state != t_complete");
        abort_request(rstate);
        unlink_request(rstate);
        return(-1);
        }
        
    rr = read_rr_from_ipc_msg(rstate->msgdata, 1, 1);
    if (!rr) return(mStatus_BadParamErr);

    // allocate registration entry, link into list
    re = mallocL("handle_regrecord_request", sizeof(registered_record_entry));
    if (!re) FatalError("ERROR: malloc");
    re->key = rstate->hdr.reg_index;
    re->rr = rr;
    re->rstate = rstate;
    re->client_context = rstate->hdr.client_context;
    rr->RecordContext = re;
    rr->RecordCallback = regrecord_callback;
    re->next = rstate->reg_recs;
    rstate->reg_recs = re;

    if (!rstate->terminate)
    	{
        rstate->terminate = connected_registration_termination;
        rstate->termination_context = rstate;
    	}
    
    if (rr->resrec.rroriginalttl == 0)
        rr->resrec.rroriginalttl = DefaultTTLforRRType(rr->resrec.rrtype);
    
	LogOperation("%3d: DNSServiceRegisterRecord %s", rstate->sd, RRDisplayString(&mDNSStorage, &rr->resrec));
    result = mDNS_Register(&mDNSStorage, rr);
    return(result);
    }

mDNSlocal mStatus add_record_to_service(request_state *rstate, service_instance *instance, uint16_t rrtype, uint16_t rdlen, char *rdata, uint32_t ttl)
	{
	ServiceRecordSet *srs = &instance->srs;
    ExtraResourceRecord *extra;
	mStatus result;
	int size;
	
	if (rdlen > sizeof(RDataBody)) size = rdlen;
    else size = sizeof(RDataBody);
	
    extra = mallocL("ExtraResourceRecord", sizeof(*extra) - sizeof(RDataBody) + size);
    if (!extra)
        {
        my_perror("ERROR: malloc");
		return mStatus_NoMemoryErr;
        }
        
    mDNSPlatformMemZero(extra, sizeof(ExtraResourceRecord));  // OK if oversized rdata not zero'd
    extra->r.resrec.rrtype = rrtype;
    extra->r.rdatastorage.MaxRDLength = (mDNSu16) size;
    extra->r.resrec.rdlength = rdlen;
    memcpy(&extra->r.rdatastorage.u.data, rdata, rdlen);
	
    result =  mDNS_AddRecordToService(&mDNSStorage, srs , extra, &extra->r.rdatastorage, ttl);
	if (result) { freeL("ExtraResourceRecord", extra); return result; }

    extra->ClientID = rstate->hdr.reg_index;
	return result;
	}

mDNSlocal mStatus handle_add_request(request_state *rstate)
    {
    uint32_t ttl;
    uint16_t rrtype, rdlen;
    char *ptr, *rdata;
    mStatus result = mStatus_UnknownErr;
    DNSServiceFlags flags;
	service_info *srvinfo = rstate->service_registration;
	service_instance *i;

	if (!srvinfo) { LogMsg("handle_add_request called with NULL service_registration"); return(-1); }

	ptr = rstate->msgdata;
    flags = get_flags(&ptr);
    rrtype = get_short(&ptr);
    rdlen = get_short(&ptr);
    rdata = get_rdata(&ptr, rdlen);
    ttl = get_long(&ptr);
	
    if (!ttl) ttl = DefaultTTLforRRType(rrtype);

	LogOperation("%3d: DNSServiceAddRecord(%##s, %s)", rstate->sd,
		(srvinfo->instances) ? srvinfo->instances->srs.RR_SRV.resrec.name->c : NULL, DNSTypeName(rrtype));

	for (i = srvinfo->instances; i; i = i->next)
		{
		result = add_record_to_service(rstate, i, rrtype, rdlen, rdata, ttl);
		if (result && i->default_local) break;
		else result = mStatus_NoError;  // suppress non-local default errors
		}
	
	return(result);
    }

mDNSlocal void update_callback(mDNS *const m, AuthRecord *const rr, RData *oldrd)
    {
    (void)m; // Unused
    if (oldrd != &rr->rdatastorage) freeL("update_callback", oldrd);
    }

mDNSlocal mStatus update_record(AuthRecord *rr, uint16_t rdlen, char *rdata, uint32_t ttl)
	{
	int rdsize;
	RData *newrd;
	mStatus result;
	
	if (rdlen > sizeof(RDataBody)) rdsize = rdlen;
    else rdsize = sizeof(RDataBody);
    newrd = mallocL("handle_update_request", sizeof(RData) - sizeof(RDataBody) + rdsize);
    if (!newrd) FatalError("ERROR: malloc");
    newrd->MaxRDLength = (mDNSu16) rdsize;
    memcpy(&newrd->u, rdata, rdlen);

	// BIND named (name daemon) doesn't allow TXT records with zero-length rdata. This is strictly speaking correct,
	// since RFC 1035 specifies a TXT record as "One or more <character-string>s", not "Zero or more <character-string>s".
	// Since some legacy apps try to create zero-length TXT records, we'll silently correct it here.
	if (rr->resrec.rrtype == kDNSType_TXT && rdlen == 0) { rdlen = 1; newrd->u.txt.c[0] = 0; }

    result = mDNS_Update(&mDNSStorage, rr, ttl, rdlen, newrd, update_callback);
	if (result) { LogMsg("ERROR: mDNS_Update - %ld", result); freeL("handle_update_request", newrd); }
	return result;
	}

mDNSlocal mStatus handle_update_request(request_state *rstate)
    {
	uint16_t rdlen;
    char *ptr, *rdata;
    uint32_t ttl;
    mStatus result = mStatus_BadReferenceErr;
	service_info *srvinfo = rstate->service_registration;
	service_instance *i;
	AuthRecord *rr = NULL;

	// get the message data
	ptr = rstate->msgdata;
    get_flags(&ptr);	// flags unused
    rdlen = get_short(&ptr);
    rdata = get_rdata(&ptr, rdlen);
    ttl = get_long(&ptr);

	if (rstate->reg_recs)
		{
		// update an individually registered record
		registered_record_entry *reptr;
		for (reptr = rstate->reg_recs; reptr; reptr = reptr->next)
			{
			if (reptr->key == rstate->hdr.reg_index)
				{
				result = update_record(reptr->rr, rdlen, rdata, ttl);
				goto end;
				}
			}
		result = mStatus_BadReferenceErr;
		goto end;
		}

	// update a record from a service record set
	if (!srvinfo) { result = mStatus_BadReferenceErr;  goto end; }
	for (i = srvinfo->instances; i; i = i->next)
		{
		if (rstate->hdr.reg_index == TXT_RECORD_INDEX) rr = &i->srs.RR_TXT;
		else
			{
			ExtraResourceRecord *e;
			for (e = i->srs.Extras; e; e = e->next)
				if (e->ClientID == rstate->hdr.reg_index) { rr = &e->r; break; }
			}

		if (!rr) { result = mStatus_BadReferenceErr; goto end; }
		result = update_record(rr, rdlen, rdata, ttl);
		if (result && i->default_local) goto end;
		else result = mStatus_NoError;  // suppress non-local default errors
		}

end:
	LogOperation("%3d: DNSServiceUpdateRecord(%##s, %s)", rstate->sd,
		(srvinfo->instances) ? srvinfo->instances->srs.RR_SRV.resrec.name->c : NULL,
		rr ? DNSTypeName(rr->resrec.rrtype) : "<NONE>");

    return(result);
    }
    
// remove a resource record registered via DNSServiceRegisterRecord()
mDNSlocal mStatus remove_record(request_state *rstate)
    {
    int shared;
    mStatus err = mStatus_UnknownErr;
    registered_record_entry *e, **ptr = &rstate->reg_recs;
	
    while(*ptr && (*ptr)->key != rstate->hdr.reg_index) ptr = &(*ptr)->next;
	if (!*ptr) { LogMsg("DNSServiceRemoveRecord - bad reference"); return mStatus_BadReferenceErr; }
	e = *ptr;
	*ptr = e->next; // unlink
	
	LogOperation("%3d: DNSServiceRemoveRecord(%#s)", rstate->sd, e->rr->resrec.name->c);
	shared = e->rr->resrec.RecordType == kDNSRecordTypeShared;
	e->rr->RecordContext = NULL;
	err = mDNS_Deregister(&mDNSStorage, e->rr);
	if (err)
		{
		LogMsg("ERROR: remove_record, mDNS_Deregister: %ld", err);
		freeL("remove_record", e->rr);
		freeL("remove_record", e);
		}
	return err;
    }

mDNSlocal mStatus remove_extra(request_state *rstate, service_instance *serv)
	{
	mStatus err = mStatus_BadReferenceErr;
	ExtraResourceRecord *ptr;

	for (ptr = serv->srs.Extras; ptr; ptr = ptr->next)
		{
		if (ptr->ClientID == rstate->hdr.reg_index) // found match
			return mDNS_RemoveRecordFromService(&mDNSStorage, &serv->srs, ptr, FreeExtraRR, ptr);
		}
	return err;
	}

mDNSlocal mStatus handle_removerecord_request(request_state *rstate)
    {
    mStatus err = mStatus_BadReferenceErr;
    char *ptr;
	service_info *srvinfo = rstate->service_registration;

    ptr = rstate->msgdata;
    get_flags(&ptr);	// flags unused

	if (rstate->reg_recs)  err = remove_record(rstate);  // remove individually registered record
	else if (!srvinfo) LogOperation("%3d: DNSServiceRemoveRecord (bad ref)", rstate->sd);
    else
		{
		service_instance *i;
		LogOperation("%3d: DNSServiceRemoveRecord(%##s)", rstate->sd,
			(srvinfo->instances) ? srvinfo->instances->srs.RR_SRV.resrec.name->c : NULL);
		for (i = srvinfo->instances; i; i = i->next)
			{
			err = remove_extra(rstate, i);
			if (err && i->default_local) break;
			else err = mStatus_NoError;  // suppress non-local default errors
			}
		}
	
    return(err);
    }

mDNSlocal void RemoveDefaultRegDomain(mDNS *const m, domainname *d)
	{
	DNameListElem *ptr = m->DefRegList, *prev = mDNSNULL;

	while (ptr)
		{
		if (SameDomainName(&ptr->name, d))
			{
			if (prev) prev->next = ptr->next;
			else m->DefRegList = ptr->next;
			mDNSPlatformMemFree(ptr);
			mDNSPlatformDefaultRegDomainChanged(d, mDNSfalse);
			return;
			}
		prev = ptr;
		ptr = ptr->next;
		}
	debugf("Requested removal of default registration domain %##s not in contained in list", d->c);
	}

mDNSlocal mStatus register_service_instance(request_state *request, const domainname *domain)
	{
	service_info *info = request->service_registration;
	service_instance *ptr, *instance;
    int instance_size;
	mStatus result;

	for (ptr = info->instances; ptr; ptr = ptr->next)
		{
		if (SameDomainName(&ptr->domain, domain))
			{ LogMsg("register_service_instance: domain %##s already registered", domain->c); return mStatus_AlreadyRegistered; }
		}
	
	instance_size = sizeof(*instance);
	if (info->txtlen > sizeof(RDataBody)) instance_size += (info->txtlen - sizeof(RDataBody));
	instance = mallocL("service_instance", instance_size);
	if (!instance) { my_perror("ERROR: malloc"); return mStatus_NoMemoryErr; }

	instance->subtypes = AllocateSubTypes(info->num_subtypes, info->type_as_string);
	if (info->num_subtypes && !instance->subtypes)
		{ free_service_instance(instance); instance = NULL; FatalError("ERROR: malloc"); }
    instance->request           = request;
	instance->sd                = request->sd;
    instance->autoname          = info->autoname;
    instance->autorename        = info->autorename;
    instance->allowremotequery  = info->allowremotequery;
    instance->renameonmemfree   = 0;
	instance->name              = info->name;
	AssignDomainName(&instance->domain, domain);
	instance->default_local = (info->default_domain && SameDomainName(domain, &localdomain));
    result = mDNS_RegisterService(&mDNSStorage, &instance->srs, &instance->name, &info->type, domain, info->host.c[0] ? &info->host : NULL, info->port,
								  info->txtdata, info->txtlen, instance->subtypes, info->num_subtypes, info->InterfaceID, regservice_callback, instance);

	if (result) free_service_instance(instance);
	else
		{
		instance->next = info->instances;
		info->instances = instance;
		}
	return result;
	}

mDNSlocal void regservice_termination_callback(void *context)
    {
	service_info *info = context;
	service_instance *i, *p;
	if (!info) { LogMsg("regservice_termination_callback context is NULL"); return; }
	if (!info->request) { LogMsg("regservice_termination_callback info->request is NULL"); return; }
	i = info->instances;
	while (i)
		{
		p = i;
		i = i->next;
		p->request = NULL;  // clear back pointer
		// only safe to free memory if registration is not valid, i.e. deregister fails (which invalidates p)
		LogOperation("%3d: DNSServiceRegister(%##s, %u) STOP", info->request->sd, p->srs.RR_SRV.resrec.name->c, mDNSVal16(p->srs.RR_SRV.resrec.rdata->u.srv.port));
		if (mDNS_DeregisterService(&mDNSStorage, &p->srs)) free_service_instance(p);
		}
	info->request->service_registration = NULL; // clear pointer from request back to info
	if (info->txtdata) { freeL("txtdata", info->txtdata); info->txtdata = NULL; }
	freeL("service_info", info);
	}

mDNSexport void udsserver_default_reg_domain_changed(const domainname *d, mDNSBool add)
	{
	request_state *rstate;
	service_info *info;

	LogMsg("%s registration domain %##s", add ? "Adding" : "Removing", d->c);
	for (rstate = all_requests; rstate; rstate = rstate->next)
		{
		if (rstate->terminate != regservice_termination_callback) continue;
		info = rstate->service_registration;
		if (!info) { LogMsg("udsserver_default_reg_domain_changed - NULL service info"); continue; } // this should never happen
		if (!info->default_domain)  continue;

		// valid default registration
		if (add) register_service_instance(rstate, d);
		else
			{
			// find the instance to remove
			service_instance *si = rstate->service_registration->instances, *prev = NULL;
			while (si)
				{
				if (SameDomainName(&si->domain, d))
					{
					mStatus err;
					if (prev) prev->next = si->next;
					else info->instances = si->next;
					err = mDNS_DeregisterService(&mDNSStorage, &si->srs);
					if (err)
						{
						LogMsg("udsserver_default_reg_domain_changed - mDNS_DeregisterService err %d", err);
						free_service_instance(si);
						}
					break;
					}
				prev = si;
				si = si->next;
				}
			if (!si) debugf("udsserver_default_reg_domain_changed - domain %##s not registered", d->c); // normal if registration failed
			}
		}
	}

mDNSlocal void handle_regservice_request(request_state *request)
    {
    DNSServiceFlags flags;
    uint32_t ifi;
    char name[1024];	// Lots of spare space for extra-long names that we'll auto-truncate down to 63 bytes
    char domain[MAX_ESCAPED_DOMAIN_NAME], host[MAX_ESCAPED_DOMAIN_NAME];
    char *ptr;
    domainname d, srv;
    mStatus result;
	service_info *service = NULL;
	
	if (request->ts != t_complete)
        {
        LogMsg("ERROR: handle_regservice_request - transfer state != t_complete");
        abort_request(request);
        unlink_request(request);
        return;
        }

	service = mallocL("service_info", sizeof(*service));
	if (!service) { my_perror("ERROR: malloc"); result = mStatus_NoMemoryErr; goto finish; }

	service->instances = NULL;
	service->request = request;
	service->txtlen  = 0;
	service->txtdata = NULL;
	request->service_registration = service;
    request->termination_context = request->service_registration;
    request->terminate = regservice_termination_callback;
	
    // extract data from message
    ptr = request->msgdata;
    flags = get_flags(&ptr);
    ifi = get_long(&ptr);
    service->InterfaceID = mDNSPlatformInterfaceIDfromInterfaceIndex(&mDNSStorage, ifi);
    if (ifi && !service->InterfaceID)
    	{ LogMsg("ERROR: handle_regservice_request - Couldn't find InterfaceID for interfaceIndex %d", ifi); goto bad_param; }
    if (get_string(&ptr, name, sizeof(name)) < 0 ||
        get_string(&ptr, service->type_as_string, MAX_ESCAPED_DOMAIN_NAME) < 0 ||
        get_string(&ptr, domain, MAX_ESCAPED_DOMAIN_NAME) < 0 ||
        get_string(&ptr, host, MAX_ESCAPED_DOMAIN_NAME) < 0)
    	{ LogMsg("ERROR: handle_regservice_request - Couldn't read name/regtype/domain"); goto bad_param; }
        
	service->port.b[0] = *ptr++;
	service->port.b[1] = *ptr++;

    service->txtlen  = get_short(&ptr);
	if (service->txtlen)
		{
		service->txtdata = mallocL("txtdata", service->txtlen);
		if (!service->txtdata) { my_perror("ERROR: malloc"); result = mStatus_NoMemoryErr; goto finish; }
		memcpy(service->txtdata, get_rdata(&ptr, service->txtlen), service->txtlen);
		}
	else service->txtdata = NULL;

	// Check for sub-types after the service type
	service->num_subtypes = ChopSubTypes(service->type_as_string);	// Note: Modifies regtype string to remove trailing subtypes
	if (service->num_subtypes < 0)
    	{ LogMsg("ERROR: handle_regservice_request - ChopSubTypes failed %s", service->type_as_string); goto bad_param; }

	// Don't try to construct "domainname t" until *after* ChopSubTypes has worked its magic
    if (!*service->type_as_string || !MakeDomainNameFromDNSNameString(&service->type, service->type_as_string))
    	{ LogMsg("ERROR: handle_regservice_request - service->type_as_string bad %s", service->type_as_string); goto bad_param; }

    if (!name[0])
		{
		service->name = (&mDNSStorage)->nicelabel;
		service->autoname = mDNStrue;
		}
    else
		{
		// If the client is allowing AutoRename, then truncate name to legal length before converting it to a DomainLabel
		if ((flags & kDNSServiceFlagsNoAutoRename) == 0)
			{
			int newlen = TruncateUTF8ToLength((mDNSu8*)name, mDNSPlatformStrLen(name), MAX_DOMAIN_LABEL);
			name[newlen] = 0;
			}
		if (!MakeDomainLabelFromLiteralString(&service->name, name))
			{ LogMsg("ERROR: handle_regservice_request - name bad %s", name); goto bad_param; }
		service->autoname = mDNSfalse;
		}
        	
	if (*domain)
		{
		service->default_domain = mDNSfalse;
		if (!MakeDomainNameFromDNSNameString(&d, domain))
			{ LogMsg("ERROR: handle_regservice_request - domain bad %s", domain); goto bad_param; }
		}
	else
		{
		service->default_domain = mDNStrue;
		MakeDomainNameFromDNSNameString(&d, "local.");
		}
	
	if (!ConstructServiceName(&srv, &service->name, &service->type, &d))
		{ LogMsg("ERROR: handle_regservice_request - Couldn't ConstructServiceName from, “%#s” “%##s” “%##s”", service->name.c, service->type.c, d.c); goto bad_param; }
		
	if (!MakeDomainNameFromDNSNameString(&service->host, host))
		{ LogMsg("ERROR: handle_regservice_request - host bad %s", host); goto bad_param; }
	service->autorename       = (flags & kDNSServiceFlagsNoAutoRename    ) == 0;
	service->allowremotequery = (flags & kDNSServiceFlagsAllowRemoteQuery) != 0;
	
	// Some clients use mDNS for lightweight copy protection, registering a pseudo-service with
	// a port number of zero. When two instances of the protected client are allowed to run on one
	// machine, we don't want to see misleading "Bogus client" messages in syslog and the console.
	if (service->port.NotAnInteger)
		{
		int count = CountExistingRegistrations(&srv, service->port);
		if (count)
			LogMsg("Client application registered %d identical instances of service %##s port %u.",
				count+1, srv.c, mDNSVal16(service->port));
		}

	LogOperation("%3d: DNSServiceRegister(\"%s\", \"%s\", \"%s\", \"%s\", %u) START",
		request->sd, name, service->type_as_string, domain, host, mDNSVal16(service->port));
	result = register_service_instance(request, &d);
	
	if (!result && !*domain)
		{
		DNameListElem *ptr;
		for (ptr = mDNSStorage.DefRegList; ptr; ptr = ptr->next)
			register_service_instance(request, &ptr->name);
		    // note that we don't report errors for non-local, non-explicit domains
		}
	
finish:
    deliver_error(request, result);
    if (result != mStatus_NoError)
        {
        abort_request(request);
        unlink_request(request);
        }
    else
        reset_connected_rstate(request);  // prepare to receive add/remove messages

    return;

bad_param:
	//if (service) freeL("service_info", service);	Don't think we should do this -- abort_request will free it a second time and crash
    deliver_error(request, mStatus_BadParamErr);
    abort_request(request);
    unlink_request(request);
    }

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - Handle DNSServiceBrowse Requests
#endif

static ARListElem *SCPrefBrowseDomains  = mDNSNULL;  // manually generated local-only PTR records for browse domains we get from SCPreferences

mDNSlocal void FoundInstance(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, mDNSBool AddRecord)
	{
	request_state *req = question->QuestionContext;
	reply_state *rep;
	(void)m; // Unused

	if (answer->rrtype != kDNSType_PTR)
		{ LogMsg("%3d: FoundInstance: Should not be called with rrtype %d (not a PTR record)", req->sd, answer->rrtype); return; }

	if (GenerateNTDResponse(&answer->rdata->u.name, answer->InterfaceID, req, &rep) != mStatus_NoError)
		{
		LogMsg("%3d: FoundInstance: %##s PTR %##s received from network is not valid DNS-SD service pointer",
			req->sd, answer->name->c, answer->rdata->u.name.c);
		return;
		}

	LogOperation("%3d: DNSServiceBrowse(%##s, %s) RESULT %s %s",
		req->sd, question->qname.c, DNSTypeName(question->qtype), AddRecord ? "Add" : "Rmv", RRDisplayString(m, answer));

	if (AddRecord) rep->rhdr->flags |= dnssd_htonl(kDNSServiceFlagsAdd);
	append_reply(req, rep);
	}

mDNSlocal mStatus add_domain_to_browser(browser_info_t *info, const domainname *d)
	{
	browser_t *b, *p;
	mStatus err;

	for (p = info->browsers; p; p = p->next)
		{
		if (SameDomainName(&p->domain, d))
			{ debugf("add_domain_to_browser - attempt to add domain %##d already in list", d->c); return mStatus_AlreadyRegistered; }
		}

	b = mallocL("browser_t", sizeof(*b));
	if (!b) return mStatus_NoMemoryErr;
	AssignDomainName(&b->domain, d);
	err = mDNS_StartBrowse(&mDNSStorage, &b->q, &info->regtype, d, info->interface_id, info->ForceMCast, FoundInstance, info->rstate);
	if (err)
		{
		LogMsg("mDNS_StartBrowse returned %d for type %##s domain %##s", err, info->regtype.c, d->c);
		freeL("browser_t", b);
		}
	else
		{
		b->next = info->browsers;
		info->browsers = b;
		}
		return err;
	}

mDNSlocal void udsserver_automatic_browse_domain_changed(const domainname *d, mDNSBool add)
	{
	request_state *r;
	debugf("DefaultBrowseDomainChanged: %s default browse domain %##s", add ? "Adding" : "Removing", d->c);

#if APPLE_OSX_mDNSResponder
	extern void machserver_automatic_browse_domain_changed(const domainname *d, mDNSBool add);
	machserver_automatic_browse_domain_changed(d, add);
#endif // APPLE_OSX_mDNSResponder
  	
  	for (r = all_requests; r; r = r->next)
		{
		browser_info_t *info = r->browser_info;
		
		if (!info || !info->default_domain) continue;
		if (add) add_domain_to_browser(info, d);
		else
			{
			browser_t **ptr = &info->browsers;
			while (*ptr)
				{
				if (SameDomainName(&(*ptr)->domain, d))
					{
					browser_t *remove = *ptr;
					*ptr = (*ptr)->next;
					mDNS_StopQueryWithRemoves(&mDNSStorage, &remove->q);
					freeL("browser_t", remove);
					return;
					}
				ptr = &(*ptr)->next;
				}
			LogMsg("Requested removal of default domain %##s not in list for sd %d", d->c, r->sd);
			}
		}
	}

mDNSlocal void FreeARElemCallback(mDNS *const m, AuthRecord *const rr, mStatus result)
	{
	(void)m;  // unused
	if (result == mStatus_MemFree) mDNSPlatformMemFree(rr->RecordContext);
	}

mDNSlocal void RegisterBrowseDomainPTR(mDNS *m, const domainname *d, int type)
	{
	// allocate/register legacy and non-legacy _browse PTR record
	mStatus err;
	ARListElem *browse = mDNSPlatformMemAllocate(sizeof(*browse));
	mDNS_SetupResourceRecord(&browse->ar, mDNSNULL, mDNSInterface_LocalOnly, kDNSType_PTR, 7200,  kDNSRecordTypeShared, FreeARElemCallback, browse);
	MakeDomainNameFromDNSNameString(browse->ar.resrec.name, mDNS_DomainTypeNames[type]);
	AppendDNSNameString            (browse->ar.resrec.name, "local");
	AssignDomainName(&browse->ar.resrec.rdata->u.name, d);
	err = mDNS_Register(m, &browse->ar);
	if (err)
		{
		LogMsg("SetSCPrefsBrowseDomain: mDNS_Register returned error %d", err);
		mDNSPlatformMemFree(browse);
		}
	else
		{
		browse->next = SCPrefBrowseDomains;
		SCPrefBrowseDomains = browse;
		}
	}

mDNSlocal void DeregisterBrowseDomainPTR(mDNS *m, const domainname *d, int type)
	{
	ARListElem *remove, **ptr = &SCPrefBrowseDomains;
	domainname lhs; // left-hand side of PTR, for comparison

	MakeDomainNameFromDNSNameString(&lhs, mDNS_DomainTypeNames[type]);
	AppendDNSNameString            (&lhs, "local");

	while (*ptr)
		{
		if (SameDomainName(&(*ptr)->ar.resrec.rdata->u.name, d) && SameDomainName((*ptr)->ar.resrec.name, &lhs))
			{
			remove = *ptr;
			*ptr = (*ptr)->next;
			mDNS_Deregister(m, &remove->ar);
			return;
			}
		else ptr = &(*ptr)->next;
		}
	}

mDNSlocal void SetPrefsBrowseDomain(mDNS *m, const domainname *d, mDNSBool add)
	{
	LogOperation("SetPrefsBrowseDomain: %s default browse domain refcount for %##s", add ? "Incrementing" : "Decrementing", d->c);

	if (add)
		{
		RegisterBrowseDomainPTR(m, d, mDNS_DomainTypeBrowse);
		RegisterBrowseDomainPTR(m, d, mDNS_DomainTypeBrowseLegacy);
		}
	else
		{
		DeregisterBrowseDomainPTR(m, d, mDNS_DomainTypeBrowse);
		DeregisterBrowseDomainPTR(m, d, mDNS_DomainTypeBrowseLegacy);
		}
	}

mDNSlocal void SetPrefsBrowseDomains(mDNS *m, DNameListElem * browseDomains, mDNSBool add)
	{
	DNameListElem * browseDomain;
	for (browseDomain = browseDomains; browseDomain; browseDomain = browseDomain->next)
		if (browseDomain->name.c[0]) SetPrefsBrowseDomain(m, &browseDomain->name, add);
		else LogMsg("SetPrefsBrowseDomains bad browse domain: %p", browseDomain);
	}

mDNSexport void udsserver_handle_configchange(mDNS *const m)
    {
    request_state *req;
	domainname      regDomain;
	DNameListElem * browseDomains;

    for (req = all_requests; req; req = req->next)
        {
		if (req->service_registration)
			{
			service_instance *ptr;
			for (ptr = req->service_registration->instances; ptr; ptr = ptr->next)
				rename_service(ptr);
			}
		}

	// Let the platform layer get the current DNS information
	mDNSPlatformGetDNSConfig(m, mDNSNULL, &regDomain, &browseDomains);

	// Did our registration domain change?
	if (!SameDomainName(&regDomain, &m->RegDomain))
		{
		if (m->RegDomain.c[0])
			{
			RemoveDefaultRegDomain(m, &m->RegDomain);
			SetPrefsBrowseDomain(m, &m->RegDomain, mDNSfalse); // if we were automatically browsing in our registration domain, stop
			}

		AssignDomainName(&m->RegDomain, &regDomain);

		if (m->RegDomain.c[0])
			{
			AddDefaultRegDomain(m, &m->RegDomain);
			SetPrefsBrowseDomain(m, &m->RegDomain, mDNStrue);
			}
		}

	// Add new browse domains to internal list
	if (browseDomains)
		{
		SetPrefsBrowseDomains(m, browseDomains, mDNStrue);
		}

	// Remove old browse domains from internal list
	if (m->BrowseDomains)
		{
		SetPrefsBrowseDomains(m, m->BrowseDomains, mDNSfalse);
		mDNS_FreeDNameList(m->BrowseDomains);
		}

	// Replace the old browse domains array with the new array
	m->BrowseDomains = browseDomains;
    }

mDNSlocal void LegacyBrowseDomainChange(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, mDNSBool AddRecord)
	{
	DNameListElem *ptr, *prev, *new;
	(void)m; // unused;
	(void)question;  // unused

	LogOperation("LegacyBrowseDomainChange: %s default browse domain %##s", AddRecord ? "Adding" : "Removing", answer->rdata->u.name.c);

	if (AddRecord)
		{
		new = mDNSPlatformMemAllocate(sizeof(DNameListElem));
		if (!new) { LogMsg("ERROR: malloc"); return; }
		AssignDomainName(&new->name, &answer->rdata->u.name);
		new->next = m->DefBrowseList;
		m->DefBrowseList = new;
		udsserver_automatic_browse_domain_changed(&new->name, mDNStrue);
		return;
		}
	else
		{
		ptr = m->DefBrowseList;
		prev = mDNSNULL;
		while (ptr)
			{
			if (SameDomainName(&ptr->name, &answer->rdata->u.name))
				{
				udsserver_automatic_browse_domain_changed(&ptr->name, mDNSfalse);
				if (prev) prev->next = ptr->next;
				else m->DefBrowseList = ptr->next;
				mDNSPlatformMemFree(ptr);
				return;
				}
			prev = ptr;
			ptr = ptr->next;
			}
		LogMsg("LegacyBrowseDomainChange: Got remove event for domain %##s not in list", answer->rdata->u.name.c);
		}
	}

mDNSlocal void TrackLegacyBrowseDomains(mDNS *const m)
	{
	static DNSQuestion LegacyBrowseDomainQ;              // our local enumeration query for _legacy._browse domains

	// start query for domains to be used in default (empty string domain) browses
	mStatus err = mDNS_GetDomains(m, &LegacyBrowseDomainQ, mDNS_DomainTypeBrowseLegacy, mDNSNULL, mDNSInterface_LocalOnly, LegacyBrowseDomainChange, mDNSNULL);

	// provide .local automatically
	SetPrefsBrowseDomain(m, &localdomain, mDNStrue);

	// <rdar://problem/4055653> dns-sd -E does not return "local."
	// register registration domain "local"
	RegisterBrowseDomainPTR(m, &localdomain, mDNS_DomainTypeRegistration);
	if (err) LogMsg("ERROR: dDNS_InitDNSConfig - mDNS_Register returned error %d", err);
	}

mDNSlocal void browse_termination_callback(void *context)
    {
	browser_info_t *info = context;
	browser_t *ptr;
	
	if (!info) return;

	while(info->browsers)
		{
		ptr = info->browsers;
		info->browsers = ptr->next;
		LogOperation("%3d: DNSServiceBrowse(%##s) STOP", info->rstate->sd, ptr->q.qname.c);
		mDNS_StopBrowse(&mDNSStorage, &ptr->q);  // no need to error-check result
		freeL("browse_termination_callback", ptr);
		}
	
	info->rstate->termination_context = NULL;
	freeL("browser_info", info);
	}

mDNSlocal void handle_browse_request(request_state *request)
    {
    DNSServiceFlags flags;
    uint32_t interfaceIndex;
    mDNSInterfaceID InterfaceID;
    char regtype[MAX_ESCAPED_DOMAIN_NAME], domain[MAX_ESCAPED_DOMAIN_NAME];
    domainname typedn, d, temp;
    mDNSs32 NumSubTypes;
    char *ptr;
    mStatus err = mStatus_NoError;
	browser_info_t *info = NULL;
	
    if (request->ts != t_complete)
        {
        LogMsg("ERROR: handle_browse_request - transfer state != t_complete");
        abort_request(request);
        unlink_request(request);
        return;
        }

    // extract data from message
    ptr = request->msgdata;
    flags = get_flags(&ptr);
    interfaceIndex = get_long(&ptr);
    if (get_string(&ptr, regtype, MAX_ESCAPED_DOMAIN_NAME) < 0 ||
        get_string(&ptr, domain, MAX_ESCAPED_DOMAIN_NAME) < 0)
		{ err = mStatus_BadParamErr;  goto error; }
    freeL("handle_browse_request", request->msgbuf);
    request->msgbuf = NULL;

    InterfaceID = mDNSPlatformInterfaceIDfromInterfaceIndex(&mDNSStorage, interfaceIndex);
    if (interfaceIndex && !InterfaceID) { err = mStatus_BadParamErr;  goto error; }

	if (!domain || (domain[0] == '\0'))
		{
		uDNS_RegisterSearchDomains(&mDNSStorage);
		}
		
	typedn.c[0] = 0;
	NumSubTypes = ChopSubTypes(regtype);	// Note: Modifies regtype string to remove trailing subtypes
	if (NumSubTypes < 0 || NumSubTypes > 1) { err = mStatus_BadParamErr;  goto error; }
	if (NumSubTypes == 1 && !AppendDNSNameString(&typedn, regtype + strlen(regtype) + 1))
		{ err = mStatus_BadParamErr;  goto error; }

    if (!regtype[0] || !AppendDNSNameString(&typedn, regtype)) { err = mStatus_BadParamErr;  goto error; }

	if (!MakeDomainNameFromDNSNameString(&temp, regtype)) { err = mStatus_BadParamErr;  goto error; }
	if (temp.c[0] > 15 && domain[0] == 0) strcpy(domain, "local."); // For over-long service types, we only allow domain "local"

	// allocate and set up browser info
	info = mallocL("browser_info_t", sizeof(*info));
	if (!info) { err = mStatus_NoMemoryErr; goto error; }

	request->browser_info = info;
	info->ForceMCast = (flags & kDNSServiceFlagsForceMulticast) != 0;
	info->interface_id = InterfaceID;
	AssignDomainName(&info->regtype, &typedn);
	info->rstate = request;
	info->default_domain = !domain[0];
	info->browsers = NULL;

	// setup termination context
	request->termination_context = info;
    request->terminate = browse_termination_callback;
	
	LogOperation("%3d: DNSServiceBrowse(\"%##s\", \"%s\") START", request->sd, info->regtype.c, domain);
	if (domain[0])
		{
		if (!MakeDomainNameFromDNSNameString(&d, domain)) { err = mStatus_BadParamErr;  goto error; }
		err = add_domain_to_browser(info, &d);
		}

	else
		{
		DNameListElem *sdom;
		for (sdom = mDNSStorage.DefBrowseList; sdom; sdom = sdom->next)
			{
			err = add_domain_to_browser(info, &sdom->name);
			if (err)
				{
				if (SameDomainName(&sdom->name, &localdomain)) break;
				else err = mStatus_NoError;  // suppress errors for non-local "default" domains
				}
			}
		}
		
	deliver_error(request, err);
	return;
    
error:
	if (info) freeL("browser_info_t", info);
	if (request->termination_context) request->termination_context = NULL;
    deliver_error(request, err);
    abort_request(request);
    unlink_request(request);
    }

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - Handle DNSServiceResolve Requests
#endif

mDNSlocal void resolve_result_callback(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, mDNSBool AddRecord)
	{
	size_t len = 0;
	char fullname[MAX_ESCAPED_DOMAIN_NAME], target[MAX_ESCAPED_DOMAIN_NAME];
	char *data;
	transfer_state result;
	reply_state *rep;
	request_state *rs = question->QuestionContext;
	resolve_termination_t *res = rs->termination_context;
	(void)m; // Unused

	LogOperation("%3d: DNSServiceResolve(%##s, %s) %s %s",
		rs->sd, question->qname.c, DNSTypeName(question->qtype), AddRecord ? "ADD" : "RMV", RRDisplayString(m, answer));
	
	// This code used to do this trick of just keeping a copy of the pointer to
	// the answer record in the cache, but the unicast query code doesn't currently
	// put its answer records in the cache, so for now we can't do this.
	
	if (!AddRecord)
		{
		if (answer->rrtype == kDNSType_SRV && res->srv == answer) res->srv = mDNSNULL;
		if (answer->rrtype == kDNSType_TXT && res->txt == answer) res->txt = mDNSNULL;
		return;
		}

	if (answer->rrtype == kDNSType_SRV) res->srv = answer;
	if (answer->rrtype == kDNSType_TXT) res->txt = answer;

	if (!res->txt || !res->srv) return;		// only deliver result to client if we have both answers
	
	ConvertDomainNameToCString(answer->name, fullname);
	ConvertDomainNameToCString(&res->srv->rdata->u.srv.target, target);

	// calculate reply length
	len += sizeof(DNSServiceFlags);
	len += sizeof(uint32_t);  // interface index
	len += sizeof(DNSServiceErrorType);
	len += strlen(fullname) + 1;
	len += strlen(target) + 1;
	len += 2 * sizeof(uint16_t);  // port, txtLen
	len += res->txt->rdlength;
	
	// allocate/init reply header
	rep =  create_reply(resolve_reply_op, len, rs);
	rep->rhdr->flags = dnssd_htonl(0);
	rep->rhdr->ifi   = dnssd_htonl(mDNSPlatformInterfaceIndexfromInterfaceID(&mDNSStorage, answer->InterfaceID));
	rep->rhdr->error = dnssd_htonl(kDNSServiceErr_NoError);

	data = rep->sdata;
	
	// write reply data to message
	put_string(fullname, &data);
	put_string(target, &data);
	*data++ = res->srv->rdata->u.srv.port.b[0];
	*data++ = res->srv->rdata->u.srv.port.b[1];
	put_short(res->txt->rdlength, &data);
	put_rdata(res->txt->rdlength, res->txt->rdata->u.data, &data);
	
	result = send_msg(rep);
	if (result == t_error || result == t_terminated)
		{
		abort_request(rs);
		unlink_request(rs);
		freeL("resolve_result_callback", rep);
		}
	else if (result == t_complete) freeL("resolve_result_callback", rep);
	else append_reply(rs, rep);
	}

mDNSlocal void resolve_termination_callback(void *context)
    {
    resolve_termination_t *term = context;
    request_state *rs;
    
    if (!term)
        {
        LogMsg("ERROR: resolve_termination_callback: double termination");
        return;
        }
    rs = term->rstate;
	LogOperation("%3d: DNSServiceResolve(%##s) STOP", rs->sd, term->qtxt.qname.c);
    
    mDNS_StopQuery(&mDNSStorage, &term->qtxt);
    mDNS_StopQuery(&mDNSStorage, &term->qsrv);
    
    freeL("resolve_termination_callback", term);
    rs->termination_context = NULL;
    }

mDNSlocal void handle_resolve_request(request_state *rstate)
    {
    DNSServiceFlags flags;
    uint32_t interfaceIndex;
    mDNSInterfaceID InterfaceID;
    char name[256], regtype[MAX_ESCAPED_DOMAIN_NAME], domain[MAX_ESCAPED_DOMAIN_NAME];
    char *ptr;  // message data pointer
    domainname fqdn;
    resolve_termination_t *term;
    mStatus err;
    
    if (rstate->ts != t_complete)
        {
        LogMsg("ERROR: handle_resolve_request - transfer state != t_complete");
        abort_request(rstate);
        unlink_request(rstate);
        return;
        }
        
    // extract the data from the message
    ptr = rstate->msgdata;
    if (!ptr)
        {
        LogMsg("ERROR: handle_resolve_request - NULL msgdata");
        abort_request(rstate);
        unlink_request(rstate);
        return;
        }
    flags = get_flags(&ptr);
    interfaceIndex = get_long(&ptr);
    InterfaceID = mDNSPlatformInterfaceIDfromInterfaceIndex(&mDNSStorage, interfaceIndex);
    if (interfaceIndex && !InterfaceID)
    	{ LogMsg("ERROR: handle_resolve_request - Couldn't find InterfaceID for interfaceIndex %d", interfaceIndex); goto bad_param; }
    if (get_string(&ptr, name, 256) < 0 ||
        get_string(&ptr, regtype, MAX_ESCAPED_DOMAIN_NAME) < 0 ||
        get_string(&ptr, domain, MAX_ESCAPED_DOMAIN_NAME) < 0)
    	{ LogMsg("ERROR: handle_resolve_request - Couldn't read name/regtype/domain"); goto bad_param; }

    // free memory in rstate since we don't need it anymore
    freeL("handle_resolve_request", rstate->msgbuf);
    rstate->msgbuf = NULL;

    if (build_domainname_from_strings(&fqdn, name, regtype, domain) < 0)
    	{ LogMsg("ERROR: handle_resolve_request - Couldn't build_domainname_from_strings “%s” “%s” “%s”", name, regtype, domain); goto bad_param; }

    // set up termination info
    term = mallocL("handle_resolve_request", sizeof(resolve_termination_t));
    mDNSPlatformMemZero(term, sizeof(*term));
    if (!term) FatalError("ERROR: malloc");

    // format questions
    term->qsrv.InterfaceID      = InterfaceID;
    term->qsrv.Target           = zeroAddr;
    memcpy(&term->qsrv.qname, &fqdn, MAX_DOMAIN_NAME);
    term->qsrv.qtype            = kDNSType_SRV;
    term->qsrv.qclass           = kDNSClass_IN;
    term->qsrv.LongLived        = mDNSfalse;
    term->qsrv.ExpectUnique     = mDNStrue;
	term->qsrv.ForceMCast       = mDNSfalse;
	term->qsrv.ReturnIntermed   = mDNSfalse;
    term->qsrv.QuestionCallback = resolve_result_callback;
    term->qsrv.QuestionContext  = rstate;
    
    term->qtxt.InterfaceID      = InterfaceID;
    term->qtxt.Target           = zeroAddr;
    memcpy(&term->qtxt.qname, &fqdn, MAX_DOMAIN_NAME);
    term->qtxt.qtype            = kDNSType_TXT;
    term->qtxt.qclass           = kDNSClass_IN;
    term->qtxt.LongLived        = mDNSfalse;
    term->qtxt.ExpectUnique     = mDNStrue;
	term->qtxt.ForceMCast       = mDNSfalse;
	term->qtxt.ReturnIntermed   = mDNSfalse;
    term->qtxt.QuestionCallback = resolve_result_callback;
    term->qtxt.QuestionContext  = rstate;

    term->rstate = rstate;
    rstate->termination_context = term;
    rstate->terminate = resolve_termination_callback;
    
    // ask the questions
	LogOperation("%3d: DNSServiceResolve(%##s) START", rstate->sd, term->qsrv.qname.c);
    err = mDNS_StartQuery(&mDNSStorage, &term->qsrv);
    if (!err) err = mDNS_StartQuery(&mDNSStorage, &term->qtxt);

    if (err)
        {
        freeL("handle_resolve_request", term);
        rstate->terminate = NULL;  // prevent abort_request() from invoking termination callback
        }
    if (deliver_error(rstate, err) < 0 || err)
        {
        abort_request(rstate);
        unlink_request(rstate);
        }
    return;

bad_param:
    deliver_error(rstate, mStatus_BadParamErr);
    abort_request(rstate);
    unlink_request(rstate);
    }
    
// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - Handle DNSServiceQueryRecord Requests
#endif

// mDNS operation functions.  Each operation has 3 associated functions - a request handler that parses
// the client's request and makes the appropriate mDNSCore call, a result handler (passed as a callback
// to the mDNSCore routine) that sends results back to the client, and a termination routine that aborts
// the mDNSCore operation if the client dies or closes its socket.

// query and resolve calls have separate request handlers that parse the arguments from the client and
// massage the name parameters appropriately, but the rest of the operations (making the query call,
// delivering the result to the client, and termination) are identical.

// what gets called when a resolve is completed and we need to send the data back to the client
mDNSlocal void queryrecord_result_callback(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, mDNSBool AddRecord)
    {
    char *data;
    char name[MAX_ESCAPED_DOMAIN_NAME];
    request_state *req = question->QuestionContext;
    reply_state *rep;
    size_t len;
    (void)m; // Unused

	LogOperation("%3d: DNSServiceQueryRecord(%##s, %s) RESULT %s", req->sd, question->qname.c, DNSTypeName(question->qtype), RRDisplayString(m, answer));
    //mDNS_StopQuery(m, question);
    
	if (answer->rdlength == 0)
		{
		deliver_async_error(req, query_reply_op, kDNSServiceErr_NoSuchRecord);
		return;
		}
    
    // calculate reply data length
    len = sizeof(DNSServiceFlags);
    len += 2 * sizeof(uint32_t);  // if index + ttl
    len += sizeof(DNSServiceErrorType);
    len += 3 * sizeof(uint16_t); // type, class, rdlen
    len += answer->rdlength;
    ConvertDomainNameToCString(answer->name, name);
    len += strlen(name) + 1;
    
    rep =  create_reply(query_reply_op, len, req);

    rep->rhdr->flags = dnssd_htonl(AddRecord ? kDNSServiceFlagsAdd : 0);
    rep->rhdr->ifi   = dnssd_htonl(mDNSPlatformInterfaceIndexfromInterfaceID(&mDNSStorage, answer->InterfaceID));
    rep->rhdr->error = dnssd_htonl(kDNSServiceErr_NoError);

    data = rep->sdata;
    
    put_string(name, &data);
    put_short(answer->rrtype, &data);
    put_short(answer->rrclass, &data);
    put_short(answer->rdlength, &data);
    put_rdata(answer->rdlength, answer->rdata->u.data, &data);
    put_long(AddRecord ? answer->rroriginalttl : 0, &data);

    append_reply(req, rep);
    return;
    }

mDNSlocal void queryrecord_termination_callback(void *context)
    {
    DNSQuestion *q = context;
	LogOperation("%3d: DNSServiceQueryRecord(%##s, %s) STOP", ((request_state *)q->QuestionContext)->sd, q->qname.c, DNSTypeName(q->qtype));
    mDNS_StopQuery(&mDNSStorage, q);  // no need to error check
    freeL("queryrecord_termination_callback", q);
    }

mDNSlocal void handle_queryrecord_request(request_state *rstate)
    {
    DNSServiceFlags flags;
    uint32_t ifi;
    char name[256];
    uint16_t rrtype, rrclass;
    char *ptr;
    mStatus result;
    mDNSInterfaceID InterfaceID;
	DNSQuestion *q;
	
    if (rstate->ts != t_complete)
        {
        LogMsg("ERROR: handle_queryrecord_request - transfer state != t_complete");
        goto error;
        }
    ptr = rstate->msgdata;
    if (!ptr)
        {
        LogMsg("ERROR: handle_queryrecord_request - NULL msgdata");
        goto error;
        }
	
    flags = get_flags(&ptr);
    ifi = get_long(&ptr);
    if (get_string(&ptr, name, 256) < 0) goto bad_param;
    rrtype = get_short(&ptr);
    rrclass = get_short(&ptr);
	InterfaceID = mDNSPlatformInterfaceIDfromInterfaceIndex(&mDNSStorage, ifi);
    if (ifi && !InterfaceID) goto bad_param;

    q = mallocL("DNSQuestion", sizeof(DNSQuestion));
    if (!q) FatalError("ERROR: handle_query - malloc");
    mDNSPlatformMemZero(q, sizeof(DNSQuestion));

    q->InterfaceID      = InterfaceID;
    q->Target           = zeroAddr;
    if (!MakeDomainNameFromDNSNameString(&q->qname, name)) { freeL("DNSQuestion", q); goto bad_param; }
    q->qtype            = rrtype;
    q->qclass           = rrclass;
    q->LongLived        = (flags & kDNSServiceFlagsLongLivedQuery     ) != 0;
    q->ExpectUnique     = mDNSfalse;
    q->ForceMCast       = (flags & kDNSServiceFlagsForceMulticast     ) != 0;
    q->ReturnIntermed   = (flags & kDNSServiceFlagsReturnIntermediates) != 0;
    q->QuestionCallback = queryrecord_result_callback;
    q->QuestionContext  = rstate;

    rstate->termination_context = q;
    rstate->terminate = queryrecord_termination_callback;

	LogOperation("%3d: DNSServiceQueryRecord(%##s, %s) START", rstate->sd, q->qname.c, DNSTypeName(q->qtype));
    result = mDNS_StartQuery(&mDNSStorage, q);
    if (result != mStatus_NoError) LogMsg("ERROR: mDNS_StartQuery: %d", (int)result);
	
    if (result) rstate->terminate = NULL;
    if (deliver_error(rstate, result) < 0) goto error;
    return;
    
bad_param:
    deliver_error(rstate, mStatus_BadParamErr);
    rstate->terminate = NULL;	// don't try to terminate insuccessful Core calls
error:
    abort_request(rstate);
    unlink_request(rstate);
    return;
    }

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - Main Request Handler & Misc
#endif

mDNSlocal reply_state *format_enumeration_reply(request_state *rstate, const char *domain, DNSServiceFlags flags, uint32_t ifi, DNSServiceErrorType err)
    {
    size_t len;
    reply_state *reply;
    char *data;

    len = sizeof(DNSServiceFlags);
    len += sizeof(uint32_t);
    len += sizeof(DNSServiceErrorType);
    len += strlen(domain) + 1;

    reply = create_reply(enumeration_reply_op, len, rstate);
    reply->rhdr->flags = dnssd_htonl(flags);
    reply->rhdr->ifi = dnssd_htonl(ifi);
    reply->rhdr->error = dnssd_htonl(err);
    data = reply->sdata;
    put_string(domain, &data);
    return reply;
    }

mDNSlocal void enum_termination_callback(void *context)
    {
    enum_termination_t *t = context;
    mDNS *coredata = &mDNSStorage;

    mDNS_StopGetDomains(coredata, &t->all->question);
    mDNS_StopGetDomains(coredata, &t->def->question);
    freeL("enum_termination_callback", t->all);
    freeL("enum_termination_callback", t->def);
    t->rstate->termination_context = NULL;
    freeL("enum_termination_callback", t);
    }

mDNSlocal void enum_result_callback(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, mDNSBool AddRecord)
    {
    char domain[MAX_ESCAPED_DOMAIN_NAME];
    domain_enum_t *de = question->QuestionContext;
    DNSServiceFlags flags = 0;
    reply_state *reply;
    (void)m; // Unused

    if (answer->rrtype != kDNSType_PTR) return;
	if (!AddRecord && de->type != mDNS_DomainTypeBrowse) return;
	
    if (AddRecord)
    	{
        flags |= kDNSServiceFlagsAdd;
        if (de->type == mDNS_DomainTypeRegistrationDefault || de->type == mDNS_DomainTypeBrowseDefault)
            flags |= kDNSServiceFlagsDefault;
    	}
    ConvertDomainNameToCString(&answer->rdata->u.name, domain);
	// note that we do NOT propagate specific interface indexes to the client - for example, a domain we learn from
	// a machine's system preferences may be discovered on the LocalOnly interface, but should be browsed on the
	// network, so we just pass kDNSServiceInterfaceIndexAny
    reply = format_enumeration_reply(de->rstate, domain, flags, kDNSServiceInterfaceIndexAny, kDNSServiceErr_NoError);
    if (!reply)
    	{
        LogMsg("ERROR: enum_result_callback, format_enumeration_reply");
        return;
    	}
    reply->next = NULL;
    append_reply(de->rstate, reply);
    return;
    }

mDNSlocal void handle_enum_request(request_state *rstate)
    {
    DNSServiceFlags flags;
    uint32_t ifi;
    mDNSInterfaceID InterfaceID;
    char *ptr = rstate->msgdata;
    domain_enum_t *def, *all;
    enum_termination_t *term;
    mStatus err;
    int result;
    
    if (rstate->ts != t_complete)
        {
        LogMsg("ERROR: handle_enum_request - transfer state != t_complete");
        abort_request(rstate);
        unlink_request(rstate);
        return;
        }
        
    flags = get_flags(&ptr);
    ifi = get_long(&ptr);
    InterfaceID = mDNSPlatformInterfaceIDfromInterfaceIndex(&mDNSStorage, ifi);
    if (ifi && !InterfaceID)
    	{
		deliver_error(rstate, mStatus_BadParamErr);
		abort_request(rstate);
		unlink_request(rstate);
		return;
    	}

    // allocate context structures
    def = mallocL("handle_enum_request", sizeof(domain_enum_t));
    all = mallocL("handle_enum_request", sizeof(domain_enum_t));
    term = mallocL("handle_enum_request", sizeof(enum_termination_t));
    if (!def || !all || !term) FatalError("ERROR: malloc");

	uDNS_RegisterSearchDomains(&mDNSStorage);

    // enumeration requires multiple questions, so we must link all the context pointers so that
    // necessary context can be reached from the callbacks
    def->rstate = rstate;
    all->rstate = rstate;
    term->def = def;
    term->all = all;
    term->rstate = rstate;
    rstate->termination_context = term;
    rstate->terminate = enum_termination_callback;
    def->question.QuestionContext = def;
    def->type = (flags & kDNSServiceFlagsRegistrationDomains) ?
        mDNS_DomainTypeRegistrationDefault: mDNS_DomainTypeBrowseDefault;
    all->question.QuestionContext = all;
    all->type = (flags & kDNSServiceFlagsRegistrationDomains) ?
        mDNS_DomainTypeRegistration : mDNS_DomainTypeBrowse;

	// if the caller hasn't specified an explicit interface, we use local-only to get the system-wide list.
	if (!InterfaceID) InterfaceID = mDNSInterface_LocalOnly;
	
    // make the calls
	LogOperation("%3d: DNSServiceEnumerateDomains(%X=%s)", rstate->sd, flags,
		(flags & kDNSServiceFlagsBrowseDomains      ) ? "kDNSServiceFlagsBrowseDomains" :
		(flags & kDNSServiceFlagsRegistrationDomains) ? "kDNSServiceFlagsRegistrationDomains" : "<<Unknown>>");
    err = mDNS_GetDomains(&mDNSStorage, &all->question, all->type, NULL, InterfaceID, enum_result_callback, all);
    if (err == mStatus_NoError)
        err = mDNS_GetDomains(&mDNSStorage, &def->question, def->type, NULL, InterfaceID, enum_result_callback, def);
    result = deliver_error(rstate, err);  // send error *before* returning local domain
    
    if (result < 0 || err)
        {
        abort_request(rstate);
        unlink_request(rstate);
        return;
        }
    }


mDNSlocal void handle_reconfirm_request(request_state *rstate)
    {
    AuthRecord *rr = read_rr_from_ipc_msg(rstate->msgdata, 0, 0);
    if (rr)
		{
		mStatus status = mDNS_ReconfirmByValue(&mDNSStorage, &rr->resrec);
		LogOperation(
			(status == mStatus_NoError) ?
			"%3d: DNSServiceReconfirmRecord(%s) interface %d initiated" :
			"%3d: DNSServiceReconfirmRecord(%s) interface %d failed: %d",
			rstate->sd, RRDisplayString(&mDNSStorage, &rr->resrec),
			mDNSPlatformInterfaceIndexfromInterfaceID(&mDNSStorage, rr->resrec.InterfaceID), status);
		status = 0;  // Adding this line eliminates a build failure when building mDNSPosix on Tiger
		}
	abort_request(rstate);
	unlink_request(rstate);
	freeL("handle_reconfirm_request", rr);
	}

#ifdef _HAVE_SETDOMAIN_SUPPORT_
mDNSlocal void free_defdomain(mDNS *const m, AuthRecord *const rr, mStatus result)
	{
	(void)m;  // unused
	if (result == mStatus_MemFree) freeL(rr->RecordContext);  // context is the enclosing list structure
	}
#endif

mDNSlocal void handle_setdomain_request(request_state *request)
	{
	mStatus err = mStatus_NoError;
	char *ptr;
	char domainstr[MAX_ESCAPED_DOMAIN_NAME];
	domainname domain;
	DNSServiceFlags flags;
#ifdef _HAVE_SETDOMAIN_SUPPORT_
	struct xucred xuc;
	socklen_t xuclen;
#endif
	
	if (request->ts != t_complete)
        {
        LogMsg("ERROR: handle_setdomain_request - transfer state != t_complete");
        abort_request(request);
        unlink_request(request);
        return;
        }

    // extract flags/domain from message
    ptr = request->msgdata;
    flags = get_flags(&ptr);
    if (get_string(&ptr, domainstr, MAX_ESCAPED_DOMAIN_NAME) < 0 ||
		!MakeDomainNameFromDNSNameString(&domain, domainstr))
		{ err = mStatus_BadParamErr; goto end; }

	freeL("handle_setdomain_request", request->msgbuf);
    request->msgbuf = NULL;

	debugf("%3d: DNSServiceSetDefaultDomainForUser(%##s)", request->sd, domain.c);

#ifdef _HAVE_SETDOMAIN_SUPPORT_
    // this functionality currently only used for Apple-specific configuration, so we don't burned other platforms by mandating
	// the existence of this socket option
	xuclen = sizeof(xuc);
	if (getsockopt(request->sd, 0, LOCAL_PEERCRED, &xuc, &xuclen))
		{ my_perror("ERROR: getsockopt, LOCAL_PEERCRED"); err = mStatus_UnknownErr; goto end; }
	if (xuc.cr_version != XUCRED_VERSION) { LogMsg("getsockopt, LOCAL_PEERCRED - bad version"); err = mStatus_UnknownErr; goto end; }
	LogMsg("Default domain %s %s for UID %d", domainstr, flags & kDNSServiceFlagsAdd ? "set" : "removed", xuc.cr_uid);

	if (flags & kDNSServiceFlagsAdd)
		{
		// register a local-only PRT record
		default_browse_list_t *newelem = malloc(sizeof(default_browse_list_t));
		if (!newelem) { LogMsg("ERROR: malloc"); err = mStatus_NoMemoryErr; goto end; }
		mDNS_SetupResourceRecord(&newelem->ptr_rec, mDNSNULL, mDNSInterface_LocalOnly, kDNSType_PTR, 7200,  kDNSRecordTypeShared, free_defdomain, newelem);
		MakeDomainNameFromDNSNameString(&newelem->ptr_rec.resrec.name, mDNS_DomainTypeNames[mDNS_DomainTypeBrowseDefault]);
		AppendDNSNameString            (&newelem->ptr_rec.resrec.name, "local");
 		AssignDomainName(&newelem->ptr_rec.resrec.rdata->u.name, &domain);
		newelem->uid = xuc.cr_uid;
		err = mDNS_Register(&mDNSStorage, &newelem->ptr_rec);
		if (err) freeL(newelem);
		else
			{
			// link into list
			newelem->next = default_browse_list;
			default_browse_list = newelem;
			}
		
		}
	else
		{
		// remove - find in list, deregister
		default_browse_list_t *ptr = default_browse_list, *prev = NULL;
		while (ptr)
			{
			if (SameDomainName(&ptr->ptr_rec.resrec.rdata->u.name, &domain))
				{
				if (prev) prev->next = ptr->next;
				else default_browse_list = ptr->next;
				err = mDNS_Deregister(&mDNSStorage, &ptr->ptr_rec);
				break;
				}
			prev = ptr;
			ptr = ptr->next;
			}
		if (!ptr) { LogMsg("Attempt to remove nonexistent domain %s for UID %d", domainstr, xuc.cr_uid); err = mStatus_Invalid; }
		}
#else
	err = mStatus_NoError;
#endif // _HAVE_SETDOMAIN_SUPPORT_
	
	end:
    deliver_error(request, err);
    abort_request(request);
    unlink_request(request);
    }

mDNSlocal void port_mapping_create_termination_callback(void *context)
	{
	port_mapping_info_t *info = (port_mapping_info_t*) context;

	if (!info) return;

	mDNS_Lock(&mDNSStorage);

	if (info->NATAddrinfo)
		{
		uDNS_FreeNATInfo(&mDNSStorage, info->NATAddrinfo);
		}

	if (info->NATMapinfo)
		{
		uDNS_DeleteNATPortMapping(&mDNSStorage, info->NATMapinfo);
		uDNS_FreeNATInfo(&mDNSStorage, info->NATMapinfo);
		}

	info->rstate->termination_context = NULL;
	freeL("port_mapping_info_t", info);
	
	mDNS_Unlock(&mDNSStorage);
	}

mDNSlocal void format_port_mapping_request(NATTraversalInfo *n, port_mapping_info_t *info)
	{
	NATPortMapRequest *req = &n->request.PortReq;
	n->context                = info;
	n->reg.RecordRegistration = NULL;
	n->state                  = NATState_Request;
	req->vers                 = NATMAP_VERS;
	req->opcode               = n->op;
	req->unused               = zeroID;
	req->priv                 = n->PrivatePort;
	req->pub                  = n->PublicPort;
	req->lease                = mDNSOpaque32fromIntVal(n->TTL ? n->TTL : NATMAP_DEFAULT_LEASE);
	}

mDNSlocal mDNSBool port_mapping_create_reply(NATTraversalInfo *n, mDNS *m, mDNSu8 *pkt, mDNSu16 len)
	{
	mStatus err = mStatus_NoError;
	port_mapping_info_t * info;
	request_state * req;
	reply_state * rep;
	int replyLen;
	char * data;

	info = (port_mapping_info_t*) n->context;
	if (!info) { LogMsg("port_mapping_create_reply called with info == NULL"); return mDNSfalse; }

	req = info->rstate;

	if (info->NATAddrinfo == n)
		{
		mDNSBool ret;

		info->addr = zeroAddr;

		ret = uDNS_HandleNATQueryAddrReply(n, m, pkt, len, &info->addr, &err);

		if (!ret)
			{
			return ret;
			}

		if (err)
			{
			deliver_async_error(req, port_mapping_create_reply_op, kDNSServiceErr_NATPortMappingUnsupported);
			uDNS_FreeNATInfo(&mDNSStorage, n);
			info->NATAddrinfo = mDNSNULL;
			return mDNStrue;
			}

		if (info->privatePort.NotAnInteger && !info->NATMapinfo)
			{
    		if (info->protocol & kDNSServiceProtocol_UDP)
				{
				info->NATMapinfo = uDNS_AllocNATInfo(&mDNSStorage, NATOp_MapUDP, info->privatePort, info->requestedPublicPort, info->requestedTTL, port_mapping_create_reply);
				}
    		else if (info->protocol & kDNSServiceProtocol_TCP)
				{
				info->NATMapinfo = uDNS_AllocNATInfo(&mDNSStorage, NATOp_MapTCP, info->privatePort, info->requestedPublicPort, info->requestedTTL, port_mapping_create_reply);
				}
			
			if (!info->NATMapinfo) { deliver_async_error(req, port_mapping_create_reply_op, mStatus_NoMemoryErr); return mDNStrue; }
			format_port_mapping_request(info->NATMapinfo, info);
			uDNS_SendNATMsg(info->NATMapinfo, &mDNSStorage);
			}
		}
	else if (info->NATMapinfo == n)
		{
		mDNSBool ret;

		ret = uDNS_HandleNATPortMapReply(n, &mDNSStorage, pkt, len);

		switch (n->state)
			{
			case NATState_Init:
			case NATState_Request:
			case NATState_Refresh:
				{
				return ret;
				}
				
			case NATState_Deleted:
				{
				deliver_async_error(req, port_mapping_create_reply_op, kDNSServiceErr_Invalid);
				uDNS_FreeNATInfo(&mDNSStorage, n);
				info->NATMapinfo = mDNSNULL;
				return ret;
				}

			case NATState_Error:
				{
				deliver_async_error(req, port_mapping_create_reply_op, kDNSServiceErr_NATPortMappingUnsupported);
				uDNS_FreeNATInfo(&mDNSStorage, n);
				info->NATMapinfo = mDNSNULL;
				return ret;
				}

			case NATState_Established:
			case NATState_Legacy:
				{
				info->receivedPublicPort = n->PublicPort;
				info->receivedTTL        = n->TTL;
				}
			}
		}
	else
		{
		LogMsg("port_mapping_create_reply called with unknown NATInfo object");
		return mDNSfalse;
		}
		
	if (!info->privatePort.NotAnInteger || info->receivedPublicPort.NotAnInteger)
		{
		// calculate reply data length
		replyLen = sizeof(DNSServiceFlags);
		replyLen += 3 * sizeof(uint32_t);  // if index + addr + ttl
		replyLen += sizeof(DNSServiceErrorType);
		replyLen += 2 * sizeof(uint16_t);  // publicAddress + privateAddress
		replyLen += sizeof(uint8_t);       // protocol 
		
		rep = create_reply(port_mapping_create_reply_op, replyLen, req);

		rep->rhdr->flags = dnssd_htonl(0);
		rep->rhdr->ifi   = dnssd_htonl(mDNSPlatformInterfaceIndexfromInterfaceID(&mDNSStorage, info->interface_id));
		rep->rhdr->error = dnssd_htonl(kDNSServiceErr_NoError);

		data = rep->sdata;

		put_long(info->addr.ip.v4.NotAnInteger, &data);
		*data = info->protocol;
		data += sizeof(uint8_t);
		put_short(info->privatePort.NotAnInteger, &data);
		put_short(info->receivedPublicPort.NotAnInteger, &data);
		put_long(info->receivedTTL, &data);

		append_reply(req, rep);
		}

	return mDNStrue;
	}

mDNSlocal void handle_port_mapping_create_request(request_state *request)
	{
    DNSServiceFlags flags;
    uint32_t interfaceIndex;
    mDNSInterfaceID InterfaceID;
	uint8_t protocol;
	uint16_t privatePort;
	uint16_t publicPort;
	uint32_t ttl;
    char *ptr;
	port_mapping_info_t *info = NULL;
	NATAddrRequest *req;
    mStatus err = mStatus_NoError;

    if (request->ts != t_complete)
        {
        LogMsg("ERROR: handle_port_mapping_create - transfer state != t_complete");
        abort_request(request);
        unlink_request(request);
        return;
        }

    // extract data from message
	ptr            = request->msgdata;
	flags          = get_flags(&ptr);
	interfaceIndex = get_long(&ptr);
	protocol       = *ptr++;
	privatePort    = get_short(&ptr);
	publicPort     = get_short(&ptr);
	ttl            = get_long(&ptr);

    freeL("handle_port_mapping_create", request->msgbuf);
    request->msgbuf = NULL;

    InterfaceID = mDNSPlatformInterfaceIDfromInterfaceIndex(&mDNSStorage, interfaceIndex);
    if (interfaceIndex && !InterfaceID) { err = mStatus_BadParamErr;  goto error; }

	if ((!protocol && (privatePort || publicPort)) || (protocol && !privatePort) || (protocol && (protocol != kDNSServiceProtocol_UDP) && (protocol != kDNSServiceProtocol_TCP))) { err = mStatus_BadParamErr; goto error; }

	// allocate and set up browser info
	info = mallocL("port_mapping_info_t", sizeof(*info));
	if (!info) { err = mStatus_NoMemoryErr; goto error; }

	mDNS_Lock(&mDNSStorage);

	request->port_mapping_create_info      = info;
	info->interface_id                     = InterfaceID;
	info->protocol                         = protocol;
	info->privatePort.NotAnInteger         = privatePort;
	info->requestedPublicPort.NotAnInteger = publicPort;
	info->receivedPublicPort               = zeroIPPort;
	info->requestedTTL                     = ttl;
	info->receivedTTL                      = 0;
	info->rstate                           = request;
	info->NATAddrinfo                      = mDNSNULL;
	info->NATMapinfo                       = mDNSNULL;

	// setup termination context
	request->termination_context      = info;
    request->terminate                = port_mapping_create_termination_callback;
	
	LogOperation("%3d: DNSServiceNATPortMappingCreate() START", request->sd);

	info->NATAddrinfo = uDNS_AllocNATInfo(&mDNSStorage, NATOp_AddrRequest, zeroIPPort, zeroIPPort, 0, port_mapping_create_reply);
	if (!info->NATAddrinfo) { mDNS_Unlock(&mDNSStorage); err = mStatus_NoMemoryErr; goto error; }

	info->NATAddrinfo->context                = info;
	info->NATAddrinfo->reg.RecordRegistration = NULL;
	info->NATAddrinfo->state                  = NATState_Request;

    // format message
	req         = &info->NATAddrinfo->request.AddrReq;
	req->vers   = NATMAP_VERS;
	req->opcode = NATOp_AddrRequest;
	
	if (!mDNSStorage.Router.ip.v4.NotAnInteger)
		{
		debugf("No router.  Will retry NAT traversal in %ld ticks", NATMAP_INIT_RETRY);
		}
	else
		{
		uDNS_SendNATMsg(info->NATAddrinfo, &mDNSStorage);
		}

	deliver_error(request, err);

	mDNS_Unlock(&mDNSStorage);

	return;
    
error:
	if (info) freeL("port_mapping_info_t", info);
	if (request->termination_context) request->termination_context = NULL;
    deliver_error(request, err);
    abort_request(request);
    unlink_request(request);
	}

mDNSlocal void addrinfo_termination_callback(void *context)
	{
	addrinfo_info_t *info = (addrinfo_info_t*) context;

	if (!info) return;

	if (info->q4.QuestionContext)
		{
		mDNS_StopQuery(&mDNSStorage, &info->q4);
		info->q4.QuestionContext = mDNSNULL;
		}
		
	if (info->q6.QuestionContext)
		{
		mDNS_StopQuery(&mDNSStorage, &info->q6);
		info->q6.QuestionContext = mDNSNULL;
		}

	info->rstate->termination_context = NULL;
	freeL("addrinfo_info_t", info);
	}

mDNSlocal void addrinfo_result_callback(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, mDNSBool AddRecord)
	{
	char *data;
	addrinfo_info_t *info = question->QuestionContext;
	reply_state *rep;
	size_t len;
	char hostname[MAX_ESCAPED_DOMAIN_NAME];
	(void)m; // Unused

	if (!info)
		{
		return;
		}

	LogOperation("%3d: DNSServiceGetAddrInfo(%##s, %s) RESULT %s", info->rstate->sd, question->qname.c, DNSTypeName(question->qtype), RRDisplayString(m, answer));

	if (answer->rdlength == 0)
		{
		deliver_async_error(info->rstate, query_reply_op, kDNSServiceErr_NoSuchRecord);
		return;
		}

	// calculate reply data length
	len = sizeof(DNSServiceFlags);
	len += 2 * sizeof(uint32_t);  // if index + ttl
	len += sizeof(DNSServiceErrorType);
	ConvertDomainNameToCString(answer->name, hostname);
	len += strlen(hostname) + 1;
	len += 2 * sizeof(uint16_t); // type, rdlen
	len += answer->rdlength;

	rep =  create_reply(addrinfo_reply_op, len, info->rstate);

	if (AddRecord) rep->rhdr->flags |= dnssd_htonl(kDNSServiceFlagsAdd);
	rep->rhdr->ifi   = dnssd_htonl(mDNSPlatformInterfaceIndexfromInterfaceID(&mDNSStorage, answer->InterfaceID));
	rep->rhdr->error = dnssd_htonl(kDNSServiceErr_NoError);

	data = rep->sdata;
	put_string(hostname, &data);
	put_short(answer->rrtype, &data);
	put_short(answer->rdlength, &data);
	put_rdata(answer->rdlength, answer->rdata->u.data, &data);
	put_long(AddRecord ? answer->rroriginalttl : 0, &data);

	append_reply(info->rstate, rep);
	return;
	}

mDNSlocal void handle_addrinfo_request(request_state *rstate)
	{
	DNSServiceFlags flags;
	uint32_t ifi;
	char hostname[256];
	uint32_t protocol;
	char *ptr;
	mStatus result;
	mDNSInterfaceID InterfaceID;
	addrinfo_info_t * info = mDNSNULL;
	domainname d;
	mStatus err = 0;
	
	if (rstate->ts != t_complete)
		{
		LogMsg("ERROR: handle_addrinfo_request - transfer state != t_complete");
		goto error;
		}
	ptr = rstate->msgdata;
	if (!ptr)
		{
		LogMsg("ERROR: handle_addrinfo_request - NULL msgdata");
		goto error;
		}
	
	flags = get_flags(&ptr);
	ifi = get_long(&ptr);
	protocol = get_long(&ptr);
	if (get_string(&ptr, hostname, 256) < 0) goto bad_param;
	InterfaceID = mDNSPlatformInterfaceIDfromInterfaceIndex(&mDNSStorage, ifi);
	if (!MakeDomainNameFromDNSNameString(&d, hostname)) { LogMsg("ERROR: handle_addrinfo_request: bad hostname: %s", hostname); goto bad_param; }
	if (ifi && !InterfaceID) goto bad_param;
	if (protocol > (kDNSServiceProtocol_IPv4|kDNSServiceProtocol_IPv6)) goto bad_param;
	
	if (!protocol)
		{
		NetworkInterfaceInfo * intf;

		if (IsLocalDomain(&d))
			{
			for (intf = mDNSStorage.HostInterfaces; intf; intf = intf->next)
				{				
				if ((intf->ip.type == mDNSAddrType_IPv4) && !mDNSIPv4AddressIsZero(intf->ip.ip.v4))
					{
					protocol |= kDNSServiceProtocol_IPv4;
					}
				else if ((intf->ip.type == mDNSAddrType_IPv6) && !mDNSIPv6AddressIsZero(intf->ip.ip.v6))
					{
					protocol |= kDNSServiceProtocol_IPv6;
					}
				}
			}
		else
			{
			for (intf = mDNSStorage.HostInterfaces; intf; intf = intf->next)
				{
				if ((intf->ip.type == mDNSAddrType_IPv4) && is_routeable_v4(&intf->ip))
					{
					protocol |= kDNSServiceProtocol_IPv4;
					}
				else if ((intf->ip.type == mDNSAddrType_IPv6) && !mDNSAddressIsv6LinkLocal(&intf->ip))
					{
					protocol |= kDNSServiceProtocol_IPv6;
					}
				}
			}
		}

	// allocate and set up info
	info = mallocL("addrinfo_info_t", sizeof(*info));
	if (!info) { err = mStatus_NoMemoryErr; goto error; }
	mDNSPlatformMemZero(info, sizeof(addrinfo_info_t));

	info->rstate				= rstate;
	rstate->termination_context = info;
	rstate->terminate = addrinfo_termination_callback;
	result            = 0;

	if (protocol & kDNSServiceProtocol_IPv4)
		{
		info->q4.InterfaceID      = InterfaceID;
		info->q4.Target           = zeroAddr;
		info->q4.qname            = d;
		info->q4.qtype            = kDNSServiceType_A;
		info->q4.qclass           = kDNSServiceClass_IN;
		info->q4.LongLived        = (flags & kDNSServiceFlagsLongLivedQuery     ) != 0;
		info->q4.ExpectUnique     = mDNSfalse;
		info->q4.ForceMCast       = (flags & kDNSServiceFlagsForceMulticast     ) != 0;
		info->q4.ReturnIntermed   = (flags & kDNSServiceFlagsReturnIntermediates) != 0;
		info->q4.QuestionCallback = addrinfo_result_callback;
		info->q4.QuestionContext  = info;

		result = mDNS_StartQuery(&mDNSStorage, &info->q4);
		if (result != mStatus_NoError)
			{
			LogMsg("ERROR: mDNS_StartQuery: %d", (int)result);
			info->q4.QuestionContext = mDNSNULL;
			}
		}

	if (!result && (protocol & kDNSServiceProtocol_IPv6))
		{
		info->q6.InterfaceID      = InterfaceID;
		info->q6.Target           = zeroAddr;
		info->q6.qname            = d;
		info->q6.qtype            = kDNSServiceType_AAAA;
		info->q6.qclass           = kDNSServiceClass_IN;
		info->q6.LongLived        = (flags & kDNSServiceFlagsLongLivedQuery     ) != 0;
		info->q6.ExpectUnique     = mDNSfalse;
		info->q6.ForceMCast       = (flags & kDNSServiceFlagsForceMulticast     ) != 0;
		info->q6.ReturnIntermed   = (flags & kDNSServiceFlagsReturnIntermediates) != 0;
		info->q6.QuestionCallback = addrinfo_result_callback;
		info->q6.QuestionContext  = info;
		
		result = mDNS_StartQuery(&mDNSStorage, &info->q6);
		if (result != mStatus_NoError)
			{
			LogMsg("ERROR: mDNS_StartQuery: %d", (int)result);
			info->q6.QuestionContext = mDNSNULL;
			}
		}

	LogOperation("%3d: DNSServiceGetAddrInfo(%##s) START", rstate->sd, d.c);

	if (result) rstate->terminate = NULL;
	if (deliver_error(rstate, result) < 0) goto error;
	return;
	
bad_param:
	deliver_error(rstate, mStatus_BadParamErr);
	rstate->terminate = NULL;	// don't try to terminate insuccessful Core calls
error:
	if (info && info->q4.QuestionContext)
		{
		mDNS_StopQuery(&mDNSStorage, &info->q4);
		info->q4.QuestionContext = mDNSNULL;
		}
	if (info && info->q6.QuestionContext)
		{
		mDNS_StopQuery(&mDNSStorage, &info->q6);
		info->q6.QuestionContext = mDNSNULL;
		}
	abort_request(rstate);
	unlink_request(rstate);
	return;
	}

mDNSlocal void request_callback(void *info)
	{
	request_state *rstate = info;
	transfer_state result;
	dnssd_sockaddr_t cliaddr;
	int dedicated_error_socket;
#if defined(_WIN32)
	u_long opt = 1;
#endif
	
	result = read_msg(rstate);
	if (result == t_morecoming)
		{
		return;
		}
	if (result == t_terminated)
		{
		abort_request(rstate);
		unlink_request(rstate);
		return;
		}
	if (result == t_error)
		{
		abort_request(rstate);
		unlink_request(rstate);
		return;
		}

	if (rstate->hdr.version != VERSION)
		{
		LogMsg("ERROR: client incompatible with daemon (client version = %d, "
		       "daemon version = %d)\n", rstate->hdr.version, VERSION);
		abort_request(rstate);
		unlink_request(rstate);
		return;
		}
	
	if (validate_message(rstate) < 0)
		{
		// note that we cannot deliver an error message if validation fails, since the path to the error socket
		// may be contained in the (invalid) message body for some message types
		abort_request(rstate);
		unlink_request(rstate);
		LogMsg("Invalid message sent by client - may indicate a malicious program running on this machine!");
		return;
		}
	
	// check if client wants silent operation
	if (rstate->hdr.flags & IPC_FLAGS_NOREPLY) rstate->no_reply = 1;

	dedicated_error_socket = (rstate->hdr.op == reg_record_request    || rstate->hdr.op == add_record_request ||
	                          rstate->hdr.op == update_record_request || rstate->hdr.op == remove_record_request);
	
	if (((rstate->hdr.flags & IPC_FLAGS_REUSE_SOCKET) == 0) != dedicated_error_socket)
		LogMsg("WARNING: client request %d with incorrect flags setting 0x%X", rstate->hdr.op, rstate->hdr.flags);

	// check if primary socket is to be used for synchronous errors, else open new socket
	if (dedicated_error_socket)
		{
		mStatus err = 0;
		int nwritten;
		dnssd_sock_t errfd = socket(AF_DNSSD, SOCK_STREAM, 0);
		if (errfd == dnssd_InvalidSocket)
			{
			my_perror("ERROR: socket");
			abort_request(rstate);
			unlink_request(rstate);
			return;
			}

		//LogOperation("request_callback: Opened dedicated errfd %d", errfd);

		#if defined(USE_TCP_LOOPBACK)
			{
			mDNSOpaque16 port;
			port.b[0] = rstate->msgdata[0];
			port.b[1] = rstate->msgdata[1];
			rstate->msgdata += 2;
			cliaddr.sin_family      = AF_INET;
			cliaddr.sin_port        = port.NotAnInteger;
			cliaddr.sin_addr.s_addr = inet_addr(MDNS_TCP_SERVERADDR);
			}
		#else
			{
			char ctrl_path[MAX_CTLPATH];
			get_string(&rstate->msgdata, ctrl_path, 256);	// path is first element in message buffer
			mDNSPlatformMemZero(&cliaddr, sizeof(cliaddr));
			cliaddr.sun_family = AF_LOCAL;
			strcpy(cliaddr.sun_path, ctrl_path);
			}
		#endif
		//LogOperation("request_callback: Connecting to “%s”", cliaddr.sun_path);
		if (connect(errfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr)) < 0)
			{
			//LogOperation("request_callback: Couldn't connect to “%s”", cliaddr.sun_path);
			my_perror("ERROR: connect");
			abort_request(rstate);
			unlink_request(rstate);
			return;
			}
#if defined(_WIN32)
		if (ioctlsocket(errfd, FIONBIO, &opt) != 0)
#else
		if (fcntl(errfd, F_SETFL, O_NONBLOCK) != 0)
#endif
			{
			my_perror("ERROR: could not set control socket to non-blocking mode");
			abort_request(rstate);
			unlink_request(rstate);
			return;
			}

		switch(rstate->hdr.op)
			{
			case reg_record_request:    err = handle_regrecord_request   (rstate); break;
			case add_record_request:    err = handle_add_request         (rstate); break;
			case update_record_request: err = handle_update_request      (rstate); break;
			case remove_record_request: err = handle_removerecord_request(rstate); break;
			default: LogMsg("%3d: ERROR: udsserver_recv_request - unsupported request type: %d", rstate->sd, rstate->hdr.op);
			}

		//LogOperation("request_callback: Returning error code %d on socket %d", err, errfd);
		err = dnssd_htonl(err);
		nwritten = send(errfd, (dnssd_sockbuf_t) &err, sizeof(err), 0);
		// On a freshly-created Unix Domain Socket, the kernel should *never* fail to buffer a four-byte write for us.
		// If not, we don't attempt to handle this failure, but we do log it.
		if (nwritten < (int)sizeof(err))
			LogMsg("ERROR: failed to write error response back to caller: %d %d %s",
				nwritten, dnssd_errno(), dnssd_strerror(dnssd_errno()));
		//else LogOperation("request_callback: Returned  error code %d on socket %d", err, errfd);
		dnssd_close(errfd);
		//LogOperation("request_callback: Closed errfd %d", errfd);
		reset_connected_rstate(rstate);		// Reset ready to accept the next request on this pipe
		}
	else
		{
		switch(rstate->hdr.op)
			{
			case resolve_request:              handle_resolve_request              (rstate); break;
			case query_request:                handle_queryrecord_request          (rstate); break;
			case browse_request:               handle_browse_request               (rstate); break;
			case reg_service_request:          handle_regservice_request           (rstate); break;
			case enumeration_request:          handle_enum_request                 (rstate); break;
			case reconfirm_record_request:     handle_reconfirm_request            (rstate); break;
			case setdomain_request:            handle_setdomain_request            (rstate); break;
			case port_mapping_create_request:  handle_port_mapping_create_request  (rstate); break;
			case addrinfo_request:             handle_addrinfo_request             (rstate); break;
			default: LogMsg("%3d: ERROR: udsserver_recv_request - unsupported request type: %d", rstate->sd, rstate->hdr.op);
			}
		}
	}

mDNSlocal void connect_callback(void *info)
    {
    dnssd_sock_t sd;
	dnssd_socklen_t len;
	unsigned long optval;
    dnssd_sockaddr_t cliaddr;
    request_state *rstate;
    (void)info; // Unused

	len = (dnssd_socklen_t) sizeof(cliaddr);
    
	sd = accept(listenfd, (struct sockaddr*) &cliaddr, &len);

    if (sd == dnssd_InvalidSocket)
        {
        if (dnssd_errno() == dnssd_EWOULDBLOCK) return;
        my_perror("ERROR: accept");
        return;
    	}
    optval = 1;

#ifdef SO_NOSIGPIPE
	// Some environments (e.g. OS X) support turning off SIGPIPE for a socket
    if (setsockopt(sd, SOL_SOCKET, SO_NOSIGPIPE, &optval, sizeof(optval)) < 0)
    	{
        my_perror("ERROR: setsockopt - SO_NOSIGPIPE - aborting client");
        dnssd_close(sd);
        return;
    	}
#endif

#if defined(_WIN32)
	if (ioctlsocket(sd, FIONBIO, &optval) != 0)
#else
	if (fcntl(sd, F_SETFL, O_NONBLOCK) != 0)
#endif
        {
		my_perror("ERROR: fcntl(sd, F_SETFL, O_NONBLOCK) - aborting client");
		dnssd_close(sd);
		return;
		}
	
	// allocate a request_state struct that will live with the socket
    rstate = mallocL("connect_callback", sizeof(request_state));
    if (!rstate) FatalError("ERROR: malloc");
    mDNSPlatformMemZero(rstate, sizeof(request_state));
    rstate->ts = t_morecoming;
    rstate->sd = sd;
    
	LogOperation("%3d: Adding FD", rstate->sd);
    if (mStatus_NoError != udsSupportAddFDToEventLoop(sd, request_callback, rstate))
        return;
    rstate->next = all_requests;
    all_requests = rstate;
    }

mDNSexport int udsserver_init(void)
	{
	dnssd_sockaddr_t laddr;
	int				 ret;
#if defined(_WIN32)
	u_long opt = 1;
#endif

	LogOperation("udsserver_init");

	// If a particular platform wants to opt out of having a PID file, define PID_FILE to be ""
	if (PID_FILE[0])
		{
		FILE *fp = fopen(PID_FILE, "w");
		if (fp != NULL)
			{
			fprintf(fp, "%d\n", getpid());
			fclose(fp);
			}
		}

	if ((listenfd = socket(AF_DNSSD, SOCK_STREAM, 0)) == dnssd_InvalidSocket)
		{
		my_perror("ERROR: socket(AF_DNSSD, SOCK_STREAM, 0); failed");
		goto error;
		}

    mDNSPlatformMemZero(&laddr, sizeof(laddr));

	#if defined(USE_TCP_LOOPBACK)
		{
		laddr.sin_family		=	AF_INET;
		laddr.sin_port			=	htons(MDNS_TCP_SERVERPORT);
		laddr.sin_addr.s_addr	=	inet_addr(MDNS_TCP_SERVERADDR);
    	ret = bind(listenfd, (struct sockaddr *) &laddr, sizeof(laddr));
		if (ret < 0)
			{
			my_perror("ERROR: bind(listenfd, (struct sockaddr *) &laddr, sizeof(laddr)); failed");
			goto error;
			}
		}
	#else
		{
    	mode_t mask = umask(0);
    	unlink(MDNS_UDS_SERVERPATH);  //OK if this fails
    	laddr.sun_family = AF_LOCAL;
	#ifndef NOT_HAVE_SA_LEN
	// According to Stevens (section 3.2), there is no portable way to
	// determine whether sa_len is defined on a particular platform.
    	laddr.sun_len = sizeof(struct sockaddr_un);
	#endif
    	strcpy(laddr.sun_path, MDNS_UDS_SERVERPATH);
		ret = bind(listenfd, (struct sockaddr *) &laddr, sizeof(laddr));
		umask(mask);
		if (ret < 0)
			{
			my_perror("ERROR: bind(listenfd, (struct sockaddr *) &laddr, sizeof(laddr)); failed");
			goto error;
			}
		}
	#endif

#if defined(_WIN32)
	//
	// SEH: do we even need to do this on windows?  this socket
	// will be given to WSAEventSelect which will automatically
	// set it to non-blocking
	//
	if (ioctlsocket(listenfd, FIONBIO, &opt) != 0)
#else
    if (fcntl(listenfd, F_SETFL, O_NONBLOCK) != 0)
#endif
		{
		my_perror("ERROR: could not set listen socket to non-blocking mode");
		goto error;
		}

	if (listen(listenfd, LISTENQ) != 0)
		{
		my_perror("ERROR: could not listen on listen socket");
		goto error;
		}

    if (mStatus_NoError != udsSupportAddFDToEventLoop(listenfd, connect_callback, (void *) NULL))
        {
        my_perror("ERROR: could not add listen socket to event loop");
        goto error;
        }

#if !defined(PLATFORM_NO_RLIMIT)
	{
	// Set maximum number of open file descriptors
	#define MIN_OPENFILES 10240
	struct rlimit maxfds, newfds;

	// Due to bugs in OS X (<rdar://problem/2941095>, <rdar://problem/3342704>, <rdar://problem/3839173>)
	// you have to get and set rlimits once before getrlimit will return sensible values
	if (getrlimit(RLIMIT_NOFILE, &maxfds) < 0) { my_perror("ERROR: Unable to get file descriptor limit"); return 0; }
	if (setrlimit(RLIMIT_NOFILE, &maxfds) < 0) my_perror("ERROR: Unable to set maximum file descriptor limit");

	if (getrlimit(RLIMIT_NOFILE, &maxfds) < 0) { my_perror("ERROR: Unable to get file descriptor limit"); return 0; }
	newfds.rlim_max = (maxfds.rlim_max > MIN_OPENFILES) ? maxfds.rlim_max : MIN_OPENFILES;
	newfds.rlim_cur = (maxfds.rlim_cur > MIN_OPENFILES) ? maxfds.rlim_cur : MIN_OPENFILES;
	if (newfds.rlim_max != maxfds.rlim_max || newfds.rlim_cur != maxfds.rlim_cur)
		if (setrlimit(RLIMIT_NOFILE, &newfds) < 0) my_perror("ERROR: Unable to set maximum file descriptor limit");

	if (getrlimit(RLIMIT_NOFILE, &maxfds) < 0) { my_perror("ERROR: Unable to get file descriptor limit"); return 0; }
	debugf("maxfds.rlim_max %d", (long)maxfds.rlim_max);
	debugf("maxfds.rlim_cur %d", (long)maxfds.rlim_cur);
	}
#endif

	TrackLegacyBrowseDomains(&mDNSStorage);
	udsserver_handle_configchange(&mDNSStorage);
    return 0;
	
error:

    my_perror("ERROR: udsserver_init");
    return -1;
    }

mDNSexport int udsserver_exit(void)
    {
	dnssd_close(listenfd);

#if !defined(USE_TCP_LOOPBACK)
	// Currently, we're unable to remove /var/run/mdnsd because we've changed to userid "nobody"
	// to give up unnecessary privilege, but we need to be root to remove this Unix Domain Socket.
	// It would be nice if we could find a solution to this problem
	if (unlink(MDNS_UDS_SERVERPATH))
		debugf("Unable to remove %s", MDNS_UDS_SERVERPATH);
#endif

	if (PID_FILE[0]) unlink(PID_FILE);

    return 0;
    }

mDNSlocal void LogClientInfo(request_state *req)
	{
	void *t = req->termination_context;
	if (t)
		{
		if (req->terminate == regservice_termination_callback)
			{
			service_instance *ptr;
			for (ptr = ((service_info *)t)->instances; ptr; ptr = ptr->next)
				LogMsgNoIdent("%3d: DNSServiceRegister         %##s %u", req->sd, ptr->srs.RR_SRV.resrec.name->c, SRS_PORT(&ptr->srs));
			}
		else if (req->terminate == browse_termination_callback)
			{
			browser_t *blist;
			for (blist = req->browser_info->browsers; blist; blist = blist->next)
				LogMsgNoIdent("%3d: DNSServiceBrowse           %##s", req->sd, blist->q.qname.c);
			}
		else if (req->terminate == resolve_termination_callback)
			LogMsgNoIdent("%3d: DNSServiceResolve          %##s", req->sd, ((resolve_termination_t *)t)->qsrv.qname.c);
		else if (req->terminate == queryrecord_termination_callback)
			LogMsgNoIdent("%3d: DNSServiceQueryRecord      %##s", req->sd, ((DNSQuestion *)          t)->qname.c);
		else if (req->terminate == enum_termination_callback)
			LogMsgNoIdent("%3d: DNSServiceEnumerateDomains %##s", req->sd, ((enum_termination_t *)   t)->all->question.qname.c);
		
		// %%% Need to add listing of NAT port mapping requests and GetAddrInfo requests here too
		
		}
	}

mDNSexport void udsserver_info(mDNS *const m)
    {
	mDNSs32 now = mDNS_TimeNow(m);
	mDNSu32 CacheUsed = 0, CacheActive = 0;
	mDNSu32 slot;
	CacheGroup *cg;
	DNSQuestion * q;
	CacheRecord *rr;
    request_state *req;

	LogMsgNoIdent("Timenow 0x%08lX (%ld)", (mDNSu32)now, now);
	LogMsgNoIdent("------------ Cache -------------");

    LogMsgNoIdent("Slt Q     TTL if    U Type rdlen");
	for (slot = 0; slot < CACHE_HASH_SLOTS; slot++)
		for(cg = m->rrcache_hash[slot]; cg; cg=cg->next)
			{
			CacheUsed++;	// Count one cache entity for the CacheGroup object
			for (rr = cg->members; rr; rr=rr->next)
				{
				mDNSs32 remain = rr->resrec.rroriginalttl - (now - rr->TimeRcvd) / mDNSPlatformOneSecond;
				NetworkInterfaceInfo *info = (NetworkInterfaceInfo *)rr->resrec.InterfaceID;
				CacheUsed++;
				if (rr->CRActiveQuestion) CacheActive++;
				LogMsgNoIdent("%3d %s%8ld %-6s%s %-6s%s",
					slot,
					rr->CRActiveQuestion ? "*" : " ",
					remain,
					info ? info->ifname : "-U-",
					(rr->resrec.RecordType & kDNSRecordTypePacketUniqueMask) ? "-" : " ",
					DNSTypeName(rr->resrec.rrtype),
					CRDisplayString(m, rr));
				usleep(1000);	// Limit rate a little so we don't flood syslog too fast
				}
			}

	if (m->rrcache_totalused != CacheUsed)
		LogMsgNoIdent("Cache use mismatch: rrcache_totalused is %lu, true count %lu", m->rrcache_totalused, CacheUsed);
	if (m->rrcache_active != CacheActive)
		LogMsgNoIdent("Cache use mismatch: rrcache_active is %lu, true count %lu", m->rrcache_active, CacheActive);
	LogMsgNoIdent("Cache currently contains %lu records; %lu referenced by active questions", CacheUsed, CacheActive);

	LogMsgNoIdent("---------- Questions -----------");
	LogMsgNoIdent("   Int  Next if      Type");
	CacheUsed = 0;
	CacheActive = 0;
	for (q = m->Questions; q; q=q->next)
		{
		mDNSs32 i = q->ThisQInterval / mDNSPlatformOneSecond;
		mDNSs32 n = (q->LastQTime + q->ThisQInterval - now) / mDNSPlatformOneSecond;
		NetworkInterfaceInfo *info = (NetworkInterfaceInfo *)q->InterfaceID;
		CacheUsed++;
		if (q->ThisQInterval) CacheActive++;
		LogMsgNoIdent("%6d%6d %-6s%s %-6s%##s",
			i, n, info ? info->ifname : "",
			!q->TargetQID.NotAnInteger ? " " : q->llq ? "L" : "O", // mDNS, long-lived, or one-shot query?
			DNSTypeName(q->qtype), q->qname.c);
		usleep(1000);	// Limit rate a little so we don't flood syslog too fast
		}
	LogMsgNoIdent("%lu question%s; %lu active", CacheUsed, CacheUsed > 1 ? "s" : "", CacheActive);

	LogMsgNoIdent("---- Active Client Requests ----");
	for (req = all_requests; req; req=req->next)
		LogClientInfo(req);
	}

mDNSexport mDNSs32 udsserver_idle(mDNSs32 nextevent)
    {
    request_state *req = all_requests, *tmp, *prev = NULL;
    reply_state *fptr;
    transfer_state result;
    mDNSs32 now = mDNS_TimeNow(&mDNSStorage);

    while(req)
        {
        result = t_uninitialized;
        if (req->u_err)
            result = send_undelivered_error(req);
        if (result != t_error && result != t_morecoming &&		// don't try to send msg if send_error failed
            (req->ts == t_complete || req->ts == t_morecoming))
            {
            while(req->replies)
                {
                if (req->replies->next) req->replies->rhdr->flags |= dnssd_htonl(kDNSServiceFlagsMoreComing);
                result = send_msg(req->replies);
                if (result == t_complete)
                    {
                    fptr = req->replies;
                    req->replies = req->replies->next;
                    freeL("udsserver_idle", fptr);
                    req->time_blocked = 0;                              // reset failure counter after successful send
                    }
                else if (result == t_terminated || result == t_error)
                    {
                    abort_request(req);
                    break;
                    }
                else if (result == t_morecoming) break;	   		// client's queues are full, move to next
                }
            }
        if (result == t_morecoming)
			{
			if (!req->time_blocked) req->time_blocked = now;
			debugf("udsserver_idle: client has been blocked for %ld seconds", (now - req->time_blocked) / mDNSPlatformOneSecond);
			if (now - req->time_blocked >= MAX_TIME_BLOCKED)
				{
				LogMsg("Could not write data to client %d after %ld seconds - aborting connection", req->sd, MAX_TIME_BLOCKED / mDNSPlatformOneSecond);
				LogClientInfo(req);
				abort_request(req);
				result = t_terminated;
				}
			else if (nextevent - now > mDNSPlatformOneSecond) nextevent = now + mDNSPlatformOneSecond;  // try again in a second
			}
        if (result == t_terminated || result == t_error)
        //since we're already doing a list traversal, we unlink the request manually instead of calling unlink_request()
            {
            tmp = req;
            if (prev) prev->next = req->next;
            if (req == all_requests) all_requests = all_requests->next;
            req = req->next;
            freeL("udsserver_idle", tmp);
            }
        else
            {
            prev = req;
            req = req->next;
            }
        }
    return nextevent;
    }
