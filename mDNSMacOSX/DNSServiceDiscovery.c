/*
 * Copyright (c) 2002 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 *
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 1.1 (the
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

#include "DNSServiceDiscovery.h"
#include "DNSServiceDiscoveryDefines.h"

#include <stdlib.h>
#include <stdio.h>
#include <servers/bootstrap.h>
#include <mach/mach.h>
#include <mach/mach_error.h>
#include <pthread.h>

#include <netinet/in.h>

extern struct rpc_subsystem internal_DNSServiceDiscoveryReply_subsystem;

extern boolean_t DNSServiceDiscoveryReply_server(
        mach_msg_header_t *InHeadP,
        mach_msg_header_t *OutHeadP);

extern
kern_return_t DNSServiceBrowserCreate_rpc
(
    mach_port_t server,
    mach_port_t client,
    DNSCString regtype,
    DNSCString domain
);

extern
kern_return_t DNSServiceDomainEnumerationCreate_rpc
(
    mach_port_t server,
    mach_port_t client,
    int registrationDomains
);

extern
kern_return_t DNSServiceRegistrationCreate_rpc
(
    mach_port_t server,
    mach_port_t client,
    DNSCString name,
    DNSCString regtype,
    DNSCString domain,
    int port,
    DNSCString txtRecord
);

extern
kern_return_t DNSServiceResolverResolve_rpc
(
    mach_port_t server,
    mach_port_t client,
    DNSCString name,
    DNSCString regtype,
    DNSCString domain
);

struct a_requests {
    struct a_requests		*next;
    mach_port_t				client_port;
    union {
        DNSServiceBrowserReply 				browserCallback;
        DNSServiceDomainEnumerationReply 	enumCallback;
        DNSServiceRegistrationReply 		regCallback;
        DNSServiceResolverReply 			resolveCallback;
    } callout;
    void					*context;
};

static struct a_requests	*a_requests	= NULL;
static pthread_mutex_t		a_requests_lock	= PTHREAD_MUTEX_INITIALIZER;

typedef struct _dns_service_discovery_t {
    mach_port_t	port;
} dns_service_discovery_t;

mach_port_t DNSServiceDiscoveryLookupServer(void)
{
    static mach_port_t sndPort 	= MACH_PORT_NULL;
    kern_return_t   result;

    if (sndPort != MACH_PORT_NULL) {
        return sndPort;
    }

    result = bootstrap_look_up(bootstrap_port, DNS_SERVICE_DISCOVERY_SERVER, &sndPort);
    if (result != KERN_SUCCESS) {
        printf("%s(): {%s:%d} bootstrap_look_up() failed: $%x\n", __FUNCTION__, __FILE__, __LINE__, (int) result);
        sndPort = MACH_PORT_NULL;
    }

    return sndPort;
}

dns_service_discovery_ref DNSServiceBrowserCreate (char *regtype, char *domain, DNSServiceBrowserReply callBack,void *context)
{
    mach_port_t serverPort = DNSServiceDiscoveryLookupServer();
    mach_port_t clientPort;
    kern_return_t result;
    dns_service_discovery_ref return_t;
    struct a_requests	*request;
    
    if (!serverPort) {
        return NULL;
    }

    result = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &clientPort);
    if (result != KERN_SUCCESS) {
        printf("Mach port receive creation failed, %s\n", mach_error_string(result));
        return NULL;
    }
    result = mach_port_insert_right(mach_task_self(), clientPort, clientPort, MACH_MSG_TYPE_MAKE_SEND);
    if (result != KERN_SUCCESS) {
        printf("Mach port send creation failed, %s\n", mach_error_string(result));
        mach_port_destroy(mach_task_self(), clientPort);
        return NULL;
    }

    return_t = malloc(sizeof(dns_service_discovery_t));
    return_t->port = clientPort;

    request = malloc(sizeof(struct a_requests));
    request->client_port = clientPort;
    request->context = context;
    request->callout.browserCallback = callBack;

    result = DNSServiceBrowserCreate_rpc(serverPort, clientPort, regtype, domain);

    if (result != KERN_SUCCESS) {
        printf("There was an error creating a browser, %s\n", mach_error_string(result));
        free(request);
        return NULL;
    }

    pthread_mutex_lock(&a_requests_lock);
    request->next = a_requests;
    a_requests = request;
    pthread_mutex_unlock(&a_requests_lock);
    
    return return_t;
}

/* Service Enumeration */

