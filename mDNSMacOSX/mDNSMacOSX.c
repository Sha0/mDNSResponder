/*
 * Copyright (c) 2002 Apple Computer, Inc. All rights reserved.
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

$Log: mDNSMacOSX.c,v $
Revision 1.52  2003/01/28 19:39:43  jgraessl

Enabling AAAA over IPv4 support.

Revision 1.51  2003/01/28 05:11:23  cheshire
Fixed backwards comparison in SearchForInterfaceByName

Revision 1.50  2003/01/13 23:49:44  jgraessl
Merged changes for the following fixes in to top of tree:
3086540  computer name changes not handled properly
3124348  service name changes are not properly handled
3124352  announcements sent in pairs, failing chattiness test

Revision 1.49  2002/12/23 22:13:30  jgraessl

Reviewed by: Stuart Cheshire
Initial IPv6 support for mDNSResponder.

Revision 1.48  2002/11/22 01:37:52  cheshire
Bug #: 3108426 mDNSResponder is monitoring ServiceEntities instead of InterfaceEntities

Revision 1.47  2002/09/21 20:44:51  zarzycki
Added APSL info

Revision 1.46  2002/09/19 21:25:35  cheshire
mDNS_sprintf() doesn't need to be in a separate file

Revision 1.45  2002/09/17 01:45:13  cheshire
Add LIST_ALL_INTERFACES symbol for debugging

Revision 1.44  2002/09/17 01:36:23  cheshire
Move Puma support to CFSocketPuma.c

Revision 1.43  2002/09/17 01:05:28  cheshire
Change mDNS_AdvertiseLocalAddresses to be an Init parameter instead of a global

Revision 1.42  2002/09/16 23:13:50  cheshire
Minor code tidying

 */

// ***************************************************************************
// mDNS-CFSocket.c:
// Supporting routines to run mDNS on a CFRunLoop platform
// ***************************************************************************

// Open Transport 2.7.x on Mac OS 9 used to send Multicast DNS queries to UDP port 53,
// before the Multicast DNS port was changed to 5353. For this reason, the mDNSResponder
// in earlier versions of Mac OS X 10.2 Jaguar used to set mDNS_AllowPort53 to 1 to allow
// it to also listen and answer queries on UDP port 53. Now that Transport 2.8 (included in
// the Classic subsystem of Mac OS X 10.2 Jaguar) has been corrected to issue Multicast DNS
// queries on UDP port 5353, this backwards-compatibility legacy support is no longer needed.
#define mDNS_AllowPort53 1

// For debugging, set LIST_ALL_INTERFACES to 1 to display all found interfaces,
// including ones that mDNSResponder chooses not to use.
#define LIST_ALL_INTERFACES 0

// For enabling AAAA records over IPv4. Setting this to 0 sends only
// A records over IPv4 and AAAA over IPv6. Setting this to 1 sends both
// AAAA and A records over both IPv4 and IPv6.
#define AAAA_OVER_V4	1

void (*NotifyClientNetworkChanged)(void);

#include "mDNSClientAPI.h"          // Defines the interface provided to the client layer above
#include "mDNSPlatformFunctions.h"	// Defines the interface to the supporting layer below
#include "mDNSMacOSX.h"				// Defines the specific types needed to run mDNS on this platform

#include <stdio.h>
#include <stdarg.h>                  // For va_list support
#include <net/if.h>
#include <net/if_dl.h>
#include <sys/uio.h>
#include <sys/param.h>
#include <sys/socket.h>

// Code contributed by Dave Heller:
// Define RUN_ON_PUMA_WITHOUT_IFADDRS to compile code that will
// work on Mac OS X 10.1, which does not have the getifaddrs call.
#define RUN_ON_PUMA_WITHOUT_IFADDRS 0
#if RUN_ON_PUMA_WITHOUT_IFADDRS
#include "CFSocketPuma.c"
#else
#include <ifaddrs.h>
#endif

#include <IOKit/IOKitLib.h>
#include <IOKit/IOMessage.h>

// ***************************************************************************
// Structures

typedef struct NetworkInterfaceInfo2_struct NetworkInterfaceInfo2;
struct NetworkInterfaceInfo2_struct
	{
	NetworkInterfaceInfo ifinfo;
	mDNS *m;
	char *ifa_name;
	int socket;
	CFSocketRef cfsocket;
	int v6socket;
	CFSocketRef	v6cfsocket;
#if mDNS_AllowPort53
	int socket53;
	CFSocketRef cfsocket53;
#endif
	};

// ***************************************************************************
// Functions

// Note, this uses mDNS_vsprintf instead of standard "vsprintf", because mDNS_vsprintf knows
// how to print special data types like IP addresses and length-prefixed domain names
mDNSexport void debugf_(const char *format, ...)
	{
	unsigned char buffer[512];
	va_list ptr;
	va_start(ptr,format);
	buffer[mDNS_vsprintf((char *)buffer, format, ptr)] = 0;
	va_end(ptr);
	fprintf(stderr, "%s\n", buffer);
	fflush(stderr);
	}

mDNSexport void verbosedebugf_(const char *format, ...)
	{
	unsigned char buffer[512];
	va_list ptr;
	va_start(ptr,format);
	buffer[mDNS_vsprintf((char *)buffer, format, ptr)] = 0;
	va_end(ptr);
	fprintf(stderr, "%s\n", buffer);
	fflush(stderr);
	}

