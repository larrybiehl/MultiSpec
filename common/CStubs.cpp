//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2017)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						CStubs.cpp
//
//	Authors:					Larry L. Biehl, Abdur Maud
//
//	Revision number:		3.5
//
//	Revision date:			03/27/2017
//
//	Language:				C
//
//	System:					Macintosh and Windows Operating Systems
//								
//	Brief description:	The purpose of the routines in this file are to allow common
//								files across Windows and Linux versions. It was originally to be
//								for common files between all MultiSpec versions in a trully c++
//								implementation. This was stopped when metrowerks stopped supporting
//								Mac; therefore their class library could not be used.
//
//	Functions in file:
/* Template for debugging
	int numberChars = sprintf ((char*)&gTextString3,
									" CStubs:CloseGraphicsWindow (gNumberOfGWindows): %d%s", 
									gNumberOfGWindows,
									gEndOfLine);
	ListString ((char*)&gTextString3, numberChars, gOutputTextH);	
*/

#include "SMulSpec.h"

#ifdef multispec_win
#include "WMultiSpec.h"
#include "CImagVew.h"
#include "CImagWin.h"
#include "CProcess.h"
#include "CPalette.h"
#include "SGrafVew.h"
#include "WDialog.h" 
#include "WImagDoc.h"
#include "WImagFrm.h"
#include	"WStatDlg.h"
#include "WTLegend.h"  

#include	"SExtGlob.h"
#endif
//#include	"SExtGlob.h" already declared in smulspec.h
#ifdef multispec_lin
#include <wx/wx.h>
#include <wx/pen.h>
#include <wx/menu.h>
#include <wx/evtloop.h>
#include "MultiSpec2.h"
#define CPen wxPen
#include "LMainFrame.h"
#include "LImageView.h"
#include "CImagWin.h"
#include "CProcess.h"
#include "CPalette.h"
#include "ldialog.h"
#endif
extern void SetPenCharacteristics(
        CPen* newPenPtr,
        RGBColor* rgbColorPtr,
        SInt16 lineThickness);

extern void ENVI_ASCII_ROI_ToThematicFileControl(void);
#ifndef multispec_lin
extern Handle GetCopyDIBHandle(
        CMImageView* imageViewCPtr);
#endif

#if defined multispec_lin
extern void MGetEntryColor(CMPaletteInfo paletteObject, UInt16 entry, RGBColor* theColorPtr);
#endif
// TODO: For Linux
#ifndef multispec_lin

void AnimateEntry(
        CMPaletteInfo paletteObject,
        UInt16 startIndex,
        RGBColor* theColorPtr)
 {
    PALETTEENTRY paletteEntry;


    paletteEntry.peRed = (UInt8) (theColorPtr->red >> 8);
    paletteEntry.peGreen = (UInt8) (theColorPtr->green >> 8);
    paletteEntry.peBlue = (UInt8) (theColorPtr->blue >> 8);
    paletteEntry.peFlags = PC_RESERVED;

    CDC* pDC = gActiveImageViewCPtr->GetDC();
    CPalette* pOldPal = pDC->SelectPalette(paletteObject, FALSE);
    paletteObject->AnimatePalette(startIndex,
            1,
            &paletteEntry);

    //	#ifndef _WIN32
    pDC->RealizePalette();
    //	#else
    //		HDC hdc = pDC->GetSafeHdc();
    //		RealizePalette(hdc);
    //	#endif

    pDC->SelectPalette(pOldPal, FALSE);
    gActiveImageViewCPtr->ReleaseDC(pDC);

} // end "AnimateEntry"
#endif 

#if defined multispec_lin

void AnimateEntry(
        CMPaletteInfo paletteObject,
        UInt16 startIndex,
        RGBColor* theColorPtr)
 {
    // Dont do anything for now. Modify later.
    /*
         PALETTEENTRY		paletteEntry;
	   
	               
         paletteEntry.peRed = (UInt8)(theColorPtr->red >> 8);
         paletteEntry.peGreen = (UInt8)(theColorPtr->green >> 8);
         paletteEntry.peBlue = (UInt8)(theColorPtr->blue >> 8);
         paletteEntry.peFlags = PC_RESERVED; 
	
         CDC* pDC = gActiveImageViewCPtr->GetDC();
         CPalette* pOldPal = pDC->SelectPalette(paletteObject, FALSE);	 
         paletteObject->AnimatePalette (startIndex, 
                                                 1, 
                                                 &paletteEntry);
			
         pDC->RealizePalette(); 


    pDC->SelectPalette(pOldPal, FALSE);
    gActiveImageViewCPtr->ReleaseDC (pDC);
     * */

} // end "AnimateEntry"
#endif 

void AppendMenu(
        MenuHandle menuHandle,
        Str255 textString)
 {

} // end "AppendMenu"

void BlockMove(
        const void *inStringPtr,
        void *outStringPtr,
        UInt32 numberBytes)
 {
    if (inStringPtr != NULL && outStringPtr != NULL) {
        // Removed the call to _fmemmove because it does not handle copying
        // across 64K boundaries properly 

        //		if (numberBytes <= USHRT_MAX)                                                                      
        //			_fmemmove ( (HPtr)outStringPtr, (HPtr)inStringPtr, (size_t)numberBytes);

        //		else		// numberBytes > USHRT_MAX
        //			{
        UInt32 count;
        HPtr lOutStringPtr = (HPtr) outStringPtr;
        HPtr lInStringPtr = (HPtr) inStringPtr;

        for (count = 0; count < numberBytes; count++) {
            *lOutStringPtr = *lInStringPtr;
            lOutStringPtr++;
            lInStringPtr++;

        } // end "for (count=0; count<numberBytes; count++)"

        //			}		// end "else numberBytes > USHRT_MAX"

    } // end "if (inStringPtr != NULL && outStringPtr != NULL)"

} // end "BlockMove"

void BlockMoveData(
        const void *inStringPtr,
        void *outStringPtr,
        UInt32 numberBytes)
 {
    if (inStringPtr != NULL && outStringPtr != NULL) {
        UInt32 count;
        HPtr lOutStringPtr = (HPtr) outStringPtr;
        HPtr lInStringPtr = (HPtr) inStringPtr;

        for (count = 0; count < numberBytes; count++) {
            *lOutStringPtr = *lInStringPtr;
            lOutStringPtr++;
            lInStringPtr++;

        } // end "for (count=0; count<numberBytes; count++)" 

    } // end "if (inStringPtr != NULL && outStringPtr != NULL)"

} // end "BlockMoveData"

SInt16 CharWidth(
        char								character)
{
	#if defined multispec_win
		CSize size;


		if (gCDCPointer != NULL)
#ifdef UNICODE
		  size = gCDCPointer->GetTextExtent((LPCWSTR)& character, 1);
#else
		  size = gCDCPointer->GetTextExtent((LPCTSTR)& character, 1);
#endif
		else //
		  size.cx = 6;

		return ( (SInt16) size.cx);
	#endif

	#if defined multispec_lin
		wxSize						size;
		char							characterString[2];
		wxString						wx_character;
		SInt16						charWidth;
		
		
		characterString[0] = character;
		characterString[1] = 0;
		wx_character = wxString::FromAscii(characterString);

		if (gCDCPointer != NULL)
			{
			size = gCDCPointer->GetTextExtent(wx_character);
			charWidth = size.GetWidth();
			
			}

		else //
		  charWidth = 6;

		return (charWidth);
	#endif
} // end "CharWidth"



