/*
 * Copyright (c) 2002 Apple Computer, Inc. All rights reserved.
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
 *
 * Formatting notes:
 * This code follows the "Whitesmiths style" C indentation rules. Plenty of discussion
 * on C indentation can be found on the web, such as <http://www.kafejo.com/komp/1tbs.htm>,
 * but for the sake of brevity here I will say just this: Curly braces are not syntactially
 * part of an "if" statement; they are the beginning and ending markers of a compound statement;
 * therefore common sense dictates that if they are part of a compound statement then they
 * should be indented to the same level as everything else in that compound statement.
 * Indenting curly braces at the same level as the "if" implies that curly braces are
 * part of the "if", which is false. (This is as misleading as people who write "char* x,y;"
 * thinking that variables x and y are both of type "char*" -- and anyone who doesn't
 * understand why variable y is not of type "char*" just proves the point that poor code
 * layout leads people to unfortunate misunderstandings about how the C language really works.)
 *
 * $Log: NetMonitor.c,v $
 * Revision 1.12  2003/05/26 03:01:28  cheshire
 * <rdar://problem/3268904> sprintf/vsprintf-style functions are unsafe; use snprintf/vsnprintf instead
 *
 * Revision 1.11  2003/05/26 00:48:13  cheshire
 * If mDNS packet contains a non-zero message ID, then display it.
 *
 * Revision 1.10  2003/05/22 01:10:32  cheshire
 * Indicate when the TC bit is set on a query packet
 *
 * Revision 1.9  2003/05/21 03:56:00  cheshire
 * Improve display of Probe queries
 *
 * Revision 1.8  2003/05/09 21:41:56  cheshire
 * Track deletion/goodbye packets as separate category
 *
 * Revision 1.7  2003/05/07 00:16:01  cheshire
 * More detailed decoding of Resource Records
 *
 * Revision 1.6  2003/05/05 21:16:16  cheshire
 * <rdar://problem/3241281> Change timenow from a local variable to a structure member
 *
 * Revision 1.5  2003/04/19 01:16:22  cheshire
 * Add filter option, to monitor only packets from a single specified source address
 *
 * Revision 1.4  2003/04/18 00:45:21  cheshire
 * Distinguish announcements (AN) from deletions (DE)
 *
 * Revision 1.3  2003/04/15 18:26:01  cheshire
 * Added timestamps and help information
 *
 * Revision 1.2  2003/04/04 20:42:02  cheshire
 * Fix broken statistics counting
 *
 * Revision 1.1  2003/04/04 01:37:14  cheshire
 * Added NetMonitor.c
 *
 */

//*************************************************************************************************************
// Incorporate mDNS.c functionality

// We want to use much of the functionality provided by "mDNS.c",
// except we'll steal the packets that would be sent to normal mDNSCoreReceive() routine
#define mDNSCoreReceive __NOT__mDNSCoreReceive__NOT__
#include "mDNS.c"
#undef mDNSCoreReceive

//*************************************************************************************************************
// Headers

#include <stdio.h>			// For printf()
#include <stdlib.h>			// For malloc()
#include <netinet/in.h>		// For INADDR_NONE
#include <arpa/inet.h>		// For inet_addr()
#include <netdb.h>			// For gethostbyname()

#include "mDNSPosix.h"      // Defines the specific types needed to run mDNS on this platform
#include "ExampleClientApp.h"

//*************************************************************************************************************
// Types and structures

enum
	{
	// Primitive operations
	OP_probe        = 0,
	OP_goodbye      = 1,

	// These are meta-categories;
	// Query and Answer operations are actually subdivided into two classes:
	// Browse  query/answer and
	// Resolve query/answer
	OP_query        = 2,
	OP_answer       = 3,

	// The "Browse" variants of query/answer
	OP_browsegroup  = 2,
	OP_browseq      = 2,
	OP_browsea      = 3,

	// The "Resolve" variants of query/answer
	OP_resolvegroup = 4,
	OP_resolveq     = 4,
	OP_resolvea     = 5,

	NumStatOps = 6
	};

typedef struct ActivityStat_struct ActivityStat;
struct ActivityStat_struct
	{
	ActivityStat *next;
	domainname srvtype;
	int printed;
	int totalops;
	int stat[NumStatOps];
	};

//*************************************************************************************************************
// Globals

static mDNS mDNSStorage;						// mDNS core uses this to store its globals
static mDNS_PlatformSupport PlatformStorage;	// Stores this platform's globals

