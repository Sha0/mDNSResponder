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


   NOTE:
   If you're building an application that uses DNS Service Discovery
   this is probably NOT the header file you're looking for.
   In most cases you will want to use /usr/include/dns_sd.h instead.

   This header file defines the lowest level raw interface to mDNSCore,
   which is appropriate *only* on tiny embedded systems where everything
   runs in a single address space and memory is extremely constrained.
   All the APIs here are malloc-free, which means that the caller is
   responsible for passing in a pointer to the relevant storage that
   will be used in the execution of that call, and (when called with
   correct parameters) all the calls are guaranteed to succeed. There
   is never a case where a call can suffer intermittent failures because
   the implementation calls malloc() and sometimes malloc() returns NULL
   because memory is so limited that no more is available.
   This is primarily for devices that need to have precisely known fixed
   memory requirements, with absolutely no uncertainty or run-time variation,
   but that certainty comes at a cost of more difficult programming.
   
   For applications running on general-purpose desktop operating systems
   (Mac OS, Linux, Solaris, Windows, etc.) the API you should use is
   /usr/include/dns_sd.h, which defines the API by which multiple
   independent client processes communicate their DNS Service Discovery
   requests to a single "mdnsd" daemon running in the background.
   
   Even on platforms that don't run multiple independent processes in
   multiple independent address spaces, you can still use the preferred
   dns_sd.h APIs by linking in "dnssd_clientshim.c", which implements
   the standard "dns_sd.h" API calls, allocates any required storage
   using malloc(), and then calls through to the low-level malloc-free
   mDNSCore routines defined here. This has the benefit that even though
   you're running on a small embedded system with a single address space,
   you can still use the exact same client C code as you'd use on a
   general-purpose desktop system.


    Change History (most recent first):

$Log: mDNSEmbeddedAPI.h,v $
Revision 1.322  2007/01/10 22:51:56  cheshire
<rdar://problem/4917539> Add support for one-shot private queries as well as long-lived private queries

Revision 1.321  2007/01/09 22:37:18  cheshire
Provide ten-second grace period for deleted keys, to give mDNSResponder
time to delete host name before it gives up access to the required key.

Revision 1.320  2007/01/05 08:30:41  cheshire
Trim excessive "$Log" checkin history from before 2006
(checkin history still available via "cvs log ..." of course)

Revision 1.319  2007/01/04 23:11:11  cheshire
<rdar://problem/4720673> uDNS: Need to start caching unicast records
When an automatic browsing domain is removed, generate appropriate "remove" events for legacy queries

Revision 1.318  2007/01/04 20:57:48  cheshire
Rename ReturnCNAME to ReturnIntermed (for ReturnIntermediates)

Revision 1.317  2007/01/04 02:39:53  cheshire
<rdar://problem/4030599> Hostname passed into DNSServiceRegister ignored for Wide-Area service registrations

Revision 1.316  2006/12/22 20:59:49  cheshire
<rdar://problem/4742742> Read *all* DNS keys from keychain,
 not just key for the system-wide default registration domain

Revision 1.315  2006/12/20 04:07:35  cheshire
Remove uDNS_info substructure from AuthRecord_struct

Revision 1.314  2006/12/19 22:49:23  cheshire
Remove uDNS_info substructure from ServiceRecordSet_struct

Revision 1.313  2006/12/19 02:38:20  cheshire
Get rid of unnecessary duplicate query ID field from DNSQuestion_struct

Revision 1.312  2006/12/19 02:18:48  cheshire
Get rid of unnecessary duplicate "void *context" field from DNSQuestion_struct

Revision 1.311  2006/12/16 01:58:31  cheshire
<rdar://problem/4720673> uDNS: Need to start caching unicast records

Revision 1.310  2006/12/15 19:09:56  cheshire
<rdar://problem/4769083> ValidateRData() should be stricter about malformed MX and SRV records
Made DomainNameLength() more defensive by adding a limit parameter, so it can be
safely used to inspect potentially malformed data received from external sources.
Without this, a domain name that starts off apparently valid, but extends beyond the end of
the received packet data, could have appeared valid if the random bytes are already in memory
beyond the end of the packet just happened to have reasonable values (e.g. all zeroes).

Revision 1.309  2006/12/14 03:02:37  cheshire
<rdar://problem/4838433> Tools: dns-sd -G 0 only returns IPv6 when you have a routable IPv6 address

Revision 1.308  2006/11/30 23:07:56  herscher
<rdar://problem/4765644> uDNS: Sync up with Lighthouse changes for Private DNS

Revision 1.307  2006/11/18 05:01:30  cheshire
Preliminary support for unifying the uDNS and mDNS code,
including caching of uDNS answers

Revision 1.306  2006/11/10 07:44:04  herscher
<rdar://problem/4825493> Fix Daemon locking failures while toggling BTMM

Revision 1.305  2006/11/10 00:54:15  cheshire
<rdar://problem/4816598> Changing case of Computer Name doesn't work

Revision 1.304  2006/10/20 05:35:05  herscher
<rdar://problem/4720713> uDNS: Merge unicast active question list with multicast list.

Revision 1.303  2006/10/04 21:37:33  herscher
Remove uDNS_info substructure from DNSQuestion_struct

Revision 1.302  2006/09/26 01:53:25  herscher
<rdar://problem/4245016> NAT Port Mapping API (for both NAT-PMP and UPnP Gateway Protocol)

Revision 1.301  2006/09/15 21:20:15  cheshire
Remove uDNS_info substructure from mDNS_struct

Revision 1.300  2006/08/14 23:24:23  cheshire
Re-licensed mDNSResponder daemon source code under Apache License, Version 2.0

Revision 1.299  2006/07/15 02:01:28  cheshire
<rdar://problem/4472014> Add Private DNS client functionality to mDNSResponder
Fix broken "empty string" browsing

Revision 1.298  2006/07/05 22:55:03  cheshire
<rdar://problem/4472014> Add Private DNS client functionality to mDNSResponder
Need Private field in uDNS_RegInfo

Revision 1.297  2006/07/05 22:20:03  cheshire
<rdar://problem/4472014> Add Private DNS client functionality to mDNSResponder

Revision 1.296  2006/06/29 05:28:01  cheshire
Added comment about mDNSlocal and mDNSexport

Revision 1.295  2006/06/29 03:02:43  cheshire
<rdar://problem/4607042> mDNSResponder NXDOMAIN and CNAME support

Revision 1.294  2006/06/28 06:50:08  cheshire
In future we may want to change definition of mDNSs32 from "signed long" to "signed int"
I doubt anyone is building mDNSResponder on systems where int is 16-bits,
but lets add a compile-time assertion to make sure.

Revision 1.293  2006/06/12 18:00:43  cheshire
To make code a little more defensive, check _ILP64 before _LP64,
in case both are set by mistake on some platforms

Revision 1.292  2006/03/19 17:00:57  cheshire
Define symbol MaxMsg instead of using hard-coded constant value '80'

Revision 1.291  2006/03/19 02:00:07  cheshire
<rdar://problem/4073825> Improve logic for delaying packets after repeated interface transitions

Revision 1.290  2006/03/08 22:42:23  cheshire
Fix spelling mistake: LocalReverseMapomain -> LocalReverseMapDomain

Revision 1.289  2006/02/26 00:54:41  cheshire
Fixes to avoid code generation warning/error on FreeBSD 7

*/

#ifndef __mDNSClientAPI_h
#define __mDNSClientAPI_h

#if defined(EFI32) || defined(EFI64)
// EFI doesn't have stdarg.h
#include "Tiano.h"
#define va_list			VA_LIST
#define va_start(a, b)	VA_START(a, b)
#define va_end(a)		VA_END(a)
#define va_arg(a, b)	VA_ARG(a, b)
#else
#include <stdarg.h>		// stdarg.h is required for for va_list support for the mDNS_vsnprintf declaration
#endif

#include "mDNSDebug.h"

