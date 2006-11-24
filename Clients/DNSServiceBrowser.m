/* -*- Mode: C; tab-width: 4 -*-
 *
 * Copyright (c) 2002-2003 Apple Computer, Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.

    Change History (most recent first):

$Log: DNSServiceBrowser.m,v $
Revision 1.32  2006/11/24 00:25:31  mkrochma
<rdar://problem/4084652> Tools: DNS Service Browser contains some bugs

Revision 1.31  2006/08/14 23:23:55  cheshire
Re-licensed mDNSResponder daemon source code under Apache License, Version 2.0

Revision 1.30  2005/01/27 17:46:16  cheshire
Added comment

Revision 1.29  2004/06/04 20:58:36  cheshire
Move DNSServiceBrowser from mDNSMacOSX directory to Clients directory

Revision 1.28  2004/05/18 23:51:26  cheshire
Tidy up all checkin comments to use consistent "<rdar://problem/xxxxxxx>" format for bug numbers

Revision 1.27  2003/11/19 18:49:48  rpantos
<rdar://problem/3282283> couple of little tweaks to previous checkin

Revision 1.26  2003/11/07 19:35:20  rpantos
<rdar://problem/3282283> Display multiple IP addresses. Connect using host rather than IP addr.

Revision 1.25  2003/10/29 05:16:54  rpantos
Checkpoint: transition from DNSServiceDiscovery.h to dns_sd.h

Revision 1.24  2003/10/28 02:25:45  rpantos
<rdar://problem/3282283> Cancel pending resolve when focus changes or service disappears.

Revision 1.23  2003/10/28 01:29:15  rpantos
<rdar://problem/3282283> Restructure a bit to make arrow keys work & views behave better.

Revision 1.22  2003/10/28 01:23:27  rpantos
<rdar://problem/3282283> Bail if mDNS cannot be initialized at startup.

Revision 1.21  2003/10/28 01:19:45  rpantos
<rdar://problem/3282283> Do not put a trailing '.' on service names. Handle PATH for HTTP txtRecords.

Revision 1.20  2003/10/28 01:13:49  rpantos
<rdar://problem/3282283> Remove filter when displaying browse results.

Revision 1.19  2003/10/28 01:10:14  rpantos
<rdar://problem/3282283> Change 'compare' to 'caseInsensitiveCompare' to fix sort order.

Revision 1.18  2003/08/12 19:55:07  cheshire
Update to APSL 2.0

*/

#import <Cocoa/Cocoa.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <dns_sd.h>

@class ServiceController;  // holds state corresponding to outstanding DNSServiceRef

@interface BrowserController : NSObject
{
    IBOutlet id nameField;
    IBOutlet id typeField;

    IBOutlet id serviceDisplayTable;
    IBOutlet id typeColumn;
    IBOutlet id nameColumn;
    IBOutlet id serviceTypeField;
    IBOutlet id serviceNameField;

    IBOutlet id hostField;
    IBOutlet id ipAddressField;
    IBOutlet id ip6AddressField;
    IBOutlet id portField;
    IBOutlet id textField;
    
    NSMutableArray *srvtypeKeys;
    NSMutableArray *srvnameKeys;
    NSMutableArray *_sortedServices;
    NSMutableDictionary *_servicesDict;
    NSString *_srvType;
    NSString *_srvName;
    NSString *_name;

	ServiceController *fServiceBrowser;
	ServiceController *fServiceResolver;
	ServiceController *fAddressResolver;
}

- (void)notifyTypeSelectionChange:(NSNotification*)note;
- (void)notifyNameSelectionChange:(NSNotification*)note;

- (IBAction)connect:(id)sender;

- (IBAction)handleTableClick:(id)sender;
- (IBAction)removeSelected:(id)sender;
- (IBAction)addNewService:(id)sender;

- (IBAction)update:(NSString *)Type;

