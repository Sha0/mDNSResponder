/*
 * Copyright (c) 2002 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 *
 * The contents of this file constitute Original Code as defined in
 * and are subject to the Apple Public Source License Version 1.1
 * (the "License").  You may not use this file except in compliance
 * with the License.  Please obtain a copy of the License at
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
 */

// ***************************************************************************
// mDNS-CFSocket.c:
// Supporting routines to run mDNS on a CFRunLoop platform
// ***************************************************************************

#include "mDNSClientAPI.h"				// Defines the interface provided to the client layer above
#include "mDNSPlatformFunctions.h"		// Defines the interface to the supporting layer below
#include "mDNSPlatformEnvironment.h"	// Defines the specific types needed to run mDNS on this platform
#include "mDNSvsprintf.h"				// Used to implement debugf_();

#include <stdio.h>
#include <stdarg.h>						// For va_list support
#include <net/if.h>
#include <net/if_dl.h>
#include <sys/uio.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <ifaddrs.h>

#include <SystemConfiguration/SystemConfiguration.h>

// ***************************************************************************
// Structures

typedef struct NetworkInterfaceInfo2_struct NetworkInterfaceInfo2;
struct NetworkInterfaceInfo2_struct
	{
	NetworkInterfaceInfo ifinfo;
	mDNS *m;
	char *ifa_name;
	NetworkInterfaceInfo2 *alias;
	int socket1;
	int socket2;
    CFSocketRef cfsocket1;
    CFSocketRef cfsocket2;
	};

// ***************************************************************************
// Globals

static CFAbsoluteTime StartTime;	// Temporary solution

// ***************************************************************************
// Functions

mDNSexport void debugf_(const char *format, ...)
	{
	unsigned char buffer[256];
    va_list ptr;
	va_start(ptr,format);
	buffer[mDNS_vsprintf(buffer, format, ptr)] = 0;
	va_end(ptr);
	fprintf(stderr, "%s\n", buffer);
	fflush(stderr);
	}

mDNSexport mStatus mDNSPlatformSendUDP(const mDNS *const m, const DNSMessage *const msg, const mDNSu8 *const end,
	mDNSIPAddr src, mDNSIPPort srcport, mDNSIPAddr dst, mDNSIPPort dstport)
	{
	NetworkInterfaceInfo2 *info = (NetworkInterfaceInfo2 *)(m->HostInterfaces);
	struct sockaddr_in to;
	to.sin_family      = AF_INET;
	to.sin_port        = dstport.NotAnInteger;
	to.sin_addr.s_addr = dst.    NotAnInteger;

	if (src.NotAnInteger == 0) debugf("mDNSPlatformSendUDP ERROR! Cannot send from zero source address");

	while (info)
		{
		if (info->ifinfo.ip.NotAnInteger == src.NotAnInteger)
			{
			int s = (srcport.NotAnInteger == MulticastDNSPort.NotAnInteger) ? info->socket1 : info->socket2;
			int err = sendto(s, msg, (UInt8*)end - (UInt8*)msg, 0, (struct sockaddr *)&to, sizeof(to));
			if (err < 0) { perror("mDNSPlatformSendUDP sendto"); return(err); }
			}
		info = (NetworkInterfaceInfo2 *)(info->ifinfo.next);
		}


	return(mStatus_NoError);
	}

static ssize_t myrecvfrom(const int s, void *const buffer, const size_t max,
	struct sockaddr *const from, size_t *const fromlen, struct in_addr *dstaddr, char ifname[128])
	{
	struct iovec databuffers = { buffer, max };
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
		if (cmPtr->cmsg_level == IPPROTO_IP && cmPtr->cmsg_type == IP_RECVDSTADDR)
			*dstaddr = *(struct in_addr *)CMSG_DATA(cmPtr);
		if (cmPtr->cmsg_level == IPPROTO_IP && cmPtr->cmsg_type == IP_RECVIF)
			{
			struct sockaddr_dl *sdl = (struct sockaddr_dl *)CMSG_DATA(cmPtr);
			if (sdl->sdl_nlen < sizeof(ifname))
				{
				mDNSPlatformMemCopy(sdl->sdl_data, ifname, sdl->sdl_nlen);
				ifname[sdl->sdl_nlen] = 0;
//				debugf("IP_RECVIF sdl_index %d, sdl_data %s len %d", sdl->sdl_index, ifname, sdl->sdl_nlen);
				}
			}
		}

	return(n);
	}

