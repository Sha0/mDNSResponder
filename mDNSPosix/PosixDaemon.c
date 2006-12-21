/* -*- Mode: C; tab-width: 4 -*-
 *
 * Copyright (c) 2003-2004 Apple Computer, Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.

	File:		daemon.c

	Contains:	main & associated Application layer for mDNSResponder on Linux.

	Change History (most recent first):

$Log: PosixDaemon.c,v $
Revision 1.33  2006/12/21 00:10:53  cheshire
Make mDNS_PlatformSupport PlatformStorage a static global instead of a stack variable

Revision 1.32  2006/11/03 22:28:50  cheshire
PosixDaemon needs to handle mStatus_ConfigChanged and mStatus_GrowCache messages

Revision 1.31  2006/08/14 23:24:46  cheshire
Re-licensed mDNSResponder daemon source code under Apache License, Version 2.0

Revision 1.30  2006/07/07 01:09:12  cheshire
<rdar://problem/4472013> Add Private DNS server functionality to dnsextd
Only use mallocL/freeL debugging routines when building mDNSResponder, not dnsextd

Revision 1.29  2005/08/04 03:37:45  mkrochma
Temporary workaround to fix posix after mDNS_SetPrimaryInterfaceInfo changed

Revision 1.28  2005/07/19 11:21:09  cheshire
<rdar://problem/4170449> Unix Domain Socket leak in mdnsd

Revision 1.27  2005/02/04 00:39:59  cheshire
Move ParseDNSServers() from PosixDaemon.c to mDNSPosix.c so all Posix client layers can use it

Revision 1.26  2005/02/02 02:21:30  cheshire
Update references to "mDNSResponder" to say "mdnsd" instead

Revision 1.25  2005/01/27 20:01:50  cheshire
udsSupportRemoveFDFromEventLoop() needs to close the file descriptor as well

Revision 1.24  2005/01/19 19:20:49  ksekar
<rdar://problem/3960191> Need a way to turn off domain discovery

Revision 1.23  2004/12/16 20:17:11  cheshire
<rdar://problem/3324626> Cache memory management improvements

Revision 1.22  2004/12/10 13:12:08  cheshire
Create no-op function RecordUpdatedNiceLabel(), required by uds_daemon.c

Revision 1.21  2004/12/01 20:57:20  ksekar
<rdar://problem/3873921> Wide Area Service Discovery must be split-DNS aware

Revision 1.20  2004/12/01 04:28:43  cheshire
<rdar://problem/3872803> Darwin patches for Solaris and Suse
Use version of daemon() provided in mDNSUNP.c instead of local copy

Revision 1.19  2004/12/01 03:30:29  cheshire
<rdar://problem/3889346> Add Unicast DNS support to mDNSPosix

Revision 1.18  2004/11/30 22:45:59  cheshire
Minor code tidying

Revision 1.17  2004/11/30 22:18:59  cheshire
<rdar://problem/3889351> Posix needs to read the list of unicast DNS servers and set server list

Revision 1.16  2004/09/21 21:05:12	cheshire
Move duplicate code out of mDNSMacOSX/daemon.c and mDNSPosix/PosixDaemon.c,
into mDNSShared/uds_daemon.c

Revision 1.15  2004/09/17 01:08:53	cheshire
Renamed mDNSClientAPI.h to mDNSEmbeddedAPI.h
  The name "mDNSClientAPI.h" is misleading to new developers looking at this code. The interfaces
  declared in that file are ONLY appropriate to single-address-space embedded applications.
  For clients on general-purpose computers, the interfaces defined in dns_sd.h should be used.

Revision 1.14  2004/09/16 00:24:49	cheshire
<rdar://problem/3803162> Fix unsafe use of mDNSPlatformTimeNow()

Revision 1.13  2004/08/11 01:59:41	cheshire
Remove "mDNS *globalInstance" parameter from udsserver_init()

Revision 1.12  2004/06/28 23:19:19	cheshire
Fix "Daemon_Init declared but never defined" warning on Linux

Revision 1.11  2004/06/25 00:26:27	rpantos
Changes to fix the Posix build on Solaris.

Revision 1.10  2004/06/08 04:59:40	cheshire
Tidy up wording -- log messages are already prefixed with "mDNSResponder", so don't need to repeat it

Revision 1.9  2004/05/29 00:14:20  rpantos
<rdar://problem/3508093> Runtime check to disable prod mdnsd on OS X.

Revision 1.8  2004/04/07 01:19:04  cheshire
Hash slot value should be unsigned

Revision 1.7  2004/02/14 06:34:57  cheshire
Use LogMsg instead of fprintf( stderr

Revision 1.6  2004/02/14 01:10:42  rpantos
Allow daemon to run if 'nobody' is not defined, with a warning. (For Roku HD1000.)

Revision 1.5  2004/02/05 07:45:43  cheshire
Add Log header

Revision 1.4  2004/01/28 21:14:23  cheshire
Reconcile debug_mode and gDebugLogging into a single flag (mDNS_DebugMode)

Revision 1.3  2004/01/19 19:51:46  cheshire
Fix compiler error (mixed declarations and code) on some versions of Linux

Revision 1.2  2003/12/11 03:03:51  rpantos
Clean up mDNSPosix so that it builds on OS X again.

Revision 1.1  2003/12/08 20:47:02  rpantos
Add support for mDNSResponder on Linux.
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <sys/types.h>

#include "mDNSEmbeddedAPI.h"
#include "mDNSDebug.h"
#include "mDNSPosix.h"
#include "uds_daemon.h"
#include "PlatformCommon.h"

#define CONFIG_FILE "/etc/mdnsd.conf"
static domainname DynDNSZone;                // Default wide-area zone for service registration
static domainname DynDNSHostname;

#define RR_CACHE_SIZE 500
static CacheEntity gRRCache[RR_CACHE_SIZE];
static mDNS_PlatformSupport PlatformStorage;

extern const char mDNSResponderVersionString[];

mDNSlocal void mDNS_StatusCallback(mDNS *const m, mStatus result)
	{
	(void)m; // Unused
	if (result == mStatus_NoError)
		{
		// On successful registration of dot-local mDNS host name, daemon may want to check if
		// any name conflict and automatic renaming took place, and if so, record the newly negotiated
		// name in persistent storage for next time. It should also inform the user of the name change.
		// On Mac OS X we store the current dot-local mDNS host name in the SCPreferences store,
		// and notify the user with a CFUserNotification.
		}
	else if (result == mStatus_ConfigChanged)
		{
		udsserver_handle_configchange();
		}
	else if (result == mStatus_GrowCache)
		{
		// Allocate another chunk of cache storage
		CacheEntity *storage = malloc(sizeof(CacheEntity) * RR_CACHE_SIZE);
		if (storage) mDNS_GrowCache(m, storage, RR_CACHE_SIZE);
		}
	}

// %%% Reconfigure() probably belongs in the platform support layer (mDNSPosix.c), not the daemon cde
// -- all client layers running on top of mDNSPosix.c need to handle network configuration changes,
// not only the Unix Domain Socket Daemon

static void Reconfigure(mDNS *m)
	{
	mDNSAddr DynDNSIP;
	mDNS_SetPrimaryInterfaceInfo(m, NULL, NULL, NULL);
	mDNS_DeleteDNSServers(m);
	if (ParseDNSServers(m, uDNS_SERVERS_FILE) < 0)
		LogMsg("Unable to parse DNS server list. Unicast DNS-SD unavailable");
	ReadDDNSSettingsFromConfFile(m, CONFIG_FILE, &DynDNSHostname, &DynDNSZone, NULL);
	FindDefaultRouteIP(&DynDNSIP);
	if (DynDNSHostname.c[0]) mDNS_AddDynDNSHostName(m, &DynDNSHostname, NULL, NULL);
	if (DynDNSIP.type)       mDNS_SetPrimaryInterfaceInfo(m, &DynDNSIP, NULL, NULL);
	m->MainCallback(m, mStatus_ConfigChanged);
	}

// Do appropriate things at startup with command line arguments. Calls exit() if unhappy.
mDNSlocal void ParseCmdLinArgs(int argc, char **argv)
	{
	if (argc > 1)
		{
		if (0 == strcmp(argv[1], "-debug")) mDNS_DebugMode = mDNStrue;
		else printf("Usage: %s [-debug]\n", argv[0]);
		}

	if (!mDNS_DebugMode)
		{
		int result = daemon(0, 0);
		if (result != 0) { LogMsg("Could not run as daemon - exiting"); exit(result); }
#if __APPLE__
		LogMsg("The POSIX mdnsd should only be used on OS X for testing - exiting");
		exit(-1);
#endif
		}
	}

mDNSlocal void DumpStateLog(mDNS *const m)
// Dump a little log of what we've been up to.
	{
	LogMsgIdent(mDNSResponderVersionString, "---- BEGIN STATE LOG ----");
	udsserver_info(m);
	LogMsgIdent(mDNSResponderVersionString, "----  END STATE LOG  ----");
	}

mDNSlocal mStatus MainLoop(mDNS *m) // Loop until we quit.
	{
	sigset_t	signals;
	mDNSBool	gotData = mDNSfalse;

	mDNSPosixListenForSignalInEventLoop(SIGINT);
	mDNSPosixListenForSignalInEventLoop(SIGTERM);
	mDNSPosixListenForSignalInEventLoop(SIGUSR1);
	mDNSPosixListenForSignalInEventLoop(SIGPIPE);
	mDNSPosixListenForSignalInEventLoop(SIGHUP) ;

	for (; ;)
		{
		// Work out how long we expect to sleep before the next scheduled task
		struct timeval	timeout;
		mDNSs32			ticks;

		// Only idle if we didn't find any data the last time around
		if (!gotData)
			{
			mDNSs32			nextTimerEvent = mDNS_Execute(m);
			nextTimerEvent = udsserver_idle(nextTimerEvent);
			ticks = nextTimerEvent - mDNS_TimeNow(m);
			if (ticks < 1) ticks = 1;
			}
		else	// otherwise call EventLoop again with 0 timemout
			ticks = 0;

		timeout.tv_sec = ticks / mDNSPlatformOneSecond;
		timeout.tv_usec = (ticks % mDNSPlatformOneSecond) * 1000000 / mDNSPlatformOneSecond;

		(void) mDNSPosixRunEventLoopOnce(m, &timeout, &signals, &gotData);

		if (sigismember(&signals, SIGHUP )) Reconfigure(m);
		if (sigismember(&signals, SIGUSR1)) DumpStateLog(m);
		// SIGPIPE happens when we try to write to a dead client; death should be detected soon in request_callback() and cleaned up.
		if (sigismember(&signals, SIGPIPE)) LogMsg("Received SIGPIPE - ignoring");
		if (sigismember(&signals, SIGINT) || sigismember(&signals, SIGTERM)) break;
		}
	return EINTR;
	}

int main(int argc, char **argv)
	{
	mStatus					err;

	ParseCmdLinArgs(argc, argv);

	LogMsgIdent(mDNSResponderVersionString, "starting");

	err = mDNS_Init(&mDNSStorage, &PlatformStorage, gRRCache, RR_CACHE_SIZE, mDNS_Init_AdvertiseLocalAddresses, 
					mDNS_StatusCallback, mDNS_Init_NoInitCallbackContext); 

	if (mStatus_NoError == err)
		err = udsserver_init();
		
	Reconfigure(&mDNSStorage);

	// Now that we're finished with anything privileged, switch over to running as "nobody"
	if (mStatus_NoError == err)
		{
		const struct passwd *pw = getpwnam("nobody");
		if (pw != NULL)
			setuid(pw->pw_uid);
		else
			LogMsg("WARNING: mdnsd continuing as root because user \"nobody\" does not exist");
		}

	if (mStatus_NoError == err)
		err = MainLoop(&mDNSStorage);
 
	LogMsgIdent(mDNSResponderVersionString, "stopping");

	mDNS_Close(&mDNSStorage);

	if (udsserver_exit() < 0)
		LogMsg("ExitCallback: udsserver_exit failed");
 
 #if MDNS_DEBUGMSGS > 0
	printf("mDNSResponder exiting normally with %ld\n", err);
 #endif
 
	return err;
	}

//		uds_daemon support		////////////////////////////////////////////////////////////

mStatus udsSupportAddFDToEventLoop(int fd, udsEventCallback callback, void *context)
/* Support routine for uds_daemon.c */
	{
	// Depends on the fact that udsEventCallback == mDNSPosixEventCallback
	return mDNSPosixAddFDToEventLoop(fd, callback, context);
	}

mStatus udsSupportRemoveFDFromEventLoop(int fd)		// Note: This also CLOSES the file descriptor
	{
	mStatus err = mDNSPosixRemoveFDFromEventLoop(fd);
	close(fd);
	return err;
	}

mDNSexport void RecordUpdatedNiceLabel(mDNS *const m, mDNSs32 delay)
	{
	(void)m;
	(void)delay;
	// No-op, for now
	}

// For convenience when using the "strings" command, this is the last thing in the file
#if mDNSResponderVersion > 1
mDNSexport const char mDNSResponderVersionString[] = "mDNSResponder-" STRINGIFY(mDNSResponderVersion) " (" __DATE__ " " __TIME__ ") ";
#else
mDNSexport const char mDNSResponderVersionString[] = "mDNSResponder (Engineering Build) (" __DATE__ " " __TIME__ ") ";
#endif
