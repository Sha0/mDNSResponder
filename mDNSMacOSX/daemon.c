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
Revision 1.180  2004/07/13 21:24:25  rpantos
Fix for <rdar://problem/3701120>.

Revision 1.179  2004/06/19 00:02:54  cheshire
Restore fix for <rdar://problem/3548256> Should not allow empty string for resolve domain

Revision 1.178  2004/06/18 19:10:00  cheshire
<rdar://problem/3588761> Current method of doing subtypes causes name collisions

Revision 1.177  2004/06/16 23:14:46  ksekar
<rdar://problem/3693816> Remove fix for <rdar://problem/3548256> Should not allow empty string for resolve domain

Revision 1.176  2004/06/11 20:27:42  cheshire
Rename "SocketRef" as "cfs" to avoid conflict with other plaforms

Revision 1.175  2004/06/10 20:23:21  cheshire
Also list interfaces in SIGINFO output

Revision 1.174  2004/06/08 18:54:48  ksekar
<rdar://problem/3681378>: mDNSResponder leaks after exploring in Printer Setup Utility

Revision 1.173  2004/06/08 17:35:12  cheshire
<rdar://problem/3683988> Detect and report if mDNSResponder uses too much CPU

Revision 1.172  2004/06/05 00:04:26  cheshire
<rdar://problem/3668639>: wide-area domains should be returned in reg. domain enumeration

Revision 1.171  2004/06/04 08:58:30  ksekar
<rdar://problem/3668624>: Keychain integration for secure dynamic update

Revision 1.170  2004/05/30 20:01:50  ksekar
<rdar://problem/3668635>: wide-area default registrations should be in
.local too - fixed service registration when clients pass an explicit
domain (broken by previous checkin)

Revision 1.169  2004/05/30 01:30:16  ksekar
<rdar://problem/3668635>: wide-area default registrations should be in
.local too

Revision 1.168  2004/05/18 23:51:26  cheshire
Tidy up all checkin comments to use consistent "<rdar://problem/xxxxxxx>" format for bug numbers

Revision 1.167  2004/05/14 16:39:47  ksekar
Browse for iChat locally for now.

Revision 1.166  2004/05/13 21:33:52  ksekar
Clean up non-local registration control via config file.  Force iChat
registrations to be local for now.

Revision 1.165  2004/05/13 04:54:20  ksekar
Unified list copy/free code.  Added symetric list for

Revision 1.164  2004/05/12 22:03:08  ksekar
Made GetSearchDomainList a true platform-layer call (declaration moved
from mDNSMacOSX.h to mDNSClientAPI.h), impelemted to return "local"
only on non-OSX platforms.  Changed call to return a copy of the list
to avoid shared memory issues.  Added a routine to free the list.

Revision 1.163  2004/05/12 02:03:25  ksekar
Non-local domains will only be browsed by default, and show up in
_browse domain enumeration, if they contain an _browse._dns-sd ptr record.

Revision 1.162  2004/04/14 23:09:29  ksekar
Support for TSIG signed dynamic updates.

Revision 1.161  2004/04/07 01:20:04  cheshire
Hash slot value should be unsigned

Revision 1.160  2004/04/06 19:51:24  cheshire
<rdar://problem/3605898> mDNSResponder will not launch if "nobody" user doesn't exist.
After more discussion, we've decided to use userid -2 if "nobody" user doesn't exist.

Revision 1.159  2004/04/03 01:36:55  cheshire
<rdar://problem/3605898> mDNSResponder will not launch if "nobody" user doesn't exist.
If "nobody" user doesn't exist, log a message and continue as "root"

Revision 1.158  2004/04/02 21:39:05  cheshire
Fix errors in comments

Revision 1.157  2004/03/19 18:49:10  ksekar
Increased size check in freeL() to account for LargeCacheRecord
structs larger than 8k

Revision 1.156  2004/03/19 18:19:19  ksekar
Fixed daemon.c to compile with malloc debugging turned on.

Revision 1.155  2004/03/13 01:57:34  ksekar
<rdar://problem/3192546>: DynDNS: Dynamic update of service records

Revision 1.154  2004/03/12 08:42:47  cheshire
<rdar://problem/3548256>: Should not allow empty string for resolve domain

Revision 1.153  2004/03/12 08:08:51  cheshire
Update comments

Revision 1.152  2004/02/05 19:39:29  cheshire
Move creation of /var/run/mDNSResponder.pid to uds_daemon.c,
so that all platforms get this functionality

Revision 1.151  2004/02/03 22:35:34  cheshire
<rdar://problem/3548256>: Should not allow empty string for resolve domain

Revision 1.150  2004/01/28 21:14:23  cheshire
Reconcile debug_mode and gDebugLogging into a single flag (mDNS_DebugMode)

Revision 1.149  2004/01/28 02:30:08  ksekar
Added default Search Domains to unicast browsing, controlled via
Networking sharing prefs pane.  Stopped sending unicast messages on
every interface.  Fixed unicast resolving via mach-port API.

Revision 1.148  2004/01/25 00:03:20  cheshire
Change to use mDNSVal16() instead of private PORT_AS_NUM() macro

Revision 1.147  2004/01/19 19:51:46  cheshire
Fix compiler error (mixed declarations and code) on some versions of Linux

Revision 1.146  2003/12/08 21:00:46  rpantos
Changes to support mDNSResponder on Linux.

Revision 1.145  2003/12/05 22:08:07  cheshire
Update version string to "mDNSResponder-61", including new mechanism to allow dots (e.g. 58.1)

Revision 1.144  2003/11/19 23:21:08  ksekar
<rdar://problem/3486646>: config change handler not called for dns-sd services

Revision 1.143  2003/11/14 21:18:32  cheshire
<rdar://problem/3484766>: Security: Crashing bug in mDNSResponder
Fix code that should use buffer size MAX_ESCAPED_DOMAIN_NAME (1005) instead of 256-byte buffers.

Revision 1.142  2003/11/08 22:18:29  cheshire
<rdar://problem/3477870>: Don't need to show process ID in *every* mDNSResponder syslog message

Revision 1.141  2003/11/07 02:30:57  cheshire
Also check per-slot cache use counts in SIGINFO state log

Revision 1.140  2003/10/21 19:58:26  cheshire
<rdar://problem/3459037> Syslog messages should show TTL as signed (for overdue records)

Revision 1.139  2003/10/21 00:10:18  rpantos
<rdar://problem/3409401>: mDNSResponder should not run as root

Revision 1.138  2003/10/07 20:16:58  cheshire
Shorten syslog message a bit

Revision 1.137  2003/09/23 02:12:43  cheshire
Also include port number in list of services registered via new UDS API

Revision 1.136  2003/09/23 02:07:25  cheshire
Include port number in DNSServiceRegistration START/STOP messages

Revision 1.135  2003/09/23 01:34:02  cheshire
In SIGINFO state log, show remaining TTL on cache records, and port number on ServiceRegistrations

Revision 1.134  2003/08/21 20:01:37  cheshire
<rdar://problem/3387941> Traffic reduction: Detect long-lived Resolve() calls, and report them in syslog

Revision 1.133  2003/08/20 23:39:31  cheshire
<rdar://problem/3344098> Review syslog messages, and remove as appropriate

Revision 1.132  2003/08/20 01:44:56  cheshire
Fix errors in LogOperation() calls (only used for debugging)

Revision 1.131  2003/08/19 05:39:43  cheshire
<rdar://problem/3380097> SIGINFO dump should include resolves started by DNSServiceQueryRecord

Revision 1.130  2003/08/16 03:39:01  cheshire
<rdar://problem/3338440> InterfaceID -1 indicates "local only"

Revision 1.129  2003/08/15 20:16:03  cheshire
<rdar://problem/3366590> mDNSResponder takes too much RPRVT
We want to avoid touching the rdata pages, so we don't page them in.
1. RDLength was stored with the rdata, which meant touching the page just to find the length.
   Moved this from the RData to the ResourceRecord object.
2. To avoid unnecessarily touching the rdata just to compare it,
   compute a hash of the rdata and store the hash in the ResourceRecord object.

Revision 1.128  2003/08/14 19:30:36  cheshire
<rdar://problem/3378473> Include list of cache records in SIGINFO output

Revision 1.127  2003/08/14 02:18:21  cheshire
<rdar://problem/3375491> Split generic ResourceRecord type into two separate types: AuthRecord and CacheRecord

Revision 1.126  2003/08/12 19:56:25  cheshire
Update to APSL 2.0

Revision 1.125  2003/08/08 18:36:04  cheshire
<rdar://problem/3344154> Only need to revalidate on interface removal on platforms that have the PhantomInterfaces bug

Revision 1.124  2003/07/25 18:28:23  cheshire
Minor fix to error messages in syslog: Display string parameters with quotes

Revision 1.123  2003/07/23 17:45:28  cheshire
<rdar://problem/3339388> mDNSResponder leaks a bit
Don't allocate memory for the reply until after we've verified that the reply is valid

Revision 1.122  2003/07/23 00:00:04  cheshire
Add comments

Revision 1.121  2003/07/20 03:38:51  ksekar
<rdar://problem/3320722> Completed support for Unix-domain socket based API.

Revision 1.120  2003/07/18 00:30:00  cheshire
<rdar://problem/3268878> Remove mDNSResponder version from packet header and use HINFO record instead

Revision 1.119  2003/07/17 19:08:58  cheshire
<rdar://problem/3332153> Remove calls to enable obsolete UDS code

