//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WUtilities.cpp
//
//	Authors:					Larry L. Biehl, Tsung Tai Yeh
//
//	Revision number:		3.0
//
//	Revision date:			01/03/2018
//
//	Language:				C
//
//	System:					Linux and Windows Operating Systems
//
//	Brief description:	The purpose of the routines in this file is to provide
//								utility type functions in MultiSpec that are common for
//								Linux and Windows.
//
//	Functions in file:	
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include	<ctype.h>  

#include "CImageWindow.h"
#include "CDisplay.h"

#include "WTextView.h"
#include "WImageView.h"

//#include "SExternalGlobals.h"

extern void GetApplicationStartupPath (
				char*									startupPathPtr,
				SInt32*								sizeOfPathPtr);



		// Prototypes for routines in this file that are only called by other
		// routines in this file.



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
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
// Called By:			DoUpdateEvent
//
//	Coded By:			Larry L. Biehl			Date: 08/31/1988
//	Revised By:			Larry L. Biehl			Date: 06/01/2017		
// TODO: For Linux

void CopyOffScreenImage (
				CMImageView*						imageViewCPtr,
				CDC*									pDC,
				CMImageWindow*						imageWindowCPtr,
				RgnHandle							invalidRgn,
				Rect*									inSourceRect,
						// see descriptions in cases for switch statement below.
				SInt16								copyType)