static mDNSAddr FilterAddr;

static int NumProbes;
static int NumGoodbyes;
static int NumQuestions;
static int NumAnswers;
static int NumAdditionals;

static ActivityStat *stats;

//*************************************************************************************************************
// Utilities

// Special version of printf that knows how to print IP addresses, DNS-format name strings, etc.
mDNSlocal void mprintf(const char *format, ...) IS_A_PRINTF_STYLE_FUNCTION(1,2);
mDNSlocal void mprintf(const char *format, ...)
	{
	unsigned char buffer[512];
	va_list ptr;
	va_start(ptr,format);
	buffer[mDNS_vsnprintf((char *)buffer, sizeof(buffer), format, ptr)] = 0;
	va_end(ptr);
	printf("%s", buffer);
	}

//*************************************************************************************************************
// Receive and process packets

mDNSexport mDNSBool ExtractServiceType(const domainname *const fqdn, domainname *const srvtype)
	{
	int i, len;
	const mDNSu8 *src = fqdn->c;
	mDNSu8 *dst = srvtype->c;
	
	len = *src;
	if (len == 0 || len >= 0x40) return(mDNSfalse);
	if (src[1] != '_') src += 1 + len;
	
	len = *src;
	if (len == 0 || len >= 0x40 || src[1] != '_') return(mDNSfalse);
	for (i=0; i<=len; i++) *dst++ = *src++;

	len = *src;
	if (len == 0 || len >= 0x40 || src[1] != '_') return(mDNSfalse);
	for (i=0; i<=len; i++) *dst++ = *src++;

	*dst++ = 0;		// Put the null root label on the end of the service type

	return(mDNStrue);
	}

mDNSlocal void recordstat(domainname *fqdn, int op, mDNSu16 rrtype)
	{
	ActivityStat **s = &stats;
	domainname srvtype;

	if (op != OP_probe)
		{
		if (rrtype == kDNSType_SRV || rrtype == kDNSType_TXT) op = op - OP_browsegroup + OP_resolvegroup;
		else if (rrtype != kDNSType_PTR) return;
		}
	
	if (!ExtractServiceType(fqdn, &srvtype)) return;

	while (*s && !SameDomainName(&(*s)->srvtype, &srvtype)) s = &(*s)->next;
	if (!*s)
		{
		int i;
		*s = malloc(sizeof(ActivityStat));
		if (!*s) exit(-1);
		(*s)->next     = NULL;
		(*s)->srvtype  = srvtype;
		(*s)->printed  = 0;
		(*s)->totalops = 0;
		for (i=0; i<NumStatOps; i++) (*s)->stat[i] = 0;
		}

	(*s)->totalops++;
	(*s)->stat[op]++;
	}

mDNSlocal void printstats(void)
	{
	int i;
	for (i=0; i<20; i++) 
		{
		int max = 0;
		ActivityStat *s, *m = NULL;
		for (s = stats; s; s=s->next)
			if (!s->printed && max < s->totalops)
				{ m = s; max = s->totalops; }
		if (!m) return;
		m->printed = mDNStrue;
		if (i==0) mprintf("%-25sTotal Ops    Probe  Goodbye  BrowseQ  BrowseA ResolveQ ResolveA\n", "Service Type");
		mprintf("%##-25s%8d %8d %8d %8d %8d %8d %8d\n", &m->srvtype, m->totalops, m->stat[OP_probe],
			m->stat[OP_goodbye], m->stat[OP_browseq], m->stat[OP_browsea], m->stat[OP_resolveq], m->stat[OP_resolvea]);
		}
	}

mDNSlocal const mDNSu8 *FindUpdate(mDNS *const m, const DNSMessage *const query, const mDNSu8 *ptr, const mDNSu8 *const end, DNSQuestion *q, ResourceRecord *pktrr)
	{
	int i;
	for (i = 0; i < query->h.numAuthorities; i++)
		{
		ptr = GetResourceRecord(m, query, ptr, end, q->InterfaceID, 0, pktrr, mDNSNULL);
		if (!ptr) break;
		if (ResourceRecordAnswersQuestion(pktrr, q)) return(ptr);
		}
	return(mDNSfalse);
	}

