/*
 * Copyright (c) 2002-2003 Apple Computer, Inc. All rights reserved.
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
 * $Log: Identify.c,v $
 * Revision 1.1  2003/08/01 02:20:02  cheshire
 * Add mDNSIdentify tool, used to discover what version of mDNSResponder a particular host is running
 *
 */

//*************************************************************************************************************
// Incorporate mDNS.c functionality

// We want to use the functionality provided by "mDNS.c",
// except we'll sneak a peek at the packets before forwarding them to the normal mDNSCoreReceive() routine
#define mDNSCoreReceive __MDNS__mDNSCoreReceive
#include "mDNS.c"
#undef mDNSCoreReceive

//*************************************************************************************************************
// Headers

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>		// For n_long, required by <netinet/ip.h> below
#include <netinet/ip.h>				// For IPTOS_LOWDELAY etc.
#include <arpa/inet.h>

#include "mDNSClientAPI.h"// Defines the interface to the mDNS core code
#include "mDNSPosix.h"    // Defines the specific types needed to run mDNS on this platform
#include "ExampleClientApp.h"

//*************************************************************************************************************
// Globals

static mDNS mDNSStorage;       // mDNS core uses this to store its globals
static mDNS_PlatformSupport PlatformStorage;  // Stores this platform's globals
#define RR_CACHE_SIZE 500
static ResourceRecord gRRCache[RR_CACHE_SIZE];

static volatile int StopNow;
static char hostname[256], hardware[256], software[256];
static mDNSOpaque16 lastid, id;

//*************************************************************************************************************
// Main code

mDNSexport void mDNSCoreReceive(mDNS *const m, DNSMessage *const msg, const mDNSu8 *const end,
	const mDNSAddr *const srcaddr, const mDNSIPPort srcport, const mDNSAddr *const dstaddr, const mDNSIPPort dstport,
	const mDNSInterfaceID InterfaceID)
	{
	// Snag copy of header ID, then call through
	lastid = msg->h.id;
	__MDNS__mDNSCoreReceive(m, msg, end, srcaddr, srcport, dstaddr, dstport, InterfaceID);
	}

static void NameCallback(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer)
	{
	(void)m;		// Unused
	(void)question;	// Unused
	id = lastid;
	ConvertDomainNameToCString(&answer->rdata->u.name, hostname);
	StopNow = 1;
	}

static void InfoCallback(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer)
	{
	mDNSu8 *p = answer->rdata->u.data;
	(void)m;		// Unused
	(void)question;	// Unused
	strncpy(hardware, p+1, p[0]);
	hardware[p[0]] = 0;
	p += 1 + p[0];
	strncpy(software, p+1, p[0]);
	software[p[0]] = 0;
	StopNow = 1;
	}

mDNSexport void WaitForAnswer(mDNS *const m, int seconds)
	{
	struct timeval end;
	gettimeofday(&end, NULL);
	end.tv_sec += seconds;
	StopNow = 0;
	while (!StopNow)
		{
		int nfds = 0;
		fd_set readfds;
		struct timeval now, remain = end;
		int result;

		FD_ZERO(&readfds);
		gettimeofday(&now, NULL);
		if (remain.tv_usec < now.tv_usec) { remain.tv_usec += 1000000; remain.tv_sec--; }
		if (remain.tv_sec < now.tv_sec) return;
		remain.tv_usec -= now.tv_usec;
		remain.tv_sec  -= now.tv_sec;
		mDNSPosixGetFDSet(m, &nfds, &readfds, &remain);
		result = select(nfds, &readfds, NULL, NULL, &remain);
		if (result >= 0) mDNSPosixProcessFDSet(m, &readfds);
		else if (errno != EINTR) StopNow = 2;
		}
	}

mDNSlocal void HandleSIG(int signal)
	{
	(void)signal;	// Unused
	debugf("");
	debugf("HandleSIG");
	StopNow = 2;
	}

