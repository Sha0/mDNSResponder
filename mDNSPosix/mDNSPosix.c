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

 * Formatting notes:
 * This code follows the "Whitesmiths style" C indentation rules. Plenty of discussion
 * on C indentation can be found on the web, such as <http://www.kafejo.com/komp/1tbs.htm>,
 * but for the sake of brevity here I will say just this: Curly braces are not syntactially
 * part of an "if" statement; they are the beginning and ending markers of a compound statement;
 * therefore common sense dictates that if they are part of a compound statement then they
 * should be indented to the same level as everything else in that compound statement.
 * Indenting curly braces at the same level as the "if" implies that curly braces are
 * part of the "if", which is false. (This is as misleading as people who write "char* x,y;"
 * thinking that variables x and y are both of type "char*" -- and anyone who doesn't
 * understand why variable y is not of type "char*" just proves the point that poor code
 * layout leads people to unfortunate misunderstandings about how the C language really works.)

	Change History (most recent first):

$Log: mDNSPosix.c,v $
Revision 1.7  2003/03/13 03:46:21  cheshire
Fixes to make the code build on Linux

Revision 1.6  2003/03/08 00:35:56  cheshire
Switched to using new "mDNS_Execute" model (see "mDNSCore/Implementer Notes.txt")

Revision 1.5  2002/12/23 22:13:31  jgraessl
Reviewed by: Stuart Cheshire
Initial IPv6 support for mDNSResponder.

Revision 1.4  2002/09/27 01:47:45  cheshire
Workaround for Linux 2.0 systems that don't have IP_PKTINFO

Revision 1.3  2002/09/21 20:44:53  zarzycki
Added APSL info

Revision 1.2  2002/09/19 21:25:36  cheshire
mDNS_sprintf() doesn't need to be in a separate file

Revision 1.1  2002/09/17 06:24:34  cheshire
First checkin
*/

#include "mDNSClientAPI.h"           // Defines the interface provided to the client layer above
#include "mDNSPlatformFunctions.h"   // Defines the interface to the supporting layer below
#include "mDNSPosix.h"				 // Defines the specific types needed to run mDNS on this platform

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <netinet/in.h>

#include "mDNSUNP.h"

// ***************************************************************************
// Structures

// PosixNetworkInterface is a record extension of the core NetworkInterfaceInfo
// type that supports extra fields needed by the Posix platform.
//
// IMPORTANT: coreIntf must be the first field in the structure because
// we cast between pointers to the two different types regularly.

typedef struct PosixNetworkInterface PosixNetworkInterface;

struct PosixNetworkInterface
	{
	NetworkInterfaceInfo    coreIntf;
	const char *            intfName;
	PosixNetworkInterface * aliasIntf;
	int                     index;
	int                     multicastSocket;
	int                     multicastSocketv6;
	};

// ***************************************************************************
// Functions

int gMDNSPlatformPosixVerboseLevel = 0;

// Note, this uses mDNS_vsprintf instead of standard "vsprintf", because mDNS_vsprintf knows
// how to print special data types like IP addresses and length-prefixed domain names
mDNSexport void debugf_(const char *format, ...)
	{
	unsigned char buffer[512];
	va_list ptr;
	va_start(ptr,format);
	buffer[mDNS_vsprintf((char *)buffer, format, ptr)] = 0;
	va_end(ptr);
	if (gMDNSPlatformPosixVerboseLevel >= 1)
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
	if (gMDNSPlatformPosixVerboseLevel >= 2)
		fprintf(stderr, "%s\n", buffer);
	fflush(stderr);
	}

#define PosixErrorToStatus(errNum) ((errNum) == 0 ? mStatus_NoError : mStatus_UnknownErr)

static void SockAddrTomDNSAddr(const struct sockaddr *const s_addr, mDNSAddr *ipAddr, mDNSIPPort *ipPort)
	{
	switch (s_addr->sa_family)
		{
		case AF_INET:
			{
			struct sockaddr_in* sin          = (struct sockaddr_in*)s_addr;
			ipAddr->type                     = mDNSAddrType_IPv4;
			ipAddr->addr.ipv4.NotAnInteger   = sin->sin_addr.s_addr;
			if (ipPort) ipPort->NotAnInteger = sin->sin_port;
			break;
			}

#ifdef mDNSIPv6Support
		case AF_INET6:
			{
			struct sockaddr_in6* sin6        = (struct sockaddr_in6*)s_addr;
			assert(sin6->sin6_len == sizeof(*sin6));
			ipAddr->type                     = mDNSAddrType_IPv6;
			ipAddr->addr.ipv6                = *(mDNSv6Addr*)&sin6->sin6_addr;
			if (ipPort) ipPort->NotAnInteger = sin6->sin6_port;
			break;
			}
#endif

		default:
			verbosedebugf("SockAddrTomDNSAddr: Uknown address family %d\n", s_addr->sa_family);
			ipAddr->type = mDNSAddrType_None;
			if (ipPort) ipPort->NotAnInteger = 0;
			break;
		}
	}

