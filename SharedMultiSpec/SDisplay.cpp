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
//	File:						SDisplay.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			05/29/2020
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	The routines in this file control the display of
// 							image data in the image windows.  The routines in
//								this file will call routines in the displayMultispectral
//								file or the displayThematic file depending on the 
//								type of image that it is.
//
/* Template for debugging
		int numberChars = sprintf ((char*)gTextString3,
													" SDisplay.cpp: (): %s",
													gEndOfLine);
		ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include "SDisplay_class.h"
#include "SPalette_class.h"

#if defined multispec_mac   

#endif	// defined multispec_mac   

#if defined multispec_wx
	#include "xImageDoc.h" 
	#include "xImageFrame.h"
	#include "xImageView.h"
	#include "xMultiSpec.h"
	#include "wx/display.h"
	#if defined multispec_wxlin
		#include "wx/rawbmp.h"
	#endif
#endif

#if defined multispec_win
	#include "SImageWindow_class.h"
	#include "WImageDoc.h" 
	#include "WImageFrame.h"
	#include "WImageView.h"
#endif	// defined multispec_win



		// Prototype descriptions for routines in this file that are only		
		// called by routines in this file.	

Boolean GetIncludeVectorOverlaysFlag (
				WindowInfoPtr						windowInfoPtr);

void SetUpImageWindowTypeParameters (
				WindowPtr							windowPtr,
				WindowInfoPtr						imageWindowInfPtr,
				FileInfoPtr							imageFileInfoPtr,
				DisplaySpecsPtr					displaySpecsPtr);

void SetWindowToImageSize (
				WindowPtr							windowPtr,
				WindowInfoPtr						imageWindowInfoPtr,
				DisplaySpecsPtr					displaySpecsPtr);



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CheckSomeDisplayEvents
//
//	Software purpose:	The purpose of this routine is to set the portpixmaps
//							and then check if any events need to be handled.  The
//							portpixmaps need to be reset in case the display operation
//							is placed in the background.  The screen portpixmap
//							needs to be used for this case not the offscreen
//							portpixmap.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 02/02/1994
//	Revised By:			Larry L. Biehl			Date: 04/15/2019

Boolean CheckSomeDisplayEvents (
				WindowInfoPtr						windowInfoPtr,
				DisplaySpecsPtr					displaySpecsPtr,
				LCToWindowUnitsVariables* 		lcToWindowUnitsVariablesPtr,
				PixMapHandle						savedPortPixMapH,
				PixMapHandle						offScreenPixMapH,
				LongRect*							sourceRectPtr,
				SInt32								displayBottomMax)

{
	#if defined multispec_mac 
		PixMapHandle imageWindowPortPixMap;
	#endif	// defined multispec_mac 

	Boolean returnFlag = TRUE;

	if (displayBottomMax != -1)
		InvalidateImageSegment (windowInfoPtr,
											//displaySpecsPtr,
											lcToWindowUnitsVariablesPtr,
											sourceRectPtr,
											displayBottomMax);
	
	#if defined multispec_wx
		if (displaySpecsPtr->drawSideBySideTitleFlag)
			(gActiveImageWindow->m_frame->m_titleBar)->
						RefreshRect (wxRect (0,
													1,
													sourceRectPtr->right-sourceRectPtr->left+1,
													14),
											true);
	
		//gActiveImageWindow->OnUpdate (NULL, NULL);
				// Force palette info to bitmap to be loaded again since since more of
				// image may have been loaded.
	
		displaySpecsPtr->paletteObject->SetPaletteLoadedFlag (FALSE);
	
				// Need to indicate that image bitmap is not being modified
	
		if (windowInfoPtr->offscreenMapSize == 0)
			EndBitMapRawDataAccess (&gActiveImageViewCPtr->m_ScaledBitmap);
	
		//gActiveImageWindow->OnUpdate (gActiveImageWindow, NULL);
		//(gActiveImageWindow->m_Canvas)->Update ();
	#endif

	#if defined multispec_win  
		gActiveImageWindow->UpdateWindow ();
	#endif	// defined multispec_win    

	returnFlag = CheckSomeEvents (
								osMask + keyDownMask + mDownMask + updateMask + mUpMask);
	
	#if defined multispec_wx
				// Get pointer to the raw bitmap data and indicate that bitmap is
				// ready to be modified.
		if (windowInfoPtr->offscreenMapSize == 0)
			windowInfoPtr->imageBaseAddressH = (HPtr)BeginBitMapRawDataAccess (
															windowInfoPtr,
															&gActiveImageViewCPtr->m_ScaledBitmap);
	#endif

	if (!returnFlag)
		sourceRectPtr->bottom = -1;

	gNextTime = TickCount () + kDisplayTimeOffset;

	return (returnFlag);

}	// end "CheckSomeDisplayEvents" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 CheckNumberDisplayColumns
//
//	Software purpose:	The purpose of this routine is to check if the number
//							of columns to be displayed is less than 'gMaxRowBytes'
//							for 8-bit data, 'gMaxRowBytes'/2 for 16-bit data
//							and 'gMaxRowBytes'/4 for 24-bit data.
//							1 is returned if the number of display columns is less
//							than the maximum and 0 is returned if the number of
//							display columns is equal to or more than the maximum.
//
//	Parameters in:					
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/18/1990
//	Revised By:			Larry L. Biehl			Date: 03/16/1999	

SInt16 CheckNumberDisplayColumns (
				UInt32								columnStart,
				UInt32								columnEnd,
				UInt32								columnInterval,
				SInt16								pixelSize,
				SInt16								displayType,
				SInt16								numberChannels,
				DialogPtr							dialogPtr,
				SInt16								startItem)

{
	UInt32								numberColumns;
	
	SInt16								alertStringNumber,
											returnCode;


	numberColumns = (columnEnd - columnStart + columnInterval) / columnInterval;

	if (displayType == kSideSideChannelDisplayType)
		numberColumns = numberChannels * (numberColumns + 2);

	#if defined multispec_mac
		Boolean 								tooManyFlag = FALSE;


		UInt32 numberRowBytes = GetNumberPixRowBytes (numberColumns, pixelSize);
		if (numberRowBytes > gMaxRowBytes)
			tooManyFlag = TRUE;

		if (gOSXCoreGraphicsFlag && numberColumns > kMaxNumberOSXDisplayColumns)
			//if (gOSXFlag && numberColumns > 128000)
			tooManyFlag = TRUE;

		if (tooManyFlag) 
			{
			NumberErrorAlert ((SInt32)columnStart, dialogPtr, startItem);

			alertStringNumber = IDS_DisplayColumnLimit;
			if (gQD32IsImplemented)
				alertStringNumber = kSQD32DisplayColumnLimit;
				
			if (gOSXCoreGraphicsFlag)
				alertStringNumber = IDS_Alert92;

					// Display an alert.	

			Handle okHandle;
			SInt16 theType;

			GetDialogItem (dialogPtr, 1, &theType, &okHandle, &gTempRect);
			HiliteControl ((ControlHandle)okHandle, 255);

			returnCode = DisplayAlert (kErrorAlertID,
												 kCautionAlert,
												 kAlertStrID,
												 alertStringNumber,
												 0,
												 NULL);

					// Force the main dialog box to be redrawn.

			InvalWindowRect (GetDialogWindow (dialogPtr),
									GetPortBounds (GetDialogPort (dialogPtr), &gTempRect));

			HiliteControl ((ControlHandle)okHandle, 0);
																								return (0);

			}	// end "if (tooManyFlag)"
	#endif	// defined multispec_mac      													

	#if defined multispec_win
		UInt32 maxNumberColumns = gMaxRowBytes;
		if (pixelSize > 8)
		  maxNumberColumns = gMaxRowBytesFor24Bits;

		if (numberColumns > maxNumberColumns) 
			{
			NumberErrorAlert ((SInt32)columnStart, dialogPtr, startItem);

			alertStringNumber = IDS_32BitDisplayColumnLimit;

					// Display an alert.

			returnCode = DisplayAlert (kErrorAlertID,
												 kCautionAlert,
												 kAlertStrID,
												 alertStringNumber,
												 0,
												 NULL);
			return (0);

			}	// end "if (numberColumns > maxNumberColumns)"		 
	#endif	// defined multispec_win

	#if defined multispec_wx
				// See note with gMaxRowBytes in xMultiSpec.cpp concerning the maximum
				// number of row bytes.
	
		UInt32 maxNumberColumns = gMaxRowBytes;

		if (numberColumns > maxNumberColumns) 
			{
			NumberErrorAlert ((SInt32)columnStart, dialogPtr, startItem);

			#if defined multispec_wxlin
				alertStringNumber = IDS_32BitDisplayColumnLimit_lin;
			#endif
			#if defined multispec_wxmac
				alertStringNumber = IDS_32BitDisplayColumnLimit_mac;
			#endif

					// Display an alert.

			returnCode = DisplayAlert (kErrorAlertID,
												 kCautionAlert,
												 kAlertStrID,
												 alertStringNumber,
												 0,
												 NULL);
			return (0);

			}	// end "if (numberColumns > maxNumberColumns)"
	#endif	// defined multispec_wx

	return (1);

}	// end "CheckNumberDisplayColumns" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 CheckNumberDisplayLines
//
//	Software purpose:	The purpose of this routine is to check if the number
//							of lines to be displayed is less than 32767.
//							1 is returned if the number of display lines is less
//							than the maximum and 0 is returned if the number of
//							display lines is equal to or more than the maximum.
//
//	Parameters in:					
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/20/1992
//	Revised By:			Larry L. Biehl			Date: 10/16/2001	

SInt16 CheckNumberDisplayLines (
				SInt32								lineStart,
				SInt32								lineEnd,
				SInt32								lineInterval,
				DialogPtr							dialogPtr,
				SInt16								startItem)

{
	UInt32								maxNumberDisplayLines,
											numberLines;

	SInt16								returnCode;


	numberLines = (lineEnd - lineStart + lineInterval) / lineInterval;

	maxNumberDisplayLines = kMaxNumberDisplayLines;
	if (gOSXCoreGraphicsFlag)
		maxNumberDisplayLines = kMaxNumberDisplayLines;

	if (numberLines > maxNumberDisplayLines) 
		{
		NumberErrorAlert (lineStart, dialogPtr, startItem);

				// Display an alert.													

		#if defined multispec_mac
			Handle okHandle;
			SInt16 theType;

			GetDialogItem (dialogPtr, 1, &theType, &okHandle, &gTempRect);
			HiliteControl ((ControlHandle)okHandle, 255);
		#endif	// defined multispec_mac

		returnCode = DisplayAlert (kErrorAlertID,
											kCautionAlert,
											kAlertStrID,
											IDS_DisplayLineLimit,
											0,
											NULL);

		#if defined multispec_mac
				// Force the main dialog box to be redrawn.

			InvalWindowRect (GetDialogWindow (dialogPtr),
									GetPortBounds (GetDialogPort (dialogPtr), &gTempRect));

			HiliteControl ((ControlHandle)okHandle, 0);
		#endif	// defined multispec_mac
	  
		return (0);

		}	// end "if (numberLines > kMaxNumberDisplayLines)" 

	return (1);

}	// end "CheckNumberDisplayLines"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		DisplayColorImage
//
//	Software purpose:	The purpose of this routine is to set up the
//							off screen pix map and call the routines to fill
//							the off screen buffer
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			DisplayImage
//
//	Coded By:			Larry L. Biehl			Date: 08/11/1988
//	Revised By:			Wei-Kang Hsu			Date: 10/14/2015
//	Revised By:			Larry L. Biehl			Date: 11/12/2019

void DisplayColorImage (
				DisplaySpecsPtr					displaySpecsPtr,
				HistogramSpecsPtr					histogramSpecsPtr)

{
	#if defined multispec_mac
		Rect 									rect;
		PixMapHandle						imageWindowPortPixMap = NULL;
	#endif
	#if defined multispec_win
		Rect 									rect;
	#endif
	
	LongRect								sourceRect;


	FileInfoPtr							fileInfoPtr;
	
	LCToWindowUnitsVariables 		lcToWindowUnitsVariables;

	PixMapHandle						offScreenPixMapH = NULL,
											savedImageWindowPortPixMap = NULL;

	HPtr									offScreenBufferPtr = NULL;

	SInt32								displayBottomMax;

	UInt32								pixRowBytes;

	SInt16								displayCode = 0,
											numberChannels = 0;

	Boolean								loadLegendListFlag = FALSE;


			// Check status of file information structure.

	if (gImageFileInfoPtr == NULL)
																									return;

			// Handle to file information should already be locked.  Get pointer
			// to file information.																
			// Make certain that we have valid 32 bit pointers in case we go to	
			// 32 bit mode.																		

	fileInfoPtr = gImageFileInfoPtr;

	SetUpImageWindowTypeParameters (gActiveImageWindow,
												gImageWindowInfoPtr,
												fileInfoPtr,
												displaySpecsPtr);

			// Set up structures to hold the offscreen bit maps.

	if (!SetUpColorImageMemory (displaySpecsPtr, &sourceRect, &pixRowBytes))
																									return;
	
			// Update scrolls to be ready to write image to window.
			// This call needs to be before the offscreenBufferPtr is set to access
			// the bitmap. UpdateActiveImageScrolls may also access a pointer to
			// the bitmap. There can be only one at a time for wxWidgets
			// Note tried leaving this out for wxWidgets since for large images,
			// CopyOffscreen was was called before loading anything in the bitmap.
			// Cost time, but leaving out caused problems with initial image thinking
			// there were scroll bars present.
	
	UpdateActiveImageWScrolls (displaySpecsPtr->magnification);
	
	gImageWindowInfoPtr->drawBitMapFlag = FALSE;

			// Erase the image window before starting to draw

	#if defined multispec_wx
		gActiveImageViewCPtr->ClearView (false);
		//gActiveImageViewCPtr->OnUpdate (NULL, NULL);
				// Do not allow updates when arrow being used to go from channel
				// to channel using arrows.
				// We do want to allow for updates if the display is from the case when
				// an image is being opened directly from a drag drop situation. We
				// want this done hear so that the entire image window is not updated
				// the first time for large images when only a few lines have been
				// loaded into the bitmap
				
		if (gCallProcessorDialogFlag || gFromToolParameterFileFlag)
			#if defined multispec_wxlin
				gActiveImageViewCPtr->m_frame->Update ();
			#endif
			#if defined multispec_wxmac
				gActiveImageViewCPtr->m_frame->Update ();
				//((CMultiSpecApp*)wxTheApp)->SafeYieldFor (NULL, wxEVT_CATEGORY_UI);
			#endif
	#endif   //	defined multispec_wx

	#if defined multispec_mac
		GetWindowClipRectangle (gActiveImageWindow, kImageFrameArea, &rect);
		rect.top -= GetTitleHeight (gActiveImageWindowInfoH);
		rect.left = 0;

		if (displaySpecsPtr->displayType == k1_ChannelThematicDisplayType)
			rect.bottom += kSBarWidth;

		::ClipRect (&rect);

		if (gOutputForce1CodeSetting != 0)
			::EraseRect (&rect);

				// Activate the palette

		::SetPalette (gActiveImageWindow, displaySpecsPtr->paletteObject, TRUE);
		::ActivatePalette (gActiveImageWindow);
	#endif   //	defined multispec_mac

	#if defined multispec_win
		GetWindowClipRectangle (gActiveImageWindow, kImageFrameArea, &rect);
		gActiveImageViewCPtr->InvalidateRect (
												(RECT*)&rect, (gOutputForce1CodeSetting != 0));

				// Save the imageBaseAddressH handle and set the same to NULL in the
				// image window info structure so that the window will be erased
				// and not redrawn.

		Handle imageBaseAddressH = gImageWindowInfoPtr->imageBaseAddressH;
		gImageWindowInfoPtr->imageBaseAddressH = NULL;
		gActiveImageViewCPtr->UpdateWindow ();
		gImageWindowInfoPtr->imageBaseAddressH = imageBaseAddressH;
	#endif   //	defined multispec_win
	
			// Set some variables for use when updating the image window.
	
	SetLCToWindowUnitVariables (gImageWindowInfoPtr->windowInfoHandle,
											kWindowsUseOrigin,
											FALSE,
											&lcToWindowUnitsVariables);
	
	gImageWindowInfoPtr->drawBitMapFlag = TRUE;

	displayBottomMax = sourceRect.bottom;

	#if defined multispec_mac
		offScreenBufferPtr = (HPtr)GetHandlePointer (
												gImageWindowInfoPtr->imageBaseAddressH);

		offScreenPixMapH =
				 (PixMapHandle)gImageWindowInfoPtr->offScreenMapHandle;
		pixRowBytes = gImageWindowInfoPtr->cgInfo.contextRowBytes;
	#endif	// defined multispec_mac

	#if defined multispec_win
		offScreenBufferPtr = (HPtr)GetHandlePointer (
													gImageWindowInfoPtr->imageBaseAddressH);
		offScreenPixMapH = (PixMapHandle)gImageWindowInfoPtr->offScreenMapHandle;

				// Make adjustment for DIB with origin in the lower left hand corner

		//offScreenBufferPtr +=
		//		 			pixRowBytes * (sDisplaySpecsPtr->imageDimensions[kVertical] - 1);
		//pixRowBytes = -pixRowBytes;
	#endif	// defined multispec_win

	#if defined multispec_wx
		if (gImageWindowInfoPtr->offscreenMapSize == 0)
					// multispectral display window
			offScreenBufferPtr = (HPtr)BeginBitMapRawDataAccess (
															gImageWindowInfoPtr,
															&gActiveImageViewCPtr->m_ScaledBitmap);
	
		else	// gImageWindowInfoPtr->offscreenMapSize > 0
					// thematic display window
			offScreenBufferPtr = (HPtr)GetHandlePointer (
													gImageWindowInfoPtr->imageBaseAddressH);
	
		offScreenPixMapH = (PixMapHandle)gImageWindowInfoPtr->offScreenMapHandle;
	#endif

	if (offScreenBufferPtr != NULL)
		{
		if (gImageWindowInfoPtr->imageType == kMultispectralImageType)
			{
					// Do 8 bit color displays here.

			if (displaySpecsPtr->pixelSize == 8)
				{
						// Do multispectral type images here.

				switch (displaySpecsPtr->displayType)
					{
					case k1_ChannelThematicDisplayType:
						numberChannels = 1;
						displayCode = 51;
						break;

					case k1_ChannelGrayLevelDisplayType:
						numberChannels = 1;
						displayCode = 1;
						break;

					case k3_ChannelDisplayType:
						numberChannels = 3;
						displayCode = 3;
						break;
					
					//case k2_ChannelDisplayType:
					case k3_2_ChannelDisplayType:
						numberChannels = 2;
						displayCode = 2;
						break;
					
					case kSideSideChannelDisplayType:
						gImageWindowInfoPtr->titleHeight = gDefaultTitleHeight;
						#if defined multispec_wx
									// The image top offset is 0 for wx implementation since
									// the coordinate bar is not a part of the image window.
						
							gImageWindowInfoPtr->imageTopOffset = 0;
						
							CMImageDoc* imageDocCPtr =
												(CMImageDoc*)gActiveImageViewCPtr->GetDocument ();
							CMImageFrame* imageFrameCPtr = imageDocCPtr->GetImageFrameCPtr ();
							imageFrameCPtr->ShowTitleView (1);
						#else
							gImageWindowInfoPtr->imageTopOffset =
								  gDefaultTitleHeight + gImageWindowInfoPtr->coordinateHeight;
						#endif
						displayCode = 7;
						break;

					}	// end "switch (displaySpecsPtr->displayType)"

				}	// end "if (displaySpecsPtr->pixelSize == 8"

			else if (displaySpecsPtr->pixelSize == 16)
				{
						// Do 16 bit color displays here.

				switch (displaySpecsPtr->displayType)
					{
					case k1_ChannelGrayLevelDisplayType: // OSX only
						numberChannels = 1;
						displayCode = 12;
						break;
					/*
					case k2_ChannelDisplayType:
						numberChannels = 2;
						displayCode = 22;
						break;
					*/
					case k3_ChannelDisplayType:
					case k3_2_ChannelDisplayType:
						numberChannels = 3;
						displayCode = 32;
						break;
					
					}	// end "switch (displaySpecsPtr->displayType)"

				}	// end "else if (displaySpecsPtr->pixelSize == 16)"

			else if (displaySpecsPtr->pixelSize >= 24)
				{
						// Do 24 bit color displays here.

				switch (displaySpecsPtr->displayType)
					{
					/*
					case k2_ChannelDisplayType:
						numberChannels = 2;
						displayCode = 23;
						break;
					*/
					case k3_ChannelDisplayType:
					case k3_2_ChannelDisplayType:
						numberChannels = 3;
						displayCode = 33;
						break;
					
					}	// end "switch (displaySpecsPtr->displayType)"

            }	// end "else if (displaySpecsPtr->pixelSize >= 24)"

			if (displayCode != 0 && gImageWindowInfoPtr->localMaxNumberBytes >= 4)
				displayCode += 100;

					// Make call here for 1, 2 and 3 channels of 8, 16 or 24 bit displays.

			if (displayCode != 0)
				{
				if (displayCode == 7)
					DisplayImagesSideBySide (displaySpecsPtr,
														fileInfoPtr,
														offScreenBufferPtr,
														pixRowBytes,
														savedImageWindowPortPixMap,
														offScreenPixMapH,
														&sourceRect,
														&lcToWindowUnitsVariables);

				else if (displayCode == 107)
					Display4_8ByteImagesSideBySide (displaySpecsPtr,
																fileInfoPtr,
																histogramSpecsPtr->histogramSummaryPtr,
																offScreenBufferPtr,
																pixRowBytes,
																savedImageWindowPortPixMap,
																offScreenPixMapH,
																&sourceRect,
																&lcToWindowUnitsVariables);

				else	// the rest of the display codes.
					DisplayCImage (displaySpecsPtr,
										histogramSpecsPtr,
										fileInfoPtr,
										offScreenBufferPtr,
										pixRowBytes,
										savedImageWindowPortPixMap,
										offScreenPixMapH,
										&sourceRect,
										numberChannels,
										displayCode,
										&lcToWindowUnitsVariables);

				if (displayCode == 51 || displayCode == 151)
					loadLegendListFlag = TRUE;

				}	// end "if (displayCode != 0)"

			}	// end "if (gImageWindowInfoPtr->windowType == kImageWindowType)"

		else	// gImageWindowInfoPtr->imageType == kThematicImageType
			{
			gImageWindowInfoPtr->windowType = kThematicWindowType;
			DisplayColorThematicImage (displaySpecsPtr,
												  fileInfoPtr,
												  offScreenBufferPtr,
												  pixRowBytes,
												  savedImageWindowPortPixMap,
												  offScreenPixMapH,
												  &sourceRect,
												  &lcToWindowUnitsVariables);

			}	// end "else ...->imageType == kThematicImageType"

		}	// end "if (offScreenBufferPtr != NULL)"

	#if defined multispec_win || defined multispec_wx
		if (sourceRect.bottom != -1)
			CheckSomeDisplayEvents (gImageWindowInfoPtr,
											 displaySpecsPtr,
											 &lcToWindowUnitsVariables,
											 NULL,
											 offScreenPixMapH,
											 &sourceRect,
											 displayBottomMax);
	
		CheckAndUnlockHandle (gImageWindowInfoPtr->imageBaseAddressH);
	
		#if defined multispec_wxmac
			if (gImageWindowInfoPtr->offscreenMapSize == 0)
				EndBitMapRawDataAccess (&gActiveImageViewCPtr->m_ScaledBitmap);
		#endif // defined multispec_wxmac
	#endif // defined multispec_win || defined multispec_wx

	#if defined multispec_mac
				// Indicate that the 'better' 8-bit palette has not been created.

		displaySpecsPtr->better8BitPaletteFlag = FALSE;

		if (displaySpecsPtr->pixelSize >= 16)
			gUpdatePaletteMenuItemsFlag = TRUE;
	
				// Set PortPix map back to original	which represents the window
				// and then dispose of the memory for the saved pix map it.
				// Do not use DisposePixMap.

		CopyHandleToHandle ((Handle)savedImageWindowPortPixMap,
									(Handle)imageWindowPortPixMap);
		UnlockAndDispose ((Handle)savedImageWindowPortPixMap);

				// Copy the last few lines it they haven't been drawn yet

		if (sourceRect.bottom != -1 || gOSXCoreGraphicsFlag)
			{
			if (gOSXCoreGraphicsFlag)
				sourceRect.top = 0;
			sourceRect.bottom = displayBottomMax;
			InvalidateImageSegment (gImageWindowInfoPtr,
											 //displaySpecsPtr,
											 &lcToWindowUnitsVariables,
											 &sourceRect,
											 displayBottomMax);

			}	// end "if (sourceRect.bottom != -1 || ...)"

		if (displaySpecsPtr->pixelSize >= 16 && gQuickDrawVersion >= 0x00000230)
			::ActivatePalette (gActiveImageWindow);

				// Set global flag TRUE if off screen bip/pix map exists and
				// active image window is the front window

		if (gActiveImageWindow == FrontWindow ())
			gActiveOffscreenMapExists = TRUE;

		UpdateImageZoomControls (
				gActiveImageWindow, displaySpecsPtr->magnification, TRUE);

		UpdateCoordinateViewControls (gActiveImageWindow);
	#endif	// defined multispec_mac

	#if defined multispec_win || defined multispec_wx
		UpdateImageZoomControls (
									gActiveImageWindow, displaySpecsPtr->magnification, TRUE);
	#endif // defined multispec_win || defined multispec_wx

	if (loadLegendListFlag)
		{
				// Load the legend list.

		gPaletteOffset = displaySpecsPtr->paletteOffset;
		gClassPaletteEntries = displaySpecsPtr->numPaletteEntriesUsed;

		UpdateThematicLegendControls ((WindowPtr)gImageWindowInfoPtr->windowPtr);
		#if defined multispec_mac
			AdjustLegendListLength (gImageWindowInfoPtr->windowPtr, FALSE);
			DrawScrollBars (gImageWindowInfoPtr->windowPtr);
		#endif	// defined _multispec_mac_

				// Update palette menu and redraw the palette if it exists.

		UpdateActiveImageLegend (displaySpecsPtr->classGroupCode, kCallCreatePalette);

		}	// end "if (loadLegendListFlag)"

}	// end "DisplayColorImage"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DisplayImage
//
//	Software purpose:	The purpose of this routine is to control drawing
//							multispectral and Thematic images in the window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			OnDisplayImage in xMainFrame.cpp
//							OnEditImageDescription in xMainFrame.cpp
//							SetUpImageWindow in SOpenImage.cpp
//							SetUpThematicImageWindow in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/18/1988
//	Revised By:			Larry L. Biehl			Date: 01/10/2020

