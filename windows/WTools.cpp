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
//	File:						WTools.cpp
//
//	Revision date:			12/17/2019
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	This file contains functions that relate to the
//								CMTool and CMSelectTool classes.
//								Part of this code come from the Microsoft Foundation Classes
//								C++ library examples.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include "SDisplay_class.h"
#include "SImageWindow_class.h"

#include "WMultiSpec.h"
#include "WImageDoc.h"
#include "WImageView.h"
#include "WTools.h"



//------------------------------------------------------------------------------------
//		CMTool implementation
//------------------------------------------------------------------------------------
//	Constructor  

CPtrList 			CMTool::c_tools;

static 				CMSelectTool selectTool;  

LCToWindowUnitsVariables		CMSelectTool::s_lcToWindowUnitsVariables;

CPoint 				CMTool::c_down;  
CPoint 				CMTool::c_last;

		// Used by the idle handler.  
CPoint 				CMTool::c_lastMousePoint;
                                
ToolType 			CMTool::c_toolType = selection; 
Boolean 				CMTool::c_nonClientAreaFlag = FALSE;    

SInt16				CMTool::s_selectType = kRectangleType; 



CMTool::CMTool (
				ToolType								toolType)
				
{
	mToolType = toolType;
	c_tools.AddTail (this); 
	
}	// end "CMTool::CMTool"



CMTool* CMTool::FindTool (
				ToolType 							toolType)
				
{
	POSITION pos = c_tools.GetHeadPosition ();
	
	while (pos != NULL)
		{
		CMTool* pTool = (CMTool*)c_tools.GetNext (pos);
		if (pTool->mToolType == toolType) 
			return pTool;
			
		}	// end "while (pos != NULL)"

	return NULL;
	
}	// end "CMTool::FindTool"



void CMTool::OnChar (
				CMImageView* 						pView,
				UINT									nChar)

{

}	// end "CMTool::OnChar"



void CMTool::OnLButtonDblClk (
				CMImageView* 						imageView,
				UInt16 								nFlags)

{

}	// end "CMTool::OnLButtonDblClk"



void CMTool::OnLButtonDown (
				CMImageView* 						imageView,
				UInt16 								nFlags,
				const CPoint& 						point)
				
{                                    
	imageView->SetCapture ();
	//c_nDownFlags = nFlags;
	c_down = point;
	c_last = point; 
	c_lastMousePoint = point;
	c_nonClientAreaFlag = FALSE;
	
}	// end "CMTool::OnLButtonDown"



void CMTool::OnLButtonUp (
				CMImageView* 						imageView,
				UInt16 								nFlags,
				const CPoint& 						point,
				Boolean								newSelectedWindowFlag)
				
{
	if (s_selectType == kRectangleType)
		ReleaseCapture ();

	if (point == c_down)
		c_toolType = selection; 
		
	c_nonClientAreaFlag = FALSE;
		
}	// end "CMTool::OnLButtonUp"



Boolean CMTool::OnMouseMove (
				CMImageView* 						pImageView,
				UInt16			 					nFlags,
				const CPoint& 						point)
				
{
	c_last = point;

	return (TRUE);
	
}	// end "CMTool::OnMouseMove"
                          


//------------------------------------------------------------------------------------
// CMSelectTool        

		// Use for polygon selections so that repeated points are not selected in
		// a row.               

		// Pen to use to draw selection rectangles and polygons. 
		
LOGPEN 				CMSelectTool::s_logpen = { s_logpen.lopnStyle = PS_DOT,
															s_logpen.lopnWidth.x = 1,
															s_logpen.lopnWidth.y = 1,
															s_logpen.lopnColor = RGB (0, 0, 0) }; 

LongPoint			CMSelectTool::s_lastLineColumnPoint; 

		// Start point in windows units that the cursor is allowed to roam around
		// in for the selected startChannel.
		
LongPoint			CMSelectTool::s_startPoint;                               
		
Rect					CMSelectTool::s_limitRectangle;
		
Rect					CMSelectTool::s_viewRect;  

SelectMode 			CMSelectTool::s_selectMode = none; 
		
double				CMSelectTool::s_hConversionFactor = 0; 

double				CMSelectTool::s_vConversionFactor = 0;
		
DisplaySpecsPtr	CMSelectTool::s_displaySpecsPtr = NULL;

