// Revised 01/09/2019  by Larry L Biehl
// Revised 03/12/2016 by Wei-Kang Hsu
// Revised 12/19/2018 by Tsung Tai Yeh

#include "SMultiSpec.h"

#include "CDisplay.h"
#include "CImageWindow.h"

#include "LImageCanvas.h"
#include "LImageDoc.h"
#include "LImageFrame.h"
#include "LImageView.h"
#include "LMainFrame.h"
#include "LMultiSpec.h"
#include "LTools.h"

extern void		DoNextDisplayChannelEvent (
						WindowPtr		window,
						char			theChar);


BEGIN_EVENT_TABLE (CMImageView, wxView)
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
	m_withinOnSizeFlag = FALSE;
	m_callUpdateScrollRangeFlag = TRUE;	
	
			// The following two flags are currently used for selections.
	m_ctlKeyDownFlag = FALSE;
	m_shiftKeyDownFlag = FALSE;

	m_thumbScrollFlag = FALSE; 
	m_printCopyModeFlag = FALSE;
		
	m_cursorColumnValue = -1;
	m_cursorLineValue = -1;
	
	m_printerTextScaling = 1.; 
	
	//m_xPixelsPerInch = 72;
	//m_yPixelsPerInch = 72;
	wxScreenDC dc;
	m_xPixelsPerInch = (SInt16)(dc.GetPPI()).GetWidth();
	m_yPixelsPerInch = (SInt16)(dc.GetPPI()).GetHeight();

	m_initializedFlag = FALSE;
	m_startchannel_sbs = 0;
   m_Scale = 1.0;
   m_hasWaveLength = 0;
	try
		{
				// Create Image Window Object for this image.

		Handle fileInfoHandle = ((CMultiSpecApp*)wxTheApp)->GetOpenImageFileInfoHandle();

		m_imageWindowCPtr = new CMImageWindow(fileInfoHandle);

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
			UpdateWindowList(this, kImageWindowType);

		}	// end "try"

    catch(int e) 
		{
		MemoryMessage(0, kObjectMessage);
		}
	
}	// end "CMImageView"



CMImageView::~CMImageView()
{  
	if (m_displayMultiCPtr != NULL)
		delete m_displayMultiCPtr;
	
	if (m_histogramCPtr != NULL)
		delete m_histogramCPtr;
		
	if (this == gProjectSelectionWindow)
		{
		gProjectSelectionWindow = NULL;
		
		ClearNewFieldList();
		
		}		// end "if (this == gProjectSelectionWindow)"
	
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
				
		}		// end "if (gActiveImageViewCPtr == this)"
		
	//CMImageFrame* imageFrameCPtr = m_frame;
	m_frame->SetImageViewCPtr (NULL);
		
	RemoveWindowFromList (this, 0);
		
}	// end "~CMImageView"



//-----------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DisposeImageWindowSupportMemory
//
//	Software purpose:	The purpose of this routine is to dispose of the memory
//							which supports the image window. This includes the memory
//							for the histogram and display specification handles and
//							the supporting offscreen window.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:						
//
//	Coded By:			Larry L. Biehl			Date: 12/24/1991
//	Revised By:			Larry L. Biehl			Date: 11/13/1995

void CMImageView::DisposeImageWindowSupportMemory (void)

{  
	if (m_histogramCPtr != NULL)                               
		m_histogramCPtr->ReleaseHistogramSupportMemory ();
	                       
	if (m_displayMultiCPtr != NULL) 
		m_displayMultiCPtr->ReleaseDisplaySupportMemory ();
	
	m_imageWindowCPtr->ReleaseOffscreenSupportMemory ();
	
}		// end "DisposeImageWindowSupportMemory" 



//-----------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CheckIfOffscreenImageExists
//
//	Software purpose:	The purpose of this routine is to determine if a offscreen
//							image exists.
//
//	Parameters in:		None			
//
//	Parameters out:	None			
//
//	Value Returned:	TRUE if offscreen image exists
//							FALSE if offscreen image does not exist
// 
// Called By:						
//
//	Coded By:			Larry L. Biehl			Date: 08/01/95
//	Revised By:			Larry L. Biehl			Date: 08/01/95	

Boolean
CMImageView::CheckIfOffscreenImageExists(void)

{  
	Boolean		returnFlag = FALSE;
	
	         
			// Verify that an offscreen images exists.
	
	if (this != NULL && m_imageWindowCPtr != NULL)
		{																								
		WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer(
							m_imageWindowCPtr->GetWindowInfoHandle ());
	
		if (windowInfoPtr != NULL)                                                   
			returnFlag = (windowInfoPtr->imageBaseAddressH != NULL);
			
		}		// end "if (this != NULL && m_imageWindowCPtr != NULL)"

	return (returnFlag);
	
}		// end "CheckIfOffscreenImageExists"


//-----------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetClassGroupCode
//
//	Software purpose:	The purpose of this routine is to return the class/group
//							code for this window object.
//
//	Parameters in:		None			
//
//	Parameters out:	None			
//
//	Value Returned:	class/group code.
// 
// Called By:						
//
//	Coded By:			Larry L. Biehl			Date: 04/03/1996
//	Revised By:			Larry L. Biehl			Date: 04/04/1996

SInt16 CMImageView::GetClassGroupCode(void)

{  
	SInt16		classGroupCode = 0;
	
	         
			// Get the class/group code.
	                              
	if (this != NULL)
		{
		if (m_displayMultiCPtr != NULL)
			{                                                                    
			DisplaySpecsPtr displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
													m_displayMultiCPtr->mDisplaySpecsHandle);
													
			if (displaySpecsPtr != NULL && !displaySpecsPtr->initializeStructureFlag) 
				classGroupCode = displaySpecsPtr->classGroupCode;
				
			}	// end "if (m_displayMultiCPtr != NULL)"
			
		}	// end "if (this != NULL)"

	return (classGroupCode);
	
}	// end "GetClassGroupCode"



