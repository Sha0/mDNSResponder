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



#include "dnssd_ipc.h"
// general utility functions


static int connect_to_server(void);
static void process_reply(CFSocketRef sr, CFSocketCallBackType t, CFDataRef dr, const void *c, void *i);
static DNSServiceDiscoveryRef deliver_msg(void *msg, int sockfd);
static int my_read(int sd, char *buf, size_t len);
static int my_write(int sd, char *buf, int len);
// server response handlers
static void handle_question_response(DNSServiceDiscoveryRef sdr, ipc_msg_hdr *hdr, char *msg);
static void handle_browse_response(DNSServiceDiscoveryRef sdr, ipc_msg_hdr *hdr, char *data);
static void handle_regservice_response(DNSServiceDiscoveryRef sdr, ipc_msg_hdr *hdr, char *data);
static void handle_regrecord_response(DNSServiceDiscoveryRef sdr, ipc_msg_hdr *hdr, char *data);
static void handle_enumeration_response(DNSServiceDiscoveryRef sdr, ipc_msg_hdr *hdr, char *data);

typedef struct _DNSServiceDiscoveryRef_t
    {
    int sockfd;  // connected socket between client and daemon
    int op;      // request/reply_op_t
    process_reply_callback process_reply;
    void *app_callback;
    void *app_context;
    int request_index;  //largest assigned index
    } _DNSServiceDiscoveryRef_t;			

typedef struct _DNSRecordReference_t
    {
    void *app_context;
    DNSServiceRegisterRecordReply app_callback;
    DNSRecordReference recref;
    int registration_id;
    DNSServiceDiscoveryRef sdr;
    } _DNSRecordReference_t;

    
//#define BAD  // make the client do some bad things to stress-test server


// exported functions

CFRunLoopSourceRef DNSServiceCreateRunLoopSource
    (
    CFAllocatorRef                      allocator,
    DNSServiceDiscoveryRef              DNSServiceRef,
    CFIndex                             order
    )
    {
    CFSocketContext context = 	{ 0, DNSServiceRef, NULL, NULL, NULL 	};
    CFSocketRef sr = CFSocketCreateWithNative(allocator, DNSServiceRef->sockfd, kCFSocketReadCallBack, process_reply, &context);
    if (!sr)
    	{
        fprintf(stderr, "ERROR: DNSServiceCreateRunLoopSource - CFSocketCreateWithNative");
        return NULL;
    	}
    CFRunLoopSourceRef rls = CFSocketCreateRunLoopSource(kCFAllocatorDefault, sr, order);
    if (!rls)
    	{
        fprintf(stderr, "ERROR: DNSServiceCreateRunLoopSource - CFSocketCreateRunLoopSource");
        return NULL;
    	}
    CFRetain(rls);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), rls, kCFRunLoopDefaultMode);
    return rls;
    }


int DNSServiceGetReadFDSet
    (
    DNSServiceDiscoveryRef              DNSServiceRef,
    int                                *nfds,
    fd_set                             *readfds
    )
    {
    return 0;
    }


int DNSServiceProcessFDSet
    (
    int                                 nfds,
    fd_set                             *readfds
    )
    {
    return -1;
    }


int DNSServiceDiscoveryRefDeallocate(DNSServiceDiscoveryRef DNSServiceRef)
    {
    if (!DNSServiceRef) return -1;
    close(DNSServiceRef->sockfd);
    free(DNSServiceRef);
    return 0;
    }


static void process_reply(CFSocketRef sr, CFSocketCallBackType t, CFDataRef dr, const void *c, void *info)
    {
    ipc_msg_hdr hdr;
    char *data;
    DNSServiceDiscoveryRef sdr = info;

    // sanity check s.d. ref
    assert(sdr->sockfd > 0);
    assert(sdr->process_reply); // && sdr->app_callback);

#ifdef UDSDEBUG
    fprintf(stderr, "Reading reply from server...  ");
#endif

    if (my_read(sdr->sockfd, (void *)&hdr, sizeof(hdr)) < 0) goto error;
    if (hdr.version != VERSION)
        {
        fprintf(stderr, "ERROR: client libraries incompatible with daemon (client version = %d, "
                "daemon version = %d)\n", VERSION, hdr.version);
        return;
        }
    data = malloc(hdr.datalen);
    if (!data) goto error;
    if (my_read(sdr->sockfd, data, hdr.datalen) < 0) goto error;
#ifdef UDSDEBUG
    fprintf(stderr,"done.\n");
#endif
    sdr->process_reply(sdr, &hdr, data);
    free(data);
    return;
error:
        exit(1);  //!!!KRS notify client of error!
    }




