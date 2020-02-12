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
//	File:						WLeaveOneOutMixingDialog.cpp : implementation file
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
//								CMLOOMixingDialog class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "WLeaveOneOutMixingDialog.h"
#include "WMultiSpec.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMLOOMixingDialog, CMDialog)
	//{{AFX_MSG_MAP (CMLOOMixingDialog)
	ON_BN_CLICKED (IDC_IdentityMatrix, OnIdentityMatrix)
	ON_BN_CLICKED (IDC_OptimumMixing, OnOptimumMixing)
	ON_BN_CLICKED (IDC_UserMixing, OnUserMixing)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMLOOMixingDialog::CMLOOMixingDialog (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog (CMLOOMixingDialog::IDD, pParent)

{
	//{{AFX_DATA_INIT (CMLOOMixingDialog)
	m_mixingParameterCode = -1;
	m_userMixingParameter = 0.0;
	//}}AFX_DATA_INIT

	m_initializedFlag = CMDialog::m_initializedFlag;

	m_loocMixingParameter = 0.0;
	
}	// end "CMLOOMixingDialog"



void CMLOOMixingDialog::DoDataExchange (
				CDataExchange* 					pDX)

{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP (CMLOOMixingDialog)
	DDX_Radio (pDX, IDC_OptimumMixing, m_mixingParameterCode);
	DDX_Text2 (pDX, IDC_UserValue, m_userMixingParameter);
	DDV_MinMaxDouble (pDX, m_userMixingParameter, 0., 3.);
	//}}AFX_DATA_MAP
	
}	// end "DoDataExchange"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the leave one out
//							mixing parameter specification dialog box to the user and 
//							copy the revised back to the principal component 
//							specification structure if the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/07/2000
//	Revised By:			Larry L. Biehl			Date: 03/07/2000	

Boolean CMLOOMixingDialog::DoDialog (
				SInt16								statsWindowMode)

{                                 
	INT_PTR								returnCode;
	
	Boolean								continueFlag = FALSE;
								

	                          
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																				return (continueFlag);
																					
	m_statsWindowMode = statsWindowMode;
	
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{		
		LOOCOptionsDialogOK (statsWindowMode,
									m_mixingParameterCode+1,
									m_userMixingParameter);

		continueFlag = TRUE;
									
		}	// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}	// end "DoDialog"



void CMLOOMixingDialog::OnIdentityMatrix ()

{
	HideDialogItem (this, IDC_UserValue);
	
}	// end "OnIdentityMatrix"



BOOL CMLOOMixingDialog::OnInitDialog ()

{
	SInt16								mixingParameterCode;
	
	
	CMDialog::OnInitDialog ();
	
			// Add extra initialization here
	
	LOOCOptionsDialogInitialize (this,
											m_statsWindowMode,
											&mixingParameterCode,
											&m_loocMixingParameter,
											&m_userMixingParameter); 

	m_mixingParameterCode = mixingParameterCode - 1;									
			
	LoadDItemRealValue (this, IDC_OptimumValue, m_loocMixingParameter, 3);
	                 
	if (UpdateData (FALSE))
		PositionDialogWindow ();
	
			// Set default text selection to first edit text item	
		                                                     
	SelectDialogItemText (this, IDC_UserValue, 0, SInt16_MAX);
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE

}	// end "OnInitDialog"



void CMLOOMixingDialog::OnOptimumMixing ()

{
	HideDialogItem (this, IDC_UserValue);
	
}	// end "OnOptimumMixing"



void CMLOOMixingDialog::OnUserMixing ()

{
	ShowDialogItem (this, IDC_UserValue);
	SelectDialogItemText (this, IDC_UserValue, 0, SInt16_MAX);

}	// end "OnUserMixing"
