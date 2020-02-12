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
//	File:						xClassifyCorrelationDialog.h
//	Implementation:		xClassifyCorrelationDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMCorrelationClassifyDialog class
//
//	Written By:				Wei-Kang Hsu			Date: 04/09/2015
//	Revised By:				Larry L. Biehl			Date: 11/14/2019
//	
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"


class CMCorrelationClassifyDialog : public CMDialog 
{
	public:
		CMCorrelationClassifyDialog (
				wxWindow*							parent = NULL,
				wxWindowID							id = IDD_SAMParameters,
				const wxString&					title =
												wxT("Specify Correlation Classifier Parameters"));

		Boolean DoDialog (
				SInt16*								covarianceEstimatePtr);

			// Implementation
	protected:
		void OnCovarianceComboSelendok (
				wxCommandEvent& 					event);
	
		void OnInitDialog (
				wxInitDialogEvent&				event);
		DECLARE_EVENT_TABLE ()
	
		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	
			
		wxBoxSizer*							bSizer166;
	
		wxButton*							m_button37,
												m_button38;
	
		wxChoice*							m_covarianceCtrl;
	
		wxStaticText*						m_staticText183;

		int 									m_covarianceEstimate;

		bool									m_initializedFlag;
		 
};	// end "class CMCorrelationClassifyDialog"