int DNSServiceResolve
    (
    DNSServiceDiscoveryRef             *DNSServiceRef,
    const DNSServiceDiscoveryFlags      flags,
    const u_int32_t                      interfaceIndex,
    const char                         *name,
    const char                         *regtype,
    const char                         *domain,
    const u_int16_t                      rrtype,
    const DNSServiceQueryReply          callBack,
    void                               *context
    )
    {
    char *msg = NULL, *ptr;
    int len, sd;
    ipc_msg_hdr *hdr;
    DNSServiceDiscoveryRef sdr;
    
    assert(DNSServiceRef);
    *DNSServiceRef = NULL;

    //!!!KRS are null params OK?
    if (!name) name = "\0";
    
    // calculate total message length
    len = sizeof(flags);
    len += sizeof(interfaceIndex);
    len += strlen(name) + 1;
    len += strlen(regtype) + 1;
    len += strlen(domain) + 1;
    len += sizeof(rrtype);

    sd = connect_to_server();
    if (sd < 0) goto error;
    hdr = create_hdr(resolve_request, &len, &ptr);
    if (!hdr) goto error;
    msg = (void *)hdr;

    put_flags(flags, &ptr);
    put_long(interfaceIndex, &ptr);
    put_string(name, &ptr);
    put_string(regtype, &ptr);
    put_string(domain, &ptr);
    put_short(rrtype, &ptr);
    
#ifdef UDSDEBUG
    fprintf(stderr, "Issuing resolve call to server...  ");
#endif

    sdr = deliver_msg(msg, sd);
    if (!sdr) goto error;
    sdr->op = resolve_request;
    sdr->process_reply = handle_question_response;
    sdr->app_callback = callBack;
    sdr->app_context = context;
    *DNSServiceRef = sdr;
#ifdef UDSDEBUG
    fprintf(stderr, "done.\n");
#endif
    return 0;

error:
    fprintf(stderr, "exiting w/ error\n");
    if (msg) free(msg);
    if (*DNSServiceRef) { free(*DNSServiceRef);  *DNSServiceRef = NULL; }
    return -1;
    }


int DNSServiceQuery
(
DNSServiceDiscoveryRef       *DNSServiceRef,
 const DNSServiceDiscoveryFlags      flags,
 const u_int32_t                      interfaceIndex,
 const char                   *name,
 const u_int16_t                      rrtype,
 const u_int16_t                      rrclass,
 const DNSServiceQueryReply          callBack,
 void                               *context
 )
    {
    char *msg = NULL, *ptr;
    int len, sd;
    ipc_msg_hdr *hdr;
    DNSServiceDiscoveryRef sdr;

    assert(DNSServiceRef);
    *DNSServiceRef = NULL;

    //!!!KRS are null params OK?
    if (!name) name = "\0";

    // calculate total message length
    len = sizeof(flags);
    len += sizeof(u_int32_t);  //interfaceIndex
    len += strlen(name) + 1;
    len += 2 * sizeof(u_int16_t);  // rrtype, rrclass

    sd = connect_to_server();
    if (sd < 0) goto error;
    hdr = create_hdr(query_request, &len, &ptr);
    if (!hdr) goto error;
    msg = (void *)hdr;

    put_flags(flags, &ptr);
    put_long(interfaceIndex, &ptr);
    put_string(name, &ptr);
    put_short(rrtype, &ptr);
    put_short(rrclass, &ptr);

#ifdef UDSDEBUG
    fprintf(stderr, "Issuing query call to server...  ");
#endif

    sdr = deliver_msg(msg, sd);
    if (!sdr) goto error;
    sdr->op = query_request;
    sdr->process_reply = handle_question_response;
    sdr->app_callback = callBack;
    sdr->app_context = context;
    *DNSServiceRef = sdr;
#ifdef UDSDEBUG
    fprintf(stderr, "done.\n");
#endif
    return 0;

error:
    fprintf(stderr, "exiting w/ error\n");
    if (msg) free(msg);
    if (*DNSServiceRef) { free(*DNSServiceRef);  *DNSServiceRef = NULL; }
    return -1;
    }


