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



#ifndef DNSSD_IPC_H
#define DNSSD_IPC_H

#include "uds_dnssd.h"
#include <sys/types.h>
#include <unistd.h>
#include <sys/un.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <assert.h>

#define UDSDEBUG  // verbose debug output

// General UDS constants
#define MDNS_UDS_SERVERPATH "/tmp/UDS_DNSSD"
#define LISTENQ 100


// IPC data encoding constants and types

#define VERSION 1

typedef enum
    {
    connection = 1,           // connected socket via DNSServiceConnect()
    reg_record_request,	  // reg/remove record only valid for connected sockets
    remove_record_request,
    enumeration_request,
    reg_service_request,
    browse_request,
    resolve_request,
    query_request,
    reconfirm_record_request
    } request_op_t;

typedef enum
    {
    enumeration_reply = 10,
    reg_service_reply,
    browse_reply,
    query_reply,
    reg_record_reply
    } reply_op_t;


typedef struct ipc_msg_hdr_struct ipc_msg_hdr;


// client stub callback to process message from server and deliver results to
// client application

typedef void (*process_reply_callback)
(
DNSServiceDiscoveryRef sdr,
ipc_msg_hdr *hdr,
char *msg
);

// allow 64-bit client to interoperate w/ 32-bit daemon
typedef union
    {
    void *context;
    u_int32_t ptr64[2];
    } client_context_t;


//!!!KRS it would be cleaner to just put the flags in the header, since they can change after the message is formatted

typedef struct ipc_msg_hdr_struct
    {
    uint32_t len;			
    uint32_t datalen;
    uint32_t version;
    union
    	{
        request_op_t request_op;
        reply_op_t reply_op;
    	} op;
    client_context_t client_context; // context passed from client, returned by server in corresponding reply
    int reg_index;                   // identifier for a record registered via DNSServiceRegisterRecord() on a
    // socket connected by DNSServiceConnect().  Must be unique in the scope of the connection, such that and
    // index/socket pair uniquely identifies a record.  (Used to select records for removal by DNSServiceRemoveRecord())
    
    } ipc_msg_hdr_struct;			



/* create_hdr
 *
 * allocate and initialize an ipc message header.  value of len should initially be the
 * length of the data, and is set to the value of the data plus the header.  data_start 
 * is set to point to the beginning of the data section.
 */
 

ipc_msg_hdr *create_hdr(int op, int *len, char **data_start);




// routines to write to and extract data from message buffers.
// caller responsible for bounds checking.  (get_rdata allocates 
// buffer with malloc())  
// ptr is the address of the pointer to the start of the field.
// it is advanced to point to the next field, or the end of the message


void put_flags(const DNSServiceDiscoveryFlags flags, char **ptr);
DNSServiceDiscoveryFlags get_flags(char **ptr);

void put_long(const u_int32_t l, char **ptr);
u_int32_t get_long(char **ptr);

void put_error_code(const DNSServiceReplyErrorType, char **ptr);
DNSServiceReplyErrorType get_error_code(char **ptr);

void put_string(const char *str, char **ptr);
char *get_string(char **ptr, char *buffer, int buflen);

void put_rdata(const int rdlen, const char *rdata, char **ptr);
char *get_rdata(char **ptr, int rdlen);

void put_short(u_int16_t s, char **ptr);
u_int16_t get_short(char **ptr);



#endif // DNSSD_IPC_H











