/*
 * Copyright (c) 2002 Apple Computer, Inc. All rights reserved.
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

    File:       mDNS.c

    Contains:   Implementation of the mDNS core itself.

    Written by: Stuart Cheshire

    Version:    mDNS Core, September 2002

    Copyright:  Copyright (c) 2002 by Apple Computer, Inc., All Rights Reserved.

    Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple Computer, Inc.
                ("Apple") in consideration of your agreement to the following terms, and your
                use, installation, modification or redistribution of this Apple software
                constitutes acceptance of these terms.  If you do not agree with these terms,
                please do not use, install, modify or redistribute this Apple software.

                In consideration of your agreement to abide by the following terms, and subject
                to these terms, Apple grants you a personal, non-exclusive license, under Apple's
                copyrights in this original Apple software (the "Apple Software"), to use,
                reproduce, modify and redistribute the Apple Software, with or without
                modifications, in source and/or binary forms; provided that if you redistribute
                the Apple Software in its entirety and without modifications, you must retain
                this notice and the following text and disclaimers in all such redistributions of
                the Apple Software.  Neither the name, trademarks, service marks or logos of
                Apple Computer, Inc. may be used to endorse or promote products derived from the
                Apple Software without specific prior written permission from Apple.  Except as
                expressly stated in this notice, no other rights or licenses, express or implied,
                are granted by Apple herein, including but not limited to any patent rights that
                may be infringed by your derivative works or by other works in which the Apple
                Software may be incorporated.

                The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
                WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
                WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
                PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN
                COMBINATION WITH YOUR PRODUCTS.

                IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
                CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
                GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
                ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION AND/OR DISTRIBUTION
                OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT, TORT
                (INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN
                ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 * This code is completely 100% portable C. It does not depend on any external header files
 * from outside the mDNS project -- all the types it expects to find are defined right here.
 * 
 * The previous point is very important: This file does not depend on any external
 * header files. It should complile on *any* platform that has a C compiler, without
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
Revision 1.145  2003/05/28 20:57:44  cheshire
<rdar://problem/3271550> mDNSResponder reports "Cannot perform multi-packet
known-answer suppression ..." This is a known issue caused by a bug in the OS X 10.2
version of mDNSResponder, so for now we should suppress this warning message.

Revision 1.144  2003/05/28 18:05:12  cheshire
<rdar://problem/3009899> mDNSResponder allows invalid service registrations
Fix silly mistake: old logic allowed "TDP" and "UCP" as valid names

Revision 1.143  2003/05/28 04:31:29  cheshire
<rdar://problem/3270733> mDNSResponder not sending probes at the prescribed time

Revision 1.142  2003/05/28 03:13:07  cheshire
<rdar://problem/3009899> mDNSResponder allows invalid service registrations
Require that the transport protocol be _udp or _tcp

Revision 1.141  2003/05/28 02:19:12  cheshire
<rdar://problem/3270634> Misleading messages generated by iChat
Better fix: Only generate the log message for queries where the TC bit is set.

Revision 1.140  2003/05/28 01:55:24  cheshire
Minor change to log messages

Revision 1.139  2003/05/28 01:52:51  cheshire
<rdar://problem/3270634> Misleading messages generated by iChat

Revision 1.138  2003/05/27 22:35:00  cheshire
<rdar://problem/3270277> mDNS_RegisterInterface needs to retrigger questions

Revision 1.137  2003/05/27 20:04:33  cheshire
<rdar://problem/3269900> mDNSResponder crash in mDNS_vsnprintf()

Revision 1.136  2003/05/27 18:50:07  cheshire
<rdar://problem/3269768> mDNS_StartResolveService doesn't inform client of port number changes

Revision 1.135  2003/05/26 04:57:28  cheshire
<rdar://problem/3268953> Delay queries when there are already answers in the cache

Revision 1.134  2003/05/26 04:54:54  cheshire
<rdar://problem/3268904> sprintf/vsprintf-style functions are unsafe; use snprintf/vsnprintf instead
Accidentally deleted '%' case from the switch statement

Revision 1.133  2003/05/26 03:21:27  cheshire
Tidy up address structure naming:
mDNSIPAddr         => mDNSv4Addr (for consistency with mDNSv6Addr)
mDNSAddr.addr.ipv4 => mDNSAddr.ip.v4
mDNSAddr.addr.ipv6 => mDNSAddr.ip.v6

Revision 1.132  2003/05/26 03:01:26  cheshire
<rdar://problem/3268904> sprintf/vsprintf-style functions are unsafe; use snprintf/vsnprintf instead

Revision 1.131  2003/05/26 00:42:05  cheshire
<rdar://problem/3268876> Temporarily include mDNSResponder version in packets

Revision 1.130  2003/05/24 16:39:48  cheshire
<rdar://problem/3268631> SendResponses also needs to handle multihoming better

Revision 1.129  2003/05/23 02:15:37  cheshire
Fixed misleading use of the term "duplicate suppression" where it should have
said "known answer suppression". (Duplicate answer suppression is something
different, and duplicate question suppression is yet another thing, so the use
of the completely vague term "duplicate suppression" was particularly bad.)

Revision 1.128  2003/05/23 01:55:13  cheshire
<rdar://problem/3267127> After name change, mDNSResponder needs to re-probe for name uniqueness

Revision 1.127  2003/05/23 01:02:15  ksekar
Bug #: <rdar://problem/3032577>: mDNSResponder needs to include unique id in default name

Revision 1.126  2003/05/22 02:29:22  cheshire
<rdar://problem/2984918> SendQueries needs to handle multihoming better
Complete rewrite of SendQueries. Works much better now :-)

Revision 1.125  2003/05/22 01:50:45  cheshire
Fix warnings, and improve log messages

Revision 1.124  2003/05/22 01:41:50  cheshire
DiscardDeregistrations doesn't need InterfaceID parameter

Revision 1.123  2003/05/22 01:38:55  cheshire
Change bracketing of #pragma mark

Revision 1.122  2003/05/21 19:59:04  cheshire
<rdar://problem/3148431> ER: Tweak responder's default name conflict behavior
Minor refinements; make sure we don't truncate in the middle of a multi-byte UTF-8 character

Revision 1.121  2003/05/21 17:54:07  ksekar
Bug #: <rdar://problem/3148431>:	ER: Tweak responder's default name conflict behavior
New rename behavior - domain name "foo" becomes "foo--2" on conflict, richtext name becomes "foo (2)"

Revision 1.120  2003/05/19 22:14:14  ksekar
<rdar://problem/3162914> mDNS probe denials/conflicts not detected unless conflict is of the same type

Revision 1.119  2003/05/16 01:34:10  cheshire
Fix some warnings

Revision 1.118  2003/05/14 18:48:40  cheshire
<rdar://problem/3159272> mDNSResponder should be smarter about reconfigurations
More minor refinements:
CFSocket.c needs to do *all* its mDNS_DeregisterInterface calls before freeing memory
mDNS_DeregisterInterface revalidates cache record when *any* representative of an interface goes away

Revision 1.117  2003/05/14 07:08:36  cheshire
<rdar://problem/3159272> mDNSResponder should be smarter about reconfigurations
Previously, when there was any network configuration change, mDNSResponder
would tear down the entire list of active interfaces and start again.
That was very disruptive, and caused the entire cache to be flushed,
and caused lots of extra network traffic. Now it only removes interfaces
that have really gone, and only adds new ones that weren't there before.

Revision 1.116  2003/05/14 06:51:56  cheshire
<rdar://problem/3027144> Rendezvous doesn't refresh server info if changed during sleep

Revision 1.115  2003/05/14 06:44:31  cheshire
Improve debugging message

Revision 1.114  2003/05/07 01:47:03  cheshire
<rdar://problem/3250330> Also protect against NULL domainlabels

Revision 1.113  2003/05/07 00:28:18  cheshire
<rdar://problem/3250330> Need to make mDNSResponder more defensive against bad clients

Revision 1.112  2003/05/06 00:00:46  cheshire
<rdar://problem/3248914> Rationalize naming of domainname manipulation functions

Revision 1.111  2003/05/05 23:42:08  cheshire
<rdar://problem/3245631> Resolves never succeed
Was setting "rr->LastAPTime = timenow - rr->LastAPTime"
instead of  "rr->LastAPTime = timenow - rr->ThisAPInterval"

Revision 1.110  2003/04/30 21:09:59  cheshire
<rdar://problem/3244727> mDNS_vsnprintf needs to be more defensive against invalid domain names

Revision 1.109  2003/04/26 02:41:56  cheshire
<rdar://problem/3241281> Change timenow from a local variable to a structure member

Revision 1.108  2003/04/25 01:45:56  cheshire
<rdar://problem/3240002> mDNS_RegisterNoSuchService needs to include a host name

Revision 1.107  2003/04/25 00:41:31  cheshire
<rdar://problem/3239912> Create single routine PurgeCacheResourceRecord(), to avoid bugs in future

Revision 1.106  2003/04/22 03:14:45  cheshire
<rdar://problem/3232229> Include Include instrumented mDNSResponder in panther now

Revision 1.105  2003/04/22 01:07:43  cheshire
<rdar://problem/3176248> DNSServiceRegistrationUpdateRecord should support a default ttl
If TTL parameter is zero, leave record TTL unchanged

Revision 1.104  2003/04/21 19:15:52  cheshire
Fix some compiler warnings

Revision 1.103  2003/04/19 02:26:35  cheshire
Bug #: <rdar://problem/3233804> Incorrect goodbye packet after conflict

Revision 1.102  2003/04/17 03:06:28  cheshire
Bug #: <rdar://problem/3231321> No need to query again when a service goes away
Set UnansweredQueries to 2 when receiving a "goodbye" packet

Revision 1.101  2003/04/15 20:58:31  jgraessl
Bug #: 3229014
Added a hash to lookup records in the cache.

Revision 1.100  2003/04/15 18:53:14  cheshire
Bug #: <rdar://problem/3229064> Bug in ScheduleNextTask
mDNS.c 1.94 incorrectly combined two "if" statements into one.

Revision 1.99  2003/04/15 18:09:13  jgraessl
Bug #: 3228892
Reviewed by: Stuart Cheshire
Added code to keep track of when the next cache item will expire so we can
call TidyRRCache only when necessary.

Revision 1.98  2003/04/03 03:43:55  cheshire
<rdar://problem/3216837>:	Off-by-one error in probe rate limiting

Revision 1.97  2003/04/02 01:48:17  cheshire
<rdar://problem/3212360> mDNSResponder sometimes suffers false self-conflicts when it sees its own packets
Additional fix pointed out by Josh:
Also set ProbeFailTime when incrementing NumFailedProbes when resetting a record back to probing state

Revision 1.96  2003/04/01 23:58:55  cheshire
Minor comment changes

Revision 1.95  2003/04/01 23:46:05  cheshire
<rdar://problem/3214832> mDNSResponder can get stuck in infinite loop after many location cycles
mDNS_DeregisterInterface() flushes the RR cache by marking all records received on that interface
to expire in one second. However, if a mDNS_StartResolveService() call is made in that one-second
window, it can get an SRV answer from one of those soon-to-be-deleted records, resulting in
FoundServiceInfoSRV() making an interface-specific query on the interface that was just removed.

Revision 1.94  2003/03/29 01:55:19  cheshire
<rdar://problem/3212360> mDNSResponder sometimes suffers false self-conflicts when it sees its own packets
Solution: Major cleanup of packet timing and conflict handling rules

Revision 1.93  2003/03/28 01:54:36  cheshire
Minor tidyup of IPv6 (AAAA) code

Revision 1.92  2003/03/27 03:30:55  cheshire
<rdar://problem/3210018> Name conflicts not handled properly, resulting in memory corruption, and eventual crash
Problem was that HostNameCallback() was calling mDNS_DeregisterInterface(), which is not safe in a callback
Fixes:
1. Make mDNS_DeregisterInterface() safe to call from a callback
2. Make HostNameCallback() use mDNS_DeadvertiseInterface() instead
   (it never really needed to deregister the interface at all)

Revision 1.91  2003/03/15 04:40:36  cheshire
Change type called "mDNSOpaqueID" to the more descriptive name "mDNSInterfaceID"

Revision 1.90  2003/03/14 20:26:37  cheshire
Reduce debugging messages (reclassify some "debugf" as "verbosedebugf")

Revision 1.89  2003/03/12 19:57:50  cheshire
Fixed typo in debug message

Revision 1.88  2003/03/12 00:17:44  cheshire
<rdar://problem/3195426> GetFreeCacheRR needs to be more willing to throw away recent records

Revision 1.87  2003/03/11 01:27:20  cheshire
Reduce debugging messages (reclassify some "debugf" as "verbosedebugf")

Revision 1.86  2003/03/06 20:44:33  cheshire
Comment tidyup

Revision 1.85  2003/03/05 03:38:35  cheshire
Bug #: 3185731 Bogus error message in console: died or deallocated, but no record of client can be found!
Fixed by leaving client in list after conflict, until client explicitly deallocates

Revision 1.84  2003/03/05 01:27:30  cheshire
Bug #: 3185482 Different TTL for multicast versus unicast replies
When building unicast replies, record TTLs are capped to 10 seconds

Revision 1.83  2003/03/04 23:48:52  cheshire
Bug #: 3188865 Double probes after wake from sleep
Don't reset record type to kDNSRecordTypeUnique if record is DependentOn another

Revision 1.82  2003/03/04 23:38:29  cheshire
Bug #: 3099194 mDNSResponder needs performance improvements
Only set rr->CRActiveQuestion to point to the
currently active representative of a question set

Revision 1.81  2003/02/21 03:35:34  cheshire
Bug #: 3179007 mDNSResponder needs to include AAAA records in additional answer section

Revision 1.80  2003/02/21 02:47:53  cheshire
Bug #: 3099194 mDNSResponder needs performance improvements
Several places in the code were calling CacheRRActive(), which searched the entire
question list every time, to see if this cache resource record answers any question.
Instead, we now have a field "CRActiveQuestion" in the resource record structure

Revision 1.79  2003/02/21 01:54:07  cheshire
Bug #: 3099194 mDNSResponder needs performance improvements
Switched to using new "mDNS_Execute" model (see "Implementer Notes.txt")

Revision 1.78  2003/02/20 06:48:32  cheshire
Bug #: 3169535 Xserve RAID needs to do interface-specific registrations
Reviewed by: Josh Graessley, Bob Bradley

Revision 1.77  2003/01/31 03:35:59  cheshire
Bug #: 3147097 mDNSResponder sometimes fails to find the correct results
When there were *two* active questions in the list, they were incorrectly
finding *each other* and *both* being marked as duplicates of another question

Revision 1.76  2003/01/29 02:46:37  cheshire
Fix for IPv6:
A physical interface is identified solely by its InterfaceID (not by IP and type).
On a given InterfaceID, mDNSCore may send both v4 and v6 multicasts.
In cases where the requested outbound protocol (v4 or v6) is not supported on
that InterfaceID, the platform support layer should simply discard that packet.

Revision 1.75  2003/01/29 01:47:40  cheshire
Rename 'Active' to 'CRActive' or 'InterfaceActive' for improved clarity

Revision 1.74  2003/01/28 05:26:25  cheshire
Bug #: 3147097 mDNSResponder sometimes fails to find the correct results
Add 'Active' flag for interfaces

Revision 1.73  2003/01/28 03:45:12  cheshire
Fixed missing "not" in "!mDNSAddrIsDNSMulticast(dstaddr)"

Revision 1.72  2003/01/28 01:49:48  cheshire
Bug #: 3147097 mDNSResponder sometimes fails to find the correct results
FindDuplicateQuestion() was incorrectly finding the question itself in the list,
and incorrectly marking it as a duplicate (of itself), so that it became inactive.

Revision 1.71  2003/01/28 01:41:44  cheshire
Bug #: 3153091 Race condition when network change causes bad stuff
When an interface goes away, interface-specific questions on that interface become orphaned.
Orphan questions cause HaveQueries to return true, but there's no interface to send them on.
Fix: mDNS_DeregisterInterface() now calls DeActivateInterfaceQuestions()

Revision 1.70  2003/01/23 19:00:20  cheshire
Protect against infinite loops in mDNS_Execute

Revision 1.69  2003/01/21 22:56:32  jgraessl
Bug #: 3124348  service name changes are not properly handled
Submitted by: Stuart Cheshire
Reviewed by: Joshua Graessley
Applying changes for 3124348 to main branch. 3124348 changes went in to a
branch for SU.

Revision 1.68  2003/01/17 04:09:27  cheshire
Bug #: 3141038 mDNSResponder Resolves are unreliable on multi-homed hosts

Revision 1.67  2003/01/17 03:56:45  cheshire
Default 24-hour TTL is far too long. Changing to two hours.

Revision 1.66  2003/01/13 23:49:41  jgraessl
Merged changes for the following fixes in to top of tree:
3086540  computer name changes not handled properly
3124348  service name changes are not properly handled
3124352  announcements sent in pairs, failing chattiness test

Revision 1.65  2002/12/23 22:13:28  jgraessl
Reviewed by: Stuart Cheshire
Initial IPv6 support for mDNSResponder.

Revision 1.64  2002/11/26 20:49:06  cheshire
Bug #: 3104543 RFC 1123 allows the first character of a name label to be either a letter or a digit

Revision 1.63  2002/09/21 20:44:49  zarzycki
Added APSL info

Revision 1.62  2002/09/20 03:25:37  cheshire
Fix some compiler warnings

Revision 1.61  2002/09/20 01:05:24  cheshire
Don't kill the Extras list in mDNS_DeregisterService()

Revision 1.60  2002/09/19 23:47:35  cheshire
Added mDNS_RegisterNoSuchService() function for assertion of non-existence
of a particular named service

Revision 1.59  2002/09/19 21:25:34  cheshire
mDNS_snprintf() doesn't need to be in a separate file

Revision 1.58  2002/09/19 04:20:43  cheshire
Remove high-ascii characters that confuse some systems

Revision 1.57  2002/09/17 01:07:08  cheshire
Change mDNS_AdvertiseLocalAddresses to be a parameter to mDNS_Init()

Revision 1.56  2002/09/16 19:44:17  cheshire
Merge in license terms from Quinn's copy, in preparation for Darwin release
*/

#include "mDNSClientAPI.h"				// Defines the interface provided to the client layer above
#include "mDNSPlatformFunctions.h"		// Defines the interface required of the supporting layer below

#if(defined(_MSC_VER))
	// Disable warnings about Microsoft Visual Studio/C++ not understanding "pragma unused"
    #pragma warning( disable:4068 )
#endif

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - DNS Protocol Constants
#endif

typedef enum
	{
	kDNSFlag0_QR_Mask     = 0x80,		// Query or response?
	kDNSFlag0_QR_Query    = 0x00,
	kDNSFlag0_QR_Response = 0x80,
	
	kDNSFlag0_OP_Mask     = 0x78,		// Operation type
	kDNSFlag0_OP_StdQuery = 0x00,
	kDNSFlag0_OP_Iquery   = 0x08,
	kDNSFlag0_OP_Status   = 0x10,
	kDNSFlag0_OP_Unused3  = 0x18,
	kDNSFlag0_OP_Notify   = 0x20,
	kDNSFlag0_OP_Update   = 0x28,
	
	kDNSFlag0_QROP_Mask   = kDNSFlag0_QR_Mask | kDNSFlag0_OP_Mask,
	
	kDNSFlag0_AA          = 0x04,		// Authoritative Answer?
	kDNSFlag0_TC          = 0x02,		// Truncated?
	kDNSFlag0_RD          = 0x01,		// Recursion Desired?
	kDNSFlag1_RA          = 0x80,		// Recursion Available?
	
	kDNSFlag1_Zero        = 0x40,		// Reserved; must be zero
	kDNSFlag1_AD          = 0x20,		// Authentic Data [RFC 2535]
	kDNSFlag1_CD          = 0x10,		// Checking Disabled [RFC 2535]

	kDNSFlag1_RC          = 0x0F,		// Response code
	kDNSFlag1_RC_NoErr    = 0x00,
	kDNSFlag1_RC_FmtErr   = 0x01,
	kDNSFlag1_RC_SrvErr   = 0x02,
	kDNSFlag1_RC_NXDomain = 0x03,
	kDNSFlag1_RC_NotImpl  = 0x04,
	kDNSFlag1_RC_Refused  = 0x05,
	kDNSFlag1_RC_YXDomain = 0x06,
	kDNSFlag1_RC_YXRRSet  = 0x07,
	kDNSFlag1_RC_NXRRSet  = 0x08,
	kDNSFlag1_RC_NotAuth  = 0x09,
	kDNSFlag1_RC_NotZone  = 0x0A
	} DNS_Flags;

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - Program Constants
#endif

mDNSexport const ResourceRecord  zeroRR;
mDNSexport const mDNSIPPort      zeroIPPort        = { { 0 } };
mDNSexport const mDNSv4Addr      zeroIPAddr        = { { 0 } };
mDNSexport const mDNSv6Addr      zerov6Addr        = { { 0 } };
mDNSexport const mDNSv4Addr      onesIPv4Addr      = { { 255, 255, 255, 255 } };
mDNSexport const mDNSv6Addr      onesIPv6Addr      = { { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 } };
mDNSlocal  const mDNSAddr	     zeroAddr          = { mDNSAddrType_None, {{{ 0 }}} };

mDNSexport const mDNSInterfaceID mDNSInterface_Any = { 0 };
mDNSlocal  const mDNSInterfaceID mDNSInterfaceMark = { (mDNSInterfaceID)~0 };

#define UnicastDNSPortAsNumber 53
#define MulticastDNSPortAsNumber 5353
mDNSexport const mDNSIPPort UnicastDNSPort     = { { UnicastDNSPortAsNumber   >> 8, UnicastDNSPortAsNumber   & 0xFF } };
mDNSexport const mDNSIPPort MulticastDNSPort   = { { MulticastDNSPortAsNumber >> 8, MulticastDNSPortAsNumber & 0xFF } };
mDNSexport const mDNSv4Addr AllDNSAdminGroup   = { { 239, 255, 255, 251 } };
mDNSexport const mDNSv4Addr	AllDNSLinkGroup    = { { 224,   0,   0, 251 } };
mDNSexport const mDNSv6Addr AllDNSLinkGroupv6  = { { 0xFF,0x02,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0xFB } };
mDNSexport const mDNSAddr   AllDNSLinkGroup_v4 = { mDNSAddrType_IPv4, { { { 224,   0,   0, 251 } } } };
mDNSexport const mDNSAddr   AllDNSLinkGroup_v6 = { mDNSAddrType_IPv6, { { { 0xFF,0x02,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0xFB } } } };

static const mDNSOpaque16 QueryFlags    = { { kDNSFlag0_QR_Query    | kDNSFlag0_OP_StdQuery,                0 } };
static const mDNSOpaque16 ResponseFlags = { { kDNSFlag0_QR_Response | kDNSFlag0_OP_StdQuery | kDNSFlag0_AA, 0 } };
#define zeroDomainNamePtr ((domainname*)"")

static const mDNSOpaque16 zeroID = { { 0, 0 } };
#ifdef mDNSResponderVersion
static const mDNSOpaque16 mDNS_MessageID = { { mDNSResponderVersion >> 8, mDNSResponderVersion & 0xFF } };
#else
#define mDNS_MessageID zeroID
#endif

static const char *const mDNS_DomainTypeNames[] =
	{
	"_browse._mdns._udp.local.",
	"_default._browse._mdns._udp.local.",
	"_register._mdns._udp.local.",
	"_default._register._mdns._udp.local."
	};

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - Specialized mDNS version of vsnprintf
#endif

static const struct mDNSprintf_format
	{
	unsigned 		leftJustify : 1;
	unsigned 		forceSign : 1;
	unsigned 		zeroPad : 1;
	unsigned 		havePrecision : 1;
	unsigned 		hSize : 1;
	unsigned 		lSize : 1;
	char	 		altForm;
	char			sign;		// +, - or space
	unsigned int	fieldWidth;
	unsigned int	precision;
	} mDNSprintf_format_default;

mDNSexport mDNSu32 mDNS_vsnprintf(char *sbuffer, mDNSu32 buflen, const char *fmt, va_list arg)
	{
	mDNSu32 nwritten = 0;
	int c;
	buflen--;		// Pre-reserve one space in the buffer for the terminating nul

	for (c = *fmt; c != 0 ; c = *++fmt)
		{
		if (c != '%')
			{
			*sbuffer++ = (char)c;
			if (++nwritten >= buflen) goto exit;
			}
		else
			{
			unsigned int i=0, j;
			// The mDNS Vsprintf Argument Conversion Buffer is used as a temporary holding area for
			// generating decimal numbers, hexdecimal numbers, IP addresses, domain name strings, etc.
			// The size needs to be enough for a 256-byte domain name plus some error text.
			#define mDNS_VACB_Size 300
			char mDNS_VACB[mDNS_VACB_Size];
			#define mDNS_VACB_Lim (&mDNS_VACB[mDNS_VACB_Size])
			#define mDNS_VACB_Remain(s) ((mDNSu32)(mDNS_VACB_Lim - s))
			char *s = mDNS_VACB_Lim, *digits;
			struct mDNSprintf_format F = mDNSprintf_format_default;
	
			while(1)	//  decode flags
				{
				c = *++fmt;
				if      (c == '-')	F.leftJustify = 1;
				else if (c == '+')	F.forceSign = 1;
				else if (c == ' ')	F.sign = ' ';
				else if (c == '#')	F.altForm++;
				else if (c == '0')	F.zeroPad = 1;
				else break;
				}
	
			if (c == '*')	//  decode field width
				{
				int f = va_arg(arg, int);
				if (f < 0) { f = -f; F.leftJustify = 1; }
				F.fieldWidth = (unsigned int)f;
				c = *++fmt;
				}
			else
				{
				for (; c >= '0' && c <= '9'; c = *++fmt)
					F.fieldWidth = (10 * F.fieldWidth) + (c - '0');
				}
	
			if (c == '.')	//  decode precision
				{
				if ((c = *++fmt) == '*')
					{ F.precision = va_arg(arg, unsigned int); c = *++fmt; }
				else for (; c >= '0' && c <= '9'; c = *++fmt)
						F.precision = (10 * F.precision) + (c - '0');
				F.havePrecision = 1;
				}
	
			if (F.leftJustify) F.zeroPad = 0;
	
			conv:
			switch (c)	//  perform appropriate conversion
				{
				unsigned long n;
				case 'h' :	F.hSize = 1; c = *++fmt; goto conv;
				case 'l' :	// fall through
				case 'L' :	F.lSize = 1; c = *++fmt; goto conv;
				case 'd' :
				case 'i' :	if (F.lSize) n = (unsigned long)va_arg(arg, long);
							else n = (unsigned long)va_arg(arg, int);
							if (F.hSize) n = (short) n;
							if ((long) n < 0) { n = (unsigned long)-(long)n; F.sign = '-'; }
							else if (F.forceSign) F.sign = '+';
							goto decimal;
				case 'u' :	if (F.lSize) n = va_arg(arg, unsigned long);
							else n = va_arg(arg, unsigned int);
							if (F.hSize) n = (unsigned short) n;
							F.sign = 0;
							goto decimal;
				decimal:	if (!F.havePrecision)
								{
								if (F.zeroPad)
									{
									F.precision = F.fieldWidth;
									if (F.sign) --F.precision;
									}
								if (F.precision < 1) F.precision = 1;
								}
							if (F.precision > mDNS_VACB_Size - 1)
								F.precision = mDNS_VACB_Size - 1;
							for (i = 0; n; n /= 10, i++) *--s = (char)(n % 10 + '0');
							for (; i < F.precision; i++) *--s = '0';
							if (F.sign) { *--s = F.sign; i++; }
							break;
	
				case 'o' :	if (F.lSize) n = va_arg(arg, unsigned long);
							else n = va_arg(arg, unsigned int);
							if (F.hSize) n = (unsigned short) n;
							if (!F.havePrecision)
								{
								if (F.zeroPad) F.precision = F.fieldWidth;
								if (F.precision < 1) F.precision = 1;
								}
							if (F.precision > mDNS_VACB_Size - 1)
								F.precision = mDNS_VACB_Size - 1;
							for (i = 0; n; n /= 8, i++) *--s = (char)(n % 8 + '0');
							if (F.altForm && i && *s != '0') { *--s = '0'; i++; }
							for (; i < F.precision; i++) *--s = '0';
							break;
	
				case 'a' :	{
							unsigned char *a = va_arg(arg, unsigned char *);
							if (!a) { static char emsg[] = "<<NULL>>"; s = emsg; i = sizeof(emsg)-1; }
							else
								{
								unsigned short *w = (unsigned short *)a;
								s = mDNS_VACB;	// Adjust s to point to the start of the buffer, not the end
								if (F.altForm)
									{
									mDNSAddr *ip = (mDNSAddr*)a;
									a = (unsigned char  *)&ip->ip.v4;
									w = (unsigned short *)&ip->ip.v6;
									switch (ip->type)
										{
										case mDNSAddrType_IPv4: F.precision =  4; break;
										case mDNSAddrType_IPv6: F.precision = 16; break;
										default:                F.precision =  0; break;
										}
									}
								switch (F.precision)
									{
									case  4: i = mDNS_snprintf(mDNS_VACB, sizeof(mDNS_VACB), "%d.%d.%d.%d",
														a[0], a[1], a[2], a[3]); break;
									case  6: i = mDNS_snprintf(mDNS_VACB, sizeof(mDNS_VACB), "%02X:%02X:%02X:%02X:%02X:%02X",
														a[0], a[1], a[2], a[3], a[4], a[5]); break;
									case 16: i = mDNS_snprintf(mDNS_VACB, sizeof(mDNS_VACB), "%04X:%04X:%04X:%04X:%04X:%04X:%04X:%04X",
														w[0], w[1], w[2], w[3], w[4], w[5], w[6], w[7]); break;
									default: i = mDNS_snprintf(mDNS_VACB, sizeof(mDNS_VACB), "%s", "<< ERROR: Must specify address size "
														"(i.e. %.4a=IPv4, %.6a=Ethernet, %.16a=IPv6) >>"); break;
									}
								}
							}
							break;
	
				case 'p' :	F.havePrecision = F.lSize = 1;
							F.precision = 8;
				case 'X' :	digits = "0123456789ABCDEF";
							goto hexadecimal;
				case 'x' :	digits = "0123456789abcdef";
				hexadecimal:if (F.lSize) n = va_arg(arg, unsigned long);
							else n = va_arg(arg, unsigned int);
							if (F.hSize) n = (unsigned short) n;
							if (!F.havePrecision)
								{
								if (F.zeroPad)
									{
									F.precision = F.fieldWidth;
									if (F.altForm) F.precision -= 2;
									}
								if (F.precision < 1) F.precision = 1;
								}
							if (F.precision > mDNS_VACB_Size - 1)
								F.precision = mDNS_VACB_Size - 1;
							for (i = 0; n; n /= 16, i++) *--s = digits[n % 16];
							for (; i < F.precision; i++) *--s = '0';
							if (F.altForm) { *--s = (char)c; *--s = '0'; i += 2; }
							break;
	
				case 'c' :	*--s = (char)va_arg(arg, int); i = 1; break;
	
				case 's' :	s = va_arg(arg, char *);
							if (!s) { static char emsg[] = "<<NULL>>"; s = emsg; i = sizeof(emsg)-1; }
							else switch (F.altForm)
								{
								case 0: { char *a=s; i=0; while(*a++) i++; break; }	// C string
								case 1: i = (unsigned char) *s++; break;	// Pascal string
								case 2: {									// DNS label-sequence name
										unsigned char *a = (unsigned char *)s;
										s = mDNS_VACB;	// Adjust s to point to the start of the buffer, not the end
										if (*a == 0) *s++ = '.';	// Special case for root DNS name
										while (*a)
											{
											if (*a > 63) { s += mDNS_snprintf(s, mDNS_VACB_Remain(s), "<<INVALID LABEL LENGTH %u>>", *a); break; }
											if (s + *a >= &mDNS_VACB[254]) { s += mDNS_snprintf(s, mDNS_VACB_Remain(s), "<<NAME TOO LONG>>"); break; }
											s += mDNS_snprintf(s, mDNS_VACB_Remain(s), "%#s.", a);
											a += 1 + *a;
											}
										i = (mDNSu32)(s - mDNS_VACB);
										s = mDNS_VACB;	// Reset s back to the start of the buffer
										break;
										}
								}
							if (F.havePrecision && i > F.precision) i = F.precision;
							break;
	
				case 'n' :	s = va_arg(arg, char *);
							if      (F.hSize) * (short *) s = (short)nwritten;
							else if (F.lSize) * (long  *) s = (long)nwritten;
							else              * (int   *) s = (int)nwritten;
							continue;
	
				default:	s = mDNS_VACB;
							i = mDNS_snprintf(mDNS_VACB, sizeof(mDNS_VACB), "<<UNKNOWN FORMAT CONVERSION CODE %%%c>>", c);

				case '%' :	*sbuffer++ = (char)c;
							if (++nwritten >= buflen) goto exit;
							break;
				}
	
			if (i < F.fieldWidth && !F.leftJustify)			// Pad on the left
				do	{
					*sbuffer++ = ' ';
					if (++nwritten >= buflen) goto exit;
					} while (i < --F.fieldWidth);
	
			if (i > buflen - nwritten) i = buflen - nwritten;
			for (j=0; j<i; j++) *sbuffer++ = *s++;			// Write the converted result
			nwritten += i;
			if (nwritten >= buflen) goto exit;
	
			for (; i < F.fieldWidth; i++)					// Pad on the right
				{
				*sbuffer++ = ' ';
				if (++nwritten >= buflen) goto exit;
				}
			}
		}
	exit:
	*sbuffer++ = 0;
	return(nwritten);
	}

