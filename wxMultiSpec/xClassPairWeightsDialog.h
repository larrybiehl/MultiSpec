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
//	File:						xClassPairWeightsDialog.h
//	Implementation:		xClassPairWeightsDialog.cpp
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
//	Revised By:				Larry L. Biehl			Date: 03/13/2018
//	
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"


class CMClassPairWeightDlg : public CMDialog
{
			// Construction
	public:
				 // standard constructor
		CMClassPairWeightDlg (
				wxWindow*							pParent = NULL,
				wxWindowID							id = IDD_ClassPairWeights,
				const wxString&					title = wxT("Specify Class Pair Weights"));

		SInt16 DoDialog (
				UInt16								numberOfClassesToUse,
				SInt16**								weightsListPtrPtr,
				SInt16								interClassWeightsSelection,
				SInt16*								defaultClassPairWeightPtr);

		enum {
			IDD = IDD_ClassPairWeights
			};

			// Implementation
	protected:
				// Generated message map functions
	
		void OnInitDialog (
				wxInitDialogEvent& 				event);
	
		void OnHelpButton (
				wxCommandEvent& 					event);
	
		void OnSelchangeClassList (
				wxCommandEvent& 					event);

		void OnAddButton (
				wxCommandEvent& 					event);

		void OnChangeNewWeight (
				wxCommandEvent& 					event);

		void OnRemoveButton (
				wxCommandEvent& 					event);

		void OnSelchangeClassPairWeightList (
				wxCommandEvent& 					event);

		void OnChangeDefaultWeight (
				wxCommandEvent& 					event);
		DECLARE_EVENT_TABLE ()
		
		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	
	
		wxFont								m_weightsListFont;

		SInt16**								m_weightsListPtrPtr;

		SInt16*								m_defaultClassPairWeightPtr;

		wxBoxSizer							*bSizer240;
	
		wxButton								*m_button51,
												*m_button52,
												*m_button53,
												*m_button54,
												*m_button55;
	
		wxListBox							*m_listBox2,
												*m_listBox3;
	
		wxStaticText						*m_staticText239,
												*m_staticText240,
												*m_staticText241,
												*m_staticText242;
	
		wxTextCtrl							*m_textCtrl127,
												*m_textCtrl128;
	
		long									m_localDefaultClassPairWeight,
												m_newClassPairWeight;

		SInt16								m_classPairWeightsSelection,
												m_selectedClassCell,
												m_selectedWeightGroupCell;

		UInt16								m_numberOfClassesToUse;

		Boolean								m_initializedFlag;
					
};	// end "class CMClassPairWeightDlg"
