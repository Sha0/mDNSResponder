/*
    File:       mDNSPlatformEnvironment.h

    Contains:   Platform-specific definitions required by the mDNS core.

    Written by: Stuart Cheshire

    Version:    mDNS Core, September 2002

    Copyright:  Copyright (c) 2002 by Apple Computer, Inc., All Rights Reserved.

    Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple Computer, Inc.
                ("Apple") in consideration of your agreement to the following terms, and your
                use, installation, modification or redistribution of this Apple software
                constitutes acceptance of these terms.  If you do not agree with these terms,
                please do not use, install, modify or redistribute this Apple software.

                In consideration of your agreement to abide by the following terms, and subject
                to these terms, Apple grants you a personal, non-exclusive license, under Apple’s
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

$Log: mDNSPlatformEnvironment.h,v $
Revision 1.9  2002/09/16 18:41:42  cheshire
Merge in license terms from Quinn's copy, in preparation for Darwin release

*/


// mDNS-PlatformEnvironment.h needs to ensure that the necessary mDNS types are defined,
// plus whatever additional types are needed to support a particular platform

// To add support for a new target platform with its own networking APIs and types,
// duplicate the "#elif __SOME_OTHER_OS__" section (including its two-line comment
// at the start) and add support for the new target platform in the new section.

#ifndef __mDNSPlatformEnvironment_h
#define __mDNSPlatformEnvironment_h

#ifdef	__cplusplus
	extern "C" {
#endif

// ***************************************************************************
// Classic Mac (Open Transport) structures

#if (TARGET_API_MAC_OS8 || __MACOS__)

// Headers needed for code on this platform
#include <OpenTptInternet.h>
#include <OpenTptClient.h>

typedef enum
	{
	mOT_Reset = 0,
	mOT_Start,
	mOT_ReusePort,
	mOT_RcvDestAddr,
	mOT_LLScope,
	mOT_AdminScope,
	mOT_Bind,
	mOT_Ready
	} mOT_State;

typedef struct { TOptionHeader h; mDNSIPAddr multicastGroupAddress; mDNSIPAddr InterfaceAddress; } TIPAddMulticastOption;
typedef struct { TOptionHeader h; UInt32 flag; } TSetBooleanOption;

// TOptionBlock is a union of various types.
// What they all have in common is that they all start with a TOptionHeader.
typedef union  { TOptionHeader h; TIPAddMulticastOption m; TSetBooleanOption b; } TOptionBlock;

struct mDNS_PlatformSupport_struct
	{
	EndpointRef ep;
	UInt32 mOTstate;				// mOT_State enum
	TOptionBlock optBlock;
	TOptMgmt optReq;
	long OTTimerTask;
	UInt32 nesting;

	// Platforms that support multi-homing will want a list of HostRecordSets instead of just one
	HostRecordSet hostset;
	};

// ***************************************************************************
// Mac OS X structures

#elif (TARGET_API_MAC_OSX || __MACOSX__)

// Headers needed for code on this platform
#include <SystemConfiguration/SystemConfiguration.h>
#include <IOKit/pwr_mgt/IOPMLib.h>
#include <sys/socket.h>
#include <netinet/in.h>

struct mDNS_PlatformSupport_struct
	{
	CFRunLoopTimerRef  CFTimer;
	SCDynamicStoreRef  Store;
	CFRunLoopSourceRef StoreRLS;
	io_connect_t       PowerConnection;
	io_object_t        PowerNotifier;
	CFRunLoopSourceRef PowerRLS;
	};

// Set this symbol to 1 to do extra debug checks on malloc() and free()
#define MACOSX_MDNS_MALLOC_DEBUGGING 0

#if MACOSX_MDNS_MALLOC_DEBUGGING
extern void *mallocL(char *msg, unsigned int size);
extern void freeL(char *msg, void *x);
#else
#define mallocL(X,Y) malloc(Y)
#define freeL(X,Y) free(Y)
#endif

// ***************************************************************************
// Placeholder for future platforms

#elif __SOME_OTHER_OS__

// ***************************************************************************
// Generic code for Unix-style platforms

#else

#error Other platforms need to make sure that types like UInt16 are defined

#endif

#ifdef	__cplusplus
	}
#endif

#endif
