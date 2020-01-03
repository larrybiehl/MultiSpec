//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WEvaluateTransformationDialog.h
//	Implementation:		WEvaluateTransformationDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMEvalTransformDialog class.
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/05/2019
//
//------------------------------------------------------------------------------------

#pragma once
	     
#include "WDialog.h"


class CMEvalTransformDialog : public CMDialog
{
	// Construction
	public:
		CMEvalTransformDialog (	// standard constructor
				CWnd* 								pParent = NULL);
	
		Boolean DoDialog (
				EvaluateTransformSpecsPtr		evaluateTransformSpecsPtr);

		// Dialog Data
		//{{AFX_DATA (CMEvalTransformDialog)
		enum { IDD = IDD_EvalTransform };
	
		BOOL									m_checkTransformationFlag,
												m_listCheckMatrixFlag,
												m_listEigenvaluesFlag,
												m_listOriginalMatrixFlag;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support
	
		void SetListCheckTransform (
				Boolean								checkTransformFlag);

		// Generated message map functions
		//{{AFX_MSG (CMEvalTransformDialog)
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnCheckTransform ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
		EvaluateTransformSpecsPtr		m_evaluateTransformSpecsPtr;
	
		Boolean		  						m_initializedFlag;
	                     		
};	// end class CMEvalTransformDialog
