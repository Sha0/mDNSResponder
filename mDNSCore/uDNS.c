/* -*- Mode: C; tab-width: 4 -*-
 *
 * Copyright (c) 2002-2006 Apple Computer, Inc. All rights reserved.
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

 * To Do:
 * Elimate all mDNSPlatformMemAllocate/mDNSPlatformMemFree from this code -- the core code
 * is supposed to be malloc-free so that it runs in constant memory determined at compile-time.
 * Any dynamic run-time requirements should be handled by the platform layer below or client layer above

	Change History (most recent first):

$Log: uDNS.c,v $
Revision 1.456  2007/09/05 21:00:17  cheshire
<rdar://problem/5457287> mDNSResponder taking up 100% CPU in ReissueBlockedQuestions
Additional refinement: ThisQInterval needs to be restored in tcpCallback, not in startPrivateQueryCallback

Revision 1.455  2007/09/05 20:53:06  cheshire
Tidied up alignment of code layout; code was clearing m->tcpAddrInfo.sock instead of m->tcpDeviceInfo.sock

Revision 1.454  2007/09/05 02:32:55  cheshire
Fixed posix build error (mixed declarations and code)

Revision 1.453  2007/09/05 02:26:57  cheshire
<rdar://problem/5457287> mDNSResponder taking up 100% CPU in ReissueBlockedQuestions
In startPrivateQueryCallback, restore q->ThisQInterval to non-zero value after GetZoneData completes

Revision 1.452  2007/08/31 22:58:22  cheshire
If we have an existing TCP connection we should re-use it instead of just bailing out
After receiving dnsbugtest response, need to set m->NextScheduledQuery to cause queries to be re-issued

Revision 1.451  2007/08/31 18:49:49  vazquez
<rdar://problem/5393719> BTMM: Need to properly deregister when stopping BTMM

Revision 1.450  2007/08/30 22:50:04  mcguire
<rdar://problem/5430628> BTMM: Tunneled services are registered when autotunnel can't be setup

Revision 1.449  2007/08/30 00:43:17  cheshire
Need to clear m->rec.r.resrec.RecordType before returning from uDNS_recvLLQResponse

Revision 1.448  2007/08/30 00:18:46  cheshire
<rdar://problem/5448804> Error messages: "SendServiceRegistration: Already have TCP connection..."

Revision 1.447  2007/08/29 01:18:33  cheshire
<rdar://problem/5400181> BTMM: Tunneled services do not need NAT port mappings
Only create NAT mappings for SRV records with AutoTarget set to Target_AutoHostAndNATMAP

Revision 1.446  2007/08/28 23:58:42  cheshire
Rename HostTarget -> AutoTarget

Revision 1.445  2007/08/28 23:53:21  cheshire
Rename serviceRegistrationCallback -> ServiceRegistrationZoneDataComplete

Revision 1.444  2007/08/27 20:29:20  cheshire
Additional debugging messages

Revision 1.443  2007/08/24 23:18:28  cheshire
mDNS_SetSecretForDomain is called with lock held; needs to use
GetAuthInfoForName_internal() instead of external version GetAuthInfoForName()

Revision 1.442  2007/08/24 22:43:06  cheshire
Tidied up coded layout

Revision 1.441  2007/08/24 01:20:55  cheshire
<rdar://problem/5434381> BTMM: Memory corruption in KeychainChanged event handling

Revision 1.440  2007/08/24 00:15:20  cheshire
Renamed GetAuthInfoForName() to GetAuthInfoForName_internal() to make it clear that it may only be called with the lock held

Revision 1.439  2007/08/23 21:47:09  vazquez
<rdar://problem/5427316> BTMM: mDNSResponder sends NAT-PMP packets on public network
make sure we clean up port mappings on base stations by sending a lease value of 0,
and only send NAT-PMP packets on private networks; also save some memory by
not using packet structs in NATTraversals.

Revision 1.438  2007/08/22 17:50:08  vazquez
<rdar://problem/5399276> Need to handle errors returned by NAT-PMP routers properly
Propagate router errors to clients, and stop logging spurious "message too short" logs.

Revision 1.437  2007/08/18 00:54:15  mcguire
<rdar://problem/5413147> BTMM: Should not register private addresses or zeros

Revision 1.436  2007/08/08 21:07:48  vazquez
<rdar://problem/5244687> BTMM: Need to advertise model information via wide-area bonjour

Revision 1.435  2007/08/03 02:04:09  vazquez
<rdar://problem/5371843> BTMM: Private LLQs never fall back to polling
Fix case where NAT-PMP returns an external address but does not support
port mappings. Undo previous change and now, if the router returns an
error in the reply packet we respect it.

Revision 1.434  2007/08/02 21:03:05  vazquez
Change NAT logic to fix case where base station with port mapping turned off
returns an external address but does not make port mappings.

Revision 1.433  2007/08/02 03:30:11  vazquez
<rdar://problem/5371843> BTMM: Private LLQs never fall back to polling

Revision 1.432  2007/08/01 18:15:19  cheshire
Fixed crash in tcpCallback; fixed some problems with LLQ setup behind NAT

Revision 1.431  2007/08/01 16:11:06  cheshire
Fixed "mixed declarations and code" compiler error in Posix build

Revision 1.430  2007/08/01 16:09:13  cheshire
Removed unused NATTraversalInfo substructure from AuthRecord; reduced structure sizecheck values accordingly

Revision 1.429  2007/08/01 03:09:22  cheshire
<rdar://problem/5344587> BTMM: Create NAT port mapping for autotunnel port

Revision 1.428  2007/08/01 01:43:36  cheshire
Need to do mDNS_DropLockBeforeCallback/ReclaimLock around invokation of NAT client callback

Revision 1.427  2007/08/01 01:31:13  cheshire
Need to initialize traversal->tcpInfo fields or code may crash

Revision 1.426  2007/08/01 01:15:57  cheshire
<rdar://problem/5375791> Need to invoke NAT client callback when not on RFC1918 private network

Revision 1.425  2007/08/01 00:04:14  cheshire
<rdar://problem/5261696> Crash in tcpKQSocketCallback
Half-open TCP connections were not being cancelled properly

Revision 1.424  2007/07/31 02:28:35  vazquez
<rdar://problem/3734269> NAT-PMP: Detect public IP address changes and base station reboot

Revision 1.423  2007/07/30 23:31:26  cheshire
Code for respecting TTL received in uDNS responses should exclude LLQ-type responses

Revision 1.422  2007/07/28 01:25:57  cheshire
<rdar://problem/4780038> BTMM: Add explicit UDP event port to LLQ setup request, to fix LLQs not working behind NAT

Revision 1.421  2007/07/28 00:04:14  cheshire
Various fixes for comments and debugging messages

Revision 1.420  2007/07/27 23:59:18  cheshire
Added compile-time structure size checks

Revision 1.419  2007/07/27 20:52:29  cheshire
Made uDNS_recvLLQResponse() return tri-state result: LLQ_Not, LLQ_First, or LLQ_Events

Revision 1.418  2007/07/27 20:32:05  vazquez
Flag a UPnP NAT traversal before starting a UPnP port mapping, and make sure all
calls to mDNS_StopNATOperation() go through the UPnP code

Revision 1.417  2007/07/27 20:19:42  cheshire
Use MDNS_LOG_VERBOSE_DEBUG for dumping out packets instead of MDNS_LOG_DEBUG

Revision 1.416  2007/07/27 19:59:28  cheshire
MUST NOT touch m->CurrentQuestion (or q) after calling AnswerCurrentQuestionWithResourceRecord()

Revision 1.415  2007/07/27 19:51:01  cheshire
Use symbol QC_addnocache instead of literal constant "2"

Revision 1.414  2007/07/27 19:30:39  cheshire
Changed mDNSQuestionCallback parameter from mDNSBool to QC_result,
to properly reflect tri-state nature of the possible responses

Revision 1.413  2007/07/27 18:44:01  cheshire
Rename "AnswerQuestionWithResourceRecord" to more informative "AnswerCurrentQuestionWithResourceRecord"

Revision 1.412  2007/07/27 18:38:56  cheshire
Rename "uDNS_CheckQuery" to more informative "uDNS_CheckCurrentQuestion"

Revision 1.411  2007/07/27 00:57:13  cheshire
Create hostname address records using standard kHostNameTTL (2 minutes) instead of 1 second

Revision 1.410  2007/07/25 21:41:00  vazquez
Make sure we clean up opened sockets when there are network transitions and when changing
port mappings

Revision 1.409  2007/07/25 03:05:02  vazquez
Fixes for:
<rdar://problem/5338913> LegacyNATTraversal: UPnP heap overflow
<rdar://problem/5338933> LegacyNATTraversal: UPnP stack buffer overflow
and a myriad of other security problems

Revision 1.408  2007/07/24 21:47:51  cheshire
Don't do mDNS_StopNATOperation() for operations we never started

Revision 1.407  2007/07/24 17:23:33  cheshire
<rdar://problem/5357133> Add list validation checks for debugging

Revision 1.406  2007/07/24 04:14:30  cheshire
<rdar://problem/5356281> LLQs not working in with NAT Traversal

Revision 1.405  2007/07/24 01:29:03  cheshire
<rdar://problem/5356026> DNSServiceNATPortMappingCreate() returns stale external address information

Revision 1.404  2007/07/20 23:10:51  cheshire
Fix code layout

Revision 1.403  2007/07/20 20:12:37  cheshire
Rename "mDNS_DomainTypeBrowseLegacy" as "mDNS_DomainTypeBrowseAutomatic"

Revision 1.402  2007/07/20 00:54:20  cheshire
<rdar://problem/4641118> Need separate SCPreferences for per-user .Mac settings

Revision 1.401  2007/07/18 03:23:33  cheshire
In GetServiceTarget, need to call SetupLocalAutoTunnelInterface_internal to bring up tunnel on demand, if necessary

Revision 1.400  2007/07/18 02:30:25  cheshire
Defer AutoTunnel server record advertising until we have at least one service to advertise
Do AutoTunnel target host selection in GetServiceTarget (instead of uDNS_RegisterService)

Revision 1.399  2007/07/18 01:02:28  cheshire
<rdar://problem/5304766> Register IPSec tunnel with IPv4-only hostname and create NAT port mappings
Declare records as kDNSRecordTypeKnownUnique so we don't get name conflicts with ourselves

Revision 1.398  2007/07/16 23:54:48  cheshire
<rdar://problem/5338850> Crash when removing or changing DNS keys

Revision 1.397  2007/07/16 20:13:31  vazquez
<rdar://problem/3867231> LegacyNATTraversal: Need complete rewrite

Revision 1.396  2007/07/14 00:33:04  cheshire
Remove temporary IPv4LL tunneling mode now that IPv6-over-IPv4 is working

Revision 1.395  2007/07/12 23:56:23  cheshire
Change "GetZoneData GOT SRV" message to debugf to reduce verbosity in syslog

Revision 1.394  2007/07/12 23:36:08  cheshire
Changed some 'LogOperation' calls to 'debugf' to reduce verbosity in syslog

Revision 1.393  2007/07/12 22:15:10  cheshire
Modified mDNS_SetSecretForDomain() so it can be called to update an existing entry

Revision 1.392  2007/07/12 02:51:27  cheshire
<rdar://problem/5303834> Automatically configure IPSec policy when resolving services

Revision 1.391  2007/07/11 23:16:31  cheshire
<rdar://problem/5304766> Register IPSec tunnel with IPv4-only hostname and create NAT port mappings
Need to prepend _autotunnel._udp to start of AutoTunnel SRV record name

Revision 1.390  2007/07/11 22:47:55  cheshire
<rdar://problem/5303807> Register IPv6-only hostname and don't create port mappings for services
In mDNS_SetSecretForDomain(), don't register records until after we've validated the parameters

Revision 1.389  2007/07/11 21:33:10  cheshire
<rdar://problem/5304766> Register IPSec tunnel with IPv4-only hostname and create NAT port mappings
Set up and register AutoTunnelTarget and AutoTunnelService DNS records

Revision 1.388  2007/07/11 19:27:10  cheshire
<rdar://problem/5303807> Register IPv6-only hostname and don't create port mappings for services
For temporary testing fake up an IPv4LL address instead of IPv6 ULA

Revision 1.387  2007/07/11 03:04:08  cheshire
<rdar://problem/5303807> Register IPv6-only hostname and don't create port mappings for AutoTunnel services
Add AutoTunnel parameter to mDNS_SetSecretForDomain; Set up AutoTunnel information for domains that require it

Revision 1.386  2007/07/10 01:57:28  cheshire
<rdar://problem/5196524> uDNS: mDNSresponder is leaking TCP connections to DNS server
Turned vast chunks of replicated code into a subroutine MakeTCPConn(...);
Made routines hold on to the reference it returns instead of leaking it

Revision 1.385  2007/07/09 23:50:18  cheshire
unlinkSRS needs to call mDNS_StopNATOperation_internal(), not mDNS_StopNATOperation()

Revision 1.384  2007/07/06 21:20:21  cheshire
Fix scheduling error (was causing "Task Scheduling Error: Continuously busy for more than a second")

Revision 1.383  2007/07/06 18:59:59  cheshire
Avoid spinning in an infinite loop when uDNS_SendNATMsg() returns an error

Revision 1.382  2007/07/04 00:49:43  vazquez
Clean up extraneous comments

Revision 1.381  2007/07/03 00:41:14  vazquez
 More changes for <rdar://problem/5301908> Clean up NAT state machine (necessary for 6 other fixes)
 Safely deal with packet replies and client callbacks

Revision 1.380  2007/07/02 22:08:47  cheshire
Fixed crash in "Received public IP address" message

Revision 1.379  2007/06/29 00:08:49  vazquez
<rdar://problem/5301908> Clean up NAT state machine (necessary for 6 other fixes)

Revision 1.378  2007/06/27 20:25:10  cheshire
Expanded dnsbugtest comment, explaining requirement that we also need these
test queries to black-hole before they get to the root name servers.

Revision 1.377  2007/06/22 21:27:21  cheshire
Modified "could not convert shared secret from base64" log message

Revision 1.376  2007/06/20 01:10:12  cheshire
<rdar://problem/5280520> Sync iPhone changes into main mDNSResponder code

Revision 1.375  2007/06/15 21:54:51  cheshire
<rdar://problem/4883206> Add packet logging to help debugging private browsing over TLS

Revision 1.374  2007/06/12 02:15:26  cheshire
Fix incorrect "DNS Server passed" LogOperation message

Revision 1.373  2007/05/31 00:25:43  cheshire
<rdar://problem/5238688> Only send dnsbugtest query for questions where it's warranted

Revision 1.372  2007/05/25 17:03:45  cheshire
lenptr needs to be declared unsigned, otherwise sign extension can mess up the shifting and ORing operations

Revision 1.371  2007/05/24 00:11:44  cheshire
Remove unnecessary lenbuf field from tcpInfo_t

Revision 1.370  2007/05/23 00:30:59  cheshire
Don't change question->TargetQID when repeating query over TCP

Revision 1.369  2007/05/21 18:04:40  cheshire
Updated comments -- port_mapping_create_reply renamed to port_mapping_reply

Revision 1.368  2007/05/17 19:12:16  cheshire
Updated comment about finding matching pair of sockets

Revision 1.367  2007/05/15 23:38:00  cheshire
Need to grab lock before calling sendRecordRegistration();

Revision 1.366  2007/05/15 00:43:05  cheshire
<rdar://problem/4983538> uDNS serviceRegistrationCallback locking failures

Revision 1.365  2007/05/10 21:19:18  cheshire
Rate-limit DNS test queries to at most one per three seconds
(useful when we have a dozen active WAB queries, and then we join a new network)

Revision 1.364  2007/05/07 20:43:45  cheshire
<rdar://problem/4241419> Reduce the number of queries and announcements

Revision 1.363  2007/05/04 22:12:48  cheshire
Work towards solving <rdar://problem/5176892> "uDNS_CheckQuery: LastQTime" log messages
When code gets in this invalid state, double ThisQInterval each time, to avoid excessive logging

Revision 1.362  2007/05/04 21:23:05  cheshire
<rdar://problem/5167263> Private DNS always returns no answers in the initial LLQ setup response
Preparatory work to enable us to do a four-way LLQ handshake over TCP, if we decide that's what we want

Revision 1.361  2007/05/03 23:50:48  cheshire
<rdar://problem/4669229> mDNSResponder ignores bogus null target in SRV record
In the case of negative answers for the address record, set the server address to zerov4Addr

Revision 1.360  2007/05/03 22:40:38  cheshire
<rdar://problem/4669229> mDNSResponder ignores bogus null target in SRV record

Revision 1.359  2007/05/02 22:21:33  cheshire
<rdar://problem/5167331> RegisterRecord and RegisterService need to cancel StartGetZoneData

Revision 1.358  2007/05/01 21:46:31  cheshire
Move GetLLQOptData/GetPktLease from uDNS.c into DNSCommon.c so that dnsextd can use them

Revision 1.357  2007/05/01 01:33:49  cheshire
Removed "#define LLQ_Info DNSQuestion" and manually reconciled code that was still referring to "LLQ_Info"

Revision 1.356  2007/04/30 21:51:06  cheshire
Updated comments

Revision 1.355  2007/04/30 21:33:38  cheshire
Fix crash when a callback unregisters a service while the UpdateSRVRecords() loop
is iterating through the m->ServiceRegistrations list

Revision 1.354  2007/04/30 01:30:04  cheshire
GetZoneData_QuestionCallback needs to call client callback function on error, so client knows operation is finished
RecordRegistrationCallback and serviceRegistrationCallback need to clear nta reference when they're invoked

Revision 1.353  2007/04/28 01:28:25  cheshire
Fixed memory leak on error path in FoundDomain

Revision 1.352  2007/04/27 19:49:53  cheshire
In uDNS_ReceiveTestQuestionResponse, also check that srcport matches

Revision 1.351  2007/04/27 19:28:02  cheshire
Any code that calls StartGetZoneData needs to keep a handle to the structure, so
it can cancel it if necessary. (First noticed as a crash in Apple Remote Desktop
-- it would start a query and then quickly cancel it, and then when
StartGetZoneData completed, it had a dangling pointer and crashed.)

Revision 1.350  2007/04/26 22:47:14  cheshire
Defensive coding: tcpCallback only needs to check "if (closed)", not "if (!n && closed)"

Revision 1.349  2007/04/26 16:04:06  cheshire
In mDNS_AddDNSServer, check whether port matches
In uDNS_CheckQuery, handle case where startLLQHandshake changes q->llq->state to LLQ_Poll

Revision 1.348  2007/04/26 04:01:59  cheshire
Copy-and-paste error: Test should be "if (result == DNSServer_Passed)" not "if (result == DNSServer_Failed)"

Revision 1.347  2007/04/26 00:35:15  cheshire
<rdar://problem/5140339> uDNS: Domain discovery not working over VPN
Fixes to make sure results update correctly when connectivity changes (e.g. a DNS server
inside the firewall may give answers where a public one gives none, and vice versa.)

Revision 1.346  2007/04/25 19:16:59  cheshire
Don't set SuppressStdPort53Queries unless we do actually send a DNS packet

Revision 1.345  2007/04/25 18:05:11  cheshire
Don't try to restart inactive (duplicate) queries

Revision 1.344  2007/04/25 17:54:07  cheshire
Don't cancel Private LLQs using a clear-text UDP packet

Revision 1.343  2007/04/25 16:40:08  cheshire
Add comment explaining uDNS_recvLLQResponse logic

Revision 1.342  2007/04/25 02:14:38  cheshire
<rdar://problem/4246187> uDNS: Identical client queries should reference a single shared core query
Additional fixes to make LLQs work properly

Revision 1.341  2007/04/24 02:07:42  cheshire
<rdar://problem/4246187> Identical client queries should reference a single shared core query
Deleted some more redundant code

Revision 1.340  2007/04/23 22:01:23  cheshire
<rdar://problem/5094009> IPv6 filtering in AirPort base station breaks Wide-Area Bonjour
As of March 2007, AirPort base stations now block incoming IPv6 connections by default, so there's no point
advertising IPv6 addresses in DNS any more -- we have to assume that most of the time a host's IPv6 address
probably won't work for incoming connections (but its IPv4 address probably will, using NAT-PMP).

Revision 1.339  2007/04/22 06:02:03  cheshire
<rdar://problem/4615977> Query should immediately return failure when no server

Revision 1.338  2007/04/21 19:44:11  cheshire
Improve uDNS_HandleNATPortMapReply log message

Revision 1.337  2007/04/21 02:03:00  cheshire
Also need to set AddressRec->resrec.RecordType in the NAT case too

Revision 1.336  2007/04/20 21:16:12  cheshire
Fixed bogus double-registration of host name -- was causing these warning messages in syslog:
Error! Tried to register AuthRecord 0181FB0C host.example.com. (Addr) that's already in the list

Revision 1.335  2007/04/19 23:57:20  cheshire
Temporary workaround for some AirPort base stations that don't seem to like us requesting public port zero

Revision 1.334  2007/04/19 23:21:51  cheshire
Fixed a couple of places where the StartGetZoneData check was backwards

Revision 1.333  2007/04/19 22:50:53  cheshire
<rdar://problem/4246187> Identical client queries should reference a single shared core query

Revision 1.332  2007/04/19 20:34:32  cheshire
Add debugging log message in uDNS_CheckQuery()

Revision 1.331  2007/04/19 20:06:41  cheshire
Rename field 'Private' (sounds like a boolean) to more informative 'AuthInfo' (it's a DomainAuthInfo pointer)

Revision 1.330  2007/04/19 19:51:54  cheshire
Get rid of unnecessary initializeQuery() routine

Revision 1.329  2007/04/19 18:03:52  cheshire
Improved "mDNS_AddSearchDomain" log message

Revision 1.328  2007/04/18 20:57:20  cheshire
Commented out "GetAuthInfoForName none found" debugging message

Revision 1.327  2007/04/17 19:21:29  cheshire
<rdar://problem/5140339> Domain discovery not working over VPN

Revision 1.326  2007/04/16 20:49:39  cheshire
Fix compile errors for mDNSPosix build

Revision 1.325  2007/04/05 22:55:35  cheshire
<rdar://problem/5077076> Records are ending up in Lighthouse without expiry information

Revision 1.324  2007/04/05 20:43:30  cheshire
Collapse sprawling code onto one line -- this is part of a bigger block of identical
code that has been copied-and-pasted into six different places in the same file.
This really needs to be turned into a subroutine.

Revision 1.323  2007/04/04 21:48:52  cheshire
<rdar://problem/4720694> Combine unicast authoritative answer list with multicast list

Revision 1.322  2007/04/03 19:53:06  cheshire
Use mDNSSameIPPort (and similar) instead of accessing internal fields directly

Revision 1.321  2007/04/02 23:44:09  cheshire
Minor code tidying

Revision 1.320  2007/03/31 01:26:13  cheshire
Take out GetAuthInfoForName syslog message

Revision 1.319  2007/03/31 01:10:53  cheshire
Add debugging

Revision 1.318  2007/03/31 00:17:11  cheshire
Remove some LogMsgs

Revision 1.317  2007/03/29 00:09:31  cheshire
Improve "uDNS_InitLongLivedQuery" log message

Revision 1.316  2007/03/28 21:16:27  cheshire
Remove DumpPacket() call now that OPT pseudo-RR rrclass bug is fixed

Revision 1.315  2007/03/28 21:02:18  cheshire
<rdar://problem/3810563> Wide-Area Bonjour should work on multi-subnet private network

Revision 1.314  2007/03/28 15:56:37  cheshire
<rdar://problem/5085774> Add listing of NAT port mapping and GetAddrInfo requests in SIGINFO output

Revision 1.313  2007/03/28 01:27:32  cheshire
<rdar://problem/4996439> Unicast DNS polling server every three seconds
StartLLQPolling was using INIT_UCAST_POLL_INTERVAL instead of LLQ_POLL_INTERVAL for the retry interval

Revision 1.312  2007/03/27 23:48:21  cheshire
Use mDNS_StopGetDomains(), not mDNS_StopQuery()

Revision 1.311  2007/03/27 22:47:51  cheshire
Remove unnecessary "*(long*)0 = 0;" to generate crash and stack trace

Revision 1.310  2007/03/24 01:24:13  cheshire
Add validator for uDNS data structures; fixed crash in RegisterSearchDomains()

Revision 1.309  2007/03/24 00:47:53  cheshire
<rdar://problem/4983538> serviceRegistrationCallback: Locking Failure! mDNS_busy (1) != mDNS_reentrancy (2)
Locking in this file is all messed up. For now we'll just work around the issue.

Revision 1.308  2007/03/24 00:41:33  cheshire
Minor code cleanup (move variable declarations to minimum enclosing scope)

Revision 1.307  2007/03/21 23:06:00  cheshire
Rename uDNS_HostnameInfo to HostnameInfo; deleted some unused fields

Revision 1.306  2007/03/21 00:30:03  cheshire
<rdar://problem/4789455> Multiple errors in DNameList-related code

Revision 1.305  2007/03/20 17:07:15  cheshire
Rename "struct uDNS_TCPSocket_struct" to "TCPSocket", "struct uDNS_UDPSocket_struct" to "UDPSocket"

Revision 1.304  2007/03/17 00:02:11  cheshire
<rdar://problem/5067013> NAT-PMP: Lease TTL is being ignored

Revision 1.303  2007/03/10 03:26:44  cheshire
<rdar://problem/4961667> uDNS: LLQ refresh response packet causes cached records to be removed from cache

Revision 1.302  2007/03/10 02:29:58  cheshire
Added comments about NAT-PMP response functions

Revision 1.301  2007/03/10 02:02:58  cheshire
<rdar://problem/4961667> uDNS: LLQ refresh response packet causes cached records to be removed from cache
Eliminate unnecessary "InternalResponseHndlr responseCallback" function pointer

Revision 1.300  2007/03/08 18:56:00  cheshire
Fixed typo: "&v4.ip.v4.b[0]" is always non-zero (ampersand should not be there)

Revision 1.299  2007/02/28 01:45:47  cheshire
<rdar://problem/4683261> NAT-PMP: Port mapping refreshes should contain actual public port
<rdar://problem/5027863> Byte order bugs in uDNS.c, uds_daemon.c, dnssd_clientstub.c

Revision 1.298  2007/02/14 03:16:39  cheshire
<rdar://problem/4789477> Eliminate unnecessary malloc/free in mDNSCore code

Revision 1.297  2007/02/08 21:12:28  cheshire
<rdar://problem/4386497> Stop reading /etc/mDNSResponder.conf on every sleep/wake

Revision 1.296  2007/01/29 16:03:22  cheshire
Fix unused parameter warning

Revision 1.295  2007/01/27 03:34:27  cheshire
Made GetZoneData use standard queries (and cached results);
eliminated GetZoneData_Callback() packet response handler

Revision 1.294  2007/01/25 00:40:16  cheshire
Unified CNAME-following functionality into cache management code (which means CNAME-following
should now also work for mDNS queries too); deleted defunct pktResponseHndlr() routine.

Revision 1.293  2007/01/23 02:56:11  cheshire
Store negative results in the cache, instead of generating them out of pktResponseHndlr()

Revision 1.292  2007/01/20 01:32:40  cheshire
Update comments and debugging messages

Revision 1.291  2007/01/20 00:07:02  cheshire
When we have credentials in the keychain for a domain, we attempt private queries, but
if the authoritative server is not set up for private queries (i.e. no _dns-query-tls
or _dns-llq-tls record) then we need to fall back to conventional non-private queries.

Revision 1.290  2007/01/19 23:41:45  cheshire
Need to clear m->rec.r.resrec.RecordType after calling GetLLQOptData()

Revision 1.289  2007/01/19 23:32:07  cheshire
Eliminate pointless timenow variable

Revision 1.288  2007/01/19 23:26:08  cheshire
Right now tcpCallback does not run holding the lock, so no need to drop the lock before invoking callbacks

Revision 1.287  2007/01/19 22:55:41  cheshire
Eliminate redundant identical parameters to GetZoneData_StartQuery()

Revision 1.286  2007/01/19 21:17:33  cheshire
StartLLQPolling needs to call SetNextQueryTime() to cause query to be done in a timely fashion

Revision 1.285  2007/01/19 18:39:11  cheshire
Fix a bunch of parameters that should have been declared "const"

Revision 1.284  2007/01/19 18:28:28  cheshire
Improved debugging messages

Revision 1.283  2007/01/19 18:09:33  cheshire
Fixed getLLQAtIndex (now called GetLLQOptData):
1. It incorrectly assumed all EDNS0 OPT records are the same size (it ignored optlen)
2. It used inefficient memory copying instead of just returning a pointer

Revision 1.282  2007/01/17 22:06:01  cheshire
Replace duplicated literal constant "{ { 0 } }" with symbol "zeroIPPort"

Revision 1.281  2007/01/17 21:58:13  cheshire
For clarity, rename ntaContext field "isPrivate" to "ntaPrivate"

Revision 1.280  2007/01/17 21:46:02  cheshire
Remove redundant duplicated "isPrivate" field from LLQ_Info

Revision 1.279  2007/01/17 21:35:31  cheshire
For clarity, rename zoneData_t field "isPrivate" to "zonePrivate"

Revision 1.278  2007/01/16 03:04:16  cheshire
<rdar://problem/4917539> Add support for one-shot private queries as well as long-lived private queries
Don't cache result of ntaContextSRV(context) in a local variable --
the macro evaluates to a different result after we clear "context->isPrivate"

Revision 1.277  2007/01/10 22:51:58  cheshire
<rdar://problem/4917539> Add support for one-shot private queries as well as long-lived private queries

Revision 1.276  2007/01/10 02:09:30  cheshire
Better LogOperation record of keys read from System Keychain

Revision 1.275  2007/01/09 22:37:18  cheshire
Provide ten-second grace period for deleted keys, to give mDNSResponder
time to delete host name before it gives up access to the required key.

Revision 1.274  2007/01/09 01:16:32  cheshire
Improve "ERROR m->CurrentQuestion already set" debugging messages

Revision 1.273  2007/01/08 23:58:00  cheshire
Don't request regDomain and browseDomains in uDNS_SetupDNSConfig() -- it just ignores those results

Revision 1.272  2007/01/05 08:30:42  cheshire
Trim excessive "$Log" checkin history from before 2006
(checkin history still available via "cvs log ..." of course)

Revision 1.271  2007/01/05 06:34:03  cheshire
Improve "ERROR m->CurrentQuestion already set" debugging messages

Revision 1.270  2007/01/05 05:44:33  cheshire
Move automatic browse/registration management from uDNS.c to mDNSShared/uds_daemon.c,
so that mDNSPosix embedded clients will compile again

Revision 1.269  2007/01/04 23:11:13  cheshire
<rdar://problem/4720673> uDNS: Need to start caching unicast records
When an automatic browsing domain is removed, generate appropriate "remove" events for legacy queries

Revision 1.268  2007/01/04 22:06:38  cheshire
Fixed crash in LLQNatMapComplete()

Revision 1.267  2007/01/04 21:45:20  cheshire
Added mDNS_DropLockBeforeCallback/mDNS_ReclaimLockAfterCallback macros,
to do additional lock sanity checking around callback invocations

Revision 1.266  2007/01/04 21:01:20  cheshire
<rdar://problem/4607042> mDNSResponder NXDOMAIN and CNAME support
Only return NXDOMAIN results to clients that request them using kDNSServiceFlagsReturnIntermediates

Revision 1.265  2007/01/04 20:47:17  cheshire
Fixed crash in CheckForUnreferencedLLQMapping()

Revision 1.264  2007/01/04 20:39:27  cheshire
Fix locking mismatch

Revision 1.263  2007/01/04 02:39:53  cheshire
<rdar://problem/4030599> Hostname passed into DNSServiceRegister ignored for Wide-Area service registrations

Revision 1.262  2007/01/04 00:29:25  cheshire
Covert LogMsg() in GetAuthInfoForName to LogOperation()

Revision 1.261  2006/12/22 20:59:49  cheshire
<rdar://problem/4742742> Read *all* DNS keys from keychain,
 not just key for the system-wide default registration domain

Revision 1.260  2006/12/21 00:06:07  cheshire
Don't need to do mDNSPlatformMemZero() -- mDNS_SetupResourceRecord() does it for us

Revision 1.259  2006/12/20 04:07:36  cheshire
Remove uDNS_info substructure from AuthRecord_struct

Revision 1.258  2006/12/19 22:49:24  cheshire
Remove uDNS_info substructure from ServiceRecordSet_struct

Revision 1.257  2006/12/19 02:38:20  cheshire
Get rid of unnecessary duplicate query ID field from DNSQuestion_struct

Revision 1.256  2006/12/19 02:18:48  cheshire
Get rid of unnecessary duplicate "void *context" field from DNSQuestion_struct

Revision 1.255  2006/12/16 01:58:31  cheshire
<rdar://problem/4720673> uDNS: Need to start caching unicast records

Revision 1.254  2006/12/15 19:23:39  cheshire
Use new DomainNameLengthLimit() function, to be more defensive against malformed
data received from the network.

Revision 1.253  2006/12/01 07:43:34  herscher
Fix byte ordering problem for one-shot TCP queries.
Iterate more intelligently over duplicates in uDNS_ReceiveMsg to avoid spin loops.

Revision 1.252  2006/11/30 23:07:57  herscher
<rdar://problem/4765644> uDNS: Sync up with Lighthouse changes for Private DNS

Revision 1.251  2006/11/28 21:42:11  mkrochma
Work around a crashing bug that was introduced by uDNS and mDNS code unification

Revision 1.250  2006/11/18 05:01:30  cheshire
Preliminary support for unifying the uDNS and mDNS code,
including caching of uDNS answers

Revision 1.249  2006/11/10 07:44:04  herscher
<rdar://problem/4825493> Fix Daemon locking failures while toggling BTMM

Revision 1.248  2006/11/08 04:26:53  cheshire
Fix typo in debugging message

Revision 1.247  2006/10/20 05:35:04  herscher
<rdar://problem/4720713> uDNS: Merge unicast active question list with multicast list.

Revision 1.246  2006/10/11 19:29:41  herscher
<rdar://problem/4744553> uDNS: mDNSResponder-111 using 100% CPU

Revision 1.245  2006/10/04 22:21:15  herscher
Tidy up references to mDNS_struct introduced when the embedded uDNS_info struct was removed.

Revision 1.244  2006/10/04 21:51:27  herscher
Replace calls to mDNSPlatformTimeNow(m) with m->timenow

Revision 1.243  2006/10/04 21:38:59  herscher
Remove uDNS_info substructure from DNSQuestion_struct

Revision 1.242  2006/09/27 00:51:46  herscher
Fix compile error when _LEGACY_NAT_TRAVERSAL_ is not defined

Revision 1.241  2006/09/26 01:54:47  herscher
<rdar://problem/4245016> NAT Port Mapping API (for both NAT-PMP and UPnP Gateway Protocol)

Revision 1.240  2006/09/15 21:20:15  cheshire
Remove uDNS_info substructure from mDNS_struct

Revision 1.239  2006/08/16 02:52:56  mkrochma
<rdar://problem/4104154> Actually fix it this time

Revision 1.238  2006/08/16 00:31:50  mkrochma
<rdar://problem/4386944> Get rid of NotAnInteger references

Revision 1.237  2006/08/15 23:38:17  mkrochma
<rdar://problem/4104154> Requested Public Port field should be set to zero on mapping deletion

Revision 1.236  2006/08/14 23:24:23  cheshire
Re-licensed mDNSResponder daemon source code under Apache License, Version 2.0

Revision 1.235  2006/07/30 05:45:36  cheshire
<rdar://problem/4304215> Eliminate MIN_UCAST_PERIODIC_EXEC

Revision 1.234  2006/07/22 02:58:36  cheshire
Code was clearing namehash twice instead of namehash and rdatahash

Revision 1.233  2006/07/20 19:46:51  mkrochma
<rdar://problem/4472014> Add Private DNS client functionality to mDNSResponder
Fix Private DNS

Revision 1.232  2006/07/15 02:01:29  cheshire
<rdar://problem/4472014> Add Private DNS client functionality to mDNSResponder
Fix broken "empty string" browsing

Revision 1.231  2006/07/05 23:28:22  cheshire
<rdar://problem/4472014> Add Private DNS client functionality to mDNSResponder

Revision 1.230  2006/06/29 03:02:44  cheshire
<rdar://problem/4607042> mDNSResponder NXDOMAIN and CNAME support

Revision 1.229  2006/03/02 22:03:41  cheshire
<rdar://problem/4395331> Spurious warning "GetLargeResourceRecord: m->rec appears to be already in use"
Refinement: m->rec.r.resrec.RecordType needs to be cleared *every* time around for loop, not just once at the end

Revision 1.228  2006/02/26 00:54:42  cheshire
Fixes to avoid code generation warning/error on FreeBSD 7

Revision 1.227  2006/01/09 20:47:05  cheshire
<rdar://problem/4395331> Spurious warning "GetLargeResourceRecord: m->rec appears to be already in use"

*/

