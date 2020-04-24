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
//	File:						SStubs.cpp
//
//	Authors:					Larry L. Biehl, Abdur Maud
//
//	Revision date:			02/21/2020
//
//	Language:				C
//
//	System:					Linux, MacOS, and Windows Operating Systems
//								
//	Brief description:	The purpose of the routines in this file are to allow common
//								files across Linux, MacOS, and Windows versions. It was
//								originally to be for common files between all MultiSpec
//								versions in a trully c++ implementation. This was stopped
//								when Metrowerks stopped supporting the Mac; therefore their
//								class library could not be used.
//
/* Template for debugging
	int numberChars = sprintf ((char*)&gTextString3,
									" CStubs:CloseGraphicsWindow (gNumberOfGWindows): %d%s", 
									gNumberOfGWindows,
									gEndOfLine);
	ListString ((char*)&gTextString3, numberChars, gOutputTextH);	
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
	
#include "SImageWindow_class.h"
#include "SPalette_class.h"

#ifdef multispec_wx
	#include "wx/evtloop.h"
	#include "wx/menu.h"
	#include "wx/pen.h"
	#include "wx/wx.h"
	#include "xMultiSpec.h"
	#include "xMainFrame.h"
	#include "xImageView.h"
	#include "xDialog.h"
	
	#define CPen wxPen
#endif

#ifdef multispec_win
	//#include "CProcessor.h"
	
	#include "WMultiSpec.h"
	#include "WDialog.h" 
	#include "WGraphDoc.h"
	#include "WGraphFrame.h"
	#include "WGraphView.h"
	#include "WImageDoc.h"
	#include "WImageFrame.h"
	#include "WImageView.h"
	#include	"WStatisticsDoc.h"
	#include "WLegendView.h"  
#endif

extern void SetPenCharacteristics (
				CPen*									newPenPtr,
				RGBColor*							rgbColorPtr,
				SInt16								lineThickness);

extern void ENVI_ASCII_ROI_ToThematicFileControl (void);

#if defined multispec_win
	extern Handle GetCopyDIBHandle (
					CMImageView*						imageViewCPtr);
#endif

#if defined multispec_wx
	extern void MGetEntryColor (
					CMPaletteInfo						paletteObject, 
					UInt16								entry, 
					RGBColor*							theColorPtr);
#endif


void AnimateEntry (
				CMPaletteInfo						paletteObject,
				UInt16								startIndex,
				RGBColor*							theColorPtr)

{
	#if defined multispec_wx
	#endif	// defined multispec_wx

	#if defined multispec_win
		PALETTEENTRY						paletteEntry;


		paletteEntry.peRed = (UInt8)(theColorPtr->red >> 8);
		paletteEntry.peGreen = (UInt8)(theColorPtr->green >> 8);
		paletteEntry.peBlue = (UInt8)(theColorPtr->blue >> 8);
		paletteEntry.peFlags = PC_RESERVED;

		CDC* pDC = gActiveImageViewCPtr->GetDC ();
		CPalette* pOldPal = pDC->SelectPalette (paletteObject, FALSE);
		paletteObject->AnimatePalette (startIndex,
													1,
													&paletteEntry);

		pDC->RealizePalette ();

		pDC->SelectPalette (pOldPal, FALSE);
		gActiveImageViewCPtr->ReleaseDC (pDC);
	#endif	// defined multispec_win
	
}	// end "AnimateEntry"



void AppendMenu (
				MenuHandle							menuHandle,
				Str255								textString)
{

}	// end "AppendMenu"



void BlockMove (
				const void*							inStringPtr,
				void*									outStringPtr,
				UInt32								numberBytes)
				
{
	if (inStringPtr != NULL && outStringPtr != NULL) 
		{
		UInt32 count;
		HPtr lOutStringPtr = (HPtr)outStringPtr;
		HPtr lInStringPtr = (HPtr)inStringPtr;

		for (count=0; count<numberBytes; count++) 
			{
			*lOutStringPtr = *lInStringPtr;
			lOutStringPtr++;
			lInStringPtr++;

			}	// end "for (count=0; count<numberBytes; count++)"

		}	// end "if (inStringPtr != NULL && outStringPtr != NULL)"

}	// end "BlockMove"



void BlockMoveData (
				const void*							inStringPtr,
				void*									outStringPtr,
				UInt32								numberBytes)
		  
{
	if (inStringPtr != NULL && outStringPtr != NULL) 
		{
		UInt32 count;
		HPtr lOutStringPtr = (HPtr) outStringPtr;
		HPtr lInStringPtr = (HPtr) inStringPtr;

		for (count=0; count<numberBytes; count++) 
			{
			*lOutStringPtr = *lInStringPtr;
			lOutStringPtr++;
			lInStringPtr++;

			}	// end "for (count=0; count<numberBytes; count++)" 

		}	// end "if (inStringPtr != NULL && outStringPtr != NULL)"

}	// end "BlockMoveData"



SInt16 CharWidth (
				char									character)

{
	#if defined multispec_wx
		wxSize								size;
		char									characterString[2];
		wxString								wx_character;
		SInt16								charWidth;
		
		
		characterString[0] = character;
		characterString[1] = 0;
		wx_character = wxString::FromAscii (characterString);

		if (gCDCPointer != NULL)
			{
			size = gCDCPointer->GetTextExtent (wx_character);
			charWidth = size.GetWidth ();
			
			}	// end "if (gCDCPointer != NULL)"

		else	// gCDCPointer == NULL
		  charWidth = 6;

		return (charWidth);
	#endif	// defined multispec_wx

	#if defined multispec_win
		CSize									size;

		USES_CONVERSION;

		if (gCDCPointer != NULL)
		  size = gCDCPointer->GetTextExtent ((LPCTSTR)A2T((LPCSTR)&character), 1);

		else	//
		  size.cx = 6;

		return ((SInt16)size.cx);
	#endif	// defined multispec_win

}	// end "CharWidth"


#if defined multispec_win
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		CheckSomeEvents
//
//	Software purpose:	This routine is a stub for the Windows version.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/11/1995
//	Revised By:			Larry L. Biehl			Date: 12/07/2018

Boolean CheckSomeEvents (
				UInt16								code)
				
{
	Boolean								returnFlag = TRUE;
	
	HWND hWnd = NULL;
	MSG msgCur;


	if (gStatusDialogPtr != NULL)
		hWnd = gStatusDialogPtr->m_hWnd;

	while (::PeekMessage (&msgCur, NULL, 0, 0, PM_REMOVE))
		{
		if (msgCur.message == WM_KEYDOWN)
			{
			//if (msgCur.wParam == 0x001b)
			if (msgCur.wParam == VK_ESCAPE)
				{
				returnFlag = FALSE;
				code = keyDownMask;
				break;

				}	// end "if (msgCur.wParam == VK_ESCAPE)"

			}	// end "if (msgCur.message == WM_CHAR)"

		if (returnFlag && !IsDialogMessage (hWnd, &msgCur))
			{
			if (msgCur.message == WM_PAINT)
				{
				TranslateMessage (&msgCur);
				DispatchMessage (&msgCur);

				}	// end "if (msgCur.message == WM_PAINT)"

			else if (gOperationCanceledFlag)
						// This will come from a cancel selection in the status dialog box
				returnFlag = FALSE;
				break;

			}	// end "if (returnFlag && !IsDialogMessage (hWnd, &msgCur))"

		}	// end "while (::PeekMessage (..."

	if (!returnFlag)
		{
		gOperationCanceledFlag = TRUE;

		if (gAlertId != 0)
			gAlertReturnCode = DisplayAlert (gAlertId,
															kCautionAlert,
															gAlertStrID,
															gAlertStringNumber,
															0,
															NULL);

		if (gAlertId == 0 || gAlertReturnCode == 3)
					// Quit immediately.
			return (FALSE);

		gOperationCanceledFlag = FALSE;

		if (gAlertReturnCode == 2)
					// Cancel the quit request.
			gAlertReturnCode = 0;

		returnFlag = TRUE;

		}	// end "if (!returnFlag)"

	else	// returnFlag
		{
		/*
				// Call base class idle for any other messages.

		Boolean bResult = ((CMultiSpecApp*)AfxGetApp ())->DoIdle (0);
		((CMultiSpecApp*)AfxGetApp ())->m_pMainWnd->SendMessage (
										WM_ENTERIDLE, MSGF_DIALOGBOX, 0);	// send command
		*/
		}	// end "else returnFlag"

	gNextTime = GetTickCount () + gTimeOffset;
    
	return (returnFlag);

}	// end "CheckSomeEvents"
#endif	// defined multispec_win



