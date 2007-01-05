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

$Log: DNSCommon.h,v $
Revision 1.40  2007/01/05 08:30:40  cheshire
Trim excessive "$Log" checkin history from before 2006
(checkin history still available via "cvs log ..." of course)

Revision 1.39  2007/01/04 21:45:20  cheshire
Added mDNS_DropLockBeforeCallback/mDNS_ReclaimLockAfterCallback macros,
to do additional lock sanity checking around callback invocations

Revision 1.38  2006/12/22 20:59:49  cheshire
<rdar://problem/4742742> Read *all* DNS keys from keychain,
 not just key for the system-wide default registration domain

Revision 1.37  2006/08/14 23:24:22  cheshire
Re-licensed mDNSResponder daemon source code under Apache License, Version 2.0

Revision 1.36  2006/07/05 22:56:07  cheshire
<rdar://problem/4472014> Add Private DNS client functionality to mDNSResponder
Update mDNSSendDNSMessage() to use uDNS_TCPSocket type instead of "int"

Revision 1.35  2006/06/29 07:42:14  cheshire
<rdar://problem/3922989> Performance: Remove unnecessary SameDomainName() checks

Revision 1.34  2006/03/18 21:47:56  cheshire
<rdar://problem/4073825> Improve logic for delaying packets after repeated interface transitions

Revision 1.33  2006/03/10 21:51:41  cheshire
<rdar://problem/4111464> After record update, old record sometimes remains in cache
Split out SameRDataBody() into a separate routine so it can be called from other code

*/

#ifndef __DNSCOMMON_H_
#define __DNSCOMMON_H_

#include "mDNSEmbeddedAPI.h"

#ifdef	__cplusplus
	extern "C" {
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

typedef enum
	{
	TSIG_ErrBadSig  = 16,
	TSIG_ErrBadKey  = 17,
	TSIG_ErrBadTime = 18
	} TSIG_ErrorCode;
	

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - General Utility Functions
#endif

extern const NetworkInterfaceInfo *GetFirstActiveInterface(const NetworkInterfaceInfo *intf);
extern mDNSInterfaceID GetNextActiveInterfaceID(const NetworkInterfaceInfo *intf);

extern mDNSu32 mDNSRandom(mDNSu32 max);
extern mDNSu32 mDNSRandomFromFixedSeed(mDNSu32 seed, mDNSu32 max);


// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - Domain Name Utility Functions
#endif
	
#define mdnsIsDigit(X)     ((X) >= '0' && (X) <= '9')
#define mDNSIsUpperCase(X) ((X) >= 'A' && (X) <= 'Z')
#define mDNSIsLowerCase(X) ((X) >= 'a' && (X) <= 'z')
#define mdnsIsLetter(X)    (mDNSIsUpperCase(X) || mDNSIsLowerCase(X))
	
#define mdnsValidHostChar(X, notfirst, notlast) (mdnsIsLetter(X) || mdnsIsDigit(X) || ((notfirst) && (notlast) && (X) == '-') )

extern mDNSu16 CompressedDomainNameLength(const domainname *const name, const domainname *parent);

extern mDNSu32 TruncateUTF8ToLength(mDNSu8 *string, mDNSu32 length, mDNSu32 max);
extern mDNSBool LabelContainsSuffix(const domainlabel *const name, const mDNSBool RichText);
extern mDNSu32 RemoveLabelSuffix(domainlabel *name, mDNSBool RichText);
extern void AppendLabelSuffix(domainlabel *name, mDNSu32 val, mDNSBool RichText);
#define ValidateDomainName(N) (DomainNameLength(N) <= MAX_DOMAIN_NAME)


// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - Resource Record Utility Functions
#endif

extern mDNSu32 RDataHashValue(mDNSu16 const rdlength, const RDataBody *const rdb);

extern mDNSBool SameRDataBody(const ResourceRecord *const r1, const RDataBody *const r2);
extern mDNSBool SameRData(const ResourceRecord *const r1, const ResourceRecord *const r2);

extern mDNSBool ResourceRecordAnswersQuestion(const ResourceRecord *const rr, const DNSQuestion *const q);
extern mDNSBool SameNameRecordAnswersQuestion(const ResourceRecord *const rr, const DNSQuestion *const q);

extern mDNSBool SameResourceRecord(ResourceRecord *r1, ResourceRecord *r2);
	
extern mDNSu16 GetRDLength(const ResourceRecord *const rr, mDNSBool estimate);

#define GetRRDomainNameTarget(RR) (                                                                          \
	((RR)->rrtype == kDNSType_CNAME || (RR)->rrtype == kDNSType_PTR || (RR)->rrtype == kDNSType_NS)          \
	                                                                 ? &(RR)->rdata->u.name       :          \
	((RR)->rrtype == kDNSType_SRV                                  ) ? &(RR)->rdata->u.srv.target : mDNSNULL )

