/* -*- Mode: C; tab-width: 4 -*-
 *
 * Copyright (c) 2003-2004, Apple Computer, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1.  Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of its
 *     contributors may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Change History (most recent first):

$Log: dnssd_clientstub.c,v $
Revision 1.65  2007/03/21 22:25:23  cheshire
<rdar://problem/4172796> Remove client retry logic now that mDNSResponder uses launchd for its Unix Domain Socket

Revision 1.64  2007/03/21 19:01:56  cheshire
<rdar://problem/5078494> IPC code not 64-bit-savvy: assumes long=32bits, and short=16bits

Revision 1.63  2007/03/12 21:48:21  cheshire
<rdar://problem/5000162> Scary unlink errors in system.log
Code was using memory after it had been freed

Revision 1.62  2007/02/28 01:44:30  cheshire
<rdar://problem/5027863> Byte order bugs in uDNS.c, uds_daemon.c, dnssd_clientstub.c

Revision 1.61  2007/02/09 03:09:42  cheshire
<rdar://problem/3869251> Cleanup: Stop returning kDNSServiceErr_Unknown so often
<rdar://problem/4177924> API: Should return kDNSServiceErr_ServiceNotRunning

Revision 1.60  2007/02/08 20:33:44  cheshire
<rdar://problem/4985095> Leak on error path in DNSServiceProcessResult

Revision 1.59  2007/01/05 08:30:55  cheshire
Trim excessive "$Log" checkin history from before 2006
(checkin history still available via "cvs log ..." of course)

Revision 1.58  2006/10/27 00:38:22  cheshire
Strip accidental trailing whitespace from lines

Revision 1.57  2006/09/30 01:06:54  cheshire
Protocol field should be uint32_t

Revision 1.56  2006/09/27 00:44:16  herscher
<rdar://problem/4249761> API: Need DNSServiceGetAddrInfo()

Revision 1.55  2006/09/26 01:52:01  herscher
<rdar://problem/4245016> NAT Port Mapping API (for both NAT-PMP and UPnP Gateway Protocol)

Revision 1.54  2006/09/21 21:34:09  cheshire
<rdar://problem/4100000> Allow empty string name when using kDNSServiceFlagsNoAutoRename

Revision 1.53  2006/09/07 04:43:12  herscher
Fix compile error on Win32 platform by moving inclusion of syslog.h

Revision 1.52  2006/08/15 23:04:21  mkrochma
<rdar://problem/4090354> Client should be able to specify service name w/o callback

Revision 1.51  2006/07/24 23:45:55  cheshire
<rdar://problem/4605276> DNSServiceReconfirmRecord() should return error code

Revision 1.50  2006/06/28 08:22:27  cheshire
<rdar://problem/4605264> dnssd_clientstub.c needs to report unlink failures in syslog

Revision 1.49  2006/06/28 07:58:59  cheshire
Minor textual tidying

*/

#include <errno.h>
#include <stdlib.h>

#include "dnssd_ipc.h"

#if defined(_WIN32)

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#define sockaddr_mdns sockaddr_in
#define AF_MDNS AF_INET

// Disable warning: "'type cast' : from data pointer 'void *' to function pointer"
#pragma warning(disable:4055)

// Disable warning: "nonstandard extension, function/data pointer conversion in expression"
#pragma warning(disable:4152)

extern BOOL IsSystemServiceDisabled();

#define sleep(X) Sleep((X) * 1000)

static int g_initWinsock = 0;

#else

#include <sys/time.h>
#include <sys/socket.h>
#include <syslog.h>

#define sockaddr_mdns sockaddr_un
#define AF_MDNS AF_LOCAL

#endif

// <rdar://problem/4096913> Specifies how many times we'll try and connect to the server.

#define DNSSD_CLIENT_MAXTRIES 4

#define CTL_PATH_PREFIX "/tmp/dnssd_clippath."
// Error socket (if needed) is named "dnssd_clipath.[pid].xxx:n" where xxx are the
// last 3 digits of the time (in seconds) and n is the 6-digit microsecond time

// General utility functions
typedef struct _DNSServiceRef_t
	{
	dnssd_sock_t sockfd;  // connected socket between client and daemon
	uint32_t op;          // request_op_t or reply_op_t
	process_reply_callback process_reply;
	void *app_callback;
	void *app_context;
	uint32_t max_index;  // largest assigned record index - 0 if no additl. recs registered
	} _DNSServiceRef_t;

typedef struct _DNSRecordRef_t
	{
	void *app_context;
	DNSServiceRegisterRecordReply app_callback;
	DNSRecordRef recref;
	uint32_t record_index;  // index is unique to the ServiceDiscoveryRef
	DNSServiceRef sdr;
	} _DNSRecordRef_t;

// Exported functions

// Write len bytes. Return 0 on success, -1 on error
static int write_all(dnssd_sock_t sd, char *buf, int len)
	{
	// Don't use "MSG_WAITALL"; it returns "Invalid argument" on some Linux versions; use an explicit while() loop instead.
	//if (send(sd, buf, len, MSG_WAITALL) != len)   return -1;
	while (len)
		{
		ssize_t num_written = send(sd, buf, len, 0);
		if (num_written < 0 || num_written > len)
			{
			// Should never happen. If it does, it indicates some OS bug,
			// or that the mDNSResponder daemon crashed (which should never happen).
			syslog(LOG_WARNING, "write_all(%d) failed %d/%d %d %s", sd, num_written, len, errno, strerror(errno));
			return -1;
			}
		buf += num_written;
		len -= num_written;
		}
	return 0;
	}

