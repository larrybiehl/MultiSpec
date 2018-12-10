//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2017)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WImageView.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			08/30/2018
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Implementation of the CMImageView class.
//								This file contains routines that are common between the 
//								Linux and Windows implementation of the CMImageView class
//								Note that this file is not used in the linux version now.
//
//	Functions in file:	CMImageView
//								~CMImageView
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
                   
#include "CDisplay.h"  
#include "CHistogram.h"
#include "CImageWindow.h"

#include "WGraphView.h"
#include "WImageDoc.h"  
#include "WImageFrame.h"
#include "WImageView.h"
#include "WMainFrame.h"
#include "WMultiSpec.h"
#include "WLegendView.h"
#include "WTools.h"   



extern Handle 			GetCopyDIBHandle (
								CMImageView* 			imageViewCPtr);

extern void				DoNextDisplayChannelEvent (
								WindowPtr							window,
								char									theChar);

		// Special mapping modes just for CScrollView implementation

#define MM_NONE             0
#define MM_SCALETOFIT       (-1)
#define WM_RECALCPARENT     0x0368  // force RecalcLayout on frame window
									//  (only for inplace frame windows) 

SInt16		CMImageView::s_currentCursor = 0;      

RECT			CMImageView::s_updateRect;

IMPLEMENT_DYNCREATE(CMImageView, CScrollView);

BEGIN_MESSAGE_MAP(CMImageView, CScrollView)
	//{{AFX_MSG_MAP(CMImageView) 
	ON_MESSAGE(WM_DOREALIZE, OnDoRealize)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_SETUP, OnUpdateFilePrintSetup)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_WM_SETFOCUS()
	ON_WM_MOUSEMOVE() 
	ON_WM_NCMOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SIZE()
	ON_WM_SETCURSOR()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_SYSKEYUP()
	ON_WM_VSCROLL()
	ON_WM_PAINT()
	ON_WM_SYSKEYDOWN()
	ON_WM_HSCROLL()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	// Standard printing commands
END_MESSAGE_MAP()


//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		CMImageView
//
//	Software purpose:	Constructor 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None  				
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 06/05/1995
//	Revised By:			Larry L. Biehl			Date: 02/26/1997	

CMImageView::CMImageView ()
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
	
	TRY
		{                      
				// Create Image Window Object for this image.
				
		Handle fileInfoHandle = 
				((CMultiSpecApp*)AfxGetApp ())->GetOpenImageFileInfoHandle ();
				   
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
			
		}	// end "TRY"
		
	CATCH_ALL (e)
		{
		MemoryMessage (0, kObjectMessage);
		}
		
	END_CATCH_ALL
	
}	// end "CMImageView"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		~CMImageView
//
//	Software purpose:	Destructor 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None  				
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 06/05/1995
//	Revised By:			Larry L. Biehl			Date: 03/15/2018	

CMImageView::~CMImageView ()

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
		                                                      
		gActiveImageWindowInfoH = NULL;
		gActiveImageFileInfoH = NULL; 
		gActiveImageLayerInfoH = NULL;
		
		gActiveLegendWidth = 0;
		
		}	// end "if (gActiveImageViewCPtr == this)"
		
	RemoveWindowFromList (this, 0);

	if (gSelectionGraphViewCPtr != NULL)
		{
		if (GetSelectionGraphImageWindow (gSelectionGraphViewCPtr) == this)
			{
			GraphPtr		graphRecordPtr;
			Handle		graphRecordHandle;


			SetDefaultSelectionGraphWindowTitle (gSelectionGraphViewCPtr);
			graphRecordHandle = gSelectionGraphViewCPtr->GetGraphRecordHandle ();
			graphRecordPtr = (GraphPtr)GetHandlePointer (graphRecordHandle);
			graphRecordPtr->imageWindow = NULL;
			graphRecordPtr->imageViewCPtr = NULL;

			}	// end "if (GetSelectionGraphImageWindow (..."

		gSelectionGraphViewCPtr->SetCheckIOMemoryFlag (TRUE);

		}	// end "if (gSelectionGraphViewCPtr != NULL)"
		
}	// end "~CMImageView"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
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
//	Coded By:			Larry L. Biehl			Date: 08/01/1995
//	Revised By:			Larry L. Biehl			Date: 08/01/1995	

Boolean CMImageView::CheckIfOffscreenImageExists (void)

{  
	Boolean								returnFlag = FALSE;
	
	         
			// Verify that an offscreen images exists.
	
	if (this != NULL && m_imageWindowCPtr != NULL)
		{																								
		WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
														m_imageWindowCPtr->GetWindowInfoHandle ());
	
		if (windowInfoPtr != NULL)                                                   
			returnFlag = (windowInfoPtr->imageBaseAddressH != NULL);
			
		}	// end "if (this != NULL && m_imageWindowCPtr != NULL)"

	return (returnFlag);
	
}	// end "CheckIfOffscreenImageExists"



void CMImageView::ClientToDoc (CPoint& point)

{
	CClientDC dc(this);
	OnPrepareDC(&dc, NULL);
	dc.DPtoLP(&point);
	
}	// end "ClientToDoc"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
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
	
}	// end "DisposeImageWindowSupportMemory"

 

Boolean CMImageView::DoDeleteKey (
				UINT 						nChar)

{                                                         
	Boolean		charHandledFlag = FALSE;
	
	if (this != NULL)                    
		{                                                                  
		if (gProjectSelectionWindow != NULL && this == gProjectSelectionWindow)
			{
			if (gStatisticsMode == kStatNewFieldPolyMode && GetCapture() == this)
				{
						// A polygon selection is in process. Delete the last selected polygon 
						// point.
							
				CMTool* pTool = CMTool::FindTool(CMTool::c_toolType);
				if (pTool != NULL)
					pTool->OnChar(this, nChar);  
									
				charHandledFlag = TRUE;
					
				}	// end "if (gStatisticsMode == kStatNewFieldPolyMode && "
			
			else if (gStatisticsMode == kStatNewFieldRectMode || 
												gStatisticsMode == kStatNewFieldPolyMode )
				{
						// Delete the selected field if one exists. 
						
				if ( GetSelectionTypeCode (gActiveImageViewCPtr) != 0 )
					{               
					ClearSelectionArea (gActiveImageViewCPtr);
		
					ShowGraphSelection ();
									
					charHandledFlag = TRUE;
						
					}	// end "if ( GetSelectionTypeCode (gActiveImageViewCPtr) != 0)"
					
				}	// end "else if (gStatisticsMode == kStatNewFieldRectMode || ..."
				
			}	// end "if (gProjectSelectionWindow != NULL && ..." 
			
		}	// end "if (this != NULL)"
	
	return (charHandledFlag);
	
} 	// end "DoDeleteKey"



