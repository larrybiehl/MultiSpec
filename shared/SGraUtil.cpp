//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//
//	File:						SGraUtil.c
//
//	Functions in file:	Boolean 				CheckGraphVectorsSize
//								Boolean 				CheckSomeGraphWindowEvents
//								void 					CopyGraphYVector
//								Boolean				CreateGraph
//								CMGraphView* 		CreateGraphWindow
//								void 					DisposeOfGraphRecordMemory
//								void 					DoGraphWActivateEvent
//								void 					DoGraphWMouseDnContent
//								Boolean 				DoGraphWiCheckSomeGraphWindowEventsndowEdit
//								void					DoGraphWindowGrow
//								void 					DoGraphWUpdateEvent
//								Boolean				DrawGraph
//								void					DrawGraphGrowIcon
//								void					DrawGraphLabels
//								void					DrawGraphTitle
//								void					ForceGraphCodeResourceLoad
//								GraphPtr				GetGraphRecordPtr
//								void					GetGraphLabels
//								void					GetGraphTitle
//								Handle 				GetGraphRecordHandle
//								void 					LoadGraphSupportArrays
//								Boolean				LoadGraphXVector
//								void					LoadGraphYVector
//								void					LoadGraphYVector2
//								void 					SetGraphWindowTitle
//								void 					UpdateGraphControls
//
//	Brief description:	The routines in this file are used to handle Graph
//								windows in MultiSpec.
//
//	Diagram of MultiSpec routine calls for the routines in the file.
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
//	Language:				C
//
//	System:					Macintosh Operating System
//
//	Authors:					Larry L. Biehl
//
//	Revision number:		3.0
//
//	Revision date:			06/22/2017
//
//------------------------------------------------------------------------------------

#include "SMulSpec.h"

#if defined multispec_lin  
	#include	"LGraphDoc.h" 
	#include	"LGraphFrame.h"
	#include	"LGraphView.h"  
	#include "LMultiSpec.h" 
   #include "SGraphic.h"
   //#include "SProtype.h"
   #include <wx/evtloop.h>
   
						
	extern void		GetGraphWindowTitle (
							WindowPtr							windowPtr,
							UCharPtr								titleStringPtr);
#endif	// defined multispec_lin
	
#if defined multispec_mac
	#include	"SGrafVew.h"   
#endif	// defined multispec_mac    
                            
#if defined multispec_win                 
//	#include	"CDatFile.h"    
	#include "WMultiSpec.h"
	#include	"SGrafVew.h" 
	#include	"WGrafDoc.h" 
	#include	"WGrafFrm.h"	 
#endif	// defined multispec_win

#include	"SExtGlob.h"

#define	kXAxis			1
#define	kYAxis			2
#define	kBothXYAxes		3	

extern void				SetGraphMinMax (
								GraphPtr								graphRecordPtr,
								SInt16								axisCode);		

extern void				UpdateGraphScales (
								Handle								graphRecordHandle);

extern double			ConvertToScientificFormat (
								double								value, 
								SInt32*								base10ExponentPtr);		

extern void				MSetGraphWindowTitle ( 
								CMGraphView*						graphViewCPtr,
								StringPtr							titleStringPtr);

extern void				GetWindowTitle (
								WindowPtr							windowPtr,
								UCharPtr								titleStringPtr);		

extern WindowPtr		GetSelectionGraphImageWindow ( 
								CMGraphView*						graphViewCPtr);


																															
			// Prototypes for routines in this file that are only called by		
			// other routines in this file.	

void			DrawGraphLabels (  
					GraphPtr								graphRecPtr);		

void 			DrawGraphLegend ( 
					GraphPtr								graphRecPtr);

void			DrawGraphTitle (  
					GraphPtr								graphRecPtr);

void			GetGraphLabels (  
					GraphPtr								graphRecPtr);

void			GetGraphTitle (
					GraphPtr								graphRecordPtr, 
					SInt32								lineStart, 
					SInt32								columnStart, 
					SInt32								lineEnd, 
					SInt32								columnEnd);

void	 		GetLikeWindowMinMax (
					WindowPtr							imageWindowPtr,
					double*								minPtr,
					double*								maxPtr);	
					
double 		GetUserHistogramBinWidth (
					GraphPtr								graphRecordPtr,
					double*								histogramBinWidthPtr,
					double								xMin,
					double								xMax);

void 			GraphWControlEvent (
					ControlHandle						theControl, 
					WindowPtr							windowPtr, 
					Point									location, 
					SInt16								partCode);
					
void 			InvalidateGraphWindow (
					WindowPtr							windowPtr, 
					SInt16								areaCode);	

void 			SetGraphMinMax (
					Handle								windowInfoHandle,
					SInt16								axisCode);			

//void 			SetGraphScales (
//					GraphPtr								graphRecordPtr);		

void 			SetXGraphScale (
					GraphPtr								graphRecordPtr);		

void 			SetYGraphScale (
					GraphPtr								graphRecordPtr);
					
void 			SetLikeWindowMinMax (
					WindowPtr							imageWindowPtr,
					double								min,
					double								max,
					double								interval);
 


//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CheckGraphVectorsSize
//
//	Software purpose:	The purpose of this routine is to check the size of
//							the input graph x and y vectors to be certain that they
//							are large enough for the requested input size.  If they
//							are not large enough, an attempt is made to make them
//							the requested size.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			CMGraphView::FinishGraphRecordSetUp in SGrafVew.cpp
//							ShowGraphWindowSelection in SSelGraf.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/09/1991
//	Revised By:			Larry L. Biehl			Date: 02/08/2006			

Boolean CheckGraphVectorsSize (
				GraphPtr								graphRecordPtr, 
				SInt32								requestedLength, 
				SInt32								numberVectors,
				SInt32								numberSets)

{
	Ptr									ptr;
	
	SInt32								error,
											numberBytes,
											numberStatVariables,
											xVectorSize,
											yVectorSize;
									
	Boolean								changedFlag,
											continueFlag;
	

			// Initialize local variables.											
			
	continueFlag = FALSE;
	error = 0;

			// Check input variables.													
			
	if (graphRecordPtr != NULL)
		{
		continueFlag = TRUE;
		
		if (gProcessorCode == kBiPlotDataProcessor)
			{
			xVectorSize = requestedLength;
			yVectorSize = requestedLength;
			
			}		// end "if (gProcessorCode == kBiPlotDataProcessor)"
			
		else if (gProcessorCode == kHistogramStatsProcessor)
			{
			xVectorSize = graphRecordPtr->xVector.size;
			yVectorSize = graphRecordPtr->xVector.size;
			
			graphRecordPtr->xVector.size = 0;
			
			}		// end "else if (gProcessorCode == kHistogramStatsProcessor)" 
			
		else		// gProcessorCode != kBiPlotDataProcessor 
			{
					// For x vector, allow for channel number and				
					// wavelength.															
					
			xVectorSize = requestedLength * 2;
			yVectorSize = requestedLength * numberVectors;
			
			}		// end "else gProcessorCode != kBiPlotDataProcessor" 
			
		AllocateV( &graphRecordPtr->xVector, xVectorSize, &error);
		
		if ( !error )
			AllocateV( &graphRecordPtr->yVector, yVectorSize, &error);
			
		continueFlag = (error == noErr);
		
		if (continueFlag)
			{
					// Get memory for the rest of the vectors.					
			
			numberBytes = 0;		
			if (gProcessorCode == kBiPlotDataProcessor && 
						gProjectInfoPtr != NULL &&
							(gBiPlotDataSpecsPtr->outlineClassCode & kOutlineClasses) )
				{
				numberBytes = (numberVectors-1) * sizeof(double) *
											(5 + 3 * gProjectInfoPtr->numberStatisticsChannels);
							
				}		// end "if (gProcessorCode == kBiPlotDataProcessor..."					
					
			else if (graphRecordPtr->flag == NU_HISTOGRAM_PLOT)
				{
				numberStatVariables = 0;
				
				if (graphRecordPtr->graphCodesAvailable & 0x0004)
					numberStatVariables = 2;
					
				if (graphRecordPtr->graphCodesAvailable & 0x0008)
					numberStatVariables = 3;
					
				if (graphRecordPtr->graphCodesAvailable & 0x0010)
					numberStatVariables = 5;
					
				numberBytes = 
						numberSets * numberVectors * numberStatVariables * sizeof(double);
							
				}		// end "if (continueFlag && ...->flag == NU_HISTOGRAM_PLOT" 
				
			if (numberBytes > 0)	
				ptr = CheckHandleSize (
							&graphRecordPtr->classStatisticsHandle,
							&continueFlag, 
							&changedFlag, 
							numberBytes );
					
			if (continueFlag)	
				ptr = CheckHandleSize (
							&graphRecordPtr->vectorDisplayHandle,
							&continueFlag, 
							&changedFlag, 
							numberVectors * sizeof(char) );
							
			if (continueFlag && 
						graphRecordPtr->flag == NU_HISTOGRAM_PLOT &&
									numberVectors > 1)
				ptr = CheckHandleSize (
							&graphRecordPtr->vectorLabelHandle,
							&continueFlag,
							&changedFlag, 
							numberVectors * sizeof(Str31) );
			
			if (continueFlag)		
				ptr = CheckHandleSize (
							&graphRecordPtr->vectorLengthsHandle,
							&continueFlag,
							&changedFlag, 
							numberSets * numberVectors * sizeof(SInt32) );
			
			if (continueFlag)		
				ptr = CheckHandleSize (
							&graphRecordPtr->vectorPaletteColorHandle,
							&continueFlag,
							&changedFlag, 
							numberVectors * sizeof(SInt32) );
			
			if (continueFlag)		
				ptr = CheckHandleSize (
							&graphRecordPtr->vectorSymbolHandle,
							&continueFlag,
							&changedFlag, 
							numberVectors * sizeof(char) );
			
			if (continueFlag)		
				ptr = CheckHandleSize (
							&graphRecordPtr->xVectorDataHandle,
							&continueFlag,
							&changedFlag, 
							numberSets * numberVectors * sizeof(SInt32) );
							
			if (continueFlag && graphRecordPtr->flag == NU_HISTOGRAM_PLOT)
				ptr = CheckHandleSize (
							&graphRecordPtr->setListHandle,
							&continueFlag,
							&changedFlag, 
							numberSets * sizeof(SInt16) );
							
			if (continueFlag && graphRecordPtr->flag == NU_HISTOGRAM_PLOT)
				ptr = CheckHandleSize (
							&graphRecordPtr->histogramBinWidthHandle,
							&continueFlag,
							&changedFlag, 
							numberSets * sizeof(double) );
							
			CheckAndUnlockHandle (graphRecordPtr->classStatisticsHandle);
			CheckAndUnlockHandle (graphRecordPtr->histogramBinWidthHandle);
			CheckAndUnlockHandle (graphRecordPtr->vectorDisplayHandle);
			CheckAndUnlockHandle (graphRecordPtr->vectorLabelHandle);
			CheckAndUnlockHandle (graphRecordPtr->vectorLengthsHandle);
			CheckAndUnlockHandle (graphRecordPtr->vectorPaletteColorHandle);
			CheckAndUnlockHandle (graphRecordPtr->vectorSymbolHandle);
			CheckAndUnlockHandle (graphRecordPtr->xVectorDataHandle);
			CheckAndUnlockHandle (graphRecordPtr->setListHandle);
			
			}		// end "if (continueFlag)" 
			
		}		// end "if (graphRecordPtr)" 
		
	return (continueFlag);
			
}		// end "CheckGraphVectorsSize" 


                    
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CheckSomeGraphWindowEvents
//
//	Software purpose:	This routine checks for specified events and 
//							handles them.  This routine is used by processors
//							which have created a graph window to allow the user
//							to review the data before going on to the next graph.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			ListFieldData in listData.c
//
//	Coded By:			Larry L. Biehl			Date: 10/04/1991
//	Revised By:			Larry L. Biehl			Date: 01/20/2017			

Boolean CheckSomeGraphWindowEvents (
				GraphPtr								graphRecPtr)

{  
#if defined multispec_mac
	DialogPtr							dialogPtr;
	WindowPtr							window;
	
	Handle								windowInfoHandle;
	
	SInt16								itemHit,
											mouseLoc,
											what;
	
	Boolean								continueFlag,
											haveEvent,
											quitFlag;
	
	
			// Do event loop processing for check some events.							
	
	quitFlag = FALSE;
	do
		{
		haveEvent = WaitNextEvent (everyEvent, &gEventRecord, gYieldTime, NIL);
			
		if ( haveEvent )
			{
			what = gEventRecord.what;
			if ( IsDialogEvent (&gEventRecord) )
				{
				if ( !DialogSelect (&gEventRecord, &dialogPtr, &itemHit) )
					{
					if (what == updateEvt)
						what = nullEvent;
						
					window = GetDialogWindow (dialogPtr);
						
					}		// end "if ( !DialogSelect (&gEventRecord, ..." 
				
				}		// end "if (IsDialogEvent (&gEventRecord))" 
				
	    	mouseLoc = FindWindow( gEventRecord.where, &window );
				
			if (what == mouseDown && window != graphRecPtr->window)
				what = nullEvent;
				
	    	switch (what)
	    		{
	    		case mouseDown:
	    		
					switch( mouseLoc ) 
						{
						case inSysWindow:
		    				#if !TARGET_API_MAC_CARBON
								SystemClick (&gEventRecord, window);
		    				#endif	// !TARGET_API_MAC_CARBON
							break;
							
						case inDrag:
	    					DragMultiSpecWindow(window, gEventRecord.where);
							break;
							
						case inGoAway:
							if ( TrackGoAway( window, gEventRecord.where ) ) 
																						return (TRUE);
							break;
							
						case inGrow:
							windowInfoHandle = (Handle)GetWRefCon(window);
							DoGraphWindowGrow (window, windowInfoHandle);
							break;
							
						case inContent:
							break;
							
						default:
							break;
					}
	    			break;
	    			
 				case keyDown:
 							// If the command key and the period have been depressed	
 							// then return FALSE indicating that the user wants to	
 							// stop the current process.										
 							
					if ( (gEventRecord.modifiers & cmdKey) &&
								( (gEventRecord.message & charCodeMask) == 0x2e) )
																					return(FALSE);
																					
					else if ( (gEventRecord.message & charCodeMask) == 0x1B )
																					return(FALSE);
																					
					else if ( ( gEventRecord.modifiers & 256 ) && 
								( ( gEventRecord.message & 255 ) == 'c' ) ) 
						{
								// Handle a graph copy.											
								
						continueFlag = DoGraphWindowEdit ( kCopy );
							
						if (!continueFlag)
																					return (FALSE);
							
						}
					break;
		  
		  		case updateEvt:
		  			DoUpdateEvent (&gEventRecord);
	         	break;
		    
				case activateEvt:
					DoActivateEvent ( (WindowPtr)gEventRecord.message,
													(gEventRecord.modifiers & activeFlag) );
					break;
		    
				case osEvt:
					DoSuspendResume (&gEventRecord);
					break;
			
				} 			// end of case what 
				
			}		// end "if (haveEvent)" 
			
		}		while (!quitFlag);
		
	return (TRUE);  
#endif	// defined multispec_mac 


#if defined multispec_win  
	CMGraphView*						graphViewCPtr;
					
	HWND 									frame_hWnd = NULL,
											view_hWnd = NULL;
							
	MSG 									msgCur;
	
	Boolean								returnFlag = TRUE,
											quitFlag;
	                 
    
   if (graphRecPtr == NULL)
   																					return (FALSE);  
                    
   graphViewCPtr = graphRecPtr->graphViewCPtr;
    
   if (graphViewCPtr == NULL)
   																					return (FALSE); 
   
   CMGraphDoc* graphDocumentPtr = (CMGraphDoc*)graphRecPtr->graphViewCPtr->GetDocument();																					
   frame_hWnd = (graphDocumentPtr->GetGraphFrameCPtr())->m_hWnd;						
   view_hWnd = graphRecPtr->graphViewCPtr->m_hWnd;
   	             
   quitFlag = FALSE; 
   do
   	{                                                                           
 		while (::PeekMessage(&msgCur, NULL, 0, 0, PM_NOREMOVE) )
	 		{  
			if (!((CMultiSpecApp*)AfxGetApp())->PumpMessage()) 
	 																					return (FALSE);
				
	 		if (msgCur.message == WM_KEYDOWN)
	 			{
	 			if (msgCur.wParam == 0x001b)
	 																					return (FALSE);
	 			
	 			}		// end "if (msgCur.message == WM_CHAR)" 
	 			
	 		else if (msgCur.hwnd == view_hWnd && msgCur.message == WM_LBUTTONDBLCLK)
	 			{                                                                             
 				quitFlag = TRUE;
 				break;   
	 			
	 			}		// end "else if (msgCur.message == WM_LBUTTONDBLCLK)"
	 			
	 		else if (msgCur.message == WM_SYSCOMMAND)
	 			{
	 			if (msgCur.hwnd == frame_hWnd)
	 				{                                                     
 					quitFlag = TRUE;
 					break;
 					
 					}		// end "if (msgCur.hwnd == frame_hWnd)"            
	 			
	 			}		// end "else if (msgCur.message == WM_DESTROY)"
	 		
	 		}		// end "while (::PeekMessage(..." 
 		
 		}		while (!quitFlag);
   
 	return (returnFlag); 
#endif	// defined multispec_win 
   
#ifdef multispec_lin
   Boolean	quitFlag = FALSE;
   Boolean returnFlag = FALSE;
	
	
   //eventLoopBasePtr->YieldFor(wxEVT_CATEGORY_UI);
   wxEventLoopBase* eventLoopBasePtr = wxEventLoopBase::GetActive();
	
   while(!quitFlag)
		{
      eventLoopBasePtr->YieldFor(wxEVT_CATEGORY_USER_INPUT + wxEVT_CATEGORY_UI);

      if(gListDataCode == 1)
			{
         returnFlag = TRUE;
         quitFlag = TRUE;
         gListDataCode = 0;
			}
      
      //if(wxGetKeyState(WXK_F2)){
      if(gListDataCode == 2)
			{
         returnFlag = FALSE;
         quitFlag = TRUE;
         gListDataCode = 3;
			}
		}
   
   return (returnFlag);
#endif
		
}		// end "CheckSomeGraphWindowEvents"


//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CopyGraphYVector
//
//	Software purpose:	This routine copies the data from the y vector for the
//							input graph record pointer to the y vector for the output
//							y vector.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			ShowGraphWindowSelection in selectionGraph.c
//
//	Coded By:			Larry L. Biehl			Date: 07/31/1992
//	Revised By:			Larry L. Biehl			Date: 07/31/1992			

void CopyGraphYVector (
				Handle								inputSelectionGraphRecHandle, 
				GraphPtr								outputGraphRecordPtr)
													
{
	GraphPtr								inputGraphRecordPtr;
	
	SInt32								error,
											index,
											vectorLength;
	
	
	if (inputSelectionGraphRecHandle != NULL)
		{
		inputGraphRecordPtr = (GraphPtr)GetHandlePointer(
													inputSelectionGraphRecHandle,
													kLock,
													kNoMoveHi);
		
		outputGraphRecordPtr->yVector.numberPoints = 
												inputGraphRecordPtr->yVector.numberPoints;
		outputGraphRecordPtr->numberVectors = inputGraphRecordPtr->numberVectors;
		vectorLength = inputGraphRecordPtr->yVector.numberPoints; 
														
		LockAndGetVectorPointer ( &inputGraphRecordPtr->yVector, &error );
		
		if (error == noErr)
			LockAndGetVectorPointer ( &outputGraphRecordPtr->yVector, &error );
		
		
		if (error == noErr)
			{
			for ( index=0; index<vectorLength; index++)
				{
				SetV( &outputGraphRecordPtr->yVector, 
						index, 
						inputGraphRecordPtr->yVector.basePtr[index], 
						&error);
				
				}		// end "for ( index=0; index<localNumberChannels..." 
						
			}		// end "if (error == noErr)" 
			
		UnlockVectorPointer ( &inputGraphRecordPtr->yVector);
		UnlockVectorPointer ( &outputGraphRecordPtr->yVector );
		
		CheckAndUnlockHandle (inputSelectionGraphRecHandle);
		
		}		// end "if (inputSelectionGraphRecHandle != NULL)" 

}		// end "CopyGraphYVector" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CreateGraph
//
//	Software purpose:	The purpose of this routine is to create a new graph
//							within the input window.
//
//	Parameters in:		A window pointer
//
//	Parameters out:	None
//
// Value Returned:	True: if graph was successfully created.
//							False: if graph was not successfully created.
// 
// Called By:			BiPlotDataControl in biPlotData.c
//							ListFieldData in SLstData.cpp
//							ShowGraphWindowSelection in SSelGraf.cpp
//							HistogramStatsControl in statHistogram.c
//
//	Coded By:			Larry L. Biehl			Date: 10/03/1991
//	Revised By:			Larry L. Biehl			Date: 01/25/1999

Boolean CreateGraph	(
				CMGraphView*						graphViewCPtr,
				GraphPtr								graphRecordPtr, 
				SInt32								lineStart, 
				SInt32								columnStart, 
				SInt32								lineEnd, 
				SInt32								columnEnd)

{
	SInt32								error;
	
	
	error = noErr;
	
	if (graphRecordPtr != NULL)
		{
				// Set the minimum and maximum values for the graph.
				
		SetGraphMinMax (graphRecordPtr, kBothXYAxes);
		
				// Get the title for the graph.											
				
		GetGraphTitle (graphRecordPtr, lineStart, columnStart, lineEnd, columnEnd);
										
				// Get the labels for the graph.											
				
		GetGraphLabels (graphRecordPtr);
      
      //printf("yScaleMin:%d, yScaleMax:%d\n", graphRecordPtr->yScaleMin, graphRecordPtr->yScaleMax);
		
		graphRecordPtr = NewGraph( graphRecordPtr, 
											graphRecordPtr->window,
											graphRecordPtr->xScaleMin, 
											graphRecordPtr->xScaleMax, 
											graphRecordPtr->yScaleMin, 
											graphRecordPtr->yScaleMax, 
											graphRecordPtr->attrb, 
											TRUE, 
											&error );
								
		if (error == noErr) 
			{
			#if defined multispec_mac 
				InvalidateGraphWindow(graphRecordPtr->window, kFrameArea);
			#endif	// defined multispec_mac
			         
			#if defined multispec_win
				if (graphRecordPtr->graphViewCPtr != NULL)
					graphRecordPtr->graphViewCPtr->Invalidate();	 
			#endif	// defined multispec_win

			#if defined multispec_lin
            if (graphRecordPtr->graphViewCPtr != NULL){
					//graphRecordPtr->graphViewCPtr->Invalidate();
               if (graphRecordPtr->drawGraphCode <= 0)
                  graphRecordPtr->drawGraphCode = 1;
               if(graphViewCPtr != NULL)
                  graphViewCPtr->m_frame->Refresh();
            }
         #endif
         
			if (graphRecordPtr->drawGraphCode <= 0)
				graphRecordPtr->drawGraphCode = 1;
				
         //UpdateGraphControls (graphRecordPtr->window);
			
			}		// end "if ( error == noErr )" 
		
		}		// end "if (graphRecordPtr != NULL)" 
		
	return (error == 0);

}		// end "CreateGraph"  
           


#if defined multispec_mac
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		CMGraphView* CreateGraphWindow
//
//	Software purpose:	The purpose of this routine is to create a graph
//							window and allocate a structure that contains handles to
//							other structures which will be connected with the
//							window and create the controls for the window.
//
//	Parameters in:		Handle to window information for image.
//
//	Parameters out:	None
//
// Value Returned:	Window pointer
// 
// Called By:			BiPlotDataControl in biPlotData.c
//							ListDataControl in SLstData.cpp
//							SelectionGraphControl in SSelGraf.cpp
//							HistogramStatsControl in statHistogram.c
//
//	Coded By:			Larry L. Biehl			Date: 09/27/1991
//	Revised By:			Larry L. Biehl			Date: 08/07/2013	

CMGraphView* CreateGraphWindow (void)

