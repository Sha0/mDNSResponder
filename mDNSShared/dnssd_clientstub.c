/*
 * Copyright (c) 2003 Apple Computer, Inc. All rights reserved.
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

$Log: dnssd_clientstub.c,v $
Revision 1.21  2004/06/18 04:53:56  rpantos
Use platform layer for socket types. Introduce USE_TCP_LOOPBACK. Remove dependency on mDNSClientAPI.h.

Revision 1.20  2004/06/12 00:50:22  cheshire
Changes for Windows compatibility

Revision 1.19  2004/05/25 18:29:33  cheshire
Move DNSServiceConstructFullName() from dnssd_clientstub.c to dnssd_clientlib.c,
so that it's also accessible to dnssd_clientshim.c (single address space) clients.

Revision 1.18  2004/05/18 23:51:27  cheshire
Tidy up all checkin comments to use consistent "<rdar://problem/xxxxxxx>" format for bug numbers

Revision 1.17  2004/05/06 18:42:58  ksekar
General dns_sd.h API cleanup, including the following radars:
<rdar://problem/3592068>: Remove flags with zero value
<rdar://problem/3479569>: Passing in NULL causes a crash.

Revision 1.16  2004/03/12 22:00:37  cheshire
Added: #include <sys/socket.h>

Revision 1.15  2004/01/20 18:36:29  ksekar
Propagated Libinfo fix for <rdar://problem/3483971>: SU:
DNSServiceUpdateRecord() doesn't allow you to update the TXT record
into TOT mDNSResponder.

Revision 1.14  2004/01/19 22:39:17  cheshire
Don't use "MSG_WAITALL"; it makes send() return "Invalid argument" on Linux;
use an explicit while() loop instead. (In any case, this should only make a difference
with non-blocking sockets, which we don't use on the client side right now.)

Revision 1.13  2004/01/19 21:46:52  cheshire
Fix compiler warning

Revision 1.12  2003/12/23 20:46:47  ksekar
<rdar://problem/3497428>: sync dnssd files between libinfo & mDNSResponder

Revision 1.11  2003/12/08 21:11:42  rpantos
Changes necessary to support mDNSResponder on Linux.

Revision 1.10  2003/10/13 23:50:53  ksekar
Updated dns_sd clientstub files to bring copies in synch with
top-of-tree Libinfo:  A memory leak in dnssd_clientstub.c is fixed,
and comments in dns_sd.h are improved.

Revision 1.9  2003/08/15 21:30:39  cheshire
Bring up to date with LibInfo version

Revision 1.8  2003/08/13 23:54:52  ksekar
Bringing dnssd_clientstub.c up to date with Libinfo, per radar 3376640

Revision 1.7  2003/08/12 19:56:25  cheshire
Update to APSL 2.0

 */

#include <errno.h>
#include <stdlib.h>
#if defined(WIN32)
#include <winsock2.h>
#include <windows.h>
#define MSG_WAITALL 0
#define sockaddr_mdns sockaddr_in
#define AF_MDNS AF_INET
#else
#include <sys/time.h>
#include <sys/socket.h>
#define sockaddr_mdns sockaddr_un
#define AF_MDNS AF_LOCAL
#endif

#include "dnssd_ipc.h"

#if defined(WIN32)
static int g_initWinsock = 0;
#endif


#define CTL_PATH_PREFIX "/tmp/dnssd_clippath."
// error socket (if needed) is named "dnssd_clipath.[pid].xxx:n" where xxx are the
// last 3 digits of the time (in seconds) and n is the 6-digit microsecond time

// general utility functions
typedef struct _DNSServiceRef_t
    {
    dnssd_sock_t sockfd;  // connected socket between client and daemon
    int op;      // request/reply_op_t
    process_reply_callback process_reply;
    void *app_callback;
    void *app_context;
    uint32_t max_index;  //largest assigned record index - 0 if no additl. recs registered
    } _DNSServiceRef_t;

typedef struct _DNSRecordRef_t
    {
    void *app_context;
    DNSServiceRegisterRecordReply app_callback;
    DNSRecordRef recref;
    int record_index;  // index is unique to the ServiceDiscoveryRef
    DNSServiceRef sdr;
    } _DNSRecordRef_t;

// exported functions

static int my_write(dnssd_sock_t sd, char *buf, int len)
    {
    while (len)
    	{
    	ssize_t num_written = send(sd, buf, len, 0);
    	if (num_written < 0 || num_written > len) return -1;
    	buf += num_written;
    	len -= num_written;
    	}
    return 0;
    }

