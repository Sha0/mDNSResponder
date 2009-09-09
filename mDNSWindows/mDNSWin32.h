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

typedef void ( *TCPReadEventHandler )( TCPSocket * sock );
typedef void ( *TCPUserCallback )();

struct TCPSocket_struct
{
	TCPSocketFlags				flags;		// MUST BE FIRST FIELD -- mDNSCore expects every TCPSocket_struct to begin with TCPSocketFlags flags
	SOCKET						fd;
	TCPReadEventHandler			readEventHandler;
	HANDLE						connectEvent;
	BOOL						connected;
	TCPUserCallback				userCallback;
	void					*	userContext;
	DWORD						lastError;
	BOOL						closed;
	OVERLAPPED					overlapped;
	WSABUF						wbuf;
	uint8_t						buf[ 4192 ];
	uint8_t					*	bptr;
	uint8_t					*	eptr;
	mDNS					*	m;
};


struct UDPSocket_struct
{
	mDNSIPPort						port; 			// MUST BE FIRST FIELD -- mDNSCoreReceive expects every UDPSocket_struct to begin with mDNSIPPort port
	mDNSAddr						addr;			// This is initialized by our code. If we don't get the 
													// dstAddr from WSARecvMsg we use this value instead.
	SOCKET							fd;
	LPFN_WSARECVMSG					recvMsgPtr;
	OVERLAPPED						overlapped;
	WSAMSG							wmsg;
	WSABUF							wbuf;
	DNSMessage						packet;
	uint8_t							controlBuffer[ 128 ];
	struct sockaddr_storage			srcAddr;		// This is filled in by the WSARecv* function
	INT								srcAddrLen;		// See above
	struct mDNSInterfaceData	*	ifd;
	UDPSocket					*	next;
	mDNS						*	m;
};


//---------------------------------------------------------------------------------------------------------------------------
/*!	@struct		mDNSInterfaceData

	@abstract	Structure containing interface-specific data.
*/

typedef struct	mDNSInterfaceData	mDNSInterfaceData;
struct	mDNSInterfaceData
{
	char						name[ 128 ];
	uint32_t					index;
	uint32_t					scopeID;
	UDPSocket					sock;
	NetworkInterfaceInfo		interfaceInfo;
	mDNSBool					hostRegistered;
	mDNSInterfaceData		*	next;
};


//---------------------------------------------------------------------------------------------------------------------------
/*!	@typedef	RegisterWaitableEventHandler
*/
typedef void		(*RegisterWaitableEventHandler)(mDNS * const inMDNS, HANDLE event, void * context );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@typedef	RegisterWaitableEventFunc
*/
typedef mStatus		(*RegisterWaitableEventFunc)(mDNS * const inMDNS, HANDLE event, void * context, RegisterWaitableEventHandler handler );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@typedef	UnregisterWaitableEventHandler
*/
typedef void		(*UnregisterWaitableEventFunc)(mDNS * const inMDNS, HANDLE event );


//---------------------------------------------------------------------------------------------------------------------------
/*!	@struct		mDNS_PlatformSupport_struct

	@abstract	Structure containing platform-specific data.
*/

struct	mDNS_PlatformSupport_struct
{
	HANDLE						mainThread;
	RegisterWaitableEventFunc	registerWaitableEventFunc;
	UnregisterWaitableEventFunc	unregisterWaitableEventFunc;
	time_t						nextDHCPLeaseExpires;
	mDNSBool					smbRegistered;
	ServiceRecordSet			smbSRS;
	mDNSBool					registeredLoopback4;
	int							interfaceCount;
	mDNSInterfaceData *			interfaceList;
	mDNSInterfaceData *			inactiveInterfaceList;
	UDPSocket					unicastSock4;
	UDPSocket					unicastSock6;
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
extern mStatus  TCPAddSocket( mDNS * const inMDNS, TCPSocket *sock );
extern mStatus	SetupInterfaceList( mDNS * const inMDNS );
extern mStatus	TearDownInterfaceList( mDNS * const inMDNS );
extern BOOL		IsWOMPEnabled();


#ifdef	__cplusplus
	}
#endif

#endif	// __MDNS_WIN32__
