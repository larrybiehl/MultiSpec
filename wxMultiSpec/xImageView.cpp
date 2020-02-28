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
//	File:						xImageView.cpp : class implementation file
//	Class Definition:		xImageView.h
//
//	Authors:					Larry L. Biehl, Wei-Kang Hsu, Tsung Tai Yeh
//
// Revision date:			03/12/2016 by Wei-Kang Hsu
// Revision date:			12/19/2018 by Tsung Tai Yeh
// Revision date:			01/20/2020  by Larry L Biehl
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the
//								CMImageView class.
//
/* Template for debugging
	int numberChars = sprintf ((char*)gTextString3,
				" xImageView:: (): %s",
				gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "SDisplay_class.h"
#include "SImageWindow_class.h"

#include "xImageCanvas.h"
#include "xImageDoc.h"
#include "xImageFrame.h"
#include "xImageView.h"
#include "xMainFrame.h"
#include "xMultiSpec.h"
#include "xTools.h"


BEGIN_EVENT_TABLE (CMImageView, wxView)
   EVT_KEY_DOWN (CMImageView::OnKeyDown)
	EVT_SET_FOCUS (CMImageView::OnFocus)
END_EVENT_TABLE ()
						

IMPLEMENT_DYNAMIC_CLASS (CMImageView, wxView)

RECT		CMImageView::s_updateRect;

CMImageView::CMImageView ()

{  
	m_displayMultiCPtr = NULL;
	m_frame = NULL;
	m_histogramCPtr = NULL;
	m_imageWindowCPtr = NULL;
	m_activeFlag = FALSE;
	
			// The following two flags are currently used for selections.
	m_ctlKeyDownFlag = FALSE;
	m_shiftKeyDownFlag = FALSE;

	m_printCopyModeFlag = FALSE;
		
	m_cursorColumnValue = -1;
	m_cursorLineValue = -1;
	
	m_printerTextScaling = 1.; 
	
	wxScreenDC dc;
	m_xPixelsPerInch = (SInt16)(dc.GetPPI()).GetWidth ();
	m_yPixelsPerInch = (SInt16)(dc.GetPPI()).GetHeight ();

	m_initializedFlag = FALSE;
	m_startchannel_sbs = 0;
   m_Scale = 1.0;
	
	try
		{
				// Create Image Window Object for this image.

		Handle fileInfoHandle = ((CMultiSpecApp*)wxTheApp)->GetOpenImageFileInfoHandle();

		m_imageWindowCPtr = new CMImageWindow (fileInfoHandle);

		m_initializedFlag = m_imageWindowCPtr->m_initializedFlag;

		if (m_initializedFlag) 
			{
					// Create Display Image Object for this image.

			m_displayMultiCPtr = new CMDisplay;

			m_initializedFlag = m_displayMultiCPtr->m_initializedFlag;

			}	// end "if (m_initializedFlag)"

		if (m_initializedFlag) 
			{
					// Create Histogram Object for this image.

			m_histogramCPtr = new CMHistogram;

			m_initializedFlag = m_histogramCPtr->m_initializedFlag;

			}	// end "if (m_initializedFlag)"

		if (m_initializedFlag)
			UpdateWindowList (this, kImageWindowType);

		}	// end "try"

    catch (int e)
		{
		MemoryMessage (0, kObjectMessage);
		}
	
}	// end "CMImageView"



CMImageView::~CMImageView (void)

{  
	if (m_displayMultiCPtr != NULL)
		delete m_displayMultiCPtr;
	
	if (m_histogramCPtr != NULL)
		delete m_histogramCPtr;
		
	if (this == gProjectSelectionWindow)
		{
		gProjectSelectionWindow = NULL;
		
		ClearNewFieldList ();
		
		}	// end "if (this == gProjectSelectionWindow)"
	
			// Delete image window class  
	
	if (m_imageWindowCPtr != NULL)
		delete m_imageWindowCPtr;
		
	if (gActiveImageViewCPtr == this)
		{
		gActiveImageViewCPtr = NULL;
		gActiveImageWindow = NULL; 
		gActiveWindowType = -1;
		gTheActiveWindow = NULL;
		                                                      
		gActiveImageWindowInfoH = NULL;
		gActiveImageFileInfoH = NULL; 
		gActiveImageLayerInfoH = NULL;
		
		gActiveLegendWidth = 0;
				
		}	// end "if (gActiveImageViewCPtr == this)"
	
	m_frame->SetImageViewCPtr (NULL);
		
	RemoveWindowFromList (this, 0);
		
}	// end "~CMImageView"



Boolean CMImageView::CheckIfOffscreenImageExists (void)

{  
	Boolean								returnFlag = FALSE;
	
	         
			// Verify that an offscreen images exists.
	
	if (m_imageWindowCPtr != NULL)
		{																								
		WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
													m_imageWindowCPtr->GetWindowInfoHandle ());
	
		if (windowInfoPtr != NULL)                                                   
			returnFlag = (windowInfoPtr->imageBaseAddressH != NULL);
			
		}	// end "if (this != NULL && m_imageWindowCPtr != NULL)"

	return (returnFlag);
	
}	// end "CheckIfOffscreenImageExists"



Boolean CMImageView::CheckIfVectorOverlaysExist (void)

{
	UInt32								numberVectorOverlays;
	
	Boolean								returnFlag = FALSE;
	
	
			// Verify that vector overlays exist.
	
	if (m_imageWindowCPtr != NULL)
		{
		numberVectorOverlays =
			GetNumberWindowVectorOverlays (m_imageWindowCPtr->GetWindowInfoHandle ());
	
		returnFlag = (numberVectorOverlays > 0);
		
		}	// end "if (this != NULL && m_imageWindowCPtr != NULL)"

	return (returnFlag);
	
}	// end "CheckIfVectorOverlaysExist"



void CMImageView::ClearView (
				bool 									flag)

{
	m_Canvas->EraseBackground (flag);
	
}	// end "ClearView"



void CMImageView::ClientToDoc (
				wxPoint& 							point)

{
    wxClientDC dc (m_Canvas);
    m_Canvas->DoPrepareDC (dc);
    point.x = dc.DeviceToLogicalX (point.x);
    point.y = dc.DeviceToLogicalY (point.y);

}	// end "ClientToDoc"



void CMImageView::CreateScaledBitmap ()

{
		 // For now, GetOffScreenImage routine in LUtility.cpp
		 // is directly setting up m_ScaledBitmap
	
	m_ScaledBitmap = wxNullBitmap;
	
}	// end "CreateScaledBitmap"



void CMImageView::DisposeImageWindowSupportMemory (void)

{
	if (m_histogramCPtr != NULL)
		m_histogramCPtr->ReleaseHistogramSupportMemory ();
	
	if (m_displayMultiCPtr != NULL)
		m_displayMultiCPtr->ReleaseDisplaySupportMemory ();
	
	m_imageWindowCPtr->ReleaseOffscreenSupportMemory ();
	
}	// end "DisposeImageWindowSupportMemory"


/*
		// Use when printing is implemented in wxWidgets versions.
void CMImageView::DoFilePrint ()

{
    gProcessorCode = kPrintProcessor;
    CScrollView::OnFilePrint ();
    gProcessorCode = 0;

}	// end "DoFilePrint"



void CMImageView::DoFilePrintPreview ()

{
    gProcessorCode = kPrintProcessor;
    CScrollView::OnFilePrintPreview ();
    gProcessorCode = 0;

}	// end "DoFilePrintPreview"
*/



