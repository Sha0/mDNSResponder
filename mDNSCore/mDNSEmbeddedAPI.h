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
 */
/*
    File:       mDNSEmbeddedAPI.h

    Contains:   Programming interface to the mDNS core, for use by embedded clients in a single address space

    Written by: Stuart Cheshire

    Version:    mDNS Core, September 2002

    Copyright:  Copyright (c) 2002-2004 by Apple Computer, Inc., All Rights Reserved.

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

    Change History (most recent first):

$Log: mDNSEmbeddedAPI.h,v $
Revision 1.53  2003/05/23 02:15:37  cheshire
Fixed misleading use of the term "duplicate suppression" where it should have
said "known answer suppression". (Duplicate answer suppression is something
different, and duplicate question suppression is yet another thing, so the use
of the completely vague term "duplicate suppression" was particularly bad.)

Revision 1.52  2003/05/22 02:29:22  cheshire
<rdar://problem/2984918> SendQueries needs to handle multihoming better
Complete rewrite of SendQueries. Works much better now :-)

Revision 1.51  2003/05/21 20:14:55  cheshire
Fix comments and warnings

Revision 1.50  2003/05/14 07:08:36  cheshire
<rdar://problem/3159272> mDNSResponder should be smarter about reconfigurations
Previously, when there was any network configuration change, mDNSResponder
would tear down the entire list of active interfaces and start again.
That was very disruptive, and caused the entire cache to be flushed,
and caused lots of extra network traffic. Now it only removes interfaces
that have really gone, and only adds new ones that weren't there before.

Revision 1.49  2003/05/07 01:49:36  cheshire
Remove "const" in ConstructServiceName prototype

Revision 1.48  2003/05/07 00:18:44  cheshire
Fix typo: "kDNSQClass_Mask" should be "kDNSClass_Mask"

Revision 1.47  2003/05/06 00:00:46  cheshire
<rdar://problem/3248914> Rationalize naming of domainname manipulation functions

Revision 1.46  2003/04/30 20:39:09  cheshire
Add comment

Revision 1.45  2003/04/29 00:40:50  cheshire
Fix compiler warnings

Revision 1.44  2003/04/26 02:41:56  cheshire
<rdar://problem/3241281> Change timenow from a local variable to a structure member

Revision 1.43  2003/04/25 01:45:56  cheshire
<rdar://problem/3240002> mDNS_RegisterNoSuchService needs to include a host name

Revision 1.42  2003/04/15 20:58:31  jgraessl

Bug #: 3229014
Added a hash to lookup records in the cache.

Revision 1.41  2003/04/15 18:09:13  jgraessl

Bug #: 3228892
Reviewed by: Stuart Cheshire
Added code to keep track of when the next cache item will expire so we can
call TidyRRCache only when necessary.

Revision 1.40  2003/03/29 01:55:19  cheshire
<rdar://problem/3212360> mDNSResponder sometimes suffers false self-conflicts when it sees its own packets
Solution: Major cleanup of packet timing and conflict handling rules

Revision 1.39  2003/03/27 03:30:55  cheshire
<rdar://problem/3210018> Name conflicts not handled properly, resulting in memory corruption, and eventual crash
Problem was that HostNameCallback() was calling mDNS_DeregisterInterface(), which is not safe in a callback
Fixes:
1. Make mDNS_DeregisterInterface() safe to call from a callback
2. Make HostNameCallback() use mDNS_DeadvertiseInterface() instead
   (it never really needed to deregister the interface at all)

Revision 1.38  2003/03/15 04:40:36  cheshire
Change type called "mDNSOpaqueID" to the more descriptive name "mDNSInterfaceID"

Revision 1.37  2003/03/14 21:34:11  cheshire
<rdar://problem/3176950> Can't setup and print to Lexmark PS printers via Airport Extreme
Increase size of cache rdata from 512 to 768

Revision 1.36  2003/03/05 03:38:35  cheshire
Bug #: 3185731 Bogus error message in console: died or deallocated, but no record of client can be found!
Fixed by leaving client in list after conflict, until client explicitly deallocates

Revision 1.35  2003/02/21 02:47:54  cheshire
Bug #: 3099194 mDNSResponder needs performance improvements
Several places in the code were calling CacheRRActive(), which searched the entire
question list every time, to see if this cache resource record answers any question.
Instead, we now have a field "CRActiveQuestion" in the resource record structure

Revision 1.34  2003/02/21 01:54:08  cheshire
Bug #: 3099194 mDNSResponder needs performance improvements
Switched to using new "mDNS_Execute" model (see "Implementer Notes.txt")

Revision 1.33  2003/02/20 06:48:32  cheshire
Bug #: 3169535 Xserve RAID needs to do interface-specific registrations
Reviewed by: Josh Graessley, Bob Bradley

Revision 1.32  2003/01/31 03:35:59  cheshire
Bug #: 3147097 mDNSResponder sometimes fails to find the correct results
When there were *two* active questions in the list, they were incorrectly
finding *each other* and *both* being marked as duplicates of another question

Revision 1.31  2003/01/29 02:46:37  cheshire
Fix for IPv6:
A physical interface is identified solely by its InterfaceID (not by IP and type).
On a given InterfaceID, mDNSCore may send both v4 and v6 multicasts.
In cases where the requested outbound protocol (v4 or v6) is not supported on
that InterfaceID, the platform support layer should simply discard that packet.

Revision 1.30  2003/01/29 01:47:08  cheshire
Rename 'Active' to 'CRActive' or 'InterfaceActive' for improved clarity

Revision 1.29  2003/01/28 05:23:43  cheshire
Bug #: 3147097 mDNSResponder sometimes fails to find the correct results
Add 'Active' flag for interfaces

Revision 1.28  2003/01/28 01:35:56  cheshire
Revise comment about ThisQInterval to reflect new semantics

Revision 1.27  2003/01/13 23:49:42  jgraessl
Merged changes for the following fixes in to top of tree:
3086540  computer name changes not handled properly
3124348  service name changes are not properly handled
3124352  announcements sent in pairs, failing chattiness test

Revision 1.26  2002/12/23 22:13:28  jgraessl

Reviewed by: Stuart Cheshire
Initial IPv6 support for mDNSResponder.

Revision 1.25  2002/09/21 20:44:49  zarzycki
Added APSL info

Revision 1.24  2002/09/19 23:47:35  cheshire
Added mDNS_RegisterNoSuchService() function for assertion of non-existence
of a particular named service

Revision 1.23  2002/09/19 21:25:34  cheshire
mDNS_sprintf() doesn't need to be in a separate file

Revision 1.22  2002/09/19 04:20:43  cheshire
Remove high-ascii characters that confuse some systems

Revision 1.21  2002/09/17 01:06:35  cheshire
Change mDNS_AdvertiseLocalAddresses to be a parameter to mDNS_Init()

Revision 1.20  2002/09/16 18:41:41  cheshire
Merge in license terms from Quinn's copy, in preparation for Darwin release

*/