// read len bytes.  return 0 on success, -1 on error
static int my_read(dnssd_sock_t sd, char *buf, int len)
    {
    if (recv(sd, buf, len, MSG_WAITALL) != len)  return -1;
    return 0;
    }

/* create_hdr
 *
 * allocate and initialize an ipc message header.  value of len should initially be the
 * length of the data, and is set to the value of the data plus the header.  data_start
 * is set to point to the beginning of the data section.  reuse_socket should be non-zero
 * for calls that can receive an immediate error return value on their primary socket.
 * if zero, the path to a control socket is appended at the beginning of the message buffer.
 * data_start is set past this string.
 */

static ipc_msg_hdr *create_hdr(int op, size_t *len, char **data_start, int reuse_socket)
    {
    char *msg = NULL;
    ipc_msg_hdr *hdr;
    int datalen;
    char ctrl_path[256];

    if (!reuse_socket)
        {
#if defined(WIN32)
		*len += 2;	// Allocate space for two-byte port number
#else
		struct timeval time;
		if (gettimeofday(&time, NULL) < 0) return NULL;
		sprintf(ctrl_path, "%s%d-%.3lx-%.6lu", CTL_PATH_PREFIX, (int)getpid(),
			(unsigned long)(time.tv_sec & 0xFFF), (unsigned long)(time.tv_usec));
        *len += strlen(ctrl_path) + 1;
#endif
        }

    datalen = *len;
    *len += sizeof(ipc_msg_hdr);

    // write message to buffer
    msg = malloc(*len);
    if (!msg) return NULL;

    bzero(msg, *len);
    hdr = (void *)msg;
    hdr->datalen = datalen;
    hdr->version = VERSION;
    hdr->op.request_op = op;
    if (reuse_socket) hdr->flags |= IPC_FLAGS_REUSE_SOCKET;
    *data_start = msg + sizeof(ipc_msg_hdr);
    if (!reuse_socket)  put_string(ctrl_path, data_start);
    return hdr;
    }

    // return a connected service ref (deallocate with DNSServiceRefDeallocate)
static DNSServiceRef connect_to_server(void)
    {
	dnssd_sockaddr_t saddr;
	DNSServiceRef sdr;

#if defined(WIN32)
	if (!g_initWinsock)
		{
		WSADATA wsaData;
		DNSServiceErrorType err;
		
		g_initWinsock = 1;

		err = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );

		if (err != 0) return NULL;
		}
#endif

	sdr = malloc(sizeof(_DNSServiceRef_t));
	if (!sdr) return(NULL);
	sdr->sockfd = socket(AF_DNSSD, SOCK_STREAM, 0);
	if (sdr->sockfd == dnssd_InvalidSocket) { free(sdr); return NULL; }
#if defined(USE_TCP_LOOPBACK)
	saddr.sin_family		=	AF_INET;
	saddr.sin_addr.s_addr	=	inet_addr(MDNS_TCP_SERVERADDR);
	saddr.sin_port			=	htons(MDNS_TCP_SERVERPORT);
#else
	saddr.sun_family = AF_LOCAL;
	strcpy(saddr.sun_path, MDNS_UDS_SERVERPATH);
#endif
	if (connect(sdr->sockfd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0) { free(sdr); return NULL; }
    return sdr;
	}

