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
Revision 1.4  2004/05/25 17:08:55  cheshire
Fix compiler warning (doesn't make sense for function return type to be const)

Revision 1.3  2004/05/21 21:41:35  cheshire
Add TXT record building and parsing APIs

Revision 1.2  2004/05/20 22:22:21  cheshire
Enable code that was bracketed by "#if 0"

Revision 1.1  2004/03/12 21:30:29  cheshire
Build a System-Context Shared Library from mDNSCore, for the benefit of developers
like Muse Research who want to be able to use mDNS/DNS-SD from GPL-licensed code.

 */

#include <malloc.h>

#include "dns_sd.h"

#if MDNS_BUILDINGSHAREDLIBRARY || MDNS_BUILDINGSTUBLIBRARY
#pragma export on
#endif

/*********************************************************************************************
 *
 *  Supporting Functions
 *
 *********************************************************************************************/

#define mdnsIsDigit(X)     ((X) >= '0' && (X) <= '9')

static int strlen(const char *str)
	{
	const char *s = str;
	while (*s) s++;
	return(s - str);
	}

static int strncmp(const char *s1, const char *s2, int n)
	{
	while (n)
		{
		if (*s1 != *s2++) return (*(const unsigned char *)s1 - *(const unsigned char *)(s2 - 1));
		if (*s1++ == 0) return (0);
		n--;
		}
	return (0);
	}

static void memcpy(void *dst, const void *src, int length)
	{
	int i;
	for (i=0; i<length; i++) ((char*)dst)[i] = ((char*)src)[i];
	}

static int DomainEndsInDot(const char *dom)
	{
	while (dom[0] && dom[1])
		{
		if (dom[0] == '\\') // advance past escaped byte sequence
			{
			if (mdnsIsDigit(dom[1]) && mdnsIsDigit(dom[2]) && mdnsIsDigit(dom[3]))
				dom += 4;			// If "\ddd"    then skip four
			else dom += 2;			// else if "\x" then skip two
			}
		else dom++;					// else goto next character
		}
	return (dom[0] == '.');
	}

static uint8_t *InternalTXTRecordSearch
	(
	uint16_t         txtLen,
	const void       *txtRecord,
	const char       *key,
	int              *keylen
	)
	{
	uint8_t *p = (uint8_t*)txtRecord;
	uint8_t *e = p + txtLen;
	*keylen = strlen(key);
	while (p<e)
		{
		uint8_t *x = p;
		p += 1 + p[0];
		if (p <= e && *keylen <= x[0] && !strncmp(key, (char*)x+1, *keylen))
			if (*keylen == x[0] || x[1+*keylen] == '=') return(x);
		}
	return(NULL);
	}

/*********************************************************************************************
 *
 *  General Utility Functions
 *
 *********************************************************************************************/

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
	if (DomainEndsInDot(regtype)) len--;
	if (len < 6) return -1; // regtype must be at least "x._udp" or "x._tcp"
	if (strncmp((regtype + len - 4), "_tcp", 4) && strncmp((regtype + len - 4), "_udp", 4)) return -1;
	while(*r) *fn++ = *r++;
	if (!DomainEndsInDot(regtype)) *fn++ = '.';

	if (!domain || !domain[0]) return -1;
	while(*d) *fn++ = *d++;
	if (!DomainEndsInDot(domain)) *fn++ = '.';
	*fn = '\0';
	return 0;
	}

/*********************************************************************************************
 *
 *   TXT Record Construction Functions
 *
 *********************************************************************************************/

typedef struct _TXTRecordRefRealType
	{
	uint8_t  *buffer;		// Pointer to data
	uint16_t buflen;		// Length of buffer
	uint16_t datalen;		// Length currently in use
	uint16_t malloced;	// Non-zero if buffer was allocated via malloc()
	} TXTRecordRefRealType;

#define txtRec ((TXTRecordRefRealType*)txtRecord)

// The opaque storage defined in the public dns_sd.h header is 16 bytes;
// make sure we don't exceed that.
struct dnssd_clientlib_CompileTimeAssertionCheck
	{
	char assert0[(sizeof(TXTRecordRefRealType) <= 16) ? 1 : -1];
	};

void TXTRecordCreate
	(
	TXTRecordRef     *txtRecord,
	uint16_t         bufferLen,
	void             *buffer
	)
	{
	txtRec->buffer   = buffer;
	txtRec->buflen   = buffer ? bufferLen : (uint16_t)0;
	txtRec->datalen  = 0;
	txtRec->malloced = 0;
	}

void TXTRecordDeallocate(TXTRecordRef *txtRecord)
	{
	if (txtRec->malloced) free(txtRec->buffer);
	}

