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
//	File:						WClassGroupDialog.h
//	Implementation:		WClassGroupDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMClassGroupDlg class
//
//	Written By:				Larry L. Biehl			Date: 0??/??/1990's
//	Revised By:				Larry L. Biehl			Date: 12/05/2019
//
//------------------------------------------------------------------------------------

#pragma once
	
#include "WOneColumnDialog.h"


class CMClassGroupDlg : public CMOneColDlg
{
	// Construction
	public:
		CMClassGroupDlg (	// standard constructor
				CWnd* 								pParent = NULL);
	
		Boolean DoDialog (
				UInt32*								numberOutputItemsPtr,
				SInt16* 								listPtr,
				SInt16								minimumNumberItems,
				SInt16								numberInputItems,
				SInt16								classGroupCode);

		// Dialog Data
		//{{AFX_DATA (CMClassGroupDlg)
			// NOTE: the ClassWizard will add data members here
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMClassGroupDlg)
		virtual BOOL OnInitDialog ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
		
	
		SInt16								m_classGroupCode;
	
		Boolean								m_initializedFlag;
	
};	// end class CMClassGroupDlg
