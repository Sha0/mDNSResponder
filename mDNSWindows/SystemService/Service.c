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
    
$Log: Service.c,v $
Revision 1.5  2004/07/09 19:08:07  shersche
<rdar://problem/3713762> ServiceSetupEventLogging() errors are handled gracefully
Bug #: 3713762

Revision 1.4  2004/06/24 20:58:15  shersche
Fix compiler error in Release build
Submitted by: herscher

Revision 1.3  2004/06/24 15:28:53  shersche
Automatically setup routes to link-local addresses upon interface list change events.
Submitted by: herscher

Revision 1.2  2004/06/23 16:56:00  shersche
<rdar://problem/3697326> locked call to udsserver_idle().
Bug #: 3697326
Submitted by: herscher

Revision 1.1  2004/06/18 04:16:41  rpantos
Move up one level.

Revision 1.1  2004/01/30 02:58:39  bradley
mDNSResponder Windows Service. Provides global Bonjour support with an IPC interface.

*/

#include	<stdio.h>
#include	<stdlib.h>


#include	"CommonServices.h"
#include	"DebugServices.h"

#include	"uds_daemon.h"
#include	"GenLinkedList.h"

#include	"Resource.h"

#include	"mDNSClientAPI.h"
#include	"mDNSWin32.h"

#if( !TARGET_OS_WINDOWS_CE )
	#include	<mswsock.h>
	#include	<process.h>
	#include	<ipExport.h>
	#include	<iphlpapi.h>
	#include	<iptypes.h>
#endif

#if 0
#pragma mark == Constants ==
#endif

//===========================================================================================================================
//	Constants
//===========================================================================================================================

#define	DEBUG_NAME					"[Server] "
#define	kServiceName				"Apple mDNSResponder"
#define	kServiceDependencies		"Tcpip\0winmgmt\0\0"
#define	kDNSServiceCacheEntryCountDefault	512

#define RR_CACHE_SIZE 500
static CacheRecord gRRCache[RR_CACHE_SIZE];
#if 0
#pragma mark == Structures ==
#endif

//===========================================================================================================================
//	Structures
//===========================================================================================================================
//---------------------------------------------------------------------------------------------------------------------------
/*!	@typedef	EventSourceFlags

	@abstract	Session flags.
	
	@constant	EventSourceFlagsNone			No flags.
	@constant	EventSourceFlagsThreadDone		Thread is no longer active.
	@constant	EventSourceFlagsNoClose			Do not close the session when the thread exits.
	@constant	EventSourceFinalized			Finalize has been called for this session
*/

typedef uint32_t		EventSourceFlags;

#define	EventSourceFlagsNone			0
#define	EventSourceFlagsThreadDone		( 1 << 2 )
#define	EventSourceFlagsNoClose			( 1 << 3 )
#define EventSourceFinalized			( 1 << 4 )


typedef struct Win32EventSource
{
	EventSourceFlags			flags;
	HANDLE						threadHandle;
	unsigned					threadID;
	HANDLE						socketEvent;
	HANDLE						closeEvent;
	udsEventCallback			callback;
	void					*	context;
	DWORD						waitCount;
	HANDLE						waitList[2];
	SOCKET						sock;
	struct Win32EventSource	*	next;
} Win32EventSource;


#if 0
#pragma mark == Prototypes ==
#endif

//===========================================================================================================================
//	Prototypes
//===========================================================================================================================

int __cdecl 		main( int argc, char *argv[] );
static void			Usage( void );
static BOOL WINAPI	ConsoleControlHandler( DWORD inControlEvent );
static OSStatus		InstallService( const char *inName, const char *inDisplayName, const char *inDescription, const char *inPath );
static OSStatus		RemoveService( const char *inName );
static OSStatus		SetServiceParameters( SC_HANDLE inSCM, const char *inServiceName, const char *inDescription );
static void			ReportStatus( int inType, const char *inFormat, ... );
static OSStatus		RunDirect( int argc, char *argv[] );

static void WINAPI	ServiceMain( DWORD argc, LPSTR argv[] );
static OSStatus		ServiceSetupEventLogging( void );
static void WINAPI	ServiceControlHandler( DWORD inControl );

static OSStatus		ServiceRun( int argc, char *argv[] );
static void			ServiceStop( void );

static OSStatus		ServiceSpecificInitialize( int argc, char *argv[] );
static OSStatus		ServiceSpecificRun( int argc, char *argv[] );
static OSStatus		ServiceSpecificStop( void );
static void			ServiceSpecificFinalize( int argc, char *argv[] );
static mStatus		EventSourceFinalize(Win32EventSource * source);
static void			EventSourceLock();
static void			EventSourceUnlock();
static mDNSs32		udsIdle(mDNS * const inMDNS, mDNSs32 interval);
static void			InterfaceListChanged(mDNS * const inMDNS);
static OSStatus		GetRouteDestination(DWORD * ifIndex, DWORD * address);
static bool			HaveLLRoute(PMIB_IPFORWARDROW rowExtant);
static OSStatus		SetLLRoute();

#define kLLNetworkAddr      "169.254.0.0"
#define kLLNetworkAddrMask  "255.255.0.0"


#include	"mDNSClientAPI.h"

#if 0
#pragma mark == Globals ==
#endif

