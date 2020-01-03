//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WLeaveOneOutMixingDialog.h
//	Implementation:		WLeaveOneOutMixingDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMLOOMixingDialog class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/12/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "WDialog.h"


class CMLOOMixingDialog : public CMDialog
{
	// Construction
	public:
		CMLOOMixingDialog (	// standard constructor
				CWnd* 								pParent = NULL);
				

		// Dialog Data
		//{{AFX_DATA (CMLOOMixingDialog)
		enum { IDD = IDD_LOOMixingParameter };
	
		double	m_userMixingParameter;
	
		int		m_mixingParameterCode;
		//}}AFX_DATA

		Boolean DoDialog (
				SInt16								statsWindowMode);


	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);    // DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMLOOMixingDialog)
		afx_msg void OnIdentityMatrix ();
	
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnOptimumMixing ();
	
		afx_msg void OnUserMixing ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	

		double								m_loocMixingParameter;

		SInt16								m_statsWindowMode;

};	// end class CMLOOMixingDialog
