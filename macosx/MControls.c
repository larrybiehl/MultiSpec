//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						MControls.c
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			04/27/2018
//
//	Language:				C
//
//	System:					Macintosh Operating System
//
//	Brief description:	The routines in this file handle control events in the
//								image and output windows.
//
//	Functions in file:	void 					DoImageZoom
//								void 					GetHorAndVerControlHandles
//								void 					HandScrollImage
//								void 					ImageWControlEvent
//								pascal void 		ImageWScroll
//								pascal void 		ImageWThumbScrollAction
//								pascal void 		ImageZoomControlEvent
//								pascal void 		OutputAutoScroll
//								void 					OutputWControlEvent
//								pascal void 		OutputWScroll
//								void 					UpdateImageWScrolls
//								void 					UpdateImageZoomControls
//								void 					UpdateOutputWScrolls
//								void 					UpdateScrolls
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
//	Diagram of MultiSpec routine calls for the routines in the file.
//
//		ImageWControlEvent
//			GetWindowClipRectangle (in multiSpec.c)
//			CopyOffScreenImage (in multiSpec.c)
//			ImageWScroll
//				GetWindowClipRectangle (in multiSpec.c)
//				CopyOffScreenImage (in multiSpec.c)
//			DoImageZoom
//				UpdateImageZoomControls
//				UpdateImageWScrolls
//
//		OutputWControlEvent
//			OutputWHScroll
//			OutputWVScroll
//
//		OutputAutoScroll
//			OutputWHScroll
//			OutputWVScroll
//
//		UpdateScrolls
//			UpdateOutputWScrolls
//			UpdateImageWScrolls
//				GetWindowClipRectangle (in MultiSpec.c)
//
//------------------------------------------------------------------------------------

//#include		"wePrint.h" 

#ifdef __cplusplus
extern "C" {
#endif

#if !use_mlte_for_text_window
pascal StyledLineBreakCode _WENoWrapLineBreak (
										const char *		inTextPtr,
										SInt32				inTextLength,
										SInt32				inTextStart,
										SInt32				inTextEnd,
										Fixed *				ioTextWidth,
										SInt32 *				ioTextOffset,
										WEReference			inWE);
#endif	// !use_mlte_for_text_window

#ifdef __cplusplus
}
#endif

//	miscellaneous constants

enum
{
			//	user tag used to save scroll step information in scrollbars
	kScrollStepTag		= 'STEP'	
};

		//	pixels to scroll when the scroll bar arrow is clicked
#define kScrollDelta		11			

extern Boolean InitializeWordSolutionEngine (void);



		// Prototypes for routines in this file that are only called by other routines 
		// in this file.				

void		AreaUnitsControlEvent (
				ControlHandle						theControl, 
				WindowPtr							theWindow, 
				Point									location);
				
void 		CoordinateUnitsControlEvent (
				ControlHandle						theControl, 
				WindowPtr							theWindow, 
				Point									location);										
			
void 		DoImageZoom (
				WindowPtr							theWindow, 
				DisplaySpecsPtr					displaySpecsPtr, 
				double								magnification, 
				Boolean								updateZoomsFlag);
				
void 		DrawPopupControl (
				ControlHandle						theControl,
				SInt16								popUpIconID);		

void 		GetHorAndVerControlHandles (
				WindowPtr							windowPtr,
				ControlHandle* 					horizontalControlHandlePtr,
				ControlHandle* 					verticalControlHandlePtr);

void		ImageWindowScroll (
				ControlHandle 						theControl,
				SInt16	 							scrollBarCode, 
				SInt16	 							scrollButton);			

SInt32	ImageWThumbScrollStepValue (
				SInt32								maxControlValue,
				SInt32								scrollStep,
				SInt32								originalOrigin,
				double								magnification,
				SInt32*								newOriginPtr);	

void		OutputWScrollChanged (
				ControlHandle						theControl);

void 		OverlayControlEvent (
				ControlHandle						theControl, 
				WindowPtr							theWindow, 
				Point									location);

void 		SetOverlayControlHilite (
				WindowPtr							theWindow,
				ControlHandle						controlHandle);



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void AreaUnitsControlEvent
//
//	Software purpose:	This routine handles the area units control event.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			ImageWControlEvent in controls.c
//
//	Coded By:			Larry L. Biehl			Date: 11/22/2000
//	Revised By:			Larry L. Biehl			Date: 11/22/2000			

void AreaUnitsControlEvent (
				ControlHandle						theControl, 
				WindowPtr							theWindow, 
				Point									location)
				
{	
	Handle								windowInfoHandle;
	MenuHandle							menuHandle;
	
	SInt16 								areaUnitsSelection;
	
	
			// Initialize local variables
			
	windowInfoHandle = GetWindowInfoHandle (theWindow);
	
			// Set up the popup menu to properly reflect the options available.

	menuHandle = GetPopUpMenuHandle (theControl);
	SetUpAreaUnitsPopUpMenu (menuHandle, windowInfoHandle, NULL);	
					
	TrackControl (theControl, location, (ControlActionUPP) -1);
																					
	areaUnitsSelection = GetControlValue (theControl);
	
	if (areaUnitsSelection > 0 &&
					areaUnitsSelection != GetCoordinateViewAreaUnits (windowInfoHandle))
		{
		SetCoordinateViewAreaUnits (windowInfoHandle, areaUnitsSelection);
					
		SetCoordinateViewLocationParameters (windowInfoHandle);
		
		InvalidateWindow (gActiveImageWindow, kCoordinateAreaUnitsArea, FALSE);		

		DrawCoordinateViewControls (windowInfoHandle);
		
		UpdateSelectionCoordinates (windowInfoHandle);
		
		}	// end "if (coordinateUnitsSelection > 0 && ..."
		
	HiliteControl (theControl, 0);
	GetControlBounds ((ControlRef)theControl, &gTempRect);
	InvalWindowRect (gActiveImageWindow, &gTempRect);
	  
}	// end "AreaUnitsControlEvent" 	

	

//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void ControlDefinition
//
//	Software purpose:	This routine is the control definition for the control
//							popup menu.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date:12/11/1996
//	Revised By:			Larry L. Biehl			Date:12/11/1996			

pascal SInt32 ControlDefinition (
				SInt16								varCode, 
				ControlHandle						theControl,
				SInt16								message, 
				SInt32								longInt)	

{
	SInt32 								returnValue = 0;
	
	
			// Switch to oppropriate section.												
		
	switch (message)
		{
		case drawCntl:
			DrawPopupControl (theControl, (SInt16)(10000+varCode));
			break;
			
		case testCntl:
			if (GetControlHilite (theControl) < 255)
				{
				Point		point;
				point.v = HiWord (longInt);
				point.h = LoWord (longInt);
				if (PtInRect (point, GetControlBounds ((ControlRef)theControl, &gTempRect)))
					returnValue = kControlButtonPart;
					
				}	// end "if ((*theControl)->contrlHilite < 255)"
			break;
			
		case calcCRgns:	// only called in 24-bit mode
			//#if !TARGET_CPU_PPC
			#if TARGET_CPU_68K
				if (longInt != 0)
					{	// is 32-bit addressing off?
					SInt32	result	= 0;
						
					OSErr	errorCode = Gestalt (gestaltAddressingModeAttr,&result);
					if (!errorCode && ((result & (1L << gestalt32BitAddressing)) == 0))
						{
						RgnHandle	region = (RgnHandle)StripAddress ((Ptr)longInt);
						RectRgn ((RgnHandle)region, &(*theControl)->contrlRect);
							
						}	// end "if (!errorCode && ..."
						
					}	// end "if (longInt != 0)"
			#endif	// TARGET_CPU_68K		//!TARGET_CPU_PPC
			break;
			
		case initCntl:
			break;
			
		case dispCntl:
			break;
			
		case posCntl:
			break;
			
		case thumbCntl:
			break;
			
		case dragCntl:
			break;
			
		case autoTrack:
			break;
			
		case calcCntlRgn:
			if (longInt != 0)
				RectRgn ((RgnHandle)longInt, 
								GetControlBounds ((ControlRef)theControl, &gTempRect));
			break;
			
		case calcThumbRgn:
			break;
			
		case drawThumbOutline:
			break;
		
		}	// end "switch (message)"
		
	return (returnValue);

}	// end "ControlDefinition"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CoordinateUnitsControlEvent
//
//	Software purpose:	This routine handles the coordinate units control event.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			ImageWControlEvent in controls.c
//
//	Coded By:			Larry L. Biehl			Date: 11/06/2000
//	Revised By:			Larry L. Biehl			Date: 04/29/2012			

void CoordinateUnitsControlEvent (
				ControlHandle						theControl, 
				WindowPtr							theWindow, 
				Point									location)
				
{	
	Handle								windowInfoHandle;
	MenuHandle							menuHandle;
	
	SInt16 								coordinateUnitsSelection;
	
	
			// Initialize local variables
			
	windowInfoHandle = GetWindowInfoHandle (theWindow);
	
			// Set up the popup menu to properly reflect the options available.

	menuHandle = GetPopUpMenuHandle (theControl);
	SetUpCoordinateUnitsPopUpMenu (menuHandle, windowInfoHandle, NULL);	
					
	TrackControl (theControl, location, (ControlActionUPP)-1);
																					
	coordinateUnitsSelection = GetControlValue (theControl);
	
	if (coordinateUnitsSelection > 0 &&
				coordinateUnitsSelection != GetCoordinateViewUnits (windowInfoHandle))
		{
		SetCoordinateViewUnits (windowInfoHandle, coordinateUnitsSelection);
					
		SetCoordinateViewLocationParameters (windowInfoHandle);
		
		UpdateSelectionCoordinates (windowInfoHandle);
		
		//InvalidateWindow (gActiveImageWindow, kCoordinateArea, FALSE);
		InvalidateWindow (gActiveImageWindow, kCoordinateCursorArea, FALSE);
		InvalidateWindow (gActiveImageWindow, kCoordinateSelectionArea, FALSE);
		InvalidateWindow (gActiveImageWindow, kCoordinateAreaUnitsArea, TRUE);
		
		}	// end "if (coordinateUnitsSelection > 0 && ..."

	DrawCoordinateViewControls (windowInfoHandle);
		
	HiliteControl (theControl, 0);
	GetControlBounds ((ControlRef)theControl, &gTempRect);
	InvalWindowRect (theWindow, &gTempRect);
	  
}	// end "CoordinateUnitsControlEvent" 	



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		DoImageZoom
//
//	Software purpose:	This routine handle redrawing an image at a 
//							different magnification.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			ImageWControlEvent in controls.c
//							ImageZoomControlEvent in controls.c
//
//	Coded By:			Larry L. Biehl			Date: 12/02/1988
//	Revised By:			Larry L. Biehl			Date: 10/16/2001			

void DoImageZoom (
				WindowPtr							theWindow, 
				DisplaySpecsPtr					displaySpecsPtr, 
				double								magnification,
				Boolean								updateZoomsFlag)

{
			// Local Structures and Variables	
			
	LongPoint							newWindowPoint,
											offScreenPoint;
	
	Rect									imageRect,
											selRect,
											titleRect;
	
	SelectionInfoPtr					selectionInfoPtr;											
	
	Handle								selectionInfoH;
	
	SInt32								windowColumnCenter,
											windowLineCenter;
	
	SInt32								imageColumnCenter,
											imageLineCenter,
											imageTopOffset,
											titleHeight;
											
	Boolean								checkForErase;
	
	
			// Check input parameters.															
			
	if (magnification < gMinMagnification)
																									return;
		
			// Initialize local variables.													
																							
	titleHeight = ((WindowInfoPtr)*gActiveWindowInfoH)->titleHeight;
				
			// Get image area in the window.													
				
	GetWindowClipRectangle (theWindow, kImageFrameArea, &imageRect);
						
			// Get window and image center for adjusting the origin so that the	
			//	window is centered around the selection rectangle area if it as 	
			//	been set up or else the present center of the image					
			
	windowLineCenter = (imageRect.bottom - imageRect.top)/2;
	windowColumnCenter = (imageRect.right - imageRect.left)/2;

			// Get the selection rectangle area												

	selectionInfoH = ((WindowInfoPtr)*gActiveWindowInfoH)->selectionInfoHandle;
	selectionInfoPtr = (SelectionInfoPtr)*selectionInfoH;

	if (magnification > displaySpecsPtr->magnification &&
																selectionInfoPtr->typeFlag != 0)
		{
		imageLineCenter = (selectionInfoPtr->offScreenRectangle.top + 
												selectionInfoPtr->offScreenRectangle.bottom)/2;
										
		imageColumnCenter = (selectionInfoPtr->offScreenRectangle.left + 
												selectionInfoPtr->offScreenRectangle.right)/2;
										
		}	// end "if (selectionInfoPtr->typeFlag != 0)" 
		
	else	// No selection rectangle has been made or the user is zooming		
				// out.																				
		{
		imageLineCenter = (SInt32)(windowLineCenter/displaySpecsPtr->magnification + 
														displaySpecsPtr->origin[kVertical] + 0.5);
		if (imageLineCenter > displaySpecsPtr->imageDimensions[kVertical])
			imageLineCenter = (SInt32)(displaySpecsPtr->imageDimensions[kVertical]/2);
				
		imageColumnCenter = (SInt32)(windowColumnCenter/displaySpecsPtr->magnification + 
														displaySpecsPtr->origin[kHorizontal] + 0.5);

		if (imageColumnCenter > displaySpecsPtr->imageDimensions[kHorizontal])
			imageColumnCenter = (SInt32)(displaySpecsPtr->imageDimensions[kHorizontal]/2);
							
		}	// end "else no selection rectangle made" 
	
			// Update origin and magnification in the display structure, update	
			// the scroll bars, and invalidate the window								
				
	displaySpecsPtr->origin[kVertical] = (SInt32)(MAX (
										0, 
										imageLineCenter - windowLineCenter/magnification + .5));
	
	displaySpecsPtr->origin[kHorizontal] = (SInt32)(MAX (
								0, 
								imageColumnCenter - windowColumnCenter/magnification + .5));
						
			// Determine whether the display window needs to be erased. 			
			// When zooming out, the new image in 											
			// the window may be smaller that the old image.  If so the 			
			// old image needs to be erase so that the outside edges of 			
			// the old image do not show after the update.								
						
	checkForErase = false;
	if (displaySpecsPtr->magnification > magnification)
		checkForErase = true;
	
	displaySpecsPtr->magnification = magnification;
	
	if (checkForErase)
		{			
		offScreenPoint.v = (SInt32)(displaySpecsPtr->imageDimensions[kVertical]);
		offScreenPoint.h = (SInt32)(displaySpecsPtr->imageDimensions[kHorizontal]);
		ConvertOffScreenPointToWinPoint (
									gActiveWindowInfoH, &offScreenPoint, &newWindowPoint);
			
		if (newWindowPoint.h < imageRect.right)
			{
			selRect = imageRect;
			selRect.left = (SInt16)newWindowPoint.h;
			selRect.bottom = MIN (selRect.bottom, (SInt16)newWindowPoint.v);
			EraseRect (&selRect);
			
			}	// end "if (newWindowPoint.h < imageRect.right)" 
			
		if (newWindowPoint.v < imageRect.bottom)
			{
			selRect = imageRect;
			selRect.top = (SInt16)newWindowPoint.v;
			EraseRect (&selRect);
			
			}	// end "if (newWindowPoint.v < imageRect.bottom)" 
				
		}	// end "if (checkForErase)" 
	
	UpdateImageZoomControls (theWindow, magnification, updateZoomsFlag);
	UpdateImageWScrolls (theWindow);
	
	if (titleHeight > 0)
		{
				// Get the title rectangle.	
				
		imageTopOffset = (SInt32)(((WindowInfoPtr)*gActiveWindowInfoH)->imageTopOffset);														
				
		titleRect.top = (SInt16)(imageTopOffset - titleHeight);
		titleRect.left = imageRect.left;
		titleRect.bottom = (SInt16)imageTopOffset;
		titleRect.right = imageRect.right;

		EraseRect (&titleRect);

		}	// end "if (titleHeight > 0)"
	
	CopyOffScreenImage (theWindow, 
								NIL, 
								&imageRect, 
								kDestinationCopy, 
								kNotFromImageUpdate);
	
	imageRect.top = 0;
	ValidWindowRect (theWindow, &imageRect);
				
}	// end "DoImageZoom"  

	