{
			// Declare local variables 
			
	Rect									tempRect;	
	
	CMGraphView*						newSelectionGraphCPtr = NULL;
	GrafPtr								savedPort;
	GraphPtr								graphRecordPtr;
	WindowInfoPtr						windowInfoPtr;
	WindowPtr							front,
											windowPtr;												 
  
	ControlHandle						controlHandle,
											theRootControl;
	
	Handle								graphRecordHandle = NULL,
											selectionWindowInfoHandle,
											windowInfoHandle,
											windowRecordHandle = NULL;
												
	OSStatus								errorCode;
	
	SInt16								grafPortType,
											graphWindowID,
											windowMenuItem,
											xSize = 280,
											ySize = 150;
											
	Boolean								continueFlag = TRUE;
	
	SignedByte							handleStatus;

									
	front = (WindowPtr)-1;
					
	graphWindowID = kGraphWindowID;
	if (gProcessorCode == kSelectionGraphProcessor)								
		graphWindowID = kGraphSelectionWindowID;
	
#	if !TARGET_API_MAC_CARBON
				// Get handle to window record.	
		windowRecordHandle = GetImageWindowRecord ();
		if (windowRecordHandle == NULL)
																					return (NULL);
																					
		if ( GetMaxSystemPixelSize () > 1 )
			{
					// Window for Macs with color displays 									

			windowPtr = GetNewCWindow (
										graphWindowID, *windowRecordHandle, gActiveImageWindow);
			grafPortType = kCGrafType;
			
			}		// end "if ( GetMaxSystemPixelSize () > 1 )" 
			
		else		// GetMaxSystemPixelSize () == 1 
			{
					// Window for Macs with Black&white only displays.						
					
			windowPtr = GetNewWindow(
								graphWindowID, *windowRecordHandle, gActiveImageWindow);
			grafPortType = kGrafType;
			
			}		// end "else GetMaxSystemPixelSize () == 1 " 
#	else	// TARGET_API_MAC_CARBON
		windowPtr = GetNewCWindow (graphWindowID, NULL, front);
//		windowPtr = GetNewCWindow (graphWindowID, NULL, gActiveImageWindow);
		grafPortType = kCGrafType;
#	endif	// !TARGET_API_MAC_CARBON, else...
		
	if (windowPtr == NULL)													
																						return (NULL);
		
	GetPort(&savedPort);
	SetPortWindowPort (windowPtr);
	
	windowInfoHandle = (Handle)MNewHandle ( sizeof(WindowInfo) );
	
			// If windowInfoHandle is NULL then there isn't enough memory.		
			// Clean up at end and exit the routine if there isn't enough	
			// memory.																			
		
	continueFlag = (windowInfoHandle != NULL);
	
	if (continueFlag)
		{
		SetWRefCon (windowPtr, (SInt32)windowInfoHandle);	
	
			// Initialize the window information record.									
			
		InitializeWindowInfoStructure (
						windowInfoHandle, kNotPointer, NULL, kNoImageType, kGraphicsWindowType);
		
		newSelectionGraphCPtr = new (CMGraphView);

		continueFlag = (newSelectionGraphCPtr != NULL);
		
		}		// end "if (continueFlag)"
   		
   if (continueFlag)
		{	
				// Set the font and font size for the window.							
				
		TextFont (gWindowTextFont); 		// monaco  
		TextSize (9);							// 9 point 
	
		windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
														windowInfoHandle, kLock, kNoMoveHi); 
						
		windowInfoPtr->windowRecordHandle = windowRecordHandle;
		windowInfoPtr->grafPortType = grafPortType;
		windowInfoPtr->graphViewCPtr = newSelectionGraphCPtr;
		
				// Initialize some more of graph record handle parameters.							
		
		graphRecordHandle = newSelectionGraphCPtr->GetGraphRecordHandle();	
		graphRecordPtr = (GraphPtr)GetHandleStatusAndPointer (
																	graphRecordHandle, 
																	&handleStatus, 
																	kMoveHi);	
						
		graphRecordPtr->window = windowPtr;
		graphRecordPtr->imageWindow = NULL;
		graphRecordPtr->textSize = GetPortTextSize (GetWindowPort(windowPtr));
		
		newSelectionGraphCPtr->SetWindowPtr (windowPtr);
	 	
	 			// Change name of current selection graph if it exists.				
	 			// Also unlock the graph record handle since it will not be			
	 			// the selection graph any more.												
	 			
	 	if (gSelectionGraphViewCPtr != NULL && 
	 										gProcessorCode == kSelectionGraphProcessor)
	 		{
			WindowPtr currentSelectionWindow = 
													gSelectionGraphViewCPtr->GetWindowPtr();
			SetGraphWindowTitle (gSelectionGraphViewCPtr, FALSE);
			GetWTitle (currentSelectionWindow, gTextString);
			selectionWindowInfoHandle = (Handle)GetWRefCon (currentSelectionWindow);
			if (selectionWindowInfoHandle != NULL)
				{
				windowMenuItem = 
						((WindowInfoPtr)*selectionWindowInfoHandle)->windowMenuItem;
				SetMenuItemText (gMultiSpecMenus[kWindowM], 
										windowMenuItem, 
										gTextString );
									
				}		// end "if (selectionWindowInfoHandle != NULL)" 
	 		
			CheckAndUnlockHandle (
									gSelectionGraphViewCPtr->GetGraphRecordHandle() );
		
	 		}		// end "if (gSelectionGraphViewCPtr != NULL && ..." 
		
				// Add new graph window to window list. Add to end of list.	
				
		UpdateWindowList (windowPtr, kGraphicsWindowType);	
			
		gUpdateWindowsMenuItemsFlag = TRUE;
		
				// Set the window title and then show the window on the screen
		
		SetGraphWindowTitle (newSelectionGraphCPtr, TRUE);
		GetWTitle (windowPtr, gTextString);
		AppendMenu (gMultiSpecMenus[kWindowM], "\pNewGraph");	
		SetMenuItemText (gMultiSpecMenus[kWindowM], 
									windowInfoPtr->windowMenuItem, 
									gTextString );
				
				// Set the location and size of the window.
				
		GetGraphWindowLocation (&xSize, &ySize);                            
		
		MoveWindow (windowPtr, 
						gNextGraphicsWindowStart.h, 
						gNextGraphicsWindowStart.v, 
						FALSE);
						
		SizeWindow (windowPtr, xSize, ySize, TRUE); 
		
				// Create the controls for the graph window if needed.												
				
		if (gProcessorCode == kHistogramStatsProcessor)	
			{
			if (gAppearanceManagerFlag)
				{
						// Create the Root control
						
				errorCode = CreateRootControl (windowPtr, &theRootControl);
				if (errorCode != noErr)
																							return (FALSE);
																							
				}		// end "if (gAppearanceManagerFlag)"
																					
					// Make the graph next set control					
			
			GetWindowPortBounds (windowPtr, &gTempRect);
			tempRect.top    = gTempRect.bottom - kSBarWidth;
			tempRect.left   = gTempRect.left - 1;
			tempRect.bottom = gTempRect.bottom + 1;
			tempRect.right  = tempRect.left + kLegendScrollWidth;															
	
			if (gAppearanceManagerFlag)	
				controlHandle = NewControl (windowPtr, 
														&tempRect, 
														"\p^", 
														FALSE, 
														0, 
														0, // kControlContentIconSuiteRes,
														0,	// 10001,
														kControlBevelButtonSmallBevelProc, 
														kNextGraphSetControl);
								
			else		// !gAppearanceManagerFlag	
				controlHandle = NewControl (windowPtr, 
														&tempRect, 
														"\p^", 
														FALSE, 
														0, 
														0, 
														0, 
														pushButProc, 
														kNextGraphSetControl);
														
							
			if (controlHandle == NULL)
																							return(FALSE);
			HiliteControl (controlHandle, 255);
		
					// Make the graph previous set control				
						
			tempRect.left   = tempRect.right;
			tempRect.right  = tempRect.left + kLegendScrollWidth;															
	
			if (gAppearanceManagerFlag)	
				controlHandle = NewControl (windowPtr, 
														&tempRect, 
														"\pv", 
														FALSE, 
														0, 
														0, // kControlContentIconSuiteRes,
														0,	// 10001,
														kControlBevelButtonSmallBevelProc, 
														kPreviousGraphSetControl);
								
			else		// !gAppearanceManagerFlag	
				controlHandle = NewControl (windowPtr, 
														&tempRect, 
														"\pv", 
														FALSE, 
														0, 
														0, 
														0, 
														pushButProc, 
														kPreviousGraphSetControl);
														
			if (controlHandle == NULL)
																						return(FALSE);
			HiliteControl (controlHandle, 255);
			
					// Make the graph select vector control				
						
			tempRect.left   = tempRect.right;
			tempRect.right  = tempRect.left + kBottomButtonWidth;
				
			if (gAppearanceManagerFlag)
				controlHandle = NewControl (
								windowPtr, 
								&tempRect, 
								"\p", 
								FALSE,
								kColorMenuID, 
								kControlContentIconSuiteRes+kControlBehaviorMultiValueMenu, 
								10002, 
								kControlBevelButtonSmallBevelProc+kControlBevelButtonMenuOnRight,
								kGraphVectorControl);
			
			else		// !gAppearanceManagerFlag	
				controlHandle = NewControl (windowPtr, 
														&tempRect, 
														"\p", 
														FALSE, 
														0, 
														0, 
														0, 
														16*131 + 2, 
														kGraphVectorControl);
			
			if (controlHandle == NULL)
																						return(FALSE);
			HiliteControl (controlHandle, 255);
			
					// Make the graph overlay control				
						
			tempRect.left   = tempRect.right;
			tempRect.right  = tempRect.left + kBottomButtonWidth;	
				
			if (gAppearanceManagerFlag)
				controlHandle = NewControl (
								windowPtr, 
								&tempRect, 
								"\p", 
								FALSE,
								kColorMenuID, 
								kControlContentIconSuiteRes+kControlBehaviorMultiValueMenu, 
								10001, 
								kControlBevelButtonSmallBevelProc+kControlBevelButtonMenuOnRight,
								kGraphOverlayControl);
			
			else		// !gAppearanceManagerFlag	
				controlHandle = NewControl (windowPtr, 
														&tempRect, 
														"\p", 
														FALSE, 
														0, 
														0, 
														0, 
														16*131 + 1, 
														kGraphOverlayControl);
			
			if (controlHandle == NULL)
																						return(FALSE);
			HiliteControl (controlHandle, 255);
			
					// Make the graph bin width control				
						
			tempRect.left   = tempRect.right;
			tempRect.right  = tempRect.left + kBottomButtonWidth;	
				
			if (gAppearanceManagerFlag)
				controlHandle = NewControl (
								windowPtr, 
								&tempRect, 
								"\p", 
								FALSE,
								kColorMenuID, 
								kControlContentIconSuiteRes+kControlBehaviorMultiValueMenu, 
								10004, 
								kControlBevelButtonSmallBevelProc+kControlBevelButtonMenuOnRight,
								kGraphBinWidthControl);
			
			else		// !gAppearanceManagerFlag	
				controlHandle = NewControl (windowPtr, 
														&tempRect, 
														"\p", 
														FALSE, 
														0, 
														0, 
														0, 
														16*131 + 4, 
														kGraphBinWidthControl);
			
			if (controlHandle == NULL)
																						return(FALSE);
			HiliteControl (controlHandle, 255);
			
			}		// end "if (gProcessorCode == kHistogramStatsProcessor)"
		
				// Show and select the window.	
			
		ShowWindow (windowPtr);											
				
		if (gProcessorCode != kBiPlotDataProcessor && 
												gProcessorCode != kHistogramStatsProcessor)
			{	
					// Activate the window
					
			SelectWindow (windowPtr);
			
			}		// end "if (gProcessorCode != kBiPlotDataProcessor && ..."
		
		CheckSomeEvents (activMask+updateMask);
		
		SetPort (savedPort);
		
	   		// Unlock the information handles.											
	  
	  	MHSetState (graphRecordHandle, handleStatus);
		CheckAndUnlockHandle (windowInfoHandle);
	   
	   }		// end "if (continueFlag)" 
   
   		// If there was a lack of memory, make certain that all handles have	
   		// been disposed of properly and dipose of the window.					
   		
   if (!continueFlag)
		{
#		if TARGET_API_MAC_CARBON
			DisposeWindow (windowPtr);
#		else
			CloseWindow (windowPtr);
			ReleaseWindowRecord (windowRecordHandle);
#		endif	// TARGET_API_MAC_CARBON else...
		windowPtr = NULL;
		UnlockAndDispose (windowInfoHandle);

		if (newSelectionGraphCPtr != NULL)
			delete (newSelectionGraphCPtr);
		
		newSelectionGraphCPtr = NULL;
		
		}		// end "if (!continueFlag)" 
	
	return (newSelectionGraphCPtr);
	
}		// end "CreateGraphWindow" 
#endif	// defined multispec_mac 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DisposeOfGraphRecordMemory
//
//	Software purpose:	The purpose of this routine is to of the memory
//							that has been allocated to the graph record structure
//							variables.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/25/1994
//	Revised By:			Larry L. Biehl			Date: 02/08/2006			

void DisposeOfGraphRecordMemory (
				GraphPtr								graphRecordPtr)

{
	SInt32								error;
	
	
	if (graphRecordPtr != NULL)
		{
		DeallocateV( &graphRecordPtr->xVector, &error);
		DeallocateV( &graphRecordPtr->yVector, &error);
							
		UnlockAndDispose (graphRecordPtr->classStatisticsHandle);
		UnlockAndDispose (graphRecordPtr->histogramBinWidthHandle);
		UnlockAndDispose (graphRecordPtr->setListHandle);
		UnlockAndDispose (graphRecordPtr->vectorDisplayHandle);
		UnlockAndDispose (graphRecordPtr->vectorLabelHandle);
		UnlockAndDispose (graphRecordPtr->vectorLengthsHandle);
		UnlockAndDispose (graphRecordPtr->vectorPaletteColorHandle);
		UnlockAndDispose (graphRecordPtr->vectorSymbolHandle);
		UnlockAndDispose (graphRecordPtr->xVectorDataHandle);
			
		}		// end "if (graphRecordPtr)" 
			
}		// end "DisposeOfGraphRecordMemory" 



#if defined multispec_mac
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoGraphWActivateEvent
//
//	Software purpose:	This routine handles the activate events for graph windows.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			DoGraphWActivateEvent
//
//	Coded By:			Larry L. Biehl			Date: 06/23/1993
//	Revised By:			Larry L. Biehl			Date: 01/26/1999				

void DoGraphWActivateEvent (
				WindowPtr							windowPtr, 
				Handle								windowInfoHandle,
				Boolean								becomingActiveFlag)
		
{
			//	Local Declarations 	
			
	GraphPtr								graphRecordPtr;
	
	ControlHandle						controlHandle,
											rootControlHandle;
											
	Handle								graphRecordHandle;
	
	UInt16								index,
											numberControls;
	
	SignedByte							handleStatus;
	
	
	
	if (becomingActiveFlag)
		{	
				// Hilite the controls as needed.
	
		graphRecordHandle = GetGraphRecordHandle(windowPtr);
		graphRecordPtr = (GraphPtr)GetHandleStatusAndPointer (
												graphRecordHandle,
												&handleStatus, 
												kNoMoveHi);
				
		UpdateGraphControls (windowPtr);
              	
		MHSetState (graphRecordHandle, handleStatus);
	     	
		}		// end "if (becomingActiveFlag)"
		
	else		// !becomingActiveFlag
		{
				// This is a deactivate event. Unhilite the graph controls			
							
		if (gAppearanceManagerFlag)
			{	
			GetRootControl(windowPtr, &rootControlHandle);
			
			if (rootControlHandle != NULL)
				{
				CountSubControls (rootControlHandle, &numberControls);
				for (index=numberControls; index>=1; index--)
					{
					GetIndexedSubControl(rootControlHandle, index, &controlHandle);
					HiliteControl (controlHandle, 255);
			     		
			     	}		// end "for (index=numberControls; index>=1; index--)"
			     	
				}		// end "if (rootControlHandle != NULL)"
	     	
	     	}		// end "if (gAppearanceManagerFlag)"
	     	
		#if !TARGET_API_MAC_CARBON
			else		// !gAppearanceManagerFlag
				{
				controlHandle = (ControlHandle)((WindowPeek)windowPtr)->controlList;
				while (controlHandle != NULL)	
					{
					HiliteControl (controlHandle, 255);
		     		controlHandle = (*controlHandle)->nextControl;
		     		
		     		}		// end "while (controlHandle != NULL)"
				
				}		// end "else !gAppearanceManagerFlag"
		#endif	// !TARGET_API_MAC_CARBON
		
		}		// end "else !becomingActiveFlag"

}		// end "DoGraphWActivateEvent" 
#endif	// defined multispec_mac

#if defined multispec_mac
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoGraphicsWindowEdit
//
//	Software purpose:	This routine handle the editing function for a
//							Graph window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/09/1993
//	Revised By:			Larry L. Biehl			Date: 06/04/1996			

Boolean DoGraphWindowEdit (
				SInt16								menuItem )
													
{
	GraphPtr								graphRecordPtr;
	
	PicHandle							thePic;
	
	SInt32								errCode,
											handleSize;
											
	Boolean								continueFlag;
	SignedByte							handleStatus;
		
	
	continueFlag = TRUE;
	
	switch (menuItem)
		{
		case kUndo:
					//	Cannot Undo 
			break;
	
		case kCut:
					//	Cannot Cut 
			break;
			
		case kCopy:		
			SetPortWindowPort (gTheActiveWindow);
			thePic = (PicHandle)OpenPicture( 
									GetWindowPortBounds (gTheActiveWindow, &gTempRect) );
				
			if (thePic != NULL)
				{
				WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer(
											gActiveWindowInfoH, kNoLock, kNoMoveHi);
				CMGraphView* graphViewCPtr = windowInfoPtr->graphViewCPtr;
				
				graphRecordPtr = (GraphPtr)GetHandleStatusAndPointer (
												graphViewCPtr->GetGraphRecordHandle(),
												&handleStatus,
												kNoMoveHi);
			
				continueFlag = DrawGraph (graphRecordPtr, TRUE);
			
				ClosePicture(  );
       	
#				if !TARGET_API_MAC_CARBON
       			ZeroScrap();
#				else
       			ScrapRef 	scrapRef;
       			ClearCurrentScrap ();
       			errCode = GetCurrentScrap (&scrapRef);
#				endif	// !TARGET_API_MAC_CARBON
							
            handleSize = GetHandleSize( (Handle)thePic );
            HLock ( (Handle)thePic );
	               	
#				if !TARGET_API_MAC_CARBON
           		errCode = PutScrap(handleSize, 'PICT', (Ptr)*thePic);
#				else
           		errCode = PutScrapFlavor (scrapRef, 
           											'PICT', 
           											kScrapFlavorMaskNone, 
           											handleSize,
           											(Ptr)*thePic);
#				endif	// !TARGET_API_MAC_CARBON
            
            if( errCode != noErr )  
					errCode = DisplayAlert (1153, kStopAlert, 0, 0, 0, NULL);
					
				else		// errCode == noErr 
					{
							// Make certain that any text edit scrap does not		
							// write over the desk scrap.		
														
					TESetScrapLength (0);
					gLastScrapCount = 0;
								
					}		// end "else errCode == noErr" 
					
            CheckAndUnlockHandle ( (Handle)thePic );
            KillPicture (thePic);
              	
				MHSetState (graphViewCPtr->GetGraphRecordHandle(), handleStatus);
              	
				}		// end "if (graphPic !=NULL)" 
				
		case kPaste:
			//	Cannot Paste 
			break;
		
		case kClear:
			//	Cannot Clear 
			break;
			
		case kSelectAll:
			//	Cannot Select All 
			break;
			
		}		// end "switch (menuItem)" 
		
	return (continueFlag);

}		// end "DoGraphWindowEdit" 
#endif	// defined multispec_mac



#if defined multispec_mac
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoGraphWindowGrow
//
//	Software purpose:	This routine handles mouse down events in the
//							grow box for graph windows.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			main
//
//	Coded By:			Larry L. Biehl			Date: 10/04/1991
//	Revised By:			Larry L. Biehl			Date: 04/30/2003		

void DoGraphWindowGrow (
				WindowPtr							window,
				Handle								windowInfoHandle)

{
	Rect									growArea;
//	GraphPtr								graphRecordPtr;
	Handle								graphRecordHandle;
	SInt32								newSize;
										
	
			// Get the current maximum rectangular display area.
													
	GetDisplayRegionRectangle (&growArea);	
			
	growArea.right -= growArea.left;
	growArea.bottom -= growArea.top;
	growArea.left = growArea.top = gGrowAreaMinimum;

	newSize = GrowWindow (window, gEventRecord.where, &growArea);
	SizeWindow (window, LoWord (newSize), HiWord (newSize), TRUE);
	InvalWindowRect (window, GetWindowPortBounds(window, &gTempRect));
	graphRecordHandle = GetGraphRecordHandle (windowInfoHandle);
	
	UpdateGraphScales (graphRecordHandle);
/*
	graphRecordPtr = (GraphPtr)GetHandlePointer(
										graphRecordHandle, kLock, kNoMoveHi);
	SetXGraphScale (window, graphRecordPtr);
	
	if (graphRecordPtr->graphCodesAvailable & 0x0001 &&
										graphRecordPtr->flag & NU_HISTOGRAM_PLOT)
		SetGraphMinMax (graphRecordPtr, kYAxis);
		
	else		// !...->graphCodesAvailable & 0x0001 && ...
		SetYGraphScale (window, graphRecordPtr);
		
	CheckAndUnlockHandle (graphRecordHandle);
*/	
//	EraseRect (&gTempRect);
	DrawGraphControls (window);
	
}		// end "DoGraphWindowGrow" 
#endif	// defined multispec_mac



#if defined multispec_mac
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		DoGraphWMouseDnContent
//
//	Software purpose:	This routine handle mouse down events in the
//							content part of a window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			main
//
//	Coded By:			Larry L. Biehl			Date: 01/23/1999
//	Revised By:			Larry L. Biehl			Date: 01/23/1999			

void DoGraphWMouseDnContent (
				WindowPtr							theWindow,
				Point									mousePt)

{
			//	Local Declarations 																
	
	ControlHandle						whichControl;
	
	SInt16								partCode;

		
	partCode = FindControl (mousePt, theWindow, &whichControl);
	if (partCode > 0)
     	{
     			// Set clip region to include the scroll bars.							
     			
		ClipRect (GetWindowPortBounds(theWindow, &gTempRect));
			
				// Exit if this was a mouse down in a control area of an		
				// inactive image window.  We will assume that the user was	
				// just wanting to select the window.								
				
		if (gNewSelectedWindowFlag)
																					return;
																					
		GraphWControlEvent (whichControl, theWindow, mousePt, partCode);
   		
		}		// end "if (partCode > 0)" 
		 
	else		// partCode == 0.  Mouse not within controls 
		{
		
		}		// end "else partCode == 0" 

}		// end "DoGraphWMouseDnContent"  
#endif	// defined multispec_mac


#if defined multispec_mac
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoGraphWUpdateEvent
//
//	Software purpose:	This routine handles the update events for the graph
//							window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			main
//
//	Coded By:			Larry L. Biehl			Date: 09/27/1991
//	Revised By:			Larry L. Biehl			Date: 06/04/1996		

void DoGraphWUpdateEvent (
				WindowPtr							window, 
				CMGraphView*						graphViewCPtr)
				
{
	GraphPtr								graphRecordPtr;
	
	Handle								graphRecordHandle;
	
	SignedByte							handleStatus;
	
	
	if (graphViewCPtr == NULL)
																								return;
	
	graphRecordHandle = graphViewCPtr->GetGraphRecordHandle();
	graphRecordPtr = (GraphPtr)GetHandleStatusAndPointer (
											graphRecordHandle,
											&handleStatus, 
											kNoMoveHi);
	
	BeginUpdate (window);
	GetWindowPortBounds (window, &graphRecordPtr->clientRect);
	
//	EraseRect (&window->portRect);
	
	DrawGraph (graphRecordPtr, FALSE);
	
	DrawControls (window);
	
	graphViewCPtr->DrawGraphGrowIcon (graphRecordPtr);
	
	EndUpdate (window);
	
	ClipRect (&graphRecordPtr->clientRect);
	
	MHSetState (graphRecordHandle, handleStatus);
	
}		// end "DoGraphWUpdateEvent" 
#endif	// defined multispec_mac

                            
                            
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean DoNextOrPreviousChannel
//
//	Software purpose:	The purpose of this routine is to force the next or previous
//							channel or feature to be drawn in the graph window.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/15/2002
//	Revised By:			Larry L. Biehl			Date: 11/15/2002

void DoNextOrPreviousChannel	( 
				CMGraphView*						graphViewCPtr,
				SInt16								controlIdentifier)

{
	GraphPtr								graphRecordPtr;
	
	Handle								graphRecordHandle; 
	
	Boolean								invalidateFlag = FALSE;
	
	SignedByte							handleStatus;

			
	graphRecordHandle = graphViewCPtr->GetGraphRecordHandle();
		             
	graphRecordPtr = (GraphPtr)GetHandleStatusAndPointer(
										graphRecordHandle, &handleStatus, kNoMoveHi);
				
	if (graphRecordPtr != NULL)
		{
		if (controlIdentifier == kNextGraphSetControl &&
								graphRecordPtr->set < graphRecordPtr->numberSets)
			{
			graphRecordPtr->set++;
			invalidateFlag = TRUE;
			
			}		// end "if (refCon == kNextGraphSetControl && ..."
			
		else if (controlIdentifier == kPreviousGraphSetControl &&
								graphRecordPtr->set > 1)
			{
			graphRecordPtr->set--;
			invalidateFlag = TRUE;
			
			}		// end "if (refCon == kNextGraphSetControl && ..."

		if (invalidateFlag)
			{
			invalidateFlag = FALSE;
         
			SetGraphMinMax (graphRecordPtr, kBothXYAxes);
         
			SetStatHistogramGraphTitle2 (graphRecordPtr);
					
			InvalidateGraphWindow (graphRecordPtr->window, kGraphArea);
			InvalidateGraphWindow (graphRecordPtr->window, kGraphBinWidth);
			
			#if defined multispec_mac
				DoGraphWUpdateEvent (
									graphRecordPtr->window, graphRecordPtr->graphViewCPtr);
			#endif // defined multispec_mac
			
			}		// end "if (invalidateFlag)"

		}		// end "if (graphRecordPtr != NULL)"
			
	MHSetState (graphRecordHandle, handleStatus);

}		// DoNextOrPreviousChannel

                            
                            
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean DrawGraph
//
//	Software purpose:	The purpose of this routine is to draw the data into
//							a graph window.
//
//	Parameters in:		A window pointer
//
//	Parameters out:	None
//
// Value Returned:	True: if graph was successfully created.
//							False: if graph was not successfully created.
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/03/1991
//	Revised By:			Larry L. Biehl			Date: 02/16/2006

