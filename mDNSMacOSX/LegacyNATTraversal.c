/* -*- Mode: C; tab-width: 4 -*-
 *
 * Copyright (c) 2004 Apple Computer, Inc. All rights reserved.
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

$Log: LegacyNATTraversal.c,v $
Revision 1.29  2007/08/03 20:18:01  vazquez
<rdar://problem/5382177> LegacyNATTraversal: reading out of bounds can lead to DoS

Revision 1.28  2007/07/31 02:28:36  vazquez
<rdar://problem/3734269> NAT-PMP: Detect public IP address changes and base station reboot

Revision 1.27  2007/07/30 23:17:03  vazquez
Since lease times are meaningless in UPnP, return NATMAP_DEFAULT_LEASE in UPnP port mapping reply

Revision 1.26  2007/07/27 22:50:08  vazquez
Allocate memory for UPnP request and reply buffers instead of using arrays

Revision 1.25  2007/07/27 20:33:44  vazquez
Make sure we clean up previous port mapping requests before starting an unmap

Revision 1.24  2007/07/27 00:57:48  vazquez
If a tcp connection is already established for doing a port mapping, don't start it again

Revision 1.23  2007/07/26 21:19:26  vazquez
Retry port mapping with incremented port number (up to max) in order to handle
port mapping conflicts on UPnP gateways

Revision 1.22  2007/07/25 21:41:00  vazquez
Make sure we clean up opened sockets when there are network transitions and when changing
port mappings

Revision 1.21  2007/07/25 03:05:03  vazquez
Fixes for:
<rdar://problem/5338913> LegacyNATTraversal: UPnP heap overflow
<rdar://problem/5338933> LegacyNATTraversal: UPnP stack buffer overflow
and a myriad of other security problems

Revision 1.20  2007/07/16 20:15:10  vazquez
<rdar://problem/3867231> LegacyNATTraversal: Need complete rewrite

Revision 1.19  2007/06/21 16:37:43  jgraessley
Bug #: 5280520
Reviewed by: Stuart Cheshire
Additional changes to get this compiling on the embedded platform.

Revision 1.18  2007/05/09 01:43:32  cheshire
<rdar://problem/5187028> Change sprintf and strcpy to their safer snprintf and strlcpy equivalents

Revision 1.17  2007/02/27 02:48:25  cheshire
Parameter to LNT_GetPublicIP function is IPv4 address, not anonymous "mDNSOpaque32" object

Revision 1.16  2006/08/14 23:24:39  cheshire
Re-licensed mDNSResponder daemon source code under Apache License, Version 2.0

Revision 1.15  2006/07/05 23:30:57  cheshire
Rename LegacyNATInit() -> LNT_Init()

Revision 1.14  2005/12/08 03:00:33  cheshire
<rdar://problem/4349971> Byte order bugs in Legacy NAT traversal code

Revision 1.13  2005/09/07 18:23:05  ksekar
<rdar://problem/4151514> Off-by-one overflow in LegacyNATTraversal

Revision 1.12  2005/07/22 21:36:16  ksekar
Fix GCC 4.0/Intel compiler warnings

Revision 1.11  2004/12/03 03:34:20  ksekar
<rdar://problem/3882674> LegacyNATTraversal.c leaks threads

Revision 1.10  2004/12/01 02:43:49  cheshire
Update copyright message

Revision 1.9  2004/10/27 02:25:05  cheshire
<rdar://problem/3816029> Random memory smashing bug

Revision 1.8  2004/10/27 02:17:21  cheshire
Turn off "safe_close: ERROR" error messages -- there are too many of them

Revision 1.7  2004/10/26 21:15:40  cheshire
<rdar://problem/3854314> Legacy NAT traversal code closes file descriptor 0
Additional fixes: Code should set fds to -1 after closing sockets.

Revision 1.6  2004/10/26 20:59:20  cheshire
<rdar://problem/3854314> Legacy NAT traversal code closes file descriptor 0

Revision 1.5  2004/10/26 01:01:35  cheshire
Use "#if 0" instead of commenting out code

Revision 1.4  2004/10/10 06:51:36  cheshire
Declared some strings "const" as appropriate

Revision 1.3  2004/09/21 23:40:12  ksekar
<rdar://problem/3810349> mDNSResponder to return errors on NAT traversal failure

Revision 1.2  2004/09/17 01:08:52  cheshire
Renamed mDNSClientAPI.h to mDNSEmbeddedAPI.h
  The name "mDNSClientAPI.h" is misleading to new developers looking at this code. The interfaces
  declared in that file are ONLY appropriate to single-address-space embedded applications.
  For clients on general-purpose computers, the interfaces defined in dns_sd.h should be used.

Revision 1.1  2004/08/18 17:35:41  ksekar
<rdar://problem/3651443>: Feature #9586: Need support for Legacy NAT gateways


*/

#ifdef _LEGACY_NAT_TRAVERSAL_

#include "mDNSEmbeddedAPI.h"
#include "mDNSMacOSX.h"
#include "uDNS.h"
#include <arpa/inet.h>

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Structures, Defines and Constants
#endif

// used to format SOAP port mapping arguments
typedef struct Property_struct 
	{
	char		*propName;
	char		*propValue;
	char		*propType;
	} Property;

/*
 *	Note: This is an evolving list of useful acronyms to know. Please add to it at will.
 *	ST		Service Type
 *	NT		Notification Type
 *	USN		Unique Service Name
 *	UDN		Unique Device Name
 *	UUID		Universally Unique Identifier
 *	URN/urn	Universal Resource Name
 */
 
// NOTE: all of the text parsing in this file is intentionally transparent so that we know exactly 
// what's being done to the text, with an eye towards preventing security problems.

#define CRLF "\r\n"