#pragma mark ***** Send and Receive

// mDNS core calls this routine when it needs to send a packet.
mDNSexport mStatus mDNSPlatformSendUDP(const mDNS *const m, const DNSMessage *const msg, const mDNSu8 *const end,
	mDNSOpaqueID InterfaceID, mDNSIPPort srcPort, const mDNSAddr *dst, mDNSIPPort dstPort)
	{
	int                     err;
	struct sockaddr_storage to;
	PosixNetworkInterface * thisIntf;

	assert(m != NULL);
	assert(msg != NULL);
	assert(end != NULL);
	assert( (((char *) end) - ((char *) msg)) > 0 );
	assert(InterfaceID != 0); // Can't send from zero source address
	assert(srcPort.NotAnInteger != 0);     // Nor from a zero source port
	assert(dstPort.NotAnInteger != 0);     // Nor from a zero source port

	if (dst->type == mDNSAddrType_IPv4)
		{
		struct sockaddr_in *sin = (struct sockaddr_in*)&to;
#if HAVE_SOCKADDR_SA_LEN
		sin->sin_len            = sizeof(*sin);
#endif
		sin->sin_family         = AF_INET;
		sin->sin_port           = dstPort.NotAnInteger;
		sin->sin_addr.s_addr    = dst->addr.ipv4.NotAnInteger;
		}

#ifdef mDNSIPv6Support
	else if (dst->type == mDNSAddrType_IPv6)
		{
		struct sockaddr_in6 *sin6 = (struct sockaddr_in6*)&to;
		mDNSPlatformMemZero(sin6, sizeof(*sin6));
		sin6->sin6_len            = sizeof(*sin6);
		sin6->sin6_family         = AF_INET6;
		sin6->sin6_port           = dstPort.NotAnInteger;
		sin6->sin6_addr           = *(struct in6_addr*)&dst->addr.ipv6;
		}
#endif

	err = 0;
	thisIntf = (PosixNetworkInterface *)(InterfaceID);
	if (dst->type == mDNSAddrType_IPv4)
		err = sendto(thisIntf->multicastSocket, msg, (char*)end - (char*)msg, 0, (struct sockaddr *)&to, GET_SA_LEN(to));

#ifdef mDNSIPv6Support
	else if (dst->type == mDNSAddrType_IPv6)
		err = sendto(thisIntf->multicastSocketv6, msg, (char*)end - (char*)msg, 0, (struct sockaddr *)&to, GET_SA_LEN(to));
#endif

	if (err > 0) err = 0;
	else if (err < 0)
		verbosedebugf("mDNSPlatformSendUDP got error %d (%s) sending packet to %#a on interface %#a/%s/%d",
					  errno, strerror(errno), dst, &thisIntf->coreIntf.ip, thisIntf->intfName, thisIntf->index);

	return PosixErrorToStatus(err);
	}

