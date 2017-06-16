//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
// File:						multiSpec.c
//
//	Authors:					Larry L. Biehl
//
//	Revision number:		3.0
//
//	Revision date:			04/24/2017
//
//	Language:				C
//
//	System:					Macintosh Operating System
//
//	Functions in file:	Boolean 				CheckSomeEvents
//								void 					ConvertOffScreenPointToWinPoint
//								void 					CopyOffScreenImage
//								void 					DoActivateEvent
//								void					DoGraphicsWActivateEvent
//								void					DoImageWActivateEvent
//								void 					DoImageWUpdateEvent
//								void					DoMouseDnContent
//								void					DoMouseDnGrow
//								void 					DoMultiSpec
//								void					DoNonGraphWindowGrow
//								void					DoOutputWActivateEvent
//								void 					DoOutputWUpdateEvent
//								void					DoPaletteWActivateEvent
//								void 					DoPaletteWUpdateEvent
//								void					DoStatisticsAddFieldEvent
//								void 					DoSuspendResume
//                      void 					DoUpdateEvent
//								void					DoWindowZoomInOutEvent
//								void 					ExitMultiSpec
//								void					FixCursor
//								void					GetSpareMemory
//								pascal long int 	MyGrowZone
//								void					UpdateCursorCoordinates
//
//	Brief description:	This program is a image analysis program patterned
//								after LARSYS that runs on an IBM VM system.
//
//	Diagram of MultiSpec routine calls for the routines in the file.
//
//		main
//			InitializeMultiSpec (in initialize.c)
//			FixCursor
//			ListString (in multiSpecUtilities.c)
//			Menus (in menus.c)
//			DoMouseDnContent
//				CheckSomeEvents
//					DoActivateEvent
//					DoUpdateEvent
//				FixCursor
//					GetWindowClipRectangle
//				ImageWControlEvent (in controls.c)
//				OutputWControlEvent (in controls.c)
//				StatisticsWControlEvent (in statistics.c)
//				SelectArea (in selectionArea.c)
//				HandScrollImage (in controls.c)
//				StatisticsWMouseDn (in statistics.c)
//			DoMouseDnGrow
//				UpdateScrolls (in controls.c)
//				DrawScrollBars (in windows.c)
//			CloseImageWindow (in windows.c)
//			DoUpdateEvent
//				DoOutputWUpdateEvent
//					GetWindowClipRectangle
//				DoImageWUpdateEvent
//					CopyOffScreenImage
//						OutlineFieldsControl (in outlineFields.c)
//					GetWindowClipRectangle
//				DoPaletteWUpdateEvent
//				DoStatWUpdateEvent
//					UpdateStatisticsWindow (in statistics.c)
//			DoActivateEvent
//				DoOutputWActivateEvent
//				DoImageWActivateEvent
//				DoPaletteWActivateEvent
//				DoStatWActivateEvent
//					ActivateStatControls
//				GetWindowClipRectangle
//			ExitMultiSpec
//				CloseTheProject (in project.c)
//				CloseImageWindow (in windows.c)
//				CloseOutputWindow (in windows.c)
//
//		ConvertWinPointToLC
//		
//		ConvertLCToWinPoint
//
//		ShowSelection
//
//		MyGrowZone
//
//		UnlockAndDispose
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"

//#include	"SMulSpec.h"
//#include	"SExtGlob.h" 
//#include "HIToolbox/Events.h"  
#include	"SGrafVew.h"

#define kContinueStopAlertID		1169	

extern void				DoNextDisplayChannelEvent (
								WindowPtr							theWindow,
								char									theChar);


			// Prototypes for routines in this file that are only called by		
			// other routines in this file.		

void 						DoImageMouseDnContent (
								WindowPtr							theWindow,
								Point									mousePt);

void						DoImageWActivateEvent (
								WindowPtr							window, 
								Handle								windowInfoHandle);

void 						DoImageWUpdateEvent (
								WindowPtr							window, 
								Handle								windowInfoHandle);

void						DoMouseDnContent (
								WindowPtr							theWindow);

void						DoMouseDnGrow (
								WindowPtr							theWindowr);

void						DoNonGraphWindowGrow (
								WindowPtr							theWindow,
								Handle								windowInfoHandle);

void 						DoOutputMouseDnContent (
								WindowPtr							theWindow,
								Point									mousePt);

void						DoOutputWActivateEvent (
								WindowPtr							window,  
								#if use_mlte_for_text_window 
									TXNObject							textH, 
								#endif
								#if !use_mlte_for_text_window 
									WEReference							textH, 
								#endif  
								Boolean								becomingActiveFlag);			

void 						DoOutputWindowGrow (
								WindowPtr							theWindow,
								Handle								windowInfoHandle);

void 						DoOutputWUpdateEvent (
								WindowPtr							theWindow, 
								#if use_mlte_for_text_window 
									TXNObject							textH, 
								#endif
								#if !use_mlte_for_text_window 
									WEReference							textH, 
								#endif 
								Rect*);

void						DoPaletteWActivateEvent (void);

void 						DoPaletteWUpdateEvent (
								WindowPtr);

void						DoStatisticsAddFieldEvent (void);

void 						DoStatisticsMouseDnContent (
								WindowPtr							theWindow,
								Point									mousePt);

void						DoWindowZoomInOutEvent (
								WindowPtr 							window, 
								SInt16					 			partCode);
								
void						DoWorkFlow (void);

void 						ExitMultiSpec (
								Boolean);

void						FixCursor (void);			

void						GetMaximumSizeForDisplayPointIsIn ( 
								Point									location,
								LongPoint*							maximumWindowSizePointPtr);

void						SendMenuClearSelectionRectangleCommand (void);

void						UpdateCursorCoordinates (
								Point 								mousePt);



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CheckSomeEvents
//
//	Software purpose:	This routine checks for specified events and 
//							handles them.  This routine is used by processors
//							which look for command-"." to stop the process and
//							and by processors which can/are running in the
//							background.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	FALSE if user issued a cancel command.
//							TRUE if user did not issue a cancel command.
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/25/1989
//	Revised By:			Larry L. Biehl			Date: 10/04/2013		

Boolean CheckSomeEvents (
				SInt16								eventMask)

{
	DialogPtr							dialogPtr;
	WindowPtr							window;
	
	SInt32								menuData;
	
	SInt16								itemHit,
											mouseLoc,
											theChar;
											
	UInt16								what;
	
	Boolean								haveEvent,
											returnFlag;

	
	if (gUseThreadedIOFlag)
		YieldToAnyThread();
	
			// Do event loop processing for check some events.							
	
	returnFlag = TRUE;
//	haveEvent = TRUE;	 
	if (eventMask != 0)
		{
		do
			{
			haveEvent = WaitNextEvent (eventMask, &gEventRecord, gYieldTime, NIL);
				
			if (haveEvent)
				{
				what = gEventRecord.what;
				
				if ( IsDialogEvent (&gEventRecord) )
					{
					if ( !DialogSelect (&gEventRecord, &dialogPtr, &itemHit) )
						{
						if (what == updateEvt)
							what = nullEvent;
							
						}		// end "if ( !DialogSelect (..." 
						
					else		// DialogSelect (&gEventRecord, &dialogPtr, &itemHit) 
						{	
						if (dialogPtr == gStatusDialogPtr && itemHit == 1)
							{
							returnFlag = FALSE;
							what = nullEvent;
																						
							}		// end "if (dialogPtr == gStatusDialogPtr && ..." 
							
						}		// end "if ( DialogSelect (&gEventRecord, ..." 
					
					}		// end "if (IsDialogEvent (&gEventRecord))" 
					
	//			else
	//				what = gEventRecord.what;
					
				switch (what)
					{
					case mouseDown:

						mouseLoc = FindWindow (gEventRecord.where, &window);
			  
						switch (mouseLoc)
							{
							case inMenuBar:
							
								menuData = MenuSelect (gEventRecord.where);
								break;
					
							case inDrag:
					
								if (window == GetDialogWindow(gStatusDialogPtr))
									DragMultiSpecWindow(window, gEventRecord.where);
									
								else		// window == NULL || (window != ... 
									FlushEvents (mUpMask,mDownMask);
									
								break;
								
							case inContent:
					
										// Flush the mouse ups so that we are ready to 	
										// look for the next mouse down.  Otherwise		
										// no more mousedowns are seen.						
								
								FlushEvents (mUpMask,mDownMask);
								break;
								
							default:
								FlushEvents (mUpMask,mDownMask);
								break;
								
							}		// end "switch (mouseLoc)" 
							
						break;
						
					case keyDown:
						
								// If the command key and the period have been 			
								// depressed then return FALSE indicating that the 	
								// user wants to stop the current process.				
						
						theChar = (SInt16)gEventRecord.message & charCodeMask;
						if ( (theChar == 0x1B) ||
							((gEventRecord.modifiers&cmdKey) && ( theChar == 0x2e )) )
							{
							if (gStatusDialogPtr != NULL && 
												gTheActiveWindow == GetDialogWindow(gStatusDialogPtr))
								FlashDLogItem (gStatusDialogPtr, 1);
							returnFlag = FALSE;
																						
							}		// end "if ( (theChar == 0x1B) || ..." 
							
						break;
			  
					case updateEvt:
					
						DoUpdateEvent (&gEventRecord);
						break;
				 
					case activateEvt:

						DoActivateEvent ( (WindowPtr)gEventRecord.message,
														(gEventRecord.modifiers & activeFlag) );
						break;
				 
					case osEvt:
						DoSuspendResume (&gEventRecord);
						break;
						
					default:
	//	    			FlushEvents (mUpMask,mDownMask);
						itemHit = what;
						break;
				
					} 			// end of case what 
					
				if ( !returnFlag )
					{
					gOperationCanceledFlag = TRUE;
					
							// If the gAlertID is something other than '0', then		
							// the requested alert box should be displayed to 			
							// allow the user to indicate whether they want to quit	
							// after the current iteration (gAlertReturnCode = 1)		
							// cancel the quit request (gAlertReturnCode = 2), or		
							// quit immediately (gAlertReturnCode = 3).						
							
					if (gAlertId != 0)
						{
								// Turn default button off in status dialog box if it is
								// showing.
						
						if (gStatusDialogPtr != NULL)		
							SetDLogControlHilite (gStatusDialogPtr, 1, 255);
				
						gAlertReturnCode = DisplayAlert (
							gAlertId, kCautionAlert, gAlertStrID, gAlertStringNumber, 0, NULL);
						
						if (gStatusDialogPtr != NULL)		
							SetDLogControlHilite (gStatusDialogPtr, 1, 0);
						
						if (gAlertReturnCode == 3)
								// Quit immediately.																	
																								return (FALSE);
																					
						if (gAlertId == kContinueStopAlertID && gAlertReturnCode == 2)	
								// Quit immediately.								
																								return (FALSE);
							
						}		// end "if (gAlertId != 0)"
						
					else		// gAlertId == 0
							// Quit immediately.																	
																								return (FALSE);
					
					gOperationCanceledFlag = FALSE;
					
					if (gAlertReturnCode == 2)
								// Cancel the quit request.
						gAlertReturnCode = 0;
						
					returnFlag = TRUE;
																					
					}		// end "if ( !returnFlag )" 
					
				}		// end "if (haveEvent)" 
				
			}		while (haveEvent);
			
		}		// end "if (eventMask != 0)"

			// Spin the cursor if needed.

	if (gPresentCursor == kSpin)
		{
		gSpinCursorIndex++;
		if (gSpinCursorIndex > kSpinEnd)
			gSpinCursorIndex = kSpin;

		SetCursor (gCursorPtr[gSpinCursorIndex]);

		}		// end "if (gPresentCursor == kSpin)"
		
	gNextTime = TickCount() + gTimeOffset;
		
	return (TRUE);
		
}		// end "CheckSomeEvents"

	

//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ClearCoordinates
//
//	Software purpose:	The purpose of this routine is to clear the coordinates
//							in the coordinate window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			DoActivateEvent in multiSpec.c
//							FixCursor in multiSpec.c
//							CloseCoordinateWindow in windows.c
//
//	Coded By:			Larry L. Biehl			Date: 07/15/1992
//	Revised By:			Larry L. Biehl			Date: 11/02/2000	

void	ClearCoordinates (void)

{

	if (GetCoordinateHeight(gActiveImageWindowInfoH) > 0)
		{
//		LoadDItemString (gCoordinatePtr, 10, (Str255*)"\p");
//		LoadDItemString (gCoordinatePtr, 12, (Str255*)"\p");
//		LoadDItemString (gCoordinatePtr, 14, (Str255*)"\p");
//		LoadDItemString (gCoordinatePtr, 16, (Str255*)"\p");
		
		InvalidateWindow (gActiveImageWindow, kCoordinateCursorArea, FALSE);
		
		}		// end "if (GetCoordinateHeight(gActiveImageWindowInfoH) > 0)" 

}		// end "ClearCoordinates" 
				


//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CopyOffScreenImage
//
//	Software purpose:	This routine copies an off screen bit/pix map
//							to the specified window base on the routine 
//							defined by copyType.
//
//	Parameters in:		copyType - See descriptions in cases for switch statement below.	
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			DoUpdateEvent
//
//	Coded By:			Larry L. Biehl			Date: 08/31/1988
//	Revised By:			Larry L. Biehl			Date: 08/04/2013		

void CopyOffScreenImage (
				WindowPtr							theWindow, 
				RgnHandle							invalidRgn, 
				Rect*									inSourceRect, 
				SInt16								copyType,
				Boolean								fromImageWindowUpdateFlag)	
																					