// NAT discovery message
static const char szSSDPMsgDiscoverNAT[] =
	"M-SEARCH * HTTP/1.1\r\n"
	"Host:239.255.255.250:1900\r\n"
	"ST:urn:schemas-upnp-org:service:WANIPConnection:1\r\n"
	"Man:\"ssdp:discover\"\r\n"
	"MX:3\r\n"
	"\r\n";

// Device description format -
// 	- device description URL
// 	- host/port
static const char szSSDPMsgDescribeDeviceFMT[] =
	"GET %s HTTP/1.1\r\n"
	"Accept: text/xml, application/xml\r\n"
	"User-Agent: Mozilla/4.0 (compatible; UPnP/1.0; Windows NT/5.1)\r\n"
	"Host: %s\r\n"
	"Connection: close\r\n"
	"\r\n";

// SOAP message header format -
// 	- control URL
// 	- action (string)
// 	- router's host/port ("host:port")
// 	- content-length
static const char szSOAPMsgControlAHeaderFMT[] =
	"POST %s HTTP/1.1\r\n"
	"Content-Type: text/xml; charset=\"utf-8\"\r\n"
	"SOAPAction: \"urn:schemas-upnp-org:service:WANIPConnection:1#%s\"\r\n"
	"User-Agent: Mozilla/4.0 (compatible; UPnP/1.0; Windows 9x)\r\n"
	"Host: %s\r\n"
	"Content-Length: %d\r\n"
	"Connection: close\r\n"
	"Pragma: no-cache\r\n"
	"\r\n";

// SOAP message Body format -
// 	- action (string)
// 	- argument list
// 	- action (string)
static const char szSOAPMsgControlABodyFMT[] =
	"<?xml version=\"1.0\"?>" CRLF
	"<SOAP-ENV:Envelope" 
	" xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"" 
	" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">" 
	"<SOAP-ENV:Body>" 
	"<m:%s" 
	" xmlns:m=\"urn:schemas-upnp-org:service:WANIPConnection:1\">" 
	"%s" 
	"</m:%s>" 
	"</SOAP-ENV:Body>" 
	"</SOAP-ENV:Envelope>" 
	CRLF;

//SOAP message argument formats -
// 	- argument name
// 	- argument value
// 	- argument name
static const char szSOAPMsgControlAArgumentFMT[] =
	"<%s>%s</%s>" ;

// 	- argument name
// 	- argument type
// 	- argument value
// 	- argument name
static const char szSOAPMsgControlAArgumentFMT_t[] =
	"<%s"
	" xmlns:dt=\"urn:schemas-microsoft-com:datatypes\""
	" dt:dt=\"%s\">%s</%s>" ;

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - Internal Functions
#endif

mDNSlocal void AddSOAPArguments(char *argsBuffer, int numArgs, Property *Arguments, int *argsLen)
	{
	int	i;
	int	n;
	for (i = 0; i < numArgs; i++) 
		{
		n = 0;
		if (Arguments[i].propType == mDNSNULL)	
			n = mDNS_snprintf(argsBuffer + *argsLen, LNT_MAXBUFSIZE - *argsLen, szSOAPMsgControlAArgumentFMT, 
							Arguments[i].propName, Arguments[i].propValue, Arguments[i].propName);
		else							
			n = mDNS_snprintf(argsBuffer + *argsLen, LNT_MAXBUFSIZE - *argsLen, szSOAPMsgControlAArgumentFMT_t, 
							Arguments[i].propName, Arguments[i].propType, Arguments[i].propValue, Arguments[i].propName);
		*argsLen += n;
		}
	}

