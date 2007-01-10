/* -*- Mode: C; tab-width: 4 -*-
 *
 * Copyright (c) 2002-2003 Apple Computer, Inc. All rights reserved.
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

$Log: uDNS.h,v $
Revision 1.48  2007/01/10 22:51:57  cheshire
<rdar://problem/4917539> Add support for one-shot private queries as well as long-lived private queries

Revision 1.47  2007/01/05 08:30:43  cheshire
Trim excessive "$Log" checkin history from before 2006
(checkin history still available via "cvs log ..." of course)

Revision 1.46  2007/01/04 01:41:47  cheshire
Use _dns-update-tls/_dns-query-tls/_dns-llq-tls instead of creating a new "_tls" subdomain

Revision 1.45  2006/12/22 20:59:49  cheshire
<rdar://problem/4742742> Read *all* DNS keys from keychain,
 not just key for the system-wide default registration domain

Revision 1.44  2006/12/20 04:07:35  cheshire
Remove uDNS_info substructure from AuthRecord_struct

Revision 1.43  2006/12/16 01:58:32  cheshire
<rdar://problem/4720673> uDNS: Need to start caching unicast records

Revision 1.42  2006/11/30 23:07:56  herscher
<rdar://problem/4765644> uDNS: Sync up with Lighthouse changes for Private DNS

Revision 1.41  2006/11/18 05:01:30  cheshire
Preliminary support for unifying the uDNS and mDNS code,
including caching of uDNS answers

Revision 1.40  2006/11/10 07:44:04  herscher
<rdar://problem/4825493> Fix Daemon locking failures while toggling BTMM

Revision 1.39  2006/10/20 05:35:05  herscher
<rdar://problem/4720713> uDNS: Merge unicast active question list with multicast list.

Revision 1.38  2006/09/26 01:54:02  herscher
<rdar://problem/4245016> NAT Port Mapping API (for both NAT-PMP and UPnP Gateway Protocol)

Revision 1.37  2006/09/15 21:20:15  cheshire
Remove uDNS_info substructure from mDNS_struct

Revision 1.36  2006/08/14 23:24:23  cheshire
Re-licensed mDNSResponder daemon source code under Apache License, Version 2.0

Revision 1.35  2006/07/30 05:45:36  cheshire
<rdar://problem/4304215> Eliminate MIN_UCAST_PERIODIC_EXEC

Revision 1.34  2006/07/15 02:01:29  cheshire
<rdar://problem/4472014> Add Private DNS client functionality to mDNSResponder
Fix broken "empty string" browsing

Revision 1.33  2006/07/05 22:53:28  cheshire
<rdar://problem/4472014> Add Private DNS client functionality to mDNSResponder
 
*/

#ifndef __UDNS_H_
#define __UDNS_H_

#include "mDNSEmbeddedAPI.h"
#include "DNSCommon.h"

#ifdef	__cplusplus
	extern "C" {
#endif

#define RESTART_GOODBYE_DELAY    (6 * mDNSPlatformOneSecond) // delay after restarting LLQ before nuking previous known answers (avoids flutter if we restart before we have networking up)
#define INIT_UCAST_POLL_INTERVAL (3 * mDNSPlatformOneSecond) // this interval is used after send failures on network transitions
	                                                         // which typically heal quickly, so we start agressively and exponentially back off
#define MAX_UCAST_POLL_INTERVAL (60 * 60 * mDNSPlatformOneSecond)
//#define MAX_UCAST_POLL_INTERVAL (1 * 60 * mDNSPlatformOneSecond)
#define LLQ_POLL_INTERVAL       (15 * 60 * mDNSPlatformOneSecond) // Polling interval for zones w/ an advertised LLQ port (ie not static zones) if LLQ fails due to NAT, etc.
#define RESPONSE_WINDOW (60 * mDNSPlatformOneSecond)         // require server responses within one minute of request

#define DEFAULT_UPDATE_LEASE 7200

// Entry points into unicast-specific routines

extern mStatus uDNS_InitLongLivedQuery(mDNS *const m, DNSQuestion *const question);
extern void    uDNS_StopLongLivedQuery (mDNS *const m, DNSQuestion *const question);

extern void uDNS_Sleep(mDNS *const m);
extern void uDNS_Wake(mDNS *const m);
#define uDNS_Close uDNS_Sleep

// uDNS_UpdateRecord
// following fields must be set, and the update validated, upon entry.
// rr->NewRData
// rr->newrdlength
// rr->UpdateCallback

extern mStatus uDNS_AddRecordToService(mDNS *const m, ServiceRecordSet *sr, ExtraResourceRecord *extra);
extern mStatus uDNS_UpdateRecord(mDNS *m, AuthRecord *rr);

extern mStatus mDNS_Register_internal(mDNS *const m, AuthRecord *const rr);
extern mStatus uDNS_RegisterRecord(mDNS *const m, AuthRecord *const rr);
extern mStatus uDNS_DeregisterRecord(mDNS *const m, AuthRecord *const rr);

extern mStatus uDNS_RegisterService(mDNS *const m, ServiceRecordSet *srs);
extern mStatus uDNS_DeregisterService(mDNS *const m, ServiceRecordSet *srs);

extern void    uDNS_CheckQuery (mDNS *const m, DNSQuestion *question);

// integer fields of msg header must be in HOST byte order before calling this routine
extern void uDNS_ReceiveMsg(mDNS *const m, DNSMessage *const msg, const mDNSu8 *const end,
	const mDNSAddr *const srcaddr, const mDNSIPPort srcport);

extern void uDNS_ReceiveNATMap(mDNS *m, mDNSu8 *pkt, mDNSu16 len);

// returns time of next scheduled event
extern void uDNS_Execute(mDNS *const m);

typedef struct                    
	{
	domainname zoneName;
	mDNSAddr   primaryAddr;
	mDNSu16    zoneClass;
	mDNSIPPort updatePort;
	mDNSIPPort queryPort;
	mDNSIPPort llqPort;
	mDNSBool   isPrivate;
	} zoneData_t;

typedef struct
	{
	zoneData_t zoneData;
	} AsyncOpResult;

typedef void AsyncOpCallback(mStatus err, mDNS *const m, void *info, const AsyncOpResult *result);

extern mStatus           uDNS_SetupDNSConfig(mDNS *const m);
extern mStatus           uDNS_RegisterSearchDomains(mDNS *const m);
extern NATTraversalInfo *uDNS_AllocNATInfo(mDNS *const m, NATOp_t op, mDNSIPPort privatePort, mDNSIPPort publicPort, mDNSu32 ttl, NATResponseHndlr callback);
extern mDNSBool          uDNS_HandleNATQueryAddrReply(NATTraversalInfo *n, mDNS * const m, mDNSu8 *pkt, mDNSu16 len, mDNSAddr *addr, mStatus *err);
extern mDNSBool          uDNS_HandleNATPortMapReply(NATTraversalInfo *n, mDNS * const m, mDNSu8 *pkt, mDNSu16 len);
extern void              uDNS_SendNATMsg(NATTraversalInfo *info, mDNS *m);
extern void              uDNS_DeleteNATPortMapping(mDNS *m, NATTraversalInfo *nat);
extern mDNSBool          uDNS_FreeNATInfo(mDNS *m, NATTraversalInfo *n);
extern DomainAuthInfo *GetAuthInfoForName(mDNS *m, const domainname *const name);

#ifdef	__cplusplus
	}
#endif

#endif // __UDNS_H_