void CMImageView::DoEditCopy()
{
	CBitmap				cBmp;

   CRect					rect, 
							destinationRect;	// For storing the size of the image to be copied.

   CDC					hDC;

	LongPoint			topleftLPoint, 
							rightbottomLPoint;
	                
	LongPoint			tempPoint;
	
   Rect					sourceRect;
   
   SInt16				legendWidth;


			// Get the size of the window
	
	GetClientRect(rect);
	
  	Handle windowInfoHandle = GetWindowInfoHandle(gActiveImageWindow);
	Handle selectionInfoHandle = GetSelectionInfoHandle (windowInfoHandle);

	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer(
											windowInfoHandle, kLock, kNoMoveHi);
																											
	GetSelectedOffscreenRectangle ( windowInfoPtr,
												&sourceRect,
												TRUE,
												TRUE );

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

	destinationRect.bottom = (int)rightbottomLPoint.h;
	destinationRect.top = (int)topleftLPoint.h;
	destinationRect.right = (int)rightbottomLPoint.v;
	destinationRect.left = (int)topleftLPoint.v;

	CClientDC   cWndDC(this);					// View is an hWnd, so we can use "this"  
   hDC.CreateCompatibleDC(&cWndDC);			// Create the memory DC. 
                                                     
	destinationRect.bottom += ::GetTitleHeight(windowInfoHandle);

	legendWidth = ::GetLegendWidth(windowInfoHandle);

			//compare the height of image and the height of the legend

	if (legendWidth > 0)
		{
		SInt16 legendBottom = GetLegendFullHeight();
		destinationRect.bottom = MAX(destinationRect.bottom, 
												destinationRect.top + legendBottom);
		destinationRect.right += legendWidth;

		}	// end "if (legendWidth > 0)"  
                                                                     
   cBmp.CreateCompatibleBitmap(&cWndDC, 
   										destinationRect.Width(), 
   										destinationRect.Height() );
	
			// Keep the old bitmap
                                                       
   CBitmap* pOldBitmap = hDC.SelectObject(&cBmp);

			//prepare the DC
                      
	hDC.BitBlt( 0, 
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
			
	CopyOffScreenImage (this, 
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
   SetClipboardData (CF_BITMAP, cBmp.GetSafeHandle() );
   CloseClipboard () ;

        // next we select the old bitmap back into the memory DC
        // so that our bitmap is not deleted when cMemDC is destroyed.
        // Then we detach the bitmap handle from the cBmp object so that
        // the bitmap is not deleted when cBmp is destroyed.

	CheckAndUnlockHandle (windowInfoHandle);
	CheckAndUnlockHandle (selectionInfoHandle);
                                     
	hDC.SelectObject(pOldBitmap);
   cBmp.Detach();

}	// end "DoEditCopy"



void CMImageView::DoFilePrint ()

{
	gProcessorCode = kPrintProcessor;
	CScrollView::OnFilePrint();
	gProcessorCode = 0;
	
} 	// end "DoFilePrint"



void CMImageView::DoFilePrintPreview ()

{  
	gProcessorCode = kPrintProcessor;
	CScrollView::OnFilePrintPreview();
	gProcessorCode = 0;
	
} 	// end "DoFilePrintPreview"



void CMImageView::DrawLegend ()
{	                                          
	CMLegendView* legendViewCPtr = 
								GetDocument()->GetImageFrameCPtr()->GetLegendViewCPtr();

	CMLegendList* legendListPtr = (CMLegendList*)GetActiveLegendListHandle();
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

			}	// end "if (m_printCopyModeFlag)"
		*/
		legendListPtr->DrawLegendList(); 
		
		}	// end "if (legendListPtr != NULL)"
	
	CMComboBox* listBoxPtr = &(legendViewCPtr->m_legendTitleCombo); 
	if (listBoxPtr != NULL)
		listBoxPtr->DrawLegendTitle();  
	
}	// end "DrawLegend"



Boolean CMImageView::GetActiveWindowFlag (void)

{
	return (GetImageFrameCPtr()->GetActiveWindowFlag());

}	// end "GetActiveWindowFlag"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
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

SInt16 CMImageView::GetClassGroupCode (void)

