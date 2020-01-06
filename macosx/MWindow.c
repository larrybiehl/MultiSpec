//	 										MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						MWindow.c
//
//	Authors:					Larry L. Biehl
//
//	Revision number:		3.0
//
//	Revision date:			03/19/2018
//
//	Language:				C
//
//	System:					Linux, Macintosh Operating System
//
//	Brief description:	This file contains functions that operate on image and
//								output windows.
//
//	Functions in file:	void 			ChangeLegendWidth
//								void 			CloseGraphicsWindow
//								Boolean 		CloseImageWindow
//								void 			CloseOutputWindow
//								void 			ClosePaletteWindow
//								void 			CreateImageWControls
//								WindowPtr 	CreateImageWindow
//								WindowPtr 	CreatePaletteWindow
//								Boolean 		CreateThematicLegendControls
//								void			DisposeOfImageWindowSupportMemory
//								void 			DrawScrollBars
//								void 			DrawThematicLegendControls
//								Handle 		GetImageWindowRecord
//								void 			ReleaseWindowRecord
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//	
//------------------------------------------------------------------------------------

//#include "SMultiSpec.h"
//#include	"SExtGlob.h"
#include	"MGraphView.h"



extern Handle GetNewSelectionInfoHandle (void);

extern void SetDefaultSelectionGraphWindowTitle (
				CMGraphView*						graphViewCPtr);

extern WindowPtr GetSelectionGraphImageWindow (
				CMGraphView*						graphViewCPtr);

#if use_mlte_for_text_window
	/*
	extern OSStatus TextViewFocusInWindow (
					WindowRef							window);

	extern pascal OSStatus TextWindowContentClicked (
					EventHandlerCallRef				nextHandler,
					EventRef								theEvent,
					void*									userData);
	*/
	extern pascal OSStatus TextWindowFocusAcquired (
					EventHandlerCallRef				nextHandler,
					EventRef								theEvent, 
					void*									userData);
#endif	// use_mlte_for_text_window

		// Prototype descriptions for routines in this file that are only
		// called by routines in this file.												

Boolean 	CreateImageWControls (
				WindowPtr							theWindow,
				Handle								windowInfoHandle,
				short int							legendWidth,
				Boolean								visible);

Boolean 	CreateThematicLegendControls (
				WindowPtr							theWindow,
				Handle								windowInfoHandle,
				Boolean								visibleFlag);

void 		DrawOutputWindowScrollBars (
				WindowPtr							theWindow);
							
void 		DrawThematicLegendControls (
				WindowPtr							theWindow);

void 		ReleaseWindowRecord (
				Handle								windowInfoHandle);
								


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CalcTextRect
//
//	Software purpose:	The purpose of this routine is to determine the view rectangle
// 						for the text output window. It comes form the Example WASTE 
//							demonstration routines.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 07/05/2001
//	Revised By:			Larry L. Biehl			Date: 07/05/2001	
							
							
void	CalcTextRect (
				WindowPtr 							window,
				LongRect*							textRectPtr)
				
{
	Rect 									portRect;
	
	
	GetWindowPortBounds (window, &portRect);

	textRectPtr->top = kTextWMargin;
	textRectPtr->left = kTextWMargin;
	textRectPtr->bottom = portRect.bottom - kSBarWidth - kTextWMargin;
	textRectPtr->right = portRect.right - kSBarWidth - kTextWMargin;
	
}	// end "CalcTextRect"
								


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ChangeLegendWidth
//
//	Software purpose:	The purpose of this routine is to change the width of
//							the legend in Thematic type image windows.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			DoMouseDnContent in MultiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 12/27/1989
//	Revised By:			Larry L. Biehl			Date: 03/11/2014	

void ChangeLegendWidth (
				WindowPtr							window, 
				Point									mousePt)
				
{
	Rect									limitRect,
											slopRect,
											thematicRect;
	
	Point									cellSize;
		
	Handle								windowInfoHandle;
									
	ListHandle							legendListHandle;
									
	RgnHandle							grayRegionHandle;
									
	SInt32								coordinateChange;
	
	Boolean								savedShowLegend;
	SInt16								currentLegendHeight,
											legendWidth;
									
	SInt16								horizontalChange;
	
	
			// Determine whether the change legend width control was double clicked on
			
	SInt32 doubleClickTime = GetDblTime ();
	SInt32 downClickTime = gEventRecord.when + doubleClickTime;
	Boolean doubleClickEvent = FALSE;						
			
	while (gEventRecord.when < downClickTime)
		{
		doubleClickEvent = GetNextEvent (mDownMask, &gEventRecord);
		GlobalToLocal (&gEventRecord.where);
		
		if ((abs (gEventRecord.where.h-mousePt.h) + 
														abs (gEventRecord.where.v-mousePt.v)) > 6)
			{
			downClickTime = 0;
			doubleClickEvent = FALSE;
			
			}	// end "if ((abs (gEventRecord.where.h-..." 
		
		if (doubleClickEvent)
			downClickTime = 0;
		
		}	// end "while (gEventRecord.when - ..." 
	
			// Get pointer to file information.												
				
	windowInfoHandle = (Handle)GetWRefCon (window);
	
			// Get local legend list handle.													
			
	legendListHandle = ((WindowInfoPtr)*windowInfoHandle)->legendListHandle;
			
			// Get current legend width.														
			
	legendWidth = gActiveLegendWidth;
	
			// Set the clip rectangle to include the legend and the image.			
			
	GetWindowClipRectangle (window, kImageScrollArea, &thematicRect);
  	ClipRect (&thematicRect);
  	
  	if (doubleClickEvent)
  		{
  		if (legendWidth == 0)
  			horizontalChange = ((WindowInfoPtr)*windowInfoHandle)->legendTitleWidth + 30;
  			
  		else	// legendWidth > 0
  			horizontalChange = (SInt16)0x8000;
  		
  		}	// end "if (doubleClickEvent)"
  	
  	else	// !doubleClickEvent
  		{
				// Create the region that specifies the size of the outline to be		
				// dragged.																				
		
		gTempRect.top = thematicRect.top;
		gTempRect.bottom = thematicRect.bottom;
		gTempRect.left =  gTempRect.right = thematicRect.left + legendWidth;
		gTempRect.right += kLegendWidthChanger + 1;
		
		RectRgn (gTempRegion1, &gTempRect);
			
				// The limit rectangle defines the area that the gray rectangle can 	
				// be drug around in.  SlopRect will allow the cursor to be drug all	
				// all over the screen.																
				
		limitRect.top = thematicRect.top;
		limitRect.bottom = thematicRect.bottom;
		limitRect.left = thematicRect.left + 
								((WindowInfoPtr)*windowInfoHandle)->legendTitleWidth + 25;
		limitRect.right = 
						thematicRect.right - gGrowAreaMinimum + kLegendWidthChanger + 1;
					
		grayRegionHandle = GetGrayRgn ();
		GetRegionBounds (grayRegionHandle, &slopRect);
		GlobalToLocal (&topLeft (slopRect));
		GlobalToLocal (&botRight (slopRect));
		slopRect.left = kLegendWidthChanger;
			
		coordinateChange = DragGrayRgn (
							gTempRegion1, mousePt, &limitRect, &slopRect, hAxisOnly, NIL);
									
		horizontalChange = LoWord (coordinateChange);
		
		}	// end "else !doubleClickEvent"
		
	if (horizontalChange == (SInt16)0x8000)
		legendWidth = 0;
		
	else	// horizontalChange != 0x8000
		{
		legendWidth += horizontalChange;
		if (legendWidth < ((WindowInfoPtr)*windowInfoHandle)->legendTitleWidth + 30)
			legendWidth = ((WindowInfoPtr)*windowInfoHandle)->legendTitleWidth + 30;
			
		}	// end "else horizontalChange != 0x8000"
			
	if (gActiveLegendWidth != legendWidth)
		{
				// Get the horizontal change.													
				
		horizontalChange = legendWidth - gActiveLegendWidth;
		
				// Erase the legend title to be ready for a new title to be drawn at a
				// new location.
				
		limitRect.top = thematicRect.top;
		limitRect.left = 0;
		limitRect.bottom = thematicRect.top + 16;
		limitRect.right = gActiveLegendWidth;
		EraseRect (&limitRect);
		InvalWindowRect (window, &limitRect);
		
				// Erase the old legend that is to be part of image if needed.		
				
		if (horizontalChange < 0)
			{
			limitRect.left = legendWidth - 6;
			limitRect.bottom = thematicRect.bottom - kSBarWidth;
			limitRect.right = gActiveLegendWidth;
			EraseRect (&limitRect);
			
			}	// end "if (horizontalChange < 0)" 
			
				// Scroll the image to the new location.									
				
		limitRect.left = legendWidth;
		if (horizontalChange > 0)
			limitRect.left = gActiveLegendWidth;
		limitRect.bottom = thematicRect.bottom - kSBarWidth;
		limitRect.right = thematicRect.right - kSBarWidth;
		SetEmptyRgn (gTempRegion1);
		ScrollRect (&limitRect, legendWidth-gActiveLegendWidth, 0, gTempRegion1);
		
				// Invalidate the right portion of the image window if the image	
				// was moved to the left to allow for a new portion of the image	
				// to be shown.																	
				
		if (horizontalChange < 0)
			{
			limitRect.left = thematicRect.right - kSBarWidth + horizontalChange;
			limitRect.bottom = thematicRect.bottom - kSBarWidth;
			limitRect.right = thematicRect.right - kSBarWidth;
			InvalWindowRect (window, &limitRect);
			
			}	// end "if (horizontalChange < 0)" 
		
				// Save the new parameters for the legend.
				
		UpdateLegendParameters (windowInfoHandle, legendWidth);
		gActiveLegendWidth = legendWidth;
		
				// Indicate whether the legend is to be drawn.							
				
		WindowInfoPtr windowInfoPtr = (WindowInfoPtr)*windowInfoHandle;
		savedShowLegend = windowInfoPtr->showLegend;
		if (legendWidth <= 11)
			windowInfoPtr->showLegend = FALSE;
		else
			windowInfoPtr->showLegend = TRUE;
			
		UpdateScrolls (window);
		DrawScrollBars (window);
		
				// If the legend is going from a no show condition to a show		
				// condition, then make certain that the legend list length			
				// is correct.																		
				
		if (!savedShowLegend && ((WindowInfoPtr)*windowInfoHandle)->showLegend)
			AdjustLegendListLength (window, FALSE);
		
				// Get the current legend list height.										
				
		currentLegendHeight = ((ListPtr)*legendListHandle)->rView.bottom - 
														((ListPtr)*legendListHandle)->rView.top;
		
				// Adjust the width of the legend list.									
				
		LSize (MAX (4, legendWidth-6), currentLegendHeight, legendListHandle);
			
				// Adjust the width of the cells.											
				
		cellSize = ((ListPtr)*legendListHandle)->cellSize;
		cellSize.h = MAX (10, legendWidth-6);
		LCellSize (cellSize, legendListHandle);
		
				// Force legend to be redrawn.			
		
		if (horizontalChange > 0)
			{
			limitRect.left = 0;
			limitRect.bottom = thematicRect.bottom - kSBarWidth;
			limitRect.right = legendWidth;
			InvalWindowRect (window, &limitRect);
			
			}	// end "if (horizontalChange > 0)" 
		
		}	// end "if (((WindowInfoPtr)*windowInfoHandle)->..." 
		
	SetEmptyRgn (gTempRegion1);
	
			// Reset the clip rectangle for the window.											
			
	GetWindowClipRectangle (window, kImageFrameArea,  &thematicRect);
  	ClipRect (&thematicRect);		
	
}	// end "ChangeLegendWidth" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CloseGraphicsWindow
//
//	Software purpose:	The purpose of this routine is to close a graphics
//							window and dispose of the structures that are related 
//							to the window including:
//								Window Information
//								and finally the Window itself.
//
//	Parameters in:		Pointer to window to be closed.
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			Menus in menus.c
//							main in multiSpec.c
//							ExitMultiSpec in multiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 09/27/1991
//	Revised By:			Larry L. Biehl			Date: 02/07/2018

void CloseGraphicsWindow (
				WindowPtr							theWindow)

