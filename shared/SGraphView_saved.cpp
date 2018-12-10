//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SGraphView.cpp : implementation file 
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			04/04/2018
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	This file contains functions that relate to the graph view 
//								class.
//
//	Functions in file:	
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"                

#if defined multispec_mac || defined multispec_mac_swift
	#include "MGraphView.h"
#endif	// defined multispec_mac || defined multispec_mac_swift
                             
#if defined multispec_win 
	#include "WMultiSpec.h"
	#include "WMainFrame.h" 
	#include "WGraphView.h"
	#include "WGraphDoc.h"
#endif	// defined multispec_win 
 
#if defined multispec_lin 
	#include "LGraphView.h"
#endif	// defined multispec_lin
 
//#include	"SExtGlob.h"	

#define	kXAxis			1
#define	kYAxis			2
#define	kBothXYAxes		3		

extern void GetGraphLabels (
				GraphPtr								graphRecPtr);

extern void	ReloadXAxis (
				GraphPtr								graphRecordPtr,
				int									xAxisSelection);		

extern void SetGraphMinMax (
				GraphPtr								graphRecordPtr,
				SInt16								axisCode);	

extern void UpdateGraphScales (
				Handle								graphRecordHandle);	



#if defined multispec_win  
	#if defined _DEBUG
		#undef THIS_FILE
		static char BASED_CODE THIS_FILE[] = __FILE__;
	#endif

	IMPLEMENT_DYNCREATE (CMGraphView, CFormView)
//	IMPLEMENT_DYNCREATE (CMGraphView, CView)
	
	CBrush		CMGraphView::s_grayBrush;      
			
	CFont			CMGraphView::s_font;  
	
	CPen			CMGraphView::s_bluePen;
	
	CPen			CMGraphView::s_grayPen;
	
	CPen			CMGraphView::s_greenPen;
	
	CPen			CMGraphView::s_ltGrayPen;
	
	CPen			CMGraphView::s_redPen;
	
	CPen			CMGraphView::s_whitePen;     

	RECT			CMGraphView::s_updateRect;

#endif	// defined multispec_win


// === Static Member Variable ===

Handle			CMGraphView::s_selectionIOInfoHandle = NULL;
		
UInt16			CMGraphView::s_graphicsWindowCount = 0;

UInt16			CMGraphView::s_numberOfGWindows = 0;


/////////////////////////////////////////////////////////////////////////////
// CMGraphView

#if defined multispec_win
	CMGraphView::CMGraphView ()
		: CFormView (CMGraphView::IDD)
#endif	// defined multispec_win

#if defined multispec_mac || defined multispec_mac_swift
	CMGraphView::CMGraphView ()
#endif	// defined multispec_mac || defined multispec_mac_swift
{  
	Boolean					continueFlag = TRUE;
	SelectionIOInfoPtr 	selectionIOPtr;
	
	
	m_graphRecordHandle = NULL;
	m_window = NULL;
	
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
			selectionIOPtr = (SelectionIOInfoPtr)GetHandlePointer (s_selectionIOInfoHandle);
								                                                          
			selectionIOPtr->channelStatsHandle = NULL;
			//selectionIOPtr->graphRecordHandle = NULL;
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
		
	#if defined multispec_win
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

				lf.lfHeight = 10;   
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
			
			//m_xAxisPopupMenu = new CMenu ();
			//m_xAxisPopupMenu->CreatePopupMenu ();
			//CComboBox* comboBoxPtr = (CComboBox*)GetDlgItem(IDC_xAxis);
		                          
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
			//int xAxisSelection = 1;
			//DDX_CBIndex (m_dialogToPtr, IDC_xAxis, xAxisSelection);

			}	// end "if (continueFlag)"
		
		if (!continueFlag) 
			AfxThrowMemoryException ();
	#endif	// defined multispec_win
	
}	// end "CMGraphView"



