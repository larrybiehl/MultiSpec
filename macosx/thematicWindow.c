//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
// File:						multiSpec.c
//
//	Authors:					Larry L. Biehl
//
//	Revision number:		3.3
//
//	Revision date:			04/26/2016
//
//	Language:				C
//
//	System:					Macintosh Operating System
//
//	Brief description:	This file contains routines to hand thematic type windows.
//
//	Functions in file:	void 					AdjustLegendListLength
//								void 					ConverWinPointToOffScreenPoint
//								void 					DoThematicMouseDnContent
//								void 					DoThematicWUpdateEvent
//								void 					DrawPopUpBox
//								void 					DrawThematicLegend
//								void					GetThematicPopupBox
//								pascal Boolean 	LegendCClickLoop
//								pascal void 		LegendListDefinition
//								void					ThematicImageWBlink
//					  			void					ThematicLegendWMouseDn
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"

#include	"SMulSpec.h"
#include "SExtGlob.h"
// #include "mwCarbonCompatibility.h"

#undef				USE_GDD_TITLE


extern pascal Boolean  LegendCClickLoop (void);

extern void 			LoadOSXColorTable (
								CMPaletteInfo						paletteHandle, 
								UCharPtr								osxColorTablePtr);

extern void 			UpdateOSXColorSpace (
								CMPaletteInfo						paletteHandle,
								UCharPtr								osxColorTablePtr);



			// Prototypes for routines in this file that are only called by		
			// other routines in this file.
			
void 						ConverWinPointToOffScreenPoint (
								Handle								windowInfoH, 
								Point									windowPoint, 
								Point*								offscreenPoint);

void 						DoThematicMouseDnContent (
								WindowPtr							theWindow,
								Point									mousePt);

void 						DoThematicWUpdateEvent (
								WindowPtr							window, 
								Handle								windowInfoHandle);

void 						DrawPopUpBox ( 
								Rect*									inputPopUpBoxPtr);
								
void 						GetThematicPopupBox (
								Rect*									popupBoxPtr,
								Handle								windowInfoHandle);
								
void 						LegendPopUpMenu (
								WindowPtr							theWindow,
								Rect*									popUpBoxPtr);

void						ThematicImageWBlink (
								Point									mousePt);

void						ThematicLegendWMouseDn (
								Point									mousePt);



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void AdjustLegendListLength
//
//	Software purpose:	This routine adjust the length of the legend
//							list in response to a change in the size of the
//							image window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			ShowHideCoordinateView in menu.c
//							DoWindowZoomInOutEvent in multiSpec.c
//							DoNonGraphicsWindowGrow in multiSpec.c
// 						AdjustImageWSize in SOpnImag.cpp
//							ChangeLegendWidth in window.c
//
//	Coded By:			Larry L. Biehl			Date: 01/29/1990
//	Revised By:			Larry L. Biehl			Date: 09/08/2006			

void AdjustLegendListLength (
				WindowPtr							theWindow, 
				Boolean								invalFlag)

{
			//	Local Declarations
			
	Rect									rectangle; 	
	
	ListHandle							legendListHandle;	
	WindowInfoHandle					windowInfoHandle;	
	
	SInt16								cellHeight,
											listTop,
											newListBottom,
											newListHeight,
											oldListBottom,
											windowType;													
	
	Boolean								legendListMovedFlag;
	
   
   		// Check input parameters.															
   		
   if (theWindow == NULL)
   																						return;
   																						
	windowInfoHandle = (WindowInfoHandle)GetWRefCon(theWindow);
   if (windowInfoHandle == NULL)
   																						return;
   																								
	windowType = ((WindowInfoPtr)*windowInfoHandle)->windowType;

   if (windowType == kThematicWindowType && 
   							((WindowInfoPtr)*windowInfoHandle)->legendWidth > 6)
   	{
   			// Get height of legend list.  Make it an integral multiple of		
   			// the cell size.																	
   	
   	legendListHandle = ((WindowInfoPtr)*windowInfoHandle)->legendListHandle;
   	if (legendListHandle == NULL)
   																						return;
   																						
		GetWindowClipRectangle (theWindow, kImageScrollArea, &rectangle);
   	cellHeight = ((ListPtr)*legendListHandle)->cellSize.v;
   	newListHeight = ((rectangle.bottom - rectangle.top -
   								15 - 2 - kSBarWidth)/cellHeight) * cellHeight;
   										
   			// Save the current bottom of the list area.								
   			
   	oldListBottom = ((ListPtr)*legendListHandle)->rView.bottom;
   	
   			// Change the position of the list top if needed.
   	
   	listTop = 16 + rectangle.top;
   	legendListMovedFlag = FALSE;
   	if (listTop != ((ListPtr)*legendListHandle)->rView.top)
   		{
   		((ListPtr)*legendListHandle)->rView.top = listTop;
   		legendListMovedFlag = TRUE;
   		
   		}		// end "if (listTop != ((ListPtr)*legendListHandle)->rView.top)"
   	
   			// Resize the list if needed.													
   	
   	if (newListHeight != oldListBottom-15 || legendListMovedFlag)
   		LSize (gActiveLegendWidth-2-4, newListHeight, legendListHandle);
   	
   			// Get the new list bottom.													
   			
   	newListBottom = ((ListPtr)*legendListHandle)->rView.bottom;
   			
   			// Invalidate a strip along the left edge of the image if the		
   			// new window is taller than the previous window.  For some			
   			// reason 'LSize' validates a strip about 15 pixels wide to the	
   			// right of the edge of the list area.										
   			// Also invalidate the new area that was added to the list at the	
   			// bottom.																			
   			
   	if (invalFlag)
   		{
  			rectangle.top = rectangle.top + 15 - 1;
  			rectangle.bottom = newListBottom + 1;
  			rectangle.left   = gActiveLegendWidth;
  			rectangle.right  = gActiveLegendWidth + 15;
  			InvalWindowRect (theWindow, &rectangle);
  			
  			}		// end "if (invalFlag)" 
  			
  		if (newListBottom > oldListBottom)
  			{
  			rectangle.top = oldListBottom;
  			rectangle.bottom = newListBottom;
  			rectangle.left   = 0;
  			rectangle.right  = gActiveLegendWidth;
  			InvalWindowRect (theWindow, &rectangle);
  			
  			}		// end "if (newListBottom != oldListBottom)" 
   			
   			// Erase a strip along the bottom that is the height of one			
   			//	cell to be sure that a half a cell is not left on the screen.	
   	
   	else if (newListBottom < oldListBottom)
   		{
			GetWindowPortBounds (theWindow, &gTempRect);
  			rectangle.top = newListBottom;
  			rectangle.bottom = gTempRect.bottom - kSBarWidth;
  			rectangle.left   = 0;
  			rectangle.right  = gActiveLegendWidth;
  			EraseRect (&rectangle);
  			
  			}		// end "else if (newListBottom < oldListBottom)" 
   	
   	}		// end "if (windowType == kThematicWindowType && ..." 

}		// end "AdjustLegendListLength" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ConverWinPointToOffScreenPoint
//
//	Software purpose:	This routine converts the input window point
//							coordinates to the corresponding image offscreen
//							point coordinates.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			ThematicImageWBlink in multiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 01/31/1991
//	Revised By:			Larry L. Biehl			Date: 05/01/1991			

