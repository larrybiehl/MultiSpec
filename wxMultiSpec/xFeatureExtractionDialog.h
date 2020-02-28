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
//	File:						xFeatureExtractionDialog.h
//	Implementation:		xFeatureExtractionDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMFeatureExtractionDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 02/24/2020
//
//------------------------------------------------------------------------------------

#pragma once
	     
#include "xDialog.h"


class CMFeatureExtractionDialog : public CMDialog
{
			// Construction
	public:
				// standard constructor
		CMFeatureExtractionDialog (
				wxWindow* 							pParent = NULL,
				wxWindowID 							id = IDD_FeatureExtraction,
				const wxString& 					title =
													wxT("Set Feature Extraction Specification"));
	
		~CMFeatureExtractionDialog ();		// standard desctructor
	
		Boolean DoDialog (
				FeatureExtractionSpecsPtr		featureExtractionSpecsPtr);

				// Dialog Data
	
		enum { IDD = IDD_FeatureExtraction };
	
			// Implementation
	protected:
		void OnChannelComboSelendok (
				wxCommandEvent&					event);

		void OnClassPairWeightsComboSelendok (
				wxCommandEvent&					event);

		void OnClassWeightsComboSelendok (
				wxCommandEvent&					event);

		void OnDecisionBoundary (
				wxCommandEvent&					event);

		void OnInitDialog (
				wxInitDialogEvent& 				event);
	
		void OnOptimizeClasses (
				wxCommandEvent&					event);

		void OnPreprocessingCloseUp (
				wxCommandEvent&					event);

		void OnPreprocessingDropDown (
				wxCommandEvent&					event);
	
		void OnPreprocessingSelendok (
				wxCommandEvent& 					event);

		void OnSpecialOptions (
				wxCommandEvent&					event);
		DECLARE_EVENT_TABLE ()
	
		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	

		wxString 							m_iclassThresString,
												m_wclassThresString,
												m_percentString,
												m_maxpixeltString,
												m_minThrestString;
	
		FeatureExtractionSpecsPtr		m_featureExtractionSpecsPtr;

		float*								m_classWeightsPtr;

		SInt16*								m_localClassPairWeightsListPtr;
	
		wxBoxSizer							*bSizer250;
	
		wxButton								*m_button65,
												*m_button66;
	
		wxCheckBox							*m_checkBox54,
												*m_checkBox55,
												*m_checkBox56;
	
		wxChoice								*m_feAlgorithmCtrl;
		#if defined multispec_wxmac
			wxChoice								*m_preprocessingCtrl;
		#endif
	
		#if defined multispec_wxlin
			wxComboBox							*m_preprocessingCtrl;
		#endif
	
		wxStaticText						*m_staticText244,
												*m_staticText245,
												*m_staticText246,
												*m_staticText247,
												*m_staticText248,
												*m_staticText249,
												*m_staticText250,
												*m_staticText251,
												*m_staticText252,
												*m_staticText253,
												*m_staticText254,
												*m_staticText255,
												*m_staticText256;
	
		wxTextCtrl							*m_textCtrl104,
												*m_textCtrl105,
												*m_textCtrl106,
												*m_textCtrl107,
												*m_textCtrl108;
	
		float									m_interclassThrehold,
												m_withinClassThreshold,
												m_optimizeThreshold;

		int									m_algorithmCode,
												m_maxPixelsPerClass,
												m_minThresholdNumber,
												m_preprocessSelection,
												m_preprocess_Saved;

		SInt16								m_localDefaultClassPairWeight;
	
		Boolean								m_listTransformationFlag,
												m_optimizeClassesFlag,
												m_specialOptionsFlag;
	
};	// end "class CMFeatureExtractionDialog"