Boolean DrawGraph	( 
				GraphPtr								graphRecPtr,
				Boolean								copyFlag)

{
	SInt32								error;
	
	SInt16								leftInset,
											rightInset;
	
	Boolean								continueFlag;
	
	
	error = noErr;
	continueFlag = TRUE;
   

   

	
	if (graphRecPtr != NULL)
		{			
		SetWindowClip (graphRecPtr, &error); 
																						
				// Make sure the inset of the graph from the outside edge is correct.
				// The extra 3 allows for a box to be drawn around the outside of
				// the graph. Also allow for controls along the bottom of the graph window.
				
		leftInset  = (SInt16)MAX (graphRecPtr->yTicLabelWidth*graphRecPtr->textScaling, 
									graphRecPtr->xTicLabelWidth/2 + 3);
		rightInset  = (SInt16)MAX (23*graphRecPtr->textScaling, 
									graphRecPtr->xTicLabelWidth/2 + 3);
      
		InsetGraph( graphRecPtr, 
			leftInset,		// 36 + 3 
			(SInt16)(graphRecPtr->textSize * 3.0 + 3*graphRecPtr->textScaling),
			rightInset,		// 20 + 3                          
			#if defined multispec_mac
				(SInt16)(graphRecPtr->textSize * 3.2 + 3*graphRecPtr->textScaling + 17),
			#endif	// defined multispec_mac
				                
			#if defined multispec_win 
				(SInt16)(graphRecPtr->textSize * 3.2 + 3*graphRecPtr->textScaling + 19),
			#endif	// defined multispec_win
			
         #if defined multispec_lin 
				(SInt16)(graphRecPtr->textSize * 3.2 + 3*graphRecPtr->textScaling + 19),
			#endif	// defined multispec_lin
			&error );
      
      
      //if(gProcessorCode == kSelectionGraphProcessor)
         //graphRecPtr->clientRect.bottom = 250;
      

      // Draw the white rectangle
		DrawAxis (graphRecPtr, &error); 
      
 																			
		if (graphRecPtr->drawGraphCode & 0x8000)       
				// Do not draw any part of graph.
																						return (noErr); 
		
		if ( (graphRecPtr->drawGraphCode & 0x0001) &&
								(graphRecPtr->xVector.numberPoints == 0 || 
												graphRecPtr->yVector.numberPoints == 0) )
																						return (noErr);   

//		if (graphRecPtr->drawGraphCode == 0)     
//			continueFlag = FALSE;
//																						return (noErr);
      
      
		if (continueFlag)
			{
			if (graphRecPtr->attrb & NU_GRID) 
				{
						// Draw grid in background of graph
						                                     
				#if defined multispec_mac	
					RGBColor				grayColor,
											savedForeGroundColor;
			
					GetForeColor (&savedForeGroundColor);
			
					grayColor.red = 0xCCCC;
					grayColor.green = 0xCCCC;
					grayColor.blue = 0xCCCC;
					RGBForeColor ( &grayColor );	
					 
					DrawLinTicks( graphRecPtr, 
										graphRecPtr->xInt, 
										graphRecPtr->yInt, 
										1000, 
										&error );
										
					RGBForeColor ( &savedForeGroundColor );
				#endif	// defined multispec_mac	       
						                                     
				#if defined multispec_win
					CPen* oldPenPtr = 
								graphRecPtr->pDC->SelectObject( &CMGraphView::s_ltGrayPen );
					 
					DrawLinTicks( graphRecPtr, 
										graphRecPtr->xInt, 
										graphRecPtr->yInt, 
										(SInt16)(1000 * graphRecPtr->textScaling), 
										&error ); 
										
					graphRecPtr->pDC->SelectObject( oldPenPtr );
				#endif	// defined multispec_win	

            #if defined multispec_lin
               //graphRecPtr->pDC->SetPen(CMGraphView::s_ltGrayPen); 
               wxPen oldPenPtr = graphRecPtr->pDC->GetPen();
               wxColour gray;
               gray.Set(wxT("#d4d4d4"));
               graphRecPtr->pDC->SetPen(wxPen(gray)); 

               // draw gray grid column line
					DrawLinTicks( graphRecPtr, 
										graphRecPtr->xInt, 
										graphRecPtr->yInt, 
										(SInt16)(1000 * graphRecPtr->textScaling), 
										&error ); 
               
               graphRecPtr->pDC->SetPen(oldPenPtr);
            #endif // defined multispec_lin

				}		// end "if ( graphRecPtr->attrb & NU_GRID )" 
				                       
			#if defined multispec_mac
//				DrawLinTicks( graphRecPtr, graphRecPtr->xInt/5, graphRecPtr->yInt/5, 5, &error );
				DrawLinTicks( graphRecPtr, graphRecPtr->xInt, graphRecPtr->yInt, 8, &error );
			#endif	// defined multispec_mac	             
						                                     
			#if defined multispec_win
				DrawLinTicks( graphRecPtr, 
									graphRecPtr->xInt, 
									graphRecPtr->yInt,
									(SInt16)(8 * graphRecPtr->textScaling),
									&error );
			#endif	// defined multispec_win
	            
         #if defined multispec_lin
            wxPen oldPenPtr = graphRecPtr->pDC->GetPen();
            wxColour gray;
            gray.Set(wxT("#d4d4d4"));
            graphRecPtr->pDC->SetPen(wxPen(gray)); 
#if 0   
            DrawLinTicks( graphRecPtr, 
									graphRecPtr->xInt, 
									graphRecPtr->yInt,
									(SInt16)(8 * graphRecPtr->textScaling),
									&error );
#endif
            graphRecPtr->pDC->SetPen(oldPenPtr);
         #endif // defined multispec_lin
			
			SetBackgroundColor (graphRecPtr->pDC, 192);

			LabelLinAxis (graphRecPtr, 
								graphRecPtr->xInt, 
								graphRecPtr->yInt, 
								3, 
								graphRecPtr->xESignificantDigits,
								graphRecPtr->xFSignificantDigits,
								graphRecPtr->yESignificantDigits, 
								graphRecPtr->yFSignificantDigits, 
								&error);
         
								
			DrawGraphTitle ( graphRecPtr );

			DrawGraphLabels ( graphRecPtr );
	
					// Draw data.
			
			if (graphRecPtr->drawGraphCode & 0x0001)
				FlaggedPlotV( graphRecPtr, 
										&graphRecPtr->xVector,
										&graphRecPtr->yVector, 
										graphRecPtr->flag, 
										5, 
										&error );
			
					// Draw biplot ellipses.
         //Turn it on when EllipsePlotV is done in linux version
			//#ifndef multispec_lin
				if (graphRecPtr->drawGraphCode & 0x0002){
					EllipsePlotV (graphRecPtr, &error);
            }
			//#endif
					// Draw density function for original statistics.
#if 1
         
			if (graphRecPtr->drawGraphCode & 0x0004)
				DensityFunctionPlotV (graphRecPtr, &error, 0x0004);
				
					// Draw density function for LOO statistics.
	
			if (graphRecPtr->drawGraphCode & 0x0008)
				DensityFunctionPlotV (graphRecPtr, &error, 0x0008);
				
					// Draw density function for Enhanced statistics.
	
			if (graphRecPtr->drawGraphCode & 0x0010)
				DensityFunctionPlotV (graphRecPtr, &error, 0x0010);
#endif				
			DrawGraphLegend (graphRecPtr);

			}		// end "if (continueFlag)"
		
//		if (!copyFlag)	
//			DrawGraphControls (graphRecPtr->window);
		
		}		// end "if (graphRecordPtr != NULL)" 
	
	if (!continueFlag)
		return (noErr);
	
	else		// continueFlag		
		return (error==0);
 

}		// end "DrawGraph" 	
	


//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DrawGraphControls
//
//	Software purpose:	This routine redraws the controls for the graph window.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			DoGraphWindowGrow in SGraUtil.cpp
//							DrawGraph in SGraUtil.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/23/1999
//	Revised By:			Larry L. Biehl			Date: 05/01/2003			

void DrawGraphControls (
				WindowPtr							windowPtr)
				
{                                           
	#if defined multispec_mac
	
				// Local Declarations			
		
		Rect									rect;	
//		Str31									tempString;
	
		GrafPtr								savedPort;	
		GraphPtr								graphRecordPtr;
	 
		ControlHandle						controlHandle,
												rootControlHandle;
		
		Handle								graphRecordHandle;
		
		SInt16								left,
												numberSets,
												refCon,
												setCode,
//												textWidth,
												top;	
	
		UInt16								index,
												numberControls;
																					
		Boolean								validateFlag;
		
	
				// Check input parameter.															
				
		if (windowPtr == NULL)
																							return;
																							
				//	Initialize local parameters  (graph port)									
	
		GetWindowPortBounds(windowPtr, &rect);
	     	
				// Get some graph parameters.
		
		graphRecordHandle = GetGraphRecordHandle (windowPtr);
				                   
		graphRecordPtr = (GraphPtr)GetHandlePointer(
													graphRecordHandle, kNoLock, kNoMoveHi);
													
//		if (graphRecordPtr->numberSets <= 1)
//																							return;
		
		setCode = graphRecordPtr->setCode;	
		numberSets = graphRecordPtr->numberSets;	
				
		GetPort (&savedPort);
		SetPortWindowPort (windowPtr);
			
				// Now draw the controls for the graph window.	
				
		if (gAppearanceManagerFlag)
			{	
			GetRootControl(windowPtr, &rootControlHandle);
			if (rootControlHandle != NULL)
				{																					
				CountSubControls (rootControlHandle, &numberControls);
				for (index=numberControls; index>=1; index--)
					{
					GetIndexedSubControl(rootControlHandle, index, &controlHandle);
					validateFlag = FALSE;
					refCon = (SInt16)GetControlReference(controlHandle);
					switch (refCon)
						{
						case kNextGraphSetControl:
						
									// Draw the go to next set control.											
				
							left = rect.left + 95;
							top  = rect.bottom - kSBarWidth;
							MoveControl (controlHandle, left, top);
						
							SizeControl (controlHandle, kLegendScrollWidth, kSBarWidth+1);
							break;
							
						case kPreviousGraphSetControl:
						
									// Draw the go back to previous set control.										
								
							left = rect.left + 95 + kLegendScrollWidth;
							top  = rect.bottom - kSBarWidth;
							MoveControl (controlHandle, left, top);
						
							SizeControl (controlHandle, kLegendScrollWidth, kSBarWidth+1);
							break;
							
						case kGraphVectorControl:
						
									// Draw the select vector control.										
								
							left = rect.left + 95 + 3*kLegendScrollWidth;
							top  = rect.bottom - kSBarWidth;
							MoveControl (controlHandle, left, top);
						
							SizeControl (controlHandle, kBottomButtonWidth, kSBarWidth+1);
							break;
							
						case kGraphOverlayControl:
						
									// Draw overlay options control.										
								
							left = rect.left + 95 + 4*kLegendScrollWidth+7;
							top  = rect.bottom - kSBarWidth;
							MoveControl (controlHandle, left, top);
						
							SizeControl (controlHandle, kBottomButtonWidth, kSBarWidth+1);
							break;
							
						case kGraphBinWidthControl:
						
									// Draw bin width control.										
								
							left = rect.left + 95 + 5*kLegendScrollWidth+14;
							top  = rect.bottom - kSBarWidth;
							MoveControl (controlHandle, left, top);
						
							SizeControl (controlHandle, kBottomButtonWidth, kSBarWidth+1);
							break;
								
						default:
							validateFlag = FALSE;
							break;
							
						}		// end "switch (refCon)" 
						
					#if !TARGET_API_MAC_CARBON
						if (validateFlag)
							{
							HLock ( (Handle)controlHandle );
							ValidWindowRect (windowPtr, GetControlBounds (controlHandle, &gTempRect));
							HUnlock ( (Handle)controlHandle );
							
							}		// end "if (validateFlag)"
				 	#endif	// !TARGET_API_MAC_CARBON
					
					}		// end "for (index=numberControls; index>=1; index--)"
					
				}		// end "if (rootControlHandle != NULL)"
			
			}		// end "if (gAppearanceManagerFlag)"
			
		#if !TARGET_API_MAC_CARBON
			else		// end "!gAppearanceManagerFlag"
				{
				controlHandle = (ControlHandle)((WindowPeek)windowPtr)->controlList;	
																																
				while (controlHandle != NULL)
					{
					validateFlag = TRUE;
					refCon = (SInt16)GetControlReference(controlHandle);
					switch (refCon)
						{
						case kNextGraphSetControl:
						
									// Draw the go to next set control.											
				
							left = rect.left + 95;
							top  = rect.bottom - kSBarWidth;
							MoveControl (controlHandle, left, top);
						
							SizeControl (controlHandle, kLegendScrollWidth, kSBarWidth+1);
							break;
							
						case kPreviousGraphSetControl:
						
									// Draw the go back to previous set control.										
								
							left = rect.left + 95 + kLegendScrollWidth;
							top  = rect.bottom - kSBarWidth;
							MoveControl (controlHandle, left, top);
						
							SizeControl (controlHandle, kLegendScrollWidth, kSBarWidth+1);
							break;
							
						case kGraphVectorControl:
						
									// Draw the select vector control.										
								
							left = rect.left + 95 + 3*kLegendScrollWidth;
							top  = rect.bottom - kSBarWidth;
							MoveControl (controlHandle, left, top);
						
							SizeControl (controlHandle, kBottomButtonWidth, kSBarWidth+1);
							break;
							
						case kGraphOverlayControl:
						
									// Draw overlay options control.										
								
							left = rect.left + 95 + 4*kLegendScrollWidth+7;
							top  = rect.bottom - kSBarWidth;
							MoveControl (controlHandle, left, top);
						
							SizeControl (controlHandle, kBottomButtonWidth, kSBarWidth+1);
							break;
							
						case kGraphBinWidthControl:
						
									// Draw bin width control.										
								
							left = rect.left + 95 + 5*kLegendScrollWidth+14;
							top  = rect.bottom - kSBarWidth;
							MoveControl (controlHandle, left, top);
						
							SizeControl (controlHandle, kBottomButtonWidth, kSBarWidth+1);
							break;
								
						default:
							validateFlag = FALSE;
							break;
							
						}		// end "switch (refCon)" 
						
					if (validateFlag)
						{
						HLock ( (Handle)controlHandle );
						ValidWindowRect (windowPtr, GetControlBounds (controlHandle, &gTempRect));
						HUnlock ( (Handle)controlHandle );
						
						}		// end "if (validateFlag)"
				
					controlHandle = (*controlHandle)->nextControl;
					
					}		// end "while (controlHandle != NULL)"
				
				}		// end "else !gAppearanceManagerFlag"
		 #endif	// !TARGET_API_MAC_CARBON
		
					//	Show window controls.		
						
		if (gAppearanceManagerFlag)
			{	
			GetRootControl (windowPtr, &rootControlHandle);
			if (rootControlHandle != NULL)
				{																					
				CountSubControls (rootControlHandle, &numberControls);
				for (index=numberControls; index>=1; index--)
					{
					GetIndexedSubControl (rootControlHandle, index, &controlHandle);
					refCon = (SInt16)GetControlReference(controlHandle);
					
					if ( numberSets > 1 || (refCon != kNextGraphSetControl && 
													refCon != kPreviousGraphSetControl) )
						ShowControl (controlHandle);
			     		
			     	}		// end "while (controlHandle != NULL)"
			     	
				}		// end "if (rootControlHandle != NULL)"
	     	
	     	}		// end "if (gAppearanceManagerFlag)"
	     	
		#if !TARGET_API_MAC_CARBON
			else		// !gAppearanceManagerFlag
				{
				controlHandle = (ControlHandle)((WindowPeek)windowPtr)->controlList;
				while (controlHandle != NULL)
					{
					refCon = (SInt16)GetControlReference(controlHandle);
					
					if ( numberSets > 1 || (refCon != kNextGraphSetControl && 
													refCon != kPreviousGraphSetControl) )
						ShowControl (controlHandle);
						
					controlHandle = (*controlHandle)->nextControl;
					
					}		// end "while (controlHandle != NULL)"
				
				}		// end "else !gAppearanceManagerFlag"
		#endif	// !TARGET_API_MAC_CARBON
		
		SetPort (savedPort);
	
	#endif	// defined multispec_mac                    
		                                                         
	#if defined multispec_win                                                    
//		graphRecPtr->pDC->TextOut (left, 
//											top, 
//											tempString, 
//											15 );
	#endif	// defined multispec_win 

}		// end "DrawGraphControls" 

                           
                           
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DrawGraphLabels
//
//	Software purpose:	This routine draws the labels for the graph in the
//							graph window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			CreateGraphWindow
//
//	Coded By:			Larry L. Biehl			Date: 10/09/1991
//	Revised By:			Larry L. Biehl			Date: 03/03/2017			

void DrawGraphLabels ( 
				GraphPtr								graphRecPtr )
													
{                         
	Rect*									clientRectPtr;
	
	SInt16								graphWidth,
											textWidth,
											xPos,
											yPos;

	
#	if defined multispec_win
		USES_CONVERSION;
#	endif

	if (graphRecPtr->xLabel[0] > 0 && graphRecPtr->xLabel[0] < 32)
		{
		clientRectPtr = &graphRecPtr->clientRect;
		
		textWidth = TextWidth( (UCharPtr)&graphRecPtr->xLabel, 
										1, 
										graphRecPtr->xLabel[0] );
						
		graphWidth = ( clientRectPtr->right - clientRectPtr->left ) -
								graphRecPtr->rightInset - graphRecPtr->leftInset;
						
		xPos = clientRectPtr->left + graphRecPtr->leftInset + 
															graphWidth/2 - textWidth/2;
															
		xPos = MAX(xPos, clientRectPtr->left);
			                                                         
		#if defined multispec_mac
			yPos = clientRectPtr->bottom - graphRecPtr->bottomInset + 
														(SInt16)(3.5 * graphRecPtr->textSize);
			MoveTo( xPos, yPos );
			DrawText( &graphRecPtr->xLabel, 1, graphRecPtr->xLabel[0] );
		#endif	// defined multispec_mac                    
			                                                         
		#if defined multispec_win  
			yPos = clientRectPtr->bottom - graphRecPtr->bottomInset + 
												(SInt16)(3.5 * graphRecPtr->textSize);                                                
			graphRecPtr->pDC->TextOut (xPos, 
												yPos, 
												(LPCTSTR)A2T((char*)&graphRecPtr->xLabel[1]), 
												graphRecPtr->xLabel[0] );
		#endif	// defined multispec_win 

      #if defined multispec_lin
         yPos = clientRectPtr->bottom - graphRecPtr->bottomInset + 
												(SInt16)(3.5 * graphRecPtr->textSize);                                                

			wxString xLabelPtr = wxString::FromAscii(&graphRecPtr->xLabel[1]);
         if(graphRecPtr->processorCode == kHistogramStatsProcessor)
            graphRecPtr->pDC->DrawText(xLabelPtr, (int)xPos, (int)yPos-15);
         else
            graphRecPtr->pDC->DrawText(xLabelPtr, (int)xPos, (int)yPos);     
      #endif

		}		// end "if (graphRecPtr->xLabel[0] > 0 && ..."
	
	
	if (graphRecPtr->yLabel[0] > 0 && graphRecPtr->yLabel[0] < 32)
		{
		clientRectPtr = &graphRecPtr->clientRect;
		 
		textWidth = TextWidth( 
						(UCharPtr)&graphRecPtr->yLabel, 1, graphRecPtr->yLabel[0] );
							
		xPos = clientRectPtr->left + 3;
		
		#if defined multispec_lin			
			yPos = clientRectPtr->top + (SInt16)(1.8* graphRecPtr->textSize); 
		#endif
		
		#ifndef multispec_lin
			yPos = clientRectPtr->top + (SInt16)(2.7* graphRecPtr->textSize);
		#endif
      
		yPos = MIN (yPos, clientRectPtr->top + graphRecPtr->topInset);
			                                                         
		#if defined multispec_mac
			MoveTo( xPos, yPos );
			DrawText( &graphRecPtr->yLabel, 1, graphRecPtr->yLabel[0] );
		#endif	// defined multispec_mac                    
			                                                         
		#if defined multispec_win                                                    
			graphRecPtr->pDC->TextOut (xPos, 
												yPos, 
												(LPCTSTR)A2T((char*)&graphRecPtr->yLabel[1]), 
												graphRecPtr->yLabel[0] );
		#endif	// defined multispec_win 
		
      #if defined multispec_lin
         wxString yLabelPtr = wxString::FromAscii(&graphRecPtr->yLabel[1]);
         graphRecPtr->pDC->DrawText(yLabelPtr, xPos, yPos);
      #endif
		
		}		// end "if (graphRecPtr->yLabel[0] > 0 && ..."

}		// end "DrawGraphLabels"    

                           
                           
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DrawGraphLegend
//
//	Software purpose:	This routine draws the legend for the graph in the specified
//							graph window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			CreateGraphWindow
//
//	Coded By:			Larry L. Biehl			Date: 01/26/1999
//	Revised By:			Larry L. Biehl			Date: 03/03/2017			

void DrawGraphLegend ( 
				GraphPtr								graphRecPtr)
													
{                         
	Rect*									clientRectPtr;
	
	char									*savedStringPtr,
											*stringPtr,
											*vectorDisplayPtr;
	
	HSInt32Ptr							vectorPaletteColorPtr;
	
	UInt32								line;
	
	SInt16								lineHeight,
											textWidth,
											xPos,
											yPos;
	

#	if defined multispec_win
		USES_CONVERSION;
#	endif
	
	if (graphRecPtr->flag == NU_HISTOGRAM_PLOT && graphRecPtr->numberVectors > 1)
		{
		clientRectPtr = &graphRecPtr->clientRect;
		
		savedStringPtr = (char*)GetHandlePointer (graphRecPtr->vectorLabelHandle,
															kLock,
															kNoMoveHi);
	
      
		vectorPaletteColorPtr = (HSInt32Ptr)GetHandlePointer (
												graphRecPtr->vectorPaletteColorHandle,
												kLock,
												kNoMoveHi);
	
		vectorDisplayPtr = (char*)GetHandlePointer (
												graphRecPtr->vectorDisplayHandle,
												kLock,
												kNoMoveHi);
		
				// Get the width of the longest vector name
		
		textWidth = 0;
		stringPtr = savedStringPtr;
		for (line=0; line<(UInt32)graphRecPtr->numberVectors; line++)
			{
			textWidth = MAX( textWidth,
										TextWidth( (UCharPtr)stringPtr, 
											1, 
											(SInt16)stringPtr[0] ) );
			
			stringPtr += 32;
											
			}		// end "for (index=0; index<graphRecPtr->..."
			
		stringPtr = savedStringPtr;
		
		xPos = clientRectPtr->right - graphRecPtr->rightInset - 10 - textWidth;
		xPos = MAX(xPos, clientRectPtr->left + graphRecPtr->leftInset);
		                                               
		#if defined multispec_mac
			yPos = (SInt16)(clientRectPtr->top + graphRecPtr->topInset + 1.3*graphRecPtr->textSize);
			lineHeight = (SInt16)(1.2 * graphRecPtr->textSize);
		#endif	// defined multispec_mac                    
					                                                         
		#if defined multispec_win  
			yPos = (SInt16)(clientRectPtr->top + graphRecPtr->topInset + 1.4*graphRecPtr->textSize);
			lineHeight = (SInt16)(1.5 * graphRecPtr->textSize); 
		#endif	// defined multispec_win  
		
      #if defined multispec_lin
         yPos = (SInt16)(clientRectPtr->top + graphRecPtr->topInset + 1.4*graphRecPtr->textSize);
			lineHeight = (SInt16)(1.5 * graphRecPtr->textSize); 
      #endif
		
		SetBackgroundColor (graphRecPtr->pDC, 255);
		
		for (line=0; line<(UInt32)graphRecPtr->numberVectors; line++)
			{	  
			if (vectorDisplayPtr [line] > 0)
				{                                                                
				#if defined multispec_mac
					ForeColor (vectorPaletteColorPtr[line]);
					MoveTo( xPos, yPos );
					DrawText( stringPtr, 1, (SInt16)stringPtr[0] );
				#endif	// defined multispec_mac                    
					                                                         
				#if defined multispec_win     
					graphRecPtr->pDC->SetTextColor (vectorPaletteColorPtr[line]);
					graphRecPtr->pDC->TextOut (xPos, 
														yPos, 
														(LPCTSTR)A2T(&stringPtr[1]), 
														stringPtr[0] );
				#endif	// defined multispec_win
				
            #if defined multispec_lin
               wxColour color;
               color.Set(vectorPaletteColorPtr[line]);
               graphRecPtr->pDC->SetTextForeground(color);
               graphRecPtr->pDC->DrawText((CharPtr)&stringPtr[1], xPos, yPos);
            #endif
				
				yPos += lineHeight;
				
				}		// end "if (vectorDisplayPtr [line] > 0)"
				
			stringPtr += 32;
			
			}		// end "for (line=0; line<graphRecPtr->numberVectors; line++)"
		
				// Make sure that the pen color is set back to black.
				
		MForeColor (graphRecPtr->pDC, blackColor);
			
		CheckAndUnlockHandle (graphRecPtr->vectorPaletteColorHandle);
		CheckAndUnlockHandle (graphRecPtr->vectorLabelHandle);
		CheckAndUnlockHandle (graphRecPtr->vectorDisplayHandle);
		
		}		// end "if (graphRecPtr->flag == NU_HISTOGRAM_PLOT && ..."

}		// end "DrawGraphLegend"     


                            
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DrawGraphTitle
//
//	Software purpose:	This routine draws the title for the graph in the
//							graph window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			CreateGraphWindow
//
//	Coded By:			Larry L. Biehl			Date: 10/08/1991
//	Revised By:			Larry L. Biehl			Date: 03/03/2017			