static DNSServiceErrorType deliver_request(void *msg, DNSServiceRef sdr, int reuse_sd)
    {
    ipc_msg_hdr *hdr = msg;
    dnssd_sockaddr_t caddr, daddr;  // (client and daemon address structs)
    char *data = (char *)msg + sizeof(ipc_msg_hdr);
    dnssd_sock_t listenfd = dnssd_InvalidSocket, errsd = dnssd_InvalidSocket;
	int ret, len = sizeof(caddr);
    DNSServiceErrorType err = kDNSServiceErr_Unknown;

    if (!hdr || sdr->sockfd < 0) return kDNSServiceErr_Unknown;

	if (!reuse_sd)
		{
        // setup temporary error socket
        if ((listenfd = socket(AF_DNSSD, SOCK_STREAM, 0)) < 0)
            goto cleanup;
        bzero(&caddr, sizeof(caddr));

#if defined(USE_TCP_LOOPBACK)
			{
			uint16_t port;
			caddr.sin_family      = AF_INET;
			caddr.sin_port        = 0;
			caddr.sin_addr.s_addr = inet_addr(MDNS_TCP_SERVERADDR);
			ret = bind(listenfd, (struct sockaddr*) &caddr, sizeof(caddr));
			if (ret < 0) goto cleanup;
			if (getsockname(listenfd, (struct sockaddr*) &caddr, &len) < 0) goto cleanup;
			listen(listenfd, 1);
			port = caddr.sin_port;
			data[0] = (char)(port >> 8);
			data[1] = (char)(port & 0x00FF);
			}
#else
			{
			mode_t mask = umask(0);
			caddr.sun_family = AF_LOCAL;
#ifndef NOT_HAVE_SA_LEN		// According to Stevens (section 3.2), there is no portable way to
								// determine whether sa_len is defined on a particular platform.
			caddr.sun_len = sizeof(struct sockaddr_un);
#endif
			strcpy(caddr.sun_path, data);
			ret = bind(listenfd, (struct sockaddr *)&caddr, sizeof(caddr));
			umask(mask);
			if (ret < 0) goto cleanup;
			listen(listenfd, 1);
			}
#endif
		}

    if (my_write(sdr->sockfd, msg, hdr->datalen + sizeof(ipc_msg_hdr)) < 0)
        goto cleanup;
    free(msg);
    msg = NULL;

    if (reuse_sd) errsd = sdr->sockfd;
    else
        {
        len = sizeof(daddr);
        errsd = accept(listenfd, (struct sockaddr *)&daddr, &len);
        if (errsd < 0)  goto cleanup;
        }

    len = recv(errsd, &err, sizeof(err), MSG_WAITALL);
    if (len != sizeof(err))
        {
        err = kDNSServiceErr_Unknown;
        }
cleanup:
    if (!reuse_sd && listenfd > 0) dnssd_close(listenfd);
    if (!reuse_sd && errsd > 0) dnssd_close(errsd);
#if !defined(USE_TCP_LOOPBACK)
    if (!reuse_sd && data) unlink(data);
#endif
    if (msg) free(msg);
    return err;
    }

int DNSSD_API DNSServiceRefSockFD(DNSServiceRef sdRef)
    {
    if (!sdRef) return -1;
    return (int) sdRef->sockfd;
    }

// handle reply from server, calling application client callback.  If there is no reply
// from the daemon on the socket contained in sdRef, the call will block.
DNSServiceErrorType DNSSD_API DNSServiceProcessResult(DNSServiceRef sdRef)
    {
    ipc_msg_hdr hdr;
    char *data;

    if (!sdRef || sdRef->sockfd < 0 || !sdRef->process_reply)
        return kDNSServiceErr_BadReference;

    if (my_read(sdRef->sockfd, (void *)&hdr, sizeof(hdr)) < 0)
        return kDNSServiceErr_Unknown;
    if (hdr.version != VERSION)
        return kDNSServiceErr_Incompatible;
    data = malloc(hdr.datalen);
    if (!data) return kDNSServiceErr_NoMemory;
    if (my_read(sdRef->sockfd, data, hdr.datalen) < 0)
        return kDNSServiceErr_Unknown;
    sdRef->process_reply(sdRef, &hdr, data);
    free(data);
    return kDNSServiceErr_NoError;
    }

void DNSSD_API DNSServiceRefDeallocate(DNSServiceRef sdRef)
    {
    if (!sdRef) return;
    if (sdRef->sockfd > 0) dnssd_close(sdRef->sockfd);
    free(sdRef);
    }

static void handle_resolve_response(DNSServiceRef sdr, ipc_msg_hdr *hdr, char *data)
    {
    DNSServiceFlags flags;
    char fullname[kDNSServiceMaxDomainName];
    char target[kDNSServiceMaxDomainName];
    uint16_t port, txtlen;
    uint32_t ifi;
    DNSServiceErrorType err;
    char *txtrecord;
    int str_error = 0;
    (void)hdr; 		//unused

    flags = get_flags(&data);
    ifi = get_long(&data);
    err = get_error_code(&data);
    if (get_string(&data, fullname, kDNSServiceMaxDomainName) < 0) str_error = 1;
    if (get_string(&data, target, kDNSServiceMaxDomainName) < 0) str_error = 1;
    port = get_short(&data);
    txtlen = get_short(&data);
    txtrecord = get_rdata(&data, txtlen);

	if (!err && str_error) err = kDNSServiceErr_Unknown;
    ((DNSServiceResolveReply)sdr->app_callback)(sdr, flags, ifi, err, fullname, target, port, txtlen, txtrecord, sdr->app_context);
    }