// generic response handler for question operations (query, resolve)
static void handle_question_response(DNSServiceDiscoveryRef sdr, ipc_msg_hdr *hdr, char *data)
    {
    DNSServiceDiscoveryFlags flags;
    u_int32_t interfaceIndex, ttl;
    DNSServiceReplyErrorType errorCode;
    char name[256]; 
    u_int16_t rrtype, rrclass, rdlen;
    char *rdata;

    //!!!KRS we should do zero-copy data extraction here using pointers into the message buffer...
#ifdef UDSDEBUG
    fprintf(stderr, "Delivering resolve reply to application\n");
#endif
    
    flags = get_flags(&data);
    interfaceIndex = get_long(&data);
    errorCode = get_error_code(&data);
    get_string(&data, name, 256);
    rrtype = get_short(&data);
    rrclass = get_short(&data);
    rdlen = get_short(&data);
    rdata = get_rdata(&data, rdlen);
    ttl = get_long(&data);
    if (!rdata) goto error;
    ((DNSServiceQueryReply)sdr->app_callback)(sdr, flags, interfaceIndex, errorCode, name, rrtype, rrclass,
                                              rdlen, rdata, ttl, sdr->app_context);
    return;
error:
    fprintf(stderr, "ERROR: handle_resolve_response\n");
    }


int DNSServiceBrowse
(
 DNSServiceDiscoveryRef             *DNSServiceRef,
 const DNSServiceDiscoveryFlags      flags,
 const u_int32_t                      interfaceIndex,
 const char                         *regtype,
 const char                         *domain,
 const DNSServiceBrowseReply         callBack,
 void                               *context
 )
    {
    char *msg = NULL, *ptr;
    int len, sd;
    ipc_msg_hdr *hdr;
    DNSServiceDiscoveryRef sdr;

    assert(DNSServiceRef);
    *DNSServiceRef = NULL;

    len = sizeof(flags);
    len += sizeof(interfaceIndex);
    len += strlen(regtype) + 1;
    len += strlen(domain) + 1;

    if ((sd = connect_to_server()) < 0) goto error;
    hdr = create_hdr(browse_request, &len, &ptr);
    if (!hdr) goto error;
    msg = (char *)hdr;
    put_flags(flags, &ptr);
    put_long(interfaceIndex, &ptr);
    put_string(regtype, &ptr);
    put_string(domain, &ptr);

#ifdef UDSDEBUG
    fprintf(stderr, "Issuing browse call to server... ");
#endif

    sdr = deliver_msg(msg, sd);
    if (!sdr) goto error;
    sdr->op = browse_request;
    sdr->process_reply = handle_browse_response;
    sdr->app_callback = callBack;
    sdr->app_context = context;
    *DNSServiceRef = sdr;
#ifdef UDSDEBUG
    fprintf(stderr, "done.\n");
#endif
    return 0;

error:
    fprintf(stderr, "exiting with error\n");
    if (msg) free(msg);
    if (*DNSServiceRef) { free(*DNSServiceRef);  *DNSServiceRef = NULL; }
    return -1;
    }




static void handle_browse_response(DNSServiceDiscoveryRef sdr, ipc_msg_hdr *hdr, char *data)
    {
    DNSServiceDiscoveryFlags      flags;
    u_int32_t                      interfaceIndex;
    DNSServiceReplyErrorType      errorCode;
    char replyName[256], replyType[256], replyDomain[256];

    flags = get_flags(&data);
    interfaceIndex = get_long(&data);
    errorCode = get_error_code(&data);
    get_string(&data, replyName, 256);
    get_string(&data, replyType, 256);
    get_string(&data, replyDomain, 256);
    ((DNSServiceBrowseReply)sdr->app_callback)(sdr, flags, interfaceIndex, errorCode, replyName, replyType, replyDomain, sdr->app_context);
    }