- (void)updateBrowseWithName:(const char *)name type:(const char *)resulttype domain:(const char *)domain interface:(uint32_t)interface flags:(DNSServiceFlags)flags;
- (void)resolveClientWitHost:(NSString *)host port:(uint16_t)port interfaceIndex:(uint32_t)interface txtRecord:(const char*)txtRecord txtLen:(uint16_t)txtLen;
- (void)updateAddress:(uint16_t)rrtype addr:(const void *)buff addrLen:(uint16_t)addrLen host:(const char*)host interfaceIndex:(uint32_t)interface more:(boolean_t)moreToCome;

- (void)_cancelPendingResolve;
- (void)_clearResolvedInfo;

@end

// The ServiceController manages cleanup of DNSServiceRef & runloop info for an outstanding request
@interface ServiceController : NSObject
{
	DNSServiceRef       fServiceRef;
	CFSocketRef         fSocketRef;
	CFRunLoopSourceRef  fRunloopSrc;
}

- (id)initWithServiceRef:(DNSServiceRef)ref;
- (void)addToCurrentRunLoop;
- (DNSServiceRef)serviceRef;
- (void)dealloc;

@end // interface ServiceController


static void	ProcessSockData(CFSocketRef s, CFSocketCallBackType type, CFDataRef address, const void *data, void *info)
// CFRunloop callback that notifies dns_sd when new data appears on a DNSServiceRef's socket.
{
	DNSServiceRef serviceRef = (DNSServiceRef)info;
	DNSServiceErrorType err = DNSServiceProcessResult(serviceRef);
	if (err != kDNSServiceErr_NoError) {
		printf("DNSServiceProcessResult() returned an error! %d\n", err);
    }
}


static void	ServiceBrowseReply(DNSServiceRef sdRef, DNSServiceFlags servFlags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, 
    const char *serviceName, const char *regtype, const char *replyDomain, void *context)
// Report newly-discovered services to the BrowserController.
{
	if (errorCode == kDNSServiceErr_NoError) {
		BrowserController *me = (BrowserController*)context;
		[me updateBrowseWithName:serviceName type:regtype domain:replyDomain interface:interfaceIndex flags:servFlags];
	} else {
		printf("ServiceBrowseReply got an error! %d\n", errorCode);
	}
}


static void ServiceResolveReply(DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex,  DNSServiceErrorType errorCode,
    const char *fullname, const char *hosttarget, uint16_t port, uint16_t txtLen, const char *txtRecord, void *context)
// Pass along resolved service info to the BrowserController.
{
	if (errorCode == kDNSServiceErr_NoError) {
		BrowserController *me = (BrowserController*)context;
		[me resolveClientWitHost:[NSString stringWithUTF8String:hosttarget] port:port interfaceIndex:interfaceIndex txtRecord:txtRecord txtLen:txtLen];
	} else {
		printf("ServiceResolveReply got an error! %d\n", errorCode);
	}
}


static void	QueryRecordReply(DNSServiceRef DNSServiceRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode,
    const char *fullname, uint16_t rrtype, uint16_t rrclass,  uint16_t rdlen, const void *rdata, uint32_t ttl, void *context)
// DNSServiceQueryRecord callback used to look up IP addresses.
{
    BrowserController *pBrowser = (BrowserController*)context;
    [pBrowser updateAddress:rrtype addr:rdata addrLen:rdlen host:fullname interfaceIndex:interfaceIndex more:((flags & kDNSServiceFlagsMoreComing) != 0)];
}


@implementation BrowserController		//Begin implementation of BrowserController methods

- (void)registerDefaults
{
    NSMutableDictionary *regDict = [NSMutableDictionary dictionary];

    NSArray *typeArray = [NSArray arrayWithObjects:@"_ftp._tcp",
												   @"_ssh._tcp",
												   @"_http._tcp",
												   @"_printer._tcp",
                                                   @"_ipp._tcp",
												   @"_presence._tcp",
                                                   @"_eppc._tcp",
												   @"_afpovertcp._tcp",
                                                   @"smb._tcp",
                                                   nil];
                                                   
    NSArray *nameArray = [NSArray arrayWithObjects:@"File Transfer (ftp)",
	                                               @"Secure Shell (ssh)",
	                                               @"Web Server (http)",
	                                               @"LPR Printer",
                                                   @"IPP Printer",
												   @"iChat",
                                                   @"Remote AppleEvents",
												   @"AppleShare Server",
                                                   @"SMB File Server",
                                                   nil];

    [regDict setObject:typeArray forKey:@"SrvTypeKeys"];
    [regDict setObject:nameArray forKey:@"SrvNameKeys"];

    [[NSUserDefaults standardUserDefaults] registerDefaults:regDict];
}


