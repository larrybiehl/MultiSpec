//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						MUtilities.c
//
//	Authors:					Larry L. Biehl, Ravi Budruk
//
//	Revision date:			08/16/2018
//
//	Language:				C
//
//	System:					Macintosh Operating System
//
//	Brief description:	The purpose of the routines in this file is to provide 
//								utility type functions in MultiSpec.
//
//	Functions in file:	Boolean 				ButtonDownEvent
//								void 					CopyOffScreenImage
//								Boolean				ListSpecifiedString
//								SInt32				MGetPixRowBytes
//								void 					UpdateDeskScrap
//								void					UpdateTextEditScrap
//								void					WriteInterfacePreferenceValue
//
//------------------------------------------------------------------------------------

//#include 	"SMultiSpec.h"
//#include 	"SExtGlob.h"

#include		"ctype.h"
						

extern void WriteInterfacePreferenceValue (
				Boolean								fullVersionFlag);


		// Prototypes for routines in this file that are only called by other routines 
		//	in this file.


/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		AppendPascaltoPascal
//
//	Software purpose:	This routine appends a pascal string to the end
//							of a pascal string
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/04/1988
//	Revised By:			Larry L. Biehl			Date: 11/04/1988

void AppendPascaltoPascal (
				UCharPtr								appendStringPtr, 
				char*									stringPtr)

{
	SInt16								lengthString,
											lengthAppendString;
	
	
	lengthString = *stringPtr;
	lengthAppendString = *appendStringPtr;
	
	*stringPtr += lengthAppendString;
	 
	stringPtr = &stringPtr[++lengthString];
	appendStringPtr++;
	
	BlockMoveData ((char*)appendStringPtr, stringPtr, lengthAppendString);

}	// end "AppendPascaltoPascal" 
*/
	

//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ButtonDownEvent
//
//	Software purpose:	This routine handles mouse down events in specified
// 						rectangles.  The rectangle is inverted and a check is 
//							made to determine if the mouse was still within the 
//							rectangle when the mouse up event occurs.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	TRUE if mouse still within the control rectangle
//							FALSE if not
// 
// Called By:			UpdateLineColumnValues in dialogUtilities.c
//							StatisticsWControlEvent in statistics.c
//
//	Coded By:			Larry L. Biehl			Date:11/28/1988
//	Revised By:			Larry L. Biehl			Date:06/10/1993			

Boolean ButtonDownEvent (
				Rect*									rectPtr, 
				Boolean								waitOnMouseUpFlag)

