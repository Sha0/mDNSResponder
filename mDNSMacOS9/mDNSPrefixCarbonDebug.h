/*
 * Copyright (c) 2002 Apple Computer, Inc. All rights reserved.
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
 */

// Global options for the Mac OS Test Responder target.
// Symbols defined here are available within all source files, like symbols
// defined globally for a project using "-d SYMBOL=VALUE" in Unix Makefiles

// For normal DeferredTask time execution, set __ONLYSYSTEMTASK__ to 0
// For easier debugging, set __ONLYSYSTEMTASK__ to 1, and OT Notifier executions
// will be deferred until SystemTask time

#define TARGET_API_MAC_CARBON 1
#define OTCARBONAPPLICATION 1

#define __ONLYSYSTEMTASK__ 1
#define MDNS_DEBUGMSGS 1
