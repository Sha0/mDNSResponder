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

$Log: uDNS.h,v $
Revision 1.4  2004/02/06 23:04:19  ksekar
Basic Dynamic Update support via mDNS_Register (dissabled via
UNICAST_REGISTRATION #define)

Revision 1.3  2004/01/24 03:38:27  cheshire
Fix minor syntactic error: Headers should use "extern" declarations, not "mDNSexport"

Revision 1.2  2004/01/23 23:23:15  ksekar
Added TCP support for truncated unicast messages.

Revision 1.1  2003/12/13 03:05:27  ksekar
Bug #: <rdar://problem/3192548>: DynDNS: Unicast query of service records

 
 */

#ifndef __UDNS_H_
#define __UDNS_H_

#include "mDNSClientAPI.h"
#include "DNSCommon.h"

// Entry points into unicast-specific routines, invoked in mDNS.c

extern mStatus uDNS_StartQuery(mDNS *const m, DNSQuestion *const question);
extern mDNSBool IsActiveUnicastQuery(DNSQuestion *const question, uDNS_data_t *u);  // returns true if OK to call StopQuery
extern mStatus uDNS_StopQuery(mDNS *const m, DNSQuestion *const question);

extern mStatus uDNS_Register(mDNS *const m, AuthRecord *const rr);
extern mStatus uDNS_Deregister(mDNS *const m, AuthRecord *const rr);


// integer fields of msg header must be in HOST byte order before calling this routine
extern void uDNS_ReceiveMsg(mDNS *const m, DNSMessage *const msg, const mDNSu8 *const end,
	const mDNSAddr *const srcaddr, const mDNSIPPort srcport, const mDNSAddr *const dstaddr, 
	const mDNSIPPort dstport, const mDNSInterfaceID InterfaceID, mDNSu8 ttl);

// return the time of the next schedule unicast-specific event
extern mDNSs32 uDNS_GetNextEventTime(const mDNS *const m);

#endif // __UDNS_H_