DNSServiceErrorType DNSSD_API DNSServiceResolve
    (
    DNSServiceRef                  	*sdRef,
    DNSServiceFlags               flags,
    uint32_t                      interfaceIndex,
    const char                         	*name,
    const char                         	*regtype,
    const char                         	*domain,
    DNSServiceResolveReply        callBack,
    void                               	*context
    )
    {
    char *msg = NULL, *ptr;
    size_t len;
    ipc_msg_hdr *hdr;
    DNSServiceRef sdr;
    DNSServiceErrorType err;

    if (!sdRef) return kDNSServiceErr_BadParam;
    *sdRef = NULL;

	if (!name || !regtype || !domain || !callBack) return kDNSServiceErr_BadParam;

    // calculate total message length
    len = sizeof(flags);
    len += sizeof(interfaceIndex);
    len += strlen(name) + 1;
    len += strlen(regtype) + 1;
    len += strlen(domain) + 1;

    hdr = create_hdr(resolve_request, &len, &ptr, 1);
    if (!hdr) goto error;
    msg = (void *)hdr;

    put_flags(flags, &ptr);
    put_long(interfaceIndex, &ptr);
    put_string(name, &ptr);
    put_string(regtype, &ptr);
    put_string(domain, &ptr);

    sdr = connect_to_server();
    if (!sdr) goto error;
    err = deliver_request(msg, sdr, 1);
    if (err)
        {
        DNSServiceRefDeallocate(sdr);
        return err;
        }
    sdr->op = resolve_request;
    sdr->process_reply = handle_resolve_response;
    sdr->app_callback = callBack;
    sdr->app_context = context;
    *sdRef = sdr;

    return err;

error:
    if (msg) free(msg);
    if (*sdRef) { free(*sdRef);  *sdRef = NULL; }
    return kDNSServiceErr_Unknown;
    }

static void handle_query_response(DNSServiceRef sdr, ipc_msg_hdr *hdr, char *data)
    {
    DNSServiceFlags flags;
    uint32_t interfaceIndex, ttl;
    DNSServiceErrorType errorCode;
    char name[kDNSServiceMaxDomainName];
    uint16_t rrtype, rrclass, rdlen;
    char *rdata;
    int str_error = 0;
    (void)hdr;//Unused

    flags = get_flags(&data);
    interfaceIndex = get_long(&data);
    errorCode = get_error_code(&data);
    if (get_string(&data, name, kDNSServiceMaxDomainName) < 0) str_error = 1;
    rrtype = get_short(&data);
    rrclass = get_short(&data);
    rdlen = get_short(&data);
    rdata = get_rdata(&data, rdlen);
	ttl = get_long(&data);

	if (!errorCode && str_error) errorCode = kDNSServiceErr_Unknown;
	((DNSServiceQueryRecordReply)sdr->app_callback)(sdr, flags, interfaceIndex, errorCode, name, rrtype, rrclass,
													rdlen, rdata, ttl, sdr->app_context);
    return;
    }

DNSServiceErrorType DNSSD_API DNSServiceQueryRecord
(
 DNSServiceRef       	         	*sdRef,
 DNSServiceFlags		         flags,
 uint32_t                      	interfaceIndex,
 const char                   		*name,
 uint16_t                      	rrtype,
 uint16_t                      	rrclass,
 DNSServiceQueryRecordReply     	callBack,
 void                               	*context
 )
    {
    char *msg = NULL, *ptr;
    size_t len;
    ipc_msg_hdr *hdr;
    DNSServiceRef sdr;
    DNSServiceErrorType err;

    if (!sdRef) return kDNSServiceErr_BadParam;
    *sdRef = NULL;

    if (!name) name = "\0";

    // calculate total message length
    len = sizeof(flags);
    len += sizeof(uint32_t);  //interfaceIndex
    len += strlen(name) + 1;
    len += 2 * sizeof(uint16_t);  // rrtype, rrclass

    hdr = create_hdr(query_request, &len, &ptr, 1);
    if (!hdr) goto error;
    msg = (void *)hdr;

    put_flags(flags, &ptr);
    put_long(interfaceIndex, &ptr);
    put_string(name, &ptr);
    put_short(rrtype, &ptr);
    put_short(rrclass, &ptr);

    sdr = connect_to_server();
    if (!sdr) goto error;
    err = deliver_request(msg, sdr, 1);
    if (err)
        {
        DNSServiceRefDeallocate(sdr);
        return err;
        }

    sdr->op = query_request;
    sdr->process_reply = handle_query_response;
    sdr->app_callback = callBack;
    sdr->app_context = context;
    *sdRef = sdr;
    return err;

error:
    if (msg) free(msg);
    if (*sdRef) { free(*sdRef);  *sdRef = NULL; }
    return kDNSServiceErr_Unknown;
    }

