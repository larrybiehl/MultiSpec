//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WRecodeThematicImageDialog.h
//	Implementation:		WRecodeThematicImageDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMRecodeThematicDialog class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/12/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "WDialog.h"


class CMRecodeThematicDialog : public CMDialog
{
	// Construction
	public:
		CMRecodeThematicDialog (	// standard constructor
				CWnd* 								pParent = NULL);

		Boolean DoDialog (
				RecodeThematicImagePtr			recodeThematicImagePtr);
	

	// Dialog Data
		//{{AFX_DATA (CMRecodeThematicDialog)
		enum { IDD = IDD_ThematicRecode };
	
		int									m_compareParameterCode,
												m_thresholdImageSelection;
	
		long									m_recodedValue,
												m_thresholdValue;
		//}}AFX_DATA


	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);    // DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMRecodeThematicDialog)
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnSelendokThresholdFileCombo ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
	
		RecodeThematicImagePtr			m_recodeThematicImagePtr;
	
		Handle								m_thresholdFileInfoHandle;
	
		Boolean								m_initializedFlag;
	
};	// end class CMRecodeThematicDialog