#ifdef __cplusplus
	extern "C" {
#endif

// ***************************************************************************
// Function scope indicators

// If you see "mDNSlocal" before a function name in a C file, it means the function is not callable outside this file
#ifndef mDNSlocal
#define mDNSlocal static
#endif
// If you see "mDNSexport" before a symbol in a C file, it means the symbol is exported for use by clients
// For every "mDNSexport" in a C file, there needs to be a corresponding "extern" declaration in some header file
// (When a C file #includes a header file, the "extern" declarations tell the compiler:
// "This symbol exists -- but not necessarily in this C file.")
#ifndef mDNSexport
#define mDNSexport
#endif

// Explanation: These local/export markers are a little habit of mine for signaling the programmers' intentions.
// When "mDNSlocal" is just a synonym for "static", and "mDNSexport" is a complete no-op, you could be
// forgiven for asking what purpose they serve. The idea is that if you see "mDNSexport" in front of a
// function definition it means the programmer intended it to be exported and callable from other files
// in the project. If you see "mDNSlocal" in front of a function definition it means the programmer
// intended it to be private to that file. If you see neither in front of a function definition it
// means the programmer forgot (so you should work out which it is supposed to be, and fix it).
// Using "mDNSlocal" instead of "static" makes it easier to do a textual searches for one or the other.
// For example you can do a search for "static" to find if any functions declare any local variables as "static"
// (generally a bad idea unless it's also "const", because static storage usually risks being non-thread-safe)
// without the results being cluttered with hundreds of matches for functions declared static.
// - Stuart Cheshire

// ***************************************************************************
// Structure packing macro

// If we're not using GNUC, it's not fatal.
// Most compilers naturally pack the on-the-wire structures correctly anyway, so a plain "struct" is usually fine.
// In the event that structures are not packed correctly, mDNS_Init() will detect this and report an error, so the
// developer will know what's wrong, and can investigate what needs to be done on that compiler to provide proper packing.
#ifndef packedstruct
 #if ((__GNUC__ > 2) || ((__GNUC__ == 2) && (__GNUC_MINOR__ >= 9)))
  #define packedstruct struct __attribute__((__packed__))
  #define packedunion  union  __attribute__((__packed__))
 #else
  #define packedstruct struct
  #define packedunion  union
 #endif
#endif

// ***************************************************************************
#if 0
#pragma mark - DNS Resource Record class and type constants
#endif

typedef enum							// From RFC 1035
	{
	kDNSClass_IN               = 1,		// Internet
	kDNSClass_CS               = 2,		// CSNET
	kDNSClass_CH               = 3,		// CHAOS
	kDNSClass_HS               = 4,		// Hesiod
	kDNSClass_NONE             = 254,	// Used in DNS UPDATE [RFC 2136]

	kDNSClass_Mask             = 0x7FFF,// Multicast DNS uses the bottom 15 bits to identify the record class...
	kDNSClass_UniqueRRSet      = 0x8000,// ... and the top bit indicates that all other cached records are now invalid

	kDNSQClass_ANY             = 255,	// Not a DNS class, but a DNS query class, meaning "all classes"
	kDNSQClass_UnicastResponse = 0x8000	// Top bit set in a question means "unicast response acceptable"
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
	kDNSType_SRV  = 33,		// 33 Service record
	kDNSType_OPT  = 41,     // EDNS0 OPT record
	kDNSType_TSIG = 250,    // 250 Transaction Signature

	kDNSQType_ANY = 255		// Not a DNS type, but a DNS query type, meaning "all types"
	} DNS_TypeValues;

// ***************************************************************************
#if 0
#pragma mark - Simple types
#endif

// mDNS defines its own names for these common types to simplify portability across
// multiple platforms that may each have their own (different) names for these types.
typedef          int   mDNSBool;
typedef   signed char  mDNSs8;
typedef unsigned char  mDNSu8;
typedef   signed short mDNSs16;
typedef unsigned short mDNSu16;

// <http://gcc.gnu.org/onlinedocs/gcc-3.3.3/cpp/Common-Predefined-Macros.html> says
//   __LP64__ _LP64
//   These macros are defined, with value 1, if (and only if) the compilation is
//   for a target where long int and pointer both use 64-bits and int uses 32-bit.
// <http://www.intel.com/software/products/compilers/clin/docs/ug/lin1077.htm> says
//   Macro Name __LP64__ Value 1
// A quick Google search for "defined(__LP64__)" OR "#ifdef __LP64__" gives 2590 hits and
// a search for "#if __LP64__" gives only 12, so I think we'll go with the majority and use defined()
#if defined(_ILP64) || defined(__ILP64__)
typedef   signed int32 mDNSs32;
typedef unsigned int32 mDNSu32;
#elif defined(_LP64) || defined(__LP64__)
typedef   signed int   mDNSs32;
typedef unsigned int   mDNSu32;
#else
typedef   signed long  mDNSs32;
typedef unsigned long  mDNSu32;
//typedef   signed int mDNSs32;
//typedef unsigned int mDNSu32;
#endif

// To enforce useful type checking, we make mDNSInterfaceID be a pointer to a dummy struct
// This way, mDNSInterfaceIDs can be assigned, and compared with each other, but not with other types
// Declaring the type to be the typical generic "void *" would lack this type checking
typedef struct mDNSInterfaceID_dummystruct { void *dummy; } *mDNSInterfaceID;

// These types are for opaque two- and four-byte identifiers.
// The "NotAnInteger" fields of the unions allow the value to be conveniently passed around in a
// register for the sake of efficiency, and compared for equality or inequality, but don't forget --
// just because it is in a register doesn't mean it is an integer. Operations like greater than,
// less than, add, multiply, increment, decrement, etc., are undefined for opaque identifiers,
// and if you make the mistake of trying to do those using the NotAnInteger field, then you'll
// find you get code that doesn't work consistently on big-endian and little-endian machines.
typedef packedunion { mDNSu8 b[ 2]; mDNSu16 NotAnInteger; } mDNSOpaque16;
typedef packedunion { mDNSu8 b[ 4]; mDNSu32 NotAnInteger; } mDNSOpaque32;
typedef packedunion { mDNSu8 b[ 6]; mDNSu16 w[3]; mDNSu32 l[1]; } mDNSOpaque48;
typedef packedunion { mDNSu8 b[ 8]; mDNSu16 w[4]; mDNSu32 l[2]; } mDNSOpaque64;
typedef packedunion { mDNSu8 b[16]; mDNSu16 w[8]; mDNSu32 l[4]; } mDNSOpaque128;

typedef mDNSOpaque16  mDNSIPPort;		// An IP port is a two-byte opaque identifier (not an integer)
typedef mDNSOpaque32  mDNSv4Addr;		// An IP address is a four-byte opaque identifier (not an integer)
typedef mDNSOpaque128 mDNSv6Addr;		// An IPv6 address is a 16-byte opaque identifier (not an integer)
typedef mDNSOpaque48  mDNSEthAddr;		// An Ethernet address is a six-byte opaque identifier (not an integer)

enum
	{
	mDNSAddrType_None    = 0,
	mDNSAddrType_IPv4    = 4,
	mDNSAddrType_IPv6    = 6,
	mDNSAddrType_Unknown = ~0	// Special marker value used in known answer list recording
	};

typedef struct
	{
	mDNSs32 type;
	union { mDNSv6Addr v6; mDNSv4Addr v4; } ip;
	} mDNSAddr;

enum { mDNSfalse = 0, mDNStrue = 1 };

#define mDNSNULL 0L

enum
	{
	mStatus_Waiting           = 1,
	mStatus_NoError           = 0,

	// mDNS return values are in the range FFFE FF00 (-65792) to FFFE FFFF (-65537)
	// The top end of the range (FFFE FFFF) is used for error codes;
	// the bottom end of the range (FFFE FF00) is used for non-error values;

	// Error codes:
	mStatus_UnknownErr        = -65537,		// First value: 0xFFFE FFFF
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
	mStatus_Firewall          = -65550,
	mStatus_Incompatible      = -65551,
	mStatus_BadInterfaceErr   = -65552,
	mStatus_Refused           = -65553,
	mStatus_NoSuchRecord      = -65554,
	mStatus_NoAuth            = -65555,
	mStatus_NoSuchKey         = -65556,
	mStatus_NATTraversal      = -65557,
	mStatus_DoubleNAT         = -65558,
	mStatus_BadTime           = -65559,
	mStatus_BadSig            = -65560,     // while we define this per RFC 2845, BIND 9 returns Refused for bad/missing signatures
	mStatus_BadKey            = -65561,
	mStatus_TransientErr      = -65562,     // transient failures, e.g. sending packets shortly after a network transition or wake from sleep
	// -65563 to -65786 currently unused; available for allocation

	// tcp connection status
	mStatus_ConnPending       = -65787,
	mStatus_ConnFailed        = -65788,
	mStatus_ConnEstablished   = -65789,

	// Non-error values:
	mStatus_GrowCache         = -65790,
	mStatus_ConfigChanged     = -65791,
	mStatus_MemFree           = -65792		// Last value: 0xFFFE FF00
	
	// mStatus_MemFree is the last legal mDNS error code, at the end of the range allocated for mDNS
	};

typedef mDNSs32 mStatus;

// RFC 1034/1035 specify that a domain label consists of a length byte plus up to 63 characters
#define MAX_DOMAIN_LABEL 63
typedef struct { mDNSu8 c[ 64]; } domainlabel;		// One label: length byte and up to 63 characters

// RFC 1034/1035 specify that a domain name, including length bytes, data bytes, and terminating zero, may be up to 255 bytes long
#define MAX_DOMAIN_NAME 255
typedef struct { mDNSu8 c[256]; } domainname;		// Up to 255 bytes of length-prefixed domainlabels

typedef struct { mDNSu8 c[256]; } UTF8str255;		// Null-terminated C string

// The longest legal textual form of a DNS name is 1005 bytes, including the C-string terminating NULL at the end.
// Explanation:
// When a native domainname object is converted to printable textual form using ConvertDomainNameToCString(),
// non-printing characters are represented in the conventional DNS way, as '\ddd', where ddd is a three-digit decimal number.
// The longest legal domain name is 255 bytes, in the form of four labels as shown below:
// Length byte, 63 data bytes, length byte, 63 data bytes, length byte, 63 data bytes, length byte, 61 data bytes, zero byte.
// Each label is encoded textually as characters followed by a trailing dot.
// If every character has to be represented as a four-byte escape sequence, then this makes the maximum textual form four labels
// plus the C-string terminating NULL as shown below:
// 63*4+1 + 63*4+1 + 63*4+1 + 61*4+1 + 1 = 1005.
// Note that MAX_ESCAPED_DOMAIN_LABEL is not normally used: If you're only decoding a single label, escaping is usually not required.
// It is for domain names, where dots are used as label separators, that proper escaping is vital.
#define MAX_ESCAPED_DOMAIN_LABEL 254
#define MAX_ESCAPED_DOMAIN_NAME 1005

// Most records have a TTL of 75 minutes, so that their 80% cache-renewal query occurs once per hour.
// For records containing a hostname (in the name on the left, or in the rdata on the right),
// like A, AAAA, reverse-mapping PTR, and SRV, we use a two-minute TTL by default, because we don't want
// them to hang around for too long in the cache if the host in question crashes or otherwise goes away.
// Wide-area service discovery records have a very short TTL to avoid poluting intermediate caches with
// dynamic records.  When discovered via Long Lived Queries (with change notifications), resource record
// TTLs can be safely ignored.
	
#define kStandardTTL (3600UL * 100 / 80)
#define kHostNameTTL 120UL
#define kWideAreaTTL 3

#define DefaultTTLforRRType(X) (((X) == kDNSType_A || (X) == kDNSType_AAAA || (X) == kDNSType_SRV) ? kHostNameTTL : kStandardTTL)

// ***************************************************************************
#if 0
#pragma mark - DNS Message structures
#endif

#define mDNS_numZones   numQuestions
#define mDNS_numPrereqs numAnswers
#define mDNS_numUpdates numAuthorities

typedef packedstruct
	{
	mDNSOpaque16 id;
	mDNSOpaque16 flags;
	mDNSu16 numQuestions;
	mDNSu16 numAnswers;
	mDNSu16 numAuthorities;
	mDNSu16 numAdditionals;
	} DNSMessageHeader;

// We can send and receive packets up to 9000 bytes (Ethernet Jumbo Frame size, if that ever becomes widely used)
// However, in the normal case we try to limit packets to 1500 bytes so that we don't get IP fragmentation on standard Ethernet
// 40 (IPv6 header) + 8 (UDP header) + 12 (DNS message header) + 1440 (DNS message body) = 1500 total
#define AbsoluteMaxDNSMessageData 8940
#define NormalMaxDNSMessageData 1440
typedef packedstruct
	{
	DNSMessageHeader h;						// Note: Size 12 bytes
	mDNSu8 data[AbsoluteMaxDNSMessageData];	// 40 (IPv6) + 8 (UDP) + 12 (DNS header) + 8940 (data) = 9000
	} DNSMessage;

// ***************************************************************************
#if 0
#pragma mark - Resource Record structures
#endif

// Authoritative Resource Records:
// There are four basic types: Shared, Advisory, Unique, Known Unique

// * Shared Resource Records do not have to be unique
// -- Shared Resource Records are used for DNS-SD service PTRs
// -- It is okay for several hosts to have RRs with the same name but different RDATA
// -- We use a random delay on responses to reduce collisions when all the hosts respond to the same query
// -- These RRs typically have moderately high TTLs (e.g. one hour)
// -- These records are announced on startup and topology changes for the benefit of passive listeners
// -- These records send a goodbye packet when deregistering
//
// * Advisory Resource Records are like Shared Resource Records, except they don't send a goodbye packet
//
// * Unique Resource Records should be unique among hosts within any given mDNS scope
// -- The majority of Resource Records are of this type
// -- If two entities on the network have RRs with the same name but different RDATA, this is a conflict
// -- Responses may be sent immediately, because only one host should be responding to any particular query
// -- These RRs typically have low TTLs (e.g. a few minutes)
// -- On startup and after topology changes, a host issues queries to verify uniqueness

// * Known Unique Resource Records are treated like Unique Resource Records, except that mDNS does
// not have to verify their uniqueness because this is already known by other means (e.g. the RR name
// is derived from the host's IP or Ethernet address, which is already known to be a unique identifier).

// Summary of properties of different record types:
// Probe?    Does this record type send probes before announcing?
// Conflict? Does this record type react if we observe an apparent conflict?
// Goodbye?  Does this record type send a goodbye packet on departure?
//
//               Probe? Conflict? Goodbye? Notes
// Unregistered                            Should not appear in any list (sanity check value)
// Shared         No      No       Yes     e.g. Service PTR record
// Deregistering  No      No       Yes     Shared record about to announce its departure and leave the list
// Advisory       No      No       No
// Unique         Yes     Yes      No      Record intended to be unique -- will probe to verify
// Verified       Yes     Yes      No      Record has completed probing, and is verified unique
// KnownUnique    No      Yes      No      Record is assumed by other means to be unique

// Valid lifecycle of a record:
// Unregistered ->                   Shared      -> Deregistering -(goodbye)-> Unregistered
// Unregistered ->                   Advisory                               -> Unregistered
// Unregistered -> Unique -(probe)-> Verified                               -> Unregistered
// Unregistered ->                   KnownUnique                            -> Unregistered

// Each Authoritative kDNSRecordType has only one bit set. This makes it easy to quickly see if a record
// is one of a particular set of types simply by performing the appropriate bitwise masking operation.

// Cache Resource Records (received from the network):
// There are four basic types: Answer, Unique Answer, Additional, Unique Additional
// Bit 7 (the top bit) of kDNSRecordType is always set for Cache Resource Records; always clear for Authoritative Resource Records
// Bit 6 (value 0x40) is set for answer records; clear for additional records
// Bit 5 (value 0x20) is set for records received with the kDNSClass_UniqueRRSet

enum
	{
	kDNSRecordTypeUnregistered     = 0x00,	// Not currently in any list
	kDNSRecordTypeDeregistering    = 0x01,	// Shared record about to announce its departure and leave the list

	kDNSRecordTypeUnique           = 0x02,	// Will become a kDNSRecordTypeVerified when probing is complete

	kDNSRecordTypeAdvisory         = 0x04,	// Like Shared, but no goodbye packet
	kDNSRecordTypeShared           = 0x08,	// Shared means record name does not have to be unique -- use random delay on responses

	kDNSRecordTypeVerified         = 0x10,	// Unique means mDNS should check that name is unique (and then send immediate responses)
	kDNSRecordTypeKnownUnique      = 0x20,	// Known Unique means mDNS can assume name is unique without checking
	                                        // For Dynamic Update records, Known Unique means the record must already exist on the server.
	kDNSRecordTypeUniqueMask       = (kDNSRecordTypeUnique | kDNSRecordTypeVerified | kDNSRecordTypeKnownUnique),
	kDNSRecordTypeActiveMask       = (kDNSRecordTypeAdvisory | kDNSRecordTypeShared | kDNSRecordTypeVerified | kDNSRecordTypeKnownUnique),

	kDNSRecordTypePacketAdd        = 0x80,	// Received in the Additional  Section of a DNS Response
	kDNSRecordTypePacketAddUnique  = 0x90,	// Received in the Additional  Section of a DNS Response with kDNSClass_UniqueRRSet set
	kDNSRecordTypePacketAuth       = 0xA0,	// Received in the Authorities Section of a DNS Response
	kDNSRecordTypePacketAuthUnique = 0xB0,	// Received in the Authorities Section of a DNS Response with kDNSClass_UniqueRRSet set
	kDNSRecordTypePacketAns        = 0xC0,	// Received in the Answer      Section of a DNS Response
	kDNSRecordTypePacketAnsUnique  = 0xD0,	// Received in the Answer      Section of a DNS Response with kDNSClass_UniqueRRSet set

	kDNSRecordTypePacketAnsMask    = 0x40,	// True for PacketAns and    PacketAnsUnique
	kDNSRecordTypePacketUniqueMask = 0x10	// True for PacketAddUnique, PacketAnsUnique, PacketAuthUnique
	};

typedef packedstruct { mDNSu16 priority; mDNSu16 weight; mDNSIPPort port; domainname target;   } rdataSRV;
typedef packedstruct { mDNSu16 preference;                                domainname exchange; } rdataMX;

typedef packedstruct
	{
	domainname mname;
	domainname rname;
	mDNSs32 serial;		// Modular counter; increases when zone changes
	mDNSu32 refresh;	// Time in seconds that a slave waits after successful replication of the database before it attempts replication again
	mDNSu32 retry;		// Time in seconds that a slave waits after an unsuccessful replication attempt before it attempts replication again
	mDNSu32 expire;		// Time in seconds that a slave holds on to old data while replication attempts remain unsuccessful
	mDNSu32 min;		// Nominally the minimum record TTL for this zone, in seconds; also used for negative caching.
	} rdataSOA;

typedef packedstruct
	{
	mDNSu16      vers;
	mDNSu16      llqOp;
	mDNSu16      err;
	mDNSOpaque64 id;
	mDNSu32      lease;
	} LLQOptData;

#define LLQ_OPTLEN ((3 * sizeof(mDNSu16)) + 8 + sizeof(mDNSu32))
// Windows adds pad bytes to sizeof(LLQOptData).  Use this macro when setting length fields or validating option rdata from
// off the wire.  Use sizeof(LLQOptData) when dealing with structures (e.g. memcpy).  Never memcpy between on-the-wire
// representation and a structure
	
// NOTE: rdataOpt format may be repeated an arbitrary number of times in a single resource record
typedef packedstruct
	{
	mDNSu16 opt;
	mDNSu16 optlen;
	union { LLQOptData llq; mDNSu32 lease; } OptData;
	} rdataOpt;

// StandardAuthRDSize is 264 (256+8), which is large enough to hold a maximum-sized SRV record
// MaximumRDSize is 8K the absolute maximum we support (at least for now)
#define StandardAuthRDSize 264
#define MaximumRDSize 8192

// InlineCacheRDSize is 68
// Records received from the network with rdata this size or less have their rdata stored right in the CacheRecord object
// Records received from the network with rdata larger than this have additional storage allocated for the rdata
// A quick unscientific sample from a busy network at Apple with lots of machines revealed this:
// 1461 records in cache
// 292 were one-byte TXT records
// 136 were four-byte A records
// 184 were sixteen-byte AAAA records
// 780 were various PTR, TXT and SRV records from 12-64 bytes
// Only 69 records had rdata bigger than 64 bytes
// Note that since CacheRecord object and a CacheGroup object are allocated out of the same pool, it's sensible to
// have them both be the same size. Making one smaller without making the other smaller won't actually save any memory.
#define InlineCacheRDSize 68

#define InlineCacheGroupNameSize 144

typedef union
	{
	mDNSu8      data[StandardAuthRDSize];
	mDNSv4Addr  ipv4;		// For 'A' record
	mDNSv6Addr  ipv6;		// For 'AAAA' record
	domainname  name;		// For PTR, NS, and CNAME records
	UTF8str255  txt;		// For TXT record
	rdataSRV    srv;		// For SRV record
	rdataMX     mx;			// For MX record
	rdataSOA    soa;        // For SOA record
	rdataOpt    opt;        // For eDNS0 opt record
	} RDataBody;

typedef struct
	{
	mDNSu16    MaxRDLength;	// Amount of storage allocated for rdata (usually sizeof(RDataBody))
	RDataBody  u;
	} RData;
#define sizeofRDataHeader (sizeof(RData) - sizeof(RDataBody))

typedef struct AuthRecord_struct AuthRecord;
typedef struct CacheRecord_struct CacheRecord;
typedef struct CacheGroup_struct CacheGroup;
typedef struct DNSQuestion_struct DNSQuestion;
typedef struct NTAContext_struct NTAContext;
typedef struct mDNS_struct mDNS;
typedef struct mDNS_PlatformSupport_struct mDNS_PlatformSupport;
typedef struct NATTraversalInfo_struct NATTraversalInfo;

// Note: Within an mDNSRecordCallback mDNS all API calls are legal except mDNS_Init(), mDNS_Close(), mDNS_Execute()
typedef void mDNSRecordCallback(mDNS *const m, AuthRecord *const rr, mStatus result);

// Note:
// Restrictions: An mDNSRecordUpdateCallback may not make any mDNS API calls.
// The intent of this callback is to allow the client to free memory, if necessary.
// The internal data structures of the mDNS code may not be in a state where mDNS API calls may be made safely.
typedef void mDNSRecordUpdateCallback(mDNS *const m, AuthRecord *const rr, RData *OldRData);

typedef struct
	{
	mDNSu8          RecordType;			// See enum above
	mDNSInterfaceID InterfaceID;		// Set if this RR is specific to one interface
										// For records received off the wire, InterfaceID is *always* set to the receiving interface
										// For our authoritative records, InterfaceID is usually zero, except for those few records
										// that are interface-specific (e.g. address records, especially linklocal addresses)
	domainname     *name;
	mDNSu16         rrtype;
	mDNSu16         rrclass;
	mDNSu32         rroriginalttl;		// In seconds
	mDNSu16         rdlength;			// Size of the raw rdata, in bytes
	mDNSu16         rdestimate;			// Upper bound on size of rdata after name compression
	mDNSu32         namehash;			// Name-based (i.e. case-insensitive) hash of name
	mDNSu32         rdatahash;			// For rdata containing domain name (e.g. PTR, SRV, CNAME etc.), case-insensitive name hash
										// else, for all other rdata, 32-bit hash of the raw rdata
										// Note: This requirement is important. Various routines like AddAdditionalsToResponseList(),
										// ReconfirmAntecedents(), etc., use rdatahash as a pre-flight check to see
										// whether it's worth doing a full SameDomainName() call. If the rdatahash
										// is not a correct case-insensitive name hash, they'll get false negatives.
	RData           *rdata;				// Pointer to storage for this rdata
	} ResourceRecord;

// Unless otherwise noted, states may apply to either independent record registrations or service registrations
typedef enum
	{
	regState_Zero              = 0,
	regState_FetchingZoneData  = 1,     // getting info - update not sent
	regState_Pending           = 2,     // update sent, reply not received
	regState_Registered        = 3,     // update sent, reply received
	regState_DeregPending      = 4,     // dereg sent, reply not received
	regState_DeregDeferred     = 5,     // dereg requested while in Pending state - send dereg AFTER registration is confirmed
	regState_Cancelled         = 6,     // update not sent, reg. cancelled by client
	regState_Unregistered      = 8,     // not in any list
	regState_Refresh           = 9,     // outstanding refresh (or target change) message
	regState_NATMap            = 10,    // establishing NAT port mapping or learning public address
	regState_UpdatePending     = 11,    // update in flight as result of mDNS_Update call
	regState_NoTarget          = 12,    // service registration pending registration of hostname (ServiceRegistrations only)
    regState_ExtraQueued       = 13,    // extra record to be registered upon completion of service registration (RecordRegistrations only)
	regState_NATError          = 14     // unable to complete NAT traversal
	} regState_t;

struct AuthRecord_struct
	{
	// For examples of how to set up this structure for use in mDNS_Register(),
	// see mDNS_AdvertiseInterface() or mDNS_RegisterService().
	// Basically, resrec and persistent metadata need to be set up before calling mDNS_Register().
	// mDNS_SetupResourceRecord() is avaliable as a helper routine to set up most fields to sensible default values for you

	AuthRecord     *next;				// Next in list; first element of structure for efficiency reasons
	// Field Group 1: Common ResourceRecord fields
	ResourceRecord  resrec;

	// Field Group 2: Persistent metadata for Authoritative Records
	AuthRecord     *Additional1;		// Recommended additional record to include in response
	AuthRecord     *Additional2;		// Another additional
	AuthRecord     *DependentOn;		// This record depends on another for its uniqueness checking
	AuthRecord     *RRSet;				// This unique record is part of an RRSet
	mDNSRecordCallback *RecordCallback;	// Callback function to call for state changes, and to free memory asynchronously on deregistration
	void           *RecordContext;		// Context parameter for the callback function
	mDNSu8          HostTarget;			// Set if the target of this record (PTR, CNAME, SRV, etc.) is our host name
	mDNSu8          AllowRemoteQuery;	// Set if we allow hosts not on the local link to query this record
	mDNSu8          ForceMCast;			// Set by client to advertise solely via multicast, even for apparently unicast names

	// Field Group 3: Transient state for Authoritative Records
	mDNSu8          Acknowledged;		// Set if we've given the success callback to the client
	mDNSu8          ProbeCount;			// Number of probes remaining before this record is valid (kDNSRecordTypeUnique)
	mDNSu8          AnnounceCount;		// Number of announcements remaining (kDNSRecordTypeShared)
	mDNSu8          RequireGoodbye;		// Set if this RR has been announced on the wire and will require a goodbye packet
	mDNSu8          LocalAnswer;		// Set if this RR has been delivered to LocalOnly questions
	mDNSu8          IncludeInProbe;		// Set if this RR is being put into a probe right now
	mDNSInterfaceID ImmedAnswer;		// Someone on this interface issued a query we need to answer (all-ones for all interfaces)
	mDNSu8          ImmedUnicast;		// Set if we may send our response directly via unicast to the requester
#if MDNS_LOG_ANSWER_SUPPRESSION_TIMES
	mDNSs32         ImmedAnswerMarkTime;
#endif
	mDNSInterfaceID ImmedAdditional;	// Hint that we might want to also send this record, just to be helpful
	mDNSInterfaceID SendRNow;			// The interface this query is being sent on right now
	mDNSv4Addr      v4Requester;		// Recent v4 query for this record, or all-ones if more than one recent query
	mDNSv6Addr      v6Requester;		// Recent v6 query for this record, or all-ones if more than one recent query
	AuthRecord     *NextResponse;		// Link to the next element in the chain of responses to generate
	const mDNSu8   *NR_AnswerTo;		// Set if this record was selected by virtue of being a direct answer to a question
	AuthRecord     *NR_AdditionalTo;	// Set if this record was selected by virtue of being additional to another
	mDNSs32         ThisAPInterval;		// In platform time units: Current interval for announce/probe
	mDNSs32         AnnounceUntil;		// In platform time units: Creation time + TTL
	mDNSs32         LastAPTime;			// In platform time units: Last time we sent announcement/probe
	mDNSs32         LastMCTime;			// Last time we multicast this record (used to guard against packet-storm attacks)
	mDNSInterfaceID LastMCInterface;	// Interface this record was multicast on at the time LastMCTime was recorded
	RData          *NewRData;			// Set if we are updating this record with new rdata
	mDNSu16         newrdlength;		// ... and the length of the new RData
	mDNSRecordUpdateCallback *UpdateCallback;
	mDNSu32         UpdateCredits;		// Token-bucket rate limiting of excessive updates
	mDNSs32         NextUpdateCredit;	// Time next token is added to bucket
	mDNSs32         UpdateBlocked;		// Set if update delaying is in effect

	// Field Group 4: Transient uDNS state for Authoritative Records
	regState_t   state;
	mDNSBool     lease;		// dynamic update contains (should contain) lease option
	mDNSs32      expire;	// expiration of lease (-1 for static)
	mDNSBool     Private;	// If zone is private, DNS updates may have to be encrypted to prevent eavesdropping
	mDNSOpaque16 id;		// identifier to match update request and response
	domainname   zone;			// the zone that is updated
	mDNSAddr     UpdateServer;	// DNS server that handles updates for this zone
	mDNSIPPort   UpdatePort;	// port on which server accepts dynamic updates
								// !!!KRS not technically correct to cache longer than TTL
								// SDC Perhaps should keep a reference to the relevant SRV record in the cache?
	NATTraversalInfo *NATinfo;	// NAT traversal context; may be NULL
	
	// uDNS_UpdateRecord support fields
	// Do we really need all these in *addition* to NewRData and newrdlength above?
	RData *OrigRData;
	mDNSu16 OrigRDLen;		// previously registered, being deleted
	RData *InFlightRData;
	mDNSu16 InFlightRDLen;	// currently being registered
	RData *QueuedRData;		// if the client call Update while an update is in flight, we must finish the
	mDNSu16 QueuedRDLen;	// pending operation (re-transmitting if necessary) THEN register the queued update

	// Field Group 4: Large data objects go at the end
	domainname      namestorage;
	RData           rdatastorage;		// Normally the storage is right here, except for oversized records
	// rdatastorage MUST be the last thing in the structure -- when using oversized AuthRecords, extra bytes
	// are appended after the end of the AuthRecord, logically augmenting the size of the rdatastorage
	// DO NOT ADD ANY MORE FIELDS HERE
	};

// Wrapper struct for Auth Records for higher-level code that cannot use the AuthRecord's ->next pointer field
typedef struct ARListElem
	{
	struct ARListElem *next;
	AuthRecord ar;          // Note: Must be last struct in field to accomodate oversized AuthRecords
	} ARListElem;

struct CacheGroup_struct				// Header object for a list of CacheRecords with the same name
	{
	CacheGroup     *next;				// Next CacheGroup object in this hash table bucket
	mDNSu32         namehash;			// Name-based (i.e. case insensitive) hash of name
	CacheRecord    *members;			// List of CacheRecords with this same name
	CacheRecord   **rrcache_tail;		// Tail end of that list
	domainname     *name;				// Common name for all CacheRecords in this list
	mDNSu8          namestorage[InlineCacheGroupNameSize];
	};

struct CacheRecord_struct
	{
	CacheRecord    *next;				// Next in list; first element of structure for efficiency reasons
	ResourceRecord  resrec;

	// Transient state for Cache Records
	CacheRecord    *NextInKAList;		// Link to the next element in the chain of known answers to send
	mDNSs32         TimeRcvd;			// In platform time units
	mDNSs32         DelayDelivery;		// Set if we want to defer delivery of this answer to local clients
	mDNSs32         NextRequiredQuery;	// In platform time units
	mDNSs32         LastUsed;			// In platform time units
	DNSQuestion    *CRActiveQuestion;	// Points to an active question referencing this answer
	mDNSu32         UnansweredQueries;	// Number of times we've issued a query for this record without getting an answer
	mDNSs32         LastUnansweredTime;	// In platform time units; last time we incremented UnansweredQueries
	mDNSu32         MPUnansweredQ;		// Multi-packet query handling: Number of times we've seen a query for this record
	mDNSs32         MPLastUnansweredQT;	// Multi-packet query handling: Last time we incremented MPUnansweredQ
	mDNSu32         MPUnansweredKA;		// Multi-packet query handling: Number of times we've seen this record in a KA list
	mDNSBool        MPExpectingKA;		// Multi-packet query handling: Set when we increment MPUnansweredQ; allows one KA
	CacheRecord    *NextInCFList;		// Set if this is in the list of records we just received with the cache flush bit set

	struct { mDNSu16 MaxRDLength; mDNSu8 data[InlineCacheRDSize]; } rdatastorage;	// Storage for small records is right here
	};

// Storage sufficient to hold either a CacheGroup header or a CacheRecord
typedef union CacheEntity_union CacheEntity;
union CacheEntity_union { CacheEntity *next; CacheGroup cg; CacheRecord cr; };

typedef struct
	{
	CacheRecord r;
	mDNSu8 _extradata[MaximumRDSize-InlineCacheRDSize];		// Glue on the necessary number of extra bytes
	domainname namestorage;									// Needs to go *after* the extra rdata bytes
	} LargeCacheRecord;

typedef struct uDNS_HostnameInfo
	{
	struct uDNS_HostnameInfo *next;
    domainname fqdn;
    AuthRecord *arv4;                         // registered IPv4 address record
	AuthRecord *arv6;                         // registered IPv6 address record
	mDNSRecordCallback *StatusCallback;       // callback to deliver success or error code to client layer
	const void *StatusContext;                // Client Context
	} uDNS_HostnameInfo;

enum
   	{
   	DNSServer_Untested = 0,
   	DNSServer_Failed   = 1,
   	DNSServer_Passed   = 2
   	};

typedef struct DNSServer
	{
    struct DNSServer *next;
    mDNSAddr   addr;
    mDNSBool   del;			// Set when we're planning to delete this from the list
    mDNSu32    teststate;	// Have we sent bug-detection query to this server?
    domainname domain;		// name->server matching for "split dns"
	} DNSServer;

typedef struct NetworkInterfaceInfo_struct NetworkInterfaceInfo;

// A NetworkInterfaceInfo_struct serves two purposes:
// 1. It holds the address, PTR and HINFO records to advertise a given IP address on a given physical interface
// 2. It tells mDNSCore which physical interfaces are available; each physical interface has its own unique InterfaceID.
//    Since there may be multiple IP addresses on a single physical interface,
//    there may be multiple NetworkInterfaceInfo_structs with the same InterfaceID.
//    In this case, to avoid sending the same packet n times, when there's more than one
//    struct with the same InterfaceID, mDNSCore picks one member of the set to be the
//    active representative of the set; all others have the 'InterfaceActive' flag unset.

struct NetworkInterfaceInfo_struct
	{
	// Internal state fields. These are used internally by mDNSCore; the client layer needn't be concerned with them.
	NetworkInterfaceInfo *next;

	mDNSBool        InterfaceActive;	// Set if interface is sending & receiving packets (see comment above)
	mDNSBool        IPv4Available;		// If InterfaceActive, set if v4 available on this InterfaceID
	mDNSBool        IPv6Available;		// If InterfaceActive, set if v6 available on this InterfaceID

	// Standard AuthRecords that every Responder host should have (one per active IP address)
	AuthRecord RR_A;					// 'A' or 'AAAA' (address) record for our ".local" name
	AuthRecord RR_PTR;					// PTR (reverse lookup) record
	AuthRecord RR_HINFO;

	// Client API fields: The client must set up these fields *before* calling mDNS_RegisterInterface()
	mDNSInterfaceID InterfaceID;		// Identifies physical interface; MUST NOT be 0, -1, or -2
	mDNSAddr        ip;					// The IPv4 or IPv6 address to advertise
	mDNSAddr        mask;
	char            ifname[64];			// Windows uses a GUID string for the interface name, which doesn't fit in 16 bytes
	mDNSBool        Advertise;			// False if you are only searching on this interface
	mDNSBool        McastTxRx;			// Send/Receive multicast on this { InterfaceID, address family } ?
	};

typedef struct ExtraResourceRecord_struct ExtraResourceRecord;
struct ExtraResourceRecord_struct
	{
	ExtraResourceRecord *next;
    mDNSu32 ClientID;  // Opaque ID field to be used by client to map an AddRecord call to a set of Extra records
	AuthRecord r;
	// Note: Add any additional fields *before* the AuthRecord in this structure, not at the end.
	// In some cases clients can allocate larger chunks of memory and set r->rdata->MaxRDLength to indicate
	// that this extra memory is available, which would result in any fields after the AuthRecord getting smashed
	};

// Note: Within an mDNSServiceCallback mDNS all API calls are legal except mDNS_Init(), mDNS_Close(), mDNS_Execute()
typedef struct ServiceRecordSet_struct ServiceRecordSet;
typedef void mDNSServiceCallback(mDNS *const m, ServiceRecordSet *const sr, mStatus result);

// A ServiceRecordSet is basically a convenience structure to group together
// the PTR/SRV/TXT records that make up a standard service registration
// It contains its own ServiceCallback+ServiceContext to report aggregate results up to the next layer of software above
// It also contains:
//  * the "_services" PTR record for service enumeration
//  * the optional target host name (for proxy registrations)
//  * the optional list of SubType PTR records
//  * the optional list of additional records attached to the service set (e.g. iChat pictures)
//
// ... and a bunch of stuff related to uDNS, some of which could be simplified or eliminated

struct ServiceRecordSet_struct
	{
	// These internal state fields are used internally by mDNSCore; the client layer needn't be concerned with them.
	// No fields need to be set up by the client prior to calling mDNS_RegisterService();
	// all required data is passed as parameters to that function.
	ServiceRecordSet    *next;
	
	// Begin uDNS info ****************
	// Hopefully much of this stuff can be simplified or eliminated
	
	regState_t   state;
	mDNSBool     lease;    // dynamic update contains (should contain) lease option
	mDNSs32      expire;   // expiration of lease (-1 for static)
	mDNSBool     TestForSelfConflict;  // on name conflict, check if we're just seeing our own orphaned records
	mDNSBool     Private;  // If zone is private, DNS updates may have to be encrypted to prevent eavesdropping
	mDNSOpaque16 id;
	domainname   zone;     // the zone that is updated
	mDNSAddr     ns;       // primary name server for the record's zone  !!!KRS not technically correct to cache longer than TTL
	mDNSIPPort   port;     // port on which server accepts dynamic updates
	NATTraversalInfo *NATinfo; // may be NULL
    mDNSBool     ClientCallbackDeferred;  // invoke client callback on completion of pending operation(s)
	mStatus      DeferredStatus;          // status to deliver when above flag is set
    mDNSBool     SRVUpdateDeferred;       // do we need to change target or port once current operation completes?
    mDNSBool     SRVChanged;              // temporarily deregistered service because its SRV target or port changed

	// End uDNS info ****************
	
	mDNSServiceCallback *ServiceCallback;
	void                *ServiceContext;
	mDNSBool             Conflict;	// Set if this record set was forcibly deregistered because of a conflict
	
	ExtraResourceRecord *Extras;	// Optional list of extra AuthRecords attached to this service registration
	mDNSu32              NumSubTypes;
	AuthRecord          *SubTypes;
	AuthRecord           RR_ADV;	// e.g. _services._dns-sd._udp.local. PTR _printer._tcp.local.
	AuthRecord           RR_PTR;	// e.g. _printer._tcp.local.        PTR Name._printer._tcp.local.
	AuthRecord           RR_SRV;	// e.g. Name._printer._tcp.local.   SRV 0 0 port target
	AuthRecord           RR_TXT;	// e.g. Name._printer._tcp.local.   TXT PrintQueueName
	// Don't add any fields after AuthRecord RR_TXT.
	// This is where the implicit extra space goes if we allocate a ServiceRecordSet containing an oversized RR_TXT record
	};

// ***************************************************************************
#if 0
#pragma mark - Question structures
#endif

// We record the last eight instances of each duplicate query
// This gives us v4/v6 on each of Ethernet/AirPort and Firewire, and two free slots "for future expansion"
// If the host has more active interfaces that this it is not fatal -- duplicate question suppression will degrade gracefully.
// Since we will still remember the last eight, the busiest interfaces will still get the effective duplicate question suppression.
#define DupSuppressInfoSize 8

typedef struct
	{
	mDNSs32               Time;
	mDNSInterfaceID       InterfaceID;
	mDNSs32               Type;				// v4 or v6?
	} DupSuppressInfo;

// A struct that abstracts away the differences in TCP/SSL sockets
 
typedef struct uDNS_TCPSocket_struct * uDNS_TCPSocket;

// A struct that abstracts away the differences in UDP sockets

typedef struct uDNS_UDPSocket_struct * uDNS_UDPSocket;


typedef enum
	{
	// Setup states
	LLQ_UnInit            = 0,
	LLQ_GetZoneInfo       = 1,
	LLQ_InitialRequest    = 2,
	LLQ_SecondaryRequest  = 3,
	LLQ_Refresh           = 4,
	LLQ_Retry             = 5,
	LLQ_Established       = 6,
	LLQ_Suspended         = 7,
	LLQ_SuspendDeferred   = 8, // suspend once we get zone info
	LLQ_SuspendedPoll     = 9, // suspended from polling state
	LLQ_NatMapWaitTCP     = 10,
	LLQ_NatMapWaitUDP     = 11,
	
	// Established/error states
	LLQ_Static            = 16,
	LLQ_Poll              = 17,
	LLQ_Error             = 18,
	LLQ_Cancelled         = 19
	} LLQ_State;

typedef struct
	{
	LLQ_State state;
	mDNSBool isPrivate;
	NATTraversalInfo * NATInfoTCP;	// This is used if we're browsing behind a NAT
	NATTraversalInfo * NATInfoUDP;
	mDNSAddr servAddr;
	mDNSIPPort servPort;
	mDNSIPPort eventPort;			// This is non-zero if this is a private LLQ.  It is the port number that both the TCP
	                                // and the UDP socket are bound to.  This allows us to receive event notifications via
	                                // TCP or UDP.
	uDNS_TCPSocket tcpSock;
	uDNS_UDPSocket udpSock;
	DNSQuestion *question;
	mDNSu32 origLease;  // seconds (relative)
	mDNSs32 expire; // ticks (absolute)
    mDNSs16 ntries;
	mDNSOpaque64 id;
	} LLQ_Info;

// LLQ constants
#define kDNSOpt_LLQ	   1
#define kDNSOpt_Lease  2
#define kLLQ_Vers      1
#define kLLQ_DefLease  7200 // 2 hours
#define kLLQ_MAX_TRIES 3    // retry an operation 3 times max
#define kLLQ_INIT_RESEND 2 // resend an un-ack'd packet after 2 seconds, then double for each additional
#define kLLQ_DEF_RETRY 1800 // retry a failed operation after 30 minutes
// LLQ Operation Codes
#define kLLQOp_Setup     1
#define kLLQOp_Refresh   2
#define kLLQOp_Event     3

#define LLQ_OPT_RDLEN ((2 * sizeof(mDNSu16)) + LLQ_OPTLEN)
#define LEASE_OPT_RDLEN (2 * sizeof(mDNSu16)) + sizeof(mDNSs32)

// LLQ Errror Codes
enum
	{
	LLQErr_NoError = 0,
	LLQErr_ServFull = 1,
	LLQErr_Static = 2,
	LLQErr_FormErr = 3,
	LLQErr_NoSuchLLQ = 4,
	LLQErr_BadVers = 5,
	LLQErr_UnknownErr = 6
	};

typedef void (*InternalResponseHndlr)(mDNS *const m, DNSMessage *msg, const  mDNSu8 *end, DNSQuestion *question);

#define HMAC_LEN    64
#define HMAC_IPAD   0x36
#define HMAC_OPAD   0x5c
#define MD5_LEN     16

// Internal data structure to maintain authentication information
typedef struct DomainAuthInfo
	{
	struct DomainAuthInfo *next;
	mDNSs32     deltime;
	domainname  domain;
	domainname  keyname;
	mDNSu8      keydata_ipad[HMAC_LEN];	// padded key for inner hash rounds
	mDNSu8      keydata_opad[HMAC_LEN];	// padded key for outer hash rounds
	} DomainAuthInfo;

// Note: Within an mDNSQuestionCallback mDNS all API calls are legal except mDNS_Init(), mDNS_Close(), mDNS_Execute()
typedef void mDNSQuestionCallback(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, mDNSBool AddRecord);
struct DNSQuestion_struct
	{
	// Internal state fields. These are used internally by mDNSCore; the client layer needn't be concerned with them.
	DNSQuestion          *next;
	mDNSu32               qnamehash;
	mDNSs32               DelayAnswering;	// Set if we want to defer answering this question until the cache settles
	mDNSs32               LastQTime;		// Last scheduled transmission of this Q on *all* applicable interfaces
	mDNSs32               ThisQInterval;	// LastQTime + ThisQInterval is the next scheduled transmission of this Q
											// ThisQInterval > 0 for an active question;
											// ThisQInterval = 0 for a suspended question that's still in the list
											// ThisQInterval = -1 for a cancelled question (should not still be in list)
	mDNSs32               ExpectUnicastResp;// Set when we send a query with the kDNSQClass_UnicastResponse bit set
	mDNSs32               LastAnswerPktNum;	// The sequence number of the last response packet containing an answer to this Q
	mDNSu32               RecentAnswerPkts;	// Number of answers since the last time we sent this query
	mDNSu32               CurrentAnswers;	// Number of records currently in the cache that answer this question
	mDNSu32               LargeAnswers;		// Number of answers with rdata > 1024 bytes
	mDNSu32               UniqueAnswers;	// Number of answers received with kDNSClass_UniqueRRSet bit set
	mDNSInterfaceID       FlappingInterface;// Set when an interface goes away, to flag if remove events are delivered for this Q
	DNSQuestion          *DuplicateOf;
	DNSQuestion          *NextInDQList;
	DupSuppressInfo       DupSuppress[DupSuppressInfoSize];
	mDNSInterfaceID       SendQNow;			// The interface this query is being sent on right now
	mDNSBool              SendOnAll;		// Set if we're sending this question on all active interfaces
	mDNSu32               RequestUnicast;	// Non-zero if we want to send query with kDNSQClass_UnicastResponse bit set
	mDNSs32               LastQTxTime;		// Last time this Q was sent on one (but not necessarily all) interfaces
	DomainAuthInfo       *Private;			// Set if query is currently being done using Private DNS

	// Wide Area fields.  These are used internally by the uDNS core
	InternalResponseHndlr responseCallback; // This is one of: recvSetupResponse, pktResponseHndlr or getZoneData
											// We should get rid of the function pointer, and just use the state
											// variables in the DNSQuestion_struct to tell us what we need to do
											// (maybe all three routines will end up combined into one?)
	mDNSBool              Answered;         // Have we received an answer (including NXDOMAIN) for this question?
	mDNSs32               RestartTime;      // Mark when we restart a suspended query
	uDNS_TCPSocket        sock;		        // For secure operations
	LLQ_Info              *llq;             // NULL for 1-shot queries

	// Client API fields: The client must set up these fields *before* calling mDNS_StartQuery()
	mDNSInterfaceID       InterfaceID;		// Non-zero if you want to issue queries only on a single specific IP interface
	mDNSAddr              Target;			// Non-zero if you want to direct queries to a specific unicast target address
	mDNSIPPort            TargetPort;		// Must be set if Target is set
	mDNSOpaque16          TargetQID;		// Must be set if Target is set
	domainname            qname;
	mDNSu16               qtype;
	mDNSu16               qclass;
	mDNSBool              LongLived;        // Set by client for calls to mDNS_StartQuery to indicate LLQs to unicast layer.
	mDNSBool              ExpectUnique;		// Set by client if it's expecting unique RR(s) for this question, not shared RRs
	mDNSBool              ForceMCast;		// Set by client to force mDNS query, even for apparently uDNS names
	mDNSBool              ReturnIntermed;	// Set by client to request callbacks for intermediate CNAME/NXDOMAIN results
	mDNSQuestionCallback *QuestionCallback;
	void                 *QuestionContext;
	};
	
typedef struct
	{
	// Client API fields: The client must set up name and InterfaceID *before* calling mDNS_StartResolveService()
	// When the callback is invoked, ip, port, TXTlen and TXTinfo will have been filled in with the results learned from the network.
	domainname      name;
	mDNSInterfaceID InterfaceID;		// ID of the interface the response was received on
	mDNSAddr        ip;					// Remote (destination) IP address where this service can be accessed
	mDNSIPPort      port;				// Port where this service can be accessed
	mDNSu16         TXTlen;
	mDNSu8          TXTinfo[2048];		// Additional demultiplexing information (e.g. LPR queue name)
	} ServiceInfo;

// Note: Within an mDNSServiceInfoQueryCallback mDNS all API calls are legal except mDNS_Init(), mDNS_Close(), mDNS_Execute()
typedef struct ServiceInfoQuery_struct ServiceInfoQuery;
typedef void mDNSServiceInfoQueryCallback(mDNS *const m, ServiceInfoQuery *query);
struct ServiceInfoQuery_struct
	{
	// Internal state fields. These are used internally by mDNSCore; the client layer needn't be concerned with them.
	// No fields need to be set up by the client prior to calling mDNS_StartResolveService();
	// all required data is passed as parameters to that function.
	// The ServiceInfoQuery structure memory is working storage for mDNSCore to discover the requested information
	// and place it in the ServiceInfo structure. After the client has called mDNS_StopResolveService(), it may
	// dispose of the ServiceInfoQuery structure while retaining the results in the ServiceInfo structure.
	DNSQuestion                   qSRV;
	DNSQuestion                   qTXT;
	DNSQuestion                   qAv4;
	DNSQuestion                   qAv6;
	mDNSu8                        GotSRV;
	mDNSu8                        GotTXT;
	mDNSu8                        GotADD;
	mDNSu32                       Answers;
	ServiceInfo                  *info;
	mDNSServiceInfoQueryCallback *ServiceInfoQueryCallback;
	void                         *ServiceInfoQueryContext;
	};

// ***************************************************************************
#if 0
#pragma mark - NAT Traversal structures and constants
#endif

#define NATMAP_INIT_RETRY (mDNSPlatformOneSecond / 4)          // start at 250ms w/ exponential decay
#define NATMAP_MAX_RETRY mDNSPlatformOneSecond                 // back off to once per second
#define NATMAP_MAX_TRIES 3                                     // for max 3 tries
#define NATMAP_DEFAULT_LEASE (60 * 60)  // lease life in seconds
#define NATMAP_VERS 0
#define NATMAP_RESPONSE_MASK 0x80

typedef enum
	{
	NATOp_AddrRequest = 0,
	NATOp_MapUDP      = 1,
	NATOp_MapTCP      = 2
	} NATOp_t;

enum
   	{
   	NATErr_None = 0,
   	NATErr_Vers = 1,
   	NATErr_Refused = 2,
   	NATErr_NetFail = 3,
   	NATErr_Res = 4,
   	NATErr_Opcode = 5
   	};

typedef mDNSu16 NATErr_t;

typedef enum
	{
	NATState_Init         = 0,
	NATState_Request      = 1,
	NATState_Established  = 2,
	NATState_Legacy       = 3,
	NATState_Error        = 4,
	NATState_Refresh      = 5,
	NATState_Deleted      = 6
	} NATState_t;
// Note: we have no explicit "cancelled" state, where a service/interface is deregistered while we
 // have an outstanding NAT request.  This is conveyed by the "reg" pointer being set to NULL

typedef packedstruct
	{
	mDNSu8 vers;
	mDNSu8 opcode;
	} NATAddrRequest;
	
typedef packedstruct
	{
	mDNSu8 vers;
	mDNSu8 opcode;
	mDNSOpaque16 err;
	mDNSOpaque32 uptime;
	mDNSv4Addr   PubAddr;
	} NATAddrReply;

typedef packedstruct
	{
	mDNSu8 vers;
	mDNSu8 opcode;
	mDNSOpaque16 unused;
	mDNSIPPort priv;
	mDNSIPPort pub;
	mDNSOpaque32 lease;
	} NATPortMapRequest;
	
typedef packedstruct
	{
	mDNSu8 vers;
	mDNSu8 opcode;
	mDNSOpaque16 err;
	mDNSOpaque32 uptime;
	mDNSIPPort priv;
	mDNSIPPort pub;
	mDNSOpaque32 lease;
	} NATPortMapReply;
	
// Pass NULL for pkt on error (including timeout)
typedef mDNSBool (*NATResponseHndlr)(NATTraversalInfo *n, mDNS *m, mDNSu8 *pkt, mDNSu16 len);

struct NATTraversalInfo_struct
	{
	NATOp_t op;
	NATResponseHndlr ReceiveResponse;
	union { AuthRecord *RecordRegistration; ServiceRecordSet *ServiceRegistration; } reg;
    mDNSAddr Router;
	mDNSIPPort PrivatePort;
    mDNSIPPort PublicPort;
	mDNSu32 TTL;
    union { NATAddrRequest AddrReq; NATPortMapRequest PortReq; } request;
	mDNSs32 retry;                   // absolute time when we retry
	mDNSs32 RetryInterval;           // delta between time sent and retry
	int ntries;
	NATState_t state;
	NATTraversalInfo *next;
	mDNSBool isLLQ;
	void *context;
	unsigned refs;
	};

typedef struct IPAddrListElem
	{
	mDNSAddr addr;
	struct IPAddrListElem *next;
	} IPAddrListElem;

typedef struct DNameListElem
	{
	domainname name;
	struct DNameListElem *next;
	} DNameListElem;

// ***************************************************************************
#if 0
#pragma mark - Main mDNS object, used to hold all the mDNS state
#endif

typedef void mDNSCallback(mDNS *const m, mStatus result);

#define CACHE_HASH_SLOTS 499

enum
	{
	mDNS_KnownBug_PhantomInterfaces = 1
	};

struct mDNS_struct
	{
	// Internal state fields. These hold the main internal state of mDNSCore;
	// the client layer needn't be concerned with them.
	// No fields need to be set up by the client prior to calling mDNS_Init();
	// all required data is passed as parameters to that function.

	mDNS_PlatformSupport *p;			// Pointer to platform-specific data of indeterminite size
	mDNSu32  KnownBugs;
	mDNSBool CanReceiveUnicastOn5353;
	mDNSBool AdvertiseLocalAddresses;
	mStatus mDNSPlatformStatus;
	mDNSIPPort UnicastPort4;
	mDNSIPPort UnicastPort6;
	mDNSCallback *MainCallback;
	void         *MainContext;

	// For debugging: To catch and report locking failures
	mDNSu32 mDNS_busy;					// Incremented between mDNS_Lock/mDNS_Unlock section
	mDNSu32 mDNS_reentrancy;			// Incremented when calling a client callback
	mDNSu8  mDNS_shutdown;				// Set when we're shutting down, allows us to skip some unnecessary steps
	mDNSu8  lock_rrcache;				// For debugging: Set at times when these lists may not be modified
	mDNSu8  lock_Questions;
	mDNSu8  lock_Records;
	#define MaxMsg 120
	char MsgBuffer[MaxMsg];				// Temp storage used while building error log messages

	// Task Scheduling variables
	mDNSs32  timenow_adjust;			// Correction applied if we ever discover time went backwards
	mDNSs32  timenow;					// The time that this particular activation of the mDNS code started
	mDNSs32  timenow_last;				// The time the last time we ran
	mDNSs32  NextScheduledEvent;		// Derived from values below
	mDNSs32  SuppressSending;			// Don't send *any* packets during this time
	mDNSs32  NextCacheCheck;			// Next time to refresh cache record before it expires
	mDNSs32  NextScheduledQuery;		// Next time to send query in its exponential backoff sequence
	mDNSs32  NextScheduledProbe;		// Next time to probe for new authoritative record
	mDNSs32  NextScheduledResponse;		// Next time to send authoritative record(s) in responses
	mDNSs32  RandomQueryDelay;			// For de-synchronization of query packets on the wire
	mDNSu32  RandomReconfirmDelay;		// For de-synchronization of reconfirmation queries on the wire
	mDNSs32  PktNum;					// Unique sequence number assigned to each received packet
	mDNSBool SendDeregistrations;		// Set if we need to send deregistrations (immediately)
	mDNSBool SendImmediateAnswers;		// Set if we need to send answers (immediately -- or as soon as SuppressSending clears)
	mDNSBool SleepState;				// Set if we're sleeping (send no more packets)

	// These fields only required for mDNS Searcher...
	DNSQuestion *Questions;				// List of all registered questions, active and inactive
	DNSQuestion *NewQuestions;			// Fresh questions not yet answered from cache
	DNSQuestion *CurrentQuestion;		// Next question about to be examined in AnswerLocalQuestions()
	DNSQuestion *LocalOnlyQuestions;	// Questions with InterfaceID set to mDNSInterface_LocalOnly
	DNSQuestion *NewLocalOnlyQuestions;	// Fresh local-only questions not yet answered
	mDNSu32 rrcache_size;				// Total number of available cache entries
	mDNSu32 rrcache_totalused;			// Number of cache entries currently occupied
	mDNSu32 rrcache_active;				// Number of cache entries currently occupied by records that answer active questions
	mDNSu32 rrcache_report;
	CacheEntity *rrcache_free;
	CacheGroup *rrcache_hash[CACHE_HASH_SLOTS];

	// Fields below only required for mDNS Responder...
	domainlabel nicelabel;				// Rich text label encoded using canonically precomposed UTF-8
	domainlabel hostlabel;				// Conforms to RFC 1034 "letter-digit-hyphen" ARPANET host name rules
	domainname  MulticastHostname;		// Fully Qualified "dot-local" Host Name, e.g. "Foo.local."
	UTF8str255  HIHardware;
	UTF8str255  HISoftware;
	AuthRecord *ResourceRecords;
	AuthRecord *DuplicateRecords;		// Records currently 'on hold' because they are duplicates of existing records
	AuthRecord *NewLocalRecords;		// Fresh local-only records not yet delivered to local-only questions
	AuthRecord *CurrentRecord;			// Next AuthRecord about to be examined
	NetworkInterfaceInfo *HostInterfaces;
	mDNSs32 ProbeFailTime;
	mDNSu32 NumFailedProbes;
	mDNSs32 SuppressProbes;

	// unicast-specific data
	mDNSs32          nextevent;
	ServiceRecordSet *ServiceRegistrations;
	AuthRecord       *RecordRegistrations;
	NATTraversalInfo *NATTraversals;
	mDNSu16          NextMessageID;
    DNSServer        *Servers;           // list of DNS servers
	mDNSAddr         Router;
	mDNSAddr         AdvertisedV4;       // IPv4 address pointed to by hostname
	mDNSAddr         MappedV4;           // Cache of public address if PrimaryIP is behind a NAT
	mDNSAddr         AdvertisedV6;       // IPv6 address pointed to by hostname
	domainname       ServiceRegDomain;   // (going away w/ multi-user support)
	DomainAuthInfo  *AuthInfoList;       // list of domains requiring authentication for updates
	uDNS_HostnameInfo *Hostnames;        // List of registered hostnames + hostname metadata
    DNSQuestion      ReverseMap;         // Reverse-map query to find static hostname for service target
    mDNSBool         ReverseMapActive;   // Is above query active?
    domainname       StaticHostname;     // Current answer to reverse-map query (above)
    mDNSBool         DelaySRVUpdate;     // Delay SRV target/port update to avoid "flap"
    mDNSs32          NextSRVUpdate;      // Time to perform delayed update
	domainname		 RegDomain;          // Default wide-area zone for service registration
	struct DNameListElem *BrowseDomains;      // Default wide-area zone for legacy ("empty string") browses
    domainname       FQDN;
    mDNSBool         RegisterSearchDomains;
    
	DNameListElem  *DefBrowseList;  // cache of answers to above query (where we search for empty string browses)
	DNameListElem  *DefRegList;  // manually generated list of domains where we register for empty string registrations

	mDNSs32 SuppressStdPort53Queries;	// Wait before allowing the next standard unicast query to the user's configured DNS server

	// Fixed storage, to avoid creating large objects on the stack
	DNSMessage imsg;		// Incoming message received from wire
	DNSMessage omsg;		// Outgoing message we're building
	LargeCacheRecord rec;	// Resource Record extracted from received message
	};

#define FORALL_CACHERECORDS(SLOT,CG,CR)                          \
	for ((SLOT) = 0; (SLOT) < CACHE_HASH_SLOTS; (SLOT)++)        \
		for((CG)=m->rrcache_hash[(SLOT)]; (CG); (CG)=(CG)->next) \
			for ((CR) = (CG)->members; (CR); (CR)=(CR)->next)

// ***************************************************************************
#if 0
#pragma mark - Useful Static Constants
#endif

extern const mDNSIPPort      zeroIPPort;
extern const mDNSv4Addr      zerov4Addr;
extern const mDNSv6Addr      zerov6Addr;
extern const mDNSEthAddr     zeroEthAddr;
extern const mDNSv4Addr      onesIPv4Addr;
extern const mDNSv6Addr      onesIPv6Addr;
extern const mDNSAddr        zeroAddr;

extern const mDNSInterfaceID mDNSInterface_Any;				// Zero
extern const mDNSInterfaceID mDNSInterface_LocalOnly;		// Special value
extern const mDNSInterfaceID mDNSInterface_Unicast;			// Special value

extern const mDNSIPPort      UnicastDNSPort;
extern const mDNSIPPort      NATPMPPort;
extern const mDNSIPPort      DNSEXTPort;
extern const mDNSIPPort      MulticastDNSPort;
extern const mDNSIPPort      LoopbackIPCPort;
extern const mDNSIPPort      PrivateDNSPort;
extern const mDNSIPPort      NSIPCPort;

extern const mDNSv4Addr      AllDNSAdminGroup;
//#define AllDNSLinkGroupv4 (AllDNSLinkGroup_v4.ip.v4)
//#define AllDNSLinkGroupv6 (AllDNSLinkGroup_v6.ip.v6)
extern const mDNSAddr        AllDNSLinkGroup_v4;
extern const mDNSAddr        AllDNSLinkGroup_v6;

extern const mDNSOpaque16 zeroID;
extern const mDNSOpaque16 QueryFlags;
extern const mDNSOpaque16 uQueryFlags;
extern const mDNSOpaque16 ResponseFlags;
extern const mDNSOpaque16 UpdateReqFlags;
extern const mDNSOpaque16 UpdateRespFlags;

extern const mDNSOpaque64    zeroOpaque64;

#define localdomain (*(const domainname *)"\x5" "local")
#define LocalReverseMapDomain (*(const domainname *)"\x3" "254" "\x3" "169" "\x7" "in-addr" "\x4" "arpa")
	
// ***************************************************************************
#if 0
#pragma mark - Inline functions
#endif

#if (defined(_MSC_VER))
	#define mDNSinline static __inline
#elif ((__GNUC__ > 2) || ((__GNUC__ == 2) && (__GNUC_MINOR__ >= 9)))
	#define mDNSinline static inline
#endif

// If we're not doing inline functions, then this header needs to have the extern declarations
#if !defined(mDNSinline)
extern mDNSs32      NonZeroTime(mDNSs32 t);
extern mDNSu16      mDNSVal16(mDNSOpaque16 x);
extern mDNSu32      mDNSVal32(mDNSOpaque32 x);
extern mDNSOpaque16 mDNSOpaque16fromIntVal(mDNSu16 v);
extern mDNSOpaque32 mDNSOpaque32fromIntVal(mDNSu32 v);
#endif

// If we're compiling the particular C file that instantiates our inlines, then we
// define "mDNSinline" (to empty string) so that we generate code in the following section
#if (!defined(mDNSinline) && mDNS_InstantiateInlines)
#define mDNSinline
#endif

#ifdef mDNSinline

mDNSinline mDNSs32 NonZeroTime(mDNSs32 t) { if (t) return(t); else return(1); }

mDNSinline mDNSu16 mDNSVal16(mDNSOpaque16 x) { return((mDNSu16)((mDNSu16)x.b[0] <<  8 | (mDNSu16)x.b[1])); }
mDNSinline mDNSu32 mDNSVal32(mDNSOpaque32 x) { return((mDNSu32)((mDNSu32)x.b[0] << 24 | (mDNSu32)x.b[1] << 16 | (mDNSu32)x.b[2] << 8 | (mDNSu32)x.b[3])); }

mDNSinline mDNSOpaque16 mDNSOpaque16fromIntVal(mDNSu16 v)
	{
	mDNSOpaque16 x;
	x.b[0] = (mDNSu8)(v >> 8);
	x.b[1] = (mDNSu8)(v & 0xFF);
	return(x);
	}

mDNSinline mDNSOpaque32 mDNSOpaque32fromIntVal(mDNSu32 v)
	{
	mDNSOpaque32 x;
	x.b[0] = (mDNSu8) (v >> 24)        ;
	x.b[1] = (mDNSu8)((v >> 16) & 0xFF);
	x.b[2] = (mDNSu8)((v >> 8 ) & 0xFF);
	x.b[3] = (mDNSu8)((v      ) & 0xFF);
	return x;
	}

#endif

// ***************************************************************************
#if 0
#pragma mark - Main Client Functions
#endif

// Every client should call mDNS_Init, passing in storage for the mDNS object and the mDNS_PlatformSupport object.
//
// Clients that are only advertising services should use mDNS_Init_NoCache and mDNS_Init_ZeroCacheSize.
// Clients that plan to perform queries (mDNS_StartQuery, mDNS_StartBrowse, mDNS_StartResolveService, etc.)
// need to provide storage for the resource record cache, or the query calls will return 'mStatus_NoCache'.
// The rrcachestorage parameter is the address of memory for the resource record cache, and
// the rrcachesize parameter is the number of entries in the CacheRecord array passed in.
// (i.e. the size of the cache memory needs to be sizeof(CacheRecord) * rrcachesize).
// OS X 10.3 Panther uses an initial cache size of 64 entries, and then mDNSCore sends an
// mStatus_GrowCache message if it needs more.
//
// Most clients should use mDNS_Init_AdvertiseLocalAddresses. This causes mDNSCore to automatically
// create the correct address records for all the hosts interfaces. If you plan to advertise
// services being offered by the local machine, this is almost always what you want.
// There are two cases where you might use mDNS_Init_DontAdvertiseLocalAddresses:
// 1. A client-only device, that browses for services but doesn't advertise any of its own.
// 2. A proxy-registration service, that advertises services being offered by other machines, and takes
//    the appropriate steps to manually create the correct address records for those other machines.
// In principle, a proxy-like registration service could manually create address records for its own machine too,
// but this would be pointless extra effort when using mDNS_Init_AdvertiseLocalAddresses does that for you.
//
// When mDNS has finished setting up the client's callback is called
// A client can also spin and poll the mDNSPlatformStatus field to see when it changes from mStatus_Waiting to mStatus_NoError
//
// Call mDNS_Close to tidy up before exiting
//
// Call mDNS_Register with a completed AuthRecord object to register a resource record
// If the resource record type is kDNSRecordTypeUnique (or kDNSknownunique) then if a conflicting resource record is discovered,
// the resource record's mDNSRecordCallback will be called with error code mStatus_NameConflict. The callback should deregister
// the record, and may then try registering the record again after picking a new name (e.g. by automatically appending a number).
// Following deregistration, the RecordCallback will be called with result mStatus_MemFree to signal that it is safe to deallocate
// the record's storage (memory must be freed asynchronously to allow for goodbye packets and dynamic update deregistration).
//
// Call mDNS_StartQuery to initiate a query. mDNS will proceed to issue Multicast DNS query packets, and any time a response
// is received containing a record which matches the question, the DNSQuestion's mDNSAnswerCallback function will be called
// Call mDNS_StopQuery when no more answers are required
//
// Care should be taken on multi-threaded or interrupt-driven environments.
// The main mDNS routines call mDNSPlatformLock() on entry and mDNSPlatformUnlock() on exit;
// each platform layer needs to implement these appropriately for its respective platform.
// For example, if the support code on a particular platform implements timer callbacks at interrupt time, then
// mDNSPlatformLock/Unlock need to disable interrupts or do similar concurrency control to ensure that the mDNS
// code is not entered by an interrupt-time timer callback while in the middle of processing a client call.

extern mStatus mDNS_Init      (mDNS *const m, mDNS_PlatformSupport *const p,
								CacheEntity *rrcachestorage, mDNSu32 rrcachesize,
								mDNSBool AdvertiseLocalAddresses,
								mDNSCallback *Callback, void *Context);
// See notes above on use of NoCache/ZeroCacheSize
#define mDNS_Init_NoCache                     mDNSNULL
#define mDNS_Init_ZeroCacheSize               0
// See notes above on use of Advertise/DontAdvertiseLocalAddresses
#define mDNS_Init_AdvertiseLocalAddresses     mDNStrue
#define mDNS_Init_DontAdvertiseLocalAddresses mDNSfalse
#define mDNS_Init_NoInitCallback              mDNSNULL
#define mDNS_Init_NoInitCallbackContext       mDNSNULL

extern void    mDNS_GrowCache (mDNS *const m, CacheEntity *storage, mDNSu32 numrecords);
extern void    mDNS_Close     (mDNS *const m);
extern mDNSs32 mDNS_Execute   (mDNS *const m);

extern mStatus mDNS_Register  (mDNS *const m, AuthRecord *const rr);
extern mStatus mDNS_Update    (mDNS *const m, AuthRecord *const rr, mDNSu32 newttl,
								const mDNSu16 newrdlength, RData *const newrdata, mDNSRecordUpdateCallback *Callback);
extern mStatus mDNS_Deregister(mDNS *const m, AuthRecord *const rr);

extern mStatus mDNS_StartQuery(mDNS *const m, DNSQuestion *const question);
extern mStatus mDNS_StopQuery (mDNS *const m, DNSQuestion *const question);
extern mStatus mDNS_StopQueryWithRemoves(mDNS *const m, DNSQuestion *const question);
extern mStatus mDNS_Reconfirm (mDNS *const m, CacheRecord *const cacherr);
extern mStatus mDNS_ReconfirmByValue(mDNS *const m, ResourceRecord *const rr);
extern mDNSs32 mDNS_TimeNow(const mDNS *const m);

// ***************************************************************************
#if 0
#pragma mark - Platform support functions that are accessible to the client layer too
#endif

extern mDNSs32  mDNSPlatformOneSecond;

// ***************************************************************************
#if 0
#pragma mark - General utility and helper functions
#endif

// mDNS_RegisterService is a single call to register the set of resource records associated with a given named service.
//
// mDNS_StartResolveService is single call which is equivalent to multiple calls to mDNS_StartQuery,
// to find the IP address, port number, and demultiplexing information for a given named service.
// As with mDNS_StartQuery, it executes asynchronously, and calls the ServiceInfoQueryCallback when the answer is
// found. After the service is resolved, the client should call mDNS_StopResolveService to complete the transaction.
// The client can also call mDNS_StopResolveService at any time to abort the transaction.
//
// mDNS_AddRecordToService adds an additional record to a Service Record Set.  This record may be deregistered
// via mDNS_RemoveRecordFromService, or by deregistering the service.  mDNS_RemoveRecordFromService is passed a
// callback to free the memory associated with the extra RR when it is safe to do so.  The ExtraResourceRecord
// object can be found in the record's context pointer.
	
// mDNS_GetBrowseDomains is a special case of the mDNS_StartQuery call, where the resulting answers
// are a list of PTR records indicating (in the rdata) domains that are recommended for browsing.
// After getting the list of domains to browse, call mDNS_StopQuery to end the search.
// mDNS_GetDefaultBrowseDomain returns the name of the domain that should be highlighted by default.
//
// mDNS_GetRegistrationDomains and mDNS_GetDefaultRegistrationDomain are the equivalent calls to get the list
// of one or more domains that should be offered to the user as choices for where they may register their service,
// and the default domain in which to register in the case where the user has made no selection.

extern void    mDNS_SetupResourceRecord(AuthRecord *rr, RData *RDataStorage, mDNSInterfaceID InterfaceID,
               mDNSu16 rrtype, mDNSu32 ttl, mDNSu8 RecordType, mDNSRecordCallback Callback, void *Context);

extern mStatus mDNS_RegisterService  (mDNS *const m, ServiceRecordSet *sr,
               const domainlabel *const name, const domainname *const type, const domainname *const domain,
               const domainname *const host, mDNSIPPort port, const mDNSu8 txtinfo[], mDNSu16 txtlen,
               AuthRecord *SubTypes, mDNSu32 NumSubTypes,
               const mDNSInterfaceID InterfaceID, mDNSServiceCallback Callback, void *Context);
extern mStatus mDNS_AddRecordToService(mDNS *const m, ServiceRecordSet *sr, ExtraResourceRecord *extra, RData *rdata, mDNSu32 ttl);
extern mStatus mDNS_RemoveRecordFromService(mDNS *const m, ServiceRecordSet *sr, ExtraResourceRecord *extra, mDNSRecordCallback MemFreeCallback, void *Context);
extern mStatus mDNS_RenameAndReregisterService(mDNS *const m, ServiceRecordSet *const sr, const domainlabel *newname);
extern mStatus mDNS_DeregisterService(mDNS *const m, ServiceRecordSet *sr);

extern mStatus mDNS_RegisterNoSuchService(mDNS *const m, AuthRecord *const rr,
               const domainlabel *const name, const domainname *const type, const domainname *const domain,
               const domainname *const host,
               const mDNSInterfaceID InterfaceID, mDNSRecordCallback Callback, void *Context);
#define        mDNS_DeregisterNoSuchService mDNS_Deregister

extern mStatus mDNS_StartBrowse(mDNS *const m, DNSQuestion *const question,
               const domainname *const srv, const domainname *const domain,
               const mDNSInterfaceID InterfaceID, mDNSBool ForceMCast, mDNSQuestionCallback *Callback, void *Context);
#define        mDNS_StopBrowse mDNS_StopQuery

extern mStatus mDNS_StartResolveService(mDNS *const m, ServiceInfoQuery *query, ServiceInfo *info, mDNSServiceInfoQueryCallback *Callback, void *Context);
extern void    mDNS_StopResolveService (mDNS *const m, ServiceInfoQuery *query);

typedef enum
	{
	mDNS_DomainTypeBrowse              = 0,
	mDNS_DomainTypeBrowseDefault       = 1,
	mDNS_DomainTypeBrowseLegacy        = 2,
	mDNS_DomainTypeRegistration        = 3,
	mDNS_DomainTypeRegistrationDefault = 4,
	
	mDNS_DomainTypeMax = 4
	} mDNS_DomainType;

extern const char *const mDNS_DomainTypeNames[];

extern mStatus mDNS_GetDomains(mDNS *const m, DNSQuestion *const question, mDNS_DomainType DomainType, const domainname *dom,
								const mDNSInterfaceID InterfaceID, mDNSQuestionCallback *Callback, void *Context);
#define        mDNS_StopGetDomains mDNS_StopQuery
extern mStatus mDNS_AdvertiseDomains(mDNS *const m, AuthRecord *rr, mDNS_DomainType DomainType, const mDNSInterfaceID InterfaceID, char *domname);
#define        mDNS_StopAdvertiseDomains mDNS_Deregister

extern mDNSOpaque16 mDNS_NewMessageID(mDNS * const m);

// ***************************************************************************
#if 0
#pragma mark - DNS name utility functions
#endif

// In order to expose the full capabilities of the DNS protocol (which allows any arbitrary eight-bit values
// in domain name labels, including unlikely characters like ascii nulls and even dots) all the mDNS APIs
// work with DNS's native length-prefixed strings. For convenience in C, the following utility functions
// are provided for converting between C's null-terminated strings and DNS's length-prefixed strings.

// Assignment
// A simple C structure assignment of a domainname can cause a protection fault by accessing unmapped memory,
// because that object is defined to be 256 bytes long, but not all domainname objects are truly the full size.
// This macro uses mDNSPlatformMemCopy() to make sure it only touches the actual bytes that are valid.
#define AssignDomainName(DST, SRC) mDNSPlatformMemCopy((SRC)->c, (DST)->c, DomainNameLength((SRC)))

// Comparison functions
#define SameDomainLabelCS(A,B) ((A)[0] == (B)[0] && mDNSPlatformMemSame((A), (B), (A)[0]))
extern mDNSBool SameDomainLabel(const mDNSu8 *a, const mDNSu8 *b);
extern mDNSBool SameDomainName(const domainname *const d1, const domainname *const d2);
extern mDNSBool SameDomainNameCS(const domainname *const d1, const domainname *const d2);
extern mDNSBool IsLocalDomain(const domainname *d);     // returns true for domains that by default should be looked up using link-local multicast

// Get total length of domain name, in native DNS format, including terminal root label
//   (e.g. length of "com." is 5 (length byte, three data bytes, final zero)
extern mDNSu16  DomainNameLengthLimit(const domainname *const name, const mDNSu8 *limit);
#define DomainNameLength(name) DomainNameLengthLimit((name), (name)->c + MAX_DOMAIN_NAME + 1)

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
// IMPORTANT:
// When using ConvertDomainLabelToCString, the target buffer must be MAX_ESCAPED_DOMAIN_LABEL (254) bytes long
// to guarantee there will be no buffer overrun. It is only safe to use a buffer shorter than this in rare cases
// where the label is known to be constrained somehow (for example, if the label is known to be either "_tcp" or "_udp").
// Similarly, when using ConvertDomainNameToCString, the target buffer must be MAX_ESCAPED_DOMAIN_NAME (1005) bytes long.
// See definitions of MAX_ESCAPED_DOMAIN_LABEL and MAX_ESCAPED_DOMAIN_NAME for more detailed explanation.
extern char    *ConvertDomainLabelToCString_withescape(const domainlabel *const name, char *cstr, char esc);
#define         ConvertDomainLabelToCString_unescaped(D,C) ConvertDomainLabelToCString_withescape((D), (C), 0)
#define         ConvertDomainLabelToCString(D,C)           ConvertDomainLabelToCString_withescape((D), (C), '\\')
extern char    *ConvertDomainNameToCString_withescape(const domainname *const name, char *cstr, char esc);
#define         ConvertDomainNameToCString_unescaped(D,C) ConvertDomainNameToCString_withescape((D), (C), 0)
#define         ConvertDomainNameToCString(D,C)           ConvertDomainNameToCString_withescape((D), (C), '\\')

extern void     ConvertUTF8PstringToRFC1034HostLabel(const mDNSu8 UTF8Name[], domainlabel *const hostlabel);

extern mDNSu8  *ConstructServiceName(domainname *const fqdn, const domainlabel *name, const domainname *type, const domainname *const domain);
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
#pragma mark - Other utility functions and macros
#endif

// mDNS_vsnprintf/snprintf return the number of characters written, excluding the final terminating null.
// The output is always null-terminated: for example, if the output turns out to be exactly buflen long,
// then the output will be truncated by one character to allow space for the terminating null.
// Unlike standard C vsnprintf/snprintf, they return the number of characters *actually* written,
// not the number of characters that *would* have been printed were buflen unlimited.
extern mDNSu32 mDNS_vsnprintf(char *sbuffer, mDNSu32 buflen, const char *fmt, va_list arg);
extern mDNSu32 mDNS_snprintf(char *sbuffer, mDNSu32 buflen, const char *fmt, ...) IS_A_PRINTF_STYLE_FUNCTION(3,4);
extern mDNSu32 NumCacheRecordsForInterfaceID(const mDNS *const m, mDNSInterfaceID id);
extern char *DNSTypeName(mDNSu16 rrtype);
extern char *GetRRDisplayString_rdb(const ResourceRecord *rr, RDataBody *rd, char *buffer);
#define RRDisplayString(m, rr) GetRRDisplayString_rdb(rr, &(rr)->rdata->u, (m)->MsgBuffer)
#define ARDisplayString(m, rr) GetRRDisplayString_rdb(&(rr)->resrec, &(rr)->resrec.rdata->u, (m)->MsgBuffer)
#define CRDisplayString(m, rr) GetRRDisplayString_rdb(&(rr)->resrec, &(rr)->resrec.rdata->u, (m)->MsgBuffer)
extern mDNSBool mDNSSameAddress(const mDNSAddr *ip1, const mDNSAddr *ip2);
extern void IncrementLabelSuffix(domainlabel *name, mDNSBool RichText);
extern mDNSBool IsPrivateV4Addr(mDNSAddr *addr);  // returns true for RFC1918 private addresses

#define mDNSSameIPv4Address(A,B) ((A).NotAnInteger == (B).NotAnInteger)
#define mDNSSameIPv6Address(A,B) ((A).l[0] == (B).l[0] && (A).l[1] == (B).l[1] && (A).l[2] == (B).l[2] && (A).l[3] == (B).l[3])
#define mDNSSameEthAddress(A,B)  ((A)->w[0] == (B)->w[0] && (A)->w[1] == (B)->w[1] && (A)->w[2] == (B)->w[2])

#define mDNSSameOpaque64(A,B)    ((A)->l[0] == (B)->l[0] && (A)->l[1] == (B)->l[1])
#define mDNSOpaque64IsZero(A)    ((A)->l[0] == 0 && (A)->l[1] == 0)

#define mDNSIPv4AddressIsZero(A) mDNSSameIPv4Address((A), zerov4Addr)
#define mDNSIPv6AddressIsZero(A) mDNSSameIPv6Address((A), zerov6Addr)

#define mDNSIPv4AddressIsOnes(A) mDNSSameIPv4Address((A), onesIPv4Addr)
#define mDNSIPv6AddressIsOnes(A) mDNSSameIPv6Address((A), onesIPv6Addr)

#define mDNSAddressIsAllDNSLinkGroup(X) (                                                            \
	((X)->type == mDNSAddrType_IPv4 && mDNSSameIPv4Address((X)->ip.v4, AllDNSLinkGroup_v4.ip.v4)) || \
	((X)->type == mDNSAddrType_IPv6 && mDNSSameIPv6Address((X)->ip.v6, AllDNSLinkGroup_v6.ip.v6))    )

