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

$Log: mDNSPosix.c,v $
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

struct PosixNetworkInterface {
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

static mStatus PosixErrorToStatus(int errNum)
    // For the moment we map all Posix errors to mStatus_UnknownErr.  Ultimately 
    // it would probably be a good idea to map them to their appropriate mStatus 
    // value.
{
    mStatus result;
    
    if (errNum == 0) {
        result = mStatus_NoError;
    } else {
        result = mStatus_UnknownErr;
    }
    return result;
}

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
		}
		break;
#ifdef mDNSIPv6Support
		case AF_INET6:
		{
			struct sockaddr_in6* sin6        = (struct sockaddr_in6*)s_addr;
			assert(sin6->sin6_len == sizeof(*sin6));
			ipAddr->type                     = mDNSAddrType_IPv6;
			ipAddr->addr.ipv6                = *(mDNSv6Addr*)&sin6->sin6_addr;
			if (ipPort) ipPort->NotAnInteger = sin6->sin6_port;
		}
		break;
#endif
		default:
			verbosedebugf("SockAddrTomDNSAddr: Uknown address family %d\n", s_addr->sa_family);
			ipAddr->type = mDNSAddrType_None;
			if (ipPort) ipPort->NotAnInteger = 0;
		break;
	}
}

#pragma mark ***** Send and Receive

mDNSexport mStatus mDNSPlatformSendUDP(const mDNS *const m, const DNSMessage *const msg, const mDNSu8 *const end,
    mDNSOpaqueID InterfaceID, mDNSIPPort srcPort, const mDNSAddr *dst, mDNSIPPort dstPort)
    // mDNS core calls this routine when it needs to send a packet. 
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

	if (dst->type == mDNSAddrType_IPv4) {
		struct sockaddr_in *sin = (struct sockaddr_in*)&to;
		sin->sin_len            = sizeof(*sin);
		sin->sin_family         = AF_INET;
		sin->sin_port           = dstPort.NotAnInteger;
		sin->sin_addr.s_addr    = dst->addr.ipv4.NotAnInteger;
	}
#ifdef mDNSIPv6Support
	else if (dst->type == mDNSAddrType_IPv6) {
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
    if (dst->type == mDNSAddrType_IPv4) {
		err = sendto(thisIntf->multicastSocket, 
					 msg, (char*)end - (char*)msg, 
					 0,                                     // flags
					 (struct sockaddr *)&to, to.ss_len);
	}
#ifdef mDNSIPv6Support
	else if (dst->type == mDNSAddrType_IPv6) {
		err = sendto(thisIntf->multicastSocketv6,
					 msg, (char*)end - (char*)msg,
					 0,
					 (struct sockaddr *)&to, to.ss_len);
	}
#endif

	if (err > 0) {
		err = 0;
	} else if (err < 0) {
		verbosedebugf("mDNSPlatformSendUDP got error %d (%s) sending packet to %#a on interface %#a/%s/%d",
					  errno, strerror(errno), dst, &thisIntf->coreIntf.ip, thisIntf->intfName, thisIntf->index);
	}

    return PosixErrorToStatus(err);
}

