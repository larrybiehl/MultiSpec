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
//	File:						SBiPlotData.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			02/27/2020
//
//	Language:				C
//
//	System:					Linux, Macintosh and Windows Operating Systems
//
//	Brief description:	This file contains functions that allow the user to
//								plot the data values for selected channels, classes
//								and fields and or selected area in a graphics window.
//								I call it a biplot because one channel (feature) is plotted
//								along the x axis and one channel is plotted along the
//								y axis.
/*
	Template for debugging
		int numberChars = sprintf ((char*)gTextString3,
													" SBiPlotData: (): %s", 
													gEndOfLine);
		ListString ((char*)gTextString3, numberChars, gOutputTextH);	
*/
//------------------------------------------------------------------------------------


#include "SMultiSpec.h"

#ifdef multispec_wx    
   #include "xBiplotDialog.h"
   #include "xGraphView.h"
#endif

#if defined multispec_mac || defined multispec_mac_swift
	#include	"MGraphView.h"
	
	#define IDC_ChannelPrompt					4
	#define IDC_FeatureTransformation		9
	#define IDC_CreateNewGraphWindow			11
	#define IDC_Training							12
	#define IDC_TestAreas						13
	#define IDC_DisplayPixels					26
	#define IDC_Symbols							27
	#define IDC_Color								28
	#define IDC_OutlineClassAsEllipse		29
	#define IDC_EllipsePrompt					30
	#define IDC_Border							31
	#define IDC_ThresholdLevelPrompt			32
	#define IDC_ThresholdLevel					33
	#define IDC_PercentPrompt					34
	#define IDC_ThresholdPixelCheck			35
	#define IDC_SymbolPrompt					36
	#define IDC_SymbolCombo						37
	#define IDC_WeightsPrompt					38
	#define IDC_WeightsCombo					39
	#define IDC_ClassPrompt						41
	#define IDC_ClassCombo						42

	#define IDS_BiPlot1							1
	#define IDS_BiPlot2							2
	#define IDS_BiPlot3							3
	#define IDS_Alert151							151
#endif	// defined multispec_mac || defined multispec_mac_swift 
  
#if defined multispec_win 
	#include	"WGraphView.h" 
	 
	#include "WGraphDoc.h"
	#include "WBiPlotDialog.h"
	#include "WMultiSpec.h"
#endif	// defined multispec_win

	
	
			// Prototypes for routines in this file that are only called by		
			// other routines in this file.													
											
SInt16 BiPlotClassData (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				SInt16								classNumber);

Boolean BiPlotDataDialog (void);

SInt16 BiPlotFieldData (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				SInt16								classNumberCode,
				SInt16								fieldNumber);

Boolean  BiPlotProjectData (void);
								
Boolean CheckIfStatisticsChannel (
				SInt16								channelNumber,
				Boolean								featureTransformationFlag);

void FillVectorOffsets (
				GraphPtr								graphRecordPtr);
								
SInt16 GetStatisticsChannelFeature (
				SInt16								channelNumber,
				Boolean								featureTransformationFlag);

Boolean  LoadBiPlotClassStats (
				GraphPtr								graphRecordPtr);
								
Boolean LoadBiPlotDataSpecs (
				Handle								windowInfoHandle);

Boolean SetupBiPlotStatMemory (
				ChannelStatisticsPtr*			classChannelStatsPtrPtr,
				double**								eigenValuePtrPtr,
				double**								eigenVectorPtrPtr,
				SInt16								numberChannels);

void UpdateEllipseMinMax (
				GraphPtr								graphRecordPtr,
				double								angle,
				double*								classStatisticsPtr);

void UpdateEllipseMinMax2 (
				GraphPtr								graphRecordPtr,
				double								angle,
				double*								classStatisticsPtr);



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 BiPlotClassData ()
//
//	Software purpose:	The purpose of this routine is to plot the class
//							data for the requested channels to the output text
//							window or disk file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 03/08/1994
//	Revised By:			Larry L. Biehl			Date: 10/25/1999	

SInt16 BiPlotClassData (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				SInt16								classNumber)

{
	HPClassNamesPtr					classNamesPtr;
	HPFieldIdentifiersPtr			fieldIdentPtr;
	
	SInt16								classStorage,
											fieldCount,
											fieldNumber,
											numberOfFields;
	
	
			// Initialize local variables.													
			
	classNamesPtr = gProjectInfoPtr->classNamesPtr;
	fieldIdentPtr = gProjectInfoPtr->fieldIdentPtr;
	
			// Get the class storage number.													
						
	classStorage = gProjectInfoPtr->storageClass[classNumber];
	
			// Continue only if the number of fields in the class is one or more.
	
	numberOfFields = 0;
	if (gBiPlotDataSpecsPtr->plotDataCode & kTrainingType)
		numberOfFields += classNamesPtr[classStorage].numberOfTrainFields;
	if (gBiPlotDataSpecsPtr->plotDataCode & kTestingType)
		numberOfFields += classNamesPtr[classStorage].numberOfTestFields;
		
	if (numberOfFields > 0)
		{
				// Set up status dialog. Class name, and number of						
				// training fields for the class.											
				
		LoadDItemString (gStatusDialogPtr, 
								IDC_Status6, 
								(Str255*)&classNamesPtr[classStorage].name);
		LoadDItemValue (gStatusDialogPtr, IDC_Status10, numberOfFields);
			
		fieldNumber = classNamesPtr[classStorage].firstFieldNumber;
		
		fieldCount = 1;
		while (fieldNumber != -1)
			{
					// Make certain that field is training field.						
					
			if (fieldIdentPtr[fieldNumber].fieldType & 
															gBiPlotDataSpecsPtr->plotDataCode)
				{
						// Set up field count in status dialog.							
				
				LoadDItemValue (gStatusDialogPtr, IDC_Status8, (SInt32)fieldCount);
					
						// Create a biplot of the requested data for the field.		
					
				if (BiPlotFieldData (fileIOInstructionsPtr,
												classNumber+1, 
												fieldNumber) == 0)
																							return (0);
				
				fieldCount++;
								
				}	// end "if (fieldIdentPtr[fieldNumber].field..." 
				
			fieldNumber = fieldIdentPtr[fieldNumber].nextField;
			
			}	// end "while (fieldNumber != -1)" 
			
		}	// end "if (classNamesPtr[classStorage].number ..." 
	
			// Indicate that routine completed normally.									
			
	return (1);
		
}	// end "BiPlotClassData" 
								


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				void BiPlotDataControl ()
//
//	Software purpose:	The purpose of this routine is to handle the
//							"BiPlot Data" event.  The defaults will be set
//							according to the statistics window mode and selection.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 03/08/1994
//	Revised By:			Wei-Kang Hsu			Date: 08/03/2016
//	Revised By:			Larry L. Biehl			Date: 05/03/2019

void BiPlotDataControl (void)

