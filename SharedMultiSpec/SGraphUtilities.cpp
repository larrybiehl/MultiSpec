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
//	File:						SGraphUtilities.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			05/05/2020
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	The routines in this file are used to handle Graph
//								windows in MultiSpec.
//
/*
	int numberChars = sprintf ((char*)gTextString3,
			" SGraphUtilities:LoadGraphXVector (xDataMin, xDataMax): %f, %f%s",
			graphRecordPtr->xDataMin,
			graphRecordPtr->xDataMax,
			gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#if defined multispec_wx  
	#include	"xGraphDoc.h" 
	#include	"xGraphFrame.h"
	#include	"xGraphView.h"  
	#include "xMultiSpec.h" 
   #include "SGraphic.h"
   #include "wx/evtloop.h"
   #include "SPrototypes.h"
#endif	// defined multispec_wx
	
#if defined multispec_mac || defined multispec_mac_swift
	#include	"MGraphView.h"
#endif	// defined multispec_mac || defined multispec_mac_swift
                            
#if defined multispec_win                 
	//#include	"CDatFile.h"
	#include "WMultiSpec.h"
	#include	"WGraphView.h" 
	#include	"WGraphDoc.h" 
	#include	"WGraphFrame.h"	 
#endif	// defined multispec_win

#define	kXAxis			1
#define	kYAxis			2
#define	kBothXYAxes		3	

#define	kLeftEdgeSpace	10


																															
			// Prototypes for routines in this file that are only called by		
			// other routines in this file.	

void	DrawGraphLabels (
				GraphPtr								graphRecPtr);

void	DrawGraphLegend (
				GraphPtr								graphRecPtr);

void	DrawGraphTitle (
				GraphPtr								graphRecPtr);

void	GetGraphTitle (
				GraphPtr								graphRecordPtr,
				SInt32								lineStart,
				SInt32								columnStart,
				SInt32								lineEnd,
				SInt32								columnEnd);

void	GetLikeWindowMinMax (
				WindowPtr							imageWindowPtr,
				double*								minPtr,
				double*								maxPtr);
					
double	GetUserHistogramBinWidth (
				GraphPtr								graphRecordPtr,
				double*								histogramBinWidthPtr,
				double								xMin,
				double								xMax);
					
void	SetLikeWindowMinMax (
				WindowPtr							imageWindowPtr,
				double								min,
				double								max,
				double								interval);

void	SetXGraphScale (
				GraphPtr								graphRecordPtr);

void 	SetYGraphScale (
				GraphPtr								graphRecordPtr);

void	UpdateSelectionGraphXAxisMinMax (
				GraphPtr								graphPtr);
 


//------------------------------------------------------------------------------------
//
//                   Copyright 1988-2020 Purdue Research Foundation
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
// Called By:			CMGraphView::FinishGraphRecordSetUp in SGraphView.cpp
//							ShowGraphWindowSelection in SSelectionGraph.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/09/1991
//	Revised By:			Larry L. Biehl			Date: 06/29/2019

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

	SInt16								processorCode;
									
	Boolean								changedFlag,
											continueFlag;
	

			// Initialize local variables.											
			
	continueFlag = FALSE;
	error = 0;
	processorCode = graphRecordPtr->processorCode; 

			// Check input variables.													
			
	if (graphRecordPtr != NULL)
		{
		continueFlag = TRUE;
		
		if (processorCode == kBiPlotDataProcessor)
			{
			xVectorSize = requestedLength;
			yVectorSize = requestedLength;
			
			}	// end "if (processorCode == kBiPlotDataProcessor)"
			
		else if (processorCode == kHistogramStatsProcessor)
			{
			xVectorSize = graphRecordPtr->xVector.size;
			yVectorSize = graphRecordPtr->xVector.size;
			
			graphRecordPtr->xVector.size = 0;
			
			}	// end "else if (processorCode == kHistogramStatsProcessor)" 
			
		else	// processorCode != kBiPlotDataProcessor 
			{
					// For x vector, allow for channel number, wavelength and band width.															
					
			xVectorSize = 3 * requestedLength;
			
					// For y vector, allow for wavelengths not being in order.
			
			yVectorSize = 2 * requestedLength * numberVectors;
			
			}	// end "else processorCode != kBiPlotDataProcessor"
		
		AllocateV (&graphRecordPtr->xVector, xVectorSize, &error);
		
		if (!error)
			AllocateV (&graphRecordPtr->yVector, yVectorSize, &error);
			
		continueFlag = (error == noErr);
		
		if (continueFlag)
			{
					// Get memory for the rest of the vectors.					
			
			numberBytes = 0;		
			if (processorCode == kBiPlotDataProcessor && 
						gProjectInfoPtr != NULL &&
							(gBiPlotDataSpecsPtr->outlineClassCode & kOutlineClasses))
				{
				numberBytes = (numberVectors-1) * sizeof (double) *
											(5 + 3 * gProjectInfoPtr->numberStatisticsChannels);
							
				}	// end "if (processorCode == kBiPlotDataProcessor..."					
					
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
						numberSets * numberVectors * numberStatVariables * sizeof (double);
							
				}	// end "if (continueFlag && ...->flag == NU_HISTOGRAM_PLOT" 
				
			if (numberBytes > 0)	
				ptr = CheckHandleSize (
							&graphRecordPtr->classStatisticsHandle,
							&continueFlag, 
							&changedFlag, 
							numberBytes);
					
			if (continueFlag)	
				ptr = CheckHandleSize (
							&graphRecordPtr->vectorDisplayHandle,
							&continueFlag, 
							&changedFlag, 
							numberVectors * sizeof (char));
							
			if (continueFlag && 
						graphRecordPtr->flag == NU_HISTOGRAM_PLOT &&
									numberVectors > 1)
				ptr = CheckHandleSize (
							&graphRecordPtr->vectorLabelHandle,
							&continueFlag,
							&changedFlag, 
							numberVectors * sizeof (Str31));
			
			if (continueFlag)		
				ptr = CheckHandleSize (
							&graphRecordPtr->vectorLengthsHandle,
							&continueFlag,
							&changedFlag, 
							numberSets * numberVectors * sizeof (SInt32));
			
			if (continueFlag)		
				ptr = CheckHandleSize (
							&graphRecordPtr->vectorPaletteColorHandle,
							&continueFlag,
							&changedFlag, 
							numberVectors * sizeof (SInt32));
			
			if (continueFlag)		
				ptr = CheckHandleSize (
							&graphRecordPtr->vectorSymbolHandle,
							&continueFlag,
							&changedFlag, 
							numberVectors * sizeof (char));
			
			if (continueFlag)		
				ptr = CheckHandleSize (
							&graphRecordPtr->xVectorDataHandle,
							&continueFlag,
							&changedFlag, 
							numberSets * numberVectors * sizeof (SInt32));
							
			if (continueFlag && graphRecordPtr->flag == NU_HISTOGRAM_PLOT)
				ptr = CheckHandleSize (
							&graphRecordPtr->setListHandle,
							&continueFlag,
							&changedFlag, 
							numberSets * sizeof (SInt16));
							
			if (continueFlag && graphRecordPtr->flag == NU_HISTOGRAM_PLOT)
				ptr = CheckHandleSize (
							&graphRecordPtr->histogramBinWidthHandle,
							&continueFlag,
							&changedFlag, 
							numberSets * sizeof (double));
							
			CheckAndUnlockHandle (graphRecordPtr->classStatisticsHandle);
			CheckAndUnlockHandle (graphRecordPtr->histogramBinWidthHandle);
			CheckAndUnlockHandle (graphRecordPtr->vectorDisplayHandle);
			CheckAndUnlockHandle (graphRecordPtr->vectorLabelHandle);
			CheckAndUnlockHandle (graphRecordPtr->vectorLengthsHandle);
			CheckAndUnlockHandle (graphRecordPtr->vectorPaletteColorHandle);
			CheckAndUnlockHandle (graphRecordPtr->vectorSymbolHandle);
			CheckAndUnlockHandle (graphRecordPtr->xVectorDataHandle);
			CheckAndUnlockHandle (graphRecordPtr->setListHandle);
			
			}	// end "if (continueFlag)" 
			
		}	// end "if (graphRecordPtr)" 
		
	return (continueFlag);
			
}	// end "CheckGraphVectorsSize" 


                    
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
// Called By:			ListFieldData in SListData.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/04/1991
//	Revised By:			Larry L. Biehl			Date: 04/06/2019

Boolean CheckSomeGraphWindowEvents (
				GraphPtr								graphRecPtr)

{  
#if defined multispec_mac
	char									theChar;
	
	Point									mousePt;
	
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
			
		if (haveEvent)
			{
			what = gEventRecord.what;
			if (IsDialogEvent (&gEventRecord))
				{
				if (!DialogSelect (&gEventRecord, &dialogPtr, &itemHit))
					{
					if (what == updateEvt)
						what = nullEvent;
						
					window = GetDialogWindow (dialogPtr);
						
					}	// end "if (!DialogSelect (&gEventRecord, ..." 
				
				}	// end "if (IsDialogEvent (&gEventRecord))" 
				
	    	mouseLoc = FindWindow (gEventRecord.where, &window);
				
			if (what == mouseDown && window != graphRecPtr->window)
				what = nullEvent;
				
	    	switch (what)
	    		{
	    		case mouseDown:
					switch (mouseLoc) 
						{
						case inSysWindow:
							break;
							
						case inDrag:
	    					DragMultiSpecWindow (window, gEventRecord.where);
							break;
							
						case inGoAway:
							if (TrackGoAway (window, gEventRecord.where)) 
																						return (FALSE);
							break;
							
						case inGrow:
							windowInfoHandle = (Handle)GetWRefCon (window);
							DoGraphWindowGrow (window, windowInfoHandle);
							break;
							
						case inContent:
							mousePt = gEventRecord.where;
							GlobalToLocal (&mousePt);
							DoGraphWMouseDnContent (window, mousePt);
							break;
							
						default:
							break;
						}
	    			break;
	    			
 				case keyDown:
 							// If the command key and the period have been depressed	
 							// then return FALSE indicating that the user wants to	
 							// stop the current process.										
 							
					if ((gEventRecord.modifiers & cmdKey) &&
								((gEventRecord.message & charCodeMask) == 0x2e))
																							return (FALSE);
																					
					else if ((gEventRecord.message & charCodeMask) == 0x1B)
																							return (FALSE);
																					
					else if ((gEventRecord.modifiers & 256) && 
								((gEventRecord.message & 255) == 'c')) 
						{
								// Handle a graph copy.											
								
						continueFlag = DoGraphWindowEdit (kCopy);
							
						if (!continueFlag)
																							return (FALSE);
							
						}
					
					else	// Check for right arrow to go to the next graph
						{
						theChar = (char)gEventRecord.message&charCodeMask;
						if (theChar == kRightArrowCharCode)
																							return (TRUE);
						
						}	// end "else !(gEventRecord.modifiers&cmdKey)" 						
					break;
		  
		  		case updateEvt:
		  			DoUpdateEvent (&gEventRecord);
	         	break;
		    
				case activateEvt:
					DoActivateEvent ((WindowPtr)gEventRecord.message,
													(gEventRecord.modifiers & activeFlag));
					break;
		    
				case osEvt:
					DoSuspendResume (&gEventRecord);
					break;
			
				} 			// end of case what 
				
			}	// end "if (haveEvent)" 
			
		}		while (!quitFlag);
		
	return (TRUE);  
#endif	// defined multispec_mac

#if defined multispec_mac_swift
	return (FALSE);
#endif	// defined multispec_mac_swift

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
   
   CMGraphDoc* graphDocumentPtr = (CMGraphDoc*)graphRecPtr->graphViewCPtr->GetDocument ();																					
   frame_hWnd = (graphDocumentPtr->GetGraphFrameCPtr ())->m_hWnd;						
   view_hWnd = graphViewCPtr->m_hWnd;
   	             
   quitFlag = FALSE; 
   do
   	{                                                                           
 		while (::PeekMessage (&msgCur, NULL, 0, 0, PM_NOREMOVE))		//PM_NOREMOVE
	 		{ 
			if (graphViewCPtr->m_closeGraphSelectedFlag)
				{                                     
 				quitFlag = TRUE;
				graphViewCPtr->m_closeGraphSelectedFlag = FALSE;
	 																					return (FALSE);

				}	// end "if (graphViewCPtr->m_closeGraphSelectedFlag)"
			
	 		//if (msgCur.hwnd == view_hWnd || msgCur.hwnd == frame_hWnd ||
			//																msgCur.hwnd == NULL)
			//	{
				if (!((CMultiSpecApp*)AfxGetApp ())->PumpMessage ()) 
	 																					return (FALSE);

			//	}	// end "if (msgCur.hwnd == frame_hWnd)"
				
	 		if (msgCur.message == WM_KEYDOWN)
	 			{
	 			if (msgCur.wParam == VK_ESCAPE)
	 																					return (FALSE);
				/*
				else if (msgCur.wParam == VK_LEFT)
					{
					::PeekMessage (&msgCur, NULL, VK_LEFT, VK_LEFT, PM_REMOVE);
	 																					return (FALSE);
					}
				*/
				else if (msgCur.wParam == VK_RIGHT)
					{
					::PeekMessage (&msgCur, NULL, VK_RIGHT, VK_RIGHT, PM_REMOVE);
   				quitFlag = TRUE;
 					break;  

					}	// end "else if ((msgCur.wParam == VK_RIGHT)"
	 			
	 			}	// end "if (msgCur.message == WM_KEYDOWN)" 
	 			
	 		else if (msgCur.hwnd == view_hWnd && msgCur.message == WM_LBUTTONDBLCLK)
	 			{                                                                             
 				quitFlag = TRUE;
 				break;   
	 			
	 			}	// end "else if (msgCur.message == WM_LBUTTONDBLCLK)"
	 			
	 		else if (msgCur.hwnd == view_hWnd && msgCur.message == WM_LBUTTONDOWN)
	 			{                                                                             
 				quitFlag = TRUE;
 				break;   
	 			
	 			}	// end "else if (msgCur.message == WM_LBUTTONDOWN)"
	 			
	 		else if (msgCur.message == WM_SYSCOMMAND)
	 			{
	 			if (msgCur.hwnd == frame_hWnd)
	 				{                                                     
 					quitFlag = TRUE;
 					break;
 					
 					}	// end "if (msgCur.hwnd == frame_hWnd)"            
	 			
	 			}	// end "else if (msgCur.message == WM_DESTROY)"
	 		
	 		}	// end "while (::PeekMessage (..." 
 		
 		}	while (!quitFlag);
   
 	return (returnFlag); 
#endif	// defined multispec_win 
   
#ifdef multispec_wx
   Boolean	quitFlag = FALSE;
   Boolean returnFlag = FALSE;
	
	
   //eventLoopBasePtr->YieldFor (wxEVT_CATEGORY_UI);
   wxEventLoopBase* eventLoopBasePtr = wxEventLoopBase::GetActive ();
	
   while (!quitFlag)
		{
      eventLoopBasePtr->YieldFor (wxEVT_CATEGORY_USER_INPUT + wxEVT_CATEGORY_UI);

      if (gListDataCode == 1)
			{
         returnFlag = TRUE;
         quitFlag = TRUE;
         gListDataCode = 0;
			
			}	// end "if (gListDataCode == 1)"
      
      //if (wxGetKeyState (WXK_F2)){
      if (gListDataCode == 2)
			{
         returnFlag = FALSE;
         quitFlag = TRUE;
         gListDataCode = 3;
			
			}	// end "if (gListDataCode == 2)"
			
		}	// end "while (!quitFlag)"
   
   return (returnFlag);
#endif
		
}	// end "CheckSomeGraphWindowEvents"


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
// Called By:			ShowGraphWindowSelection in SSelectionGraph.cpp
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
		inputGraphRecordPtr = (GraphPtr)GetHandlePointer (
													inputSelectionGraphRecHandle,
													kLock);
		
		outputGraphRecordPtr->yVector.numberPoints = 
												inputGraphRecordPtr->yVector.numberPoints;
		outputGraphRecordPtr->numberVectors = inputGraphRecordPtr->numberVectors;
		vectorLength = inputGraphRecordPtr->yVector.numberPoints; 
														
		LockAndGetVectorPointer (&inputGraphRecordPtr->yVector, &error);
		
		if (error == noErr)
			LockAndGetVectorPointer (&outputGraphRecordPtr->yVector, &error);
		
		
		if (error == noErr)
			{
			for (index=0; index<vectorLength; index++)
				{
				SetV (&outputGraphRecordPtr->yVector, 
						index, 
						inputGraphRecordPtr->yVector.basePtr[index], 
						&error);
				
				}	// end "for (index=0; index<localNumberChannels..." 
						
			}	// end "if (error == noErr)" 
			
		UnlockVectorPointer (&inputGraphRecordPtr->yVector);
		UnlockVectorPointer (&outputGraphRecordPtr->yVector);
		
		CheckAndUnlockHandle (inputSelectionGraphRecHandle);
		
		}	// end "if (inputSelectionGraphRecHandle != NULL)" 

}	// end "CopyGraphYVector" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
// Called By:			BiPlotDataControl in SBiPlotData.cpp
//							ListFieldData in SListData.cpp
//							ShowGraphWindowSelection in SSelectionGraph.cpp
//							HistogramStatsControl in SProjectHistogramStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/03/1991
//	Revised By:			Larry L. Biehl			Date: 02/21/2019

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
					
		//GetGraphLabels (graphRecordPtr);
		
		graphRecordPtr = NewGraph (graphRecordPtr, 
											graphRecordPtr->window,
											graphRecordPtr->xScaleMin, 
											graphRecordPtr->xScaleMax, 
											graphRecordPtr->yScaleMin, 
											graphRecordPtr->yScaleMax, 
											graphRecordPtr->attrb, 
											TRUE, 
											&error);
								
		if (error == noErr) 
			{
			#if defined multispec_mac 
				InvalidateGraphWindow (graphRecordPtr->window, kFrameArea);
			#endif	// defined multispec_mac
			         
			#if defined multispec_win
				if (graphRecordPtr->graphViewCPtr != NULL)
					graphRecordPtr->graphViewCPtr->Invalidate ();	 
			#endif	// defined multispec_win

			#if defined multispec_wx
            if (graphRecordPtr->graphViewCPtr != NULL)
					{
					//graphRecordPtr->graphViewCPtr->Invalidate ();
               if (graphRecordPtr->drawGraphCode <= 0)
                  graphRecordPtr->drawGraphCode = 1;
               if (graphViewCPtr != NULL)
                  graphViewCPtr->m_frame->Refresh ();
						
					}	// end "if (graphRecordPtr->graphViewCPtr != NULL)"
         #endif
         
			if (graphRecordPtr->drawGraphCode <= 0)
				graphRecordPtr->drawGraphCode = 1;
				
         //UpdateGraphControls (graphRecordPtr->window);
			
			}	// end "if (error == noErr)" 
		
		}	// end "if (graphRecordPtr != NULL)" 
		
	return (error == 0);

}	// end "CreateGraph"  
           


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
		DeallocateV (&graphRecordPtr->xVector, &error);
		DeallocateV (&graphRecordPtr->yVector, &error);
							
		UnlockAndDispose (graphRecordPtr->classStatisticsHandle);
		UnlockAndDispose (graphRecordPtr->histogramBinWidthHandle);
		UnlockAndDispose (graphRecordPtr->setListHandle);
		UnlockAndDispose (graphRecordPtr->vectorDisplayHandle);
		UnlockAndDispose (graphRecordPtr->vectorLabelHandle);
		UnlockAndDispose (graphRecordPtr->vectorLengthsHandle);
		UnlockAndDispose (graphRecordPtr->vectorPaletteColorHandle);
		UnlockAndDispose (graphRecordPtr->vectorSymbolHandle);
		UnlockAndDispose (graphRecordPtr->xVectorDataHandle);
			
		}	// end "if (graphRecordPtr)" 
			
}	// end "DisposeOfGraphRecordMemory" 

                            
                            
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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

			
	graphRecordHandle = graphViewCPtr->GetGraphRecordHandle ();
		             
	graphRecordPtr = (GraphPtr)GetHandleStatusAndPointer (
															graphRecordHandle, &handleStatus);
				
	if (graphRecordPtr != NULL)
		{
		if (controlIdentifier == kNextGraphSetControl &&
								graphRecordPtr->set < graphRecordPtr->numberSets)
			{
			graphRecordPtr->set++;
			invalidateFlag = TRUE;
			
			}	// end "if (refCon == kNextGraphSetControl && ..."
			
		else if (controlIdentifier == kPreviousGraphSetControl &&
								graphRecordPtr->set > 1)
			{
			graphRecordPtr->set--;
			invalidateFlag = TRUE;
			
			}	// end "if (refCon == kNextGraphSetControl && ..."

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
			
			}	// end "if (invalidateFlag)"

		}	// end "if (graphRecordPtr != NULL)"
			
	MHSetState (graphRecordHandle, handleStatus);

}	// DoNextOrPreviousChannel

                            
                            
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
//	Revised By:			Larry L. Biehl			Date: 02/20/2019

