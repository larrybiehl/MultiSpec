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
//	File:						WToolBar.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			04/16/2020
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	This file contains functions that relate to the
//								CMToolBar class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "WImageView.h"
#include "WMultiSpec.h"
#include "WToolBar.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMToolBar, CToolBar)
	//{{AFX_MSG_MAP (CToolBar)
	ON_WM_LBUTTONDBLCLK ()
	ON_WM_LBUTTONDOWN ()
	ON_WM_LBUTTONUP ()
	ON_WM_MOUSEMOVE ()
	ON_WM_RBUTTONDOWN ()
	ON_WM_RBUTTONUP ()
	//}}AFX_MSG_MAP

	ON_COMMAND_RANGE (ID_SHOWOVERLAYMENUITEMSTART, ID_SHOWOVERLAYMENUITEMEND, OnShowOverlay)
END_MESSAGE_MAP ()



CMToolBar::CMToolBar ()

{
	m_iButtonCapture = 0;
	m_overlayMenu = NULL;
	m_rightButtonFlag = FALSE;
	m_optionOverlayFlag = FALSE;

	TRY
		{
		m_overlayMenu = new CMenu ();

		if (m_overlayMenu->CreatePopupMenu ())
			{
			m_overlayMenu->AppendMenu (
								MF_STRING, ID_SHOWOVERLAYMENUITEMSTART, _T("No overlays"));
			m_overlayMenu->AppendMenu (
								MF_STRING, ID_SHOWOVERLAYMENUITEMSTART+1, _T("All overlays"));
			m_overlayMenu->AppendMenu (
								MF_SEPARATOR, ID_SHOWOVERLAYMENUITEMSTART+2, _T(""));

			}	// end "if (m_overlayMenu->CreatePopupMenu ())"

		}	// end "TRY"

	CATCH_ALL (e)
		{
		MemoryMessage (0, kObjectMessage);
		}
	
	END_CATCH_ALL
	
}	// end "CMToolBar"



CMToolBar::~CMToolBar ()

{  
   if (m_overlayMenu != NULL)
		{
		m_overlayMenu->DestroyMenu ();

		delete m_overlayMenu;

		}	// end "if (m_overlayMenu != NULL)"
	
}	// end "~CMToolBar"



UINT CMToolBar::ConvertToID (
				CPoint 								point)

{
	if ((point.x>=209)&&(point.x<=230))
																							return 11;

	else if ((point.x>=231)&&(point.x<=252))
																							return 12;

	else if ((point.x>=262)&&(point.x<=283))
																							return 14;
	
	return 0;

}	// end "ConvertToID"



void CMToolBar::IsZoomPressed (
				CPoint 								point)

{
	RECT									itemRect;


	//if ((point.x>=186) &&
	//	 		(point.x<=283) && // 252
	//	 				(point.y<=24))
	//	m_iButtonCapture = ConvertToID (point);

	m_iButtonCapture = 0;
	GetItemRect (11, &itemRect);
	if (point.y <= itemRect.bottom)
		{
		if (point.x >= itemRect.left &&
								point.x <= itemRect.right)
			m_iButtonCapture = 11; 
		
		else // point.x not in button 11
			{
			GetItemRect (12, &itemRect);
			if (point.x >= itemRect.left &&
				point.x <= itemRect.right)
				m_iButtonCapture = 12;

			else
				{
				GetItemRect (14, &itemRect);
				if (point.x >= itemRect.left &&
					point.x <= itemRect.right)
					m_iButtonCapture = 14;

				}
			}
		}

}	// end "IsZoomPressed"



void CMToolBar::OnLButtonDblClk (
				UINT 									nFlags,
				CPoint 								point)

{
	if (gProcessorCode == 0)
		{
		//OnLButtonUp (nFlags, point);
		CToolBar::OnLButtonDblClk (nFlags, point);

		IsZoomPressed (point);
	
		if (m_iButtonCapture == 11 || m_iButtonCapture == 12)
			{
			if (nFlags & MK_CONTROL)                 		
				gActiveImageViewCPtr->SetControlKeyFlag (TRUE);
		                        
			SInt16 itemID = GetItemID (m_iButtonCapture);
			((CMultiSpecApp*)AfxGetApp())->SetZoomCode (itemID);
		
			gNextTime = GetTickCount ();                          
			((CMultiSpecApp*)AfxGetApp())->m_nextControlTime = 0;

			}	// end "if (m_iButtonCapture == 11 || ...)"

		}	// end "if (gProcessorCode == 0)"
	
}	// end "OnLButtonDblClk"