mDNSexport mStatus mDNSPlatformSendUDP(const mDNS *const m, const DNSMessage *const msg, const mDNSu8 *const end,
	mDNSOpaqueID InterfaceID, mDNSIPPort srcPort, const mDNSAddr *dst, mDNSIPPort dstPort)
	{
	#pragma unused(m)
	NetworkInterfaceInfo2 *info = (NetworkInterfaceInfo2 *)(InterfaceID);
	struct sockaddr_storage to;
	int s, err;

	if (InterfaceID == 0) { debugf("mDNSPlatformSendUDP ERROR! Cannot send from zero source address"); return mStatus_BadParamErr; }

	if (dst->type == mDNSAddrType_IPv4)
		{
		struct sockaddr_in*	sin_to = (struct sockaddr_in*)&to;
		sin_to->sin_len			= sizeof(*sin_to);
		sin_to->sin_family      = AF_INET;
		sin_to->sin_port        = dstPort.NotAnInteger;
		sin_to->sin_addr.s_addr = dst->addr.ipv4.NotAnInteger;
		}
	else if (dst->type == mDNSAddrType_IPv6)
		{
		struct sockaddr_in6* sin6_to = (struct sockaddr_in6*)&to;
		sin6_to->sin6_len		= sizeof(*sin6_to);
		sin6_to->sin6_family	= AF_INET6;
		sin6_to->sin6_port		= dstPort.NotAnInteger;
		sin6_to->sin6_flowinfo	= 0;
		sin6_to->sin6_addr		= *(struct in6_addr*)&dst->addr.ipv6;
		sin6_to->sin6_scope_id	= if_nametoindex(info->ifa_name);
		}
	else
		{
		debugf("mDNSPlatformSendUDP: dst is not an IPv4 or IPv6 address!\n");
		return mStatus_BadParamErr;
		}

	debugf("mDNSPlatformSendUDP: sending on InterfaceID %X/%d", InterfaceID, dst->type);

	if (srcPort.NotAnInteger == MulticastDNSPort.NotAnInteger)
		{
		if (dst->type == mDNSAddrType_IPv4) s = info->socket;
		else s = info->v6socket;
		}
#if mDNS_AllowPort53
	else if (srcPort.NotAnInteger == UnicastDNSPort.NotAnInteger &&
			 dst->type == mDNSAddrType_IPv4)
		s = info->socket53;
#endif
	else { debugf("Source port %d not allowed", (mDNSu16)srcPort.b[0]<<8 | srcPort.b[1]); return(-1); }
	err = sendto(s, msg, (UInt8*)end - (UInt8*)msg, 0, (struct sockaddr *)&to, to.ss_len);
	if (err < 0) { perror("mDNSPlatformSendUDP sendto"); return(err); }
	
	return(mStatus_NoError);
	}

static ssize_t myrecvfrom(const int s, void *const buffer, const size_t max,
	struct sockaddr *const from, size_t *const fromlen, mDNSAddr *dstaddr, char ifname[IF_NAMESIZE])
	{
	struct iovec databuffers = { (char *)buffer, max };
	struct msghdr   msg;
	ssize_t         n;
	struct cmsghdr *cmPtr;
	char            ancillary[1024];

	// Set up the message
	msg.msg_name       = (caddr_t)from;
	msg.msg_namelen    = *fromlen;
	msg.msg_iov        = &databuffers;
	msg.msg_iovlen     = 1;
	msg.msg_control    = (caddr_t)&ancillary;
	msg.msg_controllen = sizeof(ancillary);
	msg.msg_flags      = 0;
	
	// Receive the data
	n = recvmsg(s, &msg, 0);
	if (n<0 || msg.msg_controllen < sizeof(struct cmsghdr) || (msg.msg_flags & MSG_CTRUNC))
		{ perror("recvmsg"); return(n); }
	
	*fromlen = msg.msg_namelen;
	
	// Parse each option out of the ancillary data.
	for (cmPtr = CMSG_FIRSTHDR(&msg); cmPtr; cmPtr = CMSG_NXTHDR(&msg, cmPtr))
		{
		// debugf("myrecvfrom cmsg_level %d cmsg_type %d", cmPtr->cmsg_level, cmPtr->cmsg_type);
		if (cmPtr->cmsg_level == IPPROTO_IP && cmPtr->cmsg_type == IP_RECVDSTADDR)
			{
			dstaddr->type = mDNSAddrType_IPv4;
			dstaddr->addr.ipv4.NotAnInteger = *(u_int32_t*)CMSG_DATA(cmPtr);
			}
		if (cmPtr->cmsg_level == IPPROTO_IP && cmPtr->cmsg_type == IP_RECVIF)
			{
			struct sockaddr_dl *sdl = (struct sockaddr_dl *)CMSG_DATA(cmPtr);
			if (sdl->sdl_nlen < sizeof(ifname))
				{
				mDNSPlatformMemCopy(sdl->sdl_data, ifname, sdl->sdl_nlen);
				ifname[sdl->sdl_nlen] = 0;
				// debugf("IP_RECVIF sdl_index %d, sdl_data %s len %d", sdl->sdl_index, ifname, sdl->sdl_nlen);
				}
			}
		if (cmPtr->cmsg_level == IPPROTO_IPV6 && cmPtr->cmsg_type == IPV6_PKTINFO)
			{
			struct in6_pktinfo *ip6_info = (struct in6_pktinfo*)CMSG_DATA(cmPtr);
			dstaddr->type = mDNSAddrType_IPv6;
			dstaddr->addr.ipv6 = *(mDNSv6Addr*)&ip6_info->ipi6_addr;
			if_indextoname(ip6_info->ipi6_ifindex, ifname);
			}
		}

	return(n);
	}