void CMImageView::DrawLegend ()

{
	CMLegendView* legendViewCPtr =
								GetDocument()->GetImageFrameCPtr()->GetLegendViewCPtr ();
	legendViewCPtr->UpdateThematicLegendControls ();
	CMLegendList* legendListPtr = (CMLegendList*)GetActiveLegendListHandle ();
	if (legendListPtr != NULL)
		{
		legendListPtr->DrawLegendList ();

		}	// end "if (legendListPtr != NULL)"

}	// end "DrawLegend"



Boolean CMImageView::GetActiveWindowFlag (void)

{
	return (GetImageFrameCPtr()->GetActiveWindowFlag ());

}	// end "GetActiveWindowFlag"



SInt16 CMImageView::GetClassGroupCode (void)

{  
	SInt16		classGroupCode = 0;
	
	         
		// Get the class/group code.
	
	if (m_displayMultiCPtr != NULL)
		{
		DisplaySpecsPtr displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
													m_displayMultiCPtr->mDisplaySpecsHandle);
		
		if (displaySpecsPtr != NULL && !displaySpecsPtr->initializeStructureFlag)
			classGroupCode = displaySpecsPtr->classGroupCode;
		
		}	// end "if (m_displayMultiCPtr != NULL)"
			
	return (classGroupCode);
	
}	// end "GetClassGroupCode"



Boolean CMImageView::GetControlKeyFlag (void)

{
	return (m_ctlKeyDownFlag);

}	// end "GetContolKeyFlag"



UInt16 CMImageView::GetDisplayPixelSize (void)

{
    return (GetImageFrameCPtr()->GetDisplayPixelSize ());

}	// end "GetDisplayPixelSize"



Handle CMImageView::GetDisplaySpecsHandle (void)

{
	Handle displaySpecsHandle = NULL;

	if (m_displayMultiCPtr != NULL)
		displaySpecsHandle = m_displayMultiCPtr->mDisplaySpecsHandle;

	return (displaySpecsHandle);

}	// end "GetDisplaySpecsHandle"



CMImageFrame* CMImageView::GetImageFrameCPtr (void)

{
	return (m_frame);

}	// end "GetImageFrameCPtr"



CMLegendList* CMImageView::GetImageLegendListCPtr (void)

{
	CMLegendList* 						legendListCPtr = NULL;
	

	CMLegendView* legendViewCPtr = m_frame->GetLegendViewCPtr ();

	if (legendViewCPtr != NULL)
		legendListCPtr = legendViewCPtr->GetLegendListCPtr ();

    return (legendListCPtr);

}	// end "GetImageLegendListCPtr"



CMLegendView* CMImageView::GetImageLegendViewCPtr (void)

{
    return (m_frame->GetLegendViewCPtr ());

}	// end "GetImageLegendViewCPtr"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetImageType
//
//	Software purpose:	The purpose of this routine is to return the image type
//							variable for this window object. This was originally in
//							the Windows code.
//
//	Parameters in:		None			
//
//	Parameters out:	None			
//
//	Value Returned:	0 if now window structure exists
//							window type if others such as kThematicType, or kImageType.
// 
// Called By:						
//
//	Coded By:			Larry L. Biehl			Date: 11/21/2006
//	Revised By:			Larry L. Biehl			Date: 11/21/2006	