//-----------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetImageType
//
//	Software purpose:	The purpose of this routine is to return the image type
//							variable for this window object.
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

SInt16
CMImageView::GetImageType(void)

{  
	SInt16		imageType = 0;
	
	         
			// Get the window type. 
	                              
	if (this != NULL)
		{ 
		if (m_imageWindowCPtr != NULL) 
			imageType = ::GetImageType ( m_imageWindowCPtr->GetWindowInfoHandle() );
				
		}		// end "if (this != NULL)"
	
	return (imageType);
	
}		// end "GetImageType"  


//TODO: For Linux
SInt16		
CMImageView::GetLegendFullHeight(void)

{	   
#ifndef multispec_lin
	CRect				legendRect,
						listRect;
	
	Handle 			windowInfoHandle;
	
	SInt16			legendFullHeight;
	

	windowInfoHandle = GetWindowInfoHandle(this);	
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer(
											windowInfoHandle, kNoLock, kNoMoveHi);
	legendFullHeight = GetListBottom(windowInfoPtr->legendListHandle);
	CheckAndUnlockHandle (windowInfoHandle);
	
	if (windowInfoPtr->legendListHandle != NULL)
		{                                
		GetWindowRect (legendRect); 
		(GetImageLegendListCPtr())->GetWindowRect (listRect);
		
		legendFullHeight += (SInt16)(listRect.top - legendRect.top);
		
		}		// end "if (windowInfoPtr->legendListHandle != NULL)"
	
	return (legendFullHeight); 
#else
    return -1;
#endif
}		// end "GetLegendFullHeight" 



SInt16		
CMImageView::GetLegendWidth(void)

{	
	Handle windowInfoHandle = GetWindowInfoHandle (this);
	
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer(
												windowInfoHandle,
												kNoLock,
												kNoMoveHi);
											
	if (windowInfoPtr != NULL)
		return (windowInfoPtr->legendWidth);
		
	else		// windowInfoPtr != NULL   
		return (NULL);
												   
//	return (m_legendWidth); 
	
}		// end "GetLegendWidth" 



//-----------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 numberGroups
//
//	Software purpose:	The purpose of this routine is to return the number of 
//							information groups for this window object.
//
//	Parameters in:		None			
//
//	Parameters out:	None			
//
//	Value Returned:	Number of information groups.
// 
// Called By:						
//
//	Coded By:			Larry L. Biehl			Date: 04/03/96
//	Revised By:			Larry L. Biehl			Date: 04/04/96	

SInt16 CMImageView::GetNumberGroups(void)

{  
	SInt16		numberGroups = 0;
	
	         
			// Get the number of groups.
	                              
	if (this != NULL)
		{ 
		if (m_imageWindowCPtr != NULL)
			{                                                                                                       
//			FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer(
//							m_imageWindowCPtr->GetFileInfoHandle(), kNoLock, kNoMoveHi);
							
			Handle windowInfoHandle = m_imageWindowCPtr->GetWindowInfoHandle ();
			Handle fileInfoHandle = GetFileInfoHandle (windowInfoHandle);    
			                                          
			FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer(
															fileInfoHandle, kNoLock, kNoMoveHi);
													
			if (fileInfoPtr != NULL) 
				numberGroups = fileInfoPtr->numberGroups;
				
			}		// end "if (m_imageWindowCPtr != NULL)"
				
		}		// end "if (this != NULL)"

	return (numberGroups);
	
}		// end "GetNumberGroups" 



//-----------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetTitleHeight
//
//	Software purpose:	This routine returns height for the input
//							window information handle.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/30/96
//	Revised By:			Larry L. Biehl			Date: 04/30/96			

SInt16 
CMImageView::GetTitleHeight (void)

{ 
	SInt16 			titleHeight;
	
	
	Handle windowInfoHandle = GetWindowInfoHandle (this);
	titleHeight = ::GetTitleHeight (windowInfoHandle);
	     
	return (titleHeight);
	
}		// end "GetTitleHeight"  



//-----------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetWindowType
//
//	Software purpose:	The purpose of this routine is to return the window type
//							variable for this window object.
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
//	Coded By:			Larry L. Biehl			Date: 04/03/96
//	Revised By:			Larry L. Biehl			Date: 04/04/96	

SInt16
CMImageView::GetWindowType(void)

{  
	SInt16		windowType = 0;
	
	         
			// Get the window type. 
	                              
	if (this != NULL)
		{ 
		if (m_imageWindowCPtr != NULL) 
			windowType = ::GetWindowType ( m_imageWindowCPtr->GetWindowInfoHandle() );
				
		}		// end "if (this != NULL)"
	
	return (windowType);
	
}		// end "GetWindowType"  



//-----------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetImageWindowCPtr
//
//	Software purpose:	The purpose of this routine is to set the pointer
//							to the image window object that is related to
//							to this image view.
//
//	Parameters in:		Image Window class pointer	
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl					Date:	05/10/95
//	Revised By:			Larry L. Biehl					Date: 05/10/95

void 
CMImageView::SetImageWindowCPtr (
				CMImageWindow				*imageWindowCPtr)

{
	m_imageWindowCPtr = imageWindowCPtr;
	
}		// end "SetImageWindowCPtr" 



void 
CMImageView::SetLegendBitMapInfoHeaderHandle(
				Handle			legendBitMapInfoHeaderHandle)

{  
	
}		// end "SetBitMapInfoHeader"  


//CMImageFrame* CMImageView::GetImageFrameCPtr(void);
/*
CMImageView::CMImageView()
{
    m_Scale = 1.0;
}
*/


