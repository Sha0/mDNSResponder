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

    Change History (most recent first):
    
$Log: Tool.c,v $
Revision 1.5  2003/08/12 19:56:28  cheshire
Update to APSL 2.0

Revision 1.4  2003/07/02 21:20:09  cheshire
<rdar://problem/3313413> Update copyright notices, etc., in source code comments

Revision 1.3  2003/03/22 02:57:45  cheshire
Updated mDNSWindows to use new "mDNS_Execute" model (see "mDNSCore/Implementer Notes.txt")

Revision 1.2  2002/09/21 20:44:56  zarzycki
Added APSL info

Revision 1.1  2002/09/20 06:07:55  bradley
Rendezvous Test Tool for Windows

*/

#include	<stdint.h>
#include	<stdio.h>

#define	WIN32_LEAN_AND_MEAN

#include	<windows.h>

#include	"DNSServices.h"

//===========================================================================================================================
//	Macros
//===========================================================================================================================

#define	require_action_string( X, LABEL, ACTION, STR )				\
	do 																\
	{																\
		if( !( X ) ) 												\
		{															\
			fprintf( stderr, "%s\n", ( STR ) );						\
			{ ACTION; }												\
			goto LABEL;												\
		}															\
	} while( 0 )

#define	require_string( X, LABEL, STR )								\
	do 																\
	{																\
		if( !( X ) ) 												\
		{															\
			fprintf( stderr, "%s\n", ( STR ) );						\
			goto LABEL;												\
																	\
		}															\
	} while( 0 )

#define	require_noerr_string( ERR, LABEL, STR )						\
	do 																\
	{																\
		if( ( ERR ) != 0 ) 											\
		{															\
			fprintf( stderr, "%s (%ld)\n", ( STR ), ( ERR ) );		\
			goto LABEL;												\
		}															\
	} while( 0 )

//===========================================================================================================================
//	Prototypes
//===========================================================================================================================

int 				main( int argc, char* argv[] );
static void			Usage( void );
static BOOL WINAPI	ConsoleControlHandler( DWORD inControlEvent );
static void 		BrowserCallBack( void *inContext, DNSBrowserRef inRef, DNSStatus inStatusCode, const DNSBrowserEvent *inEvent );
static void 		ResolverCallBack( void *inContext, DNSResolverRef inRef, DNSStatus inStatusCode, const DNSResolverEvent *inEvent );
static void
	RegistrationCallBack( 
		void *							inContext, 
		DNSRegistrationRef				inRef, 
		DNSStatus						inStatusCode, 
		const DNSRegistrationEvent *	inEvent );
static char *	IPv4ToString( DNSUInt32 inIP, char *outString );

//===========================================================================================================================
//	Globals
//===========================================================================================================================

static volatile int		gQuit = 0;

//===========================================================================================================================
//	main
//===========================================================================================================================