//===========================================================================================================================
//	Globals
//===========================================================================================================================
DEBUG_LOCAL	mDNS						gMDNSRecord;
DEBUG_LOCAL	mDNS_PlatformSupport		gPlatformStorage;
DEBUG_LOCAL BOOL						gServiceQuietMode		= FALSE;
DEBUG_LOCAL SERVICE_TABLE_ENTRY			gServiceDispatchTable[] = 
{
	{ kServiceName,	ServiceMain }, 
	{ NULL, 		NULL }
};
DEBUG_LOCAL SERVICE_STATUS				gServiceStatus;
DEBUG_LOCAL SERVICE_STATUS_HANDLE		gServiceStatusHandle 	= NULL;
DEBUG_LOCAL HANDLE						gServiceEventSource		= NULL;
DEBUG_LOCAL bool						gServiceAllowRemote		= false;
DEBUG_LOCAL int							gServiceCacheEntryCount	= 0;	// 0 means to use the DNS-SD default.
DEBUG_LOCAL int							gWaitCount				= 0;
DEBUG_LOCAL HANDLE					*	gWaitList				= NULL;
DEBUG_LOCAL HANDLE						gStopEvent				= NULL;
DEBUG_LOCAL CRITICAL_SECTION			gEventSourceLock;
DEBUG_LOCAL GenLinkedList				gEventSources;


#if 0
#pragma mark -
#endif

//===========================================================================================================================
//	main
//===========================================================================================================================

int	__cdecl main( int argc, char *argv[] )
{
	OSStatus		err;
	BOOL			ok;
	BOOL			start;
	int				i;
	
	debug_initialize( kDebugOutputTypeMetaConsole );
	debug_set_property( kDebugPropertyTagPrintLevel, kDebugLevelVerbose );
	
	// Install a Console Control Handler to handle things like control-c signals.
	
	ok = SetConsoleCtrlHandler( ConsoleControlHandler, TRUE );
	err = translate_errno( ok, (OSStatus) GetLastError(), kUnknownErr );
	require_noerr( err, exit );
	
	// Default to automatically starting the service dispatcher if no extra arguments are specified.
	
	start = ( argc <= 1 );
	
	// Parse arguments.
	
	for( i = 1; i < argc; ++i )
	{
		if( strcmp( argv[ i ], "-install" ) == 0 )			// Install
		{
			char		desc[ 256 ];
			
			desc[ 0 ] = 0;
			LoadStringA( GetModuleHandle( NULL ), IDS_SERVICE_DESCRIPTION, desc, sizeof( desc ) );
			err = InstallService( kServiceName, kServiceName, desc, argv[ 0 ] );
			if( err )
			{
				ReportStatus( EVENTLOG_ERROR_TYPE, "install service failed (%d)\n", err );
				goto exit;
			}
		}
		else if( strcmp( argv[ i ], "-remove" ) == 0 )		// Remove
		{
			err = RemoveService( kServiceName );
			if( err )
			{
				ReportStatus( EVENTLOG_ERROR_TYPE, "remove service failed (%d)\n", err );
				goto exit;
			}
		}
		else if( strcmp( argv[ i ], "-start" ) == 0 )		// Start
		{
			start = TRUE;
		}
		else if( strcmp( argv[ i ], "-server" ) == 0 )		// Server
		{
			err = RunDirect( argc, argv );
			if( err )
			{
				ReportStatus( EVENTLOG_ERROR_TYPE, "run service directly failed (%d)\n", err );
			}
			goto exit;
		}
		else if( strcmp( argv[ i ], "-q" ) == 0 )			// Quiet Mode (toggle)
		{
			gServiceQuietMode = !gServiceQuietMode;
		}
		else if( strcmp( argv[ i ], "-remote" ) == 0 )		// Allow Remote Connections
		{
			gServiceAllowRemote = true;
		}
		else if( strcmp( argv[ i ], "-cache" ) == 0 )		// Number of mDNS cache entries
		{
			if( i <= argc )
			{
				ReportStatus( EVENTLOG_ERROR_TYPE, "-cache used, but number of cache entries not specified\n" );
				err = kParamErr;
				goto exit;
			}
			gServiceCacheEntryCount = atoi( argv[ ++i ] );
		}
		else if( ( strcmp( argv[ i ], "-help" ) == 0 ) || 	// Help
				 ( strcmp( argv[ i ], "-h" ) == 0 ) )
		{
			Usage();
			err = 0;
			break;
		}
		else
		{
			Usage();
			err = kParamErr;
			break;
		}
	}
	
	// Start the service dispatcher if requested. This does not return until all services have terminated. If any 
	// global initialization is needed, it should be done before starting the service dispatcher, but only if it 
	// will take less than 30 seconds. Otherwise, use a separate thread for it and start the dispatcher immediately.
	
	if( start )
	{
		ok = StartServiceCtrlDispatcher( gServiceDispatchTable );
		err = translate_errno( ok, (OSStatus) GetLastError(), kInUseErr );
		if( err != kNoErr )
		{
			ReportStatus( EVENTLOG_ERROR_TYPE, "start service dispatcher failed (%d)\n", err );
			goto exit;
		}
	}
	err = 0;
	
exit:
	dlog( kDebugLevelTrace, DEBUG_NAME "exited (%d %m)\n", err, err );
	return( (int) err );
}

//===========================================================================================================================
//	Usage
//===========================================================================================================================

static void	Usage( void )
{
	fprintf( stderr, "\n" );
	fprintf( stderr, "mDNSResponder 1.0d1\n" );
	fprintf( stderr, "\n" );
	fprintf( stderr, "    <no args>    Runs the service normally\n" );
	fprintf( stderr, "    -install     Creates the service and starts it\n" );
	fprintf( stderr, "    -remove      Stops the service and deletes it\n" );
	fprintf( stderr, "    -start       Starts the service dispatcher after processing all other arguments\n" );
	fprintf( stderr, "    -server      Runs the service directly as a server (for debugging)\n" );
	fprintf( stderr, "    -q           Toggles Quiet Mode (no events or output)\n" );
	fprintf( stderr, "    -remote      Allow remote connections\n" );
	fprintf( stderr, "    -cache n     Number of mDNS cache entries (defaults to %d)\n", kDNSServiceCacheEntryCountDefault );
	fprintf( stderr, "    -h[elp]      Display Help/Usage\n" );
	fprintf( stderr, "\n" );
}

