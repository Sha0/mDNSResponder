/*
 * Copyright (c) 2002-2004 Apple Computer, Inc. All rights reserved.
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

$Log: dnsextd.c,v $
Revision 1.2  2004/08/24 23:27:57  cheshire
Fixes for Linux compatibility:
Don't use strings.h
Don't assume SIGINFO
Don't try to set servaddr.sin_len on platforms that don't have sa_len

Revision 1.1  2004/08/11 00:43:26  ksekar
<rdar://problem/3722542>: DNS Extension daemon for DNS Update Lease

*/

#include "../mDNSCore/mDNSClientAPI.h"
#include "../mDNSCore/DNSCommon.h"
#include "../mDNSCore/mDNS.c"
//!!!KRS we #include mDNS.c for the various constants defined there  - we should move these to DNSCommon.h

#include <signal.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <syslog.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>

//
// Constants
//

#define LOOPBACK "127.0.0.1"
#define NS_PORT 53
#define DAEMON_PORT 5355                // default, may be overridden via command line argument
#define LISTENQ 128                     // tcp connection backlog
#define RECV_BUFLEN 9000                
#define LEASETABLE_INIT_NBUCKETS 256    // initial hashtable size (doubles as table fills)
#define EXPIRATION_INTERVAL 300          // check for expired records every 5 minutes
#define SRV_TTL 7200                    // TTL For _dns-update SRV records

#ifdef SIGINFO
#define INFO_SIGNAL SIGINFO
#else
#define INFO_SIGNAL SIGUSR1
#endif

//
// Data Structures
// Structs/fields that must be locked for thread safety are explicitly commented
//

typedef struct
	{
    struct sockaddr_in src;
    size_t len;
    DNSMessage msg;
    // Note: extra storage for oversized (TCP) messages goes here
	} PktMsg;

typedef struct RRTableElem
	{
    struct RRTableElem *next;
    struct sockaddr_in cli;   // client's source address
    long expire;              // expiration time, in seconds since epoch
    domainname zone;          // from zone field of update message
    CacheRecord rr;           // last field in struct allows for allocation of oversized RRs
	} RRTableElem;

// daemon-wide information
typedef struct 
	{
    // server variables - read only after initialization (no locking)
    struct in_addr saddr;      // server address
    domainname zone;           // zone being updated
    int tcpsd;                 // listening TCP socket
    int udpsd;                 // listening UDP socket

    // daemon variables - read only after initialization (no locking)
    uDNS_AuthInfo *AuthInfo;   // linked list of keys for signing deletion updates
    mDNSIPPort port;           // listening port

    // lease table variables (locked via mutex after initialization)
    RRTableElem **table;       // hashtable for records with leases
    pthread_mutex_t tablelock; // mutex for lease table
    mDNSs32 nbuckets;          // buckets allocated
    mDNSs32 nelems;            // elements in table
	} DaemonInfo;

// args passed to UDP request handler thread as void*
typedef struct
	{
    PktMsg pkt;
    struct sockaddr_in cliaddr;
    DaemonInfo *d;
	} UDPRequestArgs;

// args passed to TCP request handler thread as void*
typedef struct
	{
    int sd;                     // socket connected to client
    struct sockaddr_in cliaddr; 
    DaemonInfo *d;
	} TCPRequestArgs;

//
// Global Variables
//

// booleans to determine runtime output
// read-only after initialization (no mutex protection)
static mDNSBool foreground = 0;
static mDNSBool verbose = 0;

// globals set via signal handler (accessed exclusively by main select loop and signal handler)
static mDNSBool terminate = 0;
static mDNSBool dumptable = 0;

//
// Logging Routines
// Log messages are delivered to syslog unless -f option specified
//

// common message logging subroutine
mDNSlocal void PrintLog(const char *buffer)
	{
	if (foreground)	
		{
		fprintf(stderr,"%s\n", buffer);
		fflush(stderr);
		}
	else				
		{
		openlog("dnsextd", LOG_CONS | LOG_PERROR, LOG_DAEMON);
		syslog(LOG_ERR, "%s", buffer);
		closelog();
		}
	}

// Verbose Logging (conditional on -v option)
mDNSlocal void VLog(const char *format, ...)
	{
   	unsigned char buffer[512];
	va_list ptr;

	if (!verbose) return;
	va_start(ptr,format);
	buffer[mDNS_vsnprintf((char *)buffer, sizeof(buffer), format, ptr)] = 0;
	va_end(ptr);
 	PrintLog(buffer);
	}

// Unconditional Logging
mDNSlocal void Log(const char *format, ...)
	{
   	unsigned char buffer[512];
	va_list ptr;

	va_start(ptr,format);
	buffer[mDNS_vsnprintf((char *)buffer, sizeof(buffer), format, ptr)] = 0;
	va_end(ptr);
 	PrintLog(buffer);
	}

// Error Logging
// prints message "dnsextd <function>: <operation> - <error message>" 
// must be compiled w/ -D_REENTRANT  for thread-safe errno usage
mDNSlocal void LogErr(const char *fn, const char *operation)
	{
	char buf[512];
	char errbuf[256];

	if (strerror_r(errno, errbuf, 256))
		sprintf(buf, "errno %d", errno);  // strerror failed - just print number
	snprintf(buf, 512, "%s: %s - %s", fn, operation, errbuf);
	PrintLog(buf);
	}

//
// Networking Utility Routines
//

// create a socket connected to nameserver
// caller terminates connection via close()
mDNSlocal int ConnectToServer(DaemonInfo *d)
	{
	struct sockaddr_in servaddr;
	int sd;
	
	bzero(&servaddr, sizeof(servaddr));
	if (d->saddr.s_addr) servaddr.sin_addr = d->saddr;
	else                 inet_pton(AF_INET, LOOPBACK, &d->saddr);  // use loopback if server not explicitly specified			
	servaddr.sin_port = htons(NS_PORT);
	servaddr.sin_family = AF_INET;
#ifndef NOT_HAVE_SA_LEN
	servaddr.sin_len = sizeof(servaddr); 
#endif
	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd < 0) { LogErr("ConnectToServer", "socket");  return -1; }
	if (connect(sd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) { LogErr("ConnectToServer", "connect"); return -1; }
	return sd;
	}

