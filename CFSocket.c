/*
 * Copyright (c) 2002 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 *
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 1.1 (the
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
 */

// ***************************************************************************
// mDNS-CFSocket.c:
// Supporting routines to run mDNS on a CFRunLoop platform
// ***************************************************************************

#include "mDNSEnvironment.h"
#include "mDNSPlatformFunctions.h"		// Defines the interface to the supporting layer below
#include "mDNSsprintf.h"
#include "mDNSvsprintf.h"

// ***************************************************************************
// Constants

// ***************************************************************************
// Functions

export void debugf_(const char *format, ...)
	{
	unsigned char buffer[256];
    va_list ptr;
	va_start(ptr,format);
	buffer[mDNS_vsprintf(buffer, format, ptr)] = 0;
	va_end(ptr);
	fprintf(stderr, "%s\n", buffer);
	fflush(stderr);
	}

export mStatus mDNSPlatformSendUDP(const mDNS *const m, const DNSMessage *const msg, const void *const end, IPAddr src, IPAddr dest, IPPort port)
	{
	struct sockaddr_in to;
	int err;
	to.sin_family      = AF_INET;
	to.sin_port        = port.NotAnInteger;
	to.sin_addr.s_addr = dest.NotAnInteger;

	err = sendto(m->p->socket, msg, (UInt8*)end - (UInt8*)msg, 0, (struct sockaddr *)&to, sizeof(to));
	if (err < 0) { perror("mDNSPlatformSendUDP sendto"); return(err); }

	return(mStatus_NoError);
	}

static void myCFSocketCallBack(CFSocketRef s, CFSocketCallBackType type, CFDataRef address, const void *data, void *info)
    {
	IPAddr senderaddr, destaddr, interface;
	IPPort senderport;
	mDNS *const m = (mDNS *const)info;
	DNSMessage packet;
	struct sockaddr_in from;
	int err, fromlen = sizeof(from);
	if (type != kCFSocketReadCallBack) debugf("myCFSocketCallBack: Why is type not kCFSocketReadCallBack?");
	err = recvfrom(m->p->socket, &packet, sizeof(packet), 0, (struct sockaddr *) &from, &fromlen);

	senderaddr.NotAnInteger = from.sin_addr.s_addr;
	senderport.NotAnInteger = from.sin_port;
	destaddr  = AllDNSLinkGroup;		// For now, until I work out how to get the dest address, assume it was sent to AllDNSLinkGroup
	interface = m->ip;

	debugf("myCFSocketCallBack got a packet");

	if (err < 0) debugf("myCFSocketCallBack recvfrom error %d", err);
	else if (err < sizeof(DNSMessageHeader)) debugf("myCFSocketCallBack packet length (%d) too short", err);
	else mDNSCoreReceive(m, &packet, (char*)&packet + err, senderaddr, senderport, destaddr, interface);
    }

static void myCFRunLoopTimerCallBack(CFRunLoopTimerRef timer, void *info)
    {
	mDNSCoreTask((mDNS *const)info);
    }

//CF_EXPORT void CFRunLoopTimerSetNextFireDate(CFRunLoopTimerRef timer, CFAbsoluteTimeGetCurrent() + seconds);

local mStatus mDNSPlatformInit_setup(mDNS *const m)
    {
    CFSocketContext       myCFSocketContext       = { 0, m, NULL, NULL, NULL };
    CFRunLoopTimerContext myCFRunLoopTimerContext = { 0, m, NULL, NULL, NULL };
	CFRunLoopSourceRef    rls;
	
	struct ip_mreq imr;
	const int on = 1;
	struct sockaddr_in server_sockaddr;
	mStatus err;

	// Open the socket
    m->p->socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m->p->socket < 0) { perror("socket"); return(m->p->socket); }
	
	// Add multicast group membership
	err = setsockopt(m->p->socket, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
	if (err < 0) { perror("setsockopt - SO_REUSEPORT"); return(err); }
	imr.imr_multiaddr.s_addr = AllDNSLinkGroup.NotAnInteger;
	imr.imr_interface.s_addr = 0;
	err = setsockopt(m->p->socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imr, sizeof(struct ip_mreq));
	if (err < 0) { perror("setsockopt - IP_ADD_MEMBERSHIP"); return(err); }

	// And start listening
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_port = NameServerPort.NotAnInteger;
	server_sockaddr.sin_addr.s_addr = 0;
	err = bind(m->p->socket, (struct sockaddr *) &server_sockaddr, sizeof(server_sockaddr));
	if (err < 0) { perror("bind"); return(err); }

    m->p->cfsocket = CFSocketCreateWithNative(NULL, m->p->socket, kCFSocketReadCallBack, myCFSocketCallBack, &myCFSocketContext);
    rls = CFSocketCreateRunLoopSource(NULL, m->p->cfsocket, 0);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), rls, kCFRunLoopDefaultMode);
    CFRelease(rls);

	m->p->cftimer = CFRunLoopTimerCreate(NULL, CFAbsoluteTimeGetCurrent() + 0.5, 0.5, 0, 1, myCFRunLoopTimerCallBack, &myCFRunLoopTimerContext);
    CFRunLoopAddTimer(CFRunLoopGetCurrent(), m->p->cftimer, kCFRunLoopDefaultMode);

    return(mStatus_NoError);
    }

export mStatus mDNSPlatformInit(mDNS *const m)
	{
	mStatus result = mDNSPlatformInit_setup(m);
	m->mDNSPlatformStatus = result;
	if (m->initComplete) m->initComplete(m);
    return(result);
	}

extern void mDNSPlatformClose(const mDNS *const m)
    {
    CFSocketInvalidate(m->p->cfsocket);
    CFRelease(m->p->cfsocket);

    CFRunLoopTimerInvalidate(m->p->cftimer);
    CFRelease(m->p->cftimer);
    }

export void mDNSPlatformScheduleTask(const mDNS *const m, SInt32 NextTaskTime)
	{
	//CFRunLoopTimerSetNextFireDate(m->p->cftimer, NextTaskTime / mDNSPlatformOneSecond);
	}

export       void    mDNSPlatformLock   (const mDNS *const m) { }
export       void    mDNSPlatformUnlock (const mDNS *const m) { }
export       void    mDNSPlatformStrCopy(const void *src,       void *dst)             { strcpy(dst, src); }
export       void    mDNSPlatformMemCopy(const void *src,       void *dst, UInt32 len) { memcpy(dst, src, len); }
export       Boolean mDNSPlatformMemSame(const void *src, const void *dst, UInt32 len) { return(memcmp(dst, src, len)); }
export       void    mDNSPlatformMemZero(                       void *dst, UInt32 len) { bzero(dst, len); }
export       SInt32  mDNSPlatformTimeNow()                                             { return((SInt32)(CFAbsoluteTimeGetCurrent() * mDNSPlatformOneSecond)); }
export const SInt32  mDNSPlatformOneSecond = 10;

