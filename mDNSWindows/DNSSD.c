/*
 * Copyright (c) 2003-2004 Apple Computer, Inc. All rights reserved.
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

    Change History (most recent first):
    
$Log: DNSSD.c,v $
Revision 1.1  2004/01/30 02:45:21  bradley
High-level implementation of the DNS-SD API. Supports both "direct" (compiled-in mDNSCore) and "client"
(IPC<->service) usage. Conditionals can exclude either "direct" or "client" to reduce code size.

*/

#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>

#include	"CommonServices.h"
#include	"DebugServices.h"

#include	"DNSSDDirect.h"
#include	"RMxClient.h"

#include	"DNSSD.h"

#ifdef	__cplusplus
	extern "C" {
#endif

//===========================================================================================================================
//	Constants
//===========================================================================================================================

#define	DEBUG_NAME		"[DNS-SD] "

//===========================================================================================================================
//	Prototypes
//===========================================================================================================================

DEBUG_LOCAL int DomainEndsInDot( const char *dom );

//===========================================================================================================================
//	Globals
//===========================================================================================================================

#if( DNS_SD_CLIENT_ENABLED )
	const char *			gDNSSDServer			= NULL;
	DEBUG_LOCAL	bool		gDNSSDServerCompatible 	= false;
#endif

#if 0
#pragma mark == General ==
#endif

//===========================================================================================================================
//	DNSServiceInitialize
//===========================================================================================================================

DNSServiceErrorType	DNSServiceInitialize( DNSServiceInitializeFlags inFlags, int inCacheEntryCount )
{
	DNSServiceErrorType		err;
	
	#if( DNS_SD_CLIENT_ENABLED )
		err = RMxClientInitialize();
		if( err == kNoErr )
		{
			// Perform a version check to see if the server is compatible.
			
			if( !( inFlags & kDNSServiceInitializeFlagsNoServerCheck ) )
			{
				err = DNSServiceCheckVersion();
				if( err == kNoErr )
				{
					goto exit;
				}
			}
			else
			{
				// Version check disabled so just assume the server is compatible.
				
				gDNSSDServerCompatible = true;
				goto exit;
			}
		}
	#endif
	
	#if( DNS_SD_DIRECT_ENABLED )
		#if( DNS_SD_CLIENT_ENABLED )
			dlog( kDebugLevelNotice, DEBUG_NAME "server missing or incompatible...falling back to direct implementation\n" );
		#endif
		err = DNSServiceInitialize_direct( inFlags, inCacheEntryCount );
		goto exit;
	#else
		DEBUG_UNUSED( inFlags );
		DEBUG_UNUSED( inCacheEntryCount );
		
		err = kUnsupportedErr;
		goto exit;
	#endif
	
exit:
	return( err );
}

//===========================================================================================================================
//	DNSServiceFinalize
//===========================================================================================================================

void	DNSServiceFinalize( void )
{
	#if( DNS_SD_CLIENT_ENABLED )
		RMxClientFinalize();
	#endif
	
	#if( DNS_SD_DIRECT_ENABLED )
		DNSServiceFinalize_direct();
	#endif
}

//===========================================================================================================================
//	DNSServiceCheckVersion
//===========================================================================================================================

DNSServiceErrorType	DNSServiceCheckVersion( void )
{	
	DNSServiceErrorType		err;
	
	#if( DNS_SD_CLIENT_ENABLED )
		err = DNSServiceCheckVersion_client( gDNSSDServer );
		if( err == kNoErr )
		{
			gDNSSDServerCompatible = true;
		}
	#else
		err = kUnsupportedErr;
	#endif
	
	return( err );
}

#if 0
#pragma mark -
#pragma mark == Properties ==
#endif

//===========================================================================================================================
//	DNSServiceCopyProperty
//===========================================================================================================================

DNSServiceErrorType	DNSServiceCopyProperty( DNSPropertyCode inCode, DNSPropertyData *outData )
{	
	DNSServiceErrorType		err;
	
	#if( DNS_SD_CLIENT_ENABLED )
		if( gDNSSDServerCompatible )
		{
			err = DNSServiceCopyProperty_client( gDNSSDServer, inCode, outData );
			goto exit;
		}
	#else
		DEBUG_UNUSED( inCode );
		DEBUG_UNUSED( outData );
	#endif
	
	#if( DNS_SD_DIRECT_ENABLED )
		err = kUnsupportedErr;
		goto exit;
	#else
		err = kUnsupportedErr;
		goto exit;
	#endif
	
exit:
	return( err );
}

//===========================================================================================================================
//	DNSServiceReleaseProperty
//===========================================================================================================================

DNSServiceErrorType	DNSServiceReleaseProperty( DNSPropertyData *inData )
{	
	DNSServiceErrorType		err;
	
	#if( DNS_SD_CLIENT_ENABLED )
		if( gDNSSDServerCompatible )
		{
			err = DNSServiceReleaseProperty_client( inData );
			goto exit;
		}
	#else
		DEBUG_UNUSED( inData );
	#endif
	
	#if( DNS_SD_DIRECT_ENABLED )
		err = kUnsupportedErr;
		goto exit;
	#else
		err = kUnsupportedErr;
		goto exit;
	#endif
	
exit:
	return( err );
}

#if 0
#pragma mark -
#pragma mark == Unix Domain Socket Access ==
#endif

//===========================================================================================================================
//	DNSServiceRefSockFD
//===========================================================================================================================

int	DNSServiceRefSockFD( DNSServiceRef inRef )
{
	DEBUG_UNUSED( inRef );
	
	dlog( kDebugLevelError, "DNSServiceRefSockFD is not supported\n" );
	return( -1 );
}

//===========================================================================================================================
//	DNSServiceProcessResult
//===========================================================================================================================

DNSServiceErrorType	DNSServiceProcessResult( DNSServiceRef inRef )
{
	DEBUG_UNUSED( inRef );
	
	dlog( kDebugLevelError, "DNSServiceProcessResult is not supported\n" );
	return( kDNSServiceErr_Unsupported );
}

//===========================================================================================================================
//	DNSServiceRefDeallocate
//===========================================================================================================================

void	DNSServiceRefDeallocate( DNSServiceRef inRef )
{
	#if( DNS_SD_CLIENT_ENABLED )
		if( gDNSSDServerCompatible )
		{
			DNSServiceRefDeallocate_client( inRef );
			goto exit;
		}
	#endif
	
	#if( DNS_SD_DIRECT_ENABLED )
		DNSServiceRefDeallocate_direct( inRef );
		goto exit;
	#else
		DEBUG_UNUSED( inRef );

		goto exit;
	#endif
	
exit:
	return;
}

#if 0
#pragma mark -
#pragma mark == Domain Enumeration ==
#endif

//===========================================================================================================================
//	DNSServiceEnumerateDomains
//===========================================================================================================================

DNSServiceErrorType
	DNSServiceEnumerateDomains(
		DNSServiceRef *					outRef,
		const DNSServiceFlags			inFlags,
		const uint32_t					inInterfaceIndex,
		const DNSServiceDomainEnumReply	inCallBack,
		void *							inContext )
{
	DNSServiceErrorType		err;
	
	#if( DNS_SD_CLIENT_ENABLED )
		if( gDNSSDServerCompatible )
		{
			err = DNSServiceEnumerateDomains_client( outRef, gDNSSDServer, inFlags, inInterfaceIndex, inCallBack, inContext );
			goto exit;
		}
	#endif
	
	#if( DNS_SD_DIRECT_ENABLED )
		err = DNSServiceEnumerateDomains_direct( outRef, inFlags, inInterfaceIndex, inCallBack, inContext );
		goto exit;
	#else
		DEBUG_UNUSED( outRef );
		DEBUG_UNUSED( inFlags );
		DEBUG_UNUSED( inInterfaceIndex );
		DEBUG_UNUSED( inCallBack );
		DEBUG_UNUSED( inContext );
		
		err = kUnsupportedErr;
		goto exit;
	#endif
	
exit:
	return( err );
}

#if 0
#pragma mark -
#pragma mark == Service Registration ==
#endif

//===========================================================================================================================
//	DNSServiceRegister
//===========================================================================================================================

DNSServiceErrorType
	DNSServiceRegister(
		DNSServiceRef *			outRef,
		DNSServiceFlags			inFlags,
		uint32_t				inInterfaceIndex,
		const char *			inName,
		const char *			inType,
		const char *			inDomain,
		const char *			inHost,
		uint16_t				inPort,
		uint16_t				inTXTSize,
		const void *			inTXT,
		DNSServiceRegisterReply	inCallBack,
		void *					inContext )
{
	DNSServiceErrorType		err;
	
	#if( DNS_SD_CLIENT_ENABLED )
		if( gDNSSDServerCompatible )
		{
			err = DNSServiceRegister_client( outRef, gDNSSDServer, inFlags, inInterfaceIndex, inName, inType, inDomain, 
				inHost, inPort, inTXTSize, inTXT, inCallBack, inContext );
			goto exit;
		}
	#endif
	
	#if( DNS_SD_DIRECT_ENABLED )
		err = DNSServiceRegister_direct( outRef, inFlags, inInterfaceIndex, inName, inType, inDomain, inHost, inPort, 
			inTXTSize, inTXT, inCallBack, inContext );
		goto exit;
	#else
		DEBUG_UNUSED( outRef );
		DEBUG_UNUSED( inFlags );
		DEBUG_UNUSED( inInterfaceIndex );
		DEBUG_UNUSED( inName );
		DEBUG_UNUSED( inType );
		DEBUG_UNUSED( inDomain );
		DEBUG_UNUSED( inHost );
		DEBUG_UNUSED( inPort );
		DEBUG_UNUSED( inTXTSize );
		DEBUG_UNUSED( inTXT );
		DEBUG_UNUSED( inCallBack );
		DEBUG_UNUSED( inContext );
		
		err = kUnsupportedErr;
		goto exit;
	#endif
	
exit:
	return( err );
}

//===========================================================================================================================
//	DNSServiceAddRecord
//===========================================================================================================================

DNSServiceErrorType
	DNSServiceAddRecord(
		DNSServiceRef 	inRef,
		DNSRecordRef *	outRecordRef,
		DNSServiceFlags	inFlags,
		uint16_t		inRRType,
		uint16_t		inRDataSize,
		const void *	inRData,
		uint32_t		inTTL )
{
	DNSServiceErrorType		err;
	
	#if( DNS_SD_CLIENT_ENABLED )
		if( gDNSSDServerCompatible )
		{
			err = DNSServiceAddRecord_client( inRef, outRecordRef, inFlags, inRRType, inRDataSize, inRData, inTTL );
			goto exit;
		}
	#endif
	
	#if( DNS_SD_DIRECT_ENABLED )
		err = DNSServiceAddRecord_direct( inRef, outRecordRef, inFlags, inRRType, inRDataSize, inRData, inTTL );
		goto exit;
	#else
		DEBUG_UNUSED( inRef );
		DEBUG_UNUSED( outRecordRef );
		DEBUG_UNUSED( inFlags );
		DEBUG_UNUSED( inRRType );
		DEBUG_UNUSED( inRDataSize );
		DEBUG_UNUSED( inRData );
		DEBUG_UNUSED( inTTL );
		
		err = kUnsupportedErr;
		goto exit;
	#endif
	
exit:
	return( err );
}

//===========================================================================================================================
//	DNSServiceUpdateRecord
//===========================================================================================================================

DNSServiceErrorType
	DNSServiceUpdateRecord(
	    DNSServiceRef	inRef,
	    DNSRecordRef	inRecordRef,
	    DNSServiceFlags	inFlags,
	    uint16_t 		inRDataSize,
	    const void *	inRData,
	    uint32_t		inTTL )
{
	DNSServiceErrorType		err;
	
	#if( DNS_SD_CLIENT_ENABLED )
		if( gDNSSDServerCompatible )
		{
			err = DNSServiceUpdateRecord_client( inRef, inRecordRef, inFlags, inRDataSize, inRData, inTTL );
			goto exit;
		}
	#endif
	
	#if( DNS_SD_DIRECT_ENABLED )
		err = DNSServiceUpdateRecord_direct( inRef, inRecordRef, inFlags, inRDataSize, inRData, inTTL );
		goto exit;
	#else
		DEBUG_UNUSED( inRef );
		DEBUG_UNUSED( inRecordRef );
		DEBUG_UNUSED( inFlags );
		DEBUG_UNUSED( inRDataSize );
		DEBUG_UNUSED( inRData );
		DEBUG_UNUSED( inTTL );
		
		err = kUnsupportedErr;
		goto exit;
	#endif
	
exit:
	return( err );
}

//===========================================================================================================================
//	DNSServiceRemoveRecord
//===========================================================================================================================

DNSServiceErrorType DNSServiceRemoveRecord( DNSServiceRef inRef, DNSRecordRef inRecordRef, DNSServiceFlags inFlags )
{
	DNSServiceErrorType		err;
	
	#if( DNS_SD_CLIENT_ENABLED )
		if( gDNSSDServerCompatible )
		{
			err = DNSServiceRemoveRecord_client( inRef, inRecordRef, inFlags );
			goto exit;
		}
	#endif
	
	#if( DNS_SD_DIRECT_ENABLED )
		err = DNSServiceRemoveRecord_direct( inRef, inRecordRef, inFlags );
		goto exit;
	#else
		DEBUG_UNUSED( inRef );
		DEBUG_UNUSED( inRecordRef );
		DEBUG_UNUSED( inFlags );
		
		err = kUnsupportedErr;
		goto exit;
	#endif
	
exit:
	return( err );
}

#if 0
#pragma mark -
#pragma mark == Service Discovery ==
#endif

//===========================================================================================================================
//	DNSServiceBrowse
//===========================================================================================================================

DNSServiceErrorType
	DNSServiceBrowse(
		DNSServiceRef *			outRef,
		DNSServiceFlags			inFlags,
		uint32_t				inInterfaceIndex,
		const char *			inType,   
		const char *			inDomain,
		DNSServiceBrowseReply	inCallBack,
		void *					inContext )
{
	DNSServiceErrorType		err;
	
	#if( DNS_SD_CLIENT_ENABLED )
		if( gDNSSDServerCompatible )
		{
			err = DNSServiceBrowse_client( outRef, gDNSSDServer, inFlags, inInterfaceIndex, inType, inDomain, 
				inCallBack, inContext );
			goto exit;
		}
	#endif
	
	#if( DNS_SD_DIRECT_ENABLED )
		err = DNSServiceBrowse_direct( outRef, inFlags, inInterfaceIndex, inType, inDomain, inCallBack, inContext );
		goto exit;
	#else
		DEBUG_UNUSED( outRef );
		DEBUG_UNUSED( inFlags );
		DEBUG_UNUSED( inInterfaceIndex );
		DEBUG_UNUSED( inType );
		DEBUG_UNUSED( inDomain );
		DEBUG_UNUSED( inCallBack );
		DEBUG_UNUSED( inContext );
		
		err = kUnsupportedErr;
		goto exit;
	#endif
	
exit:
	return( err );
}

//===========================================================================================================================
//	DNSServiceResolve
//===========================================================================================================================

DNSServiceErrorType
	DNSServiceResolve( 
		DNSServiceRef *			outRef,
		DNSServiceFlags			inFlags,
		uint32_t				inInterfaceIndex,
		const char *			inName,     
		const char *			inType,  
		const char *			inDomain,   
		DNSServiceResolveReply	inCallBack,
		void *					inContext )
{
	DNSServiceErrorType		err;
	
	#if( DNS_SD_CLIENT_ENABLED )
		if( gDNSSDServerCompatible )
		{
			err = DNSServiceResolve_client( outRef, gDNSSDServer, inFlags, inInterfaceIndex, inName, inType, inDomain, 
				inCallBack, inContext );
			goto exit;
		}
	#endif
	
	#if( DNS_SD_DIRECT_ENABLED )
		err = DNSServiceResolve_direct( outRef, inFlags, inInterfaceIndex, inName, inType, inDomain, inCallBack, inContext );
		goto exit;
	#else
		DEBUG_UNUSED( outRef );
		DEBUG_UNUSED( inFlags );
		DEBUG_UNUSED( inInterfaceIndex );
		DEBUG_UNUSED( inName );
		DEBUG_UNUSED( inType );
		DEBUG_UNUSED( inDomain );
		DEBUG_UNUSED( inCallBack );
		DEBUG_UNUSED( inContext );
		
		err = kUnsupportedErr;
		goto exit;
	#endif
	
exit:
	return( err );
}

#if 0
#pragma mark -
#pragma mark == Special Purpose ==
#endif

//===========================================================================================================================
//	DNSServiceConstructFullName
//
//	Copied from dnssd_clientstub.c with minimal changes to support NULL/empty name, type, and domain.
//===========================================================================================================================

int	DNSServiceConstructFullName( char *fullName, const char *service, const char *regtype, const char *domain )
{
	size_t len;
	unsigned char c;
	char *fn = fullName;
	const char *s = service;
	const char *r = regtype;
	const char *d = domain;
	
	if (service && *service)
	{
		while(*s)
		{
			c = (unsigned char) *s++;
			if (c == '.' || (c == '\\')) *fn++ = '\\';		// escape dot and backslash literals
			else if (c <= ' ')					// escape non-printable characters
			{
				*fn++ = '\\';
				*fn++ = (char) ('0' + (c / 100));
				*fn++ = (char) ('0' + (c / 10) % 10);
				c = (unsigned char)('0' + (c % 10));
			}
   			*fn++ = (char) c;
		}
		*fn++ = '.';
	}
	
	if (regtype && *regtype)
	{
		len = strlen(regtype);
		if (DomainEndsInDot(regtype)) len--;
		if (len < 4) return -1;					// regtype must end in _udp or _tcp
		if (strncmp((regtype + len - 4), "_tcp", 4) && strncmp((regtype + len - 4), "_udp", 4)) return -1;
		while(*r)
			*fn++ = *r++;
		if (!DomainEndsInDot(regtype)) *fn++ = '.';
	}
	
	if (!domain || !(*domain))
	{
		domain = "local.";
		d = domain;
	}
	len = strlen(domain);
	if (!len) return -1;
	while(*d) 
		*fn++ = *d++;						
	if (!DomainEndsInDot(domain)) *fn++ = '.';
	*fn = '\0';
	return 0;
}

//===========================================================================================================================
//	DNSServiceCreateConnection
//===========================================================================================================================

DNSServiceErrorType	DNSServiceCreateConnection( DNSServiceRef *outRef )
{
	DNSServiceErrorType		err;
	
	#if( DNS_SD_CLIENT_ENABLED )
		if( gDNSSDServerCompatible )
		{
			err = DNSServiceCreateConnection_client( outRef, gDNSSDServer );
			goto exit;
		}
	#endif
	
	#if( DNS_SD_DIRECT_ENABLED )
		err = DNSServiceCreateConnection_direct( outRef );
		goto exit;
	#else
		DEBUG_UNUSED( outRef );

		err = kUnsupportedErr;
		goto exit;
	#endif
	
exit:
	return( err );
}

//===========================================================================================================================
//	DNSServiceRegisterRecord
//===========================================================================================================================

DNSServiceErrorType
	DNSServiceRegisterRecord(
		DNSServiceRef					inRef,
		DNSRecordRef *					outRecordRef,
		DNSServiceFlags					inFlags,
		uint32_t						inInterfaceIndex,
		const char *					inName,   
		uint16_t						inRRType,
		uint16_t						inRRClass,
		uint16_t						inRDataSize,
		const void *					inRData,
		uint32_t						inTTL,
		DNSServiceRegisterRecordReply	inCallBack,
		void *							inContext )
{
	DNSServiceErrorType		err;
	
	#if( DNS_SD_CLIENT_ENABLED )
		if( gDNSSDServerCompatible )
		{
			err = DNSServiceRegisterRecord_client( inRef, outRecordRef, inFlags, inInterfaceIndex, inName, 
				inRRType, inRRClass, inRDataSize, inRData, inTTL, inCallBack, inContext );
			goto exit;
		}
	#endif
	
	#if( DNS_SD_DIRECT_ENABLED )
		err = DNSServiceRegisterRecord_direct( inRef, outRecordRef, inFlags, inInterfaceIndex, inName, 
			inRRType, inRRClass, inRDataSize, inRData, inTTL, inCallBack, inContext );
		goto exit;
	#else
		DEBUG_UNUSED( inRef );
		DEBUG_UNUSED( outRecordRef );
		DEBUG_UNUSED( inFlags );
		DEBUG_UNUSED( inInterfaceIndex );
		DEBUG_UNUSED( inName );
		DEBUG_UNUSED( inRRType );
		DEBUG_UNUSED( inRRClass );
		DEBUG_UNUSED( inRDataSize );
		DEBUG_UNUSED( inRData );
		DEBUG_UNUSED( inTTL );
		DEBUG_UNUSED( inCallBack );
		DEBUG_UNUSED( inContext );
		
		err = kUnsupportedErr;
		goto exit;
	#endif
	
exit:
	return( err );
}

//===========================================================================================================================
//	DNSServiceQueryRecord
//===========================================================================================================================

DNSServiceErrorType
	DNSServiceQueryRecord(
		DNSServiceRef *				outRef,
		DNSServiceFlags				inFlags,
		uint32_t					inInterfaceIndex,
		const char *				inName,     
		uint16_t					inRRType,
		uint16_t					inRRClass,
		DNSServiceQueryRecordReply	inCallBack,
		void *						inContext )
{
	DNSServiceErrorType		err;
	
	#if( DNS_SD_CLIENT_ENABLED )
		if( gDNSSDServerCompatible )
		{
			err = DNSServiceQueryRecord_client( outRef, gDNSSDServer, inFlags, inInterfaceIndex, inName, inRRType, inRRClass, 
				inCallBack, inContext );
			goto exit;
		}
	#endif
	
	#if( DNS_SD_DIRECT_ENABLED )
		err = DNSServiceQueryRecord_direct( outRef, inFlags, inInterfaceIndex, inName, inRRType, inRRClass, 
			inCallBack, inContext );
		goto exit;
	#else
		DEBUG_UNUSED( outRef );
		DEBUG_UNUSED( inFlags );
		DEBUG_UNUSED( inName );
		DEBUG_UNUSED( inInterfaceIndex );
		DEBUG_UNUSED( inName );
		DEBUG_UNUSED( inRRType );
		DEBUG_UNUSED( inRRClass );
		DEBUG_UNUSED( inCallBack );
		DEBUG_UNUSED( inContext );
		
		err = kUnsupportedErr;
		goto exit;
	#endif
	
exit:
	return( err );
}

//===========================================================================================================================
//	DNSServiceReconfirmRecord
//===========================================================================================================================

void
	DNSServiceReconfirmRecord(
		DNSServiceFlags	inFlags,
		uint32_t		inInterfaceIndex,
		const char *	inName,   
		uint16_t		inRRType,
		uint16_t		inRRClass,
		uint16_t		inRDataSize,
		const void *	inRData )
{
	#if( DNS_SD_CLIENT_ENABLED )
		if( gDNSSDServerCompatible )
		{
			DNSServiceReconfirmRecord_client( gDNSSDServer, inFlags, inInterfaceIndex, inName, inRRType, inRRClass, 
				inRDataSize, inRData );
			goto exit;
		}
	#endif
	
	#if( DNS_SD_DIRECT_ENABLED )
		DNSServiceReconfirmRecord_direct( inFlags, inInterfaceIndex, inName, inRRType, inRRClass, inRDataSize, inRData );
		goto exit;
	#else
		DEBUG_UNUSED( inFlags );
		DEBUG_UNUSED( inInterfaceIndex );
		DEBUG_UNUSED( inName );
		DEBUG_UNUSED( inRRType );
		DEBUG_UNUSED( inRRClass );
		DEBUG_UNUSED( inRDataSize );
		DEBUG_UNUSED( inRData );

		goto exit;
	#endif
	
exit:
	return;
}

#if 0
#pragma mark -
#pragma mark == Utilities ==
#endif

//===========================================================================================================================
//	DomainEndsInDot
//
//	Copied from dnssd_clientstub.c.
//===========================================================================================================================

DEBUG_LOCAL int DomainEndsInDot( const char *dom )
{
	check( dom );
	
	while(*dom && *(dom + 1))
	{
		if (*dom == '\\')	// advance past escaped byte sequence
		{		
			if (*(dom + 1) >= '0' && *(dom + 1) <= '9') dom += 4;
			else dom += 2;
		}
		else dom++;		// else read one character
	}
	return (*dom == '.');
}

#ifdef	__cplusplus
	}
#endif
