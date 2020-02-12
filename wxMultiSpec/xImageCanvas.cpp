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
//	File:						xImageCanvas.cpp : class implementation file
//	Class Definition:		xImageCanvas.h
//
//	Authors:					Larry L. Biehl, Abdur Rachman Maud
//
//	Revision date:			01/11/2020
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the 
//								CMImageCanvas class.
//
/* Template for debugging
	int numberChars = sprintf ((char*)gTextString3,
				" xImageCanvas:: (): %s",
				gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "xImageCanvas.h"
#include "xImageDoc.h"
#include "wx/dcbuffer.h"
#include "wx/event.h"
#include "wx/utils.h"
#include "xTools.h"
#include "xGraphView.h"

Boolean	s_repeatKeyFlag = FALSE;

IMPLEMENT_DYNAMIC_CLASS (CMImageCanvas, wxScrolledWindow)

wxSize CMImageCanvas::s_scrollingIncrement = wxSize (1, 1);	// 10, 10

BEGIN_EVENT_TABLE (CMImageCanvas, wxScrolledWindow)
	EVT_CHAR_HOOK (CMImageCanvas::OnCharHook)
	EVT_CONTEXT_MENU (CMImageCanvas::OnContextMenu)
	EVT_ERASE_BACKGROUND (CMImageCanvas::OnEraseBackground)
	EVT_IDLE (CMImageCanvas::OnIdle)
   EVT_KEY_DOWN (CMImageCanvas::OnKeyDown)
   EVT_KEY_UP (CMImageCanvas::OnKeyUp)
	EVT_LEAVE_WINDOW (CMImageCanvas::OnLeaveImageWindow)
   EVT_LEFT_DCLICK (CMImageCanvas::OnDlbClk)
	EVT_LEFT_DOWN (CMImageCanvas::OnLeftDown)
	EVT_LEFT_UP (CMImageCanvas::OnLeftUp)
	EVT_MOTION (CMImageCanvas::OnMouseMove)
	EVT_MOUSEWHEEL (CMImageCanvas::OnMouseWheel)
	EVT_PAINT (CMImageCanvas::OnPaint)
	EVT_SCROLLWIN (CMImageCanvas::OnScrollChanged)
	EVT_SET_CURSOR (CMImageCanvas::OnCursorChange)
END_EVENT_TABLE ()


CMImageCanvas::CMImageCanvas ()

{
   Init ();
	
}	// end "CMImageCanvas"



CMImageCanvas::CMImageCanvas (
				wxWindow *							parent,
				wxWindowID							id,
				const wxPoint&						pos,
				const wxSize&						size,
				long									style,
				const wxString&					name)

{
   Init ();
   Create (parent, id, pos, size, style, name);
	
			// The initial cursor for the window will be an arrow until an image
			// is loaded in the window.
	
	SetCursor (wxCursor (wxCURSOR_ARROW));
	
   wxSize initsize = parent->GetClientSize ();
   SetVirtualSize (initsize);
   m_size_h = initsize.GetHeight ();
   m_size_w = initsize.GetWidth ();
   SetScrollRate (1, 1);	// 10, 10
   m_displayImageFlag = true;
   m_featureListShowFlag = false;
   m_dataListShowFlag = false;
   m_LastSelectionPoint = wxPoint (0, 0);
	
   s_repeatKeyFlag = FALSE;
	
			// This is not needed since both MacOS and gtk2 versions are doing
			// double buffered by default.
	
   //SetBackgroundStyle (wxBG_STYLE_PAINT);
	
	EnableScrolling (true, true);
	
}	// end "CMImageCanvas"



CMImageCanvas::~CMImageCanvas (void)

{
	gPresentCursor = -1;

}	// end "~CMImageCanvas"



void CMImageCanvas::AdjustScrollBars (
				bool									callScrollFlag)

{
			// image sampled size
	
   wxSize imageSize = GetCurrentSize ();
	
			// old scroll position
	
   wxPoint pt = GetScrollPosition ();

	SetVirtualSize (imageSize);
	if (callScrollFlag)
		DoScroll (pt.x, pt.y);
	
}	// end "AdjustScrollBars"



bool CMImageCanvas::AutoScroll (
				wxPoint								currentPos,
				wxPoint								scrollPos)

{
   bool scrolledFlag = false;
   if (HasCapture ())
		{
				// If mouse is captured set rect coords if inside the image and
				// autoscroll if posible get image display rectangle
		
      wxRect displayRect = GetImageDisplayRect (scrollPos);

      wxSize oldSelection (m_TR.x - m_BL.x, m_TR.y - m_BL.y);

				// check if the current drag position is inside the image - do not allow
				// to draw rectangle out of the image
		
      m_TR.x = wxMin (wxMax (currentPos.x, displayRect.GetLeft ()),
								displayRect.GetRight ());
      m_TR.y = wxMin (wxMax (currentPos.y, displayRect.GetTop ()),
								displayRect.GetBottom ());

				// Check current drag position and update scroll regularly
		
      if (currentPos.x <= 0)
			{
         DoScroll (wxMax (
         				scrollPos.x - CMImageCanvas::s_scrollingIncrement.GetWidth (), 0),
         				-1);
         m_BL.x += (scrollPos.x - GetScrollPosition().x);
         scrolledFlag = true;
			
			}	// end "if (currentPos.x <= 0)"
		
      if (currentPos.y <= 0)
			{
         DoScroll (-1, wxMax (
         				scrollPos.y - CMImageCanvas::s_scrollingIncrement.GetHeight (), 0));
         m_BL.y += (scrollPos.y - GetScrollPosition().y);
         scrolledFlag = true;
			
			}	// end "if (currentPos.y <= 0)"
		
      if (currentPos.x >= GetClientSize().GetWidth ())
			{
         DoScroll (scrollPos.x + CMImageCanvas::s_scrollingIncrement.GetWidth (), -1);
         m_BL.x -= (GetScrollPosition().x - scrollPos.x);
         scrolledFlag = true;
			
			}	// end "if (currentPos.x >= GetClientSize().GetWidth ())"
		
      if (currentPos.y >= GetClientSize().y)
			{
         DoScroll (-1, scrollPos.y + CMImageCanvas::s_scrollingIncrement.GetHeight ());
         m_BL.y -= (GetScrollPosition().y - scrollPos.y);
         scrolledFlag = true;
			
			}	// end "if (currentPos.y >= GetClientSize().y)"
		
		CMTool* pTool = CMTool::FindTool (CMTool::c_toolType);
		if (pTool->s_selectType != kPolygonType)
			m_Selection = wxRect (wxMin (m_TR.x, m_BL.x),
											wxMin (m_TR.y, m_BL.y),
											abs (m_TR.x - m_BL.x),
											abs (m_TR.y - m_BL.y));
		
		if (scrolledFlag)
			m_View->UpdateOffscreenMapOrigin ();
		
      return scrolledFlag;
		
		}	// end "if (HasCapture ())"
	
	else	// !HasCapture ()
		return true;
	
}	// end "AutoScroll"



void CMImageCanvas::CanvasScroll (
				int									x,
				int									y)

{
   DoScroll (x, y);
	
}	// end "CanvasScroll"



wxPoint CMImageCanvas::ClientToImage (
				const wxPoint&						pos)

{
   wxPoint scrollPos = GetScrollPosition ();
   wxRect imageRect = GetImageDisplayRect (scrollPos);
   return wxPoint (pos.x - imageRect.GetLeft (), pos.y - imageRect.GetTop ());
	
}	// end "ClientToImage"



bool CMImageCanvas::Create (
				wxWindow* 							parent,
				wxWindowID 							id,
        		const wxPoint& 					pos,
        		const wxSize& 						size,
        		long 									style,
        		const wxString& 					name)

{
   bool res = wxScrolledWindow::Create (
   									parent, id, pos, parent->GetClientSize (), style, name);
   if (res)
      AdjustScrollBars (true);

   return res;
	
}	// end "Create"



void CMImageCanvas::DoMouseWheel (
				wxMouseWheelAxis 					axis,
				int 									rotation,
				bool 									ctrlDown)

{
	wxPoint								scrollPos;
	wxRect 								displayRect;
	
	
	if (ctrlDown) // Command key for MacOS
		{
					// Zoom the image when the Ctrl (Command for MacOS) key is down
		
		if (axis == wxMOUSE_WHEEL_VERTICAL)
			{
			UInt32 currentTickCount = GetTickCount ();
			if (currentTickCount >= GetMainFrame()->GetNextControlTime ())
				{
				GetMainFrame()->SetNextControlTime (gControlOffset/4);
			
				if (rotation > 0)
					{
					GetMainFrame()->SetZoomCode (ID_ZOOM_IN);
					m_View->ZoomIn ();
					GetMainFrame()->SetZoomCode (0);
					
					}	// end "if (rotation > 0)"
				
				else	// rotation <= 0
					{
					GetMainFrame()->SetZoomCode (ID_ZOOM_OUT);
					m_View->ZoomOut ();
					GetMainFrame()->SetZoomCode (0);
					
					}	// end "else rotation <= 0"
				
				}	// end "if (currentTickCount >= GetMainFrame()->GetNextControlTime ())"
			
			}	// end "if (axis == wxMOUSE_WHEEL_VERTICAL)"
		
		}	// end "if (ctrlDown)"
	
	else	// !ctrlDown
		{
				// Scroll the image
	
		scrollPos = GetScrollPosition ();

		if (axis == wxMOUSE_WHEEL_HORIZONTAL)
			{
			if (HasScrollbar (wxHORIZONTAL))
				scrollPos.x += rotation;

			}	// end "if (axis == wxMOUSE_WHEEL_HORIZONTAL)"
		
		else	// otherwise scroll the window vertically
			{
			if (HasScrollbar (wxVERTICAL))
				scrollPos.y -= rotation;
			
			}	// end "else otherwise just scroll the window"
		
		if (scrollPos.x != 0 || scrollPos.y != 0)
			{
			displayRect = m_View->m_Canvas->GetImageDisplayRect (scrollPos);
			m_View->m_Canvas->DoScroll (scrollPos.x, scrollPos.y);
			m_View->ScrollChanged ();
		
			m_View->m_Canvas->Refresh (false, &displayRect);
			
			}	// end "if (scrollPos.x != 0 || scrollPos.y != 0)"
		
		}	// end "else !ctrlDown"
	
}	// end "DoMouseWheel"



void CMImageCanvas::EraseBackground (
				bool									eraseFlag)

{
   m_displayImageFlag = !eraseFlag;
   if (eraseFlag)
		{
      wxSize psize = GetParent()->GetClientSize ();
      SetVirtualSize (psize);
      m_size_h = psize.GetHeight ();
      m_size_w = psize.GetWidth ();
		
		}	// end "if (eraseFlag)"
	
   Refresh ();
	
}	// end "EraseBackground"



CMImageView* CMImageCanvas::GetView ()

{
   return m_View;
	
}	// end "GetView"



wxSize CMImageCanvas::GetCurrentSize ()

{
	if (!m_View)
      return wxSize (0, 0);
	
   double scale = m_View->m_Scale;
   if (m_View->GetScaledBitmap().IsOk ())
      return wxSize (m_View->GetScaledBitmap().GetWidth () * scale,
							m_View->GetScaledBitmap().GetHeight () * scale);
	
	else if (m_View->ImageWindowIsAvailable ())
		{
		CMImageWindow* imageWindowCPtr = m_View->GetImageWindowCPtr ();
		WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
													imageWindowCPtr->GetWindowInfoHandle ());
      return wxSize (windowInfoPtr->maxNumberColumns * scale,
							windowInfoPtr->maxNumberLines * scale);
		
		}	// end "else if (m_View->ImageWindowIsAvailable ())"
	
	else
      return wxSize (0, 0);
	
}	// end "GetCurrentSize"



wxRect CMImageCanvas::GetImageDisplayRect (
				const wxPoint&						scrollPos)

{
   wxSize currentSize = GetCurrentSize ();
   wxPoint ptTest (0, 0);
	
			// calculate actual image display rectangle if centered
	
	return wxRect (ptTest, currentSize);
	
}	// end "GetImageDisplayRect"



wxPoint CMImageCanvas::GetLastSelectionPoint ()

{
   return m_LastSelectionPoint;
	
}	// end "GetLastSelectionPoint"



wxPoint CMImageCanvas::GetScrollPosition ()

{
   return wxPoint (GetScrollPos (wxHORIZONTAL), GetScrollPos (wxVERTICAL));
	
}	// end "GetScrollPosition"



void CMImageCanvas::Init (void)

{
   m_View = NULL;
   m_scrolledFlag = FALSE;
	
	gPresentCursor = -1;
	
}	// end "Init"



void CMImageCanvas::OnCharHook (
				wxKeyEvent&							event)

{
   CMTool* pTool = CMTool::FindTool (CMTool::c_toolType);
	
	if (pTool != NULL)
		{
		int keyCode = event.GetKeyCode ();
		
		if (keyCode == WXK_RETURN || keyCode == WXK_NUMPAD_ENTER)
			pTool->OnCharHook ();
		
		else if (keyCode == WXK_DELETE)
			{
			ClearSelectionArea (gActiveImageViewCPtr);
			ShowGraphSelection ();
			m_LastSelectionPoint = wxPoint (0, 0);
			
			if (gSelectionGraphViewCPtr != NULL)
				gSelectionGraphViewCPtr->ResetListControls ();
			
			}	// end "else if (keyCode == WXK_DELETE)"
		
		else if (gPresentCursor == kBlinkOpenCursor2 ||
																gPresentCursor == kBlinkShutCursor2)
			{
			event.Skip (false);
			
			}	// end "else if (gPresentCursor == kBlinkOpenCursor2 || ..."
		
		else
			event.Skip ();
		
		}	// end "if (pTool != NULL)"
	
	else	// pTool == NULL
		{
		event.Skip (true);

		}	// end "else pTool == NULL"
	
}	// end "OnCharHook"



void CMImageCanvas::OnContextMenu (
				wxContextMenuEvent& 						event)

{
				// Note that an context menu event does not seem to come from the
				// MultiSpec Online version (linux - gtk2) implementation.
	
#if defined multispec_wxmac
	wxPoint								screenPosition,
											cursorPosOnImage; // unit: window point
	

	if (gPresentCursor == kBlinkOpenCursor2)
		{
		if (gActiveImageViewCPtr != m_View)
					// Force this image window to be active.
			m_View->Activate (true);
		
		SetFocus ();
      screenPosition = ScreenToClient (wxGetMousePosition ());
		cursorPosOnImage = ClientToImage (screenPosition);
	
		LongPoint cursorPoint;
		cursorPoint.h = (SInt32)cursorPosOnImage.x;
		cursorPoint.v = (SInt32)cursorPosOnImage.y;

				// This get called because the control key is down.
		
		ThematicImageWBlink (cursorPoint, 2);
		
		event.Skip (false);
		return;
		
		}	// end "if (gPresentCursor == kBlinkOpenCursor2)"
#endif

	event.Skip ();
	
}	// end "OnContextMenu"



void CMImageCanvas::OnCursorChange (
				wxSetCursorEvent& 						event)

{
			// This seems to need to be called on linux (gtk) system to get the cursor to
			// change.
	
	if (gPresentCursor == kBlinkOpenCursor2)
		{
		MSetCursor (kBlinkOpenCursor2);
		event.Skip (false);
		
		}	// end "if (gPresentCursor == kBlinkOpenCursor2)"
	
	else if (gPresentCursor == kBlinkShutCursor2)
		{
		MSetCursor (kBlinkShutCursor2);
		event.Skip (false);
		
		}	// end "if (gPresentCursor == kBlinkOpenCursor2)"
	
	else	// gPresentCursor != kBlinkOpenCursor2 || kBlinkShutCursor2
		event.Skip ();
	
}	// end "OnCursorChange"



void CMImageCanvas::OnDlbClk (
				wxMouseEvent&						event)

{
   if (m_View->CheckIfOffscreenImageExists ())
		{
		if (gPresentCursor == kBlinkOpenCursor2 || gPresentCursor == kBlinkShutCursor2)
			{
			if (event.LeftIsDown ())
				{
				if (gPresentCursor != kBlinkShutCursor2)
					{
					LongPoint 							cursorPoint;
					wxPoint								cursorPosOnImage;
				
					SetCursor (wxCursor (gActiveImageViewCPtr->m_frame->m_blinkShutCursor));
					gPresentCursor = kBlinkShutCursor2;
					
      			cursorPosOnImage = ClientToImage (event.GetPosition ());
					cursorPoint.h = (SInt32)cursorPosOnImage.x;
					cursorPoint.v = (SInt32)cursorPosOnImage.y;

					SInt16 code = 1;
					if (wxGetKeyState (WXK_CONTROL))
						code = 2;
					
					else if (wxGetKeyState (WXK_ALT))
						code = 4;
				
					ThematicImageWBlink (cursorPoint, code);
					
					}	// end "if (gPresentCursor != kBlinkShutCursor1)"
				
				}	// end "if (currentMouseState.LeftIsDown ())"
			
			else	// left is up
				{
				if (gPresentCursor != kBlinkOpenCursor2)
					{
					SetCursor (wxCursor (gActiveImageViewCPtr->m_frame->m_blinkOpenCursor));
					gPresentCursor = kBlinkOpenCursor2;
					
					}	// end "if (gPresentCursor != kBlinkOpenCursor2)"
				
				}	// end "else left is up"
			
			}	// end "else if (gPresentCursor == kBlinkOpenCursor2 || ..."
		
		else	// gPresentCursor != kBlinkOpenCursor2 && gPresentCursor != ...
			{
      	CMTool* pTool = CMTool::FindTool (CMTool::c_toolType);
      	if (pTool != NULL)
         	pTool->OnLButtonDblClk (m_View, 0);
			
			else
				event.Skip ();
			
			}	// end "else gPresentCursor != kBlinkOpenCursor2 && ..."
		
		}	// end "if (m_View->CheckIfOffscreenImageExists ())"
	
}	// end "OnDlbClk"



void CMImageCanvas::OnEraseBackground (
				wxEraseEvent&						event)

{

}	// end "OnEraseBackground"



void CMImageCanvas::OnIdle (
				wxIdleEvent&						event)

{
   do
		{
		/*
				// This was used to get the 'eye closed' cursor in MultiSpec Online
				// which uses GTK-2. It did not help get this operation to work.
				// For now I think that one cannot change the cursor in a mouse down
				// operation.
		if (m_doBlinkOperationFlag)
			{
			m_doBlinkOperationFlag = false;
			MSetCursor (kBlinkShutCursor2);
			CheckSomeEvents (0);
			ThematicImageWBlink (m_cursorPoint, m_blinkCode);
			
			}	// end "if (m_doBlinkOperationFlag)"
		*/
      if (!HasCapture ())
			break;

      		// Get scroll position
		
      wxPoint scrollPos = GetScrollPosition ();
		
      		// Get mouse in client coordinates
		
      wxPoint currentPos = ScreenToClient (wxGetMousePosition ());
      
      		// Auto scroll
      		// Check current drag position and update scroll regularly
		
      if (AutoScroll (currentPos, scrollPos))
      	{
   		m_scrolledFlag = TRUE;
         event.RequestMore ();
			
         }	// end "if (AutoScroll (currentPos, scrollPos))"
	
		}	while (false);
	
}	// end "OnIdle"