SInt16 CMImageView::GetImageType (void)

{  
	SInt16		imageType = 0;
	
	         
			// Get the window type. 
	                              
	if (m_imageWindowCPtr != NULL)
		imageType = ::GetImageType (m_imageWindowCPtr->GetWindowInfoHandle ());
				
	return (imageType);
	
}	// end "GetImageType"


/*
// Used if copying images to clipboard is implemented
SInt16 CMImageView::GetLegendFullHeight (void)

{
	CRect				legendRect,
						listRect;
	
	Handle 			windowInfoHandle;
	
	SInt16			legendFullHeight;
	

	windowInfoHandle = GetWindowInfoHandle (this);
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	legendFullHeight = GetListBottom (windowInfoPtr->legendListHandle);
	CheckAndUnlockHandle (windowInfoHandle);
	
	if (windowInfoPtr->legendListHandle != NULL)
		{                                
		GetWindowRect (legendRect); 
		(GetImageLegendListCPtr())->GetWindowRect (listRect);
		
		legendFullHeight += (SInt16)(listRect.top - legendRect.top);
		
		}	// end "if (windowInfoPtr->legendListHandle != NULL)"
	
	return (legendFullHeight); 


}	// end "GetLegendFullHeight"
*/


SInt16 CMImageView::GetLegendWidth (void)

{	
	Handle windowInfoHandle = GetWindowInfoHandle (this);
	
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
											
	if (windowInfoPtr != NULL)
		return (windowInfoPtr->legendWidth);
		
	else	// windowInfoPtr != NULL
		return (NULL);
												   
	//return (m_legendWidth);
	
}	// end "GetLegendWidth"



SInt16 CMImageView::GetNumberGroups (void)

{  
	SInt16		numberGroups = 0;
	
	         
			// Get the number of groups.
	                              
	if (m_imageWindowCPtr != NULL)
		{
		Handle windowInfoHandle = m_imageWindowCPtr->GetWindowInfoHandle ();
		Handle fileInfoHandle = GetFileInfoHandle (windowInfoHandle);
		
		FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
		
		if (fileInfoPtr != NULL)
			numberGroups = fileInfoPtr->numberGroups;
		
		}	// end "if (m_imageWindowCPtr != NULL)"

	return (numberGroups);
	
}	// end "GetNumberGroups"



wxBitmap CMImageView::GetScaledBitmap ()

{
	return m_ScaledBitmap;
	
}	// end "GetScaledBitmap"



Boolean CMImageView::GetShiftKeyFlag (void)

{
	return (m_shiftKeyDownFlag);

}	// end "GetShiftKeyFlag"



SInt16 CMImageView::GetTitleHeight (void)

{ 
	SInt16 					titleHeight;
	
	
	Handle windowInfoHandle = GetWindowInfoHandle (this);
	titleHeight = ::GetTitleHeight (windowInfoHandle);
	     
	return (titleHeight);
	
}	// end "GetTitleHeight"



const wxSize& CMImageView::GetViewOffset ()

{
	return m_ViewOffset;
	
}	// end "GetViewOffset"



SInt16 CMImageView::GetWindowType (void)

{  
	SInt16					windowType = 0;
	
	         
			// Get the window type. 
	                              
	if (m_imageWindowCPtr != NULL)
		windowType = ::GetWindowType (m_imageWindowCPtr->GetWindowInfoHandle ());
	
	return (windowType);
	
}	// end "GetWindowType"



Boolean CMImageView::ImageWindowIsAvailable (void)

{  
	Boolean								returnFlag = FALSE;
	
	         
			// Verify that an offscreen images exists.
	
	if (m_imageWindowCPtr != NULL)
		returnFlag =
					::ImageWindowIsAvailable (m_imageWindowCPtr->GetWindowInfoHandle ());

	return (returnFlag);
	
}	// end "ImageWindowIsAvailable"



void CMImageView::InitialUpdate (void)

{
	SInt16								windowType;


	windowType = GetWindowType ();

	if (windowType == kThematicWindowType)
		{
		m_frame->InitialUpdate (GetDocument()->GetDocSize ());

		}	// end "if (windowType == kThematicWindowType)"

	if (m_imageWindowCPtr != NULL)
		{
				// Allow ability to go from image window class back to image view class.

		m_imageWindowCPtr->mImageViewCPtr = this;
		
		gActiveImageViewCPtr = this;
		gActiveImageWindow = this;
		gActiveWindowType = kImageWindowType;
		gTheActiveWindow = (WindowPtr)this;

		gActiveImageWindowInfoH = GetActiveImageWindowInfoHandle ();
		gActiveImageFileInfoH = GetActiveImageFileInfoHandle ();
		gActiveImageLayerInfoH = GetActiveImageLayerInfoHandle ();

		if (windowType == kThematicWindowType)
			{
					// This is a thematic type image. Delete the multispectral
					// object and get a thematic object.

			delete m_displayMultiCPtr;

			m_displayMultiCPtr = (CMDisplay*)new CMDisplay;

			}	// end "if (windowType == kThematicWindowType)"

		else	// windowType != kThematicWindowType
			{
					// Save this image view in the frame class for non-thematic images.
					// It was already saved for thematic type images when the splitter
					// view were created.

			CMImageFrame* imageFrameCPtr = m_frame;
			imageFrameCPtr->SetImageViewCPtr (this);

			}	// end "else windowType != kThematicWindowType"

   	}	// end "if (m_imageWindowCPtr != NULL)"
		
	m_frame->m_coordinatesBar->InitialUpdate ();
	
			// Get coordinate bar height
			//		Note: do not set the size of the coordinate bar in the window
			// 	information structure. The coordinate bar is not a part of the image
			//		window in wx version. It is a separate panel within the image frame.
			//		Doing so will cause problems later when drawing selections and
			//		fields.
	
   ShapeInfoPtr shapeInfoPtr = NULL;
	if (gActiveImageWindow != NULL && gSelectionGraphViewCPtr != NULL)
		{         
      shapeInfoPtr = gSelectionGraphViewCPtr->GetShapeInfoFromHandle (gShapeFilesHandle, gActiveImageWindowInfoH);
      gSelectionGraphViewCPtr->UpdateDataListCtrl ();
      gSelectionGraphViewCPtr->UpdateShowOrHideFeatureList ();

		}	// end "if (gActiveImageWindow != NULL && ..."
   
}	// end "InitialUpdate"