{  
	SInt16								classGroupCode = 0;
	
	         
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


                                                              		
Boolean CMImageView::GetControlKeyFlag(void)
{  
	if (this != NULL)                  
		return (m_ctlKeyDownFlag);
		
	else
		return (FALSE); 
	
}	// end "SetContolKeyFlag"
 


UInt16 
CMImageView::GetDisplayPixelSize (void)

{                                       
	return ( GetImageFrameCPtr()->GetDisplayPixelSize() );
	
}	// end "GetDisplayPixelSize"



Handle CMImageView::GetDisplaySpecsHandle (void)

{
	Handle				displaySpecsHandle = NULL;
	   		
	if (this != NULL)
		displaySpecsHandle = m_displayMultiCPtr->mDisplaySpecsHandle;
		
	return (displaySpecsHandle);
		
}	// end "GetDisplaySpecsHandle"
    


CMImageFrame* CMImageView::GetImageFrameCPtr (void)

{
	CMImageFrame* 	imageFrameCPtr = NULL;
	
	if (this != NULL)
		{
		imageFrameCPtr = GetDocument()->GetImageFrameCPtr();
		
		}	// end "if (this != NULL)"
		
	return (imageFrameCPtr);
	
}	// end "GetImageFrameCPtr"
  


CMLegendList* CMImageView::GetImageLegendListCPtr (void)

{  
	CMLegendList* legendListCPtr = NULL;
	
	CMLegendView* legendViewCPtr = 
								GetDocument()->GetImageFrameCPtr()->GetLegendViewCPtr();
	
	if (legendViewCPtr != NULL)
		legendListCPtr = legendViewCPtr->GetLegendListCPtr();
									
	return ( legendListCPtr );
	
}	// end "GetImageLegendListCPtr"


CMLegendView* CMImageView::GetImageLegendViewCPtr (void)

{
	return ( GetDocument()->GetImageFrameCPtr()->GetLegendViewCPtr() );
	
}	// end "GetImageLegendViewCPtr"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
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

SInt16 CMImageView::GetImageType (void)

{  
	SInt16								imageType = 0;
	
	         
			// Get the window type. 
	                              
	if (this != NULL)
		{ 
		if (m_imageWindowCPtr != NULL) 
			imageType = ::GetImageType (m_imageWindowCPtr->GetWindowInfoHandle ());
				
		}	// end "if (this != NULL)"
	
	return (imageType);
	
}	// end "GetImageType"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetLegendFullHeight
//
//	Software purpose:	The purpose of this routine is to determine the full height of
//							the legend.
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

SInt16 CMImageView::GetLegendFullHeight (void)

{	   
	CRect									legendRect,
											listRect;
	
	Handle								windowInfoHandle;
	
	SInt16								legendFullHeight;
	

	windowInfoHandle = GetWindowInfoHandle (this);	
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	legendFullHeight = GetListBottom (windowInfoPtr->legendListHandle);
	CheckAndUnlockHandle (windowInfoHandle);
	
	if (windowInfoPtr->legendListHandle != NULL)
		{                                
		GetWindowRect (legendRect); 
		(GetImageLegendListCPtr ())->GetWindowRect (listRect);
		
		legendFullHeight += (SInt16)(listRect.top - legendRect.top);
		
		}	// end "if (windowInfoPtr->legendListHandle != NULL)"
	
	return (legendFullHeight); 

}	// end "GetLegendFullHeight" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetLegendWidth
//
//	Software purpose:	The purpose of this routine is to return the width of the legend.
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

SInt16 CMImageView::GetLegendWidth (void)

{	
	Handle windowInfoHandle = GetWindowInfoHandle (this);
	
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
											
	if (windowInfoPtr != NULL)
		return (windowInfoPtr->legendWidth);
		
	else	// windowInfoPtr != NULL   
		return (0);
												   
}	// end "GetLegendWidth" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
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
//	Coded By:			Larry L. Biehl			Date: 04/03/1996
//	Revised By:			Larry L. Biehl			Date: 04/04/1996	

SInt16 CMImageView::GetNumberGroups (void)

{  
	SInt16								numberGroups = 0;
	
	         
			// Get the number of groups.
	                              
	if (this != NULL)
		{ 
		if (m_imageWindowCPtr != NULL)
			{                                                                                                       
			Handle windowInfoHandle = m_imageWindowCPtr->GetWindowInfoHandle ();
			Handle fileInfoHandle = GetFileInfoHandle (windowInfoHandle);    
			                                          
			FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
													
			if (fileInfoPtr != NULL) 
				numberGroups = fileInfoPtr->numberGroups;
				
			}	// end "if (m_imageWindowCPtr != NULL)"
				
		}	// end "if (this != NULL)"

	return (numberGroups);
	
}	// end "GetNumberGroups"



CMOutlineArea* CMImageView::GetSelectionAreaCPtr(void)

{ 
	if (this != NULL)
		return ( GetDocument()->GetSelectionAreaCPtr() );
		
	else	// this == NULL 
		return ( NULL ); 

}	// end "GetSelectionAreaCPtr"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
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
//	Coded By:			Larry L. Biehl			Date: 04/30/1996
//	Revised By:			Larry L. Biehl			Date: 04/30/1996			

SInt16 CMImageView::GetTitleHeight (void)

{ 
	SInt16								titleHeight;
	
	
	Handle windowInfoHandle = GetWindowInfoHandle (this);
	titleHeight = ::GetTitleHeight (windowInfoHandle); 
	     
	return (titleHeight);
	
}	// end "GetTitleHeight"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
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
//	Coded By:			Larry L. Biehl			Date: 04/03/1996
//	Revised By:			Larry L. Biehl			Date: 04/04/1996	

SInt16 CMImageView::GetWindowType (void)

{  
	SInt16								windowType = 0;
	
	         
			// Get the window type. 
	                              
	if (this != NULL)
		{ 
		if (m_imageWindowCPtr != NULL) 
			windowType = ::GetWindowType (m_imageWindowCPtr->GetWindowInfoHandle ());
				
		}	// end "if (this != NULL)"
	
	return (windowType);
	
}	// end "GetWindowType"   



SInt32 CMImageView::NormalizeScrollOffset(
				double				magnification,
				SInt32				scrollIncrement,
				SInt32				scrollOffset)

{
	SInt32			normalizedIncrement;
	  
							
	normalizedIncrement = 
				(SInt32)(((double)scrollOffset + scrollIncrement)/magnification + .5);
	normalizedIncrement = MAX(0, normalizedIncrement);
		
	normalizedIncrement = (SInt32)((double)normalizedIncrement * magnification - scrollOffset);
	
	return (normalizedIncrement);                   
			
}	// end "NormalizeScrollOffset"

 

void CMImageView::OnActivateView(
				BOOL 			bActivate, 
				CView* 		pActivateView,
				CView* 		pDeactiveView)
				
{
	CScrollView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	
	m_activeFlag = bActivate; 
	if (bActivate)
		ASSERT(pActivateView == this); 
		
	Boolean changeWindowFlag = 
				(pActivateView == this && pDeactiveView != GetImageLegendViewCPtr() );
		
	GetImageFrameCPtr()->ActivateImageWindowItems (bActivate, changeWindowFlag);
	
}	// end "OnActivateView"   


/*
void CMImageView::OnBeginPrinting(
				CDC* pDC, 
				CPrintInfo* pInfo) 
				
{

}
*/


void CMImageView::OnChar (UINT nChar, UINT nRepCnt, UINT nFlags)

{  
	Boolean		charHandledFlag = FALSE;
	
	if (nChar == 0x08)
		charHandledFlag = DoDeleteKey (nChar); 
		
   else if (nChar == 13 && GetCapture() != this)
		{                        
		if ( gProjectSelectionWindow != NULL &&
		     	this == gProjectSelectionWindow &&
				(gStatisticsMode == kStatNewFieldRectMode ||
									gStatisticsMode == kStatNewFieldPolyMode) )
			{                  
			if ( GetSelectionTypeCode(gActiveImageViewCPtr) != 0 )
				{
				Point			location;
				location.h = 0;
				location.v = 0;
				StatisticsWControlEvent ( gProjectInfoPtr->addToControlH, 
													FALSE, 
													location,
													kAddToListControl);
					 
				}	// end "if ( GetSelectionTypeCode (gActiveImageViewCPtr) ..."
				
			}	// end "if ( gProjectSelectionWindow && ..."
			
		}	// end "else if (nChar == 13 && GetCapture() != this)"

	CScrollView::OnChar(nChar, nRepCnt, nFlags);
	
} 	// end "OnChar"



LRESULT CMImageView::OnDoRealize(
				WPARAM 			wParam, 
				LPARAM			lParam)

{  
	LRESULT			returnCode;
	
	                                  
	if (m_imageWindowCPtr == NULL || m_displayMultiCPtr == NULL)
	
			// must be a new document
																							return 0L; 
																							
			// If wParam == NULL, then assume that wParam is to equal the
			// m_hWnd parameter for this view. 
			// wParam is not used as of 2/27/97.
			
	//if (wParam == NULL)
	//		wParam = (WPARAM)m_hWnd;
	/*
	sprintf(	(char*)&gTextString,
				"Image OnDoRealize %s",
				gEndOfLine);
	OutputString ( NULL, 
					(char*)&gTextString, 
					0, 
					1, 
					TRUE);  
	*/
	returnCode = GetImageFrameCPtr()->DoRealize(lParam != 1, this);
	/*
	sprintf(	(char*)&gTextString,
				"%s",
				gEndOfLine);
	OutputString ( NULL, 
					(char*)&gTextString, 
					0, 
					1, 
					TRUE); 
	*/
	return returnCode;
	
}	// end "OnDoRealize"



void CMImageView::OnDraw(
				CDC* 				pDC)
				
{
	SCROLLINFO			scrollInfo;

	Boolean			continueFlag = TRUE;
	Rect				sourceRect;
	SInt16			copyType;
	                                 
	                                 
	if ( CheckIfOffscreenImageExists() )
		{ 
		pDC->SetMapMode(MM_TEXT);
		
		sourceRect.top = s_updateRect.top; 
		sourceRect.bottom = s_updateRect.bottom;		
		sourceRect.left = s_updateRect.left;  	
		sourceRect.right = s_updateRect.right;
		
				// Get area of window that it to be drawn.
				// Assume for now the entire client window.
				
		if (pDC->IsPrinting())   // printer DC
			{ 																					
			copyType = 5;                        
			
			}	// end "if (pDC->IsPrinting())"
			
		else   // not printer DC
			{             
					// Take any scroll offset into account
					
			CPoint scrollOffset;

			GetScrollInfo (SB_HORZ, &scrollInfo, SIF_ALL);
			scrollOffset.x = scrollInfo.nPos;
			GetScrollInfo (SB_VERT, &scrollInfo, SIF_ALL);
			scrollOffset.y = scrollInfo.nPos;
				
			sourceRect.top += scrollOffset.y;		  
			sourceRect.bottom += scrollOffset.y;
			sourceRect.left += scrollOffset.x;	
			sourceRect.right += scrollOffset.x;
			
			SInt16 cxHorizontal = pDC->GetDeviceCaps(HORZSIZE);
			SInt16 cxHorzRes = pDC->GetDeviceCaps(HORZRES);
			
			m_xPixelsPerInch = 
						(SInt16)( (double)cxHorzRes / cxHorizontal * 25.4 );
			
			SInt16 cyHorizontal = pDC->GetDeviceCaps(VERTSIZE);
			SInt16 cyHorzRes = pDC->GetDeviceCaps(VERTRES);
			
			m_yPixelsPerInch = 
						(SInt16)( (double)cyHorzRes / cyHorizontal * 25.4 );
	
					// Verify that rect is visible.
			
			continueFlag = pDC->RectVisible((RECT*)&sourceRect);
																				
 			copyType = 4;
			
			}	// end "else not printer DC"
		
		if (continueFlag)
			{
			gCDCPointer = pDC;
			m_pDC = pDC;
			                               
			CopyOffScreenImage ( this,
									pDC,
									m_imageWindowCPtr,
									NULL, 
									&sourceRect,
									copyType);
									
			gCDCPointer = NULL; 
			m_pDC = NULL;
			
         }	// end "if (continueFlag)"
              
		if ( !pDC->IsPrinting() )
			{            
	         	// Outline selection area if one exists.
		
			CMImageDoc* pDoc = GetDocument(); 
	                                
			pDoc->Draw(pDC, this);
			
			}	// end "if ( !pDC->IsPrinting() )"
	
		}	// end "if ( CheckIfOffscreenImageExists() )"
		
}	// end "OnDraw" 
     


void CMImageView::OnEditCut ()

{
	
}	// end "OnEditCut"

             

void CMImageView::OnEditPaste (void)

{                        
	
}	// end "OnEditPaste"


/*
void CMImageView::OnEndPrinting(
				CDC* pDC, 
				CPrintInfo* pInfo)
				
{
	
}	// end "OnEndPrinting"  
*/



BOOL CMImageView::OnEraseBkgnd(CDC* pDC) 
{        
	if ( CheckIfOffscreenImageExists() )
		{
		SInt16					titleHeight; 
		
		
		CBrush brush( GetSysColor( COLOR_WINDOW ) ); 
		
		titleHeight = GetTitleHeight();
		                                
		if (titleHeight > 0)
			{
			RECT				windowRect;
									      
			GetClientRect( &windowRect );
			                               
			windowRect.top = 0;
			windowRect.bottom = titleHeight;                        
			pDC->FillRect (&windowRect, &brush);
				
			}	// end "if (titleHeight > 0)" 
		                                          
	   FillOutsideRect( pDC, &brush );
	   return TRUE;                   // Erased  
	   
	   }	// end "if ( CheckIfOffscreenImageExists() )" 
	   
	else	// !CheckIfOffscreenImageExists()
		return CScrollView::OnEraseBkgnd(pDC);

}	// end "OnEraseBkgnd"



void CMImageView::OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)

