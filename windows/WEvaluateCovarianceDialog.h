//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WEvaluateCovarianceDialog.h
//	Implementation:		WEvaluateCovarianceDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMEvalCovarianceDialog class.
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/05/2019
//
//------------------------------------------------------------------------------------

#pragma once
	     
#include "WDialog.h"


class CMEvalCovarianceDialog : public CMDialog
{
	// Construction
	public:
		CMEvalCovarianceDialog (	// standard constructor
				CWnd* 								pParent = NULL);
	
		~CMEvalCovarianceDialog ();	// standard desctructor
	
		Boolean DoDialog (
				EvaluateCovarianceSpecsPtr		evaluateCovarianceSpecsPtr);

	// Dialog Data
		//{{AFX_DATA (CMEvalCovarianceDialog)
		enum { IDD = IDD_EvalCovariances };
	
		BOOL									m_featureTransformationFlag,
												m_listInvertedMatrixFlag,
												m_listInvertedInvertedMatrixFlag,
												m_listOriginalMatrixFlag,
												m_listOriginalXInvertedMatrixFlag;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMEvalCovarianceDialog)
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnSelendokChannelCombo ();
	
		afx_msg void OnSelendokClassCombo ();
	
		afx_msg void OnTransformation ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
	
		EvaluateCovarianceSpecsPtr		m_evaluateCovarianceSpecsPtr;
	
		UInt16								m_numberEigenvectors;
		
		Boolean		  						m_featureTransformAllowedFlag,
												m_initializedFlag;
	
};	// end class CMEvalCovarianceDialog