{
	CMGraphView*						graphViewCPtr;
	FileInfoPtr							savedFileInfoPtr;
	FileIOInstructionsPtr			fileIOInstructionsPtr;
	LayerInfoPtr						savedLayerInfoPtr;
	WindowInfoPtr						savedWindowInfoPtr;
	
	SInt64								totalNumberPixels;
	
	Handle								graphRecHandle,
											activeWindowInfoHandle;
	
	SInt32								numberVectors;
											
	time_t								startTime;
											
	SInt16								activeImageGlobalHandleStatus,
											savedThresholdSize,
											projectHandleStatus;
	
	Boolean								continueFlag;
											//lineFlag = FALSE;
											
	SignedByte							handleStatus;
   
   UInt16*                       channelsPtr;
	
		    		
			// If spare memory had to be used to load code resources, then exit	
			// routine.																				
			
	if (gMemoryTypeNeeded < 0)
																							return;
																																														
			// Code resources loaded okay, so set flag back for non-Code			
			// resources.																			
			
	gMemoryTypeNeeded = 0;
 			
 			// Initialize variables.															
 	
 	continueFlag = TRUE;
 	gStatusDialogPtr = NULL;
 	gBiPlotDataSpecsPtr = NULL;
	graphRecHandle	= NULL;
	projectHandleStatus = -1;
	savedWindowInfoPtr = NULL;
	savedFileInfoPtr = NULL;
	savedLayerInfoPtr = NULL;
	fileIOInstructionsPtr = NULL; 
	
			// Reset global variables for creating biplots of data.					
			
	savedThresholdSize = gThresholdSize;
	gThresholdSize = 0;
 	
 			// Get image window globals.
 			
	continueFlag = SetUpActiveImageInformationGlobals (&activeWindowInfoHandle,
																		&activeImageGlobalHandleStatus, 
																		kUseProject);
	
			// Set up biplot specification structure.										
			
	if (continueFlag && LoadBiPlotDataSpecs (activeWindowInfoHandle))
		{
		if (gImageWindowInfoPtr == NULL || gImageFileInfoPtr == NULL)
																							return;
			
		if (BiPlotDataDialog ())
			{
					// Initialize some global variables pertaining to output.		
					
			gOutputTextOKFlag = TRUE;
			gOutputForce1Code = 0x0001;	
			
			gGraphRecordPtr = NULL;			
			
			if (gBiPlotDataSpecsPtr->outlineClassCode & kOutlineClasses)
				{
						// Save the current global information pointers and force
						// 'VerifyProjectStatsUpToDate' to get the project 
						// information pointers for the statistics if needed.
						// This is done in case the active image window is not the project
						// base image.
						
				savedWindowInfoPtr = gImageWindowInfoPtr;
				savedFileInfoPtr = gImageFileInfoPtr;
				savedLayerInfoPtr = gImageLayerInfoPtr;
				
				gImageFileInfoPtr = NULL;
				gImageLayerInfoPtr = NULL;
				gImageWindowInfoPtr = NULL;
			
						// Update statistics for project if needed	
							
				continueFlag = VerifyProjectStatsUpToDate (
															&gBiPlotDataSpecsPtr->numberClasses,
															gBiPlotDataSpecsPtr->classPtr,
															1,
															gProjectInfoPtr->statisticsCode,
															kNoStatisticsUsed, 
															kSetupGlobalInfoPointers,
															NULL);
				
				gImageWindowInfoPtr = savedWindowInfoPtr;
				gImageFileInfoPtr = savedFileInfoPtr;
				gImageLayerInfoPtr = savedLayerInfoPtr;
				
				savedWindowInfoPtr = NULL;
				savedFileInfoPtr = NULL;
				savedLayerInfoPtr = NULL;
										
				}	// end "if (...->outlineClassCode & kOutlineClasses)" 

					// Continue biplot data if everything is okay.
			
			if (continueFlag)
				{
						// Turn spin cursor on
		
				gPresentCursor = kSpin;
		
						// Force text selection to start from end of present text.	
						
				ForceTextToEnd ();
								
				}	// end "if (continueFlag)" 
					
					// List the processor name, date, time and project info.			

			continueFlag = ListHeaderInfo (
												NULL,
												kLImageInfo + 2*gBiPlotDataSpecsPtr->projectFlag,
												&gOutputForce1Code,
												kNoStatisticsUsed,
												continueFlag);
					
					// List the classes used.													
											
			if (continueFlag && 
					gBiPlotDataSpecsPtr->plotDataCode & (kTrainingType+kTestingType))
				{
				gAreaDescription.lineInterval = gBiPlotDataSpecsPtr->lineInterval;
				gAreaDescription.columnInterval = gBiPlotDataSpecsPtr->columnInterval;
	
				continueFlag =  ListClassesUsed (
												NULL,
												gImageFileInfoPtr, 
												gBiPlotDataSpecsPtr->classPtr,
												kListNoSymbols,
												kListNoWeights, 
												gBiPlotDataSpecsPtr->numberClasses, 
												NULL, 
												&gOutputForce1Code,
												FALSE);
		
						// List the fields/areas used.										
							
				if (continueFlag)							
					continueFlag = ListProjectFieldsUsed (
												gImageFileInfoPtr, 
												gBiPlotDataSpecsPtr->classPtr,
												gBiPlotDataSpecsPtr->numberClasses, 
												NULL, 
												gBiPlotDataSpecsPtr->plotDataCode, 
												NULL, 
												&gOutputForce1Code,
												FALSE);
												
				}	// end "if (... && ...plotDataCode...)" 
																		
			startTime = time (NULL);	
					
         channelsPtr = (UInt16*)GetHandlePointer (gBiPlotDataSpecsPtr->channelsHandle);
               
					// Get dialog box to display list data status.						
			
			if (continueFlag)
				gStatusDialogPtr = GetStatusDialog (kUpdateStatsInfoID, TRUE);
				
			if (gStatusDialogPtr == NULL) 
				continueFlag = FALSE;
				
			ShowStatusDialogItemSet (kStatusLine);
			ShowStatusDialogItemSet (kStatusCommand);
	
			if (continueFlag && gBiPlotDataSpecsPtr->featureTransformationFlag)	
				continueFlag = SetupFeatureTransformationMemory (
												gBiPlotDataSpecsPtr->featureTransformationFlag,
												gTransformationMatrix.numberFeatures,
												gTransformationMatrix.numberChannels,
												&gTransformationMatrix.eigenVectorPtr, 
												&gTransformationMatrix.tempMatrixPtr,
												&gTransformationMatrix.offsetVectorPtr, 
												&gTransformationMatrix.eigenFeaturePtr,
												kDoNotLoadMatricesVectors,
												NULL);
	
					// Get the graph window if needed.										
				
			if (continueFlag)
				{
				totalNumberPixels = 0;
				if (gBiPlotDataSpecsPtr->displayPixelCode & kDisplayPixels)
					totalNumberPixels = GetTotalNumberOfPixels (
														gBiPlotDataSpecsPtr->plotDataCode,
														gBiPlotDataSpecsPtr->classPtr,
														gBiPlotDataSpecsPtr->numberClasses, 
														&gBiPlotDataSpecsPtr->lineStart,
														0);
				
				numberVectors = 1;
				if (gBiPlotDataSpecsPtr->projectFlag)     
					numberVectors += gProjectInfoPtr->numberStatisticsClasses;
          
				#if defined multispec_mac 
					graphViewCPtr = CreateGraphWindow ();
				#endif

				#if defined multispec_win
					CMultiDocTemplate* graphDocTemplatePtr =
						((CMultiSpecApp*)AfxGetApp ())->GetGraphDocTemplate ();
					CMGraphDoc* graphDocCPtr =
						(CMGraphDoc*)graphDocTemplatePtr->OpenDocumentFile (NULL);
					graphViewCPtr = graphDocCPtr->GetGraphViewCPtr ();
				#endif

				#if defined multispec_wx
					wxDocument*  graph_doc = 
											((CMultiSpecApp*)wxTheApp)->ActivateListDataView ();
					graphViewCPtr = ((CMGraphDoc*)graph_doc)->GetGraphViewCPtr ();
          
							// Assign window ID for Biplot window
				
					((wxWindow*)graphViewCPtr)->SetId (GR_BIPLOT);
				#endif
				
				continueFlag = (graphViewCPtr != NULL);
		
				if (continueFlag)
					continueFlag = graphViewCPtr->FinishGraphRecordSetUp (
																	(SInt16*)channelsPtr,
																	2,
																	(SInt32)totalNumberPixels,
																	numberVectors,
																	1,
																	gImageFileInfoPtr->dataTypeCode,
																	gImageFileInfoPtr->dataTypeCode);
				
				if (continueFlag)
					{
					gGraphRecordPtr = GetGraphRecordPtr (
													graphViewCPtr, &handleStatus, &graphRecHandle);
										
							// Set the code to draw all available plots.
					
					gGraphRecordPtr->drawGraphCode = gGraphRecordPtr->graphCodesAvailable;
					
					}
				
				}	// end "if (continueFlag)" 
				
			#if defined multispec_wx
				if (gActiveImageWindow != NULL)
					gActiveImageWindow->m_frame->Update ();
			#endif
			if (continueFlag &&
						(gBiPlotDataSpecsPtr->displayPixelCode & kDisplayPixels))
				{
				if (gBiPlotDataSpecsPtr->featureTransformationFlag)
					{
							// Get the transformation matrix that will be used for		
							// this set of features.												
							
					MHLock (gTransformationMatrix.eigenVectorHandle);
					
					HDoublePtr eigenVectorPtr = (HDoublePtr)GetHandlePointer (
										gTransformationMatrix.eigenVectorHandle,
										kLock);
										
					ReduceMatrix (eigenVectorPtr, 
										gTransformationMatrix.eigenVectorPtr,
										gTransformationMatrix.numberFeatures,
										gTransformationMatrix.numberChannels,
										2,
										gBiPlotDataSpecsPtr->axisFeaturePtr,
										(gTransformationMatrix.createdByCode >= 16));

					CheckAndUnlockHandle (gTransformationMatrix.eigenVectorHandle);

					}	// end "if (...->featureTransformationFlag)" 
						
						// Intialize the nextTime variable to indicate when the next 		
						// check should occur for a command-.										
						
				gNextTime = gNextStatusTime = TickCount ();
		
				if (gBiPlotDataSpecsPtr->plotDataCode & kAreaType)
					{
              LoadDItemStringNumber (kBiPlotStrID,
												IDS_BiPlot1,
												gStatusDialogPtr, 
												IDC_Status11, 
												(Str255*)gTextString);
                  
					HideStatusDialogItemSet (kStatusClassA);
					HideStatusDialogItemSet (kStatusField);
					
							// This is placed here because a different io buffer set may
							// be needed for the project classes if the active image is not
							// the project base image.
					
					continueFlag = GetIOBufferPointers (
												&gFileIOInstructions[0],
												gImageWindowInfoPtr,
												gImageLayerInfoPtr,
												gImageFileInfoPtr,
												&gInputBufferPtr, 
												&gOutputBufferPtr,
												1,
						 						gImageWindowInfoPtr->maxNumberColumns,
						 						gBiPlotDataSpecsPtr->columnInterval,
						 						gBiPlotDataSpecsPtr->numberChannels,
												//(HUInt16Ptr)*channelsPtr,
                                    channelsPtr,
												kPackData,
												kForceBISFormat,
												kForceReal8Bytes,
												kDoNotAllowForThreadedIO,
												&fileIOInstructionsPtr);
				
					if (continueFlag)
						continueFlag = BiPlotFieldData (fileIOInstructionsPtr, 0, -1);
				
							// Dispose of the IO buffer.		
												
					DisposeIOBufferPointers (fileIOInstructionsPtr,
														&gInputBufferPtr, 
														&gOutputBufferPtr);
					
					}	// end "if (...->plotDataCode & kArea)" 
				
				if (continueFlag && 
					gBiPlotDataSpecsPtr->plotDataCode & (kTrainingType+kTestingType))
					{
               LoadDItemStringNumber (kBiPlotStrID, 
												IDS_BiPlot2,	
												gStatusDialogPtr, 
												IDC_Status11,
												(Str255*)gTextString);
                              
					ShowStatusDialogItemSet (kStatusClassA);
					ShowStatusDialogItemSet (kStatusField);
					
					continueFlag = BiPlotProjectData ();
					
					FillVectorOffsets (gGraphRecordPtr);
					
					}	// end "if (...->plotDataCode & (kTrainingType...)" 
					
				}	// end "if (continueFlag && (gBiPlotDataSpecsPtr->..." 

			HideStatusDialogItemSet (kStatusLine);
				
			if (continueFlag && 
						(gBiPlotDataSpecsPtr->outlineClassCode & kOutlineClasses))
				{
            LoadDItemStringNumber (kBiPlotStrID, 
												IDS_BiPlot3,
												gStatusDialogPtr, 
												IDC_Status11,
												(Str255*)gTextString);
				ShowStatusDialogItemSet (kStatusClassA);
				HideStatusDialogItemSet (kStatusField);

				continueFlag = LoadBiPlotClassStats (gGraphRecordPtr);

				}	// end "if (continueFlag && (gBiPlotDataSpecsPtr->..." 
			
			ReleaseFeatureTransformationMemory ();
			
			if (continueFlag && gBiPlotDataSpecsPtr->plotDataCode != 0)
				{
				GetGraphLabels (gGraphRecordPtr);
			
				continueFlag = CreateGraph (gGraphRecordPtr->graphViewCPtr,
														gGraphRecordPtr, 
														1, 
														1, 
														0, 
														0);
				
				}	// end "if (continueFlag && ...->plotDataCode != 0)"
				
			else	// !continueFlag || gBiPlotDataSpecsPtr->plotDataCode == 0
				{
				if (gGraphRecordPtr != NULL)
					{
					CloseGraphicsWindow (gGraphRecordPtr->window);
					graphRecHandle = NULL;
					
					}	// end "if (gGraphRecordPtr != NULL)"
				
				}	// end "else !continueFlag"
					
					// List end of processor information.			
					
			continueFlag = ListCPUTimeInformation (
													NULL, continueFlag, startTime);
	
					// Scroll output window to the selection and adjust the 			
					// scroll bar.																	
			
			HideStatusDialogItemSet (kStatusClassA);
			HideStatusDialogItemSet (kStatusField);
			UpdateOutputWScrolls (gOutputWindow, 1, kDisplayMessage);
				
					// Dispose of status dialog box.											
			
			CloseStatusDialog (TRUE);
			#if defined multispec_mac
				if (continueFlag)
					SelectWindow (gGraphRecordPtr->window); 
         #endif
					// Turn spin cursor off
		
			gPresentCursor = kWait;
			
			if (graphRecHandle != NULL && handleStatus >= 0)
				CheckAndUnlockHandle (graphRecHandle);
				
			gGraphRecordPtr = NULL;
			
			gOperationCanceledFlag = FALSE;
				
			}	// end "if (BiPlotDataDialog ())" 
			
		}	// end "if (... && LoadBiPlotDataSpecs (activeWindowInfoHandle))" 
		
				// Unlock the structure handles.	
				
	UnlockImageInformationHandles (
							activeImageGlobalHandleStatus, 
							activeWindowInfoHandle);	
	
	if (gBiPlotDataSpecsPtr != NULL)
		{		
		CheckAndUnlockHandle (gBiPlotDataSpecsPtr->channelsHandle);
		CheckAndUnlockHandle (gBiPlotDataSpecsPtr->classHandle);
		gBiPlotDataSpecsPtr->classPtr = NULL;
		CheckAndUnlockHandle (gBiPlotDataSpecsPtr->symbolsHandle);
		
		}	// end "if (gBiPlotDataSpecsPtr != NULL)" 
		
			// Unlock memory for the list data specifications.							
			
	CheckAndUnlockHandle (gNonProjProcessorSpecs.biPlotDataSpecsH);
	
	gBiPlotDataSpecsPtr = NULL;
	
	gImageWindowInfoPtr = NULL;
	gImageFileInfoPtr = NULL;	
	gImageLayerInfoPtr = NULL;	
	
	gThresholdSize = savedThresholdSize;

}	// end "BiPlotDataControl" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean BiPlotDataDialog
//
//	Software purpose:	The purpose of this routine is to obtain the
//							user specifications for the biplots of the image file data.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	True if user selects OK in dialog box
//							False if user selects Cancel in dialog box
//
// Called By:			BiPlotDataControl ()
//
//	Coded By:			Larry L. Biehl			Date: 03/08/1994
//	Revised By:			Larry L. Biehl			Date: 02/28/2018
	
Boolean BiPlotDataDialog (void)
	
