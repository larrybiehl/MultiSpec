//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved
//
//	File:						SSelGraf.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision number:		3.0
//
//	Revision date:			10/17/2015
//
//	Language:				C
//
//	System:					Macintosh and Windows Operating Systems
//
//	Brief description:	This file contains functions that handle placing
//								a graph of the selected area into the graph window.
//
//	Functions in file:	Boolean 			CheckMemoryForStatisticsIO
//								void				ShowGraphWindowSelection
//
//	Diagram of MultiSpec routine calls for the routine in the file.
//
//
//
//	Include files:			"MultiSpec Headers"
//								"multiSpec.h"
//	
//	Software change notice:
//
//------------------------------------------------------------------------------------
  
#include "SMulSpec.h"
#include	"SGrafVew.h"

#if defined multispec_mac

#endif	// defined multispec_mac 
                             
#if defined multispec_win
	#include	"SGraphic.h"
	#include "CImagVew.h"
	#include	"CImagWin.h"
	#include "WDrawObj.h"
	#include "WImagDoc.h"
#endif	// defined multispec_win

#if defined multispec_lin
	#include	"SGraphic.h"
	#include	"LGraphView.h"
	#include "LImageView.h"
	#include	"LImageFrame.h"
	#include "LDrawObj.h"
	#include "LImageDoc.h"
#endif	// defined multispec_win

#include	"SExtGlob.h"	


extern void			MSetGraphWindowTitle ( 
							CMGraphView*						graphViewCPtr,
							StringPtr							titleStringPtr);		

extern void			SetDefaultSelectionGraphWindowTitle ( 
							CMGraphView*						graphViewCPtr);
																
																
			// Prototypes for routines in this file that are only called by		
			// other routines in this file.													
			
Boolean 			CheckMemoryForStatisticsIO  (
						SelectionIOInfoPtr				selectionIOInfoPtr);



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SelectionGraphControl
//
//	Software purpose:	The purpose of this routine is to handle the 
//							'New Selection Graph' menu item.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/27/1992
//	Revised By:			Larry L. Biehl			Date: 10/16/2015	

Boolean SelectionGraphControl (
				CMGraphView*						newSelectionGraphViewCPtr)