mDNSlocal void myCFSocketCallBack(CFSocketRef s, CFSocketCallBackType type, CFDataRef address, const void *data, void *context)
    {
	mDNSIPAddr senderaddr, destaddr, interface;
	mDNSIPPort senderport, destport;
	NetworkInterfaceInfo2 *info = (NetworkInterfaceInfo2 *)context;
	int skt = (s == info->cfsocket1) ? info->socket1 : info->socket2;
	mDNS *const m = info->m;
	DNSMessage packet;
	struct in_addr to;
	struct sockaddr_in from;
	size_t fromlen = sizeof(from);
	char ifname[128] = "";
	int err;
	if (type != kCFSocketReadCallBack) debugf("myCFSocketCallBack: Why is type not kCFSocketReadCallBack?");
	err = myrecvfrom(skt, &packet, sizeof(packet), (struct sockaddr *)&from, &fromlen, &to, ifname);

	senderaddr.NotAnInteger = from.sin_addr.s_addr;
	senderport.NotAnInteger = from.sin_port;
	destaddr.NotAnInteger   = to.s_addr;
	destport                = (s == info->cfsocket1) ? MulticastDNSPort : UnicastDNSPort;
	interface               = info->ifinfo.ip;

	if (strcmp(info->ifa_name, ifname))
		{
		debugf("myCFSocketCallBack got a packet from %.4a to %.4a on interface %.4a/%s (Ignored)",
			&senderaddr, &destaddr, &interface, ifname);
		return;
		}
	else
		debugf("myCFSocketCallBack got a packet from %.4a to %.4a on interface %.4a/%s",
			&senderaddr, &destaddr, &interface, ifname);

	if (err < 0) debugf("myCFSocketCallBack recvfrom error %d", err);
	else if (err < sizeof(DNSMessageHeader)) debugf("myCFSocketCallBack packet length (%d) too short", err);
	else mDNSCoreReceive(m, &packet, (char*)&packet + err, senderaddr, senderport, destaddr, destport, interface);
    }

mDNSlocal void myCFRunLoopTimerCallBack(CFRunLoopTimerRef timer, void *info)
    {
	mDNSCoreTask((mDNS *const)info);
    }

mDNSlocal void GetUserSpecifiedComputerName(domainlabel *const namelabel)
	{
	CFStringEncoding encoding = kCFStringEncodingUTF8;
	CFStringRef cfs = SCDynamicStoreCopyComputerName(NULL, &encoding);
	if (cfs)
		{
		CFStringGetPascalString(cfs, namelabel->c, sizeof(*namelabel), kCFStringEncodingUTF8);
		CFRelease(cfs);
		}
	}

mDNSlocal mStatus SetupSocket(struct sockaddr_in *ifa_addr, mDNSIPPort port, int *s, CFSocketRef *c, CFSocketContext *context)
	{
	mStatus err;
	const int on = 1;
	struct ip_mreq imr;
	struct sockaddr_in listening_sockaddr;
	CFRunLoopSourceRef rls;

	// Open the socket
    *s = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	*c = NULL;
	if (*s < 0) { perror("socket"); return(*s); }
	
	// Share this port number
	err = setsockopt(*s, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
	if (err < 0) { perror("setsockopt - SO_REUSEPORT"); return(err); }

	// Receive destination addresses
	err = setsockopt(*s, IPPROTO_IP, IP_RECVDSTADDR, &on, sizeof(on));
	if (err < 0) { perror("setsockopt - IP_RECVDSTADDR"); return(err); }
	
	// Receive interface
	err = setsockopt(*s, IPPROTO_IP, IP_RECVIF, &on, sizeof(on));
	if (err < 0) { perror("setsockopt - IP_RECVIF"); return(err); }
	
	// Add multicast group membership
	imr.imr_multiaddr.s_addr = AllDNSLinkGroup.NotAnInteger;
	imr.imr_interface        = ifa_addr->sin_addr;
	err = setsockopt(*s, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imr, sizeof(struct ip_mreq));
	if (err < 0) { perror("setsockopt - IP_ADD_MEMBERSHIP"); return(err); }

	// Specify outgoing interface too
	err = setsockopt(*s, IPPROTO_IP, IP_MULTICAST_IF, &ifa_addr->sin_addr, sizeof(ifa_addr->sin_addr));
	if (err < 0) { perror("setsockopt - IP_MULTICAST_IF"); return(err); }

	// And start listening
	listening_sockaddr.sin_family      = AF_INET;
	listening_sockaddr.sin_port        = port.NotAnInteger;
	listening_sockaddr.sin_addr.s_addr = 0;		// Want to receive multicasts AND unicasts on this socket
	err = bind(*s, (struct sockaddr *) &listening_sockaddr, sizeof(listening_sockaddr));
	if (err)
		{
		if (port.NotAnInteger == UnicastDNSPort.NotAnInteger) err = 0;
		else perror("bind");
		return(err);
		}

    *c = CFSocketCreateWithNative(kCFAllocatorDefault, *s, kCFSocketReadCallBack, myCFSocketCallBack, context);
    rls = CFSocketCreateRunLoopSource(kCFAllocatorDefault, *c, 0);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), rls, kCFRunLoopDefaultMode);
    CFRelease(rls);
	
	return(err);
	}