#define mDNSAddressIsZero(X) (                                                \
	((X)->type == mDNSAddrType_IPv4 && mDNSIPv4AddressIsZero((X)->ip.v4))  || \
	((X)->type == mDNSAddrType_IPv6 && mDNSIPv6AddressIsZero((X)->ip.v6))     )

#define mDNSAddressIsValidNonZero(X) (                                        \
	((X)->type == mDNSAddrType_IPv4 && !mDNSIPv4AddressIsZero((X)->ip.v4)) || \
	((X)->type == mDNSAddrType_IPv6 && !mDNSIPv6AddressIsZero((X)->ip.v6))    )

#define mDNSAddressIsOnes(X) (                                                \
	((X)->type == mDNSAddrType_IPv4 && mDNSIPv4AddressIsOnes((X)->ip.v4))  || \
	((X)->type == mDNSAddrType_IPv6 && mDNSIPv6AddressIsOnes((X)->ip.v6))     )

#define mDNSAddressIsValid(X) (                                                                                             \
	((X)->type == mDNSAddrType_IPv4) ? !(mDNSIPv4AddressIsZero((X)->ip.v4) || mDNSIPv4AddressIsOnes((X)->ip.v4)) :          \
	((X)->type == mDNSAddrType_IPv6) ? !(mDNSIPv6AddressIsZero((X)->ip.v6) || mDNSIPv6AddressIsOnes((X)->ip.v6)) : mDNSfalse)