{
	SCROLLINFO							scrollInfo;

   double								magnification;

   LongRect								lDestinationRect,
											lSourceRect;

   Rect									destinationRect,
											sourceRect,
											windowRect;

   CMPalette*							paletteCPtr;
	
	CPoint								scrollOffset;
	
   Handle								displaySpecsHandle,
											imageBaseAddressH,
											offScreenMapHandle;

   WindowInfoPtr						windowInfoPtr;

   DisplaySpecsPtr					displaySpecsPtr;

   SInt32								hOrigin,
											numberImageOverlays,
											vOrigin;

   UInt32								numberOverlays;

   SInt16								grafPortType,
											xDimension,
											yDimension;

   SInt16								legendWidth,
											titleHeight,
											windowCode;

   Boolean								drawBaseImageFlag,
											drawVectorOverlaysFlag,
											projectWindowFlag;


   if (imageViewCPtr->CheckIfOffscreenImageExists ())
		{
				// Intialize some local variables.

      windowInfoPtr = (WindowInfoPtr) GetHandlePointer (
															imageWindowCPtr->GetWindowInfoHandle ());
		
      legendWidth = 0;

      titleHeight = windowInfoPtr->titleHeight;
      grafPortType = windowInfoPtr->grafPortType;
      numberImageOverlays = windowInfoPtr->numberImageOverlays;
      drawVectorOverlaysFlag = windowInfoPtr->drawVectorOverlaysFlag;
      numberOverlays = windowInfoPtr->numberOverlays;
      drawBaseImageFlag = windowInfoPtr->drawBaseImageFlag;

      offScreenMapHandle = windowInfoPtr->offScreenMapHandle;
      imageBaseAddressH = windowInfoPtr->imageBaseAddressH;

      projectWindowFlag = windowInfoPtr->projectWindowFlag;

      GetWindowClipRectangle (imageViewCPtr, kImageArea, &windowRect);

				// Get handle and pointer to display specification for window

      displaySpecsHandle = imageViewCPtr->m_displayMultiCPtr->mDisplaySpecsHandle;
      displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (displaySpecsHandle);

      magnification = displaySpecsPtr->magnification;
      vOrigin = displaySpecsPtr->origin[0];
      hOrigin = displaySpecsPtr->origin[1];
      yDimension = (SInt16) displaySpecsPtr->imageDimensions[0];
      xDimension = (SInt16) displaySpecsPtr->imageDimensions[1];
      
      paletteCPtr = displaySpecsPtr->paletteObject;
      if (((imageViewCPtr != gActiveImageViewCPtr) || gInBackground) &&
              displaySpecsPtr->backgroundPaletteObject != NULL)
         paletteCPtr = displaySpecsPtr->backgroundPaletteObject;

      if (copyType != kClipboardCopy && copyType != kPrinterCopy)
			{
			imageViewCPtr->GetScrollInfo (SB_HORZ, &scrollInfo, SIF_POS);
			scrollOffset.x = scrollInfo.nPos;
			imageViewCPtr->GetScrollInfo (SB_VERT, &scrollInfo, SIF_POS);
			scrollOffset.y = scrollInfo.nPos;
			
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
         case kClipboardCopy:
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

         case kSourceCopy:
						// Map to wherever sourceRect * magnification indicates.  This
						// allows one to draw parts of the window at a time.

            lSourceRect.top = inSourceRect->top;
            lSourceRect.left = inSourceRect->left;
            lSourceRect.bottom = inSourceRect->bottom;
            lSourceRect.right = inSourceRect->right;

            lDestinationRect.top =
									(SInt32)(lSourceRect.top * magnification + titleHeight);
            lDestinationRect.left =
									(SInt32)(lSourceRect.left * magnification + legendWidth);
            lDestinationRect.bottom =
									(SInt32)(lSourceRect.bottom * magnification + titleHeight);
            lDestinationRect.right =
									(SInt32)(lSourceRect.right * magnification + legendWidth);
            break;

         case kScrollCopy: // scroll copy
						// Copy portion of offscreen image map that fits in the invalid
						// region of the window. 	This is from a scroll operation.

            //lDestinationRect = (*invalidRgn)->rgnBBox;

         case kDestinationCopy:
						// Copy portion of offscreen map that fits within the input
						// destination area of the window.  This is from a window update
						// event.

            if (copyType == kDestinationCopy)
					{
               lDestinationRect.top = inSourceRect->top;
               lDestinationRect.left = inSourceRect->left;
               lDestinationRect.bottom = inSourceRect->bottom;
               lDestinationRect.right = inSourceRect->right;
               
					}	// end "if (copyType == kDestinationCopy)"

						// Make certain that the legend is not included.

				lDestinationRect.top = MAX (titleHeight + scrollOffset.y,
														lDestinationRect.top);
				lDestinationRect.left = MAX (legendWidth, lDestinationRect.left);

				lSourceRect.top = (SInt32)(
									(lDestinationRect.top - titleHeight) / magnification);
				lSourceRect.left = (SInt32)(
									(lDestinationRect.left - legendWidth) / magnification);
				lSourceRect.bottom = (SInt32)(
						(lDestinationRect.bottom - titleHeight) / magnification + .9999);
				lSourceRect.right = (SInt32)(
						(lDestinationRect.right - legendWidth) / magnification + .9999);

				lSourceRect.bottom = MIN (lSourceRect.bottom, yDimension);
				lSourceRect.right = MIN (lSourceRect.right, xDimension);

						// Make certain that source to destination is correct.  The
						// source may have been truncated to a larger value.
				
				lDestinationRect.top = (SInt32)
										(titleHeight + magnification * lSourceRect.top + .5);
				lDestinationRect.bottom = (SInt32)
									(titleHeight + magnification * lSourceRect.bottom + .5);
				lDestinationRect.left = (SInt32)
									(legendWidth + magnification * lSourceRect.left + .5);
				lDestinationRect.right = (SInt32)
									(legendWidth + magnification * lSourceRect.right + .5);
				break;	// end "case: kDestinationCopy"

			case kPrinterCopy:
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

			}	// end "switch (copyType)"

		LPBITMAPINFO lpDIBHdr; // Pointer to BITMAPINFOHEADER
		LPSTR lpDIBBits; // Pointer to DIB bits
		SInt16 bSuccess = 0; // Success/fail flag
		CPalette* oldPalette = NULL; // Previous palette
		HDC hDC;

				// Lock down the DIB, and get a pointer to the beginning of the bit
				// buffer

		lpDIBHdr = (LPBITMAPINFO)::GlobalLock ((HGLOBAL) offScreenMapHandle);
					lpDIBBits = (LPSTR)::GlobalLock ((HGLOBAL) imageBaseAddressH);

					// Get the DIB's palette, then select it into DC

		if (paletteCPtr != NULL)
			{
					// Select as background since we have
					// already realized in forground if needed 

			oldPalette = pDC->SelectPalette (paletteCPtr, TRUE);

			}	// end "if (paletteCPtr != NULL)"   

		sourceRect.top = (int) lSourceRect.top;
		sourceRect.left = (int) lSourceRect.left;
		sourceRect.bottom = (int) lSourceRect.bottom;
		sourceRect.right = (int) lSourceRect.right;

		destinationRect.top = (int) lDestinationRect.top;
		destinationRect.left = (int) lDestinationRect.left;
		destinationRect.bottom = (int) lDestinationRect.bottom;
		destinationRect.right = (int) lDestinationRect.right;

		/*
		if (copyType == kPrinterCopy)
			{   		
					// Set clipping area for the print window.
					
			gMFC_Rgn.CreateRectRgn (destinationRect.left,
												destinationRect.top,
												destinationRect.right,
												destinationRect.bottom); 

			ClipRect (&destinationRect); 

			RECT clipBox; 

			gCDCPointer->GetClipBox (&clipBox); 

			gMFC_Rgn.DeleteObject (); 

			}	// end "if (copyType == kPrinterCopy)"  
		*/
				// Adjust for inverting so that the bottom line will be selected

		//sourceRect.top += 1;
		//sourceRect.bottom += 1;

				// Make sure to use the stretching mode best for color pictures

		//::SetStretchBltMode (pDC->m_hDC, STRETCH_DELETESCANS);	// COLORONCOLOR
		pDC->SetStretchBltMode (STRETCH_DELETESCANS);

		int destinationRectWidth = RECTWIDTH (&destinationRect);
		int destinationRectHeight = RECTHEIGHT (&destinationRect);
		int sourceRectWidth = RECTWIDTH (&sourceRect);
		int sourceRectHeight = RECTHEIGHT (&sourceRect);

				// Now take into account that the device independant bitmap is
				// is stored in memory in reverse order last line to first line.

		sourceRect.top = (int) lpDIBHdr->bmiHeader.biHeight - sourceRect.bottom;

		if (drawBaseImageFlag)
			{
			hDC = pDC->GetSafeHdc ();

					// Determine whether to call StretchDIBits () or SetDIBitsToDevice ()
			/*		
			if ((RECTWIDTH (&destinationRect) == RECTWIDTH (&sourceRect)) &&
								(RECTHEIGHT (&destinationRect) == RECTHEIGHT (&sourceRect)))
				bSuccess = ::SetDIBitsToDevice (
										hDC,             						// hDC
										destinationRect.left,        		// DestX
										destinationRect.top,         		// DestY
										RECTWIDTH (&destinationRect), 		// nDestWidth
										RECTHEIGHT (&destinationRect), 	// nDestHeight
										sourceRect.left,            		// SrcX 
										sourceRect.top, 						// SrcY
										sourceRect.top,                  // nStartScan
										sourceRect.bottom - sourceRect.top + 1, // nNumScans
										lpDIBBits,                  		// lpBits
										lpDIBHdr,     							// lpBitsInfo
										DIB_RGB_COLORS);            		// wUsage

			else	// source and destination rectangles are not same size.
				{
			*/
			bSuccess = ::StretchDIBits (hDC, // hDC
													destinationRect.left, // DestX
													destinationRect.top, // DestY
													destinationRectWidth, // nDestWidth
													destinationRectHeight, // nDestHeight
													sourceRect.left, // SrcX
													sourceRect.top, // SrcY
													sourceRectWidth, // wSrcWidth
													sourceRectHeight, // wSrcHeight
													lpDIBBits, // lpBits
													lpDIBHdr, // lpBitsInfo
													DIB_PAL_COLORS, // wUsage
													SRCCOPY); // dwROP 

			}	// end "if (drawBaseImageFlag)"

		if (legendWidth > 0 &&
					(copyType == kClipboardCopy || copyType == kPrinterCopy))
			gActiveImageViewCPtr->DrawLegend ();


		::GlobalUnlock ((HGLOBAL) offScreenMapHandle);
		::GlobalUnlock ((HGLOBAL) imageBaseAddressH);

					// Reselect old palette

			if (oldPalette != NULL && paletteCPtr != oldPalette)
				pDC->SelectPalette (oldPalette, TRUE);

		if (numberImageOverlays > 0 || numberOverlays > 0 || projectWindowFlag)
			{
			windowCode = 1;
			if (copyType == kClipboardCopy)
				windowCode = 3;
			else if (copyType == kPrinterCopy)
				windowCode = 2;

			if (windowCode >= 2)
				{
				displaySpecsPtr = (DisplaySpecsPtr) GetHandlePointer (displaySpecsHandle);

				displaySpecsPtr->origin[kVertical] = (SInt16) lSourceRect.top;
				displaySpecsPtr->origin[kHorizontal] = (SInt16) lSourceRect.left;

				}	// end "if (windowCode >= 2)"

					// Draw the image overlay if they exist.

			if (numberImageOverlays > 0)
				DrawImageOverlays (imageViewCPtr,
											imageWindowCPtr->GetWindowInfoHandle (),
											NULL,
											NULL,
											&destinationRect,
											&sourceRect,
											windowCode);

						// Draw the vector overlays if they exist.

			if (drawVectorOverlaysFlag && numberOverlays > 0)
				{
				DrawArcViewShapes (imageViewCPtr,
											imageWindowCPtr->GetWindowInfoHandle (),
											&destinationRect,
											windowCode,
											NULL);
					
				}	// end "if (drawVectorOverlaysFlag && numberOverlays > 0)"

					// If this window is the project window, outline the fields if
					// requested.

			if (projectWindowFlag)
				OutlineFieldsControl (gProjectInfoPtr->statsWindowMode,
												imageViewCPtr,
												imageWindowCPtr->GetWindowInfoHandle (),
												windowCode);
				
			if (windowCode >= 2)
				{
				displaySpecsPtr =
									(DisplaySpecsPtr)GetHandlePointer (displaySpecsHandle);

				displaySpecsPtr->origin[kVertical] = vOrigin;
				displaySpecsPtr->origin[kHorizontal] = hOrigin;

				}	// end "if (windowCode >= 2)"

			}	// end "if (numberOverlays > 0 || projectWindowFlag)"
         		
				// Draw the selection area, if it exists for this window.			
      /*		
		if (copyType != kClipboardCopy)
			OutlineSelectionArea (theWindow);
		*/
				// Handle title line for side-by-side images.							

      if (titleHeight > 0)
			{
         if (copyType == kClipboardCopy || copyType == kPrinterCopy)
				{
            destinationRect.top = 0;
				destinationRect.left = 0;
				destinationRect.bottom =
							(SInt16)(titleHeight * imageViewCPtr->m_printerTextScaling + .5);

				#if defined multispec_win
					gMFC_Rgn.CreateRectRgn (destinationRect.left,
													  destinationRect.top,
													  destinationRect.right,
													  destinationRect.bottom);
				#endif	// defined multispec_win

            if (copyType == kClipboardCopy)
					ClipRect (&destinationRect);

            }	// end "if (copyType == kClipboardCopy || copyType == kPrinterCopy)" 

         else // copyType != kClipboardCopy && copyType != kPrinterCopy)
				{
            destinationRect = windowRect;
				destinationRect.top = 0;
				destinationRect.bottom = titleHeight;
				
				#if defined multispec_win
					gMFC_Rgn.CreateRectRgn (destinationRect.left,
					destinationRect.top,
					destinationRect.right,
					destinationRect.bottom);
				#endif	// defined multispec_win

				ClipRect (&destinationRect);

				destinationRect.top += scrollOffset.y;
				destinationRect.bottom += scrollOffset.y;

				}	// end "else copyType != kClipboardCopy && copyType != kPrinterCopy" 
			
         Handle windowInfoHandle = GetWindowInfoHandle (imageViewCPtr);
			DrawSideBySideTitles (windowInfoHandle,
										  imageViewCPtr,
										  &destinationRect,
										  windowCode);

			ClipRect (&windowRect);
         
			}	// end "if (titleHeight > 0)" 
			 
		gMFC_Rgn.DeleteObject ();
              
		//if (copyType != kClipboardCopy)
		//	ClipRect (&theWindow->portRect);

		}	// end "if (imageViewCPtr->CheckIfOffscreenImageExists ())"

}	// end "CopyOffScreenImage"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
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
//	Revised By:			Larry L. Biehl			Date: 03/29/2017		

