/*
 * Copyright (c) 2002-2003 Apple Computer, Inc. All rights reserved.
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

$Log: DNSCommon.h,v $
Revision 1.1  2003/12/13 03:05:27  ksekar
Bug #: <rdar://problem/3192548>: DynDNS: Unicast query of service records

 
 */

#ifndef __DNSCOMMON_H_
#define __DNSCOMMON_H_

#include "mDNSClientAPI.h"



// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - General Utility Functions
#endif

mDNSexport const NetworkInterfaceInfo *GetFirstActiveInterface(const NetworkInterfaceInfo *intf);
mDNSexport mDNSInterfaceID GetNextActiveInterfaceID(const NetworkInterfaceInfo *intf);

mDNSexport mDNSu32 mDNSRandom(mDNSu32 max);
mDNSexport mDNSBool mDNSAddrIsDNSMulticast(const mDNSAddr *ip);

#define mDNSSameIPv4Address(A,B) ((A).NotAnInteger == (B).NotAnInteger)
#define mDNSSameIPv6Address(A,B) ((A).l[0] == (B).l[0] && (A).l[1] == (B).l[1] && (A).l[2] == (B).l[2] && (A).l[3] == (B).l[3])

#define mDNSIPv4AddressIsZero(A) mDNSSameIPv4Address((A), zeroIPAddr)
#define mDNSIPv6AddressIsZero(A) mDNSSameIPv6Address((A), zerov6Addr)

#define mDNSIPv4AddressIsOnes(A) mDNSSameIPv4Address((A), onesIPv4Addr)
#define mDNSIPv6AddressIsOnes(A) mDNSSameIPv6Address((A), onesIPv6Addr)

#define mDNSAddressIsZero(X) (                                              \
	((X)->type == mDNSAddrType_IPv4 && mDNSIPv4AddressIsZero((X)->ip.v4)) || \
	((X)->type == mDNSAddrType_IPv6 && mDNSIPv6AddressIsZero((X)->ip.v6))    )

#define mDNSAddressIsOnes(X) (                                              \
	((X)->type == mDNSAddrType_IPv4 && mDNSIPv4AddressIsOnes((X)->ip.v4)) || \
	((X)->type == mDNSAddrType_IPv6 && mDNSIPv6AddressIsOnes((X)->ip.v6))    )

#define mDNSAddressIsValid(X) (                                                                                             \
	((X)->type == mDNSAddrType_IPv4) ? !(mDNSIPv4AddressIsZero((X)->ip.v4) || mDNSIPv4AddressIsOnes((X)->ip.v4)) :          \
	((X)->type == mDNSAddrType_IPv6) ? !(mDNSIPv6AddressIsZero((X)->ip.v6) || mDNSIPv6AddressIsOnes((X)->ip.v6)) : mDNSfalse)



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

mDNSexport mDNSu16 CompressedDomainNameLength(const domainname *const name, const domainname *parent);

mDNSexport mDNSBool LabelContainsSuffix(const domainlabel *const name, const mDNSBool RichText);
mDNSexport mDNSu32 RemoveLabelSuffix(domainlabel *name, mDNSBool RichText);
mDNSexport void AppendLabelSuffix(domainlabel *name, mDNSu32 val, mDNSBool RichText);

#define ValidateDomainName(N) (DomainNameLength(N) <= MAX_DOMAIN_NAME)


// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - Resource Record Utility Functions
#endif

mDNSexport mDNSu32 RDataHashValue(mDNSu16 const rdlength, const RDataBody *const rdb);

mDNSexport mDNSBool SameRData(const ResourceRecord *const r1, const ResourceRecord *const r2);

mDNSexport mDNSBool ResourceRecordAnswersQuestion(const ResourceRecord *const rr, const DNSQuestion *const q);


mDNSexport mDNSu16 GetRDLength(const ResourceRecord *const rr, mDNSBool estimate);


#define GetRRDomainNameTarget(RR) (                                                                          \
	((RR)->rrtype == kDNSType_CNAME || (RR)->rrtype == kDNSType_PTR) ? &(RR)->rdata->u.name       :          \
	((RR)->rrtype == kDNSType_SRV                                  ) ? &(RR)->rdata->u.srv.target : mDNSNULL )


// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark -
#pragma mark - DNS Message Creation Functions
#endif

mDNSexport void InitializeDNSMessage(DNSMessageHeader *h, mDNSOpaque16 id, mDNSOpaque16 flags);
mDNSexport const mDNSu8 *FindCompressionPointer(const mDNSu8 *const base, const mDNSu8 *const end, const mDNSu8 *const domname);

mDNSexport mDNSu8 *putDomainNameAsLabels(const DNSMessage *const msg, mDNSu8 *ptr, const mDNSu8 *const limit, const domainname *const name);
	
mDNSexport mDNSu8 *putRData(const DNSMessage *const msg, mDNSu8 *ptr, const mDNSu8 *const limit, ResourceRecord *rr);

mDNSexport mDNSu8 *PutResourceRecordTTL(DNSMessage *const msg, mDNSu8 *ptr, mDNSu16 *count, ResourceRecord *rr, mDNSu32 ttl);

mDNSexport mDNSu8 *PutResourceRecordCappedTTL(DNSMessage *const msg, mDNSu8 *ptr, mDNSu16 *count, ResourceRecord *rr, mDNSu32 maxttl);

#if 0
mDNSexport mDNSu8 *putEmptyResourceRecord(DNSMessage *const msg, mDNSu8 *ptr, const mDNSu8 *const limit, mDNSu16 *count, const AuthRecord *rr);
#endif

mDNSexport mDNSu8 *putQuestion(DNSMessage *const msg, mDNSu8 *ptr, const mDNSu8 *const limit, const domainname *const name, mDNSu16 rrtype, mDNSu16 rrclass);

#define PutResourceRecord(MSG, P, C, RR) PutResourceRecordTTL((MSG), (P), (C), (RR), (RR)->rroriginalttl)


// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - DNS Message Parsing Functions
#endif

mDNSexport mDNSu32 DomainNameHashValue(const domainname *const name);

mDNSexport void SetNewRData(ResourceRecord *const rr, RData *NewRData, mDNSu16 rdlength);


mDNSexport const mDNSu8 *skipDomainName(const DNSMessage *const msg, const mDNSu8 *ptr, const mDNSu8 *const end);

mDNSexport const mDNSu8 *getDomainName(const DNSMessage *const msg, const mDNSu8 *ptr, const mDNSu8 *const end,
	domainname *const name);
	
mDNSexport const mDNSu8 *skipResourceRecord(const DNSMessage *msg, const mDNSu8 *ptr, const mDNSu8 *end);

mDNSexport const mDNSu8 *GetResourceRecord(mDNS *const m, const DNSMessage * const msg, const mDNSu8 *ptr, 
    const mDNSu8 * const end, const mDNSInterfaceID InterfaceID, mDNSu8 RecordType, CacheRecord *rr, RData *RDataStorage);

mDNSexport const mDNSu8 *skipQuestion(const DNSMessage *msg, const mDNSu8 *ptr, const mDNSu8 *end);

mDNSexport const mDNSu8 *getQuestion(const DNSMessage *msg, const mDNSu8 *ptr, const mDNSu8 *end, const mDNSInterfaceID InterfaceID,
	DNSQuestion *question);
	
mDNSexport const mDNSu8 *LocateAnswers(const DNSMessage *const msg, const mDNSu8 *const end);

mDNSexport const mDNSu8 *LocateAuthorities(const DNSMessage *const msg, const mDNSu8 *const end);

#define GetLargeResourceRecord(m, msg, p, e, i, t, L) \
	(((L)->r.rdatastorage.MaxRDLength = MaximumRDSize), GetResourceRecord((m), (msg), (p), (e), (i), (t), &(L)->r, (RData*)&(L)->r.rdatastorage))

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark -
#pragma mark - Packet Sending Functions
#endif

mDNSexport mStatus mDNSSendDNSMessage(const mDNS *const m, DNSMessage *const msg, const mDNSu8 *const end, mDNSInterfaceID InterfaceID, mDNSIPPort srcport, const mDNSAddr *dst, mDNSIPPort dstport);

#endif // __DNSCOMMON_H_