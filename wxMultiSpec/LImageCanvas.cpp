//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						CMImageCanvas.cpp : class implementation file
//	Class Definition:		CMImageCanvas.h
//
//	Authors:					Larry L. Biehl, Abdur Rachman Maud
//
//	Revision date:			01/18/2019
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	This file contains functions that relate to the 
//								CMImageFrame class.
//
/* Template for debugging
	int numberChars = sprintf ((char*)gTextString3,
				" CMImageCanvas:: (): %s",
				gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------
//

#include "LImageCanvas.h"
#include "LImageDoc.h"
#include "wx/dcbuffer.h"
#include "wx/event.h"
#include "wx/utils.h"
#include "LTools.h"
#include "LGraphView.h"

extern void		DoNextDisplayChannelEvent (
				WindowPtr		window,
				char			theChar);

IMPLEMENT_DYNAMIC_CLASS (CMImageCanvas, wxScrolledWindow)

wxSize CMImageCanvas::ScrollingIncrement = wxSize (10, 10);	// 10, 10

BEGIN_EVENT_TABLE (CMImageCanvas, wxScrolledWindow)
	EVT_CHAR_HOOK (CMImageCanvas::OnCharHook)
	EVT_ERASE_BACKGROUND (CMImageCanvas::OnEraseBackground)
	EVT_IDLE (CMImageCanvas::OnIdle)
	EVT_LEAVE_WINDOW (CMImageCanvas::OnLeaveImageWindow)
   EVT_LEFT_DCLICK (CMImageCanvas::OnDlbClk)
	EVT_LEFT_DOWN (CMImageCanvas::OnLeftDown)
	EVT_LEFT_UP (CMImageCanvas::OnLeftUp)
	EVT_MOTION (CMImageCanvas::OnMotion)
	EVT_MOUSEWHEEL (CMImageCanvas::OnMouseWheel)
	EVT_PAINT (CMImageCanvas::OnPaint)
	EVT_SCROLLWIN (CMImageCanvas::OnScrollChanged)
	EVT_SET_CURSOR (CMImageCanvas::OnCursorChange)
END_EVENT_TABLE ()


CMImageCanvas::CMImageCanvas ()

{
   Init();
	
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
	
   SetCursor (wxCURSOR_ARROW);
	
   wxSize initsize = parent->GetClientSize ();
   SetVirtualSize (initsize);
   size_h = initsize.GetHeight ();
   size_w = initsize.GetWidth ();
   SetScrollRate (1, 1);	// 10, 10
   updatedbmp = true;
	m_displayImageFlag = true;
   m_featureListShowFlag = false;
   m_dataListShowFlag = false;
   m_LastSelectionPoint = wxPoint (0, 0);
	
	EnableScrolling (true, true);
	
}	// end "CMImageCanvas"



void CMImageCanvas::AdjustScrollBars (
				bool									callScrollFlag)

{
			// image sampled size
	
   wxSize imageSize = GetCurrentSize ();
	
			// old scroll position
	
   wxPoint pt = GetScrollPosition ();

   //SetScrollbar(wxHORIZONTAL, 0, GetClientSize().x, imageSize.x);
   //SetScrollbar(wxVERTICAL, 0, GetClientSize().y, imageSize.y);
	SetVirtualSize (imageSize);
	if (callScrollFlag)
		Scroll (pt.x, pt.y);
	
}	// end "AdjustScrollBars"



bool CMImageCanvas::AutoScroll (
				wxPoint								currentPos,
				wxPoint								scrollPos)

{
   bool res = false;
   if (HasCapture ())
		{
				// If mouse is captured set rect coords if inside the image and
				// autoscroll if posible get image display rectangle
		
      wxRect displayRect = GetImageDisplayRect(scrollPos);

      wxSize oldSelection(m_TR.x - m_BL.x, m_TR.y - m_BL.y);

				// check if the current drag position is inside the image - do not allow
				// to draw rectangle out of the image
		
      m_TR.x = wxMin (wxMax (currentPos.x, displayRect.GetLeft ()),
								displayRect.GetRight ());
      m_TR.y = wxMin (wxMax (currentPos.y, displayRect.GetTop ()),
								displayRect.GetBottom ());

				// Check current drag position and update scroll regularly
		
      if (currentPos.x <= 0)
			{
         Scroll (wxMax(scrollPos.x - CMImageCanvas::ScrollingIncrement.GetWidth(), 0), -1);
         m_BL.x += (scrollPos.x - GetScrollPosition().x);
         res = true;
			
			}
		
      if (currentPos.y <= 0)
			{
         Scroll (-1, wxMax(scrollPos.y - CMImageCanvas::ScrollingIncrement.GetHeight(), 0));
         m_BL.y += (scrollPos.y - GetScrollPosition().y);
         res = true;
			
			}
		
      if (currentPos.x >= GetClientSize().GetWidth())
			{
         Scroll (scrollPos.x + CMImageCanvas::ScrollingIncrement.GetWidth(), -1);
         m_BL.x -= (GetScrollPosition().x - scrollPos.x);
         res = true;
			
			}
		
      if (currentPos.y >= GetClientSize().y)
			{
         Scroll (-1, scrollPos.y + CMImageCanvas::ScrollingIncrement.GetHeight());
         m_BL.y -= (GetScrollPosition().y - scrollPos.y);
         res = true;
			
			}
		
      m_Selection = wxRect (wxMin (m_TR.x, m_BL.x),
												wxMin (m_TR.y, m_BL.y),
												abs(m_TR.x - m_BL.x),
												abs(m_TR.y - m_BL.y));
      return res;
		
		}
	
	else
		return true;
	
}	// end "AutoScroll"



void CMImageCanvas::CanvasScroll (
				int									x,
				int									y)

{
   Scroll (x, y);
	
}	// end "CanvasScroll"



wxPoint CMImageCanvas::ClientToImage (
				const wxPoint&						pos)

{
   wxPoint scrollPos = GetScrollPosition ();
   wxRect imageRect = GetImageDisplayRect (scrollPos);
   return wxPoint (pos.x - imageRect.GetLeft (), pos.y - imageRect.GetTop ());
	
}



bool CMImageCanvas::Create (wxWindow * parent, wxWindowID id,
        const wxPoint & pos, const wxSize & size, long style, const wxString & name)

{
   bool res = wxScrolledWindow::Create(parent, id, pos, parent->GetClientSize(), style, name);
   if (res)
      AdjustScrollBars(true);

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
			UInt32 currentTickCount = GetTickCount();
			if (currentTickCount >= GetMainFrame()->GetNextControlTime ())
				{
				GetMainFrame()->SetNextControlTime (gControlOffset/4);
			
				if (rotation > 0)
					{
					GetMainFrame ()->SetZoomCode (ID_ZOOM_IN);
					m_View->ZoomIn ();
					GetMainFrame ()->SetZoomCode (0);
					
					}	// end "if (rotation > 0)"
				
				else	// rotation <= 0
					{
					GetMainFrame ()->SetZoomCode (ID_ZOOM_OUT);
					m_View->ZoomOut ();
					GetMainFrame ()->SetZoomCode (0);
					
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
			scrollPos.x += rotation;

			}	// end "if (axis == wxMOUSE_WHEEL_HORIZONTAL)"
		
		else	// otherwise scroll the window vertically
			{
			scrollPos.y += rotation;
			
			}	// end "else otherwise just scroll the window"
		
		displayRect = m_View->m_Canvas->GetImageDisplayRect (scrollPos);
		//#if defined multispec_wxmac
			m_View->m_Canvas->Scroll (scrollPos.x, scrollPos.y);
		//#endif
		m_View->ScrollChanged ();
		
		m_View->m_Canvas->Refresh (false, &displayRect);
		
		}	// end "else !ctrlDown"
	
}	// end "DoMouseWheel"



void CMImageCanvas::EraseBackground (
				bool									erase)
{
   m_displayImageFlag = !erase;
   if (erase)
		{
      wxSize psize = GetParent ()->GetClientSize ();
      SetVirtualSize (psize);
      size_h = psize.GetHeight ();
      size_w = psize.GetWidth ();
		
		}
	
   Refresh ();
	
}	// end "EraseBackground"



void CMImageCanvas::FixViewOffset ()

{
   if (m_View)
		{
      wxPoint pt = GetScrollPosition ();
      wxRect displayRect = GetImageDisplayRect (pt);
      wxSize offset(displayRect.GetPosition ().x, displayRect.GetPosition ().y);
      m_View->SetViewOffset (offset);
		
		}	// end "if (m_View)"
	
}	// end "FixViewOffset"



CMImageView* CMImageCanvas::GetView ()

{
   return m_View;
	
}	// end "GetView"



wxSize CMImageCanvas::GetCurrentSize ()

{
	if (!m_View)
      return wxSize (0, 0);
	
   double scale = m_View->m_Scale;
   //if (m_View->GetScaledBitmapPtr() != NULL &&
   if (m_View->GetScaledBitmap().IsOk())
      return wxSize (m_View->GetScaledBitmap().GetWidth() * scale,
							m_View->GetScaledBitmap().GetHeight() * scale);
	
	else
      return wxSize (0, 0);
	
}	// end "GetCurrentSize"



wxRect CMImageCanvas::GetImageDisplayRect (
				const wxPoint&						scrollPos)

{
   wxSize currentSize = GetCurrentSize ();
  // wxSize clients = GetClientSize ();
   // calculate actual image position if it is centered
   /*wxPoint ptTest(
           (GetClientSize().x - currentSize.GetWidth()) <= 0 ?
           -scrollPos.x :
           ((GetClientSize().GetWidth() - currentSize.GetWidth()) * 0.5f),
           (GetClientSize().y - currentSize.GetHeight()) <= 0 ?
           -scrollPos.y :
           ((GetClientSize().GetHeight() - currentSize.GetHeight()) * 0.5f));*/
   wxPoint ptTest (0, 0);
	
		// calculate actual image diaply rectangle if centered
	
	return wxRect (ptTest, currentSize);
	
}	// end "GetImageDisplayRect"



