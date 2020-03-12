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
//	File:						WTextView.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			03/03/2020
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	This file contains functions that relate to the
//								CMTextView class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
                   
#include "WImageView.h"
#include "WMultiSpec.h"
#include "WTextDoc.h"
#include "WTextView.h"

LOGFONT NEAR CMTextView::m_lfDefFont;

#ifdef _DEBUG
		#undef THIS_FILE
		static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



IMPLEMENT_DYNCREATE (CMTextView, CEditView);

BEGIN_MESSAGE_MAP (CMTextView, CEditView)
	//{{AFX_MSG_MAP (CMTextView)
	ON_COMMAND (ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)

	ON_UPDATE_COMMAND_UI (ID_EDIT_CLEAR, OnUpdateEditClear)
	ON_UPDATE_COMMAND_UI (ID_EDIT_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI (ID_EDIT_CUT, OnUpdateEditCut)
	ON_UPDATE_COMMAND_UI (ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
	ON_UPDATE_COMMAND_UI (ID_FILE_CLOSE, OnUpdateFileClose)
	ON_UPDATE_COMMAND_UI (ID_FILE_PRINT, OnUpdateFilePrint)
	ON_UPDATE_COMMAND_UI (ID_FILE_PRINT_PREVIEW, OnUpdateFilePrintPreview)
	ON_UPDATE_COMMAND_UI (ID_FILE_PRINT_SETUP, OnUpdateFilePrintSetup)
	ON_UPDATE_COMMAND_UI (ID_FILE_SAVE, OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI (ID_FILE_SAVE_AS, OnUpdateFileSaveAs)

	ON_WM_CHAR ()
	ON_WM_CREATE ()
	ON_WM_MOUSEMOVE ()
	ON_WM_SETCURSOR ()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMTextView::CMTextView ()

{                 
	gOutputViewCPtr = this;

}	// end "CMTextView"



CMTextView::~CMTextView ()

{              
	gOutputViewCPtr = NULL;
    
	m_font.DeleteObject ();
	
}	// end "~CMTextView"


#ifdef _DEBUG
void CMTextView::AssertValid () const

{
	CEditView::AssertValid ();
	
}	// end "AssertValid"



void CMTextView::Dump (CDumpContext& dc) const

{
	CEditView::Dump (dc);
	
}	// end "Dump"



CMTextDoc* CMTextView::GetDocument () // non-debug version is inline

{
	ASSERT(m_pDocument->IsKindOf (RUNTIME_CLASS (CMTextDoc)));
	return (CMTextDoc*)m_pDocument;
	
}	// end "GetDocument"
#endif    //_DEBUG



SInt32 CMTextView::GetTextBufferLength (void)

{
	Boolean modifiedFlag = GetEditCtrl ().GetModify ();
	SInt32 bufferLength = GetBufferLength ();

			// Reset the modified flag. Under Windows 95, it
			// get set to false during the call to GetBufferLength.
	
	GetEditCtrl ().SetModify (modifiedFlag);

	return (bufferLength);
	
}	// end "GetTextBufferLength"

									

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ListString
//
//	Software purpose:	The purpose of this routine is to list the input
//							information to the output text window.
//
//	Parameters in:		Pointer to 'c' string.
//
//	Parameters out:	None
//
//	Value Returned:	True if operation is okay
//							False if the text limit was reached	
// 
// Called By:			main
//							DisplayCImage  in displayc.c
//							DisplayPImage  in displayp.c
//
//	Coded By:			Larry L. Biehl			Date: 03/30/1988
//	Revised By:			Larry L. Biehl			Date: 03/03/2017			

Boolean CMTextView::ListString (
				HPtr									textPtr, 
				UInt32								textLength,
				SInt16								charFormatCode)

{      
	USES_CONVERSION;

	TBYTE									tempWideCharacterString[1000];


			// Make certain that there is a 'c' terminator at the end of the string.
	
	if (textPtr[textLength] != 0)
		textPtr[textLength] = 0;
   
			//
	if (charFormatCode == kUnicodeCharString)
		GetEditCtrl ().ReplaceSel ((LPCTSTR)textPtr);
		
	else if (charFormatCode == kUTF8CharString)
		{
		int sizeNeeded = MultiByteToWideChar (CP_UTF8, 0, textPtr, -1, NULL, 0);

		sizeNeeded = MIN (sizeNeeded, 1000);
		MultiByteToWideChar (CP_UTF8,
									0,
									textPtr,
									-1,
									(LPWSTR)tempWideCharacterString,
									sizeNeeded);

		GetEditCtrl ().ReplaceSel ((LPCTSTR)tempWideCharacterString);
		
		}	// end "else if (charFormatCode == kUTF8CharString)"

	else	// is kASCIICharString
		GetEditCtrl ().ReplaceSel ((LPCTSTR)A2T(textPtr));
	                                 
	GetEditCtrl ().SetModify (TRUE);
	
	return (TRUE);    
	
}	// end "ListString"

 

void CMTextView::OnActivateView (
				BOOL 									bActivate,
				CView* 								pActivateView,
				CView* 								pDeactiveView)
				
{
	CEditView::OnActivateView (bActivate, pActivateView, pDeactiveView);
	
	m_activeFlag = bActivate;
	
}	// end "OnActivateView"



void CMTextView::OnChar (
				UINT 									nChar,
				UINT 									nRepCnt,
				UINT 									nFlags)

{
			// Only process the delete key for now
	
 	if (nChar == 8 && nRepCnt == 0)
		CWnd::OnChar (nChar, nRepCnt, nFlags);
	
}	// end "OnChar"



int CMTextView::OnCreate (
				LPCREATESTRUCT 					lpCreateStruct)

{
	LOGFONT								lf;
	
	
	if (CEditView::OnCreate (lpCreateStruct) == -1)
																							return -1;

			// Set font to be used.
	
	::GetObject (GetStockObject (SYSTEM_FIXED_FONT), sizeof (LOGFONT), &lf); 
	
			// Get the font size based on the system 

	long				lfHeight;

	lfHeight = ((CMultiSpecApp*)AfxGetApp())->getFontHeightForDefaultDC (10.0);
	//lfHeight = 10;
	
			// Change to Courier font.
	
	lf.lfHeight = lfHeight;		//  10;
	lf.lfWidth = 0;
	lf.lfQuality = DEFAULT_QUALITY;

	lf.lfFaceName[0] = 'C';
	lf.lfFaceName[1] = 'o';
	lf.lfFaceName[2] = 'u';
	lf.lfFaceName[3] = 'r';
	lf.lfFaceName[4] = 'i';
	lf.lfFaceName[5] = 'e';
	lf.lfFaceName[6] = 'r';
	lf.lfFaceName[7] = 0x00;
	
	if (m_font.CreateFontIndirect (&lf))
		{
		SetFont (&m_font);
		m_lfDefFont = lf;
		
		}	// end "if (m_font.CreateFontIndirect (&lf))"
	
   SetTabStops (4);
	
	return 0;
	
}	// end "OnCreate"



void CMTextView::OnFilePrintPreview ()

{
	CEditView::OnFilePrintPreview ();
	
}	// end "OnFilePrintPreview"



void CMTextView::OnInitialUpdate ()

{
			// Move the text window to the right portion of the client area
			// of the main frame rect.
	
	CRect									mainFrameRect,
											textWindowRect;
	SInt16								xLocation,
											xSize,
											yLocation,
											ySize;
	
	
	CFrameWnd* pMainFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
	pMainFrame->GetClientRect (&mainFrameRect);
	GetParentFrame()->GetWindowRect (&textWindowRect);
	
	xSize = (SInt16)(textWindowRect.right - textWindowRect.left);
	ySize = (SInt16)(textWindowRect.bottom - textWindowRect.top);
	xLocation = (SInt16)(mainFrameRect.right - xSize - 7);
	xLocation = MAX (0, xLocation);
	yLocation = (SInt16)(mainFrameRect.top + 3);
	yLocation = MAX (0,yLocation);
	GetParentFrame()->MoveWindow (xLocation, yLocation, xSize, ySize, TRUE);
	
	CEditView::OnInitialUpdate ();

			// This should set the limit to 0x0fffffff for when running
			// under Windows NT and 0xffff when running Windows 95/98.

	CEdit& editControlPtr = GetEditCtrl ();
	editControlPtr.SetLimitText (0x0FFFFFFF);

	SInt32 limit = editControlPtr.GetLimitText ();
	
}	// end "OnInitialUpdate"



void CMTextView::OnMouseMove (
				UINT 									nFlags,
				CPoint 								point)

{
	CEditView::OnMouseMove (nFlags, point);
	
} 	// end "OnMouseMove"



BOOL CMTextView::OnSetCursor (
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

	if (!m_activeFlag)
		{
		if (gPresentCursor != kArrow && gActiveImageViewCPtr != NULL)
			gActiveImageViewCPtr->UpdateCursorCoordinates ();

		SetCursor (AfxGetApp()->LoadStandardCursor (IDC_ARROW));
		gPresentCursor = kArrow;		// Non-image window cursors.
		
																						return TRUE;
		
		}	// end "if (!m_activeFlag)"
	
	gPresentCursor = kIBeam;
	
	return CEditView::OnSetCursor (pWnd, nHitTest, message);
	
}	// end "OnSetCursor"



void CMTextView::OnUpdateEditClear (
				CCmdUI* 								pCmdUI)

{
	int									startSel,
											endSel;
	
	
	UpdateEditTextClear (pCmdUI);
	
	CEdit& editControlPtr = GetEditCtrl ();
	editControlPtr.GetSel (startSel, endSel);
	pCmdUI->Enable (endSel > startSel);
	
}	// end "OnUpdateEditClear"



void CMTextView::OnUpdateEditCopy (
				CCmdUI* 								pCmdUI)

{
	int									startSel,
											endSel;
	
	
	UpdateEditTextCopy (pCmdUI);
	
	CEdit& editControlPtr = GetEditCtrl ();
	editControlPtr.GetSel (startSel, endSel);
	pCmdUI->Enable (endSel > startSel);
	
}	// end "OnUpdateEditCopy"



void CMTextView::OnUpdateEditCut (
				CCmdUI* 								pCmdUI)

{
	int									startSel,
											endSel;
	
	
	UpdateEditTextCut (pCmdUI);
	
	CEdit& editControlPtr = GetEditCtrl ();
	editControlPtr.GetSel (startSel, endSel);
	pCmdUI->Enable (endSel != startSel);
	
}	// end "OnUpdateEditCut"



void CMTextView::OnUpdateEditSelectAll (
				CCmdUI* 								pCmdUI)

{
	UpdateEditTextSelectAll (pCmdUI);
	
	pCmdUI->Enable (GetTextBufferLength () != 0);
	
}	// end "OnUpdateEditSelectAll"



void CMTextView::OnUpdateFileClose (
				CCmdUI* 								pCmdUI)

{
			// Do not allow text window to be closed.
	
	pCmdUI->Enable (FALSE);
	
}	// end "OnUpdateFileClose"



void CMTextView::OnUpdateFilePrint (
				CCmdUI* 								pCmdUI)

{                                     
	Boolean enableFlag = UpdateFileOutputTextPrint (pCmdUI);
	
	enableFlag = (enableFlag && GetTextBufferLength () != 0); 
	                                                                               
	pCmdUI->Enable (enableFlag);        
	
}	// end "OnUpdateFilePrint"



void CMTextView::OnUpdateFilePrintPreview (
				CCmdUI* 								pCmdUI)

{                                                                                                                                                  
	pCmdUI->Enable (GetTextBufferLength () != 0); 
	                      
}	// end "OnUpdateFilePrintPreview"



void CMTextView::OnUpdateFilePrintSetup (
				CCmdUI* 								pCmdUI)

{                                                                                                                     
	pCmdUI->Enable (TRUE);
	
}	// end "OnUpdateFilePrintSetup"



void CMTextView::OnUpdateFileSave (
				CCmdUI* 								pCmdUI)

{                                                                                                                     
	pCmdUI->Enable (UpdateFileSave (pCmdUI));
	
}	// end "OnUpdateFileSave"



void CMTextView::OnUpdateFileSaveAs (
				CCmdUI* 								pCmdUI)

{                       
	Boolean enableFlag = UpdateFileOutputTextSaveAs (pCmdUI);
	
	enableFlag = (enableFlag && GetTextBufferLength () != 0); 
	                                                                                             
	pCmdUI->Enable (enableFlag);            
	
}	// end "OnUpdateFileSaveAs"



BOOL CMTextView::PreCreateWindow (
				CREATESTRUCT&						cs)

{
	return CEditView::PreCreateWindow (cs);

}	// end "PreCreateWindow"