//===========================================================================================================================
//	ConsoleControlHandler
//===========================================================================================================================

static BOOL WINAPI	ConsoleControlHandler( DWORD inControlEvent )
{
	BOOL			handled;
	OSStatus		err;
	
	handled = FALSE;
	switch( inControlEvent )
	{
		case CTRL_C_EVENT:
		case CTRL_BREAK_EVENT:
		case CTRL_CLOSE_EVENT:
		case CTRL_LOGOFF_EVENT:
		case CTRL_SHUTDOWN_EVENT:
			err = ServiceSpecificStop();
			require_noerr( err, exit );
			
			handled = TRUE;
			break;
		
		default:
			break;
	}
	
exit:
	return( handled );
}

//===========================================================================================================================
//	InstallService
//===========================================================================================================================

static OSStatus	InstallService( const char *inName, const char *inDisplayName, const char *inDescription, const char *inPath )
{
	OSStatus		err;
	SC_HANDLE		scm;
	SC_HANDLE		service;
	BOOL			ok;
	TCHAR			fullPath[ MAX_PATH ];
	TCHAR *			namePtr;
	DWORD			size;
	
	scm		= NULL;
	service = NULL;
	
	// Get a full path to the executable since a relative path may have been specified.
	
	size = GetFullPathName( inPath, sizeof( fullPath ), fullPath, &namePtr );
	err = translate_errno( size > 0, (OSStatus) GetLastError(), kPathErr );
	require_noerr( err, exit );
	
	// Create the service and start it.
	
	scm = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	err = translate_errno( scm, (OSStatus) GetLastError(), kOpenErr );
	require_noerr( err, exit );
	
	service = CreateService( scm, inName, inDisplayName, SERVICE_ALL_ACCESS, SERVICE_WIN32_SHARE_PROCESS, 
							 SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, inPath, NULL, NULL, kServiceDependencies, 
							 NULL, NULL );
	err = translate_errno( service, (OSStatus) GetLastError(), kDuplicateErr );
	require_noerr( err, exit );
	
	if( inDescription )
	{
		err = SetServiceParameters( scm, inName, inDescription );
		check_noerr( err );
	}
	
	ok = StartService( service, 0, NULL );
	err = translate_errno( ok, (OSStatus) GetLastError(), kInUseErr );
	require_noerr( err, exit );
	
	ReportStatus( EVENTLOG_SUCCESS, "installed service \"%s\"/\"%s\" at \"%s\"\n", inName, inDisplayName, inPath );
	err = kNoErr;
	
exit:
	if( service )
	{
		CloseServiceHandle( service );
	}
	if( scm )
	{
		CloseServiceHandle( scm );
	}
	return( err );
}

//===========================================================================================================================
//	RemoveService
//===========================================================================================================================

static OSStatus	RemoveService( const char *inName )
{
	OSStatus			err;
	SC_HANDLE			scm;
	SC_HANDLE			service;
	BOOL				ok;
	SERVICE_STATUS		status;
	
	scm		= NULL;
	service = NULL;
	
	// Open a connection to the service.
	
	scm = OpenSCManager( 0, 0, SC_MANAGER_ALL_ACCESS );
	err = translate_errno( scm, (OSStatus) GetLastError(), kOpenErr );
	require_noerr( err, exit );
	
	service = OpenService( scm, inName, SERVICE_STOP | SERVICE_QUERY_STATUS | DELETE );
	err = translate_errno( service, (OSStatus) GetLastError(), kNotFoundErr );
	require_noerr( err, exit );
	
	// Stop the service, if it is not already stopped, then delete it.
	
	ok = QueryServiceStatus( service, &status );
	err = translate_errno( ok, (OSStatus) GetLastError(), kAuthenticationErr );
	require_noerr( err, exit );
	
	if( status.dwCurrentState != SERVICE_STOPPED )
	{
		ok = ControlService( service, SERVICE_CONTROL_STOP, &status );
		check_translated_errno( ok, (OSStatus) GetLastError(), kAuthenticationErr );
	}
	
	ok = DeleteService( service );
	err = translate_errno( ok, (OSStatus) GetLastError(), kDeletedErr );
	require_noerr( err, exit );
		
	ReportStatus( EVENTLOG_SUCCESS, "Removed service \"%s\"\n", inName );
	err = ERROR_SUCCESS;
	
exit:
	if( service )
	{
		CloseServiceHandle( service );
	}
	if( scm )
	{
		CloseServiceHandle( scm );
	}
	return( err );
}

//===========================================================================================================================
//	SetServiceParameters
//===========================================================================================================================

