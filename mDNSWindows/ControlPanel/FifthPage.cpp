/* -*- Mode: C; tab-width: 4 -*-
 *
 * Copyright (c) 2002-2004 Apple Computer, Inc. All rights reserved.
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

#include "FifthPage.h"
#include "resource.h"

#include "ConfigPropertySheet.h"
#include "SharedSecret.h"

#include <WinServices.h>
    
#define MAX_KEY_LENGTH 255


IMPLEMENT_DYNCREATE(CFifthPage, CPropertyPage)


//---------------------------------------------------------------------------------------------------------------------------
//	CFifthPage::CFifthPage
//---------------------------------------------------------------------------------------------------------------------------

CFifthPage::CFifthPage()
:
	CPropertyPage(CFifthPage::IDD)
{
	//{{AFX_DATA_INIT(CFifthPage)
	//}}AFX_DATA_INIT
}


//---------------------------------------------------------------------------------------------------------------------------
//	CFifthPage::~CFifthPage
//---------------------------------------------------------------------------------------------------------------------------

CFifthPage::~CFifthPage()
{
}


//---------------------------------------------------------------------------------------------------------------------------
//	CFifthPage::DoDataExchange
//---------------------------------------------------------------------------------------------------------------------------

void CFifthPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFifthPage)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_ADVERTISE_SMB, m_checkBox);
}

BEGIN_MESSAGE_MAP(CFifthPage, CPropertyPage)
	//{{AFX_MSG_MAP(CFifthPage)
	//}}AFX_MSG_MAP

	ON_BN_CLICKED(IDC_ADVERTISE_SMB, &CFifthPage::OnBnClickedAdvertiseSMB)

END_MESSAGE_MAP()


//---------------------------------------------------------------------------------------------------------------------------
//	CFifthPage::SetModified
//---------------------------------------------------------------------------------------------------------------------------

void CFifthPage::SetModified( BOOL bChanged )
{
	m_modified = bChanged;

	CPropertyPage::SetModified( bChanged );
}


//---------------------------------------------------------------------------------------------------------------------------
//	CFifthPage::OnSetActive
//---------------------------------------------------------------------------------------------------------------------------

BOOL
CFifthPage::OnSetActive()
{
	CConfigPropertySheet	*	psheet;
	HKEY						key = NULL;
	DWORD						dwSize;
	DWORD						enabled;
	DWORD						err;
	BOOL						b = CPropertyPage::OnSetActive();

	psheet = reinterpret_cast<CConfigPropertySheet*>(GetParent());
	require_quiet( psheet, exit );

	m_checkBox.SetCheck( 0 );

	// Now populate the browse domain box

	err = RegCreateKeyEx( HKEY_LOCAL_MACHINE, kServiceParametersNode L"\\Services\\SMB", 0,
		                  NULL, REG_OPTION_NON_VOLATILE, KEY_READ|KEY_WRITE|KEY_WOW64_32KEY, NULL, &key, NULL );
	require_noerr( err, exit );

	dwSize = sizeof( DWORD );
	err = RegQueryValueEx( key, L"Advertise", NULL, NULL, (LPBYTE) &enabled, &dwSize );
	require_noerr( err, exit );

	m_checkBox.SetCheck( enabled );
 
exit:

	if ( key )
	{
		RegCloseKey( key );
	}

	return b;
}


//---------------------------------------------------------------------------------------------------------------------------
//	CFifthPage::OnOK
//---------------------------------------------------------------------------------------------------------------------------

void
CFifthPage::OnOK()
{
	if ( m_modified )
	{
		Commit();
	}
}



//---------------------------------------------------------------------------------------------------------------------------
//	CFifthPage::Commit
//---------------------------------------------------------------------------------------------------------------------------

void
CFifthPage::Commit()
{
	HKEY		key		= NULL;
	DWORD		enabled;
	DWORD		err;

	err = RegCreateKeyEx( HKEY_LOCAL_MACHINE, kServiceParametersNode L"\\Services\\SMB", 0,
	                   	NULL, REG_OPTION_NON_VOLATILE, KEY_READ|KEY_WRITE|KEY_WOW64_32KEY, NULL, &key, NULL );
	require_noerr( err, exit );

	enabled = m_checkBox.GetCheck();
	err = RegSetValueEx( key, L"Advertise", NULL, REG_DWORD, (LPBYTE) &enabled, sizeof( enabled ) );
	require_noerr( err, exit );
	
exit:

	if ( key )
	{
		RegCloseKey( key );
	}
}


//---------------------------------------------------------------------------------------------------------------------------
//	CFifthPage::OnBnClickedRemoveBrowseDomain
//---------------------------------------------------------------------------------------------------------------------------


void CFifthPage::OnBnClickedAdvertiseSMB()
{
	SetModified( TRUE );
}

