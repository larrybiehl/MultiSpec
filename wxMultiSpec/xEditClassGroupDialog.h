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
//	File:						xEditClassGroupDialog.h
//	Implementation:		xEditClassGroupDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMEditClassGroupDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 07/26/2017
//
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h" 
#include "xLegendList.h"


class CMEditClassGroupDlg : public CMDialog 
	{
				// Construction
	public:
		CMEditClassGroupDlg (
				wxWindow* 							pParent = NULL); // standard constructor

		Boolean DoDialog (
				CMLegendList*						legendListCPtr,
				SInt16								selectedCell,
				SInt16								newEditCode,
				SInt16								classGroupIndex,
				Handle								fileInfoHandle,
				Handle								nameHandle);
	

				// Dialog Data

		enum
			{
			IDD = IDD_EditClassGroupName
			};

		// Implementation
	protected:
				// Generated message map functions

		void OnInitDialog (
				wxInitDialogEvent& 				event);
		DECLARE_EVENT_TABLE ()
	
		void CreateControls ();
	
		Boolean OnOK ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	
	
		char									m_clgrpnamebuf[1024];
	
		wxString 							m_classGroupName;

		char*									m_classGroupCStringPtr;
	
		wxButton								*m_button42,
												*m_button43;
	
		wxTextCtrl							*m_textCtrl71;
	
		Handle								m_fileInfoHandle,
												m_nameHandle;
	
		SInt16								m_classGroupIndex,
												m_newEditCode,
												m_numberClassesGroups;
	
		Boolean								m_initializedFlag,
												m_noChangeFlag;
	
};	// end "class CMEditClassGroupDlg"