void CMImageView::InvalidateRect (
				Rect* 								rectp,
				bool 									erase)

{
	Rect 									rect = *rectp;
	
	
	m_Canvas->RefreshRect (wxRect (rect.left,
    											rect.top,
    											(rect.right - rect.left),
    											(rect.bottom - rect.top)),
    											erase);

}	// end "InvalidateRect"



void CMImageView::InvalidateRect (
				wxRect* 								rectp,
				bool 									erase)

{
    wxRect rect = *rectp;

    m_Canvas->RefreshRect (rect, erase);

}	// end "InvalidateRect"



SInt32 CMImageView::NormalizeScrollOffset (
				double 								magnification,
				SInt32 								scrollIncrement,
				SInt32 								scrollOffset)

 {
	SInt32 								normalizedIncrement;


    normalizedIncrement =
            (SInt32) (((double)scrollOffset + scrollIncrement) / magnification + .5);
    normalizedIncrement = MAX (0, normalizedIncrement);

    normalizedIncrement =
    				(SInt32)((double) normalizedIncrement * magnification - scrollOffset);

    return (normalizedIncrement);

}	// end "NormalizeScrollOffset"


		// This function is called when we call wxView::Activate ()
		// Inherited from wxView

void CMImageView::OnActivateView (
				bool 									bActivate,
				wxView* 								pActivateView,
				wxView* 								pDeactiveView)

{
	if (m_frame != NULL)
		{
		m_activeFlag = (Boolean)bActivate;
		if (bActivate)
			wxASSERT (pActivateView == (wxView*)this);
		  
				// If the window is being activated, outside of a processing operating,
				// make sure the global active image information is up to date.
		
		if (gProcessorCode == 0 && bActivate)
			m_frame->UpdateActiveImageWindowInfo ();
			
		Boolean changeWindowFlag = TRUE;
			
		m_frame->ActivateImageWindowItems (bActivate, changeWindowFlag);
		
		}	// end "if (m_frame != NULL)"

}	// end "OnActivateView"



bool CMImageView::OnClose (
				bool 									deleteWindow)

{
	if (!((CMultiSpecApp*)wxTheApp)->IsActive ())
		{
			// Treat this as just a mouse down to activate the application and window.
		
		// Activate (true);  // This did not work to bring the
 																							return false;
		
		}	// end "if (!((CMultiSpecApp*)wxTheApp)->IsActive ())"
	
	if (!GetDocument()->Close ())
 																							return false;

	if (m_Canvas != NULL)
		{
		m_Canvas->SetView (NULL);
		m_Canvas = NULL;
		
		}	// end "if (m_Canvas != NULL)"

	if (m_frame != NULL)
		m_frame->SetTitle (CMMainFrameTitle);

	Activate (false);
	SetFrame (NULL);
	
	return true;
	
}	// end "OnClose"



bool CMImageView::OnCreate (
				wxDocument*							doc,
				long 									WXUNUSED (flags))

{
	m_Canvas = NULL;
	SetDocument (doc);
	m_mainFrame = wxDynamicCast (wxTheApp->GetTopWindow(), CMainFrame);

	if (m_mainFrame != NULL)
		{
		m_frame = wxDynamicCast (wxGetApp().CreateChildFrame (GetDocument(), this),
											CMImageFrame);
		CreateScaledBitmap ();
		
		m_Canvas = new CMImageCanvas (m_frame->m_mainWindow);
		
		m_frame->m_coordinatesBar = new CMCoordinateBar (m_frame->m_topWindow);
		
		m_frame->m_titleBar = new CMTitleBar (m_frame->m_titleWindow);
		m_frame->m_titleBar->m_frame = m_frame;
		m_frame->m_titleBar->m_view = this;

		m_frame->m_imageLegendViewCPtr = new CMLegendView (m_frame->m_leftWindow,
																			IDD_LegendWindow,
																			doc,
																			this);
		SetFrame (m_frame);
		m_Canvas->SetView (this);
		(m_frame->m_imageLegendViewCPtr)->SetImageView (this);
		m_frame->Show (true);
		((CMImageDoc*)doc)->SetImageFrameCPtr (m_frame);
		Activate (true);
		
		}	// end "if (m_mainFrame != NULL)"
	
	else	// m_mainFrame == NULL
		wxFAIL_MSG (_("Can't retrieve a pointer to main window"));
		
	m_imageWindowCPtr->mImageViewCPtr = this;
	((CMImageDoc*)doc)->SetImageWindowCPtrV (this);

	InitialUpdate ();
	
	return true;
	
}	// end "OnCreate"