//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void DrawPopupControl
//
//	Software purpose:	This routine is the control definition for the control
//							popup menu.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date:12/11/1996
//	Revised By:			Larry L. Biehl			Date:02/07/2018

void DrawPopupControl (
				ControlHandle						theControl,
				SInt16								popUpIconID)	

{
	PenState								penState;
	
	Rect									iconRect,
											rect;
						
	WindowPtr							windowPtr;
	
	SInt16								align,
											transform;
											
	Boolean								visibleFlag = FALSE;
	
	
	visibleFlag = IsControlVisible (theControl);
	
	if (visibleFlag)
		{	
				// Save the current pen state.											
			
		GetPenState (&penState);
		PenNormal ();
		
		windowPtr = GetControlOwner (theControl);
		GetControlBounds ((ControlRef)theControl, &iconRect);
		rect = iconRect;
		
		
		EraseRect (&rect);
		FrameRect (&rect);

				// Draw the drop down triangle
				
		if (gTempRegion1 != NULL)
			{
			OpenRgn ();
			MoveTo ((SInt16)(rect.right - 4), (SInt16)(rect.top + 9));
			LineTo ((SInt16)(rect.right - 10), (SInt16)(rect.top + 9));
			LineTo ((SInt16)(rect.right - 7), (SInt16)(rect.top + 13));
			LineTo ((SInt16)(rect.right - 4), (SInt16)(rect.top + 9));
			CloseRgn (gTempRegion1);
			InvertRgn (gTempRegion1);
			SetEmptyRgn (gTempRegion1);
			
			}	// end "if (gTempRegion1 != NULL)" 
			
		if (GetControlHilite (theControl) == 10)
			{
			InsetRect (&rect,1,1);
			InvertRect (&rect);
			
			}	// end "if (GetControlHilite (theControl) == 10)"

		if (GetControlHilite (theControl) == 255)
			{
			align = atNone;
			transform = ttDisabled;
			iconRect.left += 2;
			
			}	// end "if (GetControlHilite (theControl) == 255)"
		
		else if (GetControlHilite (theControl) == 10)
			{
			align = atAbsoluteCenter;
			transform = ttSelected;
			
			}	// end "if (GetControlHilite (theControl) == 10)"
			
		else	// GetControlHilite (theControl) != 10
			{
			align = atNone;
			transform = ttNone;
			iconRect.left += 2;
			
			}	// end "else GetControlHilite (theControl) != 10"
			
		PlotIconID (&iconRect, align, transform, popUpIconID);

		if (GetControlHilite (theControl) == 255)
			FrameRect (&rect);
	
				// Reset the pen state back to the original settings.				
	
		SetPenState (&penState);
		
		}	// end "if (visibleFlag)"
				
	else	// !(*theControl)->contrlVis
		EraseRect (GetControlBounds ((ControlRef)theControl, &rect));
				
}	// end "DrawPopupControl" 
	


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetHorAndVerControlHandles
//
//	Software purpose:	This routine gets the horizontal and vertical control handles.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 09/25/1998
//	Revised By:			Larry L. Biehl			Date: 04/13/2001			

void GetHorAndVerControlHandles (
				WindowPtr							windowPtr,
				ControlHandle* 					horizontalControlHandlePtr,
				ControlHandle* 					verticalControlHandlePtr)

{
	WindowInfoPtr						windowInfoPtr;

	Handle								windowInfoHandle;
	
	
	*horizontalControlHandlePtr = NULL;
	*verticalControlHandlePtr = NULL;
	  		
  	windowInfoHandle = (Handle)GetWRefCon (windowPtr);
  	windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
  																		
  	if (windowInfoPtr != NULL)
  		{
		*verticalControlHandlePtr = windowInfoPtr->verticalControl;
		*horizontalControlHandlePtr = windowInfoPtr->horizontalControl;
  		
  		}	// end "if (windowInfoPtr != NULL)"
  		
}	// end "GetHorAndVerControlHandles" 
	


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetHorAndVerControlMaximums
//
//	Software purpose:	This routine gets the horizontal and control maximums for the
//							input image window.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 09/25/1998
//	Revised By:			Larry L. Biehl			Date: 10/31/2001			

void GetHorAndVerControlMaximums (
				WindowPtr							windowPtr,
				SInt32*			 					horizontalMaximumPtr,
				SInt32*			 					verticalMaximumPtr)

{
	ControlHandle						hControlHandle,
											vControlHandle;
											
											
	*horizontalMaximumPtr = 0;	
	*verticalMaximumPtr = 0;

	GetHorAndVerControlHandles (windowPtr, &hControlHandle, &vControlHandle);	
					
	if (hControlHandle != NULL)
		*horizontalMaximumPtr = GetControl32BitMaximum (hControlHandle);
		
	if (vControlHandle !=NULL)
		*verticalMaximumPtr = GetControl32BitMaximum (vControlHandle);	
	
}	// end "GetHorAndVerControlMaximums" 
	


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetPopUpMenuHandle
//
//	Software purpose:	This routine gets the menu handle for the popup menu control.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			ImageWScroll
//
//	Coded By:			Larry L. Biehl			Date: 12/11/1996
//	Revised By:			Larry L. Biehl			Date: 03/02/2018			

MenuHandle GetPopUpMenuHandle (
				ControlHandle 						controlHandle)

{	
	MenuHandle							popUpMenuHandle;
			
	Boolean								bevelButtonFlag = FALSE;
	
	
	if (controlHandle != NULL)
		{
		switch (GetControlReference (controlHandle))
			{
			case kOverlayControl:
			case kLegendPaletteControl:
			case kGraphVectorControl:
			case kGraphOverlayControl:
			case kGraphBinWidthControl:
				bevelButtonFlag = TRUE;
				break;
				
			case kCoordinateUnitsControl:
			case kAreaUnitsControl:
			case kGraphXAxisLabelControl:
				bevelButtonFlag = FALSE;
				break;
				
			default:
				return (NULL);
				
			}	// end "switch (GetControlReference (controlHandle))"
				
		if (bevelButtonFlag)
			{
			if (GetBevelButtonMenuHandle (controlHandle, &popUpMenuHandle) == noErr)
																			return (popUpMenuHandle);
				
			}	// end "if (gAppearanceManagerFlag)"
			
		else	// !bevelButtonFlag
			{
			if (controlHandle != NULL)
				{
				popUpMenuHandle = GetControlPopupMenuHandle (controlHandle);
									
																			return (popUpMenuHandle);
				
				}	// end "if (controlHandle != NULL)"
				
			}	// end "else !bevelButtonFlag"
			
		}	// end "if (controlHandle != NULL)"
		
	return (NULL);
				
}	// end "GetPopUpMenuHandle"  
	


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void HandScrollImage
//
//	Software purpose:	This routine scrolls the image in the window the
//							a number of pixels in x and y directions as specified
//							by the hand cursor.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			ImageWScroll
//
//	Coded By:			Larry L. Biehl			Date:  9/08/1988
//	Revised By:			Larry L. Biehl			Date: 03/16/2018

void HandScrollImage (
				WindowPtr							theWindow, 
				Point									oldPt)

{
			// Local Structures and Variables	
			
	Point									newPt;
	
	Rect									destRect,
											titleRect;	
	
	double								hOverMagnification,
											magnification,
											vOverMagnification;
	
	DisplaySpecsPtr					displaySpecsPtr;											
		
	ControlHandle						controlHHandl,
											controlVHandl;
	
	Handle								displaySpecsH;
	
	UInt32								finalTicks,
											imageTopOffset;
	
	SInt32								controlHValue,
											controlVValue,
											h,
											hImageDimensions,
											hOrigin,
											hWindowDimensions,
											scrollHStep,
											scrollVStep,
											titleHeight,
											v,
											vImageDimensions,
											vOrigin,
											vWindowDimensions;
	
					
			//	Get vertical and horizontal scroll bar handles and determine		
			// if image can be scrolled	
			
	GetHorAndVerControlHandles (theWindow, &controlHHandl, &controlVHandl);	
	
    		// Exit routine if scroll bar controls do not exist for the window	
    		
    if (controlHHandl != NULL && controlVHandl != NULL)
    	{
    			// Determine the directions that the image can be scrolled
			
		controlVValue = GetControl32BitMaximum (controlVHandl);
		controlHValue = GetControl32BitMaximum (controlHHandl);
		
    	if (controlVValue != 0 | controlHValue != 0)
			{	
			imageTopOffset = ((WindowInfoPtr)*gActiveWindowInfoH)->imageTopOffset;
			titleHeight = ((WindowInfoPtr)*gActiveWindowInfoH)->titleHeight;
	
					// Get handle to display specification for window					
					// and then get the image magnification								
			
			GetWindowClipRectangle (theWindow, kImageFrameArea, &destRect);
			
			displaySpecsH = ((WindowInfoPtr)*gActiveWindowInfoH)->displaySpecsHandle;
			HLock (displaySpecsH);
			displaySpecsPtr = (DisplaySpecsPtr)*displaySpecsH;
			magnification = displaySpecsPtr->magnification;
			
					// Get rectangle for the title in case it is needed.				
					
			titleRect.top = (SInt16)(imageTopOffset - titleHeight);
			titleRect.left = destRect.left;
			titleRect.bottom = (SInt16)imageTopOffset;
			titleRect.right = destRect.right;
	
					// Get window dimensions													
			
			vWindowDimensions = (SInt32)((destRect.bottom-destRect.top)/magnification);
			hWindowDimensions = (SInt32)((destRect.right-destRect.left)/magnification);
	
					// Get image dimensions														
			
			vImageDimensions = (SInt32)displaySpecsPtr->imageDimensions[kVertical];
			hImageDimensions = (SInt32)displaySpecsPtr->imageDimensions[kHorizontal];
								
    		h = (controlHValue == 0)  ? 0 : 1;
    		v = (controlVValue == 0)  ? 0 : 1;
    		
			hOverMagnification = h/magnification;
			vOverMagnification = v/magnification;
			
					// Set clip region of the window to include the scroll bars so	
					// that the thumb on the scroll can be moved.						
					
			ClipRect (GetWindowPortBounds (theWindow, &gTempRect));
    		
			do
				{
				GetMouse (&newPt);
				
				scrollHStep = (SInt32)(hOverMagnification*(oldPt.h - newPt.h));
				hOrigin = displaySpecsPtr->origin[kHorizontal];
				if (hOrigin + hWindowDimensions + scrollHStep > hImageDimensions)
					scrollHStep = hImageDimensions - hWindowDimensions - hOrigin;
				if (hOrigin + scrollHStep < 0)  
					scrollHStep = -hOrigin;
				
				scrollVStep = (SInt32)(vOverMagnification*(oldPt.v - newPt.v));
				vOrigin = displaySpecsPtr->origin[kVertical];
				if (vOrigin + vWindowDimensions + scrollVStep > vImageDimensions)
					scrollVStep = vImageDimensions - vWindowDimensions - vOrigin;
				if (vOrigin + scrollVStep < 0)  
					scrollVStep = -vOrigin;
				
						// Scroll area if the mouse has moved far enough to cause a	
						// scroll operation.	
						
				if (scrollHStep != 0 || scrollVStep != 0)
					{
					h = (SInt32)(scrollHStep * magnification);
					v = (SInt32)(scrollVStep * magnification);
					
					vOrigin += scrollVStep;
					hOrigin += scrollHStep;
						
					if (scrollHStep != 0)
						{
								// Do the horizontal scroll
						
						ScrollRect (&destRect,  (SInt16)-h, 0, gTempRegion1);
						if (QDIsPortBuffered (GetWindowPort (gActiveImageWindow)))
							QDFlushPortBuffer (GetWindowPort (gActiveImageWindow), NULL);
						Delay (1, &finalTicks);
						
								// Scroll the title if needed.									
								
						if (titleHeight > 0 && h != 0)
							{
							ScrollRect (&titleRect,  (SInt16)-h, 0, gTempRegion2);
							if (QDIsPortBuffered (GetWindowPort (gActiveImageWindow)))
								QDFlushPortBuffer (GetWindowPort (gActiveImageWindow), NULL);
							Delay (1, &finalTicks);
							
							}	// end "if (titleHeight > 0 && h != 0)"
							
								// Update the area that was made invalid by the h scroll.
								
						displaySpecsPtr->origin[kHorizontal] = hOrigin;	
						CopyOffScreenImage (gActiveImageWindow, 
													gTempRegion1, 
													NIL, 
													kScrollCopy, 
													kNotFromImageUpdate);
							
						}	// end "if (scrollHStep != 0)"	
						
					if (scrollVStep != 0)
						{
								// Do the vertical scroll
						
						SetEmptyRgn (gTempRegion1);
						ScrollRect (&destRect, 0, (SInt16)-v, gTempRegion1);
						if (QDIsPortBuffered (GetWindowPort (gActiveImageWindow)))
							QDFlushPortBuffer (GetWindowPort (gActiveImageWindow), NULL);
						Delay (1, &finalTicks);
		
								// Update the area that was made invalid by the v scroll.
								
						displaySpecsPtr->origin[kVertical] = vOrigin;
						CopyOffScreenImage (gActiveImageWindow, 
													gTempRegion1, 
													NIL, 
													kScrollCopy,
													kNotFromImageUpdate);	
						
						}	// end "if (scrollVStep != 0)"	
						
							// Now update the scroll bars	
					
					SetControl32BitValue (controlHHandl, hOrigin);
					SetControl32BitValue (controlVHandl, vOrigin);
					
							// Make new point the old point to be read for new mouse	
							// location.																
							
					oldPt = newPt;
					
					}	// end "if (scrollHStep != 0 || scrollVStep != 0)"	
					
				} while (StillDown ());
				
			HUnlock (displaySpecsH);
			
					// Set clip region of the window back to just include the		
					// image part of the window.												
					// Make certain that the the clip area includes the legend		
					// for thematic type images and the coordinate portion.											
			
			destRect.top = 0;
			destRect.left = 0;		
			ClipRect (&destRect);
			
			}	// end "if (unionInvalidRgn)" 
	
		SetEmptyRgn (gTempRegion1);
		SetEmptyRgn (gTempRegion2);
			
		}	// end "if (controlHHandl != NULL && controlVHandl != NULL)" 
	
}	// end "HandScrollImage" 
	


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ImageWControlEvent
//
//	Software purpose:	This routine handles control events in image 
//							windows.  Image window control events include
//							scroll bars, zoom in, zoom out, and magnification
//							display.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			DoImageMouseDnContent in MMultiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 08/30/1988
//	Revised By:			Larry L. Biehl			Date: 04/27/2018

