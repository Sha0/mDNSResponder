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
Revision 1.256  2007/03/24 00:07:18  cheshire
Eliminate addrinfo_info_t as a separately-allocated structure, and make it part of the request_state union

Revision 1.255  2007/03/23 23:56:14  cheshire
Move list of record registrations into the request_state union

Revision 1.254  2007/03/23 23:48:56  cheshire
Eliminate service_info as a separately-allocated structure, and make it part of the request_state union

Revision 1.253  2007/03/23 23:04:29  cheshire
Eliminate browser_info_t as a separately-allocated structure, and make it part of request_state

Revision 1.252  2007/03/23 22:59:58  cheshire
<rdar://problem/4848295> Advertise model information via Bonjour
Use kStandardTTL, not kHostNameTTL

Revision 1.251  2007/03/23 22:44:07  cheshire
Instead of calling AbortUnlinkAndFree() haphazardly all over the place, make the handle* routines
return an error code, and then request_callback() does all necessary cleanup in one place.

Revision 1.250  2007/03/22 20:30:07  cheshire
Remove pointless "if (request->ts != t_complete) ..." checks

Revision 1.249  2007/03/22 20:13:27  cheshire
Delete unused client_context field

Revision 1.248  2007/03/22 20:03:37  cheshire
Rename variables for clarity: instead of using variable rs for both request_state
and reply_state, use req for request_state and rep for reply_state

Revision 1.247  2007/03/22 19:31:42  cheshire
<rdar://problem/4848295> Advertise model information via Bonjour
Add missing "model=" at start of DeviceInfo data

Revision 1.246  2007/03/22 18:31:48  cheshire
Put dst parameter first in mDNSPlatformStrCopy/mDNSPlatformMemCopy, like conventional Posix strcpy/memcpy

Revision 1.245  2007/03/22 00:49:20  cheshire
<rdar://problem/4848295> Advertise model information via Bonjour

Revision 1.244  2007/03/21 21:01:48  cheshire
<rdar://problem/4789793> Leak on error path in regrecord_callback, uds_daemon.c

Revision 1.243  2007/03/21 19:01:57  cheshire
<rdar://problem/5078494> IPC code not 64-bit-savvy: assumes long=32bits, and short=16bits

Revision 1.242  2007/03/21 18:51:21  cheshire
<rdar://problem/4549320> Code in uds_daemon.c passes function name instead of type name to mallocL/freeL

Revision 1.241  2007/03/20 00:04:50  cheshire
<rdar://problem/4837929> Should allow "udp" or "tcp" for protocol command-line arg
Fix LogOperation("DNSServiceNATPortMappingCreate(...)") message to actually show client arguments

Revision 1.240  2007/03/16 23:25:35  cheshire
<rdar://problem/5067001> NAT-PMP: Parameter validation not working correctly

Revision 1.239  2007/03/10 02:29:36  cheshire
Added comment about port_mapping_create_reply()

Revision 1.238  2007/03/07 00:26:48  cheshire
<rdar://problem/4426754> DNSServiceRemoveRecord log message should include record type

Revision 1.237  2007/02/28 01:44:29  cheshire
<rdar://problem/5027863> Byte order bugs in uDNS.c, uds_daemon.c, dnssd_clientstub.c

Revision 1.236  2007/02/14 01:58:19  cheshire
<rdar://problem/4995831> Don't delete Unix Domain Socket on exit if we didn't create it on startup

Revision 1.235  2007/02/08 21:12:28  cheshire
<rdar://problem/4386497> Stop reading /etc/mDNSResponder.conf on every sleep/wake

Revision 1.234  2007/02/06 19:06:49  cheshire
<rdar://problem/3956518> Need to go native with launchd

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
#define dnssd_strerror(X) win32_strerror(X)
#define usleep(X) Sleep(((X)+999)/1000)
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
		while ((n > 0) && isspace(((unsigned char *) buffer)[n - 1]))
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
#define dnssd_strerror(X) strerror(X)
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
	struct registered_record_entry *next;
	uint32_t key;
	AuthRecord *rr;		// Variable-sized AuthRecord
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

// for multi-domain default browsing
typedef struct browser_t
	{
	struct browser_t *next;
	domainname domain;
	DNSQuestion q;
	} browser_t;

typedef struct
	{
	mDNSInterfaceID       interface_id;
	uint8_t               protocol;
	mDNSIPPort            privatePort;
	mDNSIPPort            requestedPublicPort;
	mDNSIPPort            receivedPublicPort;
	uint32_t              requestedTTL;
	uint32_t              receivedTTL;
	mDNSAddr              addr;
	NATTraversalInfo     *NATAddrinfo;
	NATTraversalInfo     *NATMapinfo;
	struct request_state *rstate;
	} port_mapping_info_t;

typedef struct
	{
	mStatus err;		// Note: This field is in NETWORK byte order
	int nwritten;
	dnssd_sock_t sd;
	} undelivered_error_t;

typedef struct request_state
	{
	struct request_state *next;

	dnssd_sock_t sd;

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
	struct reply_state *replies;  // corresponding (active) reply list
	undelivered_error_t *u_err;
	void *termination_context;
	req_termination_fn terminate;

	//!!!KRS toss these pointers in a union
	port_mapping_info_t * port_mapping_create_info;

	union
		{
		struct
			{
			mDNSInterfaceID interface_id;
			mDNSBool default_domain;
			mDNSBool ForceMCast;
			domainname regtype;
			browser_t *browsers;
			} browser;
		struct
			{
			mDNSInterfaceID InterfaceID;
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
			service_instance *instances;
			} service;
		registered_record_entry *reg_recs;  // list of registrations for a connection-oriented request
		struct
			{
			mDNSInterfaceID       interface_id;
			uint32_t              flags;
			uint32_t              protocol;
			DNSQuestion           q4;
			DNSQuestion           q6;
			} addrinfo;
		} u;
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

static dnssd_sock_t listenfd = dnssd_InvalidSocket;
static request_state * all_requests = NULL;

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
	uint32_t ret;
	char *data = (char*) &ret;
	put_uint32(l, &data);
	return ret;
	}