void ClipRect (
				Rect*									clipRectanglePtr)
				
{
	#if defined multispec_win
		if (gCDCPointer != NULL) 
			{
			Boolean								errorFlag = FALSE;

			gMFC_Rgn.SetRectRgn (clipRectanglePtr->left,
										 clipRectanglePtr->top,
										 clipRectanglePtr->right,
										 clipRectanglePtr->bottom);

			int returnCode = gCDCPointer->SelectClipRgn (&gMFC_Rgn);

			if (returnCode == ERROR)
				errorFlag = TRUE;
			if (returnCode == COMPLEXREGION)
				errorFlag = TRUE;
			if (returnCode == NULLREGION)
				errorFlag = TRUE;
			if (returnCode == SIMPLEREGION)
				errorFlag = TRUE;

			}	// end "if (gCDCPointer != NULL)"		                   
	#endif	// defined multispec_win
	
	#if defined multispec_wx
				// Note that DestroyClippingRegion () does not seem to work for MacOS
	
		if (gCDCPointer != NULL) 
			{
			wxRect 		clipRect;
			/*
			int clipRect_x;
			int clipRect_y;
			int clipRect_width;
			int clipRect_height;
			
			gCDCPointer->GetClippingBox (&clipRect_x,
													&clipRect_y,
													&clipRect_width,
													&clipRect_height);
			*/
			/*
			clipRect.x = 0;
			clipRect.y = 0;
			clipRect.width = 0;
			clipRect.height = 0;
			wxRegion clipRgn2 (clipRect);
			gCDCPointer->SetDeviceClippingRegion (clipRgn2);
			
			gCDCPointer->GetClippingBox (&clipRect_x,
													&clipRect_y,
													&clipRect_width,
													&clipRect_height);
			*/
			clipRect.x = clipRectanglePtr->left;
			clipRect.y = clipRectanglePtr->top;
			clipRect.width = clipRectanglePtr->right - clipRectanglePtr->left;
			clipRect.height = clipRectanglePtr->bottom - clipRectanglePtr->top;
					 
			//clipRect_x = clipRectanglePtr->left;
			//clipRect_y = clipRectanglePtr->top;
			//clipRect_width = clipRectanglePtr->right - clipRectanglePtr->left;
			//clipRect_height = clipRectanglePtr->bottom - clipRectanglePtr->top;
			
			wxRegion clipRgn (clipRect);
			
			gCDCPointer->DestroyClippingRegion ();
			/*
			gCDCPointer->GetClippingBox (&clipRect_x,
													&clipRect_y,
													&clipRect_width,
													&clipRect_height);
			*/
			gCDCPointer->SetDeviceClippingRegion (clipRgn);
			/*
			gCDCPointer->GetClippingBox (&clipRect_x,
													&clipRect_y,
													&clipRect_width,
													&clipRect_height);
			int clipRect_x_test = clipRect_x;
			*/
			} 
	#endif
	
}	// end "ClipRect"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CloseGraphicsWindow
//
//	Software purpose:	This routine is a stub for the Windows version.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/21/1995
//	Revised By:			Larry L. Biehl			Date: 03/21/2018

void CloseGraphicsWindow (
				WindowPtr							theWindow)
{
	#if defined multispec_wx
		CMGraphFrame* graphFrameCPtr = ((CMGraphView*)theWindow)->m_frame;
		if (graphFrameCPtr != NULL)
			graphFrameCPtr->Close ();
	#endif

	#if defined multispec_win
		CMGraphDoc* graphDocumentPtr =
										(CMGraphDoc*)((CMGraphView*)theWindow)->GetDocument ();
		HWND hWnd = (graphDocumentPtr->GetGraphFrameCPtr ())->m_hWnd;
		LRESULT returnCode = SendMessage (hWnd, WM_CLOSE, 0, 0);
	#endif

}	// end "CloseGraphicsWindow"



void CloseWindow (
				WindowPtr							windowPtr)
		  
{
	#if defined multispec_win
		((CView*)windowPtr)->SendMessage (WM_CLOSE, 0, 0);
	#endif

}	// end "CloseWindow"


#if defined multispec_win
//------------------------------------------------------------------------------------
//
// Function:   CopyHandle (from SDK DibView sample clipbrd.c)
//
// Purpose:    Makes a copy of the given global memory block.  Returns
//             a handle to the new memory block (NULL on error).
//
//             Routine stolen verbatim out of ShowDIB.
//
// Parms:      h == Handle to global memory to duplicate.
//
// Returns:    Handle to new global memory block.
//

Handle GetCopyDIBHandle (
				CMImageView*						imageViewCPtr)