{
	if (nSBCode == SB_THUMBPOSITION || nSBCode == SB_THUMBTRACK)
		m_thumbScrollFlag = TRUE;
	
	CScrollView::OnHScroll(nSBCode, nPos, pScrollBar);

	m_thumbScrollFlag = FALSE;

}	// end "OnHScroll"



void CMImageView::OnInitialUpdate(void)

{  
	CSize					sizeDoc;
	SCROLLINFO			scrollInfo;

	SIZE					lineIncrement,
							pageIncrement;
							
	SInt16				windowType; 
							                      
   
   lineIncrement.cx = 10;
   lineIncrement.cy = 10;
   
   pageIncrement.cx = 20;
   pageIncrement.cy = 20;

	sizeDoc = GetDocument()->GetDocSize();
//	if (GetDocument()->GetDisplayCoordinatesFlag())
			// Do not include size of coordinate bar in the scroll settings
//		sizeDoc.cy -= 33;
   
	SetScrollSizes (MM_TEXT, 
							sizeDoc, 
							pageIncrement, 
							lineIncrement);
	ResizeParentToFit(FALSE);   

	scrollInfo.fMask = SIF_RANGE;
	scrollInfo.nMin = 0;

	scrollInfo.nMax = sizeDoc.cx;
	SetScrollInfo (SB_HORZ, &scrollInfo, FALSE);

	scrollInfo.nMax = sizeDoc.cy;
	SetScrollInfo (SB_VERT, &scrollInfo, FALSE);
	
	windowType = GetWindowType ();
			
	if (windowType == kThematicWindowType)
		{
		CMImageFrame* imageFrameCPtr = GetDocument()->GetImageFrameCPtr();
		imageFrameCPtr->InitialUpdate( GetDocument()->GetDocSize() ); 
		
		}	// end "if (windowType == kThematicWindowType)"
			
//	else	// windowType != kThematicType)
//		ResizeParentToFit(FALSE); 
	
	CScrollView::OnInitialUpdate(); 
                            
	if (m_imageWindowCPtr != NULL)
		{
		gActiveImageViewCPtr = this; 
		gActiveImageWindow = this; 
		                                                      
		gActiveImageWindowInfoH = GetActiveImageWindowInfoHandle();
		gActiveImageFileInfoH = GetActiveImageFileInfoHandle(); 
		gActiveImageLayerInfoH = GetActiveImageLayerInfoHandle();      
		
				// Allow ability to go from image window class back to
				// image view class.
		
		m_imageWindowCPtr->mImageViewCPtr = this;     
		
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
					
			CMImageFrame* imageFrameCPtr = GetDocument()->GetImageFrameCPtr();
			imageFrameCPtr->SetImageViewCPtr (this);
			
			}	// end "else windowType != kThematicWindowType"    
		
//   	gMemoryTypeNeeded = 1;  	
//		DisplayImage();			
//   	gMemoryTypeNeeded = 0;
   	
   	}	// end "if (m_imageWindowCPtr != NULL)"
	
}	// end "OnInitialUpdate"


                                                              		
void CMImageView::OnKeyDown (
				UINT nChar, UINT nRepCnt, UINT nFlags)

{        
	if (nChar == 0x11)
		m_ctlKeyDownFlag = TRUE;

	else if (nChar == 0x25 || nChar == 0x26 || nChar == 0x27 || nChar == 0x28)
		DoNextDisplayChannelEvent (this, nChar);

	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
	
}	// end "OnKeyDown"

 

void CMImageView::OnKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags)

{                                                                  
	if (nChar == 0x11)
		m_ctlKeyDownFlag = FALSE;
	
	CScrollView::OnKeyUp(nChar, nRepCnt, nFlags);

}	// end "OnKeyUp"



void CMImageView::OnLButtonDblClk (UINT nFlags, CPoint point)

{                                                                                                              
	if (!m_activeFlag || !CheckIfOffscreenImageExists())
		return;
	
	CMTool* pTool = CMTool::FindTool(CMTool::c_toolType);
	if (pTool != NULL)
		pTool->OnLButtonDblClk(this, nFlags);
	
	CScrollView::OnLButtonDblClk(nFlags, point);
	
}	// end "OnLButtonDblClk"



void CMImageView::OnLButtonDown (UINT nFlags, CPoint point)

{                                                                 
	if (!m_activeFlag || !CheckIfOffscreenImageExists())
		return;  
				
	//::SetCursor(AfxGetApp()->LoadCursor (gCrossCursorID));
	//gPresentCursor = kCross;
		
	CMTool* pTool = CMTool::FindTool(CMTool::c_toolType);
	if (pTool != NULL)
		pTool->OnLButtonDown (this, nFlags, point);
	
	CScrollView::OnLButtonDown (nFlags, point);

}	// end "OnLButtonDown"



void CMImageView::OnLButtonUp(UINT nFlags, CPoint point)

{                                                                                                              
	if (!m_activeFlag || !CheckIfOffscreenImageExists())
		return;
		
	CMTool* pTool = CMTool::FindTool(CMTool::c_toolType);
	if (pTool != NULL)                                                   
		pTool->OnLButtonUp(this, nFlags, point, gNewSelectedWindowFlag);
		                              
	gNewSelectedWindowFlag = FALSE;
	
	CScrollView::OnLButtonUp (nFlags, point);

}	// end "OnLButtonUp"



void CMImageView::OnMouseMove (
				UINT 			nFlags, 
				CPoint 		point)

{                                                
	if (GetActiveWindowFlag () && CheckIfOffscreenImageExists ())
		{
		Boolean			handledFlag = FALSE;
		
		                               
		CMTool* pTool = CMTool::FindTool(CMTool::c_toolType);
		
				// Set Cursor 
		/*
		if (gPresentCursor != kCross)
			{		
			//SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
			//::SetCursor(AfxGetApp()->LoadCursor(IDC_CROSS_CURSOR));
			::SetCursor(AfxGetApp()->LoadCursor(gCrossCursorID));
			gPresentCursor = kCross;
			
			}	// end "if (gPresentCursor != kCross)"
		*/
		if (pTool != NULL)
			handledFlag = pTool->OnMouseMove(this, nFlags, point);
		
		if (!handledFlag)
			{
			LongPoint longLocalPoint;
			longLocalPoint.h = point.x;
			longLocalPoint.v = point.y;	
			UpdateCursorCoordinates (&longLocalPoint);
			
			}	// end "if (!handledFlag)"
			
		}	// end "if (GetActiveWindowFlag() && ..."
	/*
	else	// !GetActiveWindowFlag() || !CheckIfOffscreenImageExists()
		{    
		if (gPresentCursor != kWait && gPresentCursor != kSpin)
			{		
			SetCursor(AfxGetApp()->LoadStandardCursor (IDC_ARROW));
			gPresentCursor = kArrow;
			
			if (gActiveImageViewCPtr != NULL)                
				gActiveImageViewCPtr->UpdateCursorCoordinate();
				
			}	// end "if (gPresentCursor != kWait && ..."
		   	
		}	// end "else !GetActiveWindowFlag() || ..."
	*/
	CScrollView::OnMouseMove (nFlags, point);
	
}	// end "OnMouseMove"    


/*
void CMImageView::OnNcMouseMove(
				UINT 			nFlags, 
				CPoint 		point)

{  	
	if (gPresentCursor != kWait && gPresentCursor != kSpin)
		{		
		SetCursor(AfxGetApp()->LoadStandardCursor (IDC_ARROW));
		gPresentCursor = kArrow;            
			            
		if (gActiveImageViewCPtr != NULL)                
			gActiveImageViewCPtr->UpdateCursorCoordinates();
			
		}	// end "if (gPresentCursor != kWait && ..."              
	
	CScrollView::OnNcMouseMove (nFlags, point); 
	
}	// end "OnNcMouseMove"
*/


void CMImageView::OnPaint ()

{                   
//	Boolean			returnFlag;
//	RECT				updateRect; 
	
	
	if ( GetUpdateRect(&s_updateRect) )
		{ 
			
		CPaintDC dc(this); // device context for painting 
		OnPrepareDC(&dc);
		OnDraw(&dc); 
		
		}	// end "	if ( GetUpdateRect(&updateRect) )"    

	//already
	// Do not call CScrollView::OnPaint() for painting messages
}



BOOL CMImageView::OnPreparePrinting(
				CPrintInfo			*pInfo)

