//
//  HAAutomounter.h
//  HAAutomounter
//
//  Created by Eric Peyton on Wed May 01 2002.
//  Copyright (c) 2002 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface HAAutomounter : NSObject {

    NSNetServiceBrowser *browser;
    NSNetService 	*resolver;
}

@end