mDNSlocal void myCFSocketCallBack(CFSocketRef s, CFSocketCallBackType type, CFDataRef address, const void *data, void *context)
	{
	mDNSAddr senderAddr, destAddr;
	mDNSIPPort senderPort;
	NetworkInterfaceInfo2 *info = (NetworkInterfaceInfo2 *)context;
	mDNS *const m = info->m;
	DNSMessage packet;
	struct sockaddr_storage from;
	size_t fromlen = sizeof(from);
	char packetifname[IF_NAMESIZE] = "";
	int err;
	
	(void)address;	// Parameter not used
	(void)data;		// Parameter not used
	
	if (type != kCFSocketReadCallBack) debugf("myCFSocketCallBack: Why is type not kCFSocketReadCallBack?");
#if mDNS_AllowPort53
	if (s == info->cfsocket53)
		err = myrecvfrom(info->socket53, &packet, sizeof(packet), (struct sockaddr *)&from, &fromlen, &destAddr, packetifname);
	else
#endif
	if (s == info->v6cfsocket)
		err = myrecvfrom(info->v6socket, &packet, sizeof(packet), (struct sockaddr *)&from, &fromlen, &destAddr, packetifname);
	else
		err = myrecvfrom(info->socket, &packet, sizeof(packet), (struct sockaddr *)&from, &fromlen, &destAddr, packetifname);

	if (err < 0) { debugf("myCFSocketCallBack recvfrom error %d", err); return; }

	if (from.ss_family == AF_INET)
		{
		struct sockaddr_in *sin = (struct sockaddr_in*)&from;
		senderAddr.type = mDNSAddrType_IPv4;
		senderAddr.addr.ipv4.NotAnInteger = sin->sin_addr.s_addr;
		senderPort.NotAnInteger = sin->sin_port;
		}
	else if (from.ss_family == AF_INET6)
		{
		struct sockaddr_in6 *sin6 = (struct sockaddr_in6*)&from;
		senderAddr.type = mDNSAddrType_IPv6;
		senderAddr.addr.ipv6 = *(mDNSv6Addr*)&sin6->sin6_addr;
		senderPort.NotAnInteger = sin6->sin6_port;
		}
	else
		{
		debugf("myCFSocketCallBack from is unknown address family %d", from.ss_family);
		return;
		}

	// Even though we indicated a specific interface in the IP_ADD_MEMBERSHIP call, a weirdness of the
	// sockets API means that even though this socket has only officially joined the multicast group
	// on one specific interface, the kernel will still deliver multicast packets to it no matter which
	// interface they arrive on. According to the official Unix Powers That Be, this is Not A Bug.
	// To work around this weirdness, we use the IP_RECVIF option to find the name of the interface
	// on which the packet arrived, and ignore the packet if it really arrived on some other interface.
	if (strcmp(info->ifa_name, packetifname))
		{
		verbosedebugf("myCFSocketCallBack got a packet from %#a to %#a on interface %#a/%s (Ignored -- really arrived on interface %s)",
			&senderAddr, &destAddr, &info->ifinfo.ip, info->ifa_name, packetifname);
		return;
		}
	else
		verbosedebugf("myCFSocketCallBack got a packet from %#a to %#a on interface %#a/%s",
			&senderAddr, &destAddr, &info->ifinfo.ip, info->ifa_name);

	if (err < (int)sizeof(DNSMessageHeader)) { debugf("myCFSocketCallBack packet length (%d) too short", err); return; }
	
#if mDNS_AllowPort53
	if (s == info->cfsocket53)
		mDNSCoreReceive(m, &packet, (unsigned char*)&packet + err, &senderAddr, senderPort, &destAddr, UnicastDNSPort, info->ifinfo.InterfaceID);
	else
#endif
	mDNSCoreReceive(m, &packet, (unsigned char*)&packet + err, &senderAddr, senderPort, &destAddr, MulticastDNSPort, info->ifinfo.InterfaceID);
	}

mDNSlocal void myCFRunLoopTimerCallBack(CFRunLoopTimerRef timer, void *info)
	{
	(void)timer;	// Parameter not used
	mDNSCoreTask((mDNS *const)info);
	}

// This gets the text of the field currently labelled "Computer Name" in the Sharing Prefs Control Panel
mDNSlocal void GetUserSpecifiedFriendlyComputerName(domainlabel *const namelabel)
	{
	CFStringEncoding encoding = kCFStringEncodingUTF8;
	CFStringRef cfs = SCDynamicStoreCopyComputerName(NULL, &encoding);
	if (cfs)
		{
		CFStringGetPascalString(cfs, namelabel->c, sizeof(*namelabel), kCFStringEncodingUTF8);
		CFRelease(cfs);
		}
	}

// This gets the text of the field currently labelled "Rendezvous Name" in the Sharing Prefs Control Panel
mDNSlocal void GetUserSpecifiedRFC1034ComputerName(domainlabel *const namelabel)
	{
	CFStringRef cfs = SCDynamicStoreCopyLocalHostName(NULL);
	if (cfs)
		{
		CFStringGetPascalString(cfs, namelabel->c, sizeof(*namelabel), kCFStringEncodingUTF8);
		CFRelease(cfs);
		}
	}

