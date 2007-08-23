/* -*- Mode: C; tab-width: 4 -*-
 *
 * Copyright (c) 2002-2006 Apple Computer, Inc. All rights reserved.
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
 *
 * Formatting notes:
 * This code follows the "Whitesmiths style" C indentation rules. Plenty of discussion
 * on C indentation can be found on the web, such as <http://www.kafejo.com/komp/1tbs.htm>,
 * but for the sake of brevity here I will say just this: Curly braces are not syntactially
 * part of an "if" statement; they are the beginning and ending markers of a compound statement;
 * therefore common sense dictates that if they are part of a compound statement then they
 * should be indented to the same level as everything else in that compound statement.
 * Indenting curly braces at the same level as the "if" implies that curly braces are
 * part of the "if", which is false. (This is as misleading as people who write "char* x,y;"
 * thinking that variables x and y are both of type "char*" -- and anyone who doesn't
 * understand why variable y is not of type "char*" just proves the point that poor code
 * layout leads people to unfortunate misunderstandings about how the C language really works.)

    Change History (most recent first):

$Log: daemon.c,v $
Revision 1.333  2007/08/23 21:02:35  cheshire
SecKeychainSetPreferenceDomain() call should be in platform-support layer, not daemon.c

Revision 1.332  2007/08/22 23:54:54  mcguire
<rdar://problem/5422558> BTMM: mDNSResponder should be able to run from the cmdline

Revision 1.331  2007/08/18 01:02:03  mcguire
<rdar://problem/5415593> No Bonjour services are getting registered at boot

Revision 1.330  2007/08/10 22:25:57  mkrochma
<rdar://problem/5396302> mDNSResponder continually complains about slow UDP packet reception -- about 400 msecs

Revision 1.329  2007/08/08 22:34:58  mcguire
<rdar://problem/5197869> Security: Run mDNSResponder as user id mdnsresponder instead of root

Revision 1.328  2007/07/27 22:43:37  cheshire
Improved mallocL/freeL "suspiciously large" debugging messages

Revision 1.327  2007/07/27 19:30:41  cheshire
Changed mDNSQuestionCallback parameter from mDNSBool to QC_result,
to properly reflect tri-state nature of the possible responses

Revision 1.326  2007/07/24 17:23:33  cheshire
<rdar://problem/5357133> Add list validation checks for debugging

Revision 1.325  2007/07/11 23:43:43  cheshire
Rename PurgeCacheResourceRecord to mDNS_PurgeCacheResourceRecord

Revision 1.324  2007/07/11 22:44:40  cheshire
<rdar://problem/5328801> SIGHUP should purge the cache

Revision 1.323  2007/07/11 03:01:50  cheshire
<rdar://problem/5303807> Register IPv6-only hostname and don't create port mappings for AutoTunnel services

Revision 1.322  2007/07/06 18:58:16  cheshire
Check m->NextScheduledNATOp in ShowTaskSchedulingError()

Revision 1.321  2007/07/02 21:54:20  cheshire
Fix compile error in MACOSX_MDNS_MALLOC_DEBUGGING checks

Revision 1.320  2007/06/28 21:16:27  cheshire
Rename "m->nextevent" as more informative "m->NextuDNSEvent"

Revision 1.319  2007/06/22 20:47:08  cheshire
<rdar://problem/5285417> DOS charset changes from CP932 to CP850 after Computer Name conflict
Made a "SafeSCPreferencesSetComputerName" routine to set the Computer Name without changing the machine's default character set

Revision 1.318  2007/06/20 01:45:40  cheshire
When showing dormant interfaces, display last-seen IP address for that dormant interface

Revision 1.317  2007/06/20 01:10:12  cheshire
<rdar://problem/5280520> Sync iPhone changes into main mDNSResponder code

Revision 1.316  2007/06/19 19:27:11  cheshire
<rdar://problem/5141540> Sandbox mDNSResponder
Weak-link sandbox_init, so mDNSResponder can be run on Tiger for regression testing

Revision 1.315  2007/06/15 21:54:51  cheshire
<rdar://problem/4883206> Add packet logging to help debugging private browsing over TLS

Revision 1.314  2007/06/15 19:23:17  cheshire
<rdar://problem/5254053> mDNSResponder renames my host without asking
Improve log messages, to distinguish user-initiated renames from automatic (name conflict) renames

Revision 1.313  2007/05/25 16:02:05  cheshire
When MACOSX_MDNS_MALLOC_DEBUGGING is enabled, log suspiciously large memory allocations

Revision 1.312  2007/05/22 19:07:21  cheshire
Add comment explaining RR_CACHE_SIZE calculation

Revision 1.311  2007/05/15 21:47:21  cheshire
Get rid of "#pragma unused(m)"

Revision 1.310  2007/05/08 00:56:17  cheshire
<rdar://problem/4118503> Share single socket instead of creating separate socket for each active interface

Revision 1.309  2007/04/30 21:33:38  cheshire
Fix crash when a callback unregisters a service while the UpdateSRVRecords() loop
is iterating through the m->ServiceRegistrations list

Revision 1.308  2007/04/28 01:31:59  cheshire
Improve debugging support for catching memory corruption problems

Revision 1.307  2007/04/24 18:32:00  cheshire
Grab a copy of KQtask string pointer in case KQcallback deletes the task

Revision 1.306  2007/04/22 19:11:51  cheshire
Some quirk of RemoveFromList (GenLinkedList.c) was corrupting the list and causing a crash
if the element being removed was not the last in the list. Fixed by removing GenLinkedList.c
from the project and just using simple vanilla C linked-list manipulation instead.

Revision 1.305  2007/04/22 06:02:03  cheshire
<rdar://problem/4615977> Query should immediately return failure when no server

Revision 1.304  2007/04/21 21:47:47  cheshire
<rdar://problem/4376383> Daemon: Add watchdog timer

Revision 1.303  2007/04/18 00:50:47  cheshire
<rdar://problem/5141540> Sandbox mDNSResponder

Revision 1.302  2007/04/07 01:01:48  cheshire
<rdar://problem/5095167> mDNSResponder periodically blocks in SSLRead

Revision 1.301  2007/04/05 19:13:48  cheshire
Use better name in SCPreferencesCreate

Revision 1.300  2007/04/04 21:22:18  cheshire
Suppress "Local Hostname changed" syslog message when name has not actually changed

Revision 1.299  2007/04/03 19:19:33  cheshire
Use mDNSIPPortIsZero() instead of peeking into 'NotAnInteger' field

Revision 1.298  2007/03/30 21:51:45  cheshire
Minor code tidying

Revision 1.297  2007/03/27 22:47:19  cheshire
On memory corruption, if ForceAlerts is set, force a crash to get a stack trace

Revision 1.296  2007/03/24 01:23:29  cheshire
Call validator for uDNS data structures

Revision 1.295  2007/03/20 23:32:49  cheshire
Minor textual tidying

Revision 1.294  2007/03/07 02:50:50  cheshire
<rdar://problem/4574528> Name conflict dialog doesn't appear if Bonjour is persistantly unable to find an available hostname

Revision 1.293  2007/03/06 22:59:01  cheshire
<rdar://problem/4157921> Security: Null dereference possible in daemon.c

Revision 1.292  2007/02/28 21:55:10  cheshire
<rdar://problem/3862944> UI: Name conflict notifications should be localized
Additional fix: We were not getting our NotificationCallBackDismissed messages
because we were scheduling our CFUserNotification RunLoopSource on the wrong runloop.
(We were incorrectly assuming CFRunLoopGetCurrent() would be the right runloop.)

Revision 1.291  2007/02/28 03:51:24  cheshire
<rdar://problem/3862944> UI: Name conflict notifications should be localized
Moved curly quotes out of the literal text and into the localized text, so they
can be replaced with alternate characters as appropriate for other languages.

Revision 1.290  2007/02/14 01:58:19  cheshire
<rdar://problem/4995831> Don't delete Unix Domain Socket on exit if we didn't create it on startup

Revision 1.289  2007/02/07 19:32:00  cheshire
<rdar://problem/4980353> All mDNSResponder components should contain version strings in SCCS-compatible format

Revision 1.288  2007/02/07 01:01:24  cheshire
<rdar://problem/3956518> Need to go native with launchd
Additional refinements -- was unnecessarily calling launch_data_free()

Revision 1.287  2007/02/06 19:06:48  cheshire
<rdar://problem/3956518> Need to go native with launchd

Revision 1.286  2007/01/06 01:00:33  cheshire
Improved SIGINFO output

Revision 1.285  2007/01/05 08:30:47  cheshire
Trim excessive "$Log" checkin history from before 2006
(checkin history still available via "cvs log ..." of course)

Revision 1.284  2007/01/05 05:44:35  cheshire
Move automatic browse/registration management from uDNS.c to mDNSShared/uds_daemon.c,
so that mDNSPosix embedded clients will compile again

Revision 1.283  2007/01/04 23:11:14  cheshire
<rdar://problem/4720673> uDNS: Need to start caching unicast records
When an automatic browsing domain is removed, generate appropriate "remove" events for legacy queries

Revision 1.282  2006/12/21 00:09:45  cheshire
Use mDNSPlatformMemZero instead of bzero

Revision 1.281  2006/11/18 05:01:32  cheshire
Preliminary support for unifying the uDNS and mDNS code,
including caching of uDNS answers

Revision 1.280  2006/11/10 00:54:16  cheshire
<rdar://problem/4816598> Changing case of Computer Name doesn't work

Revision 1.279  2006/11/02 17:44:01  cheshire
No longer have a separate uDNS ActiveQueries list

Revision 1.278  2006/10/05 04:04:24  herscher
Remove embedded uDNS_info struct from DNSQuestion_struct

Revision 1.277  2006/09/21 21:01:24  cheshire
Change 'autorename' to more accurate name 'renameonmemfree'

Revision 1.276  2006/09/17 19:12:02  cheshire
Further changes for removal of uDNS_info substructure from mDNS_struct

Revision 1.275  2006/09/15 21:20:16  cheshire
Remove uDNS_info substructure from mDNS_struct

Revision 1.274  2006/08/14 23:24:39  cheshire
Re-licensed mDNSResponder daemon source code under Apache License, Version 2.0

Revision 1.273  2006/07/30 05:43:19  cheshire
<rdar://problem/4049048> Convert mDNSResponder to use kqueue
Problems using KQueueFD with select() -- for now we'll stick to pure kevent()

Revision 1.272  2006/07/27 03:24:35  cheshire
<rdar://problem/4049048> Convert mDNSResponder to use kqueue
Further refinement: Declare KQueueEntry parameter "const"

Revision 1.271  2006/07/27 02:59:26  cheshire
<rdar://problem/4049048> Convert mDNSResponder to use kqueue
Further refinements: CFRunLoop thread needs to explicitly wake the kqueue thread
after releasing BigMutex, in case actions it took have resulted in new work for the
kqueue thread (e.g. NetworkChanged events may result in the kqueue thread having to
add new active interfaces to its list, and consequently schedule queries to be sent).

Revision 1.270  2006/07/25 17:16:36  mkrochma
Quick fix to solve kqueue related crashes and hangs

Revision 1.269  2006/07/22 06:11:37  cheshire
<rdar://problem/4049048> Convert mDNSResponder to use kqueue

Revision 1.268  2006/07/15 02:01:32  cheshire
<rdar://problem/4472014> Add Private DNS client functionality to mDNSResponder
Fix broken "empty string" browsing

Revision 1.267  2006/07/07 01:09:10  cheshire
<rdar://problem/4472013> Add Private DNS server functionality to dnsextd
Only use mallocL/freeL debugging routines when building mDNSResponder, not dnsextd

Revision 1.266  2006/07/05 23:34:53  cheshire
<rdar://problem/4472014> Add Private DNS client functionality to mDNSResponder

Revision 1.265  2006/06/29 07:32:08  cheshire
Added missing LogOperation logging for DNSServiceBrowse results

Revision 1.264  2006/06/29 05:33:30  cheshire
<rdar://problem/4607043> mDNSResponder conditional compilation options

Revision 1.263  2006/06/08 23:23:48  cheshire
Fix errant indentation of curly brace at the end of provide_DNSServiceBrowserCreate_rpc()

Revision 1.262  2006/03/18 21:49:11  cheshire
Added comment in ShowTaskSchedulingError(mDNS *const m)

Revision 1.261  2006/01/06 01:22:28  cheshire
<rdar://problem/4108164> Reword "mach_absolute_time went backwards" dialog

*/

#include <mach/mach.h>
#include <mach/mach_error.h>
#include <servers/bootstrap.h>
#include <sys/types.h>
#include <unistd.h>
#include <paths.h>
#include <fcntl.h>
#include <launch.h>
#include <pwd.h>
#include <sys/event.h>
#include <pthread.h>
#include <sandbox.h>
#include <SystemConfiguration/SCPreferencesSetSpecific.h>

#include "DNSServiceDiscoveryRequestServer.h"
#include "DNSServiceDiscoveryReply.h"

#include "uDNS.h"
#include "DNSCommon.h"
#include "mDNSMacOSX.h"				// Defines the specific types needed to run mDNS on this platform

#include "uds_daemon.h"				// Interface to the server side implementation of dns_sd.h

#include <DNSServiceDiscovery/DNSServiceDiscovery.h>
#include "helper.h"

//*************************************************************************************************************
// Macros

// Note: The C preprocessor stringify operator ('#') makes a string from its argument, without macro expansion
// e.g. If "version" is #define'd to be "4", then STRINGIFY_AWE(version) will return the string "version", not "4"
// To expand "version" to its value before making the string, use STRINGIFY(version) instead
#define STRINGIFY_ARGUMENT_WITHOUT_EXPANSION(s) #s
#define STRINGIFY(s) STRINGIFY_ARGUMENT_WITHOUT_EXPANSION(s)

//*************************************************************************************************************
// Globals

static mDNS_PlatformSupport PlatformStorage;

// Start off with a default cache of 16K (99 records)
// Each time we grow the cache we add another 99 records
// 99 * 164 = 16236 bytes.
// This fits in four 4kB pages, with 148 bytes spare for memory block headers and similar overhead
#define RR_CACHE_SIZE ((16*1024) / sizeof(CacheRecord))
static CacheEntity rrcachestorage[RR_CACHE_SIZE];

static const char kmDNSBootstrapName[] = "com.apple.mDNSResponderRestart";
static mach_port_t m_port            = MACH_PORT_NULL;
static mach_port_t client_death_port = MACH_PORT_NULL;
static mach_port_t signal_port       = MACH_PORT_NULL;
static mach_port_t server_priv_port  = MACH_PORT_NULL;

static dnssd_sock_t launchd_fd = dnssd_InvalidSocket;

// mDNS Mach Message Timeout, in milliseconds.
// We need this to be short enough that we don't deadlock the mDNSResponder if a client
// fails to service its mach message queue, but long enough to give a well-written
// client a chance to service its mach message queue without getting cut off.
// Empirically, 50ms seems to work, so we set the timeout to 250ms to give
// even extra-slow clients a fair chance before we cut them off.
#define MDNS_MM_TIMEOUT 250

static int restarting_via_mach_init = 0;	// Used on Jaguar/Panther when daemon is started via mach_init mechanism
static int started_via_launchdaemon = 0;	// Indicates we're running on Tiger or later, where daemon is managed by launchd

static int OSXVers;

static CFRunLoopRef CFRunLoop;

//*************************************************************************************************************
// Active client list structures

typedef struct DNSServiceDomainEnumeration_struct DNSServiceDomainEnumeration;
struct DNSServiceDomainEnumeration_struct
	{
	DNSServiceDomainEnumeration *next;
	mach_port_t ClientMachPort;
	DNSQuestion dom;	// Question asking for domains
	DNSQuestion def;	// Question asking for default domain
	};

typedef struct DNSServiceBrowserResult_struct DNSServiceBrowserResult;
struct DNSServiceBrowserResult_struct
	{
	DNSServiceBrowserResult *next;
	int resultType;
	domainname result;
	};

typedef struct DNSServiceBrowser_struct DNSServiceBrowser;

typedef struct DNSServiceBrowserQuestion
	{
	struct DNSServiceBrowserQuestion *next;
	DNSQuestion q;
    domainname domain;
	} DNSServiceBrowserQuestion;

struct DNSServiceBrowser_struct
	{
	DNSServiceBrowser *next;
	mach_port_t ClientMachPort;
	DNSServiceBrowserQuestion *qlist;
	DNSServiceBrowserResult *results;
	mDNSs32 lastsuccess;
    mDNSBool DefaultDomain;                // was the browse started on an explicit domain?
    domainname type;                       //  registration type 
	};