{
	WindowInfoPtr						windowInfoPtr;
	
	Handle								windowRecordHandle,
											windowInfoHandle;
											
	SInt16								windowMenuItem,
											windowType;
	
	
			// Check input variable; exit if it is NULL									
			
	if (theWindow == NULL)																	return;
	
   windowType = 0;
	windowInfoHandle = (Handle)GetWRefCon (theWindow);
	
	if (windowInfoHandle != NULL)
		{
		HLock (windowInfoHandle);
		windowInfoPtr = (WindowInfoPtr)*windowInfoHandle;
		windowType = windowInfoPtr->windowType;
		windowMenuItem = windowInfoPtr->windowMenuItem;
		windowRecordHandle = windowInfoPtr->windowRecordHandle;
		
		}	// end "if (windowInfoHandle != NULL)" 
	
	if (windowType == kGraphicsWindowType && windowInfoHandle != NULL)
		{
		if (windowInfoPtr->graphViewCPtr != NULL)
			{
					// Check if selection graph window.										
					
			if (gSelectionGraphViewCPtr != NULL &&
										theWindow == gSelectionGraphViewCPtr->GetWindowPtr ())
				gUpdateWindowsMenuItemsFlag = TRUE;
 	      						
			delete (windowInfoPtr->graphViewCPtr);
			windowInfoPtr->graphViewCPtr = NULL;
			
			}	// end "if (windowInfoPtr->graphViewCPtr != NULL)" 
		
				// Unlock handle to window information structure						
		
		HUnlock (windowInfoHandle);
		
		HideWindow (theWindow);
		
				// Get deactivate event for the window that was hidden and handle	
				// it																					
		
		CheckSomeEvents (activMask+updateMask);
			
		DisposeHandle (windowInfoHandle);
	
		#if TARGET_API_MAC_CARBON	
			DisposeWindow (theWindow);
		#else		
			CloseWindow (theWindow);
			ReleaseWindowRecord (windowRecordHandle);
		#endif	// TARGET_API_MAC_CARBON else...
		
				// Delete the window name from the window menu							
		
		RemoveWindowFromList (theWindow, windowMenuItem);
      	
      		// Unload the graph window code segment if there are now no open	
      		// graph windows.																	
      		
      if (gNumberOfGWindows == 0)
      	{
  			if (gProjectInfoPtr)
  				gProjectInfoPtr->moveMemoryFlag = TRUE;
			
			}	// end " if (gNumberOfGWindows == 0)" 

		gUpdateWindowsMenuItemsFlag = TRUE;
			
		}	// end "if (windowType == kGraphicsWindowType)" 
		
}	// end "CloseGraphicsWindow" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		CloseImageWindow
//
//	Software purpose:	The purpose of this routine is to close an image
//							window, (either Multispectral or Thematic) and dispose 
//							of the structures that are related to the window 
//							including:
//								MFileInfo
//								HistogramSpecs
//								HistogramSummary
//								SecondOrderImageStats
//								StatFileInfo
//								SelectionRectangle
//								OffscreenMap
//								DisplaySpecsInfo
//								PaletteHandle
//							and finally the Window itself.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	TRUE if operation is cancelled
//							FALSE if operation was not cancelled			
// 
// Called By:			Menus in menus.c
//							main in multiSpec.c
//							ExitMultiSpec in multiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 07/07/1988
//	Revised By:			Ravi Budruk				Date: 08/16/1988
//	Revised By:			Larry L. Biehl			Date: 03/16/2018

Boolean CloseImageWindow (
				WindowPtr							theWindow)

{
	WindowInfoPtr						windowInfoPtr;
	
	Handle								selectionInfoH,
											windowInfoHandle,
											windowRecordHandle;
	
	PaletteHandle						paletteObject;
	
	SInt16								windowMenuItem,
											windowType = 0;
	
	
			// Check input variable; exit if it is NULL									
			
	if (theWindow == NULL)													
																							return (FALSE);
	
	windowInfoHandle = (Handle)GetWRefCon (theWindow); 
	
	if (windowInfoHandle != NULL)
		windowType = ((WindowInfoPtr)*windowInfoHandle)->windowType;
	
	if (windowType == kThematicWindowType)
		{
		if (SaveIfWindowChanged (windowInfoHandle, kNoForceClose))
																							return (TRUE);
		
		}	// end "if (windowType == kThematicWindowType)"
	
	if (windowInfoHandle != NULL)
		{
		HLock (windowInfoHandle);
		windowInfoPtr = (WindowInfoPtr)*windowInfoHandle;
		windowType = windowInfoPtr->windowType;
		windowMenuItem = windowInfoPtr->windowMenuItem;
		windowRecordHandle = windowInfoPtr->windowRecordHandle;
		
		}	// end "if (windowInfoHandle != NULL)" 
	
	if ((windowType == kImageWindowType || windowType == kThematicWindowType) && 
																				windowInfoHandle != NULL)
		{
				// Dispose of image statistics file handle if it exists				
		
		CMFileStream* supportFileStreamPtr = 
									(CMFileStream*)*windowInfoPtr->supportFileStreamHandle;
		CloseFile (supportFileStreamPtr);
		windowInfoPtr->supportFileStreamHandle = 
									UnlockAndDispose (windowInfoPtr->supportFileStreamHandle);
						
				// Dispose of image mask file if it exists.			
		
		CloseFile (windowInfoPtr->mask.fileStreamHandle);
		windowInfoPtr->mask.fileStreamHandle = 
									UnlockAndDispose (windowInfoPtr->mask.fileStreamHandle);
						
				// Save the palette object if available before closing other support
				// memory.
		
		paletteObject = NULL;
		if (windowInfoPtr->displaySpecsHandle != NULL)
			paletteObject =
					((DisplaySpecsPtr)*windowInfoPtr->displaySpecsHandle)->paletteObject;
			
		DisposeOfImageWindowSupportMemory (windowInfoPtr);
			
		windowInfoPtr->displaySpecsHandle = 
										UnlockAndDispose (windowInfoPtr->displaySpecsHandle);
		
				// Get handle to the selection area information.  Dispose of any	
				// polygon list information and then dispose of the handle to		
				// the selection information.	 Also force the coordinate 			
				// selection window	to be blank.											
		
		selectionInfoH = windowInfoPtr->selectionInfoHandle;
		UnlockAndDispose (
							((SelectionInfoPtr)*selectionInfoH)->polygonCoordinatesHandle);
		((SelectionInfoPtr)*selectionInfoH)->typeFlag = 0;
		ShowSelection (windowInfoHandle);
		UnlockAndDispose (selectionInfoH);
		windowInfoPtr->selectionInfoHandle = NULL;
		
				// Dispose of the map information handle if it exists.				
				
		windowInfoPtr->mapProjectionHandle = 
										UnlockAndDispose (windowInfoPtr->mapProjectionHandle);
		
				// Dispose of the list handle if it exists, Thematic type 			
				// image windows.																	
		
		if (windowInfoPtr->legendListHandle != NULL)
			LDispose (windowInfoPtr->legendListHandle);
		windowInfoPtr->legendListHandle = NULL;
		
				// Get handle to image file information and close the file(s).		
				//	Then dispose of the file information handle.							
		
		CloseWindowImageFiles (windowInfoHandle);
		
		UnlockAndDispose (windowInfoPtr->layerInfoHandle);
		
				// Unlock handle to window information structure						
		
		HUnlock (windowInfoHandle);
		
		HideWindow (theWindow);
		
				// Update global variables before doing activate and update	events.
		
		gActiveImageFileInfoH	= NULL;
		gActiveImageWindow 		= NULL;
		gActiveImageWindowInfoH = NULL;
		gActiveImageLayerInfoH 	= NULL;
		
				// Get deactivate event for the window that was hidden and handle	
				// it																					
		
		CheckSomeEvents (activMask+updateMask);
		
				// Set the palette associated with the color window to none.		
		
		if (((WindowInfoPtr)*windowInfoHandle)->grafPortType == kCGrafType)
			{
			SetPalette (theWindow, NULL, FALSE);
			if (gPaletteWindow)
				SetPalette	(gPaletteWindow, NULL, FALSE);
			
			}	// end "if (...grafPortType == kCGrafType)" 
			
				// The window structure is being disposed of. Make sure that it
				// will not be referenced in the non-Project processor specification 
				// structures. 
				
		ClearNonProjProcessorWindowInfoHandleReferences (windowInfoHandle);
			
		SetWRefCon (theWindow, 0); 
		DisposeHandle (windowInfoHandle);
		
				// Check if current histogram vector is based on this window.  If	
				// it is, dispose the histogram vector so that it will be redone	
				// properly for the next image display.									
				
		if (theWindow == gToDisplayLevels.window)
			gToDisplayLevels.vectorHandle = 
											UnlockAndDispose (gToDisplayLevels.vectorHandle);
	
		#if TARGET_API_MAC_CARBON	
			DisposeWindow (theWindow);
		#else		
			CloseWindow (theWindow);
			ReleaseWindowRecord (windowRecordHandle);
		#endif	// TARGET_API_MAC_CARBON else...
		
				// Dispose the palette if it is not NULL.									
				
		if (paletteObject != NULL)
			DisposePalette (paletteObject);
		
		RemoveWindowFromList (theWindow, windowMenuItem);
		
				// If the last project window selection was from this window, set	
				// that parameter to NULL since that selection is not available	
				// any more.  Also remove the coordinates from the statistics 		
				// window list if they exist.													
				
		if (theWindow == gProjectSelectionWindow)
			{
		   gProjectSelectionWindow = NULL;
		
			ClearNewFieldList ();
		   
		   }	// end "if (theWindow == gProjectSelectionWindow)" 
		   
				// Make certain that an active image window still exists if 		
				// possible.																		
				
		if (gActiveImageWindow == NULL && gNumberOfIWindows > 0)
			{
			gActiveImageWindow = gWindowList[gNumberOfIWindows+2];
			gActiveImageWindowInfoH = (Handle)GetWRefCon (gActiveImageWindow);
			gActiveImageFileInfoH =
								((WindowInfoPtr)*gActiveImageWindowInfoH)->fileInfoHandle;
			gActiveImageLayerInfoH = 
								((WindowInfoPtr)*gActiveImageWindowInfoH)->layerInfoHandle;
			
			}	// end "if (gActiveImageWindow == NULL && ..." 
	
				// If graph selection window is open, then indicate that the
				// parameters for the window need to be updated. Also change title if needed.
	
		HandleSelectionGraphImageWindowClosing (theWindow);

				// Update palette menu and redraw the palette if it exists.
				
		//gUpdatePaletteMenuItemsFlag = TRUE;
		//UpdatePaletteWindow (TRUE, FALSE);
		
		gUpdateProcessorMenuItemsFlag = TRUE;
		
		if (gActiveImageWindow == NULL)
			{
			gUpdateProjectMenuItemsFlag = TRUE;

			if (gProjectInfoPtr == NULL)
				gRedrawMenuBar = TRUE;
		   	
			gUpdateWindowsMenuItemsFlag = TRUE;
			
			}	// end 'if (gActiveImageWindow == NULL)' 
			
		}	// end "if (windowType == kImageWindowType || windowType == ..." 
		
	return (FALSE);
		
}	// end "CloseImageWindow" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CloseOutputWindow
//
//	Software purpose:	The purpose of this routine is to close the
//							output window to prepare for exitting the application.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			ExitMultiSpec in multiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 12/01/1988
//	Revised By:			Larry L. Biehl			Date: 02/07/2018

void CloseOutputWindow (
				WindowPtr							theWindow)

{
	Handle								windowInfoHandle;
	
	#if use_mlte_for_text_window
		TXNObject							textH;
	#endif	// use_mlte_for_text_window
	#if !use_mlte_for_text_window
		WEReference							textH;
	#endif	// !use_mlte_for_text_window
	
	SInt16								windowType;
	
	 
	if (theWindow != NULL)
		{
      windowType = 0;
		windowInfoHandle = (Handle)GetWRefCon (theWindow);
		
		if (windowInfoHandle != NULL)
         windowType = ((WindowInfoPtr)*windowInfoHandle)->windowType;
		
		if (windowType == kOutputWindowType && windowInfoHandle != NULL)
			{
					// Dispose of the handle to the edit record								
					
			textH = ((WindowInfoPtr)*windowInfoHandle)->textH;
			if (textH != NULL)
				#if use_mlte_for_text_window  
					TXNDeleteObject (textH);
				#else
					WEDispose (textH);
				#endif
			((WindowInfoPtr)*windowInfoHandle)->textH = NULL;
			gOutputTextH = NULL;
			
					// Dispose of the long control records.
					
			LCDetach (((WindowInfoPtr)*windowInfoHandle)->verticalControl);
			LCDetach (((WindowInfoPtr)*windowInfoHandle)->horizontalControl);
			
			HideWindow (theWindow);
			
					// Get deactivate event for the window that was hidden and 		
					// handle it																					
			
			CheckSomeEvents (activMask+updateMask);
			
			UnlockAndDispose (((WindowInfoPtr)*windowInfoHandle)->fileInfoHandle);
			
			UnlockAndDispose (windowInfoHandle);
	
			#if TARGET_API_MAC_CARBON	
				DisposeWindow (theWindow);
			#else		
				CloseWindow (theWindow);
				UnlockAndDispose (gOutputWindowRecordHandle);
				gOutputWindowRecordHandle = NULL;
			#endif	// TARGET_API_MAC_CARBON else...
			
			gWindowList[0] = NULL;
				
			}	// end "windowType == kOutputWindowType && windowInfoHandle != NULL" 
			
		}	// end "if (theWindow != NULL)" 

}	// end "CloseOutputWindow" 