DNSServiceErrorType TXTRecordSetValue
	(
	TXTRecordRef     *txtRecord,
	const char       *key,
	uint8_t          valueSize,
	const void       *value
	)
	{
	uint8_t *start, *p;
	const char *k;
	int keysize, keyvalsize;

	for (k = key; *k; k++) if (*k < 0x20 || *k > 0x7E || *k == '=') return(kDNSServiceErr_Invalid);
	keysize = k - key;
	keyvalsize = 1 + keysize + (value ? (1 + valueSize) : 0);
	if (keysize < 1 || keyvalsize > 255) return(kDNSServiceErr_Invalid);
	(void)TXTRecordRemoveValue(txtRecord, key);
	if (txtRec->datalen + keyvalsize > txtRec->buflen)
		{
		unsigned char *newbuf;
		int newlen = txtRec->datalen + keyvalsize;
		if (newlen > 0xFFFF) return(kDNSServiceErr_Invalid);
		newbuf = malloc((size_t)newlen);
		if (!newbuf) return(kDNSServiceErr_NoMemory);
		memcpy(newbuf, txtRec->buffer, txtRec->datalen);
		if (txtRec->malloced) free(txtRec->buffer);
		txtRec->buffer = newbuf;
		txtRec->buflen = (uint16_t)(newlen);
		txtRec->malloced = 1;
		}
	start = txtRec->buffer + txtRec->datalen;
	p = start + 1;
	memcpy(p, key, keysize);
	p += keysize;
	if (value)
		{
		*p++ = '=';
		memcpy(p, value, valueSize);
		p += valueSize;
		}
	*start = (uint8_t)(p - start - 1);
	txtRec->datalen += p - start;
	return(kDNSServiceErr_NoError);
	}

DNSServiceErrorType TXTRecordRemoveValue
	(
	TXTRecordRef     *txtRecord,
	const char       *key
	)
	{
	int keylen, itemlen;
	uint8_t *item = InternalTXTRecordSearch(txtRec->datalen, txtRec->buffer, key, &keylen);
	if (!item) return(kDNSServiceErr_NoSuchKey);
	itemlen = 1 + item[0];
	memcpy(item, item + itemlen, txtRec->buffer + txtRec->datalen - (item + itemlen));
	txtRec->datalen -= itemlen;
	return(kDNSServiceErr_NoError);
	}

uint16_t       TXTRecordGetLength  (const TXTRecordRef *txtRecord) { return(txtRec->datalen); }
const void *   TXTRecordGetBytesPtr(const TXTRecordRef *txtRecord) { return(txtRec->buffer); }

/*********************************************************************************************
 *
 *   TXT Record Parsing Functions
 *
 *********************************************************************************************/

int TXTRecordContainsKey
	(
	uint16_t         txtLen,
	const void       *txtRecord,
	const char       *key
	)
	{
	int keylen;
	return (InternalTXTRecordSearch(txtLen, txtRecord, key, &keylen) ? 1 : 0);
	}

const void * TXTRecordGetValuePtr
	(
	uint16_t         txtLen,
	const void       *txtRecord,
	const char       *key,
	uint8_t          *valueLen
	)
	{
	int keylen;
	uint8_t *item = InternalTXTRecordSearch(txtLen, txtRecord, key, &keylen);
	if (!item || item[0] <= keylen) return(NULL);	// If key not found, or found with no value, return NULL
	*valueLen = (uint8_t)(item[0] - (keylen + 1));
	return (item + 1 + keylen + 1);
	}

uint16_t TXTRecordGetCount
	(
	uint16_t         txtLen,
	const void       *txtRecord
	)
	{
	uint16_t count = 0;
	uint8_t *p = (uint8_t*)txtRecord;
	uint8_t *e = p + txtLen;
	while (p<e) { p += 1 + p[0]; count++; }
	return((p>e) ? (uint16_t)0 : count);
	}

DNSServiceErrorType TXTRecordGetItemAtIndex
	(
	uint16_t         txtLen,
	const void       *txtRecord,
	uint16_t         index,
	uint16_t         keyBufLen,
	char             *key,
	uint8_t          *valueLen,
	const void       **value
	)
	{
	uint16_t count = 0;
	uint8_t *p = (uint8_t*)txtRecord;
	uint8_t *e = p + txtLen;
	while (p<e && count<index) { p += 1 + p[0]; count++; }	// Find requested item
	if (p<e && p + 1 + p[0] <= e)	// If valid
		{
		uint8_t *x = p+1;
		int len = 0;
		e = p + 1 + p[0];
		while (x+len<e && x[len] != '=') len++;
		if (len >= keyBufLen) return(kDNSServiceErr_NoMemory);
		memcpy(key, x, len);
		key[len] = 0;
		if (x+len<e)		// If we found '='
			{
			*value = x + len + 1;
			*valueLen = (uint8_t)(p[0] - (len + 1));
			}
		else
			{
			*value = NULL;
			*valueLen = 0;
			}
		return(kDNSServiceErr_NoError);
		}
	return(kDNSServiceErr_Invalid);
	}
