/*
 * Copyright (c) 2002 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 *
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 1.1 (the
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

#include <mach/mach.h>
#include <mach/mach_error.h>
#include <servers/bootstrap.h>
#include <sys/types.h>
#include <unistd.h>

#include "DNSServiceDiscoveryRequestServer.h"
#include "DNSServiceDiscoveryReply.h"

#include "mDNSClientAPI.h"				// Defines the interface to the client layer above
#include "mDNSPlatformEnvironment.h"	// Defines the specific types needed to run mDNS on this platform

// Globals
static mDNS mDNSStorage;
static mDNS_PlatformSupport PlatformStorage;
#define RR_CACHE_SIZE 500
static ResourceRecord rrcachestorage[RR_CACHE_SIZE];
const char PID_FILE[] = "/var/run/mDNSResponder.pid";

#if DEBUGBREAKS
static int debug_mode = 1;
#else
static int debug_mode = 0;
#endif
int use_53 = 1;

//*************************************************************************************************************
// General Utility Functions

char *CorrectRegType(char *regtype)
	{
	if (!strncmp(regtype, "_afp.", 5)) { fprintf(stderr, "mDNSResponder: Illegal protocol name _afp changed to _afpovertcp\n"); return("_afpovertcp._tcp"); }
	if (!strncmp(regtype, "_lpr.", 5)) { fprintf(stderr, "mDNSResponder: Illegal protocol name _lpr changed to _printer\n");    return("_printer._tcp"); }
	if (!strncmp(regtype, "_lpd.", 5)) { fprintf(stderr, "mDNSResponder: Illegal protocol name _lpd changed to _printer\n");    return("_printer._tcp"); }
	return(regtype);
	}

//*************************************************************************************************************
// Client Death Detection

typedef struct DNSServiceDomainEnumeration_struct DNSServiceDomainEnumeration;
struct DNSServiceDomainEnumeration_struct
	{
	DNSServiceDomainEnumeration *next;
	DNSQuestion dom;	// Question asking for domains
	DNSQuestion def;	// Question asking for default domain
	mach_port_t port;
	};

typedef struct DNSServiceBrowser_struct DNSServiceBrowser;
struct DNSServiceBrowser_struct
	{
	DNSServiceBrowser *next;
	DNSQuestion q;
	mach_port_t port;
	};

typedef struct DNSServiceResolver_struct DNSServiceResolver;
struct DNSServiceResolver_struct
	{
	DNSServiceResolver *next;
	ServiceInfoQuery q;
	ServiceInfo      i;
	mach_port_t port;
	};

typedef struct DNSServiceRegistration_struct DNSServiceRegistration;
struct DNSServiceRegistration_struct
	{
	DNSServiceRegistration *next;
	ServiceRecordSet s;
	mach_port_t port;
	};

static DNSServiceDomainEnumeration *DNSServiceDomainEnumerationList = NULL;
static DNSServiceBrowser           *DNSServiceBrowserList           = NULL;
static DNSServiceResolver          *DNSServiceResolverList          = NULL;
static DNSServiceRegistration      *DNSServiceRegistrationList      = NULL;

static mach_port_t client_death_port;

static void AbortClient(mach_port_t port)
	{
	DNSServiceDomainEnumeration **e = &DNSServiceDomainEnumerationList;
	DNSServiceBrowser           **b = &DNSServiceBrowserList;
	DNSServiceResolver          **l = &DNSServiceResolverList;
	DNSServiceRegistration      **r = &DNSServiceRegistrationList;

	while (*e && (*e)->port != port) e = &(*e)->next;
	if (*e)
		{
		DNSServiceDomainEnumeration *x = *e;
		*e = (*e)->next;
		debugf("Aborting DNSServiceDomainEnumeration %d", port);
		mDNS_StopGetDomains(&mDNSStorage, &x->dom);
		mDNS_StopGetDomains(&mDNSStorage, &x->def);
		free(x);
		return;
		}

	while (*b && (*b)->port != port) b = &(*b)->next;
	if (*b)
		{
		DNSServiceBrowser *x = *b;
		*b = (*b)->next;
		debugf("Aborting DNSServiceBrowser %d", port);
		mDNS_StopBrowse(&mDNSStorage, &x->q);
		free(x);
		return;
		}

	while (*l && (*l)->port != port) l = &(*l)->next;
	if (*l)
		{
		DNSServiceResolver *x = *l;
		*l = (*l)->next;
		debugf("Aborting DNSServiceResolver %d", port);
		mDNS_StopResolveService(&mDNSStorage, &x->q);
		free(x);
		return;
		}

	while (*r && (*r)->port != port) r = &(*r)->next;
	if (*r)
		{
		DNSServiceRegistration *x = *r;
		*r = (*r)->next;
		debugf("Aborting DNSServiceRegistration %d", port);
		mDNS_DeregisterService(&mDNSStorage, &x->s);
		return;
		}
	}

static void ClientDeathCallback(CFMachPortRef port, void *voidmsg, CFIndex size, void *info)
	{
	mach_msg_header_t *msg = (mach_msg_header_t *)voidmsg;
	if (msg->msgh_id == MACH_NOTIFY_DEAD_NAME)
		{
		const mach_dead_name_notification_t *const deathMessage = (mach_dead_name_notification_t *)msg;
		debugf("Client on port %d died or deallocated", deathMessage->not_port);
		AbortClient(deathMessage->not_port);

		/* Deallocate the send right that came in the dead name notification */
		mach_port_destroy( mach_task_self(), deathMessage->not_port );

		}
	}