// send an entire block of data over a connected socket, blocking if buffers are full
mDNSlocal int MySend(int sd, const void *msg, int len)
	{
	int n, nsent = 0;

	while (nsent < len)
		{
		n = send(sd, (char *)msg + nsent, len - nsent, 0);
		if (n < 0) { LogErr("MySend", "send");  return -1; }
		nsent += n;
		}
	return 0;
	}

// Transmit a DNS message, prefixed by its length, over TCP, blocking if necessary
mDNSlocal int SendTCPMsg(int sd, PktMsg *pkt)
	{
	uint16_t len;

	// send the lenth, in network byte order
	len = htons((uint16_t)pkt->len);
	if (MySend(sd, &len, sizeof(len)) < 0) return -1;

	// send the message
	return MySend(sd, &pkt->msg, pkt->len);
	}

// Return a DNS Message (allocated with malloc) read off of a TCP socket, or NULL on failure
// PktMsg returned contains sufficient extra storage for a Lease OPT RR
mDNSlocal PktMsg *ReadTCPMsg(int sd)
	{	
	int nread, allocsize;
	uint16_t msglen = 0;	
	PktMsg *pkt = NULL;
	int srclen;
	
	nread = recv(sd, &msglen, sizeof(msglen), MSG_WAITALL);
	if (nread < 0) { LogErr("TCPRequestForkFn", "recv"); goto error; }
	if (nread != sizeof(msglen)) { Log("Could not read length field of message"); goto error; }	

	// buffer extra space to add an OPT RR
	if (msglen > sizeof(DNSMessage)) allocsize = sizeof(PktMsg) - sizeof(DNSMessage) + msglen;
	else                             allocsize = sizeof(PktMsg);
	
	pkt = malloc(allocsize);
	if (!pkt) { LogErr("ReadTCPMsg", "malloc"); goto error; }
	bzero(pkt, sizeof(*pkt));
	pkt->len = msglen;
	srclen = sizeof(pkt->src);
	if (getpeername(sd, (struct sockaddr *)&pkt->src, &srclen) ||
		srclen != sizeof(pkt->src)) { LogErr("ReadTCPMsg", "getpeername"); bzero(&pkt->src, sizeof(pkt->src)); }
	nread = recv(sd, &pkt->msg, msglen, MSG_WAITALL);
	if (nread < 0) { LogErr("TCPRequestForkFn", "recv"); goto error; }
	if (nread != msglen) { Log("Could not read entire message"); goto error; }
	if (pkt->len < sizeof(DNSMessageHeader))
		{ Log("ReadTCPMsg: Message too short (%d bytes)", pkt->len);  goto error; }	
	return pkt;

	error:
	if (pkt) free(pkt);
	return NULL;
	}

// Convert DNS Message Header from Network to Host byte order
mDNSlocal void HdrNToH(PktMsg *pkt)
	{
	// Read the integer parts which are in IETF byte-order (MSB first, LSB second)
	mDNSu8 *ptr = (mDNSu8 *)&pkt->msg.h.numQuestions;
	pkt->msg.h.numQuestions   = (mDNSu16)((mDNSu16)ptr[0] <<  8 | ptr[1]);
	pkt->msg.h.numAnswers     = (mDNSu16)((mDNSu16)ptr[2] <<  8 | ptr[3]);
	pkt->msg.h.numAuthorities = (mDNSu16)((mDNSu16)ptr[4] <<  8 | ptr[5]);
	pkt->msg.h.numAdditionals = (mDNSu16)((mDNSu16)ptr[6] <<  8 | ptr[7]);
	}

// Convert DNS Message Header from Host to Network byte order
mDNSlocal void HdrHToN(PktMsg *pkt)
	{
	mDNSu16 numQuestions   = pkt->msg.h.numQuestions;
	mDNSu16 numAnswers     = pkt->msg.h.numAnswers;
	mDNSu16 numAuthorities = pkt->msg.h.numAuthorities;
	mDNSu16 numAdditionals = pkt->msg.h.numAdditionals;
	mDNSu8 *ptr = (mDNSu8 *)&pkt->msg.h.numQuestions;

	// Put all the integer values in IETF byte-order (MSB first, LSB second)
	*ptr++ = (mDNSu8)(numQuestions   >> 8);
	*ptr++ = (mDNSu8)(numQuestions   &  0xFF);
	*ptr++ = (mDNSu8)(numAnswers     >> 8);
	*ptr++ = (mDNSu8)(numAnswers     &  0xFF);
	*ptr++ = (mDNSu8)(numAuthorities >> 8);
	*ptr++ = (mDNSu8)(numAuthorities &  0xFF);
	*ptr++ = (mDNSu8)(numAdditionals >> 8);
	*ptr++ = (mDNSu8)(numAdditionals &  0xFF);
	}

//
// Dynamic Update Utility Routines
//

// Get the lease life of records in a dynamic update
// returns -1 on error or if no lease present
mDNSlocal mDNSs32 GetPktLease(PktMsg *pkt)
	{
	mDNSs32 lease = -1;
	const mDNSu8 *ptr = NULL, *end = (mDNSu8 *)&pkt->msg + pkt->len;
	LargeCacheRecord lcr;
	int i;
	
	HdrNToH(pkt);
	ptr = LocateAdditionals(&pkt->msg, end);
	if (ptr) 
		for (i = 0; i < pkt->msg.h.numAdditionals; i++)
			{
			ptr = GetLargeResourceRecord(NULL, &pkt->msg, ptr, end, 0, kDNSRecordTypePacketAdd, &lcr);
			if (!ptr) { Log("Unable to read additional record"); break; }
			if (lcr.r.resrec.rrtype == kDNSType_OPT)
				{
				if (lcr.r.resrec.rdlength < LEASE_OPT_SIZE) continue;
				if (lcr.r.resrec.rdata->u.opt.opt != kDNSOpt_Lease) continue;
				lease = (mDNSs32)lcr.r.resrec.rdata->u.opt.OptData.lease;
				break;
				}
			}

	HdrHToN(pkt);
	return lease;
	}

