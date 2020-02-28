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
//	File:						xBiplotDialog.h : CMBiPlotDialog definition
//	Implementation:		xBiplotDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	Header file for the CMBiPlotDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2013
//	Revised By:				Larry L. Biehl			Date: 11/11/2019
//	
//------------------------------------------------------------------------------------

#pragma once

#include "SMultiSpec.h"
#include "xDialog.h"



class CMBiPlotDialog : public CMDialog 
{
    DECLARE_DYNAMIC_CLASS (CMBiPlotDialog)
    
	public:
				 // standard constructor
		CMBiPlotDialog (
				wxWindow*							pParent = NULL,
				wxWindowID 							id = IDS_ProcessorBiPlot,
				const 								wxString& title =
																	wxT("Set Biplot Specifications"));
		~CMBiPlotDialog (); // standard desctructor
		  
		Boolean DoDialog (void);
	
	
		enum {IDD = IDS_ProcessorBiPlot};
	
	protected:
		void OnChangeHorizontalAxis (
				wxCommandEvent&					event);
	
		void OnChangeVerticalAxis (
				wxCommandEvent&					event);
	
		void OnCreateNewWindowFlag (
				wxCommandEvent&					event);
	
		void OnDisplayPixelAsSymbol (
				wxCommandEvent&					event);
	
		void OnFeatureTranformation (
				wxCommandEvent&					event);
	
		void OnImageArea (
				wxCommandEvent&					event);
	
		void OnInitDialog (
				wxInitDialogEvent& 				event);
	
		void OnOutlineClassAsEllipse (
				wxCommandEvent&					event);
	
		void OnClassWeightsComboSelendok (
				wxCommandEvent&					event);
	
		void OnTestArea (
				wxCommandEvent&					event);
	
		void OnThresholdPixelFlag (
				wxCommandEvent&					event);
	
		void OnTrainArea (
				wxCommandEvent&					event);
		DECLARE_EVENT_TABLE ()

		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
		

		wxBoxSizer							*bSizer278,
												*bSizer282;
					
		wxButton								*m_button69,
												*m_button70;
		
		wxCheckBox							*m_checkBox58,
												*m_checkBox59,
												*m_checkBox60,
												*m_checkBox61,
												*m_checkBox62,
												*m_checkBox63,
												*m_checkBox64,
												*m_checkBox65;
	
		wxComboBox							*m_comboBox52,
												*m_comboBox53;
		
		wxStaticText						*m_staticText26,
												*m_staticText177,
												*m_staticText265,
												*m_staticText266,
												*m_staticText267,
												*m_staticText268,
												*m_staticText269,
												*m_staticText270,
												*m_staticText271,
												*m_staticText278,
												*m_staticText279,
												*m_staticText280,
												*m_staticText281,
												*m_staticText282,
												*m_staticText283,
												*m_staticText284,
												*m_staticText2811;
	
		wxTextCtrl							*m_textCtrl115,
												*m_textCtrl116,
												*m_textCtrl129;
	
		float*								m_classWeightsPtr;

		double								m_thresholdPercent,
												m_saveThresholdPercent;

		SInt32								m_maxChannelFeatureNum;
	
		SInt16								m_displayPixelCode,
												m_entireIconItem,
												m_outlineClassCode,
												m_plotDataCode,
												m_newXAxisFeature,
												m_newYAxisFeature;
	
		Boolean								m_checkChannelStatisticsFlag,
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

};	// end "class CMBiPlotDialog"
