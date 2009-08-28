#include "nsIGenericFactory.h"
#include "CDNSSDService.h"

NS_COM_GLUE nsresult
NS_NewGenericModule2(nsModuleInfo const *info, nsIModule* *result);

NS_GENERIC_FACTORY_CONSTRUCTOR(CDNSSDService)

static nsModuleComponentInfo components[] =
{
    {
       CDNSSDSERVICE_CLASSNAME, 
       CDNSSDSERVICE_CID,
       CDNSSDSERVICE_CONTRACTID,
       CDNSSDServiceConstructor,
    }
};

NS_IMPL_NSGETMODULE("CDNSSDServiceModule", components) 