{
	Point									presentPt;
	
	Boolean								lastPointInRectFlag,
											returnFlag;
	
			
	returnFlag = TRUE;
	
			// Invert the rectangle.															
			
	InvertRect (rectPtr);
	
	if (waitOnMouseUpFlag)
		{
				// Wait until button goes up													
		
		do
			{
			GetMouse (&presentPt);
			lastPointInRectFlag = PtInRect (presentPt, rectPtr);
			
			if ((lastPointInRectFlag && !returnFlag) || 
															(!lastPointInRectFlag && returnFlag))
				{
				InvertRect (rectPtr);
				returnFlag = !returnFlag;
				
				}	// end "if (PtInRect (presentPt, rectPtr) && ..." 
			
			}	while (StillDown ());
			
				// Clear the event of the mouse up event.									
				
		FlushEvents (mUpMask,0);
						
		}	// end "if (waitOnMouseUpFlag)" 
		
	else	// !waitOnMouseUpFlag 
		Delay (7, (UInt32*)&gNextTime);
		
			// Invert the the rectangle that encloses the button again to make	
			// it normal.																			
	
	if (returnFlag)		
		InvertRect (rectPtr);
																				
	return (returnFlag);

}	// end "ButtonDownEvent" 
				


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
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
	
	//#if TARGET_API_MAC_CARBON
		CGContextRef						context = NULL;
	//#endif	// TARGET_API_MAC_CARBON
	
			// Intialize local variables.													
	
	windowInfoHandle = (Handle)GetWRefCon (theWindow);
	windowInfoPtr = (WindowInfoPtr)*windowInfoHandle;
	
	imageBaseAddressH = windowInfoPtr->imageBaseAddressH;
	
	if (imageBaseAddressH || windowInfoPtr->offscreenGWorld) 
		{	
		windowInfoPtr = (WindowInfoPtr)GetHandleStatusAndPointer (
																				windowInfoHandle,
																				&windowHandleStatus);
		
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

		//#if TARGET_API_MAC_CARBON
			if (gOSXCoreGraphicsFlag && imageBaseAddressH != NULL)
				{
				LongRect								lDestinationRect;
				Rect									imageRect;
				//CGContextRef						context;
				CGRect								rectangle;
				
				float									vOffset;
				
				UInt32								vMagnificationSize;
														//hMagnificationSize;
				/*											
				PixMapHandle        				portPixMap;
				Rect									pixRect;
				Rect									portRect;
				CGrafPtr								cGrafPtr;
				
				SInt16								portHOrigin;
														//portVOrigin;
				*/											
											
				displaySpecsPtr = (DisplaySpecsPtr)*displaySpecsHandle;
				
				//hMagnificationSize = (UInt32)(
				//		magnification * displaySpecsPtr->imageDimensions[kHorizontal] + .5);
						
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
						/*		
						sourceRect = *inSourceRect;
						
						destinationRect.bottom = 0;
						destinationRect.top = (SInt16)vMagnificationSize;
						destinationRect.left = (SInt16)legendWidth;
						destinationRect.right = (SInt16)hMagnificationSize;
						*/
						ExitMultiSpec (TRUE);
						break;
						
					case kScrollCopy:
								// Copy portion of offscreen image map that fits in the 		
								// invalid region of the window. 	This is from a scroll 	
								// operation.																
								
						//GetRegionBounds (invalidRgn, &destinationRect);
								
					case kDestinationCopy:
								// Copy portion of offscreen map that fits within the			
								// input destination area of the window.  This is from		
								// a window update event.												
						
						//if (copyType == kDestinationCopy)
						//	destinationRect = *inSourceRect;
																		
						lDestinationRect.bottom = 0;
						lDestinationRect.top = displaySpecsPtr->imageDimensions[kVertical];
						lDestinationRect.left = (SInt32)legendWidth;
						lDestinationRect.right = (SInt32)(legendWidth + 
													displaySpecsPtr->imageDimensions[kHorizontal]);
						break;
					
					}	// end "switch (copyType)"
				/*					
				cGrafPtr = GetWindowPort (theWindow);
				portPixMap = GetPortPixMap (cGrafPtr);
				
				GetPortBounds (cGrafPtr, &portRect);
				GetPixBounds (portPixMap, &pixRect);
				
				portHOrigin = -1000;
				
				SetOrigin (portHOrigin, 0);
				
				GetPortBounds (cGrafPtr, &portRect);
				GetPixBounds (portPixMap, &pixRect);
				*/			
 				CreateCGContextForPort (GetWindowPort (theWindow), 
													&context);
													
				gCGContextSetShouldAntialiasPtr (context, 0);
				
				gCGContextSetInterpolationQualityPtr (context, kCGInterpolationNone);

						// Save the graphics state so we can start fresh each time.
						
				//gCGContextSaveGStatePtr (context);

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
			
				gCGContextTranslateCTMPtr (context, 
													(float)(legendWidth - hOrigin * magnification), 
													vOffset);
								
				gCGContextScaleCTMPtr (context, 
												(float)magnification, 
												(float)magnification);

						// Determine the location where the image will be drawn in userspace.
				/*		
				rectangle = gCGRectMakePtr (
												legendWidth,
												0,
												lDestinationRect.right - lDestinationRect.left,
												lDestinationRect.top - lDestinationRect.bottom);
				*/											
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
					
					}	// end "if (drawBaseImageFlag)"
					
				if (numberImageOverlays > 0)
					{
					DrawImageOverlays (theWindow, 
												windowInfoHandle,
												context,
												&rectangle,
												&destinationRect,
												&windowRect,
												windowCode);
												
					}	// end "if (numberImageOverlays > 0)"
												
				//gCGContextFlushPtr (context);
				//gCGContextReleasePtr (context);
				
				}	// if (gOSXCoreGraphicsFlag && imageBaseAddressH != NULL)
			/*
			else	// !gOSXCoreGraphicsFlag || imageBaseAddressH == NULL
				{
		//#endif	// TARGET_API_MAC_CARBON
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
					
				case kSourceCopy: */
							// Map to wherever sourceRect * magnification indicates.  
							// This allows one to draw parts of the window at a time.														
					/*		
					sourceRect = *inSourceRect;
							
					destinationRect.top = 
										(SInt16)(sourceRect.top * magnification + imageTopOffset);
					destinationRect.left = 
										(SInt16)(sourceRect.left * magnification + legendWidth);
					destinationRect.bottom = 
										(SInt16)(sourceRect.bottom * magnification + imageTopOffset); 
					destinationRect.right = 
										(SInt16)(sourceRect.right * magnification + legendWidth);
					*/
				/*	ExitMultiSpec (TRUE);
							
					break;
					
				case kScrollCopy:
							// Copy portion of offscreen image map that fits in the 		
							// invalid region of the window. 	This is from a scroll 	
							// operation.																
							
					GetRegionBounds (invalidRgn, &destinationRect);
							
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
							(destinationRect.bottom - imageTopOffset)/magnification + 
																						vOrigin + .9999);
					sourceRect.right = (SInt16)(
						(destinationRect.right - legendWidth)/magnification + 
																						hOrigin + .9999);
						
					displaySpecsPtr = (DisplaySpecsPtr)*displaySpecsHandle;
					
					if (sourceRect.bottom > displaySpecsPtr->imageDimensions[kVertical])
						sourceRect.bottom = 
										(SInt16)displaySpecsPtr->imageDimensions[kVertical];
					
					if (sourceRect.right > displaySpecsPtr->imageDimensions[kHorizontal])
						sourceRect.right = 
										(SInt16)displaySpecsPtr->imageDimensions[kHorizontal];
						
							// Make certain that source to destination is correct.  The	
							// source may have been truncated to a larger value.			
						
					destinationRect.top = (SInt16)(imageTopOffset + magnification *
																(sourceRect.top - vOrigin));
					destinationRect.bottom = (SInt16)(imageTopOffset + magnification *
																(sourceRect.bottom - vOrigin));
					destinationRect.left = (SInt16)(legendWidth + magnification *
																	(sourceRect.left - hOrigin));
					destinationRect.right = (SInt16)(legendWidth + magnification *
																	(sourceRect.right - hOrigin));
					break;
				
				}	// end "switch (copyType)"	
		
			gWorldFlags = GetPixelsState ((PixMapHandle)offScreenMapH);
			if (drawBaseImageFlag && LockPixels ((PixMapHandle)offScreenMapH))
				{
				CopyBits ((BitMap*)*offScreenMapH, 
									GetPortBitMapForCopyBits (GetWindowPort (theWindow)),
									&sourceRect, 
									&destinationRect, 
									srcCopy, 
									NIL);
				
				if (!(gWorldFlags & pixelsLocked))							
					UnlockPixels ((PixMapHandle)offScreenMapH); */
				/*	
				//#if TARGET_API_MAC_CARBON
					if (QDIsPortBuffered (GetWindowPort (theWindow)))
						{
						//UInt32	finalTicks;
						QDFlushPortBuffer (GetWindowPort (theWindow), NULL);
						//Delay (10, &finalTicks);
						
						}	// end "if (QDIsPortBuffered (GetWindowPort (theWindow)))"
				//#endif	// TARGET_API_MAC_CARBON
				*/				
			/*	}	// end "if drawBaseImageFlag && (LockPixels (..."
				
					// Do image overlay.
			
			if (numberImageOverlays > 0)
				DrawImageOverlays (theWindow, 
											windowInfoHandle,
											NULL,
											NULL,
											&destinationRect,
											&sourceRect,
											windowCode);
											
			}	// end "else !gOSXCoreGraphicsFlag" 	
		*/
		MHSetState (windowInfoHandle, windowHandleStatus);
			
				// Draw the vector overlays if they exist.
			
		if (drawVectorOverlaysFlag && numberVectorOverlays > 0)
			DrawArcViewShapes (theWindow, 
										windowInfoHandle, 
										&destinationRect, 
										windowCode,
										NULL);
								
		//#if TARGET_API_MAC_CARBON
			else	// !drawVectorOverlaysFlag || numberVectorOverlays <= 0
				{
				if (QDIsPortBuffered (GetWindowPort (theWindow)))
					QDFlushPortBuffer (GetWindowPort (theWindow), NULL);
				}	// end "else !drawVectorOverlaysFlag || numberVectorOverlays <= 0" 		
		//#endif	// TARGET_API_MAC_CARBON

			 										
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
			
			}	// end "if (!fromImageWindowUpdateFlag && titleHeight > 0)" 
			
		//#if TARGET_API_MAC_CARBON
			if (context != NULL)
				{				
				gCGContextFlushPtr (context);
				gCGContextReleasePtr (context);
				
				}	// end "if (context != NULL)"
		//#endif	// TARGET_API_MAC_CARBON
			
		if (copyType != kClipboardCopy)
			ClipRect (GetWindowPortBounds (theWindow, &gTempRect));
						
		}	// end "if (imageBaseAddressH || windowInfoPtr->offscreenGWorld)"
		
	else	// imageBaseAddressH == NULL && ...
		{
		GetWindowClipRectangle (theWindow, kImageScrollArea, &windowRect);
  		EraseRect (&windowRect);
		UnhiliteImageWControls (theWindow);
  		
  		}	// end "else imageBaseAddressH == NULL && ..."
		
}	// end "CopyOffScreenImage" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListSpecifiedString
//
//	Software purpose:	This routine lists the specified string in the resource file to 
//							the output text window and/or disk file.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 02/17/1993
//	Revised By:			Larry L. Biehl			Date: 02/17/1993			