static OSStatus	SetServiceParameters( SC_HANDLE inSCM, const char *inServiceName, const char *inDescription )
{
	OSStatus				err;
	SC_LOCK					lock;
	SC_HANDLE				service;
	SERVICE_DESCRIPTION		description;
	SERVICE_FAILURE_ACTIONS	actions;
	SC_ACTION				action;
	BOOL					ok;
	
	check( inServiceName );
	check( inDescription );
	
	lock 	= NULL;
	service	= NULL;
	
	// Open the database (if not provided) and lock it to prevent other access while re-configuring.
	
	if( !inSCM )
	{
		inSCM = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
		err = translate_errno( inSCM, (OSStatus) GetLastError(), kOpenErr );
		require_noerr( err, exit );
	}
	
	lock = LockServiceDatabase( inSCM );
	err = translate_errno( lock, (OSStatus) GetLastError(), kInUseErr );
	require_noerr( err, exit );
	
	// Open a handle to the service. 

	service = OpenService( inSCM, inServiceName, SERVICE_CHANGE_CONFIG|SERVICE_START );
	err = translate_errno( service, (OSStatus) GetLastError(), kNotFoundErr );
	require_noerr( err, exit );
	
	// Change the description.
	
	description.lpDescription = (char *) inDescription;
	ok = ChangeServiceConfig2( service, SERVICE_CONFIG_DESCRIPTION, &description );
	err = translate_errno( ok, (OSStatus) GetLastError(), kParamErr );
	require_noerr( err, exit );
	
	actions.dwResetPeriod	=	INFINITE;
	actions.lpRebootMsg		=	NULL;
	actions.lpCommand		=	NULL;
	actions.cActions		=	1;
	actions.lpsaActions		=	&action;
	action.Delay			=	500;
	action.Type				=	SC_ACTION_RESTART;

	ok = ChangeServiceConfig2( service, SERVICE_CONFIG_FAILURE_ACTIONS, &actions );
	err = translate_errno( ok, (OSStatus) GetLastError(), kParamErr );
	require_noerr( err, exit );
	
	err = ERROR_SUCCESS;
	
exit:
	// Close the service and release the lock.
	
	if( service )
	{
		CloseServiceHandle( service );
	}
	if( lock )
	{
		UnlockServiceDatabase( lock ); 
	}
	return( err );
}

//===========================================================================================================================
//	ReportStatus
//===========================================================================================================================

static void	ReportStatus( int inType, const char *inFormat, ... )
{
	if( !gServiceQuietMode )
	{
		va_list		args;
		
		va_start( args, inFormat );
		if( gServiceEventSource )
		{
			char				s[ 1024 ];
			BOOL				ok;
			const char *		array[ 1 ];
			
			vsprintf( s, inFormat, args );
			array[ 0 ] = s;
			ok = ReportEvent( gServiceEventSource, (WORD) inType, 0, 0x20000001L, NULL, 1, 0, array, NULL );
			check_translated_errno( ok, GetLastError(), kUnknownErr );
		}
		else
		{
			int		n;
			
			n = vfprintf( stderr, inFormat, args );
			check( n >= 0 );
		}
		va_end( args );
	}
}

//===========================================================================================================================
//	RunDirect
//===========================================================================================================================

static OSStatus	RunDirect( int argc, char *argv[] )
{
	OSStatus		err;
	BOOL			initialized;
	
	initialized = FALSE;
	
	err = ServiceSpecificInitialize( argc, argv );
	require_noerr( err, exit );
	initialized = TRUE;
	
	// Run the service. This does not return until the service quits or is stopped.
	
	ReportStatus( EVENTLOG_SUCCESS, "Running \"%s\" service directly\n", kServiceName );
	
	err = ServiceSpecificRun( argc, argv );
	require_noerr( err, exit );
	
	// Clean up.
	
exit:
	if( initialized )
	{
		ServiceSpecificFinalize( argc, argv );
	}
	return( err );
}

#if 0
#pragma mark -
#endif

//===========================================================================================================================
//	ServiceMain
//===========================================================================================================================

static void WINAPI ServiceMain( DWORD argc, LPSTR argv[] )
{
	OSStatus		err;
	BOOL			ok;
	char			desc[ 256 ];
	
	err = ServiceSetupEventLogging();
	check_noerr( err );
	
	// Initialize the service status and register the service control handler with the name of the service.
	
	gServiceStatus.dwServiceType 				= SERVICE_WIN32_SHARE_PROCESS;
	gServiceStatus.dwCurrentState 				= 0;
	gServiceStatus.dwControlsAccepted 			= SERVICE_ACCEPT_STOP;
	gServiceStatus.dwWin32ExitCode 				= NO_ERROR;
	gServiceStatus.dwServiceSpecificExitCode 	= NO_ERROR;
	gServiceStatus.dwCheckPoint 				= 0;
	gServiceStatus.dwWaitHint 					= 0;
	
	gServiceStatusHandle = RegisterServiceCtrlHandler( argv[ 0 ], ServiceControlHandler );
	err = translate_errno( gServiceStatusHandle, (OSStatus) GetLastError(), kInUseErr );
	require_noerr( err, exit );
	
	// Setup the description. This should be done by the installer, but it doesn't support that yet.
			
	desc[ 0 ] = '\0';
	LoadStringA( GetModuleHandle( NULL ), IDS_SERVICE_DESCRIPTION, desc, sizeof( desc ) );
	err = SetServiceParameters( NULL, kServiceName, desc );
	check_noerr( err );
	
	// Mark the service as starting.
	
	gServiceStatus.dwCurrentState 	= SERVICE_START_PENDING;
	gServiceStatus.dwCheckPoint	 	= 0;
	gServiceStatus.dwWaitHint 		= 5000;	// 5 seconds
	ok = SetServiceStatus( gServiceStatusHandle, &gServiceStatus );
	check_translated_errno( ok, GetLastError(), kParamErr );
	
	// Run the service. This does not return until the service quits or is stopped.
	
	err = ServiceRun( (int) argc, argv );
	if( err != kNoErr )
	{
		gServiceStatus.dwWin32ExitCode				= ERROR_SERVICE_SPECIFIC_ERROR;
		gServiceStatus.dwServiceSpecificExitCode 	= (DWORD) err;
	}
	
	// Service-specific work is done so mark the service as stopped.
	
	gServiceStatus.dwCurrentState = SERVICE_STOPPED;
	ok = SetServiceStatus( gServiceStatusHandle, &gServiceStatus );
	check_translated_errno( ok, GetLastError(), kParamErr );
	
	// Note: The service status handle should not be closed according to Microsoft documentation.
	
exit:
	if( gServiceEventSource )
	{
		ok = DeregisterEventSource( gServiceEventSource );
		check_translated_errno( ok, GetLastError(), kUnknownErr );
		gServiceEventSource = NULL;
	}
}

