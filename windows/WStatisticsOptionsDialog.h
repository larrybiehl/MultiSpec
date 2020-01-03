//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WStatisticsOptionsDialog.h
//	Implementation:		WStatisticsOptionsDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMStatOptionsDlg class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/13/2019
//
//------------------------------------------------------------------------------------

#pragma once
	
#include "WDialog.h"


class CMStatOptionsDlg : public CMDialog
{
	// Construction
	public:
		CMStatOptionsDlg (	// standard constructor
				CWnd* 								pParent = NULL);
		
		SInt16 DoDialog (
				SInt16*								statCodePtr,
				Boolean*								keepClassStatsFlagPtr,
				double*								variancePtr,
				double*								minLogDeterminantPtr,
				Boolean*								useCommonCovarianceInLOOCFlagPtr);
	
				
		// Dialog Data
		//{{AFX_DATA (CMStatOptionsDlg)
		enum { IDD = IDD_StatisticsOptionsDialog };
	
		double								m_minLogDetValue,
												m_varianceValue;
	
		int									m_statCode;
	
		BOOL									m_classStatsOnlyFlag,
												m_setZeroVarianceFlag,
												m_useCommonCovarianceInLOOCFlag;
		//}}AFX_DATA
	

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support
	
		void SetZeroVariance (void);

		// Generated message map functions
		//{{AFX_MSG (CMStatOptionsDlg)
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnMeanStd ();
	
		afx_msg void OnMeanStdCov ();
	
		afx_msg void OnSetZeroVariance ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
};	// end class CMStatOptionsDlg