void ConverWinPointToOffScreenPoint (
				Handle								windowInfoH, 
				Point									windowPoint, 
				Point*								offscreenPoint)

{	
	double								magnification;

	DisplaySpecsPtr					displaySpecsPtr;
	
	Handle								displaySpecsH;
	
	
			// Initialize output offscreen point.											
			
	*offscreenPoint = windowPoint;
	
	if (windowInfoH != NULL)
		{
				// Get pointer to display specification.  We do not need to lock 	
				//	it here since no other routines are called							
		
		displaySpecsH = ((WindowInfoPtr)*windowInfoH)->displaySpecsHandle;
		if (displaySpecsH != NULL)
			{
			displaySpecsPtr = (DisplaySpecsPtr)*displaySpecsH;
			magnification = displaySpecsPtr->magnification;
	
			offscreenPoint->v -= ((WindowInfoPtr)*windowInfoH)->imageTopOffset;
			offscreenPoint->h -= ((WindowInfoPtr)*windowInfoH)->legendWidth;
							
			#ifdef powerc
				offscreenPoint->v = offscreenPoint->v/magnification;
				offscreenPoint->h = offscreenPoint->h/magnification;
			#else		// !powerc 
				offscreenPoint->v /= magnification;
				offscreenPoint->h /= magnification;
			#endif
			
			
			offscreenPoint->v += displaySpecsPtr->origin[kVertical];
			offscreenPoint->h += displaySpecsPtr->origin[kHorizontal];
		
			}		// end "if (displaySpecsH != NULL)" 
			
		}		// end "if (windowInfoH != NULL)" 

}		// end "ConverWinPointToOffScreenPoint" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		DoThematicMouseDnContent
//
//	Software purpose:	This routine handle mouse down events in the
//							content part of a window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			main
//
//	Coded By:			Larry L. Biehl			Date: 09/09/1988
//	Revised By:			Larry L. Biehl			Date: 07/29/1996			

void DoThematicMouseDnContent (
				WindowPtr							theWindow,
				Point									mousePt)

{
			//	Local Declarations 																
	
	Rect									popupBox;
	ControlHandle						whichControl;
	
	SInt16								partCode;

		
	partCode = FindControl (mousePt, theWindow, &whichControl);
	if (partCode > 0)
     	{
     			// Set clip region to include the scroll bars.							
     			
		ClipRect (GetWindowPortBounds(theWindow, &gTempRect));
			
				// Exit if this was a mouse down in a control area of an		
				// inactive image window.  We will assume that the user was	
				// just wanting to select the window.								
				
		if (gNewSelectedWindowFlag)
																					return;
																					
		ImageWControlEvent (whichControl, theWindow, mousePt, partCode);
   		
		}		// end "if (partCode > 0)" 
		 
	else		// partCode == 0.  Mouse not within controls 
		{
				// Handle mousedown events in the active image window.				
										
		if (theWindow == gActiveImageWindow)
			{
			switch (gPresentCursor)
				{
				case kCross:
					SelectArea (mousePt);
					break;
					
				case kHand:
					MSetCursor (kHandClosed);
		  			HandScrollImage (theWindow, mousePt);
					MSetCursor (kHand);
					break;
					
				case kLegendWidth:
					MSetCursor (kLegendWidthDown);
		  			ChangeLegendWidth (theWindow, mousePt);
					MSetCursor (kLegendWidth);
					break;
					
				case kBlinkOpenCursor1:
		  			ThematicLegendWMouseDn (mousePt);
					break;
					
				case kBlinkOpenCursor2:
		  			ThematicImageWBlink (mousePt);
					break;
					
				case kArrow:
							// Get Box for popup and check if cursor is within it.
														
					GetThematicPopupBox (&popupBox, gActiveWindowInfoH);
					
					if (PtInRect (mousePt, &popupBox))
						LegendPopUpMenu (theWindow, &popupBox);
					
					else		// !PtInRect (mousePt, &popupBox)
		  				ThematicLegendWMouseDn (mousePt);
					break;
					
				}		// end "switch (gPresentCursor)" 			
		  		
		  	}		// end "if (theWindow == gActiveImageWindow)"
		  		
		}		// end "else partCode == 0" 

}		// end "DoThematicMouseDnContent" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				void DoThematicWUpdateEvent
//
//	Software purpose:			This routine handles the update events for the 
//									Thematic type image windows.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			DoUpdateEvent in multiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 12/27/1989
//	Revised By:			Larry L. Biehl			Date: 11/27/2000			

void DoThematicWUpdateEvent (
				WindowPtr							window, 
				Handle								windowInfoHandle)

