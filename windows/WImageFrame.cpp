// WImageFrame.cpp : implementation file
//
// Revised by Larry Biehl on 08/30/2018
                    
#include "SMultiSpec.h"

#include "CDisplay.h" 
#include "CImageWindow.h"

#include "WMultiSpec.h"   
#include "WImageFrame.h" 
#include "WImageView.h"
#include "WGraphView.h"
#include "WImageDoc.h"
#include "WMainFrame.h"
#include "WLegendView.h"

//#include	"SExtGlob.h" 

extern UInt16							gDisplayBitsPerPixel; 

 

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif	// _DEBUG  
   
          
									
Boolean				CMImageFrame::s_forcePaletteBackgroundFlag = FALSE;



/////////////////////////////////////////////////////////////////////////////
// CMImageFrame

IMPLEMENT_DYNCREATE(CMImageFrame, CMDIChildWnd) 



CMImageFrame::CMImageFrame()
{
	m_imageLegendViewCPtr = NULL;
	m_imageViewCPtr = NULL;
	m_imageFrameActiveFlag = FALSE;
	m_forcePaletteBackgroundUpdateFlag = FALSE;  
	
			// Get the pixel size for the display.
		
	m_displayPixelSize = 8;                                             
	
	CDC				pDC;	
	if ( pDC.CreateIC((LPCTSTR)_T("DISPLAY"), NULL, NULL, NULL) )
		m_displayPixelSize = pDC.GetDeviceCaps(BITSPIXEL);     

}



CMImageFrame::~CMImageFrame()
{
}


BEGIN_MESSAGE_MAP(CMImageFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CMImageFrame)
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_GETMINMAXINFO()
	ON_WM_CHAR()                               
	ON_UPDATE_COMMAND_UI(ID_MAGNIFICATION, OnUpdateMagnification)
	ON_COMMAND(ID_MAGNIFICATION, OnMagnification)    
	ON_UPDATE_COMMAND_UI(ID_ZOOM_IN, OnUpdateZoomIn)
	ON_COMMAND(ID_ZOOM_IN, OnZoomIn)                  
	ON_UPDATE_COMMAND_UI(ID_ZOOM_OUT, OnUpdateZoomOut)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_ZOOM, OnUpdateZoomIndicator)  
	ON_COMMAND(ID_ZOOM_OUT, OnZoomOut)
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_EDIT_CLEAR_SELECT_RECTANGLE, OnClearSelectionRectangle)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR_SELECT_RECTANGLE, OnUpdateClearSelectionRectangle)
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdateFilePrint)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, OnUpdateFilePrintPreview)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECTION_RECTANGLE, OnUpdateEditSelectionRectangle)
	ON_COMMAND(ID_EDIT_SELECTION_RECTANGLE, OnEditSelectionRectangle)
	ON_WM_CLOSE()
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE, OnUpdateFileImageClose)
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_UPDATE_COMMAND_UI(ID_OVERLAY, OnUpdateOverlay)
	ON_COMMAND(ID_OVERLAY, OnOverlay)
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
	// Standard printing commands 
END_MESSAGE_MAP() 


void 
CMImageFrame::ActivateImageWindowItems(
				Boolean 			activateFlag, 
				Boolean			changeWindowFlag)
				
{ 	
	gNewSelectedWindowFlag = FALSE; 
		
	if ( (activateFlag != m_imageFrameActiveFlag) &&
															m_imageLegendViewCPtr != NULL )
		{
		m_forcePaletteBackgroundUpdateFlag = TRUE;
		
				// If this image window is going inactive indicate that a palette
				// change must be force if one does not occur so that the
				// thematic palette will be realized in the background. It may
				// not be if the new active image window is of multispectral type
				// and the colors do not change.
				
		if (!activateFlag)
			s_forcePaletteBackgroundFlag = TRUE;
		
		}		// end "if ( (activateFlag != m_imageFrameActiveFlag) && ..."														
															
	if (activateFlag && changeWindowFlag)
		{
		if (!m_imageFrameActiveFlag)
			{                                                                                   
			CMainFrame* pAppFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
			pAppFrame->SendMessage(WM_QUERYNEWPALETTE, NULL, NULL); 
			
			}		// end "if (!m_imageFrameActiveFlag)"   
		                                      
		m_imageFrameActiveFlag = TRUE;
		m_imageLegendViewCPtr->GetLegendListCPtr()->SetLegendListActiveFlag (TRUE);
		
		if (gSelectionGraphViewCPtr != NULL)
			gSelectionGraphViewCPtr->SetCheckIOMemoryFlag (TRUE);
			
		Handle windowInfoHandle = GetWindowInfoHandle(m_imageViewCPtr);
		WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
														windowInfoHandle, kNoLock, kNoMoveHi);
		if (windowInfoPtr->projectWindowFlag)
			{
			gProjectSelectionWindow = m_imageViewCPtr;
			
			LoadNewFieldListBox ();
			
			}		// end "if (windowInfoPtr->projectWindowFlag)"
			
		ShowGraphSelection (); 
		
				// This flag is used in selections to try to differentiate between a user
				// making a selection or just selecting the image to work on.
		
		gNewSelectedWindowFlag = TRUE; 
		
		}		// end "if (bActivate && changeWindowFlag)"
		
	if (!activateFlag && changeWindowFlag)
		{
		m_imageFrameActiveFlag = FALSE;                        
		m_imageLegendViewCPtr->GetLegendListCPtr()->SetLegendListActiveFlag (FALSE);
		
		}		// end "if (!activateFlag && changeWindowFlag)"
	
			// Make sure that the flag indicating that the control key is down
			// is set to FALSE. It may be TRUE if a menu command was processed
			// using quick keys.
			
	m_imageViewCPtr->SetControlKeyFlag(FALSE);
	
}		// end "ActivateImageWindowItems" 