/*		This feature was disabled on 3/28/2014
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				void ClosePaletteWindow
//
//	Software purpose:	The purpose of this routine is to close the
//							palette window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			Menus in menu.c
//							ExitMultiSpec in multiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 08/08/1989
//	Revised By:			Larry L. Biehl			Date: 09/08/2006

void ClosePaletteWindow (
				WindowPtr							theWindow)

{
	Handle								windowInfoHandle;
	PicHandle							myPicture;
	
	SInt16								windowType;
	
	 
	if (theWindow != NULL)
		{    	
		windowInfoHandle = (Handle)GetWRefCon (theWindow);
		
		if (windowInfoHandle != NULL)
			windowType = ((WindowInfoPtr)*windowInfoHandle)->windowType;
		
		if (windowType == kPaletteWindowType && windowInfoHandle != NULL)
			{
			HideWindow (theWindow);

					//	Dispose of picture for window if one already exists			
					
			myPicture = GetWindowPic (theWindow);
			if (myPicture != NULL)
				{
				KillPicture (myPicture);
				SetWindowPic (theWindow, NULL);
				
				}	// end "if (myPicture != NULL)" 
		
					// Set the palette associated with the color window to none.	
		
			if (((WindowInfoPtr)*windowInfoHandle)->grafPortType == kCGrafType)
				SetPalette	(theWindow, NULL, FALSE);
			
					// Get deactivate event for the window that was hidden and 		
					// handle it.																	
			
			CheckSomeEvents (activMask+updateMask);
	
			#if TARGET_API_MAC_CARBON	
				DisposeWindow (theWindow);
			#else		
				CloseWindow (theWindow);
			#endif	// TARGET_API_MAC_CARBON else...
			
			UnlockAndDispose (windowInfoHandle);
			
			gWindowList[1] = NULL;
			gPaletteWindow = NULL;
 			
			gUpdatePaletteMenuItemsFlag = TRUE;
			gUpdateWindowsMenuItemsFlag = TRUE;
			UpdatePaletteWindow (TRUE, FALSE);
				
			}	// end "windowType == kPaletteWindowType && windowInfoHandle != NULL" 
			
		}	// end "if (theWindow != NULL)" 

}	// end "ClosePaletteWindow" 
*/
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		CMGraphView* CreateGraphWindow
//
//	Software purpose:	The purpose of this routine is to create a graph
//							window and allocate a structure that contains handles to
//							other structures which will be connected with the
//							window and create the controls for the window.
//
//	Parameters in:		Handle to window information for image.
//
//	Parameters out:	None
//
// Value Returned:	Window pointer
// 
// Called By:			BiPlotDataControl in biPlotData.c
//							ListDataControl in SLstData.cpp
//							SelectionGraphControl in SSelGraf.cpp
//							HistogramStatsControl in statHistogram.c
//
//	Coded By:			Larry L. Biehl			Date: 09/27/1991
//	Revised By:			Larry L. Biehl			Date: 04/06/2018

CMGraphView* CreateGraphWindow (void)

{
			// Declare local variables 
			
	Rect									tempRect;	
	
	CMGraphView*						newGraphViewCPtr = NULL;
	GrafPtr								savedPort;
	GraphPtr								graphRecordPtr;
	WindowInfoPtr						windowInfoPtr;
	WindowPtr							front,
											windowPtr;												 
  
	ControlHandle						controlHandle,
											theRootControl;
	
	Handle								graphRecordHandle = NULL,
											selectionWindowInfoHandle,
											windowInfoHandle,
											windowRecordHandle = NULL;
												
	OSStatus								errorCode;
	
	SInt16								grafPortType,
											graphWindowID,
											windowMenuItem,
											xSize = 320,
											ySize = 170;
											
	Boolean								continueFlag = TRUE;
	
	SignedByte							handleStatus;

									
	front = (WindowPtr)-1;
					
	graphWindowID = kGraphWindowID;
	if (gProcessorCode == kSelectionGraphProcessor)								
		graphWindowID = kGraphSelectionWindowID;
		
	windowPtr = GetNewCWindow (graphWindowID, NULL, front);
	//windowPtr = GetNewCWindow (graphWindowID, NULL, gActiveImageWindow);
	grafPortType = kCGrafType;
		
	if (windowPtr == NULL)													
																						return (NULL);
		
	GetPort (&savedPort);
	SetPortWindowPort (windowPtr);
	
	windowInfoHandle = (Handle)MNewHandle (sizeof (WindowInfo));
	
			// If windowInfoHandle is NULL then there isn't enough memory.		
			// Clean up at end and exit the routine if there isn't enough	
			// memory.																			
		
	continueFlag = (windowInfoHandle != NULL);
	
	if (continueFlag)
		{
		SetWRefCon (windowPtr, (SInt32)windowInfoHandle);	
	
				// Initialize the window information record.
			
		InitializeWindowInfoStructure (
					windowInfoHandle, kNotPointer, NULL, kNoImageType, kGraphicsWindowType);
		
		newGraphViewCPtr = new (CMGraphView);

		continueFlag = (newGraphViewCPtr != NULL);
		
		}	// end "if (continueFlag)"
   		
   if (continueFlag)
		{	
				// Set the font and font size for the window.							
				
		TextFont (gWindowTextFont); 		// monaco  
		TextSize (9);							// 9 point 
	
		windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle, kLock); 
						
		windowInfoPtr->windowRecordHandle = windowRecordHandle;
		windowInfoPtr->grafPortType = grafPortType;
		windowInfoPtr->graphViewCPtr = newGraphViewCPtr;
		
				// Initialize some more of graph record handle parameters.							
		
		graphRecordHandle = newGraphViewCPtr->GetGraphRecordHandle ();
		graphRecordPtr = (GraphPtr)GetHandleStatusAndPointer (
																	graphRecordHandle, 
																	&handleStatus, 
																	kMoveHi);	
						
		graphRecordPtr->window = windowPtr;
		graphRecordPtr->imageWindow = NULL;
		graphRecordPtr->textSize = GetPortTextSize (GetWindowPort (windowPtr));
		
		newGraphViewCPtr->SetWindowPtr (windowPtr);
	 	
	 			// Change name of current selection graph if it exists.				
	 			// Also unlock the graph record handle since it will not be			
	 			// the selection graph any more.												
	 			
	 	if (gSelectionGraphViewCPtr != NULL && 
	 										gProcessorCode == kSelectionGraphProcessor)
	 		{
			WindowPtr currentSelectionWindow = 
													gSelectionGraphViewCPtr->GetWindowPtr ();
			SetGraphWindowTitle (gSelectionGraphViewCPtr, FALSE);
			GetWTitle (currentSelectionWindow, gTextString);
			selectionWindowInfoHandle = (Handle)GetWRefCon (currentSelectionWindow);
			if (selectionWindowInfoHandle != NULL)
				{
				windowMenuItem = 
						((WindowInfoPtr)*selectionWindowInfoHandle)->windowMenuItem;
				SetMenuItemText (gMultiSpecMenus[kWindowM], 
										windowMenuItem, 
										gTextString);
									
				}	// end "if (selectionWindowInfoHandle != NULL)" 
	 		
			CheckAndUnlockHandle (gSelectionGraphViewCPtr->GetGraphRecordHandle ());
		
	 		}	// end "if (gSelectionGraphViewCPtr != NULL && ..." 
		
				// Add new graph window to window list. Add to end of list.	
				
		UpdateWindowList (windowPtr, kGraphicsWindowType);	
			
		gUpdateWindowsMenuItemsFlag = TRUE;
		
				// Set the window title and then show the window on the screen
		
		SetGraphWindowTitle (newGraphViewCPtr, TRUE);
		GetWTitle (windowPtr, gTextString);
		AppendMenu (gMultiSpecMenus[kWindowM], "\pNewGraph");	
		SetMenuItemText (gMultiSpecMenus[kWindowM], 
									windowInfoPtr->windowMenuItem, 
									gTextString);
				
				// Set the location and size of the window.
				
		GetGraphWindowLocation (&xSize, &ySize);                            
		
		MoveWindow (windowPtr, 
						gNextGraphicsWindowStart.h, 
						gNextGraphicsWindowStart.v, 
						FALSE);
						
		SizeWindow (windowPtr, xSize, ySize, TRUE); 
		
				// Create the controls for the graph window if needed.														
				
		if (gProcessorCode == kSelectionGraphProcessor ||
						gProcessorCode == kListDataProcessor)
			{
					// Create the Root control
					
			errorCode = CreateRootControl (windowPtr, &theRootControl);
			if (errorCode != noErr)
																							return (FALSE);
			
					// Make the x-axis unit popup control
			
			GetWindowPortBounds (windowPtr, &gTempRect);
			tempRect.top    = gTempRect.bottom - kSBarWidth;
			tempRect.left   = gTempRect.left - 1;
			tempRect.bottom = gTempRect.bottom + 1;
			tempRect.right  = tempRect.left + kLegendScrollWidth;
			
			controlHandle = NewControl (windowPtr,
													&tempRect,
													"\p",
													FALSE,
													popupTitleCenterJust+popupTitleCondense,
													kPopUpGraphAxisLabelMenuID,
													0,
													popupMenuProc+popupUseWFont,
													kGraphXAxisLabelControl);
							
			if (controlHandle == NULL)
																							return (FALSE);
			
			graphRecordPtr->xAxisPopupControlHandle = controlHandle;
			
			MenuHandle menuHandle = GetPopUpMenuHandle (controlHandle);
			
			EnableAllMenuItems (menuHandle);

			}	// end "if (gProcessorCode == kSelectionGraphProcessor || ..."
				
		else if (gProcessorCode == kHistogramStatsProcessor)
			{
					// Create the Root control
					
			errorCode = CreateRootControl (windowPtr, &theRootControl);
			if (errorCode != noErr)
																							return (FALSE);
																							
					// Make the graph next set control
			
			GetWindowPortBounds (windowPtr, &gTempRect);
			tempRect.top    = gTempRect.bottom - kSBarWidth;
			tempRect.left   = gTempRect.left - 1;
			tempRect.bottom = gTempRect.bottom + 1;
			tempRect.right  = tempRect.left + kLegendScrollWidth;															
	
			controlHandle = NewControl (windowPtr,
													&tempRect,
													"\p^", 
													FALSE, 
													0, 
													0, // kControlContentIconSuiteRes,
													0,	// 10001,
													kControlBevelButtonSmallBevelProc, 
													kNextGraphSetControl);
							
			if (controlHandle == NULL)
																							return (FALSE);
			HiliteControl (controlHandle, 255);
		
					// Make the graph previous set control				
						
			tempRect.left   = tempRect.right;
			tempRect.right  = tempRect.left + kLegendScrollWidth;															
	
			controlHandle = NewControl (windowPtr,
													&tempRect, 
													"\pv", 
													FALSE, 
													0, 
													0, // kControlContentIconSuiteRes,
													0,	// 10001,
													kControlBevelButtonSmallBevelProc, 
													kPreviousGraphSetControl);
			
			if (controlHandle == NULL)
																						return (FALSE);
			HiliteControl (controlHandle, 255);
			
					// Make the graph select vector control				
						
			tempRect.left   = tempRect.right;
			tempRect.right  = tempRect.left + kBottomButtonWidth;
				
			controlHandle = NewControl (
						windowPtr, 
						&tempRect, 
						"\p", 
						FALSE,
						kColorMenuID, 
						kControlContentIconSuiteRes+kControlBehaviorMultiValueMenu, 
						10002, 
						kControlBevelButtonSmallBevelProc+kControlBevelButtonMenuOnRight,
						kGraphVectorControl);
			
			if (controlHandle == NULL)
																						return (FALSE);
			HiliteControl (controlHandle, 255);
			
					// Make the graph overlay control				
						
			tempRect.left   = tempRect.right;
			tempRect.right  = tempRect.left + kBottomButtonWidth;	
				
			controlHandle = NewControl (
							windowPtr, 
							&tempRect, 
							"\p", 
							FALSE,
							kColorMenuID, 
							kControlContentIconSuiteRes+kControlBehaviorMultiValueMenu, 
							10001, 
							kControlBevelButtonSmallBevelProc+kControlBevelButtonMenuOnRight,
							kGraphOverlayControl);
			
			if (controlHandle == NULL)
																						return (FALSE);
			HiliteControl (controlHandle, 255);
			
					// Make the graph bin width control				
						
			tempRect.left   = tempRect.right;
			tempRect.right  = tempRect.left + kBottomButtonWidth;	
				
			controlHandle = NewControl (
						windowPtr, 
						&tempRect, 
						"\p", 
						FALSE,
						kColorMenuID, 
						kControlContentIconSuiteRes+kControlBehaviorMultiValueMenu, 
						10004, 
						kControlBevelButtonSmallBevelProc+kControlBevelButtonMenuOnRight,
						kGraphBinWidthControl);
			
			if (controlHandle == NULL)
																						return (FALSE);
			HiliteControl (controlHandle, 255);
			
			}	// end "if (gProcessorCode == kHistogramStatsProcessor)"
		
				// Show and select the window.	
			
		ShowWindow (windowPtr);											
				
		if (gProcessorCode == kSelectionGraphProcessor ||
						gProcessorCode == kListDataProcessor)
			{	
					// Activate the window
					
			//SelectWindow (windowPtr);
			
			DrawGraphControls (windowPtr);
	
			}	// end "if (gProcessorCode == kSelectionGraphProcessor || ..."
		
		//CheckSomeEvents (activMask+updateMask);
		
		SetPort (savedPort);
		
	   		// Unlock the information handles.											
	  
	  	MHSetState (graphRecordHandle, handleStatus);
		CheckAndUnlockHandle (windowInfoHandle);
	   
	   }	// end "if (continueFlag)" 
   
   		// If there was a lack of memory, make certain that all handles have	
   		// been disposed of properly and dipose of the window.					
   		
   if (!continueFlag)
		{
		DisposeWindow (windowPtr);

		windowPtr = NULL;
		UnlockAndDispose (windowInfoHandle);

		if (newGraphViewCPtr != NULL)
			delete (newGraphViewCPtr);
		
		newGraphViewCPtr = NULL;
		
		}	// end "if (!continueFlag)" 
	
	return (newGraphViewCPtr);
	
}	// end "CreateGraphWindow"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CreateImageWControls
//
//	Software purpose:	This routine creates the controls for the
//							image window. This includes the scroll bars and the
//							zoom controls.
//
//	Functional description:	The steps in this function follow.
//			Draw the vertical control control first and set the vertical refcon 
//		 	value to 0.  Draw the horizontal control second and set the 		 
//			horizontal refcon value  to 1.  The horizontal control will allow for
//			a legend of width 'legendWidth' to be on the left side of the window.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			CreateImageWindow in window.c
//
//	Coded By:			Larry L. Biehl			Date: 09/03/1988
//	Revised By:			Larry L. Biehl			Date: 02/27/2018