void CMImageCanvas::OnKeyDown (
				wxKeyEvent& 						event)

{
	Rect									viewRect;		// used to store the image portion
																// of the window
	wxPoint								currentPos = event.GetPosition ();
	
	int									keyCode;

	SInt16								//code,
											windowType;
	
	Boolean								hasCaptureFlag;
	
	
			// Only handle if this window is not captured and a processor is not
			// in operation.
	
	hasCaptureFlag = HasCapture ();
	
   if (!hasCaptureFlag && gProcessorCode == 0)
		{
				// Also only handle if cursor is over image portion of a thematic image
				// window.
		
		windowType = m_View->GetWindowType ();
		
		bool	cursorOverImageFlag = false;
		GetWindowClipRectangle (m_View, kImageArea, &viewRect);
		if (currentPos.x >= viewRect.left && currentPos.x <= viewRect.right &&
							currentPos.y >= viewRect.top && currentPos.y <= viewRect.bottom)
			cursorOverImageFlag = true;
		
		if (windowType == kThematicImageType && cursorOverImageFlag)
			{
			keyCode = event.GetKeyCode ();
			
			if (keyCode == WXK_SHIFT)
				{
				MSetCursor (kBlinkOpenCursor2);
				
						// Do this to force the window cursor to be used, not the
						// overall cursor.
				
				wxSetCursor (wxNullCursor);

				}	// end "if (keyCode == WXK_SHIFT)"
			/*
			#if defined multispec_wxlin
						// Cannot do this since the control key controls selecting
						// the same area across multiple image windows.
				else if (keyCode == WXK_CONTROL && !wxGetKeyState (WXK_SHIFT))
					{
					MSetCursor (kBlinkOpenCursor2);
					
							// Do this to force the window cursor to be used, not the
							// overall cursor.
					
					wxSetCursor (wxNullCursor);
					
					}	// end "else if (keyCode == WXK_CONTROL && !..."
			#endif
			*/
					// The following is being tried to keep the cursor from
					// disapearing on some web browsers. Did not help.
			
			else if (keyCode == 0x41 ||
								keyCode == 0x5a ||
											keyCode == 0x27 ||
														keyCode == 0x2f)
				{
				if (s_repeatKeyFlag)
					{
					event.Skip (false);
																									return;

					}	// end "if (s_repeatKeyFlag)"
				
				else	// !s_repeatKeyFlag
					{
					s_repeatKeyFlag = TRUE;
					event.Skip (false);
					
					}	// end "else !s_repeatKeyFlag"

				}	// end "else if (keyCode == 0x41 || ..."
			
			else if (keyCode == WXK_CONTROL && (gPresentCursor == kBlinkOpenCursor2 ||
																gPresentCursor == kBlinkShutCursor2))
				{
				event.Skip (false);
																									return;
			
				}	// end "else if (keyCode == WXK_CONTROL && ..."
			
			else if (keyCode == WXK_ALT && (gPresentCursor == kBlinkOpenCursor2 ||
																gPresentCursor == kBlinkShutCursor2))
				{
				event.Skip (false);
																									return;
			
				}	// end "else if (keyCode == WXK_CONTROL && ..."
			
			else	// keyCode != WXK_SHIFT
				{
				if (keyCode == WXK_SPACE)
																									return;
				
				}	// end "else keyCode != WXK_SHIFT"
			
			//event.Skip (false);
			
			}	// end "if (windowType == kThematicImageType && cursorOverImageFlag)"
		
		}	// end "if (!hasCaptureFlag && gProcessorCode == 0)"
	
	else	// hasCaptureFlag || gProcessorCode != 0
		event.Skip ();

}	// end "OnKeyDown"



