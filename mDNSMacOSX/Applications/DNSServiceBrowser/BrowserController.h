/*
 *  BrowserController.h
 *  IP Browser
 *
 *  Created by Rod Lopez <rod@apple.com> on Thu Jul 19 2001.
 *  Copyright (c) 2001 Apple Computer, Inc. All rights reserved.
 *
 *  This is experimental proof-of-concept code. Please excuse the mess.
 */

#import <Cocoa/Cocoa.h>
#import <DNSServiceDiscovery/DNSServiceDiscovery.h>

#include <netinet/in.h>

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

    dns_service_discovery_ref 	browse_client;

}

- (IBAction)handleDomainClick:(id)sender;
- (IBAction)handleNameClick:(id)sender;
- (IBAction)handleTypeClick:(id)sender;

- (IBAction)connect:(id)sender;

- (IBAction)handleTableClick:(id)sender;
- (IBAction)removeSelected:(id)sender;
- (IBAction)addNewService:(id)sender;

- (IBAction)update:(NSString *)Type Domain:(NSString *)Domain;
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication;
- (IBAction)loadDomains:(id)sender;

- (void)updateBrowseWithResult:(int)type name:(NSString *)name type:(NSString *)resulttype domain:(NSString *)domain flags:(int)flags;
- (void)updateEnumWithResult:(int)resultType domain:(NSString *)domain flags:(int)flags;
- (void)resolveClientWithInterface:(struct sockaddr *)interface address:(struct sockaddr *)address txtRecord:(NSString *)txtRecord;

@end