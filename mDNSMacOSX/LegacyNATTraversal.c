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
#pragma mark - Defines and Constants
#endif

#define LNT_BUFSIZE 1024		// standard size of our URL buffers
#define MAX_SOAPMSGSIZE 65536
#define CRLF "\r\n"
#define H_CRLF "\r\n"
// SOAP message's CRLF:
//#define S_CRLF "\r\n"
#define S_CRLF

// NAT discovery message
static const char szSSDPMsgDiscoverNAT[] =
	"M-SEARCH * HTTP/1.1\r\n"
	"Host:239.255.255.250:1900\r\n"
	"ST:urn:schemas-upnp-org:service:WANIPConnection:1\r\n"
	"Man:\"ssdp:discover\"\r\n"
	"MX:3\r\n"
	"\r\n";

// Device description format -
// 1$: device description URL
// 2$: host/port
static const char szSSDPMsgDescribeDeviceFMT[] =
	"GET %s HTTP/1.1\r\n"
	"Accept: text/xml, application/xml\r\n"
	"User-Agent: Mozilla/4.0 (compatible; UPnP/1.0; Windows NT/5.1)\r\n"
	"Host: %s\r\n"
	"Connection: close\r\n"
	"\r\n";

// SOAP message header format -
// 1$s: control URL
// 2$s: router's host/port ("host:port")
// 3$s: action (string)
// 4$d: content-length
static const char szSOAPMsgControlAHeaderFMT[] =
	"POST %1$s HTTP/1.1\r\n"
	"Content-Type: text/xml; charset=\"utf-8\"\r\n"
	"SOAPAction: \"urn:schemas-upnp-org:service:WANIPConnection:1#%3$s\"\r\n"
	"User-Agent: Mozilla/4.0 (compatible; UPnP/1.0; Windows 9x)\r\n"
	"Host: %2$s\r\n"
	"Content-Length: %4$d\r\n"
	"Connection: close\r\n"
	"Pragma: no-cache\r\n"
	"\r\n";

// SOAP message Body format -
// 1$: action (string)
// 2$: argument list
static const char szSOAPMsgControlABodyFMT[] =
	"<?xml version=\"1.0\"?>" CRLF
	"<SOAP-ENV:Envelope" S_CRLF
	" xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"" S_CRLF
	" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">" S_CRLF
	"<SOAP-ENV:Body>" S_CRLF
	"<m:%1$s" S_CRLF
	" xmlns:m=\"urn:schemas-upnp-org:service:WANIPConnection:1\">" S_CRLF
	"%2$s" 
	"</m:%1$s>" S_CRLF
	"</SOAP-ENV:Body>" S_CRLF
	"</SOAP-ENV:Envelope>" S_CRLF
	CRLF;

//SOAP message argument formats -
// 1$: argument name
// 2$: argument value
static const char szSOAPMsgControlAArgumentFMT[] =
	"<%1$s>%2$s</%1$s>" S_CRLF;

// 1$: argument name
// 2$: argument value
// 3$: argument type
static const char szSOAPMsgControlAArgumentFMT_t[] =
	"<%1$s"
	" xmlns:dt=\"urn:schemas-microsoft-com:datatypes\""
	" dt:dt=\"%3$s\">%2$s</%1$s>" S_CRLF;

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - Utility Functions and HTTP Parsing
#endif

// this is strnstr, only portable
mDNSlocal char *strstr_n(const char *big, const char *little)
	{
	int		iBigLen;
	int		iLittleLen;

	if ((big == mDNSNULL) || (little == mDNSNULL)) return (mDNSNULL);

	iBigLen = strlen(big);
	iLittleLen = strlen(little);
	// this part is basically strnstr, except this is portable
	for (;;) 
		{
		if (iBigLen < iLittleLen)				return (mDNSNULL);
		if (strncmp(big, little, iLittleLen) == 0)	return ((char *)big);
		++big;
		--iBigLen;
		}
	}

// FindHTTPHeaderNewLine
//   Returns a pointer to the beginning of a CRLF, that is not a part of LWS.  (LWS is CRLF followed by a space or tab, and in
//   HTTP, considered as equivalent to a single space) (LWS stands for "linear white space")
// Returns a pointer the beginning of CRLF, and sets the EOH flag to whether this is the last header in the HTTP header section.
//   Also, if pbuf is NULL, or if there isn't any CRLF found in the string, or if the HTTP syntax is wrong, NULL is returned, and
//   the EOH flag is not touched.
mDNSlocal char *FindHTTPHeaderNewLine(char *pbuf, int iBufSize, int *pfEOH)
	{
	char *result;
	int i = 0;

	if (pbuf == NULL) return (mDNSNULL);

	for (;;) 
		{
		result = memchr(pbuf, '\r', iBufSize);
		if (result == NULL) { LogOperation("FindHTTPHeaderNewLine: er @(%d/%d)", i, iBufSize); return (mDNSNULL); }
		i++; // count chars

		// decrement iBufSize, and move pbuf forward
		iBufSize -= (result - pbuf);
		pbuf = result;

		++pbuf;  // now pointing right after "\r"
		--iBufSize;
		if (*pbuf == '\0') break;
		if (*pbuf != '\n') continue;

		++pbuf;  // now pointing after "\r\n"
		--iBufSize;
		if (*pbuf == '\0') break;
		if ((*pbuf == ' ') || (*pbuf == '\t')) continue;

		// at this point we know we're at the end of a header field,
		// and there's more stuff coming...

		// just need to check if this is the last header
		if ((pbuf[0] == '\r') && (pbuf[1] == '\n'))	*pfEOH = TRUE;
		else								*pfEOH = FALSE;

		return (result);
		}

	return (mDNSNULL);
	}