extern mDNSBool ValidateRData(const mDNSu16 rrtype, const mDNSu16 rdlength, const RData *const rd);
#define LocalRecordReady(X) ((X)->resrec.RecordType != kDNSRecordTypeUnique && (X)->resrec.RecordType != kDNSRecordTypeDeregistering)


// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark -
#pragma mark - DNS Message Creation Functions
#endif

extern void InitializeDNSMessage(DNSMessageHeader *h, mDNSOpaque16 id, mDNSOpaque16 flags);
extern const mDNSu8 *FindCompressionPointer(const mDNSu8 *const base, const mDNSu8 *const end, const mDNSu8 *const domname);

extern mDNSu8 *putDomainNameAsLabels(const DNSMessage *const msg, mDNSu8 *ptr, const mDNSu8 *const limit, const domainname *const name);
	
extern mDNSu8 *putRData(const DNSMessage *const msg, mDNSu8 *ptr, const mDNSu8 *const limit, ResourceRecord *rr);

// If we have a single large record to put in the packet, then we allow the packet to be up to 9K bytes,
// but in the normal case we try to keep the packets below 1500 to avoid IP fragmentation on standard Ethernet	

extern mDNSu8 *PutResourceRecordTTLWithLimit(DNSMessage *const msg, mDNSu8 *ptr, mDNSu16 *count, ResourceRecord *rr, mDNSu32 ttl, const mDNSu8 *limit);

#define PutResourceRecordTTL(msg, ptr, count, rr, ttl) PutResourceRecordTTLWithLimit((msg), (ptr), (count), (rr), (ttl), \
	((msg)->h.numAnswers || (msg)->h.numAuthorities || (msg)->h.numAdditionals) ? (msg)->data + NormalMaxDNSMessageData : (msg)->data + AbsoluteMaxDNSMessageData)

#define PutResourceRecordTTLJumbo(msg, ptr, count, rr, ttl) PutResourceRecordTTLWithLimit((msg), (ptr), (count), (rr), (ttl), \
	(msg)->data + AbsoluteMaxDNSMessageData)
			
extern mDNSu8 *PutResourceRecordCappedTTL(DNSMessage *const msg, mDNSu8 *ptr, mDNSu16 *count, ResourceRecord *rr, mDNSu32 maxttl);

extern mDNSu8 *putEmptyResourceRecord(DNSMessage *const msg, mDNSu8 *ptr, const mDNSu8 *const limit, mDNSu16 *count, const AuthRecord *rr);

extern mDNSu8 *putQuestion(DNSMessage *const msg, mDNSu8 *ptr, const mDNSu8 *const limit, const domainname *const name, mDNSu16 rrtype, mDNSu16 rrclass);

extern mDNSu8 *putZone(DNSMessage *const msg, mDNSu8 *ptr, mDNSu8 *limit, const domainname *zone, mDNSOpaque16 zoneClass);