// Read len bytes. Return 0 on success, -1 on error
static int read_all(dnssd_sock_t sd, char *buf, int len)
	{
	// Don't use "MSG_WAITALL"; it returns "Invalid argument" on some Linux versions; use an explicit while() loop instead.
	//if (recv(sd, buf, len, MSG_WAITALL) != len) return -1;

	while (len)
		{
		ssize_t num_read = recv(sd, buf, len, 0);
		if ((num_read == 0) || (num_read < 0) || (num_read > len))
			{
			// Should never happen. If it does, it indicates some OS bug,
			// or that the mDNSResponder daemon crashed (which should never happen).
			syslog(LOG_WARNING, "read_all(%d) failed %d/%d %d %s", sd, num_read, len, errno, strerror(errno));
			return -1;
			}
		buf += num_read;
		len -= num_read;
		}
	return 0;
	}

/* create_hdr
 *
 * allocate and initialize an ipc message header. Value of len should initially be the
 * length of the data, and is set to the value of the data plus the header. data_start
 * is set to point to the beginning of the data section. reuse_socket should be non-zero
 * for calls that can receive an immediate error return value on their primary socket.
 * if zero, the path to a control socket is appended at the beginning of the message buffer.
 * data_start is set past this string.
 */

static ipc_msg_hdr *create_hdr(uint32_t op, size_t *len, char **data_start, int reuse_socket)
	{
	char *msg = NULL;
	ipc_msg_hdr *hdr;
	int datalen;
#if !defined(USE_TCP_LOOPBACK)
	char ctrl_path[256];
#endif

	if (!reuse_socket)
		{
#if defined(USE_TCP_LOOPBACK)
		*len += 2;  // Allocate space for two-byte port number
#else
		struct timeval time;
		if (gettimeofday(&time, NULL) < 0)
			{ syslog(LOG_WARNING, "create_hdr: gettimeofday failed %d %s", errno, strerror(errno)); return NULL; }
		sprintf(ctrl_path, "%s%d-%.3lx-%.6lu", CTL_PATH_PREFIX, (int)getpid(),
			(unsigned long)(time.tv_sec & 0xFFF), (unsigned long)(time.tv_usec));
		*len += strlen(ctrl_path) + 1;
#endif
		}

	datalen = (int) *len;
	*len += sizeof(ipc_msg_hdr);

	// Write message to buffer
	msg = malloc(*len);
	if (!msg) { syslog(LOG_WARNING, "create_hdr: malloc failed"); return NULL; }

	bzero(msg, *len);
	hdr = (void *)msg;
	hdr->datalen = datalen;
	hdr->version = VERSION;
	hdr->op = op;
	if (reuse_socket) hdr->flags |= IPC_FLAGS_REUSE_SOCKET;
	*data_start = msg + sizeof(ipc_msg_hdr);
#if defined(USE_TCP_LOOPBACK)
	// Put dummy data in for the port, since we don't know what it is yet.
	// The data will get filled in before we send the message. This happens in deliver_request().
	if (!reuse_socket) put_uint16(0, data_start);
#else
	if (!reuse_socket) put_string(ctrl_path, data_start);
#endif
	return hdr;
	}

// Return a connected service ref (deallocate with DNSServiceRefDeallocate)
static DNSServiceRef connect_to_server(void)
	{
	dnssd_sockaddr_t saddr;
	DNSServiceRef sdr;
	int NumTries = 0;

#if defined(_WIN32)
	if (!g_initWinsock)
		{
		WSADATA wsaData;
		g_initWinsock = 1;
		if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) return NULL;
		}
	// <rdar://problem/4096913> If the system service is disabled, we only want to try to connect once
	if (IsSystemServiceDisabled()) NumTries = DNSSD_CLIENT_MAXTRIES;
#endif

#if APPLE_OSX_mDNSResponder
	NumTries = DNSSD_CLIENT_MAXTRIES;
#endif

	sdr = malloc(sizeof(_DNSServiceRef_t));
	if (!sdr) { syslog(LOG_WARNING, "connect_to_server: malloc failed"); return NULL; }
	sdr->sockfd = socket(AF_DNSSD, SOCK_STREAM, 0);
	if (sdr->sockfd == dnssd_InvalidSocket)
		{
		free(sdr);
		syslog(LOG_WARNING, "connect_to_server: socket failed %d %s", errno, strerror(errno));
		return NULL;
		}

#if defined(USE_TCP_LOOPBACK)
	saddr.sin_family      = AF_INET;
	saddr.sin_addr.s_addr = inet_addr(MDNS_TCP_SERVERADDR);
	saddr.sin_port        = htons(MDNS_TCP_SERVERPORT);
#else
	saddr.sun_family = AF_LOCAL;
	strcpy(saddr.sun_path, MDNS_UDS_SERVERPATH);
#endif

	while (1)
		{
		int err = connect(sdr->sockfd, (struct sockaddr *) &saddr, sizeof(saddr));
		if (!err) break; // If we succeeded, return sdr
		// If we failed, then it may be because the daemon is still launching.
		// This can happen for processes that launch early in the boot process, while the
		// daemon is still coming up. Rather than fail here, we'll wait a bit and try again.
		// If, after four seconds, we still can't connect to the daemon,
		// then we give up and return a failure code.
		if (++NumTries < DNSSD_CLIENT_MAXTRIES)
			sleep(1); // Sleep a bit, then try again
		else
			{
			dnssd_close(sdr->sockfd);
			sdr->sockfd = dnssd_InvalidSocket;
			free(sdr);
			return NULL;
			}
		}
	return sdr;
	}