void 
CMImageFrame::ChangeClassGroupDisplay(
				SInt16		newClassGroupCode,
				Boolean		updateClassGroupListFlag)
{                                 
	SInt16 currentClassGroupCode = m_imageViewCPtr->GetClassGroupCode ();
	
	if (currentClassGroupCode != 0 &&
										newClassGroupCode != currentClassGroupCode)
		{                               
		UpdateActiveImageLegend (newClassGroupCode, kCallCreatePalette); 
		
		if (updateClassGroupListFlag)
			{
			m_imageLegendViewCPtr->UpdateClassGroupComboList (newClassGroupCode);             
			
			}		// end "if (updateClassGroupListFlag)"
		
		}		// end "if (classGroupCode != m_classGroupCode + 1)"
	
}		// end "ChangeClassGroupDisplay"



LRESULT 
CMImageFrame::DoRealize(
				Boolean				backgroundFlag,
				CWnd*					hWnd)

{                                            
	if ( !m_imageViewCPtr->CheckIfOffscreenImageExists()	)									
																							return 0L;
																								
	Handle displaySpecsHandle = m_imageViewCPtr->GetDisplaySpecsHandle();
	DisplaySpecsPtr displaySpecsPtr = 
		(DisplaySpecsPtr)GetHandlePointer(displaySpecsHandle, kNoLock, kNoMoveHi);
	                                              
	CMPalette* newPalette = NULL;
	
	if (displaySpecsPtr != NULL)
		{
		if (backgroundFlag && (displaySpecsPtr->backgroundPaletteObject != NULL))
			newPalette = displaySpecsPtr->backgroundPaletteObject;
			
		else		// !backgroundFlag || ...	
			newPalette = displaySpecsPtr->paletteObject; 
			
		if (!backgroundFlag && s_forcePaletteBackgroundFlag)
			m_forcePaletteBackgroundUpdateFlag = TRUE;
			
		s_forcePaletteBackgroundFlag = FALSE;
			
		}		// end "if (displaySpecsPtr != NULL)"
		
	if (newPalette != NULL)
		{
		CMainFrame* pAppFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
		ASSERT(pAppFrame->IsKindOf(RUNTIME_CLASS( CMainFrame )));

		CClientDC appDC(pAppFrame);
		
				// All views but one should be a background palette.
				// lParam contains a 1 if this is called from "QueryNewPalette"
				// which is the only case to realize the palette in the foreground.

		CPalette* oldPalette = appDC.SelectPalette(newPalette, backgroundFlag); 
/*					             
		sprintf(	(char*)&gTextString,
					" %d. OPal = %4x:%4x. NPal = %4x:%4x.%s",
					backgroundFlag,
					oldPalette,
					newPalette,
					gEndOfLine);
		OutputString ( NULL, 
						(char*)&gTextString, 
						0, 
						1, 
						TRUE); 
					
		UInt16 numberEntries = newPalette->GetNumberPaletteEntries();
		
		sprintf(	(char*)&gTextString,
					"New Palette Entries = %d. %s",
					numberEntries, 
					gEndOfLine);
		OutputString ( NULL, 
						(char*)&gTextString, 
						0, 
						1, 
						TRUE);
*/		
		if (oldPalette != NULL)
			{
			UINT nColorsChanged;
			
//			#ifndef _WIN32
//				nColorsChanged = appDC.RealizePalette();
//			#else
				HDC hdc = appDC.GetSafeHdc();
				nColorsChanged = RealizePalette(hdc);
//			#endif
/*					             
			sprintf(	(char*)&gTextString,
						"Colors changed = %d. %s",
						nColorsChanged,
						gEndOfLine);
			OutputString ( NULL, 
							(char*)&gTextString, 
							0, 
							1, 
							TRUE);
*/			                                 
			if (!backgroundFlag && m_forcePaletteBackgroundUpdateFlag)
				{                                                             
//				pAppFrame->SendMessage(WM_PALETTECHANGED, (WPARAM)hWnd);
				pAppFrame->DoPaletteChanged (hWnd);
				
				}		// end "if (!backgroundFlag && ...)"
							
			appDC.SelectPalette(oldPalette, TRUE);			

//			#ifndef _WIN32
//				appDC.RealizePalette();
//			#else
				RealizePalette(hdc);
//			#endif

			
			if (nColorsChanged > 0 || m_forcePaletteBackgroundUpdateFlag ||
															gDisplayBitsPerPixel > 8)
				{         
				CMImageDoc* pDoc = GetDocument();
				pDoc->UpdateAllViews(NULL);
				m_forcePaletteBackgroundUpdateFlag = FALSE; 
				
				}		// end "if (nColorsChanged > 0)"
				
			}		// end "if (oldPalette != NULL)"
			
		else		// oldPalette == NULL 
			TRACE0("\tSelectPalette failed in CMImageView::OnDoRealize\n"); 
			
		}		// end "if (newPalette != NULL)"
			
	else		// newPalette == NULL
		TRACE0("\tSelectPalette failed in CMImageView::OnDoRealize\n"); 

	return 0L;
	
}		// end "DoRealize"