Revision 1.118  2003/07/15 21:12:28  cheshire
Added extra debugging checks in validatelists() (not used in final shipping version)

Revision 1.117  2003/07/15 01:55:15  cheshire
<rdar://problem/3315777> Need to implement service registration with subtypes

Revision 1.116  2003/07/02 21:19:51  cheshire
<rdar://problem/3313413> Update copyright notices, etc., in source code comments

Revision 1.115  2003/07/02 02:41:24  cheshire
<rdar://problem/2986146> mDNSResponder needs to start with a smaller cache and then grow it as needed

Revision 1.114  2003/07/01 21:10:20  cheshire
Reinstate checkin 1.111, inadvertently overwritten by checkin 1.112

Revision 1.113  2003/06/28 17:27:43  vlubet
<rdar://problem/3221246> Redirect standard input, standard output, and
standard error file descriptors to /dev/null just like any other
well behaved daemon

Revision 1.112  2003/06/25 23:42:19  ksekar
<rdar://problem/3249292>: Feature: New DNS-SD APIs (#7875)
Reviewed by: Stuart Cheshire
Added files necessary to implement Unix domain sockets based enhanced
DNS-SD APIs, and integrated with existing Mach-port based daemon.

Revision 1.111  2003/06/11 01:02:43  cheshire
<rdar://problem/3287858> mDNSResponder binary compatibility
Make single binary that can run on both Jaguar and Panther.

Revision 1.110  2003/06/10 01:14:11  cheshire
<rdar://problem/3286004> New APIs require a mDNSPlatformInterfaceIDfromInterfaceIndex() call

Revision 1.109  2003/06/06 19:53:43  cheshire
For clarity, rename question fields name/rrtype/rrclass as qname/qtype/qclass
(Global search-and-replace; no functional change to code execution.)

Revision 1.108  2003/06/06 14:08:06  cheshire
For clarity, pull body of main while() loop out into a separate function called mDNSDaemonIdle()

Revision 1.107  2003/05/29 05:44:55  cheshire
Minor fixes to log messages

Revision 1.106  2003/05/27 18:30:55  cheshire
<rdar://problem/3262962> Need a way to easily examine current mDNSResponder state
Dean Reece suggested SIGINFO is more appropriate than SIGHUP

Revision 1.105  2003/05/26 03:21:29  cheshire
Tidy up address structure naming:
mDNSIPAddr         => mDNSv4Addr (for consistency with mDNSv6Addr)
mDNSAddr.addr.ipv4 => mDNSAddr.ip.v4
mDNSAddr.addr.ipv6 => mDNSAddr.ip.v6

Revision 1.104  2003/05/26 00:42:06  cheshire
<rdar://problem/3268876> Temporarily include mDNSResponder version in packets

Revision 1.103  2003/05/23 23:07:44  cheshire
<rdar://problem/3268199> Must not write to stderr when running as daemon

Revision 1.102  2003/05/22 01:32:31  cheshire
Fix typo in Log message format string

Revision 1.101  2003/05/22 00:26:55  cheshire
<rdar://problem/3239284> DNSServiceRegistrationCreate() should return error on dup
Modify error message to explain that this is technically legal, but may indicate a bug.

Revision 1.100  2003/05/21 21:02:24  ksekar
<rdar://problem/3247035>: Service should be prefixed
Changed kmDNSBootstrapName to "com.apple.mDNSResponderRestart" since we're changing the main
Mach message port to "com.apple.mDNSResponder.

Revision 1.99  2003/05/21 17:33:49  cheshire
Fix warnings (mainly printf format string warnings, like using "%d" where it should say "%lu", etc.)

Revision 1.98  2003/05/20 00:33:07  cheshire
<rdar://problem/3262962> Need a way to easily examine current mDNSResponder state
SIGHUP now writes state summary to syslog

Revision 1.97  2003/05/08 00:19:08  cheshire
<rdar://problem/3250330> Forgot to set "err = mStatus_BadParamErr" in a couple of places

Revision 1.96  2003/05/07 22:10:46  cheshire
<rdar://problem/3250330> Add a few more error logging messages

Revision 1.95  2003/05/07 19:20:17  cheshire
<rdar://problem/3251391> Add version number to mDNSResponder builds

Revision 1.94  2003/05/07 00:28:18  cheshire
<rdar://problem/3250330> Need to make mDNSResponder more defensive against bad clients

Revision 1.93  2003/05/06 00:00:49  cheshire
<rdar://problem/3248914> Rationalize naming of domainname manipulation functions

Revision 1.92  2003/04/04 20:38:57  cheshire
Add $Log header

 */

#include <mach/mach.h>
#include <mach/mach_error.h>
#include <servers/bootstrap.h>
#include <sys/types.h>
#include <unistd.h>
#include <paths.h>
#include <fcntl.h>
#include <pwd.h>

#include "DNSServiceDiscoveryRequestServer.h"
#include "DNSServiceDiscoveryReply.h"

#include "mDNSClientAPI.h"			// Defines the interface to the client layer above
#include "mDNSMacOSX.h"				// Defines the specific types needed to run mDNS on this platform

#include "uds_daemon.h"				// Interface to the server side implementation of dns_sd.h

#include "GenLinkedList.h"

#include <DNSServiceDiscovery/DNSServiceDiscovery.h>

extern mDNSs32 CountSubTypes(char *regtype);
extern AuthRecord *AllocateSubTypes(mDNSs32 NumSubTypes, char *p);

//*************************************************************************************************************
// Macros

// Note: The C preprocessor stringify operator ('#') makes a string from its argument, without macro expansion
// e.g. If "version" is #define'd to be "4", then STRINGIFY_AWE(version) will return the string "version", not "4"
// To expand "version" to its value before making the string, use STRINGIFY(version) instead
#define STRINGIFY_ARGUMENT_WITHOUT_EXPANSION(s) #s 
#define STRINGIFY(s) STRINGIFY_ARGUMENT_WITHOUT_EXPANSION(s)

// convenience definition 
#define	_UNUSED	__attribute__ ((unused))

//*************************************************************************************************************
// Globals

#define LOCAL_DEFAULT_REG 1 // empty string means register in the local domain
#define DEFAULT_REG_DOMAIN "apple.com." // used if the above flag is turned off
mDNSexport mDNS mDNSStorage;
static mDNS_PlatformSupport PlatformStorage;
#define RR_CACHE_SIZE 64
static CacheRecord rrcachestorage[RR_CACHE_SIZE];

static const char kmDNSBootstrapName[] = "com.apple.mDNSResponderRestart";
static mach_port_t client_death_port = MACH_PORT_NULL;
static mach_port_t exit_m_port       = MACH_PORT_NULL;
static mach_port_t info_m_port       = MACH_PORT_NULL;
static mach_port_t server_priv_port  = MACH_PORT_NULL;

// mDNS Mach Message Timeout, in milliseconds.
// We need this to be short enough that we don't deadlock the mDNSResponder if a client
// fails to service its mach message queue, but long enough to give a well-written
// client a chance to service its mach message queue without getting cut off.
// Empirically, 50ms seems to work, so we set the timeout to 250ms to give
// even extra-slow clients a fair chance before we cut them off.
#define MDNS_MM_TIMEOUT 250

static int restarting_via_mach_init = 0;

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
	} DNSServiceBrowserQuestion;