{
	Boolean								returnFlag = TRUE;
	
	#if defined multispec_mac
		DialogSelectArea					dialogSelectArea;
									
		Rect									theBox,
												weightsPopUpBox;
		
		double								thresholdPercent,
												saveThresholdPercent;
											
		DialogPtr							dialogPtr;
		float*								localClassWeightsPtr;
		float**								localClassWeightsPtrPtr;
		
		UInt16*								localClassPtr;
												
		unsigned char*						localSymbolsPtr;
		
		Handle								okHandle,
												theHandle;
		
		SInt32								maxChannelFeatureNum,
												theNum;
												
		UInt32								localNumberClasses;
		
		SInt16								controlValue,
												displayPixelCode,
												entireIconItem,
												index,
												itemHit,
												itemHit2,
												outlineClassCode,
												plotDataCode,
												theType,
												newXAxisFeature,
												newYAxisFeature;
		
		Boolean								checkChannelStatisticsFlag,
												checkClassesPopUpFlag,
												checkFeatureTransformFlag,
												createNewGraphicsWindowFlag,
												featureTransformAllowedFlag,
												featureTransformationFlag,
												modalDone,
												thresholdFlag;
		
		
		dialogPtr = NULL;
		localClassWeightsPtr = NULL;
		checkChannelStatisticsFlag = FALSE;
		checkClassesPopUpFlag = FALSE;
		localClassPtr = NULL;
		localSymbolsPtr = NULL;
		
				// Get memory for local vectors
		
		localClassWeightsPtrPtr = NULL;
		if (gBiPlotDataSpecsPtr->projectFlag)
			localClassWeightsPtrPtr = &localClassWeightsPtr;
		
		if (gBiPlotDataSpecsPtr->projectFlag)
			returnFlag = GetDialogLocalVectors (NULL,
															NULL,
															&localClassPtr,
															NULL,
															localClassWeightsPtrPtr,
															&localSymbolsPtr,
															NULL,
															NULL);
		
				// Get the modal dialog for the biplot data specification.
		
		if (returnFlag)			
			dialogPtr = LoadRequestedDialog (kBiPlotDataSpecificationID, 
															kCopyScrap,
															1,
															2);
			
		if (dialogPtr == NULL)
			{		
			ReleaseDialogLocalVectors (NULL,
												NULL,
												localClassPtr,
												NULL,
												localClassWeightsPtr,
												localSymbolsPtr,
												NULL,
												NULL);					
																					return (FALSE);
																					
			}	// end "if (dialogPtr == NULL)" 
		
		if (gBiPlotDataSpecsPtr->projectFlag)
			{
			SetDialogItemDrawRoutine (dialogPtr, 37, gDrawSymbolsPopUpPtr);
			SetDialogItemDrawRoutine (dialogPtr, 39, &weightsPopUpBox, gDrawWeightsPopUpPtr);
			SetDialogItemDrawRoutine (dialogPtr, 42, gDrawDialogClassPopUpPtr);
			
			}	// end "if (gBiPlotDataSpecsPtr->projectFlag)"
			
		BiPlotDataDialogInitialize (dialogPtr,
												gBiPlotDataSpecsPtr,
												&dialogSelectArea,
												gImageWindowInfoPtr,
												gActiveImageWindow,
												localClassPtr,
												localSymbolsPtr,
												&newXAxisFeature,
												&newYAxisFeature,
												&featureTransformationFlag,
												&featureTransformAllowedFlag,
												&checkFeatureTransformFlag,
												&maxChannelFeatureNum,
												&plotDataCode,
												&displayPixelCode,
												&outlineClassCode,
												&thresholdFlag,
												&saveThresholdPercent,
												&gClassSelection,
												&localNumberClasses,
												&gSymbolSelection,
												&gWeightsSelection,
												&createNewGraphicsWindowFlag);
		
				// Save handle for the OK button for use later.								
				
		GetDialogItem (dialogPtr, 1, &theType, &okHandle, &theBox);
		
				// Default feature for x and y axes.											
				
		LoadDItemValue (dialogPtr, 6, (SInt32)newXAxisFeature);
		LoadDItemValue (dialogPtr, 8, (SInt32)newYAxisFeature);
		
				// Set check boxes for "pixels to plot".										
				
		controlValue = (plotDataCode & kTrainingType)  ? 1 : 0;
		SetDLogControl (dialogPtr, 12, controlValue);
		
		controlValue = (plotDataCode & kTestingType)  ? 1 : 0;
		SetDLogControl (dialogPtr, 13, controlValue);
		
		controlValue = (plotDataCode & kAreaType)  ? 1 : 0;
		SetDLogControl (dialogPtr, 14, controlValue);
		
		controlValue = (displayPixelCode & kDisplayPixels)  ? 1 : 0;
		SetDLogControl (dialogPtr, 26, controlValue);
																
				// Initialize entireIconItem value for later use.	
		
		entireIconItem = 15;
		if (gAppearanceManagerFlag)
			entireIconItem = 44;

				// Hide some of the area selection boxes if needed.						
				
		if (!(plotDataCode & kAreaType))
			{
			HideDialogItem (dialogPtr, entireIconItem);
			HideDialogItem (dialogPtr, 17);
			HideDialogItem (dialogPtr, 19);
			HideDialogItem (dialogPtr, 20);
			HideDialogItem (dialogPtr, 22);
			HideDialogItem (dialogPtr, 23);
			
			}	// end "if (!(plotDataCode & kAreaType))" 
		
		controlValue = (outlineClassCode & kOutlineClasses)  ? 1 : 0;
		SetDLogControl (dialogPtr, IDC_OutlineClassAsEllipse, controlValue);
		
		LoadDItemRealValue (dialogPtr, IDC_ThresholdLevel, saveThresholdPercent, 10);
			
		SetDLogControl (dialogPtr, IDC_ThresholdPixelCheck, thresholdFlag);

				// Center the dialog and then show it.											
				
		ShowDialogWindow (dialogPtr, kBiPlotDataSpecificationID, kSetUpDFilterTable);
		
		gDialogItemDescriptorPtr[33] = kDItemReal;
		
				// Set default text selection to first edit text item						
				
		SelectDialogItemText (dialogPtr, 6, 0, INT16_MAX);
		modalDone = FALSE;
		itemHit = 0;
		do
			{
			ModalDialog (gProcessorDialogFilterPtr, &itemHit);
		
					// If itemHit was a number item, check for bad values.  If			
					// itemHit was a radio button make appropriate control settings	
					// to indicate to the user the present selection.  Get the handle	
					// to the itemHit.  For number value items, get the string and		
					// convert it to a number.														

			GetDialogItem (dialogPtr, itemHit, &theType, &theHandle, &theBox);
			if (theType == 16)
				{
				GetDialogItemText (theHandle, gTextString);	
				StringToNum (gTextString, &theNum);
				
				}	// end "if (theType == 16)" 

			if (itemHit > 2)
				{
				switch (itemHit)
					{
					case 6:		//	 Channel/feature for horizontal axis  		
						if (theNum == 0)
							NumberErrorAlert (newXAxisFeature, dialogPtr, itemHit);
						
						else	// theNum > 0 
							{
							if (theNum > maxChannelFeatureNum)
								NumberDefault (theHandle, newXAxisFeature, (Str255*)gTextString);
								
							else	// theNum > 0 && < ...maxChannelFeatureNum 
								{
								newXAxisFeature = theNum;
								checkChannelStatisticsFlag = TRUE;
								
								}
								
							}	// end "else theNum > 0" 
						break;
						
					case 8:		//	 Channel/feature for vertical axis  		
						if (theNum == 0)  
							NumberErrorAlert (newYAxisFeature, dialogPtr, itemHit);
						
						else	// theNum > 0 
							{
							if (theNum > maxChannelFeatureNum)  
								NumberDefault (theHandle, newYAxisFeature, (Str255*)gTextString);
								
							else	// theNum > 0 && < ...maxChannelFeatureNum 
								{
								newYAxisFeature = theNum;
								checkChannelStatisticsFlag = TRUE;
								
								}
								
							}	// end "else theNum > 0" 
						break;
						
					case 9:		// check box for "Use feature transformation flag" 
						ChangeDLogCheckBox ((ControlHandle)theHandle);
						featureTransformationFlag = !featureTransformationFlag;
						checkFeatureTransformFlag = TRUE;
						break;
						
					case 11:		// Create new graphic window. 
						ChangeDLogCheckBox ((ControlHandle)theHandle);
						break;
						
					case 12:		// check box for "Training" 
					case 13:		// check box for "Test" 
						index = itemHit - 11;
						if (plotDataCode & index)
							{
							SetControlValue ((ControlHandle)theHandle, 0);
							plotDataCode -= index;
							
							}	// end "if (plotDataCode & index)" 
							
						else	// !(plotDataCode & index) 
							{
							SetControlValue ((ControlHandle)theHandle, 1);
							plotDataCode += index;
							
							}	// end "else !(plotDataCode & index)" 

						checkClassesPopUpFlag = TRUE;
						break;
						
					case 14:		// Use pixels within selected area. 
						if (plotDataCode & kAreaType)
							{
							SetControlValue ((ControlHandle)theHandle, 0);
							HideDialogItem (dialogPtr, entireIconItem);
							HideDialogItem (dialogPtr, 17);
							HideDialogItem (dialogPtr, 19);
							HideDialogItem (dialogPtr, 20);
							HideDialogItem (dialogPtr, 22);
							HideDialogItem (dialogPtr, 23);
							
							plotDataCode -= kAreaType;
							
							}	// end "if (GetControlValue ((ControlHandle)..." 
							
						else	// !GetControlValue ((ControlHandle)theHandle) 
							{
							SetControlValue ((ControlHandle)theHandle, 1);
							ShowDialogItem (dialogPtr, entireIconItem);
							ShowDialogItem (dialogPtr, 17);
							ShowDialogItem (dialogPtr, 19);
							ShowDialogItem (dialogPtr, 20);
							ShowDialogItem (dialogPtr, 22);
							ShowDialogItem (dialogPtr, 23);
							SelectDialogItemText (dialogPtr, 19, 0, INT16_MAX);
							
							plotDataCode += kAreaType;
							
							}	// end "!GetControlValue ((ControlHandle)theHandle)" 
						break;
						
					case 15:				// Entire area to selected area switch.	
					case 44:				// Entire area to selected area switch. (Appearance Manager)		
					case 19:				//	 lineStart  
					case 20:				//	 lineEnd  
					case 21:				//	 lineInterval  		
					case 22:				//	 columnStart  
					case 23:				//	 columnEnd  
					case 24:				//	 columnInterval  	
						DialogLineColumnHits (&dialogSelectArea,
														dialogPtr, 
														itemHit,
														theHandle,
														theNum);
						break;
						
					case 26:		// check box for "Display pixels as:" 
						ChangeDLogCheckBox ((ControlHandle)theHandle);
						
						if (GetControlValue ((ControlHandle)theHandle))
							displayPixelCode |= kDisplayPixels;
													
						else	// !GetControlValue (theHandle) 
							displayPixelCode &= (kPixelsAsSymbols+kPixelsAsColor);

						break;
						
					case 29:		// check box for "Outline classes with:" 
						ChangeDLogCheckBox ((ControlHandle)theHandle);
						
						if (GetControlValue ((ControlHandle)theHandle))
							outlineClassCode |= kOutlineClasses;

						else	// !GetControlValue (theHandle) 
							outlineClassCode &= kClassesAsEllipses+kClassesAsBorders;

						checkClassesPopUpFlag = TRUE;

						break;
						
					case 33:				// Threshold percent	
						thresholdPercent = GetDItemRealValue (dialogPtr, 33);
						if (thresholdPercent > kMaxThreshold)
							RealNumberErrorAlert (saveThresholdPercent, dialogPtr, 33, 10);
						if (thresholdPercent >= 0 && thresholdPercent <= kMaxThreshold)
							saveThresholdPercent = thresholdPercent;
						break;
						
					case 35:		// Threshold the results. 
						ChangeDLogCheckBox ((ControlHandle)theHandle);
						break;
						
					case 37:		// Symbol selection. 
						itemHit2 = StandardPopUpMenu (dialogPtr, 
																	36,
																	37, 
																	gPopUpSymbolsMenu, 
																	gSymbolSelection, 
																	kPopUpSymbolsMenuID);
														
						if (itemHit2 == kUserMenuItem)
							{
									// User specified symbols to be used.
									// Note that all project classes will be included in the symbol list
									// even if the user is working with a subset of the classes					
									
							HiliteControl ((ControlHandle)okHandle, 255);
							
							if (!SymbolsDialog (
											gProjectInfoPtr->numberStatisticsClasses,
											NULL,
											localSymbolsPtr,
											FALSE))
								itemHit2 = gSymbolSelection;
								
							HiliteControl ((ControlHandle)okHandle, 0);
								
							}	// end "if (itemHit2 == kUserMenuItem)" 
						
						if (itemHit2 != 0)
							gSymbolSelection = itemHit2;
		
								// Make certain that the correct label is drawn in the	
								// symbols pop up box.												
						
						InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
						break;
						
					case 39:		// Use equal weights 
						itemHit = StandardPopUpMenu (dialogPtr, 
																38,
																39, 
																gPopUpWeightsMenu, 
																gWeightsSelection, 
																kPopUpWeightsMenuID);
														
						if (itemHit == kUnequalWeightMenuItem)
							{
									// Unequal weights to be used.								
									
							HiliteControl ((ControlHandle)okHandle, 255);
							
							gWeightsSelection = ClassWeightsDialog (
									localNumberClasses,
									(SInt16*)localClassPtr,
									localClassWeightsPtr,
									gWeightsSelection, 
									gProjectInfoPtr->covarianceStatsToUse == kEnhancedStats);
									
							HiliteControl ((ControlHandle)okHandle, 0);
							
							itemHit = gWeightsSelection;
								
							}	// end "if (itemHit == kUnequalWeightMenuItem)" 
						
						if (itemHit != 0)
							gWeightsSelection = itemHit;
		
								// Make certain that the correct label is drawn in the	
								// weights pop up box.												
						
						InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
						break;
						
					case 42:		// Classes 
						itemHit = StandardPopUpMenu (dialogPtr, 
																41,
																42, 
																gPopUpAllSubsetMenu, 
																gClassSelection, 
																kPopUpAllSubsetMenuID);
														
						if (itemHit == kSubsetMenuItem)
							{
									// Subset of classes to be used.								
									
							itemHit = ClassDialog (
												&localNumberClasses,
												(SInt16*)localClassPtr, 
												1,
												gProjectInfoPtr->numberStatisticsClasses,
												gClassSelection,
												okHandle);
								
							}	// end "if (itemHit == kSubsetMenuItem)" 
						
						if (itemHit != 0)
							gClassSelection = itemHit;
						
						if (gClassSelection == kAllMenuItem)
										// Make sure that the class settings are setup for all
										// classes in case they will be used by the symbols
										// or weights popup menu.
										
							LoadTrainClassVector (&localNumberClasses,
															&gClassSelection, 
															(SInt16*)localClassPtr);
		
								// Make certain that the correct label is drawn in the	
								// class pop up box.													
						
						InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
						break;
							
					}	// end "switch (itemHit)" 
					
				if (checkFeatureTransformFlag)
					{
					checkFeatureTransformFlag = BiPlotDataDialogCheckFeatureTransform (
																	dialogPtr,
																	gImageWindowInfoPtr,
																	&gTransformationMatrix,
																	featureTransformAllowedFlag,
																	&featureTransformationFlag,
																	&maxChannelFeatureNum);
					/*
					checkFeatureTransformFlag = CheckFeatureTransformationDialog (
																dialogPtr, 
																featureTransformAllowedFlag,
																9, 
																4, 
																&featureTransformationFlag);
																
					if (featureTransformationFlag)
						maxChannelFeatureNum = gTransformationMatrix.numberFeatures;
						
					else	// !featureTransformationFlag 
						maxChannelFeatureNum = gImageWindowInfoPtr->totalNumberChannels;
					*/					
					}	// end "if (checkFeatureTransformFlag)" 
																
				if (checkChannelStatisticsFlag)
					{
					if (gBiPlotDataSpecsPtr->projectFlag)
						{
						if (CheckIfStatisticsChannel (theNum, featureTransformationFlag))
							SetDLogControlHilite (dialogPtr, 29, 0);
							
						else	// !CheckIfStatisticsChannel (... 
							{
							outlineClassCode &= (kClassesAsEllipses + kClassesAsBorders);
							SetDLogControl (dialogPtr, 29, 0);
							SetDLogControlHilite (dialogPtr, 29, 255);
						
							}	// end "else !CheckIfStatisticsChannel (theNum, ..." 
							
						}	// end "if (gBiPlotDataSpecsPtr->projectFlag)" 
						
					checkChannelStatisticsFlag = FALSE;
						
					}	// end "if (checkChannelStatisticsFlag && ...)" 

				if (checkClassesPopUpFlag)
					{
					BiPlotDataDialogHideShowClassItems (dialogPtr, plotDataCode, outlineClassCode);
					checkClassesPopUpFlag = FALSE;

					}	// end "if (checkClassesPopUpFlag)" 

				}	// end "if (itemHit > 2)" 
				
			else if (itemHit > 0) 	// and itemHit <= 2 
				{
				if (itemHit == 1 && (plotDataCode == 0))
					{
					HiliteControl ((ControlHandle)okHandle, 255);
					DisplayAlert (kErrorAlertID, 
										kStopAlert, 
										kAlertStrID, 
										IDS_Alert117,
										0, 
										NULL);
					HiliteControl ((ControlHandle)okHandle, 0);
										
					itemHit = 0;
					//updateDialogWindowFlag = TRUE;
											
					}	// end "if (itemHit == 1 && ..."
					
				if (itemHit == 1 && 
							!((displayPixelCode & kDisplayPixels) ||
														(outlineClassCode & kOutlineClasses)))
					{
					HiliteControl ((ControlHandle)okHandle, 255);
					DisplayAlert (kErrorAlertID, 
										kStopAlert, 
										kAlertStrID, 
										IDS_Alert151,
										0, 
										NULL);
					HiliteControl ((ControlHandle)okHandle, 0);
										
					itemHit = 0;
					//updateDialogWindowFlag = TRUE;
											
					}	// end "if (itemHit == 1 && ..."
				
				if (itemHit == 1)
					{	
							// Make certain that the channel or feature number make sense.	
							
					if (newXAxisFeature > maxChannelFeatureNum)
						{
						itemHit = 6;
						NumberErrorAlert (newXAxisFeature, dialogPtr, itemHit);
						
						}	// end "if (newXAxisFeature > maxChannelFeatureNum)" 
					
					else	// newXAxisFeature <= maxChannelFeatureNum 
						{
						if (newYAxisFeature > maxChannelFeatureNum)
							{
							itemHit = 8;
							NumberErrorAlert (newYAxisFeature, dialogPtr, itemHit);
							
							}	// end "if (newYAxisFeature > maxChannelFeatureNum)" 
							
						}	// end "else newXAxisFeature <= maxChannelFeatureNum" 
						
					}	// end "if (itemHit == 1)"
					
						// If item hit is 1, check if line-column values make 			
						// sense.  If they don't, sound an alert and make item hit 		
						// equal to 0 to allow user to make changes.							
						
				if (itemHit == 1 && (plotDataCode & kAreaType))
					itemHit = CheckLineColValues (
											&dialogSelectArea,
											gBiPlotDataSpecsPtr->lineStart, 
											gBiPlotDataSpecsPtr->columnStart,
											dialogPtr);
					
				if (itemHit == 1)
					{
					modalDone = TRUE;
					
					dialogSelectArea.lineInterval = GetDItemValue (dialogPtr, 21);
					dialogSelectArea.columnInterval = GetDItemValue (dialogPtr, 24);
					
					BiPlotDataDialogOK (dialogPtr,
												gBiPlotDataSpecsPtr,
												&dialogSelectArea,
												newXAxisFeature,
												newYAxisFeature,
												featureTransformationFlag,
												GetDLogControl (dialogPtr, 11),
												plotDataCode,
												displayPixelCode,
												outlineClassCode,
												saveThresholdPercent,
												GetDLogControl (dialogPtr, 35),
												gClassSelection,
												localNumberClasses,
												localClassPtr,
												gSymbolSelection,
												localSymbolsPtr,
												gWeightsSelection,
												localClassWeightsPtr);

					returnFlag = TRUE;

					}	// end if (itemHit == 1) 
				
				if (itemHit == 2)
					{
					modalDone = TRUE;
					returnFlag = FALSE;
					
					}	// end "if (itemHit == 2)" 
					
				}	// end "else if (itemHit > 0) and itemHit <= 2" 
					
			}	while (!modalDone);
			
		ReleaseDialogLocalVectors (NULL,
												NULL,
												localClassPtr,
												NULL,
												localClassWeightsPtr,
												localSymbolsPtr,
												NULL,
												NULL);
			
		//localClassWeightsPtr = CheckAndDisposePtr (localClassWeightsPtr);
			
		CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
	#endif	// defined multispec_mac

	#if defined multispec_win
		CMBiPlotDialog* dialogPtr = NULL;
		
		TRY
			{ 
			dialogPtr = new CMBiPlotDialog ();
			returnFlag = dialogPtr->DoDialog ();
			delete dialogPtr;
			}
			
		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			returnFlag = FALSE;
			}
		END_CATCH_ALL
	#endif	// defined multispec_win  
   
	#if defined multispec_wx
		CMBiPlotDialog*		dialogPtr = NULL;
		
		try
			{
			dialogPtr = new CMBiPlotDialog (NULL);
			
			returnFlag = dialogPtr->DoDialog ();
		                       
			delete dialogPtr;
			}
      catch (int e)
			{
			MemoryMessage (0, kObjectMessage);
			returnFlag = FALSE;
			}
	#endif
   
	return (returnFlag);

}	// end "BiPlotDataDialog" 



