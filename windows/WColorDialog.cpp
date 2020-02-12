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
//	File:						WColorDialog.cpp : implementation file
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
//								CMColorDialog class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "WColorDialog.h"
#include "WMultiSpec.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMColorDialog, CColorDialog)
	//{{AFX_MSG_MAP (CMColorDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMColorDialog::CMColorDialog (
				COLORREF 							clrInit /* = 0 */,
				DWORD 								dwFlags /* = 0 */,
				CWnd* 								pParentWnd /* = NULL */)
		: CColorDialog (clrInit, dwFlags, pParentWnd)

{
	//{{AFX_DATA_INIT(CMColorDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
}	// end "CMColorDialog"



void CMColorDialog::DoDataExchange (
				CDataExchange* 					pDX)

{
	CColorDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP (CMColorDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	
}	// end "DoDataExchange"



BOOL CMColorDialog::OnInitDialog ()

{
	CColorDialog::OnInitDialog ();
	                                                      
	PositionDialogWindow (this);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
	
}	// end "OnInitDialog"
