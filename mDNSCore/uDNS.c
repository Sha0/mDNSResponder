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

 * To Do:
 * Elimate all mDNSPlatformMemAllocate/mDNSPlatformMemFree from this code -- the core code
 * is supposed to be malloc-free so that it runs in constant memory determined at compile-time.
 * Any dynamic run-time requirements should be handled by the platform layer below or client layer above

    Change History (most recent first):

$Log: uDNS.c,v $
Revision 1.278  2007/01/16 03:04:16  cheshire
<rdar://problem/4917539> Add support for one-shot private queries as well as long-lived private queries
Don't cache result of ntaContextSRV(context) in a local variable --
the macro evaluates to a different result after we clear "context->isPrivate"

Revision 1.277  2007/01/10 22:51:58  cheshire
<rdar://problem/4917539> Add support for one-shot private queries as well as long-lived private queries

Revision 1.276  2007/01/10 02:09:30  cheshire
Better LogOperation record of keys read from System Keychain

Revision 1.275  2007/01/09 22:37:18  cheshire
Provide ten-second grace period for deleted keys, to give mDNSResponder
time to delete host name before it gives up access to the required key.

Revision 1.274  2007/01/09 01:16:32  cheshire
Improve "ERROR m->CurrentQuestion already set" debugging messages

Revision 1.273  2007/01/08 23:58:00  cheshire
Don't request regDomain and browseDomains in uDNS_SetupDNSConfig() -- it just ignores those results

Revision 1.272  2007/01/05 08:30:42  cheshire
Trim excessive "$Log" checkin history from before 2006
(checkin history still available via "cvs log ..." of course)

Revision 1.271  2007/01/05 06:34:03  cheshire
Improve "ERROR m->CurrentQuestion already set" debugging messages

Revision 1.270  2007/01/05 05:44:33  cheshire
Move automatic browse/registration management from uDNS.c to mDNSShared/uds_daemon.c,
so that mDNSPosix embedded clients will compile again

Revision 1.269  2007/01/04 23:11:13  cheshire
<rdar://problem/4720673> uDNS: Need to start caching unicast records
When an automatic browsing domain is removed, generate appropriate "remove" events for legacy queries

Revision 1.268  2007/01/04 22:06:38  cheshire
Fixed crash in LLQNatMapComplete()

Revision 1.267  2007/01/04 21:45:20  cheshire
Added mDNS_DropLockBeforeCallback/mDNS_ReclaimLockAfterCallback macros,
to do additional lock sanity checking around callback invocations

Revision 1.266  2007/01/04 21:01:20  cheshire
<rdar://problem/4607042> mDNSResponder NXDOMAIN and CNAME support
Only return NXDOMAIN results to clients that request them using kDNSServiceFlagsReturnIntermediates

Revision 1.265  2007/01/04 20:47:17  cheshire
Fixed crash in CheckForUnreferencedLLQMapping()

Revision 1.264  2007/01/04 20:39:27  cheshire
Fix locking mismatch

Revision 1.263  2007/01/04 02:39:53  cheshire
<rdar://problem/4030599> Hostname passed into DNSServiceRegister ignored for Wide-Area service registrations

Revision 1.262  2007/01/04 00:29:25  cheshire
Covert LogMsg() in GetAuthInfoForName to LogOperation()

Revision 1.261  2006/12/22 20:59:49  cheshire
<rdar://problem/4742742> Read *all* DNS keys from keychain,
 not just key for the system-wide default registration domain

Revision 1.260  2006/12/21 00:06:07  cheshire
Don't need to do mDNSPlatformMemZero() -- mDNS_SetupResourceRecord() does it for us

Revision 1.259  2006/12/20 04:07:36  cheshire
Remove uDNS_info substructure from AuthRecord_struct

Revision 1.258  2006/12/19 22:49:24  cheshire
Remove uDNS_info substructure from ServiceRecordSet_struct

Revision 1.257  2006/12/19 02:38:20  cheshire
Get rid of unnecessary duplicate query ID field from DNSQuestion_struct

Revision 1.256  2006/12/19 02:18:48  cheshire
Get rid of unnecessary duplicate "void *context" field from DNSQuestion_struct

Revision 1.255  2006/12/16 01:58:31  cheshire
<rdar://problem/4720673> uDNS: Need to start caching unicast records

Revision 1.254  2006/12/15 19:23:39  cheshire
Use new DomainNameLengthLimit() function, to be more defensive against malformed
data received from the network.

Revision 1.253  2006/12/01 07:43:34  herscher
Fix byte ordering problem for one-shot TCP queries.
Iterate more intelligently over duplicates in uDNS_ReceiveMsg to avoid spin loops.

Revision 1.252  2006/11/30 23:07:57  herscher
<rdar://problem/4765644> uDNS: Sync up with Lighthouse changes for Private DNS

Revision 1.251  2006/11/28 21:42:11  mkrochma
Work around a crashing bug that was introduced by uDNS and mDNS code unification

Revision 1.250  2006/11/18 05:01:30  cheshire
Preliminary support for unifying the uDNS and mDNS code,
including caching of uDNS answers

Revision 1.249  2006/11/10 07:44:04  herscher
<rdar://problem/4825493> Fix Daemon locking failures while toggling BTMM

Revision 1.248  2006/11/08 04:26:53  cheshire
Fix typo in debugging message

Revision 1.247  2006/10/20 05:35:04  herscher
<rdar://problem/4720713> uDNS: Merge unicast active question list with multicast list.

Revision 1.246  2006/10/11 19:29:41  herscher
<rdar://problem/4744553> uDNS: mDNSResponder-111 using 100% CPU

Revision 1.245  2006/10/04 22:21:15  herscher
Tidy up references to mDNS_struct introduced when the embedded uDNS_info struct was removed.

Revision 1.244  2006/10/04 21:51:27  herscher
Replace calls to mDNSPlatformTimeNow(m) with m->timenow

Revision 1.243  2006/10/04 21:38:59  herscher
Remove uDNS_info substructure from DNSQuestion_struct

Revision 1.242  2006/09/27 00:51:46  herscher
Fix compile error when _LEGACY_NAT_TRAVERSAL_ is not defined

Revision 1.241  2006/09/26 01:54:47  herscher
<rdar://problem/4245016> NAT Port Mapping API (for both NAT-PMP and UPnP Gateway Protocol)

Revision 1.240  2006/09/15 21:20:15  cheshire
Remove uDNS_info substructure from mDNS_struct

Revision 1.239  2006/08/16 02:52:56  mkrochma
<rdar://problem/4104154> Actually fix it this time

Revision 1.238  2006/08/16 00:31:50  mkrochma
<rdar://problem/4386944> Get rid of NotAnInteger references

Revision 1.237  2006/08/15 23:38:17  mkrochma
<rdar://problem/4104154> Requested Public Port field should be set to zero on mapping deletion

Revision 1.236  2006/08/14 23:24:23  cheshire
Re-licensed mDNSResponder daemon source code under Apache License, Version 2.0

Revision 1.235  2006/07/30 05:45:36  cheshire
<rdar://problem/4304215> Eliminate MIN_UCAST_PERIODIC_EXEC

Revision 1.234  2006/07/22 02:58:36  cheshire
Code was clearing namehash twice instead of namehash and rdatahash

Revision 1.233  2006/07/20 19:46:51  mkrochma
<rdar://problem/4472014> Add Private DNS client functionality to mDNSResponder
Fix Private DNS

Revision 1.232  2006/07/15 02:01:29  cheshire
<rdar://problem/4472014> Add Private DNS client functionality to mDNSResponder
Fix broken "empty string" browsing

Revision 1.231  2006/07/05 23:28:22  cheshire
<rdar://problem/4472014> Add Private DNS client functionality to mDNSResponder

Revision 1.230  2006/06/29 03:02:44  cheshire
<rdar://problem/4607042> mDNSResponder NXDOMAIN and CNAME support

Revision 1.229  2006/03/02 22:03:41  cheshire
<rdar://problem/4395331> Spurious warning "GetLargeResourceRecord: m->rec appears to be already in use"
Refinement: m->rec.r.resrec.RecordType needs to be cleared *every* time around for loop, not just once at the end

Revision 1.228  2006/02/26 00:54:42  cheshire
Fixes to avoid code generation warning/error on FreeBSD 7

Revision 1.227  2006/01/09 20:47:05  cheshire
<rdar://problem/4395331> Spurious warning "GetLargeResourceRecord: m->rec appears to be already in use"

*/

#include "uDNS.h"
#include "dns_sd.h"

#if(defined(_MSC_VER))
	// Disable "assignment within conditional expression".
	// Other compilers understand the convention that if you place the assignment expression within an extra pair
	// of parentheses, this signals to the compiler that you really intended an assignment and no warning is necessary.
	// The Microsoft compiler doesn't understand this convention, so in the absense of any other way to signal
	// to the compiler that the assignment is intentional, we have to just turn this warning off completely.
	#pragma warning(disable:4706)
#endif

#define TCP_SOCKET_FLAGS   kTCPSocketFlags_UseTLS

typedef struct
	{
	mDNS				*	m;
	DNSMessage				request;
	int						requestLen;
	DomainAuthInfo		*	authInfo;
	DNSQuestion			*	question;	// For queries
	ServiceRecordSet	*	srs;		// For service record updates
	AuthRecord			*	rr;			// For record updates
	LLQ_Info			*	llqInfo;
	DNSMessage			*	reply;
	mDNSu8					lenbuf[2];
	mDNSu16					replylen;
	unsigned long			nread;
	int						numReplies;
	} tcpInfo_t;

typedef struct SearchListElem
	{
    struct SearchListElem *next;
    domainname domain;
    int flag;
    DNSQuestion BrowseQ;
    DNSQuestion DefBrowseQ;
    DNSQuestion LegacyBrowseQ;
    DNSQuestion RegisterQ;
    DNSQuestion DefRegisterQ;
    ARListElem *AuthRecs;
	} SearchListElem;

// for domain enumeration and default browsing/registration

static SearchListElem *  SearchList           = mDNSNULL;  // where we search for _browse domains

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - General Utility Functions
#endif

// CountLabels() returns number of labels in name, excluding final root label
// (e.g. for "apple.com." CountLabels returns 2.)
mDNSlocal int CountLabels(const domainname *d)
	{
	int count = 0;
	const mDNSu8 *ptr;

	for (ptr = d->c; *ptr; ptr = ptr + ptr[0] + 1) count++;
	return count;
	}

// unlink an AuthRecord from a linked list
mDNSlocal mStatus unlinkAR(AuthRecord **list, AuthRecord *const rr)
	{
	while (*list && *list != rr) list = &(*list)->next;
	if (*list) { *list = rr->next; rr->next = mDNSNULL; return(mStatus_NoError); }
	LogMsg("ERROR: unlinkAR - no such active record %##s", rr->resrec.name->c);
	return(mStatus_NoSuchRecord);
	}

mDNSlocal void unlinkSRS(mDNS *m, ServiceRecordSet *srs)
	{
	ServiceRecordSet **p;
	NATTraversalInfo *n = m->NATTraversals;

	// verify that no NAT objects reference this service
	while (n)
		{
		if (n->reg.ServiceRegistration == srs)
			{
			NATTraversalInfo *tmp = n;
			n = n->next;
			LogMsg("ERROR: Unlinking service record set %##s still referenced by NAT traversal object!", srs->RR_SRV.resrec.name->c);
			uDNS_FreeNATInfo(m, tmp);
			}
		else n = n->next;
		}

	for (p = &m->ServiceRegistrations; *p; p = &(*p)->next)
		if (*p == srs)
			{
			ExtraResourceRecord *e;
			*p = srs->next;
			srs->next = mDNSNULL;
			for (e=srs->Extras; e; e=e->next)
				if (unlinkAR(&m->RecordRegistrations, &e->r))
					LogMsg("unlinkSRS: extra record %##s not found", e->r.resrec.name->c);
			return;
			}
	LogMsg("ERROR: unlinkSRS - SRS not found in ServiceRegistrations list %##s", srs->RR_SRV.resrec.name->c);
	}