dns_service_discovery_ref DNSServiceDomainEnumerationCreate (int registrationDomains, DNSServiceDomainEnumerationReply callBack, void *context)
{
    mach_port_t serverPort = DNSServiceDiscoveryLookupServer();
    mach_port_t clientPort;
    kern_return_t result;
    dns_service_discovery_ref return_t;
    struct a_requests	*request;

    if (!serverPort) {
        return NULL;
    }

    result = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &clientPort);
    if (result != KERN_SUCCESS) {
        printf("Mach port receive creation failed, %s\n", mach_error_string(result));
        return NULL;
    }
    result = mach_port_insert_right(mach_task_self(), clientPort, clientPort, MACH_MSG_TYPE_MAKE_SEND);
    if (result != KERN_SUCCESS) {
        printf("Mach port send creation failed, %s\n", mach_error_string(result));
        mach_port_destroy(mach_task_self(), clientPort);
        return NULL;
    }
    return_t = malloc(sizeof(dns_service_discovery_t));
    return_t->port = clientPort;

    request = malloc(sizeof(struct a_requests));
    request->client_port = clientPort;
    request->context = context;
    request->callout.enumCallback = callBack;

    result = DNSServiceDomainEnumerationCreate_rpc(serverPort, clientPort, registrationDomains);

    if (result != KERN_SUCCESS) {
        printf("There was an error creating an enumerator, %s\n", mach_error_string(result));
        free(request);
        return NULL;
    }
    
    pthread_mutex_lock(&a_requests_lock);
    request->next = a_requests;
    a_requests = request;
    pthread_mutex_unlock(&a_requests_lock);

    return return_t;
}


/* Service Registration */

dns_service_discovery_ref DNSServiceRegistrationCreate
(char *name, char *regtype, char *domain, Opaque16 port, char *txtRecord, DNSServiceRegistrationReply callBack, void *context)
{
    mach_port_t serverPort = DNSServiceDiscoveryLookupServer();
    mach_port_t clientPort;
    kern_return_t		result;
    dns_service_discovery_ref return_t;
    struct a_requests	*request;
    
    if (!serverPort) {
        return NULL;
    }

    result = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &clientPort);
    if (result != KERN_SUCCESS) {
        printf("Mach port receive creation failed, %s\n", mach_error_string(result));
        return NULL;
    }
    result = mach_port_insert_right(mach_task_self(), clientPort, clientPort, MACH_MSG_TYPE_MAKE_SEND);
    if (result != KERN_SUCCESS) {
        printf("Mach port send creation failed, %s\n", mach_error_string(result));
        mach_port_destroy(mach_task_self(), clientPort);
        return NULL;
    }
    return_t = malloc(sizeof(dns_service_discovery_t));
    return_t->port = clientPort;

    request = malloc(sizeof(struct a_requests));
    request->client_port = clientPort;
    request->context = context;
    request->callout.regCallback = callBack;

    result = DNSServiceRegistrationCreate_rpc(serverPort, clientPort, name, regtype, domain, (int)(port.NotAnInteger), txtRecord);

    if (result != KERN_SUCCESS) {
        printf("There was an error creating a resolve, %s\n", mach_error_string(result));
        free(request);
        return NULL;
    }

    pthread_mutex_lock(&a_requests_lock);
    request->next = a_requests;
    a_requests = request;
    pthread_mutex_unlock(&a_requests_lock);

    return return_t;
}

/* Resolver requests */

dns_service_discovery_ref DNSServiceResolverResolve(char *name, char *regtype, char *domain, DNSServiceResolverReply callBack, void *context)
{
    mach_port_t serverPort = DNSServiceDiscoveryLookupServer();
    mach_port_t clientPort;
    kern_return_t		result;
    dns_service_discovery_ref return_t;
    struct a_requests	*request;

    if (!serverPort) {
        return NULL;
    }

    result = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &clientPort);
    if (result != KERN_SUCCESS) {
        printf("Mach port receive creation failed, %s\n", mach_error_string(result));
        return NULL;
    }
    result = mach_port_insert_right(mach_task_self(), clientPort, clientPort, MACH_MSG_TYPE_MAKE_SEND);
    if (result != KERN_SUCCESS) {
        printf("Mach port send creation failed, %s\n", mach_error_string(result));
        mach_port_destroy(mach_task_self(), clientPort);
        return NULL;
    }
    return_t = malloc(sizeof(dns_service_discovery_t));
    return_t->port = clientPort;

    request = malloc(sizeof(struct a_requests));
    request->client_port = clientPort;
    request->context = context;
    request->callout.resolveCallback = callBack;

    DNSServiceResolverResolve_rpc(serverPort, clientPort, name, regtype, domain);

    pthread_mutex_lock(&a_requests_lock);
    request->next = a_requests;
    a_requests = request;
    pthread_mutex_unlock(&a_requests_lock);

    return return_t;
}

void DNSServiceDiscovery_handleReply(void *replyMsg)
{
    unsigned long			result = 0xFFFFFFFF;
    mach_msg_header_t *    	msgSendBufPtr;
    mach_msg_header_t *     receivedMessage;
    unsigned        		msgSendBufLength;

    msgSendBufLength = internal_DNSServiceDiscoveryReply_subsystem.maxsize;
    msgSendBufPtr = (mach_msg_header_t *) malloc(msgSendBufLength);


    receivedMessage = ( mach_msg_header_t * ) replyMsg;

    // Call DNSServiceDiscoveryReply_server to change mig-generated message into a
    // genuine mach message. It will then cause the callback to get called.
    result = DNSServiceDiscoveryReply_server ( receivedMessage, msgSendBufPtr );
    ( void ) mach_msg_send ( msgSendBufPtr );
    free(msgSendBufPtr);
}

