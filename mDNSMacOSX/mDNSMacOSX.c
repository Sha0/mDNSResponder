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
Revision 1.71  2003/05/14 07:08:37  cheshire
<rdar://problem/3159272> mDNSResponder should be smarter about reconfigurations
Previously, when there was any network configuration change, mDNSResponder
would tear down the entire list of active interfaces and start again.
That was very disruptive, and caused the entire cache to be flushed,
and caused lots of extra network traffic. Now it only removes interfaces
that have really gone, and only adds new ones that weren't there before.

Revision 1.70  2003/05/07 18:30:24  cheshire
Fix signed/unsigned comparison warning

Revision 1.69  2003/05/06 20:14:44  cheshire
Change "tp" to "tv"

Revision 1.68  2003/05/06 00:00:49  cheshire
<rdar://problem/3248914> Rationalize naming of domainname manipulation functions

Revision 1.67  2003/04/29 00:43:44  cheshire
Fix compiler warnings

Revision 1.66  2003/04/26 02:41:58  cheshire
<rdar://problem/3241281> Change timenow from a local variable to a structure member

Revision 1.65  2003/04/26 02:34:01  cheshire
Add missing mDNSexport

Revision 1.64  2003/04/15 16:48:06  jgraessl
Bug #: 3228833
Modified code in CFSocket notifier function to read all packets on the socket
instead of reading only one packet every time the notifier was called.

Revision 1.63  2003/04/15 16:33:50  jgraessl
Bug #: 3221880
Switched to our own copy of if_indextoname to improve performance.

Revision 1.62  2003/03/28 01:55:44  cheshire
Minor improvements to debugging messages

Revision 1.61  2003/03/27 03:30:56  cheshire
<rdar://problem/3210018> Name conflicts not handled properly, resulting in memory corruption, and eventual crash
Problem was that HostNameCallback() was calling mDNS_DeregisterInterface(), which is not safe in a callback
Fixes:
1. Make mDNS_DeregisterInterface() safe to call from a callback
2. Make HostNameCallback() use mDNS_DeadvertiseInterface() instead
   (it never really needed to deregister the interface at all)

Revision 1.60  2003/03/15 04:40:38  cheshire
Change type called "mDNSOpaqueID" to the more descriptive name "mDNSInterfaceID"

Revision 1.59  2003/03/11 01:23:26  cheshire
Bug #: 3194246 mDNSResponder socket problems

Revision 1.58  2003/03/06 01:43:04  cheshire
Bug #: 3189097 Additional debugging code in mDNSResponder
Improve "LIST_ALL_INTERFACES" output

Revision 1.57  2003/03/05 22:36:27  cheshire
Bug #: 3186338 Loopback doesn't work with mDNSResponder-27
Temporary workaround: Skip loopback interface *only* if we found at least one v4 interface to use

Revision 1.56  2003/03/05 01:50:38  cheshire
Bug #: 3189097 Additional debugging code in mDNSResponder

Revision 1.55  2003/02/21 01:54:09  cheshire
Bug #: 3099194 mDNSResponder needs performance improvements
Switched to using new "mDNS_Execute" model (see "Implementer Notes.txt")

Revision 1.54  2003/02/20 06:48:35  cheshire
Bug #: 3169535 Xserve RAID needs to do interface-specific registrations
Reviewed by: Josh Graessley, Bob Bradley

Revision 1.53  2003/01/29 02:21:23  cheshire
Return mStatus_Invalid if can't send packet because socket not available

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

#include "mDNSClientAPI.h"          // Defines the interface provided to the client layer above
#include "mDNSPlatformFunctions.h"	// Defines the interface to the supporting layer below
#include "mDNSMacOSX.h"				// Defines the specific types needed to run mDNS on this platform

#include <stdio.h>
#include <unistd.h>					// For close()
#include <stdarg.h>					// For va_list support
#include <net/if.h>
#include <net/if_dl.h>
#include <sys/uio.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <fcntl.h>

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
// Functions

// Note, this uses mDNS_vsprintf instead of standard "vsprintf", because mDNS_vsprintf knows
// how to print special data types like IP addresses and length-prefixed domain names
#if MDNS_DEBUGMSGS
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
#endif

#if MDNS_DEBUGMSGS > 1
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
#endif