Boolean ListSpecifiedString (
				SInt16								stringID, 
				CMFileStream*						resultsFileStreamPtr, 
				SInt16								outputCode)
													
{
	char* 								stringPtr;
	
	SInt32								stringLength;
	StringHandle						stringHandle;
	
	Boolean								continueFlag;


	stringLength = GetSpecifiedString (stringID, &stringHandle, &stringPtr);
			
	continueFlag = (stringLength > 0);
	if (stringHandle)
		{
		continueFlag = OutputString (resultsFileStreamPtr, 
												stringPtr, 
												stringLength, 
												outputCode, 
												TRUE);
	
		HUnlock ((Handle)stringHandle);
		ReleaseResource ((Handle)stringHandle);
		
		}	// end "if (stringHandle)" 
		
	return (continueFlag);

}	// end "ListSpecifiedString"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void MGetPixRowBytes
//
//	Software purpose:	The purpose of this routine is to get the number of pix row
//							bytes in the input pix map handle. This routine has to take
//							into account whether this is a carbon application or not.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/02/2001
//	Revised By:			Larry L. Biehl			Date: 11/02/2001

SInt32 MGetPixRowBytes (
				PixMapHandle 						pixMapHandle)

{
	SInt32								longPixRowBytes = 0;
	
	if (pixMapHandle != NULL)
		{
		#if TARGET_API_MAC_CARBON
			longPixRowBytes = GetPixRowBytes (pixMapHandle);
		#else	// !TARGET_API_MAC_CARBON
			UInt16			rowBytes = 0;
			
			if (pixMapHandle != NULL)
				{
				rowBytes = (**pixMapHandle).rowBytes;
				BitClr (&rowBytes, 0);
				BitClr (&rowBytes, 1);
				
				}	// end "if (pm != NULL)"
				
			longPixRowBytes = rowBytes;
		#endif	// TARGET_API_MAC_CARBON, else...	
		
		}	// end "if (pixMapHandle != NULL)"
		
	return (longPixRowBytes);

}	// end "MGetPixRowBytes"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateDeskScrap
//
//	Software purpose:	The purpose of this routine is to make certain that
// 						the desk scrap is up to date.  The only item that
//							needs to be checked is the text edit scrap.  If it
//							exists then copy it to the desk scrap.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/10/1993
//	Revised By:			Larry L. Biehl			Date: 02/10/1993	

