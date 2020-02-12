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
//	File:						CMLOOMixingDialog.h
//	Implementation:		CMLOOMixingDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	Header file for the CMLOOMixingDialog class
//
//	Written By:				Wei-Kang Hsu			Date: 01/26/2018
//	Revised By:				Larry L. Biehl			Date: 01/29/2018
//	
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"


class CMLOOMixingDialog : public CMDialog
{
			// Construction
	public:
	
				 // standard constructor
		CMLOOMixingDialog (
				wxWindow* 							pParent = NULL,
				wxWindowID 							id = wxID_ANY,
				const wxString& 					title = wxT("Set LOO Mixing Parameter"));

		Boolean DoDialog (
				SInt16								statsWindowMode);
	
	
		enum {IDD = IDD_LOOMixingParameter};


			// Implementation
	protected:
		void OnIdentityMatrix (
				wxCommandEvent&					event);

		void OnInitDialog (
				wxInitDialogEvent&				event);
			
		void OnOptimumMixing (
				wxCommandEvent&					event);
			
		void OnUserMixing (
				wxCommandEvent&					event);
		DECLARE_EVENT_TABLE ()
	 
		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	
		
		wxButton								*m_button76,
												*m_button77;
		  
		wxRadioButton						*m_radioBtn36,
												*m_radioBtn37,
												*m_radioBtn38;

		wxStaticText						*m_staticText291,
												*m_staticText293;
		
		wxTextCtrl							*m_textCtrl131;
	
		double								m_loocMixingParameter,
												m_userMixingParameter;
	
		int									m_mixingParameterCode;

		SInt16								m_statsWindowMode;
			      
};	// end "class CMLOOMixingDialog"