CMGraphView::~CMGraphView ()
{           
	if (m_graphRecordHandle != NULL)
		{                    							                               
		GraphPtr graphRecordPtr = (GraphPtr)GetHandlePointer (m_graphRecordHandle, kLock);
		
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
		
	#if defined multispec_win
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
	#endif	// defined multispec_win
		
}	// end "~CMGraphView"



#if defined multispec_win
void CMGraphView::DoDataExchange (CDataExchange* pDX)
{
	CFormView::DoDataExchange (pDX);
	DDX_CBIndex (pDX, IDC_xAxis, m_xAxisSelection);
}
#endif	// defined multispec_win

                            
                            
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DrawGraphGrowIcon
//
//	Software purpose:	The purpose of this routine is to draw the grow icon for
//							a graph window.
//
//	Parameters in:		A window pointer
//
//	Parameters out:	None
//
// Value Returned:	
// 
// Called By:			DoGraphWUpdateEvent
//
//	Coded By:			Larry L. Biehl			Date: 12/09/1991
//	Revised By:			Larry L. Biehl			Date: 03/27/2018

void CMGraphView::DrawGraphGrowIcon	(
				GraphPtr								graphRecordPtr)

{	
	Str31									tempString;
	
	SInt16								left,
											textWidth,
											top;
											
	Boolean								printCopyFlag;
									
	#if defined multispec_win
		CRect					graphWindowRect;
	#endif	// defined multispec_win 


	#if defined multispec_win
		USES_CONVERSION;
	#endif

	if (graphRecordPtr != NULL)
		{			
		#if defined multispec_mac
			printCopyFlag = FALSE;
			if (gProcessorCode == kPrintProcessor)
				printCopyFlag = TRUE;
	
			GetWindowPortBounds (graphRecordPtr->window, &gTempRect);
			
			gTempRect.top = gTempRect.bottom - 15;
		#endif	// defined multispec_win 		

		#if defined multispec_win
			printCopyFlag = m_printCopyModeFlag;
			
			GetWindowRect (graphWindowRect);
			
			//graphWindowRect.top = graphWindowRect.bottom - 15;
		#endif	// defined multispec_win
			
		if (!printCopyFlag && graphRecordPtr->numberSets > 1)
			{
					// Draw the graph set control prompt text.
														
			if (graphRecordPtr->setCode == 1)
				sprintf ((CharPtr)tempString,
								"Change Channel:");
				
			else	// graphRecordPtr->setCode == 2
				sprintf ((CharPtr)tempString,
								"Change Feature:");
					
			textWidth = TextWidth (tempString, 0, 15);
				          
			#if defined multispec_mac 
				ClipRect (&gTempRect); 
					
				left = gTempRect.left + 95 - textWidth - 1;
				top  = gTempRect.bottom - 4;  
				
				MoveTo (left, top);
				DrawText (tempString, 0, 15);
			#endif	// defined multispec_mac 

			#if defined multispec_win	
				UINT	savedTextAlign;

				left = 95 - textWidth - 1;
				top  = (SInt16)(graphWindowRect.bottom - graphWindowRect.top - 10);	// - 7
				SetBackgroundColor (graphRecordPtr->pDC, 192);
				savedTextAlign = graphRecordPtr->pDC->GetTextAlign ();
				graphRecordPtr->pDC->SetTextAlign (TA_BASELINE);
				graphRecordPtr->pDC->SetTextColor (RGB (0,0,0));
				graphRecordPtr->pDC->TextOut (left, 
															top, 
															(LPCTSTR)A2T((char*)tempString), 
															15);
				graphRecordPtr->pDC->SetTextAlign (savedTextAlign);
			#endif	// defined multispec_win 
			
			}	// end "if (!printCopyFlag && ..."	
			
		if (graphRecordPtr->flag & NU_HISTOGRAM_PLOT)
			{
					// Draw the graph set control prompt text.	

			ListBinWidthValue (graphRecordPtr, !printCopyFlag);
			
			if (!printCopyFlag)
				{
					// Draw a line along the bottom of the graph window 
					// just above the controls.
			
				#if defined multispec_mac 
					MoveTo (gTempRect.left, gTempRect.bottom-kSBarWidth);
					LineTo (gTempRect.right, gTempRect.bottom-kSBarWidth);
				#endif	// defined multispec_win 		
				#if defined multispec_win		
					graphRecordPtr->pDC->MoveTo (0, 
							graphRecordPtr->clientRect.bottom-kSBarWidth-7);	// -2
					graphRecordPtr->pDC->LineTo (
							graphRecordPtr->clientRect.right, 
							graphRecordPtr->clientRect.bottom-kSBarWidth-7);	// -2
				#endif	// defined multispec_win 

				}	// end "if (!printCopyFlag)"
			
			}	// end "if (graphRecordPtr->flag & NU_HISTOGRAM_PLOT)"	
				
		#if defined multispec_mac
			gTempRect.left = gTempRect.right - 15;
			ClipRect (&gTempRect);
				
			DrawGrowIcon (graphRecordPtr->window);
		#endif	// defined multispec_mac 
			
		}	// end "if (graphRecPtr != NULL)"
		
}	// end DrawGraphGrowIcon 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean FinishGraphRecordSetUp
//
//	Software purpose:	
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: ??/??/????
//	Revised By:			Larry L. Biehl			Date: 04/04/2018
  
Boolean CMGraphView::FinishGraphRecordSetUp (
				SInt16*								channelListPtr,
				SInt32								vectorLength,
				SInt32								numberYVectors,
				SInt32								numberSets,
				UInt16								xValueType,
				UInt16								yValueType)

{  
	GraphPtr								graphRecordPtr;
	WindowInfoPtr						windowInfoPtr;
	
	Handle								windowInfoHandle;
	
	Boolean								continueFlag = FALSE;
	
	SignedByte							handleStatus;
	
	                             
	graphRecordPtr = (GraphPtr)GetHandleStatusAndPointer (m_graphRecordHandle,
																				&handleStatus);
   
	if (graphRecordPtr != NULL)
		{ 						                       
		if (gProcessorCode == kBiPlotDataProcessor)
			{
			graphRecordPtr->flag = NU_SCATTER_PLOT;
			
					// Set the graph codes available for the biplot graphs.
														
			graphRecordPtr->graphCodesAvailable = 0;
			
			if (gBiPlotDataSpecsPtr->displayPixelCode & kDisplayPixels)
				graphRecordPtr->graphCodesAvailable += 1;
			
			if (gBiPlotDataSpecsPtr->outlineClassCode & kOutlineClasses)
				graphRecordPtr->graphCodesAvailable += 2;
			
			}	// end "if (gProcessorCode == kBiPlotDataProcessor)"
			
		else if (gProcessorCode == kHistogramStatsProcessor)
			{
			
			}	// end "else if (gProcessorCode == kHistogramStatsProcessor)"
			
		else	// Must be a response vs wavelength type of graph
			{
			if (vectorLength <= 1)
				graphRecordPtr->flag = NU_SCATTER_PLOT;
				
			else	// vectorLength > 1
				graphRecordPtr->flag = NU_LINE_PLOT;
														
			graphRecordPtr->graphCodesAvailable = 1;
			
			graphRecordPtr->imageWindow = gActiveImageWindow;

					// For now assume the min and max y-axis values are the limits
					// for the possible data. This will be adjusted later for the
					// data actually being used.
			
			windowInfoHandle = GetWindowInfoHandle (graphRecordPtr->imageWindow);
			windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
			
			graphRecordPtr->yMin = windowInfoPtr->minUsableDataValue;
			graphRecordPtr->yMax = windowInfoPtr->maxUsableDataValue;
		
			graphRecordPtr->yScaleMin = windowInfoPtr->minUsableDataValue;
			graphRecordPtr->yScaleMax = windowInfoPtr->maxUsableDataValue;
			
			}	// end "else Must be a response vs wavelength type of graph"
			
		graphRecordPtr->numberVectors = (SInt16)numberYVectors; 
			
		graphRecordPtr->numberSets = (SInt16)numberSets; 
		
		continueFlag = CheckGraphVectorsSize (graphRecordPtr, 
															vectorLength, 
															numberYVectors,
															numberSets);	
			
		graphRecordPtr->xValueTypeCode = xValueType;
		graphRecordPtr->yValueTypeCode = yValueType;											
			
		if (continueFlag)
			{
			if (gProcessorCode == kListDataProcessor)
				continueFlag =
							LoadGraphXVector (graphRecordPtr, channelListPtr, vectorLength);
			
			else if (gProcessorCode == kSelectionGraphProcessor)
				{
						// Force IO memory to be checked in the ShowGraphWindowSelection
						// routine
				
				Handle selectionIOInfoHandle =
											gSelectionGraphViewCPtr->GetSelectionIOHandle ();
				SelectionIOInfoPtr selectionIOInfoPtr =
							(SelectionIOInfoPtr)GetHandlePointer (selectionIOInfoHandle);
				
				if (selectionIOInfoPtr != NULL)
					selectionIOInfoPtr->checkIOMemoryFlag = TRUE;
				
				}	// end "if (continueFlag &&gProcessorCode == kSelectionGraphProcessor"
			
			}	// end "if (continueFlag)"
			
		if (continueFlag)
			LoadGraphSupportArrays (graphRecordPtr, vectorLength);
			
				// Make certain that there is a minimum amount of memory left.			
					
		if (continueFlag)
			continueFlag = CheckIfMemoryAvailable (5000);  
	
				// Initialize the member window variable for the Mac version.
					
		m_window = graphRecordPtr->window;          
			
		}	// end "if (graphRecordPtr != NULL)"
		
	MHSetState (m_graphRecordHandle, handleStatus);
		
	return (continueFlag);
	
}	// end "FinishGraphRecordSetUp"


#if defined multispec_win
CMGraphDoc* CMGraphView::GetDocument ()

{                                                           
	return (CMGraphDoc*)m_pDocument;
	
}	// end "GetDocument"
#endif	// defined multispec_win


void CMGraphView::IntializeGraphRecord (void)

{  
	GraphPtr 		graphRecordPtr;
	                             
	
	graphRecordPtr = (GraphPtr)GetHandlePointer (m_graphRecordHandle);
   
	if (graphRecordPtr != NULL)
		{ 						
		graphRecordPtr->title[0] = 0;    
		graphRecordPtr->title2[0] = 0; 
		graphRecordPtr->xLabel[0] = 0; 
		graphRecordPtr->yLabel[0] = 0;
		graphRecordPtr->channelDescriptionUnitString[0] = 0;
		
		graphRecordPtr->xVector.numberPoints = 0; 
		graphRecordPtr->xVector.size = 0; 
		graphRecordPtr->xVector.basePtr = NULL;
		graphRecordPtr->xVector.baseHandle = NULL;  
		
		graphRecordPtr->yVector.numberPoints = 0; 
		graphRecordPtr->yVector.size = 0; 
		graphRecordPtr->yVector.basePtr = NULL;
		graphRecordPtr->yVector.baseHandle = NULL;
		
		graphRecordPtr->clientRect.top = 0;
		graphRecordPtr->clientRect.left = 0;
		graphRecordPtr->clientRect.bottom = 0;
		graphRecordPtr->clientRect.right = 0;

		graphRecordPtr->textScaling = 1;
		
		graphRecordPtr->xDataMin = 0;                      
		graphRecordPtr->xDataMax = 0;
		
		graphRecordPtr->xEllipseMin = 0;                      
		graphRecordPtr->xEllipseMax = 0;
		
		graphRecordPtr->xMin = 0;                      
		graphRecordPtr->xMax = 0;
		
		graphRecordPtr->xReflectiveDataMin = 0;
		graphRecordPtr->xReflectiveDataMax = 0;
		
		graphRecordPtr->xScaleMin = 0;
		graphRecordPtr->xScaleMax = 0;
		
		graphRecordPtr->xThermalDataMin = 0;
		graphRecordPtr->xThermalDataMax = 0;
		
		graphRecordPtr->yDataMin = 0;                      
		graphRecordPtr->yDataMax = 0;
		
		graphRecordPtr->yEllipseMin = 0;                      
		graphRecordPtr->yEllipseMax = 0;
		
		graphRecordPtr->yMin = 0;                      
		graphRecordPtr->yMax = 0;
		
		graphRecordPtr->yScaleMin = 0;
		graphRecordPtr->yScaleMax = 0;
		
		graphRecordPtr->classThresholdValue = 0;
		
		graphRecordPtr->xInt = 1;
		graphRecordPtr->yInt = 20;
		
		graphRecordPtr->histogramBinWidth = 1;
		graphRecordPtr->origHistogramBinWidth = 1;
		
		graphRecordPtr->pDC = NULL;
		graphRecordPtr->window = NULL; 
		graphRecordPtr->imageWindow = NULL;
		graphRecordPtr->lastActiveImageWindow = NULL;
		
		graphRecordPtr->xAxisPopupControlHandle = NULL;
		
		#if defined multispec_mac
			graphRecordPtr->imageViewCPtr = NULL;
			graphRecordPtr->backgroundPatternPtr = NULL;
		#endif	// defined multispec_mac 
		 
		#if defined multispec_win
			graphRecordPtr->imageViewCPtr = gActiveImageViewCPtr;
			graphRecordPtr->backgroundPatternPtr = (Ptr)&CMGraphView::s_grayBrush;
		#endif	// defined multispec_win 
		
		graphRecordPtr->graphViewCPtr = this;
		
		graphRecordPtr->classStatisticsHandle = NULL;
		graphRecordPtr->histogramBinWidthHandle = NULL;
		graphRecordPtr->setListHandle = NULL;
		graphRecordPtr->vectorDisplayHandle = NULL;
		graphRecordPtr->vectorLabelHandle = NULL;
		graphRecordPtr->vectorLengthsHandle = NULL;
		graphRecordPtr->vectorPaletteColorHandle = NULL;
		graphRecordPtr->vectorSymbolHandle = NULL;
		graphRecordPtr->xVectorDataHandle = NULL;
			
		graphRecordPtr->imageLineColRect.top = 0;                                   
		graphRecordPtr->imageLineColRect.bottom = 0;                                   
		graphRecordPtr->imageLineColRect.left = 0;                                   
		graphRecordPtr->imageLineColRect.right = 0;
		
		graphRecordPtr->classStatsIncrement = 1;
		graphRecordPtr->attrb = NU_GRID;
		graphRecordPtr->descriptionCode = 0;
		graphRecordPtr->drawGraphCode = (SInt16)0x8000;
		
		graphRecordPtr->leftInset = 0;                      
		graphRecordPtr->topInset = 0;
		graphRecordPtr->rightInset = 0;                      
		graphRecordPtr->bottomInset = 0;
		
		graphRecordPtr->flag = NU_LINE_PLOT;
		graphRecordPtr->graphCodesAvailable = 0;
		graphRecordPtr->histogramBinCode = -1;
		graphRecordPtr->numberStatisticsChannels = 0;
		graphRecordPtr->numberSets = 1;
		graphRecordPtr->numberVectors = 1;
		graphRecordPtr->processorCode = gProcessorCode;
		graphRecordPtr->selectionTypeCode = 0;
			
		graphRecordPtr->set = 1;
		graphRecordPtr->setCode = 0;
		
		graphRecordPtr->textSize = 0;
		//graphRecordPtr->textWidth = 0;
		graphRecordPtr->xAxisCode = kChannels;
		
		graphRecordPtr->xESignificantDigits = 5;
		graphRecordPtr->xFSignificantDigits = 5;
		graphRecordPtr->yESignificantDigits = 5; 
		graphRecordPtr->yFSignificantDigits = 5;  
		
		graphRecordPtr->xTicLabelWidth = 0;
		graphRecordPtr->yTicLabelWidth = 39;    
		
		graphRecordPtr->xValueTypeCode = kIntegerType;
		graphRecordPtr->yValueTypeCode = kIntegerType;
		
		graphRecordPtr->graphWindowFlag = FALSE;
		
		}	// end "if (graphRecordPtr != NULL)"
	
}	// end "IntializeGraphRecord" 


#if defined multispec_win
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
			left = 95;  
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
#endif	// defined multispec_win
  
#if defined multispec_win
BEGIN_MESSAGE_MAP (CMGraphView, CFormView)
	//{{AFX_MSG_MAP (CMGraphView)
	ON_WM_CREATE ()
	ON_UPDATE_COMMAND_UI (ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
	ON_WM_DESTROY ()
	ON_UPDATE_COMMAND_UI (ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND (ID_EDIT_COPY, OnEditCopy)
	ON_BN_CLICKED (IDC_NextChannel, OnNextChannel)
	ON_BN_CLICKED (IDC_PreviousChannel, OnPreviousChannel)
	ON_BN_CLICKED (IDC_SelectVectors, OnLines)
	ON_CBN_DROPDOWN (IDC_xAxis, OnDropdownXAxisDropList) 
	ON_CBN_SELENDOK (IDC_xAxis, OnSelendokXAxisDropList)
	ON_WM_SIZE ()
	//}}AFX_MSG_MAP
//	ON_COMMAND_RANGE (IDC_NextChannel, IDC_NextChannel, OnNextChannel)
//	ON_COMMAND_RANGE (IDC_PreviousChannel, IDC_PreviousChannel, OnPreviousChannel)
END_MESSAGE_MAP ()
#endif	// defined multispec_win



#if defined multispec_win
int CMGraphView::OnCreate (
				LPCREATESTRUCT					lpCreateStruct)

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
				CDC*								pDC)

{
	SignedByte							handleStatus;
	 									
	LOGFONT								logfont;
	
	CFont									font;
	
	CFont*								pOldFont;

	UINT									savedTextAlign;

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

		logfont.lfHeight = (int)(10 * graphRecordPtr->textScaling);   
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
	CComboBox*				xAxisComboBoxPtr;
	GraphPtr					graphRecordPtr;

	graphRecordPtr = (GraphPtr)GetHandlePointer (m_graphRecordHandle);
	xAxisComboBoxPtr = (CComboBox*)GetDlgItem (IDC_xAxis);
	SetUpXAxisPopUpMenu (graphRecordPtr, (MenuHandle)xAxisComboBoxPtr);
	                               
}	// end "OnDropdownXAxisDropList" 



void CMGraphView::OnInitialUpdate (void)

{
	CComboBox*							xAxisComboBoxPtr;

	HBITMAP								bitmapHandle;

	Boolean								continueFlag = TRUE,
											newWindowFlag = FALSE;
	                                 
		                       
	                                  
	CMGraphDoc* graphDocCPtr = GetDocument ();
	graphDocCPtr->SetGraphViewCPtr (this); 
	
			// Force the window frame to be the same size as the graphics view.
	
	GetParentFrame ()->RecalcLayout ();
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
			
	SetXAxisDescriptionInfo (graphRecordPtr, gImageFileInfoPtr);
										
			// Get the labels for the graph.											
				
	GetGraphLabels (graphRecordPtr);
	                                 
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
		
		}	// end "if (gProcessorCode == kSelectionGraphProcessor)"
	                                 
	else if (gProcessorCode == kHistogramStatsProcessor)
		{
		newWindowFlag = TRUE;

		bitmapHandle = (HBITMAP)LoadImage (AfxGetInstanceHandle (),
														(LPCTSTR)_T("IDB_Vectors"), 
														IMAGE_BITMAP, 
														0, 
														0, 
														LR_DEFAULTCOLOR);
		if (bitmapHandle != NULL)
				m_selectVectors.SetBitmap (bitmapHandle);

		bitmapHandle = (HBITMAP)LoadImage (AfxGetInstanceHandle (),
														(LPCTSTR)_T("IDB_GraphOverlay"), 
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



void CMGraphView::OnUpdateFileSaveAs (
				CCmdUI*								pCmdUI)
				
{                                                                                                                     
	pCmdUI->Enable (FALSE);
	
}	// end "OnUpdateFileSaveAs"



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


/////////////////////////////////////////////////////////////////////////////
// CMGraphView printing

BOOL CMGraphView::OnPreparePrinting (
				CPrintInfo							*pInfo)

{
	// default preparation
	return DoPreparePrinting (pInfo);
}
                                                                         

                                                                             
void CMGraphView::OnPrint (CDC* pDC, CPrintInfo* pInfo)

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
			
	double magWidth = (double)pageWidth/
												(sourceRect.right - sourceRect.left + 1);
			
	double magHeight = (double)pageHeight/
												(sourceRect.bottom - sourceRect.top + 1);
												
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



void CMGraphView::OnUpdateEditCopy (CCmdUI* pCmdUI)
{
	UpdateEditGraphicsCopy (pCmdUI);
	
	pCmdUI->Enable (TRUE);
	
}	// end "OnUpdateEditCopy"



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

   AfxGetApp ()->m_pMainWnd->OpenClipboard ();
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



void CMGraphView::OnLines () 

{
			// Add your control notification handler code here
	
}	// end "OnLines"



void CMGraphView::OnNextChannel ()

{
	Boolean				returnFlag;


	if (((CMultiSpecApp*)AfxGetApp ())->GetZoomCode () == IDC_NextChannel)
		{
		DoNextOrPreviousChannel	(this, kNextGraphSetControl);
		
		returnFlag = UpdateGraphControls ();

		if (!returnFlag)
			{
			ReleaseCapture ();                                                                
			((CMultiSpecApp*)AfxGetApp ())->SetZoomCode (0);      
			((CMultiSpecApp*)AfxGetApp ())->SetControlDelayFlag (TRUE);

			}	// "if (!returnFlag)"

		}	// end "if (((CMultiSpecApp*)AfxGetApp ())->GetZoomCode () == .."
	
}	// end "OnNextChannel"



void CMGraphView::OnPreviousChannel ()

{              
	Boolean				returnFlag;
	

	if (((CMultiSpecApp*)AfxGetApp ())->GetZoomCode () == IDC_PreviousChannel)
		DoNextOrPreviousChannel	(this, kPreviousGraphSetControl);
		
	returnFlag = UpdateGraphControls ();
	if (!returnFlag)
		{
		ReleaseCapture ();                                                                
		((CMultiSpecApp*)AfxGetApp ())->SetZoomCode (0);      
		((CMultiSpecApp*)AfxGetApp ())->SetControlDelayFlag (TRUE);

		}	// end "if (!returnFlag)"
	
}	// end "OnPreviousChannel"



void CMGraphView::OnSelendokXAxisDropList ()

{  
	CComboBox* 			comboBoxPtr;
	GraphPtr				graphRecordPtr;

	Handle				graphRecordHandle;
	
	int					currentSelection,
							xAxisSelection;
	                  
	Boolean				returnFlag = TRUE;
	
	
	comboBoxPtr = (CComboBox*)(GetDlgItem (IDC_xAxis));
	if (comboBoxPtr != NULL)
		{                               
		DDX_CBIndex (m_dialogFromPtr, IDC_xAxis, m_xAxisSelection); 
		xAxisSelection = comboBoxPtr->GetItemData (m_xAxisSelection);
		//xAxisSelection = m_xAxisSelection + 1;

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

			#if defined multispec_win
				if (graphRecordPtr->processorCode == kListDataProcessor)
					graphRecordPtr->graphViewCPtr->Invalidate ();	 
			#endif	// defined multispec_win

			}	// end "if (errCode == noErr && ..."

		}	// end "if (comboBoxPtr != NULL)"
	
}		// end "OnSelendokXAxisDropList"



void CMGraphView::OnSize (
				UINT									nType, 
				int									cx, 
				int									cy)

{
	CFormView::OnSize (nType, cx, cy);
	
	UpdateGraphScales (GetGraphRecordHandle ());	
	                       
	MoveGraphControls ();

}	// end "OnSize"  
#endif	// defined multispec_win



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetCheckIOMemoryFlag
//
//	Software purpose:	The purpose of this routine is to set the checkIOMemory
//							flag in the selectionIOInfo structure.
//
//	Parameters in:		value to set the flag to.
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 08/22/1995
//	Revised By:			Larry L. Biehl			Date: 08/22/1995

void CMGraphView::SetCheckIOMemoryFlag (
				Boolean								flagSetting)
				
{				
	if (s_selectionIOInfoHandle != NULL)
		{
		SelectionIOInfoPtr selectionIOInfoPtr = 
						(SelectionIOInfoPtr)GetHandlePointer (s_selectionIOInfoHandle);
											
		selectionIOInfoPtr->checkIOMemoryFlag = flagSetting;

		}	// end "if (s_selectionIOInfoHandle != NULL)"
	
}	// end "SetCheckIOMemoryFlag"


#if defined multispec_win
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
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



/////////////////////////////////////////////////////////////////////////////
// CMGraghViewButton                                                        

BEGIN_MESSAGE_MAP (CMGraphViewButton, CButton)
	//{{AFX_MSG_MAP (CMGraphViewButton)
	ON_WM_LBUTTONDOWN ()
	ON_WM_MOUSEMOVE ()
	ON_WM_LBUTTONUP ()
	ON_WM_KEYDOWN ()
	ON_WM_KEYUP ()
	ON_WM_RBUTTONDOWN ()
	ON_WM_RBUTTONUP ()   
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE (ID_GRAPHOVERLAYMENUITEMSTART, ID_GRAPHOVERLAYMENUITEMEND, OnLines2)
	ON_COMMAND_RANGE (ID_SELECTVECTORMENUITEMSTART, ID_SELECTVECTORMENUITEMEND, OnSelectVector)
	ON_COMMAND_RANGE (ID_SELECTBINWIDTHMENUITEMSTART, ID_SELECTBINWIDTHMENUITEMEND, OnBinWidth)
END_MESSAGE_MAP () 
                 
                 
/////////////////////////////////////////////////////////////////////////////
// CMGraghViewButton construction/destruction 

CMGraphViewButton::CMGraphViewButton ()

{
	m_buttonID = 0;
	m_graphViewCPtr = NULL;

}	// end "CMGraphViewButton" 



CMGraphViewButton::~CMGraphViewButton ()
{                                   
	
}	// end "~CMButton"



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

 

/////////////////////////////////////////////////////////////////////////////
// CMButton message handlers 

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
			((CMultiSpecApp*)AfxGetApp ())->SetZoomCode (m_buttonID);
			((CMultiSpecApp*)AfxGetApp ())->SetControlDelayFlag (TRUE); 
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

		((CMultiSpecApp*)AfxGetApp ())->SetZoomCode (m_buttonID);
		
		captured = GetCapture ();

		if (graphOverlayMenu->TrackPopupMenu (TPM_LEFTALIGN, 
															buttonRectangle.right, 
															buttonRectangle.top, 
															this, 
															NULL))
			{
			captured = GetCapture ();
			OnLButtonUp (nFlags, point);

			}

		captured = GetCapture ();

		ReleaseCapture ();
		((CMultiSpecApp*)AfxGetApp ())->SetZoomCode (0);
		((CMultiSpecApp*)AfxGetApp ())->SetControlDelayFlag (TRUE);

		}	// end "else if (m_buttonID == IDC_GraphOverlay)"

	else if (m_buttonID == IDC_SelectVectors)
		{
		CMenu*			selectVectorMenu;
		tagRECT			buttonRectangle;

	
		GetWindowRect (&buttonRectangle);

		selectVectorMenu = m_graphViewCPtr->GetSelectVectorPopupMenu ();

		SetUpVectorPopUpMenu (m_graphViewCPtr, (MenuHandle)selectVectorMenu);

		((CMultiSpecApp*)AfxGetApp ())->SetZoomCode (m_buttonID);
		
		captured = GetCapture ();

		if (selectVectorMenu->TrackPopupMenu (TPM_LEFTALIGN, 
															buttonRectangle.right, 
															buttonRectangle.top, 
															this, 
															NULL))
			{
			captured = GetCapture ();
			OnLButtonUp (nFlags, point);

			}

		captured = GetCapture ();

		ReleaseCapture ();
		((CMultiSpecApp*)AfxGetApp ())->SetZoomCode (0);
		((CMultiSpecApp*)AfxGetApp ())->SetControlDelayFlag (TRUE);
			

		}	// end "else if (m_buttonID == IDC_SelectVectors)"

	else if (m_buttonID == IDC_BinWidth)
		{
		CMenu*			binWidthMenu;
		tagRECT			buttonRectangle;

	
		GetWindowRect (&buttonRectangle);

		binWidthMenu = m_graphViewCPtr->GetBinWidthPopupMenu ();

		SetUpBinWidthPopUpMenu (m_graphViewCPtr, (MenuHandle)binWidthMenu);

		((CMultiSpecApp*)AfxGetApp ())->SetZoomCode (m_buttonID);
		
		captured = GetCapture ();

		if (binWidthMenu->TrackPopupMenu (TPM_LEFTALIGN, 
														buttonRectangle.right, 
														buttonRectangle.top, 
														this, 
														NULL))
			{
			captured = GetCapture ();
			OnLButtonUp (nFlags, point);

			}

		captured = GetCapture ();

		ReleaseCapture ();
		((CMultiSpecApp*)AfxGetApp ())->SetZoomCode (0);
		((CMultiSpecApp*)AfxGetApp ())->SetControlDelayFlag (TRUE);
			
		}	// end "else if (m_buttonID == IDC_BinWidth)"

}	// end "OnLButtonDown"



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
			                                            
	//((CMultiSpecApp*)AfxGetApp ())->SetZoomCode (itemID);
	
}	// end "OnMouseMove" 



