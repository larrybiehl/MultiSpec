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
//	File:						xClassifyDialog.h
//	Implementation:		xClassifyDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMClassifyDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 11/09/2019
//	
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"


class CMClassifyDialog : public CMDialog
{
		 // Construction
	public:
		CMClassifyDialog (
				wxWindow*							pParent = NULL,
				wxWindowID							id = IDD_ClassifyDialog,
				const wxString&					title =
														wxT("Set Classification Specifications"));

		~CMClassifyDialog (); // standard desctructor 

		void AdjustDlgLayout ();
	
		Boolean DoDialog (void);
	
		void HideShowClassAreasItem (void);
	
		void OnStnClickedStartendinterval ();
		
				// Dialog Data

		enum {
		  IDD = IDD_ClassifyDialog
			};

		 // Implementation
	protected:
		void CheckColumnEnd (
				wxCommandEvent&					event);

		void CheckColumnStart (
				wxCommandEvent&					event);

		void CheckLineEnd (
				wxCommandEvent&					event);

		void CheckLineStart (
				wxCommandEvent&					event);


		void OnActivate (wxActivateEvent& event);
		void OnAreasComboCloseUp (
				wxCommandEvent&					event);

   	void OnAreasComboDropDown (
				wxCommandEvent&					event);

		void OnAreasComboSelendok (
				wxCommandEvent&					event);

		void OnChangeCEMThreshold (
				wxCommandEvent&					event);

		void OnChangeCorrelationCoefficient (
				wxCommandEvent&					event);

		void OnChangeCorrelationThreshold (
				wxCommandEvent&					event);

		void OnChangeKNNThreshold (
				wxCommandEvent&					event);

		void OnChangeThreshold (
				wxCommandEvent&					event);

		void OnChannelComboSelendok (
				wxCommandEvent&					event);

		void OnClassificationProcedureCloseUp (
				wxCommandEvent&					event);

		void OnClassificationProcedureDropDown (
				wxCommandEvent&					event);

		void OnClassificationProcedureSelendok (
				wxCommandEvent&					event);

		void OnClassWeightsComboSelendok (
				wxCommandEvent&					event);

		void OnCreateProbabilityFile (
				wxCommandEvent&					event);

		void OnDiskComboSelendok (
				wxCommandEvent&					event);

		void OnDiskFile (
				wxCommandEvent&					event);

		void OnFeatureTransformation (
				wxCommandEvent&					event);

		void OnImageArea (
				wxCommandEvent&					event);

		void OnImageOverlay (
				wxCommandEvent&					event);

		void OnImageOverlayComboSelendok (
				wxCommandEvent&					event);

		void OnInitDialog (
				wxInitDialogEvent& 				event);
	
		void OnLButtonDown (
				wxMouseEvent& 						event);
	
		void OnListOptions (
				wxCommandEvent&					event);

		void OnPaletteComboCloseUp (
				wxCommandEvent&					event);

		void OnPaletteComboDropDown (
				wxCommandEvent&					event);

		void OnPaletteComboSelendok (
				wxCommandEvent&					event);

		void OnTargetComboSelendok (
				wxCommandEvent&					event);

		void OnTestAreas (
				wxCommandEvent&					event);

		void OnTextWindow (
				wxCommandEvent&					event);

		void OnThresholdResults (
				wxCommandEvent&					event);

		void OnTraining (
				wxCommandEvent&					event);

		void OnTrainingLOO (
				wxCommandEvent&					event);
				
		void ToEntireImage (
				wxCommandEvent&					event);

		void ToSelectedImage (
				wxCommandEvent&					event);
		DECLARE_EVENT_TABLE ()
				
		void CheckAreaSettings ();
	
		void CheckOutputFormatItems ();
	
		void CreateControls ();

		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	

		wxString								m_cemThresholdString,
												m_corrThresholdString,
												m_knnThresholdString,
												m_ThresholdString;

		float*								m_classWeightsPtr;
	
		UInt16*								m_classAreaListPtr;
	
		wxBoxSizer 							*bSizer110,
												*bSizer122,
												*bSizer123,
												*bSizer125,
												*bSizer126,
												*bSizer127,
												*bSizer128,
												*bSizer129,
												*bSizer114;

		wxButton 							*m_button23,
												*m_button24,
												*m_button25;

		wxCheckBox 							*m_checkBox8,
												*m_checkBox9,
												*m_checkBox10,
												*m_checkBox11,
												*m_checkBox12,
												*m_checkBox13,
												*m_checkBox14,
												*m_checkBox15,
												*m_checkBox16;
	
		wxChoice								*m_fileFormatCtrl,
												*m_overlayCtrl,
												*m_symbolCtrl,
												*m_targetCtrl;

		wxFlexGridSizer					*bFlexGrid110;
		
		wxStaticText						*m_staticText60,
												*m_staticText62,
												*m_staticText145,
												*m_staticText146,
												*m_staticText147,
												*m_staticText148,
												*m_staticText149,
												*m_staticText175,
												*m_staticText176,
												*m_staticText177,
												*m_staticText178,
												*m_staticText179,
												*m_staticText180,
												*m_staticText182,
												*m_staticText183,
												*m_staticText184,
												*m_staticText185,
												*m_staticText186,
												*m_staticText187,
												*m_staticText188;
	
		wxTextCtrl							*m_textCtrl85,
												*m_textCtrl86,
												*m_textCtrl87,
												*m_textCtrl88;
	
		#if defined multispec_wxlin
			wxComboBox 							*m_paletteCtrl;
			wxComboBox 							*m_procedureCtrl;
		#endif
		#if defined multispec_wxmac
			wxChoice 							*m_paletteCtrl;
			wxChoice 							*m_procedureCtrl;
		#endif

	
		double								m_angleThreshold,
												m_cemThreshold,
												m_correlationThreshold,
												m_saveAngleThreshold,
												m_saveCEMThreshold,
												m_saveCorrelationThreshold,
												m_saveThresholdPercent,
												m_thresholdPercent;

		Handle								m_targetWindowInfoHandle;
	
		int									m_classAreaSelection,
												m_classifyListSelection,
												m_fileNamesSelection,
												m_outputFormatCode,
												m_paletteSelection,
												m_selectImageOverlaySelection;
	
		long									m_knnThreshold;

		UInt32								m_localNumberClassAreas;

		SInt16								m_classificationProcedure,
												m_correlationComboListItem,
												m_covarianceEstimate,
												m_kNearestNeighborComboListItem,
												m_listResultsTestCode,
												m_listResultsTrainingCode,
												m_nearestNeighborKValue,
												m_outputAsciiCode,
												m_parallelPipedCode,
												m_saveKNNThreshold,
												m_supportVectorMachineComboListItem;
	
		UInt16								m_classifyProcedureEnteredCode;

		Boolean								m_createImageOverlayFlag,
												m_createProbabilityFileFlag,
												m_diskFileFlag,
												m_imageAreaFlag,
												m_initializedFlag,
												m_optionKeyFlag,
												m_savedLeaveOneOutFlag,
												m_saveThresholdResultsFlag,
												m_testAreaFlag,
												m_thresholdAllowedFlag,
												m_thresholdResultsFlag,
												m_trainingAreaFlag,
												m_trainingAreaLOOFlag,
												m_trainingFieldsExistFlag;
		
};	// end "class CMClassifyDialog"