void CMImageView::ClearView(bool flag)
{
	m_Canvas->EraseBackground(flag);
}

void CMImageView::ClientToDoc(wxPoint& point) {
    wxClientDC dc(m_Canvas);
    m_Canvas->DoPrepareDC(dc);
    point.x = dc.DeviceToLogicalX(point.x);
    point.y = dc.DeviceToLogicalY(point.y);

} // end "ClientToDoc"


void CMImageView::CreateScaledBitmap()
{
    // For now, GetOffScreenImage routine in CUtility.cpp
    // is directly setting up m_ScaledBitmap
//	int imwidth,imheight;
//	CMImageDoc * document = wxDynamicCast(GetDocument(), CMImageDoc);
//	if(document)
//	{
//		m_ScaledBitmap = wxBitmap(document->GetImage());
//	}
//	else
//	{
		m_ScaledBitmap = wxNullBitmap;
//	}
}


/*
 * TODO: For Linux
void
CMImageView::DoFilePrint()
 {
    gProcessorCode = kPrintProcessor;
    CScrollView::OnFilePrint();
    gProcessorCode = 0;

} // end "DoFilePrint"

void
CMImageView::DoFilePrintPreview()
 {
    gProcessorCode = kPrintProcessor;
    CScrollView::OnFilePrintPreview();
    gProcessorCode = 0;

} // end "DoFilePrintPreview"
*/


void CMImageView::DrawLegend() 
{
	CMLegendView* legendViewCPtr =
			GetDocument()->GetImageFrameCPtr()->GetLegendViewCPtr();
	legendViewCPtr->UpdateThematicLegendControls();
	CMLegendList* legendListPtr = (CMLegendList*) GetActiveLegendListHandle();
	if (legendListPtr != NULL) 
		{
		/*		
		if (m_printCopyModeFlag)
			{
			CPoint		windowOrigin;
			CRect			windowRect;

			windowOrigin = m_pDC->GetWindowOrg ();
			GetWindowRect (windowRect);

			if (!gMFC_Rgn.CreateRectRgn(
										  0,
										  0,
										  580,
										  windowRect.bottom) )
																							return;

			gMFC_Rgn.SetRectRgn(
										  0,
										  0,
										  580,
										  windowRect.bottom);

			m_pDC->SelectClipRgn(&gMFC_Rgn);

			gMFC_Rgn.DeleteObject();

			}		// end "if (m_printCopyModeFlag)"
		*/
		legendListPtr->DrawLegendList();

		} // end "if (legendListPtr != NULL)"
	/*
	CMComboBox* listBoxPtr = &(legendViewCPtr->m_legendTitleCombo);
	if (listBoxPtr != NULL)
		listBoxPtr->DrawLegendTitle();
	*/

}	// end "DrawLegend"


Boolean CMImageView::GetActiveWindowFlag(void)
{
    return ( GetImageFrameCPtr()->GetActiveWindowFlag());

} // end "GetActiveWindowFlag"


Boolean CMImageView::GetControlKeyFlag(void) {
    if (this != NULL)
        return (m_ctlKeyDownFlag);

    else
        return (FALSE);

} // end "GetContolKeyFlag"


UInt16 CMImageView::GetDisplayPixelSize(void)
 {
    return ( GetImageFrameCPtr()->GetDisplayPixelSize());

} // end "GetDisplayPixelSize"


Handle CMImageView::GetDisplaySpecsHandle(void)
 {
    Handle displaySpecsHandle = NULL;

    if (this != NULL)
        displaySpecsHandle = m_displayMultiCPtr->mDisplaySpecsHandle;

    return (displaySpecsHandle);

} // end "GetDisplaySpecsHandle"


CMImageFrame* CMImageView::GetImageFrameCPtr(void)
{
	CMImageFrame* imageFrameCPtr = NULL;

	if (this != NULL) {
		//imageFrameCPtr = (CMImageFrame*)GetFrame();
		imageFrameCPtr = m_frame;
		} // end "if (this != NULL)"

	return (imageFrameCPtr);

} // end "GetImageFrameCPtr"


CMLegendList* CMImageView::GetImageLegendListCPtr(void)
{
	CMLegendList* legendListCPtr = NULL;

	CMLegendView* legendViewCPtr = m_frame->GetLegendViewCPtr();

	if (legendViewCPtr != NULL)
		legendListCPtr = legendViewCPtr->GetLegendListCPtr();

    return (legendListCPtr);

} // end "GetImageLegendListCPtr"


CMLegendView* CMImageView::GetImageLegendViewCPtr(void)
{
    return (m_frame->GetLegendViewCPtr());

} // end "GetImageLegendViewCPtr"


wxBitmap CMImageView::GetScaledBitmap()
//wxBitmap* CMImageView::GetScaledBitmapPtr()
{
	return m_ScaledBitmap;
}


#ifndef multispec_lin
CMOutlineArea* CMImageView::GetSelectionAreaCPtr(void) {
    if (this != NULL)
        return ( GetDocument()->GetSelectionAreaCPtr());

    else // this == NULL
        return ( NULL);

} // end "GetSelectionAreaCPtr"
#endif


Boolean CMImageView::GetShiftKeyFlag(void) {
    if (this != NULL)
        return (m_shiftKeyDownFlag);

    else
        return (FALSE);

} // end "GetShiftKeyFlag"


const wxSize & CMImageView::GetViewOffset()
{
	return m_ViewOffset;
}


