/*
 * Copyright (c) 1997-2004 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@

    Change History (most recent first):
    
$Log: PrinterSetupWizardSheet.cpp,v $
Revision 1.3  2004/06/24 20:12:07  shersche
Remove reference to Rendezvous in source code
Submitted by: herscher

Revision 1.2  2004/06/23 17:58:21  shersche
<rdar://problem/3701837> eliminated memory leaks on exit
<rdar://problem/3701926> installation of a printer that is already installed results in a no-op
Bug #: 3701837, 3701926
Submitted by: herscher

Revision 1.1  2004/06/18 04:36:57  rpantos
First checked in


*/

#include "stdafx.h"
#include "PrinterSetupWizardApp.h"
#include "PrinterSetupWizardSheet.h"
#include "CommonServices.h"
#include "DebugServices.h"
#include "WinServices.h"
#include "About.h"
#include <winspool.h>
#include <tcpxcv.h>
#include <string>

#if( !TARGET_OS_WINDOWS_CE )
#	include	<mswsock.h>
#	include	<process.h>
#endif

// Private Messages

#define WM_SERVICE_EVENT				( WM_USER + 0x100 )
#define WM_PROCESS_EVENT				( WM_USER + 0x101 )

// Service Types

#define	kPDLDataStreamServiceType		"_pdl-datastream._tcp"
#define kLPRServiceType					"_printer._tcp"
#define kIPPServiceType					"_ipp._tcp"


// CPrinterSetupWizardSheet

IMPLEMENT_DYNAMIC(CPrinterSetupWizardSheet, CPropertySheet)
CPrinterSetupWizardSheet::CPrinterSetupWizardSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage),
	m_selectedPrinter(NULL)
{
	m_arrow		=	LoadCursor(0, IDC_ARROW);
	m_wait		=	LoadCursor(0, IDC_APPSTARTING);
	m_active	=	m_arrow;
	
	Init();
}


CPrinterSetupWizardSheet::~CPrinterSetupWizardSheet()
{
	//
	// rdar://problem/3701837 memory leaks
	//
	// Clean up the ServiceRef and printer list on exit
	//
	if (m_pdlBrowser != NULL)
	{
		DNSServiceRefDeallocate(m_pdlBrowser);
	}

	while (m_printerList.size() > 0)
	{
		Printer * printer = m_printerList.front();

		m_printerList.pop_front();

		delete printer;
	}
}


// ------------------------------------------------------
// InstallEventHandler
//
// Installs an event handler for DNSService events.  
//	
int
CPrinterSetupWizardSheet::InstallEventHandler(EventHandler * handler)
{
	PrinterList::iterator iter;
	
	m_eventHandlerList.push_back(handler);

	for ( iter = m_printerList.begin(); iter != m_printerList.end(); iter++ )
	{
		Printer * printer = *iter;

		handler->OnAddPrinter(printer);
	}
	
	return kNoErr;
}


// ------------------------------------------------------
// RemoveEventHandler
//
// Removes an event handler for DNSService events.  
//	
int
CPrinterSetupWizardSheet::RemoveEventHandler(EventHandler * handler)
{
	m_eventHandlerList.remove(handler);

	return kNoErr;
}


// ------------------------------------------------------
// SetSelectedPrinter
//
// Manages setting a printer as the printer to install.  Stops
// any pending resolves.  
//	
OSStatus
CPrinterSetupWizardSheet::SetSelectedPrinter(Printer * printer)
{
	OSStatus err;

	//
	// we only want one resolve going on at a time, so we check
	// state of the m_selectedPrinter
	//
	if (m_selectedPrinter != NULL)
	{
		//
		// if we're currently resolving, then stop the resolve
		//
		if (m_selectedPrinter->serviceRef)
		{
			err = StopResolve(m_selectedPrinter);
			require_noerr(err, exit);
		}

		m_selectedPrinter = NULL;
	}

	check( m_selectedPrinter == NULL );

	err = StartResolve(printer);
	require_noerr(err, exit);

	m_selectedPrinter = printer;

exit:

	return err;
}


