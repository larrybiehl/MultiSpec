//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WClassifyCorrelationDialog.h
//	Implementation:		WClassifyCorrelationDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMCorrelationClassifyDialog class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1990's
//	Revised By:				Larry L. Biehl			Date: 12/05/2019
//
//------------------------------------------------------------------------------------

#pragma once
	     
#include "WDialog.h"


class CMCorrelationClassifyDialog : public CMDialog
{
	// Construction
	public:
		CMCorrelationClassifyDialog (	// standard constructor
				CWnd* 								pParent = NULL);
	
		Boolean DoDialog (
				SInt16*								covarianceEstimatePtr);

		// Dialog Data
		//{{AFX_DATA (CMCorrelationClassifyDialog)
		enum { IDD = IDD_SAMParameters };
	
		int									m_covarianceEstimate;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMCorrelationClassifyDialog)
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnSelendokCovarianceCombo ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
	
		Boolean								m_initializedFlag;
	
}; // end class CMCorrelationClassifyDialog