// This routine is called when the main loop detects that data is available on a socket.
static void SocketDataReady(mDNS *const m, PosixNetworkInterface *intf, int skt)
	{
	mDNSAddr   senderAddr, destAddr;
	mDNSIPPort senderPort;
	ssize_t                 packetLen;
	DNSMessage              packet;
	struct my_in_pktinfo    packetInfo;
	struct sockaddr_storage from;
	socklen_t               fromLen;
	int                     flags;
	mDNSBool                reject;

	assert(m    != NULL);
	assert(intf != NULL);
	assert(skt  >= 0);

	fromLen = sizeof(from);
	flags   = 0;
	packetLen = recvfrom_flags(skt, &packet, sizeof(packet), &flags, (struct sockaddr *) &from, &fromLen, &packetInfo);

	if (packetLen >= 0)
		{
		SockAddrTomDNSAddr((struct sockaddr*)&from, &senderAddr, &senderPort);
		SockAddrTomDNSAddr((struct sockaddr*)&packetInfo.ipi_addr, &destAddr, NULL);

		// If we have broken IP_RECVDSTADDR functionality (so far
		// I've only seen this on OpenBSD) then apply a hack to
		// convince mDNS Core that this isn't a spoof packet.
		// Basically what we do is check to see whether the
		// packet arrived as a multicast and, if so, set its
		// destAddr to the mDNS address.
		//
		// I must admit that I could just be doing something
		// wrong on OpenBSD and hence triggering this problem
		// but I'm at a loss as to how.
		//
		// If this platform doesn't have IP_PKTINFO or IP_RECVDSTADDR, then we have
		// no way to tell the destination address or interface this packet arrived on,
		// so all we can do is just assume it's a multicast

		#if HAVE_BROKEN_RECVDSTADDR || (!defined(IP_PKTINFO) && !defined(IP_RECVDSTADDR))
			if ( (destAddr.NotAnInteger == 0) && (flags & MSG_MCAST) )
				{
				destAddr.type == senderAddr.type;
				if      (senderAddr.type == mDNSAddrType_IPv4) destAddr.addr.ipv4 = AllDNSLinkGroup;
				else if (senderAddr.type == mDNSAddrType_IPv6) destAddr.addr.ipv6 = AllDNSLinkGroupv6;
				}
		#endif

		// We only accept the packet if the interface on which it came
		// in matches the interface associated with this socket.
		// We do this match by name or by index, depending on which
		// information is available.  recvfrom_flags sets the name
		// to "" if the name isn't available, or the index to -1
		// if the index is available.  This accomodates the various
		// different capabilities of our target platforms.

		reject = mDNSfalse;
		if      ( packetInfo.ipi_ifname[0] != 0 ) reject = (strcmp(packetInfo.ipi_ifname, intf->intfName) != 0);
		else if ( packetInfo.ipi_ifindex != -1 )  reject = (packetInfo.ipi_ifindex != intf->index);

		if (reject)
			{
			debugf("SocketDataReady ignored a packet from %#a to %#a on interface %s/%d expecting %#a/%s/%d",
				&senderAddr, &destAddr, packetInfo.ipi_ifname, packetInfo.ipi_ifindex,
				&intf->coreIntf.ip, intf->intfName, intf->index);
			packetLen = -1;
			}
		else
			verbosedebugf("SocketDataReady got a packet from %#a to %#a on interface %#a/%s/%d",
				&senderAddr, &destAddr, &intf->coreIntf.ip, intf->intfName, intf->index);
		}

	if (packetLen >= 0 && packetLen < sizeof(DNSMessageHeader))
		{
		debugf("SocketDataReady packet length (%d) too short", packetLen);
		packetLen = -1;
		}

	if (packetLen >= 0)
		mDNSCoreReceive(m, &packet, (mDNSu8 *)&packet + packetLen,
			&senderAddr, senderPort, &destAddr, MulticastDNSPort, intf->coreIntf.InterfaceID);
	}

#pragma mark ***** Init and Term

// On OS X this gets the text of the field labelled "Computer Name" in the Sharing Prefs Control Panel
// Other platforms can either get the information from the appropriate place,
// or they can alternatively just require all registering services to provide an explicit name
mDNSlocal void GetUserSpecifiedFriendlyComputerName(domainlabel *const namelabel)
	{
	ConvertCStringToDomainLabel("Fill in Default Service Name Here", namelabel);
	}

// This gets the current hostname, truncating it at the first dot if necessary
mDNSlocal void GetUserSpecifiedRFC1034ComputerName(domainlabel *const namelabel)
	{
	int len = 0;
	gethostname(&namelabel->c[1], MAX_DOMAIN_LABEL);
	while (len < MAX_DOMAIN_LABEL && namelabel->c[len+1] && namelabel->c[len+1] != '.') len++;
	namelabel->c[0] = len;
	}

// Searches the interface list looking for the named interface.
// Returns a pointer to if it found, or NULL otherwise.
static PosixNetworkInterface *SearchForInterfaceByName(mDNS *const m, const char *intfName)
	{
	PosixNetworkInterface *intf;

	assert(m != NULL);
	assert(intfName != NULL);

	intf = (PosixNetworkInterface*)(m->HostInterfaces);
	while ( (intf != NULL) && (strcmp(intf->intfName, intfName) != 0) )
		intf = (PosixNetworkInterface *)(intf->coreIntf.next);

	return intf;
	}