// check if a request and server response complete a successful dynamic update
mDNSlocal mDNSBool SuccessfulUpdateTransaction(PktMsg *request, PktMsg *reply)
	{
	char buf[32];
	char *vlogmsg = NULL;
	
	// check messages
	if (!request || !reply) { vlogmsg = "NULL message"; goto failure; }
	if (request->len < sizeof(DNSMessageHeader) || reply->len < sizeof(DNSMessageHeader)) { vlogmsg = "Malformatted message"; goto failure; }

	// check request operation
	if ((request->msg.h.flags.b[0] & kDNSFlag0_QROP_Mask) != (request->msg.h.flags.b[0] & kDNSFlag0_QROP_Mask))
		{ vlogmsg = "Request opcode not an update"; goto failure; }

	// check result
	if ((reply->msg.h.flags.b[1] & kDNSFlag1_RC)) { vlogmsg = "Reply contains non-zero rcode";  goto failure; }
	if ((reply->msg.h.flags.b[0] & kDNSFlag0_QROP_Mask) != (kDNSFlag0_OP_Update | kDNSFlag0_QR_Response))
		{ vlogmsg = "Reply opcode not an update response"; goto failure; }

	VLog("Successful update from %s", inet_ntop(AF_INET, &request->src.sin_addr, buf, 32));
	return mDNStrue;		

	failure:
	VLog("Request %s: %s", inet_ntop(AF_INET, &request->src.sin_addr, buf, 32), vlogmsg);
	return mDNSfalse;
	}

//
// Lease Hashtable Utility Routines
//

// double hash table size
// caller must lock table prior to invocation
mDNSlocal void RehashTable(DaemonInfo *d)
	{
	RRTableElem *ptr, *tmp, **new;
	int i, bucket, newnbuckets = d->nbuckets * 2;

	new = malloc(sizeof(RRTableElem *) * newnbuckets);
	if (!new) { LogErr("RehashTable", "malloc");  return; }
	bzero(new, newnbuckets * sizeof(RRTableElem *));

	for (i = 0; i < d->nbuckets; i++)
		{
		ptr = d->table[i];
		while (ptr)
			{
			bucket = ptr->rr.resrec.namehash % newnbuckets;
			tmp = ptr;
			ptr = ptr->next;
			tmp->next = new[bucket];
			new[bucket] = tmp;
			}
		}
	d->nbuckets = newnbuckets;
	}

// print entire contents of hashtable, invoked via SIGINFO
mDNSlocal void PrintTable(DaemonInfo *d)
	{
	int i;
	RRTableElem *ptr;
	char rrbuf[80], addrbuf[16];
	struct timeval now;
	int hr, min, sec;

	if (gettimeofday(&now, NULL)) { LogErr("PrintTable", "gettimeofday"); return; }
	if (pthread_mutex_lock(&d->tablelock)) { LogErr("PrintTable", "pthread_mutex_lock"); return; }
	
	Log("Dumping Lease Table Contents (table contains %d resource records)", d->nelems);
	for (i = 0; i < d->nbuckets; i++)
		{
		for (ptr = d->table[i]; ptr; ptr = ptr->next)
			{
			hr = ((ptr->expire - now.tv_sec) / 60) / 60;
			min = ((ptr->expire - now.tv_sec) / 60) % 60;
			sec = (ptr->expire - now.tv_sec) % 60;
			Log("Update from %s, Expires in %d:%d:%d\n\t%s", inet_ntop(AF_INET, &ptr->cli.sin_addr, addrbuf, 16), hr, min, sec,
				GetRRDisplayString_rdb(&ptr->rr.resrec, &ptr->rr.resrec.rdata->u, rrbuf));
			}
		}
	pthread_mutex_unlock(&d->tablelock);
	}

//
// Startup SRV Registration Routines 
// Register _dns-update._dns-sd._udp/_tcp.<zone> SRV records indicating the port on which
// the daemon accepts requests  
//

// delete all RRS of a given name/type
mDNSlocal mDNSu8 *putRRSetDeletion(DNSMessage *msg, mDNSu8 *ptr, mDNSu8 *limit,  ResourceRecord *rr)
	{
	ptr = putDomainNameAsLabels(msg, ptr, limit, &rr->name);
	if (!ptr || ptr + 10 >= limit) return NULL;  // out of space
	ptr[0] = (mDNSu8)(rr->rrtype  >> 8);
	ptr[1] = (mDNSu8)(rr->rrtype  &  0xFF);
	ptr[2] = (mDNSu8)((mDNSu16)kDNSQClass_ANY >> 8);
	ptr[3] = (mDNSu8)((mDNSu16)kDNSQClass_ANY &  0xFF);
	bzero(ptr+4, sizeof(rr->rroriginalttl) + sizeof(rr->rdlength)); // zero ttl/rdata
	msg->h.mDNS_numUpdates++;
	return ptr + 10;
	}