{
	// default preparation
	return DoPreparePrinting(pInfo);
}


                                                                             
void CMImageView::OnPrint(CDC* pDC, CPrintInfo* pInfo)

{
	double 		saveMagnification;
	
	Rect			sourceRect;
	
	SInt16		legendHeight,
					legendWidth,
					titleHeight;
	
	/*
	if (pInfo->m_nCurPage == 1)  // page no. 1 is the title page
		{
		PrintTitlePage(pDC, pInfo); 
		
		return; // nothing else to print on page 1 but the page title 
		
		}	// end "if (pInfo->m_nCurPage == 1)"
	
	CString strHeader = GetDocument()->GetTitle();

	PrintPageHeader(pDC, pInfo, strHeader);
	
			// PrintPageHeader() subtracts out from the pInfo->m_rectDraw the
			// amount of the page used for the header.

	pDC->SetWindowOrg(pInfo->m_rectDraw.left,-pInfo->m_rectDraw.top);
 	*/
 			// If Stretch Device Independant Bitmaps are not support on this
 			// printer, then exit.
 			            
	int returnValue = pDC->GetDeviceCaps(RASTERCAPS);
	if ( !(returnValue & RC_STRETCHDIB) )
																						return; 													
	
			// Get the source rectangle.
				   
  	Handle windowInfoHandle = GetWindowInfoHandle(this);
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer(
												windowInfoHandle, kLock, kNoMoveHi);
																								
	GetSelectedOffscreenRectangle ( windowInfoPtr,
												&sourceRect,
												TRUE,
												TRUE);
														
	CheckAndUnlockHandle (windowInfoHandle); 
	
			// Get the legend width.
			
	legendWidth = ::GetLegendWidth (windowInfoHandle);
	legendHeight = GetLegendFullHeight();
	titleHeight = ::GetTitleHeight (windowInfoHandle);
	
			// Get the width and height of the printed page in pixels. 
					
	SInt16 pageWidth = pDC->GetDeviceCaps(HORZRES);
	SInt16 pageHeight = pDC->GetDeviceCaps(VERTRES); 
	
			// Get the display pixels per inch.
			
	SInt16 cxInch = pDC->GetDeviceCaps(LOGPIXELSX);
	SInt16 cyInch = pDC->GetDeviceCaps(LOGPIXELSY);
	
			// Get the scaling needed to account for the difference in the display
			// and paper pixels per inch. 
	
	m_printerTextScaling = (double)cyInch/m_yPixelsPerInch;  
	
			// Get the magnification for the selected image scaled to one page.
			// Need to use 1.4 here to get the proper width on the printer page.
			// Do not yet know why or whether it works on all printers.
			
	double magWidth = ((double)pageWidth - 1.4 * legendWidth * m_printerTextScaling)/
												(sourceRect.right - sourceRect.left + 1);
			
	double magHeight = ((double)pageHeight - titleHeight * m_printerTextScaling)/
												(sourceRect.bottom - sourceRect.top + 1);
												
	double onePageScaleMagnification = MIN(magHeight, magWidth);
	
			// Get the user magnification to use to scale the image the same
			// as is used in the image window. Take into account any difference
			// in printer and display pixels per inch. Also take into account
			// the current image window magnification. 		
		                                                                 
	DisplaySpecsPtr displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer(
					m_displayMultiCPtr->mDisplaySpecsHandle, kNoLock, kNoMoveHi);
	
	magWidth = displaySpecsPtr->magnification * cxInch / m_xPixelsPerInch;
	magHeight = displaySpecsPtr->magnification * cyInch / m_yPixelsPerInch; 
	double userMagnification = MIN(magHeight, magWidth); 
   
   		// Store source rectangle to be used by "Draw"
   		
 	s_updateRect.left = sourceRect.left;
 	s_updateRect.right = sourceRect.right;
 	s_updateRect.top = sourceRect.top;
 	s_updateRect.bottom = sourceRect.bottom;
 	
 			// Save the image window magnification and set the magnification
 			// to be used for the printed page.
 			
 	saveMagnification = displaySpecsPtr->magnification;
 	displaySpecsPtr->magnification = MIN(
 										onePageScaleMagnification, userMagnification);
 										
 			// Set the origin so that the image is centered on the page. 
			                                             
	SInt16 centerAdjustWidth = (SInt16)(displaySpecsPtr->magnification *
												(sourceRect.right - sourceRect.left + 1) +
												1.4 * legendWidth * m_printerTextScaling );
	centerAdjustWidth = MAX( (pageWidth - centerAdjustWidth)/2, 0); 
	
	SInt16 centerAdjustHeight1 = (SInt16)(displaySpecsPtr->magnification *
												(sourceRect.bottom - sourceRect.top + 1) +
												titleHeight*m_printerTextScaling ); 
	SInt16 centerAdjustHeight2 = (SInt16)(legendHeight*m_printerTextScaling);
	SInt16 centerAdjustHeight = MAX(centerAdjustHeight1, centerAdjustHeight2);
												
	centerAdjustHeight = MAX( (pageHeight - centerAdjustHeight)/2, 0);

	pDC->SetWindowOrg(pInfo->m_rectDraw.left-centerAdjustWidth,
							pInfo->m_rectDraw.top-centerAdjustHeight);
 	
			// Now print the rest of the page 
			        
	m_printCopyModeFlag = TRUE;
	OnDraw (pDC);		
	m_printCopyModeFlag = FALSE;
	
			// Restore the magnification 		
		                                                          
	displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer(
						m_displayMultiCPtr->mDisplaySpecsHandle, kNoLock, kNoMoveHi);
	displaySpecsPtr->magnification = saveMagnification;
	
	m_printerTextScaling = 1.0;										
	
}	// end "OnPrint"

   

BOOL 
CMImageView::OnScrollBy(
				CSize 			sizeScroll, 
				BOOL 				bDoScroll)
				
{
	SCROLLINFO			scrollInfo;

	CPoint					scrollOffset;
	
	 
			// Force scroll step to be even multiple of pixel size.
			                                                       
	double magnification = m_displayMultiCPtr->GetMagnification();

			// Get current scroll setting.

	if (sizeScroll.cx != 0)
		{
		GetScrollInfo (SB_HORZ, &scrollInfo, SIF_ALL); // SIF_TRACKPOS
		scrollOffset.x = scrollInfo.nPos;
		if (m_thumbScrollFlag)
			sizeScroll.cx = scrollInfo.nTrackPos - scrollInfo.nPos;

		sizeScroll.cx = (int)NormalizeScrollOffset(magnification, 
																sizeScroll.cx,
																scrollOffset.x);
		
		}	// end "if (sizeScroll.cx != 0)"
				
	if (sizeScroll.cy != 0)
		{	   
		GetScrollInfo (SB_VERT, &scrollInfo, SIF_ALL);
		scrollOffset.y = scrollInfo.nPos;
		if (m_thumbScrollFlag)
			sizeScroll.cy = scrollInfo.nTrackPos - scrollInfo.nPos;
			
		sizeScroll.cy = (int)NormalizeScrollOffset(magnification, 
																sizeScroll.cy,
																scrollOffset.y); 
	
		SInt16 titleHeight = GetTitleHeight();
		if (titleHeight > 0)
			{
			RECT				windowRect;
								      
			GetClientRect( &windowRect );
		                               
			windowRect.top = 0;
			windowRect.bottom = titleHeight;                        
			InvalidateRect(&windowRect, TRUE);
			
			}	// end "if (titleHeight > 0)"
			
		}	// end "if (sizeScroll.cy != 0)"
	
	
	BOOL scrolledFlag = CScrollView::OnScrollBy(sizeScroll, bDoScroll);

	if (scrolledFlag)                            
		UpdateOffscreenMapOrigin ();
	
	return (scrolledFlag);

}	// end "OnScrollBy"



BOOL CMImageView::OnSetCursor (
				CWnd* 		pWnd, 
				UINT 			nHitTest, 
				UINT 			message)