void DrawGraphTitle ( 
				GraphPtr								graphRecPtr)
													
{                           
	Rect*									clientRectPtr;
	
	SInt16								graphWidth,
											textWidth,
											xPos,
											yPos;

	
#	if defined multispec_win
		USES_CONVERSION;
#	endif
	
	if (graphRecPtr->title[0] > 0 && graphRecPtr->title[0] < 64)
		{                             
		clientRectPtr = &graphRecPtr->clientRect;
		
		textWidth = TextWidth( 
						(UCharPtr)&graphRecPtr->title, 1, graphRecPtr->title[0] );
						
		graphWidth = ( clientRectPtr->right - clientRectPtr->left ) -
								graphRecPtr->rightInset - graphRecPtr->leftInset;
						
		xPos = clientRectPtr->left + graphRecPtr->leftInset + 
															graphWidth/2 - textWidth/2;
													
		xPos = MAX(xPos, clientRectPtr->left);
		
		#ifdef multispec_lin
			yPos = clientRectPtr->top + (SInt16)(0.5 * graphRecPtr->textSize);
		#endif
		
		#ifndef multispec_lin
			yPos = clientRectPtr->top + (SInt16)(1.5 * graphRecPtr->textSize);
		#endif
		
		yPos = MIN (yPos, clientRectPtr->top + graphRecPtr->topInset); 
			                                                         
		#if defined multispec_mac
			MoveTo( xPos, yPos );
			DrawText( &graphRecPtr->title, 1, graphRecPtr->title[0] ); 
		#endif	// defined multispec_mac                    
			                                                         
		#if defined multispec_win                                                    
			graphRecPtr->pDC->TextOut (xPos, 
												yPos, 
												(LPCTSTR)A2T((char*)&graphRecPtr->title[1]), 
												graphRecPtr->title[0] );
		#endif	// defined multispec_win

      #if defined multispec_lin
         graphRecPtr->pDC->DrawText((CharPtr)&graphRecPtr->title[1], xPos, yPos);
      #endif
         
		
		if (graphRecPtr->title2[0] > 0 && graphRecPtr->title2[0] < 32)
			{
			textWidth = TextWidth( 
							(UCharPtr)&graphRecPtr->title2, 1, graphRecPtr->title2[0] );
							
			xPos = clientRectPtr->left + graphRecPtr->leftInset + 
																graphWidth/2 - textWidth/2;
														
			xPos = MAX(xPos, clientRectPtr->left);

			#ifdef multispec_lin
				yPos = clientRectPtr->top + (SInt16)(1.7 * graphRecPtr->textSize);
			#endif
		
			#ifndef multispec_lin
				yPos = clientRectPtr->top + (SInt16)(2.7 * graphRecPtr->textSize);
			#endif

			yPos = MIN (yPos, clientRectPtr->top + graphRecPtr->topInset);
			
			if (graphRecPtr->flag == NU_HISTOGRAM_PLOT)
				{                                                  
				#if defined multispec_mac
					MoveTo( xPos, yPos );
					DrawText( &graphRecPtr->title2, 1, graphRecPtr->title2[0] ); 
				#endif	// defined multispec_mac                    
					                                                         
				#if defined multispec_win                                                    
					graphRecPtr->pDC->TextOut (xPos, 
														yPos, 
														(LPCTSTR)A2T((char*)&graphRecPtr->title2[1]), 
														graphRecPtr->title2[0] );
				#endif	// defined multispec_win 

            #if defined multispec_lin
               graphRecPtr->pDC->DrawText((CharPtr)&graphRecPtr->title2[1], xPos, yPos);
            #endif
				
				}		// end "if (graphRecPtr->flag == NU_HISTOGRAM_PLOT)"
				
			else		// graphRecPtr->flag != NU_HISTOGRAM_PLOT
				{                                                         
				#if defined multispec_mac
					MoveTo( xPos, yPos );
					DrawText( &graphRecPtr->title2, 1, 1 );
					
					ForeColor(redColor);
					DrawText( &graphRecPtr->title2, 2, 8 );
					
					ForeColor(greenColor);
					DrawText( &graphRecPtr->title2, 10, 11 );
					
					ForeColor(blueColor);
					DrawText( &graphRecPtr->title2, 21, 8 );
					
					ForeColor(blackColor);
					DrawText( &graphRecPtr->title2, 29, 1 );
				#endif	// defined multispec_mac                    
					                                                         
				#if defined multispec_win  
					graphRecPtr->pDC->MoveTo( xPos, yPos );
					graphRecPtr->pDC->SetTextAlign( TA_BASELINE + TA_UPDATECP ); 	
					graphRecPtr->pDC->SetTextColor( RGB(0, 0, 0) );                                                    
					graphRecPtr->pDC->TextOut (xPos, 
														yPos, 
														(LPCTSTR)A2T((char*)&graphRecPtr->title2[1]), 
														1 ); 
														
					graphRecPtr->pDC->SetTextColor( RGB(255, 0, 0) );                                                    
					graphRecPtr->pDC->TextOut (xPos, 
														yPos, 
														(LPCTSTR)A2T((char*)&graphRecPtr->title2[2]), 
														8 );
														
					graphRecPtr->pDC->SetTextColor( RGB(0, 128, 0) );                                                    
					graphRecPtr->pDC->TextOut (xPos, 
														yPos, 
														(LPCTSTR)A2T((char*)&graphRecPtr->title2[10]), 
														11 ); 
														
					graphRecPtr->pDC->SetTextColor (RGB(0, 0, 255));                                                    
					graphRecPtr->pDC->TextOut (xPos, 
														yPos, 
														(LPCTSTR)A2T((char*)&graphRecPtr->title2[21]), 
														8 ); 
														 
					graphRecPtr->pDC->SetTextColor (RGB(0, 0, 0));                                                    
					graphRecPtr->pDC->TextOut (xPos, 
														yPos, 
														(LPCTSTR)A2T((char*)&graphRecPtr->title2[29]), 
														1 ); 
					graphRecPtr->pDC->SetTextAlign (TA_BASELINE);
				#endif	// defined multispec_win 

            #if defined multispec_lin 
               wxColour color;
					//graphRecPtr->pDC->MoveTo( xPos, yPos );
   
               //graphRecPtr->pDC->SetAxisOrientation(xPos, yPos);
					//graphRecPtr->pDC->SetTextAlign( TA_BASELINE + TA_UPDATECP ); 	
               color.Set(RGB(0, 0, 0));
               graphRecPtr->pDC->SetTextForeground(color);
               wxString title2Ptr = wxString::FromAscii(&graphRecPtr->title2[1]);
               graphRecPtr->pDC->DrawText(title2Ptr[0], xPos, yPos);
					
               color.Set(RGB(255, 0, 0));
               graphRecPtr->pDC->SetTextForeground(color);
               graphRecPtr->pDC->DrawText(title2Ptr.SubString(1, 8), xPos + 5,
                                          yPos);
               
               color.Set(RGB(0, 128, 0));
               graphRecPtr->pDC->SetTextForeground(color);
               graphRecPtr->pDC->DrawText(title2Ptr.SubString(9, 18), xPos + (6*10),
                                          yPos);
               
               color.Set(RGB(0, 0, 255));
               graphRecPtr->pDC->SetTextForeground(color);
               graphRecPtr->pDC->DrawText(title2Ptr.SubString(19, 27), xPos + (6*22),
                                          yPos);
               color.Set(RGB(0, 0, 0));
               graphRecPtr->pDC->SetTextForeground(color);
               graphRecPtr->pDC->DrawText(title2Ptr[28], xPos + (6*33),
                                          yPos);
 #if 0
               color.Set(RGB(255, 0, 0));
               //graphRecPtr->pDC->SetTextForeground(color);
					wxString title2Ptr1 = wxString::FromAscii(&graphRecPtr->title2[2]);
               //graphRecPtr->pDC->DrawText(wxT((CharPtr)&graphRecPtr->title2[2]), xPos, yPos);
               graphRecPtr->pDC->DrawText(title2Ptr1, xPos, yPos);

               color.Set(RGB(0, 128, 0));
					title2Ptr = wxString::FromAscii(&graphRecPtr->title2[10]);
               //graphRecPtr->pDC->DrawText(wxT((CharPtr)&graphRecPtr->title2[10]), xPos, yPos);
               graphRecPtr->pDC->DrawText(title2Ptr, xPos, yPos);
               color.Set(RGB(0, 0, 255));
					title2Ptr = wxString::FromAscii(&graphRecPtr->title2[21]);
               //graphRecPtr->pDC->DrawText(wxT((CharPtr)&graphRecPtr->title2[21]), xPos, yPos);
               graphRecPtr->pDC->DrawText(title2Ptr, xPos, yPos);
               color.Set(RGB(0, 0, 0));
					title2Ptr = wxString::FromAscii(&graphRecPtr->title2[29]);
               //graphRecPtr->pDC->DrawText(wxT((CharPtr)&graphRecPtr->title2[29]), xPos, yPos);
               graphRecPtr->pDC->DrawText(title2Ptr, xPos, yPos);
					//graphRecPtr->pDC->SetTextAlign( TA_BASELINE );
#endif
				#endif	// defined multispec_lin
				
				}		// end "else graphRecPtr->flag != NU_HISTOGRAM_PLOT"
			
			}		// end "if (graphRecPtr->title2[0] > 0 && ..." 
		
		}		// end "if (graphRecPtr->title[0] > 0 && ..." 

}		// end "DrawGraphTitle"       



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ForceGraphicsCodeResourceLoad
//
//	Software purpose:	The purpose of this routine is to force the 'Graphics' code
//							resource to be loaded into memory.  It is called by any
//							processor that needs graphics code resource.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/03/1991
//	Revised By:			Larry L. Biehl			Date: 10/03/1991

void ForceGraphCodeResourceLoad (void)

{
	
			// If spare memory had to be used to load code resources, then exit	
			// routine.																				
			
	if (gMemoryTypeNeeded < 0)
																							return;
																							
			// Code resources loaded okay, so set flag back for non-Code			
			// resources.																			
			
	gMemoryTypeNeeded = 0;

}		// end "ForceGraphicsCodeResourceLoad"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle GetGraphRecordHandle
//
//	Software purpose:	The purpose of this routine is to get the graph record 
//							handle for the input window.
//
//	Parameters in:		A window pointer
//
//	Parameters out:	None
//
// Value Returned:	Handle to the graph record.
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/08/1996
//	Revised By:			Larry L. Biehl			Date: 01/26/1999

Handle GetGraphRecordHandle (
				WindowPtr							windowPtr)

{
	Handle								graphRecordHandle = NULL; 
	
	
	#if defined multispec_mac 
		Handle								windowInfoHandle;
		
		 				
		windowInfoHandle = GetWindowInfoHandle (windowPtr);
		
		graphRecordHandle = GetGraphRecordHandle (windowInfoHandle);
	#endif		// defined multispec_mac 
	
	#if defined multispec_win 					
		if (windowPtr != NULL)                         	
			graphRecordHandle = ((CMGraphView*)windowPtr)->GetGraphRecordHandle();
	#endif		// defined multispec_win
	
   #if defined multispec_lin
      if (windowPtr != NULL)                         	
			graphRecordHandle = ((CMGraphView*)windowPtr)->GetGraphRecordHandle();
   #endif
		
	return (graphRecordHandle);

}		// end "GetGraphRecordHandle" 



Handle GetGraphRecordHandle (
				Handle								windowInfoHandle)

{
	Handle								graphRecordHandle = NULL; 
	
	
	#if defined multispec_mac 
		CMGraphView*						graphViewCPtr;
		WindowInfoPtr						windowInfoPtr;
		
		 				
		windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
															windowInfoHandle, kNoLock, kNoMoveHi);
																
		if (windowInfoPtr != NULL)
			{												
			if (windowInfoPtr->graphViewCPtr != NULL)
				{
				graphViewCPtr = windowInfoPtr->graphViewCPtr;
				
												return (graphViewCPtr->GetGraphRecordHandle() );
				
				}		// end "if (windowInfoPtr->graphViewCPtr != NULL)"
			
			}		// end "if (windowInfoPtr != NULL)"
	#endif		// defined multispec_mac 
	
	#if defined multispec_win 					
																
	#endif		// defined multispec_win
		
	return (graphRecordHandle);

}		// end "GetGraphRecordHandle" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		GraphPtr GetGraphRecordPtr
//
//	Software purpose:	The purpose of this routine is to get the pointer
//							to the graph record for the input window.
//
//	Parameters in:		A window pointer
//
//	Parameters out:	None
//
// Value Returned:	Pointer to the graph record.
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/03/1991
//	Revised By:			Larry L. Biehl			Date: 10/04/1991

GraphPtr GetGraphRecordPtr (
				CMGraphView*						graphViewCPtr, 
				SignedByte*							handleStatusPtr, 
				Handle*								graphRecHandlePtr)

{
	GraphPtr								graphRecordPtr;    
	
	
			// Initialize local variables.											
			
	graphRecordPtr = NULL;
	
	if (graphViewCPtr != NULL)
		{        
		*graphRecHandlePtr = graphViewCPtr->GetGraphRecordHandle();
		
		if (*graphRecHandlePtr != NULL)
			{
			*handleStatusPtr = MHGetState (*graphRecHandlePtr);
				                   
			graphRecordPtr = (GraphPtr)GetHandlePointer(
													*graphRecHandlePtr, kLock, kMoveHi);	
				
			}		// end "if (*graphRecHandlePtr != NULL)"
			
		}		// end "if (graphViewCPtr != NULL)" 
		
	return (graphRecordPtr);

}		// end "GetGraphRecordPtr" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetGraphLabels
//
//	Software purpose:	This routine gets the x and y axis labels for the 
//							graph in the graph window based on the processor
//							the graph is created under.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			CreateGraphWindow
//
//	Coded By:			Larry L. Biehl			Date: 10/09/1991
//	Revised By:			Larry L. Biehl			Date: 01/01/2006			

void GetGraphLabels (
				GraphPtr								graphRecordPtr)
													
{
	char									number[64];
	
	double								maxValue,
											minValue;
	
	char									*xLabelStringPtr,
											*yLabelStringPtr;
											
	SInt32								error,
											ticLabelWidth;
											
	
	xLabelStringPtr = (char*)&graphRecordPtr->xLabel;
	yLabelStringPtr = (char*)&graphRecordPtr->yLabel;
	
	if (xLabelStringPtr != NULL)
		{
		switch (graphRecordPtr->processorCode)
			{
			case kListDataProcessor:
            
			case kSelectionGraphProcessor:
				xLabelStringPtr[0] = sprintf ((char*)&xLabelStringPtr[1],
															"Channel");
		
				graphRecordPtr->xESignificantDigits = 0; 
				graphRecordPtr->xFSignificantDigits = 0; 
			
				break;
				
			case kBiPlotDataProcessor:
				if (gBiPlotDataSpecsPtr->featureTransformationFlag)
					xLabelStringPtr[0] = sprintf ((char*)&xLabelStringPtr[1],
													"Feature %hd",
													gBiPlotDataSpecsPtr->axisFeaturePtr[0]+1);
									
				else		// !...->featureTransformationFlag 
					xLabelStringPtr[0] = sprintf ((char*)&xLabelStringPtr[1],
													"Channel %hd",
													gBiPlotDataSpecsPtr->axisFeaturePtr[0]+1);
		
				graphRecordPtr->xESignificantDigits = 0; 
				graphRecordPtr->xFSignificantDigits = 0;  
				
				break;
				
			case kHistogramStatsProcessor:
				xLabelStringPtr[0] = sprintf ( (char*)&xLabelStringPtr[1],
														"Data Value");
		
//				graphRecordPtr->xSignificantDigits = 0; 
				
				break;
			
			}		// end "switch (graphRecordPtr->processorCode)" 
		
		}		// end "if (xLabelStringPtr != NULL)" 

	if (yLabelStringPtr != NULL)
		{
		switch (graphRecordPtr->processorCode)
			{
			case kListDataProcessor:
			case kSelectionGraphProcessor:
				yLabelStringPtr[0] = sprintf ((char*)&yLabelStringPtr[1],
														"Value");
														
//				minValue = graphRecordPtr->yMin;
//				maxValue = graphRecordPtr->yMax;
				minValue = graphRecordPtr->yScaleMin;
				maxValue = graphRecordPtr->yScaleMax;
				
//				graphRecordPtr->ySignificantDigits = 0; 
			
				break;
				
			case kBiPlotDataProcessor:
				if (gBiPlotDataSpecsPtr->featureTransformationFlag)
					yLabelStringPtr[0] = sprintf ((char*)&yLabelStringPtr[1],
													"Feature %hd",
													gBiPlotDataSpecsPtr->axisFeaturePtr[1]+1);
									
				else		// !...->featureTransformationFlag 
					yLabelStringPtr[0] = sprintf ((char*)&yLabelStringPtr[1],
													"Channel %hd",
													gBiPlotDataSpecsPtr->axisFeaturePtr[1]+1);
														
				minValue = graphRecordPtr->yMin;
				maxValue = graphRecordPtr->yMax;
				graphRecordPtr->yESignificantDigits = 0;
				graphRecordPtr->yFSignificantDigits = 0;
				
				break;
				
			case kHistogramStatsProcessor:
				yLabelStringPtr[0] = sprintf ((char*)&yLabelStringPtr[1],
														"Count");
														
				minValue = 0;
				maxValue = gStatHistogramSpecsPtr->totalNumberValues;
//				graphRecordPtr->yESignificantDigits = 0; 
//				graphRecordPtr->yFSignificantDigits = 0; 
				
				break;
			
			}		// end "switch (graphRecordPtr->processorCode)" 
			
				// Get length of max y tic label string.
				
		FormatR (number, 
						&minValue, 
						graphRecordPtr->yESignificantDigits, 
						graphRecordPtr->yFSignificantDigits,
						false, 
						&error);
		ticLabelWidth = TextWidth((UCharPtr)number, 0, strlen(number));
				
		FormatR (number, 
						&maxValue, 
						graphRecordPtr->yESignificantDigits, 
						graphRecordPtr->yFSignificantDigits,
						false, 
						&error);
		graphRecordPtr->yTicLabelWidth = TextWidth((UCharPtr)number, 0, strlen(number));
		graphRecordPtr->yTicLabelWidth = (SInt16)
											MAX (graphRecordPtr->yTicLabelWidth, ticLabelWidth);
		
		graphRecordPtr->yTicLabelWidth = MAX (39, graphRecordPtr->yTicLabelWidth+8);
		
		}		// end "if (yLabelString != NULL)" 
		
}		// end "GetGraphLabels"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetGraphTitle
//
//	Software purpose:	This routine gets the title for the graph in the
//							graph window based on the processor the graph
//							is created under.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			CreateGraph in SGraUtil.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/08/1991
//	Revised By:			Larry L. Biehl			Date: 01/23/1999			

void GetGraphTitle (
				GraphPtr								graphRecordPtr, 
				SInt32								lineStart, 
				SInt32								columnStart, 
				SInt32								lineEnd, 
				SInt32								columnEnd)
													
{
	if (graphRecordPtr != NULL)
		{
		switch (graphRecordPtr->processorCode)
			{
			case kListDataProcessor:
			case kSelectionGraphProcessor:
				Str15	tempString;
				
				tempString[0] = 0;
				if (graphRecordPtr->selectionTypeCode == kPolygonType)
					sprintf ( (char*)tempString,
										"Polygon ");
				
				if (columnEnd <= columnStart && lineEnd <= lineStart)
					{
					sprintf ( 	(char*)&graphRecordPtr->title[1],
										"%sLine %ld - Column %ld Data Value",
										tempString,
										lineStart,
										columnStart);
										
					graphRecordPtr->title2[1] = 0;
					
					}		// end "if (columnEnd <= columnStart && ..." 
										
				else		// columnEnd > columnStart || lineEnd > lineStart)  
					{
					sprintf ( 	(char*)&graphRecordPtr->title[1],
										"%sLines %ld-%ld, Columns %ld-%ld Data Values",
										tempString,
										lineStart,
										lineEnd,
										columnStart,
										columnEnd);
										
					sprintf ( 	(char*)&graphRecordPtr->title2[1],
										"(Average, +-Std Dev, Min-Max)");
										
					}		// end "else columnEnd > columnStart || ..." 
										
				graphRecordPtr->title[0] = strlen ( (char*)&graphRecordPtr->title[1] );
				graphRecordPtr->title2[0] = strlen ( (char*)&graphRecordPtr->title2[1] );
			
				break;
            //Turn it on when GetBiPlotGraphTitle done in linux version
//#ifndef multispec_lin		
			case kBiPlotDataProcessor:
				GetBiPlotGraphTitle (graphRecordPtr);
				break;
//#endif
				                  
//			#if defined multispec_mac
			case kHistogramStatsProcessor:
				SetStatHistogramGraphTitles (graphRecordPtr);
				break;
//			#endif	// defined multispec_mac   
				
			}		// end "switch (graphRecordPtr->processorCode)"
			
		}		// end "if (graphRecordPtr != NULL)"
		
}		// end "GetGraphTitle" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetGraphWindowLocation
//
//	Software purpose:	This routine loads the global variables containing the
//							graph window location and return the graph window size.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 08/30/1995
//	Revised By:			Larry L. Biehl			Date: 08/30/1995			

void GetGraphWindowLocation (
				SInt16*								xSizePtr,
				SInt16*								ySizePtr)
													
{  
	Rect									rect;
	
	        
			// Get size of screen to place the window within.  
	
	#if defined multispec_mac 		
		BitMap								bitMap;
		
			     
		GetQDGlobalsScreenBits (&bitMap);
		rect = bitMap.bounds;
		if (gColorQDflag)
			{     
			GDHandle gdHandle = GetMainDevice ();
			rect = (*gdHandle)->gdRect;
			
			}		// end "if (gColorQDflag)"
	#endif	// defined multispec_mac                      
	
	#if defined multispec_win 			                               
		CFrameWnd* pMainFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
		pMainFrame->GetClientRect ( (tagRECT*)&rect);
		
				// Take the status bar into account.
				
		rect.bottom -= kStatusBarHeight; 
	#endif	// defined multispec_win 
	
	gNextGraphicsWindowStart.h -= gGraphicsWindowOffset; 
	gNextGraphicsWindowStart.v -= 10;  
	 
	gGraphicsWindowOffset = -gGraphicsWindowOffset;
	
	if (gNextGraphicsWindowStart.v < 40 ||
			rect.right - gNextGraphicsWindowStart.h - 3 < *xSizePtr ||
			rect.bottom - gNextGraphicsWindowStart.v - 3 < *ySizePtr)
		{                                                            
		gNextGraphicsWindowStart.h	= MAX( (rect.right - *xSizePtr - 9), 5 );
		gNextGraphicsWindowStart.v	= MAX( 
								(rect.bottom - *ySizePtr - 9), 
								40 ); 
		if (gOSXFlag)
			gNextGraphicsWindowStart.v	= MAX( 
								(gNextGraphicsWindowStart.v - 70), 
								40 );
		
		gGraphicsWindowOffset = 10;
		
		}		// end "if (gNextGraphicsWindowStart.v < 40)" 
		                                                                  
	*xSizePtr = MIN(rect.right - 3 - gNextGraphicsWindowStart.h, 
						*xSizePtr); 
						
	*ySizePtr = MIN(rect.bottom - 3 - gNextGraphicsWindowStart.v, 
						*ySizePtr);

}		// end "GetGraphWindowLocation" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		double GetUserHistogramBinWidth
//
//	Software purpose:	The purpose of this routine is to determine the min and max
//							values for all graph windows that belong to the input 
//							image window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/08/2006
//	Revised By:			Larry L. Biehl			Date: 02/14/2006			

double GetUserHistogramBinWidth (
				GraphPtr								graphRecordPtr,
				double*								histogramBinWidthPtr,
				double								xMin,
				double								xMax)