//===========================================================================================================================
//	ServiceSetupEventLogging
//===========================================================================================================================

static OSStatus	ServiceSetupEventLogging( void )
{
	OSStatus			err;
	HKEY				key;
	const char *		s;
	DWORD				typesSupported;
	char				path[ MAX_PATH ];
	DWORD 				n;
	
	key = NULL;
	
	// Add/Open source name as a sub-key under the Application key in the EventLog registry key.

	s = "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\" kServiceName;
	err = RegCreateKey( HKEY_LOCAL_MACHINE, s, &key );
	require_noerr( err, exit );
	
	// Add the name to the EventMessageFile subkey.
	
	path[ 0 ] = '\0';
	GetModuleFileName( NULL, path, MAX_PATH );
	n = (DWORD)( strlen( path ) + 1 );
	err = RegSetValueEx( key, "EventMessageFile", 0, REG_EXPAND_SZ, (const LPBYTE) path, n );
	require_noerr( err, exit );
	
	// Set the supported event types in the TypesSupported subkey.
	
	typesSupported = 0 
					 | EVENTLOG_SUCCESS
					 | EVENTLOG_ERROR_TYPE
					 | EVENTLOG_WARNING_TYPE
					 | EVENTLOG_INFORMATION_TYPE
					 | EVENTLOG_AUDIT_SUCCESS
					 | EVENTLOG_AUDIT_FAILURE; 
	err = RegSetValueEx( key, "TypesSupported", 0, REG_DWORD, (const LPBYTE) &typesSupported, sizeof( DWORD ) );
	require_noerr( err, exit );
	
	// Set up the event source.
	
	gServiceEventSource = RegisterEventSource( NULL, kServiceName );
	err = translate_errno( gServiceEventSource, (OSStatus) GetLastError(), kParamErr );
	require_noerr( err, exit );
		
exit:
	if( key )
	{
		RegCloseKey( key );
	}
	return( err );
}

//===========================================================================================================================
//	ServiceControlHandler
//===========================================================================================================================

static void WINAPI	ServiceControlHandler( DWORD inControl )
{
	BOOL		setStatus;
	BOOL		ok;
	
	setStatus = TRUE;
	switch( inControl )
	{
		case SERVICE_CONTROL_STOP:
			dlog( kDebugLevelNotice, DEBUG_NAME "ServiceControlHandler: SERVICE_CONTROL_STOP\n" );
			
			ServiceStop();
			setStatus = FALSE;
			break;
		
		default:
			dlog( kDebugLevelNotice, DEBUG_NAME "ServiceControlHandler: event (0x%08X)\n", inControl );
			break;
	}
	
	if( setStatus && gServiceStatusHandle )
	{
		ok = SetServiceStatus( gServiceStatusHandle, &gServiceStatus );
		check_translated_errno( ok, GetLastError(), kUnknownErr );
	}
}

//===========================================================================================================================
//	ServiceRun
//===========================================================================================================================

static OSStatus	ServiceRun( int argc, char *argv[] )
{
	OSStatus		err;
	BOOL			initialized;
	BOOL			ok;
	
	DEBUG_UNUSED( argc );
	DEBUG_UNUSED( argv );
	
	initialized = FALSE;
	
	// Initialize the service-specific stuff and mark the service as running.
	
	err = ServiceSpecificInitialize( argc, argv );
	require_noerr( err, exit );
	initialized = TRUE;
	
	gServiceStatus.dwCurrentState = SERVICE_RUNNING;
	ok = SetServiceStatus( gServiceStatusHandle, &gServiceStatus );
	check_translated_errno( ok, GetLastError(), kParamErr );
	
	// Run the service-specific stuff. This does not return until the service quits or is stopped.
	
	ReportStatus( EVENTLOG_INFORMATION_TYPE, "mDNSResponder started\n" );
	err = ServiceSpecificRun( argc, argv );
	ReportStatus( EVENTLOG_INFORMATION_TYPE, "mDNSResponder stopped (%d)\n", err );
	require_noerr( err, exit );
	
	// Service stopped. Clean up and we're done.
	
exit:
	if( initialized )
	{
		ServiceSpecificFinalize( argc, argv );
	}
	return( err );
}

//===========================================================================================================================
//	ServiceStop
//===========================================================================================================================

static void	ServiceStop( void )
{
	BOOL			ok;
	OSStatus		err;
	
	// Signal the event to cause the service to exit.
	
	if( gServiceStatusHandle )
	{
		gServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
		ok = SetServiceStatus( gServiceStatusHandle, &gServiceStatus );
		check_translated_errno( ok, GetLastError(), kParamErr );
	}
		
	err = ServiceSpecificStop();
	check_noerr( err );
}

#if 0
#pragma mark -
#pragma mark == Service Specific ==
#endif

//===========================================================================================================================
//	ServiceSpecificInitialize
//===========================================================================================================================