mach_port_t DNSServiceDiscoveryMachPort(dns_service_discovery_ref dnsServiceDiscovery)
{
    return dnsServiceDiscovery->port;
}

void DNSServiceDiscoveryDeallocate(dns_service_discovery_ref dnsServiceDiscovery)
{
    struct a_requests	*request0, *request;
    mach_port_t reply = dnsServiceDiscovery->port;

    if (dnsServiceDiscovery->port) {
        pthread_mutex_lock(&a_requests_lock);
        request0 = NULL;
        request  = a_requests;
        while (request) {
               if (request->client_port == reply) {
                /* request info found, remove from list */
                if (request0) {
                    request0->next = request->next;
                } else {
                    a_requests = request->next;
                }
                break;
            }
        }
        pthread_mutex_unlock(&a_requests_lock);

        free(request);
        
        mach_port_destroy(mach_task_self(), dnsServiceDiscovery->port);
    }
    return;
}

// reply functions, calls the users setup callbacks with function pointers

kern_return_t internal_DNSServiceDomainEnumerationReply_rpc
(
    mach_port_t reply,
    int resultType,
    DNSCString replyDomain,
    int flags
)
{
    struct a_requests	*request;
    void *requestContext = NULL;
    DNSServiceDomainEnumerationReply callback = NULL;

    pthread_mutex_lock(&a_requests_lock);
    request  = a_requests;
    while (request) {
        if (request->client_port == reply) {
            break;
        }
        request = request->next;
    }

    if (request != NULL) {
        callback = (*request->callout.enumCallback);
        requestContext = request->context;
    }
    pthread_mutex_unlock(&a_requests_lock);

    if (request != NULL) {
        (callback)(resultType, replyDomain, flags, requestContext);
    }
    
    return KERN_SUCCESS;

}

kern_return_t internal_DNSServiceBrowserReply_rpc
(
    mach_port_t reply,
    int resultType,
    DNSCString replyName,
    DNSCString replyType,
    DNSCString replyDomain,
    int flags
)
{
    struct a_requests	*request;
    void *requestContext = NULL;
    DNSServiceBrowserReply callback = NULL;

    pthread_mutex_lock(&a_requests_lock);
    request  = a_requests;
    while (request) {
        if (request->client_port == reply) {
            break;
        }
        request = request->next;
    }
    if (request != NULL) {
        callback = (*request->callout.browserCallback);
        requestContext = request->context;
    }

    pthread_mutex_unlock(&a_requests_lock);

    if (request != NULL) {
        (callback)(resultType, replyName, replyType, replyDomain, flags, requestContext);
    }

    return KERN_SUCCESS;
}


kern_return_t internal_DNSServiceRegistrationReply_rpc
(
    mach_port_t reply,
    int resultType
)
{
    struct a_requests	*request;
    void *requestContext = NULL;
    DNSServiceRegistrationReply callback = NULL;

    pthread_mutex_lock(&a_requests_lock);
    request  = a_requests;
    while (request) {
        if (request->client_port == reply) {
            break;
        }
        request = request->next;
    }
    if (request != NULL) {
        callback = (*request->callout.regCallback);
        requestContext = request->context;
    }

    pthread_mutex_unlock(&a_requests_lock);
    if (request != NULL) {
        (callback)(resultType, requestContext);
    }
    return KERN_SUCCESS;
}


kern_return_t internal_DNSServiceResolverReply_rpc
(
    mach_port_t reply,
    sockaddr_t interface,
    sockaddr_t address,
    DNSCString txtRecord,
    int flags
)
{
    struct sockaddr  *interface_storage = NULL;
    struct sockaddr  *address_storage = NULL;
    struct a_requests	*request;
    void *requestContext = NULL;
    DNSServiceResolverReply callback = NULL;

    if (interface) {
        int len = ((struct sockaddr *)interface)->sa_len;
        interface_storage = (struct sockaddr *)malloc(len);
        bcopy(interface, interface_storage,len);
    }

    if (address) {
        int len = ((struct sockaddr *)address)->sa_len;
        address_storage = (struct sockaddr *)malloc(len);
        bcopy(address, address_storage, len);
    }

    pthread_mutex_lock(&a_requests_lock);
    request  = a_requests;
    while (request) {
        if (request->client_port == reply) {
            break;
        }
        request = request->next;
    }

    if (request != NULL) {
        callback = (*request->callout.resolveCallback);
        requestContext = request->context;
    }
    pthread_mutex_unlock(&a_requests_lock);

    if (request != NULL) {
        (callback)(interface_storage, address_storage, txtRecord, flags, requestContext);
    }

    if (interface) {
        free(interface_storage);
    }
    if (address) {
        free(address_storage);
    }
    
    return KERN_SUCCESS;
}