Boolean DrawGraph	(
				GraphPtr								graphRecPtr,
				Boolean								copyFlag)

{
	SInt32								error;
	
	SInt16								bottomInset,
											leftInset,
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
				// kLeftEdgeSpace extra pixels are allow on the left for some white space before the y-axis
				// labels
				
		leftInset = (SInt16)MAX (graphRecPtr->yTicLabelWidth*graphRecPtr->textScaling + kLeftEdgeSpace,
											graphRecPtr->xTicLabelWidth/2 + 3 + kLeftEdgeSpace);
			
		rightInset = (SInt16)MAX (23*graphRecPtr->textScaling, 
											graphRecPtr->xTicLabelWidth/2 + 3);

		if (graphRecPtr->processorCode == kHistogramStatsProcessor)
			{
			#if defined multispec_mac || defined multispec_mac_swift
				bottomInset = (SInt16)(graphRecPtr->textSize * 3.2 + 3*graphRecPtr->textScaling + 17);
			#endif	// defined multispec_mac || defined multispec_mac_swift
										 
			#if defined multispec_win 
				bottomInset = (SInt16)(graphRecPtr->textSize * 3.2 + 3*graphRecPtr->textScaling + 28);	// + 19
			#endif	// defined multispec_win
				
			#if defined multispec_wx 
				#if defined multispec_wxmac
					bottomInset = (SInt16)(graphRecPtr->textSize * 3.2 + 3*graphRecPtr->textScaling + 10);	// 60
				#else
					bottomInset = (SInt16)(graphRecPtr->textSize * 3.2 + 3*graphRecPtr->textScaling + 19);
				#endif
			#endif	// defined multispec_wx

			}	// end "if (graphRecPtr->processorCode == kHistogramStatsProcessor)"

		else   // graphRecPtr->processorCode != kHistogramStatsProcessor)
			{
			#if defined multispec_mac || defined multispec_mac_swift
				bottomInset = (SInt16)(graphRecPtr->textSize * 3.2 + 3*graphRecPtr->textScaling + 17);
			#endif	// defined multispec_mac || defined multispec_mac_swift
										 
			#if defined multispec_win 
				bottomInset = (SInt16)(graphRecPtr->textSize * 3.2 + 3*graphRecPtr->textScaling + 15);
			#endif	// defined multispec_win
				
			#if defined multispec_wx 
				bottomInset = (SInt16)(graphRecPtr->textSize * 3.2 + 3*graphRecPtr->textScaling + 19);
			#endif	// defined multispec_wx

			}	// end "else graphRecPtr->processorCode != kHistogramStatsProcessor"
			
		InsetGraph (
				graphRecPtr, 
				leftInset,		// 36 + 3
				(SInt16)(graphRecPtr->textSize * 3.0 + 3*graphRecPtr->textScaling),
				rightInset,		// 20 + 3                          
				bottomInset,
				&error);
			
      //if (gProcessorCode == kSelectionGraphProcessor)
		//	graphRecPtr->clientRect.bottom = 250;
      
				// Draw the white rectangle
				
		DrawAxis (graphRecPtr, &error); 
 																			
		if (graphRecPtr->drawGraphCode & 0x8000)       
				// Do not draw any part of graph.
																						return (noErr); 
		
		if ((graphRecPtr->drawGraphCode & 0x0001) &&
								(graphRecPtr->xVector.numberPoints == 0 || 
												graphRecPtr->yVector.numberPoints == 0))
																						return (noErr);   

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
					RGBForeColor (&grayColor);	
					 
					DrawLinTicks (graphRecPtr, 
										graphRecPtr->xInt, 
										graphRecPtr->yInt, 
										1000 * graphRecPtr->textScaling,
										&error);
										
					RGBForeColor (&savedForeGroundColor);
				#endif	// defined multispec_mac	       
						                                     
				#if defined multispec_win
					CPen* oldPenPtr = 
								graphRecPtr->pDC->SelectObject (&CMGraphView::s_ltGrayPen);
					 
					DrawLinTicks (graphRecPtr, 
										graphRecPtr->xInt, 
										graphRecPtr->yInt, 
										(SInt16)(1000 * graphRecPtr->textScaling), 
										&error); 
										
					graphRecPtr->pDC->SelectObject (oldPenPtr);
				#endif	// defined multispec_win	

            #if defined multispec_wx
               //graphRecPtr->pDC->SetPen (CMGraphView::s_ltGrayPen); 
               wxPen oldPenPtr = graphRecPtr->pDC->GetPen ();
               wxColour gray;
               gray.Set (wxT("#d4d4d4"));
               graphRecPtr->pDC->SetPen (wxPen (gray)); 

							// Draw gray grid column line

					wxDC* pDC = graphRecPtr->pDC;
               wxFont ffont (
							gFontSize, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
               pDC->SetFont (ffont);

					DrawLinTicks (graphRecPtr,
										graphRecPtr->xInt, 
										graphRecPtr->yInt, 
										(SInt16)(1000 * graphRecPtr->textScaling), 
										&error); 
               
               graphRecPtr->pDC->SetPen (oldPenPtr);
            #endif	// defined multispec_wx

				}	// end "if (graphRecPtr->attrb & NU_GRID)" 
				                       
			#if defined multispec_mac
				//DrawLinTicks (graphRecPtr, graphRecPtr->xInt/5, graphRecPtr->yInt/5, 5, &error);
				DrawLinTicks (graphRecPtr, graphRecPtr->xInt, graphRecPtr->yInt, 8, &error);
			#endif	// defined multispec_mac	             
						                                     
			#if defined multispec_win
				DrawLinTicks (graphRecPtr, 
									graphRecPtr->xInt, 
									graphRecPtr->yInt,
									(SInt16)(8 * graphRecPtr->textScaling),
									&error);
			#endif	// defined multispec_win
	            
         #if defined multispec_wx
            wxPen oldPenPtr = graphRecPtr->pDC->GetPen ();
            wxColour gray;
            gray.Set (wxT("#d4d4d4"));
            graphRecPtr->pDC->SetPen (wxPen (gray)); 
				/*   
            DrawLinTicks (graphRecPtr, 
									graphRecPtr->xInt, 
									graphRecPtr->yInt,
									(SInt16)(8 * graphRecPtr->textScaling),
									&error);
				*/
            graphRecPtr->pDC->SetPen (oldPenPtr);
         #endif // defined multispec_wx
			
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
								
			DrawGraphTitle (graphRecPtr);

			DrawGraphLabels (graphRecPtr);
	
					// Draw data.
			
			if (graphRecPtr->drawGraphCode & 0x0001)
				FlaggedPlotV (graphRecPtr, 
										&graphRecPtr->xVector,
										&graphRecPtr->yVector, 
										graphRecPtr->flag, 
										5, 
										&error);
			
					// Draw biplot ellipses.
			if (graphRecPtr->drawGraphCode & 0x0002)
				EllipsePlotV (graphRecPtr, &error);
				
					// Draw density function for original statistics.
         
			if (graphRecPtr->drawGraphCode & 0x0004)
				DensityFunctionPlotV (graphRecPtr, &error, 0x0004);
				
					// Draw density function for LOO statistics.
	
			if (graphRecPtr->drawGraphCode & 0x0008)
				DensityFunctionPlotV (graphRecPtr, &error, 0x0008);
				
					// Draw density function for Enhanced statistics.
	
			if (graphRecPtr->drawGraphCode & 0x0010)
				DensityFunctionPlotV (graphRecPtr, &error, 0x0010);

			DrawGraphLegend (graphRecPtr);

			}	// end "if (continueFlag)"
		
		//if (!copyFlag)	
		//	DrawGraphControls (graphRecPtr->window);
		
		}	// end "if (graphRecordPtr != NULL)" 
	
	if (!continueFlag)
		return (noErr);
	
	else	// continueFlag		
		return (error==0);

}	// end "DrawGraph" 	
	


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
// Called By:			DoGraphWindowGrow in SGraphUtilities.cpp
//							DrawGraph in SGraphUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/23/1999
//	Revised By:			Larry L. Biehl			Date: 04/06/2019

void DrawGraphControls (
				WindowPtr							windowPtr)
				
{                                           
	#if defined multispec_mac
	
				// Local Declarations			
		
		Rect									rect;	
		//Str31									tempString;
	
		GrafPtr								savedPort;	
		GraphPtr								graphRecordPtr;
	 
		ControlHandle						controlHandle,
												rootControlHandle;
		
		Handle								graphRecordHandle;
		
		SInt16								left,
												numberSets,
												refCon,
												setCode,
												//textWidth,
												top;	
	
		UInt16								index,
												numberControls;
																					
		Boolean								validateFlag;
		
	
				// Check input parameter.															
				
		if (windowPtr == NULL)
																							return;
																							
				//	Initialize local parameters  (graph port)									
	
		GetWindowPortBounds (windowPtr, &rect);
	     	
				// Get some graph parameters.
		
		graphRecordHandle = GetGraphRecordHandle (windowPtr);
				                   
		graphRecordPtr = (GraphPtr)GetHandlePointer (graphRecordHandle);
													
		//if (graphRecordPtr->numberSets <= 1)
		//																					return;
		
		setCode = graphRecordPtr->setCode;	
		numberSets = graphRecordPtr->numberSets;	
				
		GetPort (&savedPort);
		SetPortWindowPort (windowPtr);
			
				// Now draw the controls for the graph window.	
				
		GetRootControl (windowPtr, &rootControlHandle);
		if (rootControlHandle != NULL)
			{																					
			CountSubControls (rootControlHandle, &numberControls);
			for (index=numberControls; index>=1; index--)
				{
				GetIndexedSubControl (rootControlHandle, index, &controlHandle);
				validateFlag = FALSE;
				refCon = (SInt16)GetControlReference (controlHandle);
				switch (refCon)
					{
					case kNextGraphSetControl:
						
								// Draw the go to next set control.											
				
						left = rect.left + 105;
						top  = rect.bottom - kSBarWidth;
						MoveControl (controlHandle, left, top);
						
						SizeControl (controlHandle, kLegendScrollWidth, kSBarWidth+1);
						break;
							
					case kPreviousGraphSetControl:
						
								// Draw the go back to previous set control.										
								
						left = rect.left + 105 + kLegendScrollWidth;
						top  = rect.bottom - kSBarWidth;
						MoveControl (controlHandle, left, top);
						
						SizeControl (controlHandle, kLegendScrollWidth, kSBarWidth+1);
						break;
							
					case kGraphVectorControl:
						
								// Draw the select vector control.										
								
						left = rect.left + 105 + 3*kLegendScrollWidth;
						top  = rect.bottom - kSBarWidth;
						MoveControl (controlHandle, left, top);
						
						SizeControl (controlHandle, kBottomButtonWidth, kSBarWidth+1);
						break;
							
					case kGraphOverlayControl:
						
								// Draw overlay options control.										
								
						left = rect.left + 105 + 4*kLegendScrollWidth+7;
						top  = rect.bottom - kSBarWidth;
						MoveControl (controlHandle, left, top);
						
						SizeControl (controlHandle, kBottomButtonWidth, kSBarWidth+1);
						break;
							
					case kGraphBinWidthControl:
						
								// Draw bin width control.										
								
						left = rect.left + 105 + 5*kLegendScrollWidth+14;
						top  = rect.bottom - kSBarWidth;
						MoveControl (controlHandle, left, top);
						
						SizeControl (controlHandle, kBottomButtonWidth, kSBarWidth+1);
						break;
							
					case kGraphXAxisLabelControl:
						
								// Draw x-axis label control.
								
						left = (rect.left + rect.right)/2 - 115;
						left = MAX (0, left);
						top  = rect.bottom - kSBarWidth - 10;
						MoveControl (controlHandle, left, top);
						
						SizeControl (controlHandle, 230, kSBarWidth+1);
						break;
								
					default:
						validateFlag = FALSE;
						break;
							
					}	// end "switch (refCon)" 

				}	// end "for (index=numberControls; index>=1; index--)"
					
			}	// end "if (rootControlHandle != NULL)"

				//	Show window controls for histogram statistics graph windows.
	
		if (graphRecordPtr->processorCode == kHistogramStatsProcessor)
			{
			GetRootControl (windowPtr, &rootControlHandle);
			if (rootControlHandle != NULL)
				{																					
				CountSubControls (rootControlHandle, &numberControls);
				for (index=numberControls; index>=1; index--)
					{
					GetIndexedSubControl (rootControlHandle, index, &controlHandle);
					refCon = (SInt16)GetControlReference (controlHandle);
						
					if (numberSets > 1 || (refCon != kNextGraphSetControl && 
													refCon != kPreviousGraphSetControl))
						ShowControl (controlHandle);
							
					}	// end "while (controlHandle != NULL)"
						
				}	// end "if (rootControlHandle != NULL)"
			
			}	// end "if (graphRecordPtr->processorCode == kHistogramStatsProcessor)"
	     	
		SetPort (savedPort);
	#endif	// defined multispec_mac                    
		                                                         
	#if defined multispec_win                                                    
		//graphRecPtr->pDC->TextOut (left, 
		//										top, 
		//										tempString, 
		//										15);
	#endif	// defined multispec_win 

}	// end "DrawGraphControls" 

                           
                           
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
//	Revised By:			Larry L. Biehl			Date: 04/04/2019

void DrawGraphLabels (
				GraphPtr								graphRecPtr)
													