{
	Rect									destinationRect,
											sourceRect,
											windowRect;
	
	double								magnification;
	
	DisplaySpecsPtr					displaySpecsPtr;
	WindowInfoPtr						windowInfoPtr;
										
	GWorldFlags							gWorldFlags;

	Handle								displaySpecsHandle,
											imageBaseAddressH,
											offScreenMapH,
											windowInfoHandle;
											
	SInt32								hOrigin,
											imageTopOffset,
											legendWidth,
											numberImageOverlays,
											numberVectorOverlays,
											titleHeight,
											vOrigin;
	
	SInt16								grafPortType,
											windowCode;
											
	Boolean								drawBaseImageFlag,
											drawVectorOverlaysFlag,
											projectWindowFlag;

	SignedByte							windowHandleStatus;
	
	#if TARGET_API_MAC_CARBON
		CGContextRef						context = NULL;
	#endif		// TARGET_API_MAC_CARBON
	
			// Intialize local variables.													
	
	windowInfoHandle = (Handle)GetWRefCon(theWindow);
	windowInfoPtr = (WindowInfoPtr)*windowInfoHandle;
	
	imageBaseAddressH = windowInfoPtr->imageBaseAddressH;
	
	if (imageBaseAddressH || windowInfoPtr->offscreenGWorld) 
		{	
		windowInfoPtr = (WindowInfoPtr)GetHandleStatusAndPointer(
																windowInfoHandle,
																&windowHandleStatus,
																kNoMoveHi);
		
		projectWindowFlag = windowInfoPtr->projectWindowFlag;
		
		legendWidth =  windowInfoPtr->legendWidth;
		if (!windowInfoPtr->showLegend)
			legendWidth = 0;
		
		titleHeight = windowInfoPtr->titleHeight;
		
		imageTopOffset = windowInfoPtr->imageTopOffset;
		if (copyType == kClipboardCopy)
			imageTopOffset = titleHeight;
		
		grafPortType = windowInfoPtr->grafPortType;
		numberVectorOverlays = windowInfoPtr->numberOverlays;
		drawVectorOverlaysFlag = windowInfoPtr->drawVectorOverlaysFlag;
		numberImageOverlays = windowInfoPtr->numberImageOverlays;
		drawBaseImageFlag = windowInfoPtr->drawBaseImageFlag;
		offScreenMapH = windowInfoPtr->offScreenMapHandle;
							
				// Get handle and pointer to display specification for window		
			
		displaySpecsHandle = ((WindowInfoPtr)*windowInfoHandle)->displaySpecsHandle;
		displaySpecsPtr = (DisplaySpecsPtr)*displaySpecsHandle;
		
		magnification = displaySpecsPtr->magnification;
		vOrigin = displaySpecsPtr->origin[kVertical];
		hOrigin = displaySpecsPtr->origin[kHorizontal];

				// Set window code depending on whether this is an image window update
				// or a clipboard/print update.
					
		windowCode = kToImageWindow;
		if (copyType == kClipboardCopy)
			windowCode = kToClipboardWindow;
		
				// Set clip region of window to the image portion.	
	
		GetWindowClipRectangle (theWindow, kImageFrameArea, &windowRect);					
		
		if (copyType != kClipboardCopy)
			ClipRect (&windowRect);

		#if TARGET_API_MAC_CARBON				
			if (gOSXCoreGraphicsFlag && imageBaseAddressH != NULL)
				{
				LongRect								lDestinationRect;
				Rect									imageRect;
//				CGContextRef						context;
				CGRect								rectangle;
				
				float									vOffset;
				
				UInt32								vMagnificationSize;
//														hMagnificationSize;
/*											
				PixMapHandle        				portPixMap;
				Rect									pixRect;
				Rect									portRect;
				CGrafPtr								cGrafPtr;
				
				SInt16								portHOrigin;
//														portVOrigin;
*/											
											
				displaySpecsPtr = (DisplaySpecsPtr)*displaySpecsHandle;
				
//				hMagnificationSize = (UInt32)(
//						magnification * displaySpecsPtr->imageDimensions[kHorizontal] + .5);
						
				vMagnificationSize = (UInt32)(
						magnification * displaySpecsPtr->imageDimensions[kVertical] + .5);
		
						// Branch to appropriate case for copyType.							
						
				switch (copyType)
					{
					case kClipboardCopy:
								// Map sourceRect to destination that starts at (0,0)			
								// This is for copy to clipboard.									
								
						sourceRect = *inSourceRect;
						
						lDestinationRect.top = titleHeight + 
							(SInt32)((sourceRect.bottom - sourceRect.top) * magnification);
						lDestinationRect.left = legendWidth;
						lDestinationRect.bottom = 0; 
						lDestinationRect.right = legendWidth + (SInt32)
										((sourceRect.right - sourceRect.left) * magnification);
						break;
						
					case kSourceCopy:
								// Map to wherever sourceRect * magnification 					
								// indicates.  This allows one to draw parts of the 			
								// window at a time.														
								
//						sourceRect = *inSourceRect;
						
//						destinationRect.bottom = 0;
//						destinationRect.top = (SInt16)vMagnificationSize;
//						destinationRect.left = (SInt16)legendWidth;
//						destinationRect.right = (SInt16)hMagnificationSize;

						ExitMultiSpec (TRUE);
								
						break;
						
					case kScrollCopy:
								// Copy portion of offscreen image map that fits in the 		
								// invalid region of the window. 	This is from a scroll 	
								// operation.																
								
//						GetRegionBounds(invalidRgn, &destinationRect);
								
					case kDestinationCopy:
								// Copy portion of offscreen map that fits within the			
								// input destination area of the window.  This is from		
								// a window update event.												
						
//						if (copyType == kDestinationCopy)
//							destinationRect = *inSourceRect;
																		
						lDestinationRect.bottom = 0;
						lDestinationRect.top = displaySpecsPtr->imageDimensions[kVertical];
						lDestinationRect.left = (SInt32)legendWidth;
						lDestinationRect.right = (SInt32)(legendWidth + 
													displaySpecsPtr->imageDimensions[kHorizontal]);
							
						break;
					
					}		// end "switch (copyType)"
/*					
				cGrafPtr = GetWindowPort(theWindow);
				portPixMap = GetPortPixMap (cGrafPtr);
				
				GetPortBounds (cGrafPtr, &portRect);
				GetPixBounds (portPixMap, &pixRect);
				
				portHOrigin = -1000;
				
				SetOrigin (portHOrigin, 0);
				
				GetPortBounds (cGrafPtr, &portRect);
				GetPixBounds (portPixMap, &pixRect);
*/			
 				CreateCGContextForPort (GetWindowPort(theWindow), 
													&context);
													
				gCGContextSetShouldAntialiasPtr (context, 0);
				
				gCGContextSetInterpolationQualityPtr (context, kCGInterpolationNone);

						// Save the graphics state so we can start fresh each time.
						
//				gCGContextSaveGStatePtr (context);

						// Get image and window sizes.
				
				GetWindowClipRectangle (theWindow, kFrameArea, &windowRect);
				GetWindowClipRectangle (theWindow, kImageArea, &imageRect);
				
						// Save the destination rectangle for calls that may use quickdraw 
						// bitmaps (DrawShapeFiles).
				
				destinationRect = imageRect;
				
						// Set clipping for core graphics context.
															
				rectangle.origin.x = legendWidth;
				rectangle.origin.y = windowRect.bottom - imageRect.bottom;
				rectangle.size.width = imageRect.right - imageRect.left;
				rectangle.size.height = imageRect.bottom - imageRect.top;
				
				gCGContextClipToRectPtr (context, rectangle);
				
						// Translate to QuickDraw coordinate system
				
				vOffset = (float)(windowRect.bottom - imageRect.bottom);
				if (vMagnificationSize + imageTopOffset + kSBarWidth >= windowRect.bottom)
					vOffset -= (float)(vMagnificationSize - 
								(imageRect.bottom - imageRect.top + vOrigin * magnification));
			
				gCGContextTranslateCTMPtr (
								context, 
								(float)(legendWidth - hOrigin * magnification), 
								vOffset);
								
				gCGContextScaleCTMPtr (context, 
												(float)magnification, 
												(float)magnification);

						// Determine the location where the image will be drawn in userspace.
						
//				rectangle = gCGRectMakePtr (legendWidth,
//														0,
//														lDestinationRect.right - lDestinationRect.left,
//														lDestinationRect.top - lDestinationRect.bottom);
															
				rectangle.origin.x = 0;
				rectangle.origin.y = 0;
				rectangle.size.width = lDestinationRect.right - lDestinationRect.left;
				rectangle.size.height = lDestinationRect.top - lDestinationRect.bottom;
				
				if (drawBaseImageFlag)
					{								
							// Draw the image to the core graphics context.
							
					gCGContextDrawImagePtr (context, 
														rectangle, 
														windowInfoPtr->cgInfo.imageRef);
					
					}		// end "if (drawBaseImageFlag)"
					
				if (numberImageOverlays > 0)
					{
					DrawImageOverlays (theWindow, 
												windowInfoHandle,
												context,
												&rectangle,
												&destinationRect,
												&windowRect,
												windowCode);
												
					}		// end "if (numberImageOverlays > 0)"
												
//				gCGContextFlushPtr (context);
//				gCGContextReleasePtr (context);
				
				}		// if (gOSXCoreGraphicsFlag && imageBaseAddressH != NULL)
				
			else		// !gOSXCoreGraphicsFlag || imageBaseAddressH != NULL
				{
		#endif		// TARGET_API_MAC_CARBON
					// Branch to appropriate case for copyType.							
					
			switch (copyType)
				{
				case kClipboardCopy:
							// Map sourceRect to destination that starts at (0,0)			
							// This is for copy to clipboard.									
							
					sourceRect = *inSourceRect;
					
					destinationRect.top = titleHeight;
					destinationRect.left = legendWidth;
					destinationRect.bottom = titleHeight + (SInt16)
								((sourceRect.bottom - sourceRect.top) * magnification); 
					destinationRect.right = legendWidth + (SInt16)
								((sourceRect.right - sourceRect.left) * magnification);
					break;
					
				case kSourceCopy:
							// Map to wherever sourceRect * magnification 					
							// indicates.  This allows one to draw parts of the 			
							// window at a time.														
							
//					sourceRect = *inSourceRect;
							
//					destinationRect.top = 
//										(SInt16)(sourceRect.top * magnification + imageTopOffset);
//					destinationRect.left = 
//										(SInt16)(sourceRect.left * magnification + legendWidth);
//					destinationRect.bottom = 
//										(SInt16)(sourceRect.bottom * magnification + imageTopOffset); 
//					destinationRect.right = 
//										(SInt16)(sourceRect.right * magnification + legendWidth);

					ExitMultiSpec (TRUE);
							
					break;
					
				case kScrollCopy:
							// Copy portion of offscreen image map that fits in the 		
							// invalid region of the window. 	This is from a scroll 	
							// operation.																
							
					GetRegionBounds(invalidRgn, &destinationRect);
							
				case kDestinationCopy:
							// Copy portion of offscreen map that fits within the			
							// input destination area of the window.  This is from		
							// a window update event.												
					
					if (copyType == kDestinationCopy)
						destinationRect = *inSourceRect;
						
							// Make certain that the title and legend is not included.				
							
					destinationRect.top = MAX (imageTopOffset, destinationRect.top);
					destinationRect.left = MAX (legendWidth, destinationRect.left);
					
							// Make certain that the scroll bars are not included.		
							
					destinationRect.right = 
											MIN (windowRect.right, destinationRect.right);
					destinationRect.bottom = 
											MIN (windowRect.bottom, destinationRect.bottom);
					
					sourceRect.top = (SInt16)(
						(destinationRect.top - imageTopOffset)/magnification + vOrigin);
					sourceRect.left = (SInt16)(
						(destinationRect.left - legendWidth)/magnification + hOrigin);
					sourceRect.bottom = (SInt16)(
						(destinationRect.bottom - imageTopOffset)/magnification + vOrigin + .9999);
					sourceRect.right = (SInt16)(
						(destinationRect.right - legendWidth)/magnification + hOrigin + .9999);
						
					displaySpecsPtr = (DisplaySpecsPtr)*displaySpecsHandle;
					
					if (sourceRect.bottom > displaySpecsPtr->imageDimensions[kVertical])
						sourceRect.bottom = (SInt16)displaySpecsPtr->imageDimensions[kVertical];
					
					if (sourceRect.right > displaySpecsPtr->imageDimensions[kHorizontal])
						sourceRect.right = (SInt16)displaySpecsPtr->imageDimensions[kHorizontal];
						
							// Make certain that source to destination is correct.  The	
							// source may have been truncated to a larger value.			
						
					destinationRect.top = (SInt16)( imageTopOffset + magnification *
																(sourceRect.top - vOrigin) );
					destinationRect.bottom = (SInt16)( imageTopOffset + magnification *
																(sourceRect.bottom - vOrigin) );
					destinationRect.left = (SInt16)( legendWidth + magnification *
																	(sourceRect.left - hOrigin) );
					destinationRect.right = (SInt16)( legendWidth + magnification *
																	(sourceRect.right - hOrigin) );
		 	
					break;
				
				}		// end "switch (copyType)"	
		
			gWorldFlags = GetPixelsState ((PixMapHandle)offScreenMapH);
			if (drawBaseImageFlag && LockPixels ((PixMapHandle)offScreenMapH))
				{
				CopyBits ( (BitMap*)*offScreenMapH, 
									GetPortBitMapForCopyBits ( GetWindowPort(theWindow) ),
									&sourceRect, 
									&destinationRect, 
									srcCopy, 
									NIL);
				
				if ( !(gWorldFlags & pixelsLocked) )							
					UnlockPixels ( (PixMapHandle)offScreenMapH );
/*	
				#if TARGET_API_MAC_CARBON	
					if (QDIsPortBuffered (GetWindowPort(theWindow)))
						{
//						UInt32	finalTicks;
						QDFlushPortBuffer (GetWindowPort(theWindow), NULL);
//						Delay (10, &finalTicks);
						
						}		// end "if (QDIsPortBuffered (GetWindowPort(theWindow)))"
				#endif	// TARGET_API_MAC_CARBON
*/				
				}		// end "if drawBaseImageFlag && (LockPixels ((PixMapHandle)offScreenMapH))" 
				
					// Do image overlay.
			
			if (numberImageOverlays > 0)
				DrawImageOverlays (theWindow, 
											windowInfoHandle,
											NULL,
											NULL,
											&destinationRect,
											&sourceRect,
											windowCode);
											
			}		// end "else !gOSXCoreGraphicsFlag" 	
		
		MHSetState (windowInfoHandle, windowHandleStatus);
			
				// Draw the vector overlays if they exist.
			
		if (drawVectorOverlaysFlag && numberVectorOverlays > 0)
			DrawArcViewShapes (theWindow, 
										windowInfoHandle, 
										&destinationRect, 
										windowCode,
										NULL);
								
		#if TARGET_API_MAC_CARBON		
			else	// !drawVectorOverlaysFlag || numberVectorOverlays <= 0
				{
				if (QDIsPortBuffered (GetWindowPort(theWindow)))
					QDFlushPortBuffer (GetWindowPort(theWindow), NULL);
				}		// end "else !drawVectorOverlaysFlag || numberVectorOverlays <= 0" 		
		#endif	// TARGET_API_MAC_CARBON

			 										
				// If this window is the project window, outline the fields if		
				// requested.																		
	
		if (projectWindowFlag)
			OutlineFieldsControl (gProjectInfoPtr->statsWindowMode, 
											theWindow, 
											windowInfoHandle,
											windowCode);
				
				// Draw the selection area, if it exists for this window.			
		
		if (copyType != kClipboardCopy)
			OutlineSelectionArea (theWindow);
			
				// Handle title line for side-by-side images if needed.							
				
		if (!fromImageWindowUpdateFlag && titleHeight > 0)
			{
			destinationRect.top = imageTopOffset - titleHeight;
			destinationRect.bottom = imageTopOffset;
			ClipRect (&destinationRect);
			
			DrawSideBySideTitles (windowInfoHandle, 
											theWindow, 
											&destinationRect,
											windowCode);
			
			}		// end "if (!fromImageWindowUpdateFlag && titleHeight > 0)" 
			
		#if TARGET_API_MAC_CARBON				
			if (context != NULL)
				{				
				gCGContextFlushPtr (context);
				gCGContextReleasePtr (context);
				
				}		// end "if (context != NULL)"
		#endif		// TARGET_API_MAC_CARBON
			
		if (copyType != kClipboardCopy)
			ClipRect (GetWindowPortBounds(theWindow, &gTempRect));
						
		}		// end "if (imageBaseAddressH || windowInfoPtr->offscreenGWorld)"
		
	else		// imageBaseAddressH == NULL && ...
		{
		GetWindowClipRectangle (theWindow, kImageScrollArea, &windowRect);
  		EraseRect (&windowRect);
		UnhiliteImageWControls (theWindow);
  		
  		}		// end "else imageBaseAddressH == NULL && ..."
		
}		// end "CopyOffScreenImage" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoActivateEvent
//
//	Software purpose:	This routine handles the activate/deactive events.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			main
//
//	Coded By:			Larry L. Biehl			Date: 04/06/1988
// Revised By:			Ravi S. Budruk			Date: 08/16/1988	
//	Revised By:			Larry L. Biehl			Date: 09/08/2006			

void DoActivateEvent (
				WindowPtr							window, 
				Boolean								becomingActive)

{
			//	Local Declarations 																

	Rect									rect;
	
	GrafPtr								savedPort;
	WindowInfoPtr						windowInfoPtr = NULL;
	
	ControlHandle						controlHandle,
											rootControlHandle;
											
	Handle								windowInfoHandle;
	
	#if use_mlte_for_text_window
		TXNObject							textH;
	#endif
	#if !use_mlte_for_text_window
		WEReference							textH;
	#endif
	
	SInt32								startControlHilite,
											coordinateHeight,
											refCon,
											windowMenuItem,
											windowType;
											
	UInt16								index,
											numberControls;
											
	SignedByte							handleStatus;

		   
		   // Check input parameters.  Exit if not valid.								
		   
	if (window == NULL)
																							return;
																							 		
	GetPort (&savedPort);
	SetPortWindowPort (window);
		    		    				
			// Set clip region of window to entire port to draw 						
			// growicon and then reset clip region of the window						
			// to scroll bar edges.																
			
	ClipRect (GetWindowPortBounds(window, &gTempRect));
     	
     		// Get handle to additional window information and lock it.				
     		
	windowInfoHandle = (Handle)GetWRefCon(window);
	
			// Get windowType and text handle for window if MultiSpec window		
	
	windowMenuItem = 0;
	windowType = 0;
	textH = NULL;
	coordinateHeight = 0;
	if (windowInfoHandle != NULL)
		{	
		handleStatus = HGetState (windowInfoHandle);
		HLock (windowInfoHandle);
		windowInfoPtr = (WindowInfoPtr)*windowInfoHandle;
		
		windowMenuItem = windowInfoPtr->windowMenuItem;
		windowType = windowInfoPtr->windowType;
		textH = windowInfoPtr->textH;
		coordinateHeight = windowInfoPtr->coordinateHeight;
		
		}		// end "if (windowInfoHandle != NULL)" 
     		
  	if (coordinateHeight > 0)
		InvalidateWindow (window, kCoordinateArea, FALSE);

	if (becomingActive)
		{					
     			// Initialize flag for indicating if an off screen bit/pix map		
     			// exits for the active window.  It will be set TRUE if window is	
     			// image type and off screen map pointer is not NULL					
     		
     	gActiveOffscreenMapExists = FALSE;
		     	
     			// Reset the applicaton global window variables							
     			
     	gTheActiveWindow 		= window;
     	gActiveWindowInfoH 	= windowInfoHandle;
     	gActiveWindowType		= windowType;
     	gActiveLegendWidth 	= 0;
     	if (windowInfoPtr != NULL)
     		{
     		if (windowInfoPtr->showLegend)
     			gActiveLegendWidth = windowInfoPtr->legendWidth;
     			
     		}		// end "if (windowInfoPtr != NULL)" 
     	
				// Window is becoming active.  Go to the respective window			
				// activate routine.																
		
		if (windowType == kOutputWindowType) 
			DoOutputWActivateEvent (window, textH, becomingActive);
		
		else if (windowType == kImageWindowType || windowType == kThematicWindowType)
			DoImageWActivateEvent (window, windowInfoHandle);
		
		else if (windowType == kStatisticsWindowType) 
			DoStatWActivateEvent ();
		
		else if (windowType == kPaletteWindowType) 
			DoPaletteWActivateEvent ();
		
		else if (windowType == kGraphicsWindowType) 
			DoGraphWActivateEvent (window, windowInfoHandle, becomingActive);
 		
 				// Make certain that reseting the graf port will be to 'window'	
 				 		
 		savedPort = (GrafPtr)GetWindowPort (window);
 		
		}		// end "becomingActive" 
     
	else		// Window is becoming inactive												
		{
		if (windowType == kOutputWindowType && textH != NULL) 
			DoOutputWActivateEvent (window, textH, becomingActive);
		
		else if (windowType == kGraphicsWindowType) 
			DoGraphWActivateEvent (window, windowInfoHandle, becomingActive);
		
		else if (windowType == kImageWindowType || 
					windowType == kThematicWindowType)
			{		
					//	Hide window controls for multispectral image, Thematic 		
					// image and output windows.	

			if (gAppearanceManagerFlag)
				{
				startControlHilite = kHorizontalControl;
				if (gOSXFlag)
					startControlHilite = -1;
				
				GetRootControl(window, &rootControlHandle);
				
				if (rootControlHandle != NULL)
					{
					CountSubControls (rootControlHandle, &numberControls);
					for (index=numberControls; index>=1; index--)
						{
						GetIndexedSubControl(rootControlHandle, index, &controlHandle);
						refCon = (SInt16)GetControlReference (controlHandle);
			
						if (refCon > startControlHilite)
							HiliteControl (controlHandle, 255);
						
						else		// refCon <= startControlHilite
							HideControl (controlHandle);
						
								// Validate the control rectangle area so that it is not		
								// updated.  No image or text will be drawn in the control	
								// area even if the window is not active.							
						
						#if !TARGET_API_MAC_CARBON
							HLock ( (Handle)controlHandle );	
							if ( IsControlVisible(controlHandle) )
								ValidWindowRect (window, GetControlBounds (controlHandle, &gTempRect));
							HUnlock ( (Handle)controlHandle );
						#endif	// !TARGET_API_MAC_CARBON
			     			
			     		}		// end "for (index=numberControls; index>=1; index--)"
			     		
			   	}		// end "if (rootControlHandle != NULL)"
	     		
	     		}		// end "if (gAppearanceManagerFlag)" 
	     		
			#if !TARGET_API_MAC_CARBON
		     	else		// !gAppearanceManagerFlag
		     		{
					controlHandle = (ControlHandle)((WindowPeek)window)->controlList;
					while (controlHandle != NULL)
						{
						refCon = (SInt16)GetControlReference (controlHandle);
			
						if (refCon > kHorizontalControl)
							HiliteControl (controlHandle, 255);
						
						else		// refCon <= kHorizontalControl
							HideControl (controlHandle);
						
								// Validate the control rectangle area so that it is not		
								// updated.  No image or text will be drawn in the control	
								// area even if the window is not active.							
						
						HLock ( (Handle)controlHandle );	
						if ( (*controlHandle)->contrlVis )
							ValidWindowRect (window, GetControlBounds (controlHandle, &gTempRect));
						HUnlock ( (Handle)controlHandle );
						 
			     		controlHandle = (*controlHandle)->nextControl;
			     			
			     		}		// end "while (controlHandle != NULL)"
		     		
		     		}		// end "else !gAppearanceManagerFlag"
			#endif	// !TARGET_API_MAC_CARBON
	     		
	     	}		// end "else if (windowType == kImageWindowType ||..." 
			
				// Do deactivate for list if statistics window.							
				
		else if (windowType == kStatisticsWindowType)  
			LActivate (FALSE, gStatisticsListHandle);
     	
     	if (gTheActiveWindow == window)
     		{	
	     			// Set flag indicating if an off screen bit/pix map exits for 		
	     			// the active window to FALSE, since there is no active window.	
	     		
	     	gActiveOffscreenMapExists = FALSE;

	     			// Reset the application global window variables.						
	     			
	     	gTheActiveWindow 	= NULL;
	     	gActiveWindowInfoH = NULL;
	     	gActiveWindowType		= 0;
	     	
	     	}		// end "if (gTheActiveWindow == window)"
		
		}		// end "else - window is becoming inactive 
		
	gUpdateFileMenuItemsFlag = TRUE; 
	gUpdateEditMenuItemsFlag = TRUE; 
	gUpdateProjectMenuItemsFlag = TRUE;
	gUpdatePaletteMenuItemsFlag = TRUE;
	gUpdateWindowsMenuItemsFlag = TRUE;
		
	if (windowType == kOutputWindowType || 
				windowType == kImageWindowType || 
						windowType == kThematicWindowType)
		{
				// Draw grow icon	
		
				// Set clip region to include scroll bars							
			
		GetWindowClipRectangle (window, kImageScrollArea, &rect);	
		ClipRect (&rect);		
		DrawGrowIcon (window);
		
				// Reset clip region to inside of scroll bars							
			
		GetWindowClipRectangle (window, kImageFrameArea, &rect);
				
		rect.left = 0;		// Make certain the legend list is included.		
		ClipRect (&rect);
		
		}		// end "if (windowType == kOutputWindowType || ..." 
		
	else if (windowType == kGraphicsWindowType)
		{
		GetWindowPortBounds (window, &gTempRect);
		gTempRect.top = (SInt16)(gTempRect.bottom - 15);
		gTempRect.left = (SInt16)(gTempRect.right - 15);
		ClipRect( &gTempRect );
		DrawGrowIcon( window );
		ClipRect (GetWindowPortBounds(window, &gTempRect));
		
		}		// end "if (windowType == kGraphicsWindowType)" 
		
	if (windowInfoHandle != NULL)
		HSetState (windowInfoHandle, handleStatus);
	
	SetPort (savedPort);

}		// end "DoActivateEvent"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		DoCoordinateViewUpdateEvent
//
//	Software purpose:	This routine handles updating the coordinate view to make
//							sure that it is drawn correctly.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			DoImageWUpdateEvent in multiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 11/01/2000
//	Revised By:			Larry L. Biehl			Date: 04/30/2012			

