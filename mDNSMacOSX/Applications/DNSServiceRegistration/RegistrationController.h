/* RegistrationController */

#import <Cocoa/Cocoa.h>

@interface RegistrationController : NSObject
{
    IBOutlet NSTableColumn 	*typeColumn;
    IBOutlet NSTableColumn 	*nameColumn;
    IBOutlet NSTableColumn 	*portColumn;
    IBOutlet NSTableColumn 	*domainColumn;
    IBOutlet NSTableColumn 	*textColumn;

    IBOutlet NSTableView	*serviceDisplayTable;

    IBOutlet NSTextField	*serviceTypeField;
    IBOutlet NSTextField	*serviceNameField;
    IBOutlet NSTextField	*servicePortField;
    IBOutlet NSTextField	*serviceDomainField;
    IBOutlet NSTextField	*serviceTextField;
    
    NSMutableArray		*srvtypeKeys;
    NSMutableArray		*srvnameKeys;
    NSMutableArray		*srvportKeys;
    NSMutableArray		*srvdomainKeys;
    NSMutableArray		*srvtextKeys;

    NSMutableDictionary		*registeredDict;
}

- (IBAction)registerService:(id)sender;
- (IBAction)unregisterService:(id)sender;

- (IBAction)addNewService:(id)sender;
- (IBAction)removeSelected:(id)sender;

@end