static DNSServiceErrorType deliver_request(ipc_msg_hdr *hdr, DNSServiceRef sdr, int reuse_sd)
	{
	uint32_t datalen = hdr->datalen;
	dnssd_sockaddr_t caddr, daddr;  // (client and daemon address structs)
	char *const data = (char *)hdr + sizeof(ipc_msg_hdr);
	dnssd_sock_t listenfd = dnssd_InvalidSocket, errsd = dnssd_InvalidSocket;
	int ret;
	dnssd_socklen_t len = (dnssd_socklen_t) sizeof(caddr);
	DNSServiceErrorType err;

	if (!hdr           ) { syslog(LOG_WARNING, "deliver_request: !hdr"                  ); return kDNSServiceErr_Unknown; }
	if (sdr->sockfd < 0) { syslog(LOG_WARNING, "deliver_request: sockfd %d", sdr->sockfd); return kDNSServiceErr_Unknown; }

	if (!reuse_sd)
		{
		// Setup temporary error socket
		if ((listenfd = socket(AF_DNSSD, SOCK_STREAM, 0)) < 0) goto cleanup;
		bzero(&caddr, sizeof(caddr));

#if defined(USE_TCP_LOOPBACK)
			{
			union { uint16_t s; u_char b[2]; } port;
			caddr.sin_family      = AF_INET;
			caddr.sin_port        = 0;
			caddr.sin_addr.s_addr = inet_addr(MDNS_TCP_SERVERADDR);
			ret = bind(listenfd, (struct sockaddr*) &caddr, sizeof(caddr));
			if (ret < 0) goto cleanup;
			if (getsockname(listenfd, (struct sockaddr*) &caddr, &len) < 0) goto cleanup;
			listen(listenfd, 1);
			port.s = caddr.sin_port;
			data[0] = port.b[0];  // don't switch the byte order, as the
			data[1] = port.b[1];  // daemon expects it in network byte order
			}
#else
			{
			mode_t mask = umask(0);
			caddr.sun_family = AF_LOCAL;
// According to Stevens (section 3.2), there is no portable way to
// determine whether sa_len is defined on a particular platform.
#ifndef NOT_HAVE_SA_LEN
			caddr.sun_len = sizeof(struct sockaddr_un);
#endif
			//syslog(LOG_WARNING, "deliver_request: creating UDS: %s\n", data);
			strcpy(caddr.sun_path, data);
			ret = bind(listenfd, (struct sockaddr *)&caddr, sizeof(caddr));
			umask(mask);
			if (ret < 0) goto cleanup;
			listen(listenfd, 1);
			}
#endif
		}

	ConvertHeaderBytes(hdr);
	//syslog(LOG_WARNING, "deliver_request writing %ld bytes\n", datalen + sizeof(ipc_msg_hdr));
	//syslog(LOG_WARNING, "deliver_request name is %s\n", (char *)msg + sizeof(ipc_msg_hdr));
	if (write_all(sdr->sockfd, (char *)hdr, datalen + sizeof(ipc_msg_hdr)) < 0) goto cleanup;

	if (reuse_sd) errsd = sdr->sockfd;
	else
		{
		//syslog(LOG_WARNING, "deliver_request: accept\n");
		len = sizeof(daddr);
		errsd = accept(listenfd, (struct sockaddr *)&daddr, &len);
		//syslog(LOG_WARNING, "deliver_request: accept returned %d\n", errsd);
		if (errsd < 0) goto cleanup;
		}

	if (read_all(errsd, (char*)&err, (int)sizeof(err)) < 0)
		err = kDNSServiceErr_Unknown;	// read_all will have written a message to syslog for us
	else
		err = ntohl(err);

	//syslog(LOG_WARNING, "deliver_request: retrieved error code %d\n", err);

cleanup:
	if (!reuse_sd)
		{
		if (listenfd > 0) dnssd_close(listenfd);
		if (errsd    > 0) dnssd_close(errsd);
#if !defined(USE_TCP_LOOPBACK)
		// syslog(LOG_WARNING, "deliver_request: removing UDS: %s\n", data);
		if (unlink(data) != 0)
			syslog(LOG_WARNING, "WARNING: unlink(\"%s\") failed errno %d (%s)", data, errno, strerror(errno));
		// else syslog(LOG_WARNING, "deliver_request: removed UDS: %s\n", data);
#endif
		}
	free(hdr);
	return err;
	}

int DNSSD_API DNSServiceRefSockFD(DNSServiceRef sdRef)
	{
	if (!sdRef) return -1;
	return (int) sdRef->sockfd;
	}

// Handle reply from server, calling application client callback. If there is no reply
// from the daemon on the socket contained in sdRef, the call will block.
DNSServiceErrorType DNSSD_API DNSServiceProcessResult(DNSServiceRef sdRef)
	{
	DNSServiceErrorType err = kDNSServiceErr_NoError;
	ipc_msg_hdr hdr;
	char *data;

	if (!sdRef || sdRef->sockfd < 0 || !sdRef->process_reply)
		return kDNSServiceErr_BadReference;

	// return NoError on EWOULDBLOCK. This will handle the case
	// where a non-blocking socket is told there is data, but it was a false positive.
	// On error, read_all will have written a message to syslog for us
	if (read_all(sdRef->sockfd, (void *)&hdr, sizeof(hdr)) < 0)
		return (dnssd_errno() == dnssd_EWOULDBLOCK) ? kDNSServiceErr_NoError : kDNSServiceErr_Unknown;

	ConvertHeaderBytes(&hdr);
	if (hdr.version != VERSION)
		return kDNSServiceErr_Incompatible;
	data = malloc(hdr.datalen);
	if (!data) return kDNSServiceErr_NoMemory;
	if (read_all(sdRef->sockfd, data, hdr.datalen) < 0)
		err = kDNSServiceErr_Unknown; // read_all will have written a message to syslog for us
	else
		sdRef->process_reply(sdRef, &hdr, data);
	free(data);
	return err;
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
	uint16_t txtlen;
	union { uint16_t s; u_char b[2]; } port;
	uint32_t ifi;
	DNSServiceErrorType err;
	unsigned char *txtrecord;
	int str_error = 0;
	(void)hdr; 		//unused

	flags = get_flags(&data);
	ifi = get_uint32(&data);
	err = get_error_code(&data);
	if (get_string(&data, fullname, kDNSServiceMaxDomainName) < 0) str_error = 1;
	if (get_string(&data, target,   kDNSServiceMaxDomainName) < 0) str_error = 1;
	port.b[0] = *data++;
	port.b[1] = *data++;
	txtlen = get_uint16(&data);
	txtrecord = (unsigned char *)get_rdata(&data, txtlen);

	if (!err && str_error) { err = kDNSServiceErr_Unknown; syslog(LOG_WARNING, "handle_resolve_response: error reading result from daemon"); }

	((DNSServiceResolveReply)sdr->app_callback)(sdr, flags, ifi, err, fullname, target, port.s, txtlen, txtrecord, sdr->app_context);
	}