int main( int argc, char* argv[] )
{	
	DNSStatus						err;
	int								i;
	const char *					name;
	const char *					type;
	const char *					domain;
	int								port;
	const char *					text;
	DNSBrowserRef					browser;
	DNSResolverFlags				resolverFlags;
	DNSDomainRegistrationType		domainType;
	const char *					label;
	
	// Set up DNS Services and install a Console Control Handler to handle things like control-c signals.
	
	err = DNSServicesInitialize( kDNSFlagAdvertise, 0 );
	require_noerr_string( err, exit, "could not initialize Rendezvous" );
	
	SetConsoleCtrlHandler( ConsoleControlHandler, TRUE );
	
	// Parse the command line arguments (ignore first argument since it's just the program name).
	
	require_action_string( argc >= 2, exit, err = kDNSBadParamErr, "no arguments specified" );
	
	for( i = 1; i < argc; ++i )
	{
		if( strcmp( argv[ i ], "-bbd" ) == 0 )
		{
			// 'b'rowse for 'b'rowsing 'd'omains
			
			fprintf( stdout, "browsing for browsing domains\n" );
			
			err = DNSBrowserCreate( 0, BrowserCallBack, NULL, &browser );
			require_noerr_string( err, exit, "create browser failed" );
			
			err = DNSBrowserStartDomainSearch( browser, 0 );
			require_noerr_string( err, exit, "start domain search failed" );
		}
		else if( strcmp( argv[ i ], "-brd" ) == 0 )
		{
			// 'b'rowse for 'r'egistration 'd'omains
			
			fprintf( stdout, "browsing for registration domains\n" );
			
			err = DNSBrowserCreate( 0, BrowserCallBack, NULL, &browser );
			require_noerr_string( err, exit, "create browser failed" );
			
			err = DNSBrowserStartDomainSearch( browser, kDNSBrowserFlagRegistrationDomainsOnly );
			require_noerr_string( err, exit, "start domain search failed" );
		}
		else if( strcmp( argv[ i ], "-bs" ) == 0 )
		{
			// 'b'rowse for 's'ervices <type> <domain>
						
			require_action_string( argc > ( i + 2 ), exit, err = kDNSBadParamErr, "missing arguments" );
			++i;
			type 	= argv[ i++ ];
			domain 	= argv[ i ];
			if( ( domain[ 0 ] == '.' ) && ( domain[ 1 ] == '\0' ) )
			{
				domain = "local.";
			}
			fprintf( stdout, "browsing for \"%s.%s\"\n", type, domain );
			
			err = DNSBrowserCreate( 0, BrowserCallBack, NULL, &browser );
			require_noerr_string( err, exit, "create browser failed" );
			
			err = DNSBrowserStartServiceSearch( browser, kDNSBrowserFlagAutoResolve, type, domain );
			require_noerr_string( err, exit, "start service search failed" );
		}
		else if( strcmp( argv[ i ], "-lsi" ) == 0 )
		{
			// 'l'ookup 's'ervice 'i'nstance <name> <type> <domain>
			
			require_action_string( argc > ( i + 3 ), exit, err = kDNSBadParamErr, "missing arguments" );
			++i;
			name 	= argv[ i++ ];
			type 	= argv[ i++ ];
			domain 	= argv[ i ];
			if( ( domain[ 0 ] == '.' ) && ( domain[ 1 ] == '\0' ) )
			{
				domain = "local.";
			}
			fprintf( stdout, "resolving \"%s.%s.%s\"\n", name, type, domain );
			
			resolverFlags = kDNSResolverFlagOnlyIfUnique | 
							kDNSResolverFlagAutoReleaseByName;
			err = DNSResolverCreate( resolverFlags, name, type, domain, ResolverCallBack, 0, NULL, NULL );
			require_noerr_string( err, exit, "create resolver failed" );
		}
		else if( ( strcmp( argv[ i ], "-rdb" ) == 0 ) || ( strcmp( argv[ i ], "-rdbd" ) == 0 ) )
		{
			// 'r'egister 'd'omain for 'b'rowsing ['d'efault] <domain>
						
			require_action_string( argc > ( i + 1 ), exit, err = kDNSBadParamErr, "missing arguments" );
			if( strcmp( argv[ i ], "-rdb" ) == 0 )
			{
				domainType = kDNSDomainRegistrationTypeBrowse;
				label = "";
			}
			else
			{
				domainType = kDNSDomainRegistrationTypeBrowseDefault;
				label = "default ";
			}
			++i;
			domain = argv[ i ];
			if( ( domain[ 0 ] == '.' ) && ( domain[ 1 ] == '\0' ) )
			{
				domain = "local.";
			}
			fprintf( stdout, "registering \"%s\" as %sbrowse domain\n", domain, label );
			
			err = DNSDomainRegistrationCreate( 0, domain, domainType, NULL );
			require_noerr_string( err, exit, "create domain registration failed" );
		}
		else if( ( strcmp( argv[ i ], "-rdr" ) == 0 ) || ( strcmp( argv[ i ], "-rdrd" ) == 0 ) )
		{
			// 'r'egister 'd'omain for 'r'egistration ['d'efault] <domain>
			
			require_action_string( argc > ( i + 1 ), exit, err = kDNSBadParamErr, "missing arguments" );
			if( strcmp( argv[ i ], "-rdr" ) == 0 )
			{
				domainType = kDNSDomainRegistrationTypeRegistration;
				label = "";
			}
			else
			{
				domainType = kDNSDomainRegistrationTypeRegistrationDefault;
				label = "default ";
			}
			++i;
			domain = argv[ i ];
			if( ( domain[ 0 ] == '.' ) && ( domain[ 1 ] == '\0' ) )
			{
				domain = "local.";
			}
			fprintf( stdout, "registering \"%s\" as %sregistration domain\n", domain, label );
			
			err = DNSDomainRegistrationCreate( 0, domain, domainType, NULL );
			require_noerr_string( err, exit, "create domain registration failed" );
		}
		else if( strcmp( argv[ i ], "-rs" ) == 0 )
		{
			// 'r'egister 's'ervice <name> <type> <domain> <port> <txt>
						
			require_action_string( argc > ( i + 5 ), exit, err = kDNSBadParamErr, "missing arguments" );
			++i;
			name 	= argv[ i++ ];
			type 	= argv[ i++ ];
			domain 	= argv[ i++ ];
			port 	= atoi( argv[ i++ ] );
			text 	= argv[ i ];
			if( ( domain[ 0 ] == '.' ) && ( domain[ 1 ] == '\0' ) )
			{
				domain = "local.";
			}
			fprintf( stdout, "registering service \"%s.%s.%s\" port %d text \"%s\"\n", name, type, domain, port, text );
			
			err = DNSRegistrationCreate( 0, name, type, domain, port, text, RegistrationCallBack, NULL, NULL );
			require_noerr_string( err, exit, "create registration failed" );
		}
		else if( ( strcmp( argv[ i ], "-help" ) == 0 ) || ( strcmp( argv[ i ], "-h" ) == 0 ) )
		{
			// Help
			
			Usage();
			goto exit;
		}
		else
		{
			// Unknown parameter.
			
			require_action_string( 0, exit, err = kDNSBadParamErr, "unknown parameter" );
			goto exit;
		}
	}
	
	// Sleep until control-c'd.
	
	while( !gQuit )
	{
		Sleep( 200 );
	}
	
exit:
	if( err )
	{
		Usage();
	}
	DNSServicesFinalize();
	return( err );
}

