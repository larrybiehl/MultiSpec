//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WGaussianParameterDialog.h
//	Implementation:		WGaussianParameterDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMGaussianParameterDlg class.
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/12/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "WDialog.h"


class CMGaussianParameterDlg : public CMDialog
{
	// Construction
	public:
		CMGaussianParameterDlg (	// standard constructor
				CWnd* 								pParent = NULL);
	
		Boolean DoDialog (
				double*								gaussianStretchPtr);

		// Dialog Data
		//{{AFX_DATA (CMGaussianParameterDlg)
		enum { IDD = IDD_GaussianParameter };
	
		double								m_gaussianStretch;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);    // DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMGaussianParameterDlg)
		virtual BOOL OnInitDialog ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
};	// end class CMGaussianParameterDlg