// set retry timestamp for record with exponential backoff
// (for service record sets, use RR_SRV as representative for time checks
mDNSlocal void SetRecordRetry(mDNS *const m, AuthRecord *rr, mStatus SendErr)
	{
	rr->LastAPTime = m->timenow;
	if (SendErr == mStatus_TransientErr || rr->ThisAPInterval < INIT_UCAST_POLL_INTERVAL)  rr->ThisAPInterval = INIT_UCAST_POLL_INTERVAL;
	else if (rr->ThisAPInterval*2 <= MAX_UCAST_POLL_INTERVAL)                              rr->ThisAPInterval *= 2;
	else                                                                                   rr->ThisAPInterval = MAX_UCAST_POLL_INTERVAL;
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Name Server List Management
#endif

mDNSexport void mDNS_AddDNSServer(mDNS *const m, const mDNSAddr *addr, const domainname *d)
    {
	DNSServer *s, **p = &m->Servers;

	mDNS_Lock(m);
	if (!d) d = (const domainname *)"";

	while (*p)                 // Check if we already have this {server,domain} pair registered
		{
		if (mDNSSameAddress(&(*p)->addr, addr) && SameDomainName(&(*p)->domain, d))
			LogMsg("Note: DNS Server %#a for domain %##s registered more than once", addr, d->c);
		p=&(*p)->next;
		}

	// allocate, add to list
	s = mDNSPlatformMemAllocate(sizeof(*s));
	if (!s) { LogMsg("Error: mDNS_AddDNSServer - malloc"); goto end; }
	s->addr      = *addr;
	s->del       = mDNSfalse;
	s->teststate = DNSServer_Untested;
	AssignDomainName(&s->domain, d);
	s->next = mDNSNULL;
	*p = s;

	end:
	mDNS_Unlock(m);
    }

mDNSexport void mDNS_DeleteDNSServers(mDNS *const m)
    {
	DNSServer *s;
	mDNS_Lock(m);

	s = m->Servers;
	m->Servers = mDNSNULL;
	while (s)
		{
		DNSServer *tmp = s;
		s = s->next;
		mDNSPlatformMemFree(tmp);
		}

	mDNS_Unlock(m);
    }

 // ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - authorization management
#endif

mDNSexport DomainAuthInfo *GetAuthInfoForName(mDNS *m, const domainname *const name)
	{
	const domainname *n = name;
	DomainAuthInfo **p = &m->AuthInfoList;
	DomainAuthInfo *ptr;

	// First purge any dead keys from the list
	while (*p)
		{
		if ((*p)->deltime && m->timenow - (*p)->deltime >= 0)
			{
			ptr = *p;
			LogOperation("Deleting expired key %##s %##s", ptr->domain.c, ptr->keyname.c);
			*p = ptr->next;
			// Probably not essential, but just to be safe, zero out the secret key data
			// so we don't leave it hanging around in memory
			// (where it could potentially get exposed via some other bug)
			mDNSPlatformMemZero(ptr, sizeof(*ptr));
			mDNSPlatformMemFree(ptr);
			}
		else
			p = &(*p)->next;
		}
	
	while (n->c[0])
		{
		for (ptr = m->AuthInfoList; ptr; ptr = ptr->next)
			if (SameDomainName(&ptr->domain, n))
				{
				LogOperation("GetAuthInfoForName %##s %##s %##s", name->c, ptr->domain.c, ptr->keyname.c);
				return(ptr);
				}
		n = (const domainname *)(n->c + 1 + n->c[0]);
		}
	LogOperation("GetAuthInfoForName none found for %##s", name->c);
	return mDNSNULL;
	}

mDNSexport mStatus mDNS_SetSecretForDomain(mDNS *m, DomainAuthInfo *info,
	const domainname *domain, const domainname *keyname, const char *b64keydata)
	{
	DomainAuthInfo **p = &m->AuthInfoList;
	if (!info || !b64keydata) return(mStatus_BadParamErr);

	info->next    = mDNSNULL;
	info->deltime = 0;
	AssignDomainName(&info->domain,  domain);
	AssignDomainName(&info->keyname, keyname);

	if (DNSDigest_ConstructHMACKeyfromBase64(info, b64keydata) < 0)
		{
		LogMsg("ERROR: mDNS_SetSecretForDomain: could not convert shared secret %s from base64", b64keydata);
		return(mStatus_BadParamErr);
		}

	while (*p && (*p) != info) p=&(*p)->next;
	if (!*p) *p = info;
	else LogMsg("Error! Tried to mDNS_SetSecretForDomain: %##s %##s with DomainAuthInfo %p that's already in the list",
		domain->c, keyname->c, info);

	return(mStatus_NoError);
	}

 // ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - NAT Traversal
#endif

mDNSlocal mDNSBool DomainContainsLabelString(const domainname *d, const char *str)
	{
	const domainlabel *l;
	domainlabel buf;

	if (!MakeDomainLabelFromLiteralString(&buf, str)) return mDNSfalse;

	for (l = (const domainlabel *)d; l->c[0]; l = (const domainlabel *)(l->c + l->c[0]+1))
		if (SameDomainLabel(l->c, buf.c)) return mDNStrue;
	return mDNSfalse;
	}

// allocate struct, link into global list, initialize
mDNSexport NATTraversalInfo *uDNS_AllocNATInfo(mDNS *const m, NATOp_t op, mDNSIPPort privatePort, mDNSIPPort publicPort, mDNSu32 ttl, NATResponseHndlr callback)
	{
	NATTraversalInfo *info = m->NATTraversals;

	for (info = m->NATTraversals; info; info = info->next)
		{
		if (info->op == op && info->PrivatePort.NotAnInteger && (info->PrivatePort.NotAnInteger == privatePort.NotAnInteger))
			{
			break;
			}
		}

	if (info)
		{
		info->refs++;
		}
	else
		{
		info = mDNSPlatformMemAllocate(sizeof(NATTraversalInfo));
		if (!info) { LogMsg("ERROR: malloc"); goto exit; }
		mDNSPlatformMemZero(info, sizeof(NATTraversalInfo));
		info->next = m->NATTraversals;
		m->NATTraversals = info;
		info->retry = m->timenow + NATMAP_INIT_RETRY;
		info->RetryInterval = NATMAP_INIT_RETRY;
		info->op = op;
		info->state = NATState_Init;
		info->ReceiveResponse = callback;
		info->PrivatePort = privatePort;
		info->PublicPort = publicPort;
		info->Router = m->Router;
		info->TTL = ttl;
		info->refs = 1;
		}
exit:

	return info;
	}

mDNSlocal void FormatPortMaprequest(NATTraversalInfo *info)
	{
	NATPortMapRequest *req = &info->request.PortReq;

	req->vers = NATMAP_VERS;
	req->opcode = info->op;
	req->unused = zeroID;
	req->priv = info->PrivatePort;
	req->pub = info->PrivatePort;
	req->lease = mDNSOpaque32fromIntVal(NATMAP_DEFAULT_LEASE);
	}

mDNSlocal void SendInitialPMapReq(mDNS *m, NATTraversalInfo *info)
	{
	if (!m->Router.ip.v4.NotAnInteger)
		{
		debugf("No router.  Will retry NAT traversal in %ld seconds", NATMAP_INIT_RETRY);
		info->retry = m->timenow + NATMAP_INIT_RETRY;
		info->RetryInterval = NATMAP_INIT_RETRY;
		return;
		}
    uDNS_SendNATMsg(info, m);
	return;
	}

mDNSlocal NATTraversalInfo * AllocLLQNatMap(mDNS *const m, NATOp_t op, mDNSIPPort port, NATResponseHndlr callback)
	{
	NATTraversalInfo * info;

	info = uDNS_AllocNATInfo(m, op, port, zeroIPPort, 0, callback);

	if (!info)
	{
		LogMsg("AllocLLQNatMap: memory exhausted");
		goto exit;
	}

	if (info->state == NATState_Init)
	{
		info->reg.RecordRegistration  = mDNSNULL;
		info->reg.ServiceRegistration = mDNSNULL;
		info->state = NATState_Request;
		info->isLLQ = mDNStrue;

		//FormatPortMaprequest(info, m->UnicastPort4);
		FormatPortMaprequest(info);
		SendInitialPMapReq(m, info);
	}

exit:

	return info;
	}

// unlink from list, deallocate
mDNSexport mDNSBool uDNS_FreeNATInfo(mDNS *m, NATTraversalInfo *n)
	{
	NATTraversalInfo *ptr, *prev = mDNSNULL;
	ServiceRecordSet *s = m->ServiceRegistrations;

	// Verify that object is not referenced by any services
	while (s)
		{
		if (s->NATinfo == n)
			{
			LogMsg("Error: Freeing NAT info object still referenced by Service Record Set %##s!", s->RR_SRV.resrec.name->c);
			s->NATinfo = mDNSNULL;
			}
		s = s->next;
		}

	ptr = m->NATTraversals;
	while (ptr)
		{
		if (ptr == n)
			{
			if (prev) prev->next = ptr->next;
			else m->NATTraversals = ptr->next;
			mDNSPlatformMemFree(n);
			return mDNStrue;
			}
		prev = ptr;
		ptr = ptr->next;
		}
	LogMsg("uDNS_FreeNATInfo: NATTraversalInfo not found in list");
	return mDNSfalse;
	}

mDNSexport void uDNS_SendNATMsg(NATTraversalInfo *info, mDNS *m)
	{
	mStatus err;

	if (info->state != NATState_Request && info->state != NATState_Refresh)
		{ LogMsg("uDNS_SendNATMsg: Bad state %d", info->state); return; }

	if (m->Router.ip.v4.NotAnInteger)
		{
		// send msg if we have a router
		const mDNSu8 *end = (mDNSu8 *)&info->request;
		if (info->op == NATOp_AddrRequest) end += sizeof(NATAddrRequest);
		else end += sizeof(NATPortMapRequest);

		err = mDNSPlatformSendUDP(m, &info->request, end, 0, &m->Router, NATPMPPort);
		if (!err) (info->ntries++);  // don't increment attempt counter if the send failed
		}

	// set retry
	if (info->RetryInterval < NATMAP_INIT_RETRY) info->RetryInterval = NATMAP_INIT_RETRY;
	else if (info->RetryInterval * 2 > NATMAP_MAX_RETRY) info->RetryInterval = NATMAP_MAX_RETRY;
	else info->RetryInterval *= 2;
	info->retry = m->timenow + info->RetryInterval;
	}

mDNSexport mDNSBool uDNS_HandleNATQueryAddrReply(NATTraversalInfo *n, mDNS * const m, mDNSu8 *pkt, mDNSu16 len, mDNSAddr *addr, mStatus *err)
	{
	NATAddrReply *response = (NATAddrReply *)pkt;

	(void) m;

	*err = mStatus_NoError;

	if (n->state != NATState_Request)
		{
		LogMsg("uDNS_HandleNATQueryAddrReply: bad state %d", n->state);
		return mDNSfalse;
		}

	if (!pkt) // timeout
		{
#ifdef _LEGACY_NAT_TRAVERSAL_
		*err = LNT_GetPublicIP(&addr->ip.v4);
		if (*err) goto end;
		else n->state = NATState_Legacy;
#else
		debugf("uDNS_HandleNATQueryAddrReply: timeout");
		*err = mStatus_NATTraversal;
		goto end;
#endif // _LEGACY_NAT_TRAVERSAL_
		}
	else
		{
		if (len < sizeof(*response))
			{
			LogMsg("uDNS_HandleNATQueryAddrReply: response too short (%d bytes)", len);
			return mDNSfalse;
			}
		if (response->vers != NATMAP_VERS)
			{
			LogMsg("uDNS_HandleNATQueryAddrReply: received version %d (expect version %d)", pkt[0], NATMAP_VERS);
			return mDNSfalse;
			}
		if (response->opcode != (NATOp_AddrRequest | NATMAP_RESPONSE_MASK))
			{
			LogMsg("uDNS_HandleNATQueryAddrReply: bad response code %d", response->opcode);
			return mDNSfalse;
			}
		if (response->err.NotAnInteger)
			{
			LogMsg("uDNS_HandleNATQueryAddrReply: received error %d", mDNSVal16(response->err));
			*err = mStatus_NATTraversal;
			goto end;
			}

		addr->ip.v4 = response->PubAddr;
		n->state = NATState_Established;
		}

	if (IsPrivateV4Addr(addr))
		{
		LogMsg("uDNS_HandleNATQueryAddrReply: Double NAT");
		*err = mStatus_DoubleNAT;
		goto end;
		}

end:

	return mDNStrue;
	}

mDNSlocal mDNSBool ReceiveNATAddrResponse(NATTraversalInfo *n, mDNS *m, mDNSu8 *pkt, mDNSu16 len)
	{
	mStatus err = mStatus_NoError;
	AuthRecord *rr = mDNSNULL;
	mDNSAddr addr;
	mDNSBool ret;

	rr = n->reg.RecordRegistration;
	if (!rr)
		{
		LogMsg("RegisterHostnameRecord: registration cancelled");
		return mDNSfalse;
		}

	addr.type = mDNSAddrType_IPv4;
	addr.ip.v4 = rr->resrec.rdata->u.ipv4;

	ret = uDNS_HandleNATQueryAddrReply(n, m, pkt, len, &addr, &err);

	if (!ret)
		{
		return ret;
		}

	if (err)
		{
		goto end;
		}

	LogOperation("Received public IP address %d.%d.%d.%d from NAT.", addr.ip.v4.b[0], addr.ip.v4.b[1], addr.ip.v4.b[2], addr.ip.v4.b[3]);

	rr->resrec.rdata->u.ipv4 = addr.ip.v4;  // replace rdata w/ public address
	mDNS_Register_internal(m, rr);

end:

	if (err)
		{
		uDNS_FreeNATInfo(m, n);
		if (rr)
			{
			rr->NATinfo = mDNSNULL;
			rr->state = regState_Unregistered;    // note that rr is not yet in global list
			rr->RecordCallback(m, rr, mStatus_NATTraversal);
			// note - unsafe to touch rr after callback
			}
		}
	return mDNStrue;
	}

mDNSlocal void StartGetPublicAddr(mDNS *m, AuthRecord *AddressRec)
	{
	NATAddrRequest *req;

	NATTraversalInfo *info = uDNS_AllocNATInfo(m, NATOp_AddrRequest, zeroIPPort, zeroIPPort, 0, ReceiveNATAddrResponse);
	if (!info) { mDNS_Register_internal(m, AddressRec); return; }
	AddressRec->NATinfo = info;
	info->reg.RecordRegistration = AddressRec;
	info->state = NATState_Request;

    // format message
	req = &info->request.AddrReq;
	req->vers = NATMAP_VERS;
	req->opcode = NATOp_AddrRequest;

	if (!m->Router.ip.v4.NotAnInteger)
		{
		debugf("No router.  Will retry NAT traversal in %ld ticks", NATMAP_INIT_RETRY);
		return;
		}

	uDNS_SendNATMsg(info, m);
	}

mDNSlocal void RefreshNATMapping(NATTraversalInfo *n, mDNS *m)
	{
	n->state = NATState_Refresh;
	n->RetryInterval = NATMAP_INIT_RETRY;
	n->ntries = 0;
	uDNS_SendNATMsg(n, m);
	}

extern void pktResponseHndlr(mDNS * const m, DNSMessage *msg, const mDNSu8 *end, DNSQuestion *question);
mDNSexport void pktResponseHndlr(mDNS * const m, DNSMessage *msg, const mDNSu8 *end, DNSQuestion *question)
	{
	const mDNSu8 *ptr;
	int i;
	CacheRecord *cr = &m->rec.r;
	mDNSBool goodbye;
	int followedCNames = 0;
	static const int maxCNames = 5;
	LLQ_Info *llqInfo = question->llq;
	domainname origname;
	origname.c[0] = 0;

	if (question != m->CurrentQuestion)
		{ LogMsg("ERROR: pktResponseHdnlr called without CurrentQuestion ptr set!");  return; }

	if (question->Answered == 0 && msg->h.numAnswers == 0 && question->ReturnIntermed)
		{
		/* NXDOMAIN error or empty RR set - notify client */
		question->Answered = mDNStrue;

		/* Create empty resource record */
		cr->resrec.RecordType = kDNSRecordTypeUnregistered;
		cr->resrec.InterfaceID = mDNSInterface_Any;
		cr->resrec.name = &question->qname;
		cr->resrec.rrtype = question->qtype;
		cr->resrec.rrclass = question->qclass;
		cr->resrec.rroriginalttl = 1; /* What should we use for the TTL? TTL from SOA for domain? */
		cr->resrec.rdlength      = 0;
		cr->resrec.rdestimate    = 0;
		cr->resrec.namehash      = 0;
		cr->resrec.rdatahash     = 0;
		cr->resrec.rdata = (RData*)&cr->rdatastorage;
		cr->resrec.rdata->MaxRDLength = cr->resrec.rdlength;

		/* Pass empty answer to callback */
		mDNS_DropLockBeforeCallback();
		question->QuestionCallback(m, question, &cr->resrec, 0);
		mDNS_ReclaimLockAfterCallback();
		// CAUTION: Need to be careful after calling question->QuestionCallback(),
		// because the client's callback function is allowed to do anything,
		// including starting/stopping queries, registering/deregistering records, etc.
		m->rec.r.resrec.RecordType = 0; // Clear RecordType to show we're not still using it
		if (question != m->CurrentQuestion)
			{
			debugf("pktResponseHndlr - CurrentQuestion changed by QuestionCallback - returning.");
			return;
			}
		}

	question->Answered = mDNStrue;

	ptr = LocateAnswers(msg, end);
	if (!ptr) goto pkt_error;

	for (i = 0; i < msg->h.numAnswers; i++)
		{
		ptr = GetLargeResourceRecord(m, msg, ptr, end, 0, kDNSRecordTypePacketAns, &m->rec);
		if (!ptr) goto pkt_error;
		goodbye = question->LongLived ? ((mDNSs32)cr->resrec.rroriginalttl == -1) : mDNSfalse;
		if (ResourceRecordAnswersQuestion(&cr->resrec, question))
			{
			if (cr->resrec.rrtype == kDNSType_CNAME)
				{
				if (followedCNames > (maxCNames - 1)) LogMsg("Error: too many CNAME referals for question %##s", &origname);
				else
					{
					debugf("Following cname %##s -> %##s", question->qname.c, cr->resrec.rdata->u.name.c);
					if (question->ReturnIntermed)
						{
						mDNS_DropLockBeforeCallback();
						question->QuestionCallback(m, question, &cr->resrec, !goodbye);
						mDNS_ReclaimLockAfterCallback();
						// CAUTION: Need to be careful after calling question->QuestionCallback(),
						// because the client's callback function is allowed to do anything,
						// including starting/stopping queries, registering/deregistering records, etc.
						if (question != m->CurrentQuestion)
							{
							m->rec.r.resrec.RecordType = 0; // Clear RecordType to show we're not still using it
							debugf("pktResponseHndlr - CurrentQuestion changed by QuestionCallback - returning.");
							return;
							}
						}
					AssignDomainName(&origname, &question->qname);
					AssignDomainName(&question->qname, &cr->resrec.rdata->u.name);
					question->qnamehash = DomainNameHashValue(&question->qname);
					followedCNames++;
					i = -1; // restart packet answer matching
					ptr = LocateAnswers(msg, end);
					m->rec.r.resrec.RecordType = 0; // Clear RecordType to show we're not still using it
					continue;
					}
				}
			}

		m->rec.r.resrec.RecordType = 0; // Clear RecordType to show we're not still using it
		}

	// Our interval may be set lower to recover from failures -- now that we have an answer, fully back off retry.
	// If the server advertised an LLQ-specific port number then that implies that this zone
	// *wants* to support LLQs, so if the setup fails (e.g. because we are behind a NAT)
	// then we use a slightly faster polling rate to give slightly better user experience.
	if (question->LongLived && llqInfo->state == LLQ_Poll && llqInfo->servPort.NotAnInteger) question->ThisQInterval = LLQ_POLL_INTERVAL;
	else if (question->ThisQInterval < MAX_UCAST_POLL_INTERVAL) question->ThisQInterval = MAX_UCAST_POLL_INTERVAL;
	return;

	pkt_error:
	LogMsg("ERROR: pktResponseHndlr - received malformed response to query for %##s (%s)",
		   question->qname.c, DNSTypeName(question->qtype));
	return;
	}

mDNSlocal void startLLQPolling(mDNS * const m, LLQ_Info * info)
	{
	LogOperation("startLLQPolling: %##s", info->question->qname.c);
	info->question->responseCallback = pktResponseHndlr;
	info->state = LLQ_Poll;
	info->question->LastQTime = m->timenow;  // trigger immediate poll
	info->question->ThisQInterval = INIT_UCAST_POLL_INTERVAL;
	}

mDNSlocal void StartLLQNatMap(mDNS *m, LLQ_Info * llq)
	{
	if (llq->state == LLQ_NatMapWaitTCP)
		{
		llq->NATInfoTCP = AllocLLQNatMap(m, NATOp_MapTCP, llq->eventPort, uDNS_HandleNATPortMapReply);

		if (!llq->NATInfoTCP)
			{
			LogMsg("StartLLQNatMap: memory exhausted");
			goto exit;
			}
		}
	else
		{
		if (llq->isPrivate)
			{
			llq->NATInfoUDP = AllocLLQNatMap(m, NATOp_MapUDP, llq->eventPort, uDNS_HandleNATPortMapReply);
			}
		else
			{
			llq->NATInfoUDP = AllocLLQNatMap(m, NATOp_MapUDP, m->UnicastPort4, uDNS_HandleNATPortMapReply);
			}

		if (!llq->NATInfoUDP)
			{
			LogMsg("StartLLQNatMap: memory exhausted");
			goto exit;
			}
		}

exit:

	return;
	}

mDNSlocal void initializeQuery(DNSMessage *msg, DNSQuestion *question)
	{
	mDNSPlatformMemZero(msg, sizeof(*msg));
    InitializeDNSMessage(&msg->h, question->TargetQID, uQueryFlags);
	}

mDNSlocal mDNSu8 *putLLQ(DNSMessage *const msg, mDNSu8 *ptr, DNSQuestion *question, LLQOptData *data, mDNSBool includeQuestion)
	{
	AuthRecord rr;
	ResourceRecord *opt = &rr.resrec;
	rdataOpt *optRD;

	//!!!KRS when we implement multiple llqs per message, we'll need to memmove anything past the question section
	if (includeQuestion)
		{
		ptr = putQuestion(msg, ptr, msg->data + AbsoluteMaxDNSMessageData, &question->qname, question->qtype, question->qclass);
		if (!ptr) { LogMsg("ERROR: putLLQ - putQuestion"); return mDNSNULL; }
		}
	// locate OptRR if it exists, set pointer to end
	// !!!KRS implement me

	// format opt rr (fields not specified are zero-valued)
	mDNS_SetupResourceRecord(&rr, mDNSNULL, mDNSInterface_Any, kDNSType_OPT, kStandardTTL, kDNSRecordTypeKnownUnique, mDNSNULL, mDNSNULL);
	opt->rdlength = LLQ_OPT_RDLEN;
	opt->rdestimate = LLQ_OPT_RDLEN;

	optRD = &rr.resrec.rdata->u.opt;
	optRD->opt = kDNSOpt_LLQ;
	optRD->optlen = LLQ_OPTLEN;
	optRD->OptData.llq = *data;
	ptr = PutResourceRecordTTLJumbo(msg, ptr, &msg->h.numAdditionals, opt, 0);
	if (!ptr) { LogMsg("ERROR: putLLQ - PutResourceRecordTTLJumbo"); return mDNSNULL; }

	return ptr;
	}

mDNSlocal mStatus constructQueryMsg(DNSMessage *msg, mDNSu8 **endPtr, DNSQuestion *const question)
	{
	initializeQuery(msg, question);

	*endPtr = putQuestion(msg, msg->data, msg->data + AbsoluteMaxDNSMessageData, &question->qname, question->qtype, question->qclass);
    if (!*endPtr)
        {
        LogMsg("ERROR: Unicast query out of space in packet");
        return mStatus_UnknownErr;
        }
	return mStatus_NoError;
	}

mDNSlocal mDNSBool getLLQAtIndex(mDNS *m, DNSMessage *msg, const mDNSu8 *end, LLQOptData *llq, int index)
	{
	LargeCacheRecord lcr;
	int i;
	const mDNSu8 *ptr;

	mDNSPlatformMemZero(&lcr, sizeof(lcr));

	ptr = LocateAdditionals(msg, end);
	if (!ptr) return mDNSfalse;

	// find the last additional
	for (i = 0; i < msg->h.numAdditionals; i++)
//		{ ptr = GetLargeResourceRecord(m, msg, ptr, end, 0, kDNSRecordTypePacketAdd, &lcr); if (!ptr) return mDNSfalse; }
//!!!KRS workaround for LH server bug, which puts OPT as first additional
		{
		ptr = GetLargeResourceRecord(m, msg, ptr, end, 0, kDNSRecordTypePacketAdd, &lcr);
		if (!ptr) return mDNSfalse;
		if (lcr.r.resrec.rrtype == kDNSType_OPT) break;
		}
	if (lcr.r.resrec.rrtype != kDNSType_OPT) return mDNSfalse;
	if (lcr.r.resrec.rdlength < (index + 1) * LLQ_OPT_RDLEN) return mDNSfalse;  // rdata too small
	mDNSPlatformMemCopy((mDNSu8 *)&lcr.r.resrec.rdata->u.opt.OptData.llq + (index * sizeof(*llq)), llq, sizeof(*llq));
	return mDNStrue;
	}

mDNSlocal mDNSBool recvLLQEvent(mDNS *m, DNSQuestion *q, DNSMessage *msg, const mDNSu8 *end, const mDNSAddr *srcaddr, mDNSIPPort srcport)
	{
	DNSMessage ack;
	mDNSu8 *ackEnd = ack.data;
	mStatus err;
	LLQOptData opt;

    // find Opt RR, verify correct ID
	if (!getLLQAtIndex(m, msg, end, &opt, 0))  { debugf("Pkt does not contain LLQ Opt");                         return mDNSfalse; }
	if (!q->llq) { LogMsg("Error: recvLLQEvent - question object does not contain LLQ metadata");                return mDNSfalse; }
	if (!mDNSSameOpaque64(&opt.id, &q->llq->id))
		{
		debugf("recvLLQEvent opt.id %08X %08X != q->llq->id %08X %08X", opt.id.l[0], opt.id.l[1], q->llq->id.l[0], q->llq->id.l[1]);
		return mDNSfalse;
		}
	if (opt.llqOp != kLLQOp_Event) { if (!q->llq->ntries) LogMsg("recvLLQEvent - Bad LLQ Opcode %d", opt.llqOp); return mDNSfalse; }

    // invoke response handler
	if (m->CurrentQuestion != q)
		{
		LogMsg("recvLLQEvent: ERROR m->CurrentQuestion not set: q %##s (%s)", q->qname.c, DNSTypeName(q->qtype));
		if (m->CurrentQuestion)
			LogMsg("recvLLQEvent: ERROR m->CurrentQuestion not set: CurrentQuestion %##s (%s)", m->CurrentQuestion->qname.c, DNSTypeName(m->CurrentQuestion->qtype));
		else
			LogMsg("recvLLQEvent: ERROR m->CurrentQuestion not set: CurrentQuestion NULL");
		}

	q->responseCallback(m, msg, end, q);
	if (m->CurrentQuestion != q) { m->CurrentQuestion = mDNSNULL; return mDNStrue; }
	m->CurrentQuestion = mDNSNULL;

    //  format and send ack
	InitializeDNSMessage(&ack.h, msg->h.id, ResponseFlags);
	ackEnd = putLLQ(&ack, ack.data, mDNSNULL, &opt, mDNSfalse);
	if (!ackEnd) { LogMsg("ERROR: recvLLQEvent - putLLQ");  return mDNSfalse; }
	err = mDNSSendDNSMessage(m, &ack, ackEnd, mDNSInterface_Any, srcaddr, srcport, mDNSNULL, mDNSNULL);
	if (err) debugf("ERROR: recvLLQEvent - mDNSSendDNSMessage returned %ld", err);
	return mDNStrue;
	}

mDNSlocal void recvRefreshReply(mDNS *m, DNSMessage *msg, const mDNSu8 *end, DNSQuestion *q)
	{
	LLQ_Info *qInfo;
	LLQOptData pktData;

	qInfo = q->llq;
	if (!getLLQAtIndex(m, msg, end, &pktData, 0)) { LogMsg("ERROR recvRefreshReply - getLLQAtIndex"); return; }
	if (pktData.llqOp != kLLQOp_Refresh) return;
	if (!mDNSSameOpaque64(&pktData.id, &qInfo->id)) { LogMsg("recvRefreshReply - ID mismatch.  Discarding");  return; }
	if (pktData.err != LLQErr_NoError) { LogMsg("recvRefreshReply: received error %d from server", pktData.err); return; }

	qInfo->expire    = q->LastQTime + ((mDNSs32)pktData.lease * mDNSPlatformOneSecond);
	q->ThisQInterval = ((mDNSs32)pktData.lease * mDNSPlatformOneSecond/2);

	qInfo->origLease = pktData.lease;
	qInfo->state = LLQ_Established;
	}

mDNSlocal mDNSBool recvLLQResponse(mDNS *m, DNSMessage *msg, const mDNSu8 *end, const mDNSAddr *srcaddr, mDNSIPPort srcport)
	{
	DNSQuestion pktQ, *q;
	const mDNSu8 *ptr = msg->data;
	LLQ_Info *llqInfo;

	if (!msg->h.numQuestions) return mDNSfalse;

	ptr = getQuestion(msg, ptr, end, 0, &pktQ);
	if (!ptr) return mDNSfalse;
	pktQ.TargetQID = msg->h.id;

	q = m->Questions;
	while (q)
		{
		llqInfo = q->llq;
		if (q->LongLived &&
			llqInfo &&
			q->qnamehash == pktQ.qnamehash &&
			q->qtype == pktQ.qtype &&
			SameDomainName(&q->qname, &pktQ.qname))
			{
			if (m->CurrentQuestion)
				LogMsg("recvLLQResponse: ERROR m->CurrentQuestion already set: %##s (%s)", m->CurrentQuestion->qname.c, DNSTypeName(m->CurrentQuestion->qtype));
			m->CurrentQuestion = q;
			if (llqInfo->state == LLQ_Established || (llqInfo->state == LLQ_Refresh && msg->h.numAnswers))
				{
				if (recvLLQEvent(m, q, msg, end, srcaddr, srcport)) { m->CurrentQuestion = mDNSNULL; return mDNStrue; }
				}
			else if (msg->h.id.NotAnInteger == q->TargetQID.NotAnInteger)
				{
				if (llqInfo->state == LLQ_Refresh && msg->h.numAdditionals && !msg->h.numAnswers)
					{ recvRefreshReply(m, msg, end, q); m->CurrentQuestion = mDNSNULL; return mDNStrue; }
				if (llqInfo->state < LLQ_Static)
					{
					if ((llqInfo->state != LLQ_InitialRequest && llqInfo->state != LLQ_SecondaryRequest) || mDNSSameAddress(srcaddr, &llqInfo->servAddr))
						{ q->responseCallback(m, msg, end, q); m->CurrentQuestion = mDNSNULL; return mDNStrue; }
					}
				}
			m->CurrentQuestion = mDNSNULL;
			}
		q = q->next;
		}
	return mDNSfalse;
	}

mDNSlocal void tcpCallback(uDNS_TCPSocket sock, void * context, mDNSBool ConnectionEstablished, mStatus err)
	{
	tcpInfo_t		*	tcpInfo = (tcpInfo_t *)context;
	mDNSBool			closed = mDNSfalse;
	mDNSu8			*	end;
	long				n;
	mDNS			*	m = tcpInfo->m;

	if (err) goto exit;

	if (ConnectionEstablished)
		{
		// connection is established - send the message
		if (tcpInfo->llqInfo)
			{
			LLQOptData	llqData;			// set llq rdata
			llqData.vers    = kLLQ_Vers;
			llqData.llqOp   = kLLQOp_Setup;
			llqData.err     = LLQErr_NoError;
			llqData.id      = zeroOpaque64;
			llqData.lease   = kLLQ_DefLease;
			initializeQuery(&tcpInfo->request, tcpInfo->llqInfo->question);
			end = putLLQ(&tcpInfo->request, tcpInfo->request.data, tcpInfo->llqInfo->question, &llqData, mDNStrue);

			if (!end)
				{
				LogMsg("ERROR: tcpCallback - putLLQ");
				err = mStatus_UnknownErr;
				goto exit;
				}
			}
		else if (tcpInfo->question)
			{
			err = constructQueryMsg(&tcpInfo->request, &end, tcpInfo->question);

			if (err)
				{
				LogMsg("ERROR: tcpCallback: constructQueryMsg - %ld", err);
				err = mStatus_UnknownErr;
				goto exit;
				}
			}
		else
			{
			end = ((mDNSu8*) &tcpInfo->request) + tcpInfo->requestLen;
			}

		err = mDNSSendDNSMessage(m, &tcpInfo->request, end, mDNSInterface_Any, &zeroAddr, zeroIPPort, sock, tcpInfo->authInfo);

		if (err)
			{
			debugf("ERROR: tcpCallback: mDNSSendDNSMessage - %ld", err);
			err = mStatus_UnknownErr;
			goto exit;
			}

		// Record time we sent this questions
		// (Not sure why we care)
		if (tcpInfo->question)
			tcpInfo->question->LastQTime = mDNS_TimeNow_NoLock(m);
		}
	else
		{
		if (tcpInfo->nread < 2)
			{
			// read msg len
			n = mDNSPlatformReadTCP(sock, tcpInfo->lenbuf + tcpInfo->nread, sizeof(tcpInfo->lenbuf) - tcpInfo->nread, &closed);
			if (n < 0)
				{
				LogMsg("ERROR:tcpCallback - attempt to read message length failed (read returned %d)", n);
				err = mStatus_ConnFailed;
				goto exit;
				}
			else if (!n && closed)
				{
				// It's perfectly fine for this socket to close after the first reply. The server might
				// be sending gratuitous replies using UDP and doesn't have a need to leave the TCP
				// socket open.
				// We'll only log this event if we've never received a reply before.
				if (tcpInfo->numReplies == 0) LogMsg("ERROR: socket close prematurely");
				err = mStatus_ConnFailed;
				goto exit;
				}

			tcpInfo->nread += n;
			if (tcpInfo->nread < (int) sizeof(tcpInfo->lenbuf)) goto exit;
			tcpInfo->replylen = (mDNSu16) ((mDNSu16) tcpInfo->lenbuf[0] << 8 | tcpInfo->lenbuf[1]);

			if (tcpInfo->replylen < sizeof(DNSMessageHeader))
				{
				LogMsg("ERROR: tcpCallback - length too short (%d bytes)", tcpInfo->replylen);
				err = mStatus_UnknownErr;
				goto exit;
				}

			tcpInfo->reply = mDNSPlatformMemAllocate(tcpInfo->replylen);

			if (!tcpInfo->reply)
				{
				LogMsg("ERROR: tcpCallback - malloc failed");
				err = mStatus_NoMemoryErr;
				goto exit;
				}
			}

		n = mDNSPlatformReadTCP(sock, ((char *)tcpInfo->reply) + (tcpInfo->nread - sizeof(tcpInfo->lenbuf)), tcpInfo->replylen - (tcpInfo->nread - sizeof(tcpInfo->lenbuf)), &closed);

		if (n < 0)
			{
			LogMsg("ERROR: tcpCallback - read returned %d", n);
			err = mStatus_ConnFailed;
			goto exit;
			}
		else if (!n && closed)
			{
			LogMsg("ERROR: socket close prematurely");
			err = mStatus_ConnFailed;
			goto exit;
			}

		tcpInfo->nread += n;

		if ((tcpInfo->nread - sizeof(tcpInfo->lenbuf)) == tcpInfo->replylen)
			{
			DNSMessage *msg = tcpInfo->reply;
			
			tcpInfo->numReplies++;
			
			if (tcpInfo->llqInfo)
				{
				// Finished reading message; convert the integer parts which are in IETF byte-order (MSB first, LSB second)
				mDNSu8 *ptr = (mDNSu8 *)&msg->h.numQuestions;
				msg->h.numQuestions   = (mDNSu16)((mDNSu16)ptr[0] << 8 | ptr[1]);
				msg->h.numAnswers     = (mDNSu16)((mDNSu16)ptr[2] << 8 | ptr[3]);
				msg->h.numAuthorities = (mDNSu16)((mDNSu16)ptr[4] << 8 | ptr[5]);
				msg->h.numAdditionals = (mDNSu16)((mDNSu16)ptr[6] << 8 | ptr[7]);

				mDNS_Lock(m);
				if (tcpInfo->llqInfo->state == LLQ_SecondaryRequest)
					{
					LLQOptData llq;
					if (getLLQAtIndex(m, msg, (mDNSu8*) (msg + tcpInfo->replylen), &llq, 0))
						tcpInfo->llqInfo->id = llq.id;
					}

				recvLLQResponse(m, msg, (mDNSu8*) (msg + tcpInfo->replylen), &tcpInfo->llqInfo->servAddr, tcpInfo->llqInfo->servPort);

				mDNSPlatformMemFree(tcpInfo->reply);
				tcpInfo->reply = mDNSNULL;
				tcpInfo->nread = 0;
				tcpInfo->replylen = 0;
				mDNS_Unlock(m);
				}
			else
				{
				mDNSCoreReceive(m, msg, (mDNSu8 *)msg + tcpInfo->replylen, mDNSNULL, zeroIPPort, mDNSNULL, zeroIPPort, 0);
				mDNSPlatformTCPCloseConnection(sock);
				mDNSPlatformMemFree(tcpInfo->reply);
				mDNSPlatformMemFree(tcpInfo);
				}
			}
		}

exit:

	if (err)
		{
		mDNSPlatformTCPCloseConnection(sock);

		if (tcpInfo->llqInfo)
			{
			tcpInfo->llqInfo->tcpSock = mDNSNULL;
			// ConnFailed is actually okay.  It just means that the server closed the connection but the LLQ
			// is still okay.  If the error isn't ConnFailed, then the LLQ is in bad shape.
			if (err != mStatus_ConnFailed) tcpInfo->llqInfo->state = LLQ_Error;
			}

		if (tcpInfo->rr)
			{
			mDNSBool deregPending = (tcpInfo->rr->state == regState_DeregPending) ? mDNStrue : mDNSfalse;

			unlinkAR(&m->RecordRegistrations, tcpInfo->rr);
			tcpInfo->rr->state = regState_Unregistered;

			if (!deregPending)
				{
				mDNS_DropLockBeforeCallback();
				if (tcpInfo->rr->RecordCallback) tcpInfo->rr->RecordCallback(m, tcpInfo->rr, err);
				mDNS_ReclaimLockAfterCallback();
				}
			}

		if (tcpInfo->srs)
			{
			mDNSBool deregPending = (tcpInfo->srs->state == regState_DeregPending) ? mDNStrue : mDNSfalse;

			unlinkSRS(m, tcpInfo->srs);
			tcpInfo->srs->state = regState_Unregistered;

			if (!deregPending)
				{
				mDNS_DropLockBeforeCallback();
				tcpInfo->srs->ServiceCallback(m, tcpInfo->srs, err);
				mDNS_ReclaimLockAfterCallback();
				//!!!KRS will mem still be free'd on error?
				// NOTE: not safe to touch any client structures here
				}
			}

		if (tcpInfo->reply)
			{
			mDNSPlatformMemFree(tcpInfo->reply);
			tcpInfo->reply = mDNSNULL;
			}

		mDNSPlatformMemFree(tcpInfo);
		}
	}

mDNSlocal void sendChallengeResponse(mDNS *m, DNSQuestion *q, LLQOptData *llq)
	{
	LLQ_Info *info = q->llq;
	DNSMessage response;
	mDNSu8 *responsePtr = response.data;
	mStatus err;
	LLQOptData llqBuf;
	mDNSs32 timenow = m->timenow;

	if (info->ntries++ == kLLQ_MAX_TRIES)
		{
		LogMsg("sendChallengeResponse: %d failed attempts for LLQ %##s Will re-try in %d minutes",
			   kLLQ_MAX_TRIES, q->qname.c, kLLQ_DEF_RETRY / 60);
		info->state      = LLQ_Retry;
		q->LastQTime     = m->timenow;
		q->ThisQInterval = (kLLQ_DEF_RETRY * mDNSPlatformOneSecond);
		// !!!KRS give a callback error in these cases?
		return;
		}

	if (!llq)
		{
		llq = &llqBuf;
		llq->vers  = kLLQ_Vers;
		llq->llqOp = kLLQOp_Setup;
		llq->err   = LLQErr_NoError;
		llq->id    = info->id;
		llq->lease = info->origLease;
		}

	q->LastQTime     = timenow;
	q->ThisQInterval = (kLLQ_INIT_RESEND * info->ntries * mDNSPlatformOneSecond);

	if (constructQueryMsg(&response, &responsePtr, q)) goto error;
	responsePtr = putLLQ(&response, responsePtr, q, llq, mDNSfalse);
	if (!responsePtr) { LogMsg("ERROR: sendChallengeResponse - putLLQ"); goto error; }

	err = mDNSSendDNSMessage(m, &response, responsePtr, mDNSInterface_Any, &info->servAddr, info->servPort, mDNSNULL, mDNSNULL);
	if (err) debugf("ERROR: sendChallengeResponse - mDNSSendDNSMessage returned %ld", err);
	// on error, we procede as normal and retry after the appropriate interval

	return;

	error:
	info->state = LLQ_Error;
	}

mDNSlocal void hndlRequestChallenge(mDNS *m, DNSMessage *pktMsg, const mDNSu8 *end, LLQOptData *llq, DNSQuestion *q)
	{
	LLQ_Info *info = q->llq;
	mDNSs32 timenow = m->timenow;
	switch(llq->err)
		{
		case LLQErr_NoError: break;
		case LLQErr_ServFull:
			LogMsg("hndlRequestChallenge - received ServFull from server for LLQ %##s.  Retry in %lu sec", q->qname.c, llq->lease);
			q->LastQTime     = m->timenow;
			q->ThisQInterval = ((mDNSs32)llq->lease * mDNSPlatformOneSecond);
			info->state = LLQ_Retry;
			pktResponseHndlr(m, pktMsg, end, q);  // get available answers
		case LLQErr_Static:
			info->state = LLQ_Static;
			LogMsg("LLQ %##s: static", q->qname.c);
			pktResponseHndlr(m, pktMsg, end, q);
			return;
		case LLQErr_FormErr:
			LogMsg("ERROR: hndlRequestChallenge - received FormErr from server for LLQ %##s", q->qname.c);
			goto error;
		case LLQErr_BadVers:
			LogMsg("ERROR: hndlRequestChallenge - received BadVers from server");
			goto error;
		case LLQErr_UnknownErr:
			LogMsg("ERROR: hndlRequestChallenge - received UnknownErr from server for LLQ %##s", q->qname.c);
			goto error;
		default:
			LogMsg("ERROR: hndlRequestChallenge - received invalid error %d for LLQ %##s", llq->err, q->qname.c);
			goto error;
		}

	if (info->origLease != llq->lease)
		debugf("hndlRequestChallenge: requested lease %lu, granted lease %lu", info->origLease, llq->lease);

	// cache expiration in case we go to sleep before finishing setup
	info->origLease = llq->lease;
	info->expire = timenow + ((mDNSs32)llq->lease * mDNSPlatformOneSecond);

	// update state
	info->state  = LLQ_SecondaryRequest;
	info->id     = llq->id;
	info->ntries = 0; // first attempt to send response

	sendChallengeResponse(m, q, llq);
	return;
	error:
	info->state = LLQ_Error;
	}

mDNSlocal void hndlChallengeResponseAck(mDNS *m, DNSMessage *pktMsg, const mDNSu8 *end, LLQOptData *llq, DNSQuestion *q)
	{
	LLQ_Info *info = q->llq;

	if (llq->err) { LogMsg("hndlChallengeResponseAck - received error %d from server", llq->err); goto error; }
	if (!mDNSSameOpaque64(&info->id, &llq->id)) { LogMsg("hndlChallengeResponseAck - ID changed.  discarding"); return; } // this can happen rarely (on packet loss + reordering)
	info->expire = m->timenow + ((mDNSs32)llq->lease * mDNSPlatformOneSecond);
	q->LastQTime     = m->timenow;
	q->ThisQInterval = ((mDNSs32)llq->lease * mDNSPlatformOneSecond / 2);
	info->origLease = llq->lease;
	info->state = LLQ_Established;
	q->responseCallback = pktResponseHndlr;

	pktResponseHndlr(m, pktMsg, end, q);
	return;

	error:
	info->state = LLQ_Error;
	}

// response handler for initial and secondary setup responses
mDNSlocal void recvSetupResponse(mDNS *m, DNSMessage *pktMsg, const mDNSu8 *end, DNSQuestion *q)
	{
	DNSQuestion pktQuestion;
	LLQOptData llq;
	const mDNSu8 *ptr = pktMsg->data;
	LLQ_Info *info = q->llq;
	mDNSu8 rcode = (mDNSu8)(pktMsg->h.flags.b[1] & kDNSFlag1_RC);
	mStatus err = mStatus_NoError;

	if (rcode && rcode != kDNSFlag1_RC_NXDomain)
		{
		err = mStatus_UnknownErr;
		goto exit;
		}

	ptr = getQuestion(pktMsg, ptr, end, 0, &pktQuestion);

	if (!ptr)
		{
		LogMsg("ERROR: recvSetupResponse - getQuestion");
		err = mStatus_UnknownErr;
		goto exit;
		}

	if (!SameDomainName(&q->qname, &pktQuestion.qname))
		{
		LogMsg("ERROR: recvSetupResponse - mismatched question in response for llq setup %##s", q->qname.c);
		err = mStatus_UnknownErr;
		goto exit;
		}

	if (!getLLQAtIndex(m, pktMsg, end, &llq, 0))
		{
		debugf("recvSetupResponse - GetLLQAtIndex");
		err = mStatus_UnknownErr;
		goto exit;
		}

	if (llq.llqOp != kLLQOp_Setup)
		{
		LogMsg("ERROR: recvSetupResponse - bad op %d", llq.llqOp);
		err = mStatus_UnknownErr;
		goto exit;
		}

	if (llq.vers != kLLQ_Vers)
		{
		LogMsg("ERROR: recvSetupResponse - bad vers %d", llq.vers);
		err = mStatus_UnknownErr;
		goto exit;
		}

	if (info->state == LLQ_InitialRequest)
		{
		hndlRequestChallenge(m, pktMsg, end, &llq, q);
		goto exit;
		}

	if (info->state == LLQ_SecondaryRequest)
		{
		// Fix this immediately if not sooner.  Copy the id from the LLQOptData into our llq info struct.  This is only
		// an issue for private LLQs, because we skip parts 2 and 3 of the handshake.  This is related to a bigger
		// problem of the current implementation of TCP LLQ setup: we're not handling state transitions correctly
		// if the server sends back SERVFULL or STATIC.
		if (info->isPrivate)
			info->id = llq.id;

		hndlChallengeResponseAck(m, pktMsg, end, &llq, q);
		goto exit;
		}

	LogMsg("recvSetupResponse - bad state %d", info->state);
	err = mStatus_UnknownErr;

exit:

	if (err)
		{
		startLLQPolling(m, info);
		}
	}

mDNSlocal void startLLQHandshakePrivate(mDNS *m, LLQ_Info *info, mDNSBool defer)
	{
	tcpInfo_t *context;
	mDNSs32 timenow = m->timenow;
	mStatus err = mStatus_NoError;
	DomainAuthInfo * authInfo = mDNSNULL;

	// Let's look for credentials right now.  If we can't find them, then it's an error.

	authInfo = GetAuthInfoForName(m, &info->question->qname);

	if (!authInfo)
		{
		LogMsg("ERROR: startLLQHandshakePrivate: cannot find credentials for question %##s", info->question->qname.c);
		err = mStatus_UnknownErr;
		goto exit;
		}

	if (!info->eventPort.NotAnInteger)
		{
		uDNS_TCPSocket	tcpSock = mDNSNULL;
		uDNS_UDPSocket	udpSock = mDNSNULL;
		mDNSIPPort		port = { { 0 } };
		mDNSBool		good = mDNSfalse;
		int				i;

		// Try 100 times to find sockets

		for (i = 0; i < 100; i++)
			{
			tcpSock = mDNSPlatformTCPSocket(m, TCP_SOCKET_FLAGS, &port);

			if (!tcpSock)
				{
				continue;
				}

			udpSock = mDNSPlatformUDPSocket(m, port);

			if (!udpSock)
				{
				mDNSPlatformTCPCloseConnection(tcpSock);
				continue;
				}

			good = mDNStrue;
			break;
			}

			if (!good)
				{
				err = mStatus_UnknownErr;
				goto exit;
				}

			info->tcpSock   = tcpSock;
			info->udpSock   = udpSock;
			info->eventPort = port;
		}

	if (IsPrivateV4Addr(&m->AdvertisedV4))
		{
		if (!info->NATInfoTCP)
			{
			info->state = LLQ_NatMapWaitTCP;
			StartLLQNatMap(m, info);
			goto exit;
			}
		else if (!info->NATInfoUDP)
			{
			info->state = LLQ_NatMapWaitUDP;

			StartLLQNatMap(m, info);

			goto exit;
			}
		else if (info->NATInfoTCP->state == NATState_Error || info->NATInfoUDP->state == NATState_Error)
			{
			err = mStatus_UnknownErr;
			goto exit;
			}
		else if (info->NATInfoTCP->state != NATState_Established && info->NATInfoTCP->state != NATState_Legacy)
			{
			info->state = LLQ_NatMapWaitTCP;
			goto exit;
			}
		else if (info->NATInfoUDP->state != NATState_Established && info->NATInfoUDP->state != NATState_Legacy)
			{
			info->state = LLQ_NatMapWaitUDP;
			goto exit;
			}
		}

	context = (tcpInfo_t *)mDNSPlatformMemAllocate(sizeof(tcpInfo_t));

	if (!context)
		{
		LogMsg("ERROR: startLLQHandshakePrivate - memallocate failed");
		err = mStatus_NoMemoryErr;
		goto exit;
		}

	mDNSPlatformMemZero(context, sizeof(tcpInfo_t));
	context->m = m;
	context->authInfo = authInfo;
	context->llqInfo = info;

	if (!defer)
		{
		if (!info->tcpSock)
			{
			LogMsg("ERROR: startLLQHandshakePrivate - tcpSock is NULL.");
			err = mStatus_UnknownErr;
			}
		else if (!mDNSPlatformTCPIsConnected(info->tcpSock))
			{
			err = mDNSPlatformTCPConnect(info->tcpSock, &info->servAddr, info->servPort, 0, tcpCallback, context);
			}
		else
			{
			err = mStatus_ConnEstablished;
			}

		if ((err != mStatus_ConnEstablished) && (err != mStatus_ConnPending))
			{
			LogMsg("startLLQHandshakePrivate: connection failed");
			goto exit;
			}

		if (err == mStatus_ConnEstablished)  // manually invoke callback if connection completes
			{
			tcpCallback(info->tcpSock, context, mDNStrue, mStatus_NoError);
			}

		err = mStatus_NoError;
		}

	// update question/info state

	info->state							= LLQ_SecondaryRequest;
	info->origLease						= kLLQ_DefLease;
	info->question->ThisQInterval       = (kLLQ_INIT_RESEND * mDNSPlatformOneSecond);
	info->question->LastQTime			= timenow;
	info->question->responseCallback	= recvSetupResponse;

exit:

	if (err)
		{
		startLLQPolling(m, info);
		}
	}

mDNSlocal void RemoveLLQNatMappings(mDNS * m, LLQ_Info * llq)
	{
	if (llq->NATInfoTCP && (--llq->NATInfoTCP->refs == 0))
		{
		uDNS_DeleteNATPortMapping(m, llq->NATInfoTCP);
		uDNS_FreeNATInfo(m, llq->NATInfoTCP);
		}

	llq->NATInfoTCP = mDNSNULL;

	if (llq->NATInfoUDP && (--llq->NATInfoUDP->refs == 0))
		{
		uDNS_DeleteNATPortMapping(m, llq->NATInfoUDP);
		uDNS_FreeNATInfo(m, llq->NATInfoUDP);
		}

	llq->NATInfoUDP = mDNSNULL;
	}

mDNSlocal void startLLQHandshake(mDNS *m, LLQ_Info *info, mDNSBool defer)
	{
	DNSMessage msg;
	mDNSu8 *end;
	LLQOptData llqData;
	DNSQuestion *q = info->question;
	mStatus err = mStatus_NoError;
	mDNSs32 timenow = m->timenow;

	if (IsPrivateV4Addr(&m->AdvertisedV4))
		{
		if (!info->NATInfoUDP)
			{
			info->state = LLQ_NatMapWaitUDP;
			StartLLQNatMap(m, info);
			}

		if (info->NATInfoUDP->state == NATState_Error)
			{
			err = mStatus_UnknownErr;
			goto exit;
			}

		if (info->NATInfoUDP->state != NATState_Established && info->NATInfoUDP->state != NATState_Legacy)
			{
			info->state = LLQ_NatMapWaitUDP;
			goto exit;
			}
		}

	if (info->ntries++ >= kLLQ_MAX_TRIES)
		{
		LogMsg("startLLQHandshake: %d failed attempts for LLQ %##s.  Polling.", kLLQ_MAX_TRIES, q->qname.c, kLLQ_DEF_RETRY / 60);
		err = mStatus_UnknownErr;
		goto exit;
		}

    // set llq rdata
	llqData.vers    = kLLQ_Vers;
	llqData.llqOp   = kLLQOp_Setup;
	llqData.err     = LLQErr_NoError;
	llqData.id      = zeroOpaque64;
	llqData.lease   = kLLQ_DefLease;

	initializeQuery(&msg, q);
	end = putLLQ(&msg, msg.data, q, &llqData, mDNStrue);
	if (!end)
		{
		LogMsg("ERROR: startLLQHandshake - putLLQ");
		info->state = LLQ_Error;
		return;
		}

	if (!defer) // if we are to defer, we simply set the retry timers so the request goes out in the future
		{
		err = mDNSSendDNSMessage(m, &msg, end, mDNSInterface_Any, &info->servAddr, info->servPort, mDNSNULL, mDNSNULL);

		// on error, we procede as normal and retry after the appropriate interval

		if (err)
			{
			debugf("ERROR: startLLQHandshake - mDNSSendDNSMessage returned %ld", err);
			err = mStatus_NoError;
			}
		}

	// update question/info state
	info->state = LLQ_InitialRequest;
	info->origLease = kLLQ_DefLease;
	q->LastQTime = timenow;
	q->ThisQInterval    = (kLLQ_INIT_RESEND * mDNSPlatformOneSecond);
	q->responseCallback = recvSetupResponse;

	err = mStatus_NoError;

exit:

	if (err)
		{
		startLLQPolling(m, info);
		}
	}

mDNSlocal void LLQNatMapComplete(mDNS *m, NATTraversalInfo * n)
	{
	if (!n) { LogMsg("Error: LLQNatMapComplete called with NULL NATTraversalInfo"); return; }
	if (n->state != NATState_Established && n->state != NATState_Legacy && n->state != NATState_Error)
		{ LogMsg("LLQNatMapComplete - bad nat state %d", n->state); return; }

	if (m->CurrentQuestion)
		LogMsg("LLQNatMapComplete: ERROR m->CurrentQuestion already set: %##s (%s)", m->CurrentQuestion->qname.c, DNSTypeName(m->CurrentQuestion->qtype));
	m->CurrentQuestion = m->Questions;

	while (m->CurrentQuestion)
		{
		DNSQuestion *q = m->CurrentQuestion;
		LLQ_Info *llqInfo = q->llq;
		m->CurrentQuestion = m->CurrentQuestion->next;
		if (q->LongLived && llqInfo)
			{
			if (llqInfo->isPrivate)
				{
				if ((llqInfo->state == LLQ_NatMapWaitTCP) && (llqInfo->NATInfoTCP == n))
					{
					if (n->state != NATState_Error)
						{
						llqInfo->state = LLQ_NatMapWaitUDP;
						startLLQHandshakePrivate(m, llqInfo, mDNSfalse);
						}
					else
						{
						RemoveLLQNatMappings(m, llqInfo);
						startLLQPolling(m, llqInfo);
						}
					}
				else if ((llqInfo->state == LLQ_NatMapWaitUDP) && (llqInfo->NATInfoUDP == n))
					{
					if (n->state != NATState_Error)
						{
						llqInfo->state = LLQ_GetZoneInfo;

						if (llqInfo->isPrivate)
							startLLQHandshakePrivate(m, llqInfo, mDNSfalse);
						else
							startLLQHandshake(m, llqInfo, mDNSfalse);
						}
					else
						{
						RemoveLLQNatMappings(m, llqInfo);
						startLLQPolling(m, llqInfo);
						}
					}
				}
			else
				{
				if (llqInfo->state == LLQ_NatMapWaitUDP)
				{
					if (n->state != NATState_Error)
						{
						llqInfo->state = LLQ_GetZoneInfo;
						startLLQHandshake(m, llqInfo, mDNSfalse);
						}
					else
						{
						RemoveLLQNatMappings(m, llqInfo);
						startLLQPolling(m, llqInfo);
						}
					}
				}
			}
		}
	m->CurrentQuestion = mDNSNULL;
	}

// if we ever want to refine support for multiple hostnames, we can add logic matching service names to a particular hostname
// for now, we grab the first registered DynDNS name, if any, or a static name we learned via a reverse-map query
mDNSlocal const domainname *GetServiceTarget(mDNS *m, AuthRecord *srv)
	{
	if (!srv->HostTarget)		// If not automatically tracking this host's current name, just return the exising target
		return(&srv->resrec.rdata->u.srv.target);
	else
		{
		uDNS_HostnameInfo *hi = m->Hostnames;
		while (hi)
			{
			if (hi->arv4 && (hi->arv4->state == regState_Registered || hi->arv4->state == regState_Refresh)) return(hi->arv4->resrec.name);
			if (hi->arv6 && (hi->arv6->state == regState_Registered || hi->arv6->state == regState_Refresh)) return(hi->arv6->resrec.name);
			hi = hi->next;
			}
		if (m->StaticHostname.c[0]) return(&m->StaticHostname);
		return(mDNSNULL);
		}
	}

mDNSlocal void SendServiceRegistration(mDNS *m, ServiceRecordSet *srs)
	{
	DNSMessage msg;
	mDNSu8 *ptr = msg.data;
	mDNSu8 *end = (mDNSu8 *)&msg + sizeof(DNSMessage);
	mDNSOpaque16 id;
	mStatus err = mStatus_NoError;
	mDNSIPPort privport;
	NATTraversalInfo *nat = srs->NATinfo;
	mDNSBool mapped = mDNSfalse;
	const domainname *target;
	DomainAuthInfo * authInfo;
	AuthRecord *srv = &srs->RR_SRV;
	mDNSu32 i;

	privport = zeroIPPort;

	if (!srs->ns.ip.v4.NotAnInteger) { LogMsg("SendServiceRegistration - NS not set!"); return; }

	id = mDNS_NewMessageID(m);
	InitializeDNSMessage(&msg.h, id, UpdateReqFlags);

	// setup resource records
	SetNewRData(&srs->RR_PTR.resrec, mDNSNULL, 0);
	SetNewRData(&srs->RR_TXT.resrec, mDNSNULL, 0);

	// replace port w/ NAT mapping if necessary
 	if (nat && nat->PublicPort.NotAnInteger &&
		(nat->state == NATState_Established || nat->state == NATState_Refresh || nat->state == NATState_Legacy))
		{
		privport = srv->resrec.rdata->u.srv.port;
		srv->resrec.rdata->u.srv.port = nat->PublicPort;
		mapped = mDNStrue;
		}

	// construct update packet
    // set zone
	ptr = putZone(&msg, ptr, end, &srs->zone, mDNSOpaque16fromIntVal(srv->resrec.rrclass));
	if (!ptr) { err = mStatus_UnknownErr; goto exit; }

	if (srs->TestForSelfConflict)
		{
		// update w/ prereq that SRV already exist to make sure previous registration was ours, and delete any stale TXT records
		if (!(ptr = PutResourceRecordTTLJumbo(&msg, ptr, &msg.h.mDNS_numPrereqs, &srs->RR_SRV.resrec, 0))) { err = mStatus_UnknownErr; goto exit; }
		if (!(ptr = putDeleteRRSet(&msg, ptr, srs->RR_TXT.resrec.name, srs->RR_TXT.resrec.rrtype)))       { err = mStatus_UnknownErr; goto exit; }
		}

	else if (srs->state != regState_Refresh && srs->state != regState_UpdatePending)
		{
		// use SRV name for prereq
		ptr = putPrereqNameNotInUse(srv->resrec.name, &msg, ptr, end);
		if (!ptr) { err = mStatus_UnknownErr; goto exit; }
		}

	//!!!KRS Need to do bounds checking and use TCP if it won't fit!!!
	if (!(ptr = PutResourceRecordTTLJumbo(&msg, ptr, &msg.h.mDNS_numUpdates, &srs->RR_PTR.resrec, srs->RR_PTR.resrec.rroriginalttl))) { err = mStatus_UnknownErr; goto exit; }

	for (i = 0; i < srs->NumSubTypes; i++)
		if (!(ptr = PutResourceRecordTTLJumbo(&msg, ptr, &msg.h.mDNS_numUpdates, &srs->SubTypes[i].resrec, srs->SubTypes[i].resrec.rroriginalttl))) { err = mStatus_UnknownErr; goto exit; }

	if (srs->state == regState_UpdatePending) // we're updating the txt record
		{
		AuthRecord *txt = &srs->RR_TXT;
		// delete old RData
		SetNewRData(&txt->resrec, txt->OrigRData, txt->OrigRDLen);
		if (!(ptr = putDeletionRecord(&msg, ptr, &srs->RR_TXT.resrec))) { err = mStatus_UnknownErr; goto exit; }  // delete old rdata

		// add new RData
		SetNewRData(&txt->resrec, txt->InFlightRData, txt->InFlightRDLen);
		if (!(ptr = PutResourceRecordTTLJumbo(&msg, ptr, &msg.h.mDNS_numUpdates, &srs->RR_TXT.resrec, srs->RR_TXT.resrec.rroriginalttl))) { err = mStatus_UnknownErr; goto exit; }
		}
	else
		if (!(ptr = PutResourceRecordTTLJumbo(&msg, ptr, &msg.h.mDNS_numUpdates, &srs->RR_TXT.resrec, srs->RR_TXT.resrec.rroriginalttl))) { err = mStatus_UnknownErr; goto exit; }

	target = GetServiceTarget(m, srv);
	if (!target)
		{
		debugf("Couldn't get target for service %##s", srv->resrec.name->c);
		srs->state = regState_NoTarget;
		return;
		}

	if (!SameDomainName(target, &srv->resrec.rdata->u.srv.target))
		{
		AssignDomainName(&srv->resrec.rdata->u.srv.target, target);
		SetNewRData(&srv->resrec, mDNSNULL, 0);
		}

	ptr = PutResourceRecordTTLJumbo(&msg, ptr, &msg.h.mDNS_numUpdates, &srv->resrec, srv->resrec.rroriginalttl);
	if (!ptr) { err = mStatus_UnknownErr; goto exit; }

	if (srs->lease)
		{ ptr = putUpdateLease(&msg, ptr, DEFAULT_UPDATE_LEASE); if (!ptr) { err = mStatus_UnknownErr; goto exit; } }

	if (srs->state != regState_Refresh && srs->state != regState_DeregDeferred && srs->state != regState_UpdatePending)
		{
		srs->state = regState_Pending;
		}

	srs->id = id;
	authInfo  = GetAuthInfoForName(m, srs->RR_SRV.resrec.name);

	if (srs->Private)
		{
		tcpInfo_t		*	info;
		uDNS_TCPSocket		sock;
		mDNSIPPort			port = { { 0 } };

		info = (tcpInfo_t *)mDNSPlatformMemAllocate(sizeof(tcpInfo_t));

		if (!info)
			{
			LogMsg("ERROR: SendServiceRegistration - memallocate failed");
			err = mStatus_UnknownErr;
			goto exit;
			}

		mDNSPlatformMemZero(info, sizeof(tcpInfo_t));
		info->request  = msg;
		info->m        = m;
		info->authInfo = authInfo;
		info->srs      = srs;
		info->requestLen = (int) (ptr - ((mDNSu8*) &msg));

		sock = mDNSPlatformTCPSocket(m, TCP_SOCKET_FLAGS, &port);

		if (!sock)
			{
			LogMsg("SendServiceRegistration: uanble to create TCP socket");
			mDNSPlatformMemFree(info);
			goto exit;
			}

		err = mDNSPlatformTCPConnect(sock, &srs->ns, srs->port, 0, tcpCallback, info);

		// manually invoke callback if connection completes

		if (err == mStatus_ConnEstablished)
			{
			tcpCallback(sock, info, mDNStrue, mStatus_NoError);
			err = mStatus_NoError;
			}
		else if (err == mStatus_ConnPending)
			{
			err = mStatus_NoError;
			}
		else
			{
			LogMsg("SendServiceRegistration: connection failed");
			mDNSPlatformMemFree(info);
			goto exit;
			}
		}
	else
		{
		err = mDNSSendDNSMessage(m, &msg, ptr, mDNSInterface_Any, &srs->ns, srs->port, mDNSNULL, authInfo);
		if (err) debugf("ERROR: SendServiceRegistration - mDNSSendDNSMessage - %ld", err);
		}

	SetRecordRetry(m, &srs->RR_SRV, err);

	err = mStatus_NoError;

exit:

	if (mapped)
		{
		srv->resrec.rdata->u.srv.port = privport;
		}

	if (err)
		{
		LogMsg("SendServiceRegistration - Error formatting message");

		unlinkSRS(m, srs);
		srs->state = regState_Unregistered;
		mDNS_DropLockBeforeCallback();
		srs->ServiceCallback(m, srs, err);
		mDNS_ReclaimLockAfterCallback();
		//!!!KRS will mem still be free'd on error?
		// NOTE: not safe to touch any client structures here
		}
	}

/* StartGetZoneData
 *
 * Asynchronously find the address of the nameserver for the enclosing zone for a given domain name,
 * i.e. the server to which update and LLQ requests will be sent for a given name.  Once the address is
 * derived, it will be passed to the callback, along with a context pointer.  If the zone cannot
 * be determined or if an error occurs, an all-zeros address will be passed and a message will be
 * written to the syslog.
 *
 * If the FindUpdatePort arg is set, the port on which the server accepts dynamic updates is determined
 * by querying for the _dns-update._udp.<zone>. SRV record.  Likewise, if the FindLLQPort arg is set,
 * the port on which the server accepts long lived queries is determined by querying for
 * _dns-llq._udp.<zone>. record.  If either of these queries fail, or flags are not specified,
 * the llqPort and updatePort fields in the result structure are set to zero.
 *
 *  Steps for deriving the zone name are as follows:
 *
 * Query for an SOA record for the required domain.  If we don't get an answer (or an SOA in the Authority
 * section), we strip the leading label from the name and repeat, until we get an answer.
 *
 * The name of the SOA record is our enclosing zone.  The mname field in the SOA rdata is the domain
 * name of the primary NS.
 *
 * We verify that there is an NS record with this zone for a name and the mname for its rdata.
 * (!!!KRS this seems redundant, but BIND does this, and it should normally be zero-overhead since
 * the NS query will get us address records in the additionals section, which we'd otherwise have to
 * explicitly query for.)
 *
 * We then query for the address record for this nameserver (if it is not in the addionals section of
 * the NS record response.)
 */

// state machine states
typedef enum
	{
	init,
	lookupSOA,
	foundZone,
	lookupNS,
	foundNS,
	lookupA,
	foundA,
	lookupPort,
	foundPort,
	complete
	} ntaState;

// state machine actions
typedef enum
	{
	smContinue,  // continue immediately to next state
	smBreak,     // break until next packet/timeout
	smError      // terminal error - cleanup and abort
	} smAction;

typedef enum { lookupUpdateSRV, lookupQuerySRV, lookupLLQSRV } AsyncOpTarget;

typedef struct
	{
	domainname      origName;            // name we originally try to convert
	domainname      *curSOA;             // name we have an outstanding SOA query for
	ntaState        state;               // determines what we do upon receiving a packet
	mDNS            *m;
	domainname      zone;                // left-hand-side of SOA record
	mDNSu16         zoneClass;
	domainname      ns;                  // mname in SOA rdata, verified in confirmNS state
	mDNSv4Addr      addr;                // address of nameserver
	DNSQuestion     question;            // storage for any active question
	DNSQuestion     extraQuestion;       // additional storage
	mDNSBool        questionActive;      // if true, StopQuery() can be called on the question field
	AsyncOpTarget   target;
	mDNSBool        isPrivate;           // if true, we try to lookup the privateSRV first
	mDNSIPPort      updatePort;
	mDNSIPPort      queryPort;
	mDNSIPPort      llqPort;
	AsyncOpCallback *callback;       // caller specified function to be called upon completion
	void            *callbackInfo;
	} ntaContext;

mDNSlocal const domainname *PUBLIC_UPDATE_SERVICE_TYPE  = (const domainname*)"\x0B_dns-update"     "\x04_udp";
mDNSlocal const domainname *PUBLIC_LLQ_SERVICE_TYPE     = (const domainname*)"\x08_dns-llq"        "\x04_udp";

mDNSlocal const domainname *PRIVATE_UPDATE_SERVICE_TYPE = (const domainname*)"\x0F_dns-update-tls" "\x04_tcp";
mDNSlocal const domainname *PRIVATE_QUERY_SERVICE_TYPE  = (const domainname*)"\x0E_dns-query-tls"  "\x04_tcp";
mDNSlocal const domainname *PRIVATE_LLQ_SERVICE_TYPE    = (const domainname*)"\x0C_dns-llq-tls"    "\x04_tcp";

#define ntaContextSRV(X) (\
	(X)->target == lookupUpdateSRV ? (context->isPrivate ? PRIVATE_UPDATE_SERVICE_TYPE : PUBLIC_UPDATE_SERVICE_TYPE) : \
	(X)->target == lookupQuerySRV  ? (context->isPrivate ? PRIVATE_QUERY_SERVICE_TYPE  : PRIVATE_QUERY_SERVICE_TYPE) : \
	(X)->target == lookupLLQSRV    ? (context->isPrivate ? PRIVATE_LLQ_SERVICE_TYPE    : PUBLIC_LLQ_SERVICE_TYPE   ) : \
	(const domainname*)"")