mDNSlocal void DisplayTimestamp(void)
	{
	struct timeval tv;
	struct tm tm;
	gettimeofday(&tv, NULL);
	localtime_r((time_t*)&tv.tv_sec, &tm);
	mprintf("\n%d:%02d:%02d.%06d\n", tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec);
	}

mDNSlocal void DisplayPacketHeader(const DNSMessage *const msg, const mDNSAddr *srcaddr)
	{
	const char *const ptype = (msg->h.flags.b[0] & kDNSFlag0_QR_Response) ? "R" : "Q";

	DisplayTimestamp();
	mprintf("%#-16a -%s-        Q:%3d  Ans:%3d  Auth:%3d  Add:%3d",
		srcaddr, ptype, msg->h.numQuestions, msg->h.numAnswers, msg->h.numAuthorities, msg->h.numAdditionals);

	if (msg->h.id.NotAnInteger) mprintf("  ID:%u", ((mDNSu16)msg->h.id.b[0])<<8 | msg->h.id.b[1]);

	if (msg->h.flags.b[0] & kDNSFlag0_TC)
		{
		if (msg->h.flags.b[0] & kDNSFlag0_QR_Response) mprintf("   Truncated");
		else                                           mprintf("   Truncated (KA list continues in next packet)");
		}
	mprintf("\n");
	}

mDNSlocal void DisplayResourceRecord(const mDNSAddr *const srcaddr, const char *const op, const ResourceRecord *const pktrr)
	{
	RDataBody *rd = &pktrr->rdata->u;
	mDNSu8 *rdend = (mDNSu8 *)rd + pktrr->rdata->RDLength;
	mDNSu8 *t = rd->txt.c;
	mprintf("%#-16a %-4s %-5s %##s", srcaddr, op, DNSTypeName(pktrr->rrtype), pktrr->name.c);

	switch(pktrr->rrtype)
		{
		case kDNSType_A:	mprintf(" -> %.4a", &rd->ip); break;
		case kDNSType_PTR:	mprintf(" -> %##s", &rd->name); break;
		case kDNSType_HINFO:// same as kDNSType_TXT below
		case kDNSType_TXT:	while (t < rdend) { mprintf(" -> %#s", t); t += 1+t[0]; }
							break;
		case kDNSType_AAAA:	mprintf(" -> %.16a", &rd->ipv6); break;
		case kDNSType_SRV:	mprintf(" -> %##s:%d", &rd->srv.target, ((mDNSu16)rd->srv.port.b[0] << 8) | rd->srv.port.b[1]); break;
		}
	
	mprintf("\n");
	}

mDNSlocal void DisplayQuery(mDNS *const m, const DNSMessage *const msg, const mDNSu8 *const end, const mDNSAddr *srcaddr, const mDNSInterfaceID InterfaceID)
	{
	int i;
	const mDNSu8 *ptr = msg->data;
	const mDNSu8 *auth = LocateAuthorities(msg, end);
	const mDNSu8 *p2;
	ResourceRecord pktrr;

	DisplayPacketHeader(msg, srcaddr);

	for (i=0; i<msg->h.numQuestions; i++)
		{
		DNSQuestion q;
		ptr = getQuestion(msg, ptr, end, InterfaceID, &q);
		if (!ptr) { mprintf("%#-16a **** ERROR: FAILED TO READ QUESTION **** \n", srcaddr); return; }
		p2 = FindUpdate(m, msg, auth, end, &q, &pktrr);
		if (p2)
			{
			NumProbes++;
			DisplayResourceRecord(srcaddr, "(P)", &pktrr);
			recordstat(&q.name, OP_probe, q.rrtype);
			auth = p2; // Having displayed this update record, adjust auth forward so we don't display the same one again.
			}
		else
			{
			NumQuestions++;
			mprintf("%#-16a (Q)  %-5s %##s\n", srcaddr, DNSTypeName(q.rrtype), q.name.c);
			recordstat(&q.name, OP_query, q.rrtype);
			}
		}

	for (i=0; i<msg->h.numAnswers; i++)
		{
		ptr = GetResourceRecord(m, msg, ptr, end, InterfaceID, 0, &pktrr, mDNSNULL);
		if (!ptr) { mprintf("%#-16a **** ERROR: FAILED TO READ KNOWN ANSWER **** \n", srcaddr); return; }
		DisplayResourceRecord(srcaddr, "(KA)", &pktrr);
		}
	}