// perform dynamic update.
// specify deletion by passing false for the register parameter, otherwise register the records.
mDNSlocal int UpdateSRV(DaemonInfo *d, mDNSBool registration)
	{
	int sd = -1;
	mDNSOpaque16 id;
	PktMsg pkt;
	mDNSu8 *ptr = pkt.msg.data;
	mDNSu8 *end = (mDNSu8 *)&pkt.msg + sizeof(DNSMessage);
	mDNSu16 nAdditHBO;  // num additionas, in host byte order, required by message digest routine
	char hostname[1024], buf[80];
	PktMsg *reply = NULL;
	AuthRecord rr;
	int result = -1;
	
	// format the SRV record
	mDNS_SetupResourceRecord(&rr, NULL, 0, kDNSType_SRV, SRV_TTL, kDNSRecordTypeUnique, NULL, NULL);
	rr.resrec.rrclass = kDNSClass_IN;
	rr.resrec.rdata->u.srv.priority = 0;
	rr.resrec.rdata->u.srv.weight = 0;
	rr.resrec.rdata->u.srv.port.NotAnInteger = d->port.NotAnInteger;
	if (!gethostname(hostname, 1024) < 0 || MakeDomainNameFromDNSNameString(&rr.resrec.rdata->u.srv.target, hostname))
		rr.resrec.rdata->u.srv.target.c[0] = '\0';

	// Initialize message
	id.NotAnInteger = 0;
	InitializeDNSMessage(&pkt.msg.h, id, UpdateReqFlags);
	pkt.src.sin_addr.s_addr = htonl(INADDR_ANY); // address field set solely for verbose logging in subroutines
	pkt.src.sin_family = AF_INET;
	
	// format message body
	ptr = putZone(&pkt.msg, ptr, end, &d->zone, mDNSOpaque16fromIntVal(kDNSClass_IN));
	if (!ptr) goto end;

	// do _udp
	MakeDomainNameFromDNSNameString(&rr.resrec.name, "_dns-update._udp.");
	strcpy(rr.resrec.name.c + strlen(rr.resrec.name.c), d->zone.c);
	VLog("%s  %s", registration ? "Registering SRV record" : "Deleting existing RRSet",
		 GetRRDisplayString_rdb(&rr.resrec, &rr.resrec.rdata->u, buf));
	if (registration) ptr = PutResourceRecord(&pkt.msg, ptr, &pkt.msg.h.mDNS_numUpdates, &rr.resrec);
	else              ptr = putRRSetDeletion(&pkt.msg, ptr, end, &rr.resrec);
	if (!ptr) goto end;
	
	// do _tcp
	MakeDomainNameFromDNSNameString(&rr.resrec.name, "_dns-update._tcp.");
	strcpy(rr.resrec.name.c + strlen(rr.resrec.name.c), d->zone.c);
	VLog("%s  %s", registration ? "Registering SRV record" : "Deleting existing RRSet",
		 GetRRDisplayString_rdb(&rr.resrec, &rr.resrec.rdata->u, buf));	
	if (registration) ptr = PutResourceRecord(&pkt.msg, ptr, &pkt.msg.h.mDNS_numUpdates, &rr.resrec);
	else              ptr = putRRSetDeletion(&pkt.msg, ptr, end, &rr.resrec);
	if (!ptr) goto end;
	
	nAdditHBO = pkt.msg.h.numAdditionals;
	HdrHToN(&pkt);
	if (d->AuthInfo)
		{
		ptr = DNSDigest_SignMessage(&pkt.msg, &ptr, &nAdditHBO, d->AuthInfo);
		if (!ptr) goto end;
		}
	pkt.len = ptr - (mDNSu8 *)&pkt.msg;
	
	// send message, receive reply
	sd = ConnectToServer(d);
	if (sd < 0) { Log("UpdateSRV: ConnectToServer failed"); goto end; }  
	if (SendTCPMsg(sd, &pkt)) { Log("UpdateSRV: SendTCPMsg failed"); }
	reply = ReadTCPMsg(sd);
	if (!SuccessfulUpdateTransaction(&pkt, reply))
		Log("SRV record registration failed with rcode %d", reply->msg.h.flags.b[1] & kDNSFlag1_RC);
	else result = 0;
	
	end:
	if (!ptr) { Log("UpdateSRV: Error constructing lease expiration update"); }
	if (sd >= 0) close(sd);	
	if (reply) free(reply);
	return result;
   	}

// wrapper routines/macros
#define ClearUpdateSRV(d) UpdateSRV(d, 0)

// clear any existing records prior to registration
mDNSlocal int SetUpdateSRV(DaemonInfo *d)
	{
	int err;

	err = ClearUpdateSRV(d);         // clear any existing record
	if (!err) err = UpdateSRV(d, 1);
	return err;
	}

//
// Argument Parsing and Configuration
//

// read authentication information for a zone from command line argument
// global optind corresponds to keyname argument on entry
mDNSlocal int ReadAuthKey(int argc, char *argv[], DaemonInfo *d)
	{
	uDNS_AuthInfo *auth = NULL;
	char keybuf[512];
	mDNSs32 keylen;
	
	auth = malloc(sizeof(*auth));
	if (!auth) { perror("ReadAuthKey, malloc");  goto error; }
	auth->next = NULL;
	if (argc < optind + 2) return -1;  // keyname + zone, secret 
	if (!MakeDomainNameFromDNSNameString(&auth->keyname, optarg))
		{ fprintf(stderr, "Bad key name %s", optarg); goto error; }
	if (!MakeDomainNameFromDNSNameString(&auth->zone, argv[optind++]))
		{ fprintf(stderr, "Bad zone %s", argv[optind-1]); goto error; }

	keylen = DNSDigest_Base64ToBin(argv[optind++], keybuf, 512);
	if (keylen < 0)
		{ fprintf(stderr, "Bad shared secret %s (must be base-64 encoded string)", argv[optind-1]); goto error; }
	DNSDigest_ConstructHMACKey(auth, keybuf, (mDNSu32)keylen);
	d->AuthInfo = auth;
	return 0;

	error:
	if (auth) free(auth);
	return -1;	
	}

