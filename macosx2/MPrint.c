//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						MPrint.c
//
//	Authors:					Larry L. Biehl
//
//	Revision number:		3.0
//
//	Revision date:			03/19/2018
//
//	Language:				C
//
//	System:					Macintosh Operating System
//
//	Brief description:	The routines in this file handle printing of the text in the 
//								output window.
//
//								NOTE.  Some of the ideas for the routines in this
//								file came from THINK's CAPPS editor construction kit.
//
//	Functions in file:	Boolean 				AbortPrint
//								static void 		DoPageHeader
//								void					DoPageSetup
//								void 					DoPrint
//								void 					DrawPrintImage
//								void 					ForcePrintSegmentLoad
//								void 					GetPrinterPageSize
//								void 					PrintImageWindow
//								void 					PrintTextWindow
//								Boolean 				SetUpPrintedImageInfo
//								short int 			ValidatePrintRecord
//
//------------------------------------------------------------------------------------

#include	"MGraphView.h"
										
extern OSStatus 				DoImagePageSetup (
										Handle								windowInfoHandle);

// extern void 			PMRectToRect (const PMRect * inPMRect, Rect * outRect);


			// Prototypes for routines in this file that are only called by		
			// other routines in this file.													
			
Boolean 	AbortPrint (void);

void 		CopyPrintOffscreenImage (
				WindowPtr 							windowPtr,
				void*									printPort, 
				WindowInfoPtr						imageWindowInfoPtr,
				Rect*									sourceRectPtr,
				Rect*									destinationRectPtr,
				double								magnification,
				SInt16								titleHeight,
				SInt16								legendWidth,
				Boolean								legendShowsFlag, 
				ListHandle							printLegendListHandle, 
				SInt16								legendLeftStart, 
				SInt16								legendTopStart);			

void 		DoImagePrint (
				Boolean 								DoJobDlog,
				WindowPtr 							windowPtr,
				WindowInfoPtr						windowInfoPtr,
				DisplaySpecsPtr					displaySpecsPtr);

//static void DoPageHeader (void);
														
pascal void DrawPrintImage (
				DialogPtr							dialogPtr,
				SInt16								itemNumber);		

void 		GetImagePrintPagesInfo (
				WindowInfoPtr						imageWindowInfoPtr,
				double								magnification,
				Rect*									sourceRectPtr,
				SInt32*								destinationWidthPtr,
				SInt32*								destinationHeightPtr,
				SInt16*								pageWidthPtr,
				SInt16*								pageHeightPtr,
				SInt32*								middlePageWidthPtr,
				SInt32*								middlePageHeightPtr,
				SInt32*								noPageWidthPtr,
				SInt32*								noPageHeightPtr);

void 		GetPrinterPageSize (
				SInt16*								pageHeightPtr,
				SInt16*								pageWidthPtr);		

#if TARGET_API_MAC_CARBON
	void 		PrintGraphWindow (
					WindowPtr 							graphWindowPtr,
					PMPrintSession						printSession,
					PMPageFormat						pageFormat);
#else	// !TARGET_API_MAC_CARBON
	void 		PrintGraphWindow (
					WindowPtr 							graphWindowPtr);
#endif	// TARGET_API_MAC_CARBON, else...			

#if TARGET_API_MAC_CARBON
	void 		PrintImageWindow (
					WindowPtr 							imageWindowPtr,
					WindowInfoPtr						imageWindowInfoPtr, 
					double								magnification,
					PMPrintSession						printSession,
					PMPageFormat						pageFormat);
#else	// !TARGET_API_MAC_CARBON
	void 		PrintImageWindow (
					WindowPtr 							imageWindowPtr,
					WindowInfoPtr						imageWindowInfoPtr, 
					double								magnification);
#endif	// TARGET_API_MAC_CARBON, else...

/*
void 		PrintTextWindow (
				WindowPtr 							wPtr,
				WindowInfoPtr						windowInfoPtr);
*/
Boolean 	SetUpPrintedImageInfo (
				WindowInfoPtr						imageWindowInfoPtr,
				double*								magnificationPtr,
				Rect*									printableAreaPtr);

SInt16	 ValidatePrintRecord (void);



//*************************** STATICS *********************************

static double				sMagnification;

#if !TARGET_API_MAC_CARBON
	static TPPrPort 			pPrPort;
#endif 	// !TARGET_API_MAC_CARBON

static Rect					sPrintableArea;

//static Point 				pageTopLeft;

//static StringPtr 			pDate;
//static StringPtr 			pTime;

//static char 				pageStr[] = "\pPage ";

//static short int  		ascent;
//static short int  		docSize;
//static short int  		lineHeight;
//static short int  		pageNum;
//static short int  		pageRight;



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean AbortPrint
//
//	Software purpose:	The purpose of this routine is to check for and 
//							handle an abort event (cmd-.).
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
// Called By:		
//
//	Coded By:			THINK C					Date: ??/??/??
//	Revised By:			Larry L. Biehl			Date: 01/28/89			

static  Boolean aborted;

Boolean AbortPrint (void)


{
#if !TARGET_API_MAC_CARBON
	if (aborted)
																						return (TRUE);

	if (PrError ())
		{
		aborted = TRUE;
																						return (TRUE);
		
		}	// end "if (PrError ())" 

	if (EventAvail (keyDownMask, &gEventRecord))
		if ((gEventRecord.modifiers & cmdKey) &&
											((unsigned char) gEventRecord.message == '.'))
			{
			aborted = TRUE;
																						return (TRUE);
			
			}	// end "if ((event.modifiers & cmdKey) && ..." 
#endif 	// !TARGET_API_MAC_CARBON

	return (FALSE);
	
}	// end "AbortPrint" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CopyPrintOffscreenImage
//
//	Software purpose:	The purpose of this routine is to copy the offscreen image to
//							the windows used for printing - the printed page and the print
//							preview.
//
//	Parameters in:					
//
//	Parameters out:	
//
// Value Returned:	None
// 
// Called By:			DrawPrintImage in print.c
//							PrintImageWindow in print.c
//
//	Coded By:			Larry L. Biehl			Date: 04/22/1998	
//	Revised By:			Larry L. Biehl			Date: 06/28/2011			

void CopyPrintOffscreenImage (
				WindowPtr 							windowPtr, 
				void*									printPortPtr, 
				WindowInfoPtr						imageWindowInfoPtr,
				Rect*									sourceRectPtr,
				Rect*									destinationRectPtr,
				double								magnification,
				SInt16								titleHeight,
				SInt16								legendWidth,
				Boolean								legendShowsFlag, 
				ListHandle							printLegendListHandle, 
				SInt16								legendLeftStart, 
				SInt16								legendTopStart)

{								
	double								saveMagnification;
	
	GWorldFlags							gWorldFlags;
	
	Rect									clipRect;
	
	DisplaySpecsPtr					displaySpecsPtr;
	
	Handle								offScreenMapH;
	RgnHandle							tempRegion;
	
	SInt16								grafPortType,
											saveOrigin[2],
											sourceImageWidth;
											
	//UInt32								saveCoordinateHeight,
	//										saveImageTopOffset;
	

	tempRegion = NewRgn ();
	if (tempRegion == NULL)
																								return;
																								
			// Initialize a print window record. This record is needed so that the
			// window information handle can be stored in it for later use by
			// "OutlineFieldControl" which expects the window information handle
			// to be store in the refcon position.
		
	offScreenMapH = imageWindowInfoPtr->offScreenMapHandle;
	grafPortType = imageWindowInfoPtr->grafPortType;
	
	sourceImageWidth = sourceRectPtr->right - sourceRectPtr->left;
						
	displaySpecsPtr = (DisplaySpecsPtr)*imageWindowInfoPtr->displaySpecsHandle;
	
	GetClip (tempRegion);
	
	clipRect = *destinationRectPtr;
	ClipRect (&clipRect);
	
	if (!AbortPrint () && sourceImageWidth > 0)
		{			
		if (gOSXCoreGraphicsFlag)
			{
			#if TARGET_API_MAC_CARBON
				LongRect					lDestinationRect;
				Rect						printPortRect;
				CGContextRef			context;
				CGRect					rectangle;
				
				float						hOffset,
											vOffset;
				
				SInt32					hOrigin,
											vOrigin;
				
				UInt32					vMagnificationSize,
											hMagnificationSize;
											

				vOrigin = displaySpecsPtr->origin[kVertical];
				hOrigin = displaySpecsPtr->origin[kHorizontal];
				
				hMagnificationSize = (UInt32)(
						magnification * displaySpecsPtr->imageDimensions[kHorizontal] + .5);
						
				vMagnificationSize = (UInt32)(
						magnification * displaySpecsPtr->imageDimensions[kVertical] + .5);

				lDestinationRect.bottom = 0;
				lDestinationRect.top = displaySpecsPtr->imageDimensions[kVertical];
				lDestinationRect.left = (SInt32)legendWidth;
				lDestinationRect.right = (SInt32)(legendWidth +
												displaySpecsPtr->imageDimensions[kHorizontal]);
			
						// If 'windowPtr' is not NULL, then this is a call to draw into a
						// dialog box window. the printPortPtr variable is a 'CGrafPtr'.
						// If 'windowPtr' is NULL, then 'printPortPtr' is a CGContextRef for
						// the printer window.
				/*
				if (windowPtr != NULL)
					{
					CreateCGContextForPort ((CGrafPtr)printPortPtr, &context);
					GetPortBounds ((CGrafPtr)printPortPtr, &printPortRect);
					
					}	// end "if (windowPtr != NULL)"
				
				else	// windowPtr != NULL
					{
					context = (CGContextRef)printPortPtr;
					printPortRect = sPrintableArea;
					
					}	// end "else windowPtr != NULL"
				*/
				GetPortBounds ((CGrafPtr)printPortPtr, &printPortRect);
					
				gQDBeginCGContextPtr ((CGrafPtr)printPortPtr, &context);
													
				gCGContextSetShouldAntialiasPtr (context, 0);

						// Get image and window sizes.
				
				//GetWindowClipRectangle (windowPtr, kFrameArea, &windowRect);
				//GetWindowClipRectangle (windowPtr, kImageArea, &imageRect);
				
						// Set clipping for core graphics context.
															
				rectangle.origin.x = destinationRectPtr->left;
				rectangle.origin.y = printPortRect.bottom - destinationRectPtr->bottom;
				rectangle.size.width =
										destinationRectPtr->right - destinationRectPtr->left;
				rectangle.size.height =
										destinationRectPtr->bottom - destinationRectPtr->top;
				
				gCGContextClipToRectPtr (context, rectangle);
				
						// Translate to QuickDraw coordinate system
				
				vOffset = (float)(printPortRect.bottom - destinationRectPtr->bottom);
				//if (vMagnificationSize + titleHeight >= printPortRect.bottom)
				vOffset -= (float)(vMagnificationSize - (destinationRectPtr->bottom - 
							destinationRectPtr->top + sourceRectPtr->top * magnification));
												
				hOffset = (float)(destinationRectPtr->left -
																sourceRectPtr->left * magnification);
			
				gCGContextTranslateCTMPtr (context,
													hOffset, 
													vOffset);
			
				gCGContextScaleCTMPtr (context, 
												(float)magnification, 
												(float)magnification);
															
				rectangle.origin.x = 0;
				rectangle.origin.y = 0;
				rectangle.size.width = lDestinationRect.right - lDestinationRect.left;
				rectangle.size.height = lDestinationRect.top - lDestinationRect.bottom;
												
						// Draw the image to the core graphics context.
						
				gCGContextDrawImagePtr (context, 
												rectangle,
												imageWindowInfoPtr->cgInfo.imageRef);
														
				gCGContextFlushPtr (context);
				
				gQDEndCGContextPtr ((CGrafPtr)printPortPtr, &context);
				
						// Store the port rectangle in gViewRect in case it is needed for
						// outlining project fields.
						
				gViewRect = printPortRect;
				/*
				if (windowPtr != NULL)
					gCGContextReleasePtr (context);
				*/
			#endif	// TARGET_API_MAC_CARBON
			}	// end "if (gOSXCoreGraphicsFlag)"
			
		else	// !gOSXCoreGraphicsFlag
			{	
			gWorldFlags = GetPixelsState ((PixMapHandle)offScreenMapH);
			if (LockPixels ((PixMapHandle)offScreenMapH))
				{
				CopyBits ((BitMap*)*offScreenMapH, 
								//GetPortBitMapForCopyBits (GetWindowPort (windowPtr)),
								GetPortBitMapForCopyBits ((CGrafPtr)printPortPtr),
								sourceRectPtr, 
								destinationRectPtr, 
								srcCopy, 
								NULL);
											
				if (!(gWorldFlags & pixelsLocked))	
					UnlockPixels ((PixMapHandle)offScreenMapH);
				
				}	// end "if (LockPixels (offScreenMapH))"
				
			if (windowPtr == NULL)
				windowPtr = (WindowPtr)printPortPtr;
				
			}	// end "else !gOSXCoreGraphicsFlag"
		
		}	// end "if (!AbortPrint () && sourceImageWidth > 0)" 
		
			// Prepare to draw vector information on top of the image.
	 										
	 		// If this is a project image, then also copy the field			
	 		// boundaries to the printer port.							
	 		
	if ((imageWindowInfoPtr->numberOverlays || 
							imageWindowInfoPtr->projectWindowFlag) && 
									sourceImageWidth > 0 && 
											!AbortPrint ())
		{
		SetOrigin (-destinationRectPtr->left+legendWidth, -destinationRectPtr->top);
		
		clipRect = *destinationRectPtr;	
		OffsetRect (&clipRect, 
							-destinationRectPtr->left+legendWidth, 
							-destinationRectPtr->top);
		ClipRect (&clipRect);
				
				// Set up window information handle to the active image		
				// window to the proper configuration and use the handle		
				// as the window information handle for the print window.	
		
				// Adjust the display origin to account for a portion of		
				// the image being printed on this page.	
																
		saveOrigin[kVertical] = displaySpecsPtr->origin[kVertical];
		saveOrigin[kHorizontal] = displaySpecsPtr->origin[kHorizontal];
		saveMagnification = displaySpecsPtr->magnification;
		//saveImageTopOffset = imageWindowInfoPtr->imageTopOffset;
				
		displaySpecsPtr->origin[kVertical] = sourceRectPtr->top;
		displaySpecsPtr->origin[kHorizontal] = sourceRectPtr->left;
		//imageWindowInfoPtr->imageTopOffset = imageWindowInfoPtr->titleHeight;
	
				// Set magnification to be used for outlining the fields.				
				
		displaySpecsPtr->magnification = magnification;
		
		if (imageWindowInfoPtr->numberOverlays > 0)
			DrawArcViewShapes (windowPtr,
										imageWindowInfoPtr->windowInfoHandle,
										destinationRectPtr, 
										kToPrintWindow,
										NULL);		
		
		if (imageWindowInfoPtr->projectWindowFlag)
			OutlineFieldsControl (gProjectInfoPtr->statsWindowMode, 
											windowPtr, 
											imageWindowInfoPtr->windowInfoHandle,
											kToPrintWindow);
																
		displaySpecsPtr->origin[kVertical] = saveOrigin[kVertical];
		displaySpecsPtr->origin[kHorizontal] = saveOrigin[kHorizontal];
		displaySpecsPtr->magnification = saveMagnification;
		//imageWindowInfoPtr->imageTopOffset = saveImageTopOffset;
		
		SetOrigin (0, 0);
		
		}	// end "if (imageWindowInfoPtr->projectWindowFlag && ...)" 
	
			// Handle title line for side-by-side images.						
			
	if (titleHeight > 0)
		{
		if (!AbortPrint ())
			{
			SetOrigin (-destinationRectPtr->left, -destinationRectPtr->top+titleHeight);
			
			clipRect = *destinationRectPtr;	
			OffsetRect (&clipRect, 
							-destinationRectPtr->left,
							-destinationRectPtr->top+titleHeight);
			clipRect.top = 0;
			ClipRect (&clipRect);
				
					// Set up window information handle to the active image		
					// window to the proper configuration and use the handle		
					// as the window information handle for the print window.	
			
					// Adjust the display origin to account for a portion of		
					// the image being printed on this page.	
																	
			saveOrigin[kVertical] = displaySpecsPtr->origin[kVertical];
			saveOrigin[kHorizontal] = displaySpecsPtr->origin[kHorizontal];
			saveMagnification = displaySpecsPtr->magnification;
			//saveImageTopOffset = imageWindowInfoPtr->imageTopOffset;
			//saveCoordinateHeight = imageWindowInfoPtr->coordinateHeight;
				
			displaySpecsPtr->origin[kVertical] = sourceRectPtr->top;
			displaySpecsPtr->origin[kHorizontal] = sourceRectPtr->left;
			displaySpecsPtr->magnification = magnification;
			//imageWindowInfoPtr->imageTopOffset = imageWindowInfoPtr->titleHeight;
			//imageWindowInfoPtr->coordinateHeight = 0;
			
			DrawSideBySideTitles (imageWindowInfoPtr->windowInfoHandle, 
											windowPtr, 
											destinationRectPtr,
											kToPrintWindow);
																
			displaySpecsPtr->origin[kVertical] = saveOrigin[kVertical];
			displaySpecsPtr->origin[kHorizontal] = saveOrigin[kHorizontal];
			displaySpecsPtr->magnification = saveMagnification;
			//imageWindowInfoPtr->imageTopOffset = saveImageTopOffset;
			//imageWindowInfoPtr->coordinateHeight = saveCoordinateHeight;
			
			SetOrigin (0, 0);
			
			}	// end "if (!AbortPrint ())" 
			
		}	// end "if (titleHeight > 0)"
			
	if (legendShowsFlag && !AbortPrint ())
		{		
		clipRect = *destinationRectPtr;
		clipRect.left -= legendWidth;
		ClipRect (&clipRect);
		
				// Draw the legend title, 'Legend'.									
				// Note the legend will be drawn on each 'left' page.			

		TextFont (gWindowTextFont); 		// monaco  
		TextSize (12);							// 12 point 
		
		DrawThematicLegend (windowPtr, 
									(CGrafPtr)printPortPtr,
									imageWindowInfoPtr, 
									printLegendListHandle, 
									legendLeftStart, 
									legendTopStart,
									FALSE);
		
		}	// end "if (imageWindowInfoPtr->showLegend && ... " 																
	
	SetClip (tempRegion);
	DisposeRgn (tempRegion);
	
}	// end "CopyPrintOffscreenImage" 


