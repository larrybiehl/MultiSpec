//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
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
