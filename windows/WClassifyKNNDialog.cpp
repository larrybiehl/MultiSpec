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
//	File:						WClassifyKNNDialog.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			08/09/2019
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	This file contains functions that relate to the
//								CMKNNClassifyDialog class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "WClassifyKNNDialog.h"
#include "WMultiSpec.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMKNNClassifyDialog, CMDialog)
	//{{AFX_MSG_MAP (CMKNNClassifyDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMKNNClassifyDialog::CMKNNClassifyDialog (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog (CMKNNClassifyDialog::IDD, pParent)

{
	//{{AFX_DATA_INIT(CMKNNClassifyDialog)
	m_nearestNeighborKValue = 5;
	//}}AFX_DATA_INIT
	
	m_initializedFlag = CMDialog::m_initializedFlag;
	
}	// end "CMKNNClassifyDialog"



void CMKNNClassifyDialog::DoDataExchange (
				CDataExchange* 					pDX)

{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP (CMKNNClassifyDialog)
	DDX_Text (pDX, IDC_KValue, m_nearestNeighborKValue);
	DDV_MinMaxLong (pDX, m_nearestNeighborKValue, 1, 100);
	//}}AFX_DATA_MAP
	
}	// end "DoDataExchange"



//-----------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the KNN Classifier
//							specification dialog box to the user and copy the
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
//	Coded By:			Larry L. Biehl			Date: 08/09/2019
//	Revised By:			Larry L. Biehl			Date: 08/09/2019	

Boolean CMKNNClassifyDialog::DoDialog (
				SInt16*								nearestNeighborKValuePtr)

{  
	INT_PTR								returnCode;
	
	Boolean								continueFlag = FALSE;
								

	                          
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																			return (FALSE);
																			
	m_nearestNeighborKValue = *nearestNeighborKValuePtr;
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{
		continueFlag = TRUE;
		
		*nearestNeighborKValuePtr = m_nearestNeighborKValue; 
		
		}	// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}	// end "DoDialog"



BOOL CMKNNClassifyDialog::OnInitDialog ()

{
	CMDialog::OnInitDialog ();
	
			// Set default text selection	
		                                                     
	SelectDialogItemText (this, IDC_KValue, 0, SInt16_MAX);	

	return FALSE;  // return TRUE  unless you set the focus to a control
	
}	// end "OnInitDialog"