void CMGraphViewButton::OnLButtonUp (
				UINT									nFlags,
				CPoint								point)
{
	CWnd*									captured;

	//if (m_buttonID == IDC_GraphOverlay)
	//	{
	//
	//	}	// end "if (m_buttonID == IDC_GraphOverlay)"
		
	captured = GetCapture ();
	if (GetCapture () == this)
		{
		ReleaseCapture ();                                                                
		((CMultiSpecApp*)AfxGetApp ())->SetZoomCode (0);
		((CMultiSpecApp*)AfxGetApp ())->SetControlDelayFlag (TRUE);
		
		}	// end "if (GetCapture () == this)"
		
	CButton::OnLButtonUp (nFlags, point);
	
}	// end "OnLButtonUp" 

                                 

void CMGraphViewButton::OnKeyDown (
				UINT									nChar,
				UINT									nRepCnt,
				UINT									nFlags)
{     
	if (nChar == 0x10)
		{                
		//CMLegendView* legendViewCPtr = (CMLegendView*)GetParent ();
		//legendViewCPtr->GetLegendListCPtr ()->CheckShiftKeyDown ();
		
		}	// end "if (nChar == 0x10)"
	                                                                                                                                                                                        
	else if (nChar == 0x11)           
		//gActiveImageViewCPtr->SetControlKeyFlag (TRUE);
	
	CButton::OnKeyDown (nChar, nRepCnt, nFlags);
	
}	// end "OnKeyDown"



