#import <Foundation/Foundation.h>

#import "HAAutomounter.h"

int main (int argc, const char * argv[]) {
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

    [[HAAutomounter alloc] init];

    [[NSRunLoop currentRunLoop] run];

    [pool release];
    return 0;
}