//===========================================================================================================================
//	Usage
//===========================================================================================================================

static void	Usage( void )
{
	fprintf( stderr, "\n" );
	fprintf( stderr, "rendezvous - Rendezvous Tool for Windows 1.0d1\n" );
	fprintf( stderr, "\n" );
	fprintf( stderr, "  -bbd                                       'b'rowse for 'b'rowsing 'd'omains\n" );
	fprintf( stderr, "  -brd                                       'b'rowse for 'r'egistration 'd'omains\n" );
	fprintf( stderr, "  -bs <type> <domain>                        'b'rowse for 's'ervices\n" );
	fprintf( stderr, "  -lsi <name> <type> <domain>                'l'ookup 's'ervice 'i'nstance\n" );
	fprintf( stderr, "  -rdb[d] <domain>                           'r'egister 'd'omain for 'b'rowsing ['d'efault]\n" );
	fprintf( stderr, "  -rdr[d] <domain>                           'r'egister 'd'omain for 'r'egistration ['d'efault]\n" );
	fprintf( stderr, "  -rs <name> <type> <domain> <port> <txt>    'r'egister 's'ervice\n" );
	fprintf( stderr, "  -h[elp]                                    'h'elp\n" );
	fprintf( stderr, "\n" );
	fprintf( stderr, "Examples:\n" );
	fprintf( stderr, "\n" );
	fprintf( stderr, "  rendezvous -bbd\n" );
	fprintf( stderr, "  rendezvous -bs \"_airport._tcp\" \"local.\"\n" );
	fprintf( stderr, "  rendezvous -lsi \"My Base Station\" \"_airport._tcp\" \"local.\"\n" );
	fprintf( stderr, "  rendezvous -rdb \"apple.com\"\n" );
	fprintf( stderr, "  rendezvous -rs \"My Computer\" \"_airport._tcp\" \"local.\" 1234 \"My Info\"\n" );
	fprintf( stderr, "\n" );
}

//===========================================================================================================================
//	ConsoleControlHandler
//===========================================================================================================================

static BOOL WINAPI	ConsoleControlHandler( DWORD inControlEvent )
{
	BOOL		handled;
	
	handled = 0;
	switch( inControlEvent )
	{
		case CTRL_C_EVENT:
		case CTRL_BREAK_EVENT:
		case CTRL_CLOSE_EVENT:
		case CTRL_LOGOFF_EVENT:
		case CTRL_SHUTDOWN_EVENT:
			gQuit = 1;
			handled = 1;
			break;
		
		default:
			break;
	}
	return( handled );
}

//===========================================================================================================================
//	BrowserCallBack
//===========================================================================================================================