{                         
	Rect*									clientRectPtr;
	
	SInt16								graphWidth,
											textWidth,
											xPos,
											yPos;

	
	#if defined multispec_win
		USES_CONVERSION;
	#endif

	if (graphRecPtr->xLabel[0] > 0 && graphRecPtr->xLabel[0] < 32)
		{
		clientRectPtr = &graphRecPtr->clientRect;
		
		textWidth = TextWidth ((UCharPtr)&graphRecPtr->xLabel, 
										1, 
										graphRecPtr->xLabel[0]);
						
		graphWidth = (clientRectPtr->right - clientRectPtr->left) -
								graphRecPtr->rightInset - graphRecPtr->leftInset;
						
		xPos = clientRectPtr->left + graphRecPtr->leftInset + 
															graphWidth/2 - textWidth/2;
															
		xPos = MAX (xPos, clientRectPtr->left);
			                                                         
		#if defined multispec_mac
			yPos = clientRectPtr->bottom - graphRecPtr->bottomInset + 
												(SInt16)(3.5 * graphRecPtr->textSize) + 2;
			if (graphRecPtr->processorCode == kHistogramStatsProcessor)
				yPos -= 4;
			MoveTo (xPos, yPos);
			DrawText (&graphRecPtr->xLabel, 1, graphRecPtr->xLabel[0]);
		#endif	// defined multispec_mac                    
			                                                         
		#if defined multispec_win  
			yPos = clientRectPtr->bottom - graphRecPtr->bottomInset + 
												(SInt16)(3.5 * graphRecPtr->textSize) + 4;                                                
			graphRecPtr->pDC->TextOut (xPos, 
												yPos, 
												(LPCTSTR)A2T((char*)&graphRecPtr->xLabel[1]), 
												graphRecPtr->xLabel[0]);
		#endif	// defined multispec_win 

      #if defined multispec_wx
         yPos = clientRectPtr->bottom - graphRecPtr->bottomInset + 
												(SInt16)(3.5 * graphRecPtr->textSize);                                                

			wxString xLabelPtr = wxString::FromAscii (&graphRecPtr->xLabel[1]);
         if (graphRecPtr->processorCode == kHistogramStatsProcessor)
            graphRecPtr->pDC->DrawText (xLabelPtr, (int)xPos, (int)yPos-15);
         else
            graphRecPtr->pDC->DrawText (xLabelPtr, (int)xPos, (int)yPos);     
      #endif

		}	// end "if (graphRecPtr->xLabel[0] > 0 && ..."
	
	if (graphRecPtr->yLabel[0] > 0 && graphRecPtr->yLabel[0] < 32)
		{
		clientRectPtr = &graphRecPtr->clientRect;
		 
		textWidth = TextWidth (
						(UCharPtr)&graphRecPtr->yLabel, 1, graphRecPtr->yLabel[0]);
							
		xPos = clientRectPtr->left + 3 + kLeftEdgeSpace;
		
		#if defined multispec_wx			
			yPos = clientRectPtr->top + (SInt16)(1.8* graphRecPtr->textSize); 
		#endif
		
		#ifndef multispec_wx
			yPos = clientRectPtr->top + (SInt16)(2.7* graphRecPtr->textSize);
		#endif
      
		yPos = MIN (yPos, clientRectPtr->top + graphRecPtr->topInset);
			                                                         
		#if defined multispec_mac
			MoveTo (xPos, yPos);
			DrawText (&graphRecPtr->yLabel, 1, graphRecPtr->yLabel[0]);
		#endif	// defined multispec_mac                    
			                                                         
		#if defined multispec_win                                                    
			graphRecPtr->pDC->TextOut (xPos, 
												yPos, 
												(LPCTSTR)A2T((char*)&graphRecPtr->yLabel[1]), 
												graphRecPtr->yLabel[0]);
		#endif	// defined multispec_win 
		
      #if defined multispec_wx
         wxString yLabelPtr = wxString::FromAscii (&graphRecPtr->yLabel[1]);
         graphRecPtr->pDC->DrawText (yLabelPtr, xPos, yPos);
      #endif
		
		}	// end "if (graphRecPtr->yLabel[0] > 0 && ..."

}	// end "DrawGraphLabels"    

                           
                           
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
	

	#if defined multispec_win
		USES_CONVERSION;
	#endif
	
	if (graphRecPtr->flag == NU_HISTOGRAM_PLOT && graphRecPtr->numberVectors > 1)
		{
		clientRectPtr = &graphRecPtr->clientRect;
		
		savedStringPtr = (char*)GetHandlePointer (graphRecPtr->vectorLabelHandle,
																	kLock);
	
      
		vectorPaletteColorPtr = (HSInt32Ptr)GetHandlePointer (
												graphRecPtr->vectorPaletteColorHandle,
												kLock);
	
		vectorDisplayPtr = (char*)GetHandlePointer (
												graphRecPtr->vectorDisplayHandle,
												kLock);
		
				// Get the width of the longest vector name
		
		textWidth = 0;
		stringPtr = savedStringPtr;
		for (line=0; line<(UInt32)graphRecPtr->numberVectors; line++)
			{
			textWidth = MAX (textWidth,
										TextWidth ((UCharPtr)stringPtr, 
											1, 
											(SInt16)stringPtr[0]));
			
			stringPtr += 32;
											
			}	// end "for (index=0; index<graphRecPtr->..."
			
		stringPtr = savedStringPtr;
		
		xPos = clientRectPtr->right - graphRecPtr->rightInset - 10 - textWidth;
		xPos = MAX (xPos, clientRectPtr->left + graphRecPtr->leftInset);
		                                               
		#if defined multispec_mac
			yPos = (SInt16)(clientRectPtr->top + graphRecPtr->topInset +
																			1.3*graphRecPtr->textSize);
			lineHeight = (SInt16)(1.2 * graphRecPtr->textSize);
		#endif	// defined multispec_mac                    
					                                                         
		#if defined multispec_win  
			yPos = (SInt16)(clientRectPtr->top + graphRecPtr->topInset +
																			1.4*graphRecPtr->textSize);
			lineHeight = (SInt16)(1.5 * graphRecPtr->textSize); 
		#endif	// defined multispec_win  
		
      #if defined multispec_wx
         yPos = (SInt16)(clientRectPtr->top + graphRecPtr->topInset +
         																1.4*graphRecPtr->textSize);
			lineHeight = (SInt16)(1.5 * graphRecPtr->textSize); 
      #endif
		
		SetBackgroundColor (graphRecPtr->pDC, 255);
		
		for (line=0; line<(UInt32)graphRecPtr->numberVectors; line++)
			{	  
			if (vectorDisplayPtr [line] > 0)
				{                                                                
				#if defined multispec_mac
					ForeColor (vectorPaletteColorPtr[line]);
					MoveTo (xPos, yPos);
					DrawText (stringPtr, 1, (SInt16)stringPtr[0]);
				#endif	// defined multispec_mac                    
					                                                         
				#if defined multispec_win     
					graphRecPtr->pDC->SetTextColor (vectorPaletteColorPtr[line]);
					graphRecPtr->pDC->TextOut (xPos, 
														yPos, 
														(LPCTSTR)A2T(&stringPtr[1]), 
														stringPtr[0]);
				#endif	// defined multispec_win
				
            #if defined multispec_wx
               wxColour color;
               color.Set (vectorPaletteColorPtr[line]);
               graphRecPtr->pDC->SetTextForeground (color);
               graphRecPtr->pDC->DrawText ((CharPtr)&stringPtr[1], xPos, yPos);
            #endif
				
				yPos += lineHeight;
				
				}	// end "if (vectorDisplayPtr [line] > 0)"
				
			stringPtr += 32;
			
			}	// end "for (line=0; line<graphRecPtr->numberVectors; line++)"
		
				// Make sure that the pen color is set back to black.
				
		MForeColor (graphRecPtr->pDC, blackColor);
			
		CheckAndUnlockHandle (graphRecPtr->vectorPaletteColorHandle);
		CheckAndUnlockHandle (graphRecPtr->vectorLabelHandle);
		CheckAndUnlockHandle (graphRecPtr->vectorDisplayHandle);
		
		}	// end "if (graphRecPtr->flag == NU_HISTOGRAM_PLOT && ..."

}	// end "DrawGraphLegend"     


                            
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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

	
	#if defined multispec_win
		USES_CONVERSION;
	#endif
	
	if (graphRecPtr->title[0] > 0 && graphRecPtr->title[0] < 64)
		{                             
		clientRectPtr = &graphRecPtr->clientRect;
		
		textWidth = TextWidth ((UCharPtr)&graphRecPtr->title, 
										1, 
										graphRecPtr->title[0]);
						
		graphWidth = (clientRectPtr->right - clientRectPtr->left) -
											graphRecPtr->rightInset - graphRecPtr->leftInset;
						
		xPos = clientRectPtr->left + graphRecPtr->leftInset + 
															graphWidth/2 - textWidth/2;
													
		xPos = MAX (xPos, clientRectPtr->left);
		
		#ifdef multispec_wx
			yPos = clientRectPtr->top + (SInt16)(0.5 * graphRecPtr->textSize);
		#endif
		
		#ifndef multispec_wx
			yPos = clientRectPtr->top + (SInt16)(1.5 * graphRecPtr->textSize);
		#endif
		
		yPos = MIN (yPos, clientRectPtr->top + graphRecPtr->topInset); 
			                                                         
		#if defined multispec_mac
			MoveTo (xPos, yPos);
			DrawText (&graphRecPtr->title, 1, graphRecPtr->title[0]); 
		#endif	// defined multispec_mac                    
			                                                         
		#if defined multispec_win                                                    
			graphRecPtr->pDC->TextOut (xPos, 
												yPos, 
												(LPCTSTR)A2T((char*)&graphRecPtr->title[1]), 
												graphRecPtr->title[0]);
		#endif	// defined multispec_win

      #if defined multispec_wx
         graphRecPtr->pDC->DrawText ((CharPtr)&graphRecPtr->title[1], xPos, yPos);
      #endif
         
		
		if (graphRecPtr->title2[0] > 0 && graphRecPtr->title2[0] < 32)
			{
			textWidth = TextWidth ((UCharPtr)&graphRecPtr->title2, 
												1, 
												graphRecPtr->title2[0]);
							
			xPos = clientRectPtr->left + graphRecPtr->leftInset + 
																graphWidth/2 - textWidth/2;
														
			xPos = MAX (xPos, clientRectPtr->left);

			#ifdef multispec_wx
				yPos = clientRectPtr->top + (SInt16)(1.7 * graphRecPtr->textSize);
			#endif
		
			#ifndef multispec_wx
				yPos = clientRectPtr->top + (SInt16)(2.7 * graphRecPtr->textSize);
			#endif

			yPos = MIN (yPos, clientRectPtr->top + graphRecPtr->topInset);
			
			if (graphRecPtr->flag == NU_HISTOGRAM_PLOT)
				{                                                  
				#if defined multispec_mac
					MoveTo (xPos, yPos);
					DrawText (&graphRecPtr->title2, 1, graphRecPtr->title2[0]); 
				#endif	// defined multispec_mac                    
					                                                         
				#if defined multispec_win                                                    
					graphRecPtr->pDC->TextOut (xPos, 
														yPos, 
														(LPCTSTR)A2T((char*)&graphRecPtr->title2[1]), 
														graphRecPtr->title2[0]);
				#endif	// defined multispec_win 

            #if defined multispec_wx
               graphRecPtr->pDC->DrawText (
               								(CharPtr)&graphRecPtr->title2[1], xPos, yPos);
            #endif
				
				}	// end "if (graphRecPtr->flag == NU_HISTOGRAM_PLOT)"
				
			else	// graphRecPtr->flag != NU_HISTOGRAM_PLOT
				{                                                         
				#if defined multispec_mac
					MoveTo (xPos, yPos);
					DrawText (&graphRecPtr->title2, 1, 1);
					
					ForeColor (redColor);
					DrawText (&graphRecPtr->title2, 2, 8);
					
					ForeColor (greenColor);
					DrawText (&graphRecPtr->title2, 10, 11);
					
					ForeColor (blueColor);
					DrawText (&graphRecPtr->title2, 21, 8);
					
					ForeColor (blackColor);
					DrawText (&graphRecPtr->title2, 29, 1);
				#endif	// defined multispec_mac                    
					                                                         
				#if defined multispec_win  
					UINT	savedTextAlign;


					graphRecPtr->pDC->MoveTo (xPos, yPos);
					savedTextAlign = graphRecPtr->pDC->GetTextAlign (); 
					graphRecPtr->pDC->SetTextAlign (TA_BASELINE + TA_UPDATECP); 	
					graphRecPtr->pDC->SetTextColor (RGB (0, 0, 0));                                                    
					graphRecPtr->pDC->TextOut (xPos, 
														yPos, 
														(LPCTSTR)A2T((char*)&graphRecPtr->title2[1]), 
														1); 
														
					graphRecPtr->pDC->SetTextColor (RGB (255, 0, 0));                                                    
					graphRecPtr->pDC->TextOut (xPos, 
														yPos, 
														(LPCTSTR)A2T((char*)&graphRecPtr->title2[2]), 
														8);
														
					graphRecPtr->pDC->SetTextColor (RGB (0, 128, 0));                                                    
					graphRecPtr->pDC->TextOut (xPos, 
														yPos, 
														(LPCTSTR)A2T((char*)&graphRecPtr->title2[10]), 
														11); 
														
					graphRecPtr->pDC->SetTextColor (RGB (0, 0, 255));                                                    
					graphRecPtr->pDC->TextOut (xPos, 
														yPos, 
														(LPCTSTR)A2T((char*)&graphRecPtr->title2[21]), 
														8); 
														 
					graphRecPtr->pDC->SetTextColor (RGB (0, 0, 0));                                                    
					graphRecPtr->pDC->TextOut (xPos, 
														yPos, 
														(LPCTSTR)A2T((char*)&graphRecPtr->title2[29]), 
														1); 
					graphRecPtr->pDC->SetTextAlign (savedTextAlign);
				#endif	// defined multispec_win 

            #if defined multispec_wx 
               wxColour color;
					//graphRecPtr->pDC->MoveTo (xPos, yPos);
   
               //graphRecPtr->pDC->SetAxisOrientation (xPos, yPos);
					//graphRecPtr->pDC->SetTextAlign (TA_BASELINE + TA_UPDATECP); 	
               color.Set (RGB (0, 0, 0));
               graphRecPtr->pDC->SetTextForeground (color);
               wxString title2Ptr = wxString::FromAscii (&graphRecPtr->title2[1]);
               graphRecPtr->pDC->DrawText (title2Ptr[0], xPos, yPos);
					
               color.Set (RGB (255, 0, 0));
               graphRecPtr->pDC->SetTextForeground (color);
               graphRecPtr->pDC->DrawText (title2Ptr.SubString (1, 8), xPos + 5,
                                          yPos);
               
               color.Set (RGB (0, 128, 0));
               graphRecPtr->pDC->SetTextForeground (color);
               graphRecPtr->pDC->DrawText (title2Ptr.SubString (9, 18), xPos + (6*10),
                                          yPos);
               
               color.Set (RGB (0, 0, 255));
               graphRecPtr->pDC->SetTextForeground (color);
               graphRecPtr->pDC->DrawText (title2Ptr.SubString (19, 27), xPos + (6*22),
                                          yPos);
               color.Set (RGB (0, 0, 0));
               graphRecPtr->pDC->SetTextForeground (color);
               graphRecPtr->pDC->DrawText (title2Ptr[28], xPos + (6*33),
                                          yPos);
				#endif	// defined multispec_wx
				
				}	// end "else graphRecPtr->flag != NU_HISTOGRAM_PLOT"
			
			}	// end "if (graphRecPtr->title2[0] > 0 && ..." 
		
		}	// end "if (graphRecPtr->title[0] > 0 && ..." 

}	// end "DrawGraphTitle"       



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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

}	// end "ForceGraphicsCodeResourceLoad"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
			graphRecordHandle = ((CMGraphView*)windowPtr)->GetGraphRecordHandle ();
	#endif		// defined multispec_win
	
   #if defined multispec_wx
      if (windowPtr != NULL)                         	
			graphRecordHandle = ((CMGraphView*)windowPtr)->GetGraphRecordHandle ();
   #endif
		
	return (graphRecordHandle);

}	// end "GetGraphRecordHandle" 



Handle GetGraphRecordHandle (
				Handle								windowInfoHandle)

{
	Handle								graphRecordHandle = NULL; 
	
	
	#if defined multispec_mac 
		CMGraphView*						graphViewCPtr;
		WindowInfoPtr						windowInfoPtr;
		
		 				
		windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
																
		if (windowInfoPtr != NULL)
			{												
			if (windowInfoPtr->graphViewCPtr != NULL)
				{
				graphViewCPtr = windowInfoPtr->graphViewCPtr;
				
												return (graphViewCPtr->GetGraphRecordHandle ());
				
				}	// end "if (windowInfoPtr->graphViewCPtr != NULL)"
			
			}	// end "if (windowInfoPtr != NULL)"
	#endif	// defined multispec_mac 
	
	#if defined multispec_win 					
																
	#endif	// defined multispec_win
		
	return (graphRecordHandle);

}	// end "GetGraphRecordHandle" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
		*graphRecHandlePtr = graphViewCPtr->GetGraphRecordHandle ();
		
		if (*graphRecHandlePtr != NULL)
			{
			*handleStatusPtr = MHGetState (*graphRecHandlePtr);
				                   
			graphRecordPtr = (GraphPtr)GetHandlePointer (
																*graphRecHandlePtr, kLock, kMoveHi);
				
			}	// end "if (*graphRecHandlePtr != NULL)"
			
		}	// end "if (graphViewCPtr != NULL)" 
		
	return (graphRecordPtr);

}	// end "GetGraphRecordPtr" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
//	Revised By:			Larry L. Biehl			Date: 04/19/2019

void GetGraphLabels (
				GraphPtr								graphRecordPtr)
													