HPFieldPointsPtr	CMSelectTool::s_selectionPointsPtr = NULL;

MapProjectionInfoPtr	CMSelectTool::s_mapProjectionInfoPtr = NULL;

SelectionInfoPtr	CMSelectTool::s_selectionInfoPtr = NULL; 

SInt32				CMSelectTool::s_prevLineStart = -1;                            
SInt32				CMSelectTool::s_prevColumnStart = -1;                                  
SInt32				CMSelectTool::s_prevLineEnd = -1;                                         
SInt32				CMSelectTool::s_prevColumnEnd = -1; 

UInt32				CMSelectTool::s_memoryLimitNumber = 0;
UInt32				CMSelectTool::s_bytesNeededIncrement = 0;
UInt32				CMSelectTool::s_bytesNeeded = 0;            
		
UInt16				CMSelectTool::s_startChannel = 0; 

Boolean				CMSelectTool::s_initPolygonFlag = FALSE;

  

CMSelectTool::CMSelectTool ()
		: CMTool (selection)

{

}	// end "CMSelectTool::CMSelectTool"



void CMSelectTool::AddPolygonPoint (
				CMImageView* 						imageViewCPtr,
				LongPoint							newLineColPoint)

{
	UInt32								index;
	

	if (s_selectionInfoPtr->numberPoints == (SInt32)s_memoryLimitNumber)
		{
		CheckAndUnlockHandle (s_selectionInfoPtr->polygonCoordinatesHandle);

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
	
}  // end AddPolygonPoint

 

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
   										
	s_initPolygonFlag	= TRUE;                                                      
	
	return (TRUE);

}	//end InitPolygon



void CMSelectTool::OnChar (
				CMImageView* 						imageViewCPtr,
				UINT									nChar)
				
{      
	if (s_selectType == kRectangleType)
																							return; 
		                        
	if (s_selectMode != none && nChar == 8 && s_selectionInfoPtr->numberPoints > 0)
		{                                                
		LongPoint							drawLPoint;
													
		Handle 								windowInfoHandle;
													
		UInt32								index;
			  
			
				// Delete the last polygon point.
					
		s_selectionInfoPtr->numberPoints--; 
		DeletePolyPointStatNewFieldW (s_selectionInfoPtr->numberPoints); 
			                       
		GetBoundingSelectionRectangles (s_displaySpecsPtr,
													s_selectionInfoPtr, 
													s_selectionPointsPtr,
													s_startChannel); 
													
				// Erase the current 'moving' polygon line.
					
		CRect rect (c_down.x, c_down.y, c_last.x, c_last.y);
		rect.NormalizeRect ();   

		rect.right++;
		rect.bottom++;

		imageViewCPtr->InvalidateRect (rect, FALSE);
		imageViewCPtr->UpdateWindow ();

				// Erase the last fixed segment of the polygon line if
				// it exists. 
			
		if (s_selectionInfoPtr->numberPoints > 0)  
			{ 
			windowInfoHandle = GetWindowInfoHandle (imageViewCPtr);
					
			index = s_selectionInfoPtr->numberPoints - 1;
		
			ConvertLCToWinPoint ((LongPoint*)&s_selectionPointsPtr[index],
										&drawLPoint,
										&s_lcToWindowUnitsVariables);
												
					// Adjust the window units to reflect the channel that the			
					// selection was made within.														

			drawLPoint.h -= gChannelWindowOffset - 
												s_startChannel * gChannelWindowInterval;
				
			CRect rect ((int)drawLPoint.h, (int)drawLPoint.v, c_down.x, c_down.y);
			rect.NormalizeRect ();       
	
			rect.right++;
			rect.bottom++;
			imageViewCPtr->InvalidateRect (rect, FALSE);
		
			c_down.x = (int)drawLPoint.h;
			c_down.y = (int)drawLPoint.v;
				
			}	// end "if (s_selectionInfoPtr->numberPoints > 0)"
	                                                     
		imageViewCPtr->UpdateWindow ();
			
				// Now draw the current 'moving' polygon line.  
			
		if (s_selectionInfoPtr->numberPoints > 0)  
			{                                      
			CClientDC dc (imageViewCPtr);
                   
			CPen pen;
			if (pen.CreatePenIndirect (&s_logpen))                          
         	{
				CPen* pOldPen;
	
				pOldPen = dc.SelectObject (&pen);  
				
				dc.MoveTo (c_down);   
				dc.LineTo (c_last);
					
				}	// end "if (pen.CreatePenIndirect (&s_logpen))" 
				
			}	// end "if (s_selectionInfoPtr->numberPoints > 0)"
		
		}	// end "if (s_selectMode != none && nChar == 8 && ..."
	
}	// end "OnChar"



