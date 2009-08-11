/* -*- Mode: C; tab-width: 4 -*-
 *
 * Copyright (c) 1997-2004 Apple Computer, Inc. All rights reserved.
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
 */

#pragma once


#include "firstpage.h"
#include "secondpage.h"
#include "thirdpage.h"
#include "fourthpage.h"
#include "UtilTypes.h"
#include "Logger.h"
#include "dns_sd.h"
#include <stdexcept>
#include <map>

using namespace PrinterSetupWizard;

// CPrinterSetupWizardSheet

class CPrinterSetupWizardSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CPrinterSetupWizardSheet)

public:

	struct WizardException
	{
		CString	text;
		CString	caption;
	};

public:

	CPrinterSetupWizardSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CPrinterSetupWizardSheet();

	CPropertyPage*
	GetLastPage();

	void
	SetLastPage(CPropertyPage * page );

	void
	SetSelectedPrinter(Printer * printer);

	Printer*
	GetSelectedPrinter();

	OSStatus
	LoadPrinterDriver(const CString & filename);

	HCURSOR
	GetCursor();

	//
	// handles end of process event
	//
	virtual LRESULT
	OnProcessEvent(WPARAM inWParam, LPARAM inLParam);
	
	virtual LRESULT
	OnSocketEvent(WPARAM inWParam, LPARAM inLParam);

	virtual BOOL
	OnCommand(WPARAM wParam, LPARAM lParam);

	virtual BOOL
	OnInitDialog();

	virtual BOOL
	OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message);

	virtual void
	OnContextMenu(CWnd * pWnd, CPoint pos);

	afx_msg void
	OnOK();

	OSStatus
	StartResolve( Printer * printer );

	OSStatus
	StopResolve( Printer * printer );

	Printers				m_printers;

	HCURSOR					m_active;
	HCURSOR					m_arrow;
	HCURSOR					m_wait;

protected:
	DECLARE_MESSAGE_MAP()
	CFirstPage		m_pgFirst;
	CSecondPage		m_pgSecond;
	CThirdPage		m_pgThird;
	CFourthPage		m_pgFourth;

	void
	OnServiceResolved(
		Service				*	service);

	void Init(void);

private:

	static void DNSSD_API
	OnBrowse(
		DNSServiceRef 			inRef,
		DNSServiceFlags 		inFlags,
		uint32_t 				inInterfaceIndex,
		DNSServiceErrorType 	inErrorCode,
		const char *			inName,	
		const char *			inType,	
		const char *			inDomain,	
		void *					inContext );

	static void DNSSD_API
	OnResolve(
		DNSServiceRef			inRef,
		DNSServiceFlags			inFlags,
		uint32_t				inInterfaceIndex,
		DNSServiceErrorType		inErrorCode,
		const char *			inFullName,	
		const char *			inHostName, 
		uint16_t 				inPort,
		uint16_t 				inTXTSize,
		const char *			inTXT,
		void *					inContext );

	static void DNSSD_API
	OnQuery(
		DNSServiceRef			inRef, 
		DNSServiceFlags			inFlags, 
		uint32_t				inInterfaceIndex, 
		DNSServiceErrorType		inErrorCode,
		const char			*	inFullName, 
		uint16_t				inRRType, 
		uint16_t				inRRClass, 
		uint16_t				inRDLen, 
		const void			*	inRData, 
		uint32_t				inTTL, 
		void				*	inContext);

	Printer*
	OnAddPrinter(
		uint32_t 				inInterfaceIndex,
		const char			*	inName,	
		const char			*	inType,	
		const char			*	inDomain,
		bool					moreComing);

	OSStatus
	OnRemovePrinter(
		Printer				*	printer,
		bool					moreComing);

	OSStatus
	OnAddService(
		Printer				*	printer,
		uint32_t 				inInterfaceIndex,
		const char			*	inName,	
		const char			*	inType,	
		const char			*	inDomain);

	OSStatus
	OnRemoveService(
		Service				*	service);

	void
	OnResolveService(
		Service				*	service );

	static bool
	OrderServiceFunc( const Service * a, const Service * b );

	static bool
	OrderQueueFunc( const Queue * q1, const Queue * q2 );

	OSStatus
	StartOperation( DNSServiceRef ref );

	OSStatus
	StopOperation( DNSServiceRef & ref );

	OSStatus
	StartBrowse();

	OSStatus
	StopBrowse();

	OSStatus
	StartResolve( Service * service );

	OSStatus
	StopResolve( Service * service );

	OSStatus
	ParseTextRecord( Service * service, Queue * q, uint16_t inTXTSize, const char * inTXT );

	OSStatus
	LoadPrinterNames();

	Printer*
	Lookup( const char * name );

	OSStatus
	InstallPrinter(Printer * printer);

	OSStatus
	InstallPrinterPDLAndLPR(Printer * printer, Service * service, DWORD protocol, Logger & log);

	OSStatus
	InstallPrinterIPP(Printer * printer, Service * service, Logger & log);

	static unsigned WINAPI
	InstallDriverThread( LPVOID inParam );

	typedef std::list<CString>			PrinterNames;
	typedef std::list<DNSServiceRef>	ServiceRefList;
	static CPrinterSetupWizardSheet	*	m_self;
	PrinterNames						m_printerNames;
	Printer							*	m_selectedPrinter;
	bool								m_driverThreadFinished;
	DWORD								m_driverThreadExitCode;
	ServiceRefList						m_serviceRefList;
	DNSServiceRef						m_pdlBrowser;
	DNSServiceRef						m_lprBrowser;
	DNSServiceRef						m_ippBrowser;
	DNSServiceRef						m_resolver;

	CPropertyPage					*	m_lastPage;
};


inline Printer*
CPrinterSetupWizardSheet::GetSelectedPrinter()
{	
	return m_selectedPrinter;
}


inline HCURSOR
CPrinterSetupWizardSheet::GetCursor()
{
	return m_active;
}


inline CPropertyPage*
CPrinterSetupWizardSheet::GetLastPage()
{
	return m_lastPage;
}


inline void
CPrinterSetupWizardSheet::SetLastPage(CPropertyPage * lastPage)
{
	m_lastPage = lastPage;
}


// Service Types

#define	kPDLServiceType		"_pdl-datastream._tcp."
#define kLPRServiceType		"_printer._tcp."
#define kIPPServiceType		"_ipp._tcp."