void CMImageFrame::GetCoordinateViewComboText(
				char*									comboItemStringPtr,
				UInt16								itemNumber)

{  
	CComboBox* comboBoxPtr = 
				(CComboBox*)m_coordinatesBar.GetDlgItem(itemNumber);

	comboItemStringPtr[0] = (UInt8)comboBoxPtr->GetLBText (
													comboBoxPtr->GetCurSel(), 
													(LPTSTR)gWideTextString); 

	wcstombs (&comboItemStringPtr[1], gWideTextString, 32);
//#	else
//		comboItemStringPtr[0] = (UInt8)comboBoxPtr->GetLBText (
//													comboBoxPtr->GetCurSel(), 
//													(LPTSTR)&comboItemStringPtr[1]);
//#	endif
	
}		// end "GetCoordinateViewComboText"	
 

Boolean
CMImageFrame::GetActiveWindowFlag (void)

{ 
	if (this != NULL)
		return (m_imageFrameActiveFlag); 

	else		// this == NULL
		return (FALSE);

}		// end "GetActiveWindowFlag"                         
	


 
void
CMImageFrame::InitialUpdate ( 
				CSize     		imageViewSize)

{  
	CRect						rectFrame;

	                                 
	GetWindowRect (rectFrame);
	CSize size = rectFrame.Size();
	
			// This represents just the size allowed for the image. This was
			// set by the ResizeParentToFit call in CMImageView::OnInitialUpdate
			// routine. Add the amount needed for the legend and the split window bar.
			
	size.cx += 170;                     
	SetWindowPos(NULL, 0, 0, size.cx, size.cy,
										SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
		                                  
	m_wndSplitter.SetColumnInfo (0, 170, 0);
	m_wndSplitter.SetColumnInfo (1, imageViewSize.cx, 0);
	m_wndSplitter.SetRowInfo (0, imageViewSize.cy, 0); 
	
	m_wndSplitter.RecalcLayout(); 
	
	Handle windowInfoHandle = GetWindowInfoHandle (m_imageViewCPtr); 
	
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer(
												windowInfoHandle,
												kNoLock,
												kNoMoveHi);
	
	if (windowInfoPtr != NULL)
		{							
		windowInfoPtr->showLegend = TRUE; 
		windowInfoPtr->legendWidth = 170;
		
		}		// end "if (windowInfoPtr != NULL)"    
		
	m_imageLegendViewCPtr->SetLegendWidthSetFlag();
				
	m_imageLegendViewCPtr->AdjustLegendListLength();

}		// end "InitialUpdate" 	



void CMImageFrame::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Add your message handler code here and/or call default
	
	CMDIChildWnd::OnChar(nChar, nRepCnt, nFlags);
	
}



void CMImageFrame::OnClose()
{                                                                 
	Handle						windowInfoHandle;
	Boolean						closeFlag = TRUE;
	
	
	if (gProcessorCode != -1)
		{
		windowInfoHandle = GetWindowInfoHandle(m_imageViewCPtr);
		                          
		closeFlag = !SaveIfWindowChanged (windowInfoHandle, FALSE);
		
		}		// end "if (gProcessorCode != -1)"
	
	if (closeFlag || gProcessorCode == -1)
		CMDIChildWnd::OnClose();
	
}	// end "OnClose"
   



int 
CMImageFrame::OnCreate(
				LPCREATESTRUCT 			lpCreateStruct)

{ 
	SInt16		returnValue;
	
	
	if ( (returnValue = CMDIChildWnd::OnCreate(lpCreateStruct)) != -1)
		{
				// Add Dialog Bar to the view for the coordinates
				
		if ( !m_coordinatesBar.Create(
										this, IDD_Coordinates, CBRS_TOP, IDD_Coordinates) )
			returnValue = -1;
			
		else	// m_coordinatesBar.Create(...
			{
					// Bar has been set up. Hide for now.
		
			m_coordinatesBar.ShowWindow(SW_HIDE);              
			
			}		// end "" 
		
		}		// end "if ( (returnValue = ..."
	
	return returnValue;
	
}	// end "OnCreate"



BOOL CMImageFrame::OnCreateClient(
				LPCREATESTRUCT 		lpcs, 
				CCreateContext* 		pContext)
				
