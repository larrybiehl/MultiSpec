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
//	File:						xUtilities.cpp
//
//	Authors:					Larry L. Biehl, Tsung Tai Yeh
//
//	Revision date:			01/10/2020
//
//	Language:				C
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	The purpose of the routines in this file is to provide
//								utility type functions in MultiSpec for Linux OS.
//
/* Template for debugging
		int numberChars = sprintf ((char*)gTextString3,
													" xUtilities.cpp: (): %s",
													gEndOfLine);
		ListString ((char*)gTextString3, numberChars, gOutputTextH);	
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include	<ctype.h>  

#include "SImageWindow_class.h"
#include "SDisplay_class.h"

#include "wx/combobox.h"
#include "wx/stdpaths.h"
#include "xImageFrame.h"
#include "xImageView.h"

//#if defined multispec_wx
#include "wx/rawbmp.h"
//#endif


#if defined multispec_wxmac
	#include "xBitmapRefData.h"
#endif	// defined multispec_wxmac



void* BeginBitMapRawDataAccess (
				WindowInfoPtr						windowInfoPtr,
				wxBitmap*							bitMapPtr)

{
	#if defined multispec_wxlin
				// Get the bitmap raw data pointer again. It may have changed.
				// Only do this for multispectral image windows

		#if defined multispec_wxlin_alpha
			wxAlphaPixelData pixeldata (gActiveImageViewCPtr->m_ScaledBitmap);
		#else
			wxNativePixelData pixeldata (gActiveImageViewCPtr->m_ScaledBitmap);
		#endif
		return (pixeldata.GetPixels().m_ptr);
	#endif
	
	#if defined multispec_wxmac
		return (bitMapPtr->GetBitmapData()->BeginRawAccess ());
	#endif
	
}	// end "BeginBitMapRawDataAccess"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		CopyOffScreenImage
//
//	Software purpose:	This routine copies an off screen bit/pix map
//							to the specified window base on the routine 
//							defined by copyType.
//
//	Parameters in:				
//
//	Parameters out:				
//
//Value Returned:		None
// 
// Called By:			OnDraw in xImageView.cpp
//							DoThematicWColorsUpdate in SThematicWindow.cpp
//
//	Coded By:			Larry L. Biehl			Date: 08/31/1988
//	Revised By:			Larry L. Biehl			Date: 01/10/2020

void CopyOffScreenImage (
				CMImageView*						imageViewCPtr,
				CDC*									pDC,
				CMImageWindow*						imageWindowCPtr,
				RgnHandle							invalidRgn,
				Rect*									inSourceRect,
						// see descriptions in cases for switch statement below.
				SInt16								copyType)

{
	double								hOrigin,
											magnification,
											vOrigin;

   LongRect								lDestinationRect,
											lSourceRect;
	
											//tDestinationRect,
											//tSourceRect;

   Rect									destinationRect,
											sourceRect,
											windowRect;

   CMPalette*							paletteCPtr;
	
	wxPoint								scrollOffset;
	
   Handle								displaySpecsHandle,
											imageBaseAddressH,
											offScreenMapHandle;

   WindowInfoPtr						windowInfoPtr;

   DisplaySpecsPtr					displaySpecsPtr;
	
	int 									pixRowBytes;

   SInt32								numberImageOverlays,
											xDimension,
											yDimension;

   UInt32								numberVectorOverlays;

   SInt16								grafPortType;

   SInt16								legendWidth,
											titleHeight,
											windowCode;

   Boolean								drawBaseImageFlag,
											drawVectorOverlaysFlag,
											projectWindowFlag;
	

				// Intialize some local variables.

	windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
														imageWindowCPtr->GetWindowInfoHandle ());
	
   if (windowInfoPtr->drawBitMapFlag)
		{
      legendWidth = 0;

      titleHeight = windowInfoPtr->titleHeight;
      grafPortType = windowInfoPtr->grafPortType;
      numberImageOverlays = windowInfoPtr->numberImageOverlays;
      drawVectorOverlaysFlag = windowInfoPtr->drawVectorOverlaysFlag;
      numberVectorOverlays = windowInfoPtr->numberVectorOverlays;
      drawBaseImageFlag = windowInfoPtr->drawBaseImageFlag;

      offScreenMapHandle = windowInfoPtr->offScreenMapHandle;
      imageBaseAddressH = windowInfoPtr->imageBaseAddressH;

      projectWindowFlag = windowInfoPtr->projectWindowFlag;

      GetWindowClipRectangle (imageViewCPtr, kImageArea, &windowRect);
		
      		// Note that for the copy, any titleHeight for side by side images
      		// needs to be removed. The offset is being handled by offsetting the
      		// original where the image is being drawn.
				// Note used now.
		
		//windowRect.top -= titleHeight;
		//windowRect.bottom -= titleHeight;

				// Get handle and pointer to display specification for window

      displaySpecsHandle = imageViewCPtr->m_displayMultiCPtr->mDisplaySpecsHandle;
      displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (displaySpecsHandle);

      magnification = displaySpecsPtr->magnification;
      vOrigin = displaySpecsPtr->origin[kVertical];
      hOrigin = displaySpecsPtr->origin[kHorizontal];
      yDimension = (SInt32)displaySpecsPtr->imageDimensions[0];
      xDimension = (SInt32)displaySpecsPtr->imageDimensions[1];
      
      paletteCPtr = displaySpecsPtr->paletteObject;
      if (((imageViewCPtr != gActiveImageViewCPtr) || gInBackground) &&
              displaySpecsPtr->backgroundPaletteObject != NULL)
         paletteCPtr = displaySpecsPtr->backgroundPaletteObject;

      if (copyType != kClipboardCopy && copyType != kPrinterCopy)
			{
			imageViewCPtr->m_Canvas->CalcUnscrolledPosition (0,
																				0,
																				&scrollOffset.x,
																				&scrollOffset.y);

			windowRect.top += MAX (0 , scrollOffset.y);
			windowRect.bottom += MAX (0, scrollOffset.y);
			windowRect.left += MAX (scrollOffset.x, 0);
			windowRect.right += MAX (scrollOffset.x, 0);

			}	// end "if (copyType != kClipboardCopy && copyType != kPrinterCopy)" 

      else // copyType == kClipboardCopy || copyType == kPrinterCopy)
			{
         scrollOffset.x = 0;
         scrollOffset.y = 0;

			}	// end "else copyType == kClipboardCopy || copyType == kPrinterCopy)"

      if (vOrigin < 0)
         copyType = kDestinationCopy;

				// Set clip region of window to the image portion.						

		//if (copyType != kClipboardCopy)
		//	ClipRect (&windowRect);

				// Branch to appropriate case for copyType.								

      switch (copyType)
			{
			/*
         case kClipboardCopy:
						// This case is currently not implemented for wxWidgets base apps
						// Map sourceRect to destination that starts at (0,0)
						// This is for copy to clipboard.    

            legendWidth = windowInfoPtr->legendWidth;

            lSourceRect.top = inSourceRect->top;
            lSourceRect.left = inSourceRect->left;
            lSourceRect.bottom = inSourceRect->bottom;
            lSourceRect.right = inSourceRect->right;

            lDestinationRect.top = (SInt32) titleHeight;
            lDestinationRect.left = (SInt32) legendWidth;
            lDestinationRect.bottom = titleHeight + (SInt32)
									((lSourceRect.bottom - lSourceRect.top) * magnification);
            lDestinationRect.right = legendWidth + (SInt32)
									((lSourceRect.right - lSourceRect.left) * magnification);
            break;
				*/
         case kSourceCopy:
						// Map to wherever sourceRect * magnification indicates.  This
						// allows one to draw parts of the window at a time.
						// Note that the update rectangle that comes from wxWidgets seems
						// to leave off the a pixel to the right and bottom.

            lSourceRect.top = inSourceRect->top;
            lSourceRect.left = inSourceRect->left;
            lSourceRect.bottom = inSourceRect->bottom + 1;
            lSourceRect.right = inSourceRect->right + 1;
				
				lDestinationRect.left =
											(SInt32)(lSourceRect.left / magnification);
				if (lSourceRect.right > windowRect.right)
					{
							// Map to where image is to go in the window.
					
					lDestinationRect.right =
											(SInt32)(windowRect.right / magnification + .9999);
					
					}	// end "if (lSourceRect.right > windowRect.right)"
				
				else	//	lSourceRect.right <= windowRect.right
					{
             	lDestinationRect.right =
											(SInt32)(lSourceRect.right / magnification + .9999);
					
					}	// end "else	lSourceRect.right <= windowRect.right"
				
				lDestinationRect.top =
									(SInt32)(lSourceRect.top / magnification);
				if (lSourceRect.bottom > windowRect.bottom)
					{
					lDestinationRect.bottom =
											(SInt32)(windowRect.bottom / magnification + .9999);
					
					}	// end "if (lSourceRect.bottom > windowRect.bottom)"
				
				else	// lSourceRect.bottom <= windowRect.bottom
					{
             	lDestinationRect.bottom =
									(SInt32)(lSourceRect.bottom / magnification + .9999);
					
					}	// end "else lSourceRect.bottom <= windowRect.bottom"
					
				lDestinationRect.left = MAX (0, lDestinationRect.left);
				lDestinationRect.top = MAX (0, lDestinationRect.top);
				/*
						// for testing

				tDestinationRect.top = (SInt32)(windowRect.top / magnification);
				tDestinationRect.bottom =
										(SInt32)(windowRect.bottom / magnification + .9999);
				tDestinationRect.left = (SInt32)(windowRect.left / magnification);
				tDestinationRect.right =
										(SInt32)(windowRect.right / magnification + .9999);
				
				tDestinationRect.left = MAX (0, tDestinationRect.left);
				tDestinationRect.top = MAX (0, tDestinationRect.top);
				
				tSourceRect.top = (SInt32)((windowRect.top) / magnification);
				tSourceRect.left = (SInt32)((windowRect.left) / magnification);
				tSourceRect.bottom =
									(SInt32)((windowRect.bottom) / magnification + .9999);
				tSourceRect.right =
									(SInt32)((windowRect.right) / magnification + .9999);
				
				tSourceRect.bottom = MIN (tSourceRect.bottom, yDimension);
				tSourceRect.right = MIN (tSourceRect.right, xDimension);
				tSourceRect.left = MAX (0, tSourceRect.left);
				tSourceRect.top = MAX (0, tSourceRect.top);
				*/
            break;
			/*
         case kScrollCopy: // scroll copy
						// This case is currently not implemented for wxWidgets base apps
						// Copy portion of offscreen image map that fits in the invalid
						// region of the window. 	This is from a scroll operation.

            //lDestinationRect = (*invalidRgn)->rgnBBox;
			*/
         case kDestinationCopy:
						// Copy portion of offscreen map that fits within the input
						// destination area of the window.  This is from a window update
						// event.
				/*
            if (copyType == kDestinationCopy)
					{
               lDestinationRect.top = inSourceRect->top;
               lDestinationRect.left = inSourceRect->left;
               lDestinationRect.bottom = inSourceRect->bottom;
               lDestinationRect.right = inSourceRect->right;
               
					}	// end "if (copyType == kDestinationCopy)"
				*/
						// Make certain that the legend is not included.

				lDestinationRect.top = (SInt32)(windowRect.top/ magnification);
				lDestinationRect.bottom =
										(SInt32)(windowRect.bottom/ magnification + .9999);
				lDestinationRect.left = (SInt32)(windowRect.left/ magnification);
				lDestinationRect.right =
										(SInt32)(windowRect.right/ magnification + .9999);
				
				lDestinationRect.left = MAX (0, lDestinationRect.left);
				lDestinationRect.top = MAX (0, lDestinationRect.top);
				
				lSourceRect.top = (SInt32)((windowRect.top) / magnification);
				lSourceRect.left = (SInt32)((windowRect.left) / magnification);
				lSourceRect.bottom =
									(SInt32)((windowRect.bottom) / magnification + .9999);
				lSourceRect.right =
									(SInt32)((windowRect.right) / magnification + .9999);
				
				lSourceRect.bottom = MIN (lSourceRect.bottom, yDimension);
				lSourceRect.right = MIN (lSourceRect.right, xDimension);
				lSourceRect.left = MAX (0, lSourceRect.left);
				lSourceRect.top = MAX (0, lSourceRect.top);
				break;	// end "case: kDestinationCopy"
			/*
			case kPrinterCopy:
						// This case is currently not implemented for wxWidgets base apps
						// Printer. Just map the source to a destination rectangle that
						// starts at 0, 0. The origon has been offset so that the image
						// appears in the middle of the page.									

				lSourceRect.top = inSourceRect->top;
				lSourceRect.left = inSourceRect->left;
				lSourceRect.bottom = inSourceRect->bottom;
				lSourceRect.right = inSourceRect->right;
				legendWidth = windowInfoPtr->legendWidth;

				lDestinationRect.top = (SInt32)
									(titleHeight * imageViewCPtr->m_printerTextScaling + .5);
				lDestinationRect.bottom = (SInt32)
							(titleHeight * imageViewCPtr->m_printerTextScaling +
								magnification * (lSourceRect.bottom - lSourceRect.top) + .5);
				lDestinationRect.left = (SInt32)
								(legendWidth * imageViewCPtr->m_printerTextScaling * 1.4 + .5);
				lDestinationRect.right = (SInt32)
							(legendWidth * imageViewCPtr->m_printerTextScaling * 1.4 +
								magnification * (lSourceRect.right - lSourceRect.left) + .5);
				break;
			*/
			}	// end "switch (copyType)"

		sourceRect.top = (int)lSourceRect.top;
		sourceRect.left = (int)lSourceRect.left;
		sourceRect.bottom = (int)lSourceRect.bottom;
		sourceRect.right = (int)lSourceRect.right;

		destinationRect.top = (int)lDestinationRect.top;
		destinationRect.left = (int)lDestinationRect.left;
		destinationRect.bottom = (int)lDestinationRect.bottom;
		destinationRect.right = (int)lDestinationRect.right;
		
		//ClipRect (&destinationRect);
	
		if (windowInfoPtr->imageType == kThematicImageType ||
							displaySpecsPtr->displayType == k1_ChannelThematicDisplayType)
			{
						// RGB values will be copied to the bitmap if the palette changed
						// since the last copy.
				
				if (!paletteCPtr->GetPaletteLoadedFlag ())
					{
					unsigned char*		bitmapBufferPtr;
					
					int					column,
											line,
											updateEndLine,
											updateStartLine;
					
					updateStartLine = displaySpecsPtr->updateStartLine;
					updateEndLine = displaySpecsPtr->updateEndLine;
					
					wxBitmap& scaledBitmap = imageViewCPtr->m_ScaledBitmap;
					unsigned char* imageDataPtr = (unsigned char*)imageBaseAddressH;
					#if defined multispec_wxmac
						unsigned char* baseBitmapBufferPtr =
												(UCharPtr)BeginBitMapRawDataAccess (windowInfoPtr, &scaledBitmap);
						pixRowBytes = scaledBitmap.GetBitmapData()->GetBytesPerRow ();
					#endif
					#if defined multispec_wxlin
						#if defined multispec_wxlin_alpha
							wxAlphaPixelData pixeldata (scaledBitmap);
						#else
							wxNativePixelData pixeldata (scaledBitmap);
						#endif
						unsigned char* baseBitmapBufferPtr =
													(unsigned char*)pixeldata.GetPixels().m_ptr;
						pixRowBytes = pixeldata.GetRowStride ();
					#endif
					int numberColumns = scaledBitmap.GetWidth ();
					int numberLines = scaledBitmap.GetHeight ();
					
					if (updateStartLine == -1)
						updateStartLine = 0;
					
					if (updateEndLine == -1)
						updateEndLine = numberLines;
					
					baseBitmapBufferPtr += (SInt64)updateStartLine * pixRowBytes;
					imageDataPtr += updateStartLine * numberColumns;
				
					for (line=updateStartLine; line<updateEndLine; line++)
						{
						bitmapBufferPtr = baseBitmapBufferPtr;
						for (column=0; column<numberColumns; column++)
							{
							#if defined multispec_wxmac_alpha
										// Skip first (Alpha) byte
								bitmapBufferPtr++;
							#endif
							
							*bitmapBufferPtr =
												paletteCPtr->mPaletteObject[*imageDataPtr].red;
							bitmapBufferPtr++;
							*bitmapBufferPtr =
												paletteCPtr->mPaletteObject[*imageDataPtr].green;
							bitmapBufferPtr++;
							*bitmapBufferPtr =
												paletteCPtr->mPaletteObject[*imageDataPtr].blue;
							
							#if defined multispec_wxlin_alpha
										// Skip last (Alpha) byte
								bitmapBufferPtr++;
							#endif
							
							imageDataPtr++;
							bitmapBufferPtr++;
							
							}	// end "for (column=0; column<numberColumns; column++)"
						
						baseBitmapBufferPtr += pixRowBytes;
						
						}	// end "for (line=0; line<numberLines; line++)"
					
					EndBitMapRawDataAccess (&scaledBitmap);
					
					paletteCPtr->SetPaletteLoadedFlag (TRUE);
					/*
					int numberChars = sprintf ((char*)gTextString3,
						" xUtilities.cpp:CopyOffScreenImage (updateStartLine, updateEndLine): %d, %d%s",
						updateStartLine,
						updateEndLine,
						gEndOfLine);
					ListString ((char*)gTextString3, numberChars, gOutputTextH);
					*/
					}	// end "if (paletteCPtr->GetPaletteChangedFlag ())"

						// Draw the legend
				
				imageViewCPtr->DrawLegend ();
				
				}	// end "if (displaySpecsPtr->displayType == ...)"
		
		else	// ...->imageType == kMultispectralImageType &&
				//							...->displayType != k1_ChannelThematicDisplayType
				{

				}	// end "else displaySpecsPtr->displayType != ..."
		
		if (numberImageOverlays > 0 || numberVectorOverlays > 0 || projectWindowFlag)
			{
			windowCode = kToImageWindow;
			if (copyType == kClipboardCopy)
				windowCode = kToClipboardWindow;
			else if (copyType == kPrinterCopy)
				windowCode = kToPrintWindow;

			if (windowCode >= kToPrintWindow)
				{
				displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (displaySpecsHandle);

				displaySpecsPtr->origin[kVertical] = lSourceRect.top;
				displaySpecsPtr->origin[kHorizontal] = lSourceRect.left;
				
				}	// end "if (windowCode >= kToPrintWindow)"
			
			}	// end "if (numberImageOverlays > 0 || numberVectorOverlays > 0 || ..."
					  
			// Draw the image overlay if they exist.

				// originally in xImageCanvas.cpp OnPaint
		
		wxMemoryDC displaydc;
		displaydc.SelectObject (imageViewCPtr->m_ScaledBitmap);
		//displaydc.SelectObject (imageViewCPtr->m_ScaledGraphicsBitmap);
		
		if (imageViewCPtr->m_ScaledBitmap.Ok () && drawBaseImageFlag)
			{
			/*
			int numberChars = sprintf ((char*)gTextString3,
													" xUtilities.cpp:CopyOffscreen (left, right, top, bottom): %d, %d, %d, %d%s",
													destinationRect.left,
													destinationRect.right,
													destinationRect.top,
													destinationRect.bottom,
													gEndOfLine);
			ListString ((char*)gTextString3, numberChars, gOutputTextH);
			*/
			/*
			int numberChars4 = sprintf ((char*)gTextString3,
					" xUtilities.cpp:CopyOffscreen (updateStartLine, updateEndLine): %d, %d%s%s",
					displaySpecsPtr->updateStartLine,
					displaySpecsPtr->updateEndLine,
					gEndOfLine,
					gEndOfLine);
			ListString ((char*)gTextString3, numberChars4, gOutputTextH);
			*/
			/*
			int numberChars2 = sprintf ((char*)gTextString3,
													" xUtilities.cpp:CopyOffscreen (tdestination): %d, %d, %d, %d%s",
													tDestinationRect.left,
													tDestinationRect.right,
													tDestinationRect.top,
													tDestinationRect.bottom,
													gEndOfLine);
			ListString ((char*)gTextString3, numberChars2, gOutputTextH);
			
			int numberChars3 = sprintf ((char*)gTextString3,
													" xUtilities.cpp:CopyOffscreen (tSourceRect): %d, %d, %d, %d%s",
													tSourceRect.left,
													tSourceRect.right,
													tSourceRect.top,
													tSourceRect.bottom,
													gEndOfLine);
			ListString ((char*)gTextString3, numberChars3, gOutputTextH);
			*/
			int destinationRectWidth =  destinationRect.right - destinationRect.left;
			int destinationRectHeight = destinationRect.bottom - destinationRect.top;
			
			pDC->Blit (destinationRect.left, // DestX
							destinationRect.top, // DestY
							destinationRectWidth, // nDestWidth
							destinationRectHeight, // nDestHeight   
							&displaydc,  
							destinationRect.left, // SrcX
							destinationRect.top, // SrcY
							wxCOPY, // 
							false, // useMask
							wxDefaultCoord, // 
							wxDefaultCoord); // 
			
			}	// end "if (my_image.Ok () && drawBaseImageFlag)"
		
		displaydc.SelectObject (wxNullBitmap);
		
		#if defined multispec_wxlin
			if (windowInfoPtr->offscreenMapSize == 0)
				{
						// Get the bitmap raw data pointer again. It may have changed.
						// imageBaseAddressH is the raw pointer to the bitmap data.

				#if defined multispec_wxlin_alpha
					wxAlphaPixelData pixeldata (imageViewCPtr->m_ScaledBitmap);
				#else
					wxNativePixelData pixeldata (imageViewCPtr->m_ScaledBitmap);
				#endif
				windowInfoPtr->imageBaseAddressH = (unsigned char*)pixeldata.GetPixels().m_ptr;
				
				}	// end "if (windowInfoPtr->offscreenMapSize == 0)"
		#endif
		
			// Draw the image overlay if they exist.
		
		if (numberImageOverlays > 0)
			DrawImageOverlays (imageViewCPtr,
										imageWindowCPtr->GetWindowInfoHandle (),
										pDC,
										NULL,
										NULL,
										&destinationRect,
										//&windowRect,
										&sourceRect, // sourceRect changed inside
										windowCode);
		  
			// Draw the vector overlays if they exist.
		
		if (drawVectorOverlaysFlag && numberVectorOverlays > 0)
			DrawArcViewShapes (imageViewCPtr,
										imageWindowCPtr->GetWindowInfoHandle (),
										&windowRect,
										windowCode,
										NULL);
		/*
				// If this window is the project window, outline the fields if requested.
				// Set clipping area so that all of text will be printed on the image,
				// not just the image segment clipped area.
		
		if (projectWindowFlag)
			{
      	GetWindowClipRectangle (imageViewCPtr, kImageArea, &windowRect);
			ClipRect (&windowRect);
			
			OutlineFieldsControl (gProjectInfoPtr->statsWindowMode,
											  imageViewCPtr,
											  imageWindowCPtr->GetWindowInfoHandle (),
											  windowCode);
			
			}	// end "if (projectWindowFlag)"
		*/
		if (numberImageOverlays > 0 ||
					numberVectorOverlays > 0 ||
							(projectWindowFlag && windowCode >= 2))
			{
			displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (displaySpecsHandle);

			displaySpecsPtr->origin[kVertical] = vOrigin;
			displaySpecsPtr->origin[kHorizontal] = hOrigin;

			}	// end "if (numberImageOverlays > 0 || numberVectorOverlays > 0 || ... "
		
				// The titles for side by side images are handled by paint events in
				// the TitleBar class
		
				// Not clear this is needed. Does not seem to work anyway.
				
		//pDC->DestroyClippingRegion ();
		
		}	// end "if (windowInfoPtr->drawBitMapFlag)"

}	// end "CopyOffScreenImage"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CreateClassNameComboBoxList
//
//	Software purpose:	This routine creates a class name list in the input combo
//							box list.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/10/1996
//	Revised By:			Larry L. Biehl			Date: 11/21/2019