mDNSexport mDNSu32 mDNS_snprintf(char *sbuffer, mDNSu32 buflen, const char *fmt, ...)
	{
	mDNSu32 length;
	
    va_list ptr;
	va_start(ptr,fmt);
	length = mDNS_vsnprintf(sbuffer, buflen, fmt, ptr);
	va_end(ptr);
	
	return(length);
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - General Utility Functions
#endif

// We define DNSTypeName this way because if we declare it as a static (local) function
// then some compilers give "static function defined but not used" warnings in non-debug builds.
// Conditionalizing it with "#if MDNS_DEBUGMSGS" so that it is not present in non-debug builds
// fixes the warning on those compilers, but doesn't work on other compilers that don't strip
// unreachable code, which then give "undefined function" errors.
extern char *DNSTypeName(mDNSu16 rrtype);
char *DNSTypeName(mDNSu16 rrtype)
	{
	switch (rrtype)
		{
		case kDNSType_A:	return("Addr");
		case kDNSType_CNAME:return("CNAME");
		case kDNSType_PTR:	return("PTR");
		case kDNSType_HINFO:return("HINFO");
		case kDNSType_TXT:  return("TXT");
		case kDNSType_AAAA:	return("AAAA");
		case kDNSType_SRV:	return("SRV");
		default:			{
							static char buffer[16];
							mDNS_snprintf(buffer, sizeof(buffer), "(%d)", rrtype);
							return(buffer);
							}
		}
	}

mDNSlocal mDNSu32 mDNSRandom(mDNSu32 max)
	{
	static mDNSu32 seed = 0;
	mDNSu32 mask = 1;
	
	if (!seed) seed = (mDNSu32)mDNSPlatformTimeNow();
	while (mask < max) mask = (mask << 1) | 1;
	do seed = seed * 21 + 1; while ((seed & mask) > max);
	return (seed & mask);
	}

#define mDNSSameIPv4Address(A,B) ((A).NotAnInteger == (B).NotAnInteger)
#define mDNSSameIPv6Address(A,B) ((A).l[0] == (B).l[0] && (A).l[1] == (B).l[1] && (A).l[2] == (B).l[2] && (A).l[3] == (B).l[3])
#define mDNSIPv4AddressIsZero(A) mDNSSameIPv4Address((A), zeroIPAddr)
#define mDNSIPv6AddressIsZero(A) mDNSSameIPv6Address((A), zerov6Addr)

mDNSexport mDNSBool mDNSSameAddress(const mDNSAddr *ip1, const mDNSAddr *ip2)
	{
	if (ip1->type == ip2->type)
		{
		switch (ip1->type)
			{
			case mDNSAddrType_IPv4 : return(mDNSSameIPv4Address(ip1->ip.v4, ip2->ip.v4));
			case mDNSAddrType_IPv6 : return(mDNSSameIPv6Address(ip1->ip.v6, ip2->ip.v6));
			}
		}
	return(mDNSfalse);
	}

mDNSlocal mDNSBool mDNSAddrIsDNSMulticast(const mDNSAddr *ip)
	{
	mDNSBool result = mDNSfalse;
	switch(ip->type)
		{
		case mDNSAddrType_IPv4:
			result = ip->ip.v4.NotAnInteger == AllDNSLinkGroup.NotAnInteger ? mDNStrue : mDNSfalse;
			break;
		
		case mDNSAddrType_IPv6:
			if (ip->ip.v6.l[0] == AllDNSLinkGroupv6.l[0] &&
				ip->ip.v6.l[1] == AllDNSLinkGroupv6.l[1] &&
				ip->ip.v6.l[2] == AllDNSLinkGroupv6.l[2] &&
				ip->ip.v6.l[3] == AllDNSLinkGroupv6.l[3])
				result = mDNStrue;
			else result = mDNSfalse;
			break;
		}
	
	return result;
	}

mDNSlocal const NetworkInterfaceInfo *GetFirstActiveInterface(const NetworkInterfaceInfo *intf)
	{
	while (intf && !intf->InterfaceActive) intf = intf->next;
	return(intf);
	}

mDNSlocal mDNSInterfaceID GetNextActiveInterfaceID(const NetworkInterfaceInfo *intf)
	{
	const NetworkInterfaceInfo *next = GetFirstActiveInterface(intf->next);
	if (next) return(next->InterfaceID); else return(mDNSNULL);
	}

#define ActiveQuestion(Q) ((Q)->ThisQInterval > 0 && !(Q)->DuplicateOf)
#define TimeToSendThisQuestion(Q,time) (ActiveQuestion(Q) && (time) - ((Q)->LastQTime + (Q)->ThisQInterval) >= 0)

mDNSlocal void SetNextQueryTime(mDNS *const m, const DNSQuestion *const q)
	{
	if (ActiveQuestion(q))
		if (m->NextScheduledQuery - (q->LastQTime + q->ThisQInterval) > 0)
			m->NextScheduledQuery = (q->LastQTime + q->ThisQInterval);
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - Domain Name Utility Functions
#endif

#define mdnsIsLetter(X) (((X) >= 'A' && (X) <= 'Z') || ((X) >= 'a' && (X) <= 'z'))
#define mdnsIsDigit(X) (((X) >= '0' && (X) <= '9'))

mDNSexport mDNSBool SameDomainLabel(const mDNSu8 *a, const mDNSu8 *b)
	{
	int i;
	const int len = *a++;

	if (len > MAX_DOMAIN_LABEL)
		{ debugf("Malformed label (too long)"); return(mDNSfalse); }

	if (len != *b++) return(mDNSfalse);
	for (i=0; i<len; i++)
		{
		mDNSu8 ac = *a++;
		mDNSu8 bc = *b++;
		if (ac >= 'A' && ac <= 'Z') ac += 'a' - 'A';
		if (bc >= 'A' && bc <= 'Z') bc += 'a' - 'A';
		if (ac != bc) return(mDNSfalse);
		}
	return(mDNStrue);
	}

mDNSexport mDNSBool SameDomainName(const domainname *const d1, const domainname *const d2)
	{
	const mDNSu8 *      a   = d1->c;
	const mDNSu8 *      b   = d2->c;
	const mDNSu8 *const max = d1->c + MAX_DOMAIN_NAME;			// Maximum that's valid

	while (*a || *b)
		{
		if (a + 1 + *a >= max)
			{ debugf("Malformed domain name (more than 255 characters)"); return(mDNSfalse); }
		if (!SameDomainLabel(a, b)) return(mDNSfalse);
		a += 1 + *a;
		b += 1 + *b;
		}
	
	return(mDNStrue);
	}

// Returns length of a domain name INCLUDING the byte for the final null label
// i.e. for the root label "." it returns one
// For the FQDN "com." it returns 5 (length byte, three data bytes, final zero)
// Legal results are 1 (just root label) to 255 (MAX_DOMAIN_NAME)
// If the given domainname is invalid, result is 256
mDNSexport mDNSu16 DomainNameLength(const domainname *const name)
	{
	const mDNSu8 *src = name->c;
	while (*src)
		{
		if (*src > MAX_DOMAIN_LABEL) return(MAX_DOMAIN_NAME+1);
		src += 1 + *src;
		if (src - name->c >= MAX_DOMAIN_NAME) return(MAX_DOMAIN_NAME+1);
		}
	return((mDNSu16)(src - name->c + 1));
	}

// CompressedDomainNameLength returns the length of a domain name INCLUDING the byte
// for the final null label i.e. for the root label "." it returns one.
// E.g. for the FQDN "foo.com." it returns 9
// (length, three data bytes, length, three more data bytes, final zero).
// In the case where a parent domain name is provided, and the given name is a child
// of that parent, CompressedDomainNameLength returns the length of the prefix portion
// of the child name, plus TWO bytes for the compression pointer.
// E.g. for the name "foo.com." with parent "com.", it returns 6
// (length, three data bytes, two-byte compression pointer).
mDNSlocal mDNSu16 CompressedDomainNameLength(const domainname *const name, const domainname *parent)
	{
	const mDNSu8 *src = name->c;
	if (parent && parent->c[0] == 0) parent = mDNSNULL;
	while (*src)
		{
		if (*src > MAX_DOMAIN_LABEL) return(MAX_DOMAIN_NAME+1);
		if (parent && SameDomainName((domainname *)src, parent)) return((mDNSu16)(src - name->c + 2));
		src += 1 + *src;
		if (src - name->c >= MAX_DOMAIN_NAME) return(MAX_DOMAIN_NAME+1);
		}
	return((mDNSu16)(src - name->c + 1));
	}

// AppendLiteralLabelString appends a single label to an existing (possibly empty) domainname.
// The C string contains the label as-is, with no escaping, etc.
// Any dots in the name are literal dots, not label separators
// If successful, AppendLiteralLabelString returns a pointer to the next unused byte
// in the domainname bufer (i.e., the next byte after the terminating zero).
// If unable to construct a legal domain name (i.e. label more than 63 bytes, or total more than 255 bytes)
// AppendLiteralLabelString returns mDNSNULL.
mDNSexport mDNSu8 *AppendLiteralLabelString(domainname *const name, const char *cstr)
	{
	mDNSu8       *      ptr  = name->c + DomainNameLength(name) - 1;	// Find end of current name
	const mDNSu8 *const lim1 = name->c + MAX_DOMAIN_NAME - 1;			// Limit of how much we can add (not counting final zero)
	const mDNSu8 *const lim2 = ptr + 1 + MAX_DOMAIN_LABEL;
	const mDNSu8 *const lim  = (lim1 < lim2) ? lim1 : lim2;
	mDNSu8       *lengthbyte = ptr++;									// Record where the length is going to go
	
	while (*cstr && ptr < lim) *ptr++ = (mDNSu8)*cstr++;	// Copy the data
	*lengthbyte = (mDNSu8)(ptr - lengthbyte - 1);			// Fill in the length byte
	*ptr++ = 0;												// Put the null root label on the end
	if (*cstr) return(mDNSNULL);							// Failure: We didn't successfully consume all input
	else return(ptr);										// Success: return new value of ptr
	}

// AppendDNSNameString appends zero or more labels to an existing (possibly empty) domainname.
// The C string is in conventional DNS syntax:
// Textual labels, escaped as necessary using the usual DNS '\' notation, separated by dots.
// If successful, AppendDNSNameString returns a pointer to the next unused byte
// in the domainname bufer (i.e., the next byte after the terminating zero).
// If unable to construct a legal domain name (i.e. label more than 63 bytes, or total more than 255 bytes)
// AppendDNSNameString returns mDNSNULL.
mDNSexport mDNSu8 *AppendDNSNameString(domainname *const name, const char *cstr)
	{
	mDNSu8       *      ptr = name->c + DomainNameLength(name) - 1;	// Find end of current name
	const mDNSu8 *const lim = name->c + MAX_DOMAIN_NAME - 1;		// Limit of how much we can add (not counting final zero)
	while (*cstr && ptr < lim)										// While more characters, and space to put them...
		{
		mDNSu8 *lengthbyte = ptr++;									// Record where the length is going to go
		while (*cstr && *cstr != '.' && ptr < lim)					// While we have characters in the label...
			{
			mDNSu8 c = (mDNSu8)*cstr++;								// Read the character
			if (c == '\\')											// If escape character, check next character
				{
				if (*cstr == '\\' || *cstr == '.')					// If a second escape, or a dot,
					c = (mDNSu8)*cstr++;							// just use the second character
				else if (mdnsIsDigit(cstr[0]) && mdnsIsDigit(cstr[1]) && mdnsIsDigit(cstr[2]))
					{												// else, if three decimal digits,
					int v0 = cstr[0] - '0';							// then interpret as three-digit decimal
					int v1 = cstr[1] - '0';
					int v2 = cstr[2] - '0';
					int val = v0 * 100 + v1 * 10 + v2;
					if (val <= 255) { c = (mDNSu8)val; cstr += 3; }	// If valid value, use it
					}
				}
			*ptr++ = c;												// Write the character
			}
		if (*cstr) cstr++;											// Skip over the trailing dot (if present)
		if (ptr - lengthbyte - 1 > MAX_DOMAIN_LABEL)				// If illegal label, abort
			return(mDNSNULL);
		*lengthbyte = (mDNSu8)(ptr - lengthbyte - 1);				// Fill in the length byte
		}

	*ptr++ = 0;														// Put the null root label on the end
	if (*cstr) return(mDNSNULL);									// Failure: We didn't successfully consume all input
	else return(ptr);												// Success: return new value of ptr
	}

// AppendDomainLabel appends a single label to a name.
// If successful, AppendDomainLabel returns a pointer to the next unused byte
// in the domainname bufer (i.e., the next byte after the terminating zero).
// If unable to construct a legal domain name (i.e. label more than 63 bytes, or total more than 255 bytes)
// AppendDomainLabel returns mDNSNULL.
mDNSexport mDNSu8 *AppendDomainLabel(domainname *const name, const domainlabel *const label)
	{
	int i;
	mDNSu8 *ptr = name->c + DomainNameLength(name) - 1;

	// Check label is legal
	if (label->c[0] > MAX_DOMAIN_LABEL) return(mDNSNULL);

	// Check that ptr + length byte + data bytes + final zero does not exceed our limit
	if (ptr + 1 + label->c[0] + 1 > name->c + MAX_DOMAIN_NAME) return(mDNSNULL);

	for (i=0; i<=label->c[0]; i++) *ptr++ = label->c[i];	// Copy the label data
	*ptr++ = 0;								// Put the null root label on the end
	return(ptr);
	}

mDNSexport mDNSu8 *AppendDomainName(domainname *const name, const domainname *const append)
	{
	mDNSu8       *      ptr = name->c + DomainNameLength(name) - 1;	// Find end of current name
	const mDNSu8 *const lim = name->c + MAX_DOMAIN_NAME - 1;		// Limit of how much we can add (not counting final zero)
	const mDNSu8 *      src = append->c;
	while(src[0])
		{
		int i;
		if (ptr + 1 + src[0] > lim) return(mDNSNULL);
		for (i=0; i<=src[0]; i++) *ptr++ = src[i];
		*ptr = 0;	// Put the null root label on the end
		src += i;
		}
	return(ptr);
	}

// MakeDomainLabelFromLiteralString makes a single domain label from a single literal C string (with no escaping).
// If successful, MakeDomainLabelFromLiteralString returns mDNStrue.
// If unable to convert the whole string to a legal domain label (i.e. because length is more than 63 bytes) then
// MakeDomainLabelFromLiteralString makes a legal domain label from the first 63 bytes of the string and returns mDNSfalse.
// In some cases silently truncated oversized names to 63 bytes is acceptable, so the return result may be ignored.
// In other cases silent truncation may not be acceptable, so in those cases the calling function needs to check the return result.
mDNSexport mDNSBool MakeDomainLabelFromLiteralString(domainlabel *const label, const char *cstr)
	{
	mDNSu8       *      ptr   = label->c + 1;						// Where we're putting it
	const mDNSu8 *const limit = label->c + 1 + MAX_DOMAIN_LABEL;	// The maximum we can put
	while (*cstr && ptr < limit) *ptr++ = (mDNSu8)*cstr++;			// Copy the label
	label->c[0] = (mDNSu8)(ptr - label->c - 1);						// Set the length byte
	return(*cstr == 0);												// Return mDNStrue if we successfully consumed all input
	}

// MakeDomainNameFromDNSNameString makes a native DNS-format domainname from a C string.
// The C string is in conventional DNS syntax:
// Textual labels, escaped as necessary using the usual DNS '\' notation, separated by dots.
// If successful, MakeDomainNameFromDNSNameString returns a pointer to the next unused byte
// in the domainname bufer (i.e., the next byte after the terminating zero).
// If unable to construct a legal domain name (i.e. label more than 63 bytes, or total more than 255 bytes)
// MakeDomainNameFromDNSNameString returns mDNSNULL.
mDNSexport mDNSu8 *MakeDomainNameFromDNSNameString(domainname *const name, const char *cstr)
	{
	name->c[0] = 0;									// Make an empty domain name
	return(AppendDNSNameString(name, cstr));		// And then add this string to it
	}

mDNSexport char *ConvertDomainLabelToCString_withescape(const domainlabel *const label, char *ptr, char esc)
	{
	const mDNSu8 *      src = label->c;							// Domain label we're reading
	const mDNSu8        len = *src++;							// Read length of this (non-null) label
	const mDNSu8 *const end = src + len;						// Work out where the label ends
	if (len > MAX_DOMAIN_LABEL) return(mDNSNULL);				// If illegal label, abort
	while (src < end)											// While we have characters in the label
		{
		mDNSu8 c = *src++;
		if (esc)
			{
			if (c == '.')										// If character is a dot,
				*ptr++ = esc;									// Output escape character
			else if (c <= ' ')									// If non-printing ascii,
				{												// Output decimal escape sequence
				*ptr++ = esc;
				*ptr++ = (char)  ('0' + (c / 100)     );
				*ptr++ = (char)  ('0' + (c /  10) % 10);
				c      = (mDNSu8)('0' + (c      ) % 10);
				}
			}
		*ptr++ = (char)c;										// Copy the character
		}
	*ptr = 0;													// Null-terminate the string
	return(ptr);												// and return
	}

// Note, to guarantee that there will be no possible overrun, cstr must be at least 1005 bytes
// The longest legal domain name is 255 bytes, in the form of three 64-byte labels, one 62-byte label,
// and the null root label.
// If every label character has to be escaped as a four-byte escape sequence, the maximum textual
// ascii display of this is 63*4 + 63*4 + 63*4 + 61*4 = 1000 label characters,
// plus four dots and the null at the end of the C string = 1005
mDNSexport char *ConvertDomainNameToCString_withescape(const domainname *const name, char *ptr, char esc)
	{
	const mDNSu8 *src         = name->c;								// Domain name we're reading
	const mDNSu8 *const max   = name->c + MAX_DOMAIN_NAME;			// Maximum that's valid
	
	if (*src == 0) *ptr++ = '.';									// Special case: For root, just write a dot

	while (*src)													// While more characters in the domain name
		{
		if (src + 1 + *src >= max) return(mDNSNULL);
		ptr = ConvertDomainLabelToCString_withescape((const domainlabel *)src, ptr, esc);
		if (!ptr) return(mDNSNULL);
		src += 1 + *src;
		*ptr++ = '.';												// Write the dot after the label
		}

	*ptr++ = 0;														// Null-terminate the string
	return(ptr);													// and return
	}

// RFC 1034 rules:
// Host names must start with a letter, end with a letter or digit,
// and have as interior characters only letters, digits, and hyphen.
// This was subsequently modified in RFC 1123 to allow the first character to be either a letter or a digit
#define mdnsValidHostChar(X, notfirst, notlast) (mdnsIsLetter(X) || mdnsIsDigit(X) || ((notfirst) && (notlast) && (X) == '-') )

mDNSexport void ConvertUTF8PstringToRFC1034HostLabel(const mDNSu8 UTF8Name[], domainlabel *const hostlabel)
	{
	const mDNSu8 *      src  = &UTF8Name[1];
	const mDNSu8 *const end  = &UTF8Name[1] + UTF8Name[0];
	      mDNSu8 *      ptr  = &hostlabel->c[1];
	const mDNSu8 *const lim  = &hostlabel->c[1] + MAX_DOMAIN_LABEL;
	while (src < end)
		{
		// Delete apostrophes from source name
		if (src[0] == '\'') { src++; continue; }		// Standard straight single quote
		if (src + 2 < end && src[0] == 0xE2 && src[1] == 0x80 && src[2] == 0x99)
			{ src += 3; continue; }	// Unicode curly apostrophe
		if (ptr < lim)
			{
			if (mdnsValidHostChar(*src, (ptr > &hostlabel->c[1]), (src < end-1))) *ptr++ = *src;
			else if (ptr > &hostlabel->c[1] && ptr[-1] != '-') *ptr++ = '-';
			}
		src++;
		}
	while (ptr > &hostlabel->c[1] && ptr[-1] == '-') ptr--;	// Truncate trailing '-' marks
	hostlabel->c[0] = (mDNSu8)(ptr - &hostlabel->c[1]);
	}

mDNSexport mDNSu8 *ConstructServiceName(domainname *const fqdn,
	const domainlabel *name, const domainname *const type, const domainname *const domain)
	{
	int i, len;
	mDNSu8 *dst = fqdn->c;
	const mDNSu8 *src;
	const char *errormsg;

	if (name && name->c[0])
		{
		src = name->c;									// Put the service name into the domain name
		len = *src;
		if (len >= 0x40) { errormsg="Service instance name too long"; goto fail; }
		for (i=0; i<=len; i++) *dst++ = *src++;
		}
	else
		name = (domainlabel*)"";	// Set this up to be non-null, to avoid errors if we have to call LogMsg() below
	
	src = type->c;										// Put the service type into the domain name
	len = *src;
	if (len == 0 || len >= 0x40)  { errormsg="Invalid service application protocol name"; goto fail; }
	for (i=0; i<=len; i++) *dst++ = *src++;

	len = *src;
	//if (len == 0 || len >= 0x40)  { errormsg="Invalid service transport protocol name"; goto fail; }
	if (!(len == 4 && src[1] == '_' &&
		(((src[2] | 0x20) == 'u' && (src[3] | 0x20) == 'd') || ((src[2] | 0x20) == 't' && (src[3] | 0x20) == 'c')) &&
		(src[4] | 0x20) == 'p'))
		{ errormsg="Service transport protocol name must be _udp or _tcp"; goto fail; }
	for (i=0; i<=len; i++) *dst++ = *src++;
	
	if (*src) { errormsg="Service type must have only two labels"; goto fail; }
	
	*dst = 0;
	dst = AppendDomainName(fqdn, domain);
	if (!dst) { errormsg="Service domain too long"; goto fail; }
	return(dst);

fail:
	LogMsg("ConstructServiceName: %s: %#s.%##s%##s", errormsg, name->c, type->c, domain->c);
	return(mDNSNULL);
	}

mDNSexport mDNSBool DeconstructServiceName(const domainname *const fqdn,
	domainlabel *const name, domainname *const type, domainname *const domain)
	{
	int i, len;
	const mDNSu8 *src = fqdn->c;
	const mDNSu8 *max = fqdn->c + MAX_DOMAIN_NAME;
	mDNSu8 *dst;
	
	dst = name->c;										// Extract the service name from the domain name
	len = *src;
	if (len >= 0x40) { debugf("DeconstructServiceName: service name too long"); return(mDNSfalse); }
	for (i=0; i<=len; i++) *dst++ = *src++;
	
	dst = type->c;										// Extract the service type from the domain name
	len = *src;
	if (len >= 0x40) { debugf("DeconstructServiceName: service type too long"); return(mDNSfalse); }
	for (i=0; i<=len; i++) *dst++ = *src++;

	len = *src;
	if (len >= 0x40) { debugf("DeconstructServiceName: service type too long"); return(mDNSfalse); }
	for (i=0; i<=len; i++) *dst++ = *src++;
	*dst++ = 0;		// Put the null root label on the end of the service type

	dst = domain->c;									// Extract the service domain from the domain name
	while (*src)
		{
		len = *src;
		if (len >= 0x40)
			{ debugf("DeconstructServiceName: service domain label too long"); return(mDNSfalse); }
		if (src + 1 + len + 1 >= max)
			{ debugf("DeconstructServiceName: service domain too long"); return(mDNSfalse); }
		for (i=0; i<=len; i++) *dst++ = *src++;
		}
	*dst++ = 0;		// Put the null root label on the end
	
	return(mDNStrue);
	}

// returns true if a rich text label ends in " (n)", or if an RFC 1034
// name ends in "--n", where n is some digit.
mDNSlocal mDNSBool LabelContainsSuffix(const domainlabel *name, const mDNSBool RichText)
	{
	mDNSu16 l = name->c[0];
	
	if (RichText) 
		{ 
		if (l < 4) return mDNSfalse;							// Need at least " (2)"
		if (name->c[l--] != ')') return mDNSfalse;				// Last char must be ')'
		if (!mdnsIsDigit(name->c[l])) return mDNSfalse;			// Preceeded by a digit
		l--;
		while (l > 2 && mdnsIsDigit(name->c[l])) l--;			// Strip off digits
		return (name->c[l] == '(' && name->c[l - 1] == ' ');
		}
	else
		{
		if (l < 3) return mDNSfalse;							// Need at least "--2"
		if (!mdnsIsDigit(name->c[l])) return mDNSfalse;			// Last char must be a digit
		l--;
		while (l > 2 && mdnsIsDigit(name->c[l])) l--;			// Strip off digits
		return (name->c[l] == '-' && name->c[l - 1] == '-');
		}
	}

// removes an auto-generated suffix (appended on a name collision) from a label.  caller is
// responsible for ensuring that the label does indeed contain a suffix.  returns the number 
// from the suffix that was removed.
mDNSlocal mDNSu32 RemoveLabelSuffix(domainlabel *name, mDNSBool RichText)
	{
	mDNSu32 val = 0, multiplier = 1;
		
	if (RichText) name->c[0]--;  // chop closing parentheses from RT suffix
	// Get any existing numerical suffix off the name
	while (mdnsIsDigit(name->c[name->c[0]])) 
		{ val += (name->c[name->c[0]] - '0') * multiplier; multiplier *= 10; name->c[0]--; }	
	name->c[0] -= 2;  // chop opening parentheses and whitespace (RT) or double-hyphen (RFC 1034)
	return(val);
	}

// appends a numerical suffix to a label, with the number following a whitespace and enclosed 
// in parentheses (rich text) or following two consecutive hyphens (RFC 1034 domain label).
mDNSlocal void AppendLabelSuffix(domainlabel *name, mDNSu32 val, mDNSBool RichText)
	{
	mDNSu32 divisor = 1, chars = 3;	// Shortest possible RFC1034 name suffix is 3 characters ("--2")
	if (RichText) chars = 4;		// Shortest possible RichText suffix is 4 characters (" (2)")
	
	// Truncate trailing spaces from RichText names
	if (RichText) while (name->c[name->c[0]] == ' ') name->c[0]--;

	while (val >= divisor * 10) { divisor *= 10; chars++; }

	if (name->c[0] > (mDNSu8)(MAX_DOMAIN_LABEL - chars))
		{
		name->c[0] = (mDNSu8)(MAX_DOMAIN_LABEL - chars);
		// If the following character is a UTF-8 continuation character,
		// we just chopped a multi-byte UTF-8 character in the middle, so strip back to a safe truncation point
		while (name->c[0] > 0 && (name->c[name->c[0]+1] & 0xC0) == 0x80) name->c[0]--;
		}

	if (RichText) { name->c[++name->c[0]] = ' '; name->c[++name->c[0]] = '('; }
	else          { name->c[++name->c[0]] = '-'; name->c[++name->c[0]] = '-'; }
	
	while (divisor)
		{
		name->c[++name->c[0]] = (mDNSu8)('0' + val / divisor);
		val     %= divisor;
		divisor /= 10;
		}

	if (RichText) name->c[++name->c[0]] = ')';
	}

mDNSexport void IncrementLabelSuffix(domainlabel *name, mDNSBool RichText)
	{
	mDNSu32 val = 0;

	if (LabelContainsSuffix(name, RichText)) 
		val = RemoveLabelSuffix(name, RichText);
		
	// If existing suffix, increment it, else start by renaming "Foo" as "Foo (2)" or "Foo--2" as appropriate.
	// After sequentially trying each single-digit suffix, we try a random 2-digit suffix, then 3-digit, then
	// continue generating random 4-digit integers.
	if      (val ==  0)	val = 2;
	else if (val <   9)	val++;
	else if (val <  10)	val = mDNSRandom(89) + 10;
	else if (val < 100)	val = mDNSRandom(899) + 100;
	else 			val = mDNSRandom(8999) + 1000;
	
	AppendLabelSuffix(name, val, RichText);
	}	

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - Resource Record Utility Functions
#endif

#define RRIsAddressType(RR) ((RR)->rrtype == kDNSType_A || (RR)->rrtype == kDNSType_AAAA)

#define ResourceRecordIsValidAnswer(RR) ( ((RR)->             RecordType & kDNSRecordTypeActiveMask)  && \
		((RR)->Additional1 == mDNSNULL || ((RR)->Additional1->RecordType & kDNSRecordTypeActiveMask)) && \
		((RR)->Additional2 == mDNSNULL || ((RR)->Additional2->RecordType & kDNSRecordTypeActiveMask)) && \
		((RR)->DependentOn == mDNSNULL || ((RR)->DependentOn->RecordType & kDNSRecordTypeActiveMask))  )

#define ResourceRecordIsValidInterfaceAnswer(RR, INTID) \
	(ResourceRecordIsValidAnswer(RR) && \
	((RR)->InterfaceID == mDNSInterface_Any || (RR)->InterfaceID == (INTID)))

#define RRUniqueOrKnownUnique(RR) ((RR)->RecordType == kDNSRecordTypeUnique || (RR)->RecordType == kDNSRecordTypeKnownUnique)

#define DefaultProbeCountForTypeUnique ((mDNSu8)3)
#define DefaultProbeCountForRecordType(X)      ((X) == kDNSRecordTypeUnique ? DefaultProbeCountForTypeUnique : (mDNSu8)0)

// For records that have *never* been announced on the wire, their AnnounceCount will be set to InitialAnnounceCount (10).
// When de-registering these records we do not need to send any goodbye packet because we never announced them in the first
// place. If AnnounceCount is less than InitialAnnounceCount that means we have announced them at least once, so a goodbye
// packet is needed. For this reason, if we ever reset AnnounceCount (e.g. after an interface change) we set it to
// ReannounceCount (9), not InitialAnnounceCount. If we were to reset AnnounceCount back to InitialAnnounceCount that would
// imply that the record had never been announced on the wire (which is false) and if the client were then to immediately
// deregister that record before it had a chance to announce, we'd fail to send its goodbye packet (which would be a bug).
#define InitialAnnounceCount ((mDNSu8)10)
#define ReannounceCount      ((mDNSu8)9)

// Note that the announce intervals use exponential backoff, doubling each time. The probe intervals do not.
// This means that because the announce interval is doubled after sending the first packet, the first
// observed on-the-wire inter-packet interval between announcements is actually one second.
// The half-second value here may be thought of as a conceptual (non-existent) half-second delay *before* the first packet is sent.
#define DefaultProbeIntervalForTypeUnique (mDNSPlatformOneSecond/4)
#define DefaultAnnounceIntervalForTypeShared (mDNSPlatformOneSecond/2)
#define DefaultAnnounceIntervalForTypeUnique (mDNSPlatformOneSecond/2)

#define DefaultAPIntervalForRecordType(X)  ((X) == kDNSRecordTypeShared      ? DefaultAnnounceIntervalForTypeShared : \
											(X) == kDNSRecordTypeUnique      ? DefaultProbeIntervalForTypeUnique    : \
											(X) == kDNSRecordTypeVerified    ? DefaultAnnounceIntervalForTypeUnique : \
											(X) == kDNSRecordTypeKnownUnique ? DefaultAnnounceIntervalForTypeUnique : 0)