{  
	Boolean		returnFlag = FALSE;
	
	
//	if (gGetFileImageType == kThematicImageType)
//		{ 
		if (m_wndSplitter.CreateStatic(this,
										1, 
										2) )		// TODO: adjust the number of rows, columns 
										
//		if (m_wndSplitter.Create(this,
//										1, 2,		// TODO: adjust the number of rows, columns
//										CSize(20, 20),	// TODO: adjust the minimum pane size
//										pContext) )
			{                                                                 
			returnFlag = TRUE;
			
			SetSplitterParameters (gGetFileImageType);
			
			((CMImageDoc*)pContext->m_pCurrentDoc)->SetImageFrameCPtr(this);
			
			if ( !m_wndSplitter.CreateView(0,
												0,
												RUNTIME_CLASS(CMLegendView),
												CSize(0,0),
												pContext) )                                                                      
				returnFlag = FALSE; 
			
			if (returnFlag) 
				{   
				if ( !m_wndSplitter.CreateView(0,
														1,
														pContext->m_pNewViewClass,
														CSize(100,100),
														pContext) )                                              
					returnFlag = FALSE;                               
					
				}		// end "if (returnFlag)"
			
			if (returnFlag)
				{	       
						// Store the two views for the frame;
						                                                 
				m_imageLegendViewCPtr = (CMLegendView*)m_wndSplitter.GetPane(0,0);
				m_imageViewCPtr = (CMImageView*)m_wndSplitter.GetPane(0,1);
				
						// Store the image view class in the legend view class
					                                                                       
				m_imageLegendViewCPtr->SetImageView (m_imageViewCPtr);
				            
//				SetActiveView ((CView*)m_wndSplitter.GetPane(0,1));

				}		// end "if (returnFlag)"
			
			}		// end "if ( m_wndSplitter.Create(this, ..."
			
//		}		// end "if (gGetFileImageType == kThematicType)"
		
//	else		// gGetFileImageType != kThematicType
//		{
//		if ( CMDIChildWnd::OnCreateClient(lpcs, pContext) )
//			{		    
//			((CMImageDoc*)pContext->m_pCurrentDoc)->SetImageFrameCPtr(this);                                                       
//			returnFlag = TRUE; 
			
//			}		// end "if ( CMDIChildWnd::OnCreateClient(lpcs, pContext) )"
			
//		}		// end "else gGetFileImageType != kThematicType" 
		
	return (returnFlag);
	
}		// end "OnCreateClient" 


/////////////////////////////////////////////////////////////////////////////
// CMImageFrame message handlers


void 
CMImageFrame::OnGetMinMaxInfo(
				MINMAXINFO FAR* 			lpMMI)

{  
/*	double				magnification;                            
							
	UInt16				height,
							offscreenHeight,
							offscreenWidth,
							width; 
							                    
	
	if (gActiveImageViewCPtr != NULL)
		{	
		gActiveImageViewCPtr->m_displayMultiCPtr->
								GetOffscreenDimensions (&offscreenHeight,
																&offscreenWidth);
											
		magnification = 
			gActiveImageViewCPtr->m_displayMultiCPtr->GetMagnification();
											
		width = (UInt16)((double)offscreenWidth * magnification + .5);
		height = (UInt16)((double)offscreenHeight * magnification + .5);
	
		lpMMI->ptMaxSize.x = width + 20;
		lpMMI->ptMaxSize.y = height + 28;
		lpMMI->ptMaxPosition.x = 20;
		lpMMI->ptMaxPosition.y = 20;
		
		}		// end "if (gActiveImageWindowCPtr)"
*/							
	CMDIChildWnd::OnGetMinMaxInfo(lpMMI); 
	
}		// end "OnGetMinMaxInfo"



BOOL CMImageFrame::OnSetCursor (
				CWnd* 			pWnd, 
				UINT 				nHitTest, 
				UINT 				message)   
				
{                       
	if (gPresentCursor == kWait || gPresentCursor == kSpin)
		{
				// Wait cursor in affect. Processing underway.
				// Restart the wait cursor in case in was changed to pointer
				// before entering the image frame.
		
		AfxGetApp ()->DoWaitCursor (0);
																			return (TRUE);
		
		}	// end "if (gPresentCursor == kWait || gPresentCursor == kSpin)"
	
	if (gPresentCursor != kArrow)
		{
		if (gActiveImageViewCPtr != NULL)                
			gActiveImageViewCPtr->UpdateCursorCoordinates();
		                                                                
		gPresentCursor = kArrow; 	// Non image window cursor
		
		}	// end "if (gPresentCursor != kArrow)"
	
	return CMDIChildWnd::OnSetCursor (pWnd, nHitTest, message);
	
}	// end "OnSetCursor"



void 
CMImageFrame::OnMagnification(void) 
				
{  	
	m_imageViewCPtr->UpdateScrolls(1.0); 

	::UpdateScaleInformation (gActiveImageWindowInfoH);
	
}		// end "OnMagnification"    

 
 
void CMImageFrame::OnUpdateZoomIndicator(
				CCmdUI* 		pCmdUI) 
				
{  
	USES_CONVERSION;

	if (m_imageViewCPtr->CheckIfOffscreenImageExists())
		{                                       
		CMDisplay* displayMultiCPtr = m_imageViewCPtr->m_displayMultiCPtr;
		double magnification = displayMultiCPtr->GetMagnification();
							
		SInt16 fieldPrecision = 1;
		if (magnification < 1)
			fieldPrecision = 3;
							          
		sprintf ((char*)&gTextString, "Zoom  = x %4.*f", fieldPrecision, magnification);
		
		if (fieldPrecision == 3)
			sprintf ((char*)&gTextString[9], "%s", (char*)&gTextString[10]);
			                                                                                                                                                                   
		pCmdUI->Enable(TRUE);
		pCmdUI->SetText((LPCTSTR)A2T((char*)gTextString));
							
		}		// end "if ( ...->CheckIfOffscreenImageExists() )"
		
		
	else		//	!CheckIfOffscreenImageExists()                                                                                                                                                  
		pCmdUI->Enable(FALSE);
	
}		// end "OnUpdateZoomIndicator"   

  
  
void 
CMImageFrame::OnZoomIn(void) 
				
{   			
	m_imageViewCPtr->ZoomIn(); 
	
}		// end "OnZoomIn"



void 
CMImageFrame::OnZoomOut(void) 
				
