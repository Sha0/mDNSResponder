//
//  HAAutomounter.m
//  HAAutomounter
//
//  Created by Eric Peyton on Wed May 01 2002.
//  Copyright (c) 2002 __MyCompanyName__. All rights reserved.
//

#import "HAAutomounter.h"

#import <AppKit/AppKit.h>

#include <sys/types.h>
#include "arpa/inet.h"
#include <netinet/in.h>

@implementation HAAutomounter

- (id)init
{
    self = [super init];

    browser = [[NSNetServiceBrowser alloc] init];

    [browser setDelegate:self];

    [browser searchForServicesOfType:@"_mountme._tcp." inDomain:@""];
    
    [browser scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode: NSDefaultRunLoopMode];

    return self;
}

- (void)netServiceBrowser:(NSNetServiceBrowser *)aNetServiceBrowser didFindService:(NSNetService *)aNetService moreComing:(BOOL)moreComing
{
    if (resolver) {
        [resolver release];
    }

    resolver = [[NSNetService alloc] initWithDomain:[aNetService domain] type:[aNetService type] name:[aNetService name]];
    [resolver setDelegate:self];
    [resolver scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode: NSDefaultRunLoopMode];

    [resolver resolve];
}


- (void)netServiceDidResolveAddress:(NSNetService *)sender;
{
    if ([[sender addresses] count]) {
    
        // URL mount the volume
        NSData *addr = [[sender addresses] objectAtIndex:0];
        struct sockaddr_in *address = CFDataGetBytePtr((CFDataRef)addr);
        NSString *ipAddr = [NSString stringWithCString:inet_ntoa(((struct in_addr)((struct sockaddr_in *)address)->sin_addr))];
        int port = ((struct sockaddr_in *)address)->sin_port;
        NSArray *txtArray = [[sender protocolSpecificInformation] componentsSeparatedByString:@","];

        if ([txtArray count] == 3) {
            NSString *user = [txtArray objectAtIndex:0];
            NSString *password = [txtArray objectAtIndex:1];
            NSString *share = [txtArray objectAtIndex:2];
    
            [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:[NSString stringWithFormat:@"afp://%@:%@@%@:%d/%@", user, password, ipAddr, port, share]]];
        } else {
            NSLog(@"incompatible format for txt record, s/b user,password,share");
        }

    } else {
        NSLog(@"No address %@", sender);
    }
}

- (void)netServiceBrowser:(NSNetServiceBrowser *)aNetServiceBrowser didRemoveService:(NSNetService *)aNetService moreComing:(BOOL)moreComing
{
    // unmount the volume
}


@end