#define mDNSAddressIsv6LinkLocal(X) ((X)->type == mDNSAddrType_IPv6 && (X)->ip.v6.b[0] == 0xFE && ((X)->ip.v6.b[1] & 0xC0) == 0x80)

// ***************************************************************************
#if 0
#pragma mark - Authentication Support
#endif

// Unicast DNS and Dynamic Update specific Client Calls
//
// mDNS_SetSecretForDomain tells the core to authenticate (via TSIG with an HMAC_MD5 hash of the shared secret)
// when dynamically updating a given zone (and its subdomains).  The key used in authentication must be in
// domain name format.  The shared secret must be a null-terminated base64 encoded string.  A minimum size of
// 16 bytes (128 bits) is recommended for an MD5 hash as per RFC 2485.
// Calling this routine multiple times for a zone replaces previously entered values.  Call with a NULL key
// to dissable authentication for the zone.

extern mStatus mDNS_SetSecretForDomain(mDNS *m, DomainAuthInfo *info,
	const domainname *domain, const domainname *keyname, const char *b64keydata);

// Hostname/Unicast Interface Configuration

// All hostnames advertised point to one IPv4 address and/or one IPv6 address, set via SetPrimaryInterfaceInfo.  Invoking this routine
// updates all existing hostnames to point to the new address.
	