static OSStatus	ServiceSpecificInitialize( int argc, char *argv[] )
{
	OSStatus						err;
	
	DEBUG_UNUSED( argc );
	DEBUG_UNUSED( argv );
	
	memset( &gMDNSRecord, 0, sizeof gMDNSRecord);
	memset( &gPlatformStorage, 0, sizeof gPlatformStorage);

	gPlatformStorage.interfaceListChangedCallback = InterfaceListChanged;
	gPlatformStorage.idleThreadCallback = udsIdle;

	InitializeCriticalSection(&gEventSourceLock);
	
	gStopEvent	=	CreateEvent(NULL, FALSE, FALSE, NULL);
	err = translate_errno( gStopEvent, errno_compat(), kNoResourcesErr );
	require_noerr( err, exit );

	err = mDNS_Init( &gMDNSRecord, &gPlatformStorage, gRRCache, RR_CACHE_SIZE, mDNS_Init_AdvertiseLocalAddresses, mDNS_Init_NoInitCallback, mDNS_Init_NoInitCallbackContext); 
	require_noerr( err, exit);

	err = udsserver_init(&gMDNSRecord);
	require_noerr( err, exit);

	//
	// set a route to link local addresses (169.254.0.0)
	//
	SetLLRoute();

exit:
	if( err != kNoErr )
	{
		ServiceSpecificFinalize( argc, argv );
	}
	return( err );
}

//===========================================================================================================================
//	ServiceSpecificRun
//===========================================================================================================================

static OSStatus	ServiceSpecificRun( int argc, char *argv[] )
{
	DWORD result;
	
	DEBUG_UNUSED( argc );
	DEBUG_UNUSED( argv );

	// Main event loop. Process connection requests and state changes (i.e. quit).
	while( (result = WaitForSingleObject(gStopEvent, INFINITE)) != WAIT_OBJECT_0 )
	{
		// Unexpected wait result.
		dlog( kDebugLevelWarning, DEBUG_NAME "%s: unexpected wait result (result=0x%08X)\n", __ROUTINE__, result );
	}

	return kNoErr;
}

//===========================================================================================================================
//	ServiceSpecificStop
//===========================================================================================================================

static OSStatus	ServiceSpecificStop( void )
{
	OSStatus	err;
	BOOL	 	ok;

	ok = SetEvent(gStopEvent);
	err = translate_errno( ok, (OSStatus) GetLastError(), kUnknownErr );
	require_noerr( err, exit );
exit:
	return( err );
}

//===========================================================================================================================
//	ServiceSpecificFinalize
//===========================================================================================================================

static void	ServiceSpecificFinalize( int argc, char *argv[] )
{
	DEBUG_UNUSED( argc );
	DEBUG_UNUSED( argv );
	
	//
	// clean up any open sessions
	//
	while (gEventSources.Head)
	{
		EventSourceFinalize((Win32EventSource*) gEventSources.Head);
	}
	//
	// give a chance for the udsserver code to clean up
	//
	udsserver_exit();

	//
	// and finally close down the mDNSCore
	//
	mDNS_Close(&gMDNSRecord);

	//
	// clean up the event sources mutex...no one should be using it now
	//
	DeleteCriticalSection(&gEventSourceLock);
}


static void
InterfaceListChanged(mDNS * const inMDNS)
{
	DEBUG_UNUSED( inMDNS );

	SetLLRoute();
}


static mDNSs32
udsIdle(mDNS * const inMDNS, mDNSs32 interval)
{
	DEBUG_UNUSED( inMDNS );

	//
	// rdar://problem/3697326
	//
	// udsserver_idle wasn't being locked.  This resulted
	// in multiple threads contesting for the all_requests
	// data structure in uds_daemon.c
	//
	mDNSPlatformLock(&gMDNSRecord);

	interval = udsserver_idle(interval);

	mDNSPlatformUnlock(&gMDNSRecord);

	return interval;
}


mDNSlocal unsigned WINAPI
udsSocketThread(LPVOID inParam)
{
	Win32EventSource	*	source = (Win32EventSource*) inParam;
	bool					safeToClose;
	mStatus					err    = 0;

	while (!(source->flags & EventSourceFinalized))
	{
		DWORD result;

		result = WaitForMultipleObjects(source->waitCount, source->waitList, FALSE, INFINITE);
		
		//
		// socket event
		//
		if (result == WAIT_OBJECT_0)
		{
			mDNSPlatformLock(&gMDNSRecord);
			source->callback(source->context);
			mDNSPlatformUnlock(&gMDNSRecord);
		}
		//
		// close event
		//
		else if (result == WAIT_OBJECT_0 + 1)
		{
			mDNSPlatformLock(&gMDNSRecord);

			//
			// this is a bit of a hack.  we want to clean up the internal data structures
			// so we'll go in here and it will clean up for us
			//
			shutdown(source->sock, 2);
			source->callback(source->context);

			mDNSPlatformUnlock(&gMDNSRecord);

			break;
		}
		else
		{
			// Unexpected wait result.
			dlog( kDebugLevelWarning, DEBUG_NAME "%s: unexpected wait result (result=0x%08X)\n", __ROUTINE__, result );
		}
	}

	EventSourceLock();
	source->flags |= EventSourceFlagsThreadDone;
	safeToClose = !( source->flags & EventSourceFlagsNoClose );
	EventSourceUnlock();

	if( safeToClose )
	{
		EventSourceFinalize( source );
	}

	_endthreadex_compat( (unsigned) err );
	return( (unsigned) err );	
}