mDNSlocal mStatus SetupSocket(struct ifaddrs* ifa, mDNSIPPort port, int *s, CFSocketRef *c, CFSocketContext *context)
	{
	mStatus err;
	const int on = 1;
	const int twofivefive = 255;
	CFRunLoopSourceRef rls;
	
	// Open the socket...
	*s = socket(ifa->ifa_addr->sa_family, SOCK_DGRAM, IPPROTO_UDP);
	*c = NULL;
	if (*s < 0) { perror("socket"); return(*s); }
	
	// ... with a shared UDP port
	err = setsockopt(*s, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
	if (err < 0) { perror("setsockopt - SO_REUSEPORT"); return(err); }

	if (ifa->ifa_addr->sa_family == AF_INET)
		{
		struct ip_mreq imr;
		struct sockaddr_in	*sin = (struct sockaddr_in*)ifa->ifa_addr;
		struct sockaddr_in listening_sockaddr;
		
		// We want to receive destination addresses
		err = setsockopt(*s, IPPROTO_IP, IP_RECVDSTADDR, &on, sizeof(on));
		if (err < 0) { perror("setsockopt - IP_RECVDSTADDR"); return(err); }
		
		// We want to receive interface identifiers
		err = setsockopt(*s, IPPROTO_IP, IP_RECVIF, &on, sizeof(on));
		if (err < 0) { perror("setsockopt - IP_RECVIF"); return(err); }
		
		// Add multicast group membership on this interface
		imr.imr_multiaddr.s_addr = AllDNSLinkGroup.NotAnInteger;
		imr.imr_interface        = sin->sin_addr;
		err = setsockopt(*s, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imr, sizeof(imr));
		if (err < 0) { perror("setsockopt - IP_ADD_MEMBERSHIP"); return(err); }
		
		// Specify outgoing interface too
		err = setsockopt(*s, IPPROTO_IP, IP_MULTICAST_IF, &sin->sin_addr, sizeof(sin->sin_addr));
		if (err < 0) { perror("setsockopt - IP_MULTICAST_IF"); return(err); }
		
		// Send unicast packets with TTL 255
		err = setsockopt(*s, IPPROTO_IP, IP_TTL, &twofivefive, sizeof(twofivefive));
		if (err < 0) { perror("setsockopt - IP_TTL"); return(err); }
		
		// And multicast packets with TTL 255 too
		err = setsockopt(*s, IPPROTO_IP, IP_MULTICAST_TTL, &twofivefive, sizeof(twofivefive));
		if (err < 0) { perror("setsockopt - IP_MULTICAST_TTL"); return(err); }

		// And start listening for packets
		listening_sockaddr.sin_family      = AF_INET;
		listening_sockaddr.sin_port        = port.NotAnInteger;
		listening_sockaddr.sin_addr.s_addr = 0; // Want to receive multicasts AND unicasts on this socket
		err = bind(*s, (struct sockaddr *) &listening_sockaddr, sizeof(listening_sockaddr));
		if (err)
			{
			if (port.NotAnInteger == UnicastDNSPort.NotAnInteger) err = 0;
			else perror("bind");
			return(err);
			}
		}
	else if (ifa->ifa_addr->sa_family == AF_INET6)
		{
		struct ipv6_mreq i6mr;
		int	interface_id = if_nametoindex(ifa->ifa_name);
		struct sockaddr_in6 listening_sockaddr6;
		
		// We want to receive destination addresses and receive interface identifiers
		err = setsockopt(*s, IPPROTO_IPV6, IPV6_PKTINFO, &on, sizeof(on));
		if (err < 0) { perror("setsockopt - IPV6_PKTINFO"); return(err); }
		
		// We want to receive only IPv6 packets, without this option, we may
		// get IPv4 addresses as mapped addresses.
		err = setsockopt(*s, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on));
		if (err < 0) { perror("setsockopt - IPV6_V6ONLY"); return(err); }
		
		// Add multicast group membership on this interface
		i6mr.ipv6mr_interface = interface_id;
		i6mr.ipv6mr_multiaddr = *(struct in6_addr*)&AllDNSLinkGroupv6;
		err = setsockopt(*s, IPPROTO_IPV6, IPV6_JOIN_GROUP, &i6mr, sizeof(i6mr));
		if (err < 0) { perror("setsockopt - IPV6_JOIN_GROUP"); return(err); }
		
		// Specify outgoing interface too
		err = setsockopt(*s, IPPROTO_IPV6, IPV6_MULTICAST_IF, &interface_id, sizeof(interface_id));
		if (err < 0) { perror("setsockopt - IPV6_MULTICAST_IF"); return(err); }
		
		// Send unicast packets with TTL 255
		err = setsockopt(*s, IPPROTO_IPV6, IPV6_UNICAST_HOPS, &twofivefive, sizeof(twofivefive));
		if (err < 0) { perror("setsockopt - IPV6_UNICAST_HOPS"); return(err); }
		
		// And multicast packets with TTL 255 too
		err = setsockopt(*s, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &twofivefive, sizeof(twofivefive));
		if (err < 0) { perror("setsockopt - IPV6_MULTICAST_HOPS"); return(err); }
		
		// And start listening for packets
		bzero(&listening_sockaddr6, sizeof(listening_sockaddr6));
		listening_sockaddr6.sin6_len		 = sizeof(listening_sockaddr6);
		listening_sockaddr6.sin6_family      = AF_INET6;
		listening_sockaddr6.sin6_port        = port.NotAnInteger;
		listening_sockaddr6.sin6_flowinfo	 = 0;
//		listening_sockaddr6.sin6_addr = IN6ADDR_ANY_INIT; // Want to receive multicasts AND unicasts on this socket
		listening_sockaddr6.sin6_scope_id	 = 0;
		err = bind(*s, (struct sockaddr *) &listening_sockaddr6, sizeof(listening_sockaddr6));
		if (err)
			{
			if (port.NotAnInteger == UnicastDNSPort.NotAnInteger) err = 0;
			else perror("bind");
			return(err);
			}
		}

	*c = CFSocketCreateWithNative(kCFAllocatorDefault, *s, kCFSocketReadCallBack, myCFSocketCallBack, context);
	rls = CFSocketCreateRunLoopSource(kCFAllocatorDefault, *c, 0);
	CFRunLoopAddSource(CFRunLoopGetCurrent(), rls, kCFRunLoopDefaultMode);
	CFRelease(rls);
	
	return(err);
	}

