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
//	File:						xClassWeightsDialog.h
//	Implementation:		xClassWeightsDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	Header file for the CMClassifyDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 04/30/2018
//	
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"

class CMClassWeightsDlg : public CMDialog
{
			// Construction
	public:
		CMClassWeightsDlg (
				wxWindow*							pParent = NULL,
				wxWindowID							id = IDD_ClassWeights,
				const wxString&					title = wxT("Specify Class Weights"));
	
		~CMClassWeightsDlg ();

		SInt16 DoDialog (
				UInt16								numberOutputClassesToUse,
				SInt16*								classListPtr,
				float*								weightsListPtr,
				SInt16								weightsSelection,
				Boolean								useEnhancedStatFlag);
	
				// Dialog Data

		enum {
			IDD = IDD_ClassWeights
			};

			// Implementation
	protected:
		void OnChangeWeight (
				wxCommandEvent&					event);

		void OnEnterNewWeightButton (
				wxCommandEvent&					event);

		void OnEqualWeightButton (
				wxCommandEvent&					event);

		void OnHelpButton (
				wxCommandEvent&					event);

		void OnInitDialog (
				wxInitDialogEvent& 				event);
	
		void OnOK (
				wxCommandEvent&					event);

		void OnSelchangeList (
				wxCommandEvent&					event);
		DECLARE_EVENT_TABLE ()
		
		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	

		wxFont								m_weightsListFont;
	
		wxString								m_weightString;
	
		float*								m_weightsListPtr;
		SInt16*								m_classListPtr;

		wxBoxSizer							*bSizer136;
		
		wxButton								*m_button30,
												*m_button31,
												*m_button32,
												*m_button33,
												*m_button34;
	
		wxListBox							*m_listBox1;
	
		wxRadioButton						*m_radioBtn11,
												*m_radioBtn12;
	
		wxStaticText						*m_staticText188,
												*m_staticText189,
												*m_staticText190,
												*m_staticText191,
												*m_staticText192;
	
		wxTextCtrl 							*m_textCtrl89;
	
		double								m_saveNewWeight,
												m_weightSum;
	
		int									m_weightUnits;

		SInt16								m_selectedCell,
												m_weightsSelection;
		UInt16								m_numberOutputClassesToUse;

		Boolean								m_initializedFlag,
												m_useEnhancedStatFlag;
												
};	// end "class CMClassWeightsDlg"