void CreateClassNameComboBoxList (
				CComboBox*							comboBoxCPtr)

{
	SignedByte							handleStatus;
	UInt16								classIndex,
											classStorage;

	USES_CONVERSION;
	

	comboBoxCPtr->ResetContent ();

			// Add current list of classes to the combo box.

   if (gProjectInfoPtr->numberStatisticsClasses < kMaxNumberStatClasses - 1)
		comboBoxCPtr->AddString ((LPCTSTR)_T("'New'"));

			// Make certain that the pointer to class names structure is good.

	HPClassNamesPtr classNamesPtr = (HPClassNamesPtr) GetHandleStatusAndPointer (
												gProjectInfoPtr->classNamesHandle, &handleStatus);

	for (classIndex=0;
			classIndex<gProjectInfoPtr->numberStatisticsClasses;
				classIndex++)
		{
				// Get the class storage number.											

		classStorage = gProjectInfoPtr->storageClass[classIndex];

		comboBoxCPtr->AddString (
									(LPCTSTR)A2T((LPSTR)&classNamesPtr[classStorage].name[1]));

		}	// end "for (classIndex=0; classIndex<..."  

}	// end "CreateClassNameComboBoxList"



void GetApplicationStartupPath (
				char*									startupPathPtr,
				SInt32*								sizeOfPathPtr)

