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
#include <ifaddrs.h>

// ***************************************************************************
// Constants

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

typedef struct
	{
	HostRecordSet hostrecords;
	mDNS *m;
	int socket1;
	int socket2;
    CFSocketRef cfsocket1;
    CFSocketRef cfsocket2;
	} InterfaceInfo;

mDNSexport mStatus mDNSPlatformSendUDP(const mDNS *const m, const DNSMessage *const msg, const mDNSu8 *const end,
	mDNSIPAddr src, mDNSIPPort srcport, mDNSIPAddr dst, mDNSIPPort dstport)
	{
	int err = -1;
	InterfaceInfo *info = (InterfaceInfo *)(m->HostRecords);
	struct sockaddr_in to;
	to.sin_family      = AF_INET;
	to.sin_port        = dstport.NotAnInteger;
	to.sin_addr.s_addr = dst.    NotAnInteger;

	while (info)
		{
		int s = (srcport.NotAnInteger == MulticastDNSPort.NotAnInteger) ? info->socket1 : info->socket2;
		err = sendto(s, msg, (UInt8*)end - (UInt8*)msg, 0, (struct sockaddr *)&to, sizeof(to));
		info = (InterfaceInfo *)(info->hostrecords.next);
		}

	if (err < 0) { perror("mDNSPlatformSendUDP sendto"); return(err); }

	return(mStatus_NoError);
	}

mDNSlocal void myCFSocketCallBack(CFSocketRef s, CFSocketCallBackType type, CFDataRef address, const void *data, void *context)
    {
	mDNSIPAddr senderaddr, destaddr, interface;
	mDNSIPPort senderport, destport;
	InterfaceInfo *info = (InterfaceInfo *)context;
	int skt = (s == info->cfsocket1) ? info->socket1 : info->socket2;
	mDNS *const m = info->m;
	DNSMessage packet;
	struct sockaddr_in from;
	int err, fromlen = sizeof(from);
	if (type != kCFSocketReadCallBack) debugf("myCFSocketCallBack: Why is type not kCFSocketReadCallBack?");
	err = recvfrom(skt, &packet, sizeof(packet), 0, (struct sockaddr *) &from, &fromlen);

	senderaddr.NotAnInteger = from.sin_addr.s_addr;
	senderport.NotAnInteger = from.sin_port;
	destaddr  = AllDNSLinkGroup;		// For now, until I work out how to get the dest address, assume it was sent to AllDNSLinkGroup
	destport  = (s == info->cfsocket1) ? MulticastDNSPort : UnicastDNSPort;
	interface = m->HostRecords->ip;

	debugf("myCFSocketCallBack got a packet from %.4a", &senderaddr);

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
	CFStringRef cfs = CSCopyMachineName();
	CFStringGetPascalString(cfs, namelabel->c, sizeof(*namelabel), kCFStringEncodingUTF8);
	CFRelease(cfs);
	}

mDNSlocal mStatus SetupSocket(struct sockaddr_in *ifa_addr, mDNSIPPort port, int *s, CFSocketRef *c, CFSocketContext *context)
	{
	mStatus err;
	struct ip_mreq imr;
	const int on = 1;
	struct sockaddr_in server_sockaddr;
	CFRunLoopSourceRef rls;

	// Open the sockets
    *s = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	*c = NULL;
	if (*s < 0) { perror("socket"); return(*s); }
	
	// Add multicast group membership
	err = setsockopt(*s, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
	if (err < 0) { perror("setsockopt - SO_REUSEPORT"); return(err); }
	imr.imr_multiaddr.s_addr = AllDNSLinkGroup.NotAnInteger;
	imr.imr_interface.s_addr = 0;
	err = setsockopt(*s, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imr, sizeof(struct ip_mreq));
	if (err < 0) { perror("setsockopt - IP_ADD_MEMBERSHIP"); return(err); }

	// And start listening
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_port = port.NotAnInteger;
	// Binding to the interface address seems to stop multicasts being received
//	server_sockaddr.sin_addr.s_addr = ifa_addr->sin_addr.s_addr;
	server_sockaddr.sin_addr.s_addr = 0;
	err = bind(*s, (struct sockaddr *) &server_sockaddr, sizeof(server_sockaddr));
	if (err < 0 && port.NotAnInteger != UnicastDNSPort.NotAnInteger) perror("bind");
	if (err) return(err);

    *c = CFSocketCreateWithNative(NULL, *s, kCFSocketReadCallBack, myCFSocketCallBack, context);
    rls = CFSocketCreateRunLoopSource(NULL, *c, 0);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), rls, kCFRunLoopDefaultMode);
    CFRelease(rls);
	
	return(err);
	}