{                            
	m_imageViewCPtr->ZoomOut();                               
	
}		// end "OnZoomOut"

   

void 
CMImageFrame::OnSetFocus(CWnd* pOldWnd)

{    
			// The active image window class pointer will only change when
			// there is no active processing funtion running.
			
	UpdateActiveImageWindowInfo ();
		
	CMDIChildWnd::OnSetFocus(pOldWnd);
	
}		// end "OnSetFocus"


afx_msg void 
CMImageFrame::OnSysCommand (
				UINT 			nID,
				LPARAM		lParam)
                      
{                     
	if (gProcessorCode != kListDataProcessor || 
								(nID != SC_CLOSE && nID != SC_MAXIMIZE))
		CFrameWnd::OnSysCommand (nID, lParam);

}	// end "OnSysCommand"



void CMImageFrame::OnUpdateClearSelectionRectangle(CCmdUI* pCmdUI)
{  
	SInt16		selectionTypeCode;
	Boolean 		enableFlag;
	
	
			// Update the menu text.
			
	selectionTypeCode = GetSelectionTypeCode (gActiveImageViewCPtr);
	UpdateEditClearSelection (pCmdUI, selectionTypeCode);
	
			// Determine whether menu item is enabled.
			
	enableFlag = FALSE;
	if (gActiveImageViewCPtr != NULL)
		enableFlag = (selectionTypeCode != 0);
		
	pCmdUI->Enable ( enableFlag );
	
}		// end "OnUpdateClearSelectionRectangle"



void CMImageFrame::OnClearSelectionRectangle()
{                                              
	ClearSelectionArea (gActiveImageViewCPtr);
	
	ShowGraphSelection ();
	
}		// end "OnClearSelectionRectangle"

 

void CMImageFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)

{
	// Add your message handler code here and/or call default
	
	CMDIChildWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}



void CMImageFrame::OnEditSelectAll()
{                                                                        
	DoEditSelectAllImage (gActiveImageViewCPtr);
	
}		// end "OnEditSelectAll" 



void 
CMImageFrame::OnUpdateEditSelectAll(
				CCmdUI* 											pCmdUI)
				
{  
	UpdateEditImageSelectAll (pCmdUI);
	
	Boolean	rectangularSelectionFlag = GetRectangularSelectionFlag ();
		                                                                                                                                                      
	pCmdUI->Enable( gActiveImageViewCPtr->CheckIfOffscreenImageExists() &&
									rectangularSelectionFlag );
	
}		// end "OnUpdateEditSelectAll"



void CMImageFrame::OnUpdateFilePrint(CCmdUI* pCmdUI)
{                                                                                                                                                           
	Handle windowInfoHandle = GetActiveImageWindowInfoHandle (); 
	             
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer(
														windowInfoHandle,
														kLock,
														kNoMoveHi); 
														          
	Boolean enableFlag = UpdateFileImagePrint (pCmdUI, windowInfoPtr);
	 
	CheckAndUnlockHandle (windowInfoHandle);
	                                                                                              
	pCmdUI->Enable(enableFlag);
	
}



void CMImageFrame::OnFilePrint()
{
	((CMultiSpecApp*)AfxGetApp())->SetPrintOrientation (GetActiveWindowInfoHandle());

	gActiveImageViewCPtr->DoFilePrint();
	
}



void CMImageFrame::OnUpdateFilePrintPreview(CCmdUI* pCmdUI)
{  	                                         
	pCmdUI->Enable( gActiveImageViewCPtr->CheckIfOffscreenImageExists() );
	
}



void CMImageFrame::OnFilePrintPreview()
{                              
	((CMultiSpecApp*)AfxGetApp())->SetPrintOrientation (GetActiveWindowInfoHandle());

	gActiveImageViewCPtr->DoFilePrintPreview();
	
}



void 
CMImageFrame::OnEditCopy()

{                                              
	gActiveImageViewCPtr->DoEditCopy();
	
}    



void CMImageFrame::OnEditSelectionRectangle()
{
	EditSelectionDialog(kRectangleType, FALSE);
	
}		// end "OnEditSelectionRectangle"



void CMImageFrame::OnUpdateEditCopy(CCmdUI* pCmdUI)
{                                                                                                                                               
	Handle windowInfoHandle = GetActiveImageWindowInfoHandle (); 
	             
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer(
														windowInfoHandle,
														kLock,
														kNoMoveHi); 
														
	Boolean enableFlag = UpdateEditImageCopy (pCmdUI, windowInfoPtr); 
	 
	CheckAndUnlockHandle (windowInfoHandle);
	                                                                                     
	pCmdUI->Enable( enableFlag );
	
}



void CMImageFrame::OnUpdateEditSelectionRectangle(CCmdUI* pCmdUI)
{                                                                                       
	pCmdUI->Enable( GetRectangularSelectionFlag () );
	
}		// end "OnUpdateEditSelectionRectangle"



void CMImageFrame::OnUpdateFileImageClose(CCmdUI* pCmdUI)
{  
	UpdateFileImageClose(pCmdUI);

	pCmdUI->Enable (TRUE);
	
}		// end "OnUpdateFileImageClose"  



void 
CMImageFrame::OnUpdateMagnification(
				CCmdUI* 		pCmdUI) 
				
