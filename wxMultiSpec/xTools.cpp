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
//	File:						xTools.cpp
//	Class Definition:		xTools.h
//
//	Brief description:	Part of this code come from the Microsoft Foundation Classes C++
//								library examples. For linux and MacOS, the existing code was adapted.
//
//	Language:				C
//
//	System:					Linux and Macintosh Operating Systems
//
//	Authors:					Larry L. Biehl, A.R. Maud
//
//	Revision date:			01/11/2020
//
/* Template for debugging
	int numberChars = sprintf ((char*)gTextString3,
				" xTools:: (): %s",
				gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "xMultiSpec.h"
#include "xTools.h"
#include "SDisplay_class.h"
#include "xImageView.h"
#include "SImageWindow_class.h"
#include "xImageDoc.h"

#include "wx/listimpl.cpp"
WX_DEFINE_LIST (CPtrList);


//------------------------------------------------------------------------------------
//		CMTool implementation
//------------------------------------------------------------------------------------
//	Constructor  

CPtrList CMTool::c_tools;

static CMSelectTool selectTool;

LCToWindowUnitsVariables CMSelectTool::s_lcToWindowUnitsVariables;

wxPoint 	CMTool::c_down;
wxPoint 	CMTool::c_last;

ToolType CMTool::c_toolType = selection;
bool 		CMTool::c_flag_addtolist = FALSE;
Boolean 	CMTool::c_nonClientAreaFlag = FALSE;

SInt16 	CMTool::s_selectType = kRectangleType;



CMTool::CMTool (
				ToolType									toolType)

{
    mToolType = toolType;
    c_tools.Append (this);

}	// end "CMTool::CMTool"



CMTool::~CMTool ()

{
			// Not sure if something like this is needed.
			
	//WX_CLEAR_LIST (wxToolBarToolsList, m_tools);

}	// end "CMTool::~CMTool"



CMTool* CMTool::FindTool (
				ToolType									toolType)

{
	CPtrList::Node* pos = c_tools.GetFirst ();
	while (pos)
		{
		CMTool* pTool = pos->GetData ();
		if (pTool->mToolType == toolType)
																						return pTool;

		pos = pos->GetNext ();
		
		}	// end "while (pos != NULL)"
	
	return NULL;

}	// end "CMTool::FindTool"



void CMTool::OnLButtonDblClk (
				CMImageView*						imageView,
				UInt16								nFlags)

{

}	// end "CMTool::OnLButtonDblClk"



void CMTool::OnLButtonDown (
				CMImageView*						imageView,
				UInt16								nFlags,
				const wxPoint						point)

{
   if (!((imageView->m_Canvas)->HasCapture ()))
      (imageView->m_Canvas)->CaptureMouse ();
	
	//wxPoint scrollOffset = (imageView->m_Canvas)->GetScrollPosition ();
	//c_down = point + scrollOffset;

	c_down = point;
	c_last = c_down;
	c_nonClientAreaFlag = FALSE;

}	// end "CMTool::OnLButtonDown"



void CMTool::OnLButtonUp (
				CMImageView*						imageView,
				UInt16								nFlags,
				const wxPoint						point,
				Boolean								newSelectedWindowFlag)

{
	wxPoint								localPoint;
	
	
	if (s_selectType == kRectangleType)
		{
		if ((imageView->m_Canvas)->HasCapture ())
			(imageView->m_Canvas)->ReleaseMouse ();
		
		}	// end "if (s_selectType == kRectangleType)"
	
	wxPoint scrollOffset = (imageView->m_Canvas)->GetScrollPosition ();
	localPoint = point + scrollOffset;

	if (localPoint == c_down)
		c_toolType = selection;

	c_nonClientAreaFlag = FALSE;

}	// end "CMTool::OnLButtonUp"



Boolean CMTool::OnMouseMove (
				CMImageView*						pImageView,
				UInt16								nFlags,
				const wxPoint 						point,
				Boolean								updateViewRectFlag)

{
	c_last = point;

	return (TRUE);

}	// end "CMTool::OnMouseMove"



Boolean CMTool::OnCharHook ()

{
	Boolean								returnFlag = FALSE;
	
	
	if (c_flag_addtolist == TRUE)
		{
		if (gProjectInfoPtr != NULL)
			{
			Point location;
			location.h = 0;
			location.v = 0;
		
			SInt64 windowid64 = (SInt64)((int*)(gProjectInfoPtr->addToControlH));
			SInt16 windowid = (SInt16)windowid64;
			if (gProjectWindow->GetFrame()->FindWindow (windowid)->IsEnabled ())
				{
				returnFlag = StatisticsWControlEvent (
												gProjectInfoPtr->addToControlH,
												FALSE,
												location,
												kAddToListControl);
		
				c_flag_addtolist = !returnFlag;
				
				}	// end "if (...->FindWindow (windowid)->IsEnabled ())"
			
			}	// end "if (gProjectInfoPtr != NULL)"
		
		else	// gProjectInfoPtr == NULL
			c_flag_addtolist = FALSE;
		
		}	// end "if (c_flag_addtolist == TRUE)"
	
	return (returnFlag);

}	// end "OnCharHook"



// CMSelectTool

		// Use for polygon selections so that repeated points are not selected in
		// a row.

LongPoint CMSelectTool::s_lastLineColumnPoint;

		// Start point in windows units that the cursor is allowed to roam around
		// in for the selected startChannel.

LongPoint CMSelectTool::s_startPoint = {0, 0};

Rect CMSelectTool::s_limitRectangle;

Rect CMSelectTool::s_viewRect;

SelectMode CMSelectTool::s_selectMode = none;

double CMSelectTool::s_hConversionFactor = 0;

double CMSelectTool::s_vConversionFactor = 0;

DisplaySpecsPtr CMSelectTool::s_displaySpecsPtr = NULL;

HPFieldPointsPtr CMSelectTool::s_selectionPointsPtr = NULL;

MapProjectionInfoPtr CMSelectTool::s_mapProjectionInfoPtr = NULL;

SelectionInfoPtr CMSelectTool::s_selectionInfoPtr = NULL;

SInt32 CMSelectTool::s_prevLineStart = -1;
SInt32 CMSelectTool::s_prevColumnStart = -1;
SInt32 CMSelectTool::s_prevLineEnd = -1;
SInt32 CMSelectTool::s_prevColumnEnd = -1;

UInt32 CMSelectTool::s_memoryLimitNumber = 0;
UInt32 CMSelectTool::s_bytesNeededIncrement = 0;
UInt32 CMSelectTool::s_bytesNeeded = 0;

UInt16 CMSelectTool::s_startChannel = 0;

Boolean CMSelectTool::s_initPolygonFlag = FALSE;



CMSelectTool::CMSelectTool ()
			: CMTool (selection)

{
	//kbdstate = wxKeyboardState();
	
}	// end "CMSelectTool::CMSelectTool"



Boolean CMSelectTool::InitPolygon ()

{
	if (s_initPolygonFlag)
																						return (TRUE);

	if (!InitializePolygonSelection (s_selectionInfoPtr,
												&s_selectionPointsPtr,
												&s_memoryLimitNumber,
												&s_bytesNeeded,
												&s_bytesNeededIncrement))
																						return (FALSE);

			// Indicate that there has not been a last line column polygon point.

	s_lastLineColumnPoint.h = -1;
	s_lastLineColumnPoint.v = -1;

			// Indicate that this routine does not need to be called again for this
			// polygon selection.

	s_initPolygonFlag = TRUE;

	return (TRUE);

}	//	end "InitPolygon"



void CMSelectTool::AddPolygonPoint (
				CMImageView*						imageViewCPtr,
				LongPoint							newLineColPoint)

{
	UInt32 								index;


	if (s_selectionInfoPtr->numberPoints == (SInt32)s_memoryLimitNumber)
		{
		CheckAndUnlockHandle(s_selectionInfoPtr->polygonCoordinatesHandle);

		s_bytesNeeded += s_bytesNeededIncrement;

		MSetHandleSize (&s_selectionInfoPtr->polygonCoordinatesHandle, s_bytesNeeded);

		s_selectionPointsPtr = (HPFieldPointsPtr)GetHandlePointer (
												 s_selectionInfoPtr->polygonCoordinatesHandle,
												 kLock);

		s_memoryLimitNumber += 100;

		}	// end "if (s_selectionInfoPtr->numberPoints == s_memoryLimitNumber)"

    index = s_selectionInfoPtr->numberPoints;

    s_selectionPointsPtr[index].line = newLineColPoint.v;
    s_selectionPointsPtr[index].col = newLineColPoint.h;

    s_selectionInfoPtr->numberPoints++;

    GetBoundingSelectionRectangles (s_displaySpecsPtr,
												s_selectionInfoPtr,
												s_selectionPointsPtr,
												s_startChannel);

}	// end AddPolygonPoint



void CMSelectTool::OnLButtonDblClk (
				CMImageView*						imageViewCPtr,
				UInt16								nFlags)

{
	if (s_selectType == kPolygonType)
		{
		wxRect			rect;

		LongPoint		longPoint;

		double			factor;

		Handle			windowInfoHandle;

		SInt16			coordinateViewUnits,
							unitsToUseCode;

		Boolean			useStartLineColumnFlag;
		

		s_selectMode = none;
		s_initPolygonFlag = FALSE;

		windowInfoHandle = GetWindowInfoHandle (imageViewCPtr);

				// Get the window point for the first polygon index. It may be needed
				// later. It is done here before 's_selectionPointsPtr' is invalidated.

		ConvertLCToWinPoint ((LongPoint*)&s_selectionPointsPtr[0],
									 &longPoint,
									 &s_lcToWindowUnitsVariables);

		longPoint.h += s_startChannel * gChannelWindowInterval - gChannelWindowOffset;

				// Now officially close the polygon.

		ClosePolygonSelection (s_selectionInfoPtr);
		s_selectionPointsPtr = NULL;

				// Invalidate the rectangle which inludes the line segment from the last
				// polygon point to the first polygon point. Do this only if the number
				// of polygon points is more than 1.

		if (s_selectionInfoPtr->numberPoints > 1)
			{
					// Get the rectangle that the segment from the last point to the
					// first point.

			rect.SetLeft ((int) longPoint.h);
			rect.SetTop ((int) longPoint.v);
			rect.SetRight (c_last.x);
			rect.SetBottom (c_last.y);

					// Verify that all of the new line will be drawn.

			c_last.x++;
			c_last.y++;
			
			imageViewCPtr->InvalidateRect (&rect, FALSE);
				
			}	// end "if (s_selectionInfoPtr->numberPoints > 1)"

				// Force the selected polygon to be draw in all windows if this is a
				// side by side image.

		if (gSideBySideChannels > 1)
			OutlineSelectionArea (imageViewCPtr);
       
				// Now get the bounding area in coordinate units.

		Handle mapProjectionHandle = GetFileMapProjectionHandle2(windowInfoHandle);
		MapProjectionInfoPtr mapProjectionInfoPtr =
								(MapProjectionInfoPtr)GetHandlePointer (mapProjectionHandle);

		ComputeSelectionCoordinates (windowInfoHandle,
												 mapProjectionInfoPtr,
												 &s_selectionInfoPtr->lineColumnRectangle,
												 &s_selectionInfoPtr->coordinateRectangle);

				// Show the coordinates of the rectangle that bounds the polygon.

		imageViewCPtr->UpdateSelectionCoordinates ();

				// Set this selection for all image windows if requested.
				// If shift key is down, then do not use the start line
				// and column to adjust the selections in the windows.

		useStartLineColumnFlag = TRUE;
		if (wxGetKeyState (WXK_SHIFT))
			useStartLineColumnFlag = FALSE;

		#if defined multispec_wxlin
			if (!wxGetKeyState (WXK_CONTROL))
		#endif
		#if defined multispec_wxmac
					// Note that WXK_CONTROL is the command key for Mac OS
					// WXK_RAW_CONTROL is the control key, but it cannot be
					// used for this. It does not seems to be available for
					// this. Assuming this is because it is the key for
					// contextual menus.
			//if (!wxGetKeyState (WXK_RAW_CONTROL))
			if (!wxGetKeyState (WXK_CONTROL))
		#endif
			{
			unitsToUseCode = kLineColumnUnits;

			coordinateViewUnits = GetCoordinateViewUnits (windowInfoHandle);
			if (GetCoordinateHeight (windowInfoHandle) > 0)
				{
				if (coordinateViewUnits >= kKilometersUnitsMenuItem)
					unitsToUseCode = kMapUnits;

				else if (coordinateViewUnits == kDecimalLatLongUnitsMenuItem ||
												coordinateViewUnits == kDMSLatLongUnitsMenuItem)
					unitsToUseCode = kLatLongUnits;

            }	// end "if (GetCoordinateHeight (gActiveImageWindowInfoH) > 0)"

			factor = GetCoordinateViewFactor (windowInfoHandle);

			SetSelectionForAllWindows (windowInfoHandle,
												  s_selectionInfoPtr,
												  &s_selectionInfoPtr->coordinateRectangle,
												  useStartLineColumnFlag,
												  unitsToUseCode,
												  factor);

			}	// end "if (GetKeyState (WXK_CONTROL))"

		CheckAndUnlockHandle (imageViewCPtr->GetDisplaySpecsHandle ());
		s_displaySpecsPtr = NULL;

		Handle selectionInfoHandle = GetSelectionInfoHandle (imageViewCPtr);
		CheckAndUnlockHandle (selectionInfoHandle);
		s_selectionInfoPtr = NULL;

		s_hConversionFactor = 0;
		s_vConversionFactor = 0;
		s_startChannel = 0;

				// Reset the static select type back to rectangular.

		s_selectType = kRectangleType;

				// Show selection info in graph window if needed.

		ShowGraphSelection ();
        
		c_flag_addtolist = TRUE;
		
		if ((imageViewCPtr->m_Canvas)->HasCapture ())
			(imageViewCPtr->m_Canvas)->ReleaseMouse ();
		
		}	// end "s_selectType == kPolygonType"

}	// end "OnLButtonDblClk"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CMSelectTool::OnLButtonDown
//
//	Software purpose:	Handle a selection when the left mouse button goes down.
//							This code was adapted from the Windows version.
//
//	Parameters in:		None		
//
//	Parameters out:	None			
//
//	Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 
//	Revised By:			Larry L. Biehl			Date: 04/24/2019

void CMSelectTool::OnLButtonDown (
				CMImageView*						imageViewCPtr,
				UInt16								nFlags,
				const wxPoint						point)

{
	wxPoint								localPoint;
	wxRect								rect;
	DoubleRect							coordinateRect;
	LongPoint							longLocalPoint;
	LongRect								lineColumnRect;
	Rect									displayRect;

	Handle								displaySpecsHandle,
											selectionInfoHandle,
											windowInfoHandle;
	
	int									extraSpace;
	

	c_flag_addtolist = FALSE;

	wxPoint scrollOffset = (imageViewCPtr->m_Canvas)->GetScrollPosition ();

	localPoint = point + scrollOffset;
	windowInfoHandle = GetWindowInfoHandle (imageViewCPtr);

	if (s_selectMode != netSelect)
		{
				// Get the type of selection to be done - rectangle or polygon.

		s_selectType = kRectangleType;
		if (GetProjectWindowFlag (GetWindowInfoHandle (imageViewCPtr)))
			{
			gProjectSelectionWindow = imageViewCPtr;

			if (gProjectInfoPtr->statsWindowMode == kSelectFieldMode &&
												gProjectInfoPtr->selectionType == kPolygonType)
				{
				if (gNewSelectedWindowFlag)
					{
							// Assume that this is just a selection of this window to bring
							// it to the top and not the beginning of a selection.

					gNewSelectedWindowFlag = FALSE;

					}	// end "if (gNewSelectedWindowFlag)"

				s_selectType = kPolygonType;
				
				}	// end "if (...->statsWindowMode == kSelectFieldMode && ..."

			}	// end "if (GetProjectWindowFlag (GetWindowInfoHandle (..."

		selectionInfoHandle = GetSelectionInfoHandle (imageViewCPtr);

		s_selectionInfoPtr = (SelectionInfoPtr)GetHandlePointer (selectionInfoHandle,
																					kLock);
		
				// Get Map Information Pointer

		Handle fileInfoHandle = GetFileInfoHandle (windowInfoHandle);
		Handle mapProjectionHandle = GetFileMapProjectionHandle (fileInfoHandle);

		s_mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																			mapProjectionHandle, kLock);

				// Get pointer to display specifications which will be needed later

		displaySpecsHandle = imageViewCPtr->GetDisplaySpecsHandle ();
		s_displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
																			displaySpecsHandle, kLock);

				// Get the magnification factors to use to convert from screen
				// elements to lines and columns

		s_vConversionFactor = s_displaySpecsPtr->displayedLineInterval /
																s_displaySpecsPtr->magnification;

		s_hConversionFactor = s_displaySpecsPtr->displayedColumnInterval /
																s_displaySpecsPtr->magnification;

				//	Initialize some limit variables

		GetWindowClipRectangle (imageViewCPtr, kImageArea, &s_viewRect);
		s_viewRect.left += scrollOffset.x;
		s_viewRect.right += scrollOffset.x;
		s_viewRect.top += scrollOffset.y;
		s_viewRect.bottom += scrollOffset.y;

		longLocalPoint.h = localPoint.x;
		longLocalPoint.v = localPoint.y;
		s_startChannel = GetSelectionRectangleLimits (TRUE,
																	 0,
																	 &longLocalPoint,
																	 s_displaySpecsPtr,
																	 &s_viewRect,
																	 &s_limitRectangle,
																	 &s_startPoint);
		imageViewCPtr->m_startchannel_sbs = s_startChannel;
		
				// Set the global variables needed to convert from line-column
				// units to window units.

		SetChannelWindowVariables (kToImageWindow, imageViewCPtr, kNotCoreGraphics);

		SetLCToWindowUnitVariables (windowInfoHandle,
												kWindowsUseOrigin,
												FALSE,
												&s_lcToWindowUnitsVariables);

		s_selectMode = netSelect;
	
		extraSpace = (int)ceil (s_displaySpecsPtr->magnification);

		}	// end "if (s_selectMode != netSelect)"

	longLocalPoint.h = localPoint.x;
	longLocalPoint.v = localPoint.y;

	if (s_selectType == kPolygonType)
		{
		LongPoint			newLineColPoint,
								polygonLocalPoint;
		wxRect				rect;

		if (s_selectionInfoPtr->polygonCoordinatesHandle == NULL)
			s_initPolygonFlag = FALSE;

		if (!s_initPolygonFlag)
			InitPolygon ();
		
				// Get the line and column for the input point.
				// Need to remove the scroll offset for the conversion.

		polygonLocalPoint.h = longLocalPoint.h - scrollOffset.x;
		polygonLocalPoint.v = longLocalPoint.v - scrollOffset.y;
		ConvertWinPointToLC (&polygonLocalPoint, &newLineColPoint);

		if (s_lastLineColumnPoint.h != newLineColPoint.h ||
													s_lastLineColumnPoint.v != newLineColPoint.v)
			
			{
			AddPolygonPoint (imageViewCPtr, newLineColPoint);

			AddPolyPointStatNewFieldW (newLineColPoint);

					// Remove the current 'moving' segment and force draw of the fixed
					// line segment, if there is a 'moving' segment, i.e. this is not
					// the first polygon point.

			if (s_selectionInfoPtr->numberPoints > 1)
				{
						// Get the rectangle that contains the current 'moving' line segment.

				rect.x = MIN (c_last.x, c_down.x);
				rect.y = MIN (c_last.y, c_down.y);
				rect.width = abs (c_down.x - c_last.x);
				rect.height = abs (c_down.y - c_last.y);
				
						// Verify that the new line will be drawn. The new window point may
						// be moved to the upper left to be at the upper left of the pixel
						// when the image is zoomed larger than 1.
						// Also verify that bottom and right edges are cleared.

				//rect.x -= (int)s_displaySpecsPtr->magnification;
				//rect.y -= (int)s_displaySpecsPtr->magnification;
				//rect.width += (int)(2 * s_displaySpecsPtr->magnification);
				//rect.height += (int)(2 * s_displaySpecsPtr->magnification);

				rect.x -= (scrollOffset.x + extraSpace);
				rect.y -= (scrollOffset.y + extraSpace);
				rect.width += 2 * extraSpace;
				rect.height += 2 * extraSpace;

				imageViewCPtr->InvalidateRect (&rect, FALSE);

				}	// end "if (s_selectionInfoPtr->numberPoints > 0)"

					// Update the cursor coordinates if the coordinate view is displayed.
					// The rectangle coordinates which includes the current polygon will
					// not be displayed until the polygon is closed.

			if (imageViewCPtr->GetDocument()->GetDisplayCoordinatesFlag ())
				{
				imageViewCPtr->UpdateCursorCoordinatesNoDisplayOrigin (&longLocalPoint,
																							s_displaySpecsPtr);

				}	// end "if (pImageView->GetDocument()->s_displayCoordinatesFlag)"

					// Now convert the LC point back to window units to force it
					// to be at the upper left corner of the pixel if the zoom factor
					// is greater than 1.

			ConvertLCToWinPoint (&newLineColPoint,
										&longLocalPoint,
										&s_lcToWindowUnitsVariables);

			longLocalPoint.h +=
							s_startChannel * gChannelWindowInterval - gChannelWindowOffset;

			localPoint.x = (int)longLocalPoint.h + scrollOffset.x;
			localPoint.y = (int)longLocalPoint.v + scrollOffset.y;

			s_lastLineColumnPoint = newLineColPoint;

			}	// end "if (s_lastLineColumnPoint != newLineColPoint)"

		else	// s_lastLineColumnPoint = newLineColPoint
			{
					// Do not save this 'down' point.

																								return;

			}	// end "else s_lastLineColumnPoint = newLineColPoint"
		
		}	// end "if (s_selectType == kPolygonType)"

	else if (s_selectType == kRectangleType)
		{
		displayRect.left = displayRect.right = localPoint.x;
		displayRect.top = displayRect.bottom = localPoint.y;

		ComputeSelectionLineColumns (s_displaySpecsPtr,
												 s_mapProjectionInfoPtr,
												 &lineColumnRect,
												 &displayRect,
												 &s_startPoint,
												 &coordinateRect);

		if (imageViewCPtr->GetDocument()->GetDisplayCoordinatesFlag ())
			{
			s_prevLineStart = lineColumnRect.top;
			s_prevColumnStart = lineColumnRect.left;
			s_prevLineEnd = lineColumnRect.bottom;
			s_prevColumnEnd = lineColumnRect.right;

			DrawSelectionCoordinates (windowInfoHandle,
											  &lineColumnRect,
											  &coordinateRect,
											  1);

			imageViewCPtr->UpdateCursorCoordinatesNoDisplayOrigin (&longLocalPoint,
																						s_displaySpecsPtr);

			}	// end "if (pImageView->GetDocument()->s_displayCoordinatesFlag)"

		}	// end "else if (s_selectType == kRectangleType)"

			// Do base class operations

	CMTool::OnLButtonDown (imageViewCPtr, nFlags, localPoint);

}	// end "CMSelectTool::OnLButtonDown"



void CMSelectTool::OnLButtonUp (
				CMImageView*						imageViewCPtr,
				UInt16								nFlags,
				const wxPoint						point,
				Boolean								newSelectedWindowFlag)

{
	if (imageViewCPtr->m_Canvas->HasCapture())
		{
		if (s_selectType != kPolygonType)
			imageViewCPtr->m_Canvas->ReleaseMouse ();
		
		wxPoint scrollOffset = (imageViewCPtr->m_Canvas)->GetScrollPosition ();
		c_last = point + scrollOffset;
		
		if (s_selectMode == netSelect)
			{
			wxRect				rect;
			DoubleRect			coordinateRect;
			LongRect				lineColumnRect;

			double				factor;

			Rect					displayRect;
			
			Handle				windowInfoHandle;

			SInt16				coordinateViewUnits,
									unitsToUseCode;

			Boolean				useStartLineColumnFlag;

			windowInfoHandle = GetWindowInfoHandle (imageViewCPtr);

			if (s_selectType == kRectangleType)
				{
				if (!newSelectedWindowFlag || c_down.x != c_last.x || c_down.y != c_last.y)
					{
							// Remove the current selection.
					
					ClearSelectionArea (imageViewCPtr);
					
							// Create the final selection rectangle
					
					rect.x = MIN (c_last.x, c_down.x);
					rect.y = MIN (c_last.y, c_down.y);
					rect.width = abs (c_down.x - c_last.x);
					rect.height = abs (c_down.y - c_last.y);

					displayRect.left = rect.x;
					displayRect.top = rect.y;
					displayRect.right = rect.x + rect.width;
					displayRect.bottom = rect.y + rect.height;
					
					ComputeSelectionLineColumns (s_displaySpecsPtr,
															 s_mapProjectionInfoPtr,
															 &lineColumnRect,
															 &displayRect,
															 &s_startPoint,
															 &coordinateRect);

					SetSelectionInformation (imageViewCPtr,
													 s_displaySpecsPtr,
													 s_selectionInfoPtr,
													 s_selectType,
													 s_startChannel,
													 &lineColumnRect,
													 &coordinateRect);

							// Force the new selection to be drawn.

					OutlineSelectionArea (imageViewCPtr);

							// Add to statistics window if needed.

					WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																						windowInfoHandle);

					if (windowInfoPtr->projectWindowFlag)
						{
						gProjectSelectionWindow = imageViewCPtr;
						
						LoadNewFieldListBox ();

						}	// end "if (windowInfoPtr->projectWindowFlag)"
	
							// Set this selection for all image windows if requested.
							// If shift key is down, then do not use the start line
							// and column to adjust the selections in the windows.
					
					useStartLineColumnFlag = TRUE;
					if (imageViewCPtr->GetShiftKeyFlag ())
						useStartLineColumnFlag = FALSE;

					if (imageViewCPtr->GetControlKeyFlag ())
						{
						unitsToUseCode = kLineColumnUnits;
	
						coordinateViewUnits = GetCoordinateViewUnits (windowInfoHandle);
						if (GetCoordinateHeight (windowInfoHandle) > 0)
							{
							if (coordinateViewUnits >= kKilometersUnitsMenuItem)
								unitsToUseCode = kMapUnits;

							else if (coordinateViewUnits == kDecimalLatLongUnitsMenuItem ||
												coordinateViewUnits == kDMSLatLongUnitsMenuItem)
								unitsToUseCode = kLatLongUnits;

							}	// end "if (GetCoordinateHeight (windowInfoHandle) > 0)"

						factor = GetCoordinateViewFactor (windowInfoHandle);

						SetSelectionForAllWindows (windowInfoHandle,
															s_selectionInfoPtr,
															&coordinateRect,
															useStartLineColumnFlag,
															unitsToUseCode,
															factor);

						}	// end "if (imageViewCPtr->GetControlKeyFlag ())"

							// This is the selection rectangle so we are done.
							// Unlock the display specification handle

					CheckAndUnlockHandle (imageViewCPtr->GetDisplaySpecsHandle ());
					s_displaySpecsPtr = NULL;

					Handle fileInfoHandle = GetFileInfoHandle (windowInfoHandle);
					Handle mapProjectionHandle = GetFileMapProjectionHandle (fileInfoHandle);
					CheckAndUnlockHandle (mapProjectionHandle);
					s_mapProjectionInfoPtr = NULL;

					Handle selectionInfoHandle = GetSelectionInfoHandle (imageViewCPtr);
					CheckAndUnlockHandle (selectionInfoHandle);
					s_selectionInfoPtr = NULL;

					s_hConversionFactor = 0;
					s_vConversionFactor = 0;
					s_startChannel = 0;

							// Show selection info in graph window if needed.

					ShowGraphSelection ();

					}	// end "if (!newSelectedWindowFlag || ..."

				imageViewCPtr->UpdateSelectionCoordinates ();

				s_selectMode = none;

            }	// end "if (s_selectType == kRectangleType)"

			}	// end "if (selectMode == netSelect)"

		}	// end "if (imageViewCPtr->GetCapture() == imageViewCPtr)"

	c_flag_addtolist = TRUE;
	CMTool::OnLButtonUp (imageViewCPtr, nFlags, point, newSelectedWindowFlag);

}	// end "CMSelectTool::OnLButtonUp"



Boolean CMSelectTool::OnMouseMove (
				CMImageView*						pImageView,
				UInt16								nFlags,
				const wxPoint						point,
				Boolean								updateViewRectFlag)

{
	wxPoint								localCPoint,
											scrollOffset;
	
	wxRect								rect;

	DoubleRect							coordinateRect;
	
	LCToWindowUnitsVariables 		lcToWindowUnitsVariables;

	LongRect								lineColumnRect;
	
	LongPoint							localPoint;

	Rect									displayRect;
	
	HPFieldPointsPtr 					selectionPointsPtr;
	SelectionInfoPtr 					selectionInfoPtr;

	UInt32								numberPixels;
	
	int									extraSpace;

	Boolean								continueFlag;
	
	SignedByte 							handleStatus,
											selectionPointsStatus;


	
			// If image was scrolled, the view rectangle needs to be updated.

	scrollOffset = (pImageView->m_Canvas)->GetScrollPosition ();
	if (updateViewRectFlag)
		{
		GetWindowClipRectangle (pImageView, kImageArea, &s_viewRect);
		s_viewRect.left += scrollOffset.x;
		s_viewRect.right += scrollOffset.x;
		s_viewRect.top += scrollOffset.y;
		s_viewRect.bottom += scrollOffset.y;

		LongPoint tempLongPoint;
		tempLongPoint.h = c_down.x;
		tempLongPoint.v = c_down.y;
		s_startChannel = GetSelectionRectangleLimits (TRUE,
																	  0,
																	  &tempLongPoint,
																	  s_displaySpecsPtr,
																	  &s_viewRect,
																	  &s_limitRectangle,
																	  &s_startPoint);
		
		}	// end "if (updateViewRectFlag)"
	
	continueFlag = TRUE;
	if (s_selectMode != netSelect)
		continueFlag = FALSE;

	else if (s_selectType == kPolygonType &&
            (s_selectionInfoPtr == NULL || s_selectionInfoPtr->numberPoints <= 0))
		continueFlag = FALSE;

	if (continueFlag)
		{
		localCPoint = point + scrollOffset;

		localPoint.h = localCPoint.x;
		localPoint.v = localCPoint.y;
      
		if (localCPoint != c_last)
			{
			Handle windowInfoHandle = GetWindowInfoHandle (pImageView);
	
			extraSpace = (int)ceil (s_displaySpecsPtr->magnification);
			
			if (s_selectType == kPolygonType)
				{
				Handle selectionInfoH = GetSelectionInfoHandle (windowInfoHandle);
				selectionInfoPtr =
							(SelectionInfoPtr)GetHandleStatusAndPointer (selectionInfoH,
																						&handleStatus);
				selectionPointsPtr =
							(HPFieldPointsPtr)GetHandleStatusAndPointer (
														selectionInfoPtr->polygonCoordinatesHandle,
														&selectionPointsStatus);
			
				SetChannelWindowVariables (kToImageWindow, pImageView, kNotCoreGraphics);
				SetLCToWindowUnitVariables (windowInfoHandle,
														kWindowsUseOrigin,
														FALSE,
														&lcToWindowUnitsVariables);
				
				}	// end "if (s_selectType == kPolygonType)"
			
					// Get the current selection rectangle.
			
			if (s_selectType == kRectangleType)
				{
				rect.x = MIN (c_last.x, c_down.x);
				rect.y = MIN (c_last.y, c_down.y);
				rect.width = abs (c_down.x - c_last.x);
				rect.height = abs (c_down.y - c_last.y);
				
				rect.x -= (scrollOffset.x + extraSpace);
				rect.y -= (scrollOffset.y + extraSpace);
				rect.width += 2 * extraSpace;
				rect.height += 2 * extraSpace;
				
				}	// end "if (s_selectType == kRectangleType)"
			
			else	// s_selectType == kPolygonType)
				{
				wxPoint lastPointLessScroll = c_last - scrollOffset;
				GetPolygonEnclosingRectangle (selectionPointsPtr,
														selectionInfoPtr->numberPoints,
														&lcToWindowUnitsVariables,
														&lastPointLessScroll,
														&rect);
				
				}	// end "else s_selectType == kPolygonType)"
			
					// Clear the current selection area
			
			pImageView->InvalidateRect (&rect, FALSE);

					// Peg the selection to the edge of the image if needed.

			localCPoint.y = MIN (localCPoint.y, s_limitRectangle.bottom - 1);
			localCPoint.x = MIN (localCPoint.x, s_limitRectangle.right - 1);

			localCPoint.y = MAX (localCPoint.y, s_limitRectangle.top);
			localCPoint.x = MAX (localCPoint.x, s_limitRectangle.left);
			
					// Now force area to be refreshed where the new selection will be
					// if not the same as the previous area
			
			if (localCPoint != c_last)
				{
				if (s_selectType == kRectangleType)
					{
					rect.x = MIN (localCPoint.x, c_down.x);
					rect.y = MIN (localCPoint.y, c_down.y);
					rect.width = abs (c_down.x - localCPoint.x);
					rect.height = abs (c_down.y - localCPoint.y);
					
					displayRect.left = rect.x;
					displayRect.right = rect.x + rect.width;
					displayRect.top = rect.y;
					displayRect.bottom = rect.y + rect.height;
					ComputeSelectionLineColumns (s_displaySpecsPtr,
															s_mapProjectionInfoPtr,
															&lineColumnRect,
															&displayRect,
															&s_startPoint,
															&coordinateRect);
					
					rect.x -= (scrollOffset.x + extraSpace);
					rect.y -= (scrollOffset.y + extraSpace);
					rect.width += 2 * extraSpace;
					rect.height += 2 * extraSpace;
					
					}	// end "if (s_selectType == kRectangleType)"
				
				else	// s_selectType == kPolygonType
					{
					wxPoint currentPointLessScroll = localCPoint - scrollOffset;
					GetPolygonEnclosingRectangle (selectionPointsPtr,
															selectionInfoPtr->numberPoints,
															&lcToWindowUnitsVariables,
															&currentPointLessScroll,
															&rect);
			
					}	// end "else s_selectType == kPolygonType)"

						// Invalidate the rectangle that includes the current
						// selection line segment. Do not erase the background
						// before drawing over. This reduces flashing.
				
				pImageView->InvalidateRect (&rect, FALSE);

				if (pImageView->GetDocument()->GetDisplayCoordinatesFlag ())
					{
					if (s_selectType == kRectangleType)
						{
						if (lineColumnRect.top != s_prevLineStart ||
										 lineColumnRect.left != s_prevColumnStart ||
										 lineColumnRect.bottom != s_prevLineEnd ||
										 lineColumnRect.right != s_prevColumnEnd)
							{
							s_prevLineStart = lineColumnRect.top;
							s_prevColumnStart = lineColumnRect.left;
							s_prevLineEnd = lineColumnRect.bottom;
							s_prevColumnEnd = lineColumnRect.right;

							numberPixels = (s_prevLineEnd - s_prevLineStart + 1) *
											  (s_prevColumnEnd - s_prevColumnStart + 1);

							DrawSelectionCoordinates (GetWindowInfoHandle (pImageView),
																&lineColumnRect,
																&coordinateRect,
																numberPixels);

							}	// end "if (lineColumnRect.top != ..."

						}	// end "if (s_selectType == kRectangleType)"

					LongPoint longLocalPoint;
					longLocalPoint.h = localCPoint.x;
					longLocalPoint.v = localCPoint.y;
					pImageView->UpdateCursorCoordinatesNoDisplayOrigin (
															&longLocalPoint, s_displaySpecsPtr);

					}	// end "if (pImageView->GetDocument()->s_displayCoordinatesFlag)"
				
				}	// end "if (localCPoint != c_last"
			
			CMTool::OnMouseMove (pImageView, nFlags, localCPoint, updateViewRectFlag);

			}	// end "if (localCPoint != c_last)"

		return (TRUE);

		}	// end "if (selectMode == netSelect && ..."

	return (FALSE);

}	// end "CMSelectTool::OnMouseMove"