void CMImageView::InitialUpdate(void)
{
	SInt16				windowType;
	/*
   lineIncrement.cx = 10;
   lineIncrement.cy = 10;

   pageIncrement.cx = 20;
   pageIncrement.cy = 20;

	SetScrollSizes( MM_TEXT,
							GetDocument()->GetDocSize(),
							pageIncrement,
							lineIncrement);
	ResizeParentToFit(FALSE);
	*/
	windowType = GetWindowType ();

	if (windowType == kThematicWindowType)
		{
		m_frame->InitialUpdate (GetDocument()->GetDocSize());

		}		// end "if (windowType == kThematicWindowType)"

//	else		// windowType != kThematicType)
//		ResizeParentToFit(FALSE);

	if (m_imageWindowCPtr != NULL)
		{
				// Allow ability to go from image window class back to image view class.

		m_imageWindowCPtr->mImageViewCPtr = this;
		
		gActiveImageViewCPtr = this;
		gActiveImageWindow = this;
		gActiveWindowType = kImageWindowType;
		gTheActiveWindow = (WindowPtr)this;

		gActiveImageWindowInfoH = GetActiveImageWindowInfoHandle();
		gActiveImageFileInfoH = GetActiveImageFileInfoHandle();
		gActiveImageLayerInfoH = GetActiveImageLayerInfoHandle();

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

			//CMImageFrame* imageFrameCPtr = GetDocument()->GetImageFrameCPtr();
			CMImageFrame* imageFrameCPtr = m_frame;
			imageFrameCPtr->SetImageViewCPtr (this);

			}	// end "else windowType != kThematicWindowType"

		//gMemoryTypeNeeded = 1;
		//DisplayImage();
		//gMemoryTypeNeeded = 0;

   	}	// end "if (m_imageWindowCPtr != NULL)"
		
	m_frame->m_coordinatesBar->InitialUpdate ();
	
			// Get coordinate bar height
			//		Note: do not set the size of the coordinate bar in the window
			// 	information structure. The coordinate bar is not a part of the image
			//		window in wx version. It is a separate panel within the image frame.
			//		Doing so will cause problems later when drawing selections and
			//		fields.
	
	//wxRect coordinateRect = m_frame->m_coordinatesBar->GetRect ();
	//SetCoordinateHeight (gActiveImageWindowInfoH, coordinateRect.height);

   ShapeInfoPtr shapeInfoPtr = NULL;
	if (gActiveImageWindow != NULL && gSelectionGraphViewCPtr!= NULL)
		{         
      shapeInfoPtr = gSelectionGraphViewCPtr->GetShapeInfoFromHandle(gShapeFilesHandle, gActiveImageWindowInfoH);
      gSelectionGraphViewCPtr->UpdateDataListCtrl ();
      gSelectionGraphViewCPtr->UpdateShowOrHideFeatureList();
		//if(shapeInfoPtr!= NULL && shapeInfoPtr->shapeType == 5)
		//	{         
		//	gSelectionGraphViewCPtr->ShowFeatureList();
		//	}
		//else
		//	gSelectionGraphViewCPtr->HideFeatureList();      
		}	
   
}	// end "InitialUpdate"


void CMImageView::InvalidateRect(Rect* rectp, bool erase)
{
    Rect rect = *rectp;
    
    m_Canvas->RefreshRect(wxRect(rect.left, rect.top, (rect.right - rect.left), (rect.bottom - rect.top)),erase);

}


void CMImageView::InvalidateRect(wxRect* rectp, bool erase)
{
    wxRect rect = *rectp;

    m_Canvas->RefreshRect(rect,erase);

}


SInt32 CMImageView::NormalizeScrollOffset(
        double magnification,
        SInt32 scrollIncrement,
        SInt32 scrollOffset)
 {
    SInt32 normalizedIncrement;


    normalizedIncrement =
            (SInt32) (((double) scrollOffset + scrollIncrement) / magnification + .5);
    normalizedIncrement = MAX(0, normalizedIncrement);

    normalizedIncrement = (SInt32) ((double) normalizedIncrement * magnification - scrollOffset);

    return (normalizedIncrement);

} // end "NormalizeScrollOffset"


// This function is called when we call wxView::Activate()
// Inherited from wxView
void CMImageView::OnActivateView(
        bool bActivate,
        wxView* pActivateView,
        wxView* pDeactiveView)
{
	if (m_frame != NULL)
		{
		m_activeFlag = (Boolean)bActivate;
		if (bActivate)
			wxASSERT(pActivateView == (wxView*)this);
		  
				// If the window is being activated, outside of a processing operating, make sure the global
				// active image information is up to date.
		
		if (gProcessorCode == 0 && bActivate)
			m_frame->UpdateActiveImageWindowInfo ();
			
		Boolean changeWindowFlag = TRUE;
			
		m_frame->ActivateImageWindowItems (bActivate, changeWindowFlag);
		
		}	// end "if (m_frame != NULL)"

}	// end "OnActivateView"



bool CMImageView::OnClose(bool deleteWindow)

{
	if (!GetDocument()->Close())
		{
 		return false;
		}

	if (m_Canvas)
		{
		m_Canvas->ClearBackground();
		m_Canvas->SetView(NULL);
		m_Canvas = NULL;
		}

	//wxFrame * frame = wxDynamicCast(GetFrame(), wxFrame);
	if (m_frame)
		{
		m_frame->SetTitle(CMMainFrameTitle);
		
		}

	Activate(false);
	SetFrame(NULL);
	return true;
	
}	// end "OnClose"



bool CMImageView::OnCreate (
				wxDocument *doc, long WXUNUSED(flags))
{
	m_Canvas = NULL;
	SetDocument(doc);
	m_mainFrame = wxDynamicCast (wxTheApp->GetTopWindow(), CMainFrame);

	if (m_mainFrame)
		{
		m_frame = wxDynamicCast (wxGetApp().CreateChildFrame (GetDocument(), this), CMImageFrame);
		CreateScaledBitmap ();
		m_Canvas = new CMImageCanvas (m_frame->m_mainWindow);
		m_frame->m_coordinatesBar = new CMCoordinateBar (m_frame->m_topWindow);
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
		}
	else
		{
		wxFAIL_MSG(_("Can't retrieve a pointer to main window"));
		}
		
	m_imageWindowCPtr->mImageViewCPtr = this;
	((CMImageDoc*)doc)->SetImageWindowCPtrV (this);

	InitialUpdate();
	return true;
	
}	// end "OnCreate"