{                                              
	GraphPtr								selectionGraphRecordPtr;
 	SelectionIOInfoPtr				selectionIOPtr;
 	CMGraphView*						oldSelectionGraphViewCPtr;
 	                     	                          
 	Handle								oldSelectionGraphRecHandle, 
											selectionGraphRecHandle;    
 	
	Boolean								continueFlag; 
	
	SignedByte							handleStatus;
 	
 	                                              
	// Initialize some local variables.
			
	selectionIOPtr = NULL;             
	selectionGraphRecHandle = NULL;
	selectionGraphRecordPtr = NULL;
	
			// Save current selection graph information in case it is needs to
			// be restored.
	
   
	oldSelectionGraphViewCPtr = gSelectionGraphViewCPtr;
	                 
	oldSelectionGraphRecHandle = NULL;
	if (oldSelectionGraphViewCPtr != NULL) 
		oldSelectionGraphRecHandle = oldSelectionGraphViewCPtr->GetGraphRecordHandle();
     
			// Get the number of channels in the active image window.
		
	#if defined __Not_Framework
		newSelectionGraphViewCPtr = CreateGraphWindow ();
	#endif	// defined __Not_Framework
                               
	#if !defined __Not_Framework
		CMImageWindow* imageWindowCPtr = gActiveImageViewCPtr->GetImageWindowCPtr();
	#endif	// !defined __Not_Framework	
	
	Handle activeImageWindowInfoH = GetActiveImageWindowInfoHandle ();
	
	continueFlag = (newSelectionGraphViewCPtr != NULL);

	if (continueFlag)
		{
		WindowInfoPtr imageWindowInfoPtr = (WindowInfoPtr)GetHandlePointer(
											activeImageWindowInfoH, kNoLock, kNoMoveHi);
											
		SInt16 totalNumberChannels = imageWindowInfoPtr->totalNumberChannels;
		
		Handle fileInfoHandle = GetFileInfoHandle (imageWindowInfoPtr);
		FileInfoPtr imageFileInfoPtr = (FileInfoPtr)GetHandlePointer(
													fileInfoHandle, kNoLock, kNoMoveHi);
		UInt16 yDataTypeCode = imageFileInfoPtr->dataTypeCode;
		
		continueFlag = newSelectionGraphViewCPtr->FinishGraphRecordSetUp(
																					NULL,
																					totalNumberChannels,
																					5,
																					1,
																					kIntegerType,
																					yDataTypeCode);
								
		}		// end "if (continueFlag)"

	if (continueFlag)
		{  
		gSelectionGraphViewCPtr = newSelectionGraphViewCPtr; 

				// Lock the handle for the graph record.
					
		selectionGraphRecordPtr = GetGraphRecordPtr ( 	
														gSelectionGraphViewCPtr, 
														&handleStatus, 
														&selectionGraphRecHandle);
      
		if (selectionGraphRecordPtr != NULL)	
			{ 
			ShowGraphWindowSelection (oldSelectionGraphRecHandle);
			
			gUpdateWindowsMenuItemsFlag = TRUE;
	 	      										
	 	   }		// end "if (selectionGraphRecordPtr)" 

		if (selectionGraphRecHandle != NULL)
			CheckAndUnlockHandle (selectionGraphRecHandle);
			
		if (selectionGraphRecordPtr == NULL)
			{	
	      CloseGraphicsWindow (gSelectionGraphViewCPtr->GetWindowPtr());
	      
			gSelectionGraphViewCPtr = oldSelectionGraphViewCPtr;
	      
	      }		// end "if ( !selectionGraphRecordPtr )" 

		}		// end "if (continueFlag)"
		
	return (continueFlag); 
		
}		// end "SelectionGraphControl" 


                           
//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
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
//	Coded By:			Larry L. Biehl			Date: 08/13/2013
//	Revised By:			Larry L. Biehl			Date: 08/13/2013			

void SetDefaultSelectionGraphWindowTitle ( 
				CMGraphView*						graphViewCPtr)
													
{
	SInt16								savedProcessorCode;
	
	
	savedProcessorCode = gProcessorCode;
	gProcessorCode = kSelectionGraphProcessor;
	SetGraphWindowTitle (graphViewCPtr, TRUE);
	gProcessorCode = savedProcessorCode;

}		// end "SetDefaultSelectionGraphWindowTitle"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ShowGraphWindowSelection ()
//
//	Software purpose:	This routine handles making a graph of the data within
//							the selected area.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			Menus in menus.c
//							ShowGraphSelection in selectionArea.c
//
//	Coded By:			Larry L. Biehl			Date: 11/27/1991
//	Revised By:			Larry L. Biehl			Date: 10/16/2015			

void ShowGraphWindowSelection (
				Handle								oldSelectionGraphRecHandle)

