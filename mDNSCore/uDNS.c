/*
 * Copyright (c) 2002-2004 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@

    Change History (most recent first):

$Log: uDNS.c,v $
Revision 1.94  2004/10/12 02:49:20  ksekar
<rdar://problem/3831228> Clean up LLQ sleep/wake, error handling

Revision 1.93  2004/10/08 04:17:25  ksekar
<rdar://problem/3831819> Don't use DNS extensions if the server does not advertise required SRV record

Revision 1.92  2004/10/08 03:54:35  ksekar
<rdar://problem/3831802> Refine unicast polling intervals

Revision 1.91  2004/09/30 17:45:34  ksekar
<rdar://problem/3821119> lots of log messages: mDNS_SetPrimaryIP: IP address unchanged

Revision 1.90  2004/09/25 00:22:13  ksekar
<rdar://problem/3815534> Crash in uDNS_RegisterService

Revision 1.89  2004/09/24 19:14:53  cheshire
Remove unused "extern mDNS mDNSStorage"

Revision 1.88  2004/09/23 20:48:15  ksekar
Clarify retransmission debugf messages.

Revision 1.87  2004/09/22 00:41:59  cheshire
Move tcp connection status codes into the legal range allocated for mDNS use

Revision 1.86  2004/09/21 23:40:11  ksekar
<rdar://problem/3810349> mDNSResponder to return errors on NAT traversal failure

Revision 1.85  2004/09/21 22:38:27  ksekar
<rdar://problem/3810286> PrimaryIP type uninitialized

Revision 1.84  2004/09/18 00:30:39  cheshire
<rdar://problem/3806643> Infinite loop in CheckServiceRegistrations

Revision 1.83  2004/09/17 00:31:51  cheshire
For consistency with ipv6, renamed rdata field 'ip' to 'ipv4'

Revision 1.82  2004/09/16 21:36:36  cheshire
<rdar://problem/3803162> Fix unsafe use of mDNSPlatformTimeNow()
Changes to add necessary locking calls around unicast DNS operations

Revision 1.81  2004/09/16 02:29:39  cheshire
Moved mDNS_Lock/mDNS_Unlock to DNSCommon.c; Added necessary locking around
uDNS_ReceiveMsg, uDNS_StartQuery, uDNS_UpdateRecord, uDNS_RegisterService

Revision 1.80  2004/09/16 01:58:21  cheshire
Fix compiler warnings

Revision 1.79  2004/09/16 00:24:48  cheshire
<rdar://problem/3803162> Fix unsafe use of mDNSPlatformTimeNow()

Revision 1.78  2004/09/15 01:16:57  ksekar
<rdar://problem/3797598> mDNSResponder printing too many messages

Revision 1.77  2004/09/14 23:27:47  cheshire
Fix compile errors

Revision 1.76  2004/09/14 22:22:00  ksekar
<rdar://problem/3800920> Legacy browses broken against some BIND versions

Revision 1.75  2004/09/03 19:23:05  ksekar
<rdar://problem/3788460>: Need retransmission mechanism for wide-area service registrations

Revision 1.74  2004/09/02 17:49:04  ksekar
<rdar://problem/3785135>: 8A246: mDNSResponder crash while logging on restart
Fixed incorrect conversions, changed %s to %##s for all domain names.

Revision 1.73  2004/09/02 01:39:40  cheshire
For better readability, follow consistent convention that QR bit comes first, followed by OP bits

Revision 1.72  2004/09/01 03:59:29  ksekar
<rdar://problem/3783453>: Conditionally compile out uDNS code on Windows

Revision 1.71  2004/08/27 17:51:53  ksekar
Replaced unnecessary LogMsg with debugf.

Revision 1.70  2004/08/25 00:37:27  ksekar
<rdar://problem/3774635>: Cleanup DynDNS hostname registration code

Revision 1.69  2004/08/18 17:35:41  ksekar
<rdar://problem/3651443>: Feature #9586: Need support for Legacy NAT gateways

Revision 1.68  2004/08/14 03:22:41  cheshire
<rdar://problem/3762579> Dynamic DNS UI <-> mDNSResponder glue
Add GetUserSpecifiedDDNSName() routine
Convert ServiceRegDomain to domainname instead of C string
Replace mDNS_GenerateFQDN/mDNS_GenerateGlobalFQDN with mDNS_SetFQDNs

Revision 1.67  2004/08/13 23:46:58  cheshire
"asyncronous" -> "asynchronous"

Revision 1.66  2004/08/13 23:37:02  cheshire
Now that we do both uDNS and mDNS, global replace "uDNS_info.hostname" with
"uDNS_info.UnicastHostname" for clarity

Revision 1.65  2004/08/13 23:12:32  cheshire
Don't use strcpy() and strlen() on "struct domainname" objects;
use AssignDomainName() and DomainNameLength() instead
(A "struct domainname" is a collection of packed pascal strings, not a C string.)

Revision 1.64  2004/08/13 23:01:05  cheshire
Use platform-independent mDNSNULL instead of NULL

Revision 1.63  2004/08/12 00:32:36  ksekar
<rdar://problem/3759567>: LLQ Refreshes never terminate if unanswered

Revision 1.62  2004/08/10 23:19:14  ksekar
<rdar://problem/3722542>: DNS Extension daemon for Wide Area Service Discovery
Moved routines/constants to allow extern access for garbage collection daemon

Revision 1.61  2004/07/30 17:40:06  ksekar
<rdar://problem/3739115>: TXT Record updates not available for wide-area services

Revision 1.60  2004/07/29 19:40:05  ksekar
NAT-PMP Support - minor fixes and cleanup

Revision 1.59  2004/07/29 19:27:15  ksekar
NAT-PMP Support - minor fixes and cleanup

Revision 1.58  2004/07/27 07:35:38  shersche
fix syntax error, variables declared in the middle of a block

Revision 1.57  2004/07/26 22:49:30  ksekar
<rdar://problem/3651409>: Feature #9516: Need support for NAT-PMP in client

Revision 1.56  2004/07/26 19:14:44  ksekar
<rdar://problem/3737814>: 8A210: mDNSResponder crashed in startLLQHandshakeCallback

Revision 1.55  2004/07/15 19:01:33  ksekar
<rdar://problem/3681029>: Check for incorrect time comparisons

Revision 1.54  2004/06/22 02:10:53  ksekar
<rdar://problem/3705433>: Lighthouse failure causes packet flood to DNS

Revision 1.53  2004/06/17 20:49:09  ksekar
<rdar://problem/3690436>: mDNSResponder crash while location cycling

Revision 1.52  2004/06/17 01:13:11  ksekar
<rdar://problem/3696616>: polling interval too short

Revision 1.51  2004/06/10 04:36:44  cheshire
Fix compiler warning

Revision 1.50  2004/06/10 00:55:13  ksekar
<rdar://problem/3686213>: crash on network reconnect

Revision 1.49  2004/06/10 00:10:50  ksekar
<rdar://problem/3686174>: Infinite Loop in uDNS_Execute()

Revision 1.48  2004/06/09 20:03:37  ksekar
<rdar://problem/3686163>: Incorrect copying of resource record in deregistration

Revision 1.47  2004/06/09 03:48:28  ksekar
<rdar://problem/3685226>: nameserver address fails with prod. Lighthouse server

Revision 1.46  2004/06/09 01:44:30  ksekar
<rdar://problem/3681378> reworked Cache Record copy code

Revision 1.45  2004/06/08 18:54:47  ksekar
<rdar://problem/3681378>: mDNSResponder leaks after exploring in Printer Setup Utility

Revision 1.44  2004/06/05 00:33:51  cheshire
<rdar://problem/3681029>: Check for incorrect time comparisons

Revision 1.43  2004/06/05 00:14:44  cheshire
Fix signed/unsigned and other compiler warnings

Revision 1.42  2004/06/04 22:36:16  ksekar
Properly set u->nextevent in uDNS_Execute

Revision 1.41  2004/06/04 08:58:29  ksekar
<rdar://problem/3668624>: Keychain integration for secure dynamic update

Revision 1.40  2004/06/03 03:09:58  ksekar
<rdar://problem/3668626>: Garbage Collection for Dynamic Updates

Revision 1.39  2004/06/01 23:46:50  ksekar
<rdar://problem/3675149>: DynDNS: dynamically look up LLQ/Update ports

Revision 1.38  2004/05/31 22:19:44  ksekar
<rdar://problem/3258021>: Feature: DNS server->client notification on
record changes (#7805) - revert to polling mode on setup errors

Revision 1.37  2004/05/28 23:42:37  ksekar
<rdar://problem/3258021>: Feature: DNS server->client notification on record changes (#7805)

Revision 1.36  2004/05/18 23:51:25  cheshire
Tidy up all checkin comments to use consistent "<rdar://problem/xxxxxxx>" format for bug numbers

Revision 1.35  2004/05/07 23:01:04  ksekar
Cleaned up list traversal in deriveGoodbyes - removed unnecessary
conditional assignment.

Revision 1.34  2004/05/05 18:26:12  ksekar
Periodically re-transmit questions if the send() fails.  Include
internal questions in retransmission.

Revision 1.33  2004/05/05 17:40:06  ksekar
Removed prerequisite from deregistration update - it does not work for
shared records, and is unnecessary until we have more sophisticated
name conflict management.

Revision 1.32  2004/05/05 17:32:18  ksekar
Prevent registration of loopback interface caused by removal of
Multicast flag in interface structure.

Revision 1.31  2004/05/05 17:05:02  ksekar
Use LargeCacheRecord structs when pulling records off packets

Revision 1.30  2004/04/16 21:33:27  ksekar
Fixed bug in processing GetZoneData responses that do not use BIND formatting.

Revision 1.29  2004/04/15 20:03:13  ksekar
Clarified log message when pulling bad resource records off packet.

Revision 1.28  2004/04/15 00:51:28  bradley
Minor tweaks for Windows and C++ builds. Added casts for signed/unsigned integers and 64-bit pointers.
Prefix some functions with mDNS to avoid conflicts. Disable benign warnings on Microsoft compilers.

Revision 1.27  2004/04/14 23:09:28  ksekar
Support for TSIG signed dynamic updates.

Revision 1.26  2004/04/14 19:36:05  ksekar
Fixed memory corruption error in deriveGoodbyes.

Revision 1.25  2004/04/14 04:07:11  ksekar
Fixed crash in IsActiveUnicastQuery().  Removed redundant checks in routine.

Revision 1.24  2004/04/08 09:41:40  bradley
Added const to AuthRecord in deadvertiseIfCallback to match callback typedef.

Revision 1.23  2004/03/24 00:29:45  ksekar
Make it safe to call StopQuery in a unicast question callback

Revision 1.22  2004/03/19 10:11:09  bradley
Added AuthRecord * cast from umalloc for C++ builds.

Revision 1.21  2004/03/15 02:03:45  bradley
Added const to params where needed to match prototypes. Changed SetNewRData calls to use 0 instead
of -1 for unused size to fix warning. Disable assignment within conditional warnings with Visual C++.

Revision 1.20  2004/03/13 02:07:26  ksekar
<rdar://problem/3192546>: DynDNS: Dynamic update of service records

Revision 1.19  2004/03/13 01:57:33  ksekar
<rdar://problem/3192546>: DynDNS: Dynamic update of service records

Revision 1.18  2004/02/21 08:34:15  bradley
Added casts from void * to specific type for C++ builds. Changed void * l-value cast
r-value cast to fix problems with VC++ builds. Removed empty switch to fix VC++ error.

Revision 1.17  2004/02/21 02:06:24  cheshire
Can't use anonymous unions -- they're non-standard and don't work on all compilers

Revision 1.16  2004/02/12 01:51:45  cheshire
Don't try to send uDNS queries unless we have at least one uDNS server available

Revision 1.15  2004/02/10 03:02:46  cheshire
Fix compiler warning

Revision 1.14  2004/02/06 23:04:19  ksekar
Basic Dynamic Update support via mDNS_Register (dissabled via
UNICAST_REGISTRATION #define)

Revision 1.13  2004/02/03 22:15:01  ksekar
Fixed nameToAddr error check: don't abort state machine on nxdomain error.

Revision 1.12  2004/02/03 19:47:36  ksekar
Added an asynchronous state machine mechanism to uDNS.c, including
calls to find the parent zone for a domain name.  Changes include code
in repository previously dissabled via "#if 0 //incomplete".  Codepath
is currently unused, and will be called to create update records, etc.

Revision 1.11  2004/01/30 02:12:30  ksekar
Changed uDNS_ReceiveMsg() to correctly return void.

Revision 1.10  2004/01/29 02:59:17  ksekar
Unicast DNS: Changed from a resource record oriented question/response
matching to packet based matching.  New callback architecture allows
collections of records in a response to be processed differently
depending on the nature of the request, and allows the same structure
to be used for internal and client-driven queries with different processing needs.

Revision 1.9  2004/01/28 20:20:45  ksekar
Unified ActiveQueries and ActiveInternalQueries lists, using a flag to
demux them.  Check-in includes work-in-progress code, #ifdef'd out.

Revision 1.8  2004/01/28 02:30:07  ksekar
Added default Search Domains to unicast browsing, controlled via
Networking sharing prefs pane.  Stopped sending unicast messages on
every interface.  Fixed unicast resolving via mach-port API.

Revision 1.7  2004/01/27 20:15:22  cheshire
<rdar://problem/3541288>: Time to prune obsolete code for listening on port 53

Revision 1.6  2004/01/24 23:47:17  cheshire
Use mDNSOpaque16fromIntVal() instead of shifting and masking

Revision 1.5  2004/01/24 04:59:15  cheshire
Fixes so that Posix/Linux, OS9, Windows, and VxWorks targets build again

Revision 1.4  2004/01/24 04:19:26  cheshire
Restore overwritten checkin 1.2

Revision 1.3  2004/01/23 23:23:15  ksekar
Added TCP support for truncated unicast messages.

Revision 1.2  2004/01/22 03:48:41  cheshire
Make sure uDNS client doesn't accidentally use query ID zero

Revision 1.1  2003/12/13 03:05:27  ksekar
<rdar://problem/3192548>: DynDNS: Unicast query of service records

 */

#include "uDNS.h"

#if(defined(_MSC_VER))
	// Disable "assignment within conditional expression".
	// Other compilers understand the convention that if you place the assignment expression within an extra pair
	// of parentheses, this signals to the compiler that you really intended an assignment and no warning is necessary.
	// The Microsoft compiler doesn't understand this convention, so in the absense of any other way to signal
	// to the compiler that the assignment is intentional, we have to just turn this warning off completely.
	#pragma warning(disable:4706)
#endif

#define umalloc(x)         mDNSPlatformMemAllocate(x)       // short hands for common routines
#define ufree(x)           mDNSPlatformMemFree(x)
#define ubzero(x,y)        mDNSPlatformMemZero(x,y)
#define umemcpy(x, y, l)   mDNSPlatformMemCopy(y, x, l)  // uses memcpy(2) arg ordering

// Asynchronous operation types

typedef enum
	{
	zoneDataResult
	// other async. operation names go here
	} AsyncOpResultType;

typedef struct
	{
    domainname zoneName; 
    mDNSAddr primaryAddr;
    mDNSu16 zoneClass;
    mDNSIPPort llqPort;
    mDNSIPPort updatePort;
	} zoneData_t;

// other async. result struct defs go here

typedef struct
	{
    AsyncOpResultType type;
    zoneData_t zoneData;
    // other async result structs go here
	} AsyncOpResult;

typedef void AsyncOpCallback(mStatus err, mDNS *const m, void *info, const AsyncOpResult *result); 


// Private Function Prototypes
// Note:  In general, functions are ordered such that they do not require forward declarations.
// However, prototypes are used where cyclic call graphs exist (e.g. foo calls bar, and bar calls
// foo), or when they aid in the grouping or readability of code (e.g. state machine code that is easier
// read top-to-bottom.)

mDNSlocal void hndlTruncatedAnswer(DNSQuestion *question,  const mDNSAddr *src, mDNS *m);
mDNSlocal mStatus startGetZoneData(domainname *name, mDNS *m, mDNSBool findUpdatePort, mDNSBool findLLQPort, AsyncOpCallback callback, void *callbackInfo);
mDNSlocal mDNSBool recvLLQResponse(mDNS *m, DNSMessage *msg, const mDNSu8 *end, const mDNSAddr *srcaddr, mDNSIPPort srcport, const mDNSInterfaceID InterfaceID);
mDNSlocal void sendRecordRegistration(mDNS *const m, AuthRecord *rr);
mDNSlocal void SendServiceRegistration(mDNS *m, ServiceRecordSet *srs);
mDNSlocal void serviceRegistrationCallback(mStatus err, mDNS *const m, void *srsPtr, const AsyncOpResult *result);
mDNSlocal void SendRecordUpdate(mDNS *m, AuthRecord *rr, uDNS_RegInfo *info);
mDNSlocal mStatus RegisterService(mDNS *m, ServiceRecordSet *srs);
mDNSlocal void UpdateServiceTargets(mDNS *const m);

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Temporary workaround
#endif

// 17 Places in this file directly call mDNSPlatformTimeNow(), which is unsafe
// The platform function is now called mDNSPlatformRawTime(), and
// mDNSPlatformTimeNow() is defined here as a temporary workaround.
// This is a gross hack, and after this change has been tested for a while,
// all these calls should be replaced by simple references to m->timenow

