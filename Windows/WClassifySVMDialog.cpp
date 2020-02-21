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
//	File:						WClassifySVMDialog.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			10/26/2019
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	This file contains functions that relate to the
//								CMSVMClassifyDialog class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "WClassifySVMDialog.h"
#include "WMultiSpec.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMSVMClassifyDialog, CMDialog)
	//{{AFX_MSG_MAP (CMSVMClassifyDialog)
	ON_CBN_SELENDOK (IDC_SVM_TYPE, OnSelendokSVMTypeCombo)
	ON_CBN_SELENDOK (IDC_SVM_KERNEL_TYPE, OnSelendokSVMKernelCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP () 



CMSVMClassifyDialog::CMSVMClassifyDialog (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog (CMSVMClassifyDialog::IDD, pParent)

{
	//{{AFX_DATA_INIT(CMSVMClassifyDialog)
	//}}AFX_DATA_INIT
	
	m_initializedFlag = CMDialog::m_initializedFlag;
}



void CMSVMClassifyDialog::DoDataExchange (CDataExchange* pDX)

{
	CDialog::DoDataExchange (pDX);
	
	//{{AFX_DATA_MAP (CMSVMClassifyDialog)
	DDX_CBIndex (pDX, IDC_SVM_TYPE, m_svm_type);
	DDX_CBIndex (pDX, IDC_SVM_KERNEL_TYPE, m_kernel_type);
	DDX_Text (pDX, IDC_SVM_EPS, m_eps);
	DDX_Check (pDX, IDC_SVM_SHRINKING, m_shrinking);
	DDX_Text (pDX, IDC_SVM_CACHE_SIZE, m_cache_size);
	DDX_Text (pDX, IDC_SVM_DEGREE, m_degree);
	DDX_Text (pDX, IDC_SVM_GAMMA, m_gamma);
	DDX_Text (pDX, IDC_SVM_COEF0, m_coef0);
	DDX_Text (pDX, IDC_SVM_COST, m_cost);
	DDX_Text (pDX, IDC_SVM_NU, m_nu);
	DDX_Text (pDX, IDC_SVM_P, m_p);
	DDX_Check (pDX, IDC_SVM_PROBABILITY, m_probability);
	//}}AFX_DATA_MAP
} 



//-----------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the SVM Classifier
//							specification dialog box to the user and copy the
//							revised back to the classify specification structure if
//							the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:			
// 
//	Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/24/2019
//	Revised By:			Larry L. Biehl			Date: 09/24/2019

Boolean CMSVMClassifyDialog::DoDialog ()

{  
	INT_PTR								returnCode;
	
	Boolean								continueFlag = FALSE;
								

	                          
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																			return (FALSE);
																			
	//m_nearestNeighborKValue = *nearestNeighborKValuePtr;
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{
		continueFlag = TRUE;

		if (gClassifySpecsPtr->svm_type != m_svm_type ||
				gClassifySpecsPtr->svm_kernel_type != m_kernel_type ||
					gClassifySpecsPtr->svm_degree != m_degree ||
						gClassifySpecsPtr->svm_gamma != m_gamma ||
							gClassifySpecsPtr->svm_coef0 != m_coef0 ||
								gClassifySpecsPtr->svm_cache_size != m_cache_size ||
									gClassifySpecsPtr->svm_eps != m_eps ||
										gClassifySpecsPtr->svm_cost != m_cost ||
												gClassifySpecsPtr->svm_nu != m_nu ||
													gClassifySpecsPtr->svm_p != m_p ||
				gClassifySpecsPtr->svm_shrinking != m_shrinking ||
					gClassifySpecsPtr->svm_probability != m_probability)
			gClassifySpecsPtr->supportVectorMachineModelAvailableFlag = false;
		
		gClassifySpecsPtr->svm_type = m_svm_type;
		gClassifySpecsPtr->svm_kernel_type = m_kernel_type;
		gClassifySpecsPtr->svm_degree = m_degree;
		gClassifySpecsPtr->svm_gamma = m_gamma;
		gClassifySpecsPtr->svm_coef0 = m_coef0;
		gClassifySpecsPtr->svm_nu = m_nu;
		gClassifySpecsPtr->svm_cache_size = m_cache_size;
		gClassifySpecsPtr->svm_cost = m_cost;
		gClassifySpecsPtr->svm_eps = m_eps;
		gClassifySpecsPtr->svm_p = m_p;
		gClassifySpecsPtr->svm_shrinking = m_shrinking;
		gClassifySpecsPtr->svm_probability = m_probability;
		
		}	// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}	// end "DoDialog"



BOOL CMSVMClassifyDialog::OnInitDialog ()

{
	m_svm_type = gClassifySpecsPtr->svm_type;
	m_kernel_type = gClassifySpecsPtr->svm_kernel_type;
	m_degree = gClassifySpecsPtr->svm_degree;
	m_gamma = gClassifySpecsPtr->svm_gamma;
	m_coef0 = gClassifySpecsPtr->svm_coef0;
	m_cache_size = gClassifySpecsPtr->svm_cache_size;
	m_eps = gClassifySpecsPtr->svm_eps;
	m_cost = gClassifySpecsPtr->svm_cost;
	m_nu = gClassifySpecsPtr->svm_nu;
	m_p = gClassifySpecsPtr->svm_p;
	m_shrinking = gClassifySpecsPtr->svm_shrinking;
	m_probability = gClassifySpecsPtr->svm_probability;
	
	CMDialog::OnInitDialog ();
	
   SVMClassifyDialogShowHideParameters (this,
														m_svm_type,
														m_kernel_type);
	
			// Set default text selection	
		                                                     
	//SelectDialogItemText (this, IDC_KValue, 0, SInt16_MAX);

	return FALSE;  // return TRUE  unless you set the focus to a control
	
}	// end "OnInitDialog"

  

void CMSVMClassifyDialog::OnSelendokSVMTypeCombo ()

{                
	SInt16				savedSVMType;
	
	
	savedSVMType = m_svm_type;
	                                    
	DDX_CBIndex (m_dialogFromPtr, 
						IDC_SVM_TYPE, 
						m_svm_type); 
	
	if (savedSVMType != m_svm_type)
		SVMClassifyDialogShowHideParameters (this,
															m_svm_type,
															m_kernel_type);
	
}	// end "OnSelendokSVMTypeCombo"

  

void CMSVMClassifyDialog::OnSelendokSVMKernelCombo ()

{    
	SInt16				savedSVMKernel;
	
	
	savedSVMKernel = m_kernel_type;
	                                    
	DDX_CBIndex (m_dialogFromPtr, 
						IDC_SVM_KERNEL_TYPE, 
						m_kernel_type);                                                        
	
	if (savedSVMKernel != m_kernel_type)
		SVMClassifyDialogShowHideParameters (this,
															m_svm_type,
															m_kernel_type);
	
}	// end "OnSelendokSVMKernelCombo"