Boolean CreateImageWControls (
				WindowPtr							theWindow, 
				Handle 								windowInfoHandle, 
				SInt16								legendWidth, 
				Boolean								visible)
				
{
	Rect									portRect;
	
	ControlHandle						controlHandle,
											theRootControl;
											
	OSStatus								errorCode;
	
	SInt16								minMax = 0;
	
	
	if (gAppearanceManagerFlag)
		{
				// Create the Root control
				
		errorCode = CreateRootControl (theWindow, &theRootControl);
		if (errorCode != noErr)
																							return (FALSE);
																					
		}	// end "if (gAppearanceManagerFlag)"
																					
	GetWindowPortBounds (theWindow, &portRect);
                                 
			// Note.  UnHilite the controls after making them since there is no	
			// image in the window yet	
	
			// Create the vertical scroll bar control										
			// Scale the vertical scroll bar to fit the window
										
	gTempRect.top    = portRect.top + kZoomHeight - 1;
	gTempRect.left   = portRect.right - kSBarWidth;
	gTempRect.bottom = portRect.bottom - kSBarWidth - 3*kZoomHeight + 1;
	gTempRect.right  = portRect.right + 1;	

			// Make the vertical scroll bar													

	controlHandle = NewControl (theWindow, 
											&gTempRect, 
											"\p", 
											visible, 
											minMax, 
											minMax, 
											minMax, 
											scrollBarProc, 
											kVerticalControl);
											
	if (controlHandle == NULL)
																							return (FALSE);
	HiliteControl (controlHandle, 255);
	((WindowInfoPtr)*windowInfoHandle)->verticalControl = controlHandle;
											

			// Create the horizontal scroll bar control									
			// Scale the horizontal scroll bar to fit the window						

	gTempRect.top    = portRect.bottom - kSBarWidth;
	gTempRect.left   = portRect.left + legendWidth - 1;
	gTempRect.bottom = portRect.bottom + 1;
	gTempRect.right  = portRect.right - kSBarWidth - kMagWidth;
	if (gTempRect.left > gTempRect.right)
		gTempRect.left = gTempRect.right;
	
			// Make the horizontal scroll bar												
	
	controlHandle = NewControl (theWindow, 
											&gTempRect, 
											"\p", 
											visible, 
											minMax, 	// value
											minMax, 	// min
											minMax, 	// max
											scrollBarProc, 
											kHorizontalControl);
								
	if (controlHandle == NULL)
																							return (FALSE);
	HiliteControl (controlHandle, 255);
	((WindowInfoPtr)*windowInfoHandle)->horizontalControl = controlHandle;
																
											
			// Create the coordinate view control													
			//		Get the control rectangle														

	gTempRect.top    = portRect.top - 1;
	gTempRect.left   = portRect.right - kSBarWidth;
	gTempRect.bottom = portRect.top + kZoomHeight + 1;
	gTempRect.right  = portRect.right + 1;
	
			// Make the coordinate view control													
	
	controlHandle = NewControl (theWindow,
											&gTempRect, 
											"\p-", 
											FALSE,
											0, 
											0,	// kControlContentIconSuiteRes, 
											0,	// 10001, 
											kControlBevelButtonSmallBevelProc,
											kCoordinateViewControl);
						
	if (controlHandle == NULL)
																							return (FALSE);
	HiliteControl (controlHandle, 0);
	((WindowInfoPtr)*windowInfoHandle)->coordinateViewControl = controlHandle;
											
											
			// Create the zoom-in control														
			// Get the control rectangle														

	gTempRect.top    = portRect.bottom - kSBarWidth - kZoomHeight;
	gTempRect.left   = portRect.right - kSBarWidth;
	gTempRect.bottom = portRect.bottom - kSBarWidth + 1;
	gTempRect.right  = portRect.right + 1;
	
			// Make the zoom-in control														
	
	controlHandle = NewControl (theWindow,
											&gTempRect, 
											"\p", 
											FALSE,
											0, 
											kControlContentCIconRes, 
											301, 
											kControlBevelButtonSmallBevelProc,
											kZoomInControl);
						
	if (controlHandle == NULL)
																							return (FALSE);
	HiliteControl (controlHandle, 255);
	((WindowInfoPtr)*windowInfoHandle)->zoomInControl = controlHandle;
																
											
			// Create the zoom-out control													
			// Get the control rectangle														

	gTempRect.top    = portRect.bottom - kSBarWidth - kZoomHeight - kZoomHeight;
	gTempRect.left   = portRect.right - kSBarWidth;
	gTempRect.bottom = portRect.bottom - kSBarWidth - kZoomHeight + 1;
	gTempRect.right  = portRect.right + 1;
	
			// Make the zoom-out control														
	
	controlHandle = NewControl (theWindow,
											&gTempRect, 
											"\p", 
											FALSE,
											0, 
											kControlContentCIconRes, 
											300, 
											kControlBevelButtonSmallBevelProc,
											kZoomOutControl);
	
	if (controlHandle == NULL)
																							return (FALSE);
	HiliteControl (controlHandle, 255);
	((WindowInfoPtr)*windowInfoHandle)->zoomOutControl = controlHandle;
																
			// Create the overlay control									
			// Get the control rectangle														

	gTempRect.top    = portRect.bottom - kSBarWidth;
	gTempRect.left   = portRect.left + legendWidth;
	gTempRect.bottom = portRect.bottom + 1;
	gTempRect.right  = portRect.left + kBottomButtonWidth;
	
			// Make the overlay control
	
	controlHandle = NewControl (
					theWindow, 
					&gTempRect, 
					"\p", 
					FALSE,
					kPopUpOverlaysMenuID, 
					kControlContentIconSuiteRes+kControlBehaviorMultiValueMenu, 
					10001, 
					kControlBevelButtonSmallBevelProc+kControlBevelButtonMenuOnRight,
					kOverlayControl);
	
	if (controlHandle == NULL)
																							return (FALSE);
																				
	HiliteControl (controlHandle, 255);
	((WindowInfoPtr)*windowInfoHandle)->overlayControl = controlHandle;
																
											
			// Create the magnification display control									
			// Get the control rectangle														

	gTempRect.top    = portRect.bottom - kSBarWidth;
	gTempRect.left   = portRect.right - kSBarWidth - kMagWidth;
	gTempRect.bottom = portRect.bottom + 1;
	gTempRect.right  = portRect.right - kSBarWidth + 1;
	
			// Make the magnification control													
	
	controlHandle = NewControl (theWindow,
											&gTempRect, 
											"\pX 1.0", 
											FALSE,
											0, 
											0,	// kControlContentIconSuiteRes, 
											0,	// 10001, 
											kControlBevelButtonSmallBevelProc,
											kMagControl);
						
	if (controlHandle == NULL)
																							return (FALSE);
	HiliteControl (controlHandle, 255);
	((WindowInfoPtr)*windowInfoHandle)->magnificationControl = controlHandle;
	
			// Create the coordinate units control														

	gTempRect.top    = portRect.top + 2;
	gTempRect.left   = portRect.left + 2;
	gTempRect.bottom = gTempRect.top + 20;
	gTempRect.right  = gTempRect.left + 126;
	
	controlHandle = NewControl (theWindow, 
											&gTempRect, 
											"\p", 
											FALSE,
											popupTitleCenterJust+popupTitleCondense,
											kPopUpCoordinateUnitsMenuID,
											0,
											popupMenuProc+popupUseWFont,
											kCoordinateUnitsControl);
						
	if (controlHandle == NULL)
																							return (FALSE);
																					
	((WindowInfoPtr)*windowInfoHandle)->coordinateUnitsControl = controlHandle;
	
			// Set the starting location for displaying the cursor coordinates.
																					
	SetCoordinateViewCursorStart (windowInfoHandle, 146);
											
			// Create the number pixels/area control														

	gTempRect.top    = portRect.top + 2;
	gTempRect.left   = gTempRect.right + 16;
	gTempRect.bottom = gTempRect.top + 10;
	gTempRect.right  = gTempRect.right + 16 + 12;
	
	controlHandle = NewControl (theWindow,
											&gTempRect, 
											"\p", 
											FALSE,
											popupTitleCenterJust+popupTitleCondense,
											kPopUpAreaUnitsMenuID,
											0,
											popupMenuProc+popupUseWFont,
											kAreaUnitsControl);
	
	if (controlHandle == NULL)
																							return (FALSE);
																					
	((WindowInfoPtr)*windowInfoHandle)->coordinateViewAreaUnitsControl = controlHandle;
		
			// set clip region of window.  I.E. clip at scroll bar edges.			

	gViewRect.top = portRect.top;
	gViewRect.left = portRect.left + legendWidth;
	gViewRect.bottom =  portRect.bottom - kSBarWidth;
	gViewRect.right = portRect.right - kSBarWidth;
	
	ClipRect (&gViewRect);
	
	return (TRUE);
	
}	// end "CreateImageWControls"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		WindowPtr CreateImageWindow
//
//	Software purpose:	The purpose of this routine is to create an image
//							window and allocate a structure that contains handles to
//							other structures which will be connected with the
//							window and create the scroll bars for the window.
//
//	Parameters in:		Handle to window information for image.
//
//	Parameters out:	None
//
// Value Returned:	Window pointer		
// 
// Called By:			SetUpImageWindow in menus.c
// 						SetUpThematicImageWindow in menus.c
//
//	Global Data:
//
//	Coded By:			Larry L. Biehl			Date: 06/27/1988
//	Revised By:			Ravi S. Budruk			Date: 08/02/1988	
//	Revised By:			Larry L. Biehl			Date: 09/05/2017	

Boolean CreateImageWindow (
				Handle								windowInfoHandle, 
				Boolean								thematicWindowFlag)
				
