/*
 * Copyright (c) 2002-2003 Apple Computer, Inc. All rights reserved.
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

$Log: BrowserController.h,v $
Revision 1.10  2003/10/29 05:17:26  rpantos
Checkpoint: transition from DNSServiceDiscovery.h to dns_sd.h

Revision 1.9  2003/10/28 02:25:26  rpantos
3282283/9,10: Cancel pending resolve when focus changes or service disappears.

Revision 1.8  2003/10/28 01:28:54  rpantos
3282283/4,5: Restructure a bit to make arrow keys work & views behave better.

Revision 1.7  2003/08/12 19:55:07  cheshire
Update to APSL 2.0

 */

#import <Cocoa/Cocoa.h>
#import <DNSServiceDiscovery/DNSServiceDiscovery.h>

#include <netinet/in.h>
#include "dns_sd.h"


@interface BrowserController : NSObject
{
    IBOutlet id domainField;
    IBOutlet id nameField;
    IBOutlet id typeField;

    IBOutlet id serviceDisplayTable;
    IBOutlet id typeColumn;
    IBOutlet id nameColumn;
    IBOutlet id serviceTypeField;
    IBOutlet id serviceNameField;

    IBOutlet id ipAddressField;
    IBOutlet id portField;
    IBOutlet id textField;
    
    NSMutableArray *srvtypeKeys;
    NSMutableArray *srvnameKeys;
    NSMutableArray *domainKeys;
    NSMutableArray *nameKeys;
    NSString *Domain;
    NSString *SrvType;
    NSString *SrvName;
    NSString *Name;

	DNSServiceRef				fDomainBrowser;
	DNSServiceRef				fServiceBrowser;
	DNSServiceRef				fServiceResolver;

}

- (IBAction)handleDomainClick:(id)sender;
- (IBAction)handleNameClick:(id)sender;
- (IBAction)handleTypeClick:(id)sender;
- (void)notifyTypeSelectionChange:(NSNotification*)note;
- (void)notifyNameSelectionChange:(NSNotification*)note;

- (IBAction)connect:(id)sender;

- (IBAction)handleTableClick:(id)sender;
- (IBAction)removeSelected:(id)sender;
- (IBAction)addNewService:(id)sender;

- (IBAction)update:(NSString *)Type Domain:(NSString *)Domain;
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication;
- (IBAction)loadDomains:(id)sender;

- (void)updateBrowseWithResult:(DNSServiceFlags)flags name:(NSString *)name type:(NSString *)resulttype domain:(NSString *)domain;
- (void)updateEnumWithResult:(DNSServiceFlags)flags domain:(NSString *)domain;
- (void)resolveClientWithInterface:(struct sockaddr *)interface address:(struct sockaddr *)address txtRecord:(NSString *)txtRecord;
- (void)resolveClientWitHost:(NSString *)host port:(uint16_t)port interfaceIndex:(uint32_t)interface txtRecord:(const char*)txtRecord txtLen:(uint16_t)txtLen;


- (void)_cancelPendingResolve;

@end