void DoCoordinateViewUpdateEvent (
				WindowPtr							window, 
				Handle								windowInfoHandle)

{
	Rect									coordinateRect;
	
	RGBColor								rgbColor,
											savedForeGroundColor;
											
	SInt16								scaleCursorStart,
											selectionCursorStart;
											
	Boolean								activeFlag;
											
	
			// Intialize local variables.
			
	activeFlag = (window == FrontWindow() && gActiveOffscreenMapExists);						
	GetWindowClipRectangle (window, kCoordinateArea, &coordinateRect);
	coordinateRect.bottom--;
	
	selectionCursorStart = GetCoordinateViewSelectionStart (windowInfoHandle) - 5;
	scaleCursorStart = GetCoordinateViewScaleStart (windowInfoHandle) - 5; 
		
	GetForeColor (&savedForeGroundColor);
		
			// Force the coordinate view background to be gray.
			
	RGBForeColor (&gCoordinateViewBackgroundColor);
	PaintRect (&coordinateRect);
	
			// Draw a black line at bottom of Coordinate view 
	
	ForeColor (blackColor);
	MoveTo( coordinateRect.left, coordinateRect.bottom );
	LineTo( coordinateRect.right, coordinateRect.bottom ); 
	
			// Adjust coordinate rectangle for later drawing.
			
	InsetRect (&coordinateRect, -1, -1); 
	coordinateRect.right--;
	coordinateRect.bottom -= 2;
	
	if (activeFlag)
		{
				// Draw the shadow affects around the coordinate view
				
				// Draw the white bevel line on left and top 
			                        
		rgbColor.red = 0xFFFF;
		rgbColor.green = 0xFFFF;
		rgbColor.blue = 0xFFFF;
		RGBForeColor (&rgbColor);	
			
		MoveTo( coordinateRect.left, coordinateRect.bottom );
		LineTo( coordinateRect.left, coordinateRect.top ); 
		LineTo( coordinateRect.right, coordinateRect.top ); 
				
				// Draw white part of the cursor and selection separator
				 
		MoveTo( selectionCursorStart, coordinateRect.top+3 );
		LineTo( selectionCursorStart, coordinateRect.bottom-3 ); 
				
				// Draw white part of the scale separator
				
		MoveTo( scaleCursorStart, coordinateRect.top+3 );
		LineTo( scaleCursorStart, coordinateRect.bottom-3 ); 
		
				// Draw the gray bevel line on right and bottom  
			
		rgbColor.red = 0xAAAA;
		rgbColor.green = 0xAAAA;
		rgbColor.blue = 0xAAAA;
		
		RGBForeColor (&rgbColor);
		coordinateRect.right--;
		MoveTo( coordinateRect.right, coordinateRect.top+1 );
		LineTo( coordinateRect.right, coordinateRect.bottom ); 
		LineTo( coordinateRect.left+1, coordinateRect.bottom ); 
				
				// Draw gray part of the cursor and selection separator
				 
		MoveTo( selectionCursorStart-1, coordinateRect.top+3 );
		LineTo( selectionCursorStart-1, coordinateRect.bottom-3 ); 
				
				// Draw gray part of the scale separator
				 
		MoveTo( scaleCursorStart-1, coordinateRect.top+3 );
		LineTo( scaleCursorStart-1, coordinateRect.bottom-3 ); 
		
		}		// end "if (activeFlag)"
		
	else		// !activeFlag
		{
		rgbColor.red = 0xAAAA;
		rgbColor.green = 0xAAAA;
		rgbColor.blue = 0xAAAA;
		RGBForeColor (&rgbColor);
		
				// Draw gray line for cursor and selection separator lines  
		
		MoveTo( selectionCursorStart-1, coordinateRect.top+3 );
		LineTo( selectionCursorStart-1, coordinateRect.bottom-3 ); 
		
				// Draw gray line for the scale separator  
		
		MoveTo( scaleCursorStart-1, coordinateRect.top+3 );
		LineTo( scaleCursorStart-1, coordinateRect.bottom-3 ); 
		
		}		// end "else !activeFlag"
	
			// Reset the foreground color back to black.
	
	if (activeFlag)	
		RGBForeColor (&savedForeGroundColor);
	
			// Draw the line direction indicator
			
	MoveTo(coordinateRect.left+137, coordinateRect.top+10);
	LineTo(coordinateRect.left+137, coordinateRect.top+4); 
	MoveTo(coordinateRect.left+136, coordinateRect.top+9);
	LineTo(coordinateRect.left+138, coordinateRect.top+9); 
	MoveTo(coordinateRect.left+136, coordinateRect.top+5);
	LineTo(coordinateRect.left+138, coordinateRect.top+5); 
	
			// Draw the column direction indicator
			
	MoveTo(coordinateRect.left+134, coordinateRect.top+18);
	LineTo(coordinateRect.left+140, coordinateRect.top+18); 
	MoveTo(coordinateRect.left+135, coordinateRect.top+17);
	LineTo(coordinateRect.left+135, coordinateRect.top+19);  
	MoveTo(coordinateRect.left+139, coordinateRect.top+17);
	LineTo(coordinateRect.left+139, coordinateRect.top+19); 
	
	RGBForeColor (&savedForeGroundColor);
	
}		// end "DoCoordinateViewUpdateEvent"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoImageMouseDnContent
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
// Called By:			DoMouseDnContent in multiSpec
//
//	Coded By:			Larry L. Biehl			Date: 09/09/1988
//	Revised By:			Larry L. Biehl			Date: 07/29/1996			

void DoImageMouseDnContent (
				WindowPtr							theWindow,
				Point									mousePt)

{
			//	Local Declarations 																
	
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
		  			HandScrollImage (gActiveImageWindow, mousePt);
					MSetCursor (kHand);
					break;
					
				case kArrow:
														
//					GetCoordinateAreaPopupBox (&popupBox, gActiveWindowInfoH);
					
//					if ( PtInRect ( mousePt, &popupBox ) )
//						LegendPopUpMenu (&popupBox);
						
					break;
					
				}		// end "switch (gPresentCursor)" 			
		  		
		  	}		// end "if (theWindow == gActiveImageWindow)"
		  		
		}		// end "else partCode == 0" 

}		// end "DoImageMouseDnContent"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		DoImageWActivateEvent
//
//	Software purpose:	This routine handles the activate/deactivate events
//							for image windows.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			main
//
//	Coded By:			Larry L. Biehl			Date: 04/06/1988
// Revised By:			Ravi S. Budruk			Date: 08/16/1988
//	Revised By:			Larry L. Biehl			Date: 09/08/2006				

void DoImageWActivateEvent (
				WindowPtr							window, 
				Handle								windowInfoHandle)

{
			//	Local Declarations 
			
	double 								magnification;	
	
	WindowInfoPtr						windowInfoPtr;	
											
	Handle								imageBaseAddressH;
	
	
			// Note that windowInfoHandle has already been locked by calling 		
			// routine.																				
			
	windowInfoPtr = (WindowInfoPtr)*windowInfoHandle;
     	
     			// Get handle to off screen image handle. 								
     				
	imageBaseAddressH = windowInfoPtr->imageBaseAddressH;
	
			// Set global flag TRUE if off screen bip/pix map exists.				
			
	if (imageBaseAddressH || windowInfoPtr->offscreenGWorld) 
		gActiveOffscreenMapExists = TRUE;
     			
	gActiveImageWindow = window;
	gActiveImageWindowInfoH = windowInfoHandle;
	
	gActiveImageFileInfoH = windowInfoPtr->fileInfoHandle;
	gActiveImageLayerInfoH = windowInfoPtr->layerInfoHandle;
		    		
			//	Show window controls for image windows	
	
	if (gOSXFlag)
		{
		HiliteControl (windowInfoPtr->verticalControl, 0);
		HiliteControl (windowInfoPtr->horizontalControl, 0);
		
		}		// end "if (gOSXFlag)"
		
	else		// !gOSXFlag
		{		
		ShowControl (windowInfoPtr->verticalControl);
		ShowControl (windowInfoPtr->horizontalControl);
		
		}		// end "else !gOSXFlag"
	
	     // Update thematic legend controls if needed
	     
   if (gActiveWindowType == kThematicWindowType)  	
		UpdateThematicLegendControls (window);
	
			// Update the zoom controls.
	
	magnification = GetWindowImageMagnification (windowInfoPtr);
	UpdateImageZoomControls (window, magnification, TRUE);
	
			// Handle the coordinate window.													
			
 	if (windowInfoPtr->coordinateHeight > 0)
 		{
		UpdateCoordinateViewControls (window);
		
		gCoordinateLineValue = 0;
		gCoordinateColumnValue = 0;
		
		}		// end "if (windowInfoPtr->coordinateHeight > 0)" 
	
			// If graph selection window is open, then indicate that the			
			// parameters for the window need to be updated.							
	
	if (gSelectionGraphViewCPtr != NULL)
		gSelectionGraphViewCPtr->SetCheckIOMemoryFlag(TRUE);

	gUpdateProcessorMenuItemsFlag = TRUE;
	gUpdateWindowsMenuItemsFlag = TRUE;

	//UpdatePaletteWindow ((gPaletteImageWindow != gActiveImageWindow), FALSE);
	
			// Get the blink routine to use.  It was determined in 				
			// UpdatePaletteMenu.				

	DisplaySpecsPtr displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
										windowInfoPtr->displaySpecsHandle, kNoLock, kNoMoveHi);
	if (displaySpecsPtr != NULL)
		gBlinkProcedure = displaySpecsPtr->blinkProcedure;												
	
//	gBlinkProcedure = kBlink1;	
//	GetItemMark (gMultiSpecMenus[kPaletteM], kBlink2MenuItem, &markChar);
//	if (markChar != noMark)
//		gBlinkProcedure = kBlink2;
	
			// Update the global last project selection window.						
			
	if (windowInfoPtr->projectWindowFlag && 
		((SelectionInfoPtr)*windowInfoPtr->selectionInfoHandle)->typeFlag != 0)
		{
		gProjectSelectionWindow = window;
		
				// If the statistics window is showing, then show the selection	
				// for this window.																
		
		LoadNewFieldListBox ();
			
		}		// end "if (windowInfoPtr->projectWindowFlag && ..." 
 	      			
			// Update the selection modal dialog window with the current 			
			// selection area if a selection rectangle exists and the coordiante	
			// window is showing.																
						
	ShowSelection (windowInfoHandle);
	ShowGraphSelection ();
	
}		// end "DoImageWActivateEvent" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoImageWUpdateEvent
//
//	Software purpose:	This routine handles the update events for the image
//							windows.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			main
//
//	Coded By:			Larry L. Biehl			Date: 04/06/1988
//	Revised By:			Larry L. Biehl			Date: 04/04/2003			

void DoImageWUpdateEvent (
				WindowPtr							window, 
				Handle								windowInfoHandle)

{
	Rect									nonImageRect,
											rect;
	
	RgnHandle							nonImageUpdateRegion,
											imageUpdateRegion,
//											newUpdateRegionH,
											rectangleRegion,
											updateRegionH;
											
	Boolean								callCoordinateUpdateFlag,
											callCopyOffScreenImageFlag,
											callTitleUpdateFlag;

		
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
	imageUpdateRegion = gTempRegion2;
	callCopyOffScreenImageFlag = TRUE;
	callCoordinateUpdateFlag = FALSE;
	callTitleUpdateFlag = FALSE;
	
	GetRegionBounds (updateRegionH, &rect);
			
	if (rectangleRegion != NULL && imageUpdateRegion != NULL)
		{
		GetWindowClipRectangle (window, kImageFrameArea, &rect);
		
		LocalToGlobal ( &topLeft(rect) );
		LocalToGlobal ( &botRight(rect) );
		
		RectRgn (rectangleRegion, &rect);
		SectRgn (updateRegionH, rectangleRegion, imageUpdateRegion);
		
		callCopyOffScreenImageFlag = !EmptyRgn (imageUpdateRegion);
		
		if (callCopyOffScreenImageFlag)
			GetRegionBounds (imageUpdateRegion, &rect);
			
		if ( ((WindowInfoPtr)*windowInfoHandle)->coordinateHeight > 0 )
			{
			SetEmptyRgn (imageUpdateRegion);
			SetEmptyRgn (rectangleRegion);
			nonImageUpdateRegion = gTempRegion2;
			
			GetWindowClipRectangle (window, kCoordinateArea, &nonImageRect);
		
			LocalToGlobal ( &topLeft(nonImageRect) );
			LocalToGlobal ( &botRight(nonImageRect) );
		
			RectRgn (rectangleRegion, &nonImageRect);
			SectRgn (updateRegionH, rectangleRegion, nonImageUpdateRegion);
		
			callCoordinateUpdateFlag = !EmptyRgn (nonImageUpdateRegion);
			
			}		// end "if ( ...->coordinateHeight > 0 )"
			
		if ( ((WindowInfoPtr)*windowInfoHandle)->titleHeight > 0 )
			{
			SetEmptyRgn (imageUpdateRegion);
			SetEmptyRgn (rectangleRegion);
			nonImageUpdateRegion = gTempRegion2;
			
			GetWindowClipRectangle (window, kTitleArea, &nonImageRect);
		
			LocalToGlobal ( &topLeft(nonImageRect) );
			LocalToGlobal ( &botRight(nonImageRect) );
		
			RectRgn (rectangleRegion, &nonImageRect);
			SectRgn (updateRegionH, rectangleRegion, nonImageUpdateRegion);
		
			callTitleUpdateFlag = !EmptyRgn (nonImageUpdateRegion);
			
			if (callTitleUpdateFlag)
				GetRegionBounds (nonImageUpdateRegion, &nonImageRect);
			
			}		// end "if ( ...->titleHeight > 0 )"
		
		}		// end "if (rectangleRegion != NULL && ..." 
	
	DisposeRgn (updateRegionH);
	
	if (imageUpdateRegion != NULL)
		SetEmptyRgn (imageUpdateRegion);
	
	if (rectangleRegion != NULL)
		SetEmptyRgn (rectangleRegion);
	
	BeginUpdate (window);
		
			// Update the coordinate area.
			
	if (callCoordinateUpdateFlag)
		{		
		ClipRect (GetWindowPortBounds(window, &gTempRect));
	
		DoCoordinateViewUpdateEvent (window, windowInfoHandle);
		
		DrawCursorCoordinates (windowInfoHandle);
		
		ShowSelection (windowInfoHandle);
		
		DrawScaleInformation (windowInfoHandle);
		
		DrawOneControl ( 
				((WindowInfoPtr)*windowInfoHandle)->coordinateUnitsControl);
		DrawOneControl ( 
				((WindowInfoPtr)*windowInfoHandle)->coordinateViewAreaUnitsControl);
				
		}		// end "if (callCoordinateUpdateFlag)"
	
	if (callCopyOffScreenImageFlag)
		{
		GlobalToLocal ( &topLeft(rect) );
		GlobalToLocal ( &botRight(rect) );
		CopyOffScreenImage (window, NIL, &rect, kDestinationCopy, kFromImageUpdate);
		
		}		// end "if (callCopyOffScreenImageFlag)" 
		
//	else		// !callCopyOffScreenImageFlag
	if (callTitleUpdateFlag)
		{
				// Check if image titles need to be drawn.
				
//		SInt16 titleHeight = ((WindowInfoPtr)*windowInfoHandle)->titleHeight;
		
//		if (titleHeight > 0)
//			{
			GlobalToLocal ( &topLeft(nonImageRect) );
			GlobalToLocal ( &botRight(nonImageRect) );
			
//			rect.top = ((WindowInfoPtr)*windowInfoHandle)->coordinateHeight;
//			rect.bottom = rect.top + titleHeight;
			ClipRect (&nonImageRect);
		
			DrawSideBySideTitles (windowInfoHandle, window, &nonImageRect, kToImageWindow);
				
//			}		// end "if (titleHeight > 0)"
		
		}		// end "if (callTitleUpdateFlag)g"	

			// Draw the grow icon and the controls.	
	
	GetWindowClipRectangle (window, kFrameArea, &rect);
	ClipRect (&rect);
	DrawControls (window);
	
	GetWindowClipRectangle (window, kImageScrollArea, &rect);
	ClipRect (&rect);
	DrawGrowIcon (window);
	
	EndUpdate (window);
	    				
			// Set clip region of window at scroll bar edges. 			
	
	GetWindowClipRectangle (window, kImageFrameArea, &rect);
	rect.top = 0;
	ClipRect (&rect);
	
}		// end "DoImageWUpdateEvent" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		DoMouseDnContent
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
//	Revised By:			Larry L. Biehl			Date: 09/08/2006			

void DoMouseDnContent (
				WindowPtr							theWindow)

{
			//	Local Declarations 																
	
	Point									mousePt;
	
	SInt16								windowType;
	
	Boolean								tempFlag;
	

			// Save the location of the cursor on mouse down							
			
	if (theWindow == NULL)
																							return;
			
	mousePt = gEventRecord.where;
	gNewSelectedWindowFlag = FALSE; 
	 				
	if (theWindow != FrontWindow())
		{
		SelectWindow (theWindow);
		if (theWindow == gOutputWindow)
																							return;
		tempFlag = CheckSomeEvents (activMask+updateMask);
		FixCursor ();
		gNewSelectedWindowFlag = TRUE; 
		
		}		// end "if (theWindow != FrontWindow())" 
		
	if (gActiveWindowInfoH == NULL)
																							return;
		
			// Convert the mouse location from global to local coordinates			
			
	GlobalToLocal (&mousePt);
		
	windowType = ((WindowInfoPtr)*gActiveWindowInfoH)->windowType;
	
	switch (windowType)
		{
		case kImageWindowType:
			DoImageMouseDnContent (theWindow, mousePt);
			break;

		case kThematicWindowType:
			DoThematicMouseDnContent (theWindow, mousePt);
			break;
				
		case kOutputWindowType:
			DoOutputMouseDnContent (theWindow, mousePt);
   		break;
   				
   	case kStatisticsWindowType:
			DoStatisticsMouseDnContent (theWindow, mousePt);
   		break;
   				
   	case kGraphicsWindowType:
			DoGraphWMouseDnContent (theWindow, mousePt);
   		break;
   				
   	}		// end "switch (windowType)"
   	
}		// end "DoMouseDnContent" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		DoMouseDnGrow
//
//	Software purpose:	This routine handles mouse down events in the
//							grow box.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			main
//
//	Coded By:			Larry L. Biehl			Date: 10/04/1991
//	Revised By:			Larry L. Biehl			Date: 12/18/2008			

