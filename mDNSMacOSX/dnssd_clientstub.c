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


#define CTL_PATH_PREFIX "/tmp/dnssd_clippath."	// control socket is named dnssd_clipath.[PID]

// general utility functions
static DNSServiceDiscoveryRef connect_to_server(void);
//static DNSServiceDiscoveryRef deliver_msg(void *msg, int sockfd);
DNSServiceReplyErrorType deliver_request(void *msg, DNSServiceDiscoveryRef sdr);
DNSServiceReplyErrorType deliver_request(void *msg, DNSServiceDiscoveryRef sdr);
static ipc_msg_hdr *create_hdr(int op, int *len, char **data_start);
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
    uint32_t max_index;  //largest assigned record index
    } _DNSServiceDiscoveryRef_t;			

typedef struct _DNSRecordRef_t
    {
    void *app_context;
    DNSServiceRegisterRecordReply app_callback;
    DNSRecordRef recref;
    int record_index;  // index is unique to the ServiceDiscoveryRef
    DNSServiceDiscoveryRef sdr;
    } _DNSRecordRef_t;

    
//#define BAD  // make the client do some bad things to stress-test server


// exported functions

int DNSServiceDiscoverySockFD(DNSServiceDiscoveryRef DNSServiceRef)
    {
    if (!DNSServiceRef) return -1;
    return DNSServiceRef->sockfd;
    }

// handle reply from server, calling application client callback.  If there is no reply
// from the daemon on the socket contained in DNSServiceRef, the call will block.
DNSServiceReplyErrorType DNSServiceDiscoveryProcessResult(DNSServiceDiscoveryRef DNSServiceRef)
    {
    ipc_msg_hdr hdr;
    char *data;

    if (!DNSServiceRef || DNSServiceRef->sockfd < 0 || !DNSServiceRef->process_reply) 
        return kDNSServiceDiscoveryErr_BadReference;

    if (my_read(DNSServiceRef->sockfd, (void *)&hdr, sizeof(hdr)) < 0) 
        return kDNSServiceDiscoveryErr_Unknown;
    if (hdr.version != VERSION)
        return kDNSServiceDiscoveryErr_Incompatible;
    data = malloc(hdr.datalen);
    if (!data) return kDNSServiceDiscoveryErr_NoMemory;
    if (my_read(DNSServiceRef->sockfd, data, hdr.datalen) < 0) 
        return kDNSServiceDiscoveryErr_Unknown;
    DNSServiceRef->process_reply(DNSServiceRef, &hdr, data);
    free(data);
    return kDNSServiceDiscoveryErr_Unknown;
    }


void DNSServiceDiscoveryRefDeallocate(DNSServiceDiscoveryRef DNSServiceRef)
    {
    if (!DNSServiceRef) return;
    if (DNSServiceRef->sockfd > 0) close(DNSServiceRef->sockfd);
    free(DNSServiceRef);
    }


DNSServiceReplyErrorType DNSServiceResolve
    (
    DNSServiceDiscoveryRef         	*DNSServiceRef,
    const DNSServiceDiscoveryFlags      flags,
    const uint32_t                      interfaceIndex,
    const char                         	*name,
    const char                         	*regtype,
    const char                         	*domain,
    const uint16_t                      rrtype,
    const DNSServiceQueryReply          callBack,
    void                               	*context
    )
    {
    char *msg = NULL, *ptr;
    int len;
    ipc_msg_hdr *hdr;
    DNSServiceDiscoveryRef sdr;
    DNSServiceReplyErrorType err;
    
    if (!DNSServiceRef) return kDNSServiceDiscoveryErr_BadParam;
    *DNSServiceRef = NULL;

    if (!name) name = "\0";
    
    // calculate total message length
    len = sizeof(flags);
    len += sizeof(interfaceIndex);
    len += strlen(name) + 1;
    len += strlen(regtype) + 1;
    len += strlen(domain) + 1;
    len += sizeof(rrtype);

    hdr = create_hdr(resolve_request, &len, &ptr);
    if (!hdr) goto error;
    msg = (void *)hdr;

    put_flags(flags, &ptr);
    put_long(interfaceIndex, &ptr);
    put_string(name, &ptr);
    put_string(regtype, &ptr);
    put_string(domain, &ptr);
    put_short(rrtype, &ptr);
    
    sdr = connect_to_server();
    if (!sdr) goto error;
    err = deliver_request(msg, sdr);
    if (err)
        {
        DNSServiceDiscoveryRefDeallocate(sdr);
        return err;
        }
    sdr->op = resolve_request;
    sdr->process_reply = handle_question_response;
    sdr->app_callback = callBack;
    sdr->app_context = context;
    *DNSServiceRef = sdr;
    
    return err;

error:
    if (msg) free(msg);
    if (*DNSServiceRef) { free(*DNSServiceRef);  *DNSServiceRef = NULL; }
    return kDNSServiceDiscoveryErr_Unknown;
    }