// Forward reference: hndlLookupSOA references GetZoneData_Callback and vice versa
mDNSlocal void GetZoneData_Callback(mDNS *const m, DNSMessage *msg, const mDNSu8 *end, DNSQuestion *question);

// explicitly set response handler
mDNSlocal mStatus GetZoneData_StartQuery(mDNS *m, DNSQuestion *q, InternalResponseHndlr callback, void *hndlrContext)
	{
	mStatus status;
	q->llq              = mDNSNULL;
	q->sock             = mDNSNULL;
	q->Answered         = mDNSfalse;
	q->RestartTime      = 0;
	q->QuestionContext  = hndlrContext;
	q->responseCallback = callback;
	
	// Temporary hack.
	// GetZoneData_StartQuery is called with the lock held, but not from a normal callback that allows API calls
	// Eventually we should fix this.
	// For now we increment mDNS_reentrancy to suppress the syslog error messages
	mDNS_DropLockBeforeCallback();
	//LogMsg("GetZoneData_StartQuery %##s (%s) %p", q->qname.c, DNSTypeName(q->qtype), q->Private);
	status = mDNS_StartQuery(m, q);
	// GetZoneData queries are a special case -- even if we have a key for them, we don't do them
	// privately, because that results in an infinite loop (i.e. to do a private query we first
	// need to get the _dns-query-tls SRV record for the zone, and we can't do *that* privately
	// because to do so we'd need to already know the _dns-query-tls SRV record
	q->Private = mDNSNULL;
	mDNS_ReclaimLockAfterCallback();

	return(status);
	}

mDNSlocal void processSOA(ntaContext *context, ResourceRecord *rr)
	{
	AssignDomainName(&context->zone, rr->name);
	context->zoneClass = rr->rrclass;
	AssignDomainName(&context->ns, &rr->rdata->u.soa.mname);
	context->state = foundZone;
	}