void DisplayImage ()

{
	Handle								windowInfoHandle;

	SInt16								handleStatus,
											imageType;

	Boolean								imageDisplayedFlag,
											saveProjectWindowFlag;


			// If spare memory had to be used to load code resources, then exit
			// routine.																				

	if (gMemoryTypeNeeded < 0)
																									return;

			// Code resources loaded okay, so set flag back for non-Code			
			// resources.																			

    gMemoryTypeNeeded = 0;

	if (SetUpActiveImageInformationGlobals (
							&windowInfoHandle, &handleStatus, kDoNotUseProject))
		{
		gProcessorCode = kDisplayProcessor;

				// Inialize local variables.

		imageType = gImageWindowInfoPtr->imageType;

				// Save 'projectWindowFlag'.  Turn it off for now so 		
				// that fields boundaries will not be drawn while the 	
				// image is being drawn.											

		saveProjectWindowFlag = gImageWindowInfoPtr->projectWindowFlag;
		gImageWindowInfoPtr->projectWindowFlag = FALSE;

				// Do not draw any vector overlays until after the entire image
				// has been loaded.
				// However, if the display dialog box is not to be called allow
				// the vectors to be drawn. This could be from a next channel
				// key event. Do not turn off or the image will flash

		if (gCallProcessorDialogFlag)
			gImageWindowInfoPtr->drawVectorOverlaysFlag = FALSE;
		
				// Make sure gOperationCanceledFlag is set to false at the
				// beginning.
		
		gOperationCanceledFlag = FALSE;

				// Call the appropriate image display routine.							
        
		switch (imageType)
			{
			case kMultispectralImageType:
				gMemoryTypeNeeded = 1;
				imageDisplayedFlag = DisplayMultispectralImage ();
				break;

			case kThematicImageType:
				gMemoryTypeNeeded = 1;
				imageDisplayedFlag = DisplayThematicImage ();
				break;

			}	// end "switch (imageType)" 

					// If image was displayed make sure that the last part of the
					// image is updated in the window.

		gImageWindowInfoPtr->drawVectorOverlaysFlag = TRUE;
		gImageWindowInfoPtr->projectWindowFlag = saveProjectWindowFlag;
		if (imageDisplayedFlag) 
			{
					// Load overlays if needed.

			if (GetIncludeVectorOverlaysFlag (gImageWindowInfoPtr))
				AddCurrentVectorOverlaysToImageWindow (gImageWindowInfoPtr);

			CheckSomeEvents (osMask + updateMask);

			}	// end "if (imageDisplayedFlag)"
		
		#if defined multispec_wx
			Handle displaySpecsHandle = GetActiveDisplaySpecsHandle ();
      	DisplaySpecsPtr displaySpecsPtr =
									(DisplaySpecsPtr)GetHandlePointer (displaySpecsHandle);
			if (displaySpecsPtr != NULL)
				{
				displaySpecsPtr->updateStartLine = -1;
				displaySpecsPtr->updateEndLine = -1;
				
				}	// end "if (displaySpecsPtr != NULL)"
		#endif

		if (imageDisplayedFlag) 
			{
					// Load overlays if needed.

			//if (GetIncludeVectorOverlaysFlag (gImageWindowInfoPtr))
			//	AddCurrentVectorOverlaysToImageWindow (gImageWindowInfoPtr);

			#if defined multispec_mac
						// Draw the overlays if needed.

				if (gCallProcessorDialogFlag && gImageWindowInfoPtr->numberVectorOverlays > 0)
						// Need to force the vectors to be drawn for this case.
					DrawArcViewShapes (gActiveImageWindow,
											  gActiveImageWindowInfoH,
											  NULL,
											  kToImageWindow,
											  NULL);

						// Draw the field boundaries if needed.							

            //if (saveProjectWindowFlag)
            //	OutlineFieldsControl (gProjectInfoPtr->statsWindowMode, 
            //									gActiveImageWindow, 
            //									gActiveImageWindowInfoH,
            //									kToImageWindow);
			#endif	// defined multispec_mac

			#if defined multispec_win || defined multispec_wx
            if (gImageWindowInfoPtr->numberVectorOverlays > 0 || saveProjectWindowFlag)
                InvalidateWindow (gActiveImageWindow, kImageFrameArea, FALSE);
			#endif	// defined multispec_win || defined multispec_wx

			//if (imageDisplayedFlag)
			//	CheckSomeEvents (osMask+updateMask);

			}	// end "if (imageDisplayedFlag)"

		gMemoryTypeNeeded = 0;
		gProcessorCode = 0;
		gOperationCanceledFlag = FALSE;

		}	// end "SetUpActiveImageInformationGlobals (..." 

	UnlockActiveImageInformationGlobals (handleStatus);

}	// end "DisplayImage" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ForceDisplayCodeResourceLoad
//
//	Software purpose:	The purpose of this is to force the 'Display' code
//							resource to be loaded into memory.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/05/1990
//	Revised By:			Larry L. Biehl			Date: 04/06/1990