struct DNSServiceBrowser_struct
	{
	DNSServiceBrowser *next;
	mach_port_t ClientMachPort;
    DNSServiceBrowserQuestion *qlist;
	DNSServiceBrowserResult *results;
	mDNSs32 lastsuccess;
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


typedef struct ExtraRecordRef
	{
    ExtraResourceRecord *localRef;  // extra added to .local service
    ExtraResourceRecord *globalRef; // extra added to default global service (may be NULL)
    struct ExtraRecordRef *next;
	} ExtraRecordRef;

typedef struct DNSServiceRegistration_struct DNSServiceRegistration;
struct DNSServiceRegistration_struct
	{
	DNSServiceRegistration *next;
	mach_port_t ClientMachPort;
	mDNSBool autoname;
	mDNSBool autorenameLS;
    mDNSBool autorenameGS;
    mDNSBool deallocate;  // gs and ls (below) will receive separate MemFree callbacks,
                          // the latter of which must deallocate the wrapper structure.
                          // ls MemFree callback: if (!gs) free wrapper;  else set deallocate flag
                          // gs callback: if (deallocate) free wrapper;  else free (gs), gs = NULL 
	domainlabel name;
    ExtraRecordRef   *ExtraRefList;
    ServiceRecordSet *gs; // default "global" (wide area) service (may be NULL)
    ServiceRecordSet ls;  // .local service (also used if client passes an explicit domain)
	// Don't add any fields after ServiceRecordSet.
	// This is where the implicit extra space goes if we allocate an oversized ServiceRecordSet object
	};

static DNSServiceDomainEnumeration *DNSServiceDomainEnumerationList = NULL;
static DNSServiceBrowser           *DNSServiceBrowserList           = NULL;
static DNSServiceResolver          *DNSServiceResolverList          = NULL;
static DNSServiceRegistration      *DNSServiceRegistrationList      = NULL;

//*************************************************************************************************************
// General Utility Functions

#if MACOSX_MDNS_MALLOC_DEBUGGING

char _malloc_options[] = "AXZ";

static void validatelists(mDNS *const m)
	{
	DNSServiceDomainEnumeration *e;
	DNSServiceBrowser           *b;
	DNSServiceResolver          *l;
	DNSServiceRegistration      *r;
	AuthRecord                  *rr;
	CacheRecord                 *cr;
	DNSQuestion                 *q;
	mDNSu32 slot;
	
	for (e = DNSServiceDomainEnumerationList; e; e=e->next)
		if (e->ClientMachPort == 0 || e->ClientMachPort == (mach_port_t)~0)
			LogMsg("!!!! DNSServiceDomainEnumerationList: %p is garbage (%X) !!!!", e, e->ClientMachPort);

	for (b = DNSServiceBrowserList; b; b=b->next)
		if (b->ClientMachPort == 0 || b->ClientMachPort == (mach_port_t)~0)
			LogMsg("!!!! DNSServiceBrowserList: %p is garbage (%X) !!!!", b, b->ClientMachPort);

	for (l = DNSServiceResolverList; l; l=l->next)
		if (l->ClientMachPort == 0 || l->ClientMachPort == (mach_port_t)~0)
			LogMsg("!!!! DNSServiceResolverList: %p is garbage (%X) !!!!", l, l->ClientMachPort);

	for (r = DNSServiceRegistrationList; r; r=r->next)
		if (r->ClientMachPort == 0 || r->ClientMachPort == (mach_port_t)~0)
			LogMsg("!!!! DNSServiceRegistrationList: %p is garbage (%X) !!!!", r, r->ClientMachPort);

	for (rr = m->ResourceRecords; rr; rr=rr->next)
		if (rr->resrec.RecordType == 0 || rr->resrec.RecordType == 0xFF)
			LogMsg("!!!! ResourceRecords list: %p is garbage (%X) !!!!", rr, rr->resrec.RecordType);

	for (rr = m->DuplicateRecords; rr; rr=rr->next)
		if (rr->resrec.RecordType == 0 || rr->resrec.RecordType == 0xFF)
			LogMsg("!!!! DuplicateRecords list: %p is garbage (%X) !!!!", rr, rr->resrec.RecordType);

	for (q = m->Questions; q; q=q->next)
		if (q->ThisQInterval == (mDNSs32)~0)
			LogMsg("!!!! Questions list: %p is garbage (%lX) !!!!", q, q->ThisQInterval);

	for (slot = 0; slot < CACHE_HASH_SLOTS; slot++)
		for (cr = mDNSStorage.rrcache_hash[slot]; cr; cr=cr->next)
			if (cr->resrec.RecordType == 0 || cr->resrec.RecordType == 0xFF)
				LogMsg("!!!! Cache slot %lu: %p is garbage (%X) !!!!", slot, rr, rr->resrec.RecordType);
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
		LogMalloc("malloc( %s : %lu ) = %p", msg, size, &mem[2]);
		mem[0] = 0xDEAD1234;
		mem[1] = size;
		//bzero(&mem[2], size);
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
		if (mem[0] != 0xDEAD1234)
			{ LogMsg("free( %s @ %p ) !!!! NOT ALLOCATED !!!!", msg, &mem[2]); return; }
		if (mem[1] > 24000)
			{ LogMsg("free( %s : %ld @ %p) too big!", msg, mem[1], &mem[2]); return; }
		LogMalloc("free( %s : %ld @ %p)", msg, mem[1], &mem[2]);
		//bzero(mem, mem[1]+8);
		memset(mem, 0xFF, mem[1]+8);
		validatelists(&mDNSStorage);
		free(mem);
		}
	}

#endif

//*************************************************************************************************************
// Client Death Detection

mDNSlocal void FreeSRS(ServiceRecordSet *s)
	{
	while (s->Extras)
		{
		ExtraResourceRecord *extras = s->Extras;
		s->Extras = s->Extras->next;
		if (extras->r.resrec.rdata != &extras->r.rdatastorage)
			freeL("Extra RData", extras->r.resrec.rdata);
		freeL("ExtraResourceRecord", extras);
		}

	if (s->RR_TXT.resrec.rdata != &s->RR_TXT.rdatastorage)
			freeL("TXT RData", s->RR_TXT.resrec.rdata);

	if (s->SubTypes) freeL("ServiceSubTypes", s->SubTypes);	
	}

mDNSlocal void FreeDNSServiceRegistration(ServiceRecordSet *srs)
	{
	DNSServiceRegistration *x = srs->ServiceContext;
	ExtraRecordRef *ref, *fptr;
	
	FreeSRS(srs); 
	if (srs == x->gs)
		{
		freeL("DNSServiceRegistration GlobalService", srs);
		x->gs = NULL;
		}
	else x->deallocate = mDNStrue;

	if (x->deallocate && !x->gs)
		{
		ref = x->ExtraRefList;
		while (ref)			
			{ fptr = ref; ref = ref->next; freeL("ExtraRecordRef", fptr); }
		freeL("DNSServiceRegistration", x);
		}
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
				LogMsg("%5d: DNSServiceBrowser(%##s) STOP; WARNING m %p != x %p", ClientMachPort, qptr->q.qname.c, m, x);
			else LogOperation("%5d: DNSServiceBrowser(%##s) STOP", ClientMachPort, qptr->q.qname.c);
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
			LogMsg("%5d: DNSServiceResolver(%##s) STOP; WARNING m %p != x %p", ClientMachPort, x->i.name.c, m, x);
		else LogOperation("%5d: DNSServiceResolver(%##s) STOP", ClientMachPort, x->i.name.c);
		mDNS_StopResolveService(&mDNSStorage, &x->q);
		freeL("DNSServiceResolver", x);
		return;
		}

	while (*r && (*r)->ClientMachPort != ClientMachPort) r = &(*r)->next;
	if (*r)
		{
		DNSServiceRegistration *x = *r;
		*r = (*r)->next;
		x->autorenameLS = mDNSfalse;
		x->autorenameGS = mDNSfalse;
		if (m && m != x)
			{
			LogMsg("%5d: DNSServiceRegistration(%##s, %u) STOP; WARNING m %p != x %p", ClientMachPort, x->ls.RR_SRV.resrec.name.c, SRS_PORT(&x->ls), m, x);
			if (x->gs) LogMsg("%5d: DNSServiceRegistration(%##s, %u) STOP; WARNING m %p != x %p", ClientMachPort, x->gs->RR_SRV.resrec.name.c, SRS_PORT(x->gs), m, x);
			}
		else
			{
			LogOperation("%5d: DNSServiceRegistration(%##s, %u) STOP", ClientMachPort, x->ls.RR_SRV.resrec.name.c, SRS_PORT(&x->ls));
			if (x->gs) LogOperation("%5d: DNSServiceRegistration(%##s, %u) STOP", ClientMachPort, x->gs->RR_SRV.resrec.name.c, SRS_PORT(x->gs));
			}
		// If mDNS_DeregisterService() returns mStatus_NoError, that means that the service was found in the list,
		// is sending its goodbye packet, and we'll get an mStatus_MemFree message when we can free the memory.
		// If mDNS_DeregisterService() returns an error, it means that the service had already been removed from
		// the list, so we should go ahead and free the memory right now
		if (x->gs && mDNS_DeregisterService(&mDNSStorage, x->gs))
			{
			// Deregister returned an error, so we free immediately
			FreeSRS(x->gs);
			x->gs = NULL;
			}
		if (mDNS_DeregisterService(&mDNSStorage, &x->ls))
			FreeDNSServiceRegistration(&x->ls);
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
	if      (e)     LogMsg("%5d: DomainEnumeration(%##s) %s%s",                   c, e->dom.qname.c,            reason, msg);
	else if (b)
		{
		for (qptr = b->qlist; qptr; qptr = qptr->next)
			        LogMsg("%5d: Browser(%##s) %s%s",                             c, qptr->q.qname.c,              reason, msg);
		}
	else if (l)     LogMsg("%5d: Resolver(%##s) %s%s",                            c, l->i.name.c,               reason, msg);
	else if (r)
		{
		            LogMsg("%5d: Registration(%##s) %s%s",                        c, r->ls.RR_SRV.resrec.name.c, reason, msg);
		if (r->gs)  LogMsg("%5d: Registration(%##s) %s%s",                        c, r->gs->RR_SRV.resrec.name.c, reason, msg);
		}
	else            LogMsg("%5d: (%s) %s, but no record of client can be found!", c,                            reason, msg);

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
	if (r) LogMsg("%5d: Registration(%##s) already exists!",      c, r->ls.RR_SRV.resrec.name.c);
	return(e || b || l || r);
	}