const wxRect & CMImageCanvas::GetSelection ()

{
   return m_Selection;
	
}



wxPoint CMImageCanvas::GetLastSelectionPoint ()

{
   return m_LastSelectionPoint;
	
}



wxPoint CMImageCanvas::GetScrollPosition ()

{
   return wxPoint (GetScrollPos (wxHORIZONTAL), GetScrollPos (wxVERTICAL));
	
}



void CMImageCanvas::Init()

{
   m_View = NULL;
	
}	// end "Init"



void CMImageCanvas::OnCharHook (
				wxKeyEvent&							event)

{
   CMTool* pTool = CMTool::FindTool(CMTool::c_toolType);
	
	if (pTool != NULL)
		{
		int keyCode = event.GetKeyCode ();
		
		if (keyCode == WXK_RETURN || keyCode == WXK_NUMPAD_ENTER)
			pTool->OnCharHook();
		
		else if (keyCode == WXK_DELETE)
			{
			ClearSelectionArea (gActiveImageViewCPtr);
			ShowGraphSelection ();
			m_LastSelectionPoint = wxPoint (0,0);
			gActiveImageViewCPtr->m_Canvas->Refresh ();
			
			if (gSelectionGraphViewCPtr != NULL)
				gSelectionGraphViewCPtr->ResetListControls ();
			
			}	// end "else if (keyCode == WXK_DELETE)"
		
		else 
			event.Skip ();
		
		}	// end "if (pTool != NULL)"
	
}	// end "OnCharHook"