{
	Handle								windowInfoHandle;
	WindowInfoPtr						windowInfoPtr;
	BITMAPINFOHEADER					*inBitMapInfoHeaderPtr,
											*outBitMapInfoHeaderPtr;
	HUInt8Ptr							inputPtr,
											copyPtr;

	Handle								handleCopy = NULL;

	UInt32								bufferSize,
											count;

	Boolean								continueFlag = TRUE;


	windowInfoHandle = GetWindowInfoHandle (imageViewCPtr);

	windowInfoPtr = (WindowInfoPtr) GetHandlePointer (
																				windowInfoHandle, kLock);

	if (windowInfoPtr != NULL &&
			windowInfoPtr->offScreenMapHandle != NULL &&
				windowInfoPtr->imageBaseAddressH != NULL)
		{
				// Lock down the DIB, and get a pointer to the beginning of the image
				// buffer

		inBitMapInfoHeaderPtr = (BITMAPINFOHEADER*)
									::GlobalLock ((HGLOBAL) windowInfoPtr->offScreenMapHandle);

		bufferSize = (UInt32)(sizeof (BITMAPINFOHEADER) +
							inBitMapInfoHeaderPtr->biClrUsed * sizeof (RGBQUAD) +
									::GlobalSize ((HGLOBAL) windowInfoPtr->imageBaseAddressH));

		if ((handleCopy = (HANDLE)::GlobalAlloc (GHND, bufferSize)) != NULL)
			{
			outBitMapInfoHeaderPtr =
									(BITMAPINFOHEADER*)::GlobalLock ((HGLOBAL) handleCopy);
			copyPtr = (HUInt8Ptr) outBitMapInfoHeaderPtr;
			inputPtr = (HUInt8Ptr) inBitMapInfoHeaderPtr;
			count = sizeof (BITMAPINFOHEADER);

			while (count--)
				*copyPtr++ = *inputPtr++;

			if (inBitMapInfoHeaderPtr->biClrUsed > 0)
				{
				Handle displaySpecsHandle = GetDisplaySpecsHandle (windowInfoPtr);
				DisplaySpecsPtr displaySpecsPtr = (DisplaySpecsPtr)
													GetHandlePointer (displaySpecsHandle, kLock);

				if (!displaySpecsPtr->paletteObject->LoadRGBQUADStructure (
																(RGBQUAD*) copyPtr, DIB_RGB_COLORS))
					continueFlag = FALSE;

				CheckAndUnlockHandle (displaySpecsHandle);

				}	// end "if (inBitMapInfoHeaderPtr->biClrUsed > 0)"   

			if (continueFlag)
				{
				Rect			sourceRect;


						// Get the source rectangle.

				Handle windowInfoHandle = GetWindowInfoHandle (gActiveImageViewCPtr);
				WindowInfoPtr windowInfoPtr = (WindowInfoPtr) GetHandlePointer (
																				windowInfoHandle, kLock);

				GetSelectedOffscreenRectangle (windowInfoPtr,
															&sourceRect,
															TRUE,
															TRUE);

				CheckAndUnlockHandle (windowInfoHandle);

						// Determine if the entire image width of the image is to be used.
						// If so, then the copy is easier.

				Boolean copyBlockFlag = FALSE;
				if (inBitMapInfoHeaderPtr->biWidth ==
															(sourceRect.right - sourceRect.left))
					copyBlockFlag = TRUE;

						// Adjust the sourceRect to take into account that the DIB map
						// is inverted top to bottom. 

				SInt16 firstLine =
								(SInt16)inBitMapInfoHeaderPtr->biHeight - sourceRect.bottom;
				SInt16 lastLine =
								(SInt16)inBitMapInfoHeaderPtr->biHeight - sourceRect.top;

						// Get the pointers to the input and output blocks of memory.

				inputPtr =
						(HUInt8Ptr) GlobalLock ((HGLOBAL) windowInfoPtr->imageBaseAddressH);
				copyPtr += inBitMapInfoHeaderPtr->biClrUsed * sizeof (RGBQUAD);

						// Get the number of bytes per line of data.

				UInt32 inputBytesPerLine = GetNumberPixRowBytes (
															(UInt32) outBitMapInfoHeaderPtr->biWidth,
															inBitMapInfoHeaderPtr->biBitCount);

						// Get start point for input buffer.

				inputPtr += firstLine*inputBytesPerLine;

				UInt32 numberOutputLines = sourceRect.bottom - sourceRect.top;

				if (copyBlockFlag)
					{
							// Adjust count to that required to copy the requested data.

					count = numberOutputLines * inputBytesPerLine;
					while (count--)
						*copyPtr++ = *inputPtr++;

					}	// end "if (copyBlockFlag)"

				else	// !copyBlockFlag
					{
					UInt32			line;

					UInt32 outputBytesPerLine = GetNumberPixRowBytes (
													 (UInt32)sourceRect.right - sourceRect.left,
													 inBitMapInfoHeaderPtr->biBitCount);

					UInt32 bytesToSkip = inputBytesPerLine - outputBytesPerLine;
					UInt32 bytesPerPixel = inBitMapInfoHeaderPtr->biBitCount / 8;

					inputPtr += sourceRect.left * bytesPerPixel;

					for (line = 0; line < numberOutputLines; line++)
						{
						count = outputBytesPerLine;

						while (count--)
							*copyPtr++ = *inputPtr++;

						inputPtr += bytesToSkip;

						}	// end "for (line=0; line<numberOutputLines; line++)"

					}	// end "else !copyBlockFlag"

				outBitMapInfoHeaderPtr->biWidth = sourceRect.right - sourceRect.left;
				outBitMapInfoHeaderPtr->biHeight = numberOutputLines;

				::GlobalUnlock (windowInfoPtr->imageBaseAddressH);

				}	// end "if (continueFlag)"

			::GlobalUnlock ((HGLOBAL) handleCopy);

			if (!continueFlag)
				{
				CheckAndUnlockHandle (handleCopy);
				handleCopy = NULL;

				}	// end "if (!continueFlag)"

			}	// end "if ((handleCopy =... != NULL)"

		::GlobalUnlock (windowInfoPtr->offScreenMapHandle);

		}	// end "if (windowInfoPtr != NULL && ..."

	CheckAndUnlockHandle (windowInfoHandle);

	return handleCopy;

}	// end "GetCopyDIBHandle"           
#endif	// defined multispec_win


SInt16 CountMenuItems (
				MenuHandle							menuHandle)

{
	if (menuHandle != NULL)
		{
		#if defined multispec_win
					// Note that this is assuming the input is a combox box pointer

			CComboBox* cComboBoxPtr = (CComboBox*)menuHandle;

			return (cComboBoxPtr->GetCount ());
		#endif	// defined multispec_win

		#if defined multispec_wx
   		if (wxDynamicCast ((wxComboBox*)menuHandle, wxComboBox) != NULL)
   			{
				wxComboBox* wxComboBoxPtr = (wxComboBox*)menuHandle;
				return (SInt16)(wxComboBoxPtr->GetCount ());
				
				}	// end "if (wxDynamicCast ((wxComboBox*)menuHandle, wxComboBox) != ...)"
		
			else	// it must be a wxChoice
   			{
				wxChoice* choiceCtrl = (wxChoice*)menuHandle;
				return (SInt16)(choiceCtrl->GetCount ());
				
				}	// end "else it is a wxChoice
		#endif	// defined multispec_wx
		
		}	// end "if (menuHandle != NULL)"
	
	return (0);

}	// end "CountMenuItems"



void CovertMultispectralToThematicControl (
				WindowInfoPtr						inputWindowInfoPtr)

{

}	// end "CovertMultispectralToThematicControl" 



void CTab2Palette (
				CTabHandle							cTableHandle,
				CMPaletteInfo						paletteObject,
				SInt16								useage,
				SInt16								srcTolerance)

{
	ColorSpec*							colorSpecPtr;
	CTabPtr								cTabPtr;
	UInt16								index,
											numberEntries;


	numberEntries = paletteObject->GetNumberPaletteEntries ();
	cTabPtr = (CTabPtr) GetHandlePointer ((Handle) cTableHandle, kLock);
	colorSpecPtr = cTabPtr->ctTable;

	for (index=0; index<numberEntries; index++)
		{
		MSetEntryColor (paletteObject, index, &colorSpecPtr[index].rgb);
		SetEntryUsage (paletteObject, index, useage, 0);

		}	// end "for (index=0; index<..."          

}	// end "CTab2Palette"  