{
	USES_CONVERSION;
	
	TBYTE*								executableFilePathPtr;

   SInt32								i,
											length,
											stringStartIndex;


	*sizeOfPathPtr = 0;

	executableFilePathPtr = (TBYTE*)GetCommandLine ();

	if (executableFilePathPtr != NULL) 
		{
				// Remove the '"' from the beginning and end of the string.

      stringStartIndex = 0;
      if (executableFilePathPtr[0] == '"')
			stringStartIndex = 1;

		strcpy (startupPathPtr, T2A(&executableFilePathPtr[stringStartIndex]));

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
		/*
				// Now change the '\' characters to '/'. This is done
				// for gdal routines. This approach will need to be changed
				// if other libraries need it differently.
				// Determined that this is not needed.

		for (i=0; i<*sizeOfPathPtr; i++)
			{
			if (startupPathPtr[i] == '\\')
			startupPathPtr[i] = '/';

			}	// end "for (i=0; i<*sizeOfPathPtr; i++)"
		*/
		}	// end "if (executableFilePathPtr != NULL)"

}	// end "GetApplicationStartupPath" 



SInt16 GetComboListSelection (
				CWnd*									cWindowPtr,
				UInt16								dialogItemNumber,
				SInt32								selectedListItemData)
{
	CComboBox*							comboBoxPtr;

	UInt32								index,
											numberComboItems;

	SInt16								comboListSelection;


	comboListSelection = -1;

	comboBoxPtr = (CComboBox*)cWindowPtr->GetDlgItem (dialogItemNumber);

   if (comboBoxPtr != NULL)
		{
      numberComboItems = (UInt32)comboBoxPtr->GetCount ();
      for (index = 0; index < numberComboItems; index++)
			{
         if ((SInt32)comboBoxPtr->GetItemData ((SInt16)index) == selectedListItemData)
				comboListSelection = (SInt16)index;

         }	// end "for (index=0; index<numberComboItems; index++)" 

		}	// end "if (comboBoxPtr != NULL)"

   return (comboListSelection);

}	// end "GetComboListSelection" 



