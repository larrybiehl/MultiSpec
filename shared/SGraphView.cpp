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
//	Revision date:			06/29/2018
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


// === Static Member Variable ===

Handle			CMGraphView::s_selectionIOInfoHandle = NULL;
		
UInt16			CMGraphView::s_graphicsWindowCount = 0;

UInt16			CMGraphView::s_numberOfGWindows = 0;



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
//	Revised By:			Larry L. Biehl			Date: 04/12/2018

void CMGraphView::DrawGraphGrowIcon	(
				GraphPtr								graphRecordPtr)

{	
	Str31									tempString;
	
	SInt16								left,
											textWidth,
											top;
											
	Boolean								printCopyFlag;
									
	#if defined multispec_lin
		wxColour								black,
												red;
												
		red.Set (wxT("#ff0000"));
		black.Set (wxT("#000000"));
	#endif	// defined multispec_lin
									
	#if defined multispec_win
		CRect					graphWindowRect;

		USES_CONVERSION;
	#endif

	if (graphRecordPtr != NULL)
		{
		#if defined multispec_lin
			printCopyFlag = m_printCopyModeFlag;
		#endif	// defined multispec_lin
		
		#if defined multispec_mac
			printCopyFlag = FALSE;
			if (gProcessorCode == kPrintProcessor)
				printCopyFlag = TRUE;
	
			GetWindowPortBounds (graphRecordPtr->window, &gTempRect);
			
			gTempRect.top = gTempRect.bottom - 15;
		#endif	// defined multispec_mac

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
				          
			#if defined multispec_lin
				left = 105 - textWidth - 1;
				top  = (SInt16)(graphRecordPtr->clientRect.bottom -
									 graphRecordPtr->clientRect.top - 7);
				graphRecordPtr->pDC->SetBackground (wxBrush (red));
				graphRecordPtr->pDC->SetTextForeground (black);
			#endif	// defined multispec_lin
				          
			#if defined multispec_mac 
				ClipRect (&gTempRect); 
					
				left = gTempRect.left + 105 - textWidth - 1;
				top  = gTempRect.bottom - 4;  
				
				MoveTo (left, top);
				DrawText (tempString, 0, 15);
			#endif	// defined multispec_mac 

			#if defined multispec_win	
				UINT	savedTextAlign;

				left = 105 - textWidth - 1;
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
			
				#if defined multispec_lin
				#endif	// defined multispec_lin
			
				#if defined multispec_mac 
					MoveTo (gTempRect.left, gTempRect.bottom-kSBarWidth);
					LineTo (gTempRect.right, gTempRect.bottom-kSBarWidth);
				#endif	// defined multispec_mac
				
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
//	Revised By:			Larry L. Biehl			Date: 07/02/2018
  
Boolean CMGraphView::FinishGraphRecordSetUp (
				SInt16*								channelListPtr,
				SInt32								channelListLength,
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
				{
						// Implementation for subsets of channels not finished. Not
						// this feature is really needed.
				/*
				SInt16* channelWavelengthOrderListPtr =
											&channelListPtr[windowInfoPtr->totalNumberChannels];
				continueFlag = LoadGraphXVector (graphRecordPtr,
															channelListPtr,
															channelListLength,
															vectorLength,
															channelWavelengthOrderListPtr);
				*/
				continueFlag = LoadGraphXVector (graphRecordPtr,
															NULL,
															channelListLength,
															vectorLength,
															NULL);
				
				}	// end "if (gProcessorCode == kListDataProcessor)"
			
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
		 
		#if defined multispec_lin || defined multispec_win
			graphRecordPtr->imageViewCPtr = gActiveImageViewCPtr;
			graphRecordPtr->backgroundPatternPtr = (Ptr)&CMGraphView::s_grayBrush;
		#endif	// defined multispec_lin || ...
		
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
		
		graphRecordPtr->channelsInWavelengthOrderFlag = TRUE;
		graphRecordPtr->graphWindowFlag = FALSE;
		
		}	// end "if (graphRecordPtr != NULL)"
	
}	// end "IntializeGraphRecord" 



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