void ImageWControlEvent (
				ControlHandle						theControl, 
				WindowPtr							theWindow, 
				Point									location, 
				SInt16								partCode)
				
{
			// Local Structures and Variables	
	
	DisplaySpecsPtr					displaySpecsPtr;											
	
	Handle								displaySpecsH;
											
	MenuHandle							popUpMenuHandle;
											
	SInt32								scrollPixels;
	
	UInt32								finalTicks;
	
	//#ifdef AVAILABLE_MAC_OS_X_VERSION_10_5_AND_LATER
	//	MenuItemIndex						paletteSelection;
	//#else	// 
		SInt16								paletteSelection;
	//#endif
	
	SInt16								errCode,
											oldOrigin,
											origin,
											refCon,
											scrollHPixels,
											scrollVPixels,
											selection;
	
	//Boolean								trackControlFlag;
	
	
			// Get the refCon of the control. This will contain a code for which 
			// control it is.																		
			// refCon = 0: Vertical scroll bar												
			// refCon = 1: Horizontal scroll bar											
			// refCon = 2: Zoom-in control													
			// refCon = 3: Zoom-out control													
			// refCon = 4: Magnification display control									
			// refCon = 5: Thematic legend scroll up control									
			// refCon = 6: Thematic legend scroll down control								
			// refCon = 7:Thematic legend text control								
			// refCon = 8:Thematic legend palette control							
			// refCon = 9:Coordinate view units control										
	
	refCon = (SInt16)GetControlReference (theControl);
	
	if (refCon >= kVerticalControl & refCon <= kAreaUnitsControl)
		{
				// Get display window rectangle less scroll bars, legend and title					
					
		GetWindowClipRectangle (theWindow, kImageFrameArea, &gViewRect);
		
				// Get handle and pointer to display specification for window		
			
		displaySpecsH = GetDisplaySpecsHandle (gActiveWindowInfoH);
		displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (displaySpecsH, kLock);
		gNextTime = 0;
		
		switch (refCon)
			{
			case kVerticalControl:		// Vertical scroll bar 
			case kHorizontalControl:		// Horizontal scroll bar 
				
				if (partCode == kControlIndicatorPart)
					{
					if (!(gEventRecord.modifiers & optionKey))
						{
						GetMouse (&gOldThumbScrollPoint);
						gCurrentControlHandle = theControl;
						
						partCode = TrackControl (
											theControl, location, gImageWThumbScrollActionPtr);
						
								// Make sure that scroll setting is correct. This causes
								// a flash but I do not know how to stop it currently.
					
						SetControl32BitValue (gCurrentControlHandle,
															displaySpecsPtr->origin[refCon]);
						
						}	// end "if (trackControlFlag)"
						
					else	// !trackControlFlag
						{
						partCode = TrackControl (theControl, location, NULL);
					
						if (partCode != 0)
							{
									// Get number of pixels to scroll window by and set	
									// new origin in the display specification				
									
							oldOrigin = (SInt16)displaySpecsPtr->origin[refCon];
				
							origin = (SInt16)GetControl32BitValue (theControl);
							
							scrollPixels = (SInt32)((oldOrigin - origin) * 
																	displaySpecsPtr->magnification);
																
									// Set up arguments for ScrollRect							
									// Be sure to handle situation where the title 			
									// should be scrolled left or right.						
									
							if (refCon == kVertical) 
								{ 
								scrollHPixels = 0;
								scrollVPixels = (SInt16)scrollPixels;
								
								}	// end "if (refCon == kVertical)"
								
							if (refCon == kHorizontal)
								{  
								scrollHPixels = (SInt16)scrollPixels;
								scrollVPixels = 0;
								
								SInt16 titleHeight = 
											((WindowInfoPtr)*gActiveWindowInfoH)->titleHeight;
								gViewRect.top -= titleHeight;
								
								}	// end "if (refCon == kHorizontal)" 
													
							ScrollRect (
									&gViewRect, scrollHPixels, scrollVPixels, gTempRegion1);
							if (QDIsPortBuffered (GetWindowPort (gActiveImageWindow)))
								QDFlushPortBuffer (GetWindowPort (gActiveImageWindow), NULL);
							Delay (1, &finalTicks);
									
									// Set values according to actual amount scrolled taking
									// into account truncation.
							
							origin = (SInt16)(oldOrigin - 
													scrollPixels/displaySpecsPtr->magnification);	
							displaySpecsPtr->origin[refCon] = origin;
					
							SetControl32BitValue (theControl, origin);
	
									// Update the area that was made invalid by the scroll.															
						
							CopyOffScreenImage (theWindow, 
														gTempRegion1, 
														NIL, 
														kScrollCopy,
														kNotFromImageUpdate);
	
							SetEmptyRgn (gTempRegion1);
							
							}	// end "if (partCode != 0)"
							
						}	// end "else !trackControlFlag" 
						
					}	// end "if (partCode == kControlIndicatorPart)" 
				
				else	// partCode == kControlUpButtonPart, kControlDownButtonPart, 
						//								kControlPageUpPart, kControlPageDownPart 
					TrackControl (theControl, location, gImageWScrollActionPtr);
				break;
				
			case kCoordinateViewControl:
				if (TrackControl (theControl, location, (ControlActionUPP) -1))
					ShowHideCoordinateView (gActiveWindowInfoH);
				break;
			
			case kZoomInControl:
			case kZoomOutControl:
				if (gAppearanceManagerFlag)
					HandleControlClick (theControl, 
												location,
												everyEvent,
												gImageZoomControlActionPtr);
				else	// !gAppearanceManagerFlag
					TrackControl (theControl, location, gImageZoomControlActionPtr);
					
				UpdateImageZoomControls (theWindow, displaySpecsPtr->magnification, true);
				break;
				
			case kMagControl:
						// If there was a mouseup in this control, then redraw the	
						// image window with a magnification of 1.						
				partCode = TrackControl (theControl, location, NIL);
				if (partCode != 0)
					DoImageZoom (theWindow, displaySpecsPtr, 1.0, true);
				break;
				
			case kOverlayControl:
						// The user has selected the overlay control.						
				OverlayControlEvent (theControl, theWindow, location);
				break;
				
			case kLegendScrollUpControl:
			case kLegendScrollDownControl:
						// If there was a mouseup in this control, scroll the thematic
						// legend as requested.	
				gPaletteOffset = (SInt16)displaySpecsPtr->paletteOffset;
				gClassPaletteEntries = (SInt16)displaySpecsPtr->numPaletteEntriesUsed;		
					
				partCode = TrackControl (
								theControl, location, gThematicLegendControlActionPtr);
				UpdateThematicLegendControls (theWindow);
				break;
				
			case kLegendPaletteControl:
            {
				SInt16 format = ((FileInfoPtr)*gActiveImageFileInfoH)->format;
				SInt16 ancillaryInfoFormat = 
									((FileInfoPtr)*gActiveImageFileInfoH)->ancillaryInfoformat;
				UInt32 numberClasses = 
									((FileInfoPtr)*gActiveImageFileInfoH)->numberClasses;
				Handle classDescriptionH = 
									((FileInfoPtr)*gActiveImageFileInfoH)->classDescriptionH;
				UInt32 colorTableOffset = 
									((FileInfoPtr)*gActiveImageFileInfoH)->colorTableOffset;
				
				errCode = noErr;
				paletteSelection = 0;
				
				popUpMenuHandle = GetPopUpMenuHandle (theControl);
				if (popUpMenuHandle != NULL)
					{
					if (displaySpecsPtr->classGroupCode == kClassDisplay)
						paletteSelection = displaySpecsPtr->thematicClassPaletteType;
					
					else	// ...classGroupCode != kClassDisplay
						paletteSelection = displaySpecsPtr->thematicGroupPaletteType;
						
					SetUpPalettePopUpMenu (NULL,
													popUpMenuHandle,
													format,
													ancillaryInfoFormat,
													numberClasses,
													classDescriptionH,
													colorTableOffset,
													displaySpecsPtr, 
													displaySpecsPtr->classGroupCode,
													paletteSelection);
	
					CheckMenuItem (popUpMenuHandle, paletteSelection, TRUE);
												
					selection = TrackControl (theControl, location, NIL);									
	
					CheckMenuItem (popUpMenuHandle, paletteSelection, FALSE);
					
					if (selection > 0)
						errCode = GetBevelButtonMenuValue (
											theControl, (MenuItemIndex*)&paletteSelection);
						
					}	// end "if (popUpMenuHandle != NULL)"
				
				if (errCode == noErr)
					{
					Boolean	paletteUpToDateFlag = true;							
					if (paletteSelection == kFalseColors)
						{
						if (FalseColorPaletteDialog ())
							paletteUpToDateFlag = false;
							
						else	// !FalseColorPaletteDialog ()
							paletteSelection = 0;
						
						}	// end "if (paletteSelection == kFalseColors)"

					if (paletteSelection > 0)
						{
						SetPaletteSpecification (displaySpecsPtr,
															displaySpecsPtr->classGroupCode,
															paletteSelection,
															paletteUpToDateFlag);
							
						UpdateActiveImageLegend (displaySpecsPtr->classGroupCode,
															kCallCreatePalette);
						
						}	// end "if (paletteSelection > 0)"
						
					}	// end "if (errCode == noErr)"
					
				HiliteControl (theControl, 0);
				GetControlBounds ((ControlRef)theControl, &gTempRect);
				InvalWindowRect (theWindow, &gTempRect);
            }
				break;
				
			case kCoordinateUnitsControl:
				CoordinateUnitsControlEvent (theControl, theWindow, location);
				break;
				
			case kAreaUnitsControl:
				AreaUnitsControlEvent (theControl, theWindow, location);
				break;
				
			default:
				break;
	    	
			}	// end "switch (refCon)" 
				
		HUnlock (displaySpecsH);

		}	// end of "if (refCon >= 0 & refCon <= 7)" 
	  
}	// end "ImageWControlEvent" 	



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Pascal void ImageWThumbScrollAction
//
//	Software purpose:	This routine handles the scroll action in 
//							TrackControl for the thumb scroll.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/27/1994
//	Revised By:			Larry L. Biehl			Date: 03/16/2018

// pascal void ImageWThumbScrollAction (void)
pascal void ImageWThumbScrollAction (
				ControlHandle						theControl, 
				SInt16								partCode)
				