{
	char									*xLabelStringPtr,
											*yLabelStringPtr;
	
	Boolean								hasWavelengthValuesFlag;
	
	
	if (graphRecordPtr == NULL)
																							return;
	
	xLabelStringPtr = (char*)&graphRecordPtr->xLabel;
	yLabelStringPtr = (char*)&graphRecordPtr->yLabel;
	
	if (xLabelStringPtr != NULL)
		{
		hasWavelengthValuesFlag =
							(graphRecordPtr->descriptionCode & kBothReflectiveThermalData);
		
		switch (graphRecordPtr->processorCode)
			{
			case kListDataProcessor:
				#if defined multispec_wx	
					if (graphRecordPtr->imageViewCPtr != NULL)
						{
						if (hasWavelengthValuesFlag &&
								graphRecordPtr->graphViewCPtr->m_frame->m_comboXlabel != NULL)
							graphRecordPtr->graphViewCPtr->m_frame->m_comboXlabel->Show ();
							
						}	// end "if (graphRecordPtr->imageViewCPtr != NULL)"
				#endif	// defined multispec_wx
			
			case kSelectionGraphProcessor:
				if (hasWavelengthValuesFlag)
					{
							// Has combobox to replace text label
					xLabelStringPtr[0] = sprintf ((char*)&xLabelStringPtr[1],
																"");
					
					#if defined multispec_wx
						wxChoice* xAxisComboBoxPtr =
										(wxChoice*)graphRecordPtr->xAxisPopupControlHandle;
						if (xAxisComboBoxPtr != NULL)
							xAxisComboBoxPtr->Show ();
					#endif	// defined multispec_wx

					#if defined multispec_mac
						ShowControl (graphRecordPtr->xAxisPopupControlHandle);
					#endif	// defined multispec_mac
					
					#if defined multispec_win
						CMGraphView* graphViewCPtr = graphRecordPtr->graphViewCPtr;
						graphViewCPtr->GetDlgItem (IDC_xAxis)->ShowWindow (SW_SHOW);
					#endif	// defined multispec_win
				
					}	// end "if (hasWavelengthValuesFlag)"

				else	// !hasWavelengthValuesFlag
					{
					xLabelStringPtr[0] = sprintf ((char*)&xLabelStringPtr[1],
																"Channel Number");
					#if defined multispec_wx
						wxChoice* xAxisComboBoxPtr =
										(wxChoice*)graphRecordPtr->xAxisPopupControlHandle;
						if (xAxisComboBoxPtr != NULL)
							xAxisComboBoxPtr->Hide ();
					#endif	// defined multispec_wx

					#if defined multispec_mac
						HideControl (graphRecordPtr->xAxisPopupControlHandle);
					#endif	// defined multispec_mac
					
					#if defined multispec_win
						CMGraphView* graphViewCPtr = graphRecordPtr->graphViewCPtr;
						graphViewCPtr->GetDlgItem (IDC_xAxis)->ShowWindow (SW_HIDE);
					#endif	// defined multispec_win

					}	// end "else !hasWavelengthValuesFlag"
				break;
				
			case kBiPlotDataProcessor:
				if (gBiPlotDataSpecsPtr->featureTransformationFlag)
					xLabelStringPtr[0] = sprintf (
												(char*)&xLabelStringPtr[1],
												"Feature %d",
												(int)(gBiPlotDataSpecsPtr->axisFeaturePtr[0]+1));
									
				else	// !...->featureTransformationFlag 
					xLabelStringPtr[0] = sprintf (
												(char*)&xLabelStringPtr[1],
												"Channel %d",
												(int)(gBiPlotDataSpecsPtr->axisFeaturePtr[0]+1));
		
				graphRecordPtr->xESignificantDigits = 0; 
				graphRecordPtr->xFSignificantDigits = 0;  
				break;
				
			case kHistogramStatsProcessor:
				xLabelStringPtr[0] = sprintf ((char*)&xLabelStringPtr[1],
														"Data Value");
				break;
			
			}	// end "switch (graphRecordPtr->processorCode)" 
		
		}	// end "if (xLabelStringPtr != NULL)" 

	if (yLabelStringPtr != NULL)
		{
		switch (graphRecordPtr->processorCode)
			{
			case kListDataProcessor:
			case kSelectionGraphProcessor:
				yLabelStringPtr[0] = sprintf ((char*)&yLabelStringPtr[1],
														"Value");
				break;
				
			case kBiPlotDataProcessor:
				if (gBiPlotDataSpecsPtr->featureTransformationFlag)
					yLabelStringPtr[0] = sprintf (
												(char*)&yLabelStringPtr[1],
												"Feature %d",
												(int)(gBiPlotDataSpecsPtr->axisFeaturePtr[1]+1));
									
				else	// !...->featureTransformationFlag 
					yLabelStringPtr[0] = sprintf (
												(char*)&yLabelStringPtr[1],
												"Channel %d",
												(int)(gBiPlotDataSpecsPtr->axisFeaturePtr[1]+1));
														
				graphRecordPtr->yESignificantDigits = 0;
				graphRecordPtr->yFSignificantDigits = 0;
				break;
				
			case kHistogramStatsProcessor:
				yLabelStringPtr[0] = sprintf ((char*)&yLabelStringPtr[1],
														"Count");
				break;
			
			}	// end "switch (graphRecordPtr->processorCode)"
		
		}	// end "if (yLabelString != NULL)" 
		
}	// end "GetGraphLabels"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
// Called By:			CreateGraph
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
					sprintf ((char*)tempString,
										"Polygon ");
				
				if (columnEnd <= columnStart && lineEnd <= lineStart)
					{
					sprintf ((char*)&graphRecordPtr->title[1],
										"%sLine %d - Column %d Data Value",
										tempString,
										(int)lineStart,
										(int)columnStart);
										
					graphRecordPtr->title2[1] = 0;
					
					}	// end "if (columnEnd <= columnStart && ..." 
										
				else	// columnEnd > columnStart || lineEnd > lineStart)  
					{
					sprintf ((char*)&graphRecordPtr->title[1],
										"%sLines %d-%d, Columns %d-%d Data Values",
										tempString,
										(int)lineStart,
										(int)lineEnd,
										(int)columnStart,
										(int)columnEnd);
										
					sprintf ((char*)&graphRecordPtr->title2[1],
										"(Average, +-Std Dev, Min-Max)");
										
					}	// end "else columnEnd > columnStart || ..." 
										
				graphRecordPtr->title[0] =
											(UInt8)strlen ((char*)&graphRecordPtr->title[1]);
				graphRecordPtr->title2[0] =
											(UInt8)strlen ((char*)&graphRecordPtr->title2[1]);
				break;

			case kBiPlotDataProcessor:
				GetBiPlotGraphTitle (graphRecordPtr);
				break;

			case kHistogramStatsProcessor:
				SetStatHistogramGraphTitles (graphRecordPtr);
				break;
				
			}	// end "switch (graphRecordPtr->processorCode)"
			
		}	// end "if (graphRecordPtr != NULL)"
		
}	// end "GetGraphTitle" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
			
			}	// end "if (gColorQDflag)"
	#endif	// defined multispec_mac                      
	
	#if defined multispec_win 			                               
		CFrameWnd* pMainFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
		pMainFrame->GetClientRect ((tagRECT*)&rect);
		
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
		gNextGraphicsWindowStart.h	= MAX ((rect.right - *xSizePtr - 9), 5);
		gNextGraphicsWindowStart.v	= MAX ((rect.bottom - *ySizePtr - 9), 
														40); 
		gNextGraphicsWindowStart.v	= MAX ((gNextGraphicsWindowStart.v - 70), 
															40);
		
		gGraphicsWindowOffset = 10;
		
		}	// end "if (gNextGraphicsWindowStart.v < 40)" 
		                                                                  
	*xSizePtr = MIN (rect.right - 3 - gNextGraphicsWindowStart.h, *xSizePtr); 
						
	*ySizePtr = MIN (rect.bottom - 3 - gNextGraphicsWindowStart.v, *ySizePtr);

}	// end "GetGraphWindowLocation" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
      
		numberBinWidths = 2 / (x_scale * histogramBinWidth);
		numberBinWidths = ceil (numberBinWidths);
		
		binWidth = numberBinWidths * histogramBinWidth;
         		
		}	// end "if (graphRecordPtr->histogramBinCode <= 0)"
		
	else	// graphRecordPtr->histogramBinCode > 0
		binWidth = graphRecordPtr->histogramBinCode * histogramBinWidth;
	
	return (binWidth);

}	// end "GetUserHistogramBinWidth" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
			graphRecordHandle = GetGraphRecordHandle (gWindowList[index]);	
			graphRecordPtr = (GraphPtr)GetHandlePointer (graphRecordHandle);	
							
			if (graphRecordPtr != NULL &&
					graphRecordPtr->imageWindow == imageWindowPtr &&
						graphRecordPtr->processorCode == kSelectionGraphProcessor)
				{
				*minPtr = MIN (*minPtr, graphRecordPtr->yDataMin);
				*maxPtr = MAX (*maxPtr, graphRecordPtr->yDataMax);
				
				}	// end "if (...->imageWindow == imageWindowPtr && ...)"
															
			}	// end "for (index=gNumberOfIWindows ..."
			
		}	// end "if (gNumberOfGWindows > 0)" 
	
}	// end "GetLikeWindowMinMax" 		



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
		graphRecordHandle = graphViewCPtr->GetGraphRecordHandle ();		
		graphRecordPtr = (GraphPtr)GetHandlePointer (graphRecordHandle);
												
		if (graphRecordPtr != NULL)
			return (graphRecordPtr->imageWindow);
													
		}	// end "if (graphViewCPtr != NULL)"
		
	return (NULL);
	
}	// end "GetSelectionGraphImageWindow" 	
	


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
	#if defined multispec_mac
		InvalidateWindow (windowPtr, areaCode, FALSE);
	#endif	// defined multispec_mac
	         
	#if defined multispec_win
		((CMGraphView*)windowPtr)->Invalidate ();	 
	#endif	// defined multispec_win
	
	#if defined multispec_wx
      //((CMGraphView*)windowPtr)->Close (true);
      //(windowPtr->m_Canvas)->Refresh ();
      ((CMGraphView*)windowPtr)->m_frame->Refresh ();
	#endif

}	// end "InvalidateGraphWindow" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
											

	#if defined multispec_win
		USES_CONVERSION;
	#endif
											
			// Draw the graph set control prompt text.
	
	numChars = sprintf ((CharPtr)tempString, "Bin width: ");
	
	FormatR ((CharPtr)&tempString[numChars], 
					&graphRecordPtr->histogramBinWidth, 
					graphRecordPtr->xESignificantDigits, 
					graphRecordPtr->xFSignificantDigits,
					false, 
					&error);
	numChars = (SInt16)strlen ((CharPtr)tempString);
	textWidth = TextWidth ((UCharPtr)tempString, 0, numChars);
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
	#if defined multispec_mac
		ClipRect (&graphRecordPtr->clientRect); 
			
		left = graphRecordPtr->clientRect.left + 105 + 6*kLegendScrollWidth+25;
		top  = graphRecordPtr->clientRect.bottom - 4;  
		
		if (!drawInWindowFlag)
					// This is for a copy or printed page.
			left = graphRecordPtr->clientRect.left + 10;
		
		MoveTo (left, top);
		DrawText (tempString, 0, numChars);
	#endif	// defined multispec_win
			
	#if defined multispec_win
		UINT	savedTextAlign;

		left = 105 + 6*kLegendScrollWidth + 16;
		top  = graphRecordPtr->clientRect.bottom - 
													graphRecordPtr->clientRect.top - 6;	// -3
		if (!drawInWindowFlag)
					// This is for a copy or printed page.
			top -= 15;

		SetBackgroundColor (graphRecordPtr->pDC, 192);
		savedTextAlign = graphRecordPtr->pDC->GetTextAlign ();
		graphRecordPtr->pDC->SetTextAlign (TA_BASELINE);
		graphRecordPtr->pDC->SetTextColor (RGB (0,0,0));
		graphRecordPtr->pDC->TextOut (left, 
													top, 
													(LPCTSTR)A2T((char*)tempString), 
													numChars);
		graphRecordPtr->pDC->SetTextAlign (savedTextAlign);
	#endif	// defined multispec_win
   
	#if defined multispec_wx
		left = 105 + 6*kLegendScrollWidth + 25;
		top  = graphRecordPtr->clientRect.bottom - 
													graphRecordPtr->clientRect.top - 3;
		if (!drawInWindowFlag)
					// This is for a copy or printed page.
			top -= 15;

      wxColour color;
      color.Set (RGB (192,0,0));
      graphRecordPtr->pDC->SetTextBackground (color);
      color.Set (RGB (0,0,0));
      graphRecordPtr->pDC->SetTextForeground (color);
	#endif	// defined multispec_wx
	
}	// end "ListBinWidthValue" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
//	Revised By:			Larry L. Biehl			Date: 05/29/2020

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

	SInt16								processorCode;
	
	
	if (graphRecordPtr != NULL)
		{
				// Now initialize the graph vectors which indicate the number 	
				// of points in each graph vector, and the start location		
				// for x vector for each y vector.										
				// For the Response versus wavelength graphs, the x vector		
				// is the same for the possible 5 y vectors.	

		processorCode = graphRecordPtr->processorCode;
		
		numberVectors = graphRecordPtr->numberVectors;
		
				// Indicate which vectors are to be drawn.
		
		vectorDisplayPtr = (UInt8*)GetHandlePointer (
															graphRecordPtr->vectorDisplayHandle);
											
		if (processorCode == kBiPlotDataProcessor)
			{
			*vectorDisplayPtr = 1;
			
			for (index=1; index<numberVectors; index++)
				vectorDisplayPtr[index] = 0;
			
			classPtr = gBiPlotDataSpecsPtr->classPtr;
			for (index=0; index<(SInt32)gBiPlotDataSpecsPtr->numberClasses; index++)
				vectorDisplayPtr[classPtr[index]] = 1;
				
			}	// end "if (processorCode == kBiPlotDataProcessor)" 
			
		else	// processorCode != kBiPlotDataProcessor 
			{
			for (index=0; index<numberVectors; index++, vectorDisplayPtr++)
				*vectorDisplayPtr = 1;
			
			}	// end "else processorCode != kBiPlotDataProcessor" 
		
				// Load the symbols that are to be used.		
		
		vectorSymbolPtr = (UInt8*)GetHandlePointer (graphRecordPtr->vectorSymbolHandle);
		
		if (processorCode == kBiPlotDataProcessor)
			{
			*vectorSymbolPtr = '+';
			vectorSymbolPtr++;
		
			symbolsPtr = (UInt8*)GetHandlePointer (gBiPlotDataSpecsPtr->symbolsHandle); 
								
			for (index=1; index<numberVectors; index++, vectorSymbolPtr++)
				*vectorSymbolPtr = symbolsPtr[index];
				
			}	// end "if (processorCode == kBiPlotDataProcessor)" 
				
		else	// processorCode != kBiPlotDataProcessor 
			{
			for (index=0; index<numberVectors; index++, vectorSymbolPtr++)
				*vectorSymbolPtr = '+';
			
			}	// end "else processorCode != kBiPlotDataProcessor" 
			
			
				// Load the x vector offset to be used for each associated		
				// y-vector.  For the Response versus wavelength graphs, the 	
				// x-vector is the same for the possible 5 y vectors.	
		
		xVectorDataPtr = (SInt32*)GetHandlePointer (graphRecordPtr->xVectorDataHandle);
		
		indexEnd = graphRecordPtr->numberSets * numberVectors;
												
		for (index=0; index<indexEnd; index++, xVectorDataPtr++)
			*xVectorDataPtr = 0;
			
				// Load the vector label handle if needed. 
			
		if (graphRecordPtr->vectorLabelHandle != NULL)
			{
					// Get a copy of the channel numbers or feature numbers being
					// loaded in for the histogram graphs.
						
			Str31* stringPtr = (Str31*)GetHandlePointer (
																graphRecordPtr->vectorLabelHandle, 
																kLock);
						
			LoadListOfClassFieldNames (
							stringPtr,
							gProjectInfoPtr->statsWindowMode,
							(gProjectInfoPtr->histogramClassFieldCode == kHistogramClass));
			
			CheckAndUnlockHandle (graphRecordPtr->vectorLabelHandle);
			
			}	// end "else if (graphRecordPtr->vectorLabelHandle != NULL)"
			
				// Load the length for each vector.											
				// This value will be modified later for the biplot graphs and histogram
				// graphs.			
		
		vectorLengthInitialValue = vectorLength;
		if (processorCode == kBiPlotDataProcessor || 
											processorCode == kHistogramStatsProcessor)	 
			vectorLengthInitialValue = 0;
		
		vectorLengthsPtr = (SInt32*)GetHandlePointer (
														graphRecordPtr->vectorLengthsHandle);
													
		for (index=0; index<indexEnd; index++, vectorLengthsPtr++)
			*vectorLengthsPtr = vectorLengthInitialValue;
			
				// Initialize the colors to use for each vector.
		
		vectorPalettePtr = (SInt32*)GetHandlePointer (
														graphRecordPtr->vectorPaletteColorHandle);
		
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

					}	// end "for (index=1; index<numberVectors; index++)" 
					
				}	// end "if (...->processorCode == kBiPlotDataProcessor)"
				
			else	// graphRecordPtr->processorCode != kBiPlotDataProcessor
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
						
				}	// end "else graphRecordPtr->processorCode != kBiPlotDataProcessor"
			
			}	// end "if (graphRecordPtr->flag == NU_SCATTER_PLOT)" 
		
		else if (graphRecordPtr->flag == NU_HISTOGRAM_PLOT)
			{
			for (index=0; index<numberVectors; index++)
				{
				colorIndex = (index % 7);
			
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
						
				else if (colorIndex == 6)
					vectorPalettePtr[index] = blackColor;

				}	// end "for (index=0; index<numberVectors; index++)" 
			
			}	// end "else if (graphRecordPtr->flag == NU_HISTOGRAM_PLOT)" 
			
		else	// else Must be a response vs wavelength type of line graph
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
				
			}	// end "else graphRecordPtr->flag != NU_HISTOGRAM_PLOT || NU_SCATTER_PLOT"
			
				// Load the set list vector. 
			
		if (processorCode == kHistogramStatsProcessor)
			{
					// Get a copy of the channel numbers or feature numbers being
					// loaded in for the histogram graphs.
						
			setListPtr = (SInt16*)GetHandlePointer (graphRecordPtr->setListHandle);
						
			featurePtr = (SInt16*)GetHandlePointer (
															gStatHistogramSpecsPtr->featureHandle);
													
			numberSets = graphRecordPtr->numberSets;
			
			if (!gStatHistogramSpecsPtr->featureTransformationFlag)			
				for (index=0; index<numberSets; index++)
					setListPtr[index] = 
										gProjectInfoPtr->channelsPtr[featurePtr[index]] + 1;
										
			else	// gStatHistogramSpecsPtr->featureTransformationFlag		
				for (index=0; index<numberSets; index++)
					setListPtr[index] = featurePtr[index] + 1;

					// Load the class mean and standard deviation data.
					 
			if (!LoadClassMeanAndStdDevData (graphRecordPtr, 
														gProjectInfoPtr->statsWindowMode))
				graphRecordPtr->graphCodesAvailable = 1;
			
			}	// end "else if (processorCode == kHistogramStatsProcessor)"
		
		}	// end "if (graphRecordPtr != NULL)" 

}	// end "LoadGraphSupportArrays" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadGraphXVector
//
//	Software purpose:	This routine loads the data into the x vector for a graph window.
//							The routine will also find the min and max values for the x-axis
//							for channel number, overall wavelength, reflective wavelengths,
//							and thermal wavelengths.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			FinishGraphRecordSetUp in SGraphView.cpp
//							ShowGraphWindowSelection in SSelectionGraph.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/09/1991
//	Revised By:			Larry L. Biehl			Date: 07/11/2019

Boolean LoadGraphXVector (
				GraphPtr								graphRecordPtr,
				SInt16*								channelListPtr, 
				SInt32								channelListLength,
				SInt32								vectorLength,
				SInt16*								inputChannelWavelengthOrderPtr)
													