{
			// Declare local variables 
	
	Point									listPoint;
	
	WindowInfoPtr						windowInfoPtr;
	WindowPtr							front,
											theWindow;
	
	Handle								fileInfoHandle = NULL,
											layerInfoHandle,
											selectionInfoH = NULL,
											offScreenMapH = NULL,
											supportFileStreamHandle = NULL,
											windowRecordHandle = NULL;
											
	#if !TARGET_API_MAC_CARBON
		Handle								tempHandle;	
	#endif	// !TARGET_API_MAC_CARBON
	
	ListHandle							listHandle = NULL;
											
	SInt16								grafPortType,
											legendWidth,
											windowMenuItem;
	
	Boolean								continueFlag = TRUE;
	
	
			// Initialize local variables.												
			
	front = (WindowPtr)-1;
	listPoint.h = listPoint.v = 0;
	
	#if !TARGET_API_MAC_CARBON	
				// Get handle to window record.													
	
		windowRecordHandle = GetImageWindowRecord ();
		if (windowRecordHandle == NULL)
																							return (NULL);
			
		if (GetMaxSystemPixelSize () > 1)
			{
					// Window for Macs with color displays 									

			theWindow = GetNewCWindow (kImageCWindowID, *windowRecordHandle, front);
			grafPortType = kCGrafType;
			
			}	// end "if (GetMaxSystemPixelSize () > 1)" 
			
		else	// GetMaxSystemPixelSize () == 1 
																							return (NULL);
		
	#else	// TARGET_API_MAC_CARBON	
		theWindow = GetNewCWindow (kImageCWindowID, NULL, front);
		grafPortType = kCGrafType;
	#endif	// !TARGET_API_MAC_CARBON, else...	
		
	if (theWindow == NULL)															return (NULL);
		
	SetPortWindowPort (theWindow);

			// Create the scroll bar controls for the image window 					
			
	continueFlag = CreateImageWControls (theWindow, windowInfoHandle, 0, FALSE);
	
	if (continueFlag)
		{
		SetWRefCon (theWindow, (SInt32)windowInfoHandle);
		
		windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																windowInfoHandle, kLock, kMoveHi);
		
		windowInfoPtr->windowRecordHandle = windowRecordHandle;
		windowInfoPtr->grafPortType = grafPortType;
		
		//if (thematicWindowFlag)
		//	{
			windowInfoPtr->showLegend = TRUE;
			
			legendWidth = 0;
			if (thematicWindowFlag)
				legendWidth = gDefaultLegendWidth;
			UpdateLegendParameters (windowInfoHandle, legendWidth);
			
		//	}	// end "if (thematicWindowFlag)" 
		
		layerInfoHandle = windowInfoPtr->layerInfoHandle;
		
				// set the window title and then show the window on the screen		
				
		fileInfoHandle = windowInfoPtr->fileInfoHandle;
		HLock (fileInfoHandle);
		
		GetImageWindowName (NULL, 
									(FileInfoPtr)*fileInfoHandle, 
									gTextString, 
									FALSE);
		MSetWindowTitle (theWindow, (UCharPtr)gTextString);	
		
		HUnlock (fileInfoHandle);
		
		if (gProjectInfoPtr != NULL)
			{
			windowInfoPtr->projectBaseImageFlag = 
											CheckIfProjectBaseImage (fileInfoHandle);
												
			windowInfoPtr->projectWindowFlag = 
							windowInfoPtr->projectBaseImageFlag ||
												CheckIfProjectAssociatedImage (fileInfoHandle);
									
			}	// end "if (gProjectInfoPtr != NULL)" 
		
				// Set the font and font size for the window.							
				
		TextFont (gWindowTextFont); 		// monaco  
		TextSize (9);							// 9 point 
	
				// Get handle to structure for image statistics information file.	
				// OR if thematic type file ...												
				// Get handle to structure for image palette information file.		
				// This structure will be used for sta formatted support files.				
	
		supportFileStreamHandle = InitializeFileStream ((Handle)NULL);
		if (supportFileStreamHandle == NULL)
			continueFlag = FALSE;
									
		}	// end "if (continueFlag)" 
		
	if (continueFlag)
		{
		windowInfoPtr->supportFileStreamHandle = supportFileStreamHandle;
		
				// Set up handle to drag gray rectangle region.							
			
		selectionInfoH = GetNewSelectionInfoHandle ();
		if (selectionInfoH == NULL)
			continueFlag = FALSE;
		
		windowInfoPtr->selectionInfoHandle = selectionInfoH;
									
		}	// end "if (continueFlag)" 
	/*		
	if (continueFlag)
		{
				// Set selection area structure variables.								
			
		((SelectionInfoPtr)*selectionInfoH)->typeFlag = 0;
		((SelectionInfoPtr)*selectionInfoH)->numberPoints = 0;
		((SelectionInfoPtr)*selectionInfoH)->numberPixels = 0;
		((SelectionInfoPtr)*selectionInfoH)->polygonCoordinatesHandle = NULL;
		((SelectionInfoPtr)*selectionInfoH)->offScreenRectangle.top = 0;
		((SelectionInfoPtr)*selectionInfoH)->offScreenRectangle.left = 0;
		((SelectionInfoPtr)*selectionInfoH)->unitsDisplayCode = kLineColumnUnits;
	
				// Get handle to memory to use for the bit/pix map	
				
		//offScreenMapH = (Handle)NewPixMap ();
									
		}	// end "if (continueFlag)" 
	*/		
	if (continueFlag)
		{
		windowInfoPtr->offScreenMapHandle = offScreenMapH;
		
				// Set pointer to offscreen bit/pix maps to NULL if needed.			
		
		//if (offScreenMapH != NULL)		
		//	((PixMapPtr)*offScreenMapH)->baseAddr = NULL;
			
		//if (thematicWindowFlag)
		//	{
			TextSize (12);		// 12 point 
		
					// Everything is okay to this point.  Now, create the list for 
					// the legend for this Thematic window.								
			
			SetRect (&gViewRect, 
						2, 
						16,
						gDefaultLegendWidth-4, 
						gGrowAreaMinimum-2-kSBarWidth);
						
			SetRect (&gTempRect, 0, 0, 1, 0);
			
			#if TARGET_API_MAC_CARBON
				CreateCustomList (&gViewRect, 
										&gTempRect, 
										listPoint, 
										&gListDefinitionSpec, 
										theWindow, 
										FALSE, 
										FALSE, 
										FALSE, 
										FALSE,
										&listHandle);
								
			#else	// !TARGET_API_MAC_CARBON
				listHandle = LNew (&gViewRect, 
											&gTempRect, 
											listPoint, 
											131, 
											theWindow, 
											FALSE, 
											FALSE, 
											FALSE, 
											FALSE);
				
						// Make certain that the legend list definition is locked.					
						
				tempHandle = (Handle)Get1Resource ('LDEF', 131);
				HLock (tempHandle);
			#endif	// TARGET_API_MAC_CARBON, else...
																	
			windowInfoPtr->legendListHandle = listHandle; 
			if (listHandle == NULL)
				continueFlag = FALSE;
			
			if (continueFlag)
				{							
						// Force selecting in the list to allow only one selection at a 	
						// time.																				
				
				(*listHandle)->selFlags = lOnlyOne;
				(*listHandle)->listFlags = lDoVAutoscroll;
				(*listHandle)->refCon = grafPortType;
				
				if (grafPortType == kCGrafType)
					(*listHandle)->lClickLoop = gLegendCClickLoopPtr;
									
				}	// end "if (continueFlag)" 
				
			#if !TARGET_API_MAC_CARBON
				HUnlock (tempHandle);
			#endif	// !TARGET_API_MAC_CARBON
				
					// Create Thematic Legend Controls
					
			if (continueFlag)
				continueFlag = CreateThematicLegendControls (
																theWindow, windowInfoHandle, FALSE);
									
		//	}	// end "if (thematicWindowFlag)" 
									
		}	// end "if (continueFlag)" 
		
	if (continueFlag)
		{	
		windowInfoPtr->windowPtr = theWindow;
		
				// Add new image window to window list.  Insert the new image		
				// window before the graphics windows.	
				
		UpdateWindowList (theWindow, kImageWindowType);	
		
		windowMenuItem  = gNumberOfIWindows + kStatisticsWItem;
			
				// Add image name to Window menu.											
				// An AppendMenu and then SetMenuItemText is used so that no characters	
				// will be interpreted as "metacharacters" by AppendMenu.											
	
		InsertMenuItem (gMultiSpecMenus[kWindowM], "\p1", windowMenuItem-1);
		HLock (fileInfoHandle);
		FileStringPtr fileNamePtr = (FileStringPtr)
							GetFileNamePPointerFromFileInfo ((FileInfoPtr)*fileInfoHandle);
		MSetMenuItemText (gMultiSpecMenus[kWindowM], 
								windowMenuItem, 
								fileNamePtr,
								kASCIICharString);
		HUnlock (fileInfoHandle);
	 	
	 			// Update image window global variables									
	     			
	   gActiveImageWindow 		= theWindow;
		gActiveImageWindowInfoH = windowInfoHandle;
	   gActiveImageFileInfoH 	= fileInfoHandle;
		gActiveImageLayerInfoH 	= layerInfoHandle;
	   
	   		// Unlock the window information handle.									
	  
		HUnlock (windowInfoHandle);
	   
	   }	// end "if (continueFlag)" 
   
   		// If there was a lack of memory, make certain that all handles have	
   		// been disposed of properly and dipose of the window.					
   		
   if (!continueFlag)
		{
		#if TARGET_API_MAC_CARBON	
			DisposeWindow (theWindow);
		#else		
			CloseWindow (theWindow);
			ReleaseWindowRecord (windowRecordHandle);
		#endif	// TARGET_API_MAC_CARBON else...
		
		UnlockAndDispose (selectionInfoH);
		UnlockAndDispose (offScreenMapH);
		UnlockAndDispose (supportFileStreamHandle);
		theWindow = NULL;
		if (listHandle != NULL)
			LDispose (listHandle);
		
		}	// end "if (!continueFlag)" 
	
	return (continueFlag);
	
}	// end "CreateImageWindow" 


/*	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		CreatePaletteWindow
//
//	Software purpose:	The purpose of this routine is to create a palette window.
//								The palette window is not used any more.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			DrawPalette in paletteProc.c
//
//	Coded By:			Larry L. Biehl			Date: 07/25/1988
//	Revised By:			Larry L. Biehl			Date: 09/08/2006	

WindowPtr CreatePaletteWindow (void)

{
	BitMap								bitMap;
	
	WindowPtr							front,
											theWindow;
											
	WindowInfoPtr						windowInfoPtr;
	
	Handle								windowInfoHandle;
	
	SInt16								grafPortType;
	
	
	//gTempRect = qd.screenBits.bounds;
	GetQDGlobalsScreenBits (&bitMap);
	
	gTempRect.top = 70;
	gTempRect.left = bitMap.bounds.right-28;
	gTempRect.bottom = bitMap.bounds.bottom - 4;
	gTempRect.right = bitMap.bounds.right - 4;
	
	front = (WindowPtr)-1;
	
	#if TARGET_API_MAC_CARBON	
		theWindow = NewCWindow (NULL, 
										&gTempRect, 
										"\p", 
										1, 
										noGrowDocProc, 
										front, 
										FALSE, 
										(SInt32)NULL);
		grafPortType = kCGrafType;
	#else		
		if (gColorQDflag)
			{
					// Window for Macs with color displays 									

			theWindow = NewCWindow (*gPaletteWindowHandle, 
											&gTempRect, 
											"\p", 
											1, 
											noGrowDocProc, 
											front, 
											FALSE, 
											(SInt32)NULL);
			grafPortType = kCGrafType;
			
			}	// end "if (gColorQDflag)" 
			
		else	// !gColorQDflag 
			{
					// Window for Macs with Black&white only displays.						
					
			theWindow = NewWindow (*gPaletteWindowHandle, 
											&gTempRect, 
											"\p", 
											1, 
											noGrowDocProc, 
											front, 
											FALSE, 
											(SInt32)NULL);
			grafPortType = kGrafType;
			
			}	// end "else !gColorQDflag " 
	#endif	// TARGET_API_MAC_CARBON else...
		
	if (theWindow == NULL)															return (NULL);
	
			// Allocate WindowInfo record to store handle to structures and		
			// variables which provide additional information about the palette	
			// window																				

	windowInfoHandle = (Handle)MNewHandle (sizeof (WindowInfo));
	SetWRefCon (theWindow, (SInt32)windowInfoHandle);
	
			// Return if windowInfoHandle is NULL											
			
	if (windowInfoHandle != NULL)
		{
				// Initialize the window information structure.														
		
		windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																	windowInfoHandle, kLock, kMoveHi);
		
				// Initialize the window information record.								
				
		InitializeWindowInfoStructure (windowInfoHandle, 
													kNotPointer, 
													NULL, 
													kNoImageType,
													kPaletteWindowType);
				
				// Initialize items specific for palette window information structure.																		
										
		windowInfoPtr->windowMenuItem  = kPaletteWindow;
		windowInfoPtr->windowRecordHandle = NULL;
		windowInfoPtr->grafPortType = grafPortType;
	
		SetPortWindowPort (theWindow);
		
				// Reset the application window global variables 						
		
		gTheActiveWindow 			= theWindow;
		gActiveWindowInfoH 		= windowInfoHandle;
		
		gWindowList[1] = theWindow;
		
		HUnlock (windowInfoHandle);
		
		}	// end "if (windowInfoHandle != NULL)" 
		
	else	// windowInfoHandle == NULL 
		{
		#if TARGET_API_MAC_CARBON	
			DisposeWindow (theWindow);
		#else		
			CloseWindow (theWindow);
		#endif	// TARGET_API_MAC_CARBON else...
		
		theWindow = NULL;
		
		}	// end "else windowInfoHandle == NULL" 
	
	return (theWindow);
	
}	// end "CreatePaletteWindow"  
*/


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CreateThematicLegendControls
//
//	Software purpose:	This routine creates the controls for the thematic legend.
//							This includes the scroll scroll up and down controls and
//							the font control.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			CreateImageWindow in window.c
//
//	Coded By:			Larry L. Biehl			Date: 11/07/1996
//	Revised By:			Larry L. Biehl			Date: 09/07/2001			

Boolean CreateThematicLegendControls (
				WindowPtr							theWindow, 
				Handle 								windowInfoHandle, 
				Boolean								visibleFlag)
				
{
			// Local Declarations			
			
	Rect									portRect,
											tempRect;													 
  
	ControlHandle						controlHandle;
	
				
			// Create the legend scroll up control	
	
	GetWindowPortBounds (theWindow, &portRect);			
	tempRect.top    = portRect.bottom - kSBarWidth;
	tempRect.left   = portRect.left - 1;
	tempRect.bottom = portRect.bottom + 1;
	tempRect.right  = tempRect.left + kLegendScrollWidth;

			// Make the legend scroll up control														
	
	controlHandle = NewControl (theWindow,
											&tempRect, 
											"\p", 
											FALSE,
											0, 
											kControlContentCIconRes, 
											303, 
											kControlBevelButtonSmallBevelProc,
											kLegendScrollUpControl);
	
	if (controlHandle == NULL)
																							return (FALSE);
																					
	HiliteControl (controlHandle, 255);
	((WindowInfoPtr)*windowInfoHandle)->legendScrollUpControl = controlHandle;
	
			// Create the legend scroll down control	
				
	tempRect.left   = tempRect.right;
	tempRect.right  = tempRect.left + kLegendScrollWidth;

			// Make the legend scroll down control															
	
	controlHandle = NewControl (theWindow,
											&tempRect, 
											"\p", 
											FALSE,
											0, 
											kControlContentCIconRes, 
											302, 
											kControlBevelButtonSmallBevelProc,
											kLegendScrollDownControl);
	
	if (controlHandle == NULL)
																							return (FALSE);
																					
	HiliteControl (controlHandle, 255);
	((WindowInfoPtr)*windowInfoHandle)->legendScrollDownControl = controlHandle;
	
			// Create the palette popup control.	
	
	tempRect.left   = tempRect.right;
	tempRect.right  = tempRect.left + kLegendScrollWidth;
	/*		
	#if TARGET_API_MAC_CARBON
		Collection			collection;
		
		CreateCustomControl (theWindow,
										&tempRect,
										&gControlDefinitionSpec, 
	                           Collection             initData,
										&controlHandle);
	#else	// !TARGET_API_MAC_CARBON
	*/
	controlHandle = NewControl (
					theWindow, 
					&tempRect, 
					"\p", 
					FALSE,
					kPopUpPaletteMenuID, 
					kControlContentIconSuiteRes+kControlBehaviorMultiValueMenu, 
					10000, 
					kControlBevelButtonSmallBevelProc+kControlBevelButtonMenuOnRight,
					kLegendPaletteControl);
	/*				
	ControlButtonContentInfo	controlButtonInfo;
	controlButtonInfo.contentType = kControlBevelButtonMenuHandleTag;
	controlButtonInfo.u.ICONHandle = (void*)gPopUpPaletteMenu;
	
	if (controlHandle != NULL)
		SetBevelButtonContentInfo (controlHandle, &controlButtonInfo);
	*/
	
	if (controlHandle == NULL)
																							return (FALSE);
																			
	HiliteControl (controlHandle, 255);
	((WindowInfoPtr)*windowInfoHandle)->legendPaletteControl = controlHandle;
	
	return (TRUE);
	
}	// end "CreateThematicLegendControls"  


