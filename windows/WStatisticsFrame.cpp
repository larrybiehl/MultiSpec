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
//	File:						WStatisticsFrame.cpp : implementation file
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
//								CAboutDlg class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "WImageDoc.h"
#include "WImageView.h"
#include "WStatisticsDoc.h"
#include "WStatisticsFrame.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



IMPLEMENT_DYNCREATE (CMStatisticsFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP (CMStatisticsFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP (CMStatisticsFrame)
	ON_COMMAND (ID_EDIT_CLEAR_SELECT_RECTANGLE, OnEditClearSelectRectangle)
	ON_COMMAND (ID_EDIT_CUT, OnEditCut)
	ON_COMMAND (ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND (ID_EDIT_SELECTION_RECTANGLE, OnEditSelectionRectangle)
	ON_COMMAND (ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_COMMAND (ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND (ID_FILE_CLOSE, OnFileProjectClose)

	ON_UPDATE_COMMAND_UI (ID_EDIT_CLEAR_SELECT_RECTANGLE, OnUpdateEditClearSelectRectangle)
	ON_UPDATE_COMMAND_UI (ID_EDIT_CUT, OnUpdateEditCut)
	ON_UPDATE_COMMAND_UI (ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI (ID_EDIT_SELECTION_RECTANGLE, OnUpdateEditSelectionRectangle)
	ON_UPDATE_COMMAND_UI (ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
	ON_UPDATE_COMMAND_UI (ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI (ID_FILE_CLOSE, OnUpdateFileProjectClose)

	ON_WM_CHAR ()
	ON_WM_CLOSE ()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMStatisticsFrame::CMStatisticsFrame (void)

{
   gProjectWindow = (CMImageView*)this; 

}	// end "CMStatisticsFrame"



CMStatisticsFrame::~CMStatisticsFrame (void)

{

}	// end "~CMStatisticsFrame"



void CMStatisticsFrame::OnChar (
				UINT 									nChar,
				UINT 									nRepCnt,
				UINT 									nFlags)

{
			// Add your message handler code here and/or call default
	
	CMDIChildWnd::OnChar (nChar, nRepCnt, nFlags);
	
}	// end "OnChar"



void CMStatisticsFrame::OnClose ()

{
			// Add your message handler code here and/or call default

	CloseTheProject ();
	
}	// end "OnClose"



void CMStatisticsFrame::OnEditClearSelectRectangle ()

{
	if (gProjectSelectionWindow != NULL)
		{
		ClearSelectionArea (gProjectSelectionWindow);
		
		if (gActiveImageViewCPtr == gProjectSelectionWindow)
			ShowGraphSelection ();
		
		}	// end "if (gProjectSelectionWindow != NULL)"
	
}	// end "OnEditClearSelectRectangle"



void CMStatisticsFrame::OnEditCut ()

{
	DoStatisticsWCut ();
	
}	// end "OnEditCut"



void CMStatisticsFrame::OnEditPaste ()

{                                             
	DoStatisticsWPaste ();
	
}	// end "OnEditPaste"



void CMStatisticsFrame::OnEditUndo ()

{                                                                  
	DoStatisticsWUndo ();
	
}	// end "OnEditUndo"



void CMStatisticsFrame::OnEditSelectAll ()

{
	DoEditSelectAllImage (gProjectSelectionWindow);
	
}	// end "OnEditSelectAll"



void CMStatisticsFrame::OnEditSelectionRectangle ()

{                                              
	EditSelectionDialog (kRectangleType, TRUE);
	
}	// end "OnEditSelectionRectangle"



void CMStatisticsFrame::OnFileProjectClose ()

{
	CloseTheProject ();
	
}	// end "OnFileProjectClose"



void CMStatisticsFrame::OnUpdateEditClearSelectRectangle (
				CCmdUI* 								pCmdUI)

{
	SInt16 selectionTypeCode = GetSelectionTypeCode (gProjectSelectionWindow);
	
			// Update Menu item text.
	
	UpdateEditClearSelection (pCmdUI, selectionTypeCode);
	
	pCmdUI->Enable (selectionTypeCode != 0);
	
}	// end "OnUpdateEditClearSelectRectangle"



void CMStatisticsFrame::OnUpdateEditCut (
				CCmdUI* 								pCmdUI)

{
	Boolean								enableFlag = FALSE;
	
	
	if (gProjectInfoPtr != NULL)
		{
		if (gProjectInfoPtr->statsWindowMode == 1)
			{
			pCmdUI->SetText (_T("Cut\tCtrl+X"));
			
			}	// end "if (gProjectInfoPtr->statsWindowMode == 1)"
		
		if (gProjectInfoPtr->statsWindowMode == 2)
			{
			pCmdUI->SetText (_T("Cut Class\tCtrl+X"));
			if (gProjectInfoPtr->currentClass >= 0)
				enableFlag = TRUE;
			
			}	// end "if (gProjectInfoPtr->statsWindowMode == 2)"
		
		if (gProjectInfoPtr->statsWindowMode == 3)
			{
			pCmdUI->SetText (_T("Cut Field\tCtrl+X"));
			if (gProjectInfoPtr->currentField >= 0)
				enableFlag = TRUE;
			
			}	// end "if (gProjectInfoPtr->statsWindowMode == 3)"

		}	// end "if (gProjectInfoPtr != NULL)"
	
	pCmdUI->Enable (enableFlag);
	
}	// end "OnUpdateEditCut"



void CMStatisticsFrame::OnUpdateEditPaste (
				CCmdUI* 								pCmdUI)

{
	Boolean								enableFlag = FALSE;
	
	SInt16								classNumber,
											editFieldClassStorage;
	
	
	if (gProjectInfoPtr != NULL)
		{
		if (gProjectInfoPtr->statsWindowMode == 1)
			{
			pCmdUI->SetText (_T("Paste\tCtrl+V"));
			if (gProjectInfoPtr->editFieldNumber >= 0 &&
														gProjectInfoPtr->currentClass == -1)
				{
				pCmdUI->SetText (_T("Paste Field\tCtrl+V"));
				if (gProjectInfoPtr->numberStatisticsClasses <
																	kMaxNumberStatClasses-1)
					enableFlag = TRUE;
				
				}	// end "if (gProjectInfoPtr->editFieldNumber >= 0 && ..."
			
			}	// end "if (gProjectInfoPtr->statsWindowMode == 1)"
		
		if (gProjectInfoPtr->statsWindowMode == 2)
			{
			pCmdUI->SetText (_T("Paste\tCtrl+V"));
			if (gProjectInfoPtr->editFieldNumber >= 0 &&
														gProjectInfoPtr->currentClass >= 0)
				{
				editFieldClassStorage = gProjectInfoPtr->fieldIdentPtr[
										gProjectInfoPtr->editFieldNumber].classStorage;
				
				classNumber = gProjectInfoPtr->
										classNamesPtr[editFieldClassStorage].classNumber - 1;
				
				if (classNumber != gProjectInfoPtr->currentClass)
					{
					pCmdUI->SetText (_T("Paste Field\tCtrl+V"));
					enableFlag = TRUE;
					
					}	// end "if (classNumber != gProjectInfoPtr->currentClass)"
				
				}	// end "if (gProjectInfoPtr->editFieldNumber >= 0 && ..."
				
			if (gProjectInfoPtr->editClassStorageNumber >= 0 &&
														gProjectInfoPtr->currentClass >= 0)
				{
				pCmdUI->SetText (_T("Paste Class Fields\tCtrl+V"));
				enableFlag = TRUE;
					
				}	// end "if (gProjectInfoPtr->editClassStorageNumber ..."
			
			}	// end "if (gProjectInfoPtr->statsWindowMode == 2)"
		
		if (gProjectInfoPtr->statsWindowMode == 3)
			{
			if (gProjectInfoPtr->editFieldNumber >= 0)
				{
				pCmdUI->SetText (_T("Paste Field\tCtrl+V"));
				
				editFieldClassStorage = gProjectInfoPtr->fieldIdentPtr[
									gProjectInfoPtr->editFieldNumber].classStorage;
					
				classNumber = gProjectInfoPtr->
									classNamesPtr[editFieldClassStorage].classNumber - 1;
				
				if (classNumber != gProjectInfoPtr->currentClass)
					enableFlag = TRUE;
				
				}	// end "if (gProjectInfoPtr->editFieldNumber >= 0 && ..."
			
			if (gProjectInfoPtr->editClassStorageNumber >= 0)
				{
				pCmdUI->SetText (_T("Paste Class Fields\tCtrl+V"));
				enableFlag = TRUE;
					
				}	// end "if (gProjectInfoPtr->editClassStorageNumber ..."
			
			}	// end "if (gProjectInfoPtr->statsWindowMode == 3)"
		
		if (gProjectInfoPtr->statsWindowMode == 4)
			pCmdUI->SetText (_T("Paste Field\tCtrl+V"));

		}	// end "if (gProjectInfoPtr != NULL)"
	
	pCmdUI->Enable (enableFlag);
	
}	// end "OnUpdateEditPaste"



void CMStatisticsFrame::OnUpdateEditSelectAll (
				CCmdUI* 								pCmdUI)

{
	Boolean enableFlag = FALSE;
	
	if (gProjectInfoPtr != NULL && gProjectInfoPtr->selectionType == kRectangleType)
		enableFlag = TRUE;
	
	pCmdUI->Enable (enableFlag);
	
}	// end "OnUpdateEditSelectAll"



void CMStatisticsFrame::OnUpdateEditSelectionRectangle (
				CCmdUI* 								pCmdUI)

{
	Boolean								enableFlag = FALSE;
	
	
	if (gProjectInfoPtr != NULL && gProjectInfoPtr->selectionType == kRectangleType)
		enableFlag = TRUE;
	
	pCmdUI->Enable (enableFlag);
	
}	// end "OnUpdateEditSelectionRectangle"



void CMStatisticsFrame::OnUpdateEditUndo (
				CCmdUI* 								pCmdUI)

{
	Boolean								enableFlag = FALSE;
	
	
			// The Undo/Redo Cut Class item will be allowed for only the 'Select
			// Field' and 'Project Class List' stat window modes.
	
	if (gProjectInfoPtr != NULL)
		{
		if (gProjectInfoPtr->editClassStorageNumber < 0 &&
															gProjectInfoPtr->editFieldNumber < 0)
 			pCmdUI->SetText (_T("Undo\tCtrl+Z"));
		
 		else	// gProjectInfoPtr->editClassStorageNumber >= 0 || ...
 			{
			if (gProjectInfoPtr->editClassStorageNumber >= 0)
 				pCmdUI->SetText (_T("Undo Cut Class\tCtrl+Z"));
			
			if (gProjectInfoPtr->editFieldNumber >= 0)
	 			pCmdUI->SetText (_T("Undo Cut Field\tCtrl+Z"));
			
			if (gProjectInfoPtr->numberStatisticsClasses < kMaxNumberStatClasses-1)
  	 			enableFlag = TRUE;
			
  	 		}	// end "else gProjectInfoPtr->editClassStorageNumber != 0 || ..."

		}	// end "if (gProjectInfoPtr != NULL)"
	
	pCmdUI->Enable (enableFlag);
	
}	// end "OnUpdateEditUndo"



void CMStatisticsFrame::OnUpdateFileProjectClose (
				CCmdUI* 								pCmdUI)

{  
	UpdateFileProjectClose (pCmdUI);

	pCmdUI->Enable (TRUE);
	
}	// end "OnUpdateFileProjectClose"



BOOL CMStatisticsFrame::PreCreateWindow (
				CREATESTRUCT& 						cs)

{
	if (cs.cx != 0)
		{
				// Set the window style
		
		cs.style = WS_CHILD |
						WS_OVERLAPPED |
						WS_MINIMIZEBOX |
						WS_SYSMENU |
						WS_CAPTION |
						WS_BORDER |
						0x00008000;  	// Don't know what this value is.
		
		}	// end "if (cs.cx != 0)"
	
	return (CMDIChildWnd::PreCreateWindow (cs));

}	// end "PreCreateWindow"