/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Static DoPageHeader
//
//	Software purpose:	The purpose of this routine is to print the page 
//							header.
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
// Called By:	
//
//	Coded By:			THINK C					Date: ??/??/??
//	Revised By:			Larry L. Biehl			Date: 08/26/2010			

static void DoPageHeader (void)

{
	Str255 								pageNumStr;
	

			// Draw the document title. 														
			
	MoveTo (pageTopLeft.h, pageTopLeft.v + ascent);
	DrawString (gTextString);

			// Draw the page number. 															
			
	NumToString ((SInt32)pageNum, pageNumStr);

	MoveTo (pageRight - (StringWidth ((ConstStr255Param)pageStr) + 
																		StringWidth (pageNumStr) + 5),
				pageTopLeft.v + ascent);
	
	DrawString ((ConstStr255Param)pageStr);
	DrawString (pageNumStr);

			// Draw the time and date. 														
			
	MoveTo (pageTopLeft.h, pageTopLeft.v + lineHeight + ascent);
	DrawString (pDate);
	DrawString ("\p  ");
	DrawString (pTime);
	
}	// end "DoPageHeader" 
*/


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoPageSetup
//
//	Software purpose:	The purpose of this routine is to call the page
// 						set up dialog.
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
// Called By:	
//
//	Coded By:			THINK C					Date: ??/??/??
//	Revised By:			Larry L. Biehl			Date: 08/17/2011			

void DoPageSetup (
				WindowPtr 							windowPtr)


{
	Handle								checkHandle,
											windowInfoHandle;
											
	SInt16								windowType;
	
	
			// If spare memory had to be used to load code resources, then exit	
			// routine.																				
			
	if (gMemoryTypeNeeded < 0)
																									return;
																							
			// Code resources loaded okay, so set flag back for non-Code			
			// resources.																			
			
	gMemoryTypeNeeded = 0;
	
			// Make certain that a minimum amount of memory is available for		
			// the dialog operation.															
			
	checkHandle = MNewHandle (6000 + gMinMemoryForDrawingDialog);
	if (checkHandle == NULL)
																									return;
																							
	DisposeHandle (checkHandle);
	
	windowInfoHandle = (Handle)GetWRefCon (windowPtr);
	windowType = GetWindowType (windowInfoHandle);									
	
	if (windowType == kOutputWindowType)
		#if !use_mlte_for_text_window	
			DoWePageSetup (gOutputTextH);
		#endif
		#if use_mlte_for_text_window
			TXNPageSetup (gOutputTextH);
		#endif	
	else if (windowType != 0)
		{			
		#if TARGET_API_MAC_CARBON
			DoImagePageSetup (windowInfoHandle);
		#else	// !TARGET_API_MAC_CARBON				
			Boolean								temp;
			
					// Open the current print driver and present the page set up dialog.	
					// This is for non text output windows.		
			
			if (gHPrint != NULL)
				{
				if (ValidatePrintRecord () == noErr)
					{
					PrOpen ();
			
					if (PrError () == noErr)
						temp = PrStlDialog (gHPrint);
						
					PrClose ();
					
					}	// end "if (ValidatePrintRecord () == noErr)"
				
				}	// end "if (gHPrint != NULL)" 
		#endif 	// TARGET_API_MAC_CARBON, else...
			
		}	// end "else if (windowType != 0)"
	
}	// end "DoPageSetup" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoPrint
//
//	Software purpose:	The purpose of this routine is to handle "print" command.
//
//							NOTE.  Most of the code for this routine came from THINK's CAPPS
//							editor construction kit.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			Menus in menu.c
//
//	Coded By:			THINK C					Date: ??/??/??
//	Revised By:			Larry L. Biehl			Date: 12/24/2008			

void DoPrint (
				Boolean 								DoJobDlog, 
				WindowPtr 							windowPtr)