mDNSlocal mDNSs32 mDNSPlatformTimeNow(mDNS *m)
	{
	if (m->mDNS_busy && m->timenow) return(m->timenow);
	LogMsg("ERROR: uDNS.c code executing without holding main mDNS lock");

	// To get a quick and easy stack trace to find out *how* this routine
	// is being called without holding main mDNS lock, uncomment the line below:
	//*(long*)0=0;
	
	return(mDNS_TimeNow(m));
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - General Utility Functions
#endif

mDNSlocal mDNSOpaque16 newMessageID(uDNS_GlobalInfo *u)
	{
	// if NextMessageID is 0 (ininitialized) or 0xffff (reserved for TCP packets) reset to 1
	if (!u->NextMessageID || u->NextMessageID == (mDNSu16)~0) u->NextMessageID = 1;
	return mDNSOpaque16fromIntVal(u->NextMessageID++);
	}

// unlink an AuthRecord from a linked list
mDNSlocal mStatus unlinkAR(AuthRecord **list, AuthRecord *const rr)
	{
	AuthRecord *rptr, *prev = mDNSNULL;
	
	for (rptr = *list; rptr; rptr = rptr->next)
		{
		if (rptr == rr)
			{
			if (prev) prev->next = rptr->next;
			else *list  = rptr->next;
			rptr->next = mDNSNULL;
			return mStatus_NoError;
			}
		prev = rptr;
		}
	LogMsg("ERROR: unlinkAR - no such active record");
	return mStatus_UnknownErr;
	}

mDNSlocal void unlinkSRS(uDNS_GlobalInfo *u, ServiceRecordSet *srs)
	{
	ServiceRecordSet **p;
	for (p = &u->ServiceRegistrations; *p; p = &(*p)->next)
		if (*p == srs) { *p = srs->next; srs->next = mDNSNULL; return; }
	LogMsg("ERROR: unlinkSRS - SRS not found in ServiceRegistrations list");
	}

mDNSlocal void LinkActiveQuestion(uDNS_GlobalInfo *u, DNSQuestion *q)
	{
	if (uDNS_IsActiveQuery(q, u))
		{ LogMsg("LinkActiveQuestion - %##s (%d) already in list!", q->qname.c, q->qtype); return; }
	
	q->next = u->ActiveQueries;
	u->ActiveQueries = q;
	}

mDNSlocal void SwapRData(mDNS *m, AuthRecord *rr, mDNSBool DeallocOld)
	{
	RData *oldrd = rr->resrec.rdata;
	mDNSu16 oldrdlen = rr->resrec.rdlength;

	if (!rr->uDNS_info.UpdateRData) { LogMsg("SwapRData invoked with NULL UpdateRData field"); return; }
	SetNewRData(&rr->resrec, rr->uDNS_info.UpdateRData, rr->uDNS_info.UpdateRDLen);
	if (DeallocOld)
		{
		rr->uDNS_info.UpdateRData = mDNSNULL;							    // Clear the NewRData pointer ...
		if (rr->uDNS_info.UpdateRDCallback) rr->uDNS_info.UpdateRDCallback(m, rr, oldrd);					// ... and let the client know
		}
	else
		{
		rr->uDNS_info.UpdateRData = oldrd;
		rr->uDNS_info.UpdateRDLen = oldrdlen;
		}
	}

// set retry timestamp for record with exponential backoff
// (for service record sets, use RR_SRV as representative for time checks
mDNSlocal void SetRecordRetry(mDNS *const m, AuthRecord *rr)
	{
	rr->LastAPTime = mDNSPlatformTimeNow(m);	
	if (rr->ThisAPInterval < INIT_UCAST_POLL_INTERVAL)   { rr->ThisAPInterval = INIT_UCAST_POLL_INTERVAL; return; }
	if (rr->ThisAPInterval*2 <= MAX_UCAST_POLL_INTERVAL) { rr->ThisAPInterval *= 2;                       return; }
	if (rr->ThisAPInterval != MAX_UCAST_POLL_INTERVAL)   { rr->ThisAPInterval = MAX_UCAST_POLL_INTERVAL;          }
	}
	

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Name Server List Management
#endif

mDNSexport void mDNS_RegisterDNS(mDNS *const m, mDNSv4Addr *const dnsAddr)
    {
    //!!!KRS do this dynamically!
    uDNS_GlobalInfo *u = &m->uDNS_info;
    int i;

	if (!dnsAddr->NotAnInteger)
        {
        LogMsg("ERROR: attempt to register DNS with IP address 0");
        return;
        }

	mDNS_Lock(m);

    for (i = 0; i < 32; i++)
        {
        if (!u->Servers[i].ip.v4.NotAnInteger)
            {
            u->Servers[i].ip.v4.NotAnInteger = dnsAddr->NotAnInteger;
			u->Servers[i].type = mDNSAddrType_IPv4;
            goto exit;
            }
        if (u->Servers[i].ip.v4.NotAnInteger == dnsAddr->NotAnInteger)
            {
            LogMsg("ERROR: mDNS_RegisterDNS - DNS already registered");
            goto exit;
            }
        }
    if (i == 32) {  LogMsg("ERROR: mDNS_RegisterDNS - too many registered servers");  }
exit:
	mDNS_Unlock(m);
    }

mDNSexport void mDNS_DeregisterDNS(mDNS *const m, mDNSv4Addr *const dnsAddr)
    {
    uDNS_GlobalInfo *u = &m->uDNS_info;
    int i;

    if (!dnsAddr->NotAnInteger)
        {
        LogMsg("ERROR: attempt to deregister DNS with IP address 0");
        return;
        }

	mDNS_Lock(m);

    for (i = 0; i < 32; i++)
        {

        if (u->Servers[i].ip.v4.NotAnInteger == dnsAddr->NotAnInteger)
            {
            u->Servers[i].ip.v4.NotAnInteger = 0;
            goto exit;
            }
        }
    if (i == 32) {  LogMsg("ERROR: mDNS_DeregisterDNS - no such DNS registered");  }
 exit:
	mDNS_Unlock(m);
   }

mDNSexport void mDNS_DeregisterDNSList(mDNS *const m)
    {
	mDNS_Lock(m);
    ubzero(m->uDNS_info.Servers, 32 * sizeof(mDNSAddr));
	mDNS_Unlock(m);
    }

mDNSexport mDNSBool mDNS_DNSRegistered(mDNS *const m)
	{
	int i;
	mDNS_Lock(m);
	for (i = 0; i < 32; i++) if (m->uDNS_info.Servers[i].ip.v4.NotAnInteger)
		{ mDNS_Unlock(m); return mDNStrue; }
	mDNS_Unlock(m);
	return mDNSfalse;
	}
	   

 // ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - authorization management
#endif


mDNSlocal uDNS_AuthInfo *GetAuthInfoForZone(const uDNS_GlobalInfo *u, const domainname *zone)
	{
	uDNS_AuthInfo *ptr;
	while (zone->c[0])
		{
		for (ptr = u->AuthInfoList; ptr; ptr = ptr->next)
			if (SameDomainName(&ptr->zone, zone)) return(ptr);
		zone = (const domainname *)(zone->c + 1 + zone->c[0]);
		}
	return mDNSNULL;
	}	

mDNSlocal void DeleteAuthInfoForZone(uDNS_GlobalInfo *u, const domainname *zone)
	{
	uDNS_AuthInfo *ptr, *prev = mDNSNULL;

	for (ptr = u->AuthInfoList; ptr; ptr = ptr->next)
		{
		if (SameDomainName(&ptr->zone, zone))
			{
			if (prev) prev->next = ptr->next;
			else u->AuthInfoList = ptr->next;
			ufree(ptr);
			return;
			}
		prev = ptr;
		}
	}

mDNSexport mStatus mDNS_SetSecretForZone(mDNS *m, domainname *zone, domainname *key, mDNSu8 *sharedSecret, mDNSu32 ssLen, mDNSBool base64)
	{
	uDNS_AuthInfo *info;
	mDNSu8 keybuf[1024];
	mDNSs32 keylen;
	uDNS_GlobalInfo *u = &m->uDNS_info;
	mStatus status = mStatus_NoError;

	mDNS_Lock(m);
	
	if (GetAuthInfoForZone(u, zone)) DeleteAuthInfoForZone(u, zone);
	if (!key) goto exit;
	
	info = (uDNS_AuthInfo*)umalloc(sizeof(uDNS_AuthInfo) + ssLen);
	if (!info) { LogMsg("ERROR: umalloc"); status = mStatus_NoMemoryErr; goto exit; }
   	ubzero(info, sizeof(uDNS_AuthInfo));
	AssignDomainName(info->zone, *zone);
	AssignDomainName(info->keyname, *key);

	if (base64)
		{
		keylen = DNSDigest_Base64ToBin((const char*)sharedSecret, keybuf, 1024);
		if (keylen < 0)
			{
			LogMsg("ERROR: mDNS_UpdateDomainRequiresAuthentication - could not convert shared secret from base64");
			ufree(info);
			status = mStatus_UnknownErr;
			goto exit;
			}		
		DNSDigest_ConstructHMACKey(info, keybuf, (mDNSu32)keylen);		
		}
	else DNSDigest_ConstructHMACKey(info, sharedSecret, ssLen);

    // link into list
	info->next = m->uDNS_info.AuthInfoList;
	m->uDNS_info.AuthInfoList = info;
exit:
	mDNS_Unlock(m);
	return status;
	}
	
 // ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - NAT Traversal
#endif

mDNSlocal mDNSBool MapServicePort(mDNS *m)
	{
	uDNS_HostnameInfo *i;

	//!!!KRS this could be cached
	for (i = m->uDNS_info.Hostnames; i; i= i->next)
		if (i->ar->uDNS_info.NATinfo && i->ar->uDNS_info.NATinfo->state != NATState_Error) return mDNStrue;
	
	return mDNSfalse;
	}

mDNSlocal mDNSBool DomainContainsLabelString(const domainname *d, const char *str)
	{
	const domainlabel *l;
	domainlabel buf;

	if (!MakeDomainLabelFromLiteralString(&buf, str)) return mDNSfalse;

	for (l = (const domainlabel *)d; l->c[0]; l = (const domainlabel *)(l->c + l->c[0]+1))
		if (SameDomainLabel(l->c, buf.c)) return mDNStrue;
	return mDNSfalse;
	}

// allocate struct, link into global list, initialize
mDNSlocal NATTraversalInfo *AllocNATInfo(mDNS *const m, NATOp_t op, NATResponseHndlr callback)
	{
	uDNS_GlobalInfo *u = &m->uDNS_info;
	NATTraversalInfo *info = umalloc(sizeof(NATTraversalInfo));
	if (!info) { LogMsg("ERROR: malloc"); return mDNSNULL; }
	ubzero(info, sizeof(NATTraversalInfo));
	info->next = u->NATTraversals;
	u->NATTraversals = info;
	info->retry = mDNSPlatformTimeNow(m) + NATMAP_INIT_RETRY;
	info->op = op;
	info->state = NATState_Init;
	info->ReceiveResponse = callback;
	info->PublicPort.NotAnInteger = 0;
	return info;
	}

// unlink from list, deallocate
mDNSlocal mDNSBool FreeNATInfo(mDNS *m, NATTraversalInfo *n)
	{
	NATTraversalInfo *ptr, *prev = mDNSNULL;

	ptr = m->uDNS_info.NATTraversals;
	while (ptr)
		{
		if (ptr == n)
			{
			if (prev) prev->next = ptr->next;
			else m->uDNS_info.NATTraversals = ptr->next;
			ufree(n);
			return mDNStrue;
			}
		prev = ptr;
		ptr = ptr->next;
		}
	LogMsg("FreeNATInfo: NATTraversalInfo not found in list");
	return mDNSfalse;
	}

mDNSlocal void SendNATMsg(NATTraversalInfo *info, mDNS *m)
	{
	mStatus err;
	mDNSAddr dst;
	mDNSIPPort dstport;
	uDNS_GlobalInfo *u = &m->uDNS_info;

	if (info->state != NATState_Request && info->state != NATState_Refresh)
		{ LogMsg("SendNATMsg: Bad state %d", info->state); return; }

	if (u->Router.ip.v4.NotAnInteger)
		{
		// send msg	if we have a router
		dst.type = u->Router.type;
		dst.ip.v4 = u->Router.ip.v4;
		dstport = mDNSOpaque16fromIntVal(NATMAP_PORT);
		err = mDNSPlatformSendUDP(m, info->request, info->request+info->requestlen, 0, &dst, dstport);
		if (!err) (info->ntries++);  // don't increment attempt counter if the send failed
		}
	
	// set retry
	if (info->RetryInterval < NATMAP_INIT_RETRY) info->RetryInterval = NATMAP_INIT_RETRY;
	else if (info->RetryInterval * 2 > NATMAP_MAX_RETRY) info->RetryInterval = NATMAP_MAX_RETRY;
	else info->RetryInterval *= 2;
	info->retry = mDNSPlatformTimeNow(m) + info->RetryInterval;

	}


mDNSlocal void ReceiveNATAddrResponse(NATTraversalInfo *n, mDNS *m, mDNSu8 *pkt, mDNSu16 len)
	{
	NATErr_t NatErr = NATErr_None;
	mStatus err = mStatus_NoError;
	AuthRecord *rr = mDNSNULL;
	mDNSAddr addr; 

	if (n->state != NATState_Request)
		{ LogMsg("ReceiveNATAddrResponse: bad state %d", n->state);  return; }
	
	rr = n->reg.RecordRegistration;
	if (!rr)
		{
		LogMsg("ReceiveNATAddrResponse: registration cancelled");
		FreeNATInfo(m, n);
		return;
		}

	addr.type = mDNSAddrType_IPv4;
	addr.ip.v4 = rr->resrec.rdata->u.ipv4;

	if (!pkt) // timeout
		{
#ifdef _LEGACY_NAT_TRAVERSAL_
		err = LNT_GetPublicIP(&addr.ip.v4);
		if (err) goto end;
		else n->state = NATState_Legacy;
#else
		debugf("ReceiveNATAddrResponse: timeout");
		err = mStatus_NATTraversal;
		goto end;
#endif // _LEGACY_NAT_TRAVERSAL_
		}
	else
		{
		if (len < ADDR_REPLY_PKTLEN)
			{
			LogMsg("ReceiveNATAddrResponse: response too short (%d bytes)", len);
			err = mStatus_NATTraversal;
			goto end;
			}
		if (pkt[0] != NATMAP_VERS)
			{
			LogMsg("ReceiveNATAddrResponse: received  version %d (expect version %d)", pkt[0], NATMAP_VERS);
			err = mStatus_NATTraversal;
			goto end;
			}
		if (pkt[1] != (NATOp_AddrRequest | NATMAP_RESPONSE_MASK))
			{
			LogMsg("ReceiveNATAddrResponse: bad response code %d", pkt[1]);
			err = mStatus_NATTraversal;
			goto end;
			}
		NatErr = (NATErr_t)((NATErr_t)pkt[2] << 8 | (NATErr_t)pkt[3]);
		if (NatErr) { LogMsg("ReceiveAddrResponse: received error %d", err); err = mStatus_NATTraversal; goto end; }
		
		addr.ip.v4.b[0] = pkt[4];
		addr.ip.v4.b[1] = pkt[5];
		addr.ip.v4.b[2] = pkt[6];
		addr.ip.v4.b[3] = pkt[7];
		n->state = NATState_Established;
		}
	
	if (IsPrivateV4Addr(&addr))
		{
		LogMsg("ReceiveNATAddrResponse: Double NAT");
		err = mStatus_DblNAT;
		goto end;
		}
	
	end:
	if (err)
		{
		rr->uDNS_info.NATinfo = mDNSNULL;
		FreeNATInfo(m, n);
		rr->uDNS_info.state = regState_Unregistered;    // note that rr is not yet in global list
		rr->RecordCallback(m, rr, mStatus_NATTraversal);
		// note - unsafe to touch rr after callback
		return;
		}
	else LogMsg("Received public IP address %d.%d.%d.%d from NAT.", addr.ip.v4.b[0], addr.ip.v4.b[1], addr.ip.v4.b[2], addr.ip.v4.b[3]);	
	rr->resrec.rdata->u.ipv4 = addr.ip.v4;  // replace rdata w/ public address
	uDNS_RegisterRecord(m, rr);
	}


mDNSlocal void StartGetPublicAddr(mDNS *m, uDNS_HostnameInfo *hInfo)
	{
	mDNSu8 *msg;
	uDNS_GlobalInfo *u = &m->uDNS_info;
	
	NATTraversalInfo *info = AllocNATInfo(m, NATOp_AddrRequest, ReceiveNATAddrResponse);
	if (!info) { uDNS_RegisterRecord(m, hInfo->ar); return; } 
	hInfo->ar->uDNS_info.NATinfo = info;
	info->reg.RecordRegistration = hInfo->ar;
	info->state = NATState_Request;
	
    // format message
	msg = info->request;
	msg[0] = NATMAP_VERS;
	msg[1] = NATOp_AddrRequest;
	info->requestlen = ADDR_REQUEST_PKTLEN;	
	
	if (!u->Router.ip.v4.NotAnInteger)
		{
		debugf("No router.  Will retry NAT traversal in %ld ticks", NATMAP_INIT_RETRY);
		return;
		}
   
	SendNATMsg(info, m);
	}


mDNSlocal void RefreshNATMapping(NATTraversalInfo *n, mDNS *m)
	{
	n->state = NATState_Refresh;
	n->RetryInterval = NATMAP_INIT_RETRY;
	n->ntries = 0;
	SendNATMsg(n, m);
	}

mDNSlocal void ReceivePortMapReply(NATTraversalInfo *n, mDNS *m, mDNSu8 *pkt, mDNSu16 len)
	{
	NATErr_t err;
	ServiceRecordSet *srs;
	mDNSIPPort priv, pub;
	mDNSOpaque32 lease;
	mDNSBool deletion;
	
	if (n->state != NATState_Request && n->state != NATState_Refresh)
		{ LogMsg("ReceivePortMapReply: bad state %d", n->state);  return; }

	deletion = !(n->request[8] |  n->request[9] | n->request[10] | n->request[11]);  // zero lease
	if (deletion) { n->state = NATState_Deleted; return; }
	// note that we explicitly ignore timeouts here
	// we keep the context struct around so that the SendServiceDeregistration code can reference
	// it to determine the public port to delete in the SRV record.
	
	srs = n->reg.ServiceRegistration;
	if (!srs)
		{
		debugf("ReceivePortMapReply: registration cancelled");
		FreeNATInfo(m, n);
		return;
		}
	
	if (!pkt) // timeout
		{
#ifdef _LEGACY_NAT_TRAVERSAL_
		int ntries = 0;
		mStatus err;
		mDNSBool tcp = DomainContainsLabelString(&srs->RR_PTR.resrec.name, "_tcp");

		priv = srs->RR_SRV.resrec.rdata->u.srv.port;  // initially request priv == pub
		pub = srs->RR_SRV.resrec.rdata->u.srv.port;
		while (1)
			{
			err = LNT_MapPort(priv, pub, tcp);
			if (!err)
				{
				n->PublicPort = pub;
				n->state = NATState_Legacy;
				goto end;
				}
			else if (err != mStatus_AlreadyRegistered || ++ntries > LEGACY_NATMAP_MAX_TRIES)
				{
				n->state = NATState_Error;
				goto end;
				}
			else
				{
				// the mapping we want is taken - try a random port
				mDNSu16 RandPort = mDNSRandom(DYN_PORT_MAX - DYN_PORT_MIN) + DYN_PORT_MIN;
				pub = mDNSOpaque16fromIntVal(RandPort);
				}
			}
#else
		goto end;
#endif // _LEGACY_NAT_TRAVERSAL_
		}

	if (len < PORTMAP_PKTLEN) { LogMsg("ReceivePortMapReply: response too short (%d bytes)", len); goto end; }
	if (pkt[0] != NATMAP_VERS) { LogMsg("ReceivePortMapReply: received  version %d (expect version %d)", pkt[0], NATMAP_VERS);  goto end; }
	if (pkt[1] != (n->op | NATMAP_RESPONSE_MASK)) { LogMsg("ReceivePortMapReply: bad response code %d", pkt[1]); goto end; }
	err = (NATErr_t)((NATErr_t)pkt[2] << 8 | (NATErr_t)pkt[3]);
	if (err) { LogMsg("ReceivePortMapReply: received error %d", err);  goto end; }

	priv.b[0] = pkt[4];
	priv.b[1] = pkt[5];
	pub.b[0]  = pkt[6];
	pub.b[1]  = pkt[7];

	lease.b[0] = pkt[8];
	lease.b[1] = pkt[9];
	lease.b[2] = pkt[10];
	lease.b[3] = pkt[11];
		
	if (priv.NotAnInteger !=  srs->RR_SRV.resrec.rdata->u.srv.port.NotAnInteger)
		{ LogMsg("ReceivePortMapReply: reply private port does not match requested private port");  goto end; }
	
	if (n->state == NATState_Refresh && pub.NotAnInteger != n->PublicPort.NotAnInteger) 					
		LogMsg("ReceivePortMapReply: NAT refresh changed public port from %d to %d", mDNSVal16(n->PublicPort), mDNSVal16(pub));
      	// !!!KRS we need to update the SRV here!
	n->PublicPort = pub;

	n->retry = mDNSPlatformTimeNow(m) + ((mDNSs32)mDNSVal32(lease) * mDNSPlatformOneSecond / 2);  // retry half way to expiration
	
	if (n->state == NATState_Refresh) { n->state = NATState_Established; return; }
	n->state = NATState_Established;

	end:
	if (n->state != NATState_Established && n->state != NATState_Legacy)
		{
		LogMsg("NAT Port Mapping: timeout");
		n->state = NATState_Error;
		FreeNATInfo(m, n);
		srs->uDNS_info.NATinfo = mDNSNULL;
		unlinkSRS(&m->uDNS_info, srs);
		srs->uDNS_info.state = regState_Unregistered;
		srs->ServiceCallback(m, srs, mStatus_NATTraversal);
		return;  // note - unsafe to touch srs here
		}

	LogMsg("Mapped private port %d to public port %d", mDNSVal16(priv), mDNSVal16(n->PublicPort));
	srs->uDNS_info.state = regState_FetchingZoneData;
	startGetZoneData(&srs->RR_SRV.resrec.name, m, mDNStrue, mDNSfalse, serviceRegistrationCallback, srs);
	}

mDNSlocal void StartNATPortMap(mDNS *m, ServiceRecordSet *srs)
	{
	mDNSu8 *msg;
	uDNS_GlobalInfo *u = &m->uDNS_info;
	NATOp_t op;
	NATTraversalInfo *info;
	mDNSOpaque32 lease;
	
	if (DomainContainsLabelString(&srs->RR_PTR.resrec.name, "_tcp")) op = NATOp_MapTCP;
	else if (DomainContainsLabelString(&srs->RR_PTR.resrec.name, "_udp")) op = NATOp_MapUDP;
	else { LogMsg("StartNATPortMap: could not determine transport protocol of service %##s", srs->RR_SRV.resrec.name.c); goto error; }

	info = AllocNATInfo(m, op, ReceivePortMapReply);
	srs->uDNS_info.NATinfo = info;
	info->reg.ServiceRegistration = srs;
	info->state = NATState_Request;
	info->requestlen = PORTMAP_PKTLEN;
	
	// format message	
	msg = info->request;	
	msg[0] = NATMAP_VERS;
	msg[1] = op;
	msg[2] = 0; // reserved
	msg[3] = 0; // reserved
	msg[4] = srs->RR_SRV.resrec.rdata->u.srv.port.b[0];  // private port
	msg[5] = srs->RR_SRV.resrec.rdata->u.srv.port.b[1];
	msg[6] = srs->RR_SRV.resrec.rdata->u.srv.port.b[0];  // requested pub. port
	msg[7] = srs->RR_SRV.resrec.rdata->u.srv.port.b[1];
	
	lease   = mDNSOpaque32fromIntVal(NATMAP_DEFAULT_LEASE);
	msg[8]  = lease.b[0];
	msg[9]  = lease.b[1];
	msg[10] = lease.b[2];
	msg[11] = lease.b[3];
	
	if (!u->Router.ip.v4.NotAnInteger)
		{
		debugf("No router.  Will retry NAT traversal in %ld seconds", NATMAP_INIT_RETRY);
		info->retry = mDNSPlatformTimeNow(m) + NATMAP_INIT_RETRY;
		info->RetryInterval = NATMAP_INIT_RETRY;
		return;
		}

    SendNATMsg(info, m);
	return;
	
	error:
	startGetZoneData(&srs->RR_SRV.resrec.name, m, mDNStrue, mDNSfalse, serviceRegistrationCallback, srs);
	}

 // ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - host name and interface management
#endif


// find the target in our list of hostnames for an SRV record
// e.g. for service example._xxx._tcp.example.foo.com, return target mycomputer.example.foo.com.
mDNSlocal const domainname *GetServiceTarget(uDNS_GlobalInfo *u, AuthRecord *srv)
	{
	int i, nzonelabels, nsrvlabels = 0;
	mDNSu8 *ptr;
	uDNS_HostnameInfo *hi;
	domainname *target, *targetzone;
	
	if (srv->resrec.rrtype != kDNSType_SRV) { LogMsg("GetServiceTarget: bad rrtype %d", srv->resrec.rrtype); return mDNSNULL; }

	// count labels in service name
	for (ptr = srv->resrec.name.c; *ptr; ptr = ptr + ptr[0] + 1) nsrvlabels++;	
	
	for (hi = u->Hostnames; hi; hi = hi->next)
		{		
		target = &hi->ar->resrec.name;
		targetzone = (domainname *)(target->c + 1 + target->c[0]); // chop hostlabel

		// count labels in target zone
		nzonelabels = 0; 
		for (ptr = targetzone->c; *ptr; ptr = ptr + ptr[0] + 1) nzonelabels++;

		if (nsrvlabels >= nzonelabels)
			{
			// locate zone in srv
			ptr = srv->resrec.name.c;
			for (i = 0; i < nsrvlabels - nzonelabels; i++) ptr = ptr + ptr[0] + 1;
			if (SameDomainName((domainname *)ptr, targetzone)) return target;
			}
		}
	return mDNSNULL;			
	}

mDNSlocal void HostnameCallback(mDNS *const m, AuthRecord *const rr, mStatus result)
	{
	void *tmpContext;
	uDNS_HostnameInfo *hi = (uDNS_HostnameInfo *)rr->RecordContext;
	mDNSu8 *ip = rr->resrec.rdata->u.ipv4.b;
	uDNS_GlobalInfo *u = &m->uDNS_info;
	ServiceRecordSet *srs;
	
	if (result == mStatus_MemFree)
		{
		debugf("MemFree:  %##s IP %d.%d.%d.%d", rr->resrec.name.c, ip[0], ip[1], ip[2], ip[3]);
		if (hi) ufree(hi);
		ufree(rr);        
		return;
		}
	
	if (result == mStatus_NameConflict && rr->resrec.RecordType == kDNSRecordTypeUnique)
		{
		// if we get a name conflict, make sure our name/addr isn't already registered by re-registering
		debugf("Name in use - retrying as type KnownUnique");
		rr->resrec.RecordType = kDNSRecordTypeKnownUnique;
		uDNS_RegisterRecord(m, rr);
		return;
		}
	
	if (rr->resrec.RecordType == kDNSRecordTypeKnownUnique)
		{
		// we've already tried to re-register.  reset RecordType before returning RR to client
		if (result == mStatus_NoSuchRecord)  // name is advertised for some other address
			result = mStatus_NameConflict;
		}
	
	if (result)
		{
		// unlink name, deliver error, free memory
		uDNS_HostnameInfo *ptr, *prev = mDNSNULL;
		LogMsg("HostnameCallback: Error %ld for registration of %##s IP %d.%d.%d.%d", result, rr->resrec.name.c, ip[0], ip[1], ip[2], ip[3]);
		if (!hi) { ufree(rr); return; }
		for (ptr = u->Hostnames; ptr; ptr = ptr->next)
			{
			if (ptr == hi)
				{
				if (prev) prev->next = ptr->next;             // unlink from list
				else u->Hostnames = ptr->next;						
				(const void *)rr->RecordContext = hi->StatusContext;
				hi->StatusCallback(m, rr, result);
				ufree(hi->ar);
				ufree(hi);
				return;
				}
			}
		LogMsg("hostname %##s no longer in list", rr->resrec.name.c);
		return;
		}

	// register any pending services that require a target
	for (srs = u->ServiceRegistrations; srs; srs = srs->next)		
		{
		if (srs->uDNS_info.state == regState_NoTarget && GetServiceTarget(u, &srs->RR_SRV))
			RegisterService(m, srs);
		}
	
	// Deliver success to client
	if (!hi) { LogMsg("HostnameCallback invoked with orphaned address record"); return; }
	LogMsg("Registered hostname %##s IP %d.%d.%d.%d", rr->resrec.name.c, ip[0], ip[1], ip[2], ip[3]);	
	tmpContext = rr->RecordContext;
	(const void *)rr->RecordContext = hi->StatusContext;
	hi->StatusCallback(m, rr, result);
	rr->RecordContext = tmpContext;
	}

// register record or begin NAT traversal
mDNSlocal void AdvertiseHostname(mDNS *m, uDNS_HostnameInfo *h)
	{
	if (IsPrivateV4Addr(&m->uDNS_info.PrimaryIP)) 
		StartGetPublicAddr(m, h);
	else 
	  {
	  mDNSu8 *ip = m->uDNS_info.PrimaryIP.ip.v4.b;
	  LogMsg("Advertising %##s IP %d.%d.%d.%d", h->ar->resrec.name.c, ip[0], ip[1], ip[2], ip[3]);
	  uDNS_RegisterRecord(m, h->ar); 
	  }
	}

// Deregister hostnames and  register new names for each host domain with the current global 
// values for the hostlabel and primary IP address
mDNSlocal void UpdateHostnameRegistrations(mDNS *m)
	{
	uDNS_GlobalInfo *u = &m->uDNS_info;	
	AuthRecord *new;
	uDNS_HostnameInfo *i;

	for (i = u->Hostnames; i; i = i->next)
		{
		// only allocate new record if existing record is actually registered (i.e. it wasn't pending the hostname or IP being set)
		if (i->ar->uDNS_info.state == regState_Unregistered) new = i->ar;
		else
			{
			new = umalloc(sizeof(AuthRecord));
			if (!new) { LogMsg("ERROR: UpdateHostnameRegistration - malloc"); return; }
			mDNS_SetupResourceRecord(new, mDNSNULL, 0, kDNSType_A,  1, kDNSRecordTypeUnique, HostnameCallback, i);
			}

		// setup new record
		if (SameDomainLabel(u->hostlabel.c, i->ar->resrec.name.c))
			AssignDomainName(new->resrec.name, i->ar->resrec.name);
		else
			{
			// hostlabel changed
			domainname *HostDomain = (domainname *)(i->ar->resrec.name.c + 1 + i->ar->resrec.name.c[0]);
			AppendDomainLabel(&new->resrec.name, &u->hostlabel);
			AppendDomainName(&new->resrec.name, HostDomain);
			}
		new->resrec.rdata->u.ipv4 = u->PrimaryIP.ip.v4;

		if (i->ar->uDNS_info.state != regState_Unregistered)
			{			
			// delete old record
			i->ar->RecordContext = mDNSNULL;  // clear backpointer to HostnameInfo
			uDNS_DeregisterRecord(m, i->ar);  
			i->ar = new;
			}
		
		// advertise new
		AdvertiseHostname(m, i);
		}
	}

mDNSexport void mDNS_AddDynDNSHostDomain(mDNS *m, const domainname *newdomain, mDNSRecordCallback *StatusCallback, const void *StatusContext)
	{
	uDNS_GlobalInfo *u = &m->uDNS_info;
	uDNS_HostnameInfo *ptr, *new;
	domainname *d;

	mDNS_Lock(m);

	// check if domain already registered
	for (ptr = u->Hostnames; ptr; ptr = ptr->next)
		{
		d = (domainname *)(ptr->ar->resrec.name.c + 1 + ptr->ar->resrec.name.c[0]);  // get the name of the registered hostname
		if (SameDomainName(newdomain, d))
			{ LogMsg("Host Domain %##s already in list", d->c); goto exit; }
		}

	// allocate and format new address record
	new = umalloc(sizeof(*new));
	if (new) new->ar = umalloc(sizeof(AuthRecord));
	if (!new || !new->ar) { LogMsg("ERROR: mDNS_AddDynDNSHostname - malloc"); goto exit; }
	new->StatusCallback = StatusCallback;
	new->StatusContext = StatusContext;
	mDNS_SetupResourceRecord(new->ar, mDNSNULL, 0, kDNSType_A,  1, kDNSRecordTypeUnique, HostnameCallback, new);	
	AppendDomainLabel(&new->ar->resrec.name, &u->hostlabel);
	AppendDomainName(&new->ar->resrec.name, newdomain);
	new->next = u->Hostnames;
	u->Hostnames = new;
	if (u->PrimaryIP.ip.v4.NotAnInteger)
		{
		// only set RData if we have a valid IP
		if (u->MappedPrimaryIP.ip.v4.NotAnInteger) new->ar->resrec.rdata->u.ipv4 = u->MappedPrimaryIP.ip.v4;  //!!!KRS implement code that caches this
		else                                       new->ar->resrec.rdata->u.ipv4 = u->PrimaryIP.ip.v4;
		AdvertiseHostname(m, new);
		}
	else new->ar->uDNS_info.state = regState_Unregistered;
exit:
	mDNS_Unlock(m);
	}

mDNSexport void mDNS_RemoveDynDNSHostDomain(mDNS *m, const domainname *domain)
	{
	uDNS_GlobalInfo *u = &m->uDNS_info;
	uDNS_HostnameInfo *ptr, *prev = mDNSNULL;
	domainname *d;

	mDNS_Lock(m);

	for (ptr = u->Hostnames; ptr; ptr = ptr->next)
		{
		d = (domainname *)(ptr->ar->resrec.name.c + 1 + ptr->ar->resrec.name.c[0]);  // get the name of the registered hostname
		if (SameDomainName(domain, d))
			{
			if (prev) prev->next = ptr->next;             // unlink from list
			else u->Hostnames = ptr->next;
			uDNS_DeregisterRecord(m, ptr->ar);           // deregister the record
			goto exit;
			}
		prev = ptr;			
		}
	LogMsg("mDNS_RemoveDynDNSHostDomain: no such domain %##s", domain->c);
exit:
	mDNS_Unlock(m);
	}

mDNSexport void mDNS_SetPrimaryIP(mDNS *m, const mDNSAddr *ip)
	{
	uDNS_GlobalInfo *u = &m->uDNS_info;

	if (!ip) { LogMsg("mDNS_SetPrimaryIP passed NULL argument"); return; }

	mDNS_Lock(m);
	
	if (ip->ip.v4.NotAnInteger == u->PrimaryIP.ip.v4.NotAnInteger)
		debugf("mDNS_SetPrimaryIP: IP address unchanged");
	else if (ip->type !=mDNSAddrType_IPv4)
		debugf("Unsupported (non IPv4) address type passed to mDNS_SetPrimaryIP - ignoring");
	else
		{
		u->PrimaryIP = *ip;
		UpdateHostnameRegistrations(m);
		}

	mDNS_Unlock(m);
	}

mDNSexport void mDNS_SetDynDNSComputerName(mDNS *m, const domainlabel *hostlabel)
	{
	uDNS_GlobalInfo *u = &m->uDNS_info;

	mDNS_Lock(m);

	if (!u->hostlabel.c[0]) { u->hostlabel = *hostlabel; goto exit; }  // initialization
	if (SameDomainLabel(u->hostlabel.c, hostlabel->c)) { debugf("mDNS_SetDynDNSComputerName: hostlabel unchanged"); goto exit; }
	u->hostlabel = *hostlabel;
	UpdateHostnameRegistrations(m);	
	UpdateServiceTargets(m);   //!!!KRS this gets done for us for empty-string registrations - how do we tell when we need to do it?

exit:
	mDNS_Unlock(m);
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Incoming Message Processing
#endif

mDNSlocal mDNSBool kaListContainsAnswer(DNSQuestion *question, CacheRecord *rr)
	{
	CacheRecord *ptr;

	for (ptr = question->uDNS_info.knownAnswers; ptr; ptr = ptr->next)
		if (SameResourceRecord(&ptr->resrec, &rr->resrec)) return mDNStrue;

	return mDNSfalse;
	}


mDNSlocal void removeKnownAnswer(DNSQuestion *question, CacheRecord *rr)
	{
	CacheRecord *ptr, *prev = mDNSNULL;

	for (ptr = question->uDNS_info.knownAnswers; ptr; ptr = ptr->next)
		{
		if (SameResourceRecord(&ptr->resrec, &rr->resrec))
			{
			if (prev) prev->next = ptr->next;
			else question->uDNS_info.knownAnswers = ptr->next;
			ufree(ptr);
			return;
			}
		prev = ptr;
		}
	LogMsg("removeKnownAnswer() called for record not in KA list");
	}


mDNSlocal void addKnownAnswer(DNSQuestion *question, const CacheRecord *rr)
	{
	CacheRecord *newCR = mDNSNULL;
	mDNSu32 size;	

	size = sizeof(CacheRecord) + rr->resrec.rdlength - InlineCacheRDSize;
	newCR = (CacheRecord *)umalloc(size);
	if (!newCR) { LogMsg("ERROR: addKnownAnswer - malloc"); return; }
	umemcpy(newCR, rr, size);
	newCR->resrec.rdata = (RData*)&newCR->rdatastorage;
	newCR->resrec.rdata->MaxRDLength = rr->resrec.rdlength;
	newCR->next = question->uDNS_info.knownAnswers;
	question->uDNS_info.knownAnswers = newCR;
	}

mDNSlocal void deriveGoodbyes(mDNS * const m, DNSMessage *msg, const  mDNSu8 *end, DNSQuestion *question)
	{
	const mDNSu8 *ptr;
	int i;
	CacheRecord *fptr, *ka, *cr, *answers = mDNSNULL, *prev = mDNSNULL;
	LargeCacheRecord *lcr;
	
	if (question != m->uDNS_info.CurrentQuery) { LogMsg("ERROR: deriveGoodbyes called without CurrentQuery set!"); return; }

	ptr = LocateAnswers(msg, end);
	if (!ptr) goto pkt_error;

	if (!msg->h.numAnswers)
		{
		// delete the whole KA list
		ka = question->uDNS_info.knownAnswers;
		while (ka)
			{
			debugf("deriving goodbye for %##s", ka->resrec.name.c);
			
			m->mDNS_reentrancy++; // Increment to allow client to legally make mDNS API calls from the callback
			question->QuestionCallback(m, question, &ka->resrec, mDNSfalse);
			m->mDNS_reentrancy--; // Decrement to block mDNS API calls again
			if (question != m->uDNS_info.CurrentQuery)
				{
				debugf("deriveGoodbyes - question removed via callback.  returning.");
				return;
				}
			fptr = ka;
			ka = ka->next;
			ufree(fptr);
			}
		question->uDNS_info.knownAnswers = mDNSNULL;
		return;
		}
	
	// make a list of all the new answers
	for (i = 0; i < msg->h.numAnswers; i++)
		{
		lcr = (LargeCacheRecord *)umalloc(sizeof(LargeCacheRecord));
		if (!lcr) goto malloc_error;
		ubzero(lcr, sizeof(LargeCacheRecord));
		ptr = GetLargeResourceRecord(m, msg, ptr, end, 0, kDNSRecordTypePacketAns, lcr);
		if (!ptr) goto pkt_error;
		cr = &lcr->r;
		if (ResourceRecordAnswersQuestion(&cr->resrec, question)) 
			{
			cr->next = answers;
			answers = cr;
			}
		else ufree(cr);
		}
	
	// make sure every known answer is in the answer list
	ka = question->uDNS_info.knownAnswers;
	while (ka)
		{
		for (cr = answers; cr; cr = cr->next)
			{ if (SameResourceRecord(&ka->resrec, &cr->resrec)) break; }
		if (!cr)
			{
			// record is in KA list but not answer list - remove from KA list
			if (prev) prev->next = ka->next;
			else question->uDNS_info.knownAnswers = ka->next;
			debugf("deriving goodbye for %##s", ka->resrec.name.c);
			m->mDNS_reentrancy++; // Increment to allow client to legally make mDNS API calls from the callback
			question->QuestionCallback(m, question, &ka->resrec, mDNSfalse);
			m->mDNS_reentrancy--; // Decrement to block mDNS API calls again
			if (question != m->uDNS_info.CurrentQuery)
				{
				debugf("deriveGoodbyes - question removed via callback.  returning.");
				return;
				}
			fptr = ka;
			ka = ka->next;
			ufree(fptr);
			}
		else
			{
			prev = ka;
			ka = ka->next;
			}
		}

	// free temp answers list
	cr = answers;
	while (cr) { fptr = cr; cr = cr->next; ufree(fptr); }

	return;
	
	pkt_error:
	LogMsg("ERROR: deriveGoodbyes - received malformed response to query for %##s (%d)",
		   question->qname.c, question->qtype);
	return;

	malloc_error:
	LogMsg("ERROR: Malloc");	
	}

mDNSlocal void pktResponseHndlr(mDNS * const m, DNSMessage *msg, const  mDNSu8 *end, DNSQuestion *question, mDNSBool llq)
	{
	const mDNSu8 *ptr;
	int i;
	LargeCacheRecord lcr;
	CacheRecord *cr = &lcr.r;
	mDNSBool goodbye, inKAList;
	LLQ_Info *llqInfo = question->uDNS_info.llq;
	
	if (question != m->uDNS_info.CurrentQuery)
		{ LogMsg("ERROR: pktResponseHdnlr called without CurrentQuery ptr set!");  return; }
		
	ptr = LocateAnswers(msg, end);
	if (!ptr) goto pkt_error;

	for (i = 0; i < msg->h.numAnswers; i++)
		{
		ptr = GetLargeResourceRecord(m, msg, ptr, end, 0, kDNSRecordTypePacketAns, &lcr);
		if (!ptr) goto pkt_error;
		if (ResourceRecordAnswersQuestion(&cr->resrec, question))
			{
			goodbye = llq ? ((mDNSs32)cr->resrec.rroriginalttl == -1) : mDNSfalse;
			inKAList = kaListContainsAnswer(question, cr);

			if ((goodbye && !inKAList) || (!goodbye && inKAList)) continue;  // list up to date
			if (!inKAList) addKnownAnswer(question, cr);
			if (goodbye) removeKnownAnswer(question, cr);
			m->mDNS_reentrancy++; // Increment to allow client to legally make mDNS API calls from the callback
			question->QuestionCallback(m, question, &cr->resrec, !goodbye);
			m->mDNS_reentrancy--; // Decrement to block mDNS API calls again
			if (question != m->uDNS_info.CurrentQuery)
				{
				debugf("pktResponseHndlr - CurrentQuery changed by QuestionCallback - returning");
				return;
				}
			}			
		else
			{
			LogMsg("unexpected answer: %##s", cr->resrec.name.c);
			}
		}
	if (llq && (llqInfo->state == LLQ_Poll || llqInfo->deriveRemovesOnResume))	
		{ deriveGoodbyes(m, msg, end,question);  llqInfo->deriveRemovesOnResume = mDNSfalse; }

	// our interval may be set lower to recover from failures - now that we have an answer, fully back off retry
	if (question->ThisQInterval < MAX_UCAST_POLL_INTERVAL) question->ThisQInterval = MAX_UCAST_POLL_INTERVAL;
	return;

	pkt_error:
	LogMsg("ERROR: pktResponseHndlr - received malformed response to query for %##s (%d)",
		   question->qname.c, question->qtype);
	return;
	}

mDNSlocal void simpleResponseHndlr(mDNS * const m, DNSMessage *msg, const  mDNSu8 *end, DNSQuestion *question, void *context)
	{
	(void)context; // unused
	pktResponseHndlr(m, msg, end, question, mDNSfalse);
	}

mDNSlocal void llqResponseHndlr(mDNS * const m, DNSMessage *msg, const  mDNSu8 *end, DNSQuestion *question, void *context)
	{
	(void)context; // unused
	pktResponseHndlr(m, msg, end, question, mDNStrue);
	}

mDNSlocal mStatus checkUpdateResult(domainname *name, mDNSu8 rcode, const DNSMessage *msg)
	{	
	(void)msg;  // currently unused, needed for TSIG errors
	if (!rcode) return mStatus_NoError;
	else if (rcode == kDNSFlag1_RC_YXDomain)
		{
		LogMsg("Name in use: %##s", name->c);
		return mStatus_NameConflict;
		}
	else if (rcode == kDNSFlag1_RC_Refused)
		{
		LogMsg("Update %##s refused", name->c);
		return mStatus_Refused;
		}
	else if (rcode == kDNSFlag1_RC_NXRRSet)
		{
		LogMsg("Reregister refusted (NXRRSET): %##s", name->c);
		return mStatus_NoSuchRecord;
		}
	else if (rcode == kDNSFlag1_RC_NotAuth)
		{
		LogMsg("Permission denied (NOAUTH): %##s", name->c);
		return mStatus_NoAuth;
		}
	else if (rcode == kDNSFlag1_RC_FmtErr)
		{
		LogMsg("Format Error: %##s", name->c);
		return mStatus_UnknownErr;
		//!!!KRS need to parse message for TSIG errors
		}
	else
		{
		LogMsg("Update %##s failed with rcode %d", name->c, rcode);
		return mStatus_UnknownErr;
		}
	}

mDNSlocal void hndlServiceUpdateReply(mDNS * const m, ServiceRecordSet *srs,  mStatus err)
	{	
	mDNSBool InvokeCallback = mDNSfalse;	
	AuthRecord *UpdateR = mDNSNULL;
	uDNS_RegInfo *info = &srs->uDNS_info;
	NATTraversalInfo *nat = srs->uDNS_info.NATinfo;

	switch (info->state)
		{
		case regState_Pending:
			if (err == mStatus_NameConflict && !info->TestForSelfConflict)
				{
				info->TestForSelfConflict = mDNStrue;
				debugf("checking for self-conflict of service %##s", srs->RR_SRV.resrec.name.c);
				SendServiceRegistration(m, srs);
				return;
				}
			else if (info->TestForSelfConflict)
				{
				info->TestForSelfConflict = mDNSfalse;
				if (err == mStatus_NoSuchRecord) err = mStatus_NameConflict;  // NoSuchRecord implies that our prereq was not met, so we actually have a name conflict
				if (err) info->state = regState_Unregistered;
				else info->state = regState_Registered;				
				InvokeCallback = mDNStrue;
				break;
				}
			else if (err == mStatus_UnknownErr && info->lease)
				{
				LogMsg("Re-trying update of service %##s without lease option", srs->RR_SRV.resrec.name.c);
				info->lease = mDNSfalse;
				info->expire = -1;
				SendServiceRegistration(m, srs);
				return;
				}
			else
				{
				if (err) { LogMsg("Error %ld for registration of service %##s", err, srs->RR_SRV.resrec.name.c); info->state = regState_Unregistered; } //!!!KRS make sure all structs will still get cleaned up when client calls DeregisterService with this state
				else info->state = regState_Registered;
				InvokeCallback = mDNStrue;
				break;
				}
		case regState_Refresh:
			if (err)
				{
				LogMsg("Error %ld for refresh of service %##s", err, srs->RR_SRV.resrec.name.c);
				InvokeCallback = mDNStrue;
				info->state = regState_Unregistered;				
				}
			else info->state = regState_Registered;				
			info->OrigTarget.c[0] = '\0';  // if we were updating the target, clear the original target
			break;
		case regState_DeregPending:
			if (err) LogMsg("Error %ld for refresh of service %##s", err, srs->RR_SRV.resrec.name.c);
			err = mStatus_MemFree;
			InvokeCallback = mDNStrue;
			if (nat)
				{
				if (nat->state == NATState_Deleted) { FreeNATInfo(m, nat); info->NATinfo = mDNSNULL; } // deletion copmleted
				else nat->reg.ServiceRegistration = mDNSNULL;  // allow mapping deletion to continue
				}
			unlinkSRS(&m->uDNS_info, srs);
			info->state = regState_Unregistered;
			break;
		case regState_DeregDeferred:
			if (err) { debugf("Error %ld received prior to deferred derigstration of %##s", err, srs->RR_SRV.resrec.name.c); }
			debugf("Performing deferred deregistration of %##s", srs->RR_SRV.resrec.name.c); 
			info->state = regState_Registered; // benign to set even if we've got an error
			SendServiceRegistration(m, srs);
			return;		   
		case regState_UpdatePending:
			// find the record being updated
			if (srs->RR_TXT.uDNS_info.state == regState_UpdatePending) UpdateR = &srs->RR_TXT;
			else
				{
				ExtraResourceRecord *e;				
				for (e = srs->Extras; e; e = e->next)
					if (e->r.uDNS_info.state == regState_UpdatePending) { UpdateR = &e->r; break; }
				if (!e) LogMsg("ServiceRecordSet for %##s in regState_UpdatePending with no corresponding records in UpdatePending state", srs->RR_SRV.resrec.name.c);
				}
			if (err)
				{
				LogMsg("hndlServiceUpdateReply: error updating resource record");				
				if (UpdateR) UpdateR->uDNS_info.state = regState_Unregistered;
				InvokeCallback = mDNStrue;        // signal error via service callback
				}
			else if (UpdateR) 
				{
				UpdateR->uDNS_info.state = regState_Registered;
				SwapRData(m, UpdateR, mDNStrue);				
				}
			info->state = regState_Registered;			
			break;			
		default:
			LogMsg("hndlServiceUpdateReply called for service %##s in unexpected state %d with error %ld.  Unlinking.",
				   srs->RR_SRV.resrec.name.c, info->state, err);
			err = mStatus_UnknownErr;
		}

	if (info->state == regState_Registered) 
		{
		// send a queued update, if any
		ExtraResourceRecord *e;

		srs->RR_SRV.ThisAPInterval = INIT_UCAST_POLL_INTERVAL - 1;  // reset retry delay for future refreshes, dereg, etc.
		
		if (srs->RR_TXT.uDNS_info.UpdateQueued) SendRecordUpdate(m, &srs->RR_TXT, &srs->uDNS_info);
		else
			{
			for (e = srs->Extras; e; e = e->next)
				if (e->r.uDNS_info.UpdateQueued) { SendRecordUpdate(m, &e->r, &srs->uDNS_info); break; } 
			}
				
		if (info->TargetChangeDeferred)
			{
			info->ClientCallbackDeferred = InvokeCallback;
			info->DeferredStatus = err;
			info->state = regState_Refresh;  // this updates the target	   
			SendServiceRegistration(m, srs);
			return;
			}				
		}

	m->mDNS_reentrancy++; // Increment to allow client to legally make mDNS API calls from the callback
	if (InvokeCallback) srs->ServiceCallback(m, srs, err);
	else if (info->ClientCallbackDeferred)
		{
		info->ClientCallbackDeferred = mDNSfalse;
		srs->ServiceCallback(m, srs, info->DeferredStatus);
		}
	m->mDNS_reentrancy--; // Decrement to block mDNS API calls again
	// NOTE: do not touch structures after calling ServiceCallback
	}

mDNSlocal void hndlRecordUpdateReply(mDNS *m, AuthRecord *rr, mStatus err)
	{
	uDNS_GlobalInfo *u = &m->uDNS_info;
	
	if (rr->uDNS_info.state == regState_DeregPending)
		{					
		debugf("Received reply for deregister record %##s type %d", rr->resrec.name.c, rr->resrec.rrtype);
		if (err) LogMsg("ERROR: Deregistration of record %##s type %d failed with error %ld",
						rr->resrec.name.c, rr->resrec.rrtype, err);
		else err = mStatus_MemFree;
		if (unlinkAR(&m->uDNS_info.RecordRegistrations, rr))
			LogMsg("ERROR: Could not unlink resource record following deregistration");
		rr->uDNS_info.state = regState_Unregistered;
		m->mDNS_reentrancy++; // Increment to allow client to legally make mDNS API calls from the callback
		rr->RecordCallback(m, rr, err);
		m->mDNS_reentrancy--; // Decrement to block mDNS API calls again
		return;
		}

	if (rr->uDNS_info.state == regState_DeregDeferred)
		{
		if (err)
			{
			LogMsg("Cancelling deferred deregistration record %##s type %d due to registration error %ld",
				   rr->resrec.name.c, rr->resrec.rrtype, err);
			unlinkAR(&m->uDNS_info.RecordRegistrations, rr);
			rr->uDNS_info.state = regState_Unregistered;
			return;
			}
		LogMsg("Calling deferred deregistration of record %##s type %d",
			   rr->resrec.name.c, rr->resrec.rrtype);
		rr->uDNS_info.state = regState_Registered;
		uDNS_DeregisterRecord(m, rr);					
		return;
		}

	if (rr->uDNS_info.state == regState_Pending || rr->uDNS_info.state == regState_Refresh)
		{
		if (err)
			{
			if (rr->uDNS_info.lease && err == mStatus_UnknownErr)
				{
				LogMsg("Re-trying update of record %##s without lease option", rr->resrec.name.c);
				rr->uDNS_info.lease = mDNSfalse;
				rr->uDNS_info.expire = -1;
				sendRecordRegistration(m, rr);
				return;
				}
			
			LogMsg("Registration of record %##s type %d failed with error %ld",
				   rr->resrec.name.c, rr->resrec.rrtype, err);
			unlinkAR(&u->RecordRegistrations, rr); 
			rr->uDNS_info.state = regState_Unregistered;
			m->mDNS_reentrancy++; // Increment to allow client to legally make mDNS API calls from the callback
			rr->RecordCallback(m, rr, err);
			m->mDNS_reentrancy--; // Decrement to block mDNS API calls again
			return;
			}
		else
			{
			if (rr->uDNS_info.state == regState_Refresh)
				rr->uDNS_info.state = regState_Registered;			
			else
				{
				rr->uDNS_info.state = regState_Registered;
				m->mDNS_reentrancy++; // Increment to allow client to legally make mDNS API calls from the callback
				rr->RecordCallback(m, rr, err);
				m->mDNS_reentrancy--; // Decrement to block mDNS API calls again
				}
			return;
			}
		}
	
	LogMsg("Received unexpected response for record %##s type %d, in state %d, with response error %ld",
		   rr->resrec.name.c, rr->resrec.rrtype, rr->uDNS_info.state, err);
	}


mDNSlocal void SetUpdateExpiration(mDNS *m, DNSMessage *msg, const mDNSu8 *end, uDNS_RegInfo *info)
	{
	LargeCacheRecord lcr;
	const mDNSu8 *ptr;
	int i;
	mDNSu32 lease = 0;
	mDNSs32 expire;
	
	ptr = LocateAdditionals(msg, end);

	if (info->lease && (ptr = LocateAdditionals(msg, end)))
		{
		for (i = 0; i < msg->h.numAdditionals; i++)
			{
			ptr = GetLargeResourceRecord(m, msg, ptr, end, 0, kDNSRecordTypePacketAdd, &lcr);
			if (!ptr) break;
			if (lcr.r.resrec.rrtype == kDNSType_OPT)
				{
				if (lcr.r.resrec.rdlength < LEASE_OPT_SIZE) continue;
				if (lcr.r.resrec.rdata->u.opt.opt != kDNSOpt_Lease) continue;
				lease = lcr.r.resrec.rdata->u.opt.OptData.lease;
				break;
				}
			}
		}
	
	if (lease > 0)
		{
		expire = (mDNSPlatformTimeNow(m) + (((mDNSs32)lease * mDNSPlatformOneSecond)) * 3/4);
		if (info->state == regState_UpdatePending)  
            // if updating individual record, the service record set may expire sooner
			{ if (expire - info->expire < 0) info->expire = expire; }
		else info->expire = expire;
		}
	else info->expire = -1;
	}

mDNSexport void uDNS_ReceiveNATMap(mDNS *m, mDNSu8 *pkt, mDNSu16 len)
	{
	uDNS_GlobalInfo *u = &m->uDNS_info;
	NATTraversalInfo *ptr, *cur;
	NATOp_t op;
	
	// check length, version, opcode
	if (len < ADDR_REPLY_PKTLEN && len < PORTMAP_PKTLEN) { LogMsg("NAT Traversal message too short (%d bytes)", len); return; }
	if (pkt[0] != NATMAP_VERS) { LogMsg("Received NAT Traversal response with version %d (expect version %d)", pkt[0], NATMAP_VERS); return; }
	op = pkt[1];
	if (!(op & NATMAP_RESPONSE_MASK)) { LogMsg("Received NAT Traversal message that is not a response (opcode %d)", op); return; }

	ptr = u->NATTraversals;
	while (ptr)
		{
		cur = ptr;
		ptr = ptr->next;
		if ((cur->state == NATState_Request || cur->state == NATState_Refresh) &&
			(cur->op | NATMAP_RESPONSE_MASK) == op)
			cur->ReceiveResponse(cur, m, pkt, len);  // callback may delete "cur"
     		// specific request/reply matching logic handled by callback - we don't know if this was a match, so we don't break here  
		}
	}

mDNSexport void uDNS_ReceiveMsg(mDNS *const m, DNSMessage *const msg, const mDNSu8 *const end,
	const mDNSAddr *const srcaddr, const mDNSIPPort srcport, const mDNSAddr *const dstaddr,
	const mDNSIPPort dstport, const mDNSInterfaceID InterfaceID, mDNSu8 ttl)
	{
	DNSQuestion *qptr;
	AuthRecord *rptr;
	ServiceRecordSet *sptr;
	mStatus err = mStatus_NoError;
	uDNS_GlobalInfo *u = &m->uDNS_info;
	
	mDNSu8 StdR    = kDNSFlag0_QR_Response | kDNSFlag0_OP_StdQuery;
	mDNSu8 UpdateR = kDNSFlag0_QR_Response | kDNSFlag0_OP_Update;
	mDNSu8 QR_OP   = (mDNSu8)(msg->h.flags.b[0] & kDNSFlag0_QROP_Mask);
	mDNSu8 rcode   = (mDNSu8)(msg->h.flags.b[1] & kDNSFlag1_RC);
   	
    // unused
	(void)srcaddr;
	(void)srcport;
	(void)dstaddr;
	(void)dstport;
	(void)ttl;
	(void)InterfaceID;
	
	if (QR_OP == StdR)
		{		
		// !!!KRS we should to a table lookup here to see if it answers an LLQ or a 1-shot
		if (recvLLQResponse(m, msg, end, srcaddr, srcport, InterfaceID)) return;
	
		for (qptr = u->ActiveQueries; qptr; qptr = qptr->next)
			{
			//!!!KRS we should have a hashtable, hashed on message id
			if (qptr->uDNS_info.id.NotAnInteger == msg->h.id.NotAnInteger)
				{
				if (msg->h.flags.b[0] & kDNSFlag0_TC)
					{ hndlTruncatedAnswer(qptr, srcaddr, m); return; }
				else
					{
					u->CurrentQuery = qptr;
					qptr->uDNS_info.responseCallback(m, msg, end, qptr, qptr->uDNS_info.context);
					u->CurrentQuery = mDNSNULL;
					// Note: responseCallback can invalidate qptr
					return;
					}
				}
			}
		}
	if (QR_OP == UpdateR)
		{
		for (sptr = u->ServiceRegistrations; sptr; sptr = sptr->next)
			{
			if (sptr->uDNS_info.id.NotAnInteger == msg->h.id.NotAnInteger)
				{
				err = checkUpdateResult(&sptr->RR_SRV.resrec.name, rcode, msg);
				if (!err) SetUpdateExpiration(m, msg, end, &sptr->uDNS_info);
				hndlServiceUpdateReply(m, sptr, err);
				return;
				}
			}
		for (rptr = u->RecordRegistrations; rptr; rptr = rptr->next)
			{
			if (rptr->uDNS_info.id.NotAnInteger == msg->h.id.NotAnInteger)
				{
				err = checkUpdateResult(&rptr->resrec.name, rcode, msg);
				if (!err) SetUpdateExpiration(m, msg, end, &rptr->uDNS_info);				
				hndlRecordUpdateReply(m, rptr, err);
				return;
				}
			}
		}
	debugf("Received unexpected response: ID %d matches no active records", mDNSVal16(msg->h.id));
	}

		
mDNSlocal void receiveMsg(mDNS *const m, DNSMessage *const msg, const mDNSu8 *const end,
	const mDNSInterfaceID InterfaceID)
	{
	mDNSAddr *sa = mDNSNULL, *da = mDNSNULL;
	mDNSIPPort sp, dp;
	mDNSu8 ttl = 0;

	sp.NotAnInteger = 0;
	dp.NotAnInteger = 0;
	mDNS_Lock(m);
	uDNS_ReceiveMsg(m, msg, end, sa, sp, da, dp, InterfaceID, ttl);
	mDNS_Unlock(m);
	}

//!!!KRS this should go away (don't just pick one randomly!)
mDNSlocal const mDNSAddr *getInitializedDNS(uDNS_GlobalInfo *u)
    {
    int i;
    for (i = 0; i < 32; i++)
		if (u->Servers[i].ip.v4.NotAnInteger) return &u->Servers[i];

	return mDNSNULL;
    }

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Query Routines
#endif

#define sameID(x,y) mDNSPlatformMemSame(x,y,8)

mDNSlocal void initializeQuery(DNSMessage *msg, DNSQuestion *question)
	{
	mDNSOpaque16 flags = QueryFlags;
	
	ubzero(msg, sizeof(msg));
	flags.b[0] |= kDNSFlag0_RD;  // recursion desired
    InitializeDNSMessage(&msg->h, question->uDNS_info.id, flags);
	}

mDNSlocal mStatus constructQueryMsg(DNSMessage *msg, mDNSu8 **endPtr, DNSQuestion *const question)
	{
	initializeQuery(msg, question);

	*endPtr = putQuestion(msg, msg->data, msg->data + AbsoluteMaxDNSMessageData, &question->qname, question->qtype, question->qclass);
    if (!*endPtr)
        {
        LogMsg("ERROR: Unicast query out of space in packet");
        return mStatus_UnknownErr;
        }
	return mStatus_NoError;
	}

mDNSlocal mDNSu8 *putLLQ(DNSMessage *const msg, mDNSu8 *ptr, DNSQuestion *question, LLQOptData *data, mDNSBool includeQuestion)
	{
	AuthRecord rr;
	ResourceRecord *opt = &rr.resrec; 
	rdataOpt *optRD;
	
	//!!!KRS when we implement multiple llqs per message, we'll need to memmove anything past the question section
	if (includeQuestion)
		{
		ptr = putQuestion(msg, ptr, msg->data + AbsoluteMaxDNSMessageData, &question->qname, question->qtype, question->qclass);
		if (!ptr) { LogMsg("ERROR: putLLQ - putQuestion"); return mDNSNULL; }
		}
	// locate OptRR if it exists, set pointer to end 
	// !!!KRS implement me

	
	// format opt rr (fields not specified are zero-valued)
	ubzero(&rr, sizeof(AuthRecord));
	opt->rdata = &rr.rdatastorage;
	
	opt->RecordType = kDNSRecordTypeKnownUnique;  // to avoid warnings in other layers
	opt->rrtype = kDNSType_OPT;
	opt->rdlength = LLQ_OPT_SIZE;
	opt->rdestimate = LLQ_OPT_SIZE;

	optRD = &rr.resrec.rdata->u.opt;
	optRD->opt = kDNSOpt_LLQ;
	optRD->optlen = sizeof(LLQOptData);
	umemcpy(&optRD->OptData.llq, data, sizeof(LLQOptData));
	ptr = PutResourceRecordTTL(msg, ptr, &msg->h.numAdditionals, opt, 0);
	if (!ptr) { LogMsg("ERROR: putLLQ - PutResourceRecordTTL"); return mDNSNULL; }

	return ptr;
	}

			  
mDNSlocal mDNSBool getLLQAtIndex(mDNS *m, DNSMessage *msg, const mDNSu8 *end, LLQOptData *llq, int index)
	{
	LargeCacheRecord lcr;
	int i;
	const mDNSu8 *ptr;
	
	ptr = LocateAdditionals(msg, end);
	if (!ptr) return mDNSfalse;

	// find the last additional
	for (i = 0; i < msg->h.numAdditionals; i++)
//		{ ptr = GetLargeResourceRecord(m, msg, ptr, end, 0, kDNSRecordTypePacketAdd, &lcr); if (!ptr) return mDNSfalse; }
//!!!KRS workaround for LH server bug, which puts OPT as first additional
		{ ptr = GetLargeResourceRecord(m, msg, ptr, end, 0, kDNSRecordTypePacketAdd, &lcr); if (!ptr) return mDNSfalse; if (lcr.r.resrec.rrtype == kDNSType_OPT) break; }		
	if (lcr.r.resrec.rrtype != kDNSType_OPT) return mDNSfalse;
	if (lcr.r.resrec.rdlength < (index + 1) * LLQ_OPT_SIZE) return mDNSfalse;  // rdata too small 
	umemcpy(llq, (mDNSu8 *)&lcr.r.resrec.rdata->u.opt.OptData.llq + (index * sizeof(LLQOptData)), sizeof(LLQOptData));
	return mDNStrue;
	}

mDNSlocal void recvRefreshReply(mDNS *m, DNSMessage *msg, const mDNSu8 *end, DNSQuestion *q)
	{
	LLQ_Info *qInfo;
	LLQOptData pktData;

	qInfo = q->uDNS_info.llq;
	if (!getLLQAtIndex(m, msg, end, &pktData, 0)) { LogMsg("ERROR recvRefreshReply - getLLQAtIndex"); return; }
	if (pktData.llqOp != kLLQOp_Refresh) return;
	if (!sameID(pktData.id, qInfo->id)) { LogMsg("recvRefreshReply - ID mismatch.  Discarding");  return; }
	if (pktData.err != LLQErr_NoError) { LogMsg("recvRefreshReply: received error %d from server", pktData.err); return; }

	qInfo->expire = mDNSPlatformTimeNow(m) + ((mDNSs32)pktData.lease * mDNSPlatformOneSecond);
	qInfo->retry = qInfo->expire + ((mDNSs32)pktData.lease * mDNSPlatformOneSecond * 3/4);
 
	qInfo->origLease = pktData.lease;
	qInfo->state = LLQ_Established;	
	}

mDNSlocal void sendLLQRefresh(mDNS *m, DNSQuestion *q, mDNSu32 lease)
	{
	DNSMessage msg;
	mDNSu8 *end;
	LLQOptData llq;
	LLQ_Info *info = q->uDNS_info.llq;
	mStatus err;
	mDNSs32 timenow;

	timenow = mDNSPlatformTimeNow(m);
	if ((info->state == LLQ_Refresh && info->ntries >= kLLQ_MAX_TRIES) ||
		info->expire - timenow < 0)
		{
		LogMsg("Unable to refresh LLQ %##s - will retry in %d minutes", q->qname.c, kLLQ_DEF_RETRY/60);
		info->state = LLQ_Retry;
		info->retry = mDNSPlatformTimeNow(m) + kLLQ_DEF_RETRY * mDNSPlatformOneSecond;
		info->deriveRemovesOnResume = mDNStrue;
		return;
		//!!!KRS handle this - periodically try to re-establish
		}

	llq.vers = kLLQ_Vers;
	llq.llqOp = kLLQOp_Refresh;
	llq.err = LLQErr_NoError;
	umemcpy(llq.id, info->id, 8);
	llq.lease = lease;

	initializeQuery(&msg, q);
	end = putLLQ(&msg, msg.data, q, &llq, mDNStrue);
	if (!end) { LogMsg("ERROR: sendLLQRefresh - putLLQ"); return; }
	
	err = mDNSSendDNSMessage(m, &msg, end, q->InterfaceID, &info->servAddr, info->servPort);
	if (err) LogMsg("ERROR: sendLLQRefresh - mDNSSendDNSMessage returned %ld", err);

	if (info->state == LLQ_Established) info->ntries = 1;
	else info->ntries++;
	info->state = LLQ_Refresh;
	q->LastQTime = timenow;
	info->retry = (info->expire - q->LastQTime) / 2;	
	}

mDNSlocal void recvLLQEvent(mDNS *m, DNSQuestion *q, DNSMessage *msg, const mDNSu8 *end, const mDNSAddr *srcaddr, mDNSIPPort srcport, mDNSInterfaceID InterfaceID)
	{
	DNSMessage ack;
	mDNSu8 *ackEnd = ack.data;
	mStatus err;

    // invoke response handler
	m->uDNS_info.CurrentQuery = q;
	q->uDNS_info.responseCallback(m, msg, end, q, q->uDNS_info.context);
	if (m->uDNS_info.CurrentQuery != q) return;
	
    //  format and send ack
	InitializeDNSMessage(&ack.h, msg->h.id, ResponseFlags);
	ackEnd = putQuestion(&ack, ack.data, ack.data + AbsoluteMaxDNSMessageData, &q->qname, q->qtype, q->qclass);
	if (!ackEnd) { LogMsg("ERROR: recvLLQEvent - putQuestion");  return; }
	err = mDNSSendDNSMessage(m, &ack, ackEnd, InterfaceID, srcaddr, srcport); 
	if (err) LogMsg("ERROR: recvLLQEvent - mDNSSendDNSMessage returned %ld", err);
    }



mDNSlocal void hndlChallengeResponseAck(mDNS *m, DNSMessage *pktMsg, const mDNSu8 *end, LLQOptData *llq, DNSQuestion *q)
	{
	LLQ_Info *info = q->uDNS_info.llq;
	
	if (llq->err) { LogMsg("hndlChallengeResponseAck - received error %d from server", llq->err); goto error; }
	if (!sameID(info->id, llq->id)) { LogMsg("hndlChallengeResponseAck - ID changed.  discarding"); return; } // this can happen rarely (on packet loss + reordering)
	info->expire = mDNSPlatformTimeNow(m) + ((mDNSs32)llq->lease * mDNSPlatformOneSecond);
	info->retry = info->expire + ((mDNSs32)llq->lease * mDNSPlatformOneSecond * 3/4);
 
	info->origLease = llq->lease;
	info->state = LLQ_Established;	
	q->uDNS_info.responseCallback = llqResponseHndlr;
	llqResponseHndlr(m, pktMsg, end, q, mDNSNULL);
	return;

	error:
	info->state = LLQ_Error;
	}

mDNSlocal void sendChallengeResponse(mDNS *m, DNSQuestion *q, LLQOptData *llq)
	{
	LLQ_Info *info = q->uDNS_info.llq;
	DNSMessage response;
	mDNSu8 *responsePtr = response.data;
	mStatus err;
	LLQOptData llqBuf;
	mDNSs32 timenow = mDNSPlatformTimeNow(m);
	
	if (info->ntries++ == kLLQ_MAX_TRIES)
		{
		LogMsg("sendChallengeResponse: %d failed attempts for LLQ %##s. Will re-try in %d minutes",
			   kLLQ_MAX_TRIES, q->qname.c, kLLQ_DEF_RETRY / 60);
		info->state = LLQ_Retry;
		info->retry = timenow + (kLLQ_DEF_RETRY * mDNSPlatformOneSecond);
		// !!!KRS give a callback error in these cases?
		return;
		}
		

	if (!llq)
		{
		llq = &llqBuf;
		llq->vers    = kLLQ_Vers;
		llq->llqOp   = kLLQOp_Setup;
		llq->err     = LLQErr_NoError;
		umemcpy(llq->id, info->id, 8);
		llq->lease    = info->origLease;
		}

	q->LastQTime = timenow;
	info->retry = timenow  + (kLLQ_INIT_RESEND * info->ntries * mDNSPlatformOneSecond);
	
	if (constructQueryMsg(&response, &responsePtr, q)) goto error;
	responsePtr = putLLQ(&response, responsePtr, q, llq, mDNSfalse);
	if (!responsePtr) { LogMsg("ERROR: sendChallengeResponse - putLLQ"); goto error; }
	
	err = mDNSSendDNSMessage(m, &response, responsePtr, q->InterfaceID, &info->servAddr, info->servPort);
	if (err) LogMsg("ERROR: sendChallengeResponse - mDNSSendDNSMessage returned %ld", err);
	// on error, we procede as normal and retry after the appropriate interval

	return;

	error:
	info->state = LLQ_Error;
	}



mDNSlocal void hndlRequestChallenge(mDNS *m, DNSMessage *pktMsg, const mDNSu8 *end, LLQOptData *llq, DNSQuestion *q)
	{
	LLQ_Info *info = q->uDNS_info.llq;
	mDNSs32 timenow = mDNSPlatformTimeNow(m);
	switch(llq->err)
		{
		case LLQErr_NoError: break;
		case LLQErr_ServFull:
			LogMsg("hndlRequestChallenge - received ServFull from server for LLQ %##s.  Retry in %lu sec", q->qname.c, llq->lease);
			info->retry = timenow + ((mDNSs32)llq->lease * mDNSPlatformOneSecond);
			info->state = LLQ_Retry;
			simpleResponseHndlr(m, pktMsg, end, q, mDNSNULL);  // get available answers
			info->deriveRemovesOnResume = mDNStrue;
		case LLQErr_Static:
			info->state = LLQ_Static;
			LogMsg("LLQ %##s: static", q->qname.c);
			simpleResponseHndlr(m, pktMsg, end, q, mDNSNULL);
			return;
		case LLQErr_FormErr:
			LogMsg("ERROR: hndlRequestChallenge - received FormErr from server for LLQ %##s", q->qname.c);
			goto error;
		case LLQErr_BadVers:
			LogMsg("ERROR: hndlRequestChallenge - received BadVers from server");
			goto error;
		case LLQErr_UnknownErr:
			LogMsg("ERROR: hndlRequestChallenge - received UnknownErr from server for LLQ %##s", q->qname.c);
			goto error;
		default:
			LogMsg("ERROR: hndlRequestChallenge - received invalid error %d for LLQ %##s", llq->err, q->qname.c);
			goto error;
		}

	if (info->origLease != llq->lease)
		LogMsg("hndlRequestChallenge: requested lease %lu, granted lease %lu", info->origLease, llq->lease);

	// cache expiration in case we go to sleep before finishing setup
	info->origLease = llq->lease;
	info->expire = timenow + ((mDNSs32)llq->lease * mDNSPlatformOneSecond);

	// update state and timestamp
	info->state = LLQ_SecondaryRequest;
	umemcpy(info->id, llq->id, 8);
	info->ntries = 0; // first attempt to send response

	sendChallengeResponse(m, q, llq);
	return;


	error:
	info->state = LLQ_Error;	
	}


// response handler for initial and secondary setup responses
mDNSlocal void recvSetupResponse(mDNS *m, DNSMessage *pktMsg, const mDNSu8 *end, DNSQuestion *q, void *clientContext)
	{
	DNSQuestion pktQuestion;
	LLQOptData llq;
	const mDNSu8 *ptr = pktMsg->data;
	LLQ_Info *info = q->uDNS_info.llq;
	mDNSu8 rcode = (mDNSu8)(pktMsg->h.flags.b[1] & kDNSFlag1_RC);

	(void)clientContext;  // unused
	
	if (rcode && rcode != kDNSFlag1_RC_NXDomain) goto poll;
	
	ptr = getQuestion(pktMsg, ptr, end, 0, &pktQuestion);
	if (!ptr) { LogMsg("ERROR: recvSetupResponse - getQuestion"); goto poll; }
	if (!SameDomainName(&q->qname, &pktQuestion.qname))
		{ LogMsg("ERROR: recvSetupResponse - mismatched question in response for llq setup %##s", q->qname.c);   goto poll; }

	if (!getLLQAtIndex(m, pktMsg, end, &llq, 0)) { debugf("recvSetupResponse - GetLLQAtIndex"); goto poll; }
	if (llq.llqOp != kLLQOp_Setup) { LogMsg("ERROR: recvSetupResponse - bad op %d", llq.llqOp); goto poll; } 
	if (llq.vers != kLLQ_Vers) { LogMsg("ERROR: recvSetupResponse - bad vers %d", llq.vers);  goto poll; } 

	if (info->state == LLQ_InitialRequest) { hndlRequestChallenge(m, pktMsg, end, &llq, q); return; }
	if (info->state == LLQ_SecondaryRequest) { hndlChallengeResponseAck(m, pktMsg, end, &llq, q); return; }
	LogMsg("recvSetupResponse - bad state %d", info->state);

	poll:
	info->state = LLQ_Poll;
	q->uDNS_info.responseCallback = simpleResponseHndlr;
	info->question->LastQTime = mDNSPlatformTimeNow(m) - (2 * INIT_UCAST_POLL_INTERVAL);  // trigger immediate poll
	info->question->ThisQInterval = INIT_UCAST_POLL_INTERVAL;
	}



mDNSlocal void startLLQHandshake(mDNS *m, LLQ_Info *info)
	{
	DNSMessage msg;
	mDNSu8 *end;
	LLQOptData llqData;
	DNSQuestion *q = info->question; 
	mStatus err;
	mDNSs32 timenow = mDNSPlatformTimeNow(m);
	
	if (info->ntries++ >= kLLQ_MAX_TRIES)
		{
		LogMsg("startLLQHandshake: %d failed attempts for LLQ %##s. Will re-try in %d minutes",
			   kLLQ_MAX_TRIES, q->qname.c, kLLQ_DEF_RETRY / 60);
		info->state = LLQ_Retry;
		info->retry = timenow + (kLLQ_DEF_RETRY * mDNSPlatformOneSecond);
		// !!!KRS give a callback error in these cases?
		return;
		}	
	
    // set llq rdata
	llqData.vers    = kLLQ_Vers;
	llqData.llqOp   = kLLQOp_Setup;
	llqData.err     = LLQErr_NoError;
	ubzero(llqData.id, 8);
	llqData.lease    = kLLQ_DefLease;

	initializeQuery(&msg, q);
	end = putLLQ(&msg, msg.data, q, &llqData, mDNStrue);
	if (!end)
		{
		LogMsg("ERROR: startLLQHandshake - putLLQ");
		info->state = LLQ_Error;		
		return;
		}

	err = mDNSSendDNSMessage(m, &msg, end, q->InterfaceID, &info->servAddr, info->servPort);
	if (err) LogMsg("ERROR: startLLQHandshake - mDNSSendDNSMessage returned %ld", err);
	// on error, we procede as normal and retry after the appropriate interval

	// update question/info state
	info->state = LLQ_InitialRequest;
	info->origLease = kLLQ_DefLease;
    info->retry = timenow + (kLLQ_INIT_RESEND * mDNSPlatformOneSecond);
	q->LastQTime = timenow;
	q->uDNS_info.responseCallback = recvSetupResponse;
	q->uDNS_info.internal = mDNStrue;
	}

// wrapper for startLLQHandshake, invoked by async op callback
mDNSlocal void startLLQHandshakeCallback(mStatus err, mDNS *const m, void *llqInfo, const AsyncOpResult *result)
	{
	LLQ_Info *info = (LLQ_Info *)llqInfo;
	const zoneData_t *zoneInfo = mDNSNULL;
	
    // check state first to make sure it is OK to touch question object
	if (info->state == LLQ_Cancelled)
		{
		// StopQuery was called while we were getting the zone info
		debugf("startLLQHandshake - LLQ Cancelled.");
		info->question = mDNSNULL;  // question may be deallocated
		ufree(info);
		return;
		}

	if (!info->question)
		{ LogMsg("ERROR: startLLQHandshakeCallback invoked with NULL question"); goto error; }

	if (info->state != LLQ_GetZoneInfo)
		{ LogMsg("ERROR: startLLQHandshake - bad state %d", info->state); goto error; }

	if (err)
		{ LogMsg("ERROR: startLLQHandshakeCallback invoked with error code %ld", err); goto poll; }

	if (!result)
		{ LogMsg("ERROR: startLLQHandshakeCallback invoked with NULL result and no error code"); goto error; }
	
	zoneInfo = &result->zoneData;

	if (!zoneInfo->llqPort.NotAnInteger)
		{ debugf("LLQ port lookup failed - reverting to polling"); goto poll; }
		
    // cache necessary zone data
	info->servAddr.type = zoneInfo->primaryAddr.type;
	info->servAddr.ip.v4.NotAnInteger = zoneInfo->primaryAddr.ip.v4.NotAnInteger;
	info->servPort.NotAnInteger = zoneInfo->llqPort.NotAnInteger;
    info->ntries = 0;

	if (info->state == LLQ_SuspendDeferred) info->state = LLQ_Suspended;
	else startLLQHandshake(m, info);
	return;

	poll:
	info->question->uDNS_info.responseCallback = simpleResponseHndlr;
	info->state = LLQ_Poll;
	info->question->LastQTime = mDNSPlatformTimeNow(m) - (2 * INIT_UCAST_POLL_INTERVAL);  // trigger immediate poll
	info->question->ThisQInterval = INIT_UCAST_POLL_INTERVAL;
	return;

	error:
	info->state = LLQ_Error;
	}

mDNSlocal mStatus startLLQ(mDNS *m, DNSQuestion *question)
    {
	LLQ_Info *info;
	mStatus err = mStatus_NoError;
	
	// allocate / init info struct
    info = umalloc(sizeof(LLQ_Info));
    if (!info) { LogMsg("ERROR: startLLQ - malloc"); return mStatus_NoMemoryErr; }
	ubzero(info, sizeof(LLQ_Info));
    info->state = LLQ_GetZoneInfo;
	
	// link info/question
	info->question = question;
	question->uDNS_info.llq = info;

	question->uDNS_info.responseCallback = llqResponseHndlr;
	
	err = startGetZoneData(&question->qname, m, mDNSfalse, mDNStrue, startLLQHandshakeCallback, info);
    if (err)
		{
		LogMsg("ERROR: startLLQ - startGetZoneData returned %ld", err);
		info->question = mDNSNULL;
		ufree(info);
		question->uDNS_info.llq = mDNSNULL;
		return err;
		}

	LinkActiveQuestion(&m->uDNS_info, question);
	return err;
	}

mDNSlocal mDNSBool recvLLQResponse(mDNS *m, DNSMessage *msg, const mDNSu8 *end, const mDNSAddr *srcaddr, mDNSIPPort srcport, const mDNSInterfaceID InterfaceID)
	{
	DNSQuestion pktQ, *q;
	uDNS_GlobalInfo *u = &m->uDNS_info;
	const mDNSu8 *ptr = msg->data;
	LLQ_Info *llqInfo;
	
	if (!msg->h.numQuestions) return mDNSfalse;

	ptr = getQuestion(msg, ptr, end, 0, &pktQ);
	if (!ptr) return mDNSfalse;
	pktQ.uDNS_info.id = msg->h.id;
	
	// !!!KRS we should do a table lookup to quickly determine if this packet is for an LLQ
   
	q = u->ActiveQueries;
	while (q)
		{
		llqInfo = q->uDNS_info.llq;
		if (q->LongLived &&                          
			llqInfo &&
			q->qnamehash == pktQ.qnamehash &&        
			q->qtype == pktQ.qtype &&
			SameDomainName(&q->qname, &pktQ.qname))
			{
			u->CurrentQuery = q;
			if (llqInfo->state == LLQ_Established || (llqInfo->state == LLQ_Refresh && msg->h.numAnswers))			
				{ recvLLQEvent(m, q, msg, end, srcaddr, srcport, InterfaceID); return mDNStrue; }
			else if (msg->h.id.NotAnInteger != q->uDNS_info.id.NotAnInteger)
				{ q = q->next; continue; }
			else if (llqInfo->state == LLQ_Refresh && msg->h.numAdditionals && !msg->h.numAnswers)
				{ recvRefreshReply(m, msg, end, q); return mDNStrue; }
			else if (llqInfo->state < LLQ_Static)
				{ q->uDNS_info.responseCallback(m, msg, end, q, q->uDNS_info.context); return mDNStrue; }			
			}
		q = q->next;
		}
	return mDNSfalse;
	}

mDNSexport mDNSBool uDNS_IsActiveQuery(DNSQuestion *const question, uDNS_GlobalInfo *u)
    {
	DNSQuestion *q;

	for (q = u->ActiveQueries; q; q = q->next)
		{
		if (q == question)
			{
			if (!question->uDNS_info.id.NotAnInteger || question->InterfaceID || IsLocalDomain(&question->qname))
				LogMsg("Warning: Question %##s in Active Unicast Query list with id %d, interfaceID %p",
					   question->qname.c, question->uDNS_info.id.NotAnInteger, question->InterfaceID);
			return mDNStrue;
			}
		}
	return mDNSfalse;
	}

// stopLLQ happens IN ADDITION to stopQuery
mDNSlocal void stopLLQ(mDNS *m, DNSQuestion *question)
	{	
	LLQ_Info *info = question->uDNS_info.llq;
	(void)m;  // unused

	if (!question->LongLived) { LogMsg("ERROR: stopLLQ - LongLived flag not set"); return; }
	if (!info)                { LogMsg("ERROR: stopLLQ - llq info is NULL");       return; }

	switch (info->state)
		{
		case LLQ_UnInit:
			LogMsg("ERROR: stopLLQ - state LLQ_UnInit");
			//!!!KRS should we unlink info<->question here?
			return;
		case LLQ_GetZoneInfo:
		case LLQ_SuspendDeferred:		
			info->question = mDNSNULL; // remove ref to question, as it may be freed when we get called back from async op
			info->state = LLQ_Cancelled;
			return;			
		case LLQ_Established:
		case LLQ_Refresh:
			// refresh w/ lease 0
			sendLLQRefresh(m, question, 0);
			goto free_info;	
		default:			
			debugf("stopLLQ - silently discarding LLQ in state %d", info->state);				
			goto free_info;
		}
	
	free_info:
	info->question = mDNSNULL;
	ufree(info);
	question->uDNS_info.llq = mDNSNULL;
	
	}

mDNSexport mStatus uDNS_StopQuery(mDNS *const m, DNSQuestion *const question)
	{
	uDNS_GlobalInfo *u = &m->uDNS_info;
	DNSQuestion *qptr, *prev = mDNSNULL;
	CacheRecord *ka;
	
	qptr = u->ActiveQueries;
	while (qptr)
        {
        if (qptr == question)
            {
			if (question->LongLived && question->uDNS_info.llq)
				stopLLQ(m, question);
			if (m->uDNS_info.CurrentQuery == question)
				m->uDNS_info.CurrentQuery = m->uDNS_info.CurrentQuery->next;
			while (question->uDNS_info.knownAnswers)
				{
				ka = question->uDNS_info.knownAnswers;
				question->uDNS_info.knownAnswers = question->uDNS_info.knownAnswers->next;
				ufree(ka);
				}
			if (prev) prev->next = question->next;
            else u->ActiveQueries = question->next;
			return mStatus_NoError;
            }
        prev = qptr;
		qptr = qptr->next;
        }
    LogMsg("uDNS_StopQuery: no such active query (%##s)", question->qname.c);
    return mStatus_UnknownErr;
    }

mDNSlocal mStatus startQuery(mDNS *const m, DNSQuestion *const question, mDNSBool internal)
    {
    uDNS_GlobalInfo *u = &m->uDNS_info;
    DNSMessage msg;
    mDNSu8 *endPtr;
    mStatus err = mStatus_NoError;
	const mDNSAddr *server;
	
    //!!!KRS we should check if the question is already in our acivequestion list
	if (!ValidateDomainName(&question->qname))
		{
		LogMsg("Attempt to start query with invalid qname %##s %##s", question->qname.c, DNSTypeName(question->qtype));
		return mStatus_Invalid;
		}
		
	question->next = mDNSNULL;
	question->qnamehash = DomainNameHashValue(&question->qname);    // to do quick domain name comparisons
    question->uDNS_info.id = newMessageID(u);
	
	// break here if its and LLQ
	if (question->LongLived) return startLLQ(m, question);

	// else send the query to our server
	err = constructQueryMsg(&msg, &endPtr, question);
	if (err) return err;

	question->LastQTime = mDNSPlatformTimeNow(m);
	question->ThisQInterval = INIT_UCAST_POLL_INTERVAL;
    // store the question/id in active question list
    question->uDNS_info.timestamp = question->LastQTime;
	question->uDNS_info.internal = internal;
	LinkActiveQuestion(u, question);
	question->uDNS_info.knownAnswers = mDNSNULL;

	server = getInitializedDNS(u);
	if (!server) { debugf("startQuery - no initialized DNS"); err =  mStatus_NotInitializedErr; }
	else err = mDNSSendDNSMessage(m, &msg, endPtr, question->InterfaceID, server, UnicastDNSPort);	
	if (err) { debugf("ERROR: startQuery - %ld (keeping question in list for retransmission", err); }

	return err;
	}
	
mDNSexport mStatus uDNS_StartQuery(mDNS *const m, DNSQuestion *const question)
    {
	ubzero(&question->uDNS_info, sizeof(uDNS_QuestionInfo));
	question->uDNS_info.responseCallback = simpleResponseHndlr;
	question->uDNS_info.context = mDNSNULL;
	return startQuery(m, question, 0);
    }

// explicitly set response handler
mDNSlocal mStatus startInternalQuery(DNSQuestion *q, mDNS *m, InternalResponseHndlr callback, void *hndlrContext)
    {
	ubzero(&q->uDNS_info, sizeof(uDNS_QuestionInfo));
    q->QuestionContext = hndlrContext;	
    q->uDNS_info.responseCallback = callback;
	q->uDNS_info.context = hndlrContext;
    return startQuery(m, q, 1);
    }



// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Domain -> Name Server Conversion
#endif


/* startGetZoneData
 *
 * Asynchronously find the address of the nameserver for the enclosing zone for a given domain name,
 * i.e. the server to which update and LLQ requests will be sent for a given name.  Once the address is
 * derived, it will be passed to the callback, along with a context pointer.  If the zone cannot
 * be determined or if an error occurs, an all-zeros address will be passed and a message will be
 * written to the syslog.
 *
 * If the FindUpdatePort arg is set, the port on which the server accepts dynamic updates is determined
 * by querying for the _update._dns-sd._udp.<zone>. SRV record.  Likewise, if the FindLLQPort arg is set,
 * the port on which the server accepts long lived queries is determined by querying for _llq._dns-sd.
 * _udp.<zone>. record.  If either of these queries fail, or flags are not specified, the llqPort and
 * updatePort fields in the result structure are set to zero.
 *
 *  Steps for deriving the zone name are as follows:
 *
 * Query for an SOA record for the required domain.  If we don't get an answer (or an SOA in the Authority
 * section), we strip the leading label from the name and repeat, until we get an answer.
 *
 * The name of the SOA record is our enclosing zone.  The mname field in the SOA rdata is the domain
 * name of the primary NS.
 *
 * We verify that there is an NS record with this zone for a name and the mname for its rdata.
 * (!!!KRS this seems redundant, but BIND does this, and it should normally be zero-overhead since
 * the NS query will get us address records in the additionals section, which we'd otherwise have to
 * explicitly query for.)
 *
 * We then query for the address record for this nameserver (if it is not in the addionals section of
 * the NS record response.)
 */
 

// state machine types and structs
//

// state machine states
typedef enum
    {
    init,
    lookupSOA,
	foundZone,
	lookupNS,
	foundNS,
	lookupA,
	foundA,
	lookupPort,
	foundPort,
	complete
    } ntaState;

// state machine actions
typedef enum
    {
    smContinue,  // continue immediately to next state 
    smBreak,     // break until next packet/timeout 
	smError      // terminal error - cleanup and abort
    } smAction;
 
typedef struct
    {
    domainname 	origName;            // name we originally try to convert
    domainname 	*curSOA;             // name we have an outstanding SOA query for
    ntaState  	state;               // determines what we do upon receiving a packet
    mDNS	    *m;
    domainname  zone;                // left-hand-side of SOA record
    mDNSu16     zoneClass; 
    domainname  ns;                  // mname in SOA rdata, verified in confirmNS state
    mDNSv4Addr  addr;                // address of nameserver
    DNSQuestion question;            // storage for any active question
    DNSQuestion extraQuestion;       // additional storage
    mDNSBool    questionActive;      // if true, StopQuery() can be called on the question field
    mDNSBool    findUpdatePort;      
    mDNSBool    findLLQPort;
    mDNSIPPort  updatePort;
    mDNSIPPort  llqPort;
    AsyncOpCallback *callback;       // caller specified function to be called upon completion
    void        *callbackInfo;
    } ntaContext;


// function prototypes (for routines that must be used as fn pointers prior to their definitions,
// and allows states to be read top-to-bottom in logical order)
mDNSlocal void getZoneData(mDNS *const m, DNSMessage *msg, const mDNSu8 *end, DNSQuestion *question, void *contextPtr);
mDNSlocal smAction hndlLookupSOA(DNSMessage *msg, const mDNSu8 *end, ntaContext *context);
mDNSlocal void processSOA(ntaContext *context, ResourceRecord *rr);
mDNSlocal smAction confirmNS(DNSMessage *msg, const mDNSu8 *end, ntaContext *context);
mDNSlocal smAction lookupNSAddr(DNSMessage *msg, const mDNSu8 *end, ntaContext *context);
mDNSlocal smAction hndlLookupPorts(DNSMessage *msg, const mDNSu8 *end, ntaContext *context);

// initialization
mDNSlocal mStatus startGetZoneData(domainname *name, mDNS *m, mDNSBool findUpdatePort, mDNSBool findLLQPort,
								   AsyncOpCallback callback, void *callbackInfo)
    {
    ntaContext *context = (ntaContext*)umalloc(sizeof(ntaContext));
    if (!context) { LogMsg("ERROR: startGetZoneData - umalloc failed");  return mStatus_NoMemoryErr; }
	ubzero(context, sizeof(ntaContext));
    AssignDomainName(context->origName, *name);
    context->state = init;
    context->m = m;
	context->callback = callback;
	context->callbackInfo = callbackInfo;
	context->findUpdatePort = findUpdatePort;
	context->findLLQPort = findLLQPort;
    getZoneData(m, mDNSNULL, mDNSNULL, mDNSNULL, context);
    return mStatus_NoError;
    }

// state machine entry routine
mDNSlocal void getZoneData(mDNS *const m, DNSMessage *msg, const mDNSu8 *end, DNSQuestion *question, void *contextPtr)
    {
	AsyncOpResult result;
	ntaContext *context = (ntaContext*)contextPtr;
	smAction action;

    // unused
	(void)m;
	(void)question;
	
	// stop any active question
	if (context->questionActive)
		{
		uDNS_StopQuery(context->m, &context->question);
		context->questionActive = mDNSfalse;
		}

	if (msg && msg->h.flags.b[2] >> 4 && msg->h.flags.b[2] >> 4 != kDNSFlag1_RC_NXDomain)
		{
		// rcode non-zero, non-nxdomain
		LogMsg("ERROR: getZoneData - received response w/ rcode %d", msg->h.flags.b[2] >> 4);
		goto error;
		}
 	
	switch (context->state)
        {
        case init:
        case lookupSOA:
            action = hndlLookupSOA(msg, end, context);
			if (action == smError) goto error;
			if (action == smBreak) return;
		case foundZone:
		case lookupNS:
			action = confirmNS(msg, end, context);
			if (action == smError) goto error;
			if (action == smBreak) return;
		case foundNS:
		case lookupA:
			action = lookupNSAddr(msg, end, context);
			if (action == smError) goto error;
			if (action == smBreak) return;
		case foundA:
			if (!context->findUpdatePort && !context->findLLQPort)
				{
				context->state = complete;
				break;
				}
		case lookupPort:
			action = hndlLookupPorts(msg, end, context);
			if (action == smError) goto error;
			if (action == smBreak) return;
			if (action == smContinue) context->state = complete;
		case foundPort:
		case complete: break;			
		}
					  
	if (context->state != complete)
		{
		LogMsg("ERROR: getZoneData - exited state machine with state %d", context->state);
		goto error;
		}
	
	result.type = zoneDataResult;
	result.zoneData.primaryAddr.ip.v4.NotAnInteger = context->addr.NotAnInteger;
	result.zoneData.primaryAddr.type = mDNSAddrType_IPv4;
	AssignDomainName(result.zoneData.zoneName, context->zone);
	result.zoneData.zoneClass = context->zoneClass;
	result.zoneData.llqPort    = context->findLLQPort    ? context->llqPort    : zeroIPPort;
	result.zoneData.updatePort = context->findUpdatePort ? context->updatePort : zeroIPPort;
	context->callback(mStatus_NoError, context->m, context->callbackInfo, &result);
	goto cleanup;	
			
error:
	if (context && context->callback)
		context->callback(mStatus_UnknownErr, context->m, context->callbackInfo, mDNSNULL);	
cleanup:
	if (context && context->questionActive)
		{
		uDNS_StopQuery(context->m, &context->question);
		context->questionActive = mDNSfalse;
		}		
    if (context) ufree(context);
	}

mDNSlocal smAction hndlLookupSOA(DNSMessage *msg, const mDNSu8 *end, ntaContext *context)
    {
    mStatus err;
    LargeCacheRecord lcr;
	ResourceRecord *rr = &lcr.r.resrec;
	DNSQuestion *query = &context->question;
	const mDNSu8 *ptr;
	
    if (msg)
        {
        // if msg contains SOA record in answer or authority sections, update context/state and return 		
		int i;
		ptr = LocateAnswers(msg, end);
		for (i = 0; i < msg->h.numAnswers; i++)
			{
			ptr = GetLargeResourceRecord(context->m, msg, ptr, end, 0, kDNSRecordTypePacketAns, &lcr);
			if (!ptr) { LogMsg("ERROR: hndlLookupSOA, Answers - GetLargeResourceRecord returned NULL");  return smError; }
			if (rr->rrtype == kDNSType_SOA && SameDomainName(context->curSOA, &rr->name))
				{
				processSOA(context, rr);
				return smContinue;
				}
			}		
		ptr = LocateAuthorities(msg, end);
		// SOA not in answers, check in authority
		for (i = 0; i < msg->h.numAuthorities; i++)
			{
			ptr = GetLargeResourceRecord(context->m, msg, ptr, end, 0, kDNSRecordTypePacketAns, &lcr); ///!!!KRS using type PacketAns for auth
			if (!ptr) { LogMsg("ERROR: hndlLookupSOA, Authority - GetLargeResourceRecord returned NULL");  return smError; }		 		
			if (rr->rrtype == kDNSType_SOA)
				{
				processSOA(context, rr);
				return smContinue;
				}
			}
		}    

    if (context->state != init && !context->curSOA->c[0])
        {
        // we've gone down to the root and have not found an SOA  
        LogMsg("ERROR: hndlLookupSOA - recursed to root label of %##s without finding SOA", 
                context->origName.c);
		return smError;
        }

    ubzero(query, sizeof(DNSQuestion));
    // chop off leading label unless this is our first try
    if (context->state == init)  context->curSOA = &context->origName;
    else                         context->curSOA = (domainname *)(context->curSOA->c + context->curSOA->c[0]+1);
    
    context->state = lookupSOA;
    AssignDomainName(query->qname, *context->curSOA);
    query->qtype = kDNSType_SOA;
    query->qclass = kDNSClass_IN;
    err = startInternalQuery(query, context->m, getZoneData, context);
	context->questionActive = mDNStrue;
	if (err) LogMsg("hndlLookupSOA: startInternalQuery returned error %ld (breaking until next periodic retransmission)", err);

    return smBreak;     // break from state machine until we receive another packet	
    }

mDNSlocal void processSOA(ntaContext *context, ResourceRecord *rr)
	{
	AssignDomainName(context->zone, rr->name);
	context->zoneClass = rr->rrclass;
	AssignDomainName(context->ns, rr->rdata->u.soa.mname);
	context->state = foundZone;
	}


mDNSlocal smAction confirmNS(DNSMessage *msg, const mDNSu8 *end, ntaContext *context)
	{
	DNSQuestion *query = &context->question;
	mStatus err;
	LargeCacheRecord lcr;
	ResourceRecord *rr = &lcr.r.resrec;
	const mDNSu8 *ptr;
	int i;
		
	if (context->state == foundZone)
		{
		// we've just learned the zone.  confirm that an NS record exists
		AssignDomainName(query->qname, context->zone);
		query->qtype = kDNSType_NS;
		query->qclass = kDNSClass_IN;
		err = startInternalQuery(query, context->m, getZoneData, context);
		context->questionActive = mDNStrue;	   
		if (err) LogMsg("confirmNS: startInternalQuery returned error %ld (breaking until next periodic retransmission", err);
		context->state = lookupNS;
		return smBreak;  // break from SM until we receive another packet
		}
	else if (context->state == lookupNS)
		{
		ptr = LocateAnswers(msg, end);
		for (i = 0; i < msg->h.numAnswers; i++)
			{
			ptr = GetLargeResourceRecord(context->m, msg, ptr, end, 0, kDNSRecordTypePacketAns, &lcr);
			if (!ptr) { LogMsg("ERROR: confirmNS, Answers - GetLargeResourceRecord returned NULL");  return smError; }
			if (rr->rrtype == kDNSType_NS &&
				SameDomainName(&context->zone, &rr->name) && SameDomainName(&context->ns, &rr->rdata->u.name))
				{
				context->state = foundNS;
				return smContinue;  // next routine will examine additionals section of A record				
				}				
			}
		LogMsg("ERROR: could not confirm existence of NS record %##s", context->zone.c);
		return smError;
		}
	else { LogMsg("ERROR: confirmNS - bad state %d", context->state); return smError; }
	}

mDNSlocal smAction queryNSAddr(ntaContext *context)
	{
	mStatus err;
	DNSQuestion *query = &context->question;
	
	AssignDomainName(query->qname, context->ns);
	query->qtype = kDNSType_A;
	query->qclass = kDNSClass_IN;
	err = startInternalQuery(query, context->m, getZoneData, context);
	context->questionActive = mDNStrue;	
	if (err) LogMsg("confirmNS: startInternalQuery returned error %ld (breaking until next periodic retransmission)", err);
	context->state = lookupA;
	return smBreak;
	}

mDNSlocal smAction lookupNSAddr(DNSMessage *msg, const mDNSu8 *end, ntaContext *context)
	{
	const mDNSu8 *ptr;
	int i;
	LargeCacheRecord lcr;
	ResourceRecord *rr = &lcr.r.resrec;
	
	if (context->state == foundNS)
		{
		// we just found the NS record - look for the corresponding A record in the Additionals section
		if (!msg->h.numAdditionals) return queryNSAddr(context);
		ptr = LocateAdditionals(msg, end);
		if (!ptr)
			{
			LogMsg("ERROR: lookupNSAddr - LocateAdditionals returned NULL, expected %d additionals", msg->h.numAdditionals);
			return queryNSAddr(context);
			}
		else
			{
			for (i = 0; i < msg->h.numAdditionals; i++)
				{
				ptr = GetLargeResourceRecord(context->m, msg, ptr, end, 0, kDNSRecordTypePacketAns, &lcr);
				if (!ptr)
					{
					LogMsg("ERROR: lookupNSAddr, Additionals - GetLargeResourceRecord returned NULL");
					return queryNSAddr(context);
					}
				if (rr->rrtype == kDNSType_A && SameDomainName(&context->ns, &rr->name))
					{
					context->addr.NotAnInteger = rr->rdata->u.ipv4.NotAnInteger;
					context->state = foundA;
					return smContinue;
					}
				}
			}
		// no A record in Additionals - query the server
		return queryNSAddr(context);
		}
	else if (context->state == lookupA)
		{
		ptr = LocateAnswers(msg, end);
		if (!ptr) { LogMsg("ERROR: lookupNSAddr: LocateAnswers returned NULL");  return smError; }
		for (i = 0; i < msg->h.numAnswers; i++)
			{
			ptr = GetLargeResourceRecord(context->m, msg, ptr, end, 0, kDNSRecordTypePacketAns, &lcr);
			if (!ptr) { LogMsg("ERROR: lookupNSAddr, Answers - GetLargeResourceRecord returned NULL"); break; }			
			if (rr->rrtype == kDNSType_A && SameDomainName(&context->ns, &rr->name))
				{
				context->addr.NotAnInteger = rr->rdata->u.ipv4.NotAnInteger;
				context->state = foundA;
				return smContinue;
				}
			}		
		LogMsg("ERROR: lookupNSAddr: Address record not found in answer section");
		return smError;
		}
	else { LogMsg("ERROR: lookupNSAddr - bad state %d", context->state); return smError; }
	}
	
mDNSlocal smAction lookupDNSPort(DNSMessage *msg, const mDNSu8 *end, ntaContext *context, char *portName, mDNSIPPort *port)
	{
	int i;
	LargeCacheRecord lcr;
	const mDNSu8 *ptr;
	DNSQuestion *q;
	mStatus err;
	
	if (context->state == lookupPort)  // we've already issued the query
		{
		if (!msg) { LogMsg("ERROR: hndlLookupUpdatePort - NULL message"); return smError; }
		ptr = LocateAnswers(msg, end);
		for (i = 0; i < msg->h.numAnswers; i++)
			{
			ptr = GetLargeResourceRecord(context->m, msg, ptr, end, 0, kDNSRecordTypePacketAns, &lcr);
			if (!ptr) { LogMsg("ERROR: hndlLookupUpdatePort - GetLargeResourceRecord returned NULL");  return smError; }
			if (ResourceRecordAnswersQuestion(&lcr.r.resrec, &context->question))
				{
				port->NotAnInteger = lcr.r.resrec.rdata->u.srv.port.NotAnInteger;
				context->state = foundPort;
				return smContinue;
				}
			}
		debugf("hndlLookupUpdatePort - no answer for type %s", portName);
		port->NotAnInteger = 0;
		context->state = foundPort;
		return smContinue;
		}

	// query the server for the update port for the zone
	context->state = lookupPort;
	q = &context->question;
	MakeDomainNameFromDNSNameString(&q->qname, portName);
	AppendDomainName(&q->qname, &context->zone);
    q->qtype = kDNSType_SRV;
    q->qclass = kDNSClass_IN;
    err = startInternalQuery(q, context->m, getZoneData, context);
	context->questionActive = mDNStrue;
    if (err) LogMsg("hndlLookupSOA: startInternalQuery returned error %ld (breaking until next periodic retransmission)", err);
    return smBreak;     // break from state machine until we receive another packet	
	}

mDNSlocal smAction hndlLookupPorts(DNSMessage *msg, const mDNSu8 *end, ntaContext *context)
	{
	smAction action;
	
	if (context->findUpdatePort && !context->updatePort.NotAnInteger)
		{
		action = lookupDNSPort(msg, end, context, UPDATE_PORT_NAME, &context->updatePort);
		if (action != smContinue) return action;
		}
	if (context->findLLQPort && !context->llqPort.NotAnInteger)
		return lookupDNSPort(msg, end, context, LLQ_PORT_NAME, &context->llqPort);

	return smContinue;
	}


// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Truncation Handling
#endif

typedef struct
	{
    DNSQuestion  *question;
    DNSMessage reply;
    mDNSu16  replylen;
    int nread;
    mDNS *m;
	} tcpInfo_t;

// issue queries over a conected socket
mDNSlocal void conQueryCallback(int sd, void *context, mDNSBool ConnectionEstablished)
	{
	mStatus err = 0;
	char msgbuf[356];  // 96 (hdr) + 256 (domain) + 4 (class/type)
	DNSMessage *msg;
	mDNSu8 *end;
	tcpInfo_t *info = (tcpInfo_t *)context;
	DNSQuestion *question = info->question;
	int n;

	question->uDNS_info.id.NotAnInteger = (mDNSu16)~0;

	if (ConnectionEstablished)
		{
		// connection is established - send the message
		msg = (DNSMessage *)&msgbuf;
		err = constructQueryMsg(msg, &end, question);
		if (err) { LogMsg("ERROR: conQueryCallback: constructQueryMsg - %ld", err);  goto error; }
		err = mDNSSendDNSMessage_tcp(info->m, msg, end, sd);
		if (err) { LogMsg("ERROR: conQueryCallback: mDNSSendDNSMessage_tcp - %ld", err);  goto error; }
		return;
		}
	else
		{
		if (!info->nread)
			{
			// read msg len
			n = mDNSPlatformReadTCP(sd, &info->replylen, 2);
			if (n != 2)
				{
				LogMsg("ERROR:conQueryCallback - attempt to read message length failed (read returned %d)", n);
				goto error;
				}
			}
		n = mDNSPlatformReadTCP(sd, ((char *)&info->reply) + info->nread, info->replylen - info->nread);
		if (n < 0) { LogMsg("ERROR: conQueryCallback - read returned %d", n); goto error; }
		info->nread += n;
		if (info->nread == info->replylen)
			{
			// finished reading message
			receiveMsg(info->m, &info->reply, ((mDNSu8 *)&info->reply) + info->replylen, question->InterfaceID);
			mDNSPlatformTCPCloseConnection(sd);
			ufree(info);
			return;
			}
		else return;
		}
	return;

	error:
	mDNSPlatformTCPCloseConnection(sd);
	ufree(info);
	}

mDNSlocal void hndlTruncatedAnswer(DNSQuestion *question, const  mDNSAddr *src, mDNS *m)
	{
	mStatus connectionStatus;
	uDNS_QuestionInfo *info = &question->uDNS_info;
	int sd;
	tcpInfo_t *context;

	context = (tcpInfo_t *)umalloc(sizeof(tcpInfo_t));
	if (!context) { LogMsg("ERROR: hndlTruncatedAnswer - memallocate failed"); return; }
	ubzero(context, sizeof(tcpInfo_t));
	context->question = question;
	context->m = m;

	info->id.NotAnInteger = (mDNSu16)~0;             // all 1's indicates TCP queries
	info->timestamp = mDNSPlatformTimeNow(m);         // reset timestamp

	connectionStatus = mDNSPlatformTCPConnect(src, UnicastDNSPort, question->InterfaceID, conQueryCallback, context, &sd);
	if (connectionStatus == mStatus_ConnEstablished)  // manually invoke callback if connection completes
		{
		conQueryCallback(sd, context, mDNStrue);
		return;
		}
	if (connectionStatus == mStatus_ConnPending) return; // callback will be automatically invoked when connection completes
	LogMsg("hndlTruncatedAnswer: connection failed");
	uDNS_StopQuery(m, question);  //!!!KRS can we really call this here?
	}


// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Dynamic Updates
#endif



mDNSlocal void sendRecordRegistration(mDNS *const m, AuthRecord *rr)
	{
	DNSMessage msg;
	mDNSu8 *ptr = msg.data;
	mDNSu8 *end = (mDNSu8 *)&msg + sizeof(DNSMessage);
	uDNS_GlobalInfo *u = &m->uDNS_info;
	mDNSOpaque16 id;
	uDNS_AuthInfo *authInfo;
	uDNS_RegInfo *regInfo = &rr->uDNS_info;
	mStatus err = mStatus_UnknownErr;

	id = newMessageID(u);
	InitializeDNSMessage(&msg.h, id, UpdateReqFlags);
	rr->uDNS_info.id.NotAnInteger = id.NotAnInteger;
	
    // set zone
	ptr = putZone(&msg, ptr, end, &regInfo->zone, mDNSOpaque16fromIntVal(rr->resrec.rrclass));
	if (!ptr) goto error;
	
	if (rr->resrec.RecordType == kDNSRecordTypeKnownUnique || rr->uDNS_info.state == regState_Refresh)
	  {
		// KnownUnique means the record must ALREADY exist, as does refresh
		// prereq: record must exist (put record in prereq section w/ TTL 0)
	    ptr = PutResourceRecordTTL(&msg, ptr, &msg.h.mDNS_numPrereqs, &rr->resrec, 0);
		if (!ptr) goto error;
	  }
	else if (rr->resrec.RecordType != kDNSRecordTypeShared)
		{
		ptr = putPrereqNameNotInUse(&rr->resrec.name, &msg, ptr, end);
		if (!ptr) goto error;
		}

	ptr = PutResourceRecord(&msg, ptr, &msg.h.mDNS_numUpdates, &rr->resrec);
	if (!ptr) goto error;

	if (rr->uDNS_info.lease)
		ptr = putUpdateLease(&msg, ptr, kLLQ_DefLease);
	   
	rr->uDNS_info.expire = -1;
	
	authInfo = GetAuthInfoForZone(u, &regInfo->zone);
	if (authInfo)
		{
		err = mDNSSendSignedDNSMessage(m, &msg, ptr, 0, &regInfo->ns, regInfo->port, authInfo);
		if (err) LogMsg("ERROR: sendRecordRegistration - mDNSSendSignedDNSMessage - %ld", err);
		}
	else
		{
		err = mDNSSendDNSMessage(m, &msg, ptr, 0, &regInfo->ns, regInfo->port);
		if (err) LogMsg("ERROR: sendRecordRegistration - mDNSSendDNSMessage - %ld", err);
		}

	SetRecordRetry(m, rr);
	
	if (regInfo->state != regState_Refresh) regInfo->state = regState_Pending;
	return;

error:
	if (rr->uDNS_info.state != regState_Unregistered)
		{
		unlinkAR(&u->RecordRegistrations, rr);
		rr->uDNS_info.state = regState_Unregistered;
		}
	m->mDNS_reentrancy++; // Increment to allow client to legally make mDNS API calls from the callback
	rr->RecordCallback(m, rr, err);
	m->mDNS_reentrancy--; // Decrement to block mDNS API calls again
	// NOTE: not safe to touch any client structures here	
	}

mDNSlocal void RecordRegistrationCallback(mStatus err, mDNS *const m, void *authPtr, const AsyncOpResult *result)	
	{
	AuthRecord *newRR = (AuthRecord*)authPtr;
	const zoneData_t *zoneData = mDNSNULL;
	uDNS_GlobalInfo *u = &m->uDNS_info;
	AuthRecord *ptr;

	// make sure record is still in list
	for (ptr = u->RecordRegistrations; ptr; ptr = ptr->next)
		if (ptr == newRR) break;
	if (!ptr) { LogMsg("RecordRegistrationCallback - RR no longer in list.  Discarding."); return; }		

	// check error/result
	if (err) { LogMsg("RecordRegistrationCallback: error %ld", err); goto error; }
	if (!result) { LogMsg("ERROR: RecordRegistrationCallback invoked with NULL result and no error"); goto error;  }
	else zoneData = &result->zoneData;

	if (newRR->uDNS_info.state == regState_Cancelled)
		{
		//!!!KRS we should send a memfree callback here!
		LogMsg("Registration of %##s type %d cancelled prior to update",
			   newRR->resrec.name.c, newRR->resrec.rrtype);
		newRR->uDNS_info.state = regState_Unregistered;
		unlinkAR(&u->RecordRegistrations, newRR);
		return;
		}
	
	if (result->type != zoneDataResult)
		{
		LogMsg("ERROR: buildUpdatePacket passed incorrect result type %d", result->type);
		goto error;
		}

	if (newRR->resrec.rrclass != zoneData->zoneClass)
		{
		LogMsg("ERROR: New resource record's class (%d) does not match zone class (%d)",
			   newRR->resrec.rrclass, zoneData->zoneClass);
		goto error;
		}	
	
	// cache zone data
	AssignDomainName(newRR->uDNS_info.zone, zoneData->zoneName);
    newRR->uDNS_info.ns.type = mDNSAddrType_IPv4;
	newRR->uDNS_info.ns.ip.v4.NotAnInteger = zoneData->primaryAddr.ip.v4.NotAnInteger;
	if (zoneData->updatePort.NotAnInteger) newRR->uDNS_info.port = zoneData->updatePort;
	else
		{
		debugf("Update port not advertised via SRV - guessing port 53, no lease option");
		newRR->uDNS_info.port = UnicastDNSPort;
		newRR->uDNS_info.lease = mDNSfalse;
		}

	sendRecordRegistration(m, newRR);
	return;
		
error:
	if (newRR->uDNS_info.state != regState_Unregistered)
		{
		unlinkAR(&u->RecordRegistrations, newRR);
		newRR->uDNS_info.state = regState_Unregistered;
		}
	m->mDNS_reentrancy++; // Increment to allow client to legally make mDNS API calls from the callback
	newRR->RecordCallback(m, newRR, err);
	m->mDNS_reentrancy--; // Decrement to block mDNS API calls again
	// NOTE: not safe to touch any client structures here
	}

mDNSlocal void SendServiceRegistration(mDNS *m, ServiceRecordSet *srs)
	{
	DNSMessage msg;
	mDNSu8 *ptr = msg.data;
	mDNSu8 *end = (mDNSu8 *)&msg + sizeof(DNSMessage);
	uDNS_GlobalInfo *u = &m->uDNS_info;
	mDNSOpaque16 id;
	uDNS_AuthInfo *authInfo;
	uDNS_RegInfo *rInfo = &srs->uDNS_info;
	mStatus err = mStatus_UnknownErr;
	mDNSIPPort privport;
	NATTraversalInfo *nat = srs->uDNS_info.NATinfo;
	mDNSBool mapped = mDNSfalse;
	domainname *targetptr;
	const domainname *newtarget;
	AuthRecord *srv = &srs->RR_SRV;
	
	id = newMessageID(u);
	InitializeDNSMessage(&msg.h, id, UpdateReqFlags);

	// setup resource records
	SetNewRData(&srs->RR_PTR.resrec, mDNSNULL, 0);	
	SetNewRData(&srs->RR_TXT.resrec, mDNSNULL, 0);
	
	// replace port w/ NAT mapping if necessary
 	if (nat && nat->PublicPort.NotAnInteger &&
		(nat->state == NATState_Established || nat->state == NATState_Refresh || nat->state == NATState_Legacy))
		{
		privport = srv->resrec.rdata->u.srv.port;
		srv->resrec.rdata->u.srv.port = nat->PublicPort;
		mapped = mDNStrue;
		}
	
	// construct update packet
    // set zone
	ptr = putZone(&msg, ptr, end, &rInfo->zone, mDNSOpaque16fromIntVal(srv->resrec.rrclass));
	if (!ptr) goto error;
	
	if (srs->uDNS_info.TestForSelfConflict)
		{
		// update w/ prereq that records already exist to make sure previous registration was ours
		if (!(ptr = PutResourceRecordTTL(&msg, ptr, &msg.h.mDNS_numPrereqs, &srs->RR_PTR.resrec, 0))) goto error;		
		if (!(ptr = PutResourceRecordTTL(&msg, ptr, &msg.h.mDNS_numPrereqs, &srs->RR_TXT.resrec, 0))) goto error;
		if (!(ptr = PutResourceRecordTTL(&msg, ptr, &msg.h.mDNS_numPrereqs, &srs->RR_TXT.resrec, 0))) goto error;
		}
	
	else if (srs->uDNS_info.state != regState_Refresh)
		{
		// use SRV name for prereq
		ptr = putPrereqNameNotInUse(&srv->resrec.name, &msg, ptr, end);
		if (!ptr) goto error;
		}
	
	//!!!KRS  Need to do bounds checking and use TCP if it won't fit!!!
	if (!(ptr = PutResourceRecord(&msg, ptr, &msg.h.mDNS_numUpdates, &srs->RR_PTR.resrec))) goto error;
	if (!(ptr = PutResourceRecord(&msg, ptr, &msg.h.mDNS_numUpdates, &srs->RR_TXT.resrec))) goto error;

	newtarget = GetServiceTarget(u, srv);
	if (!newtarget)
		{
		debugf("Couldn't get target for service %##s", srv->resrec.name.c);
		rInfo->state = regState_NoTarget;		
		return;
		}	
	targetptr = GetRRDomainNameTarget(&srv->resrec);
	if (!targetptr)
		{
		// this should never happen
		LogMsg("Couldn't get target location for srv %##s", srv->resrec.name.c);
		goto error;
		}	
	if (rInfo->OrigTarget.c[0] && !SameDomainName(&rInfo->OrigTarget, newtarget))
		{
		// if we're changing targets, delete old target
		AssignDomainName(*targetptr, rInfo->OrigTarget);
		SetNewRData(&srv->resrec, mDNSNULL, 0);
		ptr = putDeletionRecord(&msg, ptr, &srv->resrec);
		if (!ptr) goto error;		
		srs->uDNS_info.TargetChangeDeferred = mDNSfalse;
		}
	if (!SameDomainName(targetptr, newtarget))
		{
		AssignDomainName(*targetptr, *newtarget);
		SetNewRData(&srv->resrec, mDNSNULL, 0);
		}
	ptr = PutResourceRecord(&msg, ptr, &msg.h.mDNS_numUpdates, &srv->resrec);
	if (!ptr) goto error;
	// !!!KRS do subtypes/extras etc.

	if (srs->uDNS_info.lease)
		ptr = putUpdateLease(&msg, ptr, kLLQ_DefLease);
	   
	srs->uDNS_info.expire = -1;

	authInfo = GetAuthInfoForZone(u, &rInfo->zone);
	if (authInfo)
		{
		err = mDNSSendSignedDNSMessage(m, &msg, ptr, 0, &rInfo->ns, rInfo->port, authInfo);
		if (err) { LogMsg("ERROR: SendServiceRegistration - mDNSSendSignedDNSMessage - %ld", err); goto error; }
		}
	else
		{
		err = mDNSSendDNSMessage(m, &msg, ptr, 0, &rInfo->ns, rInfo->port);
		if (err) { LogMsg("ERROR: SendServiceRegistration - mDNSSendDNSMessage - %ld", err); goto error; }
		}
	if (rInfo->state != regState_Refresh)
		rInfo->state = regState_Pending;

	SetRecordRetry(m, &srs->RR_SRV);
	rInfo->id.NotAnInteger = id.NotAnInteger;
	if (mapped) srv->resrec.rdata->u.srv.port = privport;
	return;

error:
	if (mapped) srv->resrec.rdata->u.srv.port = privport;	
	unlinkSRS(u, srs);
	rInfo->state = regState_Unregistered;
	m->mDNS_reentrancy++; // Increment to allow client to legally make mDNS API calls from the callback
	srs->ServiceCallback(m, srs, err);
	m->mDNS_reentrancy--; // Decrement to block mDNS API calls again
	//!!!KRS will mem still be free'd on error?
	// NOTE: not safe to touch any client structures here	
	}

mDNSlocal void serviceRegistrationCallback(mStatus err, mDNS *const m, void *srsPtr, const AsyncOpResult *result)
	{
	ServiceRecordSet *srs = (ServiceRecordSet *)srsPtr;
	const zoneData_t *zoneData = mDNSNULL;
	uDNS_GlobalInfo *u = &m->uDNS_info;
	
	if (err) goto error;
	if (!result) { LogMsg("ERROR: serviceRegistrationCallback invoked with NULL result and no error");  goto error; }
	else zoneData = &result->zoneData;
	
	if (result->type != zoneDataResult)
		{
		LogMsg("ERROR: buildUpdatePacket passed incorrect result type %d", result->type);
		goto error;
		}

	if (srs->uDNS_info.state == regState_Cancelled)
		{
		// client cancelled registration while fetching zone data
		srs->uDNS_info.state = regState_Unregistered;
		unlinkSRS(u, srs);
		m->mDNS_reentrancy++; // Increment to allow client to legally make mDNS API calls from the callback
		srs->ServiceCallback(m, srs, mStatus_MemFree);
		m->mDNS_reentrancy--; // Decrement to block mDNS API calls again
		return;
		}
	
	if (srs->RR_SRV.resrec.rrclass != zoneData->zoneClass)
		{
		LogMsg("Service %##s - class does not match zone", srs->RR_SRV.resrec.name.c);
		goto error;
		}
	// cache zone data
	AssignDomainName(srs->uDNS_info.zone, zoneData->zoneName);
    srs->uDNS_info.ns.type = mDNSAddrType_IPv4;
	srs->uDNS_info.ns = zoneData->primaryAddr;
	if (zoneData->updatePort.NotAnInteger) srs->uDNS_info.port = zoneData->updatePort;		
	else
		{
		debugf("Update port not advertised via SRV - guessing port 53, no lease option");
		srs->uDNS_info.port = UnicastDNSPort;
		srs->uDNS_info.lease = mDNSfalse;
		}
	SendServiceRegistration(m, srs);
	return;
		
error:
	unlinkSRS(u, srs);
	srs->uDNS_info.state = regState_Unregistered;
	m->mDNS_reentrancy++; // Increment to allow client to legally make mDNS API calls from the callback
	srs->ServiceCallback(m, srs, err);
	m->mDNS_reentrancy--; // Decrement to block mDNS API calls again
	//!!!KRS will mem still be free'd on error?
	// NOTE: not safe to touch any client structures here
	}

mDNSexport mStatus uDNS_RegisterRecord(mDNS *const m, AuthRecord *const rr)
	{
	domainname *target = GetRRDomainNameTarget(&rr->resrec);
	
	if (rr->uDNS_info.state == regState_FetchingZoneData ||
		rr->uDNS_info.state == regState_Pending ||
		rr->uDNS_info.state ==  regState_Registered)
		{
		LogMsg("Requested double-registration of physical record %##s type %d",
			   rr->resrec.name.c, rr->resrec.rrtype);
		return mStatus_AlreadyRegistered;
		}
	
	rr->resrec.rdlength   = GetRDLength(&rr->resrec, mDNSfalse);
	rr->resrec.rdestimate = GetRDLength(&rr->resrec, mDNStrue);

	if (!ValidateDomainName(&rr->resrec.name))
		{
		LogMsg("Attempt to register record with invalid name: %s", GetRRDisplayString(m, rr));
		return mStatus_Invalid;
		}

	// Don't do this until *after* we've set rr->resrec.rdlength
	if (!ValidateRData(rr->resrec.rrtype, rr->resrec.rdlength, rr->resrec.rdata))
		{ LogMsg("Attempt to register record with invalid rdata: %s", GetRRDisplayString(m, rr));
		return mStatus_Invalid;
		}

	rr->resrec.namehash   = DomainNameHashValue(&rr->resrec.name);
	rr->resrec.rdatahash  = RDataHashValue(rr->resrec.rdlength, &rr->resrec.rdata->u);
	rr->resrec.rdnamehash = target ? DomainNameHashValue(target) : 0;

	rr->uDNS_info.state = regState_FetchingZoneData;
	rr->next = m->uDNS_info.RecordRegistrations;
	m->uDNS_info.RecordRegistrations = rr;

	rr->uDNS_info.lease = mDNStrue;
	return startGetZoneData(&rr->resrec.name, m, mDNStrue, mDNSfalse, RecordRegistrationCallback, rr);
	}

mDNSexport mStatus uDNS_DeregisterRecord(mDNS *const m, AuthRecord *const rr)
	{
	uDNS_GlobalInfo *u = &m->uDNS_info;
	DNSMessage msg;
	mDNSu8 *ptr = msg.data;
	mDNSu8 *end = (mDNSu8 *)&msg + sizeof(DNSMessage);
	mStatus err;
	uDNS_AuthInfo *authInfo;
	NATTraversalInfo *n = rr->uDNS_info.NATinfo;
 
	switch (rr->uDNS_info.state)
		{
		case regState_NATMap:
            // we're in the middle of a NAT traversal operation
			if (!n) LogMsg("uDNS_DeregisterRecord: no NAT info context");		
			else FreeNATInfo(m, n); // cause response to outstanding request to be ignored.
			                        // Note: normally here we're trying to determine our public address, 
			                        //in which case there is not state to be torn down.  For simplicity, 
			                        //we allow other operations to expire.    
            rr->uDNS_info.NATinfo = mDNSNULL;
			unlinkAR(&u->RecordRegistrations, rr);
			rr->uDNS_info.state = regState_Unregistered;
			m->mDNS_reentrancy++; // Increment to allow client to legally make mDNS API calls from the callback
			rr->RecordCallback(m, rr, mStatus_MemFree);
			m->mDNS_reentrancy--; // Decrement to block mDNS API calls again
			return mStatus_NoError;
		case regState_FetchingZoneData:
			rr->uDNS_info.state = regState_Cancelled;
			return mStatus_NoError;
		case regState_Pending:
			rr->uDNS_info.state = regState_DeregDeferred;
			LogMsg("Deferring deregistration of record %##s until registration completes", rr->resrec.name.c);
			return mStatus_NoError;
		case regState_Registered:			
		case regState_DeregPending:
			break;
		case regState_Cancelled:
			LogMsg("Double deregistration of record %##s type %d",
				   rr->resrec.name.c, rr->resrec.rrtype);
			return mStatus_UnknownErr;
		case regState_Unregistered:
			LogMsg("Requested deregistration of unregistered record %##s type %d",
				   rr->resrec.name.c, rr->resrec.rrtype);
			return mStatus_UnknownErr;
		default:
			LogMsg("ERROR: uDNS_DeregisterRecord called for record %##s type %d with unknown state %d", 
				   rr->resrec.name.c, rr->resrec.rrtype, rr->uDNS_info.state);
			return mStatus_UnknownErr;
		}
		
	InitializeDNSMessage(&msg.h, rr->uDNS_info.id, UpdateReqFlags);
	
	// put zone
	ptr = putZone(&msg, ptr, end, &rr->uDNS_info.zone, mDNSOpaque16fromIntVal(rr->resrec.rrclass));
	if (!ptr) goto error;

	if (!(ptr = putDeletionRecord(&msg, ptr, &rr->resrec))) goto error;

	authInfo = GetAuthInfoForZone(u, &rr->uDNS_info.zone);
	if (authInfo)
		{
		err = mDNSSendSignedDNSMessage(m, &msg, ptr, 0, &rr->uDNS_info.ns, rr->uDNS_info.port, authInfo);
		if (err) LogMsg("ERROR: uDNS_DeregiserRecord - mDNSSendSignedDNSMessage - %ld", err);
		}
	else
		{
		err = mDNSSendDNSMessage(m, &msg, ptr, 0, &rr->uDNS_info.ns, rr->uDNS_info.port);
		if (err) LogMsg("ERROR: uDNS_DeregisterRecord - mDNSSendDNSMessage - %ld", err); 
		}

	SetRecordRetry(m, rr);
	rr->uDNS_info.state = regState_DeregPending;

	if (n) FreeNATInfo(m, n);
	rr->uDNS_info.NATinfo = mDNSNULL;
	return mStatus_NoError;

	error:
	//!!!KRS set timer, retry
	if (rr->uDNS_info.state != regState_Unregistered)
		{
		unlinkAR(&u->RecordRegistrations, rr);
		rr->uDNS_info.state = regState_Unregistered;
		}
	return mStatus_UnknownErr;
	}

// !!!KRS look into simpler ways to do this (create a record registration, or dereg and rereg the service, etc)
mDNSlocal void UpdateServiceTargets(mDNS *const m)
	{
	uDNS_GlobalInfo *u = &m->uDNS_info;
	ServiceRecordSet *srs;
	uDNS_RegInfo *info;
	domainname *target;
	
	for (srs = u->ServiceRegistrations; srs; srs = srs->next)
		{
		info = &srs->uDNS_info;
		target = GetRRDomainNameTarget(&srs->RR_SRV.resrec);
		if (target && target->c[0]) AssignDomainName(info->OrigTarget, *target); // store old target for deletion by SendRecordRegistration
		if (info->state == regState_Registered)
			{
			info->state = regState_Refresh;
			SendServiceRegistration(m, srs);  // this will automatically grab the new target
			}
		else srs->uDNS_info.TargetChangeDeferred = mDNStrue; // trigger target change upon completion of pending operation
		}		
	}

// register a service already in list with initialized state
mDNSlocal mStatus RegisterService(mDNS *m, ServiceRecordSet *srs)
	{
	uDNS_RegInfo *info = &srs->uDNS_info;	

	if (!GetServiceTarget(&m->uDNS_info, &srs->RR_SRV))
		{
		// defer registration until we get a target
		LogMsg("No service target for %##s - deferring until registration of address record", srs->RR_SRV.resrec.name.c);
		info->state = regState_NoTarget;
		return mStatus_NoError;
		}
	
	srs->RR_SRV.resrec.rroriginalttl = 3;
	srs->RR_TXT.resrec.rroriginalttl = 3;
	srs->RR_PTR.resrec.rroriginalttl = 3;
	
	info->lease = mDNStrue;

	if (MapServicePort(m))
		{
		// !!!KRS if interface is already in NATState_Legacy, don't try NAT-PMP
		info->state = regState_NATMap;
		StartNATPortMap(m, srs);
		return mStatus_NoError;
		}
	
	info->state = regState_FetchingZoneData;
	return startGetZoneData(&srs->RR_SRV.resrec.name, m, mDNStrue, mDNSfalse, serviceRegistrationCallback, srs);
	}	
	
mDNSexport mStatus uDNS_RegisterService(mDNS *const m, ServiceRecordSet *srs)
	{
	// Note: ServiceRegistrations list is in the order they were created; important for in-order event delivery
	ServiceRecordSet **p = &m->uDNS_info.ServiceRegistrations;
	while (*p && *p != srs) p=&(*p)->next;
	if (*p) { LogMsg("uDNS_RegisterService: %p %##s already in list", srs, srs->RR_SRV.resrec.name.c); return(mStatus_AlreadyRegistered); }
	ubzero(&srs->uDNS_info, sizeof(uDNS_RegInfo));
	srs->uDNS_info.state = regState_Unregistered;
	*p = srs;
	srs->next = mDNSNULL;
	return RegisterService(m, srs);
	}	

mDNSlocal void SendServiceDeregistration(mDNS *m, ServiceRecordSet *srs)
	{
	uDNS_RegInfo *info = &srs->uDNS_info;
	uDNS_GlobalInfo *u = &m->uDNS_info;
	NATTraversalInfo *nat = srs->uDNS_info.NATinfo;
	uDNS_AuthInfo *authInfo = mDNSNULL;
	DNSMessage msg;
	mDNSOpaque16 id;
	mDNSu8 *ptr = msg.data;
	mDNSu8 *end = (mDNSu8 *)&msg + sizeof(DNSMessage);
	mStatus err = mStatus_UnknownErr;
	mDNSIPPort privport;
	mDNSBool mapped = mDNSfalse;

	if (info->state != regState_DeregPending) { LogMsg("SendServiceDeregistration: bad state %d", info->state); goto error; }
	
	id = newMessageID(u);
	InitializeDNSMessage(&msg.h, id, UpdateReqFlags);
	
    // put zone
	ptr = putZone(&msg, ptr, end, &info->zone, mDNSOpaque16fromIntVal(srs->RR_SRV.resrec.rrclass));
	if (!ptr) { LogMsg("ERROR: SendServiceDeregistration - putZone"); goto error; }
	
    // prereq: record must exist (put record in prereq section w/ TTL 0)
	//!!!KRS this just wastes space in packet - should be no-op if record doesn't exist
	//ptr = PutResourceRecordTTL(&msg, ptr, &msg.h.mDNS_numPrereqs, &srs->RR_SRV.resrec, 0);
	//if (!ptr) { LogMsg("ERROR: SendServiceDeregistration - PutResourceRecordTTL"); goto error; }

	// replace port w/ NAT mapping if necessary
	if (nat && nat->PublicPort.NotAnInteger)
		{
		privport = srs->RR_SRV.resrec.rdata->u.srv.port;
		srs->RR_SRV.resrec.rdata->u.srv.port = nat->PublicPort;
		mapped = mDNStrue;
		}

	ptr = putDeletionRecord(&msg, ptr, &srs->RR_SRV.resrec);
	if (mapped) srs->RR_SRV.resrec.rdata->u.srv.port = privport;
	if (!ptr) goto error;
	
	if (!(ptr = putDeletionRecord(&msg, ptr, &srs->RR_TXT.resrec))) goto error;
	if (!(ptr = putDeletionRecord(&msg, ptr, &srs->RR_PTR.resrec))) goto error;
	//if (!(ptr = putDeletionRecord(&msg, ptr, &srs->RR_ADV.resrec))) goto error;
	//!!!KRS  need to handle extras/subtypes etc
	
	authInfo = GetAuthInfoForZone(u, &info->zone);
	if (authInfo)
		{
		err = mDNSSendSignedDNSMessage(m, &msg, ptr, 0, &info->ns, info->port, authInfo);
		if (err) { LogMsg("ERROR: uDNS_DeregiserService - mDNSSendSignedDNSMessage - %ld", err); goto error; }
		}
	else
		{
		err = mDNSSendDNSMessage(m, &msg, ptr, 0, &info->ns, info->port);
		if (err) { LogMsg("ERROR: SendServiceDeregistration - mDNSSendDNSMessage - %ld", err); goto error; }
		}

	SetRecordRetry(m, &srs->RR_SRV);
    info->id.NotAnInteger = id.NotAnInteger;
	return;
	
	error:
	unlinkSRS(u, srs);
	info->state = regState_Unregistered;
	}

mDNSexport mStatus uDNS_DeregisterService(mDNS *const m, ServiceRecordSet *srs)
	{
	uDNS_GlobalInfo *u = &m->uDNS_info;
	NATTraversalInfo *nat = srs->uDNS_info.NATinfo;

	switch (srs->uDNS_info.state)
		{
		case regState_NATMap:
			// we're in the middle of nat mapping.  clear ptr from NAT info to RR, unlink and give memfree			
			if (!nat) LogMsg("uDNS_DeregisterRecord: no NAT info context");
			else
				{
				nat->reg.ServiceRegistration = mDNSNULL;
				FreeNATInfo(m, nat);                 // response to outstanding mapping request will be discardeda
				}
			unlinkSRS(u, srs);
			srs->uDNS_info.state = regState_Unregistered;
			m->mDNS_reentrancy++; // Increment to allow client to legally make mDNS API calls from the callback
			srs->ServiceCallback(m, srs, mStatus_MemFree);
			m->mDNS_reentrancy--; // Decrement to block mDNS API calls again
			return mStatus_NoError;		
		case regState_Unregistered:
			LogMsg("ERROR: uDNS_DeregisterService - service not registerd");
			return mStatus_UnknownErr;
		case regState_FetchingZoneData:
		case regState_Pending:
			// let the async op complete, then terminate
			srs->uDNS_info.state = regState_Cancelled;
			return mStatus_NoError;  // deliver memfree upon completion of async op
		case regState_DeregPending:
		case regState_DeregDeferred:
		case regState_Cancelled:
			LogMsg("uDNS_DeregisterService - deregistration in process");
			return mStatus_UnknownErr;
		case regState_NoTarget:
			unlinkSRS(u, srs);
			srs->uDNS_info.state = regState_Unregistered;
			m->mDNS_reentrancy++; // Increment to allow client to legally make mDNS API calls from the callback
			srs->ServiceCallback(m, srs, mStatus_MemFree);
			m->mDNS_reentrancy--; // Decrement to block mDNS API calls again
			return mStatus_NoError;
		}

	if (nat)
		{
		if (nat->state == NATState_Established)  // let other edge-case states expire for simplicity
			{
			// zero lease
			nat->request[8]  = 0;
			nat->request[9]  = 0;
			nat->request[10] = 0 ;
			nat->request[11] = 0;
			nat->state = NATState_Request;
			SendNATMsg(nat, m); 
			}
#ifdef _LEGACY_NAT_TRAVERSAL_
		else if (nat->state == NATState_Legacy)
			{
			mStatus err = mStatus_NoError;
			mDNSBool tcp = DomainContainsLabelString(&srs->RR_PTR.resrec.name, "_tcp");
			err = LNT_UnmapPort(nat->PublicPort, tcp);
			if (err) LogMsg("Legacy NAT Traversal - unmap request failed with error %ld", err);
			}
#endif // _LEGACY_NAT_TRAVERSAL_
		}

	srs->uDNS_info.state = regState_DeregPending;
	SendServiceDeregistration(m, srs);
	return mStatus_NoError;
	}


// note that the RegInfo will be either for the record, or for the parent ServiceRecordSet
mDNSlocal void SendRecordUpdate(mDNS *m, AuthRecord *rr, uDNS_RegInfo *info)
	{
	DNSMessage msg;
	mDNSu8 *ptr = msg.data;
	mDNSu8 *end = (mDNSu8 *)&msg + sizeof(DNSMessage);
	uDNS_GlobalInfo *u = &m->uDNS_info;
	mDNSOpaque16 id;
	uDNS_AuthInfo *authInfo;
	mStatus err = mStatus_UnknownErr;
	
	rr->uDNS_info.UpdateQueued = mDNSfalse;  // if this was queued, clear flag
	id = newMessageID(u);
	InitializeDNSMessage(&msg.h, id, UpdateReqFlags);
	info->id.NotAnInteger = id.NotAnInteger;
	
    // set zone
	ptr = putZone(&msg, ptr, end, &info->zone, mDNSOpaque16fromIntVal(rr->resrec.rrclass));
	if (!ptr) goto error;
	        
	// delete the original record
	ptr = putDeletionRecord(&msg, ptr, &rr->resrec);
	if (!ptr) goto error;

	// change the rdata, add the new record
	SwapRData(m, rr, mDNSfalse);
	ptr = PutResourceRecord(&msg, ptr, &msg.h.mDNS_numUpdates, &rr->resrec);
	SwapRData(m, rr, mDNSfalse);  // swap rdata back to original in case we need to retransmit
	if (!ptr) goto error;         // (rdata gets changed permanently on success)

	if (info->lease) ptr = putUpdateLease(&msg, ptr, kLLQ_DefLease);
	
	// don't report send errors - retransmission will occurr if necessary
	authInfo = GetAuthInfoForZone(u, &info->zone);
	if (authInfo)
		{
		err = mDNSSendSignedDNSMessage(m, &msg, ptr, 0, &info->ns, info->port, authInfo);
		if (err) LogMsg("ERROR: sendRecordRegistration - mDNSSendSignedDNSMessage - %ld", err);
		}
	else
		{
		err = mDNSSendDNSMessage(m, &msg, ptr, 0, &info->ns, info->port);
		if (err) LogMsg("ERROR: sendRecordRegistration - mDNSSendDNSMessage - %ld", err);
		}

	//!!! Update this when we implement retransmission for services	
	SetRecordRetry(m, rr);
	
	rr->uDNS_info.state = regState_UpdatePending;
	if (&rr->uDNS_info != info) info->state = regState_UpdatePending; // set parent SRS
	return;

error:
	LogMsg("ERROR: SendRecordUpdate.  Error formatting update message.");
	info ->state = regState_Registered;  
	}

mDNSexport mStatus uDNS_UpdateRecord(mDNS *m, AuthRecord *rr)
	{
	uDNS_GlobalInfo *u = &m->uDNS_info;
	ServiceRecordSet *sptr, *parent = mDNSNULL;
	ExtraResourceRecord *eptr;
	AuthRecord *rptr;
	uDNS_RegInfo *info = mDNSNULL;
	
	// find the record in registered service list
	for (sptr = u->ServiceRegistrations; sptr; sptr = sptr->next)
		{
		if (&sptr->RR_TXT == rr) { info = &sptr->uDNS_info; parent = sptr; break; }
		for (eptr = sptr->Extras; eptr; eptr = eptr->next)
			if (&eptr->r == rr)  { info = &sptr->uDNS_info; parent = sptr; break; }
		}

	if (!parent)
		{
		// record not part of a service - check individual record registrations
		for (rptr = u->RecordRegistrations; rptr; rptr = rptr->next)
			if (rptr == rr) { info = &rr->uDNS_info; break; }
		}

	if (!info) goto unreg_error;

	// uDNS-private pointers so that mDNS.c layer doesn't nuke rdata of an in-flight update
	rr->uDNS_info.UpdateRData = rr->NewRData;
	rr->uDNS_info.UpdateRDLen = rr->newrdlength;
	rr->uDNS_info.UpdateRDCallback = rr->UpdateCallback;
	rr->NewRData = mDNSNULL;

	
	switch(info->state)
		{
		case regState_DeregPending:
		case regState_DeregDeferred:
		case regState_Cancelled:
		case regState_Unregistered:
			// not actively registered
			goto unreg_error;
			
		case regState_FetchingZoneData:
		case regState_NATMap:
			// change rdata directly since it hasn't been sent yet			
			SwapRData(m, rr, mDNStrue);
			return mStatus_NoError;
			
		case regState_Pending:
		case regState_Refresh:
		case regState_UpdatePending:
			// registration in-flight.  mark for update after service registration completes
			rr->uDNS_info.UpdateQueued = mDNStrue;  // note that we mark the record's Queued flag, not its parent's
			return mStatus_NoError;
			
		case regState_Registered:
			SendRecordUpdate(m, rr, info);
			return mStatus_NoError;
		}

	unreg_error:
	LogMsg("Requested update of record %##s type %d, part of service not currently registered",
		   rr->resrec.name.c, rr->resrec.rrtype);
	return mStatus_Invalid;
	}


// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Periodic Execution Routines
#endif


mDNSlocal mDNSs32 CheckNATMappings(mDNS *m, mDNSs32 timenow)
	{
	NATTraversalInfo *ptr, *cur;
	mDNSs32 nextevent;

	ptr = m->uDNS_info.NATTraversals;
	nextevent = timenow + MIN_UCAST_PERIODIC_EXEC;
	
	while (ptr)
		{
		cur = ptr;
		ptr = ptr->next;
		if (cur->retry - timenow < 0)
			{
			if (cur->state == NATState_Established) RefreshNATMapping(cur, m);
			else if (cur->state == NATState_Request || cur->state == NATState_Refresh)
				{
				if (cur->ntries >= NATMAP_MAX_TRIES) cur->ReceiveResponse(cur, m, mDNSNULL, 0); // may invalidate "cur"
				else SendNATMsg(cur, m);
				}
			}
		else if (cur->retry - nextevent < 0) nextevent = cur->retry;
		}
	return nextevent;
	}

mDNSlocal mDNSs32 CheckQueries(mDNS *m, mDNSs32 timenow)
	{
	DNSQuestion *q;
	uDNS_GlobalInfo *u = &m->uDNS_info;
	LLQ_Info *llq;
	mDNSs32 sendtime;
	mDNSs32 nextevent = timenow + MIN_UCAST_PERIODIC_EXEC;
	DNSMessage msg;
	mStatus err;
	mDNSu8 *end;
	const mDNSAddr *server = getInitializedDNS(&m->uDNS_info);

	if (!server) { debugf("uDNS_Execute - no DNS server"); return nextevent; }	

	for (q = u->ActiveQueries; q; q = q->next)
		{
		llq = q->uDNS_info.llq;
		if (q->LongLived && llq->state != LLQ_Poll)
			{
			if (llq->state >= LLQ_InitialRequest && llq->state < LLQ_Suspended)
				{
				if (llq->retry - timenow < 0)				
					{
					// sanity check to avoid packet flood bugs
					if (!llq->retry) 
						LogMsg("ERROR: retry timer not set for LLQ %##s in state %d", q->qname.c, llq->state);
					else if (llq->state == LLQ_Established || llq->state == LLQ_Refresh)
						sendLLQRefresh(m, q, llq->origLease);
					else if (llq->state == LLQ_InitialRequest) 
						startLLQHandshake(m, llq);
					else if (llq->state == LLQ_SecondaryRequest)
						sendChallengeResponse(m, q, mDNSNULL);
					else if (llq->state == LLQ_Retry) 
						{ llq->ntries = 0; startLLQHandshake(m, llq); }
					}
				else if (llq->retry - nextevent < 0) nextevent = llq->retry;
				}
			}
		else
			{
			sendtime = q->LastQTime + q->ThisQInterval;
			if (sendtime - timenow < 0)
				{
				err = constructQueryMsg(&msg, &end, q);
				if (err)
					{
					LogMsg("Error: uDNS_Idle - constructQueryMsg.  Skipping question %##s",
						   q->qname.c);
					continue;
					}
				err = mDNSSendDNSMessage(m, &msg, end, q->InterfaceID, server, UnicastDNSPort);
				if (err) { debugf("ERROR: uDNS_idle - mDNSSendDNSMessage - %ld", err); } // surpress syslog messages if we have no network
				q->LastQTime = timenow;
				if (q->ThisQInterval < MAX_UCAST_POLL_INTERVAL) q->ThisQInterval = q->ThisQInterval * 2;
				}
			else if (sendtime - nextevent < 0) nextevent = sendtime;
			}
		}
	return nextevent;
	}

mDNSlocal mDNSs32 CheckRecordRegistrations(mDNS *m, mDNSs32 timenow)
	{
	AuthRecord *rr;
	uDNS_RegInfo *rInfo;
	uDNS_GlobalInfo *u = &m->uDNS_info;
 	mDNSs32 nextevent = timenow + MIN_UCAST_PERIODIC_EXEC;
	
	//!!!KRS list should be pre-sorted by expiration
	for (rr = u->RecordRegistrations; rr; rr = rr->next)
		{
		rInfo = &rr->uDNS_info;		
		if (rInfo->state == regState_Pending || rInfo->state == regState_DeregPending || rInfo->state == regState_UpdatePending || rInfo->state == regState_DeregDeferred || rInfo->state == regState_Refresh)
			{
			if (rr->LastAPTime + rr->ThisAPInterval - timenow < 0)
				{
#if MDNS_DEBUGMSGS				
				char *op = "(unknown operation)";
				if (rInfo->state == regState_Pending) op = "registration";
				else if (rInfo->state == regState_DeregPending) op = "deregistration";
				else if (rInfo->state == regState_Refresh) op = "refresh";
				debugf("Retransmit record %s %##s", op, rr->resrec.name.c);
#endif
				//LogMsg("Retransmit record %##s", rr->resrec.name.c);
				if      (rInfo->state == regState_DeregPending)   uDNS_DeregisterRecord(m, rr);
				else if (rInfo->state == regState_UpdatePending)  SendRecordUpdate(m, rr, rInfo);
				else                                              sendRecordRegistration(m, rr);
				}
			if (rr->LastAPTime + rr->ThisAPInterval - nextevent < 0) nextevent = rr->LastAPTime + rr->ThisAPInterval;
			}
		if (rInfo->lease && rInfo->state == regState_Registered && rInfo->expire > 0)
		    {		    
		    if (rInfo->expire - timenow < 0)
		        {
		        debugf("refreshing record %##s", rr->resrec.name.c);
		        rInfo->state = regState_Refresh;
		        sendRecordRegistration(m, rr);
		        }
		    if (rInfo->expire - nextevent < 0) nextevent = rInfo->expire;
		    }		
		}
	return nextevent;
	}

//!!!KRS need to implement service retransmissions!
mDNSlocal mDNSs32 CheckServiceRegistrations(mDNS *m, mDNSs32 timenow)
	{
	ServiceRecordSet *s = m->uDNS_info.ServiceRegistrations;
	uDNS_RegInfo *rInfo;
	mDNSs32 nextevent = timenow + MIN_UCAST_PERIODIC_EXEC;
	
	// Note: ServiceRegistrations list is in the order they were created; important for in-order event delivery
	while (s)
		{
		ServiceRecordSet *srs = s;
		// NOTE: Must advance s here -- SendServiceDeregistration may delete the object we're looking at,
		// and then if we tried to do srs = srs->next at the end we'd be referencing a dead object
		s = s->next;
		
		rInfo = &srs->uDNS_info;	
		if (rInfo->state == regState_Pending || rInfo->state == regState_DeregPending || rInfo->state == regState_DeregDeferred || rInfo->state == regState_Refresh)
			{
			if (srs->RR_SRV.LastAPTime + srs->RR_SRV.ThisAPInterval - timenow < 0)
				{
#if MDNS_DEBUGMSGS				
				char *op = "unknown";
				if (rInfo->state == regState_Pending) op = "registration";
				else if (rInfo->state == regState_DeregPending) op = "deregistration";
				else if (rInfo->state == regState_Refresh) op = "refresh";
				debugf("Retransmit service %s %##s", op, srs->RR_SRV.resrec.name.c);
#endif
				if (rInfo->state == regState_DeregPending) { SendServiceDeregistration(m, srs); continue; }
				else                                         SendServiceRegistration  (m, srs);
				}
			if (nextevent - srs->RR_SRV.LastAPTime + srs->RR_SRV.ThisAPInterval > 0)
				nextevent = srs->RR_SRV.LastAPTime + srs->RR_SRV.ThisAPInterval;
			}			

		if (rInfo->lease && rInfo->state == regState_Registered && rInfo->expire > 0)
		    {		    
		    if (rInfo->expire - timenow < 0)
		        {	
			    debugf("refreshing service %##s", srs->RR_SRV.resrec.name.c);
			    rInfo->state = regState_Refresh;
			    SendServiceRegistration(m, srs);
		        }
		    if (rInfo->expire - nextevent < 0) nextevent = rInfo->expire;
		    }		
		}
	return nextevent;
	}

mDNSexport void uDNS_Execute(mDNS *const m)
	{
	uDNS_GlobalInfo *u = &m->uDNS_info;
	mDNSs32 nexte, timenow = mDNSPlatformTimeNow(m);	

	u->nextevent = timenow + MIN_UCAST_PERIODIC_EXEC;
	if (!getInitializedDNS(&m->uDNS_info)) return; 	// return immediately if we don't have a DNS server
		
	
	nexte = CheckNATMappings(m, timenow);
	if (nexte - u->nextevent < 0) u->nextevent = nexte;

	nexte = CheckQueries(m, timenow);
	if (nexte - u->nextevent < 0) u->nextevent = nexte;

	nexte = CheckRecordRegistrations(m, timenow);
	if (nexte - u->nextevent < 0) u->nextevent = nexte;

	nexte = CheckServiceRegistrations(m, timenow);
	if (nexte - u->nextevent < 0) u->nextevent = nexte;
	
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Startup, Shutdown, and Sleep
#endif



mDNSlocal void SuspendLLQs(mDNS *m)
	{
	DNSQuestion *q;
	LLQ_Info *llq;
	for (q = m->uDNS_info.ActiveQueries; q; q = q->next)
		{
		llq = q->uDNS_info.llq;
		if (q->LongLived && llq && llq->state < LLQ_Suspended)
			{
			if (llq->state == LLQ_Established || llq->state == LLQ_Refresh)
				sendLLQRefresh(m, q, 0);
			if (llq->state == LLQ_GetZoneInfo) llq->state = LLQ_SuspendDeferred;  // suspend once we're done getting zone info
			else llq->state = LLQ_Suspended;
			}
		}
	}

mDNSlocal void RestartLLQs(mDNS *m)
	{
	uDNS_GlobalInfo *u = &m->uDNS_info;
	DNSQuestion *q;
	LLQ_Info *llqInfo;
	
	u->CurrentQuery = u->ActiveQueries;
	while (u->CurrentQuery)
		{
		q = u->CurrentQuery;
		u->CurrentQuery = u->CurrentQuery->next;
		llqInfo = q->uDNS_info.llq;
		if (q->LongLived && llqInfo)
			{
			if (llqInfo->state == LLQ_Suspended)		
				{ llqInfo->ntries = 0; llqInfo->deriveRemovesOnResume = mDNStrue; startLLQHandshake(m, llqInfo); }
			else if (llqInfo->state == LLQ_SuspendDeferred)
				llqInfo->state = LLQ_GetZoneInfo; // we never finished getting zone data - proceed as usual
            }
		}
	}

//!!!KRS implement me for real
mDNSlocal void SuspendRecordRegistrations(mDNS *m)
	{
	uDNS_GlobalInfo *u = &m->uDNS_info;
	AuthRecord *rr;

	for (rr = u->RecordRegistrations; rr; rr = rr->next)
		{
		uDNS_DeregisterRecord(m, rr);
		}
	}


mDNSexport void uDNS_Init(mDNS *const m)
	{
	mDNSPlatformMemZero(&m->uDNS_info, sizeof(uDNS_GlobalInfo));
	m->uDNS_info.nextevent = m->timenow_last + 0x78000000;
	}

mDNSexport void uDNS_Close(mDNS *m)
	{
	SuspendLLQs(m);
	SuspendRecordRegistrations(m);   
	}

mDNSexport void uDNS_Sleep(mDNS *m)
	{
	SuspendLLQs(m);
	}

mDNSexport void uDNS_Wake(mDNS *m)
	{
	RestartLLQs(m);
	}