void ForceDisplayCodeResourceLoad (void)

{
	// If spare memory had to be used to load code resources, then exit routine.

	if (gMemoryTypeNeeded < 0)
																									return;

			// Code resources loaded okay, so set flag back for non-Code resources.

	gMemoryTypeNeeded = 0;

}	// end "ForceDisplayCodeResourceLoad"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetDefaultImagePixelSize
//
//	Software purpose:	This routine gets the default pixel size (or depth) to
//							be used for an image window. The default pixel size
//							will always be 8, 16, or 24.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			InitializeDisplaySpecsStructure in SDisplay.cpp
//							GetDefaultPaletteSpecs in SDisplayMultispectral.cpp
//
//	Coded By:			Larry L. Biehl			Date:  9/23/1993
//	Revised By:			Larry L. Biehl			Date: 11/02/2001			

SInt16 GetDefaultImagePixelSize (
				SInt16								maxSystemPixelSize)

{
			// Local Declarations

	SInt16 pixelSize = 8;


	if (maxSystemPixelSize > 0 && maxSystemPixelSize <= 32)
		pixelSize = maxSystemPixelSize;

	else	// maxSystemPixelSize <= 0 || maxSystemPixelSize > 32 
		pixelSize = GetMaxSystemPixelSize ();

			// For pixel size to be either 8, 16, or 32.

	//if (pixelSize > 1 && pixelSize <= 8)
	if (pixelSize <= 8)
		pixelSize = 8;

	return (pixelSize);

}	// end "GetDefaultImagePixelSize"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetIncludeVectorOverlaysFlag
//
//	Software purpose:	The purpose of this routine is to get the includeVectorOverlaysFlag
//							that is stored in the display specification structure for the
//							input window information structure.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/08/2006
//	Revised By:			Larry L. Biehl			Date: 12/13/2006	

Boolean GetIncludeVectorOverlaysFlag (
				WindowInfoPtr						windowInfoPtr)

{
	DisplaySpecsPtr					displaySpecsPtr;

	Handle								displaySpecsHandle;

	Boolean								includeVectorOverlaysFlag = FALSE;


	displaySpecsHandle = GetDisplaySpecsHandle (windowInfoPtr);
	displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (displaySpecsHandle);

	if (displaySpecsPtr != NULL)
		includeVectorOverlaysFlag = displaySpecsPtr->includeVectorOverlaysFlag;

	return (includeVectorOverlaysFlag);

}	// end "GetIncludeVectorOverlaysFlag"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		UInt32 GetNumberPixRowBytes
//
//	Software purpose:	The purpose of this routine is to determine the number
//							of pixel row bytes that are required for the given
//							number of columns and pixel color size.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 06/09/1995
//	Revised By:			Larry L. Biehl			Date: 11/02/2001	

UInt32 GetNumberPixRowBytes (
				UInt32								numberColumns,
				SInt16								pixelSize)

