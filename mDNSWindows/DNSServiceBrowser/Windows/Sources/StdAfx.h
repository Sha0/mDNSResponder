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
    
$Log: StdAfx.h,v $
Revision 1.1  2004/06/18 04:04:36  rpantos
Move up one level

Revision 1.3  2004/01/30 02:56:32  bradley
Updated to support full Unicode display. Added support for all services on www.dns-sd.org.

Revision 1.2  2003/10/09 02:31:55  bradley
Define WINVER if not already defined to avoid warning with Visual Studio .NET 2003.

Revision 1.1  2003/08/21 02:06:47  bradley
Moved Rendezvous Browser for non-Windows CE into Windows sub-folder.

Revision 1.4  2003/08/12 19:56:28  cheshire
Update to APSL 2.0

Revision 1.3  2003/07/02 21:20:06  cheshire
<rdar://problem/3313413> Update copyright notices, etc., in source code comments

Revision 1.2  2002/09/21 20:44:56  zarzycki
Added APSL info

Revision 1.1  2002/09/20 06:12:53  bradley
Rendezvous Browser for Windows

*/

#if !defined(AFX_STDAFX_H__424305D2_0A97_4AA0_B9B1_A7D90D18EBA0__INCLUDED_)
#define AFX_STDAFX_H__424305D2_0A97_4AA0_B9B1_A7D90D18EBA0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
	#define WINVER 0x0400	// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#include	<afxwin.h>		// MFC core and standard components
#include	<afxext.h>		// MFC extensions
#include	<afxdtctl.h>	// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
	#include	<afxcmn.h>	// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include	<winsock2.h>

#include	<stdlib.h>

#include	"DNSServices.h"

#include	"Application.h"

#include	"ChooserDialog.h"

#endif // !defined(AFX_STDAFX_H__424305D2_0A97_4AA0_B9B1_A7D90D18EBA0__INCLUDED_)