static void EnableDeathNotificationForClient(mach_port_t port)
	{
	mach_port_t prev;
	kern_return_t r = mach_port_request_notification(mach_task_self(), port, MACH_NOTIFY_DEAD_NAME, 0,
													 client_death_port, MACH_MSG_TYPE_MAKE_SEND_ONCE, &prev);
	// If the port already died while we were thinking about it, then abort the operation right away
	if (r != KERN_SUCCESS) AbortClient(port);
	}

//*************************************************************************************************************
// Domain Enumeration

static void FoundDomain(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer)
	{
	#pragma unused(m)
	char buffer[256];
	DNSServiceDomainEnumerationReplyResultType rt;
	DNSServiceDomainEnumeration *x = (DNSServiceDomainEnumeration *)question->Context;

	debugf("FoundDomain: %##s PTR %##s", answer->name.c, answer->rdata.name.c);
	if (answer->rrtype != kDNSType_PTR) return;
	if (!x) { debugf("FoundDomain: DNSServiceDomainEnumeration is NULL"); return; }

	if (answer->rrremainingttl > 0)
		{
		if (question == &x->dom) rt = DNSServiceDomainEnumerationReplyAddDomain;
		else                     rt = DNSServiceDomainEnumerationReplyAddDomainDefault;
		}
	else
		{
		if (question == &x->dom) rt = DNSServiceDomainEnumerationReplyRemoveDomain;
		else return;
		}

	ConvertDomainNameToCString(&answer->rdata.name, buffer);
	DNSServiceDomainEnumerationReply_rpc(x->port, rt, buffer, 0);
	}

kern_return_t provide_DNSServiceDomainEnumerationCreate_rpc(mach_port_t server, mach_port_t client, int regDom)
	{
	mStatus err;
	mDNS_DomainType dt1 = regDom ? mDNS_DomainTypeRegistration        : mDNS_DomainTypeBrowse;
	mDNS_DomainType dt2 = regDom ? mDNS_DomainTypeRegistrationDefault : mDNS_DomainTypeBrowseDefault;
	DNSServiceDomainEnumeration *x = malloc(sizeof(*x));
	if (!x) { debugf("provide_DNSServiceDomainEnumerationCreate_rpc: No memory!"); return(-1); }
	x->port = client;
	x->next = DNSServiceDomainEnumerationList;
	DNSServiceDomainEnumerationList = x;
	
	debugf("Client %d: Enumerate %s Domains", client, regDom ? "Registration" : "Browsing");
	// We always give local.arpa. as the initial default browse domain, and then look for more
	DNSServiceDomainEnumerationReply_rpc(x->port, DNSServiceDomainEnumerationReplyAddDomainDefault, "local.arpa.", 0);
	err           = mDNS_GetDomains(&mDNSStorage, &x->dom, dt1, zeroIPAddr, FoundDomain, x);
	if (!err) err = mDNS_GetDomains(&mDNSStorage, &x->def, dt2, zeroIPAddr, FoundDomain, x);
	if (!err) EnableDeathNotificationForClient(client);
	else debugf("provide_DNSServiceDomainEnumerationCreate_rpc: Error %d", err);
	return(err);
	}