{
	#if defined multispec_mac || defined multispec_mac_swift
				// Based on tests run on 11/2/2001, NewGWorld computes pix row bytes
				// that are multiples of 16 bytes. Also there appears to be something
				// added such that for example 13 columns of 8 bits is bumped up to 
				// row bytes of 32 not 16 as one would expect. This was found on a PPC
				// running OS8.6 and OS9.1.		
		UInt32 numberRowBytes =
						((((UInt32)pixelSize * numberColumns + 127 + 32) >> 7) << 4);
	#endif	// defined multispec_mac || defined multispec_mac_swift

	#if defined multispec_win
		UInt32 numberRowBytes =
						((((UInt32) pixelSize * numberColumns + 31) >> 5) << 2);
	#endif	// defined multispec_win

	#if defined multispec_wx
		/*UInt32 numberRowBytes =
		((((UInt32)pixelSize * numberColumns + 31) >> 5) << 2);*/
		int pixelbytes = (int)MAX (3, pixelSize / 8);
		UInt32 numberRowBytes = (UInt32)pixelbytes * numberColumns;
	#endif
					
	return (numberRowBytes);

}	// end "GetNumberPixRowBytes"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		DisplaySpecsPtr GetActiveDisplaySpecsPtr
//
//	Software purpose:	This routine gets the pointer to the display
//							specifications structure.
//
//	Parameters in:		Pointer to the window information structure			
//
//	Parameters out:	None				
//
// Value Returned:	Pointer to the display specifications structure	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 11/09/1995
//	Revised By:			Larry L. Biehl			Date: 11/12/1995

DisplaySpecsPtr GetActiveDisplaySpecsPtr (void)

{
	DisplaySpecsPtr					displaySpecsPtr = NULL;


	#if defined multispec_mac
		if (gImageWindowInfoPtr != NULL)
			displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
														gImageWindowInfoPtr->displaySpecsHandle);
	#endif	// defined multispec_mac

	#if defined multispec_win
		if (gActiveImageViewCPtr != NULL)
			displaySpecsPtr =
						gActiveImageViewCPtr->m_displayMultiCPtr->GetDisplaySpecsPtr ();
	#endif	// defined multispec_win

	#if defined multispec_wx
		if (gActiveImageViewCPtr != NULL)
			displaySpecsPtr =
							gActiveImageViewCPtr->m_displayMultiCPtr->GetDisplaySpecsPtr ();
	#endif	// defined multispec_wx

	return (displaySpecsPtr);

}	// end "GetActiveDisplaySpecsPtr"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Handle GetDisplaySpecsStructure
//
//	Software purpose:	The purpose of this routine is to return the handle for
//							the display specification structure.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 12/06/1991
//	Revised By:			Larry L. Biehl			Date: 11/03/1995	

Handle GetDisplaySpecsStructure (
				Handle								displaySpecsHandle,
				Boolean								initializeFlag)

{
	DisplaySpecsPtr					displaySpecsPtr = NULL;

	Boolean								newHandleFlag = FALSE;
	SignedByte							handleStatus;


	if (displaySpecsHandle == NULL)
		{
		displaySpecsHandle = MNewHandle (sizeof (DisplaySpecs));
		newHandleFlag = TRUE;

		}	// end "if (displaySpecsHandle == NULL)"

	if (displaySpecsHandle != NULL)
		{
		displaySpecsPtr = (DisplaySpecsPtr)
					GetHandleStatusAndPointer (displaySpecsHandle, &handleStatus, kMoveHi);

		if (newHandleFlag)
			{
			displaySpecsPtr->initializeStructureFlag = TRUE;
			displaySpecsPtr->channelsHandle = NULL;

			}	// end "if (newHandleFlag)"

		if (initializeFlag)
			InitializeDisplaySpecsStructure (displaySpecsPtr);

		MHSetState (displaySpecsHandle, handleStatus);

		}	// end "if (displaySpecsHandle != NULL)"

	return (displaySpecsHandle);

}	// end "GetDisplaySpecsStructure"


#if defined multispec_mac || defined multispec_win
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetOffscreenGWorld
//
//	Software purpose:	The purpose of this routine is to set up an offscreen
//							pix map for those systems which have 32 bit
//							QuickDraw available.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			DisplayColorImage
//
//	Coded By:			Larry L. Biehl			Date: 05/07/1991
//	Revised By:			Larry L. Biehl			Date: 11/12/2019

SInt16 GetOffscreenGWorld (
				WindowInfoPtr						windowInfoPtr,
				DisplaySpecsPtr					displaySpecsPtr,
				LongRect*							longRectPtr,
				UInt32*								pixRowBytesPtr)