mDNSlocal void ClientDeathCallback(CFMachPortRef unusedport, void *voidmsg, CFIndex size, void *info)
	{
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

mDNSlocal void FoundDomain(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, mDNSBool AddRecord)
	{
	kern_return_t status;
	#pragma unused(m)
	char buffer[MAX_ESCAPED_DOMAIN_NAME];
	DNSServiceDomainEnumerationReplyResultType rt;
	DNSServiceDomainEnumeration *x = (DNSServiceDomainEnumeration *)question->QuestionContext;

	debugf("FoundDomain: %##s PTR %##s", answer->name.c, answer->rdata->u.name.c);
	if (answer->rrtype != kDNSType_PTR) return;
	if (!x) { debugf("FoundDomain: DNSServiceDomainEnumeration is NULL"); return; }

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
	const DNSServiceDomainEnumerationReplyResultType rt = DNSServiceDomainEnumerationReplyAddDomainDefault;

	// Allocate memory, and handle failure
	DNSServiceDomainEnumeration *x = mallocL("DNSServiceDomainEnumeration", sizeof(*x));
	if (!x) { err = mStatus_NoMemoryErr; errormsg = "No memory"; goto fail; }

	// Set up object, and link into list
	x->ClientMachPort = client;
	x->next = DNSServiceDomainEnumerationList;
	DNSServiceDomainEnumerationList = x;
	
	// Generate initial response
	verbosedebugf("%5d: Enumerate %s Domains", client, regDom ? "Registration" : "Browsing");
	// We always give local. as the initial default browse domain, and then look for more
	kern_return_t status = DNSServiceDomainEnumerationReply_rpc(x->ClientMachPort, rt, "local.", 0, MDNS_MM_TIMEOUT);
	if (status == MACH_SEND_TIMED_OUT)
		{ AbortBlockedClient(x->ClientMachPort, "local enumeration", x); return(mStatus_UnknownErr); }

	// Do the operation
	err           = mDNS_GetDomains(&mDNSStorage, &x->dom, dt1, NULL, mDNSInterface_LocalOnly, FoundDomain, x);
	if (!err) err = mDNS_GetDomains(&mDNSStorage, &x->def, dt2, NULL, mDNSInterface_LocalOnly, FoundDomain, x);
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

mDNSlocal void FoundInstance(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, mDNSBool AddRecord)
	{
	(void)m;		// Unused
	
	if (answer->rrtype != kDNSType_PTR)
		{ LogMsg("FoundInstance: Should not be called with rrtype %d (not a PTR record)", answer->rrtype); return; }
	
	domainlabel name;
	domainname type, domain;
	if (!DeconstructServiceName(&answer->rdata->u.name, &name, &type, &domain))
		{
		LogMsg("FoundInstance: %##s PTR %##s received from network is not valid DNS-SD service pointer",
			answer->name.c, answer->rdata->u.name.c);
		return;
		}

	DNSServiceBrowserResult *x = mallocL("DNSServiceBrowserResult", sizeof(*x));
	if (!x) { LogMsg("FoundInstance: Failed to allocate memory for result %##s", answer->rdata->u.name.c); return; }
	
	verbosedebugf("FoundInstance: %s %##s", AddRecord ? "Add" : "Rmv", answer->rdata->u.name.c);
	AssignDomainName(x->result, answer->rdata->u.name);
	if (AddRecord)
		 x->resultType = DNSServiceBrowserReplyAddInstance;
	else x->resultType = DNSServiceBrowserReplyRemoveInstance;
	x->next = NULL;

	DNSServiceBrowser *browser = (DNSServiceBrowser *)question->QuestionContext;
	DNSServiceBrowserResult **p = &browser->results;
	while (*p) p = &(*p)->next;
	*p = x;
	}

mDNSexport kern_return_t provide_DNSServiceBrowserCreate_rpc(mach_port_t unusedserver, mach_port_t client,
	DNSCString regtype, DNSCString domain)
	{
	// Check client parameter
	(void)unusedserver;		// Unused
	mStatus err = mStatus_NoError;
	const char *errormsg = "Unknown";
	DNameListElem *SearchDomains = NULL, *sdPtr;
	DNSServiceBrowserQuestion *qptr;
	
	if (client == (mach_port_t)-1)      { err = mStatus_Invalid; errormsg = "Client id -1 invalid";     goto fail; }
	if (CheckForExistingClient(client)) { err = mStatus_Invalid; errormsg = "Client id already in use"; goto fail; }

	// Check other parameters
	domainname t, d;
	t.c[0] = 0;
	mDNSs32 NumSubTypes = CountSubTypes(regtype);
	if (NumSubTypes < 0 || NumSubTypes > 1) { errormsg = "Bad Service SubType"; goto badparam; }
	if (NumSubTypes == 1 && !AppendDNSNameString(&t, regtype + strlen(regtype) + 1))
	                                        { errormsg = "Bad Service SubType"; goto badparam; }
	if (!regtype[0] || !AppendDNSNameString(&t, regtype))                  { errormsg = "Illegal regtype"; goto badparam; }

	// Allocate memory, and handle failure
	DNSServiceBrowser *x = mallocL("DNSServiceBrowser", sizeof(*x));
	if (!x) { err = mStatus_NoMemoryErr; errormsg = "No memory"; goto fail; }
	
	// Set up object, and link into list
	x->ClientMachPort = client;
	x->results = NULL;
	x->lastsuccess = 0;
	x->qlist = NULL;	
	x->next = DNSServiceBrowserList;
	DNSServiceBrowserList = x;

	//!!!KRS browse locally for ichat
	if (!domain[0] && (!strcmp(regtype, "_ichat._tcp.") || !strcmp(regtype, "_presence._tcp.")))
		domain = "local.";
	
	if (domain[0])
		{
		// Start browser for an explicit domain
		x->qlist = mallocL("DNSServiceBrowserQuestion", sizeof(DNSServiceBrowserQuestion));
		x->qlist->next = NULL;
		if (!x->qlist)  { err = mStatus_UnknownErr; AbortClient(client, x); errormsg = "malloc"; goto fail; }
		
		if (!MakeDomainNameFromDNSNameString(&d, domain)) { errormsg = "Illegal domain";  goto badparam; }
		LogOperation("%5d: DNSServiceBrowse(%##s%##s) START", client, t.c, d.c);
		err = mDNS_StartBrowse(&mDNSStorage, &x->qlist->q, &t, &d, mDNSInterface_Any, FoundInstance, x);
		if (err) { AbortClient(client, x); errormsg = "mDNS_StartBrowse"; goto fail; }
		}
	else
		{
		// Start browser on all domains
		SearchDomains = mDNSPlatformGetSearchDomainList();
		if (!SearchDomains) { AbortClient(client, x); errormsg = "GetSearchDomainList"; goto fail; }
		for (sdPtr = SearchDomains; sdPtr; sdPtr = sdPtr->next)
			{
			qptr = mallocL("DNSServiceBrowserQuestion", sizeof(DNSServiceBrowserQuestion));
			if (!qptr)  { err = mStatus_UnknownErr; AbortClient(client, x); errormsg = "malloc"; goto fail; }
			qptr->next = x->qlist;
			x->qlist = qptr;
			LogOperation("%5d: DNSServiceBrowse(%##s%##s) START", client, t.c, sdPtr->name.c);
			err = mDNS_StartBrowse(&mDNSStorage, &qptr->q, &t, &sdPtr->name, mDNSInterface_Any, FoundInstance, x);
			if (err) { AbortClient(client, x); errormsg = "mDNS_StartBrowse"; goto fail; }			
			}		
		}
	// Succeeded: Wrap up and return
	EnableDeathNotificationForClient(client, x);
	mDNS_FreeDNameList(SearchDomains);
	return(mStatus_NoError);
	
	badparam:
	err = mStatus_BadParamErr;
fail:
	LogMsg("%5d: DNSServiceBrowse(\"%s\", \"%s\") failed: %s (%ld)", client, regtype, domain, errormsg, err);
	if (SearchDomains) mDNS_FreeDNameList(SearchDomains);
	return(err);
	}

//*************************************************************************************************************
// Resolve Service Info

mDNSlocal void FoundInstanceInfo(mDNS *const m, ServiceInfoQuery *query)
	{
	kern_return_t status;
	DNSServiceResolver *x = (DNSServiceResolver *)query->ServiceInfoQueryContext;
	NetworkInterfaceInfoOSX *ifx = (NetworkInterfaceInfoOSX *)query->info->InterfaceID;
	if (query->info->InterfaceID == (mDNSInterfaceID)~0) ifx = mDNSNULL;
	struct sockaddr_storage interface;
	struct sockaddr_storage address;
	char cstring[1024];
	int i, pstrlen = query->info->TXTinfo[0];
	(void)m;		// Unused

	//debugf("FoundInstanceInfo %.4a %.4a %##s", &query->info->InterfaceAddr, &query->info->ip, &query->info->name);

	if (query->info->TXTlen > sizeof(cstring)) return;

	bzero(&interface, sizeof(interface));
	bzero(&address,   sizeof(address));

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
		sin6->sin6_addr		 = *(struct in6_addr*)&ifx->ifinfo.ip.ip.v6;
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
		sin6->sin6_addr			 = *(struct in6_addr*)&query->info->ip.ip.v6;
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
	x->ReportTime = (mDNSPlatformTimeNow() + 130 * mDNSPlatformOneSecond) | 1;
	// Don't report errors for old iChat ("_ichat._tcp") service.
	// New iChat ("_presence._tcp") uses DNSServiceQueryRecord() (from /usr/include/dns_sd.h) instead,
	// and so should other applications that have valid reasons to be doing ongoing record monitoring.
	if (SameDomainLabel(t.c, (mDNSu8*)"\x6_ichat")) x->ReportTime = 0;
	x->next = DNSServiceResolverList;
	DNSServiceResolverList = x;

	// Do the operation
	LogOperation("%5d: DNSServiceResolver(%##s) START", client, x->i.name.c);
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

mDNSlocal void RegCallback(mDNS *const m, ServiceRecordSet *const sr, mStatus result)
	{
	DNSServiceRegistration *x = (DNSServiceRegistration*)sr->ServiceContext;

	if (result == mStatus_NoError)
		{
		kern_return_t status;
		LogOperation("%5d: DNSServiceRegistration(%##s, %u) Name Registered", x->ClientMachPort, sr->RR_SRV.resrec.name.c, SRS_PORT(sr));
		status = DNSServiceRegistrationReply_rpc(x->ClientMachPort, result, MDNS_MM_TIMEOUT);
		if (status == MACH_SEND_TIMED_OUT)
			AbortBlockedClient(x->ClientMachPort, "registration success", x);
		}

	else if (result == mStatus_NameConflict)
		{
		LogOperation("%5d: DNSServiceRegistration(%##s, %u) Name Conflict", x->ClientMachPort, sr->RR_SRV.resrec.name.c, SRS_PORT(sr));
		// Note: By the time we get the mStatus_NameConflict message, the service is already deregistered
		// and the memory is free, so we don't have to wait for an mStatus_MemFree message as well.
		if (x->autoname)
			mDNS_RenameAndReregisterService(m, sr, mDNSNULL);
		else
			{
			// If we get a name conflict, we tell the client about it, and then they are expected to dispose
			// of their registration in the usual way (which we will catch via client death notification).
			// If the Mach queue is full, we forcibly abort the client immediately.
			kern_return_t status = DNSServiceRegistrationReply_rpc(x->ClientMachPort, result, MDNS_MM_TIMEOUT);
			if (status == MACH_SEND_TIMED_OUT)
				AbortBlockedClient(x->ClientMachPort, "registration conflict", x);
			}
		}
	
	else if (result == mStatus_MemFree)
		{
		mDNSBool *autorename = (sr == &x->ls ? &x->autorenameLS : &x->autorenameGS); 
		if (*autorename)
			{
			debugf("RegCallback renaming %#s to %#s", x->name.c, mDNSStorage.nicelabel.c);
			*autorename = mDNSfalse;
			x->name = mDNSStorage.nicelabel;
			mDNS_RenameAndReregisterService(m, sr, &x->name);
			}
		else
			{
			// SANITY CHECK: Should only get mStatus_MemFree as a result of calling mDNS_DeregisterService()
			// and should only get it with x->autorename false if we've already removed the record from our
			// list, but this check is just to make sure...
			DNSServiceRegistration **r = &DNSServiceRegistrationList;
			while (*r && *r != x) r = &(*r)->next;
			if (*r)
				{
				LogMsg("RegCallback: %##s Still in DNSServiceRegistration list; removing now", sr->RR_SRV.resrec.name.c);
				*r = (*r)->next;
				}
			// END SANITY CHECK
			LogOperation("%5d: DNSServiceRegistration(%##s, %u) Memory Free", x->ClientMachPort, sr->RR_SRV.resrec.name.c, SRS_PORT(sr));
			FreeDNSServiceRegistration(sr);
			}
		}
	
	else
		{
		LogMsg("%5d: DNSServiceRegistration(%##s, %u) Unknown Result %ld",
			x->ClientMachPort, sr->RR_SRV.resrec.name.c, SRS_PORT(sr), result);
		if (sr == x->gs) { freeL("RegCallback - ServiceRecordSet", x->gs); x->gs = NULL; }
		}
	}

mDNSlocal void CheckForDuplicateRegistrations(DNSServiceRegistration *x, domainname *srv, mDNSIPPort port)
	{
	int count = 1;			// Start with the one we're planning to register, then see if there are any more
	AuthRecord *rr;
	for (rr = mDNSStorage.ResourceRecords; rr; rr=rr->next)
		if (rr->resrec.rrtype == kDNSType_SRV &&
			rr->resrec.rdata->u.srv.port.NotAnInteger == port.NotAnInteger &&
			SameDomainName(&rr->resrec.name, srv))
			count++;

	if (count > 1)
		LogMsg("%5d: Client application registered %d identical instances of service %##s port %u.",
			x->ClientMachPort, count, srv->c, mDNSVal16(port));
	}

// Pass NULL for x to allocate the structure (for local service).  Call again w/ initialized x to add a global service.
mDNSlocal DNSServiceRegistration *RegisterService(mach_port_t client, DNSCString name, DNSCString regtype, DNSCString domain,
												  int notAnIntPort, DNSCString txtRecord, DNSServiceRegistration *x)
	{
	ServiceRecordSet *srs = NULL;  // record set to use in registration operation
	mStatus err = mStatus_NoError;
	const char *errormsg = "Unknown";
	
    // Check for sub-types after the service type
	mDNSs32 NumSubTypes = CountSubTypes(regtype);
	if (NumSubTypes < 0) { errormsg = "Bad Service SubType"; goto badparam; }

	// Check other parameters
	domainlabel n;
	domainname t, d;
	domainname srv;
	if (!name[0]) n = mDNSStorage.nicelabel;
	else if (!MakeDomainLabelFromLiteralString(&n, name))                  { errormsg = "Bad Instance Name"; goto badparam; }
	if (!regtype[0] || !MakeDomainNameFromDNSNameString(&t, regtype))      { errormsg = "Bad Service Type";  goto badparam; }	
	
	if (!MakeDomainNameFromDNSNameString(&d, domain))                      { errormsg = "Bad Domain";        goto badparam; }
	if (!ConstructServiceName(&srv, &n, &t, &d))                           { errormsg = "Bad Name";          goto badparam; }

	mDNSIPPort port;
	port.NotAnInteger = notAnIntPort;

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

	// Allocate memory, and handle failure
	if (!x)
		{
		x = mallocL("DNSServiceRegistration", sizeof(*x) - sizeof(RDataBody) + size);
		if (!x) { err = mStatus_NoMemoryErr; errormsg = "No memory"; goto fail; }		
		bzero(x, sizeof(*x) - sizeof(RDataBody) + size);
		// Set up object, and link into list
		x->ClientMachPort = client;
		x->autoname = (!name[0]);
		x->name = n;
		x->next = DNSServiceRegistrationList;
		DNSServiceRegistrationList = x;		
		srs = &x->ls; 
		}
	else
		{
		x->gs = mallocL("DNSServiceRegistration GlobalService", sizeof(ServiceRecordSet) - sizeof(RDataBody) + size);
		if (!x->gs) { err = mStatus_NoMemoryErr; errormsg = "No memory"; goto fail; } 
		srs = x->gs;
		bzero(srs, sizeof(ServiceRecordSet) - sizeof(RDataBody) + size);
		}
	
	AuthRecord *SubTypes = AllocateSubTypes(NumSubTypes, regtype);
	if (NumSubTypes && !SubTypes)
		{ freeL("DNSServiceRegistration", x); err = mStatus_NoMemoryErr; errormsg = "No memory"; goto fail; }

	// Do the operation
	LogOperation("%5d: DNSServiceRegistration(\"%s\", \"%s\", \"%s\", %u) START",
		x->ClientMachPort, name, regtype, domain, mDNSVal16(port));
	// Some clients use mDNS for lightweight copy protection, registering a pseudo-service with
	// a port number of zero. When two instances of the protected client are allowed to run on one
	// machine, we don't want to see misleading "Bogus client" messages in syslog and the console.
	if (port.NotAnInteger) CheckForDuplicateRegistrations(x, &srv, port);

	err = mDNS_RegisterService(&mDNSStorage, srs,
		&x->name, &t, &d,       // Name, type, domain
		mDNSNULL, port,			// Host and port
		txtinfo, data_len,		// TXT data, length
		SubTypes, NumSubTypes,	// Subtypes
		mDNSInterface_Any,		// Interface ID
		RegCallback, x);		// Callback and context

	if (err)
		{
		if (srs == &x->ls) AbortClient(client, x);  // don't abort client for global service
		else FreeDNSServiceRegistration(x->gs);  
		errormsg = "mDNS_RegisterService";
		goto fail; 
		}
	return x;
	
badtxt:
	LogMsg("%5d: TXT record: %.100s...", client, txtRecord);
badparam:
	err = mStatus_BadParamErr;
fail:
	LogMsg("%5d: DNSServiceRegister(\"%s\", \"%s\", \"%s\", %d) failed: %s (%ld)",
		   client, name, regtype, domain, notAnIntPort, errormsg, err);
	return NULL;
	}

mDNSexport kern_return_t provide_DNSServiceRegistrationCreate_rpc(mach_port_t unusedserver, mach_port_t client,
	DNSCString name, DNSCString regtype, DNSCString domain, int notAnIntPort, DNSCString txtRecord)
	{
	// Check client parameter
	(void)unusedserver;		// Unused
	mStatus err = mStatus_NoError;
	const char *errormsg = "Unknown";
	DNSServiceRegistration *x = NULL;
	if (client == (mach_port_t)-1)      { err = mStatus_Invalid; errormsg = "Client id -1 invalid";     goto fail; }
	if (CheckForExistingClient(client)) { err = mStatus_Invalid; errormsg = "Client id already in use"; goto fail; }

	x = RegisterService(client, name, regtype, *domain ? domain : "local.", notAnIntPort, txtRecord, NULL);
	if (!x) { err = mStatus_UnknownErr;  goto fail; } 

    //!!!KRS if we got a dynamic reg domain from the config file, use it for default (except for iChat)
	if (!*domain && mDNSStorage.uDNS_info.ServiceRegDomain[0] && strcmp(regtype, "_presence._tcp.") && strcmp(regtype, "_ichat._tcp."))
		x = RegisterService(client, name, regtype,  mDNSStorage.uDNS_info.ServiceRegDomain, notAnIntPort, txtRecord, x);		
	
	// Succeeded: Wrap up and return
	EnableDeathNotificationForClient(client, x);
	return(mStatus_NoError);

fail:
	LogMsg("%5d: DNSServiceRegister(\"%s\", \"%s\", \"%s\", %d) failed: %s (%ld)",
		   client, name, regtype, domain, notAnIntPort, errormsg, err);
	return mStatus_UnknownErr;
	}
	
mDNSlocal void mDNS_StatusCallback(mDNS *const m, mStatus result)
	{
	(void)m; // Unused
	if (result == mStatus_ConfigChanged)
		{
		DNSServiceRegistration *r;
		for (r = DNSServiceRegistrationList; r; r=r->next)
			if (r->autoname && !SameDomainLabel(r->name.c, mDNSStorage.nicelabel.c))
				{
				debugf("NetworkChanged renaming %#s to %#s", r->name.c, mDNSStorage.nicelabel.c);
				r->autorenameLS = mDNStrue;
				mDNS_DeregisterService(&mDNSStorage, &r->ls);
				if (r->gs) { mDNS_DeregisterService(&mDNSStorage, r->gs); r->autorenameGS = mDNStrue; }
				}
		udsserver_handle_configchange();
		}
	else if (result == mStatus_GrowCache)
		{
		// If we've run out of cache space, then double the total cache size and give the memory to mDNSCore
		mDNSu32 numrecords = m->rrcache_size;
		CacheRecord *storage = mallocL("mStatus_GrowCache", sizeof(CacheRecord) * numrecords);
		if (storage) mDNS_GrowCache(&mDNSStorage, storage, numrecords);
		}
	}

//*************************************************************************************************************
// Add / Update / Remove records from existing Registration


mDNSlocal ExtraResourceRecord *AddExtraRecord(DNSServiceRegistration *x, ServiceRecordSet *srs, mach_port_t client,
											  int type, const char *data, mach_msg_type_number_t data_len, uint32_t ttl)
	{
	mStatus err = mStatus_NoError;
	const char *errormsg = "Unknown";
	domainname *name = (domainname *)"";
	name = &srs->RR_SRV.resrec.name;

	(void)x;  // unused
	
	unsigned int size = sizeof(RDataBody);
	if (size < data_len)
		size = data_len;
	
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
		client, srs->RR_SRV.resrec.name.c, type, data_len, extra);
	err = mDNS_AddRecordToService(&mDNSStorage, srs, extra, &extra->r.rdatastorage, ttl);

	if (err)
		{
		freeL("Extra Resource Record", extra);
		errormsg = "mDNS_AddRecordToService";
		goto fail;
		}
	
	return extra;
	
fail:
	LogMsg("%5d: DNSServiceRegistrationAddRecord(%##s, type %d, length %d) failed: %s (%ld)", client, name->c, type, data_len, errormsg, err);
	return NULL;
	}


mDNSexport kern_return_t provide_DNSServiceRegistrationAddRecord_rpc(mach_port_t unusedserver, mach_port_t client,
	int type, const char *data, mach_msg_type_number_t data_len, uint32_t ttl, natural_t *reference)
	{
	// Check client parameter
	(void)unusedserver;		// Unused
	mStatus err = mStatus_NoError;
	const char *errormsg = "Unknown";
	if (client == (mach_port_t)-1) { err = mStatus_Invalid;         errormsg = "Client id -1 invalid"; goto fail; }
	DNSServiceRegistration *x = DNSServiceRegistrationList;
	while (x && x->ClientMachPort != client) x = x->next;
	if (!x)                        { err = mStatus_BadReferenceErr; errormsg = "No such client";       goto fail; }

	// Check other parameters
	if (data_len > 8192) { err = mStatus_BadParamErr; errormsg = "data_len > 8K"; goto fail; }

	ExtraRecordRef *ref = mallocL("ExtraRecordRef", sizeof(ExtraRecordRef));
	if (!ref) { LogMsg("ERROR: malloc"); return mStatus_NoMemoryErr; }

	ref->localRef = AddExtraRecord(x, &x->ls, client, type, data, data_len, ttl);
	if (!ref->localRef) { freeL("ExtraRecordRef", ref); *reference = (natural_t)NULL; return mStatus_UnknownErr; }

	if (x->gs) ref->globalRef = AddExtraRecord(x, x->gs, client, type, data, data_len, ttl);  // return success even if global case fails
	else ref->globalRef = NULL;
	
	// Succeeded: Wrap up and return
	ref->next = x->ExtraRefList;
	x->ExtraRefList = ref;
	*reference = (natural_t)ref;
	return mStatus_NoError;

fail:
	LogMsg("%5d: DNSServiceRegistrationAddRecord(%##s, type %d, length %d) failed: %s (%ld)", client, x->name.c, type, data_len, errormsg, err);
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
	domainname *name = (domainname *)"";

	name = &srs->RR_SRV.resrec.name;

	unsigned int size = sizeof(RDataBody);
    if (size < data_len)
		size = data_len;

	// Allocate memory, and handle failure
	RData *newrdata = mallocL("RData", sizeof(*newrdata) - sizeof(RDataBody) + size);
	if (!newrdata) { err = mStatus_NoMemoryErr; errormsg = "No memory"; goto fail; }

	// Fill in new length, and data
	newrdata->MaxRDLength = size;
	memcpy(&newrdata->u, data, data_len);
	
	// Do the operation
	LogOperation("%5d: DNSServiceRegistrationUpdateRecord(%##s, new length %d)",
		client, srs->RR_SRV.resrec.name.c, data_len);

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
	domainname *name = (domainname *)"";
	AuthRecord *gRR, *lRR;

	(void)unusedserver;  // unused
    if (client == (mach_port_t)-1) { err = mStatus_Invalid;         errormsg = "Client id -1 invalid"; goto fail; }
	DNSServiceRegistration *x = DNSServiceRegistrationList;
	while (x && x->ClientMachPort != client) x = x->next;
	if (!x)                        { err = mStatus_BadReferenceErr; errormsg = "No such client";       goto fail; }

	// Check other parameters
	if (data_len > 8192) { err = mStatus_BadParamErr; errormsg = "data_len > 8K"; goto fail; }

	// Find the record we're updating. NULL reference means update the primary TXT record
	if (!reference)
		{
		lRR = &x->ls.RR_TXT;
		gRR = x->gs ? &x->gs->RR_TXT : NULL;
		}
	else
		{
		ExtraRecordRef *ref;
		for (ref = x->ExtraRefList; ref; ref= ref->next)			
			if (ref == (ExtraRecordRef *)reference) break;
		if (!ref) { err = mStatus_BadReferenceErr; errormsg = "No such record"; goto fail; }
		lRR = &ref->localRef->r;
		gRR = ref->globalRef ? &ref->globalRef->r : NULL;
		}

	err = UpdateRecord(&x->ls, client, lRR, data, data_len, ttl);
	if (err) goto fail;

	if (gRR) UpdateRecord(x->gs, client, gRR, data, data_len, ttl); // don't return error if global fails
	return mStatus_NoError;
	
fail:
	LogMsg("%5d: DNSServiceRegistrationUpdateRecord(%##s, %X, %d) failed: %s (%ld)", client, name->c, reference, data_len, errormsg, err);
	return(err);
	}

mDNSlocal mStatus RemoveRecord(ServiceRecordSet *srs, ExtraResourceRecord *extra, mach_port_t client)
	{
	domainname *name = &srs->RR_SRV.resrec.name;
	mStatus err = mStatus_NoError;
	const char *errormsg = "Unknown";
	
	// Do the operation
	LogOperation("%5d: DNSServiceRegistrationRemoveRecord(%##s)", client, srs->RR_SRV.resrec.name.c);

	err = mDNS_RemoveRecordFromService(&mDNSStorage, srs, extra);
	if (err) { errormsg = "mDNS_RemoveRecordFromService (No such record)"; goto fail; }

	// Succeeded: Wrap up and return
	if (extra->r.resrec.rdata != &extra->r.rdatastorage)
		freeL("Extra RData", extra->r.resrec.rdata);
	freeL("ExtraResourceRecord", extra);
	return(mStatus_NoError);

fail:
	LogMsg("%5d: DNSServiceRegistrationRemoveRecord(%##s, %X) failed: %s", client, name->c, errormsg, err);
	return(err);
	}

mDNSexport kern_return_t provide_DNSServiceRegistrationRemoveRecord_rpc(mach_port_t unusedserver, mach_port_t client,
	natural_t reference)
	{
	// Check client parameter
	(void)unusedserver;		// Unused
	mStatus err = mStatus_NoError;
	const char *errormsg = "Unknown";
	ExtraRecordRef *ref, *prev = NULL;
	if (client == (mach_port_t)-1) { err = mStatus_Invalid;         errormsg = "Client id -1 invalid"; goto fail; }
	DNSServiceRegistration *x = DNSServiceRegistrationList;
	while (x && x->ClientMachPort != client) x = x->next;
	if (!x)                        { err = mStatus_BadReferenceErr; errormsg = "No such client";       goto fail; }

	ref = x->ExtraRefList;
	while (ref)
		{
		if (ref == (ExtraRecordRef *)ref) break;
		prev = ref;
	   ref = ref->next;
		}
	
	if (!ref) { err = mStatus_BadReferenceErr; errormsg = "No such reference"; goto fail; }
	err = RemoveRecord(&x->ls, ref->localRef, client);
	if (x->gs && ref->globalRef) RemoveRecord(x->gs, ref->globalRef, client);  // don't return error if this fails

	// delete the ref struct
	if (prev) prev->next = ref->next;
	else x->ExtraRefList = ref->next;
	ref->next = NULL;
	freeL("ExtraRecordRef", ref);
	return err;

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
            return;
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
        return;
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
            /* Includes success case.  */
            break;
		}

    CFAllocatorDeallocate(NULL, reply);
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

mDNSlocal void ExitCallback(CFMachPortRef port, void *msg, CFIndex size, void *info)
	{
	(void)port;		// Unused
	(void)msg;		// Unused
	(void)size;		// Unused
	(void)info;		// Unused
/*
	CacheRecord *rr;
	int rrcache_active = 0;
	for (rr = mDNSStorage.rrcache; rr; rr=rr->next) if (CacheRRActive(&mDNSStorage, rr)) rrcache_active++;
	debugf("ExitCallback: RR Cache now using %d records, %d active", mDNSStorage.rrcache_used, rrcache_active);
*/

	LogMsgIdent(mDNSResponderVersionString, "stopping");

	debugf("ExitCallback: destroyBootstrapService");
	if (!mDNS_DebugMode)
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
	if (udsserver_exit() < 0) LogMsg("ExitCallback: udsserver_exit failed");
	exit(0);
	}

// Send a mach_msg to ourselves (since that is signal safe) telling us to cleanup and exit
mDNSlocal void HandleSIGTERM(int signal)
	{
	(void)signal;		// Unused
	debugf(" ");
	debugf("SIGINT/SIGTERM");
	mach_msg_header_t header;
	header.msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_MAKE_SEND, 0);
	header.msgh_remote_port = exit_m_port;
	header.msgh_local_port = MACH_PORT_NULL;
	header.msgh_size = sizeof(header);
	header.msgh_id = 0;
	if (mach_msg_send(&header) != MACH_MSG_SUCCESS)
		{ LogMsg("HandleSIGTERM: mach_msg_send failed; Exiting immediately."); exit(-1); }
	}

mDNSlocal void INFOCallback(CFMachPortRef port, void *msg, CFIndex size, void *info)
	{
	(void)port;		// Unused
	(void)msg;		// Unused
	(void)size;		// Unused
	(void)info;		// Unused
	DNSServiceDomainEnumeration *e;
	DNSServiceBrowser           *b;
	DNSServiceResolver          *l;
	DNSServiceRegistration      *r;
	NetworkInterfaceInfoOSX     *i;
	mDNSu32 slot;
	CacheRecord *rr;
	mDNSu32 CacheUsed = 0, CacheActive = 0;
	mDNSs32 now = mDNSPlatformTimeNow();

	LogMsgIdent(mDNSResponderVersionString, "---- BEGIN STATE LOG ----");

	for (slot = 0; slot < CACHE_HASH_SLOTS; slot++)
		{
		mDNSu32 SlotUsed = 0;
		for (rr = mDNSStorage.rrcache_hash[slot]; rr; rr=rr->next)
			{
			mDNSs32 remain = rr->resrec.rroriginalttl - (now - rr->TimeRcvd) / mDNSPlatformOneSecond;
			CacheUsed++;
			SlotUsed++;
			if (rr->CRActiveQuestion) CacheActive++;
			LogMsgNoIdent("%s%6ld %-6s%-6s%s", rr->CRActiveQuestion ? "*" : " ", remain, DNSTypeName(rr->resrec.rrtype),
				((NetworkInterfaceInfoOSX *)rr->resrec.InterfaceID)->ifa_name, GetRRDisplayString(&mDNSStorage, rr));
			usleep(1000);	// Limit rate a little so we don't flood syslog too fast
			}
		if (mDNSStorage.rrcache_used[slot] != SlotUsed)
			LogMsgNoIdent("Cache use mismatch: rrcache_used[slot] is %lu, true count %lu", mDNSStorage.rrcache_used[slot], SlotUsed);
		}
	if (mDNSStorage.rrcache_totalused != CacheUsed)
		LogMsgNoIdent("Cache use mismatch: rrcache_totalused is %lu, true count %lu", mDNSStorage.rrcache_totalused, CacheUsed);
	if (mDNSStorage.rrcache_active != CacheActive)
		LogMsgNoIdent("Cache use mismatch: rrcache_active is %lu, true count %lu", mDNSStorage.rrcache_active, CacheActive);
	LogMsgNoIdent("Cache currently contains %lu records; %lu referenced by active questions", CacheUsed, CacheActive);

	for (e = DNSServiceDomainEnumerationList; e; e=e->next)
		LogMsgNoIdent("%5d: DomainEnumeration   %##s", e->ClientMachPort, e->dom.qname.c);

	for (b = DNSServiceBrowserList; b; b=b->next)
		{
		DNSServiceBrowserQuestion *qptr;
		for (qptr = b->qlist; qptr; qptr = qptr->next)
			LogMsgNoIdent("%5d: ServiceBrowse       %##s", b->ClientMachPort, qptr->q.qname.c);
		}
	for (l = DNSServiceResolverList; l; l=l->next)
		LogMsgNoIdent("%5d: ServiceResolve      %##s", l->ClientMachPort, l->i.name.c);

	for (r = DNSServiceRegistrationList; r; r=r->next)
		{
		LogMsgNoIdent("%5d: ServiceRegistration %##s %u", r->ClientMachPort, r->ls.RR_SRV.resrec.name.c, mDNSVal16(r->ls.RR_SRV.resrec.rdata->u.srv.port));
		if (r->gs) LogMsgNoIdent("%5d: ServiceRegistration %##s %u", r->ClientMachPort, r->gs->RR_SRV.resrec.name.c, mDNSVal16(r->gs->RR_SRV.resrec.rdata->u.srv.port));
		}

	udsserver_info();

	for (i = mDNSStorage.p->InterfaceList; i; i = i->next)
		{
		if (!i->Exists)
			LogMsgNoIdent("Interface: %s %5s(%lu) DORMANT",
				i->sa_family == AF_INET ? "v4" : i->sa_family == AF_INET6 ? "v6" : "??", i->ifa_name, i->scope_id);
		else
			LogMsgNoIdent("Interface: %s %5s(%lu) %s %s %2d %s %2d InterfaceID %p %s %s %#a",
				i->sa_family == AF_INET ? "v4" : i->sa_family == AF_INET6 ? "v6" : "??", i->ifa_name, i->scope_id,
				i->ifinfo.InterfaceActive ? "Active" : "      ",
				i->ifinfo.IPv4Available ? "v4" : "  ", i->ss.sktv4,
				i->ifinfo.IPv6Available ? "v6" : "  ", i->ss.sktv6,
				i->ifinfo.InterfaceID,
				i->ifinfo.Advertise ? "Adv"  : "   ",
				i->ifinfo.McastTxRx ? "TxRx" : "    ",
				&i->ifinfo.ip);
		}

	LogMsgIdent(mDNSResponderVersionString, "----  END STATE LOG  ----");
	}

mDNSlocal void HandleSIGINFO(int signal)
	{
	(void)signal;		// Unused
	mach_msg_header_t header;
	header.msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_MAKE_SEND, 0);
	header.msgh_remote_port = info_m_port;
	header.msgh_local_port = MACH_PORT_NULL;
	header.msgh_size = sizeof(header);
	header.msgh_id = 0;
	if (mach_msg_send(&header) != MACH_MSG_SUCCESS)
		LogMsg("HandleSIGINFO: mach_msg_send failed; No state log will be generated.");
	}

