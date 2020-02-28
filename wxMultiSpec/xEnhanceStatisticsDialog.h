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
//	File:						xEnhanceStatisticsDialog.h
//	Implementation:		xEnhanceStatisticsDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMEnhanceStatisticsDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 02/24/2020
//
//------------------------------------------------------------------------------------

#pragma once

#include "SMultiSpec.h"

#include "xDialog.h"

#include "wx/wx.h"
#include "wx/bmpcbox.h"


class CMEnhanceStatisticsDialog : public CMDialog
{
			// Construction
	public:
				// standard constructor
		CMEnhanceStatisticsDialog (
				wxWindow* 							parent,
				wxWindowID 							id = wxID_ANY,
				const wxString& 					title =
												wxT("Set Statistics Enhancement Specifications"),
				const wxPoint& 					pos = wxDefaultPosition,
				const wxSize& 						size = /*wxDefaultSize*/wxSize (200, 200),
				long 									style = wxDEFAULT_DIALOG_STYLE);

		~CMEnhanceStatisticsDialog (void);

		SInt16  DoDialog (
				StatEnhanceSpecsPtr   			statEnhanceSpecsPtr);


		enum {IDD = IDD_EnhanceStatistics};

			// Implementation
	protected:
		void OnInitDialog (
				wxInitDialogEvent& 				event);
	
		void OnWeightLabeledSamples (
				wxCommandEvent&					event);

		void OnUseEnhancedStats (
				wxCommandEvent&					event);

		void CheckStatColumnEnd (
				wxCommandEvent&					event);

		void CheckStatColumnStart (
				wxCommandEvent&					event);

		void CheckStatLineEnd (
				wxCommandEvent&					event);

		void CheckStatLineStart (
				wxCommandEvent&					event);

		void ToStatEntireImage (
				wxCommandEvent&					event);

		void ToStatSelectedImage (
				wxCommandEvent&					event);

		void CheckStatLineInterval (
				wxCommandEvent&					event);

		void CheckStatColumnInterval (
				wxCommandEvent&					event);

		void OnSelendokStatClassCombo (
				wxCommandEvent&					event);

		void OnSelchangeHardThresholdCombo (
				wxCommandEvent&					event);

		void OnSelchangeSoftThresholdCombo (
				wxCommandEvent&					event);

		void OnChangeHardPercentThreshold (
				wxCommandEvent&					event);

		void OnChangeSoftPercentThreshold (
				wxCommandEvent&					event);

		void OnChangeHardChiChiThreshold (
				wxCommandEvent&					event);

		void OnChangeSoftChiChiThreshold (
				wxCommandEvent&					event);

		void OnClassWeightsComboSelendok (
				wxCommandEvent&					event);
		DECLARE_EVENT_TABLE ();
	
		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	
	
		wxString 							m_hardchiThresString,
					 							m_hardpercentThresString,
					 							m_iterlengthString,
					 							m_loglikeString,
					 							m_maxNumberString,
												m_softChiThresString,
					 							m_softpercentThresString,
					 							m_weightString;
	
		float*								m_classWeightsPtr;
	
		StatEnhanceSpecsPtr				m_statEnhanceSpecsPtr;
	
		wxBoxSizer							*bSizer119;

		wxCheckBox							*m_checkBox58,
												*m_checkBox59;
	
		wxChoice								*m_hardThresholdCtrl,
												*m_softThresholdCtrl;

		wxFlexGridSizer					*fgSizer15;
	
		wxStaticText						*m_staticText188,
												*m_staticText249,
												*m_staticText250,
												*m_staticText263,
												*m_staticText278,
												*m_staticText279,
												*m_staticText280,
												*m_staticText281,
												*m_staticText307,
												*m_staticText3071,
												*m_staticText30711,
												*m_staticText30712,
												*m_staticText307121,
												*m_staticText3071211,
												*m_staticText308,
												*m_staticText3081,
												*m_staticText317,
												*m_staticText318,
												*m_staticText319,
												*m_staticText326,
												*m_staticText327,
												*m_textCtrl170,
												*m_textCtrl171;
	
		wxTextCtrl							*m_textCtrl180,
												*m_textCtrl181,
												*m_textCtrl182,
												*m_textCtrl184,
												*m_textCtrl185,
												*m_textCtrl1851,
												*m_textCtrl18511,
												*m_textCtrl191,
												*m_textCtrl192,
												*m_textCtrl193,
												*m_textCtrl194,
												*m_textCtrl195,
												*m_textCtrl196,
												*m_textCtrl204;
	
		double								m_hardChiChiThreshold,
												m_hardPercentThreshold,
												m_labelWeight,
												m_logLikeStopPercent,
												m_softChiChiThreshold,
												m_softPercentThreshold;
	
		int        							m_hardThresholdCode,
												m_softThresholdCode,
			         						saveSoftThresholdSelection;
	
		SInt32								m_iterationMax,
												m_iterationStopLength;
	
		SInt16                     	m_classWeightSet;
	
		Boolean                     	m_initializedFlag,
												m_updatingThresholdItemsFlag,
												m_useEnhancedStatisticsFlag,
												m_weightLabeledFlag;
	
};	// end "class CMEnhanceStatisticsDialog"