// A hostname is added via AddDynDNSHostName, which points to the primary interface's v4 and/or v6 addresss

// The status callback is invoked to convey success or failure codes - the callback should not modify the AuthRecord or free memory.
// Added hostnames may be removed (deregistered) via mDNS_RemoveDynDNSHostName.

// Host domains added prior to specification of the primary interface address and computer name will be deferred until
// these values are initialized.

// When routable V4 interfaces are added or removed, mDNS_UpdateLLQs should be called to re-estabish LLQs in case the
// destination address for events (i.e. the route) has changed.  For performance reasons, the caller is responsible for
// batching changes, e.g.  calling the routine only once if multiple interfaces are simultanously removed or added.

// DNS servers used to resolve unicast queries are specified by mDNS_AddDNSServer, and may later be removed via mDNS_DeleteDNSServers.
// For "split" DNS configurations, in which queries for different domains are sent to different servers (e.g. VPN and external),
// a domain may be associated with a DNS server.  For standard configurations, specify the root label (".") or NULL.
	
extern void mDNS_AddDynDNSHostName(mDNS *m, const domainname *fqdn, mDNSRecordCallback *StatusCallback, const void *StatusContext);
extern void mDNS_RemoveDynDNSHostName(mDNS *m, const domainname *fqdn);
extern void mDNS_SetPrimaryInterfaceInfo(mDNS *m, const mDNSAddr *v4addr,  const mDNSAddr *v6addr, const mDNSAddr *router);
extern void mDNS_UpdateLLQs(mDNS *m);
extern void mDNS_AddDNSServer(mDNS *const m, const mDNSAddr *dnsAddr, const domainname *domain);
extern void mDNS_DeleteDNSServers(mDNS *const m);