{  
			// This is done so that the 'OnSetCursor' routine in CMImageFrame
			// does not get called.
			                                                      
//	return TRUE;
	
//already	
//	return CScrollView::OnSetCursor(pWnd, nHitTest, message);

	if (gPresentCursor == kWait || gPresentCursor == kSpin)
		{
				// Wait cursor in affect. Processing underway.
				// Restart the wait cursor in case in was change to pointer
				// before entering the image frame.
		
		AfxGetApp ()->DoWaitCursor (0);
																					return (TRUE);
		
		}	// end "if (gPresentCursor == kWait || gPresentCursor == kSpin)"

	if (GetActiveWindowFlag () && CheckIfOffscreenImageExists ())
		{
		if (gPresentCursor != kCross)
			{		
			::SetCursor(AfxGetApp()->LoadCursor (gCrossCursorID));
			gPresentCursor = kCross;
			
			}	// end "if (gPresentCursor != kCross)"
		
																					return (TRUE);
			
		}	// end "if (GetActiveWindowFlag() && ..."
		
	else	// !GetActiveWindowFlag () || !CheckIfOffscreenImageExists ()
		{    
		//::SetCursor (AfxGetApp()->LoadStandardCursor (IDC_ARROW));
			
		if (gActiveImageViewCPtr != NULL)
			gActiveImageViewCPtr->UpdateCursorCoordinates();
				
		gPresentCursor = kArrow;
			
		return CScrollView::OnSetCursor (pWnd, nHitTest, message);
		   	
		}	// end "else !GetActiveWindowFlag() || ..."

}	// end "OnSetCursor"



void CMImageView::OnSetFocus(
				CWnd* 				pOldWnd)

{         
			// These settings will change only when
			// there is no active processing funtion running.
	                                		
	if (gProcessorCode == 0) 
		GetImageFrameCPtr()->UpdateActiveImageWindowInfo(); 
		                                     
	CScrollView::OnSetFocus(pOldWnd);
	
}	// end "OnSetFocus"



void CMImageView::OnSize (
				UINT 		nType, 
				int 		cx, 
				int 		cy)
				
{  
	if ( m_withinOnSizeFlag )
																						return;
																						
	CScrollView::OnSize(nType, cx, cy);
	
//	if (m_activeFlag && m_callUpdateScrollRangeFlag)
	if (m_callUpdateScrollRangeFlag &&
								GetImageFrameCPtr()->GetActiveWindowFlag() )
		{                                
		double magnification = m_displayMultiCPtr->GetMagnification();
		UpdateScrollRange (magnification); 
			                       
		UpdateOffscreenMapOrigin ();
		
		}	// end "if (m_callUpdateScrollRangeFlag && ...)"                
	
}	// end "OnSize"

 

void CMImageView::OnSysKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)

{
	CScrollView::OnSysKeyDown(nChar, nRepCnt, nFlags);
	
} 	// end "OnSysKeyDown"



void CMImageView::OnSysKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags)

{                                                                                                       
	if (nChar == 0x11)
		m_ctlKeyDownFlag = FALSE;
	
	CScrollView::OnSysKeyUp (nChar, nRepCnt, nFlags);
	
}	// end "OnSysKeyUp"


							
void CMImageView::OnUpdate(
				CView*		pSender,
				LPARAM		lHint,
				CObject*		pHint)
				
{  
			// The following is what is in "CView::OnUpdate" except that the
			// argument passed is TRUE.
	/*
	sprintf(	(char*)&gTextString,
				"Image Invalidate%s",
				gEndOfLine);
	OutputString ( NULL, 
					(char*)&gTextString, 
					0, 
					1, 
					TRUE);
	*/
	Invalidate (FALSE);

}	// end "OnUpdate"



void CMImageView::OnUpdateEditClear(
				CCmdUI* 		pCmdUI)
{                                                                                                                                                     
	pCmdUI->Enable(FALSE);
	
}	// end "OnUpdateEditClear"



void CMImageView::OnUpdateEditCut(
				CCmdUI* 		pCmdUI)
{                                                                                                             
	pCmdUI->Enable(FALSE);
	
}

            

void CMImageView::OnUpdateEditPaste(
				CCmdUI* 		pCmdUI)

{                                                                                                             
	pCmdUI->Enable(FALSE);
	
}
  


void CMImageView::OnUpdateEditUndo(
				CCmdUI* 		pCmdUI) 
				
{                                                                                                                                                     
	pCmdUI->Enable(FALSE);
	
}	// end "OnUpdateEditUndo"

 
 
void CMImageView::OnUpdateFilePrintSetup(
				CCmdUI* 		pCmdUI)
				
{	                                                              
	pCmdUI->Enable(TRUE);
	
}	// end "OnUpdateFilePrintSetup"



void CMImageView::OnVScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)

{              	    
	if (nSBCode == SB_THUMBPOSITION || nSBCode == SB_THUMBTRACK)
		m_thumbScrollFlag = TRUE;      
	
	CScrollView::OnVScroll (nSBCode, nPos, pScrollBar);

	m_thumbScrollFlag = FALSE;      
	
}	// end "OnVScroll"

 

void CMImageView::Remove (CMDrawObject* pObject)

{
	/*
	POSITION pos = m_selection.Find(pObject);
	if (pos != NULL)
		m_selection.RemoveAt(pos);
	*/
}	// end "Remove"



void CMImageView::ScrollH (UINT nSBCode)

{
	OnHScroll(nSBCode,0,NULL);
	
}	// end "ScrollH"



void CMImageView::ScrollV (UINT nSBCode)

{
	OnVScroll(nSBCode,0,NULL);
	
}	// end "ScrollV"


                                                              		
void CMImageView::SetControlKeyFlag(
			Boolean				flag)
{  
	if (this != NULL)                  
		m_ctlKeyDownFlag = flag; 
	
}	// end "SetContolKeyFlag"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
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
//	Coded By:			Larry L. Biehl					Date:	05/10/1995
//	Revised By:			Larry L. Biehl					Date: 05/10/1995

void CMImageView::SetImageWindowCPtr (
				CMImageWindow								*imageWindowCPtr)

{
	m_imageWindowCPtr = imageWindowCPtr;
	
}	// end "SetImageWindowCPtr" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetLegendBitMapInfoHeaderHandle
//
//	Software purpose:	The purpose of this routine is to set the handle for the 
//							legend bitmap info structure in the legend list class.
//
//	Parameters in:		Image Window class pointer	
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl					Date:	05/10/1995
//	Revised By:			Larry L. Biehl					Date: 05/10/1995

void CMImageView::SetLegendBitMapInfoHeaderHandle (
				Handle										legendBitMapInfoHeaderHandle)

{  
	if (this != NULL)
		{
		CMLegendList* legendListCPtr = GetImageLegendListCPtr ();
	
		if (legendListCPtr != NULL)                                     
			legendListCPtr->SetBitMapInfoHeaderHandle (legendBitMapInfoHeaderHandle);

		}	// end "if (this != NULL)"
	
}	// end "SetBitMapInfoHeader"

 

void CMImageView::SetScrollSizes(int nMapMode, 
											SIZE sizeTotal,
											const SIZE& sizePage, 
											const SIZE& sizeLine)
{
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

	if (m_hWnd != NULL)
		{
		// window has been created, invalidate now
		UpdateBars();
		if (nOldMapMode != m_nMapMode)
			Invalidate(TRUE);
		}
	
}	// end "SetScrollSizes"

	

//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateCursorCoordinates
//
//	Software purpose:	The purpose of this routine is to update the cursor
//							coordinates in the coordinate dialog window if it is
//							showing.
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

void	
CMImageView::UpdateCursorCoordinates (
				LongPoint*							mousePtPtr)

{                                            
	Boolean								updateMapCoordinateFlag;
	
		                 
	CMImageDoc* imageDocCPtr = GetDocument();
	
	if (imageDocCPtr->GetDisplayCoordinatesFlag () &&
						 					CheckIfOffscreenImageExists ())
		{
		updateMapCoordinateFlag = GetCursorCoordinates (mousePtPtr);
		
		if (updateMapCoordinateFlag)
			DrawCursorCoordinates (gActiveImageWindowInfoH); 
			
		}	// end "if ( imageDocCPtr->GetDisplayCoordinatesFlag() && ...)" 

}	// end "UpdateCursorCoordinates"

	

//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateCursorCoordinates
//
//	Software purpose:	The purpose of this routine is to update the cursor
//							coordinates in the coordinate dialog window if it is
//							showing.
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
//	Coded By:			Larry L. Biehl			Date: 06/22/92
//	Revised By:			Larry L. Biehl			Date: 08/03/95	