{
	double								binWidth,
											histogramBinWidth,
											numberBinWidths,
											screen_width,
											x_scale;
								
	Rect*									clientRectPtr;
											
							
			// Allow for width of data value bin.
			
	histogramBinWidth = histogramBinWidthPtr[graphRecordPtr->set-1];
	
      
	if (graphRecordPtr->histogramBinCode <= 0)
		{
				// Determine how many bin widths represent 2 pixels in the screen
				// window. The default bin width is the number of original bin widths
				// that will be larger than or equal to 2 pixels.
				
      
		clientRectPtr = &graphRecordPtr->clientRect;
		screen_width = (clientRectPtr->right - clientRectPtr->left) -
								graphRecordPtr->rightInset - graphRecordPtr->leftInset;
		
		x_scale = screen_width / (xMax - xMin);
      
//		binWidth = 2 * histogramBinWidth / x_scale;
		numberBinWidths = 2 / (x_scale * histogramBinWidth);
		
//		if (binWidth <= histogramBinWidth)
//		if (binWidth <= 1)
//			binWidth = histogramBinWidth;
			
//		else		// binWidth > histogramBinWidth
//			{
//			binWidth = ceil (binWidth/histogramBinWidth);
			numberBinWidths = ceil (numberBinWidths);
			binWidth = numberBinWidths * histogramBinWidth;
         
			
//			}		// end "else binWidth > histogramBinWidth"
		
		}		// end "if (graphRecordPtr->histogramBinCode <= 0)"
		
	else{		// graphRecordPtr->histogramBinCode > 0
		binWidth = graphRecordPtr->histogramBinCode * histogramBinWidth;
      
	}	
	return (binWidth);

}		// end "GetUserHistogramBinWidth" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetLikeWindowMinMax
//
//	Software purpose:	The purpose of this routine is to determine the min and max
//							values for all graph windows that belong to the input 
//							image window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/08/1996
//	Revised By:			Larry L. Biehl			Date: 03/14/2001			

void GetLikeWindowMinMax (
				WindowPtr							imageWindowPtr,
				double*								minPtr,
				double*								maxPtr)

{
	GraphPtr								graphRecordPtr;
	
	Handle								graphRecordHandle;
	
	UInt16								index;
	
		
				// Loop through the graph windows.													
				
	if (gNumberOfGWindows > 0)
		{
		*minPtr = DBL_MAX;
		*maxPtr = -DBL_MAX;
		
		for (index=gNumberOfIWindows+gNumberOfGWindows+2; 
				index>(UInt16)(gNumberOfIWindows+2); 
				index--)
			{
			graphRecordHandle = GetGraphRecordHandle ( gWindowList[index] );	
			graphRecordPtr = (GraphPtr)GetHandlePointer (
													graphRecordHandle, kNoLock, kNoMoveHi);	
							
			if (graphRecordPtr != NULL &&
					graphRecordPtr->imageWindow == imageWindowPtr &&
						graphRecordPtr->processorCode == kSelectionGraphProcessor)
				{
				*minPtr = MIN(*minPtr, graphRecordPtr->yDataMin);
				*maxPtr = MAX(*maxPtr, graphRecordPtr->yDataMax);
				
				}		// end "if (...->imageWindow == imageWindowPtr && ...)"
															
			}		// end "for (index=gNumberOfIWindows ..."
			
		}		// end "if (gNumberOfGWindows > 0)" 
	
}		// end "GetLikeWindowMinMax" 		



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		WindowPtr GetSelectionGraphImageWindow
//
//	Software purpose:	This routine handles the next/previous channel action in 
//							TrackControl for next and previous channel control actions.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 08/13/2013
//	Revised By:			Larry L. Biehl			Date: 08/13/2013			

WindowPtr GetSelectionGraphImageWindow ( 
				CMGraphView*						graphViewCPtr)
				
{	
	GraphPtr								graphRecordPtr;
	Handle								graphRecordHandle;
	
	
	if (graphViewCPtr != NULL)
		{
		graphRecordHandle = graphViewCPtr->GetGraphRecordHandle();		
		graphRecordPtr = (GraphPtr)GetHandlePointer(
												graphRecordHandle,
												kNoLock, 
												kNoMoveHi);
												
		if (graphRecordPtr != NULL)
			return (graphRecordPtr->imageWindow);
													
		}		// end "if (graphViewCPtr != NULL)"
		
	return (NULL);
	
}		// end "GetSelectionGraphImageWindow" 	
	


#if defined multispec_mac 
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void GraphChangeSetControlEvent
//
//	Software purpose:	This routine handles the next/previous channel action in 
//							TrackControl for next and previous channel control actions.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/26/1999
//	Revised By:			Larry L. Biehl			Date: 11/15/2002			

pascal void GraphChangeSetControlEvent (
				ControlHandle						theControl, 
				SInt16								partCode)
				
{	
	Point									mousePoint;
	Rect									controlRect;
	
	GraphPtr								graphRecordPtr;
	
	Handle								graphRecordHandle; 
	
	SInt16								refCon;
	
	Boolean								nextGraphFlag;
	
	
			// Return if mouse location has moved outside of control area.			
			
	if (partCode == 0)
																							return;
																							
	GetControlBounds (theControl, &controlRect);
																							
	while ( StillDown() ) 
		{				
				// Only zoom if the mouse is still in the control.
				
		::GetMouse(&mousePoint);
		if ( !PtInRect (mousePoint, &controlRect) )
																							return;
	
				// If the command key is down, then do not need to wait
				// 'gControlOffset' ticks - zoom away.
		
		nextGraphFlag = TRUE;
		if ( !(gEventRecord.modifiers & cmdKey) )
			{				
					// Make certain that at least 'gControlOffset' ticks 
					// have passed since the last zoom operation.	This is 
					// need for very fast systems where one click of the 
					// mouse may cause two loops through this routine.																		
																									
			if (TickCount() < gNextTime + gControlOffset)
				nextGraphFlag = FALSE;
																								
			}		// end "if ( !(gEventRecord.modifiers & cmdKey) )"
			
		if (nextGraphFlag)
			{
			gNextTime = TickCount();
																									
					// Initialize variables.
			
			refCon = (SInt16)GetControlReference (theControl);
			
			graphRecordHandle = GetGraphRecordHandle (gActiveWindowInfoH);
			graphRecordPtr = (GraphPtr)GetHandlePointer(
												graphRecordHandle, kNoLock, kNoMoveHi);
			
			DoNextOrPreviousChannel	(graphRecordPtr->graphViewCPtr, refCon);
			
					// Do an event available to load into the gEventRecord the status	
					// of the command key.
					
			#if TARGET_API_MAC_CARBON
				gEventRecord.modifiers = GetCurrentKeyModifiers();
			#else		// !TARGET_API_MAC_CARBON
				EventAvail (updateMask, &gEventRecord);
			#endif	// TARGET_API_MAC_CARBON, else ...
			
			}		// end "if (nextGraphFlag)"
			
		}		// end "while ( StillDown() )"

}		// end "GraphChangeSetControlEvent" 	 
#endif	// defined multispec_mac    
	


#if defined multispec_mac
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GraphWControlEvent
//
//	Software purpose:	This routine handles control events in graph windows. 
//							Graph window control events include advance to next set, go
//							back to previous set.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			DoMouseDnContent
//
//	Coded By:			Larry L. Biehl			Date: 01/23/1999
//	Revised By:			Larry L. Biehl			Date: 11/23/2008			

void GraphWControlEvent (
				ControlHandle						theControl, 
				WindowPtr							windowPtr, 
				Point									location, 
				SInt16								partCode)
				
{
			// Local Structures and Variables
	
	CMGraphView*						graphViewCPtr;		
	GraphPtr								graphRecordPtr;
			
	Handle								graphRecordHandle;
	MenuHandle							popUpMenuHandle;
	
	//#ifdef AVAILABLE_MAC_OS_X_VERSION_10_5_AND_LATER
#	if __MAC_OS_X_VERSION_MAX_ALLOWED >= 1050
		MenuItemIndex						selection;
#	else
		SInt16								selection;
#	endif
	
	OSErr									errCode;	
			
	SInt16								index,
											numberMenuItems,
											refCon;
	
	
			// Get the refCon of the control. This will contain a code for which 
			// control it is.																		
			// refCon = 0: Vertical scroll bar												
			// refCon = 1: Horizontal scroll bar									
	
	refCon = (SInt16)GetControlReference (theControl);
	
	if (refCon >= kNextGraphSetControl && refCon <= kGraphBinWidthControl)
		{
		gNextTime = 0;
		
		switch (refCon)
			{
			case kNextGraphSetControl:				// Advance to next graph set
			case kPreviousGraphSetControl:		// Go back to previous graph set 
			
						// If there was a mouseup in this control, advance to the
						// next graph set or go back to the previous one.						
					
				partCode = TrackControl (theControl, location, gGraphChangeSetControlActionPtr);
//				if (partCode != 0)
				UpdateGraphControls (windowPtr);
					
				break;
				
			case kGraphVectorControl:
				errCode = noErr;
				selection =  0;
				
				graphRecordHandle = GetGraphRecordHandle (windowPtr);
				graphRecordPtr = (GraphPtr)GetHandlePointer(
											graphRecordHandle, kNoLock, kNoMoveHi);
				graphViewCPtr = graphRecordPtr->graphViewCPtr;
				
				if (gAppearanceManagerFlag)
					{
					popUpMenuHandle = GetPopUpMenuHandle (theControl);
					if (popUpMenuHandle != NULL)
						{
						SetUpVectorPopUpMenu (graphViewCPtr, popUpMenuHandle);
													
						selection = TrackControl (theControl, location, NIL);		
						
						if (selection > 0)
							errCode = GetBevelButtonMenuValue (theControl, &selection);					
										
								// Now delete the current menu items.
															
						numberMenuItems = CountMenuItems (gPopUpTemporaryMenu);
						for (index=0; index<numberMenuItems; index++)			
							DeleteMenuItem (gPopUpTemporaryMenu, 1);
							
						}		// end "if (popUpMenuHandle != NULL)"
								
					}		// end "if (gAppearanceManagerFlag)"
						
				else		// !gAppearanceManagerFlag
					{
							// Set the hilite to indicate that the button has been pushed in.
								
					HiliteControl (theControl, kControlButtonPart);
					DrawPopupControl (theControl, 10002);
											
					SetUpVectorPopUpMenu (graphViewCPtr, gPopUpTemporaryMenu);
					
					numberMenuItems = CountMenuItems (gPopUpTemporaryMenu);
							
					selection = StandardControlPopUpMenu ( 
																theControl, 
																gPopUpTemporaryMenu, 
																numberMenuItems, 
																kColorMenuID,
																kNoCheckItem);	
																
							// Now delete the current menu items.
														
					for (index=0; index<numberMenuItems; index++)			
						DeleteMenuItem (gPopUpTemporaryMenu, 1);
						
					HiliteControl (theControl, 0);
/*					
					partCode = TrackControl (theControl, location, ControlActionUPP(-1));
					if (partCode != 0)
						{
						SetVectorDisplayList (gActiveWindowInfoH, partCode-1);
								
						InvalidateGraphWindow (windowPtr, kGraphArea);
							
						}		// end "if (paletteSelection > 0)"		
*/	
					}		// end "else !gAppearanceManagerFlag"
														
				if (errCode == noErr && selection > 0)
					{
					SetVectorDisplayList (graphViewCPtr, selection-1);
					
					SetGraphMinMax (gActiveWindowInfoH, kBothXYAxes);
							
					InvalidateGraphWindow (windowPtr, kGraphArea);
						
					}		// end "if (errCode == noErr && selection > 0)"
					
				InvalWindowRect (windowPtr, GetControlBounds (theControl, &gTempRect));
				break;
				
			case kGraphOverlayControl:
				errCode = noErr;
				selection =  0;
				
				graphRecordHandle = GetGraphRecordHandle (windowPtr);
				graphRecordPtr = (GraphPtr)GetHandlePointer(
											graphRecordHandle, kNoLock, kNoMoveHi);
				graphViewCPtr = graphRecordPtr->graphViewCPtr;
												
				if (gAppearanceManagerFlag)
					{
					popUpMenuHandle = GetPopUpMenuHandle (theControl);
					if (popUpMenuHandle != NULL)
						{
						SetUpOverlayPopUpMenu (graphViewCPtr, popUpMenuHandle);
													
						selection = TrackControl (theControl, location, NIL);		
						
						if (selection > 0)
							errCode = GetBevelButtonMenuValue (theControl, &selection);				
										
								// Now delete the current menu items.
															
						numberMenuItems = CountMenuItems (gPopUpTemporaryMenu);
						for (index=0; index<numberMenuItems; index++)			
							DeleteMenuItem (gPopUpTemporaryMenu, 1);
							
						}		// end "if (popUpMenuHandle != NULL)"
								
					}		// end "if (gAppearanceManagerFlag)"
						
				else		// !gAppearanceManagerFlag
					{
							// Set the hilite to indicate that the button has been pushed in.
								
					HiliteControl (theControl, kControlButtonPart);
					DrawPopupControl (theControl, 10001);
											
					SetUpOverlayPopUpMenu (graphViewCPtr, gPopUpTemporaryMenu);
					
					numberMenuItems = CountMenuItems (gPopUpTemporaryMenu);
							
					selection = StandardControlPopUpMenu ( 
																theControl, 
																gPopUpTemporaryMenu, 
																numberMenuItems, 
																kColorMenuID,
																kNoCheckItem);
						
							// Now delete the current menu items.
														
					for (index=0; index<numberMenuItems; index++)			
						DeleteMenuItem (gPopUpTemporaryMenu, 1);	
						
					HiliteControl (theControl, 0);
						
					}		// end "else !gAppearanceManagerFlag"
														
				if (errCode == noErr && selection > 0)
					{
					SetOverlayDisplayList (graphViewCPtr, selection);
							
					InvalidateGraphWindow (windowPtr, kGraphArea);
						
					}		// end "if (errCode == noErr && selection > 0)"
					
				InvalWindowRect (windowPtr, GetControlBounds (theControl, &gTempRect));
				break;
				
			case kGraphBinWidthControl:
				errCode = noErr;
				selection =  0;
				
				graphRecordHandle = GetGraphRecordHandle (windowPtr);
				graphRecordPtr = (GraphPtr)GetHandlePointer(
											graphRecordHandle, kNoLock, kNoMoveHi);
				graphViewCPtr = graphRecordPtr->graphViewCPtr;
												
				if (gAppearanceManagerFlag)
					{
					popUpMenuHandle = GetPopUpMenuHandle (theControl);
					if (popUpMenuHandle != NULL)
						{
						SetUpBinWidthPopUpMenu (graphViewCPtr, popUpMenuHandle);
													
						selection = TrackControl (theControl, location, NIL);		
						
						if (selection > 0)
							errCode = GetBevelButtonMenuValue (theControl, &selection);
						
								// Now delete the current menu items.
															
						numberMenuItems = CountMenuItems (gPopUpTemporaryMenu);
						for (index=0; index<numberMenuItems; index++)			
							DeleteMenuItem (gPopUpTemporaryMenu, 1);	
							
						}		// end "if (popUpMenuHandle != NULL)"
								
					}		// end "if (gAppearanceManagerFlag)"
						
				else		// !gAppearanceManagerFlag
					{
							// Set the hilite to indicate that the button has been pushed in.
								
					HiliteControl (theControl, kControlButtonPart);
					DrawPopupControl (theControl, 10001);
											
					SetUpBinWidthPopUpMenu (graphViewCPtr, gPopUpTemporaryMenu);
					
					numberMenuItems = CountMenuItems (gPopUpTemporaryMenu);
							
					selection = StandardControlPopUpMenu ( 
																theControl, 
																gPopUpTemporaryMenu, 
																numberMenuItems, 
																kColorMenuID,
																kNoCheckItem);
						
							// Now delete the current menu items.
														
					for (index=0; index<numberMenuItems; index++)			
						DeleteMenuItem (gPopUpTemporaryMenu, 1);	
						
					HiliteControl (theControl, 0);
						
					}		// end "else !gAppearanceManagerFlag"
														
				if (errCode == noErr && selection > 0)
					{
					if (SetHistogramBinWidth (graphViewCPtr, selection))
						{
						SetGraphMinMax (graphRecordPtr, kBothXYAxes);
						
						InvalidateGraphWindow (windowPtr, kGraphArea);
						InvalidateGraphWindow (windowPtr, kGraphBinWidth);
						
						}		// end "if (SetHistogramBinWidth (graphViewCPtr, selection))"
						
					}		// end "if (errCode == noErr && selection > 0)"
					
				InvalWindowRect (windowPtr, GetControlBounds (theControl, &gTempRect));
				break;
				
			default:
				break;
	    	
			}		// end "switch (refCon)" 

		}		// end of "if (refCon >= kNextGraphSetControl && ..." 
	  
}		// end "GraphWControlEvent" 		 
#endif	// defined multispec_mac    



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void InvalidateGraphWindow
//
//	Software purpose:	This routine invalidates the input graph window to force
//							its contents to be updated.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 07/08/1996
//	Revised By:			Larry L. Biehl			Date: 03/18/1999		

void InvalidateGraphWindow (
				WindowPtr							windowPtr, 
				SInt16								areaCode)
													
{
#	if defined multispec_mac
		InvalidateWindow (windowPtr, areaCode, FALSE);
#	endif	// defined multispec_mac
	         
#	if defined multispec_win
		((CMGraphView*)windowPtr)->Invalidate();	 
#	endif	// defined multispec_win
	
#	if defined multispec_lin
      //((CMGraphView*)windowPtr)->Close(true);
      //(windowPtr->m_Canvas)->Refresh();
      ((CMGraphView*)windowPtr)->m_frame->Refresh();
#	endif

}		// end "InvalidateGraphWindow" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ListBinWidthValue
//
//	Software purpose:	This routine list the bin width value in the control portion of
//							the graph window.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None	
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/12/2002
//	Revised By:			Larry L. Biehl			Date: 03/03/2017		

void ListBinWidthValue (
				GraphPtr								graphRecordPtr,
				Boolean								drawInWindowFlag)

{	
	Str63									tempString;
	
	SInt32								error;
	
	SInt16								left,
											numChars,
											textWidth,
											top;
											

#	if defined multispec_win
		USES_CONVERSION;
#	endif
											
			// Draw the graph set control prompt text.
	
	numChars = sprintf ((CharPtr)tempString,
									"Bin width: ");
	
	FormatR ((CharPtr)&tempString[numChars], 
					&graphRecordPtr->histogramBinWidth, 
					graphRecordPtr->xESignificantDigits, 
					graphRecordPtr->xFSignificantDigits,
					false, 
					&error);
	numChars = strlen((CharPtr)tempString);
	textWidth = TextWidth((UCharPtr)tempString, 0, numChars);
/*									
	valueFieldSize = FormatHistogramSummaryString (
										(CharPtr)&tempString[numChars],
										graphRecordPtr->histogramBinWidth,
										(SInt32)graphRecordPtr->xESignificantDigits,
										(SInt32)graphRecordPtr->xFSignificantDigits);
			
	numChars += valueFieldSize;
	textWidth = TextWidth (tempString, 
									0, 
									numChars);
*/		          
#	if defined multispec_mac
		ClipRect (&graphRecordPtr->clientRect); 
			
		left = graphRecordPtr->clientRect.left + 95 + 6*kLegendScrollWidth+25;
		top  = graphRecordPtr->clientRect.bottom - 4;  
		
		if (!drawInWindowFlag)
					// This is for a copy or printed page.
			left = graphRecordPtr->clientRect.left + 10;
		
		MoveTo (left, top);
		DrawText (tempString, 0, numChars);
#	endif	// defined multispec_win
			
#	if defined multispec_win
		left = 95 + 6*kLegendScrollWidth + 25;
		top  = graphRecordPtr->clientRect.bottom - 
													graphRecordPtr->clientRect.top - 3;
		if (!drawInWindowFlag)
					// This is for a copy or printed page.
			top -= 15;

		SetBackgroundColor (graphRecordPtr->pDC, 192);
		graphRecordPtr->pDC->SetTextColor( RGB(0,0,0) );
		graphRecordPtr->pDC->TextOut (left, 
													top, 
													(LPCTSTR)A2T((char*)tempString), 
													numChars );
#	endif	// defined multispec_win
   
#	if defined multispec_lin
		left = 95 + 6*kLegendScrollWidth + 25;
		top  = graphRecordPtr->clientRect.bottom - 
													graphRecordPtr->clientRect.top - 3;
		if (!drawInWindowFlag)
					// This is for a copy or printed page.
			top -= 15;

      wxColour color;
      color.Set(RGB(192,0,0));
      graphRecordPtr->pDC->SetTextBackground(color);
      color.Set(RGB(0,0,0));
      graphRecordPtr->pDC->SetTextForeground(color);
#	endif	// defined multispec_lin
	
}		// end "ListBinWidthValue" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void LoadGraphSupportArrays
//
//	Software purpose:	This routine initializes the graph support arrays that 
//							are used to describe which vectors are to be plotted,
//							the colors and symbols to use, the number of points
//							in each vector, and the start offset for the x vector
//							associated for each y vector.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 04/25/1994
//	Revised By:			Larry L. Biehl			Date: 08/09/2013		

void LoadGraphSupportArrays ( 
				GraphPtr								graphRecordPtr,
				SInt32								vectorLength)
													