static void SocketDataReady(mDNS *const m, PosixNetworkInterface *intf, int skt)
    // This routine is called where the main loop detects that 
    // data is available on a socket.
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
    packetLen = recvfrom_flags(skt, &packet, sizeof(packet), &flags, 
                               (struct sockaddr *) &from, &fromLen, 
                               &packetInfo);

    if (packetLen >= 0) {
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
            if ( (destAddr.NotAnInteger == 0) && (flags & MSG_MCAST) ) {
            	destAddr.type == senderAddr.type;
                if (senderAddr.type == mDNSAddrType_IPv4)
					destAddr.addr.ipv4 = AllDNSLinkGroup;
				else if (senderAddr.type == mDNSAddrType_IPv6)
					destAddr.addr.ipv6 = AllDNSLinkGroupv6;
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
        if ( packetInfo.ipi_ifname[0] != 0 ) {
            reject = (strcmp(packetInfo.ipi_ifname, intf->intfName) != 0);
        } else if ( packetInfo.ipi_ifindex != -1 ) {
            reject = (packetInfo.ipi_ifindex != intf->index);
        }

        if (reject) {
            debugf("SocketDataReady ignored a packet from %#a to %#a on interface %s/%d expecting %#a/%s/%d",
                    &senderAddr, &destAddr, packetInfo.ipi_ifname, packetInfo.ipi_ifindex, &intf->coreIntf.ip, intf->intfName, intf->index);
            packetLen = -1;
        } else {
            verbosedebugf("SocketDataReady got a packet from %#a to %#a on interface %#a/%s/%d",
                &senderAddr, &destAddr, &intf->coreIntf.ip, intf->intfName, intf->index);
        }
    }
    
    if (packetLen >= 0 && packetLen < sizeof(DNSMessageHeader)) {
        debugf("SocketDataReady packet length (%d) too short", packetLen);
        packetLen = -1;
    }

    if (packetLen >= 0) {
        mDNSCoreReceive(m, 
                        &packet, (mDNSu8 *)&packet + packetLen, 
                        &senderAddr, senderPort, 
                        &destAddr, MulticastDNSPort, 
                        intf->coreIntf.InterfaceID);
    }
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

static PosixNetworkInterface *SearchForInterfaceByName(mDNS *const m, const char *intfName)
    // Searches the interface list looking for the named interface. 
    // Returns a pointer to if it found, or NULL otherwise.
{
    PosixNetworkInterface *intf;

    assert(m != NULL);
    assert(intfName != NULL);
    
    intf = (PosixNetworkInterface*)(m->HostInterfaces);
    while ( (intf != NULL) && (strcmp(intf->intfName, intfName) != 0) ) {
        intf = (PosixNetworkInterface *)(intf->coreIntf.next);
    }
    return intf;
}

static void FreePosixNetworkInterface(PosixNetworkInterface *intf)
    // Frees the specified PosixNetworkInterface structure. 
    // The underlying interface must have already been 
    // deregistered with the mDNS core.
{
    int junk;
    
    assert(intf != NULL);
    
    if (intf->intfName != NULL) {
        free( (void *) intf->intfName);
    }
    if (intf->multicastSocket != -1) {
        junk = close(intf->multicastSocket);
        assert(junk == 0);
    }
    if (intf->multicastSocketv6 != -1) {
        junk = close(intf->multicastSocketv6);
        assert(junk == 0);
    }
    free(intf);
}

static void ClearInterfaceList(mDNS *const m)
{
    // Grab the first interface, deregister it, free it, and 
    // repeat until done.

    assert(m != NULL);
    
    while (m->HostInterfaces) {
        PosixNetworkInterface *intf;
        
        intf = (PosixNetworkInterface*)(m->HostInterfaces);
        mDNS_DeregisterInterface(m, &intf->coreIntf);

        if (gMDNSPlatformPosixVerboseLevel > 0) {
            fprintf(stderr, "Deregistered interface %s\n", intf->intfName);
        }

        FreePosixNetworkInterface(intf);
    }
}

static int SetupSocket(struct sockaddr *intfAddr, mDNSIPPort port, int interfaceIndex, int *sktPtr)
    // Sets up a multicast send/receive socket for the specified 
    // port on the interface specified by the IP addrelss intfAddr.
{
    int err;
    int junk;
    static const int kOn = 1;
    static const int kIntTwoFiveFive = 255;
    static const unsigned char kByteTwoFiveFive = 255;
    
    assert(intfAddr != NULL);
    assert(sktPtr != NULL);
    assert(*sktPtr == -1);
    
    // Open the socket...
    
    err = 0;
    if (intfAddr->sa_family == AF_INET) {
		*sktPtr = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (*sktPtr < 0) {
			err = errno;
			perror("socket");
		}
	}
#ifdef mDNSIPv6Support
	else if (intfAddr->sa_family == AF_INET6) {
		*sktPtr = socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);
		if (*sktPtr < 0) {
			err = errno;
			perror("socket");
		}
	}
#endif
	else return EINVAL;
    
    // ... with a shared UDP port
    
    if (err == 0) {
        #if defined(SO_REUSEPORT)
            err = setsockopt(*sktPtr, SOL_SOCKET, SO_REUSEPORT, &kOn, sizeof(kOn));
        #elif defined(SO_REUSEADDR)
            err = setsockopt(*sktPtr, SOL_SOCKET, SO_REUSEADDR, &kOn, sizeof(kOn));
        #else
            #error This platform has no way to avoid address busy errors on multicast.
        #endif
        if (err < 0) { 
            err = errno;
            perror("setsockopt - SO_REUSExxxx");
        }
    }

    // We want to receive destination addresses and interface identifiers.

	if (intfAddr->sa_family == AF_INET) {
		struct ip_mreq imr;
		struct sockaddr_in bindAddr;
		if (err == 0) {
			#if defined(IP_PKTINFO)
	
				// Linux
				
				err = setsockopt(*sktPtr, IPPROTO_IP, IP_PKTINFO, &kOn, sizeof(kOn));
				if (err < 0) {  
					err = errno;
					perror("setsockopt - IP_PKTINFO");
				}
			#elif defined(IP_RECVDSTADDR) || defined(IP_RECVIF)
	
				// BSD and Solaris
				
				#if defined(IP_RECVDSTADDR)
					err = setsockopt(*sktPtr, IPPROTO_IP, IP_RECVDSTADDR, &kOn, sizeof(kOn));
					if (err < 0) { 
						err = errno;
						perror("setsockopt - IP_RECVDSTADDR");
					}
				#endif
			
				#if defined(IP_RECVIF)
					if (err == 0) {
							err = setsockopt(*sktPtr, IPPROTO_IP, IP_RECVIF, &kOn, sizeof(kOn));
							if (err < 0) { 
								err = errno;
								perror("setsockopt - IP_RECVIF");
							}
					}
				#endif
			#else
				#warning This platform has no way to get the destination interface information -- will only work for single-homed hosts
			#endif
		}
		
		// Add multicast group membership on this interface
		
		if (err == 0) {
			imr.imr_multiaddr.s_addr = AllDNSLinkGroup.NotAnInteger;
			imr.imr_interface        = ((struct sockaddr_in*)intfAddr)->sin_addr;
			err = setsockopt(*sktPtr, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imr, sizeof(imr)); 
			if (err < 0) { 
				err = errno;
				perror("setsockopt - IP_ADD_MEMBERSHIP");
			}
		}
	
		// Specify outgoing interface too
		
		if (err == 0) {
			err = setsockopt(*sktPtr, IPPROTO_IP, IP_MULTICAST_IF,
			                 &((struct sockaddr_in*)intfAddr)->sin_addr,
			                 sizeof(struct in_addr));
			if (err < 0) { 
				err = errno;
				perror("setsockopt - IP_MULTICAST_IF");
			}
		}
	
		// Per the mDNS spec, send unicast packets with TTL 255
		
		if (err == 0) {
			err = setsockopt(*sktPtr, IPPROTO_IP, IP_TTL, &kIntTwoFiveFive, sizeof(kIntTwoFiveFive));
			if (err < 0) { 
				err = errno;
				perror("setsockopt - IP_TTL");
			}
		}
	
		// and multicast packets with TTL 255 too
		
		// There's some debate as to whether IP_MULTICAST_TTL is an int or a byte
		// so we just try both.
		
		if (err == 0) {
				err = setsockopt(*sktPtr, IPPROTO_IP, IP_MULTICAST_TTL, 
								 &kByteTwoFiveFive, sizeof(kByteTwoFiveFive));
			if (err < 0 && errno == EINVAL) {
			err = setsockopt(*sktPtr, IPPROTO_IP, IP_MULTICAST_TTL, 
							 &kIntTwoFiveFive, sizeof(kIntTwoFiveFive));
			}
			if (err < 0) { 
				err = errno;
				perror("setsockopt - IP_MULTICAST_TTL");
			}
		}
	
		// And start listening for packets
	
		if (err == 0) {
			bindAddr.sin_family      = AF_INET;
			bindAddr.sin_port        = port.NotAnInteger;
			bindAddr.sin_addr.s_addr = 0; // Want to receive multicasts AND unicasts on this socket
			err = bind(*sktPtr, (struct sockaddr *) &bindAddr, sizeof(bindAddr));
			if (err < 0) {
				err = errno;
				perror("bind");
				fflush(stderr);
			}
		}
	} // AF_INET
#ifdef mDNSIPv6Support
	else if (intfAddr->sa_family == AF_INET6) {
		struct ipv6_mreq imr6;
		struct sockaddr_in6 bindAddr6;
		if (err == 0) {
			#if defined(IPV6_PKTINFO)
				err = setsockopt(*sktPtr, IPPROTO_IPV6, IPV6_PKTINFO, &kOn, sizeof(kOn));
				if (err < 0) {  
					err = errno;
					perror("setsockopt - IPV6_PKTINFO");
				}
			#else
				#warning This platform has no way to get the destination interface information for IPv6 -- will only work for single-homed hosts
			#endif
		}
		
		// Add multicast group membership on this interface
		
		if (err == 0) {
			imr6.ipv6mr_multiaddr       = *(const struct in6_addr*)&AllDNSLinkGroupv6;
			imr6.ipv6mr_interface       = interfaceIndex;
			err = setsockopt(*sktPtr, IPPROTO_IPV6, IPV6_JOIN_GROUP, &imr6, sizeof(imr6)); 
			if (err < 0) {
				err = errno;
				verbosedebugf("IPV6_JOIN_GROUP %.16a on %d failed.\n", &imr6.ipv6mr_multiaddr, imr6.ipv6mr_interface);
				perror("setsockopt - IPV6_JOIN_GROUP");
			}
		}
	
		// Specify outgoing interface too
		
		if (err == 0) {
			u_int	multicast_if = interfaceIndex;
			err = setsockopt(*sktPtr, IPPROTO_IPV6, IPV6_MULTICAST_IF, &multicast_if, sizeof(multicast_if));
			if (err < 0) { 
				err = errno;
				perror("setsockopt - IPV6_MULTICAST_IF");
			}
		}
		
		// We want to receive only IPv6 packets on this socket.
		// Without this option, we may get IPv4 addresses as mapped addresses.
		if (err == 0) {
			err = setsockopt(*sktPtr, IPPROTO_IPV6, IPV6_V6ONLY, &kOn, sizeof(kOn));
			if (err < 0) {
				err = errno;
				perror("setsockopt - IPV6_V6ONLY");
			}
		}
	
		// Per the mDNS spec, send unicast packets with TTL 255
		
		if (err == 0) {
			err = setsockopt(*sktPtr, IPPROTO_IPV6, IPV6_UNICAST_HOPS, &kIntTwoFiveFive, sizeof(kIntTwoFiveFive));
			if (err < 0) { 
				err = errno;
				perror("setsockopt - IPV6_UNICAST_HOPS");
			}
		}
	
		// and multicast packets with TTL 255 too
		
		// There's some debate as to whether IP_MULTICAST_TTL is an int or a byte
		// so we just try both.
		
		if (err == 0) {
			err = setsockopt(*sktPtr, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, 
							 &kByteTwoFiveFive, sizeof(kByteTwoFiveFive));
			if (err < 0 && errno == EINVAL) {
			err = setsockopt(*sktPtr, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, 
							 &kIntTwoFiveFive, sizeof(kIntTwoFiveFive));
			}
			if (err < 0) { 
				err = errno;
				perror("setsockopt - IPV6_MULTICAST_HOPS");
			}
		}
	
		// And start listening for packets
	
		if (err == 0) {
			mDNSPlatformMemZero(&bindAddr6, sizeof(bindAddr6));
			bindAddr6.sin6_len         = sizeof(bindAddr6);
			bindAddr6.sin6_family      = AF_INET6;
			bindAddr6.sin6_port        = port.NotAnInteger;
			bindAddr6.sin6_flowinfo    = 0;
//			bindAddr6.sin6_addr.s_addr = IN6ADDR_ANY_INIT; // Want to receive multicasts AND unicasts on this socket
            bindAddr6.sin6_scope_id    = 0;
			err = bind(*sktPtr, (struct sockaddr *) &bindAddr6, sizeof(bindAddr6));
			if (err < 0) {
				err = errno;
				perror("bind");
				fflush(stderr);
			}
		}
	} // AF_INET6
#endif

    // Set the socket to non-blocking.
    
    if (err == 0) {
        err = fcntl(*sktPtr, F_GETFL, 0);
        if (err < 0) {
            err = errno;
        } else {
            err = fcntl(*sktPtr, F_SETFL, err | O_NONBLOCK);
            if (err < 0) {
                err = errno;
            }
        }
    }

    // Clean up
    
    if (err != 0 && *sktPtr != -1) {
        junk = close(*sktPtr);
        assert(junk == 0);
        *sktPtr = -1;
    }
    
    assert( (err == 0) == (*sktPtr != -1) );
    
    return err;
}