typedef struct DNSServiceResolver_struct DNSServiceResolver;
struct DNSServiceResolver_struct
	{
	DNSServiceResolver *next;
	mach_port_t ClientMachPort;
	ServiceInfoQuery q;
	ServiceInfo      i;
	mDNSs32          ReportTime;
	};

// A single registered service: ServiceRecordSet + bookkeeping
// Note that we duplicate some fields from parent DNSServiceRegistration object
// to facilitate cleanup, when instances and parent may be deallocated at different times.
typedef struct ServiceInstance
	{
    struct ServiceInstance *next;
	mach_port_t ClientMachPort;
    mDNSBool autoname;			// Set if this name is tied to the Computer Name
    mDNSBool renameonmemfree;	// Set if we just got a name conflict and now need to automatically pick a new name
    domainlabel name;
    domainname domain;
    ServiceRecordSet srs;
	// Don't add any fields after ServiceRecordSet.
	// This is where the implicit extra space goes if we allocate an oversized ServiceRecordSet object
	} ServiceInstance;

// A client-created service.  May reference several ServiceInstance objects if default
// settings cause registration in multiple domains.
typedef struct DNSServiceRegistration
	{
    struct DNSServiceRegistration *next;
	mach_port_t ClientMachPort;
    mDNSBool DefaultDomain;
    mDNSBool autoname;
    size_t rdsize;
    int NumSubTypes;
    char regtype[MAX_ESCAPED_DOMAIN_NAME]; // for use in AllocateSubtypes
    domainlabel name;  // used only if autoname is false 
    domainname type;
    mDNSIPPort port;
    unsigned char txtinfo[1024];
    size_t txt_len;
    uint32_t NextRef;
    ServiceInstance *regs;
	} DNSServiceRegistration;

static DNSServiceDomainEnumeration *DNSServiceDomainEnumerationList = NULL;
static DNSServiceBrowser           *DNSServiceBrowserList           = NULL;
static DNSServiceResolver          *DNSServiceResolverList          = NULL;
static DNSServiceRegistration      *DNSServiceRegistrationList      = NULL;

// We keep a list of client-supplied event sources in KQSocketEventSource records
typedef struct KQSocketEventSource
	{
	struct  KQSocketEventSource *next;
	int                         fd;
	KQueueEntry                 kqs;
	} KQSocketEventSource;

static KQSocketEventSource *gEventSources;

//*************************************************************************************************************
// General Utility Functions

#if APPLE_OSX_mDNSResponder && MACOSX_MDNS_MALLOC_DEBUGGING

char _malloc_options[] = "AXZ";

mDNSexport void LogMemCorruption(const char *format, ...)
	{
	char buffer[512];
	va_list ptr;
	va_start(ptr,format);
	buffer[mDNS_vsnprintf((char *)buffer, sizeof(buffer), format, ptr)] = 0;
	va_end(ptr);
	LogMsg("!!!! %s !!!!", buffer);
	NotifyOfElusiveBug("Memory Corruption", buffer);
#if ForceAlerts
	*(long*)0 = 0;	// Trick to crash and get a stack trace right here, if that's what we want
#endif
	}

mDNSlocal void validatelists(mDNS *const m)
	{
	// Check local lists
	KQSocketEventSource *k;
	for (k = gEventSources; k; k=k->next)
		if (k->fd < 0)
			LogMemCorruption("gEventSources: %p is garbage (%d)", k, k->fd);

	// Check Mach client lists
	DNSServiceDomainEnumeration *e;
	for (e = DNSServiceDomainEnumerationList; e; e=e->next)
		if (e->ClientMachPort == 0 || e->ClientMachPort == (mach_port_t)~0)
			LogMemCorruption("DNSServiceDomainEnumerationList: %p is garbage (%X)", e, e->ClientMachPort);

	DNSServiceBrowser           *b;
	for (b = DNSServiceBrowserList; b; b=b->next)
		if (b->ClientMachPort == 0 || b->ClientMachPort == (mach_port_t)~0)
			LogMemCorruption("DNSServiceBrowserList: %p is garbage (%X)", b, b->ClientMachPort);

	DNSServiceResolver          *l;
	for (l = DNSServiceResolverList; l; l=l->next)
		if (l->ClientMachPort == 0 || l->ClientMachPort == (mach_port_t)~0)
			LogMemCorruption("DNSServiceResolverList: %p is garbage (%X)", l, l->ClientMachPort);

	DNSServiceRegistration      *r;
	for (r = DNSServiceRegistrationList; r; r=r->next)
		if (r->ClientMachPort == 0 || r->ClientMachPort == (mach_port_t)~0)
			LogMemCorruption("DNSServiceRegistrationList: %p is garbage (%X)", r, r->ClientMachPort);

	// Check Unix Domain Socket client lists (uds_daemon.c)
	uds_validatelists();

	// Check core mDNS lists
	AuthRecord                  *rr;
	for (rr = m->ResourceRecords; rr; rr=rr->next)
		{
		if (rr->resrec.RecordType == 0 || rr->resrec.RecordType == 0xFF)
			LogMemCorruption("ResourceRecords list: %p is garbage (%X)", rr, rr->resrec.RecordType);
		if (rr->resrec.name != &rr->namestorage)
			LogMemCorruption("ResourceRecords list: %p name %p does not point to namestorage %p %##s",
				rr, rr->resrec.name->c, rr->namestorage.c, rr->namestorage.c);
		}

	for (rr = m->DuplicateRecords; rr; rr=rr->next)
		if (rr->resrec.RecordType == 0 || rr->resrec.RecordType == 0xFF)
			LogMemCorruption("DuplicateRecords list: %p is garbage (%X)", rr, rr->resrec.RecordType);

	DNSQuestion                 *q;
	for (q = m->Questions; q; q=q->next)
		if (q->ThisQInterval == (mDNSs32)~0 || q->next == (DNSQuestion*)~0)
			LogMemCorruption("Questions list: %p is garbage (%lX %p)", q, q->ThisQInterval, q->next);

	CacheGroup                  *cg;
	CacheRecord                 *cr;
	mDNSu32 slot;
	FORALL_CACHERECORDS(slot, cg, cr)
		{
		if (cr->resrec.RecordType == 0 || cr->resrec.RecordType == 0xFF)
			LogMemCorruption("Cache slot %lu: %p is garbage (%X)", slot, cr, cr->resrec.RecordType);
		if (cr->CRActiveQuestion)
			{
			for (q = m->Questions; q; q=q->next) if (q == cr->CRActiveQuestion) break;
			if (!q) LogMemCorruption("Cache slot %lu: CRActiveQuestion %p not in m->Questions list %s", slot, cr->CRActiveQuestion, CRDisplayString(m, cr));
			}
		}

	// Check core uDNS lists
	udns_validatelists(m);

	// Check platform-layer lists
	NetworkInterfaceInfoOSX     *i;
	for (i = m->p->InterfaceList; i; i = i->next)
		if (!i->ifa_name || i->ifa_name == (char *)~0)
			LogMemCorruption("m->p->InterfaceList: %p is garbage (%p)", i, i->ifa_name);

	ClientTunnel *t;
	for (t = m->TunnelClients; t; t=t->next)
		if (t->dstname.c[0] > 63)
			LogMemCorruption("m->TunnelClients: %p is garbage (%d)", t, t->dstname.c[0]);
	}

void *mallocL(char *msg, unsigned int size)
	{
	unsigned long *mem = malloc(size+8);
	if (!mem)
		{
		LogMsg("malloc( %s : %d ) failed", msg, size);
		return(NULL);
		}
	else
		{
		if      (size > 24000)                      LogMsg("malloc( %s : %lu ) = %p suspiciously large", msg, size, &mem[2]);
		else if (MACOSX_MDNS_MALLOC_DEBUGGING >= 2) LogMsg("malloc( %s : %lu ) = %p",                    msg, size, &mem[2]);
		mem[0] = 0xDEAD1234;
		mem[1] = size;
		//mDNSPlatformMemZero(&mem[2], size);
		memset(&mem[2], 0xFF, size);
		validatelists(&mDNSStorage);
		return(&mem[2]);
		}
	}

void freeL(char *msg, void *x)
	{
	if (!x)
		LogMsg("free( %s @ NULL )!", msg);
	else
		{
		unsigned long *mem = ((unsigned long *)x) - 2;
		if (mem[0] != 0xDEAD1234) { LogMsg("free( %s @ %p ) !!!! NOT ALLOCATED !!!!", msg, &mem[2]); return; }
		if      (mem[1] > 24000)                    LogMsg("free( %s : %ld @ %p) suspiciously large", msg, mem[1], &mem[2]);
		else if (MACOSX_MDNS_MALLOC_DEBUGGING >= 2) LogMsg("free( %s : %ld @ %p)",                    msg, mem[1], &mem[2]);
		//mDNSPlatformMemZero(mem, mem[1]+8);
		memset(mem, 0xFF, mem[1]+8);
		validatelists(&mDNSStorage);
		free(mem);
		}
	}

#endif

//*************************************************************************************************************
// Client Death Detection

mDNSlocal void FreeServiceInstance(ServiceInstance *x)
	{
	ServiceRecordSet *s = &x->srs;
	ExtraResourceRecord *e = x->srs.Extras, *tmp;
	
	while (e)
		{
		e->r.RecordContext = e;
		tmp = e;
		e = e->next;
		FreeExtraRR(&mDNSStorage, &tmp->r, mStatus_MemFree);
		}
	
	if (s->RR_TXT.resrec.rdata != &s->RR_TXT.rdatastorage)
		freeL("TXT RData", s->RR_TXT.resrec.rdata);

	if (s->SubTypes) freeL("ServiceSubTypes", s->SubTypes);
	freeL("ServiceInstance", x);
	}

// AbortClient finds whatever client is identified by the given Mach port,
// stops whatever operation that client was doing, and frees its memory.
// In the case of a service registration, the actual freeing may be deferred
// until we get the mStatus_MemFree message, if necessary
mDNSlocal void AbortClient(mach_port_t ClientMachPort, void *m)
	{
	DNSServiceDomainEnumeration **e = &DNSServiceDomainEnumerationList;
	DNSServiceBrowser           **b = &DNSServiceBrowserList;
	DNSServiceResolver          **l = &DNSServiceResolverList;
	DNSServiceRegistration      **r = &DNSServiceRegistrationList;

	while (*e && (*e)->ClientMachPort != ClientMachPort) e = &(*e)->next;
	if (*e)
		{
		DNSServiceDomainEnumeration *x = *e;
		*e = (*e)->next;
		if (m && m != x)
			LogMsg("%5d: DNSServiceDomainEnumeration(%##s) STOP; WARNING m %p != x %p", ClientMachPort, x->dom.qname.c, m, x);
		else LogOperation("%5d: DNSServiceDomainEnumeration(%##s) STOP", ClientMachPort, x->dom.qname.c);
		mDNS_StopGetDomains(&mDNSStorage, &x->dom);
		mDNS_StopGetDomains(&mDNSStorage, &x->def);
		freeL("DNSServiceDomainEnumeration", x);
		return;
		}

	while (*b && (*b)->ClientMachPort != ClientMachPort) b = &(*b)->next;
	if (*b)
		{
		DNSServiceBrowser *x = *b;
		DNSServiceBrowserQuestion *freePtr, *qptr = x->qlist;
		*b = (*b)->next;
		while (qptr)
			{
			if (m && m != x)
				LogMsg("%5d: DNSServiceBrowse(%##s) STOP; WARNING m %p != x %p", ClientMachPort, qptr->q.qname.c, m, x);
			else LogOperation("%5d: DNSServiceBrowse(%##s) STOP", ClientMachPort, qptr->q.qname.c);
			mDNS_StopBrowse(&mDNSStorage, &qptr->q);
			freePtr = qptr;
			qptr = qptr->next;
			freeL("DNSServiceBrowserQuestion", freePtr);
			}
		while (x->results)
			{
			DNSServiceBrowserResult *r = x->results;
			x->results = x->results->next;
			freeL("DNSServiceBrowserResult", r);
			}
		freeL("DNSServiceBrowser", x);
		return;
		}

	while (*l && (*l)->ClientMachPort != ClientMachPort) l = &(*l)->next;
	if (*l)
		{
		DNSServiceResolver *x = *l;
		*l = (*l)->next;
		if (m && m != x)
			LogMsg("%5d: DNSServiceResolve(%##s) STOP; WARNING m %p != x %p", ClientMachPort, x->i.name.c, m, x);
		else LogOperation("%5d: DNSServiceResolve(%##s) STOP", ClientMachPort, x->i.name.c);
		mDNS_StopResolveService(&mDNSStorage, &x->q);
		freeL("DNSServiceResolver", x);
		return;
		}

	while (*r && (*r)->ClientMachPort != ClientMachPort) r = &(*r)->next;
	if (*r)
		{
		ServiceInstance *si = NULL;
		DNSServiceRegistration *x = *r;
		*r = (*r)->next;

		si = x->regs;
		while (si)
			{
			ServiceInstance *instance = si;
			si = si->next;                 
			instance->renameonmemfree = mDNSfalse;
			if (m && m != x) LogMsg("%5d: DNSServiceRegistration(%##s, %u) STOP; WARNING m %p != x %p", ClientMachPort, instance->srs.RR_SRV.resrec.name->c, SRS_PORT(&instance->srs), m, x);
			else LogOperation("%5d: DNSServiceRegistration(%##s, %u) STOP", ClientMachPort, instance->srs.RR_SRV.resrec.name->c, SRS_PORT(&instance->srs));

			// If mDNS_DeregisterService() returns mStatus_NoError, that means that the service was found in the list,
			// is sending its goodbye packet, and we'll get an mStatus_MemFree message when we can free the memory.
			// If mDNS_DeregisterService() returns an error, it means that the service had already been removed from
			// the list, so we should go ahead and free the memory right now
			if (mDNS_DeregisterService(&mDNSStorage, &instance->srs)) FreeServiceInstance(instance); // FreeServiceInstance invalidates pointer
			}
		x->regs = NULL;
		freeL("DNSServiceRegistration", x);
		return;
		}

	LogMsg("%5d: died or deallocated, but no record of client can be found!", ClientMachPort);
	}

#define AbortBlockedClient(C,MSG,M) AbortClientWithLogMessage((C), "stopped accepting Mach messages", " (" MSG ")", (M))

mDNSlocal void AbortClientWithLogMessage(mach_port_t c, char *reason, char *msg, void *m)
	{
	DNSServiceDomainEnumeration *e = DNSServiceDomainEnumerationList;
	DNSServiceBrowser           *b = DNSServiceBrowserList;
	DNSServiceResolver          *l = DNSServiceResolverList;
	DNSServiceRegistration      *r = DNSServiceRegistrationList;
	DNSServiceBrowserQuestion   *qptr;

	while (e && e->ClientMachPort != c) e = e->next;
	while (b && b->ClientMachPort != c) b = b->next;
	while (l && l->ClientMachPort != c) l = l->next;
	while (r && r->ClientMachPort != c) r = r->next;

	if      (e) LogMsg("%5d: DomainEnumeration(%##s) %s%s",                   c, e->dom.qname.c,                reason, msg);
	else if (b)
			{
			for (qptr = b->qlist; qptr; qptr = qptr->next)
				LogMsg("%5d: Browser(%##s) %s%s",                             c, qptr->q.qname.c,               reason, msg);
			}
	else if (l) LogMsg("%5d: Resolver(%##s) %s%s",                            c, l->i.name.c,                   reason, msg);
	else if (r)
			{
			ServiceInstance *si;
			for (si = r->regs; si; si = si->next)
				LogMsg("%5d: Registration(%##s) %s%s",                        c, si->srs.RR_SRV.resrec.name->c, reason, msg);
			}
	else        LogMsg("%5d: (%s) %s, but no record of client can be found!", c,                                reason, msg);

	AbortClient(c, m);
	}