{  
	Boolean		enableFlag = FALSE;
	
	
	if ( m_imageViewCPtr->CheckIfOffscreenImageExists() )
		{                 
		CMDisplay* displayMultiCPtr = m_imageViewCPtr->m_displayMultiCPtr; 
		if (displayMultiCPtr->GetMagnification() != 1.0)
			enableFlag = TRUE;
			
		}		// end "if ( CheckIfOffscreenImageExists() )"
																						                                                                                                                                                   
	pCmdUI->Enable(enableFlag);
	
}		// end "OnUpdateMagnification"



void 
CMImageFrame::OnUpdateZoomIn(
				CCmdUI* 		pCmdUI) 
				
{               
	Boolean				enableFlag = FALSE;                            
	
	
	if ( m_imageViewCPtr->CheckIfOffscreenImageExists() )
		{
		CMDisplay* displayMultiCPtr = m_imageViewCPtr->m_displayMultiCPtr;                                               
		if ( displayMultiCPtr->GetMagnification() <
														displayMultiCPtr->GetMaxMagnification() )
			enableFlag = TRUE;
			
		}		// end "if ( CheckIfOffscreenImageExists() )"
		                                                                                                     
	pCmdUI->Enable(enableFlag);
	
}		// end "OnUpdateZoomIn" 



void 
CMImageFrame::OnUpdateZoomOut(
				CCmdUI* 		pCmdUI) 
				
{                                                  
	Boolean				enableFlag = FALSE;                                 
	
	
	if ( m_imageViewCPtr->CheckIfOffscreenImageExists() )
		{                                   
		CMDisplay* displayMultiCPtr = m_imageViewCPtr->m_displayMultiCPtr;            
		if ( displayMultiCPtr->GetMagnification() > gMinMagnification)
			enableFlag = TRUE;
			
		}		// end "if ( CheckIfOffscreenImageExists() )"
		                                                                                                     
	pCmdUI->Enable(enableFlag);
	
}		// end "OnUpdateZoomOut"



void 
CMImageFrame::OnViewCoordinatesBar(
				Boolean			displayCoordinatesFlag)
{ 
	RECT						coordinateRect,
								displayUnitsRect,
								linePromptRect;


			// Toggle the coordinates bar
	
	if (displayCoordinatesFlag)
		{
		m_coordinatesBar.ShowWindow(SW_SHOW); 
		
		SetUpCoordinateUnitsPopUpMenu (NULL, 
													gActiveImageWindowInfoH, 
													(CMDialog*)&m_coordinatesBar);

		m_coordinatesBar.m_displayUnitsCode =
								GetCoordinateViewUnits (gActiveImageWindowInfoH);

		m_coordinatesBar.m_displayUnitsListSelection =
					GetComboListSelection (&m_coordinatesBar, 
												IDC_DisplayUnitsCombo, 
												m_coordinatesBar.m_displayUnitsCode);

		DDX_CBIndex(m_coordinatesBar.m_dialogToPtr, 
							IDC_DisplayUnitsCombo, 
							m_coordinatesBar.m_displayUnitsListSelection);
													
		SetUpAreaUnitsPopUpMenu (NULL, 
											gActiveImageWindowInfoH, 
											(CMDialog*)&m_coordinatesBar);

		m_coordinatesBar.m_areaUnitsCode =
								GetCoordinateViewAreaUnits (gActiveImageWindowInfoH);

		m_coordinatesBar.m_areaUnitsListSelection =
					GetComboListSelection (&m_coordinatesBar, 
												IDC_AreaUnitsCombo, 
												m_coordinatesBar.m_areaUnitsCode);

		DDX_CBIndex(m_coordinatesBar.m_dialogToPtr, 
							IDC_AreaUnitsCombo, 
							m_coordinatesBar.m_areaUnitsListSelection);

		}		// end "if (displayCoordinatesFlag)"
		
	else		// !displayCoordinatesFlag 
		m_coordinatesBar.ShowWindow(SW_HIDE);
/*
	SCROLLINFO			scrollInfo;
	
	m_imageViewCPtr->GetScrollInfo (SB_VERT, &scrollInfo, SIF_RANGE);
	scrollInfo.nMax = scrollInfo.nMax - 33;
	m_imageViewCPtr->SetScrollInfo (SB_VERT, &scrollInfo, FALSE);
	m_imageViewCPtr->GetScrollInfo (SB_VERT, &scrollInfo, SIF_RANGE);
*/
	RecalcLayout();
/*
	m_imageViewCPtr->GetScrollInfo (SB_VERT, &scrollInfo, SIF_RANGE);
	scrollInfo.nMax = scrollInfo.nMax - 33;
	m_imageViewCPtr->SetScrollInfo (SB_VERT, &scrollInfo, FALSE);
	m_imageViewCPtr->GetScrollInfo (SB_VERT, &scrollInfo, SIF_RANGE);
*/
	if (displayCoordinatesFlag)
		{
		CComboBox* comboBoxPtr;

		m_coordinatesBar.GetWindowRect (&coordinateRect);

		comboBoxPtr = 
				(CComboBox*)m_coordinatesBar.GetDlgItem(IDC_DisplayUnitsCombo);
		comboBoxPtr->GetWindowRect (&displayUnitsRect);

		comboBoxPtr = 
				(CComboBox*)m_coordinatesBar.GetDlgItem(IDC_LineSymbol);
		comboBoxPtr->GetClientRect (&linePromptRect);

		SetCoordinateViewCursorStart (gActiveImageWindowInfoH, 
			displayUnitsRect.right - coordinateRect.left + linePromptRect.right + 8);
		
		m_coordinatesBar.UpdateCoordinateView(gActiveImageWindowInfoH);

		::UpdateScaleInformation (gActiveImageWindowInfoH);

		}		// end "if (displayCoordinatesFlag)"
	
}		// end "OnViewCoordinatesBar"


									
void CMImageFrame::SetImageViewCPtr(
				CMImageView*			imageViewCPtr)  
				
