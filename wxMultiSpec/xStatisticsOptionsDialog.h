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
//	File:						xStatisticsOptionsDialog.h
//	Implementation:		xStatisticsOptionsDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMStatOptionsDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 07/07/2017
//
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"


class CMStatOptionsDlg : public CMDialog
{
		 // Construction
	public:
				// standard constructor
		CMStatOptionsDlg (
				wxWindow* 			pParent = NULL,
				wxWindowID 			id = IDD_ClassifyDialog,
				const wxString& 	title = wxT("Set Project Statistics Options"));

		SInt16 DoDialog (
				SInt16* 				statCodePtr,
				Boolean* 			keepClassStatsFlagPtr,
				double* 				variancePtr,
				double* 				minLogDeterminantPtr,
				Boolean* 			useCommonCovarianceInLOOCFlagPtr);

				// Dialog Data

		enum { IDD = IDD_StatisticsOptionsDialog };

		 // Implementation
	protected:
		void OnInitDialog (
				wxInitDialogEvent& 				event);
	
		void OnMeanStd (
				wxCommandEvent& 					event);
	
		void OnMeanStdCov (
				wxCommandEvent& 					event);
	
		void OnSetZeroVariance (
				wxCommandEvent& 					event);
		DECLARE_EVENT_TABLE ()
	
		void CreateControls ();

		void SetZeroVariance (void);
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();


		wxBoxSizer									*bSizer181;
	
		wxCheckBox									*m_checkBox33,
														*m_checkBox34,
														*m_checkBox35;
	
		wxRadioButton								*m_radioBtn17,
														*m_radioBtn18;
	
		wxStaticText								*m_staticText193,
														*m_staticText194,
														*m_staticText195;
	
		wxTextCtrl									*m_textCtrl92,
														*m_textCtrl93;

		wxString 									m_minLogString,
			 											m_varienceString;
	
		double 										m_minLogDetValue,
			 											m_varianceValue;
	
		int 											m_statCode;
	
		bool 											m_classStatsOnlyFlag,
														m_setZeroVarianceFlag,
														m_useCommonCovarianceInLOOCFlag;
	
};	// end "class CMStatOptionsDlg"