void CMImageCanvas::OnKeyUp (
				wxKeyEvent& 						event)

{
	int keyCode = event.GetKeyCode ();

	if (keyCode == WXK_SHIFT)
		{
		if (gPresentCursor == kBlinkOpenCursor2 || gPresentCursor == kBlinkShutCursor2)
			{
			s_repeatKeyFlag = FALSE;
			MSetCursor (kCross);
			event.Skip ();
			
			}	// end "if (gPresentCursor == kBlinkOpenCursor2)"
		
		}	// end "if (keyCode == WXK_SHIFT)"
	
	else if (keyCode == 0x41 || keyCode == 0x5a || keyCode == 0x27 || keyCode == 0x2f)
		{
		s_repeatKeyFlag = FALSE;
		event.Skip (false);
		
		}	// end "else if (keyCode == 0x41 || ..."
	
	else if (gPresentCursor == kBlinkOpenCursor2 ||
														gPresentCursor == kBlinkShutCursor2)
		{
		event.Skip (false);
		
		}	// end "else if (gPresentCursor == kBlinkOpenCursor2 ..."
	
	else	// keyCode != WXK_SHIFT
		event.Skip ();
	
}	// end "OnKeyUp"



void CMImageCanvas::OnLeaveImageWindow (
				wxMouseEvent&						event)

