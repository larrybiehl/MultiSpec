// CImagVew.cpp : implementation of the CMImageView class
//						This file contains routines that are common between the 
//						Windows and Mac implementation of the CMImageView class
//
//	revised 09/03/2013 by Larry L. Biehl

#include "SMulSpec.h"
                     
#include "CHistgrm.h"
#include "CImagVew.h"
#include "CImagWin.h"
#include "CDisplay.h"
#include "WMultiSpec.h"

#if defined multispec_lin
#include "WTLegend.h" 
#endif

#if defined multispec_lin
#include "MultiSpec2.h"
#endif

#include	"SExtGlob.h"

#if defined multispec_lin
IMPLEMENT_DYNAMIC_CLASS(CMImageView, wxView)
#endif

/////////////////////////////////////////////////////////////////////////////
// CMImageView

/////////////////////////////////////////////////////////////////////////////
// CMspecView construction/destruction

CMImageView::CMImageView()
{  
	m_displayMultiCPtr = NULL;
		
	m_histogramCPtr = NULL;
	
	m_imageWindowCPtr = NULL;
	
	m_activeFlag = FALSE; 
	
	m_withinOnSizeFlag = FALSE;
	
	m_callUpdateScrollRangeFlag = TRUE;	
	
	m_ctlKeyDownFlag = FALSE;

	m_thumbScrollFlag = FALSE; 
	
	m_printCopyModeFlag = FALSE;
		
	m_cursorColumnValue = -1;
	m_cursorLineValue = -1;
	
	m_printerTextScaling = 1.; 
	
	m_xPixelsPerInch = 72;
	m_yPixelsPerInch = 72;
	
	m_initializedFlag = FALSE;
	
#if defined multispec_win
	TRY
		{                      
				// Create Image Window Object for this image.
				
		Handle fileInfoHandle = 
				((CMultiSpecApp*)AfxGetApp())->GetOpenImageFileInfoHandle();
				   
		m_imageWindowCPtr = new CMImageWindow (fileInfoHandle);
		
		m_initializedFlag = m_imageWindowCPtr->m_initializedFlag;      
		
		if (m_initializedFlag)
			{	 
					// Create Display Image Object for this image.
						         
			m_displayMultiCPtr = new CMDisplay;   
				                             
			m_initializedFlag = m_displayMultiCPtr->m_initializedFlag; 
			
			}		// end "if (m_initializedFlag)"  
		
		if (m_initializedFlag)
			{	
					// Create Histogram Object for this image.
				                     
			m_histogramCPtr = new CMHistogram;
				                             
			m_initializedFlag = m_histogramCPtr->m_initializedFlag; 
			
			}		// end "if (m_initializedFlag)"
			
		if (m_initializedFlag)
			UpdateWindowList (this, kImageWindowType);
			
		}		// end "TRY"
		
	CATCH_ALL(e)
		{
		MemoryMessage(0, kObjectMessage);
		}
	END_CATCH_ALL
#endif
   
#if defined multispec_lin
   m_Scale = 1.0;
    try
    {
        // Create Image Window Object for this image.

        //Handle fileInfoHandle = ((CMultiSpecApp) wxGetApp()).GetOpenImageFileInfoHandle();
        Handle fileInfoHandle = ((CMultiSpecApp *)wxTheApp)->GetOpenImageFileInfoHandle();

        m_imageWindowCPtr = new CMImageWindow(fileInfoHandle);

        m_initializedFlag = m_imageWindowCPtr->m_initializedFlag;

        if (m_initializedFlag) {
            // Create Display Image Object for this image.

            m_displayMultiCPtr = new CMDisplay;

            m_initializedFlag = m_displayMultiCPtr->m_initializedFlag;

        } // end "if (m_initializedFlag)"

        if (m_initializedFlag) {
            // Create Histogram Object for this image.

            m_histogramCPtr = new CMHistogram;

            m_initializedFlag = m_histogramCPtr->m_initializedFlag;

        } // end "if (m_initializedFlag)"

        if (m_initializedFlag)
            UpdateWindowList(this, kImageWindowType);

    } // end "TRY"

    catch(int e) {
        MemoryMessage(0, kObjectMessage);
    }

#endif
	
}		// end "CMImageView"



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
		                                                      
		gActiveImageWindowInfoH = NULL;
		gActiveImageFileInfoH = NULL; 
		gActiveImageLayerInfoH = NULL;
		
		gActiveLegendWidth = 0;
		
		}		// end "if (gActiveImageViewCPtr == this)"
		
	RemoveWindowFromList (this, 0);
		
}		// end "~CMImageView"