static int SetupOneInterface(mDNS *const m, struct sockaddr *intfAddr, const char *intfName, int index)
    // Creates a PosixNetworkInterface for the interface whose 
    // IP address is intfAddr and whose name is intfName 
    // and registers it with mDNS core.
{
    int err = 0;
    PosixNetworkInterface *intf;
    PosixNetworkInterface *alias = NULL;

    assert(m != NULL);
    assert(intfAddr != NULL);
    assert(intfName != NULL);
    
    // Allocate the interface structure itself.
    
    err = 0;
    intf = malloc(sizeof(*intf));
    if (intf == NULL) {
        assert(0);              // this is bad
        err = ENOMEM;
    }
    
    // And make a copy of the intfName.
    
    if (err == 0) {
        intf->intfName = strdup(intfName);
        if (intf->intfName == NULL) {
            assert(0);          // this is bad
            err = ENOMEM;
        }
    }
    
    if (err == 0) {
        // Set up the fields required by the mDNS core.
        SockAddrTomDNSAddr(intfAddr, &intf->coreIntf.ip, NULL);
		intf->coreIntf.Advertise = m->AdvertiseLocalAddresses;

        // Set up the extra fields in PosixNetworkInterface.
        
        assert(intf->intfName      != NULL);         // intf->intfName already set up above
        intf->index                = if_nametoindex(intf->intfName);
        intf->multicastSocket      = -1;
        intf->multicastSocketv6    = -1;
        alias                      = SearchForInterfaceByName(m, intf->intfName);
        if (alias == NULL) alias   = intf;
        intf->coreIntf.InterfaceID = alias;

        if (alias != intf) {
            debugf("SetupOneInterface: %s %#a is an alias of %#a",
                intfName, &intf->coreIntf.ip, &alias->coreIntf.ip);
        }
    }
    
    // Set up the multicast socket
    if (err == 0) {
        if (alias->multicastSocket == -1 && intfAddr->sa_family == AF_INET) {
            err = SetupSocket(intfAddr, MulticastDNSPort, intf->index, &alias->multicastSocket);
        }
#ifdef mDNSIPv6Support
        else if (alias->multicastSocketv6 == -1 && intfAddr->sa_family == AF_INET6) {
            err = SetupSocket(intfAddr, MulticastDNSPort, intf->index, &alias->multicastSocketv6);
        }
#endif
    }

    // The interface is all ready to go, let's register it with the mDNS core.

    if (err == 0) {
        err = mDNS_RegisterInterface(m, &intf->coreIntf);
    }

    // Clean up.
    
    if (err == 0) { 
        debugf("SetupOneInterface: %s %#a Registered", intf->intfName, &intf->coreIntf.ip);
        if (gMDNSPlatformPosixVerboseLevel > 0) {
            fprintf(stderr, "Registered interface %s\n", intf->intfName);
        }
    } else {
        // Use intfName instead of intf->intfName in the next line to avoid 
        // dereferencing NULL.

        debugf("SetupOneInterface: %s %#a failed to register %d", intfName, &intf->coreIntf.ip, err);

        if (intf != NULL) {
            FreePosixNetworkInterface(intf);
            intf = NULL;
        }
    }
    
    assert( (err == 0) == (intf != NULL) );
    
    return err;
}

