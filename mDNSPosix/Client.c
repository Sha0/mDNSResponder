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
 */
/*
    File:       client.c

    Contains:   Code to implement an mDNS browser on the Posix platform.

    Written by: Quinn

    Copyright:  Copyright (c) 2002 by Apple Computer, Inc., All Rights Reserved.

    Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple Computer, Inc.
                ("Apple") in consideration of your agreement to the following terms, and your
                use, installation, modification or redistribution of this Apple software
                constitutes acceptance of these terms.  If you do not agree with these terms,
                please do not use, install, modify or redistribute this Apple software.

                In consideration of your agreement to abide by the following terms, and subject
                to these terms, Apple grants you a personal, non-exclusive license, under Apple's
                copyrights in this original Apple software (the "Apple Software"), to use,
                reproduce, modify and redistribute the Apple Software, with or without
                modifications, in source and/or binary forms; provided that if you redistribute
                the Apple Software in its entirety and without modifications, you must retain
                this notice and the following text and disclaimers in all such redistributions of
                the Apple Software.  Neither the name, trademarks, service marks or logos of
                Apple Computer, Inc. may be used to endorse or promote products derived from the
                Apple Software without specific prior written permission from Apple.  Except as
                expressly stated in this notice, no other rights or licenses, express or implied,
                are granted by Apple herein, including but not limited to any patent rights that
                may be infringed by your derivative works or by other works in which the Apple
                Software may be incorporated.

                The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
                WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
                WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
                PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN
                COMBINATION WITH YOUR PRODUCTS.

                IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
                CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
                GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
                ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION AND/OR DISTRIBUTION
                OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT, TORT
                (INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN
                ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    Change History (most recent first):

$Log: Client.c,v $
Revision 1.5  2003/05/06 00:00:50  cheshire
<rdar://problem/3248914> Rationalize naming of domainname manipulation functions

Revision 1.4  2002/12/23 22:13:31  jgraessl

Reviewed by: Stuart Cheshire
Initial IPv6 support for mDNSResponder.

Revision 1.3  2002/09/21 20:44:53  zarzycki
Added APSL info

Revision 1.2  2002/09/19 04:20:44  cheshire
Remove high-ascii characters that confuse some systems

Revision 1.1  2002/09/17 06:24:35  cheshire
First checkin

*/

#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "mDNSClientAPI.h"// Defines the interface to the mDNS core code
#include "mDNSPosix.h"    // Defines the specific types needed to run mDNS on this platform
#include "ExampleClientApp.h"

// Globals
static mDNS mDNSStorage;       // mDNS core uses this to store its globals
static mDNS_PlatformSupport PlatformStorage;  // Stores this platform's globals
#define RR_CACHE_SIZE 500
static ResourceRecord gRRCache[RR_CACHE_SIZE];

static const char *gProgramName = "mDNSResponderPosix";

static void BrowseCallback(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer)
    // A callback from the core mDNS code that indicates that we've received a 
    // response to our query.  Note that this code runs on the main thread 
    // (in fact, there is only one thread!), so we can safely printf the results.
{
    domainlabel name;
    domainname  type;
    domainname  domain;
    char nameC[256];
    char typeC[256];
    char domainC[256];
    const char *state;

    assert(answer->rrtype == kDNSType_PTR);

    DeconstructServiceName(&answer->rdata->u.name, &name, &type, &domain);

    ConvertDomainLabelToCString_unescaped(&name, nameC);
    ConvertDomainNameToCString(&type, typeC);
    ConvertDomainNameToCString(&domain, domainC);

    // If the TTL has hit 0, the service is no longer available.
    if (answer->rrremainingttl == 0) {
        state = "Lost ";
    } else {
        state = "Found";
    }
    fprintf(stderr, "*** %s name = '%s', type = '%s', domain = '%s'\n", state, nameC, typeC, domainC);
}

static mDNSBool CheckThatServiceTypeIsUsable(const char *serviceType, mDNSBool printExplanation)
    // Checks that serviceType is a reasonable service type 
    // label and, if it isn't and printExplanation is true, prints 
    // an explanation of why not.
{
    mDNSBool result;
    
    result = mDNStrue;
    if (result && strlen(serviceType) > 63) {
        if (printExplanation) {
            fprintf(stderr, 
                    "%s: Service type specified by -t is too long (must be 63 characters or less)\n", 
                    gProgramName);
        }
        result = mDNSfalse;
    }
    if (result && serviceType[0] == 0) {
        if (printExplanation) {
            fprintf(stderr, 
                    "%s: Service type specified by -t can't be empty\n", 
                    gProgramName);
        }
        result = mDNSfalse;
    }
    return result;
}