#if 0
mDNSlocal NetworkInterfaceInfo2 *SearchForInterfaceByAddr(mDNS *const m, mDNSIPAddr ip)
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

mDNSlocal NetworkInterfaceInfo2 *SearchForInterfaceByName(mDNS *const m, char *ifname)
	{
	NetworkInterfaceInfo2 *info = (NetworkInterfaceInfo2*)(m->HostInterfaces);
	while (info)
		{
		if (!strcmp(info->ifa_name, ifname)) return(info);
		info = (NetworkInterfaceInfo2 *)(info->ifinfo.next);
		}
	return(NULL);
	}

mDNSlocal mStatus SetupInterface(mDNS *const m, NetworkInterfaceInfo2 *info, struct ifaddrs *ifa)
	{
	extern int use_53;
	mStatus err = 0;
	struct sockaddr_in *ifa_addr = (struct sockaddr_in *)ifa->ifa_addr;
    CFSocketContext myCFSocketContext = { 0, info, NULL, NULL, NULL };

	info->ifinfo.ip.NotAnInteger = ifa_addr->sin_addr.s_addr;
	info->m         = m;
	info->ifa_name  = malloc(strlen(ifa->ifa_name) + 1);
	if (!info->ifa_name) return(-1);
	strcpy(info->ifa_name, ifa->ifa_name);
	info->alias     = SearchForInterfaceByName(m, ifa->ifa_name);
	info->socket1   = 0;
	info->socket2   = 0;
    info->cfsocket1 = 0;
    info->cfsocket2 = 0;

	mDNS_RegisterInterface(m, &info->ifinfo);

	if (info->alias)
		debugf("SetupInterface: %s Flags %04X %.4a is an alias of %.4a",
			ifa->ifa_name, ifa->ifa_flags, &info->ifinfo.ip, &info->alias->ifinfo.ip);

	if (use_53) err = SetupSocket(ifa_addr, UnicastDNSPort,   &info->socket2, &info->cfsocket2, &myCFSocketContext);
	if (!err)   err = SetupSocket(ifa_addr, MulticastDNSPort, &info->socket1, &info->cfsocket1, &myCFSocketContext);

	debugf("SetupInterface: %s Flags %04X %.4a Registered",
		ifa->ifa_name, ifa->ifa_flags, &info->ifinfo.ip);

	return(err);
	}

mDNSlocal void ClearInterfaceList(mDNS *const m)
	{
	while (m->HostInterfaces)
		{
		NetworkInterfaceInfo2 *info = (NetworkInterfaceInfo2*)(m->HostInterfaces);
		mDNS_DeregisterInterface(m, &info->ifinfo);
		if (info->socket1 > 0) shutdown(info->socket1, 2);
		if (info->socket2 > 0) shutdown(info->socket2, 2);
		if (info->cfsocket1) { CFSocketInvalidate(info->cfsocket1); CFRelease(info->cfsocket1); }
		if (info->cfsocket2) { CFSocketInvalidate(info->cfsocket2); CFRelease(info->cfsocket2); }
		free(info);
		}
	}

mDNSlocal mStatus SetupInterfaceList(mDNS *const m)
	{
	struct ifaddrs *ifalist;
	int err = getifaddrs(&ifalist);
	struct ifaddrs *ifa = ifalist;
	if (err) return(err);

	while (ifa)
		{
#if 0
		if (ifa->ifa_addr->sa_family != AF_INET)
			debugf("SetupInterface: %s Flags %04X Family %d not AF_INET",
				ifa->ifa_name, ifa->ifa_flags, ifa->ifa_addr->sa_family);
		if (!(ifa->ifa_flags & IFF_UP))
			debugf("SetupInterface: %s Flags %04X Interface not IFF_UP",
				ifa->ifa_name, ifa->ifa_flags, ifa->ifa_addr->sa_family);
		if (ifa->ifa_flags & IFF_LOOPBACK)
			debugf("SetupInterface: %s Flags %04X Interface IFF_LOOPBACK",
				ifa->ifa_name, ifa->ifa_flags, ifa->ifa_addr->sa_family);
#endif
		if (ifa->ifa_addr->sa_family == AF_INET && (ifa->ifa_flags & IFF_UP) && !(ifa->ifa_flags & IFF_LOOPBACK))
			{
			NetworkInterfaceInfo2 *info = malloc(sizeof(NetworkInterfaceInfo2));
			if (!info) debugf("SetupInterfaceList: Out of Memory!");
			else SetupInterface(m, info, ifa);
			}
		ifa = ifa->ifa_next;
		}
	freeifaddrs(ifalist);
	return(err);
	}