// Routines called by the core, exported by DNSDigest.c

// Convert an arbitrary base64 encoded key key into an HMAC key (stored in AuthInfo struct)
extern mDNSs32 DNSDigest_ConstructHMACKeyfromBase64(DomainAuthInfo *info, const char *b64key);

// sign a DNS message.  The message must be complete, with all values in network byte order.  end points to the end
// of the message, and is modified by this routine.  numAdditionals is a pointer to the number of additional
// records in HOST byte order, which is incremented upon successful completion of this routine.  The function returns
// the new end pointer on success, and NULL on failure.
extern mDNSu8 *DNSDigest_SignMessage(DNSMessage *msg, mDNSu8 **end, mDNSu16 *numAdditionals, DomainAuthInfo *info, mDNSu16 tcode);

// verify a DNS message.  The message must be complete, with all values in network byte order.  end points to the
// end of the record.  tsig is a pointer to the resource record that contains the TSIG OPT record.  info is
// the matching key to use for verifying the message.  This function expects that the additionals member
// of the DNS message header has already had one subtracted from it.
extern mDNSBool DNSDigest_VerifyMessage(DNSMessage * msg, mDNSu8 * end, LargeCacheRecord * tsig, DomainAuthInfo * info, mDNSu16 * rcode, mDNSu16 * tcode);