{
   if (!HasCapture () && m_View != NULL)
   	{
		m_View->UpdateCursorCoordinates ();
		
		MSetCursor (kArrow);
		
		}	// end "OnLeaveImageWindow"
		
}	// end "OnLeaveImageWindow"



void CMImageCanvas::OnLeftDown (
				wxMouseEvent&						event)

{
	wxPoint								cursorPosOnImage; // unit: window point
	
	
	if (gProcessorCode == kSelectionGraphStatsProcess)
				// Statistics for graph are being collected from a previous
				// selection. Do not allow another to begin.
																									return;
	
	if (gActiveImageViewCPtr != m_View)
				// Force this image window to be active.
		m_View->Activate (true);
	
	SetFocus ();
   wxRect imageRect = GetImageDisplayRect (GetScrollPosition ());

   do
		{
      cursorPosOnImage = ClientToImage (event.GetPosition ());
      if (!imageRect.Contains (event.GetPosition ()))
			break;
		
      m_TR = m_BL = event.GetPosition ();
      m_Selection.SetPosition (cursorPosOnImage);   // unit: window point
      m_Selection.SetSize (wxSize (0, 0));
		m_LastSelectionPoint = cursorPosOnImage;
		
		}	while (false);
	
   if (m_View->ImageWindowIsAvailable ())
		{
      CMTool* pTool = CMTool::FindTool (CMTool::c_toolType);
      if (pTool != NULL && gPresentCursor != kBlinkOpenCursor2)
			{
			#if defined multispec_wxlin
				m_View->SetControlKeyFlag (wxGetKeyState (WXK_CONTROL));
			#endif
			#if defined multispec_wxmac
						// Note that WXK_CONTROL is the command key for Mac OS
						// WXK_RAW_CONTROL is the control key. Note that use of
						// the control key has to be handled by messages for
						// contextual menus.
				//m_View->SetControlKeyFlag (wxGetKeyState (WXK_RAW_CONTROL));
				m_View->SetControlKeyFlag (wxGetKeyState (WXK_CONTROL));
			#endif
			
			m_View->SetShiftKeyFlag (wxGetKeyState (WXK_SHIFT));
         pTool->OnLButtonDown (m_View, 0, event.GetPosition ());
			
			}	// end "if (pTool != NULL && ..."
	
		if (gSelectionGraphViewCPtr != NULL)
			gSelectionGraphViewCPtr->UpdateShowOrHideFeatureList ();
		
		if (gPresentCursor == kBlinkOpenCursor2)
			{
			LongPoint cursorPoint;
			cursorPoint.h = (SInt32)cursorPosOnImage.x;
			cursorPoint.v = (SInt32)cursorPosOnImage.y;

			SInt16 code = 1;
			if (wxGetKeyState (WXK_CONTROL) ||
						#if defined multispec_wxlin
							wxGetKeyState ((wxKeyCode)0x5a) ||
									//wxGetKeyState ((wxKeyCode)0x2f))
									wxGetKeyState (WXK_DIVIDE))
						#endif
						#if defined multispec_wxmac
							wxGetKeyState ((wxKeyCode)0x5a))
						#endif
				code = 2;
			
			else if (wxGetKeyState (WXK_ALT) ||
						#if defined multispec_wxlin
								wxGetKeyState ((wxKeyCode)0x41) ||
											wxGetKeyState ((wxKeyCode)0x27))
						#endif
						#if defined multispec_wxmac
								wxGetKeyState ((wxKeyCode)0x41))
						#endif
				code = 4;

					// Note that getting the closed eye cursor is browser dependent.
					// Does not work on Safari or Internet Explorer.
			
			ThematicImageWBlink (cursorPoint, code);
			
			}	// end "if (gPresentCursor == kBlinkOpenCursor2)"

		}	// end "if (m_View->ImageWindowIsAvailable ())"
	
}	// end "OnLeftDown"