void CMGraphViewButton::OnKeyUp (
				UINT									nChar,
				UINT									nRepCnt,
				UINT									nFlags)
{    
	if (nChar == 0x10)
		{                
		//CMLegendView* legendViewCPtr = (CMLegendView*)GetParent ();
		//legendViewCPtr->GetLegendListCPtr ()->CheckShiftKeyUp ();
		
		}	// end "if (nChar == 0x10)"  
	                                                                                                                                                                                        
	else if (nChar == 0x11)           
		//gActiveImageViewCPtr->SetControlKeyFlag (FALSE);
	
	CButton::OnKeyUp (nChar, nRepCnt, nFlags);
	
}	// end "OnKeyUp" 



void CMGraphViewButton::OnRButtonDown (
				UINT									nFlags,
				CPoint								point)

{                                                      
	
	CButton::OnRButtonDown (nFlags, point);
	                           
	if (GetCapture () == this)
		((CMultiSpecApp*)AfxGetApp ())->SetControlDelayFlag (FALSE);

}	// end "OnRButtonDown" 



void CMGraphViewButton::OnRButtonUp (
				UINT									nFlags,
				CPoint								point)
{  
	if (GetCapture () == this)                                                            
		((CMultiSpecApp*)AfxGetApp ())->SetControlDelayFlag (TRUE);
		
	CButton::OnRButtonUp (nFlags, point);
	
}	// end "OnRButtonUp"

 

