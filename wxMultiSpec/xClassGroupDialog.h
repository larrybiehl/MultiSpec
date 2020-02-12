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
//	File:						xClassGroupDialog.h : CMClassGroupDlg Definition
//	Implementation:		xClassGroupDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	Header file for the CMClassGroupDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 01/12/2018
//	
//------------------------------------------------------------------------------------

#pragma once

#include "xOneColumnDialog.h"
#include "wx/wx.h"


class CMClassGroupDlg : public CMOneColDlg
{
			// Construction
	public:
		CMClassGroupDlg (
				wxWindow*							pParent = NULL); // standard constructor
	
		~CMClassGroupDlg ();
		
		bool DoDialog (
				UInt32*								numberOutputItemsPtr,
				SInt16*								listPtr,
				SInt16								minimumNumberItems,
				SInt16								numberInputItems,
				SInt16								classGroupCode);
				
			// Implementation
	protected:
		
				// Generated message map functions
		bool OnInitDialog ();
	
		void CreatControls ();

		wxBoxSizer*							bSizerm;
		
		SInt16								m_classGroupCode;
	
		Boolean								m_initializedFlag;

};	// end "class CMClassGroupDlg"
