//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WClassifySVMDialog.h
//	Implementation:		WClassifySVMDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMSVMClassifyDialog class
//
//	Written By:				Larry L. Biehl			Date: ??/??/2019
//	Revised By:				Larry L. Biehl			Date: 09/24/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "WDialog.h"


class CMSVMClassifyDialog : public CMDialog
{
	public:
		CMSVMClassifyDialog (	// standard constructor
				CWnd* 								pParent = NULL);
	
		Boolean DoDialog ();

		// Dialog Data
		//{{AFX_DATA (CMSVMClassifyDialog)
		enum { IDD = IDD_ClassifySVMParameters };
	
		double								m_cache_size,
												m_coef0,
												m_cost,
												m_eps,
												m_gamma,
												m_nu,
												m_p;
	
		int									m_degree,
												m_kernel_type,
												m_probability,
												m_shrinking,
												m_svm_type;
		//}}AFX_DATA

	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);    // DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMSVMClassifyDialog)
		virtual BOOL OnInitDialog ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()

		void OnSelendokSVMTypeCombo ();
	
		void OnSelendokSVMKernelCombo ();
	

		Boolean								m_initializedFlag;
	
};	// end class CMSVMClassifyDialog