// ------------------------------------------------------
// InstallPrinter
//
// Installs a printer with Windows.
//
// NOTE: this works one of two ways, depending on whether
// there are drivers already installed for this printer.
// If there are, then we can just create a port with XcvData,
// and then call AddPrinter.  If not, we use the printui.dll
// to install the printer. Actually installing drivers that
// are not currently installed is painful, and it's much
// easier and less error prone to just let printui.dll do
// the hard work for us.
//	

OSStatus
CPrinterSetupWizardSheet::InstallPrinter(Printer * printer)
{
	PRINTER_DEFAULTS	printerDefaults =	{ NULL,  NULL, SERVER_ACCESS_ADMINISTER };
	DWORD				dwStatus;
	DWORD				cbInputData		=	100;
	PBYTE				pOutputData		=	NULL;
	PBYTE				buffer			=	NULL;
	DWORD				cbOutputNeeded	=	0;
	PORT_DATA_1			portData;
	HANDLE				hXcv			=	NULL;
	HANDLE				hPrinter		=	NULL;
	CString				printerName;
	BOOL				ok;
	OSStatus			err;

	check(printer != NULL);
	check(printer->installed == false);

	err = UTF8StringToStringObject(printer->name.c_str(), printerName);
	require_noerr(err, exit);

	//
	// rdar://problem/3701926 - Printer can't be installed twice
	//
	// First thing we want to do is make sure the printer isn't already installed.
	// If the printer name is found, we'll jump down out of the meat of the function
	// which effectively results in the equivalent of a no-op when the user clicks
	// the "Finish" button
	//
	DWORD dwNeeded, dwNumPrinters;
	
	ok = EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 4, NULL, 0, &dwNeeded, &dwNumPrinters);
	err = translate_errno( ok, errno_compat(), kUnknownErr );
	require_action( err == ERROR_INSUFFICIENT_BUFFER, exit, kUnknownErr);
	
	try
	{
		buffer = new unsigned char[dwNeeded];
	}
	catch (...)
	{
	}

	require_action( buffer, exit, kNoMemoryErr );
    ok = EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 4, buffer, dwNeeded, &dwNeeded, &dwNumPrinters);
	err = translate_errno( ok, errno_compat(), kUnknownErr );
	require_noerr( err, exit );

    for (DWORD index = 0; index < dwNumPrinters; index++)
    {
		PRINTER_INFO_4 * lppi4 = (PRINTER_INFO_4*) (buffer + index * sizeof(PRINTER_INFO_4));
    
		if (printerName == lppi4->pPrinterName)
		{
			printer->installed = true;
			break;
		}
	}

	if (!printer->installed)
	{
		ok = OpenPrinter(L",XcvMonitor Standard TCP/IP Port", &hXcv, &printerDefaults);
		err = translate_errno( ok, errno_compat(), kUnknownErr );
		require_noerr( err, exit );
	
		//
		// BUGBUG: MSDN said this is not required, but my experience shows it is required
		//
		try
		{
			pOutputData = new BYTE[cbInputData];
		}
		catch (...)
		{
		}

		require_action( pOutputData, exit, err = kNoMemoryErr );
	
		//
		// setup the port
		//
		ZeroMemory(&portData, sizeof(PORT_DATA_1));
		wcscpy(portData.sztPortName, printer->portName);
    	
		portData.dwPortNumber	=	printer->portNumber;
		portData.dwVersion		=	1;
    	
		portData.dwProtocol	= PROTOCOL_RAWTCP_TYPE;
		portData.cbSize		= sizeof PORT_DATA_1;
		portData.dwReserved	= 0L;
    	
		wcscpy(portData.sztQueue, printer->hostname);
		wcscpy(portData.sztIPAddress, printer->hostname); 
		wcscpy(portData.sztHostAddress, printer->hostname);
		
		ok = XcvData(hXcv, L"AddPort", (PBYTE) &portData, sizeof(PORT_DATA_1), pOutputData, cbInputData,  &cbOutputNeeded, &dwStatus);
		err = translate_errno( ok, errno_compat(), kUnknownErr );
		require_noerr( err, exit );
	
		if (printer->driverInstalled)
		{
			PRINTER_INFO_2 pInfo;
	
			ZeroMemory(&pInfo, sizeof(pInfo));
			
			pInfo.pPrinterName			=	printer->displayName.GetBuffer();
			pInfo.pServerName			=	NULL;
			pInfo.pShareName			=	NULL;
			pInfo.pPortName				=	printer->portName.GetBuffer();
			pInfo.pDriverName			=	printer->model.GetBuffer();
			pInfo.pComment				=	printer->model.GetBuffer();
			pInfo.pLocation				=	L"";
			pInfo.pDevMode				=	NULL;
			pInfo.pDevMode				=	NULL;
			pInfo.pSepFile				=	L"";
			pInfo.pPrintProcessor		=	L"winprint";
			pInfo.pDatatype				=	L"RAW";
			pInfo.pParameters			=	L"";
			pInfo.pSecurityDescriptor	=	NULL;
			pInfo.Attributes			=	PRINTER_ATTRIBUTE_QUEUED;
			pInfo.Priority				=	0;
			pInfo.DefaultPriority		=	0;
			pInfo.StartTime				=	0;
			pInfo.UntilTime				=	0;
	
			hPrinter = AddPrinter(NULL, 2, (LPBYTE) &pInfo);
			err = translate_errno( hPrinter, errno_compat(), kUnknownErr );
			require_noerr( err, exit );
		}
		else
		{
			DWORD		dwResult;
			HANDLE		hThread;
			unsigned	threadID;
			
	
			m_processFinished = false;
	
			//
			// create the thread
			//
			hThread = (HANDLE) _beginthreadex_compat( NULL, 0, InstallPrinterThread, printer, 0, &threadID );
			err = translate_errno( hThread, (OSStatus) GetLastError(), kUnknownErr );
			require_noerr( err, exit );
			
			//
			// go modal
			//
			while (!m_processFinished)
			{
				MSG msg;
	
				GetMessage( &msg, m_hWnd, 0, 0 );
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
	
			//
			// Wait until child process exits.
			//
			dwResult = WaitForSingleObject( hThread, INFINITE );
			err = translate_errno( dwResult == WAIT_OBJECT_0, errno_compat(), err = kUnknownErr );
			require_noerr( err, exit );
		}

		printer->installed = true;
	}

	//
	// if the user specified a default printer, set it
	//
	if (printer->deflt)
	{
		ok = SetDefaultPrinter(printerName);
		err = translate_errno( ok, errno_compat(), err = kUnknownErr );
		require_noerr( err, exit );
	}

exit:

	if (hPrinter != NULL)
	{
		ClosePrinter(hPrinter);
	}

	if (hXcv != NULL)
	{
		ClosePrinter(hXcv);
	}

	if (pOutputData != NULL)
	{
		delete [] pOutputData;
	}

	if (buffer != NULL)
	{
		delete [] buffer;
	}

	return err;
}