void BiPlotDataDialogInitialize (
				DialogPtr							dialogPtr,
				BiPlotDataSpecsPtr				biPlotDataSpecsPtr,
				DialogSelectArea*					dialogSelectAreaPtr,
				WindowInfoPtr						imageWindowInfoPtr,
				WindowPtr							activeImageWindow,
				UInt16*								localClassPtr,
				unsigned char*						localSymbolsPtr,
				SInt16*								newXAxisFeaturePtr,
				SInt16*								newYAxisFeaturePtr,
				Boolean*								featureTransformationFlagPtr,
				Boolean*								featureTransformAllowedFlagPtr,
				Boolean*								checkFeatureTransformFlagPtr,
				SInt32*								maxChannelFeatureNumPtr,
				SInt16*								plotDataCodePtr,
				SInt16*								displayPixelCodePtr,
				SInt16*								outlineClassCodePtr,
				Boolean*								thresholdFlagPtr,
				double*								saveThresholdPercentPtr,
				SInt16*								classSelectionPtr,
				UInt32*								localNumberClassesPtr,
				SInt16*								symbolSelectionPtr,
				SInt16*								weightsSelectionPtr,
				Boolean*								createNewGraphicsWindowFlagPtr)


{	
	SInt16								entireIconItem,
											fieldTypesPresentCode;
										
	UInt16								numberEigenvectors;
	
		
			// Load the dialog local vectors

	if (biPlotDataSpecsPtr->projectFlag)
		LoadDialogLocalVectors (NULL,
										NULL,
										NULL,
										NULL,
										localClassPtr,
										biPlotDataSpecsPtr->classHandle,
										biPlotDataSpecsPtr->numberClasses,
										NULL,
										NULL,
										0,
										localSymbolsPtr,
										biPlotDataSpecsPtr->symbolsHandle,
										gProjectInfoPtr->numberStatisticsClasses,
										NULL,
										NULL,
										0,
										NULL);
				
			// Initialize selected area structure.	
	
	entireIconItem = 15;
	if (gAppearanceManagerFlag)
		entireIconItem = 44;
	
	InitializeDialogSelectArea (dialogSelectAreaPtr,
											imageWindowInfoPtr,
											activeImageWindow,
											biPlotDataSpecsPtr->columnStart,
											biPlotDataSpecsPtr->columnEnd,
											biPlotDataSpecsPtr->columnInterval,
											biPlotDataSpecsPtr->lineStart,
											biPlotDataSpecsPtr->lineEnd,
											biPlotDataSpecsPtr->lineInterval,
											19,
											entireIconItem,
											kDontAdjustToBaseImage);	
											
			// Default feature for x and y axes.											
			
	*newXAxisFeaturePtr = biPlotDataSpecsPtr->axisFeaturePtr[0] + 1;
	*newYAxisFeaturePtr = biPlotDataSpecsPtr->axisFeaturePtr[1] + 1;
	
			// Set check box for "use feature transformation".							
			
	EigenvectorInfoExists (
					biPlotDataSpecsPtr->projectFlag, NULL, &numberEigenvectors);
					
	*featureTransformationFlagPtr = biPlotDataSpecsPtr->featureTransformationFlag;
	*featureTransformAllowedFlagPtr = (numberEigenvectors > 0);
	
	*checkFeatureTransformFlagPtr = BiPlotDataDialogCheckFeatureTransform (
													dialogPtr,
													imageWindowInfoPtr,
													&gTransformationMatrix,
													*featureTransformAllowedFlagPtr,
													featureTransformationFlagPtr,
													maxChannelFeatureNumPtr);

			// Option to create a new graphics window.  This option has not been	
			// implemented yet.																	

	*createNewGraphicsWindowFlagPtr = biPlotDataSpecsPtr->createNewGraphicsWindowFlag;

	HideDialogItem (dialogPtr, IDC_CreateNewGraphWindow);

			// Setup the pixels to plot.														
			
	*plotDataCodePtr = biPlotDataSpecsPtr->plotDataCode;
				
	if (biPlotDataSpecsPtr->projectFlag)
		{
				// If only cluster fields are available, pixels cannot be 
				// plotted. Don't know where the pixels come from.
				
		fieldTypesPresentCode = DetermineFieldTypes ();
		
		if (gProjectInfoPtr->numberStatTrainFields <= 0 ||
														fieldTypesPresentCode == 4)
			SetDLogControlHilite (dialogPtr, IDC_Training, 255);
		
		if (gProjectInfoPtr->numberStatTestFields <= 0 ||
														fieldTypesPresentCode == 4)
			SetDLogControlHilite (dialogPtr, IDC_TestAreas, 255);
					
		}	// end "if (gBiPlotDataSpecsPtr->projectFlag)" 
		
	else
		{		// !biPlotDataSpecsPtr->projectFlag 
		HideDialogItem (dialogPtr, IDC_Training);
		HideDialogItem (dialogPtr, IDC_TestAreas);

		}

			// To entire image icon.															
			// Default selected area.															
		
	#if defined multispec_mac 
		if (gAppearanceManagerFlag)
			HideDialogItem (dialogPtr, 15);
		else	// !gAppearanceManagerFlag
			HideDialogItem (dialogPtr, 44);
	#endif
		
	LoadLineColumnItems (dialogSelectAreaPtr, 
									dialogPtr, 
									kInitializeLineColumnValues, 
									kIntervalEditBoxesExist,
									1);

			// Set check box for "Display pixels as:".									
			// Set radio button for "Display pixels as symbols".						
			// Set radio button for "Display pixels as color".							
			// color image is not used for now.			
			
	*displayPixelCodePtr = biPlotDataSpecsPtr->displayPixelCode;
	
	if (!biPlotDataSpecsPtr->projectFlag)
		{
		*displayPixelCodePtr = kDisplayPixels + kPixelsAsSymbols + kPixelsAsColor;
		
		SetDLogControlHilite (dialogPtr, IDC_DisplayPixels, 255);
		
		}	// end "if (!biPlotDataSpecsPtr->projectFlag)"	

	#if defined multispec_mac
				// Hide items in the Mac version of the dialog box.
		HideDialogItem (dialogPtr, IDC_Symbols);
		HideDialogItem (dialogPtr, IDC_Color);
		HideDialogItem (dialogPtr, IDC_EllipsePrompt);
		HideDialogItem (dialogPtr, IDC_Border);
	#endif
	
			// Set check box for "Outline classes as:".									
			// Set radio button for "Outline classes as ellipses".					
			// Set radio button for "Outline classes as borders".						
			
	*outlineClassCodePtr = biPlotDataSpecsPtr->outlineClassCode;

			// If the requested channels to be plotted do not have statistics		
			// available for them then turn this item off.								
		
	if (!CheckIfStatisticsChannel (*newXAxisFeaturePtr, *featureTransformationFlagPtr) || 
			!CheckIfStatisticsChannel (*newYAxisFeaturePtr, *featureTransformationFlagPtr))
		{
		*outlineClassCodePtr &= (kClassesAsEllipses + kClassesAsBorders);
		SetDLogControlHilite (dialogPtr, IDC_OutlineClassAsEllipse, 255);
		
		}	// end "if (!CheckIfStatisticsChannel f ((..." 
	
			// Set threshold options.  														
		
	*saveThresholdPercentPtr = biPlotDataSpecsPtr->probabilityThreshold;
		
			// The threshold flag is not currently used.
	
	*thresholdFlagPtr = biPlotDataSpecsPtr->thresholdFlag;
	//SetDLogControl (dialogPtr,
	//						IDC_ThresholdPixelCheck,
	//						biPlotDataSpecsPtr->thresholdFlag);
	HideDialogItem (dialogPtr, IDC_ThresholdPixelCheck);
	
			// Symbols to use.																	
			
	*symbolSelectionPtr = biPlotDataSpecsPtr->symbolSet;

	if (!biPlotDataSpecsPtr->projectFlag)
		{
		HideDialogItem (dialogPtr, IDC_OutlineClassAsEllipse);
		HideDialogItem (dialogPtr, IDC_ThresholdLevelPrompt);
		HideDialogItem (dialogPtr, IDC_ThresholdLevel);
		HideDialogItem (dialogPtr, IDC_PercentPrompt);
		HideDialogItem (dialogPtr, IDC_ThresholdPixelCheck);
		HideDialogItem (dialogPtr, IDC_ClassPrompt);
		HideDialogItem (dialogPtr, IDC_ClassCombo);
		HideDialogItem (dialogPtr, IDC_SymbolPrompt);
		HideDialogItem (dialogPtr, IDC_SymbolCombo);
		HideDialogItem (dialogPtr, IDC_CreateNewGraphWindow);
		HideDialogItem (dialogPtr, IDC_Training);
		HideDialogItem (dialogPtr, IDC_TestAreas);
				
		}	// end "if (!biPlotDataSpecsPtr->projectFlag)"

			// Class weights.																		
			// Not used for now.																	
			
	HideDialogItem (dialogPtr, IDC_WeightsPrompt);
	HideDialogItem (dialogPtr, IDC_WeightsCombo);

	*weightsSelectionPtr = kEqualWeights;
	if (biPlotDataSpecsPtr->projectFlag)
		{
		*weightsSelectionPtr = UpdateDialogClassWeightsInfo (
								kEqualWeights,
								gProjectInfoPtr->covarianceStatsToUse == kEnhancedStats,
								NULL,
								TRUE);
		
		}	// end "if (biPlotDataSpecsPtr->projectFlag)" 
						
			// Setup the classes popup.														

	if (!(*plotDataCodePtr & 0x0003) && !(*outlineClassCodePtr & kOutlineClasses))
		{
		HideDialogItem (dialogPtr, IDC_ClassPrompt);
		HideDialogItem (dialogPtr, IDC_ClassCombo);
		
		}
	
			// Classes to use.		

	*classSelectionPtr = gBiPlotDataSpecsPtr->classSet;
	*localNumberClassesPtr = gBiPlotDataSpecsPtr->numberClasses;

}	// end "BiPlotDataDialogInitialize"



void BiPlotDataDialogOK (
				DialogPtr							dialogPtr,
				BiPlotDataSpecsPtr				biPlotDataSpecsPtr,
				DialogSelectArea*					dialogSelectAreaPtr,
				SInt16								newXAxisFeature,
				SInt16								newYAxisFeature,
				Boolean								featureTransformationFlag,
				Boolean								createNewGraphicsWindowFlag,
				SInt16								plotDataCode,
				SInt16								displayPixelCode,
				SInt16								outlineClassCode,
				double								saveThresholdPercent,
				Boolean								thresholdFlag,
				SInt16								classSelection,
				UInt16								localNumberClasses,
				UInt16*								localClassPtr,
				SInt16								symbolSelection,
				unsigned char*						localSymbolsPtr,
				SInt16								weightsSelection,
				float*								classWeightsPtr)