{
	float									*channelValuePtr,
											*channelWidthsPtr;
	
	double								value,
											widthValue;
											
	FileInfoPtr							fileInfoPtr,
											localFileInfoPtr = NULL;

	LayerInfoPtr						layerInfoPtr = NULL;
	
	//UInt16*								channelWavelengthOrderPtr;
	
	WindowInfoPtr						windowInfoPtr;
	
	Handle								channelValuesHandle,
											windowInfoHandle;
	
	SInt32								error = noErr, 
											//localIndexStart,
											wavelengthIndex,
											wavelengthWidthIndex;
	
	SInt16								channel,
											fileInfoIndex,
											index,
											localChannel;
	
	Boolean								continueFlag;
											//xDataMaxSetFlag,
											//xDataMinSetFlag;
	
   //SignedByte						handleStatus,
	//										windowHandleStatus;
	

	continueFlag = FALSE;
	if (graphRecordPtr != NULL)
		{
				// Initialize values for x vector.											
				// The channel numbers will be loaded first.  The channel values	
				// will be loaded second if they exist.									
		
		channelValuePtr = NULL;
		channelWidthsPtr = NULL;
		//channelWavelengthOrderPtr = NULL;
		channelValuesHandle = NULL;
		windowInfoHandle = GetWindowInfoHandle (graphRecordPtr->imageWindow);
		windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);

		Handle fileInfoHandle = GetFileInfoHandle (windowInfoHandle);
		fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
		
		if (fileInfoPtr != NULL &&
					windowInfoPtr != NULL &&
							windowInfoPtr->channelsInWavelengthOrderCode != kNotApplicable)
			channelValuesHandle = fileInfoPtr->channelValuesHandle;

		if (channelValuesHandle != NULL)
			{
			channelValuePtr = (float*)GetHandlePointer (channelValuesHandle, kLock);
			
			channelWidthsPtr = &channelValuePtr[fileInfoPtr->numberChannels];
			
			//channelWavelengthOrderPtr = (UInt16*)inputChannelWavelengthOrderPtr;
			//if (inputChannelWavelengthOrderPtr == NULL)
			//channelWavelengthOrderPtr =
			//							(UInt16*)&channelWidthsPtr[fileInfoPtr->numberChannels];
			
			char* xLabelStringPtr = (char*)&graphRecordPtr->xLabel;
			xLabelStringPtr[0] = sprintf ((char*)&xLabelStringPtr[1], "");

			}	// end "if (channelValuesHandle)" 
			
		graphRecordPtr->xVector.numberPoints = vectorLength;
		LockAndGetVectorPointer (&graphRecordPtr->xVector, &error);
		
		channel = 0;	
		for (index=0; index<channelListLength; index++)
			{		
			if (error != noErr)
				break;
				
			if (channelListPtr != NULL)
				channel = channelListPtr[index];

			channel++;
				
			SetV (&graphRecordPtr->xVector, index, channel, &error);
				
			}	// end "for (index=0; index<channelListLength; index++)"
			
		continueFlag = (error == noErr);
		
				// Load the channel values.

		layerInfoPtr = (LayerInfoPtr)GetHandlePointer (windowInfoPtr->layerInfoHandle);
		
		fileInfoIndex = -1;
		if (channelValuePtr != NULL && continueFlag)
			{
			channel = 0;	
			//wavelengthIndex = vectorLength;
			wavelengthWidthIndex = 2 * vectorLength;
			/*
			if (channelListLength < vectorLength)
				{
						// Fill wavelength vectors with default values of -100 to represent
						// no data for the channels not being used.
			
				wavelengthIndex = vectorLength;
				wavelengthWidthIndex = 2 * vectorLength;
				for (index=0; index<vectorLength; index++)
					{
					SetV (&graphRecordPtr->xVector, wavelengthIndex+index, -100, &error);
					if (error != noErr)
						break;
					
					SetV (&graphRecordPtr->xVector, wavelengthWidthIndex, -100, &error);
			
					if (error != noErr)
						break;
					
					}	// end "for (index=0; index<vectorLength; index++)"
				
				}	// end "if (channelListLength < vectorLength)"
			*/
			//localIndexStart = 0;
			localChannel = 0;
			for (index=0; index<channelListLength; index++)
				{		
				//if (error != noErr)
				//	break;

				if (channelListPtr != NULL)
					channel = channelListPtr[index];
				else	// chanelListPtr == NULL
					channel = index;
					
				channel++;
				localChannel++;
            
            if (fileInfoPtr != NULL && layerInfoPtr != NULL &&
							fileInfoIndex != (SInt16)layerInfoPtr[channel].fileInfoIndex)
               {
					//localIndexStart = index;
               fileInfoIndex = layerInfoPtr[channel].fileInfoIndex;
               localFileInfoPtr = &fileInfoPtr[fileInfoIndex];         
               if (localFileInfoPtr->channelValuesHandle != NULL) 
                  {
						channelValuePtr = (float*)GetHandlePointer (
												localFileInfoPtr->channelValuesHandle, kLock);
						channelWidthsPtr = &channelValuePtr[localFileInfoPtr->numberChannels];
						//channelWavelengthOrderPtr =
						//		(UInt16*)&channelWidthsPtr[localFileInfoPtr->numberChannels];
												
                  }	// end "if (fileInfoPtr != NULL && fileInfoPtr->...)" 
						
					localChannel = layerInfoPtr[channel].fileChannelNumber;
						
               }	// end "if (fileInfoPtr != NULL && ..." 
			
				value = channelValuePtr[localChannel-1];

				wavelengthIndex =
					//vectorLength + localIndexStart + channelWavelengthOrderPtr[localChannel-1];
					vectorLength + layerInfoPtr[channel].wavelengthIndex;
				SetV (&graphRecordPtr->xVector, wavelengthIndex, value, &error);	
			
				if (error != noErr)
					break;	
					
				widthValue = 0;
				if (*channelWidthsPtr > 0)
					widthValue = 0.5 * channelWidthsPtr[localChannel-1];
				
						// Load the channel band width
						
				wavelengthWidthIndex = wavelengthIndex + vectorLength;
				SetV (&graphRecordPtr->xVector, wavelengthWidthIndex, widthValue, &error);	
				
				if (error != noErr)
					break;

				}	// end "for (index=0; index<vectorLength; index++)"

			continueFlag = (error == noErr);
			
			}	// end "if (channelValuePtr && continueFlag)" 
		
		CheckAndUnlockHandle (channelValuesHandle);
		UnlockVectorPointer (&graphRecordPtr->xVector);
		
		UpdateSelectionGraphXAxisMinMax (graphRecordPtr);
		
		}	// end "if (graphRecordPtr)" 
		
	return (continueFlag);

}	// end "LoadGraphXVector" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadGraphYVector
//
//	Software purpose:	This routine loads the data into the y vector for a graph
//							window. The response values will be loaded in channel number
//							order in the first vector and when wavelength information is
//							available the reponse values will be loaded in wavelength
//							order. Most of the time this will be the same but there are
//							some image files where they are different.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			ListFieldData in SListData.cpp
//							ShowGraphWindowSelection in SSelectionGraph.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/04/1991
//	Revised By:			Larry L. Biehl			Date: 07/11/2018

void LoadGraphYVector (
				GraphPtr								graphRecordPtr, 
				double*								buffer8BytePtr,
				UInt32								vectorLength, 
				UInt16								inputIndex)
													
{ 
	double*								bufferReal8BytePtr;
	//float*								channelValuePtr;
	
	FileInfoPtr							fileInfoPtr;

	LayerInfoPtr						layerInfoPtr = NULL;
	
	//UInt16*								channelWavelengthOrderPtr;
	
	WindowInfoPtr						windowInfoPtr;
	
	Handle								//channelValuesHandle,
											fileInfoHandle,
											windowInfoHandle;
	
	SInt32								error;
	
	UInt32								channelIndex,
											index,
											indexStart;
	
	
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
			
		}	// end "for (index=0; index<vectorLength..."
	
			// Determine if the channel wavelengths are available and if so whether
			// they are in order. If they are not in order then the response values
			// need to be loaded in the vector in wavelength order
	
	windowInfoHandle = GetWindowInfoHandle (graphRecordPtr->imageWindow);
	windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);

	if (windowInfoPtr != NULL)
		layerInfoPtr = (LayerInfoPtr)GetHandlePointer (windowInfoPtr->layerInfoHandle);
		
	graphRecordPtr->channelsInWavelengthOrderFlag = TRUE;
	if (windowInfoPtr->channelsInWavelengthOrderCode == kNotInOrder &&
																				layerInfoPtr != NULL)
		{
		fileInfoHandle = GetFileInfoHandle (windowInfoHandle);
		fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
		//channelValuesHandle = fileInfoPtr->channelValuesHandle;

		//if (channelValuesHandle != NULL)
			//{
			//channelValuePtr = (float*)GetHandlePointer (channelValuesHandle, kLock);
			//channelWavelengthOrderPtr =
			//						(UInt16*)&channelValuePtr[2*fileInfoPtr->numberChannels];
		
			bufferReal8BytePtr = (double*)buffer8BytePtr;
			indexStart = graphRecordPtr->yVector.size/2;
			for (channelIndex=1; channelIndex<=vectorLength; channelIndex++)
				{
				//index = indexStart + channelWavelengthOrderPtr[bufferIndex];
				index = indexStart + layerInfoPtr[channelIndex].wavelengthIndex;
				SetV (&graphRecordPtr->yVector, index, *bufferReal8BytePtr, &error);
				bufferReal8BytePtr += inputIndex;
					
				}	// end "for (channelIndex=0; channelIndex<vectorLength; ..."
			
			graphRecordPtr->channelsInWavelengthOrderFlag = FALSE;
			
			//CheckAndUnlockHandle (channelValuesHandle);
			
			//}	// end "if (channelValuesHandle)"

		}	// end "if (windowInfoPtr->channelsInWavelengthOrderCode == kNotInOrder)"
			
	UnlockVectorPointer (&graphRecordPtr->yVector);

}	// end "LoadGraphYVector" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
// Called By:			ShowGraphWindowSelection in SSelectionGraph.cpp
//
//	Coded By:			Larry L. Biehl			Date: 07/27/1992
//	Revised By:			Larry L. Biehl			Date: 07/11/2018

void LoadGraphYVector2 (
				GraphPtr								graphRecordPtr, 
				ChannelStatisticsPtr				channelStatsPtr, 
				UInt32								vectorLength)
													
{
	ChannelStatisticsPtr				lChannelStatsPtr;
	//float*								channelValuePtr;
	
	FileInfoPtr							fileInfoPtr;

	LayerInfoPtr						layerInfoPtr = NULL;
	
	//UInt16*								channelWavelengthOrderPtr;
	
	WindowInfoPtr						windowInfoPtr;
	
	Handle								//channelValuesHandle,
											fileInfoHandle,
											windowInfoHandle;
	
	SInt32								error;
	
	UInt32								channelIndex,
											index,
											index2;
	
	graphRecordPtr->numberVectors = 5;
	graphRecordPtr->yVector.numberPoints = 
										vectorLength * graphRecordPtr->numberVectors;
	index2 = 0;
	
	LockAndGetVectorPointer (&graphRecordPtr->yVector, &error);
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
			
		}	// end "for (index=0; index<vectorLength..." 
	
			// Load the channel means + 1 standard deviation.						
			
	lChannelStatsPtr = channelStatsPtr;
	for (index=0; index<vectorLength; index++)
		{
		SetV (&graphRecordPtr->yVector, 
				index2, 
				lChannelStatsPtr->mean + lChannelStatsPtr->standardDev, 
				&error);
		index2++;
		lChannelStatsPtr++;
			
		}	// end "for (index=0; index<vectorLength..." 
	
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
			
		}	// end "for (index=0; index<vectorLength..." 
	
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
			
		}	// end "for (index=0; index<vectorLength..." 
	
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
			
		}	// end "for (index=0; index<vectorLength..." 
	
			// Determine if the channel wavelengths are available and if so whether
			// they are in order. If they are not in order then the response values
			// need to be loaded in a vector in wavelength order
	
	windowInfoHandle = GetWindowInfoHandle (graphRecordPtr->imageWindow);
	windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);

	if (windowInfoPtr != NULL)
		layerInfoPtr = (LayerInfoPtr)GetHandlePointer (windowInfoPtr->layerInfoHandle);
	
	graphRecordPtr->channelsInWavelengthOrderFlag = TRUE;
	if (windowInfoPtr->channelsInWavelengthOrderCode == kNotInOrder &&
																			layerInfoPtr != NULL)
		{
		fileInfoHandle = GetFileInfoHandle (windowInfoHandle);
		fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
		//channelValuesHandle = fileInfoPtr->channelValuesHandle;

		//if (channelValuesHandle != NULL)
			//{
			//channelValuePtr = (float*)GetHandlePointer (channelValuesHandle, kLock);
			//channelWavelengthOrderPtr =
			//						(UInt16*)&channelValuePtr[2*fileInfoPtr->numberChannels];
	
					// Load the channel means.
					
			lChannelStatsPtr = channelStatsPtr;
			index2 = graphRecordPtr->yVector.size/2;
			for (channelIndex=1; channelIndex<=vectorLength; channelIndex++)
				{
				//index = index2 + channelWavelengthOrderPtr[channelIndex];
				index = index2 + layerInfoPtr[channelIndex].wavelengthIndex;
				SetV (&graphRecordPtr->yVector, 
						index,
						lChannelStatsPtr->mean, 
						&error);
				lChannelStatsPtr++;
					
				}	// end "for (channelIndex=1; channelIndex<=vectorLength..."
			
					// Load the channel means + 1 standard deviation.						
					
			lChannelStatsPtr = channelStatsPtr;
			index2 += vectorLength;
			for (channelIndex=1; channelIndex<=vectorLength; channelIndex++)
				{
				//index = index2 + channelWavelengthOrderPtr[channelIndex];
				index = index2 + layerInfoPtr[channelIndex].wavelengthIndex;
				SetV (&graphRecordPtr->yVector, 
						index,
						lChannelStatsPtr->mean + lChannelStatsPtr->standardDev, 
						&error);
				lChannelStatsPtr++;
					
				}	// end "for (channelIndex=1; channelIndex<=vectorLength..."
			
					// Load the channel means - 1 standard deviation.						
					
			lChannelStatsPtr = channelStatsPtr;
			index2 += vectorLength;
			for (channelIndex=1; channelIndex<=vectorLength; channelIndex++)
				{
				//index = index2 + channelWavelengthOrderPtr[channelIndex];
				index = index2 + layerInfoPtr[channelIndex].wavelengthIndex;
				SetV (&graphRecordPtr->yVector, 
						index,
						lChannelStatsPtr->mean - lChannelStatsPtr->standardDev,  
						&error);
				lChannelStatsPtr++;
					
				}	// end "for (channelIndex=1; channelIndex<=vectorLength..."
			
					// Load the channel minimum values.											
					
			lChannelStatsPtr = channelStatsPtr;
			index2 += vectorLength;
			for (channelIndex=1; channelIndex<=vectorLength; channelIndex++)
				{
				//index = index2 + channelWavelengthOrderPtr[channelIndex];
				index = index2 + layerInfoPtr[channelIndex].wavelengthIndex;
				SetV (&graphRecordPtr->yVector, 
						index,
						lChannelStatsPtr->minimum, 
						&error);
				lChannelStatsPtr++;
					
				}	// end "for (channelIndex=1; channelIndex<=vectorLength..."
			
					// Load the channel maximum values.											
					
			lChannelStatsPtr = channelStatsPtr;
			index2 += vectorLength;
			for (channelIndex=1; channelIndex<=vectorLength; channelIndex++)
				{
				//index = index2 + channelWavelengthOrderPtr[channelIndex];
				index = index2 + layerInfoPtr[channelIndex].wavelengthIndex;
				SetV (&graphRecordPtr->yVector, 
						index,
						lChannelStatsPtr->maximum,  
						&error);
				lChannelStatsPtr++;
					
				}	// end "for (channelIndex=1; channelIndex<=vectorLength..."
			
			graphRecordPtr->channelsInWavelengthOrderFlag = FALSE;
			
			//CheckAndUnlockHandle (channelValuesHandle);
			
			//}	// end "if (channelValuesHandle)"

		}	// end "if (windowInfoPtr->channelsInWavelengthOrderCode == kNotInOrder)"
		
	UnlockVectorPointer (&graphRecordPtr->yVector);

}	// end "LoadGraphYVector2" 


                           
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
	#if defined multispec_mac
		MSetWindowTitle (graphViewCPtr->GetWindowPtr (), 
							&titleStringPtr[0]);
	#endif	// defined multispec_mac
			  
	#if defined multispec_win
		MSetWindowTitle ((CDocument*)graphViewCPtr->GetDocument (), 
							&titleStringPtr[0]);
	#endif	// defined multispec_win
	
	#if defined multispec_wx
      wxString ntitle =  wxString::FromUTF8 ((char*)&titleStringPtr[1]);
      (graphViewCPtr->m_frame)->SetTitle (ntitle);
	#endif

}	// end "MSetGraphWindowTitle"


                           
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ReloadXAxis
//
//	Software purpose:	This routine reloads the selection graph data to be plotted
//							to match the user request for channel number, wavelengh or
//							wavelength band width
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			CreateGraphWindow
//
//	Coded By:			Larry L. Biehl			Date: 03/02/2018
//	Revised By:			Larry L. Biehl			Date: 03/16/2018

void ReloadXAxis (
				GraphPtr								graphRecordPtr,
				int									xAxisSelection) 
				
{	
	UpdateSelectionGraphXAxisMinMax (graphRecordPtr);
   SetGraphMinMax (graphRecordPtr, kBothXYAxes);
	
}	// end "ReloadXAxis"


                           
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
//	Revised By:			Larry L. Biehl			Date: 04/06/2018

void SetGraphWindowTitle (
				CMGraphView*						graphViewCPtr,
				Boolean								newWindowFlag)
													