DNSServiceErrorType DNSSD_API DNSServiceResolve
	(
	DNSServiceRef                 *sdRef,
	DNSServiceFlags               flags,
	uint32_t                      interfaceIndex,
	const char                    *name,
	const char                    *regtype,
	const char                    *domain,
	DNSServiceResolveReply        callBack,
	void                          *context
	)
	{
	char *ptr;
	size_t len;
	ipc_msg_hdr *hdr;
	DNSServiceRef sdr;
	DNSServiceErrorType err;

	if (!sdRef) return kDNSServiceErr_BadParam;
	*sdRef = NULL;

	if (!name || !regtype || !domain || !callBack) return kDNSServiceErr_BadParam;

	// Calculate total message length
	len = sizeof(flags);
	len += sizeof(interfaceIndex);
	len += strlen(name) + 1;
	len += strlen(regtype) + 1;
	len += strlen(domain) + 1;

	hdr = create_hdr(resolve_request, &len, &ptr, 1);
	if (!hdr) return kDNSServiceErr_NoMemory;

	put_flags(flags, &ptr);
	put_uint32(interfaceIndex, &ptr);
	put_string(name, &ptr);
	put_string(regtype, &ptr);
	put_string(domain, &ptr);

	sdr = connect_to_server();
	if (!sdr) { free(hdr); return kDNSServiceErr_ServiceNotRunning; }
	
	err = deliver_request(hdr, sdr, 1);		// Will free hdr for us
	if (err)
		DNSServiceRefDeallocate(sdr);
	else
		{
		sdr->op = resolve_request;
		sdr->process_reply = handle_resolve_response;
		sdr->app_callback = callBack;
		sdr->app_context = context;
		*sdRef = sdr;
		}
	return err;
	}

static void handle_query_response(DNSServiceRef sdr, ipc_msg_hdr *hdr, char *data)
	{
	DNSServiceFlags flags;
	uint32_t interfaceIndex, ttl;
	DNSServiceErrorType err;
	char name[kDNSServiceMaxDomainName];
	uint16_t rrtype, rrclass, rdlen;
	char *rdata;
	int str_error = 0;
	(void)hdr;//Unused

	flags = get_flags(&data);
	interfaceIndex = get_uint32(&data);
	err = get_error_code(&data);
	if (get_string(&data, name, kDNSServiceMaxDomainName) < 0) str_error = 1;
	rrtype = get_uint16(&data);
	rrclass = get_uint16(&data);
	rdlen = get_uint16(&data);
	rdata = get_rdata(&data, rdlen);
	ttl = get_uint32(&data);

	if (!err && str_error) { err = kDNSServiceErr_Unknown; syslog(LOG_WARNING, "handle_query_response: error reading result from daemon"); }
	((DNSServiceQueryRecordReply)sdr->app_callback)(sdr, flags, interfaceIndex, err, name, rrtype, rrclass,
													rdlen, rdata, ttl, sdr->app_context);
	return;
	}

DNSServiceErrorType DNSSD_API DNSServiceQueryRecord
	(
	DNSServiceRef              *sdRef,
	DNSServiceFlags             flags,
	uint32_t                    interfaceIndex,
	const char                 *name,
	uint16_t                    rrtype,
	uint16_t                    rrclass,
	DNSServiceQueryRecordReply  callBack,
	void                       *context
	)
	{
	char *ptr;
	size_t len;
	ipc_msg_hdr *hdr;
	DNSServiceRef sdr;
	DNSServiceErrorType err;

	if (!sdRef) return kDNSServiceErr_BadParam;
	*sdRef = NULL;

	if (!name) name = "\0";

	// Calculate total message length
	len = sizeof(flags);
	len += sizeof(uint32_t);  //interfaceIndex
	len += strlen(name) + 1;
	len += 2 * sizeof(uint16_t);  // rrtype, rrclass

	hdr = create_hdr(query_request, &len, &ptr, 1);
	if (!hdr) return kDNSServiceErr_NoMemory;

	put_flags(flags, &ptr);
	put_uint32(interfaceIndex, &ptr);
	put_string(name, &ptr);
	put_uint16(rrtype, &ptr);
	put_uint16(rrclass, &ptr);

	sdr = connect_to_server();
	if (!sdr) { free(hdr); return kDNSServiceErr_ServiceNotRunning; }
	
	err = deliver_request(hdr, sdr, 1);		// Will free hdr for us
	if (err)
		DNSServiceRefDeallocate(sdr);
	else
		{
		sdr->op = query_request;
		sdr->process_reply = handle_query_response;
		sdr->app_callback = callBack;
		sdr->app_context = context;
		*sdRef = sdr;
		}
	return err;
	}

