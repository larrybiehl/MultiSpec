//                               MultiSpec
//
//               Laboratory for Applications of Remote Sensing
//                           Purdue University
//                        West Lafayette, IN 47907
//                         Copyright (2009-2020)
//                     (c) Purdue Research Foundation
//                           All rights reserved.
//
//   File: 					xClassifySVMDialog.h
//   Implementation: 	xClassifySVMDialog.cpp
//
//   Authors: 				Larry L. Biehl
//
//   Language:   			C++
//
//   System: 				Linux and Mac Operating Systems
//
//   Brief description:	Header file for the CMSVMClassifyDialog class
//
//   Written By: 			Tsung Tai Yeh         	Date: 09/??/2019
//   Revised By:			Larry L. Biehl 			Date: 11/13/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"


class CMSVMClassifyDialog : public CMDialog
{
	public:
		CMSVMClassifyDialog (
				wxWindow*							parent = NULL,
				wxWindowID							id = IDD_SVMParameters,
				const wxString&					title =
															wxT("Specify SVM Classifier Parameters"));
		
		Boolean DoDialog (void);
	
		// Implementation
	protected:
		void OnInitDialog (
				wxInitDialogEvent& 				event);
	
		void OnSelendokSVMKernelCombo (
				wxCommandEvent& 					event);
	
		void OnSelendokSVMTypeCombo (
				wxCommandEvent& 					event);
		DECLARE_EVENT_TABLE ()
	
		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
		
		
		wxBoxSizer							*m_bVSizer151;
		
		wxChoice								*m_kernalTypeCtrl,
												*m_svmTypeCtrl;
	
		double								m_gamma,
												m_coef0,
												m_cache_size,
												m_eps,
												m_cost,
												m_nu,
												m_p;
	
		int									m_svm_type,
												m_kernel_type,
												m_degree,
												m_shrinking,
												m_probability;
		
		bool									m_initializedFlag;
		
};	// end "class CMSVMClassifyDialog"