mDNSlocal int SetPort(DaemonInfo *d, char *PortAsString)
	{
	long l;

	l = strtol(PortAsString, NULL, 10);                    // convert string to long
	if ((!l && errno == EINVAL) || l > 65535) return -1;   // error check conversion
	d->port.NotAnInteger = htons((uint16_t)l);             // set to network byte order
	return 0;
	}
	
mDNSlocal void PrintUsage(void)
	{
	fprintf(stderr, "Usage: dnsextd <zone> [-vf] [ -s server ] [-k zone keyname secret] ...\n"
			"Use \"dnsextd -h\" for help\n");
	}

mDNSlocal void PrintHelp(void)
	{
	PrintUsage();

	fprintf(stderr,
			"dnsextd is a daemon that implements Dynamic DNS Update Leases, including those\n"
			"used in DNS Service Discovery, on behalf of name servers that do not natively\n"
			"support Dynamic Update leases.  (See dns-sd.org for more info on DNS Service \n"
			"Discovery and Update Leases.)\n\n"

			"The zone specified is the domain for which updates with leases may be issued.\n"
			"The server the daemon communicates with must be the primary master for this zone.\n\n"

			"The options are as follows:\n\n"

			"-f    Run daemon in foreground.\n\n"

			"-h    Print help.\n\n"

			"-k    Specify TSIG authentication key for dynamic updates from daemon to name server.\n"
			"      -k option is followed by the name of the zone the key applies to, the name of\n"
			"      the key, and the shared secret, as a base64 encoded string.  This option causes\n"
			"      updates deleting expired records to be signed - it does not affect updates sent\n"
			"      from clients, which are responsible for signing their own updates.\n\n"

			
			"-s    Specify address (IPv4 address in dotted-decimal notation) of Primary Master\n"
			"      name server.  Defaults to loopback (127.0.0.1), i.e. daemon and name server\n"
			"      running on the same machine.\n\n"

			"-v    Verbose output.\n\n"
		);		   
	}

// Note: ProcessArgs called before process is daemonized, and therefore must open no descriptors
// returns 0 (success) if program is to continue execution
// output control arguments (-f, -v) do not affect this routine
mDNSlocal int ProcessArgs(int argc, char *argv[], DaemonInfo *d)
	{
	int opt;

	if (argc < 2) goto arg_error;
	if (!MakeDomainNameFromDNSNameString(&d->zone, argv[1])) { fprintf(stderr, "Bad zone %s", argv[1]); goto arg_error; }
	optind++;
	
	d->port.NotAnInteger = htons(DAEMON_PORT);  // default, may be overriden by command option
	while ((opt = getopt(argc, argv, "p:hfvs:k:")) != -1)
		{
		switch(opt)
			{
			case 'p': if (SetPort(d, optarg) < 0) goto arg_error;
        			  break;

			case 'h': PrintHelp();    return -1;
			case 'f': foreground = 1; break;
			case 'v': verbose = 1;    break;
			case 's': if (!inet_pton(AF_INET, optarg, &d->saddr)) goto arg_error;
				      break;
			case 'k': if (ReadAuthKey(argc, argv, d) < 0) goto arg_error;
				      break;
			default:  goto arg_error;				
			}
		}
	return 0;
	
	arg_error:
	PrintUsage();
	return -1;
	}


//
// Initialization Routines
//

// Allocate memory, initialize locks and bookkeeping variables
mDNSlocal int InitLeaseTable(DaemonInfo *d)
	{
	if (pthread_mutex_init(&d->tablelock, NULL)) { LogErr("InitLeaseTable", "pthread_mutex_init"); return -1; }
	d->nbuckets = LEASETABLE_INIT_NBUCKETS;
	d->nelems = 0;
	d->table = malloc(sizeof(RRTableElem *) * LEASETABLE_INIT_NBUCKETS);
	if (!d->table) { LogErr("InitLeaseTable", "malloc"); return -1; }
	bzero(d->table, sizeof(RRTableElem *) * LEASETABLE_INIT_NBUCKETS);
	return 0;
	}
mDNSlocal int SetupSockets(DaemonInfo *daemon)
	{
	struct sockaddr_in daddr;
	
	// set up sockets on which we receive requests
	bzero(&daddr, sizeof(daddr));
	daddr.sin_family = AF_INET;
	daddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (daemon->port.NotAnInteger) daddr.sin_port = daemon->port.NotAnInteger;
	else                           daddr.sin_port = htons(DAEMON_PORT);
	
	daemon->tcpsd = socket(AF_INET, SOCK_STREAM, 0);
	if (!daemon->tcpsd) { LogErr("SetupSockets", "socket");  return -1; }
	if (bind(daemon->tcpsd, (struct sockaddr *)&daddr, sizeof(daddr)) < 0) { LogErr("SetupSockets", "bind"); return -1; }
	if (listen(daemon->tcpsd, LISTENQ) < 0) { LogErr("SetupSockets", "listen"); return -1; }

	daemon->udpsd = socket(AF_INET, SOCK_DGRAM, 0);
	if (!daemon->udpsd) { LogErr("SetupSockets", "socket");  return -1; }
	if (bind(daemon->udpsd, (struct sockaddr *)&daddr, sizeof(daddr)) < 0) { LogErr("SetupSockets", "bind"); return -1; }

	return 0;
	}

//
// periodic table updates
//