{
	Pattern								black;
	
	Rect									coordinateRect,
											rect;
	
	WindowInfoPtr						windowInfoPtr;
	
	RgnHandle							coordinateUpdateRegion,
											rectangleRegion,
											updateRegionH;
											
	Boolean								callCoordinateUpdateFlag;
	
	
	windowInfoPtr = (WindowInfoPtr)*windowInfoHandle;
		
			// Get rectangle enclosing area to be updated and convert to local	
			// coordinates.																		
					
	updateRegionH = NewRgn ();
	
	#if TARGET_API_MAC_CARBON
		GetWindowRegion (window, kWindowUpdateRgn, updateRegionH);
	#else		// !TARGET_API_MAC_CARBON
	
		#if ACCESSOR_CALLS_ARE_FUNCTIONS
//			OSStatus returnCode = GetWindowRegion (window, kWindowUpdateRgn, updateRegionH);
			GetWindowUpdateRgn (window, updateRegionH);
		#else		// !ACCESSOR_CALLS_ARE_FUNCTIONS
			GetWindowUpdateRgn (window, updateRegionH);
		#endif	// ACCESSOR_CALLS_ARE_FUNCTIONS, else...
		
	#endif	// !ARGET_API_MAC_CARBON
	
			// Determine if 'CopyOffScreenImage' needs to be called.					
	
	rectangleRegion = gTempRegion1;
	coordinateUpdateRegion = gTempRegion2;
	callCoordinateUpdateFlag = FALSE;
	
	GetRegionBounds(updateRegionH, &rect);
		
	GlobalToLocal (&topLeft(rect));
	GlobalToLocal (&botRight(rect));
	
	if (rectangleRegion != NULL && coordinateUpdateRegion != NULL)
		{
				// Determine if coordinate view needs to be updated
				
		if (windowInfoPtr->coordinateHeight > 0)
			{
			SetEmptyRgn (coordinateUpdateRegion);
			SetEmptyRgn (rectangleRegion);
			coordinateUpdateRegion = gTempRegion2;
			
			GetWindowClipRectangle (window, kCoordinateArea, &coordinateRect);
		
			LocalToGlobal (&topLeft(coordinateRect));
			LocalToGlobal (&botRight(coordinateRect));
		
			RectRgn (rectangleRegion, &coordinateRect);
			SectRgn (updateRegionH, rectangleRegion, coordinateUpdateRegion);
		
			callCoordinateUpdateFlag = !EmptyRgn (coordinateUpdateRegion);
			
			}		// end "if (windowInfoPtr->coordinateHeight > 0)"
		
		}		// end "if (rectangleRegion != NULL && ..." 
	
	DisposeRgn (updateRegionH);
	
	if (coordinateUpdateRegion != NULL)
		SetEmptyRgn (coordinateUpdateRegion);
	
	if (rectangleRegion != NULL)
		SetEmptyRgn (rectangleRegion);
	
			// Do the update.																		
				
	BeginUpdate (window);
			
			// Update the image as needed.													
			
	CopyOffScreenImage (window, NIL, &rect, kDestinationCopy, kFromImageUpdate);
	
	GetWindowPortBounds(window, &gTempRect);
		
			// Update the legend as needed.													
			
	if (windowInfoPtr->showLegend)
		{
		SetRect (&rect, 
						0, 
						windowInfoPtr->imageTopOffset, 
						windowInfoPtr->legendWidth, 
						gTempRect.bottom - kSBarWidth);
		EraseRect (&rect);
		ClipRect (&rect);
		
		DrawThematicLegend (window, 
									NULL,
									windowInfoPtr, 
									windowInfoPtr->legendListHandle,
									windowInfoPtr->legendLeftStart,
									windowInfoPtr->legendTopStart,
									TRUE);
			
		}		// end "if (windowInfoPtr->showLegend)" 
		
			// Update the coordinate area.
			
	if (callCoordinateUpdateFlag)
		{
		ClipRect (&gTempRect);
	
		DoCoordinateViewUpdateEvent (window, windowInfoHandle);
		
		DrawCursorCoordinates (windowInfoHandle);
		
		ShowSelection (windowInfoHandle);
		
		DrawScaleInformation (windowInfoHandle);
		
		DrawOneControl ( 
				((WindowInfoPtr)*windowInfoHandle)->coordinateUnitsControl);
		DrawOneControl ( 
				((WindowInfoPtr)*windowInfoHandle)->coordinateViewAreaUnitsControl);
				
		}		// end "if (callCoordinateUpdateFlag)"

			// Draw the grow icon and the controls.										

	GetWindowClipRectangle (window, kFrameArea, &rect);
	ClipRect (&rect);
	DrawControls (window);
	
	GetWindowClipRectangle (window, kImageScrollArea, &rect);
	ClipRect (&rect);
	DrawGrowIcon (window);
		
			// Draw the 'legend width changer rectangle'.								
	
	rect.top = rect.bottom = gTempRect.bottom;
	rect.top -= kSBarWidth;
	rect.left = rect.right = gTempRect.left + windowInfoPtr->legendWidth;
	rect.right += kLegendWidthChanger;
	FillRect (&rect, GetQDGlobalsBlack(&black));
		
	EndUpdate (window);
	    				
			// Set clip region of window at scroll bar and legend edges. 			
	
	GetWindowClipRectangle (window, kImageFrameArea, &rect);
	rect.top = 0;
	rect.left = 0;
	ClipRect (&rect);
	
}		// end "DoThematicWUpdateEvent" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void DragScrollList
//
//	Software purpose:	The purpose of this routine is to monitor a dragging
//							in the list and detect when the list needs to be scrolled.
//		
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None.
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/21/1991
//	Revised By:			Larry L. Biehl			Date: 11/06/1996

pascal void DragScrollList (void)

{
	ListHandle										legendListHandle;
	PenState											penState;
	Point												presentPt;
	
	
	GetMouse (&presentPt);
	
	legendListHandle = ((WindowInfoPtr)*gActiveImageWindowInfoH)->legendListHandle;
	
	if (presentPt.v < gViewRect.top)
		{
		if ((*legendListHandle)->visible.top > 
												(*legendListHandle)->dataBounds.top)
			{
			GetPenState (&penState);
			LScroll (0, -1, legendListHandle);
			gVerticalCellOffset--;
			SetPenState (&penState);
			
			}		// end "if ((*legendListHandle)->visible.top > ..." 
		
		}		// end "if (presentPt.v < gViewRect.top)" 
		
	else if (presentPt.v > gViewRect.bottom)
		{
		if ((*legendListHandle)->visible.bottom < 
												(*legendListHandle)->dataBounds.bottom)
		
			{
			GetPenState (&penState);
			LScroll (0, 1, legendListHandle);
			gVerticalCellOffset++;
			SetPenState (&penState);
			
			}		// end "if ((*legendListHandle)->visible.bottom < ..." 
		
		}		// end "else if (presentPt.v > gViewRect.bottom)" 
		
}		// end "DragScrollList" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DrawPopUpBox
//
//	Software purpose:	The purpose of this routine is to draw a popup box at
//							the input location for the current window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			DrawThematicLegend in thematicWindow.c
//
//	Coded By:			Larry L. Biehl			Date: 05/24/1996
//	Revised By:			Larry L. Biehl			Date: 05/24/1996	

void DrawPopUpBox ( 
				Rect*								inputPopUpBoxPtr)

{
	Rect									popUpBox;
	
	
			// Get copy of popup box location.
	
	popUpBox = *inputPopUpBoxPtr;
		
	popUpBox.right--;
	popUpBox.bottom--;
	FrameRect (&popUpBox);
	MoveTo (popUpBox.right, popUpBox.top+2);
	LineTo (popUpBox.right, popUpBox.bottom);
	LineTo (popUpBox.left+2, popUpBox.bottom);
	
	PenNormal();
	if (gTempRegion1 != NULL)
		{
		OpenRgn();
		MoveTo (popUpBox.right - 3, popUpBox.top + 5);
		LineTo (popUpBox.right - 13, popUpBox.top + 5);
		LineTo (popUpBox.right - 8, popUpBox.top + 11);
		LineTo (popUpBox.right - 3, popUpBox.top + 5);
		CloseRgn (gTempRegion1);
		InvertRgn (gTempRegion1);
		SetEmptyRgn (gTempRegion1);
		
		}		// end "if (gTempRegion1 != NULL)" 
			
}		// end "DrawPopUpBox"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DrawThematicLegend
//
//	Software purpose:	The purpose of this routine is to draw the thematic
//							image window legend.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			DoEdit in menus.c
//							DoThematicWUpdateEvent in multiSpec.c
//							PrintImageWindow in print.c
//
//	Coded By:			Larry L. Biehl			Date: 02/18/1991
//	Revised By:			Larry L. Biehl			Date: 02/12/2016	