- (id)init
{
    self = [super init];
    if (self) {
        [self registerDefaults];
        fServiceBrowser = nil;
        fServiceResolver = nil;
        fAddressResolver = nil;
        _srvType = nil;
        srvtypeKeys = [[NSMutableArray alloc] init];
        srvnameKeys = [[NSMutableArray alloc] init];
        _sortedServices = [[NSMutableArray alloc] init];
        _servicesDict = [[NSMutableDictionary alloc] init];
    }
    return self;
}


- (void)awakeFromNib
{
    [typeField sizeLastColumnToFit];
    [nameField sizeLastColumnToFit];

    [nameField setDoubleAction:@selector(connect:)];

    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(notifyTypeSelectionChange:) name:NSTableViewSelectionDidChangeNotification object:typeField];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(notifyNameSelectionChange:) name:NSTableViewSelectionDidChangeNotification object:nameField];

    [srvtypeKeys addObjectsFromArray:[[NSUserDefaults standardUserDefaults] arrayForKey:@"SrvTypeKeys"]];
    [srvnameKeys addObjectsFromArray:[[NSUserDefaults standardUserDefaults] arrayForKey:@"SrvNameKeys"]];
    
    [typeField reloadData];
}


- (void)dealloc
{
    [srvtypeKeys release];
    [srvnameKeys release];
    [_servicesDict release];
    [_sortedServices release];
    [super dealloc];
}


-(void)tableView:(NSTableView *)theTableView setObjectValue:(id)object forTableColumn:(NSTableColumn *)tableColumn row:(int)row
{
    if (row < 0) return;
}


- (int)numberOfRowsInTableView:(NSTableView *)theTableView	//Begin mandatory TableView methods
{
    if (theTableView == typeField) {
        return [srvnameKeys count];
    }
    if (theTableView == nameField) {
        return [_servicesDict count];
    }
    if (theTableView == serviceDisplayTable) {
        return [srvnameKeys count];
    }
    return 0;
}


- (id)tableView:(NSTableView *)theTableView objectValueForTableColumn:(NSTableColumn *)theColumn row:(int)rowIndex
{
    if (theTableView == typeField) {
        return [srvnameKeys objectAtIndex:rowIndex];
    }
    if (theTableView == nameField) {
        return [[_servicesDict objectForKey:[_sortedServices objectAtIndex:rowIndex]] name];
    }
    if (theTableView == serviceDisplayTable) {
        if (theColumn == typeColumn) {
            return [srvtypeKeys objectAtIndex:rowIndex];
        }
        if (theColumn == nameColumn) {
            return [srvnameKeys objectAtIndex:rowIndex];
        }
        return nil;
    }
    
    return nil;
}


- (void)notifyTypeSelectionChange:(NSNotification*)note
{
    int index = [[note object] selectedRow];
    if (index >= 0) {
        _srvType = [srvtypeKeys objectAtIndex:index];
        _srvName = [srvnameKeys objectAtIndex:index];
        [self _cancelPendingResolve];
        [self update:_srvType];
    }
}


- (void)notifyNameSelectionChange:(NSNotification*)note
{
    [self _cancelPendingResolve];
    
    int index = [[note object] selectedRow];
    if (index == -1) {
		return;
	}
    
    NSNetService *service = [_servicesDict objectForKey:[_sortedServices objectAtIndex:index]];
	DNSServiceRef serviceRef;
	DNSServiceErrorType err = DNSServiceResolve(&serviceRef,
                                         (DNSServiceFlags)0,
                               kDNSServiceInterfaceIndexAny,
                  (const char *)[[service name] UTF8String],
                 (const char *)[[service type]  UTF8String],
                (const char *)[[service domain] UTF8String],
                (DNSServiceResolveReply)ServiceResolveReply,
                                                      self);
        
	if (kDNSServiceErr_NoError == err) {
		fServiceResolver = [[ServiceController alloc] initWithServiceRef:serviceRef];
		[fServiceResolver addToCurrentRunLoop];
	}
}