DNSServiceReplyErrorType DNSServiceQuery
(
DNSServiceDiscoveryRef       		*DNSServiceRef,
 const DNSServiceDiscoveryFlags		flags,
 const uint32_t                      	interfaceIndex,
 const char                   		*name,
 const uint16_t                      	rrtype,
 const uint16_t                      	rrclass,
 const DNSServiceQueryReply          	callBack,
 void                               	*context
 )
    {
    char *msg = NULL, *ptr;
    int len;
    ipc_msg_hdr *hdr;
    DNSServiceDiscoveryRef sdr;
    DNSServiceReplyErrorType err;
    
    if (!DNSServiceRef) return kDNSServiceDiscoveryErr_BadParam;
    *DNSServiceRef = NULL;

    if (!name) name = "\0";

    // calculate total message length
    len = sizeof(flags);
    len += sizeof(uint32_t);  //interfaceIndex
    len += strlen(name) + 1;
    len += 2 * sizeof(uint16_t);  // rrtype, rrclass

    hdr = create_hdr(query_request, &len, &ptr);
    if (!hdr) goto error;
    msg = (void *)hdr;

    put_flags(flags, &ptr);
    put_long(interfaceIndex, &ptr);
    put_string(name, &ptr);
    put_short(rrtype, &ptr);
    put_short(rrclass, &ptr);

    sdr = connect_to_server();
    if (!sdr) goto error;
    err = deliver_request(msg, sdr);
    if (err)
        {
        DNSServiceDiscoveryRefDeallocate(sdr);
        return err;
        }

    sdr->op = query_request;
    sdr->process_reply = handle_question_response;
    sdr->app_callback = callBack;
    sdr->app_context = context;
    *DNSServiceRef = sdr;
    return err;

error:
    fprintf(stderr, "exiting w/ error\n");
    if (msg) free(msg);
    if (*DNSServiceRef) { free(*DNSServiceRef);  *DNSServiceRef = NULL; }
    return kDNSServiceDiscoveryErr_Unknown;
    }