{
	unsigned char						*symbolsPtr,
											*vectorDisplayPtr,
											*vectorSymbolPtr;
	
	SInt32								*vectorLengthsPtr,
											*vectorPalettePtr,
											*xVectorDataPtr;
										
	SInt16								*featurePtr,
											*classPtr,
											*setListPtr;
	 
	SInt32								index,
											indexEnd,
											numberSets,
											numberVectors;
	                                     
	UInt32								colorIndex,
											vectorLengthInitialValue;
	
	
	if (graphRecordPtr != NULL)
		{
				// Now initialize the graph vectors which indicate the number 	
				// of points in each graph vector, and the start location		
				// for x vector for each y vector.										
				// For the Response versus wavelength graphs, the x vector		
				// is the same for the possible 5 y vectors.							
		
		numberVectors = graphRecordPtr->numberVectors;
		
				// Indicate which vectors are to be drawn.
		
		vectorDisplayPtr = (UInt8*)GetHandlePointer(
													graphRecordPtr->vectorDisplayHandle, 
													kNoLock, 
													kNoMoveHi);
											
		if (gProcessorCode == kBiPlotDataProcessor)
			{
			*vectorDisplayPtr = 1;
			
			for (index=1; index<numberVectors; index++)
				vectorDisplayPtr[index] = 0;
			
			classPtr = gBiPlotDataSpecsPtr->classPtr;
			for (index=0; index<(SInt32)gBiPlotDataSpecsPtr->numberClasses; index++)
				vectorDisplayPtr[ classPtr[index] ] = 1;
				
			}		// end "if (gProcessorCode == kBiPlotDataProcessor)" 
			
		else		// gProcessorCode != kBiPlotDataProcessor 
			{
			for (index=0; index<numberVectors; index++, vectorDisplayPtr++)
				*vectorDisplayPtr = 1;
			
			}		// end "else gProcessorCode != kBiPlotDataProcessor" 
		
				// Load the symbols that are to be used.		
		
		vectorSymbolPtr = (UInt8*)GetHandlePointer(
													graphRecordPtr->vectorSymbolHandle, 
													kNoLock, 
													kNoMoveHi);
		
		if (gProcessorCode == kBiPlotDataProcessor)
			{
			*vectorSymbolPtr = '+';
			vectorSymbolPtr++;
		
			symbolsPtr = (UInt8*)GetHandlePointer(
													gBiPlotDataSpecsPtr->symbolsHandle, 
													kNoLock, 
													kNoMoveHi); 
								
			for (index=1; index<numberVectors; index++, vectorSymbolPtr++)
				*vectorSymbolPtr = symbolsPtr[index];
				
			}		// end "if (gProcessorCode == kBiPlotDataProcessor)" 
				
		else		// gProcessorCode != kBiPlotDataProcessor 
			{
			for (index=0; index<numberVectors; index++, vectorSymbolPtr++)
				*vectorSymbolPtr = '+';
			
			}		// end "else gProcessorCode != kBiPlotDataProcessor" 
			
			
				// Load the x vector offset to be used for each associated		
				// y-vector.  For the Response versus wavelength graphs, the 	
				// x-vector is the same for the possible 5 y vectors.	
		
		xVectorDataPtr = (SInt32*)GetHandlePointer(
													graphRecordPtr->xVectorDataHandle, 
													kNoLock, 
													kNoMoveHi);
		
		indexEnd = graphRecordPtr->numberSets * numberVectors;
												
		for (index=0; index<indexEnd; index++, xVectorDataPtr++)
			*xVectorDataPtr = 0;
			
				// Load the vector label handle if needed. 
			
		if (graphRecordPtr->vectorLabelHandle != NULL)
			{
					// Get a copy of the channel numbers or feature numbers being
					// loaded in for the histogram graphs.
						
			Str31* stringPtr = (Str31*)GetHandlePointer(
													graphRecordPtr->vectorLabelHandle, 
													kLock, 
													kNoMoveHi);
						
				LoadListOfClassFieldNames (stringPtr,
													gProjectInfoPtr->statsWindowMode,
													gProjectInfoPtr->histogramClassFlag);
			
			CheckAndUnlockHandle (graphRecordPtr->vectorLabelHandle);
			
			}		// end "else if (graphRecordPtr->vectorLabelHandle != NULL)"
			
				// Load the length for each vector.											
				// This value will be modified later for the biplot graphs and histogram
				// graphs.			
		
		vectorLengthInitialValue = vectorLength;
		if (gProcessorCode == kBiPlotDataProcessor || 
											gProcessorCode == kHistogramStatsProcessor)	 
			vectorLengthInitialValue = 0;
		
		vectorLengthsPtr = (SInt32*)GetHandlePointer(
													graphRecordPtr->vectorLengthsHandle, 
													kNoLock, 
													kNoMoveHi);
													
		for (index=0; index<indexEnd; index++, vectorLengthsPtr++)
			*vectorLengthsPtr = vectorLengthInitialValue;
			
				// Initialize the colors to use for each vector.
		
		vectorPalettePtr = (SInt32*)GetHandlePointer(
											graphRecordPtr->vectorPaletteColorHandle, 
											kNoLock, 
											kNoMoveHi);
		
		if (graphRecordPtr->flag == NU_SCATTER_PLOT)
			{
			if (graphRecordPtr->processorCode == kBiPlotDataProcessor)
				{
				vectorPalettePtr[0] = blackColor;
		
				for (index=1; index<numberVectors; index++)
					{
					colorIndex = (index % 6);
				
					if (colorIndex == 1)
						vectorPalettePtr[index] = redColor;
						
					else if (colorIndex == 2)
						vectorPalettePtr[index] = greenColor;
						
					else if (colorIndex == 3)
						vectorPalettePtr[index] = blueColor;
						
					else if (colorIndex == 4)
						vectorPalettePtr[index] = cyanColor;
						
					else if (colorIndex == 5)
						vectorPalettePtr[index] = magentaColor;
						
					else if (colorIndex == 0)
						vectorPalettePtr[index] = yellowColor;

					}		// end "for (index=1; index<numberVectors; index++)" 
					
				}		// end "if (...->processorCode == kBiPlotDataProcessor)"
				
			else		// graphRecordPtr->processorCode != kBiPlotDataProcessor
				{
						// processorCode == kListDataProcessor || kSelectionGraphProcessor
						
				vectorPalettePtr[0] = redColor;
				
				if (numberVectors > 1)
					vectorPalettePtr[1] = greenColor;
					
				if (numberVectors > 2)
					vectorPalettePtr[2] = greenColor;
					
				if (numberVectors > 3)
					vectorPalettePtr[3] = blueColor;
					
				if (numberVectors > 4)
					vectorPalettePtr[4] = blueColor;
						
				}		// end "else graphRecordPtr->processorCode != kBiPlotDataProcessor"
			
			}		// end "if (graphRecordPtr->flag == NU_SCATTER_PLOT)" 
		
		else if (graphRecordPtr->flag == NU_HISTOGRAM_PLOT)
			{
			for (index=0; index<numberVectors; index++)
				{
				colorIndex = (index % 6);
			
				if (colorIndex == 0)
					vectorPalettePtr[index] = redColor;
					
				else if (colorIndex == 1)
					vectorPalettePtr[index] = greenColor;
					
				else if (colorIndex == 2)
					vectorPalettePtr[index] = blueColor;
					
				else if (colorIndex == 3)
					vectorPalettePtr[index] = cyanColor;
					
				else if (colorIndex == 4)
					vectorPalettePtr[index] = magentaColor;
					
				else if (colorIndex == 5)
					vectorPalettePtr[index] = yellowColor;

				}		// end "for (index=0; index<numberVectors; index++)" 
			
			}		// end "else if (graphRecordPtr->flag == NU_HISTOGRAM_PLOT)" 
			
		else		// else Must be a response vs wavelength type of line graph
			{
			vectorPalettePtr[0] = redColor;
			
			if (numberVectors > 1)
				vectorPalettePtr[1] = greenColor;
				
			if (numberVectors > 2)
				vectorPalettePtr[2] = greenColor;
				
			if (numberVectors > 3)
				vectorPalettePtr[3] = blueColor;
				
			if (numberVectors > 4)
				vectorPalettePtr[4] = blueColor;
				
			}		// end "else graphRecordPtr->flag != NU_HISTOGRAM_PLOT || NU_SCATTER_PLOT"
			
				// Load the set list vector. 
			
		if (gProcessorCode == kHistogramStatsProcessor)
			{
					// Get a copy of the channel numbers or feature numbers being
					// loaded in for the histogram graphs.
						
			setListPtr = (SInt16*)GetHandlePointer(
													graphRecordPtr->setListHandle, 
													kNoLock, 
													kNoMoveHi);
						
			featurePtr = (SInt16*)GetHandlePointer(
													gStatHistogramSpecsPtr->featureHandle, 
													kNoLock, 
													kNoMoveHi);
													
			numberSets = graphRecordPtr->numberSets;
			
			if (!gStatHistogramSpecsPtr->featureTransformationFlag)			
				for (index=0; index<numberSets; index++)
					setListPtr[index] = 
										gProjectInfoPtr->channelsPtr[ featurePtr[index] ] + 1;
										
			else		// gStatHistogramSpecsPtr->featureTransformationFlag		
				for (index=0; index<numberSets; index++)
					setListPtr[index] = featurePtr[index] + 1;

					// Load the class mean and standard deviation data.
					 
			if ( !LoadClassMeanAndStdDevData (graphRecordPtr, 
														gProjectInfoPtr->statsWindowMode) )
				graphRecordPtr->graphCodesAvailable = 1;
			
			}		// end "else if (gProcessorCode == kHistogramStatsProcessor)"
		
		}		// end "if (graphRecordPtr != NULL)" 

}		// End LoadGraphSupportArrays 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void LoadGraphXVector
//
//	Software purpose:	This routine loads the data into the x vector for a
//							graph window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			CreateGraphWindow in graphicsUtilities.c
//							ShowGraphWindowSelection in selectionGraph.c
//
//	Coded By:			Larry L. Biehl			Date: 12/09/1991
//	Revised By:			Larry L. Biehl			Date: 01/03/2006			

Boolean LoadGraphXVector ( 
				GraphPtr								graphRecordPtr,
				SInt16*								channelListPtr, 
				SInt32								vectorLength)
													
{
	float*								channelValuePtr;
	
	double								value;
	
	Handle								channelValuesHandle;
	
	SInt32								error = noErr, 
											index2;
	
	SInt16								channel,
											index;
	
	Boolean								continueFlag;
	

	continueFlag = FALSE;

	if (graphRecordPtr != NULL)
		{
				// Initialize values for x vector.											
				// The channel numbers will be loaded first.  The channel values	
				// will be loaded second if they exist.									
		
		channelValuePtr = NULL;
		channelValuesHandle = NULL;
		
#		if defined multispec_mac
			if (graphRecordPtr->imageWindow != NULL)
				{
				Handle windowInfoHandle = (Handle)GetWRefCon (graphRecordPtr->imageWindow);
				Handle fileInfoHandle = ((WindowInfoPtr)*windowInfoHandle)->fileInfoHandle;
				if ( fileInfoHandle )
					channelValuesHandle = 
										((FileInfoPtr)*fileInfoHandle)->channelValuesHandle;
				
				}		// end "if (graphRecordPtr->imageWindow != NULL)"
#		endif	// defined multispec_mac
			
		if (channelValuesHandle != NULL)
			{
			channelValuePtr = (float*)GetHandlePointer(
											channelValuesHandle, 
											kLock, 
											kNoMoveHi);
			
			}		// end "if (channelValuesHandle)" 
			
		graphRecordPtr->xVector.numberPoints = vectorLength;
		LockAndGetVectorPointer (&graphRecordPtr->xVector, &error);
		
		channel = 0;	
		for (index=0; index<vectorLength; index++)
			{		
			if (error != noErr)
				break;
				
			if (channelListPtr != NULL)
				channel = channelListPtr[index];
			
			channel++;
				
			if (index == 0)
				graphRecordPtr->xDataMin = channel;
				
			SetV (&graphRecordPtr->xVector, index, channel, &error);
				
			}		// end "for ( index=0; index<vectorLength; index++)" 
			
		continueFlag = (error == noErr);
			
		graphRecordPtr->xDataMax = channel;
														
		graphRecordPtr->xInt = (SInt32)
				(graphRecordPtr->xDataMax - graphRecordPtr->xDataMin  + 5)/5;
				
				// Load the channel values.													
		
		if (channelValuePtr != NULL && continueFlag)
			{
			channel = 0;	
			index2 = vectorLength;
			for ( index=0; index<vectorLength; index++)
				{		
				if (channelListPtr != NULL)
					channel = channelListPtr[index];
			
				value = channelValuePtr[channel];
				
				SetV (&graphRecordPtr->xVector, index2, value, &error);	
				index2++;
			
				if (error != noErr)
					break;
					
				channel++;
				
				}		// end "for ( index=0; index<vectorLength; index++)" 
			
			continueFlag = (error == noErr);
				
			}		// end "if (channelValuePtr && continueFlag)" 
		
		CheckAndUnlockHandle (channelValuesHandle);
		UnlockVectorPointer ( &graphRecordPtr->xVector);
		
		}		// end "if (graphRecordPtr)" 
		
	return (continueFlag);

}		// End LoadGraphXVector 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void LoadGraphYVector
//
//	Software purpose:	This routine loads the data into the y vector for a
//							graph window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			ListFieldData in listData.c
//							ShowGraphWindowSelection in selectionGraph.c
//
//	Coded By:			Larry L. Biehl			Date: 10/04/1991
//	Revised By:			Larry L. Biehl			Date: 12/29/2005			

void LoadGraphYVector ( 
				GraphPtr								graphRecordPtr, 
				double*								buffer8BytePtr,
				UInt32								vectorLength, 
				UInt16								inputIndex)
													
{ 
	double*								bufferReal8BytePtr;
	
	SInt32								error;
	
	UInt32								index;
	
	
	graphRecordPtr->yVector.numberPoints = vectorLength;
	graphRecordPtr->numberVectors = 1;
	
	LockAndGetVectorPointer (&graphRecordPtr->yVector, &error);
	if (error != noErr)
																								return;
	
	bufferReal8BytePtr = (double*)buffer8BytePtr;																						
	for (index=0; index<vectorLength; index++)
		{
		SetV (&graphRecordPtr->yVector, index, *bufferReal8BytePtr, &error);
		bufferReal8BytePtr += inputIndex;
			
		}		// end "for (index=0; index<localNumberChannels..."
			
	UnlockVectorPointer (&graphRecordPtr->yVector);

}		// end "LoadGraphYVector" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void LoadGraphYVector2
//
//	Software purpose:	This routine loads the data into the y vector for a
//							graph window from a channels structure vector.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			ShowGraphWindowSelection in selectionGraph.c
//
//	Coded By:			Larry L. Biehl			Date: 07/27/1992
//	Revised By:			Larry L. Biehl			Date: 03/11/1997			

void LoadGraphYVector2 ( 
				GraphPtr								graphRecordPtr, 
				ChannelStatisticsPtr				channelStatsPtr, 
				UInt32								vectorLength)
													
{
	ChannelStatisticsPtr				lChannelStatsPtr;
	
	SInt32								error;
	
	UInt32								index,
											index2;
	
	
	graphRecordPtr->numberVectors = 5;
	graphRecordPtr->yVector.numberPoints = 
										vectorLength * graphRecordPtr->numberVectors;
	index2 = 0;
	
	LockAndGetVectorPointer ( &graphRecordPtr->yVector, &error);
	if (error != noErr)
																								return;
	
			// Load the channel means.														
			
	lChannelStatsPtr = channelStatsPtr;
	for (index=0; index<vectorLength; index++)
		{
		SetV (&graphRecordPtr->yVector, 
				index2, 
				lChannelStatsPtr->mean, 
				&error);
		index2++;
		lChannelStatsPtr++;
			
		}		// end "for (index=0; index<vectorLength..." 
	
			// Load the channel means + 1 standard deviation.						
			
	lChannelStatsPtr = channelStatsPtr;
	for ( index=0; index<vectorLength; index++)
		{
		SetV (&graphRecordPtr->yVector, 
				index2, 
				lChannelStatsPtr->mean + lChannelStatsPtr->standardDev, 
				&error);
		index2++;
		lChannelStatsPtr++;
			
		}		// end "for (index=0; index<vectorLength..." 
	
			// Load the channel means - 1 standard deviation.						
			
	lChannelStatsPtr = channelStatsPtr;
	for (index=0; index<vectorLength; index++)
		{
		SetV (&graphRecordPtr->yVector, 
				index2, 
				lChannelStatsPtr->mean - lChannelStatsPtr->standardDev,  
				&error);
		index2++;
		lChannelStatsPtr++;
			
		}		// end "for (index=0; index<vectorLength..." 
	
			// Load the channel minimum values.											
			
	lChannelStatsPtr = channelStatsPtr;
	for (index=0; index<vectorLength; index++)
		{
		SetV (&graphRecordPtr->yVector, 
				index2, 
				lChannelStatsPtr->minimum, 
				&error);
		index2++;
		lChannelStatsPtr++;
			
		}		// end "for (index=0; index<vectorLength..." 
	
			// Load the channel maximum values.											
			
	lChannelStatsPtr = channelStatsPtr;
	for (index=0; index<vectorLength; index++)
		{
		SetV (&graphRecordPtr->yVector, 
				index2, 
				lChannelStatsPtr->maximum,  
				&error);
		index2++;
		lChannelStatsPtr++;
			
		}		// end "for (index=0; index<vectorLength..." 
		
	UnlockVectorPointer (&graphRecordPtr->yVector);

}		// end "LoadGraphYVector2" 


                           
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void MSetGraphWindowTitle
//
//	Software purpose:	This this routine sets the title of the graph window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 10/07/1991
//	Revised By:			Larry L. Biehl			Date: 08/06/2013			

void MSetGraphWindowTitle ( 
				CMGraphView*						graphViewCPtr,
				StringPtr							titleStringPtr)
													
{
#	if defined multispec_mac
		MSetWindowTitle (graphViewCPtr->GetWindowPtr(), 
							&titleStringPtr[0]);
#	endif	// defined multispec_mac
			  
#	if defined multispec_win
		MSetWindowTitle ((CDocument*)graphViewCPtr->GetDocument(), 
							&titleStringPtr[0]);
#	endif	// defined multispec_win
	
#	if defined multispec_lin
     // MSetWindowTitle ((CMImageView*)graphViewCPtr->GetDocument(), 
		//					&titleStringPtr[0]);
      wxString ntitle =  wxString::FromUTF8((char*)&titleStringPtr[1]);
      (graphViewCPtr->m_frame)->SetTitle(ntitle);
			//(windowPtr->m_frame)->SetTitle(ntitle);
      //MSetWindowTitle ((CMImageView*)graphViewCPtr, 
		//					&titleStringPtr[0]);
#	endif

}		// end "MSetGraphWindowTitle"


                           
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetGraphWindowTitle
//
//	Software purpose:	This routine sets the title for the graphics
//							window based on the number of open graphics
//							windows and the associated image file name.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			CreateGraphWindow
//
//	Coded By:			Larry L. Biehl			Date: 10/07/1991
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

void SetGraphWindowTitle ( 
				CMGraphView*						graphViewCPtr,
				Boolean								newWindowFlag)
													
{
	UCharPtr								charPtr;  
	GraphPtr								graphRecordPtr;
	
	Handle								graphRecordHandle;
	

//#	if defined multispec_win
//		USES_CONVERSION;
//#	endif

	if (graphViewCPtr == NULL)
																								return;
																								
	if (gProcessorCode == kSelectionGraphProcessor)
		{
		if (newWindowFlag)
			{
			gTextString[0] = 0;
			gTextString[1] = 0;
			
			ConcatPStrings ((UCharPtr)&gTextString, (StringPtr)"\0Selection Graph\0", 254);
			
			}		// end "if (newWindowFlag)" 	
					
		else		// !newWindowFlag 
			{
#			ifndef multispec_lin
				GetWindowTitle (graphViewCPtr->GetWindowPtr(), (UCharPtr)&gTextString2);
#			endif
			
#			ifdef multispec_lin
				GetGraphWindowTitle (graphViewCPtr->GetWindowPtr(), (UCharPtr)&gTextString2);
#			endif

			charPtr = (UCharPtr)strchr ((char*)&gTextString2[1], ':');			
			
			graphRecordHandle = graphViewCPtr->GetGraphRecordHandle();		
			graphRecordPtr = (GraphPtr)GetHandlePointer(
													graphRecordHandle,
													kNoLock, 
													kNoMoveHi);
			
			if (charPtr == NULL || graphRecordPtr == NULL || (graphRecordPtr->drawGraphCode & 0x8000))
				sprintf ((char*)&gTextString, 
							"Graph Window %hd",
							graphViewCPtr->GetGraphWindowNumber());
						
			else		// charPtr != NULL && !(...->drawGraphCode & 0x8000)
				{
				sprintf ((char*)&gTextString, 
							"Graph Window: %s",
							(char*)&charPtr[2]);
							
				}		// end "else charPtr != NULL"
							
			charPtr = CtoPstring (gTextString, gTextString);
			
			}		// end "else !newWindowFlag" 
		
		}		// end "if (gProcessorCode == kSelectionGraphProcessor)" 
		
	else		// gProcessorCode != kSelectionGraphProcessor 
		{
		Handle								fileInfoHandle,
												imageWindowInfoHandle;
		
		FileStringPtr						fileNamePtr;
		 
		SInt16								numberCharacters,
												stringLength;
												
			
		numberCharacters = sprintf ((char*)&gTextString[1], 
												"Graph %hd (", 
												gNumberOfGWindows);
						
		charPtr = &gTextString[numberCharacters+1];
		stringLength = numberCharacters;
				
		gTextString2[0] = 0;
		fileNamePtr = &gTextString2[0];
									
		if (gProcessorCode == kHistogramStatsProcessor)
			{
			//fileNamePtr = &gProjectInfoPtr->imageFileName[1];
			gTextString3[0] = 0;
			strcpy ((char*)gTextString3, (char*)&gProjectInfoPtr->imageFileName[1]);
			fileNamePtr = (FileStringPtr)gTextString3;
			
			} // end "if (gProcessorCode == kHistogramStatsProcessor)"
			
		else if (gActiveImageWindow != NULL)
			{                                                                                                  
			imageWindowInfoHandle = GetActiveImageWindowInfoHandle (); 
			fileInfoHandle = GetFileInfoHandle (imageWindowInfoHandle);    
				
			if (fileInfoHandle != NULL)
				{                  
				WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer(
													imageWindowInfoHandle, kNoLock, kNoMoveHi);
				SInt16 numberImageFiles = windowInfoPtr->numberImageFiles;
													                                                          
				if (numberImageFiles > 1)
					{		
					numberCharacters = sprintf ((char*)charPtr, "L%hd-)", numberImageFiles);
					charPtr = &charPtr[numberCharacters]; 
					stringLength += numberCharacters;
					
					}		// end "if (numberImageFiles > 1)" 
			
				
				FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer(
														fileInfoHandle, kNoLock, kNoMoveHi);
				fileNamePtr = (FileStringPtr)GetFileNameCPointer (fileInfoPtr);
					
				}		// end "if (fileInfoHandle != NULL)" 
				
			}		// end "else if (gActiveImageWindow != NULL)" 
				
		else		// gProcessorCode != kHistogramStatsProcessor && gActiveImageWindow == NULL 
			{
			if (gProjectInfoPtr != NULL)
				{
				gTextString3[0] = 0;
				strcpy ((char*)gTextString3, (char*)&gProjectInfoPtr->imageFileName[1]);
				fileNamePtr = gTextString3;
				
				}		// end "if (gProjectInfoPtr != NULL)"
				
			}		// end "else gProcessorCode != kHistogramStatsProcessor && ..." 
				
		numberCharacters = sprintf ((char*)charPtr, "%s)", fileNamePtr);
		stringLength += numberCharacters;
			
		gTextString[0] = (UInt8)stringLength;
			
		}		// end "else gProcessorCode != kSelectionGraphProcessor" 
				
			// Make certain that there is at least 500 bytes left for setting		
			//	a new title.																			

	if (CheckIfMemoryAvailable (500))
		{
		MSetGraphWindowTitle (graphViewCPtr, gTextString);
/*		
		#if defined multispec_mac
			MSetWindowTitle (graphViewCPtr->GetWindowPtr(), 
								&gTextString[0]);
		#endif	// defined multispec_mac    
              
		#if defined multispec_win
			MSetWindowTitle ( (CDocument*)graphViewCPtr->GetDocument(), 
								&gTextString[0]);
		#endif	// defined multispec_win
*/		
		}		// end "if (CheckIfMemoryAvailable (500))"

}		// end "SetGraphWindowTitle"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetGraphMinMax
//
//	Software purpose:	The purpose of this routine is to determine and set the min and 
//							max values for the requested set in the input graph.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/25/1999
//	Revised By:			Larry L. Biehl			Date: 02/10/2006			

void SetGraphMinMax (
				GraphPtr								graphRecordPtr,
				SInt16								axisCode)

{
//	char									number[64];
	
	double								fortyBinWidths,
											min,
											max;
											
	double*								histogramBinWidthPtr;
											
	char*									vectorDisplayPtr;
											
	HSInt32Ptr							vectorLengthsPtr,
											vectorDataPtr;
	                               
	SInt32								error;
	
	
	if (graphRecordPtr != NULL)
		{
				// Get the graph lengths vector.
				
		vectorLengthsPtr = (SInt32*)GetHandlePointer(
													graphRecordPtr->vectorLengthsHandle,
													kLock,
													kNoMoveHi );
		
				// Get the graph offset vector. The offsets will be the same for the x
				// and y data for histogram type plots.
		
		vectorDataPtr = (SInt32*)GetHandlePointer(
													graphRecordPtr->xVectorDataHandle, 
													kLock, 
													kNoMoveHi);
	
				// Get the vector display vector. 
				
		vectorDisplayPtr = (char*)GetHandlePointer (
													graphRecordPtr->vectorDisplayHandle,
													kLock,
													kNoMoveHi);
	
				// Get the histogram bin width vector in case it is needed. 
				
		histogramBinWidthPtr = (double*)GetHandlePointer (
													graphRecordPtr->histogramBinWidthHandle,
													kLock,
													kNoMoveHi);
		
		if (axisCode & kXAxis)
			{
					// Get the min and max values for the x axis.
					
			min = DBL_MAX;
			max = -DBL_MAX; 
			
			if (graphRecordPtr->graphCodesAvailable & 0x0003)
				{
				if (graphRecordPtr->graphCodesAvailable & 0x0001)
					{
					if (graphRecordPtr->flag & NU_HISTOGRAM_PLOT)
						{
						FindMaxMinV (&graphRecordPtr->xDataMax,
											&graphRecordPtr->xDataMin,
											&graphRecordPtr->xVector,
											vectorLengthsPtr,
											vectorDataPtr,
											vectorDisplayPtr,
											graphRecordPtr->numberVectors, 
											graphRecordPtr->set);
						
						min = graphRecordPtr->xDataMin;
						max = graphRecordPtr->xDataMax;
						
								// force the difference of max and min to be at least
								// 40 (original) bins for a reasonable plot.
						
						fortyBinWidths = 32*histogramBinWidthPtr[graphRecordPtr->set-1];
						if (max-min < fortyBinWidths)
							{
							min = (max + min)/2 - fortyBinWidths/2;
							
//							if (min > 0)
//								min = MAX (0, min);
								
							max = min + fortyBinWidths;
							
							}		// end "if (max-min < fortyBinWidths)"
/*							
								// Allow for width of data value bin.
						
						if (graphRecordPtr->histogramBinCode <= 0)
							{
							screen_width = ( clientRectPtr->right - clientRectPtr->left ) -
													graph->rightInset - graph->leftInset;
													
							x_scale = screen_width / (graph->xScaleMax - graph->xScaleMin);
					//		binWidth = (UInt32)(2/x_scale + 1);
							binWidth = floor (histogramBinWidthPtr[graphRecordPtr->set-1] * 
																							2/x_scale + 1);
							if (binWidth <= 0)
								binWidth = 1;
							
							}		// end "if (graphRecordPtr->histogramBinCode <= 0)"
							
						else		// graphRecordPtr->histogramBinCode > 0
							binWidth = graph->histogramBinCode * histogramBinWidthPtr[set];
*/							
						
                  graphRecordPtr->histogramBinWidth = GetUserHistogramBinWidth (
																				graphRecordPtr,
																				histogramBinWidthPtr,
																				min,
																				max);
                  
																				
						graphRecordPtr->origHistogramBinWidth = 
														histogramBinWidthPtr[graphRecordPtr->set-1];
							
						}		// end "if (graphRecordPtr->flag & NU_HISTOGRAM_PLOT)"
						
					else if (graphRecordPtr->flag & NU_SCATTER_PLOT)	
						{
						FindMaxMinV( &graphRecordPtr->xDataMax, 
											&graphRecordPtr->xDataMin, 
											&graphRecordPtr->xVector, 
											&error );
					
						min = graphRecordPtr->xDataMin;
						max = graphRecordPtr->xDataMax;
							
						}		// end "else else if (graphRecordPtr->flag & NU_SCATTER_PLOT)"		
				
					else		// ...->flag != NU_HISTOGRAM_PLOT && != NU_SCATTER_PLOT
						{
						min = graphRecordPtr->xDataMin;
						max = graphRecordPtr->xDataMax;
						
						}		// end "else ...->flag != NU_HISTOGRAM_PLOT && ..."
					
					}		// end "if (graphRecordPtr->graphCodesAvailable & 0x0001)" 
																
				if (graphRecordPtr->graphCodesAvailable & 0x0002)
					{
					min = MIN(min, graphRecordPtr->xEllipseMin);
					max = MAX(max, graphRecordPtr->xEllipseMax);
					
					}		// end "if (graphRecordPtr->graphCodesAvailable & 0x0002)" 
				
				graphRecordPtr->xMin = min;
				graphRecordPtr->xMax = max;
				
						// Now set the scales for the x axis. This will be needed for
						// determining the scales for the y axis for histogram type
						// plots.
						
				SetXGraphScale (graphRecordPtr);
				
				}		// end "if (graphRecordPtr->graphCodesAvailable & 0x0003)"
				
			}		// end "if (axisCode & kXAxis)"
			
		if (axisCode & kYAxis)
			{
					// Now get the min and max values for the y-axis.
					
			min = DBL_MAX;
			max = -DBL_MAX; 
															
			if ( !(graphRecordPtr->graphCodesAvailable & 0x0003) )
				{
				min = 0;
				max = 0;
				
				}		// end "if ( !(graphRecordPtr->graphCodesAvailable & 0x0003) )"
				
			else		// (graphRecordPtr->graphCodesAvailable & 0x0003)
				{
				if (graphRecordPtr->graphCodesAvailable & 0x0001)
					{
					if (graphRecordPtr->flag & NU_HISTOGRAM_PLOT)
						{
						graphRecordPtr->yDataMin = 0;
						
						FindMaxBinV( &graphRecordPtr->yDataMax,
											&graphRecordPtr->xVector,
											&graphRecordPtr->yVector,
											vectorLengthsPtr,
											vectorDataPtr,
											vectorDisplayPtr,
											histogramBinWidthPtr,
											graphRecordPtr->numberVectors, 
											graphRecordPtr->set,
											graphRecordPtr);
						
						min = graphRecordPtr->yDataMin;
						max = graphRecordPtr->yDataMax;
						
						}		// end "if (graphRecordPtr->flag & NU_HISTOGRAM_PLOT)"
						
					else		// !(graphRecordPtr->flag & NU_HISTOGRAM_PLOT)
						{
						FindMaxMinV (&graphRecordPtr->yDataMax, 
											&graphRecordPtr->yDataMin, 
											&graphRecordPtr->yVector, 
											&error);
						
						min = graphRecordPtr->yDataMin;
						max = graphRecordPtr->yDataMax;

						if (graphRecordPtr->processorCode == kSelectionGraphProcessor)
							GetLikeWindowMinMax (graphRecordPtr->imageWindow, &min, &max);
							
						}		// end "else !(graphRecordPtr->flag & NU_HISTOGRAM_PLOT)"
					
					}		// end "if (graphRecordPtr->graphCodesAvailable & 0x0001)"
																
				if (graphRecordPtr->graphCodesAvailable & 0x0002)
					{
					min = MIN(min, graphRecordPtr->yEllipseMin);
					max = MAX(max, graphRecordPtr->yEllipseMax);
					
					}		// end "if (graphRecordPtr->graphCodesAvailable & 0x0002)"
					
				}		// end "else (graphRecordPtr->graphCodesAvailable & 0x0003)"  
				
			graphRecordPtr->yMin = min;
			graphRecordPtr->yMax = max;
		
					// Set the min and max scales and tic intervals for the y-axis.			

			SetYGraphScale (graphRecordPtr);
			
			}		// end "if (axisCode & kYAxis)"
			
		CheckAndUnlockHandle (graphRecordPtr->histogramBinWidthHandle);
		CheckAndUnlockHandle (graphRecordPtr->vectorLengthsHandle);
		CheckAndUnlockHandle (graphRecordPtr->xVectorDataHandle);
		CheckAndUnlockHandle (graphRecordPtr->vectorDisplayHandle);
		
		}		// end "if (graphRecordPtr != NULL)"
	
}		// end "SetGraphMinMax" 

		