static void BrowserCallBack( void *inContext, DNSBrowserRef inRef, DNSStatus inStatusCode, const DNSBrowserEvent *inEvent )
{
	#pragma unused( inContext, inRef, inStatusCode )
	
	char		ifIP[ 32 ];
	char		ip[ 32 ];
	
	switch( inEvent->type )
	{
		case kDNSBrowserEventTypeRelease:
			break;
			
		case kDNSBrowserEventTypeAddDomain:			
			fprintf( stdout, "domain \"%s\" added on interface %s\n", 
					 inEvent->data.addDomain.domain, 
					 IPv4ToString( inEvent->data.addDomain.interfaceAddr.u.ipv4.address, ifIP ) );
			break;
		
		case kDNSBrowserEventTypeAddDefaultDomain:
			fprintf( stdout, "default domain \"%s\" added on interface %s\n", 
					 inEvent->data.addDefaultDomain.domain, 
					 IPv4ToString( inEvent->data.addDefaultDomain.interfaceAddr.u.ipv4.address, ifIP ) );
			break;
		
		case kDNSBrowserEventTypeRemoveDomain:
			fprintf( stdout, "domain \"%s\" removed on interface %s\n", 
					 inEvent->data.removeDomain.domain, 
					 IPv4ToString( inEvent->data.removeDomain.interfaceAddr.u.ipv4.address, ifIP ) );
			break;
		
		case kDNSBrowserEventTypeAddService:
			fprintf( stdout, "service \"%s.%s%s\" added on interface %s\n", 
					 inEvent->data.addService.name, 
					 inEvent->data.addService.type, 
					 inEvent->data.addService.domain, 
					 IPv4ToString( inEvent->data.addService.interfaceAddr.u.ipv4.address, ifIP ) );
			break;
		
		case kDNSBrowserEventTypeRemoveService:
			fprintf( stdout, "service \"%s.%s%s\" removed on interface %s\n", 
					 inEvent->data.removeService.name, 
					 inEvent->data.removeService.type, 
					 inEvent->data.removeService.domain, 
					 IPv4ToString( inEvent->data.removeService.interfaceAddr.u.ipv4.address, ifIP ) );
			break;
		
		case kDNSBrowserEventTypeResolved:
			fprintf( stdout, "resolved \"%s.%s%s\" to %s:%u on interface %s with text \"%s\"\n", 
					 inEvent->data.resolved->name, 
					 inEvent->data.resolved->type, 
					 inEvent->data.resolved->domain, 
					 IPv4ToString( inEvent->data.resolved->address.u.ipv4.address, ip ), 
					 inEvent->data.resolved->address.u.ipv4.port, 
					 IPv4ToString( inEvent->data.resolved->interfaceAddr.u.ipv4.address, ifIP ), 
					 inEvent->data.resolved->textRecord );
			break;
		
		default:
			break;
	}
}

//===========================================================================================================================
//	ResolverCallBack
//===========================================================================================================================

static void ResolverCallBack( void *inContext, DNSResolverRef inRef, DNSStatus inStatusCode, const DNSResolverEvent *inEvent )
{
	#pragma unused( inContext, inRef, inStatusCode )
	
	char		ifIP[ 32 ];
	char		ip[ 32 ];
	
	switch( inEvent->type )
	{
		case kDNSResolverEventTypeResolved:
			fprintf( stdout, "resolved \"%s.%s.%s\" to %s:%u on interface %s with text \"%s\"\n", 
					 inEvent->data.resolved.name, 
					 inEvent->data.resolved.type, 
					 inEvent->data.resolved.domain, 
					 IPv4ToString( inEvent->data.resolved.address.u.ipv4.address, ip ), 
					 inEvent->data.resolved.interfaceAddr.u.ipv4.port, 
					 IPv4ToString( inEvent->data.resolved.interfaceAddr.u.ipv4.address, ifIP ), 
					 inEvent->data.resolved.textRecord );
			break;
		
		case kDNSResolverEventTypeRelease:
			break;
		
		default:
			break;
	}
}

//===========================================================================================================================
//	RegistrationCallBack
//===========================================================================================================================

static void
	RegistrationCallBack( 
		void *							inContext, 
		DNSRegistrationRef				inRef, 
		DNSStatus						inStatusCode, 
		const DNSRegistrationEvent *	inEvent )
{
	#pragma unused( inContext, inRef, inStatusCode )
	
	switch( inEvent->type )
	{
		case kDNSRegistrationEventTypeRelease:	
			break;
		
		case kDNSRegistrationEventTypeRegistered:
			fprintf( stdout, "name registered and active\n" );
			break;

		case kDNSRegistrationEventTypeNameCollision:
			fprintf( stdout, "name in use, please choose another name\n" );
			break;
		
		default:
			break;
	}
}

//===========================================================================================================================
//	IPv4ToString
//===========================================================================================================================

static char *	IPv4ToString( DNSUInt32 inIP, char *outString )
{
	unsigned char *ip = (unsigned char *)&inIP;
	sprintf( outString, "%u.%u.%u.%u", ip[ 0 ], ip[ 1 ], ip[ 2 ], ip[ 3 ] );
	return( outString );
}