extern mDNSu8 *putPrereqNameNotInUse(domainname *name, DNSMessage *msg, mDNSu8 *ptr, mDNSu8 *end);

extern mDNSu8 *putDeletionRecord(DNSMessage *msg, mDNSu8 *ptr, ResourceRecord *rr);

extern  mDNSu8 *putDeleteRRSet(DNSMessage *msg, mDNSu8 *ptr, const domainname *name, mDNSu16 rrtype);

extern mDNSu8 *putDeleteAllRRSets(DNSMessage *msg, mDNSu8 *ptr, const domainname *name);
	
extern mDNSu8 *putUpdateLease(DNSMessage *msg, mDNSu8 *end, mDNSu32 lease);
	
#define PutResourceRecord(MSG, P, C, RR) PutResourceRecordTTL((MSG), (P), (C), (RR), (RR)->rroriginalttl)


// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - DNS Message Parsing Functions
#endif

extern mDNSu32 DomainNameHashValue(const domainname *const name);

extern void SetNewRData(ResourceRecord *const rr, RData *NewRData, mDNSu16 rdlength);


extern const mDNSu8 *skipDomainName(const DNSMessage *const msg, const mDNSu8 *ptr, const mDNSu8 *const end);

extern const mDNSu8 *getDomainName(const DNSMessage *const msg, const mDNSu8 *ptr, const mDNSu8 *const end,
	domainname *const name);
	
extern const mDNSu8 *skipResourceRecord(const DNSMessage *msg, const mDNSu8 *ptr, const mDNSu8 *end);

extern const mDNSu8 *GetLargeResourceRecord(mDNS *const m, const DNSMessage * const msg, const mDNSu8 *ptr, 
    const mDNSu8 * end, const mDNSInterfaceID InterfaceID, mDNSu8 RecordType, LargeCacheRecord *largecr);

extern const mDNSu8 *skipQuestion(const DNSMessage *msg, const mDNSu8 *ptr, const mDNSu8 *end);

extern const mDNSu8 *getQuestion(const DNSMessage *msg, const mDNSu8 *ptr, const mDNSu8 *end, const mDNSInterfaceID InterfaceID,
	DNSQuestion *question);
	
extern const mDNSu8 *LocateAnswers(const DNSMessage *const msg, const mDNSu8 *const end);

extern const mDNSu8 *LocateAuthorities(const DNSMessage *const msg, const mDNSu8 *const end);

extern const mDNSu8 *LocateAdditionals(const DNSMessage *const msg, const mDNSu8 *const end);


// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark -
#pragma mark - Packet Sending Functions
#endif

extern mStatus mDNSSendDNSMessage(const mDNS *const m, DNSMessage *const msg, mDNSu8 *end,
	mDNSInterfaceID InterfaceID, const mDNSAddr *dst, mDNSIPPort dstport, uDNS_TCPSocket sock, DomainAuthInfo *authInfo);


// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - RR List Management & Task Management
#endif

extern void mDNS_Lock(mDNS *const m);
extern void mDNS_Unlock(mDNS *const m);

#define mDNS_DropLockBeforeCallback() do { m->mDNS_reentrancy++; \
	if (m->mDNS_busy != m->mDNS_reentrancy) \
		LogMsg("%s: Locking Failure! mDNS_busy (%ld) != mDNS_reentrancy (%ld)", __func__, m->mDNS_busy, m->mDNS_reentrancy); \
	} while (0)

#define mDNS_ReclaimLockAfterCallback() do { \
	if (m->mDNS_busy != m->mDNS_reentrancy) \
		LogMsg("%s: Unlocking Failure! mDNS_busy (%ld) != mDNS_reentrancy (%ld)", __func__, m->mDNS_busy, m->mDNS_reentrancy); \
	m->mDNS_reentrancy--; } while (0)

#ifdef	__cplusplus
	}
#endif

#endif // __DNSCOMMON_H_