static void handle_browse_response(DNSServiceRef sdr, ipc_msg_hdr *hdr, char *data)
    {
    DNSServiceFlags      flags;
    uint32_t                      interfaceIndex;
    DNSServiceErrorType      errorCode;
    char replyName[256], replyType[kDNSServiceMaxDomainName],
        replyDomain[kDNSServiceMaxDomainName];
    int str_error = 0;
	(void)hdr;//Unused

    flags = get_flags(&data);
    interfaceIndex = get_long(&data);
    errorCode = get_error_code(&data);
    if (get_string(&data, replyName, 256) < 0) str_error = 1;
    if (get_string(&data, replyType, kDNSServiceMaxDomainName) < 0) str_error = 1;
    if (get_string(&data, replyDomain, kDNSServiceMaxDomainName) < 0) str_error = 1;
	if (!errorCode && str_error) errorCode = kDNSServiceErr_Unknown;
	((DNSServiceBrowseReply)sdr->app_callback)(sdr, flags, interfaceIndex, errorCode, replyName, replyType, replyDomain, sdr->app_context);
    }

DNSServiceErrorType DNSSD_API DNSServiceBrowse
(
 DNSServiceRef                      *sdRef,
 DNSServiceFlags              flags,
 uint32_t                     interfaceIndex,
 const char                         *regtype,
 const char                         *domain,
 DNSServiceBrowseReply        callBack,
 void                               *context
 )
    {
    char *msg = NULL, *ptr;
    size_t len;
    ipc_msg_hdr *hdr;
    DNSServiceRef sdr;
    DNSServiceErrorType err;

    if (!sdRef) return kDNSServiceErr_BadParam;
    *sdRef = NULL;

    if (!domain) domain = "";

    len = sizeof(flags);
    len += sizeof(interfaceIndex);
    len += strlen(regtype) + 1;
    len += strlen(domain) + 1;

    hdr = create_hdr(browse_request, &len, &ptr, 1);
    if (!hdr) goto error;
    msg = (char *)hdr;
    put_flags(flags, &ptr);
    put_long(interfaceIndex, &ptr);
    put_string(regtype, &ptr);
    put_string(domain, &ptr);

    sdr = connect_to_server();
    if (!sdr) goto error;
    err = deliver_request(msg, sdr, 1);
    if (err)
        {
        DNSServiceRefDeallocate(sdr);
        return err;
        }
    sdr->op = browse_request;
    sdr->process_reply = handle_browse_response;
    sdr->app_callback = callBack;
    sdr->app_context = context;
    *sdRef = sdr;
    return err;

error:
    if (msg) free(msg);
    if (*sdRef) { free(*sdRef);  *sdRef = NULL; }
    return kDNSServiceErr_Unknown;
    }

static void handle_regservice_response(DNSServiceRef sdr, ipc_msg_hdr *hdr, char *data)
    {
    DNSServiceFlags flags;
    uint32_t interfaceIndex;
    DNSServiceErrorType errorCode;
    char name[256], regtype[kDNSServiceMaxDomainName], domain[kDNSServiceMaxDomainName];
    int str_error = 0;
	(void)hdr;//Unused

    flags = get_flags(&data);
    interfaceIndex = get_long(&data);
    errorCode = get_error_code(&data);
    if (get_string(&data, name, 256) < 0) str_error = 1;
    if (get_string(&data, regtype, kDNSServiceMaxDomainName) < 0) str_error = 1;
    if (get_string(&data, domain, kDNSServiceMaxDomainName) < 0) str_error = 1;
	if (!errorCode && str_error) errorCode = kDNSServiceErr_Unknown;
    ((DNSServiceRegisterReply)sdr->app_callback)(sdr, flags, errorCode, name, regtype, domain, sdr->app_context);
    }