mDNSlocal mStatus ExtractResponseHeaders(HTTPResponse *Response, char *responseBuffer, int responseLen, int destroyOriginal)
	{
	int	isLastHeader = 0;
	int	headerCount = 0;
	char	*lastLine       = mDNSNULL;
	char	*pBuf            = mDNSNULL;

	if (Response == mDNSNULL) { LogMsg("ExtractResponseHeaders: Response = NULL"); return (mStatus_NATTraversal); }
	// make copy of buffer now if necessary
	if (destroyOriginal) { Response->buf = NULL; pBuf = responseBuffer; }
	else 
		{
		int actualLen = strlen(responseBuffer);
		if ((actualLen+1) > responseLen)	{ LogOperation("Length: %d > %d", actualLen+1, responseLen); responseLen = actualLen+1; }
		
		Response->buf = (mDNSs8 *)mDNSPlatformMemAllocate(responseLen);
		if (Response->buf == mDNSNULL)	{ LogMsg("ExtractResponseHeaders: malloc returned error"); return (mStatus_NoMemoryErr); }
		
		mDNSPlatformMemCopy(Response->buf, responseBuffer, responseLen);
		pBuf = (char *)Response->buf;
		}

	// get the first line
	lastLine = FindHTTPHeaderNewLine(pBuf, responseLen, &isLastHeader);
	if (lastLine == mDNSNULL)                                 { LogMsg("ExtractResponseHeaders: FindHTTPHeaderNewLine found error"); goto error; }

	*lastLine = '\0';    				// terminate the status line
	lastLine += 2;       				// point to the rest of the buffer

	// set the status string
	Response->Status = (mDNSs8 *)strchr(pBuf, ' ');
	if (Response->Status == mDNSNULL)              { LogMsg("ExtractResponseHeaders: syntax error (status)"); goto error; }	// syntax error

	Response->Status++;			// point to the actual status

	Response->Reason = (mDNSs8 *)strchr((char *)Response->Status, ' ');
	if (Response->Reason == mDNSNULL)             { LogMsg("ExtractResponseHeaders: syntax error (reason)"); goto error; }	// syntax error
	else
		{
		Response->Reason[0] = '\0';  	// terminate status string
		Response->Reason++;            	// point to the reason string
		}

	// parse header fields line by line (while not end of headers)
	while (!isLastHeader) 
		{
		Property	*curHeader = &(Response->Headers[headerCount]);
		int		remainderLen = 0;

		curHeader->propName = lastLine;	// point header field name to the first char of the line

		// search for the end of line
		remainderLen = responseLen - (lastLine - pBuf);
		lastLine = FindHTTPHeaderNewLine(lastLine, remainderLen, &isLastHeader);
		if (lastLine == NULL) 
			{
			LogMsg("ExtractResponseHeaders: error reading header field %d @ %lu / %lu\n", headerCount, ((char *)curHeader->propName - pBuf), responseLen);
			goto error;  // syntax error
			}

		*lastLine = '\0';					// terminate this string
		lastLine += 2;					// point to beginning of next line

		curHeader->propValue = strchr((char *)curHeader->propName, ':');
		if (curHeader->propValue == NULL) { LogMsg("ExtractResponseHeaders: syntax error (header field has no ':')"); goto error; }

		curHeader->propValue[0] = '\0';		// terminate the header name string
		curHeader->propValue++;			// point after the ":"
		
		// get rid of leading spaces
		while ((curHeader->propValue[0] == ' ') || (curHeader->propValue[0] == '\t') || (curHeader->propValue[0] == '\r') || (curHeader->propValue[0] == '\n')) 
			{ curHeader->propValue++; }	// skip the space

		headerCount++;					// added one more header
		curHeader++;					// point to the next header in Response->Headers
		}

	Response->numHeaders	= headerCount;
	Response->Body		= (mDNSs8 *)(lastLine + 2);	// point after the empty line

	return (mStatus_NoError);

error:
	if (Response->buf != NULL) mDNSPlatformMemFree(Response->buf);
	return (mStatus_NATTraversal);
	}