mDNSlocal kern_return_t mDNSDaemonInitialize(void)
	{
	mStatus            err;
	CFMachPortRef      d_port = CFMachPortCreate(NULL, ClientDeathCallback, NULL, NULL);
	CFMachPortRef      s_port = CFMachPortCreate(NULL, DNSserverCallback, NULL, NULL);
	CFMachPortRef      e_port = CFMachPortCreate(NULL, ExitCallback, NULL, NULL);
	CFMachPortRef      i_port = CFMachPortCreate(NULL, INFOCallback, NULL, NULL);
	mach_port_t        m_port = CFMachPortGetPort(s_port);
	char *MachServerName = mDNSMacOSXSystemBuildNumber(NULL) < 7 ? "DNSServiceDiscoveryServer" : "com.apple.mDNSResponder";
	kern_return_t      status = bootstrap_register(bootstrap_port, MachServerName, m_port);
	CFRunLoopSourceRef d_rls  = CFMachPortCreateRunLoopSource(NULL, d_port, 0);
	CFRunLoopSourceRef s_rls  = CFMachPortCreateRunLoopSource(NULL, s_port, 0);
	CFRunLoopSourceRef e_rls  = CFMachPortCreateRunLoopSource(NULL, e_port, 0);
	CFRunLoopSourceRef i_rls  = CFMachPortCreateRunLoopSource(NULL, i_port, 0);
	
	if (status)
		{
		if (status == 1103)
			LogMsg("Bootstrap_register failed(): A copy of the daemon is apparently already running");
		else
			LogMsg("Bootstrap_register failed(): %s %d", mach_error_string(status), status);
		return(status);
		}

	err = mDNS_Init(&mDNSStorage, &PlatformStorage,
		rrcachestorage, RR_CACHE_SIZE,
		mDNS_Init_AdvertiseLocalAddresses,
		mDNS_StatusCallback, mDNS_Init_NoInitCallbackContext);
	
	if (err) { LogMsg("Daemon start: mDNS_Init failed %ld", err); return(err); }

	client_death_port = CFMachPortGetPort(d_port);
	exit_m_port = CFMachPortGetPort(e_port);
	info_m_port = CFMachPortGetPort(i_port);

	CFRunLoopAddSource(CFRunLoopGetCurrent(), d_rls, kCFRunLoopDefaultMode);
	CFRunLoopAddSource(CFRunLoopGetCurrent(), s_rls, kCFRunLoopDefaultMode);
	CFRunLoopAddSource(CFRunLoopGetCurrent(), e_rls, kCFRunLoopDefaultMode);
	CFRunLoopAddSource(CFRunLoopGetCurrent(), i_rls, kCFRunLoopDefaultMode);
	CFRelease(d_rls);
	CFRelease(s_rls);
	CFRelease(e_rls);
	CFRelease(i_rls);
	if (mDNS_DebugMode) printf("Service registered with Mach Port %d\n", m_port);
	err = udsserver_init(&mDNSStorage);
	if (err) { LogMsg("Daemon start: udsserver_init failed"); return err; }
	return(err);
	}