mDNSlocal mDNSBool CheckForExistingClient(mach_port_t c)
	{
	DNSServiceDomainEnumeration *e = DNSServiceDomainEnumerationList;
	DNSServiceBrowser           *b = DNSServiceBrowserList;
	DNSServiceResolver          *l = DNSServiceResolverList;
	DNSServiceRegistration      *r = DNSServiceRegistrationList;
	DNSServiceBrowserQuestion   *qptr;

	while (e && e->ClientMachPort != c) e = e->next;
	while (b && b->ClientMachPort != c) b = b->next;
	while (l && l->ClientMachPort != c) l = l->next;
	while (r && r->ClientMachPort != c) r = r->next;
	if (e) LogMsg("%5d: DomainEnumeration(%##s) already exists!", c, e->dom.qname.c);
	if (b)
		{
		for (qptr = b->qlist; qptr; qptr = qptr->next)
			LogMsg("%5d: Browser(%##s) already exists!",          c, qptr->q.qname.c);
		}
	if (l) LogMsg("%5d: Resolver(%##s) already exists!",          c, l->i.name.c);
	if (r) LogMsg("%5d: Registration(%##s) already exists!",      c, r->regs ? r->regs->srs.RR_SRV.resrec.name->c : NULL);
	return(e || b || l || r);
	}

mDNSlocal void ClientDeathCallback(CFMachPortRef unusedport, void *voidmsg, CFIndex size, void *info)
	{
	KQueueLock(&mDNSStorage);
	mach_msg_header_t *msg = (mach_msg_header_t *)voidmsg;
	(void)unusedport; // Unused
	(void)size; // Unused
	(void)info; // Unused
	if (msg->msgh_id == MACH_NOTIFY_DEAD_NAME)
		{
		const mach_dead_name_notification_t *const deathMessage = (mach_dead_name_notification_t *)msg;
		AbortClient(deathMessage->not_port, NULL);

		/* Deallocate the send right that came in the dead name notification */
		mach_port_destroy(mach_task_self(), deathMessage->not_port);
		}
	KQueueUnlock(&mDNSStorage, "Mach AbortClient");
	}

mDNSlocal void EnableDeathNotificationForClient(mach_port_t ClientMachPort, void *m)
	{
	mach_port_t prev;
	kern_return_t r = mach_port_request_notification(mach_task_self(), ClientMachPort, MACH_NOTIFY_DEAD_NAME, 0,
													 client_death_port, MACH_MSG_TYPE_MAKE_SEND_ONCE, &prev);
	// If the port already died while we were thinking about it, then abort the operation right away
	if (r != KERN_SUCCESS)
		AbortClientWithLogMessage(ClientMachPort, "died/deallocated before we could enable death notification", "", m);
	}

//*************************************************************************************************************
// Domain Enumeration

mDNSlocal void DomainEnumFound(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, QC_result AddRecord)
	{
	kern_return_t status;
	char buffer[MAX_ESCAPED_DOMAIN_NAME];
	DNSServiceDomainEnumerationReplyResultType rt;
	DNSServiceDomainEnumeration *x = (DNSServiceDomainEnumeration *)question->QuestionContext;
	(void)m; // Unused

	debugf("DomainEnumFound: %##s PTR %##s", answer->name->c, answer->rdata->u.name.c);
	if (answer->rrtype != kDNSType_PTR) return;
	if (!x) { debugf("DomainEnumFound: DNSServiceDomainEnumeration is NULL"); return; }

	if (AddRecord)
		{
		if (question == &x->dom) rt = DNSServiceDomainEnumerationReplyAddDomain;
		else                     rt = DNSServiceDomainEnumerationReplyAddDomainDefault;
		}
	else
		{
		if (question == &x->dom) rt = DNSServiceDomainEnumerationReplyRemoveDomain;
		else return;
		}

	LogOperation("%5d: DNSServiceDomainEnumeration(%##s) %##s %s",
		x->ClientMachPort, x->dom.qname.c, answer->rdata->u.name.c,
		!AddRecord ? "RemoveDomain" :
		question == &x->dom ? "AddDomain" : "AddDomainDefault");

	ConvertDomainNameToCString(&answer->rdata->u.name, buffer);
	status = DNSServiceDomainEnumerationReply_rpc(x->ClientMachPort, rt, buffer, 0, MDNS_MM_TIMEOUT);
	if (status == MACH_SEND_TIMED_OUT)
		AbortBlockedClient(x->ClientMachPort, "enumeration", x);
	}

mDNSexport kern_return_t provide_DNSServiceDomainEnumerationCreate_rpc(mach_port_t unusedserver, mach_port_t client,
	int regDom)
	{
	// Check client parameter
	(void)unusedserver; // Unused
	mStatus err = mStatus_NoError;
	const char *errormsg = "Unknown";
	if (client == (mach_port_t)-1)      { err = mStatus_Invalid; errormsg = "Client id -1 invalid";     goto fail; }
	if (CheckForExistingClient(client)) { err = mStatus_Invalid; errormsg = "Client id already in use"; goto fail; }

	mDNS_DomainType dt1 = regDom ? mDNS_DomainTypeRegistration        : mDNS_DomainTypeBrowse;
	mDNS_DomainType dt2 = regDom ? mDNS_DomainTypeRegistrationDefault : mDNS_DomainTypeBrowseDefault;

	// Allocate memory, and handle failure
	DNSServiceDomainEnumeration *x = mallocL("DNSServiceDomainEnumeration", sizeof(*x));
	if (!x) { err = mStatus_NoMemoryErr; errormsg = "No memory"; goto fail; }

	// Set up object, and link into list
	x->ClientMachPort = client;
	x->next = DNSServiceDomainEnumerationList;
	DNSServiceDomainEnumerationList = x;

	verbosedebugf("%5d: Enumerate %s Domains", client, regDom ? "Registration" : "Browsing");

	// Do the operation
	err           = mDNS_GetDomains(&mDNSStorage, &x->dom, dt1, NULL, mDNSInterface_LocalOnly, DomainEnumFound, x);
	if (!err) err = mDNS_GetDomains(&mDNSStorage, &x->def, dt2, NULL, mDNSInterface_LocalOnly, DomainEnumFound, x);
	if (err) { AbortClient(client, x); errormsg = "mDNS_GetDomains"; goto fail; }

	// Succeeded: Wrap up and return
	LogOperation("%5d: DNSServiceDomainEnumeration(%##s) START", client, x->dom.qname.c);
	EnableDeathNotificationForClient(client, x);
	return(mStatus_NoError);

fail:
	LogMsg("%5d: DNSServiceDomainEnumeration(%d) failed: %s (%ld)", client, regDom, errormsg, err);
	return(err);
	}

//*************************************************************************************************************
// Browse for services

mDNSlocal void FoundInstance(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, QC_result AddRecord)
	{
	(void)m;		// Unused

	if (answer->rrtype != kDNSType_PTR)
		{ LogMsg("FoundInstance: Should not be called with rrtype %d (not a PTR record)", answer->rrtype); return; }

	domainlabel name;
	domainname type, domain;
	if (!DeconstructServiceName(&answer->rdata->u.name, &name, &type, &domain))
		{
		LogMsg("FoundInstance: %##s PTR %##s received from network is not valid DNS-SD service pointer",
			answer->name->c, answer->rdata->u.name.c);
		return;
		}

	DNSServiceBrowserResult *x = mallocL("DNSServiceBrowserResult", sizeof(*x));
	if (!x) { LogMsg("FoundInstance: Failed to allocate memory for result %##s", answer->rdata->u.name.c); return; }

	verbosedebugf("FoundInstance: %s %##s", AddRecord ? "Add" : "Rmv", answer->rdata->u.name.c);
	AssignDomainName(&x->result, &answer->rdata->u.name);
	if (AddRecord)
		 x->resultType = DNSServiceBrowserReplyAddInstance;
	else x->resultType = DNSServiceBrowserReplyRemoveInstance;
	x->next = NULL;

	DNSServiceBrowser *browser = (DNSServiceBrowser *)question->QuestionContext;
	DNSServiceBrowserResult **p = &browser->results;
	while (*p) p = &(*p)->next;
	*p = x;

	LogOperation("%5d: DNSServiceBrowse(%##s, %s) RESULT %s %s",
		browser->ClientMachPort, question->qname.c, DNSTypeName(question->qtype), AddRecord ? "Add" : "Rmv", RRDisplayString(m, answer));
	}

mDNSlocal mStatus AddDomainToBrowser(DNSServiceBrowser *browser, const domainname *d)
	{
	mStatus err = mStatus_NoError;
	DNSServiceBrowserQuestion *ptr, *question = NULL;

	for (ptr = browser->qlist; ptr; ptr = ptr->next)
		{
		if (SameDomainName(&ptr->q.qname, d))
			{ debugf("Domain %##s already contained in browser", d->c); return mStatus_AlreadyRegistered; }
		}
	
	question = mallocL("DNSServiceBrowserQuestion", sizeof(DNSServiceBrowserQuestion));
	if (!question) { LogMsg("Error: malloc"); return mStatus_NoMemoryErr; }
	AssignDomainName(&question->domain, d);
	question->next = browser->qlist;
	browser->qlist = question;
	LogOperation("%5d: DNSServiceBrowse(%##s%##s) START", browser->ClientMachPort, browser->type.c, d->c);
	err = mDNS_StartBrowse(&mDNSStorage, &question->q, &browser->type, d, mDNSInterface_Any, mDNSfalse, FoundInstance, browser);
	if (err) LogMsg("Error: AddDomainToBrowser: mDNS_StartBrowse %d", err);
	return err;
	}

mDNSexport void machserver_automatic_browse_domain_changed(const domainname *d, mDNSBool add)
	{
	DNSServiceBrowser *ptr;
	for (ptr = DNSServiceBrowserList; ptr; ptr = ptr->next)
		{
		if (ptr->DefaultDomain)
			{
			if (add)
				{
				mStatus err = AddDomainToBrowser(ptr, d);
				if (err && err != mStatus_AlreadyRegistered) LogMsg("Default browse in domain %##s for client %5d failed. Continuing", d, ptr->ClientMachPort);
				}
			else
				{
				DNSServiceBrowserQuestion **q = &ptr->qlist;
				while (*q)
					{
					if (SameDomainName(&(*q)->domain, d))
						{
						DNSServiceBrowserQuestion *remove = *q;
						*q = (*q)->next;
						mDNS_StopQueryWithRemoves(&mDNSStorage, &remove->q);
						freeL("DNSServiceBrowserQuestion", remove );
						return;
						}
					q = &(*q)->next;
					}
			    LogMsg("Requested removal of default domain %##s not in client %5d's list", d->c, ptr->ClientMachPort);
				}
			}
		}
	}

mDNSexport kern_return_t provide_DNSServiceBrowserCreate_rpc(mach_port_t unusedserver, mach_port_t client,
	DNSCString regtype, DNSCString domain)
	{
	// Check client parameter
	(void)unusedserver;		// Unused
	mStatus err = mStatus_NoError;
	const char *errormsg = "Unknown";

	if (client == (mach_port_t)-1)      { err = mStatus_Invalid; errormsg = "Client id -1 invalid";     goto fail; }
	if (CheckForExistingClient(client)) { err = mStatus_Invalid; errormsg = "Client id already in use"; goto fail; }

	// Check other parameters
	domainname t, d;
	t.c[0] = 0;
	mDNSs32 NumSubTypes = ChopSubTypes(regtype);	// Note: Modifies regtype string to remove trailing subtypes
	if (NumSubTypes < 0 || NumSubTypes > 1)               { errormsg = "Bad Service SubType"; goto badparam; }
	if (NumSubTypes == 1 && !AppendDNSNameString(&t, regtype + strlen(regtype) + 1))
	                                                      { errormsg = "Bad Service SubType"; goto badparam; }
	if (!regtype[0] || !AppendDNSNameString(&t, regtype)) { errormsg = "Illegal regtype";     goto badparam; }
	domainname temp;
	if (!MakeDomainNameFromDNSNameString(&temp, regtype)) { errormsg = "Illegal regtype";     goto badparam; }
	if (temp.c[0] > 15 && (!domain || domain[0] == 0)) domain = "local."; // For over-long service types, we only allow domain "local"

	// Allocate memory, and handle failure
	DNSServiceBrowser *x = mallocL("DNSServiceBrowser", sizeof(*x));
	if (!x) { err = mStatus_NoMemoryErr; errormsg = "No memory"; goto fail; }

	// Set up object, and link into list
	AssignDomainName(&x->type, &t);
	x->ClientMachPort = client;
	x->results = NULL;
	x->lastsuccess = 0;
	x->qlist = NULL;
	x->next = DNSServiceBrowserList;
	DNSServiceBrowserList = x;

	if (domain[0])
		{
		// Start browser for an explicit domain
		x->DefaultDomain = mDNSfalse;
		if (!MakeDomainNameFromDNSNameString(&d, domain)) { errormsg = "Illegal domain";  goto badparam; }
		err = AddDomainToBrowser(x, &d);
		if (err) { AbortClient(client, x); errormsg = "AddDomainToBrowser"; goto fail; }
		}
	else
		{
		DNameListElem *sdPtr;
		// Start browser on all domains
		x->DefaultDomain = mDNStrue;
		if (!AutoBrowseDomains) { AbortClient(client, x); errormsg = "GetSearchDomainList"; goto fail; }
		for (sdPtr = AutoBrowseDomains; sdPtr; sdPtr = sdPtr->next)
			{
			err = AddDomainToBrowser(x, &sdPtr->name);
			if (err)
				{
				// only terminally bail if .local fails
				if (!SameDomainName(&localdomain, &sdPtr->name))
					LogMsg("Default browse in domain %##s failed. Continuing", sdPtr->name.c);
				else { AbortClient(client, x); errormsg = "AddDomainToBrowser"; goto fail; }
				}
			}
		}
	
	// Succeeded: Wrap up and return
	EnableDeathNotificationForClient(client, x);
	return(mStatus_NoError);

	badparam:
	err = mStatus_BadParamErr;
fail:
	LogMsg("%5d: DNSServiceBrowse(\"%s\", \"%s\") failed: %s (%ld)", client, regtype, domain, errormsg, err);
	return(err);
	}

//*************************************************************************************************************
// Resolve Service Info
	
mDNSlocal void FoundInstanceInfo(mDNS *const m, ServiceInfoQuery *query)
	{
	kern_return_t status;
	DNSServiceResolver *x = (DNSServiceResolver *)query->ServiceInfoQueryContext;
	NetworkInterfaceInfoOSX *ifx = (NetworkInterfaceInfoOSX *)query->info->InterfaceID;
	if (query->info->InterfaceID == mDNSInterface_LocalOnly) ifx = mDNSNULL;
	struct sockaddr_storage interface;
	struct sockaddr_storage address;
	char cstring[1024];
	int i, pstrlen = query->info->TXTinfo[0];
	(void)m;		// Unused

	//debugf("FoundInstanceInfo %.4a %.4a %##s", &query->info->InterfaceAddr, &query->info->ip, &query->info->name);

	if (query->info->TXTlen > sizeof(cstring)) return;

	mDNSPlatformMemZero(&interface, sizeof(interface));
	mDNSPlatformMemZero(&address,   sizeof(address));

	if (ifx && ifx->ifinfo.ip.type == mDNSAddrType_IPv4)
		{
		struct sockaddr_in *sin = (struct sockaddr_in*)&interface;
		sin->sin_len         = sizeof(*sin);
		sin->sin_family      = AF_INET;
		sin->sin_port        = 0;
		sin->sin_addr.s_addr = ifx->ifinfo.ip.ip.v4.NotAnInteger;
		}
	else if (ifx && ifx->ifinfo.ip.type == mDNSAddrType_IPv6)
		{
		struct sockaddr_in6 *sin6 = (struct sockaddr_in6*)&interface;
		sin6->sin6_len       = sizeof(*sin6);
		sin6->sin6_family    = AF_INET6;
		sin6->sin6_flowinfo  = 0;
		sin6->sin6_port      = 0;
		sin6->sin6_addr      = *(struct in6_addr*)&ifx->ifinfo.ip.ip.v6;
		sin6->sin6_scope_id  = ifx->scope_id;
		}

	if (query->info->ip.type == mDNSAddrType_IPv4)
		{
		struct sockaddr_in *sin = (struct sockaddr_in*)&address;
		sin->sin_len           = sizeof(*sin);
		sin->sin_family        = AF_INET;
		sin->sin_port          = query->info->port.NotAnInteger;
		sin->sin_addr.s_addr   = query->info->ip.ip.v4.NotAnInteger;
		}
	else
		{
		struct sockaddr_in6 *sin6 = (struct sockaddr_in6*)&address;
		sin6->sin6_len           = sizeof(*sin6);
		sin6->sin6_family        = AF_INET6;
		sin6->sin6_port          = query->info->port.NotAnInteger;
		sin6->sin6_flowinfo      = 0;
		sin6->sin6_addr          = *(struct in6_addr*)&query->info->ip.ip.v6;
		sin6->sin6_scope_id      = ifx ? ifx->scope_id : 0;
		}

	// The OS X DNSServiceResolverResolve() API is defined using a C-string,
	// but the mDNS_StartResolveService() call actually returns a packed block of P-strings.
	// Hence we have to convert the P-string(s) to a C-string before returning the result to the client.
	// ASCII-1 characters are used in the C-string as boundary markers,
	// to indicate the boundaries between the original constituent P-strings.
	for (i=1; i<query->info->TXTlen; i++)
		{
		if (--pstrlen >= 0)
			cstring[i-1] = query->info->TXTinfo[i];
		else
			{
			cstring[i-1] = 1;
			pstrlen = query->info->TXTinfo[i];
			}
		}
	cstring[i-1] = 0;		// Put the terminating NULL on the end

	LogOperation("%5d: DNSServiceResolver(%##s) -> %#a:%u", x->ClientMachPort,
		x->i.name.c, &query->info->ip, mDNSVal16(query->info->port));
	status = DNSServiceResolverReply_rpc(x->ClientMachPort,
		(char*)&interface, (char*)&address, cstring, 0, MDNS_MM_TIMEOUT);
	if (status == MACH_SEND_TIMED_OUT)
		AbortBlockedClient(x->ClientMachPort, "resolve", x);
	}