static const char kDefaultServiceType[] = "_afpovertcp._tcp.";

static void PrintUsage(char **argv)
{
    fprintf(stderr, 
            "Usage: %s [-v level] [-t type]\n", 
            gProgramName);
    fprintf(stderr, "          -v verbose mode, level is a number from 0 to 2\n");
    fprintf(stderr, "             0 = no debugging info (default)\n");
    fprintf(stderr, "             1 = standard debugging info\n");
    fprintf(stderr, "             2 = intense debugging info\n");
    fprintf(stderr, "          -t uses 'type' as the service type (default is '%s')\n", kDefaultServiceType);
}

static const char *gServiceType      = kDefaultServiceType;

static void ParseArguments(int argc, char **argv)
    // Parses our command line arguments into the global variables 
    // listed above.
{
    int ch;
    
    // Set gProgramName to the last path component of argv[0]
    
    gProgramName = strrchr(argv[0], '/');
    if (gProgramName == NULL) {
        gProgramName = argv[0];
    } else {
        gProgramName += 1;
    }
    
    // Parse command line options using getopt.
    
    do {
        ch = getopt(argc, argv, "v:t:");
        if (ch != -1) {
            switch (ch) {
                case 'v':
                    gMDNSPlatformPosixVerboseLevel = atoi(optarg);
                    if (gMDNSPlatformPosixVerboseLevel < 0 || gMDNSPlatformPosixVerboseLevel > 2) {
                        fprintf(stderr, 
                                "%s: Verbose mode must be in the range 0..2\n", 
                                gProgramName);
                        exit(1);
                    }
                    break;
                case 't':
                    gServiceType = optarg;
                    if ( ! CheckThatServiceTypeIsUsable(gServiceType, mDNStrue) ) {
                        exit(1);
                    }
                    break;
                case '?':
                default:
                    PrintUsage(argv);
                    exit(1);
                    break;
            }
        }
    } while (ch != -1);

    // Check for any left over command line arguments.
    
    if (optind != argc) {
        fprintf(stderr, "%s: Unexpected argument '%s'\n", gProgramName, argv[optind]);
        exit(1);
    }
}

int main(int argc, char **argv)
    // The program's main entry point.  The program does a trivial 
    // mDNS query, looking for all AFP servers in the local domain.
{
    int     result;
    mStatus     status;
    DNSQuestion question;
    domainname  type;
    domainname  domain;

    // Parse our command line arguments.  This won't come back if there's an error.
    ParseArguments(argc, argv);

    // Initialise the mDNS core.
	status = mDNS_Init(&mDNSStorage, &PlatformStorage,
    	gRRCache, RR_CACHE_SIZE,
    	mDNS_Init_DontAdvertiseLocalAddresses,
    	mDNS_Init_NoInitCallback, mDNS_Init_NoInitCallbackContext);
    if (status == mStatus_NoError) {
    
        // Construct and start the query.
        
        MakeDomainNameFromDNSNameString(&type, gServiceType);
        MakeDomainNameFromDNSNameString(&domain, "local.");

        status = mDNS_StartBrowse(&mDNSStorage, &question, &type, &domain, mDNSInterface_Any, BrowseCallback, NULL);
    
        // Run the platform main event loop until the user types ^C. 
        // The BrowseCallback routine is responsible for printing 
        // any results that we find.
        
        if (status == mStatus_NoError) {
            fprintf(stderr, "Hit ^C when you're bored waiting for responses.\n");
        	ExampleClientEventLoop(&mDNSStorage);
            mDNS_StopQuery(&mDNSStorage, &question);
			mDNS_Close(&mDNSStorage);
        }
    }
    
    if (status == mStatus_NoError) {
        result = 0;
    } else {
        result = 2;
    }
    if ( (result != 0) || (gMDNSPlatformPosixVerboseLevel > 0) ) {
        fprintf(stderr, "%s: Finished with status %ld, result %d\n", gProgramName, status, result);
    }

    return 0;
}
