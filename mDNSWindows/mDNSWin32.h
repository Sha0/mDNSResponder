/* -*- Mode: C; tab-width: 4 -*-
 *
 * Copyright (c) 2002-2004 Apple Computer, Inc. All rights reserved.
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
 */

#ifndef	__MDNS_WIN32__
#define	__MDNS_WIN32__

#include	"CommonServices.h"

#if( !defined( _WIN32_WCE ) )
	#include	<mswsock.h>
#endif

#include	"mDNSEmbeddedAPI.h"
#include	"uDNS.h"

#ifdef	__cplusplus
	extern "C" {
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@struct		mDNSInterfaceData

	@abstract	Structure containing interface-specific data.
*/

typedef struct	mDNSInterfaceData	mDNSInterfaceData;
struct	mDNSInterfaceData
{
	mDNSInterfaceData *			next;
	char						name[ 128 ];
	uint32_t					index;
	uint32_t					scopeID;
	SocketRef					sock;
#if( !defined( _WIN32_WCE ) )
	LPFN_WSARECVMSG				wsaRecvMsgFunctionPtr;
#endif
	HANDLE						readPendingEvent;
	NetworkInterfaceInfo		interfaceInfo;
	mDNSAddr					defaultAddr;
	mDNSBool					hostRegistered;
};

//---------------------------------------------------------------------------------------------------------------------------
/*!	@typedef	IdleThreadCallback

	@abstract	mDNSWin32 core will call out through this function pointer
				after calling mDNS_Execute
*/
typedef mDNSs32 (*IdleThreadCallback)(mDNS * const inMDNS, mDNSs32 interval);
//---------------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------------
/*!	@typedef	InterfaceListChangedCallback

	@abstract	mDNSWin32 core will call out through this function pointer
				after detecting an interface list changed event
*/
typedef void (*InterfaceListChangedCallback)(mDNS * const inMDNS);
//---------------------------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------------------------
/*!	@typedef	HostDescriptionChangedCallback

	@abstract	mDNSWin32 core will call out through this function pointer
				after detecting that the computer description has changed
*/
typedef void (*HostDescriptionChangedCallback)(mDNS * const inMDNS);
//---------------------------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------------------------
/*!	@struct		mDNS_PlatformSupport_struct

	@abstract	Structure containing platform-specific data.
*/

struct	mDNS_PlatformSupport_struct
{
	CRITICAL_SECTION			lock;
	mDNSBool					lockInitialized;
	HANDLE						cancelEvent;
	HANDLE						quitEvent;
	HANDLE						interfaceListChangedEvent;
	HANDLE						descChangedEvent;	// Computer description changed event
	HANDLE						tcpipChangedEvent;	// TCP/IP config changed
	HANDLE						ddnsChangedEvent;	// DynDNS config changed
	HANDLE						fileShareEvent;		// File Sharing changed
	HANDLE						firewallEvent;		// Firewall changed
	HANDLE						wakeupEvent;
	HANDLE						initEvent;
	time_t						nextDHCPLeaseExpires;
	HKEY						descKey;
	HKEY						tcpipKey;
	HKEY						ddnsKey;
	HKEY						fileShareKey;
	HKEY						firewallKey;
	mDNSBool					smbRegistered;
	ServiceRecordSet			smbSRS;
	mStatus						initStatus;
	mDNSBool					registeredLoopback4;
	SocketRef					interfaceListChangedSocket;
	int							interfaceCount;
	mDNSInterfaceData *			interfaceList;
	mDNSInterfaceData *			inactiveInterfaceList;
	DWORD						threadID;
	IdleThreadCallback			idleThreadCallback;
	InterfaceListChangedCallback	interfaceListChangedCallback;
	HostDescriptionChangedCallback	hostDescriptionChangedCallback;
	SocketRef						unicastSock4;
	HANDLE							unicastSock4ReadEvent;
	mDNSAddr						unicastSock4DestAddr;
#if( !defined( _WIN32_WCE ) )
	LPFN_WSARECVMSG					unicastSock4RecvMsgPtr;
#endif
	SocketRef						unicastSock6;
	HANDLE							unicastSock6ReadEvent;
	mDNSAddr						unicastSock6DestAddr;
#if( !defined( _WIN32_WCE ) )
	LPFN_WSARECVMSG					unicastSock6RecvMsgPtr;
#endif
};

//---------------------------------------------------------------------------------------------------------------------------
/*!	@struct		ifaddrs

	@abstract	Interface information
*/

struct ifaddrs
{
	struct ifaddrs *	ifa_next;
	char *				ifa_name;
	u_int				ifa_flags;
	struct sockaddr	*	ifa_addr;
	struct sockaddr	*	ifa_netmask;
	struct sockaddr	*	ifa_broadaddr;
	struct sockaddr	*	ifa_dstaddr;
	BOOL				ifa_dhcpEnabled;
	time_t				ifa_dhcpLeaseExpires;
	mDNSu8				ifa_womp;
	void *				ifa_data;
	
	struct
	{
		uint32_t		index;
	
	}	ifa_extra;
};


extern BOOL IsWOMPEnabled();


#ifdef	__cplusplus
	}
#endif

#endif	// __MDNS_WIN32__
