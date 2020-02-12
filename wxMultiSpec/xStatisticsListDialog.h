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
//	File:						xStatisticsListDialog.h
//	Implementation:		xStatisticsListDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMListStatsDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 11/15/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "SMultiSpec.h"

#include "xDialog.h"
#include "xImageView.h"
#include "wx/wx.h"


class CMListStatsDialog : public CMDialog
{
		 // Construction
	public:
				// standard constructor
		CMListStatsDialog (
		 		wxWindow* 							pParent = NULL,
		 		wxWindowID 							id = wxID_ANY,
		 		const wxString& 					title = wxT("List Stats"));
	
		Boolean DoDialog (
				SInt16 								statsWindowMode);

			// Dialog Data
	
		enum { IDD = IDD_ListStats };

		 		// Implementation
	protected:
		 		// Generated message map functions
	
		void OnClasses (
				wxCommandEvent& 					event);
	
		void OnFields (
				wxCommandEvent& 					event);
	
		void OnInitDialog (
				wxInitDialogEvent& 				event);
		DECLARE_EVENT_TABLE ()

		void CheckListFieldClassSettings (void);
	
		void CreateControls ();

		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();

	
		wxCheckBox							*m_checkBox46,
												*m_checkBox47,
												*m_checkBox48,
												*m_checkBox49,
												*m_classCheckBox,
												*m_fieldCheckBox;

		wxStaticText						*m_staticText258,
												*m_staticText259,
												*m_staticText264,
												*m_staticText265,
												*m_staticText266;
	
		wxTextCtrl							*m_textCtrl125,
												*m_textCtrl126;
	
		wxString 							m_covPrecisionString,
												m_meanPrecisionString;
	
		SInt16 								m_statsWindowMode;
	
		UInt32								m_listCovCorPrecision,
												m_listMeanStdPrecision;
	
		Boolean 								m_featureTransformationFlag,
												m_listClassFlag,
												m_listCorrelationFlag,
												m_listCovarianceFlag,
												m_listFieldFlag,
												m_listMeansStdDevFlag;
	
};	// end "class CMListStatsDialog"
