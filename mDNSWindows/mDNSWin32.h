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
    
        $Log: mDNSWin32.h,v $
        Revision 1.7  2003/07/23 02:23:01  cheshire
        Updated mDNSPlatformUnlock() to work correctly, now that <rdar://problem/3160248>
        "ScheduleNextTask needs to be smarter" has refined the way m->NextScheduledEvent is set

        Revision 1.6  2003/07/02 21:20:04  cheshire
        <rdar://problem/3313413> Update copyright notices, etc., in source code comments

        Revision 1.5  2003/04/29 00:06:09  cheshire
        <rdar://problem/3242673> mDNSWindows needs a wakeupEvent object to signal the main thread

        Revision 1.4  2003/03/22 02:57:44  cheshire
        Updated mDNSWindows to use new "mDNS_Execute" model (see "mDNSCore/Implementer Notes.txt")

        Revision 1.3  2002/09/21 20:44:54  zarzycki
        Added APSL info

        Revision 1.2  2002/09/20 05:55:16  bradley
        Multicast DNS platform plugin for Win32

*/

#ifndef	__MDNS_WIN32__
#define	__MDNS_WIN32__

#include	<winsock2.h>

#ifdef	__cplusplus
	extern "C" {
#endif

//===========================================================================================================================
//	Structures
//===========================================================================================================================

typedef SOCKET		SocketRef;

typedef struct	mDNSInterfaceInfo	mDNSInterfaceInfo;
struct	mDNSInterfaceInfo
{
	mDNSInterfaceInfo *			next;
	SocketRef					multicastSocketRef;
	HANDLE						multicastReadPendingEvent;
	SocketRef					unicastSocketRef;
	HANDLE						unicastReadPendingEvent;
	NetworkInterfaceInfo		hostSet;
	
	unsigned long				sendMulticastCounter;
	unsigned long				sendUnicastCounter;
	unsigned long				sendErrorCounter;
	
	unsigned long				recvMulticastCounter;
	unsigned long				recvUnicastCounter;
	unsigned long				recvErrorCounter;
};

struct	mDNS_PlatformSupport_struct
{
	// Synchronization Objects.
	
	CRITICAL_SECTION			lock;
	mDNSBool					lockInitialized;
	HANDLE						cancelEvent;
	HANDLE						quitEvent;
	HANDLE						interfaceListChangedEvent;
	HANDLE						wakeupEvent;
	
	SocketRef					interfaceListChangedSocketRef;
	int							interfaceCount;
	mDNSInterfaceInfo *			interfaceList;
	HANDLE						thread;
	mDNSBool					advertise;
	mDNSs32						nextWakeupTime;
};

#ifdef	__cplusplus
	}
#endif

#endif	// __MDNS_WIN32__
