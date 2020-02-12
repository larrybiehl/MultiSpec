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
//	File:						WEvaluateTransformationDialog.cpp : implementation file
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
//								CMEvalTransformDialog class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
                      
#include "WEvaluateTransformationDialog.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMEvalTransformDialog, CMDialog)
	//{{AFX_MSG_MAP (CMEvalTransformDialog)
	ON_BN_CLICKED (IDC_CheckTransform, OnCheckTransform)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMEvalTransformDialog::CMEvalTransformDialog (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog (CMEvalTransformDialog::IDD, pParent)

{
	//{{AFX_DATA_INIT (CMEvalTransformDialog)
	m_checkTransformationFlag = FALSE;
	m_listCheckMatrixFlag = FALSE;
	m_listEigenvaluesFlag = FALSE;
	m_listOriginalMatrixFlag = FALSE;
	//}}AFX_DATA_INIT
	
	m_initializedFlag = CMDialog::m_initializedFlag;
	
}	// end "CMEvalTransformDialog"



void CMEvalTransformDialog::DoDataExchange (
				CDataExchange* 					pDX)

{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP (CMEvalTransformDialog)
	DDX_Check (pDX, IDC_CheckTransform, m_checkTransformationFlag);
	DDX_Check (pDX, IDC_ListCheckMatrix, m_listCheckMatrixFlag);
	DDX_Check (pDX, IDC_ListEigenvalues, m_listEigenvaluesFlag);
	DDX_Check (pDX, IDC_ListTransform, m_listOriginalMatrixFlag);
	//}}AFX_DATA_MAP
	
}	// end "DoDataExchange"
 


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the evaluate
//							tranformation dialog box to the user and copy the
//							revised back to the EvaluateTransformSpecs structure if
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
//	Coded By:			Larry L. Biehl			Date: 05/20/1997
//	Revised By:			Larry L. Biehl			Date: 05/13/1998

Boolean CMEvalTransformDialog::DoDialog (
				EvaluateTransformSpecsPtr		evaluateTransformSpecsPtr)

{  
	SInt64								returnCode;

	Boolean								continueFlag = FALSE;
	

			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																						return (FALSE);
																			
	m_evaluateTransformSpecsPtr = evaluateTransformSpecsPtr;
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{
		continueFlag = TRUE;
					
				// List options.	
				
		EvaluateTransformationOK (evaluateTransformSpecsPtr,
											m_listEigenvaluesFlag,
											m_listOriginalMatrixFlag,
											m_checkTransformationFlag,
											m_listCheckMatrixFlag);														

		}	// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}	// end "DoDialog"



void CMEvalTransformDialog::OnCheckTransform ()

{                                                                                          
	DDX_Check (m_dialogFromPtr, IDC_CheckTransform, m_checkTransformationFlag);
	
	SetListCheckTransform (m_checkTransformationFlag);
	
}	// end "OnCheckTransform"



BOOL CMEvalTransformDialog::OnInitDialog ()

{
	CMDialog::OnInitDialog ();
	
			// Initialize dialog variables.

	EvaluateTransformationInitialize (this,
													m_evaluateTransformSpecsPtr,
													(Boolean*)&m_listEigenvaluesFlag,
													(Boolean*)&m_listOriginalMatrixFlag,
													(Boolean*)&m_checkTransformationFlag,
													(Boolean*)&m_listCheckMatrixFlag);
	                                                                       
	SetListCheckTransform (m_checkTransformationFlag);
	                  
	if (UpdateData (FALSE))
		PositionDialogWindow ();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
	
}	// end "OnInitDialog"



void CMEvalTransformDialog::SetListCheckTransform (
				Boolean								checkTransformFlag)

{                                                                            
	if (checkTransformFlag)
		{ 
		MShowDialogItem (this, IDC_ListCheckMatrix);
		MShowDialogItem (this, IDC_ListCheckMatrix2);
		
		}	// end "if (checkTransformFlag)"
		
	else    // !checkTransformFlag
		{   
		MHideDialogItem (this, IDC_ListCheckMatrix); 
		MHideDialogItem (this, IDC_ListCheckMatrix2); 
		
		}	// end "else !checkTransformFlag"
	
}	// end "SetListCheckTransform"
