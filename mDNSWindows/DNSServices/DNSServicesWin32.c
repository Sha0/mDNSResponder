/*
	$Id: DNSServicesWin32.c,v 1.1 2002/09/20 02:08:04 cheshire Exp $

	Contains:	DNS Services platform plugin for Win32.
	
	Written by: Bob Bradley
	
    Version:    Rendezvous, September 2002

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
    
        $Log: DNSServicesWin32.c,v $
        Revision 1.1  2002/09/20 02:08:04  cheshire
        Added rendezvous.exe Windows command-line tool

        Revision 1.8  2002/09/18 11:12:48  bradley
        Added Apple license and cleaned up header for public distribution.

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

extern void mDNSPlatformIdle(mDNS *const m);

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
//	DNSPlatformIdle
//===========================================================================================================================

void	DNSPlatformIdle( void )
{
	// No idling needed on Win32.
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