#ifndef __mDNSClientAPI_h
#define __mDNSClientAPI_h

#include <stdarg.h>		// stdarg.h is required for for va_list support for the mDNS_vsprintf declaration
#include "mDNSDebug.h"

#ifdef	__cplusplus
	extern "C" {
#endif

// ***************************************************************************
// Function scope indicators

// If you see "mDNSlocal" before a function name, it means the function is not callable outside this file
#ifndef mDNSlocal
#define mDNSlocal static
#endif
// If you see "mDNSexport" before a symbol, it means the symbol is exported for use by clients
#ifndef mDNSexport
#define mDNSexport
#endif

// ***************************************************************************
#if 0
#pragma mark - DNS Resource Record class and type constants
#endif

typedef enum						// From RFC 1035
	{
	kDNSClass_IN          = 1,		// Internet
	kDNSClass_CS          = 2,		// CSNET
	kDNSClass_CH          = 3,		// CHAOS
	kDNSClass_HS          = 4,		// Hesiod
	kDNSClass_NONE        = 254,	// Used in DNS UPDATE [RFC 2136]
	kDNSQClass_ANY        = 255,	// Not a DNS class, but a DNS query class, meaning "all classes"
	kDNSClass_Mask        = 0x7FFF,	// Multicast DNS uses the bottom 15 bits to identify the record class...
	kDNSClass_UniqueRRSet = 0x8000	// ... and the top bit indicates that all other cached records are now invalid
	} DNS_ClassValues;

typedef enum				// From RFC 1035
	{
	kDNSType_A = 1,			//  1 Address
	kDNSType_NS,			//  2 Name Server
	kDNSType_MD,			//  3 Mail Destination
	kDNSType_MF,			//  4 Mail Forwarder
	kDNSType_CNAME,			//  5 Canonical Name
	kDNSType_SOA,			//  6 Start of Authority
	kDNSType_MB,			//  7 Mailbox
	kDNSType_MG,			//  8 Mail Group
	kDNSType_MR,			//  9 Mail Rename
	kDNSType_NULL,			// 10 NULL RR
	kDNSType_WKS,			// 11 Well-known-service
	kDNSType_PTR,			// 12 Domain name pointer
	kDNSType_HINFO,			// 13 Host information
	kDNSType_MINFO,			// 14 Mailbox information
	kDNSType_MX,			// 15 Mail Exchanger
	kDNSType_TXT,			// 16 Arbitrary text string

	kDNSType_AAAA = 28,		// 28 IPv6 address
	kDNSType_SRV = 33,		// 33 Service record

	kDNSQType_ANY = 255		// Not a DNS type, but a DNS query type, meaning "all types"
	} DNS_TypeValues;

// ***************************************************************************
#if 0
#pragma mark - Simple types
#endif

// mDNS defines its own names for these common types to simplify portability across
// multiple platforms that may each have their own (different) names for these types.
typedef unsigned char  mDNSBool;
typedef   signed char  mDNSs8;
typedef unsigned char  mDNSu8;
typedef   signed short mDNSs16;
typedef unsigned short mDNSu16;
typedef   signed long  mDNSs32;
typedef unsigned long  mDNSu32;

// To enforce useful type checking, we make mDNSInterfaceID be a pointer to a dummy struct
// This way, mDNSInterfaceIDs can be assigned, and compared with each other, but not with other types
// Declaring the type to be the typical generic "void *" would lack this type checking
typedef struct { void *dummy; } *mDNSInterfaceID;

// These types are for opaque two- and four-byte identifiers.
// The "NotAnInteger" fields of the unions allow the value to be conveniently passed around in a register
// for the sake of efficiency, but don't forget -- just because it is in a register doesn't mean it is an
// integer. Operations like add, multiply, increment, decrement, etc., are undefined for opaque identifiers.
typedef union { mDNSu8 b[2]; mDNSu16 NotAnInteger; } mDNSOpaque16;
typedef union { mDNSu8 b[4]; mDNSu32 NotAnInteger; } mDNSOpaque32;
typedef union { mDNSu8 b[16]; mDNSu16 w[8]; mDNSu32 l[4]; } mDNSOpaque128;

typedef mDNSOpaque16 mDNSIPPort;		// An IP port is a two-byte opaque identifier (not an integer)
typedef mDNSOpaque32 mDNSIPAddr;		// An IP address is a four-byte opaque identifier (not an integer)
typedef mDNSOpaque128 mDNSv6Addr;		// An IPv6 address is a 16-byte opaque identifier (not an integer)

enum
	{
	mDNSAddrType_None = 0,
	mDNSAddrType_IPv4 = 4,
	mDNSAddrType_IPv6 = 6,
	mDNSAddrType_Any  = 0xffffffff
	};

typedef struct
	{
	mDNSu32	type;
	union
		{
		mDNSv6Addr	ipv6;
		mDNSIPAddr	ipv4;
		} addr;
	} mDNSAddr;


enum { mDNSfalse = 0, mDNStrue = 1 };

#define mDNSNULL 0L

enum
	{
	mStatus_Waiting           = 1,
	mStatus_NoError           = 0,

	// mDNS Error codes are in the range FFFE FF00 (-65792) to FFFE FFFF (-65537)
	mStatus_UnknownErr        = -65537,		// 0xFFFE FFFF
	mStatus_NoSuchNameErr     = -65538,
	mStatus_NoMemoryErr       = -65539,
	mStatus_BadParamErr       = -65540,
	mStatus_BadReferenceErr   = -65541,
	mStatus_BadStateErr       = -65542,
	mStatus_BadFlagsErr       = -65543,
	mStatus_UnsupportedErr    = -65544,
	mStatus_NotInitializedErr = -65545,
	mStatus_NoCache           = -65546,
	mStatus_AlreadyRegistered = -65547,
	mStatus_NameConflict      = -65548,
	mStatus_Invalid           = -65549,

	mStatus_ConfigChanged     = -65791,
	mStatus_MemFree           = -65792		// 0xFFFE FF00
	};

typedef mDNSs32 mStatus;

// RFC 1034/1035 specify that a domain label consists of a length byte plus up to 63 characters
#define MAX_DOMAIN_LABEL 63
typedef struct { mDNSu8 c[ 64]; } domainlabel;		// One label: length byte and up to 63 characters

// RFC 1034/1035 specify that a domain name, including length bytes, data bytes, and terminating zero, may be up to 255 bytes long
#define MAX_DOMAIN_NAME 255
typedef struct { mDNSu8 c[256]; } domainname;		// Up to 255 bytes of length-prefixed domainlabels

typedef struct { mDNSu8 c[256]; } UTF8str255;		// Null-terminated C string

// ***************************************************************************
#if 0
#pragma mark - Resource Record structures
#endif

// Shared Resource Records do not have to be unique
// -- Shared Resource Records are used for NIAS service PTRs
// -- It is okay for several hosts to have RRs with the same name but different RDATA
// -- We use a random delay on replies to reduce collisions when all the hosts reply to the same query
// -- These RRs typically have moderately high TTLs (e.g. one hour)
// -- These records are announced on startup and topology changes for the benefit of passive listeners

// Unique Resource Records should be unique among hosts within any given mDNS scope
// -- The majority of Resource Records are of this type
// -- If two entities on the network have RRs with the same name but different RDATA, this is a conflict
// -- Replies may be sent immediately, because only one host should be replying to any particular query
// -- These RRs typically have low TTLs (e.g. ten seconds)
// -- On startup and after topology changes, a host issues queries to verify uniqueness

// Known Unique Resource Records are treated like Unique Resource Records, except that mDNS does
// not have to verify their uniqueness because this is already known by other means (e.g. the RR name
// is derived from the host's IP or Ethernet address, which is already known to be a unique identifier).

enum
	{
	kDNSRecordTypeUnregistered     = 0x00,	// Not currently in any list
	kDNSRecordTypeDeregistering    = 0x01,	// Shared record about to announce its departure and leave the list

	kDNSRecordTypeUnique           = 0x08,	// Will become a kDNSRecordTypeVerified when probing is complete

	kDNSRecordTypePacketAnswer     = 0x10,	// Received in the Answer Section of a DNS Response
	kDNSRecordTypePacketAdditional = 0x11,	// Received in the Additional Section of a DNS Response
	kDNSRecordTypePacketUniqueAns  = 0x18,	// Received in the Answer Section of a DNS Response with kDNSClass_UniqueRRSet set
	kDNSRecordTypePacketUniqueAdd  = 0x19,	// Received in the Additional Section of a DNS Response with kDNSClass_UniqueRRSet set

	kDNSRecordTypeShared           = 0x20,	// Shared means record name does not have to be unique -- so use random delay on replies
	kDNSRecordTypeVerified         = 0x28,	// Unique means mDNS should check that name is unique (and then send immediate replies)
	kDNSRecordTypeKnownUnique      = 0x29,	// Known Unique means mDNS can assume name is unique without checking

	kDNSRecordTypeUniqueMask       = 0x08,	// Test for records that are supposed to not be shared with other hosts
	kDNSRecordTypeRegisteredMask   = 0xF8,	// Test for records that have not had mDNS_Deregister called on them yet
	kDNSRecordTypeActiveMask       = 0xF0	// Test for all records that have finished their probing and are now active
	};

enum
	{
	kDNSSendPriorityNone       = 0,		// Don't need to send this record right now
	kDNSSendPriorityAdditional = 1,		// Send this record as an additional, if we have space in the packet
	kDNSSendPriorityAnswer     = 2		// Need to send this record as an answer
	};

typedef struct { mDNSu16 priority; mDNSu16 weight; mDNSIPPort port; domainname target; } rdataSRV;

typedef union
	{
	mDNSu8     data[768];	// Generic untyped data (temporarily set 768 for the benefit of Airport Extreme printing)
	mDNSIPAddr ip;			// For 'A' record
	mDNSv6Addr ipv6;		// For 'AAAA' record
	domainname name;		// For PTR and CNAME records
	UTF8str255 txt;			// For TXT record
	rdataSRV   srv;			// For SRV record
	} RDataBody;

typedef struct
	{
	mDNSu16    MaxRDLength;	// Amount of storage allocated for rdata (usually sizeof(RDataBody))
	mDNSu16    RDLength;	// Size of the rdata currently stored here
	RDataBody  u;
	} RData;

typedef struct ResourceRecord_struct ResourceRecord;
typedef struct ResourceRecord_struct *ResourceRecordPtr;
typedef struct DNSQuestion_struct DNSQuestion;
typedef struct mDNS_struct mDNS;
typedef struct mDNS_PlatformSupport_struct mDNS_PlatformSupport;

// Note: All mDNS API calls except mDNS_Init(), mDNS_Close(), mDNS_Execute() are legal within an mDNSRecordCallback
typedef void mDNSRecordCallback(mDNS *const m, ResourceRecord *const rr, mStatus result);

// Note:
// Restrictions: An mDNSRecordUpdateCallback may not make any mDNS API calls.
// The intent of this callback is to allow the client to free memory, if necessary.
// The internal data structures of the mDNS code may not be in a state where mDNS API calls may be made safely.
typedef void mDNSRecordUpdateCallback(mDNS *const m, ResourceRecord *const rr, RData *OldRData);

// Fields labelled "AR:" apply to our authoritative records
// Fields labelled "CR:" apply to cache records
// Fields labelled "--:" apply to both
// (May want to make this a union later, but not now, because using the
// same storage for two different purposes always makes debugging harder.)
struct ResourceRecord_struct
	{
	ResourceRecord     *next;			// --: Next in list

	// Field Group 1: Persistent metadata for Authoritative Records
	ResourceRecord     *Additional1;	// AR: Recommended additional record to include in response
	ResourceRecord     *Additional2;	// AR: Another additional
	ResourceRecord     *DependentOn;	// AR: This record depends on another for its uniqueness checking
	ResourceRecord     *RRSet;			// AR: This unique record is part of an RRSet
	mDNSRecordCallback *RecordCallback;	// AR: Callback function to call for state changes
	void               *RecordContext;	// AR: Context parameter for the callback function
	mDNSu8              RecordType;		// --: See enum above
	mDNSu8              HostTarget;		// AR: Set if the target of this record (PTR, CNAME, SRV, etc.) is our host name

	// Field Group 2: Transient state for Authoritative Records
	mDNSu8          RRInterfaceActive;	// AR: Set if InterfaceID is zero, or if InterfaceID references an active interface
	mDNSu8          Acknowledged;		// AR: Set if we've given the success callback to the client
	mDNSu8          ProbeCount;			// AR: Number of probes remaining before this record is valid (kDNSRecordTypeUnique)
	mDNSu8          AnnounceCount;		// AR: Number of announcements remaining (kDNSRecordTypeShared)
	mDNSInterfaceID SendRNow;			// AR: The interface this query is being sent on right now
	mDNSu8          IncludeInProbe;		// AR: Set if this RR is being put into a probe right now
	mDNSu8          SendPriority;		// AR: See enum above
	mDNSAddr        Requester;			// AR: Used for inter-packet duplicate suppression
										//     If set, give the IP address of the last host that sent a truncated query for this record
										//     If set to all-ones, more than one host sent such a request in the last few milliseconds
	ResourceRecord *NextResponse;		// AR: Link to the next element in the chain of responses to generate
	const mDNSu8   *NR_AnswerTo;		// AR: Set if this record was selected by virtue of being a direct answer to a question
	ResourceRecord *NR_AdditionalTo;	// AR: Set if this record was selected by virtue of being additional to another
	mDNSs32         RRLastTxTime;		// AR: In platform time units: Last time this record was sent for any reason
	mDNSs32         ThisAPInterval;		// AR: In platform time units: Current interval for announce/probe
	mDNSs32         LastAPTime;			// AR: In platform time units: Last time we sent announcement/probe
	RData          *NewRData;			// AR: Set if we are updating this record with new rdata
	mDNSRecordUpdateCallback *UpdateCallback;

	// Field Group 3: Transient state for Cache Records
	ResourceRecord *NextInKAList;		// CR: Link to the next element in the chain of known answers to send
	mDNSs32         TimeRcvd;			// CR: In platform time units
	mDNSs32         LastUsed;			// CR: In platform time units
	mDNSu32         UseCount;			// CR: Number of times this RR has been used to answer a question
	DNSQuestion    *CRActiveQuestion;	// CR: Points to an active question referencing this answer
	mDNSu32         UnansweredQueries;	// CR: Number of times we've issued a query for this record without getting an answer
	mDNSBool        NewData;			// CR: Set if this is a record we just received

	// Field Group 4: The actual information pertaining to this resource record
	mDNSInterfaceID InterfaceID;		// --: Set if this RR is specific to one interface (e.g. a linklocal address)
										// For records received off the wire, InterfaceID is *always* set to the receiving interface
										// For our authoritative records, InterfaceID is usually zero,
										// except those few records that are interface-specific (e.g. linklocal address records)
	domainname      name;				// --: All the rest are used both in our authoritative records and in cache records
	mDNSu16         rrtype;
	mDNSu16         rrclass;
	mDNSu32         rroriginalttl;		// In seconds.
	mDNSu32         rrremainingttl;		// In seconds. Always set to correct value before calling question callback.
	mDNSu16         rdestimate;			// Upper bound on size of rdata after name compression
	RData           *rdata;				// Pointer to storage for this rdata
	RData           rdatastorage;		// Normally the storage is right here, except for oversized records
	};

typedef struct NetworkInterfaceInfo_struct NetworkInterfaceInfo;

struct NetworkInterfaceInfo_struct
	{
	NetworkInterfaceInfo *next;
	mDNSInterfaceID InterfaceID;
	mDNSAddr        ip;
	mDNSu32         scope_id;
	mDNSBool        Advertise;			// Set Advertise to false if you are only searching on this interface
	mDNSBool        InterfaceActive;	// Set InterfaceActive true if interface is sending & receiving packets
										// Set InterfaceActive false if interface is here to represent an address with A and/or AAAA records,
										// but there is already an earlier representative for this physical interface
										// which will be used for the actual sending & receiving packets
										// (this status may change as interfaces are added and removed)
	// Standard ResourceRecords that every Responder host should have (one per active IP address)
	ResourceRecord RR_A1;				// 'A' or 'AAAA' (address) record for our ".local" name
	ResourceRecord RR_A2;				// 'A' or 'AAAA' record for our ".local.arpa" name
	ResourceRecord RR_PTR;				// PTR (reverse lookup) record
	};

typedef struct ExtraResourceRecord_struct ExtraResourceRecord;
struct ExtraResourceRecord_struct
	{
	ExtraResourceRecord *next;
	ResourceRecord r;
	// Note: Add any additional fields *before* the ResourceRecord in this structure, not at the end.
	// In some cases clients can allocate larger chunks of memory and set r->rdata->MaxRDLength to indicate
	// that this extra memory is available, which would result in any fields after the ResourceRecord getting smashed
	};

// Note: All mDNS API calls except mDNS_Init(), mDNS_Close(), mDNS_Execute() are legal within an mDNSServiceCallback
typedef struct ServiceRecordSet_struct ServiceRecordSet;
typedef void mDNSServiceCallback(mDNS *const m, ServiceRecordSet *const sr, mStatus result);
struct ServiceRecordSet_struct
	{
	mDNSServiceCallback *ServiceCallback;
	void                *ServiceContext;
	ExtraResourceRecord *Extras;	// Optional list of extra ResourceRecords attached to this service registration
	mDNSBool             Conflict;	// Set if this record set was forcibly deregistered because of a conflict
	domainname           Host;		// Set if this service record does not use the standard target host name
	ResourceRecord       RR_PTR;	// e.g. _printer._tcp.local.      PTR Name._printer._tcp.local.
	ResourceRecord       RR_SRV;	// e.g. Name._printer._tcp.local. SRV 0 0 port target
	ResourceRecord       RR_TXT;	// e.g. Name._printer._tcp.local. TXT PrintQueueName
	// Don't add any fields after ResourceRecord RR_TXT.
	// This is where the implicit extra space goes if we allocate a ServiceRecordSet containing an oversized RR_TXT record
	};

// ***************************************************************************
#if 0
#pragma mark - Question structures
#endif

// Note: All mDNS API calls except mDNS_Init(), mDNS_Close(), mDNS_Execute() are legal within an mDNSQuestionCallback
typedef void mDNSQuestionCallback(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer);
struct DNSQuestion_struct
	{
	DNSQuestion          *next;
	mDNSs32               LastQTime;		// In platform time units
	mDNSs32               ThisQInterval;	// In platform time units
											// ThisQInterval > 0 for an active question;
											// ThisQInterval = 0 for a suspended question that's still in the list
											// ThisQInterval = -1 for a cancelled question that's been removed from the list
	mDNSu32               RecentAnswers;	// Number of answers since the last time we sent this query
	DNSQuestion          *DuplicateOf;
	mDNSInterfaceID       InterfaceID;		// Non-zero if you want to issue link-local queries only on a single specific IP interface
	mDNSInterfaceID       SendQNow;			// The interface this query is being sent on right now
	domainname            name;
	mDNSu16               rrtype;
	mDNSu16               rrclass;
	mDNSQuestionCallback *QuestionCallback;
	void                 *QuestionContext;
	};

typedef struct
	{
	domainname      name;
	mDNSInterfaceID InterfaceID;		// ID of the interface the response was received on
	mDNSAddr        ip;					// Remote (destination) IP address where this service can be accessed
	mDNSIPPort      port;				// Port where this service can be accessed
	mDNSu16         TXTlen;
	mDNSu8          TXTinfo[2048];		// Additional demultiplexing information (e.g. LPR queue name)
	} ServiceInfo;

// Note: All mDNS API calls except mDNS_Init(), mDNS_Close(), mDNS_Execute() are legal within an mDNSServiceInfoQueryCallback
typedef struct ServiceInfoQuery_struct ServiceInfoQuery;
typedef void mDNSServiceInfoQueryCallback(mDNS *const m, ServiceInfoQuery *query);
struct ServiceInfoQuery_struct
	{
	DNSQuestion                   qSRV;
	DNSQuestion                   qTXT;
	DNSQuestion                   qAv4;
	DNSQuestion                   qAv6;
	mDNSu8                        GotSRV;
	mDNSu8                        GotTXT;
	mDNSu8                        GotADD;
	ServiceInfo                  *info;
	mDNSServiceInfoQueryCallback *ServiceInfoQueryCallback;
	void                         *ServiceInfoQueryContext;
	};

// ***************************************************************************
#if 0
#pragma mark - Main mDNS object, used to hold all the mDNS state
#endif

typedef void mDNSCallback(mDNS *const m, mStatus result);

#define CACHE_HASH_SLOTS	37

struct mDNS_struct
	{
	mDNS_PlatformSupport *p;			// Pointer to platform-specific data of indeterminite size
	mDNSBool AdvertiseLocalAddresses;
	mStatus mDNSPlatformStatus;
	mDNSCallback *MainCallback;
	void         *MainContext;

	// For debugging: To catch and report locking failures
	mDNSu32 mDNS_busy;					// Incremented between mDNS_Lock/mDNS_Unlock section
	mDNSu32 mDNS_reentrancy;			// Incremented when calling a client callback
	mDNSu8 lock_rrcache;				// For debugging: Set at times when these lists may not be modified
	mDNSu8 lock_Questions;
	mDNSu8 lock_Records;
	mDNSu8 padding;

	// Task Scheduling variables
	mDNSs32  timenow;					// The time that this particular activation of the mDNS code started
	mDNSs32  NextScheduledEvent;		// Derived from values below
	mDNSs32  SuppressSending;			// Don't send *any* packets during this time
	mDNSs32  NextCacheCheck;			// Next time to refresh cache record before it expires
	mDNSs32  NextScheduledQuery;		// Next time to send query in its exponential backoff sequence
	mDNSs32  NextProbeTime;				// Next time to probe for new authoritative record
	mDNSs32  NextAnnouncementTime;		// Next time to announce new authoritative record
	mDNSBool SendDeregistrations;		// Set if we need to send deregistrations (immediately)
	mDNSBool SendImmediateAnswers;		// Set if we need to send answers (immediately -- or as soon as SuppressSending clears)
	mDNSBool SleepState;				// Set if we're sleeping (send no more packets)

	// These fields only required for mDNS Searcher...
	DNSQuestion *Questions;				// List of all registered questions, active and inactive
	DNSQuestion *NewQuestions;			// Fresh questions not yet answered from cache
	DNSQuestion *CurrentQuestion;		// Next question about to be examined in AnswerLocalQuestions()
	mDNSu32 rrcache_size;
	mDNSu32 rrcache_used[CACHE_HASH_SLOTS];
	mDNSu32	rrcache_totalused;
	mDNSu32 rrcache_report;
	ResourceRecord *rrcache_free;
	ResourceRecord *rrcache_hash[CACHE_HASH_SLOTS];

	// Fields below only required for mDNS Responder...
	domainlabel nicelabel;				// Rich text label encoded using canonically precomposed UTF-8
	domainlabel hostlabel;				// Conforms to RFC 1034 "letter-digit-hyphen" ARPANET host name rules
	domainname  hostname1;				// Primary Host Name, e.g. "Foo.local."
	domainname  hostname2;				// Secondary Host Name, e.g. "Foo.local.arpa."
	ResourceRecord *ResourceRecords;
	ResourceRecord *CurrentRecord;		// Next ResourceRecord about to be examined
	NetworkInterfaceInfo *HostInterfaces;
	mDNSs32 ProbeFailTime;
	mDNSs32 NumFailedProbes;
	mDNSs32 SuppressProbes;
	};

// ***************************************************************************
#if 0
#pragma mark - Useful Static Constants
#endif

extern const ResourceRecord  zeroRR;
extern const mDNSIPPort      zeroIPPort;
extern const mDNSIPAddr      zeroIPAddr;
extern const mDNSv6Addr      zerov6Addr;
extern const mDNSIPAddr      onesIPAddr;
extern const mDNSInterfaceID mDNSInterface_Any;

extern const mDNSIPPort      UnicastDNSPort;
extern const mDNSIPPort      MulticastDNSPort;
extern const mDNSIPAddr      AllDNSAdminGroup;
extern const mDNSIPAddr      AllDNSLinkGroup;
extern const mDNSv6Addr      AllDNSLinkGroupv6;
extern const mDNSAddr        AllDNSLinkGroup_v4;
extern const mDNSAddr        AllDNSLinkGroup_v6;

// ***************************************************************************
#if 0
#pragma mark - Main Client Functions
#endif

// Every client should call mDNS_Init, passing in storage for the mDNS object, mDNS_PlatformSupport object, and rrcache.
// The rrcachesize parameter is the size of (i.e. number of entries in) the rrcache array passed in.
// When mDNS has finished setting up the client's callback is called
// A client can also spin and poll the mDNSPlatformStatus field to see when it changes from mStatus_Waiting to mStatus_NoError
//
// Call mDNS_Close to tidy up before exiting
//
// Call mDNS_Register with a completed ResourceRecord object to register a resource record
// If the resource record type is kDNSRecordTypeUnique (or kDNSknownunique) then if a conflicting resource record is discovered,
// the resource record's mDNSRecordCallback will be called with error code mStatus_NameConflict. The callback should deregister
// the record, and may then try registering the record again after picking a new name (e.g. by automatically appending a number).
//
// Call mDNS_StartQuery to initiate a query. mDNS will proceed to issue Multicast DNS query packets, and any time a reply
// is received containing a record which matches the question, the DNSQuestion's mDNSAnswerCallback function will be called
// Call mDNS_StopQuery when no more answers are required
//
// The mDNS routines are intentionally not thread-safe -- adding locking operations would add overhead that may not
// be necessary or appropriate on every platform. Instead, code in a pre-emptive environment calling any mDNS routine
// (except mDNS_Init and mDNS_Close) is responsible for doing the necessary synchronization to ensure that mDNS code is
// not re-entered. This includes both client software above mDNS, and the platform support code below. For example, if
// the support code on a particular platform implements timer callbacks at interrupt time, then clients on that platform
// need to disable interrupts or do similar concurrency control to ensure that the mDNS code is not entered by an
// interrupt-time timer callback while in the middle of processing a client call.

extern mStatus mDNS_Init      (mDNS *const m, mDNS_PlatformSupport *const p,
								ResourceRecord *rrcachestorage, mDNSu32 rrcachesize,
								mDNSBool AdvertiseLocalAddresses,
								mDNSCallback *Callback, void *Context);
#define mDNS_Init_NoCache                     mDNSNULL
#define mDNS_Init_ZeroCacheSize               0
#define mDNS_Init_AdvertiseLocalAddresses     mDNStrue
#define mDNS_Init_DontAdvertiseLocalAddresses mDNSfalse
#define mDNS_Init_NoInitCallback              mDNSNULL
#define mDNS_Init_NoInitCallbackContext       mDNSNULL
extern void    mDNS_Close     (mDNS *const m);
extern mDNSs32 mDNS_Execute   (mDNS *const m);
extern mStatus mDNS_Register  (mDNS *const m, ResourceRecord *const rr);
extern mStatus mDNS_Update    (mDNS *const m, ResourceRecord *const rr, mDNSu32 newttl,
								RData *const newrdata, mDNSRecordUpdateCallback *Callback);
extern mStatus mDNS_Deregister(mDNS *const m, ResourceRecord *const rr);
extern mStatus mDNS_StartQuery(mDNS *const m, DNSQuestion *const question);
extern void    mDNS_StopQuery (mDNS *const m, DNSQuestion *const question);

// ***************************************************************************
#if 0
#pragma mark - Platform support functions that are accessible to the client layer too
#endif

extern mDNSs32  mDNSPlatformOneSecond;
extern mDNSs32  mDNSPlatformTimeNow(void);

// ***************************************************************************
#if 0
#pragma mark - General utility and helper functions
#endif

// mDNS_RegisterHostSet is a single call to register the standard resource records associated with every host.
// mDNS_RegisterService is a single call to register the set of resource records associated with a given named service.
//
// mDNS_StartResolveService is single call which is equivalent to multiple calls to mDNS_StartQuery,
// to find the IP address, port number, and demultiplexing information for a given named service.
// As with mDNS_StartQuery, it executes asynchronously, and calls the ServiceInfoQueryCallback when the answer is
// found. After the service is resolved, the client should call mDNS_StopResolveService to complete the transaction.
// The client can also call mDNS_StopResolveService at any time to abort the transaction.
//
// mDNS_GetBrowseDomains is a special case of the mDNS_StartQuery call, where the resulting answers
// are a list of PTR records indicating (in the rdata) domains that are recommended for browsing.
// After getting the list of domains to browse, call mDNS_StopQuery to end the search.
// mDNS_GetDefaultBrowseDomain returns the name of the domain that should be highlighted by default.
//
// mDNS_GetRegistrationDomains and mDNS_GetDefaultRegistrationDomain are the equivalent calls to get the list
// of one or more domains that should be offered to the user as choices for where they may register their service,
// and the default domain in which to register in the case where the user has made no selection.

extern void    mDNS_SetupResourceRecord(ResourceRecord *rr, RData *RDataStorage, mDNSInterfaceID InterfaceID,
               mDNSu16 rrtype, mDNSu32 ttl, mDNSu8 RecordType, mDNSRecordCallback Callback, void *Context);

extern void    mDNS_GenerateFQDN(mDNS *const m);
extern mStatus mDNS_RegisterInterface  (mDNS *const m, NetworkInterfaceInfo *set);
extern void    mDNS_DeregisterInterface(mDNS *const m, NetworkInterfaceInfo *set);

extern mStatus mDNS_RegisterService  (mDNS *const m, ServiceRecordSet *sr,
               const domainlabel *const name, const domainname *const type, const domainname *const domain,
               const domainname *const host, mDNSIPPort port, const mDNSu8 txtinfo[], mDNSu16 txtlen,
               const mDNSInterfaceID InterfaceID, mDNSServiceCallback Callback, void *Context);
extern mStatus mDNS_AddRecordToService(mDNS *const m, ServiceRecordSet *sr, ExtraResourceRecord *extra, RData *rdata, mDNSu32 ttl);
extern mStatus mDNS_RemoveRecordFromService(mDNS *const m, ServiceRecordSet *sr, ExtraResourceRecord *extra);
extern mStatus mDNS_RenameAndReregisterService(mDNS *const m, ServiceRecordSet *const sr, const domainlabel *newname);
extern mStatus mDNS_DeregisterService(mDNS *const m, ServiceRecordSet *sr);

extern mStatus mDNS_RegisterNoSuchService(mDNS *const m, ResourceRecord *const rr,
               const domainlabel *const name, const domainname *const type, const domainname *const domain,
               const domainname *const host,
               const mDNSInterfaceID InterfaceID, mDNSRecordCallback Callback, void *Context);
#define        mDNS_DeregisterNoSuchService mDNS_Deregister

extern mStatus mDNS_StartBrowse(mDNS *const m, DNSQuestion *const question,
               const domainname *const srv, const domainname *const domain,
               const mDNSInterfaceID InterfaceID, mDNSQuestionCallback *Callback, void *Context);
#define        mDNS_StopBrowse mDNS_StopQuery

extern mStatus mDNS_StartResolveService(mDNS *const m, ServiceInfoQuery *query, ServiceInfo *info, mDNSServiceInfoQueryCallback *Callback, void *Context);
extern void    mDNS_StopResolveService (mDNS *const m, ServiceInfoQuery *query);

typedef enum
	{
	mDNS_DomainTypeBrowse              = 0,
	mDNS_DomainTypeBrowseDefault       = 1,
	mDNS_DomainTypeRegistration        = 2,
	mDNS_DomainTypeRegistrationDefault = 3
	} mDNS_DomainType;

extern mStatus mDNS_GetDomains(mDNS *const m, DNSQuestion *const question, mDNSu8 DomainType, const mDNSInterfaceID InterfaceID, mDNSQuestionCallback *Callback, void *Context);
#define        mDNS_StopGetDomains mDNS_StopQuery
extern mStatus mDNS_AdvertiseDomains(mDNS *const m, ResourceRecord *rr, mDNSu8 DomainType, const mDNSInterfaceID InterfaceID, char *domname);
#define        mDNS_StopAdvertiseDomains mDNS_Deregister

// ***************************************************************************
#if 0
#pragma mark - DNS name utility functions
#endif

// In order to expose the full capabilities of the DNS protocol (which allows any arbitrary eight-bit values
// in domain name labels, including unlikely characters like ascii nulls and even dots) all the mDNS APIs
// work with DNS's native length-prefixed strings. For convenience in C, the following utility functions
// are provided for converting between C's null-terminated strings and DNS's length-prefixed strings.

// Comparison functions
extern mDNSBool SameDomainLabel(const mDNSu8 *a, const mDNSu8 *b);
extern mDNSBool SameDomainName(const domainname *const d1, const domainname *const d2);

// Get total length of domain name, in native DNS format, including terminal root label
//   (e.g. length of "com." is 5 (length byte, three data bytes, final zero)
extern mDNSu16  DomainNameLength(const domainname *const name);

// Append functions to append one or more labels to an existing native format domain name:
//   AppendLiteralLabelString adds a single label from a literal C string, with no escape character interpretation.
//   AppendDNSNameString      adds zero or more labels from a C string using conventional DNS dots-and-escaping interpretation
//   AppendDomainLabel        adds a single label from a native format domainlabel
//   AppendDomainName         adds zero or more labels from a native format domainname
extern mDNSu8  *AppendLiteralLabelString(domainname *const name, const char *cstr);
extern mDNSu8  *AppendDNSNameString     (domainname *const name, const char *cstr);
extern mDNSu8  *AppendDomainLabel       (domainname *const name, const domainlabel *const label);
extern mDNSu8  *AppendDomainName        (domainname *const name, const domainname *const append);

// Convert from null-terminated string to native DNS format:
//   The DomainLabel form makes a single label from a literal C string, with no escape character interpretation.
//   The DomainName form makes native format domain name from a C string using conventional DNS interpretation:
//     dots separate labels, and within each label, '\.' represents a literal dot, '\\' represents a literal
//     backslash and backslash with three decimal digits (e.g. \000) represents an arbitrary byte value.
extern mDNSBool MakeDomainLabelFromLiteralString(domainlabel *const label, const char *cstr);
extern mDNSu8  *MakeDomainNameFromDNSNameString (domainname  *const name,  const char *cstr);

// Convert native format domainlabel or domainname back to C string format
extern char    *ConvertDomainLabelToCString_withescape(const domainlabel *const name, char *cstr, char esc);
#define         ConvertDomainLabelToCString_unescaped(D,C) ConvertDomainLabelToCString_withescape((D), (C), 0)
#define         ConvertDomainLabelToCString(D,C)           ConvertDomainLabelToCString_withescape((D), (C), '\\')
extern char    *ConvertDomainNameToCString_withescape(const domainname *const name, char *cstr, char esc);
#define         ConvertDomainNameToCString_unescaped(D,C) ConvertDomainNameToCString_withescape((D), (C), 0)
#define         ConvertDomainNameToCString(D,C)           ConvertDomainNameToCString_withescape((D), (C), '\\')

extern void     ConvertUTF8PstringToRFC1034HostLabel(const mDNSu8 UTF8Name[], domainlabel *const hostlabel);

extern mDNSu8  *ConstructServiceName(domainname *const fqdn, const domainlabel *name, const domainname *const type, const domainname *const domain);
extern mDNSBool DeconstructServiceName(const domainname *const fqdn, domainlabel *const name, domainname *const type, domainname *const domain);

// Note: Some old functions have been replaced by more sensibly-named versions.
// You can uncomment the hash-defines below if you don't want to have to change your source code right away.
// When updating your code, note that (unlike the old versions) *all* the new routines take the target object
// as their first parameter.
//#define ConvertCStringToDomainName(SRC,DST)  MakeDomainNameFromDNSNameString((DST),(SRC))
//#define ConvertCStringToDomainLabel(SRC,DST) MakeDomainLabelFromLiteralString((DST),(SRC))
//#define AppendStringLabelToName(DST,SRC)     AppendLiteralLabelString((DST),(SRC))
//#define AppendStringNameToName(DST,SRC)      AppendDNSNameString((DST),(SRC))
//#define AppendDomainLabelToName(DST,SRC)     AppendDomainLabel((DST),(SRC))
//#define AppendDomainNameToName(DST,SRC)      AppendDomainName((DST),(SRC))

// ***************************************************************************
#if 0
#pragma mark - Other utility functions
#endif

extern mDNSBool mDNSSameAddress(const mDNSAddr *ip1, const mDNSAddr *ip2);
extern int mDNS_sprintf(char *sbuffer, const char *fmt, ...) IS_A_PRINTF_STYLE_FUNCTION(2,3);
extern int mDNS_vsprintf(char *sbuffer, const char *fmt, va_list arg);
extern void IncrementLabelSuffix(domainlabel *name, mDNSBool RichText);

#ifdef	__cplusplus
	}
#endif

#endif