void CMImageCanvas::OnDlbClk (
				wxMouseEvent&						event)

{
   if (m_View->CheckIfOffscreenImageExists ())
		{
      CMTool* pTool = CMTool::FindTool (CMTool::c_toolType);
      if (pTool != NULL)
         pTool->OnLButtonDblClk (m_View, 0);
		
		}	// end "if (m_View->CheckIfOffscreenImageExists ())"
	
}	// end "OnDlbClk"



void CMImageCanvas::OnEraseBackground (
				wxEraseEvent&						event)

{

}		// end "OnEraseBackground"



void CMImageCanvas::OnIdle (
				wxIdleEvent&						event)

{
   do
		{
      if (!HasCapture()) break;

      // get scroll position
      wxPoint scrollPos = GetScrollPosition ();
      // get mouse in client coordinates
      wxPoint currentPos = ScreenToClient(wxGetMousePosition ());
      
      // auto scroll
      // check current drag position and update scroll regularly
      if (AutoScroll (currentPos, scrollPos))
         event.RequestMore ();
	
		FixViewOffset();
		
		}	while (false);
	
}	// end "OnIdle"



void CMImageCanvas::OnLeaveImageWindow (
				wxMouseEvent&						event)

{
   if (!HasCapture()) 
		m_View->UpdateCursorCoordinates ();
		
}	// end "OnLeaveImageWindow"