{
	DisplaySpecsPtr					displaySpecsPtr;
	GrafPtr								savedPort;
	WindowInfoPtr						windowInfoPtr;
	
	Handle								checkHandle,
											tempHandle = NULL,
											windowInfoHandle;
	
	SInt16								windowType;
	

			// If spare memory had to be used to load code resources, then exit	
			// routine.																				
			
	if (gMemoryTypeNeeded < 0)
																									return;
																							
			// Code resources loaded okay, so set flag back for non-Code			
			// resources.																			
			
	gMemoryTypeNeeded = 0;
	
			// Make certain that a minimum amount of memory is available for		
			// the printing operation.															
			
	checkHandle = MNewHandle (6000 + gMinMemoryForDrawingDialog);
	if (checkHandle == NULL)
																									return;
	DisposeHandle (checkHandle);
	
	aborted = FALSE;
	displaySpecsPtr = NULL;
						
				// Get the pointer to the window information structure.				
				
	windowInfoHandle = (Handle)GetWRefCon (windowPtr);
	if (windowInfoHandle != NULL)
		{
		HLock (windowInfoHandle);
		windowInfoPtr = (WindowInfoPtr)*windowInfoHandle;
	
				// Make certain that window type makes sense.							
			
		windowType = windowInfoPtr->windowType;
		if (windowType != kImageWindowType && 
				windowType != kOutputWindowType && 
					windowType != kThematicWindowType &&
						windowType != kGraphicsWindowType)
			aborted = TRUE;
			
		}	// end "if (windowInfoHandle != NULL)" 
			
	else
																									return;
	
			// Set the processor code.	
	
	gProcessorCode = kPrintProcessor;
	
			// Save the current port.															
			
	GetPort (&savedPort);
	
			// If this is the output text window, then save the current clip region so
			// that the clip region can be set to the entire port rectangle so that the
			// controls can be adjusted when printing.
	
	if (windowType == kOutputWindowType)
		{
		GetClip (gTempRegion1);
			
				// Set clip region of the window to include the scroll bars so that the
				// thumb on the scroll can be moved.
					
		ClipRect (GetWindowPortBounds (windowPtr, &gTempRect));
		
		#if use_mlte_for_text_window
			OSStatus					status;
			
			status = TXNPrint (gTextObject);
			
			if (status != noErr)
				aborted = TRUE;
		#endif
		
		#if !use_mlte_for_text_window	
			DoWePrint (windowPtr);
		#endif
		
				// Unlock handles that were locked in this routine.					
				
		CheckAndUnlockHandle (windowInfoHandle);
	
		SetPort (savedPort);
		
				// Reset the clip region back to that upon entry to this routine.		
				
		SetClip (gTempRegion1);
		SetEmptyRgn (gTempRegion1);
		
				// Reset the processor code.	
		
		gProcessorCode = 0;
																									return;
		
		}	// end "if (windowType == kOutputWindowType)" 
	
			// If this is the image window, get a handle to a palette.					
	
	if (windowType == kImageWindowType || windowType == kThematicWindowType)
		{
				// Lock handle to display specifications.									
		
		displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
																windowInfoPtr->displaySpecsHandle,
																kLock);
			
		if (displaySpecsPtr == NULL)
			aborted = TRUE;
			
		}	// end "if (windowType == kImageWindowType || ..." 
		
	#if TARGET_API_MAC_CARBON
		DoImagePrint (DoJobDlog, windowPtr, windowInfoPtr, displaySpecsPtr);
	#else	// !TARGET_API_MAC_CARBON
		DialogPtr 							dPtr;
		TPrStatus 							prStatus;
										
		Point									where;
		
		SInt16								copies,
												savedPrintError;
		
		Boolean								temp;
			
		savedPrintError = ValidatePrintRecord ();
		
		if (savedPrintError == noErr)
					// Open the print driver.														
			PrOpen ();
			
		else	// savedPrintError != noErr 
			aborted = TRUE;
			
		if (PrError () == noErr && !aborted)
			{
					// If this is an image type window, then determine the number of	
					// pages that it will take to print the image and show the 			
					// information to the user.  Give the user a chance to cancel the	
					// print request.																	
						
			if (windowType == kImageWindowType || windowType == kThematicWindowType)
				aborted = SetUpPrintedImageInfo (windowInfoPtr, &sMagnification, NULL);
				
					// Do print job dialog if necessary and not aborted.					
					
			if (!aborted && DoJobDlog)
				{
				checkHandle = MNewHandle (2000 + gMinMemoryForDrawingDialog);
				aborted = (checkHandle == NULL);
				if (!aborted)
					{
					DisposeHandle (checkHandle);
					aborted = !PrJobDialog (gHPrint);
					
					}	// end "if (checkHandle != NULL)" 
				
						// Do activate and update events since we will not go back		
						// to the event loop.														
				
				gProcessorCode = 0;
				aborted = (aborted | !CheckSomeEvents (updateMask + activMask));
				gProcessorCode = kPrintProcessor;
				
				}	// end "if (DoJobDlog)" 
		
					// Continue if not aborted.													
			
			if (!aborted)
				{
				copies = (**gHPrint).prJob.iCopies;
		
						// If it's being spooled, do only one.									
					
				if ((**gHPrint).prJob.bJDocLoop == bSpoolLoop)
					copies = 1;
		
						// Draw the "printing in progress" dialog.							
					
				GetDlogOrigin (kPrintDlogSpecificationID, &where);
				dPtr = LoadRequestedDialog (
											kPrintDlogSpecificationID, kDoNotCopyScrap, 1, 2);
				if (dPtr != NULL)
					{
					MoveWindow (GetDialogWindow (dPtr), where.h, where.v, FALSE);
					ShowWindow (GetDialogWindow (dPtr));
					DrawDialog (dPtr);
				
							// Validate the print record.											
						
					temp = PrValidate (gHPrint);
			
							// Open the document.													
							
					if (PrError () == noErr)
						pPrPort = PrOpenDoc (gHPrint, NIL, NIL);
				
					if (PrError () == noErr && pPrPort != NULL)
						{
								// Print each copy.													
								
						for (; copies--;)
							{
							if (AbortPrint ())
								break;
							
							if (windowType == kImageWindowType ||
																windowType == kThematicWindowType)
								PrintImageWindow (windowPtr, windowInfoPtr, sMagnification);
							
							else if (windowType == kGraphicsWindowType)
								PrintGraphWindow (windowPtr);
							
							}	// end "for (; copies--;)" 
				
								// And close the document.											
							
						PrCloseDoc (pPrPort);
				
								// If not aborted and was spooling, print the spool file.
							
						if (!AbortPrint ())
							if ((**gHPrint).prJob.bJDocLoop == bSpoolLoop)
								PrPicFile (gHPrint, NIL, NIL, NIL, &prStatus);
							
						}	// end "if (PrError () == noErr && pPrPort != NULL)" 
			
							// Get rid of the printing dialog.									
						
					DisposeDialog (dPtr);
				
					}	// end "if (dPtr == NULL)" 
					
				}	// end "!aborted" 
				
			}	// end "if (PrError () == noErr  && !aborted)" 
		
		if (savedPrintError == noErr)	
			savedPrintError = PrError ();
			
				// Close the print driver.																
				
		PrClose ();
					
				// Make certain that the legend list definition is locked.						
				
		tempHandle = (Handle)Get1Resource ('LDEF', 131);
		HLock (tempHandle);
	#endif 	// !TARGET_API_MAC_CARBON
	
	SetPort (savedPort);
		
	CheckAndUnlockHandle (windowInfoPtr->displaySpecsHandle);
	
			// Unlock handles that were locked in this routine.					
			
	CheckAndUnlockHandle (windowInfoHandle);
	CheckAndUnlockHandle (tempHandle);
	
			// Reset the processor code.	
	
	gProcessorCode = 0;
	
}	// end "DoPrint" 



#if TARGET_API_MAC_CARBON
OSStatus DoImagePageSetup (
				Handle								windowInfoHandle)
			
{
	double								pageScale;
	
	PMOrientation						pageOrientation;
	PMPageFormat						pageFormat = NULL;
	PMPrintSession						printSession = NULL;
	Boolean								accepted = FALSE;
	OSStatus								status;


			//	Begin a print session
			
	if ((status = PMCreateSession (&printSession)) != noErr)
		goto cleanup;

				//	no pre-existing page format: create one
				
	if ((status = PMCreatePageFormat (&pageFormat)) != noErr)
		goto cleanup;
		
	if ((status = PMSessionDefaultPageFormat (printSession, pageFormat)) != noErr)
		goto cleanup;
		
			// Set the page orientation to the last user setting
			
	if (status == noErr)
		status = PMSetOrientation (
					pageFormat, GetPrintPageOrientation (windowInfoHandle), kPMUnlocked);
		
			// Set the page scale to the last user setting
			
	if (status == noErr)
		status = PMSetScale (pageFormat, GetPrintPageScale (windowInfoHandle));

			//	Display the page setup dialog
			
	if ((status = PMSessionPageSetupDialog (
												printSession, pageFormat, &accepted)) != noErr)
		goto cleanup;
		
			// Save the page orientation
	
	status = PMGetOrientation (pageFormat, &pageOrientation);
	if (status == noErr)
		SetPrintPageOrientation (windowInfoHandle, pageOrientation);
		
			// Save the scale to use to print the page
			
	status = PMGetScale (pageFormat, &pageScale);
	if (status == noErr)
		SetPrintPageScale (windowInfoHandle, pageScale);

			//	clear result code
			
	status = noErr;

cleanup:
			//	Close the print session
	
	if  (printSession != NULL)
		{
		PMRelease (printSession);
		printSession = NULL;
		
		}	// end "if  (printSession != NULL)"

			//	return result code
			
	return status;
	
}	// end "DoImagePageSetup"
#endif	// TARGET_API_MAC_CARBON



#if TARGET_API_MAC_CARBON	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoImagePrint
//
//	Software purpose:	The purpose of this routine is to handle "print" 
//							command for image windows.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			Menus in menu.c
//
//	Coded By:			THINK C					Date: 10/05/2001
//	Revised By:			Larry L. Biehl			Date: 08/18/2011			

void DoImagePrint (
				Boolean 								DoJobDlog, 
				WindowPtr 							windowPtr,
				WindowInfoPtr						windowInfoPtr,
				DisplaySpecsPtr					displaySpecsPtr)

{
	PMPrintSession						printSession = NULL;
	PMPageFormat						pageFormat = NULL;
	PMPrintSettings					printSettings = NULL;
	PMRect								pmRect;
	
	SInt16								windowType;

	DialogPtr 							dPtr;
									
	Point									where;
	
	Handle								checkHandle;
	
	OSStatus								status;
	
	SInt16								copies;
	
	Boolean								accepted = FALSE,
											pageValidationResultFlag;
	
		
	windowType = windowInfoPtr->windowType;
	
			//	start a new print session
			
	status = PMCreateSession (&printSession);
		
				//	no pre-existing page format: create one
				
	if (status == noErr)
		status = PMCreatePageFormat (&pageFormat);
		
	if (status == noErr)
		status = PMSessionDefaultPageFormat (printSession, pageFormat);
		
			// Set the page orientation to the last user setting
			
	if (status == noErr)
		status = PMSetOrientation (
							pageFormat, windowInfoPtr->printPageOrientation, kPMUnlocked);
		
			// Set the page scale to the last user setting
			
	if (status == noErr)
		status = PMSetScale (pageFormat, windowInfoPtr->printPageScale);
		
	if (status == noErr)
		status = PMSessionValidatePageFormat (
										printSession, pageFormat, &pageValidationResultFlag);

			//	create a print settings object
			
	if (status == noErr)
		status = PMCreatePrintSettings (&printSettings);
			
	if (status == noErr)
		status = PMSessionDefaultPrintSettings (printSession, printSettings);				
			
	if (status == noErr && 
				(DoJobDlog ||
						windowType == kImageWindowType ||
								windowType == kThematicWindowType))
		{
				// Make sure that there is enough memory to display the dialog box.
				
		checkHandle = MNewHandle (2000 + gMinMemoryForDrawingDialog);
		aborted = (checkHandle == NULL);
		if (!aborted)
			DisposeHandle (checkHandle);
			
		checkHandle = NULL;
		
		}	// end "if (status == noErr && (DoJobDlog || ..."
			
			// Do print job dialog if necessary and not aborted.					
			
	if (status == noErr && !aborted && DoJobDlog)
		{
				//	display the print dialog
				
		status = PMSessionPrintDialog (printSession, 
													printSettings, 
													pageFormat, 
													&accepted);
													
		if (!accepted)
			aborted = TRUE;
		
				// Do activate and update events since we will not go back		
				// to the event loop.														
		
		gProcessorCode = 0;
		aborted = (aborted | !CheckSomeEvents (updateMask + activMask));
		gProcessorCode = kPrintProcessor;
		
		}	// end "if (status == noErr && !aborted && DoJobDlog)" 

			//	get printable page rectangle (actual page rect can't be larger than this)
			
	if (status == noErr && !aborted)
		{
		status = PMGetAdjustedPageRect (pageFormat, &pmRect);
		
		if (status == noErr)	
			{
			//PMRectToRect (&pmRect, &printableArea);
			sPrintableArea.left = pmRect.left;
			sPrintableArea.top = pmRect.top;
			sPrintableArea.right = pmRect.right;
			sPrintableArea.bottom = pmRect.bottom;
			
			}	// end "if (status == noErr)"
			
		}	// end "if (status == noErr && !aborted)"

	if (status == noErr && !aborted)
		{
				// If this is an image type window, then determine the number of	
				// pages that it will take to print the image and show the 			
				// information to the user.  Give the user a chance to cancel the	
				// print request.																	
					
		if (windowType == kImageWindowType || windowType == kThematicWindowType)
			aborted = SetUpPrintedImageInfo (
											windowInfoPtr, &sMagnification, &sPrintableArea);
	
	
		}	// end "if (status == noErr && !aborted)"
		
			// Continue if not aborted.													
	
	if (status == noErr && !aborted)
		{
		copies = 1;

		if (status == noErr)
			status = PMSetFirstPage (printSettings, 1, FALSE);
			
		if (status == noErr)
			status = PMSetLastPage (printSettings, 1, FALSE);

				// Draw the "printing in progress" dialog.							
			
		GetDlogOrigin (kPrintDlogSpecificationID, &where);
		dPtr = LoadRequestedDialog (kPrintDlogSpecificationID, kDoNotCopyScrap, 1, 2);
		if (dPtr != NULL)
			{
			MoveWindow (GetDialogWindow (dPtr), where.h, where.v, FALSE);
			ShowWindow (GetDialogWindow (dPtr));
			DrawDialog (dPtr);
	
					// Open the document.	

			if (status == noErr)
				status = PMSessionBeginDocument (printSession, printSettings, pageFormat);
		
			if (status == noErr)
				{
						// Print each copy.													
						
				for (; copies--;)
					{
					if (AbortPrint ())
						break;
					
					if (windowType == kImageWindowType || windowType == kThematicWindowType)
						PrintImageWindow (windowPtr, 
												windowInfoPtr,
												sMagnification, 
												printSession, 
												pageFormat);
					
					else if (windowType == kGraphicsWindowType)
						PrintGraphWindow (windowPtr, printSession, pageFormat);
					
					}	// end "for (; copies--;)" 
		
						// And close the document.	
						
				status = PMSessionEndDocument (printSession);
					
				}	// end "if (status == noErr)" 
	
					// Get rid of the printing dialog.									
				
			DisposeDialog (dPtr);
		
			}	// end "if (dPtr == NULL)" 
		
		}	// end "if (status == noErr && !aborted)" 

	if (printSettings != NULL)
		PMRelease (printSettings);

	if (printSession != NULL)
		PMRelease (printSession);
	
}	// end "DoImagePrint" 
#endif 	// TARGET_API_MAC_CARBON 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DrawPrintImage
//
//	Software purpose:	The purpose of this routine is to draw an outline
//							of the image to be printed into the dialog box so
//							that the user can see how the image will be displayed
//							on the printed page(s).
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/11/91
//	Revised By:			Larry L. Biehl			Date: 04/22/98			

