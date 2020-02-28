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
//	File:						xEvaluateTransformationDialog.h
//	Implementation:		xEvaluateTransformationDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMEvalTransformDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 02/24/2020
//
//------------------------------------------------------------------------------------

#pragma once
	     
#include "xDialog.h"


class CMEvalTransformDialog : public CMDialog
{
			// Construction
	public:
		CMEvalTransformDialog (  // standard constructor
		 		wxWindow* 							pParent = NULL,
				wxWindowID 							id = wxID_ANY,
		 		const wxString& 					title = wxT("Check Transformation Matrix"));
	
		~CMEvalTransformDialog ();		// standard desctructor
		 
		Boolean DoDialog (
		 		EvaluateTransformSpecsPtr 		evaluateTransformSpecsPtr);

		 
				// Dialog Data
		enum { IDD = IDD_EvalTransform };
	
			// Implementation
	protected:
		void OnCheckTransform (
				wxCommandEvent& 					event);
	
		void OnInitDialog (
				wxInitDialogEvent& 				event);
		DECLARE_EVENT_TABLE ();

		void CreateControls ();

		void SetListCheckTransform (
				Boolean 								checkTransformFlag);

		bool TransferDataFromWindow ();

		bool TransferDataToWindow ();

		EvaluateTransformSpecsPtr   	m_evaluateTransformSpecsPtr;

				// Dialog items
	
		wxBoxSizer							*bSizer107,
												*bSizer108,
												*bSizer175;
	
		wxButton								*m_button20,
												*m_button21;
	
		wxCheckBox							*m_checkBox14,
												*m_checkBox15,
												*m_checkBox16,
												*m_checkBox23,
												*m_checkBox32;
	
		wxStaticText						*m_staticText196;
	
		Boolean								m_checkTransformationFlag,
												m_initializedFlag,
												m_listCheckMatrixFlag,
												m_listEigenvaluesFlag,
												m_listOriginalMatrixFlag;
	
};	// end "class CMEvalTransformDialog"