static void handle_addrinfo_response(DNSServiceRef sdr, ipc_msg_hdr *hdr, char *data)
	{
	DNSServiceFlags flags;
	uint32_t interfaceIndex, ttl;
	DNSServiceErrorType err;
	char hostname[kDNSServiceMaxDomainName];
	int str_error = 0;
	uint16_t rrtype, rdlen;
	char *rdata;
	struct sockaddr_in  sa4;
	struct sockaddr_in6 sa6;
	struct sockaddr   * sa = NULL;
	(void)hdr;//Unused

	flags = get_flags(&data);
	interfaceIndex = get_uint32(&data);
	err = get_error_code(&data);
	if (get_string(&data, hostname, kDNSServiceMaxDomainName) < 0) str_error = 1;
	rrtype = get_uint16(&data);
	rdlen = get_uint16(&data);
	rdata = get_rdata(&data, rdlen);
	ttl = get_uint32(&data);
	
	if (!err && str_error) { err = kDNSServiceErr_Unknown; syslog(LOG_WARNING, "handle_addrinfo_response: error reading result from daemon"); }

	if (err)
		{
		sa = NULL;
		}
	else
		{
		if (rrtype == kDNSServiceType_A)
			{
			memcpy(&sa4.sin_addr, rdata, rdlen);
			sa = (struct sockaddr*) &sa4;
#ifndef NOT_HAVE_SA_LEN
			sa->sa_len = sizeof(struct sockaddr_in);
#endif
			sa->sa_family = AF_INET;
			}
		else if (rrtype == kDNSServiceType_AAAA)
			{
			memcpy(&sa6.sin6_addr, rdata, rdlen);
			sa = (struct sockaddr*) &sa6;
#ifndef NOT_HAVE_SA_LEN
			sa->sa_len = sizeof(struct sockaddr_in6);
#endif
			sa->sa_family = AF_INET6;
			}
		}

	((DNSServiceGetAddrInfoReply)sdr->app_callback)(sdr, flags, interfaceIndex, err, hostname, sa, ttl, sdr->app_context);

	return;
	}

DNSServiceErrorType DNSSD_API DNSServiceGetAddrInfo
	(
	DNSServiceRef                    *sdRef,
	DNSServiceFlags                  flags,
	uint32_t                         interfaceIndex,
	uint32_t                         protocol,
	const char                       *hostname,
	DNSServiceGetAddrInfoReply       callBack,
	void                             *context          /* may be NULL */
	)
	{
	char *ptr;
	size_t len;
	ipc_msg_hdr *hdr;
	DNSServiceRef sdr;
	DNSServiceErrorType err;

	if (!sdRef) return kDNSServiceErr_BadParam;
	*sdRef = NULL;

	if (!hostname) return kDNSServiceErr_BadParam;

	// Calculate total message length
	len = sizeof(flags);
	len += sizeof(uint32_t);      //interfaceIndex
	len += sizeof(uint32_t);      // protocol
	len += strlen(hostname) + 1;

	hdr = create_hdr(addrinfo_request, &len, &ptr, 1);
	if (!hdr) return kDNSServiceErr_NoMemory;

	put_flags(flags, &ptr);
	put_uint32(interfaceIndex, &ptr);
	put_uint32(protocol, &ptr);
	put_string(hostname, &ptr);

	sdr = connect_to_server();
	if (!sdr) { free(hdr); return kDNSServiceErr_ServiceNotRunning; }

	err = deliver_request(hdr, sdr, 1);		// Will free hdr for us
	if (err)
		DNSServiceRefDeallocate(sdr);
	else
		{
		sdr->op = addrinfo_request;
		sdr->process_reply = handle_addrinfo_response;
		sdr->app_callback = callBack;
		sdr->app_context = context;
		*sdRef = sdr;
		}
	return err;
	}
	
static void handle_browse_response(DNSServiceRef sdr, ipc_msg_hdr *hdr, char *data)
	{
	DNSServiceFlags      flags;
	uint32_t                      interfaceIndex;
	DNSServiceErrorType      err;
	char replyName[256], replyType[kDNSServiceMaxDomainName],
		replyDomain[kDNSServiceMaxDomainName];
	int str_error = 0;
	(void)hdr;//Unused

	flags = get_flags(&data);
	interfaceIndex = get_uint32(&data);
	err = get_error_code(&data);
	if (get_string(&data, replyName, 256) < 0) str_error = 1;
	if (get_string(&data, replyType, kDNSServiceMaxDomainName) < 0) str_error = 1;
	if (get_string(&data, replyDomain, kDNSServiceMaxDomainName) < 0) str_error = 1;
	if (!err && str_error) { err = kDNSServiceErr_Unknown; syslog(LOG_WARNING, "handle_browse_response: error reading result from daemon"); }
	((DNSServiceBrowseReply)sdr->app_callback)(sdr, flags, interfaceIndex, err, replyName, replyType, replyDomain, sdr->app_context);
	}

DNSServiceErrorType DNSSD_API DNSServiceBrowse
	(
	DNSServiceRef         *sdRef,
	DNSServiceFlags        flags,
	uint32_t               interfaceIndex,
	const char            *regtype,
	const char            *domain,
	DNSServiceBrowseReply  callBack,
	void                  *context
	)
	{
	char *ptr;
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
	if (!hdr) return kDNSServiceErr_NoMemory;

	put_flags(flags, &ptr);
	put_uint32(interfaceIndex, &ptr);
	put_string(regtype, &ptr);
	put_string(domain, &ptr);

	sdr = connect_to_server();
	if (!sdr) { free(hdr); return kDNSServiceErr_ServiceNotRunning; }

	err = deliver_request(hdr, sdr, 1);		// Will free hdr for us
	if (err)
		DNSServiceRefDeallocate(sdr);
	else
		{
		sdr->op = browse_request;
		sdr->process_reply = handle_browse_response;
		sdr->app_callback = callBack;
		sdr->app_context = context;
		*sdRef = sdr;
		}
	return err;
	}

