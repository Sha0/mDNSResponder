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
Revision 1.16  2004/02/12 01:51:45  cheshire
Don't try to send uDNS queries unless we have at least one uDNS server available

Revision 1.15  2004/02/10 03:02:46  cheshire
Fix compiler warning

Revision 1.14  2004/02/06 23:04:19  ksekar
Basic Dynamic Update support via mDNS_Register (dissabled via
UNICAST_REGISTRATION #define)

Revision 1.13  2004/02/03 22:15:01  ksekar
Fixed nameToAddr error check: don't abort state machine on nxdomain error.

Revision 1.12  2004/02/03 19:47:36  ksekar
Added an asyncronous state machine mechanism to uDNS.c, including
calls to find the parent zone for a domain name.  Changes include code
in repository previously dissabled via "#if 0 //incomplete".  Codepath
is currently unused, and will be called to create update records, etc.

Revision 1.11  2004/01/30 02:12:30  ksekar
Changed uDNS_ReceiveMsg() to correctly return void.

Revision 1.10  2004/01/29 02:59:17  ksekar
Unicast DNS: Changed from a resource record oriented question/response
matching to packet based matching.  New callback architecture allows
collections of records in a response to be processed differently
depending on the nature of the request, and allows the same structure
to be used for internal and client-driven queries with different processing needs.

Revision 1.9  2004/01/28 20:20:45  ksekar
Unified ActiveQueries and ActiveInternalQueries lists, using a flag to
demux them.  Check-in includes work-in-progress code, #ifdef'd out.

Revision 1.8  2004/01/28 02:30:07  ksekar
Added default Search Domains to unicast browsing, controlled via
Networking sharing prefs pane.  Stopped sending unicast messages on
every interface.  Fixed unicast resolving via mach-port API.

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

#define ustrcpy(d,s) mDNSPlatformStrCopy(s,d)       // use strcpy(2) param ordering
#define umalloc(x) mDNSPlatformMemAllocate(x)       // short hands for common routines
#define ufree(x) mDNSPlatformMemFree(x)
#define ubzero(x,y) mDNSPlatformMemZero(x,y)

// Asyncronous operation types

typedef enum
	{
	zoneDataResult
	// other async. operation names go here
	} AsyncOpResultType;

typedef struct
	{
    domainname zoneName; 
    mDNSAddr primaryAddr;
    mDNSu16 zoneClass;
	} zoneData_t;

// other async. result struct defs go here

typedef struct
	{
    AsyncOpResultType type;
    union
	    {
        zoneData_t zoneData;
    	// other async result structs go here
        };
	} AsyncOpResult;

typedef void AsyncOpCallback(mStatus err, mDNS *const m, void *info, AsyncOpResult *result); 





// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - General Utility Functions
#endif

mDNSlocal mDNSOpaque16 newMessageID(uDNS_data_t *u)
	{
	// if NextMessageID is 0 (ininitialized) or 0xffff (reserved for TCP packets) reset to 1
	if (!u->NextMessageID || u->NextMessageID == (mDNSu16)~0) u->NextMessageID = 1;
	return mDNSOpaque16fromIntVal(u->NextMessageID++);
	}







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

// simpleResponseHndlr
// This routine examines each answer in a packet.  If a resource record in the answer section answers
// the question, the question callback is invoked.  No context-specific procedures are followed (e.g.
// CNAME records are not canonicalized), and no sections other than the answer section are examined.

mDNSlocal void simpleResponseHndlr(mDNS * const m, DNSMessage *msg, const  mDNSu8 *end, DNSQuestion *question,
								   void *internalContext)
	{
	const mDNSu8 *ptr;
	char errbuf[MAX_ESCAPED_DOMAIN_NAME];
	int i;
	LargeCacheRecord lcr;
	mDNSBool answered = mDNSfalse;
	
	(void)internalContext;  //unused;	

	if (!msg->h.numAnswers)
		{
		LogMsg("simpleResponseHndlr: discarding response to question %s (%d) with no answers",
			   ConvertDomainNameToCString(&question->qname, errbuf), question->qtype);
		return;
		}
	ptr = LocateAnswers(msg, end);
	if (!ptr) goto pkt_error;
	for (i = 0; i < msg->h.numAnswers; i++)
		{
		ptr = GetLargeResourceRecord(m, msg, ptr, end, 0, kDNSRecordTypePacketAns, &lcr);
		if (!ptr) goto pkt_error;
		if (ResourceRecordAnswersQuestion(&lcr.r.resrec, question)) 
			{
			question->QuestionCallback(m, question, &lcr.r.resrec, 1);
			answered = mDNStrue;
			}
		}
	if (!answered) LogMsg("simpleResponseHndlr: received response to question %s (%d) containing"
						  "no records that answered the question", ConvertDomainNameToCString(&question->qname, errbuf),
						  question->qtype);
	return;

	pkt_error:
	LogMsg("ERROR: simpleResponseHndlr - received malformed response to query for %s (%d)",
		   ConvertDomainNameToCString(&question->qname, errbuf), question->qtype);
	}

mDNSexport void uDNS_ReceiveMsg(mDNS *const m, DNSMessage *const msg, const mDNSu8 *const end,
	const mDNSAddr *const srcaddr, const mDNSIPPort srcport, const mDNSAddr *const dstaddr,
	const mDNSIPPort dstport, const mDNSInterfaceID InterfaceID, mDNSu8 ttl)
	{
	DNSQuestion *qptr;
	AuthRecord *rptr;

	const mDNSu8 StdR    = kDNSFlag0_QR_Response | kDNSFlag0_OP_StdQuery;
	const mDNSu8 UpdateR = kDNSFlag0_OP_Update   | kDNSFlag0_QR_Response;
	const mDNSu8 QR_OP   = (mDNSu8)(msg->h.flags.b[0] & kDNSFlag0_QROP_Mask);
	
	// unused
	(void)srcaddr;
	(void)srcport;
	(void)dstaddr;
	(void)dstport;
	(void)ttl;
	(void)InterfaceID;
	
	if (QR_OP == StdR)
		{
		for (qptr = m->uDNS_data.ActiveQueries; qptr; qptr = qptr->next)
			{
			if (qptr->uDNS_info.id.NotAnInteger == msg->h.id.NotAnInteger)
				{
				if (msg->h.flags.b[0] & kDNSFlag0_TC)  hndlTruncatedAnswer(qptr, srcaddr, m);
				else qptr->uDNS_info.responseCallback(m, msg, end, qptr, qptr->uDNS_info.context);
				return;
				}
			}
		}
	if (QR_OP == UpdateR)
		{
		for (rptr = m->uDNS_data.ActiveRegistrations; rptr; rptr = rptr->next)
			{
			if (rptr->uDNS_info.id.NotAnInteger == msg->h.id.NotAnInteger)
				{
				rptr->RecordCallback(m, rptr, msg->h.flags.b[2] >> 4 ? mStatus_UnknownErr : mStatus_NoError);
				return;
				}
			}
		}
	LogMsg("Received unexpected response: ID %d matches no active records", mDNSVal16(msg->h.id));		
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

//!!!KRS this should go away (don't just pick one randomly!)
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
	//!!!KRS We should remove the list check at some point...
	DNSQuestion *qptr;
	mDNSBool inList = mDNSfalse;

	for (qptr = u->ActiveQueries; qptr; qptr = qptr->next)
		if (qptr == question) { inList = mDNStrue;  break; }
	
	if (question->uDNS_info.id.NotAnInteger && inList) return mDNStrue;
	if (!question->uDNS_info.id.NotAnInteger && !inList) return mDNSfalse;

	LogMsg("ERROR: IsActiveUnicastQuery - conflicting results:\n"
		   "%s question id, %s ActiveQueries list",
		   question->uDNS_info.id.NotAnInteger ? "non-zero" : "zero",
		   inList ? "in" : "not in");
	return mDNSfalse;
	}
	
mDNSexport mStatus uDNS_StopQuery(mDNS *const m, DNSQuestion *const question)
	{
	uDNS_data_t *u = &m->uDNS_data;
	DNSQuestion *qptr, *prev = NULL;
	char errbuf[MAX_ESCAPED_DOMAIN_NAME];

	qptr = u->ActiveQueries;
	while (qptr)
        {
        if (qptr == question)
            {
            if (prev) prev->next = question->next;
            else u->ActiveQueries = question->next;
			return mStatus_NoError;
            }
        prev = qptr;
		qptr = qptr->next;
        }
    LogMsg("uDNS_StopQuery: no such active query (%s)", ConvertDomainNameToCString(&question->qname, errbuf));
    return mStatus_UnknownErr;
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
    mStatus err = mStatus_NoError;
	mDNSOpaque16 id;
	
    //!!!KRS we should check if the question is already in our acivequestion list
	if (!ValidateDomainName(&question->qname))
		{
		LogMsg("Attempt to start query with invalid qname %##s %s", question->qname.c, DNSTypeName(question->qtype));
		return mStatus_Invalid;
		}

	id = newMessageID(u);
	
	question->next = NULL;
	err = constructQueryMsg(&msg, &endPtr, id, QueryFlags, question);
	if (err) return err;


    // store the question/id in active question list
    question->uDNS_info.id.NotAnInteger = id.NotAnInteger;
    question->uDNS_info.timestamp = m->timenow;
	question->uDNS_info.internal = internal;
    question->qnamehash = DomainNameHashValue(&question->qname);    // to do quick domain name comparisons
	question->next = u->ActiveQueries;
	u->ActiveQueries = question;

	if (getInitializedDNS(u))
		err = mDNSSendDNSMessage(m, &msg, endPtr, question->InterfaceID, getInitializedDNS(u), UnicastDNSPort);
	if (err) LogMsg("ERROR: mDNSSendDNSMessage - %d", err);
	
	return err;
	}
	
mDNSexport mStatus uDNS_StartQuery(mDNS *const m, DNSQuestion *const question)
    {
    // how the answer is processed is determined by the type of query
	switch(question->qtype)
		{
		default:
			question->uDNS_info.responseCallback = simpleResponseHndlr;
			question->uDNS_info.context = NULL;
		}  
	return startQuery(m, question, 0);
    }

// explicitly set response handler
mDNSlocal mStatus startInternalQuery(DNSQuestion *q, mDNS *m, InternalResponseHndlr callback, void *hndlrContext)
    {    
    q->QuestionContext = hndlrContext;
    q->uDNS_info.responseCallback = callback;
	q->uDNS_info.context = hndlrContext;
    return startQuery(m, q, 1);
    }



// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Domain -> Name Server Conversion
#endif


/* findZoneNS
 *
 * asyncronously find the address of the nameserver for the enclosing zone for a given domain name,
 * i.e. the server to which update requests will be sent for a given name.  Once the address is
 * derived, it will be passed to the callback, along with a context pointer.  If the zone cannot
 * be determined or if an error occurs, an all-zeros address will be passed and a message will be
 * written to the syslog.  Steps for deriving the name are as follows:
 *
 * Query for an SOA record for the required domain.  If we don't get an answer (or an SOA in the Authority
 * section), we strip the leading label from the dname and repeat, until we get an answer.
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
 

// state machine types and structs
//

// state machine states
typedef enum
    {
    init,
    lookupSOA,
	foundZone,
	lookupNS,
	foundNS,
	lookupA,
	complete
    } ntaState;

// state machine actions
typedef enum
    {
    smContinue,  // continue immediately to next state 
    smBreak,     // break until next packet/timeout 
	smError      // terminal error - cleanup and abort
    } smAction;
 
typedef struct
    {
    domainname 	origName;            // name we originally try to convert
    domainname 	*curSOA;             // name we have an outstanding SOA query for
    ntaState  	state;               // determines what we do upon receiving a packet
    mDNS	    *m;
    domainname  zone;                // left-hand-side of SOA record
    mDNSu16     zoneClass; 
    domainname  ns;                  // mname in SOA rdata, verified in confirmNS state
    mDNSv4Addr  addr;                // address of nameserver
    DNSQuestion question;            // storage for any active question
    mDNSBool    questionActive;      // if true, StopQuery() can be called on the question field
    AsyncOpCallback *callback;        // caller specified function to be called upon completion
    void        *callbackInfo;
    } ntaContext;


// function prototypes (for routines that must be used as fn pointers prior to their definitions,
// and allows states to be read top-to-bottom in logical order)
mDNSlocal mStatus startGetZoneData(domainname *name, mDNS *m, AsyncOpCallback callback, void *callbackInfo);
mDNSlocal void getZoneData(mDNS *const m, DNSMessage *msg, const mDNSu8 *end, DNSQuestion *question, void *contextPtr);
mDNSlocal smAction hndlLookupSOA(DNSMessage *msg, const mDNSu8 *end, ntaContext *context);
mDNSlocal void processSOA(ntaContext *context, ResourceRecord *rr);
mDNSlocal smAction confirmNS(DNSMessage *msg, const mDNSu8 *end, ntaContext *context);
mDNSlocal smAction lookupNSAddr(DNSMessage *msg, const mDNSu8 *end, ntaContext *context);

// initialization
mDNSlocal mStatus startGetZoneData(domainname *name, mDNS *m, AsyncOpCallback callback, void *callbackInfo)
    {
    ntaContext *context = umalloc(sizeof(ntaContext));
    if (!context) { LogMsg("ERROR: startNameToAddr - umalloc failed");  return mStatus_NoMemoryErr; }
	ubzero(context, sizeof(ntaContext));
    ustrcpy(context->origName.c, name->c);
    context->state = init;
    context->m = m;
	context->callback = callback;
	context->callbackInfo = callbackInfo;
    getZoneData(m, NULL, NULL, NULL, context);
    return mStatus_NoError;
    }

// state machine entry routine
mDNSlocal void getZoneData(mDNS *const m, DNSMessage *msg, const mDNSu8 *end, DNSQuestion *question, void *contextPtr)
    {
	AsyncOpResult result;
	ntaContext *context = contextPtr;
	smAction action;

    // unused
	(void)m;
	(void)question;
	
	// stop any active question
	if (context->questionActive)
		{
		uDNS_StopQuery(context->m, &context->question);
		context->questionActive = mDNSfalse;
		}

	if (msg && msg->h.flags.b[2] >> 4 && msg->h.flags.b[2] >> 4 != kDNSFlag1_RC_NXDomain)
		{
		// rcode non-zero, non-nxdomain
		LogMsg("ERROR: getZoneData - received response w/ rcode %d", msg->h.flags.b[2] >> 4);
		goto error;
		}
 	
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
		case complete:
			result.type = zoneDataResult;
			result.zoneData.primaryAddr.ip.v4.NotAnInteger = context->addr.NotAnInteger;
			result.zoneData.primaryAddr.type = mDNSAddrType_IPv4;
			ustrcpy(result.zoneData.zoneName.c, context->zone.c);
			result.zoneData.zoneClass = context->zoneClass;
			context->callback(mStatus_NoError, context->m, context->callbackInfo, &result);
			goto cleanup;
		default: { LogMsg("ERROR: getZoneData - bad state %d", context->state); goto error; }			
        }

error:
	if (context && context->callback)
		context->callback(mStatus_UnknownErr, context->m, context->callbackInfo, NULL);	
cleanup:
	if (context->questionActive)
		{
		uDNS_StopQuery(context->m, &context->question);
		context->questionActive = mDNSfalse;
		}		
    if (context) ufree(context);
	}

mDNSlocal smAction hndlLookupSOA(DNSMessage *msg, const mDNSu8 *end, ntaContext *context)
    {
    char errbuf[MAX_ESCAPED_DOMAIN_NAME];
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
			if (!ptr) { LogMsg("ERROR: GetLargeResourceRecord returned NULL");  return smError; }
			if (rr->rrtype == kDNSType_SOA && SameDomainName(context->curSOA, &rr->name))
				{
				processSOA(context, rr);
				return smContinue;
				}
			}		

		// SOA not in answers, check in authority
		for (i = 0; i < msg->h.numAuthorities; i++)
			{
			ptr = GetLargeResourceRecord(context->m, msg, ptr, end, 0, kDNSRecordTypePacketAns, &lcr); ///!!!KRS using type PacketAns for auth
			if (!ptr) { LogMsg("ERROR: GetLargeResourceRecord returned NULL");  return smError; }		 		
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
        LogMsg("ERROR: hndlLookupSOA - recursed to root label of %s without finding SOA", 
                ConvertDomainNameToCString(&context->origName, errbuf));
		return smError;
        }

    ubzero(query, sizeof(DNSQuestion));
    // chop off leading label unless this is our first try
    if (context->state == init)  context->curSOA = &context->origName;
    else                         (void *)context->curSOA = context->curSOA->c + context->curSOA->c[0]+1;
    
    context->state = lookupSOA;
    ustrcpy(query->qname.c, context->curSOA->c);
    query->qtype = kDNSType_SOA;
    query->qclass = kDNSClass_IN;
    err = startInternalQuery(query, context->m, getZoneData, context);
    if (err) { LogMsg("hndlLookupSOA: startInternalQuery returned error %d", err);  return smError;  }
	context->questionActive = mDNStrue;
    return smBreak;     // break from state machine until we receive another packet	
    }

mDNSlocal void processSOA(ntaContext *context, ResourceRecord *rr)
	{
	ustrcpy(context->zone.c, rr->name.c);
	context->zoneClass = rr->rrclass;
	ustrcpy(context->ns.c, rr->rdata->u.soa.mname.c);
	context->state = foundZone;
	}


mDNSlocal smAction confirmNS(DNSMessage *msg, const mDNSu8 *end, ntaContext *context)
	{
	DNSQuestion *query = &context->question;
	mStatus err;
	LargeCacheRecord lcr;
	ResourceRecord *rr = &lcr.r.resrec;
	const mDNSu8 *ptr;
	int i;
    char errbuf[MAX_ESCAPED_DOMAIN_NAME];
		
	if (context->state == foundZone)
		{
		// we've just learned the zone.  confirm that an NS record exists
		ustrcpy(query->qname.c, context->zone.c);
		query->qtype = kDNSType_NS;
		query->qclass = kDNSClass_IN;
		err = startInternalQuery(query, context->m, getZoneData, context);
		if (err) { LogMsg("confirmNS: startInternalQuery returned error %d", err);  return smError; }
		context->questionActive = mDNStrue;	   
		context->state = lookupNS;
		return smBreak;  // break from SM until we receive another packet
		}
	else if (context->state == lookupNS)
		{
		ptr = LocateAnswers(msg, end);
		for (i = 0; i < msg->h.numAnswers; i++)
			{
			ptr = GetLargeResourceRecord(context->m, msg, ptr, end, 0, kDNSRecordTypePacketAns, &lcr);
			if (!ptr) { LogMsg("ERROR: GetLargeResourceRecord returned NULL");  return smError; }
			if (rr->rrtype == kDNSType_NS &&
				SameDomainName(&context->zone, &rr->name) && SameDomainName(&context->ns, &rr->rdata->u.name))
				{
				context->state = foundNS;
				return smContinue;  // next routine will examine additionals section of A record				
				}				
			}
		LogMsg("ERROR: could not confirm existence of NS record %s", ConvertDomainNameToCString(&context->zone, errbuf));
		return smError;
		}
	else { LogMsg("ERROR: confirmNS - bad state %d", context->state); return smError; }
	}
		
	
mDNSlocal smAction lookupNSAddr(DNSMessage *msg, const mDNSu8 *end, ntaContext *context)
	{
	const mDNSu8 *ptr;
	int i;
	LargeCacheRecord lcr;
	ResourceRecord *rr = &lcr.r.resrec;
	DNSQuestion *query = &context->question;
	mStatus err;
	
	if (context->state == foundNS)
		{
		// we just found the NS record - look for the corresponding A record in the Additionals section
		ptr = LocateAdditionals(msg, end);
		if (!ptr)
			{
			if (msg->h.numAdditionals)
				LogMsg("ERROR: lookupNSAddr - LocateAdditionals returned NULL, expected %d additionals", msg->h.numAdditionals);
			// error case: continue with query			
			}
		else
			{
			for (i = 0; i < msg->h.numAdditionals; i++)
				{
				ptr = GetLargeResourceRecord(context->m, msg, ptr, end, 0, kDNSRecordTypePacketAns, &lcr);
				if (!ptr) { LogMsg("ERROR: lookupNSAddr - GetLargeResourceRecord returned NULL"); break; }
				if (rr->rrtype == kDNSType_A && SameDomainName(&context->ns, &rr->name))
					{
					context->addr.NotAnInteger = rr->rdata->u.ip.NotAnInteger;
					context->state = complete;
					return smContinue;
					}
				}
			}
		// no A record in Additionals - query the server
		ustrcpy(query->qname.c, context->ns.c);
		query->qtype = kDNSType_A;
		query->qclass = kDNSClass_IN;
		err = startInternalQuery(query, context->m, getZoneData, context);
		if (err) { LogMsg("confirmNS: startInternalQuery returned error %d", err);  return smError; }
		context->questionActive = mDNStrue;
		context->state = lookupA;
		return smBreak;
		}
	else if (context->state == lookupA)
		{
		ptr = LocateAnswers(msg, end);
		if (!ptr) { LogMsg("ERROR: lookupNSAddr: LocateAnswers returned NULL");  return smError; }
		for (i = 0; i < msg->h.numAnswers; i++)
			{
			if (rr->rrtype == kDNSType_A && SameDomainName(&context->ns, &rr->name))
				{
				context->addr.NotAnInteger = rr->rdata->u.ip.NotAnInteger;
				context->state = complete;
				return smContinue;
				}
			}		
		LogMsg("ERROR: lookupNSAddr: Address record not found in answer section");
		return smError;
		}
	else { LogMsg("ERROR: lookupNSAddr - bad state %d", context->state); return smError; }
	}

		

//#endif //0 (incomplete)

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
	id.NotAnInteger = (mDNSu16)~0;

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

	info->id.NotAnInteger = (mDNSu16)~0;  // all 1's indicates TCP queries
	info->timestamp = m->timenow;         // reset timestamp

	connectionStatus = mDNSPlatformTCPConnect(src, UnicastDNSPort, question->InterfaceID, conQueryCallback, context, &sd);
	if (connectionStatus == mStatus_ConnectionEstablished)  // manually invoke callback if connection completes
		{
		conQueryCallback(sd, context, mDNStrue);
		return;
		}
	if (connectionStatus == mStatus_ConnectionPending) return; // callback will be automatically invoked when connection completes
	LogMsg("hndlTruncatedAnswer: connection failed");
	uDNS_StopQuery(m, question);  //!!!KRS can we really call this here?
	}


// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Dynamic Updates
#endif

mDNSlocal void sendUpdate(mStatus err, mDNS *const m, void *authPtr, AsyncOpResult *result)
	{
	AuthRecord *newRR = authPtr;
	DNSMessage msg;
	mDNSu8 *ptr = msg.data;
	mDNSu8 *end = (mDNSu8 *)&msg + sizeof(DNSMessage);
	AuthRecord prereq;
	zoneData_t *zoneData = &result->zoneData;
	uDNS_data_t *u = &m->uDNS_data;
	mDNSOpaque16 id;

	if (err) goto error;
	if (result->type != zoneDataResult)
		{
		LogMsg("ERROR: buildUpdatePacket passed incorrect result type %d", result->type);
		goto error;
		}

	id = newMessageID(u);
	InitializeDNSMessage(&msg.h, id, UpdateReqFlags);
	newRR->uDNS_info.id.NotAnInteger = id.NotAnInteger;
	
	// set zone
	ptr = putZone(&msg, ptr, end, &zoneData->zoneName, mDNSOpaque16fromIntVal(zoneData->zoneClass));
	if (!ptr) goto error;
	
	if (newRR->resrec.RecordType != kDNSRecordTypeShared)
		{
		// build prereq (no RR w/ same name/type may exist in zone)
		ubzero(&prereq, sizeof(AuthRecord));
		ustrcpy(prereq.resrec.name.c, newRR->resrec.name.c);
		prereq.resrec.rrtype = newRR->resrec.rrtype;
		prereq.resrec.rrclass = kDNSClass_NONE;
		ptr = putEmptyResourceRecord(&msg, ptr, end, &msg.h.numPrereqs, &prereq);
		if (!ptr) goto error;
		}

	// add new record, send the message, link into list
	if (newRR->resrec.rrclass != zoneData->zoneClass)
		{
		LogMsg("ERROR: New resource record's class (%d) does not match zone class (%d)",
			   newRR->resrec.rrclass, zoneData->zoneClass);
		goto error;
		}
	ptr = PutResourceRecord(&msg, ptr, &msg.h.numUpdates, &newRR->resrec);
	if (!ptr) goto error;

	err = mDNSSendDNSMessage(m, &msg, ptr, 0, &zoneData->primaryAddr, UnicastDNSPort);
	if (err) { LogMsg("ERROR: mDNSSendDNSMessage - %d", err); goto error; }

	// cache zone data, link into list
	ustrcpy(newRR->uDNS_info.zone.c, zoneData->zoneName.c);
    newRR->uDNS_info.ns.type = mDNSAddrType_IPv4;
	newRR->uDNS_info.ns.ip.v4.NotAnInteger = zoneData->primaryAddr.ip.v4.NotAnInteger;
	newRR->next = u->ActiveRegistrations;
	u->ActiveRegistrations = newRR;

	return;
		
error:
	newRR->RecordCallback(m, newRR, err);
	// NOTE: not safe to touch any client structures here
	}



extern mStatus uDNS_Register(mDNS *const m, AuthRecord *const rr)
	{
	domainname *target = GetRRDomainNameTarget(&rr->resrec);	

	if (rr->HostTarget)
		{
		if (target) target->c[0] = 0;
		//!!!KRS do we need to reassign target here?
		if (target && !SameDomainName(target, &m->hostname))
			{
			AssignDomainName(*target, m->hostname);
			SetNewRData(&rr->resrec, mDNSNULL, 0);
			}
		}
	else
		{
		rr->resrec.rdlength   = GetRDLength(&rr->resrec, mDNSfalse);
		rr->resrec.rdestimate = GetRDLength(&rr->resrec, mDNStrue);
		}
		
	if (!ValidateDomainName(&rr->resrec.name))
		{
		LogMsg("Attempt to register record with invalid name: %s", GetRRDisplayString(m, rr));
		return mStatus_Invalid;
		}

	// Don't do this until *after* we've set rr->resrec.rdlength
	if (!ValidateRData(rr->resrec.rrtype, rr->resrec.rdlength, rr->resrec.rdata))
		{ LogMsg("Attempt to register record with invalid rdata: %s", GetRRDisplayString(m, rr));
		return mStatus_Invalid;
		}

	rr->resrec.namehash   = DomainNameHashValue(&rr->resrec.name);
	rr->resrec.rdatahash  = RDataHashValue(rr->resrec.rdlength, &rr->resrec.rdata->u);
	rr->resrec.rdnamehash = target ? DomainNameHashValue(target) : 0;

	rr->resrec.RecordType = kDNSRecordTypeUnique;
	return startGetZoneData(&rr->resrec.name, m, sendUpdate, rr);
	}

extern mStatus uDNS_Deregister(mDNS *const m, AuthRecord *const rr)
	{
	AuthRecord *rptr, *prev = NULL;
	uDNS_data_t *u = &m->uDNS_data;
	DNSMessage msg;
	mDNSu8 *ptr = msg.data;
	mDNSu8 *end = (mDNSu8 *)&msg + sizeof(DNSMessage);
	mDNSu16 origclass;
	mStatus err;
	
	for (rptr = u->ActiveRegistrations; rptr; rptr = rptr->next)
		{
		if (rptr == rr)
			{
			if (prev) prev->next = rptr->next;
			else u->ActiveRegistrations = rptr->next;
			rptr->next = NULL;
			break;			
			}
		prev = rptr;
		}
	if (!rptr)
		{
		LogMsg("ERROR: uDNS_Deregister - no such active record");
		return mStatus_UnknownErr;
		}
	
	InitializeDNSMessage(&msg.h, rr->uDNS_info.id, UpdateReqFlags);

	// put zone
	ptr = putZone(&msg, ptr, end, &rr->uDNS_info.zone, mDNSOpaque16fromIntVal(rr->resrec.rrtype));
	if (!ptr) goto error;

	// prereq: record must exist (put record in prereq section w/ TTL 0)
	ptr = PutResourceRecordTTL(&msg, ptr, &msg.h.numPrereqs, &rr->resrec, 0);
	if (!ptr) goto error;

	// deletion: specify record w/ TTL 0, class NONE
	origclass = rr->resrec.rrclass;
	rr->resrec.rrclass = kDNSClass_NONE;
	ptr = PutResourceRecordTTL(&msg, ptr, &msg.h.numUpdates, &rr->resrec, 0);
	if (!ptr) goto error;
	rr->resrec.rrclass = origclass;

	err = mDNSSendDNSMessage(m, &msg, ptr, 0, &rr->uDNS_info.ns, UnicastDNSPort);
	if (err) { LogMsg("ERROR: mDNSSendDNSMessage - %d", err); goto error; }

	return mStatus_NoError;

	error:
	LogMsg("ERROR: uDNS_Deregister");
	return mStatus_UnknownErr;
	}

	

mDNSexport mDNSs32 uDNS_GetNextEventTime(const mDNS *const m)
    {
    const uDNS_data_t *u = &m->uDNS_data;
    (void)u;				// unused

    return m->timenow + 0x78000000;
	}