//*************************************************************************************************************
// Browse for services

mDNSlocal void FoundInstance(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer)
	{
	DNSServiceBrowser *x = (DNSServiceBrowser *)question->Context;
	int resultType = DNSServiceBrowserReplyAddInstance;
	domainlabel name;
	domainname type, domain;
	char c_name[256], c_type[256], c_dom[256];
	if (answer->rrtype != kDNSType_PTR) return;
	debugf("FoundInstance: %##s", answer->rdata.name.c);
	DeconstructServiceName(&answer->rdata.name, &name, &type, &domain);
	ConvertDomainLabelToCString_unescaped(&name, c_name);
	ConvertDomainNameToCString(&type, c_type);
	ConvertDomainNameToCString(&domain, c_dom);
	if (answer->rrremainingttl == 0) resultType = DNSServiceBrowserReplyRemoveInstance;
	debugf("DNSServiceBrowserReply_rpc sending reply for %s", c_name);
	DNSServiceBrowserReply_rpc(x->port, resultType, c_name, c_type, c_dom, 0);
	}

kern_return_t provide_DNSServiceBrowserCreate_rpc(mach_port_t server, mach_port_t client,
	DNSCString regtype, DNSCString domain)
	{
	mStatus err;
	domainname t, d;
	DNSServiceBrowser *x = malloc(sizeof(*x));
	if (!x) { debugf("provide_DNSServiceBrowserCreate_rpc: No memory!"); return(-1); }
	x->port = client;
	x->next = DNSServiceBrowserList;
	DNSServiceBrowserList = x;

	regtype = CorrectRegType(regtype);
	ConvertCStringToDomainName(regtype, &t);
	if (*domain && *domain != '.') ConvertCStringToDomainName(domain, &d);
	else ConvertCStringToDomainName("local.arpa.", &d);

	debugf("Client %d: provide_DNSServiceBrowserCreate_rpc", client);
	debugf("Client %d: Browse for Services: %##s%##s", client, &t, &d);
	err = mDNS_StartBrowse(&mDNSStorage, &x->q, &t, &d, zeroIPAddr, FoundInstance, x);
	if (!err) EnableDeathNotificationForClient(client);
	else debugf("provide_DNSServiceBrowserCreate_rpc: mDNS_StartBrowse failed");
	return(err);
	}

//*************************************************************************************************************
// Resolve Service Info

static void FoundInstanceInfo(mDNS *const m, ServiceInfoQuery *query)
	{
	DNSServiceResolver *x = (DNSServiceResolver *)query->Context;
	struct sockaddr_in interface;
	struct sockaddr_in address;

	interface.sin_len         = sizeof(interface);
	interface.sin_family      = AF_INET;
	interface.sin_port        = 0;
	interface.sin_addr.s_addr = query->info->InterfaceAddr.NotAnInteger;
	
	address.sin_len           = sizeof(address);
	address.sin_family        = AF_INET;
	address.sin_port          = query->info->port.NotAnInteger;
	address.sin_addr.s_addr   = query->info->ip.NotAnInteger;
	
	DNSServiceResolverReply_rpc(x->port, (char*)&interface, (char*)&address, query->info->txtinfo.c, 0);
	}

