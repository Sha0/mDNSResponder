/*
 * Copyright (c) 2002-2003 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@

 	File:		uds_daemon.h

 	Contains:	Interfaces necessary to talk to uds_daemon.c.

 	Version:	1.0

    Change History (most recent first):

*/

#include "mDNSClientAPI.h"


/* Client interface: */

#define PORT_AS_NUM(P) (((mDNSu16)(P).b[0]) << 8 | (P).b[1])
#define SRS_PORT_AS_NUM(S) PORT_AS_NUM((S)->RR_SRV.resrec.rdata->u.srv.port)

extern int udsserver_init( mDNS *globalInstance);

// takes the next scheduled event time, does idle work, and returns the updated nextevent time
extern mDNSs32 udsserver_idle(mDNSs32 nextevent);

extern void udsserver_info(void);	// print out info about current state

extern void udsserver_handle_configchange(void);

extern int udsserver_exit(void);	// should be called prior to app exit


/* Routines that uds_daemon expects to link against: */

typedef	void (*udsEventCallback)(void *context);

extern mStatus udsSupportAddFDToEventLoop( int fd, udsEventCallback callback, void *context);
extern mStatus udsSupportRemoveFDFromEventLoop( int fd);

extern mDNSInterfaceID mDNSPlatformInterfaceIDfromInterfaceIndex(const mDNS *const m, mDNSu32 index);
extern mDNSu32 mDNSPlatformInterfaceIndexfromInterfaceID(const mDNS *const m, mDNSInterfaceID id);