{
	SInt16								resultCode = 0;
	
	#if defined multispec_mac
		Rect									rect;

		CGrafPtr								savedPort;
		CTabPtr								cTabPtr;
		GWorldPtr							offscreenGWorld;

		CTabHandle							tempCTableHandle;
		GDHandle								savedDevice;
		Handle								tempHandle;
		PixMapHandle						offScreenPixMapH;

		UInt32								freeBytesNeeded;

		SInt16								numberEntries;


				// Initialize local variables.

		resultCode = noErr;

				// Save current graphics world.

		GetGWorld (&savedPort, &savedDevice);

		tempCTableHandle = NULL;
		if (displaySpecsPtr->paletteObject != NULL)
			{
					// Get a temporary color table handle.

			numberEntries = (*(displaySpecsPtr->paletteObject))->pmEntries;
			tempCTableHandle = (CTabHandle)MNewHandle (
					 (SInt32)numberEntries * sizeof (ColorSpec) + sizeof (ColorTable));
			if (tempCTableHandle == NULL)
																								return (1);

					// Now initialize the table with the palette entries.

			Palette2CTab (displaySpecsPtr->paletteObject, tempCTableHandle);
			cTabPtr = *tempCTableHandle;
			cTabPtr->ctSeed = GetCTSeed ();
			cTabPtr->ctFlags |= 0x4000;

			}	// end "if (displaySpecsPtr->paletteObject != NULL)"

				// Dispose of old GWorld if it exists and get a new one.

		ReleaseOffscreenSupportMemory (windowInfoPtr);

		offscreenGWorld = windowInfoPtr->offscreenGWorld;
		offScreenPixMapH = (PixMapHandle)windowInfoPtr->offScreenMapHandle;
		/*			
		if (offscreenGWorld != NULL)
			{
			UnlockPixels (offScreenPixMapH);
			AllowPurgePixels (offScreenPixMapH);
			DisposeGWorld (offscreenGWorld);
			offscreenGWorld = NULL;
			windowInfoPtr->offscreenGWorld = NULL;
			windowInfoPtr->offScreenMapHandle = NULL;
			windowInfoPtr->offscreenMapSize = 0;

			}	// end "if (offscreenGWorld != NULL)" 

		if (gOSXCoreGraphicsFlag)
			{
			UnlockAndDispose (windowInfoPtr->imageBaseAddressH);

			}	// end "if (gOSXCoreGraphicsFlag)"
		*/
				// Make certain that the number of pixel row bytes is less than the
				// maximum allowed.

		*pixRowBytesPtr = GetNumberPixRowBytes (
											(UInt32)longRectPtr->right,
											displaySpecsPtr->pixelSize);

		if (*pixRowBytesPtr > gMaxRowBytes)
			resultCode = 1;

		freeBytesNeeded = *pixRowBytesPtr * (UInt32)longRectPtr->bottom;

		RGBColor theColor;

		UInt32 index,
				 tableIndex;

		Boolean indexedColorSpaceFlag;


		windowInfoPtr->imageBaseAddressH = MNewHandle (freeBytesNeeded);

		if (windowInfoPtr->imageBaseAddressH == NULL)
			resultCode = 1;

				// Get storage for save a portPixMap in.

		if (offScreenPixMapH == NULL)
			offScreenPixMapH = NewPixMap ();

		if (offScreenPixMapH == NULL)
			resultCode = 1;

		if (resultCode == noErr)
			{
			unsigned char tablePtr[3 * 256];

			CGColorSpaceRef baseColorSpace = NULL,
					  colorSpace = NULL;

			CGDataProviderRef provider = NULL;
			CGImageAlphaInfo alphaInfo;
			Ptr offScreenBufferPtr = NULL;
			size_t bitsPerComponent;

			windowInfoPtr->offScreenMapHandle = (Handle)offScreenPixMapH;
			windowInfoPtr->offscreenMapSize = freeBytesNeeded;

			windowInfoPtr->cgInfo.contextRowBytes = *pixRowBytesPtr;

					// Lock the handle for the offscreen bit map and keep it locked.

			offScreenBufferPtr = GetHandlePointer (
												  windowInfoPtr->imageBaseAddressH,
												  kLock);

					// Note that the image base address handle is already locked.

			provider = gCGDataProviderCreateWithDataPtr (
												  NULL,
												  offScreenBufferPtr,
												  freeBytesNeeded,
												  NULL);

					// Create the colorspace.

			indexedColorSpaceFlag = FALSE;
			if (displaySpecsPtr->pixelSize == 8 &&
					  (displaySpecsPtr->displayType == k1_ChannelGrayLevelDisplayType ||
					  displaySpecsPtr->displayType == k3_ChannelDisplayType ||
					  //displaySpecsPtr->displayType == k2_ChannelDisplayType ||
					  displaySpecsPtr->displayType == k3_2_ChannelDisplayType))
				 indexedColorSpaceFlag = TRUE;

			if (windowInfoPtr->windowType == kThematicWindowType)
				indexedColorSpaceFlag = TRUE;

			if (!indexedColorSpaceFlag)
				{
				if (displaySpecsPtr->pixelSize == 8)
					baseColorSpace = gCGColorSpaceCreateDeviceGrayPtr ();

				else	// displaySpecsPtr->pixelSize > 8
					baseColorSpace = gCGColorSpaceCreateDeviceRGBPtr ();

				colorSpace = baseColorSpace;

				}	// end "if (!indexedColorSpaceFlag)"

			else	// indexedColorSpaceFlag
				{
						// Create an indexed color space for a thematic window.

				baseColorSpace = gCGColorSpaceCreateDeviceRGBPtr ();

				numberEntries =
							MGetNumberPaletteEntries (displaySpecsPtr->paletteObject);

				//numberEntries = 256;
				tableIndex = 0;
				for (index = 0; index < numberEntries; index++)
					{
					::GetEntryColor (displaySpecsPtr->paletteObject,
							 index,
							 &theColor);

					tablePtr[tableIndex] = (theColor.red >> 8);
					tablePtr[tableIndex + 1] = (theColor.green >> 8);
					tablePtr[tableIndex + 2] = (theColor.blue >> 8);

					tableIndex += 3;

					}	// end "for (index=0; index<numberEntries; index++)"

				colorSpace = gCGColorSpaceCreateIndexedPtr (
																baseColorSpace,
																numberEntries - 1,
																tablePtr);

				if (colorSpace == NULL)
																								return (1);

				}	// end "else indexedColorSpaceFlag"

					// Create the image.

			alphaInfo = kCGImageAlphaNoneSkipFirst;
			bitsPerComponent = 8;
			if (displaySpecsPtr->pixelSize == 16)
				{
				 bitsPerComponent = 5;
				 //alphaInfo = kCGBitmapByteOrder16Big;

				}	// end "if (displaySpecsPtr->pixelSize == 16)"

			windowInfoPtr->cgInfo.imageRef = gCGImageCreatePtr (
															  longRectPtr->right,
															  longRectPtr->bottom,
															  bitsPerComponent,
															  displaySpecsPtr->pixelSize,
															  windowInfoPtr->cgInfo.contextRowBytes,
															  colorSpace,
															  alphaInfo,
															  provider,
															  NULL,
															  1,
															  kCGRenderingIntentDefault);

					// Release the references to the provider and the colorspace. Copies
					// are retained by cgImage.

			gCGDataProviderReleasePtr (provider);

			gCGColorSpaceReleasePtr (baseColorSpace);

			if (indexedColorSpaceFlag)
				gCGColorSpaceReleasePtr (colorSpace);

			(*offScreenPixMapH)->pixelSize = displaySpecsPtr->pixelSize;

			}	// end "if (resultCode == noErr)" 
		/*
		if (!gOSXCoreGraphicsFlag)
			{
			rect.left = longRectPtr->left; //  - 32767;
			rect.top = longRectPtr->top; // - 32767;
			rect.right = longRectPtr->right; // - 32767;
			rect.bottom = longRectPtr->bottom; // - 32767;

					// Verify that there is enough memory available for the offscreen
					// GWorld.

			tempHandle = MNewHandle (freeBytesNeeded + 10000);
			if (tempHandle == NULL)
				resultCode = 1;

			else	// tempHandle != NULL 
				UnlockAndDispose (tempHandle);

			if (resultCode == noErr)
				resultCode = NewGWorld (&offscreenGWorld,
												 displaySpecsPtr->pixelSize,
												 &rect,
												 tempCTableHandle,
												 NULL,
												 (SInt32)0);

			if (resultCode == noErr)
				{
				windowInfoPtr->offscreenGWorld = offscreenGWorld;
				windowInfoPtr->offScreenMapHandle = (Handle)GetPortPixMap (offscreenGWorld);

						// Get the actual size of the image buffer, in case that pix row
						// bytes is not exactly as computed above.
						// Make sure that the first two bits are cleared. They are not used
						// for the number of row bytes.

				*pixRowBytesPtr = MGetPixRowBytes (GetPortPixMap (offscreenGWorld));
				freeBytesNeeded = *pixRowBytesPtr * longRectPtr->bottom;

				windowInfoPtr->offscreenMapSize = freeBytesNeeded;

						// Lock down the pixels before drawing image into the window.
						// Draw only if offscreen buffer is not purged.							

				SetGWorld (offscreenGWorld, NULL);

				if (LockPixels (GetPortPixMap (offscreenGWorld)))
					EraseRect (&rect);

				else	// !LockPixels (offscreenGWorld->portPixMap) 
					resultCode = 1;

				}	// end "if (resultCode == noErr)"

			}	// end "else !gOSXCoreGraphicsFlag"
		*/
		SetGWorld (savedPort, savedDevice);

		UnlockAndDispose ((Handle)tempCTableHandle);
	#endif	// defined multispec_mac

	#if defined multispec_win 
		BITMAPINFO*							bitMapInfoPtr;
		BITMAPINFOHEADER*					bitMapInfoHeadPtr;

		UInt32								bytesNeeded,
												maxNumberColumns,
												numberColumns;

		SInt16								numberEntries;

				// Verify that the number of columns to be displayed is less
				// than the maximum allowed.
				// Note that gMaxRowBytes & gMaxRowBytesFor24Bits really represent 
				// columns not bytes.

		numberColumns = (UInt32)longRectPtr->right - longRectPtr->left;

		maxNumberColumns = gMaxRowBytes;
		if (displaySpecsPtr->pixelSize > 8)
		  maxNumberColumns = gMaxRowBytesFor24Bits;

		if (numberColumns > maxNumberColumns)
																								return (1);

				// Make certain that the number of pixel row bytes is less than the	
				// maximum allowed.							

		*pixRowBytesPtr = GetNumberPixRowBytes (
											(UInt32)longRectPtr->right - longRectPtr->left,
											displaySpecsPtr->pixelSize);

				// Now get memory for the offscreen bit map and header. 

		resultCode = noErr;
		numberEntries = displaySpecsPtr->paletteObject->GetNumberPaletteEntries ();
		if (displaySpecsPtr->pixelSize == 24)
		  numberEntries = 0;

				// Check if the number of bytes needed for the new BITMAP
				// is the same as that in the current BITMAP; if not release the
				// old block of memory and get a new one.
				// Change on 2/16/2000 so that the old BITMAP was always cleared out.

		bytesNeeded =
				*pixRowBytesPtr * ((UInt32)longRectPtr->bottom - longRectPtr->top + 1);
		UnlockAndDispose (windowInfoPtr->imageBaseAddressH);
		windowInfoPtr->imageBaseAddressH = MNewHandle (bytesNeeded);
		windowInfoPtr->offscreenMapSize = bytesNeeded;

		if (windowInfoPtr->imageBaseAddressH == NULL)
		  resultCode = 1;

				// Dispose of old BITMAPINFOHEADER if it exists and get a new one.	

		windowInfoPtr->offScreenMapHandle =
				UnlockAndDispose (windowInfoPtr->offScreenMapHandle);

		if (resultCode == noErr) 
			{
					// Get BITMAPINFOHEADER
					// Space is always allowed for a 256 entry table.

			//bytesNeeded = sizeof (BITMAPINFOHEADER) + 256*sizeof (RGBQUAD);
			bytesNeeded = sizeof (BITMAPINFOHEADER) + 256 * sizeof (UInt16);
			windowInfoPtr->offScreenMapHandle = MNewHandle (bytesNeeded);

			gActiveImageViewCPtr->SetLegendBitMapInfoHeaderHandle (
					 windowInfoPtr->offScreenMapHandle);

			if (windowInfoPtr->offScreenMapHandle == NULL)
				resultCode = 1;

			}	// end "if (resultCode == noErr)"

		if (resultCode == noErr) 
			{
					// Load info in BITMAPINFOHEADER

			bitMapInfoHeadPtr = (BITMAPINFOHEADER*)GetHandlePointer (
													windowInfoPtr->offScreenMapHandle, kLock);

			bitMapInfoHeadPtr->biSize = 40;
			bitMapInfoHeadPtr->biWidth = longRectPtr->right - longRectPtr->left;
			bitMapInfoHeadPtr->biHeight = longRectPtr->bottom - longRectPtr->top;
			bitMapInfoHeadPtr->biPlanes = 1;
			bitMapInfoHeadPtr->biBitCount = displaySpecsPtr->pixelSize;
			bitMapInfoHeadPtr->biCompression = BI_RGB;
			bitMapInfoHeadPtr->biSizeImage = 0;
			bitMapInfoHeadPtr->biXPelsPerMeter = 0;
			bitMapInfoHeadPtr->biYPelsPerMeter = 0;
			bitMapInfoHeadPtr->biClrUsed = numberEntries; // numberEntries	
			bitMapInfoHeadPtr->biClrImportant = 0; // numberEntries	

			if (displaySpecsPtr->pixelSize == 8) 
				{
				bitMapInfoPtr = (BITMAPINFO*)bitMapInfoHeadPtr;
				if (!displaySpecsPtr->paletteObject->LoadRGBQUADStructure (
						  &bitMapInfoPtr->bmiColors[0], DIB_PAL_COLORS))
					 resultCode = 1;

				}	// end "if (displaySpecsPtr->pixelSize == 8)"

			CheckAndUnlockHandle (windowInfoPtr->offScreenMapHandle);

			}	// end "if (resultCode == noErr)" 

		if (resultCode != noErr) 
			{
			windowInfoPtr->offScreenMapHandle =
					 UnlockAndDispose (windowInfoPtr->offScreenMapHandle);

			windowInfoPtr->imageBaseAddressH =
					 UnlockAndDispose (windowInfoPtr->imageBaseAddressH);

			windowInfoPtr->offscreenMapSize = 0;

			}	// end "if (resultCode != noErr)"
	#endif	// defined multispec_win
	
	return (resultCode);

}	// end "GetOffscreenGWorld" 
#endif


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void InitializeDisplaySpecsStructure
//
//	Software purpose:	The purpose of this routine is to initialize memory for
//							display specification structure.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 12/06/1991
//	Revised By:			Larry L. Biehl			Date: 01/10/2020

void InitializeDisplaySpecsStructure (
				DisplaySpecsPtr					displaySpecsPtr)