void GetMenuItemText (
				MenuHandle							menuHandle,
				SInt16								menuItem,
				UCharPtr								stringPtr)

{
   //SInt16 stringLength = GetMenuString (menuItem, stringPtr, 255, MF_BYPOSITION);

}	// end "GetMenuItemText" 


/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt GetPopUpMenuBitsPerDataValue
//
//	Software purpose:	The purpose of this routine is to determine the requested
//							bits per data value from the menu selection.
//
//	Parameters in:		None.
//
//	Parameters out:	None.
//
//	Value Returned:	The requested number of bits. 	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/20/1992
//	Revised By:			Larry L. Biehl			Date: 05/24/1995

UInt16 GetPopUpMenuBitsPerDataValue (
				UInt16								bitsPerDataValueSelection)

{                           
	UInt16								bitsPerDataValue;


			// 'Hardwire for now. It would be nice to go through the resource list so 
			// that the code would not have to be updated when changes are made.
 
	bitsPerDataValue = 0;              

	if (bitsPerDataValueSelection == 0)
			 bitsPerDataValue = 4;

	else if (bitsPerDataValueSelection == 1) 
			 bitsPerDataValue = 8; 

	else if (bitsPerDataValueSelection == 2) 
			 bitsPerDataValue = 10;

	else if (bitsPerDataValueSelection == 3) 
			 bitsPerDataValue = 12;

	else if (bitsPerDataValueSelection == 4) 
			 bitsPerDataValue = 13;

	else if (bitsPerDataValueSelection == 5) 
			 bitsPerDataValue = 14;

	else if (bitsPerDataValueSelection == 6) 
			 bitsPerDataValue = 15;

	else if (bitsPerDataValueSelection == 7) 
			 bitsPerDataValue = 16;

	if (bitsPerDataValue == 0)
			 bitsPerDataValue = 8;

	return (bitsPerDataValue);
	
}	// end "GetPopUpMenuBitsPerDataValue" 
*/


