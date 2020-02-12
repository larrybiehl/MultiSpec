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
//	File:						xProjectionPursuitDialog.h
//	Implementation:		xProjectionPursuitDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMProjectionPursuitDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 06/21/2017
//
//------------------------------------------------------------------------------------

#pragma once
	     
#include "xDialog.h"


class CMProjectionPursuitDialog : public CMDialog
{
	public:
		CMProjectionPursuitDialog (	 // standard constructor
				wxWindow* 							pParent = NULL,
				wxWindowID 							id = IDD_ProjectionPursuit,
				const wxString& 					title =
															wxT("Set Preprocessing Specification"));
	
		virtual ~CMProjectionPursuitDialog ();  // standard destructor

		Boolean DoDialog (
				ProjectionPursuitSpecsPtr		projectionPursuitSpecsPtr);


		enum { IDD = IDD_ProjectionPursuit };

	protected:
		void OnCheckValues (
				wxCommandEvent& 					event);
	
		void OnBnClickedAlgorithm (
				wxCommandEvent& 					event);
	
		void OnBnClickedNumericaloptimization (
				wxCommandEvent& 					event);
	
		void OnBnClickedInitialBandGrouping (
				wxCommandEvent& 					event);
	
		void OnBnClickedBandGroupingMethod (
				wxCommandEvent& 					event);
	
		void OnInitDialog (
				wxInitDialogEvent& 				event);
		DECLARE_EVENT_TABLE ()
	
		void CreateControls ();
		void CheckValues (
				int 									controlID,
				SInt16 								minValue,
				SInt16 								maxValue,
				SInt16 								flag);
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	
	
		ProjectionPursuitSpecsPtr		m_projectionPursuitSpecsPtr;
	
		wxBoxSizer							*bSizer267,
												*bSizer271;
	
		wxCheckBox							*m_checkBox57;

		wxRadioButton						*m_radioBtn29,
												*m_radioBtn30,
												*m_radioBtn31,
												*m_radioBtn32,
												*m_radioBtn33,
												*m_radioBtn34,
												*m_radioBtn35;
	
		wxStaticText						*m_staticText257,
												*m_staticText258,
												*m_staticText259,
												*m_staticText260,
												*m_staticText261,
												*m_staticText262,
												*m_staticText263,
												*m_staticText264;
	
		wxTextCtrl							*m_textCtrl109,
												*m_textCtrl110,
												*m_textCtrl111,
												*m_textCtrl112,
												*m_textCtrl113,
												*m_textCtrl114;
	
		double								m_bottomUpThreshold,
												m_optimizeThreshold,
												m_topDownThreshold;
	
		int									m_algorithmCode,
												m_bothOddChoicesNumberFeatures,
												m_firstStageMethod,
												m_initialGroupingCode,
												m_maximumNumberFeatures;
	
		short									m_finalNumberFeatures,
												m_initialNumberFeatures;
	
		bool									m_numericalOptimizationFlag;
		
};	// end "class CMProjectionPursuitDialog"
