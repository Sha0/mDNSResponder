#include "dns_sd.h"
#include <DNSServiceDiscovery/DNSServiceDiscovery.h> // include Mach API to ensure no conflicts exist
#include <CoreFoundation/CoreFoundation.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define BIND_8_COMPAT 1
#include <nameser.h>
// T_SRV is not defined in older versions of nameser.h
#ifndef T_SRV
#define T_SRV 33
#endif

// constants
#define MAX_DOMAIN_LABEL 63
#define MAX_DOMAIN_NAME 255
#define MAX_CSTRING 2044


// data structure defs
typedef struct { u_char c[ 64]; } domainlabel;
typedef struct { u_char c[256]; } domainname;


typedef struct 
    { 
    uint16_t priority; 
    uint16_t weight; 
    uint16_t port; 
    domainname target;
    } srv_rdata;


// private function prototypes
static void sighdlr(int signo);
static char *ConvertDomainNameToCString_withescape(const domainname *const name, char *ptr, char esc);
static char *ConvertDomainLabelToCString_withescape(const domainlabel *const label, char *ptr, char esc);
//static void MyCallbackWrapper(CFSocketRef sr, CFSocketCallBackType t, CFDataRef dr, const void *i, void *context);
static void print_rdata(int type, int len, const u_char *rdata);
static void query_cb(const DNSServiceRef DNSServiceRef, const DNSServiceFlags flags, const u_int32_t interfaceIndex, const DNSServiceErrorType errorCode, const char *name, const u_int16_t rrtype, const u_int16_t rrclass, const u_int16_t rdlen, const void *rdata, const u_int32_t ttl, void *context);
static void resolve_cb(const DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *fullname, const char *hosttarget, uint16_t port, uint16_t txtLen, const char                          *txtRecord, void *context);

// globals
static DNSServiceRef sdr = NULL;

int main (int argc, char * argv[])  {
    int err, t;
    char *name, *type, *domain;
    char *txtstring = "My Txt Record";
    
    char full[1024];
    
    if (signal(SIGINT, sighdlr) == SIG_ERR)  fprintf(stderr, "ERROR - can't catch interupt!\n");
    if (argc < 2) exit(1);

    if (!strcmp(argv[1], "-query"))
        {
        t = atol(argv[5]);
        err = DNSServiceConstructFullName(full, argv[2], argv[3], argv[4]);
        if (err) exit(1);
        printf("resolving fullname %s type %d\n", full, t);
        err = DNSServiceQueryRecord(&sdr, 0, 0, full, t, 1, query_cb, NULL);
        while (1) DNSServiceProcessResult(sdr);
        }

    if (!strcmp(argv[1], "-regservice"))
        {
        if (argc > 2) name = argv[2];
        else name = NULL;
        err = DNSServiceRegister(&sdr, 0, 0, name, "_testservice._tcp", NULL, NULL, 123, strlen(txtstring), txtstring, NULL, NULL);
        if (err) 
            {
            printf("DNSServiceRegister returned error %d\n", err);
            exit(1);
            }
        while (1) DNSServiceProcessResult(sdr);
        }
    if (!strcmp(argv[1], "-resolve"))
        {
        name = argv[2];
        type = argv[3];
        domain = argv[4];
        err = DNSServiceResolve(&sdr, 0, 0, name, type, domain, resolve_cb, NULL);
        if (err) 
            {
            printf("DNSServiceResolve returned error %d\n", err);
            exit(1);
            }
        while(1) DNSServiceProcessResult(sdr);
        }
    exit(1);
    }    



// callbacks

// wrapper to make callbacks fit CFRunLoop callback signature
/*
static void MyCallbackWrapper(CFSocketRef sr, CFSocketCallBackType t, CFDataRef dr, const void *i, void *context)  
    {
    (void)sr;
    (void)t;
    (void)dr;
    (void)i;
    
    DNSServiceRef *sdr = context;
    DNSServiceDiscoveryProcessResult(*sdr);
    }
*/

static void query_cb(const DNSServiceRef DNSServiceRef, const DNSServiceFlags flags, const u_int32_t interfaceIndex, const DNSServiceErrorType errorCode, const char *name, const u_int16_t rrtype, const u_int16_t rrclass, const u_int16_t rdlen, const void *rdata, const u_int32_t ttl, void *context) 
    {
    (void)DNSServiceRef;
    (void)flags;
    (void)interfaceIndex;
    (void)rrclass;
    (void)ttl;
    (void)context;
    
    if (errorCode)
        {
        printf("query callback: error==%d\n", errorCode);
        return;
        }
    printf("query callback - name = %s, rdata=\n", name);
    print_rdata(rrtype, rdlen, rdata);
    }
 