void CMSelectTool::OnLButtonDblClk (
				CMImageView* 						imageViewCPtr,
				UInt16 								nFlags)

{
	
	if (s_selectType == kPolygonType)
		{
		CRect									rect;
		
		LongPoint							longPoint;

		double								factor;
		
		SInt16								coordinateViewUnits,
												unitsToUseCode;

		Boolean								useStartLineColumnFlag;
		
		
		ReleaseCapture ();
		
		s_selectMode = none;
		s_initPolygonFlag	= FALSE;
		
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
				
			rect.SetRect ((int)longPoint.h, (int)longPoint.v, c_last.x, c_last.y);
			rect.NormalizeRect ();
				
					// Verify that all of the new line will be drawn.
				
			rect.right++;
			rect.bottom++;
				
			imageViewCPtr->InvalidateRect (rect, FALSE);
			imageViewCPtr->UpdateWindow ();
				
			}	// end "if (s_selectionInfoPtr->numberPoints > 1)"
		
				// Force the selected polygon to be draw in all windows if this is a
				// side by side image.
		
		if (gSideBySideChannels > 1)
			{
			CMOutlineArea* selectionAreaCPtr = imageViewCPtr->GetSelectionAreaCPtr ();
			selectionAreaCPtr->Invalidate (imageViewCPtr);
			
			}	// end "if (gSideBySideChannels > 1)"

				// Now get the bounding area in coordinate units.

		Handle mapProjectionHandle = GetFileMapProjectionHandle2 (gActiveImageWindowInfoH);
		MapProjectionInfoPtr mapProjectionInfoPtr =
					(MapProjectionInfoPtr)GetHandlePointer (mapProjectionHandle);

		ComputeSelectionCoordinates (gActiveImageWindowInfoH,
												mapProjectionInfoPtr,
												&s_selectionInfoPtr->lineColumnRectangle,
												&s_selectionInfoPtr->coordinateRectangle);
		
				// Show the coordinates of the rectangle that bounds the polygon.
		
		imageViewCPtr->UpdateSelectionCoordinates ();
		
				// Set this selection for all image windows if requested.
				// If shift key is down, then do not use the start line
				// and column to adjust the selections in the windows.

		useStartLineColumnFlag = TRUE;
		if (GetKeyState (VK_SHIFT) & 0x8000)
			useStartLineColumnFlag = FALSE;
		
		if (GetKeyState (VK_CONTROL) < 0)
			{
			unitsToUseCode = kLineColumnUnits;

			coordinateViewUnits = GetCoordinateViewUnits (gActiveImageWindowInfoH);
			if (GetCoordinateHeight (gActiveImageWindowInfoH) > 0)
				{
				if (coordinateViewUnits >= kKilometersUnitsMenuItem)
					unitsToUseCode = kMapUnits;
				
				else if (coordinateViewUnits == kDecimalLatLongUnitsMenuItem ||
							 coordinateViewUnits == kDMSLatLongUnitsMenuItem)
					unitsToUseCode = kLatLongUnits;
				
				}	// end "if (GetCoordinateHeight (gActiveImageWindowInfoH) > 0)"

			factor = GetCoordinateViewFactor (gActiveImageWindowInfoH);
			
			SetSelectionForAllWindows (GetWindowInfoHandle (imageViewCPtr),
												s_selectionInfoPtr,
												&s_selectionInfoPtr->coordinateRectangle,
												useStartLineColumnFlag,
												unitsToUseCode,
												factor);
			
			}	// end "if (GetKeyState (VK_CONTROL) < 0)"
		
		CheckAndUnlockHandle (imageViewCPtr->GetDisplaySpecsHandle ());
		s_displaySpecsPtr = NULL;
		
		Handle selectionInfoHandle = GetSelectionInfoHandle (gActiveImageWindow);
		CheckAndUnlockHandle (selectionInfoHandle);
		s_selectionInfoPtr = NULL;
		
		s_hConversionFactor = 0;
		s_vConversionFactor = 0;
		s_startChannel = 0;
		
				// Reset the static select type back to rectangular.
		
		s_selectType = kRectangleType;
		
				// Show selection info in graph window if needed.
		
		ShowGraphSelection ();

		}	// end "s_selectType == kPolygonType"

}	// end "OnLButtonDblClk"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
//	Coded By:			Larry L. Biehl			Date: ??/??/1995?
//	Revised By:			Larry L. Biehl			Date: 09/18/2002	