{
	if (displaySpecsPtr != NULL && displaySpecsPtr->initializeStructureFlag) 
		{
		if (!CheckIfOffscreenImageExists (gImageWindowInfoPtr)) 
			{
					// Offscreen image does not exist. Initialize the offscreen
					// image variables.

			displaySpecsPtr->paletteObject = NULL;

			#if defined multispec_win || defined multispec_wx
				displaySpecsPtr->backgroundPaletteObject = NULL;
			#endif	// defined multispec_win || defined multispec_wx
			
			#if defined multispec_wx
						// A value of -1 indicates to handle all during a
						// thematic initial display update event
				displaySpecsPtr->updateStartLine = -1;
				displaySpecsPtr->updateEndLine = -1;
			#endif	// defined multispec_wx

			displaySpecsPtr->displayedLineStart = 0;
			displaySpecsPtr->displayedLineEnd = 0;
			displaySpecsPtr->displayedLineInterval = 1;
			displaySpecsPtr->displayedColumnStart = 0;
			displaySpecsPtr->displayedColumnEnd = 0;
			displaySpecsPtr->displayedColumnInterval = 1;

			displaySpecsPtr->displayedNumberChannels = 0;

			//displaySpecsPtr->displayHPixelsPerImagePixel = 1;
			//displaySpecsPtr->displayVPixelsPerImagePixel = 1; 

			displaySpecsPtr->gaussianStretch = 2.3;

			displaySpecsPtr->magnification = 1.;

			displaySpecsPtr->origin[kVertical] = 0;
			displaySpecsPtr->origin[kHorizontal] = 0;
			displaySpecsPtr->imageDimensions[kVertical] = 0;
			displaySpecsPtr->imageDimensions[kHorizontal] = 0;
			displaySpecsPtr->offscreenChannelWidth = 0;

			displaySpecsPtr->displayType = kSideSideChannelDisplayType;

			}	// end "if (!CheckIfOffscreenImageExists (..." 

		displaySpecsPtr->backgroundDataValue = 0;
		displaySpecsPtr->channelNumber = 1;
		displaySpecsPtr->redChannelNumber = 1;
		displaySpecsPtr->greenChannelNumber = 1;
		displaySpecsPtr->blueChannelNumber = 1;
		displaySpecsPtr->displaySet = 0;
		displaySpecsPtr->enhancementCode = 0;
		displaySpecsPtr->minMaxCode = kPerChannelMinMax;
		displaySpecsPtr->histogramCompute = 1;
		displaySpecsPtr->defaultHistogramFileFlag = FALSE;
		displaySpecsPtr->drawSideBySideTitleFlag = FALSE;
		displaySpecsPtr->firstTime = TRUE;
		displaySpecsPtr->dataToDisplayCompute = TRUE;
		displaySpecsPtr->backgroundValueCode = 0;
		displaySpecsPtr->maxMagnification = 99;
		displaySpecsPtr->scale = -1.;
		displaySpecsPtr->pixelSize = GetDefaultImagePixelSize (0);
		displaySpecsPtr->lastColorPixelSize = displaySpecsPtr->pixelSize;
		displaySpecsPtr->savedClassCTableHandle = NULL;
		displaySpecsPtr->savedGroupCTableHandle = NULL;
		displaySpecsPtr->filePaletteFlag = FALSE;
		displaySpecsPtr->thematicClassPaletteType = 0;
		displaySpecsPtr->thematicGroupPaletteType = 0;
		displaySpecsPtr->blinkProcedure = gBlinkProcedure;
		displaySpecsPtr->symbolToPaletteEntryH = NULL;
		displaySpecsPtr->classGroupCode = 0;
		displaySpecsPtr->classToGroupChangeFlag = FALSE;
		displaySpecsPtr->localNumberGroups = 0;
		displaySpecsPtr->localGroupNameHandle = NULL;
		displaySpecsPtr->allSubsetClassSet = 1;
		displaySpecsPtr->allSubsetGroupSet = 1;
		displaySpecsPtr->numberDisplayClasses = 0;
		displaySpecsPtr->numberDisplayGroups = 0;
		displaySpecsPtr->displayClassGroupsHandle = NULL;

		#if defined multispec_mac || defined multispec_win
			displaySpecsPtr->backgroundColor.red = 0xff00;
			displaySpecsPtr->backgroundColor.green = 0xff00;
			displaySpecsPtr->backgroundColor.blue = 0xff00;
			displaySpecsPtr->editedBackgroundColor.red = 0xff00;
			displaySpecsPtr->editedBackgroundColor.green = 0xff00;
			displaySpecsPtr->editedBackgroundColor.blue = 0xff00;
		#endif	// defined multispec_mac || defined multispec_win

		#if defined multispec_wx
			displaySpecsPtr->backgroundColor.red = 0x00ff;
			displaySpecsPtr->backgroundColor.green = 0x00ff;
			displaySpecsPtr->backgroundColor.blue = 0x00ff;
			displaySpecsPtr->editedBackgroundColor.red = 0x00ff;
			displaySpecsPtr->editedBackgroundColor.green = 0x00ff;
			displaySpecsPtr->editedBackgroundColor.blue = 0x00ff;
		#endif	// defined multispec_wx

		displaySpecsPtr->thresholdFileFlag = FALSE;
		displaySpecsPtr->numPaletteEntriesUsed = 0;
		displaySpecsPtr->color = 1;
		displaySpecsPtr->numberLevels = 256;
		displaySpecsPtr->numberPaletteLevels = 6;
		displaySpecsPtr->rgbColors = 8;
		displaySpecsPtr->better8BitPaletteFlag = FALSE;
		displaySpecsPtr->imageWindowIsUpToDateFlag = TRUE;
		displaySpecsPtr->includeVectorOverlaysFlag = TRUE;
		displaySpecsPtr->invertValues = FALSE;
		displaySpecsPtr->invertValuesFlag[0] = FALSE;
		displaySpecsPtr->invertValuesFlag[1] = FALSE;
		displaySpecsPtr->invertValuesFlag[2] = FALSE;
		displaySpecsPtr->paletteUpToDateFlag = FALSE;
		displaySpecsPtr->readPaletteFromDiskFlag = FALSE;
		displaySpecsPtr->vectorOverlaysExistFlag = FALSE;
		////displaySpecsPtr->paletteOffset = 1;
		displaySpecsPtr->paletteOffset = 0;
		displaySpecsPtr->percentTailsClipped = 2;

		if (gImageWindowInfoPtr != NULL) 
			{
					// Make sure that the pixel size setting reflects the max possible
					// relative to the number of channels. If the number of channels
					// is only 1, then the default pixel size will be 8.

			if (gImageWindowInfoPtr->totalNumberChannels == 1) 
				{
				if (gOSXFlag && !gOSXCoreGraphicsFlag)
					displaySpecsPtr->pixelSize = 16;

				else	// !gOSXFlag || gOSXCoreGraphicsFlag
					displaySpecsPtr->pixelSize = 8;

            }	// end "if (...->totalNumberChannels == 1)"

			if (displaySpecsPtr->channelsHandle == NULL) 
				{
				displaySpecsPtr->numberChannels =
                        gImageWindowInfoPtr->totalNumberChannels;
				displaySpecsPtr->channelSet = kAllMenuItem;

            }	// end "if (displaySpecsPtr->channelsHandle == NULL && ...)"

			displaySpecsPtr->lineStart = 1;
			displaySpecsPtr->lineEnd = gImageWindowInfoPtr->maxNumberLines;

			displaySpecsPtr->columnStart = 1;
			displaySpecsPtr->columnEnd = gImageWindowInfoPtr->maxNumberColumns;

					// Set the intervals so that the first and last line and column 	
					// can be displayed with a one to one scale.								

			displaySpecsPtr->lineInterval =
					  (displaySpecsPtr->lineEnd - 1) / kMaxNumberDisplayLines + 1;

			UInt32 maxNumberColumns =
					  (((gMaxRowBytes >> 3) << 6) - 63) / displaySpecsPtr->pixelSize;
			displaySpecsPtr->columnInterval =
					  (displaySpecsPtr->columnEnd - 1) / maxNumberColumns + 1;

			if (displaySpecsPtr->lineInterval > displaySpecsPtr->columnInterval)
				displaySpecsPtr->columnInterval = displaySpecsPtr->lineInterval;

			else	// ...->lineInterval <= ...->columnInterval 
				displaySpecsPtr->lineInterval = displaySpecsPtr->columnInterval;

			displaySpecsPtr->initializeStructureFlag = FALSE;

			}	// end "if (gImageWindowInfoPtr != NULL)"

		}	// end "if (displaySpecsPtr != NULL && ...)"

}	// end "InitializeDisplaySpecsStructure"					



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void InitializeClassGroupsVector
//
//	Software purpose:	The purpose of this routine is to copy the last portion 
//							of the offscreen image to the active image window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 09/28/2006
//	Revised By:			Larry L. Biehl			Date: 09/28/2006	

SInt16 InitializeClassGroupsVector (
				FileInfoPtr							fileInfoPtr,
				DisplaySpecsPtr					displaySpecsPtr)

{
	SInt32								currentNumberBytes,
											numberBytes;


	numberBytes = (SInt32)fileInfoPtr->numberClasses * sizeof (SInt16);

	currentNumberBytes = MGetHandleSize (displaySpecsPtr->displayClassGroupsHandle);

	if (currentNumberBytes != numberBytes) 
		{
		UnlockAndDispose (displaySpecsPtr->displayClassGroupsHandle);

		displaySpecsPtr->displayClassGroupsHandle = MNewHandleClear (numberBytes);

		}	// end "if (currentNumberBytes != numberBytes)"

	if (displaySpecsPtr->displayClassGroupsHandle == NULL)
		return (1);

			// Indicate that all classes are to be displayed.						

	LoadClassGroupVector (&displaySpecsPtr->numberDisplayClasses,
									displaySpecsPtr->displayClassGroupsHandle,
									fileInfoPtr->numberClasses,
									fileInfoPtr->numberGroups,
									kClassDisplay);

			// Indicate that all groups are to be displayed.						

	LoadClassGroupVector (&displaySpecsPtr->numberDisplayGroups,
									displaySpecsPtr->displayClassGroupsHandle,
									fileInfoPtr->numberClasses,
									fileInfoPtr->numberGroups,
									kGroupDisplay);

	return (0);

}	// end "InitializeClassGroupsVector"				



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void InvalidateImageSegment
//
//	Software purpose:	The purpose of this routine is to copy the last portion 
//							of the offscreen image to the active image window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			CheckSomeDisplayEvents in SDisplay.cpp
//							DisplayColorImage in SDisplay.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/02/1994
//	Revised By:			Larry L. Biehl			Date: 06/23/2020

void InvalidateImageSegment (
				WindowInfoPtr						windowInfoPtr,
				//DisplaySpecsPtr					displaySpecsPtr,
				LCToWindowUnitsVariables* 		lcToWindowUnitsVariablesPtr,
				LongRect*							sourceRectPtr,
				SInt32								displayBottomMax)

{
	Rect									destinationRect;
	
	int									columnInterval,
											columnOffset,
											lineInterval,
											lineOffset;


			// Get the destination rectangle for the input source.

	double magnification = lcToWindowUnitsVariablesPtr->magnification;
	double hOrigin = lcToWindowUnitsVariablesPtr->xOrigin;
	double vOrigin = lcToWindowUnitsVariablesPtr->yOrigin;
	SInt32 legendWidth = windowInfoPtr->legendWidth;

	columnInterval = lineInterval = 1;
	columnOffset = lineOffset = 0;
	if (gProcessorCode != kDisplayProcessor)
		{
		columnInterval = lcToWindowUnitsVariablesPtr->columnInterval;
		lineInterval = lcToWindowUnitsVariablesPtr->lineInterval;
		
		columnOffset = lcToWindowUnitsVariablesPtr->columnOffset;
		lineOffset = lcToWindowUnitsVariablesPtr->lineOffset;
		
		}	// end "if (gProcessorCode != kDisplayProcessor)"

	destinationRect.top = (int)(magnification *
		((sourceRectPtr->top+lineOffset)/lineInterval - vOrigin));
	destinationRect.bottom = (int)(magnification *
		((sourceRectPtr->bottom+lineOffset)/lineInterval - vOrigin) + .9999);
						
	#if defined multispec_mac
		destinationRect.top += windowInfoPtr->imageTopOffset;
		destinationRect.bottom += windowInfoPtr->imageTopOffset;
	#endif

	#if defined multispec_win
		destinationRect.top += windowInfoPtr->imageTopOffset;
		destinationRect.bottom += windowInfoPtr->imageTopOffset;
		destinationRect.top -= lcToWindowUnitsVariablesPtr->lineScrollOffset;
		destinationRect.bottom -= lcToWindowUnitsVariablesPtr->lineScrollOffset;
	#endif
	
	#if defined multispec_wx
		destinationRect.top -= lcToWindowUnitsVariablesPtr->lineScrollOffset;
		destinationRect.bottom -= lcToWindowUnitsVariablesPtr->lineScrollOffset;
				// The legend is not part of the image window for the wx version
		legendWidth = 0;
	#endif
	
	if (destinationRect.bottom - destinationRect.top > 2) 
		{
		destinationRect.left = (int)(legendWidth + magnification *
			((sourceRectPtr->left+columnOffset)/columnInterval - hOrigin));
		destinationRect.right = (int)(legendWidth + magnification *
			((sourceRectPtr->right+columnOffset)/columnInterval - hOrigin) + .9999);

				// Now invalidate this area in the image window.

		#if defined multispec_mac
			GrafPtr savedPort;

			GetPort (&savedPort);
			SetPortWindowPort (gActiveImageWindow);
			InvalWindowRect (gActiveImageWindow, &destinationRect);
			SetPort (savedPort);
		#endif	// defined multispec_mac

		#if defined multispec_win
			destinationRect.left -= lcToWindowUnitsVariablesPtr->columnScrollOffset;
			destinationRect.right -= lcToWindowUnitsVariablesPtr->columnScrollOffset;
			windowInfoPtr->cImageWindowPtr->mImageViewCPtr->
										InvalidateRect ((RECT*)&destinationRect, FALSE);
		#endif	// defined multispec_win 

		#if defined multispec_wx
			destinationRect.left -= lcToWindowUnitsVariablesPtr->columnScrollOffset;
			destinationRect.right -= lcToWindowUnitsVariablesPtr->columnScrollOffset;
			
					// Note that even though false is given for not erasing the background.
					// In Mac OS version, it is ignored. The background is always erased.
		
			windowInfoPtr->cImageWindowPtr->mImageViewCPtr->m_Canvas->
					RefreshRect (wxRect (destinationRect.left,
												destinationRect.top,
												destinationRect.right-destinationRect.left+1,
												destinationRect.bottom-destinationRect.top+1),
										true);
		#endif

		SInt32 overlap = (SInt32)(1 / magnification);

		sourceRectPtr->top = MAX (sourceRectPtr->bottom - overlap, 1);
		if (sourceRectPtr->top >= displayBottomMax)
			sourceRectPtr->bottom = -1;

		}	// end "if (destinationRect.bottom - destinationRect.top > 2)"
		
	if (sourceRectPtr->bottom >= displayBottomMax)
		sourceRectPtr->bottom = -1;
	
}	// end "InvalidateImageSegment" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				void SetDisplayImageWindowSizeVariables
//
//	Software purpose:			The purpose of this routine is to set the display
//									structure image window size variables.
//
// Typical usage:				Not Applicable
//
//	Functional description:	The steps in this function follow.
//
//	Parameters in:					None
//
//	Parameters out:				None
//
//	Function Value Returned:	None
// 
// Called By:						LoadMultispectralDisplaySpecs in SDisplayThematic.cpp
//										LoadThematicDisplaySpecs in SDisplayThematic.cpp
//
//	Global Data:
//
//	Coded By:			Larry L. Biehl			Date: 12/06/1991
//	Revised By:			Larry L. Biehl			Date: 03/22/2019

