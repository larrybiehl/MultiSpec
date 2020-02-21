//                                     MultiSpec
//
//                   Copyright 1988-2020 Purdue Research Foundation
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use
// this file except in compliance with the License. You may obtain a copy of the
// License at:  https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the specific
// language governing permissions and limitations under the License.
//
// MultiSpec is curated by the Laboratory for Applications of Remote Sensing at
// Purdue University in West Lafayette, IN and licensed by Larry Biehl.
//
//	File:						WAbout.h
//	Implementation:		WAbout.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CAboutDlg class
//
//	Written By:				Larry L. Biehl			Date: 05/23/1995
//	Revised By:				Larry L. Biehl			Date: 12/31/2017
//	
//------------------------------------------------------------------------------------

#pragma once

#include "WDialog.h"
#include "afxwin.h"

class CAboutDlg : public CMDialog
{
	public:
		CAboutDlg (
				CWnd* 								pParent = NULL);
		
		BOOL Create (
				CWnd* 								pParent);
		
		Boolean DoDialog (void);
	
		afx_msg void OnStnClickedStatic11 ();
	
		afx_msg void OnBnClickedCheck1 ();
	
		afx_msg void OnBnClickedSimplerversion ();
	
		// Dialog Data
		//{{AFX_DATA (CAboutDlg)
		enum { IDD = IDD_ABOUTBOX };
		//}}AFX_DATA
	
		BOOL 									m_simplerVersionFlag;
			
	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);    // DDX/DDV support
		
		Boolean								m_fromStartUpFlag,
												m_FullVersionSettingFlag,
												m_initializedFlag;
		
	//{{AFX_MSG (CAboutDlg)
	virtual BOOL OnInitDialog ();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP ()
	
};	// end class CAboutDlg