void CMImageCanvas::OnLeftDown (
				wxMouseEvent&						event)

{
	wxPoint								cursorPosOnImage; // unit: window point
	
	
	if (gActiveImageViewCPtr != m_View)
				// Force this image window to be active.
		m_View->Activate (true);
	
	SetFocus ();
   wxRect imageRect = GetImageDisplayRect (GetScrollPosition ());

   do
		{
      if (!m_View) 
			break;
		
      cursorPosOnImage = ClientToImage (event.GetPosition ());
      if (!imageRect.Contains(event.GetPosition()))
			break;
		
		Refresh ();
      m_TR = m_BL = event.GetPosition ();
      m_Selection.SetPosition (cursorPosOnImage);   // unit: window point
      m_Selection.SetSize (wxSize (0, 0));
		m_LastSelectionPoint = cursorPosOnImage;
		
		}	while (false);
		
   if (m_View->CheckIfOffscreenImageExists ())
		{
      CMTool* pTool = CMTool::FindTool(CMTool::c_toolType);
      if (pTool != NULL)
			{
			m_View->SetControlKeyFlag (wxGetKeyState (WXK_CONTROL));
			m_View->SetShiftKeyFlag (wxGetKeyState(WXK_SHIFT));
         pTool->OnLButtonDown (m_View, 0, event.GetPosition());
			
			}	// end "if (pTool != NULL)"
      
		if (gSelectionGraphViewCPtr != NULL)
			gSelectionGraphViewCPtr->UpdateShowOrHideFeatureList ();

		}	// end "if (m_View->CheckIfOffscreenImageExists())"
	
}	// end "OnLeftDown"



void CMImageCanvas::OnLeftUp (
				wxMouseEvent&						event)

{
	if (HasCapture ())
		{
      Refresh ();

      m_TR = m_BL = wxPoint (0, 0);
      m_Selection.SetPosition (m_TR);
      m_Selection.SetSize (wxSize (0, 0));
		
      if (m_View->CheckIfOffscreenImageExists ())
			{
         CMTool* pTool = CMTool::FindTool(CMTool::c_toolType);
         if (pTool != NULL)            
				pTool->OnLButtonUp (m_View, 0, event.GetPosition(), gNewSelectedWindowFlag);
			
         if (gSelectionGraphViewCPtr != NULL && m_dataListShowFlag == true)
				{
				gSelectionGraphViewCPtr->m_frame->m_checkBoxData->Enable (true);
				if (gNewSelectedWindowFlag == false)
					gSelectionGraphViewCPtr->UpdateDataListCtrl ();
				
				}	// end "if (gSelectionGraphViewCPtr != NULL && ..."
         
         gNewSelectedWindowFlag = FALSE;  
         
			//ReleaseMouse();
			
			}	// end "if (m_View->CheckIfOffscreenImageExists ())"
		
		}	// end "if (HasCapture ())"
	
}	// end "OnLeftUp"



void CMImageCanvas::OnMotion (
				wxMouseEvent&						event)