// ***************************************************************************
#if 0
#pragma mark - PlatformSupport interface
#endif

// This section defines the interface to the Platform Support layer.
// Normal client code should not use any of types defined here, or directly call any of the functions defined here.
// The definitions are placed here because sometimes clients do use these calls indirectly, via other supported client operations.
// For example, AssignDomainName is a macro defined using mDNSPlatformMemCopy()

// Every platform support module must provide the following functions.
// mDNSPlatformInit() typically opens a communication endpoint, and starts listening for mDNS packets.
// When Setup is complete, the platform support layer calls mDNSCoreInitComplete().
// mDNSPlatformSendUDP() sends one UDP packet
// When a packet is received, the PlatformSupport code calls mDNSCoreReceive()
// mDNSPlatformClose() tidies up on exit
//
// Note: mDNSPlatformMemAllocate/mDNSPlatformMemFree are only required for handling oversized resource records and unicast DNS.
// If your target platform has a well-defined specialized application, and you know that all the records it uses
// are InlineCacheRDSize or less, then you can just make a simple mDNSPlatformMemAllocate() stub that always returns
// NULL. InlineCacheRDSize is a compile-time constant, which is set by default to 64. If you need to handle records
// a little larger than this and you don't want to have to implement run-time allocation and freeing, then you
// can raise the value of this constant to a suitable value (at the expense of increased memory usage).
//
// USE CAUTION WHEN CALLING mDNSPlatformRawTime: The m->timenow_adjust correction factor needs to be added
// Generally speaking:
// Code that's protected by the main mDNS lock should just use the m->timenow value
// Code outside the main mDNS lock should use mDNS_TimeNow(m) to get properly adjusted time
// In certain cases there may be reasons why it's necessary to get the time without taking the lock first
// (e.g. inside the routines that are doing the locking and unlocking, where a call to get the lock would result in a
// recursive loop); in these cases use mDNS_TimeNow_NoLock(m) to get mDNSPlatformRawTime with the proper correction factor added.
//
// mDNSPlatformUTC returns the time, in seconds, since Jan 1st 1970 UTC and is required for generating TSIG records