void DecisionTreeControl (
				FileInfoPtr							fileInfoPtr)

{

}	// end "DecisionTreeControl"



void DeleteMenuItem (
				MenuHandle							menuHandle,
				SInt16								itemNumber)

{
	if (menuHandle != NULL)
		{
		#if defined multispec_wx
			wxComboBox* wxComboBoxPtr = (wxComboBox*)menuHandle;
			
			wxComboBoxPtr->Delete (itemNumber-1);
		#endif	// defined multispec_wx
		
		#if defined multispec_win
			CComboBox* comboBoxPtr = (CComboBox*)menuHandle;
			
			comboBoxPtr->DeleteString (itemNumber-1);
		#endif	// defined multispec_win
		
		}	// end "if (menuHandle != NULL"

}	// end "DeleteMenuItem"



void DisableMenuItem (
				MenuHandle							menuHandle,
				SInt16								itemNumber)
{
	#if defined multispec_wx
		wxMenu* menuptr = (wxMenu*)menuHandle;
		menuptr->Enable (itemNumber, false);
	#endif	// defined multispec_wx
   	
	#if defined multispec_win
		CMenu* cMenuPtr = (CMenu*)menuHandle;
		
		cMenuPtr->EnableMenuItem (itemNumber-1, 0);
	#endif	// defined multispec_win
	
}	// end "DisableMenuItem"



void DisplayPImage (
				DisplaySpecsPtr					displaySpecsPtr)

{

}	// end "DisplayPImage"



void DisposePalette (
				PaletteHandle						resourcePHandle)

{

}	// end "DisposePalette"



void DisposeRgn (
				RgnHandle							rgnHandle)

{
	if (rgnHandle != NULL)
		{
		#if defined multispec_wx
			rgnHandle->Clear ();
		#endif
		
		#if defined multispec_win
			rgnHandle->DeleteObject ();
		#endif

		delete rgnHandle;

		}	// end "if (rgnHandle != NULL)"
	
}	// end "DisposeRgn"



void DrawDialog (
				DialogPtr							dialogPtr)

{
	#if defined multispec_wx
		dialogPtr->Layout ();
		dialogPtr->Fit ();
		dialogPtr->Update ();
	#endif

	#if defined multispec_win
		dialogPtr->UpdateWindow ();
	#endif
	
}	// end "DrawDialog"  



void EnableMenuItem (
				MenuHandle							menuHandle,
				SInt16								itemNumber)

{
	#if defined multispec_wx
		wxMenu* menuptr = (wxMenu*)menuHandle;
		menuptr->Enable (itemNumber, true);
	#endif

	#if defined multispec_win
		CMenu* cMenuPtr = (CMenu*)menuHandle;
		cMenuPtr->EnableMenuItem (itemNumber - 1, 1);
	#endif
	
}	// end "EnableMenuItem"



void ENVI_ASCII_ROI_ToThematicFileControl (void)

{

}	// end "ENVI_ASCII_ROI_ToThematicFileControl"



void EraseRect (
				Rect*									rectPtr)

{

}	// end "EraseRect"



void ForeColor (
				SInt16								color)

{

}	// end "ForeColor" 



SInt32 GetCTSeed (void)

{
	return (0);

}	// end "GetCTSeed"



Boolean GetEigenStatisticsFeatures (
				SInt16*								statEigenFeaturePtr)

{
	return (FALSE);

}	// end "GetEigenStatisticsFeatures"



void GetEntryColor (
				CMPaletteInfo						srcPalette,
				SInt16								srcEntry,
				RGBColor*							dstRGBPtr)

{
	#if defined multispec_win
		PALETTEENTRY paletteEntry;

		if (srcPalette->GetPaletteEntries (srcEntry, 1, &paletteEntry) == 1)
			{
			dstRGBPtr->red = (UInt16)paletteEntry.peRed << 8;
			dstRGBPtr->green = (UInt16)paletteEntry.peGreen << 8;
			dstRGBPtr->blue = (UInt16)paletteEntry.peBlue << 8;

			}	// end "if (srcPalette->GetPaletteEntries (..."

		else	// ... != 1
			{
			dstRGBPtr->red = 0;
			dstRGBPtr->green = 0;
			dstRGBPtr->blue = 0;

			}	// end "else ... != 1)"
	#endif

	#if defined multispec_wx
		MGetEntryColor (srcPalette, srcEntry, dstRGBPtr);
	#endif
	
}	// end "GetEntryColor"



CMPaletteInfo GetNewPalette (
				SInt16								paletteId)

{
	CMPaletteInfo						palettePtr = NULL;

	SInt16								numberEntries = 0;


	if (paletteId == 1001)
		numberEntries = 39;

	else if (paletteId == 1002)
		numberEntries = 132;

	else if (paletteId == 1003)
		numberEntries = 253;

	else if (paletteId == 1004)
		numberEntries = 105;

	else if (paletteId == 1005)
		numberEntries = 124;

	else if (paletteId == 1006)
		numberEntries = 256;

	else if (paletteId == 1007)
		numberEntries = 256;

	if (numberEntries > 0)
		{
		if (MNewPalette (&palettePtr, numberEntries))
			palettePtr->SetPalette (paletteId);

		}	// end "if (numberEntries > 0)"

	return (palettePtr);

}	// end "GetNewPalette"



SInt16 GetOffscreenForNonGWorld (
				DisplaySpecsPtr					displaySpecsPtr,
				Rect*									rectPtr)

{
	return (-1);

}	// end "GetOffscreenForNonGWorld"



void GetPort (
				GrafPtr*								savedPortPtr)

{
	//*savedPortPtr = (GrafPtr)gCDCPointer;

}	// end "GetPort"



void GetDialogItem (
				DialogPtr							dialogPtr,
				SInt16								itemNumber,
				SInt16*								theTypePtr,
				Handle*								theHandlePtr,
				Rect*									theBoxPtr)

{

}	// end "GetDialogItem"



DialogPtr GetDialogWindow (
				DialogPtr							windowPtr)

{
	return (windowPtr);

}	// end "GetDialogWindow"



WindowPtr FrontWindow (void)

{
	return (NULL);

}	// end "FrontWindow"



SInt16 HandToHand (
				Handle*								handlePtr)

{
	SInt16 returnCode = 1;


	if (handlePtr != NULL && *handlePtr != NULL)
		{
		SInt32 sizeHandle;

		#if defined multispec_wx
			sizeHandle = MGetHandleSize (*handlePtr);
		#endif
		#if defined multispec_win
			sizeHandle = (UInt32)::GlobalSize (*handlePtr);
		#endif
		
		if (sizeHandle > 0)
			{
			Handle newHandle = MNewHandle (sizeHandle);

			if (newHandle != NULL)
				{
				returnCode = CopyHandleToHandle (*handlePtr, newHandle);

				if (returnCode == noErr)
					*handlePtr = newHandle;

				}	// end "if (newHandle != NULL)"

			}	// end "if (sizeHandle > 0)"

		}	// end "if (handlePtr != NULL && ..."

	return (returnCode);

}	// end "HandToHand"