mDNSlocal mStatus SetupInterface(mDNS *const m, InterfaceInfo *info, struct ifaddrs *ifa)
	{
	extern int use_53;
	mStatus err;
	struct sockaddr_in *ifa_addr = (struct sockaddr_in *)ifa->ifa_addr;
    CFSocketContext myCFSocketContext = { 0, info, NULL, NULL, NULL };

	debugf("SetupInterface: %s Flags %04X %.4a Registered",
		ifa->ifa_name, ifa->ifa_flags, &ifa_addr->sin_addr.s_addr);

	info->hostrecords.ip.NotAnInteger = ifa_addr->sin_addr.s_addr;
	mDNS_RegisterHostSet(m, &info->hostrecords);
	
	info->m = m;

	err =     SetupSocket(ifa_addr, MulticastDNSPort, &info->socket1, &info->cfsocket1, &myCFSocketContext);
	if (!err && use_53)
		err = SetupSocket(ifa_addr, UnicastDNSPort, &info->socket2, &info->cfsocket2, &myCFSocketContext);
	
	return(err);
	}

mDNSlocal void ClearInterfaceList(mDNS *const m)
	{
	while (m->HostRecords)
		{
		InterfaceInfo *info = (InterfaceInfo*)(m->HostRecords);
		mDNS_DeregisterHostSet(m, &info->hostrecords);
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
		if (ifa->ifa_addr->sa_family != AF_INET)
			debugf("SetupInterfaceList: %s Flags %04X Family %d not AF_INET",
				ifa->ifa_name, ifa->ifa_flags, ifa->ifa_addr->sa_family);
		else if (!(ifa->ifa_flags & IFF_UP))
			debugf("SetupInterfaceList: %s Flags %04X Interface not IFF_UP",
				ifa->ifa_name, ifa->ifa_flags, ifa->ifa_addr->sa_family);
		else if (ifa->ifa_flags & IFF_LOOPBACK)
			debugf("SetupInterfaceList: %s Flags %04X Interface IFF_LOOPBACK",
				ifa->ifa_name, ifa->ifa_flags, ifa->ifa_addr->sa_family);
		else
			{
			InterfaceInfo *info = malloc(sizeof(InterfaceInfo));
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
	
	// Set up the nice label
	m->nicelabel.c[0] = 0;
	GetUserSpecifiedComputerName(&m->nicelabel);
	if (m->nicelabel.c[0] == 0) ConvertCStringToDomainLabel("Macintosh", &m->nicelabel);

	// Set up the RFC 1034-compliant label
	m->hostlabel.c[0] = 0;
	ConvertUTF8PstringToRFC1034HostLabel(m->nicelabel.c, &m->hostlabel);
	if (m->hostlabel.c[0] == 0) ConvertCStringToDomainLabel("Macintosh", &m->hostlabel);

    mDNS_GenerateFQDN(m);

	m->p->cftimer = CFRunLoopTimerCreate(NULL, CFAbsoluteTimeGetCurrent() + 0.5, 0.5, 0, 1,
											myCFRunLoopTimerCallBack, &myCFRunLoopTimerContext);
    CFRunLoopAddTimer(CFRunLoopGetCurrent(), m->p->cftimer, kCFRunLoopDefaultMode);

	SetupInterfaceList(m);
	
    return(mStatus_NoError);
    }

mDNSexport mStatus mDNSPlatformInit(mDNS *const m)
	{
	mStatus result = mDNSPlatformInit_setup(m);
	m->mDNSPlatformStatus = result;
	if (m->initComplete) m->initComplete(m);
    return(result);
	}

mDNSexport void mDNSPlatformClose(mDNS *const m)
    {
    CFRunLoopTimerInvalidate(m->p->cftimer);
    CFRelease(m->p->cftimer);
	ClearInterfaceList(m);
    }

// To Do: Find out how to implement a proper modular time function in CF
mDNSexport void mDNSPlatformScheduleTask(const mDNS *const m, SInt32 NextTaskTime)
	{
	// Due to a bug in CFRunLoopTimers, if you set them to any time in the past, they don't work
	// Spot the obvious race condition: What defines "past"?
	CFAbsoluteTime bugfix = CFAbsoluteTimeGetCurrent() + 0.01;		// Add some slop to reduce risk of race condition
	CFAbsoluteTime firetime = (CFAbsoluteTime)(UInt32)NextTaskTime / (CFAbsoluteTime)mDNSPlatformOneSecond;
	if (firetime < bugfix) firetime = bugfix;
	CFRunLoopTimerSetNextFireDate(m->p->cftimer, firetime);
	}

mDNSexport void    mDNSPlatformLock   (const mDNS *const m) { }
mDNSexport void    mDNSPlatformUnlock (const mDNS *const m) { }
mDNSexport void    mDNSPlatformStrCopy(const void *src,       void *dst)             { strcpy(dst, src); }
mDNSexport UInt32  mDNSPlatformStrLen (const void *src)                              { return(strlen((char*)src)); }
mDNSexport void    mDNSPlatformMemCopy(const void *src,       void *dst, UInt32 len) { memcpy(dst, src, len); }
mDNSexport Boolean mDNSPlatformMemSame(const void *src, const void *dst, UInt32 len) { return(memcmp(dst, src, len) == 0); }
mDNSexport void    mDNSPlatformMemZero(                       void *dst, UInt32 len) { bzero(dst, len); }
mDNSexport SInt32  mDNSPlatformTimeNow() { return((SInt32)(CFAbsoluteTimeGetCurrent() * (CFAbsoluteTime)mDNSPlatformOneSecond)); }
mDNSexport SInt32  mDNSPlatformOneSecond = 10;