{	
	SInt16*									channelsPtr;
	unsigned char*							symbolsPtr;
	
				
			// Channels																	

	biPlotDataSpecsPtr->axisFeaturePtr[0] = newXAxisFeature - 1;
	biPlotDataSpecsPtr->axisFeaturePtr[1] = newYAxisFeature - 1;
	
	channelsPtr = (SInt16*)GetHandlePointer (
													biPlotDataSpecsPtr->channelsHandle);
   
	LoadChannelsVector (FALSE,
						featureTransformationFlag,
						2, 
						biPlotDataSpecsPtr->axisFeaturePtr, 
						(SInt16*)&biPlotDataSpecsPtr->numberChannels,
						channelsPtr);

			// Feature Transformation												
	 					
	biPlotDataSpecsPtr->featureTransformationFlag = 
														featureTransformationFlag;
														
			// Load any revised class list or symbols into the biplot structure
			
	if (gBiPlotDataSpecsPtr->projectFlag)
		LoadProcessorVectorsFromDialogLocalVectors (0,
																	0,
																	0,
																	0,
																	NULL,
																	NULL,
																	NULL,
																	NULL,
																	NULL,
																	NULL,
																	classSelection,
																	localNumberClasses,
																	(UInt16*)localClassPtr,
																	&biPlotDataSpecsPtr->classSet,
																	&biPlotDataSpecsPtr->numberClasses,
																	biPlotDataSpecsPtr->classHandle,
																	symbolSelection,
																	localSymbolsPtr,
																	&biPlotDataSpecsPtr->symbolSet,
																	biPlotDataSpecsPtr->symbolsHandle,
																	0,
																	NULL,
																	NULL);

	biPlotDataSpecsPtr->createNewGraphicsWindowFlag = createNewGraphicsWindowFlag;
													
			// Pixels to plot.				
	
	biPlotDataSpecsPtr->plotDataCode = plotDataCode; 
				
			// Selected area.															
			
	biPlotDataSpecsPtr->lineStart = dialogSelectAreaPtr->lineStart;
	biPlotDataSpecsPtr->lineEnd = dialogSelectAreaPtr->lineEnd;
	biPlotDataSpecsPtr->lineInterval = dialogSelectAreaPtr->lineInterval;
	biPlotDataSpecsPtr->columnStart = dialogSelectAreaPtr->columnStart;
	biPlotDataSpecsPtr->columnEnd = dialogSelectAreaPtr->columnEnd;
	biPlotDataSpecsPtr->columnInterval = dialogSelectAreaPtr->columnInterval;
													
			// Display pixels as:													
			
	biPlotDataSpecsPtr->displayPixelCode = displayPixelCode;
													
			// Outline classes with:												
		
	biPlotDataSpecsPtr->outlineClassCode = outlineClassCode;
				
			// Get probability threshold.											
	
	biPlotDataSpecsPtr->probabilityThreshold = saveThresholdPercent;

	if (biPlotDataSpecsPtr->projectFlag)
		biPlotDataSpecsPtr->probabilityThresholdCode =
				GetProbabilityThresholdCode (saveThresholdPercent, NULL);
						
			// Threshold results flag.												
			
	biPlotDataSpecsPtr->thresholdFlag = thresholdFlag;
					
			// Class Weights															
			
	if (biPlotDataSpecsPtr->projectFlag)
		UpdateProjectClassWeights (
					gProjectInfoPtr->covarianceStatsToUse == kEnhancedStats,
					weightsSelection,
					classWeightsPtr);
										
			// Class symbols.															
	
	if (biPlotDataSpecsPtr->projectFlag && 
												symbolSelection == kDefaultMenuItem)
		{
		symbolsPtr = (UCharPtr)GetHandlePointer (biPlotDataSpecsPtr->symbolsHandle);
		LoadClassSymbolVector (
					symbolsPtr, 
					IDS_Symbol1, 
					gProjectInfoPtr->numberStatisticsClasses);
					
		}	// end "if (biPlotDataSpecsPtr->projectFlag && ..."
		
	biPlotDataSpecsPtr->symbolSet = symbolSelection;
					
						// Classes																	
				
	biPlotDataSpecsPtr->classSet = classSelection;
				
	if ((plotDataCode & 0x0003) && classSelection == kAllMenuItem)
		LoadTrainClassVector (
				&biPlotDataSpecsPtr->numberClasses,
				&biPlotDataSpecsPtr->classSet, 
				biPlotDataSpecsPtr->classPtr);

}	// end "BiPlotDataDialogOK"



Boolean BiPlotDataDialogCheckFeatureTransform (
				DialogPtr							dialogPtr,
				WindowInfoPtr						imageWindowInfoPtr,
				TransformationSpecs*				transformationMatrixPtr,
				Boolean								featureTransformAllowedFlag,
				Boolean*								featureTransformationFlagPtr,
				SInt32*								maxChannelFeatureNumPtr)


{	
	Boolean								checkFeatureTransformFlag;
	
	
	checkFeatureTransformFlag = CheckFeatureTransformationDialog (
												dialogPtr, 
												featureTransformAllowedFlag,
												IDC_FeatureTransformation,
												IDC_ChannelPrompt,
												featureTransformationFlagPtr);
												
	if (*featureTransformationFlagPtr)
		*maxChannelFeatureNumPtr = transformationMatrixPtr->numberFeatures;
		
	else	// !*featureTransformationFlagPtr 
		*maxChannelFeatureNumPtr = imageWindowInfoPtr->totalNumberChannels;
		
	return (checkFeatureTransformFlag);
					
}	// end "BiPlotDataDialogCheckFeatureTransform"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void BiPlotDataDialogHideShowClassItems
//
//	Software purpose:	The purpose of this routine is to show or hide the class related
//							items in the dialog based on whether training and or test class
//							information is to be included.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/24/2014
//	Revised By:			Larry L. Biehl			Date: 06/24/2014

void BiPlotDataDialogHideShowClassItems (
				DialogPtr							dialogPtr,
				SInt16								plotDataCode,
				SInt16								outlineClassCode)

{ 
	if ((plotDataCode & 0x0003) || (outlineClassCode & kOutlineClasses))
		{
		ShowDialogItem (dialogPtr, IDC_ClassPrompt);
		ShowDialogItem (dialogPtr, IDC_ClassCombo);
		ShowDialogItem (dialogPtr, IDC_SymbolPrompt);
		ShowDialogItem (dialogPtr, IDC_SymbolCombo);
		
		}	// end "if ((plotDataCode & 0x0003) || ..."

	else	// !(plotDataCode & 0x0003) && 
				//							!(outlineClassCode & kOutlineClasses) 
		{
		HideDialogItem (dialogPtr, IDC_ClassPrompt);
		HideDialogItem (dialogPtr, IDC_ClassCombo);
		HideDialogItem (dialogPtr, IDC_SymbolPrompt);
		HideDialogItem (dialogPtr, IDC_SymbolCombo);
		
		}	// end "else !(plotDataCode & 0x0003) && ..."
					
}	// end "BiPlotDataDialogHideShowClassItems"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 BiPlotFieldData
//
//	Software purpose:	The purpose of this routine is to transfer the
//							field data for the requested channels to the output
//							text window or disk file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/08/1994
//	Revised By:			Larry L. Biehl			Date: 02/27/2018

SInt16 BiPlotFieldData (
				FileIOInstructionsPtr			fileIOInstructionsPtr, 
				SInt16								classNumberCode, 
				SInt16								fieldNumber)

{ 
	double								dataVectorPtr[2],
											xValue,
											yValue;
											
	Point									point;
											
//	FileInfoPtr							localFileInfoPtr; 
	
	double								*tempVectorPtr,
											*xBasePtr,
											*yBasePtr;
											
	HDoublePtr							bufferDoublePtr;
	
	HSInt32Ptr							vectorLengthsPtr;
											
	UInt16								*localChannelsPtr;
											
	unsigned char						*inputBufferPtr,
											*inputBufferPtr2;
	
	RgnHandle							rgnHandle;
	
	SInt32								column,
											error,
											index,
											index2,
											line,
											lineEnd,
											lineInterval,
											lineStart,
											numberPointsInClass,
											numberSamples;
	
	UInt32								columnEnd,
											columnInterval,
											columnStart,
											lineCount = 0;
											
	SInt16								errCode,
											localNumberChannels;	
	
	Boolean								continueFlag,
											polygonField,
											updateNumberLinesFlag;
	
	
			// Make certain that input values make sense									
			
	if (fieldNumber < -1)
																							return (0);
	if (gProjectInfoPtr != NULL &&
							fieldNumber >= gProjectInfoPtr->numberStorageFields)
																					 		return (0);
																						
			// Exit if this is a cluster type field.										
			
	if (fieldNumber > -1 && 
		gProjectInfoPtr->fieldIdentPtr[fieldNumber].pointType == kClusterType)
																							return (1);
	
			// Initialize local variables.													
		
	inputBufferPtr = 	(UCharPtr)gInputBufferPtr;
	inputBufferPtr2 = (UCharPtr)gOutputBufferPtr;
	localNumberChannels = gBiPlotDataSpecsPtr->numberChannels;
	
	localChannelsPtr = (UInt16*)GetHandlePointer (
								gBiPlotDataSpecsPtr->channelsHandle);
	
	index = gGraphRecordPtr->xVector.numberPoints;
				
	LockAndGetVectorPointer (&gGraphRecordPtr->xVector, &error);
	
	if (error == noErr)														
		LockAndGetVectorPointer (&gGraphRecordPtr->yVector, &error);
		
	continueFlag = (error == noErr);
																						
	if (continueFlag)
		{
				// Determine if field is described by a polygon or a rectangle.  	
				// If the field is described by a polygon then create a region 	
				// for the field.									
		
		if (fieldNumber >= 0)
			{
			gAreaDescription.lineInterval = gBiPlotDataSpecsPtr->lineInterval;	
			gAreaDescription.columnInterval = gBiPlotDataSpecsPtr->columnInterval;
			ClearAreaDescriptionOffsetVariables (&gAreaDescription);
		
			GetFieldBoundary (gProjectInfoPtr, &gAreaDescription, fieldNumber);
			
			if (gAreaDescription.columnStart == 0)
																							return (1);
			
			}	// end "if (fieldNumber >= 0)" 
			
		else	// if (fieldNumber < 0)  // It is a selected area. 
			{
			InitializeAreaDescription (&gAreaDescription, 
													gBiPlotDataSpecsPtr->lineStart, 
													gBiPlotDataSpecsPtr->lineEnd, 
													gBiPlotDataSpecsPtr->columnStart, 
													gBiPlotDataSpecsPtr->columnEnd, 
													gBiPlotDataSpecsPtr->lineInterval, 
													gBiPlotDataSpecsPtr->columnInterval,
													1,
													1,
													0);
													
			}	// end "else if (fieldNumber < 0)" 
		
		rgnHandle = 		gAreaDescription.rgnHandle;
		polygonField = 	gAreaDescription.polygonFieldFlag;
		lineStart = 		gAreaDescription.lineStart;
		lineEnd = 			gAreaDescription.lineEnd;
		columnStart = 		gAreaDescription.columnStart;
		columnEnd = 		gAreaDescription.columnEnd;
		columnInterval = 	gAreaDescription.columnInterval;
		lineInterval = 	gAreaDescription.lineInterval;
		
		lineCount = 0;
		
		MHLock (gGraphRecordPtr->vectorLengthsHandle);
		vectorLengthsPtr = (SInt32*)GetHandlePointer (
												gGraphRecordPtr->vectorLengthsHandle);
		numberPointsInClass = vectorLengthsPtr[classNumberCode];
		
				// Get point to base of data to start storing new samples at.		
				// Adjust down by one to represent the last sample in the set.		
																									
		xBasePtr = &gGraphRecordPtr->xVector.basePtr[index];						
		yBasePtr = &gGraphRecordPtr->yVector.basePtr[index];
		xBasePtr--;
		yBasePtr--;
		
				// Determine the whether the data are signed values.					
				// First for x-axis and then for y-axis									
				
		//index2 = gImageLayerInfoPtr[localChannelsPtr[0]+1].fileInfoIndex;		
		//localFileInfoPtr = &fileInfoPtr[index2];
				
		//index2 = gImageLayerInfoPtr[localChannelsPtr[1]+1].fileInfoIndex;		
		//localFileInfoPtr = &fileInfoPtr[index2];
			
				// Load some of the File IO Instructions structure that pertain
				// to the specific area being used.
				
		errCode = SetUpFileIOInstructions (fileIOInstructionsPtr,
														&gAreaDescription, 
														localNumberChannels,
														localChannelsPtr,
														kDetermineSpecialBILFlag);
														
		updateNumberLinesFlag = TRUE;
										
				// Loop through the lines for the field.									
		
		for (line=lineStart; line<=lineEnd; line+=lineInterval)
			{
					// Load the line count into the status dialog.						
			
			lineCount++;	
			if (TickCount () >= gNextStatusTime)
				{
				if (updateNumberLinesFlag)
					{
							// Load the total number of lines into the status dialog.			
								
					LoadDItemValue (gStatusDialogPtr, 
											IDC_Status20, 
											(SInt32)(lineEnd-lineStart+lineInterval)/lineInterval);
					updateNumberLinesFlag = FALSE;
											
					}	// end "if (updateNumberLinesFlag)"
					
				LoadDItemValue (gStatusDialogPtr, IDC_Status18, lineCount);
				gNextStatusTime = TickCount () + gNextStatusTimeOffset;
				
				}	// end "if (TickCount () >= gNextStatusTime)"
			
			point.v = (SInt16)line;
			point.h = (SInt16)columnStart;
				
					// Get data for the requested channels for the line of image 	
					// data.  Return if there is a file IO error.						
				 
			if (errCode >= noErr)
				errCode = GetLineOfData (fileIOInstructionsPtr,
													line, 
													columnStart,
													columnEnd,
													columnInterval,
													inputBufferPtr,
													inputBufferPtr2);
			
			if (errCode < noErr)
				{
				continueFlag = FALSE;
				break;
				
				}	// end "if (errCode < noErr)"
				
			if (errCode != kSkipLine)
				{
			   bufferDoublePtr = (HDoublePtr)inputBufferPtr2;
			   
				numberSamples = fileIOInstructionsPtr->numberOutputBufferSamples;
			   
				for (column=1; column<=numberSamples; column++)
					{
					if (!polygonField || PtInRgn (point, rgnHandle))
						{
						if (gBiPlotDataSpecsPtr->featureTransformationFlag)
							{
							tempVectorPtr = gTransformationMatrix.tempMatrixPtr;
							
							for (index2=0; index2<localNumberChannels; index2++)
								{
								*tempVectorPtr = *bufferDoublePtr;
									
								tempVectorPtr++;
								bufferDoublePtr++; 
								
								}	// end "for (index2=0; index2<..." 
						
							MatrixMultiply (
										gTransformationMatrix.tempMatrixPtr, 
										gTransformationMatrix.eigenVectorPtr, 
										dataVectorPtr, 
										1,
										localNumberChannels,
										2,
										kMat1ByTransposeMat2);
										
							xValue = dataVectorPtr[0];
							yValue = dataVectorPtr[1];
										
							}	// end "if (...->featureTransformationFlag)" 
										
						else	// !gBiPlotDataSpecsPtr->featureTransformationFlag 
							{				
							xValue = *bufferDoublePtr;
							bufferDoublePtr++;
							
							yValue = *bufferDoublePtr;
							bufferDoublePtr++;
								
							}	// end "else !...->featureTransformationFlag" 
							
								// Check if duplicated value.										
								
						for (index2=0; index2<numberPointsInClass; index2++)
							{
							if (*xBasePtr == xValue && *yBasePtr == yValue)
								break;

							xBasePtr--;
							yBasePtr--;
							
							}	// end "for (index2=0; index2<..." 
							
						if (index2 == numberPointsInClass)
							{
							SetV (&gGraphRecordPtr->xVector, 
									index, 
									xValue, 
									&error);
								
							SetV (&gGraphRecordPtr->yVector, 
									index, 
									yValue, 
									&error);
							
							numberPointsInClass++;	
							index++;
							
							xBasePtr += numberPointsInClass;
							yBasePtr += numberPointsInClass;
									
							}	// end "if (index2 == numberPointsInClass)" 
							
						else	// index2 < numberPointsInClass 
							{
									// this is a duplicate point, skip it 						
									
							xBasePtr += index2;
							yBasePtr += index2;
							
							}	// end "else index2 < numberPointsInClass" 
						
						}	// end "if (!polygonField || PtInRgn (point, rgnHandle))" 
						
					else	// polygonField && !PtInRgn (point, rgnHandle) 
						bufferDoublePtr += localNumberChannels;
						
					point.h += (SInt16)columnInterval;
						
					if (error != noErr)
						break;
							
					} 		// end "for (column=0; column<..." 
				
				continueFlag = (error == noErr);
					
				}	// end "if (errCode != kSkipLine)" 
				
					// Exit routine if user has "command period" down
				
			if (TickCount () >= gNextTime)
				{
				if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
					continueFlag = FALSE;
					
				}	// end "if (TickCount () >= gNextTime)"
				
			if (!continueFlag)
				break;
			
			if (fileIOInstructionsPtr->maskBufferPtr != NULL)
				fileIOInstructionsPtr->maskBufferPtr += 
													fileIOInstructionsPtr->numberMaskColumnsPerLine;
		      
			}	// end "for (line=lineStart; line<=lineEnd; line+=...)" 
			
		}	// end "if (continueFlag)" 
		
	LoadDItemValue (gStatusDialogPtr, IDC_Status18, lineCount);
		
			// Close up any File IO Instructions structure that pertain to the 
			// specific area used.
			
	CloseUpFileIOInstructions (fileIOInstructionsPtr, &gAreaDescription);
	
	if (continueFlag)
		{
		vectorLengthsPtr[classNumberCode] += 
											index - gGraphRecordPtr->xVector.numberPoints;
		
		gGraphRecordPtr->xVector.numberPoints = index;
		gGraphRecordPtr->yVector.numberPoints = index;
		
		}	// end "if (continueFlag)" 
	
	CloseUpAreaDescription (&gAreaDescription);
	
	CheckAndUnlockHandle (gGraphRecordPtr->vectorLengthsHandle);
		
	UnlockVectorPointer (&gGraphRecordPtr->xVector);
	UnlockVectorPointer (&gGraphRecordPtr->yVector);
		
	return ((SInt16)continueFlag);
		
}	// end "BiPlotFieldData" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean BiPlotProjectData ()
//
//	Software purpose:	The purpose of this routine is to list the
//							data for the requested channels, classes and fields
//							to the output text window or disk file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/08/1994
//	Revised By:			Larry L. Biehl			Date: 10/20/1999	

