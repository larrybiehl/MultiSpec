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
//	File:						xListResultsOptionsDialog.h
//	Implementation:		xListResultsOptionsDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMListResultsOptionsDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 06/20/2017
//
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"


class CMListResultsOptionsDlg : public CMDialog
{
		 // Construction
	public:
				 // standard constructor
		CMListResultsOptionsDlg (
		 		wxWindow* 							pParent = NULL,
		 		wxWindowID 							id = IDD_ClassWeights,
		 		const wxString& 					title =
													wxT("Classification Results List Options"));

		void DoDialog (
				SInt16* 								listResultsTrainingCodePtr,
				SInt16* 								listResultsTestCodePtr);


				// Dialog Data

		enum { IDD = IDD_ResultsListOptions };
	
		bool 									m_testAreaPerformanceFlag,
												m_testAreaSummariesFlag,
												m_testAreasUsedFlag,
												m_testClassPerformanceFlag,
												m_trainAreaPerformanceFlag,
												m_trainAreaSummariesFlag,
												m_trainAreasUsedFlag,
												m_trainClassPerformanceFlag;

				// Implementation
	protected:
		void OnInitDialog (
				wxInitDialogEvent& 				event);
		DECLARE_EVENT_TABLE ();
	
		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	

		wxBoxSizer							*bSizer142;
		
		wxCheckBox							*m_checkBox17,
												*m_checkBox18,
												*m_checkBox19,
												*m_checkBox20,
												*m_checkBox21,
												*m_checkBox22,
												*m_checkBox23,
												*m_checkBox24;
	
		wxStaticText						*m_staticText169,
												*m_staticText170;
	
		SInt16 								m_listResultsTestCode,
												m_listResultsTrainingCode;
	
};	// end "class CMListResultsOptionsDlg"
