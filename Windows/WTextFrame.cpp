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
//	File:						WTextFrame.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			08/30/2018
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	This file contains functions that relate to the
//								CMTextFrame class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
                     
#include "WImageView.h"
#include "WTextFrame.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



IMPLEMENT_DYNCREATE (CMTextFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP (CMTextFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP (CMTextFrame)
	ON_UPDATE_COMMAND_UI (ID_FILE_CLOSE, OnUpdateFileTextClose)

	ON_WM_CLOSE ()
	ON_WM_CREATE ()
	ON_WM_INITMENUPOPUP ()
	ON_WM_SETCURSOR ()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMTextFrame::CMTextFrame (void)

{

}	// end "CMTextFrame"



CMTextFrame::~CMTextFrame (void)

{

}	// end "~CMTextFrame"



void CMTextFrame::OnClose ()

{
			// Add your message handler code here and/or call default

			// I do not want the text output window to be closed.
				
	//CMDIChildWnd::OnClose ();

}	// end "OnClose"



int CMTextFrame::OnCreate (
				LPCREATESTRUCT 					lpCreateStruct)

{
	if (CMDIChildWnd::OnCreate (lpCreateStruct) == -1)
																								return -1;
	
			// Add your specialized creation code here
	
	return 0;
	
}	// end "OnCreate"



void CMTextFrame::OnInitMenuPopup (
				CMenu* 								pPopupMenu,
				UINT 									nIndex,
				BOOL 									bSysMenu)

{
	CMDIChildWnd::OnInitMenuPopup (pPopupMenu, nIndex, bSysMenu);
   
   if (bSysMenu)
   	{                                        
		if (pPopupMenu != NULL)
			pPopupMenu->EnableMenuItem (6, MF_BYPOSITION | MF_GRAYED);
			
		}	// end "if (bSysMenu)"
	
}	// end "OnInitMenuPopup"



BOOL CMTextFrame::OnSetCursor (
				CWnd* 								pWnd,
				UINT 									nHitTest,
				UINT 									message)

{                                                                                              
	if (gPresentCursor == kWait || gPresentCursor == kSpin)
		{
				// Wait cursor in affect. Processing underway.
				// Restart the wait cursor in case in was changed to pointer
				// before entering the image frame.
		
		AfxGetApp()->DoWaitCursor (0);
																					return (TRUE);
		
		}	// end "if (gPresentCursor == kWait || gPresentCursor == kSpin)"

	if (gPresentCursor != kArrow)
		{                  
		if (gActiveImageViewCPtr != NULL)
			gActiveImageViewCPtr->UpdateCursorCoordinates ();
		                                                                
		gPresentCursor = kArrow; 	// Non image window cursor
		
		}	// end "if (gPresentCursor != kArrow)"
	
	return CMDIChildWnd::OnSetCursor (pWnd, nHitTest, message);
	
}	// end "OnSetCursor"



void CMTextFrame::OnUpdateFileTextClose (
				CCmdUI*								pCmdUI)

{
	UpdateFileTextClose (pCmdUI);

	pCmdUI->Enable (FALSE);
	
}	// end "OnUpdateFileTextClose"
