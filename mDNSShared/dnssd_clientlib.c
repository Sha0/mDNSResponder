/*
 * Copyright (c) 2004 Apple Computer, Inc. All rights reserved.
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

$Log: dnssd_clientlib.c,v $
Revision 1.1  2004/03/12 21:30:29  cheshire
Build a System-Context Shared Library from mDNSCore, for the benefit of developers
like Muse Research who want to be able to use mDNS/DNS-SD from GPL-licensed code.

 */

#include "dns_sd.h"

#if MDNS_BUILDINGSHAREDLIBRARY || MDNS_BUILDINGSTUBLIBRARY
#pragma export on
#endif

#if 0

static int domain_ends_in_dot(const char *dom)
	{
	while(*dom && *(dom + 1))
		{
		if (*dom == '\\') // advance past escaped byte sequence
			{
			if (*(dom + 1) >= '0' && *(dom + 1) <= '9') dom += 4;
			else dom += 2;
			}
		else dom++; // else read one character
		}
	return (*dom == '.');
	}

int DNSServiceConstructFullName
	(
	char                      *fullName,
	const char                *service,      /* may be NULL */
	const char                *regtype,
	const char                *domain
	)
	{
	int len;
	unsigned char c;
	char *fn = fullName;
	const char *s = service;
	const char *r = regtype;
	const char *d = domain;

	if (service)
		{
		while(*s)
			{
			c = (unsigned char)*s++;
			if (c == '.' || (c == '\\')) *fn++ = '\\'; // escape dot and backslash literals
			else if (c <= ' ') // escape non-printable characters
				{
				*fn++ = '\\';
				*fn++ = (char) ('0' + (c / 100));
				*fn++ = (char) ('0' + (c / 10) % 10);
				c = (unsigned char)('0' + (c % 10));
				}
			*fn++ = (char)c;
			}
		*fn++ = '.';
		}

	if (!regtype) return -1;
	len = strlen(regtype);
	if (domain_ends_in_dot(regtype)) len--;
	if (len < 4) return -1; // regtype must end in _udp or _tcp
	if (strncmp((regtype + len - 4), "_tcp", 4) && strncmp((regtype + len - 4), "_udp", 4)) return -1;
	while(*r)
		*fn++ = *r++;
	if (!domain_ends_in_dot(regtype)) *fn++ = '.';

	if (!domain) return -1;
	len = strlen(domain);
	if (!len) return -1;
	while(*d)
		*fn++ = *d++;
	if (!domain_ends_in_dot(domain)) *fn++ = '.';
	*fn = '\0';
	return 0;
	}

#endif