extern mStatus  mDNSPlatformInit        (mDNS *const m);
extern void     mDNSPlatformClose       (mDNS *const m);
extern mStatus  mDNSPlatformSendUDP(const mDNS *const m, const void *const msg, const mDNSu8 *const end,
mDNSInterfaceID InterfaceID, const mDNSAddr *dst, mDNSIPPort dstport);

extern void     mDNSPlatformLock        (const mDNS *const m);
extern void     mDNSPlatformUnlock      (const mDNS *const m);

extern void     mDNSPlatformStrCopy     (const void *src,       void *dst);
extern mDNSu32  mDNSPlatformStrLen      (const void *src);
extern void     mDNSPlatformMemCopy     (const void *src,       void *dst, mDNSu32 len);
extern mDNSBool mDNSPlatformMemSame     (const void *src, const void *dst, mDNSu32 len);
extern void     mDNSPlatformMemZero     (                       void *dst, mDNSu32 len);
extern void *   mDNSPlatformMemAllocate (mDNSu32 len);
extern void     mDNSPlatformMemFree     (void *mem);
extern mDNSu32  mDNSPlatformRandomSeed  (void);
extern mStatus  mDNSPlatformTimeInit    (void);
extern mDNSs32  mDNSPlatformRawTime     (void);
extern mDNSs32  mDNSPlatformUTC         (void);
#define mDNS_TimeNow_NoLock(m) (mDNSPlatformRawTime() + m->timenow_adjust)

// Platform support modules should provide the following functions to map between opaque interface IDs
// and interface indexes in order to support the DNS-SD API. If your target platform does not support
// multiple interfaces and/or does not support the DNS-SD API, these functions can be empty.
extern mDNSInterfaceID mDNSPlatformInterfaceIDfromInterfaceIndex(mDNS *const m, mDNSu32 index);
extern mDNSu32 mDNSPlatformInterfaceIndexfromInterfaceID(mDNS *const m, mDNSInterfaceID id);

// Every platform support module must provide the following functions if it is to support unicast DNS
// and Dynamic Update.
// All TCP socket operations implemented by the platform layer MUST NOT BLOCK.
// mDNSPlatformTCPConnect initiates a TCP connection with a peer, adding the socket descriptor to the
// main event loop.  The return value indicates whether the connection succeeded, failed, or is pending
// (i.e. the call would block.)  On return, the descriptor parameter is set to point to the connected socket.
// The TCPConnectionCallback is subsequently invoked when the connection
// completes (in which case the ConnectionEstablished parameter is true), or data is available for
// reading on the socket (indicated by the ConnectionEstablished parameter being false.)  If the connection
// asynchronously fails, the TCPConnectionCallback should be invoked as usual, with the error being
// returned in subsequent calls to PlatformReadTCP or PlatformWriteTCP.  (This allows for platforms
// with limited asynchronous error detection capabilities.)  PlatformReadTCP and PlatformWriteTCP must
// return the number of bytes read/written, 0 if the call would block, and -1 if an error.  PlatformReadTCP
// should set the closed argument if the socket has been closed.
// PlatformTCPCloseConnection must close the connection to the peer and remove the descriptor from the
// event loop.  CloseConnectin may be called at any time, including in a ConnectionCallback.

typedef enum
	{
	kTCPSocketFlags_Zero   = 0,
	kTCPSocketFlags_UseTLS = (1 << 0)
	} uDNS_TCPSocketFlags;
	
typedef void (*TCPConnectionCallback)(uDNS_TCPSocket sock, void *context, mDNSBool ConnectionEstablished, mStatus err);
extern uDNS_TCPSocket mDNSPlatformTCPSocket(mDNS * const m, uDNS_TCPSocketFlags flags, mDNSIPPort * port);	// creates a tcp socket
extern uDNS_TCPSocket mDNSPlatformTCPAccept(uDNS_TCPSocketFlags flags, int sd);
extern int            mDNSPlatformTCPGetFlags(uDNS_TCPSocket sock);
extern int            mDNSPlatformTCPGetFD(uDNS_TCPSocket sock);
extern mStatus        mDNSPlatformTCPConnect(uDNS_TCPSocket sock, const mDNSAddr *dst, mDNSOpaque16 dstport, mDNSInterfaceID InterfaceID,
										  TCPConnectionCallback callback, void *context);
extern mDNSBool       mDNSPlatformTCPIsConnected(uDNS_TCPSocket sock);
extern void           mDNSPlatformTCPCloseConnection(uDNS_TCPSocket sock);
extern long           mDNSPlatformReadTCP(uDNS_TCPSocket sock, void *buf, unsigned long buflen, mDNSBool *closed);
extern long           mDNSPlatformWriteTCP(uDNS_TCPSocket sock, const char *msg, unsigned long len);
extern uDNS_UDPSocket mDNSPlatformUDPSocket(mDNS * const m, mDNSIPPort port);
extern void           mDNSPlatformUDPClose(uDNS_UDPSocket sock);

// mDNSPlatformTLSSetupCerts/mDNSPlatformTLSTearDownCerts used by dnsextd
extern mStatus        mDNSPlatformTLSSetupCerts(void);
extern void           mDNSPlatformTLSTearDownCerts(void);


// Platforms that support unicast browsing and dynamic update registration for clients who do not specify a domain
// in browse/registration calls must implement these routines to get the "default" browse/registration list.

extern void					mDNSPlatformGetDNSConfig(mDNS * const m, domainname *const fqdn, domainname *const regDomain, DNameListElem ** browseDomains);
extern IPAddrListElem	*	mDNSPlatformGetDNSServers(void);
extern DNameListElem    *   mDNSPlatformGetSearchDomainList(void);
extern DNameListElem	*	mDNSPlatformGetFQDN(void);
extern mStatus				mDNSPlatformGetPrimaryInterface(mDNS * const m, mDNSAddr * v4, mDNSAddr * v6, mDNSAddr * router);
extern DNameListElem	*	mDNSPlatformGetReverseMapSearchDomainList(void);
extern mStatus				mDNSPlatformRegisterSplitDNS(mDNS * const m, int * nAdditions, int * nDeletions);
extern void					mDNSPlatformDefaultRegDomainChanged(const domainname *d, mDNSBool add);
extern void					mDNSPlatformDynDNSHostNameStatusChanged(domainname *const dname, mStatus status);


// Helper functions provided by the core
extern void mDNS_FreeDNameList(DNameListElem *list);
extern void mDNS_FreeIPAddrList(IPAddrListElem * list);

#ifdef _LEGACY_NAT_TRAVERSAL_
// Support for legacy NAT traversal protocols, implemented by the platform layer and callable by the core.

#define DYN_PORT_MIN 49152 // ephemeral port range
#define DYN_PORT_MAX 65535
#define LEGACY_NATMAP_MAX_TRIES 4 // if our desired mapping is taken, how many times we try mapping to a random port

extern int     LNT_Init(void);
extern int     LNT_Destroy(void);
extern mStatus LNT_GetPublicIP(mDNSOpaque32 *ip);
extern mStatus LNT_MapPort(mDNSIPPort priv, mDNSIPPort pub, mDNSBool tcp);
extern mStatus LNT_UnmapPort(mDNSIPPort PubPort, mDNSBool tcp);
#endif // _LEGACY_NAT_TRAVERSAL_

// The core mDNS code provides these functions, for the platform support code to call at appropriate times
//
// mDNS_SetFQDN() is called once on startup (typically from mDNSPlatformInit())
// and then again on each subsequent change of the host name.
//
// mDNS_RegisterInterface() is used by the platform support layer to inform mDNSCore of what
// physical and/or logical interfaces are available for sending and receiving packets.
// Typically it is called on startup for each available interface, but register/deregister may be
// called again later, on multiple occasions, to inform the core of interface configuration changes.
// If set->Advertise is set non-zero, then mDNS_RegisterInterface() also registers the standard
// resource records that should be associated with every publicised IP address/interface:
// -- Name-to-address records (A/AAAA)
// -- Address-to-name records (PTR)
// -- Host information (HINFO)
// IMPORTANT: The specified mDNSInterfaceID MUST NOT be 0, -1, or -2; these values have special meaning
// mDNS_RegisterInterface does not result in the registration of global hostnames via dynamic update -
// see mDNS_SetPrimaryInterfaceInfo, mDNS_AddDynDNSHostName, etc. for this purpose.
// Note that the set may be deallocated immediately after it is deregistered via mDNS_DeegisterInterface.
//
// mDNS_RegisterDNS() is used by the platform support layer to provide the core with the addresses of
// available domain name servers for unicast queries/updates.  RegisterDNS() should be called once for
// each name server, typically at startup, or when a new name server becomes available.  DeregiterDNS()
// must be called whenever a registered name server becomes unavailable.  DeregisterDNSList deregisters
// all registered servers.  mDNS_DNSRegistered() returns true if one or more servers are registered in the core.
//
// mDNSCoreInitComplete() is called when the platform support layer is finished.
// Typically this is at the end of mDNSPlatformInit(), but may be later
// (on platforms like OT that allow asynchronous initialization of the networking stack).
//
// mDNSCoreReceive() is called when a UDP packet is received
//
// mDNSCoreMachineSleep() is called when the machine sleeps or wakes
// (This refers to heavyweight laptop-style sleep/wake that disables network access,
// not lightweight second-by-second CPU power management modes.)

extern void     mDNS_SetFQDN(mDNS *const m);
extern mStatus  mDNS_RegisterInterface  (mDNS *const m, NetworkInterfaceInfo *set, mDNSBool flapping);
extern void     mDNS_DeregisterInterface(mDNS *const m, NetworkInterfaceInfo *set, mDNSBool flapping);
extern void     mDNSCoreInitComplete(mDNS *const m, mStatus result);
extern void     mDNSCoreReceive(mDNS *const m, void *const msg, const mDNSu8 *const end,
								const mDNSAddr *const srcaddr, const mDNSIPPort srcport,
								const mDNSAddr *const dstaddr, const mDNSIPPort dstport, const mDNSInterfaceID InterfaceID);
extern void     mDNSCoreMachineSleep(mDNS *const m, mDNSBool wake);

extern mDNSBool mDNSAddrIsDNSMulticast(const mDNSAddr *ip);

// ***************************************************************************
#if 0
#pragma mark - Compile-Time assertion checks
#endif

// Some C compiler cleverness. We can make the compiler check certain things for
// us, and report compile-time errors if anything is wrong. The usual way to do
// this would be to use a run-time "if" statement, but then you don't find out
// what's wrong until you run the software. This way, if the assertion condition
// is false, the array size is negative, and the complier complains immediately.

struct mDNS_CompileTimeAssertionChecks
	{
	// Check that the compiler generated our on-the-wire packet format structure definitions
	// properly packed, without adding padding bytes to align fields on 32-bit or 64-bit boundaries.
	char assert0[(sizeof(rdataSRV)         == 262                          ) ? 1 : -1];
	char assert1[(sizeof(DNSMessageHeader) ==  12                          ) ? 1 : -1];
	char assert2[(sizeof(DNSMessage)       ==  12+AbsoluteMaxDNSMessageData) ? 1 : -1];
	char assert3[(sizeof(mDNSs8)           ==   1                          ) ? 1 : -1];
	char assert4[(sizeof(mDNSu8)           ==   1                          ) ? 1 : -1];
	char assert5[(sizeof(mDNSs16)          ==   2                          ) ? 1 : -1];
	char assert6[(sizeof(mDNSu16)          ==   2                          ) ? 1 : -1];
	char assert7[(sizeof(mDNSs32)          ==   4                          ) ? 1 : -1];
	char assert8[(sizeof(mDNSu32)          ==   4                          ) ? 1 : -1];
	char assert9[(sizeof(mDNSOpaque16)     ==   2                          ) ? 1 : -1];
	char assertA[(sizeof(mDNSOpaque32)     ==   4                          ) ? 1 : -1];
	char assertB[(sizeof(mDNSOpaque128)    ==  16                          ) ? 1 : -1];
	char assertC[(sizeof(CacheRecord  )    >=  sizeof(CacheGroup)          ) ? 1 : -1];
	char assertD[(sizeof(int)              >=  4                           ) ? 1 : -1];
	};

// ***************************************************************************

#ifdef __cplusplus
	}
#endif

#endif