void HideDialogItem (
				DialogPtr							dialogPtr,
				SInt16								itemNumber)

{
	#if defined multispec_wx
		wxWindow* dialogItemWindowPtr = dialogPtr->FindWindow (itemNumber);
		if (dialogItemWindowPtr != NULL)
			dialogItemWindowPtr->Show (false);
	#endif
	
	#if defined multispec_win
		CWnd* dialogItemWindowPtr = dialogPtr->GetDlgItem (itemNumber);

		if (dialogItemWindowPtr != NULL)
			dialogItemWindowPtr->ShowWindow (SW_HIDE);
	#endif
	
}	// end "HideDialogItem"



void HiliteControl (
				ControlHandle						controlHandle,
				SInt16								hiliteCode)
		  
{

}	// end "HiliteControl"



void InvalWindowRect (
				WindowPtr							windowPtr,
				Rect*									invalRectanglePtr)

{

}	// end "InvalWindowRect" 



void LActivate (
				Boolean								flag,
				#if defined multispec_wx
					wxListBox*							listBoxCPtr)
				#endif
				#if defined multispec_win
					CListBox*							listBoxCPtr)
				#endif
				
{

}	// end "LActivate"



SInt16 LAddRow (
				SInt16								numberRows,
				SInt16								startRow,
				#if defined multispec_wx
					wxListBox*							listBoxCPtr)
				#endif
				#if defined multispec_win
					CListBox*							listBoxCPtr)
				#endif
				
{
	#ifdef multispec_wx
		wxString								label ("");
		unsigned int						i,
												lastRow;
												
		lastRow = startRow + numberRows;
		for (i = startRow; i<lastRow; i++) 
			{
			listBoxCPtr->Insert (label, i);

			}	// end "for (i=startRow; i<lastRow; i++)"
	#endif
    
	return (startRow);

}	// end "LAddRow"            


#if defined multispec_wx
SInt16 LAddRow (
				SInt16								numberRows,
				SInt16								startRow,
				wxListView*							listBoxCPtr)
				
{
	wxString								label ("");
	unsigned int						i,
											lastRow;
											

	lastRow = startRow + numberRows;
	for (i=startRow; i<lastRow; i++) 
		{
		listBoxCPtr->InsertItem (i, -1); // New row with no image in 1st column
		listBoxCPtr->SetItem (i, 1, label); // Second column is blank text line
		
		}	// end "for (i=startRow; i<startRow+numberRows; i++)"

	return (startRow);

}	// end "LAddRow" 
#endif	// defined multispec_wx


void LAutoScroll (
				#if defined multispec_wx
					wxListBox*							listBoxCPtr)
				#endif
				#if defined multispec_win
					CListBox*							listBoxCPtr)
				#endif
				
{


}	// end "LAutoScroll"
 


Boolean LClick (
				Point									location,
				SInt16								modifiers,
				#if defined multispec_wx
					wxListBox*							listBoxCPtr)
				#endif
				#if defined multispec_win
					CListBox*							listBoxCPtr)
				#endif
				
{
	return (FALSE);

}	// end "LClick"



void LDelRow (
				SInt16								numberRows,
				SInt16								startRow,
				#if defined multispec_wx
					wxListBox*							listBoxCPtr)
				#endif
				#if defined multispec_win
					CListBox*							listBoxCPtr)
				#endif

{
	SInt16								index;


	startRow += numberRows - 1;
	
	#if defined multispec_wx
		if (startRow <= listBoxCPtr->GetCount () - 1)
			for (index=0; index<numberRows; index++) 
				{
				//listBoxCPtr->DeleteItem (startRow);
				listBoxCPtr->Delete (startRow);
				startRow--;

				if (startRow < 0)
					break;

				}	// end "for (index=0; index<numberRows; index++)"
	
				// Turn off any selections
	
		listBoxCPtr->SetSelection (wxNOT_FOUND);
	#endif	// defined multispec_wx

	#if defined multispec_win
		SInt16			returnCode;
	
		for (index=0; index<numberRows; index++) 
			{
			returnCode = listBoxCPtr->DeleteString (startRow);
			startRow--;

			if (returnCode == LB_ERR || startRow < 0)
				break;

			}	// end "for (index=0; index<numberRows; index++)"
	#endif
}	// end "LDelRow" 


#ifdef multispec_wx
void LDelRow (
				SInt16								numberRows,
				SInt16								startRow,
				wxListView*							listBoxCPtr)
				
{
	SInt16								index;

	startRow += numberRows - 1;

	if (startRow <= listBoxCPtr->GetItemCount () - 1)
		for (index=0; index<numberRows; index++) 
			{
			listBoxCPtr->DeleteItem (startRow);
			startRow--;

			if (startRow < 0)
				 break;

			}	// end "for (index=0; index<numberRows; index++)"
			
}	// end "LDelRow" 
#endif	// defined multispec_wx


void LDispose (
				#if defined multispec_wx
					wxListBox*							listBoxCPtr)
				#endif
				#if defined multispec_win
					CListBox*							listBoxCPtr)
				#endif

{
	if (listBoxCPtr != NULL)
		delete listBoxCPtr;

}	// end "LDispose"



void LGetCell (
				char*									stringPtr,
				SInt16*								stringLengthPtr,
				Cell									cell,
				#if defined multispec_wx
					wxListBox*							listBoxCPtr)
				#endif
				#if defined multispec_win
					CListBox*							listBoxCPtr)
				#endif
				
{
   *stringLengthPtr = 0;
	
	if (listBoxCPtr != NULL) 
		{
		#if defined multispec_wx
			wxString value = listBoxCPtr->GetString (cell.v);
			*stringLengthPtr = (SInt16)value.Len ();
			strncpy (stringPtr, 
						(const char*)value.mb_str (wxConvUTF8), 
						*stringLengthPtr + 1);
		#endif	// defined multispec_wx

		#if defined multispec_win
			USES_CONVERSION;

			*stringLengthPtr = listBoxCPtr->GetText (cell.v, gWideTextString);
			//*stringLengthPtr = listBoxCPtr->GetText (cell.v, (LPTSTR)stringPtr);
			strncpy (stringPtr, T2A(gWideTextString), *stringLengthPtr + 1);
		#endif	// defined multispec_win
			
		}	// end "if (listBoxCPtr != NULL)"
	
}	// end "LGetCell"



Boolean LGetSelect (
				Boolean								nextFlag,
				Cell*									cellPtr,
				#if defined multispec_wx
					wxListBox*							listBoxCPtr)
				#endif
				#if defined multispec_win
					CListBox*							listBoxCPtr)
				#endif
 