void CMToolBar::OnLButtonDown (
				UINT 									nFlags,
				CPoint 								point)

{      
	if (gProcessorCode == 0)
		{
		CToolBar::OnLButtonDown (nFlags, point);

		IsZoomPressed (point);

		if (m_iButtonCapture == 11 || m_iButtonCapture == 12)
			{
			if (nFlags & MK_CONTROL)                 		
				gActiveImageViewCPtr->SetControlKeyFlag (TRUE);
		                        
			SInt16 itemID = GetItemID (m_iButtonCapture);
			((CMultiSpecApp*)AfxGetApp())->SetZoomCode (itemID);

			if (nFlags & MK_SHIFT)
				((CMultiSpecApp*)AfxGetApp())->SetControlDelayFlag (FALSE);
		
			gNextTime = GetTickCount ();                          
			((CMultiSpecApp*)AfxGetApp())->m_nextControlTime = 0;

			}	// end "if (m_iButtonCapture == 11 || ...)"

		else if (m_iButtonCapture == 14)
			{
			tagRECT			buttonRectangle;

			m_optionOverlayFlag = FALSE;
			if (GetKeyState (VK_RBUTTON) < 0 || GetKeyState (VK_SHIFT) & 0x8000)
				m_optionOverlayFlag = TRUE;
		
			SetUpWindowOverlayPopUpMenu ((Handle)m_overlayMenu, m_optionOverlayFlag);

			if (m_overlayMenu->GetMenuItemCount () > 3)
				{
				GetItemRect (m_iButtonCapture, &buttonRectangle);
				ClientToScreen (&buttonRectangle);

				if (!m_overlayMenu->TrackPopupMenu (TPM_LEFTALIGN, 
																buttonRectangle.left, 
																buttonRectangle.bottom, 
																this, 
																NULL))
					{
							// No selection is made;
							// Seems like we never get here though

					}	// end "if (!m_overlayMenu->TrackPopupMenu (..."
				
				ReleaseCapture ();
				ClearMenuItems (m_overlayMenu, 3);

				}	// end "if (m_overlayMenu->GetMenuItemCount () > 3)"

			m_iButtonCapture = 0;
			
			//OnLButtonUp (nFlags, point);

			}	// end "else if (m_iButtonCapture == 14)"

		}	// end "if (gProcessorCode == 0)"

}	// end "OnLButtonDown"



void CMToolBar::OnLButtonUp (
				UINT 									nFlags,
				CPoint 								point)

{
	if (gProcessorCode == 0)
		{
		if (m_iButtonCapture == 14)
			{
			ClearMenuItems (m_overlayMenu, 3);
			ReleaseCapture ();

			m_rightButtonFlag = FALSE;

			}	// end "if (m_iButtonCapture == 14)"
		
		m_iButtonCapture = 0;
		
		((CMultiSpecApp*)AfxGetApp())->SetZoomCode (0);
		((CMultiSpecApp*)AfxGetApp())->SetControlDelayFlag (TRUE);
		gActiveImageViewCPtr->SetControlKeyFlag (FALSE);
	
		CToolBar::OnLButtonUp (nFlags, point);

		}	// end "if (gProcessorCode == 0)"
	
}	// end "OnLButtonUp"



void CMToolBar::OnMouseMove (
				UINT 									nFlags,
				CPoint 								point)

{             
	UINT	 								itemID,
											itemStyle;
						
	int									itemImage;
		

	CToolBar::OnMouseMove (nFlags, point);
	
	if (m_iButtonCapture == 11 || m_iButtonCapture == 12)
		{
		GetButtonInfo (m_iButtonCapture, itemID, itemStyle, itemImage);
		
		if (!(itemStyle & TBBS_PRESSED))
			itemID = 0;
			                                            
		((CMultiSpecApp*)AfxGetApp())->SetZoomCode (itemID);
		
		}	// end "if (m_iButtonCapture == 11 || ..."
	
}	// end "OnMouseMove"