mDNSlocal mDNSs32 mDNSDaemonIdle(void)
	{
	// 1. Call mDNS_Execute() to let mDNSCore do what it needs to do
	mDNSs32 nextevent = mDNS_Execute(&mDNSStorage);

	mDNSs32 now = mDNSPlatformTimeNow();	
	
	// 2. Deliver any waiting browse messages to clients
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
			LogMsg("%5d: DNSServiceResolver(%##s) active for over two minutes. "
				"This places considerable burden on the network.", l->ClientMachPort, l->i.name.c);
			}

	return(nextevent);
	}

mDNSexport int main(int argc, char **argv)
	{
	int i;
	kern_return_t status;
	
	for (i=1; i<argc; i++)
		{
		if (!strcmp(argv[i], "-d")) mDNS_DebugMode = mDNStrue;
		}

	signal(SIGINT,  HandleSIGTERM);		// SIGINT is what you get for a Ctrl-C
	signal(SIGTERM, HandleSIGTERM);
	signal(SIGINFO, HandleSIGINFO);

	// Register the server with mach_init for automatic restart only during normal (non-debug) mode
    if (!mDNS_DebugMode) registerBootstrapService();

	if (!mDNS_DebugMode && !restarting_via_mach_init)
		exit(0); /* mach_init will restart us immediately as a daemon */

	if (!mDNS_DebugMode)
		{
		int fd = open(_PATH_DEVNULL, O_RDWR, 0);
		if (fd != -1)
			{
			// Avoid unnecessarily duplicating a file descriptor to itself
			if (fd != STDIN_FILENO) (void)dup2(fd, STDIN_FILENO);
			if (fd != STDOUT_FILENO) (void)dup2(fd, STDOUT_FILENO);
			if (fd != STDERR_FILENO) (void)dup2(fd, STDERR_FILENO);
			if (fd != STDIN_FILENO && fd != STDOUT_FILENO && fd != STDERR_FILENO) 
				(void)close (fd);
			}
		}

	// First do the all the initialization we need root privilege for, before we change to user "nobody"
	LogMsgIdent(mDNSResponderVersionString, "starting");
	status = mDNSDaemonInitialize();

	// Now that we're finished with anything privileged, switch over to running as "nobody"
	const struct passwd *pw = getpwnam("nobody");
	if (pw != NULL)
		setuid(pw->pw_uid);
	else
		setuid(-2);		// User "nobody" is -2; use that value if "nobody" does not appear in the password database

	if (status == 0)
		{
		int numevents = 0;
		int RunLoopStatus = kCFRunLoopRunTimedOut;
		
		// This is the main work loop:
		// (1) First we give mDNSCore a chance to finish off any of its deferred work and calculate the next sleep time
		// (2) Then we make sure we've delivered all waiting browse messages to our clients
		// (3) Then we sleep for the time requested by mDNSCore, or until the next event, whichever is sooner
		// (4) On wakeup we first process *all* events
		// (5) then when no more events remain, we go back to (1) to finish off any deferred work and do it all again
		while (RunLoopStatus == kCFRunLoopRunTimedOut)
			{
			// 1. Before going into a blocking wait call and letting our process to go sleep,
			// call mDNSDaemonIdle to allow any deferred work to be completed.
			mDNSs32 nextevent = mDNSDaemonIdle();
			nextevent = udsserver_idle(nextevent);

			// 2. Work out how long we expect to sleep before the next scheduled task
			mDNSs32 ticks = nextevent - mDNSPlatformTimeNow();
			static mDNSs32 RepeatedBusy = 0;	// Debugging sanity check, to guard against CPU spins
			if (ticks > 1)
				RepeatedBusy = 0;
			else
				{
				ticks = 1;
				if (++RepeatedBusy >= mDNSPlatformOneSecond * 10)
					{ LogMsg("Task Scheduling Error: Continuously busy for the last ten seconds"); RepeatedBusy = 0; }
				}
			CFAbsoluteTime interval = (CFAbsoluteTime)ticks / (CFAbsoluteTime)mDNSPlatformOneSecond;
					
			// 3. Now do a blocking "CFRunLoopRunInMode" call so we sleep until
			// (a) our next wakeup time, or (b) an event occurs.
			// The 'true' parameter makes it return after handling any event that occurs
			// This gives us chance to regain control so we can call mDNS_Execute() before sleeping again
			verbosedebugf("main: Handled %d events; now sleeping for %d ticks", numevents, ticks);
			numevents = 0;
			RunLoopStatus = CFRunLoopRunInMode(kCFRunLoopDefaultMode, interval, true);

			// 4. Time to do some work? Handle all remaining events as quickly as we can, before returning to mDNSDaemonIdle()
			while (RunLoopStatus == kCFRunLoopRunHandledSource)
				{
				numevents++;
				RunLoopStatus = CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0.0, true);
				}
			}

		LogMsg("ERROR: CFRunLoopRun Exiting.");
		mDNS_Close(&mDNSStorage);
		}

	if (!mDNS_DebugMode) destroyBootstrapService();

	return(status);
	}

