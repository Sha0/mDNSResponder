// ***************************************************************************
// Support functions which must be provided by each set of specific PlatformSupport files

// mDNSPlatformInit() typically opens a communication endpoint, and starts listening for mDNS packets.
// When Setup is complete, the callback is called.
// mDNSPlatformSendUDP() sends one UDP packet
// When a packet is received, the PlatformSupport code calls mDNSCoreReceive()
// mDNSPlatformScheduleTask() indicates that a timer should be set,
// and mDNSCoreTask() should be called when the timer expires
// mDNSPlatformClose() tidies up on exit

#ifdef	__cplusplus
	extern "C" {
#endif

// ***************************************************************************
// DNS protocol message format

typedef struct
	{
	mDNSOpaque16 id;
	mDNSOpaque16 flags;
	mDNSu16 numQuestions;
	mDNSu16 numAnswers;
	mDNSu16 numAuthorities;
	mDNSu16 numAdditionals;
	} DNSMessageHeader;

typedef struct
	{
	DNSMessageHeader h;		// Note: Size 12 bytes
	mDNSu8 data[1460];		// Total packet size 1500 = 20 (IP) + 8 (UDP) + 12 (header) + 1460 (data)
	} DNSMessage;

// ***************************************************************************
// Functions

// Every platform support module must provide the following functions
extern mStatus  mDNSPlatformInit   (mDNS *const m);
extern void     mDNSPlatformClose  (mDNS *const m);
extern mStatus  mDNSPlatformSendUDP(const mDNS *const m, const DNSMessage *const msg, const mDNSu8 *const end,
	mDNSIPAddr src, mDNSIPPort srcport, mDNSIPAddr dst, mDNSIPPort dstport);

extern void     mDNSPlatformScheduleTask(const mDNS *const m, mDNSs32 NextTaskTime);
extern void     mDNSPlatformLock        (const mDNS *const m);
extern void     mDNSPlatformUnlock      (const mDNS *const m);

extern void     mDNSPlatformStrCopy(const void *src,       void *dst);
extern mDNSu32  mDNSPlatformStrLen (const void *src);
extern void     mDNSPlatformMemCopy(const void *src,       void *dst, mDNSu32 len);
extern mDNSBool mDNSPlatformMemSame(const void *src, const void *dst, mDNSu32 len);
extern void     mDNSPlatformMemZero(                       void *dst, mDNSu32 len);
extern mDNSs32  mDNSPlatformTimeNow();
extern mDNSs32  mDNSPlatformOneSecond;

// The core mDNS code provides these functions, for the platform support code to call at appropriate times
extern void     mDNSCoreReceive(mDNS *const m, DNSMessage *const msg, const mDNSu8 *const end,
								mDNSIPAddr srcaddr, mDNSIPPort srcport, mDNSIPAddr dstaddr, mDNSIPPort dstport, mDNSIPAddr InterfaceAddr);
extern void     mDNSCoreTask   (mDNS *const m);
extern void     mDNSCoreSleep  (mDNS *const m, mDNSBool wake);

#ifdef	__cplusplus
	}
#endif