{
	Rect									viewRect;		// used to store the image portion
																// of the window
	wxPoint								currentPos = event.GetPosition ();
	bool									hasCaptureFlag;


			// Returns true if this window has the current mouse capture.
	
	hasCaptureFlag = HasCapture();
	
   if (gProcessorCode == 0)
		{
		GetWindowClipRectangle (m_View, kImageArea, &viewRect);
				
		bool	cursorOverImageFlag = false;
		if (currentPos.x >= viewRect.left && currentPos.x <= viewRect.right &&
							currentPos.y >= viewRect.top && currentPos.y <= viewRect.bottom)
			cursorOverImageFlag = true;
			
		if (cursorOverImageFlag || hasCaptureFlag)
			{
			if (gPresentCursor != kCross)
				{
				//MSetCursor (kCross);
				SetCursor (wxCURSOR_CROSS);
				gPresentCursor = kCross;
				
				}	// end "if (gPresentCursor != kCross)"
				
			}	// end "if (cursorOverImageFlag || hasCaptureFlag)"
			
		else	// cursor is outside of the image && !hasCaptureFlag
			{
			if (gPresentCursor != kArrow)
				{
				//MSetCursor (kArrow);
				SetCursor (wxCURSOR_ARROW);
				gPresentCursor = kArrow;
				
				}	// end "if (gPresentCursor != kArrow)"
		
			}	// end "else cursor is outside of the image && !hasCaptureFlag"

		}	// end "if (gProcessorCode == 0)"
	
	//else if (gPresentCursor == kWait)
		//SetCursor (wxCURSOR_WAIT);
	//	MSetCursor (kWait);
	
   if (hasCaptureFlag)
		{
				//Returns the built-in scrollbar position.
		
		wxPoint		scrollPos = GetScrollPosition ();
      m_Selection.SetSize (wxSize (currentPos.x, currentPos.y));
      
      AutoScroll (currentPos, scrollPos);
      FixViewOffset ();
      Refresh ();
		
				// The following code was in Image view class in windows
		
      if (m_View->CheckIfOffscreenImageExists ())
			{
         Boolean handledFlag = FALSE;
         CMTool* pTool = CMTool::FindTool(CMTool::c_toolType);
         
         if (pTool != NULL)
            handledFlag = pTool->OnMouseMove (m_View, 0, currentPos);

         if (!handledFlag)
				{
            LongPoint		longLocalPoint;
            longLocalPoint.h = currentPos.x;
            longLocalPoint.v = currentPos.y;
            m_View->UpdateCursorCoordinates (&longLocalPoint);

				}	// end "if (!handledFlag)"
         
			}	// end "if (m_View->CheckIfOffscreenImageExists())"

      else // !m_View->CheckIfOffscreenImageExists()
			{
         if (m_View != NULL)
            m_View->UpdateCursorCoordinates ();

			}	// end "else !GetActiveWindowFlag() || ..."
			
		}	// end "if (HasCapture())"
		
	else	// !hasCaptureFlag
		{
      LongPoint longLocalPoint;
		
				// Make sure that flag indicating that a mouse down occurred to
				// activate the window is set to false. It may not get turned off
				// if one selects the title bar of the window. So far have not found
				// a way to capture the left button up event to turn it off.
		
		gNewSelectedWindowFlag = FALSE;
		
      if (m_View != NULL)
			{
			if (gPresentCursor == kCross)
				{           
            if (gProcessorCode == 5)
					{
							// if is polygon mode. added on 01/08/16  by Wei
					
               if (m_View->CheckIfOffscreenImageExists())
						{
                  Boolean handledFlag = FALSE;
                  CMTool* pTool = CMTool::FindTool(CMTool::c_toolType);

                  if (pTool != NULL)            
                     handledFlag = pTool->OnMouseMove (m_View, 0, currentPos);

                  if (!handledFlag)
							{
                     LongPoint longLocalPoint;
                     longLocalPoint.h = currentPos.x;
                     longLocalPoint.v = currentPos.y;
                     m_View->UpdateCursorCoordinates (&longLocalPoint);
							
							}	// end "if (!handledFlag)"
						
						}	// end "if (m_View->CheckIfOffscreenImageExists ())"
					
					}	// end "if (gProcessorCode == 5)"
				
            else	// gProcessorCode != 5
					{
               longLocalPoint.h = currentPos.x;
               longLocalPoint.v = currentPos.y;
               m_View->UpdateCursorCoordinates (&longLocalPoint);
					
					}	// end "else gProcessorCode != 5"
				
				}	// end "if (gPresentCursor == kCross)"
				
			else
				m_View->UpdateCursorCoordinates ();
				
			}	// end "if (m_View != NULL)"
			
		}	// end "else !hasCaptureFlag"
		
}	// end "OnMotion"