{
	int									cellIndex,
											selectionCode;

	Boolean selectedFlag = FALSE;
	
    if (listBoxCPtr != NULL) 
		{
		#if defined multispec_wx
			if (nextFlag) 
				{
						// Now determine if this is a single or multiple selection list.

				long flag = listBoxCPtr->GetWindowStyle ();
				if (flag == wxLB_SINGLE)	// Single selection list  
					{
					cellIndex = listBoxCPtr->GetSelection ();
					if (cellIndex != -1) 
						{
						cellPtr->v = cellIndex;
						selectedFlag = TRUE;

						}	// end "if (cellIndex != -1)"
						
					}	// end "if (flag == wxLB_SINGLE)"

				else	// flag != wxLB_SINGLE
					{
							// Multiple selection list
							// Find the next selection in the list.
							
					cellIndex = cellPtr->v;
					selectionCode = 0;

					wxArrayInt selections; 
					selectionCode = listBoxCPtr->GetSelections (selections);


					if (selectionCode > cellPtr->h) 
						{
						cellPtr->v = (SInt16)selections[cellPtr->h];
						cellPtr->h++;
						selectedFlag = TRUE;

						}	// end "if (selectionCode > cellPtr->h)"

					}	// end "else flag != wxLB_SINGLE" 

				}	// end "if (nextFlag)"

			else	// !nextFlag
				{
						// Just return whether the input cell is selected.

				selectedFlag = listBoxCPtr->IsSelected (cellPtr->v);

				}	// end "else !nextFlag"
		#endif	// defined multispec_wx

		#if defined multispec_win
			int									cellCount;
			if (nextFlag) 
				{
						// Now determine if this is a single or multiple
						// selection list.

				cellCount = listBoxCPtr->GetSelCount ();
				if (cellCount == LB_ERR) 
					{
							// Single selection list

					cellIndex = listBoxCPtr->GetCurSel ();
					if (cellIndex != LB_ERR) 
						{
						selectionCode = listBoxCPtr->GetSel (cellIndex);
						if (selectionCode != LB_ERR && selectionCode > 0) 
							{
							cellPtr->v = cellIndex;
							selectedFlag = TRUE;

							}	// end "if (selectionCode != LB_ERR && ..."

						}	// end "if (cellIndex != LB_ERR && ...->GetSel (cellIndex))"

					}	// end "if (cellCount == LB_ERR)"

				else	// cellCount != LB_ERR
					{
							// Multiple selection list
							// Find the next selection in the list.

					cellIndex = cellPtr->v;
					selectionCode = 0;
					while (selectionCode != LB_ERR && !selectedFlag) 
						{
						selectionCode = listBoxCPtr->GetSel (cellIndex);
						if (selectionCode != LB_ERR && selectionCode > 0) 
							{
							cellPtr->v = cellIndex;
							selectedFlag = TRUE;

							}	// end "if (selectionCode != LB_ERR && ..."

						cellIndex++;

						}	// end "while (selectionCode != LB_ERR && !selectedFlag)"

					}	// end "else cellCount != LB_ERR"

				}	// end "if (nextFlag)"

			else	// !nextFlag
				{
						// Just return whether the input cell is selected.

				cellIndex = listBoxCPtr->GetSel (cellPtr->v);
				if (cellIndex != LB_ERR && cellIndex > 0)
					selectedFlag = TRUE;

				}	// end "else !nextFlag"
		#endif	// defined multispec_win

		}	// end "if (listBoxCPtr != NULL)"
		
    return (selectedFlag);

}	// end "LGetSelect"


#if defined multispec_wx
Boolean LGetSelect (
				Boolean								nextFlag,
				Cell*									cellPtr,
				wxListView*							listBoxCPtr)
		  
{
	int									cellIndex,
											selectionCode;

	Boolean								selectedFlag = FALSE;
	

	if (listBoxCPtr != NULL) 
		{
		if (nextFlag) 
			{
					// Now determine if this is a single or multiple
					// selection list.

			//cellCount = listBoxCPtr->GetSelCount ();
			long flag = listBoxCPtr->GetWindowStyle ();
			if (flag & wxLC_SINGLE_SEL)
				{
						// Single selection list

				cellIndex = listBoxCPtr->GetFirstSelected ();
				if (cellIndex != -1) 
					{
					cellPtr->v = cellIndex;
					selectedFlag = TRUE;

					}	// end "if (cellIndex != LB_ERR && ...->GetSel (cellIndex))"

				}	// end "if (cellCount == LB_ERR)"

			else	// cellCount != LB_ERR
				{
						// Multiple selection list
						// Find the next selection in the list.

				cellIndex = cellPtr->v;
				selectionCode = 0;
				selectionCode = listBoxCPtr->GetNextSelected (cellIndex);
				if (selectionCode != -1) 
					{
					cellPtr->v = cellIndex;
					selectedFlag = TRUE;

					}	// end "while (selectionCode != LB_ERR && !selectedFlag)"

				}	// end "else cellCount != LB_ERR" 

			}	// end "if (nextFlag)"

		else	// !nextFlag
			{
					// Just return whether the input cell is selected.

			selectedFlag = listBoxCPtr->IsSelected (cellPtr->v);

			}	// end "else !nextFlag"

		}	// end "if (listBoxCPtr != NULL)"
		
	return (selectedFlag);

}	// end "LGetSelect"
#endif	// defined multispec_wx


void LSetCell (
				Ptr									dataPtr,
				SInt16								dataLength,
				Cell									cell,
				#if defined multispec_wx
					wxListBox*							listBoxCPtr)
				#endif
				#if defined multispec_win
					CListBox*							listBoxCPtr)
				#endif
				
{
	#if defined multispec_wx
		char									tempPtr[64];
		SInt16								numberListItems;
		

		strncpy (tempPtr, dataPtr, dataLength);
		tempPtr[dataLength] = 0;
		wxString stringptr = wxString::FromUTF8 (tempPtr);
		numberListItems = listBoxCPtr->GetCount ();
				  
		if (numberListItems > cell.v)
			listBoxCPtr->SetString (cell.v, stringptr);

		else	// numberListItems < cell.v
			listBoxCPtr->Append (stringptr);
	#endif	// defined multispec_wx

	#if defined multispec_win
		char tempPtr[64];
		SInt16 numberListItems;

		USES_CONVERSION;

		strncpy (tempPtr, dataPtr, dataLength);
		tempPtr[dataLength] = 0;

		numberListItems = listBoxCPtr->GetCount ();
		if (numberListItems >= cell.v) 
			{
			listBoxCPtr->DeleteString (cell.v);
			listBoxCPtr->InsertString (cell.v, (LPCTSTR)A2T(tempPtr));

			}	// end "if (numberListItems >= cell.v)" 

		else	// numberListItems < cell.v
			listBoxCPtr->AddString ((LPCTSTR)A2T(tempPtr));
	#endif	// defined multispec_win

}	// end "LSetCell"     
           

#if defined multispec_wx
void LSetCell (
				int									mvindex,
				int									targetindex,
				wxListView*							listBoxCPtr)
				
{
	wxListItem							mvitem;
	SInt16								numberListItems;
	SInt16								numberCols = listBoxCPtr->GetColumnCount ();


			// Index are zero based rows in the list

	numberListItems = listBoxCPtr->GetItemCount ();
	for (int i=0; i<numberCols; i++) 
		{
		mvitem.SetId (mvindex);
		mvitem.SetMask (-1);
		mvitem.SetColumn (i);
		listBoxCPtr->GetItem (mvitem);
		mvitem.SetId (targetindex);
		
		if (i == 0)
			listBoxCPtr->InsertItem (mvitem);
		else
			listBoxCPtr->SetItem (mvitem);
			
		}	// end "for (int i=0; i<numberCols; i++)"

}	// end "LSetCell"  
#endif