void SetDisplayImageWindowSizeVariables (
				DisplaySpecsPtr					displaySpecsPtr)

{
	Rect									rect;

	double								cMagnification,
											lMagnification;
	
	SInt64								freeMemory,
											lContBlock;

	UInt32								extraBytes,
											interval,
											numberBytes,
											numberColumns;

	Boolean								memoryOK,
											numberColumnsOKFlag;


	if (displaySpecsPtr != NULL) 
		{
				// Set up default display area.  Load entire image into bit/pix
				// map if there is memory; otherwise increase column/line
				// interval until image bit/pix map will fit in memory with
				// some extra memory to spare.  Set magnification so that entire
				// image will fit within window on the screen.

		interval = 0;
		freeMemory = MGetFreeMemory (&lContBlock);

				// Get estimate of other bytes needed based on the number of bytes
				// the data is in.

		extraBytes = 3 * 256;
		if (gImageWindowInfoPtr->numberBytes >= 2)
			extraBytes = 3 * gImageWindowInfoPtr->numberBins;
			
				// Verify that freeMemory and lContBlock is larger than some minimum.
				// If not then just set the interval such that only 1 pixel will
				// be displayed.
				
		if (freeMemory > kMinFreeMemory && lContBlock > kMinFreeMemory)
			{
			memoryOK = FALSE;
			do {
				interval++;
				
				if (interval < displaySpecsPtr->columnEnd)
					{
					numberColumns = (displaySpecsPtr->columnEnd -
							  displaySpecsPtr->columnStart + interval) / interval;

					if (numberColumns <= gMaxRowBytes * 8 / displaySpecsPtr->pixelSize) 
						{
						numberBytes = displaySpecsPtr->lineEnd *
												displaySpecsPtr->columnEnd /
																		(interval * interval);

						if (displaySpecsPtr->pixelSize == 16)
							numberBytes *= 2;
						else if (displaySpecsPtr->pixelSize == 32)
							numberBytes *= 4;

						numberBytes += extraBytes;

						if ((numberBytes < lContBlock) &&
									(numberBytes + kMinFreeMemory < freeMemory))
							  memoryOK = TRUE;

						}	// end "if (numberColumns < ..."
						
					}	// end "if (interval < displaySpecsPtr->columnEnd)"
					
				else	// interval >= displaySpecsPtr->columnEnd
						// Keep interval from being larger than what can make a difference
						// in the amount of memory to be used.
					break;

				}	while (!memoryOK);
				
			}	// end "if (freeMemory > kMinFreeMemory && lContBlock > kMinFreeMemory)"
			
		else	// freeMemory <= kMinFreeMemory || lContBlock <= kMinFreeMemory
			interval = displaySpecsPtr->columnEnd;

				  // Now be sure that the number of columns to be displayed is not
				  // more than the maximum number that can be.

		if (gOSXCoreGraphicsFlag && numberColumns > kMaxNumberOSXDisplayColumns) 
			{
			numberColumnsOKFlag = FALSE;
			do {
				interval++;
				numberColumns = (displaySpecsPtr->columnEnd -
                        displaySpecsPtr->columnStart + interval) / interval;

				if (numberColumns <= kMaxNumberOSXDisplayColumns)
					numberColumnsOKFlag = TRUE;

            } while (!numberColumnsOKFlag);

			}	// end "if (gOSXCoreGraphicsFlag && ..."

		displaySpecsPtr->lineInterval = interval;
		displaySpecsPtr->columnInterval = interval;

		gSelectionRectangleFlag = FALSE;

				// Get the area of the image window

		GetActiveImageClipRectangle (kImageFrameArea, &rect);

		#if defined multispec_win
					// This is a kludge, but it is the only way that I have found
					// to get around a problem when the coordinate bar is to be
					// displayed by default. When it is, Windows insists on displaying
					// the scroll bars in the window when it is not needed.  This causes
					// the the maginification to be 1 step smaller than it needs to be.
					// Was not able to find another way around it by setting scroll
					// bar setting to get them to go away.
					// This code expands the rect so that it ignores the scroll bars for
					// calculating the magnification to be used.

			CMImageDoc* imageDocCPtr = (CMImageDoc*)gActiveImageViewCPtr->GetDocument ();
			if (imageDocCPtr->GetDisplayCoordinatesFlag ()) 
				{
				int scrollBarWidth;
				scrollBarWidth = GetSystemMetrics (SM_CYHSCROLL);
				rect.bottom += scrollBarWidth;

				scrollBarWidth = GetSystemMetrics (SM_CYVSCROLL);
				rect.right += scrollBarWidth;

				//rect.bottom +=	kSBarWidth+2;
				//rect.right += kSBarWidth+2;
				}	// end "if (imageDocCPtr->GetDisplayCoordinatesFlag ())"
		#endif	// defined multispec_win

				// Select magnification so that entire image will be shown in the
				// window. Magnification is = min (cMagnification, lMagnification)

		lMagnification = (double)(rect.bottom - rect.top) /
                ((displaySpecsPtr->lineEnd - displaySpecsPtr->lineStart + interval) /
                interval);
		if (lMagnification < 1.)
			lMagnification = 1. / (SInt16)(1 / lMagnification + 0.99999);
		if (lMagnification >= 1.)
			lMagnification = (SInt16)(lMagnification + 0.00001);
		lMagnification = MAX (lMagnification, gMinMagnification);
		lMagnification = MIN (lMagnification, gMaxMagnification);

		cMagnification = (double)(rect.right - rect.left) /
                ((displaySpecsPtr->columnEnd - displaySpecsPtr->columnStart + interval) /
                interval);
		if (cMagnification < 1.)
			cMagnification = 1. / (SInt16)(1 / cMagnification + 0.99999);
		if (cMagnification >= 1.)
			cMagnification = (SInt16)(cMagnification + 0.00001);
		cMagnification = MAX (cMagnification, gMinMagnification);
		cMagnification = MIN (cMagnification, gMaxMagnification);
		/*
		int numberChars = sprintf ((char*)gTextString3,
					" SDisplay:SetDiaplayImageWindowSizeVariables (interval, cMagnification, lMagnification): %ld, %f, %f%s", 
					interval,
					cMagnification,
					lMagnification,
					gEndOfLine);
		ListString ((char*)gTextString3, numberChars, gOutputTextH);
		*/
		displaySpecsPtr->magnification = (cMagnification < lMagnification) ?
																	cMagnification : lMagnification;
					 
				// This line was added for wxWidgets situations. The size of the window
				// being returned above is not correct all of the time.
		#if defined multispec_wx
			displaySpecsPtr->magnification = MIN (displaySpecsPtr->magnification, 1.0);
		#endif

		}	// end "if (displaySpecsPtr != NULL)"

}	// end "SetDisplayImageWindowSizeVariables"   



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				Boolean SetUpColorImageMemory
//
//	Software purpose:			The purpose of this routine is to set up the
//									off screen pix map and call the routines to fill
//									the off screen buffer
//
// Typical usage:				Not Applicable
//
//	Functional description:	The steps in this function follow.
//
//	Parameters in:					None
//
//	Parameters out:				None
//
//	Function Value Returned:	None
// 
// Called By:						
//
//	Global Data:
//
//	Coded By:			Larry L. Biehl			Date: 08/11/1988
//	Revised By:			Larry L. Biehl			Date: 03/19/2019

Boolean SetUpColorImageMemory (
				DisplaySpecsPtr					displaySpecsPtr,
				LongRect*							sourceRectPtr,
				UInt32*								pixRowBytesPtr)

{
	LongRect								longRect;
	//PixMapHandle						offScreenPixMapH;


			// Check status of file information structure.								

	if (gImageFileInfoPtr == NULL)
																						return (FALSE);

			// Set up off screen rectangle to draw picture into

	longRect.top = 0;
	longRect.left = 0;
	longRect.right = (SInt32)((displaySpecsPtr->columnEnd -
				displaySpecsPtr->columnStart + displaySpecsPtr->columnInterval) /
									displaySpecsPtr->columnInterval);

	displaySpecsPtr->offscreenChannelWidth = longRect.right;

	if (displaySpecsPtr->displayType == kSideSideChannelDisplayType) 
		{
		longRect.right += 2;
		displaySpecsPtr->offscreenChannelWidth = longRect.right;
		longRect.right *= displaySpecsPtr->numberChannels;

		}	// end "if (displaySpecsPtr->displayType == kSideSideChannelDisplayType)" 

	longRect.bottom = (SInt32)((displaySpecsPtr->lineEnd -
			displaySpecsPtr->lineStart + displaySpecsPtr->lineInterval) /
			displaySpecsPtr->lineInterval);

			// Store image dimensions in display specification structure			

	displaySpecsPtr->imageDimensions[kVertical] = longRect.bottom;
	displaySpecsPtr->imageDimensions[kHorizontal] = longRect.right;

			// Set up the offscreen pix map.													

	if (GetOffscreenGWorld (gImageWindowInfoPtr,
										displaySpecsPtr,
										&longRect,
										pixRowBytesPtr) != noErr)
		return (FALSE);

			// set up source rectangle for copy bits, rect.bottom will be reset 	 
			// later																					

	*sourceRectPtr = longRect;

			// Initialize the size of the title height for the window.				

	gImageWindowInfoPtr->titleHeight = 0;

	#if defined multispec_wx
				// The image top offset is 0 for wx implementation since the
				// coordinate bar is not a part of the image window.
		gImageWindowInfoPtr->imageTopOffset = 0;
	
		CMImageDoc* imageDocCPtr =
							(CMImageDoc*)gActiveImageViewCPtr->GetDocument ();
		CMImageFrame* imageFrameCPtr = imageDocCPtr->GetImageFrameCPtr ();
		imageFrameCPtr->ShowTitleView (0);
	#endif

	#if defined multispec_mac
		ActivatePalette (gActiveImageWindow);
		gImageWindowInfoPtr->imageTopOffset = gImageWindowInfoPtr->coordinateHeight;
	#endif	// defined multispec_mac

	#if defined multispec_win
		gActiveImageViewCPtr->SendMessage (
								WM_DOREALIZE, (WPARAM)gActiveImageViewCPtr->m_hWnd, 1);
		gImageWindowInfoPtr->imageTopOffset = gImageWindowInfoPtr->coordinateHeight;
	#endif	// defined multispec_win

	return (TRUE);

}	// end "SetUpColorImageMemory"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				void SetUpImageWindowTypeParameters
//
//	Software purpose:			The purpose of this routine is to set up some parameters
//									for the image window relative to multispectral or thematic
//									window type.
//
// Typical usage:				Not Applicable
//
//	Functional description:	The steps in this function follow.
//
//	Parameters in:					None
//
//	Parameters out:				None
//
//	Function Value Returned:	None	
// 
// Called By:						
//
//	Global Data:
//
//	Coded By:			Larry L. Biehl			Date: 09/29/2006
//	Revised By:			Larry L. Biehl			Date: 11/03/2019

