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
//	File:						WPrincipalComponentsDialog.h
//	Implementation:		WPrincipalComponentsDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMPrincipalCompDialog class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/12/2019
//
//------------------------------------------------------------------------------------

#pragma once
	     
#include "WDialog.h"


class CMPrincipalCompDialog : public CMDialog
{
	// Construction
	public:
		CMPrincipalCompDialog (
				CWnd* 								pParent = NULL);
	
		~CMPrincipalCompDialog ();		// standard desctructor
	
		SInt16 DoDialog (void);

		// Dialog Data
		//{{AFX_DATA (CMPrincipalCompDialog)
		enum { IDD = IDD_PrincipalComponent };
	
		int									m_dataCode;
	
		BOOL									m_diskFileFlag,
												m_equalVariancesFlag,
												m_listEigenvectorFlag,
												m_saveEigenvectorFlag,
												m_testFlag,
												m_textWindowFlag,
												m_trainingFlag;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support
	
		void HideFieldTypeItems ();
	
		void ShowFieldTypeItems ();

		// Generated message map functions
		//{{AFX_MSG (CMPrincipalCompDialog)
		afx_msg void OnClasses ();
	
		afx_msg void OnImageArea ();
	
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnSelendokChannelCombo ();
	
		afx_msg void OnSelendokClassCombo ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
	
		Boolean								m_initializedFlag;
	
};	// end class CMPrincipalCompDialog