{
			// Local Structures and Variables	

	Point									lastPoint,
											mousePoint;
	
	Rect									controlRect,
											destRect;	
	
	double								linesPerControlStep,
											magnification;	
	
	DisplaySpecsPtr					displaySpecsPtr;									
	
	Handle								displaySpecsH;
	
	SInt32								h,
											maxControlValue,
											origin,
											originalOrigin,
											refCon,
											scrollBarLength,
											scrollStep,
											titleHeight,
											v;
											
	UInt32								finalTicks;
										
	
			// Initialize local variables.									

	refCon = GetControlReference (gCurrentControlHandle);											
		
	titleHeight = ((WindowInfoPtr)*gActiveWindowInfoH)->titleHeight;
	
	lastPoint = gOldThumbScrollPoint;

			// Get handle and pointer to display specification for window		
		
	displaySpecsH = ((WindowInfoPtr)*gActiveWindowInfoH)->displaySpecsHandle;
	displaySpecsPtr = (DisplaySpecsPtr)*displaySpecsH;
	magnification = displaySpecsPtr->magnification;
	
	GetWindowClipRectangle (gActiveImageWindow, kImageFrameArea, &destRect);
	
			// Make sure that the title are is included if this is a horizontal
			// scroll
			
	if (refCon == kHorizontal)
		destRect.top -= titleHeight;
	
			// Get number of pixels to scroll window by and set					
			// new origin in the display specification
			
	maxControlValue = GetControl32BitMaximum (gCurrentControlHandle);
	
	GetControlBounds ((ControlRef)gCurrentControlHandle, &controlRect);
	
	if (refCon == kVertical)
		scrollBarLength = controlRect.bottom - controlRect.top - 3*kSBarWidth;
		
	else	// refCon == kHorizontal
		scrollBarLength = controlRect.right - controlRect.left - 3*kSBarWidth;
		
	linesPerControlStep = (double)maxControlValue/scrollBarLength;
	
	originalOrigin = displaySpecsPtr->origin[refCon];
	
	while (StillDown ()) 
		{				
				// Only attempt to update the value if the mouse moved.
				
		::GetMouse (&mousePoint);
		if (::EqualPt (mousePoint, lastPoint))
			continue;
								
				// Set up arguments for ScrollRect											
				// Be sure to handle situation where the title 							
				// should be scrolled left or right.										
				
		origin = originalOrigin;
		
		if (refCon == kVertical)
			{
			scrollStep = (SInt32)
							(linesPerControlStep * (mousePoint.v - gOldThumbScrollPoint.v));
			
			h = 0;
			v = ImageWThumbScrollStepValue (maxControlValue,
														scrollStep,
														originalOrigin,
														magnification,
														&origin);

			}	// end "if (refCon == kVertical)"
			
		else	// refCon == kHorizontal
			{  
			scrollStep = (SInt32)
							(linesPerControlStep * (mousePoint.h - gOldThumbScrollPoint.h));
			
			v = 0;
			h = ImageWThumbScrollStepValue (maxControlValue,
														scrollStep,
														originalOrigin,
														magnification,
														&origin);

			}	// end "if (refCon == kHorizontal)"
				
		displaySpecsPtr->origin[refCon] = origin;	
					
		SetControl32BitValue (gCurrentControlHandle, origin);
			
					// Scroll area if the mouse has moved far enough to cause a	
					// scroll operation.														
					
		if (h != 0 || v != 0)
			{
			ScrollRect (&destRect, (SInt16)-h, (SInt16)-v, gTempRegion1);
			if (QDIsPortBuffered (GetWindowPort (gActiveImageWindow)))
				QDFlushPortBuffer (GetWindowPort (gActiveImageWindow), NULL);
			Delay (1, &finalTicks);
	
					// Update the area that was made invalid by the scroll.
					
			CopyOffScreenImage (gActiveImageWindow, 
										gTempRegion1, 
										NIL, 
										3,
										kNotFromImageUpdate);
			
			}	// end "if (scrollHStep != 0 || scrollVStep != 0)" 
	
		SetEmptyRgn (gTempRegion1);
		SetEmptyRgn (gTempRegion2);
		
		lastPoint = mousePoint;
		
		}	// end while (StillDown ())
	  
}	// end "ImageWThumbScrollAction" 	



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt32 ImageWThumbScrollStepValue
//
//	Software purpose:	This routine determines the thumb scroll step value and the
//							new origin.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/09/2001
//	Revised By:			Larry L. Biehl			Date: 01/09/2001			

SInt32 ImageWThumbScrollStepValue (
				SInt32								maxControlValue,
				SInt32								scrollStep,
				SInt32								originalOrigin,
				double								magnification,
				SInt32*								newOriginPtr)
				
{
	SInt32								lastOrigin,
											origin,
											step;
	
					
	lastOrigin = GetControl32BitValue (gCurrentControlHandle);
	
	origin = originalOrigin + scrollStep;
											
	if (origin >= maxControlValue)
		{
				// Handle case when scroll is at bottom or right of scroll bar.
					
		origin = maxControlValue;
	
				// Now get the steps to move from the last scroll to this scroll.
			
		step = (SInt32)((maxControlValue - lastOrigin) * magnification);
		
		}	// end "if (origin + scrollHStep > maxControlValue)"
		
	else if (origin <= 0) 
		{ 
				// Handle case when scroll is at top or left of scroll bar.
						
		origin = 0;
	
				// Now get the steps to move from the last scroll to this scroll.
				
		step = (SInt32)(-lastOrigin * magnification);
		
		}	// end "else if (origin + scrollHStep <= 0)"
		
	else	// origin > 0
		{
		origin = originalOrigin + scrollStep;
	
				// Now get the steps to move from the last scroll to this scroll.
			
		step = (SInt32)((origin - lastOrigin) * magnification);
	
			// Now take into account any truncation.
			
		origin = (SInt32)(step/magnification + lastOrigin);
		
		}	// end "else origin + scrollHStep > 0"
	
	*newOriginPtr = origin;
	
	return (step);
	  
}	// end "ImageWThumbScrollStepValue"
	


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void ImageWScroll
//
//	Software purpose:	This routine handles the scroll action in 
//							TrackControl for up/down buttons page down/up areas.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			TrackControl
//
//	Coded By:			Larry L. Biehl			Date:  8/31/1988
//	Revised By:			Larry L. Biehl			Date:  3/19/1990			

pascal void ImageWScroll (
				ControlHandle						theControl, 
				SInt16								partCode)

{
	SInt16								refCon;
	
			
			// Make certain that at least 'gControlOffset' ticks have passed 		
			// since the last scroll operation.	This is need for very fast 		
			// systems where one click of the mouse may cause two loops through 	
			// this routine.	
			// If the command key is down, then do not need to wait
			// 'gControlOffset' ticks - zoom away.
	
	if (!(gEventRecord.modifiers & cmdKey))
		{				
				// Make certain that at least 'gControlOffset' ticks have passed 		
				// since the last zoom operation.	This is need for very fast 		
				// systems where one click of the mouse may cause two loops through 	
				// this routine.																		
																								
		if (TickCount () < gNextTime + gControlOffset)
																									return;
																							
		}	// end "if (!(gEventRecord.modifiers & cmdKey))"
	
	gNextTime = TickCount ();
	
			// Get the refCon of the control. This will contain a code for which 
			// control it is.																		
			// refCon = 0: Vertical scroll bar												
			// refCon = 1: Horizontal scroll bar											
	
	refCon = (SInt16)GetControlReference (theControl);
	
	ImageWindowScroll (theControl, refCon, partCode);
		
			// Do an event available to load into the gEventRecord the status	
			// of the option key.
			
	gEventRecord.modifiers = (UInt16)GetCurrentKeyModifiers ();
	
}	// end "ImageWScroll" 
	


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ImageWindowScroll
//
//	Software purpose:	This routine scrolls the image window as requested.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			TrackControl
//
//	Coded By:			Larry L. Biehl			Date:  6/03/1993
//	Revised By:			Larry L. Biehl			Date: 03/16/2018

void ImageWindowScroll (
				ControlHandle						theControl, 
				SInt16								scrollBarCode, 
				SInt16								partCode)

{
			// Local Structures and Variables												
			
	Rect									destRect;
	
	double								magnification;
	
	DisplaySpecsPtr					displaySpecsPtr;
	
	Handle								displaySpecsH;	
	
	SInt32								h,
											imageDimensions,
											origin,
											scrollStep,
											sign,
											step,
											titleHeight,
											v,
											windowDimensions;
											
	UInt32								finalTicks;
	
	
			// Return if mouse location has moved outside of control area or		
			// the scroll bar code does not make sense.									
			
	if (partCode == 0)
																									return;
			
	if (scrollBarCode < 0 | scrollBarCode > 1)												
																									return;
																							
			// Get image dimensions																
			
	displaySpecsH = ((WindowInfoPtr)*gActiveWindowInfoH)->displaySpecsHandle;
	displaySpecsPtr = (DisplaySpecsPtr)*displaySpecsH;
			
	magnification = displaySpecsPtr->magnification;
	origin = displaySpecsPtr->origin[scrollBarCode];
	
			// Set up rectangle to scroll														
			
	GetWindowClipRectangle (gTheActiveWindow, kImageFrameArea, &destRect);
	
			// Set up h and v	depending on whether the control is vertical or 	
			// horizontal																			
			
	if (scrollBarCode == kVertical)
		{
		h = 0;
		v = -1;
		
		windowDimensions = (SInt16)((destRect.bottom-destRect.top)/magnification);
		
		}	// end "if (scrollBarCode == kVertical)" 
	
	else	// scrollBarCode == kHorizontal 
		{
		h = -1;
		v = 0;
		
				// Include the title.
				
		titleHeight = ((WindowInfoPtr)*gActiveWindowInfoH)->titleHeight;
		destRect.top -= titleHeight;
		
		windowDimensions = (SInt16)((destRect.right-destRect.left)/magnification);
		
		}	// end "else scrollBarCode == kHorizontal" 
		
			// Get image dimensions																
			
	imageDimensions = (SInt32)displaySpecsPtr->imageDimensions[scrollBarCode];
	
	switch (partCode)
		{
		case kControlUpButtonPart:
			step = -10;
			sign = -1;
			break;
			
		case kControlDownButtonPart:
			step = 10;
			sign = 1;
			break;
			
		case kControlPageUpPart:
			step = -(SInt32)(windowDimensions * magnification * 0.75);
			sign = -1;
			break;
			
		case kControlPageDownPart:
			step = (SInt32)(windowDimensions * magnification * 0.75);
			sign = 1;
			break;
			
		default:
																						return;
			break;
																				
		}	// end "switch (partCode)" 
	
			// Determine scroll step size														
				
	scrollStep = (SInt32)(step/magnification);
	if (scrollStep == 0)  
		scrollStep = sign;
	
	scrollStep = MIN (scrollStep, 
							imageDimensions - windowDimensions - origin);
						
	if (origin + scrollStep < 0)  
		scrollStep = -origin;
		
	h *= scrollStep * magnification;
	v *= scrollStep * magnification;
			
	if (h != 0 || v != 0)
		{																			
		ScrollRect (&destRect, (SInt16)h, (SInt16)v, gTempRegion1);
		if (QDIsPortBuffered (GetWindowPort (gActiveImageWindow)))
			QDFlushPortBuffer (GetWindowPort (gActiveImageWindow), NULL);
		Delay (1, &finalTicks);
		
		origin += scrollStep;
		displaySpecsPtr->origin[scrollBarCode] = origin;
					
		SetControl32BitValue (theControl, origin);
		
				// Update the area that was made invalid by the scroll					
							
		CopyOffScreenImage (gTheActiveWindow, 
									gTempRegion1, 
									NIL, 
									kScrollCopy, 
									kNotFromImageUpdate);
		
		SetEmptyRgn (gTempRegion1);
		
		}	// end "if (h != 0 || v != 0)"
	
}	// end "ImageWindowScroll" 
	


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ImageWindowSelectionScroll
//
//	Software purpose:	This routine handles the scroll action caused during
//							an image window selection process when the user is
//							dragging the selection outside of the window boundary.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			PolygonSelection.c in selectionArea.c
//							RectangleSelection.c in selectionArea.c
//
//	Coded By:			Larry L. Biehl			Date: 06/03/1993
//	Revised By:			Larry L. Biehl			Date: 04/13/2001	

void ImageWindowSelectionScroll (
				Point									newPt,
				Rect*									viewRectPtr)
				
{
			// Local Structures and Variables												
	
	ControlHandle						theControl;
	SInt16								scrollButton;
	
	
	ClipRect (GetWindowPortBounds (gActiveImageWindow, &gTempRect));
	
			// Check vertical scroll.
		
	if (newPt.v < viewRectPtr->top || newPt.v > viewRectPtr->bottom)
		{	
		theControl = ((WindowInfoPtr)*gActiveImageWindowInfoH)->verticalControl;
			
		scrollButton = kControlDownButtonPart;
		if (newPt.v < viewRectPtr->top)
			scrollButton = kControlUpButtonPart;
				
		ImageWindowScroll (theControl, 0, scrollButton);
		
		}	// end "if (newPt.v < viewRectPtr->top || newPt.v > viewRectPtr->bottom)"
	
			// Check horizontal scroll.
		
	if (newPt.h < viewRectPtr->left || newPt.h > viewRectPtr->right)
		{	
		theControl = ((WindowInfoPtr)*gActiveImageWindowInfoH)->horizontalControl;
			
		scrollButton = kControlDownButtonPart;
		if (newPt.h < viewRectPtr->left)
			scrollButton = kControlUpButtonPart;
				
		ImageWindowScroll (theControl, 1, scrollButton);
		
		}	// end "if (newPt.h < viewRectPtr->left || newPt.h > viewRectPtr->right)"
		
	ClipRect (viewRectPtr);
	
}	// end "ImageWindowSelectionScroll" 
	


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void ImageZoomControlEvent
//
//	Software purpose:	This routine handles the zoom action in 
//							TrackControl for zoom-in and zoom-out control actions.
//							The while StillDown code was put in on 10/27/2001 because of a
//							change in the way that OSX handles control click action 
//							procedures. It only gets called when the mouse moves.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			ImageWControlEvent in controls.c
//
//	Coded By:			Larry L. Biehl			Date: 05/03/1990
//	Revised By:			Larry L. Biehl			Date: 02/06/2007			

pascal void ImageZoomControlEvent (
				ControlHandle						theControl, 
				SInt16								partCode)
				
