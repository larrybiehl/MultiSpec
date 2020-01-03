//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
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