mDNSlocal smAction hndlLookupSOA(DNSMessage *msg, const mDNSu8 *end, ntaContext *context)
    {
    mStatus err;
    LargeCacheRecord lcr;
	ResourceRecord *rr = &lcr.r.resrec;
	DNSQuestion *query = &context->question;
	const mDNSu8 *ptr;

    if (msg)
        {
        // if msg contains SOA record in answer or authority sections, update context/state and return
		int i;
		ptr = LocateAnswers(msg, end);
		for (i = 0; i < msg->h.numAnswers; i++)
			{
			ptr = GetLargeResourceRecord(context->m, msg, ptr, end, 0, kDNSRecordTypePacketAns, &lcr);
			if (!ptr) { LogMsg("ERROR: hndlLookupSOA, Answers - GetLargeResourceRecord returned NULL");  return smError; }
			if (rr->rrtype == kDNSType_SOA && SameDomainName(context->curSOA, rr->name))
				{
				processSOA(context, rr);
				return smContinue;
				}
			}
		ptr = LocateAuthorities(msg, end);
		// SOA not in answers, check in authority
		for (i = 0; i < msg->h.numAuthorities; i++)
			{
			ptr = GetLargeResourceRecord(context->m, msg, ptr, end, 0, kDNSRecordTypePacketAns, &lcr); ///!!!KRS using type PacketAns for auth
			if (!ptr) { LogMsg("ERROR: hndlLookupSOA, Authority - GetLargeResourceRecord returned NULL");  return smError; }
			if (rr->rrtype == kDNSType_SOA)
				{
				processSOA(context, rr);
				return smContinue;
				}
			}
		}

    if (context->state != init && !context->curSOA->c[0])
        {
        // we've gone down to the root and have not found an SOA
        LogMsg("ERROR: hndlLookupSOA - recursed to root label of %##s without finding SOA",
                context->origName.c);
		return smError;
        }

    // chop off leading label unless this is our first try
    if (context->state == init)  context->curSOA = &context->origName;
    else                         context->curSOA = (domainname *)(context->curSOA->c + context->curSOA->c[0]+1);

    context->state = lookupSOA;
    AssignDomainName(&query->qname, context->curSOA);
    query->qtype = kDNSType_SOA;
    query->qclass = kDNSClass_IN;
    err = GetZoneData_StartQuery(context->m, &context->question, GetZoneData_Callback, context);
	if (err) LogMsg("hndlLookupSOA: GetZoneData_StartQuery returned error %ld (breaking until next periodic retransmission)", err);

    return smBreak;     // break from state machine until we receive another packet
    }

mDNSlocal smAction confirmNS(DNSMessage *msg, const mDNSu8 *end, ntaContext *context)
	{
	DNSQuestion *query = &context->question;
	mStatus err;
	LargeCacheRecord lcr;
	const ResourceRecord *const rr = &lcr.r.resrec;
	const mDNSu8 *ptr;
	int i;

	if (context->state == foundZone)
	{
		// we've just learned the zone.  confirm that an NS record exists
		AssignDomainName(&query->qname, &context->zone);
		query->qtype = kDNSType_NS;
		query->qclass = kDNSClass_IN;
		err = GetZoneData_StartQuery(context->m, query, GetZoneData_Callback, context);
		if (err) LogMsg("confirmNS: GetZoneData_StartQuery returned error %ld (breaking until next periodic retransmission", err);
		context->state = lookupNS;
		return smBreak;  // break from SM until we receive another packet
	}
	else if (context->state == lookupNS)
	{
		ptr = LocateAnswers(msg, end);

		for (i = 0; i < msg->h.numAnswers; i++)
		{
			ptr = GetLargeResourceRecord(context->m, msg, ptr, end, 0, kDNSRecordTypePacketAns, &lcr);

			if (!ptr)
			{
				LogMsg("ERROR: confirmNS, Answers - GetLargeResourceRecord returned NULL");
				return smError;
			}

			if (rr->rrtype == kDNSType_NS && SameDomainName(&context->zone, rr->name) && SameDomainName(&context->ns, &rr->rdata->u.name))
			{
				context->state = foundNS;
				return smContinue;  // next routine will examine additionals section of A record
			}
		}

		debugf("ERROR: could not confirm existence of record %##s NS %##s", context->zone.c, context->ns.c);
		return smError;
	}
	else
		{
			LogMsg("ERROR: confirmNS - bad state %d", context->state);
			return smError;
		}
	}
mDNSlocal smAction lookupDNSPort(DNSMessage *msg, const mDNSu8 *end, ntaContext *context, mDNSIPPort *port)
	{
	int i;
	LargeCacheRecord lcr;
	const mDNSu8 *ptr;
	mStatus err;

	LogOperation("lookupDNSPort %##s", ntaContextSRV(context));

	if (context->state == lookupPort)  // we've already issued the query
		{
		if (!msg) { LogMsg("ERROR: lookupDNSPort - NULL message"); return smError; }
		ptr = LocateAnswers(msg, end);
		for (i = 0; i < msg->h.numAnswers; i++)
			{
			ptr = GetLargeResourceRecord(context->m, msg, ptr, end, 0, kDNSRecordTypePacketAns, &lcr);
			if (!ptr) { LogMsg("ERROR: lookupDNSPort - GetLargeResourceRecord returned NULL");  return smError; }
			if (ResourceRecordAnswersQuestion(&lcr.r.resrec, &context->question))
				{
				*port = lcr.r.resrec.rdata->u.srv.port;
				context->state = foundPort;
				return smContinue;
				}
			}
		LogOperation("lookupDNSPort - no answer for type %##s", ntaContextSRV(context));
		*port = zeroIPPort;

		// If the context is private, and we couldn't lookup the SRV record
		// then let's retry this query with the public SRV
		if (context->isPrivate)
			{
			// Note: This state change causes ntaContextSRV() below to
			// yield a different SRV name when building the query below
			context->isPrivate = mDNSfalse;
			}
		else
			{
			context->state = foundPort;
			return smContinue;
			}
		}

	// query the server for the update port for the zone
	context->state = lookupPort;
	context->question.qname.c[0] = 0;
	AppendDomainName(&context->question.qname, ntaContextSRV(context));
	AppendDomainName(&context->question.qname, &context->zone);
	context->question.qtype = kDNSType_SRV;
	context->question.qclass = kDNSClass_IN;
	err = GetZoneData_StartQuery(context->m, &context->question, GetZoneData_Callback, context);
	if (err) LogMsg("lookupDNSPort: GetZoneData_StartQuery returned error %ld (breaking until next periodic retransmission)", err);
	return smBreak;     // break from state machine until we receive another packet
	}

mDNSlocal smAction queryNSAddr(ntaContext *context)
	{
	mStatus err;
	DNSQuestion *query = &context->question;

	AssignDomainName(&query->qname, &context->ns);
	query->qtype = kDNSType_A;
	query->qclass = kDNSClass_IN;
	err = GetZoneData_StartQuery(context->m, query, GetZoneData_Callback, context);
	if (err) LogMsg("confirmNS: GetZoneData_StartQuery returned error %ld (breaking until next periodic retransmission)", err);
	context->state = lookupA;
	return smBreak;
	}

mDNSlocal smAction lookupNSAddr(DNSMessage *msg, const mDNSu8 *end, ntaContext *context)
	{
	const mDNSu8 *ptr;
	int i;
	LargeCacheRecord lcr;
	ResourceRecord *rr = &lcr.r.resrec;

	if (context->state == foundNS)
		{
		// we just found the NS record - look for the corresponding A record in the Additionals section
		if (!msg->h.numAdditionals) return queryNSAddr(context);
		ptr = LocateAdditionals(msg, end);
		if (!ptr)
			{
			LogMsg("ERROR: lookupNSAddr - LocateAdditionals returned NULL, expected %d additionals", msg->h.numAdditionals);
			return queryNSAddr(context);
			}
		else
			{
			for (i = 0; i < msg->h.numAdditionals; i++)
				{
				ptr = GetLargeResourceRecord(context->m, msg, ptr, end, 0, kDNSRecordTypePacketAns, &lcr);
				if (!ptr)
					{
					LogMsg("ERROR: lookupNSAddr, Additionals - GetLargeResourceRecord returned NULL");
					return queryNSAddr(context);
					}
				if (rr->rrtype == kDNSType_A && SameDomainName(&context->ns, rr->name))
					{
					context->addr = rr->rdata->u.ipv4;
					context->state = foundA;
					return smContinue;
					}
				}
			}
		// no A record in Additionals - query the server
		return queryNSAddr(context);
		}
	else if (context->state == lookupA)
		{
		ptr = LocateAnswers(msg, end);
		if (!ptr) { LogMsg("ERROR: lookupNSAddr: LocateAnswers returned NULL");  return smError; }
		for (i = 0; i < msg->h.numAnswers; i++)
			{
			ptr = GetLargeResourceRecord(context->m, msg, ptr, end, 0, kDNSRecordTypePacketAns, &lcr);
			if (!ptr) { LogMsg("ERROR: lookupNSAddr, Answers - GetLargeResourceRecord returned NULL"); break; }
			if (rr->rrtype == kDNSType_A && SameDomainName(&context->ns, rr->name))
				{
				context->addr = rr->rdata->u.ipv4;
				context->state = foundA;
				return smContinue;
				}
			}
		LogMsg("ERROR: lookupNSAddr: Address record not found in answer section");
		return smError;
		}
	else { LogMsg("ERROR: lookupNSAddr - bad state %d", context->state); return smError; }
	}

// state machine entry routine
mDNSlocal void GetZoneData_Callback(mDNS *const m, DNSMessage *msg, const mDNSu8 *end, DNSQuestion *question)
    {
	AsyncOpResult result;
	ntaContext *context = (ntaContext*)question->QuestionContext;
	smAction action;

    // unused
	(void)m;

	// stop any active question
	if (context->question.ThisQInterval >= 0)
		{
		// Seems like a hack -- if this code is allowed to issue API calls, then it should have been called in the appropriate state in the first place
		// and if it's not allowed to issue API calls, then deliberately circumventing the locking check like this seems like it's asking for
		// tricky hard-to-diagnose crashing bugs
		mDNS_DropLockBeforeCallback();
		mDNS_StopQuery(context->m, &context->question);
		mDNS_ReclaimLockAfterCallback();
		}

	if (msg && (msg->h.flags.b[1] & kDNSFlag1_RC) && (msg->h.flags.b[1] & kDNSFlag1_RC) != kDNSFlag1_RC_NXDomain)
		{
		// rcode non-zero, non-nxdomain
		LogMsg("ERROR: GetZoneData_Callback - received response w/ rcode %d", msg->h.flags.b[1]  & kDNSFlag1_RC);
		goto error;
		}

	if (question) LogOperation("GetZoneData_Callback: Question %##s", question->qname.c);
	switch (context->state)
        {
        case init:
        case lookupSOA:
            action = hndlLookupSOA(msg, end, context);
			if (action == smError) goto error;
			if (action == smBreak) return;
		case foundZone:
		case lookupNS:
			action = confirmNS(msg, end, context);
			if (action == smError) goto error;
			if (action == smBreak) return;
		case foundNS:
		case lookupA:
			action = lookupNSAddr(msg, end, context);
			if (action == smError) goto error;
			if (action == smBreak) return;
		case foundA:
		case lookupPort:
			action = smError;
			switch (context->target)
				{
				case lookupUpdateSRV: action = lookupDNSPort(msg, end, context, &context->updatePort); break;
				case lookupQuerySRV:  action = lookupDNSPort(msg, end, context, &context->queryPort ); break;
				case lookupLLQSRV:    action = lookupDNSPort(msg, end, context, &context->llqPort   ); break;
				}
			if (action == smError) goto error;
			if (action == smBreak) return;
			if (action == smContinue) context->state = complete;
		case foundPort:
		case complete: break;
		}

	if (context->state != complete)
		{
		LogMsg("ERROR: GetZoneData_Callback - exited state machine with state %d", context->state);
		goto error;
		}

	result.zoneData.primaryAddr.ip.v4 = context->addr;
	result.zoneData.primaryAddr.type = mDNSAddrType_IPv4;
	AssignDomainName(&result.zoneData.zoneName, &context->zone);
	result.zoneData.zoneClass = context->zoneClass;
	result.zoneData.isPrivate = context->isPrivate;
	result.zoneData.updatePort = context->target == lookupUpdateSRV ? context->updatePort : zeroIPPort;
	result.zoneData.queryPort  = context->target == lookupQuerySRV  ? context->queryPort  : zeroIPPort;
	result.zoneData.llqPort    = context->target == lookupLLQSRV    ? context->llqPort    : zeroIPPort;
	context->callback(mStatus_NoError, context->m, context->callbackInfo, &result);
	goto cleanup;

error:
	if (context && context->callback)
		context->callback(mStatus_UnknownErr, context->m, context->callbackInfo, mDNSNULL);
cleanup:
	if (context && context->question.ThisQInterval >= 0)
		{
		// Seems like a hack -- if this code is allowed to issue API calls, then it should have been called in the appropriate state in the first place
		// and if it's not allowed to issue API calls, then deliberately circumventing the locking check like this seems like it's asking for
		// tricky hard-to-diagnose crashing bugs
		mDNS_DropLockBeforeCallback();
		mDNS_StopQuery(context->m, &context->question);
		mDNS_ReclaimLockAfterCallback();
		}
    if (context) mDNSPlatformMemFree(context);
	}

// initialization
mDNSlocal mStatus StartGetZoneData(mDNS *m, domainname *name, AsyncOpTarget target, AsyncOpCallback callback, void *callbackInfo)
    {
    ntaContext *context = (ntaContext*)mDNSPlatformMemAllocate(sizeof(ntaContext));
    if (!context) { LogMsg("ERROR: StartGetZoneData - mDNSPlatformMemAllocate failed");  return mStatus_NoMemoryErr; }

	if (m->mDNS_busy != m->mDNS_reentrancy+1)
		LogMsg("StartGetZoneData: Lock not held! mDNS_busy (%ld) mDNS_reentrancy (%ld)", m->mDNS_busy, m->mDNS_reentrancy);

	mDNSPlatformMemZero(context, sizeof(ntaContext));
    AssignDomainName(&context->origName, name);
    context->curSOA          = mDNSNULL;
    context->state           = init;
    context->m               = m;
    context->zone.c[0]       = 0;
    context->zoneClass       = 0;
    context->ns.c[0]         = 0;
    context->addr            = zerov4Addr;
    context->question.ThisQInterval = -1;
    context->target          = target;
    context->isPrivate       = GetAuthInfoForName(m, name) ? mDNStrue : mDNSfalse;
    context->callback        = callback;
    context->callbackInfo    = callbackInfo;
    context->question.QuestionContext = context;
    GetZoneData_Callback(m, mDNSNULL, mDNSNULL, &context->question);
    return mStatus_NoError;
    }

// Forward reference: StartNATPortMap references serviceRegistrationCallback and vice versa
mDNSlocal void serviceRegistrationCallback(mStatus err, mDNS *const m, void *srsPtr, const AsyncOpResult *result);

mDNSlocal void StartNATPortMap(mDNS *m, ServiceRecordSet *srs)
	{
	NATOp_t op;
	NATTraversalInfo *info;

   	if (DomainContainsLabelString(srs->RR_PTR.resrec.name, "_tcp")) op = NATOp_MapTCP;
	else if (DomainContainsLabelString(srs->RR_PTR.resrec.name, "_udp")) op = NATOp_MapUDP;
	else { LogMsg("StartNATPortMap: could not determine transport protocol of service %##s", srs->RR_SRV.resrec.name->c); goto error; }

	if (srs->NATinfo) { LogMsg("Error: StartNATPortMap - NAT info already initialized!");  uDNS_FreeNATInfo(m, srs->NATinfo); }
	info = uDNS_AllocNATInfo(m, op, srs->RR_SRV.resrec.rdata->u.srv.port, zeroIPPort, 0, uDNS_HandleNATPortMapReply);
	srs->NATinfo = info;
	info->reg.ServiceRegistration = srs;
	info->state = NATState_Request;

	FormatPortMaprequest(info);
	SendInitialPMapReq(m, info);
	return;

	error:
	StartGetZoneData(m, srs->RR_SRV.resrec.name, lookupUpdateSRV, serviceRegistrationCallback, srs);
	}

mDNSlocal void serviceRegistrationCallback(mStatus err, mDNS *const m, void *srsPtr, const AsyncOpResult *result)
	{
	ServiceRecordSet *srs = (ServiceRecordSet *)srsPtr;
	const zoneData_t *zoneData = mDNSNULL;

	if (err) goto error;
	if (!result) { LogMsg("ERROR: serviceRegistrationCallback invoked with NULL result and no error");  goto error; }
	else zoneData = &result->zoneData;

	if (srs->state == regState_Cancelled)
		{
		// client cancelled registration while fetching zone data
		srs->state = regState_Unregistered;
		unlinkSRS(m, srs);
		mDNS_DropLockBeforeCallback();
		srs->ServiceCallback(m, srs, mStatus_MemFree);
		mDNS_ReclaimLockAfterCallback();
		return;
		}

	if (srs->RR_SRV.resrec.rrclass != zoneData->zoneClass)
		{
		LogMsg("Service %##s - class does not match zone", srs->RR_SRV.resrec.name->c);
		goto error;
		}

	// cache zone data
	AssignDomainName(&srs->zone, &zoneData->zoneName);
    srs->ns.type = mDNSAddrType_IPv4;
	srs->ns = zoneData->primaryAddr;
	if (zoneData->updatePort.NotAnInteger)
	{
		srs->port = zoneData->updatePort;
		srs->Private = zoneData->isPrivate;
	}
	else
		{
		debugf("Update port not advertised via SRV - guessing port 53, no lease option");
		srs->port = UnicastDNSPort;
		srs->lease = mDNSfalse;
		}

	if (srs->RR_SRV.resrec.rdata->u.srv.port.NotAnInteger && IsPrivateV4Addr(&m->AdvertisedV4))
		{ srs->state = regState_NATMap; StartNATPortMap(m, srs); }
	else SendServiceRegistration(m, srs);
	return;

error:
	unlinkSRS(m, srs);
	srs->state = regState_Unregistered;
	mDNS_DropLockBeforeCallback();
	srs->ServiceCallback(m, srs, err);
	mDNS_ReclaimLockAfterCallback();
	// NOTE: not safe to touch any client structures here
	}

mDNSexport mDNSBool uDNS_HandleNATPortMapReply(NATTraversalInfo *n, mDNS *m, mDNSu8 *pkt, mDNSu16 len)
	{
	ServiceRecordSet *srs = n->reg.ServiceRegistration;
	mDNSIPPort priv = srs ? srs->RR_SRV.resrec.rdata->u.srv.port : n->PrivatePort;
	mDNSBool deletion = !n->request.PortReq.lease.NotAnInteger;
	NATPortMapReply *reply = (NATPortMapReply *)pkt;
	mDNSu8 *service = srs ? srs->RR_SRV.resrec.name->c : (mDNSu8 *)"\016LLQ event port";

	if (n->state != NATState_Request && n->state != NATState_Refresh)
		{ LogMsg("uDNS_HandleNATPortMapReply (%##s): bad state %d", service, n->state);  return mDNSfalse; }

	if (!pkt && !deletion) // timeout
		{
#ifdef _LEGACY_NAT_TRAVERSAL_
		mDNSIPPort pub;
		int ntries = 0;
		mStatus err;
   		mDNSBool tcp = (n->op == NATOp_MapTCP);

		pub = n->PublicPort.NotAnInteger ? n->PublicPort : priv; // initially request priv == pub if PublicPort is zero
		while (1)
			{
			err = LNT_MapPort(priv, pub, tcp);
			if (!err)
				{
				n->PublicPort = pub;
				n->state = NATState_Legacy;
				goto end;
				}
			else if (err != mStatus_AlreadyRegistered || ++ntries > LEGACY_NATMAP_MAX_TRIES)
				{
				n->state = NATState_Error;
				goto end;
				}
			else
				{
				// the mapping we want is taken - try a random port
				mDNSu16 RandPort = mDNSRandom(DYN_PORT_MAX - DYN_PORT_MIN) + DYN_PORT_MIN;
				pub = mDNSOpaque16fromIntVal(RandPort);
				}
			}
#else
		goto end;
#endif // _LEGACY_NAT_TRAVERSAL_
		}

	if (len < sizeof(*reply)) { LogMsg("uDNS_HandleNATPortMapReply: response too short (%d bytes)", len); return mDNSfalse; }
	if (reply->vers != NATMAP_VERS) { LogMsg("uDNS_HandleNATPortMapReply: received version %d (expect version %d)", pkt[0], NATMAP_VERS);  return mDNSfalse; }
	if (reply->opcode != (n->op | NATMAP_RESPONSE_MASK)) { LogMsg("uDNS_HandleNATPortMapReply: bad response code %d", pkt[1]); return mDNSfalse; }
	if (reply->err.NotAnInteger) { LogMsg("uDNS_HandleNATPortMapReply: received error %d", mDNSVal16(reply->err));  return mDNSfalse; }
	if (priv.NotAnInteger != reply->priv.NotAnInteger) return mDNSfalse;  // packet does not match this request

	if (deletion) { n->state = NATState_Deleted; return mDNStrue; }

	n->TTL = (mDNSu32)mDNSVal32(reply->lease);
	if (n->TTL > 0x70000000UL / mDNSPlatformOneSecond) n->TTL = 0x70000000UL / mDNSPlatformOneSecond;

	if (n->state == NATState_Refresh && reply->pub.NotAnInteger != n->PublicPort.NotAnInteger)
		LogMsg("uDNS_HandleNATPortMapReply: NAT refresh changed public port from %d to %d", mDNSVal16(n->PublicPort), mDNSVal16(reply->pub));
        // this should never happen
	    // !!!KRS to be defensive, use SRVChanged flag on service and deregister here

	n->PublicPort = reply->pub;
	if (reply->pub.NotAnInteger != n->request.PortReq.pub.NotAnInteger) n->request.PortReq.pub = reply->pub; // set message buffer for refreshes

	n->retry = m->timenow + ((mDNSs32)n->TTL * mDNSPlatformOneSecond / 2);  // retry half way to expiration

	if (n->state == NATState_Refresh) { n->state = NATState_Established; return mDNStrue; }
	n->state = NATState_Established;

	end:
	if (n->state != NATState_Established && n->state != NATState_Legacy)
		{
		LogMsg("NAT Port Mapping (%##s): timeout", service);
		if (pkt) LogMsg("!!! timeout with non-null packet");
		n->state = NATState_Error;
		if (srs)
			{
			uDNS_HostnameInfo *hi = m->Hostnames;
			while (hi)
				{
				if (hi->arv6 && (hi->arv6->state == regState_Registered || hi->arv6->state == regState_Refresh)) break;
				else hi = hi->next;
				}

			if (hi)
				{
				debugf("Port map failed for service %##s - using IPv6 service target", service);
				srs->NATinfo = mDNSNULL;
				uDNS_FreeNATInfo(m, n);
				goto register_service;
				}
			else srs->state = regState_NATError;
			}
		else if (n->isLLQ) LLQNatMapComplete(m, n);
		return mDNStrue;
		}
	else LogOperation("Mapped private port %d to public port %d", mDNSVal16(priv), mDNSVal16(n->PublicPort));

	if (!srs) { if (n->isLLQ) LLQNatMapComplete(m, n); return mDNStrue; }

	register_service:
	if (srs->ns.ip.v4.NotAnInteger) SendServiceRegistration(m, srs);  // non-zero server address means we already have necessary zone data to send update
	else
		{
		srs->state = regState_FetchingZoneData;
		StartGetZoneData(m, srs->RR_SRV.resrec.name, lookupUpdateSRV, serviceRegistrationCallback, srs);
		}
	return mDNStrue;
	}

mDNSexport void uDNS_DeleteNATPortMapping(mDNS *m, NATTraversalInfo *nat)
	{
	if (nat->state == NATState_Established)  // let other edge-case states expire for simplicity
		{
		// zero lease
		nat->request.PortReq.lease.NotAnInteger = 0;
		nat->request.PortReq.pub = zeroIPPort;
		nat->state = NATState_Request;
		uDNS_SendNATMsg(nat, m);
		}
#ifdef _LEGACY_NAT_TRAVERSAL_
	else if (nat->state == NATState_Legacy)
		{
		mStatus err = mStatus_NoError;
   		mDNSBool tcp = (nat->op == NATOp_MapTCP);
		err = LNT_UnmapPort(nat->PublicPort, tcp);
		if (err) LogMsg("Legacy NAT Traversal - unmap request failed with error %ld", err);
		}
#endif // _LEGACY_NAT_TRAVERSAL_
	}

// if LLQ NAT context unreferenced, delete the mapping
mDNSlocal void CheckForUnreferencedLLQMapping(mDNS *m)
	{
	NATTraversalInfo * n = m->NATTraversals;
	DNSQuestion *q;

	while (n)
		{
		NATTraversalInfo *current = n;
		n = n->next;

		for (q = m->Questions; q; q = q->next)
			if (q->LongLived && q->llq && ((q->llq->NATInfoTCP == current) || (q->llq->NATInfoUDP == current))) break;

		if (!q && current->isLLQ && (--current->refs == 0))
			{
			//to avoid race condition if we need to recreate before this finishes, we do one-shot deregistration
			if (current->state == NATState_Established || current->state == NATState_Legacy)
				uDNS_DeleteNATPortMapping(m, current); // for simplicity we allow other states to expire
			uDNS_FreeNATInfo(m, current); // note: this clears the global LLQNatInfo pointer
			}
		}
	}

 // ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - host name and interface management
#endif

mDNSlocal void SendServiceDeregistration(mDNS *m, ServiceRecordSet *srs)
	{
	DNSMessage msg;
	mDNSOpaque16 id;
	mDNSu8 *ptr = msg.data;
	mDNSu8 *end = (mDNSu8 *)&msg + sizeof(DNSMessage);
	DomainAuthInfo * authInfo;
	mStatus err = mStatus_UnknownErr;
	mDNSu32 i;

	id = mDNS_NewMessageID(m);
	InitializeDNSMessage(&msg.h, id, UpdateReqFlags);

    // put zone
	ptr = putZone(&msg, ptr, end, &srs->zone, mDNSOpaque16fromIntVal(srs->RR_SRV.resrec.rrclass));
	if (!ptr) { LogMsg("ERROR: SendServiceDeregistration - putZone"); err = mStatus_UnknownErr; goto exit; }

	if (!(ptr = putDeleteAllRRSets(&msg, ptr, srs->RR_SRV.resrec.name))) { err = mStatus_UnknownErr; goto exit; } // this deletes SRV, TXT, and Extras
	if (!(ptr = putDeletionRecord(&msg, ptr, &srs->RR_PTR.resrec))) { err = mStatus_UnknownErr; goto exit; }
	for (i = 0; i < srs->NumSubTypes; i++)
		if (!(ptr = putDeletionRecord(&msg, ptr, &srs->SubTypes[i].resrec))) { err = mStatus_UnknownErr; goto exit; }

	srs->id    = id;
	srs->state = regState_DeregPending;
	authInfo     = GetAuthInfoForName(m, srs->RR_SRV.resrec.name);

	if (srs->Private)
		{
		tcpInfo_t		*	info;
		uDNS_TCPSocket		sock;
		mDNSIPPort			port = { { 0 } };

		info = (tcpInfo_t *)mDNSPlatformMemAllocate(sizeof(tcpInfo_t));

		if (!info)
			{
			LogMsg("ERROR: SendServiceDeregistration - memallocate failed");
			err = mStatus_UnknownErr;
			goto exit;
			}

		mDNSPlatformMemZero(info, sizeof(tcpInfo_t));
		info->request  = msg;
		info->m        = m;
		info->authInfo = authInfo;
		info->srs      = srs;
		info->requestLen = (int) (ptr - ((mDNSu8*) &msg));

		sock = mDNSPlatformTCPSocket(m, TCP_SOCKET_FLAGS, &port);

		if (!sock)
			{
			LogMsg("SendServiceDeregistration: unable to create TCP socket");
			mDNSPlatformMemFree(info);
			goto exit;
			}

		err = mDNSPlatformTCPConnect(sock, &srs->ns, srs->port, 0, tcpCallback, info);

		// manually invoke callback if connection completes

		if (err == mStatus_ConnEstablished)
			{
			tcpCallback(sock, info, mDNStrue, mStatus_NoError);
			err = mStatus_NoError;
			}
		else if (err == mStatus_ConnPending)
			{
			err = mStatus_NoError;
			}
		else
			{
			LogMsg("SendServiceDeregistration: connection failed");
			mDNSPlatformMemFree(info);
			goto exit;
			}
		}
	else
		{
		err = mDNSSendDNSMessage(m, &msg, ptr, mDNSInterface_Any, &srs->ns, srs->port, mDNSNULL, authInfo);
		if (err && err != mStatus_TransientErr) { debugf("ERROR: SendServiceDeregistration - mDNSSendDNSMessage - %ld", err); goto exit; }
		}

	SetRecordRetry(m, &srs->RR_SRV, err);

	err = mStatus_NoError;

exit:

	if (err)
		{
		unlinkSRS(m, srs);
		srs->state = regState_Unregistered;
		}
	}