void LSetDrawingMode (
				Boolean								flag,
				#if defined multispec_wx
					wxListBox*							listBoxCPtr)
				#endif
				#if defined multispec_win
					CListBox*							listBoxCPtr)
				#endif
								
{

}	// end "LSetDrawingMode"


#if defined multispec_wx
void LSetDrawingMode (
				Boolean								flag,
				wxListView*							listBoxCPtr)
								
{

}	// end "LSetDrawingMode" 
#endif	// defined multispec_wx


void LSetSelect (
				Boolean								setItFlag,
				Cell									cell,
				#if defined multispec_wx
					wxListBox*							listBoxCPtr)
				#endif
				#if defined multispec_win
					CListBox*							listBoxCPtr)
				#endif
				
{
	#if defined multispec_wx
				// A count of 0 indicates that the list box is hidden
				
		if (listBoxCPtr->GetCount () > 0)
			{
			if (setItFlag)
				listBoxCPtr->SetSelection (cell.v);
			else
			  listBoxCPtr->Deselect (cell.v);
			  
			}	// end "if (listBoxCPtr->GetCount () > 0)"
	#endif	// defined multispec_wx
	
	#if defined multispec_win
		SInt16 returnCode = listBoxCPtr->SetSel (cell.v, setItFlag);
	#endif	// defined multispec_win

}	// end "LSetSelect" 
               

#if defined multispec_wx
void LSetSelect (
				Boolean								setItFlag,
				Cell									cell,
				wxListView*							listBoxCPtr)
		  
{

}	// end "LSetSelect"
#endif	// defined multispec_wx



void MosaicImagesSideBySideControl (void)

{

}	// end "MosaicImagesSideBySideControl"



RgnHandle NewRgn (void)

{
    return ((RgnHandle)MNewHandle (sizeof (RgnHandle)));

}	// end "NewRgn"



Boolean PixMap32Bit (
				Handle								offScreenPixMapH)

{
	return (FALSE);

}	// end "PixMap32Bit"



Boolean PtInRgn (
				Point									point,
				RgnHandle							rgnHandle)

{
	#if defined multispec_wx
		if (rgnHandle != NULL)
			return (rgnHandle->Contains (point.h, point.v));
		else
			return (FALSE);
	#endif
	
	#if defined multispec_win
		return (rgnHandle->PtInRegion (point.h, point.v));
	#endif
	
}	// end "PtInRgn"



void ReleaseResource (
				Handle								handle)
				
{
    UnlockAndDispose (handle);

}	// end "ReleaseResource" 	



SInt16 ResError (void)

{
	return (0);

}	// end "ResError" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetPenCharacteristics
//
//	Software purpose:	The purpose of this routine is to set the windows pen
//							characteristics. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/25/2001
//	Revised By:			Larry L. Biehl			Date: 05/25/2001	

void SetPenCharacteristics (
				CPen*									cPenPtr,
				RGBColor*							rgbColorPtr,
				SInt16								lineThickness)
				
 {
   #if defined multispec_win
		LOGPEN logpen;

		CPen* oldPenPtr = NULL;

		//   LOGPEN* 								logpenPtr;

		Boolean continueFlag;


		logpen.lopnStyle = PS_SOLID;
		logpen.lopnWidth.x = lineThickness;
		logpen.lopnWidth.y = lineThickness;

		logpen.lopnColor = RGB (rgbColorPtr->red / 256,
				rgbColorPtr->green / 256,
				rgbColorPtr->blue / 256);

		continueFlag = cPenPtr->CreatePenIndirect (&logpen);

		oldPenPtr = gCDCPointer->SelectObject (cPenPtr);
	#endif	// defined multispec_win
    
}	// end "SetPenCharacteristics" 



void SetControlValue (
				ControlHandle						controlHandle,
				SInt16								setting)
				
{
	if (controlHandle != NULL)
		{
		#if defined multispec_wx
			Boolean			wxChoiceFlag = FALSE;
		
			const wxChar*  classNamePtr =
							((wxComboBox*)controlHandle)->GetClassInfo()->GetClassName ();
			if (wxStrcmp (classNamePtr, wxT("wxChoice")) == 0)
				wxChoiceFlag = TRUE;
			
			if (wxChoiceFlag)
				((wxChoice*)controlHandle)->SetSelection (setting);
			
			else
				((wxComboBox*)controlHandle)->SetSelection (setting);
		#endif

		#if defined multispec_win
			((CComboBox*)controlHandle)->SetCurSel (setting);
		#endif

		}	// end "if (controlHandle != NULL)"
	
}	// end "SetControlValue"   
                                         


void SelectDialogItemText (
				DialogPtr							dialogPtr,
				SInt16								itemNumber,
				SInt16								startSelection,
				SInt16								endSelection)
{
	#if defined multispec_wx
				// ToDo: Find a way to select the chars from startselection to
				// endselection
		wxWindow* dialogObject = (dialogPtr)->FindWindow (itemNumber);
		if (dialogObject != NULL)
			{
			dialogObject->SetFocus ();
			((wxTextCtrl*)dialogObject)->SelectAll ();
			
			}	// end "if (dialogObject != NULL)"
	#endif

	#if defined multispec_win
		dialogPtr->GetDlgItem (itemNumber)->SetFocus ();
		dialogPtr->SendDlgItemMessage (
										itemNumber, EM_SETSEL, startSelection, endSelection);
	#endif

}	// end "SelectDialogItemText" 



void SetDialogItemText (
				Handle								theHandle,
				ConstStr255Param					textStringPtr)
		  
{

}	// end "SetDialogItemText" 



void SetEntryUsage (
				CMPaletteInfo						paletteObject,
				SInt16								paletteIndex,
				SInt16								srcUsage,
				SInt16								srcTolerance)
				
{
	#if defined multispec_win
		PALETTEENTRY paletteEntry;

		paletteObject->GetPaletteEntries (paletteIndex, 1, &paletteEntry);

		paletteEntry.peFlags = (UInt8) srcUsage;

		paletteObject->SetPaletteEntries (paletteIndex, 1, &paletteEntry);
	#endif	// defined multispec_win
	
}	// end "SetEntryUsage" 

                               

void SetFileReadOnly (
				FileInfoPtr							fileInfoPtr)
		  
{

}	// end "SetFileReadOnly"                  



void SetMenuItemText (
				MenuHandle							menuHandle,
				SInt16								menuItem,
				UCharPtr								stringPtr)
{

}	// end "SetMenuItemText"



void SetPalette (
				WindowPtr							windowPtr,
				CMPaletteInfo						paletteObject,
				Boolean								activateFlag)
		  
{

}	// end "SetPalette"             



void SetPort (
				GrafPtr								windowPtr)

{
	//gCDCPointer = (WindowPtr)windowPtr;

}	// end "SetPort"               



void SetPort (
				WindowPtr							windowPtr)

{
	//gCDCPointer = (WindowPtr)windowPtr;

}	// end "SetPort"             