{               
	m_imageViewCPtr = imageViewCPtr;
	
}		// end "SetImageViewCPtr"



void CMImageFrame::SetLegendWidth(
				SInt16		newLegendWidth)
				
{  
	Handle					windowInfoHandle;

	int						currentWidth,
								minimumWidth;

	SInt16					windowType;
						
						                                      
			// If the input legend width is less than 0, then get the 
			// current legend width to use as the new legend width. If
			// the new legend width is less than the threshold then force
			// the width to be 0 and set show legend width to FALSE.
	
	if (newLegendWidth < 0 || m_imageLegendViewCPtr == NULL) 
																				return;
	
	windowInfoHandle = GetActiveImageWindowInfoHandle ();
	windowType = GetWindowType (windowInfoHandle);

			// Get the current settings.
																						
	m_wndSplitter.GetColumnInfo (0, currentWidth, minimumWidth);
	
	if (windowType == kThematicWindowType)
		{
		if (newLegendWidth < 7)
			{
				// Force the legend width to be 0;
				                                    
			newLegendWidth = 0;            
			
			}		// end "if (currentWidth < 7)"
		
		else if (newLegendWidth < 145)
			{
				// Force the legend width to be 145;
				                                        
			newLegendWidth = 145;
			
					// Force the current legend width to be different.
					
			currentWidth = 144;          
			
			}		// end "else if (newLegendWidth < 145)"

		}		// end "windowType == kThematicWindowType"

	else		// windowType != kThematicWindowType
				// Legend is not allowed for multispectral image windows.
		newLegendWidth = 0;
		
	if (newLegendWidth != currentWidth)
		{            
		m_wndSplitter.SetColumnInfo (0, newLegendWidth, 0);
		m_wndSplitter.RecalcLayout ();
		
		}		// end "if (newLegendWidth != currentWidth)" 
	
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer(
												windowInfoHandle,
												kNoLock,
												kNoMoveHi);
	
	if (windowInfoPtr != NULL)
		{							
		windowInfoPtr->showLegend = (newLegendWidth > 0); 
		windowInfoPtr->legendWidth = newLegendWidth;
		
		}		// end "if (windowInfoPtr != NULL)"
		                                                                       
//	CMLegendView* legendViewCPtr = (CMLegendView*)m_wndSplitter.GetPane(0,0);
	m_imageLegendViewCPtr->SetLegendWidthSetFlag();
	
}		// end "SetLegendWidth"   



void 
CMImageFrame::SetSplitterParameters(
				SInt16						windowType)
				
{  
			// Set the splitter parameters such that no splitter is
			// available for image type windows and a splitter is
			// available for thematic type windows.
	
	if (windowType == kImageWindowType)
		{
		// TTY changes
//#		if defined _WIN64
			m_wndSplitter.set_cxSplitter(1);
			m_wndSplitter.set_cxSplitterGap(1);
			m_wndSplitter.set_cxBorder(0);
//#		else
//			m_wndSplitter.m_cxSplitter = 1;
//			m_wndSplitter.m_cxSplitterGap = 1;
//			m_wndSplitter.m_cxBorder = 0;
//#		endif

		}		// end "if (windowType == kImageType)"

	else		// windowType == kThematicWindowType
		{
//#		if defined _AMD64_
			m_wndSplitter.set_cxSplitter(3 + 2 + 2);
			m_wndSplitter.set_cxSplitterGap(3 + 2 + 2);
			m_wndSplitter.set_cxBorder(2);
//#		else
//			m_wndSplitter.m_cxSplitter = 3 + 2 + 2;
//			m_wndSplitter.m_cxSplitterGap = 3 + 2 + 2;
//			m_wndSplitter.m_cxBorder = 2;
//#		endif

		}		// end "else windowType == kThematicWindowTyp"
	
}		// end "SetSplitterParameters"   

   

void 
CMImageFrame::UpdateActiveImageWindowInfo(void)

{    
			// The active image window class pointer will only change when
			// there is no active processing funtion running.
			
	if (gProcessorCode == 0 && gActiveImageViewCPtr != m_imageViewCPtr)
		{  		                    
		gActiveImageViewCPtr = m_imageViewCPtr;
		gActiveImageWindow = m_imageViewCPtr; 
				
		gActiveImageWindowInfoH = GetActiveImageWindowInfoHandle();
		gActiveImageFileInfoH = GetActiveImageFileInfoHandle(); 
		gActiveImageLayerInfoH = GetActiveImageLayerInfoHandle();
     	
     	gActiveLegendWidth = 0;
     	if (GetShowLegendFlag(gActiveImageWindowInfoH))
     		gActiveLegendWidth = GetLegendWidth (gActiveImageWindowInfoH);
		
		}		// end "if (gProcessorCode == 0 && ..."
	
}		// end "UpdateActiveImageWindowInfo"