void DrawThematicLegend (
				WindowPtr							window, 
				CGrafPtr								cGraphPtr,
				WindowInfoPtr						windowInfoPtr, 
				ListHandle							legendListHandle,
				SInt16								legendLeftStart,
				SInt16								legendTopStart,
				Boolean								drawPopUpBoxFlag)

{	
	Rect									legendTitlePopUpBox;
	
	CGrafPtr								localCGraphPtr;
	DisplaySpecsPtr					displaySpecsPtr;
	
	unsigned char*						legendTitlePtr;
	
	const unsigned char*				legendTitle0Ptr = "\pLegend ";
	#ifdef USE_GDD_TITLE
		const unsigned char*				legendTitle1Ptr = "\pGDDs";
	#endif
	#ifndef USE_GDD_TITLE
		const unsigned char*				legendTitle1Ptr = "\pClasses";
	#endif
	const unsigned char*				legendTitle2Ptr = "\pGroups ";
	const unsigned char*				legendTitle3Ptr = "\pGroups/Classes";
	
	UInt32								numberLevels;
	
	SInt16								classGroupCode;
	
		
			// If using display for GDDs, do not draw the popup box outline.
			
	#ifdef USE_GDD_TITLE
		drawPopUpBoxFlag = FALSE;
	#endif
	#ifndef USE_GDD_TITLE
		drawPopUpBoxFlag = TRUE;
	#endif
	
	classGroupCode = 0;
	gPaletteOffset = 0;
	gClassPaletteEntries = 0;
	
	if (windowInfoPtr->displaySpecsHandle != NULL && 
										windowInfoPtr->offScreenMapHandle != NULL)
		{
		displaySpecsPtr = (DisplaySpecsPtr)*windowInfoPtr->displaySpecsHandle;
		classGroupCode = displaySpecsPtr->classGroupCode;
		numberLevels = displaySpecsPtr->numberLevels;
		gPaletteOffset = displaySpecsPtr->paletteOffset;
		gClassPaletteEntries = displaySpecsPtr->numPaletteEntriesUsed;
		
		}		// end "if (windowInfoPtr->displaySpecsHandle != NULL && ...)" 
	
	if (cGraphPtr == NULL)
		localCGraphPtr = GetWindowPort(window);
		
	else		// cGraphPtr != NULL
		localCGraphPtr = cGraphPtr;
		
//	LUpdate (GetPortVisibleRegion (
//								GetWindowPort (window), gTempRegion1), legendListHandle);
	LUpdate (GetPortVisibleRegion (localCGraphPtr, gTempRegion1), legendListHandle);
	SetEmptyRgn (gTempRegion1);
	
			// Draw the legend title, 'Classes' or 'Groups ".
		
	MoveTo (legendLeftStart + 4, 
				legendTopStart + 11);
	
	legendTitlePtr = (unsigned char*)legendTitle0Ptr;
	
	if (classGroupCode == 1)
		legendTitlePtr = (unsigned char*)legendTitle1Ptr;
		
	else if (classGroupCode == 2)
		legendTitlePtr = (unsigned char*)legendTitle2Ptr;
		
	else if (classGroupCode == 3)
		legendTitlePtr = (unsigned char*)legendTitle3Ptr;
	
	TextSize (12);
	DrawString (legendTitlePtr);
			
	if (drawPopUpBoxFlag && 
				classGroupCode != 0 && 
							numberLevels <= gClassListLimit)
		{
				// Get the popup box location and draw it.
		
		GetThematicPopupBox (
							&legendTitlePopUpBox, windowInfoPtr->windowInfoHandle);
			
		DrawPopUpBox (&legendTitlePopUpBox);
		
		}		// end "if (drawPopUpBoxFlag && ...)"
			
}		// end "DrawThematicLegend"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetThematicPopupBox
//
//	Software purpose:	The purpose of this routine is to get the rectangle
//							that encloses the thematic legend popup box.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 07/29/1996
//	Revised By:			Larry L. Biehl			Date: 10/17/1996	

void GetThematicPopupBox (
				Rect*									popupBoxPtr,
				Handle								windowInfoHandle)

{
			// Get the legend popup box location.
			
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)*windowInfoHandle;
			
	popupBoxPtr->top = windowInfoPtr->legendTopStart;
	popupBoxPtr->bottom = windowInfoPtr->legendTopStart + 15;
	popupBoxPtr->left = windowInfoPtr->legendLeftStart;
	popupBoxPtr->right = windowInfoPtr->legendLeftStart + 
													windowInfoPtr->legendTitleWidth + 25;
			
}		// end "GetThematicPopupBox" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal Boolean LegendCClickLoop
//
//	Software purpose:	The purpose of this routine is to handle the
//							mouse down movement within the legend list that is
//							displayed within the Thematic image windows.
//		
//	Parameters in:				
//
//	Parameters out:			
//
// Value Returned:  True.		
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 01/28/1991
//	Revised By:			Larry L. Biehl			Date: 11/07/1996

pascal Boolean LegendCClickLoop (void)

