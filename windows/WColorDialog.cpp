//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
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