static int SetupInterfaceList(mDNS *const m)
{
    int             err;
    struct ifi_info *intfList;
    struct ifi_info *thisIntf;
    struct ifi_info *firstLoopbackIntf;

    assert(m != NULL);
    debugf("SetupInterfaceList");
    err = 0;
    intfList = get_ifi_info(AF_INET, mDNStrue);
    if (intfList == NULL) {
        err = ENOENT;
    }
#ifdef mDNSIPv6Support
    if (err == 0) {
		struct ifi_info *v6intfList;
		
    	/* Get the IPv6 list */
		v6intfList = get_ifi_info(AF_INET6, mDNStrue);
		
		/* Link the IPv6 list to the end of the IPv4 list */
		for (thisIntf = intfList; thisIntf->ifi_next != NULL; thisIntf = thisIntf->ifi_next)
			;
		thisIntf->ifi_next = v6intfList;
	}
#endif
    
    if (err == 0) {
        firstLoopbackIntf = NULL;
        
        thisIntf = intfList;
        while (thisIntf != NULL) {
            if (     ((thisIntf->ifi_addr->sa_family == AF_INET) 
#ifdef mDNSIPv6Support
					  || (thisIntf->ifi_addr->sa_family == AF_INET6)
#endif
                ) &&  (thisIntf->ifi_flags & IFF_UP) ) {
                
                // The Mac OS X code also avoids interfaces with the 
                // IFF_POINTOPOINT flag set.  This prevents nuisance phone 
                // calls when dial-on-demand is enabled.  I specifically didn't 
                // pull in this feature because most UNIX hosts don't use 
                // PPP dial-on-demand.  If you nee this you should add 
                // the conditional:
                //
                //     && !(thisIntf->ifi_flags & IFF_POINTOPOINT)
                //
                // to the above "if" statement.

                if (thisIntf->ifi_flags & IFF_LOOPBACK) {
                    if (firstLoopbackIntf == NULL) {
                        firstLoopbackIntf = thisIntf;
                    }
                } else {
                    // We ignore any errors from SetupOneInterface because we want the 
                    // program to continue to run on any other interfaces and 
                    // SetupOneInterface has already printed an appropriate diagnostic 
                    // message.
                    
                    (void) SetupOneInterface(m,
                                             thisIntf->ifi_addr,
                                             thisIntf->ifi_name,
                                             thisIntf->ifi_index);
                }
            }
            thisIntf = thisIntf->ifi_next;
        }
        
        // If we found no normal interfaces but we did find a loopback 
        // interface, register the loopback interface.  This allows self-
        // discovery if no interfaces are configured.  Note that this 
        // still doesn't work on Mac OS X 10.2 and earlier, for reasons 
        // that are still being investigated [Radar ID 3016042].
        
        if ( (m->HostInterfaces == NULL) && (firstLoopbackIntf != NULL) ) {
            (void) SetupOneInterface(m, 
                                     firstLoopbackIntf->ifi_addr, 
                                     firstLoopbackIntf->ifi_name, 
                                     firstLoopbackIntf->ifi_index);
        }
    }
    
    // Clean up.
    
    if (intfList != NULL) {
        free_ifi_info(intfList);
    }
    
    return err;
}

