/*
 * Copyright (c) 2003 Apple Computer, Inc. All rights reserved.
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

 	File:		mDNSDebug.c

 	Contains:	Implementation of debugging utilities. Requires a POSIX environment.

 	Version:	1.0

    Change History (most recent first):

*/

#include "mDNSDebug.h"

#include <stdio.h>
#include <syslog.h>

#include "mDNSClientAPI.h"


static mDNSBool	gDebugLogging = mDNSfalse;

// Note, this uses mDNS_vsnprintf instead of standard "vsnprintf", because mDNS_vsnprintf knows
// how to print special data types like IP addresses and length-prefixed domain names
#if MDNS_DEBUGMSGS
mDNSexport void debugf_(const char *format, ...)
	{
	unsigned char buffer[512];
	va_list ptr;
	va_start(ptr,format);
	buffer[mDNS_vsnprintf((char *)buffer, sizeof(buffer), format, ptr)] = 0;
	va_end(ptr);
	fprintf(stderr,"%s\n", buffer);
	fflush(stderr);
	}
#endif

#if MDNS_DEBUGMSGS > 1
mDNSexport void verbosedebugf_(const char *format, ...)
	{
	unsigned char buffer[512];
	va_list ptr;
	va_start(ptr,format);
	buffer[mDNS_vsnprintf((char *)buffer, sizeof(buffer), format, ptr)] = 0;
	va_end(ptr);
	fprintf(stderr,"%s\n", buffer);
	fflush(stderr);
	}
#endif

mDNSlocal void WriteLogMsg(const char *ident, const char *buffer, int logoptflags)
	{
	if (gDebugLogging)		// In debug mode we write to stderr
		{
		fprintf(stderr,"%s\n", buffer);
		fflush(stderr);
		}
	else				// else, in production mode, we write to syslog
		{
		openlog(ident, LOG_CONS | LOG_PERROR | logoptflags, LOG_DAEMON);
		syslog(LOG_ERR, "%s", buffer);
		closelog();
		}
	}

mDNSexport void LogMsg(const char *format, ...)
	{
	unsigned char buffer[512];
	va_list ptr;
	va_start(ptr,format);
	buffer[mDNS_vsnprintf((char *)buffer, sizeof(buffer), format, ptr)] = 0;
	va_end(ptr);
	WriteLogMsg("mDNSResponder", buffer, 0);
	}

static void LogMsgWithIdent(const char *ident, const char *format, va_list ptr)
	{
	unsigned char buffer[512];
	buffer[mDNS_vsnprintf((char *)buffer, sizeof(buffer), format, ptr)] = 0;
	WriteLogMsg(ident, buffer, ident && *ident ? LOG_PID : 0);
	}

mDNSexport void LogMsgIdent(const char *ident, const char *format, ...)
	{
	va_list ptr;
	va_start(ptr,format);
	LogMsgWithIdent(ident, format, ptr);
	va_end(ptr);
	}

mDNSexport void LogMsgNoIdent(const char *format, ...)
	{
	va_list ptr;
	va_start(ptr,format);
	LogMsgWithIdent("", format, ptr);
	va_end(ptr);
	}

mDNSexport void	LogInDebugMode( void )
	{
	gDebugLogging = mDNStrue;
	}


