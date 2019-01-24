//									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//                 Purdue University
//						West Lafayette, IN 47907
//						 Copyright (1988-2019)
//						(c) Purdue Research Foundation
//                     All rights reserved.
//
//	File:						LTools.cpp
//
//	Functions in file:	
//
//	Brief description:	Part of this code come from the Microsoft Foundation Classes C++
//								library examples. For linux, the existing code was adapted.
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
//	Language:				C
//
//	System:					Linux Operating System
//
//	Authors:					Larry L. Biehl, A.R. Maud
//
//	Revision date:			01/18/2019
//
/* Template for debugging
	int numberChars = sprintf ((char*)gTextString3,
				" LTools:: (): %s",
				gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "LMultiSpec.h"
#include "LTools.h"
#include "CDisplay.h"
#include "LImageView.h"
#include "CImageWindow.h"
#include "CDisplay.h"
#include "LImageDoc.h"
#include "SExternalGlobals.h"

#include "wx/listimpl.cpp"
WX_DEFINE_LIST(CPtrList);


//------------------------------------------------------------------------------------
//		CMTool implementation
//------------------------------------------------------------------------------------
//	Constructor  

CPtrList CMTool::c_tools;

static CMSelectTool selectTool;

LCToWindowUnitsVariables CMSelectTool::s_lcToWindowUnitsVariables;

CPoint CMTool::c_down;
CPoint CMTool::c_last;

// Used by the idle handler.  
CPoint CMTool::c_lastMousePoint;

ToolType CMTool::c_toolType = selection;
Boolean CMTool::c_nonClientAreaFlag = FALSE;
bool CMTool::c_flag_addtolist = FALSE;

SInt16 CMTool::s_selectType = kRectangleType;

CMTool::CMTool (
				ToolType								toolType)

{

    mToolType = toolType;
    c_tools.Append(this);

}	// end "CMTool::CMTool"



CMTool* CMTool::FindTool (
				ToolType									toolType)
{
	//POSITION pos = c_tools.GetHeadPosition();
	CPtrList::Node* pos = c_tools.GetFirst ();
	while (pos)
		{
		//CMTool* pTool = (CMTool*) c_tools.GetNext(pos);
		CMTool* pTool = pos->GetData();
		if (pTool->mToolType == toolType)
																							return pTool;

		pos = pos->GetNext();
		
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
				const CPoint						point)

{
   if (!((imageView->m_Canvas)->HasCapture ()))
      (imageView->m_Canvas)->CaptureMouse ();

	c_down = point;
	c_last = point;
	c_lastMousePoint = point;
	c_nonClientAreaFlag = FALSE;

}	// end "CMTool::OnLButtonDown"



void CMTool::OnLButtonUp (
				CMImageView*						imageView,
				UInt16								nFlags,
				const CPoint						point,
				Boolean								newSelectedWindowFlag)

{
	if (s_selectType == kRectangleType)
		{
		if ((imageView->m_Canvas)->HasCapture ())
			(imageView->m_Canvas)->ReleaseMouse ();
		
		}	// end "if (s_selectType == kRectangleType)"

	if (point == c_down)
		c_toolType = selection;

	//c_last = point;
	c_nonClientAreaFlag = FALSE;

}	// end "CMTool::OnLButtonUp"



Boolean CMTool::OnMouseMove (
				CMImageView*						pImageView,
				UInt16								nFlags,
				const									CPoint point)

{

    c_last = point;

    return (TRUE);

}	// end "CMTool::OnMouseMove"


/*
void CMTool::OnChar (
				CMImageView*						pView,
				unsigned int						nChar)
{

}	// end "OnChar"
*/


void CMTool::OnCharHook ()

{
	if (c_flag_addtolist == TRUE)
		{
		if (gProjectInfoPtr != NULL)
			{
			Boolean	returnFlag;
			
			Point location;
			location.h = 0;
			location.v = 0;
		
			returnFlag = StatisticsWControlEvent (
												gProjectInfoPtr->addToControlH,
												FALSE,
												location,
												kAddToListControl);
		
			c_flag_addtolist = !returnFlag;
			
			}	// end "if (gProjectInfoPtr != NULL)"
		
		else	// gProjectInfoPtr == NULL
			c_flag_addtolist = FALSE;
		
		}	// end "if (c_flag_addtolist == TRUE)"

}	// end "OnCharHook"