// Frees the specified PosixNetworkInterface structure. The underlying
// interface must have already been deregistered with the mDNS core.
static void FreePosixNetworkInterface(PosixNetworkInterface *intf)
	{
	assert(intf != NULL);
	if (intf->intfName != NULL)        free((void *)intf->intfName);
	if (intf->multicastSocket   != -1) assert(close(intf->multicastSocket) == 0);
	if (intf->multicastSocketv6 != -1) assert(close(intf->multicastSocketv6) == 0);
	free(intf);
	}

// Grab the first interface, deregister it, free it, and repeat until done.
static void ClearInterfaceList(mDNS *const m)
	{
	assert(m != NULL);

	while (m->HostInterfaces)
		{
		PosixNetworkInterface *intf = (PosixNetworkInterface*)(m->HostInterfaces);
		mDNS_DeregisterInterface(m, &intf->coreIntf);
		if (gMDNSPlatformPosixVerboseLevel > 0) fprintf(stderr, "Deregistered interface %s\n", intf->intfName);
		FreePosixNetworkInterface(intf);
		}
	}

// Sets up a multicast send/receive socket for the specified
// port on the interface specified by the IP addrelss intfAddr.
static int SetupSocket(struct sockaddr *intfAddr, mDNSIPPort port, int interfaceIndex, int *sktPtr)
	{
	int err = 0;
	static const int kOn = 1;
	static const int kIntTwoFiveFive = 255;
	static const unsigned char kByteTwoFiveFive = 255;

	assert(intfAddr != NULL);
	assert(sktPtr != NULL);
	assert(*sktPtr == -1);

	// Open the socket...
	if       (intfAddr->sa_family == AF_INET) *sktPtr = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
#ifdef mDNSIPv6Support
	else if (intfAddr->sa_family == AF_INET6) *sktPtr = socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);
#endif
	else return EINVAL;

	if (*sktPtr < 0) { err = errno; perror("socket"); }

	// ... with a shared UDP port
	if (err == 0)
		{
		#if defined(SO_REUSEPORT)
			err = setsockopt(*sktPtr, SOL_SOCKET, SO_REUSEPORT, &kOn, sizeof(kOn));
		#elif defined(SO_REUSEADDR)
			err = setsockopt(*sktPtr, SOL_SOCKET, SO_REUSEADDR, &kOn, sizeof(kOn));
		#else
			#error This platform has no way to avoid address busy errors on multicast.
		#endif
		if (err < 0) { err = errno; perror("setsockopt - SO_REUSExxxx"); }
		}

	// We want to receive destination addresses and interface identifiers.
	if (intfAddr->sa_family == AF_INET)
		{
		struct ip_mreq imr;
		struct sockaddr_in bindAddr;
		if (err == 0)
			{
			#if defined(IP_PKTINFO)									// Linux
				err = setsockopt(*sktPtr, IPPROTO_IP, IP_PKTINFO, &kOn, sizeof(kOn));
				if (err < 0) { err = errno; perror("setsockopt - IP_PKTINFO"); }
			#elif defined(IP_RECVDSTADDR) || defined(IP_RECVIF)		// BSD and Solaris
				#if defined(IP_RECVDSTADDR)
					err = setsockopt(*sktPtr, IPPROTO_IP, IP_RECVDSTADDR, &kOn, sizeof(kOn));
					if (err < 0) { err = errno; perror("setsockopt - IP_RECVDSTADDR"); }
				#endif
				#if defined(IP_RECVIF)
					if (err == 0)
						{
						err = setsockopt(*sktPtr, IPPROTO_IP, IP_RECVIF, &kOn, sizeof(kOn));
						if (err < 0) { err = errno; perror("setsockopt - IP_RECVIF"); }
						}
				#endif
			#else
				#warning This platform has no way to get the destination interface information -- will only work for single-homed hosts
			#endif
			}

		// Add multicast group membership on this interface
		if (err == 0)
			{
			imr.imr_multiaddr.s_addr = AllDNSLinkGroup.NotAnInteger;
			imr.imr_interface        = ((struct sockaddr_in*)intfAddr)->sin_addr;
			err = setsockopt(*sktPtr, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imr, sizeof(imr));
			if (err < 0) { err = errno; perror("setsockopt - IP_ADD_MEMBERSHIP"); }
			}

		// Specify outgoing interface too
		if (err == 0)
			{
			err = setsockopt(*sktPtr, IPPROTO_IP, IP_MULTICAST_IF, &((struct sockaddr_in*)intfAddr)->sin_addr, sizeof(struct in_addr));
			if (err < 0) { err = errno; perror("setsockopt - IP_MULTICAST_IF"); }
			}

		// Per the mDNS spec, send unicast packets with TTL 255
		if (err == 0)
			{
			err = setsockopt(*sktPtr, IPPROTO_IP, IP_TTL, &kIntTwoFiveFive, sizeof(kIntTwoFiveFive));
			if (err < 0) { err = errno; perror("setsockopt - IP_TTL"); }
			}

		// and multicast packets with TTL 255 too
		// There's some debate as to whether IP_MULTICAST_TTL is an int or a byte so we just try both.
		if (err == 0)
			{
			err = setsockopt(*sktPtr, IPPROTO_IP, IP_MULTICAST_TTL, &kByteTwoFiveFive, sizeof(kByteTwoFiveFive));
			if (err < 0 && errno == EINVAL)
				err = setsockopt(*sktPtr, IPPROTO_IP, IP_MULTICAST_TTL, &kIntTwoFiveFive, sizeof(kIntTwoFiveFive));
			if (err < 0) { err = errno; perror("setsockopt - IP_MULTICAST_TTL"); }
			}

		// And start listening for packets
		if (err == 0)
			{
			bindAddr.sin_family      = AF_INET;
			bindAddr.sin_port        = port.NotAnInteger;
			bindAddr.sin_addr.s_addr = 0; // Want to receive multicasts AND unicasts on this socket
			err = bind(*sktPtr, (struct sockaddr *) &bindAddr, sizeof(bindAddr));
			if (err < 0) { err = errno; perror("bind"); fflush(stderr); }
			}
		} // endif (intfAddr->sa_family == AF_INET)

