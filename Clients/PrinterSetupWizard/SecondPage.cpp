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
    
$Log: SecondPage.cpp,v $
Revision 1.15  2005/04/13 17:46:22  shersche
<rdar://problem/4082122> Generic PCL not selected when printers advertise multiple text records

Revision 1.14  2005/03/20 20:08:37  shersche
<rdar://problem/4055670> Second screen should not select a printer by default

Revision 1.13  2005/02/15 07:50:10  shersche
<rdar://problem/4007151> Update name

Revision 1.12  2005/02/10 22:35:11  cheshire
<rdar://problem/3727944> Update name

Revision 1.11  2005/01/31 23:54:30  shersche
<rdar://problem/3947508> Start browsing when printer wizard starts. Move browsing logic from CSecondPage object to CPrinterSetupWizardSheet object.

Revision 1.10  2005/01/20 19:54:38  shersche
Fix parse error when text record is NULL

Revision 1.9  2005/01/06 08:13:50  shersche
Don't use moreComing flag to determine number of text record, disregard queue name if qtotal isn't defined, don't disregard queue name if "rp" is the only key specified

Revision 1.8  2005/01/04 21:09:14  shersche
Fix problems in parsing text records. Fix problems in remove event handling. Ensure that the same service can't be resolved more than once.

Revision 1.7  2004/12/31 07:25:27  shersche
Tidy up printer management, and fix memory leaks when hitting 'Cancel'

Revision 1.6  2004/12/30 01:24:02  shersche
<rdar://problem/3906182> Remove references to description key
Bug #: 3906182

Revision 1.5  2004/12/30 01:02:47  shersche
<rdar://problem/3734478> Add Printer information box that displays description and location information when printer name is selected
Bug #: 3734478

Revision 1.4  2004/12/29 18:53:38  shersche
<rdar://problem/3725106>
<rdar://problem/3737413> Added support for LPR and IPP protocols as well as support for obtaining multiple text records. Reorganized and simplified codebase.
Bug #: 3725106, 3737413

Revision 1.3  2004/09/13 21:26:15  shersche
<rdar://problem/3796483> Use the moreComing flag to determine whether drawing should take place in OnAddPrinter and OnRemovePrinter callbacks
Bug #: 3796483

Revision 1.2  2004/06/26 03:19:57  shersche
clean up warning messages

Submitted by: herscher

Revision 1.1  2004/06/18 04:36:57  rpantos
First checked in


*/

#include "stdafx.h"
#include "PrinterSetupWizardApp.h"
#include "PrinterSetupWizardSheet.h"
#include "SecondPage.h"
#include "DebugServices.h"
#include "WinServices.h"
#include <winspool.h>

// local variable is initialize but not referenced
#pragma warning(disable:4189)

// CSecondPage dialog

IMPLEMENT_DYNAMIC(CSecondPage, CPropertyPage)
CSecondPage::CSecondPage()
	: CPropertyPage(CSecondPage::IDD)
{
	m_psp.dwFlags &= ~(PSP_HASHELP);
	m_psp.dwFlags |= PSP_DEFAULT|PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;

	m_psp.pszHeaderTitle	= MAKEINTRESOURCE(IDS_BROWSE_TITLE);
	m_psp.pszHeaderSubTitle	= MAKEINTRESOURCE(IDS_BROWSE_SUBTITLE);

	m_emptyListItem		=	NULL;
	m_initialized		=	false;
	m_waiting			=	false;
}


CSecondPage::~CSecondPage()
{
}


void
CSecondPage::InitBrowseList()
{
	CPrinterSetupWizardSheet	*	psheet;
	CString							text;

	psheet = reinterpret_cast<CPrinterSetupWizardSheet*>(GetParent());
	require_quiet( psheet, exit );

	//
	// load the no printers message until something shows up in the browse list
	//
	text.LoadString(IDS_NO_PRINTERS);

	LoadTextAndDisableWindow( text );

	//
	// disable the next button until there's a printer to select
	//
	psheet->SetWizardButtons(PSWIZB_BACK);

	//
	// disable the printer information box
	//
	SetPrinterInformationState( FALSE );
	m_descriptionField.SetWindowText( L"" );
	m_locationField.SetWindowText( L"" );

	//
	// and wait for the user to either hit the mouse or keyboard before selecting an item
	//
	m_gotChoice = false;

exit:

	return;
}


void CSecondPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BROWSE_LIST, m_browseList);
	DDX_Control(pDX, IDC_PRINTER_INFORMATION, m_printerInformation);

	DDX_Control(pDX, IDC_DESCRIPTION_LABEL, m_descriptionLabel);

	DDX_Control(pDX, IDC_DESCRIPTION_FIELD, m_descriptionField);

	DDX_Control(pDX, IDC_LOCATION_LABEL, m_locationLabel);

	DDX_Control(pDX, IDC_LOCATION_FIELD, m_locationField);

}


afx_msg BOOL
CSecondPage::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message)
{
	DEBUG_UNUSED(pWnd);
	DEBUG_UNUSED(nHitTest);
	DEBUG_UNUSED(message);

	CPrinterSetupWizardSheet * psheet;

	psheet = reinterpret_cast<CPrinterSetupWizardSheet*>(GetParent());
	require_quiet( psheet, exit );

	SetCursor(psheet->GetCursor());

exit:

	return TRUE;
}


BOOL
CSecondPage::OnSetActive()
{
	CPrinterSetupWizardSheet	*	psheet;
	Printer						*	printer;
	Printers::iterator				it;
	OSStatus						err = kNoErr;
	BOOL							b;

	b = CPropertyPage::OnSetActive();

	psheet = reinterpret_cast<CPrinterSetupWizardSheet*>(GetParent());
	require_action( psheet, exit, err = kUnknownErr );

	// initialize the browse list...this will remove everything currently
	// in it, and add the no printers item

	InitBrowseList();

	// And populate the list with any printers that we currently know about

	for ( it = psheet->m_printers.begin(); it != psheet->m_printers.end(); it++ )
	{
		OnAddPrinter( *it, false );
	}

	printer = psheet->GetSelectedPrinter();

	if ( printer != NULL )
	{
		m_browseList.Select( printer->item, TVGN_FIRSTVISIBLE );
	}

exit:

	return b;
}


BOOL
CSecondPage::OnKillActive()
{
	return CPropertyPage::OnKillActive();
}


BEGIN_MESSAGE_MAP(CSecondPage, CPropertyPage)
	ON_NOTIFY(TVN_SELCHANGED, IDC_BROWSE_LIST, OnTvnSelchangedBrowseList)
	ON_NOTIFY(NM_CLICK, IDC_BROWSE_LIST, OnNmClickBrowseList)
	ON_NOTIFY(TVN_KEYDOWN, IDC_BROWSE_LIST, OnTvnKeyDownBrowseList)
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()


// Printer::EventHandler implementation
OSStatus
CSecondPage::OnAddPrinter(
					Printer	*	printer,
					bool		moreComing )
{
	CPrinterSetupWizardSheet	*	psheet;
	OSStatus						err = kNoErr;

	check( IsWindow( m_hWnd ) );

	m_browseList.SetRedraw(FALSE);

	psheet = reinterpret_cast<CPrinterSetupWizardSheet*>(GetParent());
	require_quiet( psheet, exit );

	printer->item = m_browseList.InsertItem(printer->displayName);

	m_browseList.SetItemData( printer->item, (DWORD_PTR) printer );

	m_browseList.SortChildren(TVI_ROOT);

	//
	// if the searching item is still in the list
	// get rid of it
	//
	// note that order is important here.  Insert the printer
	// item before removing the placeholder so we always have
	// an item in the list to avoid experiencing the bug
	// in Microsoft's implementation of CTreeCtrl
	//
	if (m_emptyListItem != NULL)
	{
		m_browseList.DeleteItem(m_emptyListItem);
		m_emptyListItem = NULL;
		m_browseList.EnableWindow(TRUE);
	}

exit:

	if (!moreComing)
	{
		m_browseList.SetRedraw(TRUE);
		m_browseList.Invalidate();
	}

	return err;
}


OSStatus
CSecondPage::OnRemovePrinter(
				Printer	*	printer,
				bool		moreComing)
{
	CPrinterSetupWizardSheet	*	psheet;
	OSStatus						err = kNoErr;

	check( IsWindow( m_hWnd ) );
	check( printer );

	psheet = reinterpret_cast<CPrinterSetupWizardSheet*>(GetParent());
	require_quiet( psheet, exit );

	m_browseList.SetRedraw(FALSE);

	//
	// check to make sure if we're the only item in the control...i.e.
	// the list size is 1.
	//
	if (m_browseList.GetCount() > 1)
	{
		//
		// if we're not the only thing in the list, then
		// simply remove it from the list
		//
		m_browseList.DeleteItem( printer->item );
	}
	else
	{
		//
		// if we're the only thing in the list, then redisplay
		// it with the no printers message
		//
		InitBrowseList();
	}

exit:

	if ( !moreComing )
	{
		m_browseList.SetRedraw(TRUE);
		m_browseList.Invalidate();
	}

	return err;
}


