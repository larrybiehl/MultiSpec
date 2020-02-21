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
//	File:						WNewClassFieldDialog.h
//	Implementation:		WNewClassFieldDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMNewClassFieldDlg class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 03/16/2017
//
//------------------------------------------------------------------------------------

#pragma once
	
#include "WDialog.h"


class CMNewClassFieldDlg : public CMDialog
{
	// Construction
	public:
		CMNewClassFieldDlg (	// standard constructor
				CWnd* 								pParent = NULL);

		// Dialog Data
		//{{AFX_DATA (CMNewClassFieldDlg)
		enum { IDD = IDD_ClassField };
	
		CString								m_className,
												m_fieldName;
	
		int									m_classList,
												m_fieldType;
		//}}AFX_DATA
	
		Boolean DoDialog (
				Boolean								newClassOnlyFlag,
				UCharPtr								classNamePtr,
				UCharPtr								fieldNamePtr,
				SInt16*		 						fieldTypePtr);

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMNewClassFieldDlg)
		afx_msg void OnFieldType ();
	
		virtual BOOL OnInitDialog ();
	
		virtual void OnOK ();
	
		afx_msg void OnSelendokClassList ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
		SInt64								m_numberSelectionPixels;
	
		UCharPtr								m_classNamePtr,
												m_fieldNamePtr;
	
		SInt16*								m_fieldTypePtr;
	
		TBYTE									*m_classNameCStringPtr,
												*m_fieldNameCStringPtr;
	
		Boolean								m_initializedFlag,
												m_newClassOnlyFlag;
	
};	// end class CMNewClassFieldDlg