void CreateClassNameComboBoxList (
				wxChoice*							classNameCtrl)

{
	SignedByte							handleStatus;
	UInt16								classIndex,
											classStorage;
	

			// Clear method in wxControlwithitems
	
	classNameCtrl->Clear ();

			// Add current list of classes to the combo box.

   if (gProjectInfoPtr->numberStatisticsClasses < kMaxNumberStatClasses - 1)
		classNameCtrl->Append (wxT("New"));

			// Make certain that the pointer to class names structure is good.

	HPClassNamesPtr classNamesPtr = (HPClassNamesPtr)GetHandleStatusAndPointer (
											gProjectInfoPtr->classNamesHandle, &handleStatus);

	for (classIndex=0;
				classIndex<gProjectInfoPtr->numberStatisticsClasses;
					classIndex++)
		{
				// Get the class storage number.

		classStorage = gProjectInfoPtr->storageClass[classIndex];
		wxString newstr ((char*)&classNamesPtr[classStorage].name[1], wxConvUTF8);
		classNameCtrl->Append (newstr);

		}	// end "for (classIndex=0; classIndex<..."

}	// end "CreateClassNameComboBoxList"


/*
void GetApplicationStartupPath (
				char*									startupPathPtr,
				SInt32*								sizeOfPathPtr)

{
	char*									executableFilePathPtr;

   SInt32								i,
											length,
											stringStartIndex;


	*sizeOfPathPtr = 0;

	//wxStandardPaths std;
	wxString exepath = (wxStandardPaths::Get ()).GetExecutablePath ();
	strncpy (executableFilePathPtr,
				(const char*)exepath.mb_str (wxConvUTF8),
				exepath.Len ());
	//executableFilePathPtr = (char*)exepath.mb_str (wxConvUTF8);

	if (executableFilePathPtr != NULL) 
		{
				// Remove the '"' from the beginning and end of the string.

      stringStartIndex = 0;
      if (executableFilePathPtr[0] == '"')
			stringStartIndex = 1;

		strcpy (startupPathPtr, &executableFilePathPtr[stringStartIndex]);

				// Now remove the executable name. Go backward from
				// end of string to find the first '\'.

		length = (SInt32)strlen (startupPathPtr);
		for (i = length; i >= 0; i--) 
			{
			if (startupPathPtr[i] == '\\')
				break;

			}	// end "for (i=length; i>=0; i--)"

      startupPathPtr[i + 1] = 0;
		*sizeOfPathPtr = i + 1;
 
				// Now change the '\' characters to '/'. This is done
				// for gdal routines. This approach will need to be changed
				// if other libraries need it differently.
				// Determined that this is not needed.

		//for (i=0; i<*sizeOfPathPtr; i++)
		//	{
		//	if (startupPathPtr[i] == '\\')
		//	startupPathPtr[i] = '/';
		//
		//	}	// end "for (i=0; i<*sizeOfPathPtr; i++)"
 
		}	// end "if (executableFilePathPtr != NULL)"

}	// end "GetApplicationStartupPath" 
*/