#if 0
mDNSlocal NetworkInterfaceInfo2 *SearchForInterfaceByAddr(mDNS *const m, mDNSAddr ip)
	{
	NetworkInterfaceInfo2 *info = (NetworkInterfaceInfo2*)(m->HostInterfaces);
	while (info)
		{
		if (info->ifinfo.ip.NotAnInteger == ip.NotAnInteger) return(info);
		info = (NetworkInterfaceInfo2 *)(info->ifinfo.next);
		}
	return(NULL);
	}
#endif

mDNSlocal NetworkInterfaceInfo2 *SearchForInterfaceByName(mDNS *const m, char *ifname, int type)
	{
	NetworkInterfaceInfo2 *info = (NetworkInterfaceInfo2*)(m->HostInterfaces);
	while (info)
		{
		if (!strcmp(info->ifa_name, ifname) &&
			((AAAA_OVER_V4                                                 ) ||
			 (type == AF_INET  && info->ifinfo.ip.type == mDNSAddrType_IPv4) ||
			 (type == AF_INET6 && info->ifinfo.ip.type == mDNSAddrType_IPv6) ))
			 return(info);
		info = (NetworkInterfaceInfo2 *)(info->ifinfo.next);
		}
	return(NULL);
	}

mDNSlocal mStatus SetupInterface(mDNS *const m, NetworkInterfaceInfo2 *info, struct ifaddrs *ifa)
	{
	mStatus err = 0;
	NetworkInterfaceInfo2 *alias = SearchForInterfaceByName(m, ifa->ifa_name, ifa->ifa_addr->sa_family);
	if (!alias) alias = info;

	if (ifa->ifa_addr->sa_family == AF_INET)
		{
		struct sockaddr_in *ifa_addr = (struct sockaddr_in *)ifa->ifa_addr;
		info->ifinfo.ip.type = mDNSAddrType_IPv4;
		info->ifinfo.ip.addr.ipv4.NotAnInteger = ifa_addr->sin_addr.s_addr;
		}
	else if (ifa->ifa_addr->sa_family == AF_INET6)
		{
		struct sockaddr_in6 *ifa_addr = (struct sockaddr_in6 *)ifa->ifa_addr;
		info->ifinfo.ip.type = mDNSAddrType_IPv6;
		if (IN6_IS_ADDR_LINKLOCAL(&ifa_addr->sin6_addr)) ifa_addr->sin6_addr.__u6_addr.__u6_addr16[1] = 0;
		info->ifinfo.ip.addr.ipv6 = *(mDNSv6Addr*)&ifa_addr->sin6_addr;
		}
	else
		{
		debugf("SetupInterface invalid sa_family %d", ifa->ifa_addr->sa_family);
		return(-1);
		}

	info->ifinfo.InterfaceID = alias;
	// info->ifinfo.ipv4 set above
	// info->ifinfo.ipv6 set above
	info->ifinfo.scope_id                 = if_nametoindex(ifa->ifa_name);
	info->ifinfo.Advertise                = m->AdvertiseLocalAddresses;
	info->m         = m;
	info->ifa_name  = (char *)mallocL("NetworkInterfaceInfo2 name", strlen(ifa->ifa_name) + 1);
	if (!info->ifa_name) return(-1);
	strcpy(info->ifa_name, ifa->ifa_name);
	info->socket     = 0;
	info->cfsocket   = 0;
#if mDNS_AllowPort53
	info->socket53   = 0;
	info->cfsocket53 = 0;
#endif
	info->v6socket	 = 0;
	info->v6cfsocket = 0;

	mDNS_RegisterInterface(m, &info->ifinfo);

	if (alias->socket == 0 && ifa->ifa_addr->sa_family == AF_INET)
		{
		CFSocketContext myCFSocketContext = { 0, alias, NULL, NULL, NULL };
		
#if mDNS_AllowPort53
		err = SetupSocket(ifa, UnicastDNSPort,   &alias->socket53, &alias->cfsocket53, &myCFSocketContext);
#endif
		if (!err)
			err = SetupSocket(ifa, MulticastDNSPort, &alias->socket, &alias->cfsocket, &myCFSocketContext);
		
		if (err == 0)
			debugf("SetupInterface: created v4 socket(s) for %s, InterfaceID %04X, ip %#a",
				ifa->ifa_name, info->ifinfo.InterfaceID, &info->ifinfo.ip);
		else
			debugf("SetupInterface: created v4 socket(s) failed for %s, InterfaceID %04X, ip %#a",
				ifa->ifa_name, info->ifinfo.InterfaceID, &info->ifinfo.ip);
		}

	if (alias->v6socket == 0 && ifa->ifa_addr->sa_family == AF_INET6)
		{
		CFSocketContext myCFSocketContext = { 0, alias, NULL, NULL, NULL };
		
		err = SetupSocket(ifa, MulticastDNSPort, &alias->v6socket, &alias->v6cfsocket, &myCFSocketContext);
		
		if (err == 0)
			debugf("SetupInterface: created v6 socket for %s, InterfaceID %04X, ip %#a",
				ifa->ifa_name, info->ifinfo.InterfaceID, &info->ifinfo.ip);
		else
			debugf("SetupInterface: created v6 socket failed for %s, InterfaceID %04X, ip %#a",
				ifa->ifa_name, info->ifinfo.InterfaceID, &info->ifinfo.ip);
		}


	debugf("SetupInterface: %s index %d Flags %04X %#a Registered",
		ifa->ifa_name, info->ifinfo.scope_id, ifa->ifa_flags, &info->ifinfo.ip);

	return(err);
	}