mDNSlocal mStatus mDNSPlatformInit_setup(mDNS *const m)
    {
    CFRunLoopTimerContext myCFRunLoopTimerContext = { 0, m, NULL, NULL, NULL };
	StartTime = CFAbsoluteTimeGetCurrent();
	
	// Set up the nice label
	m->nicelabel.c[0] = 0;
	GetUserSpecifiedComputerName(&m->nicelabel);
	if (m->nicelabel.c[0] == 0) ConvertCStringToDomainLabel("Macintosh", &m->nicelabel);

	// Set up the RFC 1034-compliant label
	m->hostlabel.c[0] = 0;
	ConvertUTF8PstringToRFC1034HostLabel(m->nicelabel.c, &m->hostlabel);
	if (m->hostlabel.c[0] == 0) ConvertCStringToDomainLabel("Macintosh", &m->hostlabel);

    mDNS_GenerateFQDN(m);

	m->p->cftimer = CFRunLoopTimerCreate(kCFAllocatorDefault, CFAbsoluteTimeGetCurrent() + 10.0, 10.0, 0, 1,
											myCFRunLoopTimerCallBack, &myCFRunLoopTimerContext);
    CFRunLoopAddTimer(CFRunLoopGetCurrent(), m->p->cftimer, kCFRunLoopDefaultMode);

	SetupInterfaceList(m);

    return(mStatus_NoError);
    }

mDNSexport mStatus mDNSPlatformInit(mDNS *const m)
	{
	mStatus result = mDNSPlatformInit_setup(m);
	m->mDNSPlatformStatus = result;
	if (m->Callback) m->Callback(m, mStatus_NoError);
    return(result);
	}

mDNSexport void mDNSPlatformClose(mDNS *const m)
    {
    if (m->p->cftimer)
    	{
	    CFRunLoopTimerInvalidate(m->p->cftimer);
    	CFRelease(m->p->cftimer);
    	m->p->cftimer = NULL;
    	}
	ClearInterfaceList(m);
    }

// To Do: Find out how to implement a proper modular time function in CF
mDNSexport void mDNSPlatformScheduleTask(const mDNS *const m, SInt32 NextTaskTime)
	{
	// Due to a bug in CFRunLoopTimers, if you set them to any time in the past, they don't work
	// Spot the obvious race condition: What defines "past"?
	CFAbsoluteTime bugfix = CFAbsoluteTimeGetCurrent() + 0.01;		// Add some slop to reduce risk of race condition
	UInt32 x = (UInt32)NextTaskTime;
	CFAbsoluteTime firetime = StartTime + ((CFAbsoluteTime)x / (CFAbsoluteTime)mDNSPlatformOneSecond);
	if (firetime < bugfix) firetime = bugfix;
	CFRunLoopTimerSetNextFireDate(m->p->cftimer, firetime);
	}

// Locking is a no-op here, because we're CFRunLoop-based, so we can never interrupt ourselves
mDNSexport void    mDNSPlatformLock   (const mDNS *const m) { }
mDNSexport void    mDNSPlatformUnlock (const mDNS *const m) { }
mDNSexport void    mDNSPlatformStrCopy(const void *src,       void *dst)             { strcpy(dst, src); }
mDNSexport UInt32  mDNSPlatformStrLen (const void *src)                              { return(strlen((char*)src)); }
mDNSexport void    mDNSPlatformMemCopy(const void *src,       void *dst, UInt32 len) { memcpy(dst, src, len); }
mDNSexport Boolean mDNSPlatformMemSame(const void *src, const void *dst, UInt32 len) { return(memcmp(dst, src, len) == 0); }
mDNSexport void    mDNSPlatformMemZero(                       void *dst, UInt32 len) { bzero(dst, len); }
mDNSexport SInt32  mDNSPlatformTimeNow() { return((SInt32)((CFAbsoluteTimeGetCurrent() - StartTime) * (CFAbsoluteTime)mDNSPlatformOneSecond)); }
mDNSexport SInt32  mDNSPlatformOneSecond = 1000;