void SetGraphMinMax (
				Handle								windowInfoHandle,
				SInt16								axisCode)
				
{
	GraphPtr								graphRecordPtr;
	
	Handle								graphRecordHandle;
	

	graphRecordHandle = GetGraphRecordHandle (windowInfoHandle);
                   
	graphRecordPtr = (GraphPtr)GetHandlePointer(
														graphRecordHandle, kLock, kNoMoveHi);
										
	SetGraphMinMax (graphRecordPtr, axisCode);
	
	CheckAndUnlockHandle (graphRecordHandle);
	
}		// end "SetGraphMinMax" 		


/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetGraphScales
//
//	Software purpose:	The purpose of this routine is to set the x and y scales and tic
//							intervals relative to the size of the graph.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/26/2003
//	Revised By:			Larry L. Biehl			Date: 04/30/2003			

void SetGraphScales (
				GraphPtr								graphRecordPtr)

{
	SetXGraphScale (graphRecordPtr);
	
	SetYGraphScale (graphRecordPtr);
	
}		// end "SetGraphScales" 			
*/


//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetXGraphScale
//
//	Software purpose:	The purpose of this routine is to set the x scales and tic
//							intervals relative to the size of the graph.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/26/2003
//	Revised By:			Larry L. Biehl			Date: 02/14/2006			

void SetXGraphScale (
				GraphPtr								graphRecordPtr)

{
	char									number[64];
	
	double								interval;
	
	Rect*									clientRectPtr;
	
	SInt32								error,
											numberDecimalDigits,
											numberIntervals,
											screen_width,
											ticLabelWidth;
	
	
			// Get the size of the graph window.
	
#	if defined multispec_mac
		GetWindowPortBounds (graphRecordPtr->window, &graphRecordPtr->clientRect);
#	endif	// defined multispec_mac
           
#	if defined multispec_win
		CMGraphView* graphViewCPtr = graphRecordPtr->graphViewCPtr;
		graphViewCPtr->GetClientRect ((RECT*)&graphRecordPtr->clientRect);
#	endif	// defined multispec_win
	
#	if defined multispec_lin
      CMGraphView* graphViewCPtr = graphRecordPtr->graphViewCPtr;
		graphViewCPtr->GetGraphWindowClientRect(&graphRecordPtr->clientRect);

      //graphViewCPtr->GetClientRect ((RECT*)&graphRecordPtr->clientRect);
        //graphRecordPtr->clientRect.bottom = ((wxRect*)graphViewCPtr)->GetWidth();
        //printf("clientRect:%d\n", graphRecordPtr->clientRect.bottom);
      //graphRecordPtr->clientRect.top = clientRectPtr->top;
      //graphRecordPtr->clientRect.bottom = clientRectPtr->bottom;
      //graphRecordPtr->clientRect.left = clientRectPtr->left;
      //graphRecordPtr->clientRect.right = clientRectPtr->right;
#	endif
			// Get length of max x tic label string.
	
	graphRecordPtr->xESignificantDigits = 0;
	graphRecordPtr->xFSignificantDigits = 0;		
	if (graphRecordPtr->flag & NU_HISTOGRAM_PLOT)
		{
		GetNumberDecimalDigits (graphRecordPtr->xValueTypeCode,
											graphRecordPtr->xMin,
											graphRecordPtr->xMax,
											graphRecordPtr->origHistogramBinWidth,
											&graphRecordPtr->xESignificantDigits,
											&graphRecordPtr->xFSignificantDigits);
											
		}		// end "if (graphRecordPtr->flag & NU_HISTOGRAM_PLOT)"
	
	graphRecordPtr->xScaleMax = graphRecordPtr->xMax;
   	
	FormatR (number, 
					&graphRecordPtr->xScaleMax, 
					graphRecordPtr->xESignificantDigits, 
					graphRecordPtr->xFSignificantDigits, 
					false, 
					&error);
	ticLabelWidth = TextWidth ((UCharPtr)number, 0, strlen(number));
	
	graphRecordPtr->xScaleMin = graphRecordPtr->xMin;
   	
	FormatR (number, 
					&graphRecordPtr->xScaleMin, 
					graphRecordPtr->xESignificantDigits, 
					graphRecordPtr->xFSignificantDigits, 
					false, 
					&error);
	graphRecordPtr->xTicLabelWidth = TextWidth ((UCharPtr)number, 0, strlen(number));
	
	graphRecordPtr->xTicLabelWidth = (SInt16)
										MAX (graphRecordPtr->xTicLabelWidth, ticLabelWidth);
	
			// Now get the x tick interval.
	
	clientRectPtr = &graphRecordPtr->clientRect;
	screen_width = (clientRectPtr->right - clientRectPtr->left ) -
										graphRecordPtr->rightInset - graphRecordPtr->leftInset;
   
	numberIntervals = screen_width / (2*graphRecordPtr->xTicLabelWidth);

	numberIntervals = MAX (1, numberIntervals);
					
			// Now get the interval distance in "whole" graph units.
									
//	interval = 
//		(graphRecordPtr->xMax - graphRecordPtr->xMin + numberIntervals)/numberIntervals;									
	interval = (graphRecordPtr->xMax - graphRecordPtr->xMin);
	if (interval == 0)
		interval = 1;
	if (graphRecordPtr->flag & NU_HISTOGRAM_PLOT)
		interval += graphRecordPtr->origHistogramBinWidth;
		
	interval /= numberIntervals;
	
 	if (graphRecordPtr->flag & NU_HISTOGRAM_PLOT)
 		{
//		interval = MAX (graphRecordPtr->origHistogramBinWidth, interval);
		interval = ceil (interval/graphRecordPtr->origHistogramBinWidth);
		interval *= graphRecordPtr->origHistogramBinWidth;
		
		}		// end "if (graphRecordPtr->flag & NU_HISTOGRAM_PLOT)"
		
	else		// !(graphRecordPtr->flag & NU_HISTOGRAM_PLOT)
		{	
		if (graphRecordPtr->xValueTypeCode == kIntegerType)
			interval = ceil(interval);
		
		else		// graphRecordPtr->xValueTypeCode != kIntegerType
			{
			interval = ConvertToScientificFormat (interval, &numberDecimalDigits);
			interval = ceil(interval);
			interval = interval * pow ((double)10, (double)numberDecimalDigits);
		
			}		// end "else graphRecordPtr->xValueTypeCode != kIntegerType"
			
		}		// end "else !(graphRecordPtr->flag & NU_HISTOGRAM_PLOT)"
	
	if ( !(graphRecordPtr->attrb & NU_XLOG) )
		{
		graphRecordPtr->xScaleMin = floor(graphRecordPtr->xMin/interval)*interval;
		
		graphRecordPtr->xScaleMax = ceil(graphRecordPtr->xMax/interval)*interval;
		
		if ( (graphRecordPtr->flag & NU_HISTOGRAM_PLOT) ||
							graphRecordPtr->xScaleMin == graphRecordPtr->xScaleMax)
			{
			if (graphRecordPtr->xScaleMin == graphRecordPtr->xMin)
				graphRecordPtr->xScaleMin -= interval;
				
			if (graphRecordPtr->xScaleMax == graphRecordPtr->xMax)
				graphRecordPtr->xScaleMax += interval;
				
			}		// end "if (graphRecordPtr->xScaleMin == graphRecordPtr->xScaleMax)" 
		
		}		// end "if ( !( graphRecordPtr->attrb & NU_XLOG ) )"
		
			// Redo the determing the decimal digits in case the calculated scale
			// min/max causes them to change.  There should be enough space to allow
			// for another decimal in the tic labels if needed.
			
	if (graphRecordPtr->flag & NU_HISTOGRAM_PLOT)
		{
		GetNumberDecimalDigits (graphRecordPtr->xValueTypeCode,
											graphRecordPtr->xScaleMin,
											graphRecordPtr->xScaleMax,
											graphRecordPtr->origHistogramBinWidth,
											&graphRecordPtr->xESignificantDigits,
											&graphRecordPtr->xFSignificantDigits);
											
		}		// end "if (graphRecordPtr->flag & NU_HISTOGRAM_PLOT)"
		
	graphRecordPtr->xInt = interval;
	
}		// end "SetXGraphScale"		



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetYGraphScale
//
//	Software purpose:	The purpose of this routine is to set the y scales and tic
//							intervals relative to the size of the graph.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/26/2003
//	Revised By:			Larry L. Biehl			Date: 02/21/2006			

void SetYGraphScale (
				GraphPtr								graphRecordPtr)

{
	char									number[64];
	
	double								interval,
											realAdditional;
	
	SInt32								numberDecimalPlaces,
											numberDigits,
											numberIntervalDigits;
	
	UInt32								additional,
											integerInterval;
	
	
			// Get the size of the graph window.
	
#	if defined multispec_mac
		GetWindowPortBounds (graphRecordPtr->window, &graphRecordPtr->clientRect);
#	endif	// defined multispec_mac
           
#	if defined multispec_win
		CMGraphView* graphViewCPtr = graphRecordPtr->graphViewCPtr;
		graphViewCPtr->GetClientRect ((RECT*)&graphRecordPtr->clientRect);
#	endif	// defined multispec_win
			
				// Now determine the y-axis min, max and interval.\
														
	interval = (graphRecordPtr->yMax - graphRecordPtr->yMin)/5;
	
	if (graphRecordPtr->yValueTypeCode == kIntegerType)
		{
		if (interval < 1)
			interval = 1;
			
		integerInterval = (UInt32)(interval + 0.5);
	
		numberDigits = sprintf (number, "%ld", integerInterval) - 2;
		if (numberDigits >= 1)
			{
			additional = (UInt32)pow((double)10,(double)numberDigits);
			integerInterval = (integerInterval+additional)/additional;
			integerInterval *= additional;
			
			}		// end "if (numberDigits >= 1)"
			
		interval = (double)integerInterval;
		
		graphRecordPtr->yESignificantDigits = 0;
		graphRecordPtr->yFSignificantDigits = 0; 
			
		}		// end "if (graphRecordPtr->yValueTypeCode == kIntegerType)"
				
	else		// ...->yValueTypeCode == kRealType
		{
		numberDigits = 1;
		if (graphRecordPtr->yMax != 0)
			numberDigits = (SInt32)log10(fabs(graphRecordPtr->yMax));
		
		if (interval == 0)
			{
			if (numberDigits > 10)
				interval = pow((double)10,(double)(numberDigits-10));
				
			else		// numberWholeDigits <= 10
				interval = pow((double)10,(double)(numberDigits-1));

			if (interval == 0)
				interval = 1;
				
			}		// end "if (interval == 0)"
			
		else		// interval > 0
			{
			if (interval < 1)
				{
				numberDigits = 0;
				numberDecimalPlaces = GetNumberLeadingDecimalZeros (interval);
				realAdditional = pow((double)10,(double)(-numberDecimalPlaces));
				if (realAdditional > 0)
					{
					interval += realAdditional;
					interval = floor (interval/realAdditional)*realAdditional;

					}		// end "if (realAdditional > 0)"
				
				}		// end "if (interval < 1)"
			
			else		// interval >= 1
				{
				numberIntervalDigits = (SInt32)log10(interval);
//				interval = pow((double)10,(double)(numberDigits-1));
				
				realAdditional = pow((double)10,(double)numberIntervalDigits);
				if (realAdditional > 0)
					{
					interval = (interval+realAdditional)/realAdditional;
					interval = floor (interval)*realAdditional;

					}		// end "if (realAdditional > 0)"
				
				}		// end "else interval >= 1"
				
			}		// end "else interval > 0"
		
		numberIntervalDigits = 0;
		if (interval > 1)
			numberIntervalDigits = (SInt32)log10(interval);
			
		graphRecordPtr->yFSignificantDigits = GetNumberLeadingDecimalZeros (interval);
		
		if (graphRecordPtr->yFSignificantDigits == 0 && 
													numberDigits > numberIntervalDigits)
			{
			graphRecordPtr->yFSignificantDigits = (UInt16)(numberDigits - numberIntervalDigits);
//			numberDecimalPlaces = MIN (6, numberDecimalPlaces);
			
			}		// end "if (graphRecordPtr->yFSignificantDigits == 0)"
		
		}		// end "else ...->yValueTypeCode == kRealType"
	
	if ( !(graphRecordPtr->attrb & NU_YLOG) )
		{
		graphRecordPtr->yScaleMin = floor (graphRecordPtr->yMin/interval)*interval;
		graphRecordPtr->yScaleMax = ceil (graphRecordPtr->yMax/interval)*interval;
		
		if (graphRecordPtr->yScaleMin == graphRecordPtr->yScaleMax)
			{
			if (graphRecordPtr->yScaleMin == graphRecordPtr->yMin)
				graphRecordPtr->yScaleMin -= interval;
				
			if (graphRecordPtr->yScaleMax == graphRecordPtr->yMax)
				graphRecordPtr->yScaleMax += interval;
				
			}		// end "if (graphRecordPtr->yMin == graphRecordPtr->yMax)" 
		
		}		// end "if ( !(graphRecordPtr->attrb & NU_YLOG) )"
	
	graphRecordPtr->yInt = interval;
		
		// Get the significant digits to use for the y-scale.
		
	GetNumberDecimalDigits (graphRecordPtr->yValueTypeCode,
										graphRecordPtr->yScaleMin,
										graphRecordPtr->yScaleMax,
										interval,
										&graphRecordPtr->yESignificantDigits,
										&graphRecordPtr->yFSignificantDigits);
		
			//	User defined.
						
//	graphRecordPtr->yMin = 7;
//	graphRecordPtr->yMax = 49;
//	graphRecordPtr->yInt = 7;
			
			// Now for selection graph windows, force the y-axis min, max and interval
			// to be the same for all graph windows that belong to this image window. 
		
	if (graphRecordPtr->processorCode == kSelectionGraphProcessor)
		SetLikeWindowMinMax (graphRecordPtr->imageWindow, 
										graphRecordPtr->yScaleMin, 
										graphRecordPtr->yScaleMax,
										graphRecordPtr->yInt);
	
}		// end "SetYGraphScale" 		 	



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetLikeWindowMinMax
//
//	Software purpose:	The purpose of this routine is to set the min and max
//							values for all graph windows that belong to the input 
//							image window to the same value and force those windows to
//							be redrawn.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/08/1996
//	Revised By:			Larry L. Biehl			Date: 12/30/2005			

void SetLikeWindowMinMax (
				WindowPtr							imageWindowPtr,
				double								min,
				double								max,
				double								interval)

{
	GraphPtr								graphRecordPtr;
	
	Handle								graphRecordHandle;
	
	UInt16								index;
	
		
				// Loop through the graph windows.													
				
	if ( gNumberOfGWindows > 0 )
		{
		for ( index=gNumberOfIWindows+gNumberOfGWindows+2; 
				index>(UInt16)(gNumberOfIWindows+2); 
				index--)
			{	
			graphRecordHandle = GetGraphRecordHandle ( gWindowList[index] );
			graphRecordPtr = (GraphPtr)GetHandlePointer (
													graphRecordHandle, kNoLock, kNoMoveHi);	
							
			if (graphRecordPtr != NULL &&
					graphRecordPtr->imageWindow == imageWindowPtr &&
						graphRecordPtr->processorCode == kSelectionGraphProcessor)
				{
				graphRecordPtr->yScaleMin = min; 
				graphRecordPtr->yScaleMax = max;
				graphRecordPtr->yInt = interval;
				
				InvalidateGraphWindow (gWindowList[index], kGraphArea);
				
				}		// end "if (...->imageWindow == imageWindowPtr && ..."
															
			}		// end "for ( index=gNumberOfIWindows ..."
			
		}		// end "if ( gNumberOfGWindows > 0 )" 
	
}		// end "SetLikeWindowMinMax" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean SetHistogramBinWidth
//
//	Software purpose:	This routine sets the histogram bin width code
// 							to be used.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None	
// 
// Called By:			CreateGraph in SGraUtil.cpp
//							GraphWControlEvent in SGraUtil.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/11/2002
//	Revised By:			Larry L. Biehl			Date: 12/11/2002		

Boolean SetHistogramBinWidth (
				CMGraphView*						graphViewCPtr,
				SInt16								binWidthSelection)

{
	GraphPtr								graphRecordPtr;
	
	Handle								graphRecordHandle;
	
	SInt16								histogramBinCode;
	
	
	graphRecordHandle = graphViewCPtr->GetGraphRecordHandle ();
	                
	graphRecordPtr = (GraphPtr)GetHandlePointer(
										graphRecordHandle, kNoLock, kNoMoveHi);
										
	histogramBinCode = graphRecordPtr->histogramBinCode;

	if (graphRecordPtr != NULL &&
						binWidthSelection >= 1 && binWidthSelection <= 10)
		{
				// Set bin width code.
													
		if (binWidthSelection == 1)	
			graphRecordPtr->histogramBinCode = -1;
													
		else if (binWidthSelection == 2)	
			graphRecordPtr->histogramBinCode = 1;
													
		else if (binWidthSelection == 3)		
			graphRecordPtr->histogramBinCode = 2;
													
		else if (binWidthSelection == 4)		
			graphRecordPtr->histogramBinCode = 3;
													
		else if (binWidthSelection == 5)		
			graphRecordPtr->histogramBinCode = 4;
													
		else if (binWidthSelection == 6)		
			graphRecordPtr->histogramBinCode = 5;
													
		else if (binWidthSelection == 7)		
			graphRecordPtr->histogramBinCode = 10;
													
		else if (binWidthSelection == 8)		
			graphRecordPtr->histogramBinCode = 20;
													
		else if (binWidthSelection == 9)		
			graphRecordPtr->histogramBinCode = 30;
													
		else if (binWidthSelection == 10)		
			graphRecordPtr->histogramBinCode = 50;
			
		}		// end "if (graphRecordPtr != NULL && ...)"
		
	return (histogramBinCode != graphRecordPtr->histogramBinCode);
			
}		// end Routine "SetHistogramBinWidth"   



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetOverlayDisplayList
//
//	Software purpose:	This routine sets the overlay (drawGraphCode) to the input setting
//							which comes from the select overlay popup menu.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None	
// 
// Called By:			CreateGraph in SGraUtil.cpp
//							GraphWControlEvent in SGraUtil.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/05/1999
//	Revised By:			Larry L. Biehl			Date: 01/21/2003		

void SetOverlayDisplayList (
				CMGraphView*						graphViewCPtr,
				SInt16								overlaySelection)

{
	GraphPtr								graphRecordPtr;
	
	Handle								graphRecordHandle;
	
	SInt16								overlayGraphCode;
	
	
	graphRecordHandle = graphViewCPtr->GetGraphRecordHandle ();
	                
	graphRecordPtr = (GraphPtr)GetHandlePointer(
										graphRecordHandle, kNoLock, kNoMoveHi);
   
   
	if (graphRecordPtr != NULL &&
				graphRecordPtr->graphCodesAvailable > 1 &&
						overlaySelection >= 1 && overlaySelection <= 3)
		{
				// Indicate plot is to be drawn.
				
		overlayGraphCode = (graphRecordPtr->drawGraphCode & 0x0005);
		graphRecordPtr->drawGraphCode &= 0xfffa;
													
		if (overlaySelection == 1)	
			graphRecordPtr->drawGraphCode |= 0x0001;
													
		else if (overlaySelection == 2)	
			graphRecordPtr->drawGraphCode |= 0x0004;
													
		else if (overlaySelection == 3)		
			graphRecordPtr->drawGraphCode |= 0x0005;
			
		}		// end "if (graphRecordPtr != NULL && ...)"
			
}		// end Routine "SetOverlayDisplayList"   



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetStatHistogramGraphTitle2
//
//	Software purpose:	This routine gets the title for the graph for the
//							biplot graphics window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			GetGraphTitle in graphicsUtilities.c
//
//	Coded By:			Larry L. Biehl			Date: 01/23/1999
//	Revised By:			Larry L. Biehl			Date: 01/23/1999			

void SetStatHistogramGraphTitle2 (
				GraphPtr								graphRecordPtr)
											
{
	SInt16*								setListPtr;
	
	SInt16								setValue;
	
	
	if (graphRecordPtr != NULL)
		{						
		graphRecordPtr->title2[0] = 0;
		graphRecordPtr->title2[1] = 0;
		
		
		setListPtr = (SInt16*)GetHandlePointer (graphRecordPtr->setListHandle,
																kNoLock,
																kNoMoveHi);
																
		if (setListPtr != NULL)
			{
			setValue = setListPtr[graphRecordPtr->set - 1];
			
					// Create title line 2.															
					
			if (graphRecordPtr->setCode == 1)
				sprintf ( (char*)&graphRecordPtr->title2[1], 
								"Channel %hd",
								setValue);
							
			else		// graphRecordPtr->setCode == 2 
				sprintf ( (char*)&graphRecordPtr->title2[1], 
								"Feature %hd",
								setValue);
								
			}		// end "if (setListPtr != NULL)" 
								
		graphRecordPtr->title2[0] = strlen ( (char*)&graphRecordPtr->title2[1] );	
		
		}		// end "if (graphRecordPtr != NULL)" 

}		// end "SetStatHistogramGraphTitle2" 


                   
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetUpBinWidthPopUpMenu
//
//	Software purpose:	The purpose of this routine is to set up the bin width popup
//							menu.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/11/2002
//	Revised By:			Larry L. Biehl			Date: 02/14/2006