////////////////////////////////////////////////////////////////////////////
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
	//Handle						selectionInfoHandle;


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
	UInt32 index;


	if (s_selectionInfoPtr->numberPoints == (SInt32) s_memoryLimitNumber)
		{
		CheckAndUnlockHandle(s_selectionInfoPtr->polygonCoordinatesHandle);

		s_bytesNeeded += s_bytesNeededIncrement;

		MSetHandleSize(&s_selectionInfoPtr->polygonCoordinatesHandle, s_bytesNeeded);

		s_selectionPointsPtr = (HPFieldPointsPtr) GetHandlePointer (
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


/*
void CMSelectTool::OnChar (
				CMImageView*						imageViewCPtr,
				unsigned int						nChar)

{

}	// end "OnChar"
*/


void CMSelectTool::OnLButtonDblClk (
				CMImageView*						imageViewCPtr,
				UInt16								nFlags)

{
	if (s_selectType == kPolygonType)
		{
		CRect				rect;

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

		ClosePolygonSelection(s_selectionInfoPtr);
		s_selectionPointsPtr = NULL;

				// Invalidate the rectangle which inludes the line segment from the last
				// polygon point to the first polygon point. Do this only if the number
				// of polygon points is more than 1.

		if (s_selectionInfoPtr->numberPoints > 1)
			{
					// Get the rectangle that the segment from the last point to the
					// first point.

			rect.SetLeft((int) longPoint.h);
			rect.SetTop((int) longPoint.v);
			rect.SetRight(c_last.x);
			rect.SetBottom(c_last.y);
			
					// No auto-correction for the rectangle
					// rect.NormalizeRect();

					// Verify that all of the new line will be drawn.

			c_last.x++;
			c_last.y++;
			
			imageViewCPtr->InvalidateRect (&rect, FALSE);
			imageViewCPtr->m_Canvas->Update ();
				
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

		if (!wxGetKeyState (WXK_CONTROL))
			{
			unitsToUseCode = kLineColumnUnits;

			coordinateViewUnits = GetCoordinateViewUnits(windowInfoHandle);
			if (GetCoordinateHeight(windowInfoHandle) > 0)
				{
				if (coordinateViewUnits >= kKilometersUnitsMenuItem)
					unitsToUseCode = kMapUnits;

				else if (coordinateViewUnits == kDecimalLatLongUnitsMenuItem ||
														coordinateViewUnits == kDMSLatLongUnitsMenuItem)
					unitsToUseCode = kLatLongUnits;

            }	// end "if (GetCoordinateHeight (gActiveImageWindowInfoH) > 0)"

			factor = GetCoordinateViewFactor(windowInfoHandle);

			SetSelectionForAllWindows (windowInfoHandle,
												  s_selectionInfoPtr,
												  &s_selectionInfoPtr->coordinateRectangle,
												  useStartLineColumnFlag,
												  unitsToUseCode,
												  factor);

			}	// end "if (GetKeyState(VK_CONTROL) < 0)"

		CheckAndUnlockHandle(imageViewCPtr->GetDisplaySpecsHandle ());
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

		ShowGraphSelection();
        
		c_flag_addtolist = TRUE;
		
		}	// end "s_selectType == kPolygonType"

}	// end "OnLButtonDblClk"



//------------------------------------------------------------------------------------
//							Copyright (1988-20018)
//						(c) Purdue Research Foundation
//							All rights reserved.
//
//	Function name:		void CMSelectTool::OnLButtonDown
//
//	Software purpose:	
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
//	Revised By:			Larry L. Biehl			Date: 04/24/2018

void CMSelectTool::OnLButtonDown (
				CMImageView*						imageViewCPtr,
				UInt16								nFlags,
				const CPoint						point)

{
	CPoint								localPoint;
	CRect									rect;
	DoubleRect							coordinateRect;
	LongPoint							longLocalPoint;
	LongRect								lineColumnRect;
	Rect									displayRect;

	Handle								displaySpecsHandle,
											selectionInfoHandle,
											windowInfoHandle;
	

	//if (!gNewSelectedWindowFlag)
	//	{
		c_flag_addtolist = FALSE;

		localPoint = point;
		windowInfoHandle = GetWindowInfoHandle (imageViewCPtr);

				// Make sure that the start point is not off the top or left of the image.

		 //localPoint.x = MAX(localPoint.x, 0);
		 //localPoint.y = MAX(localPoint.y, imageViewCPtr->GetTitleHeight ());

		 if (s_selectMode != netSelect)
			{
					// Get the type of selection to be done - rectangle or polygon.

			s_selectType = kRectangleType;
			if (GetProjectWindowFlag (GetWindowInfoHandle (imageViewCPtr)))
				{
				gProjectSelectionWindow = imageViewCPtr;

				if (gStatisticsMode > 0 && gProjectInfoPtr->selectionType == kPolygonType)
					{
					if (gNewSelectedWindowFlag)
						{
								// Assume that this is just a selection of this window to bring
								// it to the top and not the beginning of a selection.

						gNewSelectedWindowFlag = FALSE;

						}	// end "if (gNewSelectedWindowFlag)"

					s_selectType = kPolygonType;
						  
					}	// end "if (gStatisticsMode > 0 && ..."

				}	// end "if (GetProjectWindowFlag (GetWindowInfoHandle (..."

			selectionInfoHandle = GetSelectionInfoHandle (imageViewCPtr);

			s_selectionInfoPtr = (SelectionInfoPtr)GetHandlePointer (selectionInfoHandle,
																							kLock);
			
					// Get Map Information Pointer

			Handle fileInfoHandle = GetFileInfoHandle(windowInfoHandle);
			Handle mapProjectionHandle = GetFileMapProjectionHandle(fileInfoHandle);

			s_mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																				mapProjectionHandle, kLock);

					// Get pointer to display specifications which will be needed later

			displaySpecsHandle = imageViewCPtr->GetDisplaySpecsHandle();
			s_displaySpecsPtr = (DisplaySpecsPtr) GetHandlePointer(
						 displaySpecsHandle, kLock, kNoMoveHi);

					// Get the magnification factors to use to convert from screen
					// elements to lines and columns

			s_vConversionFactor = s_displaySpecsPtr->displayedLineInterval /
																	s_displaySpecsPtr->magnification;

			s_hConversionFactor = s_displaySpecsPtr->displayedColumnInterval /
																	s_displaySpecsPtr->magnification;

					//	Initialize some limit variables

			GetWindowClipRectangle(imageViewCPtr, kImageArea, &s_viewRect);

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

			}	// end "if (s_selectMode != netSelect)"

		longLocalPoint.h = localPoint.x;
		longLocalPoint.v = localPoint.y;

		if (s_selectType == kPolygonType)
			{
			LongPoint		newLineColPoint;
			CRect				rect;

			if (s_selectionInfoPtr->polygonCoordinatesHandle == NULL)
				s_initPolygonFlag = FALSE;

			if (!s_initPolygonFlag)
				InitPolygon();
					 
					// Get the line and column for the input point.

			ConvertWinPointToLC(&longLocalPoint, &newLineColPoint);

			if (s_lastLineColumnPoint.h != newLineColPoint.h ||
														s_lastLineColumnPoint.v != newLineColPoint.v)
				
				{
				AddPolygonPoint (imageViewCPtr, newLineColPoint);

				AddPolyPointStatNewFieldW(newLineColPoint);

						// Remove the current 'moving' segment and force draw of the fixed
						// line segment, if there is a 'moving' segment, i.e. this is not
						// the first polygon point.

				if (s_selectionInfoPtr->numberPoints > 1)
					{
							// Get the rectangle that contains the current 'moving' line segment.

					//rect.SetRect(c_down.x, c_down.y, c_last.x, c_last.y);
					//rect.NormalizeRect();
					if (c_last.x > c_down.x)
						{
						rect.x = c_down.x;
						rect.width = c_last.x - c_down.x;
						
						}	// end "if (c_last.x > c_down.x)"
					
					else	// c_last.x <= c_down.x
						{
						rect.x = c_last.x;
						rect.width = c_down.x - c_last.x;
						
						}	// end "else c_last.x <= c_down.x"
					
					if (c_last.y > c_down.y)
						{
						rect.y = c_down.y;
						rect.height = c_last.y - c_down.y;
						
						}	// end "if (c_last.y > c_down.y)"
					
					else	// c_last.y <= c_down.y
						{
						rect.y = c_last.y;
						rect.height = c_down.y - c_last.y;
						
						}	// end "else c_last.y <= c_down.y"
					 
							// Verify that the new line will be drawn. The new window point may
							// be moved to the upper left to be at the upper left of the pixel
							// when the image is zoomed larger than 1.
							// Also verify that bottom and right edges are cleared.

					rect.x -= (int)s_displaySpecsPtr->magnification;
					rect.y -= (int)s_displaySpecsPtr->magnification;
					rect.width = rect.width++ + (int)s_displaySpecsPtr->magnification;
					rect.height = rect.height++ + (int)s_displaySpecsPtr->magnification;

					imageViewCPtr->InvalidateRect (&rect, FALSE);
					imageViewCPtr->OnUpdate (NULL, NULL);

					}	// end "if (s_selectionInfoPtr->numberPoints > 0)"

						// Update the cursor coordinates if the coordinate view is displayed.
						// The rectangle coordinates which includes the current polygon will
						// not be displayed until the polygon is closed.

				if (imageViewCPtr->GetDocument()->GetDisplayCoordinatesFlag ())
					{
					imageViewCPtr->UpdateCursorCoordinates (&longLocalPoint);

					}	// end "if (pImageView->GetDocument ()->s_displayCoordinatesFlag)"

						// Now convert the LC point back to window units to force it
						// to be at the upper left corner of the pixel if the zoom factor
						// is greater than 1.

				ConvertLCToWinPoint(&newLineColPoint,
							  &longLocalPoint,
							  &s_lcToWindowUnitsVariables);

				longLocalPoint.h +=
								s_startChannel * gChannelWindowInterval - gChannelWindowOffset;

				localPoint.x = (int) longLocalPoint.h;
				localPoint.y = (int) longLocalPoint.v;

				s_lastLineColumnPoint = newLineColPoint;

				}	// end "if (s_lastLineColumnPoint != newLineColPoint)"

			else	// s_lastLineColumnPoint = newLineColPoint
				{
						// Do not save this 'down' point.

																											return;

				}	// end "else s_lastLineColumnPoint = newLineColPoint"
			  
			}	// end "if (s_selectType == kPolygonType)"

		else	// s_selectType == kRectangleType
			{
			rect.x = localPoint.x;
			rect.width = localPoint.x - localPoint.x;

			rect.y = localPoint.y;
			rect.height = localPoint.y - localPoint.y;

			displayRect.left = displayRect.right = localPoint.x;
			displayRect.top = displayRect.bottom = localPoint.y;

			ComputeSelectionLineColumns (s_displaySpecsPtr,
													 s_mapProjectionInfoPtr,
													 &lineColumnRect,
													 &displayRect,
													 &s_startPoint,
													 &coordinateRect);

			if (imageViewCPtr->GetDocument ()->GetDisplayCoordinatesFlag ())
				{
				s_prevLineStart = lineColumnRect.top;
				s_prevColumnStart = lineColumnRect.left;
				s_prevLineEnd = lineColumnRect.bottom;
				s_prevColumnEnd = lineColumnRect.right;

				DrawSelectionCoordinates (windowInfoHandle,
												  &lineColumnRect,
												  &coordinateRect,
												  1);

				imageViewCPtr->UpdateCursorCoordinates (&longLocalPoint);

				}	// end "if (pImageView->GetDocument()->s_displayCoordinatesFlag)"

			}	// end "else s_selectType == kRectangleType"
		
	//	}	// end "if (!gNewSelectedWindowFlag)"

			// Do base class operations

	CMTool::OnLButtonDown (imageViewCPtr, nFlags, localPoint);

}	// end "CMSelectTool::OnLButtonDown"



void CMSelectTool::OnLButtonUp (
				CMImageView*						imageViewCPtr,
				UInt16								nFlags,
				const CPoint						point,
				Boolean								newSelectedWindowFlag)

{
	if (imageViewCPtr->m_Canvas->HasCapture())
		{
		if (s_selectType != kPolygonType)
			imageViewCPtr->m_Canvas->ReleaseMouse ();
		
		c_last = point;
		if (s_selectMode == netSelect)
			{
			CRect				rect;
			DoubleRect		coordinateRect;
			LongRect			lineColumnRect;

			double			factor;

			Rect				displayRect;
			
			Handle			windowInfoHandle;

			SInt16			coordinateViewUnits,
								unitsToUseCode;

			Boolean			useStartLineColumnFlag;

			windowInfoHandle = GetWindowInfoHandle (imageViewCPtr);

			if (s_selectType == kRectangleType)
				{
				if (!newSelectedWindowFlag || c_down.x != c_last.x || c_down.y != c_last.y)
					{
					wxClientDC dc(imageViewCPtr->m_Canvas);
					dc.SetBrush(*wxTRANSPARENT_BRUSH);
					dc.SetPen(wxPen(*wxWHITE, 1, wxPENSTYLE_DOT));
					
							// Remove the current selection.

					if (c_last.x > c_down.x)
						{
						rect.x = c_down.x;
						rect.width = c_last.x - c_down.x;
						
						}	// end "if (c_last.x > c_down.x)"
					
					else	// c_last.x <= c_down.x
						{
						rect.x = c_last.x;
						rect.width = c_down.x - c_last.x;
						
						}	// end "else c_last.x <= c_down.x"
					
					if (c_last.y > c_down.y)
						{
						rect.y = c_down.y;
						rect.height = c_last.y - c_down.y;
						
						}	// end "if (c_last.y > c_down.y)"
					
					else	// c_last.y <= c_down.y
						{
						rect.y = c_last.y;
						rect.height = c_down.y - c_last.y;
						
						}	// end "else c_last.y <= c_down.y"

					dc.DrawRectangle(rect);
					ClearSelectionArea (imageViewCPtr);

							// Create the final selection rectangle

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
						LoadNewFieldListBox();

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
				const CPoint						point)

{
	CPoint								localCPoint;

	DoubleRect							coordinateRect;

	LongRect								lineColumnRect;

	LongPoint							localPoint;

	Rect									displayRect;

	UInt32								numberPixels;

	Boolean								continueFlag,
											selectionClearedFlag;


	continueFlag = TRUE;
	if (s_selectMode != netSelect)
		continueFlag = FALSE;

	else if (s_selectType == kPolygonType &&
            (s_selectionInfoPtr == NULL || s_selectionInfoPtr->numberPoints <= 0))
		continueFlag = FALSE;

	if (continueFlag)
		{
				// First, save this mouse point in case it is needed by the idle handler
				// when in selection mode.

		c_lastMousePoint = point;

		localCPoint = point;

		localPoint.h = point.x;
		localPoint.v = point.y;
      
		if (point != c_last)
			{
			Handle windowInfoHandle = GetWindowInfoHandle(pImageView);
                  
			CRect		rect;

			wxClientDC dc(pImageView->m_Canvas);
			dc.SetBrush(*wxTRANSPARENT_BRUSH);
			dc.SetPen (wxPen(*wxWHITE, 1, wxPENSTYLE_DOT));
         dc.SetPen (wxPen(*wxBLACK, 1, wxPENSTYLE_DOT));
         
			CPoint scrollOffset;

			int		maxHRange,
						maxVRange;


			selectionClearedFlag = FALSE;

					// Get the scroll info. It will be used in determining whether
					// the image needs to be scrolled.
			
			scrollOffset = (pImageView->m_Canvas)->GetScrollPosition();
			maxHRange = (pImageView->m_Canvas)->GetScrollRange(wxHORIZONTAL);
			maxVRange = (pImageView->m_Canvas)->GetScrollRange(wxVERTICAL);
			
					// Get the current selection rectangle.

			if (c_last.x > c_down.x)
				{
				rect.x = c_down.x;
				rect.width = c_last.x - c_down.x;
				
            }
				
			else
				{
				rect.x = c_last.x;
				rect.width = c_down.x - c_last.x;
				
            }
				
			if (c_last.y > c_down.y)
				{
				rect.y = c_down.y;
				rect.height = c_last.y - c_down.y;
				
            }
				
			else
				{
				rect.y = c_last.y;
				rect.height = c_down.y - c_last.y;
				
            }
			
			if ((localPoint.h < s_viewRect.left && scrollOffset.x > 0) ||
						(localPoint.h > s_viewRect.right && scrollOffset.x < maxHRange) ||
						(localPoint.v < s_viewRect.top && scrollOffset.y > 0) ||
						(localPoint.v > s_viewRect.bottom && scrollOffset.y < maxVRange))
				{
				SInt32		hOffset,
								vOffset;

						// Clear the current selection.

				if (s_selectType == kPolygonType)
					{
							// Verify that bottom and right edges are cleared.
					
					rect.width++;
					rect.height++;
					pImageView->InvalidateRect(&rect, FALSE);
					pImageView->OnUpdate(NULL,NULL);

					}	// end "if (s_selectType == kPolygonType)"

				else	// s_selectType == kRectangleType
					{
					dc.DrawRectangle (rect);
					dc.SetPen (wxPen (*wxBLACK, 1, wxPENSTYLE_DOT));
					rect.x += 1;
					rect.y += 1;
					rect.width -= 2;
					rect.height -= 2;
					dc.DrawRectangle (rect);
					
					}	// end "else s_selectType == kRectangleType"

				selectionClearedFlag = TRUE;

						// Something not right below.
				hOffset = s_displaySpecsPtr->origin[kHorizontal];
				vOffset = s_displaySpecsPtr->origin[kVertical];

				hOffset -= s_displaySpecsPtr->origin[kHorizontal];
				vOffset -= s_displaySpecsPtr->origin[kVertical];

				hOffset = (SInt16)(hOffset * s_displaySpecsPtr->magnification);
				vOffset = (SInt16)(vOffset * s_displaySpecsPtr->magnification);

				c_down.x += (SInt16)hOffset;
				c_down.y += (SInt16)vOffset;

				c_last.x += (SInt16)hOffset;
				c_last.y += (SInt16)vOffset;

				if (hOffset != 0 || vOffset != 0)
					{
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

					}	// end "if (hOffset != 0 || vOffset != 0)"

				}	// end "if ( (localPoint.h < s_viewRect.left && ..."

					// Peg the selection to the edge of the image if needed.

			localCPoint.y = MIN(localCPoint.y, s_limitRectangle.bottom - 1);
			localCPoint.x = MIN(localCPoint.x, s_limitRectangle.right - 1);

			localCPoint.y = MAX(localCPoint.y, s_limitRectangle.top);
			localCPoint.x = MAX(localCPoint.x, s_limitRectangle.left);

			if (s_selectType == kPolygonType)
				{
						// initialization to save selected polygon datapoints
				
            LongPoint windowPoint;
            Handle selectionInfoH = GetSelectionInfoHandle (windowInfoHandle);
            SignedByte handleStatus;
            SignedByte selectionPointsStatus;
            SelectionInfoPtr selectionInfoPtr =
								(SelectionInfoPtr)GetHandleStatusAndPointer (selectionInfoH,
																							&handleStatus,
																							kNoMoveHi);
            HPFieldPointsPtr selectionPointsPtr =
								(HPFieldPointsPtr)GetHandleStatusAndPointer (
															selectionInfoPtr->polygonCoordinatesHandle,
															&selectionPointsStatus,
															kNoMoveHi);
				
            LCToWindowUnitsVariables lcToWindowUnitsVariables; 
            SetChannelWindowVariables (kToImageWindow, pImageView, kNotCoreGraphics);
            SetLCToWindowUnitVariables (windowInfoHandle,
														kWindowsUseOrigin,
														FALSE,
														&lcToWindowUnitsVariables);
				
				if (localCPoint.x != c_last.x || localCPoint.y != c_last.y)
					{
							// Erase the old line if needed and draw the new line
               
					if (!selectionClearedFlag)
						{
								// Verify that bottom and right edges are cleared.

						rect.width++;
						rect.height++;

								// Invalidate the rectangle that includes the current
								// selection line segment. Do not erase the background
								// before drawing over. This reduces flashing.
                  
						pImageView->InvalidateRect (&rect, FALSE);
                  pImageView->m_Canvas->Refresh ();
						
						}	// end "if (!selectionClearedFlag)"

					}	// end "if (localCPoint.x != c_last.x || ..."

            if (!selectionClearedFlag)
					{
					wxPen pOldPen;               
					pOldPen = dc.GetPen();
					
					int pointCount = selectionInfoPtr->numberPoints;
               wxPoint* pointlist = new wxPoint[pointCount+1];
               int sideBysideImage_offset =
									s_startChannel * gChannelWindowInterval - gChannelWindowOffset;
					int pointIndex = 0;
					//wxPoint scrollOffset = pImageView->m_Canvas->GetScrollPosition();
					for (pointIndex=0; pointIndex<pointCount; pointIndex++)
						{
						ConvertLCToWinPoint ((LongPoint*)&selectionPointsPtr[pointIndex],
													&windowPoint,
													&lcToWindowUnitsVariables);
						pointlist[pointIndex].x = windowPoint.h + sideBysideImage_offset;
						pointlist[pointIndex].y = windowPoint.v ;
						
						}	// end "for (pointIndex=0; pointIndex<pointCount; pointIndex++)"
					
					pointlist[pointIndex].x = localCPoint.x;
					pointlist[pointIndex].y = localCPoint.y;
					dc.SetBrush (*wxTRANSPARENT_BRUSH);
					dc.SetPen (wxPen (*wxWHITE, 1, wxPENSTYLE_DOT));
					dc.DrawPolygon (pointCount +1 , pointlist, 0, 0, wxODDEVEN_RULE);
					dc.SetPen (wxPen (*wxBLACK, 1, wxPENSTYLE_DOT));
					dc.DrawPolygon (pointCount +1 , pointlist, 1, 1, wxODDEVEN_RULE);
					
               delete pointlist;
					dc.SetPen (pOldPen);
					
					}	// end "if (!selectionClearedFlag)"
            
				}	// end "if (s_selectType == kPolygonType)"

			else	// s_selectType == kRectangleType
				{
						// Outline the new selection
            
				if (localCPoint.x > c_down.x)
					{
					rect.x = c_down.x;
					rect.width = localCPoint.x - c_down.x;
					
					}
					
				else
					{
					rect.x = localCPoint.x;
					rect.width = c_down.x - localCPoint.x;
					
					}
					 
				if (localCPoint.y > c_down.y)
					{
					rect.y = c_down.y;
					rect.height = localCPoint.y - c_down.y;
					
					}
					
				else
					{
					rect.y = localCPoint.y;
					rect.height = c_down.y - localCPoint.y;
					
					}
									
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

				}	// end "else s_selectType == kRectangleType"
         
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
				pImageView->UpdateCursorCoordinates (&longLocalPoint);

            }	// end "if (pImageView->GetDocument ()->s_displayCoordinatesFlag)"

			CMTool::OnMouseMove(pImageView, nFlags, localCPoint);

			}	// end "if (point != c_last)"

		return (TRUE);

		}	// end "if (selectMode == netSelect && ..."

	return (FALSE);

}	// end "CMSelectTool::OnMouseMove"