mDNSexport kern_return_t provide_DNSServiceResolverResolve_rpc(mach_port_t unusedserver, mach_port_t client,
	DNSCString name, DNSCString regtype, DNSCString domain)
	{
	// Check client parameter
	(void)unusedserver;		// Unused
	mStatus err = mStatus_NoError;
	const char *errormsg = "Unknown";
	if (client == (mach_port_t)-1)      { err = mStatus_Invalid; errormsg = "Client id -1 invalid";     goto fail; }
	if (CheckForExistingClient(client)) { err = mStatus_Invalid; errormsg = "Client id already in use"; goto fail; }

	// Check other parameters
	domainlabel n;
	domainname t, d, srv;
	if (!name[0]    || !MakeDomainLabelFromLiteralString(&n, name))        { errormsg = "Bad Instance Name"; goto badparam; }
	if (!regtype[0] || !MakeDomainNameFromDNSNameString(&t, regtype))      { errormsg = "Bad Service Type";  goto badparam; }
	if (!domain[0]  || !MakeDomainNameFromDNSNameString(&d, domain))       { errormsg = "Bad Domain";        goto badparam; }
	if (!ConstructServiceName(&srv, &n, &t, &d))                           { errormsg = "Bad Name";          goto badparam; }

	// Allocate memory, and handle failure
	DNSServiceResolver *x = mallocL("DNSServiceResolver", sizeof(*x));
	if (!x) { err = mStatus_NoMemoryErr; errormsg = "No memory"; goto fail; }

	// Set up object, and link into list
	x->ClientMachPort = client;
	x->i.InterfaceID = mDNSInterface_Any;
	x->i.name = srv;
	x->ReportTime = NonZeroTime(mDNS_TimeNow(&mDNSStorage) + 130 * mDNSPlatformOneSecond);
	x->next = DNSServiceResolverList;
	DNSServiceResolverList = x;

	// Do the operation
	LogOperation("%5d: DNSServiceResolve(%##s) START", client, x->i.name.c);
	err = mDNS_StartResolveService(&mDNSStorage, &x->q, &x->i, FoundInstanceInfo, x);
	if (err) { AbortClient(client, x); errormsg = "mDNS_StartResolveService"; goto fail; }

	// Succeeded: Wrap up and return
	EnableDeathNotificationForClient(client, x);
	return(mStatus_NoError);

badparam:
	err = mStatus_BadParamErr;
fail:
	LogMsg("%5d: DNSServiceResolve(\"%s\", \"%s\", \"%s\") failed: %s (%ld)", client, name, regtype, domain, errormsg, err);
	return(err);
	}

//*************************************************************************************************************
// Registration

mDNSexport void RecordUpdatedNiceLabel(mDNS *const m, mDNSs32 delay)
	{
	m->p->NotifyUser = NonZeroTime(m->timenow + delay);
	}

mDNSlocal void RegCallback(mDNS *const m, ServiceRecordSet *const srs, mStatus result)
	{
	ServiceInstance *si = (ServiceInstance*)srs->ServiceContext;

	if (result == mStatus_NoError)
		{
		kern_return_t status;
		LogOperation("%5d: DNSServiceRegistration(%##s, %u) Name Registered", si->ClientMachPort, srs->RR_SRV.resrec.name->c, SRS_PORT(srs));
		status = DNSServiceRegistrationReply_rpc(si->ClientMachPort, result, MDNS_MM_TIMEOUT);
		if (status == MACH_SEND_TIMED_OUT)
			AbortBlockedClient(si->ClientMachPort, "registration success", si);
		if (si->autoname && CountPeerRegistrations(m, srs) == 0)
			RecordUpdatedNiceLabel(m, 0);	// Successfully got new name, tell user immediately
		}

	else if (result == mStatus_NameConflict)
		{
		LogOperation("%5d: DNSServiceRegistration(%##s, %u) Name Conflict", si->ClientMachPort, srs->RR_SRV.resrec.name->c, SRS_PORT(srs));
		// Note: By the time we get the mStatus_NameConflict message, the service is already deregistered
		// and the memory is free, so we don't have to wait for an mStatus_MemFree message as well.
		if (si->autoname && CountPeerRegistrations(m, srs) == 0)
			{
			// On conflict for an autoname service, rename and reregister *all* autoname services
			IncrementLabelSuffix(&m->nicelabel, mDNStrue);
			m->MainCallback(m, mStatus_ConfigChanged);
			}
		else if (si->autoname)
			{
            mDNS_RenameAndReregisterService(m, srs, mDNSNULL);
            return;
			}
		else
			{
			// If we get a name conflict, we tell the client about it, and then they are expected to dispose
			// of their registration in the usual way (which we will catch via client death notification).
			// If the Mach queue is full, we forcibly abort the client immediately.
			kern_return_t status = DNSServiceRegistrationReply_rpc(si->ClientMachPort, result, MDNS_MM_TIMEOUT);
			if (status == MACH_SEND_TIMED_OUT)
				AbortBlockedClient(si->ClientMachPort, "registration conflict", NULL);
			}
		}

	else if (result == mStatus_MemFree)
		{
		if (si->renameonmemfree)	// We intentionally terminated registration so we could re-register with new name
			{
			debugf("RegCallback renaming %#s to %#s", si->name.c, m->nicelabel.c);
			si->renameonmemfree = mDNSfalse;
			si->name            = m->nicelabel;
			mDNS_RenameAndReregisterService(m, srs, &si->name);
			}
		else
			{
			// SANITY CHECK: make sure service instance is no longer in any ServiceRegistration's list
			DNSServiceRegistration *r;
			for (r = DNSServiceRegistrationList; r; r = r->next)
				{
				ServiceInstance **sp = &r->regs;
				while (*sp)
					{
					if (*sp == si) { LogMsg("RegCallback: %##s Still in list; removing", srs->RR_SRV.resrec.name->c); *sp = (*sp)->next; break; }
					sp = &(*sp)->next;
					}
			    }
			// END SANITY CHECK
			FreeServiceInstance(si);
			}
		}

	else if (result != mStatus_NATTraversal)
		LogMsg("%5d: DNSServiceRegistration(%##s, %u) Unknown Result %ld", si->ClientMachPort, srs->RR_SRV.resrec.name->c, SRS_PORT(srs), result);
	}

mDNSlocal mStatus AddServiceInstance(DNSServiceRegistration *x, const domainname *domain)
	{
	mStatus err = 0;
	ServiceInstance *si = NULL;
	AuthRecord *SubTypes = NULL;

	for (si = x->regs; si; si = si->next)
		{
		if (SameDomainName(&si->domain, domain))
			{ LogMsg("Requested addition of domain %##s already in list", domain->c); return mStatus_AlreadyRegistered; }
		}
	
	SubTypes = AllocateSubTypes(x->NumSubTypes, x->regtype);
	if (x->NumSubTypes && !SubTypes) return mStatus_NoMemoryErr;
	
	si = mallocL("ServiceInstance", sizeof(*si) - sizeof(RDataBody) + x->rdsize);
	if (!si) return mStatus_NoMemoryErr;

	si->ClientMachPort  = x->ClientMachPort;
	si->renameonmemfree = mDNSfalse;
	si->autoname        = x->autoname;
	si->name            = x->autoname ? mDNSStorage.nicelabel : x->name;
	si->domain          = *domain;

	err = mDNS_RegisterService(&mDNSStorage, &si->srs, &si->name, &x->type, domain, NULL,
		x->port, x->txtinfo, x->txt_len, SubTypes, x->NumSubTypes, mDNSInterface_Any, RegCallback, si);
	if (!err)
		{
		si->next = x->regs;
		x->regs = si;
		}
	else
		{
		LogMsg("Error %d for registration of service in domain %##s", err, domain->c);
		freeL("ServiceInstance", si);
		}
	return err;
	}

mDNSexport void machserver_automatic_registration_domain_changed(const domainname *d, mDNSBool add)
	{
	DNSServiceRegistration *reg;

	for (reg = DNSServiceRegistrationList; reg; reg = reg->next)
		{
		if (reg->DefaultDomain)
			{
			if (add)
				AddServiceInstance(reg, d);
			else
				{
				ServiceInstance **si = &reg->regs;
				while (*si)
					{
					if (SameDomainName(&(*si)->domain, d))
						{
						ServiceInstance *s = *si;
						*si = (*si)->next;
						if (mDNS_DeregisterService(&mDNSStorage, &s->srs)) FreeServiceInstance(s);  // only free memory synchronously on error
						break;
						}
					si = &(*si)->next;
					}
				if (!si) debugf("Requested removal of default domain %##s not in client %5d's list", d, reg->ClientMachPort); // normal if registration failed
				}
			}
		}
	}

mDNSexport kern_return_t provide_DNSServiceRegistrationCreate_rpc(mach_port_t unusedserver, mach_port_t client,
	DNSCString name, DNSCString regtype, DNSCString domain, IPPort IpPort, DNSCString txtRecord)
	{
	(void)unusedserver;		// Unused
	mStatus err = mStatus_NoError;
	const char *errormsg = "Unknown";

	// older versions of this code passed the port via mach IPC as an int.
	// we continue to pass it as 4 bytes to maintain binary compatibility,
	// but now ensure that the network byte order is preserved by using a struct
	mDNSIPPort port;
	port.b[0] = IpPort.bytes[2];
	port.b[1] = IpPort.bytes[3];

	if (client == (mach_port_t)-1)      { err = mStatus_Invalid; errormsg = "Client id -1 invalid";     goto fail; }
	if (CheckForExistingClient(client)) { err = mStatus_Invalid; errormsg = "Client id already in use"; goto fail; }

    // Check for sub-types after the service type
	size_t reglen = strlen(regtype) + 1;
	if (reglen > MAX_ESCAPED_DOMAIN_NAME) { errormsg = "reglen too long"; goto badparam; }
	mDNSs32 NumSubTypes = ChopSubTypes(regtype);	// Note: Modifies regtype string to remove trailing subtypes
	if (NumSubTypes < 0) { errormsg = "Bad Service SubType"; goto badparam; }

	// Check other parameters
	domainlabel n;
	domainname t, d;
	domainname srv;
	if (!name[0]) n = mDNSStorage.nicelabel;
	else if (!MakeDomainLabelFromLiteralString(&n, name))                  { errormsg = "Bad Instance Name"; goto badparam; }
	if (!regtype[0] || !MakeDomainNameFromDNSNameString(&t, regtype))      { errormsg = "Bad Service Type";  goto badparam; }
	if (!MakeDomainNameFromDNSNameString(&d, *domain ? domain : "local.")) { errormsg = "Bad Domain";        goto badparam; }
	if (!ConstructServiceName(&srv, &n, &t, &d))                           { errormsg = "Bad Name";          goto badparam; }

	unsigned char txtinfo[1024] = "";
	unsigned int data_len = 0;
	unsigned int size = sizeof(RDataBody);
	unsigned char *pstring = &txtinfo[data_len];
	char *ptr = txtRecord;

	// The OS X DNSServiceRegistrationCreate() API is defined using a C-string,
	// but the mDNS_RegisterService() call actually requires a packed block of P-strings.
	// Hence we have to convert the C-string to a P-string.
	// ASCII-1 characters are allowed in the C-string as boundary markers,
	// so that a single C-string can be used to represent one or more P-strings.
	while (*ptr)
		{
		if (++data_len >= sizeof(txtinfo)) { errormsg = "TXT record too long"; goto badtxt; }
		if (*ptr == 1)		// If this is our boundary marker, start a new P-string
			{
			pstring = &txtinfo[data_len];
			pstring[0] = 0;
			ptr++;
			}
		else
			{
			if (pstring[0] == 255) { errormsg = "TXT record invalid (component longer than 255)"; goto badtxt; }
			pstring[++pstring[0]] = *ptr++;
			}
		}

	data_len++;
	if (size < data_len)
		size = data_len;

	// Some clients use mDNS for lightweight copy protection, registering a pseudo-service with
	// a port number of zero. When two instances of the protected client are allowed to run on one
	// machine, we don't want to see misleading "Bogus client" messages in syslog and the console.
	if (!mDNSIPPortIsZero(port))
		{
		int count = CountExistingRegistrations(&srv, port);
		if (count)
			LogMsg("%5d: Client application registered %d identical instances of service %##s port %u.",
				   client, count+1, srv.c, mDNSVal16(port));
		}
	
	// Allocate memory, and handle failure
	DNSServiceRegistration *x = mallocL("DNSServiceRegistration", sizeof(*x));
	if (!x) { err = mStatus_NoMemoryErr; errormsg = "No memory"; goto fail; }
	mDNSPlatformMemZero(x, sizeof(*x));

	// Set up object, and link into list
	x->ClientMachPort = client;
	x->DefaultDomain = !domain[0];
	x->autoname = (!name[0]);
	x->rdsize = size;
	x->NumSubTypes = NumSubTypes;
	memcpy(x->regtype, regtype, reglen);
	x->name = n;
	x->type = t;
	x->port = port;
	memcpy(x->txtinfo, txtinfo, 1024);
	x->txt_len = data_len;
	x->NextRef = 0;
	x->regs = NULL;
	
	x->next = DNSServiceRegistrationList;
	DNSServiceRegistrationList = x;

	LogOperation("%5d: DNSServiceRegistration(\"%s\", \"%s\", \"%s\", %u) START",
		x->ClientMachPort, name, regtype, domain, mDNSVal16(port));

   	err = AddServiceInstance(x, &d);
	if (err) { AbortClient(client, x); errormsg = "mDNS_RegisterService"; goto fail; }  // bail if .local (or explicit domain) fails

	if (x->DefaultDomain)
		{
		DNameListElem *ptr;
		for (ptr = AutoRegistrationDomains; ptr; ptr = ptr->next)
			AddServiceInstance(x, &ptr->name);
		}

	// Succeeded: Wrap up and return
	EnableDeathNotificationForClient(client, x);
	return(mStatus_NoError);

badtxt:
	LogMsg("%5d: TXT record: %.100s...", client, txtRecord);
badparam:
	err = mStatus_BadParamErr;
fail:
	LogMsg("%5d: DNSServiceRegister(\"%s\", \"%s\", \"%s\", %d) failed: %s (%ld)",
		   client, name, regtype, domain, mDNSVal16(port), errormsg, err);
	return(err);
	}

mDNSlocal domainlabel gNotificationPrefHostLabel;	// The prefs as they were the last time we saw them
mDNSlocal domainlabel gNotificationPrefNiceLabel;
mDNSlocal domainlabel gNotificationUserHostLabel;	// The prefs as they were the last time the user saw them
mDNSlocal domainlabel gNotificationUserNiceLabel;