mDNSlocal void DisplayResponse(mDNS *const m, const DNSMessage *const msg, const mDNSu8 *end, const mDNSAddr *srcaddr, const mDNSInterfaceID InterfaceID)
	{
	int i;
	const mDNSu8 *ptr = msg->data;
	ResourceRecord pktrr;

	DisplayPacketHeader(msg, srcaddr);

	for (i=0; i<msg->h.numQuestions; i++)
		{
		DNSQuestion q;
		ptr = getQuestion(msg, ptr, end, InterfaceID, &q);
		if (!ptr) { mprintf("%#-16a **** ERROR: FAILED TO READ QUESTION **** \n", srcaddr); return; }
		mprintf("%#-16a (?)  **** ERROR: SHOULD NOT HAVE Q IN mDNS RESPONSE **** %-5s %##s\n", srcaddr, DNSTypeName(q.rrtype), q.name.c);
		}

	for (i=0; i<msg->h.numAnswers; i++)
		{
		ptr = GetResourceRecord(m, msg, ptr, end, InterfaceID, 0, &pktrr, mDNSNULL);
		if (!ptr) { mprintf("%#-16a **** ERROR: FAILED TO READ ANSWER **** \n", srcaddr); return; }
		if (pktrr.rroriginalttl)
			{
			NumAnswers++;
			DisplayResourceRecord(srcaddr, (pktrr.RecordType & kDNSRecordTypeUniqueMask) ? "(AN)" : "(A+)", &pktrr);
			recordstat(&pktrr.name, OP_answer, pktrr.rrtype);
			}
		else
			{
			NumGoodbyes++;
			DisplayResourceRecord(srcaddr, "(DE)", &pktrr);
			recordstat(&pktrr.name, OP_goodbye, pktrr.rrtype);
			}
		}

	for (i=0; i<msg->h.numAuthorities; i++)
		{
		ptr = GetResourceRecord(m, msg, ptr, end, InterfaceID, 0, &pktrr, mDNSNULL);
		if (!ptr) { mprintf("%#-16a **** ERROR: FAILED TO READ AUTHORITY **** \n", srcaddr); return; }
		mprintf("%#-16a (?)  **** ERROR: SHOULD NOT HAVE AUTHORITY IN mDNS RESPONSE **** %-5s %##s\n", srcaddr, DNSTypeName(pktrr.rrtype), pktrr.name.c);
		}

	for (i=0; i<msg->h.numAdditionals; i++)
		{
		ptr = GetResourceRecord(m, msg, ptr, end, InterfaceID, 0, &pktrr, mDNSNULL);
		if (!ptr) { mprintf("%#-16a **** ERROR: FAILED TO READ ADDITIONAL **** \n", srcaddr); return; }
		NumAdditionals++;
		DisplayResourceRecord(srcaddr, "(AD)", &pktrr);
		}
	}

mDNSexport void mDNSCoreReceive(mDNS *const m, DNSMessage *const msg, const mDNSu8 *const end,
	const mDNSAddr *srcaddr, mDNSIPPort srcport, const mDNSAddr *dstaddr, mDNSIPPort dstport, const mDNSInterfaceID InterfaceID)
	{
	// For now we're only interested in monitoring IPv4 traffic.
	// All IPv6 packets should just be duplicates of the v4 packets.
	if (srcaddr->type == mDNSAddrType_IPv4)
		if (FilterAddr.type == 0 || FilterAddr.addr.ipv4.NotAnInteger == srcaddr->addr.ipv4.NotAnInteger)
			{
			const mDNSu8 StdQ = kDNSFlag0_QR_Query    | kDNSFlag0_OP_StdQuery;
			const mDNSu8 StdR = kDNSFlag0_QR_Response | kDNSFlag0_OP_StdQuery;
			const mDNSu8 QR_OP = (mDNSu8)(msg->h.flags.b[0] & kDNSFlag0_QROP_Mask);
			
			// Read the integer parts which are in IETF byte-order (MSB first, LSB second)
			mDNSu8 *ptr = (mDNSu8 *)&msg->h.numQuestions;
			msg->h.numQuestions   = (mDNSu16)((mDNSu16)ptr[0] <<  8 | ptr[1]);
			msg->h.numAnswers     = (mDNSu16)((mDNSu16)ptr[2] <<  8 | ptr[3]);
			msg->h.numAuthorities = (mDNSu16)((mDNSu16)ptr[4] <<  8 | ptr[5]);
			msg->h.numAdditionals = (mDNSu16)((mDNSu16)ptr[6] <<  8 | ptr[7]);
			
			mDNS_Lock(m);
			if      (QR_OP == StdQ) DisplayQuery   (m, msg, end, srcaddr, InterfaceID);
			else if (QR_OP == StdR) DisplayResponse(m, msg, end, srcaddr, InterfaceID);
			else debugf("Unknown DNS packet type %02X%02X (ignored)", msg->h.flags.b[0], msg->h.flags.b[1]);
			mDNS_Unlock(m);
			}
	}