{
	Point									lCell;
	
	ListHandle							legendListHandle;
	
	SInt16								listType;
	
	
			// Exit routine if this is the first time through for this mouse		
			// down.  Allow this cell to be selected.										
		
	if (gFirstTimeClikLoop)
		{
		gFirstTimeClikLoop = FALSE;
		
		#if !TARGET_API_MAC_CARBON	
																				return (TRUE);
		#endif	// !TARGET_API_MAC_CARBON
		
		}		// end "if (gFirstTimeClikLoop)"
		
			// Get handle to the legend list and the cell of the last click.											
			
	legendListHandle = ((WindowInfoPtr)*gActiveImageWindowInfoH)->legendListHandle;
	lCell = LLastClick (legendListHandle);
		
			// Get class list type.
			
	listType = (((ListPtr)*legendListHandle)->refCon & 0x00000006)/2;
																				
	if (gPresentCursor == kBlinkOpenCursor1)
		{
		return (DoBlinkCursor1 (legendListHandle, lCell, listType, 0));
					
		}		// end "if (gPresentCursor == kBlinkOpenCursor1)" 
		
	else if (gPresentCursor == kArrow)
		{
		Point					startPt,
								presentPt;
		
		Rect					cellRect;
								
		SInt32				finalPointOffset;
		
		Boolean				spectralClassFlag;
		
		SInt16				cellHeight,
								scrolledOffset; 
								
								
		if (listType != kGroupClassDisplay)
																						return (TRUE);
		
				// Initialize variables.															
				
		gVerticalCellOffset = 0;
		finalPointOffset = 0;
									
				// Get mouse location and convert it from global to local				
				// coordinates.																		
				
		GetMouse (&startPt);
																					
				// Get the rectangle that encloses the cell and adjust to the size
				// needed to just outline the class name without the palette chip.									
		
		LRect (&cellRect, lCell, legendListHandle);
		cellRect.left += 17;
			
				// Get the height of 1 cell.														
				
		cellHeight = cellRect.bottom - cellRect.top;
		
				// Get the limit rectangle for the region to be dragged within.		
				 
		gViewRect = ((ListPtr)*legendListHandle)->rView;
		gTempRect = gViewRect;
		
				// Find if this cell is a spectral class.										
				
		SInt16 		cellValue,
						stringLength = 2;
		LGetCell (&cellValue, &stringLength, lCell, legendListHandle);
		spectralClassFlag = !(cellValue & 0x8000);
		
		if (spectralClassFlag && lCell.h >= 0 && lCell.v >= 0 && lCell.v <	
										((ListPtr)*legendListHandle)->dataBounds.bottom &&
										startPt.h > cellRect.left)
			{
					// Convert the rectangle to a region.										
				
			RectRgn (gTempRegion1, &cellRect);
				
			finalPointOffset = DragGrayRgn (gTempRegion1, 
														startPt, 
														&gViewRect, 
														&gTempRect, 
														vAxisOnly, 
														gDragScrollListPtr);
			
			scrolledOffset = gVerticalCellOffset;
			gVerticalCellOffset = HiWord (finalPointOffset);

			if (gVerticalCellOffset != -32768)
//			if (gVerticalCellOffset != 0x8000)
				{
				if (gVerticalCellOffset < 0)
					gVerticalCellOffset -= cellRect.top - startPt.v + cellHeight;
					
				else		// gVerticalCellOffset >= 0  
					gVerticalCellOffset -= cellRect.bottom - startPt.v - cellHeight;
					
						// Now get the offset in cell units.									
						
				gVerticalCellOffset =  gVerticalCellOffset/cellHeight;
				
						// Take into account any scrolling.										
						
				gVerticalCellOffset += scrolledOffset;
				
				if (lCell.v + gVerticalCellOffset >=	
										((ListPtr)*legendListHandle)->dataBounds.bottom)
					gVerticalCellOffset = 0;
				
				}		// end "if (gVerticalCellOffset != 0x8000)" 
				
			else		// gVerticalCellOffset == 0x8000 
				gVerticalCellOffset = 0;
			
			SetEmptyRgn (gTempRegion1);
				
			}		// end "if (spectralClassFlag && ...)" 
		
		if (!spectralClassFlag || startPt.h <= cellRect.left)
			{
					// Wait until button goes up.													
		
			do
				{	
				GetMouse (&presentPt);
				
				if (presentPt.v < gViewRect.top)
					LScroll (0, -1, legendListHandle);
					
				else if (presentPt.v > gViewRect.bottom)
					LScroll (0, 1, legendListHandle); 
				
				} while (StillDown());
				
			}		// end "if (!spectralClassFlag || ...)"
		
																						return (FALSE);
		
		}		// end "else if (gPresentCursor == kArrow)"
		
	return (TRUE);
	
}		// end "LegendCClickLoop"  

	

//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void LegendListDefinition
//
//	Software purpose:	This routine is the list definition for legend lists
//							for machines with color monitors.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/29/1989
//	Revised By:			Larry L. Biehl			Date: 04/26/2016			

pascal void LegendListDefinition (
				SInt16								lMessage, 
				Boolean								lSelect, 
				Rect*									lRectPtr, 
				Cell									lCell, 
				SInt16								lDataOffset, 
				SInt16								lDataLen, 
				ListHandle							listHandle)	

{
	PenState								penState;
	Rect									patternRect;
	RGBColor								newForeGroundColor,
											savedForeGroundColor;
											
	CMPaletteInfo						paletteObject;
	
	DataHandle							cellData;
	
	SInt32								length,
											listType,
											paletteIndex;
		
		
			// Switch to oppropriate section.												
		
	switch (lMessage)
		{
		case lInitMsg:
			break;
			
		case lDrawMsg:
		
			if (gClassPaletteEntries <= 0)
				break;
				
			char*			namePtr;
			Handle 		imageFileInfoH,
							nameHandle;
			SInt16		classGroupCode,
							index;
		
					// Save the current pen state.											
				
			GetPenState (&penState);
		
					// Draw the information into the cell.									
					// First draw the class/group color/pattern.							
					
			patternRect.top = lRectPtr->top + 2;
			patternRect.left = lRectPtr->left;
			patternRect.bottom = lRectPtr->bottom - 2;
			patternRect.right = lRectPtr->left + 15;
			
			PenMode (patCopy);
			
					// Get palette index.  Determine whether class or group list.	
					
			imageFileInfoH = (*listHandle)->userHandle;
			listType = (((ListPtr)*listHandle)->refCon & 0x00000006)/2;
			
					// Now get the class-group tag (sign bit of 16 bit number)
					//  if == 0 then class index, if == 1 then group index
					// and the class/group index.
					
			cellData = ((ListPtr)*listHandle)->cells;
			BlockMoveData (&((*cellData)[lDataOffset]), &index, 2);
			classGroupCode = (index & 0x8000);
			index &= 0x7fff;
			
					// Now get the correct handle to the list names to be used.
					
			FileInfoPtr fileInfoPtr = (FileInfoPtr)*imageFileInfoH;
			
			if (classGroupCode == 0)
				nameHandle = fileInfoPtr->classDescriptionH;
				
			else		// classGroupCode != 0
				nameHandle = fileInfoPtr->groupNameHandle;
				
					// Draw the palette chip only if this cell is not a class name
					// for the group-class display
			
			if (listType != kGroupClassDisplay || classGroupCode != 0)
				{
				//((ListPtr)*listHandle)->refCon & 0x00000001;
				
						// Get the index to the correct palette entry.
						// Use the classGroup to palette vector if this is a group name.
						
				if (classGroupCode == 0)
							// Class name
					paletteIndex = lCell.v;
				
				else		// classGroupCode != 0
					{
							// Group name
					
					SInt16* groupToPalettePtr = (SInt16*)*fileInfoPtr->groupTablesHandle;				
					paletteIndex = groupToPalettePtr[index];
					
					}		// end "else classGroupCode != 0"
				
				GetForeColor (&savedForeGroundColor);
					
				if (paletteIndex >= gClassPaletteEntries)
					{
					UInt16 paletteEntries = MIN(gClassPaletteEntries, 256);
					paletteIndex -= paletteEntries;
					paletteIndex = paletteIndex % (paletteEntries-1) + 1;
					
					}		// end "if (paletteIndex >= gClassPaletteEntries)"
					
//				PmForeColor (paletteIndex);
				
				paletteObject = GetActivePaletteHandle ();
				::GetEntryColor (paletteObject, paletteIndex, &newForeGroundColor);
				
				RGBForeColor (&newForeGroundColor);
				
				PaintRoundRect (&patternRect, 6, 4);
				
				RGBForeColor (&savedForeGroundColor);
					
				}		// end "if (listType != kGroupClassDisplay || ..."
			
					// Now draw the text.														
			
			TextFont (gWindowTextFont); 	// monaco  
			TextSize (12);						// 12 point 							
			
			MoveTo (patternRect.right+4, patternRect.bottom-2);
			
			if (nameHandle != NULL)
				{
				HLock (nameHandle);			
				namePtr = (char*)*nameHandle;
				namePtr = &namePtr[index*32];
				
				if (listType == kGroupClassDisplay && classGroupCode == 0)
					{
					CharPtr classNamePtr = (CharPtr)*fileInfoPtr->classDescriptionH;
					UInt16* classSymbolPtr = (UInt16*)&classNamePtr[
												fileInfoPtr->numberClasses*sizeof(Str31)];
		
					SInt16		stringLength,
									classNumberLength = 3;
					
					if (fileInfoPtr->maxClassNumberValue > 999)
						classNumberLength = 4;
					
							// Add class number of class name in group-class display
							
					sprintf ((char*)&gTextString[1], 
								"%*d-", 
								classNumberLength,
								classSymbolPtr[index]);
					pstr ((char*)&gTextString[classNumberLength+2],
							namePtr, 
							&stringLength);
							
					namePtr = (char*)gTextString;
					namePtr[0] = stringLength + classNumberLength + 1;
								
					}		// end "if (listType == kGroupClassDisplay && ..."
			
				length = MIN (namePtr[0], 31);
				DrawText (namePtr, 1, length);
			
				HUnlock (nameHandle);
					
				}		// end "if (nameHandle != NULL)"
			
					// Reset the pen state back to the original settings.				
		
			SetPenState (&penState);
			break;
			
//       case lHiliteMsg:
//			listType = (((ListPtr)*listHandle)->refCon & 0x00000006)/2;
//			if (listType == kGroupClassDisplay)
//				InvertRect (lRectPtr);
//			break;
			
//      case lCloseMsg:
//			break;
		
		}		// end "switch (lMessage)" 

}		// end "LegendListDefinition"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void LegendPopUpMenu
//
//	Software purpose:	The purpose of this routine is to handle the legend
//							popup menu action when a mouse down occurs in the 
//							legend popup rectangle.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/17/1996
//	Revised By:			Larry L. Biehl			Date: 06/08/2011