DNSServiceErrorType DNSSD_API DNSServiceSetDefaultDomainForUser
	(
	DNSServiceFlags  flags,
	const char      *domain
	)
	{
	DNSServiceRef sdr;
	DNSServiceErrorType err;
	char *ptr = NULL;
	size_t len = sizeof(flags) + strlen(domain) + 1;
	ipc_msg_hdr *hdr = create_hdr(setdomain_request, &len, &ptr, 1);
	if (!hdr) return kDNSServiceErr_NoMemory;

	put_flags(flags, &ptr);
	put_string(domain, &ptr);

	sdr = connect_to_server();
	if (!sdr) { free(hdr); return kDNSServiceErr_ServiceNotRunning; }

	err = deliver_request(hdr, sdr, 1);		// Will free hdr for us
	DNSServiceRefDeallocate(sdr);
	return err;
	}


static void handle_regservice_response(DNSServiceRef sdr, ipc_msg_hdr *hdr, char *data)
	{
	DNSServiceFlags flags;
	uint32_t interfaceIndex;
	DNSServiceErrorType err;
	char name[256], regtype[kDNSServiceMaxDomainName], domain[kDNSServiceMaxDomainName];
	int str_error = 0;
	(void)hdr;//Unused

	flags = get_flags(&data);
	interfaceIndex = get_uint32(&data);
	err = get_error_code(&data);
	if (get_string(&data, name, 256) < 0) str_error = 1;
	if (get_string(&data, regtype, kDNSServiceMaxDomainName) < 0) str_error = 1;
	if (get_string(&data, domain,  kDNSServiceMaxDomainName) < 0) str_error = 1;
	if (!err && str_error) { err = kDNSServiceErr_Unknown; syslog(LOG_WARNING, "handle_regservice_response: error reading result from daemon"); }
	((DNSServiceRegisterReply)sdr->app_callback)(sdr, flags, err, name, regtype, domain, sdr->app_context);
	}

DNSServiceErrorType DNSSD_API DNSServiceRegister
	(
	DNSServiceRef                       *sdRef,
	DNSServiceFlags                     flags,
	uint32_t                            interfaceIndex,
	const char                          *name,
	const char                          *regtype,
	const char                          *domain,
	const char                          *host,
	uint16_t                            PortInNetworkByteOrder,
	uint16_t                            txtLen,
	const void                          *txtRecord,
	DNSServiceRegisterReply             callBack,
	void                                *context
	)
	{
	char *ptr;
	size_t len;
	ipc_msg_hdr *hdr;
	DNSServiceRef sdr;
	DNSServiceErrorType err;
	union { uint16_t s; u_char b[2]; } port = { PortInNetworkByteOrder };

	if (!sdRef) return kDNSServiceErr_BadParam;
	*sdRef = NULL;

	if (!name) name = "";
	if (!regtype) return kDNSServiceErr_BadParam;
	if (!domain) domain = "";
	if (!host) host = "";
	if (!txtRecord) txtRecord = (void*)"";

	// No callback must have auto-rename
	if (!callBack && (flags & kDNSServiceFlagsNoAutoRename)) return kDNSServiceErr_BadParam;

	len = sizeof(DNSServiceFlags);
	len += sizeof(uint32_t);  // interfaceIndex
	len += strlen(name) + strlen(regtype) + strlen(domain) + strlen(host) + 4;
	len += 2 * sizeof(uint16_t);  // port, txtLen
	len += txtLen;

	hdr = create_hdr(reg_service_request, &len, &ptr, 1);
	if (!hdr) return kDNSServiceErr_NoMemory;
	if (!callBack) hdr->flags |= IPC_FLAGS_NOREPLY;

	put_flags(flags, &ptr);
	put_uint32(interfaceIndex, &ptr);
	put_string(name, &ptr);
	put_string(regtype, &ptr);
	put_string(domain, &ptr);
	put_string(host, &ptr);
	*ptr++ = port.b[0];
	*ptr++ = port.b[1];
	put_uint16(txtLen, &ptr);
	put_rdata(txtLen, txtRecord, &ptr);

	sdr = connect_to_server();
	if (!sdr) { free(hdr); return kDNSServiceErr_ServiceNotRunning; }

	err = deliver_request(hdr, sdr, 1);		// Will free hdr for us
	if (err)
		DNSServiceRefDeallocate(sdr);
	else
		{
		sdr->op = reg_service_request;
		sdr->process_reply = callBack ? handle_regservice_response : NULL;
		sdr->app_callback = callBack;
		sdr->app_context = context;
		*sdRef = sdr;
		}

	return err;
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
	interfaceIndex = get_uint32(&data);
	err = get_error_code(&data);
	if (get_string(&data, domain, kDNSServiceMaxDomainName) < 0) str_error = 1;
	if (!err && str_error) { err = kDNSServiceErr_Unknown; syslog(LOG_WARNING, "handle_enumeration_response: error reading result from daemon"); }
	((DNSServiceDomainEnumReply)sdr->app_callback)(sdr, flags, interfaceIndex, err, domain, sdr->app_context);
	}

