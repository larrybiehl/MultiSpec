//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WImageSplitterWnd.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			12/21/2017
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	This file contains functions that relate to the
//								CMImageSplitterWnd class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "WImageSplitterWnd.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif	// _DEBUG  



IMPLEMENT_DYNAMIC (CMImageSplitterWnd, CSplitterWnd)

BEGIN_MESSAGE_MAP (CMImageSplitterWnd, CSplitterWnd)
	//{{AFX_MSG_MAP (CMImageSplitterWnd)
	ON_WM_NCHITTEST ()
	//}}AFX_MSG_MAP
	// Standard printing commands 
END_MESSAGE_MAP ()



afx_msg LRESULT CMImageSplitterWnd::OnNcHitTest (
				CPoint 								point)

{
	Handle								windowInfoHandle;
	SInt16								windowType;


	windowInfoHandle = GetActiveImageWindowInfoHandle ();
	windowType = GetWindowType (windowInfoHandle);
	if (windowType == kImageWindowType)
		return HTNOWHERE;

	else    // windowType != kImageWindowType
		return (CSplitterWnd::OnNcHitTest (point));
	
}	// end "OnNcHitTest"
