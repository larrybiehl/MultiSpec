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
//	File:						WClassifyCorrelationDialog.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			01/04/2018
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	This file contains functions that relate to the
//								CMCorrelationClassifyDialog class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "WClassifyCorrelationDialog.h"
#include "WMultiSpec.h" 

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMCorrelationClassifyDialog, CMDialog)
	//{{AFX_MSG_MAP (CMCorrelationClassifyDialog)
	ON_CBN_SELENDOK (IDC_CovarianceCombo, OnSelendokCovarianceCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMCorrelationClassifyDialog::CMCorrelationClassifyDialog (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog (CMCorrelationClassifyDialog::IDD, pParent)

{
	//{{AFX_DATA_INIT(CMCorrelationClassifyDialog)
	m_covarianceEstimate = -1;
	//}}AFX_DATA_INIT
	
	m_initializedFlag = CMDialog::m_initializedFlag;
	
}	// end "CMCorrelationClassifyDialog"



void CMCorrelationClassifyDialog::DoDataExchange (
				CDataExchange* 					pDX)
				
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP (CMCorrelationClassifyDialog)
	DDX_CBIndex (pDX, IDC_CovarianceCombo, m_covarianceEstimate);
	//}}AFX_DATA_MAP
	
}	// end "DoDataExchange"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the Correlation
//							Classifier specification dialog box to the user and copy the
//							revised back to the classify specification structure if
//							the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/09/1998
//	Revised By:			Larry L. Biehl			Date: 05/26/2017	

Boolean CMCorrelationClassifyDialog::DoDialog (
				SInt16*								covarianceEstimatePtr)

{  
	INT_PTR								returnCode;
	
	Boolean								continueFlag = FALSE;
								

			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																						return (FALSE);
																			
	m_covarianceEstimate = *covarianceEstimatePtr - 1;
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{
		continueFlag = TRUE;
		
		*covarianceEstimatePtr = m_covarianceEstimate + 1; 
		
		}	// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}	// end "DoDialog"



BOOL CMCorrelationClassifyDialog::OnInitDialog ()

{
	CMDialog::OnInitDialog ();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
	
}	// end "OnInitDialog"



void CMCorrelationClassifyDialog::OnSelendokCovarianceCombo ()

{
		// Add your control notification handler code here
	
}	// end "OnSelendokCovarianceCombo"