mDNSlocal void ClearInterfaceList(mDNS *const m)
	{
	while (m->HostInterfaces)
		{
		NetworkInterfaceInfo2 *info = (NetworkInterfaceInfo2*)(m->HostInterfaces);
		mDNS_DeregisterInterface(m, &info->ifinfo);
		if (info->ifa_name  ) freeL("NetworkInterfaceInfo2 name", info->ifa_name);
		if (info->socket > 0) shutdown(info->socket, 2);
		if (info->cfsocket) { CFSocketInvalidate(info->cfsocket); CFRelease(info->cfsocket); }
		if (info->v6socket > 0) shutdown(info->v6socket, 2);
		if (info->v6cfsocket) { CFSocketInvalidate(info->v6cfsocket); CFRelease(info->v6cfsocket); }
#if mDNS_AllowPort53
		if (info->socket53 > 0) shutdown(info->socket53, 2);
		if (info->cfsocket53) { CFSocketInvalidate(info->cfsocket53); CFRelease(info->cfsocket53); }
#endif
		freeL("NetworkInterfaceInfo2", info);
		}
	}

mDNSlocal mStatus SetupInterfaceList(mDNS *const m)
	{
	struct ifaddrs *ifalist;
	int err = getifaddrs(&ifalist);
	struct ifaddrs *ifa = ifalist;
	struct ifaddrs *theLoopback = NULL;
	if (err) return(err);

	// Set up the nice label
	m->nicelabel.c[0] = 0;
	GetUserSpecifiedFriendlyComputerName(&m->nicelabel);
	if (m->nicelabel.c[0] == 0) ConvertCStringToDomainLabel("Macintosh", &m->nicelabel);

	// Set up the RFC 1034-compliant label
	m->hostlabel.c[0] = 0;
	GetUserSpecifiedRFC1034ComputerName(&m->hostlabel);
	if (m->hostlabel.c[0] == 0) ConvertCStringToDomainLabel("Macintosh", &m->hostlabel);

	mDNS_GenerateFQDN(m);

	while (ifa)
		{
#if LIST_ALL_INTERFACES
		if (ifa->ifa_addr->sa_family != AF_INET && ifa->ifa_addr->sa_family != AF_INET6)
			debugf("SetupInterface: %s Flags %04X Family %d not AF_INET or AF_INET6",
				ifa->ifa_name, ifa->ifa_flags, ifa->ifa_addr->sa_family);
		if (!(ifa->ifa_flags & IFF_UP))
			debugf("SetupInterface: %s Flags %04X Interface not IFF_UP", ifa->ifa_name, ifa->ifa_flags);
		if (ifa->ifa_flags & IFF_LOOPBACK)
			debugf("SetupInterface: %s Flags %04X Interface IFF_LOOPBACK", ifa->ifa_name, ifa->ifa_flags);
		if (ifa->ifa_flags & IFF_POINTOPOINT)
			debugf("SetupInterface: %s Flags %04X Interface IFF_POINTOPOINT", ifa->ifa_name, ifa->ifa_flags);
#endif
		if ((ifa->ifa_addr->sa_family == AF_INET || ifa->ifa_addr->sa_family == AF_INET6) &&
		    (ifa->ifa_flags & IFF_UP) && !(ifa->ifa_flags & IFF_POINTOPOINT))
			{
			if (ifa->ifa_flags & IFF_LOOPBACK)
				theLoopback = ifa;
			else
				{
				NetworkInterfaceInfo2 *info = (NetworkInterfaceInfo2 *)mallocL("NetworkInterfaceInfo2", sizeof(*info));
				if (!info) debugf("SetupInterfaceList: Out of Memory!");
				else SetupInterface(m, info, ifa);
				}
			}
		ifa = ifa->ifa_next;
		}

	if (!m->HostInterfaces && theLoopback)
		{
		NetworkInterfaceInfo2 *info = (NetworkInterfaceInfo2 *)mallocL("NetworkInterfaceInfo2", sizeof(*info));
		if (!info) debugf("SetupInterfaceList: (theLoopback) Out of Memory!");
		else SetupInterface(m, info, theLoopback);
		}

	freeifaddrs(ifalist);
	return(err);
	}