void UpdateDeskScrap (
				Boolean								clearTextEditScrapFlag)

{
	SInt32								scrapSize;
	
	SInt16								osErr,
											scrapCount;
	

			// Make certain that the desk scrap is up to date.  The				
			// text edit scrap may be the latest.										

	scrapSize = TEGetScrapLength ();
	if (scrapSize > 0)
		{         	
		#if !TARGET_API_MAC_CARBON
			PScrapStuff				pScrappStuffPtr;
			pScrappStuffPtr = InfoScrap ();
			ZeroScrap ();
			scrapCount = pScrappStuffPtr->scrapCount;
		#else
			ScrapRef 	scrapRef;
			ClearCurrentScrap ();
			osErr = GetCurrentScrap (&scrapRef);
					// Not sure how to get scrap count. May never be needed for Carbon 
					// since desk scrap may never come up.
			//??scrapCount = scrapRef.scrapCount;
			scrapCount = 0;
		#endif	// !TARGET_API_MAC_CARBON
		
		osErr = TEToScrap ();
				   
		if (osErr == noErr)
			//gLastScrapCount = pScrappStuffPtr->scrapCount;
			gLastScrapCount = scrapCount;
			
		if (clearTextEditScrapFlag)
			{
			TESetScrapLength (0);
			gLastScrapCount = 0;
			
			}	// end "if (clearTextEditScrapFlag)" 
						
		}	// end "if (scrapSize > 0)" 
				
}	// end "UpdateDeskScrap" 