// We have four copies each of the HostLabel and the NiceLabel.
// The logic works like this (for HostLabel; NiceLabel is analogous):
// m->hostlabel        is the name currently in use by the mDNS core code.
// m->p->userhostlabel is the name most recently read from the user preferences by the platform support layer.
// Any time the user changes the preferences, both m->hostlabel and m->p->userhostlabel
// are updated simultaneously. This means that if we see that the names do not match, we
// must have had a name conflict which caused the mDNS core code to update m->hostlabel.
// We respond to this mismatch by displaying an alert to tell the user about the name change,
// and by writing the new name back to persistent preferences. When we do this:
// gNotificationPrefHostLabel is set to the new name (which is written immediately to the preferences) and
// gNotificationUserHostLabel holds the name as it was the last time the user was deemed to be "aware" of it.
// These two variables allow us to display both the "old" and "new" names to the user.
// Two events cause gNotificationUserHostLabel to be updated:
//  * An mStatus_ConfigChanged message where m->p->userhostlabel != gNotificationPrefHostLabel.
//    The means the user changed userhostlabel themself, hence they must be "aware" of the name.
//  * The user dismisses the name change alert, thereby acknowledging that they are "aware" of the new name.
//
// Summary:
//
// If the user updates the preferences, m->hostlabel and m->p->userhostlabel are set to the new value,
// and then a mStatus_ConfigChanged message is delivered immediately,
// and the code notices that m->p->userhostlabel and gNotificationPrefHostLabel don't match,
// so it updates gNotificationUserHostLabel and gNotificationPrefHostLabel too.
//
// If the mDNS core code updates the name, only m->hostlabel changes.
// The code notices that m->p->userhostlabel and m->hostlabel don't match,
// immediately updates m->p->userhostlabel, gNotificationPrefHostLabel
// and the preferences on disk to match the new m->hostlabel,
// displays an alert showing both the old name (gNotificationUserHostLabel) and the new name,
// and then when the user dismisses the alert, it updates gNotificationUserHostLabel to match the other three

#ifndef NO_CFUSERNOTIFICATION
mDNSlocal CFUserNotificationRef gNotification    = NULL;
mDNSlocal CFRunLoopSourceRef    gNotificationRLS = NULL;

mDNSlocal void NotificationCallBackDismissed(CFUserNotificationRef userNotification, CFOptionFlags responseFlags)
	{
	LogOperation("NotificationCallBackDismissed");
	KQueueLock(&mDNSStorage);
	(void)responseFlags;	// Unused
	if (userNotification != gNotification) LogMsg("NotificationCallBackDismissed: Wrong CFUserNotificationRef");
	if (gNotificationRLS)
		{
		// Caution: don't use CFRunLoopGetCurrent() here, because the currently executing thread may not be our "CFRunLoopRun" thread.
		// We need to explicitly specify the desired CFRunLoop from which we want to remove this event source.
		CFRunLoopRemoveSource(CFRunLoop, gNotificationRLS, kCFRunLoopDefaultMode);
		CFRelease(gNotificationRLS);
		gNotificationRLS = NULL;
		CFRelease(gNotification);
		gNotification = NULL;
		}
	// By dismissing the alert, the user has conceptually acknowleged the rename.
	// (e.g. the machine's name is now officially "computer-2.local", not "computer.local".)
	// If we get *another* conflict, the new alert should refer to the 'old' name
	// as now being "computer-2.local", not "computer.local"
	gNotificationUserHostLabel = gNotificationPrefHostLabel;
	gNotificationUserNiceLabel = gNotificationPrefNiceLabel;
	PlatformStorage.HostNameConflict = 0;	// Clear our indicator, now user has acknowledged seeing dialog
	KQueueUnlock(&mDNSStorage, "NotificationCallBackDismissed");
	}

mDNSlocal void ShowNameConflictNotification(CFStringRef header, CFStringRef subtext)
	{
	CFMutableDictionaryRef dictionary = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	if (!dictionary) return;
	CFDictionarySetValue(dictionary, kCFUserNotificationAlertHeaderKey, header);
	CFDictionarySetValue(dictionary, kCFUserNotificationAlertMessageKey, subtext);

	CFURLRef urlRef = CFURLCreateWithFileSystemPath(NULL, CFSTR("/System/Library/CoreServices/mDNSResponder.bundle"), kCFURLPOSIXPathStyle, true);
	if (urlRef) { CFDictionarySetValue(dictionary, kCFUserNotificationLocalizationURLKey, urlRef); CFRelease(urlRef); }

	if (gNotification)	// If notification already on-screen, update it in place
		CFUserNotificationUpdate(gNotification, 0, kCFUserNotificationCautionAlertLevel, dictionary);
	else				// else, we need to create it
		{
		SInt32 error;
		gNotification = CFUserNotificationCreate(NULL, 0, kCFUserNotificationCautionAlertLevel, &error, dictionary);
		if (!gNotification || error) { LogMsg("ShowNameConflictNotification: CFUserNotificationRef: Error %d", error); return; }
		gNotificationRLS = CFUserNotificationCreateRunLoopSource(NULL, gNotification, NotificationCallBackDismissed, 0);
		if (!gNotificationRLS) { LogMsg("ShowNameConflictNotification: RLS"); CFRelease(gNotification); gNotification = NULL; return; }
		// Caution: don't use CFRunLoopGetCurrent() here, because the currently executing thread may not be our "CFRunLoopRun" thread.
		// We need to explicitly specify the desired CFRunLoop to which we want to add this event source.
		CFRunLoopAddSource(CFRunLoop, gNotificationRLS, kCFRunLoopDefaultMode);
		}

	CFRelease(dictionary);
	}
#endif /* NO_CFUSERNOTIFICATION */

// We want to write the new Computer Name to System Preferences, without disturbing the user-selected
// system-wide default character set used for things like AppleTalk NBP and NETBIOS service advertising.
// Since both are set by the same call, we need to take care to set the name without changing the default character set.
mDNSlocal Boolean SafeSCPreferencesSetComputerName(CFStringRef name)
	{
	CFStringEncoding charset = kCFStringEncodingUTF8;					// Defensive coding -- assume UTF8
	CFStringRef cfs = SCDynamicStoreCopyComputerName(NULL, &charset);	// Get Computer Name and system default character set
	CFRelease(cfs);														// Discard the old name we don't care about
	return (0 == mDNSPreferencesSetName(kmDNSComputerName, name, charset)); // Set new name, preserving current default character set
	}

static CFStringRef CFS_OQ;
static CFStringRef CFS_CQ;
static CFStringRef CFS_Format;
static CFStringRef CFS_ComputerName;
static CFStringRef CFS_ComputerNameMsg;
static CFStringRef CFS_LocalHostName;
static CFStringRef CFS_LocalHostNameMsg;

// This updates either the text of the field currently labelled "Local Hostname",
// or the text of the field currently labelled "Computer Name"
// in the Sharing Prefs Control Panel
mDNSlocal void RecordUpdatedName(const mDNS *const m, const domainlabel *const olddl, const domainlabel *const newdl,
	const CFStringRef msg, const char *const suffix, const CFStringRef subtext)
	{
	char oldname[MAX_DOMAIN_LABEL+1];
	char newname[MAX_DOMAIN_LABEL+1];
	ConvertDomainLabelToCString_unescaped(olddl, oldname);
	ConvertDomainLabelToCString_unescaped(newdl, newname);
	const CFStringRef cfoldname = CFStringCreateWithCString(NULL, oldname,  kCFStringEncodingUTF8);
	const CFStringRef cfnewname = CFStringCreateWithCString(NULL, newname,  kCFStringEncodingUTF8);
	// We tag a zero-width non-breaking space at the end of the literal text to guarantee that, no matter what
	// arbitrary computer name the user may choose, this exact text (with zero-width non-breaking space added)
	// can never be one that occurs in the Localizable.strings translation file.
	if (!cfoldname || !cfnewname)
		LogMsg("RecordUpdatedName: ERROR: Couldn't construct name");
	else
		{
		const CFStringRef s1 = CFStringCreateWithFormat(NULL, NULL, CFS_Format, cfoldname, suffix);
		const CFStringRef s2 = CFStringCreateWithFormat(NULL, NULL, CFS_Format, cfnewname, suffix);
		// On Tiger and later, if we pass an array instead of a string, CFUserNotification will translate each
		// element of the array individually for us, and then concatenate the results to make the final message.
		// This lets us have the relevant bits localized, but not the literal names, which should not be translated.
		// On Panther this does not work, so we just build the string directly, and it will not be translated.
		const CFMutableStringRef alertHeader =
			(OSXVers < 8) ? CFStringCreateMutable(NULL, 0) : (CFMutableStringRef)CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
		Boolean result;
		if (newdl == &gNotificationPrefHostLabel) result = (0 == mDNSPreferencesSetName(kmDNSLocalHostName, cfnewname, 0));
		else result = SafeSCPreferencesSetComputerName(cfnewname);
		if (!result || !s1 || !s2 || !alertHeader)
				LogMsg("RecordUpdatedName: ERROR: Couldn't update SCPreferences");
		else if (m->p->NotifyUser)
			{
#ifndef NO_CFUSERNOTIFICATION
			uid_t uid;
			gid_t gid;
			CFStringRef userName = SCDynamicStoreCopyConsoleUser(NULL, &uid, &gid);
			if (userName)
				{
				CFRelease(userName);
				typedef void CFStringAppendFN(CFMutableStringRef theString, CFStringRef appendedString);
				CFStringAppendFN *const append = (OSXVers < 8) ? &CFStringAppend : (CFStringAppendFN*)&CFArrayAppendValue;
				append(alertHeader, msg);	// Opening phrase of message, provided by caller
				append(alertHeader, CFS_OQ); append(alertHeader, s1); append(alertHeader, CFS_CQ);
				append(alertHeader, CFSTR(" is already in use on this network. "));
				append(alertHeader, CFSTR("The name has been changed to "));
				append(alertHeader, CFS_OQ); append(alertHeader, s2); append(alertHeader, CFS_CQ);
				append(alertHeader, CFSTR("."));
				ShowNameConflictNotification(alertHeader, subtext);
				}
#else
			(void)subtext;
#endif /*  NO_CFUSERNOTIFICATION */
			}
		if (s1)          CFRelease(s1);
		if (s2)          CFRelease(s2);
		if (alertHeader) CFRelease(alertHeader);
		}
	if (cfoldname) CFRelease(cfoldname);
	if (cfnewname) CFRelease(cfnewname);
	}

mDNSlocal void mDNS_StatusCallback(mDNS *const m, mStatus result)
	{
	(void)m; // Unused
	if (result == mStatus_NoError)
		{
		if (!SameDomainLabelCS(m->p->userhostlabel.c, m->hostlabel.c))
			LogOperation("Local Hostname changed from \"%#s.local\" to \"%#s.local\"", m->p->userhostlabel.c, m->hostlabel.c);
		// One second pause in case we get a Computer Name update too -- don't want to alert the user twice
		RecordUpdatedNiceLabel(m, mDNSPlatformOneSecond);
		}
	else if (result == mStatus_NameConflict)
		{
		LogOperation("Local Hostname conflict for \"%#s.local\"", m->hostlabel.c);
		if (!m->p->HostNameConflict) m->p->HostNameConflict = NonZeroTime(m->timenow);
		else if (m->timenow - m->p->HostNameConflict > 60 * mDNSPlatformOneSecond)
			{
			char oldname[MAX_DOMAIN_LABEL+1];
			ConvertDomainLabelToCString_unescaped(&gNotificationUserHostLabel, oldname);
			const CFStringRef cfoldname = CFStringCreateWithCString(NULL, oldname,  kCFStringEncodingUTF8);
			if (cfoldname)
				{
				const CFStringRef s1 = CFStringCreateWithFormat(NULL, NULL, CFS_Format, cfoldname, ".local");
				if (s1)
					{
					typedef void CFStringAppendFN(CFMutableStringRef theString, CFStringRef appendedString);
					CFStringAppendFN *const append = (OSXVers < 8) ? &CFStringAppend : (CFStringAppendFN*)&CFArrayAppendValue;
					const CFMutableStringRef alertHeader =
						(OSXVers < 8) ? CFStringCreateMutable(NULL, 0) : (CFMutableStringRef)CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
					if (alertHeader)
						{
						append(alertHeader, CFS_LocalHostName);
						append(alertHeader, CFS_OQ); append(alertHeader, s1); append(alertHeader, CFS_CQ);
						append(alertHeader, CFSTR(" is already in use on this network. "));
						append(alertHeader, CFSTR("All attempts to find an available name by adding a number to the name were also unsuccessful."));
						ShowNameConflictNotification(alertHeader, CFSTR("This may indicate a problem with the local network. Please inform your network administrator."));
						CFRelease(alertHeader);
						}
					CFRelease(s1);
					}
				CFRelease(cfoldname);
				}
			}
		}
	else if (result == mStatus_GrowCache)
		{
		// Allocate another chunk of cache storage
		CacheEntity *storage = mallocL("mStatus_GrowCache", sizeof(CacheEntity) * RR_CACHE_SIZE);
		//LogOperation("GrowCache %d * %d = %d", sizeof(CacheEntity), RR_CACHE_SIZE, sizeof(CacheEntity) * RR_CACHE_SIZE);
		if (storage) mDNS_GrowCache(m, storage, RR_CACHE_SIZE);
		}
	else if (result == mStatus_ConfigChanged)
		{
		// If the user-specified hostlabel from System Configuration has changed since the last time
		// we saw it, and *we* didn't change it, then that implies that the user has changed it,
		// so we auto-dismiss the name conflict alert.
		// We use a case-sensitive comparison here because even though changing the capitalization
		// of the name is not significant to DNS, it does confirm that the user has seen the new name
		// and acted on that information.
		if (!SameDomainLabelCS(m->p->userhostlabel.c, gNotificationPrefHostLabel.c) ||
			!SameDomainLabelCS(m->p->usernicelabel.c, gNotificationPrefNiceLabel.c))
			{
			gNotificationUserHostLabel = gNotificationPrefHostLabel = m->p->userhostlabel;
			gNotificationUserNiceLabel = gNotificationPrefNiceLabel = m->p->usernicelabel;
#ifndef NO_CFUSERNOTIFICATION
			// If we're showing a name conflict notification, and the user has manually edited
			// the name to remedy the conflict, we should now remove the notification window.
			if (gNotificationRLS) CFUserNotificationCancel(gNotification);
#endif /* NO_CFUSERNOTIFICATION */
			}

		// First we check our list of old Mach-based registered services, to see if any need to be updated to a new name
		DNSServiceRegistration *r;
		for (r = DNSServiceRegistrationList; r; r=r->next)
			if (r->autoname)
				{
				ServiceInstance *si;
				for (si = r->regs; si; si = si->next)
					{
					if (!SameDomainLabelCS(si->name.c, m->nicelabel.c))
						{
						debugf("NetworkChanged renaming %##s to %#s", si->srs.RR_SRV.resrec.name->c, m->nicelabel.c);
						si->renameonmemfree = mDNStrue;
						if (mDNS_DeregisterService(m, &si->srs))	// If service deregistered already, we can re-register immediately
							RegCallback(m, &si->srs, mStatus_MemFree);
						}
					}
				}

		// Then we call into the UDS daemon code, to let it do the same
		udsserver_handle_configchange(m);
		}
	}

//*************************************************************************************************************
// Add / Update / Remove records from existing Registration

mDNSexport kern_return_t provide_DNSServiceRegistrationAddRecord_rpc(mach_port_t unusedserver, mach_port_t client,
	int type, const char *data, mach_msg_type_number_t data_len, uint32_t ttl, natural_t *reference)
	{
	// Check client parameter
	uint32_t id;
	mStatus err = mStatus_NoError;
	const char *errormsg = "Unknown";
	DNSServiceRegistration *x = DNSServiceRegistrationList;
	if (client == (mach_port_t)-1) { err = mStatus_Invalid;         errormsg = "Client id -1 invalid"; goto fail; }
	ServiceInstance *si;
	size_t size;
	(void)unusedserver;		// Unused
	while (x && x->ClientMachPort != client) x = x->next;
	if (!x)                        { err = mStatus_BadReferenceErr; errormsg = "No such client";       goto fail; }

	// Check other parameters
	if (data_len > 8192) { err = mStatus_BadParamErr; errormsg = "data_len > 8K"; goto fail; }
	if (data_len > sizeof(RDataBody)) size = data_len;
	else size = sizeof(RDataBody);
	
	id = x->NextRef++;
	*reference = (natural_t)id;
	for (si = x->regs; si; si = si->next)
		{
		// Allocate memory, and handle failure
		ExtraResourceRecord *extra = mallocL("ExtraResourceRecord", sizeof(*extra) - sizeof(RDataBody) + size);
		if (!extra) { err = mStatus_NoMemoryErr; errormsg = "No memory"; goto fail; }
		
		// Fill in type, length, and data of new record
		extra->r.resrec.rrtype = type;
		extra->r.rdatastorage.MaxRDLength = size;
		extra->r.resrec.rdlength          = data_len;
		memcpy(&extra->r.rdatastorage.u.data, data, data_len);

		// Do the operation
		LogOperation("%5d: DNSServiceRegistrationAddRecord(%##s, type %d, length %d) REF %p",
					 client, si->srs.RR_SRV.resrec.name->c, type, data_len, extra);
		err = mDNS_AddRecordToService(&mDNSStorage, &si->srs, extra, &extra->r.rdatastorage, ttl);

		if (err)
			{
			freeL("Extra Resource Record", extra);
			errormsg = "mDNS_AddRecordToService";
			goto fail;
			}

		extra->ClientID = id;
		}

	return mStatus_NoError;

fail:
	LogMsg("%5d: DNSServiceRegistrationAddRecord(%##s, type %d, length %d) failed: %s (%ld)", client, x ? x->name.c : (mDNSu8*)"\x8""«NULL»", type, data_len, errormsg, err);
	return mStatus_UnknownErr;
	}