- (IBAction)update:(NSString *)theType
{
    DNSServiceErrorType err = kDNSServiceErr_NoError;

    [_servicesDict removeAllObjects];
    [_sortedServices removeAllObjects];
    [nameField reloadData];

    // get rid of the previous browser if one exists
    if (fServiceBrowser != nil) {
		[fServiceBrowser release];
        fServiceBrowser = nil;
    }

    // now create a browser to return the values for the nameField ...
	DNSServiceRef serviceRef;
	err = DNSServiceBrowse(&serviceRef, (DNSServiceFlags)0, 0, [theType UTF8String], NULL, ServiceBrowseReply, self);
	if (kDNSServiceErr_NoError == err) {
		fServiceBrowser = [[ServiceController alloc] initWithServiceRef:serviceRef];
		[fServiceBrowser addToCurrentRunLoop];
	}
}


- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication
{
    return YES;
}


- (void)updateBrowseWithName:(const char *)name type:(const char *)type domain:(const char *)domain interface:(uint32_t)interface flags:(DNSServiceFlags)flags
{
    NSString *key = [NSString stringWithFormat:@"%s.%s%s:%d", name, type, domain, interface];
    NSNetService *service = [[NSNetService alloc] initWithDomain:[NSString stringWithUTF8String:domain] type:[NSString stringWithUTF8String:type] name:[NSString stringWithUTF8String:name]];
    
    if (flags & kDNSServiceFlagsAdd) {
        [_servicesDict setObject:service forKey:key];
    } else {
        [_servicesDict removeObjectForKey:key];
    }

    // If not expecting any more data, then reload (redraw) TableView with newly found data
    if (!(flags & kDNSServiceFlagsMoreComing)) {
    
        // Save the current TableView selection
        int index = [nameField selectedRow];
        NSString *selected = (index >= 0) ? [_sortedServices objectAtIndex:index] : nil;
        
        [_sortedServices release];
        _sortedServices = [[_servicesDict allKeys] mutableCopy];        
        [_sortedServices sortUsingSelector:@selector(caseInsensitiveCompare:)];
        [nameField reloadData];
        
        // Restore the previous TableView selection
        index = selected ? [_sortedServices indexOfObject:selected] : -1;
        if (index >= 0) {
            [nameField selectRowIndexes:[NSIndexSet indexSetWithIndex:index] byExtendingSelection:NO];
            [nameField scrollRowToVisible:index];
        }
    }

    [service release];

    return;
}


- (void)resolveClientWitHost:(NSString *)host port:(uint16_t)port interfaceIndex:(uint32_t)interface txtRecord:(const char*)txtRecord txtLen:(uint16_t)txtLen
/* Display resolved information about the selected service. */
{
	DNSServiceRef serviceRef;

	// Start an async lookup for IPv4 & IPv6 addresses
	if (fAddressResolver != nil) {
		[fAddressResolver release];
		fAddressResolver = nil;
	}

	DNSServiceErrorType err = DNSServiceQueryRecord(&serviceRef, (DNSServiceFlags)0, interface, [host UTF8String], kDNSServiceType_A, kDNSServiceClass_IN, QueryRecordReply, self);
	if (err == kDNSServiceErr_NoError) {
		fAddressResolver = [[ServiceController alloc] initWithServiceRef:serviceRef];
		[fAddressResolver addToCurrentRunLoop];
	}

    [hostField setStringValue:host];
    [portField setIntValue:ntohs(port)];

	// kind of a hack: munge txtRecord so it's human-readable
	if (txtLen > 0) {
		char *readableText = (char*) malloc(txtLen);
		if (readableText != nil) {
			ByteCount index, subStrLen;
			memcpy(readableText, txtRecord, txtLen);
			for (index=0; index < txtLen - 1; index += subStrLen + 1) {
				subStrLen = readableText[ index];
				readableText[ index] = '\n';
			}
			[textField setStringValue:[NSString stringWithCString:&readableText[1] length:txtLen - 1]];
			free(readableText);
		}
	}
}


