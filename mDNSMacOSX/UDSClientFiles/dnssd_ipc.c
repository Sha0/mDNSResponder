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

 
ipc_msg_hdr *create_hdr(int op, int *len, char **data_start)
    {
    char *msg = NULL;
    ipc_msg_hdr *hdr;
    int datalen;

    assert(len);
    assert(data_start);

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



void put_flags(const DNSServiceDiscoveryFlags flags, char **ptr)
    {
    assert(ptr && *ptr);
    memcpy(*ptr, &flags, sizeof(DNSServiceDiscoveryFlags));
    *ptr += sizeof(flags);
    }

DNSServiceDiscoveryFlags get_flags(char **ptr)
    {
    DNSServiceDiscoveryFlags flags;
	
    assert(ptr && *ptr);
	flags = *(DNSServiceDiscoveryFlags *)*ptr;
	*ptr += sizeof(DNSServiceDiscoveryFlags);
	return flags;
    }

void put_long(const u_int32_t l, char **ptr)
    {
    assert(ptr && *ptr);

    *(u_int32_t *)(*ptr) = l;
    *ptr += sizeof(u_int32_t);
    }

u_int32_t get_long(char **ptr)
    {
    u_int32_t l;
	
    assert(ptr && *ptr);
    l = *(u_int32_t *)(*ptr);
    *ptr += sizeof(u_int32_t);
    return l;
    }

void put_error_code(const DNSServiceReplyErrorType error, char **ptr)
    {
    assert(ptr && *ptr);

    memcpy(*ptr, &error, sizeof(error));
    *ptr += sizeof(DNSServiceReplyErrorType);
    }

DNSServiceReplyErrorType get_error_code(char **ptr)
    {
    DNSServiceReplyErrorType error;
	
    assert(ptr && *ptr);
    error = *(DNSServiceReplyErrorType *)(*ptr);
    *ptr += sizeof(DNSServiceReplyErrorType);
    return error;
    }

void put_short(const u_int16_t s, char **ptr)
    {
    assert (ptr && *ptr);

    *(u_int16_t *)(*ptr) = s;
    *ptr += sizeof(u_int16_t);
    }

u_int16_t get_short(char **ptr)
    {
    u_int16_t s;

    assert(ptr && *ptr);
    s = *(u_int16_t *)(*ptr);
    *ptr += sizeof(u_int16_t);
    return s;
    }

	



void put_string(const char *str, char **ptr)
    {
    strcpy(*ptr, str);
    *ptr += strlen(str) + 1;
    }

// !!!KRS we don't properly handle the case where the string is longer than the buffer!!!	
char *get_string(char **ptr, char *buffer, int buflen)
    {
    strncpy(buffer, *ptr,  buflen - 1);
    buffer[buflen - 1] = '\0';
    *ptr += strlen(buffer) + 1;
    return buffer;
    }	

void put_rdata(const int rdlen, const char *rdata, char **ptr)
    {
    assert(ptr && *ptr);
    assert(rdata);

    memcpy(*ptr, rdata, rdlen);
    *ptr += rdlen;	
    }

char *get_rdata(char **ptr, int rdlen)
    {
    char *buffer;
		
    assert(ptr && *ptr);
    buffer = malloc(rdlen);
    if (!buffer)
        {
        perror("ERROR: malloc");
        return NULL;
        }
    memcpy(buffer, *ptr, rdlen);
    fprintf(stderr, "pulled string %s\n", *ptr);
    *ptr += rdlen;
    return buffer;
    }









