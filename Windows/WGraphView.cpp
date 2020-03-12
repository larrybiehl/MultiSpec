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
//	File:						WGraphView.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			03/09/2020
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	This file contains functions that relate to the
//								CMGraphView, CMGraphViewButton, and CMGraphViewCombo classes.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"                

#include "WGraphDoc.h"
#include "WGraphView.h"
#include "WMainFrame.h"
#include "WMultiSpec.h"

#define	kXAxis			1
#define	kYAxis			2
#define	kBothXYAxes		3		

#if defined _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



IMPLEMENT_DYNCREATE (CMGraphView, CFormView)

CBrush		CMGraphView::s_grayBrush;
		
CFont			CMGraphView::s_font;  

CPen			CMGraphView::s_bluePen,
				CMGraphView::s_grayPen,
				CMGraphView::s_greenPen,
				CMGraphView::s_ltGrayPen,
				CMGraphView::s_redPen,
				CMGraphView::s_whitePen;

RECT			CMGraphView::s_updateRect;



BEGIN_MESSAGE_MAP (CMGraphView, CFormView)
	//{{AFX_MSG_MAP (CMGraphView)
	ON_BN_CLICKED (IDC_NextChannel, OnNextChannel)
	ON_BN_CLICKED (IDC_PreviousChannel, OnPreviousChannel)

	ON_CBN_DROPDOWN (IDC_xAxis, OnDropdownXAxisDropList)

	ON_CBN_SELENDOK (IDC_xAxis, OnSelendokXAxisDropList)

	ON_COMMAND (ID_EDIT_COPY, OnEditCopy)

	ON_UPDATE_COMMAND_UI (ID_EDIT_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI (ID_FILE_SAVE_AS, OnUpdateFileSaveAs)

	ON_WM_CREATE ()

	ON_WM_DESTROY ()

	ON_WM_SIZE ()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMGraphView::CMGraphView ()
		: CFormView (CMGraphView::IDD)

{
	Boolean								continueFlag = TRUE;
	SelectionIOInfoPtr 				selectionIOPtr;
	
	
	m_graphRecordHandle = NULL;
	m_window = NULL;
	m_closeGraphSelectedFlag = FALSE;	// Used by List Data Processor
	
   m_graphRecordHandle = (Handle)MNewHandle (sizeof (GraphRecord));
   continueFlag = (m_graphRecordHandle != NULL); 
		
	IntializeGraphRecord ();
   
	if (continueFlag &&
			s_selectionIOInfoHandle == NULL &&
			gProcessorCode == kSelectionGraphProcessor)
		{                               		
		s_selectionIOInfoHandle = MNewHandle (sizeof (SelectionIOInfo));
					
		if (s_selectionIOInfoHandle != NULL)
			{ 																			
			selectionIOPtr =
							(SelectionIOInfoPtr)GetHandlePointer (s_selectionIOInfoHandle);
								                                                          
			selectionIOPtr->channelStatsHandle = NULL;
			selectionIOPtr->sumSquaresHandle = NULL;
			selectionIOPtr->ioBufferHandle = NULL;
			selectionIOPtr->bufferSize = 0;
			selectionIOPtr->outputBufferOffset = 0;
			selectionIOPtr->checkIOMemoryFlag = TRUE;
			selectionIOPtr->memoryWarningFlag = FALSE;
							
			}	// end "if (s_selectionIOInfoHandle != NULL)" 
			
		}	// end "if (continueFlag && ..."
	
	if (continueFlag)
		{                      
		s_graphicsWindowCount++;	
		m_graphicsWindowNumber = s_graphicsWindowCount;
		
		s_numberOfGWindows++;
		
		}	// end "if (continueFlag)"
		
	m_xAxisSelection = 0;
	m_printCopyModeFlag = FALSE;
	m_printerTextScaling = 1.0;
	m_dialogFromPtr = NULL;
	m_dialogToPtr = NULL;
	
	if (s_graphicsWindowCount == 1)
		{                                                        
		LOGBRUSH			logbrush; 
		LOGPEN 			logpen;
		 
							
		logpen.lopnStyle = PS_SOLID;
		logpen.lopnWidth.x = 1;
		logpen.lopnWidth.y = 1;
		
		if (continueFlag)
			{              
			logpen.lopnColor = RGB (192, 192, 192);
						
			continueFlag = s_ltGrayPen.CreatePenIndirect (&logpen);
			
			}	// end "if (continueFlag)"     
		
		if (continueFlag)
			{              
			logpen.lopnColor = RGB (128, 128, 128);
						
			continueFlag = s_grayPen.CreatePenIndirect (&logpen);
			
			}	// end "if (continueFlag)"    
		
		if (continueFlag)
			{  
			::GetObject (GetStockObject (LTGRAY_BRUSH), sizeof (LOGBRUSH), &logbrush);
						
			continueFlag = s_grayBrush.CreateBrushIndirect (&logbrush);
			
			}	// end "if (continueFlag)"
		
		if (continueFlag)
			{              
			LOGFONT		lf;
	
					// Set font to be used. 
			
			::GetObject (GetStockObject (ANSI_VAR_FONT), sizeof (LOGFONT), &lf);
			
					// Change to Courier font. 

			//lf.lfHeight = 10;
			lf.lfHeight = ((CMultiSpecApp*)AfxGetApp())->getFontHeightForDefaultDC (10.0);
			lf.lfWidth = 0;    
			lf.lfQuality = DEFAULT_QUALITY;
				
			continueFlag = s_font.CreateFontIndirect (&lf);
			
			}	// end "if (continueFlag)"
		
		}	// end "if (s_graphicsWindowCount == 1)"

	TRY
		{
		m_graphOverlayMenu = new CMenu ();
	
		m_graphOverlayMenu->CreatePopupMenu ();
		m_graphOverlayMenu->AppendMenu (MF_STRING,
													ID_GRAPHOVERLAYMENUITEMSTART, 
													(LPCTSTR)_T("Draw histogram"));
		m_graphOverlayMenu->AppendMenu (MF_STRING,
													ID_GRAPHOVERLAYMENUITEMSTART+1, 
													(LPCTSTR)_T("Draw density function"));
		m_graphOverlayMenu->AppendMenu (MF_STRING,
													ID_GRAPHOVERLAYMENUITEMSTART+2, 
													(LPCTSTR)_T("Draw histogram & density function"));
		
		m_selectVectorMenu = new CMenu ();
		m_selectVectorMenu->CreatePopupMenu ();
		
		m_binWidthMenu = new CMenu ();
		
		m_binWidthMenu->CreatePopupMenu ();
		m_binWidthMenu->AppendMenu (MF_STRING,
												ID_SELECTBINWIDTHMENUITEMSTART,
												(LPCTSTR)_T("Auto"));
		m_binWidthMenu->AppendMenu (MF_STRING,
												ID_SELECTBINWIDTHMENUITEMSTART+1,
												(LPCTSTR)_T("1"));
		m_binWidthMenu->AppendMenu (MF_STRING,
												ID_SELECTBINWIDTHMENUITEMSTART+2,
												(LPCTSTR)_T("2"));
		m_binWidthMenu->AppendMenu (MF_STRING,
												ID_SELECTBINWIDTHMENUITEMSTART+3,
												(LPCTSTR)_T("3"));
		m_binWidthMenu->AppendMenu (MF_STRING,
												ID_SELECTBINWIDTHMENUITEMSTART+4,
												(LPCTSTR)_T("4"));
		m_binWidthMenu->AppendMenu (MF_STRING,
												ID_SELECTBINWIDTHMENUITEMSTART+5,
												(LPCTSTR)_T("5"));
		m_binWidthMenu->AppendMenu (MF_STRING,
												ID_SELECTBINWIDTHMENUITEMSTART+6,
												(LPCTSTR)_T("10"));
		m_binWidthMenu->AppendMenu (MF_STRING,
												ID_SELECTBINWIDTHMENUITEMSTART+7,
												(LPCTSTR)_T("20"));
		m_binWidthMenu->AppendMenu (MF_STRING,
												ID_SELECTBINWIDTHMENUITEMSTART+8,
												(LPCTSTR)_T("30"));
		m_binWidthMenu->AppendMenu (MF_STRING,
												ID_SELECTBINWIDTHMENUITEMSTART+9,
												(LPCTSTR)_T("50"));
		
		m_dialogFromPtr = new CDataExchange (this, TRUE);

		m_dialogToPtr = new CDataExchange (this, FALSE);
	
		}	// end "TRY"

	CATCH_ALL (e)
		{
		MemoryMessage (0, kObjectMessage);
		continueFlag = FALSE;

		}
	
	END_CATCH_ALL
			
	if (continueFlag)
		{
		UpdateWindowList ((CMImageView*)this, kGraphicsWindowType);

		}	// end "if (continueFlag)"
	
	if (!continueFlag) 
		AfxThrowMemoryException ();
	
}	// end "CMGraphView"



CMGraphView::~CMGraphView ()

{           
	if (m_graphRecordHandle != NULL)
		{                    							                               
		GraphPtr graphRecordPtr =
									(GraphPtr)GetHandlePointer (m_graphRecordHandle, kLock);
		
		DisposeOfGraphRecordMemory (graphRecordPtr); 
		
		m_graphRecordHandle = UnlockAndDispose (m_graphRecordHandle);
			
		}	// end "m_graphRecordHandle != NULL"
			
	if (gSelectionGraphViewCPtr == this && s_selectionIOInfoHandle != NULL)
		{                    							
		SelectionIOInfoPtr selectionIOPtr = (SelectionIOInfoPtr)GetHandlePointer (
																		s_selectionIOInfoHandle, kLock);
							                                                          
		UnlockAndDispose (selectionIOPtr->channelStatsHandle); 
		UnlockAndDispose (selectionIOPtr->sumSquaresHandle);
		UnlockAndDispose (selectionIOPtr->ioBufferHandle); 
		
		s_selectionIOInfoHandle = UnlockAndDispose (s_selectionIOInfoHandle);
		
		}	// end "if (gSelectionGraphViewCPtr == this && ...)"
		
	if (gSelectionGraphViewCPtr == this)
		gSelectionGraphViewCPtr = NULL;
	
	if (s_numberOfGWindows > 0)	
		s_numberOfGWindows--; 
		
	if (m_dialogToPtr != NULL)
		delete m_dialogToPtr;

	if (m_dialogFromPtr != NULL)
		delete m_dialogFromPtr;

	if (m_graphOverlayMenu != NULL)
		{
		m_graphOverlayMenu->DestroyMenu ();
		delete m_graphOverlayMenu;

		}	// end "if (m_graphOverlayMenu != NULL)"

	if (m_selectVectorMenu != NULL)
		{
		m_selectVectorMenu->DestroyMenu ();
		delete m_selectVectorMenu;

		}	// end "if (m_selectVectorMenu != NULL)"

	if (m_binWidthMenu != NULL)
		{
		m_binWidthMenu->DestroyMenu ();
		delete m_binWidthMenu;

		}	// end "if (m_binWidthMenu != NULL)"
		
	RemoveWindowFromList ((CMImageView*)this, 0);
	
}	// end "~CMGraphView"



void CMGraphView::DoDataExchange (
				CDataExchange* 					pDX)

{
	CFormView::DoDataExchange (pDX);
	DDX_CBIndex (pDX, IDC_xAxis, m_xAxisSelection);
	
}	// end "DoDataExchange"



void CMGraphView::DoFilePrint ()

{
	gProcessorCode = kPrintProcessor;
	CFormView::OnFilePrint ();
	gProcessorCode = 0;
	
}	// end "DoFilePrint"



void CMGraphView::DoFilePrintPreview ()

{
	gProcessorCode = kPrintProcessor;
	CFormView::OnFilePrintPreview ();
	gProcessorCode = 0;
	
}	// end "DoFilePrintPreview"



CMGraphDoc* CMGraphView::GetDocument ()

{                                                           
	return (CMGraphDoc*)m_pDocument;
	
}	// end "GetDocument"



void CMGraphView::MoveGraphControls (void)

{
	CRect									graphWindowRect;

	GraphPtr								graphRecordPtr;

	Handle								graphRecordHandle;

	SInt32								left,
											top;

	
	GetWindowRect (graphWindowRect);
	graphRecordHandle = GetGraphRecordHandle ();
	graphRecordPtr = (GraphPtr)GetHandlePointer (graphRecordHandle);
	
		// Adjust the the "go to next set" control button to the proper position.
	
	if (graphRecordPtr->processorCode == kHistogramStatsProcessor)
		{
		CButton*								pushButton;
		pushButton = (CButton*)GetDlgItem (IDC_NextChannel);

		if (pushButton != NULL)
			{
			left = 105;  
			top = graphWindowRect.bottom - graphWindowRect.top - 23;
		
			if (pushButton != NULL)
				pushButton->SetWindowPos (NULL, 
													left,
													top,
													17,
													17,
													SWP_NOZORDER+SWP_NOACTIVATE);

			}	// end "if (pushButton != NULL)"
			
				// Adjust the the "go to previous set" button to the proper position.
	                                  																											
		pushButton = (CButton*)GetDlgItem (IDC_PreviousChannel);
		
		left += (kLegendScrollWidth+2);  

		if (pushButton != NULL)
			pushButton->SetWindowPos (NULL, 
												left,
												top,
												17,
												17,
												SWP_NOZORDER+SWP_NOACTIVATE);
			
				// Adjust the "select vector control button to the proper position.
	                                  																											
		pushButton = (CButton*)GetDlgItem (IDC_SelectVectors);
		
		left += 2*kLegendScrollWidth;  

		if (pushButton != NULL)
			pushButton->SetWindowPos (NULL, 
												left,
												top,
												17,
												17,
												SWP_NOZORDER+SWP_NOACTIVATE);
			
				// Adjust the "overlay options" control button to the proper position.
	                                  																											
		pushButton = (CButton*)GetDlgItem (IDC_GraphOverlay);
		
		left += (kLegendScrollWidth+2);  

		if (pushButton != NULL)
			pushButton->SetWindowPos (NULL, 
												left,
												top,
												17,
												17,
												SWP_NOZORDER+SWP_NOACTIVATE);
			
				// Adjust the "bin width" control button to the proper position.
	                                  																											
		pushButton = (CButton*)GetDlgItem (IDC_BinWidth);
		
		left += (kLegendScrollWidth+2);  

		if (pushButton != NULL)
			pushButton->SetWindowPos (NULL, 
												left,
												top,
												17,
												17,
												SWP_NOZORDER+SWP_NOACTIVATE);

		}	// end "if (...processorCode == kHistogramStatsProcessor)"
		
	else if (graphRecordPtr->processorCode == kSelectionGraphProcessor || 
						graphRecordPtr->processorCode == kListDataProcessor)
		{
				// Adjust the x axis popup menu
		
		CComboBox*								comboBoxPtr;
		comboBoxPtr = (CComboBox*)GetDlgItem (IDC_xAxis);
		if (comboBoxPtr != NULL)
			{
			left = (graphWindowRect.right - graphWindowRect.left) / 2 - 106;
			left = MAX (0, left);  
			top = graphWindowRect.bottom - graphWindowRect.top - 27;
		
			comboBoxPtr->SetWindowPos (NULL, 
												left, 
												top,
												212,
												17,
												SWP_NOZORDER);
			
			}	// end "if (comboBoxPtr != NULL)"
		
		}	// end "if (...processorCode == kSelectionGraphProcessor || ..."
	
}	// end "MoveGraphControls



int CMGraphView::OnCreate (
				LPCREATESTRUCT						lpCreateStruct)

{                  
	if (CFormView::OnCreate (lpCreateStruct) == -1)
																								return -1;

	SetFont (&s_font);
	
	return 0;
	
}	// end "OnCreate"



void CMGraphView::OnDestroy ()

{  
	if (gProcessorCode != kListDataProcessor)
		CFormView::OnDestroy ();
	
}	// end "OnDestroy"



void CMGraphView::OnDraw (
				CDC*									pDC)

{
	CFont									font;
	
	LOGFONT								logfont;
	
	CFont*								pOldFont;

	UINT									savedTextAlign;

	SignedByte							handleStatus;
	
	Boolean								continueFlag;
	
	
	continueFlag = TRUE;
	                                                   
	GraphPtr graphRecordPtr = (GraphPtr)GetHandleStatusAndPointer (
																				m_graphRecordHandle,
																				&handleStatus);

	graphRecordPtr->textScaling = m_printerTextScaling; 
	
			// Get the current portion of the graph window to draw the
			// graph within.
	
	if (m_printCopyModeFlag)
		{
				// Set font to be used. 
		
		::GetObject (GetStockObject (ANSI_VAR_FONT), sizeof (LOGFONT), &logfont);

		//logfont.lfHeight = (int)(10 * graphRecordPtr->textScaling);
		logfont.lfHeight = (LONG)
				(((CMultiSpecApp*)AfxGetApp())->getFontHeightForDefaultDC (10.0) * 
																		graphRecordPtr->textScaling);
		logfont.lfWidth = 0;    
		logfont.lfQuality = DEFAULT_QUALITY;
		                                                            
		continueFlag = font.CreateFontIndirect (&logfont);

		if (continueFlag)
			pOldFont = pDC->SelectObject (&font);
		
		graphRecordPtr->clientRect.top = s_updateRect.top;
		graphRecordPtr->clientRect.left = s_updateRect.left;
		graphRecordPtr->clientRect.bottom = s_updateRect.bottom;
		graphRecordPtr->clientRect.right = s_updateRect.right;

		}	// end "if (m_printCopyModeFlag)"

	else	// !m_printCopyModeFlag
		{
		pOldFont = pDC->SelectObject (&s_font);

		GetClientRect ((RECT*)&graphRecordPtr->clientRect);
	
				// Save the display pixels per inch.
		
		m_xPixelsPerInch = pDC->GetDeviceCaps (LOGPIXELSX);
		m_yPixelsPerInch = pDC->GetDeviceCaps (LOGPIXELSY);

		}	// end "else !m_printCopyModeFlag"
	   
	   	// Save the pDC in a global varable so that it can be accessed
	   	// later.
	   	
	if (continueFlag)
		{
		gCDCPointer = pDC;
		graphRecordPtr->pDC = pDC;
		
				// Get the size (height) of character.
				 
		CSize size = pDC->GetTextExtent ((LPCTSTR)_T("W"), 1);
		graphRecordPtr->textSize = (SInt16)(.7 * size.cy);
		
		savedTextAlign = pDC->GetTextAlign ();
		pDC->SetTextAlign (TA_BASELINE);											  
		Boolean returnFlag = DrawGraph (graphRecordPtr, FALSE);
		pDC->SetTextAlign (savedTextAlign);

		DrawGraphGrowIcon (graphRecordPtr);
							  
		graphRecordPtr->pDC = NULL;
		gCDCPointer = NULL; 

		pDC->SelectObject (pOldFont);

		}	// end "if (continueFlag)"

	graphRecordPtr->textScaling = 1;
	
	MHSetState (m_graphRecordHandle, handleStatus);
	                               
}	// end "OnDraw"  



void CMGraphView::OnDropdownXAxisDropList ()

{
	CComboBox*							xAxisComboBoxPtr;
	GraphPtr								graphRecordPtr;
	

	graphRecordPtr = (GraphPtr)GetHandlePointer (m_graphRecordHandle);
	xAxisComboBoxPtr = (CComboBox*)GetDlgItem (IDC_xAxis);
	SetUpXAxisPopUpMenu (graphRecordPtr, (MenuHandle)xAxisComboBoxPtr);
	                               
}	// end "OnDropdownXAxisDropList"



void CMGraphView::OnEditCopy ()

{
	CBitmap								cBmp;

   CRect									destinationRect;	// For storing the size of the image to be copied.

   CDC									hDC;


			// Get the size of the window
	
	GetClientRect (destinationRect);
	
	s_updateRect.bottom = (int)destinationRect.bottom;
	s_updateRect.top = (int)destinationRect.top;
	s_updateRect.right = (int)destinationRect.right;
	s_updateRect.left = (int)destinationRect.left;

	CClientDC   cWndDC (this);					// View is an hWnd, so we can use "this"
   hDC.CreateCompatibleDC (&cWndDC);			// Create the memory DC.

   cBmp.CreateCompatibleBitmap (&cWndDC,
   										destinationRect.Width (),
   										destinationRect.Height ());
	
			// Keep the old bitmap
	
   CBitmap* pOldBitmap = hDC.SelectObject (&cBmp);

			//prepare the DC
	
	hDC.BitBlt (0,
					0,
					destinationRect.Width (),
					destinationRect.Height (),
					&cWndDC,
					0,
					0,
					WHITENESS);

			//copy the image to the DC
	
	gCDCPointer = &hDC;
	m_printCopyModeFlag = TRUE;
	
	OnDraw (&hDC);
	
	gCDCPointer = NULL;
	m_printCopyModeFlag = FALSE;
	
			// here are the actual clipboard functions.

   AfxGetApp()->m_pMainWnd->OpenClipboard ();
   EmptyClipboard ();
   SetClipboardData (CF_BITMAP, cBmp.GetSafeHandle ());
   CloseClipboard ();

        // next we select the old bitmap back into the memory DC
        // so that our bitmap is not deleted when cMemDC is destroyed.
        // Then we detach the bitmap handle from the cBmp object so that
        // the bitmap is not deleted when cBmp is destroyed.

	hDC.SelectObject (pOldBitmap);
   cBmp.Detach ();
	
}	// end "OnEditCopy"



void CMGraphView::OnInitialUpdate (void)

{
	CComboBox*							xAxisComboBoxPtr;

	HBITMAP								bitmapHandle;

	Boolean								continueFlag = TRUE,
											newWindowFlag = FALSE;
	                                 

	CMGraphDoc* graphDocCPtr = GetDocument ();
	graphDocCPtr->SetGraphViewCPtr (this); 
	
			// Force the window frame to be the same size as the graphics view.
	
	GetParentFrame()->RecalcLayout ();
	CSize docSize = CSize (1,1);
	SetScrollSizes (MM_TEXT, docSize);

			// Subclass the next channel control button.

	if (m_nextChannel.SubclassDlgItem (IDC_NextChannel, this))
		m_nextChannel.SetButtonID (IDC_NextChannel);

			// Subclass the previous channel control button.

	if (m_previousChannel.SubclassDlgItem (IDC_PreviousChannel, this))
		m_previousChannel.SetButtonID (IDC_PreviousChannel);

			// Subclass the vector selection control button.

	if (m_selectVectors.SubclassDlgItem (IDC_SelectVectors, this))
		m_selectVectors.SetButtonID (IDC_SelectVectors);

			// Subclass the overlay control button.

	if (m_overlayControl.SubclassDlgItem (IDC_GraphOverlay, this))
		m_overlayControl.SetButtonID (IDC_GraphOverlay);

			// Subclass the overlay control button.

	if (m_binWidth.SubclassDlgItem (IDC_BinWidth, this))
		m_binWidth.SetButtonID (IDC_BinWidth);

			// Subclass the x Axis combo box.

	m_xAxisCombo.SubclassDlgItem (IDC_xAxis, this);

			// Set the graph view class pointers in the button structures.

	m_nextChannel.SetGraphViewCPtr (this);
	m_previousChannel.SetGraphViewCPtr (this);
	m_selectVectors.SetGraphViewCPtr (this);
	m_overlayControl.SetGraphViewCPtr (this);
	m_binWidth.SetGraphViewCPtr (this);

	GraphPtr graphRecordPtr = (GraphPtr)GetHandlePointer (m_graphRecordHandle);
			
	SetXAxisDescriptionInfo (graphRecordPtr, gImageWindowInfoPtr, gImageFileInfoPtr);
										
			// Get the labels for the graph.											
				
	//GetGraphLabels (graphRecordPtr);
	                                 
	if (gProcessorCode == kSelectionGraphProcessor ||
							gProcessorCode == kListDataProcessor)
		{
		newWindowFlag = TRUE;
		
		if (gProcessorCode == kSelectionGraphProcessor)
			{
					// Rename title of current selection graph window to indicate
					// that it will now not be the selection graph window.
		
			SetGraphWindowTitle (gSelectionGraphViewCPtr, FALSE);
			
			continueFlag = SelectionGraphControl (this);

			}	// end "if (gProcessorCode == kSelectionGraphProcessor)"

				// Hide graph window controls that are not needed.

		GetDlgItem (IDC_NextChannel)->ShowWindow (SW_HIDE);
		GetDlgItem (IDC_PreviousChannel)->ShowWindow (SW_HIDE);
		GetDlgItem (IDC_SelectVectors)->ShowWindow (SW_HIDE);
		GetDlgItem (IDC_GraphOverlay)->ShowWindow (SW_HIDE);
		GetDlgItem (IDC_BinWidth)->ShowWindow (SW_HIDE);

		if (graphRecordPtr->xLabel[0] > 0)
			GetDlgItem (IDC_xAxis)->ShowWindow (SW_HIDE);
		
		DDX_CBIndex (m_dialogToPtr, IDC_xAxis, m_xAxisSelection);
		xAxisComboBoxPtr = (CComboBox*)GetDlgItem (IDC_xAxis);
		SetUpXAxisPopUpMenu (graphRecordPtr, (MenuHandle)xAxisComboBoxPtr);
		
		}	// end "if (gProcessorCode == kSelectionGraphProcessor || ..."
	                                 
	else if (gProcessorCode == kHistogramStatsProcessor)
		{
		newWindowFlag = TRUE;

		bitmapHandle = (HBITMAP)LoadImage (AfxGetInstanceHandle (),
														(LPCTSTR)_T("IDB_Classes"), 
														IMAGE_BITMAP, 
														0, 
														0, 
														LR_DEFAULTCOLOR);
		if (bitmapHandle != NULL)
				m_selectVectors.SetBitmap (bitmapHandle);

		bitmapHandle = (HBITMAP)LoadImage (AfxGetInstanceHandle (),
														(LPCTSTR)_T("IDB_Overlay"), 
														IMAGE_BITMAP, 
														0, 
														0, 
														LR_DEFAULTCOLOR);
		if (bitmapHandle != NULL)
				m_overlayControl.SetBitmap (bitmapHandle);

		bitmapHandle = (HBITMAP)LoadImage (AfxGetInstanceHandle (),
														(LPCTSTR)_T("IDB_BinWidth"), 
														IMAGE_BITMAP, 
														0, 
														0, 
														LR_DEFAULTCOLOR);
		if (bitmapHandle != NULL)
				m_binWidth.SetBitmap (bitmapHandle);

				// Show the graph window controls.

		GetDlgItem (IDC_NextChannel)->ShowWindow (SW_SHOW);
		GetDlgItem (IDC_PreviousChannel)->ShowWindow (SW_SHOW);
		GetDlgItem (IDC_SelectVectors)->ShowWindow (SW_SHOW);
		GetDlgItem (IDC_GraphOverlay)->ShowWindow (SW_SHOW);
		GetDlgItem (IDC_BinWidth)->ShowWindow (SW_SHOW);
		GetDlgItem (IDC_xAxis)->ShowWindow (SW_HIDE);

		MoveGraphControls ();
		
		}	// end "if (gProcessorCode == kHistogramStatsProcessor)"

	else if (gProcessorCode == kBiPlotDataProcessor)
		GetDlgItem (IDC_xAxis)->ShowWindow (SW_HIDE);
		
	if (continueFlag)                                                         
		SetGraphWindowTitle (this, newWindowFlag); 
							
	if (gProcessorCode == kListDataProcessor || 
												gProcessorCode == kHistogramStatsProcessor)
		graphRecordPtr->drawGraphCode = (SInt16)0x8000; 

	graphRecordPtr->window = (CMImageView*)this;
	
}	// end "OnInitialUpdate"



void CMGraphView::OnNextChannel ()

{
	Boolean								returnFlag;


	if (((CMultiSpecApp*)AfxGetApp())->GetZoomCode () == IDC_NextChannel)
		{
		DoNextOrPreviousChannel	(this, kNextGraphSetControl);
		
		returnFlag = UpdateGraphControls ();

		if (!returnFlag)
			{
			ReleaseCapture ();
			((CMultiSpecApp*)AfxGetApp())->SetZoomCode (0);
			((CMultiSpecApp*)AfxGetApp())->SetControlDelayFlag (TRUE);

			}	// "if (!returnFlag)"

		}	// end "if (((CMultiSpecApp*)AfxGetApp())->GetZoomCode () == .."
	
}	// end "OnNextChannel"



BOOL CMGraphView::OnPreparePrinting (
				CPrintInfo							*pInfo)

{
			// default preparation
	
	return DoPreparePrinting (pInfo);
	
}	// end "OnPreparePrinting"



void CMGraphView::OnPreviousChannel ()

{
	Boolean								returnFlag;
	

	if (((CMultiSpecApp*)AfxGetApp())->GetZoomCode () == IDC_PreviousChannel)
		DoNextOrPreviousChannel	(this, kPreviousGraphSetControl);
	
	returnFlag = UpdateGraphControls ();
	if (!returnFlag)
		{
		ReleaseCapture ();
		((CMultiSpecApp*)AfxGetApp())->SetZoomCode (0);
		((CMultiSpecApp*)AfxGetApp())->SetControlDelayFlag (TRUE);

		}	// end "if (!returnFlag)"
	
}	// end "OnPreviousChannel"
                                                                         

                                                                             
void CMGraphView::OnPrint (
				CDC* 									pDC,
				CPrintInfo* 						pInfo)

{	
	RECT									sourceRect;
	

			// If Stretch Device Independant Bitmaps are not support on this
 			// printer, then exit.
 			            
	int returnValue = pDC->GetDeviceCaps (RASTERCAPS);
	if (!(returnValue & RC_STRETCHDIB))
																									return;
	
			// Get the source rectangle.
			
	GetClientRect ((RECT*)&sourceRect);
	
			// Get the width and height of the printed page in pixels. 
					
	SInt16 pageWidth = pDC->GetDeviceCaps (HORZRES);
	SInt16 pageHeight = pDC->GetDeviceCaps (VERTRES);
	
			// Get the display pixels per inch.
			
	SInt16 cxInch = pDC->GetDeviceCaps (LOGPIXELSX);
	SInt16 cyInch = pDC->GetDeviceCaps (LOGPIXELSY);
	
			// Get the scaling needed to account for the difference in the display
			// and paper pixels per inch. 
	
	m_printerTextScaling = (double)cyInch/m_yPixelsPerInch;  
	
			// Get the magnification for the selected image scaled to one page.
			
	double magWidth = (double)pageWidth / (sourceRect.right - sourceRect.left + 1);
			
	double magHeight = (double)pageHeight / (sourceRect.bottom - sourceRect.top + 1);
												
	double onePageScaleMagnification = MIN (magHeight, magWidth);
	
			// Get the user magnification to use to scale the image the same
			// as is used in the image window. Take into account any difference
			// in printer and display pixels per inch. 
	
	magWidth = cxInch / m_xPixelsPerInch;
	magHeight = cyInch / m_yPixelsPerInch; 
	double userMagnification = MIN (magHeight, magWidth);

 	double magnification = MIN (onePageScaleMagnification, userMagnification);
 	 										
 			// Set the origin so that the image is centered on the page. 
			                                             
	SInt16 centerAdjustWidth = (SInt16)(magnification *
														(sourceRect.right - sourceRect.left + 1));
	centerAdjustWidth = MAX ((pageWidth - centerAdjustWidth)/2, 0);
	
	SInt16 centerAdjustHeight = (SInt16)(magnification *
														(sourceRect.bottom - sourceRect.top + 1));
	centerAdjustHeight = MAX ((pageHeight - centerAdjustHeight)/2, 0);

	pDC->SetWindowOrg (pInfo->m_rectDraw.left-centerAdjustWidth,
								pInfo->m_rectDraw.top-centerAdjustHeight);

			// Save the size of the graph window to be printed into.

	s_updateRect = sourceRect;
	s_updateRect.bottom = (int)(s_updateRect.bottom * magnification);
	s_updateRect.right = (int)(s_updateRect.right * magnification);
 	
			// Now print the rest of the page 
			        
	m_printCopyModeFlag = TRUE;
	OnDraw (pDC);		
	m_printCopyModeFlag = FALSE;
	
	m_printerTextScaling = 1.0;										
	
}	// end "OnPrint"



void CMGraphView::OnSelendokXAxisDropList ()

{
	CComboBox* 							comboBoxPtr;
	GraphPtr								graphRecordPtr;

	Handle								graphRecordHandle;
	
	int									currentSelection,
											xAxisSelection;
	                  
	Boolean								returnFlag = TRUE;
	
	
	comboBoxPtr = (CComboBox*)(GetDlgItem (IDC_xAxis));
	if (comboBoxPtr != NULL)
		{                               
		DDX_CBIndex (m_dialogFromPtr, IDC_xAxis, m_xAxisSelection); 
		xAxisSelection = comboBoxPtr->GetItemData (m_xAxisSelection);

		graphRecordHandle = GetGraphRecordHandle ();
		graphRecordPtr = (GraphPtr)GetHandlePointer (graphRecordHandle);
				
		currentSelection = graphRecordPtr->xAxisCode;
			
		if (xAxisSelection > 0 && currentSelection != xAxisSelection)
			{
			graphRecordPtr->xAxisCode = xAxisSelection;
			
			if (xAxisSelection == kChannels)
				graphRecordPtr->xValueTypeCode = kIntegerType;
							
			else	// xAxisSelection != kChannels
				graphRecordPtr->xValueTypeCode = kRealType;

			ReloadXAxis (graphRecordPtr, xAxisSelection);

			if (graphRecordPtr->processorCode == kListDataProcessor)
				graphRecordPtr->graphViewCPtr->Invalidate ();

			}	// end "if (errCode == noErr && ..."

		}	// end "if (comboBoxPtr != NULL)"
	
}	// end "OnSelendokXAxisDropList"



void CMGraphView::OnSize (
				UINT									nType, 
				int									cx, 
				int									cy)

{
	CFormView::OnSize (nType, cx, cy);
	
	UpdateGraphScales (GetGraphRecordHandle ());	
	                       
	MoveGraphControls ();

}	// end "OnSize"



void CMGraphView::OnUpdateEditCopy (
				CCmdUI* 								pCmdUI)

{
	UpdateEditGraphicsCopy (pCmdUI);
	
	pCmdUI->Enable (TRUE);
	
}	// end "OnUpdateEditCopy"



void CMGraphView::OnUpdateFileSaveAs (
				CCmdUI*								pCmdUI)

{
	pCmdUI->Enable (FALSE);
	
}	// end "OnUpdateFileSaveAs"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean UpdateGraphControls
//
//	Software purpose:	This routine controls updating the thematic image legend
//							controls.  The routine is called after any change is made 
//							in the size or content of the thematic legend.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	FALSE = button was changed to disabled.
//							TRUE = button was not changed to disabled.	
// 
// Called By:			DoGraphWActivateEvent in SGraUtil.cpp
//							GraphWControlEvent in SGraUtil.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/25/1999
//	Revised By:			Larry L. Biehl			Date: 12/24/2002		

Boolean CMGraphView::UpdateGraphControls (void)

{	     
	GraphPtr								graphRecordPtr;
	
	SInt16								numberSets,
											numberVectors,
											set;
	
	Boolean								returnFlag = TRUE;
	
	
	graphRecordPtr = (GraphPtr)GetHandlePointer (m_graphRecordHandle);
											
	numberSets = graphRecordPtr->numberSets;
	set = graphRecordPtr->set;
	numberVectors = graphRecordPtr->numberVectors;
		             
	if (numberSets <= 1)
		{
		GetDlgItem (IDC_NextChannel)->ShowWindow (FALSE);
		GetDlgItem (IDC_PreviousChannel)->ShowWindow (FALSE);
		
		}	// end "if (numberSets <= 1)"
		
	else	// numberSets > 1
		{
		if (set < numberSets)
			GetDlgItem (IDC_NextChannel)->EnableWindow (TRUE);
			
		else	// set >= numberSets
			returnFlag = GetDlgItem (IDC_NextChannel)->EnableWindow (FALSE);
			
		if (set > 1)
			GetDlgItem (IDC_PreviousChannel)->EnableWindow (TRUE);
			
		else	// >set <= 1
			returnFlag = GetDlgItem (IDC_PreviousChannel)->EnableWindow (FALSE);
			
		}	// end "else numberSets > 1"

	if (numberVectors <= 1)
		GetDlgItem (IDC_SelectVectors)->ShowWindow (FALSE);
	else	// numberVectors > 1
		GetDlgItem (IDC_SelectVectors)->ShowWindow (TRUE);

	if (graphRecordPtr->graphCodesAvailable > 1)
		GetDlgItem (IDC_GraphOverlay)->ShowWindow (TRUE);
	else	// graphRecordPtr->graphCodesAvailable == 1
		GetDlgItem (IDC_GraphOverlay)->ShowWindow (FALSE);

	return (returnFlag);
	
}	// end "UpdateGraphControls" 



//------------------------------------------------------------------------------------
// CMGraghViewButton                                                        

BEGIN_MESSAGE_MAP (CMGraphViewButton, CButton)
	//{{AFX_MSG_MAP (CMGraphViewButton)
	ON_WM_KEYDOWN ()
	ON_WM_KEYUP ()
	ON_WM_LBUTTONDOWN ()
	ON_WM_LBUTTONUP ()
	ON_WM_MOUSEMOVE ()
	ON_WM_RBUTTONDOWN ()
	ON_WM_RBUTTONUP ()   
	//}}AFX_MSG_MAP

	ON_COMMAND_RANGE (ID_GRAPHOVERLAYMENUITEMSTART, ID_GRAPHOVERLAYMENUITEMEND, OnLines2)
	ON_COMMAND_RANGE (ID_SELECTBINWIDTHMENUITEMSTART, ID_SELECTBINWIDTHMENUITEMEND, OnBinWidth)
	ON_COMMAND_RANGE (ID_SELECTVECTORMENUITEMSTART, ID_SELECTVECTORMENUITEMEND, OnSelectVector)
END_MESSAGE_MAP ()
                 


CMGraphViewButton::CMGraphViewButton ()

{
	m_buttonID = 0;
	m_graphViewCPtr = NULL;

}	// end "CMGraphViewButton" 



CMGraphViewButton::~CMGraphViewButton ()

{                                   
	
}	// end "~CMButton"



void CMGraphViewButton::OnBinWidth (
				UINT									menuID)
{
	GraphPtr								graphRecordPtr;

	Handle								graphRecordHandle;
	SInt32								selection;


	ReleaseCapture ();

	selection = menuID - ID_SELECTBINWIDTHMENUITEMSTART + 1;
	if (SetHistogramBinWidth (m_graphViewCPtr, (SInt16)selection))
		{
		graphRecordHandle = m_graphViewCPtr->GetGraphRecordHandle ();
		graphRecordPtr = (GraphPtr)GetHandlePointer (graphRecordHandle, kLock);

		SetGraphMinMax (graphRecordPtr, kBothXYAxes);

		CheckAndUnlockHandle (graphRecordHandle);
		
		m_graphViewCPtr->Invalidate ();

		}	// end "if (SetHistogramBinWidth (m_graphViewCPtr, selection))"
	
}	// end "OnBinWidth"

                                 

void CMGraphViewButton::OnKeyDown (
				UINT									nChar,
				UINT									nRepCnt,
				UINT									nFlags)
				
{     
	//if (nChar == 0x10)
	//	{
		//CMLegendView* legendViewCPtr = (CMLegendView*)GetParent ();
		//legendViewCPtr->GetLegendListCPtr()->CheckShiftKeyDown ();
		
	//	}	// end "if (nChar == 0x10)"
	                                                                                                                                                                                        
	//else if (nChar == 0x11)
		//gActiveImageViewCPtr->SetControlKeyFlag (TRUE);
	
	CButton::OnKeyDown (nChar, nRepCnt, nFlags);
	
}	// end "OnKeyDown"



void CMGraphViewButton::OnKeyUp (
				UINT									nChar,
				UINT									nRepCnt,
				UINT									nFlags)

{
	/*
	if (nChar == 0x10)
		{                
		//CMLegendView* legendViewCPtr = (CMLegendView*)GetParent ();
		//legendViewCPtr->GetLegendListCPtr()->CheckShiftKeyUp ();
		
		}	// end "if (nChar == 0x10)"  
	                                                                                                                                                                                        
	else if (nChar == 0x11)           
		//gActiveImageViewCPtr->SetControlKeyFlag (FALSE);
	*/
	CButton::OnKeyUp (nChar, nRepCnt, nFlags);
	
}	// end "OnKeyUp"



void CMGraphViewButton::OnLButtonDown (
				UINT									nFlags,
				CPoint								point)

{
	CWnd*									captured;


	captured = GetCapture ();

	CButton::OnLButtonDown (nFlags, point);

	captured = GetCapture ();
	
	if (m_buttonID == IDC_NextChannel || m_buttonID == IDC_PreviousChannel)
		{
		if (GetState () & 0x0004)
			{
			((CMultiSpecApp*)AfxGetApp())->SetZoomCode (m_buttonID);
			((CMultiSpecApp*)AfxGetApp())->SetControlDelayFlag (TRUE);
			captured = GetCapture ();
			
			}	// end "if (GetState () & 0x0004)"

		}	// end "if (m_buttonID == IDC_NextChannel || ...)"

	else if (m_buttonID == IDC_GraphOverlay)
		{
		CMenu*			graphOverlayMenu;
		tagRECT			buttonRectangle;

	
		GetWindowRect (&buttonRectangle);

		graphOverlayMenu = m_graphViewCPtr->GetGraphOverlayPopupMenu ();

		SetUpOverlayPopUpMenu (m_graphViewCPtr, (MenuHandle)graphOverlayMenu);

		((CMultiSpecApp*)AfxGetApp())->SetZoomCode (m_buttonID);
		
		captured = GetCapture ();

		if (graphOverlayMenu->TrackPopupMenu (TPM_LEFTALIGN,
															buttonRectangle.right,
															buttonRectangle.top,
															this,
															NULL))
			{
			captured = GetCapture ();
			OnLButtonUp (nFlags, point);

			}	// end "graphOverlayMenu->TrackPopupMenu ..."

		captured = GetCapture ();

		ReleaseCapture ();
		((CMultiSpecApp*)AfxGetApp())->SetZoomCode (0);
		((CMultiSpecApp*)AfxGetApp())->SetControlDelayFlag (TRUE);

		}	// end "else if (m_buttonID == IDC_GraphOverlay)"

	else if (m_buttonID == IDC_SelectVectors)
		{
		CMenu*			selectVectorMenu;
		tagRECT			buttonRectangle;

	
		GetWindowRect (&buttonRectangle);

		selectVectorMenu = m_graphViewCPtr->GetSelectVectorPopupMenu ();

		SetUpVectorPopUpMenu (m_graphViewCPtr, (MenuHandle)selectVectorMenu);

		((CMultiSpecApp*)AfxGetApp())->SetZoomCode (m_buttonID);
		
		captured = GetCapture ();

		if (selectVectorMenu->TrackPopupMenu (TPM_LEFTALIGN,
															buttonRectangle.right,
															buttonRectangle.top,
															this,
															NULL))
			{
			captured = GetCapture ();
			OnLButtonUp (nFlags, point);

			}	// end "selectVectorMenu->TrackPopupMenu (..."

		captured = GetCapture ();

		ReleaseCapture ();
		((CMultiSpecApp*)AfxGetApp())->SetZoomCode (0);
		((CMultiSpecApp*)AfxGetApp())->SetControlDelayFlag (TRUE);
		

		}	// end "else if (m_buttonID == IDC_SelectVectors)"

	else if (m_buttonID == IDC_BinWidth)
		{
		CMenu*			binWidthMenu;
		tagRECT			buttonRectangle;

	
		GetWindowRect (&buttonRectangle);

		binWidthMenu = m_graphViewCPtr->GetBinWidthPopupMenu ();

		SetUpBinWidthPopUpMenu (m_graphViewCPtr, (MenuHandle)binWidthMenu);

		((CMultiSpecApp*)AfxGetApp())->SetZoomCode (m_buttonID);
		
		captured = GetCapture ();

		if (binWidthMenu->TrackPopupMenu (TPM_LEFTALIGN,
														buttonRectangle.right,
														buttonRectangle.top,
														this,
														NULL))
			{
			captured = GetCapture ();
			OnLButtonUp (nFlags, point);

			}	// end "binWidthMenu->TrackPopupMenu (..."

		captured = GetCapture ();

		ReleaseCapture ();
		((CMultiSpecApp*)AfxGetApp())->SetZoomCode (0);
		((CMultiSpecApp*)AfxGetApp())->SetControlDelayFlag (TRUE);
		
		}	// end "else if (m_buttonID == IDC_BinWidth)"

}	// end "OnLButtonDown"



void CMGraphViewButton::OnLButtonUp (
				UINT									nFlags,
				CPoint								point)
{
	CWnd*									captured;


	captured = GetCapture ();
	if (GetCapture () == this)
		{
		ReleaseCapture ();
		((CMultiSpecApp*)AfxGetApp())->SetZoomCode (0);
		((CMultiSpecApp*)AfxGetApp())->SetControlDelayFlag (TRUE);
		
		}	// end "if (GetCapture () == this)"
	
	CButton::OnLButtonUp (nFlags, point);
	
}	// end "OnLButtonUp"



void CMGraphViewButton::OnLines2 (
				UINT									menuID)

{
	SInt32								selection;


	ReleaseCapture ();

	selection = menuID - ID_GRAPHOVERLAYMENUITEMSTART + 1;
	SetOverlayDisplayList (m_graphViewCPtr, (SInt16)selection);

	m_graphViewCPtr->Invalidate ();
	
}	// end "OnLines2"



void CMGraphViewButton::OnMouseMove (
				UINT									nFlags,
				CPoint								point)

{
	UINT									itemID;
	

	CButton::OnMouseMove (nFlags, point);
	
	if (GetCapture () != this)
		itemID = 0;
	
	else	// GetCapture () == this
		{
		if (GetState () & 0x0004)
			itemID = m_buttonID;
		
		else	// !(state & 0x0008)
			itemID = 0;
		
		}	// end "else GetCapture () == this"
	
}	// end "OnMouseMove"



void CMGraphViewButton::OnRButtonDown (
				UINT									nFlags,
				CPoint								point)

{                                                      
	
	CButton::OnRButtonDown (nFlags, point);
	                           
	if (GetCapture () == this)
		((CMultiSpecApp*)AfxGetApp())->SetControlDelayFlag (FALSE);

}	// end "OnRButtonDown" 



void CMGraphViewButton::OnRButtonUp (
				UINT									nFlags,
				CPoint								point)
				
{  
	if (GetCapture () == this)                                                            
		((CMultiSpecApp*)AfxGetApp())->SetControlDelayFlag (TRUE);
		
	CButton::OnRButtonUp (nFlags, point);
	
}	// end "OnRButtonUp"

 

void CMGraphViewButton::OnSelectVector (
				UINT									menuID)

{
	GraphPtr								graphRecordPtr;

	Handle								graphRecordHandle;
	SInt32								selection;


	ReleaseCapture ();

	selection = menuID - ID_SELECTVECTORMENUITEMSTART;
	SetVectorDisplayList (m_graphViewCPtr, (SInt16)selection);

	graphRecordHandle = m_graphViewCPtr->GetGraphRecordHandle ();
	graphRecordPtr = (GraphPtr)GetHandlePointer (graphRecordHandle,
																kLock);
	
	SetGraphMinMax (graphRecordPtr, kBothXYAxes);

	m_graphViewCPtr->Invalidate ();
	
}	// end "OnSelectVector"



void CMGraphViewButton::SetButtonID (
				UInt16								buttonID)

{
	m_buttonID = buttonID;

}	// end "SetButtonID"



void CMGraphViewButton::SetGraphViewCPtr (
				CMGraphView*						graphViewCPtr)

{
	m_graphViewCPtr = graphViewCPtr;

}	// end "SetGraphViewCPtr"



//------------------------------------------------------------------------------------
// CMGraphViewCombo                                                        

BEGIN_MESSAGE_MAP (CMGraphViewCombo, CComboBox)
	//{{AFX_MSG_MAP (CMGraphViewCombo)
	ON_WM_KEYDOWN ()  
	//}}AFX_MSG_MAP
END_MESSAGE_MAP () 
                 
                 

CMGraphViewCombo::CMGraphViewCombo ()

{
	m_graphViewCPtr = NULL;

}	// end "CMGraphViewCombo" 



CMGraphViewCombo::~CMGraphViewCombo ()

{                                   
	
}	// end "~CMGraphViewCombo"

 

void CMGraphViewCombo::OnKeyDown (
				UINT 									nChar,
				UINT 									nRepCnt,
				UINT 									nFlags)

{
			// Add your message handler code here and/or call default

	switch (nChar)
		{
		case VK_LEFT:
			break;

		case VK_RIGHT:
 			break;  

		default:
			CComboBox::OnKeyDown (nChar, nRepCnt, nFlags);

		}	// end "switch (nChar)"

}	// end "OnKeyDown"



void CMGraphViewCombo::SetGraphViewCPtr (
				CMGraphView*						graphViewCPtr)

{                                                      
	m_graphViewCPtr = graphViewCPtr;

}	// end "SetGraphViewCPtr"