kern_return_t provide_DNSServiceResolverResolve_rpc(mach_port_t server, mach_port_t client,
	DNSCString name, DNSCString regtype, DNSCString domain)
	{
	mStatus err;
	domainlabel n;
	domainname t, d;
	DNSServiceResolver *x = malloc(sizeof(*x));
	if (!x) { debugf("provide_DNSServiceResolverResolve_rpc: No memory!"); return(-1); }
	x->port = client;
	x->next = DNSServiceResolverList;
	DNSServiceResolverList = x;

	ConvertCStringToDomainLabel(name, &n);
	regtype = CorrectRegType(regtype);
	ConvertCStringToDomainName(regtype, &t);
	if (*domain && *domain != '.') ConvertCStringToDomainName(domain, &d);
	else ConvertCStringToDomainName("local.arpa.", &d);
	ConstructServiceName(&x->i.name, &n, &t, &d);
	x->i.InterfaceAddr = zeroIPAddr;

	debugf("Client %d: provide_DNSServiceResolverResolve_rpc", client);
	debugf("Client %d: Resolve Service: %##s", client, &x->i.name);
	err = mDNS_StartResolveService(&mDNSStorage, &x->q, &x->i, FoundInstanceInfo, x);
	if (!err) EnableDeathNotificationForClient(client);
	else debugf("provide_DNSServiceResolverResolve_rpc: mDNS_StartResolveService failed");
	return(err);
	}

//*************************************************************************************************************
// Registration

// This sample Callback just calls mDNS_RenameAndReregisterService to automatically pick a new
// unique name for the service. For a device such as a printer, this may be appropriate.
// For a device with a user interface, and a screen, and a keyboard, the appropriate
// response may be to prompt the user and ask them to choose a new name for the service.
mDNSlocal void Callback(mDNS *const m, ServiceRecordSet *const sr, mStatus result)
	{
	DNSServiceRegistration *x = (DNSServiceRegistration*)sr->Context;
	mach_port_t port = x->port;
	switch (result)
		{
		case mStatus_NoError:      debugf("Callback: %##s Name Registered",   sr->RR_SRV.name.c); break;
		case mStatus_NameConflict: debugf("Callback: %##s Name Conflict",     sr->RR_SRV.name.c); break;
		case mStatus_MemFree:      debugf("Callback: %##s Memory Free",       sr->RR_SRV.name.c); break;
		default:                   debugf("Callback: %##s Unknown Result %d", sr->RR_SRV.name.c, result); break;
		}
	if (result == mStatus_NoError) DNSServiceRegistrationReply_rpc(port, result);
//	if (result == mStatus_NameConflict) mDNS_RenameAndReregisterService(m, sr);
	if (result == mStatus_NameConflict) { AbortClient(port); DNSServiceRegistrationReply_rpc(port, result); free(x); }
	if (result == mStatus_MemFree) { debugf("Freeing DNSServiceRegistration %d", x->port); free(x); }
	}

kern_return_t provide_DNSServiceRegistrationCreate_rpc(mach_port_t server, mach_port_t client,
	DNSCString name, DNSCString regtype, DNSCString domain, int notAnIntPort, DNSCString txtRecord)
	{
	mStatus err;
	domainlabel n;
	domainname t, d;
	mDNSIPPort port;
	DNSServiceRegistration *x = malloc(sizeof(*x));
	if (!x) { debugf("DNSServiceRegistrationRegister: No memory!"); return(-1); }
	x->port = client;
	x->next = DNSServiceRegistrationList;
	DNSServiceRegistrationList = x;

	if (*name && *name != '.') ConvertCStringToDomainLabel(name, &n);
	else n = mDNSStorage.nicelabel;
	regtype = CorrectRegType(regtype);
	ConvertCStringToDomainName(regtype, &t);
	if (*domain && *domain != '.') ConvertCStringToDomainName(domain, &d);
	else ConvertCStringToDomainName("local.arpa.", &d);
	port.NotAnInteger = notAnIntPort;

	debugf("Client %d: provide_DNSServiceRegistrationCreate_rpc", client);
	debugf("Client %d: Register Service: %#s.%##s%##s %d %s", client, &n, &t, &d, (int)port.b[0] << 8 | port.b[1], txtRecord);
	err = mDNS_RegisterService(&mDNSStorage, &x->s, port, txtRecord, &n, &t, &d, Callback, x);
	if (!err) EnableDeathNotificationForClient(client);
	debugf("Made Service Record Set for %##s", &x->s.RR_SRV.name);
	return(err);
	}