void CMImageView::OnDraw (
				CDC* 									pDC)

{
	Rect 									sourceRect;
	SInt16 								copyType;
	

	if (ImageWindowIsAvailable ())
		{
		pDC->SetMapMode (wxMM_TEXT);
		#if defined multispec_wxmac
			wxGraphicsContext* graphicsContext = pDC->GetGraphicsContext ();
			graphicsContext->SetInterpolationQuality (wxINTERPOLATION_NONE);
		#endif
		
		sourceRect.top = s_updateRect.top;
		sourceRect.bottom = s_updateRect.bottom;
		sourceRect.left = s_updateRect.left;
		sourceRect.right = s_updateRect.right;
		
		m_xPixelsPerInch = (SInt16)(pDC->GetPPI()).GetWidth ();
		m_yPixelsPerInch = (SInt16)(pDC->GetPPI()).GetHeight ();

		//copyType = kDestinationCopy;
		copyType = kSourceCopy;

      wxPoint scrollOffset;
		m_Canvas->CalcUnscrolledPosition (0, 0, &scrollOffset.x, &scrollOffset.y);
            
		sourceRect.top += scrollOffset.y;
		sourceRect.bottom += scrollOffset.y;
		sourceRect.left += scrollOffset.x;
		sourceRect.right += scrollOffset.x;
		/*
				// For debugging
		 
		int numberChars = sprintf ((char*)gTextString3,
				" xImageView:OnDraw (updateRect left right top bottom): %d, %d, %d, %d%s",
				sourceRect.left,
				sourceRect.right,
				sourceRect.top,
				sourceRect.bottom,
				gEndOfLine);
		ListString ((char*)gTextString3, numberChars, gOutputTextH);
		*/
		gCDCPointer = pDC;
		
		wxPoint deviceOriginSaved  = gCDCPointer->GetDeviceOrigin ();
		wxPoint deviceOrigin = deviceOriginSaved;
		
		if (scrollOffset.x < 0)
			deviceOrigin.x += scrollOffset.x;
		
		if (scrollOffset.y < 0)
			deviceOrigin.y += scrollOffset.y;

		gCDCPointer->SetDeviceOrigin (deviceOrigin.x, deviceOrigin.y);
		
		CopyOffScreenImage (this, pDC, m_imageWindowCPtr, NULL, &sourceRect, copyType);
		
		gCDCPointer->SetDeviceOrigin (deviceOriginSaved.x, deviceOriginSaved.y);
		
		gCDCPointer = NULL;

		}	// end "if (ImageWindowIsAvailable ())"

}	// end "OnDraw"



void CMImageView::OnFocus (
				wxFocusEvent&						event)

{
	event.Skip (true);
	
}	// end "OnFocus"



void CMImageView::OnKeyDown (
				wxKeyEvent& 						event)

{
	SInt16								windowType;
	
	Boolean								hasCaptureFlag;
	
	
			// Only handle if this window is not captured and a processor is not
			// in operation.
	
	hasCaptureFlag = m_Canvas->HasCapture ();
	
   if (!hasCaptureFlag && gProcessorCode == 0)
		{
				// Also only handle if cursor is over image portion of a thematic image
				// window.
		
		windowType = GetWindowType ();
		
		}	// end "if (!hasCaptureFlag && gProcessorCode == 0)"

}	// end "OnKeyDown"



void CMImageView::OnUpdate (
				wxView*								sender,
				wxObject*							hint)

{
    wxView::OnUpdate (sender, hint);
	
    m_Canvas->Refresh ();
				 
}	// end "OnUpdate"



void CMImageView::ScrollChanged ()

{
	UpdateOffscreenMapOrigin ();
	
			// If title bar is showing (side by side image display), the
			// title bar may need to be updated.
	
	if (m_frame->m_titleBar->IsShown ())
		{
		wxRect titleBarRect = m_frame->m_titleBar->GetClientRect ();
		(m_frame->m_titleBar)->RefreshRect (wxRect (titleBarRect.GetLeft (),
																	titleBarRect.GetTop () + 1,
																	titleBarRect.GetRight (),
																	titleBarRect.GetBottom () - 1),
														true);
		
		}	// end "if (m_frame->m_titleBar->IsShown ())"
	
}	// end "ScrollChanged"



void CMImageView::SetActiveWindowFlag (
				Boolean 								setting)

{
    GetImageFrameCPtr()->SetActiveWindowFlag (setting);

}	// end "SetActiveWindowFlag"



void CMImageView::SetControlKeyFlag (
				Boolean 								flag)

{
	m_ctlKeyDownFlag = flag;

}	// end "SetContolKeyFlag"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetImageWindowCPtr
//
//	Software purpose:	The purpose of this routine is to set the pointer
//							to the image window object that is related to
//							to this image view.
//							This came from the Windows code.
//
//	Parameters in:		Image Window class pointer
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date:	05/10/1995
//	Revised By:			Larry L. Biehl			Date: 05/10/1995