{
	Point									mousePoint;
	Rect									controlRect;
	
	double								inverse,
											magnification,
											step;
	
	DisplaySpecsPtr					displaySpecsPtr;
	
	Handle								displaySpecsH;
	
	SInt16								refCon;
	
	Boolean								zoomFlag;
	
	
			// Return if mouse location has moved outside of control area.			
			
	if (partCode == 0)
																									return;
																							
	GetControlBounds (theControl, &controlRect);
																							
	while (StillDown ()) 
		{				
				// Only zoom if the mouse is still in the control.
				
		::GetMouse (&mousePoint);
		if (!PtInRect (mousePoint, &controlRect))
																									return;
	
				// If the command key is down, then do not need to wait
				// 'gControlOffset' ticks - zoom away.
		
		zoomFlag = TRUE;
		if (!(gEventRecord.modifiers & cmdKey))
			{				
					// Make certain that at least 'gControlOffset' ticks 
					// have passed since the last zoom operation.	This is 
					// need for very fast systems where one click of the 
					// mouse may cause two loops through this routine.																		
																									
			if (TickCount () < gNextTime + gControlOffset)
				zoomFlag = FALSE;
																								
			}	// end "if (!(gEventRecord.modifiers & cmdKey))"
			
		if (zoomFlag)
			{
			gNextTime = TickCount ();
																									
					// Initialize variables.															
					// NOTE:  displaySpecsPtr is already locked.									
					
			displaySpecsH = ((WindowInfoPtr)*gActiveWindowInfoH)->displaySpecsHandle;
			displaySpecsPtr = (DisplaySpecsPtr)*displaySpecsH;
					
			magnification = displaySpecsPtr->magnification;
			
			refCon = (SInt16)GetControlReference (theControl);
				
					// Continue only if magnification is still within the limits.			
					
			if ((magnification < gMaxMagnification && refCon == kZoomInControl) ||
						(magnification > gMinMagnification && refCon == kZoomOutControl))
				{
				inverse = 1./magnification;
			
						// Check if option key is down.												
				
				step = 0.1;
					
				if (!(gEventRecord.modifiers & optionKey))
					{
					step = 1.0;
					if (magnification > 1.)
						magnification = (double)((SInt16)(magnification + .5));
					if (inverse > 1.)  
						inverse = (double)((SInt16)(inverse + .5));
						
					}	// end "if (!(gEventRecord.modifiers & optionKey))" 
				
				if (refCon == kZoomInControl)
					{
					if (magnification >= 1.) 
						magnification = magnification + step;
					
					else if (inverse > step) 		// magnification < 1. 
						magnification = 1./(inverse - step);
						
					else
						magnification = 1.;
						
					}	// end "refCon == kZoomInControl" 
			
				if (refCon == kZoomOutControl)
					{
					if (magnification <= 1.)  
						magnification = 1./(inverse + step);
					
					else if (magnification > step)	// magnification <= 1. 
						magnification = magnification - step;
					
					else	// magnification <= 1. 
						magnification = 1.;
						
					}	// end "if (refCon == kZoomOutControl" 
					
				magnification = MAX (magnification, gMinMagnification);
				magnification = MIN (magnification, gMaxMagnification);
				
				gProcessorCode = kZoomButtonControl;
				DoImageZoom (gTheActiveWindow, displaySpecsPtr, magnification, FALSE);
				gProcessorCode = 0;
				
						// Do an event available to load into the gEventRecord the status	
						// of the option and command keys.
						
				gEventRecord.modifiers = (UInt16)GetCurrentKeyModifiers ();

				}	// end "if (magnification < gMaxMagnification && ..." 
				
			}	// end "if (zoomFlag)"
		
		}	// end "while (StillDown ())"

}	// end "ImageZoomControlEvent" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void OverlayControlEvent
//
//	Software purpose:	This routine handles the overlay control event.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			ImageWControlEvent in controls.c
//
//	Coded By:			Larry L. Biehl			Date: 01/21/2001
//	Revised By:			Larry L. Biehl			Date: 12/16/2016			

void OverlayControlEvent (
				ControlHandle						theControl, 
				WindowPtr							theWindow, 
				Point									location)
				
{	
	MenuHandle							popUpMenuHandle;
	
	//#ifdef AVAILABLE_MAC_OS_X_VERSION_10_5_AND_LATER
	//	MenuItemIndex						selection;
	//#else	// 
		SInt16								selection;
	//#endif
		
	SInt16 								errCode;
											
	Boolean								continueFlag,
											shiftKeyFlag,
											optionKeyFlag;
	
	
	errCode = noErr;
	
	optionKeyFlag = FALSE;
	if (gEventRecord.modifiers & optionKey)
		optionKeyFlag = TRUE;
		
	shiftKeyFlag = FALSE;
	if (gEventRecord.modifiers & shiftKey)
		shiftKeyFlag = TRUE;
		
	popUpMenuHandle = GetPopUpMenuHandle (theControl);
	if (popUpMenuHandle != NULL)
		{
		SetUpWindowOverlayPopUpMenu (popUpMenuHandle, optionKeyFlag);
	
		if (gAppearanceManagerFlag)
			{
			selection = TrackControl (theControl, location, NIL);
		
			if (selection > 0)
				errCode = GetBevelButtonMenuValue (theControl, (MenuItemIndex*)&selection);
				
			}	// end "if (gAppearanceManagerFlag)"
		
		else	// !gAppearanceManagerFlag
			{
			TrackControl (theControl, location, (ControlActionUPP) -1);	
			selection = GetControlValue (theControl);
			
			}	// end "else !gAppearanceManagerFlag"
		
		if (errCode == noErr && selection > 0)
			{
			continueFlag = TRUE;
			if (optionKeyFlag)
				continueFlag = OverlayControlDialog (gActiveImageWindowInfoH,
																	(SInt16)(selection-4));
			
			if (continueFlag)
				DoShowOverlaySelection (gActiveImageWindow, 
												gActiveImageWindowInfoH, 
												selection,
												optionKeyFlag,
												shiftKeyFlag);
												
			}	// end "if (errCode == noErr && selection > 0)"
											
				// Now delete the current menu items.
				
		ClearMenuItems (popUpMenuHandle, 3);
		
		}	// end "if (popUpMenuHandle != NULL)"
	  
}	// end "OverlayControlEvent" 
	

/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void OutputAutoScroll
//
//	Software purpose:	The purpose of this routine is to handle auto
//							scrolling in the output window.
//
//							NOTE.  Most of the code for this routine came from
//							THINK's CAPPS editor construction kit.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			DoMouseDnContent in multiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 1/24/1989
//	Revised By:			Larry L. Biehl			Date: 6/08/1990			

pascal void OutputAutoScroll (
				//WSHandle 							textH,
				WEReference							textH, 
				Point									mousePoint)

{
	//Rect* 								rect = &(*textH)->tRect;
	LongRect 							rect;
	
	WEGetViewRect (&rect, textH);
	
	if (gPresentCursor == kIBeam)
		{
		if (mousePoint.v < rect.top)
			OutputWScroll (((WindowInfoPtr)*gActiveWindowInfoH)->verticalControl, 
									kControlUpButtonPart);
			
		else if (mousePoint.v > rect.bottom)
			OutputWScroll (((WindowInfoPtr)*gActiveWindowInfoH)->verticalControl, 
									kControlDownButtonPart);
	
		if (mousePoint.h < rect.left) 
			OutputWScroll (((WindowInfoPtr)*gActiveWindowInfoH)->horizontalControl, 
									kControlUpButtonPart);
			
		else if (mousePoint.h > rect.right) 
			OutputWScroll (((WindowInfoPtr)*gActiveWindowInfoH)->horizontalControl, 
									kControlDownButtonPart);
				
		}	// end "if (gPresentCursor == kIBeam)" 
		
}	// end "OutputAutoScroll" 
*/


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void OutputWControlEvent
//
//	Software purpose:	This routine adjusts the content of the window based
//			 				on the actions requested by the scroll bar settings.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date:  3/17/1988
//	Revised By:			Larry L. Biehl			Date: 01/08/1991			

void OutputWControlEvent (
				ControlHandle						theControl, 
				WindowPtr							theWindow, 
				Point									location, 
				SInt16								partCode)

{
	/*
			// Local Structures and Variables													
		
	SInt16								endValue,
											initialValue;
	
	//WSHandle								textH;
	WEReference							textH;
	SInt32								dh,
											dv,
											refCon;
	
	
			// Get the handle to the text in the window. The handle is stored in 
			// in the refcon variable of the window record.  If the handle is		 
			// not 0 (i.e. there is text in this window) and the partcode is not	
			// NULL, scroll the text as requested otherwise exit the routine 		
			// and do nothing.																	

	textH = ((WindowInfoPtr)*gActiveWindowInfoH)->textH;
	
			// Get the refCon of the control. This will contain a code for  		
			// which control it is.																
			// refCon = 0: Vertical scroll bar												
			// refCon = 1: Horizontal scroll bar											
			
	refCon = -1;
	if (theControl != NULL)  
		refCon = GetControlReference (theControl);
	
	if (textH != NULL && refCon >= 0 && refCon <= 1 && partCode != 0)
		{
				// Save the current control value.											
				
		initialValue = GetControlValue (theControl);
	
		if (partCode == kControlIndicatorPart)
			{
			//partCode = TrackControl (theControl, location, NIL);
			partCode = TrackControl (theControl, location, gOutputWScrollActionPtr);
			
			if (partCode == kControlIndicatorPart)
				{
				endValue = GetControlValue (theControl);
				
						// Assume vertical scroll bar for now. 							
					
				dv = initialValue - endValue;
				dh = 0;
				
				if (refCon == 0)
					dv *= gOutputVScrollFactor;
				
				else	// refCon != 0 
					{
							// This is really the horizontal scroll bar.					
							
					dh = dv * gOutputHScrollFactor;
					dv = 0;
					
					}	// end "else refCon != 0" 
					
				//WSScroll (textH, dh, dv);
				WEScroll (dh, dv, textH);
				
						// Now update the scrolls, the text buffer format 				
						// information may have changed.										
						
				UpdateOutputWScrolls (theWindow, 0, kDisplayNoMessage);
				
				
				}	// end "if (partCode != 0)" 
				
																									return;
				
			}	// end "if (partCode == kControlIndicatorPart)" 
	
				// The click was in the button or page up/down of the scroll bar.	
		
		else	// partCode != kControlIndicatorPart 
			TrackControl (theControl, location, gOutputWScrollActionPtr);
			
		}	// end "if (textH != NULL && refCon >= 0 && refCon ..." 
	*/
	LongRect								viewRect;
	#if use_mlte_for_text_window
		TXNObject							textH;
	#endif
	#if !use_mlte_for_text_window
		WEReference							textH;
	#endif
	
	SInt32								pageSize,
											refCon,
											step = 0;
	
	
			// Get the handle to the text in the window. The handle is stored in 
			// in the refcon variable of the window record.  If the handle is		 
			// not 0 (i.e. there is text in this window) and the partcode is not	
			// NULL, scroll the text as requested otherwise exit the routine 		
			// and do nothing.																	

	textH = ((WindowInfoPtr)*gActiveWindowInfoH)->textH;
	if (textH == NULL)
																									return;
	
	#if !use_mlte_for_text_window
		WEGetViewRect (&viewRect, textH);
	#endif
	
			// Get the refCon of the control. This will contain a code for  		
			// which control it is.																
			// refCon = 0: Vertical scroll bar												
			// refCon = 1: Horizontal scroll bar	
			
	refCon = -1;
	if (theControl != NULL) 
		{ 
		if (((WindowInfoPtr)*gActiveWindowInfoH)->verticalControl == theControl)
			refCon = kVertical;
			
		if (((WindowInfoPtr)*gActiveWindowInfoH)->horizontalControl == theControl)
			refCon = kHorizontal;
			
		}	// end "if (theControl != NULL) "

			//	if any control was hit, it must be one of our two scroll bars:
			//	find out which and calculate the page size for it
			
	if (refCon == kVertical)
		pageSize = viewRect.bottom - viewRect.top ;
		
	else if (refCon == kHorizontal)
		pageSize = viewRect.right - viewRect.left ;
		
	else
				// return immediately if none of our scrollbars was hit
																									return;

			//	Calculate the "scroll step" according to the part hit
			
	switch (partCode)
		{
		case kControlUpButtonPart:
			step = - ((gEventRecord.modifiers & optionKey) ? 1 : kScrollDelta) ;
			break ;

		case kControlDownButtonPart:
			step = + ((gEventRecord.modifiers & optionKey) ? 1 : kScrollDelta) ;
			break ;

		case kControlPageUpPart:
			step = - (pageSize - kScrollDelta) ;
			break ;

		case kControlPageDownPart:
			step = + (pageSize - kScrollDelta) ;
			break ;
			
		}	// end "switch (partCode)"

			//	Save scroll step as a control property
			
	SetControlProperty (
						theControl, (UInt32)gCreator, kScrollStepTag, sizeof (step), &step);

			//	Track the mouse
			
	TrackControl (theControl, location, gOutputWScrollActionPtr);
				
}	// end "OutputWControlEvent" 
	


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void OutputWScroll
//
//	Software purpose:	The purpose of this routine is to handle scrolling
//							in the horizontal and vertical controls of the 
//							output window.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date:  1/24/1989
//	Revised By:			Larry L. Biehl			Date: 07/20/2001			

pascal void OutputWScroll (
				ControlHandle						theControl, 
				SInt16								partCode)

