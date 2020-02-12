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
//	File:						WEditClassGroupDialog.h
//	Implementation:		WEditClassGroupDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMEditClassGroupDlg class.
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/05/2019
//
//------------------------------------------------------------------------------------

#pragma once
	
#include "WDialog.h"
#include "WLegendList.h"


class CMEditClassGroupDlg : public CMDialog
{
	// Construction
	public:
		CMEditClassGroupDlg (	// standard constructor
				CWnd* 								pParent = NULL);
	
		Boolean DoDialog (
				CMLegendList*						legendListCPtr,
				SInt16								selectedCell,
				SInt16								newEditCode,
				SInt16								classGroupIndex,
				Handle								fileInfoHandle,
				Handle								nameHandle);

		// Dialog Data
		//{{AFX_DATA (CMEditClassGroupDlg)
		enum { IDD = IDD_EditClassGroupName };
	
		CString								m_classGroupName;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMEditClassGroupDlg)
		virtual BOOL OnInitDialog ();
	
		virtual void OnOK ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
		TBYTE*								m_classGroupCStringPtr;
	
		Handle								m_fileInfoHandle,
												m_nameHandle;
	
		SInt16								m_classGroupIndex,
												m_newEditCode,
												m_numberClassesGroups;
	
		Boolean								m_initializedFlag,
												m_noChangeFlag;
	
};	// end class CMEditClassGroupDlg
