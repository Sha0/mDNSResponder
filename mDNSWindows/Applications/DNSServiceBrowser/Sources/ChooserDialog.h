/*
 * Copyright (c) 2002 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 1.2 (the
 * "License").  You may not use this file except in compliance with the
 * License.  Please obtain a copy of the License at
 * http://www.apple.com/publicsource and read it before using this file.
 * 
 * This Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
/*
	$Id: ChooserDialog.h,v 1.2 2002/09/21 20:44:55 zarzycki Exp $

	Contains:	Rendezvous Browser for Windows.
	
	Written by: Bob Bradley
	
    Version:    Rendezvous, September 2002

    Copyright:  Copyright (C) 2002 by Apple Computer, Inc., All Rights Reserved.

    Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple Computer, Inc.
                ("Apple") in consideration of your agreement to the following terms, and your
                use, installation, modification or redistribution of this Apple software
                constitutes acceptance of these terms.  If you do not agree with these terms,
                please do not use, install, modify or redistribute this Apple software.

                In consideration of your agreement to abide by the following terms, and subject
                to these terms, Apple grants you a personal, non-exclusive license, under Apple's
                copyrights in this original Apple software (the "Apple Software"), to use,
                reproduce, modify and redistribute the Apple Software, with or without
                modifications, in source and/or binary forms; provided that if you redistribute
                the Apple Software in its entirety and without modifications, you must retain
                this notice and the following text and disclaimers in all such redistributions of
                the Apple Software.  Neither the name, trademarks, service marks or logos of
                Apple Computer, Inc. may be used to endorse or promote products derived from the
                Apple Software without specific prior written permission from Apple.  Except as
                expressly stated in this notice, no other rights or licenses, express or implied,
                are granted by Apple herein, including but not limited to any patent rights that
                may be infringed by your derivative works or by other works in which the Apple
                Software may be incorporated.

                The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
                WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
                WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
                PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN
                COMBINATION WITH YOUR PRODUCTS.

                IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
                CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
                GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
                ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION AND/OR DISTRIBUTION
                OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT, TORT
                (INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN
                ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    Change History (most recent first):
    
        $Log: ChooserDialog.h,v $
        Revision 1.2  2002/09/21 20:44:55  zarzycki
        Added APSL info

        Revision 1.1  2002/09/20 06:12:52  bradley
        Rendezvous Browser for Windows

*/

#if !defined(AFX_CHOOSERDIALOG_H__AC258704_B307_4901_9F98_A0AC022FD8AC__INCLUDED_)
#define AFX_CHOOSERDIALOG_H__AC258704_B307_4901_9F98_A0AC022FD8AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include	<string>
#include	<vector>

#include	"afxcmn.h"

#include	"Resource.h"

#include	"DNSServices.h"

//===========================================================================================================================
//	Structures
//===========================================================================================================================

struct	ServiceInstanceInfo
{
	std::string		name;
	std::string		type;
	std::string		domain;
	std::string		ip;
	std::string		text;
	std::string		ifIP;
};

struct	ServiceTypeInfo
{
	std::string		serviceType;
	std::string		description;
	std::string		urlScheme;
};

//===========================================================================================================================
//	ChooserDialog
//===========================================================================================================================

class	ChooserDialog : public CDialog
{
	public:

		ChooserDialog(CWnd* pParent = NULL);
		virtual	~ChooserDialog( void );
		
		//{{AFX_DATA(ChooserDialog)
		enum { IDD = IDD_CHOOSER_DIALOG };
		CListCtrl mServiceList;
		CListCtrl mDomainList;
		CListCtrl mChooserList;
		//}}AFX_DATA

		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(ChooserDialog)
		public:
		virtual BOOL PreTranslateMessage(MSG* pMsg);
		protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		virtual void PostNcDestroy();
		//}}AFX_VIRTUAL

	protected:
		
		typedef std::vector < ServiceInstanceInfo >		ServiceInstanceVector;
		typedef std::vector < ServiceTypeInfo >			ServiceTypeVector;
		
		HACCEL						mMenuAcceleratorTable;
		DNSBrowserRef				mBrowser;
		BOOL						mIsServiceBrowsing;
		ServiceInstanceVector		mServiceInstances;
		ServiceTypeVector			mServiceTypes;
		
	public:

		void	PopulateServicesList( void );
		void	UpdateInfoDisplay( void );
		
		void	StartBrowsing( const char *inType, const char *inDomain );
		void	StopBrowsing( void );

	protected:

		//{{AFX_MSG(ChooserDialog)
		virtual BOOL OnInitDialog();
		afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
		afx_msg void OnDomainListChanged(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnServiceListChanged(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnChooserListChanged(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnChooserListDoubleClick(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnAbout();
		afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
		afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
		afx_msg void OnFileClose();
		virtual void OnCancel();
		afx_msg void OnExit();
		afx_msg void OnClose();
		afx_msg void OnNcDestroy();
		//}}AFX_MSG
		afx_msg LONG OnDomainAdd( WPARAM inWParam, LPARAM inLParam );
		afx_msg LONG OnDomainRemove( WPARAM inWParam, LPARAM inLParam );
		afx_msg LONG OnServiceAdd( WPARAM inWParam, LPARAM inLParam );
		afx_msg LONG OnServiceRemove( WPARAM inWParam, LPARAM inLParam );
		afx_msg LONG OnResolve( WPARAM inWParam, LPARAM inLParam );
		DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHOOSERDIALOG_H__AC258704_B307_4901_9F98_A0AC022FD8AC__INCLUDED_)