DNSServiceErrorType DNSSD_API DNSServiceRegister
    (
    DNSServiceRef                       *sdRef,
    DNSServiceFlags               flags,
    uint32_t                      interfaceIndex,
    const char                          *name,
    const char                          *regtype,
    const char                          *domain,
    const char                          *host,
    uint16_t                      port,
    uint16_t                      txtLen,
    const void                          *txtRecord,
    DNSServiceRegisterReply       callBack,
    void                                *context
    )
    {
    char *msg = NULL, *ptr;
    size_t len;
    ipc_msg_hdr *hdr;
    DNSServiceRef sdr;
    DNSServiceErrorType err;

    if (!sdRef) return kDNSServiceErr_BadParam;
    *sdRef = NULL;

    if (!name) name = "";
    if (!regtype) return kDNSServiceErr_BadParam;
    if (!domain) domain = "";
    if (!host) host = "";
    if (!txtRecord) txtRecord = (void*)"";

    // auto-name must also have auto-rename
    if (!name[0]  && (flags & kDNSServiceFlagsNoAutoRename))
        return kDNSServiceErr_BadParam;

    // no callback must have auto-name
    if (!callBack && name[0]) return kDNSServiceErr_BadParam;

    len = sizeof(DNSServiceFlags);
    len += sizeof(uint32_t);  // interfaceIndex
    len += strlen(name) + strlen(regtype) + strlen(domain) + strlen(host) + 4;
    len += 2 * sizeof(uint16_t);  // port, txtLen
    len += txtLen;

    hdr = create_hdr(reg_service_request, &len, &ptr, 1);
    if (!hdr) goto error;
    if (!callBack) hdr->flags |= IPC_FLAGS_NOREPLY;
    msg = (char *)hdr;
    put_flags(flags, &ptr);
    put_long(interfaceIndex, &ptr);
    put_string(name, &ptr);
    put_string(regtype, &ptr);
    put_string(domain, &ptr);
    put_string(host, &ptr);
    put_short(port, &ptr);
    put_short(txtLen, &ptr);
    put_rdata(txtLen, txtRecord, &ptr);

    sdr = connect_to_server();
    if (!sdr) goto error;
    err = deliver_request(msg, sdr, 1);
    if (err)
        {
        DNSServiceRefDeallocate(sdr);
        return err;
        }

    sdr->op = reg_service_request;
    sdr->process_reply = callBack ? handle_regservice_response : NULL;
    sdr->app_callback = callBack;
    sdr->app_context = context;
    *sdRef = sdr;

    return err;

error:
    if (msg) free(msg);
    if (*sdRef) 	{ free(*sdRef);  *sdRef = NULL; }
    return kDNSServiceErr_Unknown;
    }

static void handle_enumeration_response(DNSServiceRef sdr, ipc_msg_hdr *hdr, char *data)
    {
    DNSServiceFlags flags;
    uint32_t interfaceIndex;
    DNSServiceErrorType err;
    char domain[kDNSServiceMaxDomainName];
    int str_error = 0;
	(void)hdr;//Unused

    flags = get_flags(&data);
    interfaceIndex = get_long(&data);
    err = get_error_code(&data);
    if (get_string(&data, domain, kDNSServiceMaxDomainName) < 0) str_error = 1;
	if (!err && str_error) err = kDNSServiceErr_Unknown;
    ((DNSServiceDomainEnumReply)sdr->app_callback)(sdr, flags, interfaceIndex, err, domain, sdr->app_context);
    }

DNSServiceErrorType DNSSD_API DNSServiceEnumerateDomains
(
 DNSServiceRef                    *sdRef,
 DNSServiceFlags            flags,
 uint32_t                   interfaceIndex,
 DNSServiceDomainEnumReply  callBack,
 void                             *context
 )
    {
    char *msg = NULL, *ptr;
    size_t len;
    ipc_msg_hdr *hdr;
    DNSServiceRef sdr;
    DNSServiceErrorType err;

    if (!sdRef) return kDNSServiceErr_BadParam;
    *sdRef = NULL;

	if (flags != kDNSServiceFlagsBrowseDomains && flags != kDNSServiceFlagsRegistrationDomains)
		return kDNSServiceErr_BadParam;

	len = sizeof(DNSServiceFlags);
    len += sizeof(uint32_t);

    hdr = create_hdr(enumeration_request, &len, &ptr, 1);
    if (!hdr) goto error;
    msg = (void *)hdr;

    put_flags(flags, &ptr);
    put_long(interfaceIndex, &ptr);

    sdr = connect_to_server();
    if (!sdr) goto error;
    err = deliver_request(msg, sdr, 1);
    if (err)
        {
        DNSServiceRefDeallocate(sdr);
        return err;
        }

    sdr->op = enumeration_request;
    sdr->process_reply = handle_enumeration_response;
    sdr->app_callback = callBack;
    sdr->app_context = context;
    *sdRef = sdr;
    return err;

error:
    if (msg) free(msg);
    if (*sdRef) { free(*sdRef);  *sdRef = NULL; }
    return kDNSServiceErr_Unknown;
    }