void EndBitMapRawDataAccess (
				wxBitmap*							bitMapPtr)

{
	#if defined multispec_wxmac
		bitMapPtr->GetBitmapData()->EndRawAccess ();
	#endif
	
}	// end "EndBitMapRawDataAccess"



SInt16 GetComboListSelection (
				wxWindow*							cWindowPtr,
				UInt16								dialogItemNumber,
				SInt32								selectedListItemData)

{
	wxComboBox*							comboBoxPtr;
	wxWindow*							wptr;

	wxChoice*							choiceptr;
	UInt32								index,
											numberComboItems;

	SInt16								comboListSelection;


	comboListSelection = -1;

	wptr = cWindowPtr->FindWindow (dialogItemNumber);
           
   if (wxDynamicCast (wptr, wxComboBox) != NULL)
		{
				// if it's wxComboBox
		
      comboBoxPtr = (wxComboBox*)wptr;
      if (comboBoxPtr != NULL)
			{
         numberComboItems = (UInt32)comboBoxPtr->GetCount ();
         
         for (index=0; index<numberComboItems; index++)
				{
            if (comboBoxPtr->GetClientData (index))
					{
               SInt64 windowIndex64 =
											(SInt64)((int*)comboBoxPtr->GetClientData (index));
               SInt32 windowIndex = (SInt32)windowIndex64;
               if (windowIndex == selectedListItemData)
						comboListSelection = (SInt16)index;
					
               }	// end "if (comboBoxPtr->GetClientData (index))"
				
				}	// end "for (index=0; index<numberComboItems; index++)"
         
			}	// end "if (comboBoxPtr != NULL)"
		
		}	// end "if (wxDynamicCast (wptr, wxComboBox) != NULL)"
	
	else if (wxDynamicCast (wptr, wxChoice) != NULL)
		{
				// If it's wxChoice
		
      choiceptr = (wxChoice*)wptr;
      if (choiceptr != NULL)
			{
         numberComboItems = (UInt32)choiceptr->GetCount ();
         for (index = 0; index < numberComboItems; index++)
				{
            if (choiceptr->GetClientData (index))
					{
               SInt64 windowIndex64= (SInt64)((int*)choiceptr->GetClientData (index));
               SInt32 windowIndex = (SInt32) windowIndex64;   
               if (windowIndex == selectedListItemData)
						comboListSelection = (SInt16)index;

               }	// end "if (choiceptr->GetClientData (index))"
				
				}	// end "for (index=0; index<numberComboItems; index++)"

			}	// end "if (comboBoxPtr != NULL)"
		
		}	// end "else if (wxDynamicCast (wptr, wxChoice) != NULL)"
   
   return (comboListSelection);

}	// end "GetComboListSelection"