Boolean BiPlotProjectData ()

{
	FileInfoPtr							savedFileInfoPtr;
	FileIOInstructionsPtr			fileIOInstructionsPtr;
	LayerInfoPtr						savedLayerInfoPtr;
	SInt16*								classPtr;
	WindowInfoPtr						savedWindowInfoPtr;
	
	SInt16								classNumber,
											classIndex,
											numberClasses,
											projectHandleStatus;
											
	Boolean								returnFlag;
	
	
			// Initialize local variables.													
   
	UInt16* channelsPtr;		
	numberClasses = (SInt16)gBiPlotDataSpecsPtr->numberClasses;
	returnFlag = TRUE;
	
			// Continue only if number of classes is one or more.						
	
	if (numberClasses > 0)
		{				
		savedFileInfoPtr = gImageFileInfoPtr;
		savedLayerInfoPtr = gImageLayerInfoPtr;
		savedWindowInfoPtr = gImageWindowInfoPtr;
		
		gImageFileInfoPtr = NULL;
		gImageLayerInfoPtr = NULL;
		gImageWindowInfoPtr = NULL;
		
		if (GetProjectImageFileInfo (kPrompt, 
													kSetupGlobalInfoPointers,
													&gImageWindowInfoPtr,
													&gImageLayerInfoPtr,
													&gImageFileInfoPtr,
													&projectHandleStatus))
			{								
         channelsPtr = (UInt16*)GetHandlePointer (
											gBiPlotDataSpecsPtr->channelsHandle);
         
			if (GetIOBufferPointers (
								&gFileIOInstructions[0],
								gImageWindowInfoPtr,
								gImageLayerInfoPtr,
								gImageFileInfoPtr,
								&gInputBufferPtr, 
								&gOutputBufferPtr,
								1,
		 						gImageWindowInfoPtr->maxNumberColumns,
		 						gBiPlotDataSpecsPtr->columnInterval,
		 						gBiPlotDataSpecsPtr->numberChannels,
								//(HUInt16Ptr)*channelsPtr,
                        channelsPtr,
								kPackData,
								kForceBISFormat,
								kForceReal8Bytes,
								kDoNotAllowForThreadedIO,
								&fileIOInstructionsPtr))
				{							
				classPtr = gBiPlotDataSpecsPtr->classPtr;
		
						// Get offsets to use for lines and columns in case needed for
						//	project fields.
		
				//SetAreaDescriptionOffsetVariables (&gAreaDescription,
				//													gImageWindowInfoPtr,
				//													fileInfoPtr);
					
						// Set up status dialog.  Load in number of classes.					
						
				LoadDItemValue (gStatusDialogPtr, IDC_Status5, (SInt32)numberClasses);
								
				for (classIndex=0; classIndex<numberClasses; classIndex++)
					{
							// Set up status dialog.  Load in class count.						
						
					LoadDItemValue (gStatusDialogPtr, IDC_Status3, (SInt32)classIndex+1);
					
					classNumber = classPtr[classIndex] - 1;
					
							// Transfer data for the class.											
								
					if (BiPlotClassData (fileIOInstructionsPtr, classNumber) == 0)
						{
						returnFlag = FALSE;
						break;
						
						}	// end "if (BiPlotClassData (fileInfoPtr,classNumber) == 0)" 
					
					}	// end "for (classIndex=0; ... 
	
				//ClearAreaDescriptionOffsetVariables (&gAreaDescription);
				
						// Dispose of the IO buffer.		
											
				DisposeIOBufferPointers (fileIOInstructionsPtr,
													&gInputBufferPtr, 
													&gOutputBufferPtr);

				}	// end "if (GetIOBufferPointers (..."

			UnlockImageInformationHandles (projectHandleStatus, 
														gProjectInfoPtr->windowInfoHandle);
														
			}	// end "if (GetProjectImageFileInfo (kPrompt, ..."
		
				// Restore previous global information pointers.
						
		gImageFileInfoPtr = savedFileInfoPtr;
		gImageLayerInfoPtr = savedLayerInfoPtr;
		gImageWindowInfoPtr = savedWindowInfoPtr;
			
		}	// end "if (numberClasses > 0)" 
	
			// Indicate that routine completed normally.									
			
	return (1);
		
}	// end "BiPlotProjectData" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CheckIfStatisticsChannel
//
//	Software purpose:	The purpose of this routine is to determine if the
//							input channel is one for which statistics are
//							available for in the project.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	False if no statistics are available for the channel.
//							True if statistics are available for the channel.
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/17/1994
//	Revised By:			Larry L. Biehl			Date: 05/24/1994	

Boolean CheckIfStatisticsChannel (
				SInt16								channelNumber,
				Boolean								featureTransformationFlag)

{
	SInt16								index;
	
	
	if (gProjectInfoPtr != NULL)
		{
		if (featureTransformationFlag)
																						return (TRUE);
		
		else	// !featureTransformationFlag 
			{
			channelNumber--;
			for (index=0; index<gProjectInfoPtr->numberStatisticsChannels; index++)
				{
				if (channelNumber == gProjectInfoPtr->channelsPtr[index])
																						return (TRUE);
				
				}	// end "for (index=0; index<..." 
				
			}	// end "else !featureTransformationFlag" 

		}	// end "if (gProjectInfoPtr != NULL)" 
				
	return (FALSE);
		
}	// end "CheckIfStatisticsChannel" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void FillVectorOffsets
//
//	Software purpose:	This routine fills that the vector offsets
//							vector.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			GetGraphTitle in SGraphUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/29/1994
//	Revised By:			Larry L. Biehl			Date: 01/19/2017			

void FillVectorOffsets (
				GraphPtr								graphRecordPtr)
													
{
	SInt32								*vectorLengthsPtr,
											*xVectorDataPtr;
									
	SInt32								index,
											numberVectors;
	
	
	if (graphRecordPtr != NULL)
		{
      vectorLengthsPtr = (SInt32*)GetHandlePointer (
											graphRecordPtr->vectorLengthsHandle);
      xVectorDataPtr = (SInt32*)GetHandlePointer (
											graphRecordPtr->xVectorDataHandle);
            
		numberVectors = graphRecordPtr->numberVectors;
		
		for (index=1; index<numberVectors; index++)
			{
			xVectorDataPtr[index] = 
								xVectorDataPtr[index-1] + vectorLengthsPtr[index-1];
				
			}	// end "for (index=1; index<numberVectors; index++)" 
		
		}	// end "if (graphRecordPtr)" 

}	// end "FillVectorOffsets" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetBiPlotGraphTitle
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
// Called By:			GetGraphTitle in SGraphUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/22/1994
//	Revised By:			Larry L. Biehl			Date: 05/24/1994			

void GetBiPlotGraphTitle (
				GraphPtr								graphRecordPtr)
											
{
	if (graphRecordPtr != NULL)
		{
				// Create title line 1.															
				
		if (gBiPlotDataSpecsPtr->featureTransformationFlag)
			sprintf ((char*)gTextString, "Features");
						
		else	// !gBiPlotDataSpecsPtr->featureTransformationFlag 
			sprintf ((char*)gTextString, "Channels");
		
		sprintf ((char*)&graphRecordPtr->title[1],
								"BiPlot of %s %d vs %d",
								gTextString,
								gBiPlotDataSpecsPtr->axisFeaturePtr[1] + 1,
								gBiPlotDataSpecsPtr->axisFeaturePtr[0] + 1);
								
		graphRecordPtr->title[0] = (UInt8)strlen ((char*)&graphRecordPtr->title[1]);
		
				// Create title line 2.															
								
		graphRecordPtr->title2[1] = 0;
		graphRecordPtr->title2[0] = 0;
		
		}	// end "if (graphRecordPtr != NULL)" 

}	// end "GetBiPlotGraphTitle" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		short int GetStatisticsChannelFeature
//
//	Software purpose:	The purpose of this routine is to return the relative 
//							position of the input channel or feature within the
//							mean vector or covariance matrix.
//
//	Parameters in:		input channel or feature number
//							flag indicating with feature transformation status
//
//	Parameters out:	None
//
// Value Returned:	The relative position of the input channel or
//							feature within the mean vector or covariance
//							matrix.
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 05/18/1994
//	Revised By:			Larry L. Biehl			Date: 05/18/1994	

SInt16 GetStatisticsChannelFeature (
				SInt16								channelNumber,
				Boolean								featureTransformationFlag)

{
	SInt16								featureNumber,
											index;
	
	
	featureNumber = 0;
	
	if (featureTransformationFlag)
		featureNumber = channelNumber;
		
	else	// !featureTransformationFlag 
		{
		if (gProjectInfoPtr != NULL)
			{
			for (index=0; index<gProjectInfoPtr->numberStatisticsChannels; index++)
				{
				if (channelNumber == gProjectInfoPtr->channelsPtr[index])
					featureNumber = index;
				
				}	// end "for (index=0; index<..." 
	
			}	// end "if (gProjectInfoPtr != NULL)" 
			
		}	// end "else !featureTransformationFlag" 
				
	return (featureNumber);
		
}	// end "GetStatisticsChannelFeature" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean LoadBiPlotClassStats ()
//
//	Software purpose:	The purpose of this routine is to list the
//							data for the requested channels, classes and fields
//							to the output text window or disk file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 05/11/1994
//	Revised By:			Larry L. Biehl			Date: 06/17/2006	

Boolean LoadBiPlotClassStats (
				GraphPtr								graphRecordPtr)