static void handle_regrecord_response(DNSServiceRef sdr, ipc_msg_hdr *hdr, char *data)
    {
    DNSServiceFlags flags;
    uint32_t interfaceIndex;
    DNSServiceErrorType errorCode;
    DNSRecordRef rref = hdr->client_context.context;

    if (sdr->op != connection)
        {
        rref->app_callback(rref->sdr, rref, 0, kDNSServiceErr_Unknown, rref->app_context);
        return;
        }
    flags = get_flags(&data);
    interfaceIndex = get_long(&data);
    errorCode = get_error_code(&data);

    rref->app_callback(rref->sdr, rref, flags, errorCode, rref->app_context);
    }

DNSServiceErrorType DNSSD_API DNSServiceCreateConnection(DNSServiceRef *sdRef)
    {
    if (!sdRef) return kDNSServiceErr_BadParam;
    *sdRef = connect_to_server();
    if (!*sdRef)
            return kDNSServiceErr_Unknown;
    (*sdRef)->op = connection;
    (*sdRef)->process_reply = handle_regrecord_response;
    return 0;
    }

DNSServiceErrorType DNSSD_API DNSServiceRegisterRecord
(
 DNSServiceRef            	sdRef,
 DNSRecordRef                 		*RecordRef,
 DNSServiceFlags      	        flags,
 uint32_t                      	interfaceIndex,
 const char                         	*fullname,
 uint16_t                      	rrtype,
 uint16_t                      	rrclass,
 uint16_t                      	rdlen,
 const void                         	*rdata,
 uint32_t                      	ttl,
 DNSServiceRegisterRecordReply  	callBack,
 void                               	*context
 )
    {
    char *msg = NULL, *ptr;
    size_t len;
    ipc_msg_hdr *hdr = NULL;
    DNSServiceRef tmp = NULL;
    DNSRecordRef rref = NULL;

    if (!sdRef || sdRef->op != connection || sdRef->sockfd < 0)
        return kDNSServiceErr_BadReference;
    *RecordRef = NULL;

	if (flags != kDNSServiceFlagsShared && flags != kDNSServiceFlagsUnique)
		return kDNSServiceErr_BadReference;

	len = sizeof(DNSServiceFlags);
    len += 2 * sizeof(uint32_t);  // interfaceIndex, ttl
    len += 3 * sizeof(uint16_t);  // rrtype, rrclass, rdlen
    len += strlen(fullname) + 1;
    len += rdlen;

    hdr = create_hdr(reg_record_request, &len, &ptr, 0);
    if (!hdr) goto error;
    msg = (char *)hdr;
    put_flags(flags, &ptr);
    put_long(interfaceIndex, &ptr);
    put_string(fullname, &ptr);
    put_short(rrtype, &ptr);
    put_short(rrclass, &ptr);
    put_short(rdlen, &ptr);
    put_rdata(rdlen, rdata, &ptr);
    put_long(ttl, &ptr);

    rref = malloc(sizeof(_DNSRecordRef_t));
    if (!rref) goto error;
    rref->app_context = context;
    rref->app_callback = callBack;
    rref->record_index = sdRef->max_index++;
    rref->sdr = sdRef;
    *RecordRef = rref;
    hdr->client_context.context = rref;
    hdr->reg_index = rref->record_index;

    return deliver_request(msg, sdRef, 0);

error:
    if (rref) free(rref);
    if (tmp) free(tmp);
    if (hdr) free(hdr);
    return kDNSServiceErr_Unknown;
    }