#define TimeToAnnounceThisRecord(RR,time) ((RR)->RRInterfaceActive && (RR)->AnnounceCount && (time) - ((RR)->LastAPTime + (RR)->ThisAPInterval) >= 0)
#define TimeToSendThisRecord(RR,time) ((TimeToAnnounceThisRecord(RR,time) || (RR)->ImmedAnswer) && ResourceRecordIsValidAnswer(RR))

mDNSlocal mDNSBool SameRData(const mDNSu16 r1type, const mDNSu16 r2type, const RData *const r1, const RData *const r2)
	{
	if (r1type != r2type) return mDNSfalse;
	if (r1->RDLength != r2->RDLength) return(mDNSfalse);
	switch(r1type)
		{
		case kDNSType_CNAME:// Same as PTR
		case kDNSType_PTR:	return(SameDomainName(&r1->u.name, &r2->u.name));

		case kDNSType_SRV:	return( r1->u.srv.priority          == r2->u.srv.priority          &&
									r1->u.srv.weight            == r2->u.srv.weight            &&
									r1->u.srv.port.NotAnInteger == r2->u.srv.port.NotAnInteger &&
									SameDomainName(&r1->u.srv.target, &r2->u.srv.target));

		default:			return(mDNSPlatformMemSame(r1->u.data, r2->u.data, r1->RDLength));
		}
	}

mDNSlocal mDNSBool ResourceRecordAnswersQuestion(const ResourceRecord *const rr, const DNSQuestion *const q)
	{
	if (rr->InterfaceID &&
		q ->InterfaceID &&
		rr->InterfaceID != q->InterfaceID) return(mDNSfalse);

	// RR type CNAME matches any query type. QTYPE ANY matches any RR type. QCLASS ANY matches any RR class.
	if (rr->rrtype != kDNSType_CNAME && rr->rrtype  != q->rrtype  && q->rrtype  != kDNSQType_ANY ) return(mDNSfalse);
	if (                                rr->rrclass != q->rrclass && q->rrclass != kDNSQClass_ANY) return(mDNSfalse);
	return(SameDomainName(&rr->name, &q->name));
	}

mDNSlocal mDNSs32 HashSlot(const domainname *name)
	{
	mDNSu16	sum = 0;
	const mDNSu8	*c;

	for (c = name->c; c[0] != 0 && c[1] != 0; c += 2)
		{
		sum += ((c[0] >= 'A' && c[0] <= 'Z' ? c[0] + 'a' - 'A' : c[0]) << 8) |
			   (c[1] >= 'A' && c[1] <= 'Z' ? c[1] + 'a' - 'A' : c[1]);
		}
	if (c[0]) sum += ((c[0] >= 'A' && c[0] <= 'Z' ? c[0] + 'a' - 'A' : c[0]) << 8);
	return sum % CACHE_HASH_SLOTS;
	}

// SameResourceRecordSignature returns true if two resources records have the same interface, name, type, and class.
// -- i.e. if they would both be given in response to the same question.
// (TTL and rdata may differ)
mDNSlocal mDNSBool SameResourceRecordSignature(const ResourceRecord *const r1, const ResourceRecord *const r2)
	{
	if (!r1) { debugf("SameResourceRecordSignature ERROR: r1 is NULL"); return(mDNSfalse); }
	if (!r2) { debugf("SameResourceRecordSignature ERROR: r2 is NULL"); return(mDNSfalse); }
	if (r1->InterfaceID &&
		r2->InterfaceID &&
		r1->InterfaceID != r2->InterfaceID) return(mDNSfalse);
	return (r1->rrtype == r2->rrtype && r1->rrclass == r2->rrclass && SameDomainName(&r1->name, &r2->name));
	}

// PacketRRMatchesSignature behaves as SameResourceRecordSignature, except that types may differ if the
// authoratative record is in the probing state.  Probes are sent with the wildcard type, so a response of 
// any type should match, even if it is not the type the client plans to use.
mDNSlocal mDNSBool PacketRRMatchesSignature(const ResourceRecord *const pktrr, const ResourceRecord *const authrr)
        {
	if (!pktrr) { debugf("SameResourceRecordSignature ERROR: pktrr is NULL"); return(mDNSfalse); }
	if (!authrr) { debugf("SameResourceRecordSignature ERROR: authrr is NULL"); return(mDNSfalse); }
	if (pktrr->InterfaceID &&
		authrr->InterfaceID &&
		pktrr->InterfaceID != authrr->InterfaceID) return(mDNSfalse);
	if (authrr->RecordType != kDNSRecordTypeUnique && pktrr->rrtype != authrr->rrtype) return(mDNSfalse);
	return (pktrr->rrclass == authrr->rrclass && SameDomainName(&pktrr->name, &authrr->name));
	}

// SameResourceRecordSignatureAnyInterface returns true if two resources records have the same name, type, and class.
// (InterfaceID, TTL and rdata may differ)
mDNSlocal mDNSBool SameResourceRecordSignatureAnyInterface(const ResourceRecord *const r1, const ResourceRecord *const r2)
	{
	if (!r1) { debugf("SameResourceRecordSignatureAnyInterface ERROR: r1 is NULL"); return(mDNSfalse); }
	if (!r2) { debugf("SameResourceRecordSignatureAnyInterface ERROR: r2 is NULL"); return(mDNSfalse); }
	return (r1->rrtype == r2->rrtype && r1->rrclass == r2->rrclass && SameDomainName(&r1->name, &r2->name));
	}

// IdenticalResourceRecord returns true if two resources records have
// the same interface, name, type, class, and identical rdata (TTL may differ)
mDNSlocal mDNSBool IdenticalResourceRecord(const ResourceRecord *const r1, const ResourceRecord *const r2)
	{
	if (!SameResourceRecordSignature(r1, r2)) return(mDNSfalse);
	return(SameRData(r1->rrtype, r2->rrtype, r1->rdata, r2->rdata));
	}

// IdenticalResourceRecordAnyInterface returns true if two resources records have
// the same name, type, class, and identical rdata (InterfaceID and TTL may differ)
mDNSlocal mDNSBool IdenticalResourceRecordAnyInterface(const ResourceRecord *const r1, const ResourceRecord *const r2)
	{
	if (!SameResourceRecordSignatureAnyInterface(r1, r2)) return(mDNSfalse);
	return(SameRData(r1->rrtype, r2->rrtype, r1->rdata, r2->rdata));
	}

// ResourceRecord *ds is the ResourceRecord from the known answer list in the query
// This is the information that the requester believes to be correct
// ResourceRecord *rr is the answer we are proposing to give, if not suppressed
// This is the information that we believe to be correct
mDNSlocal mDNSBool ShouldSuppressKnownAnswer(const ResourceRecord *const ka, const ResourceRecord *const rr)
	{
	// If RR signature is different, or data is different, then don't suppress our answer
	if (!IdenticalResourceRecord(ka,rr)) return(mDNSfalse);
	
	// If the requester's indicated TTL is less than half the real TTL,
	// we need to give our answer before the requester's copy expires.
	// If the requester's indicated TTL is at least half the real TTL,
	// then we can suppress our answer this time.
	// If the requester's indicated TTL is greater than the TTL we believe,
	// then that's okay, and we don't need to do anything about it.
	// (If two responders on the network are offering the same information,
	// that's okay, and if they are offering the information with different TTLs,
	// the one offering the lower TTL should defer to the one offering the higher TTL.)
	return(ka->rroriginalttl >= rr->rroriginalttl / 2);
	}

mDNSlocal mDNSu16 GetRDLength(const ResourceRecord *const rr, mDNSBool estimate)
	{
	const domainname *const name = estimate ? &rr->name : mDNSNULL;
	switch (rr->rrtype)
		{
		case kDNSType_A:	return(sizeof(rr->rdata->u.ip)); break;
		case kDNSType_CNAME:// Same as PTR
		case kDNSType_PTR:	return(CompressedDomainNameLength(&rr->rdata->u.name, name));
		case kDNSType_TXT:  return(rr->rdata->RDLength); // TXT is not self-describing, so have to just trust rdlength
		case kDNSType_AAAA:	return(sizeof(rr->rdata->u.ipv6)); break;
		case kDNSType_SRV:	return(mDNSu16)(6 + CompressedDomainNameLength(&rr->rdata->u.srv.target, name));
		default:			debugf("Warning! Don't know how to get length of resource type %d", rr->rrtype);
							return(rr->rdata->RDLength);
		}
	}

mDNSlocal void SetNextAnnounceProbeTime(mDNS *const m, const ResourceRecord *const rr)
	{
	if (rr->RecordType == kDNSRecordTypeUnique)
		{
		if (m->NextProbeTime - (rr->LastAPTime + rr->ThisAPInterval) >= 0)
			m->NextProbeTime = (rr->LastAPTime + rr->ThisAPInterval);
		}
	else if (rr->AnnounceCount && ResourceRecordIsValidAnswer(rr))
		{
		if (m->NextResponseTime - (rr->LastAPTime + rr->ThisAPInterval) >= 0)
			m->NextResponseTime = (rr->LastAPTime + rr->ThisAPInterval);
		}
	}

#define GetRRHostNameTarget(RR) (                                                                                    \
	((RR)->rrtype == kDNSType_CNAME || (RR)->rrtype == kDNSType_PTR) ? &(RR)->rdata->u.name       :          \
	((RR)->rrtype == kDNSType_SRV                                  ) ? &(RR)->rdata->u.srv.target : mDNSNULL )

mDNSlocal void SetTargetToHostName(mDNS *const m, ResourceRecord *const rr)
	{
	domainname *target = GetRRHostNameTarget(rr);

	if (!target) debugf("SetTargetToHostName: Don't know how to set the target of rrtype %d", rr->rrtype);

	if (target && SameDomainName(target, &m->hostname1))
		debugf("SetTargetToHostName: Target of %##s is already %##s", rr->name.c, target->c);
	
	if (target && !SameDomainName(target, &m->hostname1))
		{
		*target = m->hostname1;
		rr->rdata->RDLength = GetRDLength(rr, mDNSfalse);
		rr->rdestimate      = GetRDLength(rr, mDNStrue);
		
		// If we're in the middle of probing this record, we need to start again,
		// because changing its rdata may change the outcome of the tie-breaker.
		// (If the record type is kDNSRecordTypeUnique (unconfirmed unique) then DefaultProbeCountForRecordType is non-zero.)
		rr->ProbeCount     = DefaultProbeCountForRecordType(rr->RecordType);

		// If we've announced this record, we really should send a goodbye packet for the old rdata before
		// changing to the new rdata. However, in practice, we only do SetTargetToHostName for unique records,
		// so when we announce them we'll set the kDNSClass_UniqueRRSet and clear any stale data that way.
		if (rr->AnnounceCount < InitialAnnounceCount && !(rr->RecordType & kDNSRecordTypeUniqueMask))
			debugf("Have announced shared record %##s at least once: should have sent a goodbye packet before updating", rr->name.c);

		if (rr->AnnounceCount < ReannounceCount)
			rr->AnnounceCount = ReannounceCount;
		rr->ThisAPInterval = DefaultAPIntervalForRecordType(rr->RecordType);
		rr->LastAPTime     = m->timenow - rr->ThisAPInterval;
		if (RRUniqueOrKnownUnique(rr) && m->SuppressProbes) rr->LastAPTime = m->SuppressProbes - rr->ThisAPInterval;
	
		// If this is a record type that's not going to probe, then delay its first announcement so that it will go out
		// synchronized with the first announcement for the other records that *are* probing
		// The addition of "rr->ThisAPInterval / 4" is to make sure that this announcement is not scheduled to go out
		// *before* the probing is complete. When the probing is complete and those records begin to
		// announce, these records will also be picked up and accelerated, because they will be considered to be
		// more than half-way to their scheduled announcement time.
		// Ignoring timing jitter, they will be exactly 3/4 of the way to their scheduled announcement time.
		// Anything between 50% and 100% would work, so aiming for 75% gives us the best safety margin in case of timing jitter.
		if (rr->ProbeCount == 0) rr->LastAPTime += DefaultProbeIntervalForTypeUnique * DefaultProbeCountForTypeUnique + rr->ThisAPInterval / 4;

		SetNextAnnounceProbeTime(m, rr);
		}
	}

mDNSlocal mStatus mDNS_Register_internal(mDNS *const m, ResourceRecord *const rr)
	{
	ResourceRecord **p = &m->ResourceRecords;
	while (*p && *p != rr) p=&(*p)->next;
	if (*p)
		{
		debugf("Error! Tried to register a ResourceRecord %p %##s that's already in the list", rr, rr->name.c);
		return(mStatus_AlreadyRegistered);
		}

	if (rr->DependentOn)
		{
		if (rr->RecordType == kDNSRecordTypeUnique)
			rr->RecordType =  kDNSRecordTypeVerified;
		else
			{
			debugf("mDNS_Register_internal: ERROR! %##s: rr->DependentOn && RecordType != kDNSRecordTypeUnique",
				rr->name.c);
			return(mStatus_Invalid);
			}
		if (rr->DependentOn->RecordType != kDNSRecordTypeUnique && rr->DependentOn->RecordType != kDNSRecordTypeVerified)
			{
			debugf("mDNS_Register_internal: ERROR! %##s: rr->DependentOn->RecordType bad type %X",
				rr->name.c, rr->DependentOn->RecordType);
			return(mStatus_Invalid);
			}
		}

	if (rr->InterfaceID)		// If this resource record is referencing a specific interface
		{
		NetworkInterfaceInfo *intf;	// Then make sure that interface exists
		for (intf = m->HostInterfaces; intf; intf = intf->next)
			if (intf->InterfaceID == rr->InterfaceID) break;
		if (!intf)
			{
			debugf("mDNS_Register_internal: Bogus InterfaceID %p in resource record", rr->InterfaceID);
			return(mStatus_BadReferenceErr);
			}
		}

	rr->next = mDNSNULL;

	// Field Group 1: Persistent metadata for Authoritative Records
//	rr->Additional1       = set to mDNSNULL in mDNS_SetupResourceRecord; may be overridden by client
//	rr->Additional2       = set to mDNSNULL in mDNS_SetupResourceRecord; may be overridden by client
//	rr->DependentOn       = set to mDNSNULL in mDNS_SetupResourceRecord; may be overridden by client
//	rr->RRSet             = set to mDNSNULL in mDNS_SetupResourceRecord; may be overridden by client
//	rr->Callback          = already set in mDNS_SetupResourceRecord
//	rr->Context           = already set in mDNS_SetupResourceRecord
//	rr->RecordType        = already set in mDNS_SetupResourceRecord
//	rr->HostTarget        = set to mDNSfalse in mDNS_SetupResourceRecord; may be overridden by client

	// Field Group 2: Transient state for Authoritative Records
	rr->RRInterfaceActive = mDNStrue;
	rr->Acknowledged      = mDNSfalse;
	rr->ProbeCount        = DefaultProbeCountForRecordType(rr->RecordType);
	rr->AnnounceCount     = InitialAnnounceCount;
	rr->IncludeInProbe    = mDNSfalse;
	rr->ImmedAnswer       = mDNSNULL;
	rr->ImmedAdditional   = mDNSNULL;
	rr->SendRNow          = mDNSNULL;
	rr->v4Requester       = zeroIPAddr;
	rr->v6Requester       = zerov6Addr;
	rr->NextResponse      = mDNSNULL;
	rr->NR_AnswerTo       = mDNSNULL;
	rr->NR_AdditionalTo   = mDNSNULL;
	rr->ThisAPInterval    = DefaultAPIntervalForRecordType(rr->RecordType);
	rr->LastAPTime        = m->timenow - rr->ThisAPInterval;
	if (RRUniqueOrKnownUnique(rr) && m->SuppressProbes) rr->LastAPTime = m->SuppressProbes - rr->ThisAPInterval;
	// If this record is not going to probe, adjust it so its announcement will go out
	// synchronized with other associated records that will probe.
	if (rr->RecordType == kDNSRecordTypeKnownUnique)
		rr->LastAPTime += DefaultProbeIntervalForTypeUnique * DefaultProbeCountForTypeUnique + rr->ThisAPInterval / 4;
	SetNextAnnounceProbeTime(m, rr);
	rr->NewRData          = mDNSNULL;
	rr->UpdateCallback    = mDNSNULL;

	// Field Group 3: Transient state for Cache Records
	rr->NextInKAList      = mDNSNULL;	// Not strictly relevant for a local record
	rr->TimeRcvd          = 0;			// Not strictly relevant for a local record
	rr->LastUsed          = 0;			// Not strictly relevant for a local record
	rr->UseCount          = 0;			// Not strictly relevant for a local record
	rr->CRActiveQuestion  = mDNSNULL;	// Not strictly relevant for a local record
	rr->UnansweredQueries = 0;			// Not strictly relevant for a local record
	rr->NewData           = mDNSfalse;	// Not strictly relevant for a local record

	// Field Group 4: The actual information pertaining to this resource record
//	rr->interface         = already set in mDNS_SetupResourceRecord
//	rr->name.c            = MUST be set by client
//	rr->rrtype            = already set in mDNS_SetupResourceRecord
//	rr->rrclass           = already set in mDNS_SetupResourceRecord
//	rr->rroriginalttl     = already set in mDNS_SetupResourceRecord
//	rr->rrremainingttl    = already set in mDNS_SetupResourceRecord

	if (rr->HostTarget)
		{
		domainname *target = GetRRHostNameTarget(rr);
		if (target) target->c[0] = 0;
		SetTargetToHostName(m, rr);	// This also sets rdlength and rdestimate for us
		}
	else
		{
		rr->rdata->RDLength = GetRDLength(rr, mDNSfalse);
		rr->rdestimate      = GetRDLength(rr, mDNStrue);
		}
//	rr->rdata             = MUST be set by client

	*p = rr;
	return(mStatus_NoError);
	}

// mDNS_Dereg_normal is used for most calls to mDNS_Deregister_internal
// mDNS_Dereg_conflict is used to indicate that this record is being forcibly deregistered because of a conflict
// mDNS_Dereg_repeat is used when cleaning up, for records that may have already been forcibly deregistered
typedef enum { mDNS_Dereg_normal, mDNS_Dereg_conflict, mDNS_Dereg_repeat } mDNS_Dereg_type;

