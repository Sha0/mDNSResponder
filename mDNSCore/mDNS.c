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
 *
 * This code is completely 100% portable C. It does not depend on any external header files
 * from outside the mDNS project -- all the types it expects to find are defined right here.
 * 
 * The previous point is very important: This file does not depend on any external
 * header files. It should compile on *any* platform that has a C compiler, without
 * making *any* assumptions about availability of so-called "standard" C functions,
 * routines, or types (which may or may not be present on any given platform).

 * Formatting notes:
 * This code follows the "Whitesmiths style" C indentation rules. Plenty of discussion
 * on C indentation can be found on the web, such as <http://www.kafejo.com/komp/1tbs.htm>,
 * but for the sake of brevity here I will say just this: Curly braces are not syntactially
 * part of an "if" statement; they are the beginning and ending markers of a compound statement;
 * therefore common sense dictates that if they are part of a compound statement then they
 * should be indented to the same level as everything else in that compound statement.
 * Indenting curly braces at the same level as the "if" implies that curly braces are
 * part of the "if", which is false. (This is as misleading as people who write "char* x,y;"
 * thinking that variables x and y are both of type "char*" -- and anyone who doesn't
 * understand why variable y is not of type "char*" just proves the point that poor code
 * layout leads people to unfortunate misunderstandings about how the C language really works.)

    Change History (most recent first):

$Log: mDNS.c,v $
Revision 1.719  2007/09/27 22:23:56  cheshire
<rdar://problem/4947392> uDNS: Use SOA to determine TTL for negative answers
Need to clear m->rec.r.resrec.RecordType after we've finished using m->rec

Revision 1.718  2007/09/27 22:02:33  cheshire
<rdar://problem/5464941> BTMM: Registered records in BTMM don't get removed from server after calling RemoveRecord

Revision 1.717  2007/09/27 21:21:39  cheshire
Export CompleteDeregistration so it's callable from other files

Revision 1.716  2007/09/27 02:12:21  cheshire
Updated GrantCacheExtensions degugging message to show new record lifetime

Revision 1.715  2007/09/27 01:20:06  cheshire
<rdar://problem/5500077> BTMM: Need to refresh LLQs based on lease life and not TTL of response

Revision 1.714  2007/09/27 00:37:01  cheshire
<rdar://problem/4947392> BTMM: Use SOA to determine TTL for negative answers

Revision 1.713  2007/09/27 00:25:39  cheshire
Added ttl_seconds parameter to MakeNegativeCacheRecord in preparation for:
<rdar://problem/4947392> uDNS: Use SOA to determine TTL for negative answers

Revision 1.712  2007/09/26 23:16:58  cheshire
<rdar://problem/5496399> BTMM: Leopard sending excessive LLQ registration requests to .Mac

Revision 1.711  2007/09/26 22:06:02  cheshire
<rdar://problem/5507399> BTMM: No immediate failure notifications for BTMM names

Revision 1.710  2007/09/26 00:49:46  cheshire
Improve packet logging to show sent and received packets,
transport protocol (UDP/TCP/TLS) and source/destination address:port

Revision 1.709  2007/09/21 21:12:36  cheshire
<rdar://problem/5498009> BTMM: Need to log updates and query packet contents

Revision 1.708  2007/09/20 23:13:37  cheshire
<rdar://problem/4038277> BTMM: Not getting LLQ remove events when logging out of VPN or disconnecting from network
Additional fix: If we have no DNS servers at all, then immediately purge all unicast cache records (including for LLQs)

Revision 1.707  2007/09/20 02:29:37  cheshire
<rdar://problem/4038277> BTMM: Not getting LLQ remove events when logging out of VPN or disconnecting from network

Revision 1.706  2007/09/20 01:13:19  cheshire
Export CacheGroupForName so it's callable from other files

Revision 1.705  2007/09/20 01:12:06  cheshire
Moved HashSlot(X) from mDNS.c to DNSCommon.h so it's usable in other files

Revision 1.704  2007/09/19 22:47:25  cheshire
<rdar://problem/5490182> Memory corruption freeing a "no such service" service record

Revision 1.703  2007/09/14 01:46:59  cheshire
Fix Posix build (#ifdef _LEGACY_NAT_TRAVERSAL_ section included a closing curly brace it should not have)

Revision 1.702  2007/09/13 22:06:46  cheshire
<rdar://problem/5480643> Tully's Free WiFi: DNS fails
Need to accept DNS responses where the query ID field matches, even if the source address does not

Revision 1.701  2007/09/12 23:22:32  cheshire
<rdar://problem/5476979> Only accept NAT Port Mapping packets from our default gateway

Revision 1.700  2007/09/12 23:03:08  cheshire
<rdar://problem/5476978> DNSServiceNATPortMappingCreate callback not giving correct interface index

Revision 1.699  2007/09/12 22:19:28  cheshire
<rdar://problem/5476977> Need to listen for port 5350 NAT-PMP announcements

Revision 1.698  2007/09/12 22:13:27  cheshire
Remove DynDNSHostNames cleanly on shutdown

Revision 1.697  2007/09/12 01:44:47  cheshire
<rdar://problem/5475938> Eliminate "Correcting TTL" syslog messages for unicast DNS records

Revision 1.696  2007/09/12 01:26:08  cheshire
Initialize LastNATReplyLocalTime to timenow, so that gateway uptime checks work more reliably

Revision 1.695  2007/09/11 19:19:16  cheshire
Correct capitalization of "uPNP" to "UPnP"

Revision 1.694  2007/09/10 22:06:51  cheshire
Rename uptime => upseconds and LastNATUptime => LastNATupseconds to make it clear these time values are in seconds

Revision 1.693  2007/09/07 22:24:36  vazquez
<rdar://problem/5466301> Need to stop spewing mDNSResponderHelper logs

Revision 1.692  2007/09/07 00:12:09  cheshire
<rdar://problem/5466010> Unicast DNS changes broke efficiency fix 3928456

Revision 1.691  2007/09/05 22:25:01  vazquez
<rdar://problem/5400521> update_record mDNSResponder leak

Revision 1.690  2007/09/05 21:48:01  cheshire
<rdar://problem/5385864> BTMM: mDNSResponder flushes wide-area Bonjour records after an hour for a zone.
Now that we're respecting the TTL of uDNS records in the cache, the LLQ maintenance cod needs
to update the cache lifetimes of all relevant records every time it successfully renews an LLQ,
otherwise those records will expire and vanish from the cache.

Revision 1.689  2007/09/05 02:29:06  cheshire
<rdar://problem/5457287> mDNSResponder taking up 100% CPU in ReissueBlockedQuestions
Additional fixes to code implementing "NoAnswer" logic

Revision 1.688  2007/08/31 22:56:39  cheshire
<rdar://problem/5407080> BTMM: TTLs incorrect on cached BTMM records

Revision 1.687  2007/08/31 19:53:14  cheshire
<rdar://problem/5431151> BTMM: IPv6 address lookup should not succeed if autotunnel cannot be setup
If AutoTunnel setup fails, the code now generates a fake NXDomain error saying that the requested AAAA record does not exist

Revision 1.686  2007/08/30 00:01:56  cheshire
Added comment about SetTargetToHostName()

Revision 1.685  2007/08/29 01:19:24  cheshire
<rdar://problem/5400181> BTMM: Tunneled services do not need NAT port mappings
Set AutoTarget to Target_AutoHostAndNATMAP for non-AutoTunnel wide-area services

Revision 1.684  2007/08/28 23:58:42  cheshire
Rename HostTarget -> AutoTarget

Revision 1.683  2007/08/28 23:53:21  cheshire
Rename serviceRegistrationCallback -> ServiceRegistrationZoneDataComplete

Revision 1.682  2007/08/27 20:28:19  cheshire
Improve "suspect uDNS response" log message

Revision 1.681  2007/08/24 23:37:23  cheshire
Added debugging message to show when ExtraResourceRecord callback gets invoked

Revision 1.680  2007/08/24 00:15:19  cheshire
Renamed GetAuthInfoForName() to GetAuthInfoForName_internal() to make it clear that it may only be called with the lock held

Revision 1.679  2007/08/23 21:47:09  vazquez
<rdar://problem/5427316> BTMM: mDNSResponder sends NAT-PMP packets on public network
make sure we clean up port mappings on base stations by sending a lease value of 0,
and only send NAT-PMP packets on private networks; also save some memory by
not using packet structs in NATTraversals.

Revision 1.678  2007/08/01 16:09:13  cheshire
Removed unused NATTraversalInfo substructure from AuthRecord; reduced structure sizecheck values accordingly

Revision 1.677  2007/08/01 01:58:24  cheshire
Added RecordType sanity check in mDNS_Register_internal

Revision 1.676  2007/08/01 00:04:13  cheshire
<rdar://problem/5261696> Crash in tcpKQSocketCallback
Half-open TCP connections were not being cancelled properly

Revision 1.675  2007/07/31 02:28:35  vazquez
<rdar://problem/3734269> NAT-PMP: Detect public IP address changes and base station reboot

Revision 1.674  2007/07/31 01:57:23  cheshire
Adding code to respect TTL received in uDNS responses turned out to
expose other problems; backing out change for now.

Revision 1.673  2007/07/30 23:31:26  cheshire
Code for respecting TTL received in uDNS responses should exclude LLQ-type responses

Revision 1.672  2007/07/28 01:25:56  cheshire
<rdar://problem/4780038> BTMM: Add explicit UDP event port to LLQ setup request, to fix LLQs not working behind NAT

Revision 1.671  2007/07/27 22:32:54  cheshire
When processing TTLs in uDNS responses, we'll currently impose a minimum effective TTL
of 2 seconds, or other stuff breaks (e.g. we end up making a negative cache entry).

Revision 1.670  2007/07/27 20:54:43  cheshire
Fixed code to respect real record TTL received in uDNS responses

Revision 1.669  2007/07/27 20:09:32  cheshire
Don't need to dump out all received mDNS packets; they're easily viewed using mDNSNetMonitor

Revision 1.668  2007/07/27 19:58:47  cheshire
Use symbolic names QC_add and QC_rmv instead of mDNStrue/mDNSfalse

Revision 1.667  2007/07/27 19:52:10  cheshire
Don't increment m->rrcache_active for no-cache add events

Revision 1.666  2007/07/27 19:30:39  cheshire
Changed mDNSQuestionCallback parameter from mDNSBool to QC_result,
to properly reflect tri-state nature of the possible responses

Revision 1.665  2007/07/27 18:44:01  cheshire
Rename "AnswerQuestionWithResourceRecord" to more informative "AnswerCurrentQuestionWithResourceRecord"

Revision 1.664  2007/07/27 18:38:56  cheshire
Rename "uDNS_CheckQuery" to more informative "uDNS_CheckCurrentQuestion"

Revision 1.663  2007/07/25 03:05:02  vazquez
Fixes for:
<rdar://problem/5338913> LegacyNATTraversal: UPnP heap overflow
<rdar://problem/5338933> LegacyNATTraversal: UPnP stack buffer overflow
and a myriad of other security problems

Revision 1.662  2007/07/24 20:22:46  cheshire
Make sure all fields of main mDNS object are initialized correctly

Revision 1.661  2007/07/21 00:54:45  cheshire
<rdar://problem/5344576> Delay IPv6 address callback until AutoTunnel route and policy is configured

Revision 1.660  2007/07/20 20:00:45  cheshire
"Legacy Browse" is better called "Automatic Browse"

Revision 1.659  2007/07/20 00:54:18  cheshire
<rdar://problem/4641118> Need separate SCPreferences for per-user .Mac settings

Revision 1.658  2007/07/18 02:28:57  cheshire
Don't set AutoTunnel settings in uDNS_RegisterService; should be done in GetServiceTarget

Revision 1.657  2007/07/18 00:57:10  cheshire
<rdar://problem/5303834> Automatically configure IPSec policy when resolving services
Only need to call AddNewClientTunnel() for IPv6 addresses

Revision 1.656  2007/07/16 23:54:48  cheshire
<rdar://problem/5338850> Crash when removing or changing DNS keys

Revision 1.655  2007/07/16 20:11:37  vazquez
<rdar://problem/3867231> LegacyNATTraversal: Need complete rewrite
Init LNT stuff and handle SSDP packets

Revision 1.654  2007/07/12 23:30:23  cheshire
Changed some 'LogOperation' calls to 'debugf' to reduce verbosity in syslog

Revision 1.653  2007/07/12 02:51:27  cheshire
<rdar://problem/5303834> Automatically configure IPSec policy when resolving services

Revision 1.652  2007/07/11 23:43:42  cheshire
Rename PurgeCacheResourceRecord to mDNS_PurgeCacheResourceRecord

Revision 1.651  2007/07/11 22:44:40  cheshire
<rdar://problem/5328801> SIGHUP should purge the cache

Revision 1.650  2007/07/11 21:34:09  cheshire
<rdar://problem/5304766> Register IPSec tunnel with IPv4-only hostname and create NAT port mappings
Need to hold mDNS_Lock when calling mDNS_AddDynDNSHostName/mDNS_RemoveDynDNSHostName

Revision 1.649  2007/07/11 02:52:52  cheshire
<rdar://problem/5303807> Register IPv6-only hostname and don't create port mappings for AutoTunnel services
In uDNS_RegisterService, set HostTarget for AutoTunnel services

Revision 1.648  2007/07/09 23:48:12  cheshire
Add parentheses around bitwise operation for clarity

Revision 1.647  2007/07/06 21:17:55  cheshire
Initialize m->retryGetAddr to timenow + 0x78000000;

Revision 1.646  2007/07/06 18:55:49  cheshire
Initialize m->NextScheduledNATOp

Revision 1.645  2007/06/29 22:55:54  cheshire
Move declaration of DNSServer *s; Fixed incomplete comment.

Revision 1.644  2007/06/29 00:07:29  vazquez
<rdar://problem/5301908> Clean up NAT state machine (necessary for 6 other fixes)

Revision 1.643  2007/06/20 01:10:12  cheshire
<rdar://problem/5280520> Sync iPhone changes into main mDNSResponder code

Revision 1.642  2007/06/15 21:54:50  cheshire
<rdar://problem/4883206> Add packet logging to help debugging private browsing over TLS

Revision 1.641  2007/05/25 00:30:24  cheshire
When checking for duplicate questions, make sure privacy (or not) status, and long-lived (or not)
status matches. This is particularly important when doing a private query for an SOA record,
which will result in a call StartGetZoneData which does a non-private query for the same SOA record.
If the latter is tagged as a duplicate of the former, then we have deadlock, and neither will complete.

Revision 1.640  2007/05/25 00:25:44  cheshire
<rdar://problem/5227737> Need to enhance putRData to output all current known types

Revision 1.639  2007/05/23 00:51:33  cheshire
Increase threshold for shedding cache records from 512 to 3000. The 512 figure was calculated when
each cache entry took about 700 bytes; now they're only 164 bytes. Also, machines have more RAM these
days, and there are more services being advertised using DNS-SD, so it makes sense to cache more.

Revision 1.638  2007/05/23 00:43:16  cheshire
If uDNS UDP response has TC (truncated) bit set, don't interpret it as being the entire RRSet

Revision 1.637  2007/05/14 23:53:00  cheshire
Export mDNS_StartQuery_internal and mDNS_StopQuery_internal so they can be called from uDNS.c

Revision 1.636  2007/05/10 23:27:15  cheshire
Update mDNS_Deregister_internal debugging messages

Revision 1.635  2007/05/07 20:43:45  cheshire
<rdar://problem/4241419> Reduce the number of queries and announcements

Revision 1.634  2007/05/04 22:09:08  cheshire
Only do "restarting exponential backoff sequence" for mDNS questions
In mDNS_RegisterInterface, only retrigger mDNS questions
In uDNS_SetupDNSConfig, use ActivateUnicastQuery() instead of just setting q->ThisQInterval directly

Revision 1.633  2007/05/04 21:45:12  cheshire
Get rid of unused q->RestartTime; Get rid of uDNS_Close (synonym for uDNS_Sleep)

Revision 1.632  2007/05/04 20:20:50  cheshire
<rdar://problem/5167331> RegisterRecord and RegisterService need to cancel StartGetZoneData
Need to set srs->nta = mDNSNULL; when regState_NoTarget

Revision 1.631  2007/05/04 00:39:42  cheshire
<rdar://problem/4410011> Eliminate looping SOA lookups
When creating a cascade of negative SOA cache entries, CacheGroup pointer cg needs to be updated
each time round the loop to reference the right CacheGroup for each newly fabricated SOA name

Revision 1.630  2007/05/03 22:40:38  cheshire
<rdar://problem/4669229> mDNSResponder ignores bogus null target in SRV record

Revision 1.629  2007/05/03 00:15:51  cheshire
<rdar://problem/4410011> Eliminate looping SOA lookups

Revision 1.628  2007/05/02 22:21:33  cheshire
<rdar://problem/5167331> RegisterRecord and RegisterService need to cancel StartGetZoneData

Revision 1.627  2007/04/30 19:29:13  cheshire
Fix display of port number in "Updating DNS Server" message

Revision 1.626  2007/04/30 04:21:13  cheshire
Can't safely call AnswerLocalQuestions() from within mDNS_Deregister() -- need to defer it until mDNS_Execute time

Revision 1.625  2007/04/28 01:34:21  cheshire
Fixed crashing bug: We need to update rr->CRActiveQuestion pointers for *all* questions
(Code was explicitly ignoring wide-area unicast questions, leading to stale pointers and crashes)

Revision 1.624  2007/04/27 21:04:30  cheshire
On network configuration change, need to call uDNS_RegisterSearchDomains

Revision 1.623  2007/04/27 19:28:01  cheshire
Any code that calls StartGetZoneData needs to keep a handle to the structure, so
it can cancel it if necessary. (First noticed as a crash in Apple Remote Desktop
-- it would start a query and then quickly cancel it, and then when
StartGetZoneData completed, it had a dangling pointer and crashed.)

Revision 1.622  2007/04/26 16:09:22  cheshire
mDNS_StopQueryWithRemoves should ignore kDNSRecordTypePacketNegative records

Revision 1.621  2007/04/26 15:43:22  cheshire
Make sure DNSServer *s is non-null before using value in LogOperation

Revision 1.620  2007/04/26 13:11:05  cheshire
Fixed crash when logging out of VPN

Revision 1.619  2007/04/26 00:35:15  cheshire
<rdar://problem/5140339> uDNS: Domain discovery not working over VPN
Fixes to make sure results update correctly when connectivity changes (e.g. a DNS server
inside the firewall may give answers where a public one gives none, and vice versa.)

Revision 1.618  2007/04/25 19:26:01  cheshire
m->NextScheduledQuery was getting set too early in SendQueries()
Improved "SendQueries didn't send all its queries" debugging message

Revision 1.617  2007/04/25 17:48:22  cheshire
Update debugging message

Revision 1.616  2007/04/25 16:38:32  cheshire
If negative cache entry already exists, reactivate it instead of creating a new one

Revision 1.615  2007/04/25 02:14:38  cheshire
<rdar://problem/4246187> uDNS: Identical client queries should reference a single shared core query
Additional fixes to make LLQs work properly

Revision 1.614  2007/04/23 21:52:45  cheshire
<rdar://problem/5094009> IPv6 filtering in AirPort base station breaks Wide-Area Bonjour

Revision 1.613  2007/04/23 04:58:20  cheshire
<rdar://problem/5072548> Crash when setting extremely large TXT records

Revision 1.612  2007/04/22 20:39:38  cheshire
<rdar://problem/4633194> Add 20 to 120ms random delay to browses

Revision 1.611  2007/04/22 18:16:29  cheshire
Removed incorrect ActiveQuestion(q) check that was preventing suspended questions from getting reactivated

Revision 1.610  2007/04/22 06:02:02  cheshire
<rdar://problem/4615977> Query should immediately return failure when no server

Revision 1.609  2007/04/20 21:17:24  cheshire
For naming consistency, kDNSRecordTypeNegative should be kDNSRecordTypePacketNegative

Revision 1.608  2007/04/20 19:45:31  cheshire
In LogAllOperations mode, dump out unknown DNS packets in their entirety

Revision 1.607  2007/04/19 23:56:25  cheshire
Don't do cache-flush processing for LLQ answers

Revision 1.606  2007/04/19 22:50:53  cheshire
<rdar://problem/4246187> Identical client queries should reference a single shared core query

Revision 1.605  2007/04/19 20:06:41  cheshire
Rename field 'Private' (sounds like a boolean) to more informative 'AuthInfo' (it's a DomainAuthInfo pointer)

Revision 1.604  2007/04/19 18:03:04  cheshire
Add "const" declaration

Revision 1.603  2007/04/06 21:00:25  cheshire
Fix log message typo

Revision 1.602  2007/04/05 22:55:35  cheshire
<rdar://problem/5077076> Records are ending up in Lighthouse without expiry information

Revision 1.601  2007/04/04 21:48:52  cheshire
<rdar://problem/4720694> Combine unicast authoritative answer list with multicast list

Revision 1.600  2007/04/04 01:31:33  cheshire
Improve debugging message

Revision 1.599  2007/04/04 00:03:26  cheshire
<rdar://problem/5089862> DNSServiceQueryRecord is returning kDNSServiceErr_NoSuchRecord for empty rdata

Revision 1.598  2007/04/03 19:43:16  cheshire
Use mDNSSameIPPort (and similar) instead of accessing internal fields directly

Revision 1.597  2007/03/31 00:32:32  cheshire
After skipping OPT and TSIG, clear m->rec.r.resrec.RecordType

Revision 1.596  2007/03/28 20:59:26  cheshire
<rdar://problem/4743285> Remove inappropriate use of IsPrivateV4Addr()

Revision 1.595  2007/03/26 23:48:16  cheshire
<rdar://problem/4848295> Advertise model information via Bonjour
Refinements to reduce unnecessary transmissions of the DeviceInfo TXT record

Revision 1.594  2007/03/26 23:05:05  cheshire
<rdar://problem/5089257> Don't cache TSIG records

Revision 1.593  2007/03/23 17:40:08  cheshire
<rdar://problem/4060169> Bug when auto-renaming Computer Name after name collision

Revision 1.592  2007/03/22 18:31:48  cheshire
Put dst parameter first in mDNSPlatformStrCopy/mDNSPlatformMemCopy, like conventional Posix strcpy/memcpy

Revision 1.591  2007/03/22 00:49:19  cheshire
<rdar://problem/4848295> Advertise model information via Bonjour

Revision 1.590  2007/03/21 23:05:59  cheshire
Rename uDNS_HostnameInfo to HostnameInfo; deleted some unused fields

Revision 1.589  2007/03/20 15:37:19  cheshire
Delete unnecessary log message

Revision 1.588  2007/03/20 00:24:44  cheshire
<rdar://problem/4175213> Should deliver "name registered" callback slightly *before* announcing PTR record

Revision 1.587  2007/03/16 22:10:56  cheshire
<rdar://problem/4471307> mDNS: Query for *either* type A or AAAA should return both types

Revision 1.586  2007/03/10 03:26:44  cheshire
<rdar://problem/4961667> uDNS: LLQ refresh response packet causes cached records to be removed from cache

Revision 1.585  2007/03/10 02:02:58  cheshire
<rdar://problem/4961667> uDNS: LLQ refresh response packet causes cached records to be removed from cache
Eliminate unnecessary "InternalResponseHndlr responseCallback" function pointer

Revision 1.584  2007/02/28 01:51:27  cheshire
Added comment about reverse-order IP address

Revision 1.583  2007/01/27 03:19:33  cheshire
Need to initialize question->sock

Revision 1.582  2007/01/25 00:40:16  cheshire
Unified CNAME-following functionality into cache management code (which means CNAME-following
should now also work for mDNS queries too); deleted defunct pktResponseHndlr() routine.

Revision 1.581  2007/01/23 02:56:11  cheshire
Store negative results in the cache, instead of generating them out of pktResponseHndlr()

Revision 1.580  2007/01/19 21:17:33  cheshire
StartLLQPolling needs to call SetNextQueryTime() to cause query to be done in a timely fashion

Revision 1.579  2007/01/19 18:39:10  cheshire
Fix a bunch of parameters that should have been declared "const"

Revision 1.578  2007/01/10 22:51:57  cheshire
<rdar://problem/4917539> Add support for one-shot private queries as well as long-lived private queries

Revision 1.577  2007/01/10 02:05:21  cheshire
Delay uDNS_SetupDNSConfig() until *after* the platform layer
has set up the interface list and security credentials

Revision 1.576  2007/01/09 02:40:57  cheshire
uDNS_SetupDNSConfig() shouldn't be called from mDNSMacOSX.c (platform support layer);
moved it to mDNS_Init() in mDNS.c (core code)

Revision 1.575  2007/01/09 00:17:25  cheshire
Improve "ERROR m->CurrentRecord already set" debugging messages

Revision 1.574  2007/01/05 08:30:41  cheshire
Trim excessive "$Log" checkin history from before 2006
(checkin history still available via "cvs log ..." of course)

Revision 1.573  2007/01/05 06:34:03  cheshire
Improve "ERROR m->CurrentQuestion already set" debugging messages

Revision 1.572  2007/01/04 23:11:11  cheshire
<rdar://problem/4720673> uDNS: Need to start caching unicast records
When an automatic browsing domain is removed, generate appropriate "remove" events for legacy queries

Revision 1.571  2007/01/04 21:45:20  cheshire
Added mDNS_DropLockBeforeCallback/mDNS_ReclaimLockAfterCallback macros,
to do additional lock sanity checking around callback invocations

Revision 1.570  2007/01/04 20:57:47  cheshire
Rename ReturnCNAME to ReturnIntermed (for ReturnIntermediates)

Revision 1.569  2007/01/04 20:27:27  cheshire
Change a LogMsg() to debugf()

Revision 1.568  2007/01/04 02:39:53  cheshire
<rdar://problem/4030599> Hostname passed into DNSServiceRegister ignored for Wide-Area service registrations

Revision 1.567  2006/12/21 00:01:37  cheshire
Tidy up code alignment

Revision 1.566  2006/12/20 04:07:34  cheshire
Remove uDNS_info substructure from AuthRecord_struct

Revision 1.565  2006/12/19 22:49:23  cheshire
Remove uDNS_info substructure from ServiceRecordSet_struct

Revision 1.564  2006/12/19 02:38:20  cheshire
Get rid of unnecessary duplicate query ID field from DNSQuestion_struct

Revision 1.563  2006/12/19 02:18:48  cheshire
Get rid of unnecessary duplicate "void *context" field from DNSQuestion_struct

Revision 1.562  2006/12/16 01:58:31  cheshire
<rdar://problem/4720673> uDNS: Need to start caching unicast records

Revision 1.561  2006/12/01 07:38:53  herscher
Only perform cache workaround fix if query is wide-area

Revision 1.560  2006/11/30 23:07:56  herscher
<rdar://problem/4765644> uDNS: Sync up with Lighthouse changes for Private DNS

Revision 1.559  2006/11/27 08:20:57  cheshire
Preliminary support for unifying the uDNS and mDNS code, including caching of uDNS answers

Revision 1.558  2006/11/10 07:44:03  herscher
<rdar://problem/4825493> Fix Daemon locking failures while toggling BTMM

Revision 1.557  2006/11/10 01:12:51  cheshire
<rdar://problem/4829718> Incorrect TTL corrections

Revision 1.556  2006/11/10 00:54:14  cheshire
<rdar://problem/4816598> Changing case of Computer Name doesn't work

Revision 1.555  2006/10/30 20:03:37  cheshire
<rdar://problem/4456945> After service restarts on different port, for a few seconds DNS-SD may return stale port number

Revision 1.554  2006/10/20 05:35:04  herscher
<rdar://problem/4720713> uDNS: Merge unicast active question list with multicast list.

Revision 1.553  2006/10/05 03:42:43  herscher
Remove embedded uDNS_info struct in DNSQuestion_struct

Revision 1.552  2006/09/15 21:20:15  cheshire
Remove uDNS_info substructure from mDNS_struct

Revision 1.551  2006/08/14 23:24:22  cheshire
Re-licensed mDNSResponder daemon source code under Apache License, Version 2.0

Revision 1.550  2006/07/27 17:58:34  cheshire
Improved text of "SendQueries didn't send all its queries; will try again" debugging message

Revision 1.549  2006/07/20 22:07:31  mkrochma
<rdar://problem/4633196> Wide-area browsing is currently broken in TOT
More fixes for uninitialized variables

Revision 1.548  2006/07/20 19:30:19  mkrochma
<rdar://problem/4633196> Wide-area browsing sometimes doesn't work in TOT

Revision 1.547  2006/07/15 02:31:30  cheshire
<rdar://problem/4630812> Suppress log messages for certain old devices with inconsistent TXT RRSet TTLs

Revision 1.546  2006/07/07 01:09:09  cheshire
<rdar://problem/4472013> Add Private DNS server functionality to dnsextd
Only use mallocL/freeL debugging routines when building mDNSResponder, not dnsextd

Revision 1.545  2006/07/05 23:10:30  cheshire
<rdar://problem/4472014> Add Private DNS client functionality to mDNSResponder
Update mDNSSendDNSMessage() to use uDNS_TCPSocket type instead of "int"

Revision 1.544  2006/06/29 07:42:14  cheshire
<rdar://problem/3922989> Performance: Remove unnecessary SameDomainName() checks

Revision 1.543  2006/06/29 01:38:43  cheshire
<rdar://problem/4605285> Only request unicast responses on wake from sleep and network connection

Revision 1.542  2006/06/27 23:40:29  cheshire
Fix typo in comment: mis-spelled "compile"

Revision 1.541  2006/06/27 19:46:24  cheshire
Updated comments and debugging messages

Revision 1.540  2006/06/15 21:35:16  cheshire
Move definitions of mDNS_vsnprintf, mDNS_SetupResourceRecord, and some constants
from mDNS.c to DNSCommon.c, so they can be accessed from dnsextd code

Revision 1.539  2006/06/08 23:45:46  cheshire
Change SimultaneousProbe messages from debugf() to LogOperation()

Revision 1.538  2006/03/19 17:13:06  cheshire
<rdar://problem/4483117> Need faster purging of stale records
Shorten kDefaultReconfirmTimeForNoAnswer to five seconds
and reconfirm whole chain of antecedents ot once

Revision 1.537  2006/03/19 02:00:07  cheshire
<rdar://problem/4073825> Improve logic for delaying packets after repeated interface transitions

Revision 1.536  2006/03/08 23:29:53  cheshire
<rdar://problem/4468716> Improve "Service Renamed" log message

Revision 1.535  2006/03/02 20:41:17  cheshire
<rdar://problem/4111464> After record update, old record sometimes remains in cache
Minor code tidying and comments to reduce the risk of similar programming errors in future

Revision 1.534  2006/03/02 03:25:46  cheshire
<rdar://problem/4111464> After record update, old record sometimes remains in cache
Code to harmonize RRSet TTLs was inadvertently rescuing expiring records

Revision 1.533  2006/02/26 00:54:41  cheshire
Fixes to avoid code generation warning/error on FreeBSD 7

*/

#include "DNSCommon.h"                  // Defines general DNS untility routines
#include "uDNS.h"						// Defines entry points into unicast-specific routines

// Disable certain benign warnings with Microsoft compilers
#if(defined(_MSC_VER))
	// Disable "conditional expression is constant" warning for debug macros.
	// Otherwise, this generates warnings for the perfectly natural construct "while(1)"
	// If someone knows a variant way of writing "while(1)" that doesn't generate warning messages, please let us know
	#pragma warning(disable:4127)
	
	// Disable "assignment within conditional expression".
	// Other compilers understand the convention that if you place the assignment expression within an extra pair
	// of parentheses, this signals to the compiler that you really intended an assignment and no warning is necessary.
	// The Microsoft compiler doesn't understand this convention, so in the absense of any other way to signal
	// to the compiler that the assignment is intentional, we have to just turn this warning off completely.
	#pragma warning(disable:4706)
#endif

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Program Constants
#endif

#define NO_HINFO 1

mDNSlocal const mDNSInterfaceID mDNSInterfaceMark = (mDNSInterfaceID)~0;

// Any records bigger than this are considered 'large' records
#define SmallRecordLimit 1024

#define kMaxUpdateCredits 10
#define kUpdateCreditRefreshInterval (mDNSPlatformOneSecond * 6)

mDNSexport const char *const mDNS_DomainTypeNames[] =
	{
	 "b._dns-sd._udp.",		// Browse
	"db._dns-sd._udp.",		// Default Browse
	"lb._dns-sd._udp.",		// Automatic Browse
	 "r._dns-sd._udp.",		// Registration
	"dr._dns-sd._udp."		// Default Registration
	};

#ifdef UNICAST_DISABLED
#define uDNS_IsActiveQuery(q, u) mDNSfalse
#endif

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - General Utility Functions
#endif

#define ActiveQuestion(Q) ((Q)->ThisQInterval > 0 && !(Q)->DuplicateOf)
#define TimeToSendThisQuestion(Q,time) (ActiveQuestion(Q) && (time) - ((Q)->LastQTime + (Q)->ThisQInterval) >= 0)

mDNSexport void SetNextQueryTime(mDNS *const m, const DNSQuestion *const q)
	{
	if (ActiveQuestion(q))
		{
		mDNSs32 sendtime = q->LastQTime + q->ThisQInterval;

		// Don't allow sendtime to be earlier than SuppressStdPort53Queries
		if (!mDNSOpaque16IsZero(q->TargetQID) && !q->LongLived && m->SuppressStdPort53Queries && (sendtime - m->SuppressStdPort53Queries < 0))
			sendtime = m->SuppressStdPort53Queries;

		if (m->NextScheduledQuery - sendtime > 0)
			m->NextScheduledQuery = sendtime;
		}
	}

mDNSexport CacheGroup *CacheGroupForName(const mDNS *const m, const mDNSu32 slot, const mDNSu32 namehash, const domainname *const name)
	{
	CacheGroup *cg;
	for (cg = m->rrcache_hash[slot]; cg; cg=cg->next)
		if (cg->namehash == namehash && SameDomainName(cg->name, name))
			break;
	return(cg);
	}

mDNSlocal CacheGroup *CacheGroupForRecord(const mDNS *const m, const mDNSu32 slot, const ResourceRecord *const rr)
	{
	return(CacheGroupForName(m, slot, rr->namehash, rr->name));
	}

mDNSlocal mDNSBool AddressIsLocalSubnet(mDNS *const m, const mDNSInterfaceID InterfaceID, const mDNSAddr *addr)
	{
	NetworkInterfaceInfo *intf;

	if (addr->type == mDNSAddrType_IPv4)
		{
		// Normally we resist touching the NotAnInteger fields, but here we're doing tricky bitwise masking so we make an exception
		if (mDNSv4AddressIsLinkLocal(&addr->ip.v4)) return(mDNStrue);
		for (intf = m->HostInterfaces; intf; intf = intf->next)
			if (intf->ip.type == addr->type && intf->InterfaceID == InterfaceID && intf->McastTxRx)
				if (((intf->ip.ip.v4.NotAnInteger ^ addr->ip.v4.NotAnInteger) & intf->mask.ip.v4.NotAnInteger) == 0)
					return(mDNStrue);
		}

	if (addr->type == mDNSAddrType_IPv6)
		{
		if (mDNSv6AddressIsLinkLocal(&addr->ip.v4)) return(mDNStrue);
		for (intf = m->HostInterfaces; intf; intf = intf->next)
			if (intf->ip.type == addr->type && intf->InterfaceID == InterfaceID && intf->McastTxRx)
				if ((((intf->ip.ip.v6.l[0] ^ addr->ip.v6.l[0]) & intf->mask.ip.v6.l[0]) == 0) &&
					(((intf->ip.ip.v6.l[1] ^ addr->ip.v6.l[1]) & intf->mask.ip.v6.l[1]) == 0) &&
					(((intf->ip.ip.v6.l[2] ^ addr->ip.v6.l[2]) & intf->mask.ip.v6.l[2]) == 0) &&
					(((intf->ip.ip.v6.l[3] ^ addr->ip.v6.l[3]) & intf->mask.ip.v6.l[3]) == 0))
						return(mDNStrue);
		}

	return(mDNSfalse);
	}

// For a single given DNSQuestion, deliver an add/remove result for the single given AuthRecord
// Used by AnswerLocalQuestions() and AnswerNewLocalOnlyQuestion()
mDNSlocal void AnswerLocalOnlyQuestionWithResourceRecord(mDNS *const m, DNSQuestion *q, AuthRecord *rr, QC_result AddRecord)
	{
	// Indicate that we've given at least one positive answer for this record, so we should be prepared to send a goodbye for it
	if (AddRecord) rr->LocalAnswer = mDNStrue;
	mDNS_DropLockBeforeCallback();		// Allow client to legally make mDNS API calls from the callback
	if (q->QuestionCallback && !q->NoAnswer)
		q->QuestionCallback(m, q, &rr->resrec, AddRecord);
	mDNS_ReclaimLockAfterCallback();	// Decrement mDNS_reentrancy to block mDNS API calls again
	}

// When a new local AuthRecord is created or deleted, AnswerLocalQuestions() runs though our LocalOnlyQuestions delivering answers
// to each, stopping if it reaches a NewLocalOnlyQuestion -- brand-new questions are handled by AnswerNewLocalOnlyQuestion().
// If the AuthRecord is marked mDNSInterface_LocalOnly, then we also deliver it to any other questions we have using mDNSInterface_Any.
// Used by AnswerForNewLocalRecords() and mDNS_Deregister_internal()
mDNSlocal void AnswerLocalQuestions(mDNS *const m, AuthRecord *rr, QC_result AddRecord)
	{
	if (m->CurrentQuestion)
		LogMsg("AnswerLocalQuestions ERROR m->CurrentQuestion already set: %##s (%s)", m->CurrentQuestion->qname.c, DNSTypeName(m->CurrentQuestion->qtype));

	m->CurrentQuestion = m->LocalOnlyQuestions;
	while (m->CurrentQuestion && m->CurrentQuestion != m->NewLocalOnlyQuestions)
		{
		DNSQuestion *q = m->CurrentQuestion;
		m->CurrentQuestion = q->next;
		if (ResourceRecordAnswersQuestion(&rr->resrec, q))
			AnswerLocalOnlyQuestionWithResourceRecord(m, q, rr, AddRecord);			// MUST NOT dereference q again
		}

	// If this AuthRecord is marked LocalOnly, then we want to deliver it to all local 'mDNSInterface_Any' questions
	if (rr->resrec.InterfaceID == mDNSInterface_LocalOnly)
		{
		m->CurrentQuestion = m->Questions;
		while (m->CurrentQuestion && m->CurrentQuestion != m->NewQuestions)
			{
			DNSQuestion *q = m->CurrentQuestion;
			m->CurrentQuestion = q->next;
			if (ResourceRecordAnswersQuestion(&rr->resrec, q))
				AnswerLocalOnlyQuestionWithResourceRecord(m, q, rr, AddRecord);		// MUST NOT dereference q again
			}
		}

	m->CurrentQuestion = mDNSNULL;
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - Resource Record Utility Functions
#endif

#define RRTypeIsAddressType(T) ((T) == kDNSType_A || (T) == kDNSType_AAAA)

#define ResourceRecordIsValidAnswer(RR) ( ((RR)->             resrec.RecordType & kDNSRecordTypeActiveMask)  && \
		((RR)->Additional1 == mDNSNULL || ((RR)->Additional1->resrec.RecordType & kDNSRecordTypeActiveMask)) && \
		((RR)->Additional2 == mDNSNULL || ((RR)->Additional2->resrec.RecordType & kDNSRecordTypeActiveMask)) && \
		((RR)->DependentOn == mDNSNULL || ((RR)->DependentOn->resrec.RecordType & kDNSRecordTypeActiveMask))  )

#define ResourceRecordIsValidInterfaceAnswer(RR, INTID) \
	(ResourceRecordIsValidAnswer(RR) && \
	((RR)->resrec.InterfaceID == mDNSInterface_Any || (RR)->resrec.InterfaceID == (INTID)))

#define DefaultProbeCountForTypeUnique ((mDNSu8)3)
#define DefaultProbeCountForRecordType(X)      ((X) == kDNSRecordTypeUnique ? DefaultProbeCountForTypeUnique : (mDNSu8)0)

#define InitialAnnounceCount ((mDNSu8)8)

// Note that the announce intervals use exponential backoff, doubling each time. The probe intervals do not.
// This means that because the announce interval is doubled after sending the first packet, the first
// observed on-the-wire inter-packet interval between announcements is actually one second.
// The half-second value here may be thought of as a conceptual (non-existent) half-second delay *before* the first packet is sent.
#define DefaultProbeIntervalForTypeUnique (mDNSPlatformOneSecond/4)
#define DefaultAnnounceIntervalForTypeShared (mDNSPlatformOneSecond/2)
#define DefaultAnnounceIntervalForTypeUnique (mDNSPlatformOneSecond/2)

#define DefaultAPIntervalForRecordType(X)  ((X) & (kDNSRecordTypeAdvisory | kDNSRecordTypeShared     ) ? DefaultAnnounceIntervalForTypeShared : \
											(X) & (kDNSRecordTypeUnique                              ) ? DefaultProbeIntervalForTypeUnique    : \
											(X) & (kDNSRecordTypeVerified | kDNSRecordTypeKnownUnique) ? DefaultAnnounceIntervalForTypeUnique : 0)

#define TimeToAnnounceThisRecord(RR,time) ((RR)->AnnounceCount && (time) - ((RR)->LastAPTime + (RR)->ThisAPInterval) >= 0)
#define TimeToSendThisRecord(RR,time) ((TimeToAnnounceThisRecord(RR,time) || (RR)->ImmedAnswer) && ResourceRecordIsValidAnswer(RR))
#define TicksTTL(RR) ((mDNSs32)(RR)->resrec.rroriginalttl * mDNSPlatformOneSecond)
#define RRExpireTime(RR) ((RR)->TimeRcvd + TicksTTL(RR))

#define MaxUnansweredQueries 4

// SameResourceRecordSignature returns true if two resources records have the same name, type, and class, and may be sent
// (or were received) on the same interface (i.e. if *both* records specify an interface, then it has to match).
// TTL and rdata may differ.
// This is used for cache flush management:
// When sending a unique record, all other records matching "SameResourceRecordSignature" must also be sent
// When receiving a unique record, all old cache records matching "SameResourceRecordSignature" are flushed

mDNSlocal mDNSBool SameResourceRecordSignature(const AuthRecord *const r1, const AuthRecord *const r2)
	{
	if (!r1) { LogMsg("SameResourceRecordSignature ERROR: r1 is NULL"); return(mDNSfalse); }
	if (!r2) { LogMsg("SameResourceRecordSignature ERROR: r2 is NULL"); return(mDNSfalse); }
	if (r1->resrec.InterfaceID &&
		r2->resrec.InterfaceID &&
		r1->resrec.InterfaceID != r2->resrec.InterfaceID) return(mDNSfalse);
	return(mDNSBool)(
		r1->resrec.rrtype   == r2->resrec.rrtype &&
		r1->resrec.rrclass  == r2->resrec.rrclass &&
		r1->resrec.namehash == r2->resrec.namehash &&
		SameDomainName(r1->resrec.name, r2->resrec.name));
	}

// PacketRRMatchesSignature behaves as SameResourceRecordSignature, except that types may differ if our
// authoratative record is unique (as opposed to shared). For unique records, we are supposed to have
// complete ownership of *all* types for this name, so *any* record type with the same name is a conflict.
// In addition, when probing we send our questions with the wildcard type kDNSQType_ANY,
// so a response of any type should match, even if it is not actually the type the client plans to use.
mDNSlocal mDNSBool PacketRRMatchesSignature(const CacheRecord *const pktrr, const AuthRecord *const authrr)
	{
	if (!pktrr)  { LogMsg("PacketRRMatchesSignature ERROR: pktrr is NULL"); return(mDNSfalse); }
	if (!authrr) { LogMsg("PacketRRMatchesSignature ERROR: authrr is NULL"); return(mDNSfalse); }
	if (pktrr->resrec.InterfaceID &&
		authrr->resrec.InterfaceID &&
		pktrr->resrec.InterfaceID != authrr->resrec.InterfaceID) return(mDNSfalse);
	if (!(authrr->resrec.RecordType & kDNSRecordTypeUniqueMask) && pktrr->resrec.rrtype != authrr->resrec.rrtype) return(mDNSfalse);
	return(mDNSBool)(
		pktrr->resrec.rrclass == authrr->resrec.rrclass &&
		pktrr->resrec.namehash == authrr->resrec.namehash &&
		SameDomainName(pktrr->resrec.name, authrr->resrec.name));
	}

// IdenticalResourceRecord returns true if two resources records have
// the same name, type, class, and identical rdata (InterfaceID and TTL may differ)

// IdenticalSameNameRecord is the same, except it skips the expensive SameDomainName() check,
// which is at its most expensive and least useful in cases where we know in advance that the names match

mDNSlocal mDNSBool IdenticalResourceRecord(const ResourceRecord *const r1, const ResourceRecord *const r2)
	{
	if (!r1) { LogMsg("IdenticalResourceRecord ERROR: r1 is NULL"); return(mDNSfalse); }
	if (!r2) { LogMsg("IdenticalResourceRecord ERROR: r2 is NULL"); return(mDNSfalse); }
	if (r1->rrtype != r2->rrtype || r1->rrclass != r2->rrclass || r1->namehash != r2->namehash || !SameDomainName(r1->name, r2->name))
		return(mDNSfalse);
	return(SameRData(r1, r2));
	}

mDNSlocal mDNSBool IdenticalSameNameRecord(const ResourceRecord *const r1, const ResourceRecord *const r2)
	{
	if (!r1) { LogMsg("IdenticalSameNameRecord ERROR: r1 is NULL"); return(mDNSfalse); }
	if (!r2) { LogMsg("IdenticalSameNameRecord ERROR: r2 is NULL"); return(mDNSfalse); }
	if (r1->rrtype != r2->rrtype || r1->rrclass != r2->rrclass)
		return(mDNSfalse);

#if VerifySameNameAssumptions
	if (r1->namehash != r2->namehash || !SameDomainName(r1->name, r2->name))
		{
		LogMsg("Bogus IdenticalSameNameRecord call: %##s does not match %##s", r1->name->c, r1->name->c);
		return(mDNSfalse);
		}
#endif

	return(SameRData(r1, r2));
	}

// CacheRecord *ks is the CacheRecord from the known answer list in the query.
// This is the information that the requester believes to be correct.
// AuthRecord *rr is the answer we are proposing to give, if not suppressed.
// This is the information that we believe to be correct.
// We've already determined that we plan to give this answer on this interface
// (either the record is non-specific, or it is specific to this interface)
// so now we just need to check the name, type, class, rdata and TTL.
mDNSlocal mDNSBool ShouldSuppressKnownAnswer(const CacheRecord *const ka, const AuthRecord *const rr)
	{
	// If RR signature is different, or data is different, then don't suppress our answer
	if (!IdenticalResourceRecord(&ka->resrec, &rr->resrec)) return(mDNSfalse);
	
	// If the requester's indicated TTL is less than half the real TTL,
	// we need to give our answer before the requester's copy expires.
	// If the requester's indicated TTL is at least half the real TTL,
	// then we can suppress our answer this time.
	// If the requester's indicated TTL is greater than the TTL we believe,
	// then that's okay, and we don't need to do anything about it.
	// (If two responders on the network are offering the same information,
	// that's okay, and if they are offering the information with different TTLs,
	// the one offering the lower TTL should defer to the one offering the higher TTL.)
	return(mDNSBool)(ka->resrec.rroriginalttl >= rr->resrec.rroriginalttl / 2);
	}

mDNSlocal void SetNextAnnounceProbeTime(mDNS *const m, const AuthRecord *const rr)
	{
	if (rr->resrec.RecordType == kDNSRecordTypeUnique)
		{
		//LogMsg("ProbeCount %d Next %ld %s",
		//	rr->ProbeCount, (rr->LastAPTime + rr->ThisAPInterval) - m->timenow, ARDisplayString(m, rr));
		if (m->NextScheduledProbe - (rr->LastAPTime + rr->ThisAPInterval) >= 0)
			m->NextScheduledProbe = (rr->LastAPTime + rr->ThisAPInterval);
		}
	else if (rr->AnnounceCount && ResourceRecordIsValidAnswer(rr))
		{
		if (m->NextScheduledResponse - (rr->LastAPTime + rr->ThisAPInterval) >= 0)
			m->NextScheduledResponse = (rr->LastAPTime + rr->ThisAPInterval);
		}
	}

mDNSlocal void InitializeLastAPTime(mDNS *const m, AuthRecord *const rr)
	{
	// To allow us to aggregate probes when a group of services are registered together,
	// the first probe is delayed 1/4 second. This means the common-case behaviour is:
	// 1/4 second wait; probe
	// 1/4 second wait; probe
	// 1/4 second wait; probe
	// 1/4 second wait; announce (i.e. service is normally announced exactly one second after being registered)

	// If we have no probe suppression time set, or it is in the past, set it now
	if (m->SuppressProbes == 0 || m->SuppressProbes - m->timenow < 0)
		{
		m->SuppressProbes = NonZeroTime(m->timenow + DefaultProbeIntervalForTypeUnique);
		// If we already have a probe scheduled to go out sooner, then use that time to get better aggregation
		if (m->SuppressProbes - m->NextScheduledProbe >= 0)
			m->SuppressProbes = m->NextScheduledProbe;
		// If we already have a query scheduled to go out sooner, then use that time to get better aggregation
		if (m->SuppressProbes - m->NextScheduledQuery >= 0)
			m->SuppressProbes = m->NextScheduledQuery;
		}
	
	rr->LastAPTime      = m->SuppressProbes - rr->ThisAPInterval;
	// Set LastMCTime to now, to inhibit multicast responses
	// (no need to send additional multicast responses when we're announcing anyway)
	rr->LastMCTime      = m->timenow;
	rr->LastMCInterface = mDNSInterfaceMark;
	
	// If this is a record type that's not going to probe, then delay its first announcement so that
	// it will go out synchronized with the first announcement for the other records that *are* probing.
	// This is a minor performance tweak that helps keep groups of related records synchronized together.
	// The addition of "rr->ThisAPInterval / 2" is to make sure that, in the event that any of the probes are
	// delayed by a few milliseconds, this announcement does not inadvertently go out *before* the probing is complete.
	// When the probing is complete and those records begin to announce, these records will also be picked up and accelerated,
	// because they will meet the criterion of being at least half-way to their scheduled announcement time.
	if (rr->resrec.RecordType != kDNSRecordTypeUnique)
		rr->LastAPTime += DefaultProbeIntervalForTypeUnique * DefaultProbeCountForTypeUnique + rr->ThisAPInterval / 2;
	
	SetNextAnnounceProbeTime(m, rr);
	}

// Right now this only applies to mDNS (.local) services where the target host is always m->MulticastHostname
// Eventually we should unify this with GetServiceTarget() in uDNS.c
mDNSlocal void SetTargetToHostName(mDNS *const m, AuthRecord *const rr)
	{
	domainname *target = GetRRDomainNameTarget(&rr->resrec);

	if (!target) debugf("SetTargetToHostName: Don't know how to set the target of rrtype %d", rr->resrec.rrtype);

	if (target && SameDomainName(target, &m->MulticastHostname))
		debugf("SetTargetToHostName: Target of %##s is already %##s", rr->resrec.name->c, target->c);
	
	if (target && !SameDomainName(target, &m->MulticastHostname))
		{
		AssignDomainName(target, &m->MulticastHostname);
		SetNewRData(&rr->resrec, mDNSNULL, 0);		// Update rdlength, rdestimate, rdatahash
		
		// If we're in the middle of probing this record, we need to start again,
		// because changing its rdata may change the outcome of the tie-breaker.
		// (If the record type is kDNSRecordTypeUnique (unconfirmed unique) then DefaultProbeCountForRecordType is non-zero.)
		rr->ProbeCount     = DefaultProbeCountForRecordType(rr->resrec.RecordType);

		// If we've announced this record, we really should send a goodbye packet for the old rdata before
		// changing to the new rdata. However, in practice, we only do SetTargetToHostName for unique records,
		// so when we announce them we'll set the kDNSClass_UniqueRRSet and clear any stale data that way.
		if (rr->RequireGoodbye && rr->resrec.RecordType == kDNSRecordTypeShared)
			debugf("Have announced shared record %##s (%s) at least once: should have sent a goodbye packet before updating",
				rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype));

		rr->AnnounceCount  = InitialAnnounceCount;
		rr->RequireGoodbye = mDNSfalse;
		rr->ThisAPInterval = DefaultAPIntervalForRecordType(rr->resrec.RecordType);
		InitializeLastAPTime(m,rr);
		}
	}

mDNSlocal void AcknowledgeRecord(mDNS *const m, AuthRecord *const rr)
	{
	if (rr->RecordCallback)
		{
		// CAUTION: MUST NOT do anything more with rr after calling rr->Callback(), because the client's callback function
		// is allowed to do anything, including starting/stopping queries, registering/deregistering records, etc.
		rr->Acknowledged = mDNStrue;
		mDNS_DropLockBeforeCallback();		// Allow client to legally make mDNS API calls from the callback
		rr->RecordCallback(m, rr, mStatus_NoError);
		mDNS_ReclaimLockAfterCallback();	// Decrement mDNS_reentrancy to block mDNS API calls again
		}
	}

// Two records qualify to be local duplicates if the RecordTypes are the same, or if one is Unique and the other Verified
#define RecordLDT(A,B) ((A)->resrec.RecordType == (B)->resrec.RecordType || \
	((A)->resrec.RecordType | (B)->resrec.RecordType) == (kDNSRecordTypeUnique | kDNSRecordTypeVerified))
#define RecordIsLocalDuplicate(A,B) \
	((A)->resrec.InterfaceID == (B)->resrec.InterfaceID && RecordLDT((A),(B)) && IdenticalResourceRecord(&(A)->resrec, &(B)->resrec))

// Exported so uDNS.c can call this
mDNSexport mStatus mDNS_Register_internal(mDNS *const m, AuthRecord *const rr)
	{
	domainname *target = GetRRDomainNameTarget(&rr->resrec);
	AuthRecord *r;
	AuthRecord **p = &m->ResourceRecords;
	AuthRecord **d = &m->DuplicateRecords;

	if ((mDNSs32)rr->resrec.rroriginalttl <= 0)
		{ LogMsg("mDNS_Register_internal: TTL must be 1 - 0x7FFFFFFF %s", ARDisplayString(m, rr)); return(mStatus_BadParamErr); }

	if (!rr->resrec.RecordType)
		{ LogMsg("mDNS_Register_internal: RecordType must be non-zero %s", ARDisplayString(m, rr)); return(mStatus_BadParamErr); }

	while (*p && *p != rr) p=&(*p)->next;
	while (*d && *d != rr) d=&(*d)->next;
	if (*d || *p)
		{
		LogMsg("Error! Tried to register AuthRecord %p %##s (%s) that's already in the list",
			rr, rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype));
		return(mStatus_AlreadyRegistered);
		}

	if (rr->DependentOn)
		{
		if (rr->resrec.RecordType == kDNSRecordTypeUnique)
			rr->resrec.RecordType =  kDNSRecordTypeVerified;
		else
			{
			LogMsg("mDNS_Register_internal: ERROR! %##s (%s): rr->DependentOn && RecordType != kDNSRecordTypeUnique",
				rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype));
			return(mStatus_Invalid);
			}
		if (!(rr->DependentOn->resrec.RecordType & (kDNSRecordTypeUnique | kDNSRecordTypeVerified)))
			{
			LogMsg("mDNS_Register_internal: ERROR! %##s (%s): rr->DependentOn->RecordType bad type %X",
				rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype), rr->DependentOn->resrec.RecordType);
			return(mStatus_Invalid);
			}
		}

	// If this resource record is referencing a specific interface, make sure it exists
	if (rr->resrec.InterfaceID && rr->resrec.InterfaceID != mDNSInterface_LocalOnly)
		{
		NetworkInterfaceInfo *intf;
		for (intf = m->HostInterfaces; intf; intf = intf->next)
			if (intf->InterfaceID == rr->resrec.InterfaceID) break;
		if (!intf)
			{
			debugf("mDNS_Register_internal: Bogus InterfaceID %p in resource record", rr->resrec.InterfaceID);
			return(mStatus_BadReferenceErr);
			}
		}

	rr->next = mDNSNULL;

	// Field Group 1: The actual information pertaining to this resource record
	// Set up by client prior to call

	// Field Group 2: Persistent metadata for Authoritative Records
//	rr->Additional1       = set to mDNSNULL  in mDNS_SetupResourceRecord; may be overridden by client
//	rr->Additional2       = set to mDNSNULL  in mDNS_SetupResourceRecord; may be overridden by client
//	rr->DependentOn       = set to mDNSNULL  in mDNS_SetupResourceRecord; may be overridden by client
//	rr->RRSet             = set to mDNSNULL  in mDNS_SetupResourceRecord; may be overridden by client
//	rr->Callback          = already set      in mDNS_SetupResourceRecord
//	rr->Context           = already set      in mDNS_SetupResourceRecord
//	rr->RecordType        = already set      in mDNS_SetupResourceRecord
//	rr->HostTarget        = set to mDNSfalse in mDNS_SetupResourceRecord; may be overridden by client
//	rr->AllowRemoteQuery  = set to mDNSfalse in mDNS_SetupResourceRecord; may be overridden by client
	// Make sure target is not uninitialized data, or we may crash writing debugging log messages
	if (rr->AutoTarget && target) target->c[0] = 0;

	// Field Group 3: Transient state for Authoritative Records
	rr->Acknowledged      = mDNSfalse;
	rr->ProbeCount        = DefaultProbeCountForRecordType(rr->resrec.RecordType);
	rr->AnnounceCount     = InitialAnnounceCount;
	rr->RequireGoodbye    = mDNSfalse;
	rr->LocalAnswer       = mDNSfalse;
	rr->IncludeInProbe    = mDNSfalse;
	rr->ImmedAnswer       = mDNSNULL;
	rr->ImmedUnicast      = mDNSfalse;
	rr->ImmedAdditional   = mDNSNULL;
	rr->SendRNow          = mDNSNULL;
	rr->v4Requester       = zerov4Addr;
	rr->v6Requester       = zerov6Addr;
	rr->NextResponse      = mDNSNULL;
	rr->NR_AnswerTo       = mDNSNULL;
	rr->NR_AdditionalTo   = mDNSNULL;
	rr->ThisAPInterval    = DefaultAPIntervalForRecordType(rr->resrec.RecordType);
	if (!rr->AutoTarget) InitializeLastAPTime(m, rr);
//	rr->LastAPTime        = Set for us in InitializeLastAPTime()
//	rr->LastMCTime        = Set for us in InitializeLastAPTime()
//	rr->LastMCInterface   = Set for us in InitializeLastAPTime()
	rr->NewRData          = mDNSNULL;
	rr->newrdlength       = 0;
	rr->UpdateCallback    = mDNSNULL;
	rr->UpdateCredits     = kMaxUpdateCredits;
	rr->NextUpdateCredit  = 0;
	rr->UpdateBlocked     = 0;

	// Field Group 4: Transient uDNS state for Authoritative Records
	rr->state             = regState_Zero;
	rr->uselease          = 0;
	rr->expire            = 0;
	rr->Private           = 0;
	rr->id                = zeroID;
	rr->zone.c[0]         = 0;
	rr->UpdateServer      = zeroAddr;
	rr->UpdatePort        = zeroIPPort;
	rr->nta               = mDNSNULL;
	rr->tcp               = mDNSNULL;
	rr->OrigRData         = 0;
	rr->OrigRDLen         = 0;
	rr->InFlightRData     = 0;
	rr->InFlightRDLen     = 0;
	rr->QueuedRData       = 0;
	rr->QueuedRDLen       = 0;

//	rr->resrec.interface         = already set in mDNS_SetupResourceRecord
//	rr->resrec.name->c            = MUST be set by client
//	rr->resrec.rrtype            = already set in mDNS_SetupResourceRecord
//	rr->resrec.rrclass           = already set in mDNS_SetupResourceRecord
//	rr->resrec.rroriginalttl     = already set in mDNS_SetupResourceRecord
//	rr->resrec.rdata             = MUST be set by client, unless record type is CNAME or PTR and rr->HostTarget is set

	if (rr->AutoTarget)
		SetTargetToHostName(m, rr);	// Also sets rdlength and rdestimate for us, and calls InitializeLastAPTime();
	else
		{
		rr->resrec.rdlength   = GetRDLength(&rr->resrec, mDNSfalse);
		rr->resrec.rdestimate = GetRDLength(&rr->resrec, mDNStrue);
		}

	if (!ValidateDomainName(rr->resrec.name))
		{ LogMsg("Attempt to register record with invalid name: %s", ARDisplayString(m, rr)); return(mStatus_Invalid); }

	// BIND named (name daemon) doesn't allow TXT records with zero-length rdata. This is strictly speaking correct,
	// since RFC 1035 specifies a TXT record as "One or more <character-string>s", not "Zero or more <character-string>s".
	// Since some legacy apps try to create zero-length TXT records, we'll silently correct it here.
	if (rr->resrec.rrtype == kDNSType_TXT && rr->resrec.rdlength == 0) { rr->resrec.rdlength = 1; rr->resrec.rdata->u.txt.c[0] = 0; }

	// Don't do this until *after* we've set rr->resrec.rdlength
	if (!ValidateRData(rr->resrec.rrtype, rr->resrec.rdlength, rr->resrec.rdata))
		{ LogMsg("Attempt to register record with invalid rdata: %s", ARDisplayString(m, rr)); return(mStatus_Invalid); }

	rr->resrec.namehash   = DomainNameHashValue(rr->resrec.name);
	rr->resrec.rdatahash  = target ? DomainNameHashValue(target) : RDataHashValue(rr->resrec.rdlength, &rr->resrec.rdata->u);
	
	if (rr->resrec.InterfaceID == mDNSInterface_LocalOnly)
		{
		// If this is supposed to be unique, make sure we don't have any name conflicts
		if (rr->resrec.RecordType & kDNSRecordTypeUniqueMask)
			{
			const AuthRecord *s1 = rr->RRSet ? rr->RRSet : rr;
			for (r = m->ResourceRecords; r; r=r->next)
				{
				const AuthRecord *s2 = r->RRSet ? r->RRSet : r;
				if (s1 != s2 && SameResourceRecordSignature(r, rr) && !SameRData(&r->resrec, &rr->resrec))
					break;
				}
			if (r)	// If we found a conflict, set RecordType = kDNSRecordTypeDeregistering so we'll deliver the callback
				{
				debugf("Name conflict %p %##s (%s)", rr, rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype));
				rr->resrec.RecordType    = kDNSRecordTypeDeregistering;
				rr->resrec.rroriginalttl = 0;
				rr->ImmedAnswer          = mDNSInterfaceMark;
				m->NextScheduledResponse = m->timenow;
				}
			}
		}

	// Now that we've finished building our new record, make sure it's not identical to one we already have
	for (r = m->ResourceRecords; r; r=r->next) if (RecordIsLocalDuplicate(r, rr)) break;
	
	if (r)
		{
		debugf("Adding to duplicate list %p %s", rr, ARDisplayString(m,rr));
		*d = rr;
		// If the previous copy of this record is already verified unique,
		// then indicate that we should move this record promptly to kDNSRecordTypeUnique state.
		// Setting ProbeCount to zero will cause SendQueries() to advance this record to
		// kDNSRecordTypeVerified state and call the client callback at the next appropriate time.
		if (rr->resrec.RecordType == kDNSRecordTypeUnique && r->resrec.RecordType == kDNSRecordTypeVerified)
			rr->ProbeCount = 0;
		}
	else
		{
		debugf("Adding to active record list %p %s", rr, ARDisplayString(m,rr));
		if (!m->NewLocalRecords) m->NewLocalRecords = rr;
		*p = rr;
		}

	if (rr->resrec.InterfaceID != mDNSInterface_Any || rr->ForceMCast || IsLocalDomain(rr->resrec.name))
		{
		// For records that are not going to probe, acknowledge them right away
		if (rr->resrec.RecordType != kDNSRecordTypeUnique && rr->resrec.RecordType != kDNSRecordTypeDeregistering)
			AcknowledgeRecord(m, rr);
		}
#ifndef UNICAST_DISABLED
	else
		{
		if (rr->resrec.RecordType == kDNSRecordTypeUnique) rr->resrec.RecordType = kDNSRecordTypeVerified;
		rr->ProbeCount    = 0;
		rr->AnnounceCount = 0;
		rr->state = regState_FetchingZoneData;
		rr->uselease = mDNStrue;
		rr->nta = StartGetZoneData(m, rr->resrec.name, ZoneServiceUpdate, RecordRegistrationCallback, rr);
		return rr->nta ? mStatus_NoError : mStatus_NoMemoryErr;
		}
#endif
	
	return(mStatus_NoError);
	}

mDNSlocal void RecordProbeFailure(mDNS *const m, const AuthRecord *const rr)
	{
	m->ProbeFailTime = m->timenow;
	m->NumFailedProbes++;
	// If we've had fifteen or more probe failures, rate-limit to one every five seconds.
	// If a bunch of hosts have all been configured with the same name, then they'll all
	// conflict and run through the same series of names: name-2, name-3, name-4, etc.,
	// up to name-10. After that they'll start adding random increments in the range 1-100,
	// so they're more likely to branch out in the available namespace and settle on a set of
	// unique names quickly. If after five more tries the host is still conflicting, then we
	// may have a serious problem, so we start rate-limiting so we don't melt down the network.
	if (m->NumFailedProbes >= 15)
		{
		m->SuppressProbes = NonZeroTime(m->timenow + mDNSPlatformOneSecond * 5);
		LogMsg("Excessive name conflicts (%lu) for %##s (%s); rate limiting in effect",
			m->NumFailedProbes, rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype));
		}
	}

mDNSlocal void CompleteRDataUpdate(mDNS *const m, AuthRecord *const rr)
	{
	RData *OldRData = rr->resrec.rdata;
	SetNewRData(&rr->resrec, rr->NewRData, rr->newrdlength);	// Update our rdata
	rr->NewRData = mDNSNULL;									// Clear the NewRData pointer ...
	if (rr->UpdateCallback)
		rr->UpdateCallback(m, rr, OldRData);					// ... and let the client know
	}

// NOTE: mDNS_Deregister_internal can call a user callback, which may change the record list and/or question list.
// Any code walking either list must use the CurrentQuestion and/or CurrentRecord mechanism to protect against this.
// Exported so uDNS.c can call this
mDNSexport mStatus mDNS_Deregister_internal(mDNS *const m, AuthRecord *const rr, mDNS_Dereg_type drt)
	{
	AuthRecord *r2;
	mDNSu8 RecordType = rr->resrec.RecordType;
	AuthRecord **p = &m->ResourceRecords;	// Find this record in our list of active records

	while (*p && *p != rr) p=&(*p)->next;

	if (*p)
		{
		// We found our record on the main list. See if there are any duplicates that need special handling.
		if (drt == mDNS_Dereg_conflict)		// If this was a conflict, see that all duplicates get the same treatment
			{
			// Scan for duplicates of rr, and mark them for deregistration at the end of this routine, after we've finished
			// deregistering rr. We need to do this scan *before* we give the client the chance to free and reuse the rr memory.
			for (r2 = m->DuplicateRecords; r2; r2=r2->next) if (RecordIsLocalDuplicate(r2, rr)) r2->ProbeCount = 0xFF;
			}
		else
			{
			// Before we delete the record (and potentially send a goodbye packet)
			// first see if we have a record on the duplicate list ready to take over from it.
			AuthRecord **d = &m->DuplicateRecords;
			while (*d && !RecordIsLocalDuplicate(*d, rr)) d=&(*d)->next;
			if (*d)
				{
				AuthRecord *dup = *d;
				debugf("Duplicate record %p taking over from %p %##s (%s)",
					dup, rr, rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype));
				*d        = dup->next;		// Cut replacement record from DuplicateRecords list
				dup->next = rr->next;		// And then...
				rr->next  = dup;			// ... splice it in right after the record we're about to delete
				dup->resrec.RecordType        = rr->resrec.RecordType;
				dup->ProbeCount      = rr->ProbeCount;
				dup->AnnounceCount   = rr->AnnounceCount;
				dup->RequireGoodbye  = rr->RequireGoodbye;
				dup->ImmedAnswer     = rr->ImmedAnswer;
				dup->ImmedUnicast    = rr->ImmedUnicast;
				dup->ImmedAdditional = rr->ImmedAdditional;
				dup->v4Requester     = rr->v4Requester;
				dup->v6Requester     = rr->v6Requester;
				dup->ThisAPInterval  = rr->ThisAPInterval;
				dup->LastAPTime      = rr->LastAPTime;
				dup->LastMCTime      = rr->LastMCTime;
				dup->LastMCInterface = rr->LastMCInterface;
				rr->RequireGoodbye = mDNSfalse;
				}
			}
		}
	else
		{
		// We didn't find our record on the main list; try the DuplicateRecords list instead.
		p = &m->DuplicateRecords;
		while (*p && *p != rr) p=&(*p)->next;
		// If we found our record on the duplicate list, then make sure we don't send a goodbye for it
		if (*p) rr->RequireGoodbye = mDNSfalse;
		if (*p) debugf("DNS_Deregister_internal: Deleting DuplicateRecord %p %##s (%s)",
			rr, rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype));
		}

	if (!*p)
		{
		// No need to log an error message if we already know this is a potentially repeated deregistration
		if (drt != mDNS_Dereg_repeat)
			LogMsg("mDNS_Deregister_internal: Record %p %##s (%s) not found in list",
				rr, rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype));
		return(mStatus_BadReferenceErr);
		}

	// If this is a shared record and we've announced it at least once,
	// we need to retract that announcement before we delete the record

	// If this is a record (including mDNSInterface_LocalOnly records) for which we've given local answers then
	// it's tempting to just do "AnswerLocalQuestions(m, rr, mDNSfalse)" here, but that would not not be safe.
	// The AnswerLocalQuestions routine walks the question list invoking client callbacks, using the "m->CurrentQuestion"
	// mechanism to cope with the client callback modifying the question list while that's happening.
	// However, mDNS_Deregister could have been called from a client callback (e.g. from the domain enumeration callback FoundDomain)
	// which means that the "m->CurrentQuestion" mechanism is already in use to protect that list, so we can't use it twice.
	// More generally, if we invoke callbacks from within a client callback, then those callbacks could deregister other
	// records, thereby invoking yet more callbacks, without limit.
	// The solution is to defer delivering the "Remove" events until mDNS_Execute time, just like we do for sending
	// actual goodbye packets.
	
#ifndef UNICAST_DISABLED
	if (rr->resrec.InterfaceID != mDNSInterface_LocalOnly && !rr->ForceMCast && !IsLocalDomain(rr->resrec.name))
		if (rr->RequireGoodbye)
			{
			if (rr->tcp) { DisposeTCPConn(rr->tcp); rr->tcp = mDNSNULL; }
			rr->resrec.RecordType    = kDNSRecordTypeDeregistering;
			uDNS_DeregisterRecord(m, rr);
			if (rr->tcp) return(mStatus_NoError);
			}
#endif UNICAST_DISABLED

	if (RecordType == kDNSRecordTypeShared && (rr->RequireGoodbye || rr->LocalAnswer))
		{
		verbosedebugf("mDNS_Deregister_internal: Sending deregister for %s", ARDisplayString(m, rr));
		rr->resrec.RecordType    = kDNSRecordTypeDeregistering;
		rr->resrec.rroriginalttl = 0;
		rr->ImmedAnswer          = mDNSInterfaceMark;
		if (m->NextScheduledResponse - (m->timenow + mDNSPlatformOneSecond/10) >= 0)
			m->NextScheduledResponse = (m->timenow + mDNSPlatformOneSecond/10);
		}
	else
		{
		*p = rr->next;					// Cut this record from the list
		// If someone is about to look at this, bump the pointer forward
		if (m->CurrentRecord   == rr) m->CurrentRecord   = rr->next;
		if (m->NewLocalRecords == rr) m->NewLocalRecords = rr->next;
		rr->next = mDNSNULL;

		if      (RecordType == kDNSRecordTypeUnregistered)
			LogMsg("mDNS_Deregister_internal: %s already marked kDNSRecordTypeUnregistered", ARDisplayString(m, rr));
		else if (RecordType == kDNSRecordTypeDeregistering)
			LogMsg("mDNS_Deregister_internal: %s already marked kDNSRecordTypeDeregistering", ARDisplayString(m, rr));
		else
			{
			verbosedebugf("mDNS_Deregister_internal: Deleting record for %s", ARDisplayString(m, rr));
			rr->resrec.RecordType = kDNSRecordTypeUnregistered;
			}

		if ((drt == mDNS_Dereg_conflict || drt == mDNS_Dereg_repeat) && RecordType == kDNSRecordTypeShared)
			debugf("mDNS_Deregister_internal: Cannot have a conflict on a shared record! %##s (%s)",
				rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype));

		// If we have an update queued up which never executed, give the client a chance to free that memory
		if (rr->NewRData) CompleteRDataUpdate(m, rr);	// Update our rdata, clear the NewRData pointer, and return memory to the client

		if (rr->nta) { CancelGetZoneData(m, rr->nta); rr->nta = mDNSNULL; }
		if (rr->tcp) { DisposeTCPConn(rr->tcp);       rr->tcp = mDNSNULL; }

		// CAUTION: MUST NOT do anything more with rr after calling rr->Callback(), because the client's callback function
		// is allowed to do anything, including starting/stopping queries, registering/deregistering records, etc.
		// In this case the likely client action to the mStatus_MemFree message is to free the memory,
		// so any attempt to touch rr after this is likely to lead to a crash.
		mDNS_DropLockBeforeCallback();		// Allow client to legally make mDNS API calls from the callback
		if (drt != mDNS_Dereg_conflict)
			{
			if (rr->RecordCallback)
				rr->RecordCallback(m, rr, mStatus_MemFree);			// MUST NOT touch rr after this
			}
		else
			{
			RecordProbeFailure(m, rr);
			if (rr->RecordCallback)
				rr->RecordCallback(m, rr, mStatus_NameConflict);	// MUST NOT touch rr after this
			// Now that we've finished deregistering rr, check our DuplicateRecords list for any that we marked previously.
			// Note that with all the client callbacks going on, by the time we get here all the
			// records we marked may have been explicitly deregistered by the client anyway.
			r2 = m->DuplicateRecords;
			while (r2)
				{
				if (r2->ProbeCount != 0xFF) r2 = r2->next;
				else { mDNS_Deregister_internal(m, r2, mDNS_Dereg_conflict); r2 = m->DuplicateRecords; }
				}
			}
		mDNS_ReclaimLockAfterCallback();	// Decrement mDNS_reentrancy to block mDNS API calls again
		}
	return(mStatus_NoError);
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - Packet Sending Functions
#endif

mDNSlocal void AddRecordToResponseList(AuthRecord ***nrpp, AuthRecord *rr, AuthRecord *add)
	{
	if (rr->NextResponse == mDNSNULL && *nrpp != &rr->NextResponse)
		{
		**nrpp = rr;
		// NR_AdditionalTo must point to a record with NR_AnswerTo set (and not NR_AdditionalTo)
		// If 'add' does not meet this requirement, then follow its NR_AdditionalTo pointer to a record that does
		// The referenced record will definitely be acceptable (by recursive application of this rule)
		if (add && add->NR_AdditionalTo) add = add->NR_AdditionalTo;
		rr->NR_AdditionalTo = add;
		*nrpp = &rr->NextResponse;
		}
	debugf("AddRecordToResponseList: %##s (%s) already in list", rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype));
	}

mDNSlocal void AddAdditionalsToResponseList(mDNS *const m, AuthRecord *ResponseRecords, AuthRecord ***nrpp, const mDNSInterfaceID InterfaceID)
	{
	AuthRecord  *rr, *rr2;
	for (rr=ResponseRecords; rr; rr=rr->NextResponse)			// For each record we plan to put
		{
		// (Note: This is an "if", not a "while". If we add a record, we'll find it again
		// later in the "for" loop, and we will follow further "additional" links then.)
		if (rr->Additional1 && ResourceRecordIsValidInterfaceAnswer(rr->Additional1, InterfaceID))
			AddRecordToResponseList(nrpp, rr->Additional1, rr);

		if (rr->Additional2 && ResourceRecordIsValidInterfaceAnswer(rr->Additional2, InterfaceID))
			AddRecordToResponseList(nrpp, rr->Additional2, rr);

		// For SRV records, automatically add the Address record(s) for the target host
		if (rr->resrec.rrtype == kDNSType_SRV)
			{
			for (rr2=m->ResourceRecords; rr2; rr2=rr2->next)					// Scan list of resource records
				if (RRTypeIsAddressType(rr2->resrec.rrtype) &&					// For all address records (A/AAAA) ...
					ResourceRecordIsValidInterfaceAnswer(rr2, InterfaceID) &&	// ... which are valid for answer ...
					rr->resrec.rdatahash == rr2->resrec.namehash &&			// ... whose name is the name of the SRV target
					SameDomainName(&rr->resrec.rdata->u.srv.target, rr2->resrec.name))
					AddRecordToResponseList(nrpp, rr2, rr);
			}
		else if (RRTypeIsAddressType(rr->resrec.rrtype))	// For A or AAAA, put counterpart as additional
			{
			for (rr2=m->ResourceRecords; rr2; rr2=rr2->next)					// Scan list of resource records
				if (RRTypeIsAddressType(rr2->resrec.rrtype) &&					// For all address records (A/AAAA) ...
					ResourceRecordIsValidInterfaceAnswer(rr2, InterfaceID) &&	// ... which are valid for answer ...
					rr->resrec.namehash == rr2->resrec.namehash &&				// ... and have the same name
					SameDomainName(rr->resrec.name, rr2->resrec.name))
					AddRecordToResponseList(nrpp, rr2, rr);
			}
		else if (rr->resrec.rrtype == kDNSType_PTR)			// For service PTR, see if we want to add DeviceInfo record
			{
			if (ResourceRecordIsValidInterfaceAnswer(&m->DeviceInfo, InterfaceID) &&
				SameDomainLabel(rr->resrec.rdata->u.name.c, m->DeviceInfo.resrec.name->c))
				AddRecordToResponseList(nrpp, &m->DeviceInfo, rr);
			}
		}
	}

mDNSlocal void SendDelayedUnicastResponse(mDNS *const m, const mDNSAddr *const dest, const mDNSInterfaceID InterfaceID)
	{
	AuthRecord *rr;
	AuthRecord  *ResponseRecords = mDNSNULL;
	AuthRecord **nrp             = &ResponseRecords;

	// Make a list of all our records that need to be unicast to this destination
	for (rr = m->ResourceRecords; rr; rr=rr->next)
		{
		// If we find we can no longer unicast this answer, clear ImmedUnicast
		if (rr->ImmedAnswer == mDNSInterfaceMark               ||
			mDNSSameIPv4Address(rr->v4Requester, onesIPv4Addr) ||
			mDNSSameIPv6Address(rr->v6Requester, onesIPv6Addr)  )
			rr->ImmedUnicast = mDNSfalse;

		if (rr->ImmedUnicast && rr->ImmedAnswer == InterfaceID)
			if ((dest->type == mDNSAddrType_IPv4 && mDNSSameIPv4Address(rr->v4Requester, dest->ip.v4)) ||
				(dest->type == mDNSAddrType_IPv6 && mDNSSameIPv6Address(rr->v6Requester, dest->ip.v6)))
				{
				rr->ImmedAnswer  = mDNSNULL;				// Clear the state fields
				rr->ImmedUnicast = mDNSfalse;
				rr->v4Requester  = zerov4Addr;
				rr->v6Requester  = zerov6Addr;
				if (rr->NextResponse == mDNSNULL && nrp != &rr->NextResponse)	// rr->NR_AnswerTo
					{ rr->NR_AnswerTo = (mDNSu8*)~0; *nrp = rr; nrp = &rr->NextResponse; }
				}
		}

	AddAdditionalsToResponseList(m, ResponseRecords, &nrp, InterfaceID);

	while (ResponseRecords)
		{
		mDNSu8 *responseptr = m->omsg.data;
		mDNSu8 *newptr;
		InitializeDNSMessage(&m->omsg.h, zeroID, ResponseFlags);
		
		// Put answers in the packet
		while (ResponseRecords && ResponseRecords->NR_AnswerTo)
			{
			rr = ResponseRecords;
			if (rr->resrec.RecordType & kDNSRecordTypeUniqueMask)
				rr->resrec.rrclass |= kDNSClass_UniqueRRSet;		// Temporarily set the cache flush bit so PutResourceRecord will set it
			newptr = PutResourceRecord(&m->omsg, responseptr, &m->omsg.h.numAnswers, &rr->resrec);
			rr->resrec.rrclass &= ~kDNSClass_UniqueRRSet;			// Make sure to clear cache flush bit back to normal state
			if (!newptr && m->omsg.h.numAnswers) break;	// If packet full, send it now
			if (newptr) responseptr = newptr;
			ResponseRecords = rr->NextResponse;
			rr->NextResponse    = mDNSNULL;
			rr->NR_AnswerTo     = mDNSNULL;
			rr->NR_AdditionalTo = mDNSNULL;
			rr->RequireGoodbye  = mDNStrue;
			}
		
		// Add additionals, if there's space
		while (ResponseRecords && !ResponseRecords->NR_AnswerTo)
			{
			rr = ResponseRecords;
			if (rr->resrec.RecordType & kDNSRecordTypeUniqueMask)
				rr->resrec.rrclass |= kDNSClass_UniqueRRSet;		// Temporarily set the cache flush bit so PutResourceRecord will set it
			newptr = PutResourceRecord(&m->omsg, responseptr, &m->omsg.h.numAdditionals, &rr->resrec);
			rr->resrec.rrclass &= ~kDNSClass_UniqueRRSet;			// Make sure to clear cache flush bit back to normal state
			
			if (newptr) responseptr = newptr;
			if (newptr && m->omsg.h.numAnswers) rr->RequireGoodbye = mDNStrue;
			else if (rr->resrec.RecordType & kDNSRecordTypeUniqueMask) rr->ImmedAnswer = mDNSInterfaceMark;
			ResponseRecords = rr->NextResponse;
			rr->NextResponse    = mDNSNULL;
			rr->NR_AnswerTo     = mDNSNULL;
			rr->NR_AdditionalTo = mDNSNULL;
			}

		if (m->omsg.h.numAnswers) mDNSSendDNSMessage(m, &m->omsg, responseptr, mDNSInterface_Any, dest, MulticastDNSPort, mDNSNULL, mDNSNULL);
		}
	}

mDNSexport void CompleteDeregistration(mDNS *const m, AuthRecord *rr)
	{
	// Clearing rr->RequireGoodbye signals mDNS_Deregister_internal() that
	// it should go ahead and immediately dispose of this registration
	rr->resrec.RecordType = kDNSRecordTypeShared;
	rr->RequireGoodbye    = mDNSfalse;
	if (rr->LocalAnswer) { AnswerLocalQuestions(m, rr, mDNSfalse); rr->LocalAnswer = mDNSfalse; }
	mDNS_Deregister_internal(m, rr, mDNS_Dereg_normal);		// Don't touch rr after this
	}

// NOTE: DiscardDeregistrations calls mDNS_Deregister_internal which can call a user callback, which may change
// the record list and/or question list.
// Any code walking either list must use the CurrentQuestion and/or CurrentRecord mechanism to protect against this.
mDNSlocal void DiscardDeregistrations(mDNS *const m)
	{
	if (m->CurrentRecord)
		LogMsg("DiscardDeregistrations ERROR m->CurrentRecord already set %s", ARDisplayString(m, m->CurrentRecord));
	m->CurrentRecord = m->ResourceRecords;
	
	while (m->CurrentRecord)
		{
		AuthRecord *rr = m->CurrentRecord;
		if (rr->resrec.RecordType == kDNSRecordTypeDeregistering)
			CompleteDeregistration(m, rr);		// Don't touch rr after this
		else
			m->CurrentRecord = rr->next;
		}
	}

mDNSlocal void GrantUpdateCredit(AuthRecord *rr)
	{
	if (++rr->UpdateCredits >= kMaxUpdateCredits) rr->NextUpdateCredit = 0;
	else rr->NextUpdateCredit = NonZeroTime(rr->NextUpdateCredit + kUpdateCreditRefreshInterval);
	}

// Note about acceleration of announcements to facilitate automatic coalescing of
// multiple independent threads of announcements into a single synchronized thread:
// The announcements in the packet may be at different stages of maturity;
// One-second interval, two-second interval, four-second interval, and so on.
// After we've put in all the announcements that are due, we then consider
// whether there are other nearly-due announcements that are worth accelerating.
// To be eligible for acceleration, a record MUST NOT be older (further along
// its timeline) than the most mature record we've already put in the packet.
// In other words, younger records can have their timelines accelerated to catch up
// with their elder bretheren; this narrows the age gap and helps them eventually get in sync.
// Older records cannot have their timelines accelerated; this would just widen
// the gap between them and their younger bretheren and get them even more out of sync.

// NOTE: SendResponses calls mDNS_Deregister_internal which can call a user callback, which may change
// the record list and/or question list.
// Any code walking either list must use the CurrentQuestion and/or CurrentRecord mechanism to protect against this.
mDNSlocal void SendResponses(mDNS *const m)
	{
	int pktcount = 0;
	AuthRecord *rr, *r2;
	mDNSs32 maxExistingAnnounceInterval = 0;
	const NetworkInterfaceInfo *intf = GetFirstActiveInterface(m->HostInterfaces);

	m->NextScheduledResponse = m->timenow + 0x78000000;

	for (rr = m->ResourceRecords; rr; rr=rr->next)
		if (rr->ImmedUnicast)
			{
			mDNSAddr v4 = { mDNSAddrType_IPv4, {{{0}}} };
			mDNSAddr v6 = { mDNSAddrType_IPv6, {{{0}}} };
			v4.ip.v4 = rr->v4Requester;
			v6.ip.v6 = rr->v6Requester;
			if (!mDNSIPv4AddressIsZero(rr->v4Requester)) SendDelayedUnicastResponse(m, &v4, rr->ImmedAnswer);
			if (!mDNSIPv6AddressIsZero(rr->v6Requester)) SendDelayedUnicastResponse(m, &v6, rr->ImmedAnswer);
			if (rr->ImmedUnicast)
				{
				LogMsg("SendResponses: ERROR: rr->ImmedUnicast still set: %s", ARDisplayString(m, rr));
				rr->ImmedUnicast = mDNSfalse;
				}
			}

	// ***
	// *** 1. Setup: Set the SendRNow and ImmedAnswer fields to indicate which interface(s) the records need to be sent on
	// ***

	// Run through our list of records, and decide which ones we're going to announce on all interfaces
	for (rr = m->ResourceRecords; rr; rr=rr->next)
		{
		while (rr->NextUpdateCredit && m->timenow - rr->NextUpdateCredit >= 0) GrantUpdateCredit(rr);
		if (TimeToAnnounceThisRecord(rr, m->timenow) && ResourceRecordIsValidAnswer(rr))
			{
			rr->ImmedAnswer = mDNSInterfaceMark;		// Send on all interfaces
			if (maxExistingAnnounceInterval < rr->ThisAPInterval)
				maxExistingAnnounceInterval = rr->ThisAPInterval;
			if (rr->UpdateBlocked) rr->UpdateBlocked = 0;
			}
		}

	// Any interface-specific records we're going to send are marked as being sent on all appropriate interfaces (which is just one)
	// Eligible records that are more than half-way to their announcement time are accelerated
	for (rr = m->ResourceRecords; rr; rr=rr->next)
		if ((rr->resrec.InterfaceID && rr->ImmedAnswer) ||
			(rr->ThisAPInterval <= maxExistingAnnounceInterval &&
			TimeToAnnounceThisRecord(rr, m->timenow + rr->ThisAPInterval/2) &&
			ResourceRecordIsValidAnswer(rr)))
			rr->ImmedAnswer = mDNSInterfaceMark;		// Send on all interfaces

	// When sending SRV records (particularly when announcing a new service) automatically add related Address record(s) as additionals
	// NOTE: Currently all address records are interface-specific, so it's safe to set ImmedAdditional to their InterfaceID,
	// which will be non-null. If by some chance there is an address record that's not interface-specific (should never happen)
	// then all that means is that it won't get sent -- which would not be the end of the world.
	for (rr = m->ResourceRecords; rr; rr=rr->next)
		{
		if (rr->ImmedAnswer && rr->resrec.rrtype == kDNSType_SRV)
			for (r2=m->ResourceRecords; r2; r2=r2->next)				// Scan list of resource records
				if (RRTypeIsAddressType(r2->resrec.rrtype) &&			// For all address records (A/AAAA) ...
					ResourceRecordIsValidAnswer(r2) &&					// ... which are valid for answer ...
					rr->LastMCTime - r2->LastMCTime >= 0 &&				// ... which we have not sent recently ...
					rr->resrec.rdatahash == r2->resrec.namehash &&		// ... whose name is the name of the SRV target
					SameDomainName(&rr->resrec.rdata->u.srv.target, r2->resrec.name) &&
					(rr->ImmedAnswer == mDNSInterfaceMark || rr->ImmedAnswer == r2->resrec.InterfaceID))
					r2->ImmedAdditional = r2->resrec.InterfaceID;		// ... then mark this address record for sending too
		// We also make sure we send the DeviceInfo TXT record too, if necessary
		// We check for RecordType == kDNSRecordTypeShared because we don't want to tag the
		// DeviceInfo TXT record onto a goodbye packet (RecordType == kDNSRecordTypeDeregistering).
		if (rr->ImmedAnswer && rr->resrec.RecordType == kDNSRecordTypeShared && rr->resrec.rrtype == kDNSType_PTR)
			if (ResourceRecordIsValidAnswer(&m->DeviceInfo) && SameDomainLabel(rr->resrec.rdata->u.name.c, m->DeviceInfo.resrec.name->c))
				{
				if (!m->DeviceInfo.ImmedAnswer) m->DeviceInfo.ImmedAnswer = rr->ImmedAnswer;
				else                            m->DeviceInfo.ImmedAnswer = mDNSInterfaceMark;
				}
		}

	// If there's a record which is supposed to be unique that we're going to send, then make sure that we give
	// the whole RRSet as an atomic unit. That means that if we have any other records with the same name/type/class
	// then we need to mark them for sending too. Otherwise, if we set the kDNSClass_UniqueRRSet bit on a
	// record, then other RRSet members that have not been sent recently will get flushed out of client caches.
	// -- If a record is marked to be sent on a certain interface, make sure the whole set is marked to be sent on that interface
	// -- If any record is marked to be sent on all interfaces, make sure the whole set is marked to be sent on all interfaces
	for (rr = m->ResourceRecords; rr; rr=rr->next)
		if (rr->resrec.RecordType & kDNSRecordTypeUniqueMask)
			{
			if (rr->ImmedAnswer)			// If we're sending this as answer, see that its whole RRSet is similarly marked
				{
				for (r2 = m->ResourceRecords; r2; r2=r2->next)
					if (ResourceRecordIsValidAnswer(r2))
						if (r2->ImmedAnswer != mDNSInterfaceMark &&
							r2->ImmedAnswer != rr->ImmedAnswer && SameResourceRecordSignature(r2, rr))
							r2->ImmedAnswer = rr->ImmedAnswer;
				}
			else if (rr->ImmedAdditional)	// If we're sending this as additional, see that its whole RRSet is similarly marked
				{
				for (r2 = m->ResourceRecords; r2; r2=r2->next)
					if (ResourceRecordIsValidAnswer(r2))
						if (r2->ImmedAdditional != rr->ImmedAdditional && SameResourceRecordSignature(r2, rr))
							r2->ImmedAdditional = rr->ImmedAdditional;
				}
			}

	// Now set SendRNow state appropriately
	for (rr = m->ResourceRecords; rr; rr=rr->next)
		{
		if (rr->ImmedAnswer == mDNSInterfaceMark)		// Sending this record on all appropriate interfaces
			{
			rr->SendRNow = !intf ? mDNSNULL : (rr->resrec.InterfaceID) ? rr->resrec.InterfaceID : intf->InterfaceID;
			rr->ImmedAdditional = mDNSNULL;				// No need to send as additional if sending as answer
			rr->LastMCTime      = m->timenow;
			rr->LastMCInterface = rr->ImmedAnswer;
			// If we're announcing this record, and it's at least half-way to its ordained time, then consider this announcement done
			if (TimeToAnnounceThisRecord(rr, m->timenow + rr->ThisAPInterval/2))
				{
				rr->AnnounceCount--;
				rr->ThisAPInterval *= 2;
				rr->LastAPTime = m->timenow;
				debugf("Announcing %##s (%s) %d", rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype), rr->AnnounceCount);
				}
			}
		else if (rr->ImmedAnswer)						// Else, just respond to a single query on single interface:
			{
			rr->SendRNow        = rr->ImmedAnswer;		// Just respond on that interface
			rr->ImmedAdditional = mDNSNULL;				// No need to send as additional too
			rr->LastMCTime      = m->timenow;
			rr->LastMCInterface = rr->ImmedAnswer;
			}
		SetNextAnnounceProbeTime(m, rr);
		//if (rr->SendRNow) LogMsg("%-15.4a %s", &rr->v4Requester, ARDisplayString(m, rr));
		}

	// ***
	// *** 2. Loop through interface list, sending records as appropriate
	// ***

	while (intf)
		{
		int numDereg    = 0;
		int numAnnounce = 0;
		int numAnswer   = 0;
		mDNSu8 *responseptr = m->omsg.data;
		mDNSu8 *newptr;
		InitializeDNSMessage(&m->omsg.h, zeroID, ResponseFlags);
	
		// First Pass. Look for:
		// 1. Deregistering records that need to send their goodbye packet
		// 2. Updated records that need to retract their old data
		// 3. Answers and announcements we need to send
		// In all cases, if we fail, and we've put at least one answer, we break out of the for loop so we can
		// send this packet and then try again.
		// If we have not put even one answer, then we don't bail out. We pretend we succeeded anyway,
		// because otherwise we'll end up in an infinite loop trying to send a record that will never fit.
		for (rr = m->ResourceRecords; rr; rr=rr->next)
			if (rr->SendRNow == intf->InterfaceID)
				{
				if (rr->resrec.RecordType == kDNSRecordTypeDeregistering)
					{
					newptr = PutResourceRecordTTL(&m->omsg, responseptr, &m->omsg.h.numAnswers, &rr->resrec, 0);
					if (newptr) { responseptr = newptr; numDereg++; }
					else if (m->omsg.h.numAnswers) break;
					}
				else if (rr->NewRData && !m->SleepState)					// If we have new data for this record
					{
					RData *OldRData     = rr->resrec.rdata;
					mDNSu16 oldrdlength = rr->resrec.rdlength;
					// See if we should send a courtesy "goodbye" for the old data before we replace it.
					if (ResourceRecordIsValidAnswer(rr) && rr->RequireGoodbye)
						{
						newptr = PutResourceRecordTTL(&m->omsg, responseptr, &m->omsg.h.numAnswers, &rr->resrec, 0);
						if (newptr) { responseptr = newptr; numDereg++; rr->RequireGoodbye = mDNSfalse; }
						else if (m->omsg.h.numAnswers) break;
						}
					// Now try to see if we can fit the update in the same packet (not fatal if we can't)
					SetNewRData(&rr->resrec, rr->NewRData, rr->newrdlength);
					if (rr->resrec.RecordType & kDNSRecordTypeUniqueMask)
						rr->resrec.rrclass |= kDNSClass_UniqueRRSet;		// Temporarily set the cache flush bit so PutResourceRecord will set it
					newptr = PutResourceRecord(&m->omsg, responseptr, &m->omsg.h.numAnswers, &rr->resrec);
					rr->resrec.rrclass &= ~kDNSClass_UniqueRRSet;			// Make sure to clear cache flush bit back to normal state
					if (newptr) { responseptr = newptr; rr->RequireGoodbye = mDNStrue; }
					SetNewRData(&rr->resrec, OldRData, oldrdlength);
					}
				else
					{
					if (rr->resrec.RecordType & kDNSRecordTypeUniqueMask)
						rr->resrec.rrclass |= kDNSClass_UniqueRRSet;		// Temporarily set the cache flush bit so PutResourceRecord will set it
					newptr = PutResourceRecordTTL(&m->omsg, responseptr, &m->omsg.h.numAnswers, &rr->resrec, m->SleepState ? 0 : rr->resrec.rroriginalttl);
					rr->resrec.rrclass &= ~kDNSClass_UniqueRRSet;			// Make sure to clear cache flush bit back to normal state
					if (newptr)
						{
						responseptr = newptr;
						rr->RequireGoodbye = (mDNSu8) (!m->SleepState);
						if (rr->LastAPTime == m->timenow) numAnnounce++; else numAnswer++;
						}
					else if (m->omsg.h.numAnswers) break;
					}
				// If sending on all interfaces, go to next interface; else we're finished now
				if (rr->ImmedAnswer == mDNSInterfaceMark && rr->resrec.InterfaceID == mDNSInterface_Any)
					rr->SendRNow = GetNextActiveInterfaceID(intf);
				else
					rr->SendRNow = mDNSNULL;
				}
	
		// Second Pass. Add additional records, if there's space.
		newptr = responseptr;
		for (rr = m->ResourceRecords; rr; rr=rr->next)
			if (rr->ImmedAdditional == intf->InterfaceID)
				if (ResourceRecordIsValidAnswer(rr))
					{
					// If we have at least one answer already in the packet, then plan to add additionals too
					mDNSBool SendAdditional = (m->omsg.h.numAnswers > 0);
					
					// If we're not planning to send any additionals, but this record is a unique one, then
					// make sure we haven't already sent any other members of its RRSet -- if we have, then they
					// will have had the cache flush bit set, so now we need to finish the job and send the rest.
					if (!SendAdditional && (rr->resrec.RecordType & kDNSRecordTypeUniqueMask))
						{
						const AuthRecord *a;
						for (a = m->ResourceRecords; a; a=a->next)
							if (a->LastMCTime      == m->timenow &&
								a->LastMCInterface == intf->InterfaceID &&
								SameResourceRecordSignature(a, rr)) { SendAdditional = mDNStrue; break; }
						}
					if (!SendAdditional)					// If we don't want to send this after all,
						rr->ImmedAdditional = mDNSNULL;		// then cancel its ImmedAdditional field
					else if (newptr)						// Else, try to add it if we can
						{
						if (rr->resrec.RecordType & kDNSRecordTypeUniqueMask)
							rr->resrec.rrclass |= kDNSClass_UniqueRRSet;	// Temporarily set the cache flush bit so PutResourceRecord will set it
						newptr = PutResourceRecord(&m->omsg, newptr, &m->omsg.h.numAdditionals, &rr->resrec);
						rr->resrec.rrclass &= ~kDNSClass_UniqueRRSet;		// Make sure to clear cache flush bit back to normal state
						if (newptr)
							{
							responseptr = newptr;
							rr->ImmedAdditional = mDNSNULL;
							rr->RequireGoodbye = mDNStrue;
							// If we successfully put this additional record in the packet, we record LastMCTime & LastMCInterface.
							// This matters particularly in the case where we have more than one IPv6 (or IPv4) address, because otherwise,
							// when we see our own multicast with the cache flush bit set, if we haven't set LastMCTime, then we'll get
							// all concerned and re-announce our record again to make sure it doesn't get flushed from peer caches.
							rr->LastMCTime      = m->timenow;
							rr->LastMCInterface = intf->InterfaceID;
							}
						}
					}
	
		if (m->omsg.h.numAnswers > 0 || m->omsg.h.numAdditionals)
			{
			debugf("SendResponses: Sending %d Deregistration%s, %d Announcement%s, %d Answer%s, %d Additional%s on %p",
				numDereg,                  numDereg                  == 1 ? "" : "s",
				numAnnounce,               numAnnounce               == 1 ? "" : "s",
				numAnswer,                 numAnswer                 == 1 ? "" : "s",
				m->omsg.h.numAdditionals, m->omsg.h.numAdditionals == 1 ? "" : "s", intf->InterfaceID);
			if (intf->IPv4Available) mDNSSendDNSMessage(m, &m->omsg, responseptr, intf->InterfaceID, &AllDNSLinkGroup_v4, MulticastDNSPort, mDNSNULL, mDNSNULL);
			if (intf->IPv6Available) mDNSSendDNSMessage(m, &m->omsg, responseptr, intf->InterfaceID, &AllDNSLinkGroup_v6, MulticastDNSPort, mDNSNULL, mDNSNULL);
			if (!m->SuppressSending) m->SuppressSending = NonZeroTime(m->timenow + (mDNSPlatformOneSecond+9)/10);
			if (++pktcount >= 1000) { LogMsg("SendResponses exceeded loop limit %d: giving up", pktcount); break; }
			// There might be more things to send on this interface, so go around one more time and try again.
			}
		else	// Nothing more to send on this interface; go to next
			{
			const NetworkInterfaceInfo *next = GetFirstActiveInterface(intf->next);
			#if MDNS_DEBUGMSGS && 0
			const char *const msg = next ? "SendResponses: Nothing more on %p; moving to %p" : "SendResponses: Nothing more on %p";
			debugf(msg, intf, next);
			#endif
			intf = next;
			}
		}

	// ***
	// *** 3. Cleanup: Now that everything is sent, call client callback functions, and reset state variables
	// ***

	if (m->CurrentRecord)
		LogMsg("SendResponses ERROR m->CurrentRecord already set %s", ARDisplayString(m, m->CurrentRecord));
	m->CurrentRecord = m->ResourceRecords;
	while (m->CurrentRecord)
		{
		rr = m->CurrentRecord;
		m->CurrentRecord = rr->next;

		if (rr->SendRNow)
			{
			if (rr->resrec.InterfaceID != mDNSInterface_LocalOnly)
				LogMsg("SendResponses: No active interface to send: %s", ARDisplayString(m, rr));
			rr->SendRNow = mDNSNULL;
			}

		if (rr->ImmedAnswer)
			{
			if (rr->NewRData) CompleteRDataUpdate(m,rr);	// Update our rdata, clear the NewRData pointer, and return memory to the client
	
			if (rr->resrec.RecordType == kDNSRecordTypeDeregistering)
				CompleteDeregistration(m, rr);		// Don't touch rr after this
			else
				{
				rr->ImmedAnswer  = mDNSNULL;
				rr->ImmedUnicast = mDNSfalse;
				rr->v4Requester  = zerov4Addr;
				rr->v6Requester  = zerov6Addr;
				}
			}
		}
	verbosedebugf("SendResponses: Next in %ld ticks", m->NextScheduledResponse - m->timenow);
	}

// Calling CheckCacheExpiration() is an expensive operation because it has to look at the entire cache,
// so we want to be lazy about how frequently we do it.
// 1. If a cache record is currently referenced by *no* active questions,
//    then we don't mind expiring it up to a minute late (who will know?)
// 2. Else, if a cache record is due for some of its final expiration queries,
//    we'll allow them to be late by up to 2% of the TTL
// 3. Else, if a cache record has completed all its final expiration queries without success,
//    and is expiring, and had an original TTL more than ten seconds, we'll allow it to be one second late
// 4. Else, it is expiring and had an original TTL of ten seconds or less (includes explicit goodbye packets),
//    so allow at most 1/10 second lateness
#define CacheCheckGracePeriod(RR) (                                                   \
	((RR)->DelayDelivery                           ) ? (mDNSPlatformOneSecond/10)   : \
	((RR)->CRActiveQuestion == mDNSNULL            ) ? (60 * mDNSPlatformOneSecond) : \
	((RR)->UnansweredQueries < MaxUnansweredQueries) ? (TicksTTL(rr)/50)            : \
	((RR)->resrec.rroriginalttl > 10               ) ? (mDNSPlatformOneSecond)      : (mDNSPlatformOneSecond/10))

// Note: MUST call SetNextCacheCheckTime any time we change:
// rr->TimeRcvd
// rr->resrec.rroriginalttl
// rr->UnansweredQueries
// rr->CRActiveQuestion
// Also, any time we set rr->DelayDelivery we should call SetNextCacheCheckTime to ensure m->NextCacheCheck is set if necessary
// Clearing rr->DelayDelivery does not require a call to SetNextCacheCheckTime
mDNSlocal void SetNextCacheCheckTime(mDNS *const m, CacheRecord *const rr)
	{
	rr->NextRequiredQuery = RRExpireTime(rr);

	// If we have an active question, then see if we want to schedule a refresher query for this record.
	// Usually we expect to do four queries, at 80-82%, 85-87%, 90-92% and then 95-97% of the TTL.
	if (rr->CRActiveQuestion && rr->UnansweredQueries < MaxUnansweredQueries)
		{
		rr->NextRequiredQuery -= TicksTTL(rr)/20 * (MaxUnansweredQueries - rr->UnansweredQueries);
		rr->NextRequiredQuery += mDNSRandom((mDNSu32)TicksTTL(rr)/50);
		verbosedebugf("SetNextCacheCheckTime: %##s (%s) NextRequiredQuery in %ld sec CacheCheckGracePeriod %d ticks",
			rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype),
			(rr->NextRequiredQuery - m->timenow) / mDNSPlatformOneSecond, CacheCheckGracePeriod(rr));
		}

	if (m->NextCacheCheck - (rr->NextRequiredQuery + CacheCheckGracePeriod(rr)) > 0)
		m->NextCacheCheck = (rr->NextRequiredQuery + CacheCheckGracePeriod(rr));
	
	if (rr->DelayDelivery)
		if (m->NextCacheCheck - rr->DelayDelivery > 0)
			m->NextCacheCheck = rr->DelayDelivery;
	}

#define kMinimumReconfirmTime                     ((mDNSu32)mDNSPlatformOneSecond *  5)
#define kDefaultReconfirmTimeForWake              ((mDNSu32)mDNSPlatformOneSecond *  5)
#define kDefaultReconfirmTimeForNoAnswer          ((mDNSu32)mDNSPlatformOneSecond *  5)
#define kDefaultReconfirmTimeForFlappingInterface ((mDNSu32)mDNSPlatformOneSecond * 30)

mDNSlocal mStatus mDNS_Reconfirm_internal(mDNS *const m, CacheRecord *const rr, mDNSu32 interval)
	{
	if (interval < kMinimumReconfirmTime)
		interval = kMinimumReconfirmTime;
	if (interval > 0x10000000)	// Make sure interval doesn't overflow when we multiply by four below
		interval = 0x10000000;

	// If the expected expiration time for this record is more than interval+33%, then accelerate its expiration
	if (RRExpireTime(rr) - m->timenow > (mDNSs32)((interval * 4) / 3))
		{
		// Add a 33% random amount to the interval, to avoid synchronization between multiple hosts
		// For all the reconfirmations in a given batch, we want to use the same random value
		// so that the reconfirmation questions can be grouped into a single query packet
		if (!m->RandomReconfirmDelay) m->RandomReconfirmDelay = 1 + mDNSRandom(0x3FFFFFFF);
		interval += mDNSRandomFromFixedSeed(m->RandomReconfirmDelay, interval/3);
		rr->TimeRcvd          = m->timenow - (mDNSs32)interval * 3;
		rr->resrec.rroriginalttl     = (interval * 4 + mDNSPlatformOneSecond - 1) / mDNSPlatformOneSecond;
		SetNextCacheCheckTime(m, rr);
		}
	debugf("mDNS_Reconfirm_internal:%6ld ticks to go for %s %p",
		RRExpireTime(rr) - m->timenow, CRDisplayString(m, rr), rr->CRActiveQuestion);
	return(mStatus_NoError);
	}

#define MaxQuestionInterval         (3600 * mDNSPlatformOneSecond)

// BuildQuestion puts a question into a DNS Query packet and if successful, updates the value of queryptr.
// It also appends to the list of known answer records that need to be included,
// and updates the forcast for the size of the known answer section.
mDNSlocal mDNSBool BuildQuestion(mDNS *const m, DNSMessage *query, mDNSu8 **queryptr, DNSQuestion *q,
	CacheRecord ***kalistptrptr, mDNSu32 *answerforecast)
	{
	mDNSBool ucast = (q->LargeAnswers || q->RequestUnicast) && m->CanReceiveUnicastOn5353;
	mDNSu16 ucbit = (mDNSu16)(ucast ? kDNSQClass_UnicastResponse : 0);
	const mDNSu8 *const limit = query->data + NormalMaxDNSMessageData;
	mDNSu8 *newptr = putQuestion(query, *queryptr, limit, &q->qname, q->qtype, (mDNSu16)(q->qclass | ucbit));
	if (!newptr)
		{
		debugf("BuildQuestion: No more space in this packet for question %##s (%s)", q->qname.c, DNSTypeName(q->qtype));
		return(mDNSfalse);
		}
	else if (newptr + *answerforecast >= limit)
		{
		verbosedebugf("BuildQuestion: Retracting question %##s (%s) new forecast total %d",
			q->qname.c, DNSTypeName(q->qtype), newptr + *answerforecast - query->data);
		query->h.numQuestions--;
		return(mDNSfalse);
		}
	else
		{
		mDNSu32 forecast = *answerforecast;
		const mDNSu32 slot = HashSlot(&q->qname);
		const CacheGroup *const cg = CacheGroupForName(m, slot, q->qnamehash, &q->qname);
		CacheRecord *rr;
		CacheRecord **ka = *kalistptrptr;	// Make a working copy of the pointer we're going to update

		for (rr = cg ? cg->members : mDNSNULL; rr; rr=rr->next)				// If we have a resource record in our cache,
			if (rr->resrec.InterfaceID == q->SendQNow &&					// received on this interface
				rr->NextInKAList == mDNSNULL && ka != &rr->NextInKAList &&	// which is not already in the known answer list
				rr->resrec.rdlength <= SmallRecordLimit &&					// which is small enough to sensibly fit in the packet
				SameNameRecordAnswersQuestion(&rr->resrec, q) &&			// which answers our question
				rr->TimeRcvd + TicksTTL(rr)/2 - m->timenow >				// and its half-way-to-expiry time is at least 1 second away
												mDNSPlatformOneSecond)		// (also ensures we never include goodbye records with TTL=1)
				{
				*ka = rr;	// Link this record into our known answer chain
				ka = &rr->NextInKAList;
				// We forecast: compressed name (2) type (2) class (2) TTL (4) rdlength (2) rdata (n)
				forecast += 12 + rr->resrec.rdestimate;
				// If we're trying to put more than one question in this packet, and it doesn't fit
				// then undo that last question and try again next time
				if (query->h.numQuestions > 1 && newptr + forecast >= limit)
					{
					debugf("BuildQuestion: Retracting question %##s (%s) new forecast total %d",
						q->qname.c, DNSTypeName(q->qtype), newptr + forecast - query->data);
					query->h.numQuestions--;
					ka = *kalistptrptr;		// Go back to where we started and retract these answer records
					while (*ka) { CacheRecord *rr = *ka; *ka = mDNSNULL; ka = &rr->NextInKAList; }
					return(mDNSfalse);		// Return false, so we'll try again in the next packet
					}
				}

		// Success! Update our state pointers, increment UnansweredQueries as appropriate, and return
		*queryptr        = newptr;				// Update the packet pointer
		*answerforecast  = forecast;			// Update the forecast
		*kalistptrptr    = ka;					// Update the known answer list pointer
		if (ucast) q->ExpectUnicastResp = NonZeroTime(m->timenow);

		for (rr = cg ? cg->members : mDNSNULL; rr; rr=rr->next)				// For every resource record in our cache,
			if (rr->resrec.InterfaceID == q->SendQNow &&					// received on this interface
				rr->NextInKAList == mDNSNULL && ka != &rr->NextInKAList &&	// which is not in the known answer list
				SameNameRecordAnswersQuestion(&rr->resrec, q))				// which answers our question
					{
					rr->UnansweredQueries++;								// indicate that we're expecting a response
					rr->LastUnansweredTime = m->timenow;
					SetNextCacheCheckTime(m, rr);
					}

		return(mDNStrue);
		}
	}

// When we have a query looking for a specified name, but there appear to be no answers with
// that name, ReconfirmAntecedents() is called with depth=0 to start the reconfirmation process
// for any records in our cache that reference the given name (e.g. PTR and SRV records).
// For any such cache record we find, we also recursively call ReconfirmAntecedents() for *its* name.
// We increment depth each time we recurse, to guard against possible infinite loops, with a limit of 5.
// A typical reconfirmation scenario might go like this:
// Depth 0: Name "myhost.local" has no address records
// Depth 1: SRV "My Service._example._tcp.local." refers to "myhost.local"; may be stale
// Depth 2: PTR "_example._tcp.local." refers to "My Service"; may be stale
// Depth 3: PTR "_services._dns-sd._udp.local." refers to "_example._tcp.local."; may be stale
// Currently depths 4 and 5 are not expected to occur; if we did get to depth 5 we'd reconfim any records we
// found referring to the given name, but not recursively descend any further reconfirm *their* antecedents.
mDNSlocal void ReconfirmAntecedents(mDNS *const m, const domainname *const name, const mDNSu32 namehash, const int depth)
	{
	mDNSu32 slot;
	CacheGroup *cg;
	CacheRecord *cr;
	debugf("ReconfirmAntecedents (depth=%d) for %##s", depth, name->c);
	FORALL_CACHERECORDS(slot, cg, cr)
		{
		domainname *crtarget = GetRRDomainNameTarget(&cr->resrec);
		if (crtarget && cr->resrec.rdatahash == namehash && SameDomainName(crtarget, name))
			{
			LogOperation("ReconfirmAntecedents: Reconfirming (depth=%d) %s", depth, CRDisplayString(m, cr));
			mDNS_Reconfirm_internal(m, cr, kDefaultReconfirmTimeForNoAnswer);
			if (depth < 5) ReconfirmAntecedents(m, cr->resrec.name, cr->resrec.namehash, depth+1);
			}
		}
	}

// Only DupSuppressInfos newer than the specified 'time' are allowed to remain active
mDNSlocal void ExpireDupSuppressInfo(DupSuppressInfo ds[DupSuppressInfoSize], mDNSs32 time)
	{
	int i;
	for (i=0; i<DupSuppressInfoSize; i++) if (ds[i].Time - time < 0) ds[i].InterfaceID = mDNSNULL;
	}

mDNSlocal void ExpireDupSuppressInfoOnInterface(DupSuppressInfo ds[DupSuppressInfoSize], mDNSs32 time, mDNSInterfaceID InterfaceID)
	{
	int i;
	for (i=0; i<DupSuppressInfoSize; i++) if (ds[i].InterfaceID == InterfaceID && ds[i].Time - time < 0) ds[i].InterfaceID = mDNSNULL;
	}

mDNSlocal mDNSBool SuppressOnThisInterface(const DupSuppressInfo ds[DupSuppressInfoSize], const NetworkInterfaceInfo * const intf)
	{
	int i;
	mDNSBool v4 = !intf->IPv4Available;		// If this interface doesn't do v4, we don't need to find a v4 duplicate of this query
	mDNSBool v6 = !intf->IPv6Available;		// If this interface doesn't do v6, we don't need to find a v6 duplicate of this query
	for (i=0; i<DupSuppressInfoSize; i++)
		if (ds[i].InterfaceID == intf->InterfaceID)
			{
			if      (ds[i].Type == mDNSAddrType_IPv4) v4 = mDNStrue;
			else if (ds[i].Type == mDNSAddrType_IPv6) v6 = mDNStrue;
			if (v4 && v6) return(mDNStrue);
			}
	return(mDNSfalse);
	}

mDNSlocal int RecordDupSuppressInfo(DupSuppressInfo ds[DupSuppressInfoSize], mDNSs32 Time, mDNSInterfaceID InterfaceID, mDNSs32 Type)
	{
	int i, j;

	// See if we have this one in our list somewhere already
	for (i=0; i<DupSuppressInfoSize; i++) if (ds[i].InterfaceID == InterfaceID && ds[i].Type == Type) break;

	// If not, find a slot we can re-use
	if (i >= DupSuppressInfoSize)
		{
		i = 0;
		for (j=1; j<DupSuppressInfoSize && ds[i].InterfaceID; j++)
			if (!ds[j].InterfaceID || ds[j].Time - ds[i].Time < 0)
				i = j;
		}
	
	// Record the info about this query we saw
	ds[i].Time        = Time;
	ds[i].InterfaceID = InterfaceID;
	ds[i].Type        = Type;
	
	return(i);
	}

mDNSlocal mDNSBool AccelerateThisQuery(mDNS *const m, DNSQuestion *q)
	{
	// If more than 90% of the way to the query time, we should unconditionally accelerate it
	if (TimeToSendThisQuestion(q, m->timenow + q->ThisQInterval/10))
		return(mDNStrue);

	// If half-way to next scheduled query time, only accelerate if it will add less than 512 bytes to the packet
	if (TimeToSendThisQuestion(q, m->timenow + q->ThisQInterval/2))
		{
		// We forecast: qname (n) type (2) class (2)
		mDNSu32 forecast = (mDNSu32)DomainNameLength(&q->qname) + 4;
		const mDNSu32 slot = HashSlot(&q->qname);
		const CacheGroup *const cg = CacheGroupForName(m, slot, q->qnamehash, &q->qname);
		CacheRecord *rr;
		for (rr = cg ? cg->members : mDNSNULL; rr; rr=rr->next)				// If we have a resource record in our cache,
			if (rr->resrec.rdlength <= SmallRecordLimit &&					// which is small enough to sensibly fit in the packet
				SameNameRecordAnswersQuestion(&rr->resrec, q) &&			// which answers our question
				rr->TimeRcvd + TicksTTL(rr)/2 - m->timenow >= 0 &&			// and it is less than half-way to expiry
				rr->NextRequiredQuery - (m->timenow + q->ThisQInterval) > 0)// and we'll ask at least once again before NextRequiredQuery
				{
				// We forecast: compressed name (2) type (2) class (2) TTL (4) rdlength (2) rdata (n)
				forecast += 12 + rr->resrec.rdestimate;
				if (forecast >= 512) return(mDNSfalse);	// If this would add 512 bytes or more to the packet, don't accelerate
				}
		return(mDNStrue);
		}

	return(mDNSfalse);
	}

// How Standard Queries are generated:
// 1. The Question Section contains the question
// 2. The Additional Section contains answers we already know, to suppress duplicate responses

// How Probe Queries are generated:
// 1. The Question Section contains queries for the name we intend to use, with QType=ANY because
// if some other host is already using *any* records with this name, we want to know about it.
// 2. The Authority Section contains the proposed values we intend to use for one or more
// of our records with that name (analogous to the Update section of DNS Update packets)
// because if some other host is probing at the same time, we each want to know what the other is
// planning, in order to apply the tie-breaking rule to see who gets to use the name and who doesn't.

mDNSlocal void SendQueries(mDNS *const m)
	{
	mDNSu32 slot;
	CacheGroup *cg;
	CacheRecord *cr;
	AuthRecord *ar;
	int pktcount = 0;
	DNSQuestion *q;
	// For explanation of maxExistingQuestionInterval logic, see comments for maxExistingAnnounceInterval
	mDNSs32 maxExistingQuestionInterval = 0;
	const NetworkInterfaceInfo *intf = GetFirstActiveInterface(m->HostInterfaces);
	CacheRecord *KnownAnswerList = mDNSNULL;

	// 1. If time for a query, work out what we need to do
	if (m->timenow - m->NextScheduledQuery >= 0)
		{
		CacheRecord *rr;

		// We're expecting to send a query anyway, so see if any expiring cache records are close enough
		// to their NextRequiredQuery to be worth batching them together with this one
		FORALL_CACHERECORDS(slot, cg, rr)
			if (rr->CRActiveQuestion && rr->UnansweredQueries < MaxUnansweredQueries)
				if (m->timenow + TicksTTL(rr)/50 - rr->NextRequiredQuery >= 0)
					{
					q = rr->CRActiveQuestion;
					ExpireDupSuppressInfoOnInterface(q->DupSuppress, m->timenow - TicksTTL(rr)/20, rr->resrec.InterfaceID);
					if      (q->Target.type) q->SendQNow = mDNSInterfaceMark;	// If targeted query, mark it
					else if (!mDNSOpaque16IsZero(q->TargetQID))     q->LastQTime = m->timenow - q->ThisQInterval;	// For uDNS, adjust LastQTime
					else if (q->SendQNow == mDNSNULL)               q->SendQNow = rr->resrec.InterfaceID;
					else if (q->SendQNow != rr->resrec.InterfaceID) q->SendQNow = mDNSInterfaceMark;
					}

		if (m->SuppressStdPort53Queries && m->timenow - m->SuppressStdPort53Queries >= 0)
			m->SuppressStdPort53Queries = 0; // If suppression time has passed, clear it
			
		// Scan our list of questions to see which:
		//     *WideArea*  queries need to be sent
		//     *unicast*   queries need to be sent
		//     *multicast* queries we're definitely going to send
		if (m->CurrentQuestion)
			LogMsg("SendQueries ERROR m->CurrentQuestion already set: %##s (%s)", m->CurrentQuestion->qname.c, DNSTypeName(m->CurrentQuestion->qtype));
		m->CurrentQuestion = m->Questions;
		while (m->CurrentQuestion)
			{
			q = m->CurrentQuestion;
			if (ActiveQuestion(q) && !mDNSOpaque16IsZero(q->TargetQID)) uDNS_CheckCurrentQuestion(m);
			else if (mDNSOpaque16IsZero(q->TargetQID) && q->Target.type && (q->SendQNow || TimeToSendThisQuestion(q, m->timenow)))
				{
				mDNSu8       *qptr        = m->omsg.data;
				const mDNSu8 *const limit = m->omsg.data + sizeof(m->omsg.data);
				InitializeDNSMessage(&m->omsg.h, q->TargetQID, QueryFlags);
				qptr = putQuestion(&m->omsg, qptr, limit, &q->qname, q->qtype, q->qclass);
				mDNSSendDNSMessage(m, &m->omsg, qptr, mDNSInterface_Any, &q->Target, q->TargetPort, mDNSNULL, mDNSNULL);
				q->ThisQInterval    *= QuestionIntervalStep;
				if (q->ThisQInterval > MaxQuestionInterval)
					q->ThisQInterval = MaxQuestionInterval;
				q->LastQTime         = m->timenow;
				q->LastQTxTime       = m->timenow;
				q->RecentAnswerPkts  = 0;
				q->SendQNow          = mDNSNULL;
				q->ExpectUnicastResp = NonZeroTime(m->timenow);
				}
			else if (mDNSOpaque16IsZero(q->TargetQID) && !q->Target.type && TimeToSendThisQuestion(q, m->timenow))
				{
				//LogOperation("Time to send %##s (%s) %d", q->qname.c, DNSTypeName(q->qtype), m->timenow - (q->LastQTime + q->ThisQInterval));
				q->SendQNow = mDNSInterfaceMark;		// Mark this question for sending on all interfaces
				if (maxExistingQuestionInterval < q->ThisQInterval)
					maxExistingQuestionInterval = q->ThisQInterval;
				}
			// If m->CurrentQuestion wasn't modified out from under us, advance it now
			// We can't do this at the start of the loop because uDNS_CheckCurrentQuestion() depends on having
			// m->CurrentQuestion point to the right question
			if (q == m->CurrentQuestion) m->CurrentQuestion = m->CurrentQuestion->next;
			}

		// Scan our list of questions
		// (a) to see if there are any more that are worth accelerating, and
		// (b) to update the state variables for *all* the questions we're going to send
		// Note: Don't set NextScheduledQuery until here, because uDNS_CheckCurrentQuestion in the loop above can add new questions to the list,
		// which causes NextScheduledQuery to get (incorrectly) set to m->timenow. Setting it here is the right place, because the very
		// next thing we do is scan the list and call SetNextQueryTime() for every question we find, so we know we end up with the right value.
		m->NextScheduledQuery = m->timenow + 0x78000000;
		for (q = m->Questions; q; q=q->next)
			{
			if (mDNSOpaque16IsZero(q->TargetQID) && (q->SendQNow ||
				(!q->Target.type && ActiveQuestion(q) && q->ThisQInterval <= maxExistingQuestionInterval && AccelerateThisQuery(m,q))))
				{
				// If at least halfway to next query time, advance to next interval
				// If less than halfway to next query time, then
				// treat this as logically a repeat of the last transmission, without advancing the interval
				if (m->timenow - (q->LastQTime + q->ThisQInterval/2) >= 0)
					{
					//LogOperation("Accelerating %##s (%s) %d", q->qname.c, DNSTypeName(q->qtype), m->timenow - (q->LastQTime + q->ThisQInterval));
					q->SendQNow = mDNSInterfaceMark;	// Mark this question for sending on all interfaces
					q->ThisQInterval *= QuestionIntervalStep;
					if (q->ThisQInterval > MaxQuestionInterval)
						q->ThisQInterval = MaxQuestionInterval;
					else if (q->CurrentAnswers == 0 && q->ThisQInterval == InitialQuestionInterval * QuestionIntervalStep2)
						{
						// Generally don't need to log this.
						// It's not especially noteworthy if a query finds no results -- this usually happens for domain
						// enumeration queries in the LL subdomain (e.g. "db._dns-sd._udp.0.0.254.169.in-addr.arpa")
						// and when there simply happen to be no instances of the service the client is looking
						// for (e.g. iTunes is set to look for RAOP devices, and the current network has none).
						debugf("SendQueries: Zero current answers for %##s (%s); will reconfirm antecedents",
							q->qname.c, DNSTypeName(q->qtype));
						// Sending third query, and no answers yet; time to begin doubting the source
						ReconfirmAntecedents(m, &q->qname, q->qnamehash, 0);
						}
					}

				// Mark for sending. (If no active interfaces, then don't even try.)
				q->SendOnAll = (q->SendQNow == mDNSInterfaceMark);
				if (q->SendOnAll)
					{
					q->SendQNow  = !intf ? mDNSNULL : (q->InterfaceID) ? q->InterfaceID : intf->InterfaceID;
					q->LastQTime = m->timenow;
					}

				// If we recorded a duplicate suppression for this question less than half an interval ago,
				// then we consider it recent enough that we don't need to do an identical query ourselves.
				ExpireDupSuppressInfo(q->DupSuppress, m->timenow - q->ThisQInterval/2);

				q->LastQTxTime      = m->timenow;
				q->RecentAnswerPkts = 0;
				if (q->RequestUnicast) q->RequestUnicast--;
				}
			// For all questions (not just the ones we're sending) check what the next scheduled event will be
			SetNextQueryTime(m,q);
			}
		}

	// 2. Scan our authoritative RR list to see what probes we might need to send
	if (m->timenow - m->NextScheduledProbe >= 0)
		{
		m->NextScheduledProbe = m->timenow + 0x78000000;

		if (m->CurrentRecord)
			LogMsg("SendQueries ERROR m->CurrentRecord already set %s", ARDisplayString(m, m->CurrentRecord));
		m->CurrentRecord = m->ResourceRecords;
		while (m->CurrentRecord)
			{
			AuthRecord *rr = m->CurrentRecord;
			m->CurrentRecord = rr->next;
			if (rr->resrec.RecordType == kDNSRecordTypeUnique)			// For all records that are still probing...
				{
				// 1. If it's not reached its probe time, just make sure we update m->NextScheduledProbe correctly
				if (m->timenow - (rr->LastAPTime + rr->ThisAPInterval) < 0)
					{
					SetNextAnnounceProbeTime(m, rr);
					}
				// 2. else, if it has reached its probe time, mark it for sending and then update m->NextScheduledProbe correctly
				else if (rr->ProbeCount)
					{
					// Mark for sending. (If no active interfaces, then don't even try.)
					rr->SendRNow   = !intf ? mDNSNULL : (rr->resrec.InterfaceID) ? rr->resrec.InterfaceID : intf->InterfaceID;
					rr->LastAPTime = m->timenow;
					rr->ProbeCount--;
					SetNextAnnounceProbeTime(m, rr);
					if (rr->ProbeCount == 0)
						{
						// If this is the last probe for this record, then see if we have any matching records
						// on our duplicate list which should similarly have their ProbeCount cleared to zero...
						AuthRecord *r2;
						for (r2 = m->DuplicateRecords; r2; r2=r2->next)
							if (r2->resrec.RecordType == kDNSRecordTypeUnique && RecordIsLocalDuplicate(r2, rr))
								r2->ProbeCount = 0;
						// ... then acknowledge this record to the client.
						// We do this optimistically, just as we're about to send the third probe.
						// This helps clients that both advertise and browse, and want to filter themselves
						// from the browse results list, because it helps ensure that the registration
						// confirmation will be delivered 1/4 second *before* the browse "add" event.
						// A potential downside is that we could deliver a registration confirmation and then find out
						// moments later that there's a name conflict, but applications have to be prepared to handle
						// late conflicts anyway (e.g. on connection of network cable, etc.), so this is nothing new.
						if (!rr->Acknowledged) AcknowledgeRecord(m, rr);
						}
					}
				// else, if it has now finished probing, move it to state Verified,
				// and update m->NextScheduledResponse so it will be announced
				else
					{
					if (!rr->Acknowledged) AcknowledgeRecord(m, rr);	// Defensive, just in case it got missed somehow
					rr->resrec.RecordType     = kDNSRecordTypeVerified;
					rr->ThisAPInterval = DefaultAnnounceIntervalForTypeUnique;
					rr->LastAPTime     = m->timenow - DefaultAnnounceIntervalForTypeUnique;
					SetNextAnnounceProbeTime(m, rr);
					}
				}
			}
		m->CurrentRecord = m->DuplicateRecords;
		while (m->CurrentRecord)
			{
			AuthRecord *rr = m->CurrentRecord;
			m->CurrentRecord = rr->next;
			if (rr->resrec.RecordType == kDNSRecordTypeUnique && rr->ProbeCount == 0 && !rr->Acknowledged)
				AcknowledgeRecord(m, rr);
			}
		}

	// 3. Now we know which queries and probes we're sending,
	// go through our interface list sending the appropriate queries on each interface
	while (intf)
		{
		AuthRecord *rr;
		mDNSu8 *queryptr = m->omsg.data;
		InitializeDNSMessage(&m->omsg.h, zeroID, QueryFlags);
		if (KnownAnswerList) verbosedebugf("SendQueries:   KnownAnswerList set... Will continue from previous packet");
		if (!KnownAnswerList)
			{
			// Start a new known-answer list
			CacheRecord **kalistptr = &KnownAnswerList;
			mDNSu32 answerforecast = 0;
			
			// Put query questions in this packet
			for (q = m->Questions; q; q=q->next)
				{
				if (mDNSOpaque16IsZero(q->TargetQID) && (q->SendQNow == intf->InterfaceID))
					{
					debugf("SendQueries: %s question for %##s (%s) at %d forecast total %d",
						SuppressOnThisInterface(q->DupSuppress, intf) ? "Suppressing" : "Putting    ",
						q->qname.c, DNSTypeName(q->qtype), queryptr - m->omsg.data, queryptr + answerforecast - m->omsg.data);
					// If we're suppressing this question, or we successfully put it, update its SendQNow state
					if (SuppressOnThisInterface(q->DupSuppress, intf) ||
						BuildQuestion(m, &m->omsg, &queryptr, q, &kalistptr, &answerforecast))
							q->SendQNow = (q->InterfaceID || !q->SendOnAll) ? mDNSNULL : GetNextActiveInterfaceID(intf);
					}
				}

			// Put probe questions in this packet
			for (rr = m->ResourceRecords; rr; rr=rr->next)
				if (rr->SendRNow == intf->InterfaceID)
					{
					mDNSBool ucast = (rr->ProbeCount >= DefaultProbeCountForTypeUnique-1) && m->CanReceiveUnicastOn5353;
					mDNSu16 ucbit = (mDNSu16)(ucast ? kDNSQClass_UnicastResponse : 0);
					const mDNSu8 *const limit = m->omsg.data + ((m->omsg.h.numQuestions) ? NormalMaxDNSMessageData : AbsoluteMaxDNSMessageData);
					mDNSu8 *newptr = putQuestion(&m->omsg, queryptr, limit, rr->resrec.name, kDNSQType_ANY, (mDNSu16)(rr->resrec.rrclass | ucbit));
					// We forecast: compressed name (2) type (2) class (2) TTL (4) rdlength (2) rdata (n)
					mDNSu32 forecast = answerforecast + 12 + rr->resrec.rdestimate;
					if (newptr && newptr + forecast < limit)
						{
						queryptr       = newptr;
						answerforecast = forecast;
						rr->SendRNow = (rr->resrec.InterfaceID) ? mDNSNULL : GetNextActiveInterfaceID(intf);
						rr->IncludeInProbe = mDNStrue;
						verbosedebugf("SendQueries:   Put Question %##s (%s) probecount %d",
							rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype), rr->ProbeCount);
						}
					else
						{
						verbosedebugf("SendQueries:   Retracting Question %##s (%s)",
							rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype));
						m->omsg.h.numQuestions--;
						}
					}
				}

		// Put our known answer list (either new one from this question or questions, or remainder of old one from last time)
		while (KnownAnswerList)
			{
			CacheRecord *rr = KnownAnswerList;
			mDNSu32 SecsSinceRcvd = ((mDNSu32)(m->timenow - rr->TimeRcvd)) / mDNSPlatformOneSecond;
			mDNSu8 *newptr = PutResourceRecordTTL(&m->omsg, queryptr, &m->omsg.h.numAnswers, &rr->resrec, rr->resrec.rroriginalttl - SecsSinceRcvd);
			if (newptr)
				{
				verbosedebugf("SendQueries:   Put %##s (%s) at %d - %d",
					rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype), queryptr - m->omsg.data, newptr - m->omsg.data);
				queryptr = newptr;
				KnownAnswerList = rr->NextInKAList;
				rr->NextInKAList = mDNSNULL;
				}
			else
				{
				// If we ran out of space and we have more than one question in the packet, that's an error --
				// we shouldn't have put more than one question if there was a risk of us running out of space.
				if (m->omsg.h.numQuestions > 1)
					LogMsg("SendQueries:   Put %d answers; No more space for known answers", m->omsg.h.numAnswers);
				m->omsg.h.flags.b[0] |= kDNSFlag0_TC;
				break;
				}
			}

		for (rr = m->ResourceRecords; rr; rr=rr->next)
			if (rr->IncludeInProbe)
				{
				mDNSu8 *newptr = PutResourceRecord(&m->omsg, queryptr, &m->omsg.h.numAuthorities, &rr->resrec);
				rr->IncludeInProbe = mDNSfalse;
				if (newptr) queryptr = newptr;
				else LogMsg("SendQueries:   How did we fail to have space for the Update record %##s (%s)?",
					rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype));
				}
		
		if (queryptr > m->omsg.data)
			{
			if ((m->omsg.h.flags.b[0] & kDNSFlag0_TC) && m->omsg.h.numQuestions > 1)
				LogMsg("SendQueries: Should not have more than one question (%d) in a truncated packet", m->omsg.h.numQuestions);
			debugf("SendQueries:   Sending %d Question%s %d Answer%s %d Update%s on %p",
				m->omsg.h.numQuestions,   m->omsg.h.numQuestions   == 1 ? "" : "s",
				m->omsg.h.numAnswers,     m->omsg.h.numAnswers     == 1 ? "" : "s",
				m->omsg.h.numAuthorities, m->omsg.h.numAuthorities == 1 ? "" : "s", intf->InterfaceID);
			if (intf->IPv4Available) mDNSSendDNSMessage(m, &m->omsg, queryptr, intf->InterfaceID, &AllDNSLinkGroup_v4, MulticastDNSPort, mDNSNULL, mDNSNULL);
			if (intf->IPv6Available) mDNSSendDNSMessage(m, &m->omsg, queryptr, intf->InterfaceID, &AllDNSLinkGroup_v6, MulticastDNSPort, mDNSNULL, mDNSNULL);
			if (!m->SuppressSending) m->SuppressSending = NonZeroTime(m->timenow + (mDNSPlatformOneSecond+9)/10);
			if (++pktcount >= 1000)
				{ LogMsg("SendQueries exceeded loop limit %d: giving up", pktcount); break; }
			// There might be more records left in the known answer list, or more questions to send
			// on this interface, so go around one more time and try again.
			}
		else	// Nothing more to send on this interface; go to next
			{
			const NetworkInterfaceInfo *next = GetFirstActiveInterface(intf->next);
			#if MDNS_DEBUGMSGS && 0
			const char *const msg = next ? "SendQueries:   Nothing more on %p; moving to %p" : "SendQueries:   Nothing more on %p";
			debugf(msg, intf, next);
			#endif
			intf = next;
			}
		}

	// 4. Final housekeeping
	
	// 4a. Debugging check: Make sure we announced all our records
	for (ar = m->ResourceRecords; ar; ar=ar->next)
		if (ar->SendRNow)
			{
			if (ar->resrec.InterfaceID != mDNSInterface_LocalOnly)
				LogMsg("SendQueries: No active interface to send: %s", ARDisplayString(m, ar));
			ar->SendRNow = mDNSNULL;
			}

	// 4b. When we have lingering cache records that we're keeping around for a few seconds in the hope
	// that their interface which went away might come back again, the logic will want to send queries
	// for those records, but we can't because their interface isn't here any more, so to keep the
	// state machine ticking over we just pretend we did so.
	// If the interface does not come back in time, the cache record will expire naturally
	FORALL_CACHERECORDS(slot, cg, cr)
		if (cr->CRActiveQuestion && cr->UnansweredQueries < MaxUnansweredQueries && m->timenow - cr->NextRequiredQuery >= 0)
			{
			cr->UnansweredQueries++;
			cr->CRActiveQuestion->SendQNow = mDNSNULL;
			SetNextCacheCheckTime(m, cr);
			}

	// 4c. Debugging check: Make sure we sent all our planned questions
	// Do this AFTER the lingering cache records check above, because that will prevent spurious warnings for questions
	// we legitimately couldn't send because the interface is no longer available
	for (q = m->Questions; q; q=q->next)
		if (q->SendQNow)
			{
			LogMsg("SendQueries: No active interface to send: %##s (%s)", q->qname.c, DNSTypeName(q->qtype));
			q->SendQNow = mDNSNULL;
			}
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - RR List Management & Task Management
#endif

// NOTE: AnswerCurrentQuestionWithResourceRecord can call a user callback, which may change the record list and/or question list.
// Any code walking either list must use the m->CurrentQuestion (and possibly m->CurrentRecord) mechanism to protect against this.
// In fact, to enforce this, the routine will *only* answer the question currently pointed to by m->CurrentQuestion,
// which will be auto-advanced (possibly to NULL) if the client callback cancels the question.
mDNSexport void AnswerCurrentQuestionWithResourceRecord(mDNS *const m, CacheRecord *const rr, const QC_result AddRecord)
	{
	DNSQuestion *const q = m->CurrentQuestion;
	mDNSBool followcname = rr->resrec.RecordType != kDNSRecordTypePacketNegative && AddRecord &&
							rr->resrec.rrtype == kDNSType_CNAME && q->qtype != kDNSType_CNAME;
	verbosedebugf("AnswerCurrentQuestionWithResourceRecord:%4lu %s TTL %d %s", q->CurrentAnswers, AddRecord ? "Add" : "Rmv", rr->resrec.rroriginalttl, CRDisplayString(m, rr));

	// Note: Use caution here. In the case of records with rr->DelayDelivery set, AnswerCurrentQuestionWithResourceRecord(... mDNStrue)
	// may be called twice, once when the record is received, and again when it's time to notify local clients.
	// If any counters or similar are added here, care must be taken to ensure that they are not double-incremented by this.

	rr->LastUsed = m->timenow;
	if (AddRecord == QC_add && !q->DuplicateOf && rr->CRActiveQuestion != q)
		{
		if (!rr->CRActiveQuestion) m->rrcache_active++;	// If not previously active, increment rrcache_active count
		debugf("AnswerCurrentQuestionWithResourceRecord: Updating CRActiveQuestion to %p for cache record %s", q, CRDisplayString(m,rr));
		rr->CRActiveQuestion = q;						// We know q is non-null
		SetNextCacheCheckTime(m, rr);
		}

	// If this is:
	// (a) a no-cache add, where we've already done at least one 'QM' query, or
	// (b) a normal add, where we have at least one unique-type answer,
	// then there's no need to keep polling the network.
	// (If we have an answer in the cache, then we'll automatically ask again in time to stop it expiring.)
	// We do this for mDNS questions and uDNS one-shot questions, but not for
	// uDNS LongLived questions, because that would mess up our LLQ lease renewal timing.
	if ((AddRecord == QC_addnocache && !q->RequestUnicast) ||
		(AddRecord == QC_add && (q->ExpectUnique || (rr->resrec.RecordType & kDNSRecordTypePacketUniqueMask))))
		if (ActiveQuestion(q) && (mDNSOpaque16IsZero(q->TargetQID) || !q->LongLived))
			{
			q->LastQTime        = m->timenow;
			q->LastQTxTime      = m->timenow;
			q->RecentAnswerPkts = 0;
			q->ThisQInterval    = MaxQuestionInterval;
			q->RequestUnicast   = mDNSfalse;
			}

	if (rr->DelayDelivery) return;		// We'll come back later when CacheRecordDeferredAdd() calls us

	// Only deliver negative answers if client has explicitly requested them
	if (rr->resrec.RecordType == kDNSRecordTypePacketNegative && (!AddRecord || !q->ReturnIntermed)) return;

	// For CNAME results to non-CNAME questions, only inform the client if they explicitly requested that
	if (q->QuestionCallback && !q->NoAnswer && (!followcname || q->ReturnIntermed))
		{
		mDNS_DropLockBeforeCallback();		// Allow client (and us) to legally make mDNS API calls
		q->QuestionCallback(m, q, &rr->resrec, AddRecord);
		mDNS_ReclaimLockAfterCallback();	// Decrement mDNS_reentrancy to block mDNS API calls again
		}
	// NOTE: Proceed with caution here because client callback function is allowed to do anything,
	// including starting/stopping queries, registering/deregistering records, etc.

	if (followcname && m->CurrentQuestion == q && q->CNAMEReferrals < 10)
		{
		const mDNSu32 c = q->CNAMEReferrals + 1;
		// Right now we just stop and re-use the existing query. If we really wanted to be 100% perfect,
		// and track CNAMEs coming and going, we should really create a subbordinate query here,
		// which we would subsequently cancel and retract if the CNAME referral record were removed.
		// In reality this is such a corner case we'll ignore it until someone actually needs it.
		LogOperation("AnswerCurrentQuestionWithResourceRecord: following CNAME referral for %s", CRDisplayString(m, rr));
		mDNS_StopQuery_internal(m, q);								// Stop old query
		AssignDomainName(&q->qname, &rr->resrec.rdata->u.name);		// Update qname
		q->qnamehash = DomainNameHashValue(&q->qname);				// and namehash
		mDNS_StartQuery_internal(m, q);								// start new query
		q->CNAMEReferrals = c;										// and keep count of how many times we've done this
		}
	}

mDNSlocal void CacheRecordDeferredAdd(mDNS *const m, CacheRecord *rr)
	{
	rr->DelayDelivery = 0;		// Note, only need to call SetNextCacheCheckTime() when DelayDelivery is set, not when it's cleared
	if (m->CurrentQuestion)
		LogMsg("CacheRecordDeferredAdd ERROR m->CurrentQuestion already set: %##s (%s)", m->CurrentQuestion->qname.c, DNSTypeName(m->CurrentQuestion->qtype));
	m->CurrentQuestion = m->Questions;
	while (m->CurrentQuestion && m->CurrentQuestion != m->NewQuestions)
		{
		DNSQuestion *q = m->CurrentQuestion;
		if (ResourceRecordAnswersQuestion(&rr->resrec, q))
			AnswerCurrentQuestionWithResourceRecord(m, rr, QC_add);
		if (m->CurrentQuestion == q)	// If m->CurrentQuestion was not auto-advanced, do it ourselves now
			m->CurrentQuestion = q->next;
		}
	m->CurrentQuestion = mDNSNULL;
	}

mDNSlocal mDNSs32 CheckForSoonToExpireRecords(mDNS *const m, const domainname *const name, const mDNSu32 namehash, const mDNSu32 slot)
	{
	const mDNSs32 threshhold = m->timenow + mDNSPlatformOneSecond;	// See if there are any records expiring within one second
	const mDNSs32 start      = m->timenow - 0x10000000;
	mDNSs32 delay = start;
	CacheGroup *cg = CacheGroupForName(m, slot, namehash, name);
	CacheRecord *rr;
	for (rr = cg ? cg->members : mDNSNULL; rr; rr=rr->next)
		if (threshhold - RRExpireTime(rr) >= 0)		// If we have records about to expire within a second
			if (delay - RRExpireTime(rr) < 0)		// then delay until after they've been deleted
				delay = RRExpireTime(rr);
	if (delay - start > 0) return(NonZeroTime(delay));
	else return(0);
	}

// CacheRecordAdd is only called from mDNSCoreReceiveResponse, *never* directly as a result of a client API call.
// If new questions are created as a result of invoking client callbacks, they will be added to
// the end of the question list, and m->NewQuestions will be set to indicate the first new question.
// rr is a new CacheRecord just received into our cache
// (kDNSRecordTypePacketAns/PacketAnsUnique/PacketAdd/PacketAddUnique).
// NOTE: CacheRecordAdd calls AnswerCurrentQuestionWithResourceRecord which can call a user callback,
// which may change the record list and/or question list.
// Any code walking either list must use the CurrentQuestion and/or CurrentRecord mechanism to protect against this.
mDNSlocal void CacheRecordAdd(mDNS *const m, CacheRecord *rr)
	{
	DNSQuestion *q;
	for (q = m->Questions; q; q=q->next)
		{
		if (ResourceRecordAnswersQuestion(&rr->resrec, q))
			{
			// If this question is one that's actively sending queries, and it's received ten answers within one
			// second of sending the last query packet, then that indicates some radical network topology change,
			// so reset its exponential backoff back to the start. We must be at least at the eight-second interval
			// to do this. If we're at the four-second interval, or less, there's not much benefit accelerating
			// because we will anyway send another query within a few seconds. The first reset query is sent out
			// randomized over the next four seconds to reduce possible synchronization between machines.
			if (q->LastAnswerPktNum != m->PktNum)
				{
				q->LastAnswerPktNum = m->PktNum;
				if (mDNSOpaque16IsZero(q->TargetQID) && ActiveQuestion(q) && ++q->RecentAnswerPkts >= 10 &&
					q->ThisQInterval > InitialQuestionInterval * QuestionIntervalStep3 && m->timenow - q->LastQTxTime < mDNSPlatformOneSecond)
					{
					LogMsg("CacheRecordAdd: %##s (%s) got immediate answer burst; restarting exponential backoff sequence",
						q->qname.c, DNSTypeName(q->qtype));
					q->LastQTime      = m->timenow - InitialQuestionInterval + (mDNSs32)mDNSRandom((mDNSu32)mDNSPlatformOneSecond*4);
					q->ThisQInterval  = InitialQuestionInterval;
					SetNextQueryTime(m,q);
					}
				}
			verbosedebugf("CacheRecordAdd %p %##s (%s) %lu",
				rr, rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype), rr->resrec.rroriginalttl);
			q->CurrentAnswers++;
			if (rr->resrec.rdlength > SmallRecordLimit) q->LargeAnswers++;
			if (rr->resrec.RecordType & kDNSRecordTypePacketUniqueMask) q->UniqueAnswers++;
			if (q->CurrentAnswers > 4000)
				{
				static int msgcount = 0;
				if (msgcount++ < 10)
					LogMsg("CacheRecordAdd: %##s (%s) has %d answers; shedding records to resist DOS attack",
						q->qname.c, DNSTypeName(q->qtype), q->CurrentAnswers);
				rr->resrec.rroriginalttl = 0;
				rr->UnansweredQueries = MaxUnansweredQueries;
				}
			}
		}

	if (!rr->DelayDelivery)
		{
		if (m->CurrentQuestion)
			LogMsg("CacheRecordAdd ERROR m->CurrentQuestion already set: %##s (%s)", m->CurrentQuestion->qname.c, DNSTypeName(m->CurrentQuestion->qtype));
		m->CurrentQuestion = m->Questions;
		while (m->CurrentQuestion && m->CurrentQuestion != m->NewQuestions)
			{
			DNSQuestion *q = m->CurrentQuestion;
			if (ResourceRecordAnswersQuestion(&rr->resrec, q))
				AnswerCurrentQuestionWithResourceRecord(m, rr, QC_add);
			if (m->CurrentQuestion == q)	// If m->CurrentQuestion was not auto-advanced, do it ourselves now
				m->CurrentQuestion = q->next;
			}
		m->CurrentQuestion = mDNSNULL;
		}

	SetNextCacheCheckTime(m, rr);
	}

// NoCacheAnswer is only called from mDNSCoreReceiveResponse, *never* directly as a result of a client API call.
// If new questions are created as a result of invoking client callbacks, they will be added to
// the end of the question list, and m->NewQuestions will be set to indicate the first new question.
// rr is a new CacheRecord just received from the wire (kDNSRecordTypePacketAns/AnsUnique/Add/AddUnique)
// but we don't have any place to cache it. We'll deliver question 'add' events now, but we won't have any
// way to deliver 'remove' events in future, nor will we be able to include this in known-answer lists,
// so we immediately bump ThisQInterval up to MaxQuestionInterval to avoid pounding the network.
// NOTE: NoCacheAnswer calls AnswerCurrentQuestionWithResourceRecord which can call a user callback,
// which may change the record list and/or question list.
// Any code walking either list must use the CurrentQuestion and/or CurrentRecord mechanism to protect against this.
mDNSlocal void NoCacheAnswer(mDNS *const m, CacheRecord *rr)
	{
	LogMsg("No cache space: Delivering non-cached result for %##s", m->rec.r.resrec.name->c);
	if (m->CurrentQuestion)
		LogMsg("NoCacheAnswer ERROR m->CurrentQuestion already set: %##s (%s)", m->CurrentQuestion->qname.c, DNSTypeName(m->CurrentQuestion->qtype));
	m->CurrentQuestion = m->Questions;
	while (m->CurrentQuestion)
		{
		DNSQuestion *q = m->CurrentQuestion;
		if (ResourceRecordAnswersQuestion(&rr->resrec, q))
			AnswerCurrentQuestionWithResourceRecord(m, rr, QC_addnocache);	// QC_addnocache means "don't expect remove events for this"
		if (m->CurrentQuestion == q)	// If m->CurrentQuestion was not auto-advanced, do it ourselves now
			m->CurrentQuestion = q->next;
		}
	m->CurrentQuestion = mDNSNULL;
	}

// CacheRecordRmv is only called from CheckCacheExpiration, which is called from mDNS_Execute
// If new questions are created as a result of invoking client callbacks, they will be added to
// the end of the question list, and m->NewQuestions will be set to indicate the first new question.
// rr is an existing cache CacheRecord that just expired and is being deleted
// (kDNSRecordTypePacketAns/PacketAnsUnique/PacketAdd/PacketAddUnique).
// NOTE: CacheRecordRmv calls AnswerCurrentQuestionWithResourceRecord which can call a user callback,
// which may change the record list and/or question list.
// Any code walking either list must use the CurrentQuestion and/or CurrentRecord mechanism to protect against this.
mDNSlocal void CacheRecordRmv(mDNS *const m, CacheRecord *rr)
	{
	if (m->CurrentQuestion)
		LogMsg("CacheRecordRmv ERROR m->CurrentQuestion already set: %##s (%s)", m->CurrentQuestion->qname.c, DNSTypeName(m->CurrentQuestion->qtype));
	m->CurrentQuestion = m->Questions;
	while (m->CurrentQuestion && m->CurrentQuestion != m->NewQuestions)
		{
		DNSQuestion *q = m->CurrentQuestion;
		if (ResourceRecordAnswersQuestion(&rr->resrec, q))
			{
			verbosedebugf("CacheRecordRmv %p %s", rr, CRDisplayString(m, rr));
			q->FlappingInterface1 = mDNSNULL;
			q->FlappingInterface2 = mDNSNULL;
			if (q->CurrentAnswers == 0)
				LogMsg("CacheRecordRmv ERROR: How can CurrentAnswers already be zero for %p %##s (%s)?",
					q, q->qname.c, DNSTypeName(q->qtype));
			else
				{
				q->CurrentAnswers--;
				if (rr->resrec.rdlength > SmallRecordLimit) q->LargeAnswers--;
				if (rr->resrec.RecordType & kDNSRecordTypePacketUniqueMask) q->UniqueAnswers--;
				}
			if (rr->resrec.rdata->MaxRDLength) // Never generate "remove" events for negative results
				{
				if (q->CurrentAnswers == 0)
					{
					LogOperation("CacheRecordRmv: Last answer for %##s (%s) expired from cache; will reconfirm antecedents",
						q->qname.c, DNSTypeName(q->qtype));
					ReconfirmAntecedents(m, &q->qname, q->qnamehash, 0);
					}
				AnswerCurrentQuestionWithResourceRecord(m, rr, QC_rmv);
				}
			}
		if (m->CurrentQuestion == q)	// If m->CurrentQuestion was not auto-advanced, do it ourselves now
			m->CurrentQuestion = q->next;
		}
	m->CurrentQuestion = mDNSNULL;
	}

mDNSlocal void ReleaseCacheEntity(mDNS *const m, CacheEntity *e)
	{
#if APPLE_OSX_mDNSResponder && MACOSX_MDNS_MALLOC_DEBUGGING >= 1
	unsigned int i;
	for (i=0; i<sizeof(*e); i++) ((char*)e)[i] = 0xFF;
#endif
	e->next = m->rrcache_free;
	m->rrcache_free = e;
	m->rrcache_totalused--;
	}

mDNSlocal void ReleaseCacheGroup(mDNS *const m, CacheGroup **cp)
	{
	CacheEntity *e = (CacheEntity *)(*cp);
	//LogMsg("ReleaseCacheGroup:  Releasing CacheGroup for %p, %##s", (*cp)->name->c, (*cp)->name->c);
	if ((*cp)->rrcache_tail != &(*cp)->members)
		LogMsg("ERROR: (*cp)->members == mDNSNULL but (*cp)->rrcache_tail != &(*cp)->members)");
	//if ((*cp)->name != (domainname*)((*cp)->namestorage))
	//	LogMsg("ReleaseCacheGroup: %##s, %p %p", (*cp)->name->c, (*cp)->name, (domainname*)((*cp)->namestorage));
	if ((*cp)->name != (domainname*)((*cp)->namestorage)) mDNSPlatformMemFree((*cp)->name);
	(*cp)->name = mDNSNULL;
	*cp = (*cp)->next;			// Cut record from list
	ReleaseCacheEntity(m, e);
	}

mDNSlocal void ReleaseCacheRecord(mDNS *const m, CacheRecord *r)
	{
	//LogMsg("ReleaseCacheRecord: Releasing %s", CRDisplayString(m, r));
	if (r->resrec.rdata && r->resrec.rdata != (RData*)&r->rdatastorage) mDNSPlatformMemFree(r->resrec.rdata);
	r->resrec.rdata = mDNSNULL;
	ReleaseCacheEntity(m, (CacheEntity *)r);
	}

// Note: We want to be careful that we deliver all the CacheRecordRmv calls before delivering
// CacheRecordDeferredAdd calls. The in-order nature of the cache lists ensures that all
// callbacks for old records are delivered before callbacks for newer records.
mDNSlocal void CheckCacheExpiration(mDNS *const m, CacheGroup *const cg)
	{
	CacheRecord **rp = &cg->members;

	if (m->lock_rrcache) { LogMsg("CheckCacheExpiration ERROR! Cache already locked!"); return; }
	m->lock_rrcache = 1;

	while (*rp)
		{
		CacheRecord *const rr = *rp;
		mDNSs32 event = RRExpireTime(rr);
		if (m->timenow - event >= 0)	// If expired, delete it
			{
			*rp = rr->next;				// Cut it from the list
			verbosedebugf("CheckCacheExpiration: Deleting%7d %4d %p %s",
				m->timenow - rr->TimeRcvd, rr->resrec.rroriginalttl, rr->CRActiveQuestion, CRDisplayString(m, rr));
			if (rr->CRActiveQuestion)	// If this record has one or more active questions, tell them it's going away
				{
				CacheRecordRmv(m, rr);
				m->rrcache_active--;
				}
			ReleaseCacheRecord(m, rr);
			}
		else							// else, not expired; see if we need to query
			{
			if (rr->DelayDelivery && rr->DelayDelivery - m->timenow > 0)
				event = rr->DelayDelivery;
			else
				{
				if (rr->DelayDelivery) CacheRecordDeferredAdd(m, rr);
				if (rr->CRActiveQuestion && rr->UnansweredQueries < MaxUnansweredQueries)
					{
					if (m->timenow - rr->NextRequiredQuery < 0)		// If not yet time for next query
						event = rr->NextRequiredQuery;				// then just record when we want the next query
					else											// else trigger our question to go out now
						{
						// Set NextScheduledQuery to timenow so that SendQueries() will run.
						// SendQueries() will see that we have records close to expiration, and send FEQs for them.
						m->NextScheduledQuery = m->timenow;
						// After sending the query we'll increment UnansweredQueries and call SetNextCacheCheckTime(),
						// which will correctly update m->NextCacheCheck for us.
						event = m->timenow + 0x3FFFFFFF;
						}
					}
				}
			verbosedebugf("CheckCacheExpiration:%6d %5d %s",
				(event - m->timenow) / mDNSPlatformOneSecond, CacheCheckGracePeriod(rr), CRDisplayString(m, rr));
			if (m->NextCacheCheck - (event + CacheCheckGracePeriod(rr)) > 0)
				m->NextCacheCheck = (event + CacheCheckGracePeriod(rr));
			rp = &rr->next;
			}
		}
	if (cg->rrcache_tail != rp) verbosedebugf("CheckCacheExpiration: Updating CacheGroup tail from %p to %p", cg->rrcache_tail, rp);
	cg->rrcache_tail = rp;
	m->lock_rrcache = 0;
	}

mDNSlocal void AnswerNewQuestion(mDNS *const m)
	{
	mDNSBool ShouldQueryImmediately = mDNStrue;
	CacheRecord *rr;
	DNSQuestion *q = m->NewQuestions;		// Grab the question we're going to answer
	const mDNSu32 slot = HashSlot(&q->qname);
	CacheGroup *const cg = CacheGroupForName(m, slot, q->qnamehash, &q->qname);

	verbosedebugf("AnswerNewQuestion: Answering %##s (%s)", q->qname.c, DNSTypeName(q->qtype));

	if (cg) CheckCacheExpiration(m, cg);
	m->NewQuestions = q->next;				// Advance NewQuestions to the next *after* calling CheckCacheExpiration();

	if (m->lock_rrcache) LogMsg("AnswerNewQuestion ERROR! Cache already locked!");
	// This should be safe, because calling the client's question callback may cause the
	// question list to be modified, but should not ever cause the rrcache list to be modified.
	// If the client's question callback deletes the question, then m->CurrentQuestion will
	// be advanced, and we'll exit out of the loop
	m->lock_rrcache = 1;
	if (m->CurrentQuestion)
		LogMsg("AnswerNewQuestion ERROR m->CurrentQuestion already set: %##s (%s)", m->CurrentQuestion->qname.c, DNSTypeName(m->CurrentQuestion->qtype));
	m->CurrentQuestion = q;		// Indicate which question we're answering, so we'll know if it gets deleted

	if (q->NoAnswer == NoAnswer_Fail)
		{
		LogMsg("AnswerNewQuestion: NoAnswer_Fail %##s (%s)", q->qname.c, DNSTypeName(q->qtype));
		MakeNegativeCacheRecord(m, &q->qname, q->qnamehash, q->qtype, q->qclass, 60);
		q->NoAnswer = NoAnswer_Normal;		// Temporarily turn off answer suppression
		AnswerCurrentQuestionWithResourceRecord(m, &m->rec.r, QC_addnocache);
		q->NoAnswer = NoAnswer_Fail;		// Restore NoAnswer state
		m->rec.r.resrec.RecordType = 0;		// Clear RecordType to show we're not still using it
		}

	// If 'mDNSInterface_Any' question, see if we want to tell it about LocalOnly records
	if (m->CurrentQuestion == q && q->InterfaceID == mDNSInterface_Any)
		{
		if (m->CurrentRecord)
			LogMsg("AnswerNewQuestion ERROR m->CurrentRecord already set %s", ARDisplayString(m, m->CurrentRecord));
		m->CurrentRecord = m->ResourceRecords;
		while (m->CurrentRecord && m->CurrentRecord != m->NewLocalRecords)
			{
			AuthRecord *rr = m->CurrentRecord;
			m->CurrentRecord = rr->next;
			if (rr->resrec.InterfaceID == mDNSInterface_LocalOnly)
				if (ResourceRecordAnswersQuestion(&rr->resrec, q))
					{
					AnswerLocalOnlyQuestionWithResourceRecord(m, q, rr, mDNStrue);
					if (m->CurrentQuestion != q) break;		// If callback deleted q, then we're finished here
					}
			}
		m->CurrentRecord = mDNSNULL;
		}

	if (m->CurrentQuestion == q)
		{
		for (rr = cg ? cg->members : mDNSNULL; rr; rr=rr->next)
			if (SameNameRecordAnswersQuestion(&rr->resrec, q))
				{
				// SecsSinceRcvd is whole number of elapsed seconds, rounded down
				mDNSu32 SecsSinceRcvd = ((mDNSu32)(m->timenow - rr->TimeRcvd)) / mDNSPlatformOneSecond;
				if (rr->resrec.rroriginalttl <= SecsSinceRcvd)
					{
					LogMsg("AnswerNewQuestion: How is rr->resrec.rroriginalttl %lu <= SecsSinceRcvd %lu for %##s (%s) %d %d",
						rr->resrec.rroriginalttl, SecsSinceRcvd, rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype), m->timenow, rr->TimeRcvd);
					continue;	// Go to next one in loop
					}
	
				// If this record set is marked unique, then that means we can reasonably assume we have the whole set
				// -- we don't need to rush out on the network and query immediately to see if there are more answers out there
				if ((rr->resrec.RecordType & kDNSRecordTypePacketUniqueMask) || (q->ExpectUnique))
					ShouldQueryImmediately = mDNSfalse;
				q->CurrentAnswers++;
				if (rr->resrec.rdlength > SmallRecordLimit) q->LargeAnswers++;
				if (rr->resrec.RecordType & kDNSRecordTypePacketUniqueMask) q->UniqueAnswers++;
				AnswerCurrentQuestionWithResourceRecord(m, rr, QC_add);
				if (m->CurrentQuestion != q) break;		// If callback deleted q, then we're finished here
				}
			else if (RRTypeIsAddressType(rr->resrec.rrtype) && RRTypeIsAddressType(q->qtype))
				if (rr->resrec.namehash == q->qnamehash && SameDomainName(rr->resrec.name, &q->qname))
					ShouldQueryImmediately = mDNSfalse;
		}

	if (m->CurrentQuestion == q && ShouldQueryImmediately && ActiveQuestion(q))
		{
		q->ThisQInterval  = InitialQuestionInterval;
		q->LastQTime      = m->timenow - q->ThisQInterval;
		if (mDNSOpaque16IsZero(q->TargetQID))
			{
			// Compute random delay in the range 1-6 seconds, then divide by 50 to get 20-120ms
			if (!m->RandomQueryDelay)
				m->RandomQueryDelay = (mDNSPlatformOneSecond + mDNSRandom(mDNSPlatformOneSecond*5) - 1) / 50 + 1;
			q->LastQTime += m->RandomQueryDelay;
			}
		
		m->NextScheduledQuery = m->timenow;
		}

	m->CurrentQuestion = mDNSNULL;
	m->lock_rrcache = 0;
	}

// When a NewLocalOnlyQuestion is created, AnswerNewLocalOnlyQuestion runs though our ResourceRecords delivering any
// appropriate answers, stopping if it reaches a NewLocalRecord -- these will be handled by AnswerLocalQuestions
mDNSlocal void AnswerNewLocalOnlyQuestion(mDNS *const m)
	{
	DNSQuestion *q = m->NewLocalOnlyQuestions;		// Grab the question we're going to answer
	m->NewLocalOnlyQuestions = q->next;				// Advance NewQuestions to the next (if any)

	debugf("AnswerNewLocalOnlyQuestion: Answering %##s (%s)", q->qname.c, DNSTypeName(q->qtype));

	if (m->CurrentQuestion)
		LogMsg("AnswerNewLocalOnlyQuestion ERROR m->CurrentQuestion already set: %##s (%s)", m->CurrentQuestion->qname.c, DNSTypeName(m->CurrentQuestion->qtype));
	m->CurrentQuestion = q;		// Indicate which question we're answering, so we'll know if it gets deleted

	if (m->CurrentRecord)
		LogMsg("AnswerNewLocalOnlyQuestion ERROR m->CurrentRecord already set %s", ARDisplayString(m, m->CurrentRecord));
	m->CurrentRecord = m->ResourceRecords;

	while (m->CurrentRecord && m->CurrentRecord != m->NewLocalRecords)
		{
		AuthRecord *rr = m->CurrentRecord;
		m->CurrentRecord = rr->next;
		if (ResourceRecordAnswersQuestion(&rr->resrec, q))
			{
			AnswerLocalOnlyQuestionWithResourceRecord(m, q, rr, mDNStrue);
			if (m->CurrentQuestion != q) break;		// If callback deleted q, then we're finished here
			}
		}

	m->CurrentQuestion = mDNSNULL;
	m->CurrentRecord   = mDNSNULL;
	}

mDNSlocal CacheEntity *GetCacheEntity(mDNS *const m, const CacheGroup *const PreserveCG)
	{
	CacheEntity *e = mDNSNULL;

	if (m->lock_rrcache) { LogMsg("GetFreeCacheRR ERROR! Cache already locked!"); return(mDNSNULL); }
	m->lock_rrcache = 1;
	
	// If we have no free records, ask the client layer to give us some more memory
	if (!m->rrcache_free && m->MainCallback)
		{
		if (m->rrcache_totalused != m->rrcache_size)
			LogMsg("GetFreeCacheRR: count mismatch: m->rrcache_totalused %lu != m->rrcache_size %lu",
				m->rrcache_totalused, m->rrcache_size);
		
		// We don't want to be vulnerable to a malicious attacker flooding us with an infinite
		// number of bogus records so that we keep growing our cache until the machine runs out of memory.
		// To guard against this, if our cache grows above 512kB (approx 3168 records at 164 bytes each),
		// and we're actively using less than 1/32 of that cache, then we purge all the unused records
		// and recycle them, instead of allocating more memory.
		if (m->rrcache_size > 3000 && m->rrcache_size / 32 > m->rrcache_active)
			LogOperation("Possible denial-of-service attack in progress: m->rrcache_size %lu; m->rrcache_active %lu",
				m->rrcache_size, m->rrcache_active);
		else
			{
			mDNS_DropLockBeforeCallback();		// Allow client to legally make mDNS API calls from the callback
			m->MainCallback(m, mStatus_GrowCache);
			mDNS_ReclaimLockAfterCallback();	// Decrement mDNS_reentrancy to block mDNS API calls again
			}
		}
	
	// If we still have no free records, recycle all the records we can.
	// Enumerating the entire cache is moderately expensive, so when we do it, we reclaim all the records we can in one pass.
	if (!m->rrcache_free)
		{
		#if LogAllOperations || MDNS_DEBUGMSGS
		mDNSu32 oldtotalused = m->rrcache_totalused;
		#endif
		mDNSu32 slot;
		for (slot = 0; slot < CACHE_HASH_SLOTS; slot++)
			{
			CacheGroup **cp = &m->rrcache_hash[slot];
			while (*cp)
				{
				CacheRecord **rp = &(*cp)->members;
				while (*rp)
					{
					// Records that answer still-active questions are not candidates for recycling
					// Records that are currently linked into the CacheFlushRecords list may not be recycled, or we'll crash
					if ((*rp)->CRActiveQuestion || (*rp)->NextInCFList)
						rp=&(*rp)->next;
					else
						{
						CacheRecord *rr = *rp;
						*rp = (*rp)->next;			// Cut record from list
						ReleaseCacheRecord(m, rr);
						}
					}
				if ((*cp)->rrcache_tail != rp)
					verbosedebugf("GetFreeCacheRR: Updating rrcache_tail[%lu] from %p to %p", slot, (*cp)->rrcache_tail, rp);
				(*cp)->rrcache_tail = rp;
				if ((*cp)->members || (*cp)==PreserveCG) cp=&(*cp)->next;
				else ReleaseCacheGroup(m, cp);
				}
			}
		LogOperation("GetCacheEntity recycled %d records to reduce cache from %d to %d",
			oldtotalused - m->rrcache_totalused, oldtotalused, m->rrcache_totalused);
		}

	if (m->rrcache_free)	// If there are records in the free list, take one
		{
		e = m->rrcache_free;
		m->rrcache_free = e->next;
		if (++m->rrcache_totalused >= m->rrcache_report)
			{
			LogOperation("RR Cache now using %ld objects", m->rrcache_totalused);
			if (m->rrcache_report < 100) m->rrcache_report += 10;
			else                         m->rrcache_report += 100;
			}
		mDNSPlatformMemZero(e, sizeof(*e));
		}

	m->lock_rrcache = 0;

	return(e);
	}

mDNSlocal CacheRecord *GetCacheRecord(mDNS *const m, CacheGroup *cg, mDNSu16 RDLength)
	{
	CacheRecord *r = (CacheRecord *)GetCacheEntity(m, cg);
	if (r)
		{
		r->resrec.rdata = (RData*)&r->rdatastorage;	// By default, assume we're usually going to be using local storage
		if (RDLength > InlineCacheRDSize)			// If RDLength is too big, allocate extra storage
			{
			r->resrec.rdata = (RData*)mDNSPlatformMemAllocate(sizeofRDataHeader + RDLength);
			if (r->resrec.rdata) r->resrec.rdata->MaxRDLength = r->resrec.rdlength = RDLength;
			else { ReleaseCacheEntity(m, (CacheEntity*)r); r = mDNSNULL; }
			}
		}
	return(r);
	}

mDNSlocal CacheGroup *GetCacheGroup(mDNS *const m, const mDNSu32 slot, const ResourceRecord *const rr)
	{
	mDNSu16 namelen = DomainNameLength(rr->name);
	CacheGroup *cg = (CacheGroup*)GetCacheEntity(m, mDNSNULL);
	if (!cg) { LogMsg("GetCacheGroup: Failed to allocate memory for %##s", rr->name->c); return(mDNSNULL); }
	cg->next         = m->rrcache_hash[slot];
	cg->namehash     = rr->namehash;
	cg->members      = mDNSNULL;
	cg->rrcache_tail = &cg->members;
	cg->name         = (domainname*)cg->namestorage;
	//LogMsg("GetCacheGroup: %-10s %d-byte cache name %##s",
	//	(namelen > InlineCacheGroupNameSize) ? "Allocating" : "Inline", namelen, rr->name->c);
	if (namelen > InlineCacheGroupNameSize) cg->name = mDNSPlatformMemAllocate(namelen);
	if (!cg->name)
		{
		LogMsg("GetCacheGroup: Failed to allocate name storage for %##s", rr->name->c);
		ReleaseCacheEntity(m, (CacheEntity*)cg);
		return(mDNSNULL);
		}
	AssignDomainName(cg->name, rr->name);

	if (CacheGroupForRecord(m, slot, rr)) LogMsg("GetCacheGroup: Already have CacheGroup for %##s", rr->name->c);
	m->rrcache_hash[slot] = cg;
	if (CacheGroupForRecord(m, slot, rr) != cg) LogMsg("GetCacheGroup: Not finding CacheGroup for %##s", rr->name->c);
	
	return(cg);
	}

mDNSexport void mDNS_PurgeCacheResourceRecord(mDNS *const m, CacheRecord *rr)
	{
	// Make sure we mark this record as thoroughly expired -- we don't ever want to give
	// a positive answer using an expired record (e.g. from an interface that has gone away).
	// We don't want to clear CRActiveQuestion here, because that would leave the record subject to
	// summary deletion without giving the proper callback to any questions that are monitoring it.
	// By setting UnansweredQueries to MaxUnansweredQueries we ensure it won't trigger any further expiration queries.
	rr->TimeRcvd          = m->timenow - mDNSPlatformOneSecond * 60;
	rr->UnansweredQueries = MaxUnansweredQueries;
	rr->resrec.rroriginalttl     = 0;
	SetNextCacheCheckTime(m, rr);
	}

mDNSexport mDNSs32 mDNS_TimeNow(const mDNS *const m)
	{
	mDNSs32 time;
	mDNSPlatformLock(m);
	if (m->mDNS_busy)
		{
		LogMsg("mDNS_TimeNow called while holding mDNS lock. This is incorrect. Code protected by lock should just use m->timenow.");
		if (!m->timenow) LogMsg("mDNS_TimeNow: m->mDNS_busy is %ld but m->timenow not set", m->mDNS_busy);
		}
	
	if (m->timenow) time = m->timenow;
	else            time = mDNS_TimeNow_NoLock(m);
	mDNSPlatformUnlock(m);
	return(time);
	}

mDNSexport mDNSs32 mDNS_Execute(mDNS *const m)
	{
	mDNS_Lock(m);	// Must grab lock before trying to read m->timenow

	if (m->timenow - m->NextScheduledEvent >= 0)
		{
		int i;

		verbosedebugf("mDNS_Execute");
		if (m->CurrentQuestion)
			LogMsg("mDNS_Execute: ERROR m->CurrentQuestion already set: %##s (%s)", m->CurrentQuestion->qname.c, DNSTypeName(m->CurrentQuestion->qtype));
	
		// 1. If we're past the probe suppression time, we can clear it
		if (m->SuppressProbes && m->timenow - m->SuppressProbes >= 0) m->SuppressProbes = 0;
	
		// 2. If it's been more than ten seconds since the last probe failure, we can clear the counter
		if (m->NumFailedProbes && m->timenow - m->ProbeFailTime >= mDNSPlatformOneSecond * 10) m->NumFailedProbes = 0;
		
		// 3. Purge our cache of stale old records
		if (m->rrcache_size && m->timenow - m->NextCacheCheck >= 0)
			{
			mDNSu32 slot;
			m->NextCacheCheck = m->timenow + 0x3FFFFFFF;
			for (slot = 0; slot < CACHE_HASH_SLOTS; slot++)
				{
				CacheGroup **cp = &m->rrcache_hash[slot];
				while (*cp)
					{
					CheckCacheExpiration(m, *cp);
					if ((*cp)->members) cp=&(*cp)->next;
					else ReleaseCacheGroup(m, cp);
					}
				}
			}
	
		// 4. See if we can answer any of our new local questions from the cache
		for (i=0; m->NewQuestions && i<1000; i++)
			{
			if (m->NewQuestions->DelayAnswering && m->timenow - m->NewQuestions->DelayAnswering < 0) break;
			AnswerNewQuestion(m);
			}
		if (i >= 1000) LogMsg("mDNS_Execute: AnswerNewQuestion exceeded loop limit");
		
		for (i=0; m->NewLocalOnlyQuestions && i<1000; i++) AnswerNewLocalOnlyQuestion(m);
		if (i >= 1000) LogMsg("mDNS_Execute: AnswerNewLocalOnlyQuestion exceeded loop limit");

		for (i=0; i<1000 && m->NewLocalRecords && LocalRecordReady(m->NewLocalRecords); i++)
			{
			AuthRecord *rr = m->NewLocalRecords;
			m->NewLocalRecords = m->NewLocalRecords->next;
			AnswerLocalQuestions(m, rr, mDNStrue);
			}
		if (i >= 1000) LogMsg("mDNS_Execute: AnswerForNewLocalRecords exceeded loop limit");

		// 5. See what packets we need to send
		if (m->mDNSPlatformStatus != mStatus_NoError || m->SleepState) DiscardDeregistrations(m);
		else if (m->SuppressSending == 0 || m->timenow - m->SuppressSending >= 0)
			{
			// If the platform code is ready, and we're not suppressing packet generation right now
			// then send our responses, probes, and questions.
			// We check the cache first, because there might be records close to expiring that trigger questions to refresh them.
			// We send queries next, because there might be final-stage probes that complete their probing here, causing
			// them to advance to announcing state, and we want those to be included in any announcements we send out.
			// Finally, we send responses, including the previously mentioned records that just completed probing.
			m->SuppressSending = 0;
	
			// 6. Send Query packets. This may cause some probing records to advance to announcing state
			if (m->timenow - m->NextScheduledQuery >= 0 || m->timenow - m->NextScheduledProbe >= 0) SendQueries(m);
			if (m->timenow - m->NextScheduledQuery >= 0)
				{
				DNSQuestion *q;
				LogMsg("mDNS_Execute: SendQueries didn't send all its queries (%d - %d = %d) will try again in one second",
					m->timenow, m->NextScheduledQuery, m->timenow - m->NextScheduledQuery);
				m->NextScheduledQuery = m->timenow + mDNSPlatformOneSecond;
				for (q = m->Questions; q; q=q->next)
					if (ActiveQuestion(q) && q->LastQTime + q->ThisQInterval - m->timenow <= 0)
						LogMsg("mDNS_Execute: SendQueries didn't send %##s (%s)", q->qname.c, DNSTypeName(q->qtype));
				}
			if (m->timenow - m->NextScheduledProbe >= 0)
				{
				LogMsg("mDNS_Execute: SendQueries didn't send all its probes (%d - %d = %d) will try again in one second",
					m->timenow, m->NextScheduledProbe, m->timenow - m->NextScheduledProbe);
				m->NextScheduledProbe = m->timenow + mDNSPlatformOneSecond;
				}
	
			// 7. Send Response packets, including probing records just advanced to announcing state
			if (m->timenow - m->NextScheduledResponse >= 0) SendResponses(m);
			if (m->timenow - m->NextScheduledResponse >= 0)
				{
				LogMsg("mDNS_Execute: SendResponses didn't send all its responses; will try again in one second");
				m->NextScheduledResponse = m->timenow + mDNSPlatformOneSecond;
				}
			}

		// Clear RandomDelay values, ready to pick a new different value next time
		m->RandomQueryDelay     = 0;
		m->RandomReconfirmDelay = 0;
		}

	// Note about multi-threaded systems:
	// On a multi-threaded system, some other thread could run right after the mDNS_Unlock(),
	// performing mDNS API operations that change our next scheduled event time.
	//
	// On multi-threaded systems (like the current Windows implementation) that have a single main thread
	// calling mDNS_Execute() (and other threads allowed to call mDNS API routines) it is the responsibility
	// of the mDNSPlatformUnlock() routine to signal some kind of stateful condition variable that will
	// signal whatever blocking primitive the main thread is using, so that it will wake up and execute one
	// more iteration of its loop, and immediately call mDNS_Execute() again. The signal has to be stateful
	// in the sense that if the main thread has not yet entered its blocking primitive, then as soon as it
	// does, the state of the signal will be noticed, causing the blocking primitive to return immediately
	// without blocking. This avoids the race condition between the signal from the other thread arriving
	// just *before* or just *after* the main thread enters the blocking primitive.
	//
	// On multi-threaded systems (like the current Mac OS 9 implementation) that are entirely timer-driven,
	// with no main mDNS_Execute() thread, it is the responsibility of the mDNSPlatformUnlock() routine to
	// set the timer according to the m->NextScheduledEvent value, and then when the timer fires, the timer
	// callback function should call mDNS_Execute() (and ignore the return value, which may already be stale
	// by the time it gets to the timer callback function).

#ifndef UNICAST_DISABLED
	uDNS_Execute(m);
#endif
	mDNS_Unlock(m);		// Calling mDNS_Unlock is what gives m->NextScheduledEvent its new value
	return(m->NextScheduledEvent);
	}

// Call mDNSCoreMachineSleep(m, mDNStrue) when the machine is about to go to sleep.
// Call mDNSCoreMachineSleep(m, mDNSfalse) when the machine is has just woken up.
// Normally, the platform support layer below mDNSCore should call this, not the client layer above.
// Note that sleep/wake calls do not have to be paired one-for-one; it is acceptable to call
// mDNSCoreMachineSleep(m, mDNSfalse) any time there is reason to believe that the machine may have just
// found itself in a new network environment. For example, if the Ethernet hardware indicates that the
// cable has just been connected, the platform support layer should call mDNSCoreMachineSleep(m, mDNSfalse)
// to make mDNSCore re-issue its outstanding queries, probe for record uniqueness, etc.
// While it is safe to call mDNSCoreMachineSleep(m, mDNSfalse) at any time, it does cause extra network
// traffic, so it should only be called when there is legitimate reason to believe the machine
// may have become attached to a new network.
mDNSexport void mDNSCoreMachineSleep(mDNS *const m, mDNSBool sleepstate)
	{
	AuthRecord *rr;

	mDNS_Lock(m);

	m->SleepState = sleepstate;
	LogOperation("%s at %ld", sleepstate ? "Sleeping" : "Waking", m->timenow);

	if (sleepstate)
		{
#ifndef UNICAST_DISABLED
		uDNS_Sleep(m);
#endif
		// Mark all the records we need to deregister and send them
		for (rr = m->ResourceRecords; rr; rr=rr->next)
			if (rr->resrec.RecordType == kDNSRecordTypeShared && rr->RequireGoodbye)
				rr->ImmedAnswer = mDNSInterfaceMark;
		SendResponses(m);
		}
	else
		{
		DNSQuestion *q;
		mDNSu32 slot;
		CacheGroup *cg;
		CacheRecord *cr;

#ifndef UNICAST_DISABLED
		uDNS_Wake(m);
#endif
        // 1. Retrigger all our questions
		for (q = m->Questions; q; q=q->next)				// Scan our list of questions
			if (mDNSOpaque16IsZero(q->TargetQID) && ActiveQuestion(q))
				{
				q->ThisQInterval    = InitialQuestionInterval;	// MUST be > zero for an active question
				q->RequestUnicast   = 2;						// Set to 2 because is decremented once *before* we check it
				q->LastQTime        = m->timenow - q->ThisQInterval;
				q->RecentAnswerPkts = 0;
				ExpireDupSuppressInfo(q->DupSuppress, m->timenow);
				m->NextScheduledQuery = m->timenow;
				}

		// 2. Re-validate our cache records
		m->NextCacheCheck  = m->timenow;
		FORALL_CACHERECORDS(slot, cg, cr)
			mDNS_Reconfirm_internal(m, cr, kDefaultReconfirmTimeForWake);

		// 3. Retrigger probing and announcing for all our authoritative records
		for (rr = m->ResourceRecords; rr; rr=rr->next)
			{
			if (rr->resrec.RecordType == kDNSRecordTypeVerified && !rr->DependentOn) rr->resrec.RecordType = kDNSRecordTypeUnique;
			rr->ProbeCount     = DefaultProbeCountForRecordType(rr->resrec.RecordType);
			rr->AnnounceCount  = InitialAnnounceCount;
			rr->ThisAPInterval = DefaultAPIntervalForRecordType(rr->resrec.RecordType);
			InitializeLastAPTime(m, rr);
			}
		}

	mDNS_Unlock(m);
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - Packet Reception Functions
#endif

#define MustSendRecord(RR) ((RR)->NR_AnswerTo || (RR)->NR_AdditionalTo)

mDNSlocal mDNSu8 *GenerateUnicastResponse(const DNSMessage *const query, const mDNSu8 *const end,
	const mDNSInterfaceID InterfaceID, mDNSBool LegacyQuery, DNSMessage *const response, AuthRecord *ResponseRecords)
	{
	mDNSu8          *responseptr     = response->data;
	const mDNSu8    *const limit     = response->data + sizeof(response->data);
	const mDNSu8    *ptr             = query->data;
	AuthRecord  *rr;
	mDNSu32          maxttl = 0x70000000;
	int i;

	// Initialize the response fields so we can answer the questions
	InitializeDNSMessage(&response->h, query->h.id, ResponseFlags);

	// ***
	// *** 1. Write out the list of questions we are actually going to answer with this packet
	// ***
	if (LegacyQuery)
		{
		maxttl = 10;
		for (i=0; i<query->h.numQuestions; i++)						// For each question...
			{
			DNSQuestion q;
			ptr = getQuestion(query, ptr, end, InterfaceID, &q);	// get the question...
			if (!ptr) return(mDNSNULL);
	
			for (rr=ResponseRecords; rr; rr=rr->NextResponse)		// and search our list of proposed answers
				{
				if (rr->NR_AnswerTo == ptr)							// If we're going to generate a record answering this question
					{												// then put the question in the question section
					responseptr = putQuestion(response, responseptr, limit, &q.qname, q.qtype, q.qclass);
					if (!responseptr) { debugf("GenerateUnicastResponse: Ran out of space for questions!"); return(mDNSNULL); }
					break;		// break out of the ResponseRecords loop, and go on to the next question
					}
				}
			}
	
		if (response->h.numQuestions == 0) { LogMsg("GenerateUnicastResponse: ERROR! Why no questions?"); return(mDNSNULL); }
		}

	// ***
	// *** 2. Write Answers
	// ***
	for (rr=ResponseRecords; rr; rr=rr->NextResponse)
		if (rr->NR_AnswerTo)
			{
			mDNSu8 *p = PutResourceRecordCappedTTL(response, responseptr, &response->h.numAnswers, &rr->resrec, maxttl);
			if (p) responseptr = p;
			else { debugf("GenerateUnicastResponse: Ran out of space for answers!"); response->h.flags.b[0] |= kDNSFlag0_TC; }
			}

	// ***
	// *** 3. Write Additionals
	// ***
	for (rr=ResponseRecords; rr; rr=rr->NextResponse)
		if (rr->NR_AdditionalTo && !rr->NR_AnswerTo)
			{
			mDNSu8 *p = PutResourceRecordCappedTTL(response, responseptr, &response->h.numAdditionals, &rr->resrec, maxttl);
			if (p) responseptr = p;
			else debugf("GenerateUnicastResponse: No more space for additionals");
			}

	return(responseptr);
	}

// AuthRecord *our is our Resource Record
// CacheRecord *pkt is the Resource Record from the response packet we've witnessed on the network
// Returns 0 if there is no conflict
// Returns +1 if there was a conflict and we won
// Returns -1 if there was a conflict and we lost and have to rename
mDNSlocal int CompareRData(AuthRecord *our, CacheRecord *pkt)
	{
	mDNSu8 ourdata[256], *ourptr = ourdata, *ourend;
	mDNSu8 pktdata[256], *pktptr = pktdata, *pktend;
	if (!our) { LogMsg("CompareRData ERROR: our is NULL"); return(+1); }
	if (!pkt) { LogMsg("CompareRData ERROR: pkt is NULL"); return(+1); }

	ourend = putRData(mDNSNULL, ourdata, ourdata + sizeof(ourdata), &our->resrec);
	pktend = putRData(mDNSNULL, pktdata, pktdata + sizeof(pktdata), &pkt->resrec);
	while (ourptr < ourend && pktptr < pktend && *ourptr == *pktptr) { ourptr++; pktptr++; }
	if (ourptr >= ourend && pktptr >= pktend) return(0);			// If data identical, not a conflict

	if (ourptr >= ourend) return(-1);								// Our data ran out first; We lost
	if (pktptr >= pktend) return(+1);								// Packet data ran out first; We won
	if (*pktptr > *ourptr) return(-1);								// Our data is numerically lower; We lost
	if (*pktptr < *ourptr) return(+1);								// Packet data is numerically lower; We won
	
	LogMsg("CompareRData ERROR: Invalid state");
	return(-1);
	}

// See if we have an authoritative record that's identical to this packet record,
// whose canonical DependentOn record is the specified master record.
// The DependentOn pointer is typically used for the TXT record of service registrations
// It indicates that there is no inherent conflict detection for the TXT record
// -- it depends on the SRV record to resolve name conflicts
// If we find any identical ResourceRecords in our authoritative list, then follow their DependentOn
// pointer chain (if any) to make sure we reach the canonical DependentOn record
// If the record has no DependentOn, then just return that record's pointer
// Returns NULL if we don't have any local RRs that are identical to the one from the packet
mDNSlocal mDNSBool MatchDependentOn(const mDNS *const m, const CacheRecord *const pktrr, const AuthRecord *const master)
	{
	const AuthRecord *r1;
	for (r1 = m->ResourceRecords; r1; r1=r1->next)
		{
		if (IdenticalResourceRecord(&r1->resrec, &pktrr->resrec))
			{
			const AuthRecord *r2 = r1;
			while (r2->DependentOn) r2 = r2->DependentOn;
			if (r2 == master) return(mDNStrue);
			}
		}
	for (r1 = m->DuplicateRecords; r1; r1=r1->next)
		{
		if (IdenticalResourceRecord(&r1->resrec, &pktrr->resrec))
			{
			const AuthRecord *r2 = r1;
			while (r2->DependentOn) r2 = r2->DependentOn;
			if (r2 == master) return(mDNStrue);
			}
		}
	return(mDNSfalse);
	}

// Find the canonical RRSet pointer for this RR received in a packet.
// If we find any identical AuthRecord in our authoritative list, then follow its RRSet
// pointers (if any) to make sure we return the canonical member of this name/type/class
// Returns NULL if we don't have any local RRs that are identical to the one from the packet
mDNSlocal const AuthRecord *FindRRSet(const mDNS *const m, const CacheRecord *const pktrr)
	{
	const AuthRecord *rr;
	for (rr = m->ResourceRecords; rr; rr=rr->next)
		{
		if (IdenticalResourceRecord(&rr->resrec, &pktrr->resrec))
			{
			while (rr->RRSet && rr != rr->RRSet) rr = rr->RRSet;
			return(rr);
			}
		}
	return(mDNSNULL);
	}

// PacketRRConflict is called when we've received an RR (pktrr) which has the same name
// as one of our records (our) but different rdata.
// 1. If our record is not a type that's supposed to be unique, we don't care.
// 2a. If our record is marked as dependent on some other record for conflict detection, ignore this one.
// 2b. If the packet rr exactly matches one of our other RRs, and *that* record's DependentOn pointer
//     points to our record, ignore this conflict (e.g. the packet record matches one of our
//     TXT records, and that record is marked as dependent on 'our', its SRV record).
// 3. If we have some *other* RR that exactly matches the one from the packet, and that record and our record
//    are members of the same RRSet, then this is not a conflict.
mDNSlocal mDNSBool PacketRRConflict(const mDNS *const m, const AuthRecord *const our, const CacheRecord *const pktrr)
	{
	const AuthRecord *ourset = our->RRSet ? our->RRSet : our;

	// If not supposed to be unique, not a conflict
	if (!(our->resrec.RecordType & kDNSRecordTypeUniqueMask)) return(mDNSfalse);

	// If a dependent record, not a conflict
	if (our->DependentOn || MatchDependentOn(m, pktrr, our)) return(mDNSfalse);

	// If the pktrr matches a member of ourset, not a conflict
	if (FindRRSet(m, pktrr) == ourset) return(mDNSfalse);

	// Okay, this is a conflict
	return(mDNStrue);
	}

// NOTE: ResolveSimultaneousProbe calls mDNS_Deregister_internal which can call a user callback, which may change
// the record list and/or question list.
// Any code walking either list must use the CurrentQuestion and/or CurrentRecord mechanism to protect against this.
mDNSlocal void ResolveSimultaneousProbe(mDNS *const m, const DNSMessage *const query, const mDNSu8 *const end,
	DNSQuestion *q, AuthRecord *our)
	{
	int i;
	const mDNSu8 *ptr = LocateAuthorities(query, end);
	mDNSBool FoundUpdate = mDNSfalse;

	for (i = 0; i < query->h.numAuthorities; i++)
		{
		ptr = GetLargeResourceRecord(m, query, ptr, end, q->InterfaceID, kDNSRecordTypePacketAuth, &m->rec);
		if (!ptr) break;
		if (ResourceRecordAnswersQuestion(&m->rec.r.resrec, q))
			{
			FoundUpdate = mDNStrue;
			if (PacketRRConflict(m, our, &m->rec.r))
				{
				int result          = (int)our->resrec.rrclass - (int)m->rec.r.resrec.rrclass;
				if (!result) result = (int)our->resrec.rrtype  - (int)m->rec.r.resrec.rrtype;
				if (!result) result = CompareRData(our, &m->rec.r);
				if (result > 0)
					LogOperation("ResolveSimultaneousProbe: %##s (%s): We won",  our->resrec.name->c, DNSTypeName(our->resrec.rrtype));
				else if (result < 0)
					{
					LogOperation("ResolveSimultaneousProbe: %##s (%s): We lost", our->resrec.name->c, DNSTypeName(our->resrec.rrtype));
					mDNS_Deregister_internal(m, our, mDNS_Dereg_conflict);
					goto exit;
					}
				}
			}
		m->rec.r.resrec.RecordType = 0;		// Clear RecordType to show we're not still using it
		}
	if (!FoundUpdate)
		LogOperation("ResolveSimultaneousProbe: %##s (%s): No Update Record found", our->resrec.name->c, DNSTypeName(our->resrec.rrtype));
exit:
	m->rec.r.resrec.RecordType = 0;		// Clear RecordType to show we're not still using it
	}

mDNSlocal CacheRecord *FindIdenticalRecordInCache(const mDNS *const m, ResourceRecord *pktrr)
	{
	mDNSu32 slot = HashSlot(pktrr->name);
	CacheGroup *cg = CacheGroupForRecord(m, slot, pktrr);
	CacheRecord *rr;
	for (rr = cg ? cg->members : mDNSNULL; rr; rr=rr->next)
		if (pktrr->InterfaceID == rr->resrec.InterfaceID && IdenticalSameNameRecord(pktrr, &rr->resrec)) break;
	return(rr);
	}

// ProcessQuery examines a received query to see if we have any answers to give
mDNSlocal mDNSu8 *ProcessQuery(mDNS *const m, const DNSMessage *const query, const mDNSu8 *const end,
	const mDNSAddr *srcaddr, const mDNSInterfaceID InterfaceID, mDNSBool LegacyQuery, mDNSBool QueryWasMulticast,
	mDNSBool QueryWasLocalUnicast, DNSMessage *const response)
	{
	mDNSBool      FromLocalSubnet    = srcaddr && AddressIsLocalSubnet(m, InterfaceID, srcaddr);
	AuthRecord   *ResponseRecords    = mDNSNULL;
	AuthRecord  **nrp                = &ResponseRecords;
	CacheRecord  *ExpectedAnswers    = mDNSNULL;			// Records in our cache we expect to see updated
	CacheRecord **eap                = &ExpectedAnswers;
	DNSQuestion  *DupQuestions       = mDNSNULL;			// Our questions that are identical to questions in this packet
	DNSQuestion **dqp                = &DupQuestions;
	mDNSs32       delayresponse      = 0;
	mDNSBool      SendLegacyResponse = mDNSfalse;
	const mDNSu8 *ptr                = query->data;
	mDNSu8       *responseptr        = mDNSNULL;
	AuthRecord   *rr;
	int i;

	// ***
	// *** 1. Parse Question Section and mark potential answers
	// ***
	for (i=0; i<query->h.numQuestions; i++)						// For each question...
		{
		mDNSBool QuestionNeedsMulticastResponse;
		int NumAnswersForThisQuestion = 0;
		DNSQuestion pktq, *q;
		ptr = getQuestion(query, ptr, end, InterfaceID, &pktq);	// get the question...
		if (!ptr) goto exit;

		// The only queries that *need* a multicast response are:
		// * Queries sent via multicast
		// * from port 5353
		// * that don't have the kDNSQClass_UnicastResponse bit set
		// These queries need multicast responses because other clients will:
		// * suppress their own identical questions when they see these questions, and
		// * expire their cache records if they don't see the expected responses
		// For other queries, we may still choose to send the occasional multicast response anyway,
		// to keep our neighbours caches warm, and for ongoing conflict detection.
		QuestionNeedsMulticastResponse = QueryWasMulticast && !LegacyQuery && !(pktq.qclass & kDNSQClass_UnicastResponse);
		// Clear the UnicastResponse flag -- don't want to confuse the rest of the code that follows later
		pktq.qclass &= ~kDNSQClass_UnicastResponse;
		
		// Note: We use the m->CurrentRecord mechanism here because calling ResolveSimultaneousProbe
		// can result in user callbacks which may change the record list and/or question list.
		// Also note: we just mark potential answer records here, without trying to build the
		// "ResponseRecords" list, because we don't want to risk user callbacks deleting records
		// from that list while we're in the middle of trying to build it.
		if (m->CurrentRecord)
			LogMsg("ProcessQuery ERROR m->CurrentRecord already set %s", ARDisplayString(m, m->CurrentRecord));
		m->CurrentRecord = m->ResourceRecords;
		while (m->CurrentRecord)
			{
			rr = m->CurrentRecord;
			m->CurrentRecord = rr->next;
			if (ResourceRecordAnswersQuestion(&rr->resrec, &pktq) && (QueryWasMulticast || QueryWasLocalUnicast || rr->AllowRemoteQuery))
				{
				if (rr->resrec.RecordType == kDNSRecordTypeUnique)
					ResolveSimultaneousProbe(m, query, end, &pktq, rr);
				else if (ResourceRecordIsValidAnswer(rr))
					{
					NumAnswersForThisQuestion++;
					// Notes:
					// NR_AnswerTo pointing into query packet means "answer via immediate legacy unicast" (may *also* choose to multicast)
					// NR_AnswerTo == (mDNSu8*)~1             means "answer via delayed unicast" (to modern querier; may promote to multicast instead)
					// NR_AnswerTo == (mDNSu8*)~0             means "definitely answer via multicast" (can't downgrade to unicast later)
					// If we're not multicasting this record because the kDNSQClass_UnicastResponse bit was set,
					// but the multicast querier is not on a matching subnet (e.g. because of overalyed subnets on one link)
					// then we'll multicast it anyway (if we unicast, the receiver will ignore it because it has an apparently non-local source)
					if (QuestionNeedsMulticastResponse || (!FromLocalSubnet && QueryWasMulticast && !LegacyQuery))
						{
						// We only mark this question for sending if it is at least one second since the last time we multicast it
						// on this interface. If it is more than a second, or LastMCInterface is different, then we may multicast it.
						// This is to guard against the case where someone blasts us with queries as fast as they can.
						if (m->timenow - (rr->LastMCTime + mDNSPlatformOneSecond) >= 0 ||
							(rr->LastMCInterface != mDNSInterfaceMark && rr->LastMCInterface != InterfaceID))
							rr->NR_AnswerTo = (mDNSu8*)~0;
						}
					else if (!rr->NR_AnswerTo) rr->NR_AnswerTo = LegacyQuery ? ptr : (mDNSu8*)~1;
					}
				}
			}

		// If we couldn't answer this question, someone else might be able to,
		// so use random delay on response to reduce collisions
		if (NumAnswersForThisQuestion == 0) delayresponse = mDNSPlatformOneSecond;	// Divided by 50 = 20ms

		// We only do the following accelerated cache expiration processing and duplicate question suppression processing
		// for multicast queries with multicast responses.
		// For any query generating a unicast response we don't do this because we can't assume we will see the response
		if (QuestionNeedsMulticastResponse)
			{
			const mDNSu32 slot = HashSlot(&pktq.qname);
			CacheGroup *cg = CacheGroupForName(m, slot, pktq.qnamehash, &pktq.qname);
			CacheRecord *rr;

			// Make a list indicating which of our own cache records we expect to see updated as a result of this query
			// Note: Records larger than 1K are not habitually multicast, so don't expect those to be updated
			for (rr = cg ? cg->members : mDNSNULL; rr; rr=rr->next)
				if (SameNameRecordAnswersQuestion(&rr->resrec, &pktq) && rr->resrec.rdlength <= SmallRecordLimit)
					if (!rr->NextInKAList && eap != &rr->NextInKAList)
						{
						*eap = rr;
						eap = &rr->NextInKAList;
						if (rr->MPUnansweredQ == 0 || m->timenow - rr->MPLastUnansweredQT >= mDNSPlatformOneSecond)
							{
							// Although MPUnansweredQ is only really used for multi-packet query processing,
							// we increment it for both single-packet and multi-packet queries, so that it stays in sync
							// with the MPUnansweredKA value, which by necessity is incremented for both query types.
							rr->MPUnansweredQ++;
							rr->MPLastUnansweredQT = m->timenow;
							rr->MPExpectingKA = mDNStrue;
							}
						}
	
			// Check if this question is the same as any of mine.
			// We only do this for non-truncated queries. Right now it would be too complicated to try
			// to keep track of duplicate suppression state between multiple packets, especially when we
			// can't guarantee to receive all of the Known Answer packets that go with a particular query.
			if (!(query->h.flags.b[0] & kDNSFlag0_TC))
				for (q = m->Questions; q; q=q->next)
					if (!q->Target.type && ActiveQuestion(q) && m->timenow - q->LastQTxTime > mDNSPlatformOneSecond / 4)
						if (!q->InterfaceID || q->InterfaceID == InterfaceID)
							if (q->NextInDQList == mDNSNULL && dqp != &q->NextInDQList)
								if (q->qtype == pktq.qtype &&
									q->qclass == pktq.qclass &&
									q->qnamehash == pktq.qnamehash && SameDomainName(&q->qname, &pktq.qname))
									{ *dqp = q; dqp = &q->NextInDQList; }
			}
		}

	// ***
	// *** 2. Now we can safely build the list of marked answers
	// ***
	for (rr = m->ResourceRecords; rr; rr=rr->next)				// Now build our list of potential answers
		if (rr->NR_AnswerTo)									// If we marked the record...
			AddRecordToResponseList(&nrp, rr, mDNSNULL);		// ... add it to the list

	// ***
	// *** 3. Add additional records
	// ***
	AddAdditionalsToResponseList(m, ResponseRecords, &nrp, InterfaceID);

	// ***
	// *** 4. Parse Answer Section and cancel any records disallowed by Known-Answer list
	// ***
	for (i=0; i<query->h.numAnswers; i++)						// For each record in the query's answer section...
		{
		// Get the record...
		AuthRecord *rr;
		CacheRecord *ourcacherr;
		ptr = GetLargeResourceRecord(m, query, ptr, end, InterfaceID, kDNSRecordTypePacketAns, &m->rec);
		if (!ptr) goto exit;

		// See if this Known-Answer suppresses any of our currently planned answers
		for (rr=ResponseRecords; rr; rr=rr->NextResponse)
			if (MustSendRecord(rr) && ShouldSuppressKnownAnswer(&m->rec.r, rr))
				{ rr->NR_AnswerTo = mDNSNULL; rr->NR_AdditionalTo = mDNSNULL; }

		// See if this Known-Answer suppresses any previously scheduled answers (for multi-packet KA suppression)
		for (rr=m->ResourceRecords; rr; rr=rr->next)
			{
			// If we're planning to send this answer on this interface, and only on this interface, then allow KA suppression
			if (rr->ImmedAnswer == InterfaceID && ShouldSuppressKnownAnswer(&m->rec.r, rr))
				{
				if (srcaddr->type == mDNSAddrType_IPv4)
					{
					if (mDNSSameIPv4Address(rr->v4Requester, srcaddr->ip.v4)) rr->v4Requester = zerov4Addr;
					}
				else if (srcaddr->type == mDNSAddrType_IPv6)
					{
					if (mDNSSameIPv6Address(rr->v6Requester, srcaddr->ip.v6)) rr->v6Requester = zerov6Addr;
					}
				if (mDNSIPv4AddressIsZero(rr->v4Requester) && mDNSIPv6AddressIsZero(rr->v6Requester))
					{
					rr->ImmedAnswer  = mDNSNULL;
					rr->ImmedUnicast = mDNSfalse;
#if MDNS_LOG_ANSWER_SUPPRESSION_TIMES
					LogMsg("Suppressed after%4d: %s", m->timenow - rr->ImmedAnswerMarkTime, ARDisplayString(m, rr));
#endif
					}
				}
			}

		// See if this Known-Answer suppresses any answers we were expecting for our cache records. We do this always,
		// even if the TC bit is not set (the TC bit will *not* be set in the *last* packet of a multi-packet KA list).
		ourcacherr = FindIdenticalRecordInCache(m, &m->rec.r.resrec);
		if (ourcacherr && ourcacherr->MPExpectingKA && m->timenow - ourcacherr->MPLastUnansweredQT < mDNSPlatformOneSecond)
			{
			ourcacherr->MPUnansweredKA++;
			ourcacherr->MPExpectingKA = mDNSfalse;
			}

		// Having built our ExpectedAnswers list from the questions in this packet, we can definitively
		// remove from our ExpectedAnswers list any records that are suppressed in the very same packet.
		// For answers that are suppressed in subsequent KA list packets, we rely on the MPQ/MPKA counting to track them.
		eap = &ExpectedAnswers;
		while (*eap)
			{
			CacheRecord *rr = *eap;
			if (rr->resrec.InterfaceID == InterfaceID && IdenticalResourceRecord(&m->rec.r.resrec, &rr->resrec))
				{ *eap = rr->NextInKAList; rr->NextInKAList = mDNSNULL; }
			else eap = &rr->NextInKAList;
			}
		
		// See if this Known-Answer is a surprise to us. If so, we shouldn't suppress our own query.
		if (!ourcacherr)
			{
			dqp = &DupQuestions;
			while (*dqp)
				{
				DNSQuestion *q = *dqp;
				if (ResourceRecordAnswersQuestion(&m->rec.r.resrec, q))
					{ *dqp = q->NextInDQList; q->NextInDQList = mDNSNULL; }
				else dqp = &q->NextInDQList;
				}
			}
		m->rec.r.resrec.RecordType = 0;		// Clear RecordType to show we're not still using it
		}

	// ***
	// *** 5. Cancel any additionals that were added because of now-deleted records
	// ***
	for (rr=ResponseRecords; rr; rr=rr->NextResponse)
		if (rr->NR_AdditionalTo && !MustSendRecord(rr->NR_AdditionalTo))
			{ rr->NR_AnswerTo = mDNSNULL; rr->NR_AdditionalTo = mDNSNULL; }

	// ***
	// *** 6. Mark the send flags on the records we plan to send
	// ***
	for (rr=ResponseRecords; rr; rr=rr->NextResponse)
		{
		if (rr->NR_AnswerTo)
			{
			mDNSBool SendMulticastResponse = mDNSfalse;		// Send modern multicast response
			mDNSBool SendUnicastResponse   = mDNSfalse;		// Send modern unicast response (not legacy unicast response)
			
			// If it's been a while since we multicast this, then send a multicast response for conflict detection, etc.
			if (m->timenow - (rr->LastMCTime + TicksTTL(rr)/4) >= 0)
				{
				SendMulticastResponse = mDNStrue;
				// If this record was marked for modern (delayed) unicast response, then mark it as promoted to
				// multicast response instead (don't want to end up ALSO setting SendUnicastResponse in the check below).
				// If this record was marked for legacy unicast response, then we mustn't change the NR_AnswerTo value.
				if (rr->NR_AnswerTo == (mDNSu8*)~1) rr->NR_AnswerTo = (mDNSu8*)~0;
				}
			
			// If the client insists on a multicast response, then we'd better send one
			if      (rr->NR_AnswerTo == (mDNSu8*)~0) SendMulticastResponse = mDNStrue;
			else if (rr->NR_AnswerTo == (mDNSu8*)~1) SendUnicastResponse   = mDNStrue;
			else if (rr->NR_AnswerTo)                SendLegacyResponse    = mDNStrue;
	
			if (SendMulticastResponse || SendUnicastResponse)
				{
#if MDNS_LOG_ANSWER_SUPPRESSION_TIMES
				rr->ImmedAnswerMarkTime = m->timenow;
#endif
				m->NextScheduledResponse = m->timenow;
				// If we're already planning to send this on another interface, just send it on all interfaces
				if (rr->ImmedAnswer && rr->ImmedAnswer != InterfaceID)
					rr->ImmedAnswer = mDNSInterfaceMark;
				else
					{
					rr->ImmedAnswer = InterfaceID;			// Record interface to send it on
					if (SendUnicastResponse) rr->ImmedUnicast = mDNStrue;
					if (srcaddr->type == mDNSAddrType_IPv4)
						{
						if      (mDNSIPv4AddressIsZero(rr->v4Requester))                rr->v4Requester = srcaddr->ip.v4;
						else if (!mDNSSameIPv4Address(rr->v4Requester, srcaddr->ip.v4)) rr->v4Requester = onesIPv4Addr;
						}
					else if (srcaddr->type == mDNSAddrType_IPv6)
						{
						if      (mDNSIPv6AddressIsZero(rr->v6Requester))                rr->v6Requester = srcaddr->ip.v6;
						else if (!mDNSSameIPv6Address(rr->v6Requester, srcaddr->ip.v6)) rr->v6Requester = onesIPv6Addr;
						}
					}
				}
			// If TC flag is set, it means we should expect that additional known answers may be coming in another packet,
			// so we allow roughly half a second before deciding to reply (we've observed inter-packet delays of 100-200ms on 802.11)
			// else, if record is a shared one, spread responses over 100ms to avoid implosion of simultaneous responses
			// else, for a simple unique record reply, we can reply immediately; no need for delay
			if      (query->h.flags.b[0] & kDNSFlag0_TC)            delayresponse = mDNSPlatformOneSecond * 20;	// Divided by 50 = 400ms
			else if (rr->resrec.RecordType == kDNSRecordTypeShared) delayresponse = mDNSPlatformOneSecond;		// Divided by 50 = 20ms
			}
		else if (rr->NR_AdditionalTo && rr->NR_AdditionalTo->NR_AnswerTo == (mDNSu8*)~0)
			{
			// Since additional records are an optimization anyway, we only ever send them on one interface at a time
			// If two clients on different interfaces do queries that invoke the same optional additional answer,
			// then the earlier client is out of luck
			rr->ImmedAdditional = InterfaceID;
			// No need to set m->NextScheduledResponse here
			// We'll send these additional records when we send them, or not, as the case may be
			}
		}

	// ***
	// *** 7. If we think other machines are likely to answer these questions, set our packet suppression timer
	// ***
	if (delayresponse && (!m->SuppressSending || (m->SuppressSending - m->timenow) < (delayresponse + 49) / 50))
		{
#if MDNS_LOG_ANSWER_SUPPRESSION_TIMES
		mDNSs32 oldss = m->SuppressSending;
		if (oldss && delayresponse)
			LogMsg("Current SuppressSending delay%5ld; require%5ld", m->SuppressSending - m->timenow, (delayresponse + 49) / 50);
#endif
		// Pick a random delay:
		// We start with the base delay chosen above (typically either 1 second or 20 seconds),
		// and add a random value in the range 0-5 seconds (making 1-6 seconds or 20-25 seconds).
		// This is an integer value, with resolution determined by the platform clock rate.
		// We then divide that by 50 to get the delay value in ticks. We defer the division until last
		// to get better results on platforms with coarse clock granularity (e.g. ten ticks per second).
		// The +49 before dividing is to ensure we round up, not down, to ensure that even
		// on platforms where the native clock rate is less than fifty ticks per second,
		// we still guarantee that the final calculated delay is at least one platform tick.
		// We want to make sure we don't ever allow the delay to be zero ticks,
		// because if that happens we'll fail the Bonjour Conformance Test.
		// Our final computed delay is 20-120ms for normal delayed replies,
		// or 400-500ms in the case of multi-packet known-answer lists.
		m->SuppressSending = m->timenow + (delayresponse + (mDNSs32)mDNSRandom((mDNSu32)mDNSPlatformOneSecond*5) + 49) / 50;
		if (m->SuppressSending == 0) m->SuppressSending = 1;
#if MDNS_LOG_ANSWER_SUPPRESSION_TIMES
		if (oldss && delayresponse)
			LogMsg("Set     SuppressSending to   %5ld", m->SuppressSending - m->timenow);
#endif
		}

	// ***
	// *** 8. If query is from a legacy client, or from a new client requesting a unicast reply, then generate a unicast response too
	// ***
	if (SendLegacyResponse)
		responseptr = GenerateUnicastResponse(query, end, InterfaceID, LegacyQuery, response, ResponseRecords);

exit:
	m->rec.r.resrec.RecordType = 0;		// Clear RecordType to show we're not still using it
	
	// ***
	// *** 9. Finally, clear our link chains ready for use next time
	// ***
	while (ResponseRecords)
		{
		rr = ResponseRecords;
		ResponseRecords = rr->NextResponse;
		rr->NextResponse    = mDNSNULL;
		rr->NR_AnswerTo     = mDNSNULL;
		rr->NR_AdditionalTo = mDNSNULL;
		}
	
	while (ExpectedAnswers)
		{
		CacheRecord *rr;
		rr = ExpectedAnswers;
		ExpectedAnswers = rr->NextInKAList;
		rr->NextInKAList = mDNSNULL;
		
		// For non-truncated queries, we can definitively say that we should expect
		// to be seeing a response for any records still left in the ExpectedAnswers list
		if (!(query->h.flags.b[0] & kDNSFlag0_TC))
			if (rr->UnansweredQueries == 0 || m->timenow - rr->LastUnansweredTime >= mDNSPlatformOneSecond)
				{
				rr->UnansweredQueries++;
				rr->LastUnansweredTime = m->timenow;
				if (rr->UnansweredQueries > 1)
					debugf("ProcessQuery: (!TC) UAQ %lu MPQ %lu MPKA %lu %s",
						rr->UnansweredQueries, rr->MPUnansweredQ, rr->MPUnansweredKA, CRDisplayString(m, rr));
				SetNextCacheCheckTime(m, rr);
				}

		// If we've seen multiple unanswered queries for this record,
		// then mark it to expire in five seconds if we don't get a response by then.
		if (rr->UnansweredQueries >= MaxUnansweredQueries)
			{
			// Only show debugging message if this record was not about to expire anyway
			if (RRExpireTime(rr) - m->timenow > 4 * mDNSPlatformOneSecond)
				debugf("ProcessQuery: (Max) UAQ %lu MPQ %lu MPKA %lu mDNS_Reconfirm() for %s",
					rr->UnansweredQueries, rr->MPUnansweredQ, rr->MPUnansweredKA, CRDisplayString(m, rr));
			mDNS_Reconfirm_internal(m, rr, kDefaultReconfirmTimeForNoAnswer);
			}
		// Make a guess, based on the multi-packet query / known answer counts, whether we think we
		// should have seen an answer for this. (We multiply MPQ by 4 and MPKA by 5, to allow for
		// possible packet loss of up to 20% of the additional KA packets.)
		else if (rr->MPUnansweredQ * 4 > rr->MPUnansweredKA * 5 + 8)
			{
			// We want to do this conservatively.
			// If there are so many machines on the network that they have to use multi-packet known-answer lists,
			// then we don't want them to all hit the network simultaneously with their final expiration queries.
			// By setting the record to expire in four minutes, we achieve two things:
			// (a) the 90-95% final expiration queries will be less bunched together
			// (b) we allow some time for us to witness enough other failed queries that we don't have to do our own
			mDNSu32 remain = (mDNSu32)(RRExpireTime(rr) - m->timenow) / 4;
			if (remain > 240 * (mDNSu32)mDNSPlatformOneSecond)
				remain = 240 * (mDNSu32)mDNSPlatformOneSecond;
			
			// Only show debugging message if this record was not about to expire anyway
			if (RRExpireTime(rr) - m->timenow > 4 * mDNSPlatformOneSecond)
				debugf("ProcessQuery: (MPQ) UAQ %lu MPQ %lu MPKA %lu mDNS_Reconfirm() for %s",
					rr->UnansweredQueries, rr->MPUnansweredQ, rr->MPUnansweredKA, CRDisplayString(m, rr));

			if (remain <= 60 * (mDNSu32)mDNSPlatformOneSecond)
				rr->UnansweredQueries++;	// Treat this as equivalent to one definite unanswered query
			rr->MPUnansweredQ  = 0;			// Clear MPQ/MPKA statistics
			rr->MPUnansweredKA = 0;
			rr->MPExpectingKA  = mDNSfalse;
			
			if (remain < kDefaultReconfirmTimeForNoAnswer)
				remain = kDefaultReconfirmTimeForNoAnswer;
			mDNS_Reconfirm_internal(m, rr, remain);
			}
		}
	
	while (DupQuestions)
		{
		int i;
		DNSQuestion *q = DupQuestions;
		DupQuestions = q->NextInDQList;
		q->NextInDQList = mDNSNULL;
		i = RecordDupSuppressInfo(q->DupSuppress, m->timenow, InterfaceID, srcaddr->type);
		debugf("ProcessQuery: Recorded DSI for %##s (%s) on %p/%s %d", q->qname.c, DNSTypeName(q->qtype), InterfaceID,
			srcaddr->type == mDNSAddrType_IPv4 ? "v4" : "v6", i);
		}
	
	return(responseptr);
	}

mDNSlocal void mDNSCoreReceiveQuery(mDNS *const m, const DNSMessage *const msg, const mDNSu8 *const end,
	const mDNSAddr *srcaddr, const mDNSIPPort srcport, const mDNSAddr *dstaddr, mDNSIPPort dstport,
	const mDNSInterfaceID InterfaceID)
	{
	mDNSu8    *responseend = mDNSNULL;
	mDNSBool   QueryWasLocalUnicast = srcaddr && dstaddr &&
		!mDNSAddrIsDNSMulticast(dstaddr) && AddressIsLocalSubnet(m, InterfaceID, srcaddr);
	
	if (!InterfaceID && dstaddr && mDNSAddrIsDNSMulticast(dstaddr))
		{
		LogMsg("Ignoring Query from %#-15a:%-5d to %#-15a:%-5d on 0x%p with "
			"%2d Question%s %2d Answer%s %2d Authorit%s %2d Additional%s (Multicast, but no InterfaceID)",
			srcaddr, mDNSVal16(srcport), dstaddr, mDNSVal16(dstport), InterfaceID,
			msg->h.numQuestions,   msg->h.numQuestions   == 1 ? ", " : "s,",
			msg->h.numAnswers,     msg->h.numAnswers     == 1 ? ", " : "s,",
			msg->h.numAuthorities, msg->h.numAuthorities == 1 ? "y,  " : "ies,",
			msg->h.numAdditionals, msg->h.numAdditionals == 1 ? "" : "s");
		return;
		}

	verbosedebugf("Received Query from %#-15a:%-5d to %#-15a:%-5d on 0x%p with "
		"%2d Question%s %2d Answer%s %2d Authorit%s %2d Additional%s",
		srcaddr, mDNSVal16(srcport), dstaddr, mDNSVal16(dstport), InterfaceID,
		msg->h.numQuestions,   msg->h.numQuestions   == 1 ? ", " : "s,",
		msg->h.numAnswers,     msg->h.numAnswers     == 1 ? ", " : "s,",
		msg->h.numAuthorities, msg->h.numAuthorities == 1 ? "y,  " : "ies,",
		msg->h.numAdditionals, msg->h.numAdditionals == 1 ? "" : "s");
	
	responseend = ProcessQuery(m, msg, end, srcaddr, InterfaceID,
		!mDNSSameIPPort(srcport, MulticastDNSPort), mDNSAddrIsDNSMulticast(dstaddr), QueryWasLocalUnicast, &m->omsg);

	if (responseend)	// If responseend is non-null, that means we built a unicast response packet
		{
		debugf("Unicast Response: %d Question%s, %d Answer%s, %d Additional%s to %#-15a:%d on %p/%ld",
			m->omsg.h.numQuestions,   m->omsg.h.numQuestions   == 1 ? "" : "s",
			m->omsg.h.numAnswers,     m->omsg.h.numAnswers     == 1 ? "" : "s",
			m->omsg.h.numAdditionals, m->omsg.h.numAdditionals == 1 ? "" : "s",
			srcaddr, mDNSVal16(srcport), InterfaceID, srcaddr->type);
		mDNSSendDNSMessage(m, &m->omsg, responseend, InterfaceID, srcaddr, srcport, mDNSNULL, mDNSNULL);
		}
	}

mDNSlocal mDNSBool TrustedSource(const mDNS *const m, const mDNSAddr *const srcaddr)
	{
	DNSServer *s;
	(void)m; // Unused
	(void)srcaddr; // Unused
	for (s = m->DNSServers; s; s = s->next)
		if (mDNSSameAddress(srcaddr, &s->addr)) return(mDNStrue);
	return(mDNSfalse);
	}

mDNSlocal const DNSQuestion *ExpectingUnicastResponseForQuestion(const mDNS *const m, const mDNSOpaque16 id, const DNSQuestion *const question)
	{
	DNSQuestion *q;
	for (q = m->Questions; q; q=q->next)
		if (mDNSSameOpaque16(q->TargetQID, id)                 &&
			q->qtype                  == question->qtype       &&
			q->qclass                 == question->qclass      &&
			q->qnamehash              == question->qnamehash   &&
			SameDomainName(&q->qname, &question->qname))
			return(q);
	return(mDNSNULL);
	}

mDNSlocal mDNSBool ExpectingUnicastResponseForRecord(mDNS *const m, const mDNSAddr *const srcaddr, const mDNSBool SrcLocal, const mDNSOpaque16 id, const CacheRecord *const rr)
	{
	DNSQuestion *q;
	(void)id;
	for (q = m->Questions; q; q=q->next)
		if (ResourceRecordAnswersQuestion(&rr->resrec, q))
			{
			if (!mDNSOpaque16IsZero(q->TargetQID))
				{
				// For now we don't do this check -- for LLQ updates, the ID doesn't seem to match the ID in the question
				// if (mDNSSameOpaque16(q->TargetQID, id)
					{
					if (mDNSSameAddress(srcaddr, &q->Target))                   return(mDNStrue);
					if (mDNSSameOpaque16(q->TargetQID, id))                     return(mDNStrue);
				//	if (q->LongLived && mDNSSameAddress(srcaddr, &q->servAddr)) return(mDNStrue); Shouldn't need this now that we have LLQType checking
					if (TrustedSource(m, srcaddr))                              return(mDNStrue);
					LogOperation("WARNING: Ignoring suspect uDNS response for %##s (%s) %#a from %#a: %s",
						q->qname.c, DNSTypeName(q->qtype), &q->Target, srcaddr, CRDisplayString(m, rr));
					return(mDNSfalse);
					}
				}
			else
				{
				if (SrcLocal && q->ExpectUnicastResp && (mDNSu32)(m->timenow - q->ExpectUnicastResp) < (mDNSu32)(mDNSPlatformOneSecond*2))
					return(mDNStrue);
				}
			}
	return(mDNSfalse);
	}

mDNSexport CacheRecord *CreateNewCacheEntry(mDNS *const m, const mDNSu32 slot, CacheGroup *cg)
	{
	CacheRecord *rr = mDNSNULL;

	// Certain data types need more space for in-memory storage than their in-packet rdlength would imply
	// Currently this applies only to rdata types containing more than one domainname,
	// or types where the domainname is not the last item in the structure
	mDNSu16 RDLength;
	switch (m->rec.r.resrec.rrtype)
		{
		case kDNSType_SOA: RDLength = sizeof(rdataSOA);         break;
		case kDNSType_RP:  RDLength = sizeof(rdataRP);          break;
		case kDNSType_PX:  RDLength = sizeof(rdataPX);          break;
		default:           RDLength = m->rec.r.resrec.rdlength; break;
		}

	if (!cg) cg = GetCacheGroup(m, slot, &m->rec.r.resrec);	// If we don't have a CacheGroup for this name, make one now
	if (cg)  rr = GetCacheRecord(m, cg, RDLength);	// Make a cache record, being careful not to recycle cg
	if (!rr) NoCacheAnswer(m, &m->rec.r);
	else
		{
		RData *saveptr = rr->resrec.rdata;		// Save the rr->resrec.rdata pointer
		*rr = m->rec.r;							// Block copy the CacheRecord object
		rr->resrec.rdata = saveptr;				// Restore rr->resrec.rdata after the structure assignment
		rr->resrec.name  = cg->name;			// And set rr->resrec.name to point into our CacheGroup header

		// If this is an oversized record with external storage allocated, copy rdata to external storage
		if      (rr->resrec.rdata == (RData*)&rr->rdatastorage && RDLength > InlineCacheRDSize)
			LogMsg("rr->resrec.rdata == &rr->rdatastorage but length > InlineCacheRDSize %##s", m->rec.r.resrec.name->c);
		else if (rr->resrec.rdata != (RData*)&rr->rdatastorage && RDLength <= InlineCacheRDSize)
			LogMsg("rr->resrec.rdata != &rr->rdatastorage but length <= InlineCacheRDSize %##s", m->rec.r.resrec.name->c);
		if (RDLength > InlineCacheRDSize)
			mDNSPlatformMemCopy(rr->resrec.rdata, m->rec.r.resrec.rdata, sizeofRDataHeader + RDLength);

		rr->next = mDNSNULL;					// Clear 'next' pointer
		*(cg->rrcache_tail) = rr;				// Append this record to tail of cache slot list
		cg->rrcache_tail = &(rr->next);			// Advance tail pointer
		if (rr->resrec.RecordType == kDNSRecordTypePacketNegative)
			rr->DelayDelivery = NonZeroTime(m->timenow);
		else if (rr->resrec.RecordType & kDNSRecordTypePacketUniqueMask &&			// If marked unique,
			rr->resrec.rdata->MaxRDLength != 0)										// and non-negative, assume we may have
			rr->DelayDelivery = NonZeroTime(m->timenow + mDNSPlatformOneSecond);	// to delay delivery of this 'add' event
		else
			rr->DelayDelivery = CheckForSoonToExpireRecords(m, rr->resrec.name, rr->resrec.namehash, slot);

		CacheRecordAdd(m, rr);  // CacheRecordAdd calls SetNextCacheCheckTime(m, rr); for us
		}
	return(rr);
	}

// CountLabels() returns number of labels in name, excluding final root label
// (e.g. for "apple.com." CountLabels returns 2.)
mDNSlocal int CountLabels(const domainname *d)
	{
	int count = 0;
	const mDNSu8 *ptr;
	for (ptr = d->c; *ptr; ptr = ptr + ptr[0] + 1) count++;
	return count;
	}

// SkipLeadingLabels skips over the first 'skip' labels in the domainname,
// returning a pointer to the suffix with 'skip' labels removed.
mDNSlocal const domainname *SkipLeadingLabels(const domainname *d, int skip)
	{
	while (skip > 0 && d->c[0]) { d = (const domainname *)(d->c + 1 + d->c[0]); skip--; }
	return(d);
	}

mDNSexport void GrantCacheExtensions(mDNS *const m, DNSQuestion *q, mDNSu32 lease)
	{
	CacheRecord *rr;
	const mDNSu32 slot = HashSlot(&q->qname);
	CacheGroup *cg = CacheGroupForName(m, slot, q->qnamehash, &q->qname);
	for (rr = cg ? cg->members : mDNSNULL; rr; rr=rr->next)
		if (rr->CRActiveQuestion == q)
			{
			//LogOperation("GrantCacheExtensions: new lease %d / %s", lease, CRDisplayString(m, rr));
			rr->TimeRcvd = m->timenow;
			rr->resrec.rroriginalttl = lease;
			rr->UnansweredQueries = 0;
			rr->MPUnansweredQ     = 0;
			rr->MPUnansweredKA    = 0;
			rr->MPExpectingKA     = mDNSfalse;
			SetNextCacheCheckTime(m, rr);
			}
	}

mDNSlocal mDNSu32 GetEffectiveTTL(const uDNS_LLQType LLQType, const mDNSu32 ttl_seconds)
	{
	mDNSu32 ttl = ttl_seconds;
	if      (LLQType == uDNS_LLQ_Poll)  ttl = LLQ_POLL_INTERVAL * 2 / mDNSPlatformOneSecond;
	else if (LLQType == uDNS_LLQ_Setup) ttl = kLLQ_DefLease;
	else if (LLQType == uDNS_LLQ_Events)
		{
		// If the TTL is -1 for uDNS LLQ event packet, that means "remove"
		if (ttl == 0xFFFFFFFF) ttl = 0;
		else                   ttl = kLLQ_DefLease;
		}
	else	// else not LLQ (standard uDNS response)
		{
		// Adjustment factor to avoid race condition:
		// Suppose real record as TTL of 3600, and our local caching server has held it for 3500 seconds, so it returns an aged TTL of 100.
		// If we do our normal refresh at 80% of the TTL, our local caching server will return 20 seconds, so we'll do another
		// 80% refresh after 16 seconds, and then the server will return 4 seconds, and so on, in the fashion of Zeno's paradox.
		// To avoid this, we extend the record's effective TTL to give it a little extra grace period.
		// We adjust the 100 second TTL to 126. This means that when we do our 80% query at 101 seconds,
		// the cached copy at our local caching server will already have expired, so the server will be forced
		// to fetch a fresh copy from the authoritative server, and then return a fresh record with the full TTL of 3600 seconds.
		ttl += ttl/4 + 2;

		// For mDNS, TTL zero means "delete this record"
		// For uDNS, TTL zero means: this data is true at this moment, but don't cache it.
		// For the sake of network efficiency, we impose a minimum effective TTL of 15 seconds.
		// If we allow a TTL of less than 2 seconds things really break (e.g. we end up making a negative cache entry).
		// In the future we may want to revisit this and consider properly supporting non-cached (TTL=0) uDNS answers.
		if (ttl < 15) ttl = 15;
		}
	
	return ttl > ttl_seconds ? ttl : ttl_seconds;	// Return the larger of the packet TTL and our calculated effective TTL
	}

// NOTE: mDNSCoreReceiveResponse calls mDNS_Deregister_internal which can call a user callback, which may change
// the record list and/or question list.
// Any code walking either list must use the CurrentQuestion and/or CurrentRecord mechanism to protect against this.
mDNSlocal void mDNSCoreReceiveResponse(mDNS *const m,
	const DNSMessage *const response, const mDNSu8 *end,
	const mDNSAddr *srcaddr, const mDNSIPPort srcport, const mDNSAddr *dstaddr, mDNSIPPort dstport,
	const mDNSInterfaceID InterfaceID)
	{
	int i;
	mDNSBool ResponseMCast    = dstaddr && mDNSAddrIsDNSMulticast(dstaddr);
	mDNSBool ResponseSrcLocal = !srcaddr || AddressIsLocalSubnet(m, InterfaceID, srcaddr);
	uDNS_LLQType LLQType      = uDNS_recvLLQResponse(m, response, end, srcaddr, srcport);

	// "(CacheRecord*)1" is a special (non-zero) end-of-list marker
	// We use this non-zero marker so that records in our CacheFlushRecords list will always have NextInCFList
	// set non-zero, and that tells GetCacheEntity() that they're not, at this moment, eligible for recycling.
	CacheRecord *CacheFlushRecords = (CacheRecord*)1;
	CacheRecord **cfp = &CacheFlushRecords;

	// All records in a DNS response packet are treated as equally valid statements of truth. If we want
	// to guard against spoof responses, then the only credible protection against that is cryptographic
	// security, e.g. DNSSEC., not worring about which section in the spoof packet contained the record
	int totalrecords = response->h.numAnswers + response->h.numAuthorities + response->h.numAdditionals;
	const mDNSu8 *ptr = response->data;

	// Currently used only for display in debugging message
	(void)srcport;
	(void)dstport;

	debugf("Received Response from %#-15a addressed to %#-15a on %p with "
		"%2d Question%s %2d Answer%s %2d Authorit%s %2d Additional%s LLQType %d",
		srcaddr, dstaddr, InterfaceID,
		response->h.numQuestions,   response->h.numQuestions   == 1 ? ", " : "s,",
		response->h.numAnswers,     response->h.numAnswers     == 1 ? ", " : "s,",
		response->h.numAuthorities, response->h.numAuthorities == 1 ? "y,  " : "ies,",
		response->h.numAdditionals, response->h.numAdditionals == 1 ? "" : "s", LLQType);

	// 1. We ignore questions (if any) in mDNS response packets
	// 2. If this is an LLQ response, we handle it much the same
	// 3. If we get a uDNS UDP response with the TC (truncated) bit set, then we can't treat this
	//    answer as being the authoritative complete RRSet, and respond by deleting all other
	//    matching cache records that don't appear in this packet.
	// Otherwise, this is a authoritative uDNS answer, so arrange for any stale records to be purged
	if (ResponseMCast || LLQType == uDNS_LLQ_Events || (response->h.flags.b[0] & kDNSFlag0_TC))
		ptr = LocateAnswers(response, end);
	// Otherwise, for one-shot queries, any answers in our cache that are not also contained
	// in this response packet are immediately deemed to be invalid.
	else
		{
		// We could possibly combine this with the similar loop at the end of this function --
		// instead of tagging cache records here and then rescuing them if we find them in the answer section,
		// we could instead use the "m->PktNum" mechanism to tag each cache record with the packet number in
		// which it was received (or refreshed), and then at the end if we find any cache records which
		// answer questions in this packet's question section, but which aren't tagged with this packet's
		// packet number, then we deduce they are old and delete them
		for (i = 0; i < response->h.numQuestions && ptr && ptr < end; i++)
			{
			DNSQuestion q;
			ptr = getQuestion(response, ptr, end, InterfaceID, &q);
			if (ptr && ExpectingUnicastResponseForQuestion(m, response->h.id, &q))
				{
				CacheRecord *rr;
				const mDNSu32 slot = HashSlot(&q.qname);
				CacheGroup *cg = CacheGroupForName(m, slot, q.qnamehash, &q.qname);
				for (rr = cg ? cg->members : mDNSNULL; rr; rr=rr->next)
					if (SameNameRecordAnswersQuestion(&rr->resrec, &q))
						{
						//LogMsg("uDNS Q for %s", CRDisplayString(m, rr));
						rr->resrec.rroriginalttl = 0;
						rr->TimeRcvd          = m->timenow;
						rr->UnansweredQueries = MaxUnansweredQueries;
						}
				}
			}
		}

	for (i = 0; i < totalrecords && ptr && ptr < end; i++)
		{
		// All responses sent via LL multicast are acceptable for caching
		// All responses received over our outbound TCP connections are acceptable for caching
		mDNSBool AcceptableResponse = ResponseMCast || !dstaddr || LLQType;
		// (Note that just because we are willing to cache something, that doesn't necessarily make it a trustworthy answer
		// to any specific question -- any code reading records from the cache needs to make that determination for itself.)

		const mDNSu8 RecordType = (mDNSu8)((i < response->h.numAnswers) ? kDNSRecordTypePacketAns : kDNSRecordTypePacketAdd);
		ptr = GetLargeResourceRecord(m, response, ptr, end, InterfaceID, RecordType, &m->rec);
		if (!ptr) goto exit;		// Break out of the loop and clean up our CacheFlushRecords list before exiting
		// Don't want to cache OPT or TSIG pseudo-RRs
		if (m->rec.r.resrec.rrtype == kDNSType_OPT || m->rec.r.resrec.rrtype == kDNSType_TSIG)
			{ m->rec.r.resrec.RecordType = 0; continue; }
		
		// When we receive uDNS LLQ responses, we assume a long cache lifetime --
		// In the case of active LLQs, we'll get remove events when the records actually do go away
		// In the case of polling LLQs, we assume the record remains valid until the next poll
		if (!mDNSOpaque16IsZero(response->h.id))
			m->rec.r.resrec.rroriginalttl = GetEffectiveTTL(LLQType, m->rec.r.resrec.rroriginalttl);

		// If response was not sent via LL multicast,
		// then see if it answers a recent query of ours, which would also make it acceptable for caching.
		if (!AcceptableResponse) AcceptableResponse = ExpectingUnicastResponseForRecord(m, srcaddr, ResponseSrcLocal, response->h.id, &m->rec.r);

		// 1. Check that this packet resource record does not conflict with any of ours
		if (mDNSOpaque16IsZero(response->h.id))
			{
			if (m->CurrentRecord)
				LogMsg("mDNSCoreReceiveResponse ERROR m->CurrentRecord already set %s", ARDisplayString(m, m->CurrentRecord));
			m->CurrentRecord = m->ResourceRecords;
			while (m->CurrentRecord)
				{
				AuthRecord *rr = m->CurrentRecord;
				m->CurrentRecord = rr->next;
				// We accept all multicast responses, and unicast responses resulting from queries we issued
				// For other unicast responses, this code accepts them only for responses with an
				// (apparently) local source address that pertain to a record of our own that's in probing state
				if (!AcceptableResponse && !(ResponseSrcLocal && rr->resrec.RecordType == kDNSRecordTypeUnique)) continue;
				if (PacketRRMatchesSignature(&m->rec.r, rr))		// If interface, name, type (if shared record) and class match...
					{
					// ... check to see if type and rdata are identical
					if (m->rec.r.resrec.rrtype == rr->resrec.rrtype && SameRData(&m->rec.r.resrec, &rr->resrec))
						{
						// If the RR in the packet is identical to ours, just check they're not trying to lower the TTL on us
						if (m->rec.r.resrec.rroriginalttl >= rr->resrec.rroriginalttl/2 || m->SleepState)
							{
							// If we were planning to send on this -- and only this -- interface, then we don't need to any more
							if      (rr->ImmedAnswer == InterfaceID) { rr->ImmedAnswer = mDNSNULL; rr->ImmedUnicast = mDNSfalse; }
							}
						else
							{
							if      (rr->ImmedAnswer == mDNSNULL)    { rr->ImmedAnswer = InterfaceID;       m->NextScheduledResponse = m->timenow; }
							else if (rr->ImmedAnswer != InterfaceID) { rr->ImmedAnswer = mDNSInterfaceMark; m->NextScheduledResponse = m->timenow; }
							}
						}
					// else, the packet RR has different type or different rdata -- check to see if this is a conflict
					else if (m->rec.r.resrec.rroriginalttl > 0 && PacketRRConflict(m, rr, &m->rec.r))
						{
						debugf("mDNSCoreReceiveResponse: Our Record: %08lX %s", rr->     resrec.rdatahash, ARDisplayString(m, rr));
						debugf("mDNSCoreReceiveResponse: Pkt Record: %08lX %s", m->rec.r.resrec.rdatahash, CRDisplayString(m, &m->rec.r));
	
						// If this record is marked DependentOn another record for conflict detection purposes,
						// then *that* record has to be bumped back to probing state to resolve the conflict
						while (rr->DependentOn) rr = rr->DependentOn;
	
						// If we've just whacked this record's ProbeCount, don't need to do it again
						if (rr->ProbeCount <= DefaultProbeCountForTypeUnique)
							{
							// If we'd previously verified this record, put it back to probing state and try again
							if (rr->resrec.RecordType == kDNSRecordTypeVerified)
								{
								debugf("mDNSCoreReceiveResponse: Reseting to Probing: %##s (%s)", rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype));
								rr->resrec.RecordType     = kDNSRecordTypeUnique;
								rr->ProbeCount     = DefaultProbeCountForTypeUnique + 1;
								rr->ThisAPInterval = DefaultAPIntervalForRecordType(kDNSRecordTypeUnique);
								InitializeLastAPTime(m, rr);
								RecordProbeFailure(m, rr);	// Repeated late conflicts also cause us to back off to the slower probing rate
								}
							// If we're probing for this record, we just failed
							else if (rr->resrec.RecordType == kDNSRecordTypeUnique)
								{
								debugf("mDNSCoreReceiveResponse: Will rename %##s (%s)", rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype));
								mDNS_Deregister_internal(m, rr, mDNS_Dereg_conflict);
								}
							// We assumed this record must be unique, but we were wrong.
							// (e.g. There are two mDNSResponders on the same machine giving
							// different answers for the reverse mapping record.)
							// This is simply a misconfiguration, and we don't try to recover from it.
							else if (rr->resrec.RecordType == kDNSRecordTypeKnownUnique)
								{
								debugf("mDNSCoreReceiveResponse: Unexpected conflict on %##s (%s) -- discarding our record",
									rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype));
								mDNS_Deregister_internal(m, rr, mDNS_Dereg_conflict);
								}
							else
								debugf("mDNSCoreReceiveResponse: Unexpected record type %X %##s (%s)",
									rr->resrec.RecordType, rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype));
							}
						}
					// Else, matching signature, different type or rdata, but not a considered a conflict.
					// If the packet record has the cache-flush bit set, then we check to see if we
					// have any record(s) of the same type that we should re-assert to rescue them
					// (see note about "multi-homing and bridged networks" at the end of this function).
					else if (m->rec.r.resrec.rrtype == rr->resrec.rrtype)
						if ((m->rec.r.resrec.RecordType & kDNSRecordTypePacketUniqueMask) && m->timenow - rr->LastMCTime > mDNSPlatformOneSecond/2)
							{ rr->ImmedAnswer = mDNSInterfaceMark; m->NextScheduledResponse = m->timenow; }
					}
				}
			}

		// 2. See if we want to add this packet resource record to our cache
		// We only try to cache answers if we have a cache to put them in
		// Also, we ignore any apparent attempts at cache poisoning unicast to us that do not answer any outstanding active query
		if (m->rrcache_size && AcceptableResponse)
			{
			const mDNSu32 slot = HashSlot(m->rec.r.resrec.name);
			CacheGroup *cg = CacheGroupForRecord(m, slot, &m->rec.r.resrec);
			CacheRecord *rr;
			// 2a. Check if this packet resource record is already in our cache
			for (rr = cg ? cg->members : mDNSNULL; rr; rr=rr->next)
				{
				// If we found this exact resource record, refresh its TTL
				if (rr->resrec.InterfaceID == InterfaceID && IdenticalSameNameRecord(&m->rec.r.resrec, &rr->resrec))
					{
					if (m->rec.r.resrec.rdlength > InlineCacheRDSize)
						verbosedebugf("Found record size %5d interface %p already in cache: %s",
							m->rec.r.resrec.rdlength, InterfaceID, CRDisplayString(m, &m->rec.r));
					rr->TimeRcvd  = m->timenow;
					
					if (m->rec.r.resrec.RecordType & kDNSRecordTypePacketUniqueMask)
						{
						// If this packet record has the kDNSClass_UniqueRRSet flag set, then add it to our cache flushing list
						if (rr->NextInCFList == mDNSNULL && cfp != &rr->NextInCFList && LLQType != uDNS_LLQ_Events)
							{ *cfp = rr; cfp = &rr->NextInCFList; *cfp = (CacheRecord*)1; }

						// If this packet record is marked unique, and our previous cached copy was not, then fix it
						if (!(rr->resrec.RecordType & kDNSRecordTypePacketUniqueMask))
							{
							DNSQuestion *q;
							for (q = m->Questions; q; q=q->next) if (ResourceRecordAnswersQuestion(&rr->resrec, q)) q->UniqueAnswers++;
							rr->resrec.RecordType = m->rec.r.resrec.RecordType;
							}
						}

					if (!mDNSPlatformMemSame(m->rec.r.resrec.rdata->u.data, rr->resrec.rdata->u.data, m->rec.r.resrec.rdlength))
						{
						// If the rdata of the packet record differs in name capitalization from the record in our cache
						// then mDNSPlatformMemSame will detect this. In this case, throw the old record away, so that clients get
						// a 'remove' event for the record with the old capitalization, and then an 'add' event for the new one.
						rr->resrec.rroriginalttl = 0;
						rr->UnansweredQueries = MaxUnansweredQueries;
						SetNextCacheCheckTime(m, rr);
						// DO NOT break out here -- we want to continue as if we never found it
						}
					else if (m->rec.r.resrec.rroriginalttl > 0)
						{
						//if (rr->resrec.rroriginalttl == 0) LogMsg("uDNS rescuing %s", CRDisplayString(m, rr));
						rr->resrec.rroriginalttl = m->rec.r.resrec.rroriginalttl;
						rr->UnansweredQueries = 0;
						rr->MPUnansweredQ     = 0;
						rr->MPUnansweredKA    = 0;
						rr->MPExpectingKA     = mDNSfalse;
						SetNextCacheCheckTime(m, rr);
						break;
						}
					else
						{
						// If the packet TTL is zero, that means we're deleting this record.
						// To give other hosts on the network a chance to protest, we push the deletion
						// out one second into the future. Also, we set UnansweredQueries to MaxUnansweredQueries.
						// Otherwise, we'll do final queries for this record at 80% and 90% of its apparent
						// lifetime (800ms and 900ms from now) which is a pointless waste of network bandwidth.
						rr->resrec.rroriginalttl = 1;
						rr->UnansweredQueries = MaxUnansweredQueries;
						SetNextCacheCheckTime(m, rr);
						break;
						}
					}
				}

			// If packet resource record not in our cache, add it now
			// (unless it is just a deletion of a record we never had, in which case we don't care)
			if (!rr && m->rec.r.resrec.rroriginalttl > 0)
				{
				rr = CreateNewCacheEntry(m, slot, cg);
				if (rr && (rr->resrec.RecordType & kDNSRecordTypePacketUniqueMask) && LLQType != uDNS_LLQ_Events)
					{ *cfp = rr; cfp = &rr->NextInCFList; *cfp = (CacheRecord*)1; }
				}
			}
		m->rec.r.resrec.RecordType = 0;		// Clear RecordType to show we're not still using it
		}

exit:
	m->rec.r.resrec.RecordType = 0;		// Clear RecordType to show we're not still using it

	// If we've just received one or more records with their cache flush bits set,
	// then scan that cache slot to see if there are any old stale records we need to flush
	while (CacheFlushRecords != (CacheRecord*)1)
		{
		CacheRecord *r1 = CacheFlushRecords, *r2;
		const mDNSu32 slot = HashSlot(r1->resrec.name);
		const CacheGroup *cg = CacheGroupForRecord(m, slot, &r1->resrec);
		CacheFlushRecords = CacheFlushRecords->NextInCFList;
		r1->NextInCFList = mDNSNULL;
		
		// Look for records in the cache with the same signature as this new one with the cache flush
		// bit set, and either (a) if they're fresh, just make sure the whole RRSet has the same TTL
		// (as required by DNS semantics) or (b) if they're old, mark them for deletion in one second.
		// We make these TTL adjustments *only* for records that still have *more* than one second
		// remaining to live. Otherwise, a record that we tagged for deletion half a second ago
		// (and now has half a second remaining) could inadvertently get its life extended, by either
		// (a) if we got an explicit goodbye packet half a second ago, the record would be considered
		// "fresh" and would be incorrectly resurrected back to the same TTL as the rest of the RRSet,
		// or (b) otherwise, the record would not be fully resurrected, but would be reset to expire
		// in one second, thereby inadvertently delaying its actual expiration, instead of hastening it.
		// If this were to happen repeatedly, the record's expiration could be deferred indefinitely.
		// To avoid this, we need to ensure that the cache flushing operation will only act to
		// *decrease* a record's remaining lifetime, never *increase* it. If a record has less than
		// one second to go, we simply leave it alone, and leave it to expire at its assigned time.
		for (r2 = cg ? cg->members : mDNSNULL; r2; r2=r2->next)
			if (r1->resrec.InterfaceID == r2->resrec.InterfaceID &&
				r1->resrec.rrtype      == r2->resrec.rrtype &&
				r1->resrec.rrclass     == r2->resrec.rrclass)
				if (RRExpireTime(r2) - m->timenow > mDNSPlatformOneSecond)
					{
					// If record is recent, just ensure the whole RRSet has the same TTL (as required by DNS semantics)
					// else, if record is old, mark it to be flushed
					if (m->timenow - r2->TimeRcvd < mDNSPlatformOneSecond)
						{
						// If we find mismatched TTLs in an RRSet, correct them.
						// We only do this for records with a TTL of 2 or higher. It's possible to have a
						// goodbye announcement with the cache flush bit set (or a case change on record rdata,
						// which we treat as a goodbye followed by an addition) and in that case it would be
						// inappropriate to synchronize all the other records to a TTL of 0 (or 1).
						// We suppress the message for the specific case of correcting from 240 to 60 for type TXT,
						// because certain early Bonjour devices are known to have this specific mismatch, and
						// there's no point filling syslog with messages about something we already know about.
						// We also don't log this for uDNS responses, since a caching name server is obliged
						// to give us an aged TTL to correct for how long it has held the record,
						// so our received TTLs are expected to vary in that case
						if (r2->resrec.rroriginalttl != r1->resrec.rroriginalttl && r1->resrec.rroriginalttl > 1)
							{
							if (r2->resrec.rroriginalttl != 240 && r1->resrec.rroriginalttl != 60 && r2->resrec.rrtype != kDNSType_TXT &&
								mDNSOpaque16IsZero(response->h.id))
								LogMsg("Correcting TTL from %4d to %4d for %s",
									r2->resrec.rroriginalttl, r1->resrec.rroriginalttl, CRDisplayString(m, r2));
							r2->resrec.rroriginalttl = r1->resrec.rroriginalttl;
							}
						}
					else				// else, if record is old, mark it to be flushed
						{
						verbosedebugf("Cache flush %p X %p %s", r1, r2, CRDisplayString(m, r2));
						// We set stale records to expire in one second.
						// This gives the owner a chance to rescue it if necessary.
						// This is important in the case of multi-homing and bridged networks:
						//   Suppose host X is on Ethernet. X then connects to an AirPort base station, which happens to be
						//   bridged onto the same Ethernet. When X announces its AirPort IP address with the cache-flush bit
						//   set, the AirPort packet will be bridged onto the Ethernet, and all other hosts on the Ethernet
						//   will promptly delete their cached copies of the (still valid) Ethernet IP address record.
						//   By delaying the deletion by one second, we give X a change to notice that this bridging has
						//   happened, and re-announce its Ethernet IP address to rescue it from deletion from all our caches.
						// We set UnansweredQueries to MaxUnansweredQueries to avoid expensive and unnecessary
						// final expiration queries for this record.
						r2->resrec.rroriginalttl = 1;
						r2->UnansweredQueries = MaxUnansweredQueries;
						}
					r2->TimeRcvd = m->timenow;
					SetNextCacheCheckTime(m, r2);
					}
		if (r1->DelayDelivery)	// If we were planning to delay delivery of this record, see if we still need to
			{
			// Note, only need to call SetNextCacheCheckTime() when DelayDelivery is set, not when it's cleared
			r1->DelayDelivery = CheckForSoonToExpireRecords(m, r1->resrec.name, r1->resrec.namehash, slot);
			if (!r1->DelayDelivery) CacheRecordDeferredAdd(m, r1);
			}
		}

	// See if we need to generate negative cache entries for unanswered unicast questions
	ptr = response->data;
	for (i = 0; i < response->h.numQuestions && ptr && ptr < end; i++)
		{
		DNSQuestion q;
		ptr = getQuestion(response, ptr, end, InterfaceID, &q);
		if (ptr && ExpectingUnicastResponseForQuestion(m, response->h.id, &q))
			{
			CacheRecord *rr;
			mDNSu32 slot = HashSlot(&q.qname);
			CacheGroup *cg = CacheGroupForName(m, slot, q.qnamehash, &q.qname);
			for (rr = cg ? cg->members : mDNSNULL; rr; rr=rr->next)
				if (SameNameRecordAnswersQuestion(&rr->resrec, &q))
					{
					// 1. If we got a fresh answer to this query, then don't need to generate a negative entry
					// 2. If we already had a negative entry which we were about to discard, then we should resurrect it
					if (rr->resrec.rroriginalttl) break;
					if (rr->resrec.RecordType == kDNSRecordTypePacketNegative) break;
					}

			if (!rr || rr->resrec.RecordType == kDNSRecordTypePacketNegative)
				{
				// We start off assuming a negative caching TTL of 60 seconds
				// but then look to see if we can find an SOA authority record to tell us a better value we should be using
				mDNSu32 negttl = GetEffectiveTTL(LLQType, 60);
				int repeat = 0;
				const domainname *name = &q.qname;
				mDNSu32           hash = q.qnamehash;

				// If we're going to make (or update) a negative entry, then look for the appropriate TTL from the SOA record
				if (response->h.numAuthorities && (ptr = LocateAuthorities(response, end)) != mDNSNULL)
					{
					ptr = GetLargeResourceRecord(m, response, ptr, end, InterfaceID, kDNSRecordTypePacketAuth, &m->rec);
					if (ptr && m->rec.r.resrec.rrtype == kDNSType_SOA)
						{
						mDNSu32 ttl_s = m->rec.r.resrec.rroriginalttl < m->rec.r.resrec.rdata->u.soa.min ?
										m->rec.r.resrec.rroriginalttl : m->rec.r.resrec.rdata->u.soa.min;
						if (negttl < ttl_s) negttl = ttl_s;
	
						// Special check for SOA queries: If we queried for a.b.c.d.com, and got no answer,
						// with an Authority Section SOA record for d.com, then this is a hint that the authority
						// is d.com, and consequently SOA records b.c.d.com and c.d.com don't exist either.
						// To do this we set the repeat count so the while loop below will make a series of negative cache entries for us
						if (q.qtype == kDNSType_SOA)
							{
							int qcount = CountLabels(&q.qname);
							int scount = CountLabels(m->rec.r.resrec.name);
							if (qcount - 1 > scount)
								if (SameDomainName(SkipLeadingLabels(&q.qname, qcount - scount), m->rec.r.resrec.name))
									repeat = qcount - 1 - scount;
							}
						}
					m->rec.r.resrec.RecordType = 0;		// Clear RecordType to show we're not still using it
					}

				// If we already had a negative cache entry just update it, else make one or more new negative cache entries
				if (rr)
					{
					rr->TimeRcvd = m->timenow;
					rr->resrec.rroriginalttl = negttl;
					}
				else while (1)
					{
					LogOperation("Making negative cache entry TTL %d for %##s (%s)", negttl, name->c, DNSTypeName(q.qtype));
					MakeNegativeCacheRecord(m, name, hash, q.qtype, q.qclass, negttl);
					CreateNewCacheEntry(m, slot, cg);
					m->rec.r.resrec.RecordType = 0;		// Clear RecordType to show we're not still using it
					if (!repeat) break;
					repeat--;
					name = (const domainname *)(name->c + 1 + name->c[0]);
					hash = DomainNameHashValue(name);
					slot = HashSlot(name);
					cg   = CacheGroupForName(m, slot, hash, name);
					}
				}
			}
		}
	}

mDNSexport void MakeNegativeCacheRecord(mDNS *const m, const domainname *const name, const mDNSu32 namehash, const mDNSu16 rrtype, const mDNSu16 rrclass, mDNSu32 ttl_seconds)
	{
	// Create empty resource record
	m->rec.r.resrec.RecordType    = kDNSRecordTypePacketNegative;
	m->rec.r.resrec.InterfaceID   = mDNSInterface_Any;
	m->rec.r.resrec.name          = name;	// Will be updated to point to cg->name when we call CreateNewCacheEntry
	m->rec.r.resrec.rrtype        = rrtype;
	m->rec.r.resrec.rrclass       = rrclass;
	m->rec.r.resrec.rroriginalttl = ttl_seconds;
	m->rec.r.resrec.rdlength      = 0;
	m->rec.r.resrec.rdestimate    = 0;
	m->rec.r.resrec.namehash      = namehash;
	m->rec.r.resrec.rdatahash     = 0;
	m->rec.r.resrec.rdata = (RData*)&m->rec.r.rdatastorage;
	m->rec.r.resrec.rdata->MaxRDLength = m->rec.r.resrec.rdlength;

	m->rec.r.NextInKAList       = mDNSNULL;
	m->rec.r.TimeRcvd           = m->timenow;
	m->rec.r.DelayDelivery      = 0;
	m->rec.r.NextRequiredQuery  = m->timenow;
	m->rec.r.LastUsed           = m->timenow;
	m->rec.r.CRActiveQuestion   = mDNSNULL;
	m->rec.r.UnansweredQueries  = 0;
	m->rec.r.LastUnansweredTime = 0;
	m->rec.r.MPUnansweredQ      = 0;
	m->rec.r.MPLastUnansweredQT = 0;
	m->rec.r.MPUnansweredKA     = 0;
	m->rec.r.MPExpectingKA      = mDNSfalse;
	m->rec.r.NextInCFList       = mDNSNULL;
	}

mDNSexport void mDNSCoreReceive(mDNS *const m, void *const pkt, const mDNSu8 *const end,
	const mDNSAddr *const srcaddr, const mDNSIPPort srcport, const mDNSAddr *dstaddr, const mDNSIPPort dstport,
	const mDNSInterfaceID InterfaceID)
	{
	mDNSInterfaceID ifid = InterfaceID;
	DNSMessage  *msg  = (DNSMessage *)pkt;
	const mDNSu8 StdQ = kDNSFlag0_QR_Query    | kDNSFlag0_OP_StdQuery;
	const mDNSu8 StdR = kDNSFlag0_QR_Response | kDNSFlag0_OP_StdQuery;
	const mDNSu8 UpdR = kDNSFlag0_QR_Response | kDNSFlag0_OP_Update;
	mDNSu8 QR_OP;
	mDNSu8 *ptr = mDNSNULL;
	mDNSBool TLS = (dstaddr == (mDNSAddr *)1);	// For debug logs: dstaddr = 0 means TCP; dstaddr = 1 means TLS
	if (TLS) dstaddr = mDNSNULL;

#ifndef UNICAST_DISABLED
	if (mDNSSameAddress(srcaddr, &m->Router))
		{
		if (mDNSSameIPPort(srcport, NATPMPPort))
			{
			mDNS_Lock(m);
			uDNS_ReceiveNATPMPPacket(m, InterfaceID, pkt, (mDNSu16)(end - (mDNSu8 *)pkt));
			mDNS_Unlock(m);
			return;
			}
#ifdef _LEGACY_NAT_TRAVERSAL_
		if (mDNSSameIPPort(srcport, SSDPPort))
			{
			mDNS_Lock(m);
			LNT_ConfigureRouterInfo(m, InterfaceID, pkt, (mDNSu16)(end - (mDNSu8 *)pkt));
			mDNS_Unlock(m);
			return;
			}
#endif
		}
#endif
	if ((unsigned)(end - (mDNSu8 *)pkt) < sizeof(DNSMessageHeader)) { LogMsg("DNS Message too short"); return; }
	QR_OP = (mDNSu8)(msg->h.flags.b[0] & kDNSFlag0_QROP_Mask);
	// Read the integer parts which are in IETF byte-order (MSB first, LSB second)
	ptr = (mDNSu8 *)&msg->h.numQuestions;
	msg->h.numQuestions   = (mDNSu16)((mDNSu16)ptr[0] << 8 | ptr[1]);
	msg->h.numAnswers     = (mDNSu16)((mDNSu16)ptr[2] << 8 | ptr[3]);
	msg->h.numAuthorities = (mDNSu16)((mDNSu16)ptr[4] << 8 | ptr[5]);
	msg->h.numAdditionals = (mDNSu16)((mDNSu16)ptr[6] << 8 | ptr[7]);

	if (!m) { LogMsg("mDNSCoreReceive ERROR m is NULL"); return; }
	
	// We use zero addresses and all-ones addresses at various places in the code to indicate special values like "no address"
	// If we accept and try to process a packet with zero or all-ones source address, that could really mess things up
	if (srcaddr && !mDNSAddressIsValid(srcaddr)) { debugf("mDNSCoreReceive ignoring packet from %#a", srcaddr); return; }

	mDNS_Lock(m);
	m->PktNum++;
#ifndef UNICAST_DISABLED
	if (!dstaddr || (!mDNSAddressIsAllDNSLinkGroup(dstaddr) && (QR_OP == StdR || QR_OP == UpdR)))
		{
		if (!mDNSOpaque16IsZero(msg->h.id)) ifid = mDNSInterface_Any;
		uDNS_ReceiveMsg(m, msg, end, srcaddr, srcport);
		if (mDNS_LogLevel >= MDNS_LOG_VERBOSE_DEBUG)
			DumpPacket(m, mDNSfalse, TLS ? "TLS" : !dstaddr ? "TCP" : "UDP", srcaddr, srcport, msg, end);
		// Note: mDNSCore also needs to get access to received unicast responses
		}
#endif
	if      (QR_OP == StdQ) mDNSCoreReceiveQuery   (m, msg, end, srcaddr, srcport, dstaddr, dstport, ifid);
	else if (QR_OP == StdR) mDNSCoreReceiveResponse(m, msg, end, srcaddr, srcport, dstaddr, dstport, ifid);
	else if (QR_OP != UpdR)
		{
		LogMsg("Unknown DNS packet type %02X%02X from %#-15a:%-5d to %#-15a:%-5d on %p (ignored)",
			msg->h.flags.b[0], msg->h.flags.b[1], srcaddr, mDNSVal16(srcport), dstaddr, mDNSVal16(dstport), InterfaceID);
		}
	// Packet reception often causes a change to the task list:
	// 1. Inbound queries can cause us to need to send responses
	// 2. Conflicing response packets received from other hosts can cause us to need to send defensive responses
	// 3. Other hosts announcing deletion of shared records can cause us to need to re-assert those records
	// 4. Response packets that answer questions may cause our client to issue new questions
	mDNS_Unlock(m);
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - Searcher Functions
#endif

// Targets are considered the same if both queries are untargeted, or
// if both are targeted to the same address+port
// (If Target address is zero, TargetPort is undefined)
#define SameQTarget(A,B) (((A)->Target.type == mDNSAddrType_None && (B)->Target.type == mDNSAddrType_None) || \
	(mDNSSameAddress(&(A)->Target, &(B)->Target) && mDNSSameIPPort((A)->TargetPort, (B)->TargetPort)))

mDNSlocal DNSQuestion *FindDuplicateQuestion(const mDNS *const m, const DNSQuestion *const question)
	{
	DNSQuestion *q;
	// Note: A question can only be marked as a duplicate of one that occurs *earlier* in the list.
	// This prevents circular references, where two questions are each marked as a duplicate of the other.
	// Accordingly, we break out of the loop when we get to 'question', because there's no point searching
	// further in the list.
	for (q = m->Questions; q && q != question; q=q->next)		// Scan our list for another question
		if (q->InterfaceID == question->InterfaceID &&			// with the same InterfaceID,
			SameQTarget(q, question)                &&			// and same unicast/multicast target settings
			q->qtype       == question->qtype       &&			// type,
			q->qclass      == question->qclass      &&			// class,
			q->AuthInfo    == question->AuthInfo    &&			// and privacy status matches
			q->LongLived   == question->LongLived   &&			// and long-lived status matches
			q->qnamehash   == question->qnamehash   &&
			SameDomainName(&q->qname, &question->qname))		// and name
			return(q);
	return(mDNSNULL);
	}

// This is called after a question is deleted, in case other identical questions were being
// suppressed as duplicates
mDNSlocal void UpdateQuestionDuplicates(mDNS *const m, DNSQuestion *const question)
	{
	DNSQuestion *q;
	for (q = m->Questions; q; q=q->next)		// Scan our list of questions
		if (q->DuplicateOf == question)			// To see if any questions were referencing this as their duplicate
			if ((q->DuplicateOf = FindDuplicateQuestion(m, q)) == mDNSNULL)
				{
				// If q used to be a duplicate, but now is not,
				// then inherit the state from the question that's going away
				q->LastQTime         = question->LastQTime;
				q->ThisQInterval     = question->ThisQInterval;
				q->ExpectUnicastResp = question->ExpectUnicastResp;
				q->LastAnswerPktNum  = question->LastAnswerPktNum;
				q->RecentAnswerPkts  = question->RecentAnswerPkts;
				q->RequestUnicast    = question->RequestUnicast;
				q->LastQTxTime       = question->LastQTxTime;
				q->CNAMEReferrals    = question->CNAMEReferrals;
				q->nta               = question->nta;
				q->servAddr          = question->servAddr;
				q->servPort          = question->servPort;

				q->state             = question->state;
			//	q->NATInfoUDP        = question->NATInfoUDP;
				q->eventPort         = question->eventPort;
			//	q->tcp               = question->tcp;
				q->origLease         = question->origLease;
				q->expire            = question->expire;
				q->ntries            = question->ntries;
				q->id                = question->id;

				question->nta        = mDNSNULL;	// If we've got a GetZoneData in progress, transfer it to the newly active question
			//	question->NATInfoUDP = mDNSNULL;
			//	question->tcp        = mDNSNULL;
				if (q->nta)
					{
					LogOperation("UpdateQuestionDuplicates transferred nta pointer for %##s (%s)", q->qname.c, DNSTypeName(q->qtype));
					q->nta->ZoneDataContext = q;
					}

				// Need to work out how to safely transfer this state too -- appropriate context pointers need to be updated or the code will crash
				if (question->tcp) LogOperation("UpdateQuestionDuplicates did not transfer tcp pointer");
	
				SetNextQueryTime(m,q);
				}
	}

// lookup a DNS Server, matching by name in split-dns configurations.  Result stored in addr parameter if successful
mDNSlocal DNSServer *GetServerForName(mDNS *m, const domainname *name)
    {
	DNSServer *curmatch = mDNSNULL, *p;
	int curmatchlen = -1, ncount = name ? CountLabels(name) : 0;

	for (p = m->DNSServers; p; p = p->next)
		{
		int scount = CountLabels(&p->domain);
		if (!p->del && ncount >= scount && scount > curmatchlen)
			if (SameDomainName(SkipLeadingLabels(name, ncount - scount), &p->domain))
				{ curmatch = p; curmatchlen = scount; }
		}
	return(curmatch);
	}

#define ValidQuestionTarget(Q) (((Q)->Target.type == mDNSAddrType_IPv4 || (Q)->Target.type == mDNSAddrType_IPv6) && \
	(mDNSSameIPPort((Q)->TargetPort, UnicastDNSPort) || mDNSSameIPPort((Q)->TargetPort, MulticastDNSPort)))

mDNSlocal void ActivateUnicastQuery(mDNS *const m, DNSQuestion *const question)
	{
	if (!question->DuplicateOf)
		{
		if (question->LongLived)
			{
			question->ThisQInterval = 0;	// Question is suspended, waiting for GetZoneData to complete
			question->LastQTime     = m->timenow;
			LogOperation("uDNS_InitLongLivedQuery: %##s %s %s %d",
				question->qname.c, DNSTypeName(question->qtype), question->AuthInfo ? "(Private)" : "", question->ThisQInterval);
			if (question->nta) CancelGetZoneData(m, question->nta);
			question->state = LLQ_GetZoneInfo;		// Necessary to stop "bad state" error in startLLQHandshakeCallback
			question->nta = StartGetZoneData(m, &question->qname, ZoneServiceLLQ, startLLQHandshakeCallback, question);
			if (!question->nta) LogMsg("ERROR: startLLQ - StartGetZoneData failed");
			}
		else
			{
			question->ThisQInterval = InitialQuestionInterval;
			question->LastQTime     = m->timenow - question->ThisQInterval;
			}
		}

	// For now this AutoTunnel stuff is specific to Mac OS X.
	// In the future, if there's demand, we may see if we can abstract it out cleanly into the platform layer
	#if APPLE_OSX_mDNSResponder
		if (question->qtype == kDNSType_AAAA && question->AuthInfo && question->AuthInfo->AutoTunnel && question->QuestionCallback != AutoTunnelCallback)
			{
			question->NoAnswer = NoAnswer_Suspended;
			AddNewClientTunnel(m, question);
			}
	#endif // APPLE_OSX_mDNSResponder
	}

mDNSexport mStatus mDNS_StartQuery_internal(mDNS *const m, DNSQuestion *const question)
	{
	if (question->Target.type && !ValidQuestionTarget(question))
		{
		LogMsg("Warning! Target.type = %ld port = %u (Client forgot to initialize before calling mDNS_StartQuery?)",
			question->Target.type, mDNSVal16(question->TargetPort));
		question->Target.type = mDNSAddrType_None;
		}

	if (!question->Target.type)		// No question->Target specified, so clear TargetPort and TargetQID
		{
		question->TargetPort = zeroIPPort;
		question->TargetQID  = zeroID;
		}

#ifndef UNICAST_DISABLED
	// If the client has specified 'kDNSServiceFlagsForceMulticast'
	// then we do a multicast query on that interface, even for unicast domains.
	if (question->InterfaceID == mDNSInterface_LocalOnly || question->ForceMCast || IsLocalDomain(&question->qname))
		question->TargetQID = zeroID;
	else
		question->TargetQID = mDNS_NewMessageID(m);
#else
    question->TargetQID = zeroID;
#endif // UNICAST_DISABLED

	debugf("mDNS_StartQuery: %##s (%s)", question->qname.c, DNSTypeName(question->qtype));

	if (m->rrcache_size == 0)	// Can't do queries if we have no cache space allocated
		return(mStatus_NoCache);
	else
		{
		int i;
		DNSQuestion **q;
		
		if (!ValidateDomainName(&question->qname))
			{
			LogMsg("Attempt to start query with invalid qname %##s (%s)", question->qname.c, DNSTypeName(question->qtype));
			return(mStatus_Invalid);
			}

		// Note: It important that new questions are appended at the *end* of the list, not prepended at the start
		q = &m->Questions;
		if (question->InterfaceID == mDNSInterface_LocalOnly) q = &m->LocalOnlyQuestions;
		while (*q && *q != question) q=&(*q)->next;

		if (*q)
			{
			LogMsg("Error! Tried to add a question %##s (%s) that's already in the active list",
				question->qname.c, DNSTypeName(question->qtype));
			return(mStatus_AlreadyRegistered);
			}

		*q = question;

		// If this question is referencing a specific interface, verify it exists
		if (question->InterfaceID && question->InterfaceID != mDNSInterface_LocalOnly)
			{
			NetworkInterfaceInfo *intf;
			for (intf = m->HostInterfaces; intf; intf = intf->next)
				if (intf->InterfaceID == question->InterfaceID) break;
			if (!intf)
				LogMsg("Note: InterfaceID %p for question %##s (%s) not currently found in active interface list",
					question->InterfaceID, question->qname.c, DNSTypeName(question->qtype));
			}

		// Note: In the case where we already have the answer to this question in our cache, that may be all the client
		// wanted, and they may immediately cancel their question. In this case, sending an actual query on the wire would
		// be a waste. For that reason, we schedule our first query to go out in half a second (InitialQuestionInterval).
		// If AnswerNewQuestion() finds that we have *no* relevant answers currently in our cache, then it will accelerate
		// that to go out immediately.
		question->next              = mDNSNULL;
		question->qnamehash         = DomainNameHashValue(&question->qname);	// MUST do this before FindDuplicateQuestion()
		question->DelayAnswering    = CheckForSoonToExpireRecords(m, &question->qname, question->qnamehash, HashSlot(&question->qname));
		question->LastQTime         = m->timenow;
		question->ThisQInterval     = InitialQuestionInterval;					// MUST be > zero for an active question
		question->ExpectUnicastResp = 0;
		question->LastAnswerPktNum  = m->PktNum;
		question->RecentAnswerPkts  = 0;
		question->CurrentAnswers    = 0;
		question->LargeAnswers      = 0;
		question->UniqueAnswers     = 0;
		question->FlappingInterface1 = mDNSNULL;
		question->FlappingInterface2 = mDNSNULL;
		// GetZoneData queries are a special case -- even if we have a key for them, we don't do them privately,
		// because that would result in an infinite loop (i.e. to do a private query we first need to get
		// the _dns-query-tls SRV record for the zone, and we can't do *that* privately because to do so
		// we'd need to already know the _dns-query-tls SRV record.
		// Also: Make sure we set AuthInfo before calling FindDuplicateQuestion()
		question->AuthInfo          = (question->QuestionCallback == GetZoneData_QuestionCallback) ? mDNSNULL
		                            : GetAuthInfoForName_internal(m, &question->qname);
		question->DuplicateOf       = FindDuplicateQuestion(m, question);
		question->NextInDQList      = mDNSNULL;
		question->SendQNow          = mDNSNULL;
		question->SendOnAll         = mDNSfalse;
		question->RequestUnicast    = 0;
		question->LastQTxTime       = m->timenow;
		question->CNAMEReferrals    = 0;

		question->qDNSServer        = mDNSNULL;
		question->nta               = mDNSNULL;
		question->servAddr          = zeroAddr;
		question->servPort          = zeroIPPort;
		question->tcp               = mDNSNULL;
		question->NoAnswer          = 0;

		question->state             = LLQ_GetZoneInfo;
		mDNSPlatformMemZero(&question->NATInfoUDP, sizeof(question->NATInfoUDP));
		question->eventPort         = zeroIPPort;
		question->origLease         = 0;
		question->expire            = 0;
		question->ntries            = 0;
		question->id                = zeroOpaque64;

		for (i=0; i<DupSuppressInfoSize; i++)
			question->DupSuppress[i].InterfaceID = mDNSNULL;

		if (!question->DuplicateOf)
			debugf("mDNS_StartQuery: Question %##s (%s) %p %d (%p) started",
				question->qname.c, DNSTypeName(question->qtype), question->InterfaceID,
				question->LastQTime + question->ThisQInterval - m->timenow, question);
		else
			debugf("mDNS_StartQuery: Question %##s (%s) %p %d (%p) duplicate of (%p)",
				question->qname.c, DNSTypeName(question->qtype), question->InterfaceID,
				question->LastQTime + question->ThisQInterval - m->timenow, question, question->DuplicateOf);

		if (question->InterfaceID == mDNSInterface_LocalOnly)
			{
			if (!m->NewLocalOnlyQuestions) m->NewLocalOnlyQuestions = question;
			}
		else
			{
			if (!m->NewQuestions) m->NewQuestions = question;

			// If the question's id is non-zero, then it's Wide Area
			// MUST NOT do this Wide Area setup until near the end of
			// mDNS_StartQuery_internal -- this code may itself issue queries (e.g. SOA,
			// NS, etc.) and if we haven't finished setting up our own question and setting
			// m->NewQuestions if necessary then we could end up recursively re-entering
			// this routine with the question list data structures in an inconsistent state.
			if (!mDNSOpaque16IsZero(question->TargetQID))
				{
				question->qDNSServer = GetServerForName(m, &question->qname);
				ActivateUnicastQuery(m, question);
				}
			SetNextQueryTime(m,question);
			}

		return(mStatus_NoError);
		}
	}

// CancelGetZoneData is an internal routine (i.e. must be called with the lock already held)
mDNSexport void CancelGetZoneData(mDNS *const m, ZoneData *nta)
	{
	LogOperation("CancelGetZoneData %##s (%s)", nta->question.qname.c, DNSTypeName(nta->question.qtype));
	mDNS_StopQuery_internal(m, &nta->question);
	mDNSPlatformMemFree(nta);
	}

mDNSexport mStatus mDNS_StopQuery_internal(mDNS *const m, DNSQuestion *const question)
	{
	const mDNSu32 slot = HashSlot(&question->qname);
	CacheGroup *cg = CacheGroupForName(m, slot, question->qnamehash, &question->qname);
	CacheRecord *rr;
	DNSQuestion **q = &m->Questions;
	
	//LogOperation("mDNS_StopQuery_internal %##s (%s)", question->qname.c, DNSTypeName(question->qtype));

	if (question->InterfaceID == mDNSInterface_LocalOnly) q = &m->LocalOnlyQuestions;
	while (*q && *q != question) q=&(*q)->next;
	if (*q) *q = (*q)->next;
	else
		{
		if (question->ThisQInterval >= 0)	// Only log error message if the query was supposed to be active
			LogMsg("mDNS_StopQuery_internal: Question %##s (%s) not found in active list",
				question->qname.c, DNSTypeName(question->qtype));
#if ForceAlerts
		*(long*)0 = 0;
#endif
		return(mStatus_BadReferenceErr);
		}

	// Take care to cut question from list *before* calling UpdateQuestionDuplicates
	UpdateQuestionDuplicates(m, question);
	// But don't trash ThisQInterval until afterwards.
	question->ThisQInterval = -1;

	// If there are any cache records referencing this as their active question, then see if there is any
	// other question that is also referencing them, else their CRActiveQuestion needs to get set to NULL.
	for (rr = cg ? cg->members : mDNSNULL; rr; rr=rr->next)
		{
		if (rr->CRActiveQuestion == question)
			{
			DNSQuestion *q;
			for (q = m->Questions; q; q=q->next)		// Scan our list of questions
				if (ActiveQuestion(q) && ResourceRecordAnswersQuestion(&rr->resrec, q))
					break;
			debugf("mDNS_StopQuery_internal: Updating CRActiveQuestion to %p for cache record %s", q, CRDisplayString(m,rr));
			rr->CRActiveQuestion = q;		// Question used to be active; new value may or may not be null
			if (!q) m->rrcache_active--;	// If no longer active, decrement rrcache_active count
			}
		}

	// If we just deleted the question that CacheRecordAdd() or CacheRecordRmv()is about to look at,
	// bump its pointer forward one question.
	if (m->CurrentQuestion == question)
		{
		debugf("mDNS_StopQuery_internal: Just deleted the currently active question: %##s (%s)",
			question->qname.c, DNSTypeName(question->qtype));
		m->CurrentQuestion = question->next;
		}

	if (m->NewQuestions == question)
		{
		debugf("mDNS_StopQuery_internal: Just deleted a new question that wasn't even answered yet: %##s (%s)",
			question->qname.c, DNSTypeName(question->qtype));
		m->NewQuestions = question->next;
		}

	if (m->NewLocalOnlyQuestions == question) m->NewLocalOnlyQuestions = question->next;

	// Take care not to trash question->next until *after* we've updated m->CurrentQuestion and m->NewQuestions
	question->next = mDNSNULL;

	// LogMsg("mDNS_StopQuery_internal: Question %##s (%s) removed", question->qname.c, DNSTypeName(question->qtype));

	// And finally, cancel any associated GetZoneData operation that's still running.
	// Must not do this until last, because there's a good chance the GetZoneData question is the next in the list,
	// so if we delete it earlier in this routine, we could find that our "question->next" pointer above is already
	// invalid before we even use it. By making sure that we update m->CurrentQuestion and m->NewQuestions if necessary
	// *first*, then they're all ready to be updated a second time if necessary when we cancel our GetZoneData query.
	if (question->nta) CancelGetZoneData(m, question->nta);
	if (question->tcp) { DisposeTCPConn(question->tcp); question->tcp = mDNSNULL; }
	if (!mDNSOpaque16IsZero(question->TargetQID) && question->LongLived) uDNS_StopLongLivedQuery(m, question);

	return(mStatus_NoError);
	}

mDNSexport mStatus mDNS_StartQuery(mDNS *const m, DNSQuestion *const question)
	{
	mStatus status;
	mDNS_Lock(m);
	status = mDNS_StartQuery_internal(m, question);
	mDNS_Unlock(m);
	return(status);
	}

mDNSexport mStatus mDNS_StopQuery(mDNS *const m, DNSQuestion *const question)
	{
	mStatus status;
	mDNS_Lock(m);
	status = mDNS_StopQuery_internal(m, question);
	mDNS_Unlock(m);
	return(status);
	}

// Note that mDNS_StopQueryWithRemoves() does not currently implement the full generality of the other APIs
// Specifically, question callbacks invoked as a result of this call cannot themselves make API calls.
// We invoke the callback without using mDNS_DropLockBeforeCallback/mDNS_ReclaimLockAfterCallback
// specifically to catch and report if the client callback does try to make API calls
mDNSexport mStatus mDNS_StopQueryWithRemoves(mDNS *const m, DNSQuestion *const question)
	{
	mStatus status;
	DNSQuestion *qq;
	mDNS_Lock(m);

	// Check if question is new -- don't want to give remove events for a question we haven't even answered yet
	for (qq = m->NewQuestions; qq; qq=qq->next) if (qq == question) break;

	status = mDNS_StopQuery_internal(m, question);
	if (status == mStatus_NoError && !qq)
		{
		CacheRecord *rr;
		const mDNSu32 slot = HashSlot(&question->qname);
		CacheGroup *const cg = CacheGroupForName(m, slot, question->qnamehash, &question->qname);
		LogOperation("Generating terminal removes for %##s (%s)", question->qname.c, DNSTypeName(question->qtype));
		for (rr = cg ? cg->members : mDNSNULL; rr; rr=rr->next)
			if (rr->resrec.RecordType != kDNSRecordTypePacketNegative && SameNameRecordAnswersQuestion(&rr->resrec, question))
				{
				// Don't use mDNS_DropLockBeforeCallback() here, since we don't allow API calls
				if (question->QuestionCallback)
					question->QuestionCallback(m, question, &rr->resrec, mDNSfalse);
				}
		}
	mDNS_Unlock(m);
	return(status);
	}

mDNSexport mStatus mDNS_Reconfirm(mDNS *const m, CacheRecord *const cr)
	{
	mStatus status;
	mDNS_Lock(m);
	status = mDNS_Reconfirm_internal(m, cr, kDefaultReconfirmTimeForNoAnswer);
	if (status == mStatus_NoError) ReconfirmAntecedents(m, cr->resrec.name, cr->resrec.namehash, 0);
	mDNS_Unlock(m);
	return(status);
	}

mDNSexport mStatus mDNS_ReconfirmByValue(mDNS *const m, ResourceRecord *const rr)
	{
	mStatus status = mStatus_BadReferenceErr;
	CacheRecord *cr;
	mDNS_Lock(m);
	cr = FindIdenticalRecordInCache(m, rr);
	debugf("mDNS_ReconfirmByValue: %p %s", cr, RRDisplayString(m, rr));
	if (cr) status = mDNS_Reconfirm_internal(m, cr, kDefaultReconfirmTimeForNoAnswer);
	if (status == mStatus_NoError) ReconfirmAntecedents(m, cr->resrec.name, cr->resrec.namehash, 0);
	mDNS_Unlock(m);
	return(status);
	}

mDNSexport mStatus mDNS_StartBrowse(mDNS *const m, DNSQuestion *const question,
	const domainname *const srv, const domainname *const domain,
	const mDNSInterfaceID InterfaceID, mDNSBool ForceMCast, mDNSQuestionCallback *Callback, void *Context)
	{
	question->InterfaceID      = InterfaceID;
	question->Target           = zeroAddr;
	question->qtype            = kDNSType_PTR;
	question->qclass           = kDNSClass_IN;
	question->LongLived        = mDNSfalse;
	question->ExpectUnique     = mDNSfalse;
	question->ForceMCast       = ForceMCast;
	question->ReturnIntermed   = mDNSfalse;
	question->QuestionCallback = Callback;
	question->QuestionContext  = Context;
	if (!ConstructServiceName(&question->qname, mDNSNULL, srv, domain)) return(mStatus_BadParamErr);

#ifndef UNICAST_DISABLED
    if (question->InterfaceID != mDNSInterface_LocalOnly && !question->ForceMCast && !IsLocalDomain(&question->qname))
		{
		question->LongLived     = mDNStrue;
		question->ThisQInterval = InitialQuestionInterval;
		question->LastQTime     = m->timenow - question->ThisQInterval;
		}
#endif // UNICAST_DISABLED
	return(mDNS_StartQuery(m, question));
	}

mDNSlocal mDNSBool MachineHasActiveIPv6(mDNS *const m)
	{
	NetworkInterfaceInfo *intf;
	for (intf = m->HostInterfaces; intf; intf = intf->next)
	if (intf->ip.type == mDNSAddrType_IPv6) return(mDNStrue);
	return(mDNSfalse);
	}

mDNSlocal void FoundServiceInfoSRV(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, QC_result AddRecord)
	{
	ServiceInfoQuery *query = (ServiceInfoQuery *)question->QuestionContext;
	mDNSBool PortChanged = !mDNSSameIPPort(query->info->port, answer->rdata->u.srv.port);
	if (!AddRecord) return;
	if (answer->rrtype != kDNSType_SRV) return;

	query->info->port = answer->rdata->u.srv.port;

	// If this is our first answer, then set the GotSRV flag and start the address query
	if (!query->GotSRV)
		{
		query->GotSRV             = mDNStrue;
		query->qAv4.InterfaceID   = answer->InterfaceID;
		AssignDomainName(&query->qAv4.qname, &answer->rdata->u.srv.target);
		query->qAv6.InterfaceID   = answer->InterfaceID;
		AssignDomainName(&query->qAv6.qname, &answer->rdata->u.srv.target);
		mDNS_StartQuery(m, &query->qAv4);
		// Only do the AAAA query if this machine actually has IPv6 active
		if (MachineHasActiveIPv6(m)) mDNS_StartQuery(m, &query->qAv6);
		}
	// If this is not our first answer, only re-issue the address query if the target host name has changed
	else if ((query->qAv4.InterfaceID != query->qSRV.InterfaceID && query->qAv4.InterfaceID != answer->InterfaceID) ||
		!SameDomainName(&query->qAv4.qname, &answer->rdata->u.srv.target))
		{
		mDNS_StopQuery(m, &query->qAv4);
		if (query->qAv6.ThisQInterval >= 0) mDNS_StopQuery(m, &query->qAv6);
		if (SameDomainName(&query->qAv4.qname, &answer->rdata->u.srv.target) && !PortChanged)
			{
			// If we get here, it means:
			// 1. This is not our first SRV answer
			// 2. The interface ID is different, but the target host and port are the same
			// This implies that we're seeing the exact same SRV record on more than one interface, so we should
			// make our address queries at least as broad as the original SRV query so that we catch all the answers.
			query->qAv4.InterfaceID = query->qSRV.InterfaceID;	// Will be mDNSInterface_Any, or a specific interface
			query->qAv6.InterfaceID = query->qSRV.InterfaceID;
			}
		else
			{
			query->qAv4.InterfaceID   = answer->InterfaceID;
			AssignDomainName(&query->qAv4.qname, &answer->rdata->u.srv.target);
			query->qAv6.InterfaceID   = answer->InterfaceID;
			AssignDomainName(&query->qAv6.qname, &answer->rdata->u.srv.target);
			}
		debugf("FoundServiceInfoSRV: Restarting address queries for %##s (%s)", query->qAv4.qname.c, DNSTypeName(query->qAv4.qtype));
		mDNS_StartQuery(m, &query->qAv4);
		// Only do the AAAA query if this machine actually has IPv6 active
		if (MachineHasActiveIPv6(m)) mDNS_StartQuery(m, &query->qAv6);
		}
	else if (query->ServiceInfoQueryCallback && query->GotADD && query->GotTXT && PortChanged)
		{
		if (++query->Answers >= 100)
			debugf("**** WARNING **** Have given %lu answers for %##s (SRV) %##s %u",
				query->Answers, query->qSRV.qname.c, answer->rdata->u.srv.target.c,
				mDNSVal16(answer->rdata->u.srv.port));
		query->ServiceInfoQueryCallback(m, query);
		}
	// CAUTION: MUST NOT do anything more with query after calling query->Callback(), because the client's
	// callback function is allowed to do anything, including deleting this query and freeing its memory.
	}

mDNSlocal void FoundServiceInfoTXT(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, QC_result AddRecord)
	{
	ServiceInfoQuery *query = (ServiceInfoQuery *)question->QuestionContext;
	if (!AddRecord) return;
	if (answer->rrtype != kDNSType_TXT) return;
	if (answer->rdlength > sizeof(query->info->TXTinfo)) return;

	query->GotTXT       = mDNStrue;
	query->info->TXTlen = answer->rdlength;
	query->info->TXTinfo[0] = 0;		// In case answer->rdlength is zero
	mDNSPlatformMemCopy(query->info->TXTinfo, answer->rdata->u.txt.c, answer->rdlength);

	verbosedebugf("FoundServiceInfoTXT: %##s GotADD=%d", query->info->name.c, query->GotADD);

	// CAUTION: MUST NOT do anything more with query after calling query->Callback(), because the client's
	// callback function is allowed to do anything, including deleting this query and freeing its memory.
	if (query->ServiceInfoQueryCallback && query->GotADD)
		{
		if (++query->Answers >= 100)
			debugf("**** WARNING **** have given %lu answers for %##s (TXT) %#s...",
				query->Answers, query->qSRV.qname.c, answer->rdata->u.txt.c);
		query->ServiceInfoQueryCallback(m, query);
		}
	}

mDNSlocal void FoundServiceInfo(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, QC_result AddRecord)
	{
	ServiceInfoQuery *query = (ServiceInfoQuery *)question->QuestionContext;
	//LogOperation("FoundServiceInfo %d %s", AddRecord, RRDisplayString(m, answer));
	if (!AddRecord) return;
	
	if (answer->rrtype == kDNSType_A)
		{
		query->info->ip.type = mDNSAddrType_IPv4;
		query->info->ip.ip.v4 = answer->rdata->u.ipv4;
		}
	else if (answer->rrtype == kDNSType_AAAA)
		{
		query->info->ip.type = mDNSAddrType_IPv6;
		query->info->ip.ip.v6 = answer->rdata->u.ipv6;
		}
	else
		{
		debugf("FoundServiceInfo: answer %##s type %d (%s) unexpected", answer->name->c, answer->rrtype, DNSTypeName(answer->rrtype));
		return;
		}

	query->GotADD = mDNStrue;
	query->info->InterfaceID = answer->InterfaceID;

	verbosedebugf("FoundServiceInfo v%ld: %##s GotTXT=%d", query->info->ip.type, query->info->name.c, query->GotTXT);

	// CAUTION: MUST NOT do anything more with query after calling query->Callback(), because the client's
	// callback function is allowed to do anything, including deleting this query and freeing its memory.
	if (query->ServiceInfoQueryCallback && query->GotTXT)
		{
		if (++query->Answers >= 100)
			debugf(answer->rrtype == kDNSType_A ?
				"**** WARNING **** have given %lu answers for %##s (A) %.4a" :
				"**** WARNING **** have given %lu answers for %##s (AAAA) %.16a",
				query->Answers, query->qSRV.qname.c, &answer->rdata->u.data);
		query->ServiceInfoQueryCallback(m, query);
		}
	}

// On entry, the client must have set the name and InterfaceID fields of the ServiceInfo structure
// If the query is not interface-specific, then InterfaceID may be zero
// Each time the Callback is invoked, the remainder of the fields will have been filled in
// In addition, InterfaceID will be updated to give the interface identifier corresponding to that response
mDNSexport mStatus mDNS_StartResolveService(mDNS *const m,
	ServiceInfoQuery *query, ServiceInfo *info, mDNSServiceInfoQueryCallback *Callback, void *Context)
	{
	mStatus status;
	mDNS_Lock(m);

	query->qSRV.ThisQInterval       = -1;		// So that mDNS_StopResolveService() knows whether to cancel this question
	query->qSRV.InterfaceID         = info->InterfaceID;
	query->qSRV.Target              = zeroAddr;
	AssignDomainName(&query->qSRV.qname, &info->name);
	query->qSRV.qtype               = kDNSType_SRV;
	query->qSRV.qclass              = kDNSClass_IN;
	query->qSRV.LongLived           = mDNSfalse;
	query->qSRV.ExpectUnique        = mDNStrue;
	query->qSRV.ForceMCast          = mDNSfalse;
	query->qSRV.ReturnIntermed      = mDNSfalse;
	query->qSRV.QuestionCallback    = FoundServiceInfoSRV;
	query->qSRV.QuestionContext     = query;

	query->qTXT.ThisQInterval       = -1;		// So that mDNS_StopResolveService() knows whether to cancel this question
	query->qTXT.InterfaceID         = info->InterfaceID;
	query->qTXT.Target              = zeroAddr;
	AssignDomainName(&query->qTXT.qname, &info->name);
	query->qTXT.qtype               = kDNSType_TXT;
	query->qTXT.qclass              = kDNSClass_IN;
	query->qTXT.LongLived           = mDNSfalse;
	query->qTXT.ExpectUnique        = mDNStrue;
	query->qTXT.ForceMCast          = mDNSfalse;
	query->qTXT.ReturnIntermed      = mDNSfalse;
	query->qTXT.QuestionCallback    = FoundServiceInfoTXT;
	query->qTXT.QuestionContext     = query;

	query->qAv4.ThisQInterval       = -1;		// So that mDNS_StopResolveService() knows whether to cancel this question
	query->qAv4.InterfaceID         = info->InterfaceID;
	query->qAv4.Target              = zeroAddr;
	query->qAv4.qname.c[0]          = 0;
	query->qAv4.qtype               = kDNSType_A;
	query->qAv4.qclass              = kDNSClass_IN;
	query->qAv4.LongLived           = mDNSfalse;
	query->qAv4.ExpectUnique        = mDNStrue;
	query->qAv4.ForceMCast          = mDNSfalse;
	query->qAv4.ReturnIntermed      = mDNSfalse;
	query->qAv4.QuestionCallback    = FoundServiceInfo;
	query->qAv4.QuestionContext     = query;

	query->qAv6.ThisQInterval       = -1;		// So that mDNS_StopResolveService() knows whether to cancel this question
	query->qAv6.InterfaceID         = info->InterfaceID;
	query->qAv6.Target              = zeroAddr;
	query->qAv6.qname.c[0]          = 0;
	query->qAv6.qtype               = kDNSType_AAAA;
	query->qAv6.qclass              = kDNSClass_IN;
	query->qAv6.LongLived           = mDNSfalse;
	query->qAv6.ExpectUnique        = mDNStrue;
	query->qAv6.ForceMCast          = mDNSfalse;
	query->qAv6.ReturnIntermed      = mDNSfalse;
	query->qAv6.QuestionCallback    = FoundServiceInfo;
	query->qAv6.QuestionContext     = query;

	query->GotSRV                   = mDNSfalse;
	query->GotTXT                   = mDNSfalse;
	query->GotADD                   = mDNSfalse;
	query->Answers                  = 0;

	query->info                     = info;
	query->ServiceInfoQueryCallback = Callback;
	query->ServiceInfoQueryContext  = Context;

//	info->name      = Must already be set up by client
//	info->interface = Must already be set up by client
	info->ip        = zeroAddr;
	info->port      = zeroIPPort;
	info->TXTlen    = 0;

	// We use mDNS_StartQuery_internal here because we're already holding the lock
	status = mDNS_StartQuery_internal(m, &query->qSRV);
	if (status == mStatus_NoError) status = mDNS_StartQuery_internal(m, &query->qTXT);
	if (status != mStatus_NoError) mDNS_StopResolveService(m, query);

	mDNS_Unlock(m);
	return(status);
	}

mDNSexport void    mDNS_StopResolveService (mDNS *const m, ServiceInfoQuery *q)
	{
	mDNS_Lock(m);
	// We use mDNS_StopQuery_internal here because we're already holding the lock
	if (q->qSRV.ThisQInterval >= 0) mDNS_StopQuery_internal(m, &q->qSRV);
	if (q->qTXT.ThisQInterval >= 0) mDNS_StopQuery_internal(m, &q->qTXT);
	if (q->qAv4.ThisQInterval >= 0) mDNS_StopQuery_internal(m, &q->qAv4);
	if (q->qAv6.ThisQInterval >= 0) mDNS_StopQuery_internal(m, &q->qAv6);
	mDNS_Unlock(m);
	}

mDNSexport mStatus mDNS_GetDomains(mDNS *const m, DNSQuestion *const question, mDNS_DomainType DomainType, const domainname *dom,
	const mDNSInterfaceID InterfaceID, mDNSQuestionCallback *Callback, void *Context)
	{
	question->InterfaceID      = InterfaceID;
	question->Target           = zeroAddr;
	question->qtype            = kDNSType_PTR;
	question->qclass           = kDNSClass_IN;
	question->LongLived        = mDNSfalse;
	question->ExpectUnique     = mDNSfalse;
	question->ForceMCast       = mDNSfalse;
	question->ReturnIntermed   = mDNSfalse;
	question->QuestionCallback = Callback;
	question->QuestionContext  = Context;
	if (DomainType > mDNS_DomainTypeMax) return(mStatus_BadParamErr);
	if (!MakeDomainNameFromDNSNameString(&question->qname, mDNS_DomainTypeNames[DomainType])) return(mStatus_BadParamErr);
	if (!dom) dom = &localdomain;
	if (!AppendDomainName(&question->qname, dom)) return(mStatus_BadParamErr);
	return(mDNS_StartQuery(m, question));
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - Responder Functions
#endif

mDNSexport mStatus mDNS_Register(mDNS *const m, AuthRecord *const rr)
	{
	mStatus status;
	mDNS_Lock(m);
	status = mDNS_Register_internal(m, rr);
	mDNS_Unlock(m);
	return(status);
	}

mDNSexport mStatus mDNS_Update(mDNS *const m, AuthRecord *const rr, mDNSu32 newttl,
	const mDNSu16 newrdlength, RData *const newrdata, mDNSRecordUpdateCallback *Callback)
	{
#ifndef UNICAST_DISABLED
	mDNSBool unicast = !(rr->resrec.InterfaceID == mDNSInterface_LocalOnly || IsLocalDomain(rr->resrec.name));
#else
	mDNSBool unicast = mDNSfalse;
#endif

	if (!ValidateRData(rr->resrec.rrtype, newrdlength, newrdata))
		{
		LogMsg("Attempt to update record with invalid rdata: %s", GetRRDisplayString_rdb(&rr->resrec, &newrdata->u, m->MsgBuffer));
		return(mStatus_Invalid);
		}
	
	mDNS_Lock(m);

	// If TTL is unspecified, leave TTL unchanged
	if (newttl == 0) newttl = rr->resrec.rroriginalttl;

	// If we already have an update queued up which has not gone through yet,
	// give the client a chance to free that memory
	if (!unicast && rr->NewRData)
		{
		RData *n = rr->NewRData;
		rr->NewRData = mDNSNULL;			// Clear the NewRData pointer ...
		if (rr->UpdateCallback)
			rr->UpdateCallback(m, rr, n);	// ...and let the client free this memory, if necessary
		}

	rr->NewRData             = newrdata;
	rr->newrdlength          = newrdlength;
	rr->UpdateCallback       = Callback;

	if (unicast) { mStatus status = uDNS_UpdateRecord(m, rr); mDNS_Unlock(m); return(status); }

	if (rr->resrec.rroriginalttl == newttl &&
		rr->resrec.rdlength == newrdlength && mDNSPlatformMemSame(rr->resrec.rdata->u.data, newrdata->u.data, newrdlength))
		CompleteRDataUpdate(m, rr);
	else
		{
		domainlabel name;
		domainname type, domain;
		DeconstructServiceName(rr->resrec.name, &name, &type, &domain);
		rr->AnnounceCount = InitialAnnounceCount;
		// iChat often does suprious record updates where no data has changed. For the _presence service type, using
		// name/value pairs, the mDNSPlatformMemSame() check above catches this and correctly suppresses the wasteful
		// update. For the _ichat service type, the XML encoding introduces spurious noise differences into the data
		// even though there's no actual semantic change, so the mDNSPlatformMemSame() check doesn't help us.
		// To work around this, we simply unilaterally limit all legacy _ichat-type updates to a single announcement.
		if (SameDomainLabel(type.c, (mDNSu8*)"\x6_ichat")) rr->AnnounceCount = 1;
		rr->ThisAPInterval       = DefaultAPIntervalForRecordType(rr->resrec.RecordType);
		InitializeLastAPTime(m, rr);
		while (rr->NextUpdateCredit && m->timenow - rr->NextUpdateCredit >= 0) GrantUpdateCredit(rr);
		if (!rr->UpdateBlocked && rr->UpdateCredits) rr->UpdateCredits--;
		if (!rr->NextUpdateCredit) rr->NextUpdateCredit = NonZeroTime(m->timenow + kUpdateCreditRefreshInterval);
		if (rr->AnnounceCount > rr->UpdateCredits + 1) rr->AnnounceCount = (mDNSu8)(rr->UpdateCredits + 1);
		if (rr->UpdateCredits <= 5)
			{
			mDNSu32 delay = 6 - rr->UpdateCredits;		// Delay 1 second, then 2, then 3, etc. up to 6 seconds maximum
			if (!rr->UpdateBlocked) rr->UpdateBlocked = NonZeroTime(m->timenow + (mDNSs32)delay * mDNSPlatformOneSecond);
			rr->ThisAPInterval *= 4;
			rr->LastAPTime = rr->UpdateBlocked - rr->ThisAPInterval;
			LogMsg("Excessive update rate for %##s; delaying announcement by %ld second%s",
				rr->resrec.name->c, delay, delay > 1 ? "s" : "");
			}
		rr->resrec.rroriginalttl = newttl;
		}

	mDNS_Unlock(m);
	return(mStatus_NoError);
	}

// NOTE: mDNS_Deregister calls mDNS_Deregister_internal which can call a user callback, which may change
// the record list and/or question list.
// Any code walking either list must use the CurrentQuestion and/or CurrentRecord mechanism to protect against this.
mDNSexport mStatus mDNS_Deregister(mDNS *const m, AuthRecord *const rr)
	{
	mStatus status;
	mDNS_Lock(m);
	status = mDNS_Deregister_internal(m, rr, mDNS_Dereg_normal);
	mDNS_Unlock(m);
	return(status);
	}

// Circular reference: AdvertiseInterface references mDNS_HostNameCallback, which calls mDNS_SetFQDN, which call AdvertiseInterface
mDNSlocal void mDNS_HostNameCallback(mDNS *const m, AuthRecord *const rr, mStatus result);

mDNSlocal NetworkInterfaceInfo *FindFirstAdvertisedInterface(mDNS *const m)
	{
	NetworkInterfaceInfo *intf;
	for (intf = m->HostInterfaces; intf; intf = intf->next)
		if (intf->Advertise) break;
	return(intf);
	}

mDNSlocal void AdvertiseInterface(mDNS *const m, NetworkInterfaceInfo *set)
	{
	char buffer[MAX_REVERSE_MAPPING_NAME];
	NetworkInterfaceInfo *primary = FindFirstAdvertisedInterface(m);
	if (!primary) primary = set; // If no existing advertised interface, this new NetworkInterfaceInfo becomes our new primary

	// Send dynamic update for non-linklocal IPv4 Addresses
	mDNS_SetupResourceRecord(&set->RR_A,     mDNSNULL, set->InterfaceID, kDNSType_A,     kHostNameTTL, kDNSRecordTypeUnique,      mDNS_HostNameCallback, set);
	mDNS_SetupResourceRecord(&set->RR_PTR,   mDNSNULL, set->InterfaceID, kDNSType_PTR,   kHostNameTTL, kDNSRecordTypeKnownUnique, mDNSNULL, mDNSNULL);
	mDNS_SetupResourceRecord(&set->RR_HINFO, mDNSNULL, set->InterfaceID, kDNSType_HINFO, kHostNameTTL, kDNSRecordTypeUnique,      mDNSNULL, mDNSNULL);

#if ANSWER_REMOTE_HOSTNAME_QUERIES
	set->RR_A    .AllowRemoteQuery  = mDNStrue;
	set->RR_PTR  .AllowRemoteQuery  = mDNStrue;
	set->RR_HINFO.AllowRemoteQuery  = mDNStrue;
#endif
	// 1. Set up Address record to map from host name ("foo.local.") to IP address
	// 2. Set up reverse-lookup PTR record to map from our address back to our host name
	AssignDomainName(&set->RR_A.namestorage, &m->MulticastHostname);
	if (set->ip.type == mDNSAddrType_IPv4)
		{
		set->RR_A.resrec.rrtype = kDNSType_A;
		set->RR_A.resrec.rdata->u.ipv4 = set->ip.ip.v4;
		// Note: This is reverse order compared to a normal dotted-decimal IP address, so we can't use our customary "%.4a" format code
		mDNS_snprintf(buffer, sizeof(buffer), "%d.%d.%d.%d.in-addr.arpa.",
			set->ip.ip.v4.b[3], set->ip.ip.v4.b[2], set->ip.ip.v4.b[1], set->ip.ip.v4.b[0]);
		}
	else if (set->ip.type == mDNSAddrType_IPv6)
		{
		int i;
		set->RR_A.resrec.rrtype = kDNSType_AAAA;
		set->RR_A.resrec.rdata->u.ipv6 = set->ip.ip.v6;
		for (i = 0; i < 16; i++)
			{
			static const char hexValues[] = "0123456789ABCDEF";
			buffer[i * 4    ] = hexValues[set->ip.ip.v6.b[15 - i] & 0x0F];
			buffer[i * 4 + 1] = '.';
			buffer[i * 4 + 2] = hexValues[set->ip.ip.v6.b[15 - i] >> 4];
			buffer[i * 4 + 3] = '.';
			}
		mDNS_snprintf(&buffer[64], sizeof(buffer)-64, "ip6.arpa.");
		}

	MakeDomainNameFromDNSNameString(&set->RR_PTR.namestorage, buffer);
	set->RR_PTR.AutoTarget = Target_AutoHost;	// Tell mDNS that the target of this PTR is to be kept in sync with our host name
	set->RR_PTR.ForceMCast = mDNStrue;			// This PTR points to our dot-local name, so don't ever try to write it into a uDNS server

	set->RR_A.RRSet = &primary->RR_A;			// May refer to self

	mDNS_Register_internal(m, &set->RR_A);
	mDNS_Register_internal(m, &set->RR_PTR);

	if (!NO_HINFO && m->HIHardware.c[0] > 0 && m->HISoftware.c[0] > 0 && m->HIHardware.c[0] + m->HISoftware.c[0] <= 254)
		{
		mDNSu8 *p = set->RR_HINFO.resrec.rdata->u.data;
		AssignDomainName(&set->RR_HINFO.namestorage, &m->MulticastHostname);
		set->RR_HINFO.DependentOn = &set->RR_A;
		mDNSPlatformMemCopy(p, &m->HIHardware, 1 + (mDNSu32)m->HIHardware.c[0]);
		p += 1 + (int)p[0];
		mDNSPlatformMemCopy(p, &m->HISoftware, 1 + (mDNSu32)m->HISoftware.c[0]);
		mDNS_Register_internal(m, &set->RR_HINFO);
		}
	else
		{
		debugf("Not creating HINFO record: platform support layer provided no information");
		set->RR_HINFO.resrec.RecordType = kDNSRecordTypeUnregistered;
		}
	}

mDNSlocal void DeadvertiseInterface(mDNS *const m, NetworkInterfaceInfo *set)
	{
	NetworkInterfaceInfo *intf;
	
    // If we still have address records referring to this one, update them
	NetworkInterfaceInfo *primary = FindFirstAdvertisedInterface(m);
	AuthRecord *A = primary ? &primary->RR_A : mDNSNULL;
	for (intf = m->HostInterfaces; intf; intf = intf->next)
		if (intf->RR_A.RRSet == &set->RR_A)
			intf->RR_A.RRSet = A;

	// Unregister these records.
	// When doing the mDNS_Close processing, we first call DeadvertiseInterface for each interface, so by the time the platform
	// support layer gets to call mDNS_DeregisterInterface, the address and PTR records have already been deregistered for it.
	// Also, in the event of a name conflict, one or more of our records will have been forcibly deregistered.
	// To avoid unnecessary and misleading warning messages, we check the RecordType before calling mDNS_Deregister_internal().
	if (set->RR_A.    resrec.RecordType) mDNS_Deregister_internal(m, &set->RR_A,     mDNS_Dereg_normal);
	if (set->RR_PTR.  resrec.RecordType) mDNS_Deregister_internal(m, &set->RR_PTR,   mDNS_Dereg_normal);
	if (set->RR_HINFO.resrec.RecordType) mDNS_Deregister_internal(m, &set->RR_HINFO, mDNS_Dereg_normal);
	}

mDNSexport void mDNS_SetFQDN(mDNS *const m)
	{
	domainname newmname;
	NetworkInterfaceInfo *intf;
	AuthRecord *rr;
	newmname.c[0] = 0;

	if (!AppendDomainLabel(&newmname, &m->hostlabel))  { LogMsg("ERROR: mDNS_SetFQDN: Cannot create MulticastHostname"); return; }
	if (!AppendLiteralLabelString(&newmname, "local")) { LogMsg("ERROR: mDNS_SetFQDN: Cannot create MulticastHostname"); return; }
	if (SameDomainNameCS(&m->MulticastHostname, &newmname)) { LogMsg("mDNS_SetFQDN - hostname unchanged"); return; }

	mDNS_Lock(m);

	AssignDomainName(&m->MulticastHostname, &newmname);
	// 1. Stop advertising our address records on all interfaces
	for (intf = m->HostInterfaces; intf; intf = intf->next)
		if (intf->Advertise) DeadvertiseInterface(m, intf);

	// 2. Start advertising our address records using the new name
	for (intf = m->HostInterfaces; intf; intf = intf->next)
		if (intf->Advertise) AdvertiseInterface(m, intf);

	// 3. Make sure that any SRV records (and the like) that reference our
	// host name in their rdata get updated to reference this new host name
	for (rr = m->ResourceRecords;  rr; rr=rr->next) if (rr->AutoTarget) SetTargetToHostName(m, rr);
	for (rr = m->DuplicateRecords; rr; rr=rr->next) if (rr->AutoTarget) SetTargetToHostName(m, rr);
	
	mDNS_Unlock(m);
	}

mDNSlocal void mDNS_HostNameCallback(mDNS *const m, AuthRecord *const rr, mStatus result)
	{
	(void)rr;	// Unused parameter
	
	#if MDNS_DEBUGMSGS
		{
		char *msg = "Unknown result";
		if      (result == mStatus_NoError)      msg = "Name registered";
		else if (result == mStatus_NameConflict) msg = "Name conflict";
		debugf("mDNS_HostNameCallback: %##s (%s) %s (%ld)", rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype), msg, result);
		}
	#endif

	if (result == mStatus_NoError)
		{
		// Notify the client that the host name is successfully registered
		if (m->MainCallback)
			m->MainCallback(m, mStatus_NoError);
		}
	else if (result == mStatus_NameConflict)
		{
		domainlabel oldlabel = m->hostlabel;

		// 1. First give the client callback a chance to pick a new name
		if (m->MainCallback)
			m->MainCallback(m, mStatus_NameConflict);

		// 2. If the client callback didn't do it, add (or increment) an index ourselves
		// This needs to be case-insensitive compare, because we need to know that the name has been changed so as to
		// remedy the conflict, and a name that differs only in capitalization will just suffer the exact same conflict again.
		if (SameDomainLabelCS(m->hostlabel.c, oldlabel.c))
			IncrementLabelSuffix(&m->hostlabel, mDNSfalse);
		
		// 3. Generate the FQDNs from the hostlabel,
		// and make sure all SRV records, etc., are updated to reference our new hostname
		mDNS_SetFQDN(m);
		LogMsg("Local Hostname %#s.local already in use; will try %#s.local instead", oldlabel.c, m->hostlabel.c);
		}
	else if (result == mStatus_MemFree)
		{
		// .local hostnames do not require goodbyes - we ignore the MemFree (which is sent directly by
		// mDNS_Deregister_internal), and allow the caller to deallocate immediately following mDNS_DeadvertiseInterface
		debugf("mDNS_HostNameCallback: MemFree (ignored)");
		}
	else
		LogMsg("mDNS_HostNameCallback: Unknown error %ld for registration of record %s", result,  rr->resrec.name->c);
	}

mDNSlocal void UpdateInterfaceProtocols(mDNS *const m, NetworkInterfaceInfo *active)
	{
	NetworkInterfaceInfo *intf;
	active->IPv4Available = mDNSfalse;
	active->IPv6Available = mDNSfalse;
	for (intf = m->HostInterfaces; intf; intf = intf->next)
		if (intf->InterfaceID == active->InterfaceID)
			{
			if (intf->ip.type == mDNSAddrType_IPv4 && intf->McastTxRx) active->IPv4Available = mDNStrue;
			if (intf->ip.type == mDNSAddrType_IPv6 && intf->McastTxRx) active->IPv6Available = mDNStrue;
			}
	}

mDNSexport mStatus mDNS_RegisterInterface(mDNS *const m, NetworkInterfaceInfo *set, mDNSBool flapping)
	{
	mDNSBool FirstOfType = mDNStrue;
	NetworkInterfaceInfo **p = &m->HostInterfaces;

	if (!set->InterfaceID)
		{ LogMsg("Error! Tried to register a NetworkInterfaceInfo %#a with zero InterfaceID", &set->ip); return(mStatus_Invalid); }

	if (!mDNSAddressIsValidNonZero(&set->mask))
		{ LogMsg("Error! Tried to register a NetworkInterfaceInfo %#a with invalid mask %#a", &set->ip, &set->mask); return(mStatus_Invalid); }

	mDNS_Lock(m);
	
	// Assume this interface will be active now, unless we find a duplicate already in the list
	set->InterfaceActive = mDNStrue;
	set->IPv4Available   = (set->ip.type == mDNSAddrType_IPv4 && set->McastTxRx);
	set->IPv6Available   = (set->ip.type == mDNSAddrType_IPv6 && set->McastTxRx);

	// Scan list to see if this InterfaceID is already represented
	while (*p)
		{
		if (*p == set)
			{
			LogMsg("Error! Tried to register a NetworkInterfaceInfo that's already in the list");
			mDNS_Unlock(m);
			return(mStatus_AlreadyRegistered);
			}

		if ((*p)->InterfaceID == set->InterfaceID)
			{
			// This InterfaceID already represented by a different interface in the list, so mark this instance inactive for now
			set->InterfaceActive = mDNSfalse;
			if (set->ip.type == (*p)->ip.type) FirstOfType = mDNSfalse;
			if (set->ip.type == mDNSAddrType_IPv4 && set->McastTxRx) (*p)->IPv4Available = mDNStrue;
			if (set->ip.type == mDNSAddrType_IPv6 && set->McastTxRx) (*p)->IPv6Available = mDNStrue;
			}

		p=&(*p)->next;
		}

	set->next = mDNSNULL;
	*p = set;
	
	if (set->Advertise)
		AdvertiseInterface(m, set);

	LogOperation("mDNS_RegisterInterface: InterfaceID %p %s (%#a) %s", set->InterfaceID, set->ifname, &set->ip,
		set->InterfaceActive ?
			"not represented in list; marking active and retriggering queries" :
			"already represented in list; marking inactive for now");
	
	// In early versions of OS X the IPv6 address remains on an interface even when the interface is turned off,
	// giving the false impression that there's an active representative of this interface when there really isn't.
	// Therefore, when registering an interface, we want to re-trigger our questions and re-probe our Resource Records,
	// even if we believe that we previously had an active representative of this interface.
	if (set->McastTxRx && ((m->KnownBugs & mDNS_KnownBug_PhantomInterfaces) || FirstOfType || set->InterfaceActive))
		{
		DNSQuestion *q;
		AuthRecord *rr;
		// If flapping, delay between first and second queries is eight seconds instead of one
		mDNSs32 delay    = flapping ? mDNSPlatformOneSecond   * 5 : 0;
		mDNSu8  announce = flapping ? (mDNSu8)1                   : InitialAnnounceCount;

		// Use a small amount of randomness:
		// In the case of a network administrator turning on an Ethernet hub so that all the
		// connected machines establish link at exactly the same time, we don't want them all
		// to go and hit the network with identical queries at exactly the same moment.
		if (!m->SuppressSending) m->SuppressSending = m->timenow + (mDNSs32)mDNSRandom((mDNSu32)InitialQuestionInterval);
		
		if (flapping)
			{
			LogMsg("Note: Frequent transitions for interface %s (%#a); network traffic reduction measures in effect", set->ifname, &set->ip);
			if (!m->SuppressProbes ||
				m->SuppressProbes - (m->timenow + delay) < 0)
				m->SuppressProbes = (m->timenow + delay);
			}

		for (q = m->Questions; q; q=q->next)								// Scan our list of questions
			if (mDNSOpaque16IsZero(q->TargetQID))
				if (!q->InterfaceID || q->InterfaceID == set->InterfaceID)		// If non-specific Q, or Q on this specific interface,
					{															// then reactivate this question
					mDNSBool dodelay = flapping && (q->FlappingInterface1 == set->InterfaceID || q->FlappingInterface2 == set->InterfaceID);
					mDNSs32 initial  = dodelay ? InitialQuestionInterval * QuestionIntervalStep2 : InitialQuestionInterval;
					mDNSs32 qdelay   = dodelay ? mDNSPlatformOneSecond * 5 : 0;
					if (dodelay) LogOperation("No cache records for expired %##s (%s); okay to delay questions a little", q->qname.c, DNSTypeName(q->qtype));
						
					if (!q->ThisQInterval || q->ThisQInterval > initial)
						{
						q->ThisQInterval = initial;
						q->RequestUnicast = 2; // Set to 2 because is decremented once *before* we check it
						}
					q->LastQTime = m->timenow - q->ThisQInterval + qdelay;
					q->RecentAnswerPkts = 0;
					SetNextQueryTime(m,q);
					}
		
		// For all our non-specific authoritative resource records (and any dormant records specific to this interface)
		// we now need them to re-probe if necessary, and then re-announce.
		for (rr = m->ResourceRecords; rr; rr=rr->next)
			if (!rr->resrec.InterfaceID || rr->resrec.InterfaceID == set->InterfaceID)
				{
				if (rr->resrec.RecordType == kDNSRecordTypeVerified && !rr->DependentOn) rr->resrec.RecordType = kDNSRecordTypeUnique;
				rr->ProbeCount     = DefaultProbeCountForRecordType(rr->resrec.RecordType);
				if (rr->AnnounceCount < announce) rr->AnnounceCount  = announce;
				rr->ThisAPInterval = DefaultAPIntervalForRecordType(rr->resrec.RecordType);
				InitializeLastAPTime(m, rr);
				}
		}

	mDNS_Unlock(m);
	return(mStatus_NoError);
	}

// NOTE: mDNS_DeregisterInterface calls mDNS_Deregister_internal which can call a user callback, which may change
// the record list and/or question list.
// Any code walking either list must use the CurrentQuestion and/or CurrentRecord mechanism to protect against this.
mDNSexport void mDNS_DeregisterInterface(mDNS *const m, NetworkInterfaceInfo *set, mDNSBool flapping)
	{
	NetworkInterfaceInfo **p = &m->HostInterfaces;
	
	mDNSBool revalidate = mDNSfalse;
	// If this platform has the "phantom interfaces" known bug (e.g. Jaguar), we have to revalidate records every
	// time an interface goes away. Otherwise, when you disconnect the Ethernet cable, the system reports that it
	// still has an IPv6 address, and if we don't revalidate those records don't get deleted in a timely fashion.
	if (m->KnownBugs & mDNS_KnownBug_PhantomInterfaces) revalidate = mDNStrue;

	mDNS_Lock(m);

	// Find this record in our list
	while (*p && *p != set) p=&(*p)->next;
	if (!*p) { debugf("mDNS_DeregisterInterface: NetworkInterfaceInfo not found in list"); mDNS_Unlock(m); return; }

	// Unlink this record from our list
	*p = (*p)->next;
	set->next = mDNSNULL;

	if (!set->InterfaceActive)
		{
		// If this interface not the active member of its set, update the v4/v6Available flags for the active member
		NetworkInterfaceInfo *intf;
		for (intf = m->HostInterfaces; intf; intf = intf->next)
			if (intf->InterfaceActive && intf->InterfaceID == set->InterfaceID)
				UpdateInterfaceProtocols(m, intf);
		}
	else
		{
		NetworkInterfaceInfo *intf;
		for (intf = m->HostInterfaces; intf; intf = intf->next)
			if (intf->InterfaceID == set->InterfaceID)
				break;
		if (intf)
			{
			LogOperation("mDNS_DeregisterInterface: Another representative of InterfaceID %p %s (%#a) exists;"
				" making it active", set->InterfaceID, set->ifname, &set->ip);
			intf->InterfaceActive = mDNStrue;
			UpdateInterfaceProtocols(m, intf);
			
			// See if another representative *of the same type* exists. If not, we mave have gone from
			// dual-stack to v6-only (or v4-only) so we need to reconfirm which records are still valid.
			for (intf = m->HostInterfaces; intf; intf = intf->next)
				if (intf->InterfaceID == set->InterfaceID && intf->ip.type == set->ip.type)
					break;
			if (!intf) revalidate = mDNStrue;
			}
		else
			{
			mDNSu32 slot;
			CacheGroup *cg;
			CacheRecord *rr;
			DNSQuestion *q;
			DNSServer *s;

			LogOperation("mDNS_DeregisterInterface: Last representative of InterfaceID %p %s (%#a) deregistered;"
				" marking questions etc. dormant", set->InterfaceID, set->ifname, &set->ip);

			if (flapping)
				LogMsg("Note: Frequent transitions for interface %s (%#a); network traffic reduction measures in effect",
					set->ifname, &set->ip);

			// 1. Deactivate any questions specific to this interface, and tag appropriate questions
			// so that mDNS_RegisterInterface() knows how swiftly it needs to reactivate them
			for (q = m->Questions; q; q=q->next)
				{
				if (q->InterfaceID == set->InterfaceID) q->ThisQInterval = 0;
				if (!q->InterfaceID || q->InterfaceID == set->InterfaceID)
					{
					q->FlappingInterface2 = q->FlappingInterface1;
					q->FlappingInterface1 = set->InterfaceID;		// Keep history of the last two interfaces to go away
					}
				}

			// 2. Flush any cache records received on this interface
			revalidate = mDNSfalse;		// Don't revalidate if we're flushing the records
			FORALL_CACHERECORDS(slot, cg, rr)
				if (rr->resrec.InterfaceID == set->InterfaceID)
					{
					// If this interface is deemed flapping,
					// postpone deleting the cache records in case the interface comes back again
					if (!flapping) mDNS_PurgeCacheResourceRecord(m, rr);
					else
						{
						// We want these record to go away in 30 seconds
						// We set UnansweredQueries = MaxUnansweredQueries so we don't waste time doing any queries for them --
						// if the interface does come back, any relevant questions will be reactivated anyway
						mDNS_Reconfirm_internal(m, rr, kDefaultReconfirmTimeForFlappingInterface);
						rr->UnansweredQueries = MaxUnansweredQueries;
						}
					}

			// 3. Any DNS servers specific to this interface are now unusable
			for (s = m->DNSServers; s; s = s->next)
				if (s->interface == set->InterfaceID)
					{
					s->interface = mDNSInterface_Any;
					s->teststate = DNSServer_Disabled;
					}
			}
		}

	// If we were advertising on this interface, deregister those address and reverse-lookup records now
	if (set->Advertise) DeadvertiseInterface(m, set);

	// If we have any cache records received on this interface that went away, then re-verify them.
	// In some versions of OS X the IPv6 address remains on an interface even when the interface is turned off,
	// giving the false impression that there's an active representative of this interface when there really isn't.
	// Don't need to do this when shutting down, because *all* interfaces are about to go away
	if (revalidate && !m->mDNS_shutdown)
		{
		mDNSu32 slot;
		CacheGroup *cg;
		CacheRecord *rr;
		m->NextCacheCheck = m->timenow;
		FORALL_CACHERECORDS(slot, cg, rr)
			if (rr->resrec.InterfaceID == set->InterfaceID)
				mDNS_Reconfirm_internal(m, rr, kDefaultReconfirmTimeForFlappingInterface);
		}

	mDNS_Unlock(m);
	}

mDNSlocal void ServiceCallback(mDNS *const m, AuthRecord *const rr, mStatus result)
	{
	ServiceRecordSet *sr = (ServiceRecordSet *)rr->RecordContext;
	(void)m;	// Unused parameter

	#if MDNS_DEBUGMSGS
		{
		char *msg = "Unknown result";
		if      (result == mStatus_NoError)      msg = "Name Registered";
		else if (result == mStatus_NameConflict) msg = "Name Conflict";
		else if (result == mStatus_MemFree)      msg = "Memory Free";
		debugf("ServiceCallback: %##s (%s) %s (%ld)", rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype), msg, result);
		}
	#endif

	// Only pass on the NoError acknowledgement for the SRV record (when it finishes probing)
	if (result == mStatus_NoError && rr != &sr->RR_SRV) return;

	// If we got a name conflict on either SRV or TXT, forcibly deregister this service, and record that we did that
	if (result == mStatus_NameConflict)
		{
		sr->Conflict = mDNStrue;				// Record that this service set had a conflict
		mDNS_DeregisterService(m, sr);			// Unlink the records from our list
		return;
		}
	
	if (result == mStatus_MemFree)
		{
		// If the PTR record or any of the subtype PTR records are still in the process of deregistering,
		// don't pass on the NameConflict/MemFree message until every record is finished cleaning up.
		mDNSu32 i;
		if (sr->RR_PTR.resrec.RecordType != kDNSRecordTypeUnregistered) return;
		for (i=0; i<sr->NumSubTypes; i++) if (sr->SubTypes[i].resrec.RecordType != kDNSRecordTypeUnregistered) return;

		// If this ServiceRecordSet was forcibly deregistered, and now its memory is ready for reuse,
		// then we can now report the NameConflict to the client
		if (sr->Conflict) result = mStatus_NameConflict;
		}

	// CAUTION: MUST NOT do anything more with sr after calling sr->Callback(), because the client's callback
	// function is allowed to do anything, including deregistering this service and freeing its memory.
	if (sr->ServiceCallback)
		sr->ServiceCallback(m, sr, result);
	}

mDNSlocal void NSSCallback(mDNS *const m, AuthRecord *const rr, mStatus result)
	{
	ServiceRecordSet *sr = (ServiceRecordSet *)rr->RecordContext;
	if (sr->ServiceCallback)
		sr->ServiceCallback(m, sr, result);
	}

mDNSlocal mStatus uDNS_RegisterService(mDNS *const m, ServiceRecordSet *srs)
	{
	mDNSu32 i;
	ServiceRecordSet **p = &m->ServiceRegistrations;
	while (*p && *p != srs) p=&(*p)->uDNS_next;
	if (*p) { LogMsg("uDNS_RegisterService: %p %##s already in list", srs, srs->RR_SRV.resrec.name->c); return(mStatus_AlreadyRegistered); }

	srs->uDNS_next = mDNSNULL;
	*p = srs;

	srs->RR_SRV.resrec.rroriginalttl = kHostNameTTL;
	srs->RR_TXT.resrec.rroriginalttl = kStandardTTL;
	srs->RR_PTR.resrec.rroriginalttl = kStandardTTL;
	for (i = 0; i < srs->NumSubTypes;i++) srs->SubTypes[i].resrec.rroriginalttl = kStandardTTL;

	srs->srs_uselease = mDNStrue;

	if (srs->RR_SRV.AutoTarget)
		{
		// For autotunnel services pointing at our IPv6 ULA we don't need or want a NAT mapping, but for all other
		// advertised services referencing our uDNS hostname, we want NAT mappings automatically created as appropriate,
		// with the port number in our advertised SRV record automatically tracking the external mapped port.
		DomainAuthInfo *AuthInfo = GetAuthInfoForName_internal(m, srs->RR_SRV.resrec.name);
		if (!AuthInfo || !AuthInfo->AutoTunnel) srs->RR_SRV.AutoTarget = Target_AutoHostAndNATMAP;
		}

	if (!GetServiceTarget(m, srs))
		{
		// defer registration until we've got a target
		debugf("uDNS_RegisterService - no target for %##s", srs->RR_SRV.resrec.name->c);
		srs->state = regState_NoTarget;
		srs->nta   = mDNSNULL;
		return mStatus_NoError;
		}

	srs->state = regState_FetchingZoneData;
	srs->nta = StartGetZoneData(m, srs->RR_SRV.resrec.name, ZoneServiceUpdate, ServiceRegistrationZoneDataComplete, srs);
	return srs->nta ? mStatus_NoError : mStatus_NoMemoryErr;
	}

// Note:
// Name is first label of domain name (any dots in the name are actual dots, not label separators)
// Type is service type (e.g. "_ipp._tcp.")
// Domain is fully qualified domain name (i.e. ending with a null label)
// We always register a TXT, even if it is empty (so that clients are not
// left waiting forever looking for a nonexistent record.)
// If the host parameter is mDNSNULL or the root domain (ASCII NUL),
// then the default host name (m->MulticastHostname) is automatically used
// If the optional target host parameter is set, then the storage it points to must remain valid for the lifetime of the service registration
mDNSexport mStatus mDNS_RegisterService(mDNS *const m, ServiceRecordSet *sr,
	const domainlabel *const name, const domainname *const type, const domainname *const domain,
	const domainname *const host, mDNSIPPort port, const mDNSu8 txtinfo[], mDNSu16 txtlen,
	AuthRecord *SubTypes, mDNSu32 NumSubTypes,
	const mDNSInterfaceID InterfaceID, mDNSServiceCallback Callback, void *Context)
	{
	mStatus err;
	mDNSu32 i;

	sr->state                  = regState_Zero;
	sr->srs_uselease           = 0;
	sr->expire                 = 0;
	sr->TestForSelfConflict    = 0;
	sr->Private                = 0;
	sr->id                     = zeroID;
	sr->zone.c[0]              = 0;
	sr->ns                     = zeroAddr;
	sr->SRSUpdatePort          = zeroIPPort;
	mDNSPlatformMemZero(&sr->NATinfo, sizeof(sr->NATinfo));
	sr->ClientCallbackDeferred = 0;
	sr->DeferredStatus         = 0;
	sr->SRVUpdateDeferred      = 0;
	sr->SRVChanged             = 0;
	sr->tcp                    = mDNSNULL;

	sr->ServiceCallback = Callback;
	sr->ServiceContext  = Context;
	sr->Conflict        = mDNSfalse;

	sr->Extras          = mDNSNULL;
	sr->NumSubTypes     = NumSubTypes;
	sr->SubTypes        = SubTypes;
	
	// Initialize the AuthRecord objects to sane values
	// Need to initialize everything correctly *before* making the decision whether to do a RegisterNoSuchService and bail out
	mDNS_SetupResourceRecord(&sr->RR_ADV, mDNSNULL, InterfaceID, kDNSType_PTR, kStandardTTL, kDNSRecordTypeAdvisory, ServiceCallback, sr);
	mDNS_SetupResourceRecord(&sr->RR_PTR, mDNSNULL, InterfaceID, kDNSType_PTR, kStandardTTL, kDNSRecordTypeShared,   ServiceCallback, sr);
	mDNS_SetupResourceRecord(&sr->RR_SRV, mDNSNULL, InterfaceID, kDNSType_SRV, kHostNameTTL, kDNSRecordTypeUnique,   ServiceCallback, sr);
	mDNS_SetupResourceRecord(&sr->RR_TXT, mDNSNULL, InterfaceID, kDNSType_TXT, kStandardTTL, kDNSRecordTypeUnique,   ServiceCallback, sr);

	// If port number is zero, that means the client is really trying to do a RegisterNoSuchService
	if (mDNSIPPortIsZero(port))
		return(mDNS_RegisterNoSuchService(m, &sr->RR_SRV, name, type, domain, mDNSNULL, mDNSInterface_Any, NSSCallback, sr));

	// If the client is registering an oversized TXT record,
	// it is the client's responsibility to alloate a ServiceRecordSet structure that is large enough for it
	if (sr->RR_TXT.resrec.rdata->MaxRDLength < txtlen)
		sr->RR_TXT.resrec.rdata->MaxRDLength = txtlen;

	// Set up the record names
	// For now we only create an advisory record for the main type, not for subtypes
	// We need to gain some operational experience before we decide if there's a need to create them for subtypes too
	if (ConstructServiceName(&sr->RR_ADV.namestorage, (const domainlabel*)"\x09_services", (const domainname*)"\x07_dns-sd\x04_udp", domain) == mDNSNULL)
		return(mStatus_BadParamErr);
	if (ConstructServiceName(&sr->RR_PTR.namestorage, mDNSNULL, type, domain) == mDNSNULL) return(mStatus_BadParamErr);
	if (ConstructServiceName(&sr->RR_SRV.namestorage, name,     type, domain) == mDNSNULL) return(mStatus_BadParamErr);
	AssignDomainName(&sr->RR_TXT.namestorage, sr->RR_SRV.resrec.name);
	
	// 1. Set up the ADV record rdata to advertise our service type
	AssignDomainName(&sr->RR_ADV.resrec.rdata->u.name, sr->RR_PTR.resrec.name);

	// 2. Set up the PTR record rdata to point to our service name
	// We set up two additionals, so when a client asks for this PTR we automatically send the SRV and the TXT too
	AssignDomainName(&sr->RR_PTR.resrec.rdata->u.name, sr->RR_SRV.resrec.name);
	sr->RR_PTR.Additional1 = &sr->RR_SRV;
	sr->RR_PTR.Additional2 = &sr->RR_TXT;

	// 2a. Set up any subtype PTRs to point to our service name
	// If the client is using subtypes, it is the client's responsibility to have
	// already set the first label of the record name to the subtype being registered
	for (i=0; i<NumSubTypes; i++)
		{
		domainname st;
		AssignDomainName(&st, sr->SubTypes[i].resrec.name);
		st.c[1+st.c[0]] = 0;			// Only want the first label, not the whole FQDN (particularly for mDNS_RenameAndReregisterService())
		AppendDomainName(&st, type);
		mDNS_SetupResourceRecord(&sr->SubTypes[i], mDNSNULL, InterfaceID, kDNSType_PTR, kStandardTTL, kDNSRecordTypeShared, ServiceCallback, sr);
		if (ConstructServiceName(&sr->SubTypes[i].namestorage, mDNSNULL, &st, domain) == mDNSNULL) return(mStatus_BadParamErr);
		AssignDomainName(&sr->SubTypes[i].resrec.rdata->u.name, &sr->RR_SRV.namestorage);
		sr->SubTypes[i].Additional1 = &sr->RR_SRV;
		sr->SubTypes[i].Additional2 = &sr->RR_TXT;
		}

	// 3. Set up the SRV record rdata.
	sr->RR_SRV.resrec.rdata->u.srv.priority = 0;
	sr->RR_SRV.resrec.rdata->u.srv.weight   = 0;
	sr->RR_SRV.resrec.rdata->u.srv.port     = port;

	// Setting AutoTarget tells DNS that the target of this SRV is to be automatically kept in sync with our host name
	if (host && host->c[0]) AssignDomainName(&sr->RR_SRV.resrec.rdata->u.srv.target, host);
	else { sr->RR_SRV.AutoTarget = Target_AutoHost; sr->RR_SRV.resrec.rdata->u.srv.target.c[0] = '\0'; }

	// 4. Set up the TXT record rdata,
	// and set DependentOn because we're depending on the SRV record to find and resolve conflicts for us
	if (txtinfo == mDNSNULL) sr->RR_TXT.resrec.rdlength = 0;
	else if (txtinfo != sr->RR_TXT.resrec.rdata->u.txt.c)
		{
		sr->RR_TXT.resrec.rdlength = txtlen;
		if (sr->RR_TXT.resrec.rdlength > sr->RR_TXT.resrec.rdata->MaxRDLength) return(mStatus_BadParamErr);
		mDNSPlatformMemCopy(sr->RR_TXT.resrec.rdata->u.txt.c, txtinfo, txtlen);
		}
	sr->RR_TXT.DependentOn = &sr->RR_SRV;

#ifndef UNICAST_DISABLED
	// If the client has specified an explicit InterfaceID,
	// then we do a multicast registration on that interface, even for unicast domains.
	if (!(InterfaceID == mDNSInterface_LocalOnly || IsLocalDomain(&sr->RR_SRV.namestorage)))
		{
		mStatus status;
		mDNS_Lock(m);
		// BIND named (name daemon) doesn't allow TXT records with zero-length rdata. This is strictly speaking correct,
		// since RFC 1035 specifies a TXT record as "One or more <character-string>s", not "Zero or more <character-string>s".
		// Since some legacy apps try to create zero-length TXT records, we'll silently correct it here.
		// (We have to duplicate this check here because uDNS_RegisterService() bypasses the usual mDNS_Register_internal() bottleneck)
		if (!sr->RR_TXT.resrec.rdlength) { sr->RR_TXT.resrec.rdlength = 1; sr->RR_TXT.resrec.rdata->u.txt.c[0] = 0; }
		
		status = uDNS_RegisterService(m, sr);
		mDNS_Unlock(m);
		return(status);
		}
#endif
	mDNS_Lock(m);
	err = mDNS_Register_internal(m, &sr->RR_SRV);
	if (!err) err = mDNS_Register_internal(m, &sr->RR_TXT);
	// We register the RR_PTR last, because we want to be sure that in the event of a forced call to
	// mDNS_Close, the RR_PTR will be the last one to be forcibly deregistered, since that is what triggers
	// the mStatus_MemFree callback to ServiceCallback, which in turn passes on the mStatus_MemFree back to
	// the client callback, which is then at liberty to free the ServiceRecordSet memory at will. We need to
	// make sure we've deregistered all our records and done any other necessary cleanup before that happens.
	if (!err) err = mDNS_Register_internal(m, &sr->RR_ADV);
	for (i=0; i<NumSubTypes; i++) if (!err) err = mDNS_Register_internal(m, &sr->SubTypes[i]);
	if (!err) err = mDNS_Register_internal(m, &sr->RR_PTR);

	mDNS_Unlock(m);
	
	if (err) mDNS_DeregisterService(m, sr);
	return(err);
	}

mDNSlocal void DummyCallback(mDNS *const m, AuthRecord *rr, mStatus result)
	{
	(void)m;		// Unused
	(void)rr;		// Unused
	(void)result;	// Unused
	LogOperation("DummyCallback %d %s", result, ARDisplayString(m, rr));
	}

mDNSexport mStatus mDNS_AddRecordToService(mDNS *const m, ServiceRecordSet *sr,
	ExtraResourceRecord *extra, RData *rdata, mDNSu32 ttl)
	{
	ExtraResourceRecord **e;
	mStatus status;

	extra->next = mDNSNULL;
	mDNS_SetupResourceRecord(&extra->r, rdata, sr->RR_PTR.resrec.InterfaceID,
		extra->r.resrec.rrtype, ttl, kDNSRecordTypeUnique, ServiceCallback, sr);
	AssignDomainName(&extra->r.namestorage, sr->RR_SRV.resrec.name);
	
	mDNS_Lock(m);
	e = &sr->Extras;
	while (*e) e = &(*e)->next;

	if (ttl == 0) ttl = kStandardTTL;

	extra->r.DependentOn = &sr->RR_SRV;

	debugf("mDNS_AddRecordToService adding record to %##s %s %d",
		extra->r.resrec.name->c, DNSTypeName(extra->r.resrec.rrtype), extra->r.resrec.rdlength);

	status = mDNS_Register_internal(m, &extra->r);
	if (status == mStatus_NoError)
		{
		*e = extra;
#ifndef UNICAST_DISABLED
		if (sr->RR_SRV.resrec.InterfaceID != mDNSInterface_LocalOnly && !IsLocalDomain(sr->RR_SRV.resrec.name))
			{
			extra->r.resrec.RecordType = kDNSRecordTypeShared;	// don't want it to conflict with the service name (???)
			extra->r.RecordCallback = DummyCallback;	// don't generate callbacks for extra RRs for unicast services (WHY NOT????)
			if (sr->state != regState_Registered && sr->state != regState_Refresh) extra->r.state = regState_ExtraQueued;
			}
#endif
		}

	mDNS_Unlock(m);
	return(status);
	}

mDNSexport mStatus mDNS_RemoveRecordFromService(mDNS *const m, ServiceRecordSet *sr, ExtraResourceRecord *extra,
	mDNSRecordCallback MemFreeCallback, void *Context)
	{
	ExtraResourceRecord **e;
	mStatus status;

	mDNS_Lock(m);
	e = &sr->Extras;
	while (*e && *e != extra) e = &(*e)->next;
	if (!*e)
		{
		debugf("mDNS_RemoveRecordFromService failed to remove record from %##s", extra->r.resrec.name->c);
		status = mStatus_BadReferenceErr;
		}
	else
		{
		debugf("mDNS_RemoveRecordFromService removing record from %##s", extra->r.resrec.name->c);
		extra->r.RecordCallback = MemFreeCallback;
		extra->r.RecordContext  = Context;
		*e = (*e)->next;
		status = mDNS_Deregister_internal(m, &extra->r, mDNS_Dereg_normal);
		}
	mDNS_Unlock(m);
	return(status);
	}

mDNSexport mStatus mDNS_RenameAndReregisterService(mDNS *const m, ServiceRecordSet *const sr, const domainlabel *newname)
	{
	// NOTE: Don't need to use mDNS_Lock(m) here, because this code is just using public routines
	// mDNS_RegisterService() and mDNS_AddRecordToService(), which do the right locking internally.
	domainlabel name1, name2;
	domainname type, domain;
	const domainname *host = sr->RR_SRV.AutoTarget ? mDNSNULL : &sr->RR_SRV.resrec.rdata->u.srv.target;
	ExtraResourceRecord *extras = sr->Extras;
	mStatus err;

	DeconstructServiceName(sr->RR_SRV.resrec.name, &name1, &type, &domain);
	if (!newname)
		{
		name2 = name1;
		IncrementLabelSuffix(&name2, mDNStrue);
		newname = &name2;
		}
	
	if (SameDomainName(&domain, &localdomain))
		LogMsg("%##s service renamed from \"%#s\" to \"%#s\"", type.c, name1.c, newname->c);
	else LogMsg("%##s service (domain %##s) renamed from \"%#s\" to \"%#s\"",type.c, domain.c, name1.c, newname->c);

	err = mDNS_RegisterService(m, sr, newname, &type, &domain,
		host, sr->RR_SRV.resrec.rdata->u.srv.port, sr->RR_TXT.resrec.rdata->u.txt.c, sr->RR_TXT.resrec.rdlength,
		sr->SubTypes, sr->NumSubTypes,
		sr->RR_PTR.resrec.InterfaceID, sr->ServiceCallback, sr->ServiceContext);

	// mDNS_RegisterService() just reset sr->Extras to NULL.
	// Fortunately we already grabbed ourselves a copy of this pointer (above), so we can now run
	// through the old list of extra records, and re-add them to our freshly created service registration
	while (!err && extras)
		{
		ExtraResourceRecord *e = extras;
		extras = extras->next;
		err = mDNS_AddRecordToService(m, sr, e, e->r.resrec.rdata, e->r.resrec.rroriginalttl);
		}
	
	return(err);
	}

// NOTE: mDNS_DeregisterService calls mDNS_Deregister_internal which can call a user callback,
// which may change the record list and/or question list.
// Any code walking either list must use the CurrentQuestion and/or CurrentRecord mechanism to protect against this.
mDNSexport mStatus mDNS_DeregisterService(mDNS *const m, ServiceRecordSet *sr)
	{
	// If port number is zero, that means this was actually registered using mDNS_RegisterNoSuchService()
	if (mDNSIPPortIsZero(sr->RR_SRV.resrec.rdata->u.srv.port)) return(mDNS_DeregisterNoSuchService(m, &sr->RR_SRV));

#ifndef UNICAST_DISABLED
	if (!(sr->RR_SRV.resrec.InterfaceID == mDNSInterface_LocalOnly || IsLocalDomain(sr->RR_SRV.resrec.name)))
		{
		mStatus status;
		mDNS_Lock(m);
		status = uDNS_DeregisterService(m, sr);
		mDNS_Unlock(m);
		return(status);
		}
#endif
	if (sr->RR_PTR.resrec.RecordType == kDNSRecordTypeUnregistered)
		{
		debugf("Service set for %##s already deregistered", sr->RR_SRV.resrec.name->c);
		return(mStatus_BadReferenceErr);
		}
	else if (sr->RR_PTR.resrec.RecordType == kDNSRecordTypeDeregistering)
		{
		debugf("Service set for %##s already in the process of deregistering", sr->RR_SRV.resrec.name->c);
		// Avoid race condition:
		// If a service gets a conflict, then we set the Conflict flag to tell us to generate
		// an mStatus_NameConflict message when we get the mStatus_MemFree for our PTR record.
		// If the client happens to deregister the service in the middle of that process, then
		// we clear the flag back to the normal state, so that we deliver a plain mStatus_MemFree
		// instead of incorrectly promoting it to mStatus_NameConflict.
		// This race condition is exposed particularly when the conformance test generates
		// a whole batch of simultaneous conflicts across a range of services all advertised
		// using the same system default name, and if we don't take this precaution then
		// we end up incrementing m->nicelabel multiple times instead of just once.
		// <rdar://problem/4060169> Bug when auto-renaming Computer Name after name collision
		sr->Conflict = mDNSfalse;
		return(mStatus_NoError);
		}
	else
		{
		mDNSu32 i;
		mStatus status;
		ExtraResourceRecord *e;
		mDNS_Lock(m);
		e = sr->Extras;
	
		// We use mDNS_Dereg_repeat because, in the event of a collision, some or all of the
		// SRV, TXT, or Extra records could have already been automatically deregistered, and that's okay
		mDNS_Deregister_internal(m, &sr->RR_SRV, mDNS_Dereg_repeat);
		mDNS_Deregister_internal(m, &sr->RR_TXT, mDNS_Dereg_repeat);
		
		mDNS_Deregister_internal(m, &sr->RR_ADV, mDNS_Dereg_normal);
	
		// We deregister all of the extra records, but we leave the sr->Extras list intact
		// in case the client wants to do a RenameAndReregister and reinstate the registration
		while (e)
			{
			mDNS_Deregister_internal(m, &e->r, mDNS_Dereg_repeat);
			e = e->next;
			}

		for (i=0; i<sr->NumSubTypes; i++)
			mDNS_Deregister_internal(m, &sr->SubTypes[i], mDNS_Dereg_normal);

		// Be sure to deregister the PTR last!
		// Deregistering this record is what triggers the mStatus_MemFree callback to ServiceCallback,
		// which in turn passes on the mStatus_MemFree (or mStatus_NameConflict) back to the client callback,
		// which is then at liberty to free the ServiceRecordSet memory at will. We need to make sure
		// we've deregistered all our records and done any other necessary cleanup before that happens.
		status = mDNS_Deregister_internal(m, &sr->RR_PTR, mDNS_Dereg_normal);
		mDNS_Unlock(m);
		return(status);
		}
	}

// Create a registration that asserts that no such service exists with this name.
// This can be useful where there is a given function is available through several protocols.
// For example, a printer called "Stuart's Printer" may implement printing via the "pdl-datastream" and "IPP"
// protocols, but not via "LPR". In this case it would be prudent for the printer to assert the non-existence of an
// "LPR" service called "Stuart's Printer". Without this precaution, another printer than offers only "LPR" printing
// could inadvertently advertise its service under the same name "Stuart's Printer", which might be confusing for users.
mDNSexport mStatus mDNS_RegisterNoSuchService(mDNS *const m, AuthRecord *const rr,
	const domainlabel *const name, const domainname *const type, const domainname *const domain,
	const domainname *const host,
	const mDNSInterfaceID InterfaceID, mDNSRecordCallback Callback, void *Context)
	{
	mDNS_SetupResourceRecord(rr, mDNSNULL, InterfaceID, kDNSType_SRV, kHostNameTTL, kDNSRecordTypeUnique, Callback, Context);
	if (ConstructServiceName(&rr->namestorage, name, type, domain) == mDNSNULL) return(mStatus_BadParamErr);
	rr->resrec.rdata->u.srv.priority    = 0;
	rr->resrec.rdata->u.srv.weight      = 0;
	rr->resrec.rdata->u.srv.port        = zeroIPPort;
	if (host && host->c[0]) AssignDomainName(&rr->resrec.rdata->u.srv.target, host);
	else rr->AutoTarget = Target_AutoHost;
	return(mDNS_Register(m, rr));
	}

mDNSexport mStatus mDNS_AdvertiseDomains(mDNS *const m, AuthRecord *rr,
	mDNS_DomainType DomainType, const mDNSInterfaceID InterfaceID, char *domname)
	{
	mDNS_SetupResourceRecord(rr, mDNSNULL, InterfaceID, kDNSType_PTR, kStandardTTL, kDNSRecordTypeShared, mDNSNULL, mDNSNULL);
	if (!MakeDomainNameFromDNSNameString(&rr->namestorage, mDNS_DomainTypeNames[DomainType])) return(mStatus_BadParamErr);
	if (!MakeDomainNameFromDNSNameString(&rr->resrec.rdata->u.name, domname))                 return(mStatus_BadParamErr);
	return(mDNS_Register(m, rr));
	}
	
mDNSOpaque16 mDNS_NewMessageID(mDNS * const m)
	{
	static mDNSBool randomized = mDNSfalse;

	if (!randomized) { m->NextMessageID = (mDNSu16)mDNSRandom(0xFFFF); randomized = mDNStrue; }
	if (m->NextMessageID == 0) m->NextMessageID++;
	return mDNSOpaque16fromIntVal(m->NextMessageID++);
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - Startup and Shutdown
#endif

mDNSlocal void mDNS_GrowCache_internal(mDNS *const m, CacheEntity *storage, mDNSu32 numrecords)
	{
	if (storage && numrecords)
		{
		mDNSu32 i;
		debugf("Adding cache storage for %d more records (%d bytes)", numrecords, numrecords*sizeof(CacheEntity));
		for (i=0; i<numrecords; i++) storage[i].next = &storage[i+1];
		storage[numrecords-1].next = m->rrcache_free;
		m->rrcache_free = storage;
		m->rrcache_size += numrecords;
		}
	}

mDNSexport void mDNS_GrowCache(mDNS *const m, CacheEntity *storage, mDNSu32 numrecords)
	{
	mDNS_Lock(m);
	mDNS_GrowCache_internal(m, storage, numrecords);
	mDNS_Unlock(m);
	}

mDNSexport mStatus mDNS_Init(mDNS *const m, mDNS_PlatformSupport *const p,
	CacheEntity *rrcachestorage, mDNSu32 rrcachesize,
	mDNSBool AdvertiseLocalAddresses, mDNSCallback *Callback, void *Context)
	{
	mDNSu32 slot;
	mDNSs32 timenow;
	mStatus result;
	
	if (!rrcachestorage) rrcachesize = 0;
	
	m->p                       = p;
	m->KnownBugs               = 0;
	m->CanReceiveUnicastOn5353 = mDNSfalse; // Assume we can't receive unicasts on 5353, unless platform layer tells us otherwise
	m->AdvertiseLocalAddresses = AdvertiseLocalAddresses;
	m->mDNSPlatformStatus      = mStatus_Waiting;
	m->UnicastPort4            = zeroIPPort;
	m->UnicastPort6            = zeroIPPort;
	m->MainCallback            = Callback;
	m->MainContext             = Context;
	m->rec.r.resrec.RecordType = 0;

	// For debugging: To catch and report locking failures
	m->mDNS_busy               = 0;
	m->mDNS_reentrancy         = 0;
	m->mDNS_shutdown           = mDNSfalse;
	m->lock_rrcache            = 0;
	m->lock_Questions          = 0;
	m->lock_Records            = 0;

	// Task Scheduling variables
	result = mDNSPlatformTimeInit();
	if (result != mStatus_NoError) return(result);
	m->timenow_adjust = (mDNSs32)mDNSRandom(0xFFFFFFFF);
	timenow = mDNS_TimeNow_NoLock(m);

	m->timenow                 = 0;		// MUST only be set within mDNS_Lock/mDNS_Unlock section
	m->timenow_last            = timenow;
	m->NextScheduledEvent      = timenow;
	m->SuppressSending         = timenow;
	m->NextCacheCheck          = timenow + 0x78000000;
	m->NextScheduledQuery      = timenow + 0x78000000;
	m->NextScheduledProbe      = timenow + 0x78000000;
	m->NextScheduledResponse   = timenow + 0x78000000;
	m->NextScheduledNATOp      = timenow + 0x78000000;
	m->RandomQueryDelay        = 0;
	m->RandomReconfirmDelay    = 0;
	m->PktNum                  = 0;
	m->SendDeregistrations     = mDNSfalse;
	m->SendImmediateAnswers    = mDNSfalse;
	m->SleepState              = mDNSfalse;

	// These fields only required for mDNS Searcher...
	m->Questions               = mDNSNULL;
	m->NewQuestions            = mDNSNULL;
	m->CurrentQuestion         = mDNSNULL;
	m->LocalOnlyQuestions      = mDNSNULL;
	m->NewLocalOnlyQuestions   = mDNSNULL;
	m->rrcache_size            = 0;
	m->rrcache_totalused       = 0;
	m->rrcache_active          = 0;
	m->rrcache_report          = 10;
	m->rrcache_free            = mDNSNULL;

	for (slot = 0; slot < CACHE_HASH_SLOTS; slot++) m->rrcache_hash[slot] = mDNSNULL;

	mDNS_GrowCache_internal(m, rrcachestorage, rrcachesize);

	// Fields below only required for mDNS Responder...
	m->hostlabel.c[0]          = 0;
	m->nicelabel.c[0]          = 0;
	m->MulticastHostname.c[0]  = 0;
	m->HIHardware.c[0]         = 0;
	m->HISoftware.c[0]         = 0;
	m->ResourceRecords         = mDNSNULL;
	m->DuplicateRecords        = mDNSNULL;
	m->NewLocalRecords         = mDNSNULL;
	m->CurrentRecord           = mDNSNULL;
	m->HostInterfaces          = mDNSNULL;
	m->ProbeFailTime           = 0;
	m->NumFailedProbes         = 0;
	m->SuppressProbes          = 0;

#ifndef UNICAST_DISABLED
	m->NextuDNSEvent            = timenow + 0x78000000;
	m->NextSRVUpdate            = timenow + 0x78000000;
	m->SuppressStdPort53Queries = 0;

	m->ServiceRegistrations     = mDNSNULL;
	m->NextMessageID            = 0;
	m->DNSServers               = mDNSNULL;

	m->Router                   = zeroAddr;
	m->AdvertisedV4             = zeroAddr;
	m->AdvertisedV6             = zeroAddr;

	m->AuthInfoList             = mDNSNULL;

	m->ReverseMap.ThisQInterval = -1;
	m->StaticHostname.c[0]      = 0;
	m->FQDN.c[0]                = 0;
	m->Hostnames                = mDNSNULL;
	m->AutoTunnelHostAddr.b[0]  = 0;
	m->AutoTunnelHostAddrActive = mDNSfalse;
	m->AutoTunnelLabel.c[0]     = 0;

	m->RegisterSearchDomains    = mDNSfalse;

	// NAT traversal fields
	m->NATTraversals            = mDNSNULL;
	m->CurrentNATTraversal      = mDNSNULL;
	m->retryIntervalGetAddr     = 0;	// delta between time sent and retry
	m->retryGetAddr             = timenow + 0x78000000;	// absolute time when we retry
	m->ExternalAddress          = zerov4Addr;

	m->NATMcastRecvskt          = mDNSNULL;
	m->NATMcastRecvsk2          = mDNSNULL;
	m->LastNATupseconds         = 0;
	m->LastNATReplyLocalTime    = timenow;

	m->UPnPInterfaceID          = 0;
	m->UPnPRouterPort           = zeroIPPort;
	m->UPnPSOAPPort             = zeroIPPort;
	m->UPnPRouterURL            = mDNSNULL;
	m->UPnPSOAPURL              = mDNSNULL;
	m->UPnPRouterAddressString  = mDNSNULL;
	m->UPnPSOAPAddressString    = mDNSNULL;
#endif

#if APPLE_OSX_mDNSResponder
	m->TunnelClients            = mDNSNULL;
#endif

	result = mDNSPlatformInit(m);

#ifndef UNICAST_DISABLED
	// It's better to do this *after* the platform layer has set up the
	// interface list and security credentials
	uDNS_SetupDNSConfig(m);						// Get initial DNS configuration
#endif

	return(result);
	}

mDNSlocal void DynDNSHostNameCallback(mDNS *const m, AuthRecord *const rr, mStatus result)
	{
	(void)m;	// unused
	debugf("NameStatusCallback: result %d for registration of name %##s", result, rr->resrec.name->c);
	mDNSPlatformDynDNSHostNameStatusChanged(rr->resrec.name, result);
	}

mDNSexport mStatus uDNS_SetupDNSConfig(mDNS *const m)
	{
	mDNSu32 slot;
	CacheGroup *cg;
	CacheRecord *cr;

	mDNSAddr     v4, v6, r;
	domainname   fqdn;
	DNSServer   *ptr, **p = &m->DNSServers;
	DNSQuestion *q;

	if (m->RegisterSearchDomains) uDNS_RegisterSearchDomains(m);

	mDNS_Lock(m);

	// Let the platform layer get the current DNS information
	// The m->RegisterSearchDomains boolean is so that we lazily get the search domain list only on-demand
	// (no need to hit the network with domain enumeration queries until we actually need that information).
	for (ptr = m->DNSServers; ptr; ptr = ptr->next) ptr->del = mDNStrue;

	mDNSPlatformSetDNSConfig(m, mDNStrue, mDNSfalse, &fqdn, mDNSNULL, mDNSNULL);

	// Update our qDNSServer pointers before we go and free the DNSServer object memory
	for (q = m->Questions; q; q=q->next)
		if (!mDNSOpaque16IsZero(q->TargetQID))
			{
			DNSServer *s = GetServerForName(m, &q->qname);
			if (q->qDNSServer != s)
				{
				// If DNS Server for this question has changed, reactivate it
				LogOperation("Updating DNS Server from %#a:%d to %#a:%d for %##s (%s)",
					q->qDNSServer ? &q->qDNSServer->addr : mDNSNULL, mDNSVal16(q->qDNSServer ? q->qDNSServer->port : zeroIPPort),
					s             ? &s->addr             : mDNSNULL, mDNSVal16(s             ? s->port             : zeroIPPort),
					q->qname.c, DNSTypeName(q->qtype));
				q->qDNSServer = s;
				ActivateUnicastQuery(m, q);
				}
			}

	while (*p)
		{
		if ((*p)->del)
			{
			// Scan our cache, looking for uDNS records that we would have queried this server for.
			// We reconfirm any records that match, because in this world of split DNS, firewalls, etc.
			// different DNS servers can give different answers to the same question.
			ptr = *p;
			ptr->del = mDNSfalse;	// Clear del so GetServerForName will (temporarily) find this server again before it's finally deleted
			FORALL_CACHERECORDS(slot, cg, cr)
				if (!cr->resrec.InterfaceID && GetServerForName(m, cr->resrec.name) == ptr)
					mDNS_Reconfirm_internal(m, cr, kDefaultReconfirmTimeForNoAnswer);
			*p = (*p)->next;
			mDNSPlatformMemFree(ptr);
			}
		else
			p = &(*p)->next;
		}

	// If we have no DNS servers at all, then immediately purge all unicast cache records (including for LLQs)
	// This is important for giving prompt remove events when the user disconnects the Ethernet cable or turns off wireless
	// Otherwise, stale data lingers for 5-10 seconds, which is not the user-experience people expect from Bonjour
	if (!m->DNSServers) FORALL_CACHERECORDS(slot, cg, cr) if (!cr->resrec.InterfaceID) mDNS_PurgeCacheResourceRecord(m, cr);

	// Did our FQDN change?
	if (!SameDomainName(&fqdn, &m->FQDN))
		{
		if (m->FQDN.c[0]) mDNS_RemoveDynDNSHostName(m, &m->FQDN);

		AssignDomainName(&m->FQDN, &fqdn);

		if (m->FQDN.c[0])
			{
			mDNSPlatformDynDNSHostNameStatusChanged(&m->FQDN, 1);
			mDNS_AddDynDNSHostName(m, &m->FQDN, DynDNSHostNameCallback, mDNSNULL);
			}
		}

	mDNS_Unlock(m);

	// handle router and primary interface changes
	v4 = v6 = r = zeroAddr;
	v4.type = r.type = mDNSAddrType_IPv4;

	if (mDNSPlatformGetPrimaryInterface(m, &v4, &v6, &r) == mStatus_NoError && !mDNSv4AddressIsLinkLocal(&v4.ip.v4))
		{
		mDNS_SetPrimaryInterfaceInfo(m,
			!mDNSIPv4AddressIsZero(v4.ip.v4) ? &v4 : mDNSNULL,
			!mDNSIPv6AddressIsZero(v6.ip.v6) ? &v6 : mDNSNULL,
			!mDNSIPv4AddressIsZero(r .ip.v4) ? &r  : mDNSNULL);
		}
	else
		{
		mDNS_SetPrimaryInterfaceInfo(m, mDNSNULL, mDNSNULL, mDNSNULL);
		if (m->FQDN.c[0]) mDNSPlatformDynDNSHostNameStatusChanged(&m->FQDN, 1);	// Set status to 1 to indicate temporary failure
		}

	return mStatus_NoError;
	}

mDNSexport void mDNSCoreInitComplete(mDNS *const m, mStatus result)
	{
	m->mDNSPlatformStatus = result;
	if (m->MainCallback)
		{
		mDNS_Lock(m);
		mDNS_DropLockBeforeCallback();		// Allow client to legally make mDNS API calls from the callback
		m->MainCallback(m, mStatus_NoError);
		mDNS_ReclaimLockAfterCallback();	// Decrement mDNS_reentrancy to block mDNS API calls again
		mDNS_Unlock(m);
		}
	}

mDNSexport void mDNS_Close(mDNS *const m)
	{
	mDNSu32 rrcache_active = 0;
	mDNSu32 rrcache_totalused = 0;
	mDNSu32 slot;
	NetworkInterfaceInfo *intf;
	mDNS_Lock(m);
	
	m->mDNS_shutdown = mDNStrue;

#ifndef UNICAST_DISABLED
	uDNS_Sleep(m);
	while (m->Hostnames) mDNS_RemoveDynDNSHostName(m, &m->Hostnames->fqdn);
#endif

	rrcache_totalused = m->rrcache_totalused;
	for (slot = 0; slot < CACHE_HASH_SLOTS; slot++)
		{
		while (m->rrcache_hash[slot])
			{
			CacheGroup *cg = m->rrcache_hash[slot];
			while (cg->members)
				{
				CacheRecord *rr = cg->members;
				cg->members = cg->members->next;
				if (rr->CRActiveQuestion) rrcache_active++;
				ReleaseCacheRecord(m, rr);
				}
			cg->rrcache_tail = &cg->members;
			ReleaseCacheGroup(m, &m->rrcache_hash[slot]);
			}
		}
	debugf("mDNS_Close: RR Cache was using %ld records, %lu active", rrcache_totalused, rrcache_active);
	if (rrcache_active != m->rrcache_active)
		LogMsg("*** ERROR *** rrcache_active %lu != m->rrcache_active %lu", rrcache_active, m->rrcache_active);

	for (intf = m->HostInterfaces; intf; intf = intf->next)
		if (intf->Advertise)
			DeadvertiseInterface(m, intf);

	// Make sure there are nothing but deregistering records remaining in the list
	if (m->CurrentRecord)
		LogMsg("mDNS_Close ERROR m->CurrentRecord already set %s", ARDisplayString(m, m->CurrentRecord));

	// First we deregister any non-shared records. In particular, we want to make sure we deregister
	// any extra records added to a Service Record Set first, before we deregister its PTR record.
	m->CurrentRecord = m->ResourceRecords;
	while (m->CurrentRecord)
		{
		AuthRecord *rr = m->CurrentRecord;
		if (rr->resrec.RecordType & kDNSRecordTypeUniqueMask)
			mDNS_Deregister_internal(m, rr, mDNS_Dereg_normal);
		else
			m->CurrentRecord = rr->next;
		}

	// Now deregister any remaining records we didn't get the first time through
	while (m->CurrentRecord)
		{
		AuthRecord *rr = m->CurrentRecord;
		if (rr->resrec.RecordType != kDNSRecordTypeDeregistering)
			mDNS_Deregister_internal(m, rr, mDNS_Dereg_normal);
		else
			m->CurrentRecord = rr->next;
		}

	if (m->ResourceRecords) debugf("mDNS_Close: Sending final packets for deregistering records");
	else debugf("mDNS_Close: No deregistering records remain");

	// If any deregistering records remain, send their deregistration announcements before we exit
	if (m->mDNSPlatformStatus != mStatus_NoError) DiscardDeregistrations(m);
	else if (m->ResourceRecords) SendResponses(m);
	if (m->ResourceRecords) LogMsg("mDNS_Close failed to send goodbye for: %s", ARDisplayString(m, m->ResourceRecords));
	
	mDNS_Unlock(m);
	debugf("mDNS_Close: mDNSPlatformClose");
	mDNSPlatformClose(m);
	debugf("mDNS_Close: done");
	}