#ifdef mDNSIPv6Support
	else if (intfAddr->sa_family == AF_INET6)
		{
		struct ipv6_mreq imr6;
		struct sockaddr_in6 bindAddr6;
		if (err == 0)
			{
			#if defined(IPV6_PKTINFO)
				err = setsockopt(*sktPtr, IPPROTO_IPV6, IPV6_PKTINFO, &kOn, sizeof(kOn));
				if (err < 0) { err = errno; perror("setsockopt - IPV6_PKTINFO"); }
			#else
				#warning This platform has no way to get the destination interface information for IPv6 -- will only work for single-homed hosts
			#endif
			}

		// Add multicast group membership on this interface
		if (err == 0)
			{
			imr6.ipv6mr_multiaddr       = *(const struct in6_addr*)&AllDNSLinkGroupv6;
			imr6.ipv6mr_interface       = interfaceIndex;
			err = setsockopt(*sktPtr, IPPROTO_IPV6, IPV6_JOIN_GROUP, &imr6, sizeof(imr6));
			if (err < 0)
				{
				err = errno;
				verbosedebugf("IPV6_JOIN_GROUP %.16a on %d failed.\n", &imr6.ipv6mr_multiaddr, imr6.ipv6mr_interface);
				perror("setsockopt - IPV6_JOIN_GROUP");
				}
			}

		// Specify outgoing interface too
		if (err == 0)
			{
			u_int	multicast_if = interfaceIndex;
			err = setsockopt(*sktPtr, IPPROTO_IPV6, IPV6_MULTICAST_IF, &multicast_if, sizeof(multicast_if));
			if (err < 0) { err = errno; perror("setsockopt - IPV6_MULTICAST_IF"); }
			}

		// We want to receive only IPv6 packets on this socket.
		// Without this option, we may get IPv4 addresses as mapped addresses.
		if (err == 0)
			{
			err = setsockopt(*sktPtr, IPPROTO_IPV6, IPV6_V6ONLY, &kOn, sizeof(kOn));
			if (err < 0) { err = errno; perror("setsockopt - IPV6_V6ONLY"); }
			}

		// Per the mDNS spec, send unicast packets with TTL 255
		if (err == 0)
			{
			err = setsockopt(*sktPtr, IPPROTO_IPV6, IPV6_UNICAST_HOPS, &kIntTwoFiveFive, sizeof(kIntTwoFiveFive));
			if (err < 0) { err = errno; perror("setsockopt - IPV6_UNICAST_HOPS"); }
			}

		// and multicast packets with TTL 255 too
		// There's some debate as to whether IPV6_MULTICAST_HOPS is an int or a byte so we just try both.
		if (err == 0)
			{
			err = setsockopt(*sktPtr, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &kByteTwoFiveFive, sizeof(kByteTwoFiveFive));
			if (err < 0 && errno == EINVAL)
				err = setsockopt(*sktPtr, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &kIntTwoFiveFive, sizeof(kIntTwoFiveFive));
			if (err < 0) { err = errno; perror("setsockopt - IPV6_MULTICAST_HOPS"); }
			}

		// And start listening for packets
		if (err == 0)
			{
			mDNSPlatformMemZero(&bindAddr6, sizeof(bindAddr6));
			bindAddr6.sin6_len         = sizeof(bindAddr6);
			bindAddr6.sin6_family      = AF_INET6;
			bindAddr6.sin6_port        = port.NotAnInteger;
			bindAddr6.sin6_flowinfo    = 0;
//			bindAddr6.sin6_addr.s_addr = IN6ADDR_ANY_INIT; // Want to receive multicasts AND unicasts on this socket
			bindAddr6.sin6_scope_id    = 0;
			err = bind(*sktPtr, (struct sockaddr *) &bindAddr6, sizeof(bindAddr6));
			if (err < 0) { err = errno; perror("bind"); fflush(stderr); }
			}
		} // endif (intfAddr->sa_family == AF_INET6)