mDNSlocal void UpdateSRV(mDNS *m, ServiceRecordSet *srs)
	{
	ExtraResourceRecord *e;

	// Target change if:
	// We have a target and were previously waiting for one, or
	// We had a target and no longer do, or
	// The target has changed

	domainname *curtarget = &srs->RR_SRV.resrec.rdata->u.srv.target;
	const domainname *const nt = GetServiceTarget(m, &srs->RR_SRV);
	const domainname *const newtarget = nt ? nt : (domainname*)"";
	mDNSBool TargetChanged = (newtarget->c[0] && srs->state == regState_NoTarget) || !SameDomainName(curtarget, newtarget);
	mDNSBool HaveZoneData = srs->ns.ip.v4.NotAnInteger ? mDNStrue : mDNSfalse;

	// Nat state change if:
	// We were behind a NAT, and now we are behind a new NAT, or
	// We're not behind a NAT but our port was previously mapped to a different public port
	// We were not behind a NAT and now we are

	NATTraversalInfo *nat = srs->NATinfo;
	mDNSIPPort port = srs->RR_SRV.resrec.rdata->u.srv.port;
	mDNSBool NATChanged = mDNSfalse;
	mDNSBool NowBehindNAT = port.NotAnInteger && IsPrivateV4Addr(&m->AdvertisedV4);
	mDNSBool WereBehindNAT = nat != mDNSNULL;
	mDNSBool NATRouterChanged = nat && nat->Router.ip.v4.NotAnInteger != m->Router.ip.v4.NotAnInteger;
	mDNSBool PortWasMapped = nat && (nat->state == NATState_Established || nat->state == NATState_Legacy) && nat->PublicPort.NotAnInteger != port.NotAnInteger;

	if (m->mDNS_busy != m->mDNS_reentrancy+1)
		LogMsg("UpdateSRV: Lock not held! mDNS_busy (%ld) mDNS_reentrancy (%ld)", m->mDNS_busy, m->mDNS_reentrancy);

	if (WereBehindNAT && NowBehindNAT && NATRouterChanged) NATChanged = mDNStrue;
	else if (!NowBehindNAT && PortWasMapped)               NATChanged = mDNStrue;
	else if (!WereBehindNAT && NowBehindNAT)               NATChanged = mDNStrue;

	if (!TargetChanged && !NATChanged) return;

	debugf("UpdateSRV (%##s) HadZoneData=%d, TargetChanged=%d, newtarget=%##s, NowBehindNAT=%d, WereBehindNAT=%d, NATRouterChanged=%d, PortWasMapped=%d",
		   srs->RR_SRV.resrec.name->c,  HaveZoneData, TargetChanged, newtarget, NowBehindNAT, WereBehindNAT, NATRouterChanged, PortWasMapped);

	switch(srs->state)
		{
		case regState_FetchingZoneData:
		case regState_Cancelled:
		case regState_DeregPending:
		case regState_DeregDeferred:
		case regState_Unregistered:
		case regState_NATMap:
		case regState_ExtraQueued:
			// In these states, the SRV has either not yet been registered (it will get up-to-date information when it is)
			// or is in the process of, or has already been, deregistered
			return;

		case regState_Pending:
		case regState_Refresh:
		case regState_UpdatePending:
			// let the in-flight operation complete before updating
			srs->SRVUpdateDeferred = mDNStrue;
			return;

		case regState_NATError:
			if (!NATChanged) return;
			// if nat changed, register if we have a target (below)

		case regState_NoTarget:
			if (newtarget->c[0])
				{
				debugf("UpdateSRV: %s service %##s", HaveZoneData ? (NATChanged && NowBehindNAT ? "Starting Port Map for" : "Registering") : "Getting Zone Data for", srs->RR_SRV.resrec.name->c);
				if (!HaveZoneData)
					{
					srs->state = regState_FetchingZoneData;
					StartGetZoneData(m, srs->RR_SRV.resrec.name, lookupUpdateSRV, serviceRegistrationCallback, srs);
					}
				else
					{
					if (nat && (NATChanged || !NowBehindNAT)) { srs->NATinfo = mDNSNULL; uDNS_FreeNATInfo(m, nat); }
					if (NATChanged && NowBehindNAT) { srs->state = regState_NATMap; StartNATPortMap(m, srs); }
					else SendServiceRegistration(m, srs);
					}
				}
			return;

		case regState_Registered:
			// target or nat changed.  deregister service.  upon completion, we'll look for a new target
			debugf("UpdateSRV: SRV record changed for service %##s - deregistering (will re-register with new SRV)",  srs->RR_SRV.resrec.name->c);
			for (e = srs->Extras; e; e = e->next) e->r.state = regState_ExtraQueued;  // extra will be re-registed if the service is re-registered
			srs->SRVChanged = mDNStrue;
			SendServiceDeregistration(m, srs);
			return;
		
		default: LogMsg("UpdateSRV: Unknown state %d for %##s", srs->state, srs->RR_SRV.resrec.name->c);
		}
	}

mDNSlocal void UpdateSRVRecords(mDNS *m)
	{
	ServiceRecordSet *srs;
	for (srs = m->ServiceRegistrations; srs; srs = srs->next) UpdateSRV(m, srs);
	}

mDNSlocal void HostnameCallback(mDNS *const m, AuthRecord *const rr, mStatus result)
	{
	uDNS_HostnameInfo *hi = (uDNS_HostnameInfo *)rr->RecordContext;

	if (result == mStatus_MemFree)
		{
		if (hi)
			{
			if      (hi->arv4 == rr)                hi->arv4 = mDNSNULL;
			else if (hi->arv4 == rr)                hi->arv6 = mDNSNULL;
			rr->RecordContext = mDNSNULL;
			if (!hi->arv4 && !hi->arv6) mDNSPlatformMemFree(hi);  // free hi when both v4 and v6 AuthRecs deallocated
			}

		mDNSPlatformMemFree(rr);
		return;
		}

	if (result)
		{
		// don't unlink or free - we can retry when we get a new address/router
		if (rr->resrec.rrtype == kDNSType_A)
			LogMsg("HostnameCallback: Error %ld for registration of %##s IP %.4a", result, rr->resrec.name->c, &rr->resrec.rdata->u.ipv4);
		else
			LogMsg("HostnameCallback: Error %ld for registration of %##s IP %.16a", result, rr->resrec.name->c, &rr->resrec.rdata->u.ipv6);
		if (!hi) { mDNSPlatformMemFree(rr); return; }
		if (rr->state != regState_Unregistered) LogMsg("Error: HostnameCallback invoked with error code for record not in regState_Unregistered!");

		if ((!hi->arv4 || hi->arv4->state == regState_Unregistered) &&
			(!hi->arv6 || hi->arv6->state == regState_Unregistered))
			{
			// only deliver status if both v4 and v6 fail
			rr->RecordContext = (void *)hi->StatusContext;
			if (hi->StatusCallback)
				hi->StatusCallback(m, rr, result); // client may NOT make API calls here
			rr->RecordContext = (void *)hi;
			}
		return;
		}
	// register any pending services that require a target
	mDNS_Lock(m);
	UpdateSRVRecords(m);
	mDNS_Unlock(m);

	// Deliver success to client
	if (!hi) { LogMsg("HostnameCallback invoked with orphaned address record"); return; }
	if (rr->resrec.rrtype == kDNSType_A)
		LogMsg("Registered hostname %##s IP %.4a", rr->resrec.name->c, &rr->resrec.rdata->u.ipv4);
	else
		LogMsg("Registered hostname %##s IP %.16a", rr->resrec.name->c, &rr->resrec.rdata->u.ipv6);

	rr->RecordContext = (void *)hi->StatusContext;
	if (hi->StatusCallback)
		hi->StatusCallback(m, rr, result); // client may NOT make API calls here
	rr->RecordContext = (void *)hi;
	}

// register record or begin NAT traversal
mDNSlocal void AdvertiseHostname(mDNS *m, uDNS_HostnameInfo *h)
	{
	if (m->AdvertisedV4.ip.v4.NotAnInteger && h->arv4->state == regState_Unregistered)
		{
		if (IsPrivateV4Addr(&m->AdvertisedV4))
			StartGetPublicAddr(m, h->arv4);
		else
			{
			LogMsg("Advertising %##s IP %.4a", h->arv4->resrec.name->c, &m->AdvertisedV4.ip.v4);
			mDNS_Register_internal(m, h->arv4);
			}
		}
	if (m->AdvertisedV6.ip.v6.b[0] && h->arv6->state == regState_Unregistered)
		{
		LogMsg("Advertising %##s IP %.16a", h->arv4->resrec.name->c, &m->AdvertisedV6.ip.v6);
		mDNS_Register_internal(m, h->arv6);
		}
	}

mDNSlocal void FoundStaticHostname(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, mDNSBool AddRecord)
	{
	const domainname *pktname = &answer->rdata->u.name;
	domainname *storedname = &m->StaticHostname;
	uDNS_HostnameInfo *h = m->Hostnames;

	(void)question;

	debugf("FoundStaticHostname: %##s -> %##s (%s)", question->qname.c, answer->rdata->u.name.c, AddRecord ? "added" : "removed");
	if (AddRecord && !SameDomainName(pktname, storedname))
		{
		AssignDomainName(storedname, pktname);
		while (h)
			{
			if ((h->arv4 && (h->arv4->state == regState_FetchingZoneData || h->arv4->state == regState_Pending || h->arv4->state == regState_NATMap)) ||
				(h->arv6 && (h->arv6->state == regState_FetchingZoneData || h->arv6->state == regState_Pending)))
				{
				// if we're in the process of registering a dynamic hostname, delay SRV update so we don't have to reregister services if the dynamic name succeeds
				m->DelaySRVUpdate = mDNStrue;
				m->NextSRVUpdate = m->timenow + (5 * mDNSPlatformOneSecond);
				return;
				}
			h = h->next;
			}
		UpdateSRVRecords(m);
		}
	else if (!AddRecord && SameDomainName(pktname, storedname))
		{
		storedname->c[0] = 0;
		UpdateSRVRecords(m);
		}
	}

mDNSlocal void GetStaticHostname(mDNS *m)
	{
	char buf[MAX_ESCAPED_DOMAIN_NAME];
	DNSQuestion *q = &m->ReverseMap;
	mDNSu8 *ip = m->AdvertisedV4.ip.v4.b;
	mStatus err;

	if (m->ReverseMapActive)
		{
		// Seems like a hack -- if this code is allowed to issue API calls, then it should have been called in the appropriate state in the first place
		// and if it's not allowed to issue API calls, then deliberately circumventing the locking check like this seems like it's asking for
		// tricky hard-to-diagnose crashing bugs
		mDNS_DropLockBeforeCallback();
		mDNS_StopQuery(m, q);
		mDNS_ReclaimLockAfterCallback();
		m->ReverseMapActive = mDNSfalse;
		}

	m->StaticHostname.c[0] = 0;
	if (!m->AdvertisedV4.ip.v4.NotAnInteger) return;
	mDNSPlatformMemZero(q, sizeof(*q));
	mDNS_snprintf(buf, MAX_ESCAPED_DOMAIN_NAME, "%d.%d.%d.%d.in-addr.arpa.", ip[3], ip[2], ip[1], ip[0]);
    if (!MakeDomainNameFromDNSNameString(&q->qname, buf)) { LogMsg("Error: GetStaticHostname - bad name %s", buf); return; }

	q->InterfaceID      = mDNSInterface_Any;
    q->Target           = zeroAddr;
    q->qtype            = kDNSType_PTR;
    q->qclass           = kDNSClass_IN;
    q->LongLived        = mDNSfalse;
    q->ExpectUnique     = mDNSfalse;
    q->ForceMCast       = mDNSfalse;
    q->ReturnIntermed   = mDNSfalse;
    q->QuestionCallback = FoundStaticHostname;
    q->QuestionContext  = mDNSNULL;

	// Seems like a hack -- if this code is allowed to issue API calls, then it should have been called in the appropriate state in the first place
	// and if it's not allowed to issue API calls, then deliberately circumventing the locking check like this seems like it's asking for
	// tricky hard-to-diagnose crashing bugs
	mDNS_DropLockBeforeCallback();
	err = mDNS_StartQuery(m, q);
	mDNS_ReclaimLockAfterCallback();
	if (err) LogMsg("Error: GetStaticHostname - StartQuery returned error %d", err);
	else m->ReverseMapActive = mDNStrue;
	}

mDNSlocal void AssignHostnameInfoAuthRecord(mDNS *m, uDNS_HostnameInfo *hi, int type)
	{
	AuthRecord **dst = (type == mDNSAddrType_IPv4 ? &hi->arv4 : &hi->arv6);
	AuthRecord *ar =  mDNSPlatformMemAllocate(sizeof(*ar));

	if (type != mDNSAddrType_IPv4 && type != mDNSAddrType_IPv6) { LogMsg("ERROR: AssignHostnameInfoAuthRecord - bad type %d", type); return; }
	if (!ar) { LogMsg("ERROR: AssignHostnameInfoAuthRecord - malloc"); return; }

	mDNS_SetupResourceRecord(ar, mDNSNULL, 0, (mDNSu16) (type == mDNSAddrType_IPv4 ? kDNSType_A : kDNSType_AAAA),  1, kDNSRecordTypeKnownUnique, HostnameCallback, hi);
	AssignDomainName(ar->resrec.name, &hi->fqdn);

	// only set RData if we have a valid IP
	if (type == mDNSAddrType_IPv4 && m->AdvertisedV4.ip.v4.NotAnInteger)
		{
		if (m->MappedV4.ip.v4.NotAnInteger) ar->resrec.rdata->u.ipv4 = m->MappedV4.ip.v4;
		else                                ar->resrec.rdata->u.ipv4 = m->AdvertisedV4.ip.v4;
		}
	else if (type == mDNSAddrType_IPv6 && m->AdvertisedV6.ip.v6.b[0])
		{
		ar->resrec.rdata->u.ipv6 = m->AdvertisedV6.ip.v6;
		}

	ar->state = regState_Unregistered;

	if (*dst)
		{
		LogMsg("ERROR: AssignHostnameInfoAuthRecord - overwriting %s AuthRec", type == mDNSAddrType_IPv4 ? "IPv4" : "IPv6");
		unlinkAR(&m->RecordRegistrations, *dst);
		(*dst)->RecordContext = mDNSNULL;  // defensively clear backpointer to avoid doubly-referenced context
		}

	*dst = ar;
	}

// Deregister hostnames and register new names for each host domain with the current global
// values for the hostlabel and primary IP address
mDNSlocal void UpdateHostnameRegistrations(mDNS *m)
	{
	uDNS_HostnameInfo *i;

	for (i = m->Hostnames; i; i = i->next)
		{
		if (i->arv4 && i->arv4->state != regState_Unregistered &&
			i->arv4->resrec.rdata->u.ipv4.NotAnInteger != m->AdvertisedV4.ip.v4.NotAnInteger &&
			i->arv4->resrec.rdata->u.ipv4.NotAnInteger !=m->MappedV4.ip.v4.NotAnInteger)
			{
			uDNS_DeregisterRecord(m, i->arv4);
			i->arv4 = mDNSNULL;
			}
		if (i->arv6 && !mDNSPlatformMemSame(i->arv6->resrec.rdata->u.ipv6.b, m->AdvertisedV6.ip.v6.b, 16) && i->arv6->state != regState_Unregistered)
			{
			uDNS_DeregisterRecord(m, i->arv6);
			i->arv6 = mDNSNULL;
			}

		if (!i->arv4 && m->AdvertisedV4.ip.v4.NotAnInteger)                    AssignHostnameInfoAuthRecord(m, i, mDNSAddrType_IPv4);
		else if (i->arv4 && i->arv4->state == regState_Unregistered) i->arv4->resrec.rdata->u.ipv4 = m->AdvertisedV4.ip.v4;  // simply overwrite unregistered
		if (!i->arv6 && m->AdvertisedV6.ip.v6.b[0])                            AssignHostnameInfoAuthRecord(m, i, mDNSAddrType_IPv6);
		else if (i->arv6 &&i->arv6->state == regState_Unregistered)  i->arv6->resrec.rdata->u.ipv6 = m->AdvertisedV6.ip.v6;

		AdvertiseHostname(m, i);
		}
	}

mDNSexport void mDNS_AddDynDNSHostName(mDNS *m, const domainname *fqdn, mDNSRecordCallback *StatusCallback, const void *StatusContext)
   {
	uDNS_HostnameInfo *ptr, *new;

	mDNS_Lock(m);

	// check if domain already registered
	for (ptr = m->Hostnames; ptr; ptr = ptr->next)
		{
		if (SameDomainName(fqdn, &ptr->fqdn))
			{ LogMsg("Host Domain %##s already in list", fqdn->c); goto exit; }
		}

	// allocate and format new address record
	new = mDNSPlatformMemAllocate(sizeof(*new));
	if (!new) { LogMsg("ERROR: mDNS_AddDynDNSHostname - malloc"); goto exit; }
	mDNSPlatformMemZero(new, sizeof(*new));
    new->next = m->Hostnames;
	m->Hostnames = new;

	AssignDomainName(&new->fqdn, fqdn);
	new->StatusCallback = StatusCallback;
	new->StatusContext = StatusContext;

	if (m->AdvertisedV4.ip.v4.NotAnInteger) AssignHostnameInfoAuthRecord(m, new, mDNSAddrType_IPv4);
	else new->arv4 = mDNSNULL;
	if (m->AdvertisedV6.ip.v6.b[0])         AssignHostnameInfoAuthRecord(m, new, mDNSAddrType_IPv6);
	else new->arv6 = mDNSNULL;

	if (m->AdvertisedV6.ip.v6.b[0] || m->AdvertisedV4.ip.v4.NotAnInteger) AdvertiseHostname(m, new);

exit:
	mDNS_Unlock(m);
	}

mDNSexport void mDNS_RemoveDynDNSHostName(mDNS *m, const domainname *fqdn)
	{
	uDNS_HostnameInfo **ptr = &m->Hostnames;

	mDNS_Lock(m);

	while (*ptr && !SameDomainName(fqdn, &(*ptr)->fqdn)) ptr = &(*ptr)->next;
	if (!*ptr) LogMsg("mDNS_RemoveDynDNSHostName: no such domainname %##s", fqdn->c);
	else
		{
		uDNS_HostnameInfo *hi = *ptr;
		*ptr = (*ptr)->next; // unlink
		if (hi->arv4)
			{
			hi->arv4->RecordContext = mDNSNULL; // about to free wrapper struct
			if (hi->arv4->state != regState_Unregistered) uDNS_DeregisterRecord(m, hi->arv4);
			else { mDNSPlatformMemFree(hi->arv4); hi->arv4 = mDNSNULL; }
			}
		if (hi->arv6)
			{
			hi->arv6->RecordContext = mDNSNULL; // about to free wrapper struct
			if (hi->arv6->state != regState_Unregistered) uDNS_DeregisterRecord(m, hi->arv6);
			else { mDNSPlatformMemFree(hi->arv6); hi->arv6 = mDNSNULL; }
			}
		mDNSPlatformMemFree(hi);
		}
	UpdateSRVRecords(m);
	mDNS_Unlock(m);
	}