/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		WindowPtr DisposeOfImageWindowSupportMemory
//
//	Software purpose:	The purpose of this routine is to dispose of the
//							memory which supports the image window. This includes
//							the memory for the histogram and display specification
//							handles.  This also includes the memory associated with
//							the supporting offscreen window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			ModalFileSpecification in fileIO.c
//							CloseImageWindow in window.c
//
//	Coded By:			Larry L. Biehl			Date: 12/24/1991
//	Revised By:			Larry L. Biehl			Date: 08/28/1997	

void DisposeOfImageWindowSupportMemory (
				WindowInfoPtr						windowInfoPtr)

{
	DisplaySpecsPtr					displaySpecsPtr;
	
	Handle								displaySpecsH;
	
	SignedByte							handleStatus;
	
		
	if (windowInfoPtr != NULL)
		{
				// Get handle to histogram structure and dispose of handle to it	
				// if there is one.																
	
		HistogramSpecsPtr histogramSpecsPtr = (HistogramSpecsPtr)GetHandlePointer (
																windowInfoPtr->histogramSpecsHandle,
																kLock);
		if (histogramSpecsPtr != NULL)
			{
			UnlockAndDispose (histogramSpecsPtr->channelsHandle);
			UnlockAndDispose (histogramSpecsPtr->histogramArrayH);
			
			}	// end "if (histogramSpecsPtr != NULL)" 
			
		windowInfoPtr->histogramSpecsHandle = 
									UnlockAndDispose (windowInfoPtr->histogramSpecsHandle);
			
		windowInfoPtr->histogramSummaryHandle = 
									UnlockAndDispose (windowInfoPtr->histogramSummaryHandle);
							
				// Dispose of memory for the image sums and sums of squares.
				
		ImageSecondOrderStatsPtr imageSecondOrderStatsPtr = 
															&windowInfoPtr->imageSecondOrderStats;
										
		imageSecondOrderStatsPtr->sumsHandle = 
									UnlockAndDispose (imageSecondOrderStatsPtr->sumsHandle);
		imageSecondOrderStatsPtr->sumSquaresHandle =
								UnlockAndDispose (imageSecondOrderStatsPtr->sumSquaresHandle);
						
				// Dispose of or initialize information in the display specs structure 
				// that relates to the image being disposed of in memory and prepare the 
				// structure for a new image.
		
		displaySpecsH = windowInfoPtr->displaySpecsHandle;
				
		if (displaySpecsH != NULL)
			{
			handleStatus = HGetState (displaySpecsH);
			HLock (displaySpecsH);
			displaySpecsPtr = (DisplaySpecsPtr)*displaySpecsH;
			
			displaySpecsPtr->initializeStructureFlag = TRUE;
						
			displaySpecsPtr->symbolToPaletteEntryH = 
								UnlockAndDispose (displaySpecsPtr->symbolToPaletteEntryH);
					
			displaySpecsPtr->channelsHandle = 
								UnlockAndDispose (displaySpecsPtr->channelsHandle);
			
			displaySpecsPtr->displayClassGroupsHandle = 
								UnlockAndDispose (displaySpecsPtr->displayClassGroupsHandle);
			displaySpecsPtr->numberDisplayClasses = 0;
			displaySpecsPtr->numberDisplayGroups = 0;
			displaySpecsPtr->magnification = 1; 
			displaySpecsPtr->origin[kVertical] = 0;
			displaySpecsPtr->origin[kHorizontal] = 0;
			displaySpecsPtr->imageDimensions[kVertical] = 0;
			displaySpecsPtr->imageDimensions[kHorizontal] = 0;

			displaySpecsPtr->savedClassCTableHandle = (CTabHandle)
						UnlockAndDispose ((Handle)displaySpecsPtr->savedClassCTableHandle);
			
			displaySpecsPtr->savedGroupCTableHandle = (CTabHandle)
						UnlockAndDispose ((Handle)displaySpecsPtr->savedGroupCTableHandle);
			
			HSetState (displaySpecsH, handleStatus);
			
			}	// end "if (displaySpecsH != NULL)" 
				
				// Dispose of off screen bit/pix map and storage for offscreen		
				// image.																			
		
		if (gUseGWorldFlag && windowInfoPtr->grafPortType == kCGrafType)
			{
			if (windowInfoPtr->offscreenGWorld)
				{
				UnlockPixels ((PixMapHandle)windowInfoPtr->offScreenMapHandle);
				AllowPurgePixels ((PixMapHandle)windowInfoPtr->offScreenMapHandle);
				DisposeGWorld (windowInfoPtr->offscreenGWorld);
				
						// Now try to get the GWorld memory back again to be used	
						// for the next time a new image is displayed.					
				
				if (gNumberAvailableGWorldPtrs < kMaxNumberIWindows)
					{
					gGWorldPlaceHolderList[gNumberAvailableGWorldPtrs] =
																	MNewPointer (sizeof (CGrafPort));
					
					if (gGWorldPlaceHolderList[gNumberAvailableGWorldPtrs])
						gNumberAvailableGWorldPtrs++;
					
					}	// end "if (gNumberAvailableGWorldPtrs < ..." 
					
				windowInfoPtr->offscreenGWorld = NULL;
				
				}	// end "if (windowInfoPtr->offscreenGWorld)" 
			
			}	// end "if (gUseGWorldFlag && grafPortType == ...)" 
			
		else	// !gUseGWorldFlag || ...grafPortType == kGrafType 
			{		
			UnlockAndDispose (windowInfoPtr->offScreenMapHandle);
			
			windowInfoPtr->imageBaseAddressH = 
										UnlockAndDispose (windowInfoPtr->imageBaseAddressH);
			
			}	// end "else !gUseGWorldFlag || ..." 
			
		windowInfoPtr->offScreenMapHandle = NULL;
			
		}	// end "if (windowInfoPtr)" 
			
}	// end "DisposeOfImageWindowSupportMemory" 
*/	


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DrawCoordinateViewControls
//
//	Software purpose:	This routine redraws the scroll bars and the grow
//							icon when the window size changes.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			DoNonGraphWindowGrow in multiSpec.c
//							DoWindowZoomInOutEvent in multiSpec.c
//							DisplayThematicDialogOK in SDisThem.cpp
//							ShowCoordinateWindow in SMapTran.cpp
//							AdjustImageWSize in SOpnImag.cpp
//							ChangeLegendWidth in window.c
//
//	Coded By:			Larry L. Biehl			Date: 11/23/2000
//	Revised By:			Larry L. Biehl			Date: 11/23/2000			

void DrawCoordinateViewControls (
				Handle								windowInfoHandle)
				
{
			// Local Declarations	
	
	Rect									rect;
	
	WindowInfoPtr						windowInfoPtr;	
	
	SInt16								left,
											top;
											
	SignedByte							handleStatus;
	

			// Check input parameter.															
			
	if (windowInfoHandle == NULL)
																										return;
																						
			//	Initialize local parameters									
			
	windowInfoPtr = (WindowInfoPtr)GetHandleStatusAndPointer (windowInfoHandle,
																					&handleStatus);
																				
			// Save the current clip area and then set the clip area 
			// to the coordinate view.													
	
	GetClip (gTempRegion1);
														
	GetWindowClipRectangle (windowInfoPtr->windowPtr, kCoordinateArea, &rect);
	ClipRect (&rect);

			// Note.  Hide the controls so that when one is moved, it doesn't 	
			// mess up the other controls	
			
	HideControl (windowInfoPtr->coordinateUnitsControl);
	HideControl (windowInfoPtr->coordinateViewAreaUnitsControl);
			
			// Move the area units control.													

	left = windowInfoPtr->coordinateViewAreaPopupStart;
	top  = 2;
	MoveControl (windowInfoPtr->coordinateViewAreaUnitsControl, left, top);
	SizeControl (windowInfoPtr->coordinateViewAreaUnitsControl, 18, 20);		// x, y
		
			//	Show coordinate view controls.															
			
	if (windowInfoPtr->coordinateHeight > 0)
		{
		ShowControl (windowInfoPtr->coordinateUnitsControl);
		
		if (GetSelectionTypeCode (windowInfoPtr) > 0)
			ShowControl (windowInfoPtr->coordinateViewAreaUnitsControl);
			
		}	// end "else if (windowInfoPtr->coordinateHeight > 0)" 
																							
			// Reset the clip region back to that upon entry to this routine.		
			
	SetClip (gTempRegion1);
	SetEmptyRgn (gTempRegion1);
	
	MHSetState (windowInfoHandle, handleStatus);

}	// end "DrawCoordinateViewControls" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		DrawOutputWindowScrollBars
//
//	Software purpose:	This routine redraws the scroll bars and the grow
//							icon when the window size changes.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			DoNonGraphWindowGrow in multiSpec.c
//							DoWindowZoomInOutEvent in multiSpec.c
//							DisplayThematicDialogOK in SDisThem.cpp
//							ShowCoordinateWindow in SMapTran.cpp
//							AdjustImageWSize in SOpnImag.cpp
//							ChangeLegendWidth in window.c
//
//	Coded By:			Larry L. Biehl			Date: 12/19/2001
//	Revised By:			Larry L. Biehl			Date: 12/19/2001			

void DrawOutputWindowScrollBars (
				WindowPtr							theWindow)
				
{
			// Local Declarations	
	
	Rect									rect;	
	
	GrafPtr								savedPort;
	
	ControlHandle						horizontalControl,
											verticalControl;
											
	Handle								windowInfoHandle;
	
	SInt16								height,
											left,
											top,
											width;
	

			// Check input parameter.															
			
	if (theWindow == NULL)
																										return;
																						
			//	Initialize local parameters									

	GetWindowClipRectangle (theWindow, kImageScrollArea, &rect);
	
			// Get the window type																
			
	windowInfoHandle = (Handle)GetWRefCon (theWindow);
	if (windowInfoHandle == NULL)
																										return;			
				
	GetPort (&savedPort);
	SetPortWindowPort (theWindow);

			// Set clip region of window to scroll bar portion of the window									

	ClipRect (&rect);	
														
			// Draw vertical scroll bar.									

	verticalControl = ((WindowInfoPtr)*windowInfoHandle)->verticalControl;	
		
	left = rect.right - kSBarWidth;
	top  = rect.top - 1;
	MoveControl (verticalControl, left, top);

	height = rect.bottom - top - kSBarWidth + 1;
	SizeControl (verticalControl, kSBarWidth+1, height);
			
	#if !TARGET_API_MAC_CARBON
		HLock ((Handle)verticalControl);
		ValidWindowRect (theWindow, 
									GetControlBounds (verticalControl, &gTempRect));
		HUnlock ((Handle)verticalControl);
	 #endif	// !TARGET_API_MAC_CARBON
					
			// Draw horizontal scroll bar					

	horizontalControl = ((WindowInfoPtr)*windowInfoHandle)->horizontalControl;
		
	left = rect.left - 1;		
		
	top  = rect.bottom - kSBarWidth;
	MoveControl (horizontalControl, left, top);

	width  = rect.right - left - kSBarWidth + 1;
	SizeControl (horizontalControl, width, kSBarWidth+1);
			
	#if !TARGET_API_MAC_CARBON
		HLock ((Handle)horizontalControl);
		ValidWindowRect (theWindow, 
									GetControlBounds (horizontalControl, &gTempRect));
		HUnlock ((Handle)horizontalControl);
	 #endif	// !TARGET_API_MAC_CARBON
	
			// Draw the grow icon for the window											

	DrawGrowIcon (theWindow);
	
			//	Show window controls.
	
	ShowControl (verticalControl);	
	ShowControl (horizontalControl);	
	
			// Set clip region of window at scroll bar edges.							

	rect.bottom -= kSBarWidth;
	rect.right  -= kSBarWidth;
	ClipRect (&rect);				
		
	SetPort (savedPort);

}	// end "DrawOutputWindowScrollBars" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				DrawScrollBars
//
//	Software purpose:	This routine redraws the scroll bars and the grow
//							icon when the window size changes.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			DoNonGraphWindowGrow in multiSpec.c
//							DoWindowZoomInOutEvent in multiSpec.c
//							DisplayThematicDialogOK in SDisThem.cpp
//							ShowCoordinateWindow in SMapTran.cpp
//							AdjustImageWSize in SOpnImag.cpp
//							ChangeLegendWidth in window.c
//
//	Coded By:			Larry L. Biehl			Date: 03/17/1988
//	Revised By:			Larry L. Biehl			Date: 09/08/2006			

void DrawScrollBars (
				WindowPtr							theWindow)
				