void CMImageView::OnDraw (CDC* pDC)

{

	Boolean continueFlag = TRUE;
	Rect sourceRect;
	SInt16 copyType;

	if (CheckIfOffscreenImageExists())
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

		//wxRect sourcewxrect(sourceRect.left,sourceRect.top,sourceRect.right-sourceRect.left,sourceRect.bottom-sourceRect.top);
				// Get area of window that is to be drawn.
				// Assume for now the entire client window.

		// Take any scroll offset into account

		//wxPoint scrollOffset;
		//m_Canvas->CalcUnscrolledPosition(0,0,&scrollOffset.x,&scrollOffset.y);

		//sourceRect.top += scrollOffset.y;
		//sourceRect.bottom += scrollOffset.y;
		//sourceRect.left += scrollOffset.x;
		//sourceRect.right += scrollOffset.x;

		//SInt16 cxHorizontal = pDC->GetDeviceCaps(HORZSIZE);
		//SInt16 cxHorzRes = pDC->GetDeviceCaps(HORZRES);

		//m_xPixelsPerInch = (SInt16) ((double) cxHorzRes / cxHorizontal * 25.4);
		m_xPixelsPerInch = (SInt16)(pDC->GetPPI()).GetWidth();
		//SInt16 cyHorizontal = pDC->GetDeviceCaps(VERTSIZE);
		//SInt16 cyHorzRes = pDC->GetDeviceCaps(VERTRES);

		//m_yPixelsPerInch = (SInt16) ((double) cyHorzRes / cyHorizontal * 25.4);
		m_yPixelsPerInch = (SInt16)(pDC->GetPPI()).GetHeight();
		// Verify that rect is in visible region.
		/**************Temporary******************
		* wxRect* clippingregion;
		pDC->GetClippingBox(*clippingregion);
		//continueFlag = pDC->RectVisible((RECT*) & sourceRect);
		continueFlag = clippingregion->Intersects(sourcewxrect);*/
		copyType = 4;

      wxPoint scrollOffset;
		m_Canvas->CalcUnscrolledPosition (0, 0, &scrollOffset.x, &scrollOffset.y);
      //Handle displaySpecsHandle = m_displayMultiCPtr->mDisplaySpecsHandle;
      //DisplaySpecsPtr displaySpecsPtr =
      //      (DisplaySpecsPtr) GetHandlePointer (displaySpecsHandle);
            
      Handle windowInfoHandle = GetWindowInfoHandle (this);
      WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer(
												windowInfoHandle, kLock, kNoMoveHi);
		
      GetSelectedOffscreenRectangle (windowInfoPtr,
												&sourceRect,
												TRUE,
												TRUE);
//			sourceRect.top += scrollOffset.y;		  
//			sourceRect.bottom += scrollOffset.y;
//			sourceRect.left += scrollOffset.x;	
//			sourceRect.right += scrollOffset.x;
      
		// } // end "else not printer DC"

		if (continueFlag)
			{
			gCDCPointer = pDC;
			m_pDC = pDC;
         
//         printf("sourceRect top = %d \n", sourceRect.top);
//         printf (" OnDraw: top, bottom, left, right: %ld, %ld, %ld, %ld \n",
//												sourceRect.top, 
//												sourceRect.bottom,
//												sourceRect.left, 
//												sourceRect.right);         
            
         wxPoint deviceOriginSaved  = gCDCPointer->GetDeviceOrigin();
         wxPoint deviceOrigin =  deviceOriginSaved;
//         printf("device origin = (%d, %d)\n", deviceOrigin.x, deviceOrigin.y);
         if(scrollOffset.x < 0)  deviceOrigin.x += scrollOffset.x;
         if(scrollOffset.y < 0)  deviceOrigin.y += scrollOffset.y;

         
         gCDCPointer->SetDeviceOrigin(deviceOrigin.x, deviceOrigin.y);             
			CopyOffScreenImage(this, pDC, m_imageWindowCPtr, NULL, &sourceRect, copyType);
         
         gCDCPointer->SetDeviceOrigin(deviceOriginSaved.x, deviceOriginSaved.y);
         
			gCDCPointer = NULL;
			m_pDC = NULL;

			} // end "if (continueFlag)"
		
		}	// end "if ( CheckIfOffscreenImageExists() )"

}	// end "OnDraw"



void CMImageView::OnFocus (
				wxFocusEvent&									event)

{
	event.Skip (true);
	
}	// end "OnFocus"



void CMImageView::OnUpdate(wxView *sender, wxObject *hint)

{
    wxView::OnUpdate(sender, hint); 
    	 
    //m_Scale = m_frame->m_zoom;
    m_Canvas->Refresh();
				 
}	// end "OnUpdate"



void CMImageView::SetActiveWindowFlag (Boolean setting)
{
    GetImageFrameCPtr()->SetActiveWindowFlag(setting);

} // end "SetActiveWindowFlag"



void CMImageView::SetViewOffset(const wxSize & value)
{
	m_ViewOffset = value;
	
}


void CMImageView::ScrollChanged()

{
	UpdateOffscreenMapOrigin();
	
}	// end "ScrollChanged"



void CMImageView::SetControlKeyFlag( Boolean flag)

{
	if (this != NULL)
		m_ctlKeyDownFlag = flag;

}	// end "SetContolKeyFlag"



void CMImageView::SetShiftKeyFlag( Boolean flag)

{
    if (this != NULL)
        m_shiftKeyDownFlag = flag;

}	// end "SetContolKeyFlag"