{
			// Local Structures and Variables.	
	/*			
	LongRect								weViewRect;											
	
	SInt16								maxCtlValue,
				  							oldCtlValue;
	
	SInt32							 	dH,
								 			dV,
											refCon;
	SInt16								scrollAmt;


			// Initialize the scroll offset variables.									
			
	dH = 0;
	dV = 0;

			// Determine which control it is.												

	refCon = (SInt32)GetControlReference (theControl);
	
			// Save old control value.															
		
	oldCtlValue = GetControlValue (theControl);
	
	WEGetViewRect (&weViewRect, gOutputTextH);

	if (refCon == 0)
		{
				// Vertical control.																
				// Calculate the amount to be scrolled.  If can't scroll in the	
				// given direction, then return, rather than flashing the arrow. 	
			
		switch (partCode)
			{
			case kControlUpButtonPart:
				dV = gOutputLineHeight * gOutputVScrollFactor;
				break;
				
			case kControlPageUpPart:
				//dV = (**gOutputTextH).tRect.bottom - (**gOutputTextH).tRect.top;
				dV = weViewRect.bottom - weViewRect.top;
				break;
	
			case kControlDownButtonPart:
				dV = -gOutputLineHeight * gOutputVScrollFactor;
				break;
				
			case kControlPageDownPart:
				//dV = (**gOutputTextH).tRect.top - (**gOutputTextH).tRect.bottom;
				dV = weViewRect.top - weViewRect.bottom;
				break;
	
			default:
				return;
				
			}	// end "switch (partCode)" 
			
		scrollAmt = -dV/gOutputVScrollFactor;
			
		}	// end "if (refcon == 0)" 

	else	//  refcon != 0 
		{
				// Horizontal control.															
				// Calculate the amount to be scrolled.  If can't scroll in the	
				// given direction, then return, rather than flashing the arrow. 	
			
		switch (partCode)
			{
			case kControlUpButtonPart:
				dH = kDefaultCharWidth * gOutputHScrollFactor;
				break;
				
			case kControlPageUpPart:
				//dH = (**gOutputTextH).tRect.right - (**gOutputTextH).tRect.left;
				dH = weViewRect.right - weViewRect.left;
				break;
	
			case kControlDownButtonPart:
				dH = -kDefaultCharWidth * gOutputHScrollFactor;
				break;
				
			case kControlPageDownPart:
				//dH = (**gOutputTextH).tRect.left - (**gOutputTextH).tRect.right;
				dH = weViewRect.left - weViewRect.right;
				break;
	
			default:
				return;
				
			}	// end "switch (partCode)" 
			
		scrollAmt = -dH/gOutputHScrollFactor;
			
		}	// end "else  refcon != 0" 
	
			// Pin the value to be between CtlMin and CtlMax.							
		
	if (scrollAmt <= 0)
		{
		if (oldCtlValue == 0)				
																									return;
		scrollAmt = MAX (0, (oldCtlValue + scrollAmt));
		
		}	// end "if (scrollAmt <= 0)" 
																			
	else	// scrollAmt > 0 
		{
		maxCtlValue = GetControlMaximum (theControl);
		if (oldCtlValue == maxCtlValue)								
																									return;
		scrollAmt = MIN (maxCtlValue, (oldCtlValue + scrollAmt));
		
		}	// end "else scrollAmt > 0" 

	scrollAmt = oldCtlValue - scrollAmt;
	
	if (refCon == 0)
		dV = scrollAmt * gOutputVScrollFactor;
		
	else	// refCon != 0 
		dH = scrollAmt * gOutputHScrollFactor;

	//WSScroll (gOutputTextH, dH, dV);
	WEScroll (dH, dV, gOutputTextH);
				
			// Now update the scrolls, the text buffer format 							
			// information may have changed.													
			
	UpdateOutputWScrolls ((*theControl)->contrlOwner, 0, kDisplayNoMessage);
	*/
	switch (partCode)
		{
		case kControlUpButtonPart:
		case kControlDownButtonPart:
		case kControlPageUpPart:
		case kControlPageDownPart:
			{
			SInt32		value = LCGetValue (theControl);
			SInt32		max = LCGetMax (theControl);
			SInt32		step = 0;
			UInt32		actualSize;

					//	Retrieve precalculated step value from scroll bar
					
			if (GetControlProperty (theControl, 
											(UInt32)gCreator, 
											kScrollStepTag, 
											sizeof (step),
											&actualSize, 
											&step) != noErr)
				{
																									return;
				
				}

					//	Move the scroll bar thumb by the precalculated step
					//	and scroll the text accordingly
					
			if (((value < max) && (step > 0)) || ((value > 0) && (step < 0)))
				{
				LCSetValue (theControl, value + step);
				OutputWScrollChanged (theControl);
				
				}	// end "if (((value < max) && (step > 0)) || ..."
				
			break;
			
			}	// end "case ..."

		case kControlIndicatorPart:
			{
			OutputWScrollChanged (theControl);
			break;
			
			}	// end "case kControlIndicatorPart:"
		
		}	// end "switch (partCode)"
	  
}	// end "OutputWScroll"  
	


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void OutputWScrollChanged
//
//	Software purpose:	The purpose of this routine is to scroll text to reflect new 
//							scroll bar setting.
//
//							From WEDemo routines.
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			????						Date: ??/??/??
//	Revised By:			Larry L. Biehl			Date: 07/20/2001	

void OutputWScrollChanged (
				ControlHandle						theControl)
				
{
	LongRect								destRect,
											viewRect;
											
	SInt32								refCon;
	
	SInt32							 	dH,
								 			dV;
											

	#if !use_mlte_for_text_window
		WEGetViewRect (&viewRect, gOutputTextH);
		WEGetDestRect (&destRect, gOutputTextH);
	#endif
	
			// Get the refCon of the control. This will contain a code for  		
			// which control it is.																
			// refCon = 0: Vertical scroll bar												
			// refCon = 1: Horizontal scroll bar											
	
	refCon = -1;		
	if (theControl != NULL) 
		{ 
		if (((WindowInfoPtr)*gActiveWindowInfoH)->verticalControl == theControl)
			refCon = kVertical;
			
		if (((WindowInfoPtr)*gActiveWindowInfoH)->horizontalControl == theControl)
			refCon = kHorizontal;
			
		}	// end "if (theControl != NULL) "

			//	if any control was hit, it must be one of our two scroll bars:
			//	find out which and calculate the page size for it
			
	dH = 0;
	dV = 0;
	if (refCon == kVertical)
		{
		SInt32  currentScrollValue = LCGetValue (theControl);
		dV = viewRect.top - destRect.top - currentScrollValue;
		
		}
		
	else if (refCon == kHorizontal)
		dH = viewRect.left - destRect.left - LCGetValue (theControl);

	#if !use_mlte_for_text_window
		WEScroll (dH, dV, gOutputTextH);
	#endif
	
}	// end "OutputWScrollChanged"


/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetOverlayControlHilite
//
//	Software purpose:	The purpose of this routine is to set the hilite for the overlay
//							control based on the number of overlays available for the window.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/01/2001
//	Revised By:			Larry L. Biehl			Date: 02/01/2001

void SetOverlayControlHilite (
				WindowPtr							theWindow,
				ControlHandle						controlHandle)

{
	Handle 								windowInfoHandle;
	
			
	windowInfoHandle = GetWindowInfoHandle (theWindow);
	
	if (windowInfoHandle != NULL)
		{
		if (((WindowInfoPtr)*windowInfoHandle)->numberOverlays > 0)
			HiliteControl (controlHandle, 0);
			
		else	// ...->numberOverlays == 0
			HiliteControl (controlHandle, 255);
			
		}	// end "if (windowInfoHandle != NULL)"
	  
}	// end "SetOverlayControlHilite" 
*/


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 StandardControlPopUpMenu
//
//	Software purpose:	The purpose of this routine is to handle the 'standard'
//							popup menu action when a mouse down occurs in a 
//							popup control.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:			ImageWControlEvent in controls.c
//
//	Coded By:			Larry L. Biehl			Date: 12/11/1996
//	Revised By:			Larry L. Biehl			Date: 07/23/2001

SInt16 StandardControlPopUpMenu (
				ControlHandle						theControl,
				MenuHandle							popUpMenu, 
				SInt16								currentSelection, 
				SInt16								popUpMenuID,
				Boolean								checkItemFlag)

{
	Point									popUpLoc;
	
	SInt16								menuData;


			// Get location of pop up box and convert it to global.					
	
	GetControlBounds ((ControlRef)theControl, &gTempRect);
	popUpLoc.v = (SInt16)(gTempRect.top + 1);
	popUpLoc.h = (SInt16)(gTempRect.right + 1);
	LocalToGlobal (&popUpLoc);
	
			// Insert the pop up menu and then let user make a selection.			
			
	InsertMenu (popUpMenu, -1);
	
			// Put check by current selection.												
	
	if (checkItemFlag)		
		CheckMenuItem (popUpMenu, currentSelection, true);
		
	menuData = (SInt16)PopUpMenuSelect (
											popUpMenu, popUpLoc.v, popUpLoc.h, currentSelection);
	
			// Remove the check by the "old" selection.									
			
	if (checkItemFlag)
		CheckMenuItem (popUpMenu, currentSelection, false);
		
	DeleteMenu (popUpMenuID);
	
			// Return the selection that was made.											
	
	return (LoWord (menuData));
			
}	// end "StandardControlPopUpMenu" 
	


#if !use_mlte_for_text_window
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void TextScrolled
//
//	Software purpose:	This is a callback routine called whenever the text is scrolled 
//							automatically. Since auto-scrolling is enabled, WEScroll may be 
//							invoked internally by WASTEin many different circumstances, and 
//							we want to be notified when this happens so we can adjust the 
//							scroll bars.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			WASTE engine
//
//	Coded By:			??							Date: ??/??/??
//	Revised By:			Larry L. Biehl			Date: 07/19/2001			

pascal void TextScrolled (
				WEReference 						we)
				
{
			//	Make sure the scroll bars are in synch with the destination rectangle
	
	UpdateOutputWScrolls (gOutputWindow, 0, kDisplayNoMessage);
			
}	// end "TextScrolled"
#endif	// !use_mlte_for_text_window
	


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void ThematicLegendControlEvent
//
//	Software purpose:	This routine handles scroll the thematic legend.
//							The while StillDown code was put in on 10/27/2001 because of a
//							change in the way that OSX handles control click action 
//							procedures.  It only gets called when the mouse moves.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			ImageWControlEvent in controls.c
//
//	Coded By:			Larry L. Biehl			Date: 11/07/1996
//	Revised By:			Larry L. Biehl			Date: 10/27/2001			

pascal void ThematicLegendControlEvent (
				ControlHandle						theControl, 
				SInt16								partCode)
				
{
	Point									mousePoint;
	Rect									controlRect;
	
	ListHandle 							legendListHandle;
	SInt16								refCon;
	
	Boolean								scrollFlag;
	
	
			// Return if mouse location has moved outside of control area.			
			
	if (partCode == 0)
																									return;
																							
	GetControlBounds (theControl, &controlRect);
																							
	while (StillDown ()) 
		{				
				// Only zoom if the mouse is still in the control.
				
		::GetMouse (&mousePoint);
		if (!PtInRect (mousePoint, &controlRect))
																									return;
	
			// If the command key is down, then do not need to wait
			// 'gControlOffset' ticks - scroll away.
		
		scrollFlag = TRUE;
		if (!(gEventRecord.modifiers & cmdKey))
			{		
					// Make certain that at least 'gControlOffset' ticks have passed 		
					// since the last zoom operation.	This is need for very fast 		
					// systems where one click of the mouse may cause two loops through 	
					// this routine.																		
																									
			if (TickCount () < gNextTime + gControlOffset)
				scrollFlag = FALSE;
																									
			}	// end "if (!(gEventRecord.modifiers & cmdKey))"
			
		if (scrollFlag)
			{
			gNextTime = TickCount ();
			
			legendListHandle = 
							((WindowInfoPtr)*gActiveImageWindowInfoH)->legendListHandle;
			
			refCon = (SInt16)GetControlReference (theControl);
				
			if (refCon == kLegendScrollUpControl)
				{
				if ((*legendListHandle)->visible.top > 
														(*legendListHandle)->dataBounds.top)
					{
					LScroll (0, -1, legendListHandle);
					
					}	// end "if ((*legendListHandle)->visible.top > ..."
				
				}	// end "if (refCon == kLegendScrollUpControl)"
				
			else	// refCon == kLegendScrollDownControl
				{
				if ((*legendListHandle)->visible.bottom < 
															(*legendListHandle)->dataBounds.bottom)
					{
					LScroll (0, 1, legendListHandle); 
					
					}	// end "if ((*legendListHandle)->visible.bottom < ..."
				
				}	// end "if (refCon == kLegendScrollUpControl)"
				
					// Do an event available to load into the gEventRecord the status	
					// of the command key.
					
			gEventRecord.modifiers = (UInt16)GetCurrentKeyModifiers ();
			
			}	// end "if (scrollFlag)"
			
		}	// end "while (StillDown ())"

}	// end "ThematicLegendControlEvent" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UnhiliteImageWControls
//
//	Software purpose:	This routine unhilites all controls in an image window.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			CopyOffScreenImage in multispec.c
//
//	Coded By:			Larry L. Biehl			Date: 12/10/1996
//	Revised By:			Larry L. Biehl			Date: 07/23/2001			

void UnhiliteImageWControls (
				WindowPtr							theWindow)

{
	ControlHandle						controlHandle,
											rootControlHandle;
	
	SInt16								refCon;
	
	UInt16								index,
											numberControls;
	
	
			// Check input parameters.															
			
	if (theWindow == NULL)
																									return;
	
			// Update zoom-in, zoom-out, and magnification display controls		

	if (gAppearanceManagerFlag)
		{
		GetRootControl (theWindow, &rootControlHandle);
		
		if (rootControlHandle != NULL)
			{
			CountSubControls (rootControlHandle, &numberControls);
			for (index=numberControls; index>=1; index--)
				{
				GetIndexedSubControl (rootControlHandle, index, &controlHandle);
				refCon = (SInt16)GetControlReference (controlHandle);
				if (refCon != kCoordinateViewControl)
					HiliteControl (controlHandle, 255);
		     			
		     	}	// end "for (index=numberControls; index>=1; index--)" 
		     	
			}	// end "if (rootControlHandle != NULL)"
	     	
		}	// end "if (gAppearanceManagerFlag)"
		     	
}	// end "UnhiliteImageWControls" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateImageWScrolls
//
//	Software purpose:	This routine updates the scroll bars for image
//							windows.  The routine is called after the image is
//							first drawn, the size of the window changes, and
//							the magnification changes.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			DoImageZoom in controls.c
//							UpdateScrolls in controls.c
//							DisplayMultispectralImage in displayMultiSpectral.c
//							DisplayThematicImage in displayThematic.c
//
//	Coded By:			Larry L. Biehl			Date:  8/31/1988
//	Revised By:			Larry L. Biehl			Date: 03/16/2018