// This function parses the xml body of the device description response from the router. Basically, we look to make sure this is a response
// referencing a service we care about (WANIPConnection), look for the "controlURL" header immediately following, and copy the addressing and URL info we need
mDNSlocal void handleLNTDeviceDescriptionResponse(tcpLNTInfo *tcpInfo)
	{
	mDNS	*m 	= tcpInfo->m;
	char		*ptr 	= (char *)tcpInfo->Reply;
	char		*endBuf = (char *)tcpInfo->Reply + tcpInfo->nread;

	// find the service we care about
	while (ptr && ptr != endBuf) 
		{ 
		if (*ptr == 'W' && (strncasecmp(ptr, "WANIPConnection:1", 17) == 0)) break;	// find the first 'W'; is this WANIPConnection? if not, keep looking
		ptr++;
		}
	if (ptr == mDNSNULL || ptr == endBuf)	{ LogOperation("handleLNTDeviceDescriptionResponse: didn't find WANIPConnection:1 string"); return; }

	// find "controlURL", starting from where we left off
	while (ptr && ptr != endBuf) 
		{ 
		if (*ptr == 'c' && (strncasecmp(ptr, "controlURL", 10) == 0)) break;			// find the first 'c'; is this controlURL? if not, keep looking
		ptr++;
		}
	if (ptr == mDNSNULL || ptr == endBuf)	{ LogOperation("handleLNTDeviceDescriptionResponse: didn't find controlURL string"); return; }
	ptr += 11;							// skip over "controlURL>"
	if (ptr >= endBuf) { LogOperation("handleLNTDeviceDescriptionResponse: past end of buffer and no body!"); return; } // check ptr again in case we skipped over the end of the buffer

	// is there an address string "http://"? starting from where we left off
	if (strncasecmp(ptr, "http://", 7) == 0)
		{
		int	i;
		char	*addrPtr = mDNSNULL;
		
		ptr += 7;						//skip over "http://"
		if (ptr >= endBuf) { LogOperation("handleLNTDeviceDescriptionResponse: past end of buffer and no URL!"); return; }
		addrPtr = ptr;
		for (i = 0; addrPtr && addrPtr != endBuf; i++, addrPtr++) if (*addrPtr == '/') break; // first find the beginning of the URL and count the chars
		if (addrPtr == mDNSNULL || addrPtr == endBuf)	{ LogOperation("handleLNTDeviceDescriptionResponse: didn't find SOAP address string"); return; }

		// allocate the buffer	(len i+1 so we have space to terminate the string)
		if (m->uPNPSOAPAddressString != mDNSNULL)	mDNSPlatformMemFree(m->uPNPSOAPAddressString);
		if ((m->uPNPSOAPAddressString = (mDNSu8 *)	mDNSPlatformMemAllocate(i+1)) == mDNSNULL) { LogMsg("can't mDNSPlatformMemAllocate for SOAP address string"); return; }
		
		strncpy((char *)m->uPNPSOAPAddressString, ptr, i);				// copy the address string
		m->uPNPSOAPAddressString[i] = '\0';							// terminate the string
		}
	
	if (m->uPNPSOAPAddressString == mDNSNULL)	m->uPNPSOAPAddressString = m->uPNPRouterAddressString; // just copy the pointer, don't allocate more memory
	LogOperation("handleLNTDeviceDescriptionResponse: SOAP address string [%s]", m->uPNPSOAPAddressString);

	// find port and router URL, starting after the "http://" if it was there
	while (ptr && ptr != endBuf) 
		{ 
		if (*ptr == ':')										// found the port number
			{
			int	port;
			ptr++;										// skip over ':'
			if (ptr == endBuf) { LogOperation("handleLNTDeviceDescriptionResponse: reached end of buffer and no address!"); return; }
			port = (int)strtol(ptr, (char **)mDNSNULL, 10);				// get the port
			m->uPNPSOAPPort = mDNSOpaque16fromIntVal(port);		// store it properly converted
			}
		else if (*ptr == '/')									// found SOAP URL
			{
			int		j;
			char		*urlPtr = mDNSNULL;
			if (mDNSIPPortIsZero(m->uPNPSOAPPort))	m->uPNPSOAPPort = m->uPNPRouterPort;	// fill in default port if we didn't find one before

			urlPtr = ptr;
			for (j = 0; urlPtr && urlPtr != endBuf; j++, urlPtr++) if (*urlPtr == '<') break;	// first find the next '<' and count the chars
			if (urlPtr == mDNSNULL || urlPtr == endBuf) { LogOperation("handleLNTDeviceDescriptionResponse: didn't find SOAP URL string"); return; }

			// allocate the buffer	(len j+2 because we're copying from the first '/' and so we have space to terminate the string)
			if (m->uPNPSOAPURL != mDNSNULL)	mDNSPlatformMemFree(m->uPNPSOAPURL);
			if ((m->uPNPSOAPURL = (mDNSu8 *)	mDNSPlatformMemAllocate(j+1)) == mDNSNULL) { LogMsg("can't mDNSPlatformMemAllocate SOAP URL"); return; }
			
			// now copy
			strncpy((char *)m->uPNPSOAPURL, ptr, j);			// this URL looks something like "/uuid:0013-108c-4b3f0000f3dc"
			m->uPNPSOAPURL[j] = '\0';					// terminate the string
			break;									// we've got everything we need, so get out here
			}
		ptr++;	// continue
		}

	// if we get to the end and haven't found the URL fill in the defaults
	if (m->uPNPSOAPURL == mDNSNULL)	
		{
		m->uPNPSOAPURL = m->uPNPRouterURL;				// just copy the pointer, don't allocate more memory
		m->uPNPSOAPPort = m->uPNPRouterPort;
		}
	
	LogOperation("handleLNTDeviceDescriptionResponse: SOAP URL [%s] port %d\n", m->uPNPSOAPURL, mDNSVal16(m->uPNPSOAPPort));
	}

mDNSlocal void handleLNTGetExternalAddressResponse(tcpLNTInfo *tcpInfo)
	{
	mDNS		*m = tcpInfo->m;
	mStatus		err = mStatus_NoError;
	NATAddrReply	addrReply;
	char			*addrPtr;
	char			*ptr = (char *)tcpInfo->Reply;
	char			*endBuf = (char *)tcpInfo->Reply + tcpInfo->nread;

	// find "NewExternalIPAddress"
	while (ptr && ptr != endBuf) 
		{ 
		if (*ptr == 'N' && (strncasecmp(ptr, "NewExternalIPAddress", 20) == 0)) break;	// find the first 'N'; is this NewExternalIPAddress? if not, keep looking
		ptr++;
		}
	if (ptr == mDNSNULL || ptr == endBuf)	return;	// bad or incomplete response
	ptr+=21;									// skip over "NewExternalIPAddress>"
	if (ptr >= endBuf) { LogOperation("handleLNTGetExternalAddressResponse: past end of buffer!"); return; }

	// find the end of the address and terminate the string so inet_pton() can convert it
	for (addrPtr = ptr; addrPtr && addrPtr != endBuf; addrPtr++) if (*addrPtr == '<') break;	// first find the next '<' and count the chars
	if (addrPtr == mDNSNULL || addrPtr == endBuf) { LogOperation("handleLNTDeviceDescriptionResponse: didn't find SOAP URL string"); return; }
	*addrPtr = '\0';

//	LogOperation("handleLNTGetExternalAddressResponse: %s", ptr);
	if (inet_pton(AF_INET, ptr, &addrReply.PubAddr) <= 0)	{ LogMsg("handleLNTGetExternalAddressResponse: Router returned bad address"); err = mStatus_NATTraversal; }
	if (!err)
		{
		LogOperation("handleLNTGetExternalAddressResponse: Mapped remote host %.4a", &addrReply.PubAddr);
		addrReply.vers 		= 0;	// don't care about version
		addrReply.opcode	= NATOp_AddrResponse;
		addrReply.uptime	= m->timenow; 	// don't care about uptime
		}
	
	addrReply.err = err;
	natTraversalHandleAddressReply(m, (mDNSu8 *)&addrReply);
	}

// forward declaration
mDNSlocal mStatus SendSOAPMsgControlAction(mDNS *m, tcpLNTInfo *info, char *Action, int numArgs, Property *Arguments, LNTOp_t op);