void GetMenuItemText (
				MenuHandle							menuHandle,
				SInt16								menuItem,
				UCharPtr								stringPtr)

{
   //SInt16 stringLength = GetMenuString (menuItem, stringPtr, 255, MF_BYPOSITION);

}	// end "GetMenuItemText"



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
// Called By:			SetUpColorImageMemory in SDisplay.cpp
//							ConvertMultispectralToThematic in SFieldsToThematicsFile.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/07/1991
//	Revised By:			Larry L. Biehl			Date: 03/22/2019

SInt16 GetOffscreenGWorld (
				WindowInfoPtr						windowInfoPtr,
				DisplaySpecsPtr					displaySpecsPtr,
				LongRect*							longRectPtr,
				UInt32*								pixRowBytesPtr)

{
	UInt32								bytesNeeded;

	SInt16								numberPaletteEntries = 0,
											resultCode = noErr;

	int									pixelSize;

			// Now get memory for the bit map and storage to be used for image.

	numberPaletteEntries = displaySpecsPtr->paletteObject->GetNumberPaletteEntries ();
	pixelSize = displaySpecsPtr->pixelSize;
	windowInfoPtr->rowBytes = 0;

	if (windowInfoPtr->imageType == kThematicImageType ||
					displaySpecsPtr->displayType == k1_ChannelThematicDisplayType)
		{
				// Get memory for storage for (indexed) thematic image
		
		*pixRowBytesPtr = (UInt32)longRectPtr->right - longRectPtr->left;

		bytesNeeded =
				*pixRowBytesPtr * ((UInt32)longRectPtr->bottom - longRectPtr->top + 1);
		
		if (windowInfoPtr->offscreenMapSize > 0)
			UnlockAndDispose (windowInfoPtr->imageBaseAddressH);
		
		windowInfoPtr->imageBaseAddressH = MNewHandle (bytesNeeded);
		windowInfoPtr->offscreenMapSize = bytesNeeded;
		
		displaySpecsPtr->paletteObject->SetPaletteLoadedFlag (FALSE);

		}	// end "if (windowInfoPtr->imageType == kThematicImageType || ..."

	else	// multispectral image type
		{
		numberPaletteEntries = 0;
		
				// Dispose of memory allocated for a previous thematic image
		
		if (windowInfoPtr->offscreenMapSize > 0)
			UnlockAndDispose (windowInfoPtr->imageBaseAddressH);
		
		windowInfoPtr->imageBaseAddressH = NULL;
		windowInfoPtr->offscreenMapSize = 0;
		
		}	// else multispectral image type

			// 24 bits is used for wxBitmaps for multispectral windows since transparency is
			// not used

	#if defined multispec_wxlin
		#if defined multispec_wxlin_alpha
			pixelSize = 32;
		#else
			pixelSize = 24;
		#endif
	#endif

	#if defined multispec_wxmac
				// Note Mac OS bitmaps seem to always be 32 bits
		#if defined multispec_wxmac_alpha
			pixelSize = 32;
		#else
			pixelSize = 32;
		#endif
	#endif

	wxBitmap & scaledBitmap = gActiveImageViewCPtr->m_ScaledBitmap;
	scaledBitmap.Create (longRectPtr->right, longRectPtr->bottom, pixelSize);
	
	//wxGraphicsRenderer* graphicsRenderer = wxGraphicsRenderer::GetDefaultRenderer ();
	//gActiveImageViewCPtr->m_ScaledGraphicsBitmap = graphicsRenderer->CreateBitmap (scaledBitmap);
	if (scaledBitmap.IsOk ())
		{
		if (numberPaletteEntries == 0)
			{
					// This is for multispectral image windows. Colors will be copied to the
					// bitmap directly.
			
			#if defined multispec_wxmac
						// Code says to use GetRawData but it did not work correctly.
				//wxPixelData data;
				//windowInfoPtr->imageBaseAddressH = (Handle)scaledBitmap.GetRawData (data, 32);
				//windowInfoPtr->imageBaseAddressH = (Handle)scaledBitmap.GetBitmapData()->BeginRawAccess ();
				windowInfoPtr->imageBaseAddressH = (Handle)scaledBitmap.GetRawAccess ();
				//windowInfoPtr->imageBaseAddressH = (Handle)gActiveImageViewCPtr->m_ScaledGraphicsBitmap.GetNativeBitmap ();
				*pixRowBytesPtr = scaledBitmap.GetBitmapData()->GetBytesPerRow ();
			#endif
			
			#if defined multispec_wxlin
				#if defined multispec_wxlin_alpha
					wxAlphaPixelData pixeldata (scaledBitmap);
				#else
					wxNativePixelData pixeldata (scaledBitmap);
				#endif
				windowInfoPtr->imageBaseAddressH = (unsigned char*)pixeldata.GetPixels().m_ptr;
				*pixRowBytesPtr = pixeldata.GetRowStride ();
			#endif
			
					// This will indicate that the image base address is to a wxBitmap
			
			windowInfoPtr->offscreenMapSize = 0;
			windowInfoPtr->rowBytes = *pixRowBytesPtr;
			/*
			int numberChars = sprintf ((char*)gTextString3,
													" xUtilities.cpp: (imageBaseAddressH, IsOk, pixRowBytes): %ld, %d, %d%s",
													windowInfoPtr->imageBaseAddressH,
													scaledBitmap.IsOk (),
													*pixRowBytesPtr,
													gEndOfLine);
			ListString ((char*)gTextString3, numberChars, gOutputTextH);
			*/
			}	// end "if (numberPaletteEntries == 0)"
		
		}	// end "if (scaledBitmap.IsOk ()"

	if (windowInfoPtr->imageBaseAddressH == NULL)
		{
		unsigned char message[] = " Not able to create bitmap for the image. Try reducing the number of columns/lines or increasing the column/line interval.";
		DisplayAlert (kErrorAlertID,
							kStopAlert,
							0,
							0,
							0,
							message);
		
		}	// end "if (windowInfoPtr->imageBaseAddressH == NULL)"

			// Initialize variable for initial thematic image updates in case
			// they are needed.

	displaySpecsPtr->updateStartLine = -1;
	displaySpecsPtr->updateEndLine = -1;

	if (windowInfoPtr->imageBaseAddressH == NULL)
		resultCode = 1;

	if (resultCode != noErr)
		windowInfoPtr->offscreenMapSize = 0;

	return (resultCode);

}	// end "GetOffscreenGWorld"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		wxRegion GetPolygonEnclosingRectangle
//
//	Software purpose:	The purpose of this routine is to obtain the smallest
//							rectangle in window units that includes the boundary of the
//							first and last polygon points plus the new point the user is
//							deciding to select.
//							This routine is used to define the area on the window to
//							invalidate.
//
//	Parameters in:
//
//	Parameters out:
//
//	Value Returned:
//
// Called By:
//
//	Global Data:
//
//	Coded By:			Larry L. Biehl			Date: 04/02/2019
//	Revised By:			Larry L. Biehl			Date: 04/05/2019