mDNSexport mStatus mDNSPlatformInit(mDNS *const m)
    // mDNS core calls this routine to initialise the platform-
    // specific data.
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

mDNSexport void mDNSPlatformClose(mDNS *const m)
    // mDNS core calls this routine to clean up the platform-specific 
    // data.  In our case all we need to do is to tear down every 
    // network interface.
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

mDNSexport void    mDNSPlatformLock   (const mDNS *const m) 
    // mDNS core calls this routine when it wants to prevent 
    // the platform from reentering mDNS core code.
{
    #pragma unused(m)
}

mDNSexport void    mDNSPlatformUnlock (const mDNS *const m) 
    // mDNS core calls this routine when it release the lock 
    // taken by mDNSPlatformLock and allow the platform to 
    // reenter mDNS core code.
{ 
    #pragma unused(m)
}

#pragma mark ***** Strings

mDNSexport void    mDNSPlatformStrCopy(const void *src,       void *dst)
    // mDNS core calls this routine to copy C strings. 
    // On the Posix platform this maps directly to the 
    // ANSI C strcpy.
{ 
    strcpy((char *)dst, (char *)src);
}

mDNSexport mDNSu32  mDNSPlatformStrLen (const void *src)
    // mDNS core calls this routine to get the length of a C string. 
    // On the Posix platform this maps directly to the ANSI C strlen.
{ 
    return strlen((char*)src);
}