void CMGraphViewButton::OnLines2 (
				UINT									menuID)
{
	SInt32								selection;


	ReleaseCapture ();

	selection = menuID - ID_GRAPHOVERLAYMENUITEMSTART + 1;
	SetOverlayDisplayList (m_graphViewCPtr, (SInt16)selection);

	m_graphViewCPtr->Invalidate ();
	
}	// end "OnLines2"

 

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
		//InvalidateGraphWindow (windowPtr, kGraphArea);
		//InvalidateGraphWindow (windowPtr, kGraphBinWidth);
		m_graphViewCPtr->Invalidate ();

		}	// end "if (SetHistogramBinWidth (m_graphViewCPtr, selection))"
	
}	// end "OnBinWidth"



/////////////////////////////////////////////////////////////////////////////
// CMGraphViewCombo                                                        

BEGIN_MESSAGE_MAP (CMGraphViewCombo, CComboBox)
	//{{AFX_MSG_MAP (CMGraphViewCombo)
	ON_WM_KEYDOWN ()  
	//}}AFX_MSG_MAP
END_MESSAGE_MAP () 
                 
                 
/////////////////////////////////////////////////////////////////////////////
// CMGraghViewButton construction/destruction 

CMGraphViewCombo::CMGraphViewCombo ()

{
	m_graphViewCPtr = NULL;

}	// end "CMGraphViewCombo" 



CMGraphViewCombo::~CMGraphViewCombo ()
{                                   
	
}	// end "~CMGraphViewCombo"

 

void CMGraphViewCombo::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
			// Add your message handler code here and/or call default

	switch (nChar)
		{
		case VK_LEFT:
			break;

		case VK_RIGHT:
 			break;  

		default:
			CComboBox::OnKeyDown(nChar, nRepCnt, nFlags);

		}	// end "switch (nChar)"

}	// end "OnKeyDown"



void CMGraphViewCombo::SetGraphViewCPtr (
				CMGraphView*						graphViewCPtr)

{                                                      
	m_graphViewCPtr = graphViewCPtr;

}	// end "SetGraphViewCPtr"


#endif	// defined multispec_win 

