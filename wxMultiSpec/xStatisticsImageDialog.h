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
//	File:						xStatisticsImageDialog.h
//	Implementation:		xStatisticsImageDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	Header file for the CMStatImageDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 11/12/2019
//	
//------------------------------------------------------------------------------------

#pragma once

#include "SMultiSpec.h"
#include "xDialog.h"


class CMStatImageDialog : public CMDialog
{
	public:
				// standard constructor
		CMStatImageDialog (
				wxWindow* 							pParent = NULL,
				wxWindowID 							id = IDS_ProcessorCreateImageStat,
				const wxString& 					title =
											wxT("Set Create Statistics Images Specifications"));
	
		~CMStatImageDialog (); // standard desctructor

		Boolean DoDialog ();

		enum { IDD = IDS_ProcessorCreateImageStat };
	
		  // Implementation
	protected:
		void OnChannelComboSelendok (
				wxCommandEvent& 					event);
	
		void OnClassComboSelendok (
				wxCommandEvent& 					event);
	
		void OnClickAreaRadio (
				wxCommandEvent& 					event);
	
		void OnClickClassesRadio (
				wxCommandEvent& 					event);
	
		void OnClickIndividualRadio (
				wxCommandEvent& 					event);
	
		void OnClickOverallRadio (
				wxCommandEvent& 					event);
	
		void OnClickUserSettingRadio (
				wxCommandEvent& 					event);
	
		void OnInitDialog (
				wxInitDialogEvent& 				event);
		DECLARE_EVENT_TABLE ()

		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	
	

		SInt16								*m_featurePtr,
												*m_channelsPtr,
												*m_classPtr;

		wxBoxSizer							*bSizer292;

		wxCheckBox							*m_checkBox74;

		wxComboBox							*m_comboBox58;

		wxRadioButton						*m_radioBtn36,
												*m_radioBtn37,
												*m_radioBtn38,
												*m_radioBtn39,
												*m_radioBtn40,
												*m_radioBtn41,
												*m_radioBtn42;
	
		wxStaticBoxSizer					*m_sbSizer8;
	
		wxStaticText						*m_staticText177,
												*m_staticText312,
												*m_staticText313,
												*m_staticText315,
												*m_staticText316,
												*m_staticText317,
												*m_staticText318,
												*m_staticText319,
												*m_staticText320,
												*m_staticText283;

		wxTextCtrl							*m_textCtrl153,
												*m_textCtrl154,
												*m_textCtrl155;
	
		double								m_userMaximum,
												m_userMinimum;

		SInt16								m_areaCode,
												m_individualMinMaxCode,
												m_maximumNumberChannels,
												m_minMaxCode,
												m_perClassFieldCode,
												m_overallMinMaxCode,
												m_userMinMaxCode;

		Boolean        					m_channelsAllAvailableFlag,
												m_featureTransformationFlag;

};	// end "class CMStatImageDialog"