int DNSServiceRegister
(
 DNSServiceDiscoveryRef             *DNSServiceRef,
 const DNSServiceDiscoveryFlags      flags,
 const u_int32_t                      interfaceIndex,
 const char                         *name,
 const char                         *regtype,
 const char                         *domain,
 const u_int16_t                      port,
 const u_int16_t                      txtLen,
 const void                         *txtRecord,
 const u_int32_t                      txtTTL,
 const DNSServiceRegisterReply       callBack,
 void                               *context
 )
    {
    char *msg = NULL, *ptr;
    int len, sd;
    ipc_msg_hdr *hdr;
    DNSServiceDiscoveryRef sdr;

    assert(DNSServiceRef);
    *DNSServiceRef = NULL;

    len = sizeof(DNSServiceDiscoveryFlags);
    len += 2 * sizeof(u_int32_t);  // interfaceIndex + txtTTL
    len += strlen(name) + strlen(regtype) + strlen(domain) + 3;
    len += 2 * sizeof(u_int16_t);  // port, txtLen
    len += txtLen;

    if ((sd = connect_to_server()) < 0) goto error;
    hdr = create_hdr(reg_service_request, &len, &ptr);
    if (!hdr) goto error;
    msg = (char *)hdr;
    put_flags(flags, &ptr);
    put_long(interfaceIndex, &ptr);
    put_string(name, &ptr);
    put_string(regtype, &ptr);
    put_string(domain, &ptr);
    put_short(port, &ptr);
    put_short(txtLen, &ptr);
    put_rdata(txtLen, txtRecord, &ptr);
    put_long(txtTTL, &ptr);

    fprintf(stderr, "put args: name=%s, type=%s, dom=%s\n", name, regtype, domain);
        
    sdr = deliver_msg(msg, sd);
    if (!sdr) goto error;
    sdr->op = reg_service_request;
    sdr->process_reply = handle_regservice_response;
    sdr->app_callback = callBack;
    sdr->app_context = context;
    *DNSServiceRef = sdr;

    return 0;
    
error:
    fprintf(stderr, "exiting with error\n");
    if (msg) free(msg);
    if (*DNSServiceRef) 	{ free(*DNSServiceRef);  *DNSServiceRef = NULL; }
    assert(0);  //!!!KRS
    return -1;
    }


static void handle_regservice_response(DNSServiceDiscoveryRef sdr, ipc_msg_hdr *hdr, char *data)
    {
    DNSServiceDiscoveryFlags flags;
    u_int32_t interfaceIndex;
    DNSServiceReplyErrorType errorCode;
    char name[256], regtype[256], domain[256];

    flags = get_flags(&data);
    interfaceIndex = get_long(&data);
    errorCode = get_error_code(&data);
    get_string(&data, name, 256);
    get_string(&data, regtype, 256);
    get_string(&data, domain, 256);
    ((DNSServiceRegisterReply)sdr->app_callback)(sdr, flags, interfaceIndex, errorCode, name, regtype, domain, sdr->app_context);
    }

int DNSServiceEnumerateDomains
(
 DNSServiceDiscoveryRef             	*DNSServiceRef,
 const DNSServiceDiscoveryFlags      flags,
 const u_int32_t                      interfaceIndex,
 const u_int32_t                      registrationDomains,
 const DNSServiceDomainEnumerationReply       callBack,
 void                               *context
 )
    {
    char *msg = NULL, *ptr;
    int len, sd;
    ipc_msg_hdr *hdr;
    DNSServiceDiscoveryRef sdr;

    assert(DNSServiceRef);
    *DNSServiceRef = NULL;

    len = sizeof(DNSServiceDiscoveryFlags);
    len += 2 * sizeof(u_int32_t);  // interfaceIndex, registrationDomains

    sd = connect_to_server();
    if (sd < 0) goto error;
    hdr = create_hdr(enumeration_request, &len, &ptr);
    if (!hdr) goto error;
    msg = (void *)hdr;

    put_flags(flags, &ptr);
    put_long(interfaceIndex, &ptr);
    put_long(registrationDomains, &ptr);

    sdr = deliver_msg(msg, sd);
    if (!sdr) goto error;
    sdr->op = enumeration_request;
    sdr->process_reply = handle_enumeration_response;
    sdr->app_callback = callBack;
    sdr->app_context = context;
    *DNSServiceRef = sdr;
    return 0;

error:
    fprintf(stderr, "exiting w/ error\n");
    if (msg) free(msg);
    if (*DNSServiceRef) { free(*DNSServiceRef);  *DNSServiceRef = NULL; }
    return -1;
    }


static void handle_enumeration_response(DNSServiceDiscoveryRef sdr, ipc_msg_hdr *hdr, char *data)
    {
    DNSServiceDiscoveryFlags flags;
    u_int32_t interfaceIndex;
    DNSServiceReplyErrorType err;
    char domain[256];

    flags = get_flags(&data);
    interfaceIndex = get_long(&data);
    err = get_error_code(&data);
    get_string(&data, domain, 256);
    ((DNSServiceDomainEnumerationReply)sdr->app_callback)(sdr, flags, interfaceIndex, err, domain, sdr->app_context);
    }