void SetUpImageWindowTypeParameters (
				WindowPtr							windowPtr,
				WindowInfoPtr						imageWindowInfoPtr,
				FileInfoPtr							imageFileInfoPtr,
				DisplaySpecsPtr					displaySpecsPtr)

{
	if (imageWindowInfoPtr->imageType == kMultispectralImageType) 
		{
		if (displaySpecsPtr->displayType == k1_ChannelThematicDisplayType) 
			{
			if (imageWindowInfoPtr->windowType != kThematicWindowType) 
				{
				imageWindowInfoPtr->windowType = kThematicWindowType;
				gActiveWindowType = kThematicWindowType;
				imageWindowInfoPtr->showLegend = TRUE;

				if (imageWindowInfoPtr->legendWidth == 0) 
					{
							// Must currently be multispectral type window. Set the window 
							// for a legend. Make sure at least some of the image window is 
							// showing.
							
					UpdateLegendParameters (imageWindowInfoPtr->windowInfoHandle,
													gDefaultLegendWidth);

					gActiveLegendWidth = gDefaultLegendWidth;

					#if defined multispec_mac   
						SetWindowToImageSize (
												windowPtr, imageWindowInfoPtr, displaySpecsPtr);
					#endif	// defined multispec_mac

					#if defined multispec_win 
						CRect rectFrame;

						CMImageDoc* imageDocCPtr = 
												(CMImageDoc*)gActiveImageViewCPtr->GetDocument ();
						CMImageFrame* imageFrameCPtr = imageDocCPtr->GetImageFrameCPtr ();

								// Add the amount needed for the legend and the split window 
								// bar.

						imageFrameCPtr->GetWindowRect (rectFrame);
						CSize size = rectFrame.Size ();
						size.cx += 180;
						imageFrameCPtr->SetWindowPos (
														NULL, 
														0, 
														0, 
														size.cx, 
														size.cy,
														SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

						imageFrameCPtr->SetSplitterParameters (kThematicWindowType);
						imageFrameCPtr->SetLegendWidth (gImageWindowInfoPtr->legendWidth);
					#endif	// defined multispec_win

					#if defined multispec_wx
						int			frameClientHeight,
										frameClientWidth, 
										mainWindowWidth,
										newFrameClientWidth;

						CMImageDoc* imageDocCPtr = 
												(CMImageDoc*)gActiveImageViewCPtr->GetDocument ();
						CMImageFrame* imageFrameCPtr = imageDocCPtr->GetImageFrameCPtr ();
						
						imageFrameCPtr->GetClientSize (
															&frameClientWidth, &frameClientHeight);
					
						#if defined multispec_wxlin
							int			mainWindowHeight;
					
							GetMainFrame()->GetClientSize (
																&mainWindowWidth, &mainWindowHeight);
							mainWindowWidth -= 2;
						#endif
						#if defined multispec_wxmac
							wxRect	displayRect;
							displayRect = wxDisplay().GetClientArea ();
					
							mainWindowWidth = displayRect.GetWidth () - 2;
						#endif

						newFrameClientWidth = 
											frameClientWidth + imageWindowInfoPtr->legendWidth;
						newFrameClientWidth = MIN (mainWindowWidth, newFrameClientWidth);

						imageFrameCPtr->SetClientSize (
															newFrameClientWidth, frameClientHeight);
						
						imageFrameCPtr->SetSplitterParameters (kThematicWindowType);
						imageFrameCPtr->SetLegendWidth (gImageWindowInfoPtr->legendWidth);
					#endif

					}	// end "if (imageWindowInfPtr->legendWidth == 0"

				}	// end "if (imageWindowInfPtr->windowType != kThematicWindowType)"

			InitializeClassGroupsVector (imageFileInfoPtr, displaySpecsPtr);

			}	// end "if (...->displayType == k1_ChannelThematicDisplayType)"

		else	// displaySpecsPtr->displayType != k1_ChannelThematicDisplayType
			{
			if (imageWindowInfoPtr->windowType != kImageWindowType) 
				{
							// Must currently be ThematicWindowType. Change parameters to be
							// ImageWindowType.

				imageWindowInfoPtr->windowType = kImageWindowType;
				imageWindowInfoPtr->showLegend = FALSE;
				imageWindowInfoPtr->legendWidth = 0;
				gActiveLegendWidth = 0;

				UpdateThematicLegendControls ((WindowPtr)imageWindowInfoPtr->windowPtr);
				//AdjustLegendListLength (imageWindowInfPtr->windowPtr, FALSE);	
				#if defined multispec_mac
					SetWindowToImageSize (windowPtr, imageWindowInfoPtr, displaySpecsPtr);
					DrawScrollBars (imageWindowInfoPtr->windowPtr);
				#endif	// defined multispec_mac

				#if defined multispec_win  
					CMImageDoc* imageDocCPtr = 
											(CMImageDoc*)gActiveImageViewCPtr->GetDocument ();
					CMImageFrame* imageFrameCPtr = imageDocCPtr->GetImageFrameCPtr ();
					imageFrameCPtr->SetSplitterParameters (kImageWindowType);
					imageFrameCPtr->SetLegendWidth (gImageWindowInfoPtr->legendWidth);
				#endif	// defined multispec_win

				#if defined multispec_wx
					CMImageDoc* imageDocCPtr = 
												(CMImageDoc*)gActiveImageViewCPtr->GetDocument ();
					CMImageFrame* imageFrameCPtr = imageDocCPtr->GetImageFrameCPtr ();
					imageFrameCPtr->SetSplitterParameters (kImageWindowType);
					imageFrameCPtr->SetLegendWidth (gImageWindowInfoPtr->legendWidth);
				#endif	// defined multispec_wx

            }	// end "if (imageWindowInfPtr->windowType != kImageWindowType)"

			}	// end "else ...->displayType != k1_ChannelThematicDisplayType"

		}	// end "if (imageWindowInfPtr->windowType == kImageWindowType)" 

	else	// imageWindowInfPtr->imageType == kThematicImageType 
		{
		imageWindowInfoPtr->windowType = kThematicWindowType;
		#if defined multispec_wx
			CMImageDoc* imageDocCPtr = (CMImageDoc*)gActiveImageViewCPtr->GetDocument ();
			CMImageFrame* imageFrameCPtr = imageDocCPtr->GetImageFrameCPtr ();
			imageFrameCPtr->SetSplitterParameters (kThematicWindowType);
			imageFrameCPtr->SetLegendWidth (gImageWindowInfoPtr->legendWidth);
		#endif	// defined multispec_wx
		
		}	// end "else imageWindowInfPtr->imageType == kThematicImageType"

}	// end "SetUpImageWindowTypeParameters" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				void SetVectorOverlays
//
//	Software purpose:			The purpose of this routine is to set the vector overlay
//									parameter and check box for the multispectral and thematic
//									dialog boxes.
//
// Typical usage:				Not Applicable
//
//	Functional description:	
//
//	Parameters in:					None
//
//	Parameters out:				None
//
//	Function Value Returned:	None	
// 
// Called By:						
//
//	Global Data:
//
//	Coded By:			Larry L. Biehl			Date: 12/11/2006
//	Revised By:			Larry L. Biehl			Date: 12/28/2006	

void SetVectorOverlays (
				WindowInfoPtr						windowInfoPtr,
				DialogPtr							dialogPtr,
				SInt16								vectorOverlayItem,
				Boolean*								includeVectorOverlaysFlagPtr)

{
			// Only show option to overlay vectors if no image currently exists
			// for the window and shape files do exist.

	if (CheckIfOffscreenImageExists (windowInfoPtr) || gNumberShapeFiles <= 0)
	//if (gActiveOffscreenMapExists || gNumberShapeFiles <= 0)
		{
		SetDLogControl (dialogPtr, vectorOverlayItem, 0);
		HideDialogItem (dialogPtr, vectorOverlayItem);
		*includeVectorOverlaysFlagPtr = FALSE;

		}	// end "if (gActiveOffscreenMapExists || gNumberShapeFiles <= 0)"

	else	// !gActiveOffscreenMapExists
		{
				// Check vector overlays are available for this image.

		if (CheckIfVectorOverlaysIntersectImage (windowInfoPtr)) 
			{
			SetDLogControl (dialogPtr, vectorOverlayItem, 1);
			SetDLogControlHilite (dialogPtr, vectorOverlayItem, 0);
			*includeVectorOverlaysFlagPtr = TRUE;

			}	// end "if (CheckIfVectorOverlaysIntersectImage (windowInfoPtr))"

		else	// !CheckIfVectorOverlaysIntersectImage (windowInfoPtr)
			{
			SetDLogControl (dialogPtr, vectorOverlayItem, 0);
			SetDLogControlHilite (dialogPtr, vectorOverlayItem, 255);
			*includeVectorOverlaysFlagPtr = FALSE;

			}	// end "if (CheckIfVectorOverlaysIntersectImage (windowInfoPtr))"

		}	// end "else !gActiveOffscreenMapExists"

}	// end "SetVectorOverlays"  



#if defined multispec_mac	
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				void SetWindowToImageSize
//
//	Software purpose:			The purpose of this routine is to set the window to fit
//									around the image size yet stay within the screen size.
//
// Typical usage:				Not Applicable
//
//	Functional description:	
//
//	Parameters in:					None
//
//	Parameters out:				None
//
//	Function Value Returned:	None	
// 
// Called By:						
//
//	Global Data:
//
//	Coded By:			Larry L. Biehl			Date: 12/17/2008
//	Revised By:			Larry L. Biehl			Date: 07/12/2011	

void SetWindowToImageSize (
				WindowPtr							windowPtr,
				WindowInfoPtr						imageWindowInfoPtr,
				DisplaySpecsPtr					displaySpecsPtr)
				
{
	Point									lowerRightLocation,
											upperLeftLocation;

	Rect									maxDeviceSize,
											rect;

	LongPoint							minimumWindowSizePoint;
	

			// Get the current maximum rectangular display area for the monitor

	//GetDisplayRegionRectangle (&growArea);									
	//growArea.right -= growArea.left;

			// Get the current window size

	GetWindowPortBounds (windowPtr, &rect);

			// If an image is already drawn in the window, get the minimum size needed
			// for that image with the magnication setting used.

	if (displaySpecsPtr->imageDimensions[kHorizontal] > 0)
		GetMinimumDisplaySizeForImage (imageWindowInfoPtr->windowInfoHandle,
													&minimumWindowSizePoint);

	else	// Image not displayed yet
		minimumWindowSizePoint.h = rect.right + gActiveLegendWidth;

			// Now get the max that is possible for the device that the image lower right corner
			// is in. The default will be the size of the current window.

	upperLeftLocation = *((Point*)&rect.top);
	LocalToGlobal (&upperLeftLocation);
	lowerRightLocation = *((Point*)&rect.bottom);
	LocalToGlobal (&lowerRightLocation);

	maxDeviceSize.left = upperLeftLocation.h;
	maxDeviceSize.top = upperLeftLocation.v;
	maxDeviceSize.right = lowerRightLocation.h;
	maxDeviceSize.bottom = lowerRightLocation.v;
	GetMaximumSizeForDevicePointIsIn (&lowerRightLocation, &maxDeviceSize);

	rect.right = (SInt16)MIN (
					minimumWindowSizePoint.h, maxDeviceSize.right - upperLeftLocation.h);

	GrowNonGraphWindow (windowPtr,
								imageWindowInfoPtr->windowInfoHandle,
								(SInt16)rect.bottom,
								(SInt16)rect.right);

}	// end "SetWindowToImageSize" 
#endif	// defined multispec_mac