mDNSlocal void UpdateCallback(mDNS *const m, AuthRecord *const rr, RData *OldRData)
	{
	(void)m;		// Unused
	if (OldRData != &rr->rdatastorage)
		freeL("Old RData", OldRData);
	}

mDNSlocal mStatus UpdateRecord(ServiceRecordSet *srs, mach_port_t client, AuthRecord *rr, const char *data, mach_msg_type_number_t data_len, uint32_t ttl)
	{
    // Check client parameter
	mStatus err = mStatus_NoError;
	const char *errormsg = "Unknown";
	const domainname *name = (const domainname *)"";

	name = srs->RR_SRV.resrec.name;

	unsigned int size = sizeof(RDataBody);
    if (size < data_len)
		size = data_len;

	// Allocate memory, and handle failure
	RData *newrdata = mallocL("RData", sizeof(*newrdata) - sizeof(RDataBody) + size);
	if (!newrdata) { err = mStatus_NoMemoryErr; errormsg = "No memory"; goto fail; }

	// Fill in new length, and data
	newrdata->MaxRDLength = size;
	memcpy(&newrdata->u, data, data_len);
	
	// BIND named (name daemon) doesn't allow TXT records with zero-length rdata. This is strictly speaking correct,
	// since RFC 1035 specifies a TXT record as "One or more <character-string>s", not "Zero or more <character-string>s".
	// Since some legacy apps try to create zero-length TXT records, we'll silently correct it here.
	if (rr->resrec.rrtype == kDNSType_TXT && data_len == 0) { data_len = 1; newrdata->u.txt.c[0] = 0; }

	// Do the operation
	LogOperation("%5d: DNSServiceRegistrationUpdateRecord(%##s, new length %d)",
		client, srs->RR_SRV.resrec.name->c, data_len);

	err = mDNS_Update(&mDNSStorage, rr, ttl, data_len, newrdata, UpdateCallback);
	if (err)
		{
		errormsg = "mDNS_Update";
		freeL("RData", newrdata);
		return err;
		}
	return(mStatus_NoError);

fail:
	LogMsg("%5d: DNSServiceRegistrationUpdateRecord(%##s, %d) failed: %s (%ld)", client, name->c, data_len, errormsg, err);
	return(err);
	}

mDNSexport kern_return_t provide_DNSServiceRegistrationUpdateRecord_rpc(mach_port_t unusedserver, mach_port_t client,
		natural_t reference, const char *data, mach_msg_type_number_t data_len, uint32_t ttl)
   	{
    // Check client parameter
	mStatus err = mStatus_NoError;
	const char *errormsg = "Unknown";
	const domainname *name = (const domainname *)"";
	ServiceInstance *si;

	(void)unusedserver; // unused
    if (client == (mach_port_t)-1) { err = mStatus_Invalid;         errormsg = "Client id -1 invalid"; goto fail; }
	DNSServiceRegistration *x = DNSServiceRegistrationList;
	while (x && x->ClientMachPort != client) x = x->next;
	if (!x)                        { err = mStatus_BadReferenceErr; errormsg = "No such client";       goto fail; }

	// Check other parameters
	if (data_len > 8192) { err = mStatus_BadParamErr; errormsg = "data_len > 8K"; goto fail; }

	for (si = x->regs; si; si = si->next)
		{
		AuthRecord *r = NULL;

		// Find the record we're updating. NULL reference means update the primary TXT record
		if (!reference) r = &si->srs.RR_TXT;
		else
			{
			ExtraResourceRecord *ptr;
			for (ptr = si->srs.Extras; ptr; ptr = ptr->next)
				{
				if ((natural_t)ptr->ClientID == reference)
					{ r = &ptr->r; break; }
				}
			if (!r) { err = mStatus_BadReferenceErr; errormsg = "No such record"; goto fail; }
			}
		err = UpdateRecord(&si->srs, client, r, data, data_len, ttl);
		if (err) goto fail;  //!!!KRS this will cause failures for non-local defaults!
		}
					
	return mStatus_NoError;

fail:
	LogMsg("%5d: DNSServiceRegistrationUpdateRecord(%##s, %X, %d) failed: %s (%ld)", client, name->c, reference, data_len, errormsg, err);
	return(err);
	}

mDNSlocal mStatus RemoveRecord(ServiceRecordSet *srs, ExtraResourceRecord *extra, mach_port_t client)
	{
	const domainname *const name = srs->RR_SRV.resrec.name;
	mStatus err = mStatus_NoError;

	// Do the operation
	LogOperation("%5d: DNSServiceRegistrationRemoveRecord(%##s)", client, srs->RR_SRV.resrec.name->c);

	err = mDNS_RemoveRecordFromService(&mDNSStorage, srs, extra, FreeExtraRR, extra);
	if (err) LogMsg("%5d: DNSServiceRegistrationRemoveRecord (%##s) failed: %d", client, name->c, err);
	
	return err;
	}

mDNSexport kern_return_t provide_DNSServiceRegistrationRemoveRecord_rpc(mach_port_t unusedserver, mach_port_t client,
	natural_t reference)
	{
	// Check client parameter
	(void)unusedserver;		// Unused
	mStatus err = mStatus_NoError;
	const char *errormsg = "Unknown";
	if (client == (mach_port_t)-1) { err = mStatus_Invalid;         errormsg = "Client id -1 invalid"; goto fail; }
	DNSServiceRegistration *x = DNSServiceRegistrationList;
	ServiceInstance *si;

	while (x && x->ClientMachPort != client) x = x->next;
	if (!x)                        { err = mStatus_BadReferenceErr; errormsg = "No such client";       goto fail; }

	for (si = x->regs; si; si = si->next)
		{
		ExtraResourceRecord *e;
		for (e = si->srs.Extras; e; e = e->next)
			{
			if ((natural_t)e->ClientID == reference)
				{
				err = RemoveRecord(&si->srs, e, client);
				break;
				}
			}
		if (!e) { err = mStatus_BadReferenceErr; errormsg = "No such reference"; goto fail; }
		}

	return mStatus_NoError;

fail:
	LogMsg("%5d: DNSServiceRegistrationRemoveRecord(%X) failed: %s (%ld)", client, reference, errormsg, err);
	return(err);
	}

//*************************************************************************************************************
// Support Code

mDNSlocal void DNSserverCallback(CFMachPortRef port, void *msg, CFIndex size, void *info)
	{
	mig_reply_error_t *request = msg;
	mig_reply_error_t *reply;
	mach_msg_return_t mr;
	int               options;
	(void)port;		// Unused
	(void)size;		// Unused
	(void)info;		// Unused

	KQueueLock(&mDNSStorage);
	
	/* allocate a reply buffer */
	reply = CFAllocatorAllocate(NULL, provide_DNSServiceDiscoveryRequest_subsystem.maxsize, 0);

	/* call the MiG server routine */
	(void) DNSServiceDiscoveryRequest_server(&request->Head, &reply->Head);

	if (!(reply->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) && (reply->RetCode != KERN_SUCCESS))
		{
        if (reply->RetCode == MIG_NO_REPLY)
			{
            /*
             * This return code is a little tricky -- it appears that the
             * demux routine found an error of some sort, but since that
             * error would not normally get returned either to the local
             * user or the remote one, we pretend it's ok.
             */
            CFAllocatorDeallocate(NULL, reply);
            goto done;
			}

        /*
         * destroy any out-of-line data in the request buffer but don't destroy
         * the reply port right (since we need that to send an error message).
         */
        request->Head.msgh_remote_port = MACH_PORT_NULL;
        mach_msg_destroy(&request->Head);
		}

    if (reply->Head.msgh_remote_port == MACH_PORT_NULL)
		{
        /* no reply port, so destroy the reply */
        if (reply->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX)
            mach_msg_destroy(&reply->Head);
        CFAllocatorDeallocate(NULL, reply);
        goto done;
		}

    /*
     * send reply.
     *
     * We don't want to block indefinitely because the client
     * isn't receiving messages from the reply port.
     * If we have a send-once right for the reply port, then
     * this isn't a concern because the send won't block.
     * If we have a send right, we need to use MACH_SEND_TIMEOUT.
     * To avoid falling off the kernel's fast RPC path unnecessarily,
     * we only supply MACH_SEND_TIMEOUT when absolutely necessary.
     */

    options = MACH_SEND_MSG;
    if (MACH_MSGH_BITS_REMOTE(reply->Head.msgh_bits) == MACH_MSG_TYPE_MOVE_SEND_ONCE)
        options |= MACH_SEND_TIMEOUT;

    mr = mach_msg(&reply->Head,		/* msg */
		      options,			/* option */
		      reply->Head.msgh_size,	/* send_size */
		      0,			/* rcv_size */
		      MACH_PORT_NULL,		/* rcv_name */
		      MACH_MSG_TIMEOUT_NONE,	/* timeout */
		      MACH_PORT_NULL);		/* notify */

    /* Has a message error occurred? */
    switch (mr)
		{
        case MACH_SEND_INVALID_DEST:
        case MACH_SEND_TIMED_OUT:
            /* the reply can't be delivered, so destroy it */
            mach_msg_destroy(&reply->Head);
            break;

        default :
            /* Includes success case. */
            break;
		}

    CFAllocatorDeallocate(NULL, reply);
	
done:
	KQueueUnlock(&mDNSStorage, "Mach client event");
	}

mDNSlocal kern_return_t registerBootstrapService()
	{
	kern_return_t status;
	mach_port_t service_send_port, service_rcv_port;

	debugf("Registering Bootstrap Service");

	/*
	 * See if our service name is already registered and if we have privilege to check in.
	 */
	status = bootstrap_check_in(bootstrap_port, (char*)kmDNSBootstrapName, &service_rcv_port);
	if (status == KERN_SUCCESS)
		{
		/*
		 * If so, we must be a followup instance of an already defined server.  In that case,
		 * the bootstrap port we inherited from our parent is the server's privilege port, so set
		 * that in case we have to unregister later (which requires the privilege port).
		 */
		server_priv_port = bootstrap_port;
		restarting_via_mach_init = TRUE;
		}
	else if (status == BOOTSTRAP_UNKNOWN_SERVICE)
		{
		status = bootstrap_create_server(bootstrap_port, "/usr/sbin/mDNSResponder", getuid(),
			FALSE /* relaunch immediately, not on demand */, &server_priv_port);
		if (status != KERN_SUCCESS) return status;

		status = bootstrap_create_service(server_priv_port, (char*)kmDNSBootstrapName, &service_send_port);
		if (status != KERN_SUCCESS)
			{
			mach_port_deallocate(mach_task_self(), server_priv_port);
			return status;
			}

		status = bootstrap_check_in(server_priv_port, (char*)kmDNSBootstrapName, &service_rcv_port);
		if (status != KERN_SUCCESS)
			{
			mach_port_deallocate(mach_task_self(), server_priv_port);
			mach_port_deallocate(mach_task_self(), service_send_port);
			return status;
			}
		assert(service_send_port == service_rcv_port);
		}

	/*
	 * We have no intention of responding to requests on the service port.  We are not otherwise
	 * a Mach port-based service.  We are just using this mechanism for relaunch facilities.
	 * So, we can dispose of all the rights we have for the service port.  We don't destroy the
	 * send right for the server's privileged bootstrap port - in case we have to unregister later.
	 */
	mach_port_destroy(mach_task_self(), service_rcv_port);
	return status;
	}

mDNSlocal kern_return_t destroyBootstrapService()
	{
	debugf("Destroying Bootstrap Service");
	return bootstrap_register(server_priv_port, (char*)kmDNSBootstrapName, MACH_PORT_NULL);
	}

mDNSlocal void ExitCallback(int signal)
	{
	(void)signal; // Unused
	LogMsgIdent(mDNSResponderVersionString, "stopping");

	debugf("ExitCallback");
	if (!mDNS_DebugMode && !started_via_launchdaemon)
		destroyBootstrapService();

	debugf("ExitCallback: Aborting MIG clients");
	while (DNSServiceDomainEnumerationList)
		AbortClient(DNSServiceDomainEnumerationList->ClientMachPort, DNSServiceDomainEnumerationList);
	while (DNSServiceBrowserList)
		AbortClient(DNSServiceBrowserList          ->ClientMachPort, DNSServiceBrowserList);
	while (DNSServiceResolverList)
		AbortClient(DNSServiceResolverList         ->ClientMachPort, DNSServiceResolverList);
	while (DNSServiceRegistrationList)
		AbortClient(DNSServiceRegistrationList     ->ClientMachPort, DNSServiceRegistrationList);

	debugf("ExitCallback: mDNS_Close");
	mDNS_Close(&mDNSStorage);
	if (udsserver_exit(launchd_fd) < 0) LogMsg("ExitCallback: udsserver_exit failed");
	exit(0);
	}

// Send a mach_msg to ourselves (since that is signal safe) telling us to cleanup and exit
mDNSlocal void HandleSIG(int signal)
	{
	debugf(" ");
	debugf("HandleSIG %d", signal);
	mach_msg_header_t header;
	header.msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_MAKE_SEND, 0);
	header.msgh_remote_port = signal_port;
	header.msgh_local_port = MACH_PORT_NULL;
	header.msgh_size = sizeof(header);
	header.msgh_id = signal;
	if (mach_msg_send(&header) != MACH_MSG_SUCCESS)
		{
		LogMsg("HandleSIG %d: mach_msg_send failed", signal);
		if (signal == SIGTERM || signal == SIGINT) exit(-1);
		}
	}

mDNSlocal void INFOCallback(void)
	{
	mDNSs32 utc = mDNSPlatformUTC();
	DNSServiceDomainEnumeration *e;
	DNSServiceBrowser           *b;
	DNSServiceResolver          *l;
	DNSServiceRegistration      *r;
	NetworkInterfaceInfoOSX     *i;
	DNSServer *s;

	LogMsgIdent(mDNSResponderVersionString, "---- BEGIN STATE LOG ----");
	
	udsserver_info(&mDNSStorage);

	for (e = DNSServiceDomainEnumerationList; e; e=e->next)
		LogMsgNoIdent("%5d: Mach DomainEnumeration   %##s", e->ClientMachPort, e->dom.qname.c);

	for (b = DNSServiceBrowserList; b; b=b->next)
		{
		DNSServiceBrowserQuestion *qptr;
		for (qptr = b->qlist; qptr; qptr = qptr->next)
			LogMsgNoIdent("%5d: Mach ServiceBrowse       %##s", b->ClientMachPort, qptr->q.qname.c);
		}
	for (l = DNSServiceResolverList; l; l=l->next)
		LogMsgNoIdent("%5d: Mach ServiceResolve      %##s", l->ClientMachPort, l->i.name.c);

	for (r = DNSServiceRegistrationList; r; r=r->next)
		{
		ServiceInstance *si;
		for (si = r->regs; si; si = si->next)
			LogMsgNoIdent("%5d: Mach ServiceInstance     %##s %u", si->ClientMachPort, si->srs.RR_SRV.resrec.name->c, mDNSVal16(si->srs.RR_SRV.resrec.rdata->u.srv.port));
		}

	LogMsgNoIdent("------ Network Interfaces ------");
	for (i = mDNSStorage.p->InterfaceList; i; i = i->next)
		{
		if (!i->Exists)
			LogMsgNoIdent("Interface: %s %5s(%lu) %.6a %#a dormant for %d seconds",
				i->sa_family == AF_INET ? "v4" : i->sa_family == AF_INET6 ? "v6" : "??", i->ifa_name, i->scope_id, &i->BSSID,
				&i->ifinfo.ip, utc - i->LastSeen);
		else
			LogMsgNoIdent("Interface: %s %5s(%lu) %.6a %s %s %-15.4a %s InterfaceID %p %s %s %#a",
				i->sa_family == AF_INET ? "v4" : i->sa_family == AF_INET6 ? "v6" : "??", i->ifa_name, i->scope_id, &i->BSSID,
				i->ifinfo.InterfaceActive ? "Active" : "      ",
				i->ifinfo.IPv4Available ? "v4" : "  ",
				i->ifinfo.IPv4Available ? (mDNSv4Addr*)&i->ifa_v4addr : &zerov4Addr,
				i->ifinfo.IPv6Available ? "v6" : "  ",
				i->ifinfo.InterfaceID,
				i->ifinfo.Advertise ? "Adv"  : "   ",
				i->ifinfo.McastTxRx ? "TxRx" : "    ",
				&i->ifinfo.ip);
		}

	for (s = mDNSStorage.DNSServers; s; s = s->next)
		{
		NetworkInterfaceInfoOSX *ifx = (NetworkInterfaceInfoOSX *)s->interface;
		LogMsgNoIdent("DNS Server %##s %s%s%#a:%d %s",
			s->domain.c, ifx ? ifx->ifa_name : "", ifx ? " " : "", &s->addr, mDNSVal16(s->port),
			s->teststate == DNSServer_Untested ? "(Untested)" :
			s->teststate == DNSServer_Passed   ? ""           :
			s->teststate == DNSServer_Failed   ? "(Failed)"   :
			s->teststate == DNSServer_Disabled ? "(Disabled)" : "(Unknown state)");
		}

	mDNSs32 now = mDNS_TimeNow(&mDNSStorage);
	LogMsgNoIdent("Timenow 0x%08lX (%ld)", (mDNSu32)now, now);

	LogMsgIdent(mDNSResponderVersionString, "----  END STATE LOG  ----");
	}