BEGIN_MESSAGE_MAP(CPrinterSetupWizardSheet, CPropertySheet)
ON_MESSAGE( WM_SERVICE_EVENT, OnServiceEvent )
ON_MESSAGE( WM_PROCESS_EVENT, OnProcessEvent )
ON_WM_SETCURSOR()
END_MESSAGE_MAP()


// ------------------------------------------------------
// OnCommand
//
// Traps when the user hits Finish  
//	
BOOL CPrinterSetupWizardSheet::OnCommand(WPARAM wParam, LPARAM lParam)
{
	//
	// Check if this is OK
	//
	if (wParam == ID_WIZFINISH)              // If OK is hit...
	{
		OnOK();
	}
 
	return CPropertySheet::OnCommand(wParam, lParam);
}


// ------------------------------------------------------
// OnInitDialog
//
// Initializes this Dialog object.  We start the browse here,
// so that printers show up instantly when the user clicks
// the next button.  
//	
BOOL CPrinterSetupWizardSheet::OnInitDialog()
{
	OSStatus err;

	CPropertySheet::OnInitDialog();
	
	//
	// setup the rendezvous browsing
	//
	err = DNSServiceBrowse( &m_pdlBrowser, 0, 0, kPDLDataStreamServiceType, NULL, OnBrowse, this );
	require_noerr( err, exit );
	err = WSAAsyncSelect((SOCKET) DNSServiceRefSockFD(m_pdlBrowser), m_hWnd, WM_SERVICE_EVENT, FD_READ|FD_CLOSE);
	require_noerr( err, exit );
	m_serviceRefList.push_back(m_pdlBrowser);

exit:

	if (err != kNoErr)
	{
		WizardException exc;

		exc.text.LoadString(IDS_NO_RENDEZVOUS_SERVICE_TEXT);
		exc.caption.LoadString(IDS_NO_RENDEZVOUS_SERVICE_CAPTION);

		throw(exc);
	}

	return TRUE;
}