{
	double								angle1,
											angle2,
											determinant,
											inverseMatrix[4],
											logDeterminant,
											rotationAngle;
											
	double								*classStatisticsPtr,
											*eigenValuePtr,
											*eigenVectorPtr,
											*tempMatrixPtr;
	
	HPClassNamesPtr					classNamesPtr;
											
	SInt16								*classPtr,
											*statFeaturePtr;
										
	SInt32								classIndex,
											classStatsIncrement,
											endIndex,
											index,
											numberClasses,
											numberFeatures,
											startIndex,
											statClassNumber,
											xxIndex,
											xyIndex,
											yyIndex;
	
	SInt16								classStorage,
											numberChannels,
											xFeature,
											yFeature;
											
	Boolean								continueFlag,
											determinantOKFlag;
	
	
			// Initialize local variables.													
	
	determinantOKFlag = TRUE;		
	numberClasses = gBiPlotDataSpecsPtr->numberClasses;
	numberChannels = gProjectInfoPtr->numberStatisticsChannels;
	
	numberFeatures = gProjectInfoPtr->numberStatisticsChannels;
	if (gBiPlotDataSpecsPtr->featureTransformationFlag)
		{
		numberFeatures = gTransformationMatrix.numberFeatures;
		numberChannels = gTransformationMatrix.numberChannels;
		
		}	// end "if (gBiPlotDataSpecsPtr->featureTransformationFlag)"
	
	classPtr = gBiPlotDataSpecsPtr->classPtr;
	classNamesPtr = gProjectInfoPtr->classNamesPtr;
	
	xFeature = GetStatisticsChannelFeature (
						gBiPlotDataSpecsPtr->axisFeaturePtr[0],
						gBiPlotDataSpecsPtr->featureTransformationFlag);
	
	yFeature = GetStatisticsChannelFeature (
						gBiPlotDataSpecsPtr->axisFeaturePtr[1],
						gBiPlotDataSpecsPtr->featureTransformationFlag);
						
	xxIndex = xFeature + numberFeatures * xFeature;
	xyIndex = xFeature + numberFeatures * yFeature;
	yyIndex = yFeature + numberFeatures * yFeature;
	
	MHLock (graphRecordPtr->classStatisticsHandle);

   classStatisticsPtr = (double*)GetHandlePointer (
											graphRecordPtr->classStatisticsHandle);
            
	classStatsIncrement = 5 + 3 * numberFeatures;
								
	if (gBiPlotDataSpecsPtr->featureTransformationFlag)
		{
				// Get the transformation matrix that will be used for this			
				// set of features.																
				
		MHLock (gTransformationMatrix.eigenVectorHandle);

      HDoublePtr eigenVectorPtr = (HDoublePtr)GetHandlePointer (
											gTransformationMatrix.eigenVectorHandle,
											kLock);
											
 		ReduceMatrix (eigenVectorPtr, 
							gTransformationMatrix.eigenVectorPtr, 
							gTransformationMatrix.numberFeatures, 
							numberChannels, 
							(SInt16)numberFeatures, 
							NULL,
							(gTransformationMatrix.createdByCode >= 16));           
							
		CheckAndUnlockHandle (gTransformationMatrix.eigenVectorHandle);
		
		}	// end "if (...->featureTransformationFlag)" 
		
			// All feature will be used for now to get the inverse matrix.			
			
	statFeaturePtr = (SInt16*)GetStatisticsFeatureVector (
									gBiPlotDataSpecsPtr->featureTransformationFlag, 
									NULL);
							
	continueFlag = SetupBiPlotStatMemory (
				&gTempChannelStatsPtr,
				&eigenValuePtr,
				&eigenVectorPtr,
				numberChannels);
	
			// Continue only if number of classes is one or more.						
	
	if (continueFlag)
		{
				// Get the square root of the threshold value.  This is used		
				// for determining the radius distance of the ellipse of 			
				// concentration.																	
		/*				
		graphRecordPtr->classThresholdValue = GetChiSquaredValue (
						gProjectInfoPtr->numberStatisticsChannels, 
						(100.-gBiPlotDataSpecsPtr->probabilityThreshold)/100.);
		*/				
		graphRecordPtr->classThresholdValue = GetChiSquaredValue (
						2, 
						(100.-gBiPlotDataSpecsPtr->probabilityThreshold)/100.);
						
		graphRecordPtr->classThresholdValue = 
											sqrt (graphRecordPtr->classThresholdValue);
	
		graphRecordPtr->numberStatisticsChannels = (SInt16)numberFeatures;
						
		graphRecordPtr->xEllipseMin = DBL_MAX;
		graphRecordPtr->xEllipseMax = -DBL_MAX;
		graphRecordPtr->yEllipseMin = DBL_MAX;
		graphRecordPtr->yEllipseMax = -DBL_MAX;

				// Set up status dialog.  Load in number of classes.					
				
		LoadDItemValue (gStatusDialogPtr, IDC_Status5, (SInt32)numberClasses);
		
		for (classIndex=0; classIndex<numberClasses; classIndex++)
			{
					// Set up status dialog.  Load in class count.						
				
			LoadDItemValue (gStatusDialogPtr, IDC_Status3, (SInt32)classIndex+1);

			statClassNumber = classPtr[classIndex] - 1;
						
			classStorage = gProjectInfoPtr->storageClass[statClassNumber];
			LoadDItemString (gStatusDialogPtr, 
									IDC_Status6, 
									(Str255*)&classNamesPtr[classStorage].name);

        classStatisticsPtr = (double*)GetHandlePointer (
											graphRecordPtr->classStatisticsHandle);

			classStatisticsPtr = 
							&classStatisticsPtr[statClassNumber*classStatsIncrement];
			
					// Get the class covariance matrix - triangular	form. 			

			GetTransformedClassCovarianceMatrix (
										numberChannels, 
										gTempChannelStatsPtr, 
										gInverseMatrixMemory.inversePtr, 
										(UInt16*)statFeaturePtr, 
										(UInt16)statClassNumber,
										kSquareOutputMatrix,
										kMeanCovariance,
										gTransformationMatrix.eigenVectorPtr,
										gTransformationMatrix.tempMatrixPtr,
										(UInt16)numberFeatures);
										
					// Get the eigenvectors and eigenvalues for the covariance		
					// (or transformed covariance) matrix.									
			
			BlockMoveData (gInverseMatrixMemory.inversePtr,
							eigenValuePtr,
							numberFeatures * numberFeatures * sizeof (double));
								
			if (!ComputeEigenvectors (
									eigenValuePtr,
									(UInt16)numberFeatures, 
									eigenVectorPtr,
									gInverseMatrixMemory.ipvotPtr,
									gInverseMatrixMemory.pivotPtr,
									3))
				{
				continueFlag = FALSE;
				break;
				
				}	// end "if (!ComputeEigenvectors (..." 
										
					// Get the angle for the mean square regression lines.			
			
			angle1 = gInverseMatrixMemory.inversePtr[xyIndex];
					
			if (gInverseMatrixMemory.inversePtr[xxIndex] != 0)
				angle1 /= gInverseMatrixMemory.inversePtr[xxIndex];
			
			angle1 = atan (angle1);
			
			angle2 = gInverseMatrixMemory.inversePtr[yyIndex];
			
			if (gInverseMatrixMemory.inversePtr[xyIndex] != 0)
				angle2 /= gInverseMatrixMemory.inversePtr[xyIndex];
			
			angle2 = atan (angle2);
				
					// Get the inverse of the covariance matrix and the				
					// determinant.																
			
			inverseMatrix[0] = gInverseMatrixMemory.inversePtr[xxIndex];
			inverseMatrix[1] = gInverseMatrixMemory.inversePtr[xyIndex];
			inverseMatrix[2] = gInverseMatrixMemory.inversePtr[xyIndex];
			inverseMatrix[3] = gInverseMatrixMemory.inversePtr[yyIndex];
			
			determinantOKFlag = InvertSymmetricMatrix (
												inverseMatrix, 
												2, 
												gInverseMatrixMemory.pivotPtr, 
												gInverseMatrixMemory.indexRowPtr, 
												gInverseMatrixMemory.indexColPtr, 
												gInverseMatrixMemory.ipvotPtr,
												&determinant, 
												&logDeterminant,
												kReturnMatrixInverse);
									
			if (gOperationCanceledFlag)
				{
				continueFlag = FALSE;
				break;
				
				}	// end "if (gOperationCanceledFlag)" 
			
			if (determinantOKFlag)
				{				
				classStatisticsPtr[2] = inverseMatrix[0];
				classStatisticsPtr[3] = inverseMatrix[1];
				classStatisticsPtr[4] = inverseMatrix[3];
				
						// Get the class mean vector											
		
				GetTransformedMeanVector (
									gTempChannelStatsPtr, 
									gInverseMatrixMemory.inversePtr,
									numberChannels,
									NULL,
									gTransformationMatrix.eigenVectorPtr,
									gTransformationMatrix.tempMatrixPtr,
									gTransformationMatrix.offsetVectorPtr,
									(SInt16)numberFeatures);
			
						// Save the mean vector in the class statistics arrary for	
						// use in creating biplot graphs.									
						
				classStatisticsPtr[0] = gInverseMatrixMemory.inversePtr[xFeature];
				classStatisticsPtr[1] = gInverseMatrixMemory.inversePtr[yFeature];
				
						// Store the inverse of eigenvalues and selected transposed 
						// eigenvectors to transform data from eigenvalue space		
						// to the requested 2-dimensional feature space.				
				
				startIndex = 5;
				endIndex = startIndex + numberFeatures;
				tempMatrixPtr = eigenValuePtr;
				for (index=startIndex; index<endIndex; index++)
					{
					classStatisticsPtr[index] = 1. / (*tempMatrixPtr);
					tempMatrixPtr++;
					
					}	// end "for (index=5; index<endIndex; index++)" 
					
				startIndex = endIndex;
				endIndex += numberFeatures;
				tempMatrixPtr = &eigenVectorPtr[xFeature*numberFeatures];
				for (index=startIndex; index<endIndex; index++)
					{
					classStatisticsPtr[index] = *tempMatrixPtr;
					tempMatrixPtr++;
					
					}	// end "for (index=5; index<endIndex; index++)" 
					
				startIndex = endIndex;
				endIndex += numberFeatures;
				tempMatrixPtr = &eigenVectorPtr[yFeature*numberFeatures];
				for (index=startIndex; index<endIndex; index++)
					{
					classStatisticsPtr[index] = *tempMatrixPtr;
					tempMatrixPtr++;
					
					}	// end "for (index=5; index<endIndex; index++)" 
			
						// Update the mins and maxes.											
						// determinant is used here as a temporary variable.			
				
						// Multiply the cross variance variable by 2 since this 		
						// will be needed in all following algorithms that use it.	
						
				classStatisticsPtr[3] *= 2;		
				
						// Get the angle that the ellipse is rotated relative to		
						// the x and y axis.														
				
				rotationAngle = classStatisticsPtr[3];
				
				if (classStatisticsPtr[2] != classStatisticsPtr[4])
					{	
					rotationAngle /= (classStatisticsPtr[2] - classStatisticsPtr[4]);
					rotationAngle = .5 * atan (rotationAngle);
					
					}	// end "if (....inversePtr[xxIndex] != ..." 
					
				else	//  
					rotationAngle = 0;
					
						// Adjust min-max for limits of projection of principal 		
						// plane on x-y plane.													
						
				UpdateEllipseMinMax2 (graphRecordPtr, 0, classStatisticsPtr);
				
				UpdateEllipseMinMax2 (graphRecordPtr, kHALF_PI, classStatisticsPtr);
				
						// Adjust for min-max relative to first mean square 			
						// regression line.														
						
				//UpdateEllipseMinMax (
				//				graphRecordPtr, rotationAngle+angle1, classStatisticsPtr);
										
				//UpdateEllipseMinMax (
				//				graphRecordPtr, rotationAngle-angle1, classStatisticsPtr);
								
						// Adjust for min-max relative to second mean square
						// regression line.
										
				//UpdateEllipseMinMax (
				//				graphRecordPtr, rotationAngle+angle2, classStatisticsPtr);
											
				//UpdateEllipseMinMax (
				//				graphRecordPtr, rotationAngle-angle2, classStatisticsPtr);
				
						// Adjust for min-max relative to minor axis 1.					
						
				UpdateEllipseMinMax (
									graphRecordPtr, rotationAngle, classStatisticsPtr);
				
						// Adjust for min-max relative to minor axis 2.					
						
				rotationAngle += kHALF_PI;
				UpdateEllipseMinMax (
									graphRecordPtr, rotationAngle, classStatisticsPtr);     
									
				}	// end "if (determinantOKFlag)" 
				
			else	// !determinantOKFlag 
				{
						// List message that covariance matrix for this class 		
						// could not be inverted.												
					
				continueFlag = ListClassInformationMessage (kProjectStrID,
																			IDS_Project31,
																			NULL, 
																			gOutputForce1Code,
																			statClassNumber, 
																			continueFlag);
				
				}	// end "else !determinantOKFlag" 
			
			}	// end "for (classIndex=0; ... 
		
		if (!determinantOKFlag)
			{			
			continueFlag = FALSE;			
			SysBeep (10);
			
			}	// end "if (!determinantOKFlag)"
			
		}	// end "if (continueFlag)" 
		
	gTempChannelStatsPtr = (ChannelStatisticsPtr)CheckAndDisposePtr (
																		(Ptr)gTempChannelStatsPtr);
	eigenValuePtr = CheckAndDisposePtr (eigenValuePtr);
	eigenVectorPtr = CheckAndDisposePtr (eigenVectorPtr);
	
	ReleaseMatrixInversionMemory ();
			
	ReleaseFeatureTransformationMemory ();
	
	CheckAndUnlockHandle (graphRecordPtr->classStatisticsHandle);
			
	return (continueFlag);
		
}	// end "LoadBiPlotClassStats" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean LoadBiPlotDataSpecs
//
//	Software purpose:	The purpose of this routine is to load the BiPlot 
//							Data Specification structure with the default set of
//							parameters.
//
//	Parameters in:				.
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			BiPlotDataControl
//
//	Coded By:			Larry L. Biehl			Date: 03/08/1994
//	Revised By:			Larry L. Biehl			Date: 06/16/2016

Boolean LoadBiPlotDataSpecs (
				Handle								windowInfoHandle)

