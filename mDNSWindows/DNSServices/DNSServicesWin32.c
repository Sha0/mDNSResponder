/*
 * Copyright (c) 2002-2003 Apple Computer, Inc. All rights reserved.
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
    
        $Log: DNSServicesWin32.c,v $
        Revision 1.5  2003/07/02 21:20:10  cheshire
        <rdar://problem/3313413> Update copyright notices, etc., in source code comments

        Revision 1.4  2003/03/22 02:57:45  cheshire
        Updated mDNSWindows to use new "mDNS_Execute" model (see "mDNSCore/Implementer Notes.txt")

        Revision 1.3  2002/09/21 20:44:57  zarzycki
        Added APSL info

        Revision 1.2  2002/09/20 05:58:02  bradley
        DNS Services for Windows

*/

#if 0
#pragma mark == Preprocessor ==
#endif

//===========================================================================================================================
//	Preprocessor
//===========================================================================================================================

#if( defined( _MSC_VER ) )
	#pragma warning( disable:4068 )		// Disable "unknown pragma" warning for "pragma unused".
	#pragma warning( disable:4127 )		// Disable "conditional expression is constant" warning for debug macros.
#endif

#define	WIN32_WINDOWS		0x0401		// Needed to use waitable timers.
#define	_WIN32_WINDOWS		0x0401		// Needed to use waitable timers.
#define	WIN32_LEAN_AND_MEAN				// Needed to avoid redefinitions by Windows interfaces.

#include	<stdlib.h>
#include	<string.h>

#include	"DNSServices.h"
#include	"DNSServicesPlatformSupport.h"

#include	"mDNSClientAPI.h"
#include	"mDNSPlatformFunctions.h"
#include	"mDNSWin32.h"

#include	"DNSServicesWin32.h"

//===========================================================================================================================
//	Macros
//===========================================================================================================================

// Emulate Mac OS debugging macros for non-Mac platforms.

#define check(assertion)
#define check_string( assertion, cstring )
#define check_noerr(err)
#define check_noerr_string( error, cstring )
#define debug_string( cstring )
#define require( assertion, label )                             	do { if( !(assertion) ) goto label; } while(0)
#define require_string( assertion, label, string )					require(assertion, label)
#define require_quiet( assertion, label )							require( assertion, label )
#define require_noerr( error, label )								do { if( (error) != 0 ) goto label; } while(0)
#define require_noerr_quiet( assertion, label )						require_noerr( assertion, label )
#define require_noerr_action( error, label, action )				do { if( (error) != 0 ) { {action;}; goto label; } } while(0)
#define require_noerr_action_quiet( assertion, label, action )		require_noerr_action( assertion, label, action )
#define require_action( assertion, label, action )					do { if( !(assertion) ) { {action;}; goto label; } } while(0)
#define require_action_quiet( assertion, label, action )			require_action( assertion, label, action )
#define require_action_string( assertion, label, action, cstring )	do { if( !(assertion) ) { {action;}; goto label; } } while(0)

#if 0
#pragma mark == Prototypes ==
#endif

//===========================================================================================================================
//	Prototypes
//===========================================================================================================================

#if 0
#pragma mark == Globals ==
#endif

//===========================================================================================================================
//	Globals
//===========================================================================================================================

static mDNS						gMDNS					= { 0 };
static mDNS_PlatformSupport		gMDNSPlatformSupport	= { 0 };
static ResourceRecord *			gMDNSCache = NULL;

#if 0
#pragma mark -
#pragma mark == Platform Support ==
#endif

//===========================================================================================================================
//	DNSPlatformInitialize
//===========================================================================================================================

DNSStatus	DNSPlatformInitialize( DNSFlags inFlags, DNSCount inCacheEntryCount, mDNS **outMDNS )
{
	DNSStatus		err;
	mDNSBool		advertise;
	
	memset( &gMDNSPlatformSupport, 0, sizeof( gMDNSPlatformSupport ) );
	
	// Allocate memory for the cache.
	
	err = DNSPlatformMemAlloc( sizeof( ResourceRecord ) * inCacheEntryCount, &gMDNSCache );
	require_noerr( err, exit );
	
	// Initialize mDNS and wait for it to complete.
	
	if( inFlags & kDNSFlagAdvertise )
	{
		advertise = mDNS_Init_AdvertiseLocalAddresses;
	}
	else
	{
		advertise = mDNS_Init_DontAdvertiseLocalAddresses;
	}
	gMDNSPlatformSupport.advertise = advertise;
	err = mDNS_Init( &gMDNS, &gMDNSPlatformSupport, gMDNSCache, inCacheEntryCount, advertise, NULL, NULL );
	require_noerr( err, exit );
	require_noerr_action( gMDNS.mDNSPlatformStatus, exit, err = gMDNS.mDNSPlatformStatus );
	
	*outMDNS = &gMDNS;
	
exit:
	if( err && gMDNSCache )
	{
		DNSPlatformMemFree( gMDNSCache );
		gMDNSCache = NULL;
	}
	return( err );
}

//===========================================================================================================================
//	DNSPlatformFinalize
//===========================================================================================================================

void	DNSPlatformFinalize( void )
{
	mDNS_Close( &gMDNS );
	if( gMDNSCache )
	{
		free( gMDNSCache );
		gMDNSCache = NULL;
	}
}

//===========================================================================================================================
//	DNSPlatformMemAlloc
//===========================================================================================================================

DNSStatus	DNSPlatformMemAlloc( unsigned long inSize, void *outMem )
{
	void *		mem;
	
	check( outMem );
	
	mem = malloc( inSize );
	*( (void **) outMem ) = mem;
	if( mem )
	{
		return( kDNSNoErr );
	}
	return( kDNSNoMemoryErr );
}

//===========================================================================================================================
//	DNSPlatformMemFree
//===========================================================================================================================

void	DNSPlatformMemFree( void *inMem )
{
	check( inMem );
	
	free( inMem );
}

//===========================================================================================================================
//	DNSPlatformLock
//===========================================================================================================================

void	DNSPlatformLock( void )
{
	mDNSPlatformLock( &gMDNS );
}

//===========================================================================================================================
//	DNSPlatformUnlock
//===========================================================================================================================

void	DNSPlatformUnlock( void )
{
	mDNSPlatformUnlock( &gMDNS );
}