mStatus
udsSupportAddFDToEventLoop( SocketRef fd, udsEventCallback callback, void *context)
{
	Win32EventSource *  newSource;
	DWORD				result;
	mStatus				err;

	newSource = malloc(sizeof(Win32EventSource));
	require_action( newSource, exit, err = mStatus_NoMemoryErr );
	memset(newSource, 0, sizeof(Win32EventSource));

	newSource->flags	= 0;
	newSource->sock		= (SOCKET) fd;
	newSource->callback	= callback;
	newSource->context	= context;

	newSource->socketEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	err = translate_errno( newSource->socketEvent, (mStatus) GetLastError(), kUnknownErr );
	require_noerr( err, exit );

	newSource->closeEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	err = translate_errno( newSource->closeEvent, (mStatus) GetLastError(), kUnknownErr );
	require_noerr( err, exit );

	err = WSAEventSelect(newSource->sock, newSource->socketEvent, FD_ACCEPT|FD_READ|FD_CLOSE);
	err = translate_errno( err == 0, errno_compat(), kNoResourcesErr );
	require_noerr( err, exit );

	newSource->waitCount = 0;
	newSource->waitList[ newSource->waitCount++ ] = newSource->socketEvent;
	newSource->waitList[ newSource->waitCount++ ] = newSource->closeEvent;

	//
	// lock the list
	//
	EventSourceLock();
	
	// add the event source to the end of the list, while checking
	// to see if the list needs to be initialized
	//
	if ( gEventSources.LinkOffset == 0)
	{
		InitLinkedList( &gEventSources, offsetof( Win32EventSource, next));
	}

	AddToTail( &gEventSources, newSource);

	//
	// no longer using the list
	//
	EventSourceUnlock();

	// Create thread with _beginthreadex() instead of CreateThread() to avoid memory leaks when using static run-time 
	// libraries. See <http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dllproc/base/createthread.asp>.
	// Create the thread suspended then resume it so the thread handle and ID are valid before the thread starts running.
	newSource->threadHandle = (HANDLE) _beginthreadex_compat( NULL, 0, udsSocketThread, newSource, CREATE_SUSPENDED, &newSource->threadID );
	err = translate_errno( newSource->threadHandle, (mStatus) GetLastError(), kUnknownErr );
	require_noerr( err, exit );

	result = ResumeThread( newSource->threadHandle );
	err = translate_errno( result != (DWORD) -1, errno_compat(), kNoResourcesErr );
	require_noerr( err, exit );

exit:

	if (err && newSource)
	{
		EventSourceFinalize(newSource);
	}

	return err;
}


mStatus
udsSupportRemoveFDFromEventLoop( SocketRef fd)
{
	Win32EventSource	*	source;
	mStatus					err = mStatus_NoError;
	
	//
	// find the event source
	//
	EventSourceLock();

	for (source = gEventSources.Head; source; source = source->next)
	{
		if (source->sock == (SOCKET) fd)
		{
			break;
		}
	}

	//
	// if we found him, finalize him
	//
	if (source != NULL)
	{
		EventSourceFinalize(source);
	}

	//
	// done with the list
	//
	EventSourceUnlock();

	return err;
}


static mStatus
EventSourceFinalize(Win32EventSource * source)
{
	OSStatus				err;
	bool					locked;
	Win32EventSource	*	inserted;
	bool				sameThread;
	bool				deferClose;
	BOOL				ok;
	DWORD 				threadID;
	DWORD				result;
	
	check( source );
	
	// Find the session in the list.
	
	EventSourceLock();
	locked = true;
	
	for( inserted = (Win32EventSource*) gEventSources.Head; inserted; inserted = inserted->next )
	{
		if( inserted == source )
		{
			break;
		}
	}
	require_action( inserted, exit, err = kNotFoundErr );

	//
	// note that we've had finalize called
	//
	source->flags |= EventSourceFinalized;
	
	// If we're being called from the same thread as the session (e.g. message callback is closing the session) then 
	// we must defer the close until the thread is done because the thread is still using the session object.
	
	deferClose	= false;
	threadID	= GetCurrentThreadId();
	sameThread	= source->threadHandle && ( threadID == source->threadID );
	if( sameThread && !( source->flags & EventSourceFlagsThreadDone ) )
	{
		source->flags &= ~EventSourceFlagsNoClose;
		deferClose = true;
	}
	
	// If the thread we're not being called from the session thread, but the thread has already marked itself as
	// as done (e.g. session closed from something like a peer disconnect and at the same time the client also 
	// tried to close) then we only want to continue with the close if the thread is not going to close itself.
	
	if( !sameThread && ( source->flags & EventSourceFlagsThreadDone ) && !( source->flags & EventSourceFlagsNoClose ) )
	{
		deferClose = true;
	}
	
	// Signal a close so the thread exits.
	
	if( source->closeEvent )
	{
		ok = SetEvent( source->closeEvent );
		check_translated_errno( ok, errno_compat(), kUnknownErr );
	}	
	if( deferClose )
	{
		err = kNoErr;
		goto exit;
	}
	
	source->flags |= EventSourceFlagsNoClose;
	
	// Remove the session from the list.
	RemoveFromList(&gEventSources, source);
	
	EventSourceUnlock();
	locked = false;
	
	// Wait for the thread to exit. Give up after 10 seconds to handle a hung thread.
	
	if( source->threadHandle && ( threadID != source->threadID ) )
	{
		result = WaitForSingleObject( source->threadHandle, 10 * 1000 );
		check_translated_errno( result == WAIT_OBJECT_0, (OSStatus) GetLastError(), result );
	}
	
	// Release the thread.
	
	if( source->threadHandle )
	{
		ok = CloseHandle( source->threadHandle );
		check_translated_errno( ok, errno_compat(), kUnknownErr );
		source->threadHandle = NULL;
	}
	
	// Release the socket event.
	
	if( source->socketEvent )
	{
		ok = CloseHandle( source->socketEvent );
		check_translated_errno( ok, errno_compat(), kUnknownErr );
		source->socketEvent = NULL;
	}
	
	// Release the close event.
	
	if( source->closeEvent )
	{
		ok = CloseHandle( source->closeEvent );
		check_translated_errno( ok, errno_compat(), kUnknownErr );
		source->closeEvent = NULL;
	}
	
	// Release the memory used by the object.
	free ( source );

	err = kNoErr;
	
	dlog( kDebugLevelNotice, DEBUG_NAME "session closed\n" );
	
exit:

	if( locked )
	{
		EventSourceUnlock();
	}

	return( err );
}


static void
EventSourceLock()
{
	EnterCriticalSection(&gEventSourceLock);
}