// generic response handler for question operations (query, resolve)
static void handle_question_response(DNSServiceDiscoveryRef sdr, ipc_msg_hdr *hdr, char *data)
    {
    DNSServiceDiscoveryFlags flags;
    uint32_t interfaceIndex, ttl;
    DNSServiceReplyErrorType errorCode;
    char name[256]; 
    uint16_t rrtype, rrclass, rdlen;
    char *rdata;

    //!!!KRS we should do zero-copy data extraction here using pointers into the message buffer...
    
    flags = get_flags(&data);
    interfaceIndex = get_long(&data);
    errorCode = get_error_code(&data);
    (get_string(&data, name, 256) < 0);
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


DNSServiceReplyErrorType DNSServiceBrowse
(
 DNSServiceDiscoveryRef             *DNSServiceRef,
 const DNSServiceDiscoveryFlags     flags,
 const uint32_t                     interfaceIndex,
 const char                         *regtype,
 const char                         *domain,
 const DNSServiceBrowseReply        callBack,
 void                               *context
 )
    {
    char *msg = NULL, *ptr;
    int len;
    ipc_msg_hdr *hdr;
    DNSServiceDiscoveryRef sdr;
    DNSServiceReplyErrorType err;

    if (!DNSServiceRef) return kDNSServiceDiscoveryErr_BadParam;
    *DNSServiceRef = NULL;

    len = sizeof(flags);
    len += sizeof(interfaceIndex);
    len += strlen(regtype) + 1;
    len += strlen(domain) + 1;

    hdr = create_hdr(browse_request, &len, &ptr);
    if (!hdr) goto error;
    msg = (char *)hdr;
    put_flags(flags, &ptr);
    put_long(interfaceIndex, &ptr);
    put_string(regtype, &ptr);
    put_string(domain, &ptr);

    sdr = connect_to_server();
    if (!sdr) goto error;
    err = deliver_request(msg, sdr);
    if (err)
        {
        DNSServiceDiscoveryRefDeallocate(sdr);
        return err;
        }
    sdr->op = browse_request;
    sdr->process_reply = handle_browse_response;
    sdr->app_callback = callBack;
    sdr->app_context = context;
    *DNSServiceRef = sdr;
    return err;

error:
    fprintf(stderr, "exiting with error\n");
    if (msg) free(msg);
    if (*DNSServiceRef) { free(*DNSServiceRef);  *DNSServiceRef = NULL; }
    return kDNSServiceDiscoveryErr_Unknown;
    }




static void handle_browse_response(DNSServiceDiscoveryRef sdr, ipc_msg_hdr *hdr, char *data)
    {
    DNSServiceDiscoveryFlags      flags;
    uint32_t                      interfaceIndex;
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


DNSServiceReplyErrorType DNSServiceRegister
(
 DNSServiceDiscoveryRef             *DNSServiceRef,
 const DNSServiceDiscoveryFlags     flags,
 const uint32_t                     interfaceIndex,
 const char                         *name,
 const char                         *regtype,
 const char                         *domain,
 const uint16_t                     port,
 const uint16_t                     txtLen,
 const void                         *txtRecord,
 const uint32_t                     txtTTL,
 const DNSServiceRegisterReply      callBack,
 void                               *context
 )
    {
    char *msg = NULL, *ptr;
    int len;
    ipc_msg_hdr *hdr;
    DNSServiceDiscoveryRef sdr;
    DNSServiceReplyErrorType err;

    if (!DNSServiceRef) return kDNSServiceDiscoveryErr_BadParam;
    *DNSServiceRef = NULL;

    // auto-name must also have auto-rename
    if (!name[0] && (flags & kDNSServiceDiscoveryFlagsNoRenameOnConflict))
        return kDNSServiceDiscoveryErr_BadParam;

    len = sizeof(DNSServiceDiscoveryFlags);
    len += 2 * sizeof(uint32_t);  // interfaceIndex + txtTTL
    len += strlen(name) + strlen(regtype) + strlen(domain) + 3;
    len += 2 * sizeof(uint16_t);  // port, txtLen
    len += txtLen;

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

    sdr = connect_to_server();
    if (!sdr) goto error;
    err = deliver_request(msg, sdr);
    if (err)
        {
        DNSServiceDiscoveryRefDeallocate(sdr);
        return err;
        }

    sdr->op = reg_service_request;
    sdr->process_reply = handle_regservice_response;
    sdr->app_callback = callBack;
    sdr->app_context = context;
    *DNSServiceRef = sdr;

    return err;
    
error:
    if (msg) free(msg);
    if (*DNSServiceRef) 	{ free(*DNSServiceRef);  *DNSServiceRef = NULL; }
    return kDNSServiceDiscoveryErr_Unknown;
    }


static void handle_regservice_response(DNSServiceDiscoveryRef sdr, ipc_msg_hdr *hdr, char *data)
    {
    DNSServiceDiscoveryFlags flags;
    uint32_t interfaceIndex;
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

DNSServiceReplyErrorType DNSServiceEnumerateDomains
(
 DNSServiceDiscoveryRef           *DNSServiceRef,
 const DNSServiceDiscoveryFlags   flags,
 const uint32_t                   interfaceIndex,
 const uint32_t                   registrationDomains,
 const DNSServiceDomainEnumReply  callBack,
 void                             *context
 )
    {
    char *msg = NULL, *ptr;
    int len, sd;
    ipc_msg_hdr *hdr;
    DNSServiceDiscoveryRef sdr;
    DNSServiceReplyErrorType err;


    if (!DNSServiceRef) return kDNSServiceDiscoveryErr_BadParam;
    *DNSServiceRef = NULL;

    len = sizeof(DNSServiceDiscoveryFlags);
    len += 2 * sizeof(uint32_t);  // interfaceIndex, registrationDomains

    if (sd < 0) goto error;
    hdr = create_hdr(enumeration_request, &len, &ptr);
    if (!hdr) goto error;
    msg = (void *)hdr;

    put_flags(flags, &ptr);
    put_long(interfaceIndex, &ptr);
    put_long(registrationDomains, &ptr);

    sdr = connect_to_server();
    if (!sdr) goto error;
    err = deliver_request(msg, sdr);
    if (err)
        {
        DNSServiceDiscoveryRefDeallocate(sdr);
        return err;
        }

    sdr->op = enumeration_request;
    sdr->process_reply = handle_enumeration_response;
    sdr->app_callback = callBack;
    sdr->app_context = context;
    *DNSServiceRef = sdr;
    return err;

error:
    fprintf(stderr, "exiting w/ error\n");
    if (msg) free(msg);
    if (*DNSServiceRef) { free(*DNSServiceRef);  *DNSServiceRef = NULL; }
    return kDNSServiceDiscoveryErr_Unknown;
    }


static void handle_enumeration_response(DNSServiceDiscoveryRef sdr, ipc_msg_hdr *hdr, char *data)
    {
    DNSServiceDiscoveryFlags flags;
    uint32_t interfaceIndex;
    DNSServiceReplyErrorType err;
    char domain[256];

    flags = get_flags(&data);
    interfaceIndex = get_long(&data);
    err = get_error_code(&data);
    get_string(&data, domain, 256);
    ((DNSServiceDomainEnumReply)sdr->app_callback)(sdr, flags, interfaceIndex, err, domain, sdr->app_context);
    }



DNSServiceReplyErrorType DNSServiceConnect(DNSServiceDiscoveryRef *DNSServiceRef)
    {
    if (!DNSServiceRef) return kDNSServiceDiscoveryErr_BadParam;
    *DNSServiceRef = connect_to_server();
    if (!*DNSServiceRef)
            return kDNSServiceDiscoveryErr_Unknown;
    bzero(*DNSServiceRef, sizeof(_DNSServiceDiscoveryRef_t));
    (*DNSServiceRef)->op = connection;
    (*DNSServiceRef)->process_reply = handle_regrecord_response;
    return 0;
    }



static void handle_regrecord_response(DNSServiceDiscoveryRef sdr, ipc_msg_hdr *hdr, char *data)
    {
    DNSServiceDiscoveryFlags flags;
    uint32_t interfaceIndex;
    DNSServiceReplyErrorType errorCode;
    DNSRecordRef rref;
    
    if (sdr->op != connection) 
        {
        rref->app_callback(rref->sdr, rref, 0, 0, kDNSServiceDiscoveryErr_Unknown, rref->app_context);
        return;
        }
    rref = hdr->client_context.context;
    flags = get_flags(&data);
    interfaceIndex = get_long(&data);
    errorCode = get_error_code(&data);

    rref->app_callback(rref->sdr, rref, flags, interfaceIndex, errorCode, rref->app_context);
    }

DNSServiceReplyErrorType DNSServiceRegisterRecord
(
 const DNSServiceDiscoveryRef   	DNSServiceRef,
 DNSRecordRef                 		*RecordRef,  
 const DNSServiceDiscoveryFlags      	flags,
 const uint32_t                      	interfaceIndex,
 const char                         	*name,
 const uint16_t                      	rrtype,
 const uint16_t                      	rrclass,
 const uint16_t                      	rdlen,
 const char                         	*rdata,
 const uint32_t                      	ttl,
 const DNSServiceRegisterRecordReply  	callBack,
 void                               	*context
 )
    {
    char *msg = NULL, *ptr;
    int len;
    ipc_msg_hdr *hdr = NULL;
    DNSServiceDiscoveryRef tmp = NULL;
    DNSRecordRef rref = NULL;
    
    if (!DNSServiceRef || DNSServiceRef->op != connection | DNSServiceRef->sockfd < 0) 
        return kDNSServiceDiscoveryErr_BadReference;
    *RecordRef = NULL;
    
    len = sizeof(DNSServiceDiscoveryFlags);
    len += 2 * sizeof(uint32_t);  // interfaceIndex, ttl
    len += 3 * sizeof(uint16_t);  // rrtype, rrclass, rdlen
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

    rref = malloc(sizeof(_DNSRecordRef_t));
    if (!rref) 
        {
        perror("ERROR: malloc");
        goto error;
        }
    rref->app_context = context;
    rref->app_callback = callBack;
    rref->record_index = DNSServiceRef->max_index++;
    rref->sdr = DNSServiceRef;
    *RecordRef = rref;
    hdr->client_context.context = rref;
    hdr->reg_index = rref->record_index;  
    
    return deliver_request(msg, DNSServiceRef);

error:
    if (rref) free(rref);
    if (tmp) free(tmp);
    if (hdr) free(hdr);
    return kDNSServiceDiscoveryErr_Unknown;
    }

//DNSServiceRef returned by DNSServiceRegister()
DNSServiceReplyErrorType DNSServiceAddRecord
    (
    const DNSServiceDiscoveryRef	DNSServiceRef,
    DNSRecordRef			*RecordRef,
    const uint16_t			rrtype,
    const uint16_t			rdlen,
    const char				*rdata,
    const uint32_t			ttl
    )
    {
    ipc_msg_hdr *hdr;
    int len = 0;
    char *ptr;
    DNSRecordRef rref;

    if (!DNSServiceRef || (DNSServiceRef->op != reg_service_request) || !RecordRef) 
        return kDNSServiceDiscoveryErr_BadReference;
    *RecordRef = NULL;
    
    len += 2 * sizeof(uint16_t);  //rrtype, rdlen
    len += rdlen;
    len += sizeof(uint32_t);

    hdr = create_hdr(add_record_request, &len, &ptr);
    if (!hdr) return kDNSServiceDiscoveryErr_Unknown;
    put_short(rrtype, &ptr);
    put_short(rdlen, &ptr);
    put_rdata(rdlen, rdata, &ptr);
    put_long(ttl, &ptr);

    rref = malloc(sizeof(_DNSRecordRef_t));
    if (!rref) 
        {
        perror("ERROR: malloc");
        goto error;
        }
    rref->app_context = NULL;
    rref->app_callback = NULL;
    rref->record_index = DNSServiceRef->max_index++;
    rref->sdr = DNSServiceRef;
    *RecordRef = rref;
    hdr->client_context.context = rref;
    hdr->reg_index = rref->record_index;  
    return deliver_request((char *)hdr, DNSServiceRef);

error:
    if (hdr) free(hdr);
    if (rref) free(rref);
    if (*RecordRef) *RecordRef = NULL;
    return kDNSServiceDiscoveryErr_Unknown;
}
    

//DNSRecordRef returned by DNSServiceRegisterRecord or DNSServiceAddRecord
DNSServiceReplyErrorType DNSServiceUpdateRecord
    (
    const DNSServiceDiscoveryRef	DNSServiceRef,
    DNSRecordRef			RecordRef,
    const uint16_t			rdlen,
    const char				*rdata,
    const uint32_t			ttl
    )
    {
    ipc_msg_hdr *hdr;
    int len = 0;
    char *ptr;

    if (!DNSServiceRef || 
        (DNSServiceRef->op != connection && DNSServiceRef->op != add_record_request)) 
        return kDNSServiceDiscoveryErr_BadReference;
    
    len += sizeof(uint16_t);
    len += rdlen;
    len += sizeof(uint32_t);

    hdr = create_hdr(update_record_request, &len, &ptr);
    if (!hdr) return kDNSServiceDiscoveryErr_Unknown;
    hdr->reg_index = RecordRef ? RecordRef->record_index : TXT_RECORD_INDEX;
    put_short(rdlen, &ptr);
    put_rdata(rdlen, rdata, &ptr);
    put_long(ttl, &ptr);
    return deliver_request((char *)hdr, DNSServiceRef);
    }
    


DNSServiceReplyErrorType DNSServiceRemoveRecord
(
 const DNSServiceDiscoveryRef   DNSServiceRef,
 const DNSRecordRef            	RecordRef,
 const DNSServiceDiscoveryFlags flags
 )
    {
    ipc_msg_hdr *hdr;
    int len = 0;
    char *ptr;
    DNSServiceReplyErrorType err;


    if (!DNSServiceRef || DNSServiceRef->op != connection || DNSServiceRef->sockfd < 0) 
        return kDNSServiceDiscoveryErr_BadReference;
    
    hdr = create_hdr(remove_record_request, &len, &ptr);
    if (!hdr) return kDNSServiceDiscoveryErr_Unknown;
    hdr->reg_index = RecordRef->record_index;
    err = deliver_request((char *)hdr, DNSServiceRef);
    if (!err) free(RecordRef);
    return err;
    }

void DNSServiceReconfirmRecord
(
 const DNSServiceDiscoveryFlags     flags,
 const uint32_t                     interfaceIndex,
 const char                         *name,
 const uint16_t                     rrtype,
 const uint16_t                     rrclass,
 const uint16_t                     rdlen,
 const char                         *rdata
 )
    {
    char *ptr;
    int len;
    ipc_msg_hdr *hdr;
    DNSServiceDiscoveryRef tmp;

    len = sizeof(DNSServiceDiscoveryFlags);
    len += sizeof(uint32_t);
    len += strlen(name) + 1;
    len += 3 * sizeof(uint16_t);
    len += rdlen;
    tmp = connect_to_server();
    if (!tmp) return;
    hdr = create_hdr(reconfirm_record_request, &len, &ptr);
    if (!hdr) return;

    put_flags(flags, &ptr);
    put_long(interfaceIndex, &ptr);
    put_string(name, &ptr);
    put_short(rrtype, &ptr);
    put_short(rrclass, &ptr);
    put_short(rdlen, &ptr);
    put_rdata(rdlen, rdata, &ptr);
    my_write(tmp->sockfd, (char *)hdr, len);
    DNSServiceDiscoveryRefDeallocate(tmp);
    }
        
    

// return a connected service ref (deallocate with DNSServiceDiscoveryRefDeallocate)
static DNSServiceDiscoveryRef connect_to_server(void)
    {
    struct sockaddr_un saddr;
    DNSServiceDiscoveryRef sdr;

    sdr = malloc(sizeof(_DNSServiceDiscoveryRef_t));
    if (!sdr)
        {
        perror("ERROR: malloc");
        return NULL;
        }

    if ((sdr->sockfd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) 
  	{
  	perror("ERROR: socket");	
        free(sdr);
        return NULL;
  	}

    saddr.sun_family = AF_LOCAL;
    strcpy(saddr.sun_path, MDNS_UDS_SERVERPATH);
    if (connect(sdr->sockfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
  	{
  	perror("ERROR: connect");
        free(sdr);
        return NULL;
  	}
    return sdr;	
    }




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

/* deliver message, wait for reply, free message buffer */
DNSServiceReplyErrorType deliver_request(void *msg, DNSServiceDiscoveryRef sdr)
    {
    ipc_msg_hdr *hdr = msg;
    mode_t mask;
    struct sockaddr_un caddr, daddr;  // (client and daemon address structs)
    char path[MAX_CTLPATH];
    int listenfd = -1, errsd = -1, len;
    DNSServiceReplyErrorType err;
    
    if (!hdr || sdr->sockfd < 0) return kDNSServiceDiscoveryErr_Unknown;

    sprintf(path, "%s%d", CTL_PATH_PREFIX, (int)getpid());
    if ((listenfd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) 
        {
        perror("ERROR: socket");
        goto cleanup;
        }
    unlink(path);  //OK if this fails
    bzero(&caddr, sizeof(caddr));
    caddr.sun_family = AF_LOCAL;
    strcpy(caddr.sun_path, path);
    mask = umask(0);
    if (bind(listenfd, (struct sockaddr *)&caddr, sizeof(caddr)) < 0)
        {
        perror("ERROR: bind");
        goto cleanup;
        }
    umask(mask);
    listen(listenfd, 1024);
    
    strcpy(hdr->ctrl_path, path);
    if (my_write(sdr->sockfd, msg, hdr->len) < 0)  
        goto cleanup;
    free(msg);
    msg = NULL;


    len = sizeof(daddr);
    errsd = accept(listenfd, (struct sockaddr *)&daddr, &len);
    if (errsd < 0) 
        {
        perror("ERROR: accept");
        goto cleanup;
        }
    len = recv(errsd, &err, sizeof(err), 0);
    if (len != sizeof(err))
        fprintf(stderr, "ERROR: did not receive proper error message from server\n");
        
cleanup:
    if (listenfd > 0) close(listenfd);
    if (errsd > 0) close(errsd);	
    unlink(path);
    if (msg) free(msg);
    return err;
    }
    
    
    
/* create_hdr
 *
 * allocate and initialize an ipc message header.  value of len should initially be the
 * length of the data, and is set to the value of the data plus the header.  data_start 
 * is set to point to the beginning of the data section.
 */
     
static ipc_msg_hdr *create_hdr(int op, int *len, char **data_start)
    {
    char *msg = NULL;
    ipc_msg_hdr *hdr;
    int datalen;

    datalen = *len;
    *len += sizeof(ipc_msg_hdr);

    // write message to buffer
    msg = malloc(*len);
    if (!msg)
    	{
        perror("ERROR: malloc");
        return NULL;
    	}
    
    hdr = (void *)msg;
    hdr->len = *len;
    hdr->datalen = datalen;
    hdr->version = VERSION;
    hdr->op.request_op = op;
    *data_start = msg + sizeof(ipc_msg_hdr);
    return hdr;
    }
