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
//	File:						WAppendFileDialog.h
//	Implementation:		WAppendFileDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMAppendFileDialog class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1990's
//	Revised By:				Larry L. Biehl			Date: 12/31/2017
//	
//------------------------------------------------------------------------------------

#pragma once
	     
#include "WDialog.h"

class CMAppendFileDialog : public CMDialog
{
	public:
		CMAppendFileDialog (	// standard constructor
				CWnd* 								pParent = NULL);
		
		SInt16 DoDialog (
				FileInfoPtr							appendFileInfoPtr,
				FileInfoPtr							newFileInfoPtr,
				Boolean								modifyFlag,
				SInt64*								bytesToSkipPtr,
				Boolean								lineFlag,
				SInt32								newAfterLineChannel);

		// Dialog Data
		//{{AFX_DATA (CMAppendFileDialog)
		enum { IDD = IDD_AppendFile };
	
		CString								m_appendFileName;
	
		int									m_lineChannel;
		
		long									m_lineChannelNumber;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMAppendFileDialog)
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnLineChannel ();
	
		afx_msg void OnLastLineChannel ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP () 
		
		FileInfoPtr							m_appendFileInfoPtr;
								
		SInt64*								m_bytesToSkipPtr;
															  
		SInt32								m_maximumValue;
		 
		Boolean								m_initializedFlag,
												m_lineFlag,
												m_modifyFlag;
		
}; // end class CMAppendFileDialog
