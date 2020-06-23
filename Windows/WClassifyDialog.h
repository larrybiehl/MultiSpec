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
//	File:						WClassifyDialog.h
//	Implementation:		WClassifyDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMClassifyDialog class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1990's
//	Revised By:				Larry L. Biehl			Date: 07/27/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "WDialog.h"


class CMClassifyDialog : public CMDialog
{
	// Construction
	public:
		CMClassifyDialog (	// standard constructor
				CWnd* 								pParent = NULL);
	
		~CMClassifyDialog ();	// standard desctructor
	
		Boolean DoDialog (void);
	
		void HideShowClassAreasItem (void);

		// Dialog Data
		//{{AFX_DATA (CMClassifyDialog)
		enum { IDD = IDD_ClassifyDialog };
	
		float									m_angleThreshold,
												m_cemThreshold,
												m_correlationThreshold,
												m_thresholdPercent;
	
		int									m_classAreaSelection,
												m_classifyListSelection,
												m_classWeightsSelection,
												m_fileNamesSelection,
												m_outputFormatCode,
												m_paletteSelection,
												m_selectImageOverlaySelection;

		UInt16								m_classifyProcedureEnteredCode;
	
		BOOL									m_createImageOverlayFlag,
												m_createProbabilityFileFlag,
												m_diskFileFlag,
												m_imageAreaFlag,
												m_testAreaFlag,
												m_thresholdResultsFlag,
												m_trainingAreaFlag,
												m_trainingAreaLOOFlag;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMClassifyDialog)
		afx_msg void OnChangeCorrelationCoefficient ();
	
		afx_msg void OnChangeCorrelationThreshold ();
	
		afx_msg void OnChangeKNNThreshold ();
	
		afx_msg void OnCloseupClassificationProcedure ();
	
		afx_msg void OnCreateProbabilityFile ();
	
		afx_msg void OnDiskFile ();
	
		afx_msg void OnDropdownClassificationProcedure ();
	
		afx_msg void OnDropdownPaletteCombo ();
	
		//afx_msg void OnEnChangeThresholdvalue2 ();
	
		afx_msg void OnFeatureTransformation ();
	
		afx_msg void OnImageArea ();
	
		afx_msg void OnImageOverlay ();
	
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnListOptions ();
	
		afx_msg void OnSelendokAreasCombo ();
	
		afx_msg void OnSelendokChannelCombo ();
	
		afx_msg void OnSelendokClassificationProcedure ();
	
		afx_msg void OnSelendokClassWeightsCombo ();
	
		afx_msg void OnSelendokDiskCombo ();
	
		afx_msg void OnSelendokImageOverlayCombo ();
	
		afx_msg void OnSelendokPaletteCombo ();
	
		afx_msg void OnSelendokTargetCombo ();
	
		afx_msg void OnTestAreas ();
	
		afx_msg void OnTextWindow ();
	
		afx_msg void OnThresholdResults ();
	
		afx_msg void OnTraining ();
	
		afx_msg void OnTrainingLOO ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
		void CheckAreaSettings (void);
	
		void CheckColumnEnd (void);
	
		void CheckColumnStart (void);
	
		void CheckLineEnd (void);
	
		void CheckLineStart (void);
	
		void CheckOutputFormatItems (void);
	
		void ToEntireImage (void);
	
		void ToSelectedImage (void);
	
	
		double								m_saveAngleThreshold,
												m_saveCEMThreshold,
												m_saveCorrelationThreshold,
												m_saveThresholdPercent;
	
		float*								m_classWeightsPtr;
		UInt16*								m_classAreaListPtr;
	
		Handle								m_targetWindowInfoHandle;
	
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
												m_SupportVectorMachineComboListItem;
	
		Boolean								m_initializedFlag,
												m_optionKeyFlag,
												m_savedLeaveOneOutFlag,
												m_saveThresholdResultsFlag,
												m_thresholdAllowedFlag,
												m_trainingFieldsExistFlag;
	
public:
	afx_msg void OnEnChangeThresholdvalue();
};	// end class CMClassifyDialog
