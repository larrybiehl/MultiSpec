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
//	File:						WDisplayMinMaxDialog.h
//	Implementation:		WDisplayMinMaxDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMDisplayMinMaxDialog class
//
//	Written By:				Larry L. Biehl			Date: ??/??/2019
//	Revised By:				Larry L. Biehl			Date: 12/05/2019
//
//------------------------------------------------------------------------------------

#pragma once
	     
#include "WDialog.h"


class CMDisplayMinMaxDialog : public CMDialog
{
	// Construction
	public:
		CMDisplayMinMaxDialog (	// standard constructor
				CWnd* 								pParent = NULL);
	
		Boolean DoDialog (
				SInt16*								channelsPtr,
				SInt16								rgbColors,
				SInt16								displayType,
				SInt16								numberLevels,
				SInt16*								percentTailsClippedPtr,
				SInt16*								minMaxSelectionPtr,
				double*								minMaxValuesPtr);

		// Dialog Data
		//{{AFX_DATA (CMDisplayMinMaxDialog)
		enum { IDD = IDD_DisplayEnhancement };
	
		double								m_newBlueChannelMax,
												m_newBlueChannelMin,
												m_newGreenChannelMax,
												m_newGreenChannelMin,
												m_newRedChannelMax,
												m_newRedChannelMin;
	
		int									m_percentClipped,
												m_localMinMaxCode;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMDisplayMinMaxDialog)
		afx_msg void OnChangeMinMax ();
	
		afx_msg void OnChangePercentClipped ();
	
		afx_msg void OnChannelMinMax ();
	
		virtual BOOL OnInitDialog ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
	
		double*								m_minMaxValuesPtr;
	
		HistogramSpecsPtr					m_histogramSpecsPtr;
	
		HistogramSummaryPtr				m_histogramSummaryPtr;
	
		SInt16								*m_channelsPtr;
	
		double								m_maxValue,
												m_minValue;
	
		SInt16								m_displayType,
												m_minMaxOptionCode,
												m_numberLevels,
												m_rgbColors;
	
		Boolean								m_histogramAvailableFlag,
												m_updatingMinMaxFlag;
	
};	// end class CMDisplayMinMaxDialog
