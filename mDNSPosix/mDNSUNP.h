/*
    File:       mDNSUNP.h

    Contains:   Interface to code derived from "UNIX Network Programming".

    Written by: Quinn

    Copyright:  Copyright (c) 2002 by Apple Computer, Inc., All Rights Reserved.

    Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple Computer, Inc.
                ("Apple") in consideration of your agreement to the following terms, and your
                use, installation, modification or redistribution of this Apple software
                constitutes acceptance of these terms.  If you do not agree with these terms,
                please do not use, install, modify or redistribute this Apple software.

                In consideration of your agreement to abide by the following terms, and subject
                to these terms, Apple grants you a personal, non-exclusive license, under Apple's
                copyrights in this original Apple software (the "Apple Software"), to use,
                reproduce, modify and redistribute the Apple Software, with or without
                modifications, in source and/or binary forms; provided that if you redistribute
                the Apple Software in its entirety and without modifications, you must retain
                this notice and the following text and disclaimers in all such redistributions of
                the Apple Software.  Neither the name, trademarks, service marks or logos of
                Apple Computer, Inc. may be used to endorse or promote products derived from the
                Apple Software without specific prior written permission from Apple.  Except as
                expressly stated in this notice, no other rights or licenses, express or implied,
                are granted by Apple herein, including but not limited to any patent rights that
                may be infringed by your derivative works or by other works in which the Apple
                Software may be incorporated.

                The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
                WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
                WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
                PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN
                COMBINATION WITH YOUR PRODUCTS.

                IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
                CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
                GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
                ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION AND/OR DISTRIBUTION
                OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT, TORT
                (INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN
                ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    Change History (most recent first):

$Log: mDNSUNP.h,v $
Revision 1.2  2002/09/19 04:20:44  cheshire
Remove high-ascii characters that confuse some systems

Revision 1.1  2002/09/17 06:24:35  cheshire
First checkin

*/

#ifndef __mDNSUNP_h
#define __mDNSUNP_h

#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>

#ifdef  __cplusplus
    extern "C" {
#endif

#if !defined(HAVE_SOCKLEN_T)
    typedef unsigned int socklen_t;
#endif

#define IFI_NAME    16          /* same as IFNAMSIZ in <net/if.h> */
#define IFI_HADDR    8          /* allow for 64-bit EUI-64 in future */

// Renamed from my_in_pktinfo because in_pktinfo is used by Linux.

struct my_in_pktinfo {
    struct in_addr  ipi_addr;               /* dst IPv4 address */
    int             ipi_ifindex;            /* received interface index */
    char            ipi_ifname[IFI_NAME];   /* received interface name  */
};

extern ssize_t recvfrom_flags(int fd, void *ptr, size_t nbytes, int *flagsp,
               struct sockaddr *sa, socklen_t *salenptr, struct my_in_pktinfo *pktp);

struct ifi_info {
  char    ifi_name[IFI_NAME];   /* interface name, null terminated */
  u_char  ifi_haddr[IFI_HADDR]; /* hardware address */
  u_short ifi_hlen;             /* #bytes in hardware address: 0, 6, 8 */
  short   ifi_flags;            /* IFF_xxx constants from <net/if.h> */
  short   ifi_myflags;          /* our own IFI_xxx flags */
  int     ifi_index;            /* interface index */
  struct sockaddr  *ifi_addr;   /* primary address */
  struct sockaddr  *ifi_brdaddr;/* broadcast address */
  struct sockaddr  *ifi_dstaddr;/* destination address */
  struct ifi_info  *ifi_next;   /* next of these structures */
};

#define IFI_ALIAS   1           /* ifi_addr is an alias */

extern struct ifi_info  *get_ifi_info(int family, int doaliases);
extern void             free_ifi_info(struct ifi_info *);

#ifdef  __cplusplus
    }
#endif

#endif