int DNSServiceConnect(DNSServiceDiscoveryRef *DNSServiceRef)
    {
    int fd;

    fd = connect_to_server();
    if (fd < 0)
    	{
        *DNSServiceRef = NULL;
        return -1;
    	}

    *DNSServiceRef = malloc(sizeof(_DNSServiceDiscoveryRef_t));
    if (!*DNSServiceRef)
    	{
        perror("ERROR: malloc");
        return -1;
    	}
    bzero(*DNSServiceRef, sizeof(_DNSServiceDiscoveryRef_t));
    (*DNSServiceRef)->sockfd = fd;
    (*DNSServiceRef)->op = connection;
    (*DNSServiceRef)->process_reply = handle_regrecord_response;
    return 0;
    }



static void handle_regrecord_response(DNSServiceDiscoveryRef sdr, ipc_msg_hdr *hdr, char *data)
    {
    DNSServiceDiscoveryFlags flags;
    u_int32_t interfaceIndex;
    DNSServiceReplyErrorType errorCode;
    DNSRecordReference rref;
    
    assert(sdr->op == connection);
    rref = hdr->client_context.context;
    flags = get_flags(&data);
    interfaceIndex = get_long(&data);
    errorCode = get_error_code(&data);

    rref->app_callback(rref->sdr, rref, flags, interfaceIndex, errorCode, rref->app_context);
    }

int DNSServiceRegisterRecord
(
 const DNSServiceDiscoveryRef        DNSServiceRef,
 DNSRecordReference                 *DNSRecordRef,  
 const DNSServiceDiscoveryFlags      flags,
 const u_int32_t                      interfaceIndex,
 const char                         *name,
 const u_int16_t                      rrtype,
 const u_int16_t                      rrclass,
 const u_int16_t                      rdlen,
 const char                         *rdata,
 const u_int32_t                      ttl,
 const DNSServiceRegisterRecordReply  callBack,
 void                               *context
 )
    {
    char *msg = NULL, *ptr;
    int len;
    ipc_msg_hdr *hdr;
    DNSServiceDiscoveryRef tmp = NULL;
    DNSRecordReference rref;
    
    if (!DNSServiceRef || DNSServiceRef->op != connection) return -1;
    assert(DNSServiceRef->sockfd > 0);
    assert(DNSRecordRef);
    *DNSRecordRef = NULL;
    
    len = sizeof(DNSServiceDiscoveryFlags);
    len += 2 * sizeof(u_int32_t);  // interfaceIndex, ttl
    len += 3 * sizeof(u_int16_t);  // rrtype, rrclass, rdlen
    len += strlen(name) + 1;
    len += rdlen;

    hdr = create_hdr(reg_record_request, &len, &ptr);
    if (!hdr) goto error;
    msg = (char *)hdr;
    put_flags(flags, &ptr);
    put_long(interfaceIndex, &ptr);
    put_string(name, &ptr);
    put_short(rrtype, &ptr);
    put_short(rrclass, &ptr);
    put_short(rdlen, &ptr);
    put_rdata(rdlen, rdata, &ptr);
    put_long(ttl, &ptr);

    
    rref = malloc(sizeof(_DNSRecordReference_t));
    if (!rref) goto error;
    rref->app_context = context;
    rref->app_callback = callBack;
    rref->registration_id = DNSServiceRef->request_index++;
    rref->sdr = DNSServiceRef;
    hdr->client_context.context = rref;
    hdr->reg_index = rref->registration_id;  
    tmp	= deliver_msg(msg, DNSServiceRef->sockfd);
    fprintf(stderr, "delivered msg on fd %d\n", DNSServiceRef->sockfd);
    if (!tmp) goto error;
    free(tmp);
    *DNSRecordRef = rref;
    return 0;

error:
    assert(0);  //!!!KRS
    }

int DNSServiceRemoveRecord
(
 const DNSServiceDiscoveryRef        DNSServiceRef,
 const DNSRecordReference            DNSRecordRef,
 const DNSServiceDiscoveryFlags      flags
 )
    {
    ipc_msg_hdr *hdr;
    int len = 0;
    char *ptr;
    DNSServiceDiscoveryRef tmp;

    if (!DNSServiceRef || DNSServiceRef->op != connection) return -1;
    assert(DNSServiceRef->sockfd > 0);
    assert(DNSRecordRef);
    
    hdr = create_hdr(remove_record_request, &len, &ptr);
    if (!hdr) return -1;
    hdr->reg_index = DNSRecordRef->registration_id;
    tmp = deliver_msg((char *)hdr, DNSServiceRef->sockfd);
    if (!tmp) return -1;
    free(tmp);
    free(DNSRecordRef);
    return 0;
    }