Boolean GetPolygonEnclosingRectangle (
				HPFieldPointsPtr 					selectionPointsPtr,
				int									numberPoints,
            LCToWindowUnitsVariables*		lcToWindowUnitsVariablesPtr,
				wxPoint*								currentPointPtr,
            wxRect*								rectPtr)

{
	LongPoint 							windowPoint;
	wxPoint 								pointsPtr[3];
	
	int									maxX,
											maxY;
	
	Boolean								returnFlag = true;
	

			// Get the first and last points
	
	ConvertLCToWinPoint ((LongPoint*)&selectionPointsPtr[0],
												&windowPoint,
												lcToWindowUnitsVariablesPtr);
	pointsPtr[0].x = (int)windowPoint.h;
	pointsPtr[0].y = (int)windowPoint.v;
	
	ConvertLCToWinPoint ((LongPoint*)&selectionPointsPtr[numberPoints-1],
												&windowPoint,
												lcToWindowUnitsVariablesPtr);
	pointsPtr[1].x = (int)windowPoint.h;
	pointsPtr[1].y = (int)windowPoint.v;
	
	pointsPtr[2].x = (int)currentPointPtr->x;
	pointsPtr[2].y = (int)currentPointPtr->y;
			
	rectPtr->x = MIN (pointsPtr[0].x, pointsPtr[2].x);
	rectPtr->x = MIN (rectPtr->x, pointsPtr[1].x);
	
	rectPtr->y = MIN (pointsPtr[0].y, pointsPtr[2].y);
	rectPtr->y = MIN (rectPtr->y, pointsPtr[1].y);
	
	maxX = MAX (pointsPtr[0].x, pointsPtr[1].x);
	maxX = MAX (maxX, pointsPtr[2].x);
	rectPtr->width = maxX - rectPtr->x + 1;
	
	maxY = MAX (pointsPtr[0].y, pointsPtr[1].y);
	maxY = MAX (maxY, pointsPtr[2].y);
	rectPtr->height = maxY - rectPtr->y + 1;
	
			// Add some extra. 3 seemed to be the magic number.
	
	rectPtr->x -= 3;
	rectPtr->y -= 3;
	rectPtr->width += 6;
	rectPtr->height += 6;
	
	return (returnFlag);

}	// end "GetPolygonEnclosingRectangle"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetScreenRect
//
//	Software purpose:	The purpose of this routine is to obtain the rectangle that
//							describes the size of the screen with the mouse cursor.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:
//
// Called By:																
//										
//	Global Data:
//
//	Coded By:			Larry L. Biehl			Date: 06/29/1995
//	Revised By:			Larry L. Biehl			Date: 06/29/1995	
// ToDo: For Linux