#endif

	// Set the socket to non-blocking.
	if (err == 0)
		{
		err = fcntl(*sktPtr, F_GETFL, 0);
		if (err < 0) err = errno;
		else
			{
			err = fcntl(*sktPtr, F_SETFL, err | O_NONBLOCK);
			if (err < 0) err = errno;
			}
		}

	// Clean up
	if (err != 0 && *sktPtr != -1) { assert(close(*sktPtr) == 0); *sktPtr = -1; }
	assert( (err == 0) == (*sktPtr != -1) );
	return err;
	}

// Creates a PosixNetworkInterface for the interface whose IP address is
// intfAddr and whose name is intfName and registers it with mDNS core.
static int SetupOneInterface(mDNS *const m, struct sockaddr *intfAddr, const char *intfName, int index)
	{
	int err = 0;
	PosixNetworkInterface *intf;
	PosixNetworkInterface *alias = NULL;

	assert(m != NULL);
	assert(intfAddr != NULL);
	assert(intfName != NULL);

	// Allocate the interface structure itself.
	intf = malloc(sizeof(*intf));
	if (intf == NULL) { assert(0); err = ENOMEM; }

	// And make a copy of the intfName.
	if (err == 0)
		{
		intf->intfName = strdup(intfName);
		if (intf->intfName == NULL) { assert(0); err = ENOMEM; }
		}

	if (err == 0)
		{
		// Set up the fields required by the mDNS core.
		SockAddrTomDNSAddr(intfAddr, &intf->coreIntf.ip, NULL);
		intf->coreIntf.Advertise = m->AdvertiseLocalAddresses;

		// Set up the extra fields in PosixNetworkInterface.
		assert(intf->intfName != NULL);         // intf->intfName already set up above
		intf->index                = if_nametoindex(intf->intfName);
		intf->multicastSocket      = -1;
		intf->multicastSocketv6    = -1;
		alias                      = SearchForInterfaceByName(m, intf->intfName);
		if (alias == NULL) alias   = intf;
		intf->coreIntf.InterfaceID = alias;

		if (alias != intf)
			debugf("SetupOneInterface: %s %#a is an alias of %#a", intfName, &intf->coreIntf.ip, &alias->coreIntf.ip);
		}

	// Set up the multicast socket
	if (err == 0)
		{
		if (alias->multicastSocket == -1 && intfAddr->sa_family == AF_INET)
			err = SetupSocket(intfAddr, MulticastDNSPort, intf->index, &alias->multicastSocket);
#ifdef mDNSIPv6Support
		else if (alias->multicastSocketv6 == -1 && intfAddr->sa_family == AF_INET6)
			err = SetupSocket(intfAddr, MulticastDNSPort, intf->index, &alias->multicastSocketv6);
#endif
		}

	// The interface is all ready to go, let's register it with the mDNS core.
	if (err == 0)
		err = mDNS_RegisterInterface(m, &intf->coreIntf);

	// Clean up.
	if (err == 0)
		{
		debugf("SetupOneInterface: %s %#a Registered", intf->intfName, &intf->coreIntf.ip);
		if (gMDNSPlatformPosixVerboseLevel > 0)
			fprintf(stderr, "Registered interface %s\n", intf->intfName);
		}
	else
		{
		// Use intfName instead of intf->intfName in the next line to avoid dereferencing NULL.
		debugf("SetupOneInterface: %s %#a failed to register %d", intfName, &intf->coreIntf.ip, err);
		if (intf) { FreePosixNetworkInterface(intf); intf = NULL; }
		}

	assert( (err == 0) == (intf != NULL) );

	return err;
	}