mDNSexport void LogMsg(const char *format, ...)
	{
	unsigned char buffer[512];
	va_list ptr;
	va_start(ptr,format);
	buffer[mDNS_vsprintf((char *)buffer, format, ptr)] = 0;
	va_end(ptr);
	openlog("mDNSResponder", LOG_CONS | LOG_PERROR | LOG_PID, LOG_DAEMON);
	fprintf(stderr, "%s\n", buffer);
	syslog(LOG_ERR, "%s", buffer);
	closelog();
	fflush(stderr);
	}

static struct ifaddrs* myGetIfAddrs(int refresh)
	{
	static struct ifaddrs	*ifa = NULL;
	
	if (refresh && ifa)
		{
		freeifaddrs(ifa);
		ifa = NULL;
		}
	
	if (ifa == NULL) getifaddrs(&ifa);
	
	return ifa;
	}

static int myIfIndexToName(u_short index, char* name)
	{
	struct ifaddrs*	ifa = myGetIfAddrs(0);
	if (ifa == NULL) return -1;
	
	for (;ifa ; ifa = ifa->ifa_next)
		{
		if (ifa->ifa_addr->sa_family == AF_LINK)
			{
			struct sockaddr_dl	*sdl = (struct sockaddr_dl*)ifa->ifa_addr;
			if (sdl->sdl_index == index) strncpy(name, ifa->ifa_name, IF_NAMESIZE);
			return 0;
			}
		}
	
	return -1;
	}

mDNSexport mStatus mDNSPlatformSendUDP(const mDNS *const m, const DNSMessage *const msg, const mDNSu8 *const end,
	mDNSInterfaceID InterfaceID, mDNSIPPort srcPort, const mDNSAddr *dst, mDNSIPPort dstPort)
	{
	#pragma unused(m)
	NetworkInterfaceInfoOSX *info = (NetworkInterfaceInfoOSX *)InterfaceID;
	struct sockaddr_storage to;
	int s, err;

	if (InterfaceID == 0) { LogMsg("mDNSPlatformSendUDP ERROR! Cannot send from zero InterfaceID"); return mStatus_BadParamErr; }

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
		LogMsg("mDNSPlatformSendUDP: dst is not an IPv4 or IPv6 address!\n");
		return mStatus_BadParamErr;
		}

	if (srcPort.NotAnInteger == MulticastDNSPort.NotAnInteger)
		{
		if (dst->type == mDNSAddrType_IPv4) s = info->sktv4;
		else s = info->sktv6;
		}
#if mDNS_AllowPort53
	else if (srcPort.NotAnInteger == UnicastDNSPort.NotAnInteger && dst->type == mDNSAddrType_IPv4)
		s = info->skt53;
#endif
	else { LogMsg("Source port %d not allowed", (mDNSu16)srcPort.b[0]<<8 | srcPort.b[1]); return(-1); }
	
	if (s >= 0)
		verbosedebugf("mDNSPlatformSendUDP: sending on InterfaceID %X %s/%d to %#a:%d skt %d",
			InterfaceID, info->ifa_name, dst->type, dst, (mDNSu16)dstPort.b[0]<<8 | dstPort.b[1], s);
	else
		verbosedebugf("mDNSPlatformSendUDP: NOT sending on InterfaceID %X %s/%d (socket of this type not available)",
			InterfaceID, info->ifa_name, dst->type, dst, (mDNSu16)dstPort.b[0]<<8 | dstPort.b[1]);

	// Note: When sending, mDNSCore may often ask us to send both a v4 multicast packet and then a v6 multicast packet
	// If we don't have the corresponding type of socket available, then return mStatus_Invalid
	if (s < 0) return(mStatus_Invalid);

	err = sendto(s, msg, (UInt8*)end - (UInt8*)msg, 0, (struct sockaddr *)&to, to.ss_len);
	if (err < 0)
		{
		perror("mDNSPlatformSendUDP sendto");
		LogMsg("mDNSPlatformSendUDP sendto failed to send packet on InterfaceID %X %s/%d to %#a:%d skt %d",
			InterfaceID, info->ifa_name, dst->type, dst, (mDNSu16)dstPort.b[0]<<8 | dstPort.b[1], s);
		return(err);
		}
	
	return(mStatus_NoError);
	}