/**
 * TODO: For Linux
void CMImageView::DoEditCopy() {
    CBitmap cBmp;

    CRect rect,
            destinationRect; // For storing the size of the image to be copied.

    CDC hDC;

    LongPoint topleftLPoint,
            rightbottomLPoint;

    LongPoint tempPoint;

    Rect sourceRect;

    SInt16 legendWidth;


    // Get the size of the window

    GetClientRect(rect);

    Handle windowInfoHandle = GetWindowInfoHandle(gActiveImageWindow);
    Handle selectionInfoHandle = GetSelectionInfoHandle(windowInfoHandle);

    WindowInfoPtr windowInfoPtr = (WindowInfoPtr) GetHandlePointer(
            windowInfoHandle, kLock, kNoMoveHi);

    GetSelectedOffscreenRectangle(windowInfoPtr,
            &sourceRect,
            TRUE,
            TRUE);

    tempPoint.h = sourceRect.top;
    tempPoint.v = sourceRect.left;

    ConvertOffScreenPointToWinPoint(windowInfoHandle,
            &tempPoint,
            &topleftLPoint);

    tempPoint.h = sourceRect.bottom;
    tempPoint.v = sourceRect.right;

    ConvertOffScreenPointToWinPoint(windowInfoHandle,
            &tempPoint,
            &rightbottomLPoint);

    destinationRect.bottom = (int) rightbottomLPoint.h;
    destinationRect.top = (int) topleftLPoint.h;
    destinationRect.right = (int) rightbottomLPoint.v;
    destinationRect.left = (int) topleftLPoint.v;

    CClientDC cWndDC(this); // View is an hWnd, so we can use "this"
    hDC.CreateCompatibleDC(&cWndDC); // Create the memory DC.

    destinationRect.bottom += ::GetTitleHeight(windowInfoHandle);

    legendWidth = ::GetLegendWidth(windowInfoHandle);

    //compare the height of image and the height of the legend

    if (legendWidth > 0) {
        SInt16 legendBottom = GetLegendFullHeight();
        destinationRect.bottom = MAX(destinationRect.bottom,
                destinationRect.top + legendBottom);
        destinationRect.right += legendWidth;

    } // end "if (legendWidth > 0)"

    cBmp.CreateCompatibleBitmap(&cWndDC,
            destinationRect.Width(),
            destinationRect.Height());

    // Keep the old bitmap

    CBitmap* pOldBitmap = hDC.SelectObject(&cBmp);

    //prepare the DC

    hDC.BitBlt(0,
            0,
            destinationRect.Width(),
            destinationRect.Height(),
            &cWndDC,
            0,
            0,
            WHITENESS);

    //copy the image to the DC

    m_pDC = &hDC;
    gCDCPointer = m_pDC;
    m_printCopyModeFlag = TRUE;

    CopyOffScreenImage(this,
            &hDC,
            m_imageWindowCPtr,
            NULL,
            &sourceRect,
            1);

    m_pDC = NULL;
    gCDCPointer = NULL;
    m_printCopyModeFlag = FALSE;

    // here are the actual clipboard functions.

    AfxGetApp()->m_pMainWnd->OpenClipboard();
    EmptyClipboard();
    SetClipboardData(CF_BITMAP, cBmp.GetSafeHandle());
    CloseClipboard();

    // next we select the old bitmap back into the memory DC
    // so that our bitmap is not deleted when cMemDC is destroyed.
    // Then we detach the bitmap handle from the cBmp object so that
    // the bitmap is not deleted when cBmp is destroyed.

    CheckAndUnlockHandle(windowInfoHandle);
    CheckAndUnlockHandle(selectionInfoHandle);

    hDC.SelectObject(pOldBitmap);
    cBmp.Detach();

} // end "DoEditCopy"
*/



//-----------------------------------------------------------------------------
//					 Copyright (1988-2019)
//           (c) Purdue Research Foundation
//					All rights reserved.
//
//	Function name:		void UpdateCursorCoordinates
//
//	Software purpose:	The purpose of this routine is to update the cursor
//				coordinates in the coordinate dialog window if it is
//				showing.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
//
// Called By:			FixCursor in multiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 06/22/1992
//	Revised By:			Larry L. Biehl			Date: 12/08/2000

void CMImageView::UpdateCursorCoordinates (
				LongPoint*							mousePtPtr)

{
	Boolean								updateMapCoordinateFlag;


	CMImageDoc* imageDocCPtr = GetDocument ();

	if (imageDocCPtr->GetDisplayCoordinatesFlag () &&
															CheckIfOffscreenImageExists ())
		{
		updateMapCoordinateFlag = GetCursorCoordinates (mousePtPtr);

		if (updateMapCoordinateFlag)
			//DrawCursorCoordinates (gActiveImageWindowInfoH);
			DrawCursorCoordinates (GetWindowInfoHandle (this));

		}	// end "if ( imageDocCPtr->GetDisplayCoordinatesFlag () && ...)"

}	// end "UpdateCursorCoordinates"



//-----------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateCursorCoordinates
//
//	Software purpose:	The purpose of this routine is to update the cursor
//				coordinates in the coordinate dialog window if it is
//				showing.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
//
// Called By:			FixCursor in multiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 06/22/1992
//	Revised By:			Larry L. Biehl			Date: 01/07/2019

void CMImageView::UpdateCursorCoordinates (void)

{
	CMImageDoc* imageDocCPtr = GetDocument();

	if (imageDocCPtr->GetDisplayCoordinatesFlag())
		(imageDocCPtr->GetImageFrameCPtr())->UpdateCursorCoordinates();
 
}	// end "UpdateCursorCoordinates"



void CMImageView::UpdateOffscreenMapOrigin (void)