{
	DoubleRect							coordinateRect;
	LongRect								selectionLineColumnRect; 
	
	SInt64								numberPixels;
	
	FileIOInstructionsPtr			fileIOInstructionsPtr;
	HChannelStatisticsPtr			channelStatsPtr;  
	GraphPtr								selectionGraphRecordPtr;
	SelectionIOInfoPtr				selectionIOInfoPtr; 
	HSumSquaresStatisticsPtr		totalSumSquaresPtr;
	HUCharPtr							tiledBufferPtr;
	                                                       
	
	Handle								selectionGraphRecHandle,
											selectionIOInfoHandle,
											windowInfoHandle; 
	
	UInt32								inputBytesNeeded,
											numberTileBytes,
											totalBytesNeeded; 
											
	UInt32								vectorLength; 
	
	SInt16								activeImageGlobalHandleStatus,
											errCode,
											savedCursor,
											typeCode;
	
	Boolean								changedFlag,
											continueFlag,
											memoryOKFlag,
											multiplePixelFlag; 
												
	SignedByte							graphRecordHandleStatus;
	
	
	selectionIOInfoHandle = 
							gSelectionGraphViewCPtr->GetSelectionIOHandle();
						
	if (selectionIOInfoHandle == NULL)
																						return;
	
			// If the active image information globals are already set 				
			// then return since a processor function is in control.  This			
			// call is for an image activate event after closing a dialog.			
			// The globals may be different than what will be used here.			
			
	if (gImageWindowInfoPtr != NULL || 
			gImageLayerInfoPtr != NULL || 
			gImageFileInfoPtr != NULL)
		
																						return;
		
	typeCode = 0;
	continueFlag = TRUE;
	memoryOKFlag = TRUE;
	selectionGraphRecHandle = NULL;
	selectionGraphRecordPtr = NULL;
		
	if (SetUpActiveImageInformationGlobals (
					&windowInfoHandle,  
					&activeImageGlobalHandleStatus, 
					kDoNotUseProject))
		{
				// Determine if a selection exists and get the selection boundary.

		typeCode = GetSelectionCoordinates (gActiveImageWindow, 
															&selectionLineColumnRect,
															&coordinateRect,
															&numberPixels);

		}		// end "if ( SetUpActiveImageInformationGlobals (... )" 
			
			// Get the graph record pointer for the current selection graph		
			// window.	
    
	selectionGraphRecHandle = gSelectionGraphViewCPtr->GetGraphRecordHandle();
	
	if (selectionGraphRecHandle != NULL)
		selectionGraphRecordPtr = (GraphPtr)GetHandleStatusAndPointer(
											selectionGraphRecHandle,
											&graphRecordHandleStatus, 
											kNoMoveHi); 
	
	if (selectionGraphRecordPtr != NULL &&
						selectionGraphRecordPtr->lastActiveImageWindow != gActiveImageWindow)
		{
				// Set the title for the selection graph window
		
		gTextString[0] = 0;
		gTextString[1] = 0;
		ConcatPStrings ((UCharPtr)&gTextString, (StringPtr)"\0Selection Graph\0", 254);
			
		if (gActiveImageWindow != NULL)
			{
			ConcatPStrings ((UCharPtr)&gTextString, (StringPtr)"\0: \0", 254);
			GetActiveImageWindowTitle (gTextString2);
			if (gTextString2[gTextString2[0]] == ')')
				{
						// Remove the suffix
				
				char* stringPtr = strchr ((char*)&gTextString2[1], '(');
				if (stringPtr != NULL)
					{
					*stringPtr = 0;
					gTextString2[0] = strlen ((char*)&gTextString2[1]);
					
					}		// end "if (stringPtr != NULL)"
				
				}		// end "if (gTextString2[gTextString2[0]] == ')');"	
				
			ConcatPStrings ((UCharPtr)&gTextString, gTextString2, 254);	
			
			}		// end "if (gActiveImageWindow != NULL)"
		
		MSetGraphWindowTitle (gSelectionGraphViewCPtr, gTextString);
		
		selectionGraphRecordPtr->lastActiveImageWindow = gActiveImageWindow;
		
		}		// end "if (selectionGraphRecordPtr->imageWindow != gActiveImageWindow)"
	
	if (selectionGraphRecordPtr && typeCode != 0)
		{
		savedCursor = gPresentCursor;
		MSetCursor (kWait); 
		
		selectionIOInfoPtr = (SelectionIOInfoPtr)GetHandlePointer(
											selectionIOInfoHandle, kLock, kNoMoveHi);
	
				// Check if new values need to be read for the selection graph.		
				
		if (selectionGraphRecordPtr->imageWindow == gActiveImageWindow && 
			selectionGraphRecordPtr->imageViewCPtr == gActiveImageViewCPtr &&
			selectionGraphRecordPtr->selectionTypeCode == typeCode &&
			selectionGraphRecordPtr->imageLineColRect.top == 
														selectionLineColumnRect.top &&
			selectionGraphRecordPtr->imageLineColRect.bottom == 
														selectionLineColumnRect.bottom &&
			selectionGraphRecordPtr->imageLineColRect.left == 
														selectionLineColumnRect.left &&
			selectionGraphRecordPtr->imageLineColRect.right == 
														selectionLineColumnRect.right)
				{                    			
				continueFlag = FALSE;
				if (selectionGraphRecordPtr->drawGraphCode & 0x8000)
					{
							// Force the graph selection window to be redrawn since	
							// it has been empty.												
							                     
					#if defined multispec_mac
						InvalidateWindow (selectionGraphRecordPtr->window, kFrameArea, FALSE);
					#endif	// defined multispec_mac	  
					       
					#if defined multispec_win	                                      
						gSelectionGraphViewCPtr->Invalidate();
					#endif	// defined multispec_win
					
               #if defined multispec_lin
                 gSelectionGraphViewCPtr->m_frame->Refresh();            
               #endif
					
					}		// end "if (...->drawGraphCode & 0x8000)" 
            
				
				}		// end "if (...->imageWindow == gActiveImageWindow ..." 
				
		else		// ...->imageWindow != gActiveImageWindow || ...
			selectionIOInfoPtr->memoryWarningFlag = FALSE;
			
		if ( !selectionIOInfoPtr->memoryWarningFlag )		
			selectionGraphRecordPtr->drawGraphCode = 1;
				
				// Check memory allocation.													
		
		if (continueFlag && selectionIOInfoPtr->checkIOMemoryFlag)
			{
					// Make certain that the image window and image view class that
					// the selection graph represents is correct.
			
			selectionGraphRecordPtr->imageWindow = gActiveImageWindow;
			selectionGraphRecordPtr->imageViewCPtr = gActiveImageViewCPtr;
			
					// Save the selection code for this selection graph.
					
			selectionGraphRecordPtr->selectionTypeCode = typeCode;
			
					// Make certain that the vector sizes are large enough.			
					
			memoryOKFlag = CheckGraphVectorsSize ( 
								selectionGraphRecordPtr, 
								(SInt32)gImageWindowInfoPtr->totalNumberChannels,
								5,
								1);	
			
			selectionGraphRecordPtr->xValueTypeCode = kIntegerType;
			selectionGraphRecordPtr->yValueTypeCode = gImageFileInfoPtr->dataTypeCode;			
			
			if ( memoryOKFlag )
				memoryOKFlag = LoadGraphXVector (
								selectionGraphRecordPtr, 
								NULL, 
								(SInt32)gImageWindowInfoPtr->totalNumberChannels);		
		
			if (memoryOKFlag)
				memoryOKFlag = CheckMemoryForStatisticsIO (selectionIOInfoPtr);
				
			if (memoryOKFlag)
				{
				selectionGraphRecordPtr->flag = NU_LINE_PLOT;
				if (gImageWindowInfoPtr->totalNumberChannels <= 1)
					selectionGraphRecordPtr->flag = NU_SCATTER_PLOT;
						
				selectionIOInfoPtr->checkIOMemoryFlag = FALSE;
					
				}		// end "if (memoryOKFlag)" 
				
			continueFlag = memoryOKFlag;
				
			}		// end "if (...->checkIOMemoryFlag)" 
	
				// Check the number of bytes needed for input.								
	
		if (continueFlag)
			{
			gAreaDescription.lineInterval = 1;
			gAreaDescription.columnInterval = 1;
			GetSelectionBoundary (gActiveImageWindow, &gAreaDescription);
													
					// Determine if only a single pixel is being selected.			
					
			multiplePixelFlag = FALSE;
			if (gImageWindowInfoPtr->imageType != kThematicImageType && 
					(gAreaDescription.lineEnd > gAreaDescription.lineStart ||
					gAreaDescription.columnEnd > gAreaDescription.columnStart) )
				multiplePixelFlag = TRUE;
		
			inputBytesNeeded = gImageWindowInfoPtr->maxNumberColumns;
			inputBytesNeeded *= gImageWindowInfoPtr->numberBytes *
														gImageWindowInfoPtr->totalNumberChannels;
			inputBytesNeeded = ((inputBytesNeeded + 7) / 8) * 8;
			totalBytesNeeded = inputBytesNeeded;
		
					// Allow for converting to 8 bytes in output buffer.
					
			if (gImageWindowInfoPtr->localMaxNumberBytes == 1)
				totalBytesNeeded += 8*inputBytesNeeded;
			
			else if (gImageWindowInfoPtr->localMaxNumberBytes == 2)
				totalBytesNeeded += 4*inputBytesNeeded;
			
			else if (gImageWindowInfoPtr->localMaxNumberBytes == 4)
				totalBytesNeeded += 2*inputBytesNeeded;
			
			if (gImageFileInfoPtr->bandInterleave != kBIS &&
											totalBytesNeeded == inputBytesNeeded)
				{		
				totalBytesNeeded += inputBytesNeeded;
			
				}		// end "if (gImageFileInfoPtr->bandInterleave != kBIS)"
			
			if (!multiplePixelFlag)
				{
				if (gImageWindowInfoPtr->localBytesDifferFlag)
					totalBytesNeeded *= 2;
					
				}		// end "if (!multiplePixelFlag)"
				
					// Allow for converting to BIS format if needed.
		
					// Get the number of bytes needed for tile file IO.
					// Force it to be a multiple of 4 bytes. 
				
			numberTileBytes = GetSetTiledIOBufferBytes (gImageLayerInfoPtr,
																		gImageFileInfoPtr, 
																		NULL, 
																		gImageWindowInfoPtr->totalNumberChannels,
																		NULL);
		
			gInputBufferPtr = (HUCharPtr)CheckHandleSize ( 
											&selectionIOInfoPtr->ioBufferHandle,
											&memoryOKFlag, 
											&changedFlag, 
											totalBytesNeeded+numberTileBytes );
			
			if ( !memoryOKFlag )							
				selectionIOInfoPtr->memoryWarningFlag = TRUE;
				
			continueFlag = memoryOKFlag;
			
			}		// end "if (continueFlag)" 
			
		if (continueFlag && !oldSelectionGraphRecHandle)
			{					
			selectionIOInfoPtr->outputBufferOffset = 0;
			if (totalBytesNeeded != inputBytesNeeded)
				selectionIOInfoPtr->outputBufferOffset = inputBytesNeeded;
				
			gOutputBufferPtr = &gInputBufferPtr[
													selectionIOInfoPtr->outputBufferOffset];
													
			tiledBufferPtr = &gInputBufferPtr[totalBytesNeeded];
		
					// Set up the general parameters in the File IO Instructions structure.
		
			SetUpGeneralFileIOInstructions (&gFileIOInstructions[0],
														gImageWindowInfoPtr,	
														gImageLayerInfoPtr,
														gImageFileInfoPtr,
														gImageWindowInfoPtr->totalNumberChannels,
														NULL,
														gInputBufferPtr,
														gOutputBufferPtr,
														tiledBufferPtr,
														0,
														kDoNotPackData,
														kForceBISFormat,
														kForceReal8Bytes,
														&fileIOInstructionsPtr);
													
			vectorLength = gImageWindowInfoPtr->totalNumberChannels;
					
			if (multiplePixelFlag)
				{
						// Make sure that the graphics window for which new data are
						// being read do not get modified during an update event
						// that can get called in GetAreaStats.
			
				#if defined multispec_mac 
					GrafPtr  savedPort;
					
					GetPort ( &savedPort );
					SetPortWindowPort (selectionGraphRecordPtr->window);
					ValidWindowRect (selectionGraphRecordPtr->window, 
											GetWindowPortBounds(selectionGraphRecordPtr->window, &gTempRect));
					SetPort (savedPort);
				#endif	// defined multispec_mac
			
				channelStatsPtr = 
						(ChannelStatisticsPtr)GetHandlePointer(
											selectionIOInfoPtr->channelStatsHandle, 
											kLock, 
											kNoMoveHi);
											
				totalSumSquaresPtr = 
						(SumSquaresStatisticsPtr)GetHandlePointer(
											selectionIOInfoPtr->sumSquaresHandle, 
											kLock, 
											kNoMoveHi);
					
						// Initialize memory.													
						
				ZeroStatisticsMemory ( channelStatsPtr, 
												totalSumSquaresPtr, 
												gImageWindowInfoPtr->totalNumberChannels, 
												1, 
												kTriangleOutputMatrix);
				
						// Get the sum of data values for selected area.				
						
				gAreaDescription.lineInterval = 1;
				gAreaDescription.columnInterval = 1;
		
				errCode = GetAreaStats (fileIOInstructionsPtr, 
													channelStatsPtr, 
													totalSumSquaresPtr, 
													NULL, 
													gImageWindowInfoPtr->totalNumberChannels, 
													FALSE, 
													kMeanStdDevOnly,
													NULL,
													NULL); 
				
				if (errCode != 1)
					vectorLength = 0;
					
				else		// errCode == 1
					ComputeMeanStdDevVector ( channelStatsPtr, 
											totalSumSquaresPtr, 
											gImageWindowInfoPtr->totalNumberChannels, 
											gAreaDescription.numSamplesPerChan, 
											1,
											kTriangleInputMatrix);
									
				LoadGraphYVector2 (selectionGraphRecordPtr, 
											channelStatsPtr, 
											vectorLength);
					
				}		// end "if (multiplePixelFlag)" 
				
			else		// !multiplePixelFlag 
				{					
				gAreaDescription.lineEnd = gAreaDescription.lineStart;
				gAreaDescription.columnEnd = gAreaDescription.columnStart;
			
						// Load some of the File IO Instructions structure that pertain
						// to the specific area being used.
						
				errCode = SetUpFileIOInstructions (fileIOInstructionsPtr,
																&gAreaDescription,
																gImageWindowInfoPtr->totalNumberChannels,
																NULL,
																kDetermineSpecialBILFlag);
			                
				if (errCode == noErr)          
					errCode = GetLineOfData (fileIOInstructionsPtr,
														gAreaDescription.lineStart, 
														gAreaDescription.columnStart,
														gAreaDescription.columnStart,
														1,
														gInputBufferPtr,
														gOutputBufferPtr);
								
				if (errCode != noErr)
					vectorLength = 0;
		
						// Close up any File IO Instructions structure that pertain
						// to the specific area that was classified.
						
				CloseUpFileIOInstructions (fileIOInstructionsPtr, &gAreaDescription);
				
				if (continueFlag)				
					LoadGraphYVector (selectionGraphRecordPtr, 
											(double*)gOutputBufferPtr,
											vectorLength,
											1);
				
				}		// end "else !multiplePixelFlag" 

			CloseUpGeneralFileIOInstructions (fileIOInstructionsPtr);
						
					// Dispose of the region if it exists.						

			CloseUpAreaDescription (&gAreaDescription);
				
			LoadGraphSupportArrays (
								selectionGraphRecordPtr, 
								vectorLength);
				
			}		// end "if (continueFlag && !oldSelectionGraphRecHandle)" 
			
		if (continueFlag && oldSelectionGraphRecHandle != NULL)
			CopyGraphYVector (oldSelectionGraphRecHandle, selectionGraphRecordPtr);
			
		if (continueFlag)
			continueFlag = CreateGraph (
							gSelectionGraphViewCPtr, 
							selectionGraphRecordPtr, 
							gAreaDescription.lineStart, 
							gAreaDescription.columnStart, 
							gAreaDescription.lineEnd, 
							gAreaDescription.columnEnd);
				
		CheckAndUnlockHandle (selectionIOInfoPtr->ioBufferHandle);
		CheckAndUnlockHandle (selectionIOInfoPtr->channelStatsHandle);
		CheckAndUnlockHandle (selectionIOInfoPtr->sumSquaresHandle);
			
		CheckAndUnlockHandle (selectionIOInfoHandle);
		
		MSetCursor (savedCursor);
				
		}		// end "if (typeCode != 0)" 
		
	else		// typeCode == 0 
		continueFlag = FALSE;


	if ( typeCode == 0 || !memoryOKFlag )
		{
		selectionGraphRecordPtr->drawGraphCode = (SInt16)0x8000;
		                   
		#if defined multispec_mac
			InvalidateWindow (selectionGraphRecordPtr->window, kFrameArea, FALSE); 
		#endif	// defined multispec_mac	
			         
		#if defined multispec_win
			gSelectionGraphViewCPtr->Invalidate();	 
		#endif	// defined multispec_win
		
		#if defined multispec_lin
         gSelectionGraphViewCPtr->m_frame->Refresh();
      #endif
		}		// end "else typeCode == kRectangleType ||  !memoryOKFlag" 
		
	if (continueFlag || !memoryOKFlag)
		{
		selectionGraphRecordPtr->imageWindow = gActiveImageWindow;
		selectionGraphRecordPtr->imageLineColRect.top = 
														gAreaDescription.lineStart;
		selectionGraphRecordPtr->imageLineColRect.bottom = 
														gAreaDescription.lineEnd;
		selectionGraphRecordPtr->imageLineColRect.left = 
														gAreaDescription.columnStart;
		selectionGraphRecordPtr->imageLineColRect.right = 
														gAreaDescription.columnEnd;
		
		}		// end "if (continueFlag || !memoryOKFlag)" 
			
	if (!memoryOKFlag)
		{
		DisposeOfGraphRecordMemory (selectionGraphRecordPtr);
		
		selectionIOInfoPtr->channelStatsHandle = 
						UnlockAndDispose (selectionIOInfoPtr->channelStatsHandle);
		selectionIOInfoPtr->sumSquaresHandle = 
						UnlockAndDispose (selectionIOInfoPtr->sumSquaresHandle);
		selectionIOInfoPtr->ioBufferHandle = 
						UnlockAndDispose (selectionIOInfoPtr->ioBufferHandle);
		
		}		// end "if (!memoryOKFlag)" 
				
			// Unlock handles if needed.
		
	if (selectionGraphRecHandle != NULL)
		MHSetState (selectionGraphRecHandle, graphRecordHandleStatus);
 

	UnlockActiveImageInformationGlobals (activeImageGlobalHandleStatus);
	
	gImageWindowInfoPtr = NULL;
	gImageLayerInfoPtr = NULL;
	gImageFileInfoPtr = NULL;
			
	gOutputBufferPtr = gInputBufferPtr = NULL;
   
}		// end routine "ShowGraphWindowSelection" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CheckMemoryForStatisticsIO ()
//
//	Software purpose:	This routine makes certain that the memory allocated
//							statistics calculation input is okay.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	TRUE, if memory is okay
//							FALSE, if there is not enough memory.
// 
// Called By:			ShowGraphWindowSelection in selectionGraph.c
//
//	Coded By:			Larry L. Biehl			Date: 07/27/1992
//	Revised By:			Larry L. Biehl			Date: 07/27/1992			

Boolean CheckMemoryForStatisticsIO  (
				SelectionIOInfoPtr				selectionIOInfoPtr)

{	                                          
	SInt32								totalBytesNeeded;
	
	Boolean								changedFlag,
											continueFlag;
	
	
	continueFlag = TRUE;
			
			// Check handle for "total" first order statistics.						
				
	totalBytesNeeded = sizeof(ChannelStatistics) *
					(UInt32)gImageWindowInfoPtr->totalNumberChannels;
	
	CheckHandleSize ( &selectionIOInfoPtr->channelStatsHandle,
								&continueFlag, 
								&changedFlag, 
								totalBytesNeeded);
		
	if (continueFlag)
		{
				// Check handle for standard deviation statistics.						
				
		totalBytesNeeded = sizeof(SumSquaresStatistics) *
						(UInt32)gImageWindowInfoPtr->totalNumberChannels;
									
		CheckHandleSize ( &selectionIOInfoPtr->sumSquaresHandle, 
									&continueFlag, 
									&changedFlag, 
									totalBytesNeeded);
			
		}		// end "if (continueFlag)" 
		
	return (continueFlag);

}		// end "CheckMemoryForStatisticsIO"       