#define LNT_MAXPORTMAP_RETRIES 10
// rebuild port mapping request with new port (up to max) and send it
mDNSlocal mStatus RetryPortMap(tcpLNTInfo *tcpInfo)
	{
	char		externalPort[6];
	char		internalPort[6];
	char		localIPAddrString[30];
	char		publicPortString[40];
	Property	propArgs[8];
	mDNS	*m 		= tcpInfo->m;
	NATTraversalInfo	*n = tcpInfo->parentNATInfo;
	mDNSs32	protocol	= (n->opFlags & MapTCPFlag) ? IPPROTO_TCP : IPPROTO_UDP;
	short		newPort	= mDNSVal16(n->publicPortreq);
	mStatus	error		= mStatus_NoError;

	// increment port map values; for port, just increment by one until we reach the max number of retries
	n->publicPortreq = mDNSOpaque16fromIntVal(++newPort);
	if (tcpInfo->retries++ > LNT_MAXPORTMAP_RETRIES) return (mStatus_NATTraversal);

	// create strings to use in the message
	mDNS_snprintf(externalPort, 		sizeof(externalPort), 		"%u", mDNSVal16(n->publicPortreq));
	mDNS_snprintf(internalPort, 		sizeof(internalPort), 		"%u", mDNSVal16(n->privatePort));
	mDNS_snprintf(publicPortString, 	sizeof(publicPortString), 	"iC%u", mDNSVal16(n->publicPortreq));
	mDNS_snprintf(localIPAddrString, 	sizeof(localIPAddrString), 	"%u.%u.%u.%u",
		m->AdvertisedV4.ip.v4.b[0], m->AdvertisedV4.ip.v4.b[1], m->AdvertisedV4.ip.v4.b[2], m->AdvertisedV4.ip.v4.b[3]);

	// build the message
	mDNSPlatformMemZero(propArgs, sizeof(propArgs));
	propArgs[0].propName = "NewRemoteHost";
	propArgs[0].propValue = "";
	propArgs[0].propType = "string";
	propArgs[1].propName = "NewExternalPort";
	propArgs[1].propValue = externalPort;
	propArgs[1].propType = "ui2";
	propArgs[2].propName = "NewProtocol";
	if		(protocol == IPPROTO_TCP) propArgs[2].propValue = "TCP";
	else if	(protocol == IPPROTO_UDP) propArgs[2].propValue = "UDP";
	else 		return (mStatus_BadParamErr);
	propArgs[2].propType = "string";
	propArgs[3].propName = "NewInternalPort";
	propArgs[3].propValue = internalPort;
	propArgs[3].propType = "ui2";
	propArgs[4].propName = "NewInternalClient";
	propArgs[4].propValue = localIPAddrString;
	propArgs[4].propType = "string";
	propArgs[5].propName = "NewEnabled";
	propArgs[5].propValue = "1";
	propArgs[5].propType = "boolean";
	propArgs[6].propName = "NewPortMappingDescription";
	propArgs[6].propValue = publicPortString;
	propArgs[6].propType = "string";
	propArgs[7].propName = "NewLeaseDuration";
	propArgs[7].propValue = "0";
	propArgs[7].propType = "ui4";

	LogOperation("RetryPortMap: priv %u pub %u", mDNSVal16(n->privatePort), mDNSVal16(n->publicPortreq));
	error = SendSOAPMsgControlAction(m, &n->tcpInfo, "AddPortMapping", 8, propArgs, LNTPortMapOp);
	return (error);
	}

// This will call natTraversalHandlePortMapReply() using a NATPortMapReply structure filled in with the necessary info
// (keeps us from replicating code)
mDNSlocal void handleLNTPortMappingResponse(tcpLNTInfo *tcpInfo)
	{
	mDNS	*m = tcpInfo->m;
	mStatus	err = mStatus_NoError;
	char		*ptr = (char *)tcpInfo->Reply;
	char		*endBuf = (char *)tcpInfo->Reply + tcpInfo->nread;
	NATPortMapReply	portMapReply;
	NATTraversalInfo	*natInfo;

	// start from the beginning of the HTTP header; find "200 OK" status message; if the first characters after the 
	// space are not "200" then this is an error message or invalid in some other way
	// if the error is "500" this is an internal server error
	while (ptr && ptr != endBuf) 
		{ 
		if (*ptr == ' ') 
			{ 
			ptr++;
			if (ptr == endBuf) { LogOperation("handleLNTPortMappingResponse: past end of buffer!"); return; }
			if (strncasecmp(ptr, "200", 3) == 0) break; 
			else if (strncasecmp(ptr, "500", 3) == 0)
				{
				// now check to see if this was a port mapping conflict
				while (ptr && ptr != endBuf)
					{
					if (*ptr == 'c' || *ptr == 'C')
						if (strncasecmp(ptr, "Conflict", 8) == 0) { err = RetryPortMap(tcpInfo);	 return; }
					ptr++;
					}
				break;	// out of HTTP status search
				}
			}
		ptr++;
		}
	if (ptr == mDNSNULL || ptr == endBuf)	{ LogMsg("handleLNTPortMappingResponse: got error from router");  err = mStatus_NATTraversal; }
	
	for (natInfo = m->NATTraversals; natInfo; natInfo=natInfo->next) { if (natInfo == tcpInfo->parentNATInfo) break; }
	
	if (!natInfo)	{ LogOperation("handleLNTPortMappingResponse: can't find matching tcpInfo in NATTraversals!"); return; }
	
	if (!err)
		{
		portMapReply.vers 	= 0;	// don't care about version
		portMapReply.opcode	= (natInfo->opFlags & MapUDPFlag) ? NATOp_MapUDPResponse : NATOp_MapTCPResponse;
		portMapReply.uptime	= 0; 	// don't care about uptime
		portMapReply.priv	= natInfo->privatePort;
		portMapReply.pub	= natInfo->publicPortreq;
		portMapReply.NATRep_lease = NATMAP_DEFAULT_LEASE;
		}
	
	portMapReply.err = err;
	LogOperation("handleLNTPortMappingResponse: got a valid response, sending reply to natTraversalHandlePortMapReply(priv %d pub %d)", mDNSVal16(portMapReply.priv), mDNSVal16(portMapReply.pub));
	natTraversalHandlePortMapReply(natInfo, m, (mDNSu8 *)&portMapReply);
	}

