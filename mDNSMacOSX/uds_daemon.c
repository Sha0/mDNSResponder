/*
 * Copyright (c) 2003 Apple Computer, Inc. All rights reserved.
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


#include "mDNSClientAPI.h"
#include "mDNSMacOSX.h"
#include "uds_dnssd.h"
#include "dnssd_ipc.h"
#include <fcntl.h>


typedef enum
    {
    t_morecoming,
    t_complete,
    t_error,
    t_terminated
    } transfer_state;

typedef void (*req_termination_fn)(void *);

//!!!KRS this should be a hashtable, not a list!
typedef struct registration_entry
    {
    int key;
    ResourceRecord *rr;
    struct registration_entry *next;
    } registration_entry;


typedef struct request_state
    {
    CFRunLoopSourceRef rls;
    CFSocketRef sr;		
    int sd;			// we could use CFSocketGetNative() to get the descriptor from the CFSocket,
                                // but maintaining/using a separate descriptor eases portability to other platforms
    transfer_state ts;
    uint32_t hdr_bytes;		// bytes of header already read
    ipc_msg_hdr hdr;
    uint32_t data_bytes;	// bytes of message data already read
    char *msgbuf;		// pointer to data storage
    int bufsize;		// size of data storage
    struct reply_state *replies;  // corresponding (active) reply list
    void *termination_context;
    req_termination_fn terminate;
    void *client_context;	// don't touch this - pointer only valid in client's addr space
    registration_entry *reg_entries;  // non-null only for connection-oriented registrations
    struct request_state *next;
    } request_state;

typedef struct reply_state
    {
    transfer_state ts;
    int sd;
    uint32_t nwriten;
    uint32_t len;
    DNSServiceDiscoveryFlags *fptr; // pointer to flags field in message buffer - flags may need to be modified
                                    // after message is fully formatted  !!!KRS why not just put flags in ipc_msg_hdr?	
    char *msgbuf;		// header and message data together
    struct request_state *request;  // the request that this answers
    struct reply_state *next;   // if there are multiple unsent replies
    } reply_state;


// domain enumeration calls require 2 mDNSCore calls (for default/all domains), so we need separate interconnected
// structures to handle callbacks
typedef struct
    {
    DNSQuestion question;
    mDNS_DomainType type;
    request_state *rstate;
    } domain_enum_t;

typedef struct
    {
    domain_enum_t *all;
    domain_enum_t *def;
    request_state *rstate;
    } enum_termination_t;


typedef struct
    {
    request_state *rstate;
    client_context_t client_context;
    } regrecord_callback_context;

// globals
static int listenfd = -1;  
request_state *all_requests = NULL;
 
// private function prototypes
static void connect_callback(CFSocketRef sr, CFSocketCallBackType t, CFDataRef dr, const void *c, void *i);
static int read_msg(request_state *rs);
static int send_msg(reply_state *rs);
static void abort_request(request_state *rs);
static void request_callback(CFSocketRef sr, CFSocketCallBackType t, CFDataRef dr, const void *c, void *i);
static void handle_resolve_request(request_state *rstate);
static void question_result_callback(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer);
//static void question_callback(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer);
static void question_termination_callback(void *context);
static void handle_browse_request(request_state *request);
static void browse_termination_callback(void *context);
static void browse_result_callback(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer);
static void handle_regservice_request(request_state *request);
static void regservice_termination_callback(void *context);
static void process_service_registration(ServiceRecordSet *const srs);
static void regservice_callback(mDNS *const m, ServiceRecordSet *const srs, mStatus result);
static ipc_msg_hdr *gen_rr_response(domainname *servicename, DNSServiceDiscoveryFlags **fptr);
static void append_reply(request_state *req, reply_state *rep);
static int BuildDomainNameFromStrings(domainname *srv, char *name, char *regtype, char *domain);
static void enum_termination_callback(void *context);
static void enum_result_callback(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer);
static void handle_query_request(request_state *rstate);
static void do_question(request_state *rstate, domainname *name, u_int32_t ifi, uint16_t rrtype, int16_t rrclass);
static reply_state *format_enumeration_reply(request_state *rstate, char *domain,                                             	DNSServiceDiscoveryFlags flags, u_int32_t ifi, DNSServiceReplyErrorType err);
static void handle_enum_request(request_state *rstate);
static void handle_regrecord_request(request_state *rstate);
static void regrecord_callback(mDNS *const m, ResourceRecord *const rr, mStatus result);
static void connected_registration_termination(void *context);
static void reconfirm_record_callback(mDNS *const m, ResourceRecord *const rr, mStatus result);
static void handle_reconfirm_request(request_state *rstate);
static ResourceRecord *read_rr_from_ipc_msg(char *msgbuf, int ttl);
static void handle_removerecord_request(request_state *rstate);
static void reset_connected_rstate(request_state *rstate);


// initialization, setup/teardown functions

int udsserver_init(void)  
    {
    mode_t mask;
    struct sockaddr_un laddr;
	//umask(0777);

    if ((listenfd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) 
            goto error;
    unlink(MDNS_UDS_SERVERPATH);  //OK if this fails
    bzero(&laddr, sizeof(laddr));
    laddr.sun_family = AF_LOCAL;
    strcpy(laddr.sun_path, MDNS_UDS_SERVERPATH);
    mask = umask(0);
    if (bind(listenfd, (struct sockaddr *)&laddr, sizeof(laddr)) < 0)
        goto error;
    umask(mask);

    if (fcntl(listenfd, F_SETFL, O_NONBLOCK) < 0)
        {
        perror("ERROR: could not set listen socket to non-blocking mode");
        return -1;
        }
    listen(listenfd, LISTENQ);
    return 0;
	
error:
    perror("ERROR: ");
    return -1;
    }

int udsserver_exit(void)
    {
    close(listenfd);
    unlink(MDNS_UDS_SERVERPATH);
    //!!!KRS we should run through the list and close all open descriptors
    return 0;
    }


// add the named socket as a runloop source
int udsserver_add_rl_source(void)
    {
    CFSocketContext context = 	{ 0, NULL, NULL, NULL, NULL 	};
    CFSocketRef sr = CFSocketCreateWithNative(kCFAllocatorDefault, listenfd, kCFSocketReadCallBack, connect_callback, &context);
    if (!sr)
        {
        debugf("ERROR: udsserver_add_rl_source - CFSocketCreateWithNative");
        return -1;
    	}
    CFRunLoopSourceRef rls = CFSocketCreateRunLoopSource(kCFAllocatorDefault, sr, 0);
  
    if (!rls)
    	{
        debugf("ERROR: udsserver_add_rl_source - CFSocketCreateRunLoopSource");
        return -1;
    	}
    CFRunLoopAddSource(CFRunLoopGetCurrent(), rls, kCFRunLoopDefaultMode);
    //CFRelease(rls);
    return 0;
    }

mDNSs32 udsserver_idle(mDNSs32 nextevent)
    {
    request_state *req = all_requests;
    reply_state *fptr;
    transfer_state result;
    mDNSs32 now = mDNSPlatformTimeNow();


    while(req)
        {
        if (req->ts == t_complete || (req->ts == t_morecoming && req->replies)) //!!!KRS clean this up...
            {
            while(req->replies)
                {
                if (req->replies->next) *req->replies->fptr |= kDNSServiceDiscoveryFlagsMoreComing;
                else *req->replies->fptr |= kDNSServiceDiscoveryFlagsFinished;
                result = send_msg(req->replies);
                if (result == t_complete)
                    {
                    fptr = req->replies;
                    req->replies = req->replies->next;
                    free(fptr);
                    fprintf(stderr, "***completed\n");
                    }
                else if (result == t_terminated)
                    {
                    abort_request(req);
                    break;
                    }
                else if (result == t_morecoming)	   		// client's queues are full, move to next
                    {
                    if (nextevent - now > mDNSPlatformOneSecond)
                        nextevent = now + mDNSPlatformOneSecond;
                    break;					// start where we left off in a second
                    }
                else if (result == t_error) assert(0);  //!!!KRS do something better here!
                }
            }
        req = req->next;
        }
    return nextevent;
    }

// accept a connection on the named socket, adding the new descriptor to the runloop
static void connect_callback(CFSocketRef s, CFSocketCallBackType t, CFDataRef dr, const void *c, void *i)
    {
    int sd, clilen, optval;
    struct sockaddr_un cliaddr;
    CFSocketContext context = 	{ 0, NULL, NULL, NULL, NULL 	};
    request_state *rstate;
    
    (void)s;
    (void)t;
    (void)dr;
    (void)c;
    (void)i;

    assert(listenfd > 0);	//daemon must have already called init()
    assert(listenfd == CFSocketGetNative(s));  //!!!KRS we can remove the global listenfd...
    clilen = sizeof(cliaddr);
    sd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);

    if (sd < 0)
        {
        if (errno == EWOULDBLOCK) return; 
        perror("ERROR: accept");
        //!!!KRS we need to terminate here!  maybe not...
        return;
    	}
#ifdef UDSDEBUG
    fprintf(stderr, "***UDS: Established connection with client on sd %d, path %s\n", sd, cliaddr.sun_path);
#endif
    optval = 1;
    if (setsockopt(sd, SOL_SOCKET, SO_NOSIGPIPE, &optval, sizeof(optval)) < 0)
    	{
        perror("ERROR: setsockopt - SOL_NOSIGPIPE");  //!!! do something better here...
        return;
    	}

    if (fcntl(listenfd, F_SETFL, O_NONBLOCK) < 0)
    	{
        perror("ERROR: could not set connected socket to non-blocking mode");
        return;  //!!! do something better here...
    	}

    // allocate a request_state struct that will live with the socket
    rstate = malloc(sizeof(request_state));
    if (!rstate)
    	{
        perror("ERROR: malloc");
        return;         		//!!!KRS we should exit here!
    	}
    bzero(rstate, sizeof(request_state));
    rstate->ts = t_morecoming;
    rstate->sd = sd;
    context.info = rstate;

    //now create CFSocket wrapper and add to run loop
    rstate->sr = CFSocketCreateWithNative(kCFAllocatorDefault, sd, kCFSocketReadCallBack, request_callback, &context);
    if (!rstate->sr)
    	{
        debugf("ERROR: connect_callback - CFSocketCreateWithNative");
        free(rstate);
        return;
    	}
    rstate->rls = CFSocketCreateRunLoopSource(kCFAllocatorDefault, rstate->sr, 0);
    fprintf(stderr, "***UDS: adding rls %p\n", rstate->rls);
    assert(CFSocketGetNative(rstate->sr) == sd);
    if (!rstate->rls)
    	{
        debugf("ERROR: connect_callback - CFSocketCreateRunLoopSource");
        CFRelease(rstate->sr);
        free(rstate);
        return;
    	}
    CFRunLoopAddSource(CFRunLoopGetCurrent(), rstate->rls, kCFRunLoopDefaultMode);
    assert(CFRunLoopContainsSource(CFRunLoopGetCurrent(), rstate->rls, kCFRunLoopDefaultMode));
    rstate->next = all_requests;
    all_requests = rstate;
    }


// main client request handling routine.  reads request and calls the appropriate request-specific
// handler
static void request_callback(CFSocketRef sr, CFSocketCallBackType t, CFDataRef dr, const void *context, void *info)
    {
    request_state *rstate = info;
    transfer_state result;
    
    (void)sr;
    (void)t;
    (void)dr;
    (void)context;

    int native = CFSocketGetNative(sr);
    assert(native == rstate->sd);

    result = read_msg(rstate);
    if (result == t_morecoming)
    	{
        return;
    	}
    if (result == t_terminated)
    	{
        abort_request(rstate);
        return;
    	}
    if (result == t_error)
    	{
        //!!! need a xf call to cleanup and exit!
        return;
    	}
#ifdef UDSDEBUG
    fprintf(stderr, "done.\n");
#endif

    if (rstate->hdr.version != VERSION)
    {
        fprintf(stderr, "ERROR: client incompatible with daemon (client version = %d, "
                "daemon version = %d)\n", rstate->hdr.version, VERSION);
        return;
    }

    switch(rstate->hdr.op.request_op)
    	{
        case resolve_request: handle_resolve_request(rstate); break;
        case query_request: handle_query_request(rstate);  break;
        case browse_request: handle_browse_request(rstate); break;
        case reg_service_request: handle_regservice_request(rstate);  break;
        case enumeration_request: handle_enum_request(rstate); break;
        case reg_record_request: handle_regrecord_request(rstate);  break;
        case remove_record_request: handle_removerecord_request(rstate); break;
        case reconfirm_record_request: handle_reconfirm_request(rstate); break;
        default:
            debugf("ERROR: udsserver_recv_request - unsupported request type: %d", rstate->hdr.op.request_op);
    	}
    }

// mDNS operation functions.  Each operation has 3 associated functions - a request handler that parses
// the client's request and makes the appropriate mDNSCore call, a result handler (passed as a callback
// to the mDNSCore routine) that sends results back to the client, and a termination routine that aborts
// the mDNSCore operation if the client dies or closes its socket.


// query and resolve calls have separate request handlers that parse the arguments from the client and
// massage the name parameters appropriately, but the rest of the operations (making the query call,
// delivering the result to the client, and termination) are identical.

static void handle_query_request(request_state *rstate)
    {
    DNSServiceDiscoveryFlags flags;
    u_int32_t interfaceIndex;
    char name[256];
    u_int16_t rrtype, rrclass;
    char *ptr;
    domainname dname;
    
    assert(rstate->ts == t_complete);
    ptr = rstate->msgbuf;
    assert(ptr);
    flags = get_flags(&ptr);
    interfaceIndex = get_long(&ptr);
    get_string(&ptr, name, 256);
    rrtype = get_short(&ptr);
    rrclass = get_short(&ptr);
    MakeDomainNameFromDNSNameString (&dname, name);
    do_question(rstate, &dname, interfaceIndex, rrtype, rrclass);
    }

static void handle_resolve_request(request_state *rstate)
    {
    DNSServiceDiscoveryFlags flags;
    u_int32_t interfaceIndex;
    char name[256], regtype[256], domain[256];  //!!!KRS re-examine these buffer sizes
    u_int16_t rrtype;
    char *ptr;  // message data pointer
    domainname fqdn;
    assert(rstate->ts == t_complete);

    // extract the data from the message
    ptr = rstate->msgbuf;
    assert(ptr);
    flags = get_flags(&ptr);
    interfaceIndex = get_long(&ptr);
    get_string(&ptr, name, 256);
    get_string(&ptr, regtype, 256);
    get_string(&ptr, domain, 256);
    rrtype = get_short(&ptr);
    // free memory in rstate since we don't need it anymore
    free(rstate->msgbuf);
    rstate->msgbuf = NULL;

    if (BuildDomainNameFromStrings(&fqdn, name, regtype, domain) < 0)
    	{
        fprintf(stderr, "ERROR: invalid label(s)\n");
        return;
    	}
    do_question(rstate, &fqdn, interfaceIndex, rrtype, kDNSClass_IN);
    }

// common query issuing routine for resolve and query requests
static void do_question(request_state *rstate, domainname *name, u_int32_t ifi, uint16_t rrtype, int16_t rrclass)
    {
    DNSQuestion *q;

    (void)ifi;  //!!!KRS
    q = malloc(sizeof(DNSQuestion));
    if (!q)
    	{
        perror("ERROR: do_question - malloc");
        return;  //!!!KRS do something more meaningful here...
    	}
    bzero(q, sizeof(DNSQuestion));	

    q->QuestionContext = rstate;
    q->QuestionCallback = question_result_callback;
    memcpy(&q->qname, name, MAX_DOMAIN_NAME);
    q->qtype = rrtype;
    q->qclass = rrclass;
    q->InterfaceID = NULL;  // !!!KRS fix this

    rstate->termination_context = q;
    rstate->terminate = question_termination_callback;

    if (mDNS_StartQuery(&mDNSStorage, q) != mStatus_NoError)
    	{
        fprintf(stderr, "ERROR: mDNS_StartQuery");
        //!!!KRS
    	}
    }		

// what gets called when a resolve is completed and we need to send the data back to the client
static void question_result_callback(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer)
    {
    ipc_msg_hdr *hdr;
    char *data;
    char name[256];
    request_state *req;
    reply_state *rep;
    int len;
    DNSServiceDiscoveryFlags *fptr;

    (void)m;
    //mDNS_StopQuery(m, question);
    req = question->QuestionContext;
    
    // calculate reply data length
    len = sizeof(DNSServiceDiscoveryFlags);
    len += 2 * sizeof(u_int32_t);  // if index + ttl
    len += sizeof(DNSServiceReplyErrorType);
    len += 3 * sizeof(u_int16_t); // type, class, rdlen
    len += answer->rdata->RDLength;
    ConvertDomainNameToCString(&answer->name, name);
    len += strlen(name) + 1;
    
    hdr = create_hdr(query_reply, &len, &data);
    if (!hdr) return;					//!!!KRS do something more here!
    fptr = (DNSServiceDiscoveryFlags *)data;
    put_flags(0, &data);
    put_long(0, &data);  //!!!for ifindex - FIX THIS!
    put_error_code(0, &data);
    put_string(name, &data);
    put_short(answer->rrtype, &data);
    put_short(answer->rrclass, &data);
    put_short(answer->rdata->RDLength, &data);
    put_rdata(answer->rdata->RDLength, (char *)&answer->rdata->u, &data);
    put_long(answer->rrremainingttl, &data);

    rep = malloc(sizeof(reply_state));
    if (!rep)
    	{
        perror("ERROR: malloc");
        return;		//!!!KRS should exit here
    	}
    bzero(rep, sizeof(reply_state));
    rep->ts = t_morecoming;
    rep->sd = req->sd;
    rep->len = len;
    rep->msgbuf = (char *)hdr;
    rep->fptr = fptr;
    rep->request = req;
    // add reply to list in request struct
    append_reply(req, rep);
    return;
    }

static void question_termination_callback(void *context)
    {
    DNSQuestion *q = context;

#ifdef UDSDEBUG
    fprintf(stderr, "***UDS: Ending query\n");
#endif
    mDNS_StopQuery(&mDNSStorage,q);  //!!!KRS error-check here
    free(q);
    }

// browse
static void handle_browse_request(request_state *request)
    {
    DNSServiceDiscoveryFlags flags;
    u_int32_t interfaceIndex;
    char regtype[256], domain[256];
    DNSQuestion *q;
    domainname typedn, domdn;
    char *ptr;

    assert(request->ts == t_complete);
    q = malloc(sizeof(DNSQuestion));
    if (!q)
    	{
        perror("ERROR: handle_browse_request - malloc");
        return;  //!!!KRS something more here...
    	}
    bzero(q, sizeof(DNSQuestion));

    // extract data from message
    ptr = request->msgbuf;
    flags = get_flags(&ptr);
    interfaceIndex = get_long(&ptr);
    get_string(&ptr, regtype, 256);
    get_string(&ptr, domain, 256);
    free(request->msgbuf);
    request->msgbuf = NULL;

    q->QuestionContext = request;
    q->QuestionCallback = browse_result_callback;
    if (!MakeDomainNameFromDNSNameString(&typedn, regtype) ||
        !MakeDomainNameFromDNSNameString(&domdn, domain))
    	{
        fprintf(stderr, "ERROR: invalid label(s)\n");
        return;  //!!!KRS
    	}
    request->termination_context = q;
    request->terminate = browse_termination_callback;
    if (mDNS_StartBrowse(&mDNSStorage, q, &typedn, &domdn, 0, browse_result_callback, request) != mStatus_NoError)
    	{
        fprintf(stderr, "ERROR: mDNS_Start_Browse\n");
        //!!!KRS
    	}
    }

static void browse_result_callback(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer)
    {

    ipc_msg_hdr *hdr;
    request_state *req;
    reply_state *rep;
    DNSServiceDiscoveryFlags *fptr;
    
    (void)m;
    req = question->QuestionContext;

    hdr = gen_rr_response(&answer->rdata->u.name, &fptr);
    if (!hdr) goto error;

    rep = malloc(sizeof(reply_state));
    if (!rep)
    	{
        perror("ERROR: malloc");
        goto error;
    	}
    bzero(rep, sizeof(reply_state));
    rep->ts = t_morecoming;
    rep->sd = req->sd;
    rep->len = hdr->len;
    rep->msgbuf = (char *)hdr;
    rep->fptr = fptr;
    rep->request = req;
    // add reply to list in request struct
    append_reply(req, rep);
    return;

error:
        assert(0);  //!!!KRS
    }

static void browse_termination_callback(void *context)
    {
    DNSQuestion *q = context;

#ifdef UDSDEBUG
    fprintf(stderr, "***UDS: Ending browse\n");
#endif
    mDNS_StopBrowse(&mDNSStorage, q);  //!!!KRS error-check here
    free(q);

    }

// service registration
static void handle_regservice_request(request_state *request)
    {
    DNSServiceDiscoveryFlags flags;
    u_int32_t interfaceIndex, txtTTL;
    char name[256], regtype[256], domain[256];
    u_int16_t txtlen;
    mDNSIPPort port;
    void *txtdata;
    char *ptr;
    domainlabel n;
    domainname t, d;
    domainname srv;
    ServiceRecordSet *srs;
    mDNS *mDNSdata;

    mDNSdata = &mDNSStorage;

    assert(request->ts == t_complete);

    // extract data from message
    ptr = request->msgbuf;
    flags = get_flags(&ptr);
    interfaceIndex = get_long(&ptr);
    get_string(&ptr, name, 256);
    get_string(&ptr, regtype, 256);
    get_string(&ptr, domain, 256);
    port.NotAnInteger = get_short(&ptr);
    txtlen = get_short(&ptr);
    txtdata = get_rdata(&ptr, txtlen);
    txtTTL = get_long(&ptr);

    fprintf(stderr, "Args: flags=%d, name=%s, type=%s, domain=%s\n", flags, name, regtype, domain);

    
    if (!name[0]) n = mDNSdata->nicelabel;
    else if (!MakeDomainLabelFromLiteralString(&n, name)) goto error;
    if (!regtype[0] || !MakeDomainNameFromDNSNameString(&t, regtype)) goto error;
    if (!MakeDomainNameFromDNSNameString(&d, *domain ? domain : "local.")) goto error;
    if (!ConstructServiceName(&srv, &n, &t, &d)) goto error;

    srs = malloc(sizeof(ServiceRecordSet));
    if (!srs)
    	{
        perror("ERROR: malloc");
        goto error;
    	}
    request->termination_context = srs;
    request->terminate = regservice_termination_callback;
    

    mDNS_RegisterService(mDNSdata, srs, &n, &t, &d, NULL, port, txtdata, txtlen, 0, NULL, 0, regservice_callback, request);
    //!!!KRS check error here
    return;


error:
        assert(0);  ///KRS
    }

// service registration callback performs three duties - frees memory for deregistered services,
// handles name conflicts, and delivers comppleted registration information to the client (via
// process_service_registraion())

static void regservice_callback(mDNS *const m, ServiceRecordSet *const srs, mStatus result)
    {
    (void)m;
    if (result == mStatus_NoError)
        return process_service_registration(srs);
    else if (result == mStatus_MemFree)
        free(srs);
    else if (result == mStatus_NameConflict)
    	{
        fprintf(stderr, "ERROR: name conflict\n");  //!!!KRS need to handle this
        return;
    	} else fprintf(stderr, "ERROR: unknown result in regservice_callback\n");
    }
        
    
static void process_service_registration(ServiceRecordSet *const srs)
    {
    ipc_msg_hdr *hdr;
    DNSServiceDiscoveryFlags *fptr;
    request_state *req = srs->ServiceContext;
    reply_state *rep;
    transfer_state send_result;

    hdr = gen_rr_response(&srs->RR_SRV.name, &fptr);
    if (!hdr) goto error;
    rep = malloc(sizeof(reply_state));
    if (!rep)
    	{
        perror("ERROR: malloc");
        goto error;
    	}
    bzero(rep, sizeof(reply_state));
    rep->ts = t_morecoming;
    rep->sd = req->sd;
    rep->len = hdr->len;
    rep->msgbuf = (char *)hdr;
    rep->fptr = fptr;
    rep->request = req;
    send_result = send_msg(rep);
    if (send_result ==  t_error) goto error;
    else if (send_result == t_complete)
    	{
        free(rep);
        return;
    	}
    // if we can't send the whole reply, because the client is blocked, link it into list
    else append_reply(req, rep);
    return;
    
error:
        assert(0);  //!!!KRS
    }

static void regservice_termination_callback(void *context)
    {
    // only safe to free memory if registration is not valid, ie deregister fails
    if (mDNS_DeregisterService(&mDNSStorage, (ServiceRecordSet *)context) != mStatus_NoError)
        free(context);
    }




static void handle_regrecord_request(request_state *rstate)
    {
    ResourceRecord *rr;
    regrecord_callback_context *rcc;
    registration_entry *re;

    assert(rstate->ts == t_complete);
    rr = read_rr_from_ipc_msg(rstate->msgbuf, 1);
    if (!rr) return;

    rcc = malloc(sizeof(regrecord_callback_context));
    if (!rcc) goto malloc_error;
    rcc->rstate = rstate;
    rcc->client_context = rstate->hdr.client_context;
    rr->RecordContext = rcc;
    rr->RecordCallback = regrecord_callback;

    // allocate registration entry, link into list
    re = malloc(sizeof(registration_entry));
    if (!re) goto malloc_error;
    re->key = rstate->hdr.reg_index;
    re->rr = rr;
    re->next = rstate->reg_entries;
    rstate->reg_entries = re;

    if (!rstate->terminate)
    	{
        rstate->terminate = connected_registration_termination;
        rstate->termination_context = rstate;
    	}
    
    if (mDNS_Register(&mDNSStorage, rr) != mStatus_NoError) assert(0);  //!!!KRS
    reset_connected_rstate(rstate);
    return;

malloc_error:
    perror("ERROR: malloc");
    return;
    }

static void regrecord_callback(mDNS *const m, ResourceRecord *const rr, mStatus result)
    {
    regrecord_callback_context *rcc;
    DNSServiceDiscoveryFlags flags, *fptr;
    u_int32_t interfaceIndex;
    DNSServiceReplyErrorType errorCode;
    int len;
    ipc_msg_hdr *hdr;
    char *data;
    reply_state *reply;
    transfer_state ts;

    (void)m;
    if (result == mStatus_NameConflict) 	{ return; }  	//!!!KRS report this to client!
    if (result == mStatus_MemFree) 	{ free(rr);  return; }
    if (result != mStatus_NoError)
        {
        fprintf(stderr, "ERROR: regrecord_callback: unknown error (%d)\n", (int)result);
        return;
        }

    rcc = rr->RecordContext;
    // format result, add to the list for the request, including the client context in the header
    len = sizeof(DNSServiceDiscoveryFlags);
    len += sizeof(u_int32_t);                //interfaceIndex
    len += sizeof(DNSServiceReplyErrorType);
    
    flags = 0;           //!!!KRS should this be set?
    interfaceIndex = 0;  //!!!KRS
    errorCode = kDNSServiceDiscoveryNoErr;

    hdr = create_hdr(reg_record_reply, &len, &data);
    if (!hdr) return;
    hdr->client_context = rcc->client_context;
    fptr = (void *)data;
    put_flags(flags, &data);
    put_long(interfaceIndex, &data);
    put_error_code(errorCode, &data);

    reply = malloc(sizeof(reply_state));
    if (!reply)
    	{
        perror("ERROR: malloc");
        return;
    	}
    bzero(reply, sizeof(reply_state));
    reply->ts = t_morecoming;
    reply->sd = rcc->rstate->sd;
    reply->len = len;
    reply->msgbuf = (char *)hdr;
    reply->fptr = fptr;
    reply->request = rcc->rstate;

    ts = send_msg(reply);
    if (ts == t_error) return;  //!!!KRS memory leak
    if (ts == t_complete) free(reply);
    if (ts == t_morecoming) append_reply(rcc->rstate, reply);   // client is blocked, link reply into list
    }

static void connected_registration_termination(void *context)
    {
    registration_entry *fptr, *ptr = ((request_state *)context)->reg_entries;
    while(ptr)
    	{
        fptr = ptr->next;
        mDNS_Deregister(&mDNSStorage, ptr->rr);
        fptr = ptr;
        ptr = ptr->next;
        free(fptr);
    	}
    }
    


static void handle_removerecord_request(request_state *rstate)
    {
    registration_entry *reptr, *prev = NULL;
    mStatus err;
    reptr = rstate->reg_entries;

    while(reptr)
    	{
        if (reptr->key == rstate->hdr.reg_index)  // found match
            {
            if (prev) prev->next = reptr->next;
            else rstate->reg_entries = reptr->next;
            err  = mDNS_Deregister(&mDNSStorage, reptr->rr);

            free(reptr);  //rr gets freed by callback
            break;
            }
        prev = reptr;
        reptr = reptr->next;
    	}
    reset_connected_rstate(rstate);
    }


// domain enumeration
static void handle_enum_request(request_state *rstate)
    {
    DNSServiceDiscoveryFlags flags, add_default;
    u_int32_t ifi;
    u_int32_t reg_domains;
    char *ptr = rstate->msgbuf;
    domain_enum_t *def, *all;
    enum_termination_t *term;
    reply_state *reply;  // initial default reply
    transfer_state tr;
    mStatus err;
    
    assert(rstate->ts == t_complete);
    flags = get_flags(&ptr);
    ifi = get_long(&ptr);
    reg_domains = get_long(&ptr);

    // allocate context structures
    def = malloc(sizeof(domain_enum_t));
    all = malloc(sizeof(domain_enum_t));
    term = malloc(sizeof(enum_termination_t));
    if (!def || !all || !term)
    	{
        perror("ERROR: malloc");
        return;				//!!!KRS
    	}

    // enumeration requires multiple questions, so we must link all the context pointers so that
    // necessary context can be reached from the callbacks
    def->rstate = rstate;
    all->rstate = rstate;
    term->def = def;
    term->all = all;
    term->rstate = rstate;
    rstate->termination_context = term;
    rstate->terminate = enum_termination_callback;
    def->question.QuestionContext = def;
    def->type = reg_domains ? mDNS_DomainTypeRegistrationDefault: mDNS_DomainTypeBrowseDefault;
    all->question.QuestionContext = all;
    all->type = reg_domains ? mDNS_DomainTypeRegistration : mDNS_DomainTypeBrowse;
    

    // provide local. as the first domain automatically
    add_default = kDNSServiceDiscoveryFlagsDefault | kDNSServiceDiscoveryFlagsAdd | kDNSServiceDiscoveryFlagsFinished;
    reply = format_enumeration_reply(rstate, "local.", add_default, ifi, 0);
    tr = send_msg(reply);
    if (tr == t_error) goto error;
    if (tr == t_complete) free(reply);
    if (tr == t_morecoming) append_reply(rstate, reply); // couldn't send whole reply because client is blocked - link into list

    // make the calls
    err = mDNS_GetDomains(&mDNSStorage, &all->question, all->type, 0, enum_result_callback, all);
    if (err == mStatus_NoError)
        err = mDNS_GetDomains(&mDNSStorage, &def->question, def->type, 0, enum_result_callback, def);
    if (err == mStatus_NoError) return;

error:
    assert(0);
    }

static void enum_result_callback(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer)
    {
    char domain[256];
    domain_enum_t *de = question->QuestionContext;
    DNSServiceDiscoveryFlags flags = 0;
    reply_state *reply;

    (void)m;  //unused
    if (answer->rrtype != kDNSType_PTR) return;
    if (answer->rrremainingttl > 0)
    	{
        flags |= kDNSServiceDiscoveryFlagsAdd;
        if (de->type == mDNS_DomainTypeRegistrationDefault || de->type == mDNS_DomainTypeBrowseDefault)
            flags |= kDNSServiceDiscoveryFlagsDefault;
    	}
    else
    	{
        flags |= kDNSServiceDiscoveryFlagsRemove;
    	}
    ConvertDomainNameToCString(&answer->rdata->u.name, domain);
    reply = format_enumeration_reply(de->rstate, domain, flags, 0, 0);  //!!!KRS ifi
    if (!reply)
    	{
        fprintf(stderr, "ERROR: enum_result_callback, format_enumeration_reply\n");
        return;
    	}
    reply->next = NULL;
    append_reply(de->rstate, reply);
    return;
    }

static reply_state *format_enumeration_reply(request_state *rstate, char *domain,                                             	DNSServiceDiscoveryFlags flags, u_int32_t ifi, DNSServiceReplyErrorType err)
    {
    int len;
    reply_state *reply;
    char *data;
    ipc_msg_hdr *hdr;
    DNSServiceDiscoveryFlags *fptr;
    
    len = sizeof(DNSServiceDiscoveryFlags);
    len += sizeof(u_int32_t);
    len += sizeof(DNSServiceReplyErrorType);
    len += strlen(domain) + 1;

    hdr = create_hdr(enumeration_reply, &len, &data);
    if (!hdr) return NULL;
    fptr = (void *)data;
    put_flags(flags, &data);
    put_long(ifi, &data);
    put_error_code(err, &data);
    put_string(domain, &data);

    reply = malloc(sizeof(reply_state));
    if (!reply)
    	{
        perror("ERROR: malloc");
        free(hdr);
        return NULL;
    	}
    bzero(reply, sizeof(reply_state));
    reply->ts = t_morecoming;
    reply->sd = rstate->sd;
    reply->len = len;
    reply->msgbuf = (char *)hdr;
    reply->fptr = fptr;
    reply->request = rstate;
    return reply;
    }

static void enum_termination_callback(void *context)
    {
    enum_termination_t *t = context;
    mDNS *coredata = &mDNSStorage;

    mDNS_StopGetDomains(coredata, &t->all->question);
    mDNS_StopGetDomains(coredata, &t->def->question);
    free(t->all);
    free(t->def);
    t->rstate->termination_context = NULL;
    free(t);
    }

static void handle_reconfirm_request(request_state *rstate)
    {
    ResourceRecord *rr;

    rr = read_rr_from_ipc_msg(rstate->msgbuf, 0);
    if (!rr) return;
    rr->RecordCallback = reconfirm_record_callback;
    mDNS_Reconfirm(&mDNSStorage, rr);
    }


static void reconfirm_record_callback(mDNS *const m, ResourceRecord *const rr, mStatus result)
    {
    (void)m;
    if (result == mStatus_MemFree) free(rr);
    }

// setup rstate to accept new reg/dereg requests
static void reset_connected_rstate(request_state *rstate)
    {
    rstate->ts = t_morecoming;
    rstate->hdr_bytes = 0;
    rstate->data_bytes = 0;
    if (rstate->msgbuf) free(rstate->msgbuf);
    rstate->msgbuf = NULL;
    rstate->bufsize = 0;
    }



// returns a resource record (allocated w/ malloc) containing the data found in an IPC message
// data must be in format flags, interfaceIndex, name, rrtype, rrclass, rdlen, rdata, (optional)ttl
// (ttl only extracted/set if ttl argument is non-zero)
static ResourceRecord *read_rr_from_ipc_msg(char *msgbuf, int ttl)
    {
    char *rdata, name[256];
    ResourceRecord *rr;
    DNSServiceDiscoveryFlags flags;
    u_int32_t interfaceIndex;

    rr = malloc(sizeof(ResourceRecord));
    if (!rr) 	{ perror("ERROR: malloc");  return NULL;  	}
    bzero(rr, sizeof(ResourceRecord));
    rr->rdata = &rr->rdatastorage;
    flags = get_flags(&msgbuf);
    interfaceIndex = get_long(&msgbuf);
    rr->InterfaceID = NULL;     //!!!KRS
    get_string(&msgbuf, name, 256);
    if (!MakeDomainNameFromDNSNameString(&rr->name, name))
    	{
        //!!!KRS
        fprintf(stderr, "ERROR: bad name: %s\n", name);
        free(rr);
        return NULL;
    	}
    rr->rrtype = get_short(&msgbuf);
    if ((flags & kDNSServiceDiscoveryFlagsShared) == kDNSServiceDiscoveryFlagsShared)
        rr->RecordType = kDNSRecordTypeShared;
    if ((flags & kDNSServiceDiscoveryFlagsUnique) == kDNSServiceDiscoveryFlagsUnique)
        rr->RecordType = kDNSRecordTypeUnique;
    rr->rrclass = get_short(&msgbuf);
    rr->rdata->RDLength = get_short(&msgbuf);
    rr->rdata->MaxRDLength = sizeof(RDataBody);
    rdata = get_rdata(&msgbuf, rr->rdata->RDLength);  //!!!KRS change function to take a buffer so we don't need to copy twice
    memcpy(rr->rdata->u.data, rdata, rr->rdata->RDLength);
    free(rdata);
    if (ttl)
    	{
        rr->rroriginalttl = get_long(&msgbuf);
        rr->rrremainingttl = rr->rroriginalttl;
    	}
    return rr;
    }


// generate a response message for a browse result, service registration result, or any other call with the
// identical callback signature
static ipc_msg_hdr *gen_rr_response(domainname *servicename, DNSServiceDiscoveryFlags **fptr)
    {
    ipc_msg_hdr *hdr;
    char *data;
    int len;
    domainlabel name;
    domainname type, dom;
    char namestr[256], typestr[256], domstr[256];
    
    if (!DeconstructServiceName(servicename, &name, &type, &dom)) goto error;
    ConvertDomainLabelToCString_unescaped(&name, namestr);
    ConvertDomainNameToCString(&type, typestr);
    ConvertDomainNameToCString(&dom, domstr);

    // calculate reply data length
    len = sizeof(DNSServiceDiscoveryFlags);
    len += sizeof(u_int32_t);  // if index
    len += sizeof(DNSServiceReplyErrorType);
    len += strlen(namestr) + 1;
    len += strlen(typestr) + 1;
    len += strlen(domstr) + 1;

    hdr = create_hdr(query_reply, &len, &data);
    if (!hdr) return NULL;
    *fptr = (DNSServiceDiscoveryFlags *)data;
    put_flags(0, &data);
    put_long(0, &data);  //!!!for ifindex - FIX THIS!
    put_error_code(0, &data);
    put_string(namestr, &data);
    put_string(typestr, &data);
    put_string(domstr, &data);
    return hdr;
    
error:
    fprintf(stderr, "ERROR: gen_rr_response\n");
    assert(0);  //!!!KRS
    }


static int BuildDomainNameFromStrings(domainname *srv, char *name, char *regtype, char *domain)
    {
    domainlabel n;
    domainname d, t;

    if (!MakeDomainLabelFromLiteralString(&n, name)) goto error;
    if (!MakeDomainNameFromDNSNameString(&t, regtype)) goto error;
    if (!MakeDomainNameFromDNSNameString(&d, domain)) goto error;
    if (!ConstructServiceName(srv, &n, &t, &d)) goto error;
    return 0;

error:
        fprintf(stderr, "ERROR: bad param\n");  //!!!KRS
    return -1;
    
    }


// append a reply to the list in a request object
static void append_reply(request_state *req, reply_state *rep)
    {
    reply_state *ptr;

    if (!req->replies) req->replies = rep;
    else
    	{
        ptr = req->replies;
        while (ptr->next) ptr = ptr->next;
        ptr->next = rep;
    	}
    rep->next = NULL;
    }


// read_msg may be called any time when the transfer state (rs->ts) is t_morecoming.
// returns the current state of the request (morecoming, error, complete, terminated.)
// if there is no data on the socket, the socket will be closed and t_terminated will be returned
static int read_msg(request_state *rs)
    {
    uint32_t nleft;
    int nread;
    char buf[4];   // dummy for death notification 
    
    assert(rs->ts != t_terminated && rs->ts != t_error);
    if (rs->ts == t_complete)
    	{  // this must be death or something is wrong
        nread = recv(rs->sd, buf, 4, 0);
        if (!nread) 	{  rs->ts = t_terminated;  return t_terminated;  	}
        if (nread < 0) goto rerror;
        fprintf(stderr, "ERROR: read data from a completed request.\n");
        rs->ts = t_error;
        return t_error;
    	}

    assert(rs->ts == t_morecoming);    
    if (rs->hdr_bytes < sizeof(ipc_msg_hdr))
    	{
        nleft = sizeof(ipc_msg_hdr) - rs->hdr_bytes;
        nread = recv(rs->sd, (char *)&rs->hdr + rs->hdr_bytes, nleft, 0);
        if (nread == 0)  	{ rs->ts = t_terminated;  return t_terminated;  	}
        if (nread < 0) goto rerror;
        rs->hdr_bytes += nread;
        assert(rs->hdr_bytes <= sizeof(ipc_msg_hdr));
    	}

    // only read data if header is complete
    if (rs->hdr_bytes == sizeof(ipc_msg_hdr))
    	{
        if (rs->hdr.datalen == 0)  // ok in removerecord requests
            {
            rs->ts = t_complete;
            rs->msgbuf = NULL;
            return t_complete;
            }
        
        if (!rs->msgbuf)  // allocate the buffer first time through
            {
            assert(!rs->data_bytes);
            rs->msgbuf = malloc(rs->hdr.datalen);
            if (!rs->msgbuf)
            	{
                perror("ERROR: malloc");
                rs->ts = t_error;
                return t_error;
            	}
            }
        nleft = rs->hdr.datalen - rs->data_bytes;
        nread = recv(rs->sd, rs->msgbuf + rs->data_bytes, nleft, 0);
        if (nread == 0)  	{ rs->ts = t_terminated;  return t_terminated; 	}
        if (nread < 0)	goto rerror;
        rs->data_bytes += nread;
        assert(rs->data_bytes <= rs->hdr.datalen);
        }

    if (rs->hdr_bytes == sizeof(ipc_msg_hdr) && rs->data_bytes == rs->hdr.datalen)
        rs->ts = t_complete;
    else rs->ts = t_morecoming;

    return rs->ts;

rerror:
    if (errno == EAGAIN || errno == EINTR) return rs->ts;	
    perror("ERROR: read_msg");
    rs->ts = t_error;
    return t_error;
    }


static int send_msg(reply_state *rs)
    {
    ssize_t nwriten;
    
    assert(rs->ts == t_morecoming);
    assert(rs->msgbuf);

    nwriten = send(rs->sd, rs->msgbuf + rs->nwriten, rs->len - rs->nwriten, 0);
    if (nwriten < 0)
    	{
        if (errno == EINTR || errno == EAGAIN) nwriten = 0;
        else
            {
            if (errno == EPIPE)
            	{
                fprintf(stderr, "broken pipe - cleanup should be handled by run-loop read wakeup\n");
                rs->ts = t_terminated;
                rs->request->ts = t_terminated;
                return t_terminated;  //!!!KRS make sure that the error handling of caller allows for this!
            	}
            else
            	{
                perror("ERROR: send\n");
                rs->ts = t_error;
                return t_error;
            	}
            }
        }
    rs->nwriten += nwriten;

    if (rs->nwriten == rs->len)
    	{
        rs->ts = t_complete;
        free(rs->msgbuf);
    	}
    return rs->ts;
    }


static void abort_request(request_state *rs)
    {
    reply_state *rep, *ptr;

    fprintf(stderr, "***UDS: Terminating connection with client:\n");
    if (rs->terminate) rs->terminate(rs->termination_context);  // terminate field may not be set yet
    assert(rs->ts == t_terminated);
    if (rs->msgbuf) free(rs->msgbuf);
    CFRunLoopRemoveSource(CFRunLoopGetCurrent(), rs->rls, kCFRunLoopDefaultMode);
    assert(!CFRunLoopContainsSource(CFRunLoopGetCurrent(), rs->rls, kCFRunLoopDefaultMode));
    CFRunLoopSourceInvalidate(rs->rls);
    CFRelease(rs->rls);
    CFSocketInvalidate(rs->sr);
    CFRelease(rs->sr);
    rs->sd = -1;

    // free pending replies
    rep = rs->replies;
    while(rep)
    	{
        if (rep->msgbuf) free(rep->msgbuf);
        ptr = rep;
        rep = rep->next;
        free(ptr);
    	}
    fprintf(stderr, "...done.\n");
    }