void 
CMImageFrame::UpdateScaleInformation(
				double								scale,
				char*									scaleStringPtr)
{       
	USES_CONVERSION;

	m_coordinatesBar.SetDlgItemText(IDC_Scale, (LPCTSTR)A2T(scaleStringPtr));
	
	if (scale <= 0)
		m_coordinatesBar.GetDlgItem(IDC_ScalePrompt)->ShowWindow (SW_HIDE);

	else		// scale > 0
		m_coordinatesBar.GetDlgItem(IDC_ScalePrompt)->ShowWindow (SW_SHOW);
	
}		// end "UpdateScaleInformation"



void 
CMImageFrame::UpdateSelectionCoordinates(void)
{ 
	DoubleRect							coordinateRectangle;
	LongRect								lineColumnRectangle;
	
	SInt64								numberPixels;
	
	Handle								windowInfoHandle; 
	

	USES_CONVERSION;
                                             
			// Show the coordinates   
	
	if (GetSelectionCoordinates (m_imageViewCPtr, 
											&lineColumnRectangle, 
											&coordinateRectangle,
											&numberPixels) )
		{                                                             
		ShowDialogItem ((CMDialog*)&m_coordinatesBar, IDC_AreaUnitsCombo); 

		windowInfoHandle = GetWindowInfoHandle (m_imageViewCPtr);
		DrawSelectionCoordinates (windowInfoHandle,
											&lineColumnRectangle, 
											&coordinateRectangle,
											numberPixels);
		
		}		// end "if (GetSelectionCoordinates (m_imageViewCPtr,..." 
		
	else		// !GetSelectionCoordinates (&selectionRectangle) 
		{
		sprintf( (char*)gTextString, "");
		m_coordinatesBar.SetDlgItemText(IDC_SelectionLine, A2T((char*)gTextString));
		                                                              
		m_coordinatesBar.SetDlgItemText(IDC_SelectionColumn, A2T((char*)gTextString)); 
		                                                              
		m_coordinatesBar.SetDlgItemText(IDC_NumberPixelsPrompt, A2T((char*)gTextString)); 
		                                                              
		m_coordinatesBar.SetDlgItemText(IDC_NumberPixels, A2T((char*)gTextString));
		                                                              
		HideDialogItem ((CMDialog*)&m_coordinatesBar, IDC_AreaUnitsCombo); 
		
		}		// end "else !GetSelectionCoordinates (&selectionRectangle)"
	
}		// end "UpdateSelectionCoordinates"



void CMImageFrame::UpdateSelectionCoordinates(
				char*									lineValueStringPtr,
				char*									columnValueStringPtr)
{ 
	USES_CONVERSION;
      
	m_coordinatesBar.SetDlgItemText(IDC_SelectionLine, (LPCTSTR)A2T(lineValueStringPtr));
	
	m_coordinatesBar.SetDlgItemText(IDC_SelectionColumn, (LPCTSTR)A2T(columnValueStringPtr)); 
	
}		// end "UpdateSelectionCoordinates"



void CMImageFrame::UpdateSelectedAreaInformation(
				char*									areaDescriptionStringPtr,
				char*									areaValueStringPtr)
{       
	USES_CONVERSION;

	m_coordinatesBar.SetDlgItemText(IDC_NumberPixelsPrompt, (LPCTSTR)A2T(areaDescriptionStringPtr));
	
	m_coordinatesBar.SetDlgItemText(IDC_NumberPixels, (LPCTSTR)A2T(areaValueStringPtr)); 
	
}		// end "UpdateSelectedAreaInformation"

	

//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								� Purdue Research Foundation
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
//	Revised By:			Larry L. Biehl			Date: 12/11/2000	

void	
CMImageFrame::UpdateCursorCoordinates (
				char*									lineValueStringPtr,
				char*									columnValueStringPtr)

{	
	USES_CONVERSION;
	
	m_coordinatesBar.SetDlgItemText(IDC_CursorLine, (LPCTSTR)A2T(lineValueStringPtr));
	
	m_coordinatesBar.SetDlgItemText(IDC_CursorColumn, (LPCTSTR)A2T(columnValueStringPtr));
	
}		// end "UpdateCursorCoordinates"

	

//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								� Purdue Research Foundation
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
//	Revised By:			Larry L. Biehl			Date: 12/11/2000	

void	
CMImageFrame::UpdateCursorCoordinates (void)

{		
	USES_CONVERSION;
					
	sprintf ((char*)gTextString, "");
	
	m_coordinatesBar.SetDlgItemText(IDC_CursorColumn, (LPCTSTR)A2T((char*)gTextString));
	m_coordinatesBar.SetDlgItemText(IDC_CursorLine, (LPCTSTR)A2T((char*)gTextString));
	
}		// end "UpdateCursorCoordinates" 



void CMImageFrame::OnUpdateOverlay(CCmdUI* pCmdUI) 
{
	WindowInfoPtr			windowInfoPtr;
	
	Handle					windowInfoHandle;

	Boolean					enableFlag = FALSE;


	windowInfoHandle = GetWindowInfoHandle (gActiveImageViewCPtr);
	windowInfoPtr = (WindowInfoPtr)GetHandlePointer(
												windowInfoHandle, kNoLock, kNoMoveHi);

	if (windowInfoPtr != NULL && (windowInfoPtr->numberOverlays > 0 || 
													windowInfoPtr->numberImageOverlays > 0))
		enableFlag = TRUE;

	pCmdUI->Enable(enableFlag);
	
}



void CMImageFrame::OnOverlay() 
{
	// Add your command handler code here
	
}