// non reusable XML parsing code
mDNSlocal mDNSBool FindURLBase(char *pbuf, char *szURLBase)
	{
	char	*p;
	int	  i = 0;

	// now skip after end of this tag, then skip until controlURL tag
	p = strstr_n(pbuf, "<URLBase>");
	if (p == NULL) return (mDNSfalse);

	// skip to the actual stuff
	p += sizeof("<URLBase>") - 1;  // minus '\0'

	// skip white spaces (just in case)
	while (isspace(*p)) p++;

	// copy into szURLBase
	while ((*p != '\0') && (*p != '<') && !isspace(*p)) 
		{
		if (i++ > 1000) break;
		*szURLBase = *p;
		szURLBase++;
		p++;
		}
	*szURLBase = '\0';

	return (mDNStrue);
	}

mDNSlocal mDNSBool FindControlURL(char *pbuf, int len, char *value, mDNSBool copyValue)
	{
	char	parentName[] 	= "urn:schemas-upnp-org:service:WANIPConnection:1";
	char	name[]		= "controlURL";
	char	*p = mDNSNULL;
	char	searchBuf[100];
	int	searchLen = 0;
	int	i = 0;

	// find the device within pbuf
	p = strstr_n(pbuf, parentName);
	if (p == NULL)	return (mDNSfalse);

	// adjust strlen
	len -= (p - pbuf);
	pbuf = p;

	// now skip after end of this tag, then skip until manufacturer tag
	searchLen = snprintf(searchBuf, sizeof(searchBuf), "<%s>", name);
	p = strstr_n(pbuf, searchBuf);
	if (p == NULL) return (mDNSfalse);
	p += searchLen;

	if (copyValue == mDNStrue)
		{
		// skip white spaces (just in case)
		while (isspace(*p))	p++;
	
		// copy into value
		while ((*p != '\0') && (*p != '<')) 
			{
			if (i++ > 1000) break;
			*value = *p;
			value++;
			p++;
			}
		*value = '\0';
		}
	return (mDNStrue);
	}

mDNSlocal void ParseURL(const char *buffer, char *addrString, int addrStringLen, mDNSIPPort *port, char *URLString, int URLStringLen)
	{
	char	  buf[LNT_BUFSIZE];
	char	  *p = mDNSNULL;
	char	  *q = mDNSNULL;
	mDNSIPPort tmpPort = zeroIPPort;

	strlcpy(buf, buffer, sizeof(buf));

	p = buf;
	if (0 == strncmp(p, "http://", 7))	p += 7;	// now pointing to address in URL

	q = strchr(p, '/');

	if (URLString) 
		{
		if (mDNSNULL == q) 
			{
			URLString[0] = '/';
			URLString[1] = '\0';
			}
		else  
			{
			strlcpy(URLString, q, URLStringLen);
			*q = '\0';
			}
		}

	// find the port separator
	q = strchr(p, ':');
	if (NULL == q)	tmpPort = mDNSOpaque16fromIntVal(80);
	else 
		{
		tmpPort = mDNSOpaque16fromIntVal(atoi(q + 1));
		if (80 == mDNSVal16(tmpPort)) *q = '\0';	// HTTP is by default port 80, so don't have it in the "Host:" header
		}

	if (addrString)	strlcpy(addrString, p, addrStringLen);
	if (NULL != q)	*q = '\0';
	*port = tmpPort;

	LogOperation("ParseURL [%s] -> [%s][%s] %u\n", buffer, addrString?addrString:"", URLString?URLString:"", port);
	}

mDNSlocal void AddSOAPArguments(char *argsBuffer, int numArgs, Property *Arguments, int *argsLen)
	{
	int	i;
	int	n;
	
	for (i = 0; i < numArgs; i++) 
		{
		n = 0;
		if (Arguments[i].propType == NULL) 
			{
			n = snprintf(argsBuffer + *argsLen, MAX_SOAPMSGSIZE - *argsLen, szSOAPMsgControlAArgumentFMT,
					Arguments[i].propName, Arguments[i].propValue);
			}
		else 
			{
			n = snprintf(argsBuffer + *argsLen, MAX_SOAPMSGSIZE - *argsLen, szSOAPMsgControlAArgumentFMT_t,
					Arguments[i].propName, Arguments[i].propValue, Arguments[i].propType);
			}
		*argsLen += n;
		}
	}

// buf != NULL means parse xml and copy
mDNSlocal mStatus FindTagContent(const char *text, const char *tagname, char *buf)
	{
	char	*p;
	
	// parse the xml
	p = strstr(text, tagname);
	if (p == NULL) 						         { LogOperation("FindTagContent: can't find %s", tagname); return (mStatus_NATTraversal); }
	if (buf != mDNSNULL)
		if (sscanf(p, "%*[^>]> %[^ <] <", buf) < 1) { LogOperation("FindTagContent: Can't parse tag %s", tagname); return (mStatus_NATTraversal); }

	return (mStatus_NoError);
	}