void DoMouseDnGrow (
				WindowPtr							window)

{
	Handle								windowInfoHandle;
	
	SInt16								windowType;
	
				
	windowInfoHandle = (Handle)GetWRefCon(window);
	
	if (windowInfoHandle != NULL)
		{
		windowType = ((WindowInfoPtr)*windowInfoHandle)->windowType;
		
		if (windowType == kGraphicsWindowType)
			DoGraphWindowGrow (window, windowInfoHandle);
			
		else if (windowType == kStatisticsWindowType)
			DoProjectWindowGrow (window);
			
		#if use_mlte_for_text_window
			else if (windowType == kOutputWindowType)
				TXNGrowWindow (gOutputTextH, &gEventRecord);
		#endif
			
		else		// windowType != kGraphicsWindowType 
			DoNonGraphWindowGrow (window, windowInfoHandle);
		
		}		// end "if (windowInfoHandle != NULL)" 

}		// end "DoMouseDnGrow" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoMultiSpec
//
//	Software purpose:	The purpose of this routine is to control the 
//							MultiSpec system from initialization thru stopping.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			Operating system
//
//	Coded By:			Larry L. Biehl			Date: 03/26/1988
//	Revised By:			Ravi Budruk				Date: 08/16/1988	
//	Revised By:			Larry L. Biehl			Date: 08/05/2013	

void DoMultiSpec (void)
 
{
			// Declare local variables															
			
	#if !TARGET_API_MAC_CARBON
		Point									location = {0x0064, 0x0064};
	#endif	// !TARGET_API_MAC_CARBON

	DialogPtr							dialogPtr = NULL;
	SInt32*								debugPtr;
	WindowPtr							window;
	
	Handle								windowInfoHandle;
	
	SInt32								menuData;
		
	SInt16								errorCode = noErr,
											itemHit = 0,
											mouseLoc;
											
	UInt16								what;
	
	Boolean								dialogSelect = 0,
											haveEvent;
	
	char									theChar;
	
	
	debugPtr = NULL;
	
			// Main event processing loop														
			
	do
		{
		haveEvent = WaitNextEvent (everyEvent, &gEventRecord, gYieldTime, NIL);
			
		if (!haveEvent) 
			{
			FixCursor ();
			
			if (gMultiSpecWorkflowInfo.workFlowCode > 0)
				DoWorkFlow ();
			
			}		// end "if (!haveEvent)" 
		
		else 		// haveEvent is TRUE 
			{
			what = gEventRecord.what;
			if (IsDialogEvent (&gEventRecord))
				{
				dialogSelect = DialogSelect (&gEventRecord, &dialogPtr, &itemHit);
					
	    		switch (what)
	    			{
	      		case activateEvt:
						break;
					
	      		case updateEvt:
						what = nullEvent;
						break;
						
					case keyDown:
						break;
				
	      		default:
						what = nullEvent;
						break;
		      						
		      	}		// end "switch (what)" 
				
				}		// end "if (IsDialogEvent (&gEventRecord))" 

	    	switch (what)
	    		{
	      	case mouseDown:
		  			mouseLoc = FindWindow (gEventRecord.where, &window);
		  
		  			switch (mouseLoc)
		  				{
		  				case inDesk:
		  					break;
		  					
		    			case inMenuBar:
							UpdateMenuBeforeSelection();
		    				menuData = MenuSelect (gEventRecord.where);
		      			Menus (menuData);
		      			break;
			
		    			case inSysWindow:
		    				
		    				#if !TARGET_API_MAC_CARBON
		      				SystemClick (&gEventRecord, window);
		    				#endif	// !TARGET_API_MAC_CARBON
		      			break;
		      
		    			case inContent:
		    				DoMouseDnContent (window);
							break;
		      
		    			case inDrag:
	    					DragMultiSpecWindow(window, gEventRecord.where);
		      			break;
		      
		    			case inGrow:
		    				DoMouseDnGrow (window);
							break;
		      
		    			case inGoAway:

		      			if (!gInBackground && TrackGoAway (window, gEventRecord.where))
		      				{
		      				windowInfoHandle = GetWindowInfoHandle (window);
		  						switch (GetWindowType(windowInfoHandle))
		  							{
		  							case kOutputWindowType:
		      								// Just hide the output window.				
		      								
		      						HideWindow (window);
		      						break;
		      						
		  							case kPaletteWindowType:
										//ClosePaletteWindow (gPaletteWindow);
		      						break;
		      					
		  							case kGraphicsWindowType:
		      						CloseGraphicsWindow (window);
		      						break;
		      						
		  							case kImageWindowType:
		  							case kThematicWindowType:
		      						CloseImageWindow (window);
		      						break;
      					
			  						case kStatisticsWindowType:
//										CloseStatisticsWindow ();
										CloseTheProject ();
			      					break;
		      						
		      					}		// end "switch (gActiveWindowType)" 
		      													
		      				}		// end "if ((!gInBackground && TrackGoAway (..." 
    						break;
    						
     
     		    		case inZoomIn:
     		    		case inZoomOut:
     		    		
							DoWindowZoomInOutEvent (window, mouseLoc);
 							break;
 							
// 						default:
//		    				ExitToShell ();
 							
 						}		// end switch mouseLoc 
 					break;
 					
 				case keyDown:
					theChar = (char)gEventRecord.message&charCodeMask;
					if (gEventRecord.modifiers&cmdKey)
						{
						MInitCursor ();
						UpdateMenuBeforeSelection();
					  	Menus (MenuKey(theChar));
					  	
					  	}		// end "if (gEventRecord.modifiers&cmdKey)" 
					  	
					else		// !(gEventRecord.modifiers&cmdKey) 
						{
						if (gTheActiveWindow)
							{
							if (gTheActiveWindow == gOutputWindow)
								{
								if (theChar == 0x08)
											// Treat the delete key for the text 		
											// window the same as cut from the edit 	
											// menu.												
											
									UpdateMenuBeforeSelection();
						  			Menus (MenuKey(0x78));
									
								}		// end "if (... == gOutputWindow" 
								
							else if (gTheActiveWindow == gActiveImageWindow)
								{
								if (theChar == kBackspaceCharCode || theChar == kDeleteCharCode)
									{
											// Treat the delete key for the active 	
											// image window the same as clear 			
											// selection rectangle from the edit 	
											// menu.												
/*									
									menuData = kEditMenuID;
									menuData = (menuData<<16) + kClearSelectionRect;
									HiliteMenu (kEditMenuID);
									Delay (7, (UInt32*)&lenText);
									UpdateMenuBeforeSelection();
						  			Menus ( menuData );
*/						  			
						  			SendMenuClearSelectionRectangleCommand ();
									
									}		// end "if (theChar == 0x08" 
								
								else if (theChar == kEnterCharCode || theChar == kReturnCharCode)
									DoStatisticsAddFieldEvent ();
									
								else if (theChar == kLeftArrowCharCode || theChar == kRightArrowCharCode ||
										theChar == kUpArrowCharCode || theChar == kDownArrowCharCode)
									DoNextDisplayChannelEvent (gTheActiveWindow, theChar);
								
								}		// end "if (... == gActiveImageWindow)" 
										
							if (gTheActiveWindow == gProjectWindow)
								{
								if (theChar == 0x03 || theChar == 0x0d)
									DoStatisticsAddFieldEvent ();
									
								if (theChar == 0x08)
									{
											// Treat the delete key for the statistics 	
											// window the same as clear selection rectangle 
											// from the edit menu.	
											
						  			SendMenuClearSelectionRectangleCommand ();
									
									}		// end "if (theChar == 0x08" 
								
								}		// end "if (... == gActiveImageWindow)" 
								
							}		// end "if (gTheActiveWindow)" 
						
						}		// end "else !(gEventRecord.modifiers&cmdKey)" 						
					break;
		  
		  		case autoKey:
					theChar = (char)gEventRecord.message&charCodeMask;
					if (gTheActiveWindow != NULL)
						{								
						if (gTheActiveWindow == gActiveImageWindow)
							{	
							CGEventSourceStateID eventSource = kCGEventSourceStateCombinedSessionState;

							if (CGEventSourceKeyState(eventSource, 0x7B) || CGEventSourceKeyState(eventSource, 0x7C))
								{
								if (theChar == kLeftArrowCharCode || theChar == kRightArrowCharCode)
									DoNextDisplayChannelEvent (gTheActiveWindow, theChar);
								
								}		// end "if (CGEventSourceKeyState(eventSource, 0x7B) || ..."
							
							}		// end "if (gTheActiveWindow == gActiveImageWindow)" 
							
						}		// end "if (gTheActiveWindow != NULL)" 
	         	break;
		  
		  		case updateEvt:
		  			DoUpdateEvent (&gEventRecord);
	         	break;
	         	
	         case diskEvt:
	         	if (HiWord (gEventRecord.message) != noErr)
	         		{
	         		MInitCursor ();
		    				
		    			#if !TARGET_API_MAC_CARBON
	         			DIBadMount (location, (SInt16)gEventRecord.message);
		    			#endif	// !TARGET_API_MAC_CARBON
	         		
	         		}		// end "if ( HiWord (event.message) != noErr)"
	         	break;
		    
				case activateEvt:
					DoActivateEvent ((WindowPtr)gEventRecord.message,
													(gEventRecord.modifiers & activeFlag));
					break;
		    
				case osEvt:
					DoSuspendResume (&gEventRecord);
					break;
				
				case kHighLevelEvent:
					errorCode = AEProcessAppleEvent (&gEventRecord);
					break;
		       
				default:
					break;
			
				} 			// end of case what  
	
		 	if (gRedrawMenuBar)
		 		{
				DrawMenuBar();
				gRedrawMenuBar = FALSE;
				
				}		// end "if (gRedrawMenuBar)" 
					
			if (gDoneFlag)
				errorCode = SendAEQuitApplication ();
		
			}  // end of "else haveEvent is TRUE 
			
		endloop:;

		}  while  (!gDoneFlag);
		
			// Exit MultiSpec 																	
	
	ExitToShell ();
	
}	// end of main 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoNonGraphWindowGrow
//
//	Software purpose:	This routine handles mouse down events in the
//							grow box for non-graphics windows.
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
//	Revised By:			Larry L. Biehl			Date: 12/17/2008			

void DoNonGraphWindowGrow (
				WindowPtr							theWindow,
				Handle								windowInfoHandle)

{
			//	Local Declarations 		
			
	Rect									growArea;														
	
	SInt32								size;
	
	SInt16								height,
											width;
	
	
			// Get the current maximum rectangular display area.
														
	GetDisplayRegionRectangle (&growArea);	
			
	growArea.right -= growArea.left;
	growArea.bottom -= growArea.top;
	
			// Make certain that the grow area takes into account the legend		
			// width or the title height.														
			
	growArea.left = gGrowAreaMinimum + gActiveLegendWidth;
	growArea.top = gGrowAreaMinimum + GetImageTopOffset (windowInfoHandle);
	
//	tallerFlag = FALSE;
	
	size = GrowWindow (theWindow, gEventRecord.where, &growArea);
		
	width  = LoWord (size);
	height = HiWord (size);
	
	GrowNonGraphWindow (theWindow,
									windowInfoHandle,
									height,
									width);
/*				
	GetWindowPortBounds (theWindow, &rect);
  	ClipRect (&rect);

			// Erase growicon																		

  	scrollBar.top    = rect.bottom - kSBarWidth;
  	scrollBar.left   = rect.right - kSBarWidth;
  	scrollBar.bottom = rect.bottom;
  	scrollBar.right  = rect.right;
  				
   EraseRect (&scrollBar);
  	InvalWindowRect (theWindow, &scrollBar);


			// Invalidate vertical scroll area of old window and that portion of the
			// coordinate view above the scroll bar if new window is 	
			// wider than old one																

	if (width > rect.right)
		{
		scrollBar.top    = rect.top + ((WindowInfoPtr)*windowInfoHandle)->coordinateHeight;
		scrollBar.left   = rect.right - kSBarWidth - 1;
  		if (theWindow == gOutputWindow)
  			scrollBar.left -= 3;
		scrollBar.bottom = rect.bottom;
		scrollBar.right  = rect.right;
		
   	EraseRect (&scrollBar);
  		InvalWindowRect (theWindow, &scrollBar);
		
		scrollBar.top = rect.top;
		scrollBar.bottom = rect.top + ((WindowInfoPtr)*windowInfoHandle)->coordinateHeight;
  		InvalWindowRect (theWindow, &scrollBar);
  		
  		if (((WindowInfoPtr)*windowInfoHandle)->titleHeight > 0)
  			{
  					// Invalidate the entire title
  					
			scrollBar.top = ((WindowInfoPtr)*windowInfoHandle)->coordinateHeight;
			scrollBar.left = 0;
			scrollBar.bottom = scrollBar.top + ((WindowInfoPtr)*windowInfoHandle)->titleHeight;
   		EraseRect (&scrollBar);
  			InvalWindowRect (theWindow, &scrollBar);
  			
  			}		// end "if (((WindowInfoPtr)*windowInfoHandle)->titleHeight > 0)"
		
		}		// end "if (width > rect.right)" 
		
	else if (theWindow == gOutputWindow)
		{
		scrollBar.top    = rect.top;
		scrollBar.left   = width - kSBarWidth - 4;
		scrollBar.bottom = rect.bottom;
		scrollBar.right  = scrollBar.left + 4;
		
   	EraseRect (&scrollBar);
  		InvalWindowRect (theWindow, &scrollBar);
		
		}		// end "else if (theWindow == gOutputWindow)"

			// Invalidate horizontal scroll area of old window if new window is	
			// taller than old one																

	if (height > rect.bottom)
		{
  		scrollBar.top    = rect.bottom - kSBarWidth - 1;
  		scrollBar.left   = rect.left;
  		scrollBar.bottom = rect.bottom;
  		scrollBar.right  = width;
  		
  		if (theWindow == gOutputWindow)
  			{
  			scrollBar.top -= 8;
  			scrollBar.bottom -= 4;
  			
  			}		// end "if (theWindow == gOutputWindow)"
  			
   	EraseRect (&scrollBar);
  		InvalWindowRect (theWindow, &scrollBar);
  		
  		tallerFlag = TRUE;
  		
  		}		// end "if (height > rect.bottom)" 
		
	else if (theWindow == gOutputWindow)
		{
  		scrollBar.top    = height - kSBarWidth - 4;
  		scrollBar.left   = rect.left;
  		scrollBar.bottom = scrollBar.top + 4;
  		scrollBar.right  = width;
   	EraseRect (&scrollBar);
  		InvalWindowRect (theWindow, &scrollBar);
		
		}		// end "else if (theWindow == gOutputWindow)"

   SizeWindow (theWindow, width, height, TRUE);
  	
			// Hide the controls to be ready for updating.		
							
	if (gAppearanceManagerFlag)
		{
		GetRootControl(theWindow, &rootControlHandle);
		
		if (rootControlHandle != NULL)
			{
			CountSubControls (rootControlHandle, &numberControls);
			for (index=numberControls; index>=1; index--)
				{
				GetIndexedSubControl(rootControlHandle, index, &controlHandle);
				refCon = (SInt16)GetControlReference (controlHandle);
				
				if (refCon < kCoordinateUnitsControl)
		  			HideControl ( controlHandle );
		     			
		     	}		// end "for (index=numberControls; index>=1; index--)" 
		     	
			}		// end "if (rootControlHandle != NULL)"
	     	
		}		// end "if (gAppearanceManagerFlag)"
		
	#if !TARGET_API_MAC_CARBON
		else		// !gAppearanceManagerFlag
			{
			controlHandle = (ControlHandle)((WindowPeek)theWindow)->controlList;
			while (controlHandle != NULL)
				{
				refCon = (SInt16)GetControlReference (controlHandle);
				
				if (refCon < kCoordinateUnitsControl)
		  			HideControl ( controlHandle );
		  			
		     	controlHandle = (*controlHandle)->nextControl;
		     			
		     	}		// end "while (controlHandle != NULL)"
		     	
			}		// end "else !gAppearanceManagerFlag" 
	#endif	// !TARGET_API_MAC_CARBON
	
	AdjustLegendListLength (theWindow, tallerFlag);
	
	DoOutputWindowGrow (theWindow, windowInfoHandle);
     	
	UpdateScrolls (theWindow);
  		
	DrawScrollBars (theWindow);
	
	*/

}		// end "DoNonGraphWindowGrow"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoOutputWindowGrow
//
//	Software purpose:	This routine handle growing events specific to the output
//							window.
//							Note that using mlte in osx, this routine and the other output
//							window routines for activate, update and mousedown do not get
//							called. This all being handled within the carbon events.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			main
//
//	Coded By:			Larry L. Biehl			Date: 12/11/2001
//	Revised By:			Larry L. Biehl			Date: 12/17/2008			

void DoOutputWindowGrow (
				WindowPtr							theWindow,
				Handle								windowInfoHandle)

{	
	if (GetWindowType(windowInfoHandle) == kOutputWindowType)
		{
		GetWindowClipRectangle (theWindow, kNoScrollBarArea, &gViewRect);
		
		#if use_mlte_for_text_window
					// Note that the grow event for the mlte text window does not get
					// called. It is handled internally.
			TXNGrowWindow (gOutputTextH, &gEventRecord);
			TXNDrawObject (gOutputTextH, NULL, kTXNDrawItemTextMask);
			TXNRecalcTextLayout (gOutputTextH);

//			TXNSetHIRectBounds (gOutputTextH,
//						const HIRect *  iViewRect,
//						const HIRect *  iDestinationRect,
//			         TRUE);		// iUpdate
		#endif		// use_mlte_for_text_window
		
		#if !use_mlte_for_text_window
			LongRect								weDestRect;
			
			WEGetDestRect (&weDestRect, gOutputTextH);
		
					// Set the destination rectangle so that no text is drawn within
					// 3 pixels of the edge of the window left and right. Do not adjust
					// the top and bottom. This indicates the total length of the text.
		
			weDestRect.right = gViewRect.right - 3;
			WESetDestRect (&weDestRect, gOutputTextH);
		#endif		// !use_mlte_for_text_window
		
		UpdateOutputWScrolls (gOutputWindow, 1, kDisplayMessage);
		
		}		// end "if (GetWindowType(windowInfoHandle) == kOutputWindowType)"

}		// end "DoOutputWindowGrow"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoOutputMouseDnContent
//
//	Software purpose:	This routine handle mouse down events in the
//							content part of the output window.
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
//	Revised By:			Larry L. Biehl			Date: 02/24/2012			

void DoOutputMouseDnContent (
				WindowPtr							theWindow,
				Point									mousePt)