void UpdateImageWScrolls (
				WindowPtr							theWindow)

{
	Rect									rectLessScrolls;
	
	double								magnification;
	
	ControlHandle						controlHHandl,
											controlVHandl;
	
	Handle								displaySpecsH,
											windowInfoHandle;

	SInt32								hOrigin,
											imageColumns,
											imageLines,
											nHOrigin,
											nVOrigin,
											vOrigin,
											windowColumns,
											windowLines;
	
	
			// Initialize local variables.													
			
	displaySpecsH = NULL;
	windowInfoHandle = NULL;
	controlVHandl = NULL;
	controlHHandl = NULL;
	
			// Get handle to window information structure.								
	
	if (theWindow != NULL) 
		windowInfoHandle = (Handle)GetWRefCon (theWindow);
	
			// Get handle to display specification for window							
	
	if (windowInfoHandle != NULL)	
		displaySpecsH = ((WindowInfoPtr)*windowInfoHandle)->displaySpecsHandle;
	
			// Continue with this routine only if the display specification		
			// handle is not NULL.																
			
	if (displaySpecsH != NULL)
		{	
				// Get horizontal and vertical scroll bar control handles	
			
		GetHorAndVerControlHandles (theWindow, &controlHHandl, &controlVHandl);	
		
		}	// end "if (displaySpecsH != NULL)" 
     		
	if (controlVHandl != NULL && controlHHandl != NULL)
     	{										
				// Save the current clip region.												
				
		GetClip (gTempRegion1);
     	
		magnification = ((DisplaySpecsPtr)*displaySpecsH)->magnification;
			
				// Set clip region of the window to include the scroll bars so		
				// that the thumb on the scroll can be moved.							
		
		GetWindowClipRectangle (theWindow, kImageScrollArea, &rectLessScrolls);
		ClipRect (&rectLessScrolls);
		
				// Get window rectangle less the scroll bars								
		
		GetWindowClipRectangle (theWindow, kImageFrameArea, &rectLessScrolls);
		
				// Update the vertical scroll.												
				// Get number of image lines in off screen bit/pix map.				
					
		imageLines = 
					(SInt32)((DisplaySpecsPtr)*displaySpecsH)->imageDimensions[kVertical];
			
				// Determine number of lines in image window.							
				
		windowLines = rectLessScrolls.bottom - rectLessScrolls.top;
		
				// Set the vertical scroll bar maximum value.							
			
		nVOrigin = (SInt32)(imageLines - windowLines/magnification);
		
		SetControl32BitMaximum (controlVHandl, nVOrigin);
		
				// Determine whether vertical scroll bar should be hilighted.		
		
		vOrigin = ((DisplaySpecsPtr)*displaySpecsH)->origin[kVertical];
		if (nVOrigin <= vOrigin)
			{
			if (nVOrigin < 0)	
				nVOrigin = 0;
			if (nVOrigin != vOrigin)	
				InvalWindowRect (theWindow, &rectLessScrolls);
			if (nVOrigin == 0)
				{
				HiliteControl (controlVHandl, 255);
		
				SetControl32BitMaximum (controlVHandl, 0);
				
				}	// end "if (nVOrigin == 0)" 
				
			((DisplaySpecsPtr)*displaySpecsH)->origin[kVertical] = nVOrigin;
		
			SetControl32BitValue (controlVHandl, nVOrigin);
			
			}	// end "if (nVOrigin <= vOrigin)" 
			
		if (nVOrigin > 0)   
			HiliteControl (controlVHandl, 0);
		
		if (nVOrigin > vOrigin) 
			{  
			SetControl32BitValue (controlVHandl, vOrigin);
				
			}	// end "if (nVOrigin > vOrigin) "
		
				// Update the horizontal scroll												
				// Get number of image columns in off screen bit/pix map				
					
		imageColumns = 
				(SInt32)((DisplaySpecsPtr)*displaySpecsH)->imageDimensions[kHorizontal];
			
				// Determine number of columns in image window							
				
		windowColumns = rectLessScrolls.right - rectLessScrolls.left;
		
				// Set the horizontal scroll bar maximum value.							
			
		nHOrigin = (SInt32)(imageColumns - windowColumns/magnification);
		
		SetControl32BitMaximum (controlHHandl, nHOrigin);
		
				// Determine whether horizontal scroll bar should be hilighted		
		
		hOrigin = ((DisplaySpecsPtr)*displaySpecsH)->origin[kHorizontal];
		if (nHOrigin <= hOrigin)
			{
			if (nHOrigin < 0)	
				nHOrigin = 0;
			if (nHOrigin != hOrigin)	
				InvalWindowRect (theWindow, &rectLessScrolls);
			if (nHOrigin == 0)
				{
				HiliteControl (controlHHandl, 255);
		
				SetControl32BitMaximum (controlHHandl, 0);
				
				}	// end "if (nHOrigin <= hOrigin)" 
				
			((DisplaySpecsPtr)*displaySpecsH)->origin[kHorizontal] = nHOrigin;
			SetControl32BitValue (controlHHandl, nHOrigin);
			
			}	// end "if (nHOrigin <= hOrigin)" 
			
		if (nHOrigin > 0)   
			HiliteControl (controlHHandl, 0);
		
		if (nHOrigin > hOrigin)   
			{  
			SetControl32BitValue (controlHHandl, hOrigin);
				
			}	// end "if (nVOrigin > vOrigin) "
				
				// Update Grow Icon.																
		
		DrawGrowIcon (theWindow);
																							
				// Reset the clip region back to that upon entry to this routine.	
				
		SetClip (gTempRegion1);
		SetEmptyRgn (gTempRegion1);				
			
		}	// end "if (controlVHandl != NULL && controlHHandl != NULL)" 
		
}	// end "UpdateImageWScrolls" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateImageZoomControls
//
//	Software purpose:	This routine updates the zoom controls for the
//							image window.  The routine is called after the 
//							image is first drawn, and when the magnification 
//							changes.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			DoImageZoom in controls.c
//							ImageWControlEvent in controls.c
//							DoImageWActivateEvent in multiSpec.c
//							DisplayMultispectralImage in SDisMulc.cpp
//							DisplayColorImage in SDisplay.cpp
//							DisplayThematicImage in SDisThem.cpp
// 						
//	Coded By:			Larry L. Biehl			Date: 12/02/1988
//	Revised By:			Larry L. Biehl			Date: 07/24/2001			

void UpdateImageZoomControls (
				WindowPtr							theWindow, 
				double								magnification,
				Boolean								updateZoomsFlag)

{
	WindowInfoPtr						windowInfoPtr;
											
	Handle								windowInfoHandle;
											
	SInt16								fieldPrecision;

	SignedByte							handleStatus;
	
	
			// Check input parameters.															
			
	if (theWindow == NULL)
																									return;
																							
			// Save the current clip region.													
			
	GetClip (gTempRegion1);				
			
			// Set clip region of the window to include the scroll bars so			
			// that the controls can be changed.											
				
	ClipRect (GetWindowPortBounds (theWindow, &gTempRect));
			
   		// Get pointer to window info structure.
   
   windowInfoHandle = GetWindowInfoHandle (theWindow);
	windowInfoPtr = (WindowInfoPtr)GetHandleStatusAndPointer (windowInfoHandle,
																					&handleStatus);
	
			// Update zoom-in, zoom-out, and magnification display controls
																	
	if (windowInfoPtr != NULL)
		{
				// CoordinateViewControl
			
		HiliteControl (windowInfoPtr->coordinateViewControl, 0);
				
				// ZoomInControl and ZoomOutControl
				
		if (updateZoomsFlag)
			{
					// Hilite the zoom-in control appropriately.					
	
			if (magnification < gMaxMagnification && gActiveOffscreenMapExists) 	
				HiliteControl (windowInfoPtr->zoomInControl, 0);
			
			else	// magnification >= gMaxMagnification || 
				HiliteControl (windowInfoPtr->zoomInControl, 255);
			
					// Hilite the zoom-out control appropriately.				
			
			if (magnification > gMinMagnification && gActiveOffscreenMapExists) 		
				HiliteControl (windowInfoPtr->zoomOutControl, 0);
			
			else	// magnification <= gMinMagnification || ...
				HiliteControl (windowInfoPtr->zoomOutControl, 255);
				
			}	// end "if (updateZoomsFlag)" 
				
					// MagControl. Put magnification in the mag display control.				
				
		fieldPrecision = 1;
		if (magnification < 1)
			fieldPrecision = 3;
		/*	
		std::sprintf ((char*)&gTextString, 
								"X%4.*f", 
								fieldPrecision, 
								magnification);
		*/	
		sprintf ((char*)&gTextString, "X%4.*f", 
													fieldPrecision, magnification);
													
		if (magnification < 1)
			//std::sprintf ((char*)&gTextString, "X%s", (char*)&gTextString[2]);
			sprintf ((char*)&gTextString, "X%s", (char*)&gTextString[2]);
													
		CopyCStringToPascal ((char*)&gTextString, gTextString);
		SetControlTitle (windowInfoPtr->magnificationControl, 
								(ConstStr255Param)&gTextString);
				
		if (magnification != 1. && gActiveOffscreenMapExists)	
			HiliteControl (windowInfoPtr->magnificationControl, 0);
			
		else	// magnification == 1. || ...
			HiliteControl (windowInfoPtr->magnificationControl, 255);
				
				// OverlayControl
			
		HiliteControl (windowInfoPtr->overlayControl, 0);
		//SetOverlayControlHilite (theWindow, controlHandle);

		MHSetState (windowInfoHandle, handleStatus);
		
		}	// end "if (windowInfoPtr != NULL)"
     	
   		// Update the scale factor in the coordinate view if displayed.
   		
   UpdateScaleInformation (gActiveWindowInfoH);
																							
			// Reset the clip region back to that upon entry to this routine.		
			
	SetClip (gTempRegion1);
	SetEmptyRgn (gTempRegion1);			
     	
}	// end "UpdateImageZoomControls" 
	


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateOutputWScrolls
//
//	Software purpose:	The purpose of this routine is to adjust the 
//							scroll bars in the output window after the
//							window has been resized or anything was printed
//							that may have altered the number of lines on the
//							page and, therefore, the number of lines that aren't 
//							visible.
//
// 						This optionally scrolls to start of selection, 
//							depending on the value of scrollToSelStart.
//
//							NOTE.  Some of the code for this routine came from
//							THINK's CAPPS editor construction kit.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 01/25/1989
//	Revised By:			Larry L. Biehl			Date: 12/18/2008		

void UpdateOutputWScrolls (
				WindowPtr							theWindow, 
				SInt16								scrollToSelStart,
				Boolean								displayMessageFlag)