void	CMImageView::UpdateCursorCoordinates (void)

{   	                 
	CMImageDoc* imageDocCPtr = GetDocument();
	
	if ( imageDocCPtr->GetDisplayCoordinatesFlag() )                              
		(imageDocCPtr->GetImageFrameCPtr())->UpdateCursorCoordinates(); 

}	// end "UpdateCursorCoordinates"

   

void CMImageView::UpdateOffscreenMapOrigin(void)
				
{  			
	SCROLLINFO			scrollInfo;

	CPoint scrollOffset;

	GetScrollInfo (SB_HORZ, &scrollInfo, SIF_POS);
	scrollOffset.x = scrollInfo.nPos;
	GetScrollInfo (SB_VERT, &scrollInfo, SIF_POS);
	scrollOffset.y = scrollInfo.nPos;
	
	Handle displaySpecsHandle = m_displayMultiCPtr->mDisplaySpecsHandle;
	DisplaySpecsPtr displaySpecsPtr = 
						(DisplaySpecsPtr)GetHandlePointer(
												displaySpecsHandle, kNoLock, kNoMoveHi);
		
	displaySpecsPtr->origin[kVertical] = 
					(SInt16)((double)scrollOffset.y/displaySpecsPtr->magnification + .5);
	displaySpecsPtr->origin[kHorizontal] = 
					(SInt16)((double)scrollOffset.x/displaySpecsPtr->magnification + .5);

}	// end "UpdateOffscreenMapOrigin"  



void CMImageView::UpdateScrollRange(
				double				magnification)

{           
	
			// Force scroll range to be a multiple of the pixel size or
			// force to zero if no scroll present.

	SCROLLINFO			scrollInfo;
	
	int	 					maxRange; 
	
	                     
			// Lock out recursive calls to OnSize.
				
	m_withinOnSizeFlag = TRUE; 
	
	   
	DWORD dwStyle = GetStyle(); 
	     
	if ( !(dwStyle & WS_HSCROLL) )
		{     
//		EnableScrollBarCtrl(SB_HORZ, 1);  
//		SetScrollRange(SB_HORZ, 0, 0, FALSE);
//		EnableScrollBarCtrl(SB_HORZ, 0);
		
		}	// end "if (!(dwStyle & WS_HSCROLL))"
		
	else	// (dwStyle & WS_HSCROLL)
		{
		GetScrollInfo (SB_HORZ, &scrollInfo, SIF_RANGE);
		maxRange = scrollInfo.nMax;
		
		maxRange = (int)((maxRange + magnification + .5)/magnification);
	   maxRange = (int)(maxRange * magnification);

		scrollInfo.nMax = maxRange;
		SetScrollInfo (SB_HORZ, &scrollInfo, FALSE);
		
		}	// end "else (dwStyle & WS_HSCROLL)"   
		
//	GetScrollRange(SB_HORZ, &minRange, &maxRange);   
	                                          
	if ( !(dwStyle & WS_VSCROLL) )
		{  
//		EnableScrollBarCtrl(SB_VERT, 1);     
//		SetScrollRange(SB_VERT, 0, 0, FALSE);
//		EnableScrollBarCtrl(SB_VERT, 0);
		
		}	// end "!(dwStyle & WS_VSCROLL)"
		
	else	// (dwStyle & WS_VSCROLL)
		{	

		GetScrollInfo (SB_VERT, &scrollInfo, SIF_RANGE);
		maxRange = scrollInfo.nMax;
			
		maxRange = (int)((maxRange + magnification + .5)/magnification);
	   maxRange = (int)(maxRange * magnification);

		scrollInfo.nMax = maxRange;
		SetScrollInfo (SB_VERT, &scrollInfo, FALSE);
		
		}	// end "else (dwStyle & WS_VSCROLL)"  
	
//	GetScrollRange(SB_VERT, &minRange, &maxRange);  
		
			// Turn off lock out of recursive calls to OnSize.
		
	m_withinOnSizeFlag = FALSE;               
			
}	// end "UpdateScrollRange"



void CMImageView::UpdateScrolls(
				double				newMagnification)