- (void)updateAddress:(uint16_t)rrtype  addr:(const void *)buff addrLen:(uint16_t)addrLen host:(const char*) host interfaceIndex:(uint32_t)interface more:(boolean_t)moreToCome
/* Update address field(s) with info obtained by fAddressResolver. */
{
	if (rrtype == kDNSServiceType_A) {    // IPv4
		char addrBuff[256];
		inet_ntop(AF_INET, buff, addrBuff, sizeof addrBuff);
		strcat(addrBuff, " ");
		[ipAddressField setStringValue:[NSString stringWithFormat:@"%@%s", [ipAddressField stringValue], addrBuff]];

		if (!moreToCome) {
			[fAddressResolver release];
			fAddressResolver = nil;
	
			// After we find v4 we look for v6
			DNSServiceRef serviceRef;
			DNSServiceErrorType err;
			err = DNSServiceQueryRecord(&serviceRef, (DNSServiceFlags) 0, interface, host, kDNSServiceType_AAAA, kDNSServiceClass_IN, QueryRecordReply, self);
			if (err == kDNSServiceErr_NoError) {
				fAddressResolver = [[ServiceController alloc] initWithServiceRef:serviceRef];
				[fAddressResolver addToCurrentRunLoop];
			}
		}
	} else if (rrtype == kDNSServiceType_AAAA) {    // IPv6
		char addrBuff[256];
		inet_ntop(AF_INET6, buff, addrBuff, sizeof addrBuff);
		strcat(addrBuff, " ");
		[ip6AddressField setStringValue:[NSString stringWithFormat:@"%@%s", [ip6AddressField stringValue], addrBuff]];

		if (!moreToCome) {
			[fAddressResolver release];
			fAddressResolver = nil;
		}
	}
}


- (void)connect:(id)sender
{
    NSString *host = [hostField stringValue];
    int port = [portField intValue];
    NSString *txtRecord = [textField stringValue];

    if (!txtRecord) txtRecord = @"";

    if (!host || !port) return;

    if ([_srvType isEqualToString:@"_http._tcp"]) {
        NSString *pathDelim = @"path=";
		NSRange where;

        // If the TXT record specifies a path, extract it.
		where = [txtRecord rangeOfString:pathDelim options:NSCaseInsensitiveSearch];
        if (where.length) {
			NSRange	targetRange = { where.location + where.length, [txtRecord length] - where.location - where.length };
			NSRange	endDelim = [txtRecord rangeOfString:@"\n" options:kNilOptions range:targetRange];
			
			if (endDelim.length)   // if a delimiter was found, truncate the target range
				targetRange.length = endDelim.location - targetRange.location;

            NSString    *path = [txtRecord substringWithRange:targetRange];
            [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:[NSString stringWithFormat:@"http://%@:%d%@", host, port, path]]];
        } else {
            [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:[NSString stringWithFormat:@"http://%@:%d", host, port]]];
        }
    }
    else if ([_srvType isEqualToString:@"_ftp._tcp"])        [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:[NSString stringWithFormat:@"ftp://%@:%d/",    host, port]]];
    else if ([_srvType isEqualToString:@"_ssh._tcp"])        [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:[NSString stringWithFormat:@"ssh://%@:%d/",    host, port]]];
    else if ([_srvType isEqualToString:@"_printer._tcp"])    [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:[NSString stringWithFormat:@"lpr://%@:%d/",    host, port]]];
    else if ([_srvType isEqualToString:@"_ipp._tcp"])        [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:[NSString stringWithFormat:@"ipp://%@:%d/",    host, port]]];
    else if ([_srvType isEqualToString:@"_afpovertcp._tcp"]) [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:[NSString stringWithFormat:@"afp://%@:%d/",    host, port]]];
    else if ([_srvType isEqualToString:@"_smb._tcp"])        [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:[NSString stringWithFormat:@"smb://%@:%d/",    host, port]]];

    return;
}