void CMImageView::SetImageWindowCPtr (
				CMImageWindow*						imageWindowCPtr)

{
	m_imageWindowCPtr = imageWindowCPtr;
	
}	// end "SetImageWindowCPtr"



void CMImageView::SetLegendBitMapInfoHeaderHandle (
				Handle								legendBitMapInfoHeaderHandle)

{
		// Not being used.
	
}	// end "SetLegendBitMapInfoHeaderHandle"



void CMImageView::SetShiftKeyFlag (
				Boolean 								flag)

{
	m_shiftKeyDownFlag = flag;

}	// end "SetShiftKeyFlag"


/*
		// Will need when copying window to clipboard is implemented.
		// Need to adapt the Windows code below to wxWidgets
void CMImageView::DoEditCopy ()

{
	CBitmap 								cBmp;

	CRect 								rect,
												// For storing the size of the image to be copied.
											destinationRect;

	CDC 									hDC;

	Rect 									sourceRect;

	LongPoint 							topleftLPoint,
											rightbottomLPoint,
											tempPoint;

	SInt16 								legendWidth;


    		// Get the size of the window

	GetClientRect (rect);

	Handle windowInfoHandle = GetWindowInfoHandle (gActiveImageWindow);
	Handle selectionInfoHandle = GetSelectionInfoHandle (windowInfoHandle);

	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);

	GetSelectedOffscreenRectangle (windowInfoPtr,
											&sourceRect,
											TRUE,
											TRUE);

	tempPoint.h = sourceRect.top;
	tempPoint.v = sourceRect.left;

	ConvertOffScreenPointToWinPoint (windowInfoHandle,
												&tempPoint,
												&topleftLPoint);

	tempPoint.h = sourceRect.bottom;
	tempPoint.v = sourceRect.right;

	ConvertOffScreenPointToWinPoint (windowInfoHandle,
												&tempPoint,
												&rightbottomLPoint);

	destinationRect.bottom = (int) rightbottomLPoint.h;
	destinationRect.top = (int) topleftLPoint.h;
	destinationRect.right = (int) rightbottomLPoint.v;
	destinationRect.left = (int) topleftLPoint.v;

	CClientDC cWndDC (this);
	hDC.CreateCompatibleDC (&cWndDC);

	destinationRect.bottom += ::GetTitleHeight (windowInfoHandle);

	legendWidth = ::GetLegendWidth (windowInfoHandle);

			// Compare the height of image and the height of the legend

	if (legendWidth > 0)
		{
		SInt16 legendBottom = GetLegendFullHeight ();
		destinationRect.bottom = MAX (destinationRect.bottom,
				 destinationRect.top + legendBottom);
		destinationRect.right += legendWidth;

		}	// end "if (legendWidth > 0)"

	cBmp.CreateCompatibleBitmap (&cWndDC,
											destinationRect.Width (),
											destinationRect.Height ());

			// Keep the old bitmap

	CBitmap* pOldBitmap = hDC.SelectObject (&cBmp);

			// Prepare the DC

	hDC.BitBlt (0,
					0,
					destinationRect.Width (),
					destinationRect.Height (),
					&cWndDC,
					0,
					0,
					WHITENESS);

    	// Copy the image to the DC

    m_pDC = &hDC;
    gCDCPointer = m_pDC;
    m_printCopyModeFlag = TRUE;

    CopyOffScreenImage (this,
								&hDC,
								m_imageWindowCPtr,
								NULL,
								&sourceRect,
								1);

    m_pDC = NULL;
    gCDCPointer = NULL;
    m_printCopyModeFlag = FALSE;

    		// Here are the actual clipboard functions.

    AfxGetApp()->m_pMainWnd->OpenClipboard ();
    EmptyClipboard ();
    SetClipboardData (CF_BITMAP, cBmp.GetSafeHandle ());
    CloseClipboard ();

			 // next we select the old bitmap back into the memory DC
			 // so that our bitmap is not deleted when cMemDC is destroyed.
			 // Then we detach the bitmap handle from the cBmp object so that
			 // the bitmap is not deleted when cBmp is destroyed.

    CheckAndUnlockHandle (windowInfoHandle);
    CheckAndUnlockHandle (selectionInfoHandle);

    hDC.SelectObject (pOldBitmap);
    cBmp.Detach ();

}	// end "DoEditCopy"
*/


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void UpdateCursorCoordinates
//
//	Software purpose:	The purpose of this routine is to update the cursor
//							coordinates in the coordinate dialog window if it is
//							showing.
//							This was adapted from the Windows code.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
//
// Called By:			DrawCursorCoordinates in SMapCoordinates.cpp
//
//	Coded By:			Larry L. Biehl			Date: 06/22/1992
//	Revised By:			Larry L. Biehl			Date: 01/11/2020

void CMImageView::UpdateCursorCoordinates (
				LongPoint*							mousePtPtr)

{
	Boolean								updateMapCoordinateFlag;


	CMImageDoc* imageDocCPtr = GetDocument ();

	if (imageDocCPtr->GetDisplayCoordinatesFlag () && ImageWindowIsAvailable ())
		{
		updateMapCoordinateFlag = GetCursorCoordinates (mousePtPtr);

		if (updateMapCoordinateFlag)
			DrawCursorCoordinates (GetWindowInfoHandle (this));

		}	// end "if (imageDocCPtr->GetDisplayCoordinatesFlag () && ...)"

}	// end "UpdateCursorCoordinates"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void UpdateCursorCoordinates
//
//	Software purpose:	The purpose of this routine is to update the cursor
//							coordinates in the coordinate dialog window if it is
//							showing.
//							This came from the Windows code.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
//
// Called By:			OnLeaveImageWindow in xImageView.cpp
//							OnMouseMove in xImageView.cpp
//
//	Coded By:			Larry L. Biehl			Date: 06/22/1992
//	Revised By:			Larry L. Biehl			Date: 01/07/2019