{
    wxPoint scrollOffset;
	 

    scrollOffset = m_Canvas->GetScrollPosition();

    Handle displaySpecsHandle = m_displayMultiCPtr->mDisplaySpecsHandle;
    DisplaySpecsPtr displaySpecsPtr =
            						(DisplaySpecsPtr) GetHandlePointer (displaySpecsHandle);

    displaySpecsPtr->origin[kVertical] =
            //(SInt16)((double)scrollOffset.y / displaySpecsPtr->magnification);
            (double)scrollOffset.y / displaySpecsPtr->magnification;
    displaySpecsPtr->origin[kHorizontal] =
            //(SInt16)((double)scrollOffset.x / displaySpecsPtr->magnification);
            (double)scrollOffset.x / displaySpecsPtr->magnification;
				 
}	// end "UpdateOffscreenMapOrigin"



void CMImageView::UpdateScrollRange (
				double 									magnification)
{
	/*
	TODO: For Linux
 
    // Force scroll range to be a multiple of the pixel size or
    // force to zero if no scroll present.

    SCROLLINFO scrollInfo;

    int maxRange;


    // Lock out recursive calls to OnSize.

    m_withinOnSizeFlag = TRUE;


    DWORD dwStyle = GetStyle();

    if (!(dwStyle & WS_HSCROLL)) {
        //		EnableScrollBarCtrl(SB_HORZ, 1);
        //		SetScrollRange(SB_HORZ, 0, 0, FALSE);
        //		EnableScrollBarCtrl(SB_HORZ, 0);

    }// end "if (!(dwStyle & WS_HSCROLL))"

    else // (dwStyle & WS_HSCROLL)
    {
        GetScrollInfo(SB_HORZ, &scrollInfo, SIF_RANGE);
        maxRange = scrollInfo.nMax;

        maxRange = (int) ((maxRange + magnification + .5) / magnification);
        maxRange = (int) (maxRange * magnification);

        scrollInfo.nMax = maxRange;
        SetScrollInfo(SB_HORZ, &scrollInfo, FALSE);

    } // end "else (dwStyle & WS_HSCROLL)"

    //	GetScrollRange(SB_HORZ, &minRange, &maxRange);

    if (!(dwStyle & WS_VSCROLL)) {
        //		EnableScrollBarCtrl(SB_VERT, 1);
        //		SetScrollRange(SB_VERT, 0, 0, FALSE);
        //		EnableScrollBarCtrl(SB_VERT, 0);

    }// end "!(dwStyle & WS_VSCROLL)"

    else // (dwStyle & WS_VSCROLL)
    {

        GetScrollInfo(SB_VERT, &scrollInfo, SIF_RANGE);
        maxRange = scrollInfo.nMax;

        maxRange = (int) ((maxRange + magnification + .5) / magnification);
        maxRange = (int) (maxRange * magnification);

        scrollInfo.nMax = maxRange;
        SetScrollInfo(SB_VERT, &scrollInfo, FALSE);

    } // end "else (dwStyle & WS_VSCROLL)"

    //	GetScrollRange(SB_VERT, &minRange, &maxRange);

    // Turn off lock out of recursive calls to OnSize.

    m_withinOnSizeFlag = FALSE;
	*/
}	// end "UpdateScrollRange"

/**
 * TODO: For Linux

void CMImageView::SetScrollSizes(int nMapMode,
        SIZE sizeTotal,
        const SIZE& sizePage,
        const SIZE& sizeLine) {
    ASSERT(sizeTotal.cx >= 0 && sizeTotal.cy >= 0);
    ASSERT(nMapMode > 0);
    ASSERT(nMapMode != MM_ISOTROPIC && nMapMode != MM_ANISOTROPIC);

    int nOldMapMode = m_nMapMode;
    m_nMapMode = nMapMode;
    m_totalLog = sizeTotal;

    //BLOCK: convert logical coordinate space to device coordinates
    {
        CWindowDC dc(NULL);
        ASSERT(m_nMapMode > 0);
        dc.SetMapMode(m_nMapMode);

        // total size
        m_totalDev = m_totalLog;
        //		dc.LPtoDP((LPPOINT)&m_totalDev);
        m_pageDev = sizePage;
        //		dc.LPtoDP((LPPOINT)&m_pageDev);
        m_lineDev = sizeLine;
    } // release DC here

    // now adjust device specific sizes
    ASSERT(m_totalDev.cx >= 0 && m_totalDev.cy >= 0);
    if (m_pageDev.cx == 0)
        m_pageDev.cx = m_totalDev.cx / 10;
    if (m_pageDev.cy == 0)
        m_pageDev.cy = m_totalDev.cy / 10;
    if (m_lineDev.cx == 0)
        m_lineDev.cx = m_pageDev.cx / 10;
    if (m_lineDev.cy == 0)
        m_lineDev.cy = m_pageDev.cy / 10;

    if (m_hWnd != NULL) {
        // window has been created, invalidate now
        UpdateBars();
        if (nOldMapMode != m_nMapMode)
            Invalidate(TRUE);
    }
} // end "SetScrollSizes"
*/

