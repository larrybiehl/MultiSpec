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
//	File:						WEnhanceStatisticsDialog.h
//	Implementation:		WEnhanceStatisticsDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMEnhanceStatisticsDialog class.
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/05/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "WDialog.h"


class CMEnhanceStatisticsDialog : public CMDialog
{
	// Construction
	public:
		CMEnhanceStatisticsDialog (	// standard constructor
				CWnd* 								pParent = NULL);

		~CMEnhanceStatisticsDialog (void);

		SInt16 DoDialog (
				StatEnhanceSpecsPtr				statEnhanceSpecsPtr);

		// Dialog Data
		//{{AFX_DATA (CMEnhanceStatisticsDialog)
		enum { IDD = IDD_EnhanceStatistics };
	
		double								m_hardChiChiThreshold,
												m_hardPercentThreshold,
												m_labelWeight,
												m_logLikeStopPercent,
												m_softChiChiThreshold,
												m_softPercentThreshold;
	
		int									m_hardThresholdCode,
												m_softThresholdCode,
												m_weightsSelection;
	
		long									m_iterationMax,
												m_iterationStopLength;
	
		BOOL									m_useEnhancedStatisticsFlag,
												m_weightLabeledFlag;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);    // DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMEnhanceStatisticsDialog)
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnChangeHardChiChiThreshold ();
	
		afx_msg void OnChangeHardPercentThreshold ();
	
		afx_msg void OnChangeSoftChiChiThreshold ();
	
		afx_msg void OnChangeSoftPercentThreshold ();
	
		afx_msg void OnSelchangeHardThresholdCombo ();
	
		afx_msg void OnSelchangeSoftThresholdCombo ();
	
		afx_msg void OnSelendokClassWeightsCombo ();
	
		afx_msg void OnUseEnhancedStats ();
	
		afx_msg void OnWeightLabeledSamples ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
		void CheckColumnEnd (void);

		void CheckColumnInterval (void);
	
		void CheckColumnStart (void);
	
		void CheckLineEnd (void);

		void CheckLineInterval (void);
	
		void CheckLineStart (void);
	
		void OnSelendokClassCombo (void);
	
		void ToEntireImage (void);
	
		void ToSelectedImage (void);
	

		float*								m_classWeightsPtr;
		StatEnhanceSpecsPtr				m_statEnhanceSpecsPtr;
	
		SInt16								m_classWeightSet;
	
		Boolean								m_initializedFlag,
												m_updatingThresholdItemsFlag;
	
};	// end class CMEnhanceStatisticsDialog