static int SetupInterfaceList(mDNS *const m)
	{
	mDNSBool        foundav4       = mDNSfalse;
	int             err            = 0;
	struct ifi_info *intfList      = get_ifi_info(AF_INET, mDNStrue);
	struct ifi_info *firstLoopback = NULL;

	assert(m != NULL);
	debugf("SetupInterfaceList");

	if (intfList == NULL) err = ENOENT;

#ifdef mDNSIPv6Support
	if (err == 0)		/* Link the IPv6 list to the end of the IPv4 list */
		{
		struct ifi_info **p = &intfList;
		while (*p) p = &(*p)->ifi_next;
		*p = get_ifi_info(AF_INET6, mDNStrue);
		}
#endif

	if (err == 0)
		{
		struct ifi_info *i = intfList;
		while (i)
			{
			if (     ((i->ifi_addr->sa_family == AF_INET)
#ifdef mDNSIPv6Support
					  || (i->ifi_addr->sa_family == AF_INET6)
#endif
				) &&  (i->ifi_flags & IFF_UP) )
				{
				// The Mac OS X code also avoids interfaces with the IFF_POINTOPOINT flag set.
				//  This prevents nuisance phone calls when dial-on-demand is enabled.
				// I specifically didn't pull in this feature because most UNIX hosts don't
				// use PPP dial-on-demand.  If you need this you should add the conditional:
				//
				//     && !(i->ifi_flags & IFF_POINTOPOINT)
				//
				// to the above "if" statement.

				if (i->ifi_flags & IFF_LOOPBACK)
					{
					if (firstLoopback == NULL)
						firstLoopback = i;
					}
				else
					{
					if (SetupOneInterface(m, i->ifi_addr, i->ifi_name, i->ifi_index) == 0)
						if (i->ifi_addr->sa_family == AF_INET)
							foundav4 = mDNStrue;
					}
				}
			i = i->ifi_next;
			}

		// If we found no normal interfaces but we did find a loopback interface, register the
		// loopback interface.  This allows self-discovery if no interfaces are configured.
		// Temporary workaround: Multicast loopback on IPv6 interfaces appears not to work.
		// In the interim, we skip loopback interface only if we found at least one v4 interface to use
		// if ( (m->HostInterfaces == NULL) && (firstLoopback != NULL) )
		if ( !foundav4 && firstLoopback )
			(void) SetupOneInterface(m, firstLoopback->ifi_addr, firstLoopback->ifi_name, firstLoopback->ifi_index);
		}

	// Clean up.
	if (intfList != NULL) free_ifi_info(intfList);
	return err;
	}

// mDNS core calls this routine to initialise the platform-specific data.
mDNSexport mStatus mDNSPlatformInit(mDNS *const m)
	{
	int err;
	assert(m != NULL);

	// Tell mDNS core the names of this machine.

	// Set up the nice label
	m->nicelabel.c[0] = 0;
	GetUserSpecifiedFriendlyComputerName(&m->nicelabel);
	if (m->nicelabel.c[0] == 0) ConvertCStringToDomainLabel("Macintosh", &m->nicelabel);

	// Set up the RFC 1034-compliant label
	m->hostlabel.c[0] = 0;
	GetUserSpecifiedRFC1034ComputerName(&m->hostlabel);
	if (m->hostlabel.c[0] == 0) ConvertCStringToDomainLabel("Macintosh", &m->hostlabel);

	mDNS_GenerateFQDN(m);

	// Tell mDNS core about the network interfaces on this machine.
	err = SetupInterfaceList(m);

	// We don't do asynchronous initialization on the Posix platform, so by the time
	// we get here the setup will already have succeeded or failed.  If it succeeded,
	// we should just call mDNSCoreInitComplete() immediately.
	if (err == 0)
		mDNSCoreInitComplete(m, mStatus_NoError);

	return PosixErrorToStatus(err);
	}

// mDNS core calls this routine to clean up the platform-specific data.
// In our case all we need to do is to tear down every network interface.
mDNSexport void mDNSPlatformClose(mDNS *const m)
	{
	assert(m != NULL);
	ClearInterfaceList(m);
	}

extern mStatus mDNSPlatformPosixRefreshInterfaceList(mDNS *const m)
	{
	int err;
	ClearInterfaceList(m);
	err = SetupInterfaceList(m);
	return PosixErrorToStatus(err);
	}

#pragma mark ***** Locking

// On the Posix platform, locking is a no-op because we only ever enter
// mDNS core on the main thread.