void CMImageView::UpdateScrolls (
				double 								newMagnification)
{
	Boolean 								clearWindowFlag = FALSE,
											xOffsetZeroFlag = FALSE,
											yOffsetZeroFlag = FALSE;

	double 								halfLogicalSize,
											columnLogicalCenter,
											lineLogicalCenter,
											oldMagnification;

	CPoint 								scrollOffset;

	Rect 									offScreenRectangle;

	RECT 									selRect;
											//windowRect;

	//SIZE 								lineIncrement,
	//            						pageIncrement;
	
	SInt32 								logicalOffset;

	UInt32 								height,
											newRectHeight,
											newRectWidth,
											oldRectHeight,
											oldRectWidth,
											step,
											width;

	UInt16 								offscreenHeight,
											offscreenWidth;

	SInt16 								titleHeight;


    		// Get the current size of the client portion of the window.  I.E.
    		// without scroll bars.

	titleHeight = GetTitleHeight();

	wxRect windowRect = m_Canvas->GetClientRect ();
	//GetClientRect(&windowRect);
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
				
	//SInt16 numberChars;

	if (selectionExistsFlag) // && newMagnification > oldMagnification)
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
		//if (scrollOffset.x == 0)
		//	xOffsetZeroFlag = TRUE;
		//if (scrollOffset.y == 0)
		//	yOffsetZeroFlag = TRUE;

		//if (offscreenHeight * oldMagnification < oldRectHeight)
		if (offscreenHeight * newMagnification < oldRectHeight)
			lineLogicalCenter = (double) height / 2.;

		else	// offscreenHeight*oldMagnification >= oldRectHeight
			{
			lineLogicalCenter = (double) oldRectHeight / 2. + scrollOffset.y;
			lineLogicalCenter *= newMagnification / oldMagnification;
			//lineLogicalCenter = (double) oldRectHeight / 2.;
			//lineLogicalCenter *= newMagnification / oldMagnification + scrollOffset.y;

			}	// end "else offscreenHeight*oldMagnification >= oldRectHeight"

		//if (offscreenWidth * oldMagnification < oldRectWidth)
		if (offscreenWidth * newMagnification < oldRectWidth)
			columnLogicalCenter = (double) width / 2.;

		else	// offscreenWidth*oldMagnification >= oldRectWidth
			{
			columnLogicalCenter = (double) oldRectWidth / 2. + scrollOffset.x;
			columnLogicalCenter *= newMagnification / oldMagnification;
			//columnLogicalCenter = (double) oldRectWidth / 2.;
			//columnLogicalCenter *= newMagnification / oldMagnification + scrollOffset.x;

			}	// end "else offscreenWidth*oldMagnification >= oldRectWidth"

		}	// end "else !selectionExistsFlag || ..."

			// Get new scroll position for the x direction.

	halfLogicalSize = ((double) newRectWidth) / 2.;
	//halfLogicalSize = 0;

	logicalOffset = (SInt32) MAX(0, columnLogicalCenter - halfLogicalSize);

	if (width < newRectWidth)
		//if (columnLogicalCenter < newRectWidth)
		//if (xOffsetZeroFlag)
		logicalOffset = 0;

	if (logicalOffset != 0)
		logicalOffset = NormalizeScrollOffset (newMagnification, logicalOffset, 0);

	scrollOffset.x = (int) logicalOffset;

			// Get new scroll position for the y direction.

	halfLogicalSize = ((double) newRectHeight) / 2.;
	//halfLogicalSize = 0;

	logicalOffset = (SInt32)MAX(0, (SInt32)(lineLogicalCenter - halfLogicalSize));

	if (height < newRectHeight)
		//if (lineLogicalCenter < newRectHeight)
		//if (yOffsetZeroFlag)
		logicalOffset = 0;

	if (logicalOffset != 0)
		logicalOffset = NormalizeScrollOffset (newMagnification, logicalOffset, 0);

	scrollOffset.y = (int) logicalOffset;

			// Set new scroll postions.

	if(scrollOffset.x >= 0 || scrollOffset.y >= 0)
		{
		m_Canvas->CanvasScroll ((int)(scrollOffset.x), (int)(scrollOffset.y));
              
		}
    
			// Set new magnification

	m_displayMultiCPtr->SetMagnification (newMagnification);

	UpdateOffscreenMapOrigin ();
	 
	m_mainFrame->UpdateStatusBar (m_frame, newMagnification);
    
}		// end "UpdateScrolls"



void CMImageView::UpdateSelectionCoordinates (void)
{
	CMImageDoc* imageDocCPtr = GetDocument ();

	if (imageDocCPtr->GetDisplayCoordinatesFlag ())
		(imageDocCPtr->GetImageFrameCPtr ())->UpdateSelectionCoordinates ();

}	// end "UpdateSelectionCoordinates"



void CMImageView::ZoomIn (void)

{
	double				magnification,
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
			if (!wxGetKeyState (WXK_CONTROL))
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

			}	// end "if (!m_ctlKeyDownFlag)"

		if (magnification >= 1.0)
			magnification += step;

		else // magnification < 1.0
			magnification = 1. / (inverse - step);

				// Set new magnification
		
		magnification = MIN (magnification, maxMagnification);
		m_Scale = magnification;
		UpdateScrolls (magnification);
		
		this->OnUpdate (this, NULL);
		UpdateScaleInformation (GetWindowInfoHandle (this));
	  
		}	// end "if (magnification < maxMagnification)"
		
	else	// At max zoom value
		m_mainFrame->SetZoomCode (0);

}	// end "ZoomIn"


void CMImageView::ZoomOut (void)

{
	double 						magnification,
									inverse,
									step;


	if (m_mainFrame->GetZoomCode () == 0)
																							return;

	magnification = m_displayMultiCPtr->GetMagnification ();

	if (magnification > gMinMagnification)
		{
		inverse = 1. / magnification;

		step = 0.1;

		#if defined multispec_wxlin
			if (!wxGetKeyState (WXK_CONTROL))
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

			}	// end "if (!m_ctlKeyDownFlag)"

		if (magnification > 1.)
			magnification -= step;

		else	// magnification <= 1.
			magnification = 1. / (inverse + step);
		
				// Set new magnification

		magnification = MAX (magnification, gMinMagnification);

		m_Scale = magnification;
		UpdateScrolls (magnification);
		this->OnUpdate (this, NULL);
		
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
	
	this->OnUpdate (this, NULL);
	UpdateScaleInformation (GetWindowInfoHandle (this));

}	// end "ZoomSize"
