//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WListResultsOptionsDialog.h
//	Implementation:		WListResultsOptionsDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMListResultsOptionsDlg class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/12/2019
//
//------------------------------------------------------------------------------------

#pragma once
	     
#include "WDialog.h"


class CMListResultsOptionsDlg : public CMDialog
{
	// Construction
	public:
		CMListResultsOptionsDlg (	// standard constructor
				CWnd* 								pParent = NULL);
	
		void DoDialog (
				SInt16*								listResultsTrainingCodePtr,
				SInt16*								listResultsTestCodePtr);

		// Dialog Data
		//{{AFX_DATA (CMListResultsOptionsDlg)
		enum { IDD = IDD_ResultsListOptions };
	
		BOOL									m_testAreaPerformanceFlag,
												m_testAreaSummariesFlag,
												m_testAreasUsedFlag,
												m_testClassPerformanceFlag,
												m_trainAreaPerformanceFlag,
												m_trainAreaSummariesFlag,
												m_trainAreasUsedFlag,
												m_trainClassPerformanceFlag;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMListResultsOptionsDlg)
		virtual BOOL OnInitDialog ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
		SInt16								m_listResultsTestCode,
												m_listResultsTrainingCode;
	
};	// end class CMListResultsOptionsDlg
