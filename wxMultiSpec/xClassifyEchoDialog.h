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
//	File:						xClassifyEchoDialog.h
//	Implementation:		xClassifyEchoDialog.cpp
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
//	Revised By:				Larry L. Biehl			Date: 11/13/2019
//	
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"


class CMEchoClassifyDialog : public CMDialog 
{
			// Construction
	public:
				 // standard constructor 
		CMEchoClassifyDialog (
				wxWindow*							pParent = NULL,
				wxWindowID							id = IDD_EchoClassify,
				const wxString&					title = wxT("Specify Echo Parameters"));

		~CMEchoClassifyDialog (); // standard desctructor 

		Boolean DoDialog (
				EchoClassifierVarPtr				echoClassifierVarPtr);

				// Dialog Data

		enum {
			IDD = IDD_EchoClassify
			};

			// Implementation
	protected:
				// Generated message map functionsw
		void OnInitDialog (
				wxInitDialogEvent& 				event);
	
		void OnLogLike (
				wxCommandEvent& 					event);
				
		void OnPercent (
				wxCommandEvent& 					event);
		DECLARE_EVENT_TABLE ()
	
		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	
	
		wxString								m_cellWidthString,
												m_homoThresString,
												m_annexThresString;

		EchoClassifierVarPtr 			m_echoClassifierVarPtr;

		wxBoxSizer							*bSizer148;
			
		wxButton								*m_button33,
												*m_button34;
		
		wxCheckBox							*m_checkBox25,
												*m_checkBox26,
												*m_checkBox27;
		
		wxChoice								*m_echoAlgorithmCtrl;
		
		wxRadioButton						*m_radioBtn13,
												*m_radioBtn14;
		
		wxStaticText						*m_staticText171,
												*m_staticText172,
												*m_staticText173,
												*m_staticText174;
	
		wxTextCtrl							*m_textCtrl72,
												*m_textCtrl73,
												*m_textCtrl74;
	
		double								m_annexationThreshold,
												m_homogeneityThreshold,
												m_maxPhase1,
												m_minPhase1,
												m_savedPhase1LogLikelihoodThreshold,
												m_savedPhase1PercentThreshold;
	
		int									m_echoAlgorithmProcedure,
												m_homogeneousThresholdType;
	
		long									m_cellWidth;
	
		bool									m_combineLikeFieldsFlag,
												m_createHomogeneousFilesFlag,
												m_mixCellsFlag;

		Boolean								m_initializedFlag;
	
};	// end "class CMEchoClassifyDialog"