mDNSlocal mStatus mDNSNetMonitor(void)
	{
	mStatus status = mDNS_Init(&mDNSStorage, &PlatformStorage,
		mDNS_Init_NoCache, mDNS_Init_ZeroCacheSize,
		mDNS_Init_DontAdvertiseLocalAddresses,
		mDNS_Init_NoInitCallback, mDNS_Init_NoInitCallbackContext);
	if (status) return(status);

	ExampleClientEventLoop(&mDNSStorage);

	mprintf("\n\n");
	mprintf("Total New Service Probes:   %6d\n", NumProbes);
	mprintf("Total Goodbye Announcements:%6d\n", NumGoodbyes);
	mprintf("Total Query Questions:      %6d\n", NumQuestions);
	mprintf("Total Answers/Announcements:%6d\n", NumAnswers);
	mprintf("Total Additional Records:   %6d\n", NumAdditionals);
	mprintf("\n");
	printstats();

	mDNS_Close(&mDNSStorage);
	return(0);
	}

mDNSexport int main(int argc, char **argv)
	{
	mStatus status;
	if (argc > 2) goto usage;
	
	if (argc == 2)
		{
		FilterAddr.addr.ipv4.NotAnInteger = inet_addr(argv[1]);
		if (FilterAddr.addr.ipv4.NotAnInteger == INADDR_NONE)	// INADDR_NONE is 0xFFFFFFFF
			{
			struct hostent *h = gethostbyname(argv[1]);
			if (h) FilterAddr.addr.ipv4.NotAnInteger = *(long*)h->h_addr;
			}
		if (FilterAddr.addr.ipv4.NotAnInteger == INADDR_NONE)	// INADDR_NONE is 0xFFFFFFFF
			goto usage;
		FilterAddr.type = mDNSAddrType_IPv4;
		}
	
	status = mDNSNetMonitor();
	if (status) { fprintf(stderr, "%s: mDNSNetMonitor failed %ld\n", argv[0], status); return(status); }
	return(0);

usage:
	fprintf(stderr, "\nmDNS traffic monitor\n");
	fprintf(stderr, "Usage: %s (<host>)\n", argv[0]);
	fprintf(stderr, "Optional <host> parameter displays only packets from that host\n");

	fprintf(stderr, "\nPer-packet header output:\n");
	fprintf(stderr, "-Q- / -R-      Query Packet or Response Packet\n");
	fprintf(stderr, "Q/Ans/Auth/Add Number of questions, answers, authority records and additional records in packet.\n");

	fprintf(stderr, "\nPer-record display:\n");
	fprintf(stderr, "(P)            Probe Question (new service starting)\n");
	fprintf(stderr, "(DE)           Deletion/Goodbye (service going away)\n");
	fprintf(stderr, "(Q)            Query Question\n");
	fprintf(stderr, "(KA)           Known Answer (information querier already knows)\n");
	fprintf(stderr, "(AN)           Answer to question (or periodic announcment) (entire RR Set)\n");
	fprintf(stderr, "(A+)           Answer to question (or periodic announcment) (add to existing RR Set members)\n");
	fprintf(stderr, "(AD)           Additional record\n");

	fprintf(stderr, "\nFinal summary, sorted by service type:\n");
	fprintf(stderr, "Probe          Probes for this service type starting up\n");
	fprintf(stderr, "Goodbye        Goodbye (deletion) packets for this service type shutting down\n");
	fprintf(stderr, "BrowseQ        Browse questions from clients browsing to find a list of instances of this service\n");
	fprintf(stderr, "BrowseA        Browse answers/announcments advertising instances of this service\n");
	fprintf(stderr, "ResolveQ       Resolve questions from clients actively connecting to an instance of this service\n");
	fprintf(stderr, "ResolveA       Resolve answers/announcments giving connection information for an instance of this service\n");
	fprintf(stderr, "\n");
	return(-1);
	}