static ssize_t myrecvfrom(const int s, void *const buffer, const size_t max,
	struct sockaddr *const from, size_t *const fromlen, mDNSAddr *dstaddr, char ifname[IF_NAMESIZE])
	{
	static int numLogMessages = 0;
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
	if (n<0)
		{
		if (errno != EWOULDBLOCK && numLogMessages++ < 100) LogMsg("CFSocket.c: recvmsg(%d) returned error %d errno %d", s, n, errno);
		return(-1);
		}
	if (msg.msg_controllen < (int)sizeof(struct cmsghdr))
		{
		if (numLogMessages++ < 100) LogMsg("CFSocket.c: recvmsg(%d) msg.msg_controllen %d < sizeof(struct cmsghdr) %d",
			s, msg.msg_controllen, sizeof(struct cmsghdr));
		return(-1);
		}
	if (msg.msg_flags & MSG_CTRUNC)
		{
		if (numLogMessages++ < 100) LogMsg("CFSocket.c: recvmsg(%d) msg.msg_flags & MSG_CTRUNC", s);
		return(-1);
		}
	
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
			myIfIndexToName(ip6_info->ipi6_ifindex, ifname);
			}
		}

	return(n);
	}

mDNSlocal void myCFSocketCallBack(CFSocketRef cfs, CFSocketCallBackType CallBackType, CFDataRef address, const void *data, void *context)
	{
	mDNSAddr senderAddr, destAddr;
	mDNSIPPort senderPort, destPort = MulticastDNSPort;
	NetworkInterfaceInfoOSX *info = (NetworkInterfaceInfoOSX *)context;
	mDNS *const m = info->m;
	DNSMessage packet;
	struct sockaddr_storage from;
	size_t fromlen = sizeof(from);
	char packetifname[IF_NAMESIZE] = "";
	int err, s1 = -1, skt = CFSocketGetNative(cfs);
	int count = 0;
	
	(void)address;	// Parameter not used
	(void)data;		// Parameter not used
	
	if (CallBackType != kCFSocketReadCallBack) LogMsg("myCFSocketCallBack: Why is CallBackType %d not kCFSocketReadCallBack?", CallBackType);

#if mDNS_AllowPort53
	if      (cfs == info->cfs53) { s1 = info->skt53; destPort = UnicastDNSPort; }
	else
#endif
	if      (cfs == info->cfsv4) s1 = info->sktv4;
	else if (cfs == info->cfsv6) s1 = info->sktv6;

	if (s1 < 0 || s1 != skt)
		{
		LogMsg("myCFSocketCallBack: s1 %d native socket %d", s1, skt);
		LogMsg("myCFSocketCallBack: cfs %X, cfs53 %X, cfsv4 %X, cfsv6 %X", cfs, info->cfs53, info->cfsv4, info->cfsv6);
		LogMsg("myCFSocketCallBack: skt53 %d, sktv4 %d, sktv6 %d", info->skt53, info->sktv4, info->sktv6);
		}

	while ((err = myrecvfrom(s1, &packet, sizeof(packet), (struct sockaddr *)&from, &fromlen, &destAddr, packetifname)) >= 0)
		{
		count++;
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
			LogMsg("myCFSocketCallBack from is unknown address family %d", from.ss_family);
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
		
		mDNSCoreReceive(m, &packet, (unsigned char*)&packet + err, &senderAddr, senderPort, &destAddr, destPort, info->ifinfo.InterfaceID);
		}
	if (err < 0 && (errno != EWOULDBLOCK || count == 0))
		LogMsg("myCFSocketCallBack recvfrom(%d) error %d errno %d", s1, err, errno);
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

mDNSlocal mStatus SetupSocket(NetworkInterfaceInfoOSX *i, mDNSIPPort port, int *s, CFSocketRef *c)
	{
	int skt;
	mStatus err;
	const int on = 1;
	const int twofivefive = 255;
	CFRunLoopSourceRef rls;

	if (*s >= 0) { LogMsg("SetupSocket ERROR: socket %d is already set", *s); return(-1); }
	if (*c) { LogMsg("SetupSocket ERROR: CFSocketRef %X is already set", *c); return(-1); }

	// Open the socket...
	skt = socket(i->sa_family, SOCK_DGRAM, IPPROTO_UDP);
	if (skt < 0) { perror("socket"); return(skt); }

	// ... with a shared UDP port
	err = setsockopt(skt, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
	if (err < 0) { perror("setsockopt - SO_REUSEPORT"); return(err); }

	if (i->sa_family == AF_INET)
		{
		struct ip_mreq imr;
		struct in_addr addr = { i->ifinfo.ip.addr.ipv4.NotAnInteger };
		struct sockaddr_in listening_sockaddr;
		
		// We want to receive destination addresses
		err = setsockopt(skt, IPPROTO_IP, IP_RECVDSTADDR, &on, sizeof(on));
		if (err < 0) { perror("setsockopt - IP_RECVDSTADDR"); return(err); }
		
		// We want to receive interface identifiers
		err = setsockopt(skt, IPPROTO_IP, IP_RECVIF, &on, sizeof(on));
		if (err < 0) { perror("setsockopt - IP_RECVIF"); return(err); }
		
		// Add multicast group membership on this interface
		imr.imr_multiaddr.s_addr = AllDNSLinkGroup.NotAnInteger;
		imr.imr_interface        = addr;
		err = setsockopt(skt, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imr, sizeof(imr));
		if (err < 0) { perror("setsockopt - IP_ADD_MEMBERSHIP"); return(err); }
		
		// Specify outgoing interface too
		err = setsockopt(skt, IPPROTO_IP, IP_MULTICAST_IF, &addr, sizeof(addr));
		if (err < 0) { perror("setsockopt - IP_MULTICAST_IF"); return(err); }
		
		// Send unicast packets with TTL 255
		err = setsockopt(skt, IPPROTO_IP, IP_TTL, &twofivefive, sizeof(twofivefive));
		if (err < 0) { perror("setsockopt - IP_TTL"); return(err); }
		
		// And multicast packets with TTL 255 too
		err = setsockopt(skt, IPPROTO_IP, IP_MULTICAST_TTL, &twofivefive, sizeof(twofivefive));
		if (err < 0) { perror("setsockopt - IP_MULTICAST_TTL"); return(err); }

		// And start listening for packets
		listening_sockaddr.sin_family      = AF_INET;
		listening_sockaddr.sin_port        = port.NotAnInteger;
		listening_sockaddr.sin_addr.s_addr = 0; // Want to receive multicasts AND unicasts on this socket
		err = bind(skt, (struct sockaddr *) &listening_sockaddr, sizeof(listening_sockaddr));
		if (err)
			{
			// If we fail to bind to port 53 (because we're not root), that's okay, just tidy up and silently continue
			if (port.NotAnInteger == UnicastDNSPort.NotAnInteger) { close(skt); err = 0; }
			else perror("bind");
			return(err);
			}
		}
	else if (i->sa_family == AF_INET6)
		{
		struct ipv6_mreq i6mr;
		int	interface_id = if_nametoindex(i->ifa_name);
		struct sockaddr_in6 listening_sockaddr6;
		
		// We want to receive destination addresses and receive interface identifiers
		err = setsockopt(skt, IPPROTO_IPV6, IPV6_PKTINFO, &on, sizeof(on));
		if (err < 0) { perror("setsockopt - IPV6_PKTINFO"); return(err); }
		
		// We want to receive only IPv6 packets, without this option, we may
		// get IPv4 addresses as mapped addresses.
		err = setsockopt(skt, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on));
		if (err < 0) { perror("setsockopt - IPV6_V6ONLY"); return(err); }
		
		// Add multicast group membership on this interface
		i6mr.ipv6mr_interface = interface_id;
		i6mr.ipv6mr_multiaddr = *(struct in6_addr*)&AllDNSLinkGroupv6;
		err = setsockopt(skt, IPPROTO_IPV6, IPV6_JOIN_GROUP, &i6mr, sizeof(i6mr));
		if (err < 0) { perror("setsockopt - IPV6_JOIN_GROUP"); return(err); }
		
		// Specify outgoing interface too
		err = setsockopt(skt, IPPROTO_IPV6, IPV6_MULTICAST_IF, &interface_id, sizeof(interface_id));
		if (err < 0) { perror("setsockopt - IPV6_MULTICAST_IF"); return(err); }
		
		// Send unicast packets with TTL 255
		err = setsockopt(skt, IPPROTO_IPV6, IPV6_UNICAST_HOPS, &twofivefive, sizeof(twofivefive));
		if (err < 0) { perror("setsockopt - IPV6_UNICAST_HOPS"); return(err); }
		
		// And multicast packets with TTL 255 too
		err = setsockopt(skt, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &twofivefive, sizeof(twofivefive));
		if (err < 0) { perror("setsockopt - IPV6_MULTICAST_HOPS"); return(err); }
		
		// And start listening for packets
		bzero(&listening_sockaddr6, sizeof(listening_sockaddr6));
		listening_sockaddr6.sin6_len		 = sizeof(listening_sockaddr6);
		listening_sockaddr6.sin6_family      = AF_INET6;
		listening_sockaddr6.sin6_port        = port.NotAnInteger;
		listening_sockaddr6.sin6_flowinfo	 = 0;
//		listening_sockaddr6.sin6_addr = IN6ADDR_ANY_INIT; // Want to receive multicasts AND unicasts on this socket
		listening_sockaddr6.sin6_scope_id	 = 0;
		err = bind(skt, (struct sockaddr *) &listening_sockaddr6, sizeof(listening_sockaddr6));
		if (err) { perror("bind"); return(err); }
		}
	
	fcntl(skt, F_SETFL, fcntl(skt, F_GETFL, 0) | O_NONBLOCK); // set non-blocking
	*s = skt;
	CFSocketContext myCFSocketContext = { 0, i->ifinfo.InterfaceID, NULL, NULL, NULL };
	*c = CFSocketCreateWithNative(kCFAllocatorDefault, *s, kCFSocketReadCallBack, myCFSocketCallBack, &myCFSocketContext);
	rls = CFSocketCreateRunLoopSource(kCFAllocatorDefault, *c, 0);
	CFRunLoopAddSource(CFRunLoopGetCurrent(), rls, kCFRunLoopDefaultMode);
	CFRelease(rls);
	
	return(err);
	}

mDNSlocal mStatus SetupAddr(mDNSAddr *ip, const struct sockaddr *const sa)
	{
	if (sa->sa_family == AF_INET)
		{
		struct sockaddr_in *ifa_addr = (struct sockaddr_in *)sa;
		ip->type = mDNSAddrType_IPv4;
		ip->addr.ipv4.NotAnInteger = ifa_addr->sin_addr.s_addr;
		return(0);
		}
	else if (sa->sa_family == AF_INET6)
		{
		struct sockaddr_in6 *ifa_addr = (struct sockaddr_in6 *)sa;
		ip->type = mDNSAddrType_IPv6;
		if (IN6_IS_ADDR_LINKLOCAL(&ifa_addr->sin6_addr)) ifa_addr->sin6_addr.__u6_addr.__u6_addr16[1] = 0;
		ip->addr.ipv6 = *(mDNSv6Addr*)&ifa_addr->sin6_addr;
		return(0);
		}
	else
		{
		LogMsg("SetupAddr invalid sa_family %d", sa->sa_family);
		return(-1);
		}
	}

mDNSlocal mStatus AddInterfaceToList(mDNS *const m, struct ifaddrs *ifa)
	{
	mDNSAddr ip;
	SetupAddr(&ip, ifa->ifa_addr);
	NetworkInterfaceInfoOSX **p;
	for (p = &m->p->InterfaceList; *p; p = &(*p)->next)
		if (mDNSSameAddress(&ip, &(*p)->ifinfo.ip))
			{
			debugf("UpdateInterfaceList: Found existing interface with address %#a", &ip);
			(*p)->CurrentlyActive = mDNStrue;
			return(0);
			}

	debugf("UpdateInterfaceList: Making   new   interface with address %#a", &ip);
	NetworkInterfaceInfoOSX *i = (NetworkInterfaceInfoOSX *)mallocL("NetworkInterfaceInfoOSX", sizeof(*i));
	if (!i) return(-1);
	i->ifa_name        = (char *)mallocL("NetworkInterfaceInfoOSX name", strlen(ifa->ifa_name) + 1);
	if (!i->ifa_name) { freeL("NetworkInterfaceInfoOSX", i); return(-1); }
	strcpy(i->ifa_name, ifa->ifa_name);

	i->ifinfo.InterfaceID = mDNSNULL;
	i->ifinfo.ip          = ip;
	i->ifinfo.scope_id    = if_nametoindex(ifa->ifa_name);
	i->ifinfo.Advertise   = m->AdvertiseLocalAddresses;

	i->next            = mDNSNULL;
	i->m               = m;
	i->CurrentlyActive = mDNStrue;
	i->sa_family       = ifa->ifa_addr->sa_family;
	#if mDNS_AllowPort53
	i->skt53 = -1;
	i->cfs53 = NULL;
	#endif
	i->sktv4 = -1;
	i->cfsv4 = NULL;
	i->sktv6 = -1;
	i->cfsv6 = NULL;
	
	if (!i->ifa_name) return(-1);
	*p = i;
	return(0);
	}

mDNSlocal mStatus UpdateInterfaceList(mDNS *const m)
	{
	debugf("UpdateInterfaceList");
	mDNSBool foundav4           = mDNSfalse;
	struct ifaddrs *ifa         = myGetIfAddrs(1);
	struct ifaddrs *theLoopback = NULL;
	int err = (ifa != NULL) ? 0 : (errno != 0 ? errno : -1);
	if (err) return(err);

	// Set up the nice label
	m->nicelabel.c[0] = 0;
	GetUserSpecifiedFriendlyComputerName(&m->nicelabel);
	if (m->nicelabel.c[0] == 0) MakeDomainLabelFromLiteralString(&m->nicelabel, "Macintosh");

	// Set up the RFC 1034-compliant label
	domainlabel hostlabel;
	hostlabel.c[0] = 0;
	GetUserSpecifiedRFC1034ComputerName(&hostlabel);
	if (hostlabel.c[0] == 0) MakeDomainLabelFromLiteralString(&hostlabel, "Macintosh");
	if (!SameDomainLabel(m->hostlabel.c, hostlabel.c))
		{
		m->hostlabel = hostlabel;
		mDNS_GenerateFQDN(m);
		}

	while (ifa)
		{
#if LIST_ALL_INTERFACES
		if (ifa->ifa_addr->sa_family == AF_APPLETALK)
			debugf("UpdateInterfaceList: %4s(%d) Flags %04X Family %2d is AF_APPLETALK",
				ifa->ifa_name, if_nametoindex(ifa->ifa_name), ifa->ifa_flags, ifa->ifa_addr->sa_family);
		else if (ifa->ifa_addr->sa_family == AF_LINK)
			debugf("UpdateInterfaceList: %4s(%d) Flags %04X Family %2d is AF_LINK",
				ifa->ifa_name, if_nametoindex(ifa->ifa_name), ifa->ifa_flags, ifa->ifa_addr->sa_family);
		else if (ifa->ifa_addr->sa_family != AF_INET && ifa->ifa_addr->sa_family != AF_INET6)
			debugf("UpdateInterfaceList: %4s(%d) Flags %04X Family %2d not AF_INET (2) or AF_INET6 (30)",
				ifa->ifa_name, if_nametoindex(ifa->ifa_name), ifa->ifa_flags, ifa->ifa_addr->sa_family);
		if (!(ifa->ifa_flags & IFF_UP))
			debugf("UpdateInterfaceList: %4s(%d) Flags %04X Family %2d Interface not IFF_UP",
				ifa->ifa_name, if_nametoindex(ifa->ifa_name), ifa->ifa_flags, ifa->ifa_addr->sa_family);
		if (ifa->ifa_flags & IFF_POINTOPOINT)
			debugf("UpdateInterfaceList: %4s(%d) Flags %04X Family %2d Interface IFF_POINTOPOINT",
				ifa->ifa_name, if_nametoindex(ifa->ifa_name), ifa->ifa_flags, ifa->ifa_addr->sa_family);
		if (ifa->ifa_flags & IFF_LOOPBACK)
			debugf("UpdateInterfaceList: %4s(%d) Flags %04X Family %2d Interface IFF_LOOPBACK",
				ifa->ifa_name, if_nametoindex(ifa->ifa_name), ifa->ifa_flags, ifa->ifa_addr->sa_family);
#endif
		if ((ifa->ifa_addr->sa_family == AF_INET || ifa->ifa_addr->sa_family == AF_INET6) &&
		    (ifa->ifa_flags & IFF_UP) && !(ifa->ifa_flags & IFF_POINTOPOINT))
			{
			if (ifa->ifa_flags & IFF_LOOPBACK)
				theLoopback = ifa;
			else
				{
				AddInterfaceToList(m, ifa);
				if (ifa->ifa_addr->sa_family == AF_INET)
					foundav4 = mDNStrue;
				}
			}
		ifa = ifa->ifa_next;
		}

//	Temporary workaround: Multicast loopback on IPv6 interfaces appears not to work.
//	In the interim, we skip loopback interface only if we found at least one v4 interface to use
	if (!foundav4 && theLoopback)
		AddInterfaceToList(m, theLoopback);

	return(err);
	}

mDNSlocal NetworkInterfaceInfoOSX *SearchForInterfaceByName(mDNS *const m, char *ifname, int type)
	{
	NetworkInterfaceInfoOSX *i;
	for (i = m->p->InterfaceList; i; i = i->next)
		if (!strcmp(i->ifa_name, ifname) &&
			((AAAA_OVER_V4                                              ) ||
			 (type == AF_INET  && i->ifinfo.ip.type == mDNSAddrType_IPv4) ||
			 (type == AF_INET6 && i->ifinfo.ip.type == mDNSAddrType_IPv6) )) return(i);
	return(NULL);
	}

mDNSlocal void SetupActiveInterfaces(mDNS *const m)
	{
	debugf("SetupActiveInterfaces");
	NetworkInterfaceInfoOSX *i;
	for (i = m->p->InterfaceList; i; i = i->next)
		{
		mStatus err = 0;
		NetworkInterfaceInfo *n = &i->ifinfo;
		NetworkInterfaceInfoOSX *alias = SearchForInterfaceByName(m, i->ifa_name, i->sa_family);
		if (!alias) alias = i;
	
		if (!n->InterfaceID)
			{
			mDNS_RegisterInterface(m, n);
			debugf("SetupActiveInterfaces: Registered  %s(%d) InterfaceID %04X %#a", i->ifa_name, n->scope_id, alias, &n->ip);
			}
		else if (n->InterfaceID != alias)
			LogMsg("SetupActiveInterfaces ERROR! n->InterfaceID %p != alias %p", n->InterfaceID, alias);
	
		n->InterfaceID = alias;
	
		if (i->sa_family == AF_INET && alias->sktv4 == -1)
			{
			#if mDNS_AllowPort53
			err = SetupSocket(i, UnicastDNSPort, &alias->skt53, &alias->cfs53);
			#endif
			if (!err) err = SetupSocket(i, MulticastDNSPort, &alias->sktv4, &alias->cfsv4);
			if (err == 0) debugf("SetupActiveInterfaces: v4 socket%2d %s(%d) InterfaceID %04X %#a", alias->sktv4, i->ifa_name, n->scope_id, n->InterfaceID, &n->ip);
			else LogMsg("SetupActiveInterfaces: v4 socket%2d %s(%d) InterfaceID %04X %#a FAILED",   alias->sktv4, i->ifa_name, n->scope_id, n->InterfaceID, &n->ip);
			}
	
		if (i->sa_family == AF_INET6 && alias->sktv6 == -1)
			{
			err = SetupSocket(i, MulticastDNSPort, &alias->sktv6, &alias->cfsv6);
			if (err == 0) debugf("SetupActiveInterfaces: v6 socket%2d %s(%d) InterfaceID %04X %#a", alias->sktv6, i->ifa_name, n->scope_id, n->InterfaceID, &n->ip);
			else LogMsg("SetupActiveInterfaces: v6 socket%2d %s(%d) InterfaceID %04X %#a FAILED",   alias->sktv6, i->ifa_name, n->scope_id, n->InterfaceID, &n->ip);
			}
		}
	}

mDNSlocal void MarkAllInterfacesInactive(mDNS *const m)
	{
	debugf("MarkAllInterfacesInactive");
	NetworkInterfaceInfoOSX *i;
	for (i = m->p->InterfaceList; i; i = i->next)
		i->CurrentlyActive = mDNSfalse;
	}

mDNSlocal void ClearInactiveInterfaces(mDNS *const m)
	{
	debugf("ClearInactiveInterfaces");
	NetworkInterfaceInfoOSX **p = &m->p->InterfaceList;
	while (*p)
		{
		NetworkInterfaceInfoOSX *i = *p;
		// 1. If this interface is no longer active, or it's InterfaceID is changing, deregister it
		NetworkInterfaceInfoOSX *alias = i->ifinfo.InterfaceID;
		if (!i->CurrentlyActive || (alias && !alias->CurrentlyActive))
			{
			debugf("ClearInactiveInterfaces: Deregistering %#a", &i->ifinfo.ip);
			mDNS_DeregisterInterface(m, &i->ifinfo);
			i->ifinfo.InterfaceID = mDNSNULL;
			}

		// 2. Close all our sockets. We'll recreate them later as necessary.
		// (We may have both v4 and v6, and we may not need both now.)
		#if mDNS_AllowPort53
		if (i->skt53 >= 0) { close(i->skt53);                                   i->skt53 = -1; }
		if (i->cfs53)      { CFSocketInvalidate(i->cfs53); CFRelease(i->cfs53); i->cfs53 = NULL; }
		#endif
		if (i->sktv4 >= 0) { close(i->sktv4);                                   i->sktv4 = -1; }
		if (i->cfsv4)      { CFSocketInvalidate(i->cfsv4); CFRelease(i->cfsv4); i->cfsv4 = NULL; }
		if (i->sktv6 >= 0) { close(i->sktv6);                                   i->sktv6 = -1; }
		if (i->cfsv6)      { CFSocketInvalidate(i->cfsv6); CFRelease(i->cfsv6); i->cfsv6 = NULL; }

		// 3. If no longer active, delete interface from list and free memory
		if (!i->CurrentlyActive)
			{
			debugf("ClearInactiveInterfaces: Deleting      %#a", &i->ifinfo.ip);
			NetworkInterfaceInfoOSX *i = *p;
			*p = i->next;
			if (i->ifa_name) freeL("NetworkInterfaceInfoOSX name", i->ifa_name);
			freeL("NetworkInterfaceInfoOSX", i);
			}
		else
			p = &i->next;
		}
	}

mDNSlocal void NetworkChanged(SCDynamicStoreRef store, CFArrayRef changedKeys, void *context)
	{
	(void)store;		// Parameter not used
	(void)changedKeys;	// Parameter not used
	debugf("***   Network Configuration Change   ***");

	// When changing location, SC seems to tell us it has torn down the interfaces
	// and then a second later, tell us it has put them back again
	// For now we'll just sleep a few seconds
	//sleep(5);

	mDNS *const m = (mDNS *const)context;
	MarkAllInterfacesInactive(m);
	UpdateInterfaceList(m);
	ClearInactiveInterfaces(m);
	SetupActiveInterfaces(m);
	
	if (m->MainCallback)
		m->MainCallback(m, mStatus_ConfigChanged);
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
		case kIOMessageCanSystemPowerOff:     debugf("PowerChanged kIOMessageCanSystemPowerOff (no action)");                      break; // E0000240
		case kIOMessageSystemWillPowerOff:    debugf("PowerChanged kIOMessageSystemWillPowerOff"); mDNSCoreMachineSleep(m, true);  break; // E0000250
		case kIOMessageSystemWillNotPowerOff: debugf("PowerChanged kIOMessageSystemWillNotPowerOff (no action)");                  break; // E0000260
		case kIOMessageCanSystemSleep:        debugf("PowerChanged kIOMessageCanSystemSleep (no action)");                         break; // E0000270
		case kIOMessageSystemWillSleep:       debugf("PowerChanged kIOMessageSystemWillSleep");    mDNSCoreMachineSleep(m, true);  break; // E0000280
		case kIOMessageSystemWillNotSleep:    debugf("PowerChanged kIOMessageSystemWillNotSleep (no action)");                     break; // E0000290
		case kIOMessageSystemHasPoweredOn:    debugf("PowerChanged kIOMessageSystemHasPoweredOn"); mDNSCoreMachineSleep(m, false); break; // E0000300
		default:                              debugf("PowerChanged unknown message %X", messageType);                              break;
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

	m->hostlabel.c[0]   = 0;
	m->p->InterfaceList = mDNSNULL;
	UpdateInterfaceList(m);
	SetupActiveInterfaces(m);

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
	
	MarkAllInterfacesInactive(m);
	ClearInactiveInterfaces(m);
	}

mDNSexport mDNSs32  mDNSPlatformOneSecond = 1024;

mDNSexport mDNSs32  mDNSPlatformTimeNow(void)
	{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	// tv.tv_sec is seconds since 1st January 1970 (GMT, with no adjustment for daylight savings time)
	// tv.tv_usec is microseconds since the start of this second (i.e. values 0 to 999999)
	// We use the lower 22 bits of tv.tv_sec for the top 22 bits of our result
	// and we multiply tv.tv_usec by 16 / 15625 to get a value in the range 0-1023 to go in the bottom 10 bits.
	// This gives us a proper modular (cyclic) counter that has a resolution of roughly 1ms (actually 1/1024 second)
	// and correctly cycles every 2^22 seconds (4194304 seconds = approx 48 days).
	return( (tv.tv_sec << 10) | (tv.tv_usec * 16 / 15625) );
	}

// Locking is a no-op here, because we're single-threaded with a CFRunLoop, so we can never interrupt ourselves
mDNSexport void     mDNSPlatformLock   (const mDNS *const m) { (void)m; }
mDNSexport void     mDNSPlatformUnlock (const mDNS *const m) { (void)m; }
mDNSexport void     mDNSPlatformStrCopy(const void *src,       void *dst)              { strcpy((char *)dst, (char *)src); }
mDNSexport mDNSu32  mDNSPlatformStrLen (const void *src)                               { return(strlen((char*)src)); }
mDNSexport void     mDNSPlatformMemCopy(const void *src,       void *dst, mDNSu32 len) { memcpy(dst, src, len); }
mDNSexport mDNSBool mDNSPlatformMemSame(const void *src, const void *dst, mDNSu32 len) { return(memcmp(dst, src, len) == 0); }
mDNSexport void     mDNSPlatformMemZero(                       void *dst, mDNSu32 len) { bzero(dst, len); }
