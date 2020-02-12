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
//	File:						xClusterDialog.h
//	Implementation:		xClusterDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	Header file for the CMClassifyDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 12/04/2019
//	
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"


class CMClusterDialog : public CMDialog
{
	DECLARE_DYNAMIC_CLASS (CMClusterDialog)
	
			// Construction
	public:
		CMClusterDialog (); // standard constructor
	
		CMClusterDialog (
				wxWindow*							parent,
				wxWindowID							id = IDD_Cluster,
				const wxString&					title = wxT("Set Cluster Specifications"));
	
		~CMClusterDialog (); // standard desctructor

		Boolean DoDialog (
				Boolean								newProjectFlag);

		enum {
			IDD = IDD_Cluster
			};
	
			// Implementation
	protected:
				// Generated message map functions
		void OnAlgorithmSelection (
				wxCommandEvent&					event);

		void OnChannelComboSelendok (
				wxCommandEvent&					event);

		void OnClassifySelectedArea (
				wxCommandEvent&					event);

		void OnCreateMaskFile (
				wxCommandEvent&					event);

		void OnImageOverlay (
				wxCommandEvent&					event);

		void OnImageOverlayComboSelendok (
				wxCommandEvent&					event);

		void OnInitDialog (
				wxInitDialogEvent& 				event);

		void OnOK (
				wxCommandEvent&					event);

		void OnSelendokMaskFileDiskCombo (
				wxCommandEvent&					event);
		DECLARE_EVENT_TABLE ()
				  
		void CreateControls ();
	
		void ClassifyAreaSetting (
				bool									from,
				int									nIDC,
				int&									value);
	
		void OnAlgorithm (
				Boolean								returnFlag);
	
		void OnISODATAAlgorithm ();

		void OnSinglePassAlgorithm ();

		void UpdateClassificationItems (
				UInt16								classificationArea);

		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();


		wxString								m_classifyThresholdString;

		wxBoxSizer							*bSizer137;
	
		wxCheckBox							*m_checkBox46,
												*m_checkBox47,
												*m_checkBox48,
												*m_checkBox49;
	
		wxChoice								*m_choice18,
												*m_choice19,
												*m_choice20,
												*m_choice21;
	
		wxRadioBox							*m_algorithmSelectionRadioBox,
												*m_radioBox4;
	
		wxStaticText						*m_staticText160,
												*m_staticText174,
												*m_staticText175,
												*m_staticText176;
	
		wxTextCtrl							*m_textCtrl84;
	
		double								m_classifyThreshold,
												m_probabilityThreshold;
	
		int									m_classificationArea,
												m_clusterProcedure,
												m_maskFileFormatCode,
												m_saveStatisticsSelection,
												m_selectImageOverlaySelection,
												m_symbolSelection;

		SInt32								m_maxClassifyThreshold,
												m_transparencyValue;

		SInt16								m_channelListType;
	
		Boolean								m_clusterProcedureSetFlag,
												m_createClusterMaskFlag,
												m_createImageOverlayFlag,
												m_diskFileFlag,
												m_initializedFlag,
												m_newProjectFlag,
												m_textWindowFlag,
												m_thresholdFlag;

};	// end "class CMClusterDialog"