{
	#if !use_mlte_for_text_window
			//	Local Declarations 																
	
	ControlHandle						whichControl;
	
	SInt16								partCode;
	#endif		// !use_mlte_for_text_window

	
	#if use_mlte_for_text_window
		TXNClick (gOutputTextH, &gEventRecord);
	#endif
			
	#if !use_mlte_for_text_window
	partCode = FindControl (mousePt, theWindow, &whichControl);
	if (partCode > 0)
     	{
     			// Set clip region to include the scroll bars.							
     			
		ClipRect (GetWindowPortBounds(theWindow, &gTempRect));
			
				// Exit if this was a mouse down in a control area of an		
				// inactive image window.  We will assume that the user was	
				// just wanting to select the window.					

   	OutputWControlEvent (whichControl, theWindow, mousePt, partCode);
   		
		}		// end "if (partCode > 0)" 
		 
	else		// partCode == 0.  Mouse not within controls 
		{
		if (gPresentCursor == kIBeam)
			{	
//			#if use_mlte_for_text_window
//				TXNClick (gOutputTextH, &gEventRecord);
//			#endif
//			#if !use_mlte_for_text_window
				WEClick (mousePt, 
							gEventRecord.modifiers, 
							gEventRecord.when,
							gOutputTextH);
//			#endif
				
			
			}		// end "if (gPresentCursor == kIBeam)" 
		  		
		}		// end "else partCode == 0" 
	#endif		// !use_mlte_for_text_window
	
	gUpdateFileMenuItemsFlag = TRUE;
	gUpdateEditMenuItemsFlag = TRUE;

}		// end "DoOutputMouseDnContent" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoOutputWActivateEvent
//
//	Software purpose:	This routine handles the activate/deactive events
//							for output window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			main
//
//	Coded By:			Larry L. Biehl			Date: 04/06/1988
// Revised By:			Ravi S. Budruk			Date: 08/16/1988	
//	Revised By:			Larry L. Biehl			Date: 12/17/2008			

void DoOutputWActivateEvent (
				WindowPtr							theWindow,
				#if use_mlte_for_text_window 
					TXNObject							textH, 
				#endif
				#if !use_mlte_for_text_window 
					WEReference							textH, 
				#endif
				Boolean								becomingActiveFlag)

{
			//	Local Declarations 																

	ControlHandle						horizontalControl,
											verticalControl;
	
	Handle								windowInfoHandle;
	
	
			// Do nothing if the requested activation state is the same as the current
			// activation state.
			
	#if !use_mlte_for_text_window
		if (becomingActiveFlag == WEIsActive(textH))
																									return;
	#endif
																									
   windowInfoHandle = GetWindowInfoHandle (theWindow);
   if (windowInfoHandle == NULL)
																									return;
   
	horizontalControl = ((WindowInfoPtr)*windowInfoHandle)->horizontalControl;
	verticalControl = ((WindowInfoPtr)*windowInfoHandle)->verticalControl;
	
	if (becomingActiveFlag)
		{	
				// Window is becoming active														
				
		#if !use_mlte_for_text_window
			WEActivate (textH);
		#endif
		
				//	Show window controls.	
		
		if (gOSXFlag)
			{
			HiliteControl (horizontalControl, 0);
			HiliteControl (verticalControl, 0);
			
			}		// end "if (gOSXFlag)"
			
		else		// !gOSXFlag
			{
			ShowControl (horizontalControl);
			ShowControl (verticalControl);
			
			}		// end "else !gOSXFlag"
	     	
		}		// end "if (becomingActiveFlag)" 
		
	else		// !becomingActiveFlag													
		{		
		#if !use_mlte_for_text_window
			WEDeactivate (textH);
		#endif
		
				//	Hide window controls.		
		
		if (gOSXFlag)
			{
			HiliteControl (horizontalControl, 255);
			HiliteControl (verticalControl, 255);
			
			}		// end "if (gOSXFlag)"
			
		else		// !gOSXFlag
			{
			HideControl (horizontalControl);
			HideControl (verticalControl);
			
			}		// end "else !gOSXFlag"
				
	     	
		}		// end "else !becomingActiveFlag" 

}		// end "DoOutputWActivateEvent" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoOutputWUpdateEvent
//
//	Software purpose:	This routine handles the update events for the output
//							window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			main
//
//	Coded By:			Larry L. Biehl			Date: 04/06/1988
//	Revised By:			Larry L. Biehl			Date: 12/18/2008			

void DoOutputWUpdateEvent (
				WindowPtr							window, 
				#if use_mlte_for_text_window 
					TXNObject							textH, 
				#endif
				#if !use_mlte_for_text_window 
					WEReference							textH, 
				#endif 
				Rect*									rectPtr)

{
	if (textH != NULL)
		{				
		#if use_mlte_for_text_window
					// Note that the update event does not get called. mlte handles
					// it internally.
			TXNDrawObject (textH, NULL, kTXNDrawItemAllMask);
			TXNRecalcTextLayout (gOutputTextH);
			TXNUpdate (textH);
		#endif

		#if !use_mlte_for_text_window
			RgnHandle							updateRegionH;
			
					// Get window size less scroll bar edges. 								
		
			GetWindowClipRectangle (window, kNoScrollBarArea, rectPtr);
			
					// Update the view rectangle for the text.								
				
			updateRegionH = NewRgn ();
			if (updateRegionH != NULL)
				{
				GetPortVisibleRegion (GetWindowPort(window), updateRegionH);
		
						// Do the update.																	
					
				BeginUpdate (window);

				if (!EmptyRgn (updateRegionH))
					{
						// Erase the update region
						
					EraseRgn (updateRegionH);

							//	Draw text
						
					WEUpdate (updateRegionH, textH);
				
					ClipRect (GetWindowPortBounds(window, &gTempRect));

							//	Draw scroll bars and grow icon.
							
					DrawControls (window);					
					DrawGrowIcon (window);
									
							// Set clip region of window at scroll bar edges. 						
						
					ClipRect (rectPtr);
				
					}		// end "if (!EmptyRgn (updateRegionH))"
		
				DisposeRgn (updateRegionH);
				
				}		// end "if (updateRegionH != NULL)"
		
			EndUpdate (window);
		#endif	// !use_mlte_for_text_window
		
		}		// end "textH != NULL" 
	
}		// end "DoOutputWUpdateEvent" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoPaletteWActivateEvent
//
//	Software purpose:	This routine handles the activate events for the
//							palette window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			main
//
//	Coded By:			Larry L. Biehl			Date: 04/06/1988
//	Revised By:			Larry L. Biehl			Date: 01/24/1989
// Revised By:			Ravi S. Budruk			Date: 08/16/1988				

void DoPaletteWActivateEvent (void)
		
{

}		// end "DoPaletteWActivateEvent"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoPaletteWUpdateEvent
//
//	Software purpose:	This routine handles the update events in the palette
//							window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			main
//
//	Coded By:			Larry L. Biehl			Date: 04/06/1988
//	Revised By:			Larry L. Biehl			Date: 01/30/1991			

void DoPaletteWUpdateEvent (
				WindowPtr							window)
	
{
			//	Local Declarations 																

	PicHandle							myPicture;
	
	
	BeginUpdate (window);
	EraseRect (GetWindowPortBounds(window, &gTempRect));
	myPicture = GetWindowPic (window);
	
	if (myPicture != NULL)
		DrawPicture (myPicture, &gTempRect);
		
	EndUpdate (window);
	
}		// end "DoPaletteWUpdateEvent"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoStatisticsAddFieldEvent
//
//	Software purpose:	This routine handles the the statistics add to field
//							event that is initiated by striking a carriage return
//							or enter key.  This routine verifies that everything
//							is set to enter a new statistics field.  If so then
//							the 'AddFieldToProject' routine is called.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			main
//
//	Coded By:			Larry L. Biehl			Date: 06/10/1993
//	Revised By:			Larry L. Biehl			Date: 07/06/1998			

void DoStatisticsAddFieldEvent (void)

{
	GrafPtr								savedPort;
	
	
	if ( 	gProjectSelectionWindow &&
			(gTheActiveWindow == gProjectSelectionWindow || 
											gTheActiveWindow == gProjectWindow) &&
			(gStatisticsMode == kStatNewFieldRectMode ||
												gStatisticsMode == kStatNewFieldPolyMode) &&
			IsWindowVisible(gProjectWindow) )
		{
		if ( GetControlHilite(gProjectInfoPtr->addToControlH) != 255 )
			{
			GetPort (&savedPort);
			SetPortWindowPort (gProjectWindow);
			MInitCursor ();
		
			StatisticsWControlEvent (
					gProjectInfoPtr->addToControlH, FALSE, gEventRecord.where, 0);
			
			SetPort ( savedPort );
			
			}		// end "if ( (*...->addToControlH)->contrlHilite != 255 )" 
		
		}		// end "if (gTheActiveWindow == gProjectSelectionWindow...)" 
	
}		// end "DoStatisticsAddFieldEvent" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoStatisticsMouseDnContent
//
//	Software purpose:	This routine handle mouse down events in the
//							content part of the statistics window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			DoMouseDnContent
//
//	Coded By:			Larry L. Biehl			Date: 09/09/88
//	Revised By:			Larry L. Biehl			Date: 07/29/96			

void DoStatisticsMouseDnContent (
				WindowPtr							theWindow,
				Point									mousePt)

{
			//	Local Declarations 																
	
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

   	StatisticsWControlEvent (whichControl, TRUE, mousePt, 0);
   		
		}		// end "if (partCode > 0)" 
		 
	else		// partCode == 0.  Mouse not within controls 
		StatisticsWMouseDn (mousePt);

}		// end "DoStatisticsMouseDnContent"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoSuspendResume
//
//	Software purpose:	This routine handle suspend/resume events.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/25/1989
//	Revised By:			Larry L. Biehl			Date: 05/18/1994			

void DoSuspendResume ( 
				EventRecord*						theEventPtr)

{	
			// Switch on the high byte of message.											
			
	switch ( (theEventPtr->message >> 24) & 0x0ff )
		{
		case kMouseMovedMessage:
			FixCursor ();
			break;
			
		case kSuspendResumeMessage:
			gInBackground =
							( (theEventPtr->message & kResumeMask) == 0 );
			
					// Set yield time variable.												
					
			if (gInBackground)
				{
				gYieldTime = kBackYieldTime;
				gTimeOffset = kBackTimeOffset;
				
				UpdateDeskScrap (TRUE);
				
				}		// end "if (gInBackground)" 
				
			else		// !gInBackground 
				{
			 	gYieldTime = kFrontYieldTime;
			 	
			 	if (gOSXCoreGraphicsFlag)
					gTimeOffset = 30;
			 	
			 	else		// !gOSXCoreGraphicsFlag
					gTimeOffset = kFrontTimeOffset;
				
				}		// end "else !gInBackground" 
			 	
			 		// Suspend/resume is also an activate/deactive event.				
			 		// Do not do this if the active processor is the display			
			 		// processor.  It will corrupt the image.								
			 	
//	      if (gProcessorCode != kDisplayProcessor)
//				DoActivateEvent ( FrontWindow(), !gInBackground );
				
			if (!gInBackground)
				FixCursor ();
			 		
			break;
		
		}		// end "switch ( (gEventRecord.message >> 24) ..." 
		
}		// end "DoSuspendResume" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoUpdateEvent
//
//	Software purpose:	This routine handles the update events.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			main
//
//	Coded By:			Larry L. Biehl			Date: 04/06/1988
//	Revised By:			Larry L. Biehl			Date: 09/08/2006			

void DoUpdateEvent ( 
				EventRecord*						theEventPtr)

{
			//	Local Declarations 			
			
	Rect									rect;													

	GrafPtr								savedPort;
	WindowPtr							window;
	
	Handle								windowInfoHandle;
	
	#if use_mlte_for_text_window
		TXNObject							textH;
	#endif
	#if !use_mlte_for_text_window
		WEReference							textH;
	#endif
	
	SInt32								windowType;
	
	SignedByte							handleStatus;
	
			
	window  = (WindowPtr)theEventPtr->message;
	
	if (window != NULL)
		{
				// Get text handle and window type.											
				
		windowInfoHandle = (Handle)GetWRefCon(window);
		
		if (windowInfoHandle)
			{
			handleStatus = HGetState (windowInfoHandle);
			HLock (windowInfoHandle);
			
			textH = ((WindowInfoPtr)*windowInfoHandle)->textH;
			windowType = ((WindowInfoPtr)*windowInfoHandle)->windowType;
			
			GetPort (&savedPort);
			SetPortWindowPort (window);
			
			switch (windowType)
				{
				case kOutputWindowType:	
					DoOutputWUpdateEvent (window, textH, &rect);
					break;
			
				case kImageWindowType:	
					DoImageWUpdateEvent (window, windowInfoHandle);
					break;
			
				case kThematicWindowType:
					DoThematicWUpdateEvent (window, windowInfoHandle);
					break;
			
				case kStatisticsWindowType:
					DoStatWUpdateEvent (window);
					break;
			
				case kPaletteWindowType:
					DoPaletteWUpdateEvent (window);
					break;
			
				case kGraphicsWindowType:
					DoGraphWUpdateEvent (
						window, ((WindowInfoPtr)*windowInfoHandle)->graphViewCPtr);
					break;
						
				}		// end "switch (windowType)"
	
			HSetState (windowInfoHandle, handleStatus);
		
			SetPort (savedPort);
			
			}		// end "if (windowInfoHandle != NULL)" 
		
		}		// end "if (window != NULL)" 
	
}		// end " DoUpdateEvent" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoWindowZoomInOutEvent
//
//	Software purpose:	This routine handles the zoom events.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			main
//
//	Coded By:			Larry L. Biehl			Date: 04/06/1988
//	Revised By:			Larry L. Biehl			Date: 07/20/2011			

void DoWindowZoomInOutEvent (
				WindowPtr							window, 
				SInt16								partCode)

{
	Rect									idealStandardState,
											userStateRect;
	
	DisplaySpecsPtr					displaySpecsPtr;
	WindowInfoPtr						windowInfoPtr;
	
	ControlHandle						controlHandle,
											rootControlHandle;
											
	Handle								windowInfoHandle;
	
	SInt16								windowType;
	
	UInt16								index,
											numberControls;
											
	
			// Make sure that this is the active image window. If not then just
			// activate the window.
			
	if (window != FrontWindow())
		{
		SelectWindow (window);
//		if (window == gOutputWindow)
//																							return;
		CheckSomeEvents (activMask+updateMask);
//		FixCursor ();
																							return;
		
		}		// end "if (theWindow != FrontWindow())"
		    
	if (TrackBox (window, gEventRecord.where, partCode))
		{
		displaySpecsPtr = NULL;
		windowInfoHandle = GetWindowInfoHandle (window);
		windowType = GetWindowType (windowInfoHandle);
		
		if (windowType == kOutputWindowType)
			{
			TXNZoomWindow (gOutputTextH, partCode);
			TXNAdjustCursor (gOutputTextH, NULL);
																							return;
			
			}		// end "if (windowType == kOutputWindowType)"
			
		else if (windowType == kImageWindowType ||
													windowType == kThematicWindowType)
			{
					// Get the window size needed to display the entire image.	

			GetMinimumDisplaySizeForImage (windowInfoHandle,
														&gTempLongPoint);
																		
			}		// end "else if (windowType ==  kImageWindowType || ..." 
			
				// Get the maximum size possible for the window on the device	
				// that the mouse cursor is within.
				
		GetMaximumSizeForDevicePointIsIn (&gEventRecord.where, &gViewRect);										
/*		
		if (gColorQDflag)
			{
					// Get the device that the mouse cursor is  in.		
					
			currentDevice = GetDevicePointIsIn (&gEventRecord.where);
						
			if (currentDevice)
				{
						// Get the maximum window possible for the device.			
				
				gViewRect = (*currentDevice)->gdRect;
				gViewRect.top += 21;
				gViewRect.left += 4;
				gViewRect.bottom -= 4;
				gViewRect.right -= 4;
				
						// Adjust for menu bar height if needed.						
					
				if (currentDevice == GetMainDevice())
					gViewRect.top += GetMBarHeight();
				
				#if TARGET_API_MAC_CARBON
							// for OSX
							
					if (gOSXFlag)
						{																
						OSStatus 							status;
						
								// This is for testing for now.				
									
						status = GetAvailableWindowPositioningBounds (currentDevice, &gViewRect);
						
						gViewRect.top += 24;
						gViewRect.left += 4;
						gViewRect.bottom -= 4;
						gViewRect.right -= 4;
					  	
					  	}		// end "if (gOSXFlag)"
				  	
				#endif		// TARGET_API_MAC_CARBON
				
				}		// end "if (currentDevice)" 
				
			}		// end "if (gColorQDflag)" 
			
		else		// !gColorQDflag 
			{
			grayRegionHandle = GetGrayRgn();
			GetRegionBounds(grayRegionHandle, &gViewRect);
			gViewRect.top += 46;
			gViewRect.left += 4;
			gViewRect.bottom -= 4;
			gViewRect.right -= 4;
			
			}		// end "else !gColorQDflag" 
*/			
      		// Update the standard state to be the size of						
				// the window that allows the entire image to						
      		// be viewed or if that is larger than the 							
      		// screen then the entire													
      		// screen for the device that the cursor is in. 
				
		GetWindowStandardState (window, &idealStandardState);
		gTempRect = idealStandardState;
		GetWindowUserState (window, &userStateRect);
				
				// Determine whether the top left corner of the user				
				// rectangle is within the current device.  If so use				
				// this as the top left for the standard state.  If not			
				// use the upper left of the 'new' current device.					
						
		if (PtInRect (topLeft(userStateRect), &gViewRect))
			topLeft(idealStandardState) = topLeft(userStateRect);
			
		else		// !PtInRect (&topLeft(... 
			topLeft(idealStandardState) = topLeft(gViewRect);
			
		windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle,
																			kNoLock,
																			kNoMoveHi);
																		
		if (windowInfoPtr != NULL && windowInfoPtr->displaySpecsHandle != NULL)
			displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
																		windowInfoPtr->displaySpecsHandle,
																		kNoLock,
																		kNoMoveHi);

		if (displaySpecsPtr != NULL)
			{
					// Adjust down to the size needed to enclose the image.		
				
			idealStandardState.right = MIN(gViewRect.right,
									idealStandardState.left + (SInt16)gTempLongPoint.h);
											
			idealStandardState.bottom = MIN(gViewRect.bottom,
									idealStandardState.top + (SInt16)gTempLongPoint.v);
											
					// Now force the upper left if possible to enclose the		
					// image if needed.														
				
			idealStandardState.left = MAX(gViewRect.left,
									idealStandardState.right - (SInt16)gTempLongPoint.h);
											
			idealStandardState.top = MAX(gViewRect.top,
									idealStandardState.bottom - (SInt16)gTempLongPoint.v);
			
			}		// end "if (displaySpecsPtr)" 
			
		else		// displaySpecsPtr == NULL
			{
			idealStandardState.right = gViewRect.right;
			idealStandardState.bottom = gViewRect.bottom;
			
			}		// end "else displaySpecsPtr == NULL" 
			
		SetWindowStandardState (window, &idealStandardState);
			
				// If the window is not in the standard state then force to the	
				// standard state.																
					
		if (!EqualRect (&gTempRect, &idealStandardState))
			partCode = inZoomOut;
			
		      				
		ClipRect (GetWindowPortBounds (window, &gTempRect));
		EraseRect (&gTempRect);
		     			  				
		ZoomWindow (window, partCode, FALSE);
		  	
				// Hide the controls to be ready for updating.			
							
		if (gAppearanceManagerFlag)
			{
			GetRootControl(window, &rootControlHandle);
			
			if (rootControlHandle != NULL)
				{
				CountSubControls (rootControlHandle, &numberControls);
				for (index=numberControls; index>=1; index--)
					{
					GetIndexedSubControl(rootControlHandle, index, &controlHandle);
				  	HideControl (controlHandle);
			     			
			   	}		// end "for (index=numberControls; index>=1; index--)" 
			   	
			 	}		// end "if (rootControlHandle != NULL)"
		   
		   }		// end "if (gAppearanceManagerFlag)"
		
		#if !TARGET_API_MAC_CARBON
			else		// !gAppearanceManagerFlag
				{
				controlHandle = (ControlHandle)((WindowPeek)window)->controlList;
				while (controlHandle != NULL)
					{
			  		HideControl (controlHandle);
					controlHandle = (*controlHandle)->nextControl;
					
					}		// end "while (controlHandle != NULL)"
		     			
		  	 	}		// end "else !gAppearanceManagerFlag" 
		#endif	// !TARGET_API_MAC_CARBON
		  					
		UpdateScrolls (window);
		  					
		if (window != gPaletteWindow) 
			DrawScrollBars (window);
			
		else		// (window == gPaletteWindow)	
		   DrawGrowIcon (window);
		
		AdjustLegendListLength (window, TRUE);
		    						
		}		// end "if (TrackBox (window, ..." 
	
}		// end "DoWindowZoomInOutEvent" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoWorkFlow
//
//	Software purpose:	This routine handles doing a specific workflow that was set
//							up to create images for SC09. GOES hdf file Images are mapped to 
//							a given size and projection, displayed as channels 4, 4, 2 and saved
//							to a geotiff file.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			main
//
//	Coded By:			Larry L. Biehl			Date: 11/04/2009
//	Revised By:			Larry L. Biehl			Date: 04/24/2017		