void CMSelectTool::OnLButtonDown (
				CMImageView* 						imageViewCPtr,
				UInt16 								nFlags,
				const CPoint& 						point)
				
{
	CPoint 								localPoint;
	CRect 								rect;
	DoubleRect							coordinateRect;
	LongPoint 							longLocalPoint;
	LongRect								lineColumnRect;
	Rect									displayRect;
	
	Handle 								displaySpecsHandle,
											selectionInfoHandle;
									
									
	localPoint = point;
	
	if (s_selectMode != netSelect)
		{
				// Get the type of selection to be done - rectangle or polygon.
		
		s_selectType = kRectangleType;		
		if (GetProjectWindowFlag (GetWindowInfoHandle (imageViewCPtr)))
			{               
			gProjectSelectionWindow = gActiveImageWindow;
			                              
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
			           
		selectionInfoHandle = GetSelectionInfoHandle (gActiveImageWindow);
	                                                              
		s_selectionInfoPtr = (SelectionInfoPtr)GetHandlePointer (
																			selectionInfoHandle, 
																			kLock);
																			
				// Get Map Information Pointer
				
		Handle fileInfoHandle = GetFileInfoHandle (gActiveImageWindowInfoH);
		Handle mapProjectionHandle = GetFileMapProjectionHandle (fileInfoHandle);
	
		s_mapProjectionInfoPtr = 
						(MapProjectionInfoPtr)GetHandlePointer (
													mapProjectionHandle, kLock);
		
				// Get pointer to display specifications which will be needed later
											                                             
		displaySpecsHandle = imageViewCPtr->GetDisplaySpecsHandle ();
		s_displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
														displaySpecsHandle, kLock);
		
				// Get the magnification factors to use to convert from screen			
				// elements to lines and columns													
				
		s_vConversionFactor = s_displaySpecsPtr->displayedLineInterval/
																s_displaySpecsPtr->magnification;
																	
		s_hConversionFactor = s_displaySpecsPtr->displayedColumnInterval/
																s_displaySpecsPtr->magnification;
		
				//	Initialize some limit variables
				
		GetWindowClipRectangle (imageViewCPtr, kImageArea, &s_viewRect);
	
		longLocalPoint.h = localPoint.x;
		longLocalPoint.v = localPoint.y; 
		s_startChannel = GetSelectionRectangleLimits (TRUE, 
															0, 
															&longLocalPoint, 
															s_displaySpecsPtr,
															&s_viewRect,
															&s_limitRectangle,
															&s_startPoint);
														
				// Set the global variables needed to convert from line-column		
				// units to window units.														
					
		SetChannelWindowVariables (kToImageWindow, imageViewCPtr, kNotCoreGraphics);   
				
		SetLCToWindowUnitVariables (gActiveImageWindowInfoH,
												kWindowsUseOrigin, 
												FALSE,
												&s_lcToWindowUnitsVariables);
			
		s_selectMode = netSelect;
	
		}	// end "if (s_selectMode != netSelect)"        
	
			// Force start point to be within the image.

	localPoint.x = MAX (localPoint.x, s_limitRectangle.left);
	localPoint.y = MAX (localPoint.y, s_limitRectangle.top);
			
	localPoint.x = MIN (localPoint.x, s_limitRectangle.right);
	localPoint.y = MIN (localPoint.y, s_limitRectangle.bottom); 
	
	longLocalPoint.h = localPoint.x;
	longLocalPoint.v = localPoint.y;

	if (s_selectType == kPolygonType)
		{  
		LongPoint			newLineColPoint;   
		CRect					rect;
	
		if (!s_initPolygonFlag)
			InitPolygon ();
			
				// Get the line and column for the input point.
				
		ConvertWinPointToLC (&longLocalPoint, &newLineColPoint);
		
		if (s_lastLineColumnPoint.h != newLineColPoint.h || 
													s_lastLineColumnPoint.v != newLineColPoint.v)
			{ 
			AddPolygonPoint (imageViewCPtr,      
								 newLineColPoint);
	
			AddPolyPointStatNewFieldW (newLineColPoint);
			                                                                       
					// Remove the current 'moving' segment and force draw of the fixed
					// line segment, if there is a 'moving' segment, i.e. this is not
					// the first polygon point.
					
			if (s_selectionInfoPtr->numberPoints > 1)
				{      
						// Get the rectangle that contains the current 'moving' line segment.
							  
				rect.SetRect (c_down.x, c_down.y, c_last.x, c_last.y);
				rect.NormalizeRect (); 
			                                                          
						// Verify that the new line will be drawn. The new window point may
						// be moved to the upper left to be at the upper left of the pixel
						// when the image is zoomed larger than 1.
						// Also verify that bottom and right edges are cleared.
						                                         
				rect.left -= (int)s_displaySpecsPtr->magnification;
				rect.top -= (int)s_displaySpecsPtr->magnification;  
				rect.right++; 
				rect.bottom++; 
				 
				imageViewCPtr->InvalidateRect (rect, FALSE);
				imageViewCPtr->UpdateWindow ();          
				
				}	// end "if (s_selectionInfoPtr->numberPoints > 0)" 
			
					// Update the cursor coordinates if the coordinate view is displayed.
					// The rectangle coordinates which includes the current polygon will not 
					// be displayed until the polygon is closed.
			
			if (imageViewCPtr->GetDocument()->GetDisplayCoordinatesFlag ())
				{ 
				imageViewCPtr->UpdateCursorCoordinates (&longLocalPoint);
						                                
				}	// end "if (pImageView->GetDocument()->s_displayCoordinatesFlag)"
			
					// Now convert the LC point back to window units to force it
					// to be at the upper left corner of the pixel if the zoom factor
					// is greater than 1.                               
				
			ConvertLCToWinPoint (&newLineColPoint,
											&longLocalPoint,
											&s_lcToWindowUnitsVariables);
											
			longLocalPoint.h += s_startChannel * gChannelWindowInterval - gChannelWindowOffset;
	
			localPoint.x = (int)longLocalPoint.h;
			localPoint.y = (int)longLocalPoint.v; 
											
			s_lastLineColumnPoint = newLineColPoint;
											
			}	// end "if (s_lastLineColumnPoint != newLineColPoint)"
			
		else    // s_lastLineColumnPoint = newLineColPoint
			{
					// Do not save this 'down' point.
					
																								return; 
			
			}	// end "else s_lastLineColumnPoint = newLineColPoint"
		
		}	// end "if (s_selectType == kPolygonType)"
		
	else    // s_selectType == kRectangleType
		{                         
		rect.SetRect (localPoint.x, localPoint.y, localPoint.x, localPoint.y);
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
													
			DrawSelectionCoordinates (gActiveImageWindowInfoH,
												&lineColumnRect,
												&coordinateRect,
												1);
			
			imageViewCPtr->UpdateCursorCoordinates (&longLocalPoint);
					                                
			}	// end "if (pImageView->GetDocument()->s_displayCoordinatesFlag)"
		
		}	// end "else s_selectType == kRectangleType" 
														
			// Do base class operations
													
	CMTool::OnLButtonDown (imageViewCPtr, nFlags, localPoint);
	
}	// end "CMSelectTool::OnLButtonDown"