void SetPort (
				DialogPtr							dialogPtr)

{
	//gCDCPointer = (WindowPtr)dialogPtr;

}	// end "SetPort"               



void SetPortDialogPort (
				DialogPtr							windowPtr)

{
	//gCDCPointer = (WindowPtr)windowPtr;

}	// end "SetPortDialogPort"  

          

void SetPortWindowPort (
				WindowPtr							windowPtr)

{
	//gCDCPointer = (WindowPtr)windowPtr;

}	// end "SetPortWindowPort"        



SInt16 SetThreadState (
				UInt32								asyncIOThread,
				SInt16								threadState,
				UInt32								IOThread)

{
	return (noErr);

}	// end "SetThreadState" 

   

void SetWTitle (
				DialogPtr							dialogPtr,
				unsigned char*						textStringPtr)

{
	#if defined multispec_wx
		wxString newtitle (textStringPtr, wxConvUTF8);
		dialogPtr->SetLabel (newtitle);
	#endif

	#if defined multispec_win
		USES_CONVERSION;

		dialogPtr->SetWindowText (A2T((LPCSTR)&textStringPtr[1]));
	#endif
	
}	// end "SetWTitle"  

               

void ShowDialogItem (
				DialogPtr							dialogPtr,
				SInt16								itemNumber)

{
	#if defined multispec_wx
		wxWindow* dialogItemWindowPtr = dialogPtr->FindWindow (itemNumber);
		if (dialogItemWindowPtr != NULL)
			dialogItemWindowPtr->Show (true);
	#endif

	#if defined multispec_win
		CWnd* dialogItemWindowPtr = dialogPtr->GetDlgItem (itemNumber);

		if (dialogItemWindowPtr != NULL)
			dialogItemWindowPtr->ShowWindow (SW_SHOW);
	#endif
	
}	// end "ShowDialogItem"



void ShowDialogWindow (
				DialogPtr							dialogPtr,
				SInt16								dialogID,
				Boolean								setUpFilterTableFlag)
				
{
	#if defined multispec_wx
		dialogPtr->Show (true);
		dialogPtr->Update ();
	#endif
	
	#if defined multispec_win
		dialogPtr->UpdateWindow ();
	#endif
	
}	// end "ShowDialogWindow"



void ShowSelection (
				WindowPtr							windowPtr)

{
	if (windowPtr != NULL)
		windowPtr->UpdateSelectionCoordinates ();

}	// end "ShowSelection" 



void ShowSelection (
				Handle								windowInfoHandle)

{
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);

	if (windowInfoPtr != NULL)
		{
		CMImageView* imageViewCPtr = windowInfoPtr->cImageWindowPtr->mImageViewCPtr;
		imageViewCPtr->UpdateSelectionCoordinates ();

		}	// end "if (windowInfoPtr != NULL)"

}	// end "ShowSelection"
 


void ShowStatusDialogWindow (
				SInt16								statusDialogID)
				
{
	if (gStatusDialogPtr != NULL)
		{
		#if defined multispec_wx
			gStatusDialogPtr->Show (true);  
			gStatusDialogPtr->Update ();
		#endif
		
		gStatusDialogPtr->SetFocus ();
		CheckSomeEvents (updateMask+activMask);
		
		}	// end "if (gStatusDialogPtr != NULL)"

}	// end "ShowStatusDialogWindow" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean StillDown
//
//	Software purpose:	This routine is a stub for the Windows version. It returns FALSE
//							if the mouse button is not down and TRUE if the mouse is down.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/16/1997
//	Revised By:			Larry L. Biehl			Date: 04/17/2020

Boolean StillDown (
				SInt16								code)

{
	#if defined multispec_wx
		return (FALSE);
	#endif
	
	#if defined multispec_win
		MSG msgCur;

		if (code == 4)
			return (!::PeekMessage (&msgCur, NULL, WM_RBUTTONUP, WM_RBUTTONUP, PM_NOREMOVE));

		else	// code != 4
			return (!::PeekMessage (&msgCur, NULL, WM_LBUTTONUP, WM_LBUTTONUP, PM_NOREMOVE));
	#endif

}	// end "StillDown"


// ToDo: For Linux (Problem in TextWidth)
SInt16 StringWidth (
				UCharPtr								stringPtr)
				
{
	SInt16								width = 30;
											
											
	#if defined multispec_win
		SInt16								characterCount,
												firstCharacter = 1;
		if (stringPtr != NULL) 
			{
			characterCount = stringPtr[0];

			width = TextWidth (stringPtr, firstCharacter, characterCount);

			}	// end "if (stringPtr != NULL)"
	#endif
	
	return (width);

}	// end "StringWidth" 



SInt16 StatHistogramPopUpMenu (
				SInt16								statsWindowMode)

{
    return (0);

}	// end "StatHistogramPopUpMenu"



void SysBeep (
				SInt16								beepLength)
				
{

}	// end "SysBeep"    



void SwapMMUMode (
				SInt8*								addressModePtr)

{

}	// end "SwapMMUMode"       



void UnloadSeg (
				Ptr									funtionPointer)

{

}	// end "UnloadSeg"           



void UpdateImageZoomControls (
				WindowPtr							theWindow,
				double								magnification,
				Boolean								updateZoomsFlag)
				
{
	#if defined multispec_wx
				// Make sure the zoom information in the status bar is up to date

		CMainFrame* pMainFrame = (CMainFrame*)wxGetApp ().GetTopWindow ();
		CMImageFrame* imageFrame = ((CMImageView*)theWindow)->m_frame;
		pMainFrame->UpdateStatusBar (imageFrame, magnification);

				// Update the scale factor in the coordinate view if displayed.
			
		Handle windowInfoHandle = GetWindowInfoHandle (theWindow);
		UpdateScaleInformation (windowInfoHandle);
	#endif

}	// end "UpdateImageZoomControls"            



void UpdateOutputWScrolls (
				WindowPtr							theWindow,
				SInt16								scrollToSelStart,
				Boolean								displayMessageFlag)

{
	#if defined multispec_wx
		gOutputViewCPtr->m_textsw->SetInsertionPointEnd ();
		gOutputViewCPtr->m_textsw->Home ();
		//gOutputViewCPtr->m_textsw->SetSelectionEnd (
		//											gOutputViewCPtr->m_textsw->GetTargetEnd ()); 	
		gOutputViewCPtr->m_textsw->ScrollToEnd (); 	
	#endif
   
}	// end "UpdateOutputWScrolls"             



void UpdateOverlayControl (
				WindowPtr							theWindow)
				
{

}	// end "UpdateOverlayControl" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void UpdateStatWEditMenu
//
//	Software purpose:	This routine updates the edit menu for the
//							statistics window mode.
//
//	Parameters in:
//
//	Parameters out:
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/15/1989
//	Revised By:			Larry L. Biehl			Date: 10/06/1998

void UpdateStatWEditMenu (void)

{

}	// end "UpdateStatWEditMenu" 



void ValidRect (
				Rect*									rectPtr)

{

}	// end "ValidRect"



SInt16 YieldToThread (
				UInt32								wakeUpThread)

{
	return (noErr);

}	// end "YieldToThread"    