void LegendPopUpMenu (
				WindowPtr							theWindow,
				Rect*									popUpBoxPtr)

{
	Handle								displaySpecsH,
											offScreenMapHandle;
									
	Point									popUpLoc;
	
	SInt16								classGroupCode,
											menuData;

	
			// Exit if no image is being shown.												
			
	offScreenMapHandle = 
				((WindowInfoPtr)*gActiveImageWindowInfoH)->offScreenMapHandle;
	if (offScreenMapHandle == NULL)
																					return;
																					
	displaySpecsH = GetDisplaySpecsHandle (gActiveImageWindowInfoH);
	UInt32 numberLevels = ((DisplaySpecsPtr)*displaySpecsH)->numberLevels;
																					
	if (numberLevels > gClassListLimit)
																					return;
	
			// Get location of pop up box and convert it to global.					
	
	popUpLoc.v = popUpBoxPtr->top + 1;
	popUpLoc.h = popUpBoxPtr->left;
	LocalToGlobal (&popUpLoc);
	
			// Insert the pop up menu and then let user make a selection.			
			
	InsertMenu (gPopUpClassGroupMenu2, -1);
	
			// Put check by current selection.												
			
	classGroupCode = ((DisplaySpecsPtr)*displaySpecsH)->classGroupCode;
	CheckMenuItem (gPopUpClassGroupMenu2, classGroupCode, TRUE);
	
			// Enable group menu item if groups exist.
				
	SInt16 grafPortType = 
						((WindowInfoPtr)*gActiveImageWindowInfoH)->grafPortType;
	if (((FileInfoPtr)*gActiveImageFileInfoH)->numberGroups > 0 && 
		(grafPortType == kCGrafType || classGroupCode == kGroupDisplay))
		EnableMenuItem (gPopUpClassGroupMenu2, kGroupDisplay);
		
	else		// fileInfoPtr->numberGroups <= 0 || ...
		DisableMenuItem (gPopUpClassGroupMenu2, kGroupDisplay);
		
			// Display the popup menu and let the user select the desired
			// option.
			 
	EraseRect (popUpBoxPtr);
	menuData = PopUpMenuSelect (
				gPopUpClassGroupMenu2, popUpLoc.v, popUpLoc.h, classGroupCode);
	
			// Remove the check by the "old" selection.									
			
	CheckMenuItem (gPopUpClassGroupMenu2, classGroupCode, FALSE);
	DeleteMenu (kPopUpClassGroupMenuID2);
	
			// Invalidate the area within the pop up box.								
			
  	InvalWindowRect (theWindow, popUpBoxPtr);
	
			// Check if a new class-group code was selected. If so make
			// make the user requested change.
			
	SInt16 newClassGroupCode = LoWord (menuData);
	
	if (classGroupCode != newClassGroupCode)
		{
		UpdateActiveImageLegend (newClassGroupCode, kCallCreatePalette);
//		UpdatePaletteWindow (TRUE, FALSE);
		gUpdateFileMenuItemsFlag = TRUE;
		gUpdatePaletteMenuItemsFlag = TRUE;
		
		}		// end "if (classGroupCode != newClassGroupCode)"
			
}		// end "LegendPopUpMenu" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ThematicImageWBlink
//
//	Software purpose:	The purpose of this routine is to handle mouse
//							down events in the thematic image window when
//							the cursor is a 'blink' cursor.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			DoThematicMouseDnContent  in thematicWindow.c
//
//	Coded By:			Larry L. Biehl			Date: 01/31/1991
//	Revised By:			Larry L. Biehl			Date: 04/26/2016	

void ThematicImageWBlink (
				Point									mousePt)