{
	UCharPtr								charPtr;  
	GraphPtr								graphRecordPtr;
	
	Handle								graphRecordHandle;

	SInt16								processorCode;
	

	if (graphViewCPtr == NULL)
																								return;
											
	graphRecordHandle = graphViewCPtr->GetGraphRecordHandle ();		
	graphRecordPtr = (GraphPtr)GetHandlePointer (graphRecordHandle);
	processorCode = graphRecordPtr->processorCode;

	if (processorCode == kSelectionGraphProcessor)
		{
		if (newWindowFlag)
			{
			gTextString[0] = 0;
			gTextString[1] = 0;
			
			ConcatPStrings ((UCharPtr)gTextString, (StringPtr)"\0Selection Graph\0", 254);
			
			}	// end "if (newWindowFlag)" 	
					
		else	// !newWindowFlag 
			{
			#ifndef multispec_wx
				GetWindowTitle (graphViewCPtr->GetWindowPtr (), (UCharPtr)gTextString2);
			#endif
			
			#ifdef multispec_wx
				GetGraphWindowTitle (graphViewCPtr->GetWindowPtr (), (UCharPtr)gTextString2);
			#endif

			charPtr = (UCharPtr)strchr ((char*)&gTextString2[1], ':');			
			
			if (charPtr == NULL ||
						graphRecordPtr == NULL ||
								(graphRecordPtr->drawGraphCode & 0x8000))
				sprintf ((char*)gTextString, 
							"Graph Window %hd",
							graphViewCPtr->GetGraphWindowNumber ());
						
			else	// charPtr != NULL && !(...->drawGraphCode & 0x8000)
				{
				sprintf ((char*)gTextString, 
							"Graph Window: %s",
							(char*)&charPtr[2]);
							
				}	// end "else charPtr != NULL"
							
			charPtr = CtoPstring (gTextString, gTextString);
			
			}	// end "else !newWindowFlag" 
		
		}	// end "if (processorCode == kSelectionGraphProcessor)" 
		
	else	// processorCode != kSelectionGraphProcessor 
		{
		Handle								fileInfoHandle,
												imageWindowInfoHandle;
		
		FileStringPtr						fileNamePtr;
		 
		SInt16								numberCharacters,
												stringLength;
												
		
		if (processorCode == kListDataProcessor)
			numberCharacters = sprintf ((char*)&gTextString[1], 
													"List Data Graph (");

		else if (processorCode == kBiPlotDataProcessor)
			numberCharacters = sprintf ((char*)&gTextString[1], 
													"Biplot Graph %hd (",
													gNumberOfGWindows);

		else	// processorCode == kHistogramStatsProcessor
			numberCharacters = sprintf ((char*)&gTextString[1], 
													"Histogram Graph %hd (",
													gNumberOfGWindows);
						
		charPtr = &gTextString[numberCharacters+1];
		stringLength = numberCharacters;
				
		gTextString2[0] = 0;
		fileNamePtr = &gTextString2[0];
									
		if (processorCode == kHistogramStatsProcessor)
			{
			gTextString3[0] = 0;
			strcpy ((char*)gTextString3, (char*)&gProjectInfoPtr->imageFileName[1]);
			fileNamePtr = (FileStringPtr)gTextString3;
			
			}	// end "if (processorCode == kHistogramStatsProcessor)"
			
		else if (gActiveImageWindow != NULL)
			{                                                                                                  
			imageWindowInfoHandle = GetActiveImageWindowInfoHandle (); 
			fileInfoHandle = GetFileInfoHandle (imageWindowInfoHandle);    
				
			if (fileInfoHandle != NULL)
				{                  
				WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																				imageWindowInfoHandle);
				SInt16 numberImageFiles = windowInfoPtr->numberImageFiles;
													                                                          
				if (numberImageFiles > 1)
					{		
					numberCharacters = sprintf ((char*)charPtr, "L%hd-)", numberImageFiles);
					charPtr = &charPtr[numberCharacters]; 
					stringLength += numberCharacters;
					
					}	// end "if (numberImageFiles > 1)" 
			
				FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
				fileNamePtr = (FileStringPtr)GetFileNameCPointerFromFileInfo (fileInfoPtr);
					
				}	// end "if (fileInfoHandle != NULL)" 
				
			}	// end "else if (gActiveImageWindow != NULL)" 
				
		else	// processorCode != kHistogramStatsProcessor && gActiveImageWindow == NULL 
			{
			if (gProjectInfoPtr != NULL)
				{
				gTextString3[0] = 0;
				strcpy ((char*)gTextString3, (char*)&gProjectInfoPtr->imageFileName[1]);
				fileNamePtr = gTextString3;
				
				}	// end "if (gProjectInfoPtr != NULL)"
				
			}	// end "else processorCode != kHistogramStatsProcessor && ..." 
				
		numberCharacters = sprintf ((char*)charPtr, "%s)", fileNamePtr);
		stringLength += numberCharacters;
			
		gTextString[0] = (UInt8)stringLength;
			
		}	// end "else processorCode != kSelectionGraphProcessor" 
				
			// Make certain that there is at least 500 bytes left for setting		
			//	a new title.																			

	if (CheckIfMemoryAvailable (500))
		{
		MSetGraphWindowTitle (graphViewCPtr, gTextString);

		}	// end "if (CheckIfMemoryAvailable (500))"

}	// end "SetGraphWindowTitle"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
				
		vectorLengthsPtr = (SInt32*)GetHandlePointer (
													graphRecordPtr->vectorLengthsHandle,
													kLock);
		
				// Get the graph offset vector. The offsets will be the same for the x
				// and y data for histogram type plots.
		
		vectorDataPtr = (SInt32*)GetHandlePointer (
													graphRecordPtr->xVectorDataHandle, 
													kLock);
	
				// Get the vector display vector. 
				
		vectorDisplayPtr = (char*)GetHandlePointer (
													graphRecordPtr->vectorDisplayHandle,
													kLock);
	
				// Get the histogram bin width vector in case it is needed. 
				
		histogramBinWidthPtr = (double*)GetHandlePointer (
													graphRecordPtr->histogramBinWidthHandle,
													kLock);
		
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
							
							//if (min > 0)
							//	min = MAX (0, min);
								
							max = min + fortyBinWidths;
							
							}	// end "if (max-min < fortyBinWidths)"
						
                  graphRecordPtr->histogramBinWidth = GetUserHistogramBinWidth (
																				graphRecordPtr,
																				histogramBinWidthPtr,
																				min,
																				max);
                  
																				
						graphRecordPtr->origHistogramBinWidth = 
														histogramBinWidthPtr[graphRecordPtr->set-1];
							
						}	// end "if (graphRecordPtr->flag & NU_HISTOGRAM_PLOT)"
						
					else if (graphRecordPtr->flag & NU_SCATTER_PLOT)	
						{
						FindMaxMinV (&graphRecordPtr->xDataMax, 
											&graphRecordPtr->xDataMin, 
											&graphRecordPtr->xVector, 
											&error);
					
						min = graphRecordPtr->xDataMin;
						max = graphRecordPtr->xDataMax;
							
						}	// end "else else if (graphRecordPtr->flag & NU_SCATTER_PLOT)"		
				
					else	// ...->flag != NU_HISTOGRAM_PLOT && != NU_SCATTER_PLOT
						{
						min = graphRecordPtr->xDataMin;
						max = graphRecordPtr->xDataMax;
						
						}	// end "else ...->flag != NU_HISTOGRAM_PLOT && ..."
					
					}	// end "if (graphRecordPtr->graphCodesAvailable & 0x0001)" 
																
				if (graphRecordPtr->graphCodesAvailable & 0x0002)
					{
					min = MIN (min, graphRecordPtr->xEllipseMin);
					max = MAX (max, graphRecordPtr->xEllipseMax);
					
					}	// end "if (graphRecordPtr->graphCodesAvailable & 0x0002)" 
				
				graphRecordPtr->xMin = min;
				graphRecordPtr->xMax = max;
	
						// Now set the scales for the x axis. This will be needed for
						// determining the scales for the y axis for histogram type
						// plots.
						
				SetXGraphScale (graphRecordPtr);
				
				}	// end "if (graphRecordPtr->graphCodesAvailable & 0x0003)"
				
			}	// end "if (axisCode & kXAxis)"
			
		if (axisCode & kYAxis)
			{
					// Now get the min and max values for the y-axis.
					
			min = DBL_MAX;
			max = -DBL_MAX; 
															
			if (!(graphRecordPtr->graphCodesAvailable & 0x0003))
				{
				min = 0;
				max = 0;
				
				}	// end "if (!(graphRecordPtr->graphCodesAvailable & 0x0003))"
				
			else	// (graphRecordPtr->graphCodesAvailable & 0x0003)
				{
				if (graphRecordPtr->graphCodesAvailable & 0x0001)
					{
					if (graphRecordPtr->flag & NU_HISTOGRAM_PLOT)
						{
						graphRecordPtr->yDataMin = 0;
						
						FindMaxBinV (&graphRecordPtr->yDataMax,
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
						
						}	// end "if (graphRecordPtr->flag & NU_HISTOGRAM_PLOT)"
						
					else	// !(graphRecordPtr->flag & NU_HISTOGRAM_PLOT)
						{
						FindMaxMinV (&graphRecordPtr->yDataMax, 
											&graphRecordPtr->yDataMin, 
											&graphRecordPtr->yVector, 
											&error);
						
						min = graphRecordPtr->yDataMin;
						max = graphRecordPtr->yDataMax;

						if (graphRecordPtr->processorCode == kSelectionGraphProcessor)
							GetLikeWindowMinMax (graphRecordPtr->imageWindow, &min, &max);
							
						}	// end "else !(graphRecordPtr->flag & NU_HISTOGRAM_PLOT)"
					
					}	// end "if (graphRecordPtr->graphCodesAvailable & 0x0001)"
																
				if (graphRecordPtr->graphCodesAvailable & 0x0002)
					{
					min = MIN (min, graphRecordPtr->yEllipseMin);
					max = MAX (max, graphRecordPtr->yEllipseMax);
					
					}	// end "if (graphRecordPtr->graphCodesAvailable & 0x0002)"
					
				}	// end "else (graphRecordPtr->graphCodesAvailable & 0x0003)"  
				
			graphRecordPtr->yMin = min;
			graphRecordPtr->yMax = max;
		
					// Set the min and max scales and tic intervals for the y-axis.			

			SetYGraphScale (graphRecordPtr);
			
			}	// end "if (axisCode & kYAxis)"
			
		CheckAndUnlockHandle (graphRecordPtr->histogramBinWidthHandle);
		CheckAndUnlockHandle (graphRecordPtr->vectorLengthsHandle);
		CheckAndUnlockHandle (graphRecordPtr->xVectorDataHandle);
		CheckAndUnlockHandle (graphRecordPtr->vectorDisplayHandle);
		
		}	// end "if (graphRecordPtr != NULL)"
	
}	// end "SetGraphMinMax" 

		

void SetGraphMinMax (
				Handle								windowInfoHandle,
				SInt16								axisCode)
				
{
	GraphPtr								graphRecordPtr;
	
	Handle								graphRecordHandle;
	

	graphRecordHandle = GetGraphRecordHandle (windowInfoHandle);
                   
	graphRecordPtr = (GraphPtr)GetHandlePointer (graphRecordHandle, kLock);
										
	SetGraphMinMax (graphRecordPtr, axisCode);
	
	CheckAndUnlockHandle (graphRecordHandle);
	
}	// end "SetGraphMinMax"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
				
	if (gNumberOfGWindows > 0)
		{
		for (index=gNumberOfIWindows+gNumberOfGWindows+2; 
				index>(UInt16)(gNumberOfIWindows+2); 
				index--)
			{	
			graphRecordHandle = GetGraphRecordHandle (gWindowList[index]);
			graphRecordPtr = (GraphPtr)GetHandlePointer (graphRecordHandle);	
							
			if (graphRecordPtr != NULL &&
					graphRecordPtr->imageWindow == imageWindowPtr &&
						graphRecordPtr->processorCode == kSelectionGraphProcessor)
				{
				graphRecordPtr->yScaleMin = min; 
				graphRecordPtr->yScaleMax = max;
				graphRecordPtr->yInt = interval;
				
				InvalidateGraphWindow (gWindowList[index], kGraphArea);
				
				}	// end "if (...->imageWindow == imageWindowPtr && ..."
															
			}	// end "for (index=gNumberOfIWindows ..."
			
		}	// end "if (gNumberOfGWindows > 0)" 
	
}	// end "SetLikeWindowMinMax" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
// Called By:			CreateGraph in SGraphUtilities.cpp
//							GraphWControlEvent in SGraphUtilities.cpp
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
	                
	graphRecordPtr = (GraphPtr)GetHandlePointer (graphRecordHandle);
										
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
			
		}	// end "if (graphRecordPtr != NULL && ...)"
		
	return (histogramBinCode != graphRecordPtr->histogramBinCode);
			
}	// end "SetHistogramBinWidth"   



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
// Called By:			CreateGraph in SGraphUtilities.cpp
//							GraphWControlEvent in SGraphUtilities.cpp
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
	                
	graphRecordPtr = (GraphPtr)GetHandlePointer (graphRecordHandle);
   
   
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
			
		}	// end "if (graphRecordPtr != NULL && ...)"
			
}	// end Routine "SetOverlayDisplayList"   



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
// Called By:			GetGraphTitle in SGraphiUtilities.cpp
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
		
		setListPtr = (SInt16*)GetHandlePointer (graphRecordPtr->setListHandle);
																
		if (setListPtr != NULL)
			{
			setValue = setListPtr[graphRecordPtr->set - 1];
			
					// Create title line 2.															
					
			if (graphRecordPtr->setCode == 1)
				sprintf ((char*)&graphRecordPtr->title2[1], 
								"Channel %hd",
								setValue);
							
			else	// graphRecordPtr->setCode == 2 
				sprintf ((char*)&graphRecordPtr->title2[1], 
								"Feature %hd",
								setValue);
								
			}	// end "if (setListPtr != NULL)" 
								
		graphRecordPtr->title2[0] = (UInt8)strlen ((char*)&graphRecordPtr->title2[1]);	
		
		}	// end "if (graphRecordPtr != NULL)" 

}	// end "SetStatHistogramGraphTitle2" 


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
	
	
	graphRecordHandle = graphViewCPtr->GetGraphRecordHandle ();
                   
	graphRecordPtr = (GraphPtr)GetHandleStatusAndPointer (
															graphRecordHandle, &handleStatus);
					
	histogramBinCode = graphRecordPtr->histogramBinCode;
												
	if (graphRecordPtr != NULL)
		{
		#if defined multispec_mac
			SInt32								index,
													numberMenuItems;
												
					// Now delete the current menu items.
					
			numberMenuItems = CountMenuItems	(popUpMenuHandle);						
			for (index=0; index<numberMenuItems; index++)			
				DeleteMenuItem (popUpMenuHandle, 1);
			
					// Add the items to the popup menu
			
	 		AppendMenu (popUpMenuHandle, "\pAuto");
			CheckMenuItem (popUpMenuHandle, 1, (histogramBinCode<=0));
			
	 		AppendMenu (popUpMenuHandle, "\px1");
			CheckMenuItem (popUpMenuHandle, 2, (histogramBinCode==1));
			
	 		AppendMenu (popUpMenuHandle, "\px2");
			CheckMenuItem (popUpMenuHandle, 3, (histogramBinCode==2));
			
	 		AppendMenu (popUpMenuHandle, "\px3");
			CheckMenuItem (popUpMenuHandle, 4, (histogramBinCode==3));
			
	 		AppendMenu (popUpMenuHandle, "\px4");
			CheckMenuItem (popUpMenuHandle, 5, (histogramBinCode==4));
			
	 		AppendMenu (popUpMenuHandle, "\px5");
			CheckMenuItem (popUpMenuHandle, 6, (histogramBinCode==5));
			
	 		AppendMenu (popUpMenuHandle, "\px10");
			CheckMenuItem (popUpMenuHandle, 7, (histogramBinCode==10));
			
	 		AppendMenu (popUpMenuHandle, "\px20");
			CheckMenuItem (popUpMenuHandle, 8, (histogramBinCode==20));
			
	 		AppendMenu (popUpMenuHandle, "\px30");
			CheckMenuItem (popUpMenuHandle, 9, (histogramBinCode==30));
			
	 		AppendMenu (popUpMenuHandle, "\px50");
			CheckMenuItem (popUpMenuHandle, 10, (histogramBinCode==50));
		#endif	// defined multispec_mac
		
		#if defined multispec_win
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
		#endif	// defined multispec_win
			
		}	// end "if (graphRecordPtr != NULL)"
			
	MHSetState (graphRecordHandle, handleStatus);
			
}	// end Routine "SetUpBinWidthPopUpMenu"   


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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

	#if defined multispec_win
		CMenu*								graphOverlayPopupMenuPtr;
	#endif	// defined multispec_win
	
	
	graphRecordHandle = graphViewCPtr->GetGraphRecordHandle ();
                   
	graphRecordPtr = (GraphPtr)GetHandleStatusAndPointer (
												graphRecordHandle, &handleStatus);
					
	drawGraphCode = (graphRecordPtr->drawGraphCode & 0x0005);
												
	if (graphRecordPtr != NULL)
		{
		#if defined multispec_mac
			SInt32								index,
													numberMenuItems;
													
												
					// Now delete the current menu items.
					
			numberMenuItems = CountMenuItems	(popUpMenuHandle);						
			for (index=0; index<numberMenuItems; index++)			
				DeleteMenuItem (popUpMenuHandle, 1);
			
					// Add the items to the popup menu
			
	 		AppendMenu (popUpMenuHandle, "\pDraw histogram");
			CheckMenuItem (popUpMenuHandle, 1, (drawGraphCode==1));
			
	 		AppendMenu (popUpMenuHandle, "\pDraw density function");
			CheckMenuItem (popUpMenuHandle, 2, (drawGraphCode==4));
			
	 		AppendMenu (popUpMenuHandle, "\pDraw histogram & density function");
			CheckMenuItem (popUpMenuHandle, 3, (drawGraphCode==5));
		#endif	// defined multispec_mac
		
		#if defined multispec_win
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
		#endif	// defined multispec_win
			
		}	// end "if (graphRecordPtr != NULL)"
			
	MHSetState (graphRecordHandle, handleStatus);
			
}	// end Routine "SetUpOverlayPopUpMenu"   


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
//	Revised By:			Larry L. Biehl			Date: 04/09/2018

void SetUpVectorPopUpMenu (
				CMGraphView*						graphViewCPtr,
				MenuHandle							popUpMenuHandle)

{  
	GraphPtr								graphRecordPtr;
	
	Handle								graphRecordHandle;
	
	SignedByte							handleStatus;

	#if defined multispec_win
		CMenu*								selectVectorsPopupMenuPtr;
		UInt32								popupMenuIndex;
		UINT									checkCode;

		USES_CONVERSION;
	#endif	// defined multispec_win
	
	
	graphRecordHandle = graphViewCPtr->GetGraphRecordHandle ();
                   
	graphRecordPtr = (GraphPtr)GetHandleStatusAndPointer (
															graphRecordHandle, &handleStatus);
												
	if (graphRecordPtr->vectorLabelHandle != NULL)
		{
		UCharPtr								stringPtr,
												vectorDisplayPtr;
		 
		SInt32								index;
													
												
					// Now delete the current menu items.

		#if defined multispec_mac
			SInt32								numberMenuItems;

			numberMenuItems = CountMenuItems	(popUpMenuHandle);						
			for (index=0; index<numberMenuItems; index++)			
				DeleteMenuItem (popUpMenuHandle, 1);
		#endif	// defined multispec_mac
		
		#if defined multispec_win
			selectVectorsPopupMenuPtr = (CMenu*)popUpMenuHandle;
			ClearMenuItems (selectVectorsPopupMenuPtr, 0);
			popupMenuIndex = ID_SELECTVECTORMENUITEMSTART;
		#endif	// defined multispec_win

		stringPtr = (unsigned char*)GetHandlePointer (
															graphRecordPtr->vectorLabelHandle,
															kLock);
		
		vectorDisplayPtr = (UInt8*)GetHandlePointer (
															graphRecordPtr->vectorDisplayHandle);
			
				// Add the vector labels to the popup menu
		
		for (index=0; index<graphRecordPtr->numberVectors; index++)
			{
			#if defined multispec_mac
	 			AppendMenu (popUpMenuHandle, (UCharPtr)stringPtr);
				CheckMenuItem (popUpMenuHandle, index+1, vectorDisplayPtr[index]);
			#endif	// defined multispec_mac
		
			#if defined multispec_win
				selectVectorsPopupMenuPtr->AppendMenu (MF_STRING, 
																	popupMenuIndex+index,
																	(LPCTSTR)A2T((char*)&stringPtr[1]));

				checkCode = MF_BYPOSITION | MF_UNCHECKED;
				if (vectorDisplayPtr[index])
					checkCode = MF_BYPOSITION | MF_CHECKED;
				selectVectorsPopupMenuPtr->CheckMenuItem (index, checkCode);
			#endif	// defined multispec_win
			
			stringPtr += 32;
											
			}	// end "for (index=0; index<graphRecPtr->..."
				
		#if defined multispec_mac
			AppendMenu (popUpMenuHandle, "\pAll");
			AppendMenu (popUpMenuHandle, "\pNone");
		#endif	// defined multispec_mac
		
		#if defined multispec_win
			selectVectorsPopupMenuPtr->AppendMenu (MF_STRING,
																popupMenuIndex+index, 
																(LPCTSTR)_T("All"));
			selectVectorsPopupMenuPtr->AppendMenu (MF_STRING,
																popupMenuIndex+index+1, 
																(LPCTSTR)_T("None"));
		#endif	// defined multispec_win

		CheckAndUnlockHandle (graphRecordPtr->vectorDisplayHandle);
		CheckAndUnlockHandle (graphRecordPtr->vectorLabelHandle);
			
		}	// end "if (graphRecordPtr->vectorLabelHandle != NULL)"
			
	MHSetState (graphRecordHandle, handleStatus);
			
}	// end Routine "SetUpVectorPopUpMenu"   


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 SetUpXAxisPopUpMenu
//
//	Software purpose:	The purpose of this routine is to set up the x-axis label popup
//							menu.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	The default popup menu selection
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/02/2018
//	Revised By:			Larry L. Biehl			Date: 08/29/2020