// ------------------------------------------------------
// OnSetCursor
//
// This is called when Windows wants to know what cursor
// to display.  So we tell it.  
//	
BOOL
CPrinterSetupWizardSheet::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message)
{
	SetCursor(m_active);
	return TRUE;
}


// ------------------------------------------------------
// OnContextMenu
//
// This is not fully implemented yet.  
//	

void
CPrinterSetupWizardSheet::OnContextMenu(CWnd * pWnd, CPoint pos)
{
	CAbout dlg;

	dlg.DoModal();
}


// ------------------------------------------------------
// OnOK
//
// This is called when the user hits the "Finish" button  
//	
void
CPrinterSetupWizardSheet::OnOK()
{
	check ( m_selectedPrinter != NULL );

	SetWizardButtons( PSWIZB_DISABLEDFINISH );

	if ( InstallPrinter( m_selectedPrinter ) != kNoErr )
	{
		CString caption;
		CString message;

		caption.LoadString(IDS_INSTALL_ERROR_CAPTION);
		message.LoadString(IDS_INSTALL_ERROR_MESSAGE);

		MessageBox(message, caption, MB_OK|MB_ICONEXCLAMATION);
	}
}


// CPrinterSetupWizardSheet message handlers

void CPrinterSetupWizardSheet::Init(void)
{
	AddPage(&m_pgFirst);
	AddPage(&m_pgSecond);
	AddPage(&m_pgThird);
	AddPage(&m_pgFourth);

	m_psh.dwFlags &= (~PSH_HASHELP);

	m_psh.dwFlags |= PSH_WIZARD97|PSH_WATERMARK|PSH_HEADER;
	m_psh.pszbmWatermark = MAKEINTRESOURCE(IDB_WATERMARK);
	m_psh.pszbmHeader = MAKEINTRESOURCE(IDB_BANNER_ICON);

	m_psh.hInstance = AfxGetInstanceHandle();

	SetWizardMode();
}


LONG
CPrinterSetupWizardSheet::OnServiceEvent(WPARAM inWParam, LPARAM inLParam)
{
	if (WSAGETSELECTERROR(inLParam) && !(HIWORD(inLParam)))
    {
		dlog( kDebugLevelError, "OnServiceEvent: window error\n" );
    }
    else
    {
		SOCKET sock = (SOCKET) inWParam;

		// iterate thru list
		ServiceRefList::iterator begin = m_serviceRefList.begin();
		ServiceRefList::iterator end   = m_serviceRefList.end();

		while (begin != end)
		{
			DNSServiceRef ref = *begin++;

			check(ref != NULL);

			if ((SOCKET) DNSServiceRefSockFD(ref) == sock)
			{
				DNSServiceProcessResult(ref);
			}
		}
	}

	return ( 0 );
}


LONG
CPrinterSetupWizardSheet::OnProcessEvent(WPARAM inWParam, LPARAM inLParam)
{
	m_processFinished = true;

	return 0;
}