{
	Point									offscreenPoint;
	
	CTabHandle							cTableHandle;
	DisplaySpecsPtr					displaySpecsPtr;
	FileInfoPtr							fileInfoPtr;
	SInt16*								classToGroupPtr;
	unsigned char* 					offScreenBufferPtr;
	
	Handle								displaySpecsH,
											offScreenMapH;
												
	PaletteHandle						paletteHandle;
	
	SInt32								rowBytes;
	
	UInt32								numberClasses;
	
	UInt16								colorIndex,
											group;
											
	SignedByte							handleStatus;
	
	
			// Initialize local variables.													
			
	displaySpecsH = ((WindowInfoPtr)*gActiveImageWindowInfoH)->displaySpecsHandle;
	
	if (gOSXCoreGraphicsFlag)
		{
		#if TARGET_API_MAC_CARBON
		
		Handle								imageBaseAddressH;
	
		UInt32								boundsBottom,
												boundsRight;
		
		imageBaseAddressH = 
						((WindowInfoPtr)*gActiveImageWindowInfoH)->imageBaseAddressH;
		rowBytes = ((WindowInfoPtr)*gActiveImageWindowInfoH)->cgInfo.contextRowBytes;
		
		boundsBottom = ((DisplaySpecsPtr)*displaySpecsH)->imageDimensions[kVertical];
		boundsRight = ((DisplaySpecsPtr)*displaySpecsH)->imageDimensions[kHorizontal];
		
				// Get the window point to the offscreen location.							
				
		ConverWinPointToOffScreenPoint (
									gActiveImageWindowInfoH, mousePt, &offscreenPoint);
									
		if (offscreenPoint.h >= boundsRight)
																							return;
									
		if (offscreenPoint.v >= boundsBottom)
																							return;
																							
		offScreenBufferPtr = (unsigned char*)*imageBaseAddressH;
		
		#endif	// TARGET_API_MAC_CARBON
		
		}		// end "if (gOSXCoreGraphicsFlag)" 
		
	else		// !gOSXCoreGraphicsFlag
		{
		offScreenMapH = ((WindowInfoPtr)*gActiveImageWindowInfoH)->offScreenMapHandle;
		rowBytes = MGetPixRowBytes ((PixMapHandle)offScreenMapH);
	
				// Get the window point to the offscreen location.							
				
		ConverWinPointToOffScreenPoint (
									gActiveImageWindowInfoH, mousePt, &offscreenPoint);
									
		if (offscreenPoint.h >= ((BitMap*)*offScreenMapH)->bounds.right)
																							return;
									
		if (offscreenPoint.v >= ((BitMap*)*offScreenMapH)->bounds.bottom)
																							return;
																							
			// Get the class for the point.		
		
		offScreenBufferPtr = 
					(unsigned char*)GetPixBaseAddr ((PixMapHandle)offScreenMapH);
		
		}		// end "!gOSXCoreGraphicsFlag" 
		
	colorIndex = (unsigned short int)
				offScreenBufferPtr[rowBytes*offscreenPoint.v + offscreenPoint.h];
				
			// Get the current color table.													
			
	handleStatus = HGetState (displaySpecsH);
	HLock (displaySpecsH);
	displaySpecsPtr = (DisplaySpecsPtr)*displaySpecsH;
				
			// Exit if the color to be changed is white, black or background.  	
			// These colors are not to be changed.											
	
	numberClasses = displaySpecsPtr->numberLevels;
	if (colorIndex >= numberClasses + displaySpecsPtr->paletteOffset)
		{
		HSetState (displaySpecsH, handleStatus);
																							return;
		
		}		// end "if (colorIndex <= 0 || ..." 
		
			// Get color table and group if needed.  Also check if this class 	
			// is being displayed.																
			
	group = -1;			
 	if (displaySpecsPtr->classGroupCode == kClassDisplay)
 		{
		cTableHandle = displaySpecsPtr->savedClassCTableHandle;
		
		}		// end "if (displaySpecsPtr->classGroupCode == kClassDisplay)" 
		
	else		// displaySpecsPtr->classGroupCode == kGroupDisplay 
		{
		fileInfoPtr = (FileInfoPtr)*gActiveImageFileInfoH;
		cTableHandle = displaySpecsPtr->savedGroupCTableHandle;
//		classToGroupPtr = (SInt16*)*fileInfoPtr->groupTablesHandle;
//		group = classToGroupPtr[fileInfoPtr->numberClasses + 
//										colorIndex - displaySpecsPtr->paletteOffset];
		classToGroupPtr = GetClassToGroupPointer(fileInfoPtr);
		group = classToGroupPtr[colorIndex - displaySpecsPtr->paletteOffset];
										
		}		// end "else displaySpecsPtr->classGroupCode == kGroupDisplay" 
	
	paletteHandle = displaySpecsPtr->paletteObject;
	gPaletteOffset = displaySpecsPtr->paletteOffset;
	gClassPaletteEntries = displaySpecsPtr->numPaletteEntriesUsed;
	
	ChangeClassGroupPalette (paletteHandle, 
										group, 
										colorIndex, 
										&displaySpecsPtr->backgroundColor,
										2,
										0);
	
	if (gBlinkProcedure == kBlink1)
		DoThematicWColorsUpdate ();
	
	MSetCursor (kBlinkShutCursor);
	
	while (StillDown ()) { }
		
			// Reset the color for the pixel.												
	
	ChangeClassGroupPalette (paletteHandle, 
										group, 
										colorIndex, 
										NULL, 
										2,
										0);
	
	if (gBlinkProcedure == kBlink1)
		DoThematicWColorsUpdate ();
	
	MSetCursor (kBlinkOpenCursor2);
		
	HSetState (displaySpecsH, handleStatus);
					
}		// end "ThematicImageWBlink"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ThematicLegendWMouseDn
//
//	Software purpose:	The purpose of this routine is to handle mouse
//							down events in the legend part of the
//							thematic image window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			MouseDnEvents  in multiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 01/26/1990
//	Revised By:			Larry L. Biehl			Date: 11/01/1996	

void ThematicLegendWMouseDn (
				Point									mousePt)