mDNSexport void    mDNSPlatformMemCopy(const void *src,       void *dst, mDNSu32 len) 
    // mDNS core calls this routine to copy memory.
    // On the Posix platform this maps directly to the 
    // ANSI C memcpy.
{ 
    memcpy(dst, src, len); 
}

mDNSexport mDNSBool mDNSPlatformMemSame(const void *src, const void *dst, mDNSu32 len) 
    // mDNS core calls this routine to test whether blocks 
    // of memory are byte-for-byte identical.
    // On the Posix platform this is a simple wrapper around 
    // ANSI C memcmp.
{ 
    return memcmp(dst, src, len) == 0; 
}

mDNSexport void    mDNSPlatformMemZero(                       void *dst, mDNSu32 len) 
    // mDNS core calls this routine to clear blocks of memory.
    // On the Posix platform this is a simple wrapper around 
    // ANSI C memset.
{ 
    memset(dst, 0, len); 
}

mDNSexport mDNSs32  mDNSPlatformOneSecond = 1024;

#define ConvertTV(X) ( ((X).tv_sec << 10) | ((X).tv_usec * 16 / 15625) )

mDNSexport mDNSs32  mDNSPlatformTimeNow()
	{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	// tp.tv_sec is seconds since 1st January 1970 (GMT, with no adjustment for daylight savings time)
	// tp.tv_usec is microseconds since the start of this second (i.e. values 0 to 999999)
	// We use the lower 22 bits of tp.tv_sec for the top 22 bits of our result
	// and we multiply tp.tv_usec by 16 / 15625 to get a value in the range 0-1023 to go in the bottom 10 bits.
	// This gives us a proper modular (cyclic) counter that has a resolution of roughly 1ms (actually 1/1024 second)
	// and correctly cycles every 2^22 seconds (4194304 seconds = approx 48 days).
	return(ConvertTV(tv));
	}