void
CSecondPage::OnResolveService( Service * service )
{
	CPrinterSetupWizardSheet * psheet = reinterpret_cast<CPrinterSetupWizardSheet*>(GetParent());
	require_quiet( psheet, exit );

	check( service );

	Queue *	q = service->SelectedQueue();

	check( q );
	

	//
	// and set it to selected
	//

	m_selectedName	= service->printer->name;

	//
	// and update the printer information box
	//
	SetPrinterInformationState( TRUE );

	m_descriptionField.SetWindowText( q->description );
	m_locationField.SetWindowText( q->location );

	//
	// reset the cursor
	//

	SetCursor(psheet->m_active);

exit:

	return;
}


void CSecondPage::OnTvnSelchangedBrowseList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW					pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	CPrinterSetupWizardSheet	*	psheet;
	Printer						*	printer;
	int								err = 0;

	psheet = reinterpret_cast<CPrinterSetupWizardSheet*>(GetParent());
	require_action( psheet, exit, err = kUnknownErr );

	// The strange code here is to workaround a bug in the CTreeCtrl, whereupon the item
	// we selected isn't passed through correctly to this callback routine.

	if ( !m_gotChoice )
	{
		printer = psheet->GetSelectedPrinter();

		// If we really haven't selected a printer, then re-select NULL and exit

		if ( !printer )
		{
			m_browseList.SelectItem( NULL );

			goto exit;
		}

		// If we already have selected a printer, fake like we've clicked on it, but only
		// if the CTreeCtrl hasn't already selected it
		
		else if ( printer->item != m_browseList.GetSelectedItem() )
		{
			m_gotChoice = true;

			m_browseList.SelectItem( printer->item );

			goto exit;
		}
	}

	HTREEITEM item = m_browseList.GetSelectedItem();
	require_quiet( item, exit );

	printer = reinterpret_cast<Printer*>(m_browseList.GetItemData( item ) );
	require_quiet( printer, exit );

	//
	// this call will trigger a resolve.  When the resolve is complete,
	// our OnResolve will be called.
	//
	err = psheet->StartResolve( printer );
	require_noerr( err, exit );

	//
	// And clear out the printer information box
	//
	SetPrinterInformationState( FALSE );
	m_descriptionField.SetWindowText(L"");
	m_locationField.SetWindowText(L"");

exit:

	if (err != 0)
	{
		CString text;
		CString caption;

		text.LoadString(IDS_ERROR_SELECTING_PRINTER_TEXT);
		caption.LoadString(IDS_ERROR_SELECTING_PRINTER_CAPTION);

		MessageBox(text, caption, MB_OK|MB_ICONEXCLAMATION);
	}

	*pResult = 0;
}


void CSecondPage::OnNmClickBrowseList(NMHDR *pNMHDR, LRESULT *pResult)
{
	DEBUG_UNUSED( pNMHDR );

	m_gotChoice = true;

	*pResult = 0;
}


void CSecondPage::OnTvnKeyDownBrowseList( NMHDR * pNMHDR, LRESULT * pResult)
{
	DEBUG_UNUSED( pNMHDR );

	m_gotChoice = true;

	*pResult = 0;
}


void
CSecondPage::LoadTextAndDisableWindow( CString & text )
{
	m_emptyListItem = m_browseList.InsertItem( text, 0, 0, NULL, TVI_FIRST );
	m_browseList.SelectItem( NULL );

	//
	// this will remove everything else in the list...we might be navigating
	// back to this window, and the browse list might have changed since
	// we last displayed it.
	//
	if ( m_emptyListItem )
	{
		HTREEITEM item = m_browseList.GetNextVisibleItem( m_emptyListItem );
  
		while ( item )
		{
			m_browseList.DeleteItem( item );
			item = m_browseList.GetNextVisibleItem( m_emptyListItem );
		}
	}

	m_browseList.EnableWindow( FALSE );
}


void
CSecondPage::SetPrinterInformationState( BOOL state )
{
	m_printerInformation.EnableWindow( state );

	m_descriptionLabel.EnableWindow( state );

	m_descriptionField.EnableWindow( state );

	m_locationLabel.EnableWindow( state );

	m_locationField.EnableWindow( state );

}



