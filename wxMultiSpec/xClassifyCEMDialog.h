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
//	File:						xClassifyCEMDialog.h : CMCEMClassifyDialog Definition
//	Implementation:		xClassifyCEMDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMCEMClassifyDialog class
//
//	Written By:				Wei-Kang Hsu			Date: 04/09/2015
//	Revised By:				Larry L. Biehl			Date: 11/13/2019
//	
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"


class CMCEMClassifyDialog : public CMDialog
{
			// Construction
	public:
		CMCEMClassifyDialog (
				wxWindow*							pParent = NULL,
				wxWindowID							id = IDD_ClassifyDialog,
				const wxString&					title = wxT("Specify CEM Parameters"));

		Boolean DoDialog (
				CEMParametersPtr					cemParametersPtr,
				UInt16*								classifyProcedureEnteredCodePtr);

			// Implementation
	protected:
		void OnInitDialog (
				wxInitDialogEvent& 				event);
	
		void OnClassComboSelendok (
				wxCommandEvent& 					event);
	
		void OnUseClasses (
				wxCommandEvent& 					event);
	
		void OnUseSelectedArea (
				wxCommandEvent& 					event);
		DECLARE_EVENT_TABLE ()

		void BackgroundCorrelationSettings (
				Boolean 								areaCode);
	
		void CreateControls ();
	
		bool TransferDataToWindow ();
	
		bool TransferDataFromWindow ();


		CEMParametersPtr					m_cemParametersPtr;

		SInt16								*m_correlationMatrixClassPtr,
												*m_localClassAreaPtr;
		
		wxBoxSizer							*bSizer156;
		
		wxButton								*m_button35,
												*m_button36;
		
		wxRadioButton						*m_radioBtn15,
												*m_radioBtn16;
		
		wxStaticText						*m_staticText60,
												*m_staticText62,
												*m_staticText175,
												*m_staticText177;

		int									m_correlationMatrixCode;

		UInt32								m_localNumberClassAreas;
	
		bool									m_testFieldsFlag,
												m_trainingFieldsFlag;

		Boolean								m_initializedFlag;

}; // end "class CMCEMClassifyDialog"