void DoWorkFlow ()

{	
//	Str255								savedImageFileName;
	SInt32								menuData;
	
	
/*			// Allow user to select the file(s) to be worked on.
			// One case was for GOES HDF File
	
	gMultiSpecWorkflowInfo.workFlowCode = 2;
	menuData = (kFileMenuID<<16) + kOpenImage;
	Menus (menuData);
*/	
			// Save the name of the last file to be used later.
			
//	CopyPToP ((char*)savedImageFileName,
//					(char*)gMultiSpecWorkflowInfo.lastFileReadName);
	
			// Reformat Image
			// The following is special for U2U project
			
	gMultiSpecWorkflowInfo.workFlowCode = 1;
	
	gMultiSpecWorkflowInfo.functionCode = 98;
	
	gMultiSpecWorkflowInfo.thresholdValue = 25;
	CopyPToP (gMultiSpecWorkflowInfo.defaultName, (UCharPtr)"\p25_1latest_2016_mint_20170423");
	gConfirmReplacementAlertFlag = FALSE;
	menuData = (kReformatMenuID<<16) + kReformatChangeImageRequest;
	Menus (menuData);
	gConfirmReplacementAlertFlag = TRUE;
				
	gMultiSpecWorkflowInfo.thresholdValue = 26;
	CopyPToP (gMultiSpecWorkflowInfo.defaultName, (UCharPtr)"\p26_1latest_2016_mint_20170423");
	gConfirmReplacementAlertFlag = FALSE;
	menuData = (kReformatMenuID<<16) + kReformatChangeImageRequest;
	Menus (menuData);
	gConfirmReplacementAlertFlag = TRUE;
				
	gMultiSpecWorkflowInfo.thresholdValue = 27;
	CopyPToP (gMultiSpecWorkflowInfo.defaultName, (UCharPtr)"\p27_1latest_2016_mint_20170423");
	gConfirmReplacementAlertFlag = FALSE;
	menuData = (kReformatMenuID<<16) + kReformatChangeImageRequest;
	Menus (menuData);
	gConfirmReplacementAlertFlag = TRUE;
				
	gMultiSpecWorkflowInfo.thresholdValue = 28;
	CopyPToP (gMultiSpecWorkflowInfo.defaultName, (UCharPtr)"\p28_1latest_2016_mint_20170423");
	gConfirmReplacementAlertFlag = FALSE;
	menuData = (kReformatMenuID<<16) + kReformatChangeImageRequest;
	Menus (menuData);
	gConfirmReplacementAlertFlag = TRUE;
				
	gMultiSpecWorkflowInfo.thresholdValue = 29;
	CopyPToP (gMultiSpecWorkflowInfo.defaultName, (UCharPtr)"\p29_1latest_2016_mint_20170423");
	gConfirmReplacementAlertFlag = FALSE;
	menuData = (kReformatMenuID<<16) + kReformatChangeImageRequest;
	Menus (menuData);
	gConfirmReplacementAlertFlag = TRUE;
				
	gMultiSpecWorkflowInfo.thresholdValue = 30;
	CopyPToP (gMultiSpecWorkflowInfo.defaultName, (UCharPtr)"\p30_1latest_2016_mint_20170423");
	gConfirmReplacementAlertFlag = FALSE;
	menuData = (kReformatMenuID<<16) + kReformatChangeImageRequest;
	Menus (menuData);
	gConfirmReplacementAlertFlag = TRUE;
				
	gMultiSpecWorkflowInfo.thresholdValue = 31;
	CopyPToP (gMultiSpecWorkflowInfo.defaultName, (UCharPtr)"\p31_1latest_2016_mint_20170423");
	gConfirmReplacementAlertFlag = FALSE;
	menuData = (kReformatMenuID<<16) + kReformatChangeImageRequest;
	Menus (menuData);
	gConfirmReplacementAlertFlag = TRUE;
				
	gMultiSpecWorkflowInfo.thresholdValue = 32;
	CopyPToP (gMultiSpecWorkflowInfo.defaultName, (UCharPtr)"\p32_1latest_2016_mint_20170423");
	gConfirmReplacementAlertFlag = FALSE;
	menuData = (kReformatMenuID<<16) + kReformatChangeImageRequest;
	Menus (menuData);
	gConfirmReplacementAlertFlag = TRUE;
				
	gMultiSpecWorkflowInfo.thresholdValue = 33;
	CopyPToP (gMultiSpecWorkflowInfo.defaultName, (UCharPtr)"\p33_1latest_2016_mint_20170423");
	gConfirmReplacementAlertFlag = FALSE;
	menuData = (kReformatMenuID<<16) + kReformatChangeImageRequest;
	Menus (menuData);
	gConfirmReplacementAlertFlag = TRUE;
				
	gMultiSpecWorkflowInfo.thresholdValue = 34;
	CopyPToP (gMultiSpecWorkflowInfo.defaultName, (UCharPtr)"\p34_1latest_2016_mint_20170423");
	gConfirmReplacementAlertFlag = FALSE;
	menuData = (kReformatMenuID<<16) + kReformatChangeImageRequest;
	Menus (menuData);
	gConfirmReplacementAlertFlag = TRUE;
				
	gMultiSpecWorkflowInfo.thresholdValue = 35;
	CopyPToP (gMultiSpecWorkflowInfo.defaultName, (UCharPtr)"\p35_1latest_2016_mint_20170423");
	gConfirmReplacementAlertFlag = FALSE;
	menuData = (kReformatMenuID<<16) + kReformatChangeImageRequest;
	Menus (menuData);
	gConfirmReplacementAlertFlag = TRUE;
	
	gMultiSpecWorkflowInfo.functionCode = 99;
	
	gMultiSpecWorkflowInfo.thresholdValue = 25;
	CopyPToP (gMultiSpecWorkflowInfo.defaultName, (UCharPtr)"\p25_3earliest_2016_mint_20170423");
	gConfirmReplacementAlertFlag = FALSE;
	menuData = (kReformatMenuID<<16) + kReformatChangeImageRequest;
	Menus (menuData);
	gConfirmReplacementAlertFlag = TRUE;
	
	gMultiSpecWorkflowInfo.thresholdValue = 26;
	CopyPToP (gMultiSpecWorkflowInfo.defaultName, (UCharPtr)"\p26_3earliest_2016_mint_20170423");
	gConfirmReplacementAlertFlag = FALSE;
	menuData = (kReformatMenuID<<16) + kReformatChangeImageRequest;
	Menus (menuData);
	gConfirmReplacementAlertFlag = TRUE;
	
	gMultiSpecWorkflowInfo.thresholdValue = 27;
	CopyPToP (gMultiSpecWorkflowInfo.defaultName, (UCharPtr)"\p27_3earliest_2016_mint_20170423");
	gConfirmReplacementAlertFlag = FALSE;
	menuData = (kReformatMenuID<<16) + kReformatChangeImageRequest;
	Menus (menuData);
	gConfirmReplacementAlertFlag = TRUE;
	
	gMultiSpecWorkflowInfo.thresholdValue = 28;
	CopyPToP (gMultiSpecWorkflowInfo.defaultName, (UCharPtr)"\p28_3earliest_2016_mint_20170423");
	gConfirmReplacementAlertFlag = FALSE;
	menuData = (kReformatMenuID<<16) + kReformatChangeImageRequest;
	Menus (menuData);
	gConfirmReplacementAlertFlag = TRUE;	
	
	gMultiSpecWorkflowInfo.thresholdValue = 29;
	CopyPToP (gMultiSpecWorkflowInfo.defaultName, (UCharPtr)"\p29_3earliest_2016_mint_20170423");
	gConfirmReplacementAlertFlag = FALSE;
	menuData = (kReformatMenuID<<16) + kReformatChangeImageRequest;
	Menus (menuData);
	gConfirmReplacementAlertFlag = TRUE;
	
	gMultiSpecWorkflowInfo.thresholdValue = 30;
	CopyPToP (gMultiSpecWorkflowInfo.defaultName, (UCharPtr)"\p30_3earliest_2016_mint_20170423");
	gConfirmReplacementAlertFlag = FALSE;
	menuData = (kReformatMenuID<<16) + kReformatChangeImageRequest;
	Menus (menuData);
	gConfirmReplacementAlertFlag = TRUE;
	
	gMultiSpecWorkflowInfo.thresholdValue = 31;
	CopyPToP (gMultiSpecWorkflowInfo.defaultName, (UCharPtr)"\p31_3earliest_2016_mint_20170423");
	gConfirmReplacementAlertFlag = FALSE;
	menuData = (kReformatMenuID<<16) + kReformatChangeImageRequest;
	Menus (menuData);
	gConfirmReplacementAlertFlag = TRUE;
	
	gMultiSpecWorkflowInfo.thresholdValue = 32;
	CopyPToP (gMultiSpecWorkflowInfo.defaultName, (UCharPtr)"\p32_3earliest_2016_mint_20170423");
	gConfirmReplacementAlertFlag = FALSE;
	menuData = (kReformatMenuID<<16) + kReformatChangeImageRequest;
	Menus (menuData);
	gConfirmReplacementAlertFlag = TRUE;
	
	gMultiSpecWorkflowInfo.thresholdValue = 33;
	CopyPToP (gMultiSpecWorkflowInfo.defaultName, (UCharPtr)"\p33_3earliest_2016_mint_20170423");
	gConfirmReplacementAlertFlag = FALSE;
	menuData = (kReformatMenuID<<16) + kReformatChangeImageRequest;
	Menus (menuData);
	gConfirmReplacementAlertFlag = TRUE;
	
	gMultiSpecWorkflowInfo.thresholdValue = 34;
	CopyPToP (gMultiSpecWorkflowInfo.defaultName, (UCharPtr)"\p34_3earliest_2016_mint_20170423");
	gConfirmReplacementAlertFlag = FALSE;
	menuData = (kReformatMenuID<<16) + kReformatChangeImageRequest;
	Menus (menuData);
	gConfirmReplacementAlertFlag = TRUE;
	
	gMultiSpecWorkflowInfo.thresholdValue = 35;
	CopyPToP (gMultiSpecWorkflowInfo.defaultName, (UCharPtr)"\p35_3earliest_2016_mint_20170423");
	gConfirmReplacementAlertFlag = FALSE;
	menuData = (kReformatMenuID<<16) + kReformatChangeImageRequest;
	Menus (menuData);
	gConfirmReplacementAlertFlag = TRUE;
	
/*	
			// Rectify Image
			
	gMultiSpecWorkflowInfo.workFlowCode = 1;
	gConfirmReplacementAlertFlag = FALSE;
	menuData = (kReformatMenuID<<16) + kReformatRectifyImageRequest;
	Menus (menuData);
	gConfirmReplacementAlertFlag = TRUE;
*/	
/*			// Open File resulting image file.

	gDisplaySpecsDefault.lastDisplayType = k3_2_ChannelDisplayType;					
	gDisplaySpecsDefault.displaySet = 6;
	gDisplaySpecsDefault.rgbColors = 5;
	
	gDisplaySpecsDefault.enhancementCode = kLinearStretch;
	gDisplaySpecsDefault.minMaxCode = kPerChannelMinMax;
	gDisplaySpecsDefault.percentTailsClipped = 2;
		
	gDisplaySpecsDefault.dataTypeCode = kIntegerType;
	gDisplaySpecsDefault.numberBytes = 2;
	gDisplaySpecsDefault.numberChannels = 5;
	gDisplaySpecsDefault.signedDataFlag = TRUE;
	gDisplaySpecsDefault.backgroundValueCode = 1;
	gDisplaySpecsDefault.backgroundDataValue = 0;
				
	gDisplaySpecsDefault.magnification = 1/3;
	
	gDisplaySpecsDefault.redChannelNumber = 4;
	gDisplaySpecsDefault.greenChannelNumber = 4;
	gDisplaySpecsDefault.blueChannelNumber = 2;
	gDisplaySpecsDefault.invertValuesFlag[0] = TRUE;
	gDisplaySpecsDefault.invertValuesFlag[1] = TRUE;
	gDisplaySpecsDefault.invertValuesFlag[2] = TRUE;
	gDisplaySpecsDefault.numberLevels = 256;
	
	gDisplaySpecsDefault.structureLoadedFlag = TRUE;
*/	
/*			// Open last file that was saved or provide open image window.
			
	gMultiSpecWorkflowInfo.workFlowCode = 1;
	if (gMultiSpecWorkflowInfo.lastFileSavedInfo.fileName[0] > 0)
		{
				// Open the last file that was saved.
				
		OpenImageFile (&gMultiSpecWorkflowInfo.lastFileSavedInfo);
		gMultiSpecWorkflowInfo.lastFileSavedInfo.fileName[0] = 0;
		
		}		// end "if (gLastFileSavedInfo.fileName[0] > 0)"
	
	else		// gLastFileSavedInfo.fileName[0] == 0
		{
		menuData = (kFileMenuID<<16) + kOpenImage;
		Menus (menuData);
		
		}		// end "else gLastFileSavedInfo.fileName[0] == 0"
*/		
/*			// Set default name to use for the next file to be saved.
			
	RemoveCharsNoCase ((char*)"\p-12.global.gvar.hdf", savedImageFileName);
	ConcatPStrings ((UCharPtr)savedImageFileName, (StringPtr)"\p_global442", 255);
	CopyPToP ((char*)gMultiSpecWorkflowInfo.defaultName,
								(char*)savedImageFileName);
*/	
			// Save Image As
/*
	gMultiSpecWorkflowInfo.workFlowCode = 3;
	menuData = (kFileMenuID<<16) + kSaveOutputAs;
	Menus (menuData);
	gMultiSpecWorkflowInfo.defaultName[0] = 0;
*/	
			// Close Window
			
//	menuData = (kFileMenuID<<16) + kCloseImage;
//	Menus (menuData);
	
			// Close 2nd Window
/*			
	menuData = (kFileMenuID<<16) + kCloseImage;
	Menus (menuData);
*/	
	gMultiSpecWorkflowInfo.workFlowCode = 0;
	
}		// end "DoWorkFlow" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DragMultiSpecWindow
//
//	Software purpose:	This routine handles dragging of a window. The toolbox
//							call for dragging will only be used if sufficient
//							memory is available.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			main
//
//	Coded By:			Larry L. Biehl			Date: 06/11/1996
//	Revised By:			Larry L. Biehl			Date: 02/07/1997		

void DragMultiSpecWindow (
				WindowPtr							theWindow,
				Point									startPoint)

{
	SInt32								freeBytesPossible,
											longestContBlock;


	if (theWindow != NULL)
		{
		PurgeSpace (&freeBytesPossible, &longestContBlock);
		if (longestContBlock > 1000)
			{
			Rect		displayRegionRect;
			
			GetDisplayRegionRectangle ( &displayRegionRect );
			DragWindow (theWindow, startPoint, &displayRegionRect);
			
			}		// end "if (longestContBlock > 1000)"

		}		// end "if (theWindow != NULL)"

}		// end "DragMultiSpecWindow" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ExitMultiSpec
//
//	Software purpose:	This routine close all windows and releases
//							memory before exiting MultiSpec
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			HandleAEQuitApplication in appleEvents.c
//							SendAEQuitApplication in appleEvents.c
//							MyGrowZone in multiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 12/01/1988
//	Revised By:			Larry L. Biehl			Date: 02/12/2010			

void ExitMultiSpec (
				Boolean								forceExitFlag)