mDNSlocal void SignalCallback(CFMachPortRef port, void *msg, CFIndex size, void *info)
	{
	(void)port;		// Unused
	(void)size;		// Unused
	(void)info;		// Unused
	mach_msg_header_t *msg_header = (mach_msg_header_t *)msg;
	KQueueLock(&mDNSStorage);
	switch(msg_header->msgh_id)
		{
		case SIGHUP:	{
						mDNS *m = &mDNSStorage;
						mDNSu32 slot;
						CacheGroup *cg;
						CacheRecord *rr;
						LogMsg("SIGHUP: Purge cache");
						FORALL_CACHERECORDS(slot, cg, rr) mDNS_PurgeCacheResourceRecord(m, rr);
						} break;
		case SIGINT:
		case SIGTERM:	ExitCallback(msg_header->msgh_id); break;
		case SIGINFO:	INFOCallback(); break;
		case SIGUSR1:	LogMsg("SIGUSR1: Simulate Network Configuration Change Event");
						mDNSMacOSXNetworkChanged(&mDNSStorage); break;
		case SIGUSR2:	SigLogLevel(); break;
		default: LogMsg("SignalCallback: Unknown signal %d", msg_header->msgh_id); break;
		}
	KQueueUnlock(&mDNSStorage, "Unix Signal");
	}

// On 10.2 the MachServerName is DNSServiceDiscoveryServer
// On 10.3 and later, the MachServerName is com.apple.mDNSResponder

mDNSlocal kern_return_t mDNSDaemonInitialize(void)
	{
	mStatus            err;
	CFMachPortRef      s_port;

	CFS_OQ               = CFStringCreateWithCString(NULL, "“",  kCFStringEncodingUTF8);
	CFS_CQ               = CFStringCreateWithCString(NULL, "”",  kCFStringEncodingUTF8);
	CFS_Format           = CFStringCreateWithCString(NULL, "%@%s\xEF\xBB\xBF", kCFStringEncodingUTF8);
	CFS_ComputerName     = CFStringCreateWithCString(NULL, "The name of your computer ",  kCFStringEncodingUTF8);
	CFS_ComputerNameMsg  = CFStringCreateWithCString(NULL, "To change the name of your computer, "
		"open System Preferences and click Sharing, then type the name in the Computer Name field.",  kCFStringEncodingUTF8);
	CFS_LocalHostName    = CFStringCreateWithCString(NULL, "This computer’s local hostname ",  kCFStringEncodingUTF8);
	CFS_LocalHostNameMsg = CFStringCreateWithCString(NULL, "To change the local hostname, "
		"open System Preferences and click Sharing, then click “Edit” and type the name in the Local Hostname field.",  kCFStringEncodingUTF8);

	// If launchd already created our Mach port for us, then use that, else we create a new one of our own
	if (m_port != MACH_PORT_NULL)
		s_port = CFMachPortCreateWithPort(NULL, m_port, DNSserverCallback, NULL, NULL);
	else
		{
		s_port = CFMachPortCreate(NULL, DNSserverCallback, NULL, NULL);
		m_port = CFMachPortGetPort(s_port);
		char *MachServerName = OSXVers < 7 ? "DNSServiceDiscoveryServer" : "com.apple.mDNSResponder";
		kern_return_t      status = bootstrap_register(bootstrap_port, MachServerName, m_port);
	
		if (status)
			{
			if (status == 1103)
				LogMsg("Bootstrap_register failed(): A copy of the daemon is apparently already running");
			else
				LogMsg("Bootstrap_register failed(): %s %d", mach_error_string(status), status);
			return(status);
			}
		}

	CFMachPortRef      d_port = CFMachPortCreate(NULL, ClientDeathCallback, NULL, NULL);
	CFMachPortRef      i_port = CFMachPortCreate(NULL, SignalCallback, NULL, NULL);
	CFRunLoopSourceRef d_rls  = CFMachPortCreateRunLoopSource(NULL, d_port, 0);
	CFRunLoopSourceRef s_rls  = CFMachPortCreateRunLoopSource(NULL, s_port, 0);
	CFRunLoopSourceRef i_rls  = CFMachPortCreateRunLoopSource(NULL, i_port, 0);

	err = mDNS_Init(&mDNSStorage, &PlatformStorage,
		rrcachestorage, RR_CACHE_SIZE,
		mDNS_Init_AdvertiseLocalAddresses,
		mDNS_StatusCallback, mDNS_Init_NoInitCallbackContext);

	if (err) { LogMsg("Daemon start: mDNS_Init failed %ld", err); return(err); }

	gNotificationUserHostLabel = gNotificationPrefHostLabel = PlatformStorage.userhostlabel;
	gNotificationUserNiceLabel = gNotificationPrefNiceLabel = PlatformStorage.usernicelabel;

	client_death_port = CFMachPortGetPort(d_port);
	signal_port       = CFMachPortGetPort(i_port);

	CFRunLoop = CFRunLoopGetCurrent();
	CFRunLoopAddSource(CFRunLoop, d_rls, kCFRunLoopDefaultMode);
	CFRunLoopAddSource(CFRunLoop, s_rls, kCFRunLoopDefaultMode);
	CFRunLoopAddSource(CFRunLoop, i_rls, kCFRunLoopDefaultMode);
	CFRelease(d_rls);
	CFRelease(s_rls);
	CFRelease(i_rls);
	if (mDNS_DebugMode) printf("Service registered with Mach Port %d\n", m_port);
	return(err);
	}

mDNSlocal mDNSs32 mDNSDaemonIdle(mDNS *const m)
	{
	mDNSs32 now = mDNS_TimeNow(m);

	// 1. If we have network change events to handle, do them FIRST, before calling mDNS_Execute()
	// Detailed reason:
	// mDNSMacOSXNetworkChanged() currently closes and re-opens its sockets. If there are received packets waiting, they are lost.
	// mDNS_Execute() generates packets, including multicasts that are looped back to ourself.
	// If we call mDNS_Execute() first, and generate packets, and then call mDNSMacOSXNetworkChanged() immediately afterwards
	// we then systematically lose our own looped-back packets.
	if (m->p->NetworkChanged && now - m->p->NetworkChanged >= 0) mDNSMacOSXNetworkChanged(m);

	// 2. Call mDNS_Execute() to let mDNSCore do what it needs to do
	mDNSs32 nextevent = mDNS_Execute(m);

	if (m->p->NetworkChanged)
		if (nextevent - m->p->NetworkChanged > 0)
			nextevent = m->p->NetworkChanged;

	// 3. Deliver any waiting browse messages to clients
	DNSServiceBrowser *b = DNSServiceBrowserList;

	while (b)
		{
		// NOTE: Need to advance b to the next element BEFORE we call DeliverInstance(), because in the
		// event that the client Mach queue overflows, DeliverInstance() will call AbortBlockedClient()
		// and that will cause the DNSServiceBrowser object's memory to be freed before it returns
		DNSServiceBrowser *x = b;
		b = b->next;
		if (x->results)			// Try to deliver the list of results
			{
			while (x->results)
				{
				DNSServiceBrowserResult *const r = x->results;
				domainlabel name;
				domainname type, domain;
				DeconstructServiceName(&r->result, &name, &type, &domain);	// Don't need to check result; already validated in FoundInstance()
				char cname[MAX_DOMAIN_LABEL+1];			// Unescaped name: up to 63 bytes plus C-string terminating NULL.
				char ctype[MAX_ESCAPED_DOMAIN_NAME];
				char cdom [MAX_ESCAPED_DOMAIN_NAME];
				ConvertDomainLabelToCString_unescaped(&name, cname);
				ConvertDomainNameToCString(&type, ctype);
				ConvertDomainNameToCString(&domain, cdom);
				DNSServiceDiscoveryReplyFlags flags = (r->next) ? DNSServiceDiscoverReplyFlagsMoreComing : 0;
				kern_return_t status = DNSServiceBrowserReply_rpc(x->ClientMachPort, r->resultType, cname, ctype, cdom, flags, 1);
				// If we failed to send the mach message, try again in one second
				if (status == MACH_SEND_TIMED_OUT)
					{
					if (nextevent - now > mDNSPlatformOneSecond)
						nextevent = now + mDNSPlatformOneSecond;
					break;
					}
				else
					{
					x->lastsuccess = now;
					x->results = x->results->next;
					freeL("DNSServiceBrowserResult", r);
					}
				}
			// If this client hasn't read a single message in the last 60 seconds, abort it
			if (now - x->lastsuccess >= 60 * mDNSPlatformOneSecond)
				AbortBlockedClient(x->ClientMachPort, "browse", x);
			}
		}

	DNSServiceResolver *l;
	for (l = DNSServiceResolverList; l; l=l->next)
		if (l->ReportTime && now - l->ReportTime >= 0)
			{
			l->ReportTime = 0;
			LogMsgNoIdent("Client application bug: DNSServiceResolver(%##s) active for over two minutes. "
				"This places considerable burden on the network.", l->i.name.c);
			}

	if (m->p->NotifyUser)
		{
		if (m->p->NotifyUser - now < 0)
			{
			if (!SameDomainLabelCS(m->p->usernicelabel.c, m->nicelabel.c))
				{
				LogMsg("Name Conflict: Updated Computer Name from \"%#s\" to \"%#s\"", m->p->usernicelabel.c, m->nicelabel.c);
				gNotificationPrefNiceLabel = m->p->usernicelabel = m->nicelabel;
				RecordUpdatedName(m, &gNotificationUserNiceLabel, &gNotificationPrefNiceLabel, CFS_ComputerName, "", CFS_ComputerNameMsg);
				// Clear m->p->NotifyUser here -- even if the hostlabel has changed too, we don't want to bug the user with *two* alerts
				m->p->NotifyUser = 0;
				}
			if (!SameDomainLabelCS(m->p->userhostlabel.c, m->hostlabel.c))
				{
				LogMsg("Name Conflict: Updated Local Hostname from \"%#s.local\" to \"%#s.local\"", m->p->userhostlabel.c, m->hostlabel.c);
				m->p->HostNameConflict = 0;	// Clear our indicator, now name change has been successful
				gNotificationPrefHostLabel = m->p->userhostlabel = m->hostlabel;
				RecordUpdatedName(m, &gNotificationUserHostLabel, &gNotificationPrefHostLabel, CFS_LocalHostName, ".local", CFS_LocalHostNameMsg);
				}
			m->p->NotifyUser = 0;
			}
		else
			if (nextevent - m->p->NotifyUser > 0)
				nextevent = m->p->NotifyUser;
		}

	return(nextevent);
	}

mDNSlocal void ShowTaskSchedulingError(mDNS *const m)
	{
	mDNS_Lock(m);

	LogMsg("Task Scheduling Error: Continuously busy for more than a second");
	
	// NOTE: To accurately diagnose *why* we're busy, the debugging code here to show needs to mirror the logic in GetNextScheduledEvent

	if (m->NewQuestions && (!m->NewQuestions->DelayAnswering || m->timenow - m->NewQuestions->DelayAnswering >= 0))
		LogMsg("Task Scheduling Error: NewQuestion %##s (%s)",
			m->NewQuestions->qname.c, DNSTypeName(m->NewQuestions->qtype));
	if (m->NewLocalOnlyQuestions)
		LogMsg("Task Scheduling Error: NewLocalOnlyQuestions %##s (%s)",
			m->NewLocalOnlyQuestions->qname.c, DNSTypeName(m->NewLocalOnlyQuestions->qtype));
	if (m->NewLocalRecords && LocalRecordReady(m->NewLocalRecords))
		LogMsg("Task Scheduling Error: NewLocalRecords %s", ARDisplayString(m, m->NewLocalRecords));
	if (m->SuppressSending && m->timenow - m->SuppressSending >= 0)
		LogMsg("Task Scheduling Error: m->SuppressSending %d",       m->timenow - m->SuppressSending);
#ifndef UNICAST_DISABLED
	if (m->timenow - m->NextuDNSEvent         >= 0)
		LogMsg("Task Scheduling Error: NextuDNSEvent %d",            m->timenow - m->NextuDNSEvent);
#endif
	if (m->timenow - m->NextCacheCheck        >= 0)
		LogMsg("Task Scheduling Error: m->NextCacheCheck %d",        m->timenow - m->NextCacheCheck);
	if (m->timenow - m->NextScheduledQuery    >= 0)
		LogMsg("Task Scheduling Error: m->NextScheduledQuery %d",    m->timenow - m->NextScheduledQuery);
	if (m->timenow - m->NextScheduledProbe    >= 0)
		LogMsg("Task Scheduling Error: m->NextScheduledProbe %d",    m->timenow - m->NextScheduledProbe);
	if (m->timenow - m->NextScheduledResponse >= 0)
		LogMsg("Task Scheduling Error: m->NextScheduledResponse %d", m->timenow - m->NextScheduledResponse);
	if (m->timenow - m->NextScheduledNATOp >= 0)
		LogMsg("Task Scheduling Error: m->NextScheduledNATOp %d",    m->timenow - m->NextScheduledNATOp);

	mDNS_Unlock(&mDNSStorage);
	}

mDNSlocal void KQWokenFlushBytes(int fd, __unused short filter, __unused void *context)
	{
	// Read all of the bytes so we won't wake again.
	char    buffer[100];
	ssize_t read = sizeof(buffer);
	while (read > 0) read = recv(fd, buffer, sizeof(buffer), MSG_DONTWAIT);
	}

