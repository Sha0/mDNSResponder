/*
 * Copyright (c) 2002-2003 Apple Computer, Inc. All rights reserved.
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

    Change History (most recent first):

$Log: mDNSPlatformFunctions.h,v $
Revision 1.17  2003/07/19 03:15:15  cheshire
Add generic MemAllocate/MemFree prototypes to mDNSPlatformFunctions.h,
and add the obvious trivial implementations to each platform support layer

Revision 1.16  2003/07/02 21:19:46  cheshire
<rdar://problem/3313413> Update copyright notices, etc., in source code comments

Revision 1.15  2003/05/23 22:39:45  cheshire
<rdar://problem/3268151> Need to adjust maximum packet size for IPv6

Revision 1.14  2003/04/28 21:54:57  cheshire
Fix compiler warning

Revision 1.13  2003/03/15 04:40:36  cheshire
Change type called "mDNSOpaqueID" to the more descriptive name "mDNSInterfaceID"

Revision 1.12  2003/02/21 01:54:08  cheshire
Bug #: 3099194 mDNSResponder needs performance improvements
Switched to using new "mDNS_Execute" model (see "Implementer Notes.txt")

Revision 1.11  2002/12/23 22:13:29  jgraessl

Reviewed by: Stuart Cheshire
Initial IPv6 support for mDNSResponder.

Revision 1.10  2002/09/21 20:44:49  zarzycki
Added APSL info

Revision 1.9  2002/09/19 04:20:43  cheshire
Remove high-ascii characters that confuse some systems

Revision 1.8  2002/09/16 23:12:14  cheshire
Minor code tidying

Revision 1.7  2002/09/16 18:41:42  cheshire
Merge in license terms from Quinn's copy, in preparation for Darwin release

*/

#ifndef __mDNSPlatformFunctions_h
#define __mDNSPlatformFunctions_h

// ***************************************************************************
// Support functions which must be provided by each set of specific PlatformSupport files

// mDNSPlatformInit() typically opens a communication endpoint, and starts listening for mDNS packets.
// When Setup is complete, the callback is called.
// mDNSPlatformSendUDP() sends one UDP packet
// When a packet is received, the PlatformSupport code calls mDNSCoreReceive()
// mDNSPlatformClose() tidies up on exit

#ifdef	__cplusplus
	extern "C" {
#endif

// ***************************************************************************
// DNS protocol message format

typedef struct
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
typedef struct
	{
	DNSMessageHeader h;						// Note: Size 12 bytes
	mDNSu8 data[AbsoluteMaxDNSMessageData];	// 40 (IPv6) + 8 (UDP) + 12 (DNS header) + 8940 (data) = 9000
	} DNSMessage;

// ***************************************************************************
// Functions

// Every platform support module must provide the following functions.
// Note: mDNSPlatformMemAllocate/mDNSPlatformMemFree are only required for handling oversized resource records.
// If your target platform has a well-defined specialized application, and you know that all the records it uses
// are StandardRDSize or less, then you can just make a simple mDNSPlatformMemAllocate() stub that always returns
// NULL. StandardRDSize is a compile-time constant, which is set by default to 264. If you need to handle records
// a little larger than this and you don't want to have to implement run-time allocation and freeing, then you
// can raise the value of this constant to a suitable value (at the expense of increased memory usage).
extern mStatus  mDNSPlatformInit   (mDNS *const m);
extern void     mDNSPlatformClose  (mDNS *const m);
extern mStatus  mDNSPlatformSendUDP(const mDNS *const m, const DNSMessage *const msg, const mDNSu8 *const end,
	mDNSInterfaceID InterfaceID, mDNSIPPort srcport, const mDNSAddr *dst, mDNSIPPort dstport);

extern void     mDNSPlatformLock        (const mDNS *const m);
extern void     mDNSPlatformUnlock      (const mDNS *const m);

extern void     mDNSPlatformStrCopy     (const void *src,       void *dst);
extern mDNSu32  mDNSPlatformStrLen      (const void *src);
extern void     mDNSPlatformMemCopy     (const void *src,       void *dst, mDNSu32 len);
extern mDNSBool mDNSPlatformMemSame     (const void *src, const void *dst, mDNSu32 len);
extern void     mDNSPlatformMemZero     (                       void *dst, mDNSu32 len);
extern void *   mDNSPlatformMemAllocate (mDNSu32 len);
extern void     mDNSPlatformMemFree     (void *mem);

// The core mDNS code provides these functions, for the platform support code to call at appropriate times
extern void     mDNSCoreInitComplete(mDNS *const m, mStatus result);
extern void     mDNSCoreReceive(mDNS *const m, DNSMessage *const msg, const mDNSu8 *const end,
								const mDNSAddr *const srcaddr, const mDNSIPPort srcport,
								const mDNSAddr *const dstaddr, const mDNSIPPort dstport, const mDNSInterfaceID InterfaceID);
extern void     mDNSCoreMachineSleep(mDNS *const m, mDNSBool wake);

#ifdef	__cplusplus
	}
#endif

#endif
