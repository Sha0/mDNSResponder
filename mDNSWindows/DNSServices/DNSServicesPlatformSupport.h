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
    
$Log: DNSServicesPlatformSupport.h,v $
Revision 1.6  2003/08/12 19:56:29  cheshire
Update to APSL 2.0

Revision 1.5  2003/07/02 21:20:10  cheshire
<rdar://problem/3313413> Update copyright notices, etc., in source code comments

Revision 1.4  2003/03/22 02:57:45  cheshire
Updated mDNSWindows to use new "mDNS_Execute" model (see "mDNSCore/Implementer Notes.txt")

Revision 1.3  2002/09/21 20:44:57  zarzycki
Added APSL info

Revision 1.2  2002/09/20 05:58:02  bradley
DNS Services for Windows

*/

#ifndef	__DNS_SERVICES_PLATFORM_SUPPORT__
#define	__DNS_SERVICES_PLATFORM_SUPPORT__

#include	"mDNSClientAPI.h"

#include	"DNSServices.h"

#ifdef	__cplusplus
	extern "C" {
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSPlatformInitialize
*/

DNSStatus	DNSPlatformInitialize( DNSFlags inFlags, DNSCount inCacheEntryCount, mDNS **outMDNS );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSPlatformFinalize
*/

void	DNSPlatformFinalize( void );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSPlatformMemAlloc
*/

DNSStatus	DNSPlatformMemAlloc( unsigned long inSize, void *outMem );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSPlatformMemFree
*/

void	DNSPlatformMemFree( void *inMem );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSPlatformLock
*/

void	DNSPlatformLock( void );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSPlatformUnlock
*/

void	DNSPlatformUnlock( void );

#ifdef	__cplusplus
	}
#endif

#endif	// __DNS_SERVICES_PLATFORM_SUPPORT__