void GetScreenRect (
				Rect*									screenRectPtr)

{
	
}	// end "GetScreenRect"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetScrollOffset
//
//	Software purpose:	The purpose of this routine is to obtain the scroll offset
//							for the window represented by the input window information
//							handle.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:
//
// Called By:																
//										
//	Global Data:
//
//	Coded By:			Larry L. Biehl			Date: 05/23/1996
//	Revised By:			Larry L. Biehl			Date: 07/21/1998	
// ToDo: For Linux

void GetScrollOffset (
				Handle								windowInfoHandle,
				LongPoint*							longPointPtr)

{
	WindowInfoPtr						windowInfoPtr;
	
	
	longPointPtr->v = 0;
	longPointPtr->h = 0;

	windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	if (windowInfoPtr != NULL)
		{
		if (windowInfoPtr->cImageWindowPtr != NULL)
			{
			CMImageView* imageViewCPtr =
											windowInfoPtr->cImageWindowPtr->mImageViewCPtr;
			if (imageViewCPtr != NULL)
				{
				wxPoint scrollOffset = imageViewCPtr->m_Canvas->GetScrollPosition ();
				longPointPtr->v = scrollOffset.y;
				longPointPtr->h = scrollOffset.x;
				
				}	// end "if (imageViewCPtr != NULL)"
			
			}	// end "if (windowInfoPtr->cImageWindowPtr != NULL)"
		
		}	// end "if (windowInfoPtr != NULL)"
	
}	// end "GetScrollOffset"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void NumToString
//
//	Software purpose:	This routine is a Windows substitute for the
//							function of the same name in the Mac Toolbox.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/27/1995
//	Revised By:			Larry L. Biehl			Date: 06/27/1995	