void DNSServiceReconfirmRecord
(
 const DNSServiceDiscoveryFlags      flags,
 const u_int32_t                      interfaceIndex,
 const char                         *name,
 const u_int16_t                      rrtype,
 const u_int16_t                      rrclass,
 const u_int16_t                      rdlen,
 const char                         *rdata
 )
    {
    char *ptr;
    int len, sd;
    ipc_msg_hdr *hdr;

    len = sizeof(DNSServiceDiscoveryFlags);
    len += sizeof(u_int32_t);
    len += strlen(name) + 1;
    len += 3 * sizeof(u_int16_t);
    len += rdlen;
    sd = connect_to_server();
    if (sd < 0) return;
    hdr = create_hdr(reconfirm_record_request, &len, &ptr);
    if (!hdr) return;

    put_flags(flags, &ptr);
    put_long(interfaceIndex, &ptr);
    put_string(name, &ptr);
    put_short(rrtype, &ptr);
    put_short(rrclass, &ptr);
    put_short(rdlen, &ptr);
    put_rdata(rdlen, rdata, &ptr);
    my_write(sd, (char *)hdr, len);
    close(sd);
    }
        
    
/* connect_to_server()
 * 
 * return a socket descriptor connected to the server (mDNSResponder daemon)
 * on a Unix domain socket.  returns -1 on error.
 */
 
static int connect_to_server(void)
    {
    int sockfd;
    struct sockaddr_un cliaddr, saddr;

    if ((sockfd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) 
  	{
  	perror("ERROR: socket");	
	return -1;
  	}
    bzero(&cliaddr, sizeof(cliaddr));
    cliaddr.sun_family = AF_LOCAL;
    saddr.sun_family = AF_LOCAL;
    strcpy(saddr.sun_path, MDNS_UDS_SERVERPATH);
    if (connect(sockfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
  	{
  	perror("ERROR: connect");
 	return -1;
  	}
    return sockfd;	
    }


/* deliver_msg
*
* deliver a fully-formatted ipc message, returning the corresponding DNSServiceRef object.
* message buffer is deallocated with free().  DNSServiceRef is allocated with malloc().
* Returns NULL on error (message buffer is not freed on error.)
*/



#ifdef BAD
// do a write in 16-byte chunks with pseudo-random sleeps between each
int my_write(int sd, char *buf, int len)
    {
    int nwritten, n;

    n = send(sd, buf, len % 16, 0);
    if (n != len % 16) return -1;
    nwritten = len % 16;

    while (nwritten < len)
        {
        if (random() % 5) usleep(1000000);
        if (random() % 20) sleep(1);
        n = send(sd, buf + nwritten, 16, 0);
        if (n != 16) return -1;
        nwritten += n;
        }
    return nwritten;
    }

// do a write in 16-byte chunks with pseudo-random sleeps between each
int my_read(int sd, char *buf, size_t len)
    {
    int nread, n;

    n = recv(sd, buf, len % 16, 0);
    if (n != len % 16) return -1;
    nread = len % 16;

    while (nread < len)
        {
        if (random() % 5) usleep(100000);
        if (random() % 20) sleep(1);
        n = recv(sd, buf + nread, 16, 0);
        if (n != 16) return -1;
        nread += n;
    	}
    return nread;
    }

#else

int my_write(int sd, char *buf, int len)
    {
    if (send(sd, buf, len, MSG_WAITALL) != len)
    	{
        perror("ERROR: send");
        return -1;
    	}
    return 0;
    }

// read len bytes.  return 0 on success, -1 on error
int my_read(int sd, char *buf, size_t len)
    {
    if (recv(sd, buf, len, MSG_WAITALL) != len)
    	{
        perror("ERROR: recv");
        return -1;
    	}
    return 0;
    }
#endif // BAD    


DNSServiceDiscoveryRef deliver_msg(void *msg, int sockfd)
    {
    ipc_msg_hdr *hdr = msg;
    DNSServiceDiscoveryRef DNSServiceRef;

    assert(hdr);
    assert(sockfd > 0);

    if (my_write(sockfd, msg, hdr->len) < 0)  { free(msg); return NULL; }
    DNSServiceRef = malloc(sizeof(_DNSServiceDiscoveryRef_t));
    if (!DNSServiceRef)
    	{
        perror("ERROR: malloc");
        free(msg);
        return NULL;
    	}
    ((_DNSServiceDiscoveryRef_t *)(DNSServiceRef))->sockfd = sockfd;
    return DNSServiceRef;
    }