#if !TARGET_API_MAC_CARBON	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateTextEditScrap
//
//	Software purpose:	The purpose of this routine is to make certain that
// 						the text edit scrap is up to date.  Only scrap in memory, 
//							non-PICT type scrap, and limited size scrap is copied.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			LoadRequestedDialog in MDialogUtilities.c
//
//	Coded By:			Larry L. Biehl			Date: 12/19/1997
//	Revised By:			Larry L. Biehl			Date: 12/23/1997	

void UpdateTextEditScrap (void)

{
	ScrapStuffPtr						scrappStuffPtr;
	SInt32*								scrapDataPtr;
	
	SInt16								osErr;
	

			// Make certain that the text edit scrap is up to date.  The desk scrap may 
			// be the latest.										

	scrappStuffPtr = InfoScrap ();
	
			// Only update if the desk scrap is new and the scrap is in memory.
					
	if (scrappStuffPtr->scrapSize > 0 && gLastScrapCount != scrappStuffPtr->scrapCount)
		{
		if (scrappStuffPtr->scrapHandle != NULL)
			{
			scrapDataPtr = (SInt32*)*scrappStuffPtr->scrapHandle;
			if (*scrapDataPtr != 'PICT')
				{
				osErr = TEFromScrap ();
				
				if (osErr == noErr)
					gLastScrapCount = scrappStuffPtr->scrapCount;
				
				}	// end "if (*scrapDataPtr != 'PICT')"
				
			}	// end "if (scrappStuffPtr->scrapHandle != NULL)"
			
		}	// end "if (scrappStuffPtr->scrapSize > 0 && ..." 
		
	if (gLastScrapCount != scrappStuffPtr->scrapCount)
		TESetScrapLength (0);
				
}	// end "UpdateTextEditScrap" 
#endif	// !TARGET_API_MAC_CARBON



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void WriteInterfacePreferenceValue
//
//	Software purpose:	This routine lists the specified string in the
//							resource file to the output text window and/or
//							disk file.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			InitializeMultiSpec in MInitialize.c
//							Menus in MMenus.c
//
//	Coded By:			Larry L. Biehl			Date: 01/09/2015
//	Revised By:			Larry L. Biehl			Date: 01/09/2015			

void WriteInterfacePreferenceValue (
				Boolean								fullVersionFlag)
													
{
	SInt16								interfaceValue;
	
	
	interfaceValue = 2;
	if (gFullVersionFlag)
		interfaceValue = 1;
		
			// Set up the preference. 

	CFStringRef userInterfaceKey = CFSTR ("DefaultUserInterface");
	CFNumberRef userInterfaceValue =	CFNumberCreate (
								kCFAllocatorDefault, kCFNumberSInt16Type, &interfaceValue);
	CFPreferencesSetAppValue (
				userInterfaceKey, userInterfaceValue, kCFPreferencesCurrentApplication);
						
			// Write out the preference data. 
		
	CFPreferencesAppSynchronize (kCFPreferencesCurrentApplication);
					
			// Release CF values 
			
	CFRelease (userInterfaceKey);
	CFRelease (userInterfaceValue);

}	// end "WriteInterfacePreferenceValue" 