//-----------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
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
//	Revised By:			Larry L. Biehl			Date: 05/18/2016			

Boolean CheckSomeEvents(
        SInt16 code)
{
	Boolean returnFlag = TRUE;

	#if defined multispec_win
		HWND hWnd = NULL;
		MSG msgCur;


		if (gStatusDialogPtr != NULL)
		  hWnd = gStatusDialogPtr->m_hWnd;

		while (::PeekMessage(&msgCur, NULL, 0, 0, PM_REMOVE))
			{
			if (msgCur.message == WM_KEYDOWN) 
				{
				if (msgCur.wParam == 0x001b) 
					{
					returnFlag = FALSE;
					code = keyDownMask;
					break;

					} // end "if (msgCur.wParam == 0x001b)"

				} // end "if (msgCur.message == WM_CHAR)"

			if (returnFlag && !IsDialogMessage(hWnd, &msgCur)) 
				{
				if (msgCur.message == WM_PAINT) 
					{
					TranslateMessage(&msgCur);
					DispatchMessage(&msgCur);

					} // end "if (msgCur.message == WM_PAINT)"

				} // end "if (returnFlag && !IsDialogMessage(hWnd, &msgCur))"

			} // end "while (::PeekMessage(..."

		if (!returnFlag) 
			{
			gOperationCanceledFlag = TRUE;

			if (gAlertId != 0)
				gAlertReturnCode = DisplayAlert(
					 gAlertId,
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

			}		// end "if (!returnFlag)"

		else // returnFlag
			{
					// Call base class idle for any other messages.

			//Boolean bResult = ((CMultiSpecApp*)AfxGetApp())->DoIdle(0);
			//((CMultiSpecApp*)AfxGetApp())->m_pMainWnd->SendMessage(WM_ENTERIDLE, MSGF_DIALOGBOX, 0);    // send command

			} // end "else returnFlag"

		gNextTime = GetTickCount() + gTimeOffset;
	#endif defined multispec_win

	#if defined multispec_lin
		//wxBell();
		//if (wxUpdateUIEvent::CanUpdate(this))
		//(wxTheApp->GetTopWindow())->UpdateWindowUI();
		/*		while(...) { 
									 myApp->Yield(); 
									 wxMilliSleep(1); 
									 }
		*/
		UInt32			count = 0;
		Boolean			yieldForReturnFlag = true;
		wxEventLoopBase* eventLoopBasePtr = wxEventLoopBase::GetActive();
		if (eventLoopBasePtr != NULL)
			{	
			while (count<3 && yieldForReturnFlag)
				{
				yieldForReturnFlag = eventLoopBasePtr->YieldFor(wxEVT_CATEGORY_UI);
				count++;
				
				}		// end "while (count<5 && returnFlag)"
			/*
			int numberChars = sprintf ((char*)&gTextString3,
												" CStubs:CheckSomeEvents (count): %d%s",
												count,
												gEndOfLine);
			ListString ((char*)&gTextString3, numberChars, gOutputTextH);
			*/
			}		// end "if (eventLoopBasePtr != NULL)"

		if (wxGetKeyState(wxKeyCode(WXK_COMMAND)) && wxGetKeyState(wxKeyCode('.'))) 
			{
			returnFlag = FALSE;
			}
			
		if (!returnFlag) 
			{
			gOperationCanceledFlag = TRUE;
		  
			if (gAlertId != 0)
				{           
				//eventLoopBasePtr->Yield(false);
				gAlertReturnCode = DisplayAlert(
					 gAlertId,
					 kCautionAlert,
					 gAlertStrID,
					 gAlertStringNumber,
					 0,
					 NULL);            
				}

			if (gAlertId == 0 || gAlertReturnCode == 3)
					// Quit immediately.
				return (FALSE);

			gOperationCanceledFlag = FALSE;

			if (gAlertReturnCode == 2)
					// Cancel the quit request.
				gAlertReturnCode = 0;

			returnFlag = TRUE;

			} // end "if !returnFlag"

		gNextTime = GetTickCount() + gTimeOffset;
	#endif	// defined multispec_lin
    
	return (returnFlag);

} // end "CheckSomeEvents" 

// TODO: For Linux when done gui setup

void ClipRect(
        Rect* clipRectanglePtr)
 {
#ifndef multispec_lin
    if (gCDCPointer != NULL) {
        Boolean errorFlag = FALSE;
        //		RECT 		oldClipBox1, oldClipBox2;


        //		int returnCode2 = gCDCPointer->GetClipBox ( &oldClipBox1 );

        gMFC_Rgn.SetRectRgn(clipRectanglePtr->left,
                clipRectanglePtr->top,
                clipRectanglePtr->right,
                clipRectanglePtr->bottom);

        int returnCode = gCDCPointer->SelectClipRgn(&gMFC_Rgn);

        //		returnCode2 = gCDCPointer->GetClipBox ( &oldClipBox2 );

        if (returnCode == ERROR)
            errorFlag = TRUE;
        if (returnCode == COMPLEXREGION)
            errorFlag = TRUE;
        if (returnCode == NULLREGION)
            errorFlag = TRUE;
        if (returnCode == SIMPLEREGION)
            errorFlag = TRUE;

    } // end "if (gCDCPointer != NULL)"		                   
#endif
    
    // NOT FINISHED YET
#if defined multispec_lin
     if (gCDCPointer != NULL) {
        wxRect clipRect;
        clipRect.x = clipRectanglePtr->left;
        clipRect.y = clipRectanglePtr->top;
        clipRect.width = clipRectanglePtr->right - clipRectanglePtr->left;
        clipRect.height = clipRectanglePtr->bottom - clipRectanglePtr->top;
        /*
        int clipRect_x;
        int clipRect_y;
        int clipRect_width;
        int clipRect_height;
        gCDCPointer->GetClippingBox(&clipRect_x, &clipRect_y, &clipRect_width, &clipRect_height);
        printf("old clipRect: (x,y)= (%d,%d), (w,h)=(%d,%d) \n",clipRect_x, clipRect_y, clipRect_width, clipRect_height);
                
        clipRect_x = clipRectanglePtr->left;
        clipRect_y = clipRectanglePtr->top;
        clipRect_width = clipRectanglePtr->right - clipRectanglePtr->left;
        clipRect_height = clipRectanglePtr->bottom - clipRectanglePtr->top;
//        gCDCPointer->SetClippingRegion(clipRect);   
        printf("clipRect: (x,y)= (%d,%d), (w,h)=(%d,%d) \n",clipRect_x, clipRect_y, clipRect_width, clipRect_height);
         * */
        wxRegion clipRgn(clipRect);
//        clipRgn.x = clipRectanglePtr->left;
//        clipRgn.y = clipRectanglePtr->top;
//        clipRgn.width = clipRectanglePtr->right - clipRectanglePtr->left;
//        clipRgn.height = clipRectanglePtr->bottom - clipRectanglePtr->top;
        
        gCDCPointer->SetDeviceClippingRegion(clipRgn);
     } 
#endif
} // end "ClipRect"



//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
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
//	Revised By:			Larry L. Biehl			Date: 01/18/2017

void CloseGraphicsWindow(
        WindowPtr									theWindow)
{
#	if defined multispec_lin
		CMGraphFrame* graphFrameCPtr = ((CMGraphView*)theWindow)->m_frame;
		if (graphFrameCPtr != NULL)
			graphFrameCPtr->Close();
#	endif

#	if defined multispec_win
		((CView*)theWindow)->SendMessage(WM_CLOSE, 0, 0);
#	endif

} // end "CloseGraphicsWindow"


void CloseWindow(
        WindowPtr									windowPtr)
		  
{
#	if defined multispec_win
		((CView*) windowPtr)->SendMessage(WM_CLOSE, 0, 0);
#	endif

} // end "CloseWindow"



//---------------------------------------------------------------------
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
//---------------------------------------------------------------------
// TODO: For Linux
#ifndef multispec_lin

Handle GetCopyDIBHandle(
        CMImageView* imageViewCPtr)
 {
    Handle windowInfoHandle;
    WindowInfoPtr windowInfoPtr;
    BITMAPINFOHEADER *inBitMapInfoHeaderPtr,
            *outBitMapInfoHeaderPtr;
    HUInt8Ptr inputPtr,
            copyPtr;

    Handle handleCopy = NULL;

    UInt32 bufferSize,
            count;

    Boolean continueFlag = TRUE;


    windowInfoHandle = GetWindowInfoHandle(imageViewCPtr);

    windowInfoPtr = (WindowInfoPtr) GetHandlePointer(
            windowInfoHandle, kLock, kNoMoveHi);

    if (windowInfoPtr != NULL &&
            windowInfoPtr->offScreenMapHandle != NULL &&
            windowInfoPtr->imageBaseAddressH != NULL) {
        // Lock down the DIB, and get a pointer to the beginning of the image
        // buffer

        inBitMapInfoHeaderPtr = (BITMAPINFOHEADER*)
                ::GlobalLock((HGLOBAL) windowInfoPtr->offScreenMapHandle);

        bufferSize = (UInt32)(sizeof (BITMAPINFOHEADER) +
                inBitMapInfoHeaderPtr->biClrUsed * sizeof (RGBQUAD) +
                ::GlobalSize((HGLOBAL) windowInfoPtr->imageBaseAddressH));

        if ((handleCopy = (HANDLE)::GlobalAlloc(GHND, bufferSize)) != NULL) {
            outBitMapInfoHeaderPtr =
                    (BITMAPINFOHEADER*) ::GlobalLock((HGLOBAL) handleCopy);
            copyPtr = (HUInt8Ptr) outBitMapInfoHeaderPtr;
            inputPtr = (HUInt8Ptr) inBitMapInfoHeaderPtr;
            count = sizeof (BITMAPINFOHEADER);

            while (count--)
                *copyPtr++ = *inputPtr++;

            if (inBitMapInfoHeaderPtr->biClrUsed > 0) {
                Handle displaySpecsHandle = GetDisplaySpecsHandle(windowInfoPtr);
                DisplaySpecsPtr displaySpecsPtr = (DisplaySpecsPtr)
                        GetHandlePointer(displaySpecsHandle, kLock, kNoMoveHi);

                if (!displaySpecsPtr->paletteObject->LoadRGBQUADStructure(
                        (RGBQUAD*) copyPtr, DIB_RGB_COLORS))
                    continueFlag = FALSE;

                CheckAndUnlockHandle(displaySpecsHandle);

            } // end "if (inBitMapInfoHeaderPtr->biClrUsed > 0)"   

            if (continueFlag) {
                Rect sourceRect;


                // Get the source rectangle.

                Handle windowInfoHandle = GetWindowInfoHandle(gActiveImageViewCPtr);
                WindowInfoPtr windowInfoPtr = (WindowInfoPtr) GetHandlePointer(
                        windowInfoHandle, kLock, kNoMoveHi);

                GetSelectedOffscreenRectangle(windowInfoPtr,
                        &sourceRect,
                        TRUE,
                        TRUE);

                CheckAndUnlockHandle(windowInfoHandle);

                // Determine if the entire image width of the image is to be used.
                // If so, then the copy is easier.

                Boolean copyBlockFlag = FALSE;
                if (inBitMapInfoHeaderPtr->biWidth == (sourceRect.right - sourceRect.left))
                    copyBlockFlag = TRUE;

                // Adjust the sourceRect to take into account that the DIB map
                // is inverted top to bottom. 

                SInt16 firstLine = (SInt16) inBitMapInfoHeaderPtr->biHeight - sourceRect.bottom;
                SInt16 lastLine = (SInt16) inBitMapInfoHeaderPtr->biHeight - sourceRect.top;

                // Get the pointers to the input and output blocks of memory.

                inputPtr =
                        (HUInt8Ptr) GlobalLock((HGLOBAL) windowInfoPtr->imageBaseAddressH);
                copyPtr += inBitMapInfoHeaderPtr->biClrUsed * sizeof (RGBQUAD);

                // Get the number of bytes per line of data. 

                UInt32 inputBytesPerLine = GetNumberPixRowBytes(
                        (UInt32) outBitMapInfoHeaderPtr->biWidth,
                        inBitMapInfoHeaderPtr->biBitCount);

                // Get start point for input buffer.

                inputPtr += firstLine*inputBytesPerLine;

                UInt32 numberOutputLines = sourceRect.bottom - sourceRect.top;

                if (copyBlockFlag) {
                    // Adjust count to that required to copy the requested data.

                    count = numberOutputLines * inputBytesPerLine;
                    while (count--)
                        *copyPtr++ = *inputPtr++;

                }// end "if (copyBlockFlag)"

                else // !copyBlockFlag
                {
                    UInt32 line;

                    UInt32 outputBytesPerLine = GetNumberPixRowBytes(
                            (UInt32) sourceRect.right - sourceRect.left,
                            inBitMapInfoHeaderPtr->biBitCount);

                    UInt32 bytesToSkip = inputBytesPerLine - outputBytesPerLine;
                    UInt32 bytesPerPixel = inBitMapInfoHeaderPtr->biBitCount / 8;

                    inputPtr += sourceRect.left * bytesPerPixel;

                    for (line = 0; line < numberOutputLines; line++) {
                        count = outputBytesPerLine;

                        while (count--)
                            *copyPtr++ = *inputPtr++;

                        inputPtr += bytesToSkip;

                    } // end "for (line=0; line<numberOutputLines; line++)" 

                } // end "else !copyBlockFlag"

                outBitMapInfoHeaderPtr->biWidth = sourceRect.right - sourceRect.left;
                outBitMapInfoHeaderPtr->biHeight = numberOutputLines;

                ::GlobalUnlock(windowInfoPtr->imageBaseAddressH);

            } // end "if (continueFlag)"

            ::GlobalUnlock((HGLOBAL) handleCopy);

            if (!continueFlag) {
                CheckAndUnlockHandle(handleCopy);
                handleCopy = NULL;

            } // end "if (!continueFlag)" 

        } // end "if ((handleCopy =... != NULL)" 

        ::GlobalUnlock(windowInfoPtr->offScreenMapHandle);

    } // end "if (windowInfoPtr != NULL && ..."

    CheckAndUnlockHandle(windowInfoHandle);

    return handleCopy;

} // end "GetCopyDIBHandle"           
#endif

SInt16 CountMenuItems(
        MenuHandle menuHandle)
 {
#ifndef multispec_lin
//#if defined _X86_ 
    CMenu* cMenuPtr;
//#endif	// defined _X86_ 


    cMenuPtr = (CMenu*) menuHandle;

    return ( cMenuPtr->GetMenuItemCount());
#else
    wxMenu* menuptr = (wxMenu *) menuHandle;
    return (SInt16) (menuptr->GetMenuItemCount());
#endif

} // end "CountMenuItems"

void CovertMultispectralToThematicControl(
        WindowInfoPtr inputWindowInfoPtr)
 {

} // end "CovertMultispectralToThematicControl" 

//#ifndef multispec_lin

void CTab2Palette(
        CTabHandle cTableHandle,
        CMPaletteInfo paletteObject,
        SInt16 useage,
        SInt16 srcTolerance)
 {
    ColorSpec* colorSpecPtr;
    CTabPtr cTabPtr;
    UInt16 index,
            numberEntries;


    numberEntries = paletteObject->GetNumberPaletteEntries();
    cTabPtr = (CTabPtr) GetHandlePointer((Handle) cTableHandle, kLock, kNoMoveHi);
    colorSpecPtr = cTabPtr->ctTable;

    for (index = 0; index < numberEntries; index++) {
        MSetEntryColor(paletteObject, index, &colorSpecPtr[index].rgb);
        SetEntryUsage(paletteObject, index, useage, 0);

    } // end "for (index=0; index<..."          

} // end "CTab2Palette"  
//#endif

void DecisionTreeControl(
        FileInfoPtr fileInfoPtr)
{

} // end "DecisionTreeControl"

void DeleteMenuItem(
        MenuHandle menuHandle,
        SInt16 itemNumber)
 {

} // end "DeleteMenuItem"

void DisableItem(
        MenuHandle menuHandle,
        SInt16 itemNumber)
 {
    /*	#if defined _X86_ 
                    CMenu*								cMenuPtr;
            #endif	// defined _X86_ 
	
	
            cMenuPtr = (CMenu*)menuHandle;
		
            cMenuPtr->DisableMenuItem (itemNumber-1, 1);
     */
#if defined multispec_lin
    wxMenu* menuptr;
    menuptr = (wxMenu*) menuHandle;
    menuptr->Enable(itemNumber, false);
#endif
} // end "DisableItem"

void DisplayPImage(
        DisplaySpecsPtr displaySpecsPtr)
 {

} // end "DisplayPImage"

void DisposePalette(
        PaletteHandle resourcePHandle)
 {

} // end "DisposePalette"

void DisposeRgn(
        RgnHandle rgnHandle)
{
#ifndef multispec_lin
	if (rgnHandle != NULL)
		{
		rgnHandle->DeleteObject();
		delete rgnHandle;

		} // end "if (rgnHandle != NULL)"
#else
	if (rgnHandle != NULL)
		{
		rgnHandle->Clear();
		delete rgnHandle;
		}
#endif
} // end "DisposeRgn"


// TODO: For Linux

void DrawDialog(
        DialogPtr dialogPtr)
 {
#ifndef multispec_lin
    dialogPtr->UpdateWindow();
#else
	dialogPtr->Layout();
	dialogPtr->Fit();
	dialogPtr->Update();
#endif
} // end "DrawDialog"  

// TODO: For Linux
//#ifndef multispec_lin
#ifdef multispec_mac

void EllipsePlotV(
        GraphPtr graphRecPtr,
        SInt32* error)
{

} // end "EllipsePlotV"
#endif

void EnableItem(
        MenuHandle menuHandle,
        SInt16 itemNumber)
 {
#ifndef multispec_lin
//#if defined _X86_ 
    CMenu* cMenuPtr;
//#endif	// defined _X86_ 


    cMenuPtr = (CMenu*) menuHandle;

    cMenuPtr->EnableMenuItem(itemNumber - 1, 1);
#else
    wxMenu* menuptr;
    menuptr = (wxMenu*) menuHandle;
    menuptr->Enable(itemNumber, true);
#endif
} // end "EnableItem"

void ENVI_ASCII_ROI_ToThematicFileControl(void)
 {

} // end "ENVI_ASCII_ROI_ToThematicFileControl"  

void EraseRect(
        Rect* rectPtr)
 {

} // end "EraseRect" 

void ForeColor(
        SInt16 color)
 {

} // end "ForeColor" 


/*															
void ForceFieldOutlineUpdate (
                                Boolean						forceFlag)
				
{

}		// end "ForceFieldOutlineUpdate"
 */

#if 0
// TODO: For Linux
#ifndef multispec_lin

void GetBiPlotGraphTitle(
        GraphPtr graphRecordPtr)
 {

} // end "GetBiPlotGraphTitle" 
#endif
#endif

SInt32 GetCTSeed(void)
 {
    return (0);

} // end "GetCTSeed" 

Boolean GetEigenStatisticsFeatures(
        SInt16* statEigenFeaturePtr)
 {
    return (FALSE);

} // end "GetEigenStatisticsFeatures" 

void GetEntryColor(
        CMPaletteInfo srcPalette,
        SInt16 srcEntry,
        RGBColor* dstRGBPtr)
 {
#ifndef multispec_lin
    PALETTEENTRY paletteEntry;

    if (srcPalette->GetPaletteEntries(srcEntry, 1, &paletteEntry) == 1) {
        dstRGBPtr->red = (UInt16) paletteEntry.peRed << 8;
        dstRGBPtr->green = (UInt16) paletteEntry.peGreen << 8;
        dstRGBPtr->blue = (UInt16) paletteEntry.peBlue << 8;

    }// end "if (srcPalette->GetPaletteEntries(srcEntry, 1, &paletteEntry) == 1)"

    else // ... != 1 
    {
        dstRGBPtr->red = 0;
        dstRGBPtr->green = 0;
        dstRGBPtr->blue = 0;

    } // end "else ... != 1)" 
#else
    MGetEntryColor(srcPalette, srcEntry, dstRGBPtr);
#endif
} // end "GetEntryColor" 

CMPaletteInfo GetNewPalette(
        SInt16 paletteId)
 {
    CMPaletteInfo palettePtr = NULL;

    SInt16 numberEntries = 0;


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

    if (numberEntries > 0) {
        if (MNewPalette(&palettePtr, numberEntries))
            palettePtr->SetPalette(paletteId);

    } // end "if (numberEntries > 0)" 

    return (palettePtr);

} // end "GetNewPalette"  

SInt16 GetOffscreenForNonGWorld(
        DisplaySpecsPtr displaySpecsPtr,
        Rect* rectPtr)
{
    return (-1);

} // end "GetOffscreenForNonGWorld"  	

/*                       	
SInt16 GetPopUpMenuBitsPerDataValue (void)

{
        return (8); 

}		// end "GetPopUpMenuBitsPerDataValue"	
 */


void GetPort(
        GrafPtr* savedPortPtr)
{
    //	*savedPortPtr = (GrafPtr)gCDCPointer; 

} // end "GetPort" 

void GetDialogItem(
        DialogPtr dialogPtr,
        SInt16 itemNumber,
        SInt16* theTypePtr,
        Handle* theHandlePtr,
        Rect* theBoxPtr)
{

} // end "GetDialogItem"          

DialogPtr GetDialogWindow(
        DialogPtr windowPtr)
{
    return (windowPtr);

} // end "GetDialogWindow"     

WindowPtr FrontWindow(void)
{
    return (NULL);

} // end "FrontWindow"         

SInt16 HandToHand(
        Handle* handlePtr)
{
	SInt16 returnCode = 1;


	if (handlePtr != NULL && *handlePtr != NULL)
		{
		SInt32 sizeHandle;

#		ifndef multispec_lin
			sizeHandle = (UInt32)::GlobalSize(*handlePtr);
#		else
			sizeHandle = MGetHandleSize(*handlePtr);
#		endif
		if (sizeHandle > 0)
			{
			Handle newHandle = MNewHandle(sizeHandle);

			if (newHandle != NULL)
				{
				returnCode = CopyHandleToHandle(*handlePtr, newHandle);

				if (returnCode == noErr)
					*handlePtr = newHandle;

				}	// end "if (newHandle != NULL)"

			}	// end "if (sizeHandle > 0)"

		}		// end "if (handlePtr != NULL && ..."

	return (returnCode);

}		// end "HandToHand"

void HideDialogItem(
        DialogPtr dialogPtr,
        SInt16 itemNumber)
 {
#ifndef multispec_lin
    CWnd* dialogItemWindowPtr = dialogPtr->GetDlgItem(itemNumber);

    if (dialogItemWindowPtr != NULL)
        dialogItemWindowPtr->ShowWindow(SW_HIDE);
#else
    wxWindow* dialogItemWindowPtr = dialogPtr->FindWindow(itemNumber);
    if (dialogItemWindowPtr != NULL)
        dialogItemWindowPtr->Show(false);
#endif
} // end "HideDialogItem"

void HiliteControl(
        ControlHandle controlHandle,
        SInt16 hiliteCode)
{

} // end "HiliteControl"


/*
SInt16	HistogramStatsControl (
                                short int, 
                                Boolean)

{
        return (0);                        
	
}		// end "HistogramStatsControl"
 */

/*
// Replaced with InvalWindowRect
void InvalRect (
                                Rect*						invalRectanglePtr)
				
{

}		// end "InvalRect" 
 */


void InvalWindowRect(
        WindowPtr windowPtr,
        Rect* invalRectanglePtr)
 {

} // end "InvalWindowRect" 

/*
Boolean IsProjectData (
                                AreaDescriptionPtr				maskAreaDescriptionPtr,
                                UInt32								pixelLine,
                                UInt32								pixelColumn)
				
{                            
        return (TRUE);

}		// end "IsProjectData" 
 */


void LActivate(
        Boolean flag,
        CListBox* listBoxCPtr)
 {

} // end "LActivate"

SInt16 LAddRow(
        SInt16 numberRows,
        SInt16 startRow,
        CListBox* listBoxCPtr)
 {
#ifdef multispec_lin

    long i;
    wxString label("");

    for (i = startRow; i < startRow + numberRows; i++) {
        listBoxCPtr->InsertItem(i, -1); // New row with no image in 1st column
        listBoxCPtr->SetItem(i, 1, label); // Second column is blank text line
    }

#endif
    return (startRow);

} // end "LAddRow"            


#if defined multispec_lin
// This is used in Channel descriptions dialog

SInt16 LAddRow(
        SInt16 numberRows,
        SInt16 startRow,
        wxListBox* listBoxCPtr)
{
	wxString				label("");
	unsigned int		i,
							lastRow;

	lastRow = startRow + numberRows;
	for (i = startRow; i<lastRow; i++) {
		listBoxCPtr->Insert(label, i);
		//listBoxCPtr->Append(label);

		}		// end "for (i = startRow; i<lastRow; i++)"

	return (startRow);

} // end "LAddRow" 
#endif	// defined multispec_lin

void LAutoScroll(
        CListBox* listBoxCPtr)
{


} // end "LAutoScroll" 

#ifdef multispec_lin
void LAutoScroll(
        wxListBox* listBoxCPtr)
{


} // end "LAutoScroll"              
#endif            

Boolean LClick(
        Point location,
        SInt16 modifiers,
        CListBox* listHandle)
 {
    return (FALSE);

} // end "LClick"

void LDelRow(
        SInt16 numberRows,
        SInt16 startRow,
        CListBox* listBoxCPtr)
 {
    SInt16 index,
            returnCode;


    startRow += numberRows - 1;
#ifndef multispec_lin
    for (index = 0; index < numberRows; index++) {
        returnCode = listBoxCPtr->DeleteString(startRow);
        startRow--;

        if (returnCode == LB_ERR || startRow < 0)
            break;

    } // end "for (index=0; index<numberRows; index++)"
#else
    if (startRow <= listBoxCPtr->GetItemCount() - 1)
        for (index = 0; index < numberRows; index++) {
            listBoxCPtr->DeleteItem(startRow);
            startRow--;

            if (startRow < 0)
                break;

        } // end "for (index=0; index<numberRows; index++)"
    //   if(startRow > listBoxCPtr->GetCount()-1)
    //    for (index = 0; index < numberRows; index++) {
    //        listBoxCPtr->Delete(startRow);
    //        startRow--;
    //
    //        if (startRow < 0)
    //            break;
    //
    //    } // end "for (index=0; index<numberRows; index++)"
#endif
} // end "LDelRow" 

#ifdef multispec_lin
void LDelRow(
        SInt16 numberRows,
        SInt16 startRow,
        wxListBox* listBoxCPtr)
 {
    SInt16 index,
            returnCode;

    startRow += numberRows - 1;

    if (startRow <= listBoxCPtr->GetCount() - 1)
        for (index = 0; index < numberRows; index++) {
            //listBoxCPtr->DeleteItem(startRow);
            listBoxCPtr->Delete(startRow);
            startRow--;

            if (startRow < 0)
                break;

        } // end "for (index=0; index<numberRows; index++)"    
} // end "LDelRow" 
#endif	// defined multispec_lin

void LDispose(
        CListBox* listBoxCPtr)
 {
    if (listBoxCPtr != NULL)
        delete listBoxCPtr;

} // end "LDispose"

void LGetCell(
        char*								stringPtr,
        SInt16*							stringLengthPtr,
        Cell								cell,
        CListBox*							listBoxCPtr)
{
#	if defined multispec_win
		USES_CONVERSION;
#	endif

   *stringLengthPtr = 0;
#	ifndef multispec_lin
		if (listBoxCPtr != NULL)
			{		
			*stringLengthPtr = listBoxCPtr->GetText(cell.v, gWideTextString);
			//*stringLengthPtr = listBoxCPtr->GetText(cell.v, (LPTSTR)stringPtr);
			strncpy(stringPtr, T2A(gWideTextString), *stringLengthPtr + 1);
			}
#	else
		if (listBoxCPtr != NULL) 
			{
			wxString value = (listBoxCPtr->GetItemText(cell.v, 1));
			*stringLengthPtr = (SInt16) value.Len();
			strncpy(stringPtr, (const char*) value.mb_str(wxConvUTF8), *stringLengthPtr + 1);
			}
#	endif

} // end "LGetCell"

#if defined multispec_lin
void LGetCell(
        char* stringPtr,
        SInt16* stringLengthPtr,
        Cell cell,
        wxListBox* listBoxCPtr)
 {
    *stringLengthPtr = 0;

    if (listBoxCPtr != NULL) {
        //wxString value = (listBoxCPtr->GetItemText(cell.v, 1));
        wxString value = listBoxCPtr->GetString(cell.v);
        *stringLengthPtr = (SInt16) value.Len();
        strncpy(stringPtr, (const char*) value.mb_str(wxConvUTF8), *stringLengthPtr + 1);
    }

} // end "LGetCell"
#endif	// defined multispec_lin

Boolean LGetSelect(
        Boolean nextFlag,
        Cell* cellPtr,
        CListBox* listBoxCPtr
)
 {
    int cellCount,
            cellIndex,
            selectionCode;

    Boolean selectedFlag = FALSE;
#ifndef multispec_lin


    if (listBoxCPtr != NULL) {
        if (nextFlag) {
            // Now determine if this is a single or multiple
            // selection list.

            cellCount = listBoxCPtr->GetSelCount();
            if (cellCount == LB_ERR) {
                // Single selection list

                cellIndex = listBoxCPtr->GetCurSel();
                if (cellIndex != LB_ERR) {
                    selectionCode = listBoxCPtr->GetSel(cellIndex);
                    if (selectionCode != LB_ERR && selectionCode > 0) {
                        cellPtr->v = cellIndex;
                        selectedFlag = TRUE;

                    } // end "if (selectionCode != LB_ERR && ..."

                } // end "if ( cellIndex != LB_ERR && ...->GetSel(cellIndex) )"

            }// end "if (cellCount == LB_ERR)"

            else // cellCount != LB_ERR
            {
                // Multiple selection list
                // Find the next selection in the list.

                cellIndex = cellPtr->v;
                selectionCode = 0;
                while (selectionCode != LB_ERR && !selectedFlag) {
                    selectionCode = listBoxCPtr->GetSel(cellIndex);
                    if (selectionCode != LB_ERR && selectionCode > 0) {
                        cellPtr->v = cellIndex;
                        selectedFlag = TRUE;

                    } // end "if (selectionCode != LB_ERR && ..."

                    cellIndex++;

                } // end "while (selectionCode != LB_ERR && !selectedFlag)"

            } // end "else cellCount != LB_ERR"

        }// end "if (nextFlag)"

        else // !nextFlag
        {
            // Just return whether the input cell is selected.

            cellIndex = listBoxCPtr->GetSel(cellPtr->v);
            if (cellIndex != LB_ERR && cellIndex > 0)
                selectedFlag = TRUE;

        } // end "else !nextFlag"

    } // end "if (listBoxCPtr != NULL)"
#endif

#if defined multispec_lin
    if (listBoxCPtr != NULL) {
        if (nextFlag) {
            // Now determine if this is a single or multiple
            // selection list.

            //cellCount = listBoxCPtr->GetSelCount ();
            long flag = listBoxCPtr->GetWindowStyle();
            if (flag && wxLC_SINGLE_SEL) {
                // Single selection list

                cellIndex = listBoxCPtr->GetFirstSelected();
                if (cellIndex != -1) {
                    //selectionCode = listBoxCPtr->GetSel(cellIndex);
                    //if (selectionCode != LB_ERR && selectionCode > 0) 
                    //	{
                    cellPtr->v = cellIndex;
                    selectedFlag = TRUE;

                    //	}		// end "if (selectionCode != LB_ERR && ..."

                } // end "if ( cellIndex != LB_ERR && ...->GetSel(cellIndex) )"

            }// end "if (cellCount == LB_ERR)"

            else // cellCount != LB_ERR
            {
                // Multiple selection list
                // Find the next selection in the list.

                cellIndex = cellPtr->v;
                selectionCode = 0;
                selectionCode = listBoxCPtr->GetNextSelected(cellIndex);
                if (selectionCode != -1) {

                    cellPtr->v = cellIndex;
                    selectedFlag = TRUE;

                } // end "while (selectionCode != LB_ERR && !selectedFlag)"

            } // end "else cellCount != LB_ERR" 

        }// end "if (nextFlag)"

        else // !nextFlag
        {
            // Just return whether the input cell is selected.

            selectedFlag = listBoxCPtr->IsSelected(cellPtr->v);

        } // end "else !nextFlag"

    } // end "if (listBoxCPtr != NULL)"
#endif
    return (selectedFlag);

} // end "LGetSelect"


#if defined multispec_lin
Boolean LGetSelect(
        Boolean nextFlag,
        Cell* cellPtr,
        wxListBox* listBoxCPtr
)
 {
    
    int     cellIndex,
            selectionCode;

    Boolean selectedFlag = FALSE;

    if (listBoxCPtr != NULL) {
        if (nextFlag) {
            // Now determine if this is a single or multiple
            // selection list.

            //cellCount = listBoxCPtr->GetSelCount ();
            long flag = listBoxCPtr->GetWindowStyle();            
            if (flag == wxLB_SINGLE) { // Single selection list               
                //cellIndex = listBoxCPtr->GetFirstSelected();
                cellIndex = listBoxCPtr->GetSelection();
                if (cellIndex != -1) {
                    cellPtr->v = cellIndex;
                    selectedFlag = TRUE;

                    //	}		// end "if (selectionCode != LB_ERR && ..."
                } // end "if ( cellIndex != LB_ERR && ...->GetSel(cellIndex) )"
            }// end "if (cellCount == LB_ERR)"

            else // cellCount != LB_ERR
            {
                // Multiple selection list
                // Find the next selection in the list.
                cellIndex = cellPtr->v;
                selectionCode = 0;
                //selectionCode = listBoxCPtr->GetNextSelected(cellIndex);
    
                wxArrayInt selections; 
                selectionCode = listBoxCPtr->GetSelections(selections);
                
                
                //if (selectionCode != -1) {
                  if(selectionCode > cellPtr->h) {
                    //cellPtr->v = cellIndex;          
                    //listBoxCPtr->Deselect(selections[0]);                    
                    cellPtr->v = (SInt16)selections[cellPtr->h];
                    //selections.RemoveAt(selectionCode-1);
                    cellPtr->h++;
                    selectedFlag = TRUE;

                } // end "while (selectionCode != LB_ERR && !selectedFlag)"

            } // end "else cellCount != LB_ERR" 

        }// end "if (nextFlag)"

        else // !nextFlag
        {
            // Just return whether the input cell is selected.

            selectedFlag = listBoxCPtr->IsSelected(cellPtr->v);

        } // end "else !nextFlag"

    } // end "if (listBoxCPtr != NULL)"
    return (selectedFlag);

} // end "LGetSelect"
#endif	// defined multispec_lin

void LSetDrawingMode(
        Boolean flag,
        CListBox* listBoxCPtr)
{

} // end "LSetDrawingMode" 

#ifdef multispec_lin
void LSetDrawingMode(
        Boolean flag,
        wxListBox* listBoxCPtr)
{

} // end "LSetDrawingMode" 
#endif


#ifdef multispec_lin
void LSetCell(
        Ptr dataPtr,
        SInt16 dataLength,
        Cell cell,
        wxListBox* listBoxCPtr)
{
	char		tempPtr[64];
	SInt16	numberListItems;

	strncpy(tempPtr, dataPtr, dataLength);
	tempPtr[dataLength] = 0;
	wxString stringptr = wxString::FromUTF8(tempPtr);
	numberListItems = listBoxCPtr->GetCount();
	 
//	if (numberListItems == 0)
//		listBoxCPtr->Insert(stringptr, 0);
		  
	if (numberListItems > cell.v)
		listBoxCPtr->SetString(cell.v, stringptr);

	else // numberListItems < cell.v            
		listBoxCPtr->Append(stringptr);


} // end "LSetCell"              
#endif

void LSetCell(
        Ptr dataPtr,
        SInt16 dataLength,
        Cell cell,
        CListBox* listBoxCPtr)
 {
#ifndef multispec_lin
    char tempPtr[64];
    SInt16 numberListItems;

#	if defined multispec_win
		USES_CONVERSION;
#	endif

   strncpy(tempPtr, dataPtr, dataLength);
   tempPtr[dataLength] = 0;

   numberListItems = listBoxCPtr->GetCount();
   if (numberListItems >= cell.v) 
		{
      listBoxCPtr->DeleteString(cell.v);
      listBoxCPtr->InsertString(cell.v, (LPCTSTR)A2T(tempPtr));

		} // end "if (numberListItems >= cell.v)" 

   else // numberListItems < cell.v            
		listBoxCPtr->AddString((LPCTSTR)A2T(tempPtr));
#endif

} // end "LSetCell"                

#if defined multispec_lin
// Index are zero based rows in the list

void LSetCell(
        int mvindex,
        int targetindex,
        CListBox* listBoxCPtr)
 {
    wxListItem mvitem;
    SInt16 numberListItems;
    SInt16 numberCols = listBoxCPtr->GetColumnCount();


    numberListItems = listBoxCPtr->GetItemCount();
    for (int i = 0; i < numberCols; i++) {
        mvitem.SetId(mvindex);
        mvitem.SetMask(-1);
        mvitem.SetColumn(i);
        listBoxCPtr->GetItem(mvitem);
        mvitem.SetId(targetindex);
        if (i == 0)
            listBoxCPtr->InsertItem(mvitem);
        else
            listBoxCPtr->SetItem(mvitem);
    }

} // end "LSetCell"  
#endif

void LSetSelect(
        Boolean setItFlag,
        Cell cell,
        CListBox* listBoxCPtr)
 {
#ifndef multispec_lin
    SInt16 returnCode;


    returnCode = listBoxCPtr->SetSel(cell.v, setItFlag);

#endif

} // end "LSetSelect" 
               

#if defined multispec_lin
void LSetSelect(
        Boolean setItFlag,
        Cell cell,
        wxListBox* listBoxCPtr)
{
			// A count of 0 indicates that the list box is hidden
			
	if (listBoxCPtr->GetCount() > 0)
		{
		if (setItFlag)
			listBoxCPtr->SetSelection(cell.v);
		else
        listBoxCPtr->Deselect(cell.v);
		}
}
#endif	// defined multispec_lin



void MosaicImagesSideBySideControl(void)
 {

} // end "MosaicImagesSideBySideControl"

RgnHandle NewRgn(void)
 {
    return ( (RgnHandle) MNewHandle(sizeof (RgnHandle)));

} // end "NewRgn"

Boolean PixMap32Bit(
        Handle offScreenPixMapH)
 {
    return (FALSE);

} // end "PixMap32Bit"


Boolean PtInRgn(
				Point								point,
				RgnHandle						rgnHandle)
{
	#ifndef multispec_lin
		return (rgnHandle->PtInRegion(point.h, point.v));
	#else
		if (rgnHandle != NULL)
			return (rgnHandle->Contains(point.h, point.v));
		else
			return (FALSE);
	#endif
} // end "PtInRgn"


void ReleaseResource(
        Handle handle)
 {
    UnlockAndDispose(handle);

} // end "ReleaseResource" 	

SInt16 ResError(void)
 {
    return (0);

} // end "ResError" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2001)
//								(c) Purdue Research Foundation
//									All rights reserved.
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
// TODO: For Linux after basic gui setup


void SetPenCharacteristics(
        CPen* cPenPtr,
        RGBColor* rgbColorPtr,
        SInt16 lineThickness)
 {
   #ifndef multispec_lin
    LOGPEN logpen;

    CPen* oldPenPtr = NULL;

    //   LOGPEN* 								logpenPtr;

    Boolean continueFlag;


    logpen.lopnStyle = PS_SOLID;
    logpen.lopnWidth.x = lineThickness;
    logpen.lopnWidth.y = lineThickness;

    logpen.lopnColor = RGB(rgbColorPtr->red / 256,
            rgbColorPtr->green / 256,
            rgbColorPtr->blue / 256);

    continueFlag = cPenPtr->CreatePenIndirect(&logpen);

    /*
            cPenPtr->GetLogPen (logpenPtr);

            logpenPtr->lopnWidth.x = lineThickness;
            logpenPtr->lopnWidth.y = lineThickness;
	
            logpenPtr->lopnColor = RGB(rgbColorPtr->red/256, 
                                                                                    rgbColorPtr->green/256, 
                                                                                    rgbColorPtr->blue/256);
     */
    oldPenPtr = gCDCPointer->SelectObject(cPenPtr);
#endif
    
} // end "SetPenCharacteristics" 


void SetControlValue(
        ControlHandle controlHandle,
        SInt16 setting)
 {

} // end "SetControlValue"                                            

// TODO: Find a way to select the chars from startselection to endselection

void SelectDialogItemText(
        DialogPtr dialogPtr,
        SInt16 itemNumber,
        SInt16 startSelection,
        SInt16 endSelection)
 {
#ifndef multispec_lin
    dialogPtr->GetDlgItem(itemNumber)->SetFocus();
    //	dialogPtr->SendDlgItemMessage( 
    //										itemNumber, EM_SETSEL, 0, MAKELPARAM(0, -1) );
    dialogPtr->SendDlgItemMessage(
            itemNumber, EM_SETSEL, startSelection, endSelection);
#else
    wxWindow* dialogObject = (dialogPtr)->FindWindow(itemNumber);
    if (dialogObject != NULL)
        dialogObject->SetFocus();
    //dialogPtr->FindWindow(itemNumber)->SetFocus();
#endif
} // end "SelectDialogItemText" 

void SetDialogItemText(
        Handle theHandle,
        ConstStr255Param textStringPtr)
 {

} // end "SetDialogItemText" 

void SetEntryUsage(
        CMPaletteInfo paletteObject,
        SInt16 paletteIndex,
        SInt16 srcUsage,
        SInt16 srcTolerance)
 {
#ifndef multispec_lin
    PALETTEENTRY paletteEntry;

    paletteObject->GetPaletteEntries(paletteIndex, 1, &paletteEntry);

    paletteEntry.peFlags = (UInt8) srcUsage;

    paletteObject->SetPaletteEntries(paletteIndex, 1, &paletteEntry);
#endif
} // end "SetEntryUsage"                                

void SetFileReadOnly(
        FileInfoPtr fileInfoPtr)
 {

} // end "SetFileReadOnly"                  

void SetMenuItemText(
        MenuHandle menuHandle,
        SInt16 menuItem,
        UCharPtr stringPtr)
 {

} // end "SetMenuItemText"

void SetPalette(
        WindowPtr windowPtr,
        CMPaletteInfo paletteObject,
        Boolean activateFlag)
 {

} // end "SetPalette"             

void SetPort(
        GrafPtr windowPtr)
 {
    //	gCDCPointer = (WindowPtr)windowPtr;

} // end "SetPort"               

void SetPort(
        WindowPtr windowPtr)
 {
    //	gCDCPointer = (WindowPtr)windowPtr;

} // end "SetPort"             

void SetPort(
        DialogPtr dialogPtr)
 {
    //	gCDCPointer = (WindowPtr)windowPtr;

} // end "SetPort"               

void SetPortDialogPort(
        DialogPtr windowPtr)
 {
    //	gCDCPointer = (WindowPtr)windowPtr;

} // end "SetPortDialogPort"            

void SetPortWindowPort(
        WindowPtr windowPtr)
 {
    //	gCDCPointer = (WindowPtr)windowPtr;

} // end "SetPortWindowPort"        

SInt16 SetThreadState(
        UInt32 asyncIOThread,
        SInt16 threadState,
        UInt32 IOThread)
 {
    return (noErr);

} // end "SetThreadState"    

void SetWTitle(
        DialogPtr dialogPtr,
        unsigned char* textStringPtr)
 {
#ifndef multispec_lin
    dialogPtr->SetWindowText ((LPCTSTR)&textStringPtr[1]);
#else
    wxString newtitle(textStringPtr, wxConvUTF8);
    dialogPtr->SetLabel(newtitle);
#endif

} // end "SetWTitle"                 

void ShowDialogItem(
        DialogPtr dialogPtr,
        SInt16 itemNumber)
 {
#ifndef multispec_lin
    CWnd* dialogItemWindowPtr = dialogPtr->GetDlgItem(itemNumber);

    if (dialogItemWindowPtr != NULL)
        dialogItemWindowPtr->ShowWindow(SW_SHOW);
#else
    wxWindow* dialogItemWindowPtr = dialogPtr->FindWindow(itemNumber);
    if (dialogItemWindowPtr != NULL)
        dialogItemWindowPtr->Show(true);
#endif

} // end "ShowDialogItem"


void ShowDialogWindow(
        DialogPtr					dialogPtr,
        SInt16						dialogID,
        Boolean					setUpFilterTableFlag)
{
	#if defined multispec_win
		dialogPtr->UpdateWindow();
	#endif
	
	#if defined multispec_lin
		dialogPtr->Show(true);
		dialogPtr->Update();
	#endif
	
} // end "ShowDialogWindow"


void ShowSelection(
        WindowPtr windowPtr)
	{
	if (windowPtr != NULL)
		windowPtr->UpdateSelectionCoordinates();

	} // end "ShowSelection" 


void ShowSelection(
        Handle windowInfoHandle)
 {
    WindowInfoPtr windowInfoPtr = (WindowInfoPtr) GetHandlePointer(windowInfoHandle,
            kNoLock,
            kNoMoveHi);

    if (windowInfoPtr != NULL) {
        CMImageView* imageViewCPtr = windowInfoPtr->cImageWindowPtr->mImageViewCPtr;
        imageViewCPtr->UpdateSelectionCoordinates();

    } // end "if (windowInfoPtr != NULL)"

} // end "ShowSelection"
 

void ShowStatusDialogWindow(
        SInt16 statusDialogID)
{
	if (gStatusDialogPtr != NULL)
		{
#		ifdef multispec_lin
			gStatusDialogPtr->Show(true);  
			gStatusDialogPtr->Update();
#		endif
		gStatusDialogPtr->SetFocus();
		CheckSomeEvents (updateMask+activMask);
		
		}		// end "if (gStatusDialogPtr != NULL)"

}		// end "ShowStatusDialogWindow" 



//-----------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
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
//	Coded By:			Larry L. Biehl			Date: 01/16/97
//	Revised By:			Larry L. Biehl			Date: 12/12/2000			
// TODO: For Linux

Boolean StillDown(void)
 {
#ifndef multispec_lin
    MSG msgCur;


    return ( !::PeekMessage(&msgCur, NULL, WM_LBUTTONUP, WM_LBUTTONUP, PM_NOREMOVE));
#endif

} // end "StillDown"


// TODO: For Linux (Problem in TextWidth)

SInt16 StringWidth(
        UCharPtr stringPtr)
 {
    SInt16 characterCount,
            firstCharacter = 1,
            width = 30;
#ifndef multispec_lin


    if (stringPtr != NULL) {
        characterCount = stringPtr[0];

        width = TextWidth(stringPtr, firstCharacter, characterCount);

    } // end "if (stringPtr != NULL)"
#endif
    return (width);

} // end "StringWidth" 

SInt16 StatHistogramPopUpMenu(
        SInt16 statsWindowMode)
 {
    return (0);

} // end "StatHistogramPopUpMenu"

void SysBeep(
        SInt16 beepLength)
 {

} // end "SysBeep"    

void SwapMMUMode(
        SInt8* addressModePtr)
 {

} // end "SwapMMUMode"       

void UnloadSeg(
        Ptr funtionPointer)
 {

} // end "UnloadSeg"           

void UpdateImageZoomControls(
        WindowPtr theWindow,
        double magnification,
        Boolean updateZoomsFlag)
{
	#if defined multispec_lin
		// Make sure the zoom information in the status bar is up to date

		CMainFrame* pMainFrame = (CMainFrame*) wxGetApp().GetTopWindow();
		pMainFrame->UpdateStatusBar(magnification);

				// Update the scale factor in the coordinate view if displayed.
			
		Handle windowInfoHandle = GetWindowInfoHandle(theWindow);
		UpdateScaleInformation (windowInfoHandle);
	#endif

} // end "UpdateImageZoomControls"            

void UpdateOutputWScrolls(
        WindowPtr theWindow,
        SInt16 scrollToSelStart,
        Boolean displayMessageFlag)
 {
#ifdef multispec_lin   
//   gOutputViewCPtr->m_textsw->HideSelection(true);
   gOutputViewCPtr->m_textsw->SetInsertionPointEnd();
    gOutputViewCPtr->m_textsw->Home();
//   gOutputViewCPtr->m_textsw->SetSelectionEnd(gOutputViewCPtr->m_textsw->GetTargetEnd()); 	
   gOutputViewCPtr->m_textsw->ScrollToEnd(); 	
   
#endif
   
} // end "UpdateOutputWScrolls"             

void UpdateOverlayControl(
        WindowPtr theWindow)
 {

} // end "UpdateOverlayControl" 

void UpdatePaletteWindow(
        Boolean paletteWindowChanged,
        Boolean forceDrawPaletteWindow)
 {

} // end "UpdatePaletteMenu"

void UpdateStatWEditMenu(void)
 {

} // end "UpdateStatWEditMenu" 

void ValidRect(
        Rect* rectPtr)
 {

} // end "ValidRect"

SInt16 YieldToThread(
        UInt32 wakeUpThread)
 {
    return (noErr);

} // end "YieldToThread"    
