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
Revision 1.7  2004/01/27 20:15:22  cheshire
<rdar://problem/3541288>: Time to prune obsolete code for listening on port 53

Revision 1.6  2004/01/24 23:47:17  cheshire
Use mDNSOpaque16fromIntVal() instead of shifting and masking

Revision 1.5  2004/01/24 04:59:15  cheshire
Fixes so that Posix/Linux, OS9, Windows, and VxWorks targets build again

Revision 1.4  2004/01/24 04:19:26  cheshire
Restore overwritten checkin 1.2

Revision 1.3  2004/01/23 23:23:15  ksekar
Added TCP support for truncated unicast messages.

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

// Private Function Prototypes
// Note:  In general, functions are ordered such that they do not require forward declarations.
// However, prototypes are used where cyclic call graphs exist (e.g. foo calls bar, and bar calls
// foo), or when they aid in the readability of code (e.g. state machine code that is easier read
// top-to-bottom.

mDNSlocal void hndlTruncatedAnswer(DNSQuestion *question,  const mDNSAddr *src, mDNS *m);
mDNSlocal mStatus stopQuery(mDNS *const m, DNSQuestion *const question);

#define ustrcpy(d,s) mDNSPlatformStrCopy(s,d)       // use strcpy(2) param ordering
#define umalloc(x) mDNSPlatformMemAllocate(x)       // short hands for common routines
#define ufree(x) mDNSPlatformMemFree(x)
#define ubzero(x,y) mDNSPlatformMemZero(x,y)

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Name Server List Management
#endif

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
        if (!u->Servers[i].ip.v4.NotAnInteger)
            {
            u->Servers[i].ip.v4.NotAnInteger = dnsAddr->NotAnInteger;
			u->Servers[i].type = mDNSAddrType_IPv4;
            return;
            }
        if (u->Servers[i].ip.v4.NotAnInteger == dnsAddr->NotAnInteger)
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

        if (u->Servers[i].ip.v4.NotAnInteger == dnsAddr->NotAnInteger)
            {
            u->Servers[i].ip.v4.NotAnInteger = 0;
            return;
            }
        }
    if (i == 32) {  LogMsg("ERROR: mDNS_DeregisterDNS - no such DNS registered");  }
    }

mDNSexport void mDNS_DeregisterDNSList(mDNS *const m)
    {
    ubzero(m->uDNS_data.Servers, 32 * sizeof(mDNSAddr));
    }

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Incoming Message Processing
#endif

// InternalResponseHndlr is stored in the DNSQuestion->QuestionCallback field, but uses a different signature,
// taking the raw DNS packet and its context pointer as arguments

typedef void (*InternalResponseHndlr)(DNSMessage *msg, void *context);