{
	SInt16*								channelsPtr;
	unsigned char*						symbolsPtr;
	
	SInt16								fieldTypesPresentCode;
	
	SInt32								bytesNeeded,
											numberVectors;
	
	Boolean								changedFlag,
											continueFlag,
											projectFlag;
		
	
			// Assume routine will work okay.  If there isn't enough memory		
			// then the flag will be changed.												
			
	continueFlag = TRUE;
	
			// Determine if project information is allowed. There must be an
			// open project, the number of statistics classes must be at least
			// one, and the number of channels in the image must be the same
			// as the number in the project base image.
			
	projectFlag = (gProjectInfoPtr != NULL);
	if (projectFlag && gProjectInfoPtr->numberStatisticsClasses == 0)
		projectFlag = FALSE;
		
	if (projectFlag)
		{
		if ((UInt32)gImageWindowInfoPtr->totalNumberChannels != 
						GetTotalNumberOfChannels (gProjectInfoPtr->windowInfoHandle))
			projectFlag = FALSE;
			
		}	// end "if (projectFlag)"
	
			// If a biplot data structure already exists, then check if it is		
			// for the current window.  If not release the old structure and		
			// initialize a new one.															
			
	if (gNonProjProcessorSpecs.biPlotDataSpecsH != NULL)
		{
				// Lock the handle to the biplot specifications and get				
				// a pointer to the structure.												
	
		gBiPlotDataSpecsPtr = (BiPlotDataSpecsPtr)GetHandlePointer (
							gNonProjProcessorSpecs.biPlotDataSpecsH, kLock, kMoveHi);
							
		if (gBiPlotDataSpecsPtr->windowInfoHandle != windowInfoHandle || 
					gImageWindowInfoPtr->fileInfoVersion != 
													gBiPlotDataSpecsPtr->fileInfoVersion ||
							gBiPlotDataSpecsPtr->projectFlag != projectFlag)
			{
			ReleaseBiPlotDataSpecsMemory (&gNonProjProcessorSpecs.biPlotDataSpecsH);
			gBiPlotDataSpecsPtr = NULL;
			
			}	// end "if (gBiPlotDataSpecsPtr->fileInfoHandle != ..." 
			
		else	// gBiPlotDataSpecsPtr->windowInfoHandle == ... 
			{
			gBiPlotDataSpecsPtr->projectFlag = projectFlag;
			
			if (!projectFlag)
				gBiPlotDataSpecsPtr->plotDataCode = kAreaType;
				
			}	// end "else gBiPlotDataSpecsPtr->windowInfoHandle == ..." 
				
		}	// end "else gNonProjProcessorSpecs.biPlotDataSpecsH != NULL" 
	
	if (gNonProjProcessorSpecs.biPlotDataSpecsH == NULL)
		{
				// Set up handle to project file information								
			
		gNonProjProcessorSpecs.biPlotDataSpecsH = 
													MNewHandle (sizeof (BiPlotDataSpecs));
		if (gNonProjProcessorSpecs.biPlotDataSpecsH != NULL)
			{
			gBiPlotDataSpecsPtr = (BiPlotDataSpecsPtr)GetHandlePointer (
							gNonProjProcessorSpecs.biPlotDataSpecsH, kLock, kMoveHi);
			
					// Initialize the biplot data specification structure.			
								
			gBiPlotDataSpecsPtr->probabilityThreshold = 10.;
			gBiPlotDataSpecsPtr->channelsHandle = NULL;
			gBiPlotDataSpecsPtr->classHandle = NULL;
			gBiPlotDataSpecsPtr->symbolsHandle = NULL;
			gBiPlotDataSpecsPtr->windowInfoHandle = windowInfoHandle;
			gBiPlotDataSpecsPtr->classPtr = NULL;
			gBiPlotDataSpecsPtr->symbolsPtr = NULL;
			
			gBiPlotDataSpecsPtr->lineStart = 1;
			gBiPlotDataSpecsPtr->lineEnd = gImageWindowInfoPtr->maxNumberLines;
			gBiPlotDataSpecsPtr->lineInterval = 1;
			gBiPlotDataSpecsPtr->columnStart = 1;
			gBiPlotDataSpecsPtr->columnEnd = gImageWindowInfoPtr->maxNumberColumns;
			gBiPlotDataSpecsPtr->columnInterval = 1;
			
			gBiPlotDataSpecsPtr->fileInfoVersion = 
															gImageWindowInfoPtr->fileInfoVersion;
															
			gBiPlotDataSpecsPtr->classSet = kAllMenuItem;
			gBiPlotDataSpecsPtr->displayPixelCode = kDisplayPixels + kPixelsAsSymbols;
				
			gBiPlotDataSpecsPtr->outlineClassCode = kClassesAsEllipses;
				
					// If only cluster fields are available, pixels cannot be 
					// plotted. Don't know where the pixels come from.
					
			fieldTypesPresentCode = DetermineFieldTypes ();
			
			if (projectFlag && gProjectInfoPtr->numberStatisticsClasses > 0 &&
																	fieldTypesPresentCode != 4)
				{
				gBiPlotDataSpecsPtr->plotDataCode = 0;
				
				if (gProjectInfoPtr->numberStatTrainFields > 0)
					gBiPlotDataSpecsPtr->plotDataCode += kTrainingType;
				
				if (gProjectInfoPtr->numberStatTestFields > 0)
					gBiPlotDataSpecsPtr->plotDataCode += kTestingType;
					
				}	// end "if (projectFlag && ..." 
			
			else	// !projectFlag || gProjectInfoPtr->...) 
				gBiPlotDataSpecsPtr->plotDataCode = kAreaType;
				
			gBiPlotDataSpecsPtr->symbolSet = kDefaultMenuItem;
			
			gBiPlotDataSpecsPtr->axisFeaturePtr[0] = 0;		// x-axis 
			gBiPlotDataSpecsPtr->axisFeaturePtr[1] = 1;		// y-axis 
			
			gBiPlotDataSpecsPtr->numberChannels = 2;
			
			if (projectFlag && gProjectInfoPtr != NULL)
				{
				gBiPlotDataSpecsPtr->numberClasses =
												gProjectInfoPtr->numberStatisticsClasses;
				gBiPlotDataSpecsPtr->numberFields = 
												gProjectInfoPtr->numberStatisticsFields;
												
				}	// end "if (projectFlag && gProjectInfoPtr != NULL)" 
				
			else	// !projectFlag || gProjectInfoPtr == NULL 
				{
				gBiPlotDataSpecsPtr->numberClasses = 0;
				gBiPlotDataSpecsPtr->numberFields = 0;
				
				}	// end "else !projectFlag || gProjectInfoPtr == NULL" 

			gBiPlotDataSpecsPtr->createNewGraphicsWindowFlag = TRUE;
			gBiPlotDataSpecsPtr->firstTime = TRUE;
			gBiPlotDataSpecsPtr->projectFlag = projectFlag;
			gBiPlotDataSpecsPtr->thresholdFlag = FALSE;
			gBiPlotDataSpecsPtr->featureTransformationFlag = FALSE;
			
			}	// end "if (gbiPlotDataSpecsH != NULL)" 
		
		else	// else gNonProjProcessorSpecs.biPlotDataSpecsH == NULL 
			continueFlag = FALSE;
			
		}	// end "if (gNonProjProcessorSpecs.biPlotDataSpecsH == NULL)" 
					
			// Check memory for biplot classes vector.									
		
	if (continueFlag && gBiPlotDataSpecsPtr->projectFlag)
		{	
		bytesNeeded =
			(SInt32)gProjectInfoPtr->numberStatisticsClasses * sizeof (SInt16);
		gBiPlotDataSpecsPtr->classPtr = (SInt16*)CheckHandleSize (
										&gBiPlotDataSpecsPtr->classHandle,
										&continueFlag, 
										&changedFlag, 
										bytesNeeded);
		if (changedFlag)
			gBiPlotDataSpecsPtr->classSet = kAllMenuItem;
			
		}	// end "if (continueFlag)" 
			
			// If memory is not full, check memory for channels vector.				

	if (continueFlag)
		{
		bytesNeeded = gImageWindowInfoPtr->totalNumberChannels * sizeof (SInt16);
		channelsPtr = (SInt16*)CheckHandleSize (
										&gBiPlotDataSpecsPtr->channelsHandle, 
										&continueFlag,
										&changedFlag, 
										bytesNeeded);
											
		if (changedFlag)
			{
			channelsPtr[0] = 0;
			channelsPtr[1] = 1;
			
			}	// end "if (changedFlag)" 
			
		}	// end "if (continueFlag)" 

	if (continueFlag)
		{
		numberVectors = 1;
		if (gBiPlotDataSpecsPtr->projectFlag)
			numberVectors += gProjectInfoPtr->numberStatisticsClasses;
			
		bytesNeeded = numberVectors * sizeof (char);
		symbolsPtr = (UInt8*)CheckHandleSize (
										&gBiPlotDataSpecsPtr->symbolsHandle,
										&continueFlag, 
										&changedFlag, 
										bytesNeeded);
		if (changedFlag)
			gBiPlotDataSpecsPtr->symbolSet = kDefaultMenuItem;
			
		}	// end "if (continueFlag)" 
		
	if (continueFlag)
		{
				// Load selection rectangle information into biplot data		 		
				// specification information if the selection rectangle has 		
				// been set up																		

		GetSelectedAreaInfo (
							gActiveImageWindow,
							gImageWindowInfoPtr,
							&gBiPlotDataSpecsPtr->lineStart,
							&gBiPlotDataSpecsPtr->lineEnd,
							&gBiPlotDataSpecsPtr->columnStart,
							&gBiPlotDataSpecsPtr->columnEnd,
							kDontClearSelectionArea,
							kUseThreshold,
							kDontAdjustToBaseImage);
			
				// Make certain that the class vector is filled in properly.		
		
		if (gBiPlotDataSpecsPtr->classSet == kAllMenuItem && 
																gBiPlotDataSpecsPtr->projectFlag)								
			LoadClassVector (&gBiPlotDataSpecsPtr->numberClasses,
									gBiPlotDataSpecsPtr->classPtr);
			
		if (gBiPlotDataSpecsPtr->symbolSet == kDefaultMenuItem && 
																gBiPlotDataSpecsPtr->projectFlag)
			LoadClassSymbolVector (symbolsPtr, 
										IDS_Symbol1, 
										gProjectInfoPtr->numberStatisticsClasses);
			
		}	// end "if (continueFlag)" 
	
			// If unable to set up information for biplot								
			// specifications, release memory allocated to it (if any).				
			
	else	// !continueFlag 
		{
		ReleaseBiPlotDataSpecsMemory (&gNonProjProcessorSpecs.biPlotDataSpecsH);
		gBiPlotDataSpecsPtr = NULL;
		
		}	// end "else !continueFlag" 
		
	return (continueFlag);

}	// end "LoadBiPlotDataSpecs" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean SetupBiPlotStatMemory
//
//	Software purpose:	The purpose of this routine is to setup the 
//							memory to load the BiPlot Processor statistics.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	False if not enough memory is available.
//							True if memory was allocated okay.
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 05/16/1994
//	Revised By:			Larry L. Biehl			Date: 05/26/1994	

Boolean SetupBiPlotStatMemory (
				HChannelStatisticsPtr*			classChannelStatsPtrPtr,
				HDoublePtr*							eigenValuePtrPtr,
				HDoublePtr*							eigenVectorPtrPtr,
				SInt16								numberChannels)

{
	UInt32								bytesNeeded;
	
	Boolean								continueFlag;
	
	
			// Initialize parameters															
			
	continueFlag = TRUE;
	*classChannelStatsPtrPtr = NULL;
	*eigenValuePtrPtr = NULL;
	*eigenVectorPtrPtr = NULL;
	
				// Get pointer to memory to use for class channel statistics		
				// that will be used in computing the covariance matrix and			
				// getting the data for the class mean vector.							
				
	bytesNeeded = 
				(UInt32)numberChannels * sizeof (ChannelStatistics);
	*classChannelStatsPtrPtr = (HChannelStatisticsPtr)MNewPointer (bytesNeeded);
	continueFlag = (*classChannelStatsPtrPtr != NULL) ;
	
	bytesNeeded = 
				(UInt32)numberChannels * numberChannels * sizeof (double);
				
	if (continueFlag)
		{
				// Get memory for storing covariance and resulting						
				// eigenvalues for a class.													
				
		*eigenValuePtrPtr = (HDoublePtr)MNewPointer (bytesNeeded);
		continueFlag = (*eigenValuePtrPtr != NULL) ;
		
		}	// end "if (continueFlag)" 
	
	if (continueFlag)
		{
				// Get memory for storing  eigenvectors for a class.					
				
		*eigenVectorPtrPtr = (HDoublePtr)MNewPointer (bytesNeeded);
		continueFlag = (*eigenVectorPtrPtr != NULL) ;
		
		}	// end "if (continueFlag)" 
		
	if (continueFlag)
		{
				// Get pointer to memory to use for inverse matrix computation.	
				
		continueFlag = SetupMatrixInversionMemory (numberChannels,
																	kMeanCovariance,
																	&gInverseMatrixMemory.inversePtr, 
																	&gInverseMatrixMemory.pivotPtr, 
																	&gInverseMatrixMemory.indexRowPtr, 
																	&gInverseMatrixMemory.indexColPtr, 
																	&gInverseMatrixMemory.ipvotPtr);
		
		}	// end "if (continueFlag)" 
		
	return (continueFlag);
		
}	// end "SetupBiPlotStatMemory" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void UpdateEllipseMinMax
//
//	Software purpose:	The purpose of this routine is to estimate the minimum
//							and maximum for the ellipse based on the input angle
//							and inverse covariance matrix.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 05/11/1994
//	Revised By:			Larry L. Biehl			Date: 05/23/1994	

void UpdateEllipseMinMax (
				GraphPtr								graphRecordPtr,
				double								angle,
				double*								classStatisticsPtr)

{
	double								chiSquaredValue,
											cosAngle,
											distance,
											sinAngle,
											x_val,
											y_val;
							
	
	if (graphRecordPtr)
		{	
		chiSquaredValue = graphRecordPtr->classThresholdValue;
		
				// Adjust for min-max relative to second mean square 					
				// regression line.																
		
		cosAngle = cos (angle);
		sinAngle = sin (angle);
		
		distance = cosAngle*cosAngle*classStatisticsPtr[2] + 
								cosAngle*sinAngle*classStatisticsPtr[3] + 
										sinAngle*sinAngle*classStatisticsPtr[4];
																
		distance = chiSquaredValue/sqrt (fabs (distance));
		
		x_val = fabs (cosAngle * distance);
		y_val = fabs (sinAngle * distance);
		
				// Min/max in x direction.														
		
		graphRecordPtr->xEllipseMin = MIN (
							(classStatisticsPtr[0] - x_val),
							graphRecordPtr->xEllipseMin);
				
		graphRecordPtr->xEllipseMax = MAX (
							(classStatisticsPtr[0] + x_val),
							graphRecordPtr->xEllipseMax);
		
				// Min/max in y direction.														
			
		graphRecordPtr->yEllipseMin = MIN (
							(classStatisticsPtr[1] - y_val),
							graphRecordPtr->yEllipseMin);
				
		graphRecordPtr->yEllipseMax = MAX (
							(classStatisticsPtr[1] + y_val),
							graphRecordPtr->yEllipseMax);
							
		}	// end "if (graphRecordPtr)" 
		
}	// end "UpdateEllipseMinMax" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void UpdateEllipseMinMax2
//
//	Software purpose:	The purpose of this routine is to estimate the minimum
//							and maximum for the ellipse based on the second mean
//							square regression line.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 05/27/1994
//	Revised By:			Larry L. Biehl			Date: 05/27/1994	

void UpdateEllipseMinMax2 (
				GraphPtr								graphRecordPtr,
				double								angle,
				double*								classStatisticsPtr)

{
	double								chiSquaredValue,
											distance,
											x_val,
											xT_val,
											y_val,
											yT_val;
							
	
	if (graphRecordPtr != NULL)
		{	
		chiSquaredValue = graphRecordPtr->classThresholdValue;
		
				// Adjust for min-max relative to second mean square 					
				// regression line.																
		
		xT_val = cos (angle);
		yT_val = sin (angle);
		
		distance = xT_val*xT_val*classStatisticsPtr[5] + 
										yT_val*yT_val*classStatisticsPtr[6];
																
		distance = chiSquaredValue/sqrt (fabs (distance));
		
		xT_val *= distance;
		yT_val *= distance;
		
		x_val = classStatisticsPtr[
										5+graphRecordPtr->numberStatisticsChannels] * xT_val +
					classStatisticsPtr[
										6+graphRecordPtr->numberStatisticsChannels] * yT_val;
						
		y_val = classStatisticsPtr[
										5+2*graphRecordPtr->numberStatisticsChannels] * xT_val +
					classStatisticsPtr[
										6+2*graphRecordPtr->numberStatisticsChannels] * yT_val;
					
		x_val = fabs (x_val);
		y_val = fabs (y_val);
		
				// Min/max in x direction.														
		
		graphRecordPtr->xEllipseMin = MIN ((classStatisticsPtr[0] - x_val),
														graphRecordPtr->xEllipseMin);
			
		graphRecordPtr->xEllipseMax = MAX ((classStatisticsPtr[0] + x_val),
														graphRecordPtr->xEllipseMax);
		
				// Min/max in y direction.														
			
		graphRecordPtr->yEllipseMin = MIN ((classStatisticsPtr[1] - y_val),
														graphRecordPtr->yEllipseMin);
			
		graphRecordPtr->yEllipseMax = MAX ((classStatisticsPtr[1] + y_val),
														graphRecordPtr->yEllipseMax);
			
		}	// end "if (graphRecordPtr != NULL)" 
		
}	// end "UpdateEllipseMinMax2" 
		
