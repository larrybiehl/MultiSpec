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
//	File:						WEditClassFieldDialog.h
//	Implementation:		WEditClassFieldDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMEditClassFieldDlg class.
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/05/2019
//
//------------------------------------------------------------------------------------

#pragma once
	
#include "WDialog.h"


class CMEditClassFieldDlg : public CMDialog
{
	// Construction
	public:
		CMEditClassFieldDlg (	// standard constructor
				CWnd* 								pParent = NULL);
	
		Boolean DoDialog (
				SInt16								classFieldCode,
				SInt16								currentClass,
				SInt16								currentField,
				char*									titleStringPtr);

		// Dialog Data
		//{{AFX_DATA (CMEditClassFieldDlg)
		enum { IDD = IDD_ClassField };
		CString								m_className,
												m_fieldName;
	
		int									m_fieldType;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMEditClassFieldDlg)
		virtual BOOL OnInitDialog ();
	
		virtual void OnOK ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
		char*									m_titleStringPtr;
	
		SInt16*								m_fieldTypePtr;
	
		TBYTE									*m_classNameCStringPtr,
												*m_fieldNameCStringPtr;
	
		SInt16								m_classFieldCode,
												m_classStorage,
												m_currentField;
	
		Boolean								m_initializedFlag;
	
};	// end class CMEditClassFieldDlg