#include "uDNS.h"
#include "dns_sd.h"

#if(defined(_MSC_VER))
	// Disable "assignment within conditional expression".
	// Other compilers understand the convention that if you place the assignment expression within an extra pair
	// of parentheses, this signals to the compiler that you really intended an assignment and no warning is necessary.
	// The Microsoft compiler doesn't understand this convention, so in the absense of any other way to signal
	// to the compiler that the assignment is intentional, we have to just turn this warning off completely.
	#pragma warning(disable:4706)
#endif

typedef struct tcpInfo_t
	{
	mDNS             *m;
	TCPSocket        *sock;
	DNSMessage        request;
	int               requestLen;
	DNSQuestion      *question;   // For queries
	ServiceRecordSet *srs;        // For service record updates
	AuthRecord       *rr;         // For record updates
	mDNSAddr          Addr;
	mDNSIPPort        Port;
	DNSMessage       *reply;
	mDNSu16           replylen;
	unsigned long     nread;
	int               numReplies;
	} tcpInfo_t;

typedef struct SearchListElem
	{
	struct SearchListElem *next;
	domainname domain;
	int flag;		// -1 means delete, 0 means unchanged, +1 means newly added
	DNSQuestion BrowseQ;
	DNSQuestion DefBrowseQ;
	DNSQuestion AutomaticBrowseQ;
	DNSQuestion RegisterQ;
	DNSQuestion DefRegisterQ;
	ARListElem *AuthRecs;
	} SearchListElem;

// For domain enumeration and automatic browsing
// This is the user's DNS search list.
// In each of these domains we search for our special pointer records (lb._dns-sd._udp.<domain>, etc.)
// to discover recommended domains for domain enumeration (browse, default browse, registration,
// default registration) and possibly one or more recommended automatic browsing domains.
static SearchListElem *SearchList = mDNSNULL;

// Temporary workaround to make ServiceRecordSet list management safe.
// Ideally a ServiceRecordSet shouldn't be a special entity that's given special treatment by the uDNS code
// -- it should just be a grouping of records that are treated the same as any other registered records.
// In that case it may no longer be necessary to keep an explicit list of ServiceRecordSets, which in turn
// would avoid the perils of modifying that list cleanly while some other piece of code is iterating through it.
static ServiceRecordSet *CurrentServiceRecordSet = mDNSNULL;

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - General Utility Functions
#endif

// Unlink an AuthRecord from the m->ResourceRecords list.
// This seems risky. Probably some (or maybe all) of the places calling UnlinkAuthRecord to directly
// remove a record from the list should actually be using mDNS_Deregister/mDNS_Deregister_internal.
mDNSlocal mStatus UnlinkAuthRecord(mDNS *const m, AuthRecord *const rr)
	{
	AuthRecord **list = &m->ResourceRecords;
	while (*list && *list != rr) list = &(*list)->next;
	if (!*list)
		{
		list = &m->DuplicateRecords;
		while (*list && *list != rr) list = &(*list)->next;
		}
	if (*list) { *list = rr->next; rr->next = mDNSNULL; return(mStatus_NoError); }
	LogMsg("ERROR: UnlinkAuthRecord - no such active record %##s", rr->resrec.name->c);
	return(mStatus_NoSuchRecord);
	}

// unlinkSRS is an internal routine (i.e. must be called with the lock already held)
mDNSlocal void unlinkSRS(mDNS *const m, ServiceRecordSet *srs)
	{
	ServiceRecordSet **p;

	if (srs->NATinfo.clientContext)
		{
		mDNS_StopNATOperation_internal(m, &srs->NATinfo);
		srs->NATinfo.clientContext = mDNSNULL;
		}

	for (p = &m->ServiceRegistrations; *p; p = &(*p)->uDNS_next)
		if (*p == srs)
			{
			ExtraResourceRecord *e;
			*p = srs->uDNS_next;
			if (CurrentServiceRecordSet == srs)
				CurrentServiceRecordSet = srs->uDNS_next;
			srs->uDNS_next = mDNSNULL;
			for (e=srs->Extras; e; e=e->next)
				if (UnlinkAuthRecord(m, &e->r))
					LogMsg("unlinkSRS: extra record %##s not found", e->r.resrec.name->c);
			return;
			}
	LogMsg("ERROR: unlinkSRS - SRS not found in ServiceRegistrations list %##s", srs->RR_SRV.resrec.name->c);
	}