void DNSSD_API
CPrinterSetupWizardSheet::OnBrowse(
								DNSServiceRef 			inRef,
								DNSServiceFlags 		inFlags,
								uint32_t 				inInterfaceIndex,
								DNSServiceErrorType 	inErrorCode,
								const char *			inName,	
								const char *			inType,	
								const char *			inDomain,	
								void *					inContext )
{
	CPrinterSetupWizardSheet	*	self;
	Printer						*	printer;
	EventHandlerList::iterator		it;

	require_noerr( inErrorCode, exit );
	
	self = reinterpret_cast <CPrinterSetupWizardSheet*>( inContext );
	require_quiet( self, exit );

	printer = self->LookUp(inName);

	if (inFlags & kDNSServiceFlagsAdd)
	{
		OSStatus err;

		if (printer != NULL)
		{
			printer->refs++;
		}
		else
		{
			try
			{
				printer = new Printer;
			}
			catch (...)
			{
			}

			require_action( printer, exit, err = E_OUTOFMEMORY );

			printer->window		=	self;
			printer->ifi		=	inInterfaceIndex;
			printer->name		=	inName;
			err = UTF8StringToStringObject(inName, printer->displayName);
			check_noerr( err );
			printer->type		=	inType;
			printer->domain		=	inDomain;
			printer->installed	=	false;
			printer->deflt		=	false;
			printer->refs		=	1;
			
			self->m_printerList.push_back( printer );

			//
			// now invoke event handlers for AddPrinter event
			//
			for (it = self->m_eventHandlerList.begin(); it != self->m_eventHandlerList.end(); it++)
			{
				EventHandler * handler = *it;

				handler->OnAddPrinter(printer);
			}
		}
	}
	else if (!inFlags || (inFlags == kDNSServiceFlagsMoreComing))
	{
		if ((printer != NULL) && (--printer->refs == 0))
		{
			//
			// now invoke event handlers for RemovePrinter event
			//
			for (it = self->m_eventHandlerList.begin(); it != self->m_eventHandlerList.end(); it++)
			{
				EventHandler * handler = *it;

				handler->OnRemovePrinter(printer);
			}

			self->m_printerList.remove(printer);

			//
			// check to see if we've selected this printer
			//
			if (self->m_selectedPrinter == printer)
			{
				//
				// this guy is being removed while we're resolving it...so let's 
				// stop the resolve
				//
				if (printer->serviceRef != NULL)
				{
					self->StopResolve(printer);
				}

				self->m_selectedPrinter = NULL;
			}

			delete printer;
		}
	}

exit:

	return;
}


void DNSSD_API
CPrinterSetupWizardSheet::OnResolve(
								DNSServiceRef			inRef,
								DNSServiceFlags			inFlags,
								uint32_t				inInterfaceIndex,
								DNSServiceErrorType		inErrorCode,
								const char *			inFullName,	
								const char *			inHostName, 
								uint16_t 				inPort,
								uint16_t 				inTXTSize,
								const char *			inTXT,
								void *					inContext )
{
	Printer						*	printer;
	CPrinterSetupWizardSheet	*	self;
	EventHandlerList::iterator		it1;
	EventHandlerList::iterator		it2;
	OSStatus						err;

	require_noerr( inErrorCode, exit );

	printer = reinterpret_cast<Printer*>( inContext );
	require_quiet( printer, exit);

	self = printer->window;
	require_quiet( self, exit );

	err = self->StopResolve(printer);
	require_noerr(err, exit);
	
	//
	// hold on to the hostname
	//
	err = UTF8StringToStringObject( inHostName, printer->hostname );
	require_noerr( err, exit );

	//
	// hold on to the port
	//
	printer->portNumber = ntohs(inPort);

	//
	// parse the text record.  we create a stringlist of text record
	// entries that can be interrogated later
	//
	while (inTXTSize)
	{
		char buf[256];

		unsigned char num = *inTXT;
		check( (int) num < inTXTSize );

		memset(buf, 0, sizeof(buf));
		memcpy(buf, inTXT + 1, num);
		
		inTXTSize -= (num + 1);
		inTXT += (num + 1);

		CString elem;

		err = UTF8StringToStringObject( buf, elem );
		require_noerr( err, exit );

		int curPos = 0;

		CString key = elem.Tokenize(L"=", curPos);
		CString val = elem.Tokenize(L"=", curPos);

		if ((key == L"usb_MFG") || (key == L"usb_MANUFACTURER"))
		{
			printer->usb_MFG = val;
		}
		else if ((key == L"usb_MDL") || (key == L"usb_MODEL"))
		{
			printer->usb_MDL = val;
		}
		else if (key == L"description")
		{
			printer->description = val;
		}
		else if (key == L"product")
		{
			printer->product = val;
		}
	}

	//
	// now invoke event handlers for Resolve event
	//
	it1 = self->m_eventHandlerList.begin();
	it2 = self->m_eventHandlerList.end();

	while (it1 != it2)
	{
		EventHandler * handler = *it1++;

		handler->OnResolvePrinter(printer);
	}

exit:

	return;
}


