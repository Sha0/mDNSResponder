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

    Change History (most recent first):
    
$Log: uDNS.c,v $
Revision 1.2  2004/01/22 03:48:41  cheshire
Make sure uDNS client doesn't accidentally use query ID zero

Revision 1.1  2003/12/13 03:05:27  ksekar
Bug #: <rdar://problem/3192548>: DynDNS: Unicast query of service records

 */
 
#include "uDNS.h"

#ifndef NULL
#define NULL mDNSNULL
#endif // NULL

//#define HARDCODE_NS
#ifdef HARDCODE_NS

static mDNSAddr   hcNS = { mDNSAddrType_IPv4, { { { 17,   202,   41, 107 } } } };

#endif // HARDCODE_NS



mDNSexport void mDNS_RegisterDNS(mDNS *const m, mDNSv4Addr *const dnsAddr)
    {
    //!!!KRS do this dynamically!
    uDNS_data_t *u = &m->uDNS_data;
    int i;
    
    if (!dnsAddr->NotAnInteger)
        {
        LogMsg("ERROR: attempt to register DNS with IP address 0");
        return;
        }

    for (i = 0; i < 32; i++)
        {
        if (!u->Servers[i].NotAnInteger)
            {
            u->Servers[i].NotAnInteger = dnsAddr->NotAnInteger;
            return;
            }
        if (u->Servers[i].NotAnInteger == dnsAddr->NotAnInteger)
            {
            LogMsg("ERROR: mDNS_RegisterDNS - DNS already registered");
            return;
            }
        }
    if (i == 32) {  LogMsg("ERROR: mDNS_RegisterDNS - too many registered servers");  }
    
    }





mDNSexport void mDNS_DeregisterDNS(mDNS *const m, mDNSv4Addr *const dnsAddr)
    {
    uDNS_data_t *u = &m->uDNS_data;
    int i;
    
    if (!dnsAddr->NotAnInteger)
        {
        LogMsg("ERROR: attempt to deregister DNS with IP address 0");
        return;
        }

    for (i = 0; i < 32; i++)
        {

        if (u->Servers[i].NotAnInteger == dnsAddr->NotAnInteger)
            {
            u->Servers[i].NotAnInteger = 0;
            return;
            }
        }
    if (i == 32) {  LogMsg("ERROR: mDNS_DeregisterDNS - no such DNS registered");  }
    }


mDNSexport void mDNS_DeregisterDNSList(mDNS *const m)
    {
    mDNSPlatformMemZero(m->uDNS_data.Servers, 32 * sizeof(mDNSv4Addr));
    }
    

mDNSlocal void uDNS_ProcessAnswers(mDNS *const m, uDNS_data_t *u, DNSMessage *const msg, const mDNSu8 *ansptr, const mDNSu8 *const end, const mDNSInterfaceID ifid)
    {
    int i;
    LargeCacheRecord rr;
    DNSQuestion *qptr;

    for (i = 0; i < msg->h.numAnswers; i++)
        {
        ansptr = GetLargeResourceRecord(m, msg, ansptr, end, ifid, kDNSRecordTypePacketAns, &rr);
        if (!ansptr) return;
        for (qptr = u->ActiveQueries; qptr; qptr = qptr->next)
            {
            if (ResourceRecordAnswersQuestion(&rr.r.resrec, qptr))
                {
                if (msg->h.id.NotAnInteger != qptr->uDNS_info.id.NotAnInteger)
                    {
                    LogMsg("uDNS_ProcessAnswers: ResourceRecord answers active question with non-matching ID.  Discarding.");
                    continue;
                    }
                if (!qptr->QuestionCallback)
                    {
                    LogMsg("uDNSProcessAnswers: NULL question callback.");
                    continue;
                    }
                qptr->QuestionCallback(m, qptr, &rr.r.resrec, 1);     //!!!KRS how are we going to detect removes?
                }
            }
        }                              
    }       

mDNSexport void uDNS_ReceiveMsg(mDNS *const m, DNSMessage *const msg, const mDNSu8 *const end,
	const mDNSAddr *const srcaddr, const mDNSIPPort srcport, const mDNSAddr *const dstaddr, 
	const mDNSIPPort dstport, const mDNSInterfaceID InterfaceID, mDNSu8 ttl)
	{
	uDNS_data_t *u = &m->uDNS_data;
	int i;
	const mDNSu8 *rrptr = msg->data;
	
	// unused
	(void)srcaddr;
	(void)srcport;
	(void)dstaddr;
	(void)dstport;
	(void)ttl;
	
	
	
	for (i = 0; i < msg->h.numQuestions; i++)
	    {
	    rrptr = skipQuestion(msg, rrptr, end);
	    if (!rrptr) return;
	    }
	
	if (msg->h.numAnswers)
	    {
	    uDNS_ProcessAnswers(m, u, msg, rrptr, end, InterfaceID);
	    return;
	    }	
	}