mDNSlocal void NetworkChanged(SCDynamicStoreRef store, CFArrayRef changedKeys, void *context)
	{
	mDNS *const m = (mDNS *const)context;
	debugf("***   Network Configuration Change   ***");
	(void)store;		// Parameter not used
	(void)changedKeys;	// Parameter not used
	
	ClearInterfaceList(m);
	SetupInterfaceList(m);
	if (NotifyClientNetworkChanged) NotifyClientNetworkChanged();
	mDNSCoreSleep(m, false);
	}

mDNSlocal mStatus WatchForNetworkChanges(mDNS *const m)
	{
	mStatus err = -1;
	SCDynamicStoreContext context = { 0, m, NULL, NULL, NULL };
	SCDynamicStoreRef     store    = SCDynamicStoreCreate(NULL, CFSTR("mDNSResponder"), NetworkChanged, &context);
	CFStringRef           key1     = SCDynamicStoreKeyCreateNetworkGlobalEntity(NULL, kSCDynamicStoreDomainState, kSCEntNetIPv4);
	CFStringRef           key2     = SCDynamicStoreKeyCreateComputerName(NULL);
	CFStringRef           key3     = SCDynamicStoreKeyCreateHostNames(NULL);
	CFStringRef           pattern  = SCDynamicStoreKeyCreateNetworkInterfaceEntity(NULL, kSCDynamicStoreDomainState, kSCCompAnyRegex, kSCEntNetIPv4);
	CFMutableArrayRef     keys     = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
	CFMutableArrayRef     patterns = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);

	if (!store) { fprintf(stderr, "SCDynamicStoreCreate failed: %s\n", SCErrorString(SCError())); goto error; }
	if (!key1 || !key2 || !key3 || !keys || !pattern || !patterns) goto error;

	CFArrayAppendValue(keys, key1);
	CFArrayAppendValue(keys, key2);
	CFArrayAppendValue(keys, key3);
	CFArrayAppendValue(patterns, pattern);
	if (!SCDynamicStoreSetNotificationKeys(store, keys, patterns))
		{ fprintf(stderr, "SCDynamicStoreSetNotificationKeys failed: %s\n", SCErrorString(SCError())); goto error; }

	m->p->StoreRLS = SCDynamicStoreCreateRunLoopSource(NULL, store, 0);
	if (!m->p->StoreRLS) { fprintf(stderr, "SCDynamicStoreCreateRunLoopSource failed: %s\n", SCErrorString(SCError())); goto error; }

	CFRunLoopAddSource(CFRunLoopGetCurrent(), m->p->StoreRLS, kCFRunLoopDefaultMode);
	m->p->Store = store;
	err = 0;
	goto exit;

error:
	if (store)    CFRelease(store);

exit:
	if (key1)     CFRelease(key1);
	if (key2)     CFRelease(key2);
	if (key3)     CFRelease(key3);
	if (pattern)  CFRelease(pattern);
	if (keys)     CFRelease(keys);
	if (patterns) CFRelease(patterns);
	
	return(err);
	}

mDNSlocal void PowerChanged(void *refcon, io_service_t service, natural_t messageType, void *messageArgument)
	{
	mDNS *const m = (mDNS *const)refcon;
	(void)service;		// Parameter not used
	switch(messageType)
		{
		case kIOMessageCanSystemPowerOff:     debugf("PowerChanged kIOMessageCanSystemPowerOff (no action)");               break; // E0000240
		case kIOMessageSystemWillPowerOff:    debugf("PowerChanged kIOMessageSystemWillPowerOff"); mDNSCoreSleep(m, true);  break; // E0000250
		case kIOMessageSystemWillNotPowerOff: debugf("PowerChanged kIOMessageSystemWillNotPowerOff (no action)");           break; // E0000260
		case kIOMessageCanSystemSleep:        debugf("PowerChanged kIOMessageCanSystemSleep (no action)");                  break; // E0000270
		case kIOMessageSystemWillSleep:       debugf("PowerChanged kIOMessageSystemWillSleep");    mDNSCoreSleep(m, true);  break; // E0000280
		case kIOMessageSystemWillNotSleep:    debugf("PowerChanged kIOMessageSystemWillNotSleep (no action)");              break; // E0000290
		case kIOMessageSystemHasPoweredOn:    debugf("PowerChanged kIOMessageSystemHasPoweredOn"); mDNSCoreSleep(m, false); break; // E0000300
		default:                              debugf("PowerChanged unknown message %X", messageType);                       break;
		}
	IOAllowPowerChange(m->p->PowerConnection, (long)messageArgument);
	}

mDNSlocal mStatus WatchForPowerChanges(mDNS *const m)
	{
	IONotificationPortRef thePortRef;
	m->p->PowerConnection = IORegisterForSystemPower(m, &thePortRef, PowerChanged, &m->p->PowerNotifier);
	if (m->p->PowerConnection)
		{
		m->p->PowerRLS = IONotificationPortGetRunLoopSource(thePortRef);
		CFRunLoopAddSource(CFRunLoopGetCurrent(), m->p->PowerRLS, kCFRunLoopDefaultMode);
		return(mStatus_NoError);
		}
	return(-1);
	}