DNSServiceErrorType DNSSD_API DNSServiceEnumerateDomains
	(
	DNSServiceRef             *sdRef,
	DNSServiceFlags            flags,
	uint32_t                   interfaceIndex,
	DNSServiceDomainEnumReply  callBack,
	void                      *context
	)
	{
	char *ptr;
	size_t len;
	ipc_msg_hdr *hdr;
	DNSServiceRef sdr;
	DNSServiceErrorType err;
	int f1 = (flags & kDNSServiceFlagsBrowseDomains) != 0;
	int f2 = (flags & kDNSServiceFlagsRegistrationDomains) != 0;
	if (f1 + f2 != 1) return kDNSServiceErr_BadParam;

	if (!sdRef) return kDNSServiceErr_BadParam;
	*sdRef = NULL;

	len = sizeof(DNSServiceFlags);
	len += sizeof(uint32_t);

	hdr = create_hdr(enumeration_request, &len, &ptr, 1);
	if (!hdr) return kDNSServiceErr_NoMemory;

	put_flags(flags, &ptr);
	put_uint32(interfaceIndex, &ptr);

	sdr = connect_to_server();
	if (!sdr) { free(hdr); return kDNSServiceErr_ServiceNotRunning; }

	err = deliver_request(hdr, sdr, 1);		// Will free hdr for us
	if (err)
		DNSServiceRefDeallocate(sdr);
	else
		{
		sdr->op = enumeration_request;
		sdr->process_reply = handle_enumeration_response;
		sdr->app_callback = callBack;
		sdr->app_context = context;
		*sdRef = sdr;
		}
	return err;
	}

static void handle_regrecord_response(DNSServiceRef sdr, ipc_msg_hdr *hdr, char *data)
	{
	DNSServiceFlags flags;
	uint32_t interfaceIndex;
	DNSServiceErrorType err;
	DNSRecordRef rref = hdr->client_context.context;

	if (sdr->op != connection)
		{
		syslog(LOG_WARNING, "handle_regrecord_response: sdr->op != connection");
		rref->app_callback(rref->sdr, rref, 0, kDNSServiceErr_Unknown, rref->app_context);
		return;
		}
	flags = get_flags(&data);
	interfaceIndex = get_uint32(&data);
	err = get_error_code(&data);

	rref->app_callback(rref->sdr, rref, flags, err, rref->app_context);
	}

DNSServiceErrorType DNSSD_API DNSServiceCreateConnection(DNSServiceRef *sdRef)
	{
	if (!sdRef) return kDNSServiceErr_BadParam;
	*sdRef = connect_to_server();
	if (!*sdRef) return kDNSServiceErr_ServiceNotRunning;
	(*sdRef)->op = connection;
	(*sdRef)->process_reply = handle_regrecord_response;
	return 0;
	}

DNSServiceErrorType DNSSD_API DNSServiceRegisterRecord
	(
	DNSServiceRef                  sdRef,
	DNSRecordRef                  *RecordRef,
	DNSServiceFlags                flags,
	uint32_t                       interfaceIndex,
	const char                    *fullname,
	uint16_t                       rrtype,
	uint16_t                       rrclass,
	uint16_t                       rdlen,
	const void                    *rdata,
	uint32_t                       ttl,
	DNSServiceRegisterRecordReply  callBack,
	void                          *context
	)
	{
	char *ptr;
	size_t len;
	ipc_msg_hdr *hdr = NULL;
	DNSRecordRef rref = NULL;
	int f1 = (flags & kDNSServiceFlagsShared) != 0;
	int f2 = (flags & kDNSServiceFlagsUnique) != 0;
	if (f1 + f2 != 1) return kDNSServiceErr_BadParam;

	if (!sdRef || sdRef->op != connection || sdRef->sockfd < 0)
		return kDNSServiceErr_BadReference;
	*RecordRef = NULL;

	len = sizeof(DNSServiceFlags);
	len += 2 * sizeof(uint32_t);  // interfaceIndex, ttl
	len += 3 * sizeof(uint16_t);  // rrtype, rrclass, rdlen
	len += strlen(fullname) + 1;
	len += rdlen;

	hdr = create_hdr(reg_record_request, &len, &ptr, 0);
	if (!hdr) return kDNSServiceErr_NoMemory;

	put_flags(flags, &ptr);
	put_uint32(interfaceIndex, &ptr);
	put_string(fullname, &ptr);
	put_uint16(rrtype, &ptr);
	put_uint16(rrclass, &ptr);
	put_uint16(rdlen, &ptr);
	put_rdata(rdlen, rdata, &ptr);
	put_uint32(ttl, &ptr);

	rref = malloc(sizeof(_DNSRecordRef_t));
	if (!rref) { free(hdr); return kDNSServiceErr_NoMemory; }
	rref->app_context = context;
	rref->app_callback = callBack;
	rref->record_index = sdRef->max_index++;
	rref->sdr = sdRef;
	*RecordRef = rref;
	hdr->client_context.context = rref;
	hdr->reg_index = rref->record_index;

	return deliver_request(hdr, sdRef, 0);		// Will free hdr for us
	}

//sdRef returned by DNSServiceRegister()
DNSServiceErrorType DNSSD_API DNSServiceAddRecord
	(
	DNSServiceRef    sdRef,
	DNSRecordRef    *RecordRef,
	DNSServiceFlags  flags,
	uint16_t         rrtype,
	uint16_t         rdlen,
	const void      *rdata,
	uint32_t         ttl
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
	if (!hdr) return kDNSServiceErr_NoMemory;
	put_flags(flags, &ptr);
	put_uint16(rrtype, &ptr);
	put_uint16(rdlen, &ptr);
	put_rdata(rdlen, rdata, &ptr);
	put_uint32(ttl, &ptr);

	rref = malloc(sizeof(_DNSRecordRef_t));
	if (!rref) { free(hdr); return kDNSServiceErr_NoMemory; }
	rref->app_context = NULL;
	rref->app_callback = NULL;
	rref->record_index = sdRef->max_index++;
	rref->sdr = sdRef;
	*RecordRef = rref;
	hdr->client_context.context = rref;
	hdr->reg_index = rref->record_index;
	return deliver_request(hdr, sdRef, 0);		// Will free hdr for us
	}