//-----------------------------------------------------------------------------
//								 Copyright (1988-2010)
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
//	Coded By:			Larry L. Biehl			Date: 12/24/91
//	Revised By:			Larry L. Biehl			Date: 11/13/95	

void
CMImageView::DisposeImageWindowSupportMemory(void)

{  
	if (m_histogramCPtr != NULL)                               
		m_histogramCPtr->ReleaseHistogramSupportMemory ();
	                       
	if (m_displayMultiCPtr != NULL) 
		m_displayMultiCPtr->ReleaseDisplaySupportMemory ();
	
	m_imageWindowCPtr->ReleaseOffscreenSupportMemory ();
	
}		// end "DisposeImageWindowSupportMemory" 



//-----------------------------------------------------------------------------
//								 Copyright (1988-2010)
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
							m_imageWindowCPtr->GetWindowInfoHandle(), kNoLock, kNoMoveHi);
	
		if (windowInfoPtr != NULL)                                                   
			returnFlag = (windowInfoPtr->imageBaseAddressH != NULL);
			
		}		// end "if (this != NULL && m_imageWindowCPtr != NULL)"

	return (returnFlag);
	
}		// end "CheckIfOffscreenImageExists"

  
#ifdef _DEBUG  // debug version in mimagevw.cpp 
CMImageWindow* 
CMImageView::GetImageWindowCPtr() // non-debug version is inline
{  
	if (this == NULL)
		return (NULL);
		
	else		// this != NULL                                                                  
		return (CMImageWindow*)m_imageWindowCPtr;
}  	   
#endif	// _DEBUG  // debug version in mimagevw.cpp  



//-----------------------------------------------------------------------------
//								 Copyright (1988-2010)
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
//	Coded By:			Larry L. Biehl			Date: 04/03/96
//	Revised By:			Larry L. Biehl			Date: 04/04/96	

SInt16
CMImageView::GetClassGroupCode(void)

{  
	SInt16		classGroupCode = 0;
	
	         
			// Get the class/group code.
	                              
	if (this != NULL)
		{
		if (m_displayMultiCPtr != NULL)
			{                                                                    
			DisplaySpecsPtr displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer(
							m_displayMultiCPtr->mDisplaySpecsHandle, kNoLock, kNoMoveHi);
													
			if (displaySpecsPtr != NULL && !displaySpecsPtr->initializeStructureFlag) 
				classGroupCode = displaySpecsPtr->classGroupCode;
				
			}		// end "if (m_displayMultiCPtr != NULL)"
			
		}		// end "if (this != NULL)"

	return (classGroupCode);
	
}		// end "GetClassGroupCode" 



//-----------------------------------------------------------------------------
//								 Copyright (1988-2010)
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
//								 Copyright (1988-2010)
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

SInt16
CMImageView::GetNumberGroups(void)

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
//								 Copyright (1988-2010)
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
	titleHeight = ::GetTitleHeight(windowInfoHandle); 
	     
	return (titleHeight);
	
}		// end "GetTitleHeight"  



//-----------------------------------------------------------------------------
//								 Copyright (1988-2010)
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
//								 Copyright (1988-2010)
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
#ifndef multispec_lin
	if (this != NULL)
		{
		CMLegendList* legendListCPtr = GetImageLegendListCPtr ();
	
		if (legendListCPtr != NULL)                                     
			legendListCPtr->SetBitMapInfoHeaderHandle (legendBitMapInfoHeaderHandle);

		}		// end "if (this != NULL)"
#endif
	
}		// end "SetBitMapInfoHeader"     