mDNSlocal mStatus mDNSPlatformInit_setup(mDNS *const m)
	{
	mStatus err;

	CFRunLoopTimerContext myCFRunLoopTimerContext = { 0, m, NULL, NULL, NULL };
	
	// Note: Every CFRunLoopTimer has to be created with an initial fire time, and a repeat interval, or it becomes
	// a one-shot timer and you can't use CFRunLoopTimerSetNextFireDate(timer, when) to schedule subsequent firings.
	// Here we create it with an initial fire time ten seconds from now, and a repeat interval of ten seconds,
	// knowing that we'll reschedule it using CFRunLoopTimerSetNextFireDate(timer, when) long before that happens.
	m->p->CFTimer = CFRunLoopTimerCreate(kCFAllocatorDefault, CFAbsoluteTimeGetCurrent() + 10.0, 10.0, 0, 1,
											myCFRunLoopTimerCallBack, &myCFRunLoopTimerContext);
	CFRunLoopAddTimer(CFRunLoopGetCurrent(), m->p->CFTimer, kCFRunLoopDefaultMode);

	SetupInterfaceList(m);

	err = WatchForNetworkChanges(m);
	if (err) return(err);
	
	err = WatchForPowerChanges(m);
	return(err);
	}

mDNSexport mStatus mDNSPlatformInit(mDNS *const m)
	{
	mStatus result = mDNSPlatformInit_setup(m);
	// We don't do asynchronous initialization on OS X, so by the time we get here the setup will already
	// have succeeded or failed -- so if it succeeded, we should just call mDNSCoreInitComplete() immediately
	if (result == mStatus_NoError) mDNSCoreInitComplete(m, mStatus_NoError);
	return(result);
	}

mDNSexport void mDNSPlatformClose(mDNS *const m)
	{
	if (m->p->PowerConnection)
		{
		CFRunLoopRemoveSource(CFRunLoopGetCurrent(), m->p->PowerRLS, kCFRunLoopDefaultMode);
		CFRunLoopSourceInvalidate(m->p->PowerRLS);
		CFRelease(m->p->PowerRLS);
		IODeregisterForSystemPower(&m->p->PowerNotifier);
		m->p->PowerConnection = NULL;
		m->p->PowerNotifier   = NULL;
		m->p->PowerRLS        = NULL;
		}
	
	if (m->p->Store)
		{
		CFRunLoopRemoveSource(CFRunLoopGetCurrent(), m->p->StoreRLS, kCFRunLoopDefaultMode);
		CFRunLoopSourceInvalidate(m->p->StoreRLS);
		CFRelease(m->p->StoreRLS);
		CFRelease(m->p->Store);
		m->p->Store    = NULL;
		m->p->StoreRLS = NULL;
		}
	
	ClearInterfaceList(m);
	
	if (m->p->CFTimer)
		{
		CFRunLoopTimerInvalidate(m->p->CFTimer);
		CFRelease(m->p->CFTimer);
		m->p->CFTimer = NULL;
		}
	}

mDNSexport mDNSs32  mDNSPlatformOneSecond = 1024;

mDNSexport mDNSs32  mDNSPlatformTimeNow()
	{
	struct timeval tp;
	gettimeofday(&tp, NULL);
	// tp.tv_sec is seconds since 1st January 1970 (GMT, with no adjustment for daylight savings time)
	// tp.tv_usec is microseconds since the start of this second (i.e. values 0 to 999999)
	// We use the lower 22 bits of tp.tv_sec for the top 22 bits of our result
	// and we multiply tp.tv_usec by 16 / 15625 to get a value in the range 0-1023 to go in the bottom 10 bits.
	// This gives us a proper modular (cyclic) counter that has a resolution of roughly 1ms (actually 1/1024 second)
	// and correctly cycles every 2^22 seconds (4194304 seconds = approx 48 days).
	return( (tp.tv_sec << 10) | (tp.tv_usec * 16 / 15625) );
	}

mDNSexport void mDNSPlatformScheduleTask(const mDNS *const m, mDNSs32 NextTaskTime)
	{
	if (m->p->CFTimer)
		{
		CFAbsoluteTime ticks    = (CFAbsoluteTime)(NextTaskTime - mDNSPlatformTimeNow());
		CFAbsoluteTime interval = ticks / (CFAbsoluteTime)mDNSPlatformOneSecond;
		CFRunLoopTimerSetNextFireDate(m->p->CFTimer, CFAbsoluteTimeGetCurrent() + interval);
		}
	}

// Locking is a no-op here, because we're single-threaded with a CFRunLoop, so we can never interrupt ourselves
mDNSexport void     mDNSPlatformLock   (const mDNS *const m) { (void)m; }
mDNSexport void     mDNSPlatformUnlock (const mDNS *const m) { (void)m; }
mDNSexport void     mDNSPlatformStrCopy(const void *src,       void *dst)              { strcpy((char *)dst, (char *)src); }
mDNSexport mDNSu32  mDNSPlatformStrLen (const void *src)                               { return(strlen((char*)src)); }
mDNSexport void     mDNSPlatformMemCopy(const void *src,       void *dst, mDNSu32 len) { memcpy(dst, src, len); }
mDNSexport mDNSBool mDNSPlatformMemSame(const void *src, const void *dst, mDNSu32 len) { return(memcmp(dst, src, len) == 0); }
mDNSexport void     mDNSPlatformMemZero(                       void *dst, mDNSu32 len) { bzero(dst, len); }
