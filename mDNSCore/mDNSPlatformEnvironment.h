// mDNS-PlatformEnvironment.h needs to ensure that the necessary mDNS types are defined,
// plus whatever additional types are needed to support a particular platform

// To add support for a new target platform with its own networking APIs and types,
// duplicate the "#elif __SOME_OTHER_OS__" section (including its two-line comment
// at the start) and add support for the new target platform in the new section.

#pragma once

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
#include <sys/socket.h>
#include <netinet/in.h>

struct mDNS_PlatformSupport_struct
	{
	CFRunLoopTimerRef cftimer;
	SCDynamicStoreRef store;
	};

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
