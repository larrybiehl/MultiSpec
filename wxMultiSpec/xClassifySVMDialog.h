//                                     MultiSpec
//
//                   Copyright 1988-2020 Purdue Research Foundation
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use
// this file except in compliance with the License. You may obtain a copy of the
// License at:  https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the specific
// language governing permissions and limitations under the License.
//
// MultiSpec is curated by the Laboratory for Applications of Remote Sensing at
// Purdue University in West Lafayette, IN and licensed by Larry Biehl.
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