//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
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
// TODO: For Linux

void GetScreenRect (
				Rect*									screenRectPtr)

{
	#if defined multispec_win
		::GetWindowRect (::GetDesktopWindow (), (tagRECT*)screenRectPtr);
	#endif	// defined multispec_win
	
}	// end "GetScreenRect"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
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
// TODO: For Linux

void GetScrollOffset (
				Handle								windowInfoHandle,
				LongPoint*							longPointPtr)
{
	WindowInfoPtr						windowInfoPtr;
	
	SCROLLINFO							scrollInfo;


	longPointPtr->v = 0;
	longPointPtr->h = 0;

	windowInfoPtr = (WindowInfoPtr) GetHandlePointer (windowInfoHandle);

	if (windowInfoPtr != NULL)
		{
		if (windowInfoPtr->cImageWindowPtr != NULL)
			{
			CMImageView* imageViewCPtr =
											windowInfoPtr->cImageWindowPtr->mImageViewCPtr;

			if (imageViewCPtr != NULL)
				{
				CPoint scrollOffset;

				imageViewCPtr->GetScrollInfo (SB_HORZ, &scrollInfo, SIF_POS);
				scrollOffset.x = scrollInfo.nPos;
				imageViewCPtr->GetScrollInfo (SB_VERT, &scrollInfo, SIF_POS);
				scrollOffset.y = scrollInfo.nPos;

				longPointPtr->v = scrollOffset.y;
				longPointPtr->h = scrollOffset.x;

				}	// end "if (imageViewCPtr != NULL)"

			}	// end "if (windowInfoPtr->cImageWindowPtr != NULL)"

		}	// end "if (windowInfoPtr != NULL)"
	
}	// end "GetScrollOffset"


/*     
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 MGetLine
//
//	Software purpose:	The purpose of this routine is to call GetLine member of the
//							CMFileStream class.
//
//	Parameters in:		
//
//	Parameters out:	None.
//
//	Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/06/1995
//	Revised By:			Larry L. Biehl			Date: 09/06/1995	

SInt16 MGetLine (
				UInt32								lineNumber,
				SInt16								channelNumber,
				UInt32								columnStart,
				UInt32								columnEnd,
				UInt32*								numberSamplesPtr,
				HUCharPtr							fileIOBufferPtr,
				FileInfoPtr							fileInfoPtr,
				Boolean								BILSpecial)

{  
	SInt16								errCode;
	

			// Get the file stream pointer object for the input image window.

	CMFileStream* fileStreamPtr = fileInfoPtr->fileStreamCPtr;

	errCode = fileStreamPtr->GetLine (lineNumber,
												 channelNumber, 
												 columnStart,
												 columnEnd, 
												 numberSamplesPtr,
												 fileIOBufferPtr, 
												 fileInfoPtr,
												 BILSpecial);
 
	return (errCode);                    
 
}	// end "MGetLine" 
*/


//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
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
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
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
	CSize									size;

	USES_CONVERSION;

	if (gCDCPointer != NULL)
		size = gCDCPointer->GetTextExtent ((LPCTSTR)A2T((LPCSTR)&stringPtr[firstByte]), byteCount);

	else //
		size.cx = byteCount * 6;

	return ((SInt16) size.cx);

}	// end "TextWidth"