// NOTE: mDNS_Deregister_internal can call a user callback, which may change the record list and/or question list.
// Any code walking either list must use the CurrentQuestion and/or CurrentRecord mechanism to protect against this.
mDNSlocal mStatus mDNS_Deregister_internal(mDNS *const m, ResourceRecord *const rr, mDNS_Dereg_type drt)
	{
	mDNSu8 RecordType = rr->RecordType;
	ResourceRecord **p = &m->ResourceRecords;	// Find this record in our list of active records
	while (*p && *p != rr) p=&(*p)->next;

	if (!*p)
		{
		// No need to log an error message if we already know this is a potentially repeated deregistration
		if (drt != mDNS_Dereg_repeat)
			debugf("mDNS_Deregister_internal: Record %p %##s (%s) not found in list", rr, rr->name.c, DNSTypeName(rr->rrtype));
		return(mStatus_BadReferenceErr);
		}

	// If this is a shared record and we've announced it at least once,
	// we need to retract that announcement before we delete the record
	if (RecordType == kDNSRecordTypeShared && rr->AnnounceCount < InitialAnnounceCount && rr->RRInterfaceActive)
		{
		debugf("mDNS_Deregister_internal: Sending deregister for %##s (%s)", rr->name.c, DNSTypeName(rr->rrtype));
		rr->RecordType      = kDNSRecordTypeDeregistering;
		rr->rroriginalttl   = 0;
		rr->rrremainingttl  = 0;
		rr->ImmedAnswer     = mDNSInterfaceMark;
		m->NextResponseTime = m->timenow;
		}
	else
		{
		*p = rr->next;					// Cut this record from the list
		// If someone is about to look at this, bump the pointer forward
		if (m->CurrentRecord == rr) m->CurrentRecord = rr->next;
		rr->next = mDNSNULL;

		if      (RecordType == kDNSRecordTypeUnregistered)
			debugf("mDNS_Deregister_internal: Record %##s (%s) already marked kDNSRecordTypeUnregistered",
				rr->name.c, DNSTypeName(rr->rrtype));
		else if (RecordType == kDNSRecordTypeDeregistering)
			debugf("mDNS_Deregister_internal: Record %##s (%s) already marked kDNSRecordTypeDeregistering",
				rr->name.c, DNSTypeName(rr->rrtype));
		else
			{
			verbosedebugf("mDNS_Deregister_internal: Deleting record for %##s (%s)", rr->name.c, DNSTypeName(rr->rrtype));
			rr->RecordType = kDNSRecordTypeUnregistered;
			}

		if ((drt == mDNS_Dereg_conflict || drt == mDNS_Dereg_repeat) && RecordType == kDNSRecordTypeShared)
			debugf("mDNS_Deregister_internal: Cannot have a conflict on a shared record! %##s (%s)",
				rr->name.c, DNSTypeName(rr->rrtype));

		// If we have an update queued up which never executed, give the client a chance to free that memory
		if (rr->NewRData)
			{
			RData *OldRData = rr->rdata;
			rr->rdata = rr->NewRData;	// Update our rdata
			rr->NewRData = mDNSNULL;	// Clear the NewRData pointer ...
			if (rr->UpdateCallback)	
				rr->UpdateCallback(m, rr, OldRData);	// ... and let the client know
			}
		
		// CAUTION: MUST NOT do anything more with rr after calling rr->Callback(), because the client's callback function
		// is allowed to do anything, including starting/stopping queries, registering/deregistering records, etc.
		// In this case the likely client action to the mStatus_MemFree message is to free the memory,
		// so any attempt to touch rr after this is likely to lead to a crash.
		m->mDNS_reentrancy++; // Increment to allow client to legally make mDNS API calls from the callback
		if (RecordType == kDNSRecordTypeShared)
			{
			if (rr->RecordCallback)
				rr->RecordCallback(m, rr, mStatus_MemFree);
			}
		else if (drt == mDNS_Dereg_conflict)
			{
			m->ProbeFailTime = m->timenow;
			// If we've had ten probe failures, rate-limit to one every five seconds
			// The result is ORed with 1 to make sure SuppressProbes is not accidentally set to zero
			if (m->NumFailedProbes < 9) m->NumFailedProbes++;
			else m->SuppressProbes = (m->timenow + mDNSPlatformOneSecond * 5) | 1;
			if (rr->RecordCallback)
				rr->RecordCallback(m, rr, mStatus_NameConflict);
			}
		m->mDNS_reentrancy--; // Decrement to block mDNS API calls again
		}
	return(mStatus_NoError);
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark -
#pragma mark - DNS Message Creation Functions
#endif

mDNSlocal void InitializeDNSMessage(DNSMessageHeader *h, mDNSOpaque16 id, mDNSOpaque16 flags)
	{
	h->id             = id;
	h->flags          = flags;
	h->numQuestions   = 0;
	h->numAnswers     = 0;
	h->numAuthorities = 0;
	h->numAdditionals = 0;
	}

mDNSlocal const mDNSu8 *FindCompressionPointer(const mDNSu8 *const base, const mDNSu8 *const end, const mDNSu8 *const domname)
	{
	const mDNSu8 *result = end - *domname - 1;

	if (*domname == 0) return(mDNSNULL);	// There's no point trying to match just the root label
	
	// This loop examines each possible starting position in packet, starting end of the packet and working backwards
	while (result >= base)
		{
		// If the length byte and first character of the label match, then check further to see
		// if this location in the packet will yield a useful name compression pointer.
		if (result[0] == domname[0] && result[1] == domname[1])
			{
			const mDNSu8 *name = domname;
			const mDNSu8 *targ = result;
			while (targ + *name < end)
				{
				// First see if this label matches
				int i;
				const mDNSu8 *pointertarget;
				for (i=0; i <= *name; i++) if (targ[i] != name[i]) break;
				if (i <= *name) break;							// If label did not match, bail out
				targ += 1 + *name;								// Else, did match, so advance target pointer
				name += 1 + *name;								// and proceed to check next label
				if (*name == 0 && *targ == 0) return(result);	// If no more labels, we found a match!
				if (*name == 0) break;							// If no more labels to match, we failed, so bail out

				// The label matched, so now follow the pointer (if appropriate) and then see if the next label matches
				if (targ[0] < 0x40) continue;					// If length value, continue to check next label
				if (targ[0] < 0xC0) break;						// If 40-BF, not valid
				if (targ+1 >= end) break;						// Second byte not present!
				pointertarget = base + (((mDNSu16)(targ[0] & 0x3F)) << 8) + targ[1];
				if (targ < pointertarget) break;				// Pointertarget must point *backwards* in the packet
				if (pointertarget[0] >= 0x40) break;			// Pointertarget must point to a valid length byte
				targ = pointertarget;
				}
			}
		result--;	// We failed to match at this search position, so back up the tentative result pointer and try again
		}
	return(mDNSNULL);
	}

// Put a string of dot-separated labels as length-prefixed labels
// domainname is a fully-qualified name (i.e. assumed to be ending in a dot, even if it doesn't)
// msg points to the message we're building (pass mDNSNULL if we don't want to use compression pointers)
// end points to the end of the message so far
// ptr points to where we want to put the name
// limit points to one byte past the end of the buffer that we must not overrun
// domainname is the name to put
mDNSlocal mDNSu8 *putDomainNameAsLabels(const DNSMessage *const msg,
	mDNSu8 *ptr, const mDNSu8 *const limit, const domainname *const name)
	{
	const mDNSu8 *const base        = (const mDNSu8 *const)msg;
	const mDNSu8 *      np          = name->c;
	const mDNSu8 *const max         = name->c + MAX_DOMAIN_NAME;	// Maximum that's valid
	const mDNSu8 *      pointer     = mDNSNULL;
	const mDNSu8 *const searchlimit = ptr;

	while (*np && ptr < limit-1)		// While we've got characters in the name, and space to write them in the message...
		{
		if (*np > MAX_DOMAIN_LABEL)
			{ LogMsg("Malformed domain name %##s (label more than 63 bytes)", name->c); return(mDNSNULL); }
		
		// This check correctly allows for the final trailing root label:
		// e.g.
		// Suppose our domain name is exactly 255 bytes long, including the final trailing root label.
		// Suppose np is now at name->c[248], and we're about to write our last non-null label ("local").
		// We know that max will be at name->c[255]
		// That means that np + 1 + 5 == max - 1, so we (just) pass the "if" test below, write our
		// six bytes, then exit the loop, write the final terminating root label, and the domain
		// name we've written is exactly 255 bytes long, exactly at the correct legal limit.
		// If the name is one byte longer, then we fail the "if" test below, and correctly bail out.
		if (np + 1 + *np >= max)
			{ LogMsg("Malformed domain name %##s (more than 255 bytes)", name->c); return(mDNSNULL); }

		if (base) pointer = FindCompressionPointer(base, searchlimit, np);
		if (pointer)					// Use a compression pointer if we can
			{
			mDNSu16 offset = (mDNSu16)(pointer - base);
			*ptr++ = (mDNSu8)(0xC0 | (offset >> 8));
			*ptr++ = (mDNSu8)(        offset      );
			return(ptr);
			}
		else							// Else copy one label and try again
			{
			int i;
			mDNSu8 len = *np++;
			if (ptr + 1 + len >= limit) return(mDNSNULL);
			*ptr++ = len;
			for (i=0; i<len; i++) *ptr++ = *np++;
			}
		}

	if (ptr < limit)												// If we didn't run out of space
		{
		*ptr++ = 0;													// Put the final root label
		return(ptr);												// and return
		}

	return(mDNSNULL);
	}

mDNSlocal mDNSu8 *putRData(const DNSMessage *const msg, mDNSu8 *ptr, const mDNSu8 *const limit,
	const mDNSu16 rrtype, const RData *const rdata)
	{
	switch (rrtype)
		{
		case kDNSType_A:	if (rdata->RDLength != 4)
								{
								debugf("putRData: Illegal length %d for kDNSType_A", rdata->RDLength);
								return(mDNSNULL);
								}
							if (ptr + 4 > limit) return(mDNSNULL);
							*ptr++ = rdata->u.ip.b[0];
							*ptr++ = rdata->u.ip.b[1];
							*ptr++ = rdata->u.ip.b[2];
							*ptr++ = rdata->u.ip.b[3];
							return(ptr);

		case kDNSType_CNAME:// Same as PTR
		case kDNSType_PTR:	return(putDomainNameAsLabels(msg, ptr, limit, &rdata->u.name));

		case kDNSType_TXT:  if (ptr + rdata->RDLength > limit) return(mDNSNULL);
							mDNSPlatformMemCopy(rdata->u.data, ptr, rdata->RDLength);
							return(ptr + rdata->RDLength);

		case kDNSType_AAAA:	if (rdata->RDLength != sizeof(rdata->u.ipv6))
								{
								debugf("putRData: Illegal length %d for kDNSType_AAAA", rdata->RDLength);
								return(mDNSNULL);
								}
							if (ptr + sizeof(rdata->u.ipv6) > limit) return(mDNSNULL);
							mDNSPlatformMemCopy(&rdata->u.ipv6, ptr, sizeof(rdata->u.ipv6));
							return(ptr + sizeof(rdata->u.ipv6));

		case kDNSType_SRV:	if (ptr + 6 > limit) return(mDNSNULL);
							*ptr++ = (mDNSu8)(rdata->u.srv.priority >> 8);
							*ptr++ = (mDNSu8)(rdata->u.srv.priority     );
							*ptr++ = (mDNSu8)(rdata->u.srv.weight   >> 8);
							*ptr++ = (mDNSu8)(rdata->u.srv.weight       );
							*ptr++ = rdata->u.srv.port.b[0];
							*ptr++ = rdata->u.srv.port.b[1];
							return(putDomainNameAsLabels(msg, ptr, limit, &rdata->u.srv.target));

		default:			if (ptr + rdata->RDLength > limit) return(mDNSNULL);
							debugf("putRData: Warning! Writing resource type %d as raw data", rrtype);
							mDNSPlatformMemCopy(rdata->u.data, ptr, rdata->RDLength);
							return(ptr + rdata->RDLength);
		}
	}

mDNSlocal mDNSu8 *PutResourceRecordTTL(DNSMessage *const msg, mDNSu8 *ptr, mDNSu16 *count, ResourceRecord *rr, mDNSu32 ttl)
	{
	mDNSu8 *endofrdata;
	mDNSu16 actualLength;
	const mDNSu8 *limit = msg->data + AbsoluteMaxDNSMessageData;
	
	// If we have a single large record to put in the packet, then we allow the packet to be up to 9K bytes,
	// but in the normal case we try to keep the packets below 1500 to avoid IP fragmentation on standard Ethernet
	if (msg->h.numAnswers || msg->h.numAuthorities || msg->h.numAdditionals)
		limit = msg->data + NormalMaxDNSMessageData;

	if (rr->RecordType == kDNSRecordTypeUnregistered)
		{
		debugf("PutResourceRecord ERROR! Attempt to put kDNSRecordTypeUnregistered");
		return(ptr);
		}

	ptr = putDomainNameAsLabels(msg, ptr, limit, &rr->name);
	if (!ptr || ptr + 10 >= limit) return(mDNSNULL);	// If we're out-of-space, return mDNSNULL
	ptr[0] = (mDNSu8)(rr->rrtype  >> 8);
	ptr[1] = (mDNSu8)(rr->rrtype      );
	ptr[2] = (mDNSu8)(rr->rrclass >> 8);
	ptr[3] = (mDNSu8)(rr->rrclass     );
	ptr[4] = (mDNSu8)(ttl >> 24);
	ptr[5] = (mDNSu8)(ttl >> 16);
	ptr[6] = (mDNSu8)(ttl >>  8);
	ptr[7] = (mDNSu8)(ttl      );
	endofrdata = putRData(msg, ptr+10, limit, rr->rrtype, rr->rdata);
	if (!endofrdata) { debugf("Ran out of space in PutResourceRecord!"); return(mDNSNULL); }

	// Go back and fill in the actual number of data bytes we wrote
	// (actualLength can be less than rdlength when domain name compression is used)
	actualLength = (mDNSu16)(endofrdata - ptr - 10);
	ptr[8] = (mDNSu8)(actualLength >> 8);
	ptr[9] = (mDNSu8)(actualLength     );

	(*count)++;
	return(endofrdata);
	}

#define PutResourceRecord(MSG, P, C, RR) PutResourceRecordTTL((MSG), (P), (C), (RR), (RR)->rrremainingttl)

mDNSlocal mDNSu8 *PutResourceRecordCappedTTL(DNSMessage *const msg, mDNSu8 *ptr, mDNSu16 *count, ResourceRecord *rr, mDNSu32 maxttl)
	{
	if (maxttl > rr->rrremainingttl) maxttl = rr->rrremainingttl;
	return(PutResourceRecordTTL(msg, ptr, count, rr, maxttl));
	}

#if 0
mDNSlocal mDNSu8 *putEmptyResourceRecord(DNSMessage *const msg, mDNSu8 *ptr, const mDNSu8 *const limit,
	mDNSu16 *count, const ResourceRecord *rr)
	{
	ptr = putDomainNameAsLabels(msg, ptr, limit, &rr->name);
	if (!ptr || ptr + 10 > limit) return(mDNSNULL);		// If we're out-of-space, return mDNSNULL
	ptr[0] = (mDNSu8)(rr->rrtype  >> 8);				// Put type
	ptr[1] = (mDNSu8)(rr->rrtype      );
	ptr[2] = (mDNSu8)(rr->rrclass >> 8);				// Put class
	ptr[3] = (mDNSu8)(rr->rrclass     );
	ptr[4] = ptr[5] = ptr[6] = ptr[7] = 0;				// TTL is zero
	ptr[8] = ptr[9] = 0;								// RDATA length is zero
	(*count)++;
	return(ptr + 10);
	}
#endif

mDNSlocal mDNSu8 *putQuestion(DNSMessage *const msg, mDNSu8 *ptr, const mDNSu8 *const limit,
							const domainname *const name, mDNSu16 rrtype, mDNSu16 rrclass)
	{
	ptr = putDomainNameAsLabels(msg, ptr, limit, name);
	if (!ptr || ptr+4 >= limit) return(mDNSNULL);			// If we're out-of-space, return mDNSNULL
	ptr[0] = (mDNSu8)(rrtype  >> 8);
	ptr[1] = (mDNSu8)(rrtype      );
	ptr[2] = (mDNSu8)(rrclass >> 8);
	ptr[3] = (mDNSu8)(rrclass     );
	msg->h.numQuestions++;
	return(ptr+4);
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - DNS Message Parsing Functions
#endif

mDNSlocal const mDNSu8 *skipDomainName(const DNSMessage *const msg, const mDNSu8 *ptr, const mDNSu8 *const end)
	{
	mDNSu16 total = 0;

	if (ptr < (mDNSu8*)msg || ptr >= end)
		{ debugf("skipDomainName: Illegal ptr not within packet boundaries"); return(mDNSNULL); }

	while (1)						// Read sequence of labels
		{
		const mDNSu8 len = *ptr++;	// Read length of this label
		if (len == 0) return(ptr);	// If length is zero, that means this name is complete
		switch (len & 0xC0)
			{
			case 0x00:	if (ptr + len >= end)					// Remember: expect at least one more byte for the root label
							{ debugf("skipDomainName: Malformed domain name (overruns packet end)"); return(mDNSNULL); }
						if (total + 1 + len >= MAX_DOMAIN_NAME)	// Remember: expect at least one more byte for the root label
							{ debugf("skipDomainName: Malformed domain name (more than 255 characters)"); return(mDNSNULL); }
						ptr += len;
						total += 1 + len;
						break;

			case 0x40:	debugf("skipDomainName: Extended EDNS0 label types 0x%X not supported", len); return(mDNSNULL);
			case 0x80:	debugf("skipDomainName: Illegal label length 0x%X", len); return(mDNSNULL);
			case 0xC0:	return(ptr+1);
			}
		}
	}

// Routine to fetch an FQDN from the DNS message, following compression pointers if necessary.
mDNSlocal const mDNSu8 *getDomainName(const DNSMessage *const msg, const mDNSu8 *ptr, const mDNSu8 *const end,
	domainname *const name)
	{
	const mDNSu8 *nextbyte = mDNSNULL;					// Record where we got to before we started following pointers
	mDNSu8       *np = name->c;							// Name pointer
	const mDNSu8 *const limit = np + MAX_DOMAIN_NAME;	// Limit so we don't overrun buffer

	if (ptr < (mDNSu8*)msg || ptr >= end)
		{ debugf("getDomainName: Illegal ptr not within packet boundaries"); return(mDNSNULL); }

	*np = 0;						// Tentatively place the root label here (may be overwritten if we have more labels)

	while (1)						// Read sequence of labels
		{
		const mDNSu8 len = *ptr++;	// Read length of this label
		if (len == 0) break;		// If length is zero, that means this name is complete
		switch (len & 0xC0)
			{
			int i;
			mDNSu16 offset;

			case 0x00:	if (ptr + len >= end)		// Remember: expect at least one more byte for the root label
							{ debugf("getDomainName: Malformed domain name (overruns packet end)"); return(mDNSNULL); }
						if (np + 1 + len >= limit)	// Remember: expect at least one more byte for the root label
							{ debugf("getDomainName: Malformed domain name (more than 255 characters)"); return(mDNSNULL); }
						*np++ = len;
						for (i=0; i<len; i++) *np++ = *ptr++;
						*np = 0;	// Tentatively place the root label here (may be overwritten if we have more labels)
						break;

			case 0x40:	debugf("getDomainName: Extended EDNS0 label types 0x%X not supported in name %##s", len, name->c);
						return(mDNSNULL);

			case 0x80:	debugf("getDomainName: Illegal label length 0x%X in domain name %##s", len, name->c); return(mDNSNULL);

			case 0xC0:	offset = (mDNSu16)((((mDNSu16)(len & 0x3F)) << 8) | *ptr++);
						if (!nextbyte) nextbyte = ptr;	// Record where we got to before we started following pointers
						ptr = (mDNSu8 *)msg + offset;
						if (ptr < (mDNSu8*)msg || ptr >= end)
							{ debugf("getDomainName: Illegal compression pointer not within packet boundaries"); return(mDNSNULL); }
						if (*ptr & 0xC0)
							{ debugf("getDomainName: Compression pointer must point to real label"); return(mDNSNULL); }
						break;
			}
		}
	
	if (nextbyte) return(nextbyte);
	else return(ptr);
	}

mDNSlocal const mDNSu8 *skipResourceRecord(const DNSMessage *msg, const mDNSu8 *ptr, const mDNSu8 *end)
	{
	mDNSu16 pktrdlength;

	ptr = skipDomainName(msg, ptr, end);
	if (!ptr) { debugf("skipResourceRecord: Malformed RR name"); return(mDNSNULL); }
	
	if (ptr + 10 > end) { debugf("skipResourceRecord: Malformed RR -- no type/class/ttl/len!"); return(mDNSNULL); }
	pktrdlength = (mDNSu16)((mDNSu16)ptr[8] <<  8 | ptr[9]);
	ptr += 10;
	if (ptr + pktrdlength > end) { debugf("skipResourceRecord: RDATA exceeds end of packet"); return(mDNSNULL); }

	return(ptr + pktrdlength);
	}

mDNSlocal const mDNSu8 *GetResourceRecord(mDNS *const m, const DNSMessage *msg, const mDNSu8 *ptr, const mDNSu8 *end,
	const mDNSInterfaceID InterfaceID, mDNSu8 RecordType, ResourceRecord *rr, RData *RDataStorage)
	{
	mDNSu16 pktrdlength;

	rr->next              = mDNSNULL;
	
	// Field Group 1: Persistent metadata for Authoritative Records
	rr->Additional1       = mDNSNULL;
	rr->Additional2       = mDNSNULL;
	rr->DependentOn       = mDNSNULL;
	rr->RRSet             = mDNSNULL;
	rr->RecordCallback    = mDNSNULL;
	rr->RecordContext     = mDNSNULL;
	rr->RecordType        = RecordType;
	rr->HostTarget        = mDNSfalse;

	// Field Group 2: Transient state for Authoritative Records
	rr->RRInterfaceActive = mDNSfalse;
	rr->Acknowledged      = mDNSfalse;
	rr->ProbeCount        = 0;
	rr->AnnounceCount     = 0;
	rr->IncludeInProbe    = mDNSfalse;
	rr->ImmedAnswer       = mDNSNULL;
	rr->ImmedAdditional   = mDNSNULL;
	rr->SendRNow          = mDNSNULL;
	rr->v4Requester       = zeroIPAddr;
	rr->v6Requester       = zerov6Addr;
	rr->NextResponse      = mDNSNULL;
	rr->NR_AnswerTo       = mDNSNULL;
	rr->NR_AdditionalTo   = mDNSNULL;
	rr->ThisAPInterval    = 0;
	rr->LastAPTime        = 0;
	rr->NewRData          = mDNSNULL;
	rr->UpdateCallback    = mDNSNULL;

	// Field Group 3: Transient state for Cache Records
	rr->NextInKAList      = mDNSNULL;
	rr->TimeRcvd          = m->timenow;
	rr->LastUsed          = m->timenow;
	rr->UseCount          = 0;
	rr->CRActiveQuestion  = mDNSNULL;
	rr->UnansweredQueries = 0;
	rr->NewData           = mDNStrue;

	// Field Group 4: The actual information pertaining to this resource record
	rr->InterfaceID       = InterfaceID;
	ptr = getDomainName(msg, ptr, end, &rr->name);
	if (!ptr) { debugf("GetResourceRecord: Malformed RR name"); return(mDNSNULL); }

	if (ptr + 10 > end) { debugf("GetResourceRecord: Malformed RR -- no type/class/ttl/len!"); return(mDNSNULL); }
	
	rr->rrtype            = (mDNSu16) ((mDNSu16)ptr[0] <<  8 | ptr[1]);
	rr->rrclass           = (mDNSu16)(((mDNSu16)ptr[2] <<  8 | ptr[3]) & kDNSClass_Mask);
	rr->rroriginalttl     = (mDNSu32) ((mDNSu32)ptr[4] << 24 | (mDNSu32)ptr[5] << 16 | (mDNSu32)ptr[6] << 8 | ptr[7]);
	if (rr->rroriginalttl > 0x70000000UL / mDNSPlatformOneSecond)
		rr->rroriginalttl = 0x70000000UL / mDNSPlatformOneSecond;
	// Note: We don't have to adjust m->NextCacheCheck here -- this is just getting a record into memory for
	// us to look at. If we decide to copy it into the cache, then we'll update m->NextCacheCheck accordingly.
	rr->rrremainingttl    = 0;
	pktrdlength           = (mDNSu16)((mDNSu16)ptr[8] <<  8 | ptr[9]);
	if (ptr[2] & (kDNSClass_UniqueRRSet >> 8))
		rr->RecordType |= kDNSRecordTypeUniqueMask;
	ptr += 10;
	if (ptr + pktrdlength > end) { debugf("GetResourceRecord: RDATA exceeds end of packet"); return(mDNSNULL); }

	if (RDataStorage)
		rr->rdata = RDataStorage;
	else
		{
		rr->rdata = &rr->rdatastorage;
		rr->rdata->MaxRDLength = sizeof(RDataBody);
		}

	switch (rr->rrtype)
		{
		case kDNSType_A:	rr->rdata->u.ip.b[0] = ptr[0];
							rr->rdata->u.ip.b[1] = ptr[1];
							rr->rdata->u.ip.b[2] = ptr[2];
							rr->rdata->u.ip.b[3] = ptr[3];
							break;

		case kDNSType_CNAME:// CNAME is same as PTR
		case kDNSType_PTR:	if (!getDomainName(msg, ptr, end, &rr->rdata->u.name))
								{ debugf("GetResourceRecord: Malformed CNAME/PTR RDATA name"); return(mDNSNULL); }
							//debugf("%##s PTR %##s rdlen %d", rr->name.c, rr->rdata->u.name.c, pktrdlength);
							break;

		case kDNSType_TXT:  if (pktrdlength > rr->rdata->MaxRDLength)
								{
								debugf("GetResourceRecord: TXT rdata size (%d) exceeds storage (%d)",
									pktrdlength, rr->rdata->MaxRDLength);
								return(mDNSNULL);
								}
							rr->rdata->RDLength = pktrdlength;
							mDNSPlatformMemCopy(ptr, rr->rdata->u.data, pktrdlength);
							break;

		case kDNSType_AAAA:	mDNSPlatformMemCopy(ptr, &rr->rdata->u.ipv6, sizeof(rr->rdata->u.ipv6));
							break;

		case kDNSType_SRV:	rr->rdata->u.srv.priority = (mDNSu16)((mDNSu16)ptr[0] <<  8 | ptr[1]);
							rr->rdata->u.srv.weight   = (mDNSu16)((mDNSu16)ptr[2] <<  8 | ptr[3]);
							rr->rdata->u.srv.port.b[0] = ptr[4];
							rr->rdata->u.srv.port.b[1] = ptr[5];
							if (!getDomainName(msg, ptr+6, end, &rr->rdata->u.srv.target))
								{ debugf("GetResourceRecord: Malformed SRV RDATA name"); return(mDNSNULL); }
							//debugf("%##s SRV %##s rdlen %d", rr->name.c, rr->rdata->u.srv.target.c, pktrdlength);
							break;

		default:			if (pktrdlength > rr->rdata->MaxRDLength)
								{
								debugf("GetResourceRecord: rdata %d size (%d) exceeds storage (%d)",
									rr->rrtype, pktrdlength, rr->rdata->MaxRDLength);
								return(mDNSNULL);
								}
							debugf("GetResourceRecord: Warning! Reading resource type %d as opaque data", rr->rrtype);
							// Note: Just because we don't understand the record type, that doesn't
							// mean we fail. The DNS protocol specifies rdlength, so we can
							// safely skip over unknown records and ignore them.
							// We also grab a binary copy of the rdata anyway, since the caller
							// might know how to interpret it even if we don't.
							rr->rdata->RDLength = pktrdlength;
							mDNSPlatformMemCopy(ptr, rr->rdata->u.data, pktrdlength);
							break;
		}

	rr->rdata->RDLength   = GetRDLength(rr, mDNSfalse);
	rr->rdestimate        = GetRDLength(rr, mDNStrue);
	return(ptr + pktrdlength);
	}

mDNSlocal const mDNSu8 *skipQuestion(const DNSMessage *msg, const mDNSu8 *ptr, const mDNSu8 *end)
	{
	ptr = skipDomainName(msg, ptr, end);
	if (!ptr) { debugf("skipQuestion: Malformed domain name in DNS question section"); return(mDNSNULL); }
	if (ptr+4 > end) { debugf("skipQuestion: Malformed DNS question section -- no query type and class!"); return(mDNSNULL); }
	return(ptr+4);
	}

mDNSlocal const mDNSu8 *getQuestion(const DNSMessage *msg, const mDNSu8 *ptr, const mDNSu8 *end, const mDNSInterfaceID InterfaceID,
	DNSQuestion *question)
	{
	question->InterfaceID = InterfaceID;
	ptr = getDomainName(msg, ptr, end, &question->name);
	if (!ptr) { debugf("Malformed domain name in DNS question section"); return(mDNSNULL); }
	if (ptr+4 > end) { debugf("Malformed DNS question section -- no query type and class!"); return(mDNSNULL); }
	
	question->rrtype  = (mDNSu16)((mDNSu16)ptr[0] << 8 | ptr[1]);			// Get type
	question->rrclass = (mDNSu16)((mDNSu16)ptr[2] << 8 | ptr[3]);			// and class
	return(ptr+4);
	}

mDNSlocal const mDNSu8 *LocateAnswers(const DNSMessage *const msg, const mDNSu8 *const end)
	{
	int i;
	const mDNSu8 *ptr = msg->data;
	for (i = 0; i < msg->h.numQuestions && ptr; i++) ptr = skipQuestion(msg, ptr, end);
	return(ptr);
	}

mDNSlocal const mDNSu8 *LocateAuthorities(const DNSMessage *const msg, const mDNSu8 *const end)
	{
	int i;
	const mDNSu8 *ptr = LocateAnswers(msg, end);
	for (i = 0; i < msg->h.numAnswers && ptr; i++) ptr = skipResourceRecord(msg, ptr, end);
	return(ptr);
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark -
#pragma mark - Packet Sending Functions
#endif

mDNSlocal mStatus mDNSSendDNSMessage(const mDNS *const m, DNSMessage *const msg, const mDNSu8 *const end,
	mDNSInterfaceID InterfaceID, mDNSIPPort srcport, const mDNSAddr *dst, mDNSIPPort dstport)
	{
	mStatus status;
	mDNSu16 numQuestions   = msg->h.numQuestions;
	mDNSu16 numAnswers     = msg->h.numAnswers;
	mDNSu16 numAuthorities = msg->h.numAuthorities;
	mDNSu16 numAdditionals = msg->h.numAdditionals;
	
	// Put all the integer values in IETF byte-order (MSB first, LSB second)
	mDNSu8 *ptr = (mDNSu8 *)&msg->h.numQuestions;
	*ptr++ = (mDNSu8)(numQuestions   >> 8);
	*ptr++ = (mDNSu8)(numQuestions       );
	*ptr++ = (mDNSu8)(numAnswers     >> 8);
	*ptr++ = (mDNSu8)(numAnswers         );
	*ptr++ = (mDNSu8)(numAuthorities >> 8);
	*ptr++ = (mDNSu8)(numAuthorities     );
	*ptr++ = (mDNSu8)(numAdditionals >> 8);
	*ptr++ = (mDNSu8)(numAdditionals     );
	
	// Send the packet on the wire
	status = mDNSPlatformSendUDP(m, msg, end, InterfaceID, srcport, dst, dstport);
	
	// Put all the integer values back the way they were before we return
	msg->h.numQuestions   = numQuestions;
	msg->h.numAnswers     = numAnswers;
	msg->h.numAuthorities = numAuthorities;
	msg->h.numAdditionals = numAdditionals;

	return(status);
	}

mDNSlocal void CompleteDeregistration(mDNS *const m, ResourceRecord *rr)
	{
	// Setting AnnounceCount to InitialAnnounceCount signals mDNS_Deregister_internal()
	// that it should go ahead and immediately dispose of this registration
	rr->RecordType    = kDNSRecordTypeShared;
	rr->AnnounceCount = InitialAnnounceCount;
	mDNS_Deregister_internal(m, rr, mDNS_Dereg_normal);
	}

// NOTE: DiscardDeregistrations calls mDNS_Deregister_internal which can call a user callback, which may change
// the record list and/or question list.
// Any code walking either list must use the CurrentQuestion and/or CurrentRecord mechanism to protect against this.
mDNSlocal void DiscardDeregistrations(mDNS *const m)
	{
	if (m->CurrentRecord) debugf("DiscardDeregistrations ERROR m->CurrentRecord already set");
	m->CurrentRecord = m->ResourceRecords;
	
	while (m->CurrentRecord)
		{
		ResourceRecord *rr = m->CurrentRecord;
		m->CurrentRecord = rr->next;
		if (rr->RecordType == kDNSRecordTypeDeregistering)
			CompleteDeregistration(m, rr);
		}
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
	ResourceRecord *rr, *r2;
	mDNSs32 maxExistingAnnounceInterval = 0;
	const NetworkInterfaceInfo *intf = GetFirstActiveInterface(m->HostInterfaces);

	m->NextResponseTime = m->timenow + 0x78000000;

	// ***
	// *** 1. Setup: Set the SendRNow and ImmedAnswer fields to indicate which interface(s) the records need to be sent on
	// ***

	// Run through our list of records, and decide which ones we're going to announce on all interfaces
	for (rr = m->ResourceRecords; rr; rr=rr->next)
		if (TimeToAnnounceThisRecord(rr, m->timenow) && ResourceRecordIsValidAnswer(rr))
			{
			rr->ImmedAnswer = mDNSInterfaceMark;		// Send on all interfaces
			if (maxExistingAnnounceInterval < rr->ThisAPInterval)
				maxExistingAnnounceInterval = rr->ThisAPInterval;
			}

	// Any interface-specific records we're going to send are marked as being sent on all appropriate interfaces (which is just one)
	// Eligible records that are more than half-way to their announcement time are accelerated
	for (rr = m->ResourceRecords; rr; rr=rr->next)
		if ((rr->InterfaceID && rr->ImmedAnswer) ||
			(rr->ThisAPInterval <= maxExistingAnnounceInterval &&
			TimeToAnnounceThisRecord(rr, m->timenow + rr->ThisAPInterval/2) &&
			ResourceRecordIsValidAnswer(rr)))
			rr->ImmedAnswer = mDNSInterfaceMark;		// Send on all interfaces

	// If there's a record which is supposed to be unique that we're going to send, then make sure that we give
	// the whole RRSet as an atomic unit. That means that if we have any other records with the same name/type/class
	// then we need to mark them for sending too. Otherwise, if we set the kDNSClass_UniqueRRSet bit on a
	// record, then other RRSet members that have not been sent recently will get flushed out of client caches.
	// -- If a record is marked to be sent on a certain interface, make sure the whole set is marked to be sent on that interface
	// -- If any record is marked to be sent on all interfaces, make sure the whole set is marked to be sent on all interfaces
	for (rr = m->ResourceRecords; rr; rr=rr->next)
		if ((rr->RecordType & kDNSRecordTypeUniqueMask) && rr->ImmedAnswer)
			for (r2 = m->ResourceRecords; r2; r2=r2->next)
				if (r2->ImmedAnswer != mDNSInterfaceMark && r2->ImmedAnswer != rr->ImmedAnswer && SameResourceRecordSignature(r2, rr))
					r2->ImmedAnswer = rr->ImmedAnswer;

	// Now set SendRNow state appropriately
	for (rr = m->ResourceRecords; rr; rr=rr->next)
		{
		if (!rr->RRInterfaceActive)						// Interface inactive:
			{
			rr->ImmedAnswer     = mDNSNULL;				// Can't send this record
			rr->ImmedAdditional = mDNSNULL;
			}
		else if (rr->ImmedAnswer == mDNSInterfaceMark)	// Sending this record on all appropriate interfaces
			{
			rr->SendRNow = (rr->InterfaceID) ? rr->InterfaceID : intf->InterfaceID;
			rr->ImmedAdditional = mDNSNULL;				// No need to send as additional if sending as answer
			// If we're announcing this record, and it's at least half-way to its ordained time, then consider this annoucement done
			if (TimeToAnnounceThisRecord(rr, m->timenow + rr->ThisAPInterval/2))
				{
				rr->AnnounceCount--;
				rr->ThisAPInterval *= 2;
				rr->LastAPTime = m->timenow;
				verbosedebugf("%##s %d", rr->name.c, rr->AnnounceCount);
				}
			}
		else if (rr->ImmedAnswer)						// Else, just respond to a single query on single interface:
			{
			rr->SendRNow = rr->ImmedAnswer;				// Just reply on that interface
			rr->ImmedAdditional = mDNSNULL;				// No need to send as additional too
			}
		SetNextAnnounceProbeTime(m, rr);
		}

	// ***
	// *** 2. Loop through interface list, sending records as appropriate
	// ***

	while (intf)
		{
		int numDereg    = 0;
		int numAnnounce = 0;
		int numAnswer   = 0;
		DNSMessage response;
		mDNSu8 *responseptr = response.data;
		mDNSu8 *newptr;
		InitializeDNSMessage(&response.h, mDNS_MessageID, ResponseFlags);
	
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
				if (rr->RecordType == kDNSRecordTypeDeregistering)
					{
					newptr = PutResourceRecord(&response, responseptr, &response.h.numAnswers, rr);
					if (!newptr && response.h.numAnswers) break;
					numDereg++;
					responseptr = newptr;
					}
				else if (rr->NewRData)							// If we have new data for this record
					{
					RData *OldRData = rr->rdata;
					if (ResourceRecordIsValidAnswer(rr))		// First see if we have to de-register the old data
						{
						newptr = PutResourceRecordTTL(&response, responseptr, &response.h.numAnswers, rr, 0);
						if (!newptr && response.h.numAnswers) break;
						numDereg++;
						responseptr = newptr;
						}
					// Now try to see if we can fit the update in the same packet (not fatal if we can't)
					rr->rdata = rr->NewRData;
					newptr = PutResourceRecord(&response, responseptr, &response.h.numAnswers, rr);
					if (newptr) responseptr = newptr;
					rr->rdata = OldRData;
					}
				else
					{
					// If this record is supposed to be unique, see if we've sent its whole set
					if (rr->RecordType & kDNSRecordTypeUniqueMask)
						{
						// Try to find another member of this set that we're still planning to send on this interface
						const ResourceRecord *a;
						for (a = m->ResourceRecords; a; a=a->next)
							if (a != rr && SameResourceRecordSignature(a, rr) && a->SendRNow == intf->InterfaceID) break;
						if (a == mDNSNULL)							// If no more members of this set found
							rr->rrclass |= kDNSClass_UniqueRRSet;	// Temporarily set the cache flush bit so PutResourceRecord will set it
						}
					newptr = PutResourceRecordTTL(&response, responseptr, &response.h.numAnswers, rr, m->SleepState ? 0 : rr->rrremainingttl);
					rr->rrclass &= ~kDNSClass_UniqueRRSet;			// Make sure to clear cache flush bit back to normal state
					if (!newptr && response.h.numAnswers) break;
					if (rr->LastAPTime == m->timenow) numAnnounce++; else numAnswer++;
					responseptr = newptr;
					}
				// If sending on all interfaces, go to next interface; else we're finished now
				if (rr->ImmedAnswer == mDNSInterfaceMark && rr->InterfaceID == mDNSInterface_Any)
					rr->SendRNow = GetNextActiveInterfaceID(intf);
				else
					rr->SendRNow = mDNSNULL;
				}
	
		// Second Pass. Add additional records, if there's space.
		for (rr = m->ResourceRecords; rr; rr=rr->next)
			if (rr->ImmedAdditional == intf->InterfaceID)
				{
				if (ResourceRecordIsValidAnswer(rr) &&
					(newptr = PutResourceRecord(&response, responseptr, &response.h.numAdditionals, rr)))
						responseptr = newptr;
				// Since additionals are optional, we clear ImmedAdditional anyway, even if it didn't fit in the packet
				rr->ImmedAdditional = mDNSNULL;
				}
	
		if (response.h.numAnswers > 0)	// We *never* send a packet with only additionals in it
			{
			debugf("SendResponses: Sending %d Deregistration%s, %d Announcement%s, %d Answer%s, %d Additional%s on %p",
				numDereg,                  numDereg                  == 1 ? "" : "s",
				numAnnounce,               numAnnounce               == 1 ? "" : "s",
				numAnswer,                 numAnswer                 == 1 ? "" : "s",
				response.h.numAdditionals, response.h.numAdditionals == 1 ? "" : "s", intf->InterfaceID);
			mDNSSendDNSMessage(m, &response, responseptr, intf->InterfaceID, MulticastDNSPort, &AllDNSLinkGroup_v4, MulticastDNSPort);
			mDNSSendDNSMessage(m, &response, responseptr, intf->InterfaceID, MulticastDNSPort, &AllDNSLinkGroup_v6, MulticastDNSPort);
			if (++pktcount >= 1000)
				{ LogMsg("SendResponses exceeded loop limit %d: giving up", pktcount); break; }
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

	if (m->CurrentRecord) debugf("SendResponses: ERROR m->CurrentRecord already set");
	m->CurrentRecord = m->ResourceRecords;
	while (m->CurrentRecord)
		{
		rr = m->CurrentRecord;
		m->CurrentRecord = rr->next;

		if (rr->NewRData)
			{
			RData *OldRData = rr->rdata;
			rr->rdata = rr->NewRData;	// Update our rdata
			rr->NewRData = mDNSNULL;	// Clear the NewRData pointer ...
			if (rr->UpdateCallback)
				rr->UpdateCallback(m, rr, OldRData);	// ... and let the client know
			}

		if (rr->RecordType == kDNSRecordTypeDeregistering)
			CompleteDeregistration(m, rr);
		else
			{
			rr->ImmedAnswer     = mDNSNULL;
			rr->v4Requester     = zeroIPAddr;
			rr->v6Requester     = zerov6Addr;
			}
		}
	debugf("SendResponses: Next in %d ticks", m->NextResponseTime - m->timenow);
	}

#define MaxQuestionInterval         (3600 * mDNSPlatformOneSecond)

// BuildQuestion puts a question into a DNS Query packet and if successful, updates the value of queryptr.
// It also appends to the list of known answer records that need to be included,
// and updates the forcast for the size of the known answer section.
mDNSlocal mDNSBool BuildQuestion(mDNS *const m, DNSMessage *query, mDNSu8 **queryptr, DNSQuestion *q,
	ResourceRecord ***kalistptrptr, mDNSu32 *answerforecast)
	{
	const mDNSu8 *const limit = query->data + (query->h.numQuestions ? NormalMaxDNSMessageData : AbsoluteMaxDNSMessageData);
	mDNSu8 *newptr = putQuestion(query, *queryptr, limit, &q->name, q->rrtype, q->rrclass);
	if (!newptr)
		{
		debugf("BuildQuestion: No more space in this packet for question %##s", q->name.c);
		return(mDNSfalse);
		}
	else if (newptr + *answerforecast >= limit)
		{
		debugf("BuildQuestion: Retracting question %##s new forecast total %lu", q->name.c, newptr + *answerforecast - query->data);
		query->h.numQuestions--;
		return(mDNSfalse);
		}
	else
		{
		mDNSu32 forecast = *answerforecast;
		ResourceRecord *rr;
		ResourceRecord **ka = *kalistptrptr;	// Make a working copy of the pointer we're going to update

		for (rr=m->rrcache_hash[HashSlot(&q->name)]; rr; rr=rr->next)		// If we have a resource record in our cache,
			if (rr->InterfaceID == q->SendQNow &&							// received on this interface
				rr->NextInKAList == mDNSNULL && ka != &rr->NextInKAList &&	// which is not already in the known answer list
				ResourceRecordAnswersQuestion(rr, q))						// which answers our question, then add it
				{
				// Work out the latest time we should ask about this record to refresh it before it expires
				mDNSs32 onetenth = ((mDNSs32)rr->rroriginalttl * mDNSPlatformOneSecond) / 10;
				mDNSs32 expire = rr->TimeRcvd + (mDNSs32)rr->rroriginalttl * mDNSPlatformOneSecond;
				// If we're planning to ask again at least once before this records reaches
				// its 80% final expiration query, then we should suppress it this time.
				if ((expire - onetenth*2) - (m->timenow + q->ThisQInterval) > 0)
					{
					*ka = rr;	// Link this record into our known answer chain
					ka = &rr->NextInKAList;
					// We forecast: compressed name (2) type (2) class (2) TTL (4) rdlength (2) rdata (n)
					forecast += 12 + rr->rdestimate;
					// If we're trying to put more than one question in this packet, and it doesn't fit
					// then undo that last question and try again next time
					if (query->h.numQuestions > 1 && newptr + forecast >= limit)
						{
						debugf("BuildQuestion: Retracting question %##s new forecast total %lu", q->name.c, newptr + forecast - query->data);
						query->h.numQuestions--;
						ka = *kalistptrptr;		// Go back to where we started and retract these answer records
						while (*ka) { ResourceRecord *rr = *ka; *ka = mDNSNULL; ka = &rr->NextInKAList; }
						return(mDNSfalse);
						}
					}
				}

		// Success! Update our state pointers, increment UnansweredQueries as appropriate, and return
		*queryptr        = newptr;				// Update the packet pointer
		*answerforecast  = forecast;			// Update the forecast
		*kalistptrptr    = ka;					// Update the known answer list pointer

		for (rr=m->rrcache_hash[HashSlot(&q->name)]; rr; rr=rr->next)		// For every resource record in our cache,
			if (rr->InterfaceID == q->SendQNow &&							// received on this interface
				rr->NextInKAList == mDNSNULL && ka != &rr->NextInKAList &&	// which is not in the known answer list
				ResourceRecordAnswersQuestion(rr, q))						// which answers our question
					rr->UnansweredQueries++;								// indicate that we're expecting a response

		return(mDNStrue);
		}
	}

// How Standard Queries are generated:
// 1. The Question Section contains the question
// 2. The Additional Section contains answers we already know, to suppress duplicate replies

// How Probe Queries are generated:
// 1. The Question Section contains queries for the name we intend to use, with QType=ANY because
// if some other host is already using *any* records with this name, we want to know about it.
// 2. The Authority Section contains the proposed values we intend to use for one or more
// of our records with that name (analogous to the Update section of DNS Update packets)
// because if some other host is probing at the same time, we each want to know what the other is
// planning, in order to apply the tie-breaking rule to see who gets to use the name and who doesn't.

mDNSlocal void SendQueries(mDNS *const m)
	{
	int pktcount = 0;
	DNSQuestion *q;
	// For explanation of maxExistingQuestionInterval logic, see comments for maxExistingAnnounceInterval
	mDNSs32 maxExistingQuestionInterval = 0;
	const NetworkInterfaceInfo *intf = GetFirstActiveInterface(m->HostInterfaces);
	ResourceRecord *KnownAnswerList = mDNSNULL;

	// 1. If time for a query, work out what we need to do
	if (m->timenow - m->NextScheduledQuery >= 0)
		{
		m->NextScheduledQuery = m->timenow + 0x78000000;

		// Scan our list of questions to see which ones we're definitely going to send
		for (q = m->Questions; q; q=q->next)
			if (TimeToSendThisQuestion(q, m->timenow))
				{
				q->SendQNow = mDNSInterfaceMark;		// Mark this question for sending
				if (maxExistingQuestionInterval < q->ThisQInterval)
					maxExistingQuestionInterval = q->ThisQInterval;
				}
	
		// Scan our list of questions
		// (a) to see if there are any more that are worth accelerating, and
		// (b) to update the state variables for all the questions we're going to send
		for (q = m->Questions; q; q=q->next)
			{
			if (q->SendQNow || (q->ThisQInterval <= maxExistingQuestionInterval && TimeToSendThisQuestion(q, m->timenow + q->ThisQInterval/2)))
				{
				q->SendQNow = (q->InterfaceID) ? q->InterfaceID : intf->InterfaceID;
				// If at least halfway to next query time, advance to next interval
				// If less than halfway to next query time, treat this as logically a repeat of the last transmission, without advancing the interval
				if (m->timenow - (q->LastQTime + q->ThisQInterval/2) >= 0)
					{
					q->ThisQInterval *= 2;
					if (q->ThisQInterval > MaxQuestionInterval)
						q->ThisQInterval = MaxQuestionInterval;
					}
				q->LastQTime     = m->timenow;
				q->RecentAnswers = 0;
				}
			// For all questions (not just the ones we're sending) check with the next scheduled event will be
			SetNextQueryTime(m,q);
			}
		}

	// 2. Scan our authoritative RR list to see what probes we might need to send
	if (m->timenow - m->NextProbeTime >= 0)
		{
		m->NextProbeTime = m->timenow + 0x78000000;

		if (m->CurrentRecord) debugf("SendQueries: ERROR m->CurrentRecord already set");
		m->CurrentRecord = m->ResourceRecords;
		while (m->CurrentRecord)
			{
			ResourceRecord *rr = m->CurrentRecord;
			m->CurrentRecord = rr->next;
			if (rr->RecordType == kDNSRecordTypeUnique)			// For all records that are still probing...
				{
				// 1. If it's not reached its probe time, just make sure we update m->NextProbeTime correctly
				if (m->timenow - (rr->LastAPTime + rr->ThisAPInterval) < 0)
					{
					SetNextAnnounceProbeTime(m, rr);
					}
				// 2. else, if it has reached its probe time, mark it for sending and then update m->NextProbeTime correctly
				else if (rr->ProbeCount)
					{
					rr->SendRNow   = (rr->InterfaceID) ? rr->InterfaceID : intf->InterfaceID;
					rr->LastAPTime = m->timenow;
					rr->ProbeCount--;
					SetNextAnnounceProbeTime(m, rr);
					}
				// else, if it has now finished probing, move it to state Verified, and update m->NextResponseTime so it will be announced
				else
					{
					rr->RecordType     = kDNSRecordTypeVerified;
					rr->ThisAPInterval = DefaultAnnounceIntervalForTypeUnique;
					rr->LastAPTime     = m->timenow - DefaultAnnounceIntervalForTypeUnique;
					SetNextAnnounceProbeTime(m, rr);
					verbosedebugf("Probing for %##s (%s) complete", rr->name.c, DNSTypeName(rr->rrtype));
					if (!rr->Acknowledged && rr->RecordCallback)
						{
						// CAUTION: MUST NOT do anything more with rr after calling rr->Callback(), because the client's callback function
						// is allowed to do anything, including starting/stopping queries, registering/deregistering records, etc.
						rr->Acknowledged = mDNStrue;
						m->mDNS_reentrancy++; // Increment to allow client to legally make mDNS API calls from the callback
						rr->RecordCallback(m, rr, mStatus_NoError);
						m->mDNS_reentrancy--; // Decrement to block mDNS API calls again
						}
					}
				}
			}
		}

	// 3. Now we know which queries and probes we're sending, go through our interface list sending the appropriate queries on each interface
	while (intf)
		{
		ResourceRecord *rr;
		DNSMessage query;
		mDNSu8 *queryptr = query.data;
		InitializeDNSMessage(&query.h, mDNS_MessageID, QueryFlags);
		if (KnownAnswerList) debugf("SendQueries: KnownAnswerList set... Will continue from previous packet");
		if (!KnownAnswerList)
			{
			// Start a new known-answer list
			ResourceRecord **kalistptr = &KnownAnswerList;
			mDNSu32 answerforecast = 0;
			
			// Put query questions in this packet
			for (q = m->Questions; q; q=q->next)
				if (q->SendQNow == intf->InterfaceID)
					{
					verbosedebugf("SendQueries: Putting question for %##s at %lu forecast total %lu",
						q->name.c, queryptr - query.data, queryptr + answerforecast - query.data);
					// If we successfully put this question, update its SendQNow state
					if (BuildQuestion(m, &query, &queryptr, q, &kalistptr, &answerforecast))
						q->SendQNow = (q->InterfaceID) ? mDNSNULL : GetNextActiveInterfaceID(intf);
					}

			// Put probe questions in this packet
			for (rr = m->ResourceRecords; rr; rr=rr->next)
				if (rr->SendRNow == intf->InterfaceID)
					{
					const mDNSu8 *const limit = query.data + ((query.h.numQuestions) ? NormalMaxDNSMessageData : AbsoluteMaxDNSMessageData);
					mDNSu8 *newptr = putQuestion(&query, queryptr, limit, &rr->name, kDNSQType_ANY, rr->rrclass);
					// We forecast: compressed name (2) type (2) class (2) TTL (4) rdlength (2) rdata (n)
					mDNSu32 forecast = answerforecast + 12 + rr->rdestimate;
					if (newptr && newptr + forecast < limit)
						{
						queryptr       = newptr;
						answerforecast = forecast;
						rr->SendRNow = (rr->InterfaceID) ? mDNSNULL : GetNextActiveInterfaceID(intf);
						rr->IncludeInProbe = mDNStrue;
						verbosedebugf("SendQueries: Put Question %##s (%s) probecount %d", rr->name.c, DNSTypeName(rr->rrtype), rr->ProbeCount);
						}
					else
						{
						debugf("SendQueries: Retracting Question %##s (%s)", rr->name.c, DNSTypeName(rr->rrtype));
						query.h.numQuestions--;
						}
					}
				}

		// Put our known answer list (either new one from this question or questions, or remainder of old one from last time)
		while (KnownAnswerList)
			{
			ResourceRecord *rr = KnownAnswerList;
			mDNSu32 SecsSinceRcvd = ((mDNSu32)(m->timenow - rr->TimeRcvd)) / mDNSPlatformOneSecond;
			mDNSu8 *newptr = PutResourceRecordTTL(&query, queryptr, &query.h.numAnswers, rr, rr->rroriginalttl - SecsSinceRcvd);
			if (newptr)
				{
				verbosedebugf("SendQueries: Put %##s at %lu - %lu", rr->name.c, queryptr - query.data, newptr - query.data);
				queryptr = newptr;
				KnownAnswerList = rr->NextInKAList;
				rr->NextInKAList = mDNSNULL;
				}
			else
				{
				// If we ran out of space and we have more than one question in the packet, that's an error --
				// we shouldn't have put more than one question if there was a risk of us running out of space.
				if (query.h.numQuestions > 1)
					LogMsg("SendQueries: Put %d answers; No more space for known answers", query.h.numAnswers);
				query.h.flags.b[0] |= kDNSFlag0_TC;
				break;
				}
			}

		for (rr = m->ResourceRecords; rr; rr=rr->next)
			if (rr->IncludeInProbe)
				{
				mDNSu8 *newptr = PutResourceRecord(&query, queryptr, &query.h.numAuthorities, rr);
				rr->IncludeInProbe = mDNSfalse;
				if (newptr) queryptr = newptr;
				else LogMsg("SendQueries: How did we fail to have space for the Update record %##s (%s)?",
					rr->name.c, DNSTypeName(rr->rrtype));
				}
		
		if (queryptr > query.data)
			{
			verbosedebugf("SendQueries: Sending %d Question%s %d Answer%s %d Update%s on %p",
				query.h.numQuestions,   query.h.numQuestions   == 1 ? "" : "s",
				query.h.numAnswers,     query.h.numAnswers     == 1 ? "" : "s",
				query.h.numAuthorities, query.h.numAuthorities == 1 ? "" : "s", intf->InterfaceID);
			mDNSSendDNSMessage(m, &query, queryptr, intf->InterfaceID, MulticastDNSPort, &AllDNSLinkGroup_v4, MulticastDNSPort);
			mDNSSendDNSMessage(m, &query, queryptr, intf->InterfaceID, MulticastDNSPort, &AllDNSLinkGroup_v6, MulticastDNSPort);
			if (++pktcount >= 1000)
				{ LogMsg("SendQueries exceeded loop limit %d: giving up", pktcount); break; }
			// There might be more records left in the known answer list, or more questions to send
			// on this interface, so go around one more time and try again.
			}
		else	// Nothing more to send on this interface; go to next
			{
			const NetworkInterfaceInfo *next = GetFirstActiveInterface(intf->next);
			#if MDNS_DEBUGMSGS && 0
			const char *const msg = next ? "SendQueries: Nothing more on %p; moving to %p" : "SendQueries: Nothing more on %p";
			debugf(msg, intf, next);
			#endif
			intf = next;
			}
		}
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - RR List Management & Task Management
#endif

// NOTE: AnswerQuestionWithResourceRecord can call a user callback, which may change the record list and/or question list.
// Any code walking either list must use the CurrentQuestion and/or CurrentRecord mechanism to protect against this.
mDNSlocal void AnswerQuestionWithResourceRecord(mDNS *const m, DNSQuestion *q, ResourceRecord *rr)
	{
#if MDNS_DEBUGMSGS > 1
	if (rr->rrremainingttl)
		{
		if (rr->rrtype == kDNSType_TXT)
			debugf("AnswerQuestionWithResourceRecord Add %##s TXT %#.20s remaining ttl %lu",
				rr->name.c, rr->rdata->u.txt.c, rr->rrremainingttl);
		else
			debugf("AnswerQuestionWithResourceRecord Add %##s (%s) remaining ttl %lu",
				rr->name.c, DNSTypeName(rr->rrtype), rr->rrremainingttl);
		}
	else
		{
		if (rr->rrtype == kDNSType_TXT)
			debugf("AnswerQuestionWithResourceRecord Del %##s TXT %#.20s UnansweredQueries %lu",
				rr->name.c, rr->rdata->u.txt.c, rr->UnansweredQueries);
		else
			debugf("AnswerQuestionWithResourceRecord Del %##s (%s) UnansweredQueries %lu",
				rr->name.c, DNSTypeName(rr->rrtype), rr->UnansweredQueries);
		}
#endif

	rr->LastUsed = m->timenow;
	rr->UseCount++;
	if (ActiveQuestion(q)) rr->CRActiveQuestion = q;

	// CAUTION: MUST NOT do anything more with q after calling q->Callback(), because the client's callback function
	// is allowed to do anything, including starting/stopping queries, registering/deregistering records, etc.
	// Right now the only routines that call AnswerQuestionWithResourceRecord() are AnswerLocalQuestions() and
	// AnswerNewQuestion(), and both of them use the "m->CurrentQuestion" mechanism to protect against questions
	// being deleted out from under them.
	m->mDNS_reentrancy++; // Increment to allow client to legally make mDNS API calls from the callback
	if (q->QuestionCallback)
		q->QuestionCallback(m, q, rr);
	m->mDNS_reentrancy--; // Decrement to block mDNS API calls again
	}

// AnswerLocalQuestions is called from mDNSCoreReceiveResponse,
// and from CheckCacheExpiration, which is called from mDNS_Execute and from mDNSCoreReceiveResponse.
// AnswerLocalQuestions is *never* called directly as a result of a client API call.
// If new questions are created as a result of invoking client callbacks, they will be added to
// the end of the question list, and m->NewQuestions will be set to indicate the first new question.
// rr is either a new ResourceRecord just received into our cache,
// or an existing ResourceRecord that just expired and is being deleted.
// (kDNSRecordTypePacketAnswer/PacketAdditional/PacketUniqueAns/PacketUniqueAdd)
// NOTE: AnswerLocalQuestions calls AnswerQuestionWithResourceRecord which can call a user callback,
// which may change the record list and/or question list.
// Any code walking either list must use the CurrentQuestion and/or CurrentRecord mechanism to protect against this.
mDNSlocal void AnswerLocalQuestions(mDNS *const m, ResourceRecord *rr)
	{
	if (m->CurrentQuestion) debugf("AnswerLocalQuestions ERROR m->CurrentQuestion already set");
	m->CurrentQuestion = m->Questions;
	while (m->CurrentQuestion && m->CurrentQuestion != m->NewQuestions)
		{
		DNSQuestion *q = m->CurrentQuestion;
		m->CurrentQuestion = q->next;
		if (ResourceRecordAnswersQuestion(rr, q))
			{
			mDNSu32 SecsSinceRcvd = ((mDNSu32)(m->timenow - rr->TimeRcvd)) / mDNSPlatformOneSecond;
			if (rr->rroriginalttl <= SecsSinceRcvd)
				rr->rrremainingttl = 0;
			else
				{
				rr->rrremainingttl = rr->rroriginalttl - SecsSinceRcvd;
				// If this question is one that's actively sending queries,
				// and it's received two answers within one second of sending the query packet,
				// then reset its exponential backoff back to the start
				if (ActiveQuestion(q) && ++q->RecentAnswers > 1 && m->timenow - q->LastQTime < mDNSPlatformOneSecond)
					{
					verbosedebugf("AnswerLocalQuestions: %##s (%s) got immediate answer burst; restarting exponential backoff sequence",
						q->name.c, DNSTypeName(q->rrtype));
					q->ThisQInterval = mDNSPlatformOneSecond;
					SetNextQueryTime(m,q);
					}
				}
			AnswerQuestionWithResourceRecord(m, q, rr);
			// MUST NOT touch q again after calling AnswerQuestionWithResourceRecord()
			}
		}
	m->CurrentQuestion = mDNSNULL;
	}

mDNSlocal void AnswerNewQuestion(mDNS *const m)
	{
	mDNSu32 NumAnswersGiven = 0;
	ResourceRecord *rr;
	DNSQuestion *q = m->NewQuestions;		// Grab the question we're going to answer
	m->NewQuestions = q->next;				// Advance NewQuestions to the next (if any)

	verbosedebugf("AnswerNewQuestion: Answering %##s (%s)", q->name.c, DNSTypeName(q->rrtype));

	if (m->lock_rrcache) debugf("AnswerNewQuestion ERROR! Cache already locked!");
	// This should be safe, because calling the client's question callback may cause the
	// question list to be modified, but should not ever cause the rrcache list to be modified.
	// If the client's question callback deletes the question, then m->CurrentQuestion will
	// be advanced, and we'll exit out of the loop
	m->lock_rrcache = 1;
	if (m->CurrentQuestion) debugf("AnswerNewQuestion ERROR m->CurrentQuestion already set");
	m->CurrentQuestion = q;		// Indicate which question we're answering, so we'll know if it gets deleted
	for (rr=m->rrcache_hash[HashSlot(&q->name)]; rr && m->CurrentQuestion == q; rr=rr->next)
		if (ResourceRecordAnswersQuestion(rr, q))
			{
			mDNSu32 SecsSinceRcvd = ((mDNSu32)(m->timenow - rr->TimeRcvd)) / mDNSPlatformOneSecond;
			if (rr->rroriginalttl <= SecsSinceRcvd) rr->rrremainingttl = 0;
			else rr->rrremainingttl = rr->rroriginalttl - SecsSinceRcvd;
			
			// We only give positive responses to new questions.
			// Since this question is new, it has not received any answers yet, so there's no point
			// telling it about records that are going away that it never heard about in the first place.
			if (rr->rrremainingttl > 0)
				{
				NumAnswersGiven++;
				AnswerQuestionWithResourceRecord(m, q, rr);
				}
			// MUST NOT touch q again after calling AnswerQuestionWithResourceRecord()
			}
	if (NumAnswersGiven)
		debugf("Had   answer   for %##s already in our cache", q->name.c);
	else
		{
		debugf("Had no answers for %##s already in our cache; sending query packet", q->name.c);
		// This is safe because we only do it if NumAnswersGiven is zero,
		// in which case we never called AnswerQuestionWithResourceRecord
		q->LastQTime = m->timenow - q->ThisQInterval;
		m->NextScheduledQuery = m->timenow;
		}
	m->CurrentQuestion = mDNSNULL;
	m->lock_rrcache = 0;
	}

mDNSlocal void CheckCacheExpiration(mDNS *const m)
	{
	mDNSs32	slot;

	if (m->lock_rrcache) { debugf("CheckCacheExpiration ERROR! Cache already locked!"); return; }
	m->lock_rrcache = 1;

	m->NextCacheCheck = m->timenow + 0x78000000;

	for (slot = 0; slot < CACHE_HASH_SLOTS; slot++)
		{
		ResourceRecord **rp = &(m->rrcache_hash[slot]);
		while (*rp)
			{
			ResourceRecord *const rr = *rp;
			mDNSs32 expire = rr->TimeRcvd + (mDNSs32)rr->rroriginalttl * mDNSPlatformOneSecond;
			if (m->timenow - expire >= 0)	// If expired, delete it
				{
				*rp = rr->next;				// Cut it from the list
				verbosedebugf("CheckCacheExpiration: Deleting %##s (%s)", rr->name.c, DNSTypeName(rr->rrtype));
				AnswerLocalQuestions(m, rr);
				m->rrcache_used[slot]--;
				m->rrcache_totalused--;
				rr->next = m->rrcache_free;	// and move it back to the free list
				m->rrcache_free = rr;
				}
			else							// else, not expired; see if we need to query
				{
				DNSQuestion *q = rr->CRActiveQuestion;
				if (q && rr->UnansweredQueries < 2)
					{
					// rr->UnansweredQueries is either 0 or 1. We want to query at 80% or 90% respectively.
					mDNSs32 onetenth = ((mDNSs32)rr->rroriginalttl * mDNSPlatformOneSecond) / 10;
					mDNSs32 qt = expire - onetenth * (mDNSs32)(2 - rr->UnansweredQueries);

					// If due to query now, do it now
					if (m->timenow - qt >= 0)
						{
						q->SendQNow = mDNSInterfaceMark;	// Mark question for immediate sending
						m->NextScheduledQuery = m->timenow;	// And adjust NextScheduledQuery so it will happen
						qt += onetenth;						// Adjust qt for next expected NextCacheCheck event time after this
						}
					// Make sure NextCacheCheck is set correctly for next anticipated event
					// Note: Can't use the usual SetNextCacheCheckTime() call here because we want to compute what the next expected
					// event will be *after* the scheduled question has been sent out and has incremented rr->UnansweredQueries.
					if (m->NextCacheCheck - qt > 0)
						m->NextCacheCheck = qt;
					}
				if (m->NextCacheCheck - expire > 0)
					m->NextCacheCheck = expire;
				rp = &rr->next;
				}
			}
		}
	m->lock_rrcache = 0;
	}

mDNSlocal void SetNextCacheCheckTime(mDNS *const m, const ResourceRecord *const rr)
	{
	mDNSs32 event = rr->TimeRcvd + ((mDNSs32)rr->rroriginalttl * mDNSPlatformOneSecond);

	// If we have an active question, then see if we want to schedule an 80% or 90% query for this record
	if (rr->CRActiveQuestion && rr->UnansweredQueries < 2)
		{
		mDNSs32 onetenth = ((mDNSs32)rr->rroriginalttl * mDNSPlatformOneSecond) / 10;
		event -= onetenth * (2 - rr->UnansweredQueries);
		}

	if (m->NextCacheCheck - event > 0)
		m->NextCacheCheck = event;
	}

mDNSlocal ResourceRecord *GetFreeCacheRR(mDNS *const m)
	{
	ResourceRecord *r = m->rrcache_free;

	if (m->lock_rrcache) { debugf("GetFreeCacheRR ERROR! Cache already locked!"); return(mDNSNULL); }
	m->lock_rrcache = 1;
	
	if (r)		// If there are records in the free list, take one
		{
		m->rrcache_free = r->next;
		if (m->rrcache_totalused+1 >= m->rrcache_report)
			{
			debugf("RR Cache now using %ld records", m->rrcache_totalused+1);
			if (m->rrcache_report < 100) m->rrcache_report += 10;
			else                         m->rrcache_report += 100;
			}
		}
	else		// Else search for a candidate to recycle
		{
		mDNSs32 slot;
		ResourceRecord **rr;
		ResourceRecord **best = mDNSNULL;
		mDNSs32 bestage = -1;
		mDNSs32	bestslot = -1;

		for (slot = 0; slot < CACHE_HASH_SLOTS; slot++)
			{
			rr = &(m->rrcache_hash[slot]);
			while (*rr)
				{
				// Records that answer still-active questions are not candidates for deletion
				if (!(*rr)->CRActiveQuestion)
					{
					// Work out a weighted age, which is the number of seconds since this record was last used,
					// divided by the number of times it has been used (we want to keep frequently used records longer).
					mDNSs32 count = (*rr)->UseCount < 100 ? 1 + (mDNSs32)(*rr)->UseCount : 100;
					mDNSs32 age = (m->timenow - (*rr)->LastUsed) / count;
					mDNSu8 rtype = (mDNSu8)(((*rr)->RecordType) & ~kDNSRecordTypeUniqueMask);
					if (rtype == kDNSRecordTypePacketAnswer) age /= 2;					// Keep answer records longer than additionals
					if (bestage < age) { best = rr; bestage = age; bestslot = slot; }
					}

				rr=&(*rr)->next;
				}
			}

		if (best)
			{
			m->rrcache_used[bestslot]--;
			m->rrcache_totalused--;
			r = *best;							// Remember the record we chose
			*best = r->next;					// And detatch it from the free list
			}
		}

	m->lock_rrcache = 0;

	if (r) mDNSPlatformMemZero(r, sizeof(*r));
	return(r);
	}

mDNSlocal void PurgeCacheResourceRecord(mDNS *const m, ResourceRecord *rr)
	{
	// Make sure we mark this record as thoroughly expired -- we don't ever want to give
	// a positive answer using an expired record (e.g. from an interface that has gone away).
	// We don't want to clear CRActiveQuestion here, because that would leave the record subject to
	// summary deletion without giving the proper callback to any questions that are monitoring it.
	// By setting UnansweredQueries to 2 we ensure it won't trigger any further expiration queries.
	rr->TimeRcvd          = m->timenow - mDNSPlatformOneSecond * 60;
	rr->UnansweredQueries = 2;
	rr->rroriginalttl     = 0;
	
	// Set m->NextCacheCheck so that we will immediately clear these records
	m->NextCacheCheck = m->timenow;
	}

static mDNSs32 gNextEventScheduledAt;
static const char *gNextEventMsg = "None yet";

mDNSlocal mDNSs32 ScheduleNextTask(const mDNS *const m)
	{
	mDNSs32 nextevent = m->timenow + 0x78000000;
	const char *sign="";
	mDNSs32 interval, fraction;

	DNSQuestion *q;
	ResourceRecord *rr;
	
	gNextEventMsg = "No Event";
	
	if (m->mDNSPlatformStatus != mStatus_NoError)
		return(nextevent);
	
	// 1. If sleeping, do nothing
	if (m->SleepState)
		{ debugf("ScheduleNextTask: Sleeping"); gNextEventMsg = "Sleeping"; return(nextevent); }
	
	// 2. If we're suppressing sending right now, don't bother searching for packet generation events --
	// but do make sure we come back at the end of the suppression time to check again
	if (m->SuppressSending)
		{ verbosedebugf("ScheduleNextTask: Send Suppressed Packets"); gNextEventMsg = "Send Suppressed Packets"; return(m->SuppressSending); }

	// 3. Cache work to be done?
	if (nextevent - m->NextCacheCheck > 0)
		{ nextevent = m->NextCacheCheck; gNextEventMsg = "Check Cache"; }
	
	// 4. Scan list of active questions to see if we need to send any queries
	for (q = m->Questions; q; q=q->next)
		if (TimeToSendThisQuestion(q, nextevent))
			{ nextevent = q->LastQTime + q->ThisQInterval; gNextEventMsg = "Send Questions"; }

	// 5. Scan list of local resource records to see if we have any
	// deregistrations, probes, announcements, or replies to send
	for (rr = m->ResourceRecords; rr; rr=rr->next)
		{
		if (rr->RecordType == kDNSRecordTypeDeregistering)
			{ nextevent = m->timenow; gNextEventMsg = "Send Deregistrations"; }
		if (rr->RRInterfaceActive)
			{
			if (rr->RecordType == kDNSRecordTypeUnique && rr->LastAPTime + rr->ThisAPInterval - nextevent <= 0)
				{ nextevent = rr->LastAPTime + rr->ThisAPInterval; gNextEventMsg = "Send Probes"; }
			else if (rr->ImmedAnswer && ResourceRecordIsValidAnswer(rr))
				{ nextevent = m->timenow;                          gNextEventMsg = "Send Answers"; }
			else if (TimeToAnnounceThisRecord(rr,nextevent) && ResourceRecordIsValidAnswer(rr))
				{ nextevent = rr->LastAPTime + rr->ThisAPInterval; gNextEventMsg = "Send Announcements"; }
			}
		}

	interval = nextevent - m->timenow;
	if (interval < 0) { interval = -interval; sign = "-"; }
	fraction = interval % mDNSPlatformOneSecond;
	verbosedebugf("ScheduleNextTask: Next event: <%s> in %s%d.%03d seconds", gNextEventMsg, sign,
		interval / mDNSPlatformOneSecond, fraction * 1000 / mDNSPlatformOneSecond);

	return(nextevent);
	}

mDNSlocal void mDNS_Lock(mDNS *const m)
	{
	// MUST grab the platform lock FIRST!
	mDNSPlatformLock(m);

	// Normally, mDNS_reentrancy is zero and so is mDNS_busy
	// However, when we call a client callback mDNS_busy is one, and we increment mDNS_reentrancy too
	// If that client callback does mDNS API calls, mDNS_reentrancy and mDNS_busy will both be one
	// If mDNS_busy != mDNS_reentrancy that's a bad sign
	if (m->mDNS_busy != m->mDNS_reentrancy)
		LogMsg("mDNS_Lock: Locking failure! mDNS_busy (%ld) != mDNS_reentrancy (%ld)", m->mDNS_busy, m->mDNS_reentrancy);

	// If this is an initial entry into the mDNSCore code, set m->timenow
	// else, if this is a re-entrant entry into the mDNSCore code, m->timenow should already be set
	if (m->mDNS_busy == 0)
		{
		if (m->timenow)
			LogMsg("mDNS_Lock: m->timenow already set (%ld/%ld)", m->timenow, mDNSPlatformTimeNow() - m->timenow);
		m->timenow = mDNSPlatformTimeNow();
		if (m->timenow == 0) m->timenow = 1;
		}
	else if (m->timenow == 0)
		{
		LogMsg("mDNS_Lock: m->mDNS_busy is %ld but m->timenow not set", m->mDNS_busy);
		m->timenow = mDNSPlatformTimeNow();
		if (m->timenow == 0) m->timenow = 1;
		}

	// Increment mDNS_busy so we'll recognise re-entrant calls
	m->mDNS_busy++;
	}

mDNSlocal void mDNS_Unlock_(mDNS *const m, mDNSBool clearNextScheduledEvent)
	{
	// Decrement mDNS_busy
	m->mDNS_busy--;
	
	// Check for locking failures
	if (m->mDNS_busy != m->mDNS_reentrancy)
		LogMsg("mDNS_Unlock: Locking failure! mDNS_busy (%ld) != mDNS_reentrancy (%ld)", m->mDNS_busy, m->mDNS_reentrancy);

	// If this is a final exit from the mDNSCore code, clear m->timenow
	if (m->mDNS_busy == 0)
		{
		// Set NextScheduledEvent to timenow, to signal to mDNS_Execute() that we immediately need
		// to re-evaluate what work is to be done
		// In future we can make this smarter. Right now, we assume that after any API call, or after
		// any packet reception, we always have to call mDNS_Execute() right away to handle it.
		// In future we should be more selective, and only adjust m->NextScheduledEvent if the packet or API
		// actually resulted in some future action sooner than previously planned, and in any case, only
		// adjust m->NextScheduledEvent to the time of the next event, not to timenow
		if (clearNextScheduledEvent)
			{
			m->NextScheduledEvent = m->timenow;
			gNextEventMsg = "Re-evaluate after mDNS_Unlock";
			gNextEventScheduledAt = 0;//Indicate that this is a special (non)event
			}
	
		if (m->timenow == 0) LogMsg("mDNS_Unlock: ERROR! m->timenow aready zero");
		m->timenow = 0;
		}

	// MUST release the platform lock LAST!
	mDNSPlatformUnlock(m);
	}

#define mDNS_Unlock(M) mDNS_Unlock_((M), mDNStrue)
#define mDNS_Unlock_noclear(M) mDNS_Unlock_((M), mDNSfalse)

mDNSexport mDNSs32 mDNS_Execute(mDNS *const m)
	{
	mDNSs32 nse;

	mDNS_Lock(m);

	if (m->timenow - m->NextScheduledEvent >= 0)
		{
		int i, didwork = 0;

		verbosedebugf("mDNS_Execute");
		if (m->CurrentQuestion) debugf("mDNS_Execute: ERROR! m->CurrentQuestion already set");

		// If we're past the probe suppression time, we can clear it
		if (m->SuppressProbes && m->timenow - m->SuppressProbes >= 0)
			{ m->SuppressProbes = 0; didwork |= 0x01; }

		// If it's been more than ten seconds since the last probe failure, we can clear the counter
		if (m->NumFailedProbes && m->timenow - m->ProbeFailTime >= mDNSPlatformOneSecond * 10)
			{ m->NumFailedProbes = 0; didwork |= 0x02; }
		
		// 1. See if we can answer any of our new local questions from the cache
		for (i=0; m->NewQuestions && i<1000; i++) { AnswerNewQuestion(m); didwork |= 0x04; }
		if (i >= 1000) debugf("mDNS_Execute: AnswerNewQuestion exceeded loop limit");
	
		// 2. See what packets we need to send
		if (m->mDNSPlatformStatus != mStatus_NoError || m->SleepState)
			{
			// If the platform code is currently non-operational,
			// then we'll just complete deregistrations immediately,
			// without waiting for the goodbye packet to be sent
			DiscardDeregistrations(m);
			didwork |= 0x08;
			}
		else if (m->SuppressSending == 0 || m->timenow - m->SuppressSending >= 0)
			{
			// If the platform code is ready, and we're not suppressing packet generation right now
			// then send our responses, probes, and questions.
			// We check the cache first, because there might be records close to expiring that trigger questions to refresh them
			// We send queries next, because there might be final-stage probes that complete their probing here, causing
			// them to advance to announcing state, and we want those to be included in any announcements we send out.
			// Finally, we send responses, including the previously mentioned records that just completed probing

			if (m->SuppressSending) didwork |= 0x10;
			m->SuppressSending = 0;

			if (m->rrcache_size && m->timenow - m->NextCacheCheck >= 0)
				{
				mDNSu32 used = m->rrcache_totalused;
				// Adjust NextScheduledQuery so that we can tell if CheckCacheExpiration triggers a query
				if (m->NextScheduledQuery == m->timenow)
					m->NextScheduledQuery = m->timenow-1;
				CheckCacheExpiration(m);
				// If the rrcache_totalused is unchanged, AND no immediate queries were generated, then CheckCacheExpiration did nothing.
				// This can happen legitimately -- e.g. we were expecting a record to expire in 10 seconds, but then we get a response that
				// revives it. If it happens a repeatedly in a tight loop though, that's a sign that the NextCacheCheck logic is broken.
				if (used == m->rrcache_totalused && m->NextScheduledQuery != m->timenow)
					{
					static mDNSs32 lastmsg = 0;
					if ((mDNSu32)(m->timenow - lastmsg) < (mDNSu32)mDNSPlatformOneSecond)	// Yes, this *is* supposed to be unsigned
						LogMsg("mDNS_Execute CheckCacheExpiration(m) did no work (%d); next in %ld ticks (this is benign if it happens only rarely)",
							(mDNSu32)(m->timenow - lastmsg), m->NextCacheCheck - m->timenow);
					lastmsg = m->timenow;
					}
				didwork |= 0x20;	// Set didwork anyway -- don't need to log two syslog messages
				}
	
			if (m->timenow - m->NextScheduledQuery >= 0 || m->timenow - m->NextProbeTime >= 0) { SendQueries(m); didwork |= 0x40; }
			if (m->timenow - m->NextScheduledQuery >= 0)
				{
				LogMsg("mDNS_Execute: SendQueries didn't send all its queries; will try again in one second");
				m->NextScheduledQuery = m->timenow + mDNSPlatformOneSecond;
				}
			if (m->timenow - m->NextProbeTime >= 0)
				{
				LogMsg("mDNS_Execute: SendQueries didn't send all its probes; will try again in one second");
				m->NextProbeTime = m->timenow + mDNSPlatformOneSecond;
				}

			if (m->timenow - m->NextResponseTime >= 0) { SendResponses(m); didwork |= 0x80; }
			if (m->timenow - m->NextResponseTime >= 0)
				{
				LogMsg("mDNS_Execute: SendResponses didn't send all its responses; will try again in one second");
				m->NextResponseTime = m->timenow + mDNSPlatformOneSecond;
				}
			}
	
		if (!didwork)	// If we did no work, find out if we should have done
			{
			static mDNSs32 lastmsg = 0;
			if ((mDNSu32)(m->timenow - lastmsg) < (mDNSu32)mDNSPlatformOneSecond)	// Yes, this *is* supposed to be unsigned
			if (gNextEventScheduledAt && m->timenow - m->NextScheduledEvent >= 0)
				LogMsg("************ mDNS_Execute had no work to do (Task was \"%s\" scheduled %ld ticks ago for %ld ticks ago)",
					gNextEventMsg, m->timenow - gNextEventScheduledAt, m->timenow - m->NextScheduledEvent);
			lastmsg = m->timenow;
			}
		else if (m->timenow - m->NextScheduledEvent < 0)		// Should not have had anything to do
			{
			if (didwork & 0x01) LogMsg("************ mDNS_Execute did SuppressProbes");
			if (didwork & 0x02) LogMsg("************ mDNS_Execute did NumFailedProbes");
			if (didwork & 0x04) LogMsg("************ mDNS_Execute did AnswerNewQuestion");
			if (didwork & 0x08) LogMsg("************ mDNS_Execute did DiscardDeregistrations");
			if (didwork & 0x10) LogMsg("************ mDNS_Execute did m->SuppressSending = 0");
			if (didwork & 0x20) LogMsg("************ mDNS_Execute did CheckCacheExpiration");
			if (didwork & 0x40) LogMsg("************ mDNS_Execute did SendQueries");
			if (didwork & 0x80) LogMsg("************ mDNS_Execute did SendResponses");
			}

		{
		mDNSs32 oldNextEvent = m->NextScheduledEvent;
		m->NextScheduledEvent = ScheduleNextTask(m);
		// If our NextScheduledEvent time hasn't changed, or it is in the past, log an error message
		if (m->NextScheduledEvent == oldNextEvent || m->NextScheduledEvent - m->timenow <= 0)
			LogMsg("************ mDNSCoreTask next task \"%s\" %ld ticks from last one, %ld ticks from now",
				gNextEventMsg, m->NextScheduledEvent - oldNextEvent, m->NextScheduledEvent - m->timenow);
		gNextEventScheduledAt = mDNSPlatformTimeNow();
		}

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

	nse = m->NextScheduledEvent;
	mDNS_Unlock_noclear(m);
	return(nse);
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
	ResourceRecord *rr;

	mDNS_Lock(m);

	m->SleepState = sleepstate;
	debugf("mDNSCoreMachineSleep: %s", sleepstate ? "Sleep" : "Wake");

	if (sleepstate)
		{
		// Mark all the records we need to deregister and send them
		for (rr = m->ResourceRecords; rr; rr=rr->next)
			if (rr->RecordType == kDNSRecordTypeShared && rr->AnnounceCount < InitialAnnounceCount)
				rr->ImmedAnswer = mDNSInterfaceMark;
		SendResponses(m);
		}
	else
		{
		DNSQuestion *q;
		mDNSs32	slot;

		// 1. Retrigger all our questions
		for (q = m->Questions; q; q=q->next)				// Scan our list of questions
			if (ActiveQuestion(q))
				{
				q->LastQTime     = m->timenow - mDNSPlatformOneSecond/2;
				q->ThisQInterval = mDNSPlatformOneSecond/2;	// MUST be > zero for an active question
				q->RecentAnswers = 0;
				m->NextScheduledQuery = m->timenow;
				}

		// 2. Re-validate our cache records
		m->NextCacheCheck  = m->timenow;
		for (slot = 0; slot < CACHE_HASH_SLOTS; slot++)
			for (rr = m->rrcache_hash[slot]; rr; rr=rr->next)
				{
				// If we don't get an answer for these in the next five seconds, assume they're gone
				rr->TimeRcvd          = m->timenow;
				rr->rroriginalttl     = 5;
				rr->UnansweredQueries = 0;
				}

		// 3. Retrigger probing and announcing for all our authoritative records
		for (rr = m->ResourceRecords; rr; rr=rr->next)
			{
			if (rr->RecordType == kDNSRecordTypeVerified && !rr->DependentOn) rr->RecordType = kDNSRecordTypeUnique;
			rr->ProbeCount        = DefaultProbeCountForRecordType(rr->RecordType);
			if (rr->AnnounceCount < ReannounceCount)
				rr->AnnounceCount = ReannounceCount;
			rr->ThisAPInterval    = DefaultAPIntervalForRecordType(rr->RecordType);
			rr->LastAPTime        = m->timenow - rr->ThisAPInterval;
			SetNextAnnounceProbeTime(m, rr);
			}

		}

	mDNS_Unlock(m);
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - Packet Reception Functions
#endif

mDNSlocal mDNSBool AddRecordToResponseList(ResourceRecord **nrp,
	ResourceRecord *rr, const mDNSu8 *answerto, ResourceRecord *additionalto)
	{
	if (rr->NextResponse == mDNSNULL && nrp != &rr->NextResponse)
		{
		*nrp = rr;
		rr->NR_AnswerTo     = answerto;
		rr->NR_AdditionalTo = additionalto;
		return(mDNStrue);
		}
	else debugf("AddRecordToResponseList: %##s (%s) already in list", rr->name.c, DNSTypeName(rr->rrtype));
	return(mDNSfalse);
	}

#define MustSendRecord(RR) ((RR)->NR_AnswerTo || (RR)->NR_AdditionalTo)

mDNSlocal mDNSu8 *GenerateUnicastResponse(const DNSMessage *const query, const mDNSu8 *const end,
	const mDNSInterfaceID InterfaceID, DNSMessage *const reply, ResourceRecord *ResponseRecords)
	{
	const mDNSu8    *const limit     = reply->data + sizeof(reply->data);
	const mDNSu8    *ptr             = query->data;
	mDNSu8          *responseptr     = reply->data;
	ResourceRecord  *rr;
	int i;

	// Initialize the response fields so we can answer the questions
	InitializeDNSMessage(&reply->h, query->h.id, ResponseFlags);

	// ***
	// *** 1. Write out the list of questions we are actually going to answer with this packet
	// ***
	for (i=0; i<query->h.numQuestions; i++)						// For each question...
		{
		DNSQuestion q;
		ptr = getQuestion(query, ptr, end, InterfaceID, &q);	// get the question...
		if (!ptr) return(mDNSNULL);

		for (rr=ResponseRecords; rr; rr=rr->NextResponse)		// and search our list of proposed answers
			{
			if (rr->NR_AnswerTo == ptr)							// If we're going to generate a record answering this question
				{												// then put the question in the question section
				responseptr = putQuestion(reply, responseptr, limit, &q.name, q.rrtype, q.rrclass);
				if (!responseptr) { debugf("GenerateUnicastResponse: Ran out of space for questions!"); return(mDNSNULL); }
				break;		// break out of the ResponseRecords loop, and go on to the next question
				}
			}
		}

	if (reply->h.numQuestions == 0) { debugf("GenerateUnicastResponse: ERROR! Why no questions?"); return(mDNSNULL); }

	// ***
	// *** 2. Write Answers
	// ***
	for (rr=ResponseRecords; rr; rr=rr->NextResponse)
		if (rr->NR_AnswerTo)
			{
			mDNSu8 *p = PutResourceRecordCappedTTL(reply, responseptr, &reply->h.numAnswers, rr, 10);
			if (p) responseptr = p;
			else { debugf("GenerateUnicastResponse: Ran out of space for answers!"); reply->h.flags.b[0] |= kDNSFlag0_TC; }
			}

	// ***
	// *** 3. Write Additionals
	// ***
	for (rr=ResponseRecords; rr; rr=rr->NextResponse)
		if (rr->NR_AdditionalTo && !rr->NR_AnswerTo)
			{
			mDNSu8 *p = PutResourceRecordCappedTTL(reply, responseptr, &reply->h.numAdditionals, rr, 10);
			if (p) responseptr = p;
			else debugf("GenerateUnicastResponse: No more space for additionals");
			}

	return(responseptr);
	}

// ResourceRecord *our is our ResourceRecord
// ResourceRecord *pkt is the ResourceRecord from the response packet we've witnessed on the network
// Returns 0 if there is no conflict
// Returns +1 if there was a conflict and we won
// Returns -1 if there was a conflict and we lost and have to rename
mDNSlocal int CompareRData(ResourceRecord *our, ResourceRecord *pkt)
	{
	mDNSu8 ourdata[256], *ourptr = ourdata, *ourend;
	mDNSu8 pktdata[256], *pktptr = pktdata, *pktend;
	if (!our) { debugf("CompareRData ERROR: our is NULL"); return(+1); }
	if (!pkt) { debugf("CompareRData ERROR: pkt is NULL"); return(+1); }

	ourend = putRData(mDNSNULL, ourdata, ourdata + sizeof(ourdata), our->rrtype, our->rdata);
	pktend = putRData(mDNSNULL, pktdata, pktdata + sizeof(pktdata), pkt->rrtype, pkt->rdata);
	while (ourptr < ourend && pktptr < pktend && *ourptr == *pktptr) { ourptr++; pktptr++; }
	if (ourptr >= ourend && pktptr >= pktend) return(0);			// If data identical, not a conflict

	if (ourptr >= ourend) return(-1);								// Our data ran out first; We lost
	if (pktptr >= pktend) return(+1);								// Packet data ran out first; We won
	if (*pktptr > *ourptr) return(-1);								// Our data is numerically lower; We lost
	if (*pktptr < *ourptr) return(+1);								// Packet data is numerically lower; We won
	
	debugf("CompareRData: How did we get here?");
	return(-1);
	}

// Find the canonical DependentOn record for this RR received in a packet.
// The DependentOn pointer is typically used for the TXT record of service registrations
// It indicates that there is no inherent conflict detection for the TXT record
// -- it depends on the SRV record to resolve name conflicts
// If we find any identical ResourceRecord in our authoritative list, then follow its DependentOn
// pointers (if any) to make sure we return the canonical DependentOn record
// If the record has no DependentOn, then just return that record's pointer
// Returns NULL if we don't have any local RRs that are identical to the one from the packet
mDNSlocal const ResourceRecord *FindDependentOn(const mDNS *const m, const ResourceRecord *const pktrr)
	{
	const ResourceRecord *rr;
	for (rr = m->ResourceRecords; rr; rr=rr->next)
		{
		if (IdenticalResourceRecordAnyInterface(rr, pktrr))
			{
			while (rr->DependentOn) rr = rr->DependentOn;
			return(rr);
			}
		}
	return(mDNSNULL);
	}

// Find the canonical RRSet pointer for this RR received in a packet.
// If we find any identical ResourceRecord in our authoritative list, then follow its RRSet
// pointers (if any) to make sure we return the canonical member of this name/type/class
// Returns NULL if we don't have any local RRs that are identical to the one from the packet
mDNSlocal const ResourceRecord *FindRRSet(const mDNS *const m, const ResourceRecord *const pktrr)
	{
	const ResourceRecord *rr;
	for (rr = m->ResourceRecords; rr; rr=rr->next)
		{
		if (IdenticalResourceRecordAnyInterface(rr, pktrr))
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
mDNSlocal mDNSBool PacketRRConflict(const mDNS *const m, const ResourceRecord *const our, const ResourceRecord *const pktrr)
	{
	const ResourceRecord *ourset = our->RRSet ? our->RRSet : our;

	// If not supposed to be unique, not a conflict
	if (!(our->RecordType & kDNSRecordTypeUniqueMask)) return(mDNSfalse);

	// If a dependent record, not a conflict
	if (our->DependentOn || FindDependentOn(m, pktrr) == our) return(mDNSfalse);

	// If the pktrr matches a member of ourset, not a conflict
	if (FindRRSet(m, pktrr) == ourset) return(mDNSfalse);

	// Okay, this is a conflict
	return(mDNStrue);
	}

// NOTE: ResolveSimultaneousProbe calls mDNS_Deregister_internal which can call a user callback, which may change
// the record list and/or question list.
// Any code walking either list must use the CurrentQuestion and/or CurrentRecord mechanism to protect against this.
mDNSlocal void ResolveSimultaneousProbe(mDNS *const m, const DNSMessage *const query, const mDNSu8 *const end,
	DNSQuestion *q, ResourceRecord *our)
	{
	int i;
	const mDNSu8 *ptr = LocateAuthorities(query, end);
	mDNSBool FoundUpdate = mDNSfalse;

	for (i = 0; i < query->h.numAuthorities; i++)
		{
		ResourceRecord pktrr;
		ptr = GetResourceRecord(m, query, ptr, end, q->InterfaceID, 0, &pktrr, mDNSNULL);
		if (!ptr) break;
		if (ResourceRecordAnswersQuestion(&pktrr, q))
			{
			FoundUpdate = mDNStrue;
			if (PacketRRConflict(m, our, &pktrr))
				{
				int result          = (int)our->rrclass - (int)pktrr.rrclass;
				if (!result) result = (int)our->rrtype  - (int)pktrr.rrtype;
				if (!result) result = CompareRData(our, &pktrr);
				switch (result)
					{
					case  1:	debugf("ResolveSimultaneousProbe: %##s (%s): We won",  our->name.c, DNSTypeName(our->rrtype));
								break;
					case  0:	break;
					case -1:	debugf("ResolveSimultaneousProbe: %##s (%s): We lost", our->name.c, DNSTypeName(our->rrtype));
								mDNS_Deregister_internal(m, our, mDNS_Dereg_conflict);
								return;
					}
				}
			}
		}
	if (!FoundUpdate)
		debugf("ResolveSimultaneousProbe: %##s (%s): No Update Record found", our->name.c, DNSTypeName(our->rrtype));
	}

// ProcessQuery examines a received query to see if we have any answers to give
mDNSlocal mDNSu8 *ProcessQuery(mDNS *const m, const DNSMessage *const query, const mDNSu8 *const end,
	const mDNSAddr *srcaddr, const mDNSInterfaceID InterfaceID,
	DNSMessage *const replyunicast, mDNSBool replymulticast)
	{
	ResourceRecord  *ResponseRecords = mDNSNULL;
	ResourceRecord **nrp             = &ResponseRecords;
	mDNSBool         delayresponse   = mDNSfalse;
	mDNSBool         HaveAtLeastOneAnswer = mDNSfalse;
	const mDNSu8    *ptr             = query->data;
	mDNSu8          *responseptr     = mDNSNULL;
	ResourceRecord  *rr, *rr2;
	int i;

	// If TC flag is set, it means we should expect that additional known answers may be coming in another packet.
	if (query->h.flags.b[0] & kDNSFlag0_TC) delayresponse = mDNStrue;

	// ***
	// *** 1. Parse Question Section and mark potential answers
	// ***
	for (i=0; i<query->h.numQuestions; i++)						// For each question...
		{
		int NumAnswersForThisQuestion = 0;
		DNSQuestion q;
		ptr = getQuestion(query, ptr, end, InterfaceID, &q);	// get the question...
		if (!ptr) goto exit;
		
		// Note: We use the m->CurrentRecord mechanism here because calling ResolveSimultaneousProbe
		// can result in user callbacks which may change the record list and/or question list.
		// Also note: we just mark potential answer records here, without trying to build the
		// "ResponseRecords" list, because we don't want to risk user callbacks deleting records
		//  from that list while we're in the middle of trying to build it.
		if (m->CurrentRecord) debugf("ProcessQuery ERROR m->CurrentRecord already set");
		m->CurrentRecord = m->ResourceRecords;
		while (m->CurrentRecord)
			{
			rr = m->CurrentRecord;
			m->CurrentRecord = rr->next;
			if (ResourceRecordAnswersQuestion(rr, &q))
				{
				if (rr->RecordType == kDNSRecordTypeUnique)
					ResolveSimultaneousProbe(m, query, end, &q, rr);
				else if (ResourceRecordIsValidAnswer(rr))
					{
					NumAnswersForThisQuestion++;
					if (!rr->NR_AnswerTo) rr->NR_AnswerTo = ptr;	// Mark as potential answer
					}
				}
			}
		// If we couldn't answer this question, someone else might be able to,
		// so use random delay on response to reduce collisions
		if (NumAnswersForThisQuestion == 0) delayresponse = mDNStrue;
		}

	// ***
	// *** 2. Now we can safely build the list of marked answers
	// ***
	for (rr = m->ResourceRecords; rr; rr=rr->next)				// Now build our list of potential answers
		if (rr->NR_AnswerTo)									// If we marked the record...
			if (AddRecordToResponseList(nrp, rr, rr->NR_AnswerTo, mDNSNULL))	// ... add it to the list
				{
				nrp = &rr->NextResponse;
				if (rr->RecordType == kDNSRecordTypeShared) delayresponse = mDNStrue;
				}

	// ***
	// *** 3. Add additional records
	// ***
	for (rr=ResponseRecords; rr; rr=rr->NextResponse)			// For each record we plan to put
		{
		// (Note: This is an "if", not a "while". If we add a record, we'll find it again
		// later in the "for" loop, and we will follow further "additional" links then.)
		if (rr->Additional1 && ResourceRecordIsValidInterfaceAnswer(rr->Additional1, InterfaceID) &&
			AddRecordToResponseList(nrp, rr->Additional1, mDNSNULL, rr))
			nrp = &rr->Additional1->NextResponse;

		if (rr->Additional2 && ResourceRecordIsValidInterfaceAnswer(rr->Additional2, InterfaceID) &&
			AddRecordToResponseList(nrp, rr->Additional2, mDNSNULL, rr))
			nrp = &rr->Additional2->NextResponse;
		
		// For SRV records, automatically add the Address record(s) for the target host
		if (rr->rrtype == kDNSType_SRV)
			for (rr2=m->ResourceRecords; rr2; rr2=rr2->next)					// Scan list of resource records
				if (RRIsAddressType(rr2) &&										// For all address records (A/AAAA) ...
					ResourceRecordIsValidInterfaceAnswer(rr2, InterfaceID) &&	// ... which are valid for answer ...
					SameDomainName(&rr->rdata->u.srv.target, &rr2->name) &&		// ... whose name is the name of the SRV target
					AddRecordToResponseList(nrp, rr2, mDNSNULL, rr))
					nrp = &rr2->NextResponse;
		}

	// ***
	// *** 4. Parse Answer Section and cancel any records disallowed by known answer list
	// ***
	for (i=0; i<query->h.numAnswers; i++)						// For each record in the query's answer section...
		{
		// Get the record...
		ResourceRecord pktrr, *rr;
		ptr = GetResourceRecord(m, query, ptr, end, InterfaceID, kDNSRecordTypePacketAnswer, &pktrr, mDNSNULL);
		if (!ptr) goto exit;

		// See if it suppresses any of our planned answers
		for (rr=ResponseRecords; rr; rr=rr->NextResponse)
			if (MustSendRecord(rr) && ShouldSuppressKnownAnswer(&pktrr, rr))
				{ rr->NR_AnswerTo = mDNSNULL; rr->NR_AdditionalTo = mDNSNULL; }

		// And see if it suppresses any previously scheduled answers
		for (rr=m->ResourceRecords; rr; rr=rr->next)
			{
			// If we're planning to send this answer on one interface, and only one interface, then allow KA suppression
			if (rr->ImmedAnswer && rr->ImmedAnswer != mDNSInterfaceMark && ShouldSuppressKnownAnswer(&pktrr, rr))
				{
				if (srcaddr->type == mDNSAddrType_IPv4)
					{
					if (mDNSSameIPv4Address(rr->v4Requester, srcaddr->ip.v4)) rr->v4Requester = zeroIPAddr;
					}
				else if (srcaddr->type == mDNSAddrType_IPv6)
					{
					if (mDNSSameIPv6Address(rr->v6Requester, srcaddr->ip.v6)) rr->v6Requester = zerov6Addr;
					}
				if (mDNSIPv4AddressIsZero(rr->v4Requester) && mDNSIPv6AddressIsZero(rr->v6Requester)) rr->ImmedAnswer = mDNSNULL;
				}
			}
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
			HaveAtLeastOneAnswer = mDNStrue;
		// For oversized records which we are going to send back to the requester via unicast
		// anyway, don't waste network bandwidth by also sending them via multicast.
		// This means we lose passive conflict detection for these oversized records, but
		// that is a reasonable tradeoff -- these large records usually have an associated
		// SRV record with the same name which will catch conflicts for us anyway.
		if (replymulticast && (!replyunicast || rr->rdestimate <= 1024))
			{
			if (rr->NR_AnswerTo)
				{
				// If we're already planning to send this on another interface, just send it on all interfaces
				if (rr->ImmedAnswer && rr->ImmedAnswer != InterfaceID)
					{
					rr->ImmedAnswer = mDNSInterfaceMark;
					m->NextResponseTime = m->timenow;
					debugf("%##s : Will send on all interfaces", rr->name.c);
					}
				else
					{
					rr->ImmedAnswer = InterfaceID;			// Record interface to send it on
					m->NextResponseTime = m->timenow;
					if (srcaddr->type == mDNSAddrType_IPv4)
						{
						if (mDNSIPv4AddressIsZero(rr->v4Requester)) rr->v4Requester = srcaddr->ip.v4;
						else if (!mDNSSameIPv4Address(rr->v4Requester, srcaddr->ip.v4))
							{
							// For now we don't report this error.
							//     Due to a bug in the OS X 10.2 version of mDNSResponder, when a service sends a goodbye packet,
							//     all those old mDNSResponders respond by sending a query to verify that the service is really gone.
							//     That flood of near-simultaneous queries then triggers the message below, warning that mDNSResponder
							//     can't implement multi-packet known-answer suppression from an unbounded number of clients.
							//if (query->h.flags.b[0] & kDNSFlag0_TC)
							//	LogMsg("%##s : Cannot perform multi-packet known-answer suppression from more than one"
							//		" client at a time %.4a %.4a (this is benign if it happens only rarely)",
							//		rr->name.c, &rr->v4Requester, &srcaddr->ip.v4);
							rr->v4Requester = onesIPv4Addr;
							}
						}
					else if (srcaddr->type == mDNSAddrType_IPv6)
						{
						if (mDNSIPv6AddressIsZero(rr->v6Requester)) rr->v6Requester = srcaddr->ip.v6;
						else if (!mDNSSameIPv6Address(rr->v6Requester, srcaddr->ip.v6))
							{
							if (query->h.flags.b[0] & kDNSFlag0_TC)
								LogMsg("%##s : Cannot perform multi-packet known-answer suppression from more than one "
									"client at a time %.16a %.16a (this is benign if it happens only rarely)",
									rr->name.c, &rr->v6Requester, &srcaddr->ip.v6);
							rr->v6Requester = onesIPv6Addr;
							}
						}
					}
				}
			else if (rr->NR_AdditionalTo)
				{
				// Since additional records are an optimization anyway, we only ever send them on one interface at a time
				// If two clients on different interfaces do queries that invoke the same optional additional answer,
				// then the earlier client is out of luck
				rr->ImmedAdditional = InterfaceID;
				// No need to set m->NextResponseTime here
				// We'll send these additional records when we send them, or not, as the case may be
				}
			}
		}

	// ***
	// *** 7. If we think other machines are likely to answer these questions, set our packet suppression timer
	// ***
	if (delayresponse && !m->SuppressSending)
		{
		// Pick a random delay between 20ms and 120ms.
		m->SuppressSending = m->timenow + (mDNSPlatformOneSecond*2 + (mDNSs32)mDNSRandom((mDNSu32)mDNSPlatformOneSecond*10)) / 100;
		if (m->SuppressSending == 0) m->SuppressSending = 1;
		}

	// ***
	// *** 8. If query is from a legacy client, generate a unicast reply too
	// ***
	if (HaveAtLeastOneAnswer && replyunicast)
		responseptr = GenerateUnicastResponse(query, end, InterfaceID, replyunicast, ResponseRecords);

exit:
	// ***
	// *** 9. Finally, clear our NextResponse link chain ready for use next time
	// ***
	while (ResponseRecords)
		{
		rr = ResponseRecords;
		ResponseRecords = rr->NextResponse;
		rr->NextResponse    = mDNSNULL;
		rr->NR_AnswerTo     = mDNSNULL;
		rr->NR_AdditionalTo = mDNSNULL;
		}
	
	return(responseptr);
	}

mDNSlocal void mDNSCoreReceiveQuery(mDNS *const m, const DNSMessage *const msg, const mDNSu8 *const end,
	const mDNSAddr *srcaddr, const mDNSIPPort srcport, const mDNSAddr *dstaddr, mDNSIPPort dstport,
	const mDNSInterfaceID InterfaceID)
	{
	DNSMessage    response;
	const mDNSu8 *responseend    = mDNSNULL;
	DNSMessage   *replyunicast   = mDNSNULL;
	mDNSBool      replymulticast = mDNSfalse;
	
	verbosedebugf("Received Query from %#-15a:%d to %#-15a:%d on 0x%.8X with %d Question%s, %d Answer%s, %d Authorit%s, %d Additional%s",
		srcaddr, (mDNSu16)srcport.b[0]<<8 | srcport.b[1],
		dstaddr, (mDNSu16)dstport.b[0]<<8 | dstport.b[1],
		InterfaceID,
		msg->h.numQuestions,   msg->h.numQuestions   == 1 ? "" : "s",
		msg->h.numAnswers,     msg->h.numAnswers     == 1 ? "" : "s",
		msg->h.numAuthorities, msg->h.numAuthorities == 1 ? "y" : "ies",
		msg->h.numAdditionals, msg->h.numAdditionals == 1 ? "" : "s");
	
	// If this was a unicast query, or it was from an old (non-port-5353) client, then send a unicast response
	if (!mDNSAddrIsDNSMulticast(dstaddr) || srcport.NotAnInteger != MulticastDNSPort.NotAnInteger)
		replyunicast = &response;
	
	// If this was a multicast query, then we need to send a multicast response
	if (mDNSAddrIsDNSMulticast(dstaddr)) replymulticast = mDNStrue;
	
	responseend = ProcessQuery(m, msg, end, srcaddr, InterfaceID, replyunicast, replymulticast);
	if (replyunicast && responseend)
		{
		debugf("Unicast Response: %d Question%s, %d Answer%s, %d Additional%s to %#-15a:%d on %p/%ld",
			replyunicast->h.numQuestions,   replyunicast->h.numQuestions   == 1 ? "" : "s",
			replyunicast->h.numAnswers,     replyunicast->h.numAnswers     == 1 ? "" : "s",
			replyunicast->h.numAdditionals, replyunicast->h.numAdditionals == 1 ? "" : "s",
			srcaddr, (mDNSu16)srcport.b[0]<<8 | srcport.b[1], InterfaceID, srcaddr->type);
		mDNSSendDNSMessage(m, replyunicast, responseend, InterfaceID, dstport, srcaddr, srcport);
		}
	}

// NOTE: mDNSCoreReceiveResponse calls mDNS_Deregister_internal which can call a user callback, which may change
// the record list and/or question list.
// Any code walking either list must use the CurrentQuestion and/or CurrentRecord mechanism to protect against this.
mDNSlocal void mDNSCoreReceiveResponse(mDNS *const m,
	const DNSMessage *const response, const mDNSu8 *end, const mDNSAddr *srcaddr, const mDNSAddr *dstaddr, const mDNSInterfaceID InterfaceID)
	{
	int i;
	const mDNSu8 *ptr = LocateAnswers(response, end);	// We ignore questions (if any) in a DNS response packet
	
	// All records in a DNS response packet are treated as equally valid statements of truth. If we want
	// to guard against spoof replies, then the only credible protection against that is cryptographic
	// security, e.g. DNSSEC., not worring about which section in the spoof packet contained the record
	int totalrecords = response->h.numAnswers + response->h.numAuthorities + response->h.numAdditionals;

	(void)srcaddr;	// Currently used only for display in debugging message

	verbosedebugf("Received Response from %#-15a addressed to %#-15a on %p with %d Question%s, %d Answer%s, %d Authorit%s, %d Additional%s",
		srcaddr, dstaddr, InterfaceID,
		response->h.numQuestions,   response->h.numQuestions   == 1 ? "" : "s",
		response->h.numAnswers,     response->h.numAnswers     == 1 ? "" : "s",
		response->h.numAuthorities, response->h.numAuthorities == 1 ? "y" : "ies",
		response->h.numAdditionals, response->h.numAdditionals == 1 ? "" : "s");

	// Other mDNS devices may issue unicast queries (which we correctly answer),
	// but we never *issue* unicast queries, so if we ever receive a unicast
	// response then it is someone trying to spoof us, so ignore it!
	if (!mDNSAddrIsDNSMulticast(dstaddr))
		{ debugf("** Ignored apparent spoof mDNS response packet addressed to %#-15a **", dstaddr); return; }

	for (i = 0; i < totalrecords && ptr && ptr < end; i++)
		{
		ResourceRecord pktrr;
		mDNSu8 RecordType = (i < response->h.numAnswers) ? kDNSRecordTypePacketAnswer : kDNSRecordTypePacketAdditional;
		ptr = GetResourceRecord(m, response, ptr, end, InterfaceID, RecordType, &pktrr, mDNSNULL);
		if (!ptr) return;

		// 1. Check that this packet resource record does not conflict with any of ours
		if (m->CurrentRecord) debugf("mDNSCoreReceiveResponse ERROR m->CurrentRecord already set");
		m->CurrentRecord = m->ResourceRecords;
		while (m->CurrentRecord)
			{
			ResourceRecord *rr = m->CurrentRecord;
			m->CurrentRecord = rr->next;
			if (PacketRRMatchesSignature(&pktrr, rr))		// If interface, name, type (if verified) and class match...
				{	
				// ... check to see if rdata is identical
				if (SameRData(pktrr.rrtype, rr->rrtype, pktrr.rdata, rr->rdata))
					{
					// If the RR in the packet is identical to ours, just check they're not trying to lower the TTL on us
					if (pktrr.rroriginalttl >= rr->rroriginalttl || m->SleepState)
						{
						// If we were planning to send on this -- and only this -- interface, then we don't need to any more
						if (rr->ImmedAnswer == InterfaceID) rr->ImmedAnswer = mDNSNULL;
						}
					else
						{
						if      (rr->ImmedAnswer == mDNSNULL)    { rr->ImmedAnswer = InterfaceID;       m->NextResponseTime = m->timenow; }
						else if (rr->ImmedAnswer != InterfaceID) { rr->ImmedAnswer = mDNSInterfaceMark; m->NextResponseTime = m->timenow; }
						}
					}
				else
					{
					// else, the packet RR has different rdata -- check to see if this is a conflict
					if (pktrr.rroriginalttl > 0 && PacketRRConflict(m, rr, &pktrr))
						{
						if (rr->rrtype == kDNSType_A)
							{
							debugf("mDNSCoreReceiveResponse: Our A Data %.4a", &rr->rdata->u.ip);
							debugf("mDNSCoreReceiveResponse: Pkt A Data %.4a", &pktrr.rdata->u.ip);
							}
						else if (rr->rrtype == kDNSType_PTR)
							{
							debugf("mDNSCoreReceiveResponse: Our PTR Data %d %##s", rr->rdata->RDLength,   rr->rdata->u.name.c);
							debugf("mDNSCoreReceiveResponse: Pkt PTR Data %d %##s", pktrr.rdata->RDLength, pktrr.rdata->u.name.c);
							}
						else if (rr->rrtype == kDNSType_TXT)
							{
							debugf("mDNSCoreReceiveResponse: Our TXT Data %d %#s", rr->rdata->RDLength,   rr->rdata->u.txt.c);
							debugf("mDNSCoreReceiveResponse: Pkt TXT Data %d %#s", pktrr.rdata->RDLength, pktrr.rdata->u.txt.c);
							}
						else if (rr->rrtype == kDNSType_AAAA)
							{
							debugf("mDNSCoreReceiveResponse: Our AAAA Data %.16a", &rr->rdata->u.ipv6);
							debugf("mDNSCoreReceiveResponse: Pkt AAAA Data %.16a", &pktrr.rdata->u.ipv6);
							}
						else if (rr->rrtype == kDNSType_SRV)
							{
							debugf("mDNSCoreReceiveResponse: Our SRV Data %d %##s", rr->rdata->RDLength,   rr->rdata->u.srv.target.c);
							debugf("mDNSCoreReceiveResponse: Pkt SRV Data %d %##s", pktrr.rdata->RDLength, pktrr.rdata->u.srv.target.c);
							}

						// If this record is marked DependentOn another record for conflict detection purposes,
						// then *that* record has to be bumped back to probing state to resolve the conflict
						while (rr->DependentOn) rr = rr->DependentOn;

						// If we've just whacked this record's ProbeCount, don't need to do it again
						if (rr->ProbeCount <= DefaultProbeCountForTypeUnique)
							{
							// If we'd previously verified this record, put it back to probing state and try again
							if (rr->RecordType == kDNSRecordTypeVerified)
								{
								debugf("mDNSCoreReceiveResponse: Reseting to Probing: %##s (%s)", rr->name.c, DNSTypeName(rr->rrtype));
								rr->RecordType     = kDNSRecordTypeUnique;
								rr->ProbeCount     = DefaultProbeCountForTypeUnique + 1;
								rr->ThisAPInterval = DefaultAPIntervalForRecordType(kDNSRecordTypeUnique);
								rr->LastAPTime     = m->timenow - rr->ThisAPInterval;
								SetNextAnnounceProbeTime(m, rr);

								// We increment NumFailedProbes here to make sure that repeated late conflicts
								// will also cause us to back off to the slower probing rate
								m->ProbeFailTime = m->timenow;
								if (m->NumFailedProbes < 9) m->NumFailedProbes++;
								else m->SuppressProbes = (m->timenow + mDNSPlatformOneSecond * 5) | 1;
								}
							// If we're probing for this record, we just failed
							else if (rr->RecordType == kDNSRecordTypeUnique)
								{
								debugf("mDNSCoreReceiveResponse: Will rename %##s (%s)", rr->name.c, DNSTypeName(rr->rrtype));
								mDNS_Deregister_internal(m, rr, mDNS_Dereg_conflict);
								}
							// We assumed this record must be unique, but we were wrong.
							// (e.g. There are two mDNSResponders on the same machine giving
							// different answers for the reverse mapping record.)
							// This is simply a misconfiguration, and we don't try to recover from it.
							else if (rr->RecordType == kDNSRecordTypeKnownUnique)
								{
								debugf("mDNSCoreReceiveResponse: Unexpected conflict on %##s (%s) -- discarding our record",
									rr->name.c, DNSTypeName(rr->rrtype));
								mDNS_Deregister_internal(m, rr, mDNS_Dereg_conflict);
								}
							else
								debugf("mDNSCoreReceiveResponse: Unexpected record type %X %##s (%s)",
									rr->RecordType, rr->name.c, DNSTypeName(rr->rrtype));
							}
						}
					}
				}
			}

		// 2. See if we want to add this packet resource record to our cache
		if (m->rrcache_size)	// Only try to cache answers if we have a cache to put them in
			{
			mDNSs32	slot = HashSlot(&pktrr.name);
			ResourceRecord *rr;
			// 2a. Check if this packet resource record is already in our cache
			for (rr = m->rrcache_hash[slot]; rr; rr=rr->next)
				{
				// If we found this exact resource record, refresh its TTL
				if (IdenticalResourceRecord(&pktrr, rr))
					{
					//debugf("Found RR %##s size %d already in cache", pktrr.name.c, pktrr.rdata->RDLength);
					rr->TimeRcvd = m->timenow;
					rr->NewData = mDNStrue;
					if (pktrr.rroriginalttl > 0)
						{
						rr->rroriginalttl = pktrr.rroriginalttl;
						rr->UnansweredQueries = 0;
						}
					else
						{
						// If the packet TTL is zero, that means we're deleting this record.
						// To give other hosts on the network a chance to protest, we push the deletion
						// out one second into the future. Also, we set UnansweredQueries to 2.
						// Otherwise, we'll do final queries for this record at 80% and 90% of its apparent
						// lifetime (800ms and 900ms from now) which is a pointless waste of network bandwidth.
						rr->rroriginalttl = 1;
						rr->UnansweredQueries = 2;
						}
					SetNextCacheCheckTime(m, rr);
					break;
					}
				}

			// If packet resource record not in our cache, add it now
			// (unless it is just a deletion of a record we never had, in which case we don't care)
			if (!rr && pktrr.rroriginalttl > 0)
				{
				rr = GetFreeCacheRR(m);
				if (!rr) debugf("No cache space to add record for %#s", pktrr.name.c);
				else
					{
					*rr = pktrr;
					rr->rdata = &rr->rdatastorage;	// For now, all cache records use local storage
					rr->next = m->rrcache_hash[slot];
					m->rrcache_hash[slot] = rr;
					m->rrcache_used[slot]++;
					m->rrcache_totalused++;
					//debugf("Adding RR %##s to cache (%d)", pktrr.name.c, m->rrcache_used);
					AnswerLocalQuestions(m, rr);
					// MUST do this AFTER AnswerLocalQuestions(), because that's what sets CRActiveQuestion for us
					SetNextCacheCheckTime(m,rr);
					}
				}
			}
		}

	// If we have a cache, then run through all the new records that we've just added,
	// clear their 'NewData' flags, and if they were marked as unique in the packet,
	// then search our cache for any records with the same name/type/class,
	// and purge them if they are more than one second old.
	if (m->rrcache_size)
		{
		ResourceRecord *rr, *r;
		mDNSs32 slot;
		for (slot = 0; slot < CACHE_HASH_SLOTS; slot++)
			for (rr = m->rrcache_hash[slot]; rr; rr=rr->next)
				if (rr->NewData)
					{
					rr->NewData = mDNSfalse;
					if (rr->RecordType & kDNSRecordTypeUniqueMask)
						for (r = m->rrcache_hash[slot]; r; r=r->next)
							if (SameResourceRecordSignature(rr, r) && m->timenow - r->TimeRcvd > mDNSPlatformOneSecond)
								PurgeCacheResourceRecord(m, rr);
					}
		}
	}

mDNSexport void mDNSCoreReceive(mDNS *const m, DNSMessage *const msg, const mDNSu8 *const end,
	const mDNSAddr *const srcaddr, const mDNSIPPort srcport, const mDNSAddr *const dstaddr, const mDNSIPPort dstport,
	const mDNSInterfaceID InterfaceID)
	{
	const mDNSu8 StdQ  = kDNSFlag0_QR_Query    | kDNSFlag0_OP_StdQuery;
	const mDNSu8 StdR  = kDNSFlag0_QR_Response | kDNSFlag0_OP_StdQuery;
	const mDNSu8 QR_OP = (mDNSu8)(msg->h.flags.b[0] & kDNSFlag0_QROP_Mask);
	
	// Read the integer parts which are in IETF byte-order (MSB first, LSB second)
	mDNSu8 *ptr = (mDNSu8 *)&msg->h.numQuestions;
	msg->h.numQuestions   = (mDNSu16)((mDNSu16)ptr[0] <<  8 | ptr[1]);
	msg->h.numAnswers     = (mDNSu16)((mDNSu16)ptr[2] <<  8 | ptr[3]);
	msg->h.numAuthorities = (mDNSu16)((mDNSu16)ptr[4] <<  8 | ptr[5]);
	msg->h.numAdditionals = (mDNSu16)((mDNSu16)ptr[6] <<  8 | ptr[7]);
	
	if (!m) { debugf("mDNSCoreReceive ERROR m is NULL"); return; }
	
	mDNS_Lock(m);
	if      (QR_OP == StdQ) mDNSCoreReceiveQuery   (m, msg, end, srcaddr, srcport, dstaddr, dstport, InterfaceID);
	else if (QR_OP == StdR) mDNSCoreReceiveResponse(m, msg, end, srcaddr,          dstaddr,          InterfaceID);
	else debugf("Unknown DNS packet type %02X%02X (ignored)", msg->h.flags.b[0], msg->h.flags.b[1]);

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
#pragma mark -
#pragma mark - Searcher Functions
#endif

mDNSlocal DNSQuestion *FindDuplicateQuestion(const mDNS *const m, const DNSQuestion *const question)
	{
	DNSQuestion *q;
	// Note: A question can only be marked as a duplicate of one that occurs *earlier* in the list.
	// This prevents circular references, where two questions are each marked as a duplicate of the other.
	// Accordingly, we break out of the loop when we get to 'question', because there's no point searching
	// further in the list.
	for (q = m->Questions; q && q != question; q=q->next)	// Scan our list of questions
		if (q->InterfaceID == question->InterfaceID &&		// for another question with the same InterfaceID,
			q->rrtype      == question->rrtype      &&		// type,
			q->rrclass     == question->rrclass     &&		// class,
			SameDomainName(&q->name, &question->name))		// and name
			return(q);
	return(mDNSNULL);
	}

// This is called after a question is deleted, in case other identical questions were being
// suppressed as duplicates
mDNSlocal void UpdateQuestionDuplicates(mDNS *const m, const DNSQuestion *const question)
	{
	DNSQuestion *q;
	for (q = m->Questions; q; q=q->next)		// Scan our list of questions
		if (q->DuplicateOf == question)			// To see if any questions were referencing this as their duplicate
			{
			q->ThisQInterval = question->ThisQInterval;
			q->LastQTime     = question->LastQTime;
			q->RecentAnswers = 0;
			q->DuplicateOf   = FindDuplicateQuestion(m, q);
			SetNextQueryTime(m,q);
			}
	}

mDNSlocal mStatus mDNS_StartQuery_internal(mDNS *const m, DNSQuestion *const question)
	{
	if (m->rrcache_size == 0)	// Can't do queries if we have no cache space allocated
		return(mStatus_NoCache);
	else
		{
		// Note: It important that new questions are appended at the *end* of the list, not prepended at the start
		DNSQuestion **q = &m->Questions;
		while (*q && *q != question) q=&(*q)->next;

		if (*q)
			{
			debugf("Error! Tried to add a question that's already in the active list");
			return(mStatus_AlreadyRegistered);
			}

		if (question->InterfaceID)		// If this question is referencing a specific interface
			{
			NetworkInterfaceInfo *intf;	// Then make sure that interface exists
			for (intf = m->HostInterfaces; intf; intf = intf->next)
				if (intf->InterfaceID == question->InterfaceID) break;
			if (!intf)
				{
				debugf("mDNS_StartQuery_internal: Question %##s InterfaceID %p not found", question->name.c, question->InterfaceID);
				return(mStatus_BadReferenceErr);
				}
			}

		// Note: In the case where we already have the answer to this question in our cache, that may be all the client
		// wanted, and they may immediately cancel their question. In this case, sending an actual query on the wire would
		// be a waste. For that reason, we schedule our first query to go out in half a second. If AnswerNewQuestion() finds
		// that we have *no* relevant answers currently in our cache, then it will accelerate that to go out immediately.
		question->next          = mDNSNULL;
		question->ThisQInterval = mDNSPlatformOneSecond/2;  // MUST be > zero for an active question
		question->LastQTime     = m->timenow;
		question->RecentAnswers = 0;
		question->DuplicateOf   = FindDuplicateQuestion(m, question);
		// question->InterfaceID must be already set by caller
		question->SendQNow      = mDNSNULL;

		if (!question->DuplicateOf)
			verbosedebugf("mDNS_StartQuery_internal: Question %##s %s %p (%p) started",
				question->name.c, DNSTypeName(question->rrtype), question->InterfaceID, question);
		else
			verbosedebugf("mDNS_StartQuery_internal: Question %##s %s %p (%p) duplicate of (%p)",
				question->name.c, DNSTypeName(question->rrtype), question->InterfaceID, question, question->DuplicateOf);

		*q = question;
		
		if (!m->NewQuestions)
			debugf("mDNS_StartQuery_internal: Setting NewQuestions to %##s (%s)", question->name.c, DNSTypeName(question->rrtype));
		if (!m->NewQuestions) m->NewQuestions = question;

		SetNextQueryTime(m,question);

		return(mStatus_NoError);
		}
	}

mDNSlocal void mDNS_StopQuery_internal(mDNS *const m, DNSQuestion *const question)
	{
	ResourceRecord *rr;
	DNSQuestion **q = &m->Questions;
	while (*q && *q != question) q=&(*q)->next;
	if (*q) *q = (*q)->next;
	else debugf("mDNS_StopQuery_internal: Question %##s (%s) not found in active list",
		question->name.c, DNSTypeName(question->rrtype));

	// Take care to cut question from list *before* calling UpdateQuestionDuplicates
	UpdateQuestionDuplicates(m, question);
	// But don't trash ThisQInterval until afterwards.
	question->ThisQInterval = -1;

	// If there are any cache records referencing this as their active question, then see if any other
	// question that is also referencing them, else their CRActiveQuestion needs to get set to NULL.
	for (rr = m->rrcache_hash[HashSlot(&question->name)]; rr; rr=rr->next)
		{
		if (rr->CRActiveQuestion == question)
			{
			DNSQuestion *q;
			for (q = m->Questions; q; q=q->next)		// Scan our list of questions
				if (ActiveQuestion(q) && ResourceRecordAnswersQuestion(rr, q))
					break;
			verbosedebugf("mDNS_StopQuery_internal: Cache RR %##s setting CRActiveQuestion to %X", rr->name.c, q);
			rr->CRActiveQuestion = q;
			}
		}

	// If we just deleted the question that AnswerLocalQuestions() is about to look at,
	// bump its pointer forward one question.
	if (m->CurrentQuestion == question)
		{
		debugf("mDNS_StopQuery_internal: Just deleted the currently active question: %##s (%s)",
			question->name.c, DNSTypeName(question->rrtype));
		m->CurrentQuestion = m->CurrentQuestion->next;
		}

	if (m->NewQuestions    == question)
		{
		debugf("mDNS_StopQuery_internal: Just deleted a new question that wasn't even answered yet: %##s (%s)",
			question->name.c, DNSTypeName(question->rrtype));
		m->NewQuestions    = m->NewQuestions->next;
		}

	// Take care not to trash question->next until *after* we've updated m->CurrentQuestion and m->NewQuestions
	question->next = mDNSNULL;
	}

mDNSexport mStatus mDNS_StartQuery(mDNS *const m, DNSQuestion *const question)
	{
	mStatus status;
	mDNS_Lock(m);
	status = mDNS_StartQuery_internal(m, question);
	mDNS_Unlock(m);
	return(status);
	}

mDNSexport void mDNS_StopQuery(mDNS *const m, DNSQuestion *const question)
	{
	mDNS_Lock(m);
	mDNS_StopQuery_internal(m, question);
	mDNS_Unlock(m);
	}

mDNSexport mStatus mDNS_StartBrowse(mDNS *const m, DNSQuestion *const question,
	const domainname *const srv, const domainname *const domain,
	const mDNSInterfaceID InterfaceID, mDNSQuestionCallback *Callback, void *Context)
	{
	question->InterfaceID       = InterfaceID;
	if (!ConstructServiceName(&question->name, mDNSNULL, srv, domain)) return(mStatus_BadParamErr);
	question->rrtype            = kDNSType_PTR;
	question->rrclass           = kDNSClass_IN;
	question->QuestionCallback  = Callback;
	question->QuestionContext   = Context;
	return(mDNS_StartQuery(m, question));
	}

mDNSlocal void FoundServiceInfoSRV(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer)
	{
	ServiceInfoQuery *query = (ServiceInfoQuery *)question->QuestionContext;
	mDNSBool PortChanged = (query->info->port.NotAnInteger != answer->rdata->u.srv.port.NotAnInteger);
	if (answer->rrremainingttl == 0) return;
	if (answer->rrtype != kDNSType_SRV) return;

	query->info->port = answer->rdata->u.srv.port;

	// If this is our first answer, then set the GotSRV flag and start the address query
	if (!query->GotSRV)
		{
		query->GotSRV             = mDNStrue;
		query->qAv4.InterfaceID   = answer->InterfaceID;
		query->qAv4.name          = answer->rdata->u.srv.target;
		query->qAv6.InterfaceID   = answer->InterfaceID;
		query->qAv6.name          = answer->rdata->u.srv.target;
		mDNS_StartQuery_internal(m, &query->qAv4);
		mDNS_StartQuery_internal(m, &query->qAv6);
		}
	// If this is not our first answer, only re-issue the address query if the target host name has changed
	else if (query->qAv4.InterfaceID != answer->InterfaceID ||
		!SameDomainName(&query->qAv4.name, &answer->rdata->u.srv.target))
		{
		mDNS_StopQuery_internal(m, &query->qAv4);
		mDNS_StopQuery_internal(m, &query->qAv6);
		query->qAv4.InterfaceID   = answer->InterfaceID;
		query->qAv4.name          = answer->rdata->u.srv.target;
		query->qAv6.InterfaceID   = answer->InterfaceID;
		query->qAv6.name          = answer->rdata->u.srv.target;
		mDNS_StartQuery_internal(m, &query->qAv4);
		mDNS_StartQuery_internal(m, &query->qAv6);
		}
	else if (query->ServiceInfoQueryCallback && query->GotADD && query->GotTXT && PortChanged)
		query->ServiceInfoQueryCallback(m, query);
	// CAUTION: MUST NOT do anything more with query after calling query->Callback(), because the client's
	// callback function is allowed to do anything, including deleting this query and freeing its memory.
	}

mDNSlocal void FoundServiceInfoTXT(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer)
	{
	ServiceInfoQuery *query = (ServiceInfoQuery *)question->QuestionContext;
	if (answer->rrremainingttl == 0) return;
	if (answer->rrtype != kDNSType_TXT) return;
	if (answer->rdata->RDLength > sizeof(query->info->TXTinfo)) return;

	query->GotTXT       = (mDNSu8)(1 + (query->GotTXT || query->GotADD));
	query->info->TXTlen = answer->rdata->RDLength;
	mDNSPlatformMemCopy(answer->rdata->u.txt.c, query->info->TXTinfo, answer->rdata->RDLength);

	verbosedebugf("FoundServiceInfoTXT: %##s GotADD=%d", query->info->name.c, query->GotADD);

	// CAUTION: MUST NOT do anything more with query after calling query->Callback(), because the client's
	// callback function is allowed to do anything, including deleting this query and freeing its memory.
	if (query->ServiceInfoQueryCallback && query->GotADD)
		query->ServiceInfoQueryCallback(m, query);
	}

mDNSlocal void FoundServiceInfo(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer)
	{
	ServiceInfoQuery *query = (ServiceInfoQuery *)question->QuestionContext;
	if (answer->rrremainingttl == 0) return;
	
	if (answer->rrtype == kDNSType_A)
		{
		query->info->ip.type = mDNSAddrType_IPv4;
		query->info->ip.ip.v4 = answer->rdata->u.ip;
		}
	else if (answer->rrtype == kDNSType_AAAA)
		{
		query->info->ip.type = mDNSAddrType_IPv6;
		query->info->ip.ip.v6 = answer->rdata->u.ipv6;
		}
	else
		{
		debugf("FoundServiceInfo: answer type %d (%s) unexpected", answer->rrtype, DNSTypeName(answer->rrtype));
		return;
		}

	query->GotADD = mDNStrue;
	query->info->InterfaceID = answer->InterfaceID;

	verbosedebugf("FoundServiceInfo v%d: %##s GotTXT=%d", query->info->ip.type, query->info->name.c, query->GotTXT);

	// If query->GotTXT is 1 that means we already got a single TXT answer but didn't
	// deliver it to the client at that time, so no further action is required.
	// If query->GotTXT is 2 that means we either got more than one TXT answer,
	// or we got a TXT answer and delivered it to the client at that time, so in either
	// of these cases we may have lost information, so we should re-issue the TXT question.
	if (query->GotTXT > 1)
		{
		mDNS_StopQuery_internal(m, &query->qTXT);
		mDNS_StartQuery_internal(m, &query->qTXT);
		}

	// CAUTION: MUST NOT do anything more with query after calling query->Callback(), because the client's
	// callback function is allowed to do anything, including deleting this query and freeing its memory.
	if (query->ServiceInfoQueryCallback && query->GotTXT)
		query->ServiceInfoQueryCallback(m, query);
	}

// On entry, the client must have set the name and InterfaceID fields of the ServiceInfo structure
// If the query is not interface-specific, then InterfaceID may be zero
// Each time the Callback is invoked, the remainder of the fields will have been filled in
// In addition, InterfaceID will be updated to give the interface identifier corresponding to that reply
mDNSexport mStatus mDNS_StartResolveService(mDNS *const m,
	ServiceInfoQuery *query, ServiceInfo *info, mDNSServiceInfoQueryCallback *Callback, void *Context)
	{
	mStatus status;
	mDNS_Lock(m);

	query->qSRV.ThisQInterval       = -1;		// This question not yet in the question list
	query->qSRV.InterfaceID         = info->InterfaceID;
	query->qSRV.name                = info->name;
	query->qSRV.rrtype              = kDNSType_SRV;
	query->qSRV.rrclass             = kDNSClass_IN;
	query->qSRV.QuestionCallback    = FoundServiceInfoSRV;
	query->qSRV.QuestionContext     = query;

	query->qTXT.ThisQInterval       = -1;		// This question not yet in the question list
	query->qTXT.InterfaceID         = info->InterfaceID;
	query->qTXT.name                = info->name;
	query->qTXT.rrtype              = kDNSType_TXT;
	query->qTXT.rrclass             = kDNSClass_IN;
	query->qTXT.QuestionCallback    = FoundServiceInfoTXT;
	query->qTXT.QuestionContext     = query;

	query->qAv4.ThisQInterval       = -1;		// This question not yet in the question list
	query->qAv4.InterfaceID         = info->InterfaceID;
	query->qAv4.name.c[0]           = 0;
	query->qAv4.rrtype              = kDNSType_A;
	query->qAv4.rrclass             = kDNSClass_IN;
	query->qAv4.QuestionCallback    = FoundServiceInfo;
	query->qAv4.QuestionContext     = query;

	query->qAv6.ThisQInterval       = -1;		// This question not yet in the question list
	query->qAv6.InterfaceID         = info->InterfaceID;
	query->qAv6.name.c[0]           = 0;
	query->qAv6.rrtype              = kDNSType_AAAA;
	query->qAv6.rrclass             = kDNSClass_IN;
	query->qAv6.QuestionCallback    = FoundServiceInfo;
	query->qAv6.QuestionContext     = query;

	query->GotSRV                   = mDNSfalse;
	query->GotTXT                   = mDNSfalse;
	query->GotADD                   = mDNSfalse;

	query->info                     = info;
	query->ServiceInfoQueryCallback = Callback;
	query->ServiceInfoQueryContext  = Context;

//	info->name      = Must already be set up by client
//	info->interface = Must already be set up by client
	info->ip        = zeroAddr;
	info->port      = zeroIPPort;
	info->TXTlen    = 0;

	status = mDNS_StartQuery_internal(m, &query->qSRV);
	if (status == mStatus_NoError) status = mDNS_StartQuery_internal(m, &query->qTXT);
	if (status != mStatus_NoError) mDNS_StopResolveService(m, query);

	mDNS_Unlock(m);
	return(status);
	}

mDNSexport void    mDNS_StopResolveService (mDNS *const m, ServiceInfoQuery *query)
	{
	mDNS_Lock(m);
	if (query->qSRV.ThisQInterval >= 0) mDNS_StopQuery_internal(m, &query->qSRV);
	if (query->qTXT.ThisQInterval >= 0) mDNS_StopQuery_internal(m, &query->qTXT);
	if (query->qAv4.ThisQInterval >= 0) mDNS_StopQuery_internal(m, &query->qAv4);
	if (query->qAv6.ThisQInterval >= 0) mDNS_StopQuery_internal(m, &query->qAv6);
	mDNS_Unlock(m);
	}

mDNSexport mStatus mDNS_GetDomains(mDNS *const m, DNSQuestion *const question, mDNSu8 DomainType,
	const mDNSInterfaceID InterfaceID, mDNSQuestionCallback *Callback, void *Context)
	{
	MakeDomainNameFromDNSNameString(&question->name, mDNS_DomainTypeNames[DomainType]);
	question->InterfaceID      = InterfaceID;
	question->rrtype           = kDNSType_PTR;
	question->rrclass          = kDNSClass_IN;
	question->QuestionCallback = Callback;
	question->QuestionContext  = Context;
	return(mDNS_StartQuery(m, question));
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - Responder Functions
#endif

// Set up a ResourceRecord with sensible default values.
// These defaults may be overwritten with new values before mDNS_Register is called
mDNSexport void mDNS_SetupResourceRecord(ResourceRecord *rr, RData *RDataStorage, mDNSInterfaceID InterfaceID,
	mDNSu16 rrtype, mDNSu32 ttl, mDNSu8 RecordType, mDNSRecordCallback Callback, void *Context)
	{
	// Don't try to store a TTL bigger than we can represent in platform time units
	if (ttl > 0x7FFFFFFFUL / mDNSPlatformOneSecond)
		ttl = 0x7FFFFFFFUL / mDNSPlatformOneSecond;
	else if (ttl == 0)		// And Zero TTL is illegal
		ttl = 1;

	// Field Group 1: Persistent metadata for Authoritative Records
	rr->Additional1       = mDNSNULL;
	rr->Additional2       = mDNSNULL;
	rr->DependentOn       = mDNSNULL;
	rr->RRSet             = mDNSNULL;
	rr->RecordCallback    = Callback;
	rr->RecordContext     = Context;

	rr->RecordType        = RecordType;
	rr->HostTarget        = mDNSfalse;
	
	// Field Group 2: Transient state for Authoritative Records (set in mDNS_Register_internal)
	// Field Group 3: Transient state for Cache Records         (set in mDNS_Register_internal)

	// Field Group 4: The actual information pertaining to this resource record
	rr->InterfaceID       = InterfaceID;
	rr->name.c[0]         = 0;		// MUST be set by client
	rr->rrtype            = rrtype;
	rr->rrclass           = kDNSClass_IN;
	rr->rroriginalttl     = ttl;
	rr->rrremainingttl    = ttl;
//	rr->rdlength          = MUST set by client and/or in mDNS_Register_internal
//	rr->rdestimate        = set in mDNS_Register_internal
//	rr->rdata             = MUST be set by client

	if (RDataStorage)
		rr->rdata = RDataStorage;
	else
		{
		rr->rdata = &rr->rdatastorage;
		rr->rdata->MaxRDLength = sizeof(RDataBody);
		}
	}

mDNSexport mStatus mDNS_Register(mDNS *const m, ResourceRecord *const rr)
	{
	mStatus status;
	mDNS_Lock(m);
	status = mDNS_Register_internal(m, rr);
	mDNS_Unlock(m);
	return(status);
	}

mDNSexport mStatus mDNS_Update(mDNS *const m, ResourceRecord *const rr, mDNSu32 newttl,
	RData *const newrdata, mDNSRecordUpdateCallback *Callback)
	{
	mDNS_Lock(m);

	// If TTL is unspecified, leave TTL unchanged
	if (newttl == 0) newttl = rr->rroriginalttl;

	// If we already have an update queued up which has not gone through yet,
	// give the client a chance to free that memory
	if (rr->NewRData)
		{
		RData *n = rr->NewRData;
		rr->NewRData = mDNSNULL;	// Clear the NewRData pointer ...
		if (rr->UpdateCallback)
			rr->UpdateCallback(m, rr, n); // ...and let the client free this memory, if necessary
		}
	
	if (rr->AnnounceCount < ReannounceCount)
		rr->AnnounceCount = ReannounceCount;
	rr->ThisAPInterval   = DefaultAPIntervalForRecordType(rr->RecordType);
	rr->LastAPTime       = m->timenow - rr->ThisAPInterval;
	if (RRUniqueOrKnownUnique(rr) && m->SuppressProbes) rr->LastAPTime = m->SuppressProbes - rr->ThisAPInterval;
	SetNextAnnounceProbeTime(m, rr);
	rr->NewRData         = newrdata;
	rr->UpdateCallback   = Callback;
	rr->rroriginalttl    = newttl;
	rr->rrremainingttl   = newttl;
	mDNS_Unlock(m);
	return(mStatus_NoError);
	}

// NOTE: mDNS_Deregister calls mDNS_Deregister_internal which can call a user callback, which may change
// the record list and/or question list.
// Any code walking either list must use the CurrentQuestion and/or CurrentRecord mechanism to protect against this.
mDNSexport mStatus mDNS_Deregister(mDNS *const m, ResourceRecord *const rr)
	{
	mStatus status;
	mDNS_Lock(m);
	status = mDNS_Deregister_internal(m, rr, mDNS_Dereg_normal);
	mDNS_Unlock(m);
	return(status);
	}

mDNSlocal void HostNameCallback(mDNS *const m, ResourceRecord *const rr, mStatus result);

mDNSlocal NetworkInterfaceInfo *FindFirstAdvertisedInterface(mDNS *const m)
	{
	NetworkInterfaceInfo *intf;
	for (intf = m->HostInterfaces; intf; intf = intf->next)
		if (intf->Advertise) break;
	return(intf);
	}

mDNSlocal void mDNS_AdvertiseInterface(mDNS *const m, NetworkInterfaceInfo *set)
	{
	char buffer[256];
	NetworkInterfaceInfo *primary = FindFirstAdvertisedInterface(m);
	if (!primary) primary = set; // If no existing advertised interface, this new NetworkInterfaceInfo becomes our new primary
	
	mDNS_SetupResourceRecord(&set->RR_A1,  mDNSNULL, set->InterfaceID, kDNSType_A,   60, kDNSRecordTypeUnique,      HostNameCallback, set);
	mDNS_SetupResourceRecord(&set->RR_A2,  mDNSNULL, set->InterfaceID, kDNSType_A,   60, kDNSRecordTypeUnique,      HostNameCallback, set);
	mDNS_SetupResourceRecord(&set->RR_PTR, mDNSNULL, set->InterfaceID, kDNSType_PTR, 60, kDNSRecordTypeKnownUnique, mDNSNULL, mDNSNULL);

	// 1. Set up primary Address record to map from primary host name ("foo.local.") to IP address
	// 2. Set up secondary Address record to map from secondary host name ("foo.local.arpa.") to IP address
	// 3. Set up reverse-lookup PTR record to map from our address back to our primary host name
	// Setting HostTarget tells DNS that the target of this PTR is to be automatically kept in sync if our host name changes
	// Note: This is reverse order compared to a normal dotted-decimal IP address
	set->RR_A1.name        = m->hostname1;
	set->RR_A2.name        = m->hostname2;
	if (set->ip.type == mDNSAddrType_IPv4)
		{
		set->RR_A1.rrtype = kDNSType_A;
		set->RR_A2.rrtype = kDNSType_A;
		set->RR_A1.rdata->u.ip = set->ip.ip.v4;
		set->RR_A2.rdata->u.ip = set->ip.ip.v4;
		mDNS_snprintf(buffer, sizeof(buffer), "%d.%d.%d.%d.in-addr.arpa.",
			set->ip.ip.v4.b[3], set->ip.ip.v4.b[2], set->ip.ip.v4.b[1], set->ip.ip.v4.b[0]);
		}
	else if (set->ip.type == mDNSAddrType_IPv6)
		{
		int	i;
		set->RR_A1.rrtype = kDNSType_AAAA;
		set->RR_A2.rrtype = kDNSType_AAAA;
		set->RR_A1.rdata->u.ipv6 = set->ip.ip.v6;
		set->RR_A2.rdata->u.ipv6 = set->ip.ip.v6;
		for (i = 0; i < 16; i++)
			{
			static const char hexValues[] = "0123456789ABCDEF";
			buffer[i * 4    ] = hexValues[set->ip.ip.v6.b[15 - i] & 0x0F];
			buffer[i * 4 + 1] = '.';
			buffer[i * 4 + 2] = hexValues[set->ip.ip.v6.b[15 - i] >> 4];
			buffer[i * 4 + 3] = '.';
			}
		mDNS_snprintf(&buffer[32], sizeof(buffer)-32, "ip6.arpa.");
		}

	MakeDomainNameFromDNSNameString(&set->RR_PTR.name, buffer);
	set->RR_PTR.HostTarget = mDNStrue;	// Tell mDNS that the target of this PTR is to be kept in sync with our host name

	set->RR_A1.RRSet = &primary->RR_A1;	// May refer to self
	set->RR_A2.RRSet = &primary->RR_A2;	// May refer to self

	mDNS_Register_internal(m, &set->RR_A1);
	mDNS_Register_internal(m, &set->RR_A2);
	mDNS_Register_internal(m, &set->RR_PTR);

	// ... Add an HINFO record, etc.?
	}

mDNSlocal void mDNS_DeadvertiseInterface(mDNS *const m, NetworkInterfaceInfo *set)
	{
	NetworkInterfaceInfo *intf;
	// If we still have address records referring to this one, update them
	NetworkInterfaceInfo *primary = FindFirstAdvertisedInterface(m);
	ResourceRecord *A1 = primary ? &primary->RR_A1 : mDNSNULL;
	ResourceRecord *A2 = primary ? &primary->RR_A2 : mDNSNULL;
	for (intf = m->HostInterfaces; intf; intf = intf->next)
		{
		if (intf->RR_A1.RRSet == &set->RR_A1) intf->RR_A1.RRSet = A1;
		if (intf->RR_A2.RRSet == &set->RR_A2) intf->RR_A2.RRSet = A2;
		}

	// Unregister these records.
	// When doing the mDNS_Close processing, we first call mDNS_DeadvertiseInterface for each interface, so by the time the platform
	// support layer gets to call mDNS_DeregisterInterface, the address and PTR records have already been deregistered for it.
	// Also, in the event of a name conflict, one or more of our records will have been forcibly deregistered.
	// To avoid unnecessary and misleading warning messages, we check the RecordType before calling mDNS_Deregister_internal().
	if (set->RR_A1. RecordType) mDNS_Deregister_internal(m, &set->RR_A1,  mDNS_Dereg_normal);
	if (set->RR_A2. RecordType) mDNS_Deregister_internal(m, &set->RR_A2,  mDNS_Dereg_normal);
	if (set->RR_PTR.RecordType) mDNS_Deregister_internal(m, &set->RR_PTR, mDNS_Dereg_normal);
	}

mDNSexport void mDNS_GenerateFQDN(mDNS *const m)
	{
	domainname newname;
	mDNS_Lock(m);

	newname.c[0] = 0;
	if (!AppendDomainLabel(&newname, &m->hostlabel))  LogMsg("ERROR! Cannot create dot-local hostname");
	if (!AppendLiteralLabelString(&newname, "local")) LogMsg("ERROR! Cannot create dot-local hostname");
	if (!SameDomainName(&m->hostname1, &newname))
		{
		NetworkInterfaceInfo *intf;
		ResourceRecord *rr;

		m->hostname1 = newname;
		m->hostname2 = newname;
		if (!AppendLiteralLabelString(&m->hostname2, "arpa")) LogMsg("ERROR! Cannot create local.arpa hostname");

		// 1. Stop advertising our address records on all interfaces
		for (intf = m->HostInterfaces; intf; intf = intf->next)
			if (intf->Advertise) mDNS_DeadvertiseInterface(m, intf);

		// 2. Start advertising our address records using the new name
		for (intf = m->HostInterfaces; intf; intf = intf->next)
			if (intf->Advertise) mDNS_AdvertiseInterface(m, intf);

		// 3. Make sure that any SRV records (and the like) that reference our 
		// host name in their rdata get updated to reference this new host name
		for (rr = m->ResourceRecords; rr; rr=rr->next)
			if (rr->HostTarget) SetTargetToHostName(m, rr);
		}

	mDNS_Unlock(m);
	}

mDNSlocal void HostNameCallback(mDNS *const m, ResourceRecord *const rr, mStatus result)
	{
	#pragma unused(rr)
	switch (result)
		{
		case mStatus_NoError:
			debugf("HostNameCallback: %##s (%s) Name registered",    rr->name.c, DNSTypeName(rr->rrtype));
			break;
		case mStatus_NameConflict:
			debugf("HostNameCallback: %##s (%s) Name conflict",      rr->name.c, DNSTypeName(rr->rrtype));
			break;
		default:
			debugf("HostNameCallback: %##s (%s) Unknown result %ld", rr->name.c, DNSTypeName(rr->rrtype), result);
			break;
		}

	if (result == mStatus_NameConflict)
		{
		domainlabel oldlabel = m->hostlabel;

		// 1. First give the client callback a chance to pick a new name
		if (m->MainCallback)
			m->MainCallback(m, mStatus_NameConflict);

		// 2. If the client callback didn't do it, add (or increment) an index ourselves
		if (SameDomainLabel(m->hostlabel.c, oldlabel.c))
			IncrementLabelSuffix(&m->hostlabel, mDNSfalse);

		// 3. Generate the FQDNs from the hostlabel,
		// and make sure all SRV records, etc., are updated to reference our new hostname
		mDNS_GenerateFQDN(m);
		}
	}
mDNSexport mStatus mDNS_RegisterInterface(mDNS *const m, NetworkInterfaceInfo *set)
	{
	NetworkInterfaceInfo **p = &m->HostInterfaces;
	mDNS_Lock(m);
	
	// Assume this interface will be active
	set->InterfaceActive = mDNStrue;
	while (*p)
		{
		if (*p == set)
			{
			debugf("Error! Tried to register a NetworkInterfaceInfo that's already in the list");
			mDNS_Unlock(m);
			return(mStatus_AlreadyRegistered);
			}

		// This InterfaceID is already in the list, so mark this interface inactive for now
		if ((*p)->InterfaceID == set->InterfaceID)
			set->InterfaceActive = mDNSfalse;

		p=&(*p)->next;
		}

	set->next = mDNSNULL;
	*p = set;

	if (set->InterfaceActive)
		debugf("mDNS_RegisterInterface: InterfaceID %X not represented in list; marking active and retriggering queries", set->InterfaceID);
	else
		debugf("mDNS_RegisterInterface: InterfaceID %X already represented in list; marking inactive for now", set->InterfaceID);

	// In some versions of OS X the IPv6 address remains on an interface even when the interface is turned off,
	// giving the false impression that there's an active representative of this interface when there really isn't.
	// Therefore, when registering an interface, we want to re-trigger our questions and re-probe our Resource Records,
	// even if we believe that we previously had an active representative of this interface.
	if (1 || set->InterfaceActive)
		{
		DNSQuestion *q;
		ResourceRecord *rr;
		for (q = m->Questions; q; q=q->next)							// Scan our list of questions
			if (!q->InterfaceID || q->InterfaceID == set->InterfaceID)	// If non-specific Q, or Q on this specific interface,
				{														// then reactivate this question
				q->ThisQInterval = mDNSPlatformOneSecond/2;				// MUST be > zero for an active question
				q->LastQTime     = m->timenow - q->ThisQInterval;
				q->RecentAnswers = 0;
				if (ActiveQuestion(q)) m->NextScheduledQuery = m->timenow;
				}
		
		// Reactivate any dormant authoritative records specific to this interface
		for (rr = m->ResourceRecords; rr; rr=rr->next)
			if (rr->InterfaceID == set->InterfaceID)
				{
				rr->RRInterfaceActive = mDNStrue;
				if (rr->RecordType == kDNSRecordTypeVerified && !rr->DependentOn) rr->RecordType = kDNSRecordTypeUnique;
				rr->ProbeCount        = DefaultProbeCountForRecordType(rr->RecordType);
				if (rr->AnnounceCount < ReannounceCount)
					rr->AnnounceCount = ReannounceCount;
				rr->ThisAPInterval    = DefaultAPIntervalForRecordType(rr->RecordType);
				rr->LastAPTime        = m->timenow - rr->ThisAPInterval;
				SetNextAnnounceProbeTime(m, rr);
				}
		}

	if (set->Advertise)
		mDNS_AdvertiseInterface(m, set);

	mDNS_Unlock(m);
	return(mStatus_NoError);
	}

// NOTE: mDNS_DeregisterInterface calls mDNS_Deregister_internal which can call a user callback, which may change
// the record list and/or question list.
// Any code walking either list must use the CurrentQuestion and/or CurrentRecord mechanism to protect against this.
mDNSexport void mDNS_DeregisterInterface(mDNS *const m, NetworkInterfaceInfo *set)
	{
	NetworkInterfaceInfo **p = &m->HostInterfaces;
	mDNSBool revalidate = mDNStrue;
	mDNS_Lock(m);

	// Find this record in our list
	while (*p && *p != set) p=&(*p)->next;
	if (!*p) { debugf("mDNS_DeregisterInterface: NetworkInterfaceInfo not found in list"); return; }

	// Unlink this record from our list
	*p = (*p)->next;
	set->next = mDNSNULL;

	if (set->InterfaceActive)
		{
		NetworkInterfaceInfo *intf;
		for (intf = m->HostInterfaces; intf; intf = intf->next)
			if (intf->InterfaceID == set->InterfaceID)
				break;
		if (intf)
			{
			debugf("mDNS_DeregisterInterface: Another representative of InterfaceID %X exists; making it active",
				set->InterfaceID);
			intf->InterfaceActive = mDNStrue;
			}
		else
			{
			ResourceRecord *rr;
			DNSQuestion *q;
			mDNSs32 slot;
			debugf("mDNS_DeregisterInterface: Last representative of InterfaceID %X deregistered; marking questions etc. dormant",
				set->InterfaceID);

			// 1. Deactivate any questions specific to this interface
			for (q = m->Questions; q; q=q->next)
				if (q->InterfaceID == set->InterfaceID)
					q->ThisQInterval = 0;

			// 2. Flush any cache records received on this interface
			revalidate = mDNSfalse;		// Don't revalidate if we're flushing the records
			for (slot = 0; slot < CACHE_HASH_SLOTS; slot++)
				for (rr = m->rrcache_hash[slot]; rr; rr=rr->next)
					if (rr->InterfaceID == set->InterfaceID)
						PurgeCacheResourceRecord(m, rr);

			// 3. Deactivate any authoritative records specific to this interface
			// Any records that are in kDNSRecordTypeDeregistering state will be automatically discarded
			// when BuildResponse finds that their RRInterfaceActive flag is no longer set
			for (rr = m->ResourceRecords; rr; rr=rr->next)
				if (rr->InterfaceID == set->InterfaceID)
					rr->RRInterfaceActive = mDNSfalse;
			}
		}

	// If we were advertising on this interface, deregister those address and reverse-lookup records now
	if (set->Advertise)
		mDNS_DeadvertiseInterface(m, set);

	// If we have any cache records received on this interface that went away, then re-verify them.
	// In some versions of OS X the IPv6 address remains on an interface even when the interface is turned off,
	// giving the false impression that there's an active representative of this interface when there really isn't.
	if (revalidate)
		{
		mDNSs32	slot;
		ResourceRecord *rr;
		m->NextCacheCheck = m->timenow;
		for (slot = 0; slot < CACHE_HASH_SLOTS; slot++)
			for (rr = m->rrcache_hash[slot]; rr; rr=rr->next)
				if (rr->InterfaceID == set->InterfaceID)
					{
					// If we don't get an answer for these in the next five seconds, assume they're gone
					rr->TimeRcvd          = m->timenow;
					rr->rroriginalttl     = 5;
					rr->UnansweredQueries = 0;
					}
		}

	mDNS_Unlock(m);
	}

mDNSlocal void ServiceCallback(mDNS *const m, ResourceRecord *const rr, mStatus result)
	{
	#pragma unused(m)
	ServiceRecordSet *sr = (ServiceRecordSet *)rr->RecordContext;
	switch (result)
		{
		case mStatus_NoError:
			if (rr == &sr->RR_SRV)
				debugf("ServiceCallback: Service RR_SRV %##s Registered", rr->name.c);
			else
				debugf("ServiceCallback: %##s (%s) ERROR Should only get mStatus_NoError callback for RR_SRV",
					rr->name.c, DNSTypeName(rr->rrtype));
			break;

		case mStatus_NameConflict:
			debugf("ServiceCallback: %##s (%s) Name Conflict", rr->name.c, DNSTypeName(rr->rrtype));
			break;

		case mStatus_MemFree:
			if (rr == &sr->RR_PTR)
				debugf("ServiceCallback: Service RR_PTR %##s Memory Free", rr->name.c);
			else
				debugf("ServiceCallback: %##s (%s) ERROR Should only get mStatus_MemFree callback for RR_PTR",
					rr->name.c, DNSTypeName(rr->rrtype));
			break;

		default:
			debugf("ServiceCallback: %##s (%s) Unknown Result %ld", rr->name.c, DNSTypeName(rr->rrtype), result);
			break;
		}

	// If we got a name conflict on either SRV or TXT, forcibly deregister this service, and record that we did that
	if (result == mStatus_NameConflict) { sr->Conflict = mDNStrue; mDNS_DeregisterService(m, sr); return; }
	
	// If this ServiceRecordSet was forcibly deregistered, and now it's memory is ready for reuse,
	// then we can now report the NameConflict to the client
	if (result == mStatus_MemFree && sr->Conflict) result = mStatus_NameConflict;

	// CAUTION: MUST NOT do anything more with sr after calling sr->Callback(), because the client's callback
	// function is allowed to do anything, including deregistering this service and freeing its memory.
	if (sr->ServiceCallback)
		sr->ServiceCallback(m, sr, result);
	}

// Note:
// Name is first label of domain name (any dots in the name are actual dots, not label separators)
// Type is service type (e.g. "_printer._tcp.")
// Domain is fully qualified domain name (i.e. ending with a null label)
// We always register a TXT, even if it is empty (so that clients are not
// left waiting forever looking for a nonexistent record.)
// If the host parameter is mDNSNULL or the root domain (ASCII NUL),
// then the default host name (m->hostname1) is automatically used
mDNSexport mStatus mDNS_RegisterService(mDNS *const m, ServiceRecordSet *sr,
	const domainlabel *const name, const domainname *const type, const domainname *const domain,
	const domainname *const host, mDNSIPPort port, const mDNSu8 txtinfo[], mDNSu16 txtlen,
	const mDNSInterfaceID InterfaceID, mDNSServiceCallback Callback, void *Context)
	{
	mStatus err;

	sr->ServiceCallback = Callback;
	sr->ServiceContext  = Context;
	sr->Conflict = mDNSfalse;
	if (host && host->c[0]) sr->Host = *host;
	else sr->Host.c[0] = 0;
	
	mDNS_SetupResourceRecord(&sr->RR_PTR, mDNSNULL, InterfaceID, kDNSType_PTR, 2*3600, kDNSRecordTypeShared, ServiceCallback, sr);
	mDNS_SetupResourceRecord(&sr->RR_SRV, mDNSNULL, InterfaceID, kDNSType_SRV, 60,     kDNSRecordTypeUnique, ServiceCallback, sr);
 	mDNS_SetupResourceRecord(&sr->RR_TXT, mDNSNULL, InterfaceID, kDNSType_TXT, 60,     kDNSRecordTypeUnique, ServiceCallback, sr);
	
	// If the client is registering an oversized TXT record,
	// it is the client's responsibility to alloate a ServiceRecordSet structure that is large enough for it
	if (sr->RR_TXT.rdata->MaxRDLength < txtlen)
		sr->RR_TXT.rdata->MaxRDLength = txtlen;

	if (ConstructServiceName(&sr->RR_PTR.name, mDNSNULL, type, domain) == mDNSNULL) return(mStatus_BadParamErr);
	if (ConstructServiceName(&sr->RR_SRV.name, name,     type, domain) == mDNSNULL) return(mStatus_BadParamErr);
	sr->RR_TXT.name = sr->RR_SRV.name;
	
	// 1. Set up the PTR record rdata to point to our service name
	// We set up two additionals, so when a client asks for this PTR we automatically send the SRV and the TXT too
	sr->RR_PTR.rdata->u.name = sr->RR_SRV.name;
	sr->RR_PTR.Additional1 = &sr->RR_SRV;
	sr->RR_PTR.Additional2 = &sr->RR_TXT;

	// 2. Set up the SRV record rdata.
	sr->RR_SRV.rdata->u.srv.priority = 0;
	sr->RR_SRV.rdata->u.srv.weight   = 0;
	sr->RR_SRV.rdata->u.srv.port     = port;

	// Setting HostTarget tells DNS that the target of this SRV is to be automatically kept in sync with our host name
	if (sr->Host.c[0]) sr->RR_SRV.rdata->u.srv.target = sr->Host;
	else sr->RR_SRV.HostTarget = mDNStrue;

	// 3. Set up the TXT record rdata,
	// and set DependentOn because we're depending on the SRV record to find and resolve conflicts for us
	if (txtinfo == mDNSNULL) sr->RR_TXT.rdata->RDLength = 0;
	else if (txtinfo != sr->RR_TXT.rdata->u.txt.c)
		{
		sr->RR_TXT.rdata->RDLength = txtlen;
		if (sr->RR_TXT.rdata->RDLength > sr->RR_TXT.rdata->MaxRDLength) return(mStatus_BadParamErr);
		mDNSPlatformMemCopy(txtinfo, sr->RR_TXT.rdata->u.txt.c, txtlen);
		}
	sr->RR_TXT.DependentOn = &sr->RR_SRV;

	// 4. We have no Extras yet
	sr->Extras = mDNSNULL;

	mDNS_Lock(m);
	err = mDNS_Register_internal(m, &sr->RR_SRV);
	if (!err) err = mDNS_Register_internal(m, &sr->RR_TXT);
	// We register the RR_PTR last, because we want to be sure that in the event of a forced call to
	// mDNS_Close, the RR_PTR will be the last one to be forcibly deregistered, since that is what triggers
	// the mStatus_MemFree callback to ServiceCallback, which in turn passes on the mStatus_MemFree back to
	// the client callback, which is then at liberty to free the ServiceRecordSet memory at will. We need to
	// make sure we've deregistered all our records and done any other necessary cleanup before that happens.
	if (!err) err = mDNS_Register_internal(m, &sr->RR_PTR);
	if (err) mDNS_DeregisterService(m, sr);
	mDNS_Unlock(m);

	return(err);
	}

mDNSexport mStatus mDNS_AddRecordToService(mDNS *const m, ServiceRecordSet *sr,
	ExtraResourceRecord *extra, RData *rdata, mDNSu32 ttl)
	{
	ExtraResourceRecord **e = &sr->Extras;
	while (*e) e = &(*e)->next;

	// If TTL is unspecified, make it 60 seconds, the same as the service's TXT and SRV default
	if (ttl == 0) ttl = 60;

	extra->next          = mDNSNULL;
 	mDNS_SetupResourceRecord(&extra->r, rdata, sr->RR_PTR.InterfaceID, extra->r.rrtype, ttl, kDNSRecordTypeUnique, ServiceCallback, sr);
	extra->r.name        = sr->RR_SRV.name;
	extra->r.DependentOn = &sr->RR_SRV;
	
	debugf("mDNS_AddRecordToService adding record to %##s", extra->r.name.c);
	
	*e = extra;
	return(mDNS_Register(m, &extra->r));
	}

mDNSexport mStatus mDNS_RemoveRecordFromService(mDNS *const m, ServiceRecordSet *sr, ExtraResourceRecord *extra)
	{
	ExtraResourceRecord **e = &sr->Extras;
	while (*e && *e != extra) e = &(*e)->next;
	if (!*e)
		{
		debugf("mDNS_RemoveRecordFromService failed to remove record from %##s", extra->r.name.c);
		return(mStatus_BadReferenceErr);
		}

	debugf("mDNS_RemoveRecordFromService removing record from %##s", extra->r.name.c);
	
	*e = (*e)->next;
	return(mDNS_Deregister(m, &extra->r));
	}

mDNSexport mStatus mDNS_RenameAndReregisterService(mDNS *const m, ServiceRecordSet *const sr, const domainlabel *newname)
	{
	domainlabel name;
	domainname type, domain;
	domainname *host = mDNSNULL;
	ExtraResourceRecord *extras = sr->Extras;
	mStatus err;

	DeconstructServiceName(&sr->RR_SRV.name, &name, &type, &domain);
	if (!newname)
		{
		IncrementLabelSuffix(&name, mDNStrue);
		newname = &name;
		}
	debugf("Reregistering as %#s", newname->c);
	if (sr->RR_SRV.HostTarget == mDNSfalse && sr->Host.c[0]) host = &sr->Host;
	
	err = mDNS_RegisterService(m, sr, newname, &type, &domain,
		host, sr->RR_SRV.rdata->u.srv.port, sr->RR_TXT.rdata->u.txt.c, sr->RR_TXT.rdata->RDLength,
		sr->RR_PTR.InterfaceID, sr->ServiceCallback, sr->ServiceContext);

	// mDNS_RegisterService() just reset sr->Extras to NULL.
	// Fortunately we already grabbed ourselves a copy of this pointer (above), so we can now run
	// through the old list of extra records, and re-add them to our freshly created service registration
	while (!err && extras)
		{
		ExtraResourceRecord *e = extras;
		extras = extras->next;
		err = mDNS_AddRecordToService(m, sr, e, e->r.rdata, e->r.rroriginalttl);
		}
	
	return(err);
	}

// NOTE: mDNS_DeregisterService calls mDNS_Deregister_internal which can call a user callback,
// which may change the record list and/or question list.
// Any code walking either list must use the CurrentQuestion and/or CurrentRecord mechanism to protect against this.
mDNSexport mStatus mDNS_DeregisterService(mDNS *const m, ServiceRecordSet *sr)
	{
	mStatus status;
	ExtraResourceRecord *e;
	mDNS_Lock(m);
	e = sr->Extras;

	// We use mDNS_Dereg_repeat because, in the event of a collision, some or all of
	// these records could have already been automatically deregistered, and that's okay
	mDNS_Deregister_internal(m, &sr->RR_SRV, mDNS_Dereg_repeat);
	mDNS_Deregister_internal(m, &sr->RR_TXT, mDNS_Dereg_repeat);
	
	// We deregister all of the extra records, but we leave the sr->Extras list intact
	// in case the client wants to do a RenameAndReregister and reinstate the registration
	while (e)
		{
		mDNS_Deregister_internal(m, &e->r, mDNS_Dereg_repeat);
		e = e->next;
		}

	// Be sure to deregister the PTR last!
	// Deregistering this record is what triggers the mStatus_MemFree callback to ServiceCallback,
	// which in turn passes on the mStatus_MemFree (or mStatus_NameConflict) back to the client callback,
	// which is then at liberty to free the ServiceRecordSet memory at will. We need to make sure
	// we've deregistered all our records and done any other necessary cleanup before that happens.
	status = mDNS_Deregister_internal(m, &sr->RR_PTR, mDNS_Dereg_normal);
	mDNS_Unlock(m);
	return(status);
	}

// Create a registration that asserts that no such service exists with this name.
// This can be useful where there is a given function is available through several protocols.
// For example, a printer called "Stuart's Printer" may implement printing via the "pdl-datastream" and "IPP"
// protocols, but not via "LPR". In this case it would be prudent for the printer to assert the non-existence of an
// "LPR" service called "Stuart's Printer". Without this precaution, another printer than offers only "LPR" printing
// could inadvertently advertise its service under the same name "Stuart's Printer", which might be confusing for users.
mDNSexport mStatus mDNS_RegisterNoSuchService(mDNS *const m, ResourceRecord *const rr,
	const domainlabel *const name, const domainname *const type, const domainname *const domain,
	const domainname *const host,
	const mDNSInterfaceID InterfaceID, mDNSRecordCallback Callback, void *Context)
	{
	mDNS_SetupResourceRecord(rr, mDNSNULL, InterfaceID, kDNSType_SRV, 60, kDNSRecordTypeUnique, Callback, Context);
	if (ConstructServiceName(&rr->name, name, type, domain) == mDNSNULL) return(mStatus_BadParamErr);
	rr->rdata->u.srv.priority    = 0;
	rr->rdata->u.srv.weight      = 0;
	rr->rdata->u.srv.port        = zeroIPPort;
	if (host && host->c[0]) rr->rdata->u.srv.target = *host;
	else rr->HostTarget = mDNStrue;
	return(mDNS_Register(m, rr));
	}

mDNSexport mStatus mDNS_AdvertiseDomains(mDNS *const m, ResourceRecord *rr,
	mDNSu8 DomainType, const mDNSInterfaceID InterfaceID, char *domname)
	{
	mDNS_SetupResourceRecord(rr, mDNSNULL, InterfaceID, kDNSType_PTR, 2*3600, kDNSRecordTypeShared, mDNSNULL, mDNSNULL);
	if (!MakeDomainNameFromDNSNameString(&rr->name, mDNS_DomainTypeNames[DomainType])) return(mStatus_BadParamErr);
	if (!MakeDomainNameFromDNSNameString(&rr->rdata->u.name, domname))                 return(mStatus_BadParamErr);
	return(mDNS_Register(m, rr));
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark -
#pragma mark - Startup and Shutdown
#endif

mDNSexport mStatus mDNS_Init(mDNS *const m, mDNS_PlatformSupport *const p,
	ResourceRecord *rrcachestorage, mDNSu32 rrcachesize,
	mDNSBool AdvertiseLocalAddresses, mDNSCallback *Callback, void *Context)
	{
	mDNSs32 timenow = mDNSPlatformTimeNow();
	mStatus result;
	mDNSu32 i;
	
	if (!rrcachestorage) rrcachesize = 0;
	
	m->p                       = p;
	m->AdvertiseLocalAddresses = AdvertiseLocalAddresses;
	m->mDNSPlatformStatus      = mStatus_Waiting;
	m->MainCallback            = Callback;
	m->MainContext             = Context;

	// For debugging: To catch and report locking failures
	m->mDNS_busy               = 0;
	m->mDNS_reentrancy         = 0;
	m->lock_rrcache            = 0;
	m->lock_Questions          = 0;
	m->lock_Records            = 0;

	// Task Scheduling variables
	m->timenow                 = 0;		// MUST only be set within mDNS_Lock/mDNS_Unlock section
	m->NextScheduledEvent      = timenow;
	m->SuppressSending         = timenow;
	m->NextCacheCheck          = timenow + 0x78000000;
	m->NextScheduledQuery      = timenow + 0x78000000;
	m->NextProbeTime           = timenow + 0x78000000;
	m->NextResponseTime        = timenow + 0x78000000;
	m->SendDeregistrations     = mDNSfalse;
	m->SendImmediateAnswers    = mDNSfalse;
	m->SleepState              = mDNSfalse;

	// These fields only required for mDNS Searcher...
	m->Questions               = mDNSNULL;
	m->NewQuestions            = mDNSNULL;
	m->CurrentQuestion         = mDNSNULL;
	m->rrcache_size            = rrcachesize;
	for (i = 0; i < CACHE_HASH_SLOTS; i++) m->rrcache_used[i] = 0;
	m->rrcache_totalused       = 0;
	m->rrcache_report          = 10;
	m->rrcache_free            = rrcachestorage;
	if (rrcachesize)
		{
		for (i=0; i<rrcachesize; i++) rrcachestorage[i].next = &rrcachestorage[i+1];
		rrcachestorage[rrcachesize-1].next = mDNSNULL;
		}
	for (i = 0; i < CACHE_HASH_SLOTS; i++) m->rrcache_hash[i] = mDNSNULL;

	// Fields below only required for mDNS Responder...
	m->hostlabel.c[0]          = 0;
	m->nicelabel.c[0]          = 0;
	m->hostname1.c[0]          = 0;
	m->hostname2.c[0]          = 0;
	m->ResourceRecords         = mDNSNULL;
	m->CurrentRecord           = mDNSNULL;
	m->HostInterfaces          = mDNSNULL;
	m->ProbeFailTime           = 0;
	m->NumFailedProbes         = 0;
	m->SuppressProbes          = 0;

	result = mDNSPlatformInit(m);
	
	return(result);
	}

extern void mDNSCoreInitComplete(mDNS *const m, mStatus result)
	{
	m->mDNSPlatformStatus = result;
	if (m->MainCallback)
		m->MainCallback(m, mStatus_NoError);
	}

extern void mDNS_Close(mDNS *const m)
	{
	NetworkInterfaceInfo *intf;
	mDNS_Lock(m);

#if MDNS_DEBUGMSGS
	{
	ResourceRecord *rr;
	int rrcache_active = 0;
	mDNSs32	slot;
	for (slot = 0; slot < CACHE_HASH_SLOTS; slot++)
		for (rr = m->rrcache_hash[slot]; rr; rr=rr->next) if (rr->CRActiveQuestion) rrcache_active++;
	debugf("mDNS_Close: RR Cache now using %ld records, %d active", m->rrcache_totalused, rrcache_active);
	}
#endif

	m->Questions = mDNSNULL;		// We won't be answering any more questions!
	
	for (intf = m->HostInterfaces; intf; intf = intf->next)
		if (intf->Advertise)
			mDNS_DeadvertiseInterface(m, intf);

	// Make sure there are nothing but deregistering records remaining in the list
	if (m->CurrentRecord) debugf("mDNS_Close ERROR m->CurrentRecord already set");
	m->CurrentRecord = m->ResourceRecords;
	while (m->CurrentRecord)
		{
		ResourceRecord *rr = m->CurrentRecord;
		m->CurrentRecord = rr->next;
		if (rr->RecordType != kDNSRecordTypeDeregistering)
			{
			debugf("mDNS_Close: Record type %X still in ResourceRecords list %##s", rr->RecordType, rr->name.c);
			mDNS_Deregister_internal(m, rr, mDNS_Dereg_normal);
			}
		}

	if (m->ResourceRecords) debugf("mDNS_Close: Sending final packets for deregistering records");
	else debugf("mDNS_Close: No deregistering records remain");

	// If any deregistering records remain, send their deregistration announcements before we exit
	if (m->mDNSPlatformStatus != mStatus_NoError)
		DiscardDeregistrations(m);
	else
		while (m->ResourceRecords)
			SendResponses(m);
	
	mDNS_Unlock(m);
	debugf("mDNS_Close: mDNSPlatformClose");
	mDNSPlatformClose(m);
	debugf("mDNS_Close: done");
	}
