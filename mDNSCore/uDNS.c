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
 */

#include "uDNS.h"

#if(defined(_MSC_VER))
	// Disable "assignment within conditional expression".
	// Other compilers understand the convention that if you place the assignment expression within an extra pair
	// of parentheses, this signals to the compiler that you really intended an assignment and no warning is necessary.
	// The Microsoft compiler doesn't understand this convention, so in the absense of any other way to signal
	// to the compiler that the assignment is intentional, we have to just turn this warning off completely.
	#pragma warning(disable:4706)
#endif

// For domain enumeration and automatic browsing
// This is the user's DNS search list.
// In each of these domains we search for our special pointer records (lb._dns-sd._udp.<domain>, etc.)
// to discover recommended domains for domain enumeration (browse, default browse, registration,
// default registration) and possibly one or more recommended automatic browsing domains.
mDNSexport SearchListElem *SearchList = mDNSNULL;

// Temporary workaround to make ServiceRecordSet list management safe.
// Ideally a ServiceRecordSet shouldn't be a special entity that's given special treatment by the uDNS code
// -- it should just be a grouping of records that are treated the same as any other registered records.
// In that case it may no longer be necessary to keep an explicit list of ServiceRecordSets, which in turn
// would avoid the perils of modifying that list cleanly while some other piece of code is iterating through it.
ServiceRecordSet *CurrentServiceRecordSet = mDNSNULL;

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - General Utility Functions
#endif

// Unlink an AuthRecord from the m->ResourceRecords list.
// This seems risky. Probably some (or maybe all) of the places calling UnlinkAuthRecord to directly
// remove a record from the list should actually be using mDNS_Deregister/mDNS_Deregister_internal.
mDNSlocal mStatus UnlinkAuthRecord(mDNS *const m, AuthRecord *const rr)
	{
	AuthRecord **list = &m->ResourceRecords;
	if (m->NewLocalRecords == rr) m->NewLocalRecords = rr->next;
	if (m->CurrentRecord == rr) m->CurrentRecord = rr->next;
	while (*list && *list != rr) list = &(*list)->next;
	if (!*list)
		{
		list = &m->DuplicateRecords;
		while (*list && *list != rr) list = &(*list)->next;
		}
	if (*list) { *list = rr->next; rr->next = mDNSNULL; return(mStatus_NoError); }
	LogMsg("ERROR: UnlinkAuthRecord - no such active record %##s", rr->resrec.name->c);
	return(mStatus_NoSuchRecord);
	}

// unlinkSRS is an internal routine (i.e. must be called with the lock already held)
mDNSlocal void unlinkSRS(mDNS *const m, ServiceRecordSet *srs)
	{
	ServiceRecordSet **p;

	if (srs->NATinfo.clientContext)
		{
		mDNS_StopNATOperation_internal(m, &srs->NATinfo);
		srs->NATinfo.clientContext = mDNSNULL;
		}

	for (p = &m->ServiceRegistrations; *p; p = &(*p)->uDNS_next)
		if (*p == srs)
			{
			ExtraResourceRecord *e;
			*p = srs->uDNS_next;
			if (CurrentServiceRecordSet == srs)
				CurrentServiceRecordSet = srs->uDNS_next;
			srs->uDNS_next = mDNSNULL;
			for (e=srs->Extras; e; e=e->next)
				if (UnlinkAuthRecord(m, &e->r))
					LogMsg("unlinkSRS: extra record %##s not found", e->r.resrec.name->c);
			return;
			}
	LogMsg("ERROR: unlinkSRS - SRS not found in ServiceRegistrations list %##s", srs->RR_SRV.resrec.name->c);
	}

