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
//	File:						xEvaluateCovariancesDialog.h
//	Implementation:		xEvaluateCovariancesDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMEvalCovarianceDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 11/11/2019
//
//------------------------------------------------------------------------------------

#pragma once
	     
#include "xDialog.h"


class CMEvalCovarianceDialog : public CMDialog
{
	public:
				 // standard constructor
		CMEvalCovarianceDialog (
				wxWindow* 							pParent = NULL,
				wxWindowID 							id = wxID_ANY,
				const 								wxString& title = wxT("Check Covariances"));

		~CMEvalCovarianceDialog ();		// standard desctructor
	
		Boolean DoDialog (
				EvaluateCovarianceSpecsPtr  	evaluateCovarianceSpecsPtr);

				// Dialog Data
	
		enum {IDD = IDD_EvalCovariances};
		 
			// Implementation
	protected:
		void OnChannelComboSelendok (
				wxCommandEvent&					event);

		void OnClassComboSelendok (
				wxCommandEvent&					event);

		virtual void OnInitDialog (
				wxInitDialogEvent& 				event);

		void OnTransformation (
				wxCommandEvent&					event);
		DECLARE_EVENT_TABLE ();
	
		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();


		EvaluateCovarianceSpecsPtr  	m_evaluateCovarianceSpecsPtr;

				// Dialog items
	
		wxBoxSizer							*bSizer107,
												*bSizer108,
												*bSizer175;
	
		wxButton								*m_button20,
												*m_button21;
	
		wxCheckBox							*m_checkBox14,
												*m_checkBox15,
												*m_checkBox16,
												*m_checkBox17,
												*m_checkBox18;

		wxStaticText						*m_staticText190,
												*m_staticText191;
	
		UInt16								m_numberEigenvectors;
	
		bool									m_featureTransformationFlag,
												m_listInvertedInvertedMatrixFlag,
												m_listInvertedMatrixFlag,
												m_listOriginalMatrixFlag,
												m_listOriginalXInvertedMatrixFlag;
	
		Boolean		  						m_featureTransformAllowedFlag,
										  		m_initializedFlag;
		 
};	// end "class CMEvalCovarianceDialog"