//DNSRecordRef returned by DNSServiceRegisterRecord or DNSServiceAddRecord
DNSServiceErrorType DNSSD_API DNSServiceUpdateRecord
	(
	DNSServiceRef    sdRef,
	DNSRecordRef     RecordRef,
	DNSServiceFlags  flags,
	uint16_t         rdlen,
	const void      *rdata,
	uint32_t         ttl
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
	if (!hdr) return kDNSServiceErr_NoMemory;
	hdr->reg_index = RecordRef ? RecordRef->record_index : TXT_RECORD_INDEX;
	put_flags(flags, &ptr);
	put_uint16(rdlen, &ptr);
	put_rdata(rdlen, rdata, &ptr);
	put_uint32(ttl, &ptr);
	return deliver_request(hdr, sdRef, 0);		// Will free hdr for us
	}

DNSServiceErrorType DNSSD_API DNSServiceRemoveRecord
	(
	DNSServiceRef    sdRef,
	DNSRecordRef     RecordRef,
	DNSServiceFlags  flags
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
	if (!hdr) return kDNSServiceErr_NoMemory;
	hdr->reg_index = RecordRef->record_index;
	put_flags(flags, &ptr);
	err = deliver_request(hdr, sdRef, 0);		// Will free hdr for us
	if (!err) free(RecordRef);
	return err;
	}

DNSServiceErrorType DNSSD_API DNSServiceReconfirmRecord
	(
	DNSServiceFlags  flags,
	uint32_t         interfaceIndex,
	const char      *fullname,
	uint16_t         rrtype,
	uint16_t         rrclass,
	uint16_t         rdlen,
	const void      *rdata
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
	if (!tmp) return(kDNSServiceErr_ServiceNotRunning);
	hdr = create_hdr(reconfirm_record_request, &len, &ptr, 1);
	if (!hdr) return kDNSServiceErr_NoMemory;

	put_flags(flags, &ptr);
	put_uint32(interfaceIndex, &ptr);
	put_string(fullname, &ptr);
	put_uint16(rrtype, &ptr);
	put_uint16(rrclass, &ptr);
	put_uint16(rdlen, &ptr);
	put_rdata(rdlen, rdata, &ptr);
	ConvertHeaderBytes(hdr);
	write_all(tmp->sockfd, (char *)hdr, (int) len);
	free(hdr);
	DNSServiceRefDeallocate(tmp);
	return(kDNSServiceErr_NoError);
	}

static void handle_port_mapping_create_response(DNSServiceRef sdr, ipc_msg_hdr *hdr, char *data)
	{
	DNSServiceFlags     flags = get_flags(&data);
	uint32_t            ifi   = get_uint32(&data);
	DNSServiceErrorType err   = get_error_code(&data);

	union { uint32_t l; u_char b[4]; } addr;
	uint8_t protocol;
	union { uint16_t s; u_char b[2]; } privatePort;
	union { uint16_t s; u_char b[2]; } publicPort;
	uint32_t ttl;

	(void)hdr; 		//unused

	addr       .b[0] = *data++;
	addr       .b[1] = *data++;
	addr       .b[2] = *data++;
	addr       .b[3] = *data++;
	protocol         = *data++;
	privatePort.b[0] = *data++;
	privatePort.b[1] = *data++;
	publicPort .b[0] = *data++;
	publicPort .b[1] = *data++;
	ttl              = get_uint32(&data);

	((DNSServiceNATPortMappingReply)sdr->app_callback)(sdr, flags, ifi, err, addr.l, protocol, privatePort.s, publicPort.s, ttl, sdr->app_context);
	}

DNSServiceErrorType DNSSD_API DNSServiceNATPortMappingCreate
	(
	DNSServiceRef                       *sdRef,
	DNSServiceFlags                     flags,
	uint32_t                            interfaceIndex,
	uint32_t                            protocol,     /* TCP and/or UDP */
	uint16_t                            privatePortInNetworkByteOrder,
	uint16_t                            publicPortInNetworkByteOrder,
	uint32_t                            ttl,          /* time to live in seconds */
	DNSServiceNATPortMappingReply       callBack,
	void                                *context      /* may be NULL */
	)
	{
	char *ptr;
	size_t len;
	ipc_msg_hdr *hdr;
	DNSServiceRef sdr;
	DNSServiceErrorType err;
	union { uint16_t s; u_char b[2]; } privatePort = { privatePortInNetworkByteOrder };
	union { uint16_t s; u_char b[2]; } publicPort  = { publicPortInNetworkByteOrder };

	if (!sdRef) return kDNSServiceErr_BadParam;
	*sdRef = NULL;

	len = sizeof(flags);
	len += sizeof(interfaceIndex);
	len += sizeof(protocol);
	len += sizeof(privatePort);
	len += sizeof(publicPort);
	len += sizeof(ttl);

	hdr = create_hdr(port_mapping_create_request, &len, &ptr, 1);
	if (!hdr) return kDNSServiceErr_NoMemory;

	put_flags(flags, &ptr);
	put_uint32(interfaceIndex, &ptr);
	*ptr++ = protocol;
	*ptr++ = privatePort.b[0];
	*ptr++ = privatePort.b[1];
	*ptr++ = publicPort .b[0];
	*ptr++ = publicPort .b[1];
	put_uint32(ttl, &ptr);

	sdr = connect_to_server();
	if (!sdr) { free(hdr); return kDNSServiceErr_ServiceNotRunning; }

	err = deliver_request(hdr, sdr, 1);		// Will free hdr for us
	if (err)
		DNSServiceRefDeallocate(sdr);
	else
		{
		sdr->op = port_mapping_create_request;
		sdr->process_reply = handle_port_mapping_create_response;
		sdr->app_callback = callBack;
		sdr->app_context = context;
		*sdRef = sdr;
		}
	return err;
	}
