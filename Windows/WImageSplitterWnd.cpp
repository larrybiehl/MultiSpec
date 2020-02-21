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
