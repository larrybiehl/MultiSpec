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
//	File:						WBiPlotDialog.h
//	Implementation:		WBiPlotDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMBiPlotDialog class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1990's
//	Revised By:				Larry L. Biehl			Date: 05/22/2017
//	
//------------------------------------------------------------------------------------

#pragma once

#include "WDialog.h"


class CMBiPlotDialog : public CMDialog
{
	public:
		CMBiPlotDialog (	  // standard constructor
				CWnd* 								pParent = NULL);
	
		virtual ~CMBiPlotDialog ();

		BOOL DoDialog (void);

		// Dialog Data
	
		enum { IDD = IDD_BiPlot };

		float*								m_classWeightsPtr;

		double								m_thresholdPercent,
												m_saveThresholdPercent;

		int									m_classWeightsSelection;

		UINT									m_maxChannelFeatureNum,
												m_newXAxisFeature,
												m_newYAxisFeature;

		SInt16								m_displayPixelCode,
												m_entireIconItem,
												m_outlineClassCode,
												m_plotDataCode;

		BOOL									m_checkChannelStatisticsFlag,
												m_checkClassesPopUpFlag,
												m_checkFeatureTransformFlag,
												m_createNewGraphWindowFlag,
												m_featureTransformAllowedFlag,
												m_featureTransformationFlag,
												m_createNewGraphicsWindowFlag,
												m_thresholdFlag,
												m_trainingAreaFlag,
												m_testFlag,
												m_imageAreaFlag;

		BOOL									mb_displayPixelCode,
												mb_outlineClassCode;

	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);    // DDX/DDV support
	
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnChangeVerticalAxis ();
	
		afx_msg void OnChangeHorizontalAxis ();
	
		afx_msg void OnCreateNewWindowFlag ();
	
		afx_msg void OnDisplayPixelAsSymbol ();
	
		afx_msg void OnEnChangeHorizontalchannel ();
	
		afx_msg void OnFeatureTranformation ();
	
		afx_msg void OnImageArea ();
	
		afx_msg void OnOutlineClassAsEllipse ();
	
		afx_msg void OnSelendokClassCombo ();
	
		afx_msg void OnSelendokClassWeightsCombo ();
	
		afx_msg void OnTestArea ();
	
		afx_msg void OnThresholdPixelFlag ();
	
		afx_msg void OnTrainArea ();
		DECLARE_MESSAGE_MAP ()
	
		Boolean								m_initializedFlag;
	
};	// end class CMBiPlotDialog