void CMSelectTool::OnLButtonUp (
				CMImageView* 						imageViewCPtr,
				UInt16 								nFlags,
				const CPoint& 						point,
				Boolean								newSelectedWindowFlag)
				
{
	if (imageViewCPtr->GetCapture () == imageViewCPtr)
		{                                  
		if (s_selectMode == netSelect)
			{ 
			CRect 								rect;
			DoubleRect							coordinateRect;
			LongRect								lineColumnRect;

			double								factor;
					
			Rect									displayRect;
									
			Handle 								windowInfoHandle; 
											
			SInt16								coordinateViewUnits,
													unitsToUseCode;

			Boolean								useStartLineColumnFlag;
			                                       
					                                                
        
			if (s_selectType == kRectangleType)
				{
				if (!newSelectedWindowFlag || c_down.x != c_last.x || c_down.y != c_last.y)
					{  
					CClientDC 			dc (imageViewCPtr);
			
							// Remove the current selection.
							   	
					rect.SetRect (c_down.x, c_down.y, c_last.x, c_last.y);
					rect.NormalizeRect (); 
					dc.DrawFocusRect (rect); 
					
					ClearSelectionArea (imageViewCPtr);
					
							// Create the final selection rectangle   
				
					displayRect.left = rect.left; 
					displayRect.top = rect.top;
					displayRect.right = rect.right;
					displayRect.bottom = rect.bottom; 
					
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
							                                         
					windowInfoHandle = GetWindowInfoHandle (imageViewCPtr);               
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
					if (GetKeyState (VK_SHIFT) & 0x8000)
						useStartLineColumnFlag = FALSE;
								
					if (GetKeyState (VK_CONTROL) < 0)
						{
						unitsToUseCode = kLineColumnUnits;
		
						coordinateViewUnits =
											GetCoordinateViewUnits (gActiveImageWindowInfoH);
						if (GetCoordinateHeight (gActiveImageWindowInfoH) > 0)
							{
							if (coordinateViewUnits >= kKilometersUnitsMenuItem)
								unitsToUseCode = kMapUnits;
							
							else if (coordinateViewUnits == kDecimalLatLongUnitsMenuItem ||
										 coordinateViewUnits == kDMSLatLongUnitsMenuItem)
								unitsToUseCode = kLatLongUnits;
							
							}	// end "if (GetCoordinateHeight (gActiveImageWindowInfoH) > 0)"

						factor = GetCoordinateViewFactor (gActiveImageWindowInfoH);
						
						SetSelectionForAllWindows (GetWindowInfoHandle (imageViewCPtr), 
															s_selectionInfoPtr,
															&coordinateRect,
															useStartLineColumnFlag,
															unitsToUseCode,
															factor);
						
						}	// end "if (GetKeyState (VK_CONTROL) < 0)"

						// This is the selection rectangle so we are done.
						// Unlock the display specification handle  
	
					CheckAndUnlockHandle (imageViewCPtr->GetDisplaySpecsHandle ());
					s_displaySpecsPtr = NULL;  
					
					Handle fileInfoHandle = GetFileInfoHandle (gActiveImageWindowInfoH);
					Handle mapProjectionHandle =
														GetFileMapProjectionHandle (fileInfoHandle);
					CheckAndUnlockHandle (mapProjectionHandle);
					s_mapProjectionInfoPtr = NULL;
                       
					Handle selectionInfoHandle = 
												GetSelectionInfoHandle (gActiveImageWindow);
					CheckAndUnlockHandle (selectionInfoHandle);
					s_selectionInfoPtr = NULL; 
					
					s_hConversionFactor = 0;
					s_vConversionFactor = 0;
					s_startChannel = 0;     
								
							// Show selection info in graph window if needed.
					
					ShowGraphSelection (); 
			
					}	// end "if (!newSelectedWindowFlag)" 
						
				imageViewCPtr->UpdateSelectionCoordinates (); 
				                                               
				s_selectMode = none;   
				  
				}	// end "if (s_selectType == kRectangleType)"  
			
			}	// end "if (selectMode == netSelect)"  
			
		}	// end "if (imageViewCPtr->GetCapture () == imageViewCPtr)"

	CMTool::OnLButtonUp (imageViewCPtr, nFlags, point, newSelectedWindowFlag);
	
}	// end "CMSelectTool::CMSelectTool::OnLButtonUp"