mDNSexport void mDNS_SetPrimaryInterfaceInfo(mDNS *m, const mDNSAddr *v4addr, const mDNSAddr *v6addr, const mDNSAddr *router)
	{
	mDNSBool v4Changed, v6Changed, RouterChanged;

	if (v4addr && v4addr->type != mDNSAddrType_IPv4) { LogMsg("mDNS_SetPrimaryInterfaceInfo V4 address - incorrect type.  Discarding."); return; }
	if (v6addr && v6addr->type != mDNSAddrType_IPv6) { LogMsg("mDNS_SetPrimaryInterfaceInfo V6 address - incorrect type.  Discarding."); return; }
	if (router && router->type != mDNSAddrType_IPv4) { LogMsg("mDNS_SetPrimaryInterfaceInfo passed non-V4 router.  Discarding."); return; }

	mDNS_Lock(m);

	v4Changed   = (v4addr ? v4addr->ip.v4.NotAnInteger : 0) != m->AdvertisedV4.ip.v4.NotAnInteger;
	v6Changed   = v6addr ? !mDNSPlatformMemSame(v6addr, &m->AdvertisedV6, sizeof(*v6addr)) : (m->AdvertisedV6.ip.v6.b[0] != 0);
	RouterChanged = (router ? router->ip.v4.NotAnInteger : 0) != m->Router.ip.v4.NotAnInteger;

#if MDNS_DEBUGMSGS
	if (v4addr && (v4Changed || RouterChanged))
		LogMsg("mDNS_SetPrimaryInterfaceInfo: address changed from %d.%d.%d.%d to %d.%d.%d.%d:%d",
			   m->AdvertisedV4.ip.v4.b[0], m->AdvertisedV4.ip.v4.b[1], m->AdvertisedV4.ip.v4.b[2], m->AdvertisedV4.ip.v4.b[3],
			   v4addr->ip.v4.b[0], v4addr->ip.v4.b[1], v4addr->ip.v4.b[2], v4addr->ip.v4.b[3]);
#endif // MDNS_DEBUGMSGS

	if ((v4Changed || RouterChanged) && m->MappedV4.ip.v4.NotAnInteger) m->MappedV4.ip.v4 = zerov4Addr;
	if (v4addr) m->AdvertisedV4 = *v4addr;  else m->AdvertisedV4.ip.v4 = zerov4Addr;
	if (v6addr) m->AdvertisedV6 = *v6addr;  else m->AdvertisedV6.ip.v6 = zerov6Addr;
	if (router) m->Router       = *router;  else m->Router.ip.v4 = zerov4Addr;
	// setting router to zero indicates that nat mappings must be reestablished when router is reset

	if ((v4Changed || RouterChanged || v6Changed) && v4addr)
		{
		// don't update these unless we've got V4
		UpdateHostnameRegistrations(m);
		UpdateSRVRecords(m);
		GetStaticHostname(m);  // look up reverse map record to find any static hostnames for our IP address
		}

	mDNS_Unlock(m);
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Incoming Message Processing
#endif

mDNSlocal mStatus ParseTSIGError(mDNS *m, const DNSMessage *msg, const mDNSu8 *end, const domainname *displayname)
	{
	LargeCacheRecord lcr;
	const mDNSu8 *ptr;
	mStatus err = mStatus_NoError;
	int i;

	ptr = LocateAdditionals(msg, end);
	if (!ptr) goto finish;

	for (i = 0; i < msg->h.numAdditionals; i++)
		{
		ptr = GetLargeResourceRecord(m, msg, ptr, end, 0, kDNSRecordTypePacketAdd, &lcr);
		if (!ptr) goto finish;
		if (lcr.r.resrec.rrtype == kDNSType_TSIG)
			{
			mDNSu32 macsize;
			mDNSu8 *rd = lcr.r.resrec.rdata->u.data;
			mDNSu8 *rdend = rd + lcr.r.resrec.rdlength;
			int alglen = DomainNameLengthLimit(&lcr.r.resrec.rdata->u.name, rdend);
			if (alglen > MAX_DOMAIN_NAME) goto finish;
			rd += alglen;                                       // algorithm name
			if (rd + 6 > rdend) goto finish;
			rd += 6;                                            // 48-bit timestamp
			if (rd + sizeof(mDNSOpaque16) > rdend) goto finish;
			rd += sizeof(mDNSOpaque16);                         // fudge
			if (rd + sizeof(mDNSOpaque16) > rdend) goto finish;
			macsize = mDNSVal16(*(mDNSOpaque16 *)rd);
			rd += sizeof(mDNSOpaque16);                         // MAC size
			if (rd + macsize > rdend) goto finish;
			rd += macsize;
			if (rd + sizeof(mDNSOpaque16) > rdend) goto finish;
			rd += sizeof(mDNSOpaque16);                         // orig id
			if (rd + sizeof(mDNSOpaque16) > rdend) goto finish;
			err = mDNSVal16(*(mDNSOpaque16 *)rd);               // error code
	
			if      (err == TSIG_ErrBadSig) { LogMsg("%##s: bad signature", displayname->c);              err = mStatus_BadSig;     }
			else if (err == TSIG_ErrBadKey) { LogMsg("%##s: bad key", displayname->c);                    err = mStatus_BadKey;     }
			else if (err == TSIG_ErrBadTime) { LogMsg("%##s: bad time", displayname->c);                   err = mStatus_BadTime;    }
			else if (err)                    { LogMsg("%##s: unknown tsig error %d", displayname->c, err); err = mStatus_UnknownErr; }
			goto finish;
			}
		}

	finish:
	return err;
	}

mDNSlocal mStatus checkUpdateResult(domainname *displayname, mDNSu8 rcode, mDNS *m, const DNSMessage *msg, const mDNSu8 *end)
	{
	(void)msg;  // currently unused, needed for TSIG errors
	if (!rcode) return mStatus_NoError;
	else if (rcode == kDNSFlag1_RC_YXDomain)
		{
		debugf("name in use: %##s", displayname->c);
		return mStatus_NameConflict;
		}
	else if (rcode == kDNSFlag1_RC_Refused)
		{
		LogMsg("Update %##s refused", displayname->c);
		return mStatus_Refused;
		}
	else if (rcode == kDNSFlag1_RC_NXRRSet)
		{
		LogMsg("Reregister refused (NXRRSET): %##s", displayname->c);
		return mStatus_NoSuchRecord;
		}
	else if (rcode == kDNSFlag1_RC_NotAuth)
		{
		// TSIG errors should come with FmtErr as per RFC 2845, but BIND 9 sends them with NotAuth so we look here too
		mStatus tsigerr = ParseTSIGError(m, msg, end, displayname);
		if (!tsigerr)
			{
			LogMsg("Permission denied (NOAUTH): %##s", displayname->c);
			return mStatus_UnknownErr;
			}
		else return tsigerr;
		}
	else if (rcode == kDNSFlag1_RC_FmtErr)
		{
		mStatus tsigerr = ParseTSIGError(m, msg, end, displayname);
		if (!tsigerr)
			{
			LogMsg("Format Error: %##s", displayname->c);
			return mStatus_UnknownErr;
			}
		else return tsigerr;
		}
	else
		{
		LogMsg("Update %##s failed with rcode %d", displayname->c, rcode);
		return mStatus_UnknownErr;
		}
	}

mDNSlocal void sendRecordRegistration(mDNS *const m, AuthRecord *rr)
	{
	DNSMessage msg;
	mDNSu8 *ptr = msg.data;
	mDNSu8 *end = (mDNSu8 *)&msg + sizeof(DNSMessage);
	DomainAuthInfo * authInfo;
	mStatus err = mStatus_UnknownErr;

	rr->id = mDNS_NewMessageID(m);
	InitializeDNSMessage(&msg.h, rr->id, UpdateReqFlags);

    // set zone
	ptr = putZone(&msg, ptr, end, &rr->zone, mDNSOpaque16fromIntVal(rr->resrec.rrclass));
	if (!ptr) { err = mStatus_UnknownErr; goto exit; }

	if (rr->state == regState_UpdatePending)
		{
		// delete old RData
		SetNewRData(&rr->resrec, rr->OrigRData, rr->OrigRDLen);
		if (!(ptr = putDeletionRecord(&msg, ptr, &rr->resrec))) { err = mStatus_UnknownErr; goto exit; } // delete old rdata

		// add new RData
		SetNewRData(&rr->resrec, rr->InFlightRData, rr->InFlightRDLen);
		if (!(ptr = PutResourceRecordTTLJumbo(&msg, ptr, &msg.h.mDNS_numUpdates, &rr->resrec, rr->resrec.rroriginalttl))) { err = mStatus_UnknownErr; goto exit; }
		}

	else
		{
		if (rr->resrec.RecordType == kDNSRecordTypeKnownUnique)
			{
			// KnownUnique: Delete any previous value
			ptr = putDeleteRRSet(&msg, ptr, rr->resrec.name, rr->resrec.rrtype);
			if (!ptr) { err = mStatus_UnknownErr; goto exit; }
			}

		else if (rr->resrec.RecordType != kDNSRecordTypeShared)
			{
			ptr = putPrereqNameNotInUse(rr->resrec.name, &msg, ptr, end);
			if (!ptr) { err = mStatus_UnknownErr; goto exit; }
			}

		ptr = PutResourceRecordTTLJumbo(&msg, ptr, &msg.h.mDNS_numUpdates, &rr->resrec, rr->resrec.rroriginalttl);
		if (!ptr) { err = mStatus_UnknownErr; goto exit; }
		}

	if (rr->lease)
		{ ptr = putUpdateLease(&msg, ptr, DEFAULT_UPDATE_LEASE); if (!ptr) { err = mStatus_UnknownErr; goto exit; } }

	authInfo = GetAuthInfoForName(m, rr->resrec.name);

	if (rr->Private)
		{
		tcpInfo_t	*	info;
		uDNS_TCPSocket	sock;
		mDNSIPPort		port = { { 0 } };

		info = (tcpInfo_t *)mDNSPlatformMemAllocate(sizeof(tcpInfo_t));

		if (!info)
			{
			LogMsg("ERROR: sendRecordRegistration - memallocate failed");
			err = mStatus_UnknownErr;
			goto exit;
			}

		mDNSPlatformMemZero(info, sizeof(tcpInfo_t));

		info->m          = m;
		info->authInfo   = authInfo;
		info->rr         = rr;
		info->request    = msg;
		info->requestLen = (int) (ptr - ((mDNSu8*) &msg));

		sock = mDNSPlatformTCPSocket(m, TCP_SOCKET_FLAGS, &port);

		if (!sock)
			{
			LogMsg("sendRecordRegistration: unable to create TCP socket");
			mDNSPlatformMemFree(info);
			goto exit;
			}

		err = mDNSPlatformTCPConnect(sock, &rr->UpdateServer, rr->UpdatePort, 0, tcpCallback, info);

		if (err == mStatus_ConnEstablished)
			{
			// manually invoke callback if connection completes
			tcpCallback(sock, info, mDNStrue, mStatus_NoError);
			err = mStatus_NoError;
			}
		else if (err == mStatus_ConnPending)
			{
			// callback will be automatically invoked when connection completes
			err = mStatus_NoError;
			}
		else
			{
			LogMsg("sendRecordRegistration: connection failed");
			mDNSPlatformMemFree(info);
			goto exit;
			}
		}
	else
		{
		err = mDNSSendDNSMessage(m, &msg, ptr, mDNSInterface_Any, &rr->UpdateServer, rr->UpdatePort, mDNSNULL, authInfo);
		if (err) debugf("ERROR: sendRecordRegistration - mDNSSendDNSMessage - %ld", err);
		}

	SetRecordRetry(m, rr, err);

	if (rr->state != regState_Refresh && rr->state != regState_DeregDeferred && rr->state != regState_UpdatePending)
		rr->state = regState_Pending;

	err = mStatus_NoError;

exit:

	if (err)
		{
		LogMsg("sendRecordRegistration: Error formatting message");

		if (rr->state != regState_Unregistered)
			{
			unlinkAR(&m->RecordRegistrations, rr);
			rr->state = regState_Unregistered;
			}

		mDNS_DropLockBeforeCallback();
		if (rr->RecordCallback) rr->RecordCallback(m, rr, err);
		mDNS_ReclaimLockAfterCallback();
		// NOTE: not safe to touch any client structures here
		}
	}

mDNSlocal void hndlServiceUpdateReply(mDNS * const m, ServiceRecordSet *srs,  mStatus err)
	{
	mDNSBool InvokeCallback = mDNSfalse;
	NATTraversalInfo *nat = srs->NATinfo;
	ExtraResourceRecord **e = &srs->Extras;
	AuthRecord *txt = &srs->RR_TXT;
	switch (srs->state)
		{
		case regState_Pending:
			if (err == mStatus_NameConflict && !srs->TestForSelfConflict)
				{
				srs->TestForSelfConflict = mDNStrue;
				debugf("checking for self-conflict of service %##s", srs->RR_SRV.resrec.name->c);
				SendServiceRegistration(m, srs);
				return;
				}
			else if (srs->TestForSelfConflict)
				{
				srs->TestForSelfConflict = mDNSfalse;
				if (err == mStatus_NoSuchRecord) err = mStatus_NameConflict;  // NoSuchRecord implies that our prereq was not met, so we actually have a name conflict
				if (err) srs->state = regState_Unregistered;
				else srs->state = regState_Registered;
				InvokeCallback = mDNStrue;
				break;
				}
			else if (err == mStatus_UnknownErr && srs->lease)
				{
				LogMsg("Re-trying update of service %##s without lease option", srs->RR_SRV.resrec.name->c);
				srs->lease = mDNSfalse;
				SendServiceRegistration(m, srs);
				return;
				}
			else
				{
				if (err) { LogMsg("Error %ld for registration of service %##s", err, srs->RR_SRV.resrec.name->c); srs->state = regState_Unregistered; } //!!!KRS make sure all structs will still get cleaned up when client calls DeregisterService with this state
				else srs->state = regState_Registered;
				InvokeCallback = mDNStrue;
				break;
				}
		case regState_Refresh:
			if (err)
				{
				LogMsg("Error %ld for refresh of service %##s", err, srs->RR_SRV.resrec.name->c);
				InvokeCallback = mDNStrue;
				srs->state = regState_Unregistered;
				}
			else srs->state = regState_Registered;
			break;
		case regState_DeregPending:
			if (err) LogMsg("Error %ld for deregistration of service %##s", err, srs->RR_SRV.resrec.name->c);
			if (srs->SRVChanged)
				{
				srs->state = regState_NoTarget;  // NoTarget will allow us to pick up new target OR nat traversal state
				break;
				}
			err = mStatus_MemFree;
			InvokeCallback = mDNStrue;
			if (nat)
				{
				if (nat->state == NATState_Deleted) { srs->NATinfo = mDNSNULL; uDNS_FreeNATInfo(m, nat); } // deletion copmleted
				else nat->reg.ServiceRegistration = mDNSNULL;  // allow mapping deletion to continue
				}
			srs->state = regState_Unregistered;
			break;
		case regState_DeregDeferred:
			if (err)
				{
				debugf("Error %ld received prior to deferred derigstration of %##s", err, srs->RR_SRV.resrec.name->c);
				err = mStatus_MemFree;
				InvokeCallback = mDNStrue;
				srs->state = regState_Unregistered;
				break;
				}
			else
				{
				debugf("Performing deferred deregistration of %##s", srs->RR_SRV.resrec.name->c);
				srs->state = regState_Registered;
				SendServiceDeregistration(m, srs);
				return;
				}
		case regState_UpdatePending:
			if (err)
				{
				LogMsg("hndlServiceUpdateReply: error updating TXT record for service %##s", srs->RR_SRV.resrec.name->c);
				srs->state = regState_Unregistered;
				InvokeCallback = mDNStrue;
				}
			else
				{
				srs->state = regState_Registered;
				// deallocate old RData
				if (txt->UpdateCallback) txt->UpdateCallback(m, txt, txt->OrigRData);
				SetNewRData(&txt->resrec, txt->InFlightRData, txt->InFlightRDLen);
				txt->OrigRData = mDNSNULL;
				txt->InFlightRData = mDNSNULL;
				}
			break;
		case regState_FetchingZoneData:
		case regState_Registered:
		case regState_Cancelled:
		case regState_Unregistered:
		case regState_NATMap:
		case regState_NoTarget:
		case regState_ExtraQueued:
		case regState_NATError:
			LogMsg("hndlServiceUpdateReply called for service %##s in unexpected state %d with error %ld.  Unlinking.",
				   srs->RR_SRV.resrec.name->c, srs->state, err);
			err = mStatus_UnknownErr;
		default: LogMsg("hndlServiceUpdateReply: Unknown state %d for %##s", srs->state, srs->RR_SRV.resrec.name->c);
		}

	if ((srs->SRVChanged || srs->SRVUpdateDeferred) && (srs->state == regState_NoTarget || srs->state == regState_Registered))
		{
		if (InvokeCallback)
			{
			srs->ClientCallbackDeferred = mDNStrue;
			srs->DeferredStatus = err;
			}
		srs->SRVChanged = mDNSfalse;
		UpdateSRV(m, srs);
		return;
		}

	while (*e)
		{
		if ((*e)->r.state == regState_ExtraQueued)
			{
			if (srs->state == regState_Registered && !err)
				{
				// extra resource record queued for this service - copy zone srs and register
				AssignDomainName(&(*e)->r.zone, &srs->zone);
				(*e)->r.UpdateServer    = srs->ns;
				(*e)->r.UpdatePort  = srs->port;
				(*e)->r.lease = srs->lease;
				sendRecordRegistration(m, &(*e)->r);
				e = &(*e)->next;
				}
			else if (err && (*e)->r.state != regState_Unregistered)
				{
				// unlink extra from list
				(*e)->r.state = regState_Unregistered;
				*e = (*e)->next;
				}
			else e = &(*e)->next;
			}
		else e = &(*e)->next;
		}

	srs->RR_SRV.ThisAPInterval = INIT_UCAST_POLL_INTERVAL - 1;  // reset retry delay for future refreshes, dereg, etc.
	if (srs->state == regState_Unregistered) unlinkSRS(m, srs);
	else if (txt->QueuedRData && srs->state == regState_Registered)
		{
		if (InvokeCallback)
			{
			// if we were supposed to give a client callback, we'll do it after we update the primary txt record
			srs->ClientCallbackDeferred = mDNStrue;
			srs->DeferredStatus = err;
			}
		srs->state = regState_UpdatePending;
		txt->InFlightRData = txt->QueuedRData;
		txt->InFlightRDLen = txt->QueuedRDLen;
		txt->OrigRData = txt->resrec.rdata;
		txt->OrigRDLen = txt->resrec.rdlength;
		txt->QueuedRData = mDNSNULL;
		SendServiceRegistration(m, srs);
		return;
		}

	mDNS_DropLockBeforeCallback();
	if (InvokeCallback) srs->ServiceCallback(m, srs, err);
	else if (srs->ClientCallbackDeferred)
		{
		srs->ClientCallbackDeferred = mDNSfalse;
		srs->ServiceCallback(m, srs, srs->DeferredStatus);
		}
	mDNS_ReclaimLockAfterCallback();
	// NOTE: do not touch structures after calling ServiceCallback
	}

mDNSlocal void hndlRecordUpdateReply(mDNS *m, AuthRecord *rr, mStatus err)
	{
	mDNSBool InvokeCallback = mDNStrue;

	if (rr->state == regState_UpdatePending)
		{
		if (err)
			{
			LogMsg("Update record failed for %##s (err %d)", rr->resrec.name->c, err);
			rr->state = regState_Unregistered;
			}
		else
			{
			debugf("Update record %##s - success", rr->resrec.name->c);
			rr->state = regState_Registered;
			// deallocate old RData
			if (rr->UpdateCallback) rr->UpdateCallback(m, rr, rr->OrigRData);
			SetNewRData(&rr->resrec, rr->InFlightRData, rr->InFlightRDLen);
			rr->OrigRData = mDNSNULL;
			rr->InFlightRData = mDNSNULL;
			}
		}

	if (rr->state == regState_DeregPending)
		{
		debugf("Received reply for deregister record %##s type %d", rr->resrec.name->c, rr->resrec.rrtype);
		if (err) LogMsg("ERROR: Deregistration of record %##s type %d failed with error %ld",
						rr->resrec.name->c, rr->resrec.rrtype, err);
		err = mStatus_MemFree;
		rr->state = regState_Unregistered;
		}

	if (rr->state == regState_DeregDeferred)
		{
		if (err)
			{
			LogMsg("Cancelling deferred deregistration record %##s type %d due to registration error %ld",
				   rr->resrec.name->c, rr->resrec.rrtype, err);
			rr->state = regState_Unregistered;
			}
		debugf("Calling deferred deregistration of record %##s type %d",  rr->resrec.name->c, rr->resrec.rrtype);
		rr->state = regState_Registered;
		uDNS_DeregisterRecord(m, rr);
		return;
		}

	if (rr->state == regState_Pending || rr->state == regState_Refresh)
		{
		if (!err)
			{
			rr->state = regState_Registered;
			if (rr->state == regState_Refresh) InvokeCallback = mDNSfalse;
			}
		else
			{
			if (rr->lease && err == mStatus_UnknownErr)
				{
				LogMsg("Re-trying update of record %##s without lease option", rr->resrec.name->c);
				rr->lease = mDNSfalse;
				sendRecordRegistration(m, rr);
				return;
				}
			LogMsg("Registration of record %##s type %d failed with error %ld", rr->resrec.name->c, rr->resrec.rrtype, err);
			rr->state = regState_Unregistered;
			}
		}

	if (rr->state == regState_Unregistered) unlinkAR(&m->RecordRegistrations, rr);
	else rr->ThisAPInterval = INIT_UCAST_POLL_INTERVAL - 1;  // reset retry delay for future refreshes, dereg, etc.

	if (rr->QueuedRData && rr->state == regState_Registered)
		{
		rr->state = regState_UpdatePending;
		rr->InFlightRData = rr->QueuedRData;
		rr->InFlightRDLen = rr->QueuedRDLen;
		rr->OrigRData = rr->resrec.rdata;
		rr->OrigRDLen = rr->resrec.rdlength;
		rr->QueuedRData = mDNSNULL;
		sendRecordRegistration(m, rr);
		return;
		}

	if (InvokeCallback)
		{
		mDNS_DropLockBeforeCallback();
		if (rr->RecordCallback) rr->RecordCallback(m, rr, err);
		mDNS_ReclaimLockAfterCallback();
		}
	}

mDNSexport void uDNS_ReceiveNATMap(mDNS *m, mDNSu8 *pkt, mDNSu16 len)
	{
	NATTraversalInfo *ptr = m->NATTraversals;
	NATOp_t op;
	mDNSIPPort port;

	// check length, version, opcode
	if (len < sizeof(NATPortMapReply) && len < sizeof(NATAddrReply)) { LogMsg("NAT Traversal message too short (%d bytes)", len); return; }
	if (pkt[0] != NATMAP_VERS) { LogMsg("Received NAT Traversal response with version %d (expect version %d)", pkt[0], NATMAP_VERS); return; }
	op = (NATOp_t)pkt[1];

	if (!(op & NATMAP_RESPONSE_MASK)) { LogMsg("Received NAT Traversal message that is not a response (opcode %d)", op); return; }

	if (op == (NATOp_AddrRequest | NATMAP_RESPONSE_MASK))
		{
		port.b[0] = 0;
		port.b[1] = 0;
		}
	else
		{
		port.b[0] = pkt[8];
		port.b[1] = pkt[9];
		}

	while (ptr)
		{
		if ((ptr->state == NATState_Request || ptr->state == NATState_Refresh) && (ptr->op | NATMAP_RESPONSE_MASK) == op && (ptr->PrivatePort.NotAnInteger == port.NotAnInteger))
			if (ptr->ReceiveResponse(ptr, m, pkt, len)) break;  // note callback may invalidate ptr if it return value is non-zero
		ptr = ptr->next;
		}
	}

mDNSlocal const domainname *DNSRelayTestQuestion = (const domainname*)
	"\x1" "1" "\x1" "0" "\x1" "0" "\x3" "127" "\xa" "dnsbugtest"
	"\x1" "1" "\x1" "0" "\x1" "0" "\x3" "127" "\x7" "in-addr" "\x4" "arpa";

// Returns mDNStrue if response was handled
mDNSlocal mDNSBool uDNS_ReceiveTestQuestionResponse(mDNS *const m, DNSMessage *const msg, const mDNSu8 *const end,
	const mDNSAddr *const srcaddr)
	{
	const mDNSu8 *ptr = msg->data;
	DNSQuestion q;
	DNSServer *s;
	mDNSu32 result = 0;
	mDNSBool found = mDNSfalse;

	// 1. Find out if this is an answer to one of our test questions
	if (msg->h.numQuestions != 1) return(mDNSfalse);
	ptr = getQuestion(msg, ptr, end, mDNSInterface_Any, &q);
	if (!ptr) return(mDNSfalse);
	if (q.qtype != kDNSType_PTR || q.qclass != kDNSClass_IN) return(mDNSfalse);
	if (!SameDomainName(&q.qname, DNSRelayTestQuestion)) return(mDNSfalse);

	// 2. If the DNS relay gave us a positive response, then it's got buggy firmware
	// else, if the DNS relay gave us an error or no-answer response, it passed our test
	if ((msg->h.flags.b[1] & kDNSFlag1_RC) == kDNSFlag1_RC_NoErr && msg->h.numAnswers > 0)
		result = DNSServer_Failed;
	else
		result = DNSServer_Passed;

	// 3. Find occurrences of this server in our list, and mark them appropriately
	for (s = m->Servers; s; s = s->next)
		if (mDNSSameAddress(srcaddr, &s->addr) && s->teststate != result)
			{ s->teststate = result; found = mDNStrue; }

	// 4. Assuming we found the server in question in our list (don't want to risk being victim of a deliberate DOS attack here)
	// log a message to let the user know why Wide-Area Service Discovery isn't working
	if (found && result == DNSServer_Failed)
		LogMsg("NOTE: Wide-Area Service Discovery disabled to avoid crashing defective DNS relay %#a.", srcaddr);

	return(mDNStrue); // Return mDNStrue to tell uDNS_ReceiveMsg it doens't need to process this packet further
	}

mDNSlocal void hndlTruncatedAnswer(DNSQuestion *question, const mDNSAddr *src, mDNS *m)
	{
	uDNS_TCPSocket sock = mDNSNULL;
	tcpInfo_t *context;
	mDNSIPPort port = { { 0 } };
	mStatus err = mStatus_NoError;

	if (!src)
		{
		LogMsg("hndlTruncatedAnswer: TCP DNS response had TC bit set: ignoring");
		return;
		}

	context = (tcpInfo_t *)mDNSPlatformMemAllocate(sizeof(tcpInfo_t));
	if (!context)
		{
		LogMsg("ERROR: hndlTruncatedAnswer - memallocate failed");
		err = mStatus_UnknownErr;
		goto exit;
		}

	mDNSPlatformMemZero(context, sizeof(tcpInfo_t));
	context->question = question;
	context->m = m;
	question->TargetQID = mDNS_NewMessageID(m);

	sock = mDNSPlatformTCPSocket(m, kTCPSocketFlags_Zero, &port);

	if (!sock)
		{
		LogMsg("ERROR: unable to create TCP socket");
		err = mStatus_UnknownErr;
		goto exit;
		}

	err = mDNSPlatformTCPConnect(sock, src, UnicastDNSPort, question->InterfaceID, tcpCallback, context);

	if (err == mStatus_ConnEstablished)  // manually invoke callback if connection completes
		{
		tcpCallback(sock, context, mDNStrue, mStatus_NoError);
		err = mStatus_NoError;
		}
	else if (err == mStatus_ConnPending)
		{
		// callback will be automatically invoked when connection completes
		err = mStatus_NoError;
		}
	else
		{
		LogMsg("hndlTruncatedAnswer: connection failed");
		}

exit:

	if (err)
		{
		// Seems like a hack -- if this code is allowed to issue API calls, then it should have been called in the appropriate state in the first place
		// and if it's not allowed to issue API calls, then deliberately circumventing the locking check like this seems like it's asking for
		// tricky hard-to-diagnose crashing bugs
		mDNS_DropLockBeforeCallback();
		mDNS_StopQuery(m, question);
		mDNS_ReclaimLockAfterCallback();
		}
	}

mDNSlocal mDNSu32 GetPktLease(mDNS *m, DNSMessage *msg, const mDNSu8 *end)
	{
	const mDNSu8 *ptr = LocateAdditionals(msg, end);
	if (ptr)
		{
		int i;
		for (i = 0; i < msg->h.numAdditionals; i++)
			{
			LargeCacheRecord lcr;
			ptr = GetLargeResourceRecord(m, msg, ptr, end, 0, kDNSRecordTypePacketAdd, &lcr);
			if (!ptr) break;
			if (lcr.r.resrec.rrtype == kDNSType_OPT)
				{
				if (lcr.r.resrec.rdlength < LEASE_OPT_RDLEN) continue;
				if (lcr.r.resrec.rdata->u.opt.opt != kDNSOpt_Lease) continue;
				return(lcr.r.resrec.rdata->u.opt.OptData.lease);
				}
			}
		}
	return(0);
	}

mDNSexport void uDNS_ReceiveMsg(mDNS *const m, DNSMessage *const msg, const mDNSu8 *const end,
	const mDNSAddr *const srcaddr, const mDNSIPPort srcport)
	{
	DNSQuestion *qptr;
	AuthRecord *rptr;
	ServiceRecordSet *sptr;
	mStatus err = mStatus_NoError;

	mDNSu8 StdR    = kDNSFlag0_QR_Response | kDNSFlag0_OP_StdQuery;
	mDNSu8 UpdateR = kDNSFlag0_QR_Response | kDNSFlag0_OP_Update;
	mDNSu8 QR_OP   = (mDNSu8)(msg->h.flags.b[0] & kDNSFlag0_QROP_Mask);
	mDNSu8 rcode   = (mDNSu8)(msg->h.flags.b[1] & kDNSFlag1_RC);

	mDNSs32 timenow = m->timenow;

	debugf("uDNS_ReceiveMsg from %#-15a with "
		"%2d Question%s %2d Answer%s %2d Authorit%s %2d Additional%s",
		srcaddr,
		msg->h.numQuestions,   msg->h.numQuestions   == 1 ? ", " : "s,",
		msg->h.numAnswers,     msg->h.numAnswers     == 1 ? ", " : "s,",
		msg->h.numAuthorities, msg->h.numAuthorities == 1 ? "y,  " : "ies,",
		msg->h.numAdditionals, msg->h.numAdditionals == 1 ? "" : "s");

	if (QR_OP == StdR)
		{
		// !!!KRS we should to a table lookup here to see if it answers an LLQ or a 1-shot
		// LLQ Responses over TCP not currently supported
		if (srcaddr && recvLLQResponse(m, msg, end, srcaddr, srcport)) return;

		if (uDNS_ReceiveTestQuestionResponse(m, msg, end, srcaddr)) return;

		if (!msg->h.id.NotAnInteger) return;

		for (qptr = m->Questions; qptr; qptr = qptr->next)
			{
			//!!!KRS we should have a hashtable, hashed on message id
			if (qptr->TargetQID.NotAnInteger == msg->h.id.NotAnInteger)
				{
				if (timenow - (qptr->LastQTime + RESPONSE_WINDOW) > 0)
					{ debugf("uDNS_ReceiveMsg - response received after maximum allowed window.  Discarding"); return; }
				if (msg->h.flags.b[0] & kDNSFlag0_TC)
					{ hndlTruncatedAnswer(qptr, srcaddr, m); return; }
				else if (qptr->responseCallback)
					{
					if (m->CurrentQuestion)
						LogMsg("uDNS_ReceiveMsg: ERROR m->CurrentQuestion already set: %##s (%s)", m->CurrentQuestion->qname.c, DNSTypeName(m->CurrentQuestion->qtype));
					m->CurrentQuestion = qptr;
					qptr->responseCallback(m, msg, end, qptr);
					m->CurrentQuestion = mDNSNULL;
					// Note: responseCallback can invalidate qptr
					return;
					}
				}
			}
		}
	if (QR_OP == UpdateR && msg->h.id.NotAnInteger)
		{
		mDNSu32 lease = GetPktLease(m, msg, end);
		mDNSs32 expire = (m->timenow + (((mDNSs32)lease * mDNSPlatformOneSecond)) * 3/4);

		for (sptr = m->ServiceRegistrations; sptr; sptr = sptr->next)
			{
			if (sptr->id.NotAnInteger == msg->h.id.NotAnInteger)
				{
				err = checkUpdateResult(sptr->RR_SRV.resrec.name, rcode, m, msg, end);
				if (!err && sptr->lease)
					{
					if (!lease)
						sptr->lease = mDNSfalse;
					else
						if (sptr->expire - expire >= 0 || sptr->state != regState_UpdatePending)
							sptr->expire = expire;
					}
				hndlServiceUpdateReply(m, sptr, err);
				return;
				}
			}
		for (rptr = m->RecordRegistrations; rptr; rptr = rptr->next)
			{
			if (rptr->id.NotAnInteger == msg->h.id.NotAnInteger)
				{
				err = checkUpdateResult(rptr->resrec.name, rcode, m, msg, end);
				if (!err && rptr->lease)
					{
					if (!lease)
						rptr->lease = mDNSfalse;
					else
						if (rptr->expire - expire >= 0 || rptr->state != regState_UpdatePending)
							rptr->expire = expire;
					}
				hndlRecordUpdateReply(m, rptr, err);
				return;
				}
			}
		}
	debugf("Received unexpected response: ID %d matches no active records", mDNSVal16(msg->h.id));
	}

// lookup a DNS Server, matching by name in split-dns configurations.  Result stored in addr parameter if successful
mDNSlocal DNSServer *GetServerForName(mDNS *m, const domainname *name)
    {
	DNSServer *curmatch = mDNSNULL, *p = m->Servers;
	int i, curmatchlen = -1;
	int ncount = name ? CountLabels(name) : 0;

	while (p)
		{
		int scount = CountLabels(&p->domain);
		if (scount <= ncount && scount > curmatchlen)
			{
			// only inspect if server's domain is longer than current best match and shorter than the name itself
			const domainname *tail = name;
			for (i = 0; i < ncount - scount; i++)
				tail = (const domainname *)(tail->c + 1 + tail->c[0]);  // find "tail" (scount labels) of name
			if (SameDomainName(tail, &p->domain)) { curmatch = p; curmatchlen = scount; }
			}
		p = p->next;
		}
	return(curmatch);
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Query Routines
#endif

mDNSlocal void sendLLQRefresh(mDNS *m, DNSQuestion *q, mDNSu32 lease, uDNS_TCPSocket sock)
	{
	DNSMessage msg;
	mDNSu8 *end;
	LLQOptData llq;
	LLQ_Info *info = q->llq;
	mStatus err;
	mDNSs32 timenow;

	timenow = m->timenow;
	if ((info->state == LLQ_Refresh && info->ntries >= kLLQ_MAX_TRIES) ||
		info->expire - timenow < 0)
		{
		LogMsg("Unable to refresh LLQ %##s - will retry in %d minutes", q->qname.c, kLLQ_DEF_RETRY/60);
		info->state                 = LLQ_Retry;
		q->LastQTime                = m->timenow;
		q->ThisQInterval            = kLLQ_DEF_RETRY * mDNSPlatformOneSecond;
		return;
		//!!!KRS handle this - periodically try to re-establish
		}

	llq.vers = kLLQ_Vers;
	llq.llqOp = kLLQOp_Refresh;
	llq.err = LLQErr_NoError;
	llq.id = info->id;
	llq.lease = lease;

	initializeQuery(&msg, q);
	end = putLLQ(&msg, msg.data, q, &llq, mDNStrue);
	if (!end) { LogMsg("ERROR: sendLLQRefresh - putLLQ"); return; }

	err = mDNSSendDNSMessage(m, &msg, end, mDNSInterface_Any, &info->servAddr, info->servPort, sock, GetAuthInfoForName(m, &q->qname));
	if (err) debugf("ERROR: sendLLQRefresh - mDNSSendDNSMessage returned %ld", err);

	if (info->state == LLQ_Established) info->ntries = 1;
	else info->ntries++;
	info->state = LLQ_Refresh;
	q->LastQTime = timenow;
	}

// wrapper for startLLQHandshake, invoked by async op callback
mDNSlocal void startLLQHandshakeCallback(mStatus err, mDNS *const m, void *llqInfo, const AsyncOpResult *result)
	{
	LLQ_Info *info = (LLQ_Info *)llqInfo;
	const zoneData_t *zoneInfo = mDNSNULL;

    // check state first to make sure it is OK to touch question object
	if (info->state == LLQ_Cancelled)
		{
		// StopQuery was called while we were getting the zone info
		debugf("startLLQHandshake - LLQ Cancelled.");
		info->question = mDNSNULL;  // question may be deallocated
		mDNSPlatformMemFree(info);
		info = mDNSNULL;
		goto exit;
		}

	if (!info->question)
		{
		LogMsg("ERROR: startLLQHandshakeCallback invoked with NULL question");
		err = mStatus_UnknownErr;
		goto exit;
		}

	if (info->state != LLQ_GetZoneInfo)
		{
		LogMsg("ERROR: startLLQHandshake - bad state %d", info->state);
		err = mStatus_UnknownErr;
		goto exit;
		}

	if (err)
		{
		LogMsg("ERROR: startLLQHandshakeCallback %##s invoked with error code %ld", info->question->qname.c, err);
		startLLQPolling(m, info);
		err = mStatus_NoError;
		goto exit;
		}

	if (!result)
		{
		LogMsg("ERROR: startLLQHandshakeCallback invoked with NULL result and no error code");
		err = mStatus_UnknownErr;
		goto exit;
		}

	zoneInfo = &result->zoneData;

	if (!zoneInfo->llqPort.NotAnInteger)
		{
		debugf("LLQ port lookup failed - reverting to polling");
		info->servPort = zeroIPPort;
		startLLQPolling(m, info);
		goto exit;
		}

    // cache necessary zone data
	info->servAddr  = zoneInfo->primaryAddr;
	info->servPort  = zoneInfo->llqPort;
	info->isPrivate = zoneInfo->isPrivate;

    info->ntries = 0;

	if (info->state == LLQ_SuspendDeferred)
		{
		info->state = LLQ_Suspended;
		}
	else if (info->isPrivate)
		{
		startLLQHandshakePrivate(m, info, mDNSfalse);
		}
	else
		{
		startLLQHandshake(m, info, mDNSfalse);
		}

exit:

	if (err && info)
		{
		info->state = LLQ_Error;
		}
	}

mDNSlocal void startPrivateQueryCallback(mStatus err, mDNS *const m, void * context, const AsyncOpResult *result)
	{
	DNSQuestion * question = (DNSQuestion *) context;
	const zoneData_t *zoneInfo = mDNSNULL;
	tcpInfo_t * info;
	DomainAuthInfo * authInfo = mDNSNULL;
	uDNS_TCPSocket	sock = mDNSNULL;
	mDNSIPPort		port = { { 0 } };

	if (err)
		{
		LogMsg("ERROR: startPrivateQueryCallback %##s invoked with error code %ld", question->qname.c, err);
		goto exit;
		}

	if (!result)
		{
		LogMsg("ERROR: startPrivateQueryCallback invoked with NULL result and no error code");
		err = mStatus_UnknownErr;
		goto exit;
		}

	zoneInfo = &result->zoneData;

	if (!zoneInfo->isPrivate)
		{
		debugf("Private port lookup failed");
		err = mStatus_UnknownErr;
		goto exit;
		}

	authInfo = GetAuthInfoForName(m, &question->qname);

	if (!authInfo)
		{
		LogMsg("ERROR: startPrivateQueryCallback: cannot find credentials for question %##s", question->qname.c);
		err = mStatus_UnknownErr;
		goto exit;
		}

	info = (tcpInfo_t *)mDNSPlatformMemAllocate(sizeof(tcpInfo_t));
	if (!info)
		{
		LogMsg("ERROR: startPrivateQueryCallback - memallocate failed");
		err = mStatus_NoMemoryErr;
		goto exit;
		}

	mDNSPlatformMemZero(info, sizeof(tcpInfo_t));
	info->m        = m;
	info->question = question;
	info->authInfo = authInfo;
	question->TargetQID   = mDNS_NewMessageID(m);

	sock = mDNSPlatformTCPSocket(m, TCP_SOCKET_FLAGS, &port);

	if (!sock)
		{
		LogMsg("startPrivateQueryCallback: unable to create TCP socket");
		err = mStatus_UnknownErr;
		goto exit;
		}

	err = mDNSPlatformTCPConnect(sock, &zoneInfo->primaryAddr, zoneInfo->queryPort, question->InterfaceID, tcpCallback, info);

	if (err == mStatus_ConnEstablished)
		{
		// manually invoke callback if connection completes
		tcpCallback(sock, info, mDNStrue, mStatus_NoError);
		}
	else if (err != mStatus_ConnPending)
		{
		LogMsg("startPrivateQueryCallback: connection failed");
		mDNSPlatformTCPCloseConnection(sock);
		goto exit;
		}

	err = mStatus_NoError;

exit:

	if (err)
		{
		// Seems like a hack -- if this code is allowed to issue API calls, then it should have been called in the appropriate state in the first place
		// and if it's not allowed to issue API calls, then deliberately circumventing the locking check like this seems like it's asking for
		// tricky hard-to-diagnose crashing bugs
		mDNS_DropLockBeforeCallback();
		mDNS_StopQuery(m, question);
		mDNS_ReclaimLockAfterCallback();
		}
	}

mDNSexport mStatus uDNS_InitLongLivedQuery(mDNS * const m, DNSQuestion * const question)
    {
    LLQ_Info *info;
    mStatus err = mStatus_NoError;

    // allocate / init info struct
    info = mDNSPlatformMemAllocate(sizeof(LLQ_Info));
    if (!info)
        {
        LogMsg("ERROR: startLLQ - malloc");
        err = mStatus_NoMemoryErr;
        goto exit;
        }

    mDNSPlatformMemZero(info, sizeof(LLQ_Info));
    info->state = LLQ_GetZoneInfo;

    // link info/question
    info->question = question;

	// Set ThisQInterval to 0, signifying that this question isn't active yet.
	question->ThisQInterval = 0;
	question->LastQTime     = m->timenow;
    question->llq           = info;

    question->responseCallback = pktResponseHndlr;

    err = StartGetZoneData(m, &question->qname, lookupLLQSRV, startLLQHandshakeCallback, info);

    if (err)
        {
        LogMsg("ERROR: startLLQ - StartGetZoneData returned %ld", err);
        info->question = mDNSNULL;
        question->llq = mDNSNULL;
        goto exit;
        }

exit:

    if (err)
        {
        mDNSPlatformMemFree(info);
        }

	return err;
	}

// stopLLQ happens IN ADDITION to stopQuery
void uDNS_StopLongLivedQuery(mDNS * const m, DNSQuestion * const question)
	{
	LLQ_Info *info = question->llq;
	(void)m;  // unused

	if (!question->LongLived) { LogMsg("ERROR: stopLLQ - LongLived flag not set"); return; }
	if (!info)                { LogMsg("ERROR: stopLLQ - llq info is NULL");       return; }

	switch (info->state)
		{
		case LLQ_UnInit:
			LogMsg("ERROR: stopLLQ - state LLQ_UnInit");
			//!!!KRS should we unlink info<->question here?
			return;
		case LLQ_GetZoneInfo:
		case LLQ_SuspendDeferred:
			info->question = mDNSNULL; // remove ref to question, as it may be freed when we get called back from async op
			info->state = LLQ_Cancelled;
			return;
		case LLQ_Established:
		case LLQ_Refresh:
			// refresh w/ lease 0
			sendLLQRefresh(m, question, 0, info->tcpSock);
			goto end;
		default:
			debugf("stopLLQ - silently discarding LLQ in state %d", info->state);
			goto end;
		}

	end:
	RemoveLLQNatMappings(m, info);
	CheckForUnreferencedLLQMapping(m);
	info->question = mDNSNULL;

	if (info->tcpSock)
		{
		mDNSPlatformTCPCloseConnection(info->tcpSock);
		info->tcpSock = mDNSNULL;
		}

	if (info->udpSock)
		{
		mDNSPlatformUDPClose(info->udpSock);
		info->udpSock = mDNSNULL;
		}

	mDNSPlatformMemFree(info);
	question->llq = mDNSNULL;
	question->LongLived = mDNSfalse;
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Dynamic Updates
#endif

mDNSlocal void RecordRegistrationCallback(mStatus err, mDNS *const m, void *authPtr, const AsyncOpResult *result)
	{
	AuthRecord *newRR = (AuthRecord*)authPtr;
	const zoneData_t *zoneData = mDNSNULL;
	AuthRecord *ptr;

	// make sure record is still in list
	for (ptr = m->RecordRegistrations; ptr; ptr = ptr->next)
		if (ptr == newRR) break;
	if (!ptr) { LogMsg("RecordRegistrationCallback - RR no longer in list.  Discarding."); return; }

	// check error/result
	if (err) { LogMsg("RecordRegistrationCallback: error %ld", err); goto error; }
	if (!result) { LogMsg("ERROR: RecordRegistrationCallback invoked with NULL result and no error"); goto error;  }
	else zoneData = &result->zoneData;

	if (newRR->state == regState_Cancelled)
		{
		//!!!KRS we should send a memfree callback here!
		debugf("Registration of %##s type %d cancelled prior to update",
			   newRR->resrec.name->c, newRR->resrec.rrtype);
		newRR->state = regState_Unregistered;
		unlinkAR(&m->RecordRegistrations, newRR);
		return;
		}

	if (newRR->resrec.rrclass != zoneData->zoneClass)
		{
		LogMsg("ERROR: New resource record's class (%d) does not match zone class (%d)",
			   newRR->resrec.rrclass, zoneData->zoneClass);
		goto error;
		}

	// Don't try to do updates to the root name server.
	// We might be tempted also to block updates to any single-label name server (e.g. com, edu, net, etc.) but some
	// organizations use their own private pseudo-TLD, like ".home", etc, and we don't want to block that.
	if (zoneData->zoneName.c[0] == 0)
		{
		LogMsg("ERROR: Only name server claiming responsibility for \"%##s\" is \"%##s\"!",
			newRR->resrec.name->c, zoneData->zoneName.c);
		err = mStatus_NoSuchNameErr;
		goto error;
		}

	// cache zone data
	AssignDomainName(&newRR->zone, &zoneData->zoneName);
	newRR->UpdateServer = zoneData->primaryAddr;
	if (zoneData->updatePort.NotAnInteger)
		{
		newRR->UpdatePort = zoneData->updatePort;
		newRR->Private = zoneData->isPrivate;
		}
	else
		{
		debugf("Update port not advertised via SRV - guessing port 53, no lease option");
		newRR->UpdatePort = UnicastDNSPort;
		newRR->lease = mDNSfalse;
		}

	sendRecordRegistration(m, newRR);
	return;

error:
	if (newRR->state != regState_Unregistered)
		{
		unlinkAR(&m->RecordRegistrations, newRR);
		newRR->state = regState_Unregistered;
		}
	mDNS_DropLockBeforeCallback();
	if (newRR->RecordCallback)
		newRR->RecordCallback(m, newRR, err);
	mDNS_ReclaimLockAfterCallback();
	// NOTE: not safe to touch any client structures here
	}

mDNSlocal mStatus SetupRecordRegistration(mDNS *m, AuthRecord *rr)
	{
	domainname *target = GetRRDomainNameTarget(&rr->resrec);
	AuthRecord *ptr = m->RecordRegistrations;

	while (ptr && ptr != rr) ptr = ptr->next;
	if (ptr) { LogMsg("Error: SetupRecordRegistration - record %##s already in list!", rr->resrec.name->c); return mStatus_AlreadyRegistered; }

	if (rr->state == regState_FetchingZoneData ||
		rr->state == regState_Pending ||
		rr->state ==  regState_Registered)
		{
		LogMsg("Requested double-registration of physical record %##s type %d",
			   rr->resrec.name->c, rr->resrec.rrtype);
		return mStatus_AlreadyRegistered;
		}

	rr->resrec.rdlength   = GetRDLength(&rr->resrec, mDNSfalse);
	rr->resrec.rdestimate = GetRDLength(&rr->resrec, mDNStrue);

	if (!ValidateDomainName(rr->resrec.name))
		{
		LogMsg("Attempt to register record with invalid name: %s", ARDisplayString(m, rr));
		return mStatus_Invalid;
		}

	// Don't do this until *after* we've set rr->resrec.rdlength
	if (!ValidateRData(rr->resrec.rrtype, rr->resrec.rdlength, rr->resrec.rdata))
		{
		LogMsg("Attempt to register record with invalid rdata: %s", ARDisplayString(m, rr));
		return mStatus_Invalid;
		}

	rr->resrec.namehash   = DomainNameHashValue(rr->resrec.name);
	rr->resrec.rdatahash  = target ? DomainNameHashValue(target) : RDataHashValue(rr->resrec.rdlength, &rr->resrec.rdata->u);

	rr->state = regState_FetchingZoneData;
	rr->next = m->RecordRegistrations;
	m->RecordRegistrations = rr;
	rr->lease = mDNStrue;

	return mStatus_NoError;
	}

mDNSexport mStatus uDNS_RegisterRecord(mDNS *const m, AuthRecord *const rr)
	{
	mStatus err = SetupRecordRegistration(m, rr);
	if (err) return err;
	else return StartGetZoneData(m, rr->resrec.name, lookupUpdateSRV, RecordRegistrationCallback, rr);
	}

mDNSlocal void SendRecordDeregistration(mDNS *m, AuthRecord *rr)
	{
	DNSMessage msg;
	mDNSu8 *ptr = msg.data;
	DomainAuthInfo * authInfo;
	mDNSu8 *end = (mDNSu8 *)&msg + sizeof(DNSMessage);
	mStatus err = mStatus_NoError;

	InitializeDNSMessage(&msg.h, rr->id, UpdateReqFlags);

	ptr = putZone(&msg, ptr, end, &rr->zone, mDNSOpaque16fromIntVal(rr->resrec.rrclass));
	if (!ptr) { err = mStatus_UnknownErr; goto exit; }
	if (!(ptr = putDeletionRecord(&msg, ptr, &rr->resrec))) { err = mStatus_UnknownErr; goto exit; }

	rr->state = regState_DeregPending;
	authInfo            = GetAuthInfoForName(m, rr->resrec.name);

	if (rr->Private)
		{
		tcpInfo_t	*	info;
		uDNS_TCPSocket	sock;
		mDNSIPPort		port = { { 0 } };

		info = (tcpInfo_t *)mDNSPlatformMemAllocate(sizeof(tcpInfo_t));

		if (!info)
			{
			LogMsg("ERROR: SendRecordDeregistration - memallocate failed");
			err = mStatus_UnknownErr;
			goto exit;
			}

		mDNSPlatformMemZero(info, sizeof(tcpInfo_t));
		info->request  = msg;
		info->m        = m;
		info->authInfo = authInfo;
		info->rr       = rr;
		info->requestLen = (int) (ptr - ((mDNSu8*) &msg));

		sock = mDNSPlatformTCPSocket(m, TCP_SOCKET_FLAGS, &port);

		if (!sock)
			{
			LogMsg("SendRecordDeregistration: unable to create TCP socket");
			mDNSPlatformMemFree(info);
			goto exit;
			}

		err = mDNSPlatformTCPConnect(sock, &rr->UpdateServer, rr->UpdatePort, 0, tcpCallback, info);

		if (err == mStatus_ConnEstablished)
			{
			// manually invoke callback if connection completes
			tcpCallback(sock, info, mDNStrue, mStatus_NoError);
			err = mStatus_NoError;
			}
		else if (err == mStatus_ConnPending)
			{
			// callback will be automatically invoked when connection completes
			err = mStatus_NoError;
			}
		else
			{
			LogMsg("SendRecordDeregistration: connection failed");
			mDNSPlatformMemFree(info);
			goto exit;
			}
		}
	else
		{
		err = mDNSSendDNSMessage(m, &msg, ptr, mDNSInterface_Any, &rr->UpdateServer, rr->UpdatePort, mDNSNULL, authInfo);
		if (err) debugf("ERROR: SendRecordDeregistration - mDNSSendDNSMessage - %ld", err);
		}

	SetRecordRetry(m, rr, err);

	err = mStatus_NoError;

exit:

	if (err)
		{
		LogMsg("Error: SendRecordDeregistration - could not contruct deregistration packet");
		unlinkAR(&m->RecordRegistrations, rr);
		rr->state = regState_Unregistered;
		}
	}

mDNSexport mStatus uDNS_DeregisterRecord(mDNS *const m, AuthRecord *const rr)
	{
	NATTraversalInfo *n = rr->NATinfo;

	switch (rr->state)
		{
		case regState_NATMap:
            // we're in the middle of a NAT traversal operation
            rr->NATinfo = mDNSNULL;
			if (!n) LogMsg("uDNS_DeregisterRecord: no NAT info context");
			else uDNS_FreeNATInfo(m, n); // cause response to outstanding request to be ignored.
			                        // Note: normally here we're trying to determine our public address,
			                        //in which case there is not state to be torn down.  For simplicity,
			                        //we allow other operations to expire.
			rr->state = regState_Unregistered;
			break;
		case regState_ExtraQueued:
			rr->state = regState_Unregistered;
			break;
		case regState_FetchingZoneData:
			rr->state = regState_Cancelled;
			return mStatus_NoError;
		case regState_Refresh:
		case regState_Pending:
		case regState_UpdatePending:
			rr->state = regState_DeregDeferred;
			LogMsg("Deferring deregistration of record %##s until registration completes", rr->resrec.name->c);
			return mStatus_NoError;
		case regState_Registered:
		case regState_DeregPending:
			break;
		case regState_DeregDeferred:
		case regState_Cancelled:
			LogMsg("Double deregistration of record %##s type %d",
				   rr->resrec.name->c, rr->resrec.rrtype);
			return mStatus_UnknownErr;
		case regState_Unregistered:
			LogMsg("Requested deregistration of unregistered record %##s type %d",
				   rr->resrec.name->c, rr->resrec.rrtype);
			return mStatus_UnknownErr;
		case regState_NATError:
		case regState_NoTarget:
			LogMsg("ERROR: uDNS_DeregisterRecord called for record %##s with bad state %s", rr->resrec.name->c, rr->state == regState_NoTarget ? "regState_NoTarget" : "regState_NATError");
			return mStatus_UnknownErr;
		default: LogMsg("uDNS_DeregisterRecord: Unknown state %d for %##s", rr->state, rr->resrec.name->c);
		}

	if (rr->state == regState_Unregistered)
		{
		// unlink and deliver memfree
		unlinkAR(&m->RecordRegistrations, rr);
		mDNS_DropLockBeforeCallback();
		if (rr->RecordCallback)
			rr->RecordCallback(m, rr, mStatus_MemFree);
		mDNS_ReclaimLockAfterCallback();
		return mStatus_NoError;
		}

	rr->NATinfo = mDNSNULL;
	if (n) uDNS_FreeNATInfo(m, n);

	SendRecordDeregistration(m, rr);
	return mStatus_NoError;
	}

mDNSexport mStatus uDNS_RegisterService(mDNS *const m, ServiceRecordSet *srs)
	{
	mDNSu32 i;
	ServiceRecordSet **p = &m->ServiceRegistrations;
	while (*p && *p != srs) p=&(*p)->next;
	if (*p) { LogMsg("uDNS_RegisterService: %p %##s already in list", srs, srs->RR_SRV.resrec.name->c); return(mStatus_AlreadyRegistered); }

	*p = srs;
	srs->next = mDNSNULL;

	srs->RR_SRV.resrec.rroriginalttl = kWideAreaTTL;
	srs->RR_TXT.resrec.rroriginalttl = kWideAreaTTL;
	srs->RR_PTR.resrec.rroriginalttl = kWideAreaTTL;
	for (i = 0; i < srs->NumSubTypes;i++) srs->SubTypes[i].resrec.rroriginalttl = kWideAreaTTL;

	srs->lease = mDNStrue;

	if (!GetServiceTarget(m, &srs->RR_SRV))
		{
		// defer registration until we've got a target
		debugf("uDNS_RegisterService - no target for %##s", srs->RR_SRV.resrec.name->c);
		srs->state = regState_NoTarget;
		return mStatus_NoError;
		}

	srs->state = regState_FetchingZoneData;
	return StartGetZoneData(m, srs->RR_SRV.resrec.name, lookupUpdateSRV, serviceRegistrationCallback, srs);
	}

mDNSexport mStatus uDNS_DeregisterService(mDNS *const m, ServiceRecordSet *srs)
	{
	NATTraversalInfo *nat = srs->NATinfo;
	char *errmsg = "Unknown State";

	// don't re-register with a new target following deregistration
	srs->SRVChanged = srs->SRVUpdateDeferred = mDNSfalse;

	if (nat)
		{
		if (nat->state == NATState_Established || nat->state == NATState_Refresh || nat->state == NATState_Legacy)
			uDNS_DeleteNATPortMapping(m, nat);
		nat->reg.ServiceRegistration = mDNSNULL;
		srs->NATinfo = mDNSNULL;
		uDNS_FreeNATInfo(m, nat);
		}

	switch (srs->state)
		{
		case regState_Unregistered:
			debugf("uDNS_DeregisterService - service %##s not registered", srs->RR_SRV.resrec.name->c);
			return mStatus_BadReferenceErr;
		case regState_FetchingZoneData:
			// let the async op complete, then terminate
			srs->state = regState_Cancelled;
			return mStatus_NoError;  // deliver memfree upon completion of async op
		case regState_Pending:
		case regState_Refresh:
		case regState_UpdatePending:
			// deregister following completion of in-flight operation
			srs->state = regState_DeregDeferred;
			return mStatus_NoError;
		case regState_DeregPending:
		case regState_DeregDeferred:
		case regState_Cancelled:
			debugf("Double deregistration of service %##s", srs->RR_SRV.resrec.name->c);
			return mStatus_NoError;
		case regState_NATError:  // not registered
		case regState_NATMap:    // not registered
		case regState_NoTarget:  // not registered
			unlinkSRS(m, srs);
			srs->state = regState_Unregistered;
			mDNS_DropLockBeforeCallback();
			srs->ServiceCallback(m, srs, mStatus_MemFree);
			mDNS_ReclaimLockAfterCallback();
			return mStatus_NoError;
		case regState_Registered:
			srs->state = regState_DeregPending;
			SendServiceDeregistration(m, srs);
			return mStatus_NoError;
		case regState_ExtraQueued: // only for record registrations
			errmsg = "bad state (regState_ExtraQueued)";
			goto error;
		default: LogMsg("uDNS_DeregisterService: Unknown state %d for %##s", srs->state, srs->RR_SRV.resrec.name->c);
		}

	error:
	LogMsg("Error, uDNS_DeregisterService: %s", errmsg);
	return mStatus_BadReferenceErr;
	}

mDNSexport mStatus uDNS_AddRecordToService(mDNS *const m, ServiceRecordSet *sr, ExtraResourceRecord *extra)
	{
	mStatus err = mStatus_UnknownErr;

	extra->r.resrec.RecordType = kDNSRecordTypeShared;  // don't want it to conflict with the service name
	extra->r.RecordCallback = mDNSNULL;  // don't generate callbacks for extra RRs

	if (sr->state == regState_Registered || sr->state == regState_Refresh)
		err = mDNS_Register_internal(m, &extra->r);
	else
		{
		err = SetupRecordRegistration(m, &extra->r);
		extra->r.state = regState_ExtraQueued; // %%% Is it okay to overwrite the previous state?
		}

	if (!err)
		{
		extra->next = sr->Extras;
		sr->Extras = extra;
		}
	return err;
	}

mDNSexport mStatus uDNS_UpdateRecord(mDNS *m, AuthRecord *rr)
	{
	ServiceRecordSet *parent = mDNSNULL;
	AuthRecord *rptr;
	regState_t *stateptr = mDNSNULL;

	// find the record in registered service list
	for (parent = m->ServiceRegistrations; parent; parent = parent->next)
		if (&parent->RR_TXT == rr) { stateptr = &parent->state; break; }

	if (!parent)
		{
		// record not part of a service - check individual record registrations
		for (rptr = m->RecordRegistrations; rptr; rptr = rptr->next)
			if (rptr == rr) { stateptr = &rr->state; break; }
		if (!rptr) goto unreg_error;
		}

	switch(*stateptr)
		{
		case regState_DeregPending:
		case regState_DeregDeferred:
		case regState_Cancelled:
		case regState_Unregistered:
			// not actively registered
			goto unreg_error;

		case regState_FetchingZoneData:
		case regState_NATMap:
		case regState_ExtraQueued:
		case regState_NoTarget:
			// change rdata directly since it hasn't been sent yet
			if (rr->UpdateCallback) rr->UpdateCallback(m, rr, rr->resrec.rdata);
			SetNewRData(&rr->resrec, rr->NewRData, rr->newrdlength);
			rr->NewRData = mDNSNULL;
			return mStatus_NoError;

		case regState_Pending:
		case regState_Refresh:
		case regState_UpdatePending:
			// registration in-flight.  queue rdata and return
			if (rr->QueuedRData && rr->UpdateCallback)
				// if unsent rdata is already queued, free it before we replace it
				rr->UpdateCallback(m, rr, rr->QueuedRData);
			rr->QueuedRData = rr->NewRData;
			rr->QueuedRDLen = rr->newrdlength;
			rr->NewRData = mDNSNULL;
			return mStatus_NoError;

		case regState_Registered:
			rr->OrigRData = rr->resrec.rdata;
			rr->OrigRDLen = rr->resrec.rdlength;
			rr->InFlightRData = rr->NewRData;
			rr->InFlightRDLen = rr->newrdlength;
			rr->NewRData = mDNSNULL;
			*stateptr = regState_UpdatePending;
			if (parent)  SendServiceRegistration(m, parent);
			else sendRecordRegistration(m, rr);
			return mStatus_NoError;

		case regState_NATError:
			LogMsg("ERROR: uDNS_UpdateRecord called for record %##s with bad state regState_NATError", rr->resrec.name->c);
			return mStatus_UnknownErr;  // states for service records only

		default: LogMsg("uDNS_UpdateRecord: Unknown state %d for %##s", *stateptr, rr->resrec.name->c);
		}

	unreg_error:
	LogMsg("Requested update of record %##s type %d, part of service not currently registered",
		   rr->resrec.name->c, rr->resrec.rrtype);
	return mStatus_Invalid;
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Periodic Execution Routines
#endif

mDNSlocal mDNSs32 CheckNATMappings(mDNS *m, mDNSs32 timenow)
	{
	NATTraversalInfo *ptr = m->NATTraversals;
	mDNSs32 nextevent = timenow + 0x3FFFFFFF;

	while (ptr)
		{
		NATTraversalInfo *cur = ptr;
		ptr = ptr->next;
		if (cur->op != NATOp_AddrRequest || cur->state != NATState_Established)  // no refresh necessary for established Add requests
			{
			if (cur->retry - timenow < 0)
				{
				if (cur->state == NATState_Established) RefreshNATMapping(cur, m);
				else if (cur->state == NATState_Request || cur->state == NATState_Refresh)
					{
					if (cur->ntries >= NATMAP_MAX_TRIES) cur->ReceiveResponse(cur, m, mDNSNULL, 0); // may invalidate "cur"
					else
						{
						uDNS_SendNATMsg(cur, m);
						if (cur->retry - nextevent < 0) nextevent = cur->retry;
						}
					}
				}
			else if (cur->retry - nextevent < 0) nextevent = cur->retry;
			}
		}
	return nextevent;
	}

mDNSexport void uDNS_CheckQuery(mDNS * const m, DNSQuestion * q)
	{
	LLQ_Info *llq;
	mDNSs32 sendtime;
	DNSMessage msg;
	mStatus err = mStatus_NoError;
	mDNSu8 *end;

	if (m->CurrentQuestion)
		LogMsg("uDNS_CheckQuery: ERROR m->CurrentQuestion already set: %##s (%s)", m->CurrentQuestion->qname.c, DNSTypeName(m->CurrentQuestion->qtype));
	m->CurrentQuestion = q;
	llq = q->llq;

	if (q != m->CurrentQuestion) { m->CurrentQuestion = mDNSNULL; return; }

	if (q->LongLived && llq->state != LLQ_Poll)
		{
		if (llq->state >= LLQ_InitialRequest && llq->state <= LLQ_Established)
			{
			if ((q->LastQTime + q->ThisQInterval) - m->timenow <= 0)
				{
				// sanity check to avoid packet flood bugs
				if (!q->ThisQInterval)
					LogMsg("ERROR: retry timer not set for LLQ %##s in state %d", q->qname.c, llq->state);
				else if (llq->state == LLQ_Established || llq->state == LLQ_Refresh)
					{
					sendLLQRefresh(m, q, llq->origLease, llq->tcpSock);
					}
				else if (llq->state == LLQ_InitialRequest)
					{
					if (llq->isPrivate)
						startLLQHandshakePrivate(m, llq, mDNSfalse);
					else
						startLLQHandshake(m, llq, mDNSfalse);
					}
				else if (llq->state == LLQ_SecondaryRequest)
					{
					sendChallengeResponse(m, q, mDNSNULL);
					}
				else if (llq->state == LLQ_Retry)
					{
					llq->ntries = 0;
					if (llq->isPrivate)
						startLLQHandshakePrivate(m, llq, mDNSfalse);
					else
						startLLQHandshake(m, llq, mDNSfalse);
					}
				}
			}
		else if ((q->LastQTime + q->ThisQInterval) - m->timenow <= 0)
			{
			q->LastQTime = m->timenow;
			}
		}
	else
		{
		sendtime = q->LastQTime + q->ThisQInterval;

		// Don't allow sendtime to be earlier than SuppressStdPort53Queries
		if (!q->LongLived && m->SuppressStdPort53Queries && sendtime - m->SuppressStdPort53Queries < 0)
			sendtime = m->SuppressStdPort53Queries;

		if (sendtime - m->timenow <= 0)
			{
			DNSServer *server = GetServerForName(m, &q->qname);

			if (server)
				{
				DomainAuthInfo *private = mDNSNULL;

				if (server->teststate == DNSServer_Untested)
					{
					InitializeDNSMessage(&msg.h, mDNS_NewMessageID(m), uQueryFlags);
					end = putQuestion(&msg, msg.data, msg.data + AbsoluteMaxDNSMessageData, DNSRelayTestQuestion, kDNSType_PTR, kDNSClass_IN);
					}
				else
					{
					err = constructQueryMsg(&msg, &end, q);
					private = q->Private;
					}

				if (err)  LogMsg("Error: uDNS_CheckQuery - constructQueryMsg.  Skipping question %##s", q->qname.c);
				else
					{
					if (server->teststate != DNSServer_Failed)
						{
						if (!private)
							err = mDNSSendDNSMessage(m, &msg, end, mDNSInterface_Any, &server->addr, UnicastDNSPort, mDNSNULL, mDNSNULL);
						else
							err = StartGetZoneData(m, &q->qname, q->LongLived ? lookupLLQSRV : lookupQuerySRV, startPrivateQueryCallback, q);
						}

					m->SuppressStdPort53Queries = NonZeroTime(m->timenow + (mDNSPlatformOneSecond+99)/100);
					if (err) debugf("ERROR: uDNS_idle - mDNSSendDNSMessage - %ld", err); // surpress syslog messages if we have no network
					else if (!q->LongLived && q->ThisQInterval < MAX_UCAST_POLL_INTERVAL)
						{
						q->ThisQInterval = q->ThisQInterval * 2;  // don't increase interval if send failed
						}
					}
				}

			q->LastQTime = m->timenow;
			}
		}
	m->CurrentQuestion = mDNSNULL;
	}

mDNSlocal mDNSs32 CheckRecordRegistrations(mDNS *m, mDNSs32 timenow)
	{
	AuthRecord *rr;
 	mDNSs32 nextevent = timenow + 0x3FFFFFFF;

	//!!!KRS list should be pre-sorted by expiration
	for (rr = m->RecordRegistrations; rr; rr = rr->next)
		{
		if (rr->state == regState_Pending || rr->state == regState_DeregPending || rr->state == regState_UpdatePending || rr->state == regState_DeregDeferred || rr->state == regState_Refresh)
			{
			if (rr->LastAPTime + rr->ThisAPInterval - timenow < 0)
				{
#if MDNS_DEBUGMSGS
				char *op = "(unknown operation)";
				if      (rr->state == regState_Pending     ) op = "registration";
				else if (rr->state == regState_DeregPending) op = "deregistration";
				else if (rr->state == regState_Refresh     ) op = "refresh";
				debugf("Retransmit record %s %##s", op, rr->resrec.name->c);
#endif
				//LogMsg("Retransmit record %##s", rr->resrec.name->c);
				if      (rr->state == regState_DeregPending)   SendRecordDeregistration(m, rr);
				else                                              sendRecordRegistration(m, rr);
				}
			if (rr->LastAPTime + rr->ThisAPInterval - nextevent < 0) nextevent = rr->LastAPTime + rr->ThisAPInterval;
			}
		if (rr->lease && rr->state == regState_Registered)
			{
			if (rr->expire - timenow < 0)
				{
				debugf("refreshing record %##s", rr->resrec.name->c);
				rr->state = regState_Refresh;
				sendRecordRegistration(m, rr);
				}
			if (rr->expire - nextevent < 0) nextevent = rr->expire;
			}
		}
	return nextevent;
	}

mDNSlocal mDNSs32 CheckServiceRegistrations(mDNS *m, mDNSs32 timenow)
	{
	ServiceRecordSet *s = m->ServiceRegistrations;
	mDNSs32 nextevent = timenow + 0x3FFFFFFF;

	// Note: ServiceRegistrations list is in the order they were created; important for in-order event delivery
	while (s)
		{
		ServiceRecordSet *srs = s;
		// NOTE: Must advance s here -- SendServiceDeregistration may delete the object we're looking at,
		// and then if we tried to do srs = srs->next at the end we'd be referencing a dead object
		s = s->next;
		if (srs->state == regState_Pending || srs->state == regState_DeregPending || srs->state == regState_DeregDeferred || srs->state == regState_Refresh  || srs->state == regState_UpdatePending)
			{
			if (srs->RR_SRV.LastAPTime + srs->RR_SRV.ThisAPInterval - timenow < 0)
				{
#if MDNS_DEBUGMSGS
				char *op = "unknown";
				if      (srs->state == regState_Pending      ) op = "registration";
				else if (srs->state == regState_DeregPending) op = "deregistration";
				else if (srs->state == regState_Refresh      ) op = "refresh";
				else if (srs->state == regState_UpdatePending) op = "txt record update";
				debugf("Retransmit service %s %##s", op, srs->RR_SRV.resrec.name->c);
#endif
				if (srs->state == regState_DeregPending) { SendServiceDeregistration(m, srs); continue; }
				else                                         SendServiceRegistration  (m, srs);
				}
			if (nextevent - srs->RR_SRV.LastAPTime + srs->RR_SRV.ThisAPInterval > 0)
				nextevent = srs->RR_SRV.LastAPTime + srs->RR_SRV.ThisAPInterval;
			}

		if (srs->lease && srs->state == regState_Registered)
			{
			if (srs->expire - timenow < 0)
				{
				debugf("refreshing service %##s", srs->RR_SRV.resrec.name->c);
				srs->state = regState_Refresh;
				SendServiceRegistration(m, srs);
				}
			if (srs->expire - nextevent < 0) nextevent = srs->expire;
			}
		}
	return nextevent;
	}

mDNSexport void uDNS_Execute(mDNS *const m)
	{
	mDNSs32 nexte, timenow = m->timenow;

	m->nextevent = timenow + 0x3FFFFFFF;

	if (m->DelaySRVUpdate && m->NextSRVUpdate - timenow < 0)
		{
		m->DelaySRVUpdate = mDNSfalse;
		UpdateSRVRecords(m);
		}

	nexte = CheckNATMappings(m, timenow);
	if (nexte - m->nextevent < 0) m->nextevent = nexte;

	if (m->SuppressStdPort53Queries && m->timenow - m->SuppressStdPort53Queries >= 0)
		m->SuppressStdPort53Queries = 0; // If suppression time has passed, clear it

	nexte = CheckRecordRegistrations(m, timenow);
	if (nexte - m->nextevent < 0) m->nextevent = nexte;

	nexte = CheckServiceRegistrations(m, timenow);
	if (nexte - m->nextevent < 0) m->nextevent = nexte;

	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Startup, Shutdown, and Sleep
#endif

// DeregisterActive causes active LLQs to be removed from the server, e.g. before sleep.  Pass false
// following a location change, as the server will reject deletions from a source address different
// from the address on which the LLQ was created.

mDNSlocal void SuspendLLQs(mDNS *m, mDNSBool DeregisterActive)
	{
	DNSQuestion *q;
	LLQ_Info *llq;

	for (q = m->Questions; q; q = q->next)
		{
		llq = q->llq;
		if (q->LongLived && llq)
			{
			if (llq->state == LLQ_GetZoneInfo)
				{
				debugf("Marking %##s suspend-deferred", q->qname.c);
				llq->state = LLQ_SuspendDeferred;  // suspend once we're done getting zone info
				}
			else if (llq->state < LLQ_Suspended)
				{
				if (DeregisterActive && (llq->state == LLQ_Established || llq->state == LLQ_Refresh))
					{
					debugf("Deleting LLQ %##s", q->qname.c);
					sendLLQRefresh(m, q, 0, llq->tcpSock);
					}
				debugf("Marking %##s suspended", q->qname.c);
				llq->state = LLQ_Suspended;
				llq->eventPort = zeroIPPort;

				if (llq->tcpSock)
					{
					mDNSPlatformTCPCloseConnection(llq->tcpSock);
					llq->tcpSock = mDNSNULL;
					}

				if (llq->udpSock)
					{
					mDNSPlatformUDPClose(llq->udpSock);
					llq->udpSock = mDNSNULL;
					}

				llq->id = zeroOpaque64;
				}
			else if (llq->state == LLQ_Poll)
				{
				debugf("Marking %##s suspended-poll", q->qname.c);
				llq->state = LLQ_SuspendedPoll;
				}

			if (llq->NATInfoTCP || llq->NATInfoUDP)
				{
				// may not need nat mapping if we restart with new route
				RemoveLLQNatMappings(m, llq);
				}
			}
		}
	CheckForUnreferencedLLQMapping(m);
	}

mDNSlocal void RestartQueries(mDNS *m)
	{
	DNSQuestion *q;
	LLQ_Info *llqInfo;
	mDNSs32 timenow = m->timenow;

	if (m->CurrentQuestion)
		LogMsg("RestartQueries: ERROR m->CurrentQuestion already set: %##s (%s)", m->CurrentQuestion->qname.c, DNSTypeName(m->CurrentQuestion->qtype));
	m->CurrentQuestion = m->Questions;
	while (m->CurrentQuestion)
		{
		q = m->CurrentQuestion;
		m->CurrentQuestion = m->CurrentQuestion->next;

		if (q->TargetQID.NotAnInteger)
			{
			llqInfo = q->llq;

			q->RestartTime = timenow;
			q->Answered = mDNSfalse;
			if (q->LongLived)
				{
				if (!llqInfo) { LogMsg("Error: RestartQueries - %##s long-lived with NULL info", q->qname.c); continue; }
				if (llqInfo->state == LLQ_Suspended || llqInfo->state == LLQ_NatMapWaitTCP || llqInfo->state == LLQ_NatMapWaitUDP)
					{
					llqInfo->ntries = -1;
					if (llqInfo->isPrivate)
						startLLQHandshakePrivate(m, llqInfo, mDNStrue);	// we set defer to true since several events that may generate restarts often arrive in rapid succession, and this cuts unnecessary packets
					else
						startLLQHandshake(m, llqInfo, mDNSfalse);	// we set defer to true since several events that may generate restarts often arrive in rapid succession, and this cuts unnecessary packets
					}
				else if (llqInfo->state == LLQ_SuspendDeferred)
					llqInfo->state = LLQ_GetZoneInfo; // we never finished getting zone data - proceed as usual
				else if (llqInfo->state == LLQ_SuspendedPoll)
					{
					// if we were polling, we may have had bad zone data due to firewall, etc. - refetch
					llqInfo->ntries = 0;
					llqInfo->state = LLQ_GetZoneInfo;
					StartGetZoneData(m, &q->qname, lookupLLQSRV, startLLQHandshakeCallback, llqInfo);
					}
				}
			else { q->LastQTime = timenow; q->ThisQInterval = INIT_UCAST_POLL_INTERVAL; } // trigger poll in 1 second (to reduce packet rate when restarts come in rapid succession)
			}
		}
	m->CurrentQuestion = mDNSNULL;
	}

mDNSexport void mDNS_UpdateLLQs(mDNS *m)
	{
	mDNS_Lock(m);
	SuspendLLQs(m, mDNStrue);
	RestartQueries(m);
	mDNS_Unlock(m);
	}

// simplest sleep logic - rather than having sleep states that must be dealt with explicitly in all parts of
// the code, we simply send a deregistration, and put the service in Refresh state, with a timeout far enough
// in the future that we'll sleep (or the sleep will be cancelled) before it is retransmitted.  Then to wake,
// we just move up the timers.

mDNSlocal void SleepRecordRegistrations(mDNS *m)
	{
	DNSMessage msg;
	AuthRecord *rr = m->RecordRegistrations;
	mDNSs32 timenow = m->timenow;

	while (rr)
		{
		if (rr->state == regState_Registered ||
			rr->state == regState_Refresh)
			{
			mDNSu8 *ptr = msg.data, *end = (mDNSu8 *)&msg + sizeof(DNSMessage);
			InitializeDNSMessage(&msg.h, mDNS_NewMessageID(m), UpdateReqFlags);

			// construct deletion update
			ptr = putZone(&msg, ptr, end, &rr->zone, mDNSOpaque16fromIntVal(rr->resrec.rrclass));
			if (!ptr) { LogMsg("Error: SleepRecordRegistrations - could not put zone"); return; }
			ptr = putDeletionRecord(&msg, ptr, &rr->resrec);
			if (!ptr) {  LogMsg("Error: SleepRecordRegistrations - could not put deletion record"); return; }

			mDNSSendDNSMessage(m, &msg, ptr, mDNSInterface_Any, &rr->UpdateServer, rr->UpdatePort, mDNSNULL, GetAuthInfoForName(m, rr->resrec.name));
			rr->state = regState_Refresh;
			rr->LastAPTime = timenow;
			rr->ThisAPInterval = 300 * mDNSPlatformOneSecond;
			}
		rr = rr->next;
		}
	}

mDNSlocal void WakeRecordRegistrations(mDNS *m)
	{
	mDNSs32 timenow = m->timenow;
	AuthRecord *rr = m->RecordRegistrations;

	while (rr)
		{
		if (rr->state == regState_Refresh)
			{
			// trigger slightly delayed refresh (we usually get this message before kernel is ready to send packets)
			rr->LastAPTime = timenow;
			rr->ThisAPInterval = INIT_UCAST_POLL_INTERVAL;
			}
		rr = rr->next;
		}
	}

mDNSlocal void SleepServiceRegistrations(mDNS *m)
	{
	ServiceRecordSet *srs = m->ServiceRegistrations;
	while(srs)
		{
		ServiceRecordSet *info = srs;
		NATTraversalInfo *nat = info->NATinfo;

		if (nat)
			{
			if (nat->state == NATState_Established || nat->state == NATState_Refresh || nat->state == NATState_Legacy)
				uDNS_DeleteNATPortMapping(m, nat);
			nat->reg.ServiceRegistration = mDNSNULL;
			srs->NATinfo = mDNSNULL;
			uDNS_FreeNATInfo(m, nat);
			}

		if (info->state == regState_UpdatePending)
			{
			// act as if the update succeeded, since we're about to delete the name anyway
			AuthRecord *txt = &srs->RR_TXT;
			info->state = regState_Registered;
			// deallocate old RData
			if (txt->UpdateCallback) txt->UpdateCallback(m, txt, txt->OrigRData);
			SetNewRData(&txt->resrec, txt->InFlightRData, txt->InFlightRDLen);
			txt->OrigRData = mDNSNULL;
			txt->InFlightRData = mDNSNULL;
			}

		if (info->state == regState_Registered || info->state == regState_Refresh)
			{
			mDNSOpaque16 origid  = srs->id;
			info->state = regState_DeregPending;  // state expected by SendDereg()
			SendServiceDeregistration(m, srs);
			info->id = origid;
			info->state = regState_NoTarget;  // when we wake, we'll re-register (and optionally nat-map) once our address record completes
			srs->RR_SRV.resrec.rdata->u.srv.target.c[0] = 0;
			}
		srs = srs->next;
		}
	}

mDNSlocal void WakeServiceRegistrations(mDNS *m)
	{
	mDNSs32 timenow = m->timenow;
	ServiceRecordSet *srs = m->ServiceRegistrations;
	while(srs)
		{
		if (srs->state == regState_Refresh)
			{
			// trigger slightly delayed refresh (we usually get this message before kernel is ready to send packets)
			srs->RR_SRV.LastAPTime = timenow;
			srs->RR_SRV.ThisAPInterval = INIT_UCAST_POLL_INTERVAL;
			}
		srs = srs->next;
		}
	}

mDNSlocal void MarkSearchListElem(domainname *domain)
	{
	SearchListElem *new, *ptr;

	// if domain is in list, mark as pre-existent (0)
	for (ptr = SearchList; ptr; ptr = ptr->next)
		if (SameDomainName(&ptr->domain, domain))
			{
			if (ptr->flag != 1) ptr->flag = 0;  // gracefully handle duplicates - if it is already marked as add, don't bump down to preexistent
			break;
			}

	// if domain not in list, add to list, mark as add (1)
	if (!ptr)
		{
		new = mDNSPlatformMemAllocate(sizeof(SearchListElem));
		if (!new) { LogMsg("ERROR: MarkSearchListElem - malloc"); return; }
		mDNSPlatformMemZero(new, sizeof(SearchListElem));
		AssignDomainName(&new->domain, domain);
		new->flag = 1;  // add
		new->next = SearchList;
		SearchList = new;
		}
	}

mDNSlocal void DynDNSHostNameCallback(mDNS *const m, AuthRecord *const rr, mStatus result)
	{
	(void)m;  // unused
	debugf("NameStatusCallback: result %d for registration of name %##s", result, rr->resrec.name->c);
	mDNSPlatformDynDNSHostNameStatusChanged(rr->resrec.name, result);
	}

mDNSlocal void FreeARElemCallback(mDNS *const m, AuthRecord *const rr, mStatus result)
	{
	(void)m;  // unused
	if (result == mStatus_MemFree) mDNSPlatformMemFree(rr->RecordContext);
	}

mDNSlocal void FoundDomain(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, mDNSBool AddRecord)
	{
	SearchListElem *slElem = question->QuestionContext;
	ARListElem *arElem, *ptr, *prev;
    AuthRecord *dereg;
	const char *name;
	mStatus err;

	if (AddRecord)
		{
		arElem = mDNSPlatformMemAllocate(sizeof(ARListElem));
		if (!arElem) { LogMsg("ERROR: malloc");  return; }
		mDNS_SetupResourceRecord(&arElem->ar, mDNSNULL, mDNSInterface_LocalOnly, kDNSType_PTR, 7200,  kDNSRecordTypeShared, FreeARElemCallback, arElem);
		if      (question == &slElem->BrowseQ)       name = mDNS_DomainTypeNames[mDNS_DomainTypeBrowse];
		else if (question == &slElem->DefBrowseQ)    name = mDNS_DomainTypeNames[mDNS_DomainTypeBrowseDefault];
		else if (question == &slElem->LegacyBrowseQ) name = mDNS_DomainTypeNames[mDNS_DomainTypeBrowseLegacy];
		else if (question == &slElem->RegisterQ)     name = mDNS_DomainTypeNames[mDNS_DomainTypeRegistration];
		else if (question == &slElem->DefRegisterQ)  name = mDNS_DomainTypeNames[mDNS_DomainTypeRegistrationDefault];
		else { LogMsg("FoundDomain - unknown question"); return; }

		MakeDomainNameFromDNSNameString(arElem->ar.resrec.name, name);
		AppendDNSNameString            (arElem->ar.resrec.name, "local");
		AssignDomainName(&arElem->ar.resrec.rdata->u.name, &answer->rdata->u.name);
		err = mDNS_Register(m, &arElem->ar);
		if (err)
			{
			LogMsg("ERROR: FoundDomain - mDNS_Register returned %d", err);
			mDNSPlatformMemFree(arElem);
			return;
			}
		arElem->next = slElem->AuthRecs;
		slElem->AuthRecs = arElem;
		}
	else
		{
		ptr = slElem->AuthRecs;
		prev = mDNSNULL;
		while (ptr)
			{
			if (SameDomainName(&ptr->ar.resrec.rdata->u.name, &answer->rdata->u.name))
				{
				debugf("Deregistering PTR %##s -> %##s", ptr->ar.resrec.name->c, ptr->ar.resrec.rdata->u.name.c);
                dereg = &ptr->ar;
				if (prev) prev->next = ptr->next;
				else slElem->AuthRecs = ptr->next;
                ptr = ptr->next;
				err = mDNS_Deregister(m, dereg);
				if (err) LogMsg("ERROR: FoundDomain - mDNS_Deregister returned %d", err);
				}
			else
				{
				prev = ptr;
				ptr = ptr->next;
				}
			}
		}
	}

mDNSlocal mStatus RegisterNameServers(mDNS *const m)
	{
	IPAddrListElem	* list;
	IPAddrListElem	* elem;

	mDNS_DeleteDNSServers(m); // deregister orig list

	list = mDNSPlatformGetDNSServers();

	for (elem = list; elem; elem = elem->next)
		{
		LogOperation("RegisterNameServers: Adding %#a", &elem->addr);
		mDNS_AddDNSServer(m, &elem->addr, mDNSNULL);
		}

	mDNS_FreeIPAddrList(list);

	return mStatus_NoError;
	}

// This should probably move to the UDS daemon -- the concept of legacy clients and automatic registration / automatic browsing
// is really a UDS API issue, not something intrinsic to uDNS

mDNSlocal mStatus RegisterSearchDomains(mDNS *const m)
	{
	SearchListElem *ptr, *prev, *freeSLPtr;
	DNameListElem	*	elem;
	DNameListElem	*	list;
	ARListElem *arList;
	mStatus err;
	mDNSBool dict = 1;

	// step 1: mark each elem for removal (-1), unless we aren't passed a dictionary in which case we mark as preexistent
	for (ptr = SearchList; ptr; ptr = ptr->next)
		{
		ptr->flag = dict ? -1 : 0;
		}

	// get all the domains from the platform layer
	list = mDNSPlatformGetSearchDomainList();

	for (elem = list; elem; elem = elem->next)
		{
		MarkSearchListElem(&elem->name);
		}

	mDNS_FreeDNameList(list);

	list = mDNSPlatformGetFQDN();

	if (list)
		{
		MarkSearchListElem(&list->name);
		mDNS_FreeDNameList(list);
		}

	list = mDNSPlatformGetReverseMapSearchDomainList();

	for (elem = list; elem; elem = elem->next)
		{
		MarkSearchListElem(&elem->name);
		}

	mDNS_FreeDNameList(list);

	if (m->RegDomain.c[0])
		{
		MarkSearchListElem(&m->RegDomain);         // implicitly browse reg domain too (no-op if same as BrowseDomain)
		}

	// delete elems marked for removal, do queries for elems marked add
	prev = mDNSNULL;
	ptr = SearchList;
	while (ptr)
		{
		if (ptr->flag == -1)  // remove
			{
			mDNS_StopQuery(m, &ptr->BrowseQ);
			mDNS_StopQuery(m, &ptr->RegisterQ);
			mDNS_StopQuery(m, &ptr->DefBrowseQ);
			mDNS_StopQuery(m, &ptr->DefRegisterQ);
			mDNS_StopQuery(m, &ptr->LegacyBrowseQ);

            // deregister records generated from answers to the query
			arList = ptr->AuthRecs;
			ptr->AuthRecs = mDNSNULL;
			while (arList)
				{
				AuthRecord *dereg = &arList->ar;
				arList = arList->next;
				debugf("Deregistering PTR %##s -> %##s", dereg->resrec.name->c, dereg->resrec.rdata->u.name.c);
				err = mDNS_Deregister(m, dereg);
				if (err) LogMsg("ERROR: RegisterSearchDomains mDNS_Deregister returned %d", err);
				}

			// remove elem from list, delete
			if (prev) prev->next = ptr->next;
			else SearchList = ptr->next;
			freeSLPtr = ptr;
			ptr = ptr->next;
			mDNSPlatformMemFree(freeSLPtr);
			continue;
			}

		if (ptr->flag == 1)  // add
			{
			mStatus err1, err2, err3, err4, err5;
			err1 = mDNS_GetDomains(m, &ptr->BrowseQ,       mDNS_DomainTypeBrowse,              &ptr->domain, mDNSInterface_Any, FoundDomain, ptr);
			err2 = mDNS_GetDomains(m, &ptr->DefBrowseQ,    mDNS_DomainTypeBrowseDefault,       &ptr->domain, mDNSInterface_Any, FoundDomain, ptr);
			err3 = mDNS_GetDomains(m, &ptr->RegisterQ,     mDNS_DomainTypeRegistration,        &ptr->domain, mDNSInterface_Any, FoundDomain, ptr);
			err4 = mDNS_GetDomains(m, &ptr->DefRegisterQ,  mDNS_DomainTypeRegistrationDefault, &ptr->domain, mDNSInterface_Any, FoundDomain, ptr);
			err5 = mDNS_GetDomains(m, &ptr->LegacyBrowseQ, mDNS_DomainTypeBrowseLegacy,        &ptr->domain, mDNSInterface_Any, FoundDomain, ptr);
			if (err1 || err2 || err3 || err4 || err5)
				LogMsg("GetDomains for domain %##s returned error(s):\n"
					   "%d (mDNS_DomainTypeBrowse)\n"
					   "%d (mDNS_DomainTypeBrowseDefault)\n"
					   "%d (mDNS_DomainTypeRegistration)\n"
					   "%d (mDNS_DomainTypeRegistrationDefault)"
					   "%d (mDNS_DomainTypeBrowseLegacy)\n",
					   ptr->domain.c, err1, err2, err3, err4, err5);
			ptr->flag = 0;
			}

		if (ptr->flag) { LogMsg("RegisterSearchDomains - unknown flag %d.  Skipping.", ptr->flag); }

		prev = ptr;
		ptr = ptr->next;
		}

	return mStatus_NoError;
	}

mDNSexport mStatus uDNS_RegisterSearchDomains(mDNS * const m)
	{
	m->RegisterSearchDomains = mDNStrue;
	return RegisterSearchDomains(m);
	}

// Construction of Default Browse domain list (i.e. when clients pass NULL) is as follows:
// 1) query for b._dns-sd._udp.local on LocalOnly interface
//    (.local manually generated via explicit callback)
// 2) for each search domain (from prefs pane), query for b._dns-sd._udp.<searchdomain>.
// 3) for each result from (2), register LocalOnly PTR record b._dns-sd._udp.local. -> <result>
// 4) result above should generate a callback from question in (1).  result added to global list
// 5) global list delivered to client via GetSearchDomainList()
// 6) client calls to enumerate domains now go over LocalOnly interface
//    (!!!KRS may add outgoing interface in addition)

mDNSexport mStatus uDNS_SetupDNSConfig(mDNS *const m)
	{
	int				nAdditions;
	int				nDeletions;
	mDNSAddr        v4;
	mDNSAddr		v6;
	mDNSAddr        r;
    domainname      fqdn;

	// Let the platform layer get the current DNS information
	mDNSPlatformGetDNSConfig(m, &fqdn, mDNSNULL, mDNSNULL);

	// Did our FQDN change?
	if (!SameDomainName(&fqdn, &m->FQDN))
		{
		if (m->FQDN.c[0])
			{
			mDNS_RemoveDynDNSHostName(m, &m->FQDN);
			}

		AssignDomainName(&m->FQDN, &fqdn);

		if (m->FQDN.c[0])
			{
			mDNSPlatformDynDNSHostNameStatusChanged(&m->FQDN, 1);
			mDNS_AddDynDNSHostName(m, &m->FQDN, DynDNSHostNameCallback, mDNSNULL);
			}
		}

	// handle any changes to search domains and DNS server addresses
	if (mDNSPlatformRegisterSplitDNS(m, &nAdditions, &nDeletions) != mStatus_NoError)
		{
		RegisterNameServers(m);  // fall back to non-split DNS aware configuration on failure
		}

	// This bit of trickery is to ensure that we're lazily calling RegisterSearchDomains.
	// The RegisterSearchDomains boolean is set when we call uDNS_RegisterSearchDomains.
	// This is called in uds_daemon.c
	if (m->RegisterSearchDomains)
		{
		RegisterSearchDomains(m);  // note that we register name servers *before* search domains
		}

	// handle router and primary interface changes
	v4 = v6 = r = zeroAddr;
	v4.type = r.type = mDNSAddrType_IPv4;

	if (mDNSPlatformGetPrimaryInterface(m, &v4, &v6, &r) == mStatus_NoError)
		{
		// handle primary interface changes
        // if we gained or lost DNS servers (e.g. logged into VPN) "toggle" primary address so it gets re-registered even if it is unchanged
		if (nAdditions || nDeletions)
			{
			mDNS_SetPrimaryInterfaceInfo(m, mDNSNULL, mDNSNULL, mDNSNULL);
			}

		if (v4.ip.v4.b[0] != 169 || v4.ip.v4.b[1] != 254)
			{
			mDNS_SetPrimaryInterfaceInfo(m, &v4.ip.v4.b[0] ? &v4 : mDNSNULL, v6.ip.v6.b[0] ? &v6 : mDNSNULL, r.ip.v4.NotAnInteger ? &r : mDNSNULL);
			}
		else
			{
			mDNS_SetPrimaryInterfaceInfo(m, mDNSNULL, mDNSNULL, mDNSNULL);  // primary IP is link-local
			}
		}
	else
		{
		mDNS_SetPrimaryInterfaceInfo(m, mDNSNULL, mDNSNULL, mDNSNULL);

		if (m->FQDN.c[0])
			{
			mDNSPlatformDynDNSHostNameStatusChanged(&m->FQDN, 1);	// Set status to 1 to indicate temporary failure
			}
		}

	return mStatus_NoError;
	}

mDNSexport void uDNS_Sleep(mDNS *const m)
	{
	SuspendLLQs(m, mDNStrue);
	SleepServiceRegistrations(m);
	SleepRecordRegistrations(m);
	}

mDNSexport void uDNS_Wake(mDNS *const m)
	{
	RestartQueries(m);
	WakeServiceRegistrations(m);
	WakeRecordRegistrations(m);
	}