mDNSlocal void PrintHTTPResponse(HTTPResponse *Response)
	{
	int	i;
	if (Response == NULL) return;
	
	LogOperation(" *** HTTP response begin *** \n");
	LogOperation(" * status = [%s], reason = [%s] *\n", Response->Status, Response->Reason);
	for (i = 0; i < Response->numHeaders; i++) 
		LogOperation(" * Header \"%s\" = [%s]\n", Response->Headers[i].propName, Response->Headers[i].propValue);
	LogOperation(" * body = [%s] *\n", Response->Body);
	LogOperation(" *** HTTP response end *** \n");
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - Internal Functions
#endif

mDNSlocal void handleLNTDeviceDescriptionResponse(tcpLNTInfo *tcpInfo)
	{
	mDNS	*m = tcpInfo->m;
	char		URLBaseBuf[LNT_BUFSIZE];
	char		controlURLBuf[LNT_BUFSIZE];
	char		RouterBasePortString[LNT_BUFSIZE];
	mDNSIPPort	BasePort = zeroIPPort;

	if (FindControlURL((char *)tcpInfo->reply, tcpInfo->replylen, controlURLBuf, mDNStrue) != mDNStrue) { LogMsg("GetDeviceDesc: can't find control URL"); return; }

	// now see if there's the URLBase
	if (FindURLBase((char *)tcpInfo->reply, URLBaseBuf) != mDNStrue) 
		{
		// use default numbers from device description
		BasePort = m->uPNPRouterPort;
		strlcpy(RouterBasePortString, (char *)m->uPNPRouterAddressString, sizeof(RouterBasePortString));
		}
	else 
		{
		ParseURL(URLBaseBuf, RouterBasePortString, sizeof(RouterBasePortString), &BasePort, mDNSNULL, 0);

		if ((strlen(RouterBasePortString) == 0) || mDNSIPPortIsZero(BasePort))
			{
			BasePort = m->uPNPRouterPort;
			strlcpy(RouterBasePortString, (char *)m->uPNPRouterAddressString, sizeof(RouterBasePortString));
			}
		}

	// Control URL
	ParseURL(controlURLBuf, (char *)m->uPNPSOAPAddressString, LNT_BUFSIZE, &m->uPNPSOAPPort, (char *)m->uPNPSOAPURL, LNT_BUFSIZE);

	if ((strlen((char *)m->uPNPSOAPAddressString) == 0) || mDNSIPPortIsZero(m->uPNPSOAPPort)) 
		{
		// use base default
		m->uPNPSOAPPort = BasePort;
		strlcpy((char *)m->uPNPSOAPAddressString, RouterBasePortString, LNT_BUFSIZE);
		}

	if (tcpInfo->Response.buf) { mDNSPlatformMemFree(tcpInfo->Response.buf); tcpInfo->Response.buf = mDNSNULL; }
	}

mDNSlocal void handleLNTGetExternalAddressResponse(tcpLNTInfo *tcpInfo)
	{
	mDNS	*m = tcpInfo->m;
	mStatus	err = mStatus_NoError;
	char		buf[LNT_BUFSIZE];
	NATAddrReply	addrReply;

	if (FindTagContent((char *)tcpInfo->Response.Body, "NewExternalIPAddress", buf) == mStatus_NoError) 
		{
		LogOperation("handleLNTGetExternalAddressResponse: Mapped remote host = %s", buf);
		if (inet_pton(AF_INET, buf, &addrReply.PubAddr) <= 0) 
			{ LogMsg("handleLNTGetExternalAddressResponse: Router returned bad address"); err = mStatus_NATTraversal; }
		}

	if (!err)
		{
		addrReply.vers 		= 0;	// don't care about version
		addrReply.opcode	= NATOp_AddrResponse;
		addrReply.uptime	= 0; 	// don't care about uptime
		}
	
	addrReply.err = err;
	natTraversalHandleAddressReply(m, (mDNSu8 *)&addrReply);
	if (tcpInfo->Response.buf) { mDNSPlatformMemFree(tcpInfo->Response.buf); tcpInfo->Response.buf = mDNSNULL; }
	}

// This will call natTraversalHandlePortMapReply() using a NATPortMapReply structure filled in with the necessary info
// (keeps us from replicating code)
mDNSlocal void handleLNTPortMappingResponse(tcpLNTInfo *tcpInfo)
	{
	mDNS	*m = tcpInfo->m;
	mStatus	err = mStatus_NoError;
	NATPortMapReply	portMapReply;
	NATTraversalInfo	*ptr;

	if (strcmp((char *)tcpInfo->Response.Status, "200") != 0) { LogMsg("handleLNTPortMappingResponse: got error from router"); err = mStatus_NATTraversal; }
	
	for (ptr = m->NATTraversals; ptr; ptr=ptr->next) { if (ptr->tcpInfo == tcpInfo) break; }
	
	if (!ptr) 
		{ 
		// this could occur from an unmap operation or if we have an incomplete response
		LogOperation("handleLNTPortMappingResponse: can't find matching tcpInfo in NATTraversals"); 
		if (tcpInfo->Response.buf) { mDNSPlatformMemFree(tcpInfo->Response.buf); tcpInfo->Response.buf = mDNSNULL; } 
		return; 
		}
	
	if (!err)
		{
		ptr->opFlags 		|= LegacyFlag;
		portMapReply.vers 	= 0;	// don't care about version
		portMapReply.opcode	= (ptr->opFlags & MapUDPFlag) ? NATOp_MapUDPResponse : NATOp_MapTCPResponse;
		portMapReply.uptime	= 0; 	// don't care about uptime
		portMapReply.priv	= ptr->privatePort;
		portMapReply.pub	= ptr->publicPortreq;
		portMapReply.NATRep_lease	= ptr->portMappingLease;
		}
	
	portMapReply.err = err;
	LogOperation("handleLNTPortMappingResponse: got a valid response, sending reply to natTraversalHandlePortMapReply()");
	LogOperation("%p priv %d pub %d", ptr, mDNSVal16(portMapReply.priv), mDNSVal16(portMapReply.pub));
	natTraversalHandlePortMapReply(ptr, m, (mDNSu8 *)&portMapReply);
	if (tcpInfo->Response.buf) { mDNSPlatformMemFree(tcpInfo->Response.buf); tcpInfo->Response.buf = mDNSNULL; }
	}

mDNSlocal void tcpConnectionCallback(TCPSocket *sock, void *context, mDNSBool ConnectionEstablished, mStatus err)
	{
	mStatus		status	= mStatus_NoError;
	tcpLNTInfo	*tcpInfo	= (tcpLNTInfo *)context;
	mDNSBool		closed	= mDNSfalse;
	long			n		= 0;
	long			nsent	= 0;

	if (err) 				{ LogMsg("tcpConnectionCallback: received error"); goto exit; }
	if (tcpInfo == mDNSNULL)	{ LogMsg("tcpConnectionCallback: bad tcpInfo context"); goto exit; }

	// this callback is called without the lock held
	mDNS_Lock(tcpInfo->m);

	if (ConnectionEstablished)
		{
		// connection is established - send the message
		LogOperation("tcpConnectionCallback: connection established, sending message");
		nsent = mDNSPlatformWriteTCP(sock, (char *)tcpInfo->request, tcpInfo->requestLen);
		if (nsent != (long)tcpInfo->requestLen) { LogMsg("tcpConnectionCallback: error writing"); status = mStatus_UnknownErr; goto exit; }
		}
	else
		{
		n = mDNSPlatformReadTCP(sock, (char *)tcpInfo->reply + tcpInfo->nread, tcpInfo->replylen - tcpInfo->nread, &closed);
		LogOperation("tcpConnectionCallback: mDNSPlatformReadTCP read %d bytes", n);

		if		(n < 0)	{ LogMsg        ("tcpConnectionCallback - read returned %d", n);						status = mStatus_ConnFailed; goto exit; }
		else if	(closed)	{ LogOperation("tcpConnectionCallback: socket closed by remote end %d", tcpInfo->nread);	status = mStatus_ConnFailed; goto exit; }

		tcpInfo->nread += n;
		tcpInfo->numReplies++;
		LogOperation("tcpConnectionCallback tcpInfo->nread %d", tcpInfo->nread);
		if (tcpInfo->nread > MAX_SOAPMSGSIZE) 
			{
			LogOperation("result truncated...");
			tcpInfo->nread = MAX_SOAPMSGSIZE;
			}

		if (FindControlURL((char *)tcpInfo->reply, tcpInfo->nread, mDNSNULL, mDNSfalse) == mDNStrue)	handleLNTDeviceDescriptionResponse(tcpInfo);
		else
			{
			status = ExtractResponseHeaders(&tcpInfo->Response, (char *)tcpInfo->reply, tcpInfo->nread, FALSE);
			if (status) { LogMsg("tcpConnectionCallback: received invalid message from router"); status = mStatus_Invalid; goto exit; }
			PrintHTTPResponse(&tcpInfo->Response); 
			
			if (FindTagContent((char *)tcpInfo->Response.Body, "NewExternalIPAddress", mDNSNULL) == mStatus_NoError) handleLNTGetExternalAddressResponse(tcpInfo);
			else handleLNTPortMappingResponse(tcpInfo);
			}
		}
exit:	
	if (err || status)
		{
		mDNSPlatformTCPCloseConnection(sock);
		if (tcpInfo->reply)
			{
			mDNSPlatformMemFree(tcpInfo->reply);
			tcpInfo->reply = mDNSNULL;
			}
		}
	mDNS_Unlock(tcpInfo->m);
	}

mDNSlocal tcpLNTInfo *MakeTCPConnection(mDNS *const m, const mDNSu8 *const msg, const mDNSu8 *const end, const mDNSAddr *const Addr, const mDNSIPPort Port)
	{
	mStatus		err;
	mDNSIPPort	srcport = zeroIPPort;
	tcpLNTInfo	*info = (tcpLNTInfo *)mDNSPlatformMemAllocate(sizeof(tcpLNTInfo));
	if (!info) { LogMsg("ERROR: MakeTCPConnection - memallocate failed"); return(mDNSNULL); }

	mDNSPlatformMemZero(info, sizeof(tcpLNTInfo));
	info->m = m;
	if (msg)
		{
		info->request		= (mDNSs8 *)msg;
		info->requestLen	= (int)(end - msg);
		}
	if (mDNSIPv4AddressIsZero(Addr->ip.v4) || mDNSIPPortIsZero(Port)) 
		{ LogMsg("LNT MakeTCPConnection: bad address/port (%#a : %d)", Addr, Port); mDNSPlatformMemFree(info); return(mDNSNULL); }
	info->Addr		= *Addr;
	info->Port		= Port;

	info->replylen 	= MAX_SOAPMSGSIZE;
	info->reply 	= mDNSPlatformMemAllocate(info->replylen);
	if (!info->reply)	{ LogMsg("LNT MakeTCPConnection: unable to allocate reply buffer"); mDNSPlatformMemFree(info); return(mDNSNULL); }

	info->sock = mDNSPlatformTCPSocket(m, kTCPSocketFlags_Zero, &srcport);
	if (!info->sock) { LogMsg("LNT MakeTCPConnection: uanble to create TCP socket"); mDNSPlatformMemFree(info); mDNSPlatformMemFree(info->reply); return(mDNSNULL); }
	LogOperation("MakeTCPConnection: connecting to %#a : %d", &info->Addr, mDNSVal16(info->Port));
	err = mDNSPlatformTCPConnect(info->sock, Addr, Port, 0, tcpConnectionCallback, info);

	if      (err == mStatus_ConnEstablished) { tcpConnectionCallback(info->sock, info, mDNStrue, mStatus_NoError); }
	else if (err != mStatus_ConnPending    ) { LogMsg("LNT MakeTCPConnection: connection failed"); mDNSPlatformMemFree(info); mDNSPlatformMemFree(info->reply); return(mDNSNULL); }
	return(info);
	}

mDNSlocal tcpLNTInfo *SendSOAPMsgControlAction(mDNS *m, char *Action, int numArgs, Property *Arguments)
	{
	char	*sendBuffer		= mDNSNULL;
	char	*sendBufferBody	= mDNSNULL;
	char	*sendBufferArgs		= mDNSNULL;
	char	*end				= mDNSNULL;
	int	totalLen		= 0;
	int	headerLen		= 0;
	int	bodyLen		= 0;
	int	argsLen		= 0;
	tcpLNTInfo *info	= mDNSNULL;

	if ((sendBuffer		= (char *) mDNSPlatformMemAllocate(MAX_SOAPMSGSIZE)) == mDNSNULL) { LogMsg("can't mDNSPlatformMemAllocate for sendBuffer"); return (mDNSNULL); }
	if ((sendBufferBody	= (char *) mDNSPlatformMemAllocate(MAX_SOAPMSGSIZE)) == mDNSNULL) { LogMsg("can't mDNSPlatformMemAllocate for sendBufferBody"); mDNSPlatformMemFree(sendBuffer); return (mDNSNULL); }
	if ((sendBufferArgs	= (char *) mDNSPlatformMemAllocate(MAX_SOAPMSGSIZE)) == mDNSNULL) { LogMsg("can't mDNSPlatformMemAllocate for sendBufferArgs"); mDNSPlatformMemFree(sendBuffer); mDNSPlatformMemFree(sendBuffer); mDNSPlatformMemFree(sendBufferBody); return (mDNSNULL); }

	// handle arguments if any
	if (Arguments != mDNSNULL) AddSOAPArguments(sendBufferArgs, numArgs, Arguments, &argsLen);
	sendBufferArgs[argsLen] = '\0';

	// create message body
	bodyLen = snprintf(sendBufferBody, MAX_SOAPMSGSIZE, szSOAPMsgControlABodyFMT, Action, sendBufferArgs);
	// create message header
	headerLen = snprintf(sendBuffer, MAX_SOAPMSGSIZE, szSOAPMsgControlAHeaderFMT, m->uPNPSOAPURL, m->uPNPSOAPAddressString, Action, bodyLen);

	strlcpy(sendBuffer + headerLen, sendBufferBody, MAX_SOAPMSGSIZE - headerLen);
	totalLen = headerLen + bodyLen;

	end = sendBuffer + totalLen;
	info = MakeTCPConnection(m, (mDNSu8 *)sendBuffer, (mDNSu8 *)end, &m->Router, m->uPNPSOAPPort);
	if (info == mDNSNULL) mDNSPlatformMemFree(sendBuffer);

	if (sendBufferBody 	!= mDNSNULL)	mDNSPlatformMemFree(sendBufferBody);
	if (sendBufferArgs 	!= mDNSNULL)	mDNSPlatformMemFree(sendBufferArgs);
	return (info);
	}

mDNSlocal tcpLNTInfo *GetDeviceDescription(mDNS *m)
	{
	char	*sendBuffer	= mDNSNULL;
	char	*end			= mDNSNULL;
	int	bufLen		= 0;
	tcpLNTInfo *info	= mDNSNULL;

	// build message
	if ((sendBuffer = (char *) mDNSPlatformMemAllocate(MAX_SOAPMSGSIZE)) == mDNSNULL) { LogMsg("can't malloc for sendBuffer"); return (mDNSNULL); }
	bufLen = snprintf(sendBuffer, MAX_SOAPMSGSIZE, szSSDPMsgDescribeDeviceFMT, m->uPNPRouterURL, m->uPNPRouterAddressString);

	LogOperation("Describe Device: [%s]", sendBuffer);
	end = sendBuffer + bufLen;
	info = MakeTCPConnection(m, (mDNSu8 *)sendBuffer, (mDNSu8 *)end, &m->Router, m->uPNPRouterPort);
	if (info == mDNSNULL) mDNSPlatformMemFree(sendBuffer);

	return (info);
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
	
	snprintf(externalPort, sizeof(externalPort), "%u", mDNSVal16(n->publicPort));
	
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

	if (n->tcpInfo 	!= mDNSNULL)	{ mDNSPlatformMemFree(n->tcpInfo); n->tcpInfo = mDNSNULL; }	// clear out old tcpInfo
	n->tcpInfo = SendSOAPMsgControlAction(m, "DeletePortMapping", 3, propArgs);
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

	// create strings to use in the message
	snprintf(externalPort, 	sizeof(externalPort), 		"%u", mDNSVal16(n->publicPortreq));
	snprintf(internalPort, 		sizeof(internalPort), 		"%u", mDNSVal16(n->privatePort));
	snprintf(publicPortString, 	sizeof(publicPortString), 	"iC%u", mDNSVal16(n->publicPortreq));
	snprintf(localIPAddrString, 	sizeof(localIPAddrString), 	"%u.%u.%u.%u",
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

	if (n->tcpInfo 	!= mDNSNULL)	{ mDNSPlatformMemFree(n->tcpInfo); n->tcpInfo = mDNSNULL; }	// clear out old tcpInfo
	n->tcpInfo = SendSOAPMsgControlAction(m, "AddPortMapping", 8, propArgs);
	return (error);
	}

mDNSexport mStatus LNT_GetExternalAddress(mDNS *m)
	{
	mStatus	error = mStatus_NoError;
	if (m->tcpAddrInfo 	!= mDNSNULL)	{ mDNSPlatformMemFree(m->tcpAddrInfo); m->tcpAddrInfo = mDNSNULL; }	// clear out old tcpInfo
	m->tcpAddrInfo = SendSOAPMsgControlAction(m, "GetExternalIPAddress", 0, mDNSNULL);
	if (!m->tcpAddrInfo) error = mStatus_NATTraversal;
	return (error);
	}

mDNSexport void LNT_ConfigureRouterInfo(mDNS *m, mDNSu8 *data, mDNSu16 len)
	{
	int		i;
	mDNSBool	isValidResponse	= mDNSfalse;
	mStatus	error			= mStatus_NoError;
	HTTPResponse			Response;

	Response.Status		= mDNSNULL;
	Response.Reason		= mDNSNULL;
	Response.numHeaders	= 0;
	Response.Body			= mDNSNULL;

	data[len] = '\0';		// XXX: why? I think the extract call needs the buffer to be null-terminated
	
	// Only looking for a response to our discovery message, not notifications
	if (strncmp((char *)data, "HTTP/1.1", 8) == 0) 
		{
		error = ExtractResponseHeaders(&Response, (char *)data, len, mDNStrue);
		if (error) { LogMsg("LNT_ConfigureRouterInfo: received invalid message from router"); goto end; }
		}
		
	// check to see if this is a relevant packet
	for (i = 0; i < Response.numHeaders; i++) 
		{
		Property *pHeader = &(Response.Headers[i]);

		LogOperation("Header %d propName: %s, propValue %s", i, pHeader->propName, pHeader->propValue);
		// check to see if this is a response to our discovery message
		if (((strcasecmp(pHeader->propName, "ST") == 0) || (strcasecmp(pHeader->propName, "NT") == 0)) && 
			((strcmp(pHeader->propValue, "urn:schemas-upnp-org:service:WANIPConnection:1") == 0) ||
			(strcmp(pHeader->propValue, "urn:schemas-upnp-org:device:InternetGatewayDevice:1") == 0)))
			{
			isValidResponse = TRUE;
			}
		}

	// leave the message alone if we don't need it
	if (!isValidResponse) { LogMsg("LNT_ConfigureRouterInfo: invalid response from router"); return; }

	// loop through the headers
	for (i = 0; i < Response.numHeaders; i++) 
		{
		Property *pHeader = &(Response.Headers[i]);

		if (strcasecmp(pHeader->propName, "Location") == 0) 
			{
			char *p;
			char *q;

			LogOperation("Checking Location...");
			p = pHeader->propValue;
			if (strncmp(p, "http://", 7) != 0) continue;	// hope for another Location header to correct it
			p += 7;  							// skip over "http://"
			q = strchr(p, '/');

			// This is the meat of it: here we find the URL and the port that we need to create a connection with the router
			// set the control URL first
			if (q == mDNSNULL) 
				{
				m->uPNPRouterURL[0] = '/';
				m->uPNPRouterURL[1] = '\0';
				}
			else 
				{
				// this URL looks something like "/dyndev/uuid:0013-108c-4b3f0000f3dc"
				strncpy((char *)m->uPNPRouterURL, q, LNT_BUFSIZE - 1);
				m->uPNPRouterURL[LNT_BUFSIZE - 1] = '\0';
				// terminate the host/port string
				*q = '\0';
				}

			LogOperation("Device Description URL set to[%s]...\n", m->uPNPRouterURL);

			// see if port is specified
			q = strchr(p, ':');
			if (q == mDNSNULL) 
				{
				// default port
				snprintf((char *)m->uPNPRouterAddressString, LNT_BUFSIZE, "%s", p);
				m->uPNPRouterPort = mDNSOpaque16fromIntVal(80);
				}
			else 
				{
				// don't include the ":80" - HTTP is by default port 80
				if (atoi(q+1) == 80) *q = '\0';
				strlcpy((char *)m->uPNPRouterAddressString, p, LNT_BUFSIZE);

				// terminate the host part and point to it
				*q = '\0';
				q++;
				m->uPNPRouterPort = mDNSOpaque16fromIntVal(atoi(q));
				}

			LogOperation("Router Address set to[%s]...\n", m->uPNPRouterAddressString);
			}
		}
end:
	if (m->tcpDeviceInfo 	!= mDNSNULL)	{ mDNSPlatformMemFree(m->tcpDeviceInfo); m->tcpDeviceInfo = mDNSNULL; }	// clear out old tcpInfo
	m->tcpDeviceInfo = GetDeviceDescription(m);
	}

mDNSexport mStatus LNT_SendDiscoveryMsg(mDNS *m)
	{
	mStatus		err 	  = mStatus_NoError;
	const mDNSu8	*msg = mDNSNULL;
	const mDNSu8	*end = mDNSNULL;

	// allocate everything we're going to need now
	if (m->uPNPRouterURL == mDNSNULL) 	   if ((m->uPNPRouterURL = (mDNSu8 *) mDNSPlatformMemAllocate(LNT_BUFSIZE)) == mDNSNULL) { LogMsg("can't mDNSPlatformMemAllocate for uPNPRouterURL"); return (mStatus_NoMemoryErr); }
	if (m->uPNPSOAPURL 	== mDNSNULL) 		   if ((m->uPNPSOAPURL = (mDNSu8 *) mDNSPlatformMemAllocate(LNT_BUFSIZE)) == mDNSNULL) { LogMsg("can't mDNSPlatformMemAllocate for uPNPSOAPURL"); return (mStatus_NoMemoryErr); }
	if (m->uPNPRouterAddressString == mDNSNULL) if ((m->uPNPRouterAddressString = (mDNSu8 *) mDNSPlatformMemAllocate(LNT_BUFSIZE)) == mDNSNULL) { LogMsg("can't mDNSPlatformMemAllocate for uPNPRouterAddressString"); return (mStatus_NoMemoryErr); }
	if (m->uPNPSOAPAddressString == mDNSNULL)   if ((m->uPNPSOAPAddressString = (mDNSu8 *) mDNSPlatformMemAllocate(LNT_BUFSIZE)) == mDNSNULL) { LogMsg("can't mDNSPlatformMemAllocate for uPNPSOAPAddressString"); return (mStatus_NoMemoryErr); }

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