// hack to search-replace perror's to LogMsg's
mDNSlocal void my_perror(char *errmsg)
	{
	LogMsg("%s: %s", errmsg, dnssd_strerror(dnssd_errno()));
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
mDNSlocal transfer_state send_undelivered_error(request_state *req)
	{
	int nwritten;

	nwritten = send(req->u_err->sd, (char *)(&req->u_err->err) + req->u_err->nwritten, sizeof(mStatus) - req->u_err->nwritten, 0);
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
	if ((unsigned int)(nwritten + req->u_err->nwritten) >= sizeof(mStatus))
		{
		freeL("undelivered_error_t", req->u_err);
		req->u_err = NULL;
		return t_complete;
		}
	req->u_err->nwritten += nwritten;
	return t_morecoming;
	}

mDNSlocal int send_msg(reply_state *rep)
	{
	ssize_t nwriten;
	if (!rep->msgbuf) { LogMsg("ERROR: send_msg called with NULL message buffer"); return(rep->ts = t_error); }
	if (rep->request->no_reply) { freeL("reply_state msgbuf (no_reply)", rep->msgbuf); return(rep->ts = t_complete); }

	ConvertHeaderBytes(rep->mhdr);
	nwriten = send(rep->sd, rep->msgbuf + rep->nwriten, rep->len - rep->nwriten, 0);
	ConvertHeaderBytes(rep->mhdr);

	if (nwriten < 0)
		{
		if (dnssd_errno() == dnssd_EINTR || dnssd_errno() == dnssd_EWOULDBLOCK) nwriten = 0;
		else
			{
#if !defined(PLATFORM_NO_EPIPE)
			if (dnssd_errno() == EPIPE)
				return(rep->request->ts = rep->ts = t_terminated);
			else
#endif
				{ my_perror("ERROR: send\n"); return(rep->ts = t_error); }
			}
		}
	rep->nwriten += nwriten;
	if (rep->nwriten == rep->len) { freeL("reply_state msgbuf (t_complete)", rep->msgbuf); rep->ts = t_complete; }
	return rep->ts;
	}

mDNSlocal void abort_request(request_state *req)
	{
	reply_state *rep;

	if (req->terminate) req->terminate(req->termination_context);  // terminate field may not be set yet

	LogOperation("%3d: Removing FD", req->sd);
	udsSupportRemoveFDFromEventLoop(req->sd);					// Note: This also closes file descriptor req->sd for us

	// Don't use dnssd_InvalidSocket (-1) because that's the sentinel value MACOSX_MDNS_MALLOC_DEBUGGING uses
	// for detecting when the memory for an object is inadvertently freed while the object is still on some list
	req->sd = -2;

	// free pending replies
	rep = req->replies;
	while (rep)
		{
		reply_state *ptr = rep;
		rep = rep->next;
		if (ptr->msgbuf) freeL("reply_state msgbuf (abort)", ptr->msgbuf);
		freeL("reply_state (abort)", ptr);
		}

	if (req->u_err)
		{
		freeL("request_state (abort)", req->u_err);
		req->u_err = NULL;
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

mDNSlocal void AbortUnlinkAndFree(request_state *req)
	{
	request_state **p = &all_requests;
	abort_request(req);
	while (*p && *p != req) p=&(*p)->next;
	if (*p) { *p = req->next; freeL("request_state/AbortUnlinkAndFree", req); }
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
	reply = mallocL("reply_state", sizeof(reply_state));
	if (!reply) FatalError("ERROR: malloc");
	mDNSPlatformMemZero(reply, sizeof(reply_state));
	reply->ts = t_morecoming;
	reply->sd = request->sd;
	reply->request = request;
	reply->len = totallen;
	reply->msgbuf = mallocL("reply_state msgbuf", totallen);
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
	reply_state **ptr = &req->replies;
	while (*ptr) ptr = &(*ptr)->next;
	*ptr = rep;
	rep->next = NULL;
	}

mDNSlocal void deliver_async_error(request_state *req, reply_op_t op, mStatus err)
	{
	if (!req->no_reply)
		{
		reply_state *reply = create_reply(op, 256, req);
		reply->rhdr->error = dnssd_htonl(err);
		append_reply(req, reply);
		}
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
			undeliv = mallocL("undelivered_error_t", sizeof(undelivered_error_t));
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

// read_msg may be called any time when the transfer state (req->ts) is t_morecoming.
// returns the current state of the request (morecoming, error, complete, terminated.)
// if there is no data on the socket, the socket will be closed and t_terminated will be returned
mDNSlocal int read_msg(request_state *req)
	{
	uint32_t nleft;
	int nread;
	char buf[4];   // dummy for death notification

	if (req->ts == t_terminated || req->ts == t_error)
		{
		LogMsg("ERROR: read_msg called with transfer state terminated or error");
		req->ts = t_error;
		return t_error;
		}

	if (req->ts == t_complete)
		{  // this must be death or something is wrong
		nread = recv(req->sd, buf, 4, 0);
		if (!nread) { req->ts = t_terminated; return t_terminated; }
		if (nread < 0) goto rerror;
		LogMsg("ERROR: read data from a completed request.");
		req->ts = t_error;
		return t_error;
		}

	if (req->ts != t_morecoming)
		{
		LogMsg("ERROR: read_msg called with invalid transfer state (%d)", req->ts);
		req->ts = t_error;
		return t_error;
		}

	if (req->hdr_bytes < sizeof(ipc_msg_hdr))
		{
		nleft = sizeof(ipc_msg_hdr) - req->hdr_bytes;
		nread = recv(req->sd, (char *)&req->hdr + req->hdr_bytes, nleft, 0);
		if (nread == 0) { req->ts = t_terminated; return t_terminated; }
		if (nread < 0) goto rerror;
		req->hdr_bytes += nread;
		if (req->hdr_bytes == sizeof(ipc_msg_hdr))
			{
			ConvertHeaderBytes(&req->hdr);
			if (req->hdr.version != VERSION)
				{
				LogMsg("ERROR: read_msg - client version 0x%08X does not match daemon version 0x%08X", req->hdr.version, VERSION);
				req->ts = t_error;
				return t_error;
				}
			}
		if (req->hdr_bytes > sizeof(ipc_msg_hdr))
			{
			LogMsg("ERROR: read_msg - read too many header bytes");
			req->ts = t_error;
			return t_error;
			}
		}

	// only read data if header is complete
	if (req->hdr_bytes == sizeof(ipc_msg_hdr))
		{
		if (req->hdr.datalen == 0)  // ok in removerecord requests
			{
			req->ts = t_complete;
			req->msgbuf = NULL;
			return t_complete;
			}

		if (!req->msgbuf)  // allocate the buffer first time through
			{
			req->msgbuf = mallocL("request_state msgbuf", req->hdr.datalen + MSG_PAD_BYTES);
			if (!req->msgbuf)
				{
				my_perror("ERROR: malloc");
				req->ts = t_error;
				return t_error;
				}
			req->msgdata = req->msgbuf;
			mDNSPlatformMemZero(req->msgbuf, req->hdr.datalen + MSG_PAD_BYTES);
			}
		nleft = req->hdr.datalen - req->data_bytes;
		nread = recv(req->sd, req->msgbuf + req->data_bytes, nleft, 0);
		if (nread == 0) { req->ts = t_terminated; return t_terminated; }
		if (nread < 0) goto rerror;
		req->data_bytes += nread;
		if (req->data_bytes > req->hdr.datalen)
			{
			LogMsg("ERROR: read_msg - read too many data bytes");
			req->ts = t_error;
			return t_error;
			}
		}

	if (req->hdr_bytes == sizeof(ipc_msg_hdr) && req->data_bytes == req->hdr.datalen)
		req->ts = t_complete;
	else req->ts = t_morecoming;

	return req->ts;

rerror:
	if (dnssd_errno() == dnssd_EWOULDBLOCK || dnssd_errno() == dnssd_EINTR) return t_morecoming;
	my_perror("ERROR: read_msg");
	req->ts = t_error;
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

	interfaceIndex = get_uint32(&msgbuf);
	if (get_string(&msgbuf, name, 256) < 0)
		{
		LogMsg("ERROR: read_rr_from_ipc_msg - get_string");
		return NULL;
		}
	type = get_uint16(&msgbuf);
	class = get_uint16(&msgbuf);
	rdlen = get_uint16(&msgbuf);

	if (rdlen > sizeof(RDataBody)) storage_size = rdlen;
	else storage_size = sizeof(RDataBody);

	rr = mallocL("AuthRecord/read_rr_from_ipc_msg", sizeof(AuthRecord) - sizeof(RDataBody) + storage_size);
	if (!rr) FatalError("ERROR: malloc");
	mDNS_SetupResourceRecord(rr, mDNSNULL, mDNSPlatformInterfaceIDfromInterfaceIndex(&mDNSStorage, interfaceIndex),
		type, 0, (mDNSu8) ((flags & kDNSServiceFlagsShared) ? kDNSRecordTypeShared : kDNSRecordTypeUnique), mDNSNULL, mDNSNULL);

	if (!MakeDomainNameFromDNSNameString(rr->resrec.name, name))
		{
		LogMsg("ERROR: bad name: %s", name);
		freeL("AuthRecord/read_rr_from_ipc_msg", rr);
		return NULL;
		}

	if (flags & kDNSServiceFlagsAllowRemoteQuery) rr->AllowRemoteQuery  = mDNStrue;
	rr->resrec.rrclass = class;
	rr->resrec.rdlength = rdlen;
	rr->resrec.rdata->MaxRDLength = rdlen;
	rdata = get_rdata(&msgbuf, rdlen);
	memcpy(rr->resrec.rdata->u.data, rdata, rdlen);
	if (GetTTL) rr->resrec.rroriginalttl = get_uint32(&msgbuf);
	rr->resrec.namehash = DomainNameHashValue(rr->resrec.name);
	SetNewRData(&rr->resrec, mDNSNULL, 0);	// Sets rr->rdatahash for us
	return rr;
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
	freeL("ExtraResourceRecord/FreeExtraRR", extra);
	}

mDNSlocal void free_service_instance(service_instance *srv)
	{
	request_state *rstate = srv->request;
	ExtraResourceRecord *e = srv->srs.Extras, *tmp;

	// clear pointers from parent struct
	if (rstate)
		{
		service_instance **p = &rstate->u.service.instances;
		while (*p)
			{
			if (*p == srv)
				{
				*p = (*p)->next;
				break;
				}
			p = &(*p)->next;
			}
		}

	while (e)
		{
		e->r.RecordContext = e;
		tmp = e;
		e = e->next;
		FreeExtraRR(&mDNSStorage, &tmp->r, mStatus_MemFree);
		}

	if (srv->subtypes) { freeL("ServiceSubTypes", srv->subtypes); srv->subtypes = NULL; }
	freeL("service_instance", srv);
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
		instance->request->u.service.default_domain &&
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
			else append_reply(req, rep);
			}
		if (instance->autoname && CountPeerRegistrations(m, srs) == 0)
			RecordUpdatedNiceLabel(m, 0);	// Successfully got new name, tell user immediately
		}
	else if (result == mStatus_MemFree)
		{
		if (instance->renameonmemfree)
			{
			instance->renameonmemfree = 0;
			instance->name = m->nicelabel;
			err = mDNS_RenameAndReregisterService(m, srs, &instance->name);
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
				mDNS_RenameAndReregisterService(m, srs, mDNSNULL);
			}
		else
			{
			request_state *req = instance->request;
			if (!req) { LogMsg("ERROR: regservice_callback: received result %ld with a NULL request pointer", result); return; }
			free_service_instance(instance);
			if (!SuppressError) deliver_async_error(req, reg_service_reply_op, result);
			}
		}
	else
		{
		request_state *req = instance->request;
		if (!req) { LogMsg("ERROR: regservice_callback: received result %ld with a NULL request pointer", result); return; }
		if (result != mStatus_NATTraversal) LogMsg("ERROR: unknown result in regservice_callback: %ld", result);
		free_service_instance(instance);
		if (!SuppressError) deliver_async_error(req, reg_service_reply_op, result);
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
	(void)m; // Unused

	if (!re)
		{
		// parent struct already freed by termination callback
		if (!result) LogMsg("Error: regrecord_callback: successful registration of orphaned record");
		else
			{
			if (result != mStatus_MemFree) LogMsg("regrecord_callback: error %d received after parent termination", result);
			freeL("AuthRecord/regrecord_callback", rr);
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
	reply->rhdr->ifi   = dnssd_htonl(mDNSPlatformInterfaceIndexfromInterfaceID(m, rr->resrec.InterfaceID));
	reply->rhdr->error = dnssd_htonl(result);

	if (result)
		{
		// unlink from list, free memory
		registered_record_entry **ptr = &re->rstate->u.reg_recs;
		while (*ptr && (*ptr) != re) ptr = &(*ptr)->next;
		if (!*ptr) { LogMsg("regrecord_callback - record not in list!"); return; }
		*ptr = (*ptr)->next;
		freeL("registered_record_entry AuthRecord regrecord_callback", re->rr);
		re->rr = rr = NULL;
		freeL("registered_record_entry regrecord_callback", re);
		re = NULL;
		}

	append_reply(rstate, reply);
	}

mDNSlocal void connected_registration_termination(void *context)
	{
	int shared;
	registered_record_entry *fptr, *ptr = ((request_state *)context)->u.reg_recs;
	while (ptr)
		{
		fptr = ptr;
		ptr = ptr->next;
		shared = fptr->rr->resrec.RecordType == kDNSRecordTypeShared;
		fptr->rr->RecordContext = NULL;
		mDNS_Deregister(&mDNSStorage, fptr->rr);
		freeL("registered_record_entry/connected_registration_termination", fptr);
		}
	}

mDNSlocal mStatus handle_regrecord_request(request_state *request)
	{
	AuthRecord *rr;
	registered_record_entry *re;
	mStatus result;

	rr = read_rr_from_ipc_msg(request->msgdata, 1, 1);
	if (!rr) return(mStatus_BadParamErr);

	// allocate registration entry, link into list
	re = mallocL("registered_record_entry", sizeof(registered_record_entry));
	if (!re) FatalError("ERROR: malloc");
	re->key = request->hdr.reg_index;
	re->rr = rr;
	re->rstate = request;
	re->client_context = request->hdr.client_context;
	rr->RecordContext = re;
	rr->RecordCallback = regrecord_callback;
	re->next = request->u.reg_recs;
	request->u.reg_recs = re;

	if (!request->terminate)
		{
		request->terminate = connected_registration_termination;
		request->termination_context = request;
		}

	if (rr->resrec.rroriginalttl == 0)
		rr->resrec.rroriginalttl = DefaultTTLforRRType(rr->resrec.rrtype);

	LogOperation("%3d: DNSServiceRegisterRecord %s", request->sd, RRDisplayString(&mDNSStorage, &rr->resrec));
	result = mDNS_Register(&mDNSStorage, rr);
	return(result);
	}

mDNSlocal mStatus add_record_to_service(request_state *request, service_instance *instance, uint16_t rrtype, uint16_t rdlen, char *rdata, uint32_t ttl)
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
	if (result) { freeL("ExtraResourceRecord/add_record_to_service", extra); return result; }

	extra->ClientID = request->hdr.reg_index;
	return result;
	}

mDNSlocal mStatus handle_add_request(request_state *request)
	{
	uint32_t ttl;
	uint16_t rrtype, rdlen;
	char *ptr, *rdata;
	mStatus result = mStatus_UnknownErr;
	DNSServiceFlags flags;
	service_instance *i;

	ptr = request->msgdata;
	flags = get_flags(&ptr);
	rrtype = get_uint16(&ptr);
	rdlen = get_uint16(&ptr);
	rdata = get_rdata(&ptr, rdlen);
	ttl = get_uint32(&ptr);

	if (!ttl) ttl = DefaultTTLforRRType(rrtype);

	LogOperation("%3d: DNSServiceAddRecord(%##s, %s)", request->sd,
		(request->u.service.instances) ? request->u.service.instances->srs.RR_SRV.resrec.name->c : NULL, DNSTypeName(rrtype));

	for (i = request->u.service.instances; i; i = i->next)
		{
		result = add_record_to_service(request, i, rrtype, rdlen, rdata, ttl);
		if (result && i->default_local) break;
		else result = mStatus_NoError;  // suppress non-local default errors
		}

	return(result);
	}

mDNSlocal void update_callback(mDNS *const m, AuthRecord *const rr, RData *oldrd)
	{
	(void)m; // Unused
	if (oldrd != &rr->rdatastorage) freeL("RData/update_callback", oldrd);
	}

mDNSlocal mStatus update_record(AuthRecord *rr, uint16_t rdlen, char *rdata, uint32_t ttl)
	{
	int rdsize;
	RData *newrd;
	mStatus result;

	if (rdlen > sizeof(RDataBody)) rdsize = rdlen;
	else rdsize = sizeof(RDataBody);
	newrd = mallocL("RData/update_record", sizeof(RData) - sizeof(RDataBody) + rdsize);
	if (!newrd) FatalError("ERROR: malloc");
	newrd->MaxRDLength = (mDNSu16) rdsize;
	memcpy(&newrd->u, rdata, rdlen);

	// BIND named (name daemon) doesn't allow TXT records with zero-length rdata. This is strictly speaking correct,
	// since RFC 1035 specifies a TXT record as "One or more <character-string>s", not "Zero or more <character-string>s".
	// Since some legacy apps try to create zero-length TXT records, we'll silently correct it here.
	if (rr->resrec.rrtype == kDNSType_TXT && rdlen == 0) { rdlen = 1; newrd->u.txt.c[0] = 0; }

	result = mDNS_Update(&mDNSStorage, rr, ttl, rdlen, newrd, update_callback);
	if (result) { LogMsg("ERROR: mDNS_Update - %ld", result); freeL("RData/update_record", newrd); }
	return result;
	}

mDNSlocal mStatus handle_update_request(request_state *request)
	{
	uint16_t rdlen;
	char *ptr, *rdata;
	uint32_t ttl;
	mStatus result = mStatus_BadReferenceErr;
	service_instance *i;
	AuthRecord *rr = NULL;

	// get the message data
	ptr = request->msgdata;
	get_flags(&ptr);	// flags unused
	rdlen = get_uint16(&ptr);
	rdata = get_rdata(&ptr, rdlen);
	ttl = get_uint32(&ptr);

	if (request->u.reg_recs)
		{
		// update an individually registered record
		registered_record_entry *reptr;
		for (reptr = request->u.reg_recs; reptr; reptr = reptr->next)
			{
			if (reptr->key == request->hdr.reg_index)
				{
				result = update_record(reptr->rr, rdlen, rdata, ttl);
				goto end;
				}
			}
		result = mStatus_BadReferenceErr;
		goto end;
		}

	// update a record from a service record set
	for (i = request->u.service.instances; i; i = i->next)
		{
		if (request->hdr.reg_index == TXT_RECORD_INDEX) rr = &i->srs.RR_TXT;
		else
			{
			ExtraResourceRecord *e;
			for (e = i->srs.Extras; e; e = e->next)
				if (e->ClientID == request->hdr.reg_index) { rr = &e->r; break; }
			}

		if (!rr) { result = mStatus_BadReferenceErr; goto end; }
		result = update_record(rr, rdlen, rdata, ttl);
		if (result && i->default_local) goto end;
		else result = mStatus_NoError;  // suppress non-local default errors
		}

end:
	LogOperation("%3d: DNSServiceUpdateRecord(%##s, %s)", request->sd,
		(request->u.service.instances) ? request->u.service.instances->srs.RR_SRV.resrec.name->c : NULL,
		rr ? DNSTypeName(rr->resrec.rrtype) : "<NONE>");

	return(result);
	}

// remove a resource record registered via DNSServiceRegisterRecord()
mDNSlocal mStatus remove_record(request_state *request)
	{
	int shared;
	mStatus err = mStatus_UnknownErr;
	registered_record_entry *e, **ptr = &request->u.reg_recs;

	while (*ptr && (*ptr)->key != request->hdr.reg_index) ptr = &(*ptr)->next;
	if (!*ptr) { LogMsg("DNSServiceRemoveRecord - bad reference"); return mStatus_BadReferenceErr; }
	e = *ptr;
	*ptr = e->next; // unlink

	LogOperation("%3d: DNSServiceRemoveRecord(%#s)", request->sd, e->rr->resrec.name->c);
	shared = e->rr->resrec.RecordType == kDNSRecordTypeShared;
	e->rr->RecordContext = NULL;
	err = mDNS_Deregister(&mDNSStorage, e->rr);
	if (err)
		{
		LogMsg("ERROR: remove_record, mDNS_Deregister: %ld", err);
		freeL("registered_record_entry AuthRecord remove_record", e->rr);
		freeL("registered_record_entry remove_record", e);
		}
	return err;
	}

mDNSlocal mStatus remove_extra(const request_state *const request, service_instance *const serv, mDNSu16 *const rrtype)
	{
	mStatus err = mStatus_BadReferenceErr;
	ExtraResourceRecord *ptr;

	for (ptr = serv->srs.Extras; ptr; ptr = ptr->next)
		{
		if (ptr->ClientID == request->hdr.reg_index) // found match
			{
			*rrtype = ptr->r.resrec.rrtype;
			return mDNS_RemoveRecordFromService(&mDNSStorage, &serv->srs, ptr, FreeExtraRR, ptr);
			}
		}
	return err;
	}

mDNSlocal mStatus handle_removerecord_request(request_state *request)
	{
	mStatus err = mStatus_BadReferenceErr;
	char *ptr;

	ptr = request->msgdata;
	get_flags(&ptr);	// flags unused

	if (request->u.reg_recs)  err = remove_record(request);  // remove individually registered record
	else
		{
		service_instance *i;
		mDNSu16 rrtype = 0;
		for (i = request->u.service.instances; i; i = i->next)
			{
			err = remove_extra(request, i, &rrtype);
			if (err && i->default_local) break;
			else err = mStatus_NoError;  // suppress non-local default errors
			}
		LogOperation("%3d: DNSServiceRemoveRecord(%##s, %s)", request->sd,
			(request->u.service.instances) ? request->u.service.instances->srs.RR_SRV.resrec.name->c : NULL,
			rrtype ? DNSTypeName(rrtype) : "<NONE>");
		}

	return(err);
	}

mDNSlocal void RemoveDefaultRegDomain(mDNS *const m, domainname *d)
	{
	DNameListElem **p = &m->DefRegList;

	while ((*p))
		{
		if (SameDomainName(&(*p)->name, d))
			{
			DNameListElem *ptr = *p;
			*p = ptr->next;
			mDNSPlatformMemFree(ptr);
			mDNSPlatformDefaultRegDomainChanged(d, mDNSfalse);
			return;
			}
		p = &(*p)->next;
		}
	debugf("Requested removal of default registration domain %##s not in contained in list", d->c);
	}

mDNSlocal mStatus register_service_instance(request_state *request, const domainname *domain)
	{
	service_instance *ptr, *instance;
	int instance_size;
	mStatus result;

	for (ptr = request->u.service.instances; ptr; ptr = ptr->next)
		{
		if (SameDomainName(&ptr->domain, domain))
			{ LogMsg("register_service_instance: domain %##s already registered", domain->c); return mStatus_AlreadyRegistered; }
		}

	instance_size = sizeof(*instance);
	if (request->u.service.txtlen > sizeof(RDataBody)) instance_size += (request->u.service.txtlen - sizeof(RDataBody));
	instance = mallocL("service_instance", instance_size);
	if (!instance) { my_perror("ERROR: malloc"); return mStatus_NoMemoryErr; }

	instance->subtypes = AllocateSubTypes(request->u.service.num_subtypes, request->u.service.type_as_string);
	if (request->u.service.num_subtypes && !instance->subtypes)
		{ free_service_instance(instance); instance = NULL; FatalError("ERROR: malloc"); }
	instance->request           = request;
	instance->sd                = request->sd;
	instance->autoname          = request->u.service.autoname;
	instance->autorename        = request->u.service.autorename;
	instance->allowremotequery  = request->u.service.allowremotequery;
	instance->renameonmemfree   = 0;
	instance->name              = request->u.service.name;
	AssignDomainName(&instance->domain, domain);
	instance->default_local = (request->u.service.default_domain && SameDomainName(domain, &localdomain));
	result = mDNS_RegisterService(&mDNSStorage, &instance->srs, &instance->name, &request->u.service.type, domain, request->u.service.host.c[0] ? &request->u.service.host : NULL, request->u.service.port,
								  request->u.service.txtdata, request->u.service.txtlen, instance->subtypes, request->u.service.num_subtypes, request->u.service.InterfaceID, regservice_callback, instance);

	if (result) free_service_instance(instance);
	else
		{
		instance->next = request->u.service.instances;
		request->u.service.instances = instance;
		}
	return result;
	}

mDNSlocal void UpdateDeviceInfoRecord(mDNS *const m, mDNSBool force);

mDNSlocal void regservice_termination_callback(void *context)
	{
	request_state *request = context;
	service_instance *i, *p;
	if (!request) { LogMsg("regservice_termination_callback context is NULL"); return; }
	i = request->u.service.instances;
	while (i)
		{
		p = i;
		i = i->next;
		p->request = NULL;  // clear back pointer
		// only safe to free memory if registration is not valid, i.e. deregister fails (which invalidates p)
		LogOperation("%3d: DNSServiceRegister(%##s, %u) STOP", request->sd, p->srs.RR_SRV.resrec.name->c, mDNSVal16(p->srs.RR_SRV.resrec.rdata->u.srv.port));
		if (mDNS_DeregisterService(&mDNSStorage, &p->srs)) free_service_instance(p);
		}
	request->terminate = NULL; // clear pointer from request back to info
	if (request->u.service.txtdata) { freeL("service_info txtdata", request->u.service.txtdata); request->u.service.txtdata = NULL; }
	UpdateDeviceInfoRecord(&mDNSStorage, mDNSfalse);
	}

mDNSexport void udsserver_default_reg_domain_changed(const domainname *d, mDNSBool add)
	{
	request_state *request;

	LogMsg("%s registration domain %##s", add ? "Adding" : "Removing", d->c);
	for (request = all_requests; request; request = request->next)
		{
		if (request->terminate != regservice_termination_callback) continue;
		if (!request->u.service.default_domain)  continue;

		// valid default registration
		if (add) register_service_instance(request, d);
		else
			{
			// find the instance to remove
			service_instance **p = &request->u.service.instances;
			while (*p)
				{
				if (SameDomainName(&(*p)->domain, d))
					{
					mStatus err;
					service_instance *si = *p;
					*p = si->next;
					err = mDNS_DeregisterService(&mDNSStorage, &si->srs);
					if (err)
						{
						LogMsg("udsserver_default_reg_domain_changed - mDNS_DeregisterService err %d", err);
						free_service_instance(si);
						}
					break;
					}
				p = &(*p)->next;
				}
			if (!*p) debugf("udsserver_default_reg_domain_changed - domain %##s not registered", d->c); // normal if registration failed
			}
		}
	}

mDNSlocal mStatus handle_regservice_request(request_state *request)
	{
	char *ptr = request->msgdata;
	char name[256];	// Lots of spare space for extra-long names that we'll auto-truncate down to 63 bytes
	char domain[MAX_ESCAPED_DOMAIN_NAME], host[MAX_ESCAPED_DOMAIN_NAME];
	char type_as_string[MAX_ESCAPED_DOMAIN_NAME];
	domainname d, srv;
	mStatus err;

	DNSServiceFlags flags = get_flags(&ptr);
	uint32_t interfaceIndex = get_uint32(&ptr);
	mDNSInterfaceID InterfaceID = mDNSPlatformInterfaceIDfromInterfaceIndex(&mDNSStorage, interfaceIndex);
	if (interfaceIndex && !InterfaceID)
		{ LogMsg("ERROR: handle_regservice_request - Couldn't find InterfaceID for interfaceIndex %d", interfaceIndex); return(mStatus_BadParamErr); }

	if (get_string(&ptr, name, sizeof(name)) < 0 ||
		get_string(&ptr, type_as_string, MAX_ESCAPED_DOMAIN_NAME) < 0 ||
		get_string(&ptr, domain, MAX_ESCAPED_DOMAIN_NAME) < 0 ||
		get_string(&ptr, host, MAX_ESCAPED_DOMAIN_NAME) < 0)
		{ LogMsg("ERROR: handle_regservice_request - Couldn't read name/regtype/domain"); return(mStatus_BadParamErr); }

	request->u.service.InterfaceID = InterfaceID;
	request->u.service.instances = NULL;
	request->u.service.txtlen  = 0;
	request->u.service.txtdata = NULL;
	mDNSPlatformStrCopy(request->u.service.type_as_string, type_as_string);
	request->termination_context = request;

	request->u.service.port.b[0] = *ptr++;
	request->u.service.port.b[1] = *ptr++;

	request->u.service.txtlen  = get_uint16(&ptr);
	if (request->u.service.txtlen)
		{
		request->u.service.txtdata = mallocL("service_info txtdata", request->u.service.txtlen);
		if (!request->u.service.txtdata) FatalError("ERROR: handle_regservice_request - malloc");
		memcpy(request->u.service.txtdata, get_rdata(&ptr, request->u.service.txtlen), request->u.service.txtlen);
		}
	else request->u.service.txtdata = NULL;

	// Check for sub-types after the service type
	request->u.service.num_subtypes = ChopSubTypes(request->u.service.type_as_string);	// Note: Modifies regtype string to remove trailing subtypes
	if (request->u.service.num_subtypes < 0)
		{ LogMsg("ERROR: handle_regservice_request - ChopSubTypes failed %s", request->u.service.type_as_string); return(mStatus_BadParamErr); }

	// Don't try to construct "domainname t" until *after* ChopSubTypes has worked its magic
	if (!*request->u.service.type_as_string || !MakeDomainNameFromDNSNameString(&request->u.service.type, request->u.service.type_as_string))
		{ LogMsg("ERROR: handle_regservice_request - request->u.service.type_as_string bad %s", request->u.service.type_as_string); return(mStatus_BadParamErr); }

	if (!name[0])
		{
		request->u.service.name = mDNSStorage.nicelabel;
		request->u.service.autoname = mDNStrue;
		}
	else
		{
		// If the client is allowing AutoRename, then truncate name to legal length before converting it to a DomainLabel
		if ((flags & kDNSServiceFlagsNoAutoRename) == 0)
			{
			int newlen = TruncateUTF8ToLength((mDNSu8*)name, mDNSPlatformStrLen(name), MAX_DOMAIN_LABEL);
			name[newlen] = 0;
			}
		if (!MakeDomainLabelFromLiteralString(&request->u.service.name, name))
			{ LogMsg("ERROR: handle_regservice_request - name bad %s", name); return(mStatus_BadParamErr); }
		request->u.service.autoname = mDNSfalse;
		}

	if (*domain)
		{
		request->u.service.default_domain = mDNSfalse;
		if (!MakeDomainNameFromDNSNameString(&d, domain))
			{ LogMsg("ERROR: handle_regservice_request - domain bad %s", domain); return(mStatus_BadParamErr); }
		}
	else
		{
		request->u.service.default_domain = mDNStrue;
		MakeDomainNameFromDNSNameString(&d, "local.");
		}

	if (!ConstructServiceName(&srv, &request->u.service.name, &request->u.service.type, &d))
		{ LogMsg("ERROR: handle_regservice_request - Couldn't ConstructServiceName from, “%#s” “%##s” “%##s”", request->u.service.name.c, request->u.service.type.c, d.c); return(mStatus_BadParamErr); }

	if (!MakeDomainNameFromDNSNameString(&request->u.service.host, host))
		{ LogMsg("ERROR: handle_regservice_request - host bad %s", host); return(mStatus_BadParamErr); }
	request->u.service.autorename       = (flags & kDNSServiceFlagsNoAutoRename    ) == 0;
	request->u.service.allowremotequery = (flags & kDNSServiceFlagsAllowRemoteQuery) != 0;

	// Some clients use mDNS for lightweight copy protection, registering a pseudo-service with
	// a port number of zero. When two instances of the protected client are allowed to run on one
	// machine, we don't want to see misleading "Bogus client" messages in syslog and the console.
	if (request->u.service.port.NotAnInteger)
		{
		int count = CountExistingRegistrations(&srv, request->u.service.port);
		if (count)
			LogMsg("Client application registered %d identical instances of service %##s port %u.",
				count+1, srv.c, mDNSVal16(request->u.service.port));
		}

	LogOperation("%3d: DNSServiceRegister(\"%s\", \"%s\", \"%s\", \"%s\", %u) START",
		request->sd, name, request->u.service.type_as_string, domain, host, mDNSVal16(request->u.service.port));
	err = register_service_instance(request, &d);

	if (!err && !*domain)
		{
		DNameListElem *ptr;
		for (ptr = mDNSStorage.DefRegList; ptr; ptr = ptr->next)
			register_service_instance(request, &ptr->name);
			// note that we don't report errors for non-local, non-explicit domains
		}

	if (request->u.service.autoname) UpdateDeviceInfoRecord(&mDNSStorage, mDNStrue);

	if (!err) request->terminate = regservice_termination_callback;

	return(err);
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

mDNSlocal mStatus add_domain_to_browser(request_state *info, const domainname *d)
	{
	browser_t *b, *p;
	mStatus err;

	for (p = info->u.browser.browsers; p; p = p->next)
		{
		if (SameDomainName(&p->domain, d))
			{ debugf("add_domain_to_browser - attempt to add domain %##d already in list", d->c); return mStatus_AlreadyRegistered; }
		}

	b = mallocL("browser_t", sizeof(*b));
	if (!b) return mStatus_NoMemoryErr;
	AssignDomainName(&b->domain, d);
	err = mDNS_StartBrowse(&mDNSStorage, &b->q, &info->u.browser.regtype, d, info->u.browser.interface_id, info->u.browser.ForceMCast, FoundInstance, info);
	if (err)
		{
		LogMsg("mDNS_StartBrowse returned %d for type %##s domain %##s", err, info->u.browser.regtype.c, d->c);
		freeL("browser_t/add_domain_to_browser", b);
		}
	else
		{
		b->next = info->u.browser.browsers;
		info->u.browser.browsers = b;
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
		if (!r || !r->u.browser.default_domain) continue;
		if (add) add_domain_to_browser(r, d);
		else
			{
			browser_t **ptr = &r->u.browser.browsers;
			while (*ptr)
				{
				if (SameDomainName(&(*ptr)->domain, d))
					{
					browser_t *remove = *ptr;
					*ptr = (*ptr)->next;
					mDNS_StopQueryWithRemoves(&mDNSStorage, &remove->q);
					freeL("browser_t/udsserver_automatic_browse_domain_changed", remove);
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

mDNSlocal void UpdateDeviceInfoRecord(mDNS *const m, mDNSBool force)
	{
	int num_autoname = 0;

	if (!force)
		{
		request_state *req;
		service_instance *ptr;
		for (req = all_requests; req; req = req->next)
			if (req->terminate == regservice_termination_callback)
				for (ptr = req->u.service.instances; ptr; ptr = ptr->next)
					if (ptr->autoname)
						num_autoname++;
		}

	// If DeviceInfo record is currently registered, see if we need to deregister it
	if (m->DeviceInfo.resrec.RecordType != kDNSRecordTypeUnregistered)
		if (force || num_autoname == 0 || !SameDomainLabelCS(m->DeviceInfo.resrec.name->c, m->nicelabel.c))
			{
			LogOperation("UpdateDeviceInfoRecord Deregister %##s", m->DeviceInfo.resrec.name);
			mDNS_Deregister(m, &m->DeviceInfo);
			}

	// If DeviceInfo record is not currently registered, see if we need to register it
	if (m->DeviceInfo.resrec.RecordType == kDNSRecordTypeUnregistered)
		if (force || num_autoname > 0)
			{
			mDNSu8 len = m->HIHardware.c[0] < 255 - 6 ? m->HIHardware.c[0] : 255 - 6;
			mDNS_SetupResourceRecord(&m->DeviceInfo, mDNSNULL, mDNSNULL, kDNSType_TXT, kStandardTTL, kDNSRecordTypeKnownUnique, mDNSNULL, mDNSNULL);
			ConstructServiceName(m->DeviceInfo.resrec.name, &m->nicelabel, &DeviceInfoName, &localdomain);
			mDNSPlatformMemCopy(m->DeviceInfo.resrec.rdata->u.data + 1, "model=", 6);
			mDNSPlatformMemCopy(m->DeviceInfo.resrec.rdata->u.data + 7, m->HIHardware.c + 1, len);
			m->DeviceInfo.resrec.rdata->u.data[0] = 6 + len;	// "model=" plus the device string
			m->DeviceInfo.resrec.rdlength         = 7 + len;	// One extra for the length byte at the start of the string
			LogOperation("UpdateDeviceInfoRecord   Register %##s", m->DeviceInfo.resrec.name);
			mDNS_Register(m, &m->DeviceInfo);
			}
	}

mDNSexport void udsserver_handle_configchange(mDNS *const m)
	{
	request_state *req;
	service_instance *ptr;
	domainname      regDomain;
	DNameListElem * browseDomains;

	UpdateDeviceInfoRecord(m, mDNSfalse);

	for (req = all_requests; req; req = req->next)
		if (req->terminate == regservice_termination_callback)
			for (ptr = req->u.service.instances; ptr; ptr = ptr->next)
				if (ptr->autoname && !SameDomainLabelCS(ptr->name.c, m->nicelabel.c))
					{
					ptr->renameonmemfree = 1;
					if (mDNS_DeregisterService(m, &ptr->srs))	// If service deregistered already, we can re-register immediately
						regservice_callback(m, &ptr->srs, mStatus_MemFree);
					}

	// Let the platform layer get the current DNS information
	mDNSPlatformGetDNSConfig(mDNSNULL, &regDomain, &browseDomains);

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
	if (browseDomains) SetPrefsBrowseDomains(m, browseDomains, mDNStrue);

	// Remove old browse domains from internal list
	if (m->BrowseDomains)
		{
		SetPrefsBrowseDomains(m, m->BrowseDomains, mDNSfalse);
		while (m->BrowseDomains)
			{
			DNameListElem *fptr = m->BrowseDomains;
			m->BrowseDomains = m->BrowseDomains->next;
			mDNSPlatformMemFree(fptr);
			}
		}

	// Replace the old browse domains array with the new array
	m->BrowseDomains = browseDomains;
	}

mDNSlocal void LegacyBrowseDomainChange(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, mDNSBool AddRecord)
	{
	(void)m; // unused;
	(void)question;  // unused

	LogOperation("LegacyBrowseDomainChange: %s default browse domain %##s", AddRecord ? "Adding" : "Removing", answer->rdata->u.name.c);

	if (AddRecord)
		{
		DNameListElem *new = mDNSPlatformMemAllocate(sizeof(DNameListElem));
		if (!new) { LogMsg("ERROR: malloc"); return; }
		AssignDomainName(&new->name, &answer->rdata->u.name);
		new->next = m->DefBrowseList;
		m->DefBrowseList = new;
		udsserver_automatic_browse_domain_changed(&new->name, mDNStrue);
		return;
		}
	else
		{
		DNameListElem **p = &m->DefBrowseList;
		while (*p)
			{
			if (SameDomainName(&(*p)->name, &answer->rdata->u.name))
				{
				DNameListElem *ptr = *p;
				udsserver_automatic_browse_domain_changed(&ptr->name, mDNSfalse);
				*p = ptr->next;
				mDNSPlatformMemFree(ptr);
				return;
				}
			p = &(*p)->next;
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
	request_state *info = context;
	if (!info) return;

	while (info->u.browser.browsers)
		{
		browser_t *ptr = info->u.browser.browsers;
		info->u.browser.browsers = ptr->next;
		LogOperation("%3d: DNSServiceBrowse(%##s) STOP", info->sd, ptr->q.qname.c);
		mDNS_StopBrowse(&mDNSStorage, &ptr->q);  // no need to error-check result
		freeL("browser_t/browse_termination_callback", ptr);
		}

	info->termination_context = NULL;
	}

mDNSlocal mStatus handle_browse_request(request_state *request)
	{
	char *ptr = request->msgdata;

	char regtype[MAX_ESCAPED_DOMAIN_NAME], domain[MAX_ESCAPED_DOMAIN_NAME];
	domainname typedn, d, temp;
	mDNSs32 NumSubTypes;
	mStatus err = mStatus_NoError;

	DNSServiceFlags flags = get_flags(&ptr);
	uint32_t interfaceIndex = get_uint32(&ptr);
	mDNSInterfaceID InterfaceID = mDNSPlatformInterfaceIDfromInterfaceIndex(&mDNSStorage, interfaceIndex);
	if (interfaceIndex && !InterfaceID) return(mStatus_BadParamErr);

	if (get_string(&ptr, regtype, MAX_ESCAPED_DOMAIN_NAME) < 0 ||
		get_string(&ptr, domain, MAX_ESCAPED_DOMAIN_NAME) < 0) return(mStatus_BadParamErr);

	if (!domain || (domain[0] == '\0')) uDNS_RegisterSearchDomains(&mDNSStorage);

	typedn.c[0] = 0;
	NumSubTypes = ChopSubTypes(regtype);	// Note: Modifies regtype string to remove trailing subtypes
	if (NumSubTypes < 0 || NumSubTypes > 1) return(mStatus_BadParamErr);
	if (NumSubTypes == 1 && !AppendDNSNameString(&typedn, regtype + strlen(regtype) + 1)) return(mStatus_BadParamErr);

	if (!regtype[0] || !AppendDNSNameString(&typedn, regtype)) return(mStatus_BadParamErr);

	if (!MakeDomainNameFromDNSNameString(&temp, regtype)) return(mStatus_BadParamErr);
	if (temp.c[0] > 15 && domain[0] == 0) mDNSPlatformStrCopy(domain, "local."); // For over-long service types, we only allow domain "local"

	// Set up browser info
	request->u.browser.ForceMCast = (flags & kDNSServiceFlagsForceMulticast) != 0;
	request->u.browser.interface_id = InterfaceID;
	AssignDomainName(&request->u.browser.regtype, &typedn);
	request->u.browser.default_domain = !domain[0];
	request->u.browser.browsers = NULL;

	// setup termination context
	request->termination_context = request;

	LogOperation("%3d: DNSServiceBrowse(\"%##s\", \"%s\") START", request->sd, request->u.browser.regtype.c, domain);
	if (domain[0])
		{
		if (!MakeDomainNameFromDNSNameString(&d, domain)) return(mStatus_BadParamErr);
		err = add_domain_to_browser(request, &d);
		}

	else
		{
		DNameListElem *sdom;
		for (sdom = mDNSStorage.DefBrowseList; sdom; sdom = sdom->next)
			{
			err = add_domain_to_browser(request, &sdom->name);
			if (err)
				{
				if (SameDomainName(&sdom->name, &localdomain)) break;
				else err = mStatus_NoError;  // suppress errors for non-local "default" domains
				}
			}
		}

	if (!err) request->terminate = browse_termination_callback;

	return(err);
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
	reply_state *rep;
	request_state *req = question->QuestionContext;
	resolve_termination_t *res = req->termination_context;
	(void)m; // Unused

	LogOperation("%3d: DNSServiceResolve(%##s, %s) %s %s",
		req->sd, question->qname.c, DNSTypeName(question->qtype), AddRecord ? "ADD" : "RMV", RRDisplayString(m, answer));

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
	rep =  create_reply(resolve_reply_op, len, req);
	rep->rhdr->flags = dnssd_htonl(0);
	rep->rhdr->ifi   = dnssd_htonl(mDNSPlatformInterfaceIndexfromInterfaceID(m, answer->InterfaceID));
	rep->rhdr->error = dnssd_htonl(kDNSServiceErr_NoError);

	data = rep->sdata;

	// write reply data to message
	put_string(fullname, &data);
	put_string(target, &data);
	*data++ = res->srv->rdata->u.srv.port.b[0];
	*data++ = res->srv->rdata->u.srv.port.b[1];
	put_uint16(res->txt->rdlength, &data);
	put_rdata(res->txt->rdlength, res->txt->rdata->u.data, &data);

	append_reply(req, rep);
	}

mDNSlocal void resolve_termination_callback(void *context)
	{
	resolve_termination_t *term = context;
	request_state *req;

	if (!term)
		{
		LogMsg("ERROR: resolve_termination_callback: double termination");
		return;
		}
	req = term->rstate;
	LogOperation("%3d: DNSServiceResolve(%##s) STOP", req->sd, term->qtxt.qname.c);

	mDNS_StopQuery(&mDNSStorage, &term->qtxt);
	mDNS_StopQuery(&mDNSStorage, &term->qsrv);

	freeL("resolve_termination_t", term);
	req->termination_context = NULL;
	}

mDNSlocal mStatus handle_resolve_request(request_state *request)
	{
	char *ptr = request->msgdata;
	char name[256], regtype[MAX_ESCAPED_DOMAIN_NAME], domain[MAX_ESCAPED_DOMAIN_NAME];
	domainname fqdn;
	resolve_termination_t *term;
	mStatus err;

	// extract the data from the message
	DNSServiceFlags flags = get_flags(&ptr);
	uint32_t interfaceIndex = get_uint32(&ptr);
	mDNSInterfaceID InterfaceID = mDNSPlatformInterfaceIDfromInterfaceIndex(&mDNSStorage, interfaceIndex);
	if (interfaceIndex && !InterfaceID)
		{ LogMsg("ERROR: handle_resolve_request - Couldn't find InterfaceID for interfaceIndex %d", interfaceIndex); return(mStatus_BadParamErr); }

	if (get_string(&ptr, name, 256) < 0 ||
		get_string(&ptr, regtype, MAX_ESCAPED_DOMAIN_NAME) < 0 ||
		get_string(&ptr, domain, MAX_ESCAPED_DOMAIN_NAME) < 0)
		{ LogMsg("ERROR: handle_resolve_request - Couldn't read name/regtype/domain"); return(mStatus_BadParamErr); }

	if (build_domainname_from_strings(&fqdn, name, regtype, domain) < 0)
		{ LogMsg("ERROR: handle_resolve_request - Couldn't build_domainname_from_strings “%s” “%s” “%s”", name, regtype, domain); return(mStatus_BadParamErr); }

	// set up termination info
	term = mallocL("resolve_termination_t", sizeof(resolve_termination_t));
	mDNSPlatformMemZero(term, sizeof(*term));
	if (!term) FatalError("ERROR: malloc");

	// format questions
	term->qsrv.InterfaceID      = InterfaceID;
	term->qsrv.Target           = zeroAddr;
	memcpy(&term->qsrv.qname, &fqdn, MAX_DOMAIN_NAME);
	term->qsrv.qtype            = kDNSType_SRV;
	term->qsrv.qclass           = kDNSClass_IN;
	term->qsrv.LongLived        = (flags & kDNSServiceFlagsLongLivedQuery     ) != 0;
	term->qsrv.ExpectUnique     = mDNStrue;
	term->qsrv.ForceMCast       = (flags & kDNSServiceFlagsForceMulticast     ) != 0;
	term->qsrv.ReturnIntermed   = (flags & kDNSServiceFlagsReturnIntermediates) != 0;
	term->qsrv.QuestionCallback = resolve_result_callback;
	term->qsrv.QuestionContext  = request;

	term->qtxt.InterfaceID      = InterfaceID;
	term->qtxt.Target           = zeroAddr;
	memcpy(&term->qtxt.qname, &fqdn, MAX_DOMAIN_NAME);
	term->qtxt.qtype            = kDNSType_TXT;
	term->qtxt.qclass           = kDNSClass_IN;
	term->qtxt.LongLived        = (flags & kDNSServiceFlagsLongLivedQuery     ) != 0;
	term->qtxt.ExpectUnique     = mDNStrue;
	term->qtxt.ForceMCast       = (flags & kDNSServiceFlagsForceMulticast     ) != 0;
	term->qtxt.ReturnIntermed   = (flags & kDNSServiceFlagsReturnIntermediates) != 0;
	term->qtxt.QuestionCallback = resolve_result_callback;
	term->qtxt.QuestionContext  = request;

	term->rstate = request;
	request->termination_context = term;

	// ask the questions
	LogOperation("%3d: DNSServiceResolve(%##s) START", request->sd, term->qsrv.qname.c);
	err = mDNS_StartQuery(&mDNSStorage, &term->qsrv);
	if (!err) err = mDNS_StartQuery(&mDNSStorage, &term->qtxt);

	if (err) freeL("resolve_termination_t/handle_resolve_request", term);
	else request->terminate = resolve_termination_callback;

	return(err);
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
	rep->rhdr->ifi   = dnssd_htonl(mDNSPlatformInterfaceIndexfromInterfaceID(m, answer->InterfaceID));
	rep->rhdr->error = dnssd_htonl(kDNSServiceErr_NoError);

	data = rep->sdata;

	put_string(name, &data);
	put_uint16(answer->rrtype, &data);
	put_uint16(answer->rrclass, &data);
	put_uint16(answer->rdlength, &data);
	put_rdata(answer->rdlength, answer->rdata->u.data, &data);
	put_uint32(AddRecord ? answer->rroriginalttl : 0, &data);

	append_reply(req, rep);
	}

mDNSlocal void queryrecord_termination_callback(void *context)
	{
	DNSQuestion *q = context;
	LogOperation("%3d: DNSServiceQueryRecord(%##s, %s) STOP", ((request_state *)q->QuestionContext)->sd, q->qname.c, DNSTypeName(q->qtype));
	mDNS_StopQuery(&mDNSStorage, q);  // no need to error check
	freeL("DNSQuestion/queryrecord_termination_callback", q);
	}

mDNSlocal mStatus handle_queryrecord_request(request_state *request)
	{
	char *ptr = request->msgdata;
	char name[256];
	uint16_t rrtype, rrclass;
	mStatus err;
	DNSQuestion *q;

	DNSServiceFlags flags = get_flags(&ptr);
	uint32_t interfaceIndex = get_uint32(&ptr);
	mDNSInterfaceID InterfaceID = mDNSPlatformInterfaceIDfromInterfaceIndex(&mDNSStorage, interfaceIndex);
	if (interfaceIndex && !InterfaceID) return(mStatus_BadParamErr);

	if (get_string(&ptr, name, 256) < 0) return(mStatus_BadParamErr);
	rrtype  = get_uint16(&ptr);
	rrclass = get_uint16(&ptr);

	q = mallocL("DNSQuestion", sizeof(DNSQuestion));
	if (!q) FatalError("ERROR: handle_query - malloc");
	mDNSPlatformMemZero(q, sizeof(DNSQuestion));

	q->InterfaceID      = InterfaceID;
	q->Target           = zeroAddr;
	if (!MakeDomainNameFromDNSNameString(&q->qname, name)) { freeL("DNSQuestion/handle_queryrecord_request", q); return(mStatus_BadParamErr); }
	q->qtype            = rrtype;
	q->qclass           = rrclass;
	q->LongLived        = (flags & kDNSServiceFlagsLongLivedQuery     ) != 0;
	q->ExpectUnique     = mDNSfalse;
	q->ForceMCast       = (flags & kDNSServiceFlagsForceMulticast     ) != 0;
	q->ReturnIntermed   = (flags & kDNSServiceFlagsReturnIntermediates) != 0;
	q->QuestionCallback = queryrecord_result_callback;
	q->QuestionContext  = request;

	request->termination_context = q;

	LogOperation("%3d: DNSServiceQueryRecord(%##s, %s) START", request->sd, q->qname.c, DNSTypeName(q->qtype));
	err = mDNS_StartQuery(&mDNSStorage, q);
	if (err) LogMsg("ERROR: mDNS_StartQuery: %d", (int)err);

	if (err) freeL("DNSQuestion/handle_queryrecord_request", q);
	else request->terminate = queryrecord_termination_callback;

	return(err);
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - Main Request Handler & Misc
#endif

mDNSlocal reply_state *format_enumeration_reply(request_state *request, const char *domain, DNSServiceFlags flags, uint32_t ifi, DNSServiceErrorType err)
	{
	size_t len;
	reply_state *reply;
	char *data;

	len = sizeof(DNSServiceFlags);
	len += sizeof(uint32_t);
	len += sizeof(DNSServiceErrorType);
	len += strlen(domain) + 1;

	reply = create_reply(enumeration_reply_op, len, request);
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
	freeL("domain_enum_t all", t->all);
	freeL("domain_enum_t def", t->def);
	t->rstate->termination_context = NULL;
	freeL("enum_termination_t", t);
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
	if (!reply) { LogMsg("ERROR: enum_result_callback, format_enumeration_reply"); return; }
	append_reply(de->rstate, reply);
	}

mDNSlocal mStatus handle_enum_request(request_state *request)
	{
	char *ptr = request->msgdata;
	domain_enum_t *def, *all;
	enum_termination_t *term;
	mStatus err;

	DNSServiceFlags flags = get_flags(&ptr);
	uint32_t interfaceIndex = get_uint32(&ptr);
	mDNSInterfaceID InterfaceID = mDNSPlatformInterfaceIDfromInterfaceIndex(&mDNSStorage, interfaceIndex);
	if (interfaceIndex && !InterfaceID) return(mStatus_BadParamErr);

	// allocate context structures
	def = mallocL("domain_enum_t def", sizeof(domain_enum_t));
	all = mallocL("domain_enum_t all", sizeof(domain_enum_t));
	term = mallocL("enum_termination_t", sizeof(enum_termination_t));
	if (!def || !all || !term) FatalError("ERROR: malloc");

	uDNS_RegisterSearchDomains(&mDNSStorage);

	// enumeration requires multiple questions, so we must link all the context pointers so that
	// necessary context can be reached from the callbacks
	def->rstate = request;
	all->rstate = request;
	term->def = def;
	term->all = all;
	term->rstate = request;
	request->termination_context = term;
	request->terminate = enum_termination_callback;
	def->question.QuestionContext = def;
	def->type = (flags & kDNSServiceFlagsRegistrationDomains) ?
		mDNS_DomainTypeRegistrationDefault: mDNS_DomainTypeBrowseDefault;
	all->question.QuestionContext = all;
	all->type = (flags & kDNSServiceFlagsRegistrationDomains) ?
		mDNS_DomainTypeRegistration : mDNS_DomainTypeBrowse;

	// if the caller hasn't specified an explicit interface, we use local-only to get the system-wide list.
	if (!InterfaceID) InterfaceID = mDNSInterface_LocalOnly;

	// make the calls
	LogOperation("%3d: DNSServiceEnumerateDomains(%X=%s)", request->sd, flags,
		(flags & kDNSServiceFlagsBrowseDomains      ) ? "kDNSServiceFlagsBrowseDomains" :
		(flags & kDNSServiceFlagsRegistrationDomains) ? "kDNSServiceFlagsRegistrationDomains" : "<<Unknown>>");
	err = mDNS_GetDomains(&mDNSStorage, &all->question, all->type, NULL, InterfaceID, enum_result_callback, all);
	if (err == mStatus_NoError)
		err = mDNS_GetDomains(&mDNSStorage, &def->question, def->type, NULL, InterfaceID, enum_result_callback, def);

	return(err);
	}


mDNSlocal mStatus handle_reconfirm_request(request_state *request)
	{
	AuthRecord *rr = read_rr_from_ipc_msg(request->msgdata, 0, 0);
	if (rr)
		{
		mStatus status = mDNS_ReconfirmByValue(&mDNSStorage, &rr->resrec);
		LogOperation(
			(status == mStatus_NoError) ?
			"%3d: DNSServiceReconfirmRecord(%s) interface %d initiated" :
			"%3d: DNSServiceReconfirmRecord(%s) interface %d failed: %d",
			request->sd, RRDisplayString(&mDNSStorage, &rr->resrec),
			mDNSPlatformInterfaceIndexfromInterfaceID(&mDNSStorage, rr->resrec.InterfaceID), status);
		status = 0;  // Adding this line eliminates a build failure when building mDNSPosix on Tiger
		}
	freeL("AuthRecord/handle_reconfirm_request", rr);
	// We always want to abort a DNSServiceReconfirmRecord() immediately -- there are no asynchronous results to return
	return(-1);
	}

#ifdef _HAVE_SETDOMAIN_SUPPORT_
mDNSlocal void free_defdomain(mDNS *const m, AuthRecord *const rr, mStatus result)
	{
	(void)m;  // unused
	if (result == mStatus_MemFree) freeL("AuthRecord/free_defdomain", rr->RecordContext);  // context is the enclosing list structure
	}
#endif

mDNSlocal mStatus handle_setdomain_request(request_state *request)
	{
	char *ptr = request->msgdata;
	mStatus err = mStatus_NoError;
	char domainstr[MAX_ESCAPED_DOMAIN_NAME];
	domainname domain;
#ifdef _HAVE_SETDOMAIN_SUPPORT_
	struct xucred xuc;
	socklen_t xuclen;
#endif

	// extract flags/domain from message
	DNSServiceFlags flags = get_flags(&ptr);
	(void)flags; // Unused
	if (get_string(&ptr, domainstr, MAX_ESCAPED_DOMAIN_NAME) < 0 ||
		!MakeDomainNameFromDNSNameString(&domain, domainstr)) return(mStatus_BadParamErr);

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
		if (err) freeL("default_browse_list_t/handle_setdomain_request", newelem);
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
		default_browse_list_t **p = &default_browse_list;
		while (*p)
			{
			if (SameDomainName(&(*p)->ptr_rec.resrec.rdata->u.name, &domain))
				{
				default_browse_list_t *ptr = *p;
				*p = ptr->next;
				err = mDNS_Deregister(&mDNSStorage, &ptr->ptr_rec);
				break;
				}
			p = &(*p)->next;
			}
		if (!*p) { LogMsg("Attempt to remove nonexistent domain %s for UID %d", domainstr, xuc.cr_uid); err = mStatus_Invalid; }
		}
#else
	err = mStatus_NoError;
#endif // _HAVE_SETDOMAIN_SUPPORT_

	return(err);
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

// Called via function pointer when we get a NAT-PMP address request or port mapping response
mDNSlocal mDNSBool port_mapping_create_reply(NATTraversalInfo *n, mDNS *m, mDNSu8 *pkt)
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

		ret = uDNS_HandleNATQueryAddrReply(n, m, pkt, &info->addr, &err);

		if (!ret)
			{
			return ret;
			}

		if (err)
			{
			deliver_async_error(req, port_mapping_create_reply_op, kDNSServiceErr_NATPortMappingUnsupported);
			uDNS_FreeNATInfo(m, n);
			info->NATAddrinfo = mDNSNULL;
			return mDNStrue;
			}

		if (info->privatePort.NotAnInteger && !info->NATMapinfo)
			{
			if (info->protocol & kDNSServiceProtocol_UDP)
				{
				info->NATMapinfo = uDNS_AllocNATInfo(m, NATOp_MapUDP, info->privatePort, info->requestedPublicPort, info->requestedTTL, port_mapping_create_reply);
				}
			else if (info->protocol & kDNSServiceProtocol_TCP)
				{
				info->NATMapinfo = uDNS_AllocNATInfo(m, NATOp_MapTCP, info->privatePort, info->requestedPublicPort, info->requestedTTL, port_mapping_create_reply);
				}

			if (!info->NATMapinfo) { deliver_async_error(req, port_mapping_create_reply_op, mStatus_NoMemoryErr); return mDNStrue; }
			info->NATMapinfo->context = info;
			info->NATMapinfo->reg.RecordRegistration = NULL;
			info->NATMapinfo->state                  = NATState_Request;
			uDNS_FormatPortMaprequest(info->NATMapinfo);
			uDNS_SendNATMsg(info->NATMapinfo, m);
			}
		}
	else if (info->NATMapinfo == n)
		{
		mDNSBool ret;

		ret = uDNS_HandleNATPortMapReply(n, m, pkt);

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
				uDNS_FreeNATInfo(m, n);
				info->NATMapinfo = mDNSNULL;
				return ret;
				}

			case NATState_Error:
				{
				deliver_async_error(req, port_mapping_create_reply_op, kDNSServiceErr_NATPortMappingUnsupported);
				uDNS_FreeNATInfo(m, n);
				info->NATMapinfo = mDNSNULL;
				return ret;
				}

			case NATState_Established:
			case NATState_Legacy:
				{
				info->receivedPublicPort = n->PublicPort;
				info->receivedTTL        = n->lease;
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
		rep->rhdr->ifi   = dnssd_htonl(mDNSPlatformInterfaceIndexfromInterfaceID(m, info->interface_id));
		rep->rhdr->error = dnssd_htonl(kDNSServiceErr_NoError);

		data = rep->sdata;

		*data++ = info->addr.ip.v4.b[0];
		*data++ = info->addr.ip.v4.b[1];
		*data++ = info->addr.ip.v4.b[2];
		*data++ = info->addr.ip.v4.b[3];
		*data++ = info->protocol;
		*data++ = info->privatePort.b[0];
		*data++ = info->privatePort.b[1];
		*data++ = info->receivedPublicPort.b[0];
		*data++ = info->receivedPublicPort.b[1];
		put_uint32(info->receivedTTL, &data);

		append_reply(req, rep);
		}

	return mDNStrue;
	}

mDNSlocal mStatus handle_port_mapping_create_request(request_state *request)
	{
	char *ptr = request->msgdata;
	uint8_t protocol;
	mDNSIPPort privatePort;
	mDNSIPPort publicPort;
	uint32_t ttl;
	port_mapping_info_t *info = NULL;
	NATAddrRequest *req;
	mStatus err = mStatus_NoError;

	DNSServiceFlags flags = get_flags(&ptr);
	(void)flags; // Unused
	uint32_t interfaceIndex = get_uint32(&ptr);
	mDNSInterfaceID InterfaceID = mDNSPlatformInterfaceIDfromInterfaceIndex(&mDNSStorage, interfaceIndex);
	if (interfaceIndex && !InterfaceID) return(mStatus_BadParamErr);

	protocol         = *ptr++;
	privatePort.b[0] = *ptr++;
	privatePort.b[1] = *ptr++;
	publicPort .b[0] = *ptr++;
	publicPort .b[1] = *ptr++;
	ttl              = get_uint32(&ptr);

	if (protocol == 0)
		{
		// If protocol == 0 (i.e. just request public address) then privatePort, publicPort, ttl must be zero too
		if (privatePort.NotAnInteger || publicPort.NotAnInteger || ttl) return(mStatus_BadParamErr);
		}
	else
		{
		if (!privatePort.NotAnInteger) return(mStatus_BadParamErr);
		if (!(protocol & (kDNSServiceProtocol_UDP | kDNSServiceProtocol_TCP))) return(mStatus_BadParamErr);
		}

	// allocate and set up browser info
	info = mallocL("port_mapping_info_t", sizeof(*info));
	if (!info) return(mStatus_NoMemoryErr);

	mDNS_Lock(&mDNSStorage);

	request->port_mapping_create_info = info;
	info->interface_id                = InterfaceID;
	info->protocol                    = protocol;
	info->privatePort                 = privatePort;
	info->requestedPublicPort         = publicPort;
	info->receivedPublicPort          = zeroIPPort;
	info->requestedTTL                = ttl;
	info->receivedTTL                 = 0;
	info->rstate                      = request;
	info->NATAddrinfo                 = mDNSNULL;
	info->NATMapinfo                  = mDNSNULL;

	request->termination_context      = info;

	LogOperation("%3d: DNSServiceNATPortMappingCreate(%X, %u, %u, %d) START", request->sd, protocol, mDNSVal16(privatePort), mDNSVal16(publicPort), ttl);

	info->NATAddrinfo = uDNS_AllocNATInfo(&mDNSStorage, NATOp_AddrRequest, zeroIPPort, zeroIPPort, 0, port_mapping_create_reply);
	if (!info->NATAddrinfo) { mDNS_Unlock(&mDNSStorage); return(mStatus_NoMemoryErr); }

	info->NATAddrinfo->context                = info;
	info->NATAddrinfo->reg.RecordRegistration = NULL;
	info->NATAddrinfo->state                  = NATState_Request;

	// format message
	req         = &info->NATAddrinfo->request.AddrReq;
	req->vers   = NATMAP_VERS;
	req->opcode = NATOp_AddrRequest;

	if (!mDNSStorage.Router.ip.v4.NotAnInteger)
		debugf("No router.  Will retry NAT traversal in %ld ticks", NATMAP_INIT_RETRY);
	else
		uDNS_SendNATMsg(info->NATAddrinfo, &mDNSStorage);

	mDNS_Unlock(&mDNSStorage);

	if (err) freeL("port_mapping_info_t/handle_port_mapping_create_request", info);
	else request->terminate = port_mapping_create_termination_callback;

	return(err);
	}

mDNSlocal void addrinfo_termination_callback(void *context)
	{
	request_state *request = context;

	if (request->u.addrinfo.q4.QuestionContext)
		{
		mDNS_StopQuery(&mDNSStorage, &request->u.addrinfo.q4);
		request->u.addrinfo.q4.QuestionContext = mDNSNULL;
		}

	if (request->u.addrinfo.q6.QuestionContext)
		{
		mDNS_StopQuery(&mDNSStorage, &request->u.addrinfo.q6);
		request->u.addrinfo.q6.QuestionContext = mDNSNULL;
		}

	request->termination_context = NULL;
	}

mDNSlocal void addrinfo_result_callback(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, mDNSBool AddRecord)
	{
	char *data;
	request_state *request = question->QuestionContext;
	reply_state *rep;
	size_t len;
	char hostname[MAX_ESCAPED_DOMAIN_NAME];
	(void)m; // Unused

	LogOperation("%3d: DNSServiceGetAddrInfo(%##s, %s) RESULT %s", request->sd, question->qname.c, DNSTypeName(question->qtype), RRDisplayString(m, answer));

	if (answer->rdlength == 0)
		{
		deliver_async_error(request, query_reply_op, kDNSServiceErr_NoSuchRecord);
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

	rep =  create_reply(addrinfo_reply_op, len, request);

	if (AddRecord) rep->rhdr->flags |= dnssd_htonl(kDNSServiceFlagsAdd);
	rep->rhdr->ifi   = dnssd_htonl(mDNSPlatformInterfaceIndexfromInterfaceID(m, answer->InterfaceID));
	rep->rhdr->error = dnssd_htonl(kDNSServiceErr_NoError);

	data = rep->sdata;
	put_string(hostname, &data);
	put_uint16(answer->rrtype, &data);
	put_uint16(answer->rdlength, &data);
	put_rdata(answer->rdlength, answer->rdata->u.data, &data);
	put_uint32(AddRecord ? answer->rroriginalttl : 0, &data);

	append_reply(request, rep);
	}

mDNSlocal mStatus handle_addrinfo_request(request_state *request)
	{
	char *ptr = request->msgdata;
	char hostname[256];
	uint32_t protocol;
	domainname d;
	mStatus err = 0;

	DNSServiceFlags flags = get_flags(&ptr);
	uint32_t interfaceIndex = get_uint32(&ptr);
	mDNSInterfaceID InterfaceID = mDNSPlatformInterfaceIDfromInterfaceIndex(&mDNSStorage, interfaceIndex);
	if (interfaceIndex && !InterfaceID) return(mStatus_BadParamErr);

	protocol = get_uint32(&ptr);
	if (protocol > (kDNSServiceProtocol_IPv4|kDNSServiceProtocol_IPv6)) return(mStatus_BadParamErr);

	if (get_string(&ptr, hostname, 256) < 0) return(mStatus_BadParamErr);
	if (!MakeDomainNameFromDNSNameString(&d, hostname)) { LogMsg("ERROR: handle_addrinfo_request: bad hostname: %s", hostname); return(mStatus_BadParamErr); }

	if (!protocol)
		{
		NetworkInterfaceInfo * intf;
		if (IsLocalDomain(&d))
			{
			for (intf = mDNSStorage.HostInterfaces; intf; intf = intf->next)
				{
				if      ((intf->ip.type == mDNSAddrType_IPv4) && !mDNSIPv4AddressIsZero(intf->ip.ip.v4)) protocol |= kDNSServiceProtocol_IPv4;
				else if ((intf->ip.type == mDNSAddrType_IPv6) && !mDNSIPv6AddressIsZero(intf->ip.ip.v6)) protocol |= kDNSServiceProtocol_IPv6;
				}
			}
		else
			{
			for (intf = mDNSStorage.HostInterfaces; intf; intf = intf->next)
				{
				if      ((intf->ip.type == mDNSAddrType_IPv4) && !mDNSAddressIsv4LinkLocal(&intf->ip)) protocol |= kDNSServiceProtocol_IPv4;
				else if ((intf->ip.type == mDNSAddrType_IPv6) && !mDNSAddressIsv6LinkLocal(&intf->ip)) protocol |= kDNSServiceProtocol_IPv6;
				}
			}
		}

	request->termination_context = request;

	if (protocol & kDNSServiceProtocol_IPv4)
		{
		request->u.addrinfo.q4.InterfaceID      = InterfaceID;
		request->u.addrinfo.q4.Target           = zeroAddr;
		request->u.addrinfo.q4.qname            = d;
		request->u.addrinfo.q4.qtype            = kDNSServiceType_A;
		request->u.addrinfo.q4.qclass           = kDNSServiceClass_IN;
		request->u.addrinfo.q4.LongLived        = (flags & kDNSServiceFlagsLongLivedQuery     ) != 0;
		request->u.addrinfo.q4.ExpectUnique     = mDNSfalse;
		request->u.addrinfo.q4.ForceMCast       = (flags & kDNSServiceFlagsForceMulticast     ) != 0;
		request->u.addrinfo.q4.ReturnIntermed   = (flags & kDNSServiceFlagsReturnIntermediates) != 0;
		request->u.addrinfo.q4.QuestionCallback = addrinfo_result_callback;
		request->u.addrinfo.q4.QuestionContext  = request;

		err = mDNS_StartQuery(&mDNSStorage, &request->u.addrinfo.q4);
		if (err != mStatus_NoError)
			{
			LogMsg("ERROR: mDNS_StartQuery: %d", (int)err);
			request->u.addrinfo.q4.QuestionContext = mDNSNULL;
			}
		}

	if (!err && (protocol & kDNSServiceProtocol_IPv6))
		{
		request->u.addrinfo.q6.InterfaceID      = InterfaceID;
		request->u.addrinfo.q6.Target           = zeroAddr;
		request->u.addrinfo.q6.qname            = d;
		request->u.addrinfo.q6.qtype            = kDNSServiceType_AAAA;
		request->u.addrinfo.q6.qclass           = kDNSServiceClass_IN;
		request->u.addrinfo.q6.LongLived        = (flags & kDNSServiceFlagsLongLivedQuery     ) != 0;
		request->u.addrinfo.q6.ExpectUnique     = mDNSfalse;
		request->u.addrinfo.q6.ForceMCast       = (flags & kDNSServiceFlagsForceMulticast     ) != 0;
		request->u.addrinfo.q6.ReturnIntermed   = (flags & kDNSServiceFlagsReturnIntermediates) != 0;
		request->u.addrinfo.q6.QuestionCallback = addrinfo_result_callback;
		request->u.addrinfo.q6.QuestionContext  = request;

		err = mDNS_StartQuery(&mDNSStorage, &request->u.addrinfo.q6);
		if (err != mStatus_NoError)
			{
			LogMsg("ERROR: mDNS_StartQuery: %d", (int)err);
			request->u.addrinfo.q6.QuestionContext = mDNSNULL;
			}
		}

	LogOperation("%3d: DNSServiceGetAddrInfo(%##s) START", request->sd, d.c);

	if (!err) request->terminate = addrinfo_termination_callback;

	return(err);
	}

mDNSlocal void request_callback(void *info)
	{
	request_state *request = info;
	transfer_state result;
	dnssd_sockaddr_t cliaddr;
	int dedicated_error_socket;
#if defined(_WIN32)
	u_long opt = 1;
#endif

	result = read_msg(request);
	if (result == t_morecoming) return;
	if (result == t_terminated || result == t_error) { AbortUnlinkAndFree(request); return; }

	if (request->hdr.version != VERSION)
		{
		LogMsg("ERROR: client incompatible with daemon (client version = %d, "
			   "daemon version = %d)\n", request->hdr.version, VERSION);
		AbortUnlinkAndFree(request);
		return;
		}

	if (validate_message(request) < 0)
		{
		// note that we cannot deliver an error message if validation fails, since the path to the error socket
		// may be contained in the (invalid) message body for some message types
		AbortUnlinkAndFree(request);
		LogMsg("Invalid message sent by client - may indicate a malicious program running on this machine!");
		return;
		}

	// check if client wants silent operation
	if (request->hdr.flags & IPC_FLAGS_NOREPLY) request->no_reply = 1;

	dedicated_error_socket = (request->hdr.op == reg_record_request    || request->hdr.op == add_record_request ||
							  request->hdr.op == update_record_request || request->hdr.op == remove_record_request);

	if (((request->hdr.flags & IPC_FLAGS_REUSE_SOCKET) == 0) != dedicated_error_socket)
		LogMsg("WARNING: client request %d with incorrect flags setting 0x%X", request->hdr.op, request->hdr.flags);

	// check if primary socket is to be used for synchronous errors, else open new socket
	if (dedicated_error_socket)
		{
		mStatus err = 0;
		int nwritten;
		dnssd_sock_t errfd = socket(AF_DNSSD, SOCK_STREAM, 0);
		if (errfd == dnssd_InvalidSocket) { my_perror("ERROR: socket"); AbortUnlinkAndFree(request); return; }

		//LogOperation("request_callback: Opened dedicated errfd %d", errfd);

		#if defined(USE_TCP_LOOPBACK)
			{
			mDNSOpaque16 port;
			port.b[0] = request->msgdata[0];
			port.b[1] = request->msgdata[1];
			request->msgdata += 2;
			cliaddr.sin_family      = AF_INET;
			cliaddr.sin_port        = port.NotAnInteger;
			cliaddr.sin_addr.s_addr = inet_addr(MDNS_TCP_SERVERADDR);
			}
		#else
			{
			char ctrl_path[MAX_CTLPATH];
			get_string(&request->msgdata, ctrl_path, 256);	// path is first element in message buffer
			mDNSPlatformMemZero(&cliaddr, sizeof(cliaddr));
			cliaddr.sun_family = AF_LOCAL;
			mDNSPlatformStrCopy(cliaddr.sun_path, ctrl_path);
			}
		#endif
		//LogOperation("request_callback: Connecting to “%s”", cliaddr.sun_path);
		if (connect(errfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr)) < 0)
			{
			//LogOperation("request_callback: Couldn't connect to “%s”", cliaddr.sun_path);
			my_perror("ERROR: connect");
			AbortUnlinkAndFree(request);
			return;
			}
#if defined(_WIN32)
		if (ioctlsocket(errfd, FIONBIO, &opt) != 0)
#else
		if (fcntl(errfd, F_SETFL, O_NONBLOCK) != 0)
#endif
			{
			my_perror("ERROR: could not set control socket to non-blocking mode");
			AbortUnlinkAndFree(request);
			return;
			}

		switch(request->hdr.op)
			{
			case reg_record_request:    err = handle_regrecord_request   (request); break;
			case add_record_request:    err = handle_add_request         (request); break;
			case update_record_request: err = handle_update_request      (request); break;
			case remove_record_request: err = handle_removerecord_request(request); break;
			default: LogMsg("%3d: ERROR: udsserver_recv_request - unsupported request type: %d", request->sd, request->hdr.op);
			}
		freeL("request_state msgbuf", request->msgbuf);

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
		}
	else
		{
		mStatus err = 0;
		switch(request->hdr.op)
			{
			case resolve_request:              err = handle_resolve_request              (request); break;
			case query_request:                err = handle_queryrecord_request          (request); break;
			case browse_request:               err = handle_browse_request               (request); break;
			case reg_service_request:          err = handle_regservice_request           (request); break;
			case enumeration_request:          err = handle_enum_request                 (request); break;
			case reconfirm_record_request:     err = handle_reconfirm_request            (request); break;
			case setdomain_request:            err = handle_setdomain_request            (request); break;
			case port_mapping_create_request:  err = handle_port_mapping_create_request  (request); break;
			case addrinfo_request:             err = handle_addrinfo_request             (request); break;
			default: LogMsg("%3d: ERROR: udsserver_recv_request - unsupported request type: %d", request->sd, request->hdr.op);
			}
		freeL("request_state msgbuf", request->msgbuf);
		if (deliver_error(request, err) < 0 || err) { AbortUnlinkAndFree(request); return; }
		}

	// Reset ready to accept the next request on this pipe
	request->ts         = t_morecoming;
	request->hdr_bytes  = 0;
	request->data_bytes = 0;
	request->msgbuf     = mDNSNULL;
	request->msgdata    = mDNSNULL;
	request->bufsize    = 0;
	}

mDNSlocal void connect_callback(void *info)
	{
	dnssd_sock_t sd;
	dnssd_socklen_t len;
	unsigned long optval;
	dnssd_sockaddr_t cliaddr;
	request_state *request;
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
	request = mallocL("request_state/connect_callback", sizeof(request_state));
	if (!request) FatalError("ERROR: malloc");
	mDNSPlatformMemZero(request, sizeof(request_state));
	request->ts = t_morecoming;
	request->sd = sd;

	LogOperation("%3d: Adding FD", request->sd);
	if (mStatus_NoError != udsSupportAddFDToEventLoop(sd, request_callback, request))
		return;
	request->next = all_requests;
	all_requests = request;
	}

mDNSexport int udsserver_init(dnssd_sock_t skt)
	{
	dnssd_sockaddr_t laddr;
	int ret;
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

	if (skt != dnssd_InvalidSocket)
		listenfd = skt;
	else
		{
		listenfd = socket(AF_DNSSD, SOCK_STREAM, 0);
		if (listenfd == dnssd_InvalidSocket)
			{
			my_perror("ERROR: socket(AF_DNSSD, SOCK_STREAM, 0); failed");
			goto error;
			}

		mDNSPlatformMemZero(&laddr, sizeof(laddr));

		#if defined(USE_TCP_LOOPBACK)
			{
			laddr.sin_family = AF_INET;
			laddr.sin_port = htons(MDNS_TCP_SERVERPORT);
			laddr.sin_addr.s_addr = inet_addr(MDNS_TCP_SERVERADDR);
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
			mDNSPlatformStrCopy(laddr.sun_path, MDNS_UDS_SERVERPATH);
			ret = bind(listenfd, (struct sockaddr *) &laddr, sizeof(laddr));
			umask(mask);
			if (ret < 0)
				{
				my_perror("ERROR: bind(listenfd, (struct sockaddr *) &laddr, sizeof(laddr)); failed");
				goto error;
				}
			}
		#endif
		}

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

mDNSexport int udsserver_exit(dnssd_sock_t skt)
	{
	// If the launching environment created no listening socket,
	// that means we created it ourselves, so we should clean it up on exit
	if (skt == dnssd_InvalidSocket)
		{
		dnssd_close(listenfd);
#if !defined(USE_TCP_LOOPBACK)
		// Currently, we're unable to remove /var/run/mdnsd because we've changed to userid "nobody"
		// to give up unnecessary privilege, but we need to be root to remove this Unix Domain Socket.
		// It would be nice if we could find a solution to this problem
		if (unlink(MDNS_UDS_SERVERPATH))
			debugf("Unable to remove %s", MDNS_UDS_SERVERPATH);
#endif
		}

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
			for (ptr = req->u.service.instances; ptr; ptr = ptr->next)
				LogMsgNoIdent("%3d: DNSServiceRegister         %##s %u", req->sd, ptr->srs.RR_SRV.resrec.name->c, SRS_PORT(&ptr->srs));
			}
		else if (req->terminate == browse_termination_callback)
			{
			browser_t *blist;
			for (blist = req->u.browser.browsers; blist; blist = blist->next)
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
	request_state **req = &all_requests;
	reply_state *fptr;
	transfer_state result;
	mDNSs32 now = mDNS_TimeNow(&mDNSStorage);

	while (*req)
		{
		result = t_uninitialized;
		if ((*req)->u_err)
			result = send_undelivered_error(*req);
		if (result != t_error && result != t_morecoming &&		// don't try to send msg if send_error failed
			((*req)->ts == t_complete || (*req)->ts == t_morecoming))
			{
			while ((*req)->replies)
				{
				if ((*req)->replies->next) (*req)->replies->rhdr->flags |= dnssd_htonl(kDNSServiceFlagsMoreComing);
				result = send_msg((*req)->replies);
				if (result == t_complete)
					{
					fptr = (*req)->replies;
					(*req)->replies = (*req)->replies->next;
					freeL("reply_state/udsserver_idle", fptr);
					(*req)->time_blocked = 0;                              // reset failure counter after successful send
					}
				else if (result == t_terminated || result == t_error)
					{
					abort_request(*req);
					break;
					}
				else if (result == t_morecoming) break;  // client's queues are full, move to next
				}
			}
		if (result == t_morecoming)
			{
			if (!(*req)->time_blocked) (*req)->time_blocked = now;
			debugf("udsserver_idle: client has been blocked for %ld seconds", (now - (*req)->time_blocked) / mDNSPlatformOneSecond);
			if (now - (*req)->time_blocked >= MAX_TIME_BLOCKED)
				{
				LogMsg("Could not write data to client %d after %ld seconds - aborting connection", (*req)->sd, MAX_TIME_BLOCKED / mDNSPlatformOneSecond);
				LogClientInfo(*req);
				abort_request(*req);
				result = t_terminated;
				}
			else if (nextevent - now > mDNSPlatformOneSecond) nextevent = now + mDNSPlatformOneSecond;  // try again in a second
			}
		if (result == t_terminated || result == t_error)
			{
			// Since we're already doing a list traversal, we unlink the request manually instead of calling AbortUnlinkAndFree()
			request_state *tmp = *req;
			*req = tmp->next;
			freeL("request_state/udsserver_idle", tmp);
			}
		else
			req = &(*req)->next;
		}
	return nextevent;
	}
