/*
	File:		mDNSWin32.h

	Contains:	Multicast DNS platform plugin implementation for Win32.
	
	Written by:	Bob Bradley
	
	Version:	mDNS Win32, September 2002

	Copyright:	(C) 2002 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		$Log: mDNSWin32.h,v $
		Revision 1.1  2002/09/19 04:33:39  cheshire
		Code received from Bob Bradley
		
		Revision 1.1  2002/09/18 01:18:19  bradley
		Multicast DNS platform plugin implementation for Win32.	
*/

#ifndef	__MDNS_WIN32__
#define	__MDNS_WIN32__

#define	_WIN32_WINDOWS		0x0401

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
	
	SocketRef					interfaceListChangedSocketRef;
	int							interfaceCount;
	mDNSInterfaceInfo *			interfaceList;
	HANDLE						timer;
	HANDLE						thread;
	mDNSBool					advertise;
};

#ifdef	__cplusplus
	}
#endif

#endif	// __MDNS_WIN32__
