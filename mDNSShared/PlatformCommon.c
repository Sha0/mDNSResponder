/* -*- Mode: C; tab-width: 4 -*-
 *
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

$Log: PlatformCommon.c,v $
Revision 1.1  2004/12/01 01:51:35  cheshire
Move ReadDDNSSettingsFromConfFile() from mDNSMacOSX.c to PlatformCommon.c

 */

#include <stdio.h>				// Needed for fopen() etc.
#include <string.h>				// Needed for strlen() etc.
#include <sys/errno.h>			// Needed for errno etc.

#include "mDNSEmbeddedAPI.h"	// Defines the interface provided to the client layer above
#include "PlatformCommon.h"

// dst must be at least MAX_ESCAPED_DOMAIN_NAME bytes, and option must be less than 20 bytes in length
mDNSlocal mDNSBool GetConfigOption(char *dst, const char *option, FILE *f)
	{
	char buf[1024];
	int len = strlen(option);
	if (len + MAX_ESCAPED_DOMAIN_NAME > 1024) { LogMsg("GetConfigOption: option %s too long", option); return mDNSfalse; }
	fseek(f, 0, SEEK_SET);  // set position to beginning of stream
	while (fgets(buf, 1024, f))
		{
		if (!strncmp(buf, option, len))
			{
			strncpy(dst, buf + len + 1, MAX_ESCAPED_DOMAIN_NAME-1);
			if (dst[MAX_ESCAPED_DOMAIN_NAME-1]) dst[MAX_ESCAPED_DOMAIN_NAME-1] = '\0';
			len = strlen(dst);
			if (len && dst[len-1] == '\n') dst[len-1] = '\0';  // chop newline
			return mDNStrue;
			}
		}
	debugf("Option %s not set", option);
	return mDNSfalse;
	}

mDNSexport void ReadDDNSSettingsFromConfFile(mDNS *const m, const char *const filename, domainname *const hostname, domainname *const domain)
	{
	char zone  [MAX_ESCAPED_DOMAIN_NAME];
	char fqdn  [MAX_ESCAPED_DOMAIN_NAME];
	char secret[MAX_ESCAPED_DOMAIN_NAME] = "";
	int slen;
	mStatus err;
	FILE *f = fopen(filename, "r");

    hostname->c[0] = 0;
    domain->c[0] = 0;

	if (f)
		{
		if (GetConfigOption(fqdn, "hostname", f) && !MakeDomainNameFromDNSNameString(hostname, fqdn)) goto badf;
		if (GetConfigOption(zone, "zone", f) && !MakeDomainNameFromDNSNameString(domain, zone)) goto badf;
		GetConfigOption(secret, "secret-64", f);  // failure means no authentication	   
		fclose(f);
		f = NULL;
		}
	else
		{
		if (errno != ENOENT) LogMsg("ERROR: Config file exists, but cannot be opened.");
		return;
		}

	if (secret[0])
		{
		// for now we assume keyname = service reg domain and we use same key for service and hostname registration
		slen = strlen(secret);
		err = mDNS_SetSecretForZone(m, domain, domain, secret, slen, mDNStrue);
		if (err) LogMsg("ERROR: mDNS_SetSecretForZone returned %d for domain %##s", err, domain->c);
		}

	return;

	badf:
	LogMsg("ERROR: malformatted config file");
	if (f) fclose(f);	
	}