// Delete a resource record from the nameserver via a dynamic update
mDNSlocal void DeleteRecord(DaemonInfo *d, CacheRecord *rr, domainname *zone)
	{
	int sd = -1;
	mDNSOpaque16 id;
	PktMsg pkt;
	mDNSu8 *ptr = pkt.msg.data;
	mDNSu8 *end = (mDNSu8 *)&pkt.msg + sizeof(DNSMessage);
	mDNSu16 nAdditHBO;  // num additionas, in host byte order, required by message digest routine
	char buf[80];
	PktMsg *reply = NULL;
	
	VLog("Expiring record %s", GetRRDisplayString_rdb(&rr->resrec, &rr->resrec.rdata->u, buf));	
	sd = ConnectToServer(d);
	if (sd < 0) { Log("DeleteRecord: ConnectToServer failed"); goto end; }
	
	id.NotAnInteger = 0;
	InitializeDNSMessage(&pkt.msg.h, id, UpdateReqFlags);
	
	ptr = putZone(&pkt.msg, ptr, end, zone, mDNSOpaque16fromIntVal(rr->resrec.rrclass));
	if (!ptr) goto end;
	ptr = putDeletionRecord(&pkt.msg, ptr, &rr->resrec);
	if (!ptr) goto end;

	nAdditHBO = pkt.msg.h.numAdditionals;
	HdrHToN(&pkt);

	if (d->AuthInfo)
		{
		ptr = DNSDigest_SignMessage(&pkt.msg, &ptr, &nAdditHBO, d->AuthInfo);
		if (!ptr) goto end;
		}

	pkt.len = ptr - (mDNSu8 *)&pkt.msg;
	pkt.src.sin_addr.s_addr = htonl(INADDR_ANY); // address field set solely for verbose logging in subroutines
	pkt.src.sin_family = AF_INET;
	if (SendTCPMsg(sd, &pkt)) { Log("DeleteRecord: SendTCPMsg failed"); }
	reply = ReadTCPMsg(sd);
	if (!SuccessfulUpdateTransaction(&pkt, reply))
		Log("Expiration update failed with rcode %d", reply->msg.h.flags.b[1] & kDNSFlag1_RC);
					  
	end:
	if (!ptr) { Log("DeleteRecord: Error constructing lease expiration update"); }
	if (sd >= 0) close(sd);	
	if (reply) free(reply);
	}

// iterate over table, deleting expired records
mDNSlocal void DeleteExpiredRecords(DaemonInfo *d)
	{
	int i;
	RRTableElem *ptr, *prev, *fptr;	
	struct timeval now;

	if (gettimeofday(&now, NULL)) { LogErr("DeleteExpiredRecords ", "gettimeofday"); return; }
	if (pthread_mutex_lock(&d->tablelock)) { LogErr("DeleteExpiredRecords", "pthread_mutex_lock"); return; }
	for (i = 0; i < d->nbuckets; i++)
		{
		ptr = d->table[i];
		prev = NULL;
		while (ptr)
			{
			if (ptr->expire - now.tv_sec < 0)
				{
				// delete record from server
				DeleteRecord(d, &ptr->rr, &ptr->zone);
				if (prev) prev->next = ptr->next;
				else d->table[i] = ptr->next;
				fptr = ptr;
				ptr = ptr->next;
				free(fptr);
				d->nelems--;
				}
			else
				{
				prev = ptr;
				ptr = ptr->next;
				}
			}
		}
	pthread_mutex_unlock(&d->tablelock);
	}

//
// main update request handling
//

// Add, delete, or refresh records in table based on contents of a successfully completed dynamic update
mDNSlocal void UpdateLeaseTable(PktMsg *pkt, DaemonInfo *d, mDNSs32 lease)
	{
	RRTableElem *prev, *rptr, *new = NULL;
	int i, bucket;
	LargeCacheRecord lcr;
	const mDNSu8 *ptr, *end;
	struct timeval time;
	DNSQuestion zone;
	char buf[80];
	
	if (pthread_mutex_lock(&d->tablelock)) { LogErr("UpdateLeaseTable", "pthread_mutex_lock"); return; }
	HdrNToH(pkt);
	ptr = pkt->msg.data;
	end = (mDNSu8 *)&pkt->msg + pkt->len;
	ptr = getQuestion(&pkt->msg, ptr, end, 0, &zone);
	if (!ptr) { Log("UpdateLeaseTable: cannot read zone");  goto cleanup; }
	ptr = LocateAuthorities(&pkt->msg, end);
	if (!ptr) { Log("UpdateLeaseTable: Format error");  goto cleanup; }
	
	for (i = 0; i < pkt->msg.h.mDNS_numUpdates; i++)
		{
		ptr = GetLargeResourceRecord(NULL, &pkt->msg, ptr, end, 0, kDNSRecordTypePacketAns, &lcr);
		if (!ptr) { Log("UpdateLeaseTable: GetLargeResourceRecord returned NULL"); goto cleanup; }
		//!!!KRS we should include rdata in hash here
		bucket = lcr.r.resrec.namehash % d->nbuckets;
		
		// look for RR in table
		prev = NULL;
		rptr = d->table[bucket];
		while (rptr)
			{
			if (SameResourceRecord(&rptr->rr.resrec, &lcr.r.resrec)) break;
			prev = rptr;
			rptr = rptr->next;
			}
		
		if (rptr)
			{
			// Record is already in table
			if (!lcr.r.resrec.rroriginalttl && lcr.r.resrec.rrclass == kDNSClass_NONE)
				{
				// deletion record				
				VLog("Received deletion update for %s", GetRRDisplayString_rdb(&lcr.r.resrec, &lcr.r.resrec.rdata->u, buf));
				if (prev) prev->next = rptr->next;
				else d->table[bucket] = rptr->next;
				free(rptr);
				d->nelems--;
				}
			else
				{
				// refresh
				if (lease < 0)
					{
					Log("Update for record %s already in lease table with no refresh lease specified",
						GetRRDisplayString_rdb(&lcr.r.resrec, &lcr.r.resrec.rdata->u, buf));
					}
				else
					{
					if (gettimeofday(&time, NULL)) { LogErr("UpdateLeaseTable", "gettimeofday"); goto cleanup; }
					rptr->expire = time.tv_sec + (unsigned)lease;
					VLog("Refreshing lease for %s", GetRRDisplayString_rdb(&lcr.r.resrec, &lcr.r.resrec.rdata->u, buf));					
					}
				}
			}
		else if (lease > 0)
			{
			// New record - add to table
			if (d->nelems > d->nbuckets) RehashTable(d);
			if (gettimeofday(&time, NULL)) { LogErr("UpdateLeaseTable", "gettimeofday"); goto cleanup; }
			new = malloc(sizeof(RRTableElem) + lcr.r.resrec.rdlength - InlineCacheRDSize);
			if (!new) { LogErr("UpdateLeaseTable", "malloc"); goto cleanup; }
			memcpy(&new->rr, &lcr.r, sizeof(CacheRecord) + lcr.r.resrec.rdlength - InlineCacheRDSize);
			new->rr.resrec.rdata = (RData *)&new->rr.rdatastorage;
			new->expire = time.tv_sec + (unsigned)lease;
			new->cli.sin_addr = pkt->src.sin_addr;
			strcpy(new->zone.c, zone.qname.c);
			new->next = d->table[bucket];
			d->table[bucket] = new;
			d->nelems++;
			VLog("Adding update for %s to lease table", GetRRDisplayString_rdb(&lcr.r.resrec, &lcr.r.resrec.rdata->u, buf));
			}
		}
					
	cleanup:
	pthread_mutex_unlock(&d->tablelock);
	HdrHToN(pkt);
	}