//*************************************************************************************************************
// Support Code

void
DNSserverCallback(CFMachPortRef port, void *msg, CFIndex size, void *info)
{
	mig_reply_error_t		*request = msg;
	mig_reply_error_t		*reply;
	mach_msg_return_t		mr;
	int				options;

	/* allocate a reply buffer */
	reply = CFAllocatorAllocate(NULL, provide_DNSServiceDiscoveryRequest_subsystem.maxsize, 0);

	/* call the MiG server routine */
	(void) DNSServiceDiscoveryRequest_server(&request->Head, &reply->Head);

	if (!(reply->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) && (reply->RetCode != KERN_SUCCESS)) {

        if (reply->RetCode == MIG_NO_REPLY) {
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

    if (reply->Head.msgh_remote_port == MACH_PORT_NULL) {
        /* no reply port, so destroy the reply */
        if (reply->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) {
            mach_msg_destroy(&reply->Head);
        }
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
    if (MACH_MSGH_BITS_REMOTE(reply->Head.msgh_bits) == MACH_MSG_TYPE_MOVE_SEND_ONCE) {
        options |= MACH_SEND_TIMEOUT;
    }
    mr = mach_msg(&reply->Head,		/* msg */
		      options,			/* option */
		      reply->Head.msgh_size,	/* send_size */
		      0,			/* rcv_size */
		      MACH_PORT_NULL,		/* rcv_name */
		      MACH_MSG_TIMEOUT_NONE,	/* timeout */
		      MACH_PORT_NULL);		/* notify */


    /* Has a message error occurred? */
    switch (mr) {
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

kern_return_t start(const char *bundleName, const char *bundleDir)
	{
	mStatus            err;
	CFMachPortRef      d_port = CFMachPortCreate(NULL, ClientDeathCallback, NULL, NULL);
	CFMachPortRef      s_port = CFMachPortCreate(NULL, DNSserverCallback, NULL, NULL);
	mach_port_t        m_port = CFMachPortGetPort(s_port);
	kern_return_t      status = bootstrap_register(bootstrap_port, DNS_SERVICE_DISCOVERY_SERVER, m_port);
	CFRunLoopSourceRef d_rls  = CFMachPortCreateRunLoopSource(NULL, d_port, 0);
	CFRunLoopSourceRef s_rls  = CFMachPortCreateRunLoopSource(NULL, s_port, 0);
	if (status)
		{
		fprintf(stderr, "Bootstrap_register failed(): %s %d\n", mach_error_string(status), status);
		return(status);
		}
	
	err = mDNS_Init(&mDNSStorage, &PlatformStorage, rrcachestorage, RR_CACHE_SIZE, NULL, NULL);
	if (err) { fprintf(stderr, "Daemon start: mDNS_Init failed %ld\n", err); return(err); }

	client_death_port = CFMachPortGetPort(d_port);
	CFRunLoopAddSource(CFRunLoopGetCurrent(), d_rls, kCFRunLoopDefaultMode);
	CFRunLoopAddSource(CFRunLoopGetCurrent(), s_rls, kCFRunLoopDefaultMode);
	CFRelease(d_rls);
	CFRelease(s_rls);
	if (debug_mode) printf("Service registered with Mach Port %d\n", m_port);

	return(err);
	}

int main(int argc, char **argv)
	{
	int i;
	kern_return_t status;

	for (i=1; i<argc; i++)
		{
		if (!strcmp(argv[i], "-d")) debug_mode = 1;
		if (!strcmp(argv[i], "-no53")) use_53 = 0;
		}

	status = start(NULL, NULL);

	if (status == 0)
		{
		if (!debug_mode)
			{
			FILE *fp = fopen(PID_FILE, "w");
			if (fp != NULL)
				{
				fprintf(fp, "%d\n", getpid());
				fclose(fp);
				}
			daemon(0,0);
			}
		CFRunLoopRun();
		}

	return(status);
	}