void CMImageCanvas::OnMouseWheel (wxMouseEvent& event)

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
	
	int xx, yy;
	double m_zoom = m_View->m_Scale;
	//wxSize mframesize = (this->GetParent())->GetClientSize ();
	
			// Shifts the device origin so we don't have to worry about the current
			// scroll position ourselves

	wxPaintDC dc (this);
	
			// Fills the dc with the current bg brush
			// Find Out where the window is scrolled to

	DoPrepareDC (dc);
	dc.Clear ();
	
			//	Sets the scale to be applied when converting from logical units to device
			// units

   dc.SetUserScale (m_zoom, m_zoom);
	
			// Sets the bg brush used in Clear(). Default:wxTRANSPARENT_BRUSH

	dc.SetBackground (wxBrush (*wxWHITE));
	
   if (updatedbmp)
		m_View->OnDraw (&dc);
	
	else
				// Next time call copy offscreenimage
		updatedbmp = true;
	
	wxBitmap my_image (m_View->m_ScaledBitmap);
	//wxBitmap my_image (*m_View->m_ScaledBitmapPtr);
	
	if (m_displayImageFlag && my_image.IsOk())
		{
		size_h = my_image.GetHeight() * m_zoom;
		size_w = my_image.GetWidth() * m_zoom;

		SetVirtualSize (size_w, size_h); //Sets the virtual size of the window in pixels.
		
		dc.SetUserScale (1, 1);
		
		//m_View->OnDraw(&dc);
      CMTool* pTool = CMTool::FindTool (CMTool::c_toolType);
		if (!m_Selection.IsEmpty() && pTool->s_selectType != kPolygonType)
			{
			dc.SetBrush (*wxTRANSPARENT_BRUSH);
			//dc.SetLogicalFunction (wxXOR);
			dc.SetPen(wxPen (*wxWHITE, 1, wxPENSTYLE_DOT));
			CalcUnscrolledPosition (m_Selection.GetX(), m_Selection.GetY(), &xx, &yy);
			dc.DrawRectangle (wxPoint(xx, yy), m_Selection.GetSize());
			dc.SetPen(wxPen (*wxBLACK, 1, wxPENSTYLE_DOT));
			wxSize rectSpec = m_Selection.GetSize ();
			rectSpec.x -= 2;
			rectSpec.y -= 2;
			dc.DrawRectangle (wxPoint(xx+1, yy+1), rectSpec);
			
			}	// end "if (!m_Selection.IsEmpty() && ...)"
		
		CMImageDoc* pDoc = m_View->GetDocument ();
		pDoc->Draw (&dc, m_View);
		
		}	// end "if (m_displayImageFlag && my_image.IsOk())"

   //wxLogTrace(wxTraceMask(), wxT("CMImageCanvas::OnPaint"));
	
}	// end "OnPaint"



void CMImageCanvas::OnScrollChanged (
				wxScrollWinEvent& 						event)
				
{
	m_View->ScrollChanged ();
	wxPoint scrollPos = GetScrollPosition ();
	wxRect displayRect = m_View->m_Canvas->GetImageDisplayRect (scrollPos);
	#if defined multispec_wxmac
		m_View->m_Canvas->Scroll (scrollPos.x, scrollPos.y);
	#endif
	
	m_View->m_Canvas->Refresh (false, &displayRect);

	//printf("scrollchanged ... ");
	/*
	printf ("OnScrollchanged: top, bottom, left, right: %d, %d, %d, %d \n",
				displayRect.y,
				displayRect.y + displayRect.height,
				displayRect.x,
				displayRect.x + displayRect.width);
	*/
	//wxPoint scrollPos = GetScrollPosition();
	//wxRect displayRect = m_View->m_Canvas->GetImageDisplayRect(scrollPos);
	//m_View->InvalidateRect(&displayRect, FALSE);
	
}	// end "OnScrollChanged"



void CMImageCanvas::OnCursorChange (
				wxSetCursorEvent& 						event)
				
{
	
	//wxCursor				cursor;
	//wxCoord				xPosition,
	//							yPosition;
	
	
			// Setting the global cursor variable to 0 will force it to be set
			// to the correct value later.
	
	//cursor = event.GetCursor ();
	//xPosition = event.GetX ();
	//yPosition = event.GetY ();
	gPresentCursor = 0;
	
	event.Skip ();
	
}	// end "OnCursorChange"



void CMImageCanvas::SetView (CMImageView* value)

{
   m_View = value;
	
}