void CMImageView::UpdateCursorCoordinates (void)

{
	CMImageDoc* imageDocCPtr = GetDocument ();

	if (imageDocCPtr->GetDisplayCoordinatesFlag ())
		(imageDocCPtr->GetImageFrameCPtr())->UpdateCursorCoordinates ();
	
}	// end "UpdateCursorCoordinates"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void UpdateCursorCoordinatesNoDisplayOrigin
//
//	Software purpose:	The purpose of this routine is to update the cursor
//							coordinates in the coordinate dialog window if it is
//							showing.
//							This came from the Windows code.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/22/1992
//	Revised By:			Larry L. Biehl			Date: 04/02/2019

void CMImageView::UpdateCursorCoordinatesNoDisplayOrigin (
				LongPoint*							mousePtPtr,
				DisplaySpecsPtr					displaySpecsPtr)

{
	double 					savedOrigin[2];
		

	savedOrigin[kVertical] = displaySpecsPtr->origin[kVertical];
	savedOrigin[kHorizontal] = displaySpecsPtr->origin[kHorizontal];

	displaySpecsPtr->origin[kVertical] = 0;
	displaySpecsPtr->origin[kHorizontal] = 0;
	
	UpdateCursorCoordinates (mousePtPtr);
	
	displaySpecsPtr->origin[kVertical] = savedOrigin[kVertical];
	displaySpecsPtr->origin[kHorizontal] = savedOrigin[kHorizontal];

}	// end "UpdateCursorCoordinatesNoDisplayOrigin"



void CMImageView::UpdateOffscreenMapOrigin (void)

{
	wxPoint 								scrollOffset;
	 

    scrollOffset = m_Canvas->GetScrollPosition ();

    Handle displaySpecsHandle = m_displayMultiCPtr->mDisplaySpecsHandle;
    DisplaySpecsPtr displaySpecsPtr =
									(DisplaySpecsPtr) GetHandlePointer (displaySpecsHandle);

    displaySpecsPtr->origin[kVertical] =
            (double)scrollOffset.y / displaySpecsPtr->magnification;
    displaySpecsPtr->origin[kHorizontal] =
            (double)scrollOffset.x / displaySpecsPtr->magnification;
				 
}	// end "UpdateOffscreenMapOrigin"



void CMImageView::UpdateScrolls (
				double 								newMagnification)

{
	double 								halfLogicalSize,
											columnLogicalCenter,
											lineLogicalCenter,
											oldMagnification;

	Rect 									offScreenRectangle;

	wxPoint 								scrollOffset;

	SInt32 								logicalOffset;

	UInt32 								height,
											newRectHeight,
											newRectWidth,
											offscreenHeight,
											offscreenWidth,
											oldRectHeight,
											oldRectWidth,
											width;


    		// Get the current size of the client portion of the window.  I.E.
    		// without scroll bars.

	wxRect windowRect = m_Canvas->GetClientRect ();
	oldRectWidth = (UInt32)(windowRect.width);
	oldRectHeight = (UInt32)(windowRect.height);

	m_displayMultiCPtr->GetOffscreenDimensions (&offscreenHeight,
            												&offscreenWidth);

	width = (UInt32)((double)offscreenWidth * newMagnification + .5);
	height = (UInt32)((double)offscreenHeight * newMagnification + .5);

	newRectWidth = oldRectWidth;
	newRectHeight = oldRectHeight;

	oldMagnification = m_displayMultiCPtr->GetMagnification ();
    
	m_Canvas->AdjustScrollBars (true);

	Boolean selectionExistsFlag = GetSelectionOffscreenRectangle (
            													this, &offScreenRectangle);
				
	if (selectionExistsFlag)
		{
				// A selection exists and the user is zooming into the window.
				// Remember that the offscreen rectangle is 0 based. The line-
				// column selection is 1 based.

		lineLogicalCenter = newMagnification *
				 ((double)offScreenRectangle.top + offScreenRectangle.bottom + 1) / 2.;

		columnLogicalCenter = newMagnification *
				 ((double)offScreenRectangle.left + offScreenRectangle.right + 1) / 2.;

		}	// end "if (selectionExistsFlag && ...)"

	else	//	if (!selectionExistsFlag || ...
		{
				// No selection rectangle has been made or the user is zooming out.

		m_Canvas->CalcUnscrolledPosition (0, 0, &(scrollOffset.x), &(scrollOffset.y));

		if (offscreenHeight * newMagnification < oldRectHeight)
			lineLogicalCenter = (double) height / 2.;

		else	// offscreenHeight*oldMagnification >= oldRectHeight
			{
			lineLogicalCenter = (double) oldRectHeight / 2. + scrollOffset.y;
			lineLogicalCenter *= newMagnification / oldMagnification;

			}	// end "else offscreenHeight*oldMagnification >= oldRectHeight"

		if (offscreenWidth * newMagnification < oldRectWidth)
			columnLogicalCenter = (double) width / 2.;

		else	// offscreenWidth*oldMagnification >= oldRectWidth
			{
			columnLogicalCenter = (double) oldRectWidth / 2. + scrollOffset.x;
			columnLogicalCenter *= newMagnification / oldMagnification;

			}	// end "else offscreenWidth*oldMagnification >= oldRectWidth"

		}	// end "else !selectionExistsFlag || ..."

			// Get new scroll position for the x direction.

	halfLogicalSize = ((double) newRectWidth) / 2.;

	logicalOffset = (SInt32) MAX (0, columnLogicalCenter - halfLogicalSize);

	if (width < newRectWidth)
		logicalOffset = 0;

	if (logicalOffset != 0)
		logicalOffset = NormalizeScrollOffset (newMagnification, logicalOffset, 0);

	scrollOffset.x = (int) logicalOffset;

			// Get new scroll position for the y direction.

	halfLogicalSize = ((double) newRectHeight) / 2.;

	logicalOffset = (SInt32)MAX (0, (SInt32)(lineLogicalCenter - halfLogicalSize));

	if (height < newRectHeight)
		logicalOffset = 0;

	if (logicalOffset != 0)
		logicalOffset = NormalizeScrollOffset (newMagnification, logicalOffset, 0);

	scrollOffset.y = (int) logicalOffset;

			// Set new scroll postions.

	if (scrollOffset.x >= 0 || scrollOffset.y >= 0)
		{
		m_Canvas->CanvasScroll ((int)(scrollOffset.x), (int)(scrollOffset.y));
              
		}
    
			// Set new magnification

	m_displayMultiCPtr->SetMagnification (newMagnification);

	ScrollChanged ();
	 
	m_mainFrame->UpdateStatusBar (m_frame, newMagnification);
    
}	// end "UpdateScrolls"