{
	Cell									lCell;
	
	Handle								displaySpecsH;
	ListHandle							legendListHandle;
	
	SInt16								cellLine,
											listType;
	
	Boolean								doubleClick;
	
	
	legendListHandle = ((WindowInfoPtr)*gActiveImageWindowInfoH)->legendListHandle;
	gViewRect = ((ListPtr)*legendListHandle)->rView;
	if (PtInRect (mousePt, &gViewRect))
		{	
				// Update globals to reflect the palette offset and the number of 
				// palette entries for this image
							
		displaySpecsH = GetActiveDisplaySpecsHandle ();
		gPaletteOffset = ((DisplaySpecsPtr)*displaySpecsH)->paletteOffset;
		gClassPaletteEntries = 
							((DisplaySpecsPtr)*displaySpecsH)->numPaletteEntriesUsed;
			
				// Prepare for the first time through the click loop.

		PenSize (1,1);
		gFirstTimeClikLoop = TRUE;
		doubleClick = LClick (mousePt, gEventRecord.modifiers, legendListHandle);
		
		if (gPresentCursor == kArrow)
			{
					// Get class list type.
					
			listType = (((ListPtr)*legendListHandle)->refCon & 0x00000006)/2;
			
			if (doubleClick)
				{
				lCell = LLastClick (legendListHandle);
				cellLine = lCell.v;
					
				if (mousePt.h > gViewRect.left + 15)
					{
							// Edit the group name if the list line represents a group,
							// edit the class name if the list line represents a class,
							// or create a new group if the list line represents a 
							// class and the option key is down.
							
					SInt16 stringLength = 2;
					LGetCell (&cellLine, &stringLength, lCell, legendListHandle);
					SInt16 classGroupChangeCode = kEditClass;
					if (cellLine & 0x8000)
						classGroupChangeCode = kEditGroup;
			
					else		// !(cellLine & 0x8000)
						{
						if ((listType == kGroupClassDisplay) &&
													(gEventRecord.modifiers & controlKey))
							classGroupChangeCode = kNewGroup;
						
						}		// end "else !(cellLine & 0x8000)"
						
					cellLine &= 0x7fff;
							
					if (EditGroupClassDialog (
								legendListHandle, lCell.v, classGroupChangeCode, cellLine))
						{
//						if (classGroupChangeCode == kNewGroup)
//							UpdatePaletteWindow (TRUE, FALSE);
						
						}		// end "if (EditGroupClassDialog (..." 
					
					}		// end "if (mousePt.h > gViewRect.left + 15)" 
				
				else 		// && (mousePt.h <= gViewRect.left + 15
					{
					if (gColorQDflag)
							// Allow editing palette only if color quickdraw is present.
							
						EditClassGroupPalette (legendListHandle,
													displaySpecsH,
													cellLine,
													listType);
						
					}		// end "else (mousePt.h <= gViewRect.left + 15" 
			
				}		// end "if (doubleClick)"
			
			else		// !doubleClick
				{		
				if (listType == kGroupClassDisplay)
					EditGroups (legendListHandle); 
				
				UpdateThematicLegendControls (gActiveImageWindow);
							
				}		// else !doubleClick
				
			}		// end "if (gPresentCursor == kArrow)"	
																		
		}		// end "if (PtInRect (mousePt, &rect))" 
					
}		// end "ThematicLegendWMouseDn" 



#if TARGET_API_MAC_CARBON
//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void LoadOSXColorTable
//
//	Software purpose:	This routine loads the color table to be used to create a
//							core graphics image.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None	
// 
// Called By:			ActivateImageWindowPalette in SPalette.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/25/2001
//	Revised By:			Larry L. Biehl			Date: 10/25/2001	

void LoadOSXColorTable (
				CMPaletteInfo						paletteHandle, 
				UCharPtr								osxColorTablePtr)

{
	RGBColor								theColor;
	
	UInt32								index,
											numberEntries,
											tableIndex;
											
				
	numberEntries = MGetNumberPaletteEntries (paletteHandle);
	
	tableIndex = 0;				
	for (index=0; index<numberEntries; index++)
		{
		::GetEntryColor (paletteHandle, 
								index, 
								&theColor);
			
		osxColorTablePtr[tableIndex] = (theColor.red >> 8);
		osxColorTablePtr[tableIndex+1] = (theColor.green >> 8);
		osxColorTablePtr[tableIndex+2] = (theColor.blue >> 8);
		
		tableIndex += 3;
		
		}		// end "for (index=0; index<numberEntries; index++)"

}		// end "LoadOSXColorTable"
#endif	// TARGET_API_MAC_CARBON



#if TARGET_API_MAC_CARBON
//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateOSXColorSpace
//
//	Software purpose:	This routine creates a new core graphics image with the input
//							color table.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None	
// 
// Called By:			ActivateImageWindowPalette in SPalette.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/26/2001
//	Revised By:			Larry L. Biehl			Date: 10/26/2001

void UpdateOSXColorSpace (
				CMPaletteInfo						paletteHandle,
				UCharPtr								osxColorTablePtr)

{
	CGColorSpaceRef					baseColorSpace,
											colorSpace;
								
	CGDataProviderRef					provider;
	CGImageRef 							imageRef;
	Ptr									offScreenBufferPtr = NULL;
	WindowInfoPtr						imageWindowInfoPtr;
	
	Handle								imageBaseAddressH;
	
	size_t								bytesPerRow,
											imageHeight,
											imageWidth;
											
	UInt32								numberEntries,
											offscreenMapSize;
	
	
	imageWindowInfoPtr = (WindowInfoPtr)*gActiveImageWindowInfoH;
	
	imageRef = imageWindowInfoPtr->cgInfo.imageRef;
	if (imageRef != NULL)
		{
		imageWidth = gCGImageGetWidthPtr (imageWindowInfoPtr->cgInfo.imageRef);
		imageHeight = gCGImageGetHeightPtr (imageWindowInfoPtr->cgInfo.imageRef);
		bytesPerRow = gCGImageGetBytesPerRowPtr (imageWindowInfoPtr->cgInfo.imageRef);
		
		gCGImageReleasePtr (imageRef);
		
		}		// end "if (imageRef != NULL)"
		
	imageBaseAddressH = imageWindowInfoPtr->imageBaseAddressH;
	offscreenMapSize = imageWindowInfoPtr->offscreenMapSize;
			
			// Lock the handle for the offscreen bit map and keep it locked.
		
	offScreenBufferPtr = GetHandlePointer (
									imageBaseAddressH, 
									kNoLock, 
									kNoMoveHi);
				
			// Note that the image base address handle is already locked.
			
	provider = gCGDataProviderCreateWithDataPtr (
									NULL,
									offScreenBufferPtr, 
									offscreenMapSize, 
									NULL);
		
			// Create an indexed color space for a thematic window.
			
	baseColorSpace = gCGColorSpaceCreateDeviceRGBPtr ();
	
	numberEntries = MGetNumberPaletteEntries (paletteHandle);
							
	colorSpace = gCGColorSpaceCreateIndexedPtr (
										baseColorSpace, 
										numberEntries-1, 
										osxColorTablePtr);
			
			// Create the image.
			
	((WindowInfoPtr)*gActiveImageWindowInfoH)->cgInfo.imageRef = gCGImageCreatePtr (
									imageWidth,
									imageHeight,
									8,
									8,
									bytesPerRow,
									colorSpace,
									kCGImageAlphaNoneSkipFirst,
									provider,
									NULL,
									1,
									kCGRenderingIntentDefault);
										
			// Release the references to the provider and the colorspace. Copies
			// are retained by cgImage.
			
	gCGDataProviderReleasePtr (provider);
	
	gCGColorSpaceReleasePtr (baseColorSpace);
	gCGColorSpaceReleasePtr (colorSpace);

}		// end "UpdateOSXColorSpace"
#endif	// TARGET_API_MAC_CARBON