mDNSexport int main(int argc, char **argv)
	{
	mStatus status;
	
	if (argc < 2) goto usage;
	
    // Initialise the mDNS core.
	status = mDNS_Init(&mDNSStorage, &PlatformStorage,
    	gRRCache, RR_CACHE_SIZE,
    	mDNS_Init_DontAdvertiseLocalAddresses,
    	mDNS_Init_NoInitCallback, mDNS_Init_NoInitCallbackContext);
	if (status) { fprintf(stderr, "Daemon start: mDNS_Init failed %ld\n", status); return(status); }

	signal(SIGINT, HandleSIG);	// SIGINT is what you get for a Ctrl-C
	signal(SIGTERM, HandleSIG);

	struct in_addr s4;
	struct in6_addr s6;

	char buffer[256];
	DNSQuestion q;
	q.InterfaceID      = mDNSInterface_Any;
	q.qtype            = kDNSType_PTR;
	q.qclass           = kDNSClass_IN;
	q.QuestionCallback = NameCallback;
	q.QuestionContext  = NULL;

	if (inet_pton(AF_INET, argv[1], &s4) == 1)
		{
		mDNSu8 *p = (mDNSu8 *)&s4;
		mDNS_snprintf(buffer, sizeof(buffer), "%d.%d.%d.%d.in-addr.arpa.", p[3], p[2], p[1], p[0]);
		printf("%s\n", buffer);
		}
	else if (inet_pton(AF_INET6, argv[1], &s6) == 1)
		{
		int i;
		mDNSu8 *p = (mDNSu8 *)&s6;
		for (i = 0; i < 16; i++)
			{
			static const char hexValues[] = "0123456789ABCDEF";
			buffer[i * 4    ] = hexValues[p[15-i] & 0x0F];
			buffer[i * 4 + 1] = '.';
			buffer[i * 4 + 2] = hexValues[p[15-i] >> 4];
			buffer[i * 4 + 3] = '.';
			}
		mDNS_snprintf(&buffer[64], sizeof(buffer)-64, "ip6.arpa.");
		printf("%s\n", buffer);
		}
	else
		strcpy(hostname, argv[1]);

	// If user entered a dotted decimal address, first find the host name
	if (*hostname == 0)
		{
		MakeDomainNameFromDNSNameString(&q.qname, buffer);
		status = mDNS_StartQuery(&mDNSStorage, &q);
		WaitForAnswer(&mDNSStorage, 4);
		mDNS_StopQuery(&mDNSStorage, &q);
		if (StopNow == 2) goto exit;	// Interrupted with Ctrl-C
		if (StopNow == 0) { fprintf(stderr, "No answer found for %s\n", buffer); goto exit; }
		printf("Host name is: %s\n", hostname);
		}

	// Now we have the host name; get the HINFO
	MakeDomainNameFromDNSNameString(&q.qname, hostname);
	q.qtype = kDNSType_HINFO;
	q.QuestionCallback = InfoCallback;
	status = mDNS_StartQuery(&mDNSStorage, &q);
	WaitForAnswer(&mDNSStorage, 4);
	mDNS_StopQuery(&mDNSStorage, &q);
	if (StopNow == 2) goto exit;	// Interrupted with Ctrl-C
	if (StopNow == 1)	// Interrupted because we found the answer we wanted
		{
		printf("HINFO Hardware: %s\n", hardware);
		printf("HINFO Software: %s\n", software);
		}
	else
		{
		printf("Host has no HINFO record; Appears to be ");
		if (id.b[1]) printf("mDNSResponder-%d\n", id.b[1]);
		else printf("very early Panther build\n");
		}

exit:
	mDNS_Close(&mDNSStorage);
	return(0);

usage:
	fprintf(stderr, "%s <hostname> or <IPv4 address> or <IPv6 address>\n", argv[0]);
	return(-1);
	}