{	
	SInt16								savedTextMemoryShortCode;
	
	
	if (theWindow == NULL)
																									return;
	
	savedTextMemoryShortCode = gTextMemoryShortCode;
	
	#if !use_mlte_for_text_window
		LongRect								weDestRect,
												weViewRect;
		
		GrafPtr								savedPort;
		WindowInfoPtr						windowInfoPtr;
		
		Handle								windowInfoHandle;
		
		//WEViewReference					weViewReference;
		
		SInt32								max,
												total,
												value,
												visible;
		
		SignedByte							handleStatus;
		
		Boolean								okay,
												rebuildFlag;
												
				// Display a message that the output text window is being 				
				// updated in the status dialog box if it is open.							
		
		if (displayMessageFlag)			
			LoadDItemStringNumber (kDialogStrID, 
											10, 
											gStatusDialogPtr, 
											gStatusTitleItemNumber, 
											&gTextString);
										
				// Initialize local variables.													
		
		windowInfoHandle = NULL;
		windowInfoPtr = NULL;
		
				// Get handle to window information and check the window type.  Call
				// the appropriate scroll update routine depending on window type.	
		
		if	(theWindow != NULL)  
			windowInfoHandle = (Handle)GetWRefCon (theWindow);
			
				// Get pointer to window information structure.								
				
		if (windowInfoHandle != NULL)
			{
			handleStatus = HGetState (windowInfoHandle);
			HLock (windowInfoHandle);
			windowInfoPtr = (WindowInfoPtr)*windowInfoHandle;
			
			}	// end "if (windowInfoHandle != NULL)" 
			
		if (windowInfoPtr->textH != NULL)
			{
					// Save the current port and make certain that 'theWindow' is the	
					// active port.																	
					
			GetPort (&savedPort);
			SetPortWindowPort (theWindow);
			
			//weViewReference = WEGetCurrentView (windowInfoPtr->textH);
			//WESetCurrentView (weViewReference);
			
			GetClip (gTempRegion1);
				
					// Set clip region of the window to include the scroll bars so		
					// that the thumb on the scroll can be moved.							
						
			ClipRect (GetWindowPortBounds (theWindow, &gTempRect));
			
			okay = false;
				
				// Update the view rectangle for the text.								
				
			CalcTextRect (gOutputWindow, &weViewRect);
			WESetViewRect (&weViewRect, windowInfoPtr->textH);
		
					// Get the destination rectangle.
		
			WEGetDestRect (&weDestRect, windowInfoPtr->textH);
			
			rebuildFlag = false;

					//	Calculate the new scroll bar settings
					// 	First do the vertical scroll bar

					//	NOTE:  (destRect.bottom - destRect.top) always equals the total text 
					// height because WASTE automatically updates destRect.bottom whenever 
					// line breaks are recalculated

			total = weDestRect.bottom - weDestRect.top;		//	total pixel height
			visible = weViewRect.bottom - weViewRect.top;	//	visible pixel height
			max = total - visible;								//	scrollable range (in pixels)
			value = weViewRect.top - weDestRect.top;			//	thumb location within  
																			// 	scrollable range.
			value = max;												//	thumb location within  
																			// 	scrollable range.

					//	Make sure max is always non-negative
				
			if (max <= 0) 
				max = 0 ;

					//	Notify SmartScroll
			
			//SetSmartScrollInfo (bar, visible, total);

					//	Reset the vertical scroll bar
					
			LCSetMax (windowInfoPtr->verticalControl, max);
			LCSetValue (windowInfoPtr->verticalControl, value);

					//	Now do the horizontal axis
					//		Calculate new scroll bar settings
					
			//total = weDestRect.right - weDestRect.left;	//	total pixel width
			total = gMaxCharsInLine*kDefaultCharWidth;		//	total pixel width
			visible = weViewRect.right - weViewRect.left;	//	visible pixel width
			max = total - visible;								//	scrollable range (in pixels)
			value = weViewRect.left - weDestRect.left;		//	thumb location within 
																			// 	scrollable range

					//	Make sure max is always non-negative
					
			if (max <= 0) 
				max = 0;

					//	Notify SmartScroll
					
			//SetSmartScrollInfo (bar, visible, total);

					//	Reset the horizontal scroll bar
					
			LCSetMax (windowInfoPtr->horizontalControl, max);
			LCSetValue (windowInfoPtr->horizontalControl, value);
		
					// Do scrolling to selection start here if requested.					
		
			if (scrollToSelStart)
				{
						// Scroll text to reflect new scroll bar setting
						
				SInt32 horizontalScrollSetting = weViewRect.left - weDestRect.left - 
													LCGetValue (windowInfoPtr->horizontalControl);
						
				SInt32 verticalScrollSetting = weViewRect.top - weDestRect.top - 
													LCGetValue (windowInfoPtr->verticalControl);

				WEScroll (horizontalScrollSetting,
								verticalScrollSetting,
								windowInfoPtr->textH);
					
						// Check if any of output text window needs to be invalidated	
						// to force the window to be written into.							
				
				//if (gViewRect.top < gViewRect.bottom)
				//	InvalRect (&gViewRect);
				
				}	// end "if (scrollToSelStart)" 
				
			//ShowControl (windowInfoPtr->horizontalControl);
			//ShowControl (windowInfoPtr->verticalControl);
			HiliteControl (windowInfoPtr->horizontalControl, 0);
			HiliteControl (windowInfoPtr->verticalControl, 0);
				
					// Reset the clip region back to that upon entry to this routine.	
					
			SetClip (gTempRegion1);
			/*
			shortViewRect.top = weViewRect.top;
			shortViewRect.left = weViewRect.left;
			shortViewRect.bottom = weViewRect.bottom;
			shortViewRect.right = weViewRect.right;
			ClipRect (&shortViewRect);
			*/
			SetEmptyRgn (gTempRegion1);				
			
			SetPort (savedPort);
																			
			}	// end "if (windowInfoPtr->textH != NULL)" 
			
				// Make sure that the text structure is unlocked.							
				
		//CheckAndUnlockHandle ((Handle)windowInfoPtr->textH);
		
		HSetState (windowInfoHandle, handleStatus);
	#endif	// !use_mlte_for_text_window
	
	#if use_mlte_for_text_window
		/*
		WindowInfoPtr						windowInfoPtr = NULL;	
		Handle								windowInfoHandle = NULL;
	
				// Get handle to window information and check the window type.  Call
				// the appropriate scroll update routine depending on window type.	
		
		if	(theWindow != NULL)  
			windowInfoHandle = (Handle)GetWRefCon (theWindow);
			
				// Get pointer to window information structure.								
				
		if (windowInfoHandle != NULL)
			windowInfoPtr = (WindowInfoPtr)*windowInfoHandle;
			
		if (windowInfoPtr != NULL && windowInfoPtr->textH != NULL)
			TXNUpdate (windowInfoPtr->textH);
		*/			
		TXNRecalcTextLayout (gOutputTextH);
		TXNDrawObject (gOutputTextH, NULL, kTXNDrawItemTextMask);
		TXNForceUpdate (gOutputTextH);
	#endif	// use_mlte_for_text_window
		
			// Indicate that memory was short for updating the text window if		
			// memory was found to be short during this routine.  This is being	
			// so that we do not get into an infinite loop with the output text	
			// window being updated after a memory short alert when the window	
			// update itself is what is causing the memory short alert.				
			
	if (gTextMemoryShortCode != savedTextMemoryShortCode)
		gTextMemoryShortCode++;
		
			// Force update of the file and edit menu items.
			
	gUpdateFileMenuItemsFlag = TRUE;
	gUpdateEditMenuItemsFlag = TRUE;
				
}	// end "UpdateOutputWScrolls" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateOverlayControl
//
//	Software purpose:	This routine updates the overlay control for the
//							image window.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			CloseOverlayFile in SArcView.cpp
//							CloseImageOverlayFile in SImageOverlays.cpp
//							CheckImageHeader in SOpnImag.cpp
// 						
//	Coded By:			Larry L. Biehl			Date: 02/01/2001
//	Revised By:			Larry L. Biehl			Date: 01/10/2003			

void UpdateOverlayControl (
				WindowPtr							theWindow)

{
	ControlHandle						controlHandle;
	Handle								windowInfoHandle;
	
	UInt32								totalNumberImageOverlays;
	
	Boolean								visibleFlag;
	
											
	windowInfoHandle = GetWindowInfoHandle (theWindow);
	if (windowInfoHandle == NULL)
																									return;
															
	controlHandle = ((WindowInfoPtr)*windowInfoHandle)->overlayControl;								
	if (controlHandle != NULL)
		{
		totalNumberImageOverlays = 
						((WindowInfoPtr)*windowInfoHandle)->numberOverlays +
									((WindowInfoPtr)*windowInfoHandle)->numberImageOverlays;
		
		if (gAppearanceManagerFlag)
			visibleFlag = IsControlVisible (controlHandle);
							
		if ((totalNumberImageOverlays == 0 && visibleFlag) || 
				(totalNumberImageOverlays > 0 && !visibleFlag))
			{
					// Redraw the scroll bars.
					 
			DrawScrollBars (theWindow);
			
			}	// end "if ((totalNumberImageOverlays == 0 && ..."
			
		}	// end "if (controlHandle != NULL)"
     	
}	// end "UpdateOverlayControl" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateScrolls
//
//	Software purpose:	This routine controls updating the scroll bars for
//							image and output windows.  The routine is called 
//							after any change is made in the content of the
//							window.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/25/1989
//	Revised By:			Larry L. Biehl			Date: 07/20/2011			

void UpdateScrolls (
				WindowPtr							theWindow)

{
	Handle								windowInfoHandle;
	
	SInt16								windowType;
	
	
			// Get handle to window information and check the window type.  Call
			// the appropriate scroll update routine depending on window type.	
	
	if	(theWindow != NULL)
		{
		windowInfoHandle = (Handle)GetWRefCon (theWindow);
		
		if (windowInfoHandle != NULL)
			{
			windowType = ((WindowInfoPtr)*windowInfoHandle)->windowType;
	
			if (windowType == kImageWindowType || windowType == kThematicWindowType) 
				{
				UpdateImageWScrolls (theWindow);
				
				if (windowType == kThematicWindowType) 
					UpdateThematicLegendControls (theWindow);
					
				}	// end "if (windowType == kImageWindowType || ..."
	
			else if (windowType == kOutputWindowType)  
				UpdateOutputWScrolls (theWindow, 0, kDisplayNoMessage);
			
			}	// end "if (windowInfoHandle != NULL)" 
			
		}	// end "if	(theWindow != NULL)" 
	
}	// end "UpdateScrolls" 

//enum
//	{
//	kEOL							=	0x0D
//	};


#if !use_mlte_for_text_window
#if TARGET_RT_MAC_MACHO
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean InitializeWordSolutionEngine
//
//	Software purpose:	This routine initializes the word solution engine
//							software that is used in the output text window.
//							This code is not included in the 'InitializeMultiSpec'
//							routine because it framents the heap.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/13/1991
//	Revised By:			Larry L. Biehl			Date: 01/08/2008			

Boolean InitializeWordSolutionEngine (void)

{
	PageMarginRec						pageMargins;
	LongRect								viewRect;
	Rect 									portRect;
	
	Handle								hPageMargins = NULL;
	
	OSStatus								errorReturn;
	
	
	#ifdef powerc
	
				// Load in word solution engine code resources for the power pc	
		/*	
		wsBridgeHandle = (JmpHandle)Get1Resource ('wseb', 128);
		if (wsBridgeHandle == NULL)
																							return (FALSE);
													
		MoveHHi ((Handle)wsBridgeHandle);		
		HLock ((Handle)wsBridgeHandle);
		
		gWSEBridgePtr = NewRoutineDescriptor (
							(ProcPtr)*wsBridgeHandle, uppWSEProcInfo, kM68kISA);
													
		if (gWSEBridgePtr == NULL)
																							return (FALSE);
		*/
	#endif
	
			// Initialize DataPak's Word Solution Engine.								
				
	//WSStartup (128, (ProcPtr)NULL);
	
	#ifdef powerc
		/*
		UniversalProcPtr errProccPtr = NewRoutineDescriptor (
					(ProcPtr)MyWSErrorProc, 
					kPascalStackBased
		 					| RESULT_SIZE (SIZE_CODE (sizeof (Boolean)))
							| STACK_ROUTINE_PARAMETER (1, SIZE_CODE (sizeof (ProcPtr))), 
					kPowerPCISA);
							
		WSSetErrProc ((ProcPtr)errProccPtr);
		*/
	#endif
	
	#ifndef powerc
		//WSSetErrProc ((ProcPtr)MyWSErrorProc);
	#endif
		
		gOutputAutoScrollActionPtr = NewWEScrollUPP (TextScrolled);									
		
	if (gOutputAutoScrollActionPtr == NULL)
																							return (FALSE);
																				
	gWENoWrapLineBreakProcPtr = NewWELineBreakUPP (_WENoWrapLineBreak);
	
	
			// Resset routine to handle memory shortage.									
	
	SetGrowZone ((GrowZoneUPP)MyGrowZone);
	
			// Get a copy of the globals and set the minimum memory variable		
			// needed to do a text operation.  For now we will specify the			
			// maximum of 7 times the maximum text buffer size and 12000 bytes.	
	
	gTextMemoryMinimum = 0;		// For WASTE
	gTextMemoryMinimum = MAX (gTextMemoryMinimum, 12000);
	
	GetWindowPortBounds (gOutputWindow, &portRect);

	viewRect.top = 3;
	viewRect.left = 3;
	viewRect.bottom = portRect.bottom - kSBarWidth - 3;
	viewRect.right = portRect.right - kSBarWidth - 3;
	
	SetPortWindowPort (gOutputWindow);
	if (WENew (&viewRect, &viewRect, weDoAutoScroll |
												 	weDoOutlineHilite |
									 				weDoUndo |
									 				weDoMultipleUndo |
													weDoIntCutAndPaste |
													weDoDragAndDrop |
													weDoUseTempMem |
													weDoDrawOffscreen, &gOutputTextH))
																							return (FALSE);

			// If gOutputTextH is NULL, then there isn't enough memory.	Clean				
			// up at end and exit the routine if there isn't enough memory.		
		
	if (gOutputTextH == NULL)		
																							return (FALSE);

			//	Save a reference to the window in the WE instance
			
	errorReturn = WESetUserInfo (kWindowTag, 
											(SInt32)gOutputWindow, 
											gOutputTextH);
	if (errorReturn != noErr)
																							return (FALSE);

			//	Set up our scroll callback
			
	errorReturn = WESetInfo (weScrollProc, 
										&gOutputAutoScrollActionPtr, 
										gOutputTextH);
	if (errorReturn != noErr)
																							return (FALSE);

			//	Associate a default page margins record with the WE reference
			
	pageMargins.top = 72 << 16 ;			//	one inch
	pageMargins.bottom = 72 << 16 ;
	pageMargins.left = 72 << 16 ;
	pageMargins.right = 72 << 16 ;
	
	errorReturn = PtrToHand (&pageMargins, &hPageMargins, sizeof (pageMargins));
	if (errorReturn != noErr)
																							return (FALSE);
	
	errorReturn = WESetUserInfo (kPageMarginsTag, 
											(SInt32)hPageMargins, 
											gOutputTextH);
	if (errorReturn != noErr)
																							return (FALSE);
	hPageMargins = NULL;

	errorReturn = WESetInfo (weLineBreakHook, 
										&gWENoWrapLineBreakProcPtr, 
										gOutputTextH);
	if (errorReturn != noErr)
																							return (FALSE);

	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)*gActiveWindowInfoH;
	windowInfoPtr->textH  = gOutputTextH;
	
	return (TRUE);
		
}	// end "InitializeWordSolutionEngine" 
#endif	// TARGET_RT_MAC_MACHO


pascal StyledLineBreakCode _WENoWrapLineBreak (
				const char *						inTextPtr,
				SInt32								inTextLength,
				SInt32								inTextStart,
				SInt32								inTextEnd,
				Fixed *								ioTextWidth,
				SInt32 *								ioTextOffset,
				WEReference							inWE)
										
{
	#pragma unused (inTextLength, ioTextWidth, inWE)

	SInt32								ii;
	
	
	StyledLineBreakCode breakCode = smBreakOverflow;

	for (ii=inTextStart; ii<inTextEnd; ii++)
		{
		if (inTextPtr[ii] == kCarriageReturn)
			{
			breakCode = smBreakWord;
			*ioTextOffset = ii + 1;
			break;
			
			}	// end "if (inTextPtr[ii] == kCarriageReturn)"
			
		}	// end "for (ii=inTextStart; ii<inTextEnd; ii++)"

	if (breakCode == smBreakOverflow)
		{
		*ioTextOffset = ii;
		
		}	// end "if (breakCode == smBreakOverflow)"

	return breakCode ;
}
#endif	// !use_mlte_for_text_window