mDNSexport mDNSBool IsActiveUnicastQuery(DNSQuestion *const question)
    {
    return (question->uDNS_info.id.NotAnInteger != 0);
    }

mDNSexport mStatus uDNS_StopQuery(mDNS *const m, DNSQuestion *const question)
    {
     uDNS_data_t *u = &m->uDNS_data;
     DNSQuestion *qptr, *prev = NULL;
     char errbuf[MAX_ESCAPED_DOMAIN_NAME];
     
     for (qptr = u->ActiveQueries; qptr; qptr = qptr->next)
        {
        if (qptr == question)
            {
            if (prev) prev->next = question->next;
            else u->ActiveQueries = question->next;
            return mStatus_NoError;
            }
        prev = question;
        }
    LogMsg("uDNS_StopQuery: no such active query (%s)", ConvertDomainNameToCString(&question->qname, errbuf));
    return mStatus_UnknownErr;
    }
    
//!!!KRS this should go away, and we should be smarter!    
mDNSlocal mStatus SendMessageToAllServers(const mDNS *const m, uDNS_data_t *u, DNSMessage *const msg, const mDNSu8 *const end, mDNSInterfaceID InterfaceID)
    {
    int i;
    mStatus err = mStatus_NoError;
    mDNSAddr addr;
    
    addr.type = mDNSAddrType_IPv4;
    
    for (i = 0; i < 32; i++)
        {
        if (u->Servers[i].NotAnInteger)
            {
            addr.ip.v4.NotAnInteger = u->Servers[i].NotAnInteger;
            if (mDNSSendDNSMessage(m, msg, end, InterfaceID, MulticastDNSPort, &addr, UnicastDNSPort))
                {
                err= mStatus_UnknownErr;
                LogMsg("ERROR: mDNSSendMessage");
                }
            }
        }    
    return err;
    }

mDNSexport mStatus uDNS_StartQuery(mDNS *const m, DNSQuestion *const question)
    {
    uDNS_data_t *u = &m->uDNS_data;
    DNSMessage msg;
    mDNSu8 *endPtr;
    NetworkInterfaceInfo *ifi;
    mStatus err = mStatus_NoError;
    mDNSOpaque16 id;
    
    //!!!KRS we should check if the question is already in our acivequestion list
	if (!ValidateDomainName(&question->qname))
		{
		LogMsg("Attempt to start query with invalid qname %##s %s", question->qname.c, DNSTypeName(question->qtype));
		return mStatus_Invalid;
		}
		
    // construct the packet
    mDNSPlatformMemZero(&msg, sizeof(msg));
    if (++u->NextMessageID == 0) u->NextMessageID = 1;	// Don't want to ever use ID zero
    id.b[0] = u->NextMessageID >> 8;
    id.b[1] = u->NextMessageID & 0xFF;
    InitializeDNSMessage(&msg.h, id, QueryFlags);
    endPtr = putQuestion(&msg, msg.data, msg.data + AbsoluteMaxDNSMessageData, &question->qname, question->qtype, question->qclass);
    if (!endPtr) 
        {
        LogMsg("ERROR: Unicast query out of space in packet");
        return mStatus_UnknownErr;
        }
    
    // store the question/id in active question list
    question->uDNS_info.id = id;
    question->uDNS_info.timestamp = m->timenow;
    question->next = u->ActiveQueries;
    question->qnamehash = DomainNameHashValue(&question->qname);    // to do quick domain name comparisons
    u->ActiveQueries = question;
    
    if (question->InterfaceID)
        {
        err = SendMessageToAllServers(m, u, &msg, endPtr, question->InterfaceID);
        }
    else
        {        
        // !!!KRS sending message on all interfaces
        for (ifi = m->HostInterfaces; ifi; ifi = ifi->next)
            {
            if (!ifi->InterfaceActive || !ifi->IPv4Available) continue;
            if (SendMessageToAllServers(m, u, &msg, endPtr, ifi->InterfaceID))
                err = mStatus_UnknownErr;
	    }
        }
    return err;     
    }
        
    
mDNSexport mDNSs32 uDNS_GetNextEventTime(const mDNS *const m)    
    {
    const uDNS_data_t *u = &m->uDNS_data;
    (void)u;				// unused

    return m->timenow + 0x78000000;    
    }