mDNSexport void     mDNSPlatformScheduleTask(const mDNS *const m, mDNSs32 nextTaskTime)
    // mDNS core calls this routine to tell the platform when it should next 
    // give time to the core by calling mDNSCoreTask.  nextTaskTime is the 
    // time at which the platform should call mDNSCoreTask.  This time 
    // supercedes any previous times set by this routine.  The time is 
    // in platform time units; there are mDNSPlatformOneSecond platform 
    // time units per second.  The time is an absolute time, derived by 
    // adding N platform time units to the value returned by 
    // mDNSPlatformTimeNow.  Note that the time might be in the past, 
    // in which case the platform should call mDNSCoreTask as soon as 
    // possible.  Also note that the quantity is a signed 32 bit number 
    // and thus it's quite possible that it'll wrap during the life time 
    // of the platform.  mDNS core handles this perfectly, and so should 
    // your platform.
	{
	mDNSs32 delta;
    assert(m != NULL);
    gettimeofday(&m->p->NextEvent, NULL);					// Get time now
    delta = nextTaskTime - ConvertTV(m->p->NextEvent);		// Work out how many ticks from now to nextTaskTime
    if (delta < 0) delta = 0;
    
    m->p->NextEvent.tv_sec += (delta >> 10);				// Convert that to a struct timeval
    m->p->NextEvent.tv_usec += ((delta & 0x3FF) * 15625) / 16;
    if (m->p->NextEvent.tv_usec >= 1000000)					// Correct if we incremented tv_usec above one million
    	{
    	m->p->NextEvent.tv_usec -= 1000000;
    	m->p->NextEvent.tv_sec  += 1;
    	}
	}

mDNSexport void mDNSPosixGetFDSet(const mDNS *const m, int *nfds, fd_set *readfds, struct timeval *timeout)
	{
	PosixNetworkInterface *info = (PosixNetworkInterface *)(m->HostInterfaces);
	struct timeval n = m->p->NextEvent, tp;
	gettimeofday(&tp, NULL);								// Get time now
	
	// If we're already past NextEvent, then interval is zero
	if (tp.tv_sec > n.tv_sec || ((tp.tv_sec == n.tv_sec && tp.tv_usec > n.tv_usec)))
		tp.tv_sec = tp.tv_usec = 0;
	else	// else, interval is NextEvent minus timenow
		{
		if (n.tv_usec < tp.tv_usec)
			{
			n.tv_usec += 1000000;
			n.tv_sec  -= 1;
			}
		tp.tv_sec  = n.tv_sec  - tp.tv_sec;
		tp.tv_usec = n.tv_usec - tp.tv_usec;
		}

	// If client's proposed timeout is more than what we want, then reduce it
	if (timeout->tv_sec > tp.tv_sec ||
		(timeout->tv_sec == tp.tv_sec && timeout->tv_usec > tp.tv_usec))
		*timeout = tp;

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
	}

mDNSexport void mDNSPosixProcessFDSet(mDNS *const m, int selectresult, fd_set *readfds)
	{
	assert(m       != NULL);
	assert(readfds != NULL);

	if (selectresult == 0)
		{
		debugf("Timeout");
		mDNSCoreTask(m);
		}
	else
		{
		PosixNetworkInterface *info = (PosixNetworkInterface *)(m->HostInterfaces);
		debugf("Got a packet");
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
	}
