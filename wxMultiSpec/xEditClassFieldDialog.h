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
//	File:						xEditClassFieldDialog.h
//	Implementation:		xEditClassFieldDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMEditClassFieldDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 07/07/2017
//
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"


class CMEditClassFieldDlg : public CMDialog
{
		 // Construction
	public:
				// standard constructor
		CMEditClassFieldDlg (
				wxWindow* 							pParent = NULL,
				wxWindowID 							id = IDD_ClassifyDialog,
				const wxString& 					title = wxT("Set Project Options"));

		Boolean DoDialog (
				SInt16 								classFieldCode,
				SInt16 								currentClass,
				SInt16 								currentField,
				char* 								titleStringPtr);


				// Dialog Data

		enum {
			IDD = IDD_ClassField
			};
	
		wxString								m_className,
				 								m_fieldName;
	
		char 									m_classNamebuf[1024],
			 									m_fieldNamebuf[1024];

		 // Implementation
	protected:
				// Generated message map functions
	
		void CheckLength (
				wxCommandEvent& 					event);
	
		void OnInitDialog (
				wxInitDialogEvent& 				event);
		DECLARE_EVENT_TABLE ()
	
		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	
	
		char									*m_classNameCStringPtr,
												*m_fieldNameCStringPtr,
												*m_titleStringPtr;
	
		SInt16*								m_fieldTypePtr;
	
		wxBoxSizer							*bSizer285;
	
		wxButton								*m_button84,
												*m_button85;
	
		wxComboBox							*m_comboBox56;
	
		wxRadioButton						*m_radioBtn27,
												*m_radioBtn28;
	
		wxStaticText						*m_staticText265,
												*m_staticText266,
												*m_staticText267,
												*m_staticText268,
												*m_staticText269,
												*m_staticText270;
	
		wxTextCtrl							*m_textCtrl102,
												*m_textCtrl103;

		int 									m_fieldType;

		SInt16								m_classFieldCode,
												m_classStorage,
												m_currentField;
	
		Boolean								m_initializedFlag;
	
};	// end "class CMEditClassFieldDlg"
