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
	mDNS					*	m;
	char						name[ 128 ];
	uint32_t					index;
	uint32_t					scopeID;
	SocketRef					sock;
#if( !defined( _WIN32_WCE ) )
	LPFN_WSARECVMSG				wsaRecvMsgFunctionPtr;
#endif
	struct sockaddr_storage		srcAddr;
	INT							srcAddrLen;
	uint8_t						controlBuffer[ 128 ];
	DNSMessage					packet;
	OVERLAPPED					overlapped;
	WSAMSG						wmsg;
	WSABUF						wbuf;
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
	HANDLE						mainThread;
	time_t						nextDHCPLeaseExpires;
	mDNSBool					smbRegistered;
	ServiceRecordSet			smbSRS;
	mDNSBool					registeredLoopback4;
	SocketRef					interfaceListChangedSocket;
	int							interfaceCount;
	mDNSInterfaceData *			interfaceList;
	mDNSInterfaceData *			inactiveInterfaceList;
	SocketRef						unicastSock4;
	HANDLE							unicastSock4ReadEvent;
	mDNSAddr						unicastSock4DestAddr;
#if( !defined( _WIN32_WCE ) )
	LPFN_WSARECVMSG					unicastSock4RecvMsgPtr;
#endif
	struct sockaddr_storage			unicastSock4SrcAddr;
	INT								unicastSock4SrcAddrLen;
	uint8_t							unicastSock4ControlBuffer[ 128 ];
	DNSMessage						unicastSock4Packet;
	OVERLAPPED						unicastSock4Overlapped;
	WSAMSG							unicastSock4WMsg;
	WSABUF							unicastSock4WBuf;
	SocketRef						unicastSock6;
	HANDLE							unicastSock6ReadEvent;
	mDNSAddr						unicastSock6DestAddr;
#if( !defined( _WIN32_WCE ) )
	LPFN_WSARECVMSG					unicastSock6RecvMsgPtr;
#endif
	struct sockaddr_storage			unicastSock6SrcAddr;
	INT								unicastSock6SrcAddrLen;
	uint8_t							unicastSock6ControlBuffer[ 128 ];
	DNSMessage						unicastSock6Packet;
	OVERLAPPED						unicastSock6Overlapped;
	WSAMSG							unicastSock6WMsg;
	WSABUF							unicastSock6WBuf;
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


extern void		InterfaceListDidChange( mDNS * const inMDNS );
extern void		ComputerDescriptionDidChange( mDNS * const inMDNS );
extern void		TCPIPConfigDidChange( mDNS * const inMDNS );
extern void		DynDNSConfigDidChange( mDNS * const inMDNS );
extern void		FileSharingDidChange( mDNS * const inMDNS );
extern void		FirewallDidChange( mDNS * const inMDNS );
extern mStatus	SetupInterfaceList( mDNS * const inMDNS );
extern mStatus	TearDownInterfaceList( mDNS * const inMDNS );
extern BOOL		IsWOMPEnabled();


#ifdef	__cplusplus
	}
#endif

#endif	// __MDNS_WIN32__