{
	WindowPtr							window;
	
	Handle								windowInfoHandle;
	
	UInt32								count;
	
	SInt16								i,
											itemHit,
											windowType;
											
	Boolean								cancelledFlag = FALSE;
	
		
			// Release the spare memory in case it may be needed for saving		
			// files.																				
	
	SetHandleSize (gSpareWarnMemoryH, (Size)0 );
	SetHandleSize (gSpareCodeMemoryH, (Size)0 );
	
			// Make sure the cursor is the pointer.
	
	MInitCursor ();
	
			// Close the project if one is open.											
			
	gDoneFlag = CloseTheProject ();
	
			// Check if any of the image windows need to have the class and/or
			// group information saved.
			
	if (gDoneFlag || forceExitFlag)
		{
				// Now, hide the image and graphics windows.								
				
		for (i=0; i<gNumberOfIWindows; i++)
			{
			window = gWindowList[kNumberBaseWindows+i];
			 
			windowInfoHandle = NULL;
			if (window != NULL)
				windowInfoHandle = (Handle)GetWRefCon(window);
				 
			cancelledFlag = SaveIfWindowChanged (windowInfoHandle, forceExitFlag);
			
			if (cancelledFlag && !forceExitFlag)
				{
				gDoneFlag = FALSE;
				break;
				
				}		// end "if (cancelledFlag && !forceExitFlag)"
		   	
		   }		// end "for (i=0; i<..." 
		
		}		// end "if (gDoneFlag || forceExitFlag)"
	
	if (gDoneFlag && !forceExitFlag)
		{
				// Request user to save the output text window if it has not been saved
				// recently. If this is a forced quit by the application then the user
				// will have a chance to save the output window later on.
			
		Boolean windowChangedFlag = GetOutputWindowChangedFlag ();
		#if use_mlte_for_text_window
			if (windowChangedFlag && TXNDataSize (gOutputTextH) > 2)
		#endif		// use_mlte_for_text_window
		#if !use_mlte_for_text_window
			if (windowChangedFlag && WEGetTextLength (gOutputTextH) > 2)
		#endif		// !use_mlte_for_text_window
			{
			if (LoadSpecifiedStringNumberStringP (
												kAlertStrID, 
												IDS_Alert54, 
												(char*)gTextString,
												(char*)gTextString2,
												TRUE,
												(char*)&gTextString3[1] ) )
				itemHit = DisplayAlert (kSaveAlertID, kCautionAlert, 0, 0, 0, gTextString);
				
			if (itemHit == 1)
				gDoneFlag = SaveTextOutputAs ();
				
			else if (itemHit == 2)
				gDoneFlag = FALSE;
			
			}		// end "if (windowChangedFlag &&  && (*gOutputTextH)->tLength > 2)"
			
		}		// end "if (gDoneFlag && !forceExitFlag)"
	
			// Check if user cancelled 'quit application'								
			
	if (gDoneFlag || forceExitFlag)
		{
				// Release memory for non-project processor structures.				
		
		gProcessorCode = 0;
		ReleaseNonProjProcessorStructures ();
		
				// Hide palette window.															
		
		if (gPaletteWindow != NULL)	
			HideWindow (gPaletteWindow);
		
				// Now, hide the image and graphics windows.								
				
		for (i=0; i<gNumberOfIWindows+gNumberOfGWindows; i++)
			{
			window = gWindowList[kNumberBaseWindows+i];
			if (window != NULL)
		   	HideWindow (window);
		   	
		   }		// end "for (i=0; i<..." 
		
				// Now, hide the text window.													
		      
		HideWindow (gOutputWindow);
		
				// Close palette window.														
		
		//ClosePaletteWindow (gPaletteWindow);
				
				// Close the image and graphics windows.									
				
		while (gNumberOfIWindows+gNumberOfGWindows > 0)
			{
			windowInfoHandle = NULL;
			window = gWindowList[
						kNumberBaseWindows+gNumberOfIWindows+gNumberOfGWindows-1];
						
			if (window != NULL)
				windowInfoHandle = (Handle)GetWRefCon(window);
		
			if (windowInfoHandle != NULL)
				{
				windowType = ((WindowInfoPtr)*windowInfoHandle)->windowType;
		      
			   switch (windowType)
			   	{		
							// Close graphics windows.											
							
	  				case kGraphicsWindowType:
	      			CloseGraphicsWindow (window);
	      			break;
	      						
							// Close image windows												
							
	  				case kImageWindowType:
	  				case kThematicWindowType:
	      			CloseImageWindow (window);
	      			break;
					
					}		// end "switch (windowType)" 
					
				}		// end "if (windowInfoHandle != NULL)" 
				
			else		// windowInfoHandle == NULL
				break;
						
			}		// end "while (gNumberOfIWindows+gNumberOfGWindows > 0)" 
		
				// Close the Palette window
				
		gPaletteWindowHandle = UnlockAndDispose (gPaletteWindowHandle);
		
				// Close the Statistics window
				
		gProjectWindowHandle = UnlockAndDispose (gProjectWindowHandle);
		
				// Close output window															
		
		#if use_mlte_for_text_window
			if (forceExitFlag && TXNDataSize (gOutputTextH) > 2)
		#endif
		#if !use_mlte_for_text_window
			if (forceExitFlag && WEGetTextLength(gOutputTextH) > 2)
		#endif
			SaveTextOutputAs ();
	
		CloseOutputWindow (gOutputWindow);
		gOutputWindow = NULL;
		
				// Dispose of Window Handles and Pointers
		
		for (count=0; count<kMaxNumberIWindows; count++)
			{
			UnlockAndDispose (gImageWindowHandleList[count]);
										
			CheckAndDisposePtr ((Ptr)gGWorldPlaceHolderList[count]);
				
			}		// end "for (count=0; count<gNumberAvailableGWorldPtrs; count++)"
			
				// Dispose of image and vector overlays
		
		CloseAllOverlayFiles ();
			
				// Dispose of other global memory.
		
		ClearTransformationMatrix (FALSE);
					
		gChannelSubsetsH = UnlockAndDispose (gChannelSubsetsH);
			
				// Dispose of the spare memory handles.
				
		gSpareWarnMemoryH = UnlockAndDispose (gSpareWarnMemoryH);
		gSpareCodeMemoryH = UnlockAndDispose (gSpareCodeMemoryH);
		
				// Dispose of the printer handle
		
		#if !TARGET_API_MAC_CARBON
			gHPrint = (THPrint)UnlockAndDispose ((Handle)gHPrint);
		#endif	// !TARGET_API_MAC_CARBON
		
		if (!gOSXFlag)
			{
					// Dispose of regions
			
			if (gTempRegion1 != NULL)
				{
				SetEmptyRgn (gTempRegion1);		
				DisposeRgn (gTempRegion1);
				
				}		// end "if (gTempRegion1 != NULL)"
				
			if (gTempRegion2 != NULL)	
				{
				SetEmptyRgn (gTempRegion2);
				DisposeRgn (gTempRegion2);
				
				}		// end "if (gTempRegion2 != NULL)"
			
			if (gSelectionClipRgn != NULL)
				{	
				SetEmptyRgn (gSelectionClipRgn);
				DisposeRgn (gSelectionClipRgn);
				
				}		// end "if (gSelectionClipRgn != NULL)"
				
			}		// end "if (!gOSXFlag)"
		
		if (gAppleEventsFlag)
  			AEDisposeDesc(&gSelfAddressDescription);	
			
		#if include_gdal_capability
			CloseGDALInterface ();
		#endif	// include_gdal_capability
				
		#if TARGET_API_MAC_CARBON			
//			if (gApplicationServicesBundle != NULL)
//				CFBundleUnloadExecutable (gApplicationServicesBundle);
			
//			if (gApplicationServicesBundle != NULL)	
//				CFRelease (gApplicationServicesBundle);
		#endif	// TARGET_API_MAC_CARBON	
		
		}		// end "if (gDoneFlag || forceExitFlag)" 
		
}		// end "ExitMultiSpec" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void FixCursor
//
//	Software purpose:	This routine sets the cursor to the proper type
//							depending on the mouse location, content of the
//							window, and key board status.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			DoMouseDnContent in multiSpec.c
//							DoMultiSpec in multiSpec.c
//							DoSuspendResume in multiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 09/09/1988
//	Revised By:			Larry L. Biehl			Date: 09/08/2006			

void FixCursor (void)

{
			//	Local Declarations 																
	
	Point									mousePt;
	
	ListHandle							legendListHandle;
	
	SInt16								grafPortType,
											windowType;
	
	Boolean								cursorSetFlag;
									
		
			
	if (gProcessorCode != 0)
		{
		MSetCursor (gPresentCursor);
																						return;
		
		}		// end "if (gProcessorCode != 0)" 


			// Track the mouse and set the cursor appropriately:						
			// [1] ibeam if in output window, arrow if outside							
			// [2] WSIdle our textedit window, so the insertion bar blinks			
			// [3] crossCursor if image window, arrow if outside						
			// [4] hand if image window is not empty, the image can be scrolled,	
			//     the cursor is over the image and the option key is down.		
			
	mousePt = gEventRecord.where;
	GlobalToLocal (&mousePt);
	
	if (gTheActiveWindow == NULL)
		{
		MInitCursor();
																						return;
		
		}		// end "if (gTheActiveWindow == NULL)"

	if (gTheActiveWindow == gOutputWindow)
		{
		#if use_mlte_for_text_window
			TXNAdjustCursor (gOutputTextH, NULL);
			
			TXNIdle (gOutputTextH);
		#endif	// !use_mlte_for_text_window
		#if !use_mlte_for_text_window
			SetRectRgn (gTempRegion1, -0x7FFF, -0x7FFF, 0x7FFF, 0x7FFF) ;			
			if (WEAdjustCursor (gEventRecord.where, gTempRegion1, gOutputTextH))
				gPresentCursor = kIBeam;
			
			else		// !WEAdjustCursor (...
				MInitCursor();					
			SetEmptyRgn (gTempRegion1);
			
			WEIdle (NIL, gOutputTextH);
		#endif	// !use_mlte_for_text_window
																						return;
		
		}		// end "if (gTheActiveWindow == gOutputWindow)" 
	
	if (gTheActiveWindow == gActiveImageWindow)
		{
		cursorSetFlag = FALSE;
		windowType = ((WindowInfoPtr)*gActiveWindowInfoH)->windowType;
		grafPortType = ((WindowInfoPtr)*gActiveWindowInfoH)->grafPortType;
		if (windowType == kThematicWindowType)
			{
			if (gActiveOffscreenMapExists && (gEventRecord.modifiers & shiftKey) &&
																	grafPortType == kCGrafType)
				{
						// Check if within color 'chips' for legend.						
				
				if (((WindowInfoPtr)*gActiveImageWindowInfoH)->showLegend)
					{
					legendListHandle = 
									((WindowInfoPtr)*gActiveWindowInfoH)->legendListHandle;
					gViewRect = ((ListPtr)*legendListHandle)->rView;
					gViewRect.right = gViewRect.left + 15;
					gViewRect.bottom = GetListBottom (legendListHandle);

					if (PtInRect (mousePt, &gViewRect))
						{
						MSetCursor (kBlinkOpenCursor1);
						cursorSetFlag = TRUE;
						
						}		// end "if (PtInRect (mousePt, &gViewRect)\)" 
						
					}		// end "if (((WindowInfoPtr)*gActiveImageWindowInfoH)->..." 
					
				if (!cursorSetFlag)
					{
					GetWindowClipRectangle (gActiveImageWindow, kImageArea, &gViewRect);
					if (PtInRect (mousePt, &gViewRect))
						{
						MSetCursor (kBlinkOpenCursor2);
						cursorSetFlag = TRUE;
							
						}		// end "if ( PtInRect (mousePt, &gViewRect) )" 
					
					}		// end "if (!cursorSetFlag)" 
					
				}		// end "if (gActiveOffscreenMapExists && ...)" 
			
			if (!cursorSetFlag)
				{
					// Check if within legend 'adjuster' area.							
				
				GetWindowPortBounds (gActiveImageWindow, &gTempRect);	
//				gViewRect.top = gViewRect.bottom = gActiveImageWindow->portRect.bottom;
				gViewRect.top = gViewRect.bottom = gTempRect.bottom;
				gViewRect.top -= kSBarWidth;
				
//				gViewRect.left = gViewRect.right = 
//										gActiveImageWindow->portRect.left + gActiveLegendWidth;
				gViewRect.left = gViewRect.right = gTempRect.left + gActiveLegendWidth;
				gViewRect.right += kLegendWidthChanger;
				
				if ( PtInRect (mousePt, &gViewRect) )
					{
					MSetCursor (kLegendWidth);
					cursorSetFlag = TRUE;
					
					}		// end "if ( PtInRect (mousePt, &gViewRect) )" 
					
				}		// end "if (!cursorSetFlag)" 
				
			if (!cursorSetFlag && !gActiveOffscreenMapExists)
				{
				MInitCursor();
				cursorSetFlag = TRUE;
				
				}		// end "if (!cursorSetFlag && !gActiveOffscreenMapExists)" 
			
			}		// end "if ( windowType == kThematicWindowType)" 
			
		if (gActiveOffscreenMapExists && !cursorSetFlag)
			{
			GetWindowClipRectangle (gActiveImageWindow, kImageArea, &gViewRect);
			if ( PtInRect (mousePt, &gViewRect) )
				{
				if ( gEventRecord.modifiers & optionKey )
					{
					MSetCursor (kHand);
					cursorSetFlag = TRUE;
					
					}		// end "if ( gEventRecord.modifiers & optionKey )" 
					
				else
					{
					MSetCursor (kCross);
					UpdateCursorCoordinates (mousePt);
					
					}		// end "else !(gEventRecord.modifiers & optionKey)" 
					
				}		// end "if ( PtInRect (mousePt, &gViewRect) )" 
				
			else		// !PtInRect (mousePt, &gViewRect)
				{
				if (gPresentCursor != kArrow)
					ClearCoordinates ();
					
				MInitCursor();
				
				}		// end "else !PtInRect (mousePt, &gViewRect) )" 
				
			}		// end "if (gActiveOffscreenMapExists)" 
			
		}		// end "if (gTheActiveWindow && (gTheActiveWindow == ..." 
		
	else		// gTheActiveWindow != gActiveImageWindow
		MInitCursor();
				
}		// end "FixCursor" 


/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		GDHandle GetDeviceMouseIsIn
//
//	Software purpose:	This routine obtains the device that the mouse is currently
//							within.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			DoWindowZoomInOutEvent
//
//	Coded By:			Larry L. Biehl			Date: 07/07/2005
//	Revised By:			Larry L. Biehl			Date: 07/07/2005			

GDHandle GetDeviceMouseIsIn ()

{
	GDHandle								currentDevice,
											device;


			// Get the device that the mouse cursor is  in.					
				
	device = GetDeviceList ();

	currentDevice = NULL;
	
	while (device && currentDevice == NULL)
		{
		gTempRect = (*device)->gdRect;
		if ( TestDeviceAttribute (device, screenDevice) &&
			  TestDeviceAttribute (device, screenActive) &&
			  PtInRect (gEventRecord.where, &gTempRect) )
			currentDevice = device;
			
		device = GetNextDevice (device);
		
		}		// end "while (device && ..."
		
	return (currentDevice);
				
}		// end "GetDeviceMouseIsIn" 
*/


//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		GDHandle GetDevicePointIsIn
//
//	Software purpose:	This routine obtains the device that the mouse is currently
//							within.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			DoWindowZoomInOutEvent
//
//	Coded By:			Larry L. Biehl			Date: 07/11/2011
//	Revised By:			Larry L. Biehl			Date: 07/11/2011			

GDHandle GetDevicePointIsIn (
				Point*								locationPtr)

{
	GDHandle								currentDevice,
											device;


			// Get the device that the mouse cursor is  in.					
				
	device = GetDeviceList ();

	currentDevice = NULL;
	
	while (device && currentDevice == NULL)
		{
		gTempRect = (*device)->gdRect;
		if ( TestDeviceAttribute (device, screenDevice) &&
			  TestDeviceAttribute (device, screenActive) &&
			  PtInRect (*locationPtr, &gTempRect) )
			currentDevice = device;
			
		device = GetNextDevice (device);
		
		}		// end "while (device && ..."
		
	return (currentDevice);
				
}		// end "GetDevicePointIsIn" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetDisplayRegionRectangle
//
//	Software purpose:	This routine obtains the current rectangle that includes
//							the entire display area less a 3 pixel border.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/07/1997
//	Revised By:			Larry L. Biehl			Date: 06/02/2003			

void GetDisplayRegionRectangle ( 
				Rect*									displayRegionRectPtr )

{
	RgnHandle grayRegionHandle = GetGrayRgn();
	
	if (grayRegionHandle != NULL)
		{
		GetRegionBounds(grayRegionHandle, displayRegionRectPtr);
		InsetRect (displayRegionRectPtr, 3, 3);
		
		}		// end "if (grayRegionHandle != NULL)"
/*
	#if TARGET_API_MAC_CARBON
				// for OSX
				
		if (gOSXFlag)
			{
			Rect									availableRect;
	
			GDHandle								device;
													
			OSStatus 							status;
			
					// This is for testing for now.				
						
			device = GetDeviceList ();
			while (device != NULL)
				{
				status = GetAvailableWindowPositioningBounds (device, &availableRect);
				device = GetNextDevice (device);
				
				}		// end "while (device != NULL)" 
		  	
		  	}		// end "if (gOSXFlag)"
	  	
	#endif		// TARGET_API_MAC_CARBON
*/				
}		// end "GetDisplayRegionRectangle" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetMaximumSizeForDisplayPointIsIn
//
//	Software purpose:	This routine determines the maximum display area for the display
//							that the input point is in.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 07/11/2011
//	Revised By:			Larry L. Biehl			Date: 07/12/2011			

void GetMaximumSizeForDevicePointIsIn ( 
				Point*								locationPtr,
				Rect*									maximumRectPtr)

{	
	GDHandle								currentDevice;
											
			
			// Get the device that the input location is in.		
			
	currentDevice = GetDevicePointIsIn (locationPtr);
				
	if (currentDevice)
		{
				// Get the maximum window possible for the device.			
		
		*maximumRectPtr = (*currentDevice)->gdRect;
		maximumRectPtr->top += 21;
		maximumRectPtr->left += 4;
		maximumRectPtr->bottom -= 4;
		maximumRectPtr->right -= 4;
		
				// Adjust for menu bar height if needed.						
			
		if ( currentDevice == GetMainDevice() )
			maximumRectPtr->top += GetMBarHeight();
		
		#if TARGET_API_MAC_CARBON
					// for OSX
					
			if (gOSXFlag)
				{																
				OSStatus 							status;
				
						// This is for testing for now.				
							
				status = GetAvailableWindowPositioningBounds (currentDevice, maximumRectPtr);
				
				maximumRectPtr->top += 24;
				maximumRectPtr->left += 4;
				maximumRectPtr->bottom -= 4;
				maximumRectPtr->right -= 4;
				
				}		// end "if (gOSXFlag)"
			
		#endif		// TARGET_API_MAC_CARBON
		
		}		// end "if (currentDevice)" 
						
}		// end "GetMaximumSizeForDisplayPointIsIn" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetMinimumDisplaySizeForImage
//
//	Software purpose:	This routine determines the minimum display area needed for 
//							the image currently being displayed in the window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			DoWindowZoomInOutEvent
//
//	Coded By:			Larry L. Biehl			Date: 12/17/2008
//	Revised By:			Larry L. Biehl			Date: 12/17/2008			

void GetMinimumDisplaySizeForImage ( 
				Handle								windowInfoHandle,
				LongPoint*							minimumWindowSizePointPtr)