static void
EventSourceUnlock()
{
	LeaveCriticalSection(&gEventSourceLock);
}


//===========================================================================================================================
//	HaveLLRoute
//===========================================================================================================================

static bool
HaveLLRoute(PMIB_IPFORWARDROW rowExtant)
{
	PMIB_IPFORWARDTABLE	pIpForwardTable	= NULL;
	DWORD				dwSize			= 0;
	BOOL				bOrder			= FALSE;
	OSStatus			err;
	bool				found			= false;
	unsigned long int	i;

	//
	// Find out how big our buffer needs to be.
	//
	err = GetIpForwardTable(NULL, &dwSize, bOrder);
	require_action( err == ERROR_INSUFFICIENT_BUFFER, exit, err = kUnknownErr );

	//
	// Allocate the memory for the table
	//
	pIpForwardTable = (PMIB_IPFORWARDTABLE) malloc( dwSize );
	require_action( pIpForwardTable, exit, err = kNoMemoryErr );
  
	//
	// Now get the table.
	//
	err = GetIpForwardTable(pIpForwardTable, &dwSize, bOrder);
	require_noerr( err, exit );

	//
	// Search for the row in the table we want.
	//
	for ( i = 0; i < pIpForwardTable->dwNumEntries; i++)
	{
		if (pIpForwardTable->table[i].dwForwardDest == inet_addr(kLLNetworkAddr))
		{
			memcpy( rowExtant, &(pIpForwardTable->table[i]), sizeof(*rowExtant) );
			found = true;
			break;
		}
	}

exit:

	if ( pIpForwardTable != NULL ) 
	{
		free(pIpForwardTable);
	}
    
	return found;
}


//===========================================================================================================================
//	SetLLRoute
//===========================================================================================================================

static OSStatus
SetLLRoute()
{
	DWORD				ifIndex;
	MIB_IPFORWARDROW	rowExtant;
	bool				addRoute;
	MIB_IPFORWARDROW	row;
	OSStatus			err;

	ZeroMemory(&row, sizeof(row));

	err = GetRouteDestination(&ifIndex, &row.dwForwardNextHop);
	require_noerr( err, exit );
	row.dwForwardDest		= inet_addr(kLLNetworkAddr);
	row.dwForwardIfIndex	= ifIndex;
	row.dwForwardMask		= inet_addr(kLLNetworkAddrMask);
	row.dwForwardType		= 3;
	row.dwForwardProto		= MIB_IPPROTO_NETMGMT;
	row.dwForwardAge		= 0;
	row.dwForwardPolicy		= 0;
	row.dwForwardMetric1	= 30;
	row.dwForwardMetric2	= (DWORD) - 1;
	row.dwForwardMetric3	= (DWORD) - 1;
	row.dwForwardMetric4	= (DWORD) - 1;
	row.dwForwardMetric5	= (DWORD) - 1;

	addRoute = true;

	//
	// check to make sure we don't already have a route
	//
	if (HaveLLRoute(&rowExtant))
	{
		//
		// set the age to 0 so that we can do a memcmp.
		//
		rowExtant.dwForwardAge = 0;

		//
		// check to see if this route is the same as our route
		//
		if (memcmp(&row, &rowExtant, sizeof(row)) != 0)
		{
			//
			// if it isn't then delete this entry
			//
			DeleteIpForwardEntry(&rowExtant);
		}
		else
		{
			//
			// else it is, so we don't want to create another route
			//
			addRoute = false;
		}
	}

	if (addRoute && row.dwForwardNextHop)
	{
		err = CreateIpForwardEntry(&row);

		require_noerr( err, exit );
	}

exit:

	return ( err );
}


//===========================================================================================================================
//	GetRouteDestination
//===========================================================================================================================

static OSStatus
GetRouteDestination(DWORD * ifIndex, DWORD * address)
{
	struct in_addr		ia;
	IP_ADAPTER_INFO	*	pAdapterInfo	=	NULL;
	IP_ADAPTER_INFO	*	pAdapter		=	NULL;
	ULONG				bufLen;
	OSStatus			err;

	//
	// GetBestInterface will fail if there is no default gateway
	// configured.  If that happens, we will just take the first
	// interface in the list. MSDN support says there is no surefire
	// way to manually determine what the best interface might
	// be for a particular network address.
	//
	ia.s_addr	=	inet_addr(kLLNetworkAddr);
	err			=	GetBestInterface(*(IPAddr*) &ia, ifIndex);

	if (err)
	{
		*ifIndex = 0;
	}

	//
	// Make an initial call to GetAdaptersInfo to get
	// the necessary size into the bufLen variable
	//
	err = GetAdaptersInfo( NULL, &bufLen);
	require_action( err == ERROR_BUFFER_OVERFLOW, exit, err = kUnknownErr );

	pAdapterInfo = (IP_ADAPTER_INFO*) malloc( bufLen );
	require_action( pAdapterInfo, exit, err = kNoMemoryErr );
	
	err = GetAdaptersInfo( pAdapterInfo, &bufLen);
	require_noerr( err, exit );
	
	pAdapter	=	pAdapterInfo;
	err			=	kUnknownErr;
			
	while (pAdapter)
	{
		//
		// if we don't have an interface selected, choose the first one
		//
		if (!(*ifIndex) || (pAdapter->Index == (*ifIndex)))
		{
			*address =	inet_addr( pAdapter->IpAddressList.IpAddress.String );
			*ifIndex =  pAdapter->Index;
			err		 =	kNoErr;
			break;
		}
	
		pAdapter = pAdapter->Next;
	}

exit:

	if ( pAdapterInfo != NULL )
	{
		free( pAdapterInfo );
	}

	return( err );
}