void SetUpBinWidthPopUpMenu (
				CMGraphView*						graphViewCPtr,
				MenuHandle							popUpMenuHandle)

{  
	GraphPtr								graphRecordPtr;
	
	Handle								graphRecordHandle;
													
	SInt16								histogramBinCode;
	
	SignedByte							handleStatus;

	#if defined multispec_win
		CMenu*								graphBinWidthPopupMenuPtr;
	#endif	// defined multispec_win 
	
	
	graphRecordHandle = graphViewCPtr->GetGraphRecordHandle();
                   
	graphRecordPtr = (GraphPtr)GetHandleStatusAndPointer(
												graphRecordHandle, &handleStatus, kNoMoveHi);
					
	histogramBinCode = graphRecordPtr->histogramBinCode;
												
	if (graphRecordPtr != NULL)
		{
#		if defined multispec_mac
			SInt32								index,
													numberMenuItems;
												
					// Now delete the current menu items.
					
			numberMenuItems = CountMenuItems	(popUpMenuHandle);						
			for (index=0; index<numberMenuItems; index++)			
				DeleteMenuItem (popUpMenuHandle, 1);
			
					// Add the items to the popup menu
			
	 		AppendMenu (popUpMenuHandle, "\pAuto");
			CheckMenuItem ( popUpMenuHandle, 1, (histogramBinCode<=0) );
			
	 		AppendMenu (popUpMenuHandle, "\px1");
			CheckMenuItem ( popUpMenuHandle, 2, (histogramBinCode==1) );
			
	 		AppendMenu (popUpMenuHandle, "\px2");
			CheckMenuItem ( popUpMenuHandle, 3, (histogramBinCode==2) );
			
	 		AppendMenu (popUpMenuHandle, "\px3");
			CheckMenuItem ( popUpMenuHandle, 4, (histogramBinCode==3) );
			
	 		AppendMenu (popUpMenuHandle, "\px4");
			CheckMenuItem ( popUpMenuHandle, 5, (histogramBinCode==4) );
			
	 		AppendMenu (popUpMenuHandle, "\px5");
			CheckMenuItem ( popUpMenuHandle, 6, (histogramBinCode==5) );
			
	 		AppendMenu (popUpMenuHandle, "\px10");
			CheckMenuItem ( popUpMenuHandle, 7, (histogramBinCode==10) );
			
	 		AppendMenu (popUpMenuHandle, "\px20");
			CheckMenuItem ( popUpMenuHandle, 8, (histogramBinCode==20) );
			
	 		AppendMenu (popUpMenuHandle, "\px30");
			CheckMenuItem ( popUpMenuHandle, 9, (histogramBinCode==30) );
			
	 		AppendMenu (popUpMenuHandle, "\px50");
			CheckMenuItem ( popUpMenuHandle, 10, (histogramBinCode==50) );
#		endif	// defined multispec_mac
		
#		if defined multispec_win
			UINT					checkCode;

			graphBinWidthPopupMenuPtr = (CMenu*)popUpMenuHandle;

			checkCode = MF_BYPOSITION | MF_UNCHECKED;
			if (histogramBinCode <= 0)
				checkCode = MF_BYPOSITION | MF_CHECKED;
			graphBinWidthPopupMenuPtr->CheckMenuItem (0, checkCode);

			checkCode = MF_BYPOSITION | MF_UNCHECKED;
			if (histogramBinCode == 1)
				checkCode = MF_BYPOSITION | MF_CHECKED;
			graphBinWidthPopupMenuPtr->CheckMenuItem (1, checkCode);

			checkCode = MF_BYPOSITION | MF_UNCHECKED;
			if (histogramBinCode == 2)
				checkCode = MF_BYPOSITION | MF_CHECKED;
			graphBinWidthPopupMenuPtr->CheckMenuItem (2, checkCode);

			checkCode = MF_BYPOSITION | MF_UNCHECKED;
			if (histogramBinCode == 3)
				checkCode = MF_BYPOSITION | MF_CHECKED;
			graphBinWidthPopupMenuPtr->CheckMenuItem (3, checkCode);

			checkCode = MF_BYPOSITION | MF_UNCHECKED;
			if (histogramBinCode == 4)
				checkCode = MF_BYPOSITION | MF_CHECKED;
			graphBinWidthPopupMenuPtr->CheckMenuItem (4, checkCode);

			checkCode = MF_BYPOSITION | MF_UNCHECKED;
			if (histogramBinCode == 5)
				checkCode = MF_BYPOSITION | MF_CHECKED;
			graphBinWidthPopupMenuPtr->CheckMenuItem (5, checkCode);

			checkCode = MF_BYPOSITION | MF_UNCHECKED;
			if (histogramBinCode == 10)
				checkCode = MF_BYPOSITION | MF_CHECKED;
			graphBinWidthPopupMenuPtr->CheckMenuItem (6, checkCode);

			checkCode = MF_BYPOSITION | MF_UNCHECKED;
			if (histogramBinCode == 20)
				checkCode = MF_BYPOSITION | MF_CHECKED;
			graphBinWidthPopupMenuPtr->CheckMenuItem (7, checkCode);

			checkCode = MF_BYPOSITION | MF_UNCHECKED;
			if (histogramBinCode == 30)
				checkCode = MF_BYPOSITION | MF_CHECKED;
			graphBinWidthPopupMenuPtr->CheckMenuItem (8, checkCode);

			checkCode = MF_BYPOSITION | MF_UNCHECKED;
			if (histogramBinCode == 50)
				checkCode = MF_BYPOSITION | MF_CHECKED;
			graphBinWidthPopupMenuPtr->CheckMenuItem (9, checkCode);
#		endif	// defined multispec_win
			
		}		// end "if (graphRecordPtr != NULL)"
			
	MHSetState (graphRecordHandle, handleStatus);
			
}		// end Routine "SetUpBinWidthPopUpMenu"   


                   
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetUpOverlayPopUpMenu
//
//	Software purpose:	The purpose of this routine is to set up the overlay popup
//							menu.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/05/1999
//	Revised By:			Larry L. Biehl			Date: 11/22/2002

void SetUpOverlayPopUpMenu (
				CMGraphView*						graphViewCPtr,
				MenuHandle							popUpMenuHandle)

{  
	GraphPtr								graphRecordPtr;
	
	Handle								graphRecordHandle;
													
	SInt16								drawGraphCode;
	
	SignedByte							handleStatus;

#	if defined multispec_win
		CMenu*								graphOverlayPopupMenuPtr;
#	endif	// defined multispec_win
	
	
	graphRecordHandle = graphViewCPtr->GetGraphRecordHandle();
                   
	graphRecordPtr = (GraphPtr)GetHandleStatusAndPointer(
												graphRecordHandle, &handleStatus, kNoMoveHi);
					
	drawGraphCode = (graphRecordPtr->drawGraphCode & 0x0005);
												
	if (graphRecordPtr != NULL)
		{
#		if defined multispec_mac
			SInt32								index,
													numberMenuItems;
													
												
					// Now delete the current menu items.
					
			numberMenuItems = CountMenuItems	(popUpMenuHandle);						
			for (index=0; index<numberMenuItems; index++)			
				DeleteMenuItem (popUpMenuHandle, 1);
			
					// Add the items to the popup menu
			
	 		AppendMenu (popUpMenuHandle, "\pDraw histogram");
			CheckMenuItem ( popUpMenuHandle, 1, (drawGraphCode==1) );
			
	 		AppendMenu (popUpMenuHandle, "\pDraw density function");
			CheckMenuItem ( popUpMenuHandle, 2, (drawGraphCode==4) );
			
	 		AppendMenu (popUpMenuHandle, "\pDraw histogram & density function");
			CheckMenuItem ( popUpMenuHandle, 3, (drawGraphCode==5) );
#		endif	// defined multispec_mac
		
#		if defined multispec_win
			UINT					checkCode;

			graphOverlayPopupMenuPtr = (CMenu*)popUpMenuHandle;

			checkCode = MF_BYPOSITION | MF_UNCHECKED;
			if (drawGraphCode == 1)
				checkCode = MF_BYPOSITION | MF_CHECKED;
			graphOverlayPopupMenuPtr->CheckMenuItem (0, checkCode);

			checkCode = MF_BYPOSITION | MF_UNCHECKED;
			if (drawGraphCode == 4)
				checkCode = MF_BYPOSITION | MF_CHECKED;
			graphOverlayPopupMenuPtr->CheckMenuItem (1, checkCode);

			checkCode = MF_BYPOSITION | MF_UNCHECKED;
			if (drawGraphCode == 5)
				checkCode = MF_BYPOSITION | MF_CHECKED;
			graphOverlayPopupMenuPtr->CheckMenuItem (2, checkCode);
#		endif	// defined multispec_win
			
		}		// end "if (graphRecordPtr != NULL)"
			
	MHSetState (graphRecordHandle, handleStatus);
			
}		// end Routine "SetUpOverlayPopUpMenu"   


                   
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetUpVectorPopUpMenu
//
//	Software purpose:	The purpose of this routine is to set up the vector popup
//							menu with the vector labels stored in the graph structure.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/27/1999
//	Revised By:			Larry L. Biehl			Date: 11/27/2002

void SetUpVectorPopUpMenu (
				CMGraphView*						graphViewCPtr,
				MenuHandle							popUpMenuHandle)

{  
	GraphPtr								graphRecordPtr;
	
	Handle								graphRecordHandle;
	
	SignedByte							handleStatus;

#	if defined multispec_win
		CMenu*								selectVectorsPopupMenuPtr;
		UInt32								popupMenuIndex;
		UINT									checkCode;
#	endif	// defined multispec_win
	
	
	graphRecordHandle = graphViewCPtr->GetGraphRecordHandle();
                   
	graphRecordPtr = (GraphPtr)GetHandleStatusAndPointer(
												graphRecordHandle, &handleStatus, kNoMoveHi);
												
	if (graphRecordPtr->vectorLabelHandle != NULL)
		{
		UCharPtr								stringPtr,
												vectorDisplayPtr;
		 
		SInt32								index;
													
												
					// Now delete the current menu items.

#		if defined multispec_mac
			SInt32								numberMenuItems;

			numberMenuItems = CountMenuItems	(popUpMenuHandle);						
			for (index=0; index<numberMenuItems; index++)			
				DeleteMenuItem (popUpMenuHandle, 1);
#		endif	// defined multispec_mac
		
#		if defined multispec_win
			selectVectorsPopupMenuPtr = (CMenu*)popUpMenuHandle;
			ClearMenuItems (selectVectorsPopupMenuPtr, 0);
			popupMenuIndex = ID_SELECTVECTORMENUITEMSTART;
#		endif	// defined multispec_mac

	 	
		stringPtr = (unsigned char*)GetHandlePointer (
															graphRecordPtr->vectorLabelHandle,
															kLock,
															kNoMoveHi);
		
		vectorDisplayPtr = (UInt8*)GetHandlePointer(
															graphRecordPtr->vectorDisplayHandle, 
															kNoLock, 
															kNoMoveHi);
			
				// Add the vector labels to the popup menu
		
		for (index=0; index<graphRecordPtr->numberVectors; index++)
			{
#			if defined multispec_mac
	 			AppendMenu (popUpMenuHandle, (UCharPtr)stringPtr);
				CheckMenuItem (popUpMenuHandle, index+1, vectorDisplayPtr[index]);
#			endif	// defined multispec_mac
		
#			if defined multispec_win
				selectVectorsPopupMenuPtr->AppendMenu(MF_STRING, 
													popupMenuIndex+index, 
													(LPCTSTR)&stringPtr[1]);

				checkCode = MF_BYPOSITION | MF_UNCHECKED;
				if (vectorDisplayPtr[index])
					checkCode = MF_BYPOSITION | MF_CHECKED;
				selectVectorsPopupMenuPtr->CheckMenuItem (
													index,
													checkCode);
#			endif	// defined multispec_mac
			
			stringPtr += 32;
											
			}		// end "for (index=0; index<graphRecPtr->..."
				
#		if defined multispec_mac
			AppendMenu (popUpMenuHandle, "\pAll");
			AppendMenu (popUpMenuHandle, "\pNone");
#		endif	// defined multispec_mac
		
#		if defined multispec_win
			selectVectorsPopupMenuPtr->AppendMenu(MF_STRING,
													popupMenuIndex+index, 
													(LPCTSTR)_T("All"));
			selectVectorsPopupMenuPtr->AppendMenu(MF_STRING,
													popupMenuIndex+index+1, 
													(LPCTSTR)_T("None"));
#		endif	// defined multispec_mac

		CheckAndUnlockHandle (graphRecordPtr->vectorDisplayHandle);
		CheckAndUnlockHandle (graphRecordPtr->vectorLabelHandle);
			
		}		// end "if (graphRecordPtr->vectorLabelHandle != NULL)"
			
	MHSetState (graphRecordHandle, handleStatus);
			
}		// end Routine "SetUpVectorPopUpMenu"   



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetVectorDisplayList
//
//	Software purpose:	This routine sets the vector display list to the input setting
//							which comes from the select vector popup menu.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None	
// 
// Called By:			CreateGraph in SGraUtil.cpp
//							GraphWControlEvent in SGraUtil.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/27/1999
//	Revised By:			Larry L. Biehl			Date: 11/27/2002		

void SetVectorDisplayList (
				CMGraphView*						graphViewCPtr,
				SInt16								vectorSelection)

{
	GraphPtr								graphRecordPtr;
	UCharPtr								vectorDisplayPtr;
	
	Handle								graphRecordHandle;
	
	UInt32								index,
											numberVectors;
	
	
	graphRecordHandle = graphViewCPtr->GetGraphRecordHandle ();
	                
	graphRecordPtr = (GraphPtr)GetHandlePointer(
										graphRecordHandle, kNoLock, kNoMoveHi);
										
	numberVectors = graphRecordPtr->numberVectors;
		
			// Indicate which vectors are to be drawn.
	
	vectorDisplayPtr = (UCharPtr)GetHandlePointer(
												graphRecordPtr->vectorDisplayHandle, 
												kNoLock, 
												kNoMoveHi);
   
#	if defined multispec_lin
		if ((UInt32)vectorSelection == numberVectors+1)
			{
					// Make sure that all vectors are selected.
					
			for (index=0; index<numberVectors; index++, vectorDisplayPtr++)
				*vectorDisplayPtr = 1;
			
			}		// end "if (vectorSelection == 1)"
		if ((UInt32)vectorSelection == numberVectors+2)		
			{
					// Make sure that all vectors are selected.
					
			for (index=0; index<numberVectors; index++, vectorDisplayPtr++)
				*vectorDisplayPtr = 0;
			
			}		// end "if (vectorSelection == 1)"
		
		if (vectorSelection <=numberVectors)
			{
					// Toggle the vector selection.
					
			vectorDisplayPtr[vectorSelection] = 1 - vectorDisplayPtr[vectorSelection];
			
			}		// end "else if (vectorSelection >= 0)"
#	endif

#	ifndef multispec_lin
		if ((UInt32)vectorSelection >= numberVectors+1)
			{
					// Make sure that all vectors are selected.
					
			for (index=0; index<numberVectors; index++, vectorDisplayPtr++)
				*vectorDisplayPtr = 0;
			
			}		// end "if (vectorSelection == 1)"
													
		else if ((UInt32)vectorSelection >= numberVectors)		
			{
					// Make sure that all vectors are selected.
				
			for (index=0; index<numberVectors; index++, vectorDisplayPtr++)
				*vectorDisplayPtr = 1;
			
			}		// end "if (vectorSelection == 1)"
			
		else if (vectorSelection >= 0)
			{
					// Toggle the vector selection.
					
			vectorDisplayPtr[vectorSelection] = 1 - vectorDisplayPtr[vectorSelection];
			
			}		// end "else if (vectorSelection >= 0)"
#	endif
			
}		// end "SetVectorDisplayList"   



#if defined multispec_mac 
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateGraphControls
//
//	Software purpose:	This routine controls updating the thematic image legend
//							controls.  The routine is called after any change is made 
//							in the size or content of the thematic legend.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None	
// 
// Called By:			DoGraphWActivateEvent in SGraUtil.cpp
//							GraphWControlEvent in SGraUtil.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/25/1999
//	Revised By:			Larry L. Biehl			Date: 01/26/1999		

void UpdateGraphControls (
				WindowPtr							windowPtr)

{	     
	GraphPtr								graphRecordPtr;
	             
	ControlHandle						controlHandle,
											rootControlHandle;
												
	Handle								graphRecordHandle;
	
	SInt16								numberSets,
											refCon,
											set;		
	
	UInt16								index,
											numberControls;
	
	
			// Update graph window controls
				
	if (gAppearanceManagerFlag)
		GetRootControl(windowPtr, &rootControlHandle);
		
#	if !TARGET_API_MAC_CARBON
		else		// !gAppearanceManagerFlag
			rootControlHandle = (ControlHandle)((WindowPeek)windowPtr)->controlList;
#	endif	// !TARGET_API_MAC_CARBON
	
	if (rootControlHandle == NULL)
																						return;
	
	graphRecordHandle = GetGraphRecordHandle (windowPtr);
	graphRecordPtr = (GraphPtr)GetHandlePointer(
											graphRecordHandle, kNoLock, kNoMoveHi);
											
	numberSets = graphRecordPtr->numberSets;
	set = graphRecordPtr->set;
											
	if (gAppearanceManagerFlag)
		{						
		CountSubControls (rootControlHandle, &numberControls);
		for (index=numberControls; index>=1; index--)
			{
			GetIndexedSubControl(rootControlHandle, index, &controlHandle);
			refCon = (SInt16)GetControlReference (controlHandle);
			
			switch (refCon)
				{
				case kNextGraphSetControl:
					if (numberSets <= 1)
						HideControl (controlHandle);
						
					else		// numberSets > 1
						{
						if (set < numberSets)
							HiliteControl (controlHandle, 0);
							
						else		// set >= numberSets
							HiliteControl (controlHandle, 255);
							
						}		// end "else numberSets > 1"
					break;
					
				case kPreviousGraphSetControl:
					if (numberSets <= 1)
						HideControl (controlHandle);
						
					else		// >numberSets > 1
						{
						if (set > 1)
							HiliteControl (controlHandle, 0);
							
						else		// >set <= 1
							HiliteControl (controlHandle, 255);
							
						}		// end "else numberSets <= 1"
					break;
					
				case kGraphVectorControl:
					HiliteControl (controlHandle, 0);
					break;
					
				case kGraphOverlayControl:
					HiliteControl (controlHandle, 0);
					break;
					
				case kGraphBinWidthControl:
					HiliteControl (controlHandle, 0);
					break;
				
				}		// end "switch (refCon)" 
	     			
	     	}		// end "while (controlHandle != NULL)" 
     	
     	}		// end "if (gAppearanceManagerFlag)"
     	
#	if !TARGET_API_MAC_CARBON
		else		// !gAppearanceManagerFlag
			{		
			controlHandle = rootControlHandle;				
			while (controlHandle != NULL)
				{
				refCon = (SInt16)GetControlReference (controlHandle);
				
				switch (refCon)
					{
					case kNextGraphSetControl:
						if (numberSets <= 1)
							HideControl (controlHandle);
							
						else		// numberSets > 1
							{
							if (set < numberSets)
								HiliteControl (controlHandle, 0);
								
							else		// set >= numberSets
								HiliteControl (controlHandle, 255);
								
							}		// end "else numberSets > 1"
						break;
						
					case kPreviousGraphSetControl:
						if (numberSets <= 1)
							HideControl (controlHandle);
							
						else		// >numberSets > 1
							{
							if (set > 1)
								HiliteControl (controlHandle, 0);
								
							else		// >set <= 1
								HiliteControl (controlHandle, 255);
								
							}		// end "else numberSets <= 1"
						break;
						
					case kGraphVectorControl:
						HiliteControl (controlHandle, 0);
						break;
						
					case kGraphOverlayControl:
						HiliteControl (controlHandle, 0);
						break;
					
					case kGraphBinWidthControl:
						HiliteControl (controlHandle, 0);
						break;
					
					}		// end "switch (refCon)" 
					
		     	controlHandle = (*controlHandle)->nextControl;
		     			
		     	}		// end "while (controlHandle != NULL)" 
		     	
			}		// end "else !gAppearanceManagerFlag"
#	endif	// !TARGET_API_MAC_CARBON
	
}		// end "UpdateGraphControls"
#endif	// defined multispec_mac   



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateGraphScales
//
//	Software purpose:	This routine updates the graph scales after a change in the size
//							of the graph.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None	
// 
// Called By:			DoGraphWActivateEvent in SGraUtil.cpp
//							GraphWControlEvent in SGraUtil.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/30/2003
//	Revised By:			Larry L. Biehl			Date: 04/30/2003		

void UpdateGraphScales (
				Handle								graphRecordHandle)

{	  
	GraphPtr								graphRecordPtr;
	
	
	graphRecordPtr = (GraphPtr)GetHandlePointer(
										graphRecordHandle, kLock, kNoMoveHi);
										
	if (graphRecordPtr != NULL)
		{
		SetXGraphScale (graphRecordPtr);
		
		if (graphRecordPtr->graphCodesAvailable & 0x0001 &&
											graphRecordPtr->flag & NU_HISTOGRAM_PLOT){
			SetGraphMinMax (graphRecordPtr, kYAxis);
         
      }
			
		else
			{		// !...->graphCodesAvailable & 0x0001 && ...
			SetYGraphScale (graphRecordPtr);
			}
			
		}		// end "if (graphRecordPtr != NULL)"
		
	CheckAndUnlockHandle (graphRecordHandle); 
	
}		// end "UpdateGraphScales"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void VerifyNeedForSelectVectorControl
//
//	Software purpose:	This routine verifies the need for the select vector control. If
//							the number of vectors is 1, then the control is not needed and 
//							therefore it is deleted from the window.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None	
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/27/1999
//	Revised By:			Larry L. Biehl			Date: 02/05/1999		

void VerifyNeedForSelectVectorControl (
				WindowPtr							windowPtr)

{	                  
#if defined multispec_mac 
	GraphPtr								graphRecordPtr;

	ControlHandle						controlHandle,
#	if !TARGET_API_MAC_CARBON
												nextControlHandle,
#	endif	// !TARGET_API_MAC_CARBON
											rootControlHandle;
											
	Handle								graphRecordHandle;
	
	SInt16								refCon;	
	
	UInt16								index,
											numberControls;
	
	
			// Update graph window controls	
				
	if (gAppearanceManagerFlag)
		GetRootControl(windowPtr, &rootControlHandle);
		
#	if !TARGET_API_MAC_CARBON
		else		// !gAppearanceManagerFlag
			rootControlHandle = (ControlHandle)((WindowPeek)windowPtr)->controlList;
#	endif	// !TARGET_API_MAC_CARBON
	
	if (rootControlHandle != NULL)
		{
		graphRecordHandle = GetGraphRecordHandle (windowPtr);
		graphRecordPtr = (GraphPtr)GetHandlePointer(
												graphRecordHandle, kNoLock, kNoMoveHi);
		
		}		// end "if (rootControlHandle != NULL)"
		
	if (graphRecordPtr != NULL)
		{					
		if (gAppearanceManagerFlag)	
			{												
			CountSubControls (rootControlHandle, &numberControls);
			for (index=numberControls; index>=1; index--)
				{
				GetIndexedSubControl(rootControlHandle, index, &controlHandle);
				refCon = (SInt16)GetControlReference (controlHandle);
				
				switch (refCon)
					{
					case kGraphVectorControl:
						if (graphRecordPtr->numberVectors <= 1)
							DisposeControl (controlHandle);
						break;
						
					case kGraphOverlayControl:
						if (graphRecordPtr->graphCodesAvailable == 1)
							DisposeControl (controlHandle);
						break;
						
					}		// end "switch (refCon)"
		     			
		     	}		// end "while (controlHandle != NULL)" 
		
			}		// end "if (gAppearanceManagerFlag)"
		
#		if !TARGET_API_MAC_CARBON
			else		// !gAppearanceManagerFlag
				{
				controlHandle = rootControlHandle;			
				while (controlHandle != NULL)	
					{
					refCon = (SInt16)GetControlReference (controlHandle);
					nextControlHandle = (*controlHandle)->nextControl;
					
					switch (refCon)
						{
						case kGraphVectorControl:
							if (graphRecordPtr->numberVectors <= 1)
								DisposeControl (controlHandle);
							break;
							
						case kGraphOverlayControl:
							if (graphRecordPtr->graphCodesAvailable == 1)
								DisposeControl (controlHandle);
							break;
							
						}		// end "switch (refCon)"
						
					controlHandle = nextControlHandle;
			     			
			     	}		// end "while (controlHandle != NULL)"
				
				}		// end "else !gAppearanceManagerFlag"
#		endif	// !TARGET_API_MAC_CARBON
	     	
		}		// end "if (graphRecordPtr != NULL)"
#endif	// defined multispec_mac
	
#if defined multispec_win

#endif	// defined multispec_win
	
}		// end "VerifyNeedForSelectVectorControl"  