mDNSlocal void processAnswers(mDNS *m, uDNS_data_t *u, DNSMessage *const msg, const mDNSu8 *ansptr, const mDNSu8 *const end,
								   const mDNSAddr *src, const mDNSInterfaceID ifid)
    {
    int i = 0;
    LargeCacheRecord rr;
    DNSQuestion *qptr;

	if (msg->h.flags.b[0] & kDNSFlag0_TC)
		{
		// truncate bit set - find matching question
 		for (qptr = u->ActiveQueries; qptr; qptr = qptr->next)
			{
			if (msg->h.id.NotAnInteger == qptr->uDNS_info.id.NotAnInteger)
				{
				hndlTruncatedAnswer(qptr, src, m);
				return;
				}
			}

		for (qptr = u->ActiveInternalQueries; qptr; qptr = qptr->next)
			{
			if (msg->h.id.NotAnInteger == qptr->uDNS_info.id.NotAnInteger)
				{
				hndlTruncatedAnswer(qptr, src, m);
				return;
				}
			}
		LogMsg("Received truncated answer that does not match any active query.  Discarding.");
		return;
		}

    //!!!KRS we should determine up front if it matches an internal or external query - be careful if we have to reset the NextIndex variables!
	//!!!KRS we should call LocateAnswers() here!
	//!!!KRS this can be done more efficiently...

	for (i = 0; i < msg->h.numAnswers; i++)
        {
        ansptr = GetLargeResourceRecord(m, msg, ansptr, end, ifid, kDNSRecordTypePacketAns, &rr);
        if (!ansptr) return;
        for (qptr = u->ActiveQueries; qptr; qptr = qptr->next)
            {
            if (msg->h.id.NotAnInteger == qptr->uDNS_info.id.NotAnInteger &&
                ResourceRecordAnswersQuestion(&rr.r.resrec, qptr) && qptr->QuestionCallback)
                {
                qptr->QuestionCallback(m, qptr, &rr.r.resrec, 1);     //!!!KRS how are we going to detect removes?
                }
            }
        for (qptr = u->ActiveInternalQueries; qptr; qptr = qptr->next)
            {
            if (msg->h.id.NotAnInteger == qptr->uDNS_info.id.NotAnInteger &&
                ResourceRecordAnswersQuestion(&rr.r.resrec, qptr) && qptr->QuestionCallback)
                {
				((InternalResponseHndlr)qptr->QuestionCallback)(msg, qptr->QuestionContext);
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
	    processAnswers(m, u, msg, rrptr, end, srcaddr, InterfaceID);
	    return;
	    }
	}

mDNSlocal void receiveMsg(mDNS *const m, DNSMessage *const msg, const mDNSu8 *const end,
	const mDNSInterfaceID InterfaceID)
	{
	mDNSAddr *sa = NULL, *da = NULL;
	mDNSIPPort sp, dp;
	mDNSu8 ttl = 0;

	sp.NotAnInteger = 0;
	dp.NotAnInteger = 0;
	uDNS_ReceiveMsg(m, msg, end, sa, sp, da, dp, InterfaceID, ttl);
	}

//!!!KRS this should go away
mDNSlocal mDNSAddr *getInitializedDNS(uDNS_data_t *u)
    {
    int i;
    for (i = 0; i < 32; i++)
		if (u->Servers[i].ip.v4.NotAnInteger) return &u->Servers[i];

	LogMsg("ERROR: getInitializedDNS - no initialized servers.");
	return NULL;
    }

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Query Routines
#endif

mDNSexport mDNSBool IsActiveUnicastQuery(DNSQuestion *const question, uDNS_data_t *u)
    {
	DNSQuestion *qptr;

	// see if query is in our lists
	for (qptr = u->ActiveQueries; qptr; qptr = qptr->next)
		if (qptr == question) return mDNStrue;
	for (qptr = u->ActiveInternalQueries; qptr; qptr = qptr->next)
		if (qptr == question) return mDNStrue;
	return mDNSfalse;
	}

mDNSlocal mStatus stopQuery(mDNS *const m, DNSQuestion *const question)
    {
	uDNS_data_t *u = &m->uDNS_data;
	DNSQuestion *qptr, *prev = NULL;
	char errbuf[MAX_ESCAPED_DOMAIN_NAME];
	mDNSBool internal = question->uDNS_info.internal;

	qptr = internal ? u->ActiveInternalQueries : u->ActiveQueries;

	while (qptr)
        {
        if (qptr == question)
            {
            if (prev) prev->next = question->next;
            else
				{
				if (internal) u->ActiveInternalQueries = question->next;
				else u->ActiveQueries = question->next;
				}
			return mStatus_NoError;
            }
        prev = qptr;
		qptr = qptr->next;
        }
    LogMsg("uDNS_StopQuery: no such active query (%s)", ConvertDomainNameToCString(&question->qname, errbuf));
    return mStatus_UnknownErr;
    }
mDNSexport mStatus uDNS_StopQuery(mDNS *const m, DNSQuestion *const question)
	{
	return stopQuery(m, question);
	}

mDNSlocal mStatus constructQueryMsg(DNSMessage *msg, mDNSu8 **endPtr, mDNSOpaque16 id, mDNSOpaque16 flags, DNSQuestion *const question)
	{
	ubzero(msg, sizeof(msg));
	flags.b[0] |= kDNSFlag0_RD;
    InitializeDNSMessage(&msg->h, id, flags);

    *endPtr = putQuestion(msg, msg->data, msg->data + AbsoluteMaxDNSMessageData, &question->qname, question->qtype, question->qclass);
    if (!*endPtr)
        {
        LogMsg("ERROR: Unicast query out of space in packet");
        return mStatus_UnknownErr;
        }
	return mStatus_NoError;
	}

mDNSlocal mStatus startQuery(mDNS *const m, DNSQuestion *const question, mDNSBool internal)
    {
    uDNS_data_t *u = &m->uDNS_data;
    DNSMessage msg;
    mDNSu8 *endPtr;
    NetworkInterfaceInfo *ifi;
    mStatus err = mStatus_NoError, rv;
    mDNSu16 idval;

    //!!!KRS we should check if the question is already in our acivequestion list
	if (!ValidateDomainName(&question->qname))
		{
		LogMsg("Attempt to start query with invalid qname %##s %s", question->qname.c, DNSTypeName(question->qtype));
		return mStatus_Invalid;
		}

    // determine message ID to demux response
    if (!u->NextInternalMessageID) u->NextInternalMessageID = (mDNSu16)~0;   // initialize on first query
	if (!u->NextMessageID) u->NextMessageID = 1;
    if (u->NextInternalMessageID == u->NextMessageID)
        {
        LogMsg("startQuery: Internal and external message IDs have collided - resetting");
        u->NextInternalMessageID = (mDNSu16)~0;
        u->NextMessageID = 1;
        }
	idval = internal ? --u->NextInternalMessageID : ++u->NextMessageID;
	question->next = NULL;
	err = constructQueryMsg(&msg, &endPtr, mDNSOpaque16fromIntVal(idval), QueryFlags, question);
	if (err) return err;

    // store the question/id in active question list
    question->uDNS_info.id.NotAnInteger = mDNSOpaque16fromIntVal(idval).NotAnInteger;
    question->uDNS_info.timestamp = m->timenow;
	question->uDNS_info.internal = internal;
    question->qnamehash = DomainNameHashValue(&question->qname);    // to do quick domain name comparisons
    if (internal)
        {
        question->next = u->ActiveInternalQueries;
        u->ActiveInternalQueries = question;
        }
    else
        {
        question->next = u->ActiveQueries;
        u->ActiveQueries = question;
        }

	if (question->InterfaceID)
        {
		err = mDNSSendDNSMessage(m, &msg, endPtr, question->InterfaceID, getInitializedDNS(u), UnicastDNSPort);
		if (err) LogMsg("ERROR: mDNSSendDNSMessage - %d", err);
		}
	else
		{
		for (ifi = m->HostInterfaces; ifi; ifi = ifi->next)
            {
            if (!ifi->InterfaceActive || !ifi->IPv4Available) continue;
			rv = mDNSSendDNSMessage(m, &msg, endPtr, ifi->InterfaceID, getInitializedDNS(u), UnicastDNSPort);
			if (rv)  { LogMsg("ERROR: mDNSSendDNSMessage - %d", rv);  err = rv; }
    	    }
        }
	return err;
	}

mDNSexport mStatus uDNS_StartQuery(mDNS *const m, DNSQuestion *const question)
    {
    return startQuery(m, question, 0);
    }

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Truncation Handling
#endif

typedef struct
	{
    DNSQuestion  *question;
    DNSMessage reply;
    mDNSu16  replylen;
    int nread;
    mDNS *m;
	} tcpInfo_t;

// issue queries over a conected socket
mDNSlocal void conQueryCallback(int sd, void *context, mDNSBool ConnectionEstablished)
	{
	mDNSOpaque16 flags, id;
	mStatus err = 0;
	char msgbuf[356];  // 96 (hdr) + 256 (domain) + 4 (class/type)
	DNSMessage *msg;
	mDNSu8 *end;
	tcpInfo_t *info = (tcpInfo_t *)context;
	DNSQuestion *question = info->question;
	int n;

	flags.NotAnInteger = 0;
	id.NotAnInteger = 0;

	if (ConnectionEstablished)
		{
		// connection is established - send the message
		msg = (DNSMessage *)&msgbuf;
		err = constructQueryMsg(msg, &end, id, flags, question);
		if (err) { LogMsg("ERROR: conQueryCallback: constructQueryMsg - %d", err);  goto error; }
		err = mDNSSendDNSMessage_tcp(info->m, msg, end, sd);
		if (err) { LogMsg("ERROR: conQueryCallback: mDNSSendDNSMessage_tcp - %d", err);  goto error; }
		return;
		}
	else
		{
		if (!info->nread)
			{
			// read msg len
			n = mDNSPlatformReadTCP(sd, &info->replylen, 2);
			if (n != 2)
				{
				LogMsg("ERROR:conQueryCallback - attempt to read message length failed (read returned %d)", n);
				goto error;
				}
			}
		n = mDNSPlatformReadTCP(sd, ((char *)&info->reply) + info->nread, info->replylen - info->nread);
		if (n < 0) { LogMsg("ERROR: conQueryCallback - read returned %d", n); goto error; }
		info->nread += n;
		if (info->nread == info->replylen)
			{
			// finished reading message
			receiveMsg(info->m, &info->reply, ((mDNSu8 *)&info->reply) + info->replylen, question->InterfaceID);
			mDNSPlatformTCPCloseConnection(sd);
			ufree(info);
			return;
			}
		else return;
		}
	return;

	error:
	mDNSPlatformTCPCloseConnection(sd);
	ufree(info);
	}

mDNSlocal void hndlTruncatedAnswer(DNSQuestion *question, const  mDNSAddr *src, mDNS *m)
	{
	mStatus connectionStatus;
	uDNS_QuestionInfo *info = &question->uDNS_info;
	int sd;
	tcpInfo_t *context;

	context = (tcpInfo_t *)umalloc(sizeof(tcpInfo_t));
	if (!context) { LogMsg("ERROR: hndlTruncatedAnswer - memallocate failed"); return; }
	ubzero(context, sizeof(tcpInfo_t));
	context->question = question;
	context->m = m;

	// zero ID, reset timestamp
	info->id.NotAnInteger = 0;
	info->timestamp = m->timenow;

	connectionStatus = mDNSPlatformTCPConnect(src, UnicastDNSPort, question->InterfaceID, conQueryCallback, context, &sd);
	if (connectionStatus == mStatus_ConnectionEstablished)  // manually invoke callback if connection completes
		{
		conQueryCallback(sd, context, mDNStrue);
		return;
		}
	if (connectionStatus == mStatus_ConnectionPending) return; // callback will be automatically invoked when connection completes
	LogMsg("hndlTruncatedAnswer: connection failed");
	stopQuery(m, question);  //!!!KRS can we really call this here?
	}

mDNSexport mDNSs32 uDNS_GetNextEventTime(const mDNS *const m)
    {
    const uDNS_data_t *u = &m->uDNS_data;
    (void)u;				// unused

    return m->timenow + 0x78000000;
    }