{  
	SCROLLINFO			scrollInfo;

	Boolean				clearWindowFlag = FALSE;
	
	double            halfLogicalSize,
							columnLogicalCenter,
							lineLogicalCenter,
							oldMagnification;
							
	CPoint 				scrollOffset;
	
	Rect					offScreenRectangle;
	
	RECT					selRect,
							windowRect;
	
	SIZE					lineIncrement,
							pageIncrement;

	UInt32				height,
							logicalOffset,
							newRectHeight,
							newRectWidth,
							oldRectHeight,
							oldRectWidth,
							step,
							width;
							
	UInt16				offscreenHeight,
							offscreenWidth;
							
	SInt16 				titleHeight;
							                     
   
   		// Get the current size of the client portion of the window.  I.E.
   		// without scroll bars. 
   		
	titleHeight = GetTitleHeight();
   		                      
	GetClientRect( &windowRect );
	oldRectWidth = (UInt32)(windowRect.right - windowRect.left);
	oldRectHeight = 
			(UInt32)(windowRect.bottom - windowRect.top - titleHeight);
	 
	 		// Update Scroll size parameters.
	
	step = MAX(10, (UInt32)(newMagnification+.95));
   step = NormalizeScrollOffset(newMagnification, step, 0);
   lineIncrement.cx = (int)MAX(1, step);
   lineIncrement.cy = lineIncrement.cx;
   
   step = (UInt16)(oldRectWidth * .75);
   step = NormalizeScrollOffset(newMagnification, step, 0);
   pageIncrement.cx = (int)MAX(1, step);
   
   
   step = (UInt16)(oldRectHeight * .75);
   step = NormalizeScrollOffset(newMagnification, step, 0);
   pageIncrement.cy = (int)MAX(1, step);      
		
	m_displayMultiCPtr->GetOffscreenDimensions (
											&offscreenHeight,
											&offscreenWidth);
											
	width = (UInt32)((double)offscreenWidth * newMagnification + .5);
	height = (UInt32)((double)offscreenHeight * newMagnification + .5);
	
	m_callUpdateScrollRangeFlag = FALSE;
                                        
	SetScrollSizes( MM_TEXT, 
							CSize( (unsigned int)width, 
										(unsigned int)(height+titleHeight)), 
							pageIncrement, 
							lineIncrement);     
	m_callUpdateScrollRangeFlag = TRUE;              
	
			// Force scroll range to be a multiple of the pixel size or
			// force to zero if no scroll present.
	
	UpdateScrollRange (newMagnification);
   
   		// Now get the new size of the client portion of the window.  It might
   		// have changed if scroll bars are now needed. 
   		                      
	GetClientRect( &windowRect );
	newRectWidth = (UInt32)(windowRect.right - windowRect.left);
	newRectHeight = 
			(UInt32)(windowRect.bottom - windowRect.top - titleHeight);
			
			// Update the scroll position so that the specified portion
			// of the image stays in the center. The specified portioin
			// will be the selected area if available or the current
			// center of the image.                            
	                				                                                      
	oldMagnification = m_displayMultiCPtr->GetMagnification();
	
	Boolean selectionExistsFlag = GetSelectionOffscreenRectangle (
																	this, &offScreenRectangle);

	if (selectionExistsFlag) // && newMagnification > oldMagnification)
		{
				// A selection exists and the user is zooming into the window.
				// Remember that the offscreen rectangle is 0 based. The line-
				// column selection is 1 based.
				
		lineLogicalCenter = newMagnification *
				((double)offScreenRectangle.top + offScreenRectangle.bottom + 1)/2.;
										
		columnLogicalCenter = newMagnification *
				((double)offScreenRectangle.left + offScreenRectangle.right + 1)/2.;
										
		}	// end "if (selectionExistsFlag && ...)" 
		
	else	//	if (!selectionExistsFlag || ...																				
		{
				// No selection rectangle has been made or the user is zooming		
				// out.      
			
		GetScrollInfo (SB_HORZ, &scrollInfo, SIF_POS);
		scrollOffset.x = scrollInfo.nPos;
		GetScrollInfo (SB_VERT, &scrollInfo, SIF_POS);
		scrollOffset.y = scrollInfo.nPos;
		
		if (offscreenHeight * oldMagnification < oldRectHeight) 
			lineLogicalCenter = (double)height/2.;
			
		else	// offscreenHeight*oldMagnification >= oldRectHeight
			{ 	
			lineLogicalCenter = (double)oldRectHeight/2. + scrollOffset.y; 		
			lineLogicalCenter *= newMagnification/oldMagnification;
			
			}	// end "else offscreenHeight*oldMagnification >= oldRectHeight" 
		
		if (offscreenWidth * oldMagnification < oldRectWidth) 
			columnLogicalCenter = (double)width/2.;
			
		else	// offscreenWidth*oldMagnification >= oldRectWidth
			{ 	                                               
			columnLogicalCenter = (double)oldRectWidth/2. + scrollOffset.x;		
			columnLogicalCenter *= newMagnification/oldMagnification; 
			
			}	// end "else offscreenWidth*oldMagnification >= oldRectWidth"
							
		}	// end "else !selectionExistsFlag || ..." 
	
			// Get new scroll position for the x direction. 
	
	halfLogicalSize = ((double)newRectWidth) / 2.;
																		
	logicalOffset = (UInt32)MAX(0, columnLogicalCenter - halfLogicalSize);

	if (width < newRectWidth)
		logicalOffset = 0;

	if (logicalOffset != 0)
		logicalOffset = NormalizeScrollOffset(newMagnification, logicalOffset, 0);

	scrollOffset.x = (int)logicalOffset; 
	
			// Get new scroll position for the y direction.
	
	halfLogicalSize = ((double)newRectHeight) / 2.;
																		
	logicalOffset = (UInt32)MAX(0, lineLogicalCenter - halfLogicalSize);

	if (height < newRectHeight)
		logicalOffset = 0;

	if (logicalOffset != 0)
		logicalOffset = NormalizeScrollOffset(newMagnification, logicalOffset, 0);

	scrollOffset.y = (int)logicalOffset;
	
			// Set new scroll postions.
			
	GetScrollInfo (SB_HORZ, &scrollInfo);
	scrollInfo.fMask = SIF_POS;
	scrollInfo.nPos = scrollOffset.x;
	SetScrollInfo (SB_HORZ, &scrollInfo, TRUE);
		
	GetScrollInfo (SB_VERT, &scrollInfo);
	scrollInfo.fMask = SIF_POS;
	scrollInfo.nPos = scrollOffset.y;
	SetScrollInfo (SB_VERT, &scrollInfo, TRUE);
	
			// Set new magnification    
		
	m_displayMultiCPtr->SetMagnification(newMagnification);
	
	UpdateOffscreenMapOrigin ();
	
			// Window does not have to be cleared if new magnification is
			// larger than old magnification.
	
	if (newMagnification >= oldMagnification)
		{
		clearWindowFlag = FALSE;
		                                       
		if (titleHeight > 0)
			{ 
			selRect = windowRect;       
			selRect.top = 0;
			selRect.bottom = titleHeight;                        
			InvalidateRect(&selRect, TRUE);
			
			}	// end "if (GetTitleHeight(this) > 0)"
			
		}	// end "if (newMagnification >= oldMagnification)"
		
	else	// newMagnification < oldMagnification
		{
		LongPoint	newWindowPoint,	
						offScreenPoint;
		
		Handle windowInfoHandle = GetWindowInfoHandle(this);
		
		offScreenPoint.v = offscreenHeight;
		offScreenPoint.h = offscreenWidth;                           
		
		ConvertOffScreenPointToWinPoint(
							windowInfoHandle, &offScreenPoint, &newWindowPoint);
							
		if (newWindowPoint.h < windowRect.right)
			{
			selRect = windowRect;
			selRect.left = (int)newWindowPoint.h;
			selRect.bottom = MIN(selRect.bottom, (int)newWindowPoint.v);
			InvalidateRect(&selRect, TRUE);
			
			}	// end "if (newWindowPoint.h < imageRect.right)" 
							
		if (newWindowPoint.v < windowRect.bottom)
			{
			selRect = windowRect;
			selRect.top = (int)newWindowPoint.v;                        
			InvalidateRect(&selRect, TRUE);
			
			}	// end "if (newWindowPoint.v < imageRect.bottom)" 
		
		}	// end "else newMagnification < oldMagnification"
		
	Invalidate(clearWindowFlag);                           
			
}	// end "UpdateScrolls"


 
void CMImageView::UpdateSelectionCoordinates (void)
				
{   
	CMImageDoc* imageDocCPtr = GetDocument();
	
	if ( imageDocCPtr->GetDisplayCoordinatesFlag() )
		(imageDocCPtr->GetImageFrameCPtr())->UpdateSelectionCoordinates();
	
}	// end "UpdateSelectionCoordinates"

 

void CMImageView::ZoomIn (void)
				
{   			
	double		magnification,
					maxMagnification,
					inverse,
					step;   
					
					                 
	if ( ((CMultiSpecApp*)AfxGetApp())->GetZoomCode() == 0 )
																			return;
								                                                      
	magnification = m_displayMultiCPtr->GetMagnification();
					
	maxMagnification = m_displayMultiCPtr->GetMaxMagnification();
						
	if (magnification < maxMagnification)
		{  
		inverse = 1./magnification;
		            
		step = 0.1;
		
		if (!m_ctlKeyDownFlag)
			{       
			step = 1.0;   
		
			if (magnification > 1)
				magnification = (double)(UInt16)(magnification + .5);
				
			if (inverse > 1)
				inverse = (double)(UInt16)(inverse + .5);
				
			}	// end "if (!m_ctlKeyDownFlag)"
			
		if (magnification >= 1.0)
			magnification += step;
			
		else	// magnification < 1.0
			magnification = 1./(inverse - step); 
			                                                    
		magnification = MIN(magnification, maxMagnification);
		
		UpdateScrolls(magnification); 

		UpdateScaleInformation (gActiveImageWindowInfoH);
		
		}	// end "if (magnification < maxMagnification)"
	
}	// end "ZoomIn"


void CMImageView::ZoomOut(void)
				
{  
	double		magnification,
					inverse,
					step;  
					
					                 
	if ( ((CMultiSpecApp*)AfxGetApp())->GetZoomCode() == 0)
																			return; 
	                				                                                      
	magnification = m_displayMultiCPtr->GetMagnification();
						
	if (magnification > gMinMagnification)
		{  
		inverse = 1./magnification; 
		            
		step = 0.1;
		
		if (!m_ctlKeyDownFlag)
			{       
			step = 1.0;   
		
			if (magnification > 1)
				magnification = (double)(UInt16)(magnification + .5);
				
			if (inverse > 1)
				inverse = (double)(UInt16)(inverse + .5);
				
			}	// end "if (!m_ctlKeyDownFlag)"
			
		if (magnification > 1.)
			magnification -= step;
			
		else	// magnification <= 1.
			magnification = 1./(inverse + step);
			
		magnification = MAX(magnification, gMinMagnification);
		
		UpdateScrolls(magnification); 

		UpdateScaleInformation (gActiveImageWindowInfoH);
		
		}	// end "if (magnification < gMaxMagnification)"                                
	
}	// end "ZoomOut"



  
#ifdef _DEBUG  // debug version in WImagevView.cpp

/////////////////////////////////////////////////////////////////////////////
// CMImageView diagnostics

void CMImageView::AssertValid() const

{
	CScrollView::AssertValid();
	
}



void CMImageView::Dump(CDumpContext& dc) const

{
	CScrollView::Dump(dc);
	
}



CMImageDoc* CMImageView::GetDocument() // non-debug version is inline

{
	if (this != NULL)
		{
		ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMImageDoc)));
		return (CMImageDoc*)m_pDocument;

		}	// end "if (this != NULL)"

	else	// this == NULL
		return (NULL);
	
}



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetImageWindowCPtr
//
//	Software purpose:	The purpose of this routine is to return the image window class
//							pointer member of the class.
//							Note that the non-debug version is inline
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
//	Coded By:			Larry L. Biehl			Date: 08/01/1995
//	Revised By:			Larry L. Biehl			Date: 08/01/1995	

CMImageWindow* CMImageView::GetImageWindowCPtr () 

{  
	if (this == NULL)
		return (NULL);
		
	else	// this != NULL                                                                  
		return (CMImageWindow*)m_imageWindowCPtr;
}  	   
#endif	// _DEBUG. debug version in WImagevView.cpp