{
			// Local Declarations	
	
	Rect									rect;	
	
	GrafPtr								savedPort;
	
	ControlHandle						rootControlHandle,
											theControl;
											
	Handle								windowInfoHandle;
	
	SInt16								height,
											left,
											legendWidth,
											lZoomHeight,
											magControlWidth = 41,
											magSpaceWidth,
											refCon,
											top,
											width,
											windowType;
											
	UInt32								totalNumberOverlays;
	
	UInt16								index,
											numberControls;
	

			// Check input parameter.															
			
	if (theWindow == NULL)
																										return;
																						
			//	Initialize local parameters									

	GetWindowClipRectangle (theWindow, kImageScrollArea, &rect);
	
	#if TARGET_API_MAC_CARBON
		magControlWidth = 51;
	#else	// !TARGET_API_MAC_CARBON
		magControlWidth = 41;
	#endif	// TARGET_API_MAC_CARBON, else
	
			// Get the window type																
			
	windowInfoHandle = (Handle)GetWRefCon (theWindow);
	if (windowInfoHandle == NULL)
																										return;		
																							
	windowType = ((WindowInfoPtr)*windowInfoHandle)->windowType;
	
	if (windowType == kOutputWindowType)
		{
		DrawOutputWindowScrollBars (theWindow);
																										return;
		
		}	// end "if (windowType == kOutputWindowType)"	
				
	GetPort (&savedPort);
	SetPortWindowPort (theWindow);
	
			// Get the legend width.  This may be non-zero for Thematic type 		
			// image windows.	 																	
			
	legendWidth = ((WindowInfoPtr)*windowInfoHandle)->legendWidth;
	totalNumberOverlays = ((WindowInfoPtr)*windowInfoHandle)->numberOverlays +
									((WindowInfoPtr)*windowInfoHandle)->numberImageOverlays;

			// Set clip region of window to scroll bar portion of the window									

	ClipRect (&rect);

			// Initialize space needed for the magnification control and the 		
			// zoom controls.																		
			// Note.  Hide the controls so that when one is moved, it doesn't 	
			// mess up the other controls														
	
	magSpaceWidth = 0;
	lZoomHeight = 0;	

	if (gAppearanceManagerFlag)	
		{									
		GetRootControl(theWindow, &rootControlHandle);
		
		if (rootControlHandle != NULL)
			{
			CountSubControls (rootControlHandle, &numberControls);
			for (index=numberControls; index>=1; index--)
				{
				GetIndexedSubControl(rootControlHandle, index, &theControl);
				refCon = (SInt16)GetControlReference (theControl);
				switch (refCon)
					{
					case kZoomInControl:
						lZoomHeight = 2*kZoomHeight + 1;
						break;
						
					case kMagControl:
						magSpaceWidth = magControlWidth + 2;
						break;
						
					default:
						break;
						
					}	// end "switch (refCon)" 
					
				HideControl (theControl);
				
				}	// end "for (index=numberControls; index>=1; index--)" 
				
			}	// end "if (rootControlHandle != NULL)"
		
		}	// end "if (gAppearanceManagerFlag)"
		
	#if !TARGET_API_MAC_CARBON
		else	// !gAppearanceManagerFlag
			{
			theControl = (ControlHandle)((WindowPeek)theWindow)->controlList;
			while (theControl != NULL)
				{
				refCon = (SInt16)GetControlReference (theControl);
				switch (refCon)
					{
					case kZoomInControl:
						lZoomHeight = 2*kZoomHeight + 1;
						break;
						
					case kMagControl:
						magSpaceWidth = magControlWidth + 2;
						break;
						
					default:
						break;
						
					}	// end "switch (refCon)" 
					
				HideControl (theControl);
				theControl = (*theControl)->nextControl;
				
				}	// end "while (theControl != NULL)" 
				
			}	// end "else !gAppearanceManagerFlag"
	#endif	// !TARGET_API_MAC_CARBON
		
			// Now draw the controls.															

	Boolean					validateFlag;	

	if (gAppearanceManagerFlag)	
		{
		for (index=numberControls; index>=1; index--)
			{	
			GetIndexedSubControl(rootControlHandle, index, &theControl);
			validateFlag = TRUE;
			refCon = (SInt16)GetControlReference (theControl);
			switch (refCon)
				{
				case kVerticalControl:		
							// Draw vertical scroll bar.											
		
					left = rect.right - kSBarWidth;
					top  = rect.top - 1;
					if (windowType == kImageWindowType || windowType == kThematicWindowType)
						top  += kZoomHeight;
					MoveControl (theControl, left, top);
				
					height = rect.bottom - top - kSBarWidth - lZoomHeight + 1;
					SizeControl (theControl, kSBarWidth+1, height);
					break;
					
				case kHorizontalControl:
							// Draw horizontal scroll bar	
		
					left = rect.left - 1;
					if (windowType == kThematicWindowType)
						left += legendWidth + kLegendWidthChanger + 1;
						
					if (totalNumberOverlays > 0)	
						left += kBottomButtonWidth - 1;			
						
					top  = rect.bottom - kSBarWidth;
					MoveControl (theControl, left, top);
				
					width  = rect.right - left - magSpaceWidth - kSBarWidth + 1;
					SizeControl (theControl, width, kSBarWidth+1);
					break;
						
				case kCoordinateViewControl:
							// Draw coordinate view control.													
				
					left = rect.right - kSBarWidth;
					top  = rect.top  - 1;
					MoveControl (theControl, left, top);
				
					SizeControl (theControl, kSBarWidth+1, kZoomHeight);
					
					validateFlag = FALSE;
					break;
						
				case kZoomInControl:
							// Draw zoom-in control.													
				
					left = rect.right - kSBarWidth;
					top  = rect.bottom - kSBarWidth - kZoomHeight;
					MoveControl (theControl, left, top);
				
					SizeControl (theControl, kSBarWidth+1, kZoomHeight);
					break;
				
				case kZoomOutControl:
							// Draw zoom-out control.												
						
					left = rect.right - kSBarWidth;
					top  = rect.bottom - kSBarWidth - kZoomHeight - kZoomHeight;
					MoveControl (theControl, left, top);
				
					SizeControl (theControl, kSBarWidth+1, kZoomHeight);
					break;
					
				case kMagControl:
							// Draw magnification display control.								
				
					left = rect.right - kSBarWidth - magControlWidth - 1;
					top  = rect.bottom - kSBarWidth;
					MoveControl (theControl, left, top);
				
					SizeControl (theControl, magControlWidth+1, kSBarWidth+1);
					break;
					
				case kOverlayControl:
							// Draw overlay control.
		
					left = rect.left - 1;
					if (windowType == kThematicWindowType)
						left += legendWidth + kLegendWidthChanger + 1;									
				
					top  = rect.bottom - kSBarWidth;
					MoveControl (theControl, left, top);
				
					SizeControl (theControl, kBottomButtonWidth, kSBarWidth+1);
					break;
					
				default:
					validateFlag = FALSE;
					break;
					
				}	// end "switch (refCon)" 
			
			#if !TARGET_API_MAC_CARBON
				if (validateFlag)
					{
					HLock ((Handle)theControl);
					ValidWindowRect (theWindow, 
											GetControlBounds (theControl, &gTempRect));
					HUnlock ((Handle)theControl);
					
					}	// end "if (validateFlag)"
			 #endif	// !TARGET_API_MAC_CARBON
			
			}	// end "while (theControl != NULL)" 
		
		}	// end "if (gAppearanceManagerFlag)"
		
	#if !TARGET_API_MAC_CARBON
		else	// !gAppearanceManagerFlag
			{
			theControl = (ControlHandle)((WindowPeek)theWindow)->controlList;
			while (theControl != NULL)
				{
				validateFlag = TRUE;
				refCon = (SInt16)GetControlReference (theControl);
				switch (refCon)
					{
					case kVerticalControl:
								// Draw vertical scroll bar.											
			
						left = rect.right - kSBarWidth;
						top  = rect.top - 1;
						if (windowType == kImageWindowType || 
																	windowType == kThematicWindowType)
							top  += kZoomHeight;
						MoveControl (theControl, left, top);
					
						height = rect.bottom - top - kSBarWidth - lZoomHeight + 1;
						SizeControl (theControl, kSBarWidth+1, height);
						break;
						
					case kHorizontalControl:
								// Draw horizontal scroll bar	
			
						left = rect.left - 1;
						if (windowType == kThematicWindowType)
							left += legendWidth + kLegendWidthChanger + 1;
							
						if (totalNumberOverlays > 0)	
							left += kBottomButtonWidth - 1;			
							
						top  = rect.bottom - kSBarWidth;
						MoveControl (theControl, left, top);
					
						width  = rect.right - left - magSpaceWidth - kSBarWidth + 1;
						SizeControl (theControl, width, kSBarWidth+1);
						break;
							
					case kCoordinateViewControl:
								// Draw coordinate view control.													
					
						left = rect.right - kSBarWidth;
						top  = rect.top  - 1;
						MoveControl (theControl, left, top);
					
						SizeControl (theControl, kSBarWidth+1, kZoomHeight);
						
						validateFlag = FALSE;
						break;
							
					case kZoomInControl:
								// Draw zoom-in control.													
					
						left = rect.right - kSBarWidth;
						top  = rect.bottom - kSBarWidth - kZoomHeight;
						MoveControl (theControl, left, top);
					
						SizeControl (theControl, kSBarWidth+1, kZoomHeight);
						break;
					
					case kZoomOutControl:
								// Draw zoom-out control.												
							
						left = rect.right - kSBarWidth;
						top  = rect.bottom - kSBarWidth - kZoomHeight - kZoomHeight;
						MoveControl (theControl, left, top);
					
						SizeControl (theControl, kSBarWidth+1, kZoomHeight);
						break;
						
					case kMagControl:
								// Draw magnification display control.								
					
						left = rect.right - kSBarWidth - magControlWidth - 1;
						top  = rect.bottom - kSBarWidth;
						MoveControl (theControl, left, top);
					
						SizeControl (theControl, magControlWidth+1, kSBarWidth+1);
						break;
						
					case kOverlayControl:
								// Draw overlay control.
			
						left = rect.left - 1;
						if (windowType == kThematicWindowType)
							left += legendWidth + kLegendWidthChanger + 1;									
					
						top  = rect.bottom - kSBarWidth;
						MoveControl (theControl, left, top);
					
						SizeControl (theControl, kBottomButtonWidth, kSBarWidth+1);
						break;
						
					default:
						validateFlag = FALSE;
						break;
						
					}	// end "switch (refCon)" 
				
				if (validateFlag)
					{
					HLock ((Handle)theControl);
					ValidWindowRect (theWindow, 
												GetControlBounds (theControl, &gTempRect));
					HUnlock ((Handle)theControl);
					
					}	// end "if (validateFlag)"
				
				theControl = (*theControl)->nextControl;
				
				}	// end "while (theControl != NULL)" 
				
			}	// end "else !gAppearanceManagerFlag"
	#endif	// !TARGET_API_MAC_CARBON
		
			// Draw the 'legend width changer rectangle' if needed.					
			
	if (windowType == kThematicWindowType)
		DrawThematicLegendControls (theWindow);
		
			// Draw the coordinate view controls.
		
	if (windowType == kImageWindowType || windowType == kThematicWindowType)
		DrawCoordinateViewControls (windowInfoHandle);
	
			// Draw the grow icon for the window											

	DrawGrowIcon (theWindow);
	
			//	Show window controls.	

	if (gAppearanceManagerFlag)	
		{	
		for (index=numberControls; index>=1; index--)
			{
			GetIndexedSubControl(rootControlHandle, index, &theControl);
			refCon = (SInt16)GetControlReference (theControl);
			if (refCon < kOverlayControl)
				ShowControl (theControl);
				
			else if (refCon == kOverlayControl && totalNumberOverlays > 0)
				ShowControl (theControl);
	     		
	     	}	// end "while (theControl != NULL)" 
     	
     	}	// end "if (gAppearanceManagerFlag)"
     	
	#if !TARGET_API_MAC_CARBON
		else	// !gAppearanceManagerFlag	
			{	
			theControl = (ControlHandle)((WindowPeek)theWindow)->controlList;
			while (theControl != NULL)
				{
				refCon = (SInt16)GetControlReference (theControl);
				if (refCon < kOverlayControl)
					ShowControl (theControl);
					
				else if (refCon == kOverlayControl && totalNumberOverlays > 0)
					ShowControl (theControl);
					
				theControl = (*theControl)->nextControl;
		     		
		     	}	// end "while (theControl != NULL)" 
	     	
	     	}	// end "if (gAppearanceManagerFlag)"
	#endif	// !TARGET_API_MAC_CARBON
	
			// Set clip region of window at scroll bar edges.							

	rect.bottom -= kSBarWidth;
	rect.right  -= kSBarWidth;
	ClipRect (&rect);				
		
	SetPort (savedPort);

}	// end "DrawScrollBars" 
	


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DrawThematicLegendControls
//
//	Software purpose:	This routine redraws the controls for the thematic legend.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			DrawScrollBars in window.c
//
//	Coded By:			Larry L. Biehl			Date: 11/07/1996
//	Revised By:			Larry L. Biehl			Date: 09/08/2001			

void DrawThematicLegendControls (
				WindowPtr							theWindow)
				