- (IBAction)handleTableClick:(id)sender
{
    //populate the text fields
}


- (IBAction)removeSelected:(id)sender
{
    // remove the selected row and force a refresh

    int selectedRow = [serviceDisplayTable selectedRow];

    if (selectedRow) {

        [srvtypeKeys removeObjectAtIndex:selectedRow];
        [srvnameKeys removeObjectAtIndex:selectedRow];

        [[NSUserDefaults standardUserDefaults] setObject:srvtypeKeys forKey:@"SrvTypeKeys"];
        [[NSUserDefaults standardUserDefaults] setObject:srvnameKeys forKey:@"SrvNameKeys"];

        [typeField reloadData];
        [serviceDisplayTable reloadData];
    }
}


- (IBAction)addNewService:(id)sender
{
    // add new entries from the edit fields to the arrays for the defaults
    NSString *newType = [serviceTypeField stringValue];
    NSString *newName = [serviceNameField stringValue];

    // 3282283: trim trailing '.' from service type field
    if ([newType length] && [newType hasSuffix:@"."])
        newType = [newType substringToIndex:[newType length] - 1];

    if ([newType length] && [newName length]) {
        [srvtypeKeys addObject:newType];
        [srvnameKeys addObject:newName];

        [[NSUserDefaults standardUserDefaults] setObject:srvtypeKeys forKey:@"SrvTypeKeys"];
        [[NSUserDefaults standardUserDefaults] setObject:srvnameKeys forKey:@"SrvNameKeys"];

        [typeField reloadData];
        [serviceDisplayTable reloadData];
    }
}


- (void)_cancelPendingResolve
// If there a a Resolve outstanding, cancel it.
{
    [fAddressResolver release];
    fAddressResolver = nil;

    [fServiceResolver release];
    fServiceResolver = nil;

	[self _clearResolvedInfo];
}


- (void)_clearResolvedInfo
// Erase the display of resolved info.
{
	[hostField setStringValue:@""];
	[ipAddressField setStringValue:@""];
	[ip6AddressField setStringValue:@""];
	[portField setStringValue:@""];
	[textField setStringValue:@""];
}

@end // implementation BrowserController


@implementation ServiceController : NSObject
{
	DNSServiceRef        fServiceRef;
	CFSocketRef          fSocketRef;
	CFRunLoopSourceRef   fRunloopSrc;
}


- (id)initWithServiceRef:(DNSServiceRef)ref
{
	self = [super init];
    if (self) {
        fServiceRef = ref;
    }
	return self;
}


- (void)addToCurrentRunLoop
{
	CFSocketContext	context = { 0, (void*)fServiceRef, NULL, NULL, NULL };

	fSocketRef = CFSocketCreateWithNative(kCFAllocatorDefault, DNSServiceRefSockFD(fServiceRef), kCFSocketReadCallBack, ProcessSockData, &context);
	if (fSocketRef) {
		fRunloopSrc = CFSocketCreateRunLoopSource(kCFAllocatorDefault, fSocketRef, 0);
    }
	if (fRunloopSrc) {
		CFRunLoopAddSource(CFRunLoopGetCurrent(), fRunloopSrc, kCFRunLoopDefaultMode);
    } else {
		printf("Could not listen to runloop socket\n");
    }
}


- (DNSServiceRef)serviceRef
{
	return fServiceRef;
}


- (void)dealloc
{
	if (fSocketRef) {
		CFSocketInvalidate(fSocketRef);		// Note: Also closes the underlying socket
		CFRelease(fSocketRef);
	}

	if (fRunloopSrc) {
		CFRunLoopRemoveSource(CFRunLoopGetCurrent(), fRunloopSrc, kCFRunLoopDefaultMode);
		CFRelease(fRunloopSrc);
	}

	DNSServiceRefDeallocate(fServiceRef);

	[super dealloc];
}


@end // implementation ServiceController

int main(int argc, const char *argv[])
{
    return NSApplicationMain(argc, argv);
}
