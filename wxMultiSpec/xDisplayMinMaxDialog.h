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
//	File:						xDisplayMinMaxDialog.h : CMDisplayMinMaxDialog Definition
//	Implementation:		xDisplayMinMaxDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMDisplayMinMaxDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2013
//	Revised By:				Larry L. Biehl			Date: 06/20/2017
//
//------------------------------------------------------------------------------------

#pragma once

#include "SMultiSpec.h"
#include "wx/wx.h"
#include "xDialog.h"


class CMDisplayMinMaxDialog : public CMDialog
{
		 // Construction
	public:
		CMDisplayMinMaxDialog ();
	
		CMDisplayMinMaxDialog (
				wxWindow* 							pParent = NULL,
				wxWindowID 							id = wxID_ANY,
				const wxString& 					title = wxT("Set Enhancement Specifications")); // standard constructor
	
		~CMDisplayMinMaxDialog (); // standard desctructor

		Boolean DoDialog (
				SInt16* 								channelsPtr,
				SInt16 								rgbColors,
				SInt16 								displayType,
				SInt16 								numberLevels,
				SInt16* 								percentTailsClippedPtr,
				SInt16* 								minMaxSelectionPtr,
				double* 								minMaxValuesPtr);
	
		enum {
		  IDD = IDD_DisplayEnhancement
			};


	protected:
				// Generated message map functions
				
		void OnInitDialog (
				wxInitDialogEvent& 				event);
	
		void OnChannelMinMax (
				wxCommandEvent&					event);

		void OnChangePercentClipped (
				wxCommandEvent&					event);

		void OnChangeMinMaxBlue (
				wxCommandEvent&					event);

		void OnChangeMinMaxGreen (
				wxCommandEvent&					event);

		void OnChangeMinMaxRed (
				wxCommandEvent&					event);
		DECLARE_EVENT_TABLE ()
		
		void CheckNewMinMaxValue (
				int									id,
				SInt16								channelIndex);
	
		void CreateControls ();
	
		SInt16 GetNumberDecimalDigits (
				SInt16 								channelIndex);
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	
		void UpdateMinMaxOptions ();
	
		int VerifyDataRangeStartEndValues (
				double								minValue,
				double								maxValue,
				int									minID);
	

		wxString 							m_percentClippedString;
	
		double* 								m_minMaxValuesPtr;

		HistogramSpecsPtr 				m_histogramSpecsPtr;
	
		HistogramSummaryPtr 				m_histogramSummaryPtr;

		SInt16*								m_channelsPtr;

		wxBoxSizer							*bSizer1;

		wxButton								*m_button1,
												*m_button2;

		wxRadioButton 						*m_radioBtn1,
												*m_radioBtn2,
												*m_radioBtn3,
												*m_radioBtn4;
	
		wxStaticText 						*m_staticText3,
												*m_staticText5,
												*m_staticText7,
												*m_staticText9,
												*m_staticText12,
												*m_staticText23,
												*m_staticText24,
												*m_staticText25,
												*m_staticText26,
												*m_staticText27,
												*m_staticText21,
												*m_staticText28,
												*m_staticText29,
												*m_staticText30,
												*m_staticText34,
												*m_staticText35,
												*m_staticText36,
												*m_staticText37,
												*m_staticText38,
												*m_staticText241,
												*m_staticText251,
												*m_staticText261,
												*m_staticText271,
												*m_staticText281;

		wxTextCtrl 							*m_textCtrl1,
												*m_textCtrl2,
												*m_textCtrl3,
												*m_textCtrl4,
												*m_textCtrl5,
												*m_textCtrl6,
												*m_textCtrl7;
	
		double 								m_maxValue,
												m_minValue,
			 									m_newBlueChannelMax,
												m_newBlueChannelMin,
												m_newGreenChannelMax,
												m_newGreenChannelMin,
												m_newRedChannelMax,
												m_newRedChannelMin;
	
		int									m_percentClipped,
												m_localMinMaxCode;

		SInt16 								m_displayType,
												m_minMaxOptionCode,
												m_numberLevels,
												m_rgbColors;

		Boolean 								m_histogramAvailableFlag,
												m_updatingMinMaxFlag;
	
};	// end "class CMDisplayMinMaxDialog"