Boolean CMSelectTool::OnMouseMove (
				CMImageView* 						pImageView, 
				UInt16			 					nFlags, 
				const CPoint& 						point)
				                  	
{                        	
	CPoint								localCPoint;

	DoubleRect							coordinateRect;
   
	LongRect								lineColumnRect;

	LongPoint 							localPoint;
	
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
			DWORD 					dwStyle;
			
			Handle 					windowInfoHandle = GetWindowInfoHandle (pImageView);
										
			CRect						newRect,
										rect;

			CClientDC dc (pImageView);
			
			CPoint 					scrollOffset;
			
			int						maxHRange,
										maxVRange;
			
			
			selectionClearedFlag = FALSE;
			
					// Get the scroll info. It will be used in determining whether
					// the image needs to be scrolled.

			SCROLLINFO			scrollInfo;

			pImageView->GetScrollInfo (SB_HORZ, &scrollInfo);
			scrollOffset.x = scrollInfo.nPos;
			maxHRange = scrollInfo.nMax - s_viewRect.right;    
			
			pImageView->GetScrollInfo (SB_VERT, &scrollInfo);
			scrollOffset.y = scrollInfo.nPos; 
			maxVRange = scrollInfo.nMax - s_viewRect.bottom;
			
			dwStyle = pImageView->GetStyle ();
			if (!(dwStyle & WS_HSCROLL))
				maxHRange = 0;   
			if (!(dwStyle & WS_VSCROLL))
				maxVRange = 0;    
				
					// Get the current selection rectangle.
						  
			rect.SetRect (c_down.x, c_down.y, c_last.x, c_last.y);
			rect.NormalizeRect ();           
				
			if (dc.PtVisible (point)) 
				c_nonClientAreaFlag = FALSE;
					 
			else    // !dc.PtVisible (point)
				{  		
						// Indicate that the mouse is in the non-client area of the window. 
						// This flag is use by 'OnIdle'. 
					
				c_nonClientAreaFlag = TRUE;

				if (GetTickCount () < ((CMultiSpecApp*)AfxGetApp())->m_nextControlTime)
					{	
					if (localPoint.h > s_viewRect.left - 20 &&
							localPoint.h < s_viewRect.right + 20 && 
							localPoint.v > s_viewRect.top - 20 &&
							localPoint.v < s_viewRect.bottom + 20)
																							return (TRUE);

					}	// end "if (GetTickCount () > ..."

				((CMultiSpecApp*)AfxGetApp())->m_nextControlTime = GetTickCount () + 50;
                        
						// Check if scrolling is possible.
							
				if ((localPoint.h < s_viewRect.left && scrollOffset.x > 0) ||
						(localPoint.h > s_viewRect.right && scrollOffset.x < maxHRange) || 
					  (localPoint.v < s_viewRect.top && scrollOffset.y > 0) ||
						(localPoint.v > s_viewRect.bottom && scrollOffset.y < maxVRange))
					{                         
					SInt32				hOffset,
											vOffset;    
					
							// Clear the current selection.					
				
					if (s_selectType == kPolygonType)
						{                             
								// Verify that bottom and right edges are cleared.
								 
						rect.right++; 
						rect.bottom++;
						
								// Invalidate the rectangle that includes the current 
								// selection line segment. Do not erase the background
								// before drawing over. This reduces flashing.
								 
						pImageView->InvalidateRect (rect, FALSE);
						pImageView->UpdateWindow ();
			
						}	// end "if (s_selectType == kPolygonType)" 
							
					else    // s_selectType == kRectangleType
						{					 
						dc.DrawFocusRect (rect);      
							
						}	// end "else s_selectType == kRectangleType"
							
					selectionClearedFlag = TRUE; 
									 
					hOffset = (SInt32)s_displaySpecsPtr->origin[kHorizontal];
					vOffset = (SInt32)s_displaySpecsPtr->origin[kVertical];
						   
							// Scroll the image if needed  
			
					if (localPoint.v < s_viewRect.top)
						pImageView->ScrollV (SB_LINEUP);
				
					else if (localPoint.v > s_viewRect.bottom)
						pImageView->ScrollV (SB_LINEDOWN);
				
					if (localPoint.h < s_viewRect.left) 
						pImageView->ScrollH (SB_LINELEFT);
				
					else if (localPoint.h > s_viewRect.right)
						pImageView->ScrollH (SB_LINERIGHT); 
							
					hOffset -= (SInt32)s_displaySpecsPtr->origin[kHorizontal];
					vOffset -= (SInt32)s_displaySpecsPtr->origin[kVertical];  
							
					hOffset = (SInt32)(hOffset * s_displaySpecsPtr->magnification);
					vOffset = (SInt32)(vOffset * s_displaySpecsPtr->magnification);
						
					c_down.x += (SInt32)hOffset;
					c_down.y += (SInt32)vOffset;
						
					c_last.x += (SInt32)hOffset;
					c_last.y += (SInt32)vOffset; 
						
					if (hOffset != 0 || vOffset != 0)
						{
						LongPoint 		tempLongPoint;
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
							
					}	// end "if ((localPoint.h < s_viewRect.left && ..." 
						
				}	// end "else !dc.PtVisible (point)"
					
					// Peg the selection to the edge of the image if needed.
						
			localCPoint.y = MIN (localCPoint.y, s_limitRectangle.bottom-1);
			localCPoint.x = MIN (localCPoint.x, s_limitRectangle.right-1); 
						
			localCPoint.y = MAX (localCPoint.y, s_limitRectangle.top);
			localCPoint.x = MAX (localCPoint.x, s_limitRectangle.left); 

			if (s_selectType == kPolygonType)
				{
				if (localCPoint.x != c_last.x || localCPoint.y != c_last.y)
					{        	
							// Erase the old line if needed and draw the new line
	            
	            if (!selectionClearedFlag) 
	            	{                  
								// Verify that bottom and right edges are cleared.
									 
						rect.right++; 
						rect.bottom++; 
							
								// Invalidate the rectangle that includes the current 
								// selection line segment. Do not erase the background
								// before drawing over. This reduces flashing.
									 
						pImageView->InvalidateRect (rect, FALSE);
						pImageView->UpdateWindow ();   
						
						}	// end "if (!selectionClearedFlag)" 
					
					}	// end "if (localCPoint.x != c_last.x || ..."
					
				if (!selectionClearedFlag) 
					{	    
					CPen pen;
					if (pen.CreatePenIndirect (&s_logpen)) 
						{ 
						CPen* pOldPen;
		
						pOldPen = dc.SelectObject (&pen);  
						
						dc.MoveTo (c_down); 
						dc.LineTo (localCPoint);     
		
						dc.SelectObject (pOldPen);
						
						}	// end "if (pen.CreatePenIndirect (&s_logpen))"
					
					}	// end "if (!selectionClearedFlag)"                            
				
				}	// end "if (s_selectType == kPolygonType)" 
				
			else    // s_selectType == kRectangleType
				{		
						// Outline the new selection
	
				newRect.SetRect (c_down.x, c_down.y, localCPoint.x, localCPoint.y); 
				newRect.NormalizeRect ();
					
				if (newRect.top != rect.top ||
						newRect.left != rect.left ||
							newRect.bottom != rect.bottom ||
								newRect.right != rect.right)
					{
					if (!selectionClearedFlag)
						dc.DrawFocusRect (rect);
						
					selectionClearedFlag = TRUE;	
						
					}	// end "if (newRect.top != rect.top || ..." 
						
				if (selectionClearedFlag) 
					dc.DrawFocusRect (newRect);  
				
				displayRect.left = newRect.left;
				displayRect.right = newRect.right;
				displayRect.top = newRect.top; 
				displayRect.bottom =  newRect.bottom;
				ComputeSelectionLineColumns (
											s_displaySpecsPtr,
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
					if (	lineColumnRect.top != s_prevLineStart ||
							lineColumnRect.left != s_prevColumnStart ||
							lineColumnRect.bottom != s_prevLineEnd ||
							lineColumnRect.right != s_prevColumnEnd	) 
						{                                                                     
						s_prevLineStart = lineColumnRect.top;                            
						s_prevColumnStart = lineColumnRect.left;                                  
						s_prevLineEnd = lineColumnRect.bottom;                                         
						s_prevColumnEnd = lineColumnRect.right;
						
						numberPixels = (s_prevLineEnd - s_prevLineStart + 1) * 
														(s_prevColumnEnd - s_prevColumnStart + 1);
							
						DrawSelectionCoordinates (gActiveImageWindowInfoH,
																&lineColumnRect,
																&coordinateRect,
																numberPixels); 
						
						}	// end "if (lineColumnRect.top != ..."    
						
					}	// end "if (s_selectType == kRectangleType)"    
						                               
				LongPoint longLocalPoint;
				longLocalPoint.h = localCPoint.x;
				longLocalPoint.v = localCPoint.y;
				gActiveImageViewCPtr->UpdateCursorCoordinates (&longLocalPoint);
				                                
				}	// end "if (pImageView->GetDocument()->s_displayCoordinatesFlag)"
	
			CMTool::OnMouseMove (pImageView, nFlags, localCPoint);
			
			}	// end "if (point != c_last)"  
		
		return (TRUE);
		
		}	// end "if (selectMode == netSelect && ..."
		
	return (FALSE);
		
}	// end "CMSelectTool::OnMouseMove"