mDNSlocal void * KQueueLoop(void *m_param)
	{
	mDNS            *m = m_param;
	int             numevents = 0;

#if USE_SELECT_WITH_KQUEUEFD
	fd_set          readfds;
	FD_ZERO(&readfds);
	const int multiplier = 1000000    / mDNSPlatformOneSecond;
#else
	const int multiplier = 1000000000 / mDNSPlatformOneSecond;
#endif
	
	pthread_mutex_lock(&PlatformStorage.BigMutex);
	LogOperation("Starting time value 0x%08lX (%ld)", (mDNSu32)mDNSStorage.timenow_last, mDNSStorage.timenow_last);
	
	// This is the main work loop:
	// (1) First we give mDNSCore a chance to finish off any of its deferred work and calculate the next sleep time
	// (2) Then we make sure we've delivered all waiting browse messages to our clients
	// (3) Then we sleep for the time requested by mDNSCore, or until the next event, whichever is sooner
	// (4) On wakeup we first process *all* events
	// (5) then when no more events remain, we go back to (1) to finish off any deferred work and do it all again
	for ( ; ; )
		{
		#define kEventsToReadAtOnce 1
		struct kevent new_events[kEventsToReadAtOnce];

		// Run mDNS_Execute to find out the time we next need to wake up
		mDNSs32 start          = mDNSPlatformRawTime();
		mDNSs32 nextTimerEvent = udsserver_idle(mDNSDaemonIdle(m));
		mDNSs32 end            = mDNSPlatformRawTime();
		if (end - start >= WatchDogReportingThreshold)
			LogOperation("WARNING: Idle task took %dms to complete", end - start);
		
		// Convert absolute wakeup time to a relative time from now
		mDNSs32 ticks = nextTimerEvent - mDNS_TimeNow(m);
		if (ticks < 1) ticks = 1;
		
		static mDNSs32 RepeatedBusy = 0;	// Debugging sanity check, to guard against CPU spins
		if (ticks > 1)
			RepeatedBusy = 0;
		else
			{
			ticks = 1;
			if (++RepeatedBusy >= mDNSPlatformOneSecond) { ShowTaskSchedulingError(&mDNSStorage); RepeatedBusy = 0; }
			}

		verbosedebugf("KQueueLoop: Handled %d events; now sleeping for %d ticks", numevents, ticks);
		numevents = 0;

		// Release the lock, and sleep until:
		// 1. Something interesting happens like a packet arriving, or
		// 2. The other thread writes a byte to WakeKQueueLoopFD to poke us and make us wake up, or
		// 3. The timeout expires
		pthread_mutex_unlock(&PlatformStorage.BigMutex);

#if USE_SELECT_WITH_KQUEUEFD
		struct timeval timeout;
		timeout.tv_sec = ticks / mDNSPlatformOneSecond;
		timeout.tv_usec = (ticks % mDNSPlatformOneSecond) * multiplier;
		FD_SET(KQueueFD, &readfds);
		if (select(KQueueFD+1, &readfds, NULL, NULL, &timeout) < 0)
			{ LogMsg("select(%d) failed errno %d (%s)", KQueueFD, errno, strerror(errno)); sleep(1); }
#else
		struct timespec timeout;
		timeout.tv_sec = ticks / mDNSPlatformOneSecond;
		timeout.tv_nsec = (ticks % mDNSPlatformOneSecond) * multiplier;
		// In my opinion, you ought to be able to call kevent() with nevents set to zero,
		// and have it work similarly to the way it does with nevents non-zero --
		// i.e. it waits until either an event happens or the timeout expires, and then wakes up.
		// In fact, what happens if you do this is that it just returns immediately. So, we have
		// to pass nevents set to one, and then we just ignore the event it gives back to us. -- SC
		if (kevent(KQueueFD, NULL, 0, new_events, 1, &timeout) < 0)
			{ LogMsg("kevent(%d) failed errno %d (%s)", KQueueFD, errno, strerror(errno)); sleep(1); }
#endif

		pthread_mutex_lock(&PlatformStorage.BigMutex);
		// We have to ignore the event we may have been told about above, because that
		// was done without holding the lock, and between the time we woke up and the
		// time we reclaimed the lock the other thread could have done something that
		// makes the event no longer valid. Now we have the lock, we call kevent again
		// and this time we can safely process the events it tells us about.

		static const struct timespec zero_timeout = { 0, 0 };
		int events_found;
		while ((events_found = kevent(KQueueFD, NULL, 0, new_events, kEventsToReadAtOnce, &zero_timeout)) != 0)
			{
			if (events_found > kEventsToReadAtOnce || (events_found < 0 && errno != EINTR))
				{
				// Not sure what to do here, our kqueue has failed us - this isn't ideal
				LogMsg("ERROR: KQueueLoop - kevent failed errno %d (%s)", errno, strerror(errno));
				exit(errno);
				}

			numevents += events_found;

			int i;
			for (i = 0; i < events_found; i++)
				{
				const KQueueEntry *const kqentry = new_events[i].udata;
				mDNSs32 start = mDNSPlatformRawTime();
				const char *const KQtask = kqentry->KQtask;	// Grab a copy in case KQcallback deletes the task
				(void)KQtask;
				kqentry->KQcallback(new_events[i].ident, new_events[i].filter, kqentry->KQcontext);
				mDNSs32 end   = mDNSPlatformRawTime();
				if (end - start >= WatchDogReportingThreshold)
					LogOperation("WARNING: %s took %dms to complete", KQtask, end - start);
				}
			}
		}
	
	return NULL;
	}

mDNSlocal void LaunchdCheckin(void)
	{
	launch_data_t msg  = launch_data_new_string(LAUNCH_KEY_CHECKIN);
	launch_data_t resp = launch_msg(msg);
	launch_data_free(msg);
	if (!resp) { LogMsg("launch_msg returned NULL"); return; }

	if (launch_data_get_type(resp) == LAUNCH_DATA_ERRNO)
		{
		int err = launch_data_get_errno(resp);
		// When running on Tiger with "ServiceIPC = false", we get "err == EACCES" to tell us there's no launchdata to fetch
		if (err != EACCES) LogMsg("launch_msg returned %d", err);
		else LogOperation("Launchd provided no launchdata; will open Mach port and Unix Domain Socket explicitly...", err);
		}
	else
		{
		launch_data_t skts = launch_data_dict_lookup(resp, LAUNCH_JOBKEY_SOCKETS);
		if (!skts) LogMsg("launch_data_dict_lookup LAUNCH_JOBKEY_SOCKETS returned NULL");
		else
			{
			launch_data_t skt = launch_data_dict_lookup(skts, "Listeners");
			if (!skt) LogMsg("launch_data_dict_lookup Listeners returned NULL");
			else
				{
				launch_data_t s = launch_data_array_get_index(skt, 0);
				if (!s) LogMsg("launch_data_array_get_index(skt, 0) returned NULL");
				else
					{
					launchd_fd = launch_data_get_fd(s);
					LogOperation("Launchd Unix Domain Socket: %d", launchd_fd);
					// In some early versions of 10.4.x, the permissions on the UDS were not set correctly, so we fix them here
					chmod(MDNS_UDS_SERVERPATH, S_IRUSR|S_IWUSR | S_IRGRP|S_IWGRP | S_IROTH|S_IWOTH);
					}
				}
			}

		launch_data_t ports = launch_data_dict_lookup(resp, "MachServices");
		if (!ports) LogMsg("launch_data_dict_lookup MachServices returned NULL");
		else
			{
			launch_data_t p = launch_data_dict_lookup(ports, "com.apple.mDNSResponder");
			if (!p) LogOperation("launch_data_array_get_index(ports, 0) returned NULL");
			else
				{
				m_port = launch_data_get_fd(p);
				LogOperation("Launchd Mach Port: %d", m_port);
				if (m_port == ~0U) m_port = MACH_PORT_NULL;
				}
			}
		}
	launch_data_free(resp);
	}

mDNSlocal void DropPrivileges(void)
	{
	LogMsg("Started as root.  Dropping privileges.");
	static const char login[] = "_mdnsresponder";
	struct passwd *pwd = getpwnam(login);
	if (NULL == pwd)
		LogMsg("Could not find account name \"%s\".  Continuing as root after all.", login);
	else
		{
		uid_t uid = pwd->pw_uid;
		gid_t gid = pwd->pw_gid;

		if (unlink(MDNS_UDS_SERVERPATH) < 0 && errno != ENOENT) LogMsg("DropPrivileges: Could not unlink \"%s\": (%d) %s", MDNS_UDS_SERVERPATH, errno, strerror(errno));
		else
			{
			static char path[] = "/var/run/mdns/mDNSResponder";
			char *p = strrchr(path, '/');
			*p = '\0';
			if (mkdir(path, 0755) < 0 && errno != EEXIST) LogMsg("DropPrivileges: Could not create directory \"%s\": (%d) %s", path, errno, strerror(errno));
			else if (chown(path, uid, gid) < 0) LogMsg("DropPrivileges: Could not chown directory \"%s\": (%d) %s", path, errno, strerror(errno));
			else
				{
				*p = '/';
				if (unlink(path) < 0 && errno != ENOENT) LogMsg("DropPrivileges: Could not unlink \"%s\": (%d) %s", path, errno, strerror(errno));
				else if (symlink(path, MDNS_UDS_SERVERPATH) < 0) LogMsg("DropPrivileges: Could not symlink \"%s\" -> \"%s\": (%d) %s", MDNS_UDS_SERVERPATH, path, errno, strerror(errno));
				else LogOperation("DropPrivileges: Created subdirectory and symlink");
				}
			}

		if (0 != initgroups(login, gid)) LogMsg("initgroups(\"%s\", %lu) failed.  Continuing.", login,        (unsigned long)gid);
		if (0 != setgid(gid))            LogMsg("setgid(%lu) failed.  Continuing with group %lu privileges.", (unsigned long)getegid());
		if (0 != setuid(uid))            LogMsg("setuid(%lu) failed. Continuing as root after all.",          (unsigned long)uid);
		}
	}

extern int sandbox_init(const char *profile, uint64_t flags, char **errorbuf) __attribute__((weak_import));

mDNSexport int main(int argc, char **argv)
	{
	int i;
	kern_return_t status;
	pthread_t KQueueThread;

	for (i=1; i<argc; i++)
		{
		if (!strcasecmp(argv[i], "-d"           )) mDNS_DebugMode = mDNStrue;
		if (!strcasecmp(argv[i], "-launchd"     )) started_via_launchdaemon = mDNStrue;
		if (!strcasecmp(argv[i], "-launchdaemon")) started_via_launchdaemon = mDNStrue;
		}

	if (0 == geteuid()) DropPrivileges();

	signal(SIGHUP,  HandleSIG);		// (Debugging) Purge the cache to check for cache handling bugs
	signal(SIGINT,  HandleSIG);		// Ctrl-C: Detach from Mach BootstrapService and exit cleanly
	signal(SIGPIPE, SIG_IGN  );		// Don't want SIGPIPE signals -- we'll handle EPIPE errors directly
	signal(SIGTERM, HandleSIG);		// Machine shutting down: Detach from and exit cleanly like Ctrl-C
	signal(SIGINFO, HandleSIG);		// (Debugging) Write state snapshot to syslog
	signal(SIGUSR1, HandleSIG);		// (Debugging) Simulate network change notification from System Configuration Framework
	signal(SIGUSR2, HandleSIG);		// (Debugging) Change log level

	mDNSStorage.p = &PlatformStorage;	// Make sure mDNSStorage.p is set up, because validatelists uses it
	LogMsgIdent(mDNSResponderVersionString, "starting");
	LaunchdCheckin();

	// Register the server with mach_init for automatic restart only during normal (non-debug) mode
    if (!mDNS_DebugMode && !started_via_launchdaemon)
    	{
    	registerBootstrapService();
    	if (!restarting_via_mach_init) exit(0); // mach_init will restart us immediately as a daemon
		int fd = open(_PATH_DEVNULL, O_RDWR, 0);
		if (fd < 0) LogMsg("open(_PATH_DEVNULL, O_RDWR, 0) failed errno %d (%s)", errno, strerror(errno));
		else
			{
			// Avoid unnecessarily duplicating a file descriptor to itself
			if (fd != STDIN_FILENO)  if (dup2(fd, STDIN_FILENO)  < 0) LogMsg("dup2(fd, STDIN_FILENO)  failed errno %d (%s)", errno, strerror(errno));
			if (fd != STDOUT_FILENO) if (dup2(fd, STDOUT_FILENO) < 0) LogMsg("dup2(fd, STDOUT_FILENO) failed errno %d (%s)", errno, strerror(errno));
			if (fd != STDERR_FILENO) if (dup2(fd, STDERR_FILENO) < 0) LogMsg("dup2(fd, STDERR_FILENO) failed errno %d (%s)", errno, strerror(errno));
			if (fd != STDIN_FILENO && fd != STDOUT_FILENO && fd != STDERR_FILENO) (void)close(fd);
			}
		}

	// Create the kqueue, mutex and thread to support KQSockets
	KQueueFD = kqueue();
	if (KQueueFD == -1) { LogMsg("kqueue() failed errno %d (%s)", errno, strerror(errno)); status = errno; goto exit; }
	
	i = pthread_mutex_init(&PlatformStorage.BigMutex, NULL);
	if (i == -1) { LogMsg("pthread_mutex_init() failed errno %d (%s)", errno, strerror(errno)); status = errno; goto exit; }
	
	int fdpair[2] = {0, 0};
	i = socketpair(AF_UNIX, SOCK_STREAM, 0, fdpair);
	if (i == -1) { LogMsg("socketpair() failed errno %d (%s)", errno, strerror(errno)); status = errno; goto exit; }

	// Socket pair returned us two identical sockets connected to each other
	// We will use the first socket to send the second socket. The second socket
	// will be added to the kqueue so it will wake when data is sent.
	static const KQueueEntry wakeKQEntry = { KQWokenFlushBytes, NULL, "kqueue wakeup after CFRunLoop event" };
	PlatformStorage.WakeKQueueLoopFD = fdpair[0];
	KQueueSet(fdpair[1], EV_ADD, EVFILT_READ, &wakeKQEntry);
	
	// Invoke sandbox profile /usr/share/sandbox/mDNSResponder.sb
#if MDNS_NO_SANDBOX
	LogMsg("Note: Compiled without Apple Sandbox support");
#else
	if (!sandbox_init)
		LogMsg("Note: Running without Apple Sandbox support (not available on this OS)");
	else
		{
		char *sandbox_msg;
		int sandbox_err = sandbox_init("mDNSResponder", SANDBOX_NAMED, &sandbox_msg);
		if (sandbox_err) { LogMsg("WARNING: sandbox_init error %s", sandbox_msg); sandbox_free_error(sandbox_msg); }
		else LogOperation("Now running under Apple Sandbox restrictions");
		}
#endif

	OSXVers = mDNSMacOSXSystemBuildNumber(NULL);
	status = mDNSDaemonInitialize();
	if (status) { LogMsg("Daemon start: mDNSDaemonInitialize failed"); goto exit; }
	status = udsserver_init(launchd_fd);
	if (status) { LogMsg("Daemon start: udsserver_init failed"); goto exit; }
	
	// Start the kqueue thread
	i = pthread_create(&KQueueThread, NULL, KQueueLoop, &mDNSStorage);
	if (i == -1) { LogMsg("pthread_create() failed errno %d (%s)", errno, strerror(errno)); status = errno; goto exit; }

	if (status == 0)
		{
		CFRunLoopRun();
		LogMsg("ERROR: CFRunLoopRun Exiting.");
		mDNS_Close(&mDNSStorage);
		}

	LogMsgIdent(mDNSResponderVersionString, "exiting");

exit:
	if (!mDNS_DebugMode && !started_via_launchdaemon) destroyBootstrapService();
	return(status);
	}

// uds_daemon.c support routines /////////////////////////////////////////////

// Arrange things so that callback is called with context when data appears on fd
mStatus udsSupportAddFDToEventLoop(int fd, udsEventCallback callback, void *context)
	{
	KQSocketEventSource *newSource = (KQSocketEventSource*) mallocL("KQSocketEventSource", sizeof *newSource);
	if (!newSource) return mStatus_NoMemoryErr;

	mDNSPlatformMemZero(newSource, sizeof(*newSource));
	newSource->fd = fd;
	newSource->kqs.KQcallback = callback;
	newSource->kqs.KQcontext  = context;
	newSource->kqs.KQtask     = "UDS client";

	if (KQueueSet(fd, EV_ADD, EVFILT_READ, &newSource->kqs) == 0)
		{
		KQSocketEventSource **p = &gEventSources;
		while (*p) p = &(*p)->next;
		*p = newSource;
		return mStatus_NoError;
		}
	else
		{
		close(fd);
		free(newSource);
		return mStatus_NoMemoryErr;
		}
	}

mStatus udsSupportRemoveFDFromEventLoop(int fd)		// Note: This also CLOSES the file descriptor
	{
	KQSocketEventSource **p = &gEventSources;
	while (*p && (*p)->fd != fd) p = &(*p)->next;
	if (*p) 
		{
		KQSocketEventSource *s = *p;
		*p = (*p)->next;
		close(s->fd);
		freeL("KQSocketEventSource", s);
		return mStatus_NoError;
		}
	return mStatus_NoSuchNameErr;
	}

// If mDNSResponder crashes, then this string will be magically included in the automatically-generated crash log
const char *__crashreporter_info__ = mDNSResponderVersionString;
asm(".desc ___crashreporter_info__, 0x10");

// For convenience when using the "strings" command, this is the last thing in the file
mDNSexport const char mDNSResponderVersionString_SCCS[] = "@(#) " STRINGIFY(mDNSResponderVersion) " (" __DATE__ " " __TIME__ ")";