void NumToString (
				SInt32								numberValue,
				UCharPtr								stringPtr)

{
   sprintf ((CharPtr)&stringPtr[1], "%d", (int)numberValue);

	stringPtr[0] = strlen ((CharPtr)&stringPtr[1]);

}	// end "NumToString"   


/*
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void NumToString
//
//	Software purpose:	This routine is a Windows substitute for the
//							function of the same name in the Mac Toolbox.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/27/1995
//	Revised By:			Larry L. Biehl			Date: 06/27/1995	
							               	                
void	NumToString (
				UInt32								numberValue,
				UCharPtr								stringPtr)
 
{
	sprintf ((CharPtr)&stringPtr[1], "%lu", numberValue);
 
	stringPtr[0] = strlen ((CharPtr)&stringPtr[1]);

}	// end "NumToString"
*/



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		CharPtr PtoCstr
//
//	Software purpose:	This routine is a Windows substitute for the
//							function of the same name in the Mac Toolbox.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/19/1995
//	Revised By:			Larry L. Biehl			Date: 04/19/1995			

CharPtr PtoCstr (
				UCharPtr								textStringPtr)

{
   if (textStringPtr != NULL)
		{
      size_t bytesToMove = *textStringPtr;

      if (bytesToMove > 0)
			{
         memmove (textStringPtr, &textStringPtr[1], bytesToMove);
			textStringPtr[bytesToMove] = '\0';

			}	// end "if (bytesToMove > 0)"

		}	// end "if (textStringPtr != NULL)"

   return (CharPtr) textStringPtr;

}	// end "PtoCstr"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		CharPtr StringToNum
//
//	Software purpose:	This routine is a Windows substitute for the
//							function of the same name in the Mac Toolbox.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 04/19/1995
//	Revised By:			Larry L. Biehl			Date: 12/21/2016			

void StringToNum (
				ConstStr255Param					theString,
				SInt32*								theNumPtr)

{
	CharPtr stringPtr = (CharPtr) theString;

			// Force a c string terminator.

	stringPtr[stringPtr[0]+1] = '\0';
	*theNumPtr = atol (&stringPtr[1]);

}	// end "StringToNum"



SInt16 TextWidth (
				UCharPtr								stringPtr,
				SInt16								firstByte,
				SInt16								byteCount)

{
	wxSize								size;

	if (gCDCPointer != NULL)
		{
		wxString string = wxString::FromAscii (&stringPtr[firstByte]);
		size = gCDCPointer->GetTextExtent (string);
		
		}	// end "if (gCDCPointer != NULL)"

	else	// gCDCPointer == NULL
		size.x = byteCount * 6;

	return ((SInt16) size.x);
	
}	// end "TextWidth"



void SetUpToolTip (
				wxControl*							control,
				SInt16								stringnumber)

{
   GetSpecifiedStringNumber (kFileIOStrID, stringnumber, (UCharPtr)gTextString, TRUE);
	wxString wdcard ((char*)&gTextString[1], wxConvUTF8);
   control->SetToolTip (wdcard);
	
}	// end "SetUpToolTip"
