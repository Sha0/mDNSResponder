/*
 * Copyright (c) 2002-2004 Apple Computer, Inc. All rights reserved.
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

*/

#include "FirstPage.h"
#include "resource.h"

#include "ConfigPropertySheet.h"
#include "SharedSecret.h"

#define MAX_KEY_LENGTH 255


IMPLEMENT_DYNCREATE(CFirstPage, CPropertyPage)

//---------------------------------------------------------------------------------------------------------------------------
//	CFirstPage::CFirstPage
//---------------------------------------------------------------------------------------------------------------------------

CFirstPage::CFirstPage()
:
	CPropertyPage(CFirstPage::IDD),
	m_statusKey( NULL )
{
	//{{AFX_DATA_INIT(CFirstPage)
	//}}AFX_DATA_INIT

	OSStatus err;

	err = RegCreateKey( HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Services\\" kServiceName L"\\Parameters\\DynDNS\\State\\Hostnames", &m_statusKey );
	check_noerr( err );

	err = RegCreateKey( HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Services\\" kServiceName L"\\Parameters\\DynDNS\\Setup\\Hostnames", &m_setupKey );
	check_noerr( err );
}

CFirstPage::~CFirstPage()
{
	if ( m_statusKey )
	{
		RegCloseKey( m_statusKey );
		m_statusKey = NULL;
	}

	if ( m_setupKey )
	{
		RegCloseKey( m_setupKey );
		m_setupKey = NULL;
	}
}


//---------------------------------------------------------------------------------------------------------------------------
//	CFirstPage::DoDataExchange
//---------------------------------------------------------------------------------------------------------------------------

void CFirstPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFirstPage)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_EDIT1, m_hostnameControl);
	DDX_Control(pDX, IDC_FAILURE, m_failureIcon);
	DDX_Control(pDX, IDC_SUCCESS, m_successIcon);
}

BEGIN_MESSAGE_MAP(CFirstPage, CPropertyPage)
	//{{AFX_MSG_MAP(CFirstPage)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedSharedSecret)
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeHostname)
END_MESSAGE_MAP()


//---------------------------------------------------------------------------------------------------------------------------
//	CFirstPage::OnEnChangedHostname
//---------------------------------------------------------------------------------------------------------------------------

void CFirstPage::OnEnChangeHostname()
{
	SetModified( TRUE );
}


//---------------------------------------------------------------------------------------------------------------------------
//	CFirstPage::OnBnClickedSharedSecret
//---------------------------------------------------------------------------------------------------------------------------

void CFirstPage::OnBnClickedSharedSecret()
{
	CString name;

	m_hostnameControl.GetWindowText( name );

	CSharedSecret dlg;

	dlg.m_secretName = name;

	if ( dlg.DoModal() == IDOK )
	{
		dlg.Commit();
	}
}


//---------------------------------------------------------------------------------------------------------------------------
//	CFirstPage::SetModified
//---------------------------------------------------------------------------------------------------------------------------

void CFirstPage::SetModified( BOOL bChanged )
{
	m_modified = bChanged ? true : false;

	CPropertyPage::SetModified( bChanged );
}


//---------------------------------------------------------------------------------------------------------------------------
//	CFirstPage::OnSetActive
//---------------------------------------------------------------------------------------------------------------------------

BOOL
CFirstPage::OnSetActive()
{
	TCHAR	name[kDNSServiceMaxDomainName + 1];
	DWORD	nameLen = ( kDNSServiceMaxDomainName + 1 ) * sizeof( TCHAR );
	DWORD	err;

	BOOL b = CPropertyPage::OnSetActive();

	m_modified = FALSE;

	if ( m_setupKey )
	{
		err = RegQueryValueEx( m_setupKey, L"", NULL, NULL, (LPBYTE) name, &nameLen );

		if ( !err )
		{
			m_hostnameControl.SetWindowText( name );
		}
	}

	// Check the status of this hostname

	err = CheckStatus();
	check_noerr( err );

	return b;
}


//---------------------------------------------------------------------------------------------------------------------------
//	CFirstPage::OnOK
//---------------------------------------------------------------------------------------------------------------------------

void
CFirstPage::OnOK()
{
	if ( m_modified )
	{
		Commit();
	}
}


//---------------------------------------------------------------------------------------------------------------------------
//	CFirstPage::Commit
//---------------------------------------------------------------------------------------------------------------------------

void
CFirstPage::Commit()
{
	DWORD	enabled = 1;
	CString	name;
	DWORD	err;

	m_hostnameControl.GetWindowText( name );

	// Convert to lower case

	name.MakeLower();

	// Remove trailing dot

	name.TrimRight( '.' );

	err = RegSetValueEx( m_setupKey, L"", 0, REG_SZ, (LPBYTE) (LPCTSTR) name, ( name.GetLength() + 1 ) * sizeof( TCHAR ) );
	require_noerr( err, exit );
	
	err = RegSetValueEx( m_setupKey, L"Enabled", 0, REG_DWORD, (LPBYTE) &enabled, sizeof( DWORD ) );
	require_noerr( err, exit );

exit:

	return;
}


//---------------------------------------------------------------------------------------------------------------------------
//	CFirstPage::CheckStatus
//---------------------------------------------------------------------------------------------------------------------------

OSStatus
CFirstPage::CheckStatus()
{
	DWORD		status = 0;
	DWORD		dwSize = sizeof( DWORD );
	OSStatus	err;

	// Get the status field 

	err = RegQueryValueEx( m_statusKey, L"Status", NULL, NULL, (LPBYTE) &status, &dwSize );      
	require_noerr( err, exit );

	ShowStatus( status );

exit:

	return kNoErr;
}


//---------------------------------------------------------------------------------------------------------------------------
//	CFirstPage::ShowStatus
//---------------------------------------------------------------------------------------------------------------------------

void
CFirstPage::ShowStatus( DWORD status )
{
	if ( status )
	{
		m_failureIcon.ShowWindow( SW_HIDE );
		m_successIcon.ShowWindow( SW_SHOW );
	}
	else
	{
		m_failureIcon.ShowWindow( SW_SHOW );
		m_successIcon.ShowWindow( SW_HIDE );
	}
}


//---------------------------------------------------------------------------------------------------------------------------
//	CFirstPage::OnRegistryChanged
//---------------------------------------------------------------------------------------------------------------------------

void
CFirstPage::OnRegistryChanged()
{
	CheckStatus();
}