SInt16 SetUpXAxisPopUpMenu (
				GraphPtr								graphRecordPtr,
				MenuHandle							popUpMenuHandle)

{
	char									xAxisLabel [256],
											wavelengthUnits[16];
	
	int									length,
											newSelection = kChannels;

	SInt32								index,
											numberMenuItems;
													
	SInt16								descriptionCode = kChannels,
											xAxisCode;

	Boolean								useDisabledMenuItemFlag = TRUE;

	#if defined multispec_wx
		wxChoice*							xLabelComboBoxPtr = (wxChoice*)popUpMenuHandle;

				// Note that choice menu items in wxWidgets cannot be disabled.
				
		useDisabledMenuItemFlag	= FALSE;
	
		//int									currentSelection;
	
		//currentSelection = xLabelComboBoxPtr->GetSelection ();
		//xAxisCode = (SInt64)((int*)xLabelComboBoxPtr->GetClientData (currentSelection));
	#endif

	#if defined multispec_win
		CComboBox*							xLabelComboBoxPtr = (CComboBox*)popUpMenuHandle;
		
		
		useDisabledMenuItemFlag	= FALSE;
	#endif

	if (graphRecordPtr != NULL && popUpMenuHandle != NULL)
		{
		//#if defined multispec_mac || defined multispec_win
			descriptionCode = graphRecordPtr->descriptionCode;
		
					// Get the units string
		
			sprintf (wavelengthUnits, "um)");
			if (graphRecordPtr->channelDescriptionUnitString[0] != 0)
				{
				length = snprintf (wavelengthUnits,
										14,
										"%s",
										graphRecordPtr->channelDescriptionUnitString);
				length = sprintf (&wavelengthUnits[length],
										")");
			
				}	// end "if (fgraphRecordPtr->channelDescriptionUnitString[0] != 0)"

			xAxisCode = graphRecordPtr->xAxisCode;
		
					// Now delete the current menu items except for the first one.
					
			numberMenuItems = CountMenuItems	(popUpMenuHandle);
			for (index=numberMenuItems; index>1; index--)
				DeleteMenuItem (popUpMenuHandle, index);

			#if defined multispec_wx
				xLabelComboBoxPtr->SetClientData (0, (void*)kChannels);
			#endif

			#if defined multispec_win
				xLabelComboBoxPtr->SetItemData (0, kChannels);
			#endif

					// Add and enable those items that make sense for the data set.
		
			index = 1;
			newSelection = kChannels;
			if (descriptionCode & kBothReflectiveThermalData)
				{
						// Either Reflective and/or thermal wavelength data exists.											
						// An AppendMenu and then SetMenuItemText is used so that no
						// characters will be interpreted as "metacharacters" by AppendMenu.
				
				length = sprintf (&xAxisLabel[1], "Wavelength (%s", wavelengthUnits);
				xAxisLabel[0] = length;
         
				MAppendMenuItemText ((MenuRef)popUpMenuHandle, (UInt8*)xAxisLabel);
   
            #if defined multispec_wx
					xLabelComboBoxPtr->SetClientData (index, (void*)kWavelengths);
				#endif
				#if defined multispec_win
					xLabelComboBoxPtr->SetItemData (index, kWavelengths);
				#endif
				index++;
				if (xAxisCode == kWavelengths ||
								xAxisCode == kReflectiveWavelengths ||
											xAxisCode == kThermalWavelengths)
					newSelection = kWavelengths;
				
				if (descriptionCode & kBandWidthInfoExists ||
						(descriptionCode & kReflectiveData && descriptionCode & kThermalData))
					{
					length = sprintf (&xAxisLabel[1],
												"Wavelength-bandwidths (%s",
												wavelengthUnits);
					xAxisLabel[0] = length;
					if (useDisabledMenuItemFlag || (descriptionCode & kBandWidthInfoExists))
						{
						MAppendMenuItemText ((MenuRef)popUpMenuHandle, (UInt8*)xAxisLabel);
      
						#if defined multispec_wx
							xLabelComboBoxPtr->SetClientData (index, (void*)kBandWidths);
						#endif
						#if defined multispec_win
							xLabelComboBoxPtr->SetItemData (index, kBandWidths);
						#endif
						index++;

						}	// end "if (useDisabledMenuItemFlag || ..."
					
					if (descriptionCode & kBandWidthInfoExists)
						{
						if (xAxisCode == kBandWidths)
							newSelection = kBandWidths;
					
						}	// end "if (descriptionCode & kBandWidthInfoExists)"
				
					if (descriptionCode & kReflectiveData && descriptionCode & kThermalData)
						{
						if (useDisabledMenuItemFlag &&
														!(descriptionCode & kBandWidthInfoExists))
							DisableMenuItem (popUpMenuHandle, kBandWidths-1);
						
								// Both Reflective and Thermal data exists, allow the ability
								// for the user to select one or the other.
						
						length = sprintf (&xAxisLabel[1],
												"Wavelength-reflective (%s",
												wavelengthUnits);
						xAxisLabel[0] = length;
             
						MAppendMenuItemText ((MenuRef)popUpMenuHandle, (UInt8*)xAxisLabel);
         
                  #if defined multispec_wx
							xLabelComboBoxPtr->SetClientData (
																index, (void*)kReflectiveWavelengths);
						#endif
						#if defined multispec_win
							xLabelComboBoxPtr->SetItemData (index, kReflectiveWavelengths);
						#endif
						index++;

						if (xAxisCode == kReflectiveWavelengths)
							newSelection = kReflectiveWavelengths;
						
						length = sprintf (&xAxisLabel[1],
												"Wavelength-reflective bandwidths (%s",
												wavelengthUnits);
						xAxisLabel[0] = length;
						
						if (useDisabledMenuItemFlag ||
														(descriptionCode & kBandWidthInfoExists))
							{
							MAppendMenuItemText ((MenuRef)popUpMenuHandle, (UInt8*)xAxisLabel);
                 
                     #if defined multispec_wx
								xLabelComboBoxPtr->SetClientData (
																index, (void*)kReflectiveBandWidths);
							#endif
							#if defined multispec_win
								xLabelComboBoxPtr->SetItemData (index, kReflectiveBandWidths);
							#endif
							index++;

							}	// end "if (useDisabledMenuItemFlag || ..."
						
						if (descriptionCode & kBandWidthInfoExists)
							{
							if (xAxisCode == kReflectiveBandWidths)
								newSelection = kReflectiveBandWidths;
							
							}	// end "if (descriptionCode & kBandWidthInfoExists)"

						#if defined multispec_win
							else if (useDisabledMenuItemFlag) // bandWidth info does not exist
								DisableMenuItem (popUpMenuHandle, kReflectiveBandWidths-1);
						#endif
										
						length = sprintf (&xAxisLabel[1],
												"Wavelength-thermal (%s",
												wavelengthUnits);
						xAxisLabel[0] = length;
                
						MAppendMenuItemText ((MenuRef)popUpMenuHandle, (UInt8*)xAxisLabel);
        
                  #if defined multispec_wx
							xLabelComboBoxPtr->SetClientData (
																	index, (void*)kThermalWavelengths);
						#endif
						#if defined multispec_win
							xLabelComboBoxPtr->SetItemData (index, kThermalWavelengths);
						#endif
						index++;

						if (xAxisCode == kThermalWavelengths)
							newSelection = kThermalWavelengths;
						
						length = sprintf (&xAxisLabel[1],
												"Wavelength-thermal bandwidths (%s",
												wavelengthUnits);
						xAxisLabel[0] = length;

						if (useDisabledMenuItemFlag ||
															(descriptionCode & kBandWidthInfoExists))
							{
							MAppendMenuItemText (
													(MenuRef)popUpMenuHandle, (UInt8*)xAxisLabel);
                
                     #if defined multispec_wx
								xLabelComboBoxPtr->SetClientData (
																	index, (void*)kThermalBandWidths);
							#endif
							#if defined multispec_win
								xLabelComboBoxPtr->SetItemData (index, kThermalBandWidths);
							#endif
							index++;

							}	// end "if (useDisabledMenuItemFlag || ..."
						
						if (descriptionCode & kBandWidthInfoExists)
							{
							if (xAxisCode == kThermalBandWidths)
								newSelection = kThermalBandWidths;
							
							}	// end "if (descriptionCode & kBandWidthInfoExists)"
					
						else if (useDisabledMenuItemFlag)	// bandWidth info does not exist
							DisableMenuItem (popUpMenuHandle, kThermalBandWidths-1);
								
						}	// end "if (descriptionCode & kReflectiveData && ...)"
					
					else	// Either reflective or thermal data exists not both
						{
								// Update new selection to be wavelength or bandwidth if the
								// current selection is for reflective or thermal of the same.
								
						if (xAxisCode > kBandWidths)
							{
							newSelection = kWavelengths;
							if ((xAxisCode == kReflectiveBandWidths || 
												xAxisCode == kThermalBandWidths) &&
															descriptionCode & kBandWidthInfoExists)
								newSelection = kBandWidths;
								
							}	// end "if (xAxisCode > kBandWidths)"
						
						}	// end "else Either reflective or thermal data exists not both"
					
					}	// end "if (descriptionCode & kBandWidthInfoExists || ..."	
			
				}	// end "if (descriptionCode & kBothReflectiveThermalData)"
		
					// Change graph record settings if the default graph type had to
					// change because of a different set of channels.
		
			if (xAxisCode != newSelection)
				{
				#if defined multispec_wx
					xLabelComboBoxPtr->SetSelection (newSelection-1);
				#endif

				#if defined multispec_win
					xLabelComboBoxPtr->SetCurSel (newSelection-1);
				#endif

				graphRecordPtr->xAxisCode = newSelection;
				
				if (newSelection == kChannels)
					graphRecordPtr->xValueTypeCode = kIntegerType;
					
				else	// selection != kChannels
					graphRecordPtr->xValueTypeCode = kRealType;
				
				}	// end "if (xAxisCode != newSelection)"
 
			#if defined multispec_wx
				xLabelComboBoxPtr->SetSelection (newSelection-1);
			#endif

			#if defined multispec_mac
				CheckMenuItem (popUpMenuHandle, kChannels, (newSelection<=kChannels));
				CheckMenuItem (popUpMenuHandle, newSelection, TRUE);
			#endif
 
			#if defined multispec_win
				xLabelComboBoxPtr->SetCurSel (newSelection-1);
			#endif

		//#endif	// defined multispec_mac || defined multispec_win
		}	// end "if (graphRecordPtr != NULL)"
					
	#if defined multispec_wx || defined multispec_win
				// Make the return value 0 based.
		newSelection--;
	#endif

	return (newSelection);
			
}	// end Routine "SetUpXAxisPopUpMenu"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
// Called By:			CreateGraph in SGraphUtilities.cpp
//							GraphWControlEvent in SGraphUtilities.cpp
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
	                
	graphRecordPtr = (GraphPtr)GetHandlePointer (graphRecordHandle);
										
	numberVectors = graphRecordPtr->numberVectors;
		
			// Indicate which vectors are to be drawn.
	
	vectorDisplayPtr = (UCharPtr)GetHandlePointer (
															graphRecordPtr->vectorDisplayHandle);
   
	#if defined multispec_wx
		if ((UInt32)vectorSelection == numberVectors+1)
			{
					// Make sure that all vectors are selected.
					
			for (index=0; index<numberVectors; index++, vectorDisplayPtr++)
				*vectorDisplayPtr = 1;
			
			}	// end "if (vectorSelection == 1)"
		if ((UInt32)vectorSelection == numberVectors+2)		
			{
					// Make sure that all vectors are selected.
					
			for (index=0; index<numberVectors; index++, vectorDisplayPtr++)
				*vectorDisplayPtr = 0;
			
			}	// end "if (vectorSelection == 1)"
		
		if (vectorSelection <=numberVectors)
			{
					// Toggle the vector selection.
					
			vectorDisplayPtr[vectorSelection] = 1 - vectorDisplayPtr[vectorSelection];
			
			}	// end "else if (vectorSelection >= 0)"
	#endif

	#ifndef multispec_wx
		if ((UInt32)vectorSelection >= numberVectors+1)
			{
					// Make sure that all vectors are selected.
					
			for (index=0; index<numberVectors; index++, vectorDisplayPtr++)
				*vectorDisplayPtr = 0;
			
			}	// end "if (vectorSelection == 1)"
													
		else if ((UInt32)vectorSelection >= numberVectors)		
			{
					// Make sure that all vectors are selected.
				
			for (index=0; index<numberVectors; index++, vectorDisplayPtr++)
				*vectorDisplayPtr = 1;
			
			}	// end "if (vectorSelection == 1)"
			
		else if (vectorSelection >= 0)
			{
					// Toggle the vector selection.
					
			vectorDisplayPtr[vectorSelection] = 1 - vectorDisplayPtr[vectorSelection];
			
			}	// end "else if (vectorSelection >= 0)"
	#endif
			
}	// end "SetVectorDisplayList"   


                           
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetXAxisDescriptionInfo
//
//	Software purpose:	This routine sets the x-axis description information to match
//							that for the input image being used.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			CreateGraphWindow
//
//	Coded By:			Larry L. Biehl			Date: 03/16/2018
//	Revised By:			Larry L. Biehl			Date: 04/06/2018

void SetXAxisDescriptionInfo (
				GraphPtr								graphRecordPtr,
				WindowInfoPtr						windowInfoPtr,
				FileInfoPtr							fileInfoPtr)
				
{
	if (graphRecordPtr != NULL)
		{
		graphRecordPtr->descriptionCode = 0;
		graphRecordPtr->channelDescriptionUnitString[0] = 0;
		if (windowInfoPtr != NULL)
			{
			strcpy ((char*)graphRecordPtr->channelDescriptionUnitString,
							(char*)fileInfoPtr->channelDescriptionUnitString);
			graphRecordPtr->descriptionCode = windowInfoPtr->descriptionCode;
			
			}	// end "if (windowInfoPtr != NULL)"
		
		}	// end "if (graphRecordPtr != NULL)"
	
}	// end "SetXAxisDescriptionInfo"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
//	Revised By:			Larry L. Biehl			Date: 04/04/2019

void SetXGraphScale (
				GraphPtr								graphRecordPtr)

{
	char									number[64];
	
	double								interval;
	
	Rect*									clientRectPtr;
	
	//int									xFSigfigs;
	
	SInt32								error,
											numberDecimalDigits,
											numberIntervals,
											screen_width,
											ticLabelWidth;
	
	
			// Get the size of the graph window.
	
	#if defined multispec_mac
		GetWindowPortBounds (graphRecordPtr->window, &graphRecordPtr->clientRect);
	#endif	// defined multispec_mac
           
	#if defined multispec_win
		CMGraphView* graphViewCPtr = graphRecordPtr->graphViewCPtr;
		graphViewCPtr->GetClientRect ((RECT*)&graphRecordPtr->clientRect);
	#endif	// defined multispec_win
	
	#if defined multispec_wx
      CMGraphView* graphViewCPtr = graphRecordPtr->graphViewCPtr;
		graphViewCPtr->GetGraphWindowClientRect (&graphRecordPtr->clientRect);
	#endif
			// Get length of max x tic label string.
	
		graphRecordPtr->xESignificantDigits = 0;
	graphRecordPtr->xFSignificantDigits = 0;
	interval = 1;
	if (graphRecordPtr->flag & NU_HISTOGRAM_PLOT)
		interval = graphRecordPtr->origHistogramBinWidth;
	
	GetNumberDecimalDigits (graphRecordPtr->xValueTypeCode,
										graphRecordPtr->xMin,
										graphRecordPtr->xMax,
										interval,
										&graphRecordPtr->xESignificantDigits,
										&graphRecordPtr->xFSignificantDigits);
	
	if ((graphRecordPtr->processorCode == kSelectionGraphProcessor ||
			graphRecordPtr->processorCode == kListDataProcessor) &&
				graphRecordPtr->xMax - graphRecordPtr->xMin > 10 &&
					graphRecordPtr->xFSignificantDigits >= 1)
		graphRecordPtr->xFSignificantDigits -= 1;
	
	graphRecordPtr->xScaleMax = graphRecordPtr->xMax;

	FormatR (number,
					&graphRecordPtr->xScaleMax, 
					graphRecordPtr->xESignificantDigits, 
					graphRecordPtr->xFSignificantDigits, 
					false, 
					&error);
	ticLabelWidth = TextWidth ((UCharPtr)number, 0, (SInt16)strlen (number));
	
	graphRecordPtr->xScaleMin = graphRecordPtr->xMin;
   	
	FormatR (number, 
					&graphRecordPtr->xScaleMin, 
					graphRecordPtr->xESignificantDigits, 
					graphRecordPtr->xFSignificantDigits, 
					false, 
					&error);
	graphRecordPtr->xTicLabelWidth = 
									TextWidth ((UCharPtr)number, 0, (SInt16)strlen (number));
	
	graphRecordPtr->xTicLabelWidth = (SInt16)
										MAX (graphRecordPtr->xTicLabelWidth, ticLabelWidth);
	
			// Now get the x tick interval.
	
	clientRectPtr = &graphRecordPtr->clientRect;
	screen_width = (clientRectPtr->right - clientRectPtr->left) -
										graphRecordPtr->rightInset - graphRecordPtr->leftInset;
   
	numberIntervals = screen_width / (2*graphRecordPtr->xTicLabelWidth);

	numberIntervals = MAX (1, numberIntervals);
					
			// Now get the interval distance in "whole" graph units.
									
	interval = (graphRecordPtr->xMax - graphRecordPtr->xMin);
	if (interval == 0)
		interval = 1;
	
	if (graphRecordPtr->flag & NU_HISTOGRAM_PLOT)
		interval += graphRecordPtr->histogramBinWidth;
	
	interval /= numberIntervals;
	
 	if (graphRecordPtr->flag & NU_HISTOGRAM_PLOT)
 		{
		interval = ceil (interval/graphRecordPtr->histogramBinWidth);
		interval *= graphRecordPtr->histogramBinWidth;
	
		}	// end "if (graphRecordPtr->flag & NU_HISTOGRAM_PLOT)"
		
	else	// !(graphRecordPtr->flag & NU_HISTOGRAM_PLOT)
		{	
		if (graphRecordPtr->xValueTypeCode == kIntegerType)
			interval = ceil (interval);
		
		else	// graphRecordPtr->xValueTypeCode != kIntegerType
			{
			interval = ConvertToScientificFormat (interval, &numberDecimalDigits);
			interval = ceil (interval);
			interval = interval * pow ((double)10, (double)numberDecimalDigits);
		
			}	// end "else graphRecordPtr->xValueTypeCode != kIntegerType"
			
		}	// end "else !(graphRecordPtr->flag & NU_HISTOGRAM_PLOT)"
	
	if (!(graphRecordPtr->attrb & NU_XLOG))
		{
		if (graphRecordPtr->flag & NU_HISTOGRAM_PLOT)
			{
			graphRecordPtr->xScaleMin =
								floor ((graphRecordPtr->xMin-interval/2)/interval)*interval;
		
			graphRecordPtr->xScaleMax =
								ceil ((graphRecordPtr->xMax+interval/2)/interval)*interval;
				
			}	// end "if (graphRecordPtr->flag & NU_HISTOGRAM_PLOT)"
		
		else	// !(graphRecordPtr->flag & NU_HISTOGRAM_PLOT)
			{
			graphRecordPtr->xScaleMin = floor (graphRecordPtr->xMin/interval)*interval;
			
					// Adjust for case when xMin is just a vary small value less than
					// the tick more than xScaleMin
			
			if (graphRecordPtr->xScaleMin + interval - graphRecordPtr->xMin < .01 * interval)
				graphRecordPtr->xScaleMin += interval;
		
			graphRecordPtr->xScaleMax = ceil (graphRecordPtr->xMax/interval)*interval;
			
					// Adjust for case when xMax is just a vary small value more than
					// the tick less than xScaleMax
			
			if (graphRecordPtr->xScaleMax - interval > graphRecordPtr->xScaleMin &&
			 		graphRecordPtr->xMax - (graphRecordPtr->xScaleMax - interval) <
			 																				.01 * interval)
				graphRecordPtr->xScaleMax -= interval;
			
			}	// end "else !(graphRecordPtr->flag & NU_HISTOGRAM_PLOT)"
		
		if (graphRecordPtr->xScaleMin == graphRecordPtr->xScaleMax)
			{
			graphRecordPtr->xScaleMin -= interval;
			graphRecordPtr->xScaleMax += interval;
			
			}	// end "if (graphRecordPtr->xScaleMin == graphRecordPtr->xScaleMax)"
		
		}	// end "if (!(graphRecordPtr->attrb & NU_XLOG))"
		
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
											
		}	// end "if (graphRecordPtr->flag & NU_HISTOGRAM_PLOT)"
		
	graphRecordPtr->xInt = interval;
	
}	// end "SetXGraphScale"		



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
//	Revised By:			Larry L. Biehl			Date: 04/05/2018

void SetYGraphScale (
				GraphPtr								graphRecordPtr)

{
	char									number[64];
	
	double								interval,
											realAdditional;
	
	SInt32								error,
											numberDecimalPlaces,
											numberDigits,
											numberIntervalDigits,
											ticLabelWidth;
	
	UInt32								additional,
											integerInterval;
	
	
			// Get the size of the graph window.
	
	#if defined multispec_mac
		GetWindowPortBounds (graphRecordPtr->window, &graphRecordPtr->clientRect);
	#endif	// defined multispec_mac
           
	#if defined multispec_win
		CMGraphView* graphViewCPtr = graphRecordPtr->graphViewCPtr;
		graphViewCPtr->GetClientRect ((RECT*)&graphRecordPtr->clientRect);
	#endif	// defined multispec_win
			
				// Now determine the y-axis min, max and interval.
														
	interval = (graphRecordPtr->yMax - graphRecordPtr->yMin)/5;
	
	if (graphRecordPtr->yValueTypeCode == kIntegerType)
		{
		if (interval < 1)
			interval = 1;
			
		integerInterval = (UInt32)(interval + 0.5);
	
		numberDigits = sprintf (number, "%u", (unsigned int)integerInterval) - 2;
		if (numberDigits >= 1)
			{
			additional = (UInt32)pow ((double)10,(double)numberDigits);
			integerInterval = (integerInterval+additional)/additional;
			integerInterval *= additional;
			
			}	// end "if (numberDigits >= 1)"
			
		interval = (double)integerInterval;
		
		graphRecordPtr->yESignificantDigits = 0;
		graphRecordPtr->yFSignificantDigits = 0; 
			
		}	// end "if (graphRecordPtr->yValueTypeCode == kIntegerType)"
				
	else	// ...->yValueTypeCode == kRealType
		{
		numberDigits = 1;
		if (graphRecordPtr->yMax != 0)
			numberDigits = (SInt32)log10 (fabs (graphRecordPtr->yMax));
		
		if (interval == 0)
			{
			if (numberDigits > 10)
				interval = pow ((double)10,(double)(numberDigits-10));
				
			else	// numberWholeDigits <= 10
				interval = pow ((double)10,(double)(numberDigits-1));

			if (interval == 0)
				interval = 1;
				
			}	// end "if (interval == 0)"
			
		else	// interval > 0
			{
			if (interval < 1)
				{
				numberDigits = 0;
				numberDecimalPlaces = GetNumberLeadingDecimalZeros (interval);
				realAdditional = pow ((double)10,(double)(-numberDecimalPlaces));
				if (realAdditional > 0)
					{
					interval += realAdditional;
					interval = floor (interval/realAdditional)*realAdditional;

					}	// end "if (realAdditional > 0)"
				
				}	// end "if (interval < 1)"
			
			else	// interval >= 1
				{
				numberIntervalDigits = (SInt32)log10 (interval);
				//interval = pow ((double)10,(double)(numberDigits-1));
				
				realAdditional = pow ((double)10,(double)numberIntervalDigits);
				if (realAdditional > 0)
					{
					interval = (interval+realAdditional)/realAdditional;
					interval = floor (interval)*realAdditional;

					}	// end "if (realAdditional > 0)"
				
				}	// end "else interval >= 1"
				
			}	// end "else interval > 0"
		
		numberIntervalDigits = 0;
		if (interval > 1)
			numberIntervalDigits = (SInt32)log10 (interval);
			
		graphRecordPtr->yFSignificantDigits = GetNumberLeadingDecimalZeros (interval);
		
		if (graphRecordPtr->yFSignificantDigits == 0 && 
													numberDigits > numberIntervalDigits)
			{
			graphRecordPtr->yFSignificantDigits =
													(UInt16)(numberDigits - numberIntervalDigits);
			
			}	// end "if (graphRecordPtr->yFSignificantDigits == 0)"
		
		}	// end "else ...->yValueTypeCode == kRealType"
	
	if (!(graphRecordPtr->attrb & NU_YLOG))
		{
		graphRecordPtr->yScaleMin = floor (graphRecordPtr->yMin/interval)*interval;
		graphRecordPtr->yScaleMax = ceil (graphRecordPtr->yMax/interval)*interval;
		
		if (graphRecordPtr->yScaleMin == graphRecordPtr->yScaleMax)
			{
			if (graphRecordPtr->yScaleMin == graphRecordPtr->yMin)
				graphRecordPtr->yScaleMin -= interval;
				
			if (graphRecordPtr->yScaleMax == graphRecordPtr->yMax)
				graphRecordPtr->yScaleMax += interval;
				
			}	// end "if (graphRecordPtr->yMin == graphRecordPtr->yMax)" 
		
		}	// end "if (!(graphRecordPtr->attrb & NU_YLOG))"
	
	graphRecordPtr->yInt = interval;
		
		// Get the significant digits to use for the y-scale.
		
	GetNumberDecimalDigits (graphRecordPtr->yValueTypeCode,
										graphRecordPtr->yScaleMin,
										graphRecordPtr->yScaleMax,
										interval,
										&graphRecordPtr->yESignificantDigits,
										&graphRecordPtr->yFSignificantDigits);
		
			//	User defined.
						
	//graphRecordPtr->yMin = 7;
	//graphRecordPtr->yMax = 49;
	//graphRecordPtr->yInt = 7;
			
			// Now for selection graph windows, force the y-axis min, max and interval
			// to be the same for all graph windows that belong to this image window. 
		
	if (graphRecordPtr->processorCode == kSelectionGraphProcessor)
		SetLikeWindowMinMax (graphRecordPtr->imageWindow, 
										graphRecordPtr->yScaleMin, 
										graphRecordPtr->yScaleMax,
										graphRecordPtr->yInt);
			
				// Get length of max y tic label string.
				
		FormatR (number, 
						&graphRecordPtr->yScaleMin,
						graphRecordPtr->yESignificantDigits, 
						graphRecordPtr->yFSignificantDigits,
						false, 
						&error);
		ticLabelWidth = TextWidth ((UCharPtr)number, 0, (SInt16)strlen (number));
				
		FormatR (number, 
						&graphRecordPtr->yScaleMax,
						graphRecordPtr->yESignificantDigits, 
						graphRecordPtr->yFSignificantDigits,
						false, 
						&error);
		graphRecordPtr->yTicLabelWidth = 
									TextWidth ((UCharPtr)number, 0, (SInt16)strlen (number));
		graphRecordPtr->yTicLabelWidth = (SInt16)
											MAX (graphRecordPtr->yTicLabelWidth, ticLabelWidth);
		
		graphRecordPtr->yTicLabelWidth = MAX (39, graphRecordPtr->yTicLabelWidth+8);
	
}	// end "SetYGraphScale"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
// Called By:			DoGraphWActivateEvent in SGraphUtilities.cpp
//							GraphWControlEvent in SGraphUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/30/2003
//	Revised By:			Larry L. Biehl			Date: 04/30/2003		

void UpdateGraphScales (
				Handle								graphRecordHandle)

{	  
	GraphPtr								graphRecordPtr;
	
	
	graphRecordPtr = (GraphPtr)GetHandlePointer (graphRecordHandle, kLock);
										
	if (graphRecordPtr != NULL)
		{
		SetXGraphScale (graphRecordPtr);
		
		if (graphRecordPtr->graphCodesAvailable & 0x0001 &&
											graphRecordPtr->flag & NU_HISTOGRAM_PLOT)
			{
			SetGraphMinMax (graphRecordPtr, kYAxis);
			}
			
		else	// !...->graphCodesAvailable & 0x0001 && ...
			{		
			SetYGraphScale (graphRecordPtr);
			}
			
		}	// end "if (graphRecordPtr != NULL)"
		
	CheckAndUnlockHandle (graphRecordHandle); 
	
}	// end "UpdateGraphScales"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void UpdateSelectionGraphXAxisMinMax
//
//	Software purpose:	The routine finds the min and max values for the x-axis
//							for channel number, overall wavelength, reflective wavelengths,
//							and thermal wavelengths.
//							xAxis value of -100 indicate no data. This can occur for images
//							which are not in wavelength order and a subset of channels are
//							being used (such as in ListData processor).
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/16/2018
//	Revised By:			Larry L. Biehl			Date: 06/29/2018

void UpdateSelectionGraphXAxisMinMax (
				GraphPtr								graphPtr)
													
{
	GRAPHDATA							*xValuePtr,
											*xWidthPtr;
	
	double								value,
											widthValue;
	
	SInt32								error = noErr;
	
	UInt32								numberPoints;
	
	SInt16								index;
	
	Boolean								xDataMaxSetFlag,
											xDataMinSetFlag;
	

	if (graphPtr != NULL)
		{
		graphPtr->xDataMin = 1;
		graphPtr->xDataMax = 1;
		
				// Initialize values for x vector.
		
		LockAndGetVectorPointer (&graphPtr->xVector, &error);
		
		if (error != noErr)
																									return;
		
		xValuePtr = NULL;
		xWidthPtr = NULL;
		numberPoints = graphPtr->xVector.numberPoints;
		if	((graphPtr->descriptionCode & kBothReflectiveThermalData) &&
				graphPtr->xAxisCode > kChannels)
			{
			xValuePtr = &((&graphPtr->xVector)->basePtr[numberPoints]);
			
			if (graphPtr->xAxisCode == kBandWidths ||
					graphPtr->xAxisCode == kReflectiveBandWidths ||
						graphPtr->xAxisCode == kThermalBandWidths)
				{
				xWidthPtr = &((&graphPtr->xVector)->basePtr[2*numberPoints]);
				
				}	// end "if (graph->xAxisCode == kBandWidths || ..."
			
			}	// end "if	(hasWavelengthValuesFlag && ..."
			
		else
			xValuePtr = &((&graphPtr->xVector)->basePtr[0]);
														
		xDataMinSetFlag = FALSE;
		xDataMaxSetFlag = FALSE;
		
		for (index=0; index<numberPoints; index++)
			{
			value = *xValuePtr;
			
			if (value > 0)
				{
				widthValue = 0;
				if (xWidthPtr != NULL)
					{
					widthValue = *xWidthPtr;
					xWidthPtr++;
					
					}	// end "if (xWidthPtr != NULL)"
				
				if (!xDataMinSetFlag)
					{
					if (graphPtr->xAxisCode == kWavelengths ||
								graphPtr->xAxisCode == kChannels)
						{
						graphPtr->xDataMin = value;
						xDataMinSetFlag = TRUE;
						
						}	// end "if (graphRecordPtr->xAxisCode == kWavelengths || ..."
						
					else if (graphPtr->xAxisCode == kBandWidths)
						{
						graphPtr->xDataMin = value - widthValue;
						xDataMinSetFlag = TRUE;
						
						}	// end "if (graphRecordPtr->xAxisCode == kBandWidths)"
					
					else if (graphPtr->xAxisCode == kReflectiveWavelengths && value < 3)
						{
						graphPtr->xDataMin = value;
						xDataMinSetFlag = TRUE;
						
						}	// end "if (...->xAxisCode == kReflectiveWavelengths && ...)"
					
					else if (graphPtr->xAxisCode == kReflectiveBandWidths && value < 3)
						{
						graphPtr->xDataMin = value - widthValue;
						xDataMinSetFlag = TRUE;
						
						}	// end "else if (...->xAxisCode == kReflectiveBandWidths ...)"
					
					else if (graphPtr->xAxisCode == kThermalWavelengths && value >= 3)
						{
						graphPtr->xDataMin = value;
						xDataMinSetFlag = true;
						
						}	// end "else if (...->xAxisCode == kThermalWavelengths && ..."
					
					else if (graphPtr->xAxisCode == kThermalBandWidths && value >= 3)
						{
						graphPtr->xDataMin = value - widthValue;
						xDataMinSetFlag = true;
						
						}	// end "else if (...->xAxisCode == kThermalBandWidths ..."
						
					}	// end "if (!xDataMinSetFlag)"
					
				if (!xDataMaxSetFlag)
					{
					if (graphPtr->xAxisCode == kReflectiveWavelengths)
						{
						if (value < 3)
							graphPtr->xDataMax = value;
							
						else	// value >= 3
							xDataMaxSetFlag = TRUE;
							
						}	// end "if (...->xAxisCode == kReflectiveWavelengths)"

					else if (graphPtr->xAxisCode == kReflectiveBandWidths)
						{
						if (value < 3)
							graphPtr->xDataMax = value + widthValue;
							
						else	// value >= 3
							xDataMaxSetFlag = TRUE;
							
						}	// end "else if (...->xAxisCode == kReflectiveBandWidths)"
					
					}	// end "if (!xDataMaxSetFlag)"
				
				}	// end "if (value > 0)"
			
			xValuePtr++;
		
			}	// end "for (index=0; index<numberPoints; index++)"
			
		if (graphPtr->xAxisCode == kWavelengths ||
					graphPtr->xAxisCode == kChannels)
			graphPtr->xDataMax = value;
			
		else if (graphPtr->xAxisCode == kBandWidths)
			graphPtr->xDataMax = value + widthValue;
			
		else if (graphPtr->xAxisCode == kThermalWavelengths && value > 3)
			graphPtr->xDataMax = value;
			
		else if (graphPtr->xAxisCode == kThermalBandWidths && value > 3)
			graphPtr->xDataMax = value + widthValue;
	
		UnlockVectorPointer (&graphPtr->xVector);
		
		}	// end "if (graphPtr)"

}	// end "UpdateSelectionGraphXAxisMinMax"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
//	Revised By:			Larry L. Biehl			Date: 02/23/2018

void VerifyNeedForSelectVectorControl (
				WindowPtr							windowPtr)

{	                  
#if defined multispec_mac 
	GraphPtr								graphRecordPtr = NULL;

	ControlHandle						controlHandle,
											rootControlHandle = NULL;
											
	Handle								graphRecordHandle;
	
	SInt16								refCon;	
	
	UInt16								index,
											numberControls;
	
	
			// Update graph window controls	
				
	GetRootControl (windowPtr, &rootControlHandle);

	if (rootControlHandle != NULL)
		{
		graphRecordHandle = GetGraphRecordHandle (windowPtr);
		graphRecordPtr = (GraphPtr)GetHandlePointer (graphRecordHandle);
		
		}	// end "if (rootControlHandle != NULL)"
		
	if (graphRecordPtr != NULL)
		{					
		CountSubControls (rootControlHandle, &numberControls);
		for (index=numberControls; index>=1; index--)
			{
			GetIndexedSubControl (rootControlHandle, index, &controlHandle);
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
					
				}	// end "switch (refCon)"
					
			}	// end "while (controlHandle != NULL)" 
		
		}	// end "if (graphRecordPtr != NULL)"
#endif	// defined multispec_mac
	
#if defined multispec_win

#endif	// defined multispec_win
	
}	// end "VerifyNeedForSelectVectorControl"  