// set retry timestamp for record with exponential backoff
// (for service record sets, use RR_SRV as representative for time checks
mDNSlocal void SetRecordRetry(mDNS *const m, AuthRecord *rr, mStatus SendErr)
	{
	mDNSs32 elapsed = m->timenow - rr->LastAPTime;
	rr->LastAPTime = m->timenow;

#if 0
	// Code for stress-testing registration renewal code
	if (rr->expire && rr->expire - m->timenow > mDNSPlatformOneSecond * 120)
		{
		LogInfo("Adjusting expiry from %d to 120 seconds for %s",
			(rr->expire - m->timenow) / mDNSPlatformOneSecond, ARDisplayString(m, rr));
		rr->expire = m->timenow + mDNSPlatformOneSecond * 120;
		}
#endif

	if (rr->expire && rr->expire - m->timenow > mDNSPlatformOneSecond)
		{
		mDNSs32 remaining = rr->expire - m->timenow;
		rr->ThisAPInterval = remaining/2 + mDNSRandom(remaining/10);
		debugf("SetRecordRetry refresh in %4d of %4d for %s",
			rr->ThisAPInterval / mDNSPlatformOneSecond, (rr->expire - m->timenow) / mDNSPlatformOneSecond, ARDisplayString(m, rr));
		return;
		}

	rr->expire = 0;

	// If at least half our our time interval has elapsed, it's time to double rr->ThisAPInterval
	// If resulting interval is too small, set to at least INIT_UCAST_POLL_INTERVAL (3 seconds)
	// If resulting interval is too large, set to at most 30 minutes
	if (rr->ThisAPInterval / 2 <= elapsed) rr->ThisAPInterval *= 2;
	if (rr->ThisAPInterval < INIT_UCAST_POLL_INTERVAL || SendErr == mStatus_TransientErr)
		rr->ThisAPInterval = INIT_UCAST_POLL_INTERVAL;
	rr->ThisAPInterval += mDNSRandom(rr->ThisAPInterval/20);
	if (rr->ThisAPInterval > 30 * 60 * mDNSPlatformOneSecond)
		rr->ThisAPInterval = 30 * 60 * mDNSPlatformOneSecond;

	LogInfo("SetRecordRetry retry   in %4d for %s", rr->ThisAPInterval / mDNSPlatformOneSecond, ARDisplayString(m, rr));
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Name Server List Management
#endif

mDNSexport DNSServer *mDNS_AddDNSServer(mDNS *const m, const domainname *d, const mDNSInterfaceID interface, const mDNSAddr *addr, const mDNSIPPort port)
	{
	DNSServer **p = &m->DNSServers;
	DNSServer *tmp = mDNSNULL;
	
	if (!d) d = (const domainname *)"";

	LogInfo("mDNS_AddDNSServer: Adding %#a for %##s", addr, d->c);
	if (m->mDNS_busy != m->mDNS_reentrancy+1)
		LogMsg("mDNS_AddDNSServer: Lock not held! mDNS_busy (%ld) mDNS_reentrancy (%ld)", m->mDNS_busy, m->mDNS_reentrancy);

	while (*p)	// Check if we already have this {interface,address,port,domain} tuple registered
		{
		if ((*p)->interface == interface && (*p)->teststate != DNSServer_Disabled &&
			mDNSSameAddress(&(*p)->addr, addr) && mDNSSameIPPort((*p)->port, port) && SameDomainName(&(*p)->domain, d))
			{
			if (!((*p)->flags & DNSServer_FlagDelete)) debugf("Note: DNS Server %#a:%d for domain %##s (%p) registered more than once", addr, mDNSVal16(port), d->c, interface);
			(*p)->flags &= ~DNSServer_FlagDelete;
			tmp = *p;
			*p = tmp->next;
			tmp->next = mDNSNULL;
			}
		else
			p=&(*p)->next;
		}

	if (tmp) *p = tmp; // move to end of list, to ensure ordering from platform layer
	else
		{
		// allocate, add to list
		*p = mDNSPlatformMemAllocate(sizeof(**p));
		if (!*p) LogMsg("Error: mDNS_AddDNSServer - malloc");
		else
			{
			(*p)->interface = interface;
			(*p)->addr      = *addr;
			(*p)->port      = port;
			(*p)->flags     = DNSServer_FlagNew;
			(*p)->teststate = /* DNSServer_Untested */ DNSServer_Passed;
			(*p)->lasttest  = m->timenow - INIT_UCAST_POLL_INTERVAL;
			AssignDomainName(&(*p)->domain, d);
			(*p)->next = mDNSNULL;
			}
		}
	return(*p);
	}

mDNSexport void PushDNSServerToEnd(mDNS *const m, DNSQuestion *q)
	{
	DNSServer *orig = q->qDNSServer;
	DNSServer **p = &m->DNSServers;
	
	if (m->mDNS_busy != m->mDNS_reentrancy+1)
		LogMsg("PushDNSServerToEnd: Lock not held! mDNS_busy (%ld) mDNS_reentrancy (%ld)", m->mDNS_busy, m->mDNS_reentrancy);

	if (!q->qDNSServer)
		{
		LogMsg("PushDNSServerToEnd: Null DNS server for %##s (%s) %d", q->qname.c, DNSTypeName(q->qtype), q->unansweredQueries);
		goto end;
		}

	LogInfo("PushDNSServerToEnd: Pushing DNS server %#a:%d (%##s) due to %d unanswered queries for %##s (%s)",
		&q->qDNSServer->addr, mDNSVal16(q->qDNSServer->port), q->qDNSServer->domain.c, q->unansweredQueries, q->qname.c, DNSTypeName(q->qtype));

	while (*p)
		{
		if (*p == q->qDNSServer) *p = q->qDNSServer->next;
		else p=&(*p)->next;
		}

	*p = q->qDNSServer;
	q->qDNSServer->next = mDNSNULL;

end:
	q->qDNSServer = GetServerForName(m, &q->qname);

	if (q->qDNSServer != orig)
		{
		if (q->qDNSServer) LogInfo("PushDNSServerToEnd: Server for %##s (%s) changed to %#a:%d (%##s)", q->qname.c, DNSTypeName(q->qtype), &q->qDNSServer->addr, mDNSVal16(q->qDNSServer->port), q->qDNSServer->domain.c);
		else               LogInfo("PushDNSServerToEnd: Server for %##s (%s) changed to <null>",        q->qname.c, DNSTypeName(q->qtype));
		q->ThisQInterval = q->ThisQInterval / QuestionIntervalStep; // Decrease interval one step so we don't quickly bounce between servers for queries that will not be answered.
		}
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - authorization management
#endif

mDNSlocal DomainAuthInfo *GetAuthInfoForName_direct(mDNS *m, const domainname *const name)
	{
	const domainname *n = name;
	while (n->c[0])
		{
		DomainAuthInfo *ptr;
		for (ptr = m->AuthInfoList; ptr; ptr = ptr->next)
			if (SameDomainName(&ptr->domain, n))
				{
				debugf("GetAuthInfoForName %##s Matched %##s Key name %##s", name->c, ptr->domain.c, ptr->keyname.c);
				return(ptr);
				}
		n = (const domainname *)(n->c + 1 + n->c[0]);
		}
	//LogInfo("GetAuthInfoForName none found for %##s", name->c);
	return mDNSNULL;
	}

// MUST be called with lock held
mDNSexport DomainAuthInfo *GetAuthInfoForName_internal(mDNS *m, const domainname *const name)
	{
	DomainAuthInfo **p = &m->AuthInfoList;

	if (m->mDNS_busy != m->mDNS_reentrancy+1)
		LogMsg("GetAuthInfoForName_internal: Lock not held! mDNS_busy (%ld) mDNS_reentrancy (%ld)", m->mDNS_busy, m->mDNS_reentrancy);

	// First purge any dead keys from the list
	while (*p)
		{
		if ((*p)->deltime && m->timenow - (*p)->deltime >= 0 && AutoTunnelUnregistered(*p))
			{
			DNSQuestion *q;
			DomainAuthInfo *info = *p;
			LogInfo("GetAuthInfoForName_internal deleting expired key %##s %##s", info->domain.c, info->keyname.c);
			*p = info->next;	// Cut DomainAuthInfo from list *before* scanning our question list updating AuthInfo pointers
			for (q = m->Questions; q; q=q->next)
				if (q->AuthInfo == info)
					{
					q->AuthInfo = GetAuthInfoForName_direct(m, &q->qname);
					debugf("GetAuthInfoForName_internal updated q->AuthInfo from %##s to %##s for %##s (%s)",
						info->domain.c, q->AuthInfo ? q->AuthInfo->domain.c : mDNSNULL, q->qname.c, DNSTypeName(q->qtype));
					}

			// Probably not essential, but just to be safe, zero out the secret key data
			// so we don't leave it hanging around in memory
			// (where it could potentially get exposed via some other bug)
			mDNSPlatformMemZero(info, sizeof(*info));
			mDNSPlatformMemFree(info);
			}
		else
			p = &(*p)->next;
		}

	return(GetAuthInfoForName_direct(m, name));
	}

mDNSexport DomainAuthInfo *GetAuthInfoForName(mDNS *m, const domainname *const name)
	{
	DomainAuthInfo *d;
	mDNS_Lock(m);
	d = GetAuthInfoForName_internal(m, name);
	mDNS_Unlock(m);
	return(d);
	}

// MUST be called with the lock held
mDNSexport mStatus mDNS_SetSecretForDomain(mDNS *m, DomainAuthInfo *info,
	const domainname *domain, const domainname *keyname, const char *b64keydata, mDNSBool AutoTunnel)
	{
	DNSQuestion *q;
	DomainAuthInfo **p = &m->AuthInfoList;
	if (!info || !b64keydata) { LogMsg("mDNS_SetSecretForDomain: ERROR: info %p b64keydata %p", info, b64keydata); return(mStatus_BadParamErr); }

	LogInfo("mDNS_SetSecretForDomain: domain %##s key %##s%s", domain->c, keyname->c, AutoTunnel ? " AutoTunnel" : "");

	info->AutoTunnel = AutoTunnel;
	AssignDomainName(&info->domain,  domain);
	AssignDomainName(&info->keyname, keyname);
	mDNS_snprintf(info->b64keydata, sizeof(info->b64keydata), "%s", b64keydata);

	if (DNSDigest_ConstructHMACKeyfromBase64(info, b64keydata) < 0)
		{
		LogMsg("mDNS_SetSecretForDomain: ERROR: Could not convert shared secret from base64: domain %##s key %##s %s", domain->c, keyname->c, mDNS_LoggingEnabled ? b64keydata : "");
		return(mStatus_BadParamErr);
		}

	// Don't clear deltime until after we've ascertained that b64keydata is valid
	info->deltime = 0;

	while (*p && (*p) != info) p=&(*p)->next;
	if (*p) return(mStatus_AlreadyRegistered);

	// Caution: Only zero AutoTunnelHostRecord.namestorage and AutoTunnelNAT.clientContext AFTER we've determined that this is a NEW DomainAuthInfo
	// being added to the list. Otherwise we risk smashing our AutoTunnel host records and NATOperation that are already active and in use.
	info->AutoTunnelHostRecord.resrec.RecordType = kDNSRecordTypeUnregistered;
	info->AutoTunnelHostRecord.namestorage.c[0] = 0;
	info->AutoTunnelTarget    .resrec.RecordType = kDNSRecordTypeUnregistered;
	info->AutoTunnelDeviceInfo.resrec.RecordType = kDNSRecordTypeUnregistered;
	info->AutoTunnelService   .resrec.RecordType = kDNSRecordTypeUnregistered;
	info->AutoTunnelNAT.clientContext = mDNSNULL;
	info->next = mDNSNULL;
	*p = info;

	// Check to see if adding this new DomainAuthInfo has changed the credentials for any of our questions
	for (q = m->Questions; q; q=q->next)
		{
		DomainAuthInfo *newinfo = GetAuthInfoForQuestion(m, q);
		if (q->AuthInfo != newinfo)
			{
			debugf("mDNS_SetSecretForDomain updating q->AuthInfo from %##s to %##s for %##s (%s)",
				q->AuthInfo ? q->AuthInfo->domain.c : mDNSNULL,
				newinfo     ? newinfo    ->domain.c : mDNSNULL, q->qname.c, DNSTypeName(q->qtype));
			q->AuthInfo = newinfo;
			}
		}

	return(mStatus_NoError);
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - NAT Traversal
#endif

mDNSlocal mStatus uDNS_SendNATMsg(mDNS *m, NATTraversalInfo *info)
	{
	mStatus err = mStatus_NoError;

	// send msg if we have a router and it is a private address
	if (!mDNSIPv4AddressIsZero(m->Router.ip.v4) && mDNSv4AddrIsRFC1918(&m->Router.ip.v4))
		{
		union { NATAddrRequest NATAddrReq; NATPortMapRequest NATPortReq; } u = { { NATMAP_VERS, NATOp_AddrRequest } } ;
		const mDNSu8 *end = (mDNSu8 *)&u + sizeof(NATAddrRequest);
	
		if (info)			// For NATOp_MapUDP and NATOp_MapTCP, fill in additional fields
			{
			mDNSu8 *p = (mDNSu8 *)&u.NATPortReq.NATReq_lease;
			u.NATPortReq.opcode  = info->Protocol;
			u.NATPortReq.unused  = zeroID;
			u.NATPortReq.intport = info->IntPort;
			u.NATPortReq.extport = info->RequestedPort;
			p[0] = (mDNSu8)((info->NATLease >> 24) &  0xFF);
			p[1] = (mDNSu8)((info->NATLease >> 16) &  0xFF);
			p[2] = (mDNSu8)((info->NATLease >>  8) &  0xFF);
			p[3] = (mDNSu8)( info->NATLease        &  0xFF);
			end = (mDNSu8 *)&u + sizeof(NATPortMapRequest);
			}

		err = mDNSPlatformSendUDP(m, (mDNSu8 *)&u, end, 0, mDNSNULL, &m->Router, NATPMPPort);

#ifdef _LEGACY_NAT_TRAVERSAL_
		if (mDNSIPPortIsZero(m->UPnPRouterPort) || mDNSIPPortIsZero(m->UPnPSOAPPort)) LNT_SendDiscoveryMsg(m);
		else if (info) err = LNT_MapPort(m, info);
		else err = LNT_GetExternalAddress(m);
#endif // _LEGACY_NAT_TRAVERSAL_
		}
	return(err);
	}

mDNSexport void RecreateNATMappings(mDNS *const m)
	{
	NATTraversalInfo *n;
	for (n = m->NATTraversals; n; n=n->next)
		{
		n->ExpiryTime    = 0;		// Mark this mapping as expired
		n->retryInterval = NATMAP_INIT_RETRY;
		n->retryPortMap  = m->timenow;
#ifdef _LEGACY_NAT_TRAVERSAL_
		if (n->tcpInfo.sock) { mDNSPlatformTCPCloseConnection(n->tcpInfo.sock); n->tcpInfo.sock = mDNSNULL; }
#endif // _LEGACY_NAT_TRAVERSAL_
		}

	m->NextScheduledNATOp = m->timenow;		// Need to send packets immediately
	}

mDNSexport void natTraversalHandleAddressReply(mDNS *const m, mDNSu16 err, mDNSv4Addr ExtAddr)
	{
	static mDNSu16 last_err = 0;
	
	if (err)
		{
		if (err != last_err) LogMsg("Error getting external address %d", err);
		ExtAddr = zerov4Addr;
		}
	else
		{
		LogInfo("Received external IP address %.4a from NAT", &ExtAddr);
		if (mDNSv4AddrIsRFC1918(&ExtAddr))
			LogMsg("Double NAT (external NAT gateway address %.4a is also a private RFC 1918 address)", &ExtAddr);
		if (mDNSIPv4AddressIsZero(ExtAddr))
			err = NATErr_NetFail; // fake error to handle routers that pathologically report success with the zero address
		}
		
	if (!mDNSSameIPv4Address(m->ExternalAddress, ExtAddr))
		{
		m->ExternalAddress = ExtAddr;
		RecreateNATMappings(m);		// Also sets NextScheduledNATOp for us
		}

	if (!err) // Success, back-off to maximum interval
		m->retryIntervalGetAddr = NATMAP_MAX_RETRY_INTERVAL;
	else if (!last_err) // Failure after success, retry quickly (then back-off exponentially)
		m->retryIntervalGetAddr = NATMAP_INIT_RETRY;
	// else back-off normally in case of pathological failures

	m->retryGetAddr = m->timenow + m->retryIntervalGetAddr;
	if (m->NextScheduledNATOp - m->retryIntervalGetAddr > 0)
		m->NextScheduledNATOp = m->retryIntervalGetAddr;

	last_err = err;
	}

// Both places that call NATSetNextRenewalTime() update m->NextScheduledNATOp correctly afterwards
mDNSlocal void NATSetNextRenewalTime(mDNS *const m, NATTraversalInfo *n)
	{
	n->retryInterval = (n->ExpiryTime - m->timenow)/2;
	if (n->retryInterval < NATMAP_MIN_RETRY_INTERVAL)	// Min retry interval is 2 seconds
		n->retryInterval = NATMAP_MIN_RETRY_INTERVAL;
	n->retryPortMap = m->timenow + n->retryInterval;
	}

// Note: When called from handleLNTPortMappingResponse() only pkt->err, pkt->extport and pkt->NATRep_lease fields are filled in
mDNSexport void natTraversalHandlePortMapReply(mDNS *const m, NATTraversalInfo *n, const mDNSInterfaceID InterfaceID, mDNSu16 err, mDNSIPPort extport, mDNSu32 lease)
	{
	const char *prot = n->Protocol == NATOp_MapUDP ? "UDP" : n->Protocol == NATOp_MapTCP ? "TCP" : "?";
	(void)prot;
	n->NewResult = err;
	if (err || lease == 0 || mDNSIPPortIsZero(extport))
		{
		LogInfo("natTraversalHandlePortMapReply: %p Response %s Port %5d External Port %5d lease %d error %d",
			n, prot, mDNSVal16(n->IntPort), mDNSVal16(extport), lease, err);
		n->retryInterval = NATMAP_MAX_RETRY_INTERVAL;
		n->retryPortMap = m->timenow + NATMAP_MAX_RETRY_INTERVAL;
		// No need to set m->NextScheduledNATOp here, since we're only ever extending the m->retryPortMap time
		if      (err == NATErr_Refused)                     n->NewResult = mStatus_NATPortMappingDisabled;
		else if (err > NATErr_None && err <= NATErr_Opcode) n->NewResult = mStatus_NATPortMappingUnsupported;
		}
	else
		{
		if (lease > 999999999UL / mDNSPlatformOneSecond)
			lease = 999999999UL / mDNSPlatformOneSecond;
		n->ExpiryTime = NonZeroTime(m->timenow + lease * mDNSPlatformOneSecond);
	
		if (!mDNSSameIPPort(n->RequestedPort, extport))
			LogInfo("natTraversalHandlePortMapReply: %p Response %s Port %5d External Port %5d changed to %5d",
				n, prot, mDNSVal16(n->IntPort), mDNSVal16(n->RequestedPort), mDNSVal16(extport));

		n->InterfaceID   = InterfaceID;
		n->RequestedPort = extport;
	
		LogInfo("natTraversalHandlePortMapReply: %p Response %s Port %5d External Port %5d lease %d",
			n, prot, mDNSVal16(n->IntPort), mDNSVal16(extport), lease);
	
		NATSetNextRenewalTime(m, n);			// Got our port mapping; now set timer to renew it at halfway point
		m->NextScheduledNATOp = m->timenow;		// May need to invoke client callback immediately
		}
	}

// Must be called with the mDNS_Lock held
mDNSexport mStatus mDNS_StartNATOperation_internal(mDNS *const m, NATTraversalInfo *traversal)
	{
	NATTraversalInfo **n;
	
	LogInfo("mDNS_StartNATOperation_internal Protocol %d IntPort %d RequestedPort %d NATLease %d",
		traversal->Protocol, mDNSVal16(traversal->IntPort), mDNSVal16(traversal->RequestedPort), traversal->NATLease);

	// Note: It important that new traversal requests are appended at the *end* of the list, not prepended at the start
	for (n = &m->NATTraversals; *n; n=&(*n)->next)
		{
		if (traversal == *n)
			{
			LogMsg("Error! Tried to add a NAT traversal that's already in the active list: request %p Prot %d Int %d TTL %d",
				traversal, traversal->Protocol, mDNSVal16(traversal->IntPort), traversal->NATLease);
			return(mStatus_AlreadyRegistered);
			}
		if (traversal->Protocol && traversal->Protocol == (*n)->Protocol && mDNSSameIPPort(traversal->IntPort, (*n)->IntPort) &&
			!mDNSSameIPPort(traversal->IntPort, SSHPort))
			LogMsg("Warning: Created port mapping request %p Prot %d Int %d TTL %d "
				"duplicates existing port mapping request %p Prot %d Int %d TTL %d",
				traversal, traversal->Protocol, mDNSVal16(traversal->IntPort), traversal->NATLease,
				*n,        (*n)     ->Protocol, mDNSVal16((*n)     ->IntPort), (*n)     ->NATLease);
		}

	// Initialize necessary fields
	traversal->next            = mDNSNULL;
	traversal->ExpiryTime      = 0;
	traversal->retryInterval   = NATMAP_INIT_RETRY;
	traversal->retryPortMap    = m->timenow;
	traversal->NewResult       = mStatus_NoError;
	traversal->ExternalAddress = onesIPv4Addr;
	traversal->ExternalPort    = zeroIPPort;
	traversal->Lifetime        = 0;
	traversal->Result          = mStatus_NoError;

	// set default lease if necessary
	if (!traversal->NATLease) traversal->NATLease = NATMAP_DEFAULT_LEASE;

#ifdef _LEGACY_NAT_TRAVERSAL_
	mDNSPlatformMemZero(&traversal->tcpInfo, sizeof(traversal->tcpInfo));
#endif // _LEGACY_NAT_TRAVERSAL_

	if (!m->NATTraversals)		// If this is our first NAT request, kick off an address request too
		{
		m->retryGetAddr         = m->timenow;
		m->retryIntervalGetAddr = NATMAP_INIT_RETRY;
		}

	m->NextScheduledNATOp = m->timenow;	// This will always trigger sending the packet ASAP, and generate client callback if necessary

	*n = traversal;		// Append new NATTraversalInfo to the end of our list

	return(mStatus_NoError);
	}

// Must be called with the mDNS_Lock held
mDNSexport mStatus mDNS_StopNATOperation_internal(mDNS *m, NATTraversalInfo *traversal)
	{
	mDNSBool unmap = mDNStrue;
	NATTraversalInfo *p;
	NATTraversalInfo **ptr = &m->NATTraversals;

	while (*ptr && *ptr != traversal) ptr=&(*ptr)->next;
	if (*ptr) *ptr = (*ptr)->next;		// If we found it, cut this NATTraversalInfo struct from our list
	else
		{
		LogMsg("mDNS_StopNATOperation: NATTraversalInfo %p not found in list", traversal);
		return(mStatus_BadReferenceErr);
		}

	LogInfo("mDNS_StopNATOperation_internal %d %d %d %d",
		traversal->Protocol, mDNSVal16(traversal->IntPort), mDNSVal16(traversal->RequestedPort), traversal->NATLease);

	if (m->CurrentNATTraversal == traversal)
		m->CurrentNATTraversal = m->CurrentNATTraversal->next;

	if (traversal->Protocol)
		for (p = m->NATTraversals; p; p=p->next)
			if (traversal->Protocol == p->Protocol && mDNSSameIPPort(traversal->IntPort, p->IntPort))
				{
				if (!mDNSSameIPPort(traversal->IntPort, SSHPort))
					LogMsg("Warning: Removed port mapping request %p Prot %d Int %d TTL %d "
						"duplicates existing port mapping request %p Prot %d Int %d TTL %d",
						traversal, traversal->Protocol, mDNSVal16(traversal->IntPort), traversal->NATLease,
						p,         p        ->Protocol, mDNSVal16(p        ->IntPort), p        ->NATLease);
				unmap = mDNSfalse;
				}

	if (traversal->ExpiryTime && unmap)
		{
		traversal->NATLease = 0;
		traversal->retryInterval = 0;
		uDNS_SendNATMsg(m, traversal);
		}

	// Even if we DIDN'T make a successful UPnP mapping yet, we might still have a partially-open TCP connection we need to clean up
	#ifdef _LEGACY_NAT_TRAVERSAL_
		{
		mStatus err = LNT_UnmapPort(m, traversal);
		if (err) LogMsg("Legacy NAT Traversal - unmap request failed with error %d", err);
		}
	#endif // _LEGACY_NAT_TRAVERSAL_

	return(mStatus_NoError);
	}

mDNSexport mStatus mDNS_StartNATOperation(mDNS *const m, NATTraversalInfo *traversal)
	{
	mStatus status;
	mDNS_Lock(m);
	status = mDNS_StartNATOperation_internal(m, traversal);
	mDNS_Unlock(m);
	return(status);
	}

mDNSexport mStatus mDNS_StopNATOperation(mDNS *const m, NATTraversalInfo *traversal)
	{
	mStatus status;
	mDNS_Lock(m);
	status = mDNS_StopNATOperation_internal(m, traversal);
	mDNS_Unlock(m);
	return(status);
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - Long-Lived Queries
#endif

// Lock must be held -- otherwise m->timenow is undefined
mDNSlocal void StartLLQPolling(mDNS *const m, DNSQuestion *q)
	{
	debugf("StartLLQPolling: %##s", q->qname.c);
	q->state = LLQ_Poll;
	q->ThisQInterval = INIT_UCAST_POLL_INTERVAL;
	// We want to send our poll query ASAP, but the "+ 1" is because if we set the time to now,
	// we risk causing spurious "SendQueries didn't send all its queries" log messages
	q->LastQTime     = m->timenow - q->ThisQInterval + 1;
	SetNextQueryTime(m, q);
#if APPLE_OSX_mDNSResponder
	UpdateAutoTunnelDomainStatuses(m);
#endif
	}

mDNSlocal mDNSu8 *putLLQ(DNSMessage *const msg, mDNSu8 *ptr, const DNSQuestion *const question, const LLQOptData *const data)
	{
	AuthRecord rr;
	ResourceRecord *opt = &rr.resrec;
	rdataOPT *optRD;

	//!!!KRS when we implement multiple llqs per message, we'll need to memmove anything past the question section
	ptr = putQuestion(msg, ptr, msg->data + AbsoluteMaxDNSMessageData, &question->qname, question->qtype, question->qclass);
	if (!ptr) { LogMsg("ERROR: putLLQ - putQuestion"); return mDNSNULL; }

	// locate OptRR if it exists, set pointer to end
	// !!!KRS implement me

	// format opt rr (fields not specified are zero-valued)
	mDNS_SetupResourceRecord(&rr, mDNSNULL, mDNSInterface_Any, kDNSType_OPT, kStandardTTL, kDNSRecordTypeKnownUnique, mDNSNULL, mDNSNULL);
	opt->rrclass    = NormalMaxDNSMessageData;
	opt->rdlength   = sizeof(rdataOPT);	// One option in this OPT record
	opt->rdestimate = sizeof(rdataOPT);

	optRD = &rr.resrec.rdata->u.opt[0];
	optRD->opt = kDNSOpt_LLQ;
	optRD->u.llq = *data;
	ptr = PutResourceRecordTTLJumbo(msg, ptr, &msg->h.numAdditionals, opt, 0);
	if (!ptr) { LogMsg("ERROR: putLLQ - PutResourceRecordTTLJumbo"); return mDNSNULL; }

	return ptr;
	}

// Normally we'd just request event packets be sent directly to m->LLQNAT.ExternalPort, except...
// with LLQs over TLS/TCP we're doing a weird thing where instead of requesting packets be sent to ExternalAddress:ExternalPort
// we're requesting that packets be sent to ExternalPort, but at the source address of our outgoing TCP connection.
// Normally, after going through the NAT gateway, the source address of our outgoing TCP connection is the same as ExternalAddress,
// so this is fine, except when the TCP connection ends up going over a VPN tunnel instead.
// To work around this, if we find that the source address for our TCP connection is not a private address, we tell the Dot Mac
// LLQ server to send events to us directly at port 5353 on that address, instead of at our mapped external NAT port.

mDNSlocal mDNSu16 GetLLQEventPort(const mDNS *const m, const mDNSAddr *const dst)
	{
	mDNSAddr src;
	mDNSPlatformSourceAddrForDest(&src, dst);
	//LogMsg("GetLLQEventPort: src %#a for dst %#a (%d)", &src, dst, mDNSv4AddrIsRFC1918(&src.ip.v4) ? mDNSVal16(m->LLQNAT.ExternalPort) : 0);
	return(mDNSv4AddrIsRFC1918(&src.ip.v4) ? mDNSVal16(m->LLQNAT.ExternalPort) : mDNSVal16(MulticastDNSPort));
	}

// Normally called with llq set.
// May be called with llq NULL, when retransmitting a lost Challenge Response
mDNSlocal void sendChallengeResponse(mDNS *const m, DNSQuestion *const q, const LLQOptData *llq)
	{
	mDNSu8 *responsePtr = m->omsg.data;
	LLQOptData llqBuf;

	if (q->ntries++ == kLLQ_MAX_TRIES)
		{
		LogMsg("sendChallengeResponse: %d failed attempts for LLQ %##s", kLLQ_MAX_TRIES, q->qname.c);
		StartLLQPolling(m,q);
		return;
		}

	if (!llq)		// Retransmission: need to make a new LLQOptData
		{
		llqBuf.vers     = kLLQ_Vers;
		llqBuf.llqOp    = kLLQOp_Setup;
		llqBuf.err      = LLQErr_NoError;	// Don't need to tell server UDP notification port when sending over UDP
		llqBuf.id       = q->id;
		llqBuf.llqlease = q->ReqLease;
		llq = &llqBuf;
		}

	q->LastQTime     = m->timenow;
	q->ThisQInterval = q->tcp ? 0 : (kLLQ_INIT_RESEND * q->ntries * mDNSPlatformOneSecond);		// If using TCP, don't need to retransmit
	SetNextQueryTime(m, q);

	// To simulate loss of challenge response packet, uncomment line below
	//if (q->ntries == 1) return;

	InitializeDNSMessage(&m->omsg.h, q->TargetQID, uQueryFlags);
	responsePtr = putLLQ(&m->omsg, responsePtr, q, llq);
	if (responsePtr)
		{
		mStatus err = mDNSSendDNSMessage(m, &m->omsg, responsePtr, mDNSInterface_Any, q->LocalSocket, &q->servAddr, q->servPort, q->tcp ? q->tcp->sock : mDNSNULL, q->AuthInfo);
		if (err)
			{
			LogMsg("sendChallengeResponse: mDNSSendDNSMessage%s failed: %d", q->tcp ? " (TCP)" : "", err);
			if (q->tcp) { DisposeTCPConn(q->tcp); q->tcp = mDNSNULL; }
			}
		}
	else StartLLQPolling(m,q);
	}

mDNSlocal void SetLLQTimer(mDNS *const m, DNSQuestion *const q, const LLQOptData *const llq)
	{
	mDNSs32 lease = (mDNSs32)llq->llqlease * mDNSPlatformOneSecond;
	q->ReqLease      = llq->llqlease;
	q->LastQTime     = m->timenow;
	q->expire        = m->timenow + lease;
	q->ThisQInterval = lease/2 + mDNSRandom(lease/10);
	debugf("SetLLQTimer setting %##s (%s) to %d %d", q->qname.c, DNSTypeName(q->qtype), lease/mDNSPlatformOneSecond, q->ThisQInterval/mDNSPlatformOneSecond);
	SetNextQueryTime(m, q);
	}

mDNSlocal void recvSetupResponse(mDNS *const m, mDNSu8 rcode, DNSQuestion *const q, const LLQOptData *const llq)
	{
	if (rcode && rcode != kDNSFlag1_RC_NXDomain)
		{ LogMsg("ERROR: recvSetupResponse %##s (%s) - rcode && rcode != kDNSFlag1_RC_NXDomain", q->qname.c, DNSTypeName(q->qtype)); return; }

	if (llq->llqOp != kLLQOp_Setup)
		{ LogMsg("ERROR: recvSetupResponse %##s (%s) - bad op %d", q->qname.c, DNSTypeName(q->qtype), llq->llqOp); return; }

	if (llq->vers != kLLQ_Vers)
		{ LogMsg("ERROR: recvSetupResponse %##s (%s) - bad vers %d", q->qname.c, DNSTypeName(q->qtype), llq->vers); return; }

	if (q->state == LLQ_InitialRequest)
		{
		//LogInfo("Got LLQ_InitialRequest");

		if (llq->err) { LogMsg("recvSetupResponse - received llq->err %d from server", llq->err); StartLLQPolling(m,q); return; }
	
		if (q->ReqLease != llq->llqlease)
			debugf("recvSetupResponse: requested lease %lu, granted lease %lu", q->ReqLease, llq->llqlease);
	
		// cache expiration in case we go to sleep before finishing setup
		q->ReqLease = llq->llqlease;
		q->expire = m->timenow + ((mDNSs32)llq->llqlease * mDNSPlatformOneSecond);
	
		// update state
		q->state  = LLQ_SecondaryRequest;
		q->id     = llq->id;
		q->ntries = 0; // first attempt to send response
		sendChallengeResponse(m, q, llq);
		}
	else if (q->state == LLQ_SecondaryRequest)
		{
		//LogInfo("Got LLQ_SecondaryRequest");

		// Fix this immediately if not sooner.  Copy the id from the LLQOptData into our DNSQuestion struct.  This is only
		// an issue for private LLQs, because we skip parts 2 and 3 of the handshake.  This is related to a bigger
		// problem of the current implementation of TCP LLQ setup: we're not handling state transitions correctly
		// if the server sends back SERVFULL or STATIC.
		if (q->AuthInfo)
			{
			LogInfo("Private LLQ_SecondaryRequest; copying id %08X%08X", llq->id.l[0], llq->id.l[1]);
			q->id = llq->id;
			}

		if (llq->err) { LogMsg("ERROR: recvSetupResponse %##s (%s) code %d from server", q->qname.c, DNSTypeName(q->qtype), llq->err); StartLLQPolling(m,q); return; }
		if (!mDNSSameOpaque64(&q->id, &llq->id))
			{ LogMsg("recvSetupResponse - ID changed.  discarding"); return; } // this can happen rarely (on packet loss + reordering)
		q->state         = LLQ_Established;
		q->ntries        = 0;
		SetLLQTimer(m, q, llq);
#if APPLE_OSX_mDNSResponder
		UpdateAutoTunnelDomainStatuses(m);
#endif
		}
	}

mDNSexport uDNS_LLQType uDNS_recvLLQResponse(mDNS *const m, const DNSMessage *const msg, const mDNSu8 *const end, const mDNSAddr *const srcaddr, const mDNSIPPort srcport)
	{
	DNSQuestion pktQ, *q;
	if (msg->h.numQuestions && getQuestion(msg, msg->data, end, 0, &pktQ))
		{
		const rdataOPT *opt = GetLLQOptData(m, msg, end);

		for (q = m->Questions; q; q = q->next)
			{
			if (!mDNSOpaque16IsZero(q->TargetQID) && q->LongLived && q->qtype == pktQ.qtype && q->qnamehash == pktQ.qnamehash && SameDomainName(&q->qname, &pktQ.qname))
				{
				debugf("uDNS_recvLLQResponse found %##s (%s) %d %#a %#a %X %X %X %X %d",
					q->qname.c, DNSTypeName(q->qtype), q->state, srcaddr, &q->servAddr,
					opt ? opt->u.llq.id.l[0] : 0, opt ? opt->u.llq.id.l[1] : 0, q->id.l[0], q->id.l[1], opt ? opt->u.llq.llqOp : 0);
				if (q->state == LLQ_Poll) debugf("uDNS_LLQ_Events: q->state == LLQ_Poll msg->h.id %d q->TargetQID %d", mDNSVal16(msg->h.id), mDNSVal16(q->TargetQID));
				if (q->state == LLQ_Poll && mDNSSameOpaque16(msg->h.id, q->TargetQID))
					{
					m->rec.r.resrec.RecordType = 0;		// Clear RecordType to show we're not still using it
					debugf("uDNS_recvLLQResponse got poll response; moving to LLQ_InitialRequest for %##s (%s)", q->qname.c, DNSTypeName(q->qtype));
					q->state         = LLQ_InitialRequest;
					q->servPort      = zeroIPPort;		// Clear servPort so that startLLQHandshake will retry the GetZoneData processing
					q->ThisQInterval = LLQ_POLL_INTERVAL + mDNSRandom(LLQ_POLL_INTERVAL/10);	// Retry LLQ setup in approx 15 minutes
					q->LastQTime     = m->timenow;
					SetNextQueryTime(m, q);
					return uDNS_LLQ_Entire;		// uDNS_LLQ_Entire means flush stale records; assume a large effective TTL
					}
				// Note: In LLQ Event packets, the msg->h.id does not match our q->TargetQID, because in that case the msg->h.id nonce is selected by the server
				else if (opt && q->state == LLQ_Established && opt->u.llq.llqOp == kLLQOp_Event && mDNSSameOpaque64(&opt->u.llq.id, &q->id))
					{
					mDNSu8 *ackEnd;
					//debugf("Sending LLQ ack for %##s (%s)", q->qname.c, DNSTypeName(q->qtype));
					InitializeDNSMessage(&m->omsg.h, msg->h.id, ResponseFlags);
					ackEnd = putLLQ(&m->omsg, m->omsg.data, q, &opt->u.llq);
					if (ackEnd) mDNSSendDNSMessage(m, &m->omsg, ackEnd, mDNSInterface_Any, q->LocalSocket, srcaddr, srcport, mDNSNULL, mDNSNULL);
					m->rec.r.resrec.RecordType = 0;		// Clear RecordType to show we're not still using it
					debugf("uDNS_LLQ_Events: q->state == LLQ_Established msg->h.id %d q->TargetQID %d", mDNSVal16(msg->h.id), mDNSVal16(q->TargetQID));
					return uDNS_LLQ_Events;
					}
				if (opt && mDNSSameOpaque16(msg->h.id, q->TargetQID))
					{
					if (q->state == LLQ_Established && opt->u.llq.llqOp == kLLQOp_Refresh && mDNSSameOpaque64(&opt->u.llq.id, &q->id) && msg->h.numAdditionals && !msg->h.numAnswers)
						{
						if (opt->u.llq.err != LLQErr_NoError) LogMsg("recvRefreshReply: received error %d from server", opt->u.llq.err);
						else
							{
							//LogInfo("Received refresh confirmation ntries %d for %##s (%s)", q->ntries, q->qname.c, DNSTypeName(q->qtype));
							// If we're waiting to go to sleep, then this LLQ deletion may have been the thing
							// we were waiting for, so schedule another check to see if we can sleep now.
							if (opt->u.llq.llqlease == 0 && m->SleepLimit) m->NextScheduledSPRetry = m->timenow;
							GrantCacheExtensions(m, q, opt->u.llq.llqlease);
							SetLLQTimer(m, q, &opt->u.llq);
							q->ntries = 0;
							}
						m->rec.r.resrec.RecordType = 0;		// Clear RecordType to show we're not still using it
						return uDNS_LLQ_Ignore;
						}
					if (q->state < LLQ_Established && mDNSSameAddress(srcaddr, &q->servAddr))
						{
						LLQ_State oldstate = q->state;
						recvSetupResponse(m, msg->h.flags.b[1] & kDNSFlag1_RC_Mask, q, &opt->u.llq);
						m->rec.r.resrec.RecordType = 0;		// Clear RecordType to show we're not still using it
						// We have a protocol anomaly here in the LLQ definition.
						// Both the challenge packet from the server and the ack+answers packet have opt->u.llq.llqOp == kLLQOp_Setup.
						// However, we need to treat them differently:
						// The challenge packet has no answers in it, and tells us nothing about whether our cache entries
						// are still valid, so this packet should not cause us to do anything that messes with our cache.
						// The ack+answers packet gives us the whole truth, so we should handle it by updating our cache
						// to match the answers in the packet, and only the answers in the packet.
						return (oldstate == LLQ_SecondaryRequest ? uDNS_LLQ_Entire : uDNS_LLQ_Ignore);
						}
					}
				}
			}
		m->rec.r.resrec.RecordType = 0;		// Clear RecordType to show we're not still using it
		}
	return uDNS_LLQ_Not;
	}

// Stub definition of TCPSocket_struct so we can access flags field. (Rest of TCPSocket_struct is platform-dependent.)
struct TCPSocket_struct { TCPSocketFlags flags; /* ... */ };

// tcpCallback is called to handle events (e.g. connection opening and data reception) on TCP connections for
// Private DNS operations -- private queries, private LLQs, private record updates and private service updates
mDNSlocal void tcpCallback(TCPSocket *sock, void *context, mDNSBool ConnectionEstablished, mStatus err)
	{
	tcpInfo_t *tcpInfo = (tcpInfo_t *)context;
	mDNSBool   closed  = mDNSfalse;
	mDNS      *m       = tcpInfo->m;
	DNSQuestion *const q = tcpInfo->question;
	tcpInfo_t **backpointer =
		q                 ? &q           ->tcp :
		tcpInfo->srs      ? &tcpInfo->srs->tcp :
		tcpInfo->rr       ? &tcpInfo->rr ->tcp : mDNSNULL;
	if (backpointer && *backpointer != tcpInfo)
		LogMsg("tcpCallback: %d backpointer %p incorrect tcpInfo %p question %p srs %p rr %p",
			mDNSPlatformTCPGetFD(tcpInfo->sock), *backpointer, tcpInfo, q, tcpInfo->srs, tcpInfo->rr);

	if (err) goto exit;

	if (ConnectionEstablished)
		{
		mDNSu8    *end = ((mDNSu8*) &tcpInfo->request) + tcpInfo->requestLen;
		DomainAuthInfo *AuthInfo;

		// Defensive coding for <rdar://problem/5546824> Crash in mDNSResponder at GetAuthInfoForName_internal + 366
		// Don't know yet what's causing this, but at least we can be cautious and try to avoid crashing if we find our pointers in an unexpected state
		if (tcpInfo->srs && tcpInfo->srs->RR_SRV.resrec.name != &tcpInfo->srs->RR_SRV.namestorage)
			LogMsg("tcpCallback: ERROR: tcpInfo->srs->RR_SRV.resrec.name %p != &tcpInfo->srs->RR_SRV.namestorage %p",
				tcpInfo->srs->RR_SRV.resrec.name, &tcpInfo->srs->RR_SRV.namestorage);
		if (tcpInfo->rr && tcpInfo->rr->resrec.name != &tcpInfo->rr->namestorage)
			LogMsg("tcpCallback: ERROR: tcpInfo->rr->resrec.name %p != &tcpInfo->rr->namestorage %p",
				tcpInfo->rr->resrec.name, &tcpInfo->rr->namestorage);
		if (tcpInfo->srs && tcpInfo->srs->RR_SRV.resrec.name != &tcpInfo->srs->RR_SRV.namestorage) return;
		if (tcpInfo->rr  && tcpInfo->rr->        resrec.name != &tcpInfo->rr->        namestorage) return;

		AuthInfo =  tcpInfo->srs ? GetAuthInfoForName(m, tcpInfo->srs->RR_SRV.resrec.name) :
					tcpInfo->rr  ? GetAuthInfoForName(m, tcpInfo->rr->resrec.name)         : mDNSNULL;

		// connection is established - send the message
		if (q && q->LongLived && q->state == LLQ_Established)
			{
			end = ((mDNSu8*) &tcpInfo->request) + tcpInfo->requestLen;
			}
		else if (q && q->LongLived && q->state != LLQ_Poll && !mDNSIPPortIsZero(m->LLQNAT.ExternalPort) && !mDNSIPPortIsZero(q->servPort))
			{
			// Notes:
			// If we have a NAT port mapping, ExternalPort is the external port
			// If we have a routable address so we don't need a port mapping, ExternalPort is the same as our own internal port
			// If we need a NAT port mapping but can't get one, then ExternalPort is zero
			LLQOptData llqData;			// set llq rdata
			llqData.vers  = kLLQ_Vers;
			llqData.llqOp = kLLQOp_Setup;
			llqData.err   = GetLLQEventPort(m, &tcpInfo->Addr);	// We're using TCP; tell server what UDP port to send notifications to
			LogInfo("tcpCallback: eventPort %d", llqData.err);
			llqData.id    = zeroOpaque64;
			llqData.llqlease = kLLQ_DefLease;
			InitializeDNSMessage(&tcpInfo->request.h, q->TargetQID, uQueryFlags);
			end = putLLQ(&tcpInfo->request, tcpInfo->request.data, q, &llqData);
			if (!end) { LogMsg("ERROR: tcpCallback - putLLQ"); err = mStatus_UnknownErr; goto exit; }
			AuthInfo = q->AuthInfo;		// Need to add TSIG to this message
			}
		else if (q)
			{
			InitializeDNSMessage(&tcpInfo->request.h, q->TargetQID, uQueryFlags);
			end = putQuestion(&tcpInfo->request, tcpInfo->request.data, tcpInfo->request.data + AbsoluteMaxDNSMessageData, &q->qname, q->qtype, q->qclass);
			AuthInfo = q->AuthInfo;		// Need to add TSIG to this message
			}

		err = mDNSSendDNSMessage(m, &tcpInfo->request, end, mDNSInterface_Any, mDNSNULL, &tcpInfo->Addr, tcpInfo->Port, sock, AuthInfo);
		if (err) { debugf("ERROR: tcpCallback: mDNSSendDNSMessage - %d", err); err = mStatus_UnknownErr; goto exit; }

		// Record time we sent this question
		if (q)
			{
			mDNS_Lock(m);
			q->LastQTime = m->timenow;
			if (q->ThisQInterval < (256 * mDNSPlatformOneSecond))	// Now we have a TCP connection open, make sure we wait at least 256 seconds before retrying
				q->ThisQInterval = (256 * mDNSPlatformOneSecond);
			SetNextQueryTime(m, q);
			mDNS_Unlock(m);
			}
		}
	else
		{
		long n;
		if (tcpInfo->nread < 2)			// First read the two-byte length preceeding the DNS message
			{
			mDNSu8 *lenptr = (mDNSu8 *)&tcpInfo->replylen;
			n = mDNSPlatformReadTCP(sock, lenptr + tcpInfo->nread, 2 - tcpInfo->nread, &closed);
			if (n < 0) { LogMsg("ERROR: tcpCallback - attempt to read message length failed (%d)", n); err = mStatus_ConnFailed; goto exit; }
			else if (closed)
				{
				// It's perfectly fine for this socket to close after the first reply. The server might
				// be sending gratuitous replies using UDP and doesn't have a need to leave the TCP socket open.
				// We'll only log this event if we've never received a reply before.
				// BIND 9 appears to close an idle connection after 30 seconds.
				if (tcpInfo->numReplies == 0) LogMsg("ERROR: socket closed prematurely tcpInfo->nread = %d", tcpInfo->nread);
				err = mStatus_ConnFailed;
				goto exit;
				}

			tcpInfo->nread += n;
			if (tcpInfo->nread < 2) goto exit;

			tcpInfo->replylen = (mDNSu16)((mDNSu16)lenptr[0] << 8 | lenptr[1]);
			if (tcpInfo->replylen < sizeof(DNSMessageHeader))
				{ LogMsg("ERROR: tcpCallback - length too short (%d bytes)", tcpInfo->replylen); err = mStatus_UnknownErr; goto exit; }

			tcpInfo->reply = mDNSPlatformMemAllocate(tcpInfo->replylen);
			if (!tcpInfo->reply) { LogMsg("ERROR: tcpCallback - malloc failed"); err = mStatus_NoMemoryErr; goto exit; }
			}

		n = mDNSPlatformReadTCP(sock, ((char *)tcpInfo->reply) + (tcpInfo->nread - 2), tcpInfo->replylen - (tcpInfo->nread - 2), &closed);

		if      (n < 0)  { LogMsg("ERROR: tcpCallback - read returned %d", n);            err = mStatus_ConnFailed; goto exit; }
		else if (closed) { LogMsg("ERROR: socket closed prematurely %d", tcpInfo->nread); err = mStatus_ConnFailed; goto exit; }

		tcpInfo->nread += n;

		if ((tcpInfo->nread - 2) == tcpInfo->replylen)
			{
			AuthRecord *rr    = tcpInfo->rr;
			DNSMessage *reply = tcpInfo->reply;
			mDNSu8     *end   = (mDNSu8 *)tcpInfo->reply + tcpInfo->replylen;
			mDNSAddr    Addr  = tcpInfo->Addr;
			mDNSIPPort  Port  = tcpInfo->Port;
			tcpInfo->numReplies++;
			tcpInfo->reply    = mDNSNULL;	// Detach reply buffer from tcpInfo_t, to make sure client callback can't cause it to be disposed
			tcpInfo->nread    = 0;
			tcpInfo->replylen = 0;
			
			// If we're going to dispose this connection, do it FIRST, before calling client callback
			// Note: Sleep code depends on us clearing *backpointer here -- it uses the clearing of rr->tcp and srs->tcp
			// as the signal that the DNS deregistration operation with the server has completed, and the machine may now sleep
			if (backpointer)
				if (!q || !q->LongLived || m->SleepState)
					{ *backpointer = mDNSNULL; DisposeTCPConn(tcpInfo); }
			
			if (rr && rr->resrec.RecordType == kDNSRecordTypeDeregistering)
				{
				mDNS_Lock(m);
				LogInfo("tcpCallback: CompleteDeregistration %s", ARDisplayString(m, rr));
				CompleteDeregistration(m, rr);		// Don't touch rr after this
				mDNS_Unlock(m);
				}
			else
				mDNSCoreReceive(m, reply, end, &Addr, Port, (sock->flags & kTCPSocketFlags_UseTLS) ? (mDNSAddr *)1 : mDNSNULL, zeroIPPort, 0);
			// USE CAUTION HERE: Invoking mDNSCoreReceive may have caused the environment to change, including canceling this operation itself
			
			mDNSPlatformMemFree(reply);
			return;
			}
		}

exit:

	if (err)
		{
		// Clear client backpointer FIRST -- that way if one of the callbacks cancels its operation
		// we won't end up double-disposing our tcpInfo_t
		if (backpointer) *backpointer = mDNSNULL;

		mDNS_Lock(m);		// Need to grab the lock to get m->timenow

		if (q)
			{
			if (q->ThisQInterval == 0 || q->LastQTime + q->ThisQInterval - m->timenow > MAX_UCAST_POLL_INTERVAL)
				{
				q->LastQTime     = m->timenow;
				q->ThisQInterval = MAX_UCAST_POLL_INTERVAL;
				SetNextQueryTime(m, q);
				}
			// ConnFailed may be actually okay. It just means that the server closed the connection but the LLQ may still be okay.
			// If the error isn't ConnFailed, then the LLQ is in bad shape.
			if (err != mStatus_ConnFailed)
				{
				if (q->LongLived && q->state != LLQ_Poll) StartLLQPolling(m, q);
				}
			}

		if (tcpInfo->rr) SetRecordRetry(m, tcpInfo->rr, mStatus_NoError);

		if (tcpInfo->srs) SetRecordRetry(m, &tcpInfo->srs->RR_SRV, mStatus_NoError);

		mDNS_Unlock(m);

		DisposeTCPConn(tcpInfo);
		}
	}

mDNSlocal tcpInfo_t *MakeTCPConn(mDNS *const m, const DNSMessage *const msg, const mDNSu8 *const end,
	TCPSocketFlags flags, const mDNSAddr *const Addr, const mDNSIPPort Port,
	DNSQuestion *const question, ServiceRecordSet *const srs, AuthRecord *const rr)
	{
	mStatus err;
	mDNSIPPort srcport = zeroIPPort;
	tcpInfo_t *info = (tcpInfo_t *)mDNSPlatformMemAllocate(sizeof(tcpInfo_t));
	if (!info) { LogMsg("ERROR: MakeTCP - memallocate failed"); return(mDNSNULL); }
	mDNSPlatformMemZero(info, sizeof(tcpInfo_t));

	info->m          = m;
	info->sock       = mDNSPlatformTCPSocket(m, flags, &srcport);
	info->requestLen = 0;
	info->question   = question;
	info->srs        = srs;
	info->rr         = rr;
	info->Addr       = *Addr;
	info->Port       = Port;
	info->reply      = mDNSNULL;
	info->replylen   = 0;
	info->nread      = 0;
	info->numReplies = 0;

	if (msg)
		{
		info->requestLen = (int) (end - ((mDNSu8*)msg));
		mDNSPlatformMemCopy(&info->request, msg, info->requestLen);
		}

	if (!info->sock) { LogMsg("SendServiceRegistration: unable to create TCP socket"); mDNSPlatformMemFree(info); return(mDNSNULL); }
	err = mDNSPlatformTCPConnect(info->sock, Addr, Port, 0, tcpCallback, info);

	// Probably suboptimal here.
	// Instead of returning mDNSNULL here on failure, we should probably invoke the callback with an error code.
	// That way clients can put all the error handling and retry/recovery code in one place,
	// instead of having to handle immediate errors in one place and async errors in another.
	// Also: "err == mStatus_ConnEstablished" probably never happens.

	// Don't need to log "connection failed" in customer builds -- it happens quite often during sleep, wake, configuration changes, etc.
	if      (err == mStatus_ConnEstablished) { tcpCallback(info->sock, info, mDNStrue, mStatus_NoError); }
	else if (err != mStatus_ConnPending    ) { LogInfo("MakeTCPConnection: connection failed"); DisposeTCPConn(info); return(mDNSNULL); }
	return(info);
	}

mDNSexport void DisposeTCPConn(struct tcpInfo_t *tcp)
	{
	mDNSPlatformTCPCloseConnection(tcp->sock);
	if (tcp->reply) mDNSPlatformMemFree(tcp->reply);
	mDNSPlatformMemFree(tcp);
	}

// Lock must be held
mDNSexport void startLLQHandshake(mDNS *m, DNSQuestion *q)
	{
	if (mDNSIPv4AddressIsOnes(m->LLQNAT.ExternalAddress))
		{
		LogInfo("startLLQHandshake: waiting for NAT status for %##s (%s)", q->qname.c, DNSTypeName(q->qtype));
		q->ThisQInterval = LLQ_POLL_INTERVAL + mDNSRandom(LLQ_POLL_INTERVAL/10);	// Retry in approx 15 minutes
		q->LastQTime = m->timenow;
		SetNextQueryTime(m, q);
		return;
		}

	if (mDNSIPPortIsZero(m->LLQNAT.ExternalPort))
		{
		LogInfo("startLLQHandshake: Cannot receive inbound packets; will poll for %##s (%s)", q->qname.c, DNSTypeName(q->qtype));
		StartLLQPolling(m, q);
		return;
		}

	if (mDNSIPPortIsZero(q->servPort))
		{
		debugf("startLLQHandshake: StartGetZoneData for %##s (%s)", q->qname.c, DNSTypeName(q->qtype));
		q->ThisQInterval = LLQ_POLL_INTERVAL + mDNSRandom(LLQ_POLL_INTERVAL/10);	// Retry in approx 15 minutes
		q->LastQTime     = m->timenow;
		SetNextQueryTime(m, q);
		q->servAddr = zeroAddr;
		// We know q->servPort is zero because of check above
		if (q->nta) CancelGetZoneData(m, q->nta);
		q->nta = StartGetZoneData(m, &q->qname, ZoneServiceLLQ, LLQGotZoneData, q);
		return;
		}

	if (q->AuthInfo)
		{
		if (q->tcp) LogInfo("startLLQHandshake: Disposing existing TCP connection for %##s (%s)", q->qname.c, DNSTypeName(q->qtype));
		if (q->tcp) DisposeTCPConn(q->tcp);
		q->tcp = MakeTCPConn(m, mDNSNULL, mDNSNULL, kTCPSocketFlags_UseTLS, &q->servAddr, q->servPort, q, mDNSNULL, mDNSNULL);
		if (!q->tcp)
			q->ThisQInterval = mDNSPlatformOneSecond * 5;	// If TCP failed (transient networking glitch) try again in five seconds
		else
			{
			q->state         = LLQ_SecondaryRequest;		// Right now, for private DNS, we skip the four-way LLQ handshake
			q->ReqLease      = kLLQ_DefLease;
			q->ThisQInterval = 0;
			}
		q->LastQTime     = m->timenow;
		SetNextQueryTime(m, q);
		}
	else
		{
		debugf("startLLQHandshake: m->AdvertisedV4 %#a%s Server %#a:%d%s %##s (%s)",
			&m->AdvertisedV4,                     mDNSv4AddrIsRFC1918(&m->AdvertisedV4.ip.v4) ? " (RFC 1918)" : "",
			&q->servAddr, mDNSVal16(q->servPort), mDNSAddrIsRFC1918(&q->servAddr)             ? " (RFC 1918)" : "",
			q->qname.c, DNSTypeName(q->qtype));

		if (q->ntries++ >= kLLQ_MAX_TRIES)
			{
			LogMsg("startLLQHandshake: %d failed attempts for LLQ %##s Polling.", kLLQ_MAX_TRIES, q->qname.c);
			StartLLQPolling(m, q);
			}
		else
			{
			mDNSu8 *end;
			LLQOptData llqData;

			// set llq rdata
			llqData.vers  = kLLQ_Vers;
			llqData.llqOp = kLLQOp_Setup;
			llqData.err   = LLQErr_NoError;	// Don't need to tell server UDP notification port when sending over UDP
			llqData.id    = zeroOpaque64;
			llqData.llqlease = kLLQ_DefLease;
	
			InitializeDNSMessage(&m->omsg.h, q->TargetQID, uQueryFlags);
			end = putLLQ(&m->omsg, m->omsg.data, q, &llqData);
			if (!end) { LogMsg("ERROR: startLLQHandshake - putLLQ"); StartLLQPolling(m,q); return; }
	
			mDNSSendDNSMessage(m, &m->omsg, end, mDNSInterface_Any, q->LocalSocket, &q->servAddr, q->servPort, mDNSNULL, mDNSNULL);
	
			// update question state
			q->state         = LLQ_InitialRequest;
			q->ReqLease      = kLLQ_DefLease;
			q->ThisQInterval = (kLLQ_INIT_RESEND * mDNSPlatformOneSecond);
			q->LastQTime     = m->timenow;
			SetNextQueryTime(m, q);
			}
		}
	}

// forward declaration so GetServiceTarget can do reverse lookup if needed
mDNSlocal void GetStaticHostname(mDNS *m);

mDNSexport const domainname *GetServiceTarget(mDNS *m, AuthRecord *const rr)
	{
	debugf("GetServiceTarget %##s", rr->resrec.name->c);

	if (!rr->AutoTarget)		// If not automatically tracking this host's current name, just return the existing target
		return(&rr->resrec.rdata->u.srv.target);
	else
		{
#if APPLE_OSX_mDNSResponder
		DomainAuthInfo *AuthInfo = GetAuthInfoForName_internal(m, rr->resrec.name);
		if (AuthInfo && AuthInfo->AutoTunnel)
			{
			// If this AutoTunnel is not yet active, start it now (which entails activating its NAT Traversal request,
			// which will subsequently advertise the appropriate records when the NAT Traversal returns a result)
			if (!AuthInfo->AutoTunnelNAT.clientContext && m->AutoTunnelHostAddr.b[0])
				SetupLocalAutoTunnelInterface_internal(m);
			if (AuthInfo->AutoTunnelHostRecord.namestorage.c[0] == 0) return(mDNSNULL);
			return(&AuthInfo->AutoTunnelHostRecord.namestorage);
			}
		else
#endif // APPLE_OSX_mDNSResponder
			{
			const int srvcount = CountLabels(rr->resrec.name);
			HostnameInfo *besthi = mDNSNULL, *hi;
			int best = 0;
			for (hi = m->Hostnames; hi; hi = hi->next)
				if (hi->arv4.state == regState_Registered || hi->arv4.state == regState_Refresh ||
					hi->arv6.state == regState_Registered || hi->arv6.state == regState_Refresh)
					{
					int x, hostcount = CountLabels(&hi->fqdn);
					for (x = hostcount < srvcount ? hostcount : srvcount; x > 0 && x > best; x--)
						if (SameDomainName(SkipLeadingLabels(rr->resrec.name, srvcount - x), SkipLeadingLabels(&hi->fqdn, hostcount - x)))
							{ best = x; besthi = hi; }
					}
	
			if (besthi) return(&besthi->fqdn);
			}
		if (m->StaticHostname.c[0]) return(&m->StaticHostname);
		else GetStaticHostname(m); // asynchronously do reverse lookup for primary IPv4 address
		return(mDNSNULL);
		}
	}

// Called with lock held
mDNSlocal void SendServiceRegistration(mDNS *m, ServiceRecordSet *srs)
	{
	mDNSu8 *ptr = m->omsg.data;
	mDNSu8 *end = (mDNSu8 *)&m->omsg + sizeof(DNSMessage);
	mDNSOpaque16 id;
	mStatus err = mStatus_NoError;
	const domainname *target;
	mDNSu32 i;

	if (m->mDNS_busy != m->mDNS_reentrancy+1)
		LogMsg("SendServiceRegistration: Lock not held! mDNS_busy (%ld) mDNS_reentrancy (%ld)", m->mDNS_busy, m->mDNS_reentrancy);

	if (mDNSIPv4AddressIsZero(srs->SRSUpdateServer.ip.v4))	// Don't know our UpdateServer yet
		{
		srs->RR_SRV.LastAPTime = m->timenow;
		if (srs->RR_SRV.ThisAPInterval < mDNSPlatformOneSecond * 5)
			srs->RR_SRV.ThisAPInterval = mDNSPlatformOneSecond * 5;
		return;
		}

	if (srs->state == regState_Registered) srs->state = regState_Refresh;

	id = mDNS_NewMessageID(m);
	InitializeDNSMessage(&m->omsg.h, id, UpdateReqFlags);

	// setup resource records
	SetNewRData(&srs->RR_PTR.resrec, mDNSNULL, 0);		// Update rdlength, rdestimate, rdatahash
	SetNewRData(&srs->RR_TXT.resrec, mDNSNULL, 0);		// Update rdlength, rdestimate, rdatahash

	// replace port w/ NAT mapping if necessary
	if (srs->RR_SRV.AutoTarget == Target_AutoHostAndNATMAP && !mDNSIPPortIsZero(srs->NATinfo.ExternalPort))
		srs->RR_SRV.resrec.rdata->u.srv.port = srs->NATinfo.ExternalPort;

	// construct update packet
	// set zone
	ptr = putZone(&m->omsg, ptr, end, &srs->zone, mDNSOpaque16fromIntVal(srs->RR_SRV.resrec.rrclass));
	if (!ptr) { err = mStatus_UnknownErr; goto exit; }

	if (srs->TestForSelfConflict)
		{
		// update w/ prereq that SRV already exist to make sure previous registration was ours, and delete any stale TXT records
		if (!(ptr = PutResourceRecordTTLJumbo(&m->omsg, ptr, &m->omsg.h.mDNS_numPrereqs, &srs->RR_SRV.resrec, 0))) { err = mStatus_UnknownErr; goto exit; }
		if (!(ptr = putDeleteRRSet(&m->omsg, ptr, srs->RR_TXT.resrec.name, srs->RR_TXT.resrec.rrtype)))            { err = mStatus_UnknownErr; goto exit; }
		}

	else if (srs->state != regState_Refresh && srs->state != regState_UpdatePending)
		{
		// use SRV name for prereq
		//ptr = putPrereqNameNotInUse(srs->RR_SRV.resrec.name, &m->omsg, ptr, end);

		// For now, until we implement RFC 4701 (DHCID RR) to detect whether an existing record is someone else using the name, or just a
		// stale echo of our own previous registration before we changed our host name, we just overwrite whatever may have already been there
		ptr = putDeleteRRSet(&m->omsg, ptr, srs->RR_SRV.resrec.name, kDNSQType_ANY);
		if (!ptr) { err = mStatus_UnknownErr; goto exit; }
		}

	//!!!KRS Need to do bounds checking and use TCP if it won't fit!!!
	if (!(ptr = PutResourceRecordTTLJumbo(&m->omsg, ptr, &m->omsg.h.mDNS_numUpdates, &srs->RR_PTR.resrec, srs->RR_PTR.resrec.rroriginalttl))) { err = mStatus_UnknownErr; goto exit; }

	for (i = 0; i < srs->NumSubTypes; i++)
		if (!(ptr = PutResourceRecordTTLJumbo(&m->omsg, ptr, &m->omsg.h.mDNS_numUpdates, &srs->SubTypes[i].resrec, srs->SubTypes[i].resrec.rroriginalttl))) { err = mStatus_UnknownErr; goto exit; }

	if (srs->state == regState_UpdatePending) // we're updating the txt record
		{
		AuthRecord *txt = &srs->RR_TXT;
		// delete old RData
		SetNewRData(&txt->resrec, txt->OrigRData, txt->OrigRDLen);
		if (!(ptr = putDeletionRecord(&m->omsg, ptr, &srs->RR_TXT.resrec))) { err = mStatus_UnknownErr; goto exit; }	// delete old rdata

		// add new RData
		SetNewRData(&txt->resrec, txt->InFlightRData, txt->InFlightRDLen);
		if (!(ptr = PutResourceRecordTTLJumbo(&m->omsg, ptr, &m->omsg.h.mDNS_numUpdates, &srs->RR_TXT.resrec, srs->RR_TXT.resrec.rroriginalttl))) { err = mStatus_UnknownErr; goto exit; }
		}
	else
		if (!(ptr = PutResourceRecordTTLJumbo(&m->omsg, ptr, &m->omsg.h.mDNS_numUpdates, &srs->RR_TXT.resrec, srs->RR_TXT.resrec.rroriginalttl))) { err = mStatus_UnknownErr; goto exit; }

	target = GetServiceTarget(m, &srs->RR_SRV);
	if (!target || target->c[0] == 0)
		{
		debugf("SendServiceRegistration - no target for %##s", srs->RR_SRV.resrec.name->c);
		srs->state = regState_NoTarget;
		return;
		}

	if (!SameDomainName(target, &srs->RR_SRV.resrec.rdata->u.srv.target))
		{
		AssignDomainName(&srs->RR_SRV.resrec.rdata->u.srv.target, target);
		SetNewRData(&srs->RR_SRV.resrec, mDNSNULL, 0);		// Update rdlength, rdestimate, rdatahash
		}

	ptr = PutResourceRecordTTLJumbo(&m->omsg, ptr, &m->omsg.h.mDNS_numUpdates, &srs->RR_SRV.resrec, srs->RR_SRV.resrec.rroriginalttl);
	if (!ptr) { err = mStatus_UnknownErr; goto exit; }

	if (srs->srs_uselease)
		{ ptr = putUpdateLease(&m->omsg, ptr, DEFAULT_UPDATE_LEASE); if (!ptr) { err = mStatus_UnknownErr; goto exit; } }

	if (srs->state != regState_Refresh && srs->state != regState_DeregDeferred && srs->state != regState_UpdatePending)
		srs->state = regState_Pending;

	srs->id = id;

	if (srs->Private)
		{
		if (srs->tcp) LogInfo("SendServiceRegistration: Disposing existing TCP connection for %s", ARDisplayString(m, &srs->RR_SRV));
		if (srs->tcp) DisposeTCPConn(srs->tcp);
		srs->tcp = MakeTCPConn(m, &m->omsg, ptr, kTCPSocketFlags_UseTLS, &srs->SRSUpdateServer, srs->SRSUpdatePort, mDNSNULL, srs, mDNSNULL);
		if (!srs->tcp) srs->RR_SRV.ThisAPInterval = mDNSPlatformOneSecond * 5; // If failed to make TCP connection, try again in ten seconds (5*2)
		else if (srs->RR_SRV.ThisAPInterval < mDNSPlatformOneSecond * 30) srs->RR_SRV.ThisAPInterval = mDNSPlatformOneSecond * 30;
		}
	else
		{
		err = mDNSSendDNSMessage(m, &m->omsg, ptr, mDNSInterface_Any, mDNSNULL, &srs->SRSUpdateServer, srs->SRSUpdatePort, mDNSNULL, GetAuthInfoForName_internal(m, srs->RR_SRV.resrec.name));
		if (err) debugf("ERROR: SendServiceRegistration - mDNSSendDNSMessage - %d", err);
		}

	SetRecordRetry(m, &srs->RR_SRV, err);
	return;

exit:
	if (err)
		{
		LogMsg("SendServiceRegistration ERROR formatting message %d!! Permanently abandoning service registration %##s", err, srs->RR_SRV.resrec.name->c);
		unlinkSRS(m, srs);
		srs->state = regState_Unregistered;

		mDNS_DropLockBeforeCallback();
		srs->ServiceCallback(m, srs, err);
		mDNS_ReclaimLockAfterCallback();
		// CAUTION: MUST NOT do anything more with rr after calling rr->Callback(), because the client's callback function
		// is allowed to do anything, including starting/stopping queries, registering/deregistering records, etc.
		}
	}

mDNSlocal const domainname *PUBLIC_UPDATE_SERVICE_TYPE  = (const domainname*)"\x0B_dns-update"     "\x04_udp";
mDNSlocal const domainname *PUBLIC_LLQ_SERVICE_TYPE     = (const domainname*)"\x08_dns-llq"        "\x04_udp";

mDNSlocal const domainname *PRIVATE_UPDATE_SERVICE_TYPE = (const domainname*)"\x0F_dns-update-tls" "\x04_tcp";
mDNSlocal const domainname *PRIVATE_QUERY_SERVICE_TYPE  = (const domainname*)"\x0E_dns-query-tls"  "\x04_tcp";
mDNSlocal const domainname *PRIVATE_LLQ_SERVICE_TYPE    = (const domainname*)"\x0C_dns-llq-tls"    "\x04_tcp";

#define ZoneDataSRV(X) (\
	(X)->ZoneService == ZoneServiceUpdate ? ((X)->ZonePrivate ? PRIVATE_UPDATE_SERVICE_TYPE : PUBLIC_UPDATE_SERVICE_TYPE) : \
	(X)->ZoneService == ZoneServiceQuery  ? ((X)->ZonePrivate ? PRIVATE_QUERY_SERVICE_TYPE  : (const domainname*)""     ) : \
	(X)->ZoneService == ZoneServiceLLQ    ? ((X)->ZonePrivate ? PRIVATE_LLQ_SERVICE_TYPE    : PUBLIC_LLQ_SERVICE_TYPE   ) : (const domainname*)"")

// Forward reference: GetZoneData_StartQuery references GetZoneData_QuestionCallback, and
// GetZoneData_QuestionCallback calls GetZoneData_StartQuery
mDNSlocal mStatus GetZoneData_StartQuery(mDNS *const m, ZoneData *zd, mDNSu16 qtype);

// GetZoneData_QuestionCallback is called from normal client callback context (core API calls allowed)
mDNSlocal void GetZoneData_QuestionCallback(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, QC_result AddRecord)
	{
	ZoneData *zd = (ZoneData*)question->QuestionContext;

	debugf("GetZoneData_QuestionCallback: %s %s", AddRecord ? "Add" : "Rmv", RRDisplayString(m, answer));

	if (!AddRecord) return;												// Don't care about REMOVE events
	if (AddRecord == QC_addnocache && answer->rdlength == 0) return;	// Don't care about transient failure indications
	if (answer->rrtype != question->qtype) return;						// Don't care about CNAMEs

	if (answer->rrtype == kDNSType_SOA)
		{
		debugf("GetZoneData GOT SOA %s", RRDisplayString(m, answer));
		mDNS_StopQuery(m, question);
		if (answer->rdlength)
			{
			AssignDomainName(&zd->ZoneName, answer->name);
			zd->ZoneClass = answer->rrclass;
			AssignDomainName(&zd->question.qname, &zd->ZoneName);
			GetZoneData_StartQuery(m, zd, kDNSType_SRV);
			}
		else if (zd->CurrentSOA->c[0])
			{
			zd->CurrentSOA = (domainname *)(zd->CurrentSOA->c + zd->CurrentSOA->c[0]+1);
			AssignDomainName(&zd->question.qname, zd->CurrentSOA);
			GetZoneData_StartQuery(m, zd, kDNSType_SOA);
			}
		else
			{
			LogInfo("GetZoneData recursed to root label of %##s without finding SOA", zd->ChildName.c);
			zd->ZoneDataCallback(m, mStatus_NoSuchNameErr, zd);
			mDNSPlatformMemFree(zd);
			}
		}
	else if (answer->rrtype == kDNSType_SRV)
		{
		debugf("GetZoneData GOT SRV %s", RRDisplayString(m, answer));
		mDNS_StopQuery(m, question);
// Right now we don't want to fail back to non-encrypted operations
// If the AuthInfo has the AutoTunnel field set, then we want private or nothing
// <rdar://problem/5687667> BTMM: Don't fallback to unencrypted operations when SRV lookup fails
#if 0
		if (!answer->rdlength && zd->ZonePrivate && zd->ZoneService != ZoneServiceQuery)
			{
			zd->ZonePrivate = mDNSfalse;	// Causes ZoneDataSRV() to yield a different SRV name when building the query
			GetZoneData_StartQuery(m, zd, kDNSType_SRV);		// Try again, non-private this time
			}
		else
#endif
			{
			if (answer->rdlength)
				{
				AssignDomainName(&zd->Host, &answer->rdata->u.srv.target);
				zd->Port = answer->rdata->u.srv.port;
				AssignDomainName(&zd->question.qname, &zd->Host);
				GetZoneData_StartQuery(m, zd, kDNSType_A);
				}
			else
				{
				zd->ZonePrivate = mDNSfalse;
				zd->Host.c[0] = 0;
				zd->Port = zeroIPPort;
				zd->Addr = zeroAddr;
				zd->ZoneDataCallback(m, mStatus_NoError, zd);
				mDNSPlatformMemFree(zd);
				}
			}
		}
	else if (answer->rrtype == kDNSType_A)
		{
		debugf("GetZoneData GOT A %s", RRDisplayString(m, answer));
		mDNS_StopQuery(m, question);
		zd->Addr.type  = mDNSAddrType_IPv4;
		zd->Addr.ip.v4 = (answer->rdlength == 4) ? answer->rdata->u.ipv4 : zerov4Addr;
		// In order to simulate firewalls blocking our outgoing TCP connections, returning immediate ICMP errors or TCP resets,
		// the code below will make us try to connect to loopback, resulting in an immediate "port unreachable" failure.
		// This helps us test to make sure we handle this case gracefully
		// <rdar://problem/5607082> BTMM: mDNSResponder taking 100 percent CPU after upgrading to 10.5.1
#if 0
		zd->Addr.ip.v4.b[0] = 127;
		zd->Addr.ip.v4.b[1] = 0;
		zd->Addr.ip.v4.b[2] = 0;
		zd->Addr.ip.v4.b[3] = 1;
#endif
		zd->ZoneDataCallback(m, mStatus_NoError, zd);
		mDNSPlatformMemFree(zd);
		}
	}

// GetZoneData_StartQuery is called from normal client context (lock not held, or client callback)
mDNSlocal mStatus GetZoneData_StartQuery(mDNS *const m, ZoneData *zd, mDNSu16 qtype)
	{
	if (qtype == kDNSType_SRV)
		{
		AssignDomainName(&zd->question.qname, ZoneDataSRV(zd));
		AppendDomainName(&zd->question.qname, &zd->ZoneName);
		debugf("lookupDNSPort %##s", zd->question.qname.c);
		}

	zd->question.ThisQInterval       = -1;		// So that GetZoneData_QuestionCallback() knows whether to cancel this question (Is this necessary?)
	zd->question.InterfaceID         = mDNSInterface_Any;
	zd->question.Target              = zeroAddr;
	//zd->question.qname.c[0]        = 0;			// Already set
	zd->question.qtype               = qtype;
	zd->question.qclass              = kDNSClass_IN;
	zd->question.LongLived           = mDNSfalse;
	zd->question.ExpectUnique        = mDNStrue;
	zd->question.ForceMCast          = mDNSfalse;
	zd->question.ReturnIntermed      = mDNStrue;
	zd->question.QuestionCallback    = GetZoneData_QuestionCallback;
	zd->question.QuestionContext     = zd;

	//LogMsg("GetZoneData_StartQuery %##s (%s) %p", zd->question.qname.c, DNSTypeName(zd->question.qtype), zd->question.Private);
	return(mDNS_StartQuery(m, &zd->question));
	}

// StartGetZoneData is an internal routine (i.e. must be called with the lock already held)
mDNSexport ZoneData *StartGetZoneData(mDNS *const m, const domainname *const name, const ZoneService target, ZoneDataCallback callback, void *ZoneDataContext)
	{
	DomainAuthInfo *AuthInfo = GetAuthInfoForName_internal(m, name);
	int initialskip = (AuthInfo && AuthInfo->AutoTunnel) ? DomainNameLength(name) - DomainNameLength(&AuthInfo->domain) : 0;
	ZoneData *zd = (ZoneData*)mDNSPlatformMemAllocate(sizeof(ZoneData));
	if (!zd) { LogMsg("ERROR: StartGetZoneData - mDNSPlatformMemAllocate failed"); return mDNSNULL; }
	mDNSPlatformMemZero(zd, sizeof(ZoneData));
	AssignDomainName(&zd->ChildName, name);
	zd->ZoneService      = target;
	zd->CurrentSOA       = (domainname *)(&zd->ChildName.c[initialskip]);
	zd->ZoneName.c[0]    = 0;
	zd->ZoneClass        = 0;
	zd->Host.c[0]        = 0;
	zd->Port             = zeroIPPort;
	zd->Addr             = zeroAddr;
	zd->ZonePrivate      = AuthInfo && AuthInfo->AutoTunnel ? mDNStrue : mDNSfalse;
	zd->ZoneDataCallback = callback;
	zd->ZoneDataContext  = ZoneDataContext;

	zd->question.QuestionContext = zd;
	AssignDomainName(&zd->question.qname, zd->CurrentSOA);

	mDNS_DropLockBeforeCallback();		// GetZoneData_StartQuery expects to be called from a normal callback, so we emulate that here
	GetZoneData_StartQuery(m, zd, kDNSType_SOA);
	mDNS_ReclaimLockAfterCallback();

	return zd;
	}

// GetZoneData queries are a special case -- even if we have a key for them, we don't do them privately,
// because that would result in an infinite loop (i.e. to do a private query we first need to get
// the _dns-query-tls SRV record for the zone, and we can't do *that* privately because to do so
// we'd need to already know the _dns-query-tls SRV record.
// Also, as a general rule, we never do SOA queries privately
mDNSexport DomainAuthInfo *GetAuthInfoForQuestion(mDNS *m, const DNSQuestion *const q)	// Must be called with lock held
	{
	if (q->QuestionCallback == GetZoneData_QuestionCallback) return(mDNSNULL);
	if (q->qtype            == kDNSType_SOA                ) return(mDNSNULL);
	return(GetAuthInfoForName_internal(m, &q->qname));
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - host name and interface management
#endif

// Called in normal client context (lock not held)
mDNSlocal void CompleteSRVNatMap(mDNS *m, NATTraversalInfo *n)
	{
	ServiceRecordSet *srs = (ServiceRecordSet *)n->clientContext;
	debugf("SRVNatMap complete %.4a IntPort %u ExternalPort %u NATLease %u", &n->ExternalAddress, mDNSVal16(n->IntPort), mDNSVal16(n->ExternalPort), n->NATLease);

	if (!srs) { LogMsg("CompleteSRVNatMap called with unknown ServiceRecordSet object"); return; }
	if (!n->NATLease) return;

	mDNS_Lock(m);
	if (!mDNSIPv4AddressIsZero(srs->SRSUpdateServer.ip.v4))
		SendServiceRegistration(m, srs);	// non-zero server address means we already have necessary zone data to send update
	else
		{
		// SHOULD NEVER HAPPEN!
		LogInfo("ERROR: CompleteSRVNatMap called but srs->SRSUpdateServer.ip.v4 is zero!");
		srs->state = regState_FetchingZoneData;
		if (srs->srs_nta) CancelGetZoneData(m, srs->srs_nta); // Make sure we cancel old one before we start a new one
		srs->srs_nta = StartGetZoneData(m, srs->RR_SRV.resrec.name, ZoneServiceUpdate, ServiceRegistrationGotZoneData, srs);
		}
	mDNS_Unlock(m);
	}

mDNSlocal void StartSRVNatMap(mDNS *m, ServiceRecordSet *srs)
	{
	const mDNSu8 *p = srs->RR_PTR.resrec.name->c;
	if (p[0]) p += 1 + p[0];
	if      (SameDomainLabel(p, (mDNSu8 *)"\x4" "_tcp")) srs->NATinfo.Protocol = NATOp_MapTCP;
	else if (SameDomainLabel(p, (mDNSu8 *)"\x4" "_udp")) srs->NATinfo.Protocol = NATOp_MapUDP;
	else { LogMsg("StartSRVNatMap: could not determine transport protocol of service %##s", srs->RR_SRV.resrec.name->c); return; }
	
	if (srs->NATinfo.clientContext) mDNS_StopNATOperation_internal(m, &srs->NATinfo);
	// Don't try to set IntPort here --
	// SendServiceRegistration overwrites srs->RR_SRV.resrec.rdata->u.srv.port with external (mapped) port number
	//srs->NATinfo.IntPort      = srs->RR_SRV.resrec.rdata->u.srv.port;
	srs->NATinfo.RequestedPort  = srs->RR_SRV.resrec.rdata->u.srv.port;
	srs->NATinfo.NATLease       = 0;		// Request default lease
	srs->NATinfo.clientCallback = CompleteSRVNatMap;
	srs->NATinfo.clientContext  = srs;
	mDNS_StartNATOperation_internal(m, &srs->NATinfo);
	}

// Called in normal callback context (i.e. mDNS_busy and mDNS_reentrancy are both 1)
mDNSexport void ServiceRegistrationGotZoneData(mDNS *const m, mStatus err, const ZoneData *zoneData)
	{
	ServiceRecordSet *srs = (ServiceRecordSet *)zoneData->ZoneDataContext;

	if (m->mDNS_busy != m->mDNS_reentrancy)
		LogMsg("ServiceRegistrationGotZoneData: mDNS_busy (%ld) != mDNS_reentrancy (%ld)", m->mDNS_busy, m->mDNS_reentrancy);

	if (!srs->RR_SRV.resrec.rdata)
		{ LogMsg("ServiceRegistrationGotZoneData: ERROR: srs->RR_SRV.resrec.rdata is NULL"); return; }

	srs->srs_nta = mDNSNULL;

	// Start off assuming we're going to use a lease
	// If we get an error from the server, and the update port as given in the SRV record is 53, then we'll retry without the lease option
	srs->srs_uselease = mDNStrue;

	if (err || !zoneData) return;

	if (mDNSIPPortIsZero(zoneData->Port) || mDNSAddressIsZero(&zoneData->Addr)) return;

	// cache zone data
	AssignDomainName(&srs->zone, &zoneData->ZoneName);
	srs->SRSUpdateServer.type = mDNSAddrType_IPv4;
	srs->SRSUpdateServer      = zoneData->Addr;
	srs->SRSUpdatePort        = zoneData->Port;
	srs->Private              = zoneData->ZonePrivate;

	srs->RR_SRV.LastAPTime     = m->timenow;
	srs->RR_SRV.ThisAPInterval = 0;

	debugf("ServiceRegistrationGotZoneData My IPv4 %#a%s Server %#a:%d%s for %##s",
		&m->AdvertisedV4, mDNSv4AddrIsRFC1918(&m->AdvertisedV4.ip.v4) ? " (RFC1918)" : "",
		&srs->SRSUpdateServer, mDNSVal16(srs->SRSUpdatePort), mDNSAddrIsRFC1918(&srs->SRSUpdateServer) ? " (RFC1918)" : "",
		srs->RR_SRV.resrec.name->c);

	// If we have non-zero service port (always?)
	// and a private address, and update server is non-private
	// and this service is AutoTarget
	// then initiate a NAT mapping request. On completion it will do SendServiceRegistration() for us
	if (!mDNSIPPortIsZero(srs->RR_SRV.resrec.rdata->u.srv.port) &&
		mDNSv4AddrIsRFC1918(&m->AdvertisedV4.ip.v4) && !mDNSAddrIsRFC1918(&srs->SRSUpdateServer) &&
		srs->RR_SRV.AutoTarget == Target_AutoHostAndNATMAP)
		{
		srs->state = regState_NATMap;
		debugf("ServiceRegistrationGotZoneData StartSRVNatMap");
		StartSRVNatMap(m, srs);
		}
	else
		{
		mDNS_Lock(m);
		SendServiceRegistration(m, srs);
		mDNS_Unlock(m);
		}
	}

mDNSlocal void SendServiceDeregistration(mDNS *m, ServiceRecordSet *srs)
	{
	mDNSOpaque16 id;
	mDNSu8 *ptr = m->omsg.data;
	mDNSu8 *end = (mDNSu8 *)&m->omsg + sizeof(DNSMessage);
	mStatus err = mStatus_UnknownErr;
	mDNSu32 i;

	if (mDNSIPv4AddressIsZero(srs->SRSUpdateServer.ip.v4))	// Don't know our UpdateServer yet
		{
		srs->RR_SRV.LastAPTime = m->timenow;
		if (srs->RR_SRV.ThisAPInterval < mDNSPlatformOneSecond * 5)
			srs->RR_SRV.ThisAPInterval = mDNSPlatformOneSecond * 5;
		return;
		}

	id = mDNS_NewMessageID(m);
	InitializeDNSMessage(&m->omsg.h, id, UpdateReqFlags);

	// put zone
	ptr = putZone(&m->omsg, ptr, end, &srs->zone, mDNSOpaque16fromIntVal(srs->RR_SRV.resrec.rrclass));
	if (!ptr) { LogMsg("ERROR: SendServiceDeregistration - putZone"); err = mStatus_UnknownErr; goto exit; }

	if (!(ptr = putDeleteAllRRSets(&m->omsg, ptr, srs->RR_SRV.resrec.name))) { err = mStatus_UnknownErr; goto exit; } // this deletes SRV, TXT, and Extras
	if (!(ptr = putDeletionRecord(&m->omsg, ptr, &srs->RR_PTR.resrec))) { err = mStatus_UnknownErr; goto exit; }
	for (i = 0; i < srs->NumSubTypes; i++)
		if (!(ptr = putDeletionRecord(&m->omsg, ptr, &srs->SubTypes[i].resrec))) { err = mStatus_UnknownErr; goto exit; }

	srs->id    = id;
	srs->state = regState_DeregPending;
	srs->RR_SRV.expire = 0;		// Indicate that we have no active registration any more

	if (srs->Private)
		{
		LogInfo("SendServiceDeregistration TCP %p %s", srs->tcp, ARDisplayString(m, &srs->RR_SRV));
		if (srs->tcp) LogInfo("SendServiceDeregistration: Disposing existing TCP connection for %s", ARDisplayString(m, &srs->RR_SRV));
		if (srs->tcp) DisposeTCPConn(srs->tcp);
		srs->tcp = MakeTCPConn(m, &m->omsg, ptr, kTCPSocketFlags_UseTLS, &srs->SRSUpdateServer, srs->SRSUpdatePort, mDNSNULL, srs, mDNSNULL);
		if (!srs->tcp) srs->RR_SRV.ThisAPInterval = mDNSPlatformOneSecond * 5; // If failed to make TCP connection, try again in ten seconds (5*2)
		else if (srs->RR_SRV.ThisAPInterval < mDNSPlatformOneSecond * 30) srs->RR_SRV.ThisAPInterval = mDNSPlatformOneSecond * 30;
		}
	else
		{
		err = mDNSSendDNSMessage(m, &m->omsg, ptr, mDNSInterface_Any, mDNSNULL, &srs->SRSUpdateServer, srs->SRSUpdatePort, mDNSNULL, GetAuthInfoForName_internal(m, srs->RR_SRV.resrec.name));
		if (err && err != mStatus_TransientErr) { debugf("ERROR: SendServiceDeregistration - mDNSSendDNSMessage - %d", err); goto exit; }
		}

	SetRecordRetry(m, &srs->RR_SRV, err);
	return;

exit:
	if (err)
		{
		LogMsg("SendServiceDeregistration ERROR formatting message %d!! Permanently abandoning service registration %##s", err, srs->RR_SRV.resrec.name->c);
		unlinkSRS(m, srs);
		srs->state = regState_Unregistered;
		}
	}

// Called with lock held
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
	mDNSBool HaveZoneData  = !mDNSIPv4AddressIsZero(srs->SRSUpdateServer.ip.v4);

	// Nat state change if:
	// We were behind a NAT, and now we are behind a new NAT, or
	// We're not behind a NAT but our port was previously mapped to a different external port
	// We were not behind a NAT and now we are

	mDNSIPPort port        = srs->RR_SRV.resrec.rdata->u.srv.port;
	mDNSBool NowNeedNATMAP = (srs->RR_SRV.AutoTarget == Target_AutoHostAndNATMAP && !mDNSIPPortIsZero(port) && mDNSv4AddrIsRFC1918(&m->AdvertisedV4.ip.v4) && !mDNSAddrIsRFC1918(&srs->SRSUpdateServer));
	mDNSBool WereBehindNAT = (srs->NATinfo.clientContext != mDNSNULL);
	mDNSBool PortWasMapped = (srs->NATinfo.clientContext && !mDNSSameIPPort(srs->NATinfo.RequestedPort, port));		// I think this is always false -- SC Sept 07
	mDNSBool NATChanged    = (!WereBehindNAT && NowNeedNATMAP) || (!NowNeedNATMAP && PortWasMapped);

	debugf("UpdateSRV %##s newtarget %##s TargetChanged %d HaveZoneData %d port %d NowNeedNATMAP %d WereBehindNAT %d PortWasMapped %d NATChanged %d",
		srs->RR_SRV.resrec.name->c, newtarget,
		TargetChanged, HaveZoneData, mDNSVal16(port), NowNeedNATMAP, WereBehindNAT, PortWasMapped, NATChanged);

	if (m->mDNS_busy != m->mDNS_reentrancy+1)
		LogMsg("UpdateSRV: Lock not held! mDNS_busy (%ld) mDNS_reentrancy (%ld)", m->mDNS_busy, m->mDNS_reentrancy);

	if (!TargetChanged && !NATChanged) return;

	switch(srs->state)
		{
		case regState_FetchingZoneData:
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
				debugf("UpdateSRV: %s service %##s", HaveZoneData ? (NATChanged && NowNeedNATMAP ? "Starting Port Map for" : "Registering") : "Getting Zone Data for", srs->RR_SRV.resrec.name->c);
				if (!HaveZoneData)
					{
					srs->state = regState_FetchingZoneData;
					if (srs->srs_nta) CancelGetZoneData(m, srs->srs_nta); // Make sure we cancel old one before we start a new one
					srs->srs_nta = StartGetZoneData(m, srs->RR_SRV.resrec.name, ZoneServiceUpdate, ServiceRegistrationGotZoneData, srs);
					}
				else
					{
					if (srs->NATinfo.clientContext && (NATChanged || !NowNeedNATMAP))
						{
						mDNS_StopNATOperation_internal(m, &srs->NATinfo);
						srs->NATinfo.clientContext = mDNSNULL;
						}
					if (NATChanged && NowNeedNATMAP && srs->RR_SRV.AutoTarget == Target_AutoHostAndNATMAP)
						{ srs->state = regState_NATMap; StartSRVNatMap(m, srs); }
					else SendServiceRegistration(m, srs);
					}
				}
			return;

		case regState_Registered:
			// target or nat changed.  deregister service.  upon completion, we'll look for a new target
			debugf("UpdateSRV: SRV record changed for service %##s - deregistering (will re-register with new SRV)",  srs->RR_SRV.resrec.name->c);
			for (e = srs->Extras; e; e = e->next) e->r.state = regState_ExtraQueued;	// extra will be re-registed if the service is re-registered
			srs->SRVChanged = mDNStrue;
			SendServiceDeregistration(m, srs);
			return;

		default: LogMsg("UpdateSRV: Unknown state %d for %##s", srs->state, srs->RR_SRV.resrec.name->c);
		}
	}

// Called with lock held
mDNSlocal void UpdateSRVRecords(mDNS *m)
	{
	debugf("UpdateSRVRecords%s", m->SleepState ? " (ignored due to SleepState)" : "");
	if (m->SleepState) return;

	if (CurrentServiceRecordSet)
		LogMsg("UpdateSRVRecords ERROR CurrentServiceRecordSet already set");
	CurrentServiceRecordSet = m->ServiceRegistrations;
	
	while (CurrentServiceRecordSet)
		{
		ServiceRecordSet *s = CurrentServiceRecordSet;
		CurrentServiceRecordSet = CurrentServiceRecordSet->uDNS_next;
		UpdateSRV(m, s);
		}

	mDNS_DropLockBeforeCallback();		// mDNS_SetFQDN expects to be called without the lock held, so we emulate that here
	mDNS_SetFQDN(m);
	mDNS_ReclaimLockAfterCallback();
	}

// Forward reference: AdvertiseHostname references HostnameCallback, and HostnameCallback calls AdvertiseHostname
mDNSlocal void HostnameCallback(mDNS *const m, AuthRecord *const rr, mStatus result);

// Called in normal client context (lock not held)
mDNSlocal void hostnameGetPublicAddressCallback(mDNS *m, NATTraversalInfo *n)
	{
	HostnameInfo *h = (HostnameInfo *)n->clientContext;

	if (!h) { LogMsg("RegisterHostnameRecord: registration cancelled"); return; }

	if (!n->Result)
		{
		if (mDNSIPv4AddressIsZero(n->ExternalAddress) || mDNSv4AddrIsRFC1918(&n->ExternalAddress)) return;
		
		if (h->arv4.resrec.RecordType)
			{
			if (mDNSSameIPv4Address(h->arv4.resrec.rdata->u.ipv4, n->ExternalAddress)) return;	// If address unchanged, do nothing
			LogInfo("Updating hostname %##s IPv4 from %.4a to %.4a (NAT gateway's external address)",
				h->arv4.resrec.name->c, &h->arv4.resrec.rdata->u.ipv4, &n->ExternalAddress);
			mDNS_Deregister(m, &h->arv4);	// mStatus_MemFree callback will re-register with new address
			}
		else
			{
			LogInfo("Advertising hostname %##s IPv4 %.4a (NAT gateway's external address)", h->arv4.resrec.name->c, &n->ExternalAddress);
			h->arv4.resrec.RecordType = kDNSRecordTypeKnownUnique;
			h->arv4.resrec.rdata->u.ipv4 = n->ExternalAddress;
			mDNS_Register(m, &h->arv4);
			}
		}
	}

// register record or begin NAT traversal
mDNSlocal void AdvertiseHostname(mDNS *m, HostnameInfo *h)
	{
	if (!mDNSIPv4AddressIsZero(m->AdvertisedV4.ip.v4) && h->arv4.resrec.RecordType == kDNSRecordTypeUnregistered)
		{
		mDNS_SetupResourceRecord(&h->arv4, mDNSNULL, mDNSInterface_Any, kDNSType_A, kHostNameTTL, kDNSRecordTypeUnregistered, HostnameCallback, h);
		AssignDomainName(&h->arv4.namestorage, &h->fqdn);
		h->arv4.resrec.rdata->u.ipv4 = m->AdvertisedV4.ip.v4;
		h->arv4.state = regState_Unregistered;
		if (mDNSv4AddrIsRFC1918(&m->AdvertisedV4.ip.v4))
			{
			// If we already have a NAT query active, stop it and restart it to make sure we get another callback
			if (h->natinfo.clientContext) mDNS_StopNATOperation_internal(m, &h->natinfo);
			h->natinfo.Protocol         = 0;
			h->natinfo.IntPort          = zeroIPPort;
			h->natinfo.RequestedPort    = zeroIPPort;
			h->natinfo.NATLease         = 0;
			h->natinfo.clientCallback   = hostnameGetPublicAddressCallback;
			h->natinfo.clientContext    = h;
			mDNS_StartNATOperation_internal(m, &h->natinfo);
			}
		else
			{
			LogInfo("Advertising hostname %##s IPv4 %.4a", h->arv4.resrec.name->c, &m->AdvertisedV4.ip.v4);
			h->arv4.resrec.RecordType = kDNSRecordTypeKnownUnique;
			mDNS_Register_internal(m, &h->arv4);
			}
		}

	if (!mDNSIPv6AddressIsZero(m->AdvertisedV6.ip.v6) && h->arv6.resrec.RecordType == kDNSRecordTypeUnregistered)
		{
		mDNS_SetupResourceRecord(&h->arv6, mDNSNULL, mDNSInterface_Any, kDNSType_AAAA, kHostNameTTL, kDNSRecordTypeKnownUnique, HostnameCallback, h);
		AssignDomainName(&h->arv6.namestorage, &h->fqdn);
		h->arv6.resrec.rdata->u.ipv6 = m->AdvertisedV6.ip.v6;
		h->arv6.state = regState_Unregistered;
		LogInfo("Advertising hostname %##s IPv6 %.16a", h->arv6.resrec.name->c, &m->AdvertisedV6.ip.v6);
		mDNS_Register_internal(m, &h->arv6);
		}
	}

mDNSlocal void HostnameCallback(mDNS *const m, AuthRecord *const rr, mStatus result)
	{
	HostnameInfo *hi = (HostnameInfo *)rr->RecordContext;

	if (result == mStatus_MemFree)
		{
		if (hi)
			{
			// If we're still in the Hostnames list, update to new address
			HostnameInfo *i;
			LogInfo("HostnameCallback: Got mStatus_MemFree for %p %p %s", hi, rr, ARDisplayString(m, rr));
			for (i = m->Hostnames; i; i = i->next)
				if (rr == &i->arv4 || rr == &i->arv6)
					{ mDNS_Lock(m); AdvertiseHostname(m, i); mDNS_Unlock(m); return; }

			// Else, we're not still in the Hostnames list, so free the memory
			if (hi->arv4.resrec.RecordType == kDNSRecordTypeUnregistered &&
				hi->arv6.resrec.RecordType == kDNSRecordTypeUnregistered)
				{
				if (hi->natinfo.clientContext) mDNS_StopNATOperation_internal(m, &hi->natinfo);
				hi->natinfo.clientContext = mDNSNULL;
				mDNSPlatformMemFree(hi);	// free hi when both v4 and v6 AuthRecs deallocated
				}
			}
		return;
		}

	if (result)
		{
		// don't unlink or free - we can retry when we get a new address/router
		if (rr->resrec.rrtype == kDNSType_A)
			LogMsg("HostnameCallback: Error %d for registration of %##s IP %.4a", result, rr->resrec.name->c, &rr->resrec.rdata->u.ipv4);
		else
			LogMsg("HostnameCallback: Error %d for registration of %##s IP %.16a", result, rr->resrec.name->c, &rr->resrec.rdata->u.ipv6);
		if (!hi) { mDNSPlatformMemFree(rr); return; }
		if (rr->state != regState_Unregistered) LogMsg("Error: HostnameCallback invoked with error code for record not in regState_Unregistered!");

		if (hi->arv4.state == regState_Unregistered &&
			hi->arv6.state == regState_Unregistered)
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
		LogInfo("Registered hostname %##s IP %.4a", rr->resrec.name->c, &rr->resrec.rdata->u.ipv4);
	else
		LogInfo("Registered hostname %##s IP %.16a", rr->resrec.name->c, &rr->resrec.rdata->u.ipv6);

	rr->RecordContext = (void *)hi->StatusContext;
	if (hi->StatusCallback)
		hi->StatusCallback(m, rr, result); // client may NOT make API calls here
	rr->RecordContext = (void *)hi;
	}

mDNSlocal void FoundStaticHostname(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, QC_result AddRecord)
	{
	const domainname *pktname = &answer->rdata->u.name;
	domainname *storedname = &m->StaticHostname;
	HostnameInfo *h = m->Hostnames;

	(void)question;

	debugf("FoundStaticHostname: %##s -> %##s (%s)", question->qname.c, answer->rdata->u.name.c, AddRecord ? "added" : "removed");
	if (AddRecord && !SameDomainName(pktname, storedname))
		{
		AssignDomainName(storedname, pktname);
		while (h)
			{
			if (h->arv4.state == regState_FetchingZoneData || h->arv4.state == regState_Pending || h->arv4.state == regState_NATMap ||
				h->arv6.state == regState_FetchingZoneData || h->arv6.state == regState_Pending)
				{
				// if we're in the process of registering a dynamic hostname, delay SRV update so we don't have to reregister services if the dynamic name succeeds
				m->NextSRVUpdate = NonZeroTime(m->timenow + 5 * mDNSPlatformOneSecond);
				return;
				}
			h = h->next;
			}
		mDNS_Lock(m);
		UpdateSRVRecords(m);
		mDNS_Unlock(m);
		}
	else if (!AddRecord && SameDomainName(pktname, storedname))
		{
		mDNS_Lock(m);
		storedname->c[0] = 0;
		UpdateSRVRecords(m);
		mDNS_Unlock(m);
		}
	}

// Called with lock held
mDNSlocal void GetStaticHostname(mDNS *m)
	{
	char buf[MAX_REVERSE_MAPPING_NAME_V4];
	DNSQuestion *q = &m->ReverseMap;
	mDNSu8 *ip = m->AdvertisedV4.ip.v4.b;
	mStatus err;

	if (m->ReverseMap.ThisQInterval != -1) return; // already running
	if (mDNSIPv4AddressIsZero(m->AdvertisedV4.ip.v4)) return;

	mDNSPlatformMemZero(q, sizeof(*q));
	// Note: This is reverse order compared to a normal dotted-decimal IP address, so we can't use our customary "%.4a" format code
	mDNS_snprintf(buf, sizeof(buf), "%d.%d.%d.%d.in-addr.arpa.", ip[3], ip[2], ip[1], ip[0]);
	if (!MakeDomainNameFromDNSNameString(&q->qname, buf)) { LogMsg("Error: GetStaticHostname - bad name %s", buf); return; }

	q->InterfaceID      = mDNSInterface_Any;
	q->Target           = zeroAddr;
	q->qtype            = kDNSType_PTR;
	q->qclass           = kDNSClass_IN;
	q->LongLived        = mDNSfalse;
	q->ExpectUnique     = mDNSfalse;
	q->ForceMCast       = mDNSfalse;
	q->ReturnIntermed   = mDNStrue;
	q->QuestionCallback = FoundStaticHostname;
	q->QuestionContext  = mDNSNULL;

	LogInfo("GetStaticHostname: %##s (%s)", q->qname.c, DNSTypeName(q->qtype));
	err = mDNS_StartQuery_internal(m, q);
	if (err) LogMsg("Error: GetStaticHostname - StartQuery returned error %d", err);
	}

mDNSexport void mDNS_AddDynDNSHostName(mDNS *m, const domainname *fqdn, mDNSRecordCallback *StatusCallback, const void *StatusContext)
   {
	HostnameInfo **ptr = &m->Hostnames;

	LogInfo("mDNS_AddDynDNSHostName %##s", fqdn);

	while (*ptr && !SameDomainName(fqdn, &(*ptr)->fqdn)) ptr = &(*ptr)->next;
	if (*ptr) { LogMsg("DynDNSHostName %##s already in list", fqdn->c); return; }

	// allocate and format new address record
	*ptr = mDNSPlatformMemAllocate(sizeof(**ptr));
	if (!*ptr) { LogMsg("ERROR: mDNS_AddDynDNSHostName - malloc"); return; }

	mDNSPlatformMemZero(*ptr, sizeof(**ptr));
	AssignDomainName(&(*ptr)->fqdn, fqdn);
	(*ptr)->arv4.state     = regState_Unregistered;
	(*ptr)->arv6.state     = regState_Unregistered;
	(*ptr)->StatusCallback = StatusCallback;
	(*ptr)->StatusContext  = StatusContext;

	AdvertiseHostname(m, *ptr);
	}

mDNSexport void mDNS_RemoveDynDNSHostName(mDNS *m, const domainname *fqdn)
	{
	HostnameInfo **ptr = &m->Hostnames;

	LogInfo("mDNS_RemoveDynDNSHostName %##s", fqdn);

	while (*ptr && !SameDomainName(fqdn, &(*ptr)->fqdn)) ptr = &(*ptr)->next;
	if (!*ptr) LogMsg("mDNS_RemoveDynDNSHostName: no such domainname %##s", fqdn->c);
	else
		{
		HostnameInfo *hi = *ptr;
		// We do it this way because, if we have no active v6 record, the "mDNS_Deregister_internal(m, &hi->arv4);"
		// below could free the memory, and we have to make sure we don't touch hi fields after that.
		mDNSBool f4 = hi->arv4.resrec.RecordType != kDNSRecordTypeUnregistered && hi->arv4.state != regState_Unregistered;
		mDNSBool f6 = hi->arv6.resrec.RecordType != kDNSRecordTypeUnregistered && hi->arv6.state != regState_Unregistered;
		if (f4) LogInfo("mDNS_RemoveDynDNSHostName removing v4 %##s", fqdn);
		if (f6) LogInfo("mDNS_RemoveDynDNSHostName removing v6 %##s", fqdn);
		*ptr = (*ptr)->next; // unlink
		if (f4) mDNS_Deregister_internal(m, &hi->arv4, mDNS_Dereg_normal);
		if (f6) mDNS_Deregister_internal(m, &hi->arv6, mDNS_Dereg_normal);
		// When both deregistrations complete we'll free the memory in the mStatus_MemFree callback
		}
	UpdateSRVRecords(m);
	}

// Currently called without holding the lock
// Maybe we should change that?
mDNSexport void mDNS_SetPrimaryInterfaceInfo(mDNS *m, const mDNSAddr *v4addr, const mDNSAddr *v6addr, const mDNSAddr *router)
	{
	mDNSBool v4Changed, v6Changed, RouterChanged;

	if (m->mDNS_busy != m->mDNS_reentrancy)
		LogMsg("mDNS_SetPrimaryInterfaceInfo: mDNS_busy (%ld) != mDNS_reentrancy (%ld)", m->mDNS_busy, m->mDNS_reentrancy);

	if (v4addr && v4addr->type != mDNSAddrType_IPv4) { LogMsg("mDNS_SetPrimaryInterfaceInfo v4 address - incorrect type.  Discarding. %#a", v4addr); return; }
	if (v6addr && v6addr->type != mDNSAddrType_IPv6) { LogMsg("mDNS_SetPrimaryInterfaceInfo v6 address - incorrect type.  Discarding. %#a", v6addr); return; }
	if (router && router->type != mDNSAddrType_IPv4) { LogMsg("mDNS_SetPrimaryInterfaceInfo passed non-v4 router.  Discarding. %#a",        router); return; }

	mDNS_Lock(m);

	if (v4addr && !mDNSv4AddressIsLinkLocal(&v4addr->ip.v4)) v6addr = mDNSNULL;

	v4Changed     = !mDNSSameIPv4Address(m->AdvertisedV4.ip.v4, v4addr ? v4addr->ip.v4 : zerov4Addr);
	v6Changed     = !mDNSSameIPv6Address(m->AdvertisedV6.ip.v6, v6addr ? v6addr->ip.v6 : zerov6Addr);
	RouterChanged = !mDNSSameIPv4Address(m->Router.ip.v4,       router ? router->ip.v4 : zerov4Addr);

	if (v4addr && (v4Changed || RouterChanged))
		debugf("mDNS_SetPrimaryInterfaceInfo: address changed from %#a to %#a", &m->AdvertisedV4, v4addr);

	if (v4addr) m->AdvertisedV4 = *v4addr; else m->AdvertisedV4.ip.v4 = zerov4Addr;
	if (v6addr) m->AdvertisedV6 = *v6addr; else m->AdvertisedV6.ip.v6 = zerov6Addr;
	if (router) m->Router       = *router; else m->Router      .ip.v4 = zerov4Addr;
	// setting router to zero indicates that nat mappings must be reestablished when router is reset

	if (v4Changed || RouterChanged || v6Changed)
		{
		HostnameInfo *i;
		LogInfo("mDNS_SetPrimaryInterfaceInfo: %s%s%s%#a %#a %#a",
			v4Changed     ? "v4Changed "     : "",
			RouterChanged ? "RouterChanged " : "",
			v6Changed     ? "v6Changed "     : "", v4addr, v6addr, router);

		for (i = m->Hostnames; i; i = i->next)
			{
			LogInfo("mDNS_SetPrimaryInterfaceInfo updating host name registrations for %##s", i->fqdn.c);
	
			if (i->arv4.resrec.RecordType > kDNSRecordTypeDeregistering &&
				!mDNSSameIPv4Address(i->arv4.resrec.rdata->u.ipv4, m->AdvertisedV4.ip.v4))
				{
				LogInfo("mDNS_SetPrimaryInterfaceInfo deregistering %s", ARDisplayString(m, &i->arv4));
				mDNS_Deregister_internal(m, &i->arv4, mDNS_Dereg_normal);
				}
	
			if (i->arv6.resrec.RecordType > kDNSRecordTypeDeregistering &&
				!mDNSSameIPv6Address(i->arv6.resrec.rdata->u.ipv6, m->AdvertisedV6.ip.v6))
				{
				LogInfo("mDNS_SetPrimaryInterfaceInfo deregistering %s", ARDisplayString(m, &i->arv6));
				mDNS_Deregister_internal(m, &i->arv6, mDNS_Dereg_normal);
				}

			// AdvertiseHostname will only register new address records.
			// For records still in the process of deregistering it will ignore them, and let the mStatus_MemFree callback handle them.
			AdvertiseHostname(m, i);
			}

		if (v4Changed || RouterChanged)
			{
			m->ExternalAddress      = zerov4Addr;
			m->retryIntervalGetAddr = NATMAP_INIT_RETRY;
			m->retryGetAddr         = m->timenow;
			m->NextScheduledNATOp   = m->timenow;
			m->LastNATMapResultCode = NATErr_None;
#ifdef _LEGACY_NAT_TRAVERSAL_
			LNT_ClearState(m);
#endif // _LEGACY_NAT_TRAVERSAL_
			}

		if (m->ReverseMap.ThisQInterval != -1) mDNS_StopQuery_internal(m, &m->ReverseMap);
		m->StaticHostname.c[0] = 0;
		
		UpdateSRVRecords(m); // Will call GetStaticHostname if needed
		
#if APPLE_OSX_mDNSResponder
		if (RouterChanged)	uuid_generate(m->asl_uuid);
		UpdateAutoTunnelDomainStatuses(m);
#endif
		}

	mDNS_Unlock(m);
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Incoming Message Processing
#endif

mDNSlocal mStatus ParseTSIGError(mDNS *const m, const DNSMessage *const msg, const mDNSu8 *const end, const domainname *const displayname)
	{
	const mDNSu8 *ptr;
	mStatus err = mStatus_NoError;
	int i;

	ptr = LocateAdditionals(msg, end);
	if (!ptr) goto finish;

	for (i = 0; i < msg->h.numAdditionals; i++)
		{
		ptr = GetLargeResourceRecord(m, msg, ptr, end, 0, kDNSRecordTypePacketAdd, &m->rec);
		if (!ptr) goto finish;
		if (m->rec.r.resrec.rrtype == kDNSType_TSIG)
			{
			mDNSu32 macsize;
			mDNSu8 *rd = m->rec.r.resrec.rdata->u.data;
			mDNSu8 *rdend = rd + m->rec.r.resrec.rdlength;
			int alglen = DomainNameLengthLimit(&m->rec.r.resrec.rdata->u.name, rdend);
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

			if      (err == TSIG_ErrBadSig)  { LogMsg("%##s: bad signature", displayname->c);              err = mStatus_BadSig;     }
			else if (err == TSIG_ErrBadKey)  { LogMsg("%##s: bad key", displayname->c);                    err = mStatus_BadKey;     }
			else if (err == TSIG_ErrBadTime) { LogMsg("%##s: bad time", displayname->c);                   err = mStatus_BadTime;    }
			else if (err)                    { LogMsg("%##s: unknown tsig error %d", displayname->c, err); err = mStatus_UnknownErr; }
			goto finish;
			}
		m->rec.r.resrec.RecordType = 0;		// Clear RecordType to show we're not still using it
		}

	finish:
	m->rec.r.resrec.RecordType = 0;		// Clear RecordType to show we're not still using it
	return err;
	}

mDNSlocal mStatus checkUpdateResult(mDNS *const m, const domainname *const displayname, const mDNSu8 rcode, const DNSMessage *const msg, const mDNSu8 *const end)
	{
	(void)msg;	// currently unused, needed for TSIG errors
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
		// TSIG errors should come with FormErr as per RFC 2845, but BIND 9 sends them with NotAuth so we look here too
		mStatus tsigerr = ParseTSIGError(m, msg, end, displayname);
		if (!tsigerr)
			{
			LogMsg("Permission denied (NOAUTH): %##s", displayname->c);
			return mStatus_UnknownErr;
			}
		else return tsigerr;
		}
	else if (rcode == kDNSFlag1_RC_FormErr)
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

// Called with lock held
mDNSlocal void SendRecordRegistration(mDNS *const m, AuthRecord *rr)
	{
	mDNSu8 *ptr = m->omsg.data;
	mDNSu8 *end = (mDNSu8 *)&m->omsg + sizeof(DNSMessage);
	mStatus err = mStatus_UnknownErr;

	if (m->mDNS_busy != m->mDNS_reentrancy+1)
		LogMsg("SendRecordRegistration: Lock not held! mDNS_busy (%ld) mDNS_reentrancy (%ld)", m->mDNS_busy, m->mDNS_reentrancy);

	if (mDNSIPv4AddressIsZero(rr->UpdateServer.ip.v4))	// Don't know our UpdateServer yet
		{
		rr->LastAPTime = m->timenow;
		if (rr->ThisAPInterval < mDNSPlatformOneSecond * 5)
			rr->ThisAPInterval = mDNSPlatformOneSecond * 5;
		return;
		}

	rr->RequireGoodbye = mDNStrue;
	rr->updateid = mDNS_NewMessageID(m);
	InitializeDNSMessage(&m->omsg.h, rr->updateid, UpdateReqFlags);

	// set zone
	ptr = putZone(&m->omsg, ptr, end, rr->zone, mDNSOpaque16fromIntVal(rr->resrec.rrclass));
	if (!ptr) { err = mStatus_UnknownErr; goto exit; }

	if (rr->state == regState_UpdatePending)
		{
		// delete old RData
		SetNewRData(&rr->resrec, rr->OrigRData, rr->OrigRDLen);
		if (!(ptr = putDeletionRecord(&m->omsg, ptr, &rr->resrec))) { err = mStatus_UnknownErr; goto exit; } // delete old rdata

		// add new RData
		SetNewRData(&rr->resrec, rr->InFlightRData, rr->InFlightRDLen);
		if (!(ptr = PutResourceRecordTTLJumbo(&m->omsg, ptr, &m->omsg.h.mDNS_numUpdates, &rr->resrec, rr->resrec.rroriginalttl))) { err = mStatus_UnknownErr; goto exit; }
		}

	else
		{
		if (rr->resrec.RecordType == kDNSRecordTypeKnownUnique)
			{
			// KnownUnique: Delete any previous value
			ptr = putDeleteRRSet(&m->omsg, ptr, rr->resrec.name, rr->resrec.rrtype);
			if (!ptr) { err = mStatus_UnknownErr; goto exit; }
			}

		else if (rr->resrec.RecordType != kDNSRecordTypeShared)
			{
			// For now don't do this, until we have the logic for intelligent grouping of individual recors into logical service record sets
			//ptr = putPrereqNameNotInUse(rr->resrec.name, &m->omsg, ptr, end);
			if (!ptr) { err = mStatus_UnknownErr; goto exit; }
			}

		ptr = PutResourceRecordTTLJumbo(&m->omsg, ptr, &m->omsg.h.mDNS_numUpdates, &rr->resrec, rr->resrec.rroriginalttl);
		if (!ptr) { err = mStatus_UnknownErr; goto exit; }
		}

	if (rr->uselease)
		{
		ptr = putUpdateLease(&m->omsg, ptr, DEFAULT_UPDATE_LEASE); if (!ptr) { err = mStatus_UnknownErr; goto exit; }
		}

	if (rr->Private)
		{
		LogInfo("SendRecordRegistration TCP %p %s", rr->tcp, ARDisplayString(m, rr));
		if (rr->tcp) LogInfo("SendRecordRegistration: Disposing existing TCP connection for %s", ARDisplayString(m, rr));
		if (rr->tcp) DisposeTCPConn(rr->tcp);
		rr->tcp = MakeTCPConn(m, &m->omsg, ptr, kTCPSocketFlags_UseTLS, &rr->UpdateServer, rr->UpdatePort, mDNSNULL, mDNSNULL, rr);
		if (!rr->tcp) rr->ThisAPInterval = mDNSPlatformOneSecond * 5; // If failed to make TCP connection, try again in ten seconds (5*2)
		else if (rr->ThisAPInterval < mDNSPlatformOneSecond * 30) rr->ThisAPInterval = mDNSPlatformOneSecond * 30;
		}
	else
		{
		err = mDNSSendDNSMessage(m, &m->omsg, ptr, mDNSInterface_Any, mDNSNULL, &rr->UpdateServer, rr->UpdatePort, mDNSNULL, GetAuthInfoForName_internal(m, rr->resrec.name));
		if (err) debugf("ERROR: SendRecordRegistration - mDNSSendDNSMessage - %d", err);
		}

	SetRecordRetry(m, rr, err);

	if (rr->state != regState_Refresh && rr->state != regState_DeregDeferred && rr->state != regState_UpdatePending)
		rr->state = regState_Pending;

	return;

exit:
	LogMsg("SendRecordRegistration: Error formatting message for %s", ARDisplayString(m, rr));
	}

// Called with lock held
mDNSlocal void hndlServiceUpdateReply(mDNS *const m, ServiceRecordSet *srs,  mStatus err)
	{
	mDNSBool InvokeCallback = mDNSfalse;
	ExtraResourceRecord **e = &srs->Extras;
	AuthRecord *txt = &srs->RR_TXT;

	if (m->mDNS_busy != m->mDNS_reentrancy+1)
		LogMsg("hndlServiceUpdateReply: Lock not held! mDNS_busy (%ld) mDNS_reentrancy (%ld)", m->mDNS_busy, m->mDNS_reentrancy);

	debugf("hndlServiceUpdateReply: err %d state %d %##s", err, srs->state, srs->RR_SRV.resrec.name->c);

	SetRecordRetry(m, &srs->RR_SRV, mStatus_NoError);

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
				if (err == mStatus_NoSuchRecord) err = mStatus_NameConflict;	// NoSuchRecord implies that our prereq was not met, so we actually have a name conflict
				if (!err) srs->state = regState_Registered;
				InvokeCallback = mDNStrue;
				break;
				}
			else if (srs->srs_uselease && err == mStatus_UnknownErr && mDNSSameIPPort(srs->SRSUpdatePort, UnicastDNSPort))
				{
				LogMsg("Re-trying update of service %##s without lease option", srs->RR_SRV.resrec.name->c);
				srs->srs_uselease = mDNSfalse;
				SendServiceRegistration(m, srs);
				return;
				}
			else
				{
				//!!!KRS make sure all structs will still get cleaned up when client calls DeregisterService with this state
				if (err) LogMsg("Error %d for registration of service %##s", err, srs->RR_SRV.resrec.name->c);
				else srs->state = regState_Registered;
				InvokeCallback = mDNStrue;
				break;
				}
		case regState_Refresh:
			if (err)
				{
				LogMsg("Error %d for refresh of service %##s", err, srs->RR_SRV.resrec.name->c);
				InvokeCallback = mDNStrue;
				}
			else srs->state = regState_Registered;
			break;
		case regState_DeregPending:
			if (err) LogMsg("Error %d for deregistration of service %##s", err, srs->RR_SRV.resrec.name->c);
			if (srs->SRVChanged)
				{
				srs->state = regState_NoTarget;	// NoTarget will allow us to pick up new target OR nat traversal state
				break;
				}
			err = mStatus_MemFree;
			InvokeCallback = mDNStrue;
			if (srs->NATinfo.clientContext)
				{
				// deletion completed
				mDNS_StopNATOperation_internal(m, &srs->NATinfo);
				srs->NATinfo.clientContext = mDNSNULL;
				}
			srs->state = regState_Unregistered;
			break;
		case regState_DeregDeferred:
			if (err)
				{
				debugf("Error %d received prior to deferred deregistration of %##s", err, srs->RR_SRV.resrec.name->c);
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
		case regState_NoTarget:
			// This state is used when using SendServiceDeregistration() when going to sleep -- no further action required
			return;
		case regState_FetchingZoneData:
		case regState_Registered:
		case regState_Unregistered:
		case regState_NATMap:
		case regState_ExtraQueued:
		case regState_NATError:
			LogMsg("hndlServiceUpdateReply called for service %##s in unexpected state %d with error %d.  Unlinking.",
				   srs->RR_SRV.resrec.name->c, srs->state, err);
			err = mStatus_UnknownErr;
		default: LogMsg("hndlServiceUpdateReply: Unknown state %d for %##s", srs->state, srs->RR_SRV.resrec.name->c);
		}

	if ((srs->SRVChanged || srs->SRVUpdateDeferred) && (srs->state == regState_NoTarget || srs->state == regState_Registered))
		{
		debugf("hndlServiceUpdateReply: SRVChanged %d SRVUpdateDeferred %d state %d", srs->SRVChanged, srs->SRVUpdateDeferred, srs->state);
		if (InvokeCallback)
			{
			srs->ClientCallbackDeferred = mDNStrue;
			srs->DeferredStatus = err;
			}
		srs->SRVChanged = srs->SRVUpdateDeferred = mDNSfalse;
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
				(*e)->r.zone = &srs->zone;
				(*e)->r.UpdateServer    = srs->SRSUpdateServer;
				(*e)->r.UpdatePort  = srs->SRSUpdatePort;
				(*e)->r.uselease = srs->srs_uselease;
				SendRecordRegistration(m, &(*e)->r);
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

	if (srs->state == regState_Unregistered)
		{
		if (err != mStatus_MemFree)
			LogMsg("hndlServiceUpdateReply ERROR! state == regState_Unregistered but err != mStatus_MemFree. Permanently abandoning service registration %##s",
				srs->RR_SRV.resrec.name->c);
		unlinkSRS(m, srs);
		}
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
	if (InvokeCallback)
		srs->ServiceCallback(m, srs, err);
	else if (srs->ClientCallbackDeferred)
		{
		srs->ClientCallbackDeferred = mDNSfalse;
		srs->ServiceCallback(m, srs, srs->DeferredStatus);
		}
	mDNS_ReclaimLockAfterCallback();
	// CAUTION: MUST NOT do anything more with rr after calling rr->Callback(), because the client's callback function
	// is allowed to do anything, including starting/stopping queries, registering/deregistering records, etc.
	}

// Called with lock held
mDNSlocal void hndlRecordUpdateReply(mDNS *m, AuthRecord *rr, mStatus err)
	{
	mDNSBool InvokeCallback = mDNStrue;

	if (m->mDNS_busy != m->mDNS_reentrancy+1)
		LogMsg("hndlRecordUpdateReply: Lock not held! mDNS_busy (%ld) mDNS_reentrancy (%ld)", m->mDNS_busy, m->mDNS_reentrancy);

	LogInfo("hndlRecordUpdateReply: err %d state %d %s", err, rr->state, ARDisplayString(m, rr));

	if (m->SleepState) return;		// If we just sent a deregister on going to sleep, no further action required

	SetRecordRetry(m, rr, mStatus_NoError);

	if (rr->state == regState_UpdatePending)
		{
		if (err) LogMsg("Update record failed for %##s (err %d)", rr->resrec.name->c, err);
		rr->state = regState_Registered;
		// deallocate old RData
		if (rr->UpdateCallback) rr->UpdateCallback(m, rr, rr->OrigRData);
		SetNewRData(&rr->resrec, rr->InFlightRData, rr->InFlightRDLen);
		rr->OrigRData = mDNSNULL;
		rr->InFlightRData = mDNSNULL;
		}

	if (rr->state == regState_DeregPending)
		{
		debugf("Received reply for deregister record %##s type %d", rr->resrec.name->c, rr->resrec.rrtype);
		if (err) LogMsg("ERROR: Deregistration of record %##s type %d failed with error %d",
						rr->resrec.name->c, rr->resrec.rrtype, err);
		err = mStatus_MemFree;
		rr->state = regState_Unregistered;
		}

	if (rr->state == regState_DeregDeferred)
		{
		if (err)
			{
			LogMsg("Cancelling deferred deregistration record %##s type %d due to registration error %d",
				   rr->resrec.name->c, rr->resrec.rrtype, err);
			rr->state = regState_Unregistered;
			}
		debugf("Calling deferred deregistration of record %##s type %d",  rr->resrec.name->c, rr->resrec.rrtype);
		rr->state = regState_Registered;
		mDNS_Deregister_internal(m, rr, mDNS_Dereg_normal);
		return;
		}

	if (rr->state == regState_Pending || rr->state == regState_Refresh)
		{
		if (!err)
			{
			if (rr->state == regState_Refresh) InvokeCallback = mDNSfalse;
			rr->state = regState_Registered;
			}
		else
			{
			if (rr->uselease && err == mStatus_UnknownErr && mDNSSameIPPort(rr->UpdatePort, UnicastDNSPort))
				{
				LogMsg("Re-trying update of record %##s without lease option", rr->resrec.name->c);
				rr->uselease = mDNSfalse;
				SendRecordRegistration(m, rr);
				return;
				}
			LogMsg("hndlRecordUpdateReply: Registration of record %##s type %d failed with error %d", rr->resrec.name->c, rr->resrec.rrtype, err);
			return;
			}
		}

	if (rr->state == regState_Unregistered)		// Should never happen
		{
		LogMsg("hndlRecordUpdateReply rr->state == regState_Unregistered %s", ARDisplayString(m, rr));
		return;
		}

	if (rr->QueuedRData && rr->state == regState_Registered)
		{
		rr->state = regState_UpdatePending;
		rr->InFlightRData = rr->QueuedRData;
		rr->InFlightRDLen = rr->QueuedRDLen;
		rr->OrigRData = rr->resrec.rdata;
		rr->OrigRDLen = rr->resrec.rdlength;
		rr->QueuedRData = mDNSNULL;
		SendRecordRegistration(m, rr);
		return;
		}

	if (InvokeCallback && rr->RecordCallback)
		{
		mDNS_DropLockBeforeCallback();
		rr->RecordCallback(m, rr, err);
		mDNS_ReclaimLockAfterCallback();
		}
	// CAUTION: MUST NOT do anything more with rr after calling rr->Callback(), because the client's callback function
	// is allowed to do anything, including starting/stopping queries, registering/deregistering records, etc.
	}

mDNSexport void uDNS_ReceiveNATPMPPacket(mDNS *m, const mDNSInterfaceID InterfaceID, mDNSu8 *pkt, mDNSu16 len)
	{
	NATTraversalInfo *ptr;
	NATAddrReply     *AddrReply    = (NATAddrReply    *)pkt;
	NATPortMapReply  *PortMapReply = (NATPortMapReply *)pkt;
	mDNSu32 nat_elapsed, our_elapsed;

	// Minimum packet is vers (1) opcode (1) err (2) upseconds (4) = 8 bytes
	if (!AddrReply->err && len < 8) { LogMsg("NAT Traversal message too short (%d bytes)", len); return; }
	if (AddrReply->vers != NATMAP_VERS) { LogMsg("Received NAT Traversal response with version %d (expected %d)", pkt[0], NATMAP_VERS); return; }

	// Read multi-byte numeric values (fields are identical in a NATPortMapReply)
	AddrReply->err       = (mDNSu16) (                                                (mDNSu16)pkt[2] << 8 | pkt[3]);
	AddrReply->upseconds = (mDNSs32) ((mDNSs32)pkt[4] << 24 | (mDNSs32)pkt[5] << 16 | (mDNSs32)pkt[6] << 8 | pkt[7]);

	nat_elapsed = AddrReply->upseconds - m->LastNATupseconds;
	our_elapsed = (m->timenow - m->LastNATReplyLocalTime) / mDNSPlatformOneSecond;
	debugf("uDNS_ReceiveNATPMPPacket %X upseconds %u nat_elapsed %d our_elapsed %d", AddrReply->opcode, AddrReply->upseconds, nat_elapsed, our_elapsed);

	// We compute a conservative estimate of how much the NAT gateways's clock should have advanced
	// 1. We subtract 12.5% from our own measured elapsed time, to allow for NAT gateways that have an inacurate clock that runs slowly
	// 2. We add a two-second safety margin to allow for rounding errors:
	//    -- e.g. if NAT gateway sends a packet at t=2.00 seconds, then one at t=7.99, that's virtually 6 seconds,
	//       but based on the values in the packet (2,7) the apparent difference is only 5 seconds
	//    -- similarly, if we're slow handling packets and/or we have coarse clock granularity, we could over-estimate the true interval
	//       (e.g. t=1.99 seconds rounded to 1, and t=8.01 rounded to 8, gives an apparent difference of 7 seconds)
	if (AddrReply->upseconds < m->LastNATupseconds || nat_elapsed + 2 < our_elapsed - our_elapsed/8)
		{ LogMsg("NAT gateway %#a rebooted", &m->Router); RecreateNATMappings(m); }

	m->LastNATupseconds      = AddrReply->upseconds;
	m->LastNATReplyLocalTime = m->timenow;
#ifdef _LEGACY_NAT_TRAVERSAL_
	LNT_ClearState(m);
#endif // _LEGACY_NAT_TRAVERSAL_

	if (AddrReply->opcode == NATOp_AddrResponse)
		{
#if APPLE_OSX_mDNSResponder
		static char msgbuf[16];
		mDNS_snprintf(msgbuf, sizeof(msgbuf), "%d", AddrReply->err);
		mDNSASLLog((uuid_t *)&m->asl_uuid, "natt.natpmp.AddressRequest", AddrReply->err ? "failure" : "success", msgbuf, "");
#endif
		if (!AddrReply->err && len < sizeof(NATAddrReply)) { LogMsg("NAT Traversal AddrResponse message too short (%d bytes)", len); return; }
		natTraversalHandleAddressReply(m, AddrReply->err, AddrReply->ExtAddr);
		}
	else if (AddrReply->opcode == NATOp_MapUDPResponse || AddrReply->opcode == NATOp_MapTCPResponse)
		{
		mDNSu8 Protocol = AddrReply->opcode & 0x7F;
#if APPLE_OSX_mDNSResponder
		static char msgbuf[16];
		mDNS_snprintf(msgbuf, sizeof(msgbuf), "%s - %d", AddrReply->opcode == NATOp_MapUDPResponse ? "UDP" : "TCP", PortMapReply->err);
		mDNSASLLog((uuid_t *)&m->asl_uuid, "natt.natpmp.PortMapRequest", PortMapReply->err ? "failure" : "success", msgbuf, "");
#endif
		if (!PortMapReply->err)
			{
			if (len < sizeof(NATPortMapReply)) { LogMsg("NAT Traversal PortMapReply message too short (%d bytes)", len); return; }
			PortMapReply->NATRep_lease = (mDNSu32) ((mDNSu32)pkt[12] << 24 | (mDNSu32)pkt[13] << 16 | (mDNSu32)pkt[14] << 8 | pkt[15]);
			}

		// Since some NAT-PMP server implementations don't return the requested internal port in
		// the reply, we can't associate this reply with a particular NATTraversalInfo structure.
		// We globally keep track of the most recent error code for mappings.
		m->LastNATMapResultCode = PortMapReply->err;
		
		for (ptr = m->NATTraversals; ptr; ptr=ptr->next)
			if (ptr->Protocol == Protocol && mDNSSameIPPort(ptr->IntPort, PortMapReply->intport))
				natTraversalHandlePortMapReply(m, ptr, InterfaceID, PortMapReply->err, PortMapReply->extport, PortMapReply->NATRep_lease);
		}
	else { LogMsg("Received NAT Traversal response with version unknown opcode 0x%X", AddrReply->opcode); return; }

	// Don't need an SSDP socket if we get a NAT-PMP packet
	if (m->SSDPSocket) { debugf("uDNS_ReceiveNATPMPPacket destroying SSDPSocket %p", &m->SSDPSocket); mDNSPlatformUDPClose(m->SSDPSocket); m->SSDPSocket = mDNSNULL; }
	}

// <rdar://problem/3925163> Shorten DNS-SD queries to avoid NAT bugs
// <rdar://problem/4288449> Add check to avoid crashing NAT gateways that have buggy DNS relay code
//
// We know of bugs in home NAT gateways that cause them to crash if they receive certain DNS queries.
// The DNS queries that make them crash are perfectly legal DNS queries, but even if they weren't,
// the gateway shouldn't crash -- in today's world of viruses and network attacks, software has to
// be written assuming that a malicious attacker could send them any packet, properly-formed or not.
// Still, we don't want to be crashing people's home gateways, so we go out of our way to avoid
// the queries that crash them.
//
// Some examples:
//
// 1. Any query where the name ends in ".in-addr.arpa." and the text before this is 32 or more bytes.
//    The query type does not need to be PTR -- the gateway will crash for any query type.
//    e.g. "ping long-name-crashes-the-buggy-router.in-addr.arpa" will crash one of these.
//
// 2. Any query that results in a large response with the TC bit set.
//
// 3. Any PTR query that doesn't begin with four decimal numbers.
//    These gateways appear to assume that the only possible PTR query is a reverse-mapping query
//    (e.g. "1.0.168.192.in-addr.arpa") and if they ever get a PTR query where the first four
//    labels are not all decimal numbers in the range 0-255, they handle that by crashing.
//    These gateways also ignore the remainder of the name following the four decimal numbers
//    -- whether or not it actually says in-addr.arpa, they just make up an answer anyway.
//
// The challenge therefore is to craft a query that will discern whether the DNS server
// is one of these buggy ones, without crashing it. Furthermore we don't want our test
// queries making it all the way to the root name servers, putting extra load on those
// name servers and giving Apple a bad reputation. To this end we send this query:
//     dig -t ptr 1.0.0.127.dnsbugtest.1.0.0.127.in-addr.arpa.
//
// The text preceding the ".in-addr.arpa." is under 32 bytes, so it won't cause crash (1).
// It will not yield a large response with the TC bit set, so it won't cause crash (2).
// It starts with four decimal numbers, so it won't cause crash (3).
// The name falls within the "1.0.0.127.in-addr.arpa." domain, the reverse-mapping name for the local
// loopback address, and therefore the query will black-hole at the first properly-configured DNS server
// it reaches, making it highly unlikely that this query will make it all the way to the root.
//
// Finally, the correct response to this query is NXDOMAIN or a similar error, but the
// gateways that ignore the remainder of the name following the four decimal numbers
// give themselves away by actually returning a result for this nonsense query.

mDNSlocal const domainname *DNSRelayTestQuestion = (const domainname*)
	"\x1" "1" "\x1" "0" "\x1" "0" "\x3" "127" "\xa" "dnsbugtest"
	"\x1" "1" "\x1" "0" "\x1" "0" "\x3" "127" "\x7" "in-addr" "\x4" "arpa";

// See comments above for DNSRelayTestQuestion
// If this is the kind of query that has the risk of crashing buggy DNS servers, we do a test question first
mDNSlocal mDNSBool NoTestQuery(DNSQuestion *q)
	{
	int i;
	mDNSu8 *p = q->qname.c;
	if (q->AuthInfo) return(mDNStrue);		// Don't need a test query for private queries sent directly to authoritative server over TLS/TCP
	if (q->qtype != kDNSType_PTR) return(mDNStrue);		// Don't need a test query for any non-PTR queries
	for (i=0; i<4; i++)		// If qname does not begin with num.num.num.num, can't skip the test query
		{
		if (p[0] < 1 || p[0] > 3) return(mDNSfalse);
		if (              p[1] < '0' || p[1] > '9' ) return(mDNSfalse);
		if (p[0] >= 2 && (p[2] < '0' || p[2] > '9')) return(mDNSfalse);
		if (p[0] >= 3 && (p[3] < '0' || p[3] > '9')) return(mDNSfalse);
		p += 1 + p[0];
		}
	// If remainder of qname is ".in-addr.arpa.", this is a vanilla reverse-mapping query and
	// we can safely do it without needing a test query first, otherwise we need the test query.
	return(SameDomainName((domainname*)p, (const domainname*)"\x7" "in-addr" "\x4" "arpa"));
	}

// Returns mDNStrue if response was handled
mDNSlocal mDNSBool uDNS_ReceiveTestQuestionResponse(mDNS *const m, DNSMessage *const msg, const mDNSu8 *const end,
	const mDNSAddr *const srcaddr, const mDNSIPPort srcport)
	{
	const mDNSu8 *ptr = msg->data;
	DNSQuestion pktq;
	DNSServer *s;
	mDNSu32 result = 0;

	// 1. Find out if this is an answer to one of our test questions
	if (msg->h.numQuestions != 1) return(mDNSfalse);
	ptr = getQuestion(msg, ptr, end, mDNSInterface_Any, &pktq);
	if (!ptr) return(mDNSfalse);
	if (pktq.qtype != kDNSType_PTR || pktq.qclass != kDNSClass_IN) return(mDNSfalse);
	if (!SameDomainName(&pktq.qname, DNSRelayTestQuestion)) return(mDNSfalse);

	// 2. If the DNS relay gave us a positive response, then it's got buggy firmware
	// else, if the DNS relay gave us an error or no-answer response, it passed our test
	if ((msg->h.flags.b[1] & kDNSFlag1_RC_Mask) == kDNSFlag1_RC_NoErr && msg->h.numAnswers > 0)
		result = DNSServer_Failed;
	else
		result = DNSServer_Passed;

	// 3. Find occurrences of this server in our list, and mark them appropriately
	for (s = m->DNSServers; s; s = s->next)
		{
		mDNSBool matchaddr = (s->teststate != result && mDNSSameAddress(srcaddr, &s->addr) && mDNSSameIPPort(srcport, s->port));
		mDNSBool matchid   = (s->teststate == DNSServer_Untested && mDNSSameOpaque16(msg->h.id, s->testid));
		if (matchaddr || matchid)
			{
			DNSQuestion *q;
			s->teststate = result;
			if (result == DNSServer_Passed)
				{
				LogInfo("DNS Server %#a:%d (%#a:%d) %d passed%s",
					&s->addr, mDNSVal16(s->port), srcaddr, mDNSVal16(srcport), mDNSVal16(s->testid),
					matchaddr ? "" : " NOTE: Reply did not come from address to which query was sent");
				}
			else
				{
				LogMsg("NOTE: Wide-Area Service Discovery disabled to avoid crashing defective DNS relay %#a:%d (%#a:%d) %d%s",
					&s->addr, mDNSVal16(s->port), srcaddr, mDNSVal16(srcport), mDNSVal16(s->testid),
					matchaddr ? "" : " NOTE: Reply did not come from address to which query was sent");
				}

			// If this server has just changed state from DNSServer_Untested to DNSServer_Passed, then retrigger any waiting questions.
			// We use the NoTestQuery() test so that we only retrigger questions that were actually blocked waiting for this test to complete.
			if (result == DNSServer_Passed)		// Unblock any questions that were waiting for this result
				for (q = m->Questions; q; q=q->next)
					if (q->qDNSServer == s && !NoTestQuery(q))
						{
						q->ThisQInterval = INIT_UCAST_POLL_INTERVAL / QuestionIntervalStep;
						q->unansweredQueries = 0;
						q->LastQTime = m->timenow - q->ThisQInterval;
						m->NextScheduledQuery = m->timenow;
						}
			}
		}

	return(mDNStrue); // Return mDNStrue to tell uDNS_ReceiveMsg it doesn't need to process this packet further
	}

// Called from mDNSCoreReceive with the lock held
mDNSexport void uDNS_ReceiveMsg(mDNS *const m, DNSMessage *const msg, const mDNSu8 *const end, const mDNSAddr *const srcaddr, const mDNSIPPort srcport)
	{
	DNSQuestion *qptr;
	mStatus err = mStatus_NoError;

	mDNSu8 StdR    = kDNSFlag0_QR_Response | kDNSFlag0_OP_StdQuery;
	mDNSu8 UpdateR = kDNSFlag0_QR_Response | kDNSFlag0_OP_Update;
	mDNSu8 QR_OP   = (mDNSu8)(msg->h.flags.b[0] & kDNSFlag0_QROP_Mask);
	mDNSu8 rcode   = (mDNSu8)(msg->h.flags.b[1] & kDNSFlag1_RC_Mask);

	(void)srcport; // Unused

	debugf("uDNS_ReceiveMsg from %#-15a with "
		"%2d Question%s %2d Answer%s %2d Authorit%s %2d Additional%s %d bytes",
		srcaddr,
		msg->h.numQuestions,   msg->h.numQuestions   == 1 ? ", "   : "s,",
		msg->h.numAnswers,     msg->h.numAnswers     == 1 ? ", "   : "s,",
		msg->h.numAuthorities, msg->h.numAuthorities == 1 ? "y,  " : "ies,",
		msg->h.numAdditionals, msg->h.numAdditionals == 1 ? ""     : "s", end - msg->data);

	if (QR_OP == StdR)
		{
		//if (srcaddr && recvLLQResponse(m, msg, end, srcaddr, srcport)) return;
		if (uDNS_ReceiveTestQuestionResponse(m, msg, end, srcaddr, srcport)) return;
		for (qptr = m->Questions; qptr; qptr = qptr->next)
			if (msg->h.flags.b[0] & kDNSFlag0_TC && mDNSSameOpaque16(qptr->TargetQID, msg->h.id) && m->timenow - qptr->LastQTime < RESPONSE_WINDOW)
				{
				if (!srcaddr) LogMsg("uDNS_ReceiveMsg: TCP DNS response had TC bit set: ignoring");
				else if (qptr->tcp)
					{
					// There may be a race condition here, if the server decides to drop the connection just as we decide to reuse it
					// For now it should not be serious because our normal retry logic (as used to handle UDP packet loss)
					// should take care of it but later we may want to look at handling this case explicitly
					LogInfo("uDNS_ReceiveMsg: Using existing TCP connection for %##s (%s)", qptr->qname.c, DNSTypeName(qptr->qtype));
					mDNS_DropLockBeforeCallback();
					tcpCallback(qptr->tcp->sock, qptr->tcp, mDNStrue, mStatus_NoError);
					mDNS_ReclaimLockAfterCallback();
					}
				else qptr->tcp = MakeTCPConn(m, mDNSNULL, mDNSNULL, kTCPSocketFlags_Zero, srcaddr, srcport, qptr, mDNSNULL, mDNSNULL);
				}
		}

	if (QR_OP == UpdateR)
		{
		mDNSu32 lease = GetPktLease(m, msg, end);
		mDNSs32 expire = m->timenow + (mDNSs32)lease * mDNSPlatformOneSecond;

		//rcode = kDNSFlag1_RC_ServFail;	// Simulate server failure (rcode 2)

		if (CurrentServiceRecordSet)
			LogMsg("uDNS_ReceiveMsg ERROR CurrentServiceRecordSet already set");
		CurrentServiceRecordSet = m->ServiceRegistrations;

		while (CurrentServiceRecordSet)
			{
			ServiceRecordSet *sptr = CurrentServiceRecordSet;
			CurrentServiceRecordSet = CurrentServiceRecordSet->uDNS_next;

			if (mDNSSameOpaque16(sptr->id, msg->h.id))
				{
				err = checkUpdateResult(m, sptr->RR_SRV.resrec.name, rcode, msg, end);
				if (!err && sptr->srs_uselease && lease)
					if (sptr->RR_SRV.expire - expire >= 0 || sptr->state != regState_UpdatePending)
						sptr->RR_SRV.expire = expire;
				hndlServiceUpdateReply(m, sptr, err);
				CurrentServiceRecordSet = mDNSNULL;
				return;
				}
			}

		if (m->CurrentRecord)
			LogMsg("uDNS_ReceiveMsg ERROR m->CurrentRecord already set %s", ARDisplayString(m, m->CurrentRecord));
		m->CurrentRecord = m->ResourceRecords;
		while (m->CurrentRecord)
			{
			AuthRecord *rptr = m->CurrentRecord;
			m->CurrentRecord = m->CurrentRecord->next;
			if (AuthRecord_uDNS(rptr) && mDNSSameOpaque16(rptr->updateid, msg->h.id))
				{
				err = checkUpdateResult(m, rptr->resrec.name, rcode, msg, end);
				if (!err && rptr->uselease && lease)
					if (rptr->expire - expire >= 0 || rptr->state != regState_UpdatePending)
						rptr->expire = expire;
				hndlRecordUpdateReply(m, rptr, err);
				m->CurrentRecord = mDNSNULL;
				return;
				}
			}
		}
	debugf("Received unexpected response: ID %d matches no active records", mDNSVal16(msg->h.id));
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Query Routines
#endif

mDNSexport void sendLLQRefresh(mDNS *m, DNSQuestion *q)
	{
	mDNSu8 *end;
	LLQOptData llq;

	if (q->ReqLease)
		if ((q->state == LLQ_Established && q->ntries >= kLLQ_MAX_TRIES) || q->expire - m->timenow < 0)
			{
			LogMsg("Unable to refresh LLQ %##s (%s) - will retry in %d seconds", q->qname.c, DNSTypeName(q->qtype), LLQ_POLL_INTERVAL / mDNSPlatformOneSecond);
			StartLLQPolling(m,q);
			return;
			}

	llq.vers     = kLLQ_Vers;
	llq.llqOp    = kLLQOp_Refresh;
	llq.err      = q->tcp ? GetLLQEventPort(m, &q->servAddr) : LLQErr_NoError;	// If using TCP tell server what UDP port to send notifications to
	llq.id       = q->id;
	llq.llqlease = q->ReqLease;

	InitializeDNSMessage(&m->omsg.h, q->TargetQID, uQueryFlags);
	end = putLLQ(&m->omsg, m->omsg.data, q, &llq);
	if (!end) { LogMsg("sendLLQRefresh: putLLQ failed %##s (%s)", q->qname.c, DNSTypeName(q->qtype)); return; }

	// Note that we (conditionally) add HINFO and TSIG here, since the question might be going away,
	// so we may not be able to reference it (most importantly it's AuthInfo) when we actually send the message
	end = putHINFO(m, &m->omsg, end, q->AuthInfo);
	if (!end) { LogMsg("sendLLQRefresh: putHINFO failed %##s (%s)", q->qname.c, DNSTypeName(q->qtype)); return; }

	if (q->AuthInfo)
		{
		DNSDigest_SignMessageHostByteOrder(&m->omsg, &end, q->AuthInfo);
		if (!end) { LogMsg("sendLLQRefresh: DNSDigest_SignMessage failed %##s (%s)", q->qname.c, DNSTypeName(q->qtype)); return; }
		}

	if (q->AuthInfo && !q->tcp)
		{
		LogInfo("sendLLQRefresh setting up new TLS session %##s (%s)", q->qname.c, DNSTypeName(q->qtype));
		q->tcp = MakeTCPConn(m, &m->omsg, end, kTCPSocketFlags_UseTLS, &q->servAddr, q->servPort, q, mDNSNULL, mDNSNULL);
		}
	else
		{
		mStatus err = mDNSSendDNSMessage(m, &m->omsg, end, mDNSInterface_Any, q->LocalSocket, &q->servAddr, q->servPort, q->tcp ? q->tcp->sock : mDNSNULL, mDNSNULL);
		if (err)
			{
			LogMsg("sendLLQRefresh: mDNSSendDNSMessage%s failed: %d", q->tcp ? " (TCP)" : "", err);
			if (q->tcp) { DisposeTCPConn(q->tcp); q->tcp = mDNSNULL; }
			}
		}

	q->ntries++;

	debugf("sendLLQRefresh ntries %d %##s (%s)", q->ntries, q->qname.c, DNSTypeName(q->qtype));

	q->LastQTime = m->timenow;
	SetNextQueryTime(m, q);
	}

mDNSexport void LLQGotZoneData(mDNS *const m, mStatus err, const ZoneData *zoneInfo)
	{
	DNSQuestion *q = (DNSQuestion *)zoneInfo->ZoneDataContext;

	mDNS_Lock(m);

	// If we get here it means that the GetZoneData operation has completed, and is is about to cancel
	// its question and free the ZoneData memory. We no longer need to hold onto our pointer (which
	// we use for cleaning up if our LLQ is cancelled *before* the GetZoneData operation has completes).
	q->nta      = mDNSNULL;
	q->servAddr = zeroAddr;
	q->servPort = zeroIPPort;

	if (!err && zoneInfo && !mDNSIPPortIsZero(zoneInfo->Port) && !mDNSAddressIsZero(&zoneInfo->Addr))
		{
		q->servAddr = zoneInfo->Addr;
		q->servPort = zoneInfo->Port;
		q->AuthInfo = zoneInfo->ZonePrivate ? GetAuthInfoForName_internal(m, &q->qname) : mDNSNULL;
		q->ntries = 0;
		debugf("LLQGotZoneData %#a:%d", &q->servAddr, mDNSVal16(q->servPort));
		startLLQHandshake(m, q);
		}
	else
		{
		StartLLQPolling(m,q);
		if (err == mStatus_NoSuchNameErr) 
			{
			// this actually failed, so mark it by setting address to all ones 
			q->servAddr.type = mDNSAddrType_IPv4; 
			q->servAddr.ip.v4 = onesIPv4Addr; 
			}
		}

	mDNS_Unlock(m);
	}

// Called in normal callback context (i.e. mDNS_busy and mDNS_reentrancy are both 1)
mDNSlocal void PrivateQueryGotZoneData(mDNS *const m, mStatus err, const ZoneData *zoneInfo)
	{
	DNSQuestion *q = (DNSQuestion *) zoneInfo->ZoneDataContext;

	LogInfo("PrivateQueryGotZoneData %##s (%s) err %d Zone %##s Private %d", q->qname.c, DNSTypeName(q->qtype), err, zoneInfo->ZoneName.c, zoneInfo->ZonePrivate);

	// If we get here it means that the GetZoneData operation has completed, and is is about to cancel
	// its question and free the ZoneData memory. We no longer need to hold onto our pointer (which
	// we use for cleaning up if our LLQ is cancelled *before* the GetZoneData operation has completes).
	q->nta = mDNSNULL;

	if (err || !zoneInfo || mDNSAddressIsZero(&zoneInfo->Addr) || mDNSIPPortIsZero(zoneInfo->Port))
		{
		LogInfo("ERROR: PrivateQueryGotZoneData %##s (%s) invoked with error code %d %p %#a:%d",
			q->qname.c, DNSTypeName(q->qtype), err, zoneInfo,
			zoneInfo ? &zoneInfo->Addr : mDNSNULL,
			zoneInfo ? mDNSVal16(zoneInfo->Port) : 0);
		return;
		}

	if (!zoneInfo->ZonePrivate)
		{
		debugf("Private port lookup failed -- retrying without TLS -- %##s (%s)", q->qname.c, DNSTypeName(q->qtype));
		q->AuthInfo      = mDNSNULL;		// Clear AuthInfo so we try again non-private
		q->ThisQInterval = InitialQuestionInterval;
		q->LastQTime     = m->timenow - q->ThisQInterval;
		mDNS_Lock(m);
		SetNextQueryTime(m, q);
		mDNS_Unlock(m);
		return;
		// Next call to uDNS_CheckCurrentQuestion() will do this as a non-private query
		}

	if (!q->AuthInfo)
		{
		LogMsg("ERROR: PrivateQueryGotZoneData: cannot find credentials for q %##s (%s)", q->qname.c, DNSTypeName(q->qtype));
		return;
		}

	q->TargetQID = mDNS_NewMessageID(m);
	if (q->tcp) DisposeTCPConn(q->tcp);
	q->tcp = MakeTCPConn(m, mDNSNULL, mDNSNULL, kTCPSocketFlags_UseTLS, &zoneInfo->Addr, zoneInfo->Port, q, mDNSNULL, mDNSNULL);
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Dynamic Updates
#endif

// Called in normal callback context (i.e. mDNS_busy and mDNS_reentrancy are both 1)
mDNSexport void RecordRegistrationGotZoneData(mDNS *const m, mStatus err, const ZoneData *zoneData)
	{
	AuthRecord *newRR = (AuthRecord*)zoneData->ZoneDataContext;
	AuthRecord *ptr;
	int c1, c2;

	if (m->mDNS_busy != m->mDNS_reentrancy)
		LogMsg("RecordRegistrationGotZoneData: mDNS_busy (%ld) != mDNS_reentrancy (%ld)", m->mDNS_busy, m->mDNS_reentrancy);

	newRR->nta = mDNSNULL;

	// Start off assuming we're going to use a lease
	// If we get an error from the server, and the update port as given in the SRV record is 53, then we'll retry without the lease option
	newRR->uselease = mDNStrue;

	// make sure record is still in list (!!!)
	for (ptr = m->ResourceRecords; ptr; ptr = ptr->next) if (ptr == newRR) break;
	if (!ptr) { LogMsg("RecordRegistrationGotZoneData - RR no longer in list.  Discarding."); return; }

	// check error/result
	if (err)
		{
		if (err != mStatus_NoSuchNameErr) LogMsg("RecordRegistrationGotZoneData: error %d", err);
		return;
		}

	if (!zoneData) { LogMsg("ERROR: RecordRegistrationGotZoneData invoked with NULL result and no error"); return; }

	if (newRR->resrec.rrclass != zoneData->ZoneClass)
		{
		LogMsg("ERROR: New resource record's class (%d) does not match zone class (%d)", newRR->resrec.rrclass, zoneData->ZoneClass);
		return;
		}

	// Don't try to do updates to the root name server.
	// We might be tempted also to block updates to any single-label name server (e.g. com, edu, net, etc.) but some
	// organizations use their own private pseudo-TLD, like ".home", etc, and we don't want to block that.
	if (zoneData->ZoneName.c[0] == 0)
		{
		LogInfo("RecordRegistrationGotZoneData: No name server found claiming responsibility for \"%##s\"!", newRR->resrec.name->c);
		return;
		}

	// Store discovered zone data
	c1 = CountLabels(newRR->resrec.name);
	c2 = CountLabels(&zoneData->ZoneName);
	if (c2 > c1)
		{
		LogMsg("RecordRegistrationGotZoneData: Zone \"%##s\" is longer than \"%##s\"", zoneData->ZoneName.c, newRR->resrec.name->c);
		return;
		}
	newRR->zone = SkipLeadingLabels(newRR->resrec.name, c1-c2);
	if (!SameDomainName(newRR->zone, &zoneData->ZoneName))
		{
		LogMsg("RecordRegistrationGotZoneData: Zone \"%##s\" does not match \"%##s\" for \"%##s\"", newRR->zone->c, zoneData->ZoneName.c, newRR->resrec.name->c);
		return;
		}
	newRR->UpdateServer = zoneData->Addr;
	newRR->UpdatePort   = zoneData->Port;
	newRR->Private      = zoneData->ZonePrivate;
	debugf("RecordRegistrationGotZoneData: Set newRR->UpdateServer %##s %##s to %#a:%d",
		newRR->resrec.name->c, zoneData->ZoneName.c, &newRR->UpdateServer, mDNSVal16(newRR->UpdatePort));

	if (mDNSIPPortIsZero(zoneData->Port) || mDNSAddressIsZero(&zoneData->Addr))
		{
		LogInfo("RecordRegistrationGotZoneData: No _dns-update._udp service found for \"%##s\"!", newRR->resrec.name->c);
		return;
		}

	newRR->ThisAPInterval = 5 * mDNSPlatformOneSecond;		// After doubling, first retry will happen after ten seconds

	mDNS_Lock(m);	// SendRecordRegistration expects to be called with the lock held
	SendRecordRegistration(m, newRR);
	mDNS_Unlock(m);
	}

mDNSlocal void SendRecordDeregistration(mDNS *m, AuthRecord *rr)
	{
	mDNSu8 *ptr = m->omsg.data;
	mDNSu8 *end = (mDNSu8 *)&m->omsg + sizeof(DNSMessage);

	if (mDNSIPv4AddressIsZero(rr->UpdateServer.ip.v4))	// Don't know our UpdateServer yet
		{
		rr->LastAPTime = m->timenow;
		if (rr->ThisAPInterval < mDNSPlatformOneSecond * 5)
			rr->ThisAPInterval = mDNSPlatformOneSecond * 5;
		return;
		}

	InitializeDNSMessage(&m->omsg.h, rr->updateid, UpdateReqFlags);

	ptr = putZone(&m->omsg, ptr, end, rr->zone, mDNSOpaque16fromIntVal(rr->resrec.rrclass));
	if (ptr) ptr = putDeletionRecord(&m->omsg, ptr, &rr->resrec);
	if (!ptr)
		{
		LogMsg("SendRecordDeregistration Error: could not contruct deregistration packet for %s", ARDisplayString(m, rr));
		if (rr->state == regState_DeregPending) CompleteDeregistration(m, rr);
		}
	else
		{
		rr->expire = 0;		// Indicate that we have no active registration any more
		if (rr->Private)
			{
			LogInfo("SendRecordDeregistration TCP %p %s", rr->tcp, ARDisplayString(m, rr));
			if (rr->tcp) LogInfo("SendRecordDeregistration: Disposing existing TCP connection for %s", ARDisplayString(m, rr));
			if (rr->tcp) DisposeTCPConn(rr->tcp);
			rr->tcp = MakeTCPConn(m, &m->omsg, ptr, kTCPSocketFlags_UseTLS, &rr->UpdateServer, rr->UpdatePort, mDNSNULL, mDNSNULL, rr);
			if (!rr->tcp) rr->ThisAPInterval = mDNSPlatformOneSecond * 5; // If failed to make TCP connection, try again in ten seconds (5*2)
			else if (rr->ThisAPInterval < mDNSPlatformOneSecond * 30) rr->ThisAPInterval = mDNSPlatformOneSecond * 30;
			SetRecordRetry(m, rr, mStatus_NoError);
			}
		else
			{
			mStatus err = mDNSSendDNSMessage(m, &m->omsg, ptr, mDNSInterface_Any, mDNSNULL, &rr->UpdateServer, rr->UpdatePort, mDNSNULL, GetAuthInfoForName_internal(m, rr->resrec.name));
			if (err) debugf("ERROR: SendRecordDeregistration - mDNSSendDNSMessage - %d", err);
			if (rr->state == regState_DeregPending) CompleteDeregistration(m, rr);		// Don't touch rr after this
			}
		}
	}

mDNSexport mStatus uDNS_DeregisterRecord(mDNS *const m, AuthRecord *const rr)
	{
	switch (rr->state)
		{
		case regState_NATMap:        LogMsg("regState_NATMap        %##s type %s", rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype)); return mStatus_NoError;
		case regState_ExtraQueued: rr->state = regState_Unregistered; break;
		case regState_Refresh:
		case regState_Pending:
		case regState_UpdatePending:
		case regState_FetchingZoneData:
		case regState_Registered: break;
		case regState_DeregPending: break;
		case regState_DeregDeferred: LogMsg("regState_DeregDeferred %##s type %s", rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype)); return mStatus_NoError;
		case regState_Unregistered:  LogMsg("regState_Unregistered  %##s type %s", rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype)); return mStatus_NoError;
		case regState_NATError:      LogMsg("regState_NATError      %##s type %s", rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype)); return mStatus_NoError;
		case regState_NoTarget:      LogMsg("regState_NoTarget      %##s type %s", rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype)); return mStatus_NoError;
		default: LogMsg("uDNS_DeregisterRecord: State %d for %##s type %s", rr->state, rr->resrec.name->c, DNSTypeName(rr->resrec.rrtype)); return mStatus_NoError;
		}

	if (rr->state != regState_Unregistered) { rr->state = regState_DeregPending; SendRecordDeregistration(m, rr); }
	return mStatus_NoError;
	}

// Called with lock held
mDNSexport mStatus uDNS_DeregisterService(mDNS *const m, ServiceRecordSet *srs)
	{
	char *errmsg = "Unknown State";

	if (m->mDNS_busy != m->mDNS_reentrancy+1)
		LogMsg("uDNS_DeregisterService: Lock not held! mDNS_busy (%ld) mDNS_reentrancy (%ld)", m->mDNS_busy, m->mDNS_reentrancy);

	// don't re-register with a new target following deregistration
	srs->SRVChanged = srs->SRVUpdateDeferred = mDNSfalse;

	if (srs->srs_nta) { CancelGetZoneData(m, srs->srs_nta); srs->srs_nta = mDNSNULL; }

	if (srs->NATinfo.clientContext)
		{
		mDNS_StopNATOperation_internal(m, &srs->NATinfo);
		srs->NATinfo.clientContext = mDNSNULL;
		}

	switch (srs->state)
		{
		case regState_Unregistered:
			debugf("uDNS_DeregisterService - service %##s not registered", srs->RR_SRV.resrec.name->c);
			return mStatus_BadReferenceErr;
		case regState_DeregPending:
		case regState_DeregDeferred:
			debugf("Double deregistration of service %##s", srs->RR_SRV.resrec.name->c);
			return mStatus_NoError;
		case regState_NATError:	// not registered
		case regState_NATMap:	// not registered
		case regState_NoTarget:	// not registered
			unlinkSRS(m, srs);
			srs->state = regState_Unregistered;
			mDNS_DropLockBeforeCallback();
			srs->ServiceCallback(m, srs, mStatus_MemFree);
			mDNS_ReclaimLockAfterCallback();
			return mStatus_NoError;
		case regState_Pending:
		case regState_Refresh:
		case regState_UpdatePending:
		case regState_FetchingZoneData:
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

mDNSexport mStatus uDNS_UpdateRecord(mDNS *m, AuthRecord *rr)
	{
	ServiceRecordSet *parent = mDNSNULL;
	AuthRecord *rptr;
	regState_t *stateptr = mDNSNULL;

	// find the record in registered service list
	for (parent = m->ServiceRegistrations; parent; parent = parent->uDNS_next)
		if (&parent->RR_TXT == rr) { stateptr = &parent->state; break; }

	if (!parent)
		{
		// record not part of a service - check individual record registrations
		for (rptr = m->ResourceRecords; rptr; rptr = rptr->next)
			if (rptr == rr) { stateptr = &rr->state; break; }
		if (!rptr) goto unreg_error;
		}

	switch(*stateptr)
		{
		case regState_DeregPending:
		case regState_DeregDeferred:
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
			// registration in-flight. queue rdata and return
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
			if (parent) SendServiceRegistration(m, parent);
			else SendRecordRegistration(m, rr);
			return mStatus_NoError;

		case regState_NATError:
			LogMsg("ERROR: uDNS_UpdateRecord called for record %##s with bad state regState_NATError", rr->resrec.name->c);
			return mStatus_UnknownErr;	// states for service records only

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

// The question to be checked is not passed in as an explicit parameter;
// instead it is implicit that the question to be checked is m->CurrentQuestion.
mDNSexport void uDNS_CheckCurrentQuestion(mDNS *const m)
	{
	DNSQuestion *q = m->CurrentQuestion;
	mDNSs32 sendtime = q->LastQTime + q->ThisQInterval;
	// Don't allow sendtime to be earlier than SuppressStdPort53Queries
	if (!q->LongLived && m->SuppressStdPort53Queries && sendtime - m->SuppressStdPort53Queries < 0)
		sendtime = m->SuppressStdPort53Queries;
	if (m->timenow - sendtime < 0) return;

	if (q->LongLived)
		{
		switch (q->state)
			{
			case LLQ_InitialRequest:   startLLQHandshake(m, q); break;
			case LLQ_SecondaryRequest: sendChallengeResponse(m, q, mDNSNULL); break;
			case LLQ_Established:      sendLLQRefresh(m, q); break;
			case LLQ_Poll:             break;	// Do nothing (handled below)
			}
		}

	// We repeat the check above (rather than just making this the "else" case) because startLLQHandshake can change q->state to LLQ_Poll
	if (!(q->LongLived && q->state != LLQ_Poll))
		{
		if (q->unansweredQueries >= MAX_UCAST_UNANSWERED_QUERIES)
			{
			DNSServer *orig = q->qDNSServer;
			if (orig) LogInfo("Sent %d unanswered queries for %##s (%s) to %#a:%d (%##s)", q->unansweredQueries, q->qname.c, DNSTypeName(q->qtype), &orig->addr, mDNSVal16(orig->port), orig->domain.c);

			PushDNSServerToEnd(m, q);
			q->unansweredQueries = 0;
			}

		if (q->qDNSServer && q->qDNSServer->teststate != DNSServer_Disabled)
			{
			mDNSu8 *end = m->omsg.data;
			mStatus err = mStatus_NoError;
			mDNSBool private = mDNSfalse;

			InitializeDNSMessage(&m->omsg.h, q->TargetQID, uQueryFlags);

			if (q->qDNSServer->teststate != DNSServer_Untested || NoTestQuery(q))
				{
				end = putQuestion(&m->omsg, m->omsg.data, m->omsg.data + AbsoluteMaxDNSMessageData, &q->qname, q->qtype, q->qclass);
				private = (q->AuthInfo && q->AuthInfo->AutoTunnel);
				}
			else if (m->timenow - q->qDNSServer->lasttest >= INIT_UCAST_POLL_INTERVAL)	// Make sure at least three seconds has elapsed since last test query
				{
				LogInfo("Sending DNS test query to %#a:%d", &q->qDNSServer->addr, mDNSVal16(q->qDNSServer->port));
				q->ThisQInterval = INIT_UCAST_POLL_INTERVAL / QuestionIntervalStep;
				q->qDNSServer->lasttest = m->timenow;
				end = putQuestion(&m->omsg, m->omsg.data, m->omsg.data + AbsoluteMaxDNSMessageData, DNSRelayTestQuestion, kDNSType_PTR, kDNSClass_IN);
				q->qDNSServer->testid = m->omsg.h.id;
				}

			if (end > m->omsg.data && (q->qDNSServer->teststate != DNSServer_Failed || NoTestQuery(q)))
				{
				//LogMsg("uDNS_CheckCurrentQuestion %p %d %p %##s (%s)", q, sendtime - m->timenow, private, q->qname.c, DNSTypeName(q->qtype));
				if (private)
					{
					if (q->nta) CancelGetZoneData(m, q->nta);
					q->nta = StartGetZoneData(m, &q->qname, q->LongLived ? ZoneServiceLLQ : ZoneServiceQuery, PrivateQueryGotZoneData, q);
					q->ThisQInterval = (LLQ_POLL_INTERVAL + mDNSRandom(LLQ_POLL_INTERVAL/10)) / QuestionIntervalStep;
					}
				else
					{
					if (!q->LocalSocket) q->LocalSocket = mDNSPlatformUDPSocket(m, zeroIPPort);
					if (!q->LocalSocket) err = mStatus_NoMemoryErr;	// If failed to make socket (should be very rare), we'll try again next time
					else err = mDNSSendDNSMessage(m, &m->omsg, end, q->qDNSServer->interface, q->LocalSocket, &q->qDNSServer->addr, q->qDNSServer->port, mDNSNULL, mDNSNULL);
					m->SuppressStdPort53Queries = NonZeroTime(m->timenow + (mDNSPlatformOneSecond+99)/100);
					}
				}

			if (err) debugf("ERROR: uDNS_idle - mDNSSendDNSMessage - %d", err); // surpress syslog messages if we have no network
			else
				{
				q->ThisQInterval = q->ThisQInterval * QuestionIntervalStep;	// Only increase interval if send succeeded
				q->unansweredQueries++;
				if (q->ThisQInterval > MAX_UCAST_POLL_INTERVAL)
					q->ThisQInterval = MAX_UCAST_POLL_INTERVAL;
				debugf("Increased ThisQInterval to %d for %##s (%s)", q->ThisQInterval, q->qname.c, DNSTypeName(q->qtype));
				}
			q->LastQTime = m->timenow;
			SetNextQueryTime(m, q);
			}
		else
			{
			// If we have no server for this query, or the only server is a disabled one, then we deliver
			// a transient failure indication to the client. This is important for things like iPhone
			// where we want to return timely feedback to the user when no network is available.
			// After calling MakeNegativeCacheRecord() we store the resulting record in the
			// cache so that it will be visible to other clients asking the same question.
			// (When we have a group of identical questions, only the active representative of the group gets
			// passed to uDNS_CheckCurrentQuestion -- we only want one set of query packets hitting the wire --
			// but we want *all* of the questions to get answer callbacks.)

			CacheRecord *rr;
			const mDNSu32 slot = HashSlot(&q->qname);
			CacheGroup *const cg = CacheGroupForName(m, slot, q->qnamehash, &q->qname);
			if (cg)
				for (rr = cg->members; rr; rr=rr->next)
					if (SameNameRecordAnswersQuestion(&rr->resrec, q)) mDNS_PurgeCacheResourceRecord(m, rr);

			if (!q->qDNSServer) debugf("uDNS_CheckCurrentQuestion no DNS server for %##s (%s)", q->qname.c, DNSTypeName(q->qtype));
			else LogMsg("uDNS_CheckCurrentQuestion DNS server %#a:%d for %##s is disabled", &q->qDNSServer->addr, mDNSVal16(q->qDNSServer->port), q->qname.c);

			MakeNegativeCacheRecord(m, &m->rec.r, &q->qname, q->qnamehash, q->qtype, q->qclass, 60, mDNSInterface_Any);
			// Inactivate this question until the next change of DNS servers (do this before AnswerCurrentQuestionWithResourceRecord)
			q->ThisQInterval = 0;
			q->unansweredQueries = 0;
			CreateNewCacheEntry(m, slot, cg);
			m->rec.r.resrec.RecordType = 0;		// Clear RecordType to show we're not still using it
			// MUST NOT touch m->CurrentQuestion (or q) after this -- client callback could have deleted it
			}
		}
	}

mDNSlocal void CheckNATMappings(mDNS *m)
	{
	mStatus err = mStatus_NoError;
	mDNSBool rfc1918 = mDNSv4AddrIsRFC1918(&m->AdvertisedV4.ip.v4);
	mDNSBool HaveRoutable = !rfc1918 && !mDNSIPv4AddressIsZero(m->AdvertisedV4.ip.v4);
	m->NextScheduledNATOp = m->timenow + 0x3FFFFFFF;

	if (HaveRoutable) m->ExternalAddress = m->AdvertisedV4.ip.v4;

	if (m->NATTraversals && rfc1918)			// Do we need to open NAT-PMP socket to receive multicast announcements from router?
		{
		if (m->NATMcastRecvskt == mDNSNULL)		// If we are behind a NAT and the socket hasn't been opened yet, open it
			{
			// we need to log a message if we can't get our socket, but only the first time (after success)
			static mDNSBool needLog = mDNStrue;
			m->NATMcastRecvskt = mDNSPlatformUDPSocket(m, NATPMPAnnouncementPort);
			if (!m->NATMcastRecvskt)
				{
				if (needLog)
					{
					LogMsg("CheckNATMappings: Failed to allocate port 5350 UDP multicast socket for NAT-PMP announcements");
					needLog = mDNSfalse;
					}
				}
			else
				needLog = mDNStrue;				
			}
		}
	else										// else, we don't want to listen for announcements, so close them if they're open
		{
		if (m->NATMcastRecvskt) { mDNSPlatformUDPClose(m->NATMcastRecvskt); m->NATMcastRecvskt = mDNSNULL; }
		if (m->SSDPSocket)      { debugf("CheckNATMappings destroying SSDPSocket %p", &m->SSDPSocket); mDNSPlatformUDPClose(m->SSDPSocket); m->SSDPSocket = mDNSNULL; }
		}

	if (m->NATTraversals)
		{
		if (m->timenow - m->retryGetAddr >= 0)
			{
			err = uDNS_SendNATMsg(m, mDNSNULL);		// Will also do UPnP discovery for us, if necessary
			if (!err)
				{
				if      (m->retryIntervalGetAddr < NATMAP_INIT_RETRY)             m->retryIntervalGetAddr = NATMAP_INIT_RETRY;
				else if (m->retryIntervalGetAddr < NATMAP_MAX_RETRY_INTERVAL / 2) m->retryIntervalGetAddr *= 2;
				else                                                              m->retryIntervalGetAddr = NATMAP_MAX_RETRY_INTERVAL;
				}
			// Always update m->retryGetAddr, even if we fail to send the packet. Otherwise in cases where we can't send the packet
			// (like when we have no active interfaces) we'll spin in an infinite loop repeatedly failing to send the packet
			m->retryGetAddr = m->timenow + m->retryIntervalGetAddr;
			}
		// Even when we didn't send the GetAddr packet, still need to make sure NextScheduledNATOp is set correctly
		if (m->NextScheduledNATOp - m->retryGetAddr > 0)
			m->NextScheduledNATOp = m->retryGetAddr;
		}

	if (m->CurrentNATTraversal) LogMsg("WARNING m->CurrentNATTraversal already in use");
	m->CurrentNATTraversal = m->NATTraversals;

	while (m->CurrentNATTraversal)
		{
		NATTraversalInfo *cur = m->CurrentNATTraversal;
		m->CurrentNATTraversal = m->CurrentNATTraversal->next;

		if (HaveRoutable)		// If not RFC 1918 address, our own address and port are effectively our external address and port
			{
			cur->ExpiryTime = 0;
			cur->NewResult  = mStatus_NoError;
			}
		else if (cur->Protocol)		// Check if it's time to send port mapping packets
			{
			if (m->timenow - cur->retryPortMap >= 0)						// Time to do something with this mapping
				{
				if (cur->ExpiryTime && cur->ExpiryTime - m->timenow < 0)	// Mapping has expired
					{
					cur->ExpiryTime    = 0;
					cur->retryInterval = NATMAP_INIT_RETRY;
					}

				//LogMsg("uDNS_SendNATMsg");
				err = uDNS_SendNATMsg(m, cur);

				if (cur->ExpiryTime)						// If have active mapping then set next renewal time halfway to expiry
					NATSetNextRenewalTime(m, cur);
				else										// else no mapping; use exponential backoff sequence
					{
					if      (cur->retryInterval < NATMAP_INIT_RETRY            ) cur->retryInterval = NATMAP_INIT_RETRY;
					else if (cur->retryInterval < NATMAP_MAX_RETRY_INTERVAL / 2) cur->retryInterval *= 2;
					else                                                         cur->retryInterval = NATMAP_MAX_RETRY_INTERVAL;
					cur->retryPortMap = m->timenow + cur->retryInterval;
					}
				}

			if (m->NextScheduledNATOp - cur->retryPortMap > 0)
				m->NextScheduledNATOp = cur->retryPortMap;
			}

		// Notify the client if necessary. We invoke the callback if:
		// (1) we have an ExternalAddress, or we've tried and failed a couple of times to discover it
		// and (2) the client doesn't want a mapping, or the client won't need a mapping, or the client has a successful mapping, or we've tried and failed a couple of times
		// and (3) we have new data to give the client that's changed since the last callback
		if (!mDNSIPv4AddressIsZero(m->ExternalAddress) || m->retryIntervalGetAddr > NATMAP_INIT_RETRY * 8)
			{
			const mStatus EffectiveResult = cur->NewResult ? cur->NewResult : mDNSv4AddrIsRFC1918(&m->ExternalAddress) ? mStatus_DoubleNAT : mStatus_NoError;
			const mDNSIPPort ExternalPort = HaveRoutable ? cur->IntPort :
				!mDNSIPv4AddressIsZero(m->ExternalAddress) && cur->ExpiryTime ? cur->RequestedPort : zeroIPPort;
			if (!cur->Protocol || HaveRoutable || cur->ExpiryTime || cur->retryInterval > NATMAP_INIT_RETRY * 8)
				if (!mDNSSameIPv4Address(cur->ExternalAddress, m->ExternalAddress) ||
					!mDNSSameIPPort     (cur->ExternalPort,       ExternalPort)    ||
					cur->Result != EffectiveResult)
					{
					//LogMsg("NAT callback %d %d %d", cur->Protocol, cur->ExpiryTime, cur->retryInterval);
					if (cur->Protocol && mDNSIPPortIsZero(ExternalPort) && !mDNSIPv4AddressIsZero(m->Router.ip.v4))
						{
						if (!EffectiveResult)
							LogInfo("Failed to obtain NAT port mapping %p from router %#a external address %.4a internal port %5d interval %d error %d",
								cur, &m->Router, &m->ExternalAddress, mDNSVal16(cur->IntPort), cur->retryInterval, EffectiveResult);
						else
							LogMsg("Failed to obtain NAT port mapping %p from router %#a external address %.4a internal port %5d interval %d error %d",
								cur, &m->Router, &m->ExternalAddress, mDNSVal16(cur->IntPort), cur->retryInterval, EffectiveResult);
						}

					cur->ExternalAddress = m->ExternalAddress;
					cur->ExternalPort    = ExternalPort;
					cur->Lifetime        = cur->ExpiryTime && !mDNSIPPortIsZero(ExternalPort) ?
						(cur->ExpiryTime - m->timenow + mDNSPlatformOneSecond/2) / mDNSPlatformOneSecond : 0;
					cur->Result          = EffectiveResult;
					mDNS_DropLockBeforeCallback();		// Allow client to legally make mDNS API calls from the callback
					if (cur->clientCallback)
						cur->clientCallback(m, cur);
					mDNS_ReclaimLockAfterCallback();	// Decrement mDNS_reentrancy to block mDNS API calls again
					// MUST NOT touch cur after invoking the callback
					}
			}
		}
	}

mDNSlocal mDNSs32 CheckRecordRegistrations(mDNS *m)
	{
	AuthRecord *rr;
	mDNSs32 nextevent = m->timenow + 0x3FFFFFFF;

	for (rr = m->ResourceRecords; rr; rr = rr->next)
		{
		if (rr->state == regState_FetchingZoneData ||
			rr->state == regState_Pending || rr->state == regState_DeregPending || rr->state == regState_UpdatePending ||
			rr->state == regState_DeregDeferred || rr->state == regState_Refresh || rr->state == regState_Registered)
			{
			if (rr->LastAPTime + rr->ThisAPInterval - m->timenow < 0)
				{
				if (rr->tcp) { DisposeTCPConn(rr->tcp); rr->tcp = mDNSNULL; }
				if (rr->state == regState_FetchingZoneData)
					{
					if (rr->nta) CancelGetZoneData(m, rr->nta);
					rr->nta = StartGetZoneData(m, rr->resrec.name, ZoneServiceUpdate, RecordRegistrationGotZoneData, rr);
					SetRecordRetry(m, rr, mStatus_NoError);
					}
				else if (rr->state == regState_DeregPending) SendRecordDeregistration(m, rr);
				else SendRecordRegistration(m, rr);
				}
			if (nextevent - (rr->LastAPTime + rr->ThisAPInterval) > 0)
				nextevent = (rr->LastAPTime + rr->ThisAPInterval);
			}
		}
	return nextevent;
	}

mDNSlocal mDNSs32 CheckServiceRegistrations(mDNS *m)
	{
	mDNSs32 nextevent = m->timenow + 0x3FFFFFFF;

	if (CurrentServiceRecordSet)
		LogMsg("CheckServiceRegistrations ERROR CurrentServiceRecordSet already set");
	CurrentServiceRecordSet = m->ServiceRegistrations;

	// Note: ServiceRegistrations list is in the order they were created; important for in-order event delivery
	while (CurrentServiceRecordSet)
		{
		ServiceRecordSet *srs = CurrentServiceRecordSet;
		CurrentServiceRecordSet = CurrentServiceRecordSet->uDNS_next;
		if (srs->state == regState_FetchingZoneData ||
			srs->state == regState_Pending || srs->state == regState_DeregPending  || srs->state == regState_DeregDeferred ||
			srs->state == regState_Refresh || srs->state == regState_UpdatePending || srs->state == regState_Registered)
			{
			if (srs->RR_SRV.LastAPTime + srs->RR_SRV.ThisAPInterval - m->timenow <= 0)
				{
				if (srs->tcp) { DisposeTCPConn(srs->tcp); srs->tcp = mDNSNULL; }
				if (srs->state == regState_FetchingZoneData)
					{
					if (srs->srs_nta) CancelGetZoneData(m, srs->srs_nta);
					srs->srs_nta = StartGetZoneData(m, srs->RR_SRV.resrec.name, ZoneServiceUpdate, ServiceRegistrationGotZoneData, srs);
					SetRecordRetry(m, &srs->RR_SRV, mStatus_NoError);
					}
				else if (srs->state == regState_DeregPending) SendServiceDeregistration(m, srs);
				else SendServiceRegistration(m, srs);
				}
			if (nextevent - (srs->RR_SRV.LastAPTime + srs->RR_SRV.ThisAPInterval) > 0)
				nextevent = (srs->RR_SRV.LastAPTime + srs->RR_SRV.ThisAPInterval);
			}
		}
	return nextevent;
	}

mDNSexport void uDNS_Execute(mDNS *const m)
	{
	mDNSs32 nexte;

	m->NextuDNSEvent = m->timenow + 0x3FFFFFFF;

	if (m->NextSRVUpdate && m->NextSRVUpdate - m->timenow < 0)
		{ m->NextSRVUpdate = 0; UpdateSRVRecords(m); }

	CheckNATMappings(m);

	if (m->SuppressStdPort53Queries && m->timenow - m->SuppressStdPort53Queries >= 0)
		m->SuppressStdPort53Queries = 0; // If suppression time has passed, clear it

	nexte = CheckRecordRegistrations(m);
	if (nexte - m->NextuDNSEvent < 0) m->NextuDNSEvent = nexte;

	nexte = CheckServiceRegistrations(m);
	if (nexte - m->NextuDNSEvent < 0) m->NextuDNSEvent = nexte;
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Startup, Shutdown, and Sleep
#endif

// simplest sleep logic - rather than having sleep states that must be dealt with explicitly in all parts of
// the code, we simply send a deregistration, and put the service in Refresh state, with a timeout far enough
// in the future that we'll sleep (or the sleep will be cancelled) before it is retransmitted. Then to wake,
// we just move up the timers.

mDNSexport void SleepRecordRegistrations(mDNS *m)
	{
	AuthRecord *rr;
	for (rr = m->ResourceRecords; rr; rr=rr->next)
		if (AuthRecord_uDNS(rr))
			if (rr->state == regState_Registered ||
				rr->state == regState_Refresh)
				{
				SendRecordDeregistration(m, rr);
				rr->state = regState_Refresh;
				rr->LastAPTime = m->timenow;
				rr->ThisAPInterval = 300 * mDNSPlatformOneSecond;
				}
	}

mDNSexport void SleepServiceRegistrations(mDNS *m)
	{
	ServiceRecordSet *srs = m->ServiceRegistrations;
	while (srs)
		{
		LogInfo("SleepServiceRegistrations: state %d %s", srs->state, ARDisplayString(m, &srs->RR_SRV));
		if (srs->srs_nta) { CancelGetZoneData(m, srs->srs_nta); srs->srs_nta = mDNSNULL; }

		if (srs->NATinfo.clientContext)
			{
			mDNS_StopNATOperation_internal(m, &srs->NATinfo);
			srs->NATinfo.clientContext = mDNSNULL;
			}

		if (srs->state == regState_UpdatePending)
			{
			// act as if the update succeeded, since we're about to delete the name anyway
			AuthRecord *txt = &srs->RR_TXT;
			srs->state = regState_Registered;
			// deallocate old RData
			if (txt->UpdateCallback) txt->UpdateCallback(m, txt, txt->OrigRData);
			SetNewRData(&txt->resrec, txt->InFlightRData, txt->InFlightRDLen);
			txt->OrigRData = mDNSNULL;
			txt->InFlightRData = mDNSNULL;
			}

		if (srs->state == regState_Registered || srs->state == regState_Refresh)
			SendServiceDeregistration(m, srs);

		srs->state = regState_NoTarget;	// when we wake, we'll re-register (and optionally nat-map) once our address record completes
		srs->RR_SRV.resrec.rdata->u.srv.target.c[0] = 0;
		srs->SRSUpdateServer = zeroAddr;		// This will cause UpdateSRV to do a new StartGetZoneData
		srs->RR_SRV.ThisAPInterval = 5 * mDNSPlatformOneSecond;		// After doubling, first retry will happen after ten seconds

		srs = srs->uDNS_next;
		}
	}

mDNSexport void mDNS_AddSearchDomain(const domainname *const domain)
	{
	SearchListElem **p;

	// Check to see if we already have this domain in our list
	for (p = &SearchList; *p; p = &(*p)->next)
		if (SameDomainName(&(*p)->domain, domain))
			{
			// If domain is already in list, and marked for deletion, change it to "leave alone"
			if ((*p)->flag == -1) (*p)->flag = 0;
			LogInfo("mDNS_AddSearchDomain already in list %##s", domain->c);
			return;
			}

	// if domain not in list, add to list, mark as add (1)
	*p = mDNSPlatformMemAllocate(sizeof(SearchListElem));
	if (!*p) { LogMsg("ERROR: mDNS_AddSearchDomain - malloc"); return; }
	mDNSPlatformMemZero(*p, sizeof(SearchListElem));
	AssignDomainName(&(*p)->domain, domain);
	(*p)->flag = 1;	// add
	(*p)->next = mDNSNULL;
	LogInfo("mDNS_AddSearchDomain created new %##s", domain->c);
	}

mDNSlocal void FreeARElemCallback(mDNS *const m, AuthRecord *const rr, mStatus result)
	{
	(void)m;	// unused
	if (result == mStatus_MemFree) mDNSPlatformMemFree(rr->RecordContext);
	}

mDNSlocal void FoundDomain(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, QC_result AddRecord)
	{
	SearchListElem *slElem = question->QuestionContext;
	mStatus err;
	const char *name;

	if (answer->rrtype != kDNSType_PTR) return;
	if (answer->RecordType == kDNSRecordTypePacketNegative) return;
	if (answer->InterfaceID == mDNSInterface_LocalOnly) return;

	if      (question == &slElem->BrowseQ)          name = mDNS_DomainTypeNames[mDNS_DomainTypeBrowse];
	else if (question == &slElem->DefBrowseQ)       name = mDNS_DomainTypeNames[mDNS_DomainTypeBrowseDefault];
	else if (question == &slElem->AutomaticBrowseQ) name = mDNS_DomainTypeNames[mDNS_DomainTypeBrowseAutomatic];
	else if (question == &slElem->RegisterQ)        name = mDNS_DomainTypeNames[mDNS_DomainTypeRegistration];
	else if (question == &slElem->DefRegisterQ)     name = mDNS_DomainTypeNames[mDNS_DomainTypeRegistrationDefault];
	else { LogMsg("FoundDomain - unknown question"); return; }

	LogInfo("FoundDomain: %p %s %s Q %##s A %s", answer->InterfaceID, AddRecord ? "Add" : "Rmv", name, question->qname.c, RRDisplayString(m, answer));

	if (AddRecord)
		{
		ARListElem *arElem = mDNSPlatformMemAllocate(sizeof(ARListElem));
		if (!arElem) { LogMsg("ERROR: FoundDomain out of memory"); return; }
		mDNS_SetupResourceRecord(&arElem->ar, mDNSNULL, mDNSInterface_LocalOnly, kDNSType_PTR, 7200, kDNSRecordTypeShared, FreeARElemCallback, arElem);
		MakeDomainNameFromDNSNameString(&arElem->ar.namestorage, name);
		AppendDNSNameString            (&arElem->ar.namestorage, "local");
		AssignDomainName(&arElem->ar.resrec.rdata->u.name, &answer->rdata->u.name);
		LogInfo("FoundDomain: Registering %s", ARDisplayString(m, &arElem->ar));
		err = mDNS_Register(m, &arElem->ar);
		if (err) { LogMsg("ERROR: FoundDomain - mDNS_Register returned %d", err); mDNSPlatformMemFree(arElem); return; }
		arElem->next = slElem->AuthRecs;
		slElem->AuthRecs = arElem;
		}
	else
		{
		ARListElem **ptr = &slElem->AuthRecs;
		while (*ptr)
			{
			if (SameDomainName(&(*ptr)->ar.resrec.rdata->u.name, &answer->rdata->u.name))
				{
				ARListElem *dereg = *ptr;
				*ptr = (*ptr)->next;
				LogInfo("FoundDomain: Deregistering %s", ARDisplayString(m, &dereg->ar));
				err = mDNS_Deregister(m, &dereg->ar);
				if (err) LogMsg("ERROR: FoundDomain - mDNS_Deregister returned %d", err);
				// Memory will be freed in the FreeARElemCallback
				}
			else
				ptr = &(*ptr)->next;
			}
		}
	}

#if APPLE_OSX_mDNSResponder && MACOSX_MDNS_MALLOC_DEBUGGING
mDNSexport void udns_validatelists(void *const v)
	{
	mDNS *const m = v;

	ServiceRecordSet *s;
	for (s = m->ServiceRegistrations; s; s=s->uDNS_next)
		if (s->uDNS_next == (ServiceRecordSet*)~0)
			LogMemCorruption("m->ServiceRegistrations: %p is garbage (%lX)", s, s->uDNS_next);

	NATTraversalInfo *n;
	for (n = m->NATTraversals; n; n=n->next)
		if (n->next == (NATTraversalInfo *)~0 || n->clientCallback == (NATTraversalClientCallback)~0)
			LogMemCorruption("m->NATTraversals: %p is garbage", n);

	DNSServer *d;
	for (d = m->DNSServers; d; d=d->next)
		if (d->next == (DNSServer *)~0 || d->teststate > DNSServer_Disabled)
			LogMemCorruption("m->DNSServers: %p is garbage (%d)", d, d->teststate);

	DomainAuthInfo *info;
	for (info = m->AuthInfoList; info; info = info->next)
		if (info->next == (DomainAuthInfo *)~0 || info->AutoTunnel == (mDNSBool)~0)
			LogMemCorruption("m->AuthInfoList: %p is garbage (%X)", info, info->AutoTunnel);

	HostnameInfo *hi;
	for (hi = m->Hostnames; hi; hi = hi->next)
		if (hi->next == (HostnameInfo *)~0 || hi->StatusCallback == (mDNSRecordCallback*)~0)
			LogMemCorruption("m->Hostnames: %p is garbage", n);

	SearchListElem *ptr;
	for (ptr = SearchList; ptr; ptr = ptr->next)
		if (ptr->next == (SearchListElem *)~0 || ptr->AuthRecs == (void*)~0)
			LogMemCorruption("SearchList: %p is garbage (%X)", ptr, ptr->AuthRecs);
	}
#endif

// This should probably move to the UDS daemon -- the concept of legacy clients and automatic registration / automatic browsing
// is really a UDS API issue, not something intrinsic to uDNS

mDNSexport mStatus uDNS_RegisterSearchDomains(mDNS *const m)
	{
	SearchListElem **p = &SearchList, *ptr;
	mStatus err;

	// step 1: mark each element for removal (-1)
	for (ptr = SearchList; ptr; ptr = ptr->next) ptr->flag = -1;

	// Client has requested domain enumeration or automatic browse -- time to make sure we have the search domains from the platform layer
	mDNS_Lock(m);
	m->RegisterSearchDomains = mDNStrue;
	mDNSPlatformSetDNSConfig(m, mDNSfalse, m->RegisterSearchDomains, mDNSNULL, mDNSNULL, mDNSNULL);
	mDNS_Unlock(m);

	// delete elems marked for removal, do queries for elems marked add
	while (*p)
		{
		ptr = *p;
		LogInfo("RegisterSearchDomains %d %p %##s", ptr->flag, ptr->AuthRecs, ptr->domain.c);
		if (ptr->flag == -1)	// remove
			{
			ARListElem *arList = ptr->AuthRecs;
			ptr->AuthRecs = mDNSNULL;
			*p = ptr->next;

			// If the user has "local" in their DNS searchlist, we ignore that for the purposes of domain enumeration queries
			if (!SameDomainName(&ptr->domain, &localdomain))
				{
				mDNS_StopGetDomains(m, &ptr->BrowseQ);
				mDNS_StopGetDomains(m, &ptr->RegisterQ);
				mDNS_StopGetDomains(m, &ptr->DefBrowseQ);
				mDNS_StopGetDomains(m, &ptr->DefRegisterQ);
				mDNS_StopGetDomains(m, &ptr->AutomaticBrowseQ);
				}
			mDNSPlatformMemFree(ptr);

	        // deregister records generated from answers to the query
			while (arList)
				{
				ARListElem *dereg = arList;
				arList = arList->next;
				debugf("Deregistering PTR %##s -> %##s", dereg->ar.resrec.name->c, dereg->ar.resrec.rdata->u.name.c);
				err = mDNS_Deregister(m, &dereg->ar);
				if (err) LogMsg("ERROR: RegisterSearchDomains mDNS_Deregister returned %d", err);
				// Memory will be freed in the FreeARElemCallback
				}
			continue;
			}

		if (ptr->flag == 1)	// add
			{
			// If the user has "local" in their DNS searchlist, we ignore that for the purposes of domain enumeration queries
			if (!SameDomainName(&ptr->domain, &localdomain))
				{
				mStatus err1, err2, err3, err4, err5;
				err1 = mDNS_GetDomains(m, &ptr->BrowseQ,          mDNS_DomainTypeBrowse,              &ptr->domain, mDNSInterface_Any, FoundDomain, ptr);
				err2 = mDNS_GetDomains(m, &ptr->DefBrowseQ,       mDNS_DomainTypeBrowseDefault,       &ptr->domain, mDNSInterface_Any, FoundDomain, ptr);
				err3 = mDNS_GetDomains(m, &ptr->RegisterQ,        mDNS_DomainTypeRegistration,        &ptr->domain, mDNSInterface_Any, FoundDomain, ptr);
				err4 = mDNS_GetDomains(m, &ptr->DefRegisterQ,     mDNS_DomainTypeRegistrationDefault, &ptr->domain, mDNSInterface_Any, FoundDomain, ptr);
				err5 = mDNS_GetDomains(m, &ptr->AutomaticBrowseQ, mDNS_DomainTypeBrowseAutomatic,     &ptr->domain, mDNSInterface_Any, FoundDomain, ptr);
				if (err1 || err2 || err3 || err4 || err5)
					LogMsg("GetDomains for domain %##s returned error(s):\n"
						   "%d (mDNS_DomainTypeBrowse)\n"
						   "%d (mDNS_DomainTypeBrowseDefault)\n"
						   "%d (mDNS_DomainTypeRegistration)\n"
						   "%d (mDNS_DomainTypeRegistrationDefault)"
						   "%d (mDNS_DomainTypeBrowseAutomatic)\n",
						   ptr->domain.c, err1, err2, err3, err4, err5);
				}
			ptr->flag = 0;
			}

		if (ptr->flag) { LogMsg("RegisterSearchDomains - unknown flag %d. Skipping.", ptr->flag); }

		p = &ptr->next;
		}

	return mStatus_NoError;
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

struct CompileTimeAssertionChecks_uDNS
	{
	// Check our structures are reasonable sizes. Including overly-large buffers, or embedding
	// other overly-large structures instead of having a pointer to them, can inadvertently
	// cause structure sizes (and therefore memory usage) to balloon unreasonably.
	char sizecheck_tcpInfo_t     [(sizeof(tcpInfo_t)      <=  9056) ? 1 : -1];
	char sizecheck_SearchListElem[(sizeof(SearchListElem) <=  3920) ? 1 : -1];
	};