{	
	LongPoint							offScreenPoint;
	
	DisplaySpecsPtr					displaySpecsPtr = NULL;
	WindowInfoPtr						windowInfoPtr = NULL;
	
	SInt16								savedOrigin[2];
	
	
	if (windowInfoHandle != NULL)		
		windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle,
																			kNoLock,
																			kNoMoveHi);
																		
	if (windowInfoPtr != NULL && windowInfoPtr->displaySpecsHandle != NULL)
		displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
																		windowInfoPtr->displaySpecsHandle,
																		kNoLock,
																		kNoMoveHi);
			
	if (displaySpecsPtr != NULL)
		{
		savedOrigin[kVertical] = displaySpecsPtr->origin[kVertical];
		savedOrigin[kHorizontal] = displaySpecsPtr->origin[kHorizontal];
		
		displaySpecsPtr->origin[kVertical] = 0;
		displaySpecsPtr->origin[kHorizontal] = 0;
		
		offScreenPoint.v = displaySpecsPtr->imageDimensions[kVertical];
		offScreenPoint.h = displaySpecsPtr->imageDimensions[kHorizontal];
		ConvertOffScreenPointToWinPoint ( 
								windowInfoHandle, &offScreenPoint, minimumWindowSizePointPtr);
				
		displaySpecsPtr->origin[kVertical] = savedOrigin[kVertical];
		displaySpecsPtr->origin[kHorizontal] = savedOrigin[kHorizontal];
		
				// Adjust the size needed to allow for scroll bars and	
				// one pixel extra.													
				
		minimumWindowSizePointPtr->v += kSBarWidth + 1;
		minimumWindowSizePointPtr->h += kSBarWidth + 1;
		
				// The image size needed has to be larger than the 		
				// minimum.																
				
		minimumWindowSizePointPtr->v = MAX (minimumWindowSizePointPtr->v, 
																gGrowAreaMinimum);
		minimumWindowSizePointPtr->h = MAX (minimumWindowSizePointPtr->h,  
													gGrowAreaMinimum + windowInfoPtr->legendWidth);
				
		}		// end "if (displaySpecsPtr != NULL)"
		
	else		// displaySpecsPtr == NULL
		{
		minimumWindowSizePointPtr->h = gGrowAreaMinimum;
		minimumWindowSizePointPtr->v = gGrowAreaMinimum;
		
		}		// end "else		// displaySpecsPtr == NULL"
				
}		// end "GetMinimumDisplaySizeForImage" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GrowNonGraphWindow
//
//	Software purpose:	This routine handles growing non-graphics windows.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			DoNonGraphWindowGrow
//							SetWindowToImageSize in SDisplay.cpp
//
//	Coded By:			Larry L. Biehl			Date: 09/09/1988
//	Revised By:			Larry L. Biehl			Date: 09/28/2006			

void GrowNonGraphWindow (
				WindowPtr							theWindow,
				Handle								windowInfoHandle,
				SInt16								height,
				SInt16								width)

{
			//	Local Declarations 																
	
	Rect									rect,
											scrollBar;
	
	ControlHandle						controlHandle,
											rootControlHandle;
											
//	SInt32								size;
	
	SInt16								refCon;
	
	UInt16								index,
											numberControls;
	
	Boolean								tallerFlag;
	
	
	tallerFlag = FALSE;
	
	GetWindowPortBounds (theWindow, &rect);
  	ClipRect (&rect);

			// Erase growicon																		

  	scrollBar.top    = rect.bottom - kSBarWidth;
  	scrollBar.left   = rect.right - kSBarWidth;
  	scrollBar.bottom = rect.bottom;
  	scrollBar.right  = rect.right;
  				
   EraseRect (&scrollBar);
  	InvalWindowRect (theWindow, &scrollBar);


			// Invalidate vertical scroll area of old window and that portion of the
			// coordinate view above the scroll bar if new window is 	
			// wider than old one																

	if (width > rect.right)
		{
		scrollBar.top    = rect.top + ((WindowInfoPtr)*windowInfoHandle)->coordinateHeight;
		scrollBar.left   = rect.right - kSBarWidth - 1;
  		if (theWindow == gOutputWindow)
  			scrollBar.left -= 3;
		scrollBar.bottom = rect.bottom;
		scrollBar.right  = rect.right;
		
   	EraseRect (&scrollBar);
  		InvalWindowRect (theWindow, &scrollBar);
		
		scrollBar.top = rect.top;
		scrollBar.bottom = rect.top + ((WindowInfoPtr)*windowInfoHandle)->coordinateHeight;
  		InvalWindowRect (theWindow, &scrollBar);
  		
  		if (((WindowInfoPtr)*windowInfoHandle)->titleHeight > 0)
  			{
  					// Invalidate the entire title
  					
			scrollBar.top = ((WindowInfoPtr)*windowInfoHandle)->coordinateHeight;
			scrollBar.left = 0;
			scrollBar.bottom = scrollBar.top + ((WindowInfoPtr)*windowInfoHandle)->titleHeight;
   		EraseRect (&scrollBar);
  			InvalWindowRect (theWindow, &scrollBar);
  			
  			}		// end "if (((WindowInfoPtr)*windowInfoHandle)->titleHeight > 0)"
		
		}		// end "if (width > rect.right)" 
		
	else if (theWindow == gOutputWindow)
		{
		scrollBar.top    = rect.top;
		scrollBar.left   = width - kSBarWidth - 4;
		scrollBar.bottom = rect.bottom;
		scrollBar.right  = scrollBar.left + 4;
		
   	EraseRect (&scrollBar);
  		InvalWindowRect (theWindow, &scrollBar);
		
		}		// end "else if (theWindow == gOutputWindow)"

			// Invalidate horizontal scroll area of old window if new window is	
			// taller than old one																

	if (height > rect.bottom)
		{
  		scrollBar.top    = rect.bottom - kSBarWidth - 1;
  		scrollBar.left   = rect.left;
  		scrollBar.bottom = rect.bottom;
  		scrollBar.right  = width;
  		
  		if (theWindow == gOutputWindow)
  			{
  			scrollBar.top -= 8;
  			scrollBar.bottom -= 4;
  			
  			}		// end "if (theWindow == gOutputWindow)"
  			
   	EraseRect (&scrollBar);
  		InvalWindowRect (theWindow, &scrollBar);
  		
  		tallerFlag = TRUE;
  		
  		}		// end "if (height > rect.bottom)" 
		
	else if (theWindow == gOutputWindow)
		{
  		scrollBar.top    = height - kSBarWidth - 4;
  		scrollBar.left   = rect.left;
  		scrollBar.bottom = scrollBar.top + 4;
  		scrollBar.right  = width;
   	EraseRect (&scrollBar);
  		InvalWindowRect (theWindow, &scrollBar);
		
		}		// end "else if (theWindow == gOutputWindow)"

   SizeWindow (theWindow, width, height, TRUE);
  	
			// Hide the controls to be ready for updating.		
							
	if (gAppearanceManagerFlag)
		{
		GetRootControl(theWindow, &rootControlHandle);
		
		if (rootControlHandle != NULL)
			{
			CountSubControls (rootControlHandle, &numberControls);
			for (index=numberControls; index>=1; index--)
				{
				GetIndexedSubControl(rootControlHandle, index, &controlHandle);
				refCon = (SInt16)GetControlReference (controlHandle);
				
				if (refCon < kCoordinateUnitsControl)
		  			HideControl ( controlHandle );
		     			
		     	}		// end "for (index=numberControls; index>=1; index--)" 
		     	
			}		// end "if (rootControlHandle != NULL)"
	     	
		}		// end "if (gAppearanceManagerFlag)"
		
	#if !TARGET_API_MAC_CARBON
		else		// !gAppearanceManagerFlag
			{
			controlHandle = (ControlHandle)((WindowPeek)theWindow)->controlList;
			while (controlHandle != NULL)
				{
				refCon = (SInt16)GetControlReference (controlHandle);
				
				if (refCon < kCoordinateUnitsControl)
		  			HideControl ( controlHandle );
		  			
		     	controlHandle = (*controlHandle)->nextControl;
		     			
		     	}		// end "while (controlHandle != NULL)"
		     	
			}		// end "else !gAppearanceManagerFlag" 
	#endif	// !TARGET_API_MAC_CARBON
	
	AdjustLegendListLength (theWindow, tallerFlag);
	
	DoOutputWindowGrow (theWindow, windowInfoHandle);
     	
	UpdateScrolls (theWindow);
  		
	DrawScrollBars (theWindow);

}		// end "GrowNonGraphWindow"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal long int MyGrowZone
//
//	Software purpose:	This routine handles the case when there is not
//							enough memory to continue with the present process.
//							If 'gMemoryTypeNeeded' = 0, then present warning
//							message with number of bytes needed. If =1, then
//							release spare memory for code resources and present
//							low memory warning message, if =2, then do nothing
//							since this is a request for memory to allocate
//							for the spare memory handles.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			Mac OS Memory Manager
//
//	Coded By:			Larry L. Biehl			Date: 09/01/1988
//	Revised By:			Larry L. Biehl			Date: 10/19/1995			

pascal long int MyGrowZone (
				Size									cbNeeded)

{
	Str31									tempString;
	
	double								doubleBytesNeeded;
	
	Handle								saveHandle;
	SignedByte							handleStatus;
	
	SInt32								returnValue,
											theA5;
									
	UInt32								freeBytesPossible,
											longestContBlock,
											longIntBytesNeeded,
											spareSize,
											spareSpace = 0;
	
	SInt16								numberTryLimit = 5,
											returnCode,
											stopAlertId;
									
	
			// Initialize local values.														
	
	theA5 = SetCurrentA5();		
	returnValue = 0;
	returnCode = noErr;
	
			// Get the space available.														
			
	PurgeSpace ( (SInt32*)&freeBytesPossible, (SInt32*)&longestContBlock);
	
			// Get the number of tries for attempting to get the memory.			
			// Memory types of 0 and 2 will be okay.  If the memory type is 1, 	
			// i.e. memory for code, then set the number of tries to 1.				
			
	if (gMemoryTypeNeeded == 1)
		gGetMemoryTries = 1;
		
	else		// gMemoryTypeNeeded != 1 
		gGetMemoryTries++;
	
			// If this is the first try then, purge and compact the heap to 		
			// obtain the needed amount if 'PurgeSpace' indicates that it is		
			// possible to do.																	
	
	if (gGetMemoryTries == 1)
		{
		if (longestContBlock >= cbNeeded)
			{
			PurgeMem (cbNeeded);
			returnCode = MemError ();
			if (returnCode == noErr)
				{
						// There should be enough space available.						
			
				handleStatus = -1;
				saveHandle = GZSaveHnd ();
				if (saveHandle)
					{
					handleStatus = HGetState (saveHandle);
					HLock (saveHandle);
				
					}		// end "if (saveHandle)" 
			
				longestContBlock = (UInt32)CompactMem ( (SInt16)freeBytesPossible );
		
				if (saveHandle)
					HSetState (saveHandle, handleStatus);
					
				}		// end "if (returnCode == noErr)" 
				
			}		// end "if (longestContBlock >= cbNeeded)" 
		
				// If the memory request can be satisfied (with some spare room), 
				// then have the memory manager try again.								
				// Otherwise indicate that the first try did not work.  Present 	
				// the appropriate message.															
			
		if (longestContBlock >= cbNeeded + spareSpace && returnCode == noErr)
			{
			gGetMemoryTries = 0;
			#if !TARGET_API_MAC_CARBON
				theA5 = SetA5 (theA5);
			#endif	// !TARGET_API_MAC_CARBON
																					return (1);
			
			}		// end "if (longestContBlock > cbNeeded + spareSpace && ...)" 
																				
		else		// longestContBlock < cbNeeded + spareSpace 
			{
			gGetMemoryTries = 2;
				
					// Release spare memory, if available, to make certain that 	
					// warning message can be displayed.									
				
			spareSize = 0;
			if (gSpareWarnMemoryH != NULL)
				spareSize = (UInt32)GetHandleSize (gSpareWarnMemoryH);
				
			if (spareSize > 0)
				SetHandleSize (gSpareWarnMemoryH, (Size)0 );
				
			}		// end "else longestContBlock < cbNeeded + spareSpace" 
			
		}		// end "if ( gGetMemoryTries == 1)" 
			
	if ( gGetMemoryTries > 2 )
		{
				// We are now trying to get memory to display the memory alert 	
				// ... a bad situation.  After 4 tries exit MultiSpec; 				
				// otherwise we are liable to crash the system.							
		
		if (gGetMemoryTries < numberTryLimit)
			{
			PurgeMem ( (SInt32)freeBytesPossible );
			longestContBlock = (UInt32)CompactMem ( (SInt32)freeBytesPossible );
			if (longestContBlock >= cbNeeded)
				{
				#if !TARGET_API_MAC_CARBON
					theA5 = SetA5 (theA5);
				#endif	// !TARGET_API_MAC_CARBON
																					return (1);
																					
				}		// end "if (longestContBlock >= cbNeeded)" 
																					
			else		// if (longestContBlock < cbNeeded) 
				gGetMemoryTries = numberTryLimit;
			
			}		// end "if (gGetMemoryTries < numberTryLimit)" 
			
		if (gGetMemoryTries >= numberTryLimit)
			{
					// Exit MultiSpec.															
			
			if (gGetMemoryTries == numberTryLimit)
				{
				stopAlertId = 1157;
				if (gMemoryTypeNeeded != 1 && gMemoryTypeNeeded != -1)
					stopAlertId = 1158;
				returnCode = StopAlert (stopAlertId, NIL);
				
				ExitMultiSpec (TRUE);
				
				}		// end "if (gGetMemoryTries == numberTryLimit)" 
			
			#if !TARGET_API_MAC_CARBON
				theA5 = SetA5 (theA5);
			#endif	// !TARGET_API_MAC_CARBON
			ExitToShell ();
			
			}		// end "if (gGetMemoryTries >= numberTryLimit)" 
				
		}		// end "if ( gGetMemoryTries > 2 )" 
		
			// A message will be displayed if the try count is 2.						
			// Only try counts of 2 should be at this point in the code.			
			
	if (gGetMemoryTries != 2)
		{
		#if !TARGET_API_MAC_CARBON
			theA5 = SetA5 (theA5);
		#endif	// !TARGET_API_MAC_CARBON
																						return (0);
																						
		}		// end "if (gGetMemoryTries != 2)" 
	
			// If this is memory to satisfy a data request, display a message		
			// indicating that there is not enough memory to continue present		
			// request.																				
	
	if (gMemoryTypeNeeded == 0)
		{
				// Set code to indicate that memory is short;  It could be due to	
				// the text window.																
		
		if (gTextMemoryShortCode == 0)
			gTextMemoryShortCode++;
		
				// Present a dialog box to the user as long as the text memory		
				// short code is less than 2.  This is being so that we do not 	
				// get into an infinite loop with the output text						
				// window being updated after a memory short alert when the 		
				// window update itself is what is causing the memory short 		
				// alert.																			
		
		if (gTextMemoryShortCode < 2 && !gMemoryAlertDisplayFlag)
			{
					// This code was inserted because the C compiler does not 		
					// handle an unsigned long integer properly.							
					
			longIntBytesNeeded = (UInt32)(0x7fffffff & cbNeeded);
			doubleBytesNeeded = longIntBytesNeeded;
			if ( 0x80000000 & cbNeeded )
				{
				doubleBytesNeeded += LONG_MAX;
				doubleBytesNeeded++;
				
				}		// end "if ( 0x80000000 & cbNeeded )" 
				
			doubleBytesNeeded -= (longestContBlock - spareSpace);
				
			PauseIfInBackground ();
				
			sprintf( (char*)&tempString, "%.0f", doubleBytesNeeded);
			CopyCStringToPascal ( (char*)&tempString, tempString );
			
			SInt16 numberChars = tempString[0];
			numberChars = InsertCommasInNumberString (
										(char*)&tempString[1], 
										numberChars, 
										-1,
										numberChars);
			tempString[0] = numberChars;
	
			ParamText ( (ConstStr255Param)&tempString, "\p", "\p", "\p"); 
			returnCode = StopAlert (kMemoryAlertID, NIL);
			
			gMemoryAlertDisplayFlag	= TRUE;
			
			}		// end "if (gTextMemoryShortCode < 2 && !gMemoryAlertDisplayFlag)" 
		
		SetHandleSize (gSpareWarnMemoryH, (Size)gSpareWarnSize);
		returnCode = MemError ();
		
		gMemoryError = 1;
		gGetMemoryTries = 0;
		#if !TARGET_API_MAC_CARBON
			theA5 = SetA5 (theA5);
		#endif	// !TARGET_API_MAC_CARBON
																					return (0);
		
		}		// end "if (gMemoryTypeNeeded == 0)" 
		
			// If this is memory needed to load code resources in then release	
			// the reserve memory to satisfy it and display a low memory 			
			// warning.																				
	
	if (gMemoryTypeNeeded == 1 || gMemoryTypeNeeded == -1)
		{
		if (gSpareCodeMemoryH != NULL)
			{
			spareSize = (UInt32)GetHandleSize (gSpareCodeMemoryH);
			
			if (spareSize > 0)
				SetHandleSize (gSpareCodeMemoryH, (Size)0 );
				
			PurgeMem ( (SInt32)freeBytesPossible );
			longestContBlock = (UInt32)CompactMem ( (SInt32)freeBytesPossible );
				
			}		// end "if (gSpareCodeMemoryH != NULL)" 
			
				// Indicate that spare memory had to be released to load in 		
				// the code resource.															
				
		gMemoryTypeNeeded = -1;
				
				// Warn the user that memory is low. Spare memory for the 			
				// warning message has already been released.							
		
		returnCode = CautionAlert (1159, NIL);
		#if !TARGET_API_MAC_CARBON
			theA5 = SetA5 (theA5);
		#endif	// !TARGET_API_MAC_CARBON																						return (1);
		
		}		// end "if (gMemoryTypeNeeded == 1 && gSpareCodeMemoryH != NULL)" 
		
			// If this is memory needed to expand the spare handles, then			
			// present a caution alert warning the user that memory is low.		
			
	if (gMemoryTypeNeeded == 2)
		{
		PauseIfInBackground ();
			
		returnCode = CautionAlert (kMemoryWarningAlertID, NIL);
		#if !TARGET_API_MAC_CARBON
			theA5 = SetA5 (theA5);
		#endif	// !TARGET_API_MAC_CARBON																						return (0);
			
		}		// end "if (gMemoryTypeNeeded == 2)" 
		
	#if !TARGET_API_MAC_CARBON
		theA5 = SetA5 (theA5);
	#endif	// !TARGET_API_MAC_CARBON
	return ( returnValue );
		
}		// end "MyGrowZone" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SendMenuClearSelectionRectangleCommand
//
//	Software purpose:	This routine sends a command to the Edit-Clear Selection Rectangle
//							menu.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			DoMultiSpec in multiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 02/17/1998
//	Revised By:			Larry L. Biehl			Date: 02/17/1998			

void SendMenuClearSelectionRectangleCommand (void)

{
	SInt32								menuData;
	
	UInt32								lenText;
								
									
	menuData = kEditMenuID;
	menuData = (menuData<<16) + kClearSelectionRect;
	HiliteMenu (kEditMenuID);
	Delay (7, (UInt32*)&lenText);
	UpdateMenuBeforeSelection();
	Menus (menuData);
	
}	// end of SendMenuClearSelectionRectangleCommand 

	

//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateCursorCoordinates
//
//	Software purpose:	The purpose of this routine is to update the cursor
//							coordinates in the coordinate dialog window if it is
//							showing.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			FixCursor in multiSpec.c
//							PolygonSelection in selectionArea.c
//							RectangleSelection in selectionArea.c
//
//	Coded By:			Larry L. Biehl			Date: 06/22/1992
//	Revised By:			Larry L. Biehl			Date: 12/08/2000	

void	UpdateCursorCoordinates (
				Point									mousePt)

{
	LongPoint							longMousePt;
											
	Boolean								updateMapCoordinateFlag;
	
		
	if (GetCoordinateHeight (gActiveImageWindowInfoH) > 0)
		{
		longMousePt.v = mousePt.v;
		longMousePt.h = mousePt.h;
		
		updateMapCoordinateFlag = GetCursorCoordinates (&longMousePt);
		
		if (updateMapCoordinateFlag)
			DrawCursorCoordinates (gActiveImageWindowInfoH);
			
		}		// end "if (GetCoordinateHeight (gActiveImageWindowInfoH) > 0)" 

}		// end "UpdateCursorCoordinates" 
