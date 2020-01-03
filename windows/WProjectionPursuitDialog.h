//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WProjectionPursuitDialog.h
//	Implementation:		WProjectionPursuitDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMProjectionPursuitDialog class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/12/2019
//
//------------------------------------------------------------------------------------

#pragma once
	     
#include "WDialog.h"


class CMProjectionPursuitDialog : public CMDialog
{
	public:
		CMProjectionPursuitDialog (	// standard constructor
				CWnd* 								pParent = NULL);
	
		virtual ~CMProjectionPursuitDialog ();  // standard destructor

		Boolean DoDialog (
				ProjectionPursuitSpecsPtr		projectionPursuitSpecsPtr);
	

		// Dialog Data
		//{{AFX_DATA (CMProjectionPursuitDialog)
		enum { IDD = IDD_ProjectionPursuit };
	
		double								m_bottomUpThreshold,
												m_optimizeThreshold,
												m_topDownThreshold;
	
		long									m_bothOddChoicesNumberFeatures,
												m_maximumNumberFeatures;
	
		int									m_algorithmCode,
												m_firstStageMethod,
												m_initialGroupingCode;
	
		short									m_finalNumberFeatures,
												m_initialNumberFeatures;
	
		BOOL									m_numericalOptimizationFlag;
		//}}AFX_DATA

	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);    // DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMProjectionPursuitDialog)
		afx_msg void OnBnClickedAlgorithm ();
	
		afx_msg void OnBnClickedBandGroupingMethod ();
	
		afx_msg void OnBnClickedInitialBandGrouping ();
	
		afx_msg void OnBnClickedNumericaloptimization ();
	
		virtual BOOL OnInitDialog ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	

		ProjectionPursuitSpecsPtr		m_projectionPursuitSpecsPtr;
	
};	// end class CMProjectionPursuitDialog