{
			// Local Declarations			
	
	Rect									rect,
											controlRect;	
											
	Pattern								pattern;												 
 
	ControlHandle						rootControlHandle,
											theControl;
											
	Handle								windowInfoHandle;
	
	SInt16								left,
											legendWidth,
											refCon,
											top;
	
	UInt16								index,
											numberControls;
	

			// Check input parameter.															
			
	if (theWindow == NULL)
																										return;
																						
			//	Initialize local parameters  (graph port)									

	GetWindowPortBounds (theWindow, &rect);
	
			// Get the window type																
			
	windowInfoHandle = (Handle)GetWRefCon (theWindow);
	if (windowInfoHandle == NULL)
																										return;
			
	legendWidth = ((WindowInfoPtr)*windowInfoHandle)->legendWidth; 
		
			// Draw the 'legend width changer rectangle'.					
			
	controlRect.top  = controlRect.bottom = rect.bottom;
	controlRect.top -= kSBarWidth;
	controlRect.left = controlRect.right = rect.left + legendWidth;
	controlRect.right += kLegendWidthChanger;
	FillRect (&controlRect, GetQDGlobalsBlack (&pattern));
	
	controlRect.right  = controlRect.left;
	controlRect.left = rect.left;
	if (controlRect.left < controlRect.right)
		FillRect (&controlRect, GetQDGlobalsWhite (&pattern));
	
	if (legendWidth > 0)
		{
		Boolean					validateFlag;
			
				// Now draw the controls for the thematic legend.	
				
		if (gAppearanceManagerFlag)	
			{
			GetRootControl(theWindow, &rootControlHandle);
			
			if (rootControlHandle != NULL)
				{
				CountSubControls (rootControlHandle, &numberControls);
				for (index=numberControls; index>=1; index--)
					{
					GetIndexedSubControl(rootControlHandle, index, &theControl);
					validateFlag = TRUE;
					refCon = (SInt16)GetControlReference (theControl);
					switch (refCon)
						{
						case kLegendScrollUpControl:
									// Draw legend scroll up control.											
				
							left = rect.left + legendWidth - 2*kLegendScrollWidth - 1;
							top  = rect.bottom - kSBarWidth;
							MoveControl (theControl, left, top);
						
							SizeControl (theControl, kLegendScrollWidth, kSBarWidth+1);
							break;
							
						case kLegendScrollDownControl:
									// Draw legend scroll down control.										
								
							left = rect.left + legendWidth - kLegendScrollWidth - 1;
							top  = rect.bottom - kSBarWidth;
							MoveControl (theControl, left, top);
						
							SizeControl (theControl, kLegendScrollWidth, kSBarWidth+1);
							break;
							
						case kLegendPaletteControl:
								// Draw legend text control.													
						
							left = rect.left + legendWidth - 3*kLegendScrollWidth - 9;
							top  = rect.bottom - kSBarWidth;
							MoveControl (theControl, left, top);
						
							SizeControl (theControl, kBottomButtonWidth, kSBarWidth+1);
							break;
							
						default:
							validateFlag = FALSE;
							break;
							
						}	// end "switch (refCon)" 
					
					#if !TARGET_API_MAC_CARBON
						if (validateFlag)
							{
							HLock ((Handle)theControl);
							ValidWindowRect (theWindow, GetControlBounds (theControl, &gTempRect));
							HUnlock ((Handle)theControl);
							
							}	// end "if (validateFlag)"
					#endif	// !TARGET_API_MAC_CARBON
					
					}	// end "for (index=numberControls; index>=1; index--)"
					
				}	// end "if (rootControlHandle != NULL)"
		
			}	// end "if (gAppearanceManagerFlag)"
		
		#if !TARGET_API_MAC_CARBON
			else	// !gAppearanceManagerFlag
				{
				theControl = (ControlHandle)((WindowPeek)theWindow)->controlList;
				while (theControl != NULL)
					{
					validateFlag = TRUE;
					refCon = (SInt16)GetControlReference (theControl);
					switch (refCon)
						{
						case kLegendScrollUpControl:
									// Draw legend scroll up control.											
				
							left = rect.left + legendWidth - 2*kLegendScrollWidth - 1;
							top  = rect.bottom - kSBarWidth;
							MoveControl (theControl, left, top);
						
							SizeControl (theControl, kLegendScrollWidth, kSBarWidth+1);
							break;
							
						case kLegendScrollDownControl:
									// Draw legend scroll down control.										
								
							left = rect.left + legendWidth - kLegendScrollWidth - 1;
							top  = rect.bottom - kSBarWidth;
							MoveControl (theControl, left, top);
						
							SizeControl (theControl, kLegendScrollWidth, kSBarWidth+1);
							break;
							
						case kLegendPaletteControl:
								// Draw legend text control.													
						
							left = rect.left + legendWidth - 3*kLegendScrollWidth - 9;
							top  = rect.bottom - kSBarWidth;
							MoveControl (theControl, left, top);
						
							SizeControl (theControl, kBottomButtonWidth, kSBarWidth+1);
							break;
							
						default:
							validateFlag = FALSE;
							break;
							
						}	// end "switch (refCon)" 
					
					if (validateFlag)
						{
						HLock ((Handle)theControl);
						ValidWindowRect (theWindow, 
												GetControlBounds (theControl, &gTempRect));
						HUnlock ((Handle)theControl);
						
						}	// end "if (validateFlag)"
					
					theControl = (*theControl)->nextControl;
					
					}	// end "while (theControl != NULL)"

				}	// end "else !gAppearanceManagerFlag"
		#endif	// !TARGET_API_MAC_CARBON
	
				//	Show window legend controls.	
				
		ShowControl (((WindowInfoPtr)*windowInfoHandle)->legendPaletteControl);
		ShowControl (((WindowInfoPtr)*windowInfoHandle)->legendScrollDownControl);
		ShowControl (((WindowInfoPtr)*windowInfoHandle)->legendScrollUpControl);
			
		}	// end "if (legendWidth > 0)"

}	// end "DrawThematicLegendControls" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				Handle GetImageWindowRecord
//
//	Software purpose:	The purpose of this routine is to obtain a window 
//							record for an image window.  It will first check for
//							any unused window records; if none are found then it
//							will obtain a handle for a new window record, move it 
//							to the top of the application heap past the project
//							memory (if there).
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			CreateImageWindow in window.c
//
//	Coded By:			Larry L. Biehl			Date: 03/07/1991
//	Revised By:			Larry L. Biehl			Date: 03/22/1991	

Handle GetImageWindowRecord (void)

{
	Handle								windowRecordHandle;
	
	
	windowRecordHandle = NULL;
	
	#if !TARGET_API_MAC_CARBON	
		Handle								projectInfoHandle;
		
				// Check for any unused window records.										
				
		if (gNumberAvailableImageWindowHandles > 0)
			{
			gNumberAvailableImageWindowHandles--;
			windowRecordHandle = 
								gImageWindowHandleList[gNumberAvailableImageWindowHandles];
			gImageWindowHandleList[gNumberAvailableImageWindowHandles] = NULL;
			
			}	// end "if (gNumberAvailableImageWindowHandles > 0)" 
			
		if (windowRecordHandle == NULL)
			{
				// No unused window records available, get a new window record.	
				// Note that WindowRecord and CWindowRecord are the same size.	

			windowRecordHandle = MNewHandle (sizeof (WindowRecord));
		
			if (windowRecordHandle != NULL)
				{
						// Unlock the project memory so that window record can be		
						// moved up past the project information.								
				
				projectInfoHandle = NULL;
				if (gProjectInfoPtr)
	  				UnlockProjectMemory (&gProjectInfoPtr, 0, &projectInfoHandle);
							
				MoveHHi (windowRecordHandle);
				HLock (windowRecordHandle);
				
						// Relock the project memory if it exists and update the			
						// pointers.																	
				
				if (projectInfoHandle)
					LockProjectMemory (projectInfoHandle, 0, &gProjectInfoPtr);
				
				}	// end "else windowRecordHandle != NULL" 
				
			}	// end "if (windowRecordHandle == NULL)" 
	#endif	// TARGET_API_MAC_CARBON else...
		
	return (windowRecordHandle);
		
}	// end "GetImageWindowRecord" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ReleaseWindowRecord
//
//	Software purpose:	The purpose of this routine is to release a window 
//							record for an image window.  It will add it to the 
//							list of unused records.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			CloseImageWindow in window.c
//							CreateImageWindow in window.c
//
//	Coded By:			Larry L. Biehl			Date: 03/07/1991
//	Revised By:			Larry L. Biehl			Date: 03/07/1991	

void ReleaseWindowRecord (
				Handle								windowRecordHandle)

{
	if (windowRecordHandle && 
						gNumberAvailableImageWindowHandles < kMaxNumberIWindows)
		{
		gImageWindowHandleList[gNumberAvailableImageWindowHandles] =
																					windowRecordHandle;
																			
		gNumberAvailableImageWindowHandles++;
																			
		}	// end "if (windowRecordHandle && ...)" 

}	// end "ReleaseWindowRecord"



#if use_mlte_for_text_window
/*
OSStatus GetTextViewFromWindow (
				WindowRef							window, 
				HIViewRef*							textViewPtr)
{
	OSStatus								status = paramErr;	
	const FourCharCode				kHITextViewSignature='mLTE';
	const ControlID					kTextViewControlID = {kHITextViewSignature, 1};

		
	if	(window != NULL)
		{
		status = HIViewFindByID (HIViewGetRoot (window), 
											kTextViewControlID, 
											textViewPtr);

		}	// end "if	(window != NULL)"
	
	return status;
	
}	// end "GetTextViewFromWindow"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		OSStatus TextViewFocusInWindow
//
//	Software purpose:	The purpose of this routine is to handle window focus aquired 
//							events in output text windows.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/22/2008
//	Revised By:			Larry L. Biehl			Date: 12/30/2008	

OSStatus TextViewFocusInWindow (
				WindowRef							window)
{
	OSStatus							status = paramErr;
	HIViewRef						textView = NULL;
	HIViewRef						scrollView = NULL;
	
	
			// Get the HITextView from the window
			
	status = GetTextViewFromWindow (window, &textView);

	scrollView = HIViewGetSuperview (textView);
	
	status = HIViewSetFirstSubViewFocus (HIViewGetRoot (window), NULL);
	status = HIViewSetNextFocus (scrollView, NULL);
	status = HIViewAdvanceFocus (scrollView, 0);

	return status;
	
}	// end "TextViewFocusInWindow"
*/

/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		OSStatus TextWindowContentClicked
//
//	Software purpose:	The purpose of this routine is to handle a mouse down in the
//							content portion of the text output window.
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:	status code				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/29/2008
//	Revised By:			Larry L. Biehl			Date: 12/29/2008	

pascal OSStatus TextWindowContentClicked (
				EventHandlerCallRef				nextHandler, 
				EventRef								theEvent, 
				void*									userData)

{
	#pragma unused (nextHandler, userData)
	WindowRef							window;
	OSStatus								status = noErr;
	

	status = GetEventParameter (theEvent, 
											kEventParamDirectObject, 
											typeWindowRef, 
											NULL, 
											sizeof (WindowRef), 
											NULL, 
											&window);

	if (status == noErr && window != gOutputWindow)
		{
		if (gPresentCursor == kIBeam)
			{	
			TXNClick (gOutputTextH, &gEventRecord);
			
			}	// end "if (gPresentCursor == kIBeam)" 
	
		}	// end "if (status == noErr && window != NULL)"

	return status;
	
}	// end "TextWindowContentClicked"
*/


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		OSStatus TextWindowFocusAcquired
//
//	Software purpose:	The purpose of this routine is to handle window focus aquired events
//							in output text windows.
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:	status code				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/22/2008
//	Revised By:			Larry L. Biehl			Date: 02/25/2012	

pascal OSStatus TextWindowFocusAcquired (
				EventHandlerCallRef				nextHandler, 
				EventRef								theEvent, 
				void*									userData)

{
	#pragma unused (nextHandler, userData)
	WindowRef							window;
	OSStatus								status = noErr;
	//Rect								viewRect;
	

	GetEventParameter (theEvent, 
								kEventParamDirectObject, 
								typeWindowRef, 
								NULL, 
								sizeof (WindowRef), 
								NULL, 
								&window);

	if (status == noErr && window != NULL)
		{
		gActiveOffscreenMapExists = FALSE;
	
     	gTheActiveWindow 		= window;
     	gActiveWindowInfoH 	= gOutputWindowInfoH;
     	gActiveWindowType		= kOutputWindowType;
     	gActiveLegendWidth 	= 0;
		
		//TextViewFocusInWindow (window);	
								
		TXNFocus (gTextObject, TRUE);
		/*
		GetWindowClipRectangle (window, kFrameArea, &viewRect);
		TXNSetFrameBounds (gTextObject,
									viewRect.top,
									viewRect.left,
									viewRect.bottom,
									viewRect.right,
									0);
		TXNRecalcTextLayout (gOutputTextH);
		TXNUpdate (gOutputTextH);
		*/
		gUpdateFileMenuItemsFlag = TRUE; 
		gUpdateEditMenuItemsFlag = TRUE; 
		gUpdateProjectMenuItemsFlag = TRUE;
		gUpdatePaletteMenuItemsFlag = TRUE;
		gUpdateWindowsMenuItemsFlag = TRUE;
		/*
		OSStatus								status;
		TXNControlTag						iControlTags[1];
		TXNControlData						iControlData[1];
		TXNMargins							txnMargins;
		TXNTab								txnTab;
		
		iControlTags[0] = kTXNMarginsTag;
		txnMargins.leftMargin = 5;
		txnMargins.rightMargin = 5;
		txnMargins.topMargin = 5;
		txnMargins.bottomMargin = 5;
		iControlData[0].marginsPtr = &txnMargins;		
		status = TXNGetTXNObjectControls (gTextObject, 1, iControlTags, iControlData);
		
		iControlTags[0] = kTXNSelectionStateTag;
		status = TXNGetTXNObjectControls (gTextObject, 1, iControlTags, iControlData);
		
		iControlTags[0] = kTXNDrawSelectionWhenInactiveTag;
		status = TXNGetTXNObjectControls (gTextObject, 1, iControlTags, iControlData);
		
		iControlTags[0] = kTXNTabSettingsTag;
		txnTab.value = 1;
		txnTab.tabType = kTXNLeftTab;
		iControlData[0].tabValue = txnTab;
		status = TXNGetTXNObjectControls (gTextObject, 1, iControlTags, iControlData);
		*/		
		status = noErr;
	
		}	// end "if (status == noErr && window != NULL)"

	return status;
	
}	// end "TextWindowFocusAcquired"
#endif	// use_mlte_for_text_window