mDNSlocal void tcpConnectionCallback(TCPSocket *sock, void *context, mDNSBool ConnectionEstablished, mStatus err)
	{
	mStatus		status	= mStatus_NoError;
	tcpLNTInfo	*tcpInfo	= (tcpLNTInfo *)context;
	mDNSBool		closed	= mDNSfalse;
	long			n		= 0;
	long			nsent	= 0;

	if (tcpInfo == mDNSNULL)	{ LogOperation("tcpConnectionCallback: no tcpInfo context"); status = mStatus_Invalid; goto exit; }
	// XXX: this callback is called without the lock held
	mDNS_Lock(tcpInfo->m);
	
	if (err) 	{ LogOperation("tcpConnectionCallback: received error"); goto exit; }

	if (ConnectionEstablished)
		{
		// connection is established - send the message
		LogOperation("tcpConnectionCallback: connection established, sending message");
		nsent = mDNSPlatformWriteTCP(sock, (char *)tcpInfo->Request, tcpInfo->requestLen);
		if (nsent != (long)tcpInfo->requestLen) { LogMsg("tcpConnectionCallback: error writing"); status = mStatus_UnknownErr; goto exit; }
		}
	else
		{
		n = mDNSPlatformReadTCP(sock, (char *)tcpInfo->Reply + tcpInfo->nread, tcpInfo->replyLen - tcpInfo->nread, &closed);
		LogOperation("tcpConnectionCallback: mDNSPlatformReadTCP read %d bytes", n);

		if		(n < 0)	{ LogOperation("tcpConnectionCallback - read returned %d", n);						status = mStatus_ConnFailed; goto exit; }
		else if	(closed)	{ LogOperation("tcpConnectionCallback: socket closed by remote end %d", tcpInfo->nread);	status = mStatus_ConnFailed; goto exit; }

		tcpInfo->nread += n;
		LogOperation("tcpConnectionCallback tcpInfo->nread %d", tcpInfo->nread);
		if (tcpInfo->nread > LNT_MAXBUFSIZE) 
			{
			LogOperation("result truncated...");
			tcpInfo->nread = LNT_MAXBUFSIZE;
			}

		switch (tcpInfo->op)
			{
			case LNTDiscoveryOp:	handleLNTDeviceDescriptionResponse(tcpInfo);	break;
			case LNTExternalAddrOp:	handleLNTGetExternalAddressResponse(tcpInfo);	break;
			case LNTPortMapOp:		handleLNTPortMappingResponse(tcpInfo);		break;
			case LNTPortMapDeleteOp: status = mStatus_ConfigChanged; 			break;
			default:
				LogMsg("tcpConnectionCallback: bad tcp operation! %d", tcpInfo->op);	status = mStatus_Invalid; break;
			}
		}
exit:	
	if (err || status)	
		{ 
		mDNSPlatformTCPCloseConnection(tcpInfo->sock); 
		tcpInfo->sock = mDNSNULL;
		if (tcpInfo->Request != mDNSNULL)	{ mDNSPlatformMemFree(tcpInfo->Request); tcpInfo->Request = mDNSNULL; }
		if (tcpInfo->Reply != mDNSNULL)		{ mDNSPlatformMemFree(tcpInfo->Reply); tcpInfo->Reply = mDNSNULL; }
		}
	if (tcpInfo)		mDNS_Unlock(tcpInfo->m);	
	if (status == mStatus_ConfigChanged)
		{
		tcpLNTInfo **ptr = &tcpInfo->m->tcpInfoUnmapList;
		while (*ptr && *ptr != tcpInfo) ptr=&(*ptr)->next;
		if (*ptr) { *ptr = (*ptr)->next; mDNSPlatformMemFree(tcpInfo); }	// If we found it, cut it from our list and free the memory
		}
	}

mDNSlocal mStatus MakeTCPConnection(mDNS *const m, tcpLNTInfo *info, const mDNSAddr *const Addr, const mDNSIPPort Port, LNTOp_t op)
	{
	mStatus	  err		= mStatus_NoError;
	mDNSIPPort srcport 	= zeroIPPort;

	if (mDNSIPv4AddressIsZero(Addr->ip.v4) || mDNSIPPortIsZero(Port)) 
		{ LogMsg("LNT MakeTCPConnection: bad address/port (%#a : %d)", Addr, Port); return(mStatus_Invalid); }
	info->m 		= m;
	info->Address	= *Addr;
	info->Port		= Port;
	info->op		= op;
	info->nread	= 0;
	info->replyLen 	= LNT_MAXBUFSIZE;
	if 		(info->Reply != mDNSNULL)	mDNSPlatformMemZero(info->Reply, LNT_MAXBUFSIZE);	// reuse previously allocated buffer
	else if 	((info->Reply = (mDNSs8 *) 	mDNSPlatformMemAllocate(LNT_MAXBUFSIZE)) == mDNSNULL) { LogOperation("can't mDNSPlatformMemAllocate for reply buffer"); return (mStatus_NoMemoryErr); }

	if (info->sock) { LogOperation("MakeTCPConnection: closing previous open connection"); mDNSPlatformTCPCloseConnection(info->sock); info->sock = mDNSNULL; }
	info->sock = mDNSPlatformTCPSocket(m, kTCPSocketFlags_Zero, &srcport);
	if (!info->sock) { LogMsg("LNT MakeTCPConnection: unable to create TCP socket"); mDNSPlatformMemFree(info->Reply); info->Reply = mDNSNULL; return(mStatus_NoMemoryErr); }
	LogOperation("MakeTCPConnection: connecting to %#a : %d", &info->Address, mDNSVal16(info->Port));
	err = mDNSPlatformTCPConnect(info->sock, Addr, Port, 0, tcpConnectionCallback, info);

	if        (err == mStatus_ConnEstablished)	{ mDNS_Unlock(m); tcpConnectionCallback(info->sock, info, mDNStrue, mStatus_NoError);  mDNS_Lock(m); }	// drop lock around this call
	else if (err != mStatus_ConnPending    )	{ LogMsg("LNT MakeTCPConnection: connection failed"); mDNSPlatformMemFree(info->Reply); info->Reply = mDNSNULL; }
	if (err == mStatus_ConnEstablished || err == mStatus_ConnPending) err = mStatus_NoError;
	return(err);
	}