// mDNS core calls this routine when it wants to prevent
// the platform from reentering mDNS core code.
mDNSexport void    mDNSPlatformLock   (const mDNS *const m)
	{
	#pragma unused(m)
	}

// mDNS core calls this routine when it release the lock taken by
// mDNSPlatformLock and allow the platform to reenter mDNS core code.
mDNSexport void    mDNSPlatformUnlock (const mDNS *const m)
	{
	#pragma unused(m)
	}

#pragma mark ***** Strings

// mDNS core calls this routine to copy C strings.
// On the Posix platform this maps directly to the ANSI C strcpy.
mDNSexport void    mDNSPlatformStrCopy(const void *src,       void *dst)
	{
	strcpy((char *)dst, (char *)src);
	}

// mDNS core calls this routine to get the length of a C string.
// On the Posix platform this maps directly to the ANSI C strlen.
mDNSexport mDNSu32  mDNSPlatformStrLen (const void *src)
	{
	return strlen((char*)src);
	}

// mDNS core calls this routine to copy memory.
// On the Posix platform this maps directly to the ANSI C memcpy.
mDNSexport void    mDNSPlatformMemCopy(const void *src,       void *dst, mDNSu32 len)
	{
	memcpy(dst, src, len);
	}

// mDNS core calls this routine to test whether blocks of memory are byte-for-byte
// identical. On the Posix platform this is a simple wrapper around ANSI C memcmp.
mDNSexport mDNSBool mDNSPlatformMemSame(const void *src, const void *dst, mDNSu32 len)
	{
	return memcmp(dst, src, len) == 0;
	}

// mDNS core calls this routine to clear blocks of memory.
// On the Posix platform this is a simple wrapper around ANSI C memset.
mDNSexport void    mDNSPlatformMemZero(                       void *dst, mDNSu32 len)
	{
	memset(dst, 0, len);
	}

mDNSexport mDNSs32  mDNSPlatformOneSecond = 1024;

mDNSexport mDNSs32  mDNSPlatformTimeNow()
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

mDNSexport void mDNSPosixGetFDSet(mDNS *const m, int *nfds, fd_set *readfds, struct timeval *timeout)
	{
	mDNSs32 ticks;
	struct timeval interval;

	// 1. Call mDNS_Execute() to let mDNSCore do what it needs to do
	mDNSs32 nextevent = mDNS_Execute(m);

	// 2. Build our list of active file descriptors
	PosixNetworkInterface *info = (PosixNetworkInterface *)(m->HostInterfaces);
	while (info)
		{
		if (info->multicastSocket != -1)
			{
			if (*nfds < info->multicastSocket + 1)
				*nfds = info->multicastSocket + 1;
			FD_SET(info->multicastSocket, readfds);
			}
		if (info->multicastSocketv6 != -1)
			{
			if (*nfds < info->multicastSocketv6 + 1)
				*nfds = info->multicastSocketv6 + 1;
			FD_SET(info->multicastSocketv6, readfds);
			}
		info = (PosixNetworkInterface *)(info->coreIntf.next);
		}

	// 3. Calculate the time remaining to the next scheduled event (in struct timeval format)
	ticks = nextevent - mDNSPlatformTimeNow();
	if (ticks < 1) ticks = 1;
	interval.tv_sec  = ticks >> 10;						// The high 22 bits are seconds
	interval.tv_usec = ((ticks & 0x3FF) * 15625) / 16;	// The low 10 bits are 1024ths

	// 4. If client's proposed timeout is more than what we want, then reduce it
	if (timeout->tv_sec > interval.tv_sec ||
		(timeout->tv_sec == interval.tv_sec && timeout->tv_usec > interval.tv_usec))
		*timeout = interval;
	}

mDNSexport void mDNSPosixProcessFDSet(mDNS *const m, fd_set *readfds)
	{
	PosixNetworkInterface *info;
	assert(m       != NULL);
	assert(readfds != NULL);
	info = (PosixNetworkInterface *)(m->HostInterfaces);
	while (info)
		{
		if (info->multicastSocket != -1 && FD_ISSET(info->multicastSocket, readfds))
			{
			FD_CLR(info->multicastSocket, readfds);
			SocketDataReady(m, info, info->multicastSocket);
			}
		if (info->multicastSocketv6 != -1 && FD_ISSET(info->multicastSocketv6, readfds))
			{
			FD_CLR(info->multicastSocketv6, readfds);
			SocketDataReady(m, info, info->multicastSocketv6);
			}
		info = (PosixNetworkInterface *)(info->coreIntf.next);
		}
	}