void CMImageCanvas::OnLeftUp (
				wxMouseEvent&						event)

{
	if (HasCapture ())
		{
      m_TR = m_BL = wxPoint (0, 0);
      m_Selection.SetPosition (m_TR);
      m_Selection.SetSize (wxSize (0, 0));
		
      if (m_View->ImageWindowIsAvailable ())
			{
         CMTool* pTool = CMTool::FindTool (CMTool::c_toolType);
         if (pTool != NULL)            
				pTool->OnLButtonUp (
									m_View, 0, event.GetPosition (), gNewSelectedWindowFlag);
			
         if (gSelectionGraphViewCPtr != NULL && m_dataListShowFlag == true)
				{
				gSelectionGraphViewCPtr->m_frame->m_checkBoxData->Enable (true);
				if (gNewSelectedWindowFlag == false)
					gSelectionGraphViewCPtr->UpdateDataListCtrl ();
				
				}	// end "if (gSelectionGraphViewCPtr != NULL && ..."
         
         gNewSelectedWindowFlag = FALSE;  
         
			//ReleaseMouse ();
			
			}	// end "if (m_View->ImageWindowIsAvailable ())"
		
		}	// end "if (HasCapture ())"
	
	else	// !HasCapture ()
		{
		if (gPresentCursor == kBlinkShutCursor2 && wxGetKeyState (WXK_SHIFT))
			{
			MSetCursor (kBlinkOpenCursor2);

			}
		
		else if (gPresentCursor == kBlinkOpenCursor2 && !wxGetKeyState (WXK_SHIFT))
			{
			MSetCursor (kCross);

			}	// end "if (gPresentCursor == kBlinkOpenCursor2 && ..."
		
		}	// end "else !HasCapture ()"
	
}	// end "OnLeftUp"