// set retry timestamp for record with exponential backoff
// (for service record sets, use RR_SRV as representative for time checks
mDNSlocal void SetRecordRetry(mDNS *const m, AuthRecord *rr, mStatus SendErr)
	{
	rr->LastAPTime = m->timenow;
	if (SendErr == mStatus_TransientErr || rr->ThisAPInterval < INIT_UCAST_POLL_INTERVAL)  rr->ThisAPInterval = INIT_UCAST_POLL_INTERVAL;
	else if (rr->ThisAPInterval*2 <= MAX_UCAST_POLL_INTERVAL)                              rr->ThisAPInterval *= 2;
	else                                                                                   rr->ThisAPInterval = MAX_UCAST_POLL_INTERVAL;
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Name Server List Management
#endif

mDNSexport DNSServer *mDNS_AddDNSServer(mDNS *const m, const domainname *d, const mDNSInterfaceID interface, const mDNSAddr *addr, const mDNSIPPort port)
	{
	DNSServer **p = &m->DNSServers;

	if (!d) d = (const domainname *)"";

	LogOperation("mDNS_AddDNSServer: Adding %#a for %##s", addr, d->c);
	if (m->mDNS_busy != m->mDNS_reentrancy+1)
		LogMsg("mDNS_AddDNSServer: Lock not held! mDNS_busy (%ld) mDNS_reentrancy (%ld)", m->mDNS_busy, m->mDNS_reentrancy);

	while (*p)	// Check if we already have this {server,domain} pair registered
		{
		if ((*p)->interface == interface && (*p)->teststate != DNSServer_Disabled &&
			mDNSSameAddress(&(*p)->addr, addr) && mDNSSameIPPort((*p)->port, port) && SameDomainName(&(*p)->domain, d))
			{
			if (!(*p)->del) LogMsg("Note: DNS Server %#a for domain %##s registered more than once", addr, d->c);
			(*p)->del = mDNSfalse;
			return(*p);
			}
		p=&(*p)->next;
		}

	// allocate, add to list
	*p = mDNSPlatformMemAllocate(sizeof(**p));
	if (!*p) LogMsg("Error: mDNS_AddDNSServer - malloc");
	else
		{
		(*p)->interface = interface;
		(*p)->addr      = *addr;
		(*p)->port      = port;
		(*p)->del       = mDNSfalse;
		(*p)->teststate = DNSServer_Untested;
		(*p)->lasttest  = m->timenow - INIT_UCAST_POLL_INTERVAL;
		AssignDomainName(&(*p)->domain, d);
		(*p)->next = mDNSNULL;
		}
	return(*p);
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - authorization management
#endif

mDNSlocal DomainAuthInfo *GetAuthInfoForName_direct(mDNS *m, const domainname *const name)
	{
	const domainname *n = name;
	while (n->c[0])
		{
		DomainAuthInfo *ptr;
		for (ptr = m->AuthInfoList; ptr; ptr = ptr->next)
			if (SameDomainName(&ptr->domain, n))
				{
				debugf("GetAuthInfoForName %##s Matched %##s Key name %##s", name->c, ptr->domain.c, ptr->keyname.c);
				return(ptr);
				}
		n = (const domainname *)(n->c + 1 + n->c[0]);
		}
	//LogOperation("GetAuthInfoForName none found for %##s", name->c);
	return mDNSNULL;
	}

// MUST be called with lock held
mDNSexport DomainAuthInfo *GetAuthInfoForName_internal(mDNS *m, const domainname *const name)
	{
	DomainAuthInfo **p = &m->AuthInfoList;

	if (m->mDNS_busy != m->mDNS_reentrancy+1)
		LogMsg("GetAuthInfoForName_internal: Lock not held! mDNS_busy (%ld) mDNS_reentrancy (%ld)", m->mDNS_busy, m->mDNS_reentrancy);

	// First purge any dead keys from the list
	while (*p)
		{
		if ((*p)->deltime && m->timenow - (*p)->deltime >= 0)
			{
			DNSQuestion *q;
			DomainAuthInfo *info = *p;
			LogOperation("GetAuthInfoForName_internal deleting expired key %##s %##s", info->domain.c, info->keyname.c);
			*p = info->next;	// Cut DomainAuthInfo from list *before* scanning our question list updating AuthInfo pointers
			for (q = m->Questions; q; q=q->next)
				if (q->AuthInfo == info)
					{
					q->AuthInfo = GetAuthInfoForName_direct(m, &q->qname);
					debugf("GetAuthInfoForName_internal updated q->AuthInfo from %##s to %##s for %##s (%s)",
						info->domain.c, q->AuthInfo ? q->AuthInfo->domain.c : mDNSNULL, q->qname.c, DNSTypeName(q->qtype));
					}

			// Probably not essential, but just to be safe, zero out the secret key data
			// so we don't leave it hanging around in memory
			// (where it could potentially get exposed via some other bug)
			mDNSPlatformMemZero(info, sizeof(*info));
			mDNSPlatformMemFree(info);
			}
		else
			p = &(*p)->next;
		}

	return(GetAuthInfoForName_direct(m, name));
	}

mDNSlocal DomainAuthInfo *GetAuthInfoForName(mDNS *m, const domainname *const name)
	{
	DomainAuthInfo *d;
	mDNS_Lock(m);
	d = GetAuthInfoForName_internal(m, name);
	mDNS_Unlock(m);
	return(d);
	}

// MUST be called with the lock held
mDNSexport mStatus mDNS_SetSecretForDomain(mDNS *m, DomainAuthInfo *info,
	const domainname *domain, const domainname *keyname, const char *b64keydata, mDNSBool AutoTunnel)
	{
	DNSQuestion *q;
	DomainAuthInfo **p = &m->AuthInfoList;
	if (!info || !b64keydata) { LogMsg("mDNS_SetSecretForDomain: ERROR: info %p b64keydata %p", info, b64keydata); return(mStatus_BadParamErr); }

	LogOperation("mDNS_SetSecretForDomain: domain %##s key %##s%s", domain->c, keyname->c, AutoTunnel ? " AutoTunnel" : "");

	info->AutoTunnel = AutoTunnel;
	AssignDomainName(&info->domain,  domain);
	AssignDomainName(&info->keyname, keyname);
	mDNS_snprintf(info->b64keydata, sizeof(info->b64keydata), "%s", b64keydata);

	if (DNSDigest_ConstructHMACKeyfromBase64(info, b64keydata) < 0)
		{
		LogMsg("mDNS_SetSecretForDomain: ERROR: Could not convert shared secret from base64: domain %##s key %##s %s",
			domain->c, keyname->c, LogAllOperations ? b64keydata : "");
		return(mStatus_BadParamErr);
		}

	// Don't clear deltime until after we've ascertained that b64keydata is valid
	info->deltime = 0;

	while (*p && (*p) != info) p=&(*p)->next;
	if (*p) return(mStatus_AlreadyRegistered);

	// Caution: Only zero AutoTunnelHostRecord.namestorage and AutoTunnelNAT.clientContext AFTER we've determined that this is a NEW DomainAuthInfo
	// being added to the list. Otherwise we risk smashing our AutoTunnel host records and NATOperation that are already active and in use.
	info->AutoTunnelHostRecord.namestorage.c[0] = 0;
	info->AutoTunnelNAT.clientContext = mDNSNULL;
	info->next = mDNSNULL;
	*p = info;

	// Check to see if adding this new DomainAuthInfo has changed the credentials for any of our questions
	for (q = m->Questions; q; q=q->next)
		{
		if (q->QuestionCallback != GetZoneData_QuestionCallback)
			{
			DomainAuthInfo *newinfo = GetAuthInfoForName_internal(m, &q->qname);
			if (q->AuthInfo != newinfo)
				{
				debugf("mDNS_SetSecretForDomain updating q->AuthInfo from %##s to %##s for %##s (%s)",
					q->AuthInfo ? q->AuthInfo->domain.c : mDNSNULL,
					newinfo     ? newinfo    ->domain.c : mDNSNULL, q->qname.c, DNSTypeName(q->qtype));
				q->AuthInfo = newinfo;
				}
			}
		}

	return(mStatus_NoError);
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - NAT Traversal
#endif

mDNSlocal void formatPortMapRequest(NATTraversalInfo *info, NATPortMapRequest *NATPortReq, NATOp_t op)
	{
	mDNSu8 *p = (mDNSu8 *)NATPortReq;	// NATPortMapRequest packet
	mDNSIPPort	pubPort = !mDNSIPPortIsZero(info->savedPublicPort) ? info->savedPublicPort : info->publicPortreq;
	mDNSu32 	lease   = info->portMappingLease ? info->portMappingLease : 0;	// zero means deletion

	p[ 0] = NATMAP_VERS;
	p[ 1] = op;
	p[ 2] = 0;	// unused
	p[ 3] = 0;	// unused
	p[ 4] = info->privatePort.b[0];
	p[ 5] = info->privatePort.b[1];
	p[ 6] = pubPort.b[0];
	p[ 7] = pubPort.b[1];
	p[ 8] = (mDNSu8)((lease >> 24) &  0xFF);
	p[ 9] = (mDNSu8)((lease >> 16) &  0xFF);
	p[10] = (mDNSu8)((lease >>  8) &  0xFF);
	p[11] = (mDNSu8)( lease        &  0xFF);
	}

mDNSlocal mStatus uDNS_SendNATMsg(mDNS *m, NATTraversalInfo *info, NATOptFlags_t op)
	{
	mStatus	err = mStatus_NoError;
	const mDNSu8 *msg = mDNSNULL;
	const mDNSu8 *end = mDNSNULL;
	NATAddrRequest		NATAddrReq;
	NATPortMapRequest   NATPortReq;

	// send msg if we have a router and it is a private address
	if (!mDNSIPv4AddressIsZero(m->Router.ip.v4) && mDNSv4AddrIsRFC1918(&m->Router.ip.v4))
		{
		if (op == AddrRequestFlag)
			{
			msg = (const mDNSu8 *)&NATAddrReq;
			end  = msg + sizeof(NATAddrRequest);
			NATAddrReq.vers   = NATMAP_VERS;
			NATAddrReq.opcode = NATOp_AddrRequest;
			}
		else
			{
			msg = (const mDNSu8 *)&NATPortReq;
			end  = msg + sizeof(NATPortMapRequest);
			formatPortMapRequest(info, &NATPortReq, (op == MapUDPFlag) ? NATOp_MapUDP : NATOp_MapTCP);
			}
		
#ifdef _LEGACY_NAT_TRAVERSAL_
		// send nat-pmp packet if we know we're not behind a uPNP gateway
		if (mDNSIPPortIsZero(m->uPNPSOAPPort))
			{
			err = mDNSPlatformSendUDP(m, msg, end, 0, &m->Router, NATPMPPort);
			// if there is no external address then assume we're starting from scratch and start upnp discovery as well
			if (mDNSIPv4AddressIsZero(m->ExternalAddress)) 	LNT_SendDiscoveryMsg(m); 
			}
		else
			{
			// we're behind a uPNP gateway
			if (op == AddrRequestFlag)		err = LNT_GetExternalAddress(m);
			else
				{
				mDNSBool   doTCP = (info->opFlags & MapTCPFlag) ? 1 : 0;
				// initially request priv == pub if publicPortreq is zero
				if (mDNSIPPortIsZero(info->publicPortreq)) info->publicPortreq = info->privatePort;
				info->opFlags |= LegacyFlag;
				err = LNT_MapPort(m, info, doTCP);
				}
			}
#else
		err = mDNSPlatformSendUDP(m, msg, end, 0, &m->Router, NATPMPPort);
#endif // _LEGACY_NAT_TRAVERSAL_
		}
	return(err);
	}

mDNSlocal void SetExternalAddress(mDNS *const m, mDNSv4Addr newaddr, mDNSs32 routerTimeElapsed, mDNSs32 ourTimeElapsed)
	{
	NATTraversalInfo *n;
	
	// check for router reboots or if the external address changed
	if (((routerTimeElapsed * (routerTimeElapsed/8)) - ourTimeElapsed >= 0) && mDNSSameIPv4Address(m->ExternalAddress, newaddr)) return;

	// can't set ExternalAddress to onesIPv4Addr, so reset it to zero
	if (mDNSSameIPv4Address(newaddr, onesIPv4Addr))	m->ExternalAddress = zerov4Addr;
	else m->ExternalAddress = newaddr;
	LogOperation("Received external IP address %.4a from NAT", &m->ExternalAddress);
	if (mDNSv4AddrIsRFC1918(&m->ExternalAddress)) LogMsg("natTraversalHandleAddressReply: Double NAT");
#ifdef _LEGACY_NAT_TRAVERSAL_
	if (m->tcpAddrInfo.sock)   { mDNSPlatformTCPCloseConnection(m->tcpAddrInfo.sock);   m->tcpAddrInfo.sock   = mDNSNULL; }
	if (m->tcpDeviceInfo.sock) { mDNSPlatformTCPCloseConnection(m->tcpDeviceInfo.sock); m->tcpDeviceInfo.sock = mDNSNULL; }
	if (mDNSIPv4AddressIsZero(m->ExternalAddress)) m->uPNPSOAPPort = m->uPNPRouterPort = zeroIPPort;	// reset uPNP ports
#endif // _LEGACY_NAT_TRAVERSAL_

	// NAT gateway changed. Need to refresh or recreate port mappings ASAP
	for (n = m->NATTraversals; n; n=n->next)
		{
		n->retryPortMap         = m->timenow;
		n->retryIntervalPortMap = NATMAP_INIT_RETRY;
		
		// since the public port can on some occassions be set to zero (ie. on a network where port mapping is not supported)
		// we don't want to set lastPublicPort to zero also because then the right transitions don't happen later, so
		// set it to a known invalid value (-1) instead.
		if (mDNSIPPortIsZero(n->lastPublicPort) || mDNSSameIPPort(n->lastPublicPort, mDNSOpaque16fromIntVal(-1)))
			 n->lastPublicPort = mDNSOpaque16fromIntVal(-1);
		else n->lastPublicPort = n->publicPort;
		
		n->publicPort = zeroIPPort;
#ifdef _LEGACY_NAT_TRAVERSAL_
		if (n->tcpInfo.sock) { mDNSPlatformTCPCloseConnection(n->tcpInfo.sock); n->tcpInfo.sock = mDNSNULL; }
#endif // _LEGACY_NAT_TRAVERSAL_
		}

	m->NextScheduledNATOp = m->timenow;
	}

// Pass NULL for pkt on error (including timeout)
mDNSexport void natTraversalHandleAddressReply(mDNS *const m, mDNSu8 *pkt)
	{
	NATAddrReply      *addrReply = (NATAddrReply *)pkt;
	mDNSv4Addr       addr = zerov4Addr;
	mDNSs32            routerTimeElapsed = 0;
	mDNSs32            ourTimeElapsed = 0;
	mStatus               err = mStatus_NoError;
	
	if (!pkt) // timeout
		{
		debugf("natTraversalHandleAddressReply: timeout");
		err = mStatus_NATTraversal;
		m->retryIntervalGetAddr = NATMAP_MAX_RETRY_INTERVAL;
		m->retryGetAddr = m->timenow + m->retryIntervalGetAddr;
		}
	else { err = addrReply->err; addr = addrReply->PubAddr; }
		
	if (err)
		{
		LogMsg("natTraversalHandleAddressReply: received error getting external address %d", err);
		m->retryIntervalGetAddr = NATMAP_MAX_RETRY_INTERVAL;
		m->retryGetAddr = m->timenow + m->retryIntervalGetAddr;
		return;
		}
	
	routerTimeElapsed = addrReply->uptime - m->LastNATUptime;
	m->LastNATUptime = addrReply->uptime;
	ourTimeElapsed = m->timenow - m->LastNATReplyLocalTime;
	m->LastNATReplyLocalTime = m->timenow;

	SetExternalAddress(m, addr, routerTimeElapsed, ourTimeElapsed);
	m->retryIntervalGetAddr = NATMAP_MAX_RETRY_INTERVAL;
	m->retryGetAddr = m->timenow + m->retryIntervalGetAddr;
	}

// Pass NULL for pkt on error (including timeout)
mDNSexport void natTraversalHandlePortMapReply(NATTraversalInfo *n, mDNS *const m, mDNSu8 *pkt)
	{
	NATPortMapReply *portMapReply = (NATPortMapReply *)pkt;
	mDNSIPPort		port  = zeroIPPort;
	mDNSu32			lease = NATMAP_DEFAULT_LEASE;
	mStatus			err   = mStatus_NoError;
	NATOp_t			op;
	mDNSBool		ours = mDNStrue;

	if (!n) { LogMsg("natTraversalHandlePortMapReply called with unknown NAT traversal object"); return; }
	
	if (!pkt) // timeout
		{
		debugf("natTraversalHandlePortMapReply: timeout");
		err = mStatus_NATTraversal;
		n->retryIntervalPortMap = NATMAP_MAX_RETRY_INTERVAL;
		n->retryPortMap = m->timenow + n->retryIntervalPortMap;
		}
	else
		{
		op = (n->opFlags & MapUDPFlag) ? NATOp_MapUDP : NATOp_MapTCP;
		if (!mDNSSameIPPort(n->privatePort, portMapReply->priv) || (op | 0x80) != portMapReply->opcode)
			ours = mDNSfalse;       // packet does not match this request
		err = portMapReply->err;
		port = portMapReply->pub;
		lease = portMapReply->NATRep_lease;
		}
		
	if (err && (ours || mDNSIPPortIsZero(portMapReply->priv)))
		{
		LogOperation("natTraversalHandlePortMapReply: received error making port mapping %d", err);
		n->retryIntervalPortMap = NATMAP_MAX_RETRY_INTERVAL;
		n->retryPortMap = m->timenow + n->retryIntervalPortMap;
		if 		(err == NATErr_Refused) 					n->Error = mStatus_NATPortMappingDisabled;
		else if (err > NATErr_None && err <= NATErr_Opcode)	n->Error = mStatus_NATPortMappingUnsupported;
		else 												n->Error = err;
		return;
		}

	if (!ours || n->portMappingLease == 0) return;           // not ours or deletion

	n->portMappingLease = lease;
	if (n->portMappingLease > 0x70000000UL / mDNSPlatformOneSecond)
		n->portMappingLease = 0x70000000UL / mDNSPlatformOneSecond;
	n->ExpiryTime = m->timenow + n->portMappingLease * mDNSPlatformOneSecond;

	if (!mDNSSameIPPort(n->publicPort, port))
		{
		LogOperation("natTraversalHandlePortMapReply: public port changed from %d to %d", mDNSVal16(n->publicPort), mDNSVal16(port));
		n->publicPort = port;
		}

	n->savedPublicPort = port; // Remember allocated port for future refreshes
	LogOperation("natTraversalHandlePortMapReply %p %X (%s) Local Port %d External Port %d", n,
				portMapReply->opcode,
				portMapReply->opcode == 0x81 ? "UDP Response" :
				portMapReply->opcode == 0x82 ? "TCP Response" : "?", mDNSVal16(portMapReply->priv), mDNSVal16(port));
	n->retryIntervalPortMap = ((mDNSs32)n->portMappingLease * (mDNSPlatformOneSecond / 2));	// retry half way to expiration
	n->retryPortMap = m->timenow + n->retryIntervalPortMap;
	n->Error = err;
	m->NextScheduledNATOp = m->timenow;
	}

// Must be called with the mDNS_Lock held
mDNSexport mStatus mDNS_StartNATOperation_internal(mDNS *const m, NATTraversalInfo *traversal)
	{
	NATTraversalInfo **n;
	
	LogOperation("mDNS_StartNATOperation_internal %d %d %d %d",
		traversal->protocol, mDNSVal16(traversal->privatePort), mDNSVal16(traversal->publicPortreq), traversal->portMappingLease);

	// Note: It important that new traversal requests are appended at the *end* of the list, not prepended at the start
	n = &m->NATTraversals;
	while (*n && *n != traversal) n=&(*n)->next;
	if (*n) { LogMsg("Error! Tried to add a NAT traversal that's already in the active list"); return(mStatus_AlreadyRegistered); }

	// Initialize necessary fields
	traversal->next                 = mDNSNULL;
	traversal->opFlags              = 0;
	traversal->publicPort           = zeroIPPort;
	traversal->lastPublicPort       = mDNSOpaque16fromIntVal(-1);	// see comment in SetExternalAddress() about why this is set to -1
	traversal->savedPublicPort      = zeroIPPort;
	traversal->lastExternalAddress  = onesIPv4Addr;
	traversal->Error                = mStatus_NoError;
	traversal->lastError            = mStatus_NoError;

	// set default lease if necessary
	traversal->portMappingLease = traversal->portMappingLease ? traversal->portMappingLease : NATMAP_DEFAULT_LEASE;

	mDNSPlatformMemZero(&traversal->tcpInfo, sizeof(traversal->tcpInfo));

	traversal->retryPortMap         = 0;
	traversal->retryIntervalPortMap = NATMAP_INIT_RETRY;
	traversal->ExpiryTime           = 0;

	if (!m->NATTraversals)
		{
		m->retryGetAddr = m->timenow;
		m->retryIntervalGetAddr = NATMAP_INIT_RETRY;
		}

	*n = traversal;		// Append new NATTraversalInfo to the end of our list

	if (traversal->protocol != 0 && !mDNSIPPortIsZero(traversal->privatePort))
		{
		traversal->opFlags |= ((traversal->protocol & kDNSServiceProtocol_UDP) ? MapUDPFlag : MapTCPFlag);
		traversal->retryIntervalPortMap = NATMAP_INIT_RETRY;
		traversal->retryPortMap = m->timenow;
		}

	m->NextScheduledNATOp = m->timenow;	// this will always trigger sending the packet asap

	return(mStatus_NoError);
	}

// Must be called with the mDNS_Lock held
mDNSexport mStatus mDNS_StopNATOperation_internal(mDNS *m, NATTraversalInfo *traversal)
	{
	NATTraversalInfo **ptr = &m->NATTraversals;
	
	while (*ptr && *ptr != traversal) ptr=&(*ptr)->next;
	if (*ptr) *ptr = (*ptr)->next;		// If we found it, cut this NATTraversalInfo struct from our list
	else
		{
		LogMsg("mDNS_StopNATOperation: NATTraversalInfo %p not found in list", traversal);
		return(mStatus_BadReferenceErr);
		}

	LogOperation("mDNS_StopNATOperation_internal %d %d %d %d",
		traversal->protocol, mDNSVal16(traversal->privatePort), mDNSVal16(traversal->publicPortreq), traversal->portMappingLease);

	if (m->CurrentNATTraversal == traversal)
		m->CurrentNATTraversal = m->CurrentNATTraversal->next;

	if (!(traversal->opFlags & LegacyFlag))
		{
		// let other edge-case states expire for simplicity
		if (!mDNSIPPortIsZero(traversal->publicPort))
			{
			// zero lease
			traversal->portMappingLease = 0;
			traversal->retryIntervalPortMap = 0;
			// send once only - if it fails the router will clean itself up eventually
			uDNS_SendNATMsg(m, traversal, (traversal->opFlags & MapTCPFlag) ? MapTCPFlag : MapUDPFlag);
			}
		}
#ifdef _LEGACY_NAT_TRAVERSAL_
	else
		{
		mStatus err = mStatus_NoError;
		mDNSBool tcp = (traversal->opFlags & MapTCPFlag) ? 1 : 0;
		err = LNT_UnmapPort(m, traversal, tcp);
		if (err) LogMsg("Legacy NAT Traversal - unmap request failed with error %ld", err);
		}
#endif // _LEGACY_NAT_TRAVERSAL_
	return(mStatus_NoError);
	}

mDNSexport mStatus mDNS_StartNATOperation(mDNS *m, NATTraversalInfo *traversal)
	{
	mStatus status;
	mDNS_Lock(m);
	status = mDNS_StartNATOperation_internal(m, traversal);
	mDNS_Unlock(m);
	return(status);
	}

mDNSexport mStatus mDNS_StopNATOperation(mDNS *m, NATTraversalInfo *traversal)
	{
	mStatus status;
	mDNS_Lock(m);
	status = mDNS_StopNATOperation_internal(m, traversal);
	mDNS_Unlock(m);
	return(status);
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - Long-Lived Queries
#endif

mDNSlocal void StartLLQPolling(mDNS *const m, DNSQuestion *q)
	{
	LogOperation("StartLLQPolling: %##s", q->qname.c);
	q->state = LLQ_Poll;
	q->ThisQInterval = LLQ_POLL_INTERVAL;
	q->LastQTime     = m->timenow - q->ThisQInterval;	// trigger immediate poll
	SetNextQueryTime(m, q);
	}

// Forward reference
mDNSlocal void LLQNatMapComplete(mDNS *const m, mDNSv4Addr ExternalAddress, NATTraversalInfo *n, mStatus err);

mDNSlocal void StartLLQNatMap(mDNS *m, DNSQuestion *q)
	{
	LogOperation("StartLLQNatMap: LLQ_NatMapWaitUDP");
	mDNSPlatformMemZero(&q->NATInfoUDP, sizeof(NATTraversalInfo));
	q->NATInfoUDP.privatePort = q->NATInfoUDP.publicPortreq = m->UnicastPort4;
	q->NATInfoUDP.protocol = kDNSServiceProtocol_UDP;
	q->NATInfoUDP.clientCallback = LLQNatMapComplete;
	q->NATInfoUDP.clientContext = q;	// Must be set non-null so we know this NATTraversalInfo object is in use
	mDNS_StartNATOperation_internal(m, &q->NATInfoUDP);
	}

mDNSlocal mDNSu8 *putLLQ(DNSMessage *const msg, mDNSu8 *ptr, const DNSQuestion *const question, const LLQOptData *const data, mDNSBool includeQuestion)
	{
	AuthRecord rr;
	ResourceRecord *opt = &rr.resrec;
	rdataOPT *optRD;

	//!!!KRS when we implement multiple llqs per message, we'll need to memmove anything past the question section
	if (includeQuestion)
		{
		ptr = putQuestion(msg, ptr, msg->data + AbsoluteMaxDNSMessageData, &question->qname, question->qtype, question->qclass);
		if (!ptr) { LogMsg("ERROR: putLLQ - putQuestion"); return mDNSNULL; }
		}
	// locate OptRR if it exists, set pointer to end
	// !!!KRS implement me

	// format opt rr (fields not specified are zero-valued)
	mDNS_SetupResourceRecord(&rr, mDNSNULL, mDNSInterface_Any, kDNSType_OPT, kStandardTTL, kDNSRecordTypeKnownUnique, mDNSNULL, mDNSNULL);
	opt->rdlength   = LLQ_OPT_RDLEN;
	opt->rdestimate = LLQ_OPT_RDLEN;

	optRD = &rr.resrec.rdata->u.opt;
	optRD->opt = kDNSOpt_LLQ;
	optRD->optlen = LLQ_OPTLEN;
	optRD->OptData.llq = *data;
	ptr = PutResourceRecordTTLJumbo(msg, ptr, &msg->h.numAdditionals, opt, 0);
	if (!ptr) { LogMsg("ERROR: putLLQ - PutResourceRecordTTLJumbo"); return mDNSNULL; }

	return ptr;
	}

mDNSlocal mStatus constructQueryMsg(DNSMessage *msg, mDNSu8 **endPtr, DNSQuestion *const question)
	{
	InitializeDNSMessage(&msg->h, question->TargetQID, uQueryFlags);

	*endPtr = putQuestion(msg, msg->data, msg->data + AbsoluteMaxDNSMessageData, &question->qname, question->qtype, question->qclass);
	if (!*endPtr)
	    {
	    LogMsg("ERROR: Unicast query out of space in packet");
	    return mStatus_UnknownErr;
	    }
	return mStatus_NoError;
	}

// Normally called with llq set.
// May be called with llq NULL, when retransmitting a lost Challenge Response
mDNSlocal void sendChallengeResponse(mDNS *const m, DNSQuestion *const q, const LLQOptData *llq)
	{
	mDNSu8 *responsePtr = m->omsg.data;
	mStatus err;
	LLQOptData llqBuf;

	if (q->ntries++ == kLLQ_MAX_TRIES)
		{
		LogMsg("sendChallengeResponse: %d failed attempts for LLQ %##s Will re-try in %d minutes",
			   kLLQ_MAX_TRIES, q->qname.c, kLLQ_DEF_RETRY / 60);
		q->state         = LLQ_Retry;
		q->LastQTime     = m->timenow;
		q->ThisQInterval = (kLLQ_DEF_RETRY * mDNSPlatformOneSecond);
		// !!!KRS give a callback error in these cases?
		return;
		}

	if (!llq)		// Retransmission: need to make a new LLQOptData
		{
		llqBuf.vers     = kLLQ_Vers;
		llqBuf.llqOp    = kLLQOp_Setup;
		llqBuf.err      = LLQErr_NoError;
		llqBuf.id       = q->id;
		llqBuf.llqlease = q->origLease;
		llq = &llqBuf;
		}

	q->LastQTime     = m->timenow;
	q->ThisQInterval = q->tcp ? 0 : (kLLQ_INIT_RESEND * q->ntries * mDNSPlatformOneSecond);		// If using TCP, don't need to retransmit

	if (constructQueryMsg(&m->omsg, &responsePtr, q)) goto error;
	responsePtr = putLLQ(&m->omsg, responsePtr, q, llq, mDNSfalse);
	if (!responsePtr) { LogMsg("ERROR: sendChallengeResponse - putLLQ"); goto error; }

	//LogOperation("sendChallengeResponse %#a:%d %d %p %d", &q->servAddr, mDNSVal16(q->servPort), q->tcp, q->AuthInfo, responsePtr - (mDNSu8 *)&m->omsg);
	err = mDNSSendDNSMessage(m, &m->omsg, responsePtr, mDNSInterface_Any, &q->servAddr, q->servPort, q->tcp ? q->tcp->sock : mDNSNULL, q->AuthInfo);
	if (err) debugf("ERROR: sendChallengeResponse - mDNSSendDNSMessage returned %ld", err);
	// on error, we procede as normal and retry after the appropriate interval

	return;

	error:
	q->state = LLQ_Error;
	}

mDNSlocal void recvSetupResponse(mDNS *const m, mDNSu8 rcode, DNSQuestion *const q, const rdataOPT *opt)
	{
	mStatus err = mStatus_NoError;

	if (rcode && rcode != kDNSFlag1_RC_NXDomain)
		{ LogMsg("ERROR: recvSetupResponse %##s - rcode && rcode != kDNSFlag1_RC_NXDomain", q->qname.c); goto fail; }

	if (opt->OptData.llq.llqOp != kLLQOp_Setup)
		{ LogMsg("ERROR: recvSetupResponse %##s - bad op %d", q->qname.c, opt->OptData.llq.llqOp); goto fail; }

	if (opt->OptData.llq.vers != kLLQ_Vers)
		{ LogMsg("ERROR: recvSetupResponse %##s - bad vers %d", q->qname.c, opt->OptData.llq.vers); goto fail; }

	if (q->state == LLQ_InitialRequest)
		{
		const LLQOptData *const llq = &opt->OptData.llq;
		//LogOperation("Got LLQ_InitialRequest");

		switch(llq->err)
			{
			case LLQErr_NoError: break;
			case LLQErr_ServFull:
				LogMsg("recvSetupResponse - received ServFull from server for LLQ %##s Retry in %lu sec", q->qname.c, llq->llqlease);
				q->LastQTime     = m->timenow;
				q->ThisQInterval = ((mDNSs32)llq->llqlease * mDNSPlatformOneSecond);
				q->state = LLQ_Retry;
			case LLQErr_Static:
				q->state = LLQ_Static;
				q->ThisQInterval = 0;
				LogMsg("LLQ %##s: static", q->qname.c);
				goto exit;
			case LLQErr_FormErr:
				LogMsg("ERROR: recvSetupResponse - received FormErr from server for LLQ %##s", q->qname.c);
				goto error;
			case LLQErr_BadVers:
				LogMsg("ERROR: recvSetupResponse - received BadVers from server");
				goto error;
			case LLQErr_UnknownErr:
				LogMsg("ERROR: recvSetupResponse - received UnknownErr from server for LLQ %##s", q->qname.c);
				goto error;
			default:
				LogMsg("ERROR: recvSetupResponse - received invalid error %d for LLQ %##s", llq->err, q->qname.c);
				goto error;
			}
	
		if (q->origLease != llq->llqlease)
			debugf("recvSetupResponse: requested lease %lu, granted lease %lu", q->origLease, llq->llqlease);
	
		// cache expiration in case we go to sleep before finishing setup
		q->origLease = llq->llqlease;
		q->expire = m->timenow + ((mDNSs32)llq->llqlease * mDNSPlatformOneSecond);
	
		// update state
		q->state  = LLQ_SecondaryRequest;
		q->id     = llq->id;
		// if (q->ntries == 1) goto exit;	// Test for simulating loss of challenge response packet
		q->ntries = 0; // first attempt to send response
	
		sendChallengeResponse(m, q, llq);
		goto exit;
	
		error:
		q->state = LLQ_Error;
		goto exit;
		}

	if (q->state == LLQ_SecondaryRequest)
		{
		//LogOperation("Got LLQ_SecondaryRequest");

		// Fix this immediately if not sooner.  Copy the id from the LLQOptData into our DNSQuestion struct.  This is only
		// an issue for private LLQs, because we skip parts 2 and 3 of the handshake.  This is related to a bigger
		// problem of the current implementation of TCP LLQ setup: we're not handling state transitions correctly
		// if the server sends back SERVFULL or STATIC.
		if (q->AuthInfo)
			{
			LogOperation("Private LLQ_SecondaryRequest; copying id %08X %08X", opt->OptData.llq.id.l[0], opt->OptData.llq.id.l[1]);
			q->id = opt->OptData.llq.id;
			}

		if (opt->OptData.llq.err) { LogMsg("ERROR: recvSetupResponse %##s code %d from server", q->qname.c, opt->OptData.llq.err); q->state = LLQ_Error; goto fail; }
		if (!mDNSSameOpaque64(&q->id, &opt->OptData.llq.id))
			{ LogMsg("recvSetupResponse - ID changed.  discarding"); goto exit; } // this can happen rarely (on packet loss + reordering)
		q->expire        = m->timenow + ((mDNSs32)opt->OptData.llq.llqlease *  mDNSPlatformOneSecond    );
		q->LastQTime     = m->timenow;
		q->ThisQInterval =              ((mDNSs32)opt->OptData.llq.llqlease * (mDNSPlatformOneSecond / 2));
		q->origLease     = opt->OptData.llq.llqlease;
		q->state         = LLQ_Established;
		goto exit;
		}

	if (q->state == LLQ_Established) goto exit;

	LogMsg("ERROR: recvSetupResponse %##s - bad state %d", q->qname.c, q->state);

fail:
	err = mStatus_UnknownErr;

exit:

	if (err)
		{
		LogOperation("recvSetupResponse error %d ", err);
		StartLLQPolling(m, q);
		}
	}

// Returns mDNStrue if mDNSCoreReceiveResponse should treat this packet as a series of add/remove instructions (like an mDNS response)
// Returns mDNSfalse if mDNSCoreReceiveResponse should treat this as a single authoritative result (like a normal unicast DNS response)
mDNSexport uDNS_LLQType uDNS_recvLLQResponse(mDNS *const m, const DNSMessage *const msg, const mDNSu8 *const end, const mDNSAddr *const srcaddr, const mDNSIPPort srcport)
	{
	DNSQuestion pktQ, *q;
	if (msg->h.numQuestions && getQuestion(msg, msg->data, end, 0, &pktQ))
		{
		const rdataOPT *opt = GetLLQOptData(m, msg, end);
		if (opt)
			{
			for (q = m->Questions; q; q = q->next)
				{
				if (q->LongLived && q->qtype == pktQ.qtype && q->qnamehash == pktQ.qnamehash && SameDomainName(&q->qname, &pktQ.qname))
					{
					debugf("uDNS_recvLLQResponse found %##s (%s) %d %#a %#a %X %X %X %X %d",
						q->qname.c, DNSTypeName(q->qtype), q->state, srcaddr, &q->servAddr,
						opt->OptData.llq.id.l[0], opt->OptData.llq.id.l[1], q->id.l[0], q->id.l[1], opt->OptData.llq.llqOp);
					if (q->state == LLQ_Poll)
						{
						m->rec.r.resrec.RecordType = 0;		// Clear RecordType to show we're not still using it
						return uDNS_LLQ_Poll;
						}
					else if (q->state == LLQ_Established || (q->state == LLQ_Refresh && msg->h.numAnswers))
						{
						if (opt->OptData.llq.llqOp == kLLQOp_Event && mDNSSameOpaque64(&opt->OptData.llq.id, &q->id))
							{
							mDNSu8 *ackEnd;
							if (q->LongLived && q->state == LLQ_Poll && !mDNSIPPortIsZero(q->servPort)) q->ThisQInterval = LLQ_POLL_INTERVAL;
							else if (q->ThisQInterval < MAX_UCAST_POLL_INTERVAL)                        q->ThisQInterval = MAX_UCAST_POLL_INTERVAL;
							InitializeDNSMessage(&m->omsg.h, msg->h.id, ResponseFlags);
							ackEnd = putLLQ(&m->omsg, m->omsg.data, mDNSNULL, &opt->OptData.llq, mDNSfalse);
							if (ackEnd) mDNSSendDNSMessage(m, &m->omsg, ackEnd, mDNSInterface_Any, srcaddr, srcport, mDNSNULL, mDNSNULL);
							m->rec.r.resrec.RecordType = 0;		// Clear RecordType to show we're not still using it
							return uDNS_LLQ_Events;
							}
						}
					if (mDNSSameOpaque16(msg->h.id, q->TargetQID))
						{
						if (opt->OptData.llq.llqOp == kLLQOp_Refresh && q->state == LLQ_Refresh && msg->h.numAdditionals && !msg->h.numAnswers && mDNSSameOpaque64(&opt->OptData.llq.id, &q->id))
							{
							if (opt->OptData.llq.err != LLQErr_NoError) LogMsg("recvRefreshReply: received error %d from server", opt->OptData.llq.err);
							else
								{
								q->expire   = q->LastQTime + ((mDNSs32)opt->OptData.llq.llqlease *  mDNSPlatformOneSecond   );
								q->ThisQInterval =           ((mDNSs32)opt->OptData.llq.llqlease * (mDNSPlatformOneSecond/2));
								q->origLease = opt->OptData.llq.llqlease;
								q->state = LLQ_Established;
								}
							m->rec.r.resrec.RecordType = 0;		// Clear RecordType to show we're not still using it
							return uDNS_LLQ_Events;
							}
						if (q->state < LLQ_Static)
							{
							if (mDNSSameAddress(srcaddr, &q->servAddr))
								{
								LLQ_State oldstate = q->state;
								recvSetupResponse(m, msg->h.flags.b[1] & kDNSFlag1_RC, q, opt);
								m->rec.r.resrec.RecordType = 0;		// Clear RecordType to show we're not still using it
								//DumpPacket(m, msg, end);
								// If this is our Ack+Answers packet resulting from a correct challenge response, then it's a full list
								// of answers, and any cache answers we have that are not included in this packet need to be flushed
								//LogMsg("uDNS_recvLLQResponse: recvSetupResponse state %d returning %d", oldstate, oldstate != LLQ_SecondaryRequest);
								return (oldstate == LLQ_SecondaryRequest ? uDNS_LLQ_Setup : uDNS_LLQ_Events);
								}
							}
						}
					}
				}
			m->rec.r.resrec.RecordType = 0;		// Clear RecordType to show we're not still using it
			//LogMsg("uDNS_recvLLQResponse: returning 0");
			}
		}
	return uDNS_LLQ_Not;
	}

// tcpCallback is called to handle events (e.g. connection opening and data reception) on TCP connections for
// Private DNS operations -- private queries, private LLQs, private record updates and private service updates
mDNSlocal void tcpCallback(TCPSocket *sock, void *context, mDNSBool ConnectionEstablished, mStatus err)
	{
	tcpInfo_t		*	tcpInfo = (tcpInfo_t *)context;
	mDNSBool			closed = mDNSfalse;
	mDNSu8			*	end;
	mDNS			*	m = tcpInfo->m;
	DomainAuthInfo *AuthInfo =
		tcpInfo->question ? tcpInfo->question->AuthInfo :
		tcpInfo->srs      ? GetAuthInfoForName(m, tcpInfo->srs->RR_SRV.resrec.name)  :
		tcpInfo->rr       ? GetAuthInfoForName(m, tcpInfo->rr->resrec.name) : mDNSNULL;

	tcpInfo_t **backpointer =
		tcpInfo->question ? &tcpInfo->question->tcp :
		tcpInfo->srs      ? &tcpInfo->srs->tcp :
		tcpInfo->rr       ? &tcpInfo->rr->tcp : mDNSNULL;
	if (!backpointer) LogMsg("tcpCallback: Purpose of connection unidentified");
	else if (*backpointer != tcpInfo)
		LogMsg("tcpCallback: %d backpointer %p incorrect tcpInfo %p question %p srs %p rr %p",
			mDNSPlatformTCPGetFD(tcpInfo->sock), *backpointer, tcpInfo, tcpInfo->question, tcpInfo->srs, tcpInfo->rr);

	if (err) goto exit;

	if (ConnectionEstablished)
		{
		// connection is established - send the message
		if (tcpInfo->question && tcpInfo->question->LongLived)
			{
			LLQOptData	llqData;			// set llq rdata
			llqData.vers  = kLLQ_Vers;
			llqData.llqOp = kLLQOp_Setup;
			llqData.err   = LLQErr_NoError;
			llqData.err   = mDNSVal16(tcpInfo->question->eventPort);		// Tell server our external NAT-mapped UDP port
			if (llqData.err == 0) llqData.err = 5353;	// If not using NAT, then we need events sent directly to 5353
			LogOperation("tcpCallback: eventPort %d", llqData.err);
			llqData.id    = zeroOpaque64;
			llqData.llqlease = kLLQ_DefLease;
			InitializeDNSMessage(&tcpInfo->request.h, tcpInfo->question->TargetQID, uQueryFlags);
			//LogMsg("tcpCallback: putLLQ %p", AuthInfo);
			end = putLLQ(&tcpInfo->request, tcpInfo->request.data, tcpInfo->question, &llqData, mDNStrue);

			if (!end) { LogMsg("ERROR: tcpCallback - putLLQ"); err = mStatus_UnknownErr; goto exit; }
			}
		else if (tcpInfo->question)
			{
			err = constructQueryMsg(&tcpInfo->request, &end, tcpInfo->question);
			if (err) { LogMsg("ERROR: tcpCallback: constructQueryMsg - %ld", err); err = mStatus_UnknownErr; goto exit; }
			}
		else
			end = ((mDNSu8*) &tcpInfo->request) + tcpInfo->requestLen;

		err = mDNSSendDNSMessage(m, &tcpInfo->request, end, mDNSInterface_Any, &zeroAddr, zeroIPPort, sock, AuthInfo);

		if (err) { debugf("ERROR: tcpCallback: mDNSSendDNSMessage - %ld", err); err = mStatus_UnknownErr; goto exit; }

		// Record time we sent this question
		if (tcpInfo->question)
			{
			mDNS_Lock(m);
			tcpInfo->question->LastQTime = m->timenow;
			tcpInfo->question->ThisQInterval = MAX_UCAST_POLL_INTERVAL;
			mDNS_Unlock(m);
			}
		}
	else
		{
		long n;
		if (tcpInfo->nread < 2)			// First read the two-byte length preceeding the DNS message
			{
			mDNSu8 *lenptr = (mDNSu8 *)&tcpInfo->replylen;
			n = mDNSPlatformReadTCP(sock, lenptr + tcpInfo->nread, 2 - tcpInfo->nread, &closed);
			if (n < 0) { LogMsg("ERROR:tcpCallback - attempt to read message length failed (%d)", n); err = mStatus_ConnFailed; goto exit; }
			else if (closed)
				{
				// It's perfectly fine for this socket to close after the first reply. The server might
				// be sending gratuitous replies using UDP and doesn't have a need to leave the TCP socket open.
				// We'll only log this event if we've never received a reply before.
				// BIND 9 appears to close an idle connection after 30 seconds
				if (tcpInfo->numReplies == 0) LogMsg("ERROR: socket closed prematurely %d", tcpInfo->nread);
				err = mStatus_ConnFailed;
				goto exit;
				}

			tcpInfo->nread += n;
			if (tcpInfo->nread < 2) goto exit;

			tcpInfo->replylen = (mDNSu16)((mDNSu16)lenptr[0] << 8 | lenptr[1]);
			if (tcpInfo->replylen < sizeof(DNSMessageHeader))
				{ LogMsg("ERROR: tcpCallback - length too short (%d bytes)", tcpInfo->replylen); err = mStatus_UnknownErr; goto exit; }

			tcpInfo->reply = mDNSPlatformMemAllocate(tcpInfo->replylen);
			if (!tcpInfo->reply) { LogMsg("ERROR: tcpCallback - malloc failed"); err = mStatus_NoMemoryErr; goto exit; }
			}

		n = mDNSPlatformReadTCP(sock, ((char *)tcpInfo->reply) + (tcpInfo->nread - 2), tcpInfo->replylen - (tcpInfo->nread - 2), &closed);

		if      (n < 0)  { LogMsg("ERROR: tcpCallback - read returned %d", n);            err = mStatus_ConnFailed; goto exit; }
		else if (closed) { LogMsg("ERROR: socket closed prematurely %d", tcpInfo->nread); err = mStatus_ConnFailed; goto exit; }

		tcpInfo->nread += n;

		if ((tcpInfo->nread - 2) == tcpInfo->replylen)
			{
			DNSMessage *reply = tcpInfo->reply;
			mDNSu8     *end   = (mDNSu8 *)tcpInfo->reply + tcpInfo->replylen;
			mDNSAddr    Addr  = tcpInfo->Addr;
			mDNSIPPort  Port  = tcpInfo->Port;
			tcpInfo->numReplies++;
			tcpInfo->reply    = mDNSNULL;	// Detach reply buffer from tcpInfo_t, to make sure client callback can't cause it to be disposed
			tcpInfo->nread    = 0;
			tcpInfo->replylen = 0;
			
			// If we're going to dispose this connection, do it FIRST, before calling client callback
			if (!tcpInfo->question || !tcpInfo->question->LongLived) { *backpointer = mDNSNULL; DisposeTCPConn(tcpInfo); }
			
			mDNSCoreReceive(m, reply, end, &Addr, Port, mDNSNULL, zeroIPPort, 0);
			// USE CAUTION HERE: Invoking mDNSCoreReceive may have caused the environment to change, including canceling this operation itself
			
			if (mDNS_LogLevel >= MDNS_LOG_VERBOSE_DEBUG) DumpPacket(m, reply, end);
			mDNSPlatformMemFree(reply);
			return;
			}
		}

exit:

	if (err)
		{
		// Clear client backpointer FIRST -- that way if one of the callbacks cancels its operation
		// we won't end up double-disposing our tcpInfo_t
		*backpointer = mDNSNULL;

		if (tcpInfo->question)
			{
			tcpInfo->question->LastQTime = m->timenow;
			tcpInfo->question->ThisQInterval = MAX_UCAST_POLL_INTERVAL;
			// ConnFailed is actually okay.  It just means that the server closed the connection but the LLQ is still okay.
			// If the error isn't ConnFailed, then the LLQ is in bad shape.
			if (err != mStatus_ConnFailed) tcpInfo->question->state = LLQ_Error;
			}

		if (tcpInfo->rr)
			{
			mDNSBool deregPending = (tcpInfo->rr->state == regState_DeregPending) ? mDNStrue : mDNSfalse;

			UnlinkAuthRecord(m, tcpInfo->rr);
			tcpInfo->rr->state = regState_Unregistered;

			if (!deregPending)
				{
				// Right now tcpCallback does not run holding the lock, so no need to drop the lock
				//mDNS_DropLockBeforeCallback();
				if (tcpInfo->rr->RecordCallback)
					tcpInfo->rr->RecordCallback(m, tcpInfo->rr, err);
				//mDNS_ReclaimLockAfterCallback();
				// NOTE: not safe to touch any client structures here --
				// once we issue the callback, client is free to reuse or deallocate the srs memory
				}
			}

		if (tcpInfo->srs)
			{
			mDNSBool deregPending = (tcpInfo->srs->state == regState_DeregPending) ? mDNStrue : mDNSfalse;

			unlinkSRS(m, tcpInfo->srs);
			tcpInfo->srs->state = regState_Unregistered;

			if (!deregPending)
				{
				// Right now tcpCallback does not run holding the lock, so no need to drop the lock
				//mDNS_DropLockBeforeCallback();
				tcpInfo->srs->ServiceCallback(m, tcpInfo->srs, err);
				//mDNS_ReclaimLockAfterCallback();
				// NOTE: not safe to touch any client structures here --
				// once we issue the callback, client is free to reuse or deallocate the srs memory
				}
			}

		DisposeTCPConn(tcpInfo);
		}
	}

mDNSlocal tcpInfo_t *MakeTCPConn(mDNS *const m, const DNSMessage *const msg, const mDNSu8 *const end,
	TCPSocketFlags flags, const mDNSAddr *const Addr, const mDNSIPPort Port,
	DNSQuestion *const question, ServiceRecordSet *const srs, AuthRecord *const rr)
	{
	mStatus err;
	mDNSIPPort srcport = zeroIPPort;
	tcpInfo_t *info = (tcpInfo_t *)mDNSPlatformMemAllocate(sizeof(tcpInfo_t));
	if (!info) { LogMsg("ERROR: MakeTCP - memallocate failed"); return(mDNSNULL); }

	mDNSPlatformMemZero(info, sizeof(tcpInfo_t));
	info->m          = m;
	if (msg)
		{
		info->request    = *msg;
		info->requestLen = (int) (end - ((mDNSu8*)msg));
		}
	info->question   = question;
	info->srs        = srs;
	info->rr         = rr;
	info->Addr       = *Addr;
	info->Port       = Port;

	info->sock = mDNSPlatformTCPSocket(m, flags, &srcport);
	if (!info->sock) { LogMsg("SendServiceRegistration: uanble to create TCP socket"); mDNSPlatformMemFree(info); return(mDNSNULL); }
	err = mDNSPlatformTCPConnect(info->sock, Addr, Port, 0, tcpCallback, info);

	if      (err == mStatus_ConnEstablished) { tcpCallback(info->sock, info, mDNStrue, mStatus_NoError); }
	else if (err != mStatus_ConnPending    ) { LogMsg("MakeTCPConnection: connection failed"); mDNSPlatformMemFree(info); return(mDNSNULL); }
	return(info);
	}

mDNSexport void DisposeTCPConn(struct tcpInfo_t *tcp)
	{
	mDNSPlatformTCPCloseConnection(tcp->sock);
	if (tcp->reply) mDNSPlatformMemFree(tcp->reply);
	mDNSPlatformMemFree(tcp);
	}

mDNSlocal void RemoveLLQNatMappings(mDNS *m, DNSQuestion *q)
	{
	if (q->NATInfoUDP.clientContext)
		{
		mDNS_StopNATOperation_internal(m, &q->NATInfoUDP);
		q->NATInfoUDP.clientContext = mDNSNULL;
		}
	}

mDNSlocal void startLLQHandshake(mDNS *m, DNSQuestion *q)
	{
	mStatus err = mStatus_NoError;
	if (q->AuthInfo)
		{
		LogOperation("startLLQHandshakePrivate Addr %#a%s Server %#a:%d%s %##s (%s) eventport %d",
			&m->AdvertisedV4,                     mDNSv4AddrIsRFC1918(&m->AdvertisedV4.ip.v4) ? " (RFC 1918)" : "",
			&q->servAddr, mDNSVal16(q->servPort), mDNSAddrIsRFC1918(&q->servAddr)             ? " (RFC 1918)" : "",
			q->qname.c, DNSTypeName(q->qtype), mDNSVal16(q->eventPort));

		if (mDNSv4AddrIsRFC1918(&m->AdvertisedV4.ip.v4) && !mDNSAddrIsRFC1918(&q->servAddr))
			if (q->state == LLQ_InitialRequest || mDNSIPPortIsZero(q->NATInfoUDP.publicPort))
				{
				// start
				if (!q->NATInfoUDP.clientContext) { q->state = LLQ_NatMapWaitUDP; StartLLQNatMap(m, q); goto exit; }
				// port mapping in process
				else if ((q->NATInfoUDP.opFlags & MapUDPFlag) && mDNSIPPortIsZero(q->NATInfoUDP.publicPort)) { q->state = LLQ_NatMapWaitUDP; goto exit; }
				else                                                                                         { err = mStatus_UnknownErr;     goto exit; }
				}

		LogOperation("startLLQHandshake TCP %p", q->tcp);
		if (q->tcp) LogMsg("startLLQHandshake: Already have TCP connection for %##s (%s)", q->qname.c, DNSTypeName(q->qtype));
		else q->tcp = MakeTCPConn(m, mDNSNULL, mDNSNULL, kTCPSocketFlags_UseTLS, &q->servAddr, q->servPort, q, mDNSNULL, mDNSNULL);

		// update question state
		//q->state         = LLQ_InitialRequest;
		q->state         = LLQ_SecondaryRequest;		// Right now, for private DNS, we skip the four-way LLQ handshake
		q->origLease     = kLLQ_DefLease;
		q->ThisQInterval = 0;
		q->LastQTime     = m->timenow;

		err = mStatus_NoError;
		}
	else
		{
		mDNSu8 *end;
		LLQOptData llqData;

		LogOperation("startLLQHandshake Addr %#a%s Server %#a:%d%s %##s (%s) publicPort %d",
			&m->AdvertisedV4,                     mDNSv4AddrIsRFC1918(&m->AdvertisedV4.ip.v4) ? " (RFC 1918)" : "",
			&q->servAddr, mDNSVal16(q->servPort), mDNSAddrIsRFC1918(&q->servAddr)             ? " (RFC 1918)" : "",
			q->qname.c, DNSTypeName(q->qtype), mDNSVal16(q->NATInfoUDP.publicPort));

		if (mDNSv4AddrIsRFC1918(&m->AdvertisedV4.ip.v4) && !mDNSAddrIsRFC1918(&q->servAddr) && mDNSIPPortIsZero(q->NATInfoUDP.publicPort))
			{
			// start
			if (!q->NATInfoUDP.clientContext) { q->state = LLQ_NatMapWaitUDP; StartLLQNatMap(m, q); }
			// port mapping in process
			else if ((q->NATInfoUDP.opFlags & MapUDPFlag) && mDNSIPPortIsZero(q->NATInfoUDP.publicPort)) { q->state = LLQ_NatMapWaitUDP; goto exit; }
			else                                                                                         { err = mStatus_UnknownErr;     goto exit; }
			}

		if (q->ntries++ >= kLLQ_MAX_TRIES)
			{ LogMsg("startLLQHandshake: %d failed attempts for LLQ %##s Polling.", kLLQ_MAX_TRIES, q->qname.c); err = mStatus_UnknownErr; goto exit; }

		// set llq rdata
		llqData.vers  = kLLQ_Vers;
		llqData.llqOp = kLLQOp_Setup;
		llqData.err   = LLQErr_NoError;
		llqData.id    = zeroOpaque64;
		llqData.llqlease = kLLQ_DefLease;

		InitializeDNSMessage(&m->omsg.h, q->TargetQID, uQueryFlags);
		end = putLLQ(&m->omsg, m->omsg.data, q, &llqData, mDNStrue);
		if (!end) { LogMsg("ERROR: startLLQHandshake - putLLQ"); q->state = LLQ_Error; return; }

		err = mDNSSendDNSMessage(m, &m->omsg, end, mDNSInterface_Any, &q->servAddr, q->servPort, mDNSNULL, mDNSNULL);
		// on error, we procede as normal and retry after the appropriate interval
		if (err) { debugf("ERROR: startLLQHandshake - mDNSSendDNSMessage returned %ld", err); err = mStatus_NoError; }

		// update question state
		q->state         = LLQ_InitialRequest;
		q->origLease     = kLLQ_DefLease;
		q->ThisQInterval = (kLLQ_INIT_RESEND * mDNSPlatformOneSecond);
		q->LastQTime     = m->timenow - q->ThisQInterval;

		err = mStatus_NoError;
		}

exit:
	if (err)
		{
		LogOperation("startLLQHandshake error %d ", err);
		StartLLQPolling(m, q);
		}
	}

// Called in normal client context (lock not held)
mDNSlocal void LLQNatMapComplete(mDNS *const m, mDNSv4Addr ExternalAddress, NATTraversalInfo *n, mStatus err)
	{
	(void)ExternalAddress;
	(void)n;
	
	if (m->CurrentQuestion)
		LogMsg("LLQNatMapComplete: ERROR m->CurrentQuestion already set: %##s (%s)", m->CurrentQuestion->qname.c, DNSTypeName(m->CurrentQuestion->qtype));
	m->CurrentQuestion = m->Questions;

	while (m->CurrentQuestion)
		{
		DNSQuestion *q = m->CurrentQuestion;
		m->CurrentQuestion = m->CurrentQuestion->next;
		if (q->LongLived)
			{
			if (q->state == LLQ_NatMapWaitUDP)
				{
				if (!err && !mDNSIPPortIsZero(q->NATInfoUDP.publicPort))
					{
					q->state = LLQ_GetZoneInfo;
					q->eventPort = q->NATInfoUDP.publicPort;
					startLLQHandshake(m, q);
					}
				else
					{
					LogMsg("LLQNatMapComplete error %d publicPort %d", err, mDNSVal16(q->NATInfoUDP.publicPort));
					RemoveLLQNatMappings(m, q);
					StartLLQPolling(m, q);
					}
				}
			}
		}
	m->CurrentQuestion = mDNSNULL;
	}

// if we ever want to refine support for multiple hostnames, we can add logic matching service names to a particular hostname
// for now, we grab the first registered DynDNS name, if any, or a static name we learned via a reverse-map query
mDNSexport const domainname *GetServiceTarget(mDNS *m, ServiceRecordSet *srs)
	{
	LogOperation("GetServiceTarget %##s", srs->RR_SRV.resrec.name->c);

	if (!srs->RR_SRV.AutoTarget)		// If not automatically tracking this host's current name, just return the existing target
		return(&srs->RR_SRV.resrec.rdata->u.srv.target);
	else
		{
		HostnameInfo *hi = m->Hostnames;

#if APPLE_OSX_mDNSResponder
		DomainAuthInfo *AuthInfo = GetAuthInfoForName_internal(m, srs->RR_SRV.resrec.name);
		if (AuthInfo && AuthInfo->AutoTunnel)
			{
			if (AuthInfo->AutoTunnelHostRecord.namestorage.c[0] == 0)
				{
				if (m->AutoTunnelHostAddr.b[0]) SetupLocalAutoTunnelInterface_internal(m);
				return(mDNSNULL);
				}
			return(&AuthInfo->AutoTunnelHostRecord.namestorage);
			}
#endif APPLE_OSX_mDNSResponder

		while (hi)
			{
			if (hi->arv4.state == regState_Registered || hi->arv4.state == regState_Refresh) return(hi->arv4.resrec.name);
			if (hi->arv6.state == regState_Registered || hi->arv6.state == regState_Refresh) return(hi->arv6.resrec.name);
			hi = hi->next;
			}
		if (m->StaticHostname.c[0]) return(&m->StaticHostname);
		return(mDNSNULL);
		}
	}

// Called with lock held
mDNSlocal void SendServiceRegistration(mDNS *m, ServiceRecordSet *srs)
	{
	mDNSu8 *ptr = m->omsg.data;
	mDNSu8 *end = (mDNSu8 *)&m->omsg + sizeof(DNSMessage);
	mDNSOpaque16 id;
	mStatus err = mStatus_NoError;
	const domainname *target;
	mDNSu32 i;

	if (m->mDNS_busy != m->mDNS_reentrancy+1)
		LogMsg("SendServiceRegistration: Lock not held! mDNS_busy (%ld) mDNS_reentrancy (%ld)", m->mDNS_busy, m->mDNS_reentrancy);

	if (mDNSIPv4AddressIsZero(srs->ns.ip.v4)) { LogMsg("SendServiceRegistration - NS not set!"); return; }

	id = mDNS_NewMessageID(m);
	InitializeDNSMessage(&m->omsg.h, id, UpdateReqFlags);

	// setup resource records
	SetNewRData(&srs->RR_PTR.resrec, mDNSNULL, 0);		// Update rdlength, rdestimate, rdatahash
	SetNewRData(&srs->RR_TXT.resrec, mDNSNULL, 0);		// Update rdlength, rdestimate, rdatahash

	// replace port w/ NAT mapping if necessary
 	if (srs->RR_SRV.AutoTarget == Target_AutoHostAndNATMAP && !mDNSIPPortIsZero(srs->NATinfo.publicPort))
		srs->RR_SRV.resrec.rdata->u.srv.port = srs->NATinfo.publicPort;

	// construct update packet
	// set zone
	ptr = putZone(&m->omsg, ptr, end, &srs->zone, mDNSOpaque16fromIntVal(srs->RR_SRV.resrec.rrclass));
	if (!ptr) { err = mStatus_UnknownErr; goto exit; }

	if (srs->TestForSelfConflict)
		{
		// update w/ prereq that SRV already exist to make sure previous registration was ours, and delete any stale TXT records
		if (!(ptr = PutResourceRecordTTLJumbo(&m->omsg, ptr, &m->omsg.h.mDNS_numPrereqs, &srs->RR_SRV.resrec, 0))) { err = mStatus_UnknownErr; goto exit; }
		if (!(ptr = putDeleteRRSet(&m->omsg, ptr, srs->RR_TXT.resrec.name, srs->RR_TXT.resrec.rrtype)))       { err = mStatus_UnknownErr; goto exit; }
		}

	else if (srs->state != regState_Refresh && srs->state != regState_UpdatePending)
		{
		// use SRV name for prereq
		ptr = putPrereqNameNotInUse(srs->RR_SRV.resrec.name, &m->omsg, ptr, end);
		if (!ptr) { err = mStatus_UnknownErr; goto exit; }
		}

	//!!!KRS Need to do bounds checking and use TCP if it won't fit!!!
	if (!(ptr = PutResourceRecordTTLJumbo(&m->omsg, ptr, &m->omsg.h.mDNS_numUpdates, &srs->RR_PTR.resrec, srs->RR_PTR.resrec.rroriginalttl))) { err = mStatus_UnknownErr; goto exit; }

	for (i = 0; i < srs->NumSubTypes; i++)
		if (!(ptr = PutResourceRecordTTLJumbo(&m->omsg, ptr, &m->omsg.h.mDNS_numUpdates, &srs->SubTypes[i].resrec, srs->SubTypes[i].resrec.rroriginalttl))) { err = mStatus_UnknownErr; goto exit; }

	if (srs->state == regState_UpdatePending) // we're updating the txt record
		{
		AuthRecord *txt = &srs->RR_TXT;
		// delete old RData
		SetNewRData(&txt->resrec, txt->OrigRData, txt->OrigRDLen);
		if (!(ptr = putDeletionRecord(&m->omsg, ptr, &srs->RR_TXT.resrec))) { err = mStatus_UnknownErr; goto exit; }	// delete old rdata

		// add new RData
		SetNewRData(&txt->resrec, txt->InFlightRData, txt->InFlightRDLen);
		if (!(ptr = PutResourceRecordTTLJumbo(&m->omsg, ptr, &m->omsg.h.mDNS_numUpdates, &srs->RR_TXT.resrec, srs->RR_TXT.resrec.rroriginalttl))) { err = mStatus_UnknownErr; goto exit; }
		}
	else
		if (!(ptr = PutResourceRecordTTLJumbo(&m->omsg, ptr, &m->omsg.h.mDNS_numUpdates, &srs->RR_TXT.resrec, srs->RR_TXT.resrec.rroriginalttl))) { err = mStatus_UnknownErr; goto exit; }

	target = GetServiceTarget(m, srs);
	if (!target)
		{
		debugf("SendServiceRegistration - no target for %##s", srs->RR_SRV.resrec.name->c);
		srs->state = regState_NoTarget;
		return;
		}

	if (!SameDomainName(target, &srs->RR_SRV.resrec.rdata->u.srv.target))
		{
		AssignDomainName(&srs->RR_SRV.resrec.rdata->u.srv.target, target);
		SetNewRData(&srs->RR_SRV.resrec, mDNSNULL, 0);		// Update rdlength, rdestimate, rdatahash
		}

	ptr = PutResourceRecordTTLJumbo(&m->omsg, ptr, &m->omsg.h.mDNS_numUpdates, &srs->RR_SRV.resrec, srs->RR_SRV.resrec.rroriginalttl);
	if (!ptr) { err = mStatus_UnknownErr; goto exit; }

	if (srs->srs_uselease)
		{ ptr = putUpdateLease(&m->omsg, ptr, DEFAULT_UPDATE_LEASE); if (!ptr) { err = mStatus_UnknownErr; goto exit; } }

	if (srs->state != regState_Refresh && srs->state != regState_DeregDeferred && srs->state != regState_UpdatePending)
		srs->state = regState_Pending;

	srs->id = id;

	if (srs->Private)
		{
		LogOperation("SendServiceRegistration TCP %p", srs->tcp);
		if (srs->tcp) LogMsg("SendServiceRegistration: Already have TCP connection for %s", ARDisplayString(m, &srs->RR_SRV));
		else srs->tcp = MakeTCPConn(m, &m->omsg, ptr, kTCPSocketFlags_UseTLS, &srs->ns, srs->SRSUpdatePort, mDNSNULL, srs, mDNSNULL);
		srs->RR_SRV.LastAPTime = m->timenow;
		srs->RR_SRV.ThisAPInterval = 0x3FFFFFFF; // TCP will handle any necessary retransmissions for us
		}
	else
		{
		err = mDNSSendDNSMessage(m, &m->omsg, ptr, mDNSInterface_Any, &srs->ns, srs->SRSUpdatePort, mDNSNULL, GetAuthInfoForName_internal(m, srs->RR_SRV.resrec.name));
		if (err) debugf("ERROR: SendServiceRegistration - mDNSSendDNSMessage - %ld", err);
		SetRecordRetry(m, &srs->RR_SRV, err);
		}

	err = mStatus_NoError;

exit:

	if (err)
		{
		LogMsg("SendServiceRegistration - Error formatting message %d", err);

		unlinkSRS(m, srs);
		srs->state = regState_Unregistered;

		mDNS_DropLockBeforeCallback();
		srs->ServiceCallback(m, srs, err);
		mDNS_ReclaimLockAfterCallback();
		// CAUTION: MUST NOT do anything more with rr after calling rr->Callback(), because the client's callback function
		// is allowed to do anything, including starting/stopping queries, registering/deregistering records, etc.
		}
	}

mDNSlocal const domainname *PUBLIC_UPDATE_SERVICE_TYPE  = (const domainname*)"\x0B_dns-update"     "\x04_udp";
mDNSlocal const domainname *PUBLIC_LLQ_SERVICE_TYPE     = (const domainname*)"\x08_dns-llq"        "\x04_udp";

mDNSlocal const domainname *PRIVATE_UPDATE_SERVICE_TYPE = (const domainname*)"\x0F_dns-update-tls" "\x04_tcp";
mDNSlocal const domainname *PRIVATE_QUERY_SERVICE_TYPE  = (const domainname*)"\x0E_dns-query-tls"  "\x04_tcp";
mDNSlocal const domainname *PRIVATE_LLQ_SERVICE_TYPE    = (const domainname*)"\x0C_dns-llq-tls"    "\x04_tcp";

#define ZoneDataSRV(X) (\
	(X)->ZoneService == ZoneServiceUpdate ? ((X)->ZonePrivate ? PRIVATE_UPDATE_SERVICE_TYPE : PUBLIC_UPDATE_SERVICE_TYPE) : \
	(X)->ZoneService == ZoneServiceQuery  ? ((X)->ZonePrivate ? PRIVATE_QUERY_SERVICE_TYPE  : (const domainname*)""     ) : \
	(X)->ZoneService == ZoneServiceLLQ    ? ((X)->ZonePrivate ? PRIVATE_LLQ_SERVICE_TYPE    : PUBLIC_LLQ_SERVICE_TYPE   ) : (const domainname*)"")

// Forward reference: GetZoneData_StartQuery references GetZoneData_QuestionCallback, and
// GetZoneData_QuestionCallback calls GetZoneData_StartQuery
mDNSlocal mStatus GetZoneData_StartQuery(mDNS *const m, ZoneData *zd, mDNSu16 qtype);

// GetZoneData_QuestionCallback is called from normal client callback context (core API calls allowed)
mDNSexport void GetZoneData_QuestionCallback(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, QC_result AddRecord)
	{
	ZoneData *zd = (ZoneData*)question->QuestionContext;

	debugf("GetZoneData_QuestionCallback: %s %s", AddRecord ? "Add" : "Rmv", RRDisplayString(m, answer));

	if (!AddRecord) return;												// Don't care about REMOVE events
	if (AddRecord == QC_addnocache && answer->rdlength == 0) return;	// Don't care about transient failure indications
	if (answer->rrtype != question->qtype) return;						// Don't care about CNAMEs

	if (answer->rrtype == kDNSType_SOA)
		{
		debugf("GetZoneData GOT SOA %s", RRDisplayString(m, answer));
		mDNS_StopQuery(m, question);
		if (answer->rdlength)
			{
			AssignDomainName(&zd->ZoneName, answer->name);
			zd->ZoneClass = answer->rrclass;
			AssignDomainName(&zd->question.qname, &zd->ZoneName);
			GetZoneData_StartQuery(m, zd, kDNSType_SRV);
			}
		else if (zd->CurrentSOA->c[0])
			{
			zd->CurrentSOA = (domainname *)(zd->CurrentSOA->c + zd->CurrentSOA->c[0]+1);
			AssignDomainName(&zd->question.qname, zd->CurrentSOA);
			GetZoneData_StartQuery(m, zd, kDNSType_SOA);
			}
		else
			{
			LogMsg("ERROR: GetZoneData_QuestionCallback - recursed to root label of %##s without finding SOA", zd->ChildName.c);
			zd->ZoneDataCallback(m, mStatus_NoSuchNameErr, zd);
			mDNSPlatformMemFree(zd);
			}
		}
	else if (answer->rrtype == kDNSType_SRV)
		{
		debugf("GetZoneData GOT SRV %s", RRDisplayString(m, answer));
		mDNS_StopQuery(m, question);
		if (!answer->rdlength && zd->ZonePrivate && zd->ZoneService != ZoneServiceQuery)
			{
			zd->ZonePrivate = mDNSfalse;	// Causes ZoneDataSRV() to yield a different SRV name when building the query
			GetZoneData_StartQuery(m, zd, kDNSType_SRV);		// Try again, non-private this time
			}
		else
			{
			if (answer->rdlength)
				{
				AssignDomainName(&zd->Host, &answer->rdata->u.srv.target);
				zd->Port = answer->rdata->u.srv.port;
				AssignDomainName(&zd->question.qname, &zd->Host);
				GetZoneData_StartQuery(m, zd, kDNSType_A);
				}
			else
				{
				zd->ZonePrivate = mDNSfalse;
				zd->Host.c[0] = 0;
				zd->Port = zeroIPPort;
				zd->Addr = zeroAddr;
				zd->ZoneDataCallback(m, mStatus_NoError, zd);
				mDNSPlatformMemFree(zd);
				}
			}
		}
	else if (answer->rrtype == kDNSType_A)
		{
		debugf("GetZoneData GOT A %s", RRDisplayString(m, answer));
		mDNS_StopQuery(m, question);
		zd->Addr.type  = mDNSAddrType_IPv4;
		zd->Addr.ip.v4 = (answer->rdlength == 4) ? answer->rdata->u.ipv4 : zerov4Addr;
		zd->ZoneDataCallback(m, mStatus_NoError, zd);
		mDNSPlatformMemFree(zd);
		}
	}

// GetZoneData_StartQuery is called from normal client context (lock not held, or client callback)
mDNSlocal mStatus GetZoneData_StartQuery(mDNS *const m, ZoneData *zd, mDNSu16 qtype)
	{
	if (qtype == kDNSType_SRV)
		{
		LogOperation("lookupDNSPort %##s", ZoneDataSRV(zd));
		AssignDomainName(&zd->question.qname, ZoneDataSRV(zd));
		AppendDomainName(&zd->question.qname, &zd->ZoneName);
		}

	zd->question.ThisQInterval       = -1;		// So that GetZoneData_QuestionCallback() knows whether to cancel this question (Is this necessary?)
	zd->question.InterfaceID         = mDNSInterface_Any;
	zd->question.Target              = zeroAddr;
	//zd->question.qname.c[0]        = 0;			// Already set
	zd->question.qtype               = qtype;
	zd->question.qclass              = kDNSClass_IN;
	zd->question.LongLived           = mDNSfalse;
	zd->question.ExpectUnique        = mDNStrue;
	zd->question.ForceMCast          = mDNSfalse;
	zd->question.ReturnIntermed      = mDNStrue;
	zd->question.QuestionCallback    = GetZoneData_QuestionCallback;
	zd->question.QuestionContext     = zd;

	//LogMsg("GetZoneData_StartQuery %##s (%s) %p", zd->question.qname.c, DNSTypeName(zd->question.qtype), zd->question.Private);
	return(mDNS_StartQuery(m, &zd->question));
	}

// StartGetZoneData is an internal routine (i.e. must be called with the lock already held)
mDNSexport ZoneData *StartGetZoneData(mDNS *const m, const domainname *const name, const ZoneService target, ZoneDataCallback callback, void *ZoneDataContext)
	{
	ZoneData *zd = (ZoneData*)mDNSPlatformMemAllocate(sizeof(ZoneData));
	if (!zd) { LogMsg("ERROR: StartGetZoneData - mDNSPlatformMemAllocate failed"); return mDNSNULL; }

	if (m->mDNS_busy != m->mDNS_reentrancy+1)
		LogMsg("StartGetZoneData: Lock not held! mDNS_busy (%ld) mDNS_reentrancy (%ld)", m->mDNS_busy, m->mDNS_reentrancy);

	mDNSPlatformMemZero(zd, sizeof(ZoneData));
	AssignDomainName(&zd->ChildName, name);
	zd->ZoneService      = target;
	zd->CurrentSOA       = &zd->ChildName;
	zd->ZoneName.c[0]    = 0;
	zd->ZoneClass        = 0;
	zd->Host.c[0]        = 0;
	zd->Port             = zeroIPPort;
	zd->Addr             = zeroAddr;
	zd->ZonePrivate      = GetAuthInfoForName_internal(m, name) ? mDNStrue : mDNSfalse;
	zd->ZoneDataCallback = callback;
	zd->ZoneDataContext  = ZoneDataContext;

	zd->question.QuestionContext = zd;
	AssignDomainName(&zd->question.qname, zd->CurrentSOA);

	mDNS_DropLockBeforeCallback();		// GetZoneData_StartQuery expects to be called from a normal callback, so we emulate that here
	GetZoneData_StartQuery(m, zd, kDNSType_SOA);
	mDNS_ReclaimLockAfterCallback();

	return zd;
	}

// if LLQ NAT context unreferenced, delete the mapping
mDNSlocal void CheckForUnreferencedLLQMapping(mDNS *m)
	{
	NATTraversalInfo *n = m->NATTraversals;
	DNSQuestion *q;

	while (n)
		{
		NATTraversalInfo *current = n;
		n = n->next;
		if (current->clientCallback == LLQNatMapComplete)
			{
			for (q = m->Questions; q; q = q->next) if (&q->NATInfoUDP == current) break;
			if (!q) mDNS_StopNATOperation_internal(m, current);
			}
		}
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - host name and interface management
#endif

// Called in normal client context (lock not held)
mDNSlocal void CompleteSRVNatMap(mDNS *const m, mDNSv4Addr ExternalAddress, NATTraversalInfo *n, mStatus err)
	{
	mDNSBool deletion = !n->portMappingLease;
	ServiceRecordSet *srs = (ServiceRecordSet *)n->clientContext;
	
	(void)ExternalAddress; // Unused

	LogOperation("SRVNatMap complete %.4a %u %u TTL %u", &ExternalAddress, mDNSVal16(n->privatePort), mDNSVal16(n->publicPort), n->portMappingLease);

	if (!srs) { LogMsg("CompleteSRVNatMap called with unknown ServiceRecordSet object"); return; }
	if (deletion) return;

	if (err)
		{
		HostnameInfo *hi = m->Hostnames;
		while (hi)
			{
			if (hi->arv6.state == regState_Registered || hi->arv6.state == regState_Refresh) break;
			else hi = hi->next;
			}

		if (hi)
			{
			debugf("Port map failed for service %##s - using IPv6 service target", srs->RR_SRV.resrec.name->c);
			mDNS_StopNATOperation(m, &srs->NATinfo);
			goto register_service;
			}
		else srs->state = regState_NATError;
		}

	register_service:
	mDNS_Lock(m);
	if (!mDNSIPv4AddressIsZero(srs->ns.ip.v4))
		SendServiceRegistration(m, srs);	// non-zero server address means we already have necessary zone data to send update
	else
		{
		// SHOULD NEVER HAPPEN!
		LogOperation("ERROR: CompleteSRVNatMap called but srs->ns.ip.v4 is zero!");
		srs->state = regState_FetchingZoneData;
		if (srs->nta) CancelGetZoneData(m, srs->nta); // Make sure we cancel old one before we start a new one
		srs->nta = StartGetZoneData(m, srs->RR_SRV.resrec.name, ZoneServiceUpdate, ServiceRegistrationZoneDataComplete, srs);
		}
	mDNS_Unlock(m);
	}

mDNSlocal void StartSRVNatMap(mDNS *m, ServiceRecordSet *srs)
	{
	mDNSu8 protocol;
	mDNSu8 *p = srs->RR_PTR.resrec.name->c;
	if (p[0]) p += 1 + p[0];
	if      (SameDomainLabel(p, (mDNSu8 *)"\x4" "_tcp")) protocol = kDNSServiceProtocol_TCP;
	else if (SameDomainLabel(p, (mDNSu8 *)"\x4" "_udp")) protocol = kDNSServiceProtocol_UDP;
	else { LogMsg("StartSRVNatMap: could not determine transport protocol of service %##s", srs->RR_SRV.resrec.name->c); goto error; }

	mDNSPlatformMemZero(&srs->NATinfo, sizeof(NATTraversalInfo));
	
	srs->NATinfo.privatePort = srs->NATinfo.publicPortreq = srs->RR_SRV.resrec.rdata->u.srv.port;
	srs->NATinfo.protocol = protocol;
	srs->NATinfo.clientCallback = CompleteSRVNatMap;
	srs->NATinfo.clientContext = srs;
	mDNS_StartNATOperation_internal(m, &srs->NATinfo);
	return;

	error:
	if (srs->nta) CancelGetZoneData(m, srs->nta); // Make sure we cancel old one before we start a new one
	srs->nta = StartGetZoneData(m, srs->RR_SRV.resrec.name, ZoneServiceUpdate, ServiceRegistrationZoneDataComplete, srs);
	}

// Called in normal callback context (i.e. mDNS_busy and mDNS_reentrancy are both 1)
mDNSexport void ServiceRegistrationZoneDataComplete(mDNS *const m, mStatus err, const ZoneData *zoneData)
	{
	ServiceRecordSet *srs = (ServiceRecordSet *)zoneData->ZoneDataContext;
	
	if (m->mDNS_busy != m->mDNS_reentrancy)
		LogMsg("ServiceRegistrationZoneDataComplete: mDNS_busy (%ld) != mDNS_reentrancy (%ld)", m->mDNS_busy, m->mDNS_reentrancy);

	srs->nta = mDNSNULL;

	if (err) goto error;
	if (!zoneData) { LogMsg("ERROR: ServiceRegistrationZoneDataComplete invoked with NULL result and no error"); goto error; }

	if (srs->RR_SRV.resrec.rrclass != zoneData->ZoneClass)
		{ LogMsg("Service %##s - class does not match zone", srs->RR_SRV.resrec.name->c); goto error; }

	// cache zone data
	AssignDomainName(&srs->zone, &zoneData->ZoneName);
	srs->ns.type = mDNSAddrType_IPv4;
	srs->ns = zoneData->Addr;
	if (!mDNSIPPortIsZero(zoneData->Port))
		{
		srs->SRSUpdatePort = zoneData->Port;
		srs->Private = zoneData->ZonePrivate;
		}
	else
		{
		debugf("Update port not advertised via SRV - guessing port 53, no lease option");
		srs->SRSUpdatePort = UnicastDNSPort;
		srs->srs_uselease = mDNSfalse;
		}

	LogOperation("ServiceRegistrationZoneDataComplete %#a %d %#a %d", &m->AdvertisedV4, mDNSv4AddrIsRFC1918(&m->AdvertisedV4.ip.v4), &srs->ns, mDNSAddrIsRFC1918(&srs->ns));

	if (!mDNSIPPortIsZero(srs->RR_SRV.resrec.rdata->u.srv.port) &&
		mDNSv4AddrIsRFC1918(&m->AdvertisedV4.ip.v4) && !mDNSAddrIsRFC1918(&srs->ns) &&
		srs->RR_SRV.AutoTarget == Target_AutoHostAndNATMAP)
		{
		srs->state = regState_NATMap;
		LogOperation("ServiceRegistrationZoneDataComplete StartSRVNatMap");
		StartSRVNatMap(m, srs);
		}
	else
		{
		mDNS_Lock(m);
		SendServiceRegistration(m, srs);
		mDNS_Unlock(m);
		}
	return;

error:
	unlinkSRS(m, srs);
	srs->state = regState_Unregistered;

	// Don't need to do the mDNS_DropLockBeforeCallback stuff here, because this code is
	// *already* being invoked in the right callback context, with mDNS_reentrancy correctly incremented.
	srs->ServiceCallback(m, srs, err);
	// CAUTION: MUST NOT do anything more with rr after calling srs->ServiceCallback(), because the client's callback function
	// is allowed to do anything, including starting/stopping queries, registering/deregistering records, etc.
	}

mDNSlocal void SendServiceDeregistration(mDNS *m, ServiceRecordSet *srs)
	{
	mDNSOpaque16 id;
	mDNSu8 *ptr = m->omsg.data;
	mDNSu8 *end = (mDNSu8 *)&m->omsg + sizeof(DNSMessage);
	mStatus err = mStatus_UnknownErr;
	mDNSu32 i;

	id = mDNS_NewMessageID(m);
	InitializeDNSMessage(&m->omsg.h, id, UpdateReqFlags);

	// put zone
	ptr = putZone(&m->omsg, ptr, end, &srs->zone, mDNSOpaque16fromIntVal(srs->RR_SRV.resrec.rrclass));
	if (!ptr) { LogMsg("ERROR: SendServiceDeregistration - putZone"); err = mStatus_UnknownErr; goto exit; }

	if (!(ptr = putDeleteAllRRSets(&m->omsg, ptr, srs->RR_SRV.resrec.name))) { err = mStatus_UnknownErr; goto exit; } // this deletes SRV, TXT, and Extras
	if (!(ptr = putDeletionRecord(&m->omsg, ptr, &srs->RR_PTR.resrec))) { err = mStatus_UnknownErr; goto exit; }
	for (i = 0; i < srs->NumSubTypes; i++)
		if (!(ptr = putDeletionRecord(&m->omsg, ptr, &srs->SubTypes[i].resrec))) { err = mStatus_UnknownErr; goto exit; }

	srs->id    = id;
	srs->state = regState_DeregPending;

	if (srs->Private)
		{
		LogOperation("SendServiceDeregistration for %s", ARDisplayString(m, &srs->RR_SRV));
		LogOperation("SendServiceDeregistration TCP %p", srs->tcp);
		if (srs->tcp) LogMsg("SendServiceDeregistration: Already have TCP connection for %s", ARDisplayString(m, &srs->RR_SRV));
		else srs->tcp = MakeTCPConn(m, &m->omsg, ptr, kTCPSocketFlags_UseTLS, &srs->ns, srs->SRSUpdatePort, mDNSNULL, srs, mDNSNULL);
		srs->RR_SRV.LastAPTime = m->timenow;
		srs->RR_SRV.ThisAPInterval = 0x3FFFFFFF; // TCP will handle any necessary retransmissions for us
		}
	else
		{
		err = mDNSSendDNSMessage(m, &m->omsg, ptr, mDNSInterface_Any, &srs->ns, srs->SRSUpdatePort, mDNSNULL, GetAuthInfoForName_internal(m, srs->RR_SRV.resrec.name));
		if (err && err != mStatus_TransientErr) { debugf("ERROR: SendServiceDeregistration - mDNSSendDNSMessage - %ld", err); goto exit; }
		SetRecordRetry(m, &srs->RR_SRV, err);
		}

	err = mStatus_NoError;

exit:

	if (err)
		{
		unlinkSRS(m, srs);
		srs->state = regState_Unregistered;
		}
	}

// Called with lock held
mDNSlocal void UpdateSRV(mDNS *m, ServiceRecordSet *srs)
	{
	ExtraResourceRecord *e;

	// Target change if:
	// We have a target and were previously waiting for one, or
	// We had a target and no longer do, or
	// The target has changed

	domainname *curtarget = &srs->RR_SRV.resrec.rdata->u.srv.target;
	const domainname *const nt = GetServiceTarget(m, srs);
	const domainname *const newtarget = nt ? nt : (domainname*)"";
	mDNSBool TargetChanged = (newtarget->c[0] && srs->state == regState_NoTarget) || !SameDomainName(curtarget, newtarget);
	mDNSBool HaveZoneData  = !mDNSIPv4AddressIsZero(srs->ns.ip.v4);

	// Nat state change if:
	// We were behind a NAT, and now we are behind a new NAT, or
	// We're not behind a NAT but our port was previously mapped to a different public port
	// We were not behind a NAT and now we are

	mDNSIPPort port        = srs->RR_SRV.resrec.rdata->u.srv.port;
	mDNSBool NATChanged    = mDNSfalse;
	mDNSBool NowBehindNAT  = (!mDNSIPPortIsZero(port) && mDNSv4AddrIsRFC1918(&m->AdvertisedV4.ip.v4) && !mDNSAddrIsRFC1918(&srs->ns));
	mDNSBool WereBehindNAT = (srs->NATinfo.clientContext != mDNSNULL);
	mDNSBool PortWasMapped = (srs->NATinfo.clientContext && !mDNSIPPortIsZero(srs->NATinfo.publicPort) && !mDNSSameIPPort(srs->NATinfo.publicPort, port));

	LogOperation("UpdateSRV %##s", srs->RR_SRV.resrec.name->c);

	if (m->mDNS_busy != m->mDNS_reentrancy+1)
		LogMsg("UpdateSRV: Lock not held! mDNS_busy (%ld) mDNS_reentrancy (%ld)", m->mDNS_busy, m->mDNS_reentrancy);

	else if (!NowBehindNAT && PortWasMapped)               NATChanged = mDNStrue;
	else if (!WereBehindNAT && NowBehindNAT)               NATChanged = mDNStrue;

	if (!TargetChanged && !NATChanged) return;

	debugf("UpdateSRV (%##s) HadZoneData=%d, TargetChanged=%d, newtarget=%##s, NowBehindNAT=%d, WereBehindNAT=%d, PortWasMapped=%d",
		   srs->RR_SRV.resrec.name->c,  HaveZoneData, TargetChanged, newtarget, NowBehindNAT, WereBehindNAT, PortWasMapped);

	switch(srs->state)
		{
		case regState_FetchingZoneData:
		case regState_DeregPending:
		case regState_DeregDeferred:
		case regState_Unregistered:
		case regState_NATMap:
		case regState_ExtraQueued:
			// In these states, the SRV has either not yet been registered (it will get up-to-date information when it is)
			// or is in the process of, or has already been, deregistered
			return;

		case regState_Pending:
		case regState_Refresh:
		case regState_UpdatePending:
			// let the in-flight operation complete before updating
			srs->SRVUpdateDeferred = mDNStrue;
			return;

		case regState_NATError:
			if (!NATChanged) return;
			// if nat changed, register if we have a target (below)

		case regState_NoTarget:
			if (newtarget->c[0])
				{
				debugf("UpdateSRV: %s service %##s", HaveZoneData ? (NATChanged && NowBehindNAT ? "Starting Port Map for" : "Registering") : "Getting Zone Data for", srs->RR_SRV.resrec.name->c);
				if (!HaveZoneData)
					{
					srs->state = regState_FetchingZoneData;
					if (srs->nta) CancelGetZoneData(m, srs->nta); // Make sure we cancel old one before we start a new one
					srs->nta = StartGetZoneData(m, srs->RR_SRV.resrec.name, ZoneServiceUpdate, ServiceRegistrationZoneDataComplete, srs);
					}
				else
					{
					if (srs->NATinfo.clientContext && (NATChanged || !NowBehindNAT))
						{
						mDNS_StopNATOperation_internal(m, &srs->NATinfo);
						srs->NATinfo.clientContext = mDNSNULL;
						}
					if (NATChanged && NowBehindNAT && srs->RR_SRV.AutoTarget == Target_AutoHostAndNATMAP)
						{ srs->state = regState_NATMap; StartSRVNatMap(m, srs); }
					else SendServiceRegistration(m, srs);
					}
				}
			return;

		case regState_Registered:
			// target or nat changed.  deregister service.  upon completion, we'll look for a new target
			debugf("UpdateSRV: SRV record changed for service %##s - deregistering (will re-register with new SRV)",  srs->RR_SRV.resrec.name->c);
			for (e = srs->Extras; e; e = e->next) e->r.state = regState_ExtraQueued;	// extra will be re-registed if the service is re-registered
			srs->SRVChanged = mDNStrue;
			SendServiceDeregistration(m, srs);
			return;

		default: LogMsg("UpdateSRV: Unknown state %d for %##s", srs->state, srs->RR_SRV.resrec.name->c);
		}
	}

// Called with lock held
mDNSlocal void UpdateSRVRecords(mDNS *m)
	{
	if (CurrentServiceRecordSet)
		LogMsg("UpdateSRVRecords ERROR CurrentServiceRecordSet already set");
	CurrentServiceRecordSet = m->ServiceRegistrations;
	
	while (CurrentServiceRecordSet)
		{
		ServiceRecordSet *s = CurrentServiceRecordSet;
		CurrentServiceRecordSet = CurrentServiceRecordSet->uDNS_next;
		UpdateSRV(m, s);
		}
	}

// Forward reference: AdvertiseHostname references HostnameCallback, and HostnameCallback calls AdvertiseHostname
mDNSlocal void HostnameCallback(mDNS *const m, AuthRecord *const rr, mStatus result);

// Called in normal client context (lock not held)
mDNSlocal void hostnameGetPublicAddressCallback(mDNS *const m, mDNSv4Addr ExternalAddress, NATTraversalInfo *n, mStatus err)
	{
	HostnameInfo *h = (HostnameInfo *)n->clientContext;

	if (!h) { LogMsg("RegisterHostnameRecord: registration cancelled"); return; }

	if (!err)
		{
		if (mDNSIPv4AddressIsZero(ExternalAddress) || mDNSv4AddrIsRFC1918(&ExternalAddress)) return;
		h->arv4.resrec.rdata->u.ipv4 = ExternalAddress;
		LogMsg("Advertising hostname %##s IPv4 %.4a", h->arv4.resrec.name->c, &ExternalAddress);
		mDNS_Register(m, &h->arv4);
		}
	else
		{
		mDNS_StopNATOperation_internal(m, n);
		h->arv4.state = regState_Unregistered;	// note that rr is not yet in global list
		if (h->arv4.RecordCallback)
			h->arv4.RecordCallback(m, &h->arv4, mStatus_NATTraversal);
		// note - unsafe to touch rr after callback
		}
	}

// register record or begin NAT traversal
mDNSlocal void AdvertiseHostname(mDNS *m, HostnameInfo *h)
	{
	if (!mDNSIPv4AddressIsZero(m->AdvertisedV4.ip.v4) && h->arv4.resrec.RecordType == kDNSRecordTypeUnregistered)
		{
		mDNS_SetupResourceRecord(&h->arv4, mDNSNULL, mDNSInterface_Any, kDNSType_A, kHostNameTTL, kDNSRecordTypeKnownUnique, HostnameCallback, h);
		AssignDomainName(&h->arv4.namestorage, &h->fqdn);
		h->arv4.resrec.rdata->u.ipv4 = m->AdvertisedV4.ip.v4;
		h->arv4.state = regState_Unregistered;
		if (mDNSv4AddrIsRFC1918(&m->AdvertisedV4.ip.v4))
			{
			h->natinfo.clientCallback   = hostnameGetPublicAddressCallback;
			h->natinfo.clientContext    = h;
			h->natinfo.protocol         = 0;
			h->natinfo.privatePort      = zeroIPPort;
			h->natinfo.publicPortreq    = zeroIPPort;
			h->natinfo.portMappingLease = 0;
			mDNS_StartNATOperation_internal(m, &h->natinfo);
			}
		else
			{
			LogMsg("Advertising hostname %##s IPv4 %.4a", h->arv4.resrec.name->c, &m->AdvertisedV4.ip.v4);
			mDNS_Register_internal(m, &h->arv4);
			}
		}

	if (!mDNSIPv6AddressIsZero(m->AdvertisedV6.ip.v6) && h->arv6.resrec.RecordType == kDNSRecordTypeUnregistered)
		{
		mDNS_SetupResourceRecord(&h->arv6, mDNSNULL, mDNSInterface_Any, kDNSType_AAAA, kHostNameTTL, kDNSRecordTypeKnownUnique, HostnameCallback, h);
		AssignDomainName(&h->arv6.namestorage, &h->fqdn);
		h->arv6.resrec.rdata->u.ipv6 = m->AdvertisedV6.ip.v6;
		h->arv6.state = regState_Unregistered;
		LogMsg("Advertising hostname %##s IPv6 %.16a", h->arv6.resrec.name->c, &m->AdvertisedV6.ip.v6);
		mDNS_Register_internal(m, &h->arv6);
		}
	}

mDNSlocal void HostnameCallback(mDNS *const m, AuthRecord *const rr, mStatus result)
	{
	HostnameInfo *hi = (HostnameInfo *)rr->RecordContext;

	if (result == mStatus_MemFree)
		{
		if (hi)
			{
			// If we're still in the Hostnames list, update to new address
			HostnameInfo *i;
			LogOperation("Got mStatus_MemFree for %s", ARDisplayString(m, rr));
			for (i = m->Hostnames; i; i = i->next)
				{
				if (rr == &i->arv4) { mDNS_Lock(m); AdvertiseHostname(m, i); mDNS_Unlock(m); return; }
				if (rr == &i->arv6) { mDNS_Lock(m); AdvertiseHostname(m, i); mDNS_Unlock(m); return; }
				}

			// Else, we're not still in the Hostnames list, so free the memory
			if (hi->arv4.resrec.RecordType == kDNSRecordTypeUnregistered &&
				hi->arv6.resrec.RecordType == kDNSRecordTypeUnregistered)
				mDNSPlatformMemFree(hi);	// free hi when both v4 and v6 AuthRecs deallocated
			}
		return;
		}

	if (result)
		{
		// don't unlink or free - we can retry when we get a new address/router
		if (rr->resrec.rrtype == kDNSType_A)
			LogMsg("HostnameCallback: Error %ld for registration of %##s IP %.4a", result, rr->resrec.name->c, &rr->resrec.rdata->u.ipv4);
		else
			LogMsg("HostnameCallback: Error %ld for registration of %##s IP %.16a", result, rr->resrec.name->c, &rr->resrec.rdata->u.ipv6);
		if (!hi) { mDNSPlatformMemFree(rr); return; }
		if (rr->state != regState_Unregistered) LogMsg("Error: HostnameCallback invoked with error code for record not in regState_Unregistered!");

		if (hi->arv4.state == regState_Unregistered &&
			hi->arv6.state == regState_Unregistered)
			{
			// only deliver status if both v4 and v6 fail
			rr->RecordContext = (void *)hi->StatusContext;
			if (hi->StatusCallback)
				hi->StatusCallback(m, rr, result); // client may NOT make API calls here
			rr->RecordContext = (void *)hi;
			}
		return;
		}

	// register any pending services that require a target
	mDNS_Lock(m);
	UpdateSRVRecords(m);
	mDNS_Unlock(m);

	// Deliver success to client
	if (!hi) { LogMsg("HostnameCallback invoked with orphaned address record"); return; }
	if (rr->resrec.rrtype == kDNSType_A)
		LogMsg("Registered hostname %##s IP %.4a", rr->resrec.name->c, &rr->resrec.rdata->u.ipv4);
	else
		LogMsg("Registered hostname %##s IP %.16a", rr->resrec.name->c, &rr->resrec.rdata->u.ipv6);

	rr->RecordContext = (void *)hi->StatusContext;
	if (hi->StatusCallback)
		hi->StatusCallback(m, rr, result); // client may NOT make API calls here
	rr->RecordContext = (void *)hi;
	}

mDNSlocal void FoundStaticHostname(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, QC_result AddRecord)
	{
	const domainname *pktname = &answer->rdata->u.name;
	domainname *storedname = &m->StaticHostname;
	HostnameInfo *h = m->Hostnames;

	(void)question;

	debugf("FoundStaticHostname: %##s -> %##s (%s)", question->qname.c, answer->rdata->u.name.c, AddRecord ? "added" : "removed");
	if (AddRecord && !SameDomainName(pktname, storedname))
		{
		AssignDomainName(storedname, pktname);
		while (h)
			{
			if (h->arv4.state == regState_FetchingZoneData || h->arv4.state == regState_Pending || h->arv4.state == regState_NATMap ||
				h->arv6.state == regState_FetchingZoneData || h->arv6.state == regState_Pending)
				{
				// if we're in the process of registering a dynamic hostname, delay SRV update so we don't have to reregister services if the dynamic name succeeds
				m->NextSRVUpdate = NonZeroTime(m->timenow + (5 * mDNSPlatformOneSecond));
				return;
				}
			h = h->next;
			}
		mDNS_Lock(m);
		UpdateSRVRecords(m);
		mDNS_Unlock(m);
		}
	else if (!AddRecord && SameDomainName(pktname, storedname))
		{
		mDNS_Lock(m);
		storedname->c[0] = 0;
		UpdateSRVRecords(m);
		mDNS_Unlock(m);
		}
	}

// Called with lock held
mDNSlocal void GetStaticHostname(mDNS *m)
	{
	char buf[MAX_REVERSE_MAPPING_NAME_V4];
	DNSQuestion *q = &m->ReverseMap;
	mDNSu8 *ip = m->AdvertisedV4.ip.v4.b;
	mStatus err;

	if (m->ReverseMap.ThisQInterval != -1) mDNS_StopQuery_internal(m, q);

	m->StaticHostname.c[0] = 0;
	if (mDNSIPv4AddressIsZero(m->AdvertisedV4.ip.v4)) return;
	mDNSPlatformMemZero(q, sizeof(*q));
	// Note: This is reverse order compared to a normal dotted-decimal IP address, so we can't use our customary "%.4a" format code
	mDNS_snprintf(buf, sizeof(buf), "%d.%d.%d.%d.in-addr.arpa.", ip[3], ip[2], ip[1], ip[0]);
	if (!MakeDomainNameFromDNSNameString(&q->qname, buf)) { LogMsg("Error: GetStaticHostname - bad name %s", buf); return; }

	q->InterfaceID      = mDNSInterface_Any;
	q->Target           = zeroAddr;
	q->qtype            = kDNSType_PTR;
	q->qclass           = kDNSClass_IN;
	q->LongLived        = mDNSfalse;
	q->ExpectUnique     = mDNSfalse;
	q->ForceMCast       = mDNSfalse;
	q->ReturnIntermed   = mDNStrue;
	q->QuestionCallback = FoundStaticHostname;
	q->QuestionContext  = mDNSNULL;

	err = mDNS_StartQuery_internal(m, q);
	if (err) LogMsg("Error: GetStaticHostname - StartQuery returned error %d", err);
	}

mDNSexport void mDNS_AddDynDNSHostName(mDNS *m, const domainname *fqdn, mDNSRecordCallback *StatusCallback, const void *StatusContext)
   {
	HostnameInfo **ptr = &m->Hostnames;

	LogOperation("mDNS_AddDynDNSHostName %##s", fqdn);

	while (*ptr && !SameDomainName(fqdn, &(*ptr)->fqdn)) ptr = &(*ptr)->next;
	if (*ptr) { LogMsg("DynDNSHostName %##s already in list", fqdn->c); return; }

	// allocate and format new address record
	*ptr = mDNSPlatformMemAllocate(sizeof(**ptr));
	if (!*ptr) { LogMsg("ERROR: mDNS_AddDynDNSHostName - malloc"); return; }

	mDNSPlatformMemZero(*ptr, sizeof(**ptr));
	AssignDomainName(&(*ptr)->fqdn, fqdn);
	(*ptr)->arv4.state     = regState_Unregistered;
	(*ptr)->arv6.state     = regState_Unregistered;
	(*ptr)->StatusCallback = StatusCallback;
	(*ptr)->StatusContext  = StatusContext;

	AdvertiseHostname(m, *ptr);
	}

mDNSexport void mDNS_RemoveDynDNSHostName(mDNS *m, const domainname *fqdn)
	{
	HostnameInfo **ptr = &m->Hostnames;

	LogOperation("mDNS_RemoveDynDNSHostName %##s", fqdn);

	while (*ptr && !SameDomainName(fqdn, &(*ptr)->fqdn)) ptr = &(*ptr)->next;
	if (!*ptr) LogMsg("mDNS_RemoveDynDNSHostName: no such domainname %##s", fqdn->c);
	else
		{
		HostnameInfo *hi = *ptr;
		// We do it this way because, if we have no active v6 record, the "mDNS_Deregister_internal(m, &hi->arv4);"
		// below could free the memory, and we have to make sure we don't touch hi fields after that.
		mDNSBool f4 = hi->arv4.resrec.RecordType != kDNSRecordTypeUnregistered && hi->arv4.state != regState_Unregistered;
		mDNSBool f6 = hi->arv6.resrec.RecordType != kDNSRecordTypeUnregistered && hi->arv6.state != regState_Unregistered;
		if (f4) LogOperation("mDNS_RemoveDynDNSHostName removing v4 %##s", fqdn);
		if (f6) LogOperation("mDNS_RemoveDynDNSHostName removing v6 %##s", fqdn);
		*ptr = (*ptr)->next; // unlink
		if (f4) mDNS_Deregister_internal(m, &hi->arv4, mDNS_Dereg_normal);
		if (f6) mDNS_Deregister_internal(m, &hi->arv6, mDNS_Dereg_normal);
		// When both deregistrations complete we'll free the memory in the mStatus_MemFree callback
		}
	UpdateSRVRecords(m);
	}

// Currently called without holding the lock
// Maybe we should change that?
mDNSexport void mDNS_SetPrimaryInterfaceInfo(mDNS *m, const mDNSAddr *v4addr, const mDNSAddr *v6addr, const mDNSAddr *router)
	{
	mDNSBool v4Changed, v6Changed, RouterChanged;

	if (m->mDNS_busy != m->mDNS_reentrancy)
		LogMsg("mDNS_SetPrimaryInterfaceInfo: mDNS_busy (%ld) != mDNS_reentrancy (%ld)", m->mDNS_busy, m->mDNS_reentrancy);

	if (v4addr && v4addr->type != mDNSAddrType_IPv4) { LogMsg("mDNS_SetPrimaryInterfaceInfo v4 address - incorrect type.  Discarding. %#a", v4addr); return; }
	if (v6addr && v6addr->type != mDNSAddrType_IPv6) { LogMsg("mDNS_SetPrimaryInterfaceInfo v6 address - incorrect type.  Discarding. %#a", v6addr); return; }
	if (router && router->type != mDNSAddrType_IPv4) { LogMsg("mDNS_SetPrimaryInterfaceInfo passed non-v4 router.  Discarding. %#a",        router); return; }

	mDNS_Lock(m);

	if (v4addr && !mDNSv4AddressIsLinkLocal(&v4addr->ip.v4)) v6addr = mDNSNULL;

	v4Changed     = !mDNSSameIPv4Address(m->AdvertisedV4.ip.v4, v4addr ? v4addr->ip.v4 : zerov4Addr);
	v6Changed     = !mDNSSameIPv6Address(m->AdvertisedV6.ip.v6, v6addr ? v6addr->ip.v6 : zerov6Addr);
	RouterChanged = !mDNSSameIPv4Address(m->Router.ip.v4,       router ? router->ip.v4 : zerov4Addr);

	if (v4addr && (v4Changed || RouterChanged))
		debugf("mDNS_SetPrimaryInterfaceInfo: address changed from %#a to %#a", &m->AdvertisedV4, v4addr);

	if (v4addr) m->AdvertisedV4 = *v4addr; else m->AdvertisedV4.ip.v4 = zerov4Addr;
	if (v6addr) m->AdvertisedV6 = *v6addr; else m->AdvertisedV6.ip.v6 = zerov6Addr;
	if (router) m->Router       = *router; else m->Router      .ip.v4 = zerov4Addr;
	// setting router to zero indicates that nat mappings must be reestablished when router is reset

	if (v4Changed || RouterChanged || v6Changed)
		{
		HostnameInfo *i;
		LogOperation("mDNS_SetPrimaryInterfaceInfo: %s%s%s%#a",
			v4Changed     ? "v4Changed "     : "",
			RouterChanged ? "RouterChanged " : "",
			v6Changed     ? "v6Changed "     : "", v4addr);

		for (i = m->Hostnames; i; i = i->next)
			{
			LogOperation("mDNS_SetPrimaryInterfaceInfo updating host name registrations for %##s", i->fqdn.c);
	
			if (i->arv4.resrec.RecordType > kDNSRecordTypeDeregistering &&
				!mDNSSameIPv4Address(i->arv4.resrec.rdata->u.ipv4, m->AdvertisedV4.ip.v4))
				{
				LogOperation("mDNS_SetPrimaryInterfaceInfo deregistering %s", ARDisplayString(m, &i->arv4));
				mDNS_Deregister_internal(m, &i->arv4, mDNS_Dereg_normal);
				}
	
			if (i->arv6.resrec.RecordType > kDNSRecordTypeDeregistering &&
				!mDNSSameIPv6Address(i->arv6.resrec.rdata->u.ipv6, m->AdvertisedV6.ip.v6))
				{
				LogOperation("mDNS_SetPrimaryInterfaceInfo deregistering %s", ARDisplayString(m, &i->arv6));
				mDNS_Deregister_internal(m, &i->arv6, mDNS_Dereg_normal);
				}

			// AdvertiseHostname will only register new address records.
			// For records still in the process of deregistering it will ignore them, and let the mStatus_MemFree callback handle them.
			AdvertiseHostname(m, i);
			}

		if (v4Changed || RouterChanged)
			{
			m->retryGetAddr = m->timenow;
			m->retryIntervalGetAddr = NATMAP_INIT_RETRY;
			// if the last network did not support NATs the ExternalAddress will be set to zero, so here
			// pass onesIPv4Addr instead of zero so the proper transitions happen later
			SetExternalAddress(m, onesIPv4Addr, 0, m->timenow);
			}

		UpdateSRVRecords(m);
		GetStaticHostname(m);	// look up reverse map record to find any static hostnames for our IP address
		}

	mDNS_Unlock(m);
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Incoming Message Processing
#endif

mDNSlocal mStatus ParseTSIGError(mDNS *const m, const DNSMessage *const msg, const mDNSu8 *const end, const domainname *const displayname)
	{
	const mDNSu8 *ptr;
	mStatus err = mStatus_NoError;
	int i;

	ptr = LocateAdditionals(msg, end);
	if (!ptr) goto finish;

	for (i = 0; i < msg->h.numAdditionals; i++)
		{
		ptr = GetLargeResourceRecord(m, msg, ptr, end, 0, kDNSRecordTypePacketAdd, &m->rec);
		if (!ptr) goto finish;
		if (m->rec.r.resrec.rrtype == kDNSType_TSIG)
			{
			mDNSu32 macsize;
			mDNSu8 *rd = m->rec.r.resrec.rdata->u.data;
			mDNSu8 *rdend = rd + m->rec.r.resrec.rdlength;
			int alglen = DomainNameLengthLimit(&m->rec.r.resrec.rdata->u.name, rdend);
			if (alglen > MAX_DOMAIN_NAME) goto finish;
			rd += alglen;                                       // algorithm name
			if (rd + 6 > rdend) goto finish;
			rd += 6;                                            // 48-bit timestamp
			if (rd + sizeof(mDNSOpaque16) > rdend) goto finish;
			rd += sizeof(mDNSOpaque16);                         // fudge
			if (rd + sizeof(mDNSOpaque16) > rdend) goto finish;
			macsize = mDNSVal16(*(mDNSOpaque16 *)rd);
			rd += sizeof(mDNSOpaque16);                         // MAC size
			if (rd + macsize > rdend) goto finish;
			rd += macsize;
			if (rd + sizeof(mDNSOpaque16) > rdend) goto finish;
			rd += sizeof(mDNSOpaque16);                         // orig id
			if (rd + sizeof(mDNSOpaque16) > rdend) goto finish;
			err = mDNSVal16(*(mDNSOpaque16 *)rd);               // error code

			if      (err == TSIG_ErrBadSig)  { LogMsg("%##s: bad signature", displayname->c);              err = mStatus_BadSig;     }
			else if (err == TSIG_ErrBadKey)  { LogMsg("%##s: bad key", displayname->c);                    err = mStatus_BadKey;     }
			else if (err == TSIG_ErrBadTime) { LogMsg("%##s: bad time", displayname->c);                   err = mStatus_BadTime;    }
			else if (err)                    { LogMsg("%##s: unknown tsig error %d", displayname->c, err); err = mStatus_UnknownErr; }
			goto finish;
			}
		m->rec.r.resrec.RecordType = 0;		// Clear RecordType to show we're not still using it
		}

	finish:
	m->rec.r.resrec.RecordType = 0;		// Clear RecordType to show we're not still using it
	return err;
	}

mDNSlocal mStatus checkUpdateResult(mDNS *const m, const domainname *const displayname, const mDNSu8 rcode, const DNSMessage *const msg, const mDNSu8 *const end)
	{
	(void)msg;	// currently unused, needed for TSIG errors
	if (!rcode) return mStatus_NoError;
	else if (rcode == kDNSFlag1_RC_YXDomain)
		{
		debugf("name in use: %##s", displayname->c);
		return mStatus_NameConflict;
		}
	else if (rcode == kDNSFlag1_RC_Refused)
		{
		LogMsg("Update %##s refused", displayname->c);
		return mStatus_Refused;
		}
	else if (rcode == kDNSFlag1_RC_NXRRSet)
		{
		LogMsg("Reregister refused (NXRRSET): %##s", displayname->c);
		return mStatus_NoSuchRecord;
		}
	else if (rcode == kDNSFlag1_RC_NotAuth)
		{
		// TSIG errors should come with FmtErr as per RFC 2845, but BIND 9 sends them with NotAuth so we look here too
		mStatus tsigerr = ParseTSIGError(m, msg, end, displayname);
		if (!tsigerr)
			{
			LogMsg("Permission denied (NOAUTH): %##s", displayname->c);
			return mStatus_UnknownErr;
			}
		else return tsigerr;
		}
	else if (rcode == kDNSFlag1_RC_FmtErr)
		{
		mStatus tsigerr = ParseTSIGError(m, msg, end, displayname);
		if (!tsigerr)
			{
			LogMsg("Format Error: %##s", displayname->c);
			return mStatus_UnknownErr;
			}
		else return tsigerr;
		}
	else
		{
		LogMsg("Update %##s failed with rcode %d", displayname->c, rcode);
		return mStatus_UnknownErr;
		}
	}

// Called with lock held
mDNSlocal void sendRecordRegistration(mDNS *const m, AuthRecord *rr)
	{
	mDNSu8 *ptr = m->omsg.data;
	mDNSu8 *end = (mDNSu8 *)&m->omsg + sizeof(DNSMessage);
	mStatus err = mStatus_UnknownErr;

	if (m->mDNS_busy != m->mDNS_reentrancy+1)
		LogMsg("sendRecordRegistration: Lock not held! mDNS_busy (%ld) mDNS_reentrancy (%ld)", m->mDNS_busy, m->mDNS_reentrancy);

	rr->id = mDNS_NewMessageID(m);
	InitializeDNSMessage(&m->omsg.h, rr->id, UpdateReqFlags);

	// set zone
	ptr = putZone(&m->omsg, ptr, end, &rr->zone, mDNSOpaque16fromIntVal(rr->resrec.rrclass));
	if (!ptr) { err = mStatus_UnknownErr; goto exit; }

	if (rr->state == regState_UpdatePending)
		{
		// delete old RData
		SetNewRData(&rr->resrec, rr->OrigRData, rr->OrigRDLen);
		if (!(ptr = putDeletionRecord(&m->omsg, ptr, &rr->resrec))) { err = mStatus_UnknownErr; goto exit; } // delete old rdata

		// add new RData
		SetNewRData(&rr->resrec, rr->InFlightRData, rr->InFlightRDLen);
		if (!(ptr = PutResourceRecordTTLJumbo(&m->omsg, ptr, &m->omsg.h.mDNS_numUpdates, &rr->resrec, rr->resrec.rroriginalttl))) { err = mStatus_UnknownErr; goto exit; }
		}

	else
		{
		if (rr->resrec.RecordType == kDNSRecordTypeKnownUnique)
			{
			// KnownUnique: Delete any previous value
			ptr = putDeleteRRSet(&m->omsg, ptr, rr->resrec.name, rr->resrec.rrtype);
			if (!ptr) { err = mStatus_UnknownErr; goto exit; }
			}

		else if (rr->resrec.RecordType != kDNSRecordTypeShared)
			{
			ptr = putPrereqNameNotInUse(rr->resrec.name, &m->omsg, ptr, end);
			if (!ptr) { err = mStatus_UnknownErr; goto exit; }
			}

		ptr = PutResourceRecordTTLJumbo(&m->omsg, ptr, &m->omsg.h.mDNS_numUpdates, &rr->resrec, rr->resrec.rroriginalttl);
		if (!ptr) { err = mStatus_UnknownErr; goto exit; }
		}

	if (rr->uselease)
		{
		ptr = putUpdateLease(&m->omsg, ptr, DEFAULT_UPDATE_LEASE); if (!ptr) { err = mStatus_UnknownErr; goto exit; }
		}

	if (rr->Private)
		{
		LogOperation("sendRecordRegistration TCP %p", rr->tcp);
		if (rr->tcp) LogMsg("sendRecordRegistration: Already have TCP connection for %s", ARDisplayString(m, rr));
		else rr->tcp = MakeTCPConn(m, &m->omsg, ptr, kTCPSocketFlags_UseTLS, &rr->UpdateServer, rr->UpdatePort, mDNSNULL, mDNSNULL, rr);
		rr->LastAPTime = m->timenow;
		rr->ThisAPInterval = 0x3FFFFFFF; // TCP will handle any necessary retransmissions for us
		}
	else
		{
		err = mDNSSendDNSMessage(m, &m->omsg, ptr, mDNSInterface_Any, &rr->UpdateServer, rr->UpdatePort, mDNSNULL, GetAuthInfoForName_internal(m, rr->resrec.name));
		if (err) debugf("ERROR: sendRecordRegistration - mDNSSendDNSMessage - %ld", err);
		SetRecordRetry(m, rr, err);
		}

	if (rr->state != regState_Refresh && rr->state != regState_DeregDeferred && rr->state != regState_UpdatePending)
		rr->state = regState_Pending;

	err = mStatus_NoError;

exit:

	if (err)
		{
		LogMsg("sendRecordRegistration: Error formatting message %d", err);

		if (rr->state != regState_Unregistered)
			{
			UnlinkAuthRecord(m, rr);
			rr->state = regState_Unregistered;
			}

		mDNS_DropLockBeforeCallback();
		if (rr->RecordCallback)
			rr->RecordCallback(m, rr, err);
		mDNS_ReclaimLockAfterCallback();
		// CAUTION: MUST NOT do anything more with rr after calling rr->Callback(), because the client's callback function
		// is allowed to do anything, including starting/stopping queries, registering/deregistering records, etc.
		}
	}

// Called with lock held
mDNSlocal void hndlServiceUpdateReply(mDNS *const m, ServiceRecordSet *srs,  mStatus err)
	{
	mDNSBool InvokeCallback = mDNSfalse;
	ExtraResourceRecord **e = &srs->Extras;
	AuthRecord *txt = &srs->RR_TXT;

	if (m->mDNS_busy != m->mDNS_reentrancy+1)
		LogMsg("hndlServiceUpdateReply: Lock not held! mDNS_busy (%ld) mDNS_reentrancy (%ld)", m->mDNS_busy, m->mDNS_reentrancy);

	switch (srs->state)
		{
		case regState_Pending:
			if (err == mStatus_NameConflict && !srs->TestForSelfConflict)
				{
				srs->TestForSelfConflict = mDNStrue;
				debugf("checking for self-conflict of service %##s", srs->RR_SRV.resrec.name->c);
				SendServiceRegistration(m, srs);
				return;
				}
			else if (srs->TestForSelfConflict)
				{
				srs->TestForSelfConflict = mDNSfalse;
				if (err == mStatus_NoSuchRecord) err = mStatus_NameConflict;	// NoSuchRecord implies that our prereq was not met, so we actually have a name conflict
				if (err) srs->state = regState_Unregistered;
				else srs->state = regState_Registered;
				InvokeCallback = mDNStrue;
				break;
				}
			else if (srs->srs_uselease && err == mStatus_UnknownErr && mDNSSameIPPort(srs->SRSUpdatePort, UnicastDNSPort))
				{
				LogMsg("Re-trying update of service %##s without lease option", srs->RR_SRV.resrec.name->c);
				srs->srs_uselease = mDNSfalse;
				SendServiceRegistration(m, srs);
				return;
				}
			else
				{
				//!!!KRS make sure all structs will still get cleaned up when client calls DeregisterService with this state
				if (err) { LogMsg("Error %ld for registration of service %##s", err, srs->RR_SRV.resrec.name->c); srs->state = regState_Unregistered; }
				else srs->state = regState_Registered;
				InvokeCallback = mDNStrue;
				break;
				}
		case regState_Refresh:
			if (err)
				{
				LogMsg("Error %ld for refresh of service %##s", err, srs->RR_SRV.resrec.name->c);
				InvokeCallback = mDNStrue;
				srs->state = regState_Unregistered;
				}
			else srs->state = regState_Registered;
			break;
		case regState_DeregPending:
			if (err) LogMsg("Error %ld for deregistration of service %##s", err, srs->RR_SRV.resrec.name->c);
			if (srs->SRVChanged)
				{
				srs->state = regState_NoTarget;	// NoTarget will allow us to pick up new target OR nat traversal state
				break;
				}
			err = mStatus_MemFree;
			InvokeCallback = mDNStrue;
			if (srs->NATinfo.clientContext)
				{
				// deletion completed
				mDNS_StopNATOperation_internal(m, &srs->NATinfo);
				srs->NATinfo.clientContext = mDNSNULL;
				}
			srs->state = regState_Unregistered;
			break;
		case regState_DeregDeferred:
			if (err)
				{
				debugf("Error %ld received prior to deferred derigstration of %##s", err, srs->RR_SRV.resrec.name->c);
				err = mStatus_MemFree;
				InvokeCallback = mDNStrue;
				srs->state = regState_Unregistered;
				break;
				}
			else
				{
				debugf("Performing deferred deregistration of %##s", srs->RR_SRV.resrec.name->c);
				srs->state = regState_Registered;
				SendServiceDeregistration(m, srs);
				return;
				}
		case regState_UpdatePending:
			if (err)
				{
				LogMsg("hndlServiceUpdateReply: error updating TXT record for service %##s", srs->RR_SRV.resrec.name->c);
				srs->state = regState_Unregistered;
				InvokeCallback = mDNStrue;
				}
			else
				{
				srs->state = regState_Registered;
				// deallocate old RData
				if (txt->UpdateCallback) txt->UpdateCallback(m, txt, txt->OrigRData);
				SetNewRData(&txt->resrec, txt->InFlightRData, txt->InFlightRDLen);
				txt->OrigRData = mDNSNULL;
				txt->InFlightRData = mDNSNULL;
				}
			break;
		case regState_FetchingZoneData:
		case regState_Registered:
		case regState_Unregistered:
		case regState_NATMap:
		case regState_NoTarget:
		case regState_ExtraQueued:
		case regState_NATError:
			LogMsg("hndlServiceUpdateReply called for service %##s in unexpected state %d with error %ld.  Unlinking.",
				   srs->RR_SRV.resrec.name->c, srs->state, err);
			err = mStatus_UnknownErr;
		default: LogMsg("hndlServiceUpdateReply: Unknown state %d for %##s", srs->state, srs->RR_SRV.resrec.name->c);
		}

	if ((srs->SRVChanged || srs->SRVUpdateDeferred) && (srs->state == regState_NoTarget || srs->state == regState_Registered))
		{
		if (InvokeCallback)
			{
			srs->ClientCallbackDeferred = mDNStrue;
			srs->DeferredStatus = err;
			}
		srs->SRVChanged = mDNSfalse;
		UpdateSRV(m, srs);
		return;
		}

	while (*e)
		{
		if ((*e)->r.state == regState_ExtraQueued)
			{
			if (srs->state == regState_Registered && !err)
				{
				// extra resource record queued for this service - copy zone srs and register
				AssignDomainName(&(*e)->r.zone, &srs->zone);
				(*e)->r.UpdateServer    = srs->ns;
				(*e)->r.UpdatePort  = srs->SRSUpdatePort;
				(*e)->r.uselease = srs->srs_uselease;
				sendRecordRegistration(m, &(*e)->r);
				e = &(*e)->next;
				}
			else if (err && (*e)->r.state != regState_Unregistered)
				{
				// unlink extra from list
				(*e)->r.state = regState_Unregistered;
				*e = (*e)->next;
				}
			else e = &(*e)->next;
			}
		else e = &(*e)->next;
		}

	srs->RR_SRV.ThisAPInterval = INIT_UCAST_POLL_INTERVAL - 1;	// reset retry delay for future refreshes, dereg, etc.
	if (srs->state == regState_Unregistered) unlinkSRS(m, srs);
	else if (txt->QueuedRData && srs->state == regState_Registered)
		{
		if (InvokeCallback)
			{
			// if we were supposed to give a client callback, we'll do it after we update the primary txt record
			srs->ClientCallbackDeferred = mDNStrue;
			srs->DeferredStatus = err;
			}
		srs->state = regState_UpdatePending;
		txt->InFlightRData = txt->QueuedRData;
		txt->InFlightRDLen = txt->QueuedRDLen;
		txt->OrigRData = txt->resrec.rdata;
		txt->OrigRDLen = txt->resrec.rdlength;
		txt->QueuedRData = mDNSNULL;
		SendServiceRegistration(m, srs);
		return;
		}

	mDNS_DropLockBeforeCallback();
	if (InvokeCallback)
		srs->ServiceCallback(m, srs, err);
	else if (srs->ClientCallbackDeferred)
		{
		srs->ClientCallbackDeferred = mDNSfalse;
		srs->ServiceCallback(m, srs, srs->DeferredStatus);
		}
	mDNS_ReclaimLockAfterCallback();
	// CAUTION: MUST NOT do anything more with rr after calling rr->Callback(), because the client's callback function
	// is allowed to do anything, including starting/stopping queries, registering/deregistering records, etc.
	}

// Called with lock held
mDNSlocal void hndlRecordUpdateReply(mDNS *m, AuthRecord *rr, mStatus err)
	{
	mDNSBool InvokeCallback = mDNStrue;

	if (m->mDNS_busy != m->mDNS_reentrancy+1)
		LogMsg("hndlRecordUpdateReply: Lock not held! mDNS_busy (%ld) mDNS_reentrancy (%ld)", m->mDNS_busy, m->mDNS_reentrancy);

	if (rr->state == regState_UpdatePending)
		{
		if (err)
			{
			LogMsg("Update record failed for %##s (err %d)", rr->resrec.name->c, err);
			rr->state = regState_Unregistered;
			}
		else
			{
			debugf("Update record %##s - success", rr->resrec.name->c);
			rr->state = regState_Registered;
			// deallocate old RData
			if (rr->UpdateCallback) rr->UpdateCallback(m, rr, rr->OrigRData);
			SetNewRData(&rr->resrec, rr->InFlightRData, rr->InFlightRDLen);
			rr->OrigRData = mDNSNULL;
			rr->InFlightRData = mDNSNULL;
			}
		}

	if (rr->state == regState_DeregPending)
		{
		debugf("Received reply for deregister record %##s type %d", rr->resrec.name->c, rr->resrec.rrtype);
		if (err) LogMsg("ERROR: Deregistration of record %##s type %d failed with error %ld",
						rr->resrec.name->c, rr->resrec.rrtype, err);
		err = mStatus_MemFree;
		rr->state = regState_Unregistered;
		}

	if (rr->state == regState_DeregDeferred)
		{
		if (err)
			{
			LogMsg("Cancelling deferred deregistration record %##s type %d due to registration error %ld",
				   rr->resrec.name->c, rr->resrec.rrtype, err);
			rr->state = regState_Unregistered;
			}
		debugf("Calling deferred deregistration of record %##s type %d",  rr->resrec.name->c, rr->resrec.rrtype);
		rr->state = regState_Registered;
		mDNS_Deregister_internal(m, rr, mDNS_Dereg_normal);
		return;
		}

	if (rr->state == regState_Pending || rr->state == regState_Refresh)
		{
		if (!err)
			{
			rr->state = regState_Registered;
			if (rr->state == regState_Refresh) InvokeCallback = mDNSfalse;
			}
		else
			{
			if (rr->uselease && err == mStatus_UnknownErr && mDNSSameIPPort(rr->UpdatePort, UnicastDNSPort))
				{
				LogMsg("Re-trying update of record %##s without lease option", rr->resrec.name->c);
				rr->uselease = mDNSfalse;
				sendRecordRegistration(m, rr);
				return;
				}
			LogMsg("Registration of record %##s type %d failed with error %ld", rr->resrec.name->c, rr->resrec.rrtype, err);
			rr->state = regState_Unregistered;
			}
		}

	if (rr->state == regState_Unregistered) UnlinkAuthRecord(m, rr);
	else rr->ThisAPInterval = INIT_UCAST_POLL_INTERVAL - 1;	// reset retry delay for future refreshes, dereg, etc.

	if (rr->QueuedRData && rr->state == regState_Registered)
		{
		rr->state = regState_UpdatePending;
		rr->InFlightRData = rr->QueuedRData;
		rr->InFlightRDLen = rr->QueuedRDLen;
		rr->OrigRData = rr->resrec.rdata;
		rr->OrigRDLen = rr->resrec.rdlength;
		rr->QueuedRData = mDNSNULL;
		sendRecordRegistration(m, rr);
		return;
		}

	if (InvokeCallback && rr->RecordCallback)
		{
		mDNS_DropLockBeforeCallback();
		rr->RecordCallback(m, rr, err);
		mDNS_ReclaimLockAfterCallback();
		}
	// CAUTION: MUST NOT do anything more with rr after calling rr->Callback(), because the client's callback function
	// is allowed to do anything, including starting/stopping queries, registering/deregistering records, etc.
	}

mDNSexport void uDNS_ReceiveNATPMPPacket(mDNS *m, mDNSu8 *pkt, mDNSu16 len)
	{
	NATTraversalInfo *ptr;
	NATAddrReply    *AddrReply    = (NATAddrReply    *)pkt;
	NATPortMapReply *PortMapReply = (NATPortMapReply *)pkt;

	if (!AddrReply->err && len < 8) { LogMsg("NAT Traversal message too short (%d bytes)", len); return; }
	if (AddrReply->vers != NATMAP_VERS) { LogMsg("Received NAT Traversal response with version %d (expected %d)", pkt[0], NATMAP_VERS); return; }

	// Byte-swap the multi-byte numerics
	AddrReply->err    = (mDNSu16) (                                                (mDNSu16)pkt[2] << 8 | pkt[3]);
	AddrReply->uptime = (mDNSs32) ((mDNSs32)pkt[4] << 24 | (mDNSs32)pkt[5] << 16 | (mDNSs32)pkt[6] << 8 | pkt[7]);

	if (AddrReply->opcode == NATOp_AddrResponse)
		{
		if (!AddrReply->err && len < sizeof(NATAddrReply))    { LogMsg("NAT Traversal AddrResponse message too short (%d bytes)", len); return; }
		natTraversalHandleAddressReply(m, pkt);
		}
	else if (AddrReply->opcode & (NATOp_MapUDPResponse | NATOp_MapTCPResponse))
		{
		if (!PortMapReply->err)
			{
			if (len < sizeof(NATPortMapReply)) { LogMsg("NAT Traversal PortMapReply message too short (%d bytes)", len); return; }
			PortMapReply->NATRep_lease = (mDNSs32) ((mDNSs32)pkt[12] << 24 | (mDNSs32)pkt[13] << 16 | (mDNSs32)pkt[14] << 8 | pkt[15]);
			}

		for (ptr = m->NATTraversals; ptr; ptr=ptr->next) natTraversalHandlePortMapReply(ptr, m, pkt);
		}
	else { LogMsg("Received NAT Traversal response with version unknown opcode 0x%X", AddrReply->opcode); return; }
	}

#ifdef _LEGACY_NAT_TRAVERSAL_
mDNSexport void uDNS_ReceiveSSDPPacket(mDNS *m, mDNSu8 *data, mDNSu16 len)
	{
	// Extract router's port and url from response if we don't already have it, otherwise ignore
	if (mDNSIPPortIsZero(m->uPNPRouterPort)) LNT_ConfigureRouterInfo(m, data, len);
	}
#endif // _LEGACY_NAT_TRAVERSAL_

// <rdar://problem/3925163> Shorten DNS-SD queries to avoid NAT bugs
// <rdar://problem/4288449> Add check to avoid crashing NAT gateways that have buggy DNS relay code
//
// We know of bugs in home NAT gateways that cause them to crash if they receive certain DNS queries.
// The DNS queries that make them crash are perfectly legal DNS queries, but even if they weren't,
// the gateway shouldn't crash -- in today's world of viruses and network attacks, software has to
// be written assuming that a malicious attacker could send them any packet, properly-formed or not.
// Still, we don't want to be crashing people's home gateways, so we go out of our way to avoid
// the queries that crash them.
//
// Some examples:
//
// 1. Any query where the name ends in ".in-addr.arpa." and the text before this is 32 or more bytes.
//    The query type does not need to be PTR -- the gateway will crash for any query type.
//    e.g. "ping long-name-crashes-the-buggy-router.in-addr.arpa" will crash one of these.
//
// 2. Any query that results in a large response with the TC bit set.
//
// 3. Any PTR query that doesn't begin with four decimal numbers.
//    These gateways appear to assume that the only possible PTR query is a reverse-mapping query
//    (e.g. "1.0.168.192.in-addr.arpa") and if they ever get a PTR query where the first four
//    labels are not all decimal numbers in the range 0-255, they handle that by crashing.
//    These gateways also ignore the remainder of the name following the four decimal numbers
//    -- whether or not it actually says in-addr.arpa, they just make up an answer anyway.
//
// The challenge therefore is to craft a query that will discern whether the DNS server
// is one of these buggy ones, without crashing it. Furthermore we don't want our test
// queries making it all the way to the root name servers, putting extra load on those
// name servers and giving Apple a bad reputation. To this end we send this query:
//     dig -t ptr 1.0.0.127.dnsbugtest.1.0.0.127.in-addr.arpa.
//
// The text preceding the ".in-addr.arpa." is under 32 bytes, so it won't cause crash (1).
// It will not yield a large response with the TC bit set, so it won't cause crash (2).
// It starts with four decimal numbers, so it won't cause crash (3).
// The name falls within the "1.0.0.127.in-addr.arpa." domain, the reverse-mapping name for the local
// loopback address, and therefore the query will black-hole at the first properly-configured DNS server
// it reaches, making it highly unlikely that this query will make it all the way to the root.
//
// Finally, the correct response to this query is NXDOMAIN or a similar error, but the
// gateways that ignore the remainder of the name following the four decimal numbers
// give themselves away by actually returning a result for this nonsense query.

mDNSlocal const domainname *DNSRelayTestQuestion = (const domainname*)
	"\x1" "1" "\x1" "0" "\x1" "0" "\x3" "127" "\xa" "dnsbugtest"
	"\x1" "1" "\x1" "0" "\x1" "0" "\x3" "127" "\x7" "in-addr" "\x4" "arpa";

// Returns mDNStrue if response was handled
mDNSlocal mDNSBool uDNS_ReceiveTestQuestionResponse(mDNS *const m, DNSMessage *const msg, const mDNSu8 *const end,
	const mDNSAddr *const srcaddr, const mDNSIPPort srcport)
	{
	const mDNSu8 *ptr = msg->data;
	DNSQuestion q;
	DNSServer *s;
	mDNSu32 result = 0;

	// 1. Find out if this is an answer to one of our test questions
	if (msg->h.numQuestions != 1) return(mDNSfalse);
	ptr = getQuestion(msg, ptr, end, mDNSInterface_Any, &q);
	if (!ptr) return(mDNSfalse);
	if (q.qtype != kDNSType_PTR || q.qclass != kDNSClass_IN) return(mDNSfalse);
	if (!SameDomainName(&q.qname, DNSRelayTestQuestion)) return(mDNSfalse);

	// 2. If the DNS relay gave us a positive response, then it's got buggy firmware
	// else, if the DNS relay gave us an error or no-answer response, it passed our test
	if ((msg->h.flags.b[1] & kDNSFlag1_RC) == kDNSFlag1_RC_NoErr && msg->h.numAnswers > 0)
		result = DNSServer_Failed;
	else
		result = DNSServer_Passed;

	// 3. Find occurrences of this server in our list, and mark them appropriately
	for (s = m->DNSServers; s; s = s->next)
		if (mDNSSameAddress(srcaddr, &s->addr) && mDNSSameIPPort(srcport, s->port) && s->teststate != result)
			{
			DNSQuestion *q;
			if (s->teststate != result)
				{
				s->teststate = result;
				if (result == DNSServer_Passed) LogOperation("DNS Server %#a:%d passed", srcaddr, mDNSVal16(srcport));
				else LogMsg("NOTE: Wide-Area Service Discovery disabled to avoid crashing defective DNS relay %#a:%d", srcaddr, mDNSVal16(srcport));
				}
			if (result == DNSServer_Passed)		// Unblock any questions that were waiting for this result
				for (q = m->Questions; q; q=q->next)
					if (q->qDNSServer == s)
						{ q->LastQTime = m->timenow - q->ThisQInterval; m->NextScheduledQuery = m->timenow; }
			}

	return(mDNStrue); // Return mDNStrue to tell uDNS_ReceiveMsg it doesn't need to process this packet further
	}

// Called from mDNSCoreReceive with the lock held
mDNSexport void uDNS_ReceiveMsg(mDNS *const m, DNSMessage *const msg, const mDNSu8 *const end, const mDNSAddr *const srcaddr, const mDNSIPPort srcport)
	{
	DNSQuestion *qptr;
	mStatus err = mStatus_NoError;

	mDNSu8 StdR    = kDNSFlag0_QR_Response | kDNSFlag0_OP_StdQuery;
	mDNSu8 UpdateR = kDNSFlag0_QR_Response | kDNSFlag0_OP_Update;
	mDNSu8 QR_OP   = (mDNSu8)(msg->h.flags.b[0] & kDNSFlag0_QROP_Mask);
	mDNSu8 rcode   = (mDNSu8)(msg->h.flags.b[1] & kDNSFlag1_RC);

	(void)srcport; // Unused

	debugf("uDNS_ReceiveMsg from %#-15a with "
		"%2d Question%s %2d Answer%s %2d Authorit%s %2d Additional%s",
		srcaddr,
		msg->h.numQuestions,   msg->h.numQuestions   == 1 ? ", " : "s,",
		msg->h.numAnswers,     msg->h.numAnswers     == 1 ? ", " : "s,",
		msg->h.numAuthorities, msg->h.numAuthorities == 1 ? "y,  " : "ies,",
		msg->h.numAdditionals, msg->h.numAdditionals == 1 ? "" : "s");

	if (QR_OP == StdR)
		{
		//if (srcaddr && recvLLQResponse(m, msg, end, srcaddr, srcport)) return;
		if (uDNS_ReceiveTestQuestionResponse(m, msg, end, srcaddr, srcport)) return;
		if (!mDNSOpaque16IsZero(msg->h.id))
			for (qptr = m->Questions; qptr; qptr = qptr->next)
				if (msg->h.flags.b[0] & kDNSFlag0_TC && mDNSSameOpaque16(qptr->TargetQID, msg->h.id) && m->timenow - qptr->LastQTime < RESPONSE_WINDOW)
					{
					if (!srcaddr) LogMsg("uDNS_ReceiveMsg: TCP DNS response had TC bit set: ignoring");
					else if (qptr->tcp)
						{
						// There may be a race condition here, if the server decides to drop the connection just as we decide to reuse it
						// For now it should not be serious because our normal retry logic (as used to handle UDP packet loss)
						// should take care of it but later we may want to look at handling this case explicitly
						LogOperation("uDNS_ReceiveMsg: Using existing TCP connection for %##s (%s)", qptr->qname.c, DNSTypeName(qptr->qtype));
						mDNS_DropLockBeforeCallback();
						tcpCallback(qptr->tcp->sock, qptr->tcp, mDNStrue, mStatus_NoError);
						mDNS_ReclaimLockAfterCallback();
						}
					else qptr->tcp = MakeTCPConn(m, mDNSNULL, mDNSNULL, kTCPSocketFlags_Zero, srcaddr, srcport, qptr, mDNSNULL, mDNSNULL);
					}
		}

	if (QR_OP == UpdateR && !mDNSOpaque16IsZero(msg->h.id))
		{
		mDNSu32 lease = GetPktLease(m, msg, end);
		mDNSs32 expire = (m->timenow + (((mDNSs32)lease * mDNSPlatformOneSecond)) * 3/4);

		if (CurrentServiceRecordSet)
			LogMsg("uDNS_ReceiveMsg ERROR CurrentServiceRecordSet already set");
		CurrentServiceRecordSet = m->ServiceRegistrations;

		while (CurrentServiceRecordSet)
			{
			ServiceRecordSet *sptr = CurrentServiceRecordSet;
			CurrentServiceRecordSet = CurrentServiceRecordSet->uDNS_next;

			if (mDNSSameOpaque16(sptr->id, msg->h.id))
				{
				err = checkUpdateResult(m, sptr->RR_SRV.resrec.name, rcode, msg, end);
				if (!err && sptr->srs_uselease && lease)
					if (sptr->expire - expire >= 0 || sptr->state != regState_UpdatePending)
						sptr->expire = expire;
				hndlServiceUpdateReply(m, sptr, err);
				CurrentServiceRecordSet = mDNSNULL;
				return;
				}
			}

		if (m->CurrentRecord)
			LogMsg("uDNS_ReceiveMsg ERROR m->CurrentRecord already set %s", ARDisplayString(m, m->CurrentRecord));
		m->CurrentRecord = m->ResourceRecords;
		while (m->CurrentRecord)
			{
			AuthRecord *rptr = m->CurrentRecord;
			m->CurrentRecord = m->CurrentRecord->next;
			if (mDNSSameOpaque16(rptr->id, msg->h.id))
				{
				err = checkUpdateResult(m, rptr->resrec.name, rcode, msg, end);
				if (!err && rptr->uselease && lease)
					if (rptr->expire - expire >= 0 || rptr->state != regState_UpdatePending)
						rptr->expire = expire;
				hndlRecordUpdateReply(m, rptr, err);
				m->CurrentRecord = mDNSNULL;
				return;
				}
			}
		}
	debugf("Received unexpected response: ID %d matches no active records", mDNSVal16(msg->h.id));
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Query Routines
#endif

mDNSlocal void sendLLQRefresh(mDNS *m, DNSQuestion *q, mDNSu32 lease)
	{
	mDNSu8 *end;
	LLQOptData llq;
	mStatus err;

	// If this is supposed to be a private question and the server dropped the TCP connection,
	// we don't want to cancel it with a clear-text UDP packet, and and it's not worth the expense of
	// setting up a new TLS session just to cancel the outstanding LLQ, so we just let it expire naturally
	if (lease == 0 && q->AuthInfo && !q->tcp) return;

	if ((q->state == LLQ_Refresh && q->ntries >= kLLQ_MAX_TRIES) || q->expire - m->timenow < 0)
		{
		LogMsg("Unable to refresh LLQ %##s - will retry in %d minutes", q->qname.c, kLLQ_DEF_RETRY/60);
		q->state         = LLQ_Retry;
		q->LastQTime     = m->timenow;
		q->ThisQInterval = kLLQ_DEF_RETRY * mDNSPlatformOneSecond;
		return;
		//!!!KRS handle this - periodically try to re-establish
		}

	llq.vers  = kLLQ_Vers;
	llq.llqOp = kLLQOp_Refresh;
	llq.err   = LLQErr_NoError;
	llq.id    = q->id;
	llq.llqlease = lease;

	InitializeDNSMessage(&m->omsg.h, q->TargetQID, uQueryFlags);
	end = putLLQ(&m->omsg, m->omsg.data, q, &llq, mDNStrue);
	if (!end) { LogMsg("ERROR: sendLLQRefresh - putLLQ"); return; }

	err = mDNSSendDNSMessage(m, &m->omsg, end, mDNSInterface_Any, &q->servAddr, q->servPort, q->tcp ? q->tcp->sock : mDNSNULL, q->AuthInfo);
	if (err) debugf("ERROR: sendLLQRefresh - mDNSSendDNSMessage returned %ld", err);

	if (q->state == LLQ_Established) q->ntries = 1;
	else q->ntries++;
	q->state = LLQ_Refresh;
	q->LastQTime = m->timenow;
	}

// wrapper for startLLQHandshake, invoked by async op callback
mDNSexport void startLLQHandshakeCallback(mDNS *const m, mStatus err, const ZoneData *zoneInfo)
	{
	DNSQuestion *q = (DNSQuestion *)zoneInfo->ZoneDataContext;

	// If we get here it means that the GetZoneData operation has completed, and is is about to cancel
	// its question and free the ZoneData memory. We no longer need to hold onto our pointer (which
	// we use for cleaning up if our LLQ is cancelled *before* the GetZoneData operation has completes).
	q->nta = mDNSNULL;

	// check state first to make sure it is OK to touch question object
	if (q->state == LLQ_Cancelled)
		{
		// StopQuery was called while we were getting the zone info
		debugf("startLLQHandshakeCallback - LLQ Cancelled.");
		return;
		}

	if (q->state != LLQ_GetZoneInfo)
		{
		LogMsg("ERROR: startLLQHandshakeCallback - bad state %d", q->state);
		err = mStatus_UnknownErr;
		goto exit;
		}

	if (err)
		{
		LogMsg("ERROR: startLLQHandshakeCallback %##s (%s) invoked with error code %ld", q->qname.c, DNSTypeName(q->qtype), err);
		StartLLQPolling(m, q);
		err = mStatus_NoError;
		goto exit;
		}

	if (!zoneInfo)
		{
		LogMsg("ERROR: startLLQHandshakeCallback invoked with NULL result and no error code");
		err = mStatus_UnknownErr;
		goto exit;
		}

	if (mDNSIPPortIsZero(zoneInfo->Port))
		{
		LogOperation("LLQ port lookup failed - reverting to polling for %##s (%s)", q->qname.c, DNSTypeName(q->qtype));
		q->servPort = zeroIPPort;
		StartLLQPolling(m, q);
		goto exit;
		}

	// cache necessary zone data
	q->servAddr  = zoneInfo->Addr;
	q->servPort  = zoneInfo->Port;
	if (!zoneInfo->ZonePrivate) q->AuthInfo = mDNSNULL;

	q->ntries = 0;

	if (q->state == LLQ_SuspendDeferred) q->state = LLQ_Suspended;
	else startLLQHandshake(m, q);

exit:

	if (err && q) q->state = LLQ_Error;
	}

// Called in normal callback context (i.e. mDNS_busy and mDNS_reentrancy are both 1)
mDNSlocal void startPrivateQueryCallback(mDNS *const m, mStatus err, const ZoneData *zoneInfo)
	{
	DNSQuestion *q = (DNSQuestion *) zoneInfo->ZoneDataContext;

	LogOperation("startPrivateQueryCallback %##s (%s)", q->qname.c, DNSTypeName(q->qtype));

	// If we get here it means that the GetZoneData operation has completed, and is is about to cancel
	// its question and free the ZoneData memory. We no longer need to hold onto our pointer (which
	// we use for cleaning up if our LLQ is cancelled *before* the GetZoneData operation has completes).
	q->nta = mDNSNULL;

	if (err)
		{
		LogMsg("ERROR: startPrivateQueryCallback %##s (%s) invoked with error code %ld", q->qname.c, DNSTypeName(q->qtype), err);
		goto exit;
		}

	if (!zoneInfo)
		{
		LogMsg("ERROR: startPrivateQueryCallback invoked with NULL result and no error code");
		err = mStatus_UnknownErr;
		goto exit;
		}

	if (!zoneInfo->ZonePrivate)
		{
		debugf("Private port lookup failed -- retrying without TLS -- %##s (%s)", q->qname.c, DNSTypeName(q->qtype));
		q->AuthInfo      = mDNSNULL;		// Clear AuthInfo so we try again non-private
		q->ThisQInterval = InitialQuestionInterval;
		q->LastQTime     = m->timenow - q->ThisQInterval;
		SetNextQueryTime(m, q);
		goto exit;
		// Next call to uDNS_CheckCurrentQuestion() will do this as a non-private query
		}

	if (!q->AuthInfo)
		{
		LogMsg("ERROR: startPrivateQueryCallback: cannot find credentials for q %##s (%s)", q->qname.c, DNSTypeName(q->qtype));
		err = mStatus_UnknownErr;
		goto exit;
		}

	q->TargetQID = mDNS_NewMessageID(m);
	if (q->tcp)
		{
		LogOperation("startPrivateQueryCallback: Already have TCP connection for %##s (%s)", q->qname.c, DNSTypeName(q->qtype));
		tcpCallback(q->tcp->sock, q->tcp, mDNStrue, mStatus_NoError);
		}
	else q->tcp = MakeTCPConn(m, mDNSNULL, mDNSNULL, kTCPSocketFlags_UseTLS, &zoneInfo->Addr, zoneInfo->Port, q, mDNSNULL, mDNSNULL);

exit:

	if (err) mDNS_StopQuery(m, q);
	}

// uDNS_StopLongLivedQuery happens IN ADDITION to stopQuery
mDNSexport void uDNS_StopLongLivedQuery(mDNS *const m, DNSQuestion *const question)
	{
	(void)m;	// unused

	LogOperation("uDNS_StopLongLivedQuery %##s (%s)", question->qname.c, DNSTypeName(question->qtype));

	if (!question->LongLived) { LogMsg("ERROR: uDNS_StopLongLivedQuery - LongLived flag not set"); return; }

	switch (question->state)
		{
		case LLQ_UnInit:
			LogMsg("ERROR: uDNS_StopLongLivedQuery - state LLQ_UnInit");
			//!!!KRS should we unlink info<->question here?
			return;
		case LLQ_GetZoneInfo:
		case LLQ_SuspendDeferred:
			question->state = LLQ_Cancelled;
			return;
		case LLQ_Established:
		case LLQ_Refresh:
			// refresh w/ lease 0
			sendLLQRefresh(m, question, 0);
			goto end;
		default:
			debugf("uDNS_StopLongLivedQuery - silently discarding LLQ in state %d", question->state);
			goto end;
		}

	end:

	RemoveLLQNatMappings(m, question);
	CheckForUnreferencedLLQMapping(m);
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Dynamic Updates
#endif

// Called in normal callback context (i.e. mDNS_busy and mDNS_reentrancy are both 1)
mDNSexport void RecordRegistrationCallback(mDNS *const m, mStatus err, const ZoneData *zoneData)
	{
	AuthRecord *newRR = (AuthRecord*)zoneData->ZoneDataContext;
	AuthRecord *ptr;

	if (m->mDNS_busy != m->mDNS_reentrancy)
		LogMsg("RecordRegistrationCallback: mDNS_busy (%ld) != mDNS_reentrancy (%ld)", m->mDNS_busy, m->mDNS_reentrancy);

	newRR->nta = mDNSNULL;

	// make sure record is still in list (!!!)
	for (ptr = m->ResourceRecords; ptr; ptr = ptr->next) if (ptr == newRR) break;
	if (!ptr) { LogMsg("RecordRegistrationCallback - RR no longer in list.  Discarding."); return; }

	// check error/result
	if (err) { LogMsg("RecordRegistrationCallback: error %ld", err); goto error; }
	if (!zoneData) { LogMsg("ERROR: RecordRegistrationCallback invoked with NULL result and no error"); goto error; }

	if (newRR->resrec.rrclass != zoneData->ZoneClass)
		{
		LogMsg("ERROR: New resource record's class (%d) does not match zone class (%d)",
			   newRR->resrec.rrclass, zoneData->ZoneClass);
		goto error;
		}

	// Don't try to do updates to the root name server.
	// We might be tempted also to block updates to any single-label name server (e.g. com, edu, net, etc.) but some
	// organizations use their own private pseudo-TLD, like ".home", etc, and we don't want to block that.
	if (zoneData->ZoneName.c[0] == 0)
		{
		LogMsg("RecordRegistrationCallback: Only name server claiming responsibility for \"%##s\" is \"%##s\"!", newRR->resrec.name->c, zoneData->ZoneName.c);
		err = mStatus_NoSuchNameErr;
		goto error;
		}

	// Store discovered zone data
	AssignDomainName(&newRR->zone, &zoneData->ZoneName);
	newRR->UpdateServer = zoneData->Addr;
	newRR->UpdatePort   = zoneData->Port;
	newRR->Private      = zoneData->ZonePrivate;
	debugf("RecordRegistrationCallback: Set newRR->UpdateServer %##s %##s to %#a:%d",
		newRR->resrec.name->c, zoneData->ZoneName.c, &newRR->UpdateServer, mDNSVal16(newRR->UpdatePort));

	if (mDNSIPPortIsZero(zoneData->Port) || mDNSAddressIsZero(&zoneData->Addr))
		{
		LogMsg("RecordRegistrationCallback: No _dns-update._udp service found for \"%##s\"!", newRR->resrec.name->c);
		err = mStatus_NoSuchNameErr;
		goto error;
		}


	mDNS_Lock(m);	// sendRecordRegistration expects to be called with the lock held
	sendRecordRegistration(m, newRR);
	mDNS_Unlock(m);
	return;

error:
	if (newRR->state != regState_Unregistered)
		{
		UnlinkAuthRecord(m, newRR);
		newRR->state = regState_Unregistered;
		}

	// Don't need to do the mDNS_DropLockBeforeCallback stuff here, because this code is
	// *already* being invoked in the right callback context, with mDNS_reentrancy correctly incremented.
	if (newRR->RecordCallback)
		newRR->RecordCallback(m, newRR, err);
	// CAUTION: MUST NOT do anything more with rr after calling rr->Callback(), because the client's callback function
	// is allowed to do anything, including starting/stopping queries, registering/deregistering records, etc.
	}

mDNSlocal void SendRecordDeregistration(mDNS *m, AuthRecord *rr)
	{
	mDNSu8 *ptr = m->omsg.data;
	mDNSu8 *end = (mDNSu8 *)&m->omsg + sizeof(DNSMessage);
	mStatus err = mStatus_NoError;

	InitializeDNSMessage(&m->omsg.h, rr->id, UpdateReqFlags);

	ptr = putZone(&m->omsg, ptr, end, &rr->zone, mDNSOpaque16fromIntVal(rr->resrec.rrclass));
	if (!ptr) { err = mStatus_UnknownErr; goto exit; }
	if (!(ptr = putDeletionRecord(&m->omsg, ptr, &rr->resrec))) { err = mStatus_UnknownErr; goto exit; }

	rr->state = regState_DeregPending;

	if (rr->Private)
		{
		LogOperation("SendRecordDeregistration TCP %p", rr->tcp);
		if (rr->tcp) LogMsg("SendRecordDeregistration: Already have TCP connection for %s", ARDisplayString(m, rr));
		else rr->tcp = MakeTCPConn(m, &m->omsg, ptr, kTCPSocketFlags_UseTLS, &rr->UpdateServer, rr->UpdatePort, mDNSNULL, mDNSNULL, rr);
		rr->LastAPTime = m->timenow;
		rr->ThisAPInterval = 0x3FFFFFFF; // TCP will handle any necessary retransmissions for us
		}
	else
		{
		err = mDNSSendDNSMessage(m, &m->omsg, ptr, mDNSInterface_Any, &rr->UpdateServer, rr->UpdatePort, mDNSNULL, GetAuthInfoForName_internal(m, rr->resrec.name));
		if (err) debugf("ERROR: SendRecordDeregistration - mDNSSendDNSMessage - %ld", err);
		SetRecordRetry(m, rr, err);
		}

	err = mStatus_NoError;

exit:

	if (err)
		{
		LogMsg("Error: SendRecordDeregistration - could not contruct deregistration packet");
		UnlinkAuthRecord(m, rr);
		rr->state = regState_Unregistered;
		}
	}

mDNSexport mStatus uDNS_DeregisterRecord(mDNS *const m, AuthRecord *const rr)
	{
	switch (rr->state)
		{
		case regState_NATMap:        LogMsg("regState_NATMap        %##s type %s", rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype)); return mStatus_NoError;
		case regState_ExtraQueued: rr->state = regState_Unregistered; break;
		case regState_Refresh:
		case regState_Pending:
		case regState_UpdatePending:
			rr->state = regState_DeregDeferred;
			LogMsg("Deferring deregistration of record %##s until registration completes", rr->resrec.name->c);
			return mStatus_NoError;
		case regState_FetchingZoneData:
		case regState_Registered: break;
		case regState_DeregPending: break;
		case regState_DeregDeferred: LogMsg("regState_DeregDeferred %##s type %s", rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype)); return mStatus_NoError;
		case regState_Unregistered:  LogMsg("regState_Unregistered  %##s type %s", rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype)); return mStatus_NoError;
		case regState_NATError:      LogMsg("regState_NATError      %##s type %s", rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype)); return mStatus_NoError;
		case regState_NoTarget:      LogMsg("regState_NoTarget      %##s type %s", rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype)); return mStatus_NoError;
		default: LogMsg("uDNS_DeregisterRecord: State %d for %##s type %s", rr->state, rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype)); return mStatus_NoError;
		}

	if (rr->state != regState_Unregistered) SendRecordDeregistration(m, rr);
	return mStatus_NoError;
	}

// Called with lock held
mDNSexport mStatus uDNS_DeregisterService(mDNS *const m, ServiceRecordSet *srs)
	{
	char *errmsg = "Unknown State";

	if (m->mDNS_busy != m->mDNS_reentrancy+1)
		LogMsg("uDNS_DeregisterService: Lock not held! mDNS_busy (%ld) mDNS_reentrancy (%ld)", m->mDNS_busy, m->mDNS_reentrancy);

	// don't re-register with a new target following deregistration
	srs->SRVChanged = srs->SRVUpdateDeferred = mDNSfalse;

	if (srs->nta) { CancelGetZoneData(m, srs->nta); srs->nta = mDNSNULL; }

	if (srs->NATinfo.clientContext)
		{
		mDNS_StopNATOperation_internal(m, &srs->NATinfo);
		srs->NATinfo.clientContext = mDNSNULL;
		}

	switch (srs->state)
		{
		case regState_Unregistered:
			debugf("uDNS_DeregisterService - service %##s not registered", srs->RR_SRV.resrec.name->c);
			return mStatus_BadReferenceErr;
		case regState_Pending:
		case regState_Refresh:
		case regState_UpdatePending:
			// deregister following completion of in-flight operation
			srs->state = regState_DeregDeferred;
			return mStatus_NoError;
		case regState_DeregPending:
		case regState_DeregDeferred:
			debugf("Double deregistration of service %##s", srs->RR_SRV.resrec.name->c);
			return mStatus_NoError;
		case regState_NATError:	// not registered
		case regState_NATMap:	// not registered
		case regState_NoTarget:	// not registered
			unlinkSRS(m, srs);
			srs->state = regState_Unregistered;
			mDNS_DropLockBeforeCallback();
			srs->ServiceCallback(m, srs, mStatus_MemFree);
			mDNS_ReclaimLockAfterCallback();
			return mStatus_NoError;
		case regState_FetchingZoneData:
		case regState_Registered:
			srs->state = regState_DeregPending;
			SendServiceDeregistration(m, srs);
			return mStatus_NoError;
		case regState_ExtraQueued: // only for record registrations
			errmsg = "bad state (regState_ExtraQueued)";
			goto error;
		default: LogMsg("uDNS_DeregisterService: Unknown state %d for %##s", srs->state, srs->RR_SRV.resrec.name->c);
		}

	error:
	LogMsg("Error, uDNS_DeregisterService: %s", errmsg);
	return mStatus_BadReferenceErr;
	}

mDNSexport mStatus uDNS_UpdateRecord(mDNS *m, AuthRecord *rr)
	{
	ServiceRecordSet *parent = mDNSNULL;
	AuthRecord *rptr;
	regState_t *stateptr = mDNSNULL;

	// find the record in registered service list
	for (parent = m->ServiceRegistrations; parent; parent = parent->uDNS_next)
		if (&parent->RR_TXT == rr) { stateptr = &parent->state; break; }

	if (!parent)
		{
		// record not part of a service - check individual record registrations
		for (rptr = m->ResourceRecords; rptr; rptr = rptr->next)
			if (rptr == rr) { stateptr = &rr->state; break; }
		if (!rptr) goto unreg_error;
		}

	switch(*stateptr)
		{
		case regState_DeregPending:
		case regState_DeregDeferred:
		case regState_Unregistered:
			// not actively registered
			goto unreg_error;

		case regState_FetchingZoneData:
		case regState_NATMap:
		case regState_ExtraQueued:
		case regState_NoTarget:
			// change rdata directly since it hasn't been sent yet
			if (rr->UpdateCallback) rr->UpdateCallback(m, rr, rr->resrec.rdata);
			SetNewRData(&rr->resrec, rr->NewRData, rr->newrdlength);
			rr->NewRData = mDNSNULL;
			return mStatus_NoError;

		case regState_Pending:
		case regState_Refresh:
		case regState_UpdatePending:
			// registration in-flight. queue rdata and return
			if (rr->QueuedRData && rr->UpdateCallback)
				// if unsent rdata is already queued, free it before we replace it
				rr->UpdateCallback(m, rr, rr->QueuedRData);
			rr->QueuedRData = rr->NewRData;
			rr->QueuedRDLen = rr->newrdlength;
			rr->NewRData = mDNSNULL;
			return mStatus_NoError;

		case regState_Registered:
			rr->OrigRData = rr->resrec.rdata;
			rr->OrigRDLen = rr->resrec.rdlength;
			rr->InFlightRData = rr->NewRData;
			rr->InFlightRDLen = rr->newrdlength;
			rr->NewRData = mDNSNULL;
			*stateptr = regState_UpdatePending;
			if (parent) SendServiceRegistration(m, parent);
			else sendRecordRegistration(m, rr);
			return mStatus_NoError;

		case regState_NATError:
			LogMsg("ERROR: uDNS_UpdateRecord called for record %##s with bad state regState_NATError", rr->resrec.name->c);
			return mStatus_UnknownErr;	// states for service records only

		default: LogMsg("uDNS_UpdateRecord: Unknown state %d for %##s", *stateptr, rr->resrec.name->c);
		}

	unreg_error:
	LogMsg("Requested update of record %##s type %d, part of service not currently registered",
		   rr->resrec.name->c, rr->resrec.rrtype);
	return mStatus_Invalid;
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Periodic Execution Routines
#endif

// See comments above for DNSRelayTestQuestion
// If this is the kind of query that has the risk of crashing buggy DNS servers, we do a test question first
mDNSlocal mDNSBool NoTestQuery(DNSQuestion *q)
	{
	int i;
	mDNSu8 *p = q->qname.c;
	if (q->AuthInfo) return(mDNStrue);		// Don't need a test query for private queries sent directly to authoritative server over TLS/TCP
	if (q->qtype != kDNSType_PTR) return(mDNStrue);		// Don't need a test query for any non-PTR queries
	for (i=0; i<4; i++)		// If qname does not begin with num.num.num.num, can't skip the test query
		{
		if (p[0] < 1 || p[0] > 3) return(mDNSfalse);
		if (              p[1] < '0' || p[1] > '9' ) return(mDNSfalse);
		if (p[0] >= 2 && (p[2] < '0' || p[2] > '9')) return(mDNSfalse);
		if (p[0] >= 3 && (p[3] < '0' || p[3] > '9')) return(mDNSfalse);
		p += 1 + p[0];
		}
	// If remainder of qname is ".in-addr.arpa.", this is a vanilla reverse-mapping query and
	// we can safely do it without needing a test query first, otherwise we need the test query.
	return(SameDomainName((domainname*)p, (const domainname*)"\x7" "in-addr" "\x4" "arpa"));
	}

// The question to be checked is not passed in as an explicit parameter;
// instead it is implicit that the question to be checked is m->CurrentQuestion.
mDNSexport void uDNS_CheckCurrentQuestion(mDNS *const m)
	{
	DNSQuestion *q = m->CurrentQuestion;
	mDNSs32 sendtime = q->LastQTime + q->ThisQInterval;
	// Don't allow sendtime to be earlier than SuppressStdPort53Queries
	if (!q->LongLived && m->SuppressStdPort53Queries && sendtime - m->SuppressStdPort53Queries < 0)
		sendtime = m->SuppressStdPort53Queries;
	if (m->timenow - sendtime < 0) return;

	if (q->LongLived && q->state != LLQ_Poll)
		{
		if (q->state >= LLQ_InitialRequest && q->state <= LLQ_Established)
			{
			// sanity check to avoid packet flood bugs
			if      (q->state == LLQ_Established || q->state == LLQ_Refresh) sendLLQRefresh(m, q, q->origLease);
			else if (q->state == LLQ_InitialRequest                        ) startLLQHandshake(m, q);
			else if (q->state == LLQ_SecondaryRequest                      ) sendChallengeResponse(m, q, mDNSNULL);
			else if (q->state == LLQ_Retry                                 ) { q->ntries = 0; startLLQHandshake(m, q); }
			}
		else
			{
			// This should never happen. Any LLQ not in states LLQ_InitialRequest to LLQ_Established should not have have ThisQInterval set.
			// (uDNS_CheckCurrentQuestion() is only called for DNSQuestions with non-zero ThisQInterval)
			LogMsg("uDNS_CheckCurrentQuestion: %##s (%s) state %d sendtime %d ThisQInterval %d",
				q->qname.c, DNSTypeName(q->qtype), q->state, sendtime - m->timenow, q->ThisQInterval);
			q->LastQTime = m->timenow;
			q->ThisQInterval *= 2;
			}
		}

	// We repeat the check above (rather than just making this the "else" case) because startLLQHandshake can change q->state to LLQ_Poll
	if (!(q->LongLived && q->state != LLQ_Poll))
		{
		if (q->qDNSServer && q->qDNSServer->teststate != DNSServer_Disabled)
			{
			mDNSu8 *end = m->omsg.data;
			mStatus err = mStatus_NoError;
			DomainAuthInfo *private = mDNSNULL;

			if (q->qDNSServer->teststate != DNSServer_Untested || NoTestQuery(q))
				{
				err = constructQueryMsg(&m->omsg, &end, q);
				private = q->AuthInfo;
				}
			else if (m->timenow - q->qDNSServer->lasttest >= INIT_UCAST_POLL_INTERVAL)	// Make sure at least three seconds has elapsed since last test query
				{
				LogOperation("Sending DNS test query to %#a:%d", &q->qDNSServer->addr, mDNSVal16(q->qDNSServer->port));
				q->ThisQInterval = INIT_UCAST_POLL_INTERVAL;
				q->qDNSServer->lasttest = m->timenow;
				InitializeDNSMessage(&m->omsg.h, mDNS_NewMessageID(m), uQueryFlags);
				end = putQuestion(&m->omsg, m->omsg.data, m->omsg.data + AbsoluteMaxDNSMessageData, DNSRelayTestQuestion, kDNSType_PTR, kDNSClass_IN);
				}

			if (err) LogMsg("Error: uDNS_CheckCurrentQuestion - constructQueryMsg. Skipping question %##s (%s)", q->qname.c, DNSTypeName(q->qtype));
			else
				{
				if (end > m->omsg.data && (q->qDNSServer->teststate != DNSServer_Failed || NoTestQuery(q)))
					{
					//LogMsg("uDNS_CheckCurrentQuestion %d %p %##s (%s)", sendtime - m->timenow, private, q->qname.c, DNSTypeName(q->qtype));
					if (private)
						{
						if (q->nta) LogMsg("uDNS_CheckCurrentQuestion Error: GetZoneData already started for %##s (%s)", q->qname.c, DNSTypeName(q->qtype));
						else q->nta = StartGetZoneData(m, &q->qname, q->LongLived ? ZoneServiceLLQ : ZoneServiceQuery, startPrivateQueryCallback, q);
						q->ThisQInterval = 0;		// Suspend this question until GetZoneData completes
						}
					else
						{
						err = mDNSSendDNSMessage(m, &m->omsg, end, q->qDNSServer->interface, &q->qDNSServer->addr, q->qDNSServer->port, mDNSNULL, mDNSNULL);
						m->SuppressStdPort53Queries = NonZeroTime(m->timenow + (mDNSPlatformOneSecond+99)/100);
						}
					}

				if (err) debugf("ERROR: uDNS_idle - mDNSSendDNSMessage - %ld", err); // surpress syslog messages if we have no network
				else if (!q->LongLived && q->ThisQInterval < MAX_UCAST_POLL_INTERVAL)
					{
					q->ThisQInterval = q->ThisQInterval * QuestionIntervalStep;	// Only increase interval if send succeeded
					//LogMsg("Adjusted ThisQInterval to %d for %##s (%s)", q->ThisQInterval, q->qname.c, DNSTypeName(q->qtype));
					}
				else if (q->LongLived && q->state == LLQ_Poll)
					{
					// Bit of a hack here -- if we dropped the interval down to do the DNS test query, need to put
					// it back or we'll poll every three seconds. The real solution is that the DNS test query
					// should be a real query in its own right, which other queries are blocked on, rather than
					// being shoehorned in here and borrowing another question's q->LastQTime and q->ThisQInterval
					q->ThisQInterval = LLQ_POLL_INTERVAL;
					}
				}
			q->LastQTime = m->timenow;
			}
		else
			{
			// If we have no server for this query, or the only server is a disabled one, then we deliver a
			// transient failure indication to the client. This is important for things like iPhone where we
			// want to return timely feedback to the user when no network is available. Note that although we
			// use MakeNegativeCacheRecord() here, we don't actually store it in the cache --
			// we just deliver it to the client with a no-cache ADD, and then discard it.
			if (!q->qDNSServer) LogMsg("uDNS_CheckCurrentQuestion no DNS server for %##s", q->qname.c);
			else LogMsg("uDNS_CheckCurrentQuestion DNS server %#a:%d for %##s is disabled", &q->qDNSServer->addr, mDNSVal16(q->qDNSServer->port), q->qname.c);
			MakeNegativeCacheRecord(m, &q->qname, q->qnamehash, q->qtype, q->qclass);
			// Inactivate this question until the next change of DNS servers (do this before AnswerCurrentQuestionWithResourceRecord)
			q->ThisQInterval = 0;
			AnswerCurrentQuestionWithResourceRecord(m, &m->rec.r, QC_addnocache);
			m->rec.r.resrec.RecordType = 0;		// Clear RecordType to show we're not still using it
			// MUST NOT touch m->CurrentQuestion (or q) after this -- client callback could have deleted it
			}
		}
	}

mDNSlocal void CheckNATMappings(mDNS *m)
	{
	mStatus err = mStatus_NoError;
	mDNSBool rfc1918 = mDNSv4AddrIsRFC1918(&m->AdvertisedV4.ip.v4);
	m->NextScheduledNATOp = m->timenow + 0x3FFFFFFF;

	if (m->NATTraversals)
		{
		// open NAT-PMP socket to receive multicasts from router
		if (rfc1918)
			{
			// if we are behind a NAT and the socket hasn't been opened yet, open it
			if (m->NATMcastRecvskt == mDNSNULL)  
				if((m->NATMcastRecvskt = mDNSPlatformUDPSocket(m, NATPMPPort)) == mDNSNULL) LogMsg("Can't allocate UDP multicast socket");  // log error and continue
			}
		else if (m->NATMcastRecvskt != mDNSNULL)	
			{ 
			// if we are NOT behind a NAT and this socket is open, close it
			mDNSPlatformUDPClose(m->NATMcastRecvskt); 
			m->NATMcastRecvskt = mDNSNULL; 
			}

		if (m->timenow - m->retryGetAddr >= 0)	// we have exceeded the timer interval
			{
			err = uDNS_SendNATMsg(m, mDNSNULL, AddrRequestFlag);
			if (!err)
				{
				if      (m->retryIntervalGetAddr     < NATMAP_INIT_RETRY)         m->retryIntervalGetAddr = NATMAP_INIT_RETRY;
				else if (m->retryIntervalGetAddr * 2 > NATMAP_MAX_RETRY_INTERVAL) m->retryIntervalGetAddr = NATMAP_MAX_RETRY_INTERVAL;
				else m->retryIntervalGetAddr *= 2;
				}
			// Always update m->retryGetAddr, even if we fail to send the packet. Otherwise in cases where we can't send the packet
			// (like when we have no active interfaces) we'll spin in an infinite loop repeatedly failing to send the packet
			m->retryGetAddr = m->timenow + m->retryIntervalGetAddr;
			}
	
		if (m->NextScheduledNATOp - m->retryGetAddr > 0)
			m->NextScheduledNATOp = m->retryGetAddr;
		}

	if (m->CurrentNATTraversal) LogMsg("WARNING m->CurrentNATTraversal already in use");
	m->CurrentNATTraversal = m->NATTraversals;
	while (m->CurrentNATTraversal)
		{
		NATTraversalInfo *cur = m->CurrentNATTraversal;
		m->CurrentNATTraversal = m->CurrentNATTraversal->next;

		if (!(mDNSIPv4AddressIsZero(m->AdvertisedV4.ip.v4)) && !rfc1918)
			{
			cur->Error      = mStatus_NoError;
			cur->publicPort = cur->privatePort;
			m->ExternalAddress = m->AdvertisedV4.ip.v4;
			}
		else
			{
			// check for any outstanding request or refresh
			if (cur->opFlags & (MapTCPFlag | MapUDPFlag))
				{
				if (m->timenow - cur->retryPortMap >= 0)		// we have exceeded the timer interval
					{
					if (!mDNSIPPortIsZero(cur->publicPort) && cur->ExpiryTime - m->timenow < 0)	// Mapping has expired
						{
						cur->publicPort = zeroIPPort;
						cur->retryIntervalPortMap = NATMAP_INIT_RETRY;
						}
					
					if (!mDNSIPPortIsZero(cur->publicPort))		// We have a mapping; time to refresh it
						{
						err = uDNS_SendNATMsg(m, cur, (cur->opFlags & MapTCPFlag) ? MapTCPFlag : MapUDPFlag);
						if (!err)	// only decrement if we are successful
							{
							// attempt to refresh with decreasing retry interval
							cur->retryIntervalPortMap = (cur->ExpiryTime - m->timenow) / 2;
							if (cur->retryIntervalPortMap < NATMAP_MIN_RETRY_INTERVAL)
								cur->retryIntervalPortMap = NATMAP_MIN_RETRY_INTERVAL;
							}
						}
					else										// no mapping yet; trying to get one
						{
						err = uDNS_SendNATMsg(m, cur, (cur->opFlags & MapTCPFlag) ? MapTCPFlag : MapUDPFlag);
						if (!err)
							{
							if      (cur->retryIntervalPortMap     < NATMAP_INIT_RETRY        ) cur->retryIntervalPortMap = NATMAP_INIT_RETRY;
							else if (cur->retryIntervalPortMap * 2 < NATMAP_MAX_RETRY_INTERVAL) cur->retryIntervalPortMap *= 2;
							else                                                                cur->retryIntervalPortMap = NATMAP_MAX_RETRY_INTERVAL;
							}
						}
					cur->retryPortMap = m->timenow + cur->retryIntervalPortMap;
					}
	
				if (m->NextScheduledNATOp - cur->retryPortMap > 0)
					m->NextScheduledNATOp = cur->retryPortMap;
				}
			}

		// Notify the client if necessary
		// XXX: this check for the callback should not be here, but it is because internal clients are not doing the start/stop nat operation correctly...
			if (!mDNSIPv4AddressIsZero(m->ExternalAddress) || m->retryIntervalGetAddr > NATMAP_INIT_RETRY*2 ||	// if the address is not zero or we have tried getting the address
				!mDNSIPPortIsZero(cur->publicPort) || cur->retryIntervalPortMap > NATMAP_INIT_RETRY*2)			// if the port is not zero or we have tried getting a port mapping
				if ((!(cur->opFlags & (MapUDPFlag | MapTCPFlag)) && !mDNSSameIPv4Address(m->ExternalAddress, cur->lastExternalAddress)) ||	// if all we want is an address and the address changed
					(mDNSIPv4AddressIsZero(m->ExternalAddress) && mDNSIPPortIsZero(cur->publicPort) && !mDNSSameIPPort(cur->publicPort, cur->lastPublicPort)) || // if address and port are zero and the port changed
					(!mDNSIPPortIsZero(cur->publicPort) && !mDNSSameIPPort(cur->publicPort, cur->lastPublicPort)) || 						// or the port is not zero and has changed
					cur->Error != cur->lastError)																							// or a new error occurred
					{
					cur->lastExternalAddress = m->ExternalAddress;
					cur->lastPublicPort      = cur->publicPort;
					cur->lastError           = cur->Error;
					mDNS_DropLockBeforeCallback();		// Allow client to legally make mDNS API calls from the callback
					if (cur->clientCallback)
						cur->clientCallback(m, m->ExternalAddress, cur, cur->Error);
					mDNS_ReclaimLockAfterCallback();	// Decrement mDNS_reentrancy to block mDNS API calls again
					// MUST NOT touch cur after invoking the callback
					}
		}
	}

mDNSlocal mDNSs32 CheckRecordRegistrations(mDNS *m)
	{
	AuthRecord *rr;
 	mDNSs32 nextevent = m->timenow + 0x3FFFFFFF;

	for (rr = m->ResourceRecords; rr; rr = rr->next)
		{
		if (rr->state == regState_Pending || rr->state == regState_DeregPending || rr->state == regState_UpdatePending || rr->state == regState_DeregDeferred || rr->state == regState_Refresh)
			{
			if (rr->LastAPTime + rr->ThisAPInterval - m->timenow < 0)
				{
				if (rr->tcp) { rr->LastAPTime = m->timenow; rr->ThisAPInterval = 0x3FFFFFFF; }
				else if (rr->state == regState_DeregPending) SendRecordDeregistration(m, rr);
				else sendRecordRegistration(m, rr);
				}
			if (rr->LastAPTime + rr->ThisAPInterval - nextevent < 0) nextevent = rr->LastAPTime + rr->ThisAPInterval;
			}
		if (rr->uselease && rr->state == regState_Registered)
			{
			if (rr->expire - m->timenow < 0)
				{
				debugf("refreshing record %##s", rr->resrec.name->c);
				rr->state = regState_Refresh;
				sendRecordRegistration(m, rr);
				}
			if (rr->expire - nextevent < 0) nextevent = rr->expire;
			}
		}
	return nextevent;
	}

mDNSlocal mDNSs32 CheckServiceRegistrations(mDNS *m)
	{
	mDNSs32 nextevent = m->timenow + 0x3FFFFFFF;

	if (CurrentServiceRecordSet)
		LogMsg("CheckServiceRegistrations ERROR CurrentServiceRecordSet already set");
	CurrentServiceRecordSet = m->ServiceRegistrations;

	// Note: ServiceRegistrations list is in the order they were created; important for in-order event delivery
	while (CurrentServiceRecordSet)
		{
		ServiceRecordSet *srs = CurrentServiceRecordSet;
		CurrentServiceRecordSet = CurrentServiceRecordSet->uDNS_next;
		if (srs->state == regState_Pending || srs->state == regState_DeregPending || srs->state == regState_DeregDeferred || srs->state == regState_Refresh || srs->state == regState_UpdatePending)
			{
			if (srs->RR_SRV.LastAPTime + srs->RR_SRV.ThisAPInterval - m->timenow < 0)
				{
				if (srs->tcp) { srs->RR_SRV.LastAPTime = m->timenow; srs->RR_SRV.ThisAPInterval = 0x3FFFFFFF; }
				else if (srs->state == regState_DeregPending) { SendServiceDeregistration(m, srs); continue; }
				else SendServiceRegistration(m, srs);
				}
			if (nextevent - srs->RR_SRV.LastAPTime + srs->RR_SRV.ThisAPInterval > 0)
				nextevent = srs->RR_SRV.LastAPTime + srs->RR_SRV.ThisAPInterval;
			}

		if (srs->srs_uselease && srs->state == regState_Registered)
			{
			if (srs->expire - m->timenow < 0)
				{
				debugf("refreshing service %##s", srs->RR_SRV.resrec.name->c);
				srs->state = regState_Refresh;
				SendServiceRegistration(m, srs);
				}
			if (srs->expire - nextevent < 0) nextevent = srs->expire;
			}
		}
	return nextevent;
	}

mDNSexport void uDNS_Execute(mDNS *const m)
	{
	mDNSs32 nexte;

	m->NextuDNSEvent = m->timenow + 0x3FFFFFFF;

	if (m->NextSRVUpdate && m->NextSRVUpdate - m->timenow < 0)
		{ m->NextSRVUpdate = 0; UpdateSRVRecords(m); }

	CheckNATMappings(m);

	if (m->SuppressStdPort53Queries && m->timenow - m->SuppressStdPort53Queries >= 0)
		m->SuppressStdPort53Queries = 0; // If suppression time has passed, clear it

	nexte = CheckRecordRegistrations(m);
	if (nexte - m->NextuDNSEvent < 0) m->NextuDNSEvent = nexte;

	nexte = CheckServiceRegistrations(m);
	if (nexte - m->NextuDNSEvent < 0) m->NextuDNSEvent = nexte;
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Startup, Shutdown, and Sleep
#endif

// DeregisterActive causes active LLQs to be removed from the server, e.g. before sleep. Pass false
// following a location change, as the server will reject deletions from a source address different
// from the address on which the LLQ was created.

mDNSlocal void SuspendLLQs(mDNS *m, mDNSBool DeregisterActive)
	{
	DNSQuestion *q;

	for (q = m->Questions; q; q = q->next)
		{
		if (q->LongLived)
			{
			if (q->state == LLQ_GetZoneInfo)
				{
				debugf("Marking %##s suspend-deferred", q->qname.c);
				q->state = LLQ_SuspendDeferred;	// suspend once we're done getting zone info
				}
			else if (q->state < LLQ_Suspended)
				{
				if (DeregisterActive && (q->state == LLQ_Established || q->state == LLQ_Refresh))
					{
					debugf("Deleting LLQ %##s", q->qname.c);
					sendLLQRefresh(m, q, 0);
					}
				debugf("Marking %##s suspended", q->qname.c);
				q->state = LLQ_Suspended;

				if (q->tcp) { DisposeTCPConn(q->tcp); q->tcp = mDNSNULL; }

				q->id = zeroOpaque64;
				}
			else if (q->state == LLQ_Poll)
				{
				debugf("Marking %##s suspended-poll", q->qname.c);
				q->state = LLQ_SuspendedPoll;
				}

			RemoveLLQNatMappings(m, q); // may not need nat mapping if we restart with new route
			}
		}
	CheckForUnreferencedLLQMapping(m);
	}

mDNSlocal void RestartQueries(mDNS *m)
	{
	if (m->CurrentQuestion)
		LogMsg("RestartQueries: ERROR m->CurrentQuestion already set: %##s (%s)", m->CurrentQuestion->qname.c, DNSTypeName(m->CurrentQuestion->qtype));
	m->CurrentQuestion = m->Questions;
	while (m->CurrentQuestion)
		{
		DNSQuestion *q = m->CurrentQuestion;
		m->CurrentQuestion = m->CurrentQuestion->next;

		if (!mDNSOpaque16IsZero(q->TargetQID) && !q->DuplicateOf)
			{
			if (q->LongLived)
				{
				if (q->state == LLQ_Suspended || q->state == LLQ_NatMapWaitUDP)
					{
					q->ntries = -1;
					startLLQHandshake(m, q);
					}
				else if (q->state == LLQ_SuspendDeferred)
					q->state = LLQ_GetZoneInfo; // we never finished getting zone data - proceed as usual
				else if (q->state == LLQ_SuspendedPoll)
					{
					// if we were polling, we may have had bad zone data due to firewall, etc. - refetch
					q->ntries = 0;
					q->state = LLQ_GetZoneInfo;
					if (q->nta) CancelGetZoneData(m, q->nta); // Make sure we cancel old one before we start a new one
					q->nta = StartGetZoneData(m, &q->qname, ZoneServiceLLQ, startLLQHandshakeCallback, q);
					}
				}
			else { q->LastQTime = m->timenow; q->ThisQInterval = INIT_UCAST_POLL_INTERVAL; } // trigger poll in 3 seconds (to reduce packet rate when restarts come in rapid succession)
			}
		}
	m->CurrentQuestion = mDNSNULL;
	}

mDNSexport void mDNS_UpdateLLQs(mDNS *m)
	{
	mDNS_Lock(m);
	SuspendLLQs(m, mDNStrue);
	RestartQueries(m);
	mDNS_Unlock(m);
	}

// simplest sleep logic - rather than having sleep states that must be dealt with explicitly in all parts of
// the code, we simply send a deregistration, and put the service in Refresh state, with a timeout far enough
// in the future that we'll sleep (or the sleep will be cancelled) before it is retransmitted. Then to wake,
// we just move up the timers.

mDNSlocal void SleepRecordRegistrations(mDNS *m)
	{
	AuthRecord *rr = m->ResourceRecords;

	while (rr)
		{
		if (rr->state == regState_Registered ||
			rr->state == regState_Refresh)
			{
			mDNSu8 *ptr = m->omsg.data, *end = (mDNSu8 *)&m->omsg + sizeof(DNSMessage);
			InitializeDNSMessage(&m->omsg.h, mDNS_NewMessageID(m), UpdateReqFlags);

			// construct deletion update
			ptr = putZone(&m->omsg, ptr, end, &rr->zone, mDNSOpaque16fromIntVal(rr->resrec.rrclass));
			if (!ptr) { LogMsg("Error: SleepRecordRegistrations - could not put zone"); return; }
			ptr = putDeletionRecord(&m->omsg, ptr, &rr->resrec);
			if (!ptr) { LogMsg("Error: SleepRecordRegistrations - could not put deletion record"); return; }

			mDNSSendDNSMessage(m, &m->omsg, ptr, mDNSInterface_Any, &rr->UpdateServer, rr->UpdatePort, mDNSNULL, GetAuthInfoForName_internal(m, rr->resrec.name));
			rr->state = regState_Refresh;
			rr->LastAPTime = m->timenow;
			rr->ThisAPInterval = 300 * mDNSPlatformOneSecond;
			}
		rr = rr->next;
		}
	}

mDNSlocal void WakeRecordRegistrations(mDNS *m)
	{
	AuthRecord *rr = m->ResourceRecords;

	while (rr)
		{
		if (rr->state == regState_Refresh)
			{
			// trigger slightly delayed refresh (we usually get this message before kernel is ready to send packets)
			rr->LastAPTime = m->timenow;
			rr->ThisAPInterval = INIT_UCAST_POLL_INTERVAL;
			}
		rr = rr->next;
		}
	}

mDNSlocal void SleepServiceRegistrations(mDNS *m)
	{
	ServiceRecordSet *srs = m->ServiceRegistrations;
	while (srs)
		{
		if (srs->nta) { CancelGetZoneData(m, srs->nta); srs->nta = mDNSNULL; }

		if (srs->NATinfo.clientContext)
			{
			mDNS_StopNATOperation_internal(m, &srs->NATinfo);
			srs->NATinfo.clientContext = mDNSNULL;
			}

		if (srs->state == regState_UpdatePending)
			{
			// act as if the update succeeded, since we're about to delete the name anyway
			AuthRecord *txt = &srs->RR_TXT;
			srs->state = regState_Registered;
			// deallocate old RData
			if (txt->UpdateCallback) txt->UpdateCallback(m, txt, txt->OrigRData);
			SetNewRData(&txt->resrec, txt->InFlightRData, txt->InFlightRDLen);
			txt->OrigRData = mDNSNULL;
			txt->InFlightRData = mDNSNULL;
			}

		if (srs->state == regState_Registered || srs->state == regState_Refresh)
			{
			mDNSOpaque16 origid = srs->id;
			srs->state = regState_DeregPending;	// state expected by SendDereg()
			SendServiceDeregistration(m, srs);
			srs->id = origid;
			srs->state = regState_NoTarget;	// when we wake, we'll re-register (and optionally nat-map) once our address record completes
			srs->RR_SRV.resrec.rdata->u.srv.target.c[0] = 0;
			}
		srs = srs->uDNS_next;
		}
	}

mDNSlocal void WakeServiceRegistrations(mDNS *m)
	{
	ServiceRecordSet *srs = m->ServiceRegistrations;
	while (srs)
		{
		if (srs->state == regState_Refresh)
			{
			// trigger slightly delayed refresh (we usually get this message before kernel is ready to send packets)
			srs->RR_SRV.LastAPTime = m->timenow;
			srs->RR_SRV.ThisAPInterval = INIT_UCAST_POLL_INTERVAL;
			}
		srs = srs->uDNS_next;
		}
	}

mDNSexport void mDNS_AddSearchDomain(const domainname *const domain)
	{
	SearchListElem **p;

	// Check to see if we already have this domain in our list
	for (p = &SearchList; *p; p = &(*p)->next)
		if (SameDomainName(&(*p)->domain, domain))
			{
			// If domain is already in list, and marked for deletion, change it to "leave alone"
			if ((*p)->flag == -1) (*p)->flag = 0;
			LogOperation("mDNS_AddSearchDomain already in list %##s", domain->c);
			return;
			}

	// if domain not in list, add to list, mark as add (1)
	*p = mDNSPlatformMemAllocate(sizeof(SearchListElem));
	if (!*p) { LogMsg("ERROR: mDNS_AddSearchDomain - malloc"); return; }
	mDNSPlatformMemZero(*p, sizeof(SearchListElem));
	AssignDomainName(&(*p)->domain, domain);
	(*p)->flag = 1;	// add
	(*p)->next = mDNSNULL;
	LogOperation("mDNS_AddSearchDomain created new %##s", domain->c);
	}

mDNSlocal void FreeARElemCallback(mDNS *const m, AuthRecord *const rr, mStatus result)
	{
	(void)m;	// unused
	if (result == mStatus_MemFree) mDNSPlatformMemFree(rr->RecordContext);
	}

mDNSlocal void FoundDomain(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, QC_result AddRecord)
	{
	SearchListElem *slElem = question->QuestionContext;
	mStatus err;

	if (answer->rrtype != kDNSType_PTR) return;
	if (answer->RecordType == kDNSRecordTypePacketNegative) return;

	if (AddRecord)
		{
		const char *name;
		ARListElem *arElem = mDNSPlatformMemAllocate(sizeof(ARListElem));
		if (!arElem) { LogMsg("ERROR: malloc"); return; }
		mDNS_SetupResourceRecord(&arElem->ar, mDNSNULL, mDNSInterface_LocalOnly, kDNSType_PTR, 7200, kDNSRecordTypeShared, FreeARElemCallback, arElem);
		if      (question == &slElem->BrowseQ)          name = mDNS_DomainTypeNames[mDNS_DomainTypeBrowse];
		else if (question == &slElem->DefBrowseQ)       name = mDNS_DomainTypeNames[mDNS_DomainTypeBrowseDefault];
		else if (question == &slElem->AutomaticBrowseQ) name = mDNS_DomainTypeNames[mDNS_DomainTypeBrowseAutomatic];
		else if (question == &slElem->RegisterQ)        name = mDNS_DomainTypeNames[mDNS_DomainTypeRegistration];
		else if (question == &slElem->DefRegisterQ)     name = mDNS_DomainTypeNames[mDNS_DomainTypeRegistrationDefault];
		else { LogMsg("FoundDomain - unknown question"); mDNSPlatformMemFree(arElem); return; }

		MakeDomainNameFromDNSNameString(&arElem->ar.namestorage, name);
		AppendDNSNameString            (&arElem->ar.namestorage, "local");
		AssignDomainName(&arElem->ar.resrec.rdata->u.name, &answer->rdata->u.name);
		err = mDNS_Register(m, &arElem->ar);
		if (err) { LogMsg("ERROR: FoundDomain - mDNS_Register returned %d", err); mDNSPlatformMemFree(arElem); return; }
		arElem->next = slElem->AuthRecs;
		slElem->AuthRecs = arElem;
		}
	else
		{
		ARListElem **ptr = &slElem->AuthRecs;
		while (*ptr)
			{
			if (SameDomainName(&(*ptr)->ar.resrec.rdata->u.name, &answer->rdata->u.name))
				{
				ARListElem *dereg = *ptr;
				*ptr = (*ptr)->next;
				debugf("Deregistering PTR %##s -> %##s", dereg->ar.resrec.name->c, dereg->ar.resrec.rdata->u.name.c);
				err = mDNS_Deregister(m, &dereg->ar);
				if (err) LogMsg("ERROR: FoundDomain - mDNS_Deregister returned %d", err);
				// Memory will be freed in the FreeARElemCallback
				}
			else
				ptr = &(*ptr)->next;
			}
		}
	}

#if APPLE_OSX_mDNSResponder && MACOSX_MDNS_MALLOC_DEBUGGING
mDNSexport void udns_validatelists(void *const v)
	{
	mDNS *const m = v;

	ServiceRecordSet *s;
	for (s = m->ServiceRegistrations; s; s=s->uDNS_next)
		if (s->uDNS_next == (ServiceRecordSet*)~0)
			LogMemCorruption("m->ServiceRegistrations: %p is garbage (%lX)", s, s->uDNS_next);

	NATTraversalInfo *n;
	for (n = m->NATTraversals; n; n=n->next)
		if (n->clientCallback == (NATTraversalClientCallback)~0)
			LogMemCorruption("m->NATTraversals: %p is garbage", n);

	DNSServer *d;
	for (d = m->DNSServers; d; d=d->next)
		if (d->teststate > DNSServer_Disabled)
			LogMemCorruption("m->DNSServers: %p is garbage (%d)", d, d->teststate);

	DomainAuthInfo *info;
	for (info = m->AuthInfoList; info; info = info->next)
		if (info->AutoTunnel == (mDNSBool)~0)
			LogMemCorruption("m->AuthInfoList: %p is garbage (%X)", info, info->AutoTunnel);

	HostnameInfo *hi;
	for (hi = m->Hostnames; hi; hi = hi->next)
		if (hi->StatusCallback == (mDNSRecordCallback*)~0)
			LogMemCorruption("m->Hostnames: %p is garbage", n);

	SearchListElem *ptr;
	for (ptr = SearchList; ptr; ptr = ptr->next)
		if (ptr->AuthRecs == (void*)~0)
			LogMemCorruption("SearchList: %p is garbage (%X)", ptr, ptr->AuthRecs);
	}
#endif

// This should probably move to the UDS daemon -- the concept of legacy clients and automatic registration / automatic browsing
// is really a UDS API issue, not something intrinsic to uDNS

mDNSexport mStatus uDNS_RegisterSearchDomains(mDNS *const m)
	{
	SearchListElem **p = &SearchList, *ptr;
	mStatus err;

	// step 1: mark each element for removal (-1)
	for (ptr = SearchList; ptr; ptr = ptr->next) ptr->flag = -1;

	// Client has requested domain enumeration or automatic browse -- time to make sure we have the search domains from the platform layer
	mDNS_Lock(m);
	m->RegisterSearchDomains = mDNStrue;
	mDNSPlatformSetDNSConfig(m, mDNSfalse, m->RegisterSearchDomains, mDNSNULL, mDNSNULL, mDNSNULL);
	mDNS_Unlock(m);

	// delete elems marked for removal, do queries for elems marked add
	while (*p)
		{
		ptr = *p;
		debugf("RegisterSearchDomains %d %p %##s", ptr->flag, ptr->AuthRecs, ptr->domain.c);
		if (ptr->flag == -1)	// remove
			{
			ARListElem *arList = ptr->AuthRecs;
			ptr->AuthRecs = mDNSNULL;
			*p = ptr->next;

			mDNS_StopGetDomains(m, &ptr->BrowseQ);
			mDNS_StopGetDomains(m, &ptr->RegisterQ);
			mDNS_StopGetDomains(m, &ptr->DefBrowseQ);
			mDNS_StopGetDomains(m, &ptr->DefRegisterQ);
			mDNS_StopGetDomains(m, &ptr->AutomaticBrowseQ);
			mDNSPlatformMemFree(ptr);

	        // deregister records generated from answers to the query
			while (arList)
				{
				ARListElem *dereg = arList;
				arList = arList->next;
				debugf("Deregistering PTR %##s -> %##s", dereg->ar.resrec.name->c, dereg->ar.resrec.rdata->u.name.c);
				err = mDNS_Deregister(m, &dereg->ar);
				if (err) LogMsg("ERROR: RegisterSearchDomains mDNS_Deregister returned %d", err);
				// Memory will be freed in the FreeARElemCallback
				}
			continue;
			}

		if (ptr->flag == 1)	// add
			{
			mStatus err1, err2, err3, err4, err5;
			err1 = mDNS_GetDomains(m, &ptr->BrowseQ,          mDNS_DomainTypeBrowse,              &ptr->domain, mDNSInterface_Any, FoundDomain, ptr);
			err2 = mDNS_GetDomains(m, &ptr->DefBrowseQ,       mDNS_DomainTypeBrowseDefault,       &ptr->domain, mDNSInterface_Any, FoundDomain, ptr);
			err3 = mDNS_GetDomains(m, &ptr->RegisterQ,        mDNS_DomainTypeRegistration,        &ptr->domain, mDNSInterface_Any, FoundDomain, ptr);
			err4 = mDNS_GetDomains(m, &ptr->DefRegisterQ,     mDNS_DomainTypeRegistrationDefault, &ptr->domain, mDNSInterface_Any, FoundDomain, ptr);
			err5 = mDNS_GetDomains(m, &ptr->AutomaticBrowseQ, mDNS_DomainTypeBrowseAutomatic,     &ptr->domain, mDNSInterface_Any, FoundDomain, ptr);
			if (err1 || err2 || err3 || err4 || err5)
				LogMsg("GetDomains for domain %##s returned error(s):\n"
					   "%d (mDNS_DomainTypeBrowse)\n"
					   "%d (mDNS_DomainTypeBrowseDefault)\n"
					   "%d (mDNS_DomainTypeRegistration)\n"
					   "%d (mDNS_DomainTypeRegistrationDefault)"
					   "%d (mDNS_DomainTypeBrowseAutomatic)\n",
					   ptr->domain.c, err1, err2, err3, err4, err5);
			ptr->flag = 0;
			}

		if (ptr->flag) { LogMsg("RegisterSearchDomains - unknown flag %d. Skipping.", ptr->flag); }

		p = &ptr->next;
		}

	return mStatus_NoError;
	}

// Construction of Default Browse domain list (i.e. when clients pass NULL) is as follows:
// 1) query for b._dns-sd._udp.local on LocalOnly interface
//    (.local manually generated via explicit callback)
// 2) for each search domain (from prefs pane), query for b._dns-sd._udp.<searchdomain>.
// 3) for each result from (2), register LocalOnly PTR record b._dns-sd._udp.local. -> <result>
// 4) result above should generate a callback from question in (1).  result added to global list
// 5) global list delivered to client via GetSearchDomainList()
// 6) client calls to enumerate domains now go over LocalOnly interface
//    (!!!KRS may add outgoing interface in addition)

mDNSexport void uDNS_Sleep(mDNS *const m)
	{
	SuspendLLQs(m, mDNStrue);
	SleepServiceRegistrations(m);
	SleepRecordRegistrations(m);
	}

mDNSexport void uDNS_Wake(mDNS *const m)
	{
	RestartQueries(m);
	WakeServiceRegistrations(m);
	WakeRecordRegistrations(m);
	}

struct CompileTimeAssertionChecks_uDNS
	{
	// Check our structures are reasonable sizes. Including overly-large buffers, or embedding
	// other overly-large structures instead of having a pointer to them, can inadvertently
	// cause structure sizes (and therefore memory usage) to balloon unreasonably.
	char sizecheck_tcpInfo_t     [(sizeof(tcpInfo_t)      <=  9100) ? 1 : -1];
	char sizecheck_SearchListElem[(sizeof(SearchListElem) <=  3800) ? 1 : -1];
	};