//		uds_daemon.c support routines		/////////////////////////////////////////////

// We keep a list of client-supplied event sources in PosixEventSource records 
struct CFSocketEventSource
	{
	udsEventCallback			Callback;
	void						*Context;
	int							fd;
	struct  CFSocketEventSource	*Next;
	CFSocketRef					cfs;
	CFRunLoopSourceRef			RLS;
	};
typedef struct CFSocketEventSource	CFSocketEventSource;

static GenLinkedList	gEventSources;			// linked list of CFSocketEventSource's

static void cf_callback(CFSocketRef s _UNUSED, CFSocketCallBackType t _UNUSED, CFDataRef dr _UNUSED, const void *c _UNUSED, void *i)
	// Called by CFSocket when data appears on socket
	{
	CFSocketEventSource	*source = (CFSocketEventSource*) i;
	source->Callback(source->Context);
	}

mStatus udsSupportAddFDToEventLoop(int fd, udsEventCallback callback, void *context)
	// Arrange things so that callback is called with context when data appears on fd
	{
	CFSocketEventSource	*newSource;
	CFSocketContext cfContext = 	{ 0, NULL, NULL, NULL, NULL 	};
	
	if (gEventSources.LinkOffset == 0)
		InitLinkedList(&gEventSources, offsetof(CFSocketEventSource, Next));

	if (fd >= FD_SETSIZE || fd < 0)
		return mStatus_UnsupportedErr;
	if (callback == NULL)
		return mStatus_BadParamErr;

	newSource = (CFSocketEventSource*) calloc(1, sizeof *newSource);
	if (NULL == newSource)
		return mStatus_NoMemoryErr;

	newSource->Callback = callback;
	newSource->Context = context;
	newSource->fd = fd;

	cfContext.info = newSource;
	if ( NULL != (newSource->cfs = CFSocketCreateWithNative(kCFAllocatorDefault, fd, kCFSocketReadCallBack, 
																	cf_callback, &cfContext)) &&
		 NULL != (newSource->RLS = CFSocketCreateRunLoopSource(kCFAllocatorDefault, newSource->cfs, 0)))
		{
		CFRunLoopAddSource(CFRunLoopGetCurrent(), newSource->RLS, kCFRunLoopDefaultMode);
		AddToTail(&gEventSources, newSource);
		}
	else
		{
		if (newSource->cfs)
			{
			CFSocketInvalidate(newSource->cfs);  // automatically closes socket
			CFRelease(newSource->cfs);
			}
		return mStatus_NoMemoryErr;
		}

	return mStatus_NoError;
	}

mStatus udsSupportRemoveFDFromEventLoop(int fd)
	// Reverse what was done in udsSupportAddFDToEventLoop().
	{
	CFSocketEventSource	*iSource;

	for (iSource=(CFSocketEventSource*)gEventSources.Head; iSource; iSource = iSource->Next)
		{
		if (fd == iSource->fd)
			{
			RemoveFromList(&gEventSources, iSource);
			CFRunLoopRemoveSource(CFRunLoopGetCurrent(), iSource->RLS, kCFRunLoopDefaultMode);
			CFRunLoopSourceInvalidate(iSource->RLS);
			CFRelease(iSource->RLS);
			CFSocketInvalidate(iSource->cfs);
			CFRelease(iSource->cfs);
			free(iSource);
			return mStatus_NoError;
			}
		}
	return mStatus_NoSuchNameErr;
	}

// For convenience when using the "strings" command, this is the last thing in the file
mDNSexport const char mDNSResponderVersionString[] = STRINGIFY(mDNSResponderVersion) " (" __DATE__ " " __TIME__ ")";