void CMToolBar::OnRButtonDown (
				UINT 									nFlags,
				CPoint 								point)

{           
	if (gProcessorCode == 0)
		{
		IsZoomPressed (point);
	
		if (m_iButtonCapture == 11 || m_iButtonCapture == 12)
			{                                                            		
			((CMultiSpecApp*)AfxGetApp())->SetControlDelayFlag (FALSE); 
		
			}	// end "if (m_iButtonCapture == 11 || ...)"

		else if (m_iButtonCapture == 14)
			{
			tagRECT			buttonRectangle;

			SetCapture ();
			m_optionOverlayFlag = TRUE;
			SetUpWindowOverlayPopUpMenu ((Handle)m_overlayMenu, m_optionOverlayFlag);

			if (m_overlayMenu->GetMenuItemCount () > 3)
				{
				GetItemRect (m_iButtonCapture, &buttonRectangle);
				ClientToScreen (&buttonRectangle);
			
				m_rightButtonFlag = TRUE;
				if (!m_overlayMenu->TrackPopupMenu (TPM_LEFTALIGN, 
																buttonRectangle.left, 
																buttonRectangle.bottom, 
																this, 
																NULL))
					{
							// No selection is made; turn the right button flag off.
							// Seems we never get to this point.

					} 
				
				ReleaseCapture ();
				ClearMenuItems (m_overlayMenu, 3);

				}	// end "if (m_overlayMenu->GetMenuItemCount () > 3)"

			m_iButtonCapture = 0;

			}	// end "else if (m_iButtonCapture == 14)"

		}	// end "if (gProcessorCode == 0)"
	
}	// end "OnRButtonDown"



void CMToolBar::OnRButtonUp (
				UINT 									nFlags,
				CPoint 								point)

{                            
	if (m_iButtonCapture != 14)
		{
		((CMultiSpecApp*)AfxGetApp())->SetControlDelayFlag (TRUE);

		m_rightButtonFlag = FALSE;

		}	// end "if m_iButtonCapture != 14"

	else	// m_iButtonCapture == 14
		{
		if (!m_rightButtonFlag)
			{
			ClearMenuItems (m_overlayMenu, 3);
			ReleaseCapture ();
			
			}	// end "if (!m_rightButtonFlag)"

		}	// end "else m_iButtonCapture == 14"
	
	//CToolBar::OnRButtonUp (nFlags, point);
	
}	// end "OnRButtonUp"



void CMToolBar::OnShowOverlay (
				UINT									menuID)

{
	UINT	 								itemID,
											itemStyle;
						
	int									itemImage;

	SInt32								selection;

	Boolean								continueFlag = TRUE,
											shiftKeyFlag;


	selection = menuID - ID_SHOWOVERLAYMENUITEMSTART + 1;

	ReleaseCapture ();
	
	shiftKeyFlag = FALSE;
	if (GetKeyState (VK_SHIFT) & 0x8000)
		shiftKeyFlag = TRUE;

	if (m_optionOverlayFlag)
		{
		continueFlag = OverlayControlDialog (gActiveImageWindowInfoH,
															(SInt16)(selection-4));

		}	// end "if (m_optionOverlayFlag)"
	
	if (continueFlag)
		DoShowOverlaySelection (gActiveImageViewCPtr, 
										gActiveImageWindowInfoH, 
										(SInt16)selection,
										m_optionOverlayFlag,
										shiftKeyFlag);

	m_optionOverlayFlag = FALSE;
	m_rightButtonFlag = FALSE;
	
	GetButtonInfo (14, itemID, itemStyle, itemImage);
		
	if (itemStyle & TBBS_PRESSED)
		{
		itemStyle -= TBBS_PRESSED;
		SetButtonInfo (14, itemID, itemStyle, itemImage);

		}	//end "if (itemStyle & TBBS_PRESSED)"

}	// end "OnShowOverlay"
