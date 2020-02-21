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
//	File:						WOpenFileDialog.h
//	Implementation:		WOpenFileDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMOpenFileDialog class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/12/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include <afxdlgs.h>

UINT_PTR CALLBACK HookProcedure (
			HWND 									hdlg,
			UINT 									uiMsg,
			WPARAM 								wParam,
			LPARAM 								lParam);

class CMOpenFileDialog : public CFileDialog
{
	// Construction
	public:
		CMOpenFileDialog (
				CWnd* 								pParent = NULL,
				TBYTE* 								filterString = NULL);	// standard constructor
	
		~CMOpenFileDialog ();	// standard desctructor
	
		Boolean DoDialog (
				SInt16								stringIndex);

		// Dialog Data
		//{{AFX_DATA (CMOpenFileDialog)
		//	enum { IDD = 1536 };
			// NOTE: the ClassWizard will add data members here
		CString								m_prompt;
	
		int									m_imageType,
												m_linkOption;
		//}}AFX_DATA
	
		Boolean								m_initializedFlag;

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support
	
		BOOL GetItem11String (
				UInt16								stringID,
				CString*								stringPtr);

		virtual void OnFileNameChange (void);
	
		void SetImageLinkToFalse (void);
	
		void SetImageLinkToTrue (void);

		// Generated message map functions
		//{{AFX_MSG (CMOpenFileDialog)
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnSelendokImageType ();
	
		afx_msg void OnSelendokLinkOption ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()

		CString								m_fileName;
	
		CDataExchange						*m_dialogFromPtr,
												*m_dialogToPtr;
	
		TBYTE*								m_filenamesBuffer;

		UInt32								m_linkOptionSelectionDataCode,
												m_selectedFileCount;
	
		SInt16            				m_stringIndex;

		Boolean								m_initialLinkSelectedFilesFlag,
												m_showLinkPopupMenuFlag,
												m_userSetImageType;

};	// end class CMOpenFileDialog
