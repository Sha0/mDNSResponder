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
Revision 1.68  2007/07/27 18:38:56  cheshire
Rename "uDNS_CheckQuery" to more informative "uDNS_CheckCurrentQuestion"

Revision 1.67  2007/07/20 23:11:12  cheshire
Fix code layout

Revision 1.66  2007/07/16 23:54:48  cheshire
<rdar://problem/5338850> Crash when removing or changing DNS keys

Revision 1.65  2007/07/16 20:14:22  vazquez
<rdar://problem/3867231> LegacyNATTraversal: Need complete rewrite

Revision 1.64  2007/07/11 02:53:36  cheshire
<rdar://problem/5303807> Register IPv6-only hostname and don't create port mappings for AutoTunnel services
Add ServiceRecordSet parameter in GetServiceTarget

Revision 1.63  2007/06/29 00:09:24  vazquez
<rdar://problem/5301908> Clean up NAT state machine (necessary for 6 other fixes)

Revision 1.62  2007/05/14 23:53:00  cheshire
Export mDNS_StartQuery_internal and mDNS_StopQuery_internal so they can be called from uDNS.c

Revision 1.61  2007/05/07 20:43:45  cheshire
<rdar://problem/4241419> Reduce the number of queries and announcements

Revision 1.60  2007/05/04 21:46:10  cheshire
Get rid of uDNS_Close (synonym for uDNS_Sleep)

Revision 1.59  2007/05/03 22:40:38  cheshire
<rdar://problem/4669229> mDNSResponder ignores bogus null target in SRV record

Revision 1.58  2007/05/02 22:21:33  cheshire
<rdar://problem/5167331> RegisterRecord and RegisterService need to cancel StartGetZoneData

Revision 1.57  2007/04/27 19:28:02  cheshire
Any code that calls StartGetZoneData needs to keep a handle to the structure, so
it can cancel it if necessary. (First noticed as a crash in Apple Remote Desktop
-- it would start a query and then quickly cancel it, and then when
StartGetZoneData completed, it had a dangling pointer and crashed.)

Revision 1.56  2007/04/25 02:14:38  cheshire
<rdar://problem/4246187> uDNS: Identical client queries should reference a single shared core query
Additional fixes to make LLQs work properly

Revision 1.55  2007/04/22 06:02:03  cheshire
<rdar://problem/4615977> Query should immediately return failure when no server

Revision 1.54  2007/04/04 21:48:53  cheshire
<rdar://problem/4720694> Combine unicast authoritative answer list with multicast list

Revision 1.53  2007/03/28 15:56:37  cheshire
<rdar://problem/5085774> Add listing of NAT port mapping and GetAddrInfo requests in SIGINFO output

Revision 1.52  2007/02/28 01:44:26  cheshire
<rdar://problem/5027863> Byte order bugs in uDNS.c, uds_daemon.c, dnssd_clientstub.c

Revision 1.51  2007/01/27 03:34:27  cheshire
Made GetZoneData use standard queries (and cached results);
eliminated GetZoneData_Callback() packet response handler

Revision 1.50  2007/01/19 21:17:32  cheshire
StartLLQPolling needs to call SetNextQueryTime() to cause query to be done in a timely fashion

Revision 1.49  2007/01/17 21:35:31  cheshire
For clarity, rename zoneData_t field "isPrivate" to "zonePrivate"

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

#define QuestionIntervalStep 3
#define QuestionIntervalStep2 (QuestionIntervalStep*QuestionIntervalStep)
#define QuestionIntervalStep3 (QuestionIntervalStep*QuestionIntervalStep*QuestionIntervalStep)
#define InitialQuestionInterval ((mDNSPlatformOneSecond + QuestionIntervalStep-1) / QuestionIntervalStep)

// Entry points into unicast-specific routines

extern void startLLQHandshakeCallback(mDNS *const m, mStatus err, const ZoneData *zoneInfo);

extern void    uDNS_StopLongLivedQuery(mDNS *const m, DNSQuestion *const question);

extern void uDNS_Sleep(mDNS *const m);
extern void uDNS_Wake(mDNS *const m);

// uDNS_UpdateRecord
// following fields must be set, and the update validated, upon entry.
// rr->NewRData
// rr->newrdlength
// rr->UpdateCallback

extern mStatus uDNS_AddRecordToService(mDNS *const m, ServiceRecordSet *sr, ExtraResourceRecord *extra);
extern mStatus uDNS_UpdateRecord(mDNS *m, AuthRecord *rr);

extern void SetNextQueryTime(mDNS *const m, const DNSQuestion *const q);
extern mStatus mDNS_Register_internal(mDNS *const m, AuthRecord *const rr);
// mDNS_Dereg_normal is used for most calls to mDNS_Deregister_internal
// mDNS_Dereg_conflict is used to indicate that this record is being forcibly deregistered because of a conflict
// mDNS_Dereg_repeat is used when cleaning up, for records that may have already been forcibly deregistered
typedef enum { mDNS_Dereg_normal, mDNS_Dereg_conflict, mDNS_Dereg_repeat } mDNS_Dereg_type;
extern mStatus mDNS_Deregister_internal(mDNS *const m, AuthRecord *const rr, mDNS_Dereg_type drt);
extern mStatus mDNS_StartQuery_internal(mDNS *const m, DNSQuestion *const question);
extern mStatus mDNS_StopQuery_internal(mDNS *const m, DNSQuestion *const question);

extern void RecordRegistrationCallback(mDNS *const m, mStatus err, const ZoneData *zoneData);
extern void GetZoneData_QuestionCallback(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, mDNSBool AddRecord);
extern mStatus uDNS_DeregisterRecord(mDNS *const m, AuthRecord *const rr);

extern void serviceRegistrationCallback(mDNS *const m, mStatus err, const ZoneData *result);
extern const domainname *GetServiceTarget(mDNS *m, ServiceRecordSet *srs);
extern mStatus uDNS_DeregisterService(mDNS *const m, ServiceRecordSet *srs);

extern void uDNS_CheckCurrentQuestion(mDNS *const m);

// integer fields of msg header must be in HOST byte order before calling this routine
extern void uDNS_ReceiveMsg(mDNS *const m, DNSMessage *const msg, const mDNSu8 *const end,
	const mDNSAddr *const srcaddr, const mDNSIPPort srcport);

// returns time of next scheduled event
extern void uDNS_Execute(mDNS *const m);

extern mStatus           uDNS_SetupDNSConfig(mDNS *const m);
extern mStatus           uDNS_RegisterSearchDomains(mDNS *const m);
extern mDNSBool          uDNS_recvLLQResponse(mDNS *const m, const DNSMessage *const msg, const mDNSu8 *const end, const mDNSAddr *const srcaddr, const mDNSIPPort srcport);
extern DomainAuthInfo *GetAuthInfoForName(mDNS *m, const domainname *const name);

// NAT traversal
extern void	uDNS_ReceiveNATPMPPacket(mDNS *m, mDNSu8 *pkt, mDNSu16 len);	// Called for each received NAT-PMP packet
extern void	uDNS_ReceiveSSDPPacket(mDNS *m, mDNSu8 *data, mDNSu16 len);	// Called for each SSDP discovery packet
extern void	natTraversalHandleAddressReply(mDNS *const m, mDNSu8 *pkt);
extern void	natTraversalHandlePortMapReply(NATTraversalInfo *n, mDNS *const m, mDNSu8 *pkt);

#ifdef	__cplusplus
	}
#endif

#endif // __UDNS_H_