pascal void DrawPrintImage (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{
	double								scaledMagnification;
	
	Pattern								black;
	
	Rect									destinationRect,
											scaledRect,
											sourceRect,
											theRect;
	
	GrafPtr								savedPort;
	
	Handle								theHandle;
	
	SInt32								destinationHeight,
											destinationWidth,
											left,
											heightPages,
											middlePageHeight,
											middlePageWidth,
											noPagesHeight,
											noPagesWidth,
											scaledHeight,
											scaledImageHeight,
											scaledImageLegendWidth,
											scaledImageTitleHeight,
											scaledImageWidth,
											scaledPageHeight,
											scaledPageWidth,
											scaledWidth,
											top,
											totalNumberPages,
											widthPages;
											
	SInt16								pageHeight,
											pageWidth,
											savedCoordinateHeight,
											savedLabelFieldCode,
											savedLegendWidth,
											savedTitleHeight,
											theType;
											
	
			// Get the display rectangle for the drawing to place in.				
			
	GetDialogItem (dialogPtr, 3, &theType, &theHandle, &theRect);

	GetImagePrintPagesInfo (gImageWindowInfoPtr,
									sMagnification,
									&sourceRect,
									&destinationWidth,
									&destinationHeight,
									&pageWidth,
									&pageHeight,
									&middlePageWidth,
									&middlePageHeight,
									&noPagesWidth,
									&noPagesHeight);
										
	totalNumberPages = noPagesWidth * noPagesHeight;
	
			//	Load the total number of pages.												
			
	LoadDItemValue (dialogPtr, 4, (SInt32)totalNumberPages);
	
			// Get the height and width of the area to draw the scaled pages into.
	
	scaledHeight = theRect.bottom - theRect.top;
	scaledWidth = theRect.right - theRect.left;
	
			// Determine the scaled size of a rectangle that will represent one	
			// printed page in the layout drawing.											
			
	scaledPageHeight = scaledHeight/noPagesHeight;
	scaledPageWidth = scaledWidth/noPagesWidth;
	
	if ((SInt32)pageHeight*scaledPageWidth >
													(SInt32)pageWidth*scaledPageHeight)
		scaledPageWidth = ((SInt32)pageWidth*scaledPageHeight)/pageHeight;
	
	else	// pageHeight*scaledPageWidth <= pageWidth*scaledPageHeight 
		scaledPageHeight = ((SInt32)pageHeight*scaledPageWidth)/pageWidth;
		
			// Draw an outline of the image to be printed on these pages 			
			// with a pen width of 2.															
			
	scaledImageHeight = ((SInt32)destinationHeight * scaledPageHeight)/pageHeight;
	scaledImageWidth =  ((SInt32)destinationWidth * scaledPageWidth)/pageWidth;
					
	scaledImageLegendWidth = 
			((SInt32)gImageWindowInfoPtr->legendWidth * scaledPageWidth)/pageWidth;
					
	scaledImageTitleHeight = 
			((SInt32)gImageWindowInfoPtr->titleHeight * scaledPageHeight)/pageHeight;
			
	scaledMagnification = (sMagnification * scaledPageWidth)/pageWidth;
	
			// Get the left and top position to start drawing the scaled image outline.
			
	left = (scaledWidth - scaledImageWidth)/2 + theRect.left;
	top = (scaledHeight - scaledImageHeight)/2 + theRect.top;
	
			// Set the scaled rectangle that will be used to represent the	printed image.
			
	SetRect (&scaledRect, 0, 0, scaledImageWidth, scaledImageHeight);
	
			// Now offset the scaled rectangle so that it will be positioned at the
			// upper left starting location.
			
	OffsetRect (&scaledRect, left, top);
	
	destinationRect = scaledRect;
	destinationRect.top += scaledImageTitleHeight;
	destinationRect.left += scaledImageLegendWidth;
	
			// Set the dialog port.																
	
	GetPort (&savedPort);	
	SetPortDialogPort (dialogPtr);
	
			// Set the pen characteristics.													
			
	PenPat (GetQDGlobalsBlack (&black));
	PenMode (patCopy);	
	
	EraseRect (&theRect);
	
	if (gProjectInfoPtr != NULL)
		{
		savedLabelFieldCode = gProjectInfoPtr->labelFieldCode;
		gProjectInfoPtr->labelFieldCode = 0;
		
		}	// end "if (gProjectInfoPtr != NULL)"
		
	savedLegendWidth = gImageWindowInfoPtr->legendWidth;
	savedTitleHeight = gImageWindowInfoPtr->titleHeight;
	savedCoordinateHeight = gImageWindowInfoPtr->coordinateHeight;
	
	gImageWindowInfoPtr->legendWidth = 0;
	gImageWindowInfoPtr->titleHeight = 0;
	gImageWindowInfoPtr->coordinateHeight = 0;
	
	CopyPrintOffscreenImage (GetDialogWindow (dialogPtr),
										(GrafPtr)GetDialogPort (dialogPtr), 
										gImageWindowInfoPtr,
										&sourceRect,
										&destinationRect,
										scaledMagnification,
										savedTitleHeight,
										0,
										FALSE, 
										NULL, 
										destinationRect.left, 
										destinationRect.top);
		
	gImageWindowInfoPtr->legendWidth = savedLegendWidth;
	gImageWindowInfoPtr->titleHeight = savedTitleHeight;
	gImageWindowInfoPtr->coordinateHeight = savedCoordinateHeight;
	
	if (gProjectInfoPtr != NULL)
		gProjectInfoPtr->labelFieldCode = savedLabelFieldCode;
		
			// Now draw the outline of the image.											
			
	PenSize (2, 2);
	FrameRect (&scaledRect);

	PenSize (1, 1);
	
			// Get the left and top position to start drawing the scaled printer	
			// pages.																				
			
	left = (scaledWidth - noPagesWidth*scaledPageWidth)/2 + theRect.left;
	top = (scaledHeight - noPagesHeight*scaledPageHeight)/2 + theRect.top;
	
			// Set the scaled rectangle that will be used to represent a printed	
			// page.																					
			
	SetRect (&scaledRect, 0, 0, scaledPageWidth, scaledPageHeight);
	
			// Now offset the scaled rectangle so that it will be positioned at	
			// the upper left starting location.											
			
	OffsetRect (&scaledRect, left, top);
	
			// Loop through the number of pages for the height.						
			
	for (heightPages=1; heightPages<=noPagesHeight; heightPages++)
		{
				// Loop through the number of pages for the width.						
				
		for (widthPages=1; widthPages<=noPagesWidth; widthPages++)
			{
			FrameRect (&scaledRect);
			
					// Offset the rectangle for the next "column" of printed 		
					// pages.																		
				
			if (widthPages != noPagesWidth)
				OffsetRect (&scaledRect, scaledPageWidth-1, 0);
				
			}	// end "for (widthPages=1; widthPages<=noPagesWidth; ..." 
			
		scaledRect.left = left;
		scaledRect.right = left + scaledPageWidth;
			
				// Offset the rectangle for the next "column" of printed pages.	
			
		if (heightPages != noPagesHeight)
			OffsetRect (&scaledRect, 0, scaledPageHeight-1);
		
		}	// end "for (heightPages=1; heightPages<=..." 
	
			// Set pen back to normal and the port back to the port upon entry	
			// to this routine.																	
	
	PenNormal ();
	SetPort (savedPort);
	
}	// end "DrawPrintImage" 


											
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ForcePrintSegmentLoad
//
//	Software purpose:	The purpose of this is to force code the print segment
//							to be loaded into memory. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 09/10/93
//	Revised By:			Larry L. Biehl			Date: 09/10/93

void ForcePrintSegmentLoad (void)

{
			// If spare memory had to be used to load code resources, then exit	
			// routine.																				
			
	if (gMemoryTypeNeeded < 0)
																									return;
																							
			// Code resources loaded okay, so set flag back for non-Code			
			// resources.																			
			
	gMemoryTypeNeeded = 0;

}	// end "ForcePrintSegmentLoad" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetImagePrintPagesInfo
//
//	Software purpose:	The purpose of this routine is to calculate the printer page
//							page parameters.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 04/21/98
//	Revised By:			Larry L. Biehl			Date: 04/21/98			

void GetImagePrintPagesInfo (
				WindowInfoPtr						imageWindowInfoPtr,
				double								magnification,
				Rect*									sourceRectPtr,
				SInt32*								destinationWidthPtr,
				SInt32*								destinationHeightPtr,
				SInt16*								pageWidthPtr,
				SInt16*								pageHeightPtr,
				SInt32*								middlePageWidthPtr,
				SInt32*								middlePageHeightPtr,
				SInt32*								noPageWidthPtr,
				SInt32*								noPageHeightPtr)

{
	ldiv_t 								ratio;
	
	SInt16								legendWidth,
											titleHeight;
	
			
			// Get the source rectangle.  Use the entire image if no selection	
			// has been made.  If a selection has been made use that selection	
			// rectangle.							
														
	GetSelectedOffscreenRectangle (imageWindowInfoPtr,
												sourceRectPtr, 
												TRUE,
												TRUE);
	
			// Get the legend and title parameters.
											
	legendWidth = imageWindowInfoPtr->legendWidth;
	titleHeight = imageWindowInfoPtr->titleHeight;
		
			// Get the width and height of the image in pixels taking into 		
			// account the current magnification.											
			
	*destinationWidthPtr = legendWidth + 
							(sourceRectPtr->right - sourceRectPtr->left) * magnification;
	*destinationHeightPtr = titleHeight + 
							(sourceRectPtr->bottom - sourceRectPtr->top) * magnification;
		
			// Get the width and height of the printed page in pixels.	
			
	GetPrinterPageSize (pageHeightPtr, pageWidthPtr);
		
			// Get the number of pages for the width and height of the image to 	
			// be printed.																			
			
	*noPageWidthPtr = (*destinationWidthPtr-1)/(*pageWidthPtr) + 1;
	*noPageHeightPtr = (*destinationHeightPtr-1)/(*pageHeightPtr) + 1;
	
			// Now determine if the number of pages have to be increased by 1 in either
			// direction to actually reflect what is going to be drawn on the page. A 
			// source pixel cannot be split across two pages.
	
	ratio = ldiv (
				(SInt32)(sourceRectPtr->right - sourceRectPtr->left), *noPageWidthPtr);
	*middlePageWidthPtr = ratio.quot * magnification + .5;
	
	if (ratio.rem > 0 && (ratio.quot+1) * magnification > *pageWidthPtr)
		{
		*pageWidthPtr = *middlePageWidthPtr;
		(*noPageWidthPtr)++;
		
		}	// end "if (ratio.rem > 0 && ..."
	
	ratio = ldiv (
				(SInt32)(sourceRectPtr->bottom - sourceRectPtr->top), *noPageHeightPtr);
	*middlePageHeightPtr = ratio.quot * magnification + .5;
	
	if (ratio.rem > 0 && (ratio.quot+1) * magnification > *pageHeightPtr)
		{
		*pageHeightPtr = *middlePageHeightPtr;
		(*noPageHeightPtr)++;
		
		}	// end "if (ratio.rem > 0 && ..."
	
}	// end "GetImagePrintPagesInfo" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetPrinterPageSize
//
//	Software purpose:	The purpose of this routine is to get the printer
//							page size.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/11/91
//	Revised By:			Larry L. Biehl			Date: 10/07/2001			

void GetPrinterPageSize (
				SInt16*								pageHeightPtr, 
				SInt16*								pageWidthPtr)

{
	#if TARGET_API_MAC_CARBON
		*pageHeightPtr = sPrintableArea.bottom - sPrintableArea.top;
		*pageWidthPtr = sPrintableArea.right - sPrintableArea.left;
	#else	// !TARGET_API_MAC_CARBON
		TPPrint								pPrint;
		
		SInt16								scaledMaxColorScriptWidth,
												scaledMaxColorScriptHeight;
												
											
		pPrint = *gHPrint;
		*pageWidthPtr = pPrint->prInfo.rPage.right - pPrint->prInfo.rPage.left;
		*pageHeightPtr = pPrint->prInfo.rPage.bottom - pPrint->prInfo.rPage.top;
		
				// Now adjust the page widths to take into account the smaller page	
				// size for the QMS ColorScript 100 printer.  This will negate the	
				// ability to reduce the image size using 'Page SetUp'.  Hopefully	
				// in the future the laser driver can query the printer for the		
				// proper page size.  Then this code can be deleted.						
		
		if (*pageWidthPtr <= *pageHeightPtr)
			{
			scaledMaxColorScriptWidth = 
										((SInt32)*pageWidthPtr * gMaxPrintPageWidth)/553;
			scaledMaxColorScriptHeight = 
										((SInt32)*pageHeightPtr * gMaxPrintPageHeight)/731;
			*pageWidthPtr = MIN (*pageWidthPtr, scaledMaxColorScriptWidth);
			*pageHeightPtr = MIN (*pageHeightPtr, scaledMaxColorScriptHeight);
			
			}	// end "if (*pageWidthPtr <= *pageHeightPtr)" 
			
		else	// pageWidth > pageHeight 
			{
			scaledMaxColorScriptWidth = 
										((SInt32)*pageWidthPtr * gMaxPrintPageWidth)/731;
			scaledMaxColorScriptHeight = 
										((SInt32)*pageHeightPtr * gMaxPrintPageHeight)/553;
			*pageWidthPtr = MIN (*pageWidthPtr, scaledMaxColorScriptHeight);
			*pageHeightPtr = MIN (*pageHeightPtr, scaledMaxColorScriptWidth);
			
			}	// end "else *pageWidthPtr > *pageHeightPtr"
	#endif	// TARGET_API_MAC_CARBON, else...
		
}	// end "GetPrinterPageSize"



#if TARGET_API_MAC_CARBON
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void PrintGraphWindow
//
//	Software purpose:	The purpose of this routine is to print the 
//							contents of an image window.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 02/10/1999
//	Revised By:			Larry L. Biehl			Date: 12/24/2002			

void PrintGraphWindow (
				WindowPtr 							graphWindowPtr,
				PMPrintSession						printSession,
				PMPageFormat						pageFormat)

{
	Rect									destinationRect,
											sourceRect;
											
	double								scaledPage,
											scaledPageHeight,
											scaledPageWidth;
	
	GrafPtr								savedPort;
	GraphPtr								graphRecordPtr;
											
	void*									printPort = NULL;
											
	Handle								graphRecordHandle;
	
	SInt32								sourceWidth,
											sourceHeight;
	
	SInt16								adjustedPrinterPortLeft,
											adjustedPrinterPortTop,
											pageEdgeOffset = 0,	 // Takes into account smaller	
																		 // QMS page along portrait top.
																		 // This was changed from 30 to 
																		 // 0 in 11/2001.	
											pageHeight,
											pageWidth;
											
	SignedByte							graphHandleStatus;
							
	PMRect								pmRect;			
	OSStatus								status;
	
	
	
	if (AbortPrint ())
																									return;
	
			// Initialize local variables.													
			
	graphRecordHandle = GetGraphRecordHandle (graphWindowPtr);
	
	graphRecordPtr = (GraphPtr)GetHandleStatusAndPointer (graphRecordHandle,
																			&graphHandleStatus);
																			
	if (graphRecordPtr == NULL)
																									return;
		
			// Get the width and height of the printed page in pixels.	
			
	GetPrinterPageSize (&pageHeight, &pageWidth);	
													
	
			// Get the source graph parameters.
		
	GetWindowPortBounds (graphWindowPtr, &sourceRect);
	
			// If the source rectangle fits within the printer page then just center
			// the graph in the page. 
			// If the source rectangle is larger than the printer page then scale the
			// graph down (the same in both directions) so that it fits within the page.
	
	sourceWidth = sourceRect.right - sourceRect.left + 1;	
	sourceHeight = sourceRect.bottom - sourceRect.top + 1;
				
	gTempRect = sPrintableArea;	
										
	if (pageWidth <= pageHeight)
		{
				// Portrait orientation
				
		adjustedPrinterPortTop = gTempRect.top + pageEdgeOffset;
		adjustedPrinterPortLeft = gTempRect.left;	
			
		scaledPageHeight = (double)pageHeight/sourceHeight;
		scaledPageWidth = (double)pageWidth/sourceWidth;
		
		}	// end "if (pageWidth <= pageHeight)" 
		
	else	// pageWidth > pageHeight 
		{
				// Landscape orientation
				
		adjustedPrinterPortTop = gTempRect.top;
		adjustedPrinterPortLeft = gTempRect.left + pageEdgeOffset;
			
		scaledPageHeight = (double)pageHeight/sourceWidth;
		scaledPageWidth = (double)pageWidth/sourceHeight;
		
		}	// end "else pageWidth > pageHeight"
	
	scaledPage = MIN (scaledPageHeight, scaledPageWidth);
		
	if (scaledPage < 1)
		{
		sourceWidth = (SInt32)(scaledPage * sourceWidth);
		sourceHeight = (SInt32)(scaledPage * sourceHeight);
		
		}	// end "if (scaledPage < 1)"
	
			// Initialize the destination rectangle for the page port. 
			// Note: The graph will be centered within the page.	
			
	destinationRect.left = (pageWidth - sourceWidth)/2 + adjustedPrinterPortLeft;
	destinationRect.right = destinationRect.left + sourceWidth;
															
	destinationRect.top = (pageHeight - sourceHeight)/2 + adjustedPrinterPortTop;
	destinationRect.bottom = destinationRect.top + sourceHeight;
						
			// Open this page.	
		
	pmRect.left = destinationRect.left;
	pmRect.top = destinationRect.top;
	pmRect.right = destinationRect.right;
	pmRect.bottom = destinationRect.bottom;
	
	status = PMSessionBeginPage (printSession, pageFormat, &pmRect);	

			//	Get the graphics port associated with the print context
	
	if (status == noErr)
		status = PMSessionGetGraphicsContext (printSession, 
															//kPMGraphicsContextCoreGraphics,
															kPMGraphicsContextQuickdraw,
															(void**)&printPort);
	
	if (status == noErr)
		{					
		GetPort (&savedPort);
														
				// Save the current window associated with the graph record and set
				// it to the print window.
				
		graphRecordPtr->window = (WindowPtr)printPort;
	
				// Set the printer port.															
			
		SetPort ((GrafPtr)printPort);
	
				// Set the foreground and background pen color for the print window.
		
		ForeColor (blackColor);
		BackColor (whiteColor);
		EraseRect (&gTempRect);
		
				// Set the font and font size for the print window.							
				
		TextFont (gWindowTextFont); 		// monaco  
		TextSize (9);							// 9 point 
	
				// Set the location of the graph for the printer page.
		
		graphRecordPtr->clientRect = destinationRect;
		
		DrawGraph (graphRecordPtr, TRUE);
	
		graphRecordPtr->graphViewCPtr->DrawGraphGrowIcon (graphRecordPtr);
				
		SetPort (savedPort);
		
		}	// end "if (status == noErr)"	
										
			// And close the page.	
			
	if (status == noErr)
		status = PMSessionEndPage (printSession);

			// Set port back to window.														
			
	SetPortWindowPort (graphWindowPtr);
		
			// Make sure that the text is set properly. It gets change after printing.					
			
	TextFont (gWindowTextFont); 		// monaco  
	TextSize (9);							// 9 point 
																			
			// Restore the window associated with the graph record.
			
	graphRecordPtr->window = graphWindowPtr;
	
	MHSetState (graphRecordHandle, graphHandleStatus);
	
}	// end "PrintGraphWindow" 



#else // !TARGET_API_MAC_CARBON
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void PrintGraphWindow
//
//	Software purpose:	The purpose of this routine is to print the 
//							contents of an image window.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 02/10/1999
//	Revised By:			Larry L. Biehl			Date: 12/24/2002			

void PrintGraphWindow (
				WindowPtr 							graphWindowPtr)

{
	Rect									destinationRect,
											sourceRect;
											
	double								scaledPage,
											scaledPageHeight,
											scaledPageWidth;
											
	GraphPtr								graphRecordPtr;
											
	Handle								graphRecordHandle;
	
	SInt32								sourceWidth,
											sourceHeight;
	
	SInt16								adjustedPrinterPortLeft,
											adjustedPrinterPortTop,
											pageEdgeOffset = 0,	 // Takes into account smaller	
																		 // QMS page along portrait top.
																		 // This was changed from 30 to 
																		 // 0 in 11/2001.	
											pageHeight,
											pageWidth;
											
	SignedByte							graphHandleStatus;
	
	
	
	if (AbortPrint ())
																									return;
	
			// Initialize local variables.													
			
	graphRecordHandle = GetGraphRecordHandle (graphWindowPtr);
	
	graphRecordPtr = (GraphPtr)GetHandleStatusAndPointer (graphRecordHandle,
																			&graphHandleStatus,
																			kNoMoveHi);
																			
	if (graphRecordPtr == NULL)
																									return;
																			
			// Save the current window associated with the graph record and set
			// it to the print window.
			
	graphRecordPtr->window = (WindowPtr)pPrPort;
		
			// Get the width and height of the printed page in pixels.	
			
	GetPrinterPageSize (&pageHeight, &pageWidth);	
													
	
			// Get the source graph parameters.
		
	GetWindowPortBounds (graphWindowPtr, &sourceRect);
	
			// If the source rectangle fits within the printer page then just center
			// the graph in the page. 
			// If the source rectangle is larger than the printer page then scale the
			// graph down (the same in both directions) so that it fits within the page.
	
	sourceWidth = sourceRect.right - sourceRect.left + 1;	
	sourceHeight = sourceRect.bottom - sourceRect.top + 1;	
	
	GetPortBounds ((CGrafPtr)pPrPort, &gTempRect);		
										
	if (pageWidth <= pageHeight)
		{
				// Portrait orientation
				
		adjustedPrinterPortTop = gTempRect.top + pageEdgeOffset;
		adjustedPrinterPortLeft = gTempRect.left;	
			
		scaledPageHeight = (double)pageHeight/sourceHeight;
		scaledPageWidth = (double)pageWidth/sourceWidth;
		
		}	// end "if (pageWidth <= pageHeight)" 
		
	else	// pageWidth > pageHeight 
		{
				// Landscape orientation
				
		adjustedPrinterPortTop = gTempRect.top;
		adjustedPrinterPortLeft = gTempRect.left + pageEdgeOffset;
			
		scaledPageHeight = (double)pageHeight/sourceWidth;
		scaledPageWidth = (double)pageWidth/sourceHeight;
		
		}	// end "else pageWidth > pageHeight"
	
	scaledPage = MIN (scaledPageHeight, scaledPageWidth);
		
	if (scaledPage < 1)
		{
		sourceWidth = (SInt32)(scaledPage * sourceWidth);
		sourceHeight = (SInt32)(scaledPage * sourceHeight);
		
		}	// end "if (scaledPage < 1)"
	
			// Initialize the destination rectangle for the page port. 
			// Note: The graph will be centered within the page.	
			
	destinationRect.left = (pageWidth - sourceWidth)/2 + adjustedPrinterPortLeft;
	destinationRect.right = destinationRect.left + sourceWidth;
															
	destinationRect.top = (pageHeight - sourceHeight)/2 + adjustedPrinterPortTop;
	destinationRect.bottom = destinationRect.top + sourceHeight;
	
			// Set the printer port.															
			
	SetPort ((GrafPtr)pPrPort);
	
			// Set the foreground and background pen color for the print window.
	
	ForeColor (blackColor);
	BackColor (whiteColor);
	EraseRect (&gTempRect);
	
			// Set the font and font size for the print window.							
			
	TextFont (gWindowTextFont); 		// monaco  
	TextSize (9);							// 9 point 
				
			// Open this page.															
			
	PrOpenPage (pPrPort, NIL);
	
			// Set the location of the graph for the printer page.
	
	graphRecordPtr->clientRect = destinationRect;
	
	DrawGraph (graphRecordPtr, TRUE);
	
	graphRecordPtr->graphViewCPtr->DrawGraphGrowIcon (graphRecordPtr);
										
			// And close the page.														
			
	PrClosePage (pPrPort);

			// Set port back to window.														
			
	SetPortWindowPort (graphWindowPtr);
																			
			// Restore the window associated with the graph record.
			
	graphRecordPtr->window = graphWindowPtr;
	
	MHSetState (graphRecordHandle, graphHandleStatus);
	
}	// end "PrintGraphWindow" 
#endif 	// TARGET_API_MAC_CARBON, else...



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void PrintImageWindow
//
//	Software purpose:	The purpose of this routine is to print the 
//							contents of an image window.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 08/09/89
//	Revised By:			Larry L. Biehl			Date: 11/01/2001			

#if TARGET_API_MAC_CARBON
void PrintImageWindow (
				WindowPtr 							imageWindowPtr, 
				WindowInfoPtr						imageWindowInfoPtr, 
				double								magnification,
				PMPrintSession						printSession,
				PMPageFormat						pageFormat)

{
	Cell									location;
	
	Rect									destinationRect,
											pageSourceRect,
											portRect,
											sourceRect;
	
	DisplaySpecsPtr					displaySpecsPtr;
	
	GrafPtr								savedPort;
											
	void*									printPort = NULL;
	
	SelectionInfoPtr					selectionInfoPtr;
	
	Handle								displaySpecsH;
	
	ListHandle							legendListHandle,
											printLegendListHandle;
	
	SInt32								middlePageHeight,
											middlePageWidth,
											noPagesHeight,
											noPagesWidth,
											sourceWidth,
											sourceHeight;
	
	SInt16								adjustedPrinterPortLeft,
											adjustedPrinterPortTop,
											bottomPageHeight,
											grafPortType,
											heightPages,
											imagePrintHeight,
											imagePrintWidth,
											leftPageWidth,
											legendLeftStart,
											legendTopStart,
											legendWidth,
											legendWidthPrinted,
											noMiddlePages,
											pageEdgeOffset = 0,	 // Takes into account smaller	
																		 // QMS page along portrait top.	
																		 // This was changed from 30 to 
																		 // 0 in 11/2001.	
											pageHeight,
											pageWidth,
											printHeight,
											printWidth,
											rightPageWidth,
											saveLeftEdge,
											sourceImageHeight,
											sourceImageWidth,
											titleHeight,
											topPageHeight,
											widthPages;
								
	Boolean								legendShowsFlag,
											printLegendFlag;
							
	PMRect								pmRect;			
	OSStatus								beginPageStatus,
											status;
	
	
			// If offScreenMapHandle is NULL, exit routine.									
			
	if (imageWindowInfoPtr->offScreenMapHandle == NULL)															
																									return;
	
			// Initialize local variables.													
			
	legendWidth = imageWindowInfoPtr->legendWidth;
	legendListHandle = imageWindowInfoPtr->legendListHandle;
	titleHeight = imageWindowInfoPtr->titleHeight;
	grafPortType = imageWindowInfoPtr->grafPortType;
	
			// Get the source rectangle.  Use the entire image if no selection	
			// has been made.  If a selection has been made use that selection	
			// rectangle.																			

	selectionInfoPtr =
						(SelectionInfoPtr)(*(imageWindowInfoPtr->selectionInfoHandle));
						
	displaySpecsH = imageWindowInfoPtr->displaySpecsHandle;
	displaySpecsPtr = (DisplaySpecsPtr)*displaySpecsH;

	GetImagePrintPagesInfo (imageWindowInfoPtr,
									magnification,
									&sourceRect,
									&sourceWidth,
									&sourceHeight,
									&pageWidth,
									&pageHeight,
									&middlePageWidth,
									&middlePageHeight,
									&noPagesWidth,
									&noPagesHeight);	
				
	portRect = sPrintableArea;
										
	if (pageWidth <= pageHeight)
		{
		adjustedPrinterPortTop = portRect.top + pageEdgeOffset;
		adjustedPrinterPortLeft = portRect.left;
		
		}	// end "if (pageWidth <= pageHeight)" 
		
	else	// pageWidth > pageHeight 
		{
		adjustedPrinterPortTop = portRect.top;
		adjustedPrinterPortLeft = portRect.left + pageEdgeOffset;
		
		}	// end "else pageWidth > pageHeight" 
	
			// Initialize the destination rectangle for the page port and the		
			// source rectangle for the page.												
			
	destinationRect.top = adjustedPrinterPortTop;
	destinationRect.left = adjustedPrinterPortLeft;
	destinationRect.bottom = destinationRect.top + pageHeight;
	destinationRect.right = destinationRect.left + pageWidth;
	
	pageSourceRect = sourceRect;
	
			// Get the left and top sides of the destination rectangle for the 	
			// first page.  Note: The image will be centered within the page(s).	
			
	destinationRect.left = (noPagesWidth*pageWidth - sourceWidth)/2 +
																				adjustedPrinterPortLeft;
															
	destinationRect.top = (noPagesHeight*pageHeight - sourceHeight)/2 +
																				adjustedPrinterPortTop;
															
			// Save the current left edge.										
			
	saveLeftEdge = destinationRect.left;
	
			// Get the current legend left and top.										
	
	legendLeftStart = destinationRect.left;
	legendTopStart = destinationRect.top;
	
			// Get the left and right page widths.	 										
	
	leftPageWidth = sourceWidth;
	if (noPagesWidth > 1)
		{
		leftPageWidth = destinationRect.right - destinationRect.left - legendWidth;
		sourceImageWidth = leftPageWidth/magnification;
		
				// Now make sure the image print height reflects the actual source
				// being used.
				
		leftPageWidth = legendWidth + sourceImageWidth * magnification + .5;
		
		destinationRect.left = destinationRect.right - leftPageWidth;
		
		}	// end "if (noPagesWidth > 1)"
	
	noMiddlePages = noPagesWidth - 2;
	if (noMiddlePages < 0)
		noMiddlePages = 0;
	
	rightPageWidth = sourceWidth - leftPageWidth - noMiddlePages*middlePageWidth;
	
			// Get the top and bottom page heights.	 									
	
	topPageHeight = sourceHeight;	
	if (noPagesHeight > 1)
		{
		topPageHeight = destinationRect.bottom - destinationRect.top - titleHeight;
		sourceImageHeight = topPageHeight/magnification;
		
				// Now make sure the image print height reflects the actual source
				// being used.
				
		topPageHeight = titleHeight + sourceImageHeight * magnification + .5;
		
		destinationRect.top = destinationRect.bottom - topPageHeight;
		
		}	// end "if (noPagesHeight > 1)"
	
	noMiddlePages = noPagesHeight - 2;
	if (noMiddlePages < 0)
		noMiddlePages = 0;
	
	bottomPageHeight = sourceHeight - topPageHeight - noMiddlePages*middlePageHeight;
	
			// If this is a Thematic type of image window and a legend is to be shown,
			// create the list for the legend for this print window.
	
	printLegendListHandle = NULL;
	
	if (imageWindowInfoPtr->showLegend)
		{
		SetRect (&gViewRect, 
					legendLeftStart+2,
					legendTopStart+15, 
					legendLeftStart+legendWidth-4, 
					destinationRect.bottom-2);
		
		SetRect (&gTempRect, 0, 0, 1, 0);
		location.v = 0;
		location.h = 0;
						
		status = CreateCustomList (&gViewRect, 
											&gTempRect,
											location, 
											&gListDefinitionSpec, 
											NULL, 
											FALSE, 
											FALSE, 
											FALSE, 
											FALSE,
											&printLegendListHandle);
		
		if (status != noErr || printLegendListHandle == NULL)			
			aborted = TRUE;
			
		else	// printLegendListHandle != NULL 
			{
			SetListRefCon (printLegendListHandle, imageWindowInfoPtr->grafPortType);
			LoadThematicLegendList (printLegendListHandle, TRUE);
			
			}	// end "else printLegendListHandle != NULL" 
			
		}	// end "if (imageWindowInfoPtr->showLegend)" 
		
			// Take into account any title for the top pages.							
			
	destinationRect.top += titleHeight;
	
			// Loop through the number of pages for the height.						
			
	for (heightPages=1; heightPages<=noPagesHeight; heightPages++)
		{
		if (AbortPrint ())
			break;
					
				// Set the left edge for the destination rectangle for the page.	
				
		destinationRect.left = saveLeftEdge + legendWidth;
					
				// Set the left edge for the source rectangle for the page.			
				
		pageSourceRect.left = sourceRect.left;
					
				// Get page height for this page.											
		
		printHeight = middlePageHeight;
		if (heightPages == noPagesHeight) 
			printHeight = bottomPageHeight;
		
		if (heightPages == 1) 
			printHeight = topPageHeight;
		
				// Take into account any title for the top pages.						
					
		imagePrintHeight = printHeight;
		if (titleHeight)
			imagePrintHeight = MAX (0, printHeight-titleHeight);
		
		sourceImageHeight = imagePrintHeight/magnification;
		
				// Now make sure the image print height reflects the actual source
				// being used.
				
		imagePrintHeight = sourceImageHeight * magnification + .5;
		
				// Get the bottom edge of the destination rectangle to be printed	
				// into.	 Abort if the destination bottom is beyond the page		
				// bottom.  This should not occur.											
				
		destinationRect.bottom = MIN (destinationRect.top + imagePrintHeight,
												portRect.bottom);
		
				// Get the bottom edge of the source rectangle to be printed		
				// into for this page.  Abort if the source bottom for the page 	
				// is beyond the source bottom.  This should not occur.				
				
		pageSourceRect.bottom = MIN (pageSourceRect.top + sourceImageHeight,
												sourceRect.bottom);
		
				// Determine if the legend is showing for the left page.				
										
		legendWidthPrinted = 0;													
		legendShowsFlag = FALSE;
		if (legendWidth > 0)
			legendShowsFlag = TRUE;
		
				// Loop through the number of pages for the width.						
				
		for (widthPages=1; widthPages<=noPagesWidth; widthPages++)
			{
			if (AbortPrint ())
				break;
					
					// Get page width for this page.											
			
			printWidth = middlePageWidth;
			if (widthPages == noPagesWidth) 
				printWidth = rightPageWidth;
			
			if (widthPages == 1) 
				printWidth = leftPageWidth;
		
					// Take into account any legend for the left pages.				
			
			imagePrintWidth = printWidth;
			if (imageWindowInfoPtr->showLegend && legendShowsFlag)
				imagePrintWidth = MAX (0, printWidth-legendWidth);
			
			sourceImageWidth = imagePrintWidth/magnification;
		
					// Now make sure the image print width reflects the actual source
					// being used.
				
			imagePrintWidth = sourceImageWidth * magnification + .5;
			
					// Get the right edge of the destination rectangle to be 		
					// printed into.  Abort if the destination right edge for the	
					// page is beyond the destination right edge. This should 		
					// not occur.																	
					
			destinationRect.right = MIN (destinationRect.left + imagePrintWidth,
													portRect.right);
		
					// Get the right edge of the source rectangle to be printed		
					// into for this page.  Abort if the source right edge for the	
					// page is beyond the source right edge. This should not occur.
				
			pageSourceRect.right = MIN (pageSourceRect.left + sourceImageWidth,
													sourceRect.right);
						
				// Open this page.	
				
			pmRect.left = destinationRect.left;
			pmRect.top = destinationRect.top;
			pmRect.right = destinationRect.right;
			pmRect.bottom = destinationRect.bottom;
			
					// Make sure there are no errors.
					
			status = PMSessionError (printSession);
			
			if (status == noErr)
				status = beginPageStatus = PMSessionBeginPage (printSession, pageFormat, &pmRect);	

					//	Get the graphics port associated with the print context
			
			if (status == noErr)
				status = PMSessionGetGraphicsContext (printSession, 
																	//kPMGraphicsContextCoreGraphics,
																	kPMGraphicsContextQuickdraw, 
																	(void**)&printPort);
		
					// Copy the reqested part of the image to the printer port.	
			
			printLegendFlag = legendShowsFlag;
			if (heightPages > 1)
				printLegendFlag = FALSE;
			
			if (status == noErr)
				{      
				GetPort (&savedPort);
				SetPort ((GrafPtr)printPort);
				
				if (printLegendFlag)
					SetListPort (printLegendListHandle, (CGrafPtr)printPort);
					
				CopyPrintOffscreenImage (NULL,
													printPort, 
													imageWindowInfoPtr,
													&pageSourceRect,
													&destinationRect,
													magnification,
													titleHeight,
													legendWidth,
													printLegendFlag, 
													printLegendListHandle, 
													legendLeftStart, 
													legendTopStart);
				
				SetPort (savedPort);	
													
				}	// end "if (status == noErr)"
										
					// And close the page.	
					
			if (beginPageStatus == noErr)
				beginPageStatus = PMSessionEndPage (printSession);
			
					// Get the left edge of the destination rectangle for the next	
					// page to be printed.														
					
			destinationRect.left = adjustedPrinterPortLeft;
		
					// Get the left edge of the source rectangle for the next page	
					// to be printed.																
				
			pageSourceRect.left = pageSourceRect.right;
		
					// Determine if the legend is showing for the left page.			
			
			legendWidthPrinted += printWidth;					
			if (legendWidth <= legendWidthPrinted)
				legendShowsFlag = FALSE;
				
			if (status != noErr)
				aborted = TRUE;
				
			}	// end "for (widthPages=1; widthPages<=noPagesWidth; ..." 
			
				// Get the top edge of the destination rectangle for the next		
				// page to be printed.															
				
		destinationRect.top = adjustedPrinterPortTop;
	
				// Get the top edge of the source rectangle for the next page		
				// to be printed.																	
			
		pageSourceRect.top = pageSourceRect.bottom;
		
				// Indicate that no title will printed for additional pages	down.	
				
		titleHeight = 0;
		
		}	// end "for (heightPages=1; heightPages<=..." 
		
			// Dispose of the list handle if it exists, Thematic type 				
			// image windows.																		
	
	if (printLegendListHandle != NULL)
		LDispose (printLegendListHandle);
	
			// Remove the palette for the printer port.									
			
	SetPalette	(GetWindowFromPort ((CGrafPtr)printPort), NULL, FALSE);

			// Set port back to window.														
			
	SetPortWindowPort (imageWindowPtr);
	
}	// end "PrintImageWindow"


 
#else	// !TARGET_API_MAC_CARBON
void PrintImageWindow (
				WindowPtr 							imageWindowPtr, 
				WindowInfoPtr						imageWindowInfoPtr, 
				double								magnification)

{
	Cell									location;
	
	Rect									destinationRect,
											pageSourceRect,
											portRect,
											sourceRect;
	
	DisplaySpecsPtr					displaySpecsPtr;
	
	GrafPtr								savedPort;
	
	SelectionInfoPtr					selectionInfoPtr;
	
	Handle								displaySpecsH;
	
	ListHandle							legendListHandle,
											printLegendListHandle;
	
	SInt32								middlePageHeight,
											middlePageWidth,
											noPagesHeight,
											noPagesWidth,
											sourceWidth,
											sourceHeight;
	
	SInt16								adjustedPrinterPortLeft,
											adjustedPrinterPortTop,
											bottomPageHeight,
											grafPortType,
											heightPages,
											imagePrintHeight,
											imagePrintWidth,
											leftPageWidth,
											legendLeftStart,
											legendTopStart,
											legendWidth,
											legendWidthPrinted,
											noMiddlePages,
											pageEdgeOffset = 0,	 // Takes into account smaller	
																		 // QMS page along portrait top.	
																		 // This was changed from 30 to 
																		 // 0 in 11/2001.	
											pageHeight,
											pageWidth,
											printHeight,
											printWidth,
											rightPageWidth,
											saveLeftEdge,
											sourceImageHeight,
											sourceImageWidth,
											titleHeight,
											topPageHeight,
											widthPages;
								
	Boolean								abortFlag = FALSE,
											legendShowsFlag,
											printLegendFlag;
	
	
			// Initialize local variables.													
			
	legendWidth = imageWindowInfoPtr->legendWidth;
	legendListHandle = imageWindowInfoPtr->legendListHandle;
	titleHeight = imageWindowInfoPtr->titleHeight;
	grafPortType = imageWindowInfoPtr->grafPortType;
	
			// If offScreenMapHandle is NULL, exit routine.									
			
	if (imageWindowInfoPtr->offScreenMapHandle == NULL)															
																									return;
	
			// Get the source rectangle.  Use the entire image if no selection has been
			// made.  If a selection has been made use that selection rectangle.

	selectionInfoPtr = (SelectionInfoPtr)(*(imageWindowInfoPtr->selectionInfoHandle));
						
	displaySpecsH = imageWindowInfoPtr->displaySpecsHandle;
	displaySpecsPtr = (DisplaySpecsPtr)*displaySpecsH;

	GetImagePrintPagesInfo (imageWindowInfoPtr,
									magnification,
									&sourceRect,
									&sourceWidth,
									&sourceHeight,
									&pageWidth,
									&pageHeight,
									&middlePageWidth,
									&middlePageHeight,
									&noPagesWidth,
									&noPagesHeight);
										
	GetPortBounds ((CGrafPtr)pPrPort, &portRect);
										
	if (pageWidth <= pageHeight)
		{
		adjustedPrinterPortTop = portRect.top + pageEdgeOffset;
		adjustedPrinterPortLeft = portRect.left;
		
		}	// end "if (pageWidth <= pageHeight)" 
		
	else	// pageWidth > pageHeight 
		{
		adjustedPrinterPortTop = portRect.top;
		adjustedPrinterPortLeft = portRect.left + pageEdgeOffset;
		
		}	// end "else pageWidth > pageHeight" 
	
			// Initialize the destination rectangle for the page port and the		
			// source rectangle for the page.												
			
	destinationRect.top = adjustedPrinterPortTop;
	destinationRect.left = adjustedPrinterPortLeft;
	destinationRect.bottom = destinationRect.top + pageHeight;
	destinationRect.right = destinationRect.left + pageWidth;
	
	pageSourceRect = sourceRect;
	
			// Get the left and top sides of the destination rectangle for the 	
			// first page.  Note: The image will be centered within the page(s).	
			
	destinationRect.left = (noPagesWidth*pageWidth - sourceWidth)/2 +
																				adjustedPrinterPortLeft;
															
	destinationRect.top = (noPagesHeight*pageHeight - sourceHeight)/2 +
																				adjustedPrinterPortTop;
															
			// Save the current left edge.										
			
	saveLeftEdge = destinationRect.left;
	
			// Get the current legend left and top.										
	
	legendLeftStart = destinationRect.left;
	legendTopStart = destinationRect.top;
	
			// Get the left and right page widths.	 										
	
	leftPageWidth = sourceWidth;
	if (noPagesWidth > 1)
		{
		leftPageWidth = destinationRect.right - destinationRect.left - legendWidth;
		sourceImageWidth = leftPageWidth/magnification;
		
				// Now make sure the image print height reflects the actual source
				// being used.
				
		leftPageWidth = legendWidth + sourceImageWidth * magnification + .5;
		
		destinationRect.left = destinationRect.right - leftPageWidth;
		
		}	// end "if (noPagesWidth > 1)"
	
	noMiddlePages = noPagesWidth - 2;
	if (noMiddlePages < 0)
		noMiddlePages = 0;
	
	rightPageWidth = sourceWidth - leftPageWidth - noMiddlePages*middlePageWidth;
	
			// Get the top and bottom page heights.	 									
	
	topPageHeight = sourceHeight;	
	if (noPagesHeight > 1)
		{
		topPageHeight = destinationRect.bottom - destinationRect.top - titleHeight;
		sourceImageHeight = topPageHeight/magnification;
		
				// Now make sure the image print height reflects the actual source
				// being used.
				
		topPageHeight = titleHeight + sourceImageHeight * magnification + .5;
		
		destinationRect.top = destinationRect.bottom - topPageHeight;
		
		}	// end "if (noPagesHeight > 1)"
	
	noMiddlePages = noPagesHeight - 2;
	if (noMiddlePages < 0)
		noMiddlePages = 0;
	
	bottomPageHeight = sourceHeight - topPageHeight - noMiddlePages*middlePageHeight;
	
			// Set the palette for the printer port.										
			
	SetPalette	(GetWindowFromPort ((CGrafPtr)pPrPort), 
						displaySpecsPtr->paletteObject,
						TRUE);
	ActivatePalette (GetWindowFromPort ((CGrafPtr)pPrPort));
	
	ForeColor (blackColor);
	BackColor (whiteColor);
	EraseRect (&portRect);
		
			// If this is a Thematic type of image window and a legend is to be shown,
			// create the list for the legend for this print window.
	
	printLegendListHandle = NULL;
	
	if (imageWindowInfoPtr->showLegend)
		{
		SetRect (&gViewRect, 
						legendLeftStart+2, 
						legendTopStart+15, 
						legendLeftStart+legendWidth-4, 
						destinationRect.bottom-2);
		
		SetRect (&gTempRect, 0, 0, 1, 0);
		location.v = 0;
		location.h = 0;
		printLegendListHandle = LNew (&gViewRect, 
												&gTempRect,
												location, 
												131, 
												GetWindowFromPort ((CGrafPtr)pPrPort), 
												FALSE, 
												FALSE, 
												FALSE, 
												FALSE);
		
		if (printLegendListHandle == NULL)			
			aborted = TRUE;
			
		else	// printLegendListHandle != NULL 
			{
			(*printLegendListHandle)->refCon = imageWindowInfoPtr->grafPortType;
			LoadThematicLegendList (printLegendListHandle, TRUE);
			
			}	// end "else printLegendListHandle != NULL" 
			
		}	// end "if (imageWindowInfoPtr->showLegend)" 
		
			// Take into account any title for the top pages.							
			
	destinationRect.top += titleHeight;
	
			// Loop through the number of pages for the height.						
			
	for (heightPages=1; heightPages<=noPagesHeight; heightPages++)
		{
		if (AbortPrint ())
			break;
					
				// Set the left edge for the destination rectangle for the page.	
				
		destinationRect.left = saveLeftEdge + legendWidth;
					
				// Set the left edge for the source rectangle for the page.			
				
		pageSourceRect.left = sourceRect.left;
					
				// Get page height for this page.											
		
		printHeight = middlePageHeight;
		if (heightPages == noPagesHeight) 
			printHeight = bottomPageHeight;
		if (heightPages == 1) 
			printHeight = topPageHeight;
		
				// Take into account any title for the top pages.						
					
		imagePrintHeight = printHeight;
		if (titleHeight)
			imagePrintHeight = MAX (0, printHeight-titleHeight);
		sourceImageHeight = imagePrintHeight/magnification;
		
				// Now make sure the image print height reflects the actual source
				// being used.
				
		imagePrintHeight = sourceImageHeight * magnification + .5;
		
				// Get the bottom edge of the destination rectangle to be printed	
				// into.	 Abort if the destination bottom is beyond the page		
				// bottom.  This should not occur.											
				
		destinationRect.bottom = MIN (destinationRect.top + imagePrintHeight,
												portRect.bottom);
		
				// Get the bottom edge of the source rectangle to be printed		
				// into for this page.  Abort if the source bottom for the page 	
				// is beyond the source bottom.  This should not occur.				
				
		pageSourceRect.bottom = MIN (pageSourceRect.top + sourceImageHeight,
												sourceRect.bottom);
		
				// Determine if the legend is showing for the left page.				
										
		legendWidthPrinted = 0;													
		legendShowsFlag = FALSE;
		if (legendWidth > 0)
			legendShowsFlag = TRUE;
		
				// Loop through the number of pages for the width.						
				
		for (widthPages=1; widthPages<=noPagesWidth; widthPages++)
			{
			if (AbortPrint ())
				break;
					
					// Get page width for this page.											
			
			printWidth = middlePageWidth;
			if (widthPages == noPagesWidth) 
				printWidth = rightPageWidth;
			if (widthPages == 1) 
				printWidth = leftPageWidth;
		
					// Take into account any legend for the left pages.				
			
			imagePrintWidth = printWidth;
			if (imageWindowInfoPtr->showLegend && legendShowsFlag)
				imagePrintWidth = MAX (0, printWidth-legendWidth);
			sourceImageWidth = imagePrintWidth/magnification;
		
					// Now make sure the image print width reflects the actual source
					// being used.
				
			imagePrintWidth = sourceImageWidth * magnification + .5;
			
					// Get the right edge of the destination rectangle to be 		
					// printed into.  Abort if the destination right edge for the	
					// page is beyond the destination right edge. This should 		
					// not occur.																	
					
			destinationRect.right = MIN (destinationRect.left + imagePrintWidth,
													portRect.right);
		
					// Get the right edge of the source rectangle to be printed		
					// into for this page.  Abort if the source right edge for the	
					// page is beyond the source right edge. This should not occur.
				
			pageSourceRect.right = MIN (pageSourceRect.left + sourceImageWidth,
													sourceRect.right);
						
					// Open this page.	
					
			PrOpenPage (pPrPort, NIL);
		
					// Copy the reqested part of the image to the printer port.	
			
			printLegendFlag = legendShowsFlag;
			if (heightPages > 1)
				printLegendFlag = FALSE;
                    
			GetPort (&savedPort);
			
			if (!abortFlag)
				{
				SetPort ((GrafPtr)pPrPort);		
				CopyPrintOffscreenImage (GetWindowFromPort ((CGrafPtr)pPrPort),
													(GrafPtr)pPrPort, 
													imageWindowInfoPtr,
													&pageSourceRect,
													&destinationRect,
													magnification,
													titleHeight,
													legendWidth,
													printLegendFlag, 
													printLegendListHandle, 
													legendLeftStart, 
													legendTopStart);
													
				}	// end "if (!abortFlag)"
				
			SetPort (savedPort);	
										
					// And close the page.	
					
			PrClosePage (pPrPort);
			
					// Get the left edge of the destination rectangle for the next	
					// page to be printed.														
					
			destinationRect.left = adjustedPrinterPortLeft;
		
					// Get the left edge of the source rectangle for the next page	
					// to be printed.																
				
			pageSourceRect.left = pageSourceRect.right;
		
					// Determine if the legend is showing for the left page.			
			
			legendWidthPrinted += printWidth;					
			if (legendWidth <= legendWidthPrinted)
				legendShowsFlag = FALSE;
				
			if (abortFlag)
				break;
				
			}	// end "for (widthPages=1; widthPages<=noPagesWidth; ..." 
			
				// Get the top edge of the destination rectangle for the next		
				// page to be printed.															
				
		destinationRect.top = adjustedPrinterPortTop;
	
				// Get the top edge of the source rectangle for the next page		
				// to be printed.																	
			
		pageSourceRect.top = pageSourceRect.bottom;
		
				// Indicate that no title will printed for additional pages	down.	
				
		titleHeight = 0;
		
		}	// end "for (heightPages=1; heightPages<=..." 
		
			// Dispose of the list handle if it exists, Thematic type 				
			// image windows.																		
	
	if (printLegendListHandle != NULL)
		LDispose (printLegendListHandle);
	
			// Remove the palette for the printer port.									
			
	SetPalette	(GetWindowFromPort ((CGrafPtr)pPrPort), NULL, FALSE);

			// Set port back to window.														
			
	SetPortWindowPort (imageWindowPtr);
	
}	// end "PrintImageWindow" 
#endif	// TARGET_API_MAC_CARBON, else...


/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void PrintTextWindow
//
//	Software purpose:	The purpose of this routine is to print the 
//							contents of a text window.
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
// Called By:		
//
//	Coded By:			THINK C					Date: ??/??/??
//	Revised By:			Larry L. Biehl			Date: 11/17/93			

void PrintTextWindow (
				WindowPtr 							wPtr, 
				WindowInfoPtr						windowInfoPtr)

{
	Rect									pageRect;
	
	Str255								date,
											time;
											
	//LineAPtr							lineArrayPtr;
	Ptr									ptr;
	//TextArrayPtr						textArrayPtr;
	TPPrint								pPrint = *gHPrint;
	
	SInt32								boundsTop,
											lineOffset,
											positionEnd,
											positionStart,
											relativeLengthSelText,
											scrollLocation,
											textBlockOffset;
	
	UInt32								dateTime;
	
	SInt16			   				bDevLaser = 3,
											controlValue,
				    						laser,
				    						oldScrollVal,
				  							vPixelsPerPage;
				  							
	Boolean								entireTextFlag;
	
	
			// Get the document name. 															
	
	GetWTitle (wPtr, gTextString);

			// And the current time and date. 												
			
	GetDateTime (&dateTime);
	IUDateString (dateTime, longDate, date);
	IUTimeString (dateTime, FALSE, time);
	pDate = date;
	pTime = time;

			// Get font characterstics. 														
			
	lineHeight = gOutputLineHeight-2;
	ascent = 9;
	relativeLengthSelText = 1;

			// Adjust for a laserwriter. 														
	
	laser = (*((char*)&(**gHPrint).prStl.wDev));	
	if (laser == bDevLaser)
		lineHeight++;

			// Set up the printer port characterstics. 									
			
	SetPort ((GrafPtr)pPrPort);
	TextFont (gWindowTextFont);
	TextSize (docSize = gOutputLineHeight-2);
	TextFace (0);
	TextMode (srcOr);

			// Calculate what portion of the page will be printed in. 				
			
	pageRect = (**gHPrint).prInfo.rPage;
	pageTopLeft = topLeft (pageRect);
	pageTopLeft.h += 30;
	pageRight = pageRect.right;
	pageRect.top += 3 * lineHeight + lineHeight / 2;
	pageRect.left += 30;
	
			// Figure out how many vertical pixels there are on a page.  This		
			// will be used for adjusting the scroll bar.								
		   	
	vPixelsPerPage = pageRect.bottom - pageRect.top;
	
			// Remember the scroll position. 												
				
	oldScrollVal = GetControlValue (windowInfoPtr->verticalControl);

	if (!AbortPrint ())
		{
				// Get the selected start of the text to print.							
				
		//WSGetSelect (gOutputTextH, &positionStart, &positionEnd);
		WEGetSelection (&positionStart, &positionEnd, gOutputTextH);
		entireTextFlag = FALSE;
		if (positionEnd == positionStart)
			{
					// Use the entire text window.											
					
			positionStart = 0;
			//positionEnd = (*gOutputTextH)->tLength - 2;
			positionEnd = WEGetTextLength (gOutputTextH) - 2;
			entireTextFlag = TRUE;
			scrollLocation = 0;
			
			}	// end "if (positionEnd == positionStart)" 
			
				// Get the relative pixel location of the topmost line to be 		
				// and the bottom most line of selected text to be printed.			
		
		if (!entireTextFlag)
			{
			textBlockOffset = (*gOutputTextH)->tBegin.Buf;
			ptr = (Ptr)*(*gOutputTextH)->tHandle;
			textArrayPtr = (TextArrayPtr)&ptr[textBlockOffset];
			
			lineOffset = (*gOutputTextH)->tBegin.Line - textArrayPtr[0]->TBLineBeg;
			ptr = (Ptr)*(textArrayPtr[0]->TBLines);
			lineArrayPtr = (LineAPtr)&ptr[lineOffset];
			boundsTop = lineArrayPtr[0]->Top;
			
			textBlockOffset = (*gOutputTextH)->tEnd.Buf;
			ptr = (Ptr)*(*gOutputTextH)->tHandle;
			textArrayPtr = (TextArrayPtr)&ptr[textBlockOffset];
			
			lineOffset = (*gOutputTextH)->tEnd.Line - textArrayPtr[0]->TBLineBeg;
			ptr = (Ptr)*(textArrayPtr[0]->TBLines);
			lineArrayPtr = (LineAPtr)&ptr[lineOffset];
			
			relativeLengthSelText = 
						lineArrayPtr[0]->Top + lineArrayPtr[0]->Height - boundsTop;
						
			scrollLocation = boundsTop - (*gOutputTextH)->tRect.bottom;
			
			}	// end "if (!entireTextFlag)" 
			
				// Start with page 1. 															
				
		pageNum = 1;
	
		do
			{
					// Set scroll bar to reflect progress.									
						
			controlValue = scrollLocation/gOutputVScrollFactor;
			SetPort (gOutputWindow);
			SetControlValue (windowInfoPtr->verticalControl, controlValue);
			SetPort ((GrafPtr)pPrPort);
						
			if (AbortPrint ())
				break;
				
					// Open a new page.															
					
			PrOpenPage (pPrPort, NIL);
		
					// If the user hasn't aborted, print the page header.				
					
			if (!AbortPrint ())
				DoPageHeader ();
				
					// If selected text is being printed then, check if this is 	
					// the last page to be needed.  If so, then set pageRect so 	
					// that we do not print to much at the bottom of the page.		
					
			if (!entireTextFlag)
				{	
				relativeLengthSelText -= vPixelsPerPage;
				if (relativeLengthSelText < 0)
					pageRect.bottom += relativeLengthSelText;
					
				}	// end "if (!entireTextFlag)" 
		
					// If the user hasn't aborted, print the page.						
					
			if (!AbortPrint ())
				positionStart = WSPrint (gOutputTextH, positionStart, &pageRect);
		
					// And close the page.														
					
			PrClosePage (pPrPort);
						
			pageNum++;
			scrollLocation += vPixelsPerPage;
			
					// Check if end of selected text.										
					
			if (relativeLengthSelText <= 0)
				positionStart = -1;
			
			}		while (positionStart >= 0);
		
		}	// end "if (!AbortPrint ())" 

			// Reset the scroll bar to the original value.								
			
	SetControlValue (windowInfoPtr->verticalControl, oldScrollVal);

			// Set port back to window.														
			
	SetPort (wPtr);
	
}	// end "PrintTextWindow" 
*/


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean SetUpPrintedImageInfo
//
//	Software purpose:	The purpose of this routine is to set up the 
//							information needed to print the image window.  This
//							routine will also give the user a chance to cancel
// 						the print request if he is not happy with the
//							image layout on the printed pages.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 09/05/89
//	Revised By:			Larry L. Biehl			Date: 06/28/96			

Boolean SetUpPrintedImageInfo (
				WindowInfoPtr						imageWindowInfoPtr, 
				double*								magnificationPtr,
				Rect*									printableAreaPtr)

{
	Rect									sourceRect,
											theImageRect,
											theRect;
	
	double								magHeight,
											onePageScaleMagnification,
											userDefinedMagnification;
										
	DialogPtr							dialogPtr;
	DisplaySpecsPtr					displaySpecsPtr;
	
	UserItemUPP							drawPrintImagePtr;
	
	Handle								offScreenMapH,
											theHandle;
	
	SInt16								itemHit,
											legendWidth,
											pageHeight,
											pageWidth,
											theType,
											titleHeight;
		
	Boolean								modalDone,
											returnFlag;
								

	
	offScreenMapH = imageWindowInfoPtr->offScreenMapHandle;
	
			// If offScreenMapH is NULL, exit routine.									
			
	if (offScreenMapH == NULL)													return (FALSE);
	
	if (gImageWindowInfoPtr != NULL)
																						return (FALSE);
						
			// Get the legend width and title height.										
			
	legendWidth = imageWindowInfoPtr->legendWidth;
	titleHeight = imageWindowInfoPtr->titleHeight;
			
			// Get the source rectangle.  Use the entire image if no selection	
			// has been made.  If a selection has been made use that selection	
			// rectangle.																			
												
	GetSelectedOffscreenRectangle (imageWindowInfoPtr,
												&sourceRect, 
												TRUE,
												TRUE);
	
			// Get the width and height of the printed page in pixels.				
			
	GetPrinterPageSize (&pageHeight, &pageWidth);
		
			// Get the magnification for an image scaled to one page.				
			
	*magnificationPtr = ((double)pageWidth - legendWidth)/
															(sourceRect.right - sourceRect.left);
	
	magHeight = ((double)pageHeight - titleHeight)/
															(sourceRect.bottom - sourceRect.top);
	
	onePageScaleMagnification = MIN (magHeight, *magnificationPtr);
	
	displaySpecsPtr = (DisplaySpecsPtr)*imageWindowInfoPtr->displaySpecsHandle;
	userDefinedMagnification = displaySpecsPtr->magnification;
	*magnificationPtr = userDefinedMagnification;
	
			// Get the modal dialog for the print page layout.							
				
	dialogPtr =
			LoadRequestedDialog (kPageLayoutSpecificationID, kDoNotCopyScrap, 1, 2);
	if (dialogPtr == NULL)													
																						return (TRUE);
	
			// Set global image window info pointer for the DrawPrintImage 
			// routine.
																						
	gImageWindowInfoPtr = imageWindowInfoPtr;
		
			// Intialize local user item proc pointers.	
			
	drawPrintImagePtr = NewUserItemUPP (DrawPrintImage);

	SetDialogItemDrawRoutine (dialogPtr, 3, &theImageRect, drawPrintImagePtr);
	
			// Set routine to draw outline on "OK" box.									
			
	SetDialogItemDrawRoutine (dialogPtr, 8, &theRect, gHiliteOKButtonPtr);	
	
			// Center the dialog and then show it.											
			
	ShowDialogWindow (dialogPtr, kPageLayoutSpecificationID, kDoNotSetUpDFilterTable);
	
	modalDone = FALSE;
	itemHit = 0;
	do 
		{
	   ModalDialog (gProcessorDialogFilterPtr, &itemHit);
	   
		if	(itemHit == 1)      // User selected OK 
			{
			modalDone = TRUE;
			returnFlag = FALSE;
			
			}	// end "if	(itemHit == 1)" 
	   
		if	(itemHit == 2)      // User selected Cancel 
			{
			modalDone = TRUE;
			returnFlag = TRUE;
			
			}	// end "if	(itemHit == 2)" 
			
		if (itemHit == 7)
			{
			GetDialogItem (dialogPtr, 7, &theType, &theHandle, &theRect);
			if (theHandle != NULL)
				{
				if (GetControlValue ((ControlHandle)theHandle))
					{
					SetControlValue ((ControlHandle)theHandle, 0);
					*magnificationPtr = userDefinedMagnification;
					
					}	// end "if (GetControlValue ((ControlHandle)theHandle))" 
											
				else	// !GetControlValue (theHandle) 
					{
					SetControlValue ((ControlHandle)theHandle, 1);
					*magnificationPtr = onePageScaleMagnification;
					
					}	// end "else !GetControlValue ((ControlHandle)theHandle)" 
					
				InvalWindowRect (GetDialogWindow (dialogPtr), &theImageRect);
				
						// Invalidate the item with the number of pages so that it
						// will be updated properly.
				
				GetDialogItem (dialogPtr, 4, &theType, &theHandle, &theRect);
				InvalWindowRect (GetDialogWindow (dialogPtr), &theRect);
					
				}	// end "if (theHandle != NULL)" 
			
			}	// end "if (itemHit == 7)" 
				
		} while (!modalDone);
		
	DisposeUserItemUPP (drawPrintImagePtr);
	
	HideWindow (GetDialogWindow (dialogPtr));
	DisposeDialog (dialogPtr);		
	gImageWindowInfoPtr = NULL;
	
	return (returnFlag);
	
}	// end "SetUpPrintedImageInfo" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		short int ValidatePrintRecord
//
//	Software purpose:	The purpose of this routine is to validate the
//							print record.  If the print record has not been
//							initialize yet (gPrinterOK is FALSE), then PrintDefault
//							is called.  If gPrinterOK is TRUE then PrValidate
//							is called.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 04/29/94
//	Revised By:			Larry L. Biehl			Date: 04/29/94			

SInt16 ValidatePrintRecord (void)

{
#if !TARGET_API_MAC_CARBON
	Boolean								changedFlag;
	SInt16								savedPrError;
	
	
	PrOpen ();
		
	if (PrError () == noErr)
		{
		if (gPrinterOK)
			{
			changedFlag = PrValidate (gHPrint);
				
			}	// end "if (gPrinterOK)" 
			
		else	// !gPrinterOK 
			{
			PrintDefault (gHPrint);
			
			if (PrError () == noErr)
				gPrinterOK = TRUE;
				
			}	// end "else !gPrinterOK" 
		
		}	// end "if (PrError () == noErr)" 
		
	savedPrError = PrError ();
		
	PrClose ();
		
	return (savedPrError);
#endif 	// !TARGET_API_MAC_CARBON

#if TARGET_API_MAC_CARBON
	return (noErr);
#endif 	// TARGET_API_MAC_CARBON
	
}	// end "ValidatePrintRecord"