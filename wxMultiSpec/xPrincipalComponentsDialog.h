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
//	File:						xPrincipalComponentsDialog.h
//	Implementation:		xPrincipalComponentsDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	Header file for the CMPrincipalCompDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 11/11/2019
//	
//------------------------------------------------------------------------------------

#pragma once
            
#include "xDialog.h"
#include "wx/wx.h"


class CMPrincipalCompDialog : public CMDialog
{
	DECLARE_DYNAMIC_CLASS (CMPrincipalCompDialog)
			// Construction
	public:
				// standard constructor
		CMPrincipalCompDialog (
		 		wxWindow* 							pParent = NULL,
		 		const wxString& 					title =
									wxT("Set Principal Component Analysis Specifications:"));

		~CMPrincipalCompDialog (); // standard desctructor

		SInt16 DoDialog (void);


		enum { IDD = IDD_PrincipalComponent };

			// Implementation
	protected:
				// Generated message map functions
	
		void OnChannelComboSelendok (
				wxCommandEvent& 					event);
	
		void OnClassComboSelendok (
				wxCommandEvent& 					event);
	
		void OnClasses (
				wxCommandEvent& 					event);
	
		void OnImageArea (
				wxCommandEvent& 					event);
	
		void OnInitDialog (
				wxInitDialogEvent& 				event);
		DECLARE_EVENT_TABLE ()
	
		void CreateControls ();
	
		void HideFieldTypeItems ();

		void ShowFieldTypeItems ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	

		wxCheckBox							*m_checkBox24,
												*m_checkBox25,
												*m_checkBox26,
												*m_checkBox27,
												*m_checkBox28,
												*m_checkBox29,
												*m_checkBox30;
	
		wxRadioButton						*m_radioBtn10,
												*m_radioBtn11;

		wxStaticText						*m_staticText118;
	
		int 									m_dataCode;
	
		bool 									m_diskFileFlag,
												m_equalVariancesFlag,
												m_listEigenvectorFlag,
												m_saveEigenvectorFlag,
												m_testFlag,
												m_textWindowFlag,
												m_trainingFlag;
	
		Boolean 								m_initializedFlag;
	
};	// end "class CMPrincipalCompDialog"