mDNSlocal mStatus SendSOAPMsgControlAction(mDNS *m, tcpLNTInfo *info, char *Action, int numArgs, Property *Arguments, LNTOp_t op)
	{
	mStatus	err			= mStatus_NoError;
	char	*sendBufferBody	= mDNSNULL;
	char	*sendBufferArgs		= mDNSNULL;
	int	headerLen		= 0;
	int	bodyLen		= 0;
	int	argsLen		= 0;

	// if no SOAP URL or address exists get out here
	if (m->uPNPSOAPURL == mDNSNULL || m->uPNPSOAPAddressString == mDNSNULL)	{ LogOperation("GetDeviceDescription: no SOAP URL or address string!"); return (mStatus_Invalid); }

	// handle arguments if any
	if ((sendBufferArgs = (char *) mDNSPlatformMemAllocate(LNT_MAXBUFSIZE)) == mDNSNULL) { LogOperation("can't mDNSPlatformMemAllocate for args buffer"); err = mStatus_NoMemoryErr; goto end; }
	if (Arguments != mDNSNULL)	AddSOAPArguments(sendBufferArgs, numArgs, Arguments, &argsLen);
	sendBufferArgs[argsLen] = '\0';

	// create message body
	bodyLen 			= LNT_MAXBUFSIZE - argsLen;	
	if ((sendBufferBody	= (char *) mDNSPlatformMemAllocate(bodyLen)) == mDNSNULL) { LogOperation("can't mDNSPlatformMemAllocate for body buffer"); err = mStatus_NoMemoryErr; goto end; }
	bodyLen 			= mDNS_snprintf(sendBufferBody, bodyLen, szSOAPMsgControlABodyFMT, Action, sendBufferArgs, Action);

	// create message header (bodyLen is embedded in the message to the router)
	if 		(info->Request != mDNSNULL)	mDNSPlatformMemZero(info->Request, LNT_MAXBUFSIZE);	// reuse previously allocated buffer
	else if 	((info->Request = (mDNSs8 *) 	mDNSPlatformMemAllocate(LNT_MAXBUFSIZE)) == mDNSNULL) { LogOperation("can't mDNSPlatformMemAllocate for send buffer"); err = mStatus_NoMemoryErr; goto end; }
	headerLen = mDNS_snprintf((char *)info->Request, LNT_MAXBUFSIZE - bodyLen, szSOAPMsgControlAHeaderFMT, m->uPNPSOAPURL, Action, m->uPNPSOAPAddressString, bodyLen);
	strlcpy((char *)(info->Request) + headerLen, sendBufferBody, headerLen+bodyLen);
	info->requestLen = headerLen+bodyLen;

	err = MakeTCPConnection(m, info, &m->Router, m->uPNPSOAPPort, op);

end:
	if (err && info->Request 	!= mDNSNULL)	{ mDNSPlatformMemFree(info->Request); info->Request = mDNSNULL; }
	if (sendBufferBody 		!= mDNSNULL)	mDNSPlatformMemFree(sendBufferBody);
	if (sendBufferArgs 		!= mDNSNULL)	mDNSPlatformMemFree(sendBufferArgs);
	return (err);
	}

