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

/*
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

#include <DNSServiceDiscovery/DNSServiceDiscovery.h>

// Globals
static mDNS mDNSStorage;
static mDNS_PlatformSupport PlatformStorage;
#define RR_CACHE_SIZE 500
static ResourceRecord rrcachestorage[RR_CACHE_SIZE];
static const char PID_FILE[] = "/var/run/mDNSResponder.pid";

static const char kmDNSBootstrapName[] = "com.apple.mDNSResponder";
static mach_port_t client_death_port = MACH_PORT_NULL;;
static mach_port_t exit_m_port       = MACH_PORT_NULL;;
static mach_port_t server_priv_port  = MACH_PORT_NULL;

static int restarting_via_mach_init = 0;

#if DEBUGBREAKS
static int debug_mode = 1;
#else
static int debug_mode = 0;
#endif

//*************************************************************************************************************
// General Utility Functions

void LogErrorMessage(const char *format, ...)
	{
	va_list ap;
	va_start(ap, format);
	openlog("mDNSResponder", LOG_CONS | LOG_PERROR | LOG_PID, LOG_DAEMON);
	vsyslog(LOG_ERR, format, ap);
	closelog();
	va_end(ap);
	}

//*************************************************************************************************************
// Client Death Detection

typedef struct DNSServiceDomainEnumeration_struct DNSServiceDomainEnumeration;
struct DNSServiceDomainEnumeration_struct
	{
	DNSServiceDomainEnumeration *next;
	mach_port_t ClientMachPort;
	DNSQuestion dom;	// Question asking for domains
	DNSQuestion def;	// Question asking for default domain
	};

typedef struct DNSServiceBrowser_struct DNSServiceBrowser;
struct DNSServiceBrowser_struct
	{
	DNSServiceBrowser *next;
	mach_port_t ClientMachPort;
	DNSQuestion q;
	};

typedef struct DNSServiceResolver_struct DNSServiceResolver;
struct DNSServiceResolver_struct
	{
	DNSServiceResolver *next;
	mach_port_t ClientMachPort;
	ServiceInfoQuery q;
	ServiceInfo      i;
	};

typedef struct DNSServiceRegistration_struct DNSServiceRegistration;
struct DNSServiceRegistration_struct
	{
	DNSServiceRegistration *next;
	mach_port_t ClientMachPort;
	mDNSBool autoname;
	ServiceRecordSet s;
	};

static DNSServiceDomainEnumeration *DNSServiceDomainEnumerationList = NULL;
static DNSServiceBrowser           *DNSServiceBrowserList           = NULL;
static DNSServiceResolver          *DNSServiceResolverList          = NULL;
static DNSServiceRegistration      *DNSServiceRegistrationList      = NULL;

mDNSlocal void AbortClient(mach_port_t ClientMachPort)
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
		debugf("Aborting DNSServiceDomainEnumeration %d", ClientMachPort);
		mDNS_StopGetDomains(&mDNSStorage, &x->dom);
		mDNS_StopGetDomains(&mDNSStorage, &x->def);
		free(x);
		return;
		}

	while (*b && (*b)->ClientMachPort != ClientMachPort) b = &(*b)->next;
	if (*b)
		{
		DNSServiceBrowser *x = *b;
		*b = (*b)->next;
		debugf("Aborting DNSServiceBrowser %d", ClientMachPort);
		mDNS_StopBrowse(&mDNSStorage, &x->q);
		free(x);
		return;
		}

	while (*l && (*l)->ClientMachPort != ClientMachPort) l = &(*l)->next;
	if (*l)
		{
		DNSServiceResolver *x = *l;
		*l = (*l)->next;
		debugf("Aborting DNSServiceResolver %d", ClientMachPort);
		mDNS_StopResolveService(&mDNSStorage, &x->q);
		free(x);
		return;
		}

	while (*r && (*r)->ClientMachPort != ClientMachPort) r = &(*r)->next;
	if (*r)
		{
		DNSServiceRegistration *x = *r;
		*r = (*r)->next;
		debugf("Aborting DNSServiceRegistration %d", ClientMachPort);
		mDNS_DeregisterService(&mDNSStorage, &x->s);
		// Note that we don't do the "free(x);" here -- wait for the mStatus_MemFree message
		return;
		}
	}

mDNSlocal void AbortBlockedClient(mach_port_t ClientMachPort, char *msg)
	{
	LogErrorMessage("Client %d stopped accepting Mach messages and has been disconnected from its %s reply",
		ClientMachPort, msg);
	AbortClient(ClientMachPort);
	}

mDNSlocal void ClientDeathCallback(CFMachPortRef unusedport, void *voidmsg, CFIndex size, void *info)
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

mDNSlocal void EnableDeathNotificationForClient(mach_port_t ClientMachPort)
	{
	mach_port_t prev;
	kern_return_t r = mach_port_request_notification(mach_task_self(), ClientMachPort, MACH_NOTIFY_DEAD_NAME, 0,
													 client_death_port, MACH_MSG_TYPE_MAKE_SEND_ONCE, &prev);
	// If the port already died while we were thinking about it, then abort the operation right away
	if (r != KERN_SUCCESS) AbortClient(ClientMachPort);
	}

//*************************************************************************************************************
// Domain Enumeration

mDNSlocal void FoundDomain(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer)
	{
	#pragma unused(m)
	char buffer[256];
	DNSServiceDomainEnumerationReplyResultType rt;
	DNSServiceDomainEnumeration *x = (DNSServiceDomainEnumeration *)question->Context;

	debugf("FoundDomain: %##s PTR %##s", answer->name.c, answer->rdata->u.name.c);
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

	ConvertDomainNameToCString(&answer->rdata->u.name, buffer);
	if (DNSServiceDomainEnumerationReply_rpc(x->ClientMachPort, rt, buffer, 0, 10) == MACH_SEND_TIMED_OUT)
		AbortBlockedClient(x->ClientMachPort, "enumeration");
	}

mDNSexport kern_return_t provide_DNSServiceDomainEnumerationCreate_rpc(mach_port_t unusedserver, mach_port_t client,
	int regDom)
	{
	mStatus err;

	mDNS_DomainType dt1 = regDom ? mDNS_DomainTypeRegistration        : mDNS_DomainTypeBrowse;
	mDNS_DomainType dt2 = regDom ? mDNS_DomainTypeRegistrationDefault : mDNS_DomainTypeBrowseDefault;
	const DNSServiceDomainEnumerationReplyResultType rt = DNSServiceDomainEnumerationReplyAddDomainDefault;
	DNSServiceDomainEnumeration *x = malloc(sizeof(*x));
	if (!x) { debugf("provide_DNSServiceDomainEnumerationCreate_rpc: No memory!"); return(mStatus_NoMemoryErr); }
	x->ClientMachPort = client;
	x->next = DNSServiceDomainEnumerationList;
	DNSServiceDomainEnumerationList = x;
	
	debugf("Client %d: Enumerate %s Domains", client, regDom ? "Registration" : "Browsing");
	// We always give local. as the initial default browse domain, and then look for more
	if (DNSServiceDomainEnumerationReply_rpc(x->ClientMachPort, rt, "local.", 0, 10) == MACH_SEND_TIMED_OUT)
		{
		AbortBlockedClient(x->ClientMachPort, "local enumeration");
		return(mStatus_UnknownErr);
		}

	err           = mDNS_GetDomains(&mDNSStorage, &x->dom, dt1, zeroIPAddr, FoundDomain, x);
	if (!err) err = mDNS_GetDomains(&mDNSStorage, &x->def, dt2, zeroIPAddr, FoundDomain, x);

	if (err) AbortClient(client);
	else EnableDeathNotificationForClient(client);

	if (err) debugf("provide_DNSServiceDomainEnumerationCreate_rpc: mDNS_GetDomains error %d", err);
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
	debugf("FoundInstance: %##s", answer->rdata->u.name.c);
	DeconstructServiceName(&answer->rdata->u.name, &name, &type, &domain);
	ConvertDomainLabelToCString_unescaped(&name, c_name);
	ConvertDomainNameToCString(&type, c_type);
	ConvertDomainNameToCString(&domain, c_dom);
	if (answer->rrremainingttl == 0) resultType = DNSServiceBrowserReplyRemoveInstance;
	debugf("DNSServiceBrowserReply_rpc sending reply for %s", c_name);
	if (DNSServiceBrowserReply_rpc(x->ClientMachPort, resultType, c_name, c_type, c_dom, 0, 10) == MACH_SEND_TIMED_OUT)
		AbortBlockedClient(x->ClientMachPort, "browse");
	}

mDNSexport kern_return_t provide_DNSServiceBrowserCreate_rpc(mach_port_t unusedserver, mach_port_t client,
	DNSCString regtype, DNSCString domain)
	{
	mStatus err;
	domainname t, d;
	DNSServiceBrowser *x = malloc(sizeof(*x));
	if (!x) { debugf("provide_DNSServiceBrowserCreate_rpc: No memory!"); return(mStatus_NoMemoryErr); }
	x->ClientMachPort = client;
	x->next = DNSServiceBrowserList;
	DNSServiceBrowserList = x;

	ConvertCStringToDomainName(regtype, &t);
	ConvertCStringToDomainName(*domain ? domain : "local.", &d);

	debugf("Client %d: provide_DNSServiceBrowserCreate_rpc", client);
	debugf("Client %d: Browse for Services: %##s%##s", client, &t, &d);
	err = mDNS_StartBrowse(&mDNSStorage, &x->q, &t, &d, zeroIPAddr, FoundInstance, x);

	if (err) AbortClient(client);
	else EnableDeathNotificationForClient(client);

	if (err) debugf("provide_DNSServiceBrowserCreate_rpc: mDNS_StartBrowse error %d", err);
	return(err);
	}

//*************************************************************************************************************
// Resolve Service Info

mDNSlocal void FoundInstanceInfo(mDNS *const m, ServiceInfoQuery *query)
	{
	kern_return_t status;
	DNSServiceResolver *x = (DNSServiceResolver *)query->Context;
	int len = query->info->TXTlen;
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

	// Need to put a NULL on the end for the MIG API
	if (len >= sizeof(query->info->TXTinfo) - 1)
		len = sizeof(query->info->TXTinfo) - 1;
	query->info->TXTinfo[len] = 0;
	
	status = DNSServiceResolverReply_rpc(x->ClientMachPort, (char*)&interface, (char*)&address, query->info->TXTinfo, 0, 10);
	if (status == MACH_SEND_TIMED_OUT)
		AbortBlockedClient(x->ClientMachPort, "resolve");
	}

mDNSexport kern_return_t provide_DNSServiceResolverResolve_rpc(mach_port_t unusedserver, mach_port_t client,
	DNSCString name, DNSCString regtype, DNSCString domain)
	{
	mStatus err;
	domainlabel n;
	domainname t, d;
	DNSServiceResolver *x = malloc(sizeof(*x));
	if (!x) { debugf("provide_DNSServiceResolverResolve_rpc: No memory!"); return(mStatus_NoMemoryErr); }
	x->ClientMachPort = client;
	x->next = DNSServiceResolverList;
	DNSServiceResolverList = x;

	ConvertCStringToDomainLabel(name, &n);
	ConvertCStringToDomainName(regtype, &t);
	ConvertCStringToDomainName(*domain ? domain : "local.", &d);
	ConstructServiceName(&x->i.name, &n, &t, &d);
	x->i.InterfaceAddr = zeroIPAddr;

	debugf("Client %d: provide_DNSServiceResolverResolve_rpc", client);
	debugf("Client %d: Resolve Service: %##s", client, &x->i.name);
	err = mDNS_StartResolveService(&mDNSStorage, &x->q, &x->i, FoundInstanceInfo, x);

	if (err) AbortClient(client);
	else EnableDeathNotificationForClient(client);

	if (err) debugf("provide_DNSServiceResolverResolve_rpc: mDNS_StartResolveService error %d", err);
	return(err);
	}

//*************************************************************************************************************
// Registration

mDNSlocal void FreeDNSServiceRegistration(DNSServiceRegistration *x)
	{
	while (x->s.Extras)
		{
		ExtraResourceRecord *extras = x->s.Extras;
		x->s.Extras = x->s.Extras->next;
		if (extras->r.rdata != &extras->r.rdatastorage)
			free(extras->r.rdata);
		free(extras);
		}

	if (x->s.RR_TXT.rdata != &x->s.RR_TXT.rdatastorage)
			free(x->s.RR_TXT.rdata);

	free(x);
	}

mDNSlocal void RegistrationCallback(mDNS *const m, ServiceRecordSet *const sr, mStatus result)
	{
	DNSServiceRegistration *x = (DNSServiceRegistration*)sr->Context;

	switch (result)
		{
		case mStatus_NoError:      debugf("RegistrationCallback: %##s Name Registered",   sr->RR_SRV.name.c); break;
		case mStatus_NameConflict: debugf("RegistrationCallback: %##s Name Conflict",     sr->RR_SRV.name.c); break;
		case mStatus_MemFree:      debugf("RegistrationCallback: %##s Memory Free",       sr->RR_SRV.name.c); break;
		default:                   debugf("RegistrationCallback: %##s Unknown Result %d", sr->RR_SRV.name.c, result); break;
		}

	if (result == mStatus_NoError)
		{
		if (DNSServiceRegistrationReply_rpc(x->ClientMachPort, result, 10) == MACH_SEND_TIMED_OUT)
			AbortBlockedClient(x->ClientMachPort, "registration success");
		}

	if (result == mStatus_NameConflict)
		{
		// Note: By the time we get the mStatus_NameConflict message, the service is already deregistered
		// and the memory is free, so we don't have to wait for an mStatus_MemFree message as well.
		if (x->autoname)
			mDNS_RenameAndReregisterService(m, sr);
		else
			{
			AbortClient(x->ClientMachPort); // This unlinks our DNSServiceRegistration from the list so we can safely free it
			if (DNSServiceRegistrationReply_rpc(x->ClientMachPort, result, 10) == MACH_SEND_TIMED_OUT)
				AbortBlockedClient(x->ClientMachPort, "registration conflict"); // Yes, this IS safe :-)
			FreeDNSServiceRegistration(x);
			}
		}

	if (result == mStatus_MemFree)
		{
		debugf("Freeing DNSServiceRegistration %d", x->ClientMachPort);
		FreeDNSServiceRegistration(x);
		}
	}

mDNSexport kern_return_t provide_DNSServiceRegistrationCreate_rpc(mach_port_t unusedserver, mach_port_t client,
	DNSCString name, DNSCString regtype, DNSCString domain, int notAnIntPort, DNSCString txtRecord)
	{
	mStatus err;
	domainlabel n;
	domainname t, d;
	mDNSIPPort port;
	DNSServiceRegistration *x = malloc(sizeof(*x));
	if (!x) { debugf("provide_DNSServiceRegistrationCreate_rpc: No memory!"); return(mStatus_NoMemoryErr); }
	x->ClientMachPort = client;
	x->next = DNSServiceRegistrationList;
	DNSServiceRegistrationList = x;

	x->autoname = (*name == 0);
	if (x->autoname) n = mDNSStorage.nicelabel;
	else ConvertCStringToDomainLabel(name, &n);
	ConvertCStringToDomainName(regtype, &t);
	ConvertCStringToDomainName(*domain ? domain : "local.", &d);
	port.NotAnInteger = notAnIntPort;

	debugf("Client %d: provide_DNSServiceRegistrationCreate_rpc", client);
	debugf("Client %d: Register Service: %#s.%##s%##s %d %s", client, &n, &t, &d, (int)port.b[0] << 8 | port.b[1], txtRecord);
	err = mDNS_RegisterService(&mDNSStorage, &x->s, &n, &t, &d, mDNSNULL, port, txtRecord, strlen(txtRecord), RegistrationCallback, x);

	if (err) AbortClient(client);
	else EnableDeathNotificationForClient(client);

	if (err) debugf("provide_DNSServiceRegistrationCreate_rpc: mDNS_RegisterService error %d", err);
	else debugf("Made Service Record Set for %##s", &x->s.RR_SRV.name);

	return(err);
	}

mDNSexport kern_return_t provide_DNSServiceRegistrationAddRecord_rpc(mach_port_t unusedserver, mach_port_t client,
	int type, const char *data, mach_msg_type_number_t data_len, natural_t *reference)
	{
	mStatus err;
	DNSServiceRegistration *x = DNSServiceRegistrationList;
	ExtraResourceRecord *extra;
	int size = sizeof(RDataBody);
	if (size < data_len)
		size = data_len;
	
	// Find this registered service
	while (x && x->ClientMachPort != client) x = x->next;
	if (!x) { debugf("provide_DNSServiceRegistrationAddRecord_rpc bad client %X", client); return(mStatus_BadReferenceErr); }

	// Allocate storage for our new record
	extra = malloc(sizeof(*extra) + size - sizeof(RDataBody));
	if (!extra) return(mStatus_NoMemoryErr);

	// Fill in type, length, and data
	extra->r.rrtype = type;
	extra->r.rdatastorage.MaxRDLength = size;
	extra->r.rdatastorage.RDLength    = data_len;
	memcpy(&extra->r.rdatastorage.u.data, data, data_len);
	
	// And register it
	err = mDNS_AddRecordToService(&mDNSStorage, &x->s, extra, &extra->r.rdatastorage);
	*reference = (natural_t)extra;
	debugf("Received a request to add the record of type: %d length: %d; returned reference %X", type, data_len, *reference);
	return(err);
	}

mDNSlocal void UpdateCallback(mDNS *const m, ResourceRecord *const rr, RData *OldRData)
	{
	if (OldRData != &rr->rdatastorage)
		free(OldRData);
	}

mDNSexport kern_return_t provide_DNSServiceRegistrationUpdateRecord_rpc(mach_port_t unusedserver, mach_port_t client,
	natural_t reference, int type, const char *data, mach_msg_type_number_t data_len)
	{
	mStatus err;
	DNSServiceRegistration *x = DNSServiceRegistrationList;
	ResourceRecord *rr;
	RData *newrdata;
	int size = sizeof(RDataBody);
	if (size < data_len)
		size = data_len;

	// Find this registered service
	while (x && x->ClientMachPort != client) x = x->next;
	if (!x) { debugf("provide_DNSServiceRegistrationUpdateRecord_rpc bad client %X", client); return(mStatus_BadReferenceErr); }
	
	// Find the record we're updating
	if (!reference)			// NULL reference means update the primary TXT record
		rr = &x->s.RR_TXT;
	else					// Else, scan our list to make sure we're updating a valid record that was previously added
		{
		ExtraResourceRecord *e = x->s.Extras;
		while (e && e != (ExtraResourceRecord*)reference) e = e->next;
		if (!e)
			{
			debugf("provide_DNSServiceRegistrationUpdateRecord_rpc failed to find record %X", reference);
			return(mStatus_BadReferenceErr);
			}
		rr = &e->r;
		}

	// Allocate storage for our new data
	newrdata = malloc(sizeof(*newrdata) + size - sizeof(RDataBody));
	if (!newrdata) return(mStatus_NoMemoryErr);

	// Fill in new length, and data
	newrdata->MaxRDLength = size;
	newrdata->RDLength    = data_len;
	memcpy(&newrdata->u, data, data_len);
	
	// And update our record
	err = mDNS_Update(&mDNSStorage, rr, newrdata, UpdateCallback);
	if (err)
		{
		debugf("Received a request to update the record of type: %d length: %d for reference: %X; failed %d",
			type, data_len, reference, err);
		return(err);
		}

	debugf("Received a request to update the record of type: %d length: %d for reference: %X", type, data_len, reference);
	return(err);
	}

mDNSexport kern_return_t provide_DNSServiceRegistrationRemoveRecord_rpc(mach_port_t unusedserver, mach_port_t client,
	natural_t reference)
	{
	mStatus err;
	DNSServiceRegistration *x = DNSServiceRegistrationList;
	ExtraResourceRecord *extra = (ExtraResourceRecord*)reference;
	
	// Find this registered service
	while (x && x->ClientMachPort != client) x = x->next;
	if (!x) { debugf("provide_DNSServiceRegistrationRemoveRecord_rpc bad client %X", client); return(mStatus_BadReferenceErr); }

	err = mDNS_RemoveRecordFromService(&mDNSStorage, &x->s, extra);
	if (!err) debugf("Received a request to remove the record of reference: %X", extra);
	else debugf("Received a request to remove the record of reference: %X (failed %d)", extra, err);
	free(extra);
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
	debugf("Handling signal from mach msg");
	if (!debug_mode)
		destroyBootstrapService();
	mDNS_Close(&mDNSStorage);
	exit(0);
	}

mDNSlocal kern_return_t start(const char *bundleName, const char *bundleDir)
	{
	mStatus            err;
	CFMachPortRef      d_port = CFMachPortCreate(NULL, ClientDeathCallback, NULL, NULL);
	CFMachPortRef      s_port = CFMachPortCreate(NULL, DNSserverCallback, NULL, NULL);
	CFMachPortRef      e_port = CFMachPortCreate(NULL, ExitCallback, NULL, NULL);
	mach_port_t        m_port = CFMachPortGetPort(s_port);
	kern_return_t      status = bootstrap_register(bootstrap_port, DNS_SERVICE_DISCOVERY_SERVER, m_port);
	CFRunLoopSourceRef d_rls  = CFMachPortCreateRunLoopSource(NULL, d_port, 0);
	CFRunLoopSourceRef s_rls  = CFMachPortCreateRunLoopSource(NULL, s_port, 0);
	CFRunLoopSourceRef e_rls  = CFMachPortCreateRunLoopSource(NULL, e_port, 0);
	
	if (status)
		{
		if (status == 1103)
			LogErrorMessage("Bootstrap_register failed(): A copy of the daemon is apparently already running");
		else
			LogErrorMessage("Bootstrap_register failed(): %s %d", mach_error_string(status), status);
		return(status);
		}
	
	err = mDNS_Init(&mDNSStorage, &PlatformStorage, rrcachestorage, RR_CACHE_SIZE, NULL, NULL);
	if (err) { LogErrorMessage("Daemon start: mDNS_Init failed %ld", err); return(err); }

	client_death_port = CFMachPortGetPort(d_port);
	exit_m_port = CFMachPortGetPort(e_port);

	CFRunLoopAddSource(CFRunLoopGetCurrent(), d_rls, kCFRunLoopDefaultMode);
	CFRunLoopAddSource(CFRunLoopGetCurrent(), s_rls, kCFRunLoopDefaultMode);
	CFRunLoopAddSource(CFRunLoopGetCurrent(), e_rls, kCFRunLoopDefaultMode);
	CFRelease(d_rls);
	CFRelease(s_rls);
	CFRelease(e_rls);
	if (debug_mode) printf("Service registered with Mach Port %d\n", m_port);

	return(err);
	}

mDNSlocal void HandleSIG(int signal)
	{
	debugf("");
	debugf("HandleSIG");
	
	// Send a mach_msg to ourselves (since that is signal safe) telling us to cleanup and exit
	mach_msg_return_t msg_result;
	mach_msg_header_t header;

	header.msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_MAKE_SEND, 0);
	header.msgh_remote_port = exit_m_port;
	header.msgh_local_port = MACH_PORT_NULL;
	header.msgh_size = sizeof(header);
	header.msgh_id = 0;

	msg_result = mach_msg_send(&header);
	}

mDNSexport int main(int argc, char **argv)
	{
	int i;
	kern_return_t status;
	FILE *fp;

	for (i=1; i<argc; i++)
		{
		extern int mDNS_UsePort53;
		if (!strcmp(argv[i], "-d")) debug_mode = 1;
		if (!strcmp(argv[i], "-no53")) mDNS_UsePort53 = 0;
		}

	signal(SIGINT, HandleSIG);	// SIGINT is what you get for a Ctrl-C
	signal(SIGTERM, HandleSIG);

	// Register the server with mach_init for automatic restart only during debug mode
    if (!debug_mode)
		registerBootstrapService();

	if (!debug_mode && !restarting_via_mach_init)
		exit(0); /* mach_init will restart us immediately as a daemon */

	fp = fopen(PID_FILE, "w");
	if (fp != NULL)
		{
		fprintf(fp, "%d\n", getpid());
		fclose(fp);
		}
	
	status = start(NULL, NULL);

	if (status == 0)
		{
		CFRunLoopRun();
		debugf("Exiting");
		destroyBootstrapService();
		mDNS_Close(&mDNSStorage);
		}

	return(status);
	}