// Given a successful reply from a server, create a new reply that contains lease information
// Replies are currently not signed !!!KRS change this
mDNSlocal PktMsg *FormatLeaseReply(DaemonInfo *d, PktMsg *orig, mDNSs32 lease)
	{
	PktMsg *reply;
	mDNSu8 *ptr, *end;
	mDNSOpaque16 flags;

	(void)d;  //unused
	reply = malloc(sizeof(*reply));
	if (!reply) { LogErr("FormatLeaseReply", "malloc"); return NULL; }
	flags.b[0] = kDNSFlag0_OP_Update | kDNSFlag0_QR_Response;
	flags.b[1] = 0;
 
	InitializeDNSMessage(&reply->msg.h, orig->msg.h.id, flags);
	reply->src.sin_addr.s_addr = htonl(INADDR_ANY);            // unused except for log messages
	reply->src.sin_family = AF_INET;
	ptr = reply->msg.data;
	end = (mDNSu8 *)&reply->msg + sizeof(DNSMessage);
	ptr = putUpdateLease(&reply->msg, ptr, lease);
	if (!ptr) { Log("FormatLeaseReply: putUpdateLease failed"); free(reply); return NULL; }				   
	reply->len = ptr - (mDNSu8 *)&reply->msg;
	return reply;
	}

// pkt is thread-local, not requiring locking
mDNSlocal PktMsg *HandleRequest(PktMsg *pkt, DaemonInfo *d)
	{
	int sd = -1;
	PktMsg *reply = NULL, *LeaseReply;
	mDNSs32 lease;
	char buf[32];
	
	// send msg to server, read reply
	sd = ConnectToServer(d);
	if (sd < 0)
		{ Log("Discarding request from %s due to connection errors", inet_ntop(AF_INET, &pkt->src.sin_addr, buf, 32)); goto cleanup; }
	if (SendTCPMsg(sd, pkt) < 0)
		{ Log("Couldn't relay message from %s to server.  Discarding.", inet_ntop(AF_INET, &pkt->src.sin_addr, buf, 32)); goto cleanup; }
	reply = ReadTCPMsg(sd);
	
	// process reply
	if (!SuccessfulUpdateTransaction(pkt, reply))
		{ VLog("Message from %s not a successful update.", inet_ntop(AF_INET, &pkt->src.sin_addr, buf, 32));  goto cleanup; }	
	lease = GetPktLease(pkt);
	UpdateLeaseTable(pkt, d, lease);
	if (lease > 0)
		{
		LeaseReply = FormatLeaseReply(d, reply, lease);
		if (!LeaseReply) Log("HandleRequest - unable to format lease reply");
		free(reply); 
		reply = LeaseReply;
		}
	cleanup:
	if (sd >= 0) close(sd);
	return reply;
	}

// request handler wrappers for TCP and UDP requests
// (read message off socket, fork thread that invokes main processing routine and handles cleanup)
mDNSlocal void *UDPRequestForkFn(void *vptr)
	{
	char buf[32];
	UDPRequestArgs *req = vptr;
	PktMsg *reply = NULL;
	
	VLog("Received UDP request: %d bytes from %s", req->pkt.len, inet_ntop(AF_INET, &req->pkt.src.sin_addr, buf, 32));  
	//!!!KRS strictly speaking, we shouldn't use TCP for a UDP request because the server may give us a long answer that would require truncation for UDP delivery to client
	reply = HandleRequest(&req->pkt, req->d);
	if (reply)
		{
		if (sendto(req->d->udpsd, &reply->msg, reply->len, 0, (struct sockaddr *)&req->pkt.src, sizeof(req->pkt.src)) != (int)reply->len)
			LogErr("UDPRequestForkFn", "sendto");		
		}

	if (reply) free(reply);		
	free(req);
	return NULL;
	}

//!!!KRS this needs to be changed to use non-blocking sockets
mDNSlocal int RecvUDPRequest(int sd, DaemonInfo *d)
	{
	UDPRequestArgs *req;
	pthread_t tid;
	int clisize = sizeof(req->cliaddr);
	
	req = malloc(sizeof(UDPRequestArgs));
	if (!req) { LogErr("RecvUDPRequest", "malloc"); return -1; }
	bzero(req, sizeof(*req));
	req->d = d;
	//!!!KRS if this read blocks indefinitely, we can run out of threads
	req->pkt.len = recvfrom(sd, &req->pkt.msg, sizeof(req->pkt.msg), 0, (struct sockaddr *)&req->cliaddr, &clisize);
	if ((int)req->pkt.len < 0) { LogErr("RecvUDPRequest", "recvfrom"); free(req); return -1; }
	if (clisize != sizeof(req->cliaddr)) { Log("Client address of unknown size %d", clisize); free(req); return -1; }
	req->pkt.src = req->cliaddr;
	if (pthread_create(&tid, NULL, UDPRequestForkFn, req)) { LogErr("RecvUDPRequest", "pthread_create"); free(req); return -1; }
	return 0;
	}