void CMImageView::UpdateSelectionCoordinates (void)

{
	CMImageDoc* imageDocCPtr = GetDocument ();

	if (imageDocCPtr->GetDisplayCoordinatesFlag ())
		(imageDocCPtr->GetImageFrameCPtr())->UpdateSelectionCoordinates ();

}	// end "UpdateSelectionCoordinates"



void CMImageView::ZoomIn (void)

{
	double								magnification,
											maxMagnification,
											inverse,
											step;
	

	if (m_mainFrame->GetZoomCode () == 0)
																							return;
	
	magnification = m_displayMultiCPtr->GetMagnification ();

	maxMagnification = m_displayMultiCPtr->GetMaxMagnification ();

	if (magnification < maxMagnification)
		{
		inverse = 1. / magnification;

		step = 0.1;
		#if defined multispec_wxlin
			if (!wxGetKeyState (WXK_ALT))
		#endif
		#if defined multispec_wxmac
			if (!wxGetKeyState (WXK_ALT))
		#endif
			{
			step = 1.0;

			if (magnification > 1)
				 magnification = (double)((UInt16)(magnification + .5));

			if (inverse > 1)
				 inverse = (double)((UInt16)(inverse + .5));

			}	// end "if (!m_altKeyDownFlag)"

		if (magnification >= 1.0)
			magnification += step;

		else	// magnification < 1.0
			magnification = 1. / (inverse - step);

				// Set new magnification
		
		magnification = MIN (magnification, maxMagnification);
		m_Scale = magnification;
		UpdateScrolls (magnification);
		
		OnUpdate (this, NULL);
		UpdateScaleInformation (GetWindowInfoHandle (this));
	  
		}	// end "if (magnification < maxMagnification)"
		
	else	// At max zoom value
		m_mainFrame->SetZoomCode (0);

}	// end "ZoomIn"



void CMImageView::ZoomOut (void)

{
	double 								magnification,
											inverse,
											step;


	if (m_mainFrame->GetZoomCode () == 0)
																							return;

	magnification = m_displayMultiCPtr->GetMagnification ();

	if (magnification > gMinMagnification)
		{
		inverse = 1. / magnification;

		step = 1.0;
		#if defined multispec_wxlin
			if (wxGetKeyState (WXK_ALT))
		#endif
		#if defined multispec_wxmac
			if (wxGetKeyState (WXK_ALT))
		#endif
			step = 0.1;

		if (step > 0.5)
			{
			if (magnification > 1)
				 magnification = (double)((UInt16)(magnification + .5));

			if (inverse > 1)
				 inverse = (double)((UInt16)(inverse + .5));

			}	// end "if (step > 0.5)"

		if (magnification > 1.)
			magnification -= step;

		else	// magnification <= 1.
			magnification = 1. / (inverse + step);
		
				// Set new magnification

		magnification = MAX (magnification, gMinMagnification);

		m_Scale = magnification;
		UpdateScrolls (magnification);
		OnUpdate (this, NULL);
		
		UpdateScaleInformation (GetWindowInfoHandle (this));

		}	// end "if (magnification < gMaxMagnification)"
		
	else	// At min zoom value
		m_mainFrame->SetZoomCode (0);

}	// end "ZoomOut"



void CMImageView::ZoomSize (void)

{
	m_Scale = 1.0;
	UpdateScrolls (m_Scale);
	
			// Set new magnification
	
	OnUpdate (this, NULL);
	UpdateScaleInformation (GetWindowInfoHandle (this));

}	// end "ZoomSize"