OSStatus
CPrinterSetupWizardSheet::StartResolve(Printer * printer)
{
	OSStatus err;

	check( printer );

	err = DNSServiceResolve( &printer->serviceRef, 0, 0, printer->name.c_str(), printer->type.c_str(), printer->domain.c_str(), (DNSServiceResolveReply) OnResolve, printer );
	require_noerr( err, exit);
	err = WSAAsyncSelect((SOCKET) DNSServiceRefSockFD(printer->serviceRef), m_hWnd, WM_SERVICE_EVENT, FD_READ|FD_CLOSE);
	require_noerr( err, exit );

	m_serviceRefList.push_back(printer->serviceRef);

	//
	// set the cursor to arrow+hourglass
	//
	m_active = m_wait;
	SetCursor(m_active);

exit:

	return err;
}


OSStatus
CPrinterSetupWizardSheet::StopResolve(Printer * printer)
{
	OSStatus err;

	check( printer );
	check( printer->serviceRef );

	m_serviceRefList.remove( printer->serviceRef );

	err = WSAAsyncSelect((SOCKET) DNSServiceRefSockFD(printer->serviceRef), m_hWnd, WM_SERVICE_EVENT, 0);
	check(err == 0);	

	DNSServiceRefDeallocate( printer->serviceRef );

	printer->serviceRef = NULL;
	
	//
	// set the cursor back to normal
	//
	m_active = m_arrow;
	SetCursor(m_active);

	return kNoErr;
}


Printer*
CPrinterSetupWizardSheet::LookUp(const char * inName)
{
	PrinterList::iterator it1 = m_printerList.begin();
	PrinterList::iterator it2 = m_printerList.end();

	while (it1 != it2)
	{
		Printer * printer = *it1++;

		if (printer->name == inName)
		{
			return printer;
		}
	}

	return NULL;
}


unsigned WINAPI
CPrinterSetupWizardSheet::InstallPrinterThread( LPVOID inParam )
{
	check( inParam );
		
	Printer			*	printer = (Printer*) inParam;
	CString				command;
	DWORD				exitCode;
	DWORD				dwResult;
	OSStatus			err;
	STARTUPINFO			si;
	PROCESS_INFORMATION pi;
	BOOL				ok;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	command.Format(L"rundll32.exe printui.dll,PrintUIEntry /if /b \"%s\" /f%s /r \"%s\" /m \"%s\"", printer->displayName, printer->infFileName, printer->portName, printer->model);

	ok = CreateProcess(NULL, command.GetBuffer(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	err = translate_errno( ok, errno_compat(), kUnknownErr );
	require_noerr( err, exit );

	dwResult = WaitForSingleObject( pi.hProcess, INFINITE );
	translate_errno( dwResult == WAIT_OBJECT_0, errno_compat(), err = kUnknownErr );
	require_noerr( err, exit );

	ok = GetExitCodeProcess( pi.hProcess, &exitCode );
	err = translate_errno( ok, errno_compat(), kUnknownErr );
	require_noerr( err, exit );

	//
	// Close process and thread handles. 
	//
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );

exit:

	//
	// alert the main thread
	//
	printer->window->PostMessage( WM_PROCESS_EVENT, err, exitCode );

	return 0;
}