void CMImageCanvas::OnMouseMove (
				wxMouseEvent&						event)

{
	Rect									viewRect;		// used to store the image portion
																// of the window
	wxPoint								currentPos = event.GetPosition ();
	
	SInt16								windowType;
	bool									hasCaptureFlag;


			// Returns true if this window has the current mouse capture.
	
	hasCaptureFlag = HasCapture ();
	
   if (gProcessorCode == 0 || gProcessorCode == kPolygonSelectionProcessor)
		{
				// Handle cursor setting in this section.
		
		GetWindowClipRectangle (m_View, kImageArea, &viewRect);
		bool	cursorOverImageFlag = false;
		if (currentPos.x >= viewRect.left && currentPos.x <= viewRect.right &&
							currentPos.y >= viewRect.top && currentPos.y <= viewRect.bottom)
			cursorOverImageFlag = true;
		
		if (hasCaptureFlag)
			{
			if (gPresentCursor != kCross)
				{
				MSetCursor (kCross);
				
				}	// end "if (gPresentCursor != kCross)"
			
			}	// end "if (gPresentCursor != kCross)"
		
		else	// !hasCaptureFlag
			{
			if (cursorOverImageFlag)
				{
				windowType = m_View->GetWindowType ();
				if (windowType == kThematicImageType)
					{
					if (wxGetKeyState (WXK_SHIFT))
						{
						wxMouseState currentMouseState = wxGetMouseState ();
						if (currentMouseState.LeftIsDown ())
							{
							if (gPresentCursor != kBlinkShutCursor2)
								MSetCursor (kBlinkShutCursor2);
							
							}	// end "if (currentMouseState.LeftIsDown ())"
							
						else	// left is up
							{
							MSetCursor (kBlinkOpenCursor2);
							
							}	// end "else left is up"
						
						}	// end "if (wxGetKeyState (WXK_SHIFT))"
					
					else	// !wxGetKeyState (WXK_SHIFT)
						{
						MSetCursor (kCross);
					
						}	// end "else !wxGetKeyState (WXK_SHIFT)"

					}	// end "if (windowType == kThematicImageType)"
				
				else	// windowType != kThematicImageType
					{
					MSetCursor (kCross);
				
					}	// end "else if (gPresentCursor != kCross)"
				
				}	// end "if (cursorOverImageFlag)"
			
			else	// cursor is outside of the image
				{
				if (gPresentCursor != kArrow)
					{
					MSetCursor (kArrow);
					
					}	// end "if (gPresentCursor != kArrow)"
			
				}	// end "else cursor is outside of the image"
			
			}	// end "else (!hasCaptureFlag)"

		}	// end "if (gProcessorCode == 0 || ..."
	
   if (hasCaptureFlag)
		{
		Boolean		scrolledFlag;
		
				// Returns the built-in scrollbar position.
		
		wxPoint scrollPos = GetScrollPosition ();
      scrolledFlag = AutoScroll (currentPos, scrollPos);
      scrolledFlag = scrolledFlag | m_scrolledFlag;
		
		CMTool* pTool = CMTool::FindTool (CMTool::c_toolType);
		
				// The following code was in Image view class in Windows OS
		
      if (m_View->ImageWindowIsAvailable ())
			{
         Boolean handledFlag = FALSE;
         
         if (pTool != NULL)
            handledFlag = pTool->OnMouseMove (m_View, 0, currentPos, scrolledFlag);

         if (!handledFlag)
				{
            LongPoint		longLocalPoint;
            longLocalPoint.h = currentPos.x;
            longLocalPoint.v = currentPos.y;
            m_View->UpdateCursorCoordinates (&longLocalPoint);

				}	// end "if (!handledFlag)"
         
			}	// end "if (m_View->ImageWindowIsAvailable ())"

      else // !m_View->ImageWindowIsAvailable ()
			{
         if (m_View != NULL)
            m_View->UpdateCursorCoordinates ();

			}	// end "else !m_View->ImageWindowIsAvailable ()"
			
		}	// end "if (hasCaptureFlag)"
		
	else	// !hasCaptureFlag
		{
				// Make sure that flag indicating that a mouse down occurred to
				// activate the window is set to false. It may not get turned off
				// if one selects the title bar of the window. So far have not found
				// a way to capture the left button up event to turn it off.
		
		gNewSelectedWindowFlag = FALSE;
		
      if (m_View != NULL)
			{
			if (gPresentCursor == kCross || gPresentCursor == kBlinkOpenCursor2)
				{
				Boolean handledFlag = FALSE;
				
            if (gPresentCursor == kCross &&
            								gProcessorCode == kPolygonSelectionProcessor)
					{
							// If is polygon mode. added on 01/08/16  by Wei
					
               if (m_View->ImageWindowIsAvailable ())
						{
                  CMTool* pTool = CMTool::FindTool (CMTool::c_toolType);

                  if (pTool != NULL)            
                     handledFlag = pTool->OnMouseMove (m_View,
                     												0,
                     												currentPos,
                     												m_scrolledFlag);
						
						}	// end "if (m_View->ImageWindowIsAvailable ())"
					
					}	// end "if (... && gProcessorCode == kPolygonSelectionProcessor"

				if (!handledFlag)
					{
      			LongPoint		longLocalPoint;
					longLocalPoint.h = currentPos.x;
					longLocalPoint.v = currentPos.y;
					m_View->UpdateCursorCoordinates (&longLocalPoint);
					
					}	// end "if (!handledFlag)"
				
				}	// end "if (gPresentCursor == kCross || kBlinkOpenCursor2"
				
			else	// gPresentCursor != kCross && != kBlinkOpenCursor2
				m_View->UpdateCursorCoordinates ();
				
			}	// end "if (m_View != NULL)"
			
		}	// end "else !hasCaptureFlag"
	
	m_scrolledFlag = FALSE;
	//event.Skip ();
		
}	// end "OnMouseMove"