//sdRef returned by DNSServiceRegister()
DNSServiceErrorType DNSSD_API DNSServiceAddRecord
    (
    DNSServiceRef	                sdRef,
    DNSRecordRef			*RecordRef,
    DNSServiceFlags               flags,
    uint16_t			rrtype,
    uint16_t			rdlen,
    const void				*rdata,
    uint32_t			ttl
    )
    {
    ipc_msg_hdr *hdr;
    size_t len = 0;
    char *ptr;
    DNSRecordRef rref;

    if (!sdRef || (sdRef->op != reg_service_request) || !RecordRef)
        return kDNSServiceErr_BadReference;
    *RecordRef = NULL;

    len += 2 * sizeof(uint16_t);  //rrtype, rdlen
    len += rdlen;
    len += sizeof(uint32_t);
    len += sizeof(DNSServiceFlags);

    hdr = create_hdr(add_record_request, &len, &ptr, 0);
    if (!hdr) return kDNSServiceErr_Unknown;
    put_flags(flags, &ptr);
    put_short(rrtype, &ptr);
    put_short(rdlen, &ptr);
    put_rdata(rdlen, rdata, &ptr);
    put_long(ttl, &ptr);

    rref = malloc(sizeof(_DNSRecordRef_t));
    if (!rref) goto error;
    rref->app_context = NULL;
    rref->app_callback = NULL;
    rref->record_index = sdRef->max_index++;
    rref->sdr = sdRef;
    *RecordRef = rref;
    hdr->client_context.context = rref;
    hdr->reg_index = rref->record_index;
    return deliver_request((char *)hdr, sdRef, 0);

error:
    if (hdr) free(hdr);
    if (rref) free(rref);
    if (*RecordRef) *RecordRef = NULL;
    return kDNSServiceErr_Unknown;
}

//DNSRecordRef returned by DNSServiceRegisterRecord or DNSServiceAddRecord
DNSServiceErrorType DNSSD_API DNSServiceUpdateRecord
    (
    DNSServiceRef         	sdRef,
    DNSRecordRef			RecordRef,
    DNSServiceFlags               flags,
    uint16_t			rdlen,
    const void				*rdata,
    uint32_t			ttl
    )
    {
    ipc_msg_hdr *hdr;
    size_t len = 0;
    char *ptr;

	if (!sdRef) return kDNSServiceErr_BadReference;

    len += sizeof(uint16_t);
    len += rdlen;
    len += sizeof(uint32_t);
    len += sizeof(DNSServiceFlags);

    hdr = create_hdr(update_record_request, &len, &ptr, 0);
    if (!hdr) return kDNSServiceErr_Unknown;
    hdr->reg_index = RecordRef ? RecordRef->record_index : TXT_RECORD_INDEX;
    put_flags(flags, &ptr);
    put_short(rdlen, &ptr);
    put_rdata(rdlen, rdata, &ptr);
    put_long(ttl, &ptr);
    return deliver_request((char *)hdr, sdRef, 0);
    }

DNSServiceErrorType DNSSD_API DNSServiceRemoveRecord
(
 DNSServiceRef            sdRef,
 DNSRecordRef            	RecordRef,
 DNSServiceFlags          flags
 )
    {
    ipc_msg_hdr *hdr;
    size_t len = 0;
    char *ptr;
    DNSServiceErrorType err;

    if (!sdRef || !RecordRef || !sdRef->max_index)
        return kDNSServiceErr_BadReference;

    len += sizeof(flags);
    hdr = create_hdr(remove_record_request, &len, &ptr, 0);
    if (!hdr) return kDNSServiceErr_Unknown;
    hdr->reg_index = RecordRef->record_index;
    put_flags(flags, &ptr);
    err = deliver_request((char *)hdr, sdRef, 0);
    if (!err) free(RecordRef);
    return err;
    }

void DNSSD_API DNSServiceReconfirmRecord
(
 DNSServiceFlags              flags,
 uint32_t                     interfaceIndex,
 const char                         *fullname,
 uint16_t                     rrtype,
 uint16_t                     rrclass,
 uint16_t                     rdlen,
 const void                         *rdata
 )
    {
    char *ptr;
    size_t len;
    ipc_msg_hdr *hdr;
    DNSServiceRef tmp;

    len = sizeof(DNSServiceFlags);
    len += sizeof(uint32_t);
    len += strlen(fullname) + 1;
    len += 3 * sizeof(uint16_t);
    len += rdlen;
    tmp = connect_to_server();
    if (!tmp) return;
    hdr = create_hdr(reconfirm_record_request, &len, &ptr, 1);
    if (!hdr) return;

    put_flags(flags, &ptr);
    put_long(interfaceIndex, &ptr);
    put_string(fullname, &ptr);
    put_short(rrtype, &ptr);
    put_short(rrclass, &ptr);
    put_short(rdlen, &ptr);
    put_rdata(rdlen, rdata, &ptr);
    my_write(tmp->sockfd, (char *)hdr, len);
    DNSServiceRefDeallocate(tmp);
    }