mDNSlocal mStatus GetDeviceDescription(mDNS *m, tcpLNTInfo *info)
	{
	int		bufLen		= 0;
	mStatus	err			= mStatus_NoError;

	if (m->uPNPRouterURL == mDNSNULL || m->uPNPRouterAddressString == mDNSNULL)		{ LogOperation("GetDeviceDescription: no router URL or address string!"); return (mStatus_Invalid); }

	// build message
	if 		(info->Request != mDNSNULL)		mDNSPlatformMemZero(info->Request, LNT_MAXBUFSIZE);	// reuse previously allocated buffer
	else if 	((info->Request = (mDNSs8 *) 		mDNSPlatformMemAllocate(LNT_MAXBUFSIZE)) == mDNSNULL) { LogOperation("can't mDNSPlatformMemAllocate for send buffer for discovery"); return (mStatus_NoMemoryErr); }
	bufLen = mDNS_snprintf((char *)info->Request, LNT_MAXBUFSIZE, szSSDPMsgDescribeDeviceFMT, m->uPNPRouterURL, m->uPNPRouterAddressString);

	LogOperation("Describe Device: [%s]", info->Request);
	info->requestLen = bufLen;
	err = MakeTCPConnection(m, info, &m->Router, m->uPNPRouterPort, LNTDiscoveryOp);

	return (err);
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - Entry Points
#endif

mDNSexport mStatus LNT_UnmapPort(mDNS *m, NATTraversalInfo *n, mDNSBool doTCP)
	{
	char		externalPort[10];
	Property	propArgs[3];
	mDNSs32	protocol	= doTCP ? IPPROTO_TCP : IPPROTO_UDP;
	mStatus	error		= mStatus_NoError;
	tcpLNTInfo	*info;
	tcpLNTInfo 	**infoPtr;
	
	mDNS_snprintf(externalPort, sizeof(externalPort), "%u", mDNSVal16(n->publicPort));
	
	mDNSPlatformMemZero(propArgs, sizeof(propArgs));
	propArgs[0].propName = "NewRemoteHost";
	propArgs[0].propValue = "";
	propArgs[0].propType = "string";
	propArgs[1].propName = "NewExternalPort";
	propArgs[1].propValue = externalPort;
	propArgs[1].propType = "ui2";
	propArgs[2].propName = "NewProtocol";
	if 		(protocol == IPPROTO_TCP) propArgs[2].propValue = "TCP";
	else if 	(protocol == IPPROTO_UDP) propArgs[2].propValue = "UDP";
	else 		return (mStatus_BadParamErr);
	propArgs[2].propType = "string";

	n->tcpInfo.parentNATInfo = n;

	// clean up previous port mapping requests and allocations
	if (n->tcpInfo.sock) { LogOperation("LNT_UnmapPort: closing previous open connection"); mDNSPlatformTCPCloseConnection(n->tcpInfo.sock); n->tcpInfo.sock = mDNSNULL; }
	if (n->tcpInfo.Request != mDNSNULL)	{ mDNSPlatformMemFree(n->tcpInfo.Request); n->tcpInfo.Request = mDNSNULL; }
	if (n->tcpInfo.Reply != mDNSNULL)	{ mDNSPlatformMemFree(n->tcpInfo.Reply); n->tcpInfo.Reply = mDNSNULL; }
	
	// make a copy of the tcpInfo that we can clean up later (the one passed in will be destroyed by the client as soon as this returns)
	if ((info = mDNSPlatformMemAllocate(sizeof(tcpLNTInfo))) == mDNSNULL) { LogOperation("LNT_UnmapPort: can't mDNSPlatformMemAllocate for tcpInfo"); return(mStatus_NoMemoryErr); }
	*info = n->tcpInfo;
	
	// find the end of the list
	infoPtr = &m->tcpInfoUnmapList;
	while (*infoPtr) infoPtr=&(*infoPtr)->next;
	*infoPtr = info;	// append

	error = SendSOAPMsgControlAction(m, info, "DeletePortMapping", 3, propArgs, LNTPortMapDeleteOp);
	return (error);
	}

mDNSexport mStatus LNT_MapPort(mDNS *m, NATTraversalInfo *n, mDNSBool doTCP)
	{
	char		externalPort[6];
	char		internalPort[6];
	char		localIPAddrString[30];
	char		publicPortString[40];
	Property		propArgs[8];
	mDNSs32		protocol	= doTCP ? IPPROTO_TCP : IPPROTO_UDP;
	mStatus		error		= mStatus_NoError;

	// if we already have a connection up don't make another request for the same thing
	if (n->tcpInfo.sock && mDNSPlatformTCPIsConnected(n->tcpInfo.sock) == mDNStrue) 	return (mStatus_NoError);

	// create strings to use in the message
	mDNS_snprintf(externalPort, 		sizeof(externalPort), 		"%u", mDNSVal16(n->publicPortreq));
	mDNS_snprintf(internalPort, 		sizeof(internalPort), 		"%u", mDNSVal16(n->privatePort));
	mDNS_snprintf(publicPortString, 	sizeof(publicPortString), 	"iC%u", mDNSVal16(n->publicPortreq));
	mDNS_snprintf(localIPAddrString, 	sizeof(localIPAddrString), 	"%u.%u.%u.%u",
		m->AdvertisedV4.ip.v4.b[0], m->AdvertisedV4.ip.v4.b[1], m->AdvertisedV4.ip.v4.b[2], m->AdvertisedV4.ip.v4.b[3]);

	// build the message
	mDNSPlatformMemZero(propArgs, sizeof(propArgs));
	propArgs[0].propName = "NewRemoteHost";
	propArgs[0].propValue = "";
	propArgs[0].propType = "string";
	propArgs[1].propName = "NewExternalPort";
	propArgs[1].propValue = externalPort;
	propArgs[1].propType = "ui2";
	propArgs[2].propName = "NewProtocol";
	if		(protocol == IPPROTO_TCP) propArgs[2].propValue = "TCP";
	else if	(protocol == IPPROTO_UDP) propArgs[2].propValue = "UDP";
	else 		return (mStatus_BadParamErr);
	propArgs[2].propType = "string";
	propArgs[3].propName = "NewInternalPort";
	propArgs[3].propValue = internalPort;
	propArgs[3].propType = "ui2";
	propArgs[4].propName = "NewInternalClient";
	propArgs[4].propValue = localIPAddrString;
	propArgs[4].propType = "string";
	propArgs[5].propName = "NewEnabled";
	propArgs[5].propValue = "1";
	propArgs[5].propType = "boolean";
	propArgs[6].propName = "NewPortMappingDescription";
	propArgs[6].propValue = publicPortString;
	propArgs[6].propType = "string";
	propArgs[7].propName = "NewLeaseDuration";
	propArgs[7].propValue = "0";
	propArgs[7].propType = "ui4";

	LogOperation("Sending AddPortMapping priv %u pub %u", mDNSVal16(n->privatePort), mDNSVal16(n->publicPortreq));

	n->tcpInfo.parentNATInfo = n;
	n->tcpInfo.retries = 0;
	error = SendSOAPMsgControlAction(m, &n->tcpInfo, "AddPortMapping", 8, propArgs, LNTPortMapOp);
	return (error);
	}

mDNSexport mStatus LNT_GetExternalAddress(mDNS *m)
	{
	mStatus	error = mStatus_NoError;
	error = SendSOAPMsgControlAction(m, &m->tcpAddrInfo, "GetExternalIPAddress", 0, mDNSNULL, LNTExternalAddrOp);
	return (error);
	}

// This function parses the response to our SSDP discovery message. Basically, we look to make sure this is a response
// referencing a service we care about (WANIPConnection), then look for the "Location:" header and copy the addressing and
// URL info we need.
mDNSexport void LNT_ConfigureRouterInfo(mDNS *m, mDNSu8 *data, mDNSu16 len)
	{
	char	*ptr = (char *)data;
	char	*endBuf = (char *)data + len;

	// The formatting of the HTTP header is not always the same when it comes to the placement of 
	// the service and location strings, so we just look for each of them from the beginning for every response
	
	// figure out if this is a message from a service we care about
	while (ptr && ptr != endBuf) 
		{ 
		if (*ptr == 'W' && (strncasecmp(ptr, "WANIPConnection:1", 17) == 0)) break;	// find the first 'W'; is this WANIPConnection? if not, keep looking
		ptr++;
		}
	if (ptr == mDNSNULL || ptr == endBuf)	return;	// not a message we care about

	// find "Location:", starting from the beginning
	ptr = (char *)data;
	while (ptr && ptr != endBuf) 
		{ 
		if (*ptr == 'L' && (strncasecmp(ptr, "Location", 8) == 0)) break;			// find the first 'L'; is this Location? if not, keep looking
		ptr++;
		}
	if (ptr == mDNSNULL || ptr == endBuf)	return;	// not a message we care about
	
	// find "http://", starting from where we left off
	while (ptr && ptr != endBuf) 
		{ 
		if (*ptr == 'h' && (strncasecmp(ptr, "http://", 7) == 0))					// find the first 'h'; is this a URL? if not, keep looking
			{
			int	i;
			char	*addrPtr = mDNSNULL;
			
			ptr += 7;							//skip over "http://"
			if (ptr >= endBuf) { LogOperation("LNT_ConfigureRouterInfo: past end of buffer and no URL!"); return; }
			addrPtr = ptr;
			for (i = 0; addrPtr && addrPtr != endBuf; i++, addrPtr++) if (*addrPtr == '/') break;	// first find the beginning of the URL and count the chars
			if (addrPtr == mDNSNULL || addrPtr == endBuf) return; // not a valid message
	
			// allocate the buffer	(len i+1 so we have space to terminate the string)
			if (m->uPNPRouterAddressString != mDNSNULL)	mDNSPlatformMemFree(m->uPNPRouterAddressString);
			if ((m->uPNPRouterAddressString = (mDNSu8 *)	mDNSPlatformMemAllocate(i+1)) == mDNSNULL) { LogMsg("can't mDNSPlatformMemAllocate router address string"); return; }
			
			strncpy((char *)m->uPNPRouterAddressString, ptr, i);	// copy the address string
			m->uPNPRouterAddressString[i] = '\0';			// terminate the string
			LogOperation("LNT_ConfigureRouterInfo: router address string [%s]", m->uPNPRouterAddressString);
			break;
			}
		ptr++;	// continue
		}

	// find port and router URL, starting after the "http://" if it was there
	while (ptr && ptr != endBuf) 
		{ 
		if (*ptr == ':')										// found the port number
			{
			int	port;
			ptr++;										// skip over ':'
			if (ptr == endBuf) { LogOperation("LNT_ConfigureRouterInfo: reached end of buffer and no address!"); return; }
			port = (int)strtol(ptr, (char **)mDNSNULL, 10);			// get the port
			m->uPNPRouterPort = mDNSOpaque16fromIntVal(port);	// store it properly converted
			}
		else if (*ptr == '/')									// found router URL
			{
			int	j;
			char	*urlPtr;
			if (mDNSIPPortIsZero(m->uPNPRouterPort))		m->uPNPRouterPort = mDNSOpaque16fromIntVal(80);		// fill in default port if we didn't find one before
			
			urlPtr = ptr;
			for (j = 0; urlPtr && urlPtr != endBuf; j++, urlPtr++) if (*urlPtr == '\r') break;	// first find the end of the line and count the chars
			if (urlPtr == mDNSNULL || urlPtr == endBuf) return; // not a valid message
			
			// allocate the buffer	(len j+1 so we have space to terminate the string)
			if (m->uPNPRouterURL != mDNSNULL)	 mDNSPlatformMemFree(m->uPNPRouterURL);
			if ((m->uPNPRouterURL = (mDNSu8 *) mDNSPlatformMemAllocate(j+1)) == mDNSNULL) { LogMsg("can't mDNSPlatformMemAllocate for router URL"); return; }
			
			// now copy everything to the end of the line
			strncpy((char *)m->uPNPRouterURL, ptr, j);			// this URL looks something like "/dyndev/uuid:0013-108c-4b3f0000f3dc"
			m->uPNPRouterURL[j] = '\0';					// terminate the string
			break;									// we've got everything we need, so get out here
			}
		ptr++;	// continue
		}

	if (ptr == mDNSNULL || ptr == endBuf)	return;	// not a valid message
	LogOperation("Router port %d, URL set to[%s]...\n", mDNSVal16(m->uPNPRouterPort), m->uPNPRouterURL);

	// now send message to get the device description
	GetDeviceDescription(m, &m->tcpDeviceInfo);
	}

mDNSexport mStatus LNT_SendDiscoveryMsg(mDNS *m)
	{
	mStatus		err 	  = mStatus_NoError;
	const mDNSu8	*msg = mDNSNULL;
	const mDNSu8	*end = mDNSNULL;

	// send msg if we have a router
	if (!mDNSIPv4AddressIsZero(m->Router.ip.v4))
		{
		//WANIPConnection
		msg = (const mDNSu8 *)szSSDPMsgDiscoverNAT;
		end  = msg + sizeof(szSSDPMsgDiscoverNAT);
		err = mDNSPlatformSendUDP(m, msg, end, 0, &m->Router, SSDPPort);
		}
		
	return(err);
	}

#endif /* _LEGACY_NAT_TRAVERSAL_ */