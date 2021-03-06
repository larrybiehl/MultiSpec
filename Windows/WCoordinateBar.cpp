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
//	File:						WCoordinateBar.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			03/07/2020
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	This file contains functions that relate to the
//								CMCoordinateBar class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "WCoordinateBar.h" 
#include "WImageView.h"



BEGIN_MESSAGE_MAP (CMCoordinateBar, CDialogBar)
	//{{AFX_MSG_MAP (CMCoordinateBar)
	ON_CBN_CLOSEUP (IDC_AreaUnitsCombo, OnCloseupAreaUnitsCombo)

	ON_CBN_DROPDOWN (IDC_AreaUnitsCombo, OnDropdownAreaUnitsCombo)

	ON_CBN_SELENDOK (IDC_AreaUnitsCombo, OnSelendokAreaUnits)
	ON_CBN_SELENDOK (IDC_DisplayUnitsCombo, OnSelendokDisplayUnits)

	ON_WM_SETCURSOR ()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()
 


CMCoordinateBar::CMCoordinateBar ()

{
	m_dialogFromPtr = NULL;
	m_dialogToPtr = NULL;
	m_areaUnitsCode = kNumberPixelsUnitsMenuItem;
	m_displayUnitsCode = kLineColumnUnitsMenuItem;
	m_displayUnitsListSelection = 0;
	m_areaUnitsListSelection = 0;

	TRY
		{
		m_dialogFromPtr = new CDataExchange (this, TRUE);
		m_dialogToPtr = new CDataExchange (this, FALSE);

		}

	CATCH_ALL (e)
		{
		MemoryMessage (0, kObjectMessage);

		}

	END_CATCH_ALL

}	// end "CMCoordinateBar"



CMCoordinateBar::~CMCoordinateBar ()

{
	if (m_dialogToPtr != NULL)
		delete m_dialogToPtr;

	if (m_dialogFromPtr != NULL)
		delete m_dialogFromPtr;

}	// end "~CMCoordinateBar ()"



void CMCoordinateBar::DoDataExchange (
				CDataExchange* 					pDX)

{
	CDialogBar::DoDataExchange (pDX);
	//{{AFX_DATA_MAP (CMEchoClassifyDialog)
	DDX_CBIndex (pDX, IDC_AreaUnitsCombo, m_areaUnitsListSelection);
	DDX_CBIndex (pDX, IDC_DisplayUnitsCombo, m_displayUnitsListSelection);
	//}}AFX_DATA_MAP
	
}	// end "DoDataExchange"



void CMCoordinateBar::OnCloseupAreaUnitsCombo ()

{  
 	/*
 	RECT									itemRect,
											rect;

	CComboBox*							comboBoxPtr;
	

			// Change size of area units popup box.

	comboBoxPtr = (CComboBox*)GetDlgItem (IDC_AreaUnitsCombo);
	comboBoxPtr->GetClientRect (&rect);
	itemRect.top = 8;
	itemRect.bottom = 27;
	itemRect.left = GetCoordinateViewScaleStart (gActiveImageWindowInfoH) - 20 - 8;
	itemRect.right = itemRect.left + 20;
	comboBoxPtr->MoveWindow (&itemRect, FALSE);
	*/
	//m_displayUnitsCode = GetCoordinateViewUnits (gActiveImageWindowInfoH);
	//DDX_CBIndex (m_dialogToPtr, IDC_DisplayUnitsCombo, m_displayUnitsCode);
	
}	// end "OnCloseupAreaUnitsCombo"



void CMCoordinateBar::OnDraw (
				CDC* 									pDC)

{
	SInt16								test;


	test = 1;

}	// end "OnDraw"



void CMCoordinateBar::OnDropdownAreaUnitsCombo ()

{
	/*
	RECT									itemRect,
											rect;

	CComboBox*							comboBoxPtr;
	DisplaySpecsPtr					displaySpecsPtr,


			// Change size of area units popup box.

	comboBoxPtr = (CComboBox*)GetDlgItem (IDC_AreaUnitsCombo);
	comboBoxPtr->SetDroppedWidth (100);

	comboBoxPtr->GetClientRect (&rect);
	itemRect.top = 8;
	itemRect.bottom = 27;
	itemRect.left = GetCoordinateViewScaleStart (gActiveImageWindowInfoH) - 80 - 8;
	itemRect.right = itemRect.left + 80;
	comboBoxPtr->MoveWindow (&itemRect, FALSE);
	 
	m_displayUnitsCode = GetCoordinateViewUnits (gActiveImageWindowInfoH);
	DDX_CBIndex (m_dialogToPtr, IDC_DisplayUnitsCombo, m_displayUnitsCode);
	*/
}	// end "OnDropdownAreaUnitsCombo"



void CMCoordinateBar::OnPaint ()

{
	RECT									updateRect;
	
	
	if (GetUpdateRect (&updateRect))
		{ 
			
		CPaintDC dc (this); // device context for painting 
		OnDraw (&dc);
		
		}	// end "	if (GetUpdateRect (&updateRect))"

	// Do not call CScrollView::OnPaint () for painting messages
	
}	// end "OnPaint"



void CMCoordinateBar::OnSelendokAreaUnits ()

{  
	CComboBox* 							comboBoxPtr;
	
	
	DDX_CBIndex (m_dialogFromPtr, 
						IDC_AreaUnitsCombo,
						m_areaUnitsListSelection);
					
			// Get the actual display units code.
			
	comboBoxPtr = (CComboBox*)GetDlgItem (IDC_AreaUnitsCombo);
						 
	m_areaUnitsCode = (SInt16)comboBoxPtr->GetItemData (m_areaUnitsListSelection);

	if (m_areaUnitsCode != GetCoordinateViewAreaUnits (gActiveImageWindowInfoH))
		{
		SetCoordinateViewAreaUnits (gActiveImageWindowInfoH, m_areaUnitsCode);

		UpdateCoordinateView (gActiveImageWindowInfoH);

		}	// end "if (m_displayUnitsCode != GetCoordinateViewUnits ())"
	
}	// end "OnSelendokAreaUnits"



void CMCoordinateBar::OnSelendokDisplayUnits ()

{  
	CComboBox* 							comboBoxPtr;
	
	
	DDX_CBIndex (m_dialogFromPtr, 
						IDC_DisplayUnitsCombo,
						m_displayUnitsListSelection);
					
			// Get the actual display units code.
			
	comboBoxPtr = (CComboBox*)GetDlgItem (IDC_DisplayUnitsCombo);
						 
	m_displayUnitsCode = 
					(SInt16)comboBoxPtr->GetItemData (m_displayUnitsListSelection);

	if (m_displayUnitsCode != GetCoordinateViewUnits (gActiveImageWindowInfoH))
		{
		SetCoordinateViewUnits (gActiveImageWindowInfoH, m_displayUnitsCode);

		UpdateCoordinateView (gActiveImageWindowInfoH);

		}	// end "if (m_displayUnitsCode != GetCoordinateViewUnits ())"
	
}	// end "OnSelendokDisplayUnits"



BOOL CMCoordinateBar::OnSetCursor (
				CWnd* 								pWnd,
				UINT 									nHitTest,
				UINT 									message)

{                                    
	if (gPresentCursor == kWait || gPresentCursor == kSpin)
		{
				// Make sure the wait cursor is on. The cursor may have just
				// move over the legend from being outside the window.
		
		AfxGetApp()->DoWaitCursor (0);
																						return (TRUE);

		}	// end "if (gPresentCursor != kArrow && ..."

	if (gPresentCursor != kArrow)
		{
		if (gActiveImageViewCPtr != NULL)                
			gActiveImageViewCPtr->UpdateCursorCoordinates ();
		                                                                
		gPresentCursor = kArrow; 		// Non image window cursor
		
		}	// end "if (gPresentCursor != kArrow)"  
		
	return CDialogBar::OnSetCursor (pWnd, nHitTest, message); 
	
}	// end "OnSetCursor" 


							
void CMCoordinateBar::OnUpdate (
				CView*								pSender,
				LPARAM								lHint,
				CObject*								pHint)
				
{  
			// The following is what is in "CView::OnUpdate" except that the
			// argument passed is TRUE.
	/*
	sprintf ((char*)&gTextString,
				"Image Invalidate%s",
				gEndOfLine);
	OutputString NULL,
					(char*)&gTextString, 
					0, 
					1, 
					TRUE);
	*/
	Invalidate (FALSE);

}	// end "OnUpdate"



void CMCoordinateBar::UpdateCoordinateView (
				Handle								windowInfoHandle)
				
{
	RECT									itemRect,
											rect;

	CComboBox* 							comboBoxPtr;

	double								dpiScale,
											maxScale;

	int									iDpi;

	UInt32								width;

	SInt16								numberChars;


	SetCoordinateViewLocationParameters (windowInfoHandle);

	CMImageView* imageViewCPtr = (CMImageView*)GetWindowPtr (windowInfoHandle);
	dpiScale = imageViewCPtr->m_dpiScale;

	
			// Adjust location of map unit list.

	comboBoxPtr = (CComboBox*)GetDlgItem (IDC_DisplayUnitsCombo);
	comboBoxPtr->GetClientRect (&itemRect);
	itemRect.left = (int)(dpiScale * 4);
	itemRect.right = (int)(itemRect.left + itemRect.right);
	itemRect.top = (int)(dpiScale * 8);
	itemRect.bottom = (int)(itemRect.top + itemRect.bottom);
	comboBoxPtr->MoveWindow (&itemRect, FALSE);
	
			// Adjust location of line symbol.

	comboBoxPtr = (CComboBox*)GetDlgItem (IDC_LineSymbol);
	comboBoxPtr->GetClientRect (&itemRect);
	itemRect.top = (int)(dpiScale*4);
	itemRect.bottom = itemRect.top + (int)(dpiScale*12);
	itemRect.left = GetCoordinateViewCursorStart (windowInfoHandle) - 15;
	itemRect.right = itemRect.left + (int)(dpiScale*12);
	comboBoxPtr->MoveWindow (&itemRect, FALSE);

			// Adjust location of column symbol.

	comboBoxPtr = (CComboBox*)GetDlgItem (IDC_ColumnSymbol);
	//itemRect.top = 18;
	//itemRect.bottom = 30;
	itemRect.top = (int)(dpiScale*18);
	itemRect.bottom = (int)(dpiScale*30);
	comboBoxPtr->MoveWindow (&itemRect, FALSE);

			// Adjust size of cursor line box.

	comboBoxPtr = (CComboBox*)GetDlgItem (IDC_CursorLine);
	width = GetCoordinateViewSelectionStart (windowInfoHandle) -
					GetCoordinateViewCursorStart (windowInfoHandle) -
					10;
	itemRect.top = (int)(dpiScale*4);
	itemRect.bottom = (int)(dpiScale*16);
	itemRect.left = GetCoordinateViewCursorStart (windowInfoHandle);
	itemRect.right = itemRect.left + width;
	comboBoxPtr->MoveWindow (&itemRect, FALSE);

			// Adjust size of cursor column box.

	comboBoxPtr = (CComboBox*)GetDlgItem (IDC_CursorColumn);
	itemRect.top = (int)(dpiScale*18);
	itemRect.bottom = (int)(dpiScale*30);
	comboBoxPtr->MoveWindow (&itemRect, FALSE);

			// Move separator box 1.

	comboBoxPtr = (CComboBox*)GetDlgItem (IDC_Separator1);
	itemRect.top = (int)(dpiScale*5);
	itemRect.bottom = (int)(dpiScale*30);
	itemRect.left = itemRect.right + (int)(dpiScale*4);
	itemRect.right = itemRect.left + (int)(dpiScale*3);
	comboBoxPtr->MoveWindow (&itemRect, FALSE);

			// Adjust size of selection line box.

	comboBoxPtr = (CComboBox*)GetDlgItem (IDC_SelectionLine);
	width = GetCoordinateViewNumberPixelsStart (windowInfoHandle) -
					GetCoordinateViewSelectionStart (windowInfoHandle) -
					10;
	itemRect.top = (int)(dpiScale*4);
	itemRect.bottom = (int)(dpiScale*16);
	itemRect.left = GetCoordinateViewSelectionStart (windowInfoHandle);
	itemRect.right = itemRect.left + (int)(dpiScale*width);
	comboBoxPtr->MoveWindow (&itemRect, FALSE);

			// Adjust size of selection column box.

	comboBoxPtr = (CComboBox*)GetDlgItem (IDC_SelectionColumn);
	itemRect.top = (int)(dpiScale*18);
	itemRect.bottom = (int)(dpiScale*30);
	comboBoxPtr->MoveWindow (&itemRect, FALSE);

			// Adjust size and location of "number pixels box" box.

	comboBoxPtr = (CComboBox*)GetDlgItem (IDC_NumberPixelsPrompt);
	width = GetCoordinateViewAreaPopupStart (windowInfoHandle) -
					GetCoordinateViewNumberPixelsStart (windowInfoHandle) -
					4;
	itemRect.top = (int)(dpiScale*4);
	itemRect.bottom = (int)(dpiScale*16);
	itemRect.left = GetCoordinateViewNumberPixelsStart (windowInfoHandle);
	itemRect.right = itemRect.left + width;
	comboBoxPtr->MoveWindow (&itemRect, FALSE);

			// Adjust size and location of area box.

	comboBoxPtr = (CComboBox*)GetDlgItem (IDC_NumberPixels);
	itemRect.top = (int)(dpiScale*18);
	itemRect.bottom = (int)(dpiScale*30);
	comboBoxPtr->MoveWindow (&itemRect, FALSE);

			// Move area units popup box.

	comboBoxPtr = (CComboBox*)GetDlgItem (IDC_AreaUnitsCombo);
	comboBoxPtr->GetClientRect (&rect);
	itemRect.top = (int)(dpiScale*5);
	itemRect.bottom = (int)(dpiScale*200);
	itemRect.left = GetCoordinateViewScaleStart (windowInfoHandle) - rect.right - 8;
	itemRect.right = itemRect.left + rect.right;
	comboBoxPtr->MoveWindow (&itemRect, FALSE);
	comboBoxPtr->SetDroppedWidth ((UINT)(dpiScale*100));

			// Move separator box 2.

	comboBoxPtr = (CComboBox*)GetDlgItem (IDC_Separator2);
	itemRect.top = (int)(dpiScale*5);
	itemRect.bottom = (int)(dpiScale*30);
	itemRect.left = GetCoordinateViewScaleStart (windowInfoHandle) - 5;
	itemRect.right = itemRect.left + (int)(dpiScale*3);
	comboBoxPtr->MoveWindow (&itemRect, FALSE);

			// Adjust size and location of "scale" box.
			//		First, get width of maximum possible scale value.

	maxScale = GetScale (windowInfoHandle,
									GetMaxNumberColumns (windowInfoHandle),
									gMinMagnification);
	
	if (maxScale >= 1)
		{
		numberChars = sprintf ((char*)&gTextString2,
										"%.0f",
										maxScale);

		numberChars = InsertCommasInNumberString (
									(char*)&gTextString2,
									numberChars,
									-1,
									numberChars);
			
		}	// end "if (scale >= 1)"
	
	else    // maxScale < 1
		{
		numberChars = sprintf ((char*)&gTextString2,
										"%f",
										maxScale);
		
		}	// end "else scale < 1"
	
	numberChars = sprintf ((char*)&gTextString,
									"1:%*s",
									-17,
									(char*)gTextString2);
	
	width = TextWidth (gTextString, 0, numberChars) + 10;
	width = MAX (50, width);

	comboBoxPtr = (CComboBox*)GetDlgItem (IDC_ScalePrompt);
	itemRect.top = (int)(dpiScale * 4);
	itemRect.bottom = (int)(dpiScale * 16);
	itemRect.left = GetCoordinateViewScaleStart (windowInfoHandle);
	itemRect.right = itemRect.left + width;
	comboBoxPtr->MoveWindow (&itemRect, FALSE);

			// Adjust size and location of scale value box.

	comboBoxPtr = (CComboBox*)GetDlgItem (IDC_Scale);
	itemRect.top = (int)(dpiScale * 18);
	itemRect.bottom = (int)(dpiScale * 30);
	comboBoxPtr->MoveWindow (&itemRect, FALSE);

	Invalidate ();

	UpdateSelectionCoordinates (windowInfoHandle);
	ShowSelection (windowInfoHandle);
	
}	// end "UpdateCoordinateView"
