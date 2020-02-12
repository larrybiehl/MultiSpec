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
//	File:						xListResultsDialog.h
//	Implementation:		xListResultsDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMDisplaySpecsDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 07/07/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"


class CMListResultsDialog : public CMDialog
{
				// Construction
	public:
		CMListResultsDialog (	 // standard constructor
				wxWindow* 							pParent = NULL,
				wxWindowID 							id = IDD_ClassifyDialog,
				const wxString& 					title =
													wxT("Set List Results Specifications"));

		~CMListResultsDialog (); // standard desctructor

		Boolean DoDialog (void);
	

				// Dialog Data

		enum { IDD = IDD_ListResults };
	
		double 								m_conversionFactor,
				 								m_thresholdPercent;
	
		int 									m_tableType,
			 									m_tableUnits;
	
		bool 									m_classSummaryFlag,
												m_diskFileFlag,
												m_fieldSummaryFlag,
												m_groupSummaryFlag,
												m_imageFlag,
												m_testFlag,
												m_textWindowFlag,
												m_thresholdResultsFlag,
												m_trainingFlag;

		 // Implementation
	protected:
				// Generated message map functions
		void OnAreaUnits (
				wxCommandEvent& 					event);
	
		void OnClassComboSelendok (
				wxCommandEvent& 					event);

		void OnClassSummary (
				wxCommandEvent& 					event);

		void OnFieldSummary (
				wxCommandEvent& 					event);

		void OnGroupSummary (
				wxCommandEvent& 					event);

		void OnImage (
				wxCommandEvent& 					event);

		void OnInitDialog (
				wxInitDialogEvent& 				event);
	
		void OnPercentUnits (
				wxCommandEvent& 					event);

		void OnSampleUnits (
				wxCommandEvent& 					event);

		void OnTest (
				wxCommandEvent& 					event);

		void OnThresholdResults (
				wxCommandEvent& 					event);

		void OnTraining (
				wxCommandEvent& 					event);
		DECLARE_EVENT_TABLE ()
	
		void CheckListAreaItems ();
	
		void CheckSummaryItems ();
	
		void CreateControls ();
	
		void HideShowUnitsItems (
				Boolean 								areaUnitsFlag);
	
		void SetListAreaItems ();
	
		void SetSummarizeByItems ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();


		wxBoxSizer							*bSizer188;
	
		wxCheckBox							*m_checkBox36,
												*m_checkBox37,
												*m_checkBox38,
												*m_checkBox39,
												*m_checkBox40,
												*m_checkBox41,
												*m_checkBox42,
												*m_checkBox43,
												*m_checkBox44;

		wxRadioButton						*m_radioBtn19,
												*m_radioBtn20,
												*m_radioBtn21,
												*m_radioBtn22,
												*m_radioBtn23;

		wxStaticText						*m_staticText60,
												*m_staticText62,
												*m_staticText177,
												*m_staticText203,
												*m_staticText204,
												*m_staticText205,
												*m_staticText206;

		wxTextCtrl							*m_textCtrl106,
												*m_textCtrl107;

		double 								m_saveAreaConversionFactor,
												m_saveThresholdPercent;

		SInt16 								m_localAreaCode,
												m_localSummaryCode,
												m_nonClusterFieldTypeCode;

		Boolean 								m_initializedFlag,
												m_projectWindowFlag;
		 
};	// end "class CMListResultsDialog"
