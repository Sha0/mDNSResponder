// ***************************************************************************
// Classic Mac (Open Transport) structures

#include <OpenTransport.h>
#include <OpenTptInternet.h>
#include <OpenTptClient.h>

typedef enum
	{
	mOT_Reset = 0,
	mOT_Start,
	mOT_ReusePort,
	mOT_RcvDestAddr,
	mOT_LLScope,
	mOT_AdminScope,
	mOT_Bind,
	mOT_Ready
	} mOT_State;

typedef struct { TOptionHeader h; mDNSIPAddr multicastGroupAddress; mDNSIPAddr InterfaceAddress; } TIPAddMulticastOption;
typedef struct { TOptionHeader h; UInt32 flag; } TSetBooleanOption;

// TOptionBlock is a union of various types.
// What they all have in common is that they all start with a TOptionHeader.
typedef union  { TOptionHeader h; TIPAddMulticastOption m; TSetBooleanOption b; } TOptionBlock;

struct mDNS_PlatformSupport_struct
	{
	EndpointRef ep;
	UInt32 mOTstate;				// mOT_State enum
	TOptionBlock optBlock;
	TOptMgmt optReq;
	long OTTimerTask;
	UInt32 nesting;
	NetworkInterfaceInfo interface;
	};