mDNSlocal void *TCPRequestForkFn(void *vptr)
	{
	TCPRequestArgs *req = vptr;
	PktMsg *in = NULL, *out = NULL;
	char buf[32];
	
    //!!!KRS if this read blocks indefinitely, we can run out of threads
	// read the request
	in = ReadTCPMsg(req->sd);
	if (!in)
		{
		LogMsg("TCPRequestForkFn: Could not read message from %s", inet_ntop(AF_INET, &req->cliaddr.sin_addr, buf, 32));
		goto cleanup;
		}

	VLog("Received TCP request: %d bytes from %s", in->len, inet_ntop(AF_INET, &req->cliaddr.sin_addr, buf, 32));  	
	// create the reply
	out = HandleRequest(in, req->d);
	if (!out)
		{
		LogMsg("TCPRequestForkFn: No reply for client %s", inet_ntop(AF_INET, &req->cliaddr.sin_addr, buf, 32));
		goto cleanup;
		}

	// deliver reply to client
	if (SendTCPMsg(req->sd, out) < 0) 
		{
		LogMsg("TCPRequestForkFn: Unable to send reply to client %s", inet_ntop(AF_INET, &req->cliaddr.sin_addr, buf, 32));
		goto cleanup;
		}
		
	cleanup:
	free(req);
	if (in) free(in);
	if (out) free(out);
	return NULL;	
	}

mDNSlocal int RecvTCPRequest(int sd, DaemonInfo *d)
	{
	TCPRequestArgs *req;
	pthread_t tid;
	int clilen = sizeof(req->cliaddr);
	
	req = malloc(sizeof(TCPRequestArgs));
	if (!req) { LogErr("RecvTCPRequest", "malloc"); return -1; }
	bzero(req, sizeof(*req));
	req->d = d;
	req->sd = accept(sd, (struct sockaddr *)&req->cliaddr, &clilen);
	if (req->sd < 0) { LogErr("RecvTCPRequest", "accept"); return -1; }	
	if (clilen != sizeof(req->cliaddr)) { Log("Client address of unknown size %d", clilen); free(req); return -1; }
	if (pthread_create(&tid, NULL, TCPRequestForkFn, req)) { LogErr("RecvTCPRequest", "pthread_create"); free(req); return -1; }
	return 0;
	}

// main event loop
// listen for incoming requests, periodically check table for expired records, respond to signals
mDNSlocal int ListenForUpdates(DaemonInfo *d)
	{
	int err;
	int maxfdp1;
	fd_set rset;
	struct timeval timenow, timeout = { 0, 0 };
	long NextTableCheck = 0;
	
   	VLog("Listening for requests...");

	FD_ZERO(&rset);
	if (d->tcpsd > d->udpsd) maxfdp1 = d->tcpsd + 1;
	else                     maxfdp1 = d->udpsd + 1;
	
	while(1)
		{
		// expire records if necessary, set timeout
		if (gettimeofday(&timenow, NULL)) { LogErr("ListenForUpdates", "gettimeofday"); return -1; }
		if (timenow.tv_sec >= NextTableCheck)
			{
			DeleteExpiredRecords(d);
			NextTableCheck = timenow.tv_sec + EXPIRATION_INTERVAL;
			}
		timeout.tv_sec = NextTableCheck - timenow.tv_sec;
		
		FD_SET(d->tcpsd, &rset);
		FD_SET(d->udpsd, &rset);
		err = select(maxfdp1, &rset, NULL, NULL, &timeout);		
		if (err < 0)
			{
			if (errno == EINTR)
			{
			if (terminate) { DeleteExpiredRecords(d); return 0; }
			else if (dumptable) { PrintTable(d); dumptable = 0; }
			else Log("Received unhandled signal - continuing"); 
			}
			else
				{
				LogErr("ListenForUpdates", "select"); return -1;
				}
			}
		else
			{
			if (FD_ISSET(d->tcpsd, &rset)) RecvTCPRequest(d->tcpsd, d);
			if (FD_ISSET(d->udpsd, &rset)) RecvUDPRequest(d->udpsd, d); 
			}
		}

	return 0;
	}

// signal handler sets global variables, which are inspected by main event loop
// (select automatically returns due to the handled signal)
mDNSlocal void HndlSignal(int sig)
	{
	if (sig == SIGTERM || sig == SIGINT ) { terminate = 1; return; }
	if (sig == INFO_SIGNAL)               { dumptable = 1; return; }
	}

int main(int argc, char *argv[])
	{
	DaemonInfo d;
	bzero(&d, sizeof(DaemonInfo));
	
	if (signal(SIGTERM,     HndlSignal) == SIG_ERR) perror("Can't catch SIGTERM");
	if (signal(INFO_SIGNAL, HndlSignal) == SIG_ERR) perror("Can't catch SIGINFO");
	if (signal(SIGINT,      HndlSignal) == SIG_ERR) perror("Can't catch SIGINT");
	
	if (ProcessArgs(argc, argv, &d) < 0) exit(1);

	if (!foreground)
		{
		if (daemon(0,0))
			{
			LogErr("main", "daemon");
			fprintf(stderr, "Could not daemonize process, running in foreground");
			foreground = 1;
			}	
		}

	if (InitLeaseTable(&d) < 0) exit(1);
	if (SetupSockets(&d) < 0) exit(1); 
	if (SetUpdateSRV(&d) < 0) exit(1);
		
	ListenForUpdates(&d);               // clear update srv's even if ListenForUpdates returns an error
	if (ClearUpdateSRV(&d) < 0) exit(1);
	exit(0);
	}