static void resolve_cb(const DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *fullname, const char *hosttarget, uint16_t port, uint16_t txtLen, const char *txtRecord, void *context)
    {
    int i;
    
    #pragma unused(sdRef, flags, interfaceIndex, errorCode, context, txtRecord)
    printf("Resolved %s to %s:%d (%d bytes txt data)\n", fullname, hosttarget, port, txtLen);
    printf("TXT Data:\n");
    for (i = 0; i < txtLen; i++)
        if (txtRecord[i] >= ' ') printf("%c", txtRecord[i]);
    }




// resource record data interpretation routines
static char *ConvertDomainLabelToCString_withescape(const domainlabel *const label, char *ptr, char esc)
    {
    const u_char *      src = label->c;                         // Domain label we're reading
    const u_char        len = *src++;                           // Read length of this (non-null) label
    const u_char *const end = src + len;                        // Work out where the label ends
    if (len > MAX_DOMAIN_LABEL) return(NULL);           // If illegal label, abort
    while (src < end)                                           // While we have characters in the label
        {
        u_char c = *src++;
        if (esc)
            {
            if (c == '.')                                       // If character is a dot,
                *ptr++ = esc;                                   // Output escape character
            else if (c <= ' ')                                  // If non-printing ascii,
                {                                                   // Output decimal escape sequence
                *ptr++ = esc;
                *ptr++ = (char)  ('0' + (c / 100)     );
                *ptr++ = (char)  ('0' + (c /  10) % 10);
                c      = (u_char)('0' + (c      ) % 10);
                }
            }
        *ptr++ = (char)c;                                       // Copy the character
        }
    *ptr = 0;                                                   // Null-terminate the string
    return(ptr);                                                // and return
    }
 
static char *ConvertDomainNameToCString_withescape(const domainname *const name, char *ptr, char esc)
    {
    const u_char *src         = name->c;                        // Domain name we're reading
    const u_char *const max   = name->c + MAX_DOMAIN_NAME;      // Maximum that's valid

    if (*src == 0) *ptr++ = '.';                                // Special case: For root, just write a dot

    while (*src)                                                                                                        // While more characters in the domain name
        {
        if (src + 1 + *src >= max) return(NULL);
        ptr = ConvertDomainLabelToCString_withescape((const domainlabel *)src, ptr, esc);
        if (!ptr) return(NULL);
        src += 1 + *src;
        *ptr++ = '.';                                           // Write the dot after the label
        }

    *ptr++ = 0;                                                 // Null-terminate the string
    return(ptr);                                                // and return
    }

// print arbitrary rdata in a readable manned 
static void print_rdata(int type, int len, const u_char *rdata)
    {
    int i;
    srv_rdata *srv;
    char targetstr[MAX_CSTRING];
    struct in_addr in;
    
    switch (type)
        {
        case T_TXT:
            // print all the alphanumeric and punctuation characters
            for (i = 0; i < len; i++)
                if (rdata[i] >= 32 && rdata[i] <= 127) printf("%c", rdata[i]);
            printf("\n");
            return;
        case T_SRV:
            srv = (srv_rdata *)rdata;
            ConvertDomainNameToCString_withescape(&srv->target, targetstr, 0);
            printf("pri=%d, w=%d, port=%d, target=%s\n", srv->priority, srv->weight, srv->port, targetstr);
            return;
        case T_A:
            assert(len == 4);
            memcpy(&in, rdata, sizeof(in));
            printf("%s\n", inet_ntoa(in));
            return;
        case T_PTR:
            ConvertDomainNameToCString_withescape((domainname *)rdata, targetstr, 0);
            printf("%s\n", targetstr);
            return;
        default:
            printf("ERROR: I dont know how to print RData of type %d\n", type);
            return;
        }
    }




// signal handlers, setup/teardown, etc.
static void sighdlr(int signo)
    {
    assert(signo == SIGINT);
    fprintf(stderr, "Received sigint - deallocating serviceref and exiting\n");
    if (sdr)
        DNSServiceRefDeallocate(sdr);
    exit(1);
    }


 
 
 
 
 