void CMImageCanvas::OnMouseWheel (
				wxMouseEvent& 						event)

{
			// If the mouse wheel is not captured, test if the mouse
			// pointer is over the image window and if not, don't
			// handle the message but pass it on.

	if (!GetRect().Contains (event.GetPosition ()))
		{
		wxWindow* parent = GetParent ();
		if (parent != NULL)
			{
			wxMouseEvent newevt (event);
			newevt.SetPosition (
				parent->ScreenToClient (ClientToScreen (event.GetPosition ())));
			parent->ProcessWindowEvent (newevt);
			
			}	// end "if (parent != NULL)"
		
		return;
		
		}	// end "if (!GetRect().Contains (event.GetPosition ()))"

    DoMouseWheel (event.GetWheelAxis (),
						event.GetWheelRotation (),
						event.ControlDown ());
	
}	// end "OnMouseWheel"



void CMImageCanvas::OnPaint (
				wxPaintEvent&						event)

{
   if (m_View == NULL)
																								return;
	
	//wxRect 								updateRect;
	
	double 								zoom = m_View->m_Scale;
	
	int 									xx,
											yy;
	
	Boolean								bitMapOKFlag;
	

			// Shifts the device origin so we don't have to worry about the current
			// scroll position ourselves
			// Note: tried double buffered paint but found that both MacOS and gtk2 versions
			// are doing it automatically. Do not need to do here.
			// Boolean doubleBufferedFlag = IsDoubleBuffered ();
			// wxAutoBufferedPaintDC dc (this);

	wxPaintDC dc (this);
	
			// Fills the dc with the current bg brush
			// Find Out where the window is scrolled to

	DoPrepareDC (dc);
	dc.Clear ();
	
			//	Sets the scale to be applied when converting from logical units to device
			// units

   dc.SetUserScale (zoom, zoom);
	
			// Sets the bg brush used in Clear(). Default:wxTRANSPARENT_BRUSH

	dc.SetBackground (wxBrush (*wxWHITE));
	
			// Get the update rect list
	
	int count = 1;
	wxRegionIterator upd (GetUpdateRegion ()); // get the update rect list
	while (upd)
		{
		m_View->s_updateRect.left = upd.GetX ();
		m_View->s_updateRect.right = m_View->s_updateRect.left + upd.GetW ();
		m_View->s_updateRect.top = upd.GetY ();
		m_View->s_updateRect.bottom = m_View->s_updateRect.top + upd.GetH ();

		m_View->OnDraw (&dc);
		
		count++;
		upd++;
			
		}	// end "while (upd)"
	
	bitMapOKFlag = !m_View->m_ScaledBitmap.IsNull ();
	
	//if (m_displayImageFlag && bitMapOKFlag)
	if (m_displayImageFlag)
		{
   	UInt32								numberOverlays;
	
		Boolean								drawVectorOverlaysFlag,
												projectWindowFlag;
		
		CMImageWindow* imageWindowCPtr = m_View->GetImageWindowCPtr ();
		WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
													imageWindowCPtr->GetWindowInfoHandle ());
		
		drawVectorOverlaysFlag = windowInfoPtr->drawVectorOverlaysFlag;
		projectWindowFlag = windowInfoPtr->projectWindowFlag;
		numberOverlays = windowInfoPtr->numberVectorOverlays;

				// Outline training/test areas.
		
		if (windowInfoPtr->projectWindowFlag)
			{
			Rect windowRect;
			
			GetWindowClipRectangle (m_View, kImageArea, &windowRect);
			ClipRect (&windowRect);

			gCDCPointer = &dc;
			OutlineFieldsControl (gProjectInfoPtr->statsWindowMode,
										  m_View,
										  imageWindowCPtr->GetWindowInfoHandle (),
										  1);
			gCDCPointer = NULL;
			
			}	// end "if (windowInfoPtr->projectWindowFlag)"
		
				// Draw the selection area if it exists.
		
		if (bitMapOKFlag)
			{
					// Use the bitmap size
			
			m_size_h = m_View->m_ScaledBitmap.GetHeight () * zoom;
			m_size_w = m_View->m_ScaledBitmap.GetWidth () * zoom;
			
			}	// end "if (bitMapOKFlag)"
		
		else	// !bitMapOKFlag
			{
					// Use the number of lines and columns in the image
			
			m_size_h = windowInfoPtr->maxNumberColumns * zoom;
			m_size_w = windowInfoPtr->maxNumberLines * zoom;
			
			}	// end "else !bitMapOKFlag"

				//Sets the virtual size of the window in pixels.
		
		SetVirtualSize (m_size_w, m_size_h);
		
		dc.SetUserScale (1, 1);
		
		CMTool* pTool = CMTool::FindTool (CMTool::c_toolType);
		if (!m_Selection.IsEmpty () && pTool->s_selectType != kPolygonType)
			{
			dc.SetBrush (*wxTRANSPARENT_BRUSH);
			dc.SetPen (wxPen (*wxWHITE, 1, wxPENSTYLE_DOT));
			CalcUnscrolledPosition (m_Selection.GetX (), m_Selection.GetY (), &xx, &yy);
			wxSize selectionRecSize = m_Selection.GetSize ();
			dc.DrawRectangle (wxPoint (xx, yy), selectionRecSize);
		
			dc.SetPen (wxPen (*wxBLACK, 1, wxPENSTYLE_DOT));
			selectionRecSize.x -= 2;
			selectionRecSize.y -= 2;
			dc.DrawRectangle (wxPoint (xx+1, yy+1), selectionRecSize);
			
			}	// end "if (!m_Selection.IsEmpty () && ...)"
		
		CMImageDoc* pDoc = m_View->GetDocument ();
		pDoc->DrawObjects (&dc, m_View);
		
		}	// end "if (m_displayImageFlag && ...)"
	
}	// end "OnPaint"



void CMImageCanvas::OnScrollChanged (
				wxScrollWinEvent& 				event)
				
{
	wxPoint scrollPos = GetScrollPosition ();
	m_View->m_Canvas->DoScroll (scrollPos.x, scrollPos.y);
	
	m_View->ScrollChanged ();
	
}	// end "OnScrollChanged"



void CMImageCanvas::SetView (
				CMImageView* 						viewPtr)

{
   m_View = viewPtr;
	
}	// end "SetView"
