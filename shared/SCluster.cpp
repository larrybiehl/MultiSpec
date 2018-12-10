//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	File:						SCluster.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			12/07/2018
//
//	Language:				C
//
//	System:					Linux, Macintosh & Windows Operating Systems
//
//	Functions in file:	Boolean 					ClusterClassification
//								void 						ClusterControl
//								Boolean 					ClusterDialog
//								void 						ClusterDialogItems9to11
//								Boolean 					CreateClusterMaskFile
//								Boolean 					CreateMaskFileClassNames
//								pascal void 			DrawSaveStatsPopUp
//								Boolean 					GetClusterAreaStatistics
//								Boolean 					GetClusterProjectStatistics
//								Boolean					GetNextClusterArea
//								Boolean 					GetProbabilityFile
//								Boolean 					InitializeClusterMemory 
//								Boolean 					ListClusterStatistics
//								Boolean 					LoadClusterSpecs
//								Boolean 					SaveClusterStatistics
//								Boolean 					SetUpClassToFinalClassPtr
//								Boolean		 			UpdateClusterStdDeviations
//
//	Brief description:	This file contains routines that control clustering image 
//								data.
//
//	Diagram of MultiSpec routine calls for the routines in the file.
//
//		ClusterControl
//			GetProjectImageFileInfo ()
//
//			LoadClusterSpecs
//
//			ClusterDialog
//				ProcessorDialogFilter (in dialogUtilities)
//				
//				ClusterDialogItems9to11
//
//			ClusterOnePassControl
//
//			ISODATAClusterControl
//
//
//		ClusterClassification
//
//
//		InitializeClusterMemory
//			GetNumberOfClusterPixels
//
//
//		SaveClusterStatistics
//			AddClassToProject (in project.c)
//
//			AddFieldToProject (in project.c)
//
//			SetupStatsMemory (in statCompute.c)
//
//			GetClusterProjectStatistics
//				GetProjectStatisticsPointers (in project.c)
//
//				GetClusterAreaStatistics
//					GetProjectStatisticsPointers (in project.c)
//
//					GetNextClusterArea
//
//			CloseProjectStructure (in project.c)
//
//			GetProjectStatisticsPointers (in project.c) 
//
//
//		UpdateClusterStdDeviations
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
/*
			// Template for debugging.
		int numberChars = sprintf ((char*)gTextString3,
									" SCluster:xxxx (): %s",
									gEndOfLine);
		ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"     

#if defined multispec_lin   
	#include "LClusterDialog.h" 
	#include "wx/wx.h"
#endif	// defined multispec_lin

#if defined multispec_mac || defined multispec_mac_swift
	#define	IDC_ClassifyTrainingAreas		10
	#define	IDC_ImageOverlay					36
	#define	IDC_UsePixelsThresholdFlag		37
	#define	IDC_UsePixelsThresholdValue	38
	#define	IDC_Percent							39
	#define	IDC_ImageOverlayCombo			40
	
	#define	IDS_Cluster52						52
	
	#define	IDS_Alert141						141
	#define	IDS_Alert142						142
#endif	// defined multispec_mac || defined multispec_mac_swift 
  
#if defined multispec_win   
	#include "WClusterDialog.h"
#endif	// defined multispec_win 



#define	kmaxNumberCharactersForLine		2000

#define	kDoNotSaveStatistics					0
#define	kSaveStatisticsToNewProject		1
#define	kSaveStatisticsToCurrentProject	2

#define	kClusterERDAS74OutputFormat		1
#define	kClusterTIFFGeoTIFFOutputFormat	2

#if defined multispec_mac         
	PascalVoid SpinCursorTimer (
					EventLoopTimerRef					inTimer,
					void*									inUserData);
#endif	// defined multispec_mac 



//****************************************************************************

// M a c r o s 

//****************************************************************************

#define Distance(cluster, pix, distance)													\
	{																									\
	short int    			channelTemp;														\
	CMeanType2   			normDistanceTemp;													\
	CType  					*pixPtr;																\
	CMeanType   			*clusterMeanPtr;													\
																										\
	pixPtr = pix;																					\
	clusterMeanPtr = cluster->meanPtr;														\
	distance = 0;																					\
	for (channelTemp = 0; channelTemp < numberChannels; channelTemp++)			\
		{																								\
		normDistanceTemp = (CMeanType2)*pixPtr - 											\
												(CMeanType2)*clusterMeanPtr;					\
		distance += (double)normDistanceTemp * normDistanceTemp;						\
		pixPtr++;																					\
		clusterMeanPtr++;																			\
	   }																								\
	}
	
//****************************************************************************

			// Prototypes for file routines that are only called from other 		
			// routines in this file.														

Boolean	ClusterDialog (
				FileInfoPtr							fileInfoPtr, 
				Boolean								newProjectFlag);

void		ClusterDialogItems9to11 (
				DialogPtr							dialogPtr, 
				SInt16								itemHit);
								
Boolean	CreateMaskFileClassNames (
				FileInfoPtr							maskFileInfoPtr,
				SInt32								numberClasses);

SInt32 	DeleteCluster (
				SInt32								clusterCount,
				ClusterType*						clusterToDelete,
				ClusterType*						previousCluster);

void		GetMinimumAndMaximumValueForListing (
				ClusterType*						clusterHead,
				SInt16								numberChannels,
				Boolean								stdDevFlag,
				double*								numberEDecimalDigitsPtr,
				double*								numberFDecimalDigitsPtr);	

PascalVoid DrawClusterDiskFilePopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber);

PascalVoid DrawSaveStatsPopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber);
								
SInt32 	FindSmallestCluster (
				SInt32								numberClustersToSearch,
				ClusterType**						minClusterPtr,
				ClusterType**						previousClusterPtr);	

double 	GetAverageChannelStandardDev (
				Handle								windowInfoHandle,
				FileInfoPtr							fileInfoPtr);

Boolean 	GetClusterAreaStatistics (
				FileIOInstructionsPtr			fileIOInstructionsPtr, 
				ProjectInfoPtr						projectClassInfoPtr, 
				SInt16								storageIndex, 
				HUInt16Ptr*							dataClassPtrPtr, 
				SInt16								firstLineCode);
									
Boolean 	GetClusterProjectStatistics (
				FileIOInstructionsPtr			fileIOInstructionsPtr, 
				ProjectInfoPtr						projectClassInfoPtr, 
				SInt16								numberClasses, 
				SInt16								fieldStatStorage);
								
SInt16 	GetMeanStdDevLength (
				double								maxDataValue, 
				double								minDataValue,
				SInt16								numberFDecimalDigits,
				SInt16								numberEDecimalDigits);

Boolean 	GetProbabilityFile (
				FileInfoPtr							fileInfoPtr,
				Handle*								probabilityFileInfoHPtr);
								
Boolean 	ListClusterInputParameters (
				FileInfoPtr							fileInfoPtr,
				CMFileStream*						resultsFileStreamPtr);

Boolean 	LoadClusterSpecs (
				FileInfoPtr							fileInfoPtr);



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 CheckIfTextForTextWindowIsWithinLimits
//
//	Software purpose:	The purpose of this routine is to determine if the amount of
//							text in a line of the cluster statistics is more than a defined
//							limit. If so a message is presented to the user to allow them
//							a chance to save instead to a disk file.
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:	1 if text is within limits
//							0 if the number of characters in the line is outside of the limit
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/23/2012
//	Revised By:			Larry L. Biehl			Date: 02/24/2012	

SInt16 CheckIfTextForTextWindowIsWithinLimits (
				Handle								okHandle,
				UInt32								possibleNumberChannels,
				double								maxDataValue,
				double								minDataValue,
				Boolean								writeToDiskFileFlag)

{
	SInt16								returnCode = 1;
	
	UInt16								numberEDecimalDigits,											
											numberFDecimalDigits,
											numChars,
											numChars2;
											
											
		
	GetNumberDecimalDigits (kRealType,
									maxDataValue,
									minDataValue,
									1,
									&numberEDecimalDigits,
									&numberFDecimalDigits);

	numChars = LoadRealValueString ((char*)gTextString3,
												maxDataValue,
												0,
												numberFDecimalDigits,
												numberEDecimalDigits,
												(char*)"",
												(char*)"");

	numChars2 = LoadRealValueString ((char*)gTextString3,
												minDataValue,
												0,
												numberFDecimalDigits,
												numberEDecimalDigits,
												(char*)"",
												(char*)"");
												
			// Allow for space for tab.
												
	numChars = MAX (numChars, numChars2) + 1;
	
			// Allow for cluster number and area columns
			
	if (numChars * (possibleNumberChannels + 1) + 5 > kmaxNumberCharactersForLine)
		{
		HiliteControl ((ControlHandle)okHandle, 255);
		
		if (writeToDiskFileFlag)
			DisplayAlert (kErrorAlertID, 
								kCautionAlert, 
								kAlertStrID, 
								IDS_Alert141,
								0, 
								NULL);
								
		else	// !writeToDiskFileFlag
			{
			returnCode = DisplayAlert (kContinueCancelAlertID, 
												kCautionAlert,
												kAlertStrID, 
												IDS_Alert141,
												IDS_Alert142,
												NULL);
			
			if (returnCode == 2)										
				returnCode = 0;
			
			}	// end "else !writeToDiskFileFlag"

		HiliteControl ((ControlHandle)okHandle, 0);
		
		}	// end "if (numChars * (possibleNumberChannels + 1) + ..."
	
	return (returnCode);

}	// end "CheckIfTextForTextWindowIsWithinLimits"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ClusterClassification
//
//	Software purpose:	The purpose of this routine is to classify the
//							selected area(s) into the cluster classes that
//							have been set up previously using a Euclidean
//							distance measure.
//
//	Parameters in:		Pointer to image file information structure
//							Number of clusters to use for classification
//							Pointer to buffer to use to store classification
//									results for a line in before writing to disk
//									and/or text window.
//
//	Parameters out:	None
//
// Value Returned:	True if everything worked okay
//							False if a problem came up such as shortage of memory
// 
// Called By:			ClusterOnePassControl in clusterSinglePass.c
//							ISODATAClusterControl in clusterISODATA.c
//
//	Coded By:			Eric E. Demaree		Date: Spring 1989
//	Revised By:			Larry L. Biehl			Date: 12/21/2016	

Boolean ClusterClassification (
				FileIOInstructionsPtr			fileIOInstructionsPtr, 
				SInt16								numberClusters,
				HUCharPtr							classifyBufferPtr)

	{
			// Define local structures and variables.	
	
	double   							closestDistance,	// Distance from pix to  		
																	//  closest Cluster.				
			   							currentDistance,	// Distance from pix to  		
																	//	 current Cluster.				
  											maxDistance,		// Maximum euclidean distance	
  																	//  squared that pixel can be 	
  																	//  from the closest class and 
  																	//  still be classified.			
											minutesLeft;
   		
   ClusterType 						*closestCluster,	// cluster closest to current 	
   																//  pixel. 								
   										*clusterHead,
   										*currentCluster;	// Cluster currently working on.	
																	//  pixel.	
															
	CMFileStream*						clResultsFileStreamPtr;
	
	HCTypePtr     						currentPixel,		// Vector pointer for current 	
				  							outputBufferPtr;	// Ptr to pixels. 
				  							
	HUInt16Ptr							maskBufferPtr;	
									
	SInt16								*clusterClassPtr;
									
	UInt16								*channelsPtr;	
   
  	unsigned char  					*binPtr,
											*symbolsPtr;
				  										
	Point									point;
	Ptr									stringPtr;
	RgnHandle							rgnHandle;
	
	SInt32	 							column,
											columnEnd,
											columnInterval,
											columnStart,
											line,
											lineCount,
											lineEnd,
											lineInterval,
											linesLeft,
					 						lineStart,
		 									numberClassifiedColumns,
		  									numberPixelsNotClassified,
											startTick,
		 									stringLength;
 									
 	UInt32								numberClusterClasses;
   
   SInt16								areaNumber,
				         				closestClusterNumber, // Number of closest cluster 
				         				currentClusterNumber, // Number of current cluster	
											errCode,
											fieldNumber,
											lastClassIndex,
											lastFieldIndex,      
 											numberChannels,
 											pointType,
 											totalNumberAreas;		
 											
 	UInt16								maskRequestValue;								
   
   Boolean								continueFlag,
											includePixelFlag,
											returnFlag = TRUE; 
   

   		// Cluster classify the requested image area(s) according to the 		
   		// clusters already found if requested. 										
   
   if (gClusterSpecsPtr->classificationArea != 0 &&
				gClusterSpecsPtr->clusterHead != NULL &&
					fileIOInstructionsPtr->fileInfoPtr != NULL)
   	{                      
				// Initialize local variables. 												
      
      continueFlag = TRUE;                     
		fieldNumber = 0;
		lastClassIndex = -1;
		lastFieldIndex = -1;
		numberPixelsNotClassified = 0;
   	numberChannels 	= gClusterSpecsPtr->numberChannels;
		columnInterval		= gClusterSpecsPtr->imageColumnInterval;
		lineInterval		= gClusterSpecsPtr->imageLineInterval;
		channelsPtr 		= (UInt16*)GetHandlePointer (
																gClusterSpecsPtr->channelsHandle);
		symbolsPtr			= (UCharPtr)GetHandlePointer (
																gClusterSpecsPtr->symbolsHandle);
							
		numberClusterClasses = gClusterSpecsPtr->numberClusterClasses;
		clusterClassPtr = (SInt16*)GetHandlePointer (
															gClusterSpecsPtr->clusterClassHandle); 
		
		clusterHead 		= gClusterSpecsPtr->clusterHead;
		clResultsFileStreamPtr 	= GetResultsFileStreamPtr (0);
			
		maxDistance = (double)gClusterSpecsPtr->numberChannels *
													(gClusterSpecsPtr->classifyThreshold *
																gClusterSpecsPtr->classifyThreshold);
	
		outputBufferPtr = (CType*)gOutputBufferPtr;
		 
				// Display number of clusters being used.									
		 								
		LoadDItemValue (gStatusDialogPtr, IDC_Status16, (SInt32)numberClusters);
	
				// Get total number of cluster areas.	
			 
		if (gClusterSpecsPtr->classificationArea == 1)
			{
					// Use the same areas that were used to create the clusters.
					// If no training areas were used to create the clusters,
					// then use all training areas.
					
			totalNumberAreas = gClusterSpecsPtr->totalNumberAreas; 
					
			if (gClusterSpecsPtr->clustersFrom != kTrainingType)
				{ 		
				numberClusterClasses = gProjectInfoPtr->numberStatisticsClasses;
				clusterClassPtr = NULL;   
			
				totalNumberAreas = GetNumberOfAreas (clusterClassPtr,
																	numberClusterClasses, 
																	kTrainingType,
																	kDontIncludeClusterFields);
				
				}	// end "if (gClusterSpecsPtr->clustersFrom != kTrainingType)" 
			
						// "\pClassifying Training Areas."
			LoadDItemStringNumber (kClassifyStrID, 
											IDS_Classify15,
											gStatusDialogPtr, 
											IDC_Status11, 
											(Str255*)gTextString);
			LoadDItemValue (gStatusDialogPtr, 
									IDC_Status5, 
									(SInt32)totalNumberAreas);
			
			ShowStatusDialogItemSet (kStatusClassB);
			HideStatusDialogItemSet (kStatusMinutes);
			
			}	// end "if (gClusterSpecsPtr->classificationArea == 1)" 
			
		if (gClusterSpecsPtr->classificationArea == 2)
			{
			totalNumberAreas = 1;
			
					// (Str255*)"\pClassifying Selected Image Area.");
			LoadDItemStringNumber (kClassifyStrID, 
											IDS_Classify17,
											gStatusDialogPtr, 
											IDC_Status11, 
											(Str255*)gTextString);
										
			HideStatusDialogItemSet (kStatusClassB);
			ShowStatusDialogItemSet (kStatusMinutes);
			
			}	// end "if (clusterSpecsPtr->classificationArea == 2)" 
	
				// List the number of cluster classes used.								
				
		continueFlag = MGetString (gTextString2, kFileIOStrID, IDS_NumberClasses);
		
		sprintf ((char*)gTextString, 
					"%s%s %d%s",
					gEndOfLine, 
					&gTextString2[1], 
					(int)(numberClusters+1),
					gEndOfLine);
		continueFlag = OutputString (clResultsFileStreamPtr,
												(char*)gTextString, 
												(short int)0, 
												gOutputCode, 
												continueFlag);
	
				// List the cluster classes and symbols used.							
		
		if (continueFlag)
			{	
			continueFlag = MGetString (gTextString2, kProjectStrID, IDS_Project2);
			
			sprintf ((char*)gTextString, 
						"%s %s%s", 
						gEndOfLine, 
						&gTextString2[1],
						gEndOfLine);
			continueFlag = OutputString (clResultsFileStreamPtr,
													(char*)gTextString, 
													0, 
													gOutputCode,
													continueFlag);
									
			}	// end "if (continueFlag)" 
			
		if (!continueFlag)
																							return (FALSE); 
			
				// Include the "thresholded" class.											
																						
		sprintf ((char*)gTextString2, "Thresholded                     ");
		sprintf ((char*)gTextString, 
					"    %3d: %s %c%s", 
					0, 
					gTextString2,
					symbolsPtr[0],
					gEndOfLine);
		if (!OutputString (clResultsFileStreamPtr,
									(char*)gTextString, 
									0, 
									gOutputCode,
									TRUE))
																							return (FALSE);
		
		sprintf ((char*)gTextString2, "Cluster                        ");	
		for (currentClusterNumber=1; 
				currentClusterNumber<=numberClusters; 
					currentClusterNumber++)
			{
			NumToString ((UInt32)currentClusterNumber, gTextString3);
			BlockMoveData ((Ptr)&gTextString3[1], 
									(Ptr)&gTextString2[8],
									(SInt32)gTextString3[0]);
			sprintf ((char*)gTextString, 
						"    %3d: %s  %c%s", 
						currentClusterNumber, 
						gTextString2,
						symbolsPtr[currentClusterNumber],
						gEndOfLine);
																		
			if (!OutputString (clResultsFileStreamPtr,
										(char*)gTextString, 
										0, 
										gOutputCode, 
										TRUE))
																							return (FALSE);
			
			}	// end "for (currentClusterNumber=1;..."
		
				// Insert a blank line
			
		continueFlag = OutputString (clResultsFileStreamPtr, 
												(char*)gEndOfLine, 
												0, 
												gOutputForce1Code,
												continueFlag);

				// Intialize the nextTime variable to indicate when the next 		
				// check should occur for a command-.										
				
		gNextTime = TickCount ();
		
				// Loop by number of cluster areas.											
			
		for (areaNumber=1; areaNumber<=totalNumberAreas; areaNumber++)
			{
					// Initialize status variables.											
					
			lineCount = 0;
			gNextStatusTime = TickCount ();
			gNextMinutesLeftTime = 0;
						
					// If no status dialog is available then set 'gNextMinutesLeftTime'		
					// so that the time left is not computed and displayed in		
					// the status dialog window.												
																				
			if (gStatusDialogPtr == NULL)																
				gNextMinutesLeftTime = ULONG_MAX;
			
					// Get cluster area and list cluster area identification 		
					// information.																
					
			if (gClusterSpecsPtr->classificationArea == 1)
				{
						// Set 'gNextMinutesLeftTime' so that time left will not be				
						// calculated.  It is not needed for classification of		
						// training areas.														
				
				gNextMinutesLeftTime = ULONG_MAX;
			
						// Put number of cluster area being done in the status box.	
						
				LoadDItemValue (gStatusDialogPtr, IDC_Status3, (SInt32)areaNumber);
			
						// Clusters to come from training areas.							
						
				fieldNumber = GetNextFieldArea (gProjectInfoPtr,
															clusterClassPtr, 
															numberClusterClasses, 
															&lastClassIndex, 
															lastFieldIndex, 
															kTrainingType,
															kDontIncludeClusterFields);
						
				if (fieldNumber < 0)
																							return (FALSE);
																						
				lastFieldIndex = fieldNumber;
				
						// Get the field coordinates.											
							
				GetFieldBoundary (gProjectInfoPtr, &gAreaDescription, fieldNumber);
			
						// Initialize local image area variables.
				
				lineStart = gAreaDescription.lineStart;
				lineEnd = gAreaDescription.lineEnd;
				columnStart = gAreaDescription.columnStart;
				columnEnd = gAreaDescription.columnEnd;
				rgnHandle = gAreaDescription.rgnHandle;
							
				continueFlag = ListSpecifiedStringNumber (kClusterStrID, 
																		IDS_Cluster1,
																		clResultsFileStreamPtr, 
																		gOutputCode, 
																		(SInt32)areaNumber, 
																		continueFlag);
				
				if (continueFlag)
					continueFlag = ListFieldInformation (
										fieldNumber, 0, 0, lineInterval, columnInterval, 3);
				
				}	// end "if (gClusterSpecsPtr->classificationArea == 1)" 
				
			else if (gClusterSpecsPtr->classificationArea == 2)
				{
						// Clusters to come from selected area.							
				
				lineStart = gClusterSpecsPtr->imageLineStart;
				lineEnd = gClusterSpecsPtr->imageLineEnd;
		  		columnStart = gClusterSpecsPtr->imageColumnStart;
		   	columnEnd = gClusterSpecsPtr->imageColumnEnd;
				rgnHandle = NULL;
				
						// "Classification of Selected Area"
				continueFlag = ListSpecifiedStringNumber (kClassifyStrID, 
																		IDS_Classify14, 	
																		(UCharPtr)gTextString, 
																		clResultsFileStreamPtr, 
																		gOutputCode,
																		TRUE);
	
						// List line and column number of area being classified.	
						
						// "  Lines %ld to %ld by %ld.  Columns %ld to %ld by %ld.%s"
				continueFlag = ListLineColumnIntervalString (clResultsFileStreamPtr, 
																			gOutputCode,
																			lineStart, 
																			lineEnd, 
																			lineInterval,
																			columnStart, 
																			columnEnd, 
																			columnInterval, 
																			continueFlag);
				
				}	// end "else if (gClusterSpecsPtr->classificationArea == 2)" 
							
			if (!continueFlag)
				{
				CloseUpAreaDescription (&gAreaDescription);		
																							return (FALSE);
																					
				}	// end "if (!continueFlag)"
			
					// Get the point type for the area.
					
			pointType = kRectangleType;		
			if (lastFieldIndex > -1 && gProjectInfoPtr != NULL)
				pointType = gProjectInfoPtr->fieldIdentPtr[lastFieldIndex].pointType;
						
					// Right carriage return and null terminator to correct			
					// location in line.															
			
			numberClassifiedColumns =
									(columnEnd - columnStart + columnInterval)/columnInterval;
			stringLength = numberClassifiedColumns + gNumberOfEndOfLineCharacters;
			classifyBufferPtr[numberClassifiedColumns] = gEndOfLine[0];
			
			#if defined multispec_win || defined multispec_lin      
				classifyBufferPtr[numberClassifiedColumns+1] = gEndOfLine[1];
			#endif	// defined multispec_win || defined multispec_lin 
			
			classifyBufferPtr[stringLength] = kNullTerminator;
		
					// Get lines left to do.													
			
			linesLeft = (lineEnd - lineStart + lineInterval)/lineInterval;
			LoadDItemValue (gStatusDialogPtr, IDC_Status10, linesLeft);
			LoadDItemStringNumber (kDialogStrID, 
											IDS_Dialog18,		// Blank string
											gStatusDialogPtr, 
											IDC_Status14, 
											(Str255*)gTextString);
			
					// Initialize time left variables.
					
			startTick = TickCount ();
			if (gNextMinutesLeftTime != ULONG_MAX)
				gNextMinutesLeftTime = startTick + 3*gNextStatusTimeOffset;	
			
					// Load some of the File IO Instructions structure that pertain
					// to the specific area being used.
	
			errCode = SetUpFileIOInstructions (fileIOInstructionsPtr,
															&gAreaDescription,
															lineStart,
															lineEnd,
															lineInterval,
															columnStart,
															columnEnd,
															columnInterval,
															numberChannels, 
															channelsPtr,
															kDetermineSpecialBILFlag);
														
			maskRequestValue = (UInt16)fileIOInstructionsPtr->maskValueRequest;						
			
					// Loop by rest of lines for cluster area.							
			
			for (line=lineStart; line<=lineEnd; line+=lineInterval)
				{
						// If this in an image area, update dialog status 				
						// information.															
		
				lineCount++;
				if (TickCount () >= gNextStatusTime)
					{
					LoadDItemValue (gStatusDialogPtr, IDC_Status8, lineCount);
					gNextStatusTime = TickCount () + gNextStatusTimeOffset;
					
					}	// end "if (TickCount () >= gNextStatusTime)" 
				
						// Exit routine if user has "command period" down.				
				
				if (TickCount () >= gNextTime)
					{ 
					continueFlag = CheckSomeEvents (
												osMask+keyDownMask+updateMask+mDownMask+mUpMask);            
					if (!continueFlag)
						break;
						
					}	// end "if (TickCount () >= nextTime)" 
												
						// Set vertical (line) point in case it is needed for	
						// polygonal field.															
						
				point.v = (SInt16)line;	
																						
						// Get all requested channels for first line of image data.	
						// Return if there is a file IO error.								
						 
				errCode = GetLineOfData (fileIOInstructionsPtr,
													line, 
													columnStart,
													columnEnd,
													columnInterval,
													gInputBufferPtr,
													(HUCharPtr)outputBufferPtr);
										
				if (errCode < noErr)
					{ 						
					continueFlag = FALSE;
					break;
																									
					}	// end "if (errCode < noErr)"
					
				binPtr = classifyBufferPtr;
			
				if (errCode != kSkipLine)
					{
					currentPixel = outputBufferPtr;
					
					if (fileIOInstructionsPtr->maskBufferPtr != NULL)
						maskBufferPtr = &fileIOInstructionsPtr->maskBufferPtr[
															fileIOInstructionsPtr->maskColumnStart];
					
			   			// Loop through samples for the line. 								
			   		
			   	for (column = columnStart;
			       		column <= columnEnd;
								column += columnInterval)
						{					
								// Set horizotal (column) point in case it is needed 	
								// for polygonal field.														
							
						point.h = (SInt16)column;
						
						includePixelFlag = FALSE;
						if (pointType == kRectangleType)
							includePixelFlag = TRUE;
							
						else if (pointType == kPolygonType && PtInRgn (point, rgnHandle))
							includePixelFlag = TRUE;
						
						else if (pointType == kMaskType && 
																*maskBufferPtr == maskRequestValue)
							includePixelFlag = TRUE;
							
						if (includePixelFlag)
			   			{
					      closestCluster = clusterHead;
					      currentCluster = clusterHead->next;
					      currentClusterNumber = 2;
					      closestClusterNumber = 1;
					      Distance(closestCluster, currentPixel, closestDistance);
					
					      		// Find closest cluster in absolute sense 				
					      		// (Euclidean distance). 										
					      		
					      while (currentCluster && (currentClusterNumber <= 255))
					      	{
					         Distance(currentCluster, currentPixel, currentDistance);
					
					         if (currentDistance < closestDistance)
					         	{
					            closestCluster = currentCluster;
					            closestDistance = currentDistance;
					            closestClusterNumber = currentClusterNumber;
					            
					         	}	// end "if (currentDistance && ..." 
					
					         currentCluster = currentCluster->next;
					        	currentClusterNumber++;
					         
					      	}	// end " while (currentCluster != NULL)" 
					
					      		// Assign pixel to cluster class. 					
					      
					      if (closestDistance <= maxDistance)
					      	{
					      	(closestCluster->numPixInCluster)++;
					      	*binPtr = symbolsPtr[closestClusterNumber];
					      	
					      	}	// end "if (closestDistance <= maxDistance)" 
					      	
					      else	// closestDistance > maxDistance 
					      	{
					      	numberPixelsNotClassified++;
					      	*binPtr = kBlank;
					      	
					      	}	// end "else closestDistance > maxDistance" 
					      	
				   		currentPixel += numberChannels;
					         
					      }	// end "if (includePixelFlag)" 
					      
					   else	// !includePixelFlag"
					   	{ 
					   	*binPtr = kBlank;
					   	
							if (pointType != kMaskType)
								currentPixel += numberChannels;	
										   	
					   	}	// end "else !includePixelFlag""
					   	
					   binPtr++;
						
						if (pointType == kMaskType)
							maskBufferPtr += columnInterval;
			         
			  			}	// end "for (column=columnStart; ..." 
		  			
		  			}	// end "if (errCode != kSkipLine)"
		  			
		  		else	// errCode == kSkipLine
		  			{
			   	for (column = columnStart;
			       		column <= columnEnd;
								column += columnInterval)
			       	{
					   *binPtr = kBlank;
					   binPtr++;
					   
					   }	// end "for (column = columnStart; ..."
		  			
		  			}	// end "else errCode == kSkipLine"
		  		
				continueFlag = OutputString (clResultsFileStreamPtr,
														(char*)classifyBufferPtr, 
														stringLength, 
														gOutputCode,
														TRUE);
						
				if (!continueFlag)
					break;
				
				linesLeft--;
				if (TickCount () >= gNextMinutesLeftTime)
					{
					minutesLeft = (linesLeft * (TickCount () - startTick))/
																	(double)(lineCount*kTicksPerMinute);
				
					sprintf ((char*)gTextString, " %.1f", minutesLeft);
					stringPtr = (char*)CtoPstring (gTextString, gTextString);
					LoadDItemString (gStatusDialogPtr, IDC_Status14, (Str255*)gTextString);
					
					gNextMinutesLeftTime = TickCount () + gNextMinutesLeftTimeOffset;
					
					}	// end "if (TickCount () >= gNextMinutesLeftTime)" 
			
				if (pointType == kMaskType)
					fileIOInstructionsPtr->maskBufferPtr += 
												fileIOInstructionsPtr->numberMaskColumnsPerLine;
		  			
		  		}	// end "for (line=lineStart; line<=lineEnd; ...)"
		  				
			if (continueFlag)
				LoadDItemValue (gStatusDialogPtr, IDC_Status8, lineCount);
		
					// Close up any File IO Instructions structure that pertain to the 
					// specific area used for the list data.
					
			CloseUpFileIOInstructions (fileIOInstructionsPtr, &gAreaDescription);
							
					// Dispose of the region if it exists.						
	
			CloseUpAreaDescription (&gAreaDescription);
											
			if (!continueFlag)
																							return (FALSE);
	  			
	  		}	// end "for (areaNumber=1; areaNumber<=...)" 

	   		// List cluster classification statistics. 								
		
		if (!ListSpecifiedStringNumber (
										kClusterStrID, 
										IDS_Cluster2, 
										(unsigned char*)gTextString, 
										clResultsFileStreamPtr, 
										gOutputCode,
										continueFlag))
	   																					return (FALSE);
	   		
	   currentCluster = clusterHead;
	   currentClusterNumber = 0;
	   while (currentCluster && (currentClusterNumber <= 254))
	   	{
	   	currentClusterNumber++;
			
					// Print number of pixels in classified cluster.					
					
					// " Cluster %2ld classification size:  %ld%s"
			if (!ListSpecifiedStringNumber (
										kClusterStrID, 
										IDS_Cluster42, 
										clResultsFileStreamPtr, 
										gOutputCode,
	              					(SInt32)currentClusterNumber,
	              					currentCluster->numPixInCluster,
										continueFlag))
																							return (FALSE);
				
	      currentCluster = currentCluster->next;
	      
	   	}	// end "while (currentCluster&& ...)" 
			
				// List summary of number of pixels that were not classified.		
							
		if (!ListSpecifiedStringNumber (kClusterStrID, 
													IDS_Cluster3,
													clResultsFileStreamPtr, 
													gOutputCode, 
													numberPixelsNotClassified, 
													continueFlag))
																							return (FALSE);
   		
   	}	// end "if (gClusterSpecsPtr->classificationArea != 0  && ...)" 
  	
  	return (returnFlag);
  		
}	// end "ClusterClassification"
 


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ClusterControl
//
//	Software purpose:	The purpose of this routine is to control the user
//							requests for cluster operations on an image file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			Menus in menus.c
//
//	Coded By:			Larry L. Biehl			Date: 09/12/1989
//	Revised By:			Larry L. Biehl			Date: 12/07/2018

void ClusterControl (void)

{
			// Define local variables and structures.
	
	LongRect								boundingClusterArea;
			
	SInt16								*clusterClassPtr = NULL;										
	
	CMFileStream*						clResultsFileStreamPtr;
	FileInfoPtr							fileInfoPtr;
											
	UInt32								numberClusters;
	
	SInt16								overlayIndex;
	
	Boolean								continueFlag = TRUE;
											
	time_t								startTime;
	
		    		
			// If spare memory had to be used to load code resources, then exit	
			// routine.																				
			
	if (gMemoryTypeNeeded < 0)
																										return;
																							
			// Code resources loaded okay, so set flag back for non-Code			
			// resources.																			
			
	gMemoryTypeNeeded = 0;
 			
 			// Initialize local variables.													
 			
 	gStatusDialogPtr = NULL;
	
			// Check handle to image file information.  If handle to image			
			// file information doesn't exists. Find the	image file and get		
			// the information for it.															
				
	if (GetProjectImageFileInfo (kPrompt, kSetupGlobalInfoPointers))
		{
				// Lock handle to file information and get pointer to it				
		
		fileInfoPtr = gImageFileInfoPtr;
		
				// Set up cluster specification structure.								
				
		if (LoadClusterSpecs (fileInfoPtr))
			{
			if (ClusterDialog (fileInfoPtr, gProjectInfoPtr->newProjectFlag))
				{
						// Initialize some global variables pertaining to output.	
				
				gOutputTextOKFlag = TRUE;
				gOutputCode = gClusterSpecsPtr->outputStorageType;
				gOutputForce1Code = (gOutputCode | 0x0001);
				continueFlag = TRUE;
				
				if (gOutputCode & kCreateImageOverlayCode)
					{
							// Get memory needed for the overlay image.
							
					overlayIndex = -1;
					if (gClusterSpecsPtr->imageOverlayIndex >= 0 && 
							gClusterSpecsPtr->imageOverlayIndex < 
															(SInt16)gImageOverlayHandleListLength)
						overlayIndex = gClusterSpecsPtr->imageOverlayIndex;
		
							// Determine the number of clusters that will be involved.
							// If this is single pass cluster then one has to allow for
							// the maximum number possible.
							// If Project classes will be used as centers then include
							// them.
								
					numberClusters = gClusterSpecsPtr->maxNumberClusters;
					if (gClusterSpecsPtr->mode == kSinglePass || 
											(gClusterSpecsPtr->mode == kISODATA &&
													gClusterSpecsPtr->initializationOption == 3))
						numberClusters = kMaxNumberStatClasses;
						
					else if (gClusterSpecsPtr->mode == kISODATA &&
													gClusterSpecsPtr->projectClassMeansCode == 1)
						numberClusters += gProjectInfoPtr->numberStatTrainClasses;
					
					numberClusters = MIN (numberClusters, kMaxNumberStatClasses);
					
							// Get the bounding area to be clustered.
					
					clusterClassPtr = (SInt16*)GetHandlePointer (
															gClusterSpecsPtr->clusterClassHandle);
										
					if (GetProjectFieldsBoundingArea (
															gClusterSpecsPtr->clustersFrom, 
															clusterClassPtr, 
															gClusterSpecsPtr->numberClusterClasses, 
															gClusterSpecsPtr->clusterLineStart,
															gClusterSpecsPtr->clusterLineEnd,
															gClusterSpecsPtr->clusterColumnStart,
															gClusterSpecsPtr->clusterColumnEnd,
															&boundingClusterArea) != noErr)
						continueFlag = FALSE;
					
					if (continueFlag)
						gClusterSpecsPtr->imageOverlayIndex = SetUpImageOverlayInformation (
															FindProjectBaseImageWindowInfoHandle (),
															overlayIndex,
															numberClusters,
															&boundingClusterArea,
															kDefaultColors,
															gClusterSpecsPtr->transparency);
						
					if (!continueFlag || gClusterSpecsPtr->imageOverlayIndex <= -1)
								// Not possible to create an image overlay.
						gOutputCode -= kCreateImageOverlayCode;
							
					if (gOutputCode == 0)
						continueFlag = FALSE;
					
					}	// end "if (gOutputCode & kCreateImageOverlayCode)"
				
						// Update statistics for project if needed	
		
				if (gClusterSpecsPtr->mode == kISODATA &&
							gClusterSpecsPtr->projectClassMeansCode == 1 &&
									continueFlag)
					{
					//gProjectInfoPtr->numberStatTrainClasses;
					SInt16* initializationClassPtr = (SInt16*)GetHandlePointer (
													gClusterSpecsPtr->initializationClassHandle);
														
					continueFlag = VerifyProjectStatsUpToDate (
													&gClusterSpecsPtr->numberInitializationClasses,
													initializationClassPtr,
													1,
													gProjectInfoPtr->covarianceStatsToUse, 
													kSetupGlobalInfoPointers,
													NULL);
					
							// Update 'fileInfoPtr' in case it changed when updating
							// the statistics.
							 				
					fileInfoPtr = gImageFileInfoPtr;
									
					}	// end "if (gClusterSpecsPtr->mode == kISODATA && ..."			
				
						// If cluster classification is to go to a disk file, 		
						// open the	disk file.												
				
				if (continueFlag)
					{
					InitializeAreaDescription (&gAreaDescription, 
														1, 
														gImageWindowInfoPtr->maxNumberLines, 
														1, 
														gImageWindowInfoPtr->maxNumberColumns, 
														1, 
														1,
														gImageFileInfoPtr->startLine,
														gImageFileInfoPtr->startColumn,
														gClusterSpecsPtr->diskFileFormat);
						
					continueFlag = CreateResultsDiskFiles (
																gClusterSpecsPtr->outputStorageType,
																0,
																0);
												
					}	// end "if (continueFlag)"
			
 				clResultsFileStreamPtr = GetResultsFileStreamPtr (0);
 				
						// Continue clustering if everything is okay.					
				
				if (continueFlag)
					{
							// Change cursor to watch cursor until done with process	
						
					MSetCursor (kWait);
			
							// Force text selection to start from end of present text
						
					ForceTextToEnd ();
					
					continueFlag = ListClusterInputParameters (fileInfoPtr,
																				clResultsFileStreamPtr);
																	
							// Get dialog box to display cluster status.					
							
					if (continueFlag)
						gStatusDialogPtr = GetStatusDialog (kUpdateStatsInfoID, TRUE);
						
					if (gStatusDialogPtr == NULL)
						continueFlag = FALSE;
					
					else	// gStatusDialogPtr != NULL) 
						{
						LoadDItemStringNumber (kDialogStrID, 
														IDS_Dialog18, // Blank string
														gStatusDialogPtr, 
														IDC_Status11, 
														(Str255*)gTextString);
														
						LoadDItemStringNumber (kClusterStrID, 
														IDS_Cluster27,	// "Area:"
														gStatusDialogPtr, 
														IDC_Status2, 
														(Str255*)gTextString);
							
						LoadDItemStringNumber (kClusterStrID, 
														IDS_Cluster28,	// "Line:"
														gStatusDialogPtr, 
														IDC_Status7, 
														(Str255*)gTextString);
														
						ShowStatusDialogItemSet (kStatusCommand);
						
						if (gClusterSpecsPtr->clustersFrom == kTrainingType)
							ShowStatusDialogItemSet (kStatusClassB);
						
						}	// end "else gStatusDialogPtr != NULL)" 
					
					if (continueFlag)
						{
						startTime = time (NULL);
						
								// Turn spin cursor on
				
						gPresentCursor = kSpin;
						/*
								// Could not get this to work properly
						const EventTimerInterval	kSpinCursorAnimationInterval = 
																	kEventDurationMillisecond * 250.;
						EventLoopTimerUPP aTimerUPP = NewEventLoopTimerUPP (SpinCursorTimer);
						EventLoopTimerRef aTimerRef = NULL;
						
						InstallEventLoopTimer (GetCurrentEventLoop (),
														kSpinCursorAnimationInterval,
														kSpinCursorAnimationInterval,
														aTimerUPP,
														NULL,
														&aTimerRef);
						*/
						switch (gClusterSpecsPtr->mode)
							{
							case kSinglePass:			// One Pass Cluster 	
								continueFlag = ClusterOnePassControl (fileInfoPtr);
								break;
								
							case kISODATA:			// ISODATA Cluster 
								continueFlag = ISODATAClusterControl (fileInfoPtr);
								break;
								
							case 3:			// Future algorithm 
								break;
								
							case 4:			//	Future algorithm
								break;
								
							}	// end "switch (gClusterSpecsPtr->mode)" 
							
						if (gOutputCode & kCreateImageOverlayCode)
							{
									// Update the default image overlay name.
									
							GetDefaultImageOverlayName (gClusterSpecsPtr->imageOverlayIndex);
			
							UpdateOverlayControl (FindProjectBaseImageWindowPtr ());
							
							}	// end "if (gOutputCode & kCreateImageOverlayCode)"
	
								// Make sure that any image overlay handle is unlocked.
								
						UnlockImageOverlayInfoHandle (gClusterSpecsPtr->imageOverlayIndex, 0);
						
						//RemoveEventLoopTimer (aTimerRef);
						//DisposeEventLoopTimerUPP (aTimerUPP);
						
								// List message if memory was not available to complete the 
								// process
								
						ListMemoryMessage (clResultsFileStreamPtr);

								// Turn spin cursor off
					
						gPresentCursor = kWait;
					
								// Print the CPU time taken for the clustering.			
							
						continueFlag = ListCPUTimeInformation (clResultsFileStreamPtr, 
																			continueFlag, 
																			startTime);
							
						}	// end "if (continueFlag)" 
						
					//HideStatusDialogItemSet (kStatusField);
					//HideStatusDialogItemSet (kStatusCluster);
					//HideStatusDialogItemSet (kStatusMinutes);
					//HideStatusDialogItemSet (kStatusClassB);
			
							// Scroll output window to the selection and adjust the 	
							// scroll bar.															
					
					UpdateOutputWScrolls (gOutputWindow, 1, kDisplayMessage);
				
							// Dispose of cluster status dialog box.						
					
					CloseStatusDialog (TRUE);
					
							// Change cursor back to pointer.								
								
					MInitCursor ();
						
					}	// end "if (continueFlag)" 
					
						// Close the classification results file if needed and		
						// release the memory assigned to it.								
						
				CloseResultsFiles ();
				
				}	// end "if (ClusterDialog (fileInfoPtr, newProjectFlag))" 
				
			}	// end "if (LoadClusterSpecs ())" 
			
					// Unlock the memory for the cluster specifications.				
		
		if (gClusterSpecsPtr)
			{			
			CheckAndUnlockHandle (gClusterSpecsPtr->featureHandle);
			CheckAndUnlockHandle (gClusterSpecsPtr->channelsHandle);
			CheckAndUnlockHandle (gClusterSpecsPtr->symbolsHandle);
			CheckAndUnlockHandle (gClusterSpecsPtr->clusterClassHandle);
			CheckAndUnlockHandle (gClusterSpecsPtr->initializationClassHandle);
			CheckAndUnlockHandle (gClusterSpecsPtr->probabilityFileInfoHandle);
				
			}	// end "if (gClusterSpecsPtr)" 
			
				// Close the probability file if needed.									
				
		CloseImageFile (gClusterSpecsPtr->probabilityFileInfoHandle);
					
		CheckAndUnlockHandle (gNonProjProcessorSpecs.clusterSpecsH);
		gClusterSpecsPtr = NULL;
		
		UnlockProjectWindowInfoHandles ();
		
				// Make sure the cancel operation flag is off since the routine is now
				// done.
		
		gOperationCanceledFlag = FALSE;
		
		}	// end "if (GetProjectImageFileInfo (..." 
			
			// If this project was initialized at the beginning and it is 		
			// still a new project, then dispose of the project structure.		
	
	if (gProjectInfoPtr != NULL)
		{
		if (gProjectInfoPtr->newProjectFlag)
			{
 			CloseProjectStructure (gProjectInfoPtr);
			CloseProjectWindow ();
			
			}	// end "if (gProjectInfoPtr->newProjectFlag)"
			
		else	// !gProjectInfoPtr->newProjectFlag
			{
					// Make sure that the project window is open. It may not be if the
					// project was created by this run of clustering.
					
			if (gProjectWindow == NULL)
				CreateProjectWindow ();
			
			}	// end "else !gProjectInfoPtr->newProjectFlag"

		}	// end "if (gProjectInfoPtr != NULL)"

}	// end "ClusterControl" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ClusterDialog
//
//	Software purpose:	The purpose of this routine is to handle the 		
//							modal dialog for selecting the cluster
//							specifications. 
//
//	Parameters in:		Pointer to image file information structure
//							Flag indicating whether a new project was created for
//									clustering or whether the project already existed
//									before clustering was called.
//
//	Parameters out:	None
//
// Value Returned:	True if user selected OK in dialog box.
//							False if user selected cancel or memory was short.
//
// Called By:			ClusterControl   in cluster.c
//
//	Coded By:			Larry L. Biehl			Date: 09/14/1989
//	Revised By:			Larry L. Biehl			Date: 10/22/2018

Boolean ClusterDialog (
				FileInfoPtr							fileInfoPtr, 
				Boolean								newProjectFlag)

{
	Boolean								returnFlag;
									
#if defined multispec_mac
	DialogSelectArea					dialogSelectArea;
								
	double								classifyThreshold,
											saveThresholdPercent,
											thresholdPercent;
	
	
	Rect									maskDiskFilePopUpBox,
											theBox;
	
	DialogPtr							dialogPtr;
	
	//SInt16								*channels,
	//										*featurePtr;
											
	UInt16								*localActiveFeaturesPtr = NULL,
											*localFeaturesPtr = NULL;
	
	UCharPtr								localSymbolsPtr = NULL;
									
	UserItemUPP							drawDiskFilePopUpPtr,
											drawImageOverlayPopUpPtr,
											drawSaveStatsPopUpPtr;
	
	Handle								okHandle,
											theHandle;
	
	SInt32								theNum,
											transparencyValue;
											
	SInt16								channelListType,
											classificationArea,
											clusterMode,
											//fieldsExistCode,
											//index,
											itemHit,
											itemHit2,
											theType;
											
	UInt16								localActiveNumberFeatures,
											localNumberFeatures;

	Boolean								clusterMaskFileFlag,
											createImageOverlayFlag,
											modalDone,
											modeSetFlag,
											outputDiskFileFlag,
											outputWindowFlag,
											saveEntireImageFlag,
											thresholdFlag,
											updateDialogWindowFlag;

			
			// Initialize local variables.													
	/*			
	channels = (SInt16*)GetHandlePointer (gClusterSpecsPtr->channelsHandle);
							
	featurePtr = (SInt16*)GetHandlePointer (gClusterSpecsPtr->featureHandle);
							
	symbolsPtr = (UCharPtr)GetHandlePointer (gClusterSpecsPtr->symbolsHandle);
	*/	
	dialogPtr = NULL;
	returnFlag = GetDialogLocalVectors (&localFeaturesPtr,
														NULL,
														NULL,
														NULL,
														NULL,
														&localSymbolsPtr,
														NULL,
														NULL);
	
			// Get the modal dialog for the cluster specification.					
				
	if (returnFlag)
		dialogPtr = LoadRequestedDialog (kClusterSpecificationID, kCopyScrap, 1, 2);
		
	if (dialogPtr == NULL)
		{		
		ReleaseDialogLocalVectors (localFeaturesPtr,
											NULL,
											NULL,
											NULL,
											NULL,
											localSymbolsPtr,
											NULL,
											NULL);																
																							return (FALSE);
																					
		}	// end "if (dialogPtr == NULL)"
		
			// Intialize local user item proc pointers.									

	drawImageOverlayPopUpPtr = NewUserItemUPP (DrawImageOverlayPopUp);
	drawSaveStatsPopUpPtr = NewUserItemUPP (DrawSaveStatsPopUp);
	drawDiskFilePopUpPtr = NewUserItemUPP (DrawClusterDiskFilePopUp);
	
			// Set Procedure pointers for those dialog items that need them.
			
	SetDialogItemDrawRoutine (dialogPtr, 27, gDrawChannelsPopUpPtr);
	SetDialogItemDrawRoutine (dialogPtr, 29, gDrawSymbolsPopUpPtr);
	SetDialogItemDrawRoutine (dialogPtr, 31, drawSaveStatsPopUpPtr);
	SetDialogItemDrawRoutine (dialogPtr, 40, drawImageOverlayPopUpPtr);
	SetDialogItemDrawRoutine (
								dialogPtr, 41, &maskDiskFilePopUpBox, drawDiskFilePopUpPtr);

	ClusterDialogInitialize (dialogPtr,
										localFeaturesPtr,
										localSymbolsPtr,
										newProjectFlag,
										&clusterMode,
										&modeSetFlag,
										&classificationArea,
										&dialogSelectArea,
										&classifyThreshold,
										&gChannelSelection,
										&localActiveNumberFeatures,
										&gSymbolSelection,
										&gSaveStatisticsSelection,
										&outputWindowFlag,
										&outputDiskFileFlag,
										&clusterMaskFileFlag,
										&gOutputFormatCode,
										&createImageOverlayFlag,
										&gSelectImageOverlaySelection,
										&thresholdFlag,
										&thresholdPercent,
										&transparencyValue);
	
			// Set feature/transform feature parameters  

	InitializeDialogFeatureParameters (kNoTransformation,
													localActiveNumberFeatures, 
													fileInfoPtr->numberChannels, 
													0,
													localFeaturesPtr,
													NULL,
													&localNumberFeatures,
													NULL,
													&localActiveFeaturesPtr);
													
			// Save handle for the OK button for use later.								
			
	GetDialogItem (dialogPtr, 1, &theType, &okHandle, &theBox);
				
	SetDLogControl (dialogPtr, 5, clusterMode == 1);
	
			// ISODATA Cluster.														
			
	SetDLogControl (dialogPtr, 6, clusterMode == 2);
	
			// Set classification threshold value.											
			
	LoadDItemRealValue (dialogPtr, 25, classifyThreshold, -1);
																		
			//	Get the rectangles for the channel prompt and popup boxes.			
			// Draw the default channel selection.											
			
	channelListType = kSelectItemsList;
		
			// Cluster classification output to text output window.	
	
	if (outputWindowFlag)	
		SetDLogControl (dialogPtr, 33, 1);		
	
			// Cluster classification output to disk file.				
			
	if (outputDiskFileFlag)
		SetDLogControl (dialogPtr, 34, 1);		
	
			// Cluster classification output to disk file.				
			
	if (clusterMaskFileFlag)
		SetDLogControl (dialogPtr, 35, 1);	
	
			// Cluster classification output to disk file.				
			
	if (createImageOverlayFlag)
		SetDLogControl (dialogPtr, 36, 1);
	
			// Set threshold options.  														
		
	SetDLogControl (dialogPtr, 37, thresholdFlag);
		
	LoadDItemRealValue (dialogPtr, 38, thresholdPercent, 10);
		
			// Center the dialog and then show it.											
			
	ShowDialogWindow (dialogPtr, kClusterSpecificationID, kSetUpDFilterTable);
	
	gDialogItemDescriptorPtr[25] = kDItemReal;
	
			// Set default text selection to first edit text item						
			
	SelectDialogItemText (dialogPtr, 18, 0, INT16_MAX);
	
	modalDone = FALSE;
	updateDialogWindowFlag = FALSE;
	itemHit = 0;
	do 
		{
	   ModalDialog (gProcessorDialogFilterPtr, &itemHit);
	   	
		if (itemHit > 2)
			{
					// If itemHit was a number item, check for bad values.  If		
					// itemHit was a radio button make appropriate control 			
					// settings to indicate to the user the present selection.		
					// Get the handle to the itemHit.  For number value items, get	
					//	the string and	convert it to a number.								
					
			GetDialogItem (dialogPtr, itemHit, &theType, &theHandle, &theBox);
			if (theType == 16)
				{
				GetDialogItemText (theHandle, gTextString);	
				StringToNum (gTextString, &theNum);
				
				}	// end "if (theType == 16)" 
			
			switch (itemHit)
				{
				case 5:				// Use Single Pass cluster 
					saveEntireImageFlag = gEntireImageFlag;
					
					HiliteControl ((ControlHandle)okHandle, 255);
					if (OnePassClusterDialog (fileInfoPtr, dialogPtr))
						{
						SetControlValue ((ControlHandle)theHandle, 1);
						modeSetFlag = TRUE;
						
						SetDLogControl (dialogPtr, 6, 0);
						SetDLogControl (dialogPtr, 7, 0);
						
						}	// end "if (OnePassClusterDialog (fileInfoPtr, dialogPtr))" 
		
					if (!gAppearanceManagerFlag)
						SetDLogControlHilite (dialogPtr, 2, 0);
			
					HiliteControl ((ControlHandle)okHandle, 0);
						
					gEntireImageFlag = saveEntireImageFlag;
					updateDialogWindowFlag = TRUE;
					break;
						
				case 6:				// Use ISODATA cluster 
					saveEntireImageFlag = gEntireImageFlag;
					
					HiliteControl ((ControlHandle)okHandle, 255);
					if (ISODATAClusterDialog (fileInfoPtr, dialogPtr))
						{
						SetControlValue ((ControlHandle)theHandle, 1);
						modeSetFlag = TRUE;
						
						SetDLogControl (dialogPtr, 5, 0);
						SetDLogControl (dialogPtr, 7, 0);
						
						}	// end "if (ISODATAClusterDialog (fileInfoPtr, dialogPtr))" 
					HiliteControl ((ControlHandle)okHandle, 0);
						
					gEntireImageFlag = saveEntireImageFlag;
					updateDialogWindowFlag = TRUE;						
					break;
					
				case 7:				// Use cluster algorithm 3 
					break;
					
				case 9:		// No cluster classification 
				case 10:		// Classify training fields 
				case 11:		// Classify Image area. 
					ClusterDialogItems9to11 (dialogPtr, itemHit);
					break;
					
				case 3:				// Entire area to selected area switch.
				case 12:				// Entire area to selected area switch.			
				case 18:				//	 lineStart  
				case 19:				//	 lineEnd  
				case 20:				//	 lineInterval  		
				case 21:				//	 columnStart  
				case 22:				//	 columnEnd  
				case 23:				//	 columnInterval  
					DialogLineColumnHits (&dialogSelectArea,
													dialogPtr, 
													itemHit,
													theHandle,
													theNum);
					break;
				
				case 25:		// Classification threshold. 
				   if (theNum == 0 || theNum > fileInfoPtr->maxUsableDataValue)	
						RealNumberErrorAlert (gClusterSpecsPtr->classifyThreshold,
						 							dialogPtr, 
						 							itemHit,
													2);
					break;
					
				case 27:		// Selected channels 
					itemHit = ChannelsPopUpMenu (dialogPtr,
															27, 
															1, 
															gChannelSelection, 
															channelListType);
															
					if (itemHit == kSubsetMenuItem || 
								(itemHit == 1 && channelListType == kSelectedItemsListOnly))
						{
								// Subset of channels to be used.							
								
						HiliteControl ((ControlHandle)okHandle, 255);
						ChannelsDialog ((SInt16*)&localActiveNumberFeatures,
												(SInt16*)localActiveFeaturesPtr,
												gImageLayerInfoPtr,
												fileInfoPtr,
												channelListType,
												kNoTransformation,
												NULL,
												fileInfoPtr->numberChannels,
												gChannelSelection);
						HiliteControl ((ControlHandle)okHandle, 0);
						updateDialogWindowFlag = TRUE;
								
						}	// end "if (itemHit == kSubsetMenuItem || ..." 
					
					if (channelListType == kSelectItemsList && itemHit != 0)
						gChannelSelection = UpdateDialogNumberFeatureParameters (
																			kNoTransformation, 
																			&localActiveNumberFeatures,
																			&localNumberFeatures,
																			fileInfoPtr->numberChannels, 
																			NULL,
																			0,
																			itemHit);
	
							// Make certain that the correct label is drawn in the	
							// channel pop up box.												
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
					
				case 29:		// Symbol selection. 
					itemHit2 = StandardPopUpMenu (dialogPtr, 
															28, 
															29, 
															gPopUpSymbolsMenu, 
															gSymbolSelection, 
															kPopUpSymbolsMenuID);
													
					if (itemHit2 == kUserMenuItem)
						{
								// User specified symbols to be used.						
								
						HiliteControl ((ControlHandle)okHandle, 255);
						if (!SymbolsDialog (kMaxNumberStatClasses,
													NULL,
													localSymbolsPtr,
													TRUE))
							itemHit2 = gSymbolSelection;
							
						HiliteControl ((ControlHandle)okHandle, 0);
						updateDialogWindowFlag = TRUE;
							
						}	// end "if (itemHit2 == kUserMenuItem)" 
					
					if (itemHit2 != 0)
						gSymbolSelection = itemHit2;
	
							// Make certain that the correct label is drawn in the	
							// class pop up box.													
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
					
				case 31:		// Cluster statistics to: 
					itemHit2 = StandardPopUpMenu (dialogPtr, 
															30, 
															31, 
															gPopUpSaveStatsMenu, 
															gSaveStatisticsSelection,
															kPopUpSaveStatsMenuID);
													
					if (itemHit2 != 0)
						gSaveStatisticsSelection = itemHit2;
	
							// Make certain that the correct label is drawn in the	
							// class pop up box.													
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
					
				case 33:		// Write cluster classification to output window 
				case 34:		// Write cluster classification to disk file
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					break;
				 
				case 35:		// Write cluster mask image file 
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					
					gOutputFormatCode = -gOutputFormatCode;
					InvalWindowRect (GetDialogWindow (dialogPtr), &maskDiskFilePopUpBox);
					break;
					
				case 36:		// Create image overlay 
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					createImageOverlayFlag = !createImageOverlayFlag;
					
					if (createImageOverlayFlag)
						ShowDialogItem (dialogPtr, 40);
					else	// !createImageOverlayFlag
						HideDialogItem (dialogPtr, 40);
					break;
					
				case 37:		// Threshold the results. 
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					
					if (GetControlValue ((ControlHandle)theHandle))
						ShowDialogItems (dialogPtr, 38, 39);
					else	// !GetControlValue (theHandle) 
						HideDialogItems (dialogPtr, 38, 39);
					break;
					
				case 38:				// Threshold percent	
					thresholdPercent = GetDItemRealValue (dialogPtr, 38);
					if (thresholdPercent > kMaxThreshold)
						RealNumberErrorAlert (saveThresholdPercent, dialogPtr, 38, 10);
					else if (thresholdPercent >= 0 && thresholdPercent <= kMaxThreshold)
						saveThresholdPercent = thresholdPercent;
					break;
					
				case 40:			// select overlay index
					itemHit = StandardPopUpMenu (dialogPtr, 
															36, 
															40, 
															gPopUpImageOverlayMenu, 
															gSelectImageOverlaySelection, 
															kPopUpImageOverlayMenuID);
													
					if (itemHit != 0)
						gSelectImageOverlaySelection = itemHit;
															
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
					
				case 41:		// Disk file format 
					itemHit = DiskFilePopUpMenu (
													dialogPtr, gPopUpClusterMaskDiskFileMenu, 41);
					if (itemHit != 0)
						{
						gOutputFormatCode = itemHit;
						
						}	// end "if (itemHit != 0)"
					break;
						
				}	// end "switch (itemHit)" 
				
			}	// end "if (itemHit > 2)" 
			
		else if (itemHit > 0) 	// and itemHit <= 2 
			{		
			if (itemHit == 1 && !modeSetFlag)
				{
				HiliteControl ((ControlHandle)okHandle, 255);
				DisplayAlert (kErrorAlertID, 
									kStopAlert, 
									kAlertStrID, 
									IDS_Alert107,
									0, 
									NULL);
				HiliteControl ((ControlHandle)okHandle, 0);
									
				itemHit = 0;
				updateDialogWindowFlag = TRUE;
										
				}	// end "if (itemHit == 1 && !modeSetFlag)"
										
					// If item hit is 1, check if classify area line-column values	
					// make sense.  If they don't, sound an alert and make item 	
					// hit equal to 0 to allow user to make changes.					
					
			if (itemHit == 1 && GetDLogControl (dialogPtr, 11) == 1)
				itemHit = CheckLineColValues (&dialogSelectArea,
														gClusterSpecsPtr->imageLineStart,
														gClusterSpecsPtr->imageColumnStart,
														dialogPtr);
						
					// If item hit is 'OK', check if the pixels are to be 			
					// threshholded.  If so allow the user to select the 				
					// probability file to be used.											
					
			if (itemHit == 1 && GetDLogControl (dialogPtr, 37))
				{
				returnFlag = GetProbabilityFile (
								fileInfoPtr, &gClusterSpecsPtr->probabilityFileInfoHandle);
						
				if (!returnFlag)
					itemHit = 0;
							
				}	// end "if (itemHit == 1 && ..." 
				
					// Check if listing the stats in the output text window is more than
					// the define limit. If so present message to the user and allow them
					// a chance to save to a disk file.
					
			if (itemHit == 1)
				itemHit = CheckIfTextForTextWindowIsWithinLimits (
															okHandle,
															localNumberFeatures,
															fileInfoPtr->maxDataValue,
															fileInfoPtr->minDataValue,
															(Boolean)GetDLogControl (dialogPtr, 34));
																										
					// If item hit is 1, check if the save statistics option will	
					// require the current project to be saved.  Set item hit to	
					// 0 if the user cancels from the save.								
					
			if (itemHit == 1 && gSaveStatisticsSelection == 2)
				{
				if (gProjectInfoPtr->changedFlag)
					{
					if (SaveProjectFile (2) != 0)
						itemHit = 0;
												
					}	// end "if (gProjectInfoPtr->changedFlag)"
					
				}	// end "if (itemHit == 1 && ..." 
				
			if (itemHit == 1 && gSaveStatisticsSelection == 3)
				{
				if (fileInfoPtr->numberChannels != gClusterSpecsPtr->numberChannels ||
											gProjectInfoPtr->numberStatisticsChannels != 
																				localActiveNumberFeatures)
					{
					itemHit = DisplayAlert (
									kOKCancelAlertID, kCautionAlert, kAlertStrID, 11, 0, NULL);
					if (itemHit != 1)
						itemHit = 0;
						
					}	// end "if (fileInfoPtr->numberChannels != ..." 
					
				}	// end "if (itemHit == 1 && gSaveStatisticsSelection == 3)" 
				
			if	(itemHit == 1)      // User selected OK for information 
				{
				modalDone = TRUE;
				returnFlag = TRUE;
				
						// Cluster algorithm options.					

				if (GetDLogControl (dialogPtr, 5) == 1)
					gClusterSpecsPtr->mode = kSinglePass;

				else if (GetDLogControl (dialogPtr, 6) == 1)
					gClusterSpecsPtr->mode = kISODATA;

				else if (GetDLogControl (dialogPtr, 7) == 1)
					gClusterSpecsPtr->mode = 3;
				
				ClusterDialogOK (dialogPtr,
										gClusterSpecsPtr->mode,
										(Boolean)GetDLogControl (dialogPtr, 10),
										(Boolean)GetDLogControl (dialogPtr, 11),
										&dialogSelectArea,
										(double)GetDItemRealValue (dialogPtr, 25),
										gChannelSelection,
										(SInt16)localNumberFeatures,
										(SInt16*)localFeaturesPtr,
										gSymbolSelection,
										localSymbolsPtr,
										gSaveStatisticsSelection,
										(Boolean)GetDLogControl (dialogPtr, 33),
										(Boolean)GetDLogControl (dialogPtr, 34),
										(Boolean)GetDLogControl (dialogPtr, 35),
										gOutputFormatCode,
										createImageOverlayFlag,
										gSelectImageOverlaySelection,
										(Boolean)GetDLogControl (dialogPtr, 37),
										saveThresholdPercent,
										transparencyValue);

				}	// end "if (itemHit == 1)" 
				
			if	(itemHit == 2)      // User selected Cancel for information 
				{
				modalDone = TRUE;
				returnFlag = FALSE;
				
				}	// end "if	(itemHit == 2)" 
			
			}	// end "else if (itemHit > 0) and itemHit <= 2" 
			
		if (updateDialogWindowFlag)
			{		
					// Force the main dialog box to be redrawn.
					
			InvalWindowRect (GetDialogWindow (dialogPtr), 
									GetPortBounds (GetDialogPort (dialogPtr), &gTempRect));
								
			updateDialogWindowFlag = FALSE;
								
			}	// end "if (updateDialogWindowFlag)"
				
		}	while (!modalDone);
		
	ReleaseDialogLocalVectors (localFeaturesPtr,
										NULL,
										NULL,
										NULL,
										NULL,
										localSymbolsPtr,
										NULL,
										NULL);				
		
	DisposeUserItemUPP (drawSaveStatsPopUpPtr);
	DisposeUserItemUPP (drawImageOverlayPopUpPtr);
		
	CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
	
	CheckSomeEvents (activMask+updateMask);
#endif	// defined multispec_mac
									
	#if defined multispec_win               
		CMClusterDialog*		dialogPtr = NULL;
		
		TRY
			{ 
			dialogPtr = new CMClusterDialog (); 
			
			returnFlag = dialogPtr->DoDialog (newProjectFlag); 
		                       
			delete dialogPtr;
			}
			
		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			returnFlag = FALSE;
			}
		END_CATCH_ALL       
	#endif	// defined multispec_win

	#if defined multispec_lin               
		CMClusterDialog* dialogPtr = NULL;


		//dialogPtr = new CMClusterDialog (wxTheApp->GetTopWindow ());
		dialogPtr = new CMClusterDialog (NULL);

		returnFlag = dialogPtr->DoDialog (newProjectFlag);

		delete dialogPtr;
	#endif	// defined multispec_lin
	
	return (returnFlag);
	
}	// end "ClusterDialog" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ClusterDialogInitialize
//
//	Software purpose:	The purpose of this routine is to handle the initialization of
//							parameters for the cluster dialog box. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			ClusterDialog in SCluster.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/11/2003
//	Revised By:			Larry L. Biehl			Date: 09/05/2017	

void ClusterDialogInitialize (
				DialogPtr							dialogPtr,
				UInt16*								localFeaturesPtr, 
				unsigned char*						localSymbolsPtr,
				Boolean								newProjectFlag,
				SInt16*								clusterModePtr,
				Boolean*								modeSetFlagPtr,
				SInt16*								classificationAreaPtr,
				DialogSelectArea*					dialogSelectAreaPtr,
				double*								classifyThresholdPtr,
				SInt16*								channelSelectionPtr,
				UInt16*								localNumberChannelsPtr,
				SInt16*								symbolSelectionPtr,
				SInt16*								saveStatisticsSelectionPtr,
				Boolean*								outputWindowFlagPtr,
				Boolean*								outputDiskFileFlagPtr,
				Boolean*								clusterMaskFileFlagPtr,
				SInt16*								maskFileFormatCodePtr,
				Boolean*								createImageOverlayFlagPtr,
				SInt16*								selectImageOverlaySelectionPtr,
				Boolean*								thresholdFlagPtr,
				double*								thresholdPercentPtr,
				SInt32*								transparencyValuePtr)

{
	Handle								baseImageWindowInfoHandle;
	
	SInt16								fieldsExistCode,
											index;
	
		
			// Load the dialog local vectors

	LoadDialogLocalVectors (localFeaturesPtr,
										NULL,
										gClusterSpecsPtr->featureHandle,
										gClusterSpecsPtr->numberChannels,
										NULL,
										NULL,
										0,
										NULL,
										NULL,
										0,
										localSymbolsPtr,
										gClusterSpecsPtr->symbolsHandle,
										kMaxNumberStatClasses,
										NULL,
										NULL,
										0,
										NULL);
	
			//	Do not force a default cluster algorithm.	
			
	*clusterModePtr = gClusterSpecsPtr->mode;											
	
	*modeSetFlagPtr = TRUE;
	if (gClusterSpecsPtr->mode == 0)
		*modeSetFlagPtr = FALSE;
	
			// Algorithm 3.  Hide for now.										
			
	#if defined multispec_mac 
		HideDialogItem (dialogPtr, 7);
	#endif	// defined multispec_mac 
	
			// Definition of cluster classification area.								
			// None, training fields, or selected area									
			// Check if there are any training fields that are of the area 		
			// type, i.e. not cluster type.													
		
	index	= -1;			
	fieldsExistCode = GetNextFieldArea (gProjectInfoPtr,
													NULL, 
													gProjectInfoPtr->numberStatisticsClasses, 
													&index, 
													-1, 
													kTrainingType,
													kDontIncludeClusterFields);

	*classificationAreaPtr = gClusterSpecsPtr->classificationArea;										
	//if (fieldsExistCode == -1 || gClusterSpecsPtr->clustersFrom != kTrainingType)
	if (fieldsExistCode == -1)
		{
		#ifndef multispec_lin
			SetDLogControlHilite (dialogPtr, IDC_ClassifyTrainingAreas, 255);
		#endif
		#if defined multispec_lin
			wxRadioBox* classopt = 
									(wxRadioBox*)dialogPtr->FindWindow (IDC_ClassificationMap);
			classopt->Enable (1, false);
		#endif
		if (*classificationAreaPtr == 1)
			*classificationAreaPtr = 2;
			
		}	// end "if (fieldsExistCode == -1)"
				
			// Initialize selected area structure.	
			
	SInt16 entireIconItem = 12;
	if (gAppearanceManagerFlag)
		entireIconItem = 3;
	
	InitializeDialogSelectArea (dialogSelectAreaPtr,
											gImageWindowInfoPtr,
											gProjectSelectionWindow,
											gClusterSpecsPtr->imageColumnStart,
											gClusterSpecsPtr->imageColumnEnd,
											gClusterSpecsPtr->imageColumnInterval,
											gClusterSpecsPtr->imageLineStart,
											gClusterSpecsPtr->imageLineEnd,
											gClusterSpecsPtr->imageLineInterval,
											18,
											entireIconItem,
											kAdjustToBaseImage);
	
			// To entire image icon.															
			//	Load default area to be classified											
						
	#if defined multispec_mac 
		LoadLineColumnItems (dialogSelectAreaPtr, 
									dialogPtr, 
									kInitializeLineColumnValues, 
									kIntervalEditBoxesExist,
									1);
		
		if (gAppearanceManagerFlag)
			HideDialogItem (dialogPtr, 12);
		else	// !gAppearanceManagerFlag
			HideDialogItem (dialogPtr, 3);
	#endif	// defined multispec_mac  	
			
	#if defined multispec_win || defined multispec_lin
		LoadLineColumnItems (dialogSelectAreaPtr, 
									dialogPtr, 
									kDoNotInitializeLineColumnValues, 
									kIntervalEditBoxesExist,
									1);
	#endif	// defined multispec_win || defined multispec_lin      
						
			// Make changes needed related to items 9, 10, and 11						
			
	#if defined multispec_mac 
		ClusterDialogItems9to11 (dialogPtr, gClusterSpecsPtr->classificationArea+9);
	#endif	// defined multispec_mac  	
	
			// Set classification threshold value.	
			
	*classifyThresholdPtr = gClusterSpecsPtr->classifyThreshold;					
																	
			//	Get the rectangles for the channel prompt and popup boxes.			
			// Draw the default channel selection.											
			
	*channelSelectionPtr = gClusterSpecsPtr->channelSet;
	*localNumberChannelsPtr = gClusterSpecsPtr->numberChannels;
	
			// Symbols to use.												
			
	*symbolSelectionPtr = gClusterSpecsPtr->symbolSet;
	
	*saveStatisticsSelectionPtr = gClusterSpecsPtr->saveStatisticsCode + 1;
		
		// Set up feature to allow adding to an already existing project.
		
	if (newProjectFlag || !gFullVersionFlag)
		{
		#if defined multispec_lin
			wxChoice* clusterstats = 
									((wxChoice*)dialogPtr->FindWindow (IDC_ClusterStatsCombo));
			if (clusterstats->GetCount () > 2)
				clusterstats->Delete (2);
		#endif	// defined multispec_lin
		
		#if defined multispec_mac 
 			DisableMenuItem (gPopUpSaveStatsMenu, 3);
		#endif	// defined multispec_mac 
		
		#if defined multispec_win 
			((CComboBox*)dialogPtr->GetDlgItem (IDC_ClusterStatsCombo))->DeleteString (2);
		#endif	// defined multispec_win 
		
		}	// end "if (newProjectFlag || !gFullVersionFlag)"
		
	#if defined multispec_mac
 		else	// !newProjectFlag && gFullVersionFlag 
 			EnableMenuItem (gPopUpSaveStatsMenu, 3);
	#endif	// defined multispec_mac 
	
			// Set up capability to save to a new project.																			
			
	if (!gFullVersionFlag)
		{
		*saveStatisticsSelectionPtr = 1;
		
		#if defined multispec_lin
			wxChoice* clusterstats = 
									((wxChoice*)dialogPtr->FindWindow (IDC_ClusterStatsCombo));
			if (clusterstats->GetCount () > 1)
				clusterstats->Delete (1);
		#endif	// defined multispec_lin
		
		#if defined multispec_mac 
 			DisableMenuItem (gPopUpSaveStatsMenu, 2);
		#endif	// defined multispec_mac 
		 
		#if defined multispec_win 
			((CComboBox*)dialogPtr->GetDlgItem (IDC_ClusterStatsCombo))->DeleteString (1);
		#endif	// defined multispec_win 
		
		}	// end "if (!gFullVersionFlag)"
		
	#if defined multispec_mac
 		else	// gFullVersionFlag 
 			EnableMenuItem (gPopUpSaveStatsMenu, 2);
	#endif	// defined multispec_mac 
	
			// Cluster classification output to text output window.	
	
	*outputWindowFlagPtr = FALSE;
	if (gClusterSpecsPtr->outputStorageType & kOutputWindowFormatCode)
		*outputWindowFlagPtr = TRUE;
		
			// Cluster classification output to disk file.
			
	*outputDiskFileFlagPtr = FALSE;
	if (gClusterSpecsPtr->outputStorageType & kAsciiFormatCode)
		*outputDiskFileFlagPtr = TRUE;
	
			// Cluster mask file to disk file.		
		
	*maskFileFormatCodePtr = kClusterTIFFGeoTIFFOutputFormat;
	if (gClusterSpecsPtr->diskFileFormat == kErdas74Type)
		*maskFileFormatCodePtr = kClusterERDAS74OutputFormat;
	else if (gClusterSpecsPtr->diskFileFormat == kTIFFType)
		*maskFileFormatCodePtr = kClusterTIFFGeoTIFFOutputFormat;		
	
	*clusterMaskFileFlagPtr = FALSE;		
	if (gClusterSpecsPtr->outputStorageType & kClusterMaskCode)
		*clusterMaskFileFlagPtr = TRUE;      
		
	else	// !(lOutputStorageType & kClusterMaskCode)
		*maskFileFormatCodePtr = -*maskFileFormatCodePtr;
													
			// Set text indicating whether the output file format could be GeoTIFF
			// or TIFF
			
	SetTIFF_GeoTIFF_MenuItemString (gImageWindowInfoPtr,
												dialogPtr,
                                    #if defined multispec_lin
                                       NULL,
												#endif	// defined multispec_lin  
												#if defined multispec_mac  
													gPopUpClusterMaskDiskFileMenu,
												#endif	// defined multispec_mac
												#if defined multispec_win 
													IDC_DiskCombo,
												#endif	// defined multispec_win
												kClusterTIFFGeoTIFFOutputFormat);
	
			// Cluster map to image overlay.		
	
	*createImageOverlayFlagPtr = FALSE;
	baseImageWindowInfoHandle = FindProjectBaseImageWindowInfoHandle ();
	if (baseImageWindowInfoHandle != NULL)
		{
		if (gClusterSpecsPtr->outputStorageType & kCreateImageOverlayCode)
			*createImageOverlayFlagPtr = TRUE;

		}	// end "if (baseImageWindowInfoHandle != NULL)"
						
	else	// baseImageWindowInfoHandle == NULL
		SetDLogControlHilite (dialogPtr, IDC_ImageOverlay, 255);
					
	if (*createImageOverlayFlagPtr)
		ShowDialogItem (dialogPtr, IDC_ImageOverlayCombo);
	else	// !*createImageOverlayFlagPtr
		HideDialogItem (dialogPtr, IDC_ImageOverlayCombo);
	
	*selectImageOverlaySelectionPtr = GetWindowImageOverlayIndex (
														baseImageWindowInfoHandle,
														gClusterSpecsPtr->imageOverlayIndex) + 2;
				
	#if defined multispec_win 
		CComboBox* comboBoxPtr = 
								(CComboBox*)(dialogPtr->GetDlgItem (IDC_ImageOverlayCombo));
		gPopUpImageOverlayMenu = (MenuHandle)comboBoxPtr;
	#endif	// defined multispec_win
	 																				
	SetUpImageOverlayPopUpMenu (gPopUpImageOverlayMenu, 
											baseImageWindowInfoHandle,
											*selectImageOverlaySelectionPtr);
	
			// Set threshold options.  														
	
	*thresholdFlagPtr = gClusterSpecsPtr->thresholdFlag;
		
	*thresholdPercentPtr = gClusterSpecsPtr->probabilityThreshold;
	if (!gClusterSpecsPtr->thresholdFlag)
		{
		HideDialogItem (dialogPtr, IDC_UsePixelsThresholdValue);
		HideDialogItem (dialogPtr, IDC_Percent);
		
		}	// end "if (!gClusterSpecsPtr->thresholdFlag)"
		
	HideDialogItem (dialogPtr, IDC_UsePixelsThresholdFlag);
	HideDialogItem (dialogPtr, IDC_UsePixelsThresholdValue);
	HideDialogItem (dialogPtr, IDC_Percent);
		
	*transparencyValuePtr = gClusterSpecsPtr->transparency;
	
}	// end "ClusterDialogInitialize"


		
#if defined multispec_mac
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ClusterDialogItems9to11
//
//	Software purpose:	The purpose of this routine is to make the changes
//							needed for cluster dialog item 9, 10, and 11.
//
//	Parameters in:		dialog item that is to be handled.
//							Pointer to the dialog window.
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			ClusterDialog
//
//	Coded By:			Larry L. Biehl			Date: 08/17/1990
//	Revised By:			Larry L. Biehl			Date: 08/05/1996

void ClusterDialogItems9to11 (
				DialogPtr							dialogPtr, 
				SInt16								itemHit)

{		
	SetDLogControl (dialogPtr, 9, (short int)(itemHit == 9));
	SetDLogControl (dialogPtr, 10, (short int)(itemHit == 10));
	SetDLogControl (dialogPtr, 11, (short int)(itemHit == 11));
	
	if (itemHit == 9)
		{
		HideDialogItem (dialogPtr, 3);
		HideDialogItems (dialogPtr, 12, 25);
		
		}	// end "if (itemHit == 9)"
		
	else if (itemHit == 10)
		{
		HideDialogItem (dialogPtr, 3);
		HideDialogItems (dialogPtr, 12, 14);
		ShowDialogItems (dialogPtr, 15, 17);
		HideDialogItems (dialogPtr, 18, 19);
		ShowDialogItem (dialogPtr, 20);
		HideDialogItems (dialogPtr, 21, 22);
		ShowDialogItems (dialogPtr, 23, 25);
		
		SelectDialogItemText (dialogPtr, 20, 0, INT16_MAX);
		
		}	// end "else if (itemHit == 10)" 
						
	else	// itemHit == 11
		{
		ShowDialogItems (dialogPtr, 13, 25);
		
		if (gAppearanceManagerFlag)
			ShowDialogItem (dialogPtr, 3);
		else	// !gAppearanceManagerFlag
			ShowDialogItem (dialogPtr, 12);
		
		SelectDialogItemText (dialogPtr, 18, 0, INT16_MAX);
		
		}	// end "else itemHit == 11" 
		
}	// end "ClusterDialogItems9to11"		
#endif	// defined multispec_mac  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ClusterDialogOK
//
//	Software purpose:	The purpose of this routine is to handle the setting the cluster
//							structure parameters after the user has selected the OK button
//							in the main cluster dialog box. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			ClusterDialog in SCluster.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/28/2003
//	Revised By:			Larry L. Biehl			Date: 04/18/2013	

void 	ClusterDialogOK (
				DialogPtr							dialogPtr,
				SInt16								clusterMode,
				Boolean								trainingAreasFlag,
				Boolean								selectedAreaFlag,
				DialogSelectArea*					dialogSelectAreaPtr,
				double								classifyThreshold,
				SInt16								channelSelection,
				SInt16								localNumberFeatures,
				SInt16*								localFeaturesPtr,
				SInt16								symbolSelection,
				unsigned char*						localSymbolsPtr,
				SInt16								saveStatisticsSelection,
				Boolean								textWindowFlag,
				Boolean								outputDiskFileFlag,
				Boolean								clusterMaskFileFlag,
				SInt16								maskFileFormatCode,
				Boolean								createImageOverlayFlag,
				SInt16								selectImageOverlaySelection,
				Boolean								thresholdFlag,
				double								thresholdPercent,
				SInt32								transparencyValue)

{
			// Cluster algorithm options.					

	gClusterSpecsPtr->mode = clusterMode;
		
			// Area for cluster classification.			
	
	gClusterSpecsPtr->classificationArea = 0;
	if (trainingAreasFlag)				
		gClusterSpecsPtr->classificationArea += 1;
	if (selectedAreaFlag)				
		gClusterSpecsPtr->classificationArea += 2;
	
			// Cluster classification area.										
	
	gClusterSpecsPtr->imageLineStart = dialogSelectAreaPtr->lineStart;
	gClusterSpecsPtr->imageLineEnd = dialogSelectAreaPtr->lineEnd;
	gClusterSpecsPtr->imageLineInterval = dialogSelectAreaPtr->lineInterval;
													
	gClusterSpecsPtr->imageColumnStart = dialogSelectAreaPtr->columnStart;
	gClusterSpecsPtr->imageColumnEnd = dialogSelectAreaPtr->columnEnd;
	gClusterSpecsPtr->imageColumnInterval = dialogSelectAreaPtr->columnInterval;
								
			// Classification threshold.								
	
	gClusterSpecsPtr->classifyThreshold = classifyThreshold;
		
			// Cluster Statistics To.							
			
	gClusterSpecsPtr->saveStatisticsCode = saveStatisticsSelection - 1;
	/*
	if (gClusterSpecsPtr->saveStatisticsCode == kSaveStatisticsToNewProject)
		{
				// Need to reset the project channels pointer to represent the entire
				// set of channels for cluster subset to work properly.
				
		for (index=0; index<gImageFileInfoPtr->numberChannels; index++)
			gProjectInfoPtr->channelsPtr[index] = index;
							
		}	// end "if (gClusterSpecsPtr->saveStatisticsCode == kSaveStatisticsToNewProject)"
	*/														
			// Load some common processor parameters
			// 	Channels
			// 	Class symbols	
			
	LoadProcessorVectorsFromDialogLocalVectors (
														channelSelection,
														kNoTransformation,
														gImageFileInfoPtr->numberChannels,
														localNumberFeatures,
														(UInt16*)localFeaturesPtr,
														&gClusterSpecsPtr->channelSet,
														(UInt16*)&gClusterSpecsPtr->numberChannels,
														gClusterSpecsPtr->featureHandle,
														(UInt16*)&gClusterSpecsPtr->numberChannels,
														gClusterSpecsPtr->channelsHandle,
														0,
														0,
														NULL,
														NULL,
														NULL,
														NULL,
														symbolSelection,
														localSymbolsPtr,
														&gClusterSpecsPtr->symbolSet,
														gClusterSpecsPtr->symbolsHandle,
														0,
														NULL,
														NULL);
		
			// Write classification to output window.				
			
	gClusterSpecsPtr->outputStorageType = 0;							
	if (textWindowFlag)				
		gClusterSpecsPtr->outputStorageType += kOutputWindowFormatCode;
		
			// Write classification to disk file.					
	
	if (outputDiskFileFlag)				
		gClusterSpecsPtr->outputStorageType += kAsciiFormatCode;
	
			// Write cluster mask to image file.					
	
	if (clusterMaskFileFlag)	
		{
		switch (maskFileFormatCode)
			{
			case kClusterERDAS74OutputFormat:
				gClusterSpecsPtr->outputStorageType += kClusterMaskCode;
				gClusterSpecsPtr->diskFileFormat = kErdas74Type;
				break;
					
			case kClusterTIFFGeoTIFFOutputFormat:
				gClusterSpecsPtr->outputStorageType += kClusterMaskCode;
				gClusterSpecsPtr->diskFileFormat = kTIFFType;
				break;
				
			}	// end "switch (maskFileFormatCode)"
			
		gOutputFormatCode = 0;
		if (gClusterSpecsPtr->outputStorageType & kClusterMaskCode)
			gOutputFormatCode = gClusterSpecsPtr->diskFileFormat;
			
		}	// end "if (diskFileFlag)" 
		
			// Write cluster results to image overlay.
	
	if (createImageOverlayFlag)
		gClusterSpecsPtr->outputStorageType += kCreateImageOverlayCode;

	gClusterSpecsPtr->imageOverlayIndex = GetImageOverlayIndex (
															FindProjectBaseImageWindowInfoHandle (),
															selectImageOverlaySelection - 2);
			
			// Threshold results flag.												
			
	gClusterSpecsPtr->thresholdFlag = thresholdFlag;
	
	if (gClusterSpecsPtr->thresholdFlag)
		{
				// Get probability threshold.										
		
		gClusterSpecsPtr->probabilityThreshold = thresholdPercent;

		gClusterSpecsPtr->probabilityThresholdCode = GetProbabilityThresholdCode (
													thresholdPercent, 
													gClusterSpecsPtr->probabilityFileInfoHandle);
											
		}	// end "if (gClusterSpecsPtr->thresholdFlag)" 
		
	if (createImageOverlayFlag)
		gClusterSpecsPtr->transparency = (SInt16)transparencyValue;
		
}	// end "ClusterDialogOK"		



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CreateClusterMaskFile
//
//	Software purpose:	The purpose of this routine is to create a mask file that
//							contains the location of each of the final class clusters
//							that were generate. The output file will be a thematic gis
//							file that can be used as a training mask when loaded into
//							a project.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			ISODATAClusterControl in SClustID.cpp
//							ClusterOnePassControl in SClustSP.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/12/1999
//	Revised By:			Larry L. Biehl			Date: 05/18/2018

Boolean CreateClusterMaskFile (void)

{
	SInt64								writePosOff;
	
	ldiv_t								lDivideStruct;
	Point									point;
	RgnHandle							rgnHandle;
	
	CMFileStream*						maskFileStreamPtr;
	FileInfoPtr							maskFileInfoPtr;
	FileIOInstructionsPtr			fileIOInstructionsPtr;
	HUCharPtr				 			ioOutBufferPtr;
	
	HUInt16Ptr							dataClassPtr,
											maskBufferPtr;
											
	SInt16*								classToFinalClassPtr;
	
   SInt32	        					line,
											lineCount,
											lineEnd,
											lineInterval,
				 							lineStart,
											startTick;
	
	UInt32								column,
 											columnEnd,
 											columnInterval,
							 				columnStart,
							 				columnWidth,
											count,
											firstColumn,
											firstLineCode,
											firstLineCodeEnd,
											firstLineCodeStart,
											numberMaskColumns,
											supportFileType;
	
   SInt16								areaNumber,
											errCode,
											fieldNumber,
											finalClass,
											lastClassIndex,
											lastFieldIndex,
											pointType,
											totalNumberAreas;	
	
	UInt16								maskRequestValue;
											
	Boolean								continueFlag,
											includePixelFlag,
											skipLineFlag;
											
											
			// Verify some variables.
			
	if (gClusterSpecsPtr->dataClassPtr == NULL || 
							gClusterSpecsPtr->clusterClassToFinalClassPtr == NULL)
																							return (FALSE);
	
			// Initialize local variables.
			
	continueFlag = TRUE;
	
			// Change cursor to watch cursor until done with process.				
	
	MSetCursor (kWait);
		
			// Get pointer to the mask file stream structure.
			
	maskFileInfoPtr = GetResultsFilePtr (4);
	maskFileStreamPtr = 	GetFileStreamPointer (maskFileInfoPtr);
	
			// Update the number of classes.
			
	maskFileInfoPtr->numberClasses = gClusterSpecsPtr->numberFinalClusters + 1;
	maskFileInfoPtr->maxClassNumberValue = maskFileInfoPtr->numberClasses - 1;
	
			//	List the name of the mask file in the cluster output information.	
	
	char* maskFileCNamePtr = (char*)GetFileNameCPointerFromFileInfo (maskFileInfoPtr);
	ListSpecifiedStringNumber (kClusterStrID, 
										IDS_Cluster39,
										GetResultsFileStreamPtr (0), 
										gOutputForce1Code, 
										maskFileCNamePtr,
										TRUE,
										kUTF8CharString);
				
			// Create the base Thematic Image file with all background values.	
			
	if (continueFlag)	
		continueFlag = CreateBackgroundImageFile (maskFileInfoPtr, 
																gInputBufferPtr,
																kFromClassification,
																1,
																0,
																TRUE); 
	
			//	Set title and dialog items for changing pixels to class numbers.	

	if (continueFlag)
		{
		LoadDItemStringNumber (kClusterStrID,
										IDS_Cluster47,		// "\pCreating Cluster Mask File"
										gStatusDialogPtr, 
										IDC_Status11,
										(Str255*)gTextString); 
		
		if (gClusterSpecsPtr->totalNumberAreas > 1)
			{
			ShowStatusDialogItemSet (kStatusClassA);
			LoadDItemValue (gStatusDialogPtr,
									IDC_Status5,
									(SInt32)gClusterSpecsPtr->totalNumberAreas);
			
			}	// end "if (gClusterSpecsPtr->totalNumberAreas > 1)"
		
		ShowStatusDialogItemSet (kStatusField);
		
		CheckSomeEvents (updateMask);
		
		}	// end "if (continueFlag)" 
				
			// Now change those pixels that belong to one of the cluster classes.																	
	
	if (continueFlag)
		{
				// Initialize local variables. 													
	   
		totalNumberAreas = 		gClusterSpecsPtr->totalNumberAreas;
		lineInterval = 			gClusterSpecsPtr->clusterLineInterval;
		dataClassPtr = 			gClusterSpecsPtr->dataClassPtr;
	  	classToFinalClassPtr = 	gClusterSpecsPtr->clusterClassToFinalClassPtr;
	  	
		SetUpGeneralFileIOInstructions (&gFileIOInstructions[1],
													NULL,	
													NULL,
													maskFileInfoPtr,
													1,
													NULL,
													NULL,
													NULL,
													NULL,
													0,
													kDoNotPackData,
													kDoNotForceBISFormat,	
													kDoNotForceBytes,
													&fileIOInstructionsPtr);
	  	
				// Initialize parameters controlling the first line code which is
				// based on the algorithm that is being used.
		
		firstLineCodeStart = 1;
		firstLineCodeEnd = 0;
		
		if (gClusterSpecsPtr->mode == kSinglePass)
			{
					// Single pass clustering.
					
			firstLineCodeStart = 1;
			firstLineCodeEnd = 2;
																						
			}	// end "if (gClusterSpecsPtr->mode == kSinglePass)" 		
		
		else if (gClusterSpecsPtr->mode == kISODATA)
			{
					// ISODATA clustering.
					
			firstLineCodeStart = 0;
			firstLineCodeEnd = 0;
																						
			}	// end "if (gClusterSpecsPtr->mode == kISODATA)" 
							
				// Intialize the nextTime variable to indicate when the next check	
				// should occur for a command-.													
				
		gNextTime = TickCount ();
	  	
	  	for (firstLineCode=firstLineCodeStart; 
	  			firstLineCode<=firstLineCodeEnd; 
	  				firstLineCode++)
	  		{
			fieldNumber = 0;
			lastClassIndex = -1;
			lastFieldIndex = -1;
				
					// Loop by number of cluster areas.												
					
			for (areaNumber=1; areaNumber<=totalNumberAreas; areaNumber++)
				{
						// Initialize status variables.												
						
				lineCount = 0;
				
				if (totalNumberAreas > 1)
					{
					LoadDItemValue (gStatusDialogPtr, IDC_Status3, (SInt32)areaNumber);
					
					CheckSomeEvents (mUpMask);
					
					}	// end "if (totalNumberAreas > 1)"
				
						// Get information for next cluster area.									
						
				if (!GetNextClusterArea (gProjectInfoPtr, 
													NULL, 
													0, 
													areaNumber, 
													NULL, 
													&lastClassIndex, 
													&lastFieldIndex, 
													NULL))
																							return (FALSE);
					
				lineStart = gAreaDescription.lineStart;
				lineEnd = gAreaDescription.lineEnd;
		  		columnStart = gAreaDescription.columnStart;
		   	columnEnd = gAreaDescription.columnEnd;
				columnWidth = columnEnd - columnStart + 1;
				rgnHandle = gAreaDescription.rgnHandle;
				pointType =  gAreaDescription.pointType;	
					
				if (firstLineCode == 0)
					{
							// Initialization for isodata cluster.			
							
					columnInterval = gClusterSpecsPtr->clusterColumnInterval;
					
					}	// end "if (firstLineCode == 1)" 
					
				else if (firstLineCode == 1)
					{
							// Initialization for first line of single pass cluster.			
							
					lineEnd = lineStart;
					columnInterval = 1;
					
					}	// end "else if (firstLineCode == 1)" 
				
				else if (firstLineCode == 2)
					{
							// Initialization for rest of lines for single pass cluster.	
							
					lineCount = 1;
					gAreaDescription.lineStart += lineInterval;
					lineStart = gAreaDescription.lineStart;
					
					columnInterval = gClusterSpecsPtr->clusterColumnInterval;
					
							// Remember to update the mask buffer start so that it represents 
							// the same area as that for the the image area.
					
					if (pointType == kMaskType)
						gAreaDescription.maskLineStart += lineInterval;
					
					}	// end "else if (firstLineCode == 2)" 
					
						// Get first sample.																
			   	
				firstColumn = columnStart;
													
						// Get number of columns in output mask line.			
				
				numberMaskColumns = maskFileInfoPtr->numberColumns;
				
						// Get position of first write to file.
						
				writePosOff = maskFileInfoPtr->numberHeaderBytes + 
											(lineStart-1)*numberMaskColumns + columnStart - 1;
											
						// Now set 'numberMaskColumns' to represent the number to skip in
						// the output buffer when advancing from line to line.
						
				numberMaskColumns *= lineInterval;
					
						// Set up variables for controlling when status checks are made.														
									
				startTick = gNextStatusTime = TickCount ();	
					
						// Load some of the File IO Instructions structure that pertain
						// to the specific area being used.
						
				errCode = SetUpFileIOInstructions (fileIOInstructionsPtr,
																&gAreaDescription,
																1,
																NULL,
																kDetermineSpecialBILFlag);
																
				maskRequestValue = (UInt16)fileIOInstructionsPtr->maskValueRequest;		
														
						// If lineEnd is 0, then this implies that there are no requested 
						// mask values in the area being used. Force looping through the 
						// lines to be skipped.
						
				if (lineEnd == 0)
					lineStart = 1;							
				
						// Loop by rest of lines for cluster area.								
				
				for (line=lineStart; line<=lineEnd; line+=lineInterval)
					{
							// If this in an image area, update dialog status information.	
				
					lineCount++;
					if (TickCount () >= gNextStatusTime)
						{
						LoadDItemValue (gStatusDialogPtr, IDC_Status8, (SInt32)lineCount);
						gNextStatusTime = TickCount () + gNextStatusTimeOffset;
						
						}	// end "if (TickCount () >= gNextStatusTime)"
						
					skipLineFlag = FALSE;
					if (pointType == kMaskType)
						{
						maskBufferPtr = fileIOInstructionsPtr->maskBufferPtr;
						
						if (*maskBufferPtr == 0)
							skipLineFlag = TRUE;
							
						maskBufferPtr += gAreaDescription.maskColumnStart;
							
						}	// end "if (pointType == kMaskType)"
					
					if (!skipLineFlag)
						{ 
								// Exit routine if user has "command period" down.					
						
						if (TickCount () >= gNextTime)
							{
							continueFlag = CheckSomeEvents (
												osMask+keyDownMask+updateMask+mDownMask+mUpMask);
							if (!continueFlag)
								break;
								
							}	// end "if (TickCount () >= nextTime)" 
										
								// Initialize the output buffer to that already existing for 
								// that line. This is need in case polygonal field are being 
								// used.														
								
						errCode = GetLine (fileIOInstructionsPtr,
													maskFileInfoPtr,
													line,
													0,
													(UInt32)columnStart,
													(UInt32)columnEnd,
													&count,
													(HUCharPtr)gInputBufferPtr);
													
						if (errCode != noErr)
							{
							continueFlag = FALSE;
							break;
							
							}	// end "if (errCode != noErr)"
						
								// Set point in case it is needed for a polygonal field.													
				  			
				  		point.v = (SInt16)line;
				  		
				  		ioOutBufferPtr = (HUCharPtr)gInputBufferPtr;
				  		ioOutBufferPtr += (firstColumn-columnStart);
				       		
				   	for (column=firstColumn; column<=columnEnd; column+=columnInterval)
							{	
							point.h = (SInt16)column;
						
							includePixelFlag = FALSE;
							if (pointType == kRectangleType)
								includePixelFlag = TRUE;
								
							else if (pointType == kPolygonType && PtInRgn (point, rgnHandle))
								includePixelFlag = TRUE;
							
							else if (pointType == kMaskType && 
																	*maskBufferPtr == maskRequestValue)
								includePixelFlag = TRUE;
								
							if (includePixelFlag)
				   			{
								finalClass = classToFinalClassPtr[*dataClassPtr];
								
								if (finalClass >= 0)
									*ioOutBufferPtr = (UInt8)(finalClass + 1);
									
								dataClassPtr++;
						         
						      }	// end "if (includePixelFlag)" 
						      
							ioOutBufferPtr += columnInterval;
										
							if (pointType == kMaskType)
								maskBufferPtr += columnInterval;
				         
				  			}	// end "for (column=firstColumn; ..." 
				  			
						if (pointType != kMaskType)
			  				{
					  				// Update firstColumn so that columnInterval samples are			
					  				// skipped from the end of one line to the beginning of the 	
					  				// next line.  This will allow the user to use samples from		
					  				// all columns if a column interval other than one is used.		
					  				
					  		firstColumn = columnStart + column - columnEnd - 1;
							if (firstColumn > columnEnd)
								{
								lDivideStruct = ldiv (firstColumn-columnStart, columnWidth);
								firstColumn = columnStart + lDivideStruct.rem;
								
								}	// end "if (firstColumn > columnEnd)"
								
							}	// end "if (pointType != kMaskType)"
								
								// Set pointer to correct location for writing.			
						
						if (errCode == noErr)	
							errCode = MSetMarker (maskFileStreamPtr, 
															fsFromStart, 
															writePosOff,
															kErrorMessages);
			
								// Write buffer to appropriate location in the 			
								// output disk file.						
						
						if (errCode == noErr)
							errCode = MWriteData (maskFileStreamPtr, 
															&count, 
															gInputBufferPtr,
															kErrorMessages);
															
						}	// end "if (!skipLineFlag)"
						
							// Ready counter for next position to write to file.	
							
					writePosOff += numberMaskColumns;
					
					if (pointType == kMaskType)
						fileIOInstructionsPtr->maskBufferPtr += 
												fileIOInstructionsPtr->numberMaskColumnsPerLine;
			  			
			  		}	// end "for (line=lineStart; line<=lineEnd; ...)" 
			  	
			  	if (continueFlag && firstLineCode != 1)	
					LoadDItemValue (gStatusDialogPtr, IDC_Status8, lineCount);
				
				CheckSomeEvents (mUpMask);
				
						// Close up any File IO Instructions structure that pertain to the 
						// specific area used for the list data.
						
				CloseUpFileIOInstructions (fileIOInstructionsPtr, &gAreaDescription);
								
						// Dispose of the region if it exists.						
		
				CloseUpAreaDescription (&gAreaDescription);
				
				if (!continueFlag)
																							return (FALSE);
		  			
		  		}	// end "for (areaNumber=1; areaNumber<=totolNumberAreas; ...)"
		  		
		  	}	// end "for (firstLineCode=firstLineCodeStart; ..."
		  	
		CloseUpGeneralFileIOInstructions (fileIOInstructionsPtr);
	
		}	// end "if (continueFlag)"
		
			// Create the class names for the mask file.

	if (continueFlag)
		continueFlag = CreateMaskFileClassNames (maskFileInfoPtr,
															gClusterSpecsPtr->numberFinalClusters);
															
			// Create the ERDAS Trailer or CLR file if needed.									
	
	if (continueFlag)
		{
		supportFileType = kITRLFileType;
		if (maskFileInfoPtr->format != kErdas74Type)
			supportFileType = kICLRFileType;
				
		continueFlag = CreateThematicSupportFile (
													maskFileInfoPtr,
													NULL,
													(UInt16)gClusterSpecsPtr->numberFinalClusters,
													NULL,
													0,
													NULL,
													NULL,
													NULL,
													kDefaultColors,
													(UInt16)gClusterSpecsPtr->numberFinalClusters,
													kClusterMaskCode,
													kPaletteHistogramClassNames,
													kClassDisplay,
													kCollapseClass,
													supportFileType);
									
		}	// end "if (continueFlag && ...)" 
		
	HideStatusDialogItemSet (kStatusClassA);
	HideStatusDialogItemSet (kStatusField);

	return (continueFlag);
		
}	// end "CreateClusterMaskFile"	 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CreateMaskFileClassNames
//
//	Software purpose:	The purpose of this routine is to create the cluster class names.
//
//	Parameters in:		ClusterType structure pointer
//
//	Parameters out:	None
//
// Value Returned:	True if everything worked okay
//							False if a problem came up such as shortage of memory
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/05/2000
//	Revised By:			Larry L. Biehl			Date: 05/16/2018

Boolean CreateMaskFileClassNames (
				FileInfoPtr							maskFileInfoPtr,
				SInt32								numberClasses)

{  
	ClusterType*       				cluster;
	
	UCharPtr								classNamePtr,
											namePtr;
											
	UInt16*								classSymbolPtr;
	
	SInt32								index;
	
	SInt16								classStorage,
											stringLength;
	

			// Get handle for mask file names.
			// Note that this is another place that the classDiscriptionH structure 
			// handling needs to be done differently. There ends up being 
			// numberClasses + 1 later on to handle the background class. It is handled 
			// differently later on.
			
	maskFileInfoPtr->classDescriptionH = MNewHandle (
											numberClasses * (sizeof (Str31) + sizeof (UInt16)));
								
	classNamePtr = (UCharPtr)GetHandlePointer (maskFileInfoPtr->classDescriptionH,
																kLock);							
								
	classSymbolPtr = (UInt16*)classNamePtr;
				
	if (classSymbolPtr != NULL)
		classSymbolPtr = &classSymbolPtr[numberClasses*sizeof (Str15)];
								
	if (classNamePtr != NULL)
		{					
		cluster = gClusterSpecsPtr->clusterHead;
					
		for (index=1; index<=numberClasses; index++)
			{
			stringLength = sprintf ((char*)&classNamePtr[1], "Cluster %ld", index);
		
					// Add project class name if needed.
			
			if (cluster != NULL &&
					gClusterSpecsPtr->projectClassMeansCode == 1 &&
															cluster->projectStatClassNumber >= 0)
				{
				classStorage = gProjectInfoPtr->storageClass[
																	cluster->projectStatClassNumber];
	
				namePtr = (UCharPtr)&gProjectInfoPtr->classNamesPtr[classStorage].name;
				
				stringLength = AddSpecifiedStringToClassName (classNamePtr,
																				stringLength,
																				TRUE,
																				namePtr);	
				
				}	// end "if (gClusterSpecsPtr->projectClassMeansCode == 1 && ..."
		
			if (cluster != NULL)
				cluster = cluster->next;
				
			classNamePtr[0] = (SInt8)stringLength;
			
			classNamePtr += 32;
			
			classSymbolPtr[index-1] = (UInt16)(index-1);
													
			}	// end "for (index=1; index<=numberClasses; index++)" 
			
		}	// end "if (classNamePtr != NULL)"
		
	return (classNamePtr != NULL);
	
}	//  end "CreateMaskFileClassNames"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt32 DeleteCluster
//
//	Software purpose:	The purpose of this routine is to delete the input specified
//							cluster.
//
//	Parameters in:		Number of pixels required for cluster to be kept.
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/09/2000
//	Revised By:			Larry L. Biehl			Date: 05/09/2000	

SInt32 DeleteCluster (
				SInt32								clusterCount,
				ClusterType*						clusterToDelete,
				ClusterType*						previousCluster)

{	
	ClusterType*						nextCluster;
	
	
	if (clusterToDelete != NULL)
		{
	   nextCluster = clusterToDelete->next;
	   		
		if (previousCluster != NULL)
			previousCluster->next = nextCluster;
			
		else	// previousCluster == NULL
  			gClusterSpecsPtr->clusterHead = nextCluster;
			
				// Delete the memory if it was generated in one pass cluster.	
				
		if (gClusterSpecsPtr->clusterMemoryHead == NULL)
			CheckAndDisposePtr ((char*)clusterToDelete);
			
		clusterCount--;
		
		}	// end "if (clusterToDelete != NULL)"
	
	return (clusterCount);
		
}	// end "DeleteCluster"	



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 DeleteSpecifiedClusters
//
//	Software purpose:	The purpose of this routine is to delete those clusters
//							which have fewer than the specified number of pixels.
//							This routine will work for clusters specified by single
//							pass cluster (individual blocks of memory) or those
//							specified by ISODATA (one block of memory).
//
//	Parameters in:		Number of pixels required for cluster to be kept.
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/24/1997
//	Revised By:			Larry L. Biehl			Date: 09/21/2000	

SInt16 DeleteSpecifiedClusters (
				SInt32								clusterCount,
				SInt32								numberPixelsRequired)

{
	ClusterType							*currentCluster,	// Cluster currently working on.	
											*nextCluster,
   										*previousCluster;
	

   		// Delete those clusters with too few pixels. 								
	
	//newClusterHead = NULL;
	previousCluster = NULL;	
   currentCluster = gClusterSpecsPtr->clusterHead;
   while (currentCluster != NULL)
   	{
   	nextCluster = currentCluster->next;
   	
   	if (currentCluster->numPixels < numberPixelsRequired)
   		{
   		clusterCount = DeleteCluster (clusterCount,
														currentCluster,
														previousCluster);
			/*				
   		clusterCount--;
   		
   		if (newClusterHead != NULL)
   			previousCluster->next = nextCluster;
   			
   				// Delete the memory if it was generated in one pass cluster.	
   				
   		if (gClusterSpecsPtr->clusterMemoryHead == NULL)
   			CheckAndDisposePtr ((char*)currentCluster);
			*/   		
   		}	// if (currentCluster->numPixels < minimumClusterSize) 
   		
   	else	// currentCluster->numPixels >= numberPixelsRequired 
   		{
   		//if (newClusterHead == NULL)
   		//	newClusterHead = currentCluster;
   			
   		previousCluster = currentCluster;
   		
   		}	// else currentCluster->numPixels >= ... 
   		
   	currentCluster = nextCluster;
      
   	}	// end "while (currentCluster != NULL)"
   	
	//gClusterSpecsPtr->clusterHead = newClusterHead;
   
   return ((SInt16)clusterCount);
		
}	// end "DeleteSpecifiedClusters"	



#if defined multispec_mac
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void DrawClusterDiskFilePopUp
//
//	Software purpose:	The purpose of this routine is to draw the 
//							disk file selection for Cluster Mask To Disk
//							pop up menu.  This routine will also draw a drop
//							shadow box around the pop up selection.
//
//	Parameters in:					
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/18/2013
//	Revised By:			Larry L. Biehl			Date: 04/18/2013	

pascal void DrawClusterDiskFilePopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.									
	
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpClusterMaskDiskFileMenu, 
								gOutputFormatCode, 
								(gOutputFormatCode > 0));
	
}	// end "DrawClusterDiskFilePopUp" 
#endif	// defined multispec_mac	


           
#if defined multispec_mac
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void DrawSaveStatsPopUp
//
//	Software purpose:	The purpose of this routine is to draw the save cluster
//							statistics prompt and selection for save statistics 
//							pop up menu.  This routine will also draw a drop
//							shadow box around the pop up selection.
//
//	Parameters in:		Pointer to a dialog structure
//							Item number where the save stats popup is to be drawn.
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/15/1991
//	Revised By:			Larry L. Biehl			Date: 03/15/1991	

pascal void DrawSaveStatsPopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.									
			
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpSaveStatsMenu, 
								gSaveStatisticsSelection, 
								TRUE);
	
}	// end "DrawSaveStatsPopUp"	
#endif	// defined multispec_mac  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void FindSmallestCluster
//
//	Software purpose:	The purpose of this routine is to find the cluster in the first
//							n number of clusters with the smallest number of pixels.
//
//	Parameters in:		Number of pixels required for cluster to be kept.
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/09/2000
//	Revised By:			Larry L. Biehl			Date: 05/09/2000

SInt32 FindSmallestCluster (
				SInt32								numberClustersToSearch,
				ClusterType**						minClusterPtr,
				ClusterType**						previousClusterPtr)

{
	ClusterType							*currentCluster,	// Cluster currently working on.	
											*previousCluster;
   								
	SInt32								clusterCount,
											minNumberPixels;
	

	clusterCount = 0;
	
   currentCluster = gClusterSpecsPtr->clusterHead;
   previousCluster = NULL;
   
   *minClusterPtr = currentCluster;
   *previousClusterPtr = previousCluster;
   minNumberPixels = currentCluster->numPixels;
   
   while (currentCluster != NULL && clusterCount < numberClustersToSearch)
   	{
   	if (currentCluster->numPixels < minNumberPixels)
   		{
   		minNumberPixels = currentCluster->numPixels;
   		*minClusterPtr = currentCluster;
   		*previousClusterPtr = previousCluster;
   		
   		}	// if (currentCluster->numPixels < minNumberPixels) 
   	
   	previousCluster = currentCluster;	
   	currentCluster = currentCluster->next;
      
   	}	// end "while (currentCluster != NULL && ...)"
   
   return (minNumberPixels);
		
}	// end "FindSmallestCluster"	



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		double GetAverageChannelStandardDev
//
//	Software purpose:	The purpose of this routine is to obtain the average standard
//							deviation from the hisogram information if it exists. If not
//							then a default value will be computed based on the number of 
//							bits of data.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/13/2003
//	Revised By:			Larry L. Biehl			Date: 06/09/2004	

double GetAverageChannelStandardDev (
				Handle								windowInfoHandle,
				FileInfoPtr							fileInfoPtr)

{
	double								aveageStandardDeviation;
	
	HistogramSpecsPtr					histogramSpecsPtr;
	HistogramSummaryPtr				histogramSummaryPtr;
	WindowInfoPtr						windowInfoPtr;
	
	Handle								histogramSpecsHandle,
											histogramSummaryHandle;
											
	UInt32								availableCount,
											index,
											numberChannels;
											
											
	aveageStandardDeviation = 0;
	
	windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);

	histogramSpecsHandle = GetHistogramSpecsHandle (windowInfoPtr);
	
	histogramSpecsPtr = (HistogramSpecsPtr)GetHandlePointer (histogramSpecsHandle);
																				
	if (histogramSpecsPtr != NULL)
		{
		numberChannels = histogramSpecsPtr->numberChannels;

		histogramSummaryHandle = GetHistogramSummaryHandle (windowInfoPtr);
	
		histogramSummaryPtr = (HistogramSummaryPtr)GetHandlePointer (
																					histogramSummaryHandle,
																					kLock);
		
		if (histogramSummaryPtr != NULL)
			{
			availableCount = 0;
			
			for (index=0; 
					index<numberChannels; 
						index++)
				{
					
				if (histogramSummaryPtr[index].availableFlag)
					{
					aveageStandardDeviation += histogramSummaryPtr[index].stdDeviation;
					
					availableCount++;
										
					}	// end "if (...histogram[channelNum].availableFlag)"
				
				}	// end "for (index=0; ... "
				
			if (availableCount > 0)
				aveageStandardDeviation /= availableCount;
			
			if (fileInfoPtr->numberBins <= 256)
				aveageStandardDeviation *= 2;
			
			CheckAndUnlockHandle (histogramSummaryHandle);
				
			}	// end "if (histogramSummaryPtr != NULL)" 
			
		}	// end "if (histogramSpecsPtr != NULL)"
		
	if (aveageStandardDeviation == 0)
		aveageStandardDeviation = 17 * fileInfoPtr->numberBins/256;
		
	return (aveageStandardDeviation);
	
}	//  end "GetAverageChannelStandardDev" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetClusterAreaStatistics
//
//	Software purpose:	The purpose of this routine is to determine the
//							cluster statistics to be stored into the project.  The
//							pixel assignments have already been made.  This routine
//							will use the cluster assignments and determine the
//							statistics based on the sum of squares.  The sum
//							statistics have already been computed.
//
//	Parameters in:		Pointer to image file information structure
//							Pointer to project information structure
//							The index within the class information structure
//									at which the cluster statistics are to start.
//							Pointer to the vector which contains the cluster class
//									for each clustered pixel.
//							Code indicating whether this is for the first line (=1)
//									or the rest of the lines (=2).
//
//	Parameters out:	None
//
// Value Returned:	True if everything worked okay
//							False if a problem came up such as shortage of memory
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/18/1991
//	Revised By:			Larry L. Biehl			Date: 08/30/2006

Boolean GetClusterAreaStatistics (
				FileIOInstructionsPtr			fileIOInstructionsPtr, 
				ProjectInfoPtr						projectClassInfoPtr, 
				SInt16								storageIndex, 
				HUInt16Ptr*							dataClassPtrPtr, 
				SInt16								firstLineCode)

{
			// Define local structures and variables.	
   
	double								dValue,
											minutesLeft;
											
	HChannelStatisticsPtr			areaChanPtr,
											localChanStatsPtr;
										
	HCovarianceStatisticsPtr		areaSumSquaresPtr,
											localSumSquaresStatsPtr;			
 	
	HDoublePtr							bufferPtr,
											bufferPtr2,
											outputBufferPtr,	// ptr to all pixels.
											tOutputBufferPtr; 
 	
 	HUInt16Ptr							dataClassPtr;	
 	
	Ptr									stringPtr;
	 
	SInt16								*classToFinalClassPtr;
	 
	UInt16								*channelsPtr;
	
	ldiv_t								lDivideStruct;
   Point									point;
	RgnHandle							rgnHandle;
	
  	SInt32	 							line,
											linesDone,
											lineCount,
											lineEnd,
											lineInterval,
											linesLeft,
			 								lineStart,
											numberColumns,
											numberSumSquares,
											startTick;
 	
	UInt32								channel,
							 				column,
							 				columnEnd,
											columnInterval,
											columnStart,
											columnWidth,
											covChan,
											covIndex,
											firstColumn,
											numberChannels,
											numberSamples,
											sample;
	
	SInt16								areaNumber,
											classFieldCode,
											errCode,
											fieldNumber,
											finalClass,
											lastClassIndex,
											lastFieldIndex,
 											pointType,
 											totalNumberAreas;					
   
   Boolean								continueFlag,
											polygonFieldFlag;
	
	
			// Initialize local variables. 													
   
   numberChannels = 			(UInt32)gProjectInfoPtr->numberStatisticsChannels;
	numberSumSquares = 		gProjectInfoPtr->numberCovarianceEntries;
	totalNumberAreas = 		gClusterSpecsPtr->totalNumberAreas;
	columnInterval = 			gClusterSpecsPtr->clusterColumnInterval;
	lineInterval = 			gClusterSpecsPtr->clusterLineInterval;
	channelsPtr = 				(UInt16*)gProjectInfoPtr->channelsPtr;
   outputBufferPtr = 		(HDoublePtr)gOutputBufferPtr;
   dataClassPtr =				*dataClassPtrPtr;
  	classToFinalClassPtr = 	gClusterSpecsPtr->clusterClassToFinalClassPtr;
  	continueFlag =				TRUE;
  	
  	classFieldCode = gProjectInfoPtr->keepClassStatsOnlyFlag + 1;
	GetProjectStatisticsPointers (classFieldCode, 
											storageIndex, 
											&localChanStatsPtr, 
											&localSumSquaresStatsPtr,
											NULL,
											NULL);
   
	fieldNumber = 0;
	lastClassIndex = -1;
	lastFieldIndex = -1;
					
			// Intialize the nextTime variable to indicate when the next check	
			// should occur for a command-.													
			
	gNextTime = TickCount ();
		
			// Loop by number of cluster areas.												
			
	for (areaNumber=1; areaNumber<=totalNumberAreas; areaNumber++)
		{
				// Initialize status variables.												
				
		lineCount = 0;
		linesDone = 0;
		gNextMinutesLeftTime = 0; 
		
				// Get information for next cluster area.									
				
		if (!GetNextClusterArea (projectClassInfoPtr, 
											channelsPtr, 
											(SInt16)numberChannels, 
											areaNumber, 
											&gNextMinutesLeftTime, 
											&lastClassIndex, 
											&lastFieldIndex, 
											&linesLeft))
																							return (FALSE);
			
		lineStart = gAreaDescription.lineStart;
		lineEnd = gAreaDescription.lineEnd;
  		columnStart = gAreaDescription.columnStart;
   	columnEnd = gAreaDescription.columnEnd;
		columnWidth = columnEnd - columnStart + 1;
		polygonFieldFlag = gAreaDescription.polygonFieldFlag;
		rgnHandle = gAreaDescription.rgnHandle;
		pointType =  gAreaDescription.pointType;	
			
		if (firstLineCode == 1)
			{
					// Initialization for first line of single pass cluster.			
					
			lineEnd = lineStart;
			columnInterval = 1;
			
			}	// end "if (firstLineCode == 1)" 
		
		if (firstLineCode == 2)
			{
					// Initialization for rest of lines for single pass cluster.	
					
			lineCount = 1;
			gAreaDescription.lineStart += lineInterval;
			lineStart = gAreaDescription.lineStart;
			linesLeft--;
			
					// Remember to update the mask buffer start so that it represents the
					// same area as that for the the image area.
			
			if (pointType == kMaskType)
				gAreaDescription.maskLineStart += lineInterval;
			
			}	// end "if (firstLineCode == 2)" 
			
				// Get first sample.																
	   	
		firstColumn = columnStart;
											
				// Check if first column will change from line to line.				
		
		numberColumns = (columnEnd - columnStart + columnInterval)/columnInterval;
			
				// Set up variables for controlling when status checks are made.														
							
		startTick = gNextStatusTime = TickCount ();
		
		if (gNextMinutesLeftTime != ULONG_MAX)
			gNextMinutesLeftTime = startTick + gNextStatusTimeOffset;	
			
				// Load some of the File IO Instructions structure that pertain
				// to the specific area being used.
	
		errCode = SetUpFileIOInstructions (fileIOInstructionsPtr,
														&gAreaDescription,
														(UInt16)numberChannels,
														channelsPtr,
														kDetermineSpecialBILFlag);
														
				// If lineEnd is 0, then this implies that there are no requested 
				// mask values in the area being used. Force looping through the 
				// lines to be skipped.
				
		if (lineEnd == 0)
			lineStart = 1;										
		
				// Loop by rest of lines for cluster area.								
		
		for (line=lineStart; line<=lineEnd; line+=lineInterval)
			{
					// If this in an image area, update dialog status information.	
		
			lineCount++;
			if (TickCount () >= gNextStatusTime)
				{
				LoadDItemValue (gStatusDialogPtr, IDC_Status8, (SInt32)lineCount);
				gNextStatusTime = TickCount () + gNextStatusTimeOffset;
				
				}	// end "if (TickCount () >= gNextStatusTime)" 
			
					// Exit routine if user has "command period" down.					
			
			if (TickCount () >= gNextTime)
				{
				continueFlag = CheckSomeEvents (
												osMask+keyDownMask+updateMask+mDownMask+mUpMask);
				if (!continueFlag)
					break;
					
				}	// end "if (TickCount () >= nextTime)" 
				
			column = firstColumn;
	  			
	  		point.v = (SInt16)line;
																					
					// Get all requested channels for first line of image data.  	
					// Return if there is a file IO error.									
					 
			errCode = GetLineOfData (fileIOInstructionsPtr,
												line,
												firstColumn,
												columnEnd,
												columnInterval,
												gInputBufferPtr,
												(HUCharPtr)outputBufferPtr);
										
			if (errCode < noErr)
				{ 	
				continueFlag = FALSE;
				break;
																								
				}	// end "if (errCode < noErr)"
			
			if (errCode != kSkipLine)
				{
		  		tOutputBufferPtr = (HDoublePtr)outputBufferPtr;
		     	
				if (pointType == kMaskType)
					numberSamples = fileIOInstructionsPtr->numberOutputBufferSamples;
					
				else	// pointType != kMaskType
					numberSamples = 
									(columnEnd - firstColumn + columnInterval)/columnInterval;
		       		
		   	for (sample=1; sample<=numberSamples; sample++)
					{
							// Set horizotal (column) point in case it is needed for			
							// polygonal field.															
							
				   point.h = (SInt16)column;
					
					if (!polygonFieldFlag || PtInRgn (point, rgnHandle))
		   			{
						finalClass = classToFinalClassPtr[*dataClassPtr];
						
						if (finalClass >= 0)
							{
							areaChanPtr = &localChanStatsPtr[finalClass*numberChannels];
							areaSumSquaresPtr = 
									&localSumSquaresStatsPtr[finalClass*numberSumSquares];
				   		bufferPtr = tOutputBufferPtr;
							covIndex = 0;
							
							for (channel=0; channel<numberChannels; channel++)
								{
								dValue = *bufferPtr;
													
										// Get the minimum and maximum value.					
										
								areaChanPtr[channel].minimum = 
														MIN (areaChanPtr[channel].minimum, dValue);
								areaChanPtr[channel].maximum = 
														MAX (areaChanPtr[channel].maximum, dValue);
								
										// Accumulate channel sums 								
								
								areaChanPtr[channel].sum += dValue;
								
								if (gProjectInfoPtr->statisticsCode	== kMeanCovariance)
									{
											// Accumulate the channel covariance statistics.
											
						      	bufferPtr2 = tOutputBufferPtr;
						      		
									for (covChan=0; covChan<channel; covChan++)
										{
										areaSumSquaresPtr[covIndex++] += dValue * *bufferPtr2;
						      		bufferPtr2++;
										
										}	// end "for (covChan=channel+1; ..." 
										
									}	// end "if (...->statisticsCode	== kMeanCovariance)" 
								
								areaSumSquaresPtr[covIndex++] += dValue * dValue;
								
					      	bufferPtr++;
									
								}	// end "for (channel=1; channel<..." 
								
							}	// end "if (finalClass >= 0)" 
							
						dataClassPtr++;
				         
				      }	// end "if (includePixelFlag)" 
				      
					tOutputBufferPtr += numberChannels;													
							
				   column += columnInterval;
		         
		  			}	// end "for (column=firstColumn; ..." 
		  			
				if (pointType != kMaskType)
	  				{
			  				// Update firstColumn so that columnInterval samples are			
			  				// skipped from the end of one line to the beginning of the 	
			  				// next line.  This will allow the user to use samples from		
			  				// all columns if a column interval other than one is used.		
			  				
			  		firstColumn = columnStart + column - columnEnd - 1;
					if (firstColumn > columnEnd)
						{
						lDivideStruct = ldiv (firstColumn-columnStart, columnWidth);
						firstColumn = columnStart + lDivideStruct.rem;
						
						}	// end "if (firstColumn > columnEnd)"
						
					}	// end "if (pointType != kMaskType)"
			      
				}	// end "if (errCode != kSkipLine)"
			
			linesDone++;
			linesLeft--;
			if (TickCount () >= gNextMinutesLeftTime)
				{
				minutesLeft = (linesLeft * (TickCount () - startTick))/
																(double)(lineCount*kTicksPerMinute);
				
				sprintf ((char*)gTextString, " %.1f", minutesLeft);
				stringPtr = (char*)CtoPstring (gTextString, gTextString);
				LoadDItemString (gStatusDialogPtr, IDC_Status14, (Str255*)gTextString);
					
				gNextMinutesLeftTime = TickCount () + gNextMinutesLeftTimeOffset;
				
				}	// end "if (TickCount () >= gNextMinutesLeftTime)" 
			
			if (pointType == kMaskType)
				fileIOInstructionsPtr->maskBufferPtr += 
												fileIOInstructionsPtr->numberMaskColumnsPerLine;
	  			
	  		}	// end "for (line=lineStart; line<=lineEnd; ...)" 
	  	
	  	if (continueFlag && firstLineCode != 1)
			{
			LoadDItemValue (gStatusDialogPtr, IDC_Status8, lineCount);
			CheckSomeEvents (updateMask);
			
			}	// end "if (continueFlag && firstLineCode != 1)"
		
				// Close up any File IO Instructions structure that pertain to the 
				// specific area used for the list data.
				
		CloseUpFileIOInstructions (fileIOInstructionsPtr, &gAreaDescription);
						
				// Dispose of the region if it exists.						

		CloseUpAreaDescription (&gAreaDescription);
		
		if (!continueFlag)
																							return (FALSE);
  			
  		}	// end "for (areaNumber=1; areaNumber<=totolNumberAreas; ...)" 
  		
  	*dataClassPtrPtr = dataClassPtr;
  	
  	return (TRUE);
  		
}	// end "GetClusterAreaStatistics" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetClusterProjectStatistics
//
//	Software purpose:	The purpose of this routine is to determine the
//							cluster statistics to be stored into the project.  The
//							pixel assignments have already been made.  This routine
//							will use the cluster assignments and determine the
//							statistics based on the sum of squares.  The sum
//							statistics have already been computed.
//
//	Parameters in:		Pointer to image file information structure
//							Pointer to project information structure
//							The number of cluster classes to be saved.
//							The index within the field information structure to
//									start storing the cluster statistics in.
//
//	Parameters out:	None
//
// Value Returned:	True if everything worked okay
//							False if a problem came up such as shortage of memory
// 
// Called By:			SaveClusterStatistics in SCluster.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/18/1991
//	Revised By:			Larry L. Biehl			Date: 05/18/2018

Boolean GetClusterProjectStatistics (
				FileIOInstructionsPtr			fileIOInstructionsPtr,  
				ProjectInfoPtr						projectClassInfoPtr, 
				SInt16								numberClasses, 
				SInt16								fieldStatStorage)				

{
			// Define local structures and variables.	
	
	HChannelStatisticsPtr			chanStatsPtr;
	HSumSquaresStatisticsPtr		sumSquaresStatsPtr;
   HUInt16Ptr							dataClassPtr;
   
   SInt32								lastStorageIndex,
   										startStorageIndex,
   										storageIndex,
   										numberChannels;
   										
  SInt16									classFieldCode;
   

			// Initialize local variables. 													
			
   dataClassPtr = gClusterSpecsPtr->dataClassPtr;
   numberChannels	= gProjectInfoPtr->numberStatisticsChannels;
   startStorageIndex = fieldStatStorage;
   lastStorageIndex = startStorageIndex + numberClasses;
		
			// Show status dialog items pertaining to clustering the rest of 		
			// the lines.																			
	
	ShowStatusDialogItemSet (kStatusField);
	ShowStatusDialogItemSet (kStatusCluster);

	if (gClusterSpecsPtr->clustersFrom == kTrainingType)
		{
		LoadDItemValue (gStatusDialogPtr, 
								IDC_Status5, 
								(SInt32)gClusterSpecsPtr->totalNumberAreas);
		ShowStatusDialogItemSet (kStatusClassB);
		HideStatusDialogItemSet (kStatusMinutes);
		
		}	// end "if (gClusterSpecsPtr->clustersFrom == kTrainingType)" 

	if (gClusterSpecsPtr->clustersFrom == kAreaType)
		{
		HideStatusDialogItemSet (kStatusClassB);
		ShowStatusDialogItemSet (kStatusMinutes);
		
		}	// end "if (gClusterSpecsPtr->clustersFrom == kAreaType)" 
		
	CheckSomeEvents (updateMask);
													
			// Initialize the memory for the class or field statistics. 		
	
  	classFieldCode = gProjectInfoPtr->keepClassStatsOnlyFlag + 1;
	for (storageIndex=startStorageIndex; 
			storageIndex<lastStorageIndex; 
				storageIndex++)
		{
		GetProjectStatisticsPointers (classFieldCode, 
												storageIndex, 
												&chanStatsPtr, 
												&sumSquaresStatsPtr,
												NULL,
												NULL);
									
		ZeroStatisticsMemory (chanStatsPtr, 
										sumSquaresStatsPtr, 
										(UInt16)numberChannels,
										gProjectInfoPtr->statisticsCode,
										kTriangleOutputMatrix);
										
		}	// end "for (storageIndex=startStorageIndex; ..." 
										
			// Make certain that we have a proper pointer to the project class	
			// information.																		
			
	if (projectClassInfoPtr == NULL)
		projectClassInfoPtr = gProjectInfoPtr;
	
			// Get the statistics for clusters found with single pass clustering.
	
	if (gClusterSpecsPtr->mode == kSinglePass)
		{
				// This is for the first lines.												
				
		if (!GetClusterAreaStatistics (fileIOInstructionsPtr, 
													projectClassInfoPtr, 
													fieldStatStorage, 
													&dataClassPtr, 
													1))
																							return (FALSE);
				// This is for the rest of the lines.										
				
		if (!GetClusterAreaStatistics (fileIOInstructionsPtr, 
													projectClassInfoPtr, 
													fieldStatStorage, 
													&dataClassPtr, 
													2))
																							return (FALSE);
																					
		}	// end "if (gClusterSpecsPtr->mode == kSinglePass)" 
	
			// Get the statistics for clusters found with ISODATA clustering.		
	
	if (gClusterSpecsPtr->mode == kISODATA)
		{
		if (!GetClusterAreaStatistics (fileIOInstructionsPtr, 
													projectClassInfoPtr, 
													fieldStatStorage, 
													&dataClassPtr, 
													0))
																							return (FALSE);
																					
		}	// end "if (gClusterSpecsPtr->mode == kISODATA)" 
  	
  	return (TRUE);
  		
}	// end "GetClusterProjectStatistics" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetMeanStdDevLength
//
//	Software purpose:	The purpose of this routine is to determine the
//							number of digits of the largest mean and standard 
//							deviation for the input clusters.
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:	Maximumn number of digits needed
// 
// Called By:			ListClusterStatistics in SCluster.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/14/2005
//	Revised By:			Larry L. Biehl			Date: 02/24/2012	

SInt16 GetMeanStdDevLength (
				double								maxDataValue, 
				double								minDataValue,
				SInt16								numberFDecimalDigits,
				SInt16								numberEDecimalDigits)

{
	SInt32								numChars;
	
	SInt16								maximumLength;
	
	
	maximumLength = 3;
																						
	numChars = LoadRealValueString ((char*)gTextString3,
													maxDataValue,
													0,
													numberFDecimalDigits,
													numberEDecimalDigits,
													(char*)"",
													(char*)"");			
			
	maximumLength = (SInt16)MAX (numChars, maximumLength);
			
	numChars = LoadRealValueString ((char*)gTextString3,
													minDataValue,
													0,
													numberFDecimalDigits,
													numberEDecimalDigits,
													(char*)"",
													(char*)"");
				
	maximumLength = (SInt16)MAX (numChars, maximumLength);
		
	return (maximumLength);

}	// end "GetMeanStdDevLength" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetNextClusterArea
//
//	Software purpose:	The purpose of this routine is to obtain the information
//							for the next cluster area.
//
//	Parameters in:					
//
//	Parameters out:	None
//
// Value Returned:	True if everything worked okay
//							False if a problem came up such as shortage of memory
// 
// Called By:			GetClusterAreaStatistics in cluster.c
//							OnePassClusterAreas in clusterSinglePass.c
//							ISODATAClusterPass in clusterISODATA.c
//
//	Coded By:			Larry L. Biehl			Date: 03/18/1991
//	Revised By:			Larry L. Biehl			Date: 01/12/1999	

Boolean GetNextClusterArea (
				ProjectInfoPtr						projectClassInfoPtr, 
				UInt16*								channelsPtr, 
				SInt16								numberChannels, 
				SInt16								areaNumber, 
				UInt32*								nextUpdatePtr, 
				SInt16*								lastClassIndexPtr, 
				SInt16*								lastFieldIndexPtr, 
				SInt32*								linesLeftPtr)

{
	SInt16*							clusterClassPtr;
	
	SInt16							fieldNumber;
	
		
			// Get cluster area.																	
			
	if (gClusterSpecsPtr->clustersFrom == kTrainingType)
		{
				// Do not calculate the time left for clustering of training		
				// fields.																			
		
		if (nextUpdatePtr != NULL)
			*nextUpdatePtr = ULONG_MAX;
		
				// Put number of cluster areas being done in the status box.		
				
		LoadDItemValue (gStatusDialogPtr, IDC_Status3, (SInt32)areaNumber);
		
		clusterClassPtr = (SInt16*)GetHandlePointer (
															gClusterSpecsPtr->clusterClassHandle);
		
				// Clusters to come from training areas.									
				
		fieldNumber = GetNextFieldArea (projectClassInfoPtr,
													clusterClassPtr, 
													gClusterSpecsPtr->numberClusterClasses, 
													lastClassIndexPtr, 
													*lastFieldIndexPtr, 
													kTrainingType,
													kDontIncludeClusterFields);
				
		if (fieldNumber < 0)
																							return (FALSE);
																				
		*lastFieldIndexPtr = fieldNumber;
		
				// Get the field coordinates.
				
		gAreaDescription.lineInterval = gClusterSpecsPtr->clusterLineInterval;
		gAreaDescription.columnInterval = gClusterSpecsPtr->clusterColumnInterval;												
					
		GetFieldBoundary (projectClassInfoPtr, &gAreaDescription, fieldNumber);
		
		if (gAreaDescription.polygonFieldFlag && gAreaDescription.rgnHandle == NULL)
																							return (FALSE);
		
		}	// end "if (gClusterSpecsPtr->clustersFrom == ...)" 
		
	if (gClusterSpecsPtr->clustersFrom == kAreaType)
		{
				// Clusters to come from selected area.
				
		InitializeAreaDescription (&gAreaDescription, 
												gClusterSpecsPtr->clusterLineStart, 
												gClusterSpecsPtr->clusterLineEnd, 
												gClusterSpecsPtr->clusterColumnStart, 
												gClusterSpecsPtr->clusterColumnEnd, 
												gClusterSpecsPtr->clusterLineInterval, 
												gClusterSpecsPtr->clusterColumnInterval,
												1,
												1,
												0);	
												
		LoadDItemStringNumber (kDialogStrID, 
										IDS_Dialog18,		// Blank string
										gStatusDialogPtr, 
										IDC_Status14, 
										(Str255*)gTextString);	
		
		}	// end "if (gClusterSpecsPtr->clustersFrom == ...)" 
	
			// Load some status dialog information.										
			// Get lines left to do.															
	
	if (linesLeftPtr != NULL)
		{
		*linesLeftPtr = gAreaDescription.numberLines;
		LoadDItemValue (gStatusDialogPtr, IDC_Status10, (SInt32)*linesLeftPtr);
		
		}	// end "if (linesLeftPtr != NULL)" 
	/*			
			// Set flag to indicate whether entire line of data for all 			
			// channels can be read in at one time, rather than separate 			
			// reads for each channel.
			// Decided to remove this feature															
	
	if (BILSpecialFlagPtr != NULL)	
		*BILSpecialFlagPtr = GetBILSpecial (channelsPtr, 
														numberChannels, 
														gAreaDescription.columnStart,
														gAreaDescription.columnEnd,
														1);
	*/								
	return (TRUE);
										
}	// end "GetNextClusterArea" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetMinimumAndMaximumValueForListing
//
//	Software purpose:	This routine determines the minimum and maximum data value
//							to be listed in the cluster statistics. This is used to make
//							a reasonable listing in the output text window and in any output
//							text disk file.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/24/2012
//	Revised By:			Larry L. Biehl			Date: 02/24/2012

void GetMinimumAndMaximumValueForListing (
				ClusterType*						clusterHead,
				SInt16								numberChannels,
				Boolean								stdDevFlag,
				double*								maxDataValuePtr,
				double*								minDataValuePtr)

{
	double								maxValue = -DBL_MAX,
											minValue = DBL_MAX;
											
	ClusterType*						currentCluster;
	
	SInt16								channel;

	
			// Initialize output variables.
			
			// Now list the cluster means for each of the clusters.
			
   currentCluster = clusterHead;													
   while (currentCluster != NULL)
   	{
      for (channel=0; channel<numberChannels; channel++)
      	{
					// Get the minimum and maximum value.
			
			minValue = MIN (minValue, currentCluster->meanPtr[channel]);
			maxValue = MAX (maxValue, currentCluster->meanPtr[channel]);
			
			if (stdDevFlag)
				{
				minValue = MIN (minValue, currentCluster->variancePtr[channel]);
				maxValue = MAX (maxValue, currentCluster->variancePtr[channel]);
				
				}	// end "if (stdDevFlag)"
							
			}	// end "for (channel=0; channel<numberChannels; channel++)" 
		
      currentCluster = currentCluster->next;
      
   	}	// end "while (currentCluster != NULL)" 
			
	*maxDataValuePtr = maxValue;
	*minDataValuePtr = minValue;
	
}	// end "GetMinimumAndMaximumValueForListing" 

	
			
#if defined multispec_mac
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetProbabilityFile
//
//	Software purpose:	This routine allows the user to select the Probability 
//							file to be used.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/02/1994
//	Revised By:			Larry L. Biehl			Date: 03/02/1994

Boolean GetProbabilityFile (
				FileInfoPtr							fileInfoPtr,
				Handle*								probabilityFileInfoHPtr)

{
	char									*string1Ptr,
											*string2Ptr;
	
	CMFileStream*						probabilityFileStreamPtr;
	FileInfoPtr							probabilityFileInfoPtr;
	
	UInt32								count;

	OSType								fileType;
	
	SInt16								errCode,
											stringCompare;
	
	Boolean								continueFlag;
	
	
	continueFlag = TRUE;
	string1Ptr = (char*)gTextString;
	string2Ptr = (char*)gTextString2;
	
			// Get a handle to a block of memory to be used for						
			// file information for the probability file.								
			// Lock the memory until the information has been loaded					
			// in. Then get the pointer to the file information block				
	
	if (fileInfoPtr == NULL)									
																							return (FALSE);
	
	if (*probabilityFileInfoHPtr == NULL)
		{
		*probabilityFileInfoHPtr = MNewHandle (sizeof (MFileInfo));
		
				// Initialize the variables and handles in the structure.			
		
		if (*probabilityFileInfoHPtr != NULL)
			InitializeFileInfoStructure (*probabilityFileInfoHPtr, kNotPointer);
			
		}	// end "if (*probabilityFileInfoHPtr == NULL)" 
					
	if (*probabilityFileInfoHPtr != NULL)
		{
		probabilityFileInfoPtr = (FileInfoPtr)GetHandlePointer (
																	*probabilityFileInfoHPtr, kLock);
		
		probabilityFileInfoPtr->format = 0;
		probabilityFileInfoPtr->ancillaryInfoformat = 0;

		probabilityFileStreamPtr = GetFileStreamPointer (probabilityFileInfoPtr);

				// Get the file name and volume to read probability file 			
				// from.	First make sure that we have a pointer cursor.				

		MInitCursor ();
		fileType = 'TEXT';
		stringCompare = -1;
		
		do
			{
					// Indicate that the image type is to be determined from the 	
					// header part of the file.												
				
			gGetFileImageType = 0;
			
			errCode = GetFile (probabilityFileStreamPtr, 
										1, 
										&fileType, 
										NULL, 
										NULL,
										NULL, 
										IDS_FileIO65);
			continueFlag = (errCode == noErr) & 
													(probabilityFileStreamPtr->fileName[0] != 0);
		
					// If file is set up to read probability information from, 		
					// check if file is of correct format.									
				
			if (continueFlag)
				{
				count = 128;
				errCode = MReadData (probabilityFileStreamPtr, 
											&count, 
											gTextString,
											kErrorMessages);
				continueFlag = (errCode == noErr);
					
				if (continueFlag)
					continueFlag = ReadERDASHeader (
										probabilityFileInfoPtr, 
										(char*)gTextString, 
										0);
					
				if (continueFlag)
					{
							// Make certain that the file has correct parameters.		
					
					if (probabilityFileInfoPtr->thematicType == FALSE)
						stringCompare = -1;
						
					if ((probabilityFileInfoPtr->numberClasses != 
														gNumberProbabilityClasses+1) &&
											(probabilityFileInfoPtr->numberClasses != 110))
						stringCompare = -1;
					
					}	// end "if (continueFlag)" 
							
						// Beep to alert user that file is not an image 				
						// probability file that matches the image parameters.		
					
				if (stringCompare != 0)  
					SysBeep (10);
				
				}	// end "if (continueFlag)" 
			
			}	while (continueFlag && (stringCompare != 0));
			
		CheckAndUnlockHandle (*probabilityFileInfoHPtr);

		}	// end "if (*probabilityFileInfoHPtr != NULL)" 

	else		
		continueFlag = FALSE;
  	
	return (continueFlag);
	
}	// end "GetProbabilityFile" 		
#endif	// defined multispec_mac



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				Boolean InitializeClusterMemory
//
//	Software purpose:	The purpose of this routine is to obtain the memory
//							needed for several vectors needed for clustering.
//							These include the input and output buffers and the vector
//							used to store the class assignments.  The routine
//							will also determine the number of cluster areas
//							and the number of cluster pixels.
//
//	Parameters in:		Pointer to image file information structure
//
//	Parameters out:	None
//
// Value Returned:	True if everything worked okay
//							False if a problem came up such as shortage of memory
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 03/18/1991
//	Revised By:			Larry L. Biehl			Date: 01/11/2003	

Boolean InitializeClusterMemory (
				FileIOInstructionsPtr*			fileIOInstructionsPtrPtr)

	{
	SInt16								*clusterClassPtr;
											
	UInt16								*channelsPtr;
	
	SInt16								numberChannels;
	
	Boolean								continueFlag;
	
	
			// Initialize local variables.													
	
	continueFlag = TRUE;
	numberChannels = gClusterSpecsPtr->numberChannels;
	if (gClusterSpecsPtr->saveStatisticsCode > 0)
		numberChannels = 
						MAX (numberChannels, gProjectInfoPtr->numberStatisticsChannels);
					
	clusterClassPtr = (SInt16*)GetHandlePointer (gClusterSpecsPtr->clusterClassHandle);
					
	channelsPtr = (UInt16*)GetHandlePointer (gClusterSpecsPtr->channelsHandle);
																						
			// Get total number of cluster areas.  First, assume that the			
			// cluster area is the selection area, I.E. not training areas.		
			 
	gClusterSpecsPtr->totalNumberAreas = 1;
	if (gClusterSpecsPtr->clustersFrom == kTrainingType)
		gClusterSpecsPtr->totalNumberAreas = GetNumberOfAreas (
															clusterClassPtr, 
															gClusterSpecsPtr->numberClusterClasses, 
															kTrainingType,
															kDontIncludeClusterFields);
											
	if (gClusterSpecsPtr->totalNumberAreas <= 0)
		continueFlag = FALSE;
														
   		// Allocate work space for input image data.									
			// Get buffer to read data from image file into.							
						
   if (continueFlag)						
		continueFlag = GetIOBufferPointers (&gFileIOInstructions[0],
														gImageWindowInfoPtr,
														gImageLayerInfoPtr,
														gImageFileInfoPtr,
														&gInputBufferPtr, 
														&gOutputBufferPtr,
														1, 
														gImageWindowInfoPtr->maxNumberColumns, 
														1,
														numberChannels,
														channelsPtr, 
														kPackData, 
														kForceBISFormat, 
														kForceReal8Bytes,
														kDoNotAllowForThreadedIO,
														fileIOInstructionsPtrPtr);
	
			// Get the number of cluster pixels.											
	
	if (continueFlag && 
		(gClusterSpecsPtr->saveStatisticsCode > 0 || 
				gClusterSpecsPtr->mode == kISODATA ||
							gOutputCode & kClusterMaskCode ||
										gOutputCode & kCreateImageOverlayCode))	
		{
		gClusterSpecsPtr->totalNumberClusterPixels = 
							GetTotalNumberOfPixels (gClusterSpecsPtr->clustersFrom,
															clusterClassPtr, 
															gClusterSpecsPtr->numberClusterClasses,
															&gClusterSpecsPtr->clusterLineStart,
															gClusterSpecsPtr->mode*2 + 1);
										
			// Check if number of cluster pixels make sense.							
			
		continueFlag = (gClusterSpecsPtr->totalNumberClusterPixels > 0);
										
			// Get the memory to store the pixel cluster class assignments in.	
			
		if (continueFlag)	
			{
			gClusterSpecsPtr->dataClassPtr = (HUInt16Ptr)MNewPointerClear (
							gClusterSpecsPtr->totalNumberClusterPixels * sizeof (SInt16));
			continueFlag = (gClusterSpecsPtr->dataClassPtr != NULL);
			
			}	// end "if (continueFlag)"
			
		}	// end "if (continueFlag && ..."
		
			// Get memory to store the cluster class to final class vector in.	
			
	if (continueFlag && (gClusterSpecsPtr->saveStatisticsCode > 0 || 
									gClusterSpecsPtr->classificationArea != 0 ||
									gClusterSpecsPtr->outputStorageType & kClusterMaskCode))	
		{
		gClusterSpecsPtr->clusterClassToFinalClassPtr = 
								(SInt16*)MNewPointerClear ((SInt64)512 * sizeof (SInt16));
		continueFlag = (gClusterSpecsPtr->clusterClassToFinalClassPtr != NULL);
			
		}	// end "if (continueFlag && ...)" 
		
	return (continueFlag);

}	// end "InitializeClusterMemory" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void KeepLargestClusters
//
//	Software purpose:	The purpose of this routine is to find the cluster in the first
//							n number of clusters with the smallest number of pixels.
//
//	Parameters in:		Number of pixels required for cluster to be kept.
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/09/2000
//	Revised By:			Larry L. Biehl			Date: 09/21/2000

void KeepLargestClusters (
				SInt32								numberClustersToKeep)

{
	ClusterType							*minCluster,
											*previousCluster;
											
	SInt32								clusterCount,
											minNumberPixels;


	clusterCount = gClusterSpecsPtr->numberFinalClusters;
	
	while (clusterCount > numberClustersToKeep)
		{
	  			// Find number of pixels in the smallest cluster class within the first
	  			// numberClusterClassesToKeep. 
	   
	   minNumberPixels = FindSmallestCluster (numberClustersToKeep, 
															&minCluster,
															&previousCluster);
   
	   		// Now delete all cluster classes with fewer pixels than this. 
	   
	   clusterCount = DeleteSpecifiedClusters (clusterCount,
	   														minNumberPixels);
	   														
	   if (previousCluster == NULL)
	   	previousCluster = NULL;
	   
	   if (clusterCount > numberClustersToKeep)
	   	clusterCount = DeleteCluster (clusterCount,
													minCluster,
													previousCluster);
	   
	   }	// end "while (clusterCount > numberClustersToKeep)"
	   
	gClusterSpecsPtr->numberFinalClusters = (SInt16)clusterCount;
		
}	// end "KeepLargestClusters"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListClusterInputParameters
//
//	Software purpose:	The purpose of this routine is to list the input parameters for
//							the cluster processor.
//
//	Parameters in:			
//
//	Parameters out:	None
//
// Value Returned:	
//
// Called By:			ClusterControl in SCluster.cpp
//							
//
//	Coded By:			Larry L. Biehl			Date: 02/13/2000
//	Revised By:			Larry L. Biehl			Date: 02/15/2000	

Boolean ListClusterInputParameters (
				FileInfoPtr							fileInfoPtr,
				CMFileStream*						resultsFileStreamPtr)
				
{
	SInt16								*channelsPtr = NULL,
											*clusterClassAreaPtr = NULL;
											
	UInt32								index;
	
	Boolean								continueFlag = TRUE;
	
				
			// List the classification header information.				
			
	continueFlag = ListClassificationHeaderInfo (resultsFileStreamPtr, 
																&gOutputForce1Code,
																kNoStatisticsUsed,
																continueFlag);
									
	if (gClusterSpecsPtr->mode == kSinglePass)
		{
				//	"Cluster algorithm is "one pass"
		continueFlag = ListSpecifiedStringNumber (kClusterStrID, 
																IDS_Cluster36, 
																gTextString, 
																resultsFileStreamPtr, 
																gOutputForce1Code, 
																continueFlag);											
		
				// "Critical distance 1:  %ld\r"													
		continueFlag = ListSpecifiedStringNumber (kClusterStrID, 
																IDS_Cluster24,
																resultsFileStreamPtr, 
																gOutputForce1Code, 
																gClusterSpecsPtr->criticalDistance1, 
																continueFlag);
		
				// "Critical distance 2:  %ld\r"													
		continueFlag = ListSpecifiedStringNumber (kClusterStrID, 
																IDS_Cluster25,
																resultsFileStreamPtr, 
																gOutputForce1Code, 
																gClusterSpecsPtr->criticalDistance2, 
																continueFlag);
			
		continueFlag = ListSpecifiedStringNumber (
															kClusterStrID, 
															IDS_Cluster12,
															resultsFileStreamPtr, 
															gOutputForce1Code, 
															(SInt32)gClusterSpecsPtr->minClusterSize, 
															continueFlag);
											
		}	// end "if (gClusterSpecsPtr->mode == kSinglePass)"
			
	else if (gClusterSpecsPtr->mode == kISODATA)
		{
				//	Cluster algorithm is "ISODATA"
				
		continueFlag = ListSpecifiedStringNumber (kClusterStrID, 
																IDS_Cluster38, 
																gTextString, 
																resultsFileStreamPtr, 
																gOutputForce1Code, 
																continueFlag);														
			
				// " Initialize ...					
				
		if (gClusterSpecsPtr->initializationOption == 1)
			{
			index = IDS_Cluster6;
			if (gClusterSpecsPtr->useCorrelationMatrixFlag)
				index = IDS_Cluster7;
			
			}	// end "if (gClusterSpecsPtr->initializationOption == 1)"
			
		else if (gClusterSpecsPtr->initializationOption == 2)	
			{
			index = IDS_Cluster8;
			if (gClusterSpecsPtr->useCorrelationMatrixFlag)
				index = IDS_Cluster43;
			
			}	// end "if (gClusterSpecsPtr->initializationOption == 2)"
			
		else 		// gClusterSpecsPtr->initializationOption == 3
			index = IDS_Cluster9;	
							
		continueFlag = ListSpecifiedStringNumber (kClusterStrID, 
																(SInt16)index, 
																(unsigned char*)gTextString, 
																resultsFileStreamPtr, 
																gOutputForce1Code,
																continueFlag);
											
		if (gClusterSpecsPtr->projectClassMeansCode == 1)
			continueFlag = ListSpecifiedStringNumber (kClusterStrID, 
																	IDS_Cluster44, 
																	(unsigned char*)gTextString, 
																	resultsFileStreamPtr, 
																	gOutputForce1Code,
																	continueFlag);
		
		if (gClusterSpecsPtr->initializationOption == 3)
			{
					// "Critical distance 1:  %ld\r"												
			continueFlag = ListSpecifiedStringNumber (
																kClusterStrID, 
																IDS_Cluster24,
																resultsFileStreamPtr, 
																gOutputForce1Code, 
																gClusterSpecsPtr->criticalDistance1, 
																continueFlag);
			
					// "Critical distance 2:  %ld\r"												
			continueFlag = ListSpecifiedStringNumber (
																kClusterStrID, 
																IDS_Cluster25,
																resultsFileStreamPtr, 
																gOutputForce1Code, 
																gClusterSpecsPtr->criticalDistance2, 
																continueFlag);
	
			}	// end "if (gClusterSpecsPtr->initializationOption == 3)" 
		
		else	// gClusterSpecsPtr->initializationOption != 3 
					// "Number Clusters:  %ld\r"
			continueFlag = ListSpecifiedStringNumber (
														kClusterStrID, 
														IDS_Cluster10,
														resultsFileStreamPtr, 
														gOutputForce1Code, 
														(SInt32)gClusterSpecsPtr->maxNumberClusters, 
														continueFlag);
																							
				// "Convergence (percent):  %5.1f\r"											
		continueFlag = ListSpecifiedStringNumber (kClusterStrID, 
																IDS_Cluster11,
																resultsFileStreamPtr, 
																gOutputForce1Code, 
																gClusterSpecsPtr->convergence, 
																continueFlag);
																							
				// "Minimum cluster size:  %ld\r"												
		continueFlag = ListSpecifiedStringNumber (
															kClusterStrID, 
															IDS_Cluster12,
															resultsFileStreamPtr, 
															gOutputForce1Code, 
															(SInt32)gClusterSpecsPtr->minClusterSize, 
															continueFlag);
			
		}	// end "else if (gClusterSpecsPtr->mode == kISODATA)"
														
			// List the cluster specifications.							
	
	if (gClusterSpecsPtr->clustersFrom == kTrainingType)
		{ 										
				//	"Clusters from training area(s).%s"
		continueFlag = ListSpecifiedStringNumber (kClusterStrID, 
																IDS_Cluster34, 
																gTextString, 
																resultsFileStreamPtr, 
																gOutputForce1Code, 
																continueFlag);

				// List the training fields/areas used.									
					
		clusterClassAreaPtr = (SInt16*)GetHandlePointer (
																gClusterSpecsPtr->clusterClassHandle);	
					
		gAreaDescription.lineInterval = gClusterSpecsPtr->clusterLineInterval; 
		gAreaDescription.columnInterval = gClusterSpecsPtr->clusterColumnInterval;
	
		if (continueFlag)							
			continueFlag = ListProjectFieldsUsed (fileInfoPtr, 
																clusterClassAreaPtr, 
																gClusterSpecsPtr->numberClusterClasses, 
																NULL, 
																kTrainingType, 
																resultsFileStreamPtr, 
																&gOutputForce1Code,
																FALSE);
												
		}	// end "if (gClusterSpecsPtr->clustersFrom == ...)" 
					
	else	// gClusterSpecsPtr->clustersFrom == kAreaType 
		{                           	
				//	"Clusters from selected area.%s"
		continueFlag = ListSpecifiedStringNumber (kClusterStrID, 
																IDS_Cluster41, 
																gTextString, 
																resultsFileStreamPtr, 
																gOutputForce1Code, 
																continueFlag);
												
				//	sprintf ((char*)gTextString, " Lines:   %4ld to %4ld by %ld%s"
		continueFlag = ListSpecifiedStringNumber (kSharedStrID, 
																IDS_Shared1,
																resultsFileStreamPtr, 
																gOutputForce1Code,
																gClusterSpecsPtr->clusterLineStart,
																gClusterSpecsPtr->clusterLineEnd,
																gClusterSpecsPtr->clusterLineInterval, 
																continueFlag);
												
				//	sprintf ((char*)gTextString, " Columns: %4ld to %4ld by %ld%s"
		continueFlag = ListSpecifiedStringNumber (
															kSharedStrID, 
															IDS_Shared2,
															resultsFileStreamPtr, 
															gOutputForce1Code,
															gClusterSpecsPtr->clusterColumnStart,
															gClusterSpecsPtr->clusterColumnEnd,
															gClusterSpecsPtr->clusterColumnInterval, 
															continueFlag);
		
				// Insert a blank line
			
		continueFlag = OutputString (resultsFileStreamPtr, 
												(char*)gEndOfLine, 
												0, 
												gOutputForce1Code,
												continueFlag); 	
												
		}	// end "else gClusterSpecsPtr->clustersFrom == ..." 
		
	channelsPtr = (SInt16*)GetHandlePointer (gClusterSpecsPtr->channelsHandle);

			// Print a list of the channels used.							
	
	if (continueFlag)
		continueFlag = ListChannelsUsed (fileInfoPtr,
													NULL,
													channelsPtr,
													gClusterSpecsPtr->numberChannels, 
													resultsFileStreamPtr, 
													&gOutputForce1Code,
													kNoTransformation);	
					
			// Save Project statistics.											
			
	if (gClusterSpecsPtr->saveStatisticsCode > 0)
				//	"The cluster statistics will be saved to the project file."
		continueFlag = ListSpecifiedStringNumber (kClusterStrID, 
																IDS_Cluster30, 
																gTextString, 
																resultsFileStreamPtr, 
																gOutputForce1Code, 
																continueFlag);
												
	if (gClusterSpecsPtr->outputStorageType & kClusterMaskCode)
				//	"A cluster source statistics mask file will be created"
		continueFlag = ListSpecifiedStringNumber (kClusterStrID, 
																IDS_Cluster35, 
																gTextString, 
																resultsFileStreamPtr, 
																gOutputForce1Code, 
																continueFlag);
												
			// List Cluster Classification request.
	
	if (gClusterSpecsPtr->classificationArea == 0)
				//	"No cluster classification.", 
		continueFlag = ListSpecifiedStringNumber (kClusterStrID, 
																IDS_Cluster31, 
																gTextString, 
																resultsFileStreamPtr, 
																gOutputForce1Code, 
																continueFlag);
	
	else if (gClusterSpecsPtr->classificationArea == 1)
				// "Cluster classify training area(s).", 
		continueFlag = ListSpecifiedStringNumber (kClusterStrID, 
																IDS_Cluster32, 
																gTextString, 
																resultsFileStreamPtr, 
																gOutputForce1Code, 
																continueFlag);
					
	else if (gClusterSpecsPtr->classificationArea == 2)
		{
				//	"Classify selected area:"
		continueFlag = ListSpecifiedStringNumber (kClusterStrID, 
																IDS_Cluster33, 
																gTextString, 
																resultsFileStreamPtr, 
																gOutputForce1Code, 
																continueFlag);
												
				//	sprintf ((char*)gTextString, "  Lines:   %4ld to %4ld by %ld%s"
		continueFlag = ListSpecifiedStringNumber (kSharedStrID, 
																IDS_Shared1,
																resultsFileStreamPtr, 
																gOutputForce1Code,
																gClusterSpecsPtr->imageLineStart,
																gClusterSpecsPtr->imageLineEnd,
																gClusterSpecsPtr->imageLineInterval, 
																continueFlag);
												
				//	sprintf ((char*)gTextString, "  Columns: %4ld to %4ld by %ld%s"
		continueFlag = ListSpecifiedStringNumber (kSharedStrID, 
																IDS_Shared2,
																resultsFileStreamPtr, 
																gOutputForce1Code,
																gClusterSpecsPtr->imageColumnStart,
																gClusterSpecsPtr->imageColumnEnd,
																gClusterSpecsPtr->imageColumnInterval, 
																continueFlag);
												
		}	// end "else if gClusterSpecsPtr->..." 
		
	if (gClusterSpecsPtr->classificationArea > 0) 
				// List the classify threshold value.	
		continueFlag = ListSpecifiedStringNumber (kClusterStrID, 
																IDS_Cluster52,
																resultsFileStreamPtr, 
																gOutputForce1Code, 
																gClusterSpecsPtr->classifyThreshold, 
																continueFlag);
		
			// Insert a blank line
		
	continueFlag = OutputString (resultsFileStreamPtr, 
											(char*)gEndOfLine, 
											0, 
											gOutputForce1Code,
											continueFlag); 
										
			// List "  Output Information:"
			
	continueFlag = ListSpecifiedStringNumber (kSharedStrID, 
															IDS_Shared8, 
															(unsigned char*)gTextString, 
															resultsFileStreamPtr, 
															gOutputForce1Code, 
															continueFlag);
																
	return (continueFlag);
										
}	// end "ListClusterInputParameters"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListClusterStatistics
//
//	Software purpose:	The purpose of this routine is to list the cluster
//							channel means and the number of points in each 
//							cluster.  If the standard deviation flag is TRUE, 
//							table of the cluster standard deviations will also
//							be printed.
//
//	Parameters in:		Pointer to output results file information structure.
//							Code indicating where the cluster statistics are to
//									be listed - text window and/or disk file.
//							Flag indicating whether the standard deviation is
//									to be listed.
//
//	Parameters out:	None
//
// Value Returned:	True if everything worked okay
//							False if a problem came up such as shortage of memory
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/08/1990
//	Revised By:			Larry L. Biehl			Date: 03/12/2018

Boolean ListClusterStatistics (
				CMFileStream*						clResultsFileStreamPtr, 
				SInt16*								outputCodePtr, 
				Boolean								stdDevFlag)
				
{
	Str255								pixelCountString;

	char									titleSpaceString[32];
	
	double								areaConversionFactor,
											maxDataValue,
											minDataValue,
											percent;
	
	ClusterType*						currentCluster;
	char*									stringPtr1;
	Ptr									namePtr;
	SInt16*								channelsPtr;
	
	Handle								baseImageWindowHandle;
	
	UInt32								channelStart,
											maxNameLength,
											numberBytes;
	
	SInt16								areaStringStart,
											areaUnitsStringWidth,
											channel,
											characterIncrement,
											classStorage,
											clusterCount,
											fieldSize,
											numberChannels,
											numChars,
											numberPixelsInClusterWidth,
											strLength,
											tempInteger,
											totalAreaNumberWidth,
											areaPrecision;
											
	UInt16								numberEDecimalDigits,
											numberFDecimalDigits;
	
	Boolean								continueFlag;
	
	
			// Check input variables.															
			
	if (gClusterSpecsPtr == NULL || gClusterSpecsPtr->clusterHead == NULL)
																							return (FALSE);

   		// Initialize local variables. 													
   
   continueFlag = TRUE;	
   numberChannels = gClusterSpecsPtr->numberChannels;
   channelsPtr = (SInt16*)GetHandlePointer (gClusterSpecsPtr->channelsHandle);
								
	GetMinimumAndMaximumValueForListing (gClusterSpecsPtr->clusterHead,
														numberChannels,
														stdDevFlag,
														&maxDataValue,
														&minDataValue);
		
	GetNumberDecimalDigits (kRealType,
									maxDataValue,
									minDataValue,
									1,
									&numberEDecimalDigits,
									&numberFDecimalDigits);
   
   		// Get the field sizes.
   		
	WindowInfoPtr projectWindowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																gProjectInfoPtr->windowInfoHandle);																
   
   numberBytes = projectWindowInfoPtr->numberBytes;
   
   		// Get the field character widths
   		
	fieldSize = GetMeanStdDevLength (maxDataValue, 
												minDataValue,
												numberFDecimalDigits,
												numberEDecimalDigits);
												
   characterIncrement = fieldSize + 1;
	
			// Get memory for character buffer to be used in listing.  			
			// Allow 7 digits for each channel number and 200 characters for the 	
			// labels.																				
			
	tempInteger = characterIncrement + 1;
	numberBytes = (UInt32)200 + numberChannels * MAX (7,tempInteger);
	numberBytes = MAX (1024, numberBytes);
	gCharBufferPtr1 = MNewPointer (numberBytes);
	
	if (gCharBufferPtr1 == NULL)
																							return (FALSE);

			// Get the number width for the number of pixels in each cluster. Allow
			// for maximum number of pixels that were clustered. The minimum number will be
			// 6 characters.
						
	numberPixelsInClusterWidth = CreateNumberWithCommasInString (
														(char*)pixelCountString, 
														gClusterSpecsPtr->totalNumberClusterPixels);	
	numberPixelsInClusterWidth = MAX (numberPixelsInClusterWidth, 6);														
	
			// Get area conversion factor. If 0, then the map units information do not
			// exist; the area for each cluster will not be listed. If greater than 0
			// then the number of pixels in a cluster will be converted to an area based
			// on the coordinate view setting and listed. 
											
	baseImageWindowHandle = FindProjectBaseImageWindowInfoHandle ();												
	areaConversionFactor = GetDefaultAreaConversionFactor (
															gImageFileInfoPtr->mapProjectionHandle,
															baseImageWindowHandle);
															
	GetAreaNumberWidths (areaConversionFactor,
								gClusterSpecsPtr->totalNumberClusterPixels,
								&totalAreaNumberWidth,
								&areaPrecision);

			// Allow for "()" on the units string.
			
	areaUnitsStringWidth = GetAreaUnitString (baseImageWindowHandle,
															FALSE,
															(char*)gTextString) + 2;
																
	totalAreaNumberWidth = MAX (totalAreaNumberWidth, areaUnitsStringWidth);
	totalAreaNumberWidth = MAX (totalAreaNumberWidth, 4);
	
	if (areaConversionFactor <= 0)
		totalAreaNumberWidth = 0;
															
   		// List the first title line for cluster means.									
   
   channelStart = 27;
	if (gClusterSpecsPtr->projectClassMeansCode == 1)
		{						
		UInt32 numberProjectClassClusters = 
												gClusterSpecsPtr->numberInitializationClasses;
		SInt16* initializationClassPtr = (SInt16*)GetHandlePointer (
												gClusterSpecsPtr->initializationClassHandle);
												
		maxNameLength = GetClassNameMaxLength (numberProjectClassClusters, 
															initializationClassPtr,
															NULL);
		maxNameLength = MAX (13, maxNameLength);
		
   	channelStart += maxNameLength + 4;
		sprintf (gCharBufferPtr1, 
					"  %*s", 
					-((SInt16)(maxNameLength+2)),
					"\t Project_Class");
  		
  		}	// end "if (gClusterSpecsPtr->projectClassMeansCode == 1)"
  		
  	else	// gClusterSpecsPtr->projectClassMeansCode != 1
		sprintf (gCharBufferPtr1, "");

  	sprintf (titleSpaceString, "%*s", numberPixelsInClusterWidth-6, ""); 

	continueFlag = ListSpecifiedStringNumber (kClusterStrID, 
															IDS_Cluster45,
															clResultsFileStreamPtr, 
															*outputCodePtr, 
															gCharBufferPtr1, 
															titleSpaceString,
															continueFlag);
	
			// Get a blank string and force the pascal string count to be blank.
								
	if (continueFlag)						
		continueFlag = MGetString ((UCharPtr)gCharBufferPtr1, 
												kSharedStrID, 
												IDS_Shared4);
													
	gCharBufferPtr1[0] = ' ';
			
			// strLength represents the length of the line to the start of 
			// any area information.
			
	areaStringStart = (SInt16)channelStart;
	
			// Now enter title for the area information if needed.
							
	if (areaConversionFactor > 0)
		{											
		numChars = (totalAreaNumberWidth + 2 - 6)/2;
		if (numChars > 0)
			{
			continueFlag = OutputString (clResultsFileStreamPtr, 
													gCharBufferPtr1, 
													numChars, 
													*outputCodePtr, 
													continueFlag);
													
			#if defined multispec_win || defined multispec_lin   
				gCharBufferPtr1[numChars] = ' '; 
			#endif	// defined multispec_win || defined multispec_lin  

			}	// end "if (numChars > 0)"
		
		continueFlag = ListSpecifiedStringNumber (kClusterStrID, 
																IDS_Cluster48,
																(unsigned char*)gTextString, 
																clResultsFileStreamPtr, 
																*outputCodePtr, 
																continueFlag);
										
		numChars = totalAreaNumberWidth + 2 - 6 - numChars;
		if (numChars > 0)
			{
			continueFlag = OutputString (clResultsFileStreamPtr, 
													gCharBufferPtr1, 
													numChars, 
													*outputCodePtr, 
													continueFlag);
													
			#if defined multispec_win || defined multispec_lin  
				gCharBufferPtr1[numChars] = ' '; 
			#endif	// defined multispec_win || defined multispec_lin 

			}	// end "if (numChars > 0)"
									
   	channelStart += totalAreaNumberWidth + 2;
		
		}	// end "if (areaConversionFactor > 0)"
		
	continueFlag = ListSpecifiedStringNumber (kClusterStrID, 
															IDS_Cluster49,
															(unsigned char*)gTextString, 
															clResultsFileStreamPtr, 
															*outputCodePtr, 
															continueFlag);
															
   		// List the second title line for cluster means.	
		
	if (gClusterSpecsPtr->projectClassMeansCode == 1)
		sprintf (&gCharBufferPtr1[areaStringStart-3], "\t");
	
	sprintf (&gCharBufferPtr1[areaStringStart-2], "\t\t   ");
									
	if (areaConversionFactor > 0)
		{
		GetAreaUnitString (baseImageWindowHandle, FALSE, (char*)gTextString);
		
		strLength = sprintf (&gCharBufferPtr1[channelStart-totalAreaNumberWidth-2],
									"\t(%s) ",
									(char*)&gTextString[1]);
										
		gCharBufferPtr1[channelStart-totalAreaNumberWidth-2+strLength] = ' ';
		
		}	// end "if (areaConversionFactor > 0)"

	stringPtr1 = gCharBufferPtr1 + channelStart - 1;
	tempInteger = characterIncrement - 1;
	for (channel=0; channel<numberChannels; channel++)
		{
		sprintf (stringPtr1, "\t%*d", tempInteger, (int)(channelsPtr[channel]+1));
		stringPtr1 += characterIncrement;
										
		}	// end "for (channel=0; channel<numberChannels; channel++)"
		
			// Add the carriage return 
			
	sprintf (stringPtr1, "%s", gEndOfLine);
	
	continueFlag = OutputString2 (clResultsFileStreamPtr, 
											gCharBufferPtr1, 
											0, 
											*outputCodePtr, 
											continueFlag,
											kmaxNumberCharactersForLine,
											kASCIICharString);

			// Now list the cluster means for each of the clusters.
			
   clusterCount = 0;
   currentCluster = gClusterSpecsPtr->clusterHead;													
   while (currentCluster != NULL && continueFlag)
   	{
   	clusterCount++;
   	
      		// List cluster means.														
      
      stringPtr1 = gCharBufferPtr1;
				
				// Get the string with the number of pixels in the cluster class.
				
		CreateNumberWithCommasInString ((char*)pixelCountString, 
														currentCluster->numPixels);
      
      		// Get the percent of the pixels in this cluster.
      
      if (gClusterSpecsPtr->totalNumberClusterPixels > 0)		
     		percent = (double)currentCluster->numPixels/
      									gClusterSpecsPtr->totalNumberClusterPixels * 100;
      									
      else	// gClusterSpecsPtr->totalNumberClusterPixels <= 0
      	percent = 0;
      
		if (gClusterSpecsPtr->projectClassMeansCode == 1)
			{
					// Get project class name if it was used to initialize this cluster.	
					
			if (currentCluster->projectStatClassNumber >= 0)
				{	
				classStorage = 
					gProjectInfoPtr->storageClass[currentCluster->projectStatClassNumber];
			
				namePtr = (char*)&gProjectInfoPtr->classNamesPtr[classStorage].name;
				pstr ((char*)gTextString, namePtr, &strLength);
			
				}	// end "if (...->projectStatClassNumber >= 0)"
				
			else	// currentCluster->projectStatClassNumber < 0
				sprintf ((char*)gTextString, " ");
					
					// Load the class name into a string.					
		
			numChars = sprintf (gCharBufferPtr1, 
										"      %5d\t %*s\t %*s\t %5.1f ", 
										clusterCount,
										-((SInt16)(maxNameLength+2)),
										gTextString,
										numberPixelsInClusterWidth,
										pixelCountString,
										percent);
							
			}	// end "if (gClusterSpecsPtr->projectClassMeansCode == 1)"
						
		else	// gClusterSpecsPtr->projectClassMeansCode != 1
			numChars = sprintf (gCharBufferPtr1, 
										"      %5d\t %*s\t %5.1f ", 
										clusterCount,
										numberPixelsInClusterWidth, 
										pixelCountString,
										percent);
							
		if (areaConversionFactor > 0)
			{
			sprintf (&gCharBufferPtr1[numChars], 
							"\t%*.*f ", 
							totalAreaNumberWidth,
							areaPrecision,
							(double)currentCluster->numPixels * areaConversionFactor);
			
			}	// end "if (areaConversionFactor > 0)"
						
		stringPtr1 += channelStart;
      for (channel=0; channel<numberChannels; channel++)
      	{
			LoadRealValueString (stringPtr1,
										currentCluster->meanPtr[channel],
										fieldSize,
										numberFDecimalDigits,
										numberEDecimalDigits,
										(char*)"\t",
										(char*)"");
			
			stringPtr1 += characterIncrement;
		
			}	// end "for (channel=0; channel<numberChannels; channel++)" 
		
				// Add the carriage return 
				
		sprintf (stringPtr1, "%s", gEndOfLine);
			
		continueFlag = OutputString2 (clResultsFileStreamPtr, 
												gCharBufferPtr1, 
												0, 
												*outputCodePtr, 
												continueFlag,
												kmaxNumberCharactersForLine,
												kASCIICharString);
			
      currentCluster = currentCluster->next;
      
   	}	// end "while (currentCluster != NULL && continueFlag)" 
   
   		// Write out a carriage return.													
	                        
	continueFlag = OutputString (clResultsFileStreamPtr, 
											gEndOfLine, 
											0, 
											*outputCodePtr, 
											continueFlag);
   
   if (stdDevFlag && continueFlag)
   	{
   			// List the first title line for cluster standard deviations.	
   	
   	if (gClusterSpecsPtr->projectClassMeansCode == 1)		
			sprintf (gCharBufferPtr1, 
							"  %*s", 
							-((SInt16)(maxNameLength+2)),
							"\t Project_Class");
  		
  		else	// gClusterSpecsPtr->projectClassMeansCode != 1
			sprintf ((char*)gCharBufferPtr1, 
							"");			
   		
				// "\r\nCluster          Channel Standard Deviations\r\n");
		continueFlag = ListSpecifiedStringNumber (
										kClusterStrID, 
										IDS_Cluster40,
										clResultsFileStreamPtr, 
										*outputCodePtr, 
										gCharBufferPtr1, 
										continueFlag);
	
				// Get a blank string and force the pascal string count to be blank.
									
		if (continueFlag)						
			continueFlag = MGetString ((UCharPtr)gCharBufferPtr1, 
													kSharedStrID, 
													IDS_Shared4);
														
		gCharBufferPtr1[0] = ' ';
		/*		
				// Allow for width for the area if it was used.
									
		if (areaConversionFactor > 0)
			totalAreaNumberWidth += 2;
		else	// areaConversionFactor <= 0
			totalAreaNumberWidth = 0;
		*/		
   	if (gClusterSpecsPtr->projectClassMeansCode == 1)
			sprintf (&gCharBufferPtr1[areaStringStart-3], "\t");
		
		sprintf (&gCharBufferPtr1[areaStringStart-2], "\t\t   ");
		
		if (areaConversionFactor > 0)
			sprintf (&gCharBufferPtr1[areaStringStart], "\t  ");
		
		gCharBufferPtr1[areaStringStart+3] = ' ';
	
		stringPtr1 = gCharBufferPtr1 + channelStart - 1;
		
		tempInteger = characterIncrement - 1;
		for (channel=0; channel<numberChannels; channel++)
			{
			numChars = sprintf (stringPtr1, 
										"\t%*d",
										tempInteger, 
										(int)(channelsPtr[channel]+1));
			stringPtr1 += numChars;
											
			}	// end "for (channel=0; channel<numberChannels; channel++)" 
		
				// Add the carriage return 
				
		sprintf (stringPtr1, "%s", gEndOfLine);
			
		continueFlag = OutputString2 (clResultsFileStreamPtr, 
												gCharBufferPtr1, 
												0, 
												*outputCodePtr, 
												continueFlag,
												kmaxNumberCharactersForLine,
												kASCIICharString);

   	clusterCount = 0;			
   	currentCluster = gClusterSpecsPtr->clusterHead;														
	   while (currentCluster != NULL && continueFlag)
	   	{
	   	clusterCount++;
	   	
	      		// List cluster standard deviations.									
	      
			//sprintf (gCharBufferPtr1, "%s  %3d          ", gEndOfLine, clusterCount);
			//stringPtr1 += 15 + gNumberOfEndOfLineCharacters;
			
			if (gClusterSpecsPtr->projectClassMeansCode == 1)
				{
						// Get project class name if it was used to initialize this cluster.	
						
				if (currentCluster->projectStatClassNumber >= 0)
					{	
					classStorage = gProjectInfoPtr->storageClass[
															currentCluster->projectStatClassNumber];
				
					namePtr = (char*)&gProjectInfoPtr->classNamesPtr[classStorage].name;
					pstr ((char*)gTextString, namePtr, &strLength);
				
					}	// end "if (...->projectStatClassNumber >= 0)"
					
				else	// currentCluster->projectStatClassNumber < 0
					sprintf ((char*)gTextString, " ");
						
						// Load the class name into a string.					
			
				numChars = sprintf (gCharBufferPtr1, 
											"      %5d\t %*s  \t          \t  ", 
											clusterCount,
											-((SInt16)(maxNameLength+2)),
											gTextString);
								
				}	// end "if (gClusterSpecsPtr->projectClassMeansCode == 1)"
							
			else	// gClusterSpecsPtr->projectClassMeansCode != 1
				numChars = sprintf (gCharBufferPtr1, 
											"      %5d\t            \t  ", 
											clusterCount);
		
			if (areaConversionFactor > 0)
				sprintf (&gCharBufferPtr1[areaStringStart-1], "\t");
			
			gCharBufferPtr1[areaStringStart] = ' ';
							
			stringPtr1 = gCharBufferPtr1 + channelStart - 1;
		
	      for (channel=0; channel<numberChannels; channel++)
	      	{
				LoadRealValueString (stringPtr1,
											currentCluster->variancePtr[channel],
											fieldSize,
											numberFDecimalDigits,
											numberEDecimalDigits,
											(char*)"\t",
											(char*)"");
												
				stringPtr1 += characterIncrement;
			
				}	// end "for (channel=0; channel<numberChannels; channel++)" 
		
					// Add the carriage return 
					
			sprintf (stringPtr1, "%s", gEndOfLine);
				
			continueFlag = OutputString2 (clResultsFileStreamPtr, 
													gCharBufferPtr1, 
													0, 
													*outputCodePtr, 
													continueFlag,
													kmaxNumberCharactersForLine,
													kASCIICharString);

	      currentCluster = currentCluster->next;
	      
	   	}	// end "while (currentCluster != NULL && continueFlag)" 
	   
	   		// Write out a carriage return.												
		                       
		continueFlag = OutputString (clResultsFileStreamPtr, 
												gEndOfLine, 
												0, 
												*outputCodePtr, 
												continueFlag);
			
		}	// end "if (stdDevFlag && continueFlag)" 
   																			
   		// Release the memory for the character buffer.								
   		
	gCharBufferPtr1 = CheckAndDisposePtr (gCharBufferPtr1);
   	
   return (continueFlag);
   	
}	// end "ListClusterStatistics" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean LoadClusterSpecs
//
//	Software purpose:	The purpose of this routine is to load the Cluster 
//							Specification structure with the default set of
//							parameters.
//
//	Parameters in:		Pointer to image file information structure
//
//	Parameters out:	None
//
// Value Returned:	True if everything worked okay
//							False if a problem came up such as shortage of memory
// 
// Called By:			ClusterControl
//
//	Coded By:			Larry L. Biehl			Date: 09/14/1989
//	Revised By:			Larry L. Biehl			Date: 05/16/2017

Boolean LoadClusterSpecs (
				FileInfoPtr							fileInfoPtr)

{
	SInt16								*channels,
											*clusterClassPtr,
											*featurePtr,
											*initializationClassPtr;
	
	unsigned char*						symbolsPtr;
	
	Handle								baseImageWindowHandle,
											fileInfoHandle;
	
	UInt32								bytesNeeded;
	
	Boolean								changedFlag,
											firstTimeFlag,
											returnFlag;
	
	
			// Assume routine will work okay.  If there isn't enough memory		
			// then the flag will be changed.												
			
	returnFlag = TRUE;
	
	firstTimeFlag = FALSE;
		
			// Use the fileInfo handle for the project base image window, if it exists.
			// If project image file is not open then use the fileInfoHandle that
			// is stored in the project structure.
			// This effort is put in so that the settings are not released each time a 
			// new project is created when Cluster is selected.
								
	fileInfoHandle = FindProjectBaseImageFileInfoHandle ();
	if (fileInfoHandle == NULL)
		fileInfoHandle = gImageWindowInfoPtr->fileInfoHandle;
		
	baseImageWindowHandle = FindProjectBaseImageWindowInfoHandle ();
		
			// If a cluster structure already exists, then check if 					
			// it is for the current window.  If not release the old structure 	
			// and initialize a new one.														
			
	if (gNonProjProcessorSpecs.clusterSpecsH != NULL)
		{
				// Lock the handle to the cluster specifications and 					
				// get a pointer to the structure.											
				
		gClusterSpecsPtr = (ClusterSpecsPtr)GetHandlePointer (
							gNonProjProcessorSpecs.clusterSpecsH, kLock, kMoveHi);
							
		if (gClusterSpecsPtr->fileInfoHandle != fileInfoHandle || 
				gImageWindowInfoPtr->fileInfoVersion != gClusterSpecsPtr->fileInfoVersion)
			{
			ReleaseClusterSpecsMemory (&gNonProjProcessorSpecs.clusterSpecsH);
			gClusterSpecsPtr = NULL;
			
			}	// end "if (gClusterSpecsPtr->fileInfoHandle != ..." 
			
		else	// gClusterSpecsPtr->fileInfoHandle == ...
			{
			if (gProjectInfoPtr->newProjectFlag && 
													gClusterSpecsPtr->saveStatisticsCode == 2)	
				gClusterSpecsPtr->saveStatisticsCode = 1;
			
			}	// end "else gClusterSpecsPtr->fileInfoHandle == ..." 
				
		}	// end "else gNonProjProcessorSpecs.clusterSpecsH != NULL" 
	
	if (gNonProjProcessorSpecs.clusterSpecsH == NULL)
		{
				// Set up handle to project file information								
			
		gNonProjProcessorSpecs.clusterSpecsH = MNewHandle (sizeof (ClusterSpecs));
		if (gNonProjProcessorSpecs.clusterSpecsH != NULL)
			{
			gClusterSpecsPtr = (ClusterSpecsPtr)GetHandlePointer (
										gNonProjProcessorSpecs.clusterSpecsH, kLock, kMoveHi);
			
					// Initialize the cluster specification structure.					
		
			if (gProjectInfoPtr->newProjectFlag)	
				gClusterSpecsPtr->saveStatisticsCode = 1;
				
			else	// !gProjectInfoPtr->newProjectFlag 
				gClusterSpecsPtr->saveStatisticsCode = 1;
			
			gClusterSpecsPtr->fileInfoHandle = fileInfoHandle;
			gClusterSpecsPtr->fileInfoVersion = gImageWindowInfoPtr->fileInfoVersion;
			gClusterSpecsPtr->classificationArea = 0;
			
					// Note that this default threshold is very rough. It should be based
					// on the actual range of the data values. But that is not set up yet.
					
			gClusterSpecsPtr->classifyThreshold = 
													ldexp (1., fileInfoPtr->numberBits/2);
			if (fileInfoPtr->numberBits > 8)
				gClusterSpecsPtr->classifyThreshold *= 4;
				
			gClusterSpecsPtr->clustersFrom = kAreaType;
			gClusterSpecsPtr->channelSet = kAllMenuItem;
			gClusterSpecsPtr->numberChannels = fileInfoPtr->numberChannels;
			gClusterSpecsPtr->clusterClassSet = kAllMenuItem;
			
			gClusterSpecsPtr->diskFileFormat = 0;
			
			gClusterSpecsPtr->numberClusterClasses = 0;
			
			gClusterSpecsPtr->imageOverlayIndex = -1;
			
			gClusterSpecsPtr->initializationClassSet = kAllMenuItem;
			gClusterSpecsPtr->numberInitializationClasses = 0;
												
			gClusterSpecsPtr->minNumberClusters = 6;
			gClusterSpecsPtr->maxNumberClusters = 10;
			gClusterSpecsPtr->minClusterSize = 
													gImageWindowInfoPtr->totalNumberChannels + 1;
			gClusterSpecsPtr->numberClusters = 0;
			gClusterSpecsPtr->numberFinalClusters = 0;
			gClusterSpecsPtr->symbolSet = kDefaultMenuItem;
			gClusterSpecsPtr->mode = 0;
			
			gClusterSpecsPtr->imageLineStart = 1;
			gClusterSpecsPtr->imageLineEnd = gImageWindowInfoPtr->maxNumberLines;
			gClusterSpecsPtr->imageLineInterval = 1;
			gClusterSpecsPtr->imageColumnStart = 1;
			gClusterSpecsPtr->imageColumnEnd = gImageWindowInfoPtr->maxNumberColumns;
			gClusterSpecsPtr->imageColumnInterval = 1;
				
					// Options for single pass cluster algorithm.						
			
			gClusterSpecsPtr->criticalDistance1 = GetAverageChannelStandardDev (
																baseImageWindowHandle, fileInfoPtr);
			gClusterSpecsPtr->criticalDistance2 = 2 * gClusterSpecsPtr->criticalDistance1;
	
					// The following variables are for the ISODATA algorithm.		
					
			gClusterSpecsPtr->convergence = (float)99.0;
			gClusterSpecsPtr->initializationOption = 2;
			gClusterSpecsPtr->projectClassMeansCode = 0;
			gClusterSpecsPtr->useCorrelationMatrixFlag = FALSE;
	
			gClusterSpecsPtr->clusterClassSet = kAllMenuItem;
			gClusterSpecsPtr->outputStorageType = 1;
			gClusterSpecsPtr->featureHandle = NULL;
			gClusterSpecsPtr->channelsHandle = NULL;
			gClusterSpecsPtr->symbolsHandle = NULL;
			gClusterSpecsPtr->clusterClassHandle = NULL;
			gClusterSpecsPtr->initializationClassHandle = NULL;
				
			gClusterSpecsPtr->clusterLineStart = 1;
			gClusterSpecsPtr->clusterLineEnd = gImageWindowInfoPtr->maxNumberLines;
			gClusterSpecsPtr->clusterLineInterval = 1;
			gClusterSpecsPtr->clusterColumnStart = 1;
			gClusterSpecsPtr->clusterColumnEnd = gImageWindowInfoPtr->maxNumberColumns;
			gClusterSpecsPtr->clusterColumnInterval = 1;
	
					// The following variables are used by all the routines.			
					
			gClusterSpecsPtr->totalNumberAreas = 0;
			gClusterSpecsPtr->totalNumberClusterPixels = 0;
			gClusterSpecsPtr->totalSumSquaresStatsHandle = NULL;
			gClusterSpecsPtr->totalChanStatsHandle = NULL;
			gClusterSpecsPtr->totalChanStatsPtr = NULL;
			gClusterSpecsPtr->totalSumSquaresStatsPtr = NULL;
			gClusterSpecsPtr->dataClassPtr = NULL;
			gClusterSpecsPtr->clusterClassToFinalClassPtr = NULL;
			gClusterSpecsPtr->clusterMemoryHead = NULL;
			gClusterSpecsPtr->clusterHead = NULL;
			gClusterSpecsPtr->clusterEigenCenterHead = NULL;
			
					// Variables used in clustering thresholded data.					
				
			gClusterSpecsPtr->probabilityThreshold = .1;
			gClusterSpecsPtr->probabilityFileInfoHandle = NULL;
			gClusterSpecsPtr->probabilityThresholdCode = 0;
			gClusterSpecsPtr->thresholdFlag = FALSE;
			
					// Variable used for image overlays.
					
			gClusterSpecsPtr->transparency = 0;
			
			}	// end "if (gNonProjProcessorSpecs.clusterSpecsH != NULL)" 
		
		else	// else gNonProjProcessorSpecs.clusterSpecsH == NULL 
			returnFlag = FALSE;
	
		firstTimeFlag = TRUE;
			
		}	// end "if (gNonProjProcessorSpecs.clusterSpecsH == NULL)" 
		
			// Force the default for the popup overlay to be a new overlay.
			
	gClusterSpecsPtr->imageOverlayIndex = -1;
	
			// Make sure that the number of potential project classes to be used
			// for initialization makes sense.
			
	gClusterSpecsPtr->numberInitializationClasses = 0;		
	if (gProjectInfoPtr != NULL)
		gClusterSpecsPtr->numberInitializationClasses =
												gProjectInfoPtr->numberStatTrainClasses;
					
			// If Okay, set up memory for cluster feature pointer vector.			

	if (returnFlag)
		{
		bytesNeeded = (SInt32)fileInfoPtr->numberChannels * sizeof (SInt16);
		featurePtr = (SInt16*)CheckHandleSize (&gClusterSpecsPtr->featureHandle, 
															&returnFlag, 
															&changedFlag, 
															bytesNeeded);
		if (changedFlag)
			gClusterSpecsPtr->channelSet = kAllMenuItem;
			
		}	// end "if (returnFlag)" 
			
			// If memory is not full, set up memory for cluster						
			// channels vector.																	

	if (returnFlag)
		{
		channels = (SInt16*)CheckHandleSize (&gClusterSpecsPtr->channelsHandle,
															&returnFlag, 
															&changedFlag, 
															bytesNeeded);
			
		}	// end "if (returnFlag)" 
			
			// If memory is not full, set up memory for cluster						
			// classes vector.
			
	if (gProjectInfoPtr->newProjectFlag)
		gClusterSpecsPtr->numberClusterClasses = 0;
		
	else	// !gProjectInfoPtr->newProjectFlag
		{ 
		if (gClusterSpecsPtr->numberClusterClasses == 0)
			gClusterSpecsPtr->numberClusterClasses = 
														gProjectInfoPtr->numberStatisticsClasses;
														
		else	// gClusterSpecsPtr->numberClusterClasses > 0
			gClusterSpecsPtr->numberClusterClasses = MIN (
														gProjectInfoPtr->numberStatisticsClasses,
														gClusterSpecsPtr->numberClusterClasses);
		}	// end "else !gProjectInfoPtr->newProjectFlag"	
															

	if (returnFlag && gProjectInfoPtr->numberStatisticsClasses > 0)
		{	
		bytesNeeded = (SInt32)gProjectInfoPtr->numberStatisticsClasses * 
																						sizeof (SInt16);
		clusterClassPtr = (SInt16*)CheckHandleSize (
															&gClusterSpecsPtr->clusterClassHandle,
															&returnFlag,  
															&changedFlag, 
															bytesNeeded);

		if (changedFlag)
			gClusterSpecsPtr->clusterClassSet = kAllMenuItem;
			
		}	// end "if (returnFlag && ...)" 
			
			// If memory is not full, set up memory for initialization classes vector.																	

	if (returnFlag && gClusterSpecsPtr->numberInitializationClasses > 0)
		{	
		bytesNeeded = (SInt32)gProjectInfoPtr->numberStatTrainClasses * sizeof (SInt16);
		initializationClassPtr = (SInt16*)CheckHandleSize (
													&gClusterSpecsPtr->initializationClassHandle,
													&returnFlag,  
													&changedFlag, 
													bytesNeeded);

		if (changedFlag)
			gClusterSpecsPtr->initializationClassSet = kAllMenuItem;
			
		}	// end "if (returnFlag && ...)" 
			
			// If memory is not full, check memory for cluster symbols vector.	

	if (returnFlag)
		{			
		bytesNeeded = (SInt32)(kMaxNumberStatClasses+1) * sizeof (char);
		symbolsPtr = (UCharPtr)CheckHandleSize (&gClusterSpecsPtr->symbolsHandle,
																&returnFlag,  
																&changedFlag, 
																bytesNeeded);
		if (changedFlag)
			gClusterSpecsPtr->symbolSet = kDefaultMenuItem;
										
		}	// end "if (returnFlag)" 
		
			// Load selection area information into the cluster 						
			// specification information if the selection rectangle has 			
			// been set up.																		

	gSelectionRectangleFlag = FALSE;
	if (returnFlag)
		{
		if (GetSelectedAreaInfo (gProjectSelectionWindow,
											gImageWindowInfoPtr,
											&gClusterSpecsPtr->imageLineStart,
											&gClusterSpecsPtr->imageLineEnd,
											&gClusterSpecsPtr->imageColumnStart,
											&gClusterSpecsPtr->imageColumnEnd,
											kDontClearSelectionArea,
											kUseThreshold,
											kAdjustToBaseImage))
			{
			gClusterSpecsPtr->clusterLineStart = gClusterSpecsPtr->imageLineStart;
			gClusterSpecsPtr->clusterLineEnd = gClusterSpecsPtr->imageLineEnd;
			gClusterSpecsPtr->clusterColumnStart = gClusterSpecsPtr->imageColumnStart;
			gClusterSpecsPtr->clusterColumnEnd = gClusterSpecsPtr->imageColumnEnd;
				
			}	// end "if (GetSelectedAreaInfo (..." 
													
		if (firstTimeFlag)
			{
			gClusterSpecsPtr->clusterColumnInterval = MAX (
				1,
				(gClusterSpecsPtr->clusterColumnEnd - 
															gClusterSpecsPtr->clusterColumnStart) * 
					(gClusterSpecsPtr->clusterLineEnd - 
													gClusterSpecsPtr->clusterLineStart)/10000000);
			
			gClusterSpecsPtr->clusterColumnInterval = MIN (
														5,
														gClusterSpecsPtr->clusterColumnInterval);
														
			}	// end "if (firstTimeFlag)"
				
				// Fill in array of pointers to all channels.							
		
		if (gClusterSpecsPtr->channelSet == kAllMenuItem)				
			LoadFeatureVector (kNoTransformation,
									(SInt16*)&gClusterSpecsPtr->numberChannels,
									featurePtr,
									fileInfoPtr->numberChannels);
			
				// Make certain that channel and class vectors are						
				// filled in properly.															
															
		LoadChannelsVector (kNoProject,
									kNoTransformation,
									gClusterSpecsPtr->numberChannels, 
									featurePtr, 
									(SInt16*)&gClusterSpecsPtr->numberChannels,
									channels);
		
		if (gClusterSpecsPtr->clusterClassSet == kAllMenuItem && 
													gClusterSpecsPtr->numberClusterClasses > 0)								
			LoadClassVector (&gClusterSpecsPtr->numberClusterClasses, clusterClassPtr);
		
		if (gClusterSpecsPtr->initializationClassSet == kAllMenuItem && 
											gClusterSpecsPtr->numberInitializationClasses > 0)								
			LoadTrainClassVector (&gClusterSpecsPtr->numberInitializationClasses,
											&gClusterSpecsPtr->initializationClassSet,
											initializationClassPtr);
		
		if (gClusterSpecsPtr->symbolSet == kDefaultMenuItem)
			LoadClassSymbolVector (symbolsPtr, IDS_Symbol1, kMaxNumberStatClasses);
			
		}	// end "if (returnFlag)" 
	
			// If unable to set up information for cluster specifications, 		
			// release memory allocated to it (if any).									
			
	if (!returnFlag)
		{
		ReleaseClusterSpecsMemory (&gNonProjProcessorSpecs.clusterSpecsH);
		gClusterSpecsPtr = NULL;
		
		}	// end "if (!returnFlag)" 
		
	return (returnFlag);

}	// end "LoadClusterSpecs" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean SaveClusterStatistics
//
//	Software purpose:	The purpose of this routine is to save the
//							cluster statistics in the project file if requested.
//
//	Parameters in:		Pointer to image file information structure
//							Number of cluster classes to be saved in project
//											structure.
//
//	Parameters out:	None
//
// Value Returned:	True if everything worked okay
//							False if a problem came up such as shortage of memory
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 09/26/1989
//	Revised By:			Larry L. Biehl			Date: 10/01/2013

Boolean SaveClusterStatistics (
				FileIOInstructionsPtr			fileIOInstructionsPtr, 
				SInt16								numberClasses)

{
	CharPtr								namePtr;
	ClusterType*       				cluster;
	HChannelStatisticsPtr			chanStatsPtr;
	HCovarianceStatisticsPtr		sumSquaresStatsPtr;
	HPClassNamesPtr					classNamesPtr;
	
	ProjectInfoPtr						classNameProjectInfoPtr,
											savedProjectInfoPtr;
											
	SInt16*								channelsPtr;
	
	SInt32								fieldNumber,
											numberSumSquares;
	
	SInt16								classIndex,
											classStorage,
											fieldStorage,
											index,
											numberChannels,
											savedClassStorage,
											savedFieldStatStorage,
											storageIndex;
	
	Boolean								continueFlag,
											projectStatsUpToDateFlag;
	

	if (gClusterSpecsPtr->saveStatisticsCode > 0 
			&& numberClasses > 0
			&& fileIOInstructionsPtr->fileInfoPtr != NULL)
		{
		if (gClusterSpecsPtr->clusterClassToFinalClassPtr == NULL)
																							return (FALSE);
	
		LoadDItemStringNumber (kClusterStrID, 
										IDS_Cluster29,	// "Saving Statistics ..."
										gStatusDialogPtr, 
										IDC_Status11, 
										(Str255*)gTextString);
		
				// Initialize local variables.												
				
		channelsPtr = (SInt16*)GetHandlePointer (gClusterSpecsPtr->channelsHandle);
		continueFlag = TRUE;
		projectStatsUpToDateFlag = gProjectInfoPtr->statsUpToDate;
		savedProjectInfoPtr = NULL;
				
				// If this is to a new project, then close the current project		
				// and create a new one.  The user was already asked whether the	
				// Current project was saved, if needed, before clustering began.	
		
		if (gClusterSpecsPtr->saveStatisticsCode == 1 &&	
																!gProjectInfoPtr->newProjectFlag)
			{
					// Save the old project for a while in case needed for
					// the field boundaries or the project class means were used
					// Release the memory presently used for the old project's statistics.	
			
			ReleaseStatisticsHandles (gProjectInfoPtr, 
												kOriginalStats+kLeaveOneOutStats+kEnhancedStats);
				
			savedProjectInfoPtr = gProjectInfoPtr;
			gProjectInfoPtr->changedFlag = FALSE;
			if (!CreateNewProject (TRUE))		// Save the old project.
				{
				CloseProjectStructure (savedProjectInfoPtr);
				
																							return (FALSE);
				}	// end "if (!CreateNewProject ())" 
																				
			projectStatsUpToDateFlag = TRUE;
			
			FindProjectImageWindows (FALSE);
			
			}	// end "if (gClusterSpecsPtr->saveStatisticsCode == 1 && ..." 
		
				// Indicate that an attempt should be made to move the project		
				// memory high.																	
				
		gProjectInfoPtr->moveMemoryFlag = TRUE;
												
				// Create the classes.															
		
		savedClassStorage = gProjectInfoPtr->numberStorageClasses;
		savedFieldStatStorage = gProjectInfoPtr->numberStorageStatFields;
		
		sprintf ((char*)&gTextString[1], "Cluster");
		gTextString[0] = 7; 
		
		sprintf ((char*)&gTextString3[1], "pClusterF");
		gTextString3[0] = 8; 
		
		fieldNumber = 1;
		cluster = gClusterSpecsPtr->clusterHead;
		
		classNameProjectInfoPtr = gProjectInfoPtr;
		if (savedProjectInfoPtr != NULL)
			classNameProjectInfoPtr = savedProjectInfoPtr;
		
		for (classIndex=1; classIndex<=numberClasses; classIndex++)
			{
			if (cluster == NULL)
				break;
				
					// Get unique name for the class.
			
			namePtr = NULL;
			if (gClusterSpecsPtr->projectClassMeansCode == 1 &&
															cluster->projectStatClassNumber >= 0)
				{
				classStorage = classNameProjectInfoPtr->storageClass[
																	cluster->projectStatClassNumber];
	
				namePtr = 
						(char*)&classNameProjectInfoPtr->classNamesPtr[classStorage].name;	
				
				}	// end "if (gClusterSpecsPtr->projectClassMeansCode == 1)"
					
			GetUniqueClassName (classIndex,
										(UCharPtr)gTextString,
										7,
										(gClusterSpecsPtr->projectClassMeansCode == 1),
										namePtr);
											
			GetUniqueFieldName (fieldNumber,
										(UCharPtr)gTextString3,
										8);
										
			continueFlag = AddClassToProject (gTextString);
			
			if (continueFlag)
				continueFlag = AddFieldToProject (gProjectInfoPtr->currentClass, 
																kClusterType,
																kTrainingType,
																gTextString3);
			if (!continueFlag)
				break;
				
			cluster = cluster->next;
			
			fieldNumber++;
				
			}	// end "for (classIndex=0; classIndex<..." 
			
		if (!continueFlag)
																							return (FALSE);
			
				// Update the project channels information for the new project case.																				
		
		if (gClusterSpecsPtr->saveStatisticsCode == 1)
			{		
			gProjectInfoPtr->numberStatisticsChannels =
																gClusterSpecsPtr->numberChannels;
		
					// Copy channel array from cluster information to project 		
					// information.																
				
			for (index=0; index<gProjectInfoPtr->numberStatisticsChannels; index++)
				gProjectInfoPtr->channelsPtr[index] = channelsPtr[index];
				
			}	// end "if (gClusterSpecsPtr->saveStatisticsCode == 1)" 
			
		if (gProjectInfoPtr->numberCovarianceEntries <= 0)
			gProjectInfoPtr->numberCovarianceEntries = 
							(SInt32)gProjectInfoPtr->numberStatisticsChannels * 
									(gProjectInfoPtr->numberStatisticsChannels + 1)/2;
			
				// Make certain that memory for statistics information is 			
				// sufficient.																		
				 
		if (!SetupStatsMemory ())
																							return (FALSE);
														
  		storageIndex = savedFieldStatStorage;
  		if (gProjectInfoPtr->keepClassStatsOnlyFlag)	
  			storageIndex = savedClassStorage;
  									
		continueFlag = GetClusterProjectStatistics (fileIOInstructionsPtr, 
																	savedProjectInfoPtr, 
																	numberClasses, 
																	storageIndex);
				
				// Release the memory for the project structure saved for the 		
				// class information.															
		
		CloseProjectStructure (savedProjectInfoPtr);
		
		if (!continueFlag)
																							return (FALSE);
		
				// Compute the standard deviation, correlation matrix and			
				// covariance matrices.															
		
		cluster = gClusterSpecsPtr->clusterHead;
		if (cluster == NULL)
																							return (FALSE);

		classNamesPtr = gProjectInfoPtr->classNamesPtr;
		
		numberChannels = gProjectInfoPtr->numberStatisticsChannels;
		numberSumSquares = gProjectInfoPtr->numberCovarianceEntries;
		
		GetProjectStatisticsPointers (gProjectInfoPtr->keepClassStatsOnlyFlag+1, 
												storageIndex, 
												&chanStatsPtr, 
												&sumSquaresStatsPtr,
												NULL,
												NULL);
		
  		classStorage = savedClassStorage;
		for (classIndex=1; classIndex<=numberClasses; classIndex++)
			{
					// Update class statistics.											
					
			classNamesPtr[classStorage].numberStatisticsPixels = cluster->numPixels;
			classNamesPtr[classStorage].numberTrainPixels = cluster->numPixels;
			
			classNamesPtr[classStorage].statsUpToDate = TRUE;
			
					// Update field statistics.												
					// Get pointers for field information.									
					
			fieldStorage = classNamesPtr[classStorage].firstFieldNumber;
					
			gProjectInfoPtr->fieldIdentPtr[fieldStorage].numberPixels = 
																					cluster->numPixels;
																			
			gProjectInfoPtr->fieldIdentPtr[fieldStorage].numberPixelsUsedForStats = 
																					cluster->numPixels;
																
			gProjectInfoPtr->fieldIdentPtr[fieldStorage].loadedIntoClassStats = TRUE;
										
					// Compute the first order statistics.																		
			
			ComputeMeanStdDevVector (
									chanStatsPtr, 
									sumSquaresStatsPtr, 
									numberChannels, 
									gProjectInfoPtr->fieldIdentPtr[fieldStorage].numberPixels,
									gProjectInfoPtr->statisticsCode,
									kTriangleInputMatrix);
								
  			if (!gProjectInfoPtr->keepClassStatsOnlyFlag)	
				gProjectInfoPtr->fieldIdentPtr[fieldStorage].statsUpToDate = TRUE;
				
			cluster = cluster->next;
			classStorage++;
			if (cluster == NULL)
				classIndex = numberClasses + 1;
				
			chanStatsPtr += numberChannels;
			sumSquaresStatsPtr += numberSumSquares;
			
			}	// end "for (classIndex=1; classIndex<=..." 
			
				// Indicate that project information has changed and update	menu item.																				
				
		gProjectInfoPtr->newProjectFlag = FALSE;
		gProjectInfoPtr->changedFlag = TRUE;
		gProjectInfoPtr->statsLoaded = TRUE;
		gProjectInfoPtr->statsUpToDate = projectStatsUpToDateFlag;
		gUpdateProjectMenuItemsFlag = TRUE;
		gUpdateProcessorMenuItemsFlag = TRUE;
		
				// If the statistics window is open force the statistics window 	
				// mode to be 'select new field'.											
		
		if (gProjectWindow != NULL && gProjectInfoPtr->statsWindowMode != 1) 
			NewFieldStatMode ();
		
		}	// end "if (gClusterSpecsPtr->createProjectStatistics)" 
		
	return (TRUE);

}	// end "SaveClusterStatistics" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean SetUpClassToFinalClassPtr
//
//	Software purpose:	The purpose of this routine is to set up the vector
//							which translates the cluster class number to the final
//							class number.  The final cluster numbers will start at
//							0.  The maximum number of final clusters allowed will
//							be kMaxNumberStatClasses less one for the background class.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	True if everything worked okay
//							False if a problem came up such as shortage of memory
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 03/19/1991
//	Revised By:			Larry L. Biehl			Date: 05/09/2000	

Boolean SetUpClassToFinalClassPtr (void)

{
	ClusterType							*currentCluster;
	SInt16								*clusterClassToFinalClassPtr;
	
	SInt16								finalClusterNumber,
											index;
	
	Boolean								changedFlag;
	
	
	if (gClusterSpecsPtr == NULL)
																							return (FALSE);
	
	if (gClusterSpecsPtr->numberClusters <= 0 ||
													gClusterSpecsPtr->clusterHead == NULL)
																							return (FALSE);
	
	if (gClusterSpecsPtr->clusterClassToFinalClassPtr == NULL)
																							return (TRUE);
			
			// Verify that the vector is long enough.	
			// Release current vector and get the correct size vector that is needed.
			
	gClusterSpecsPtr->clusterClassToFinalClassPtr = CheckAndDisposePtr (
													gClusterSpecsPtr->clusterClassToFinalClassPtr);								
																			
	if (!CheckPointerSize (
				(Ptr*)&gClusterSpecsPtr->clusterClassToFinalClassPtr, 
				(gClusterSpecsPtr->numberClusters+1) * sizeof (SInt16), 
				&changedFlag))
																							return (FALSE);

	clusterClassToFinalClassPtr = gClusterSpecsPtr->clusterClassToFinalClassPtr;
																						
			// Initialize the vector.														
			
	for (index=0; index<=gClusterSpecsPtr->numberClusters; index++)
		clusterClassToFinalClassPtr[index] = -1;
			
			// Loop through the clusters to obtain the cluster numbers.			
	
	finalClusterNumber = 0;
	currentCluster = gClusterSpecsPtr->clusterHead;
	while (currentCluster != NULL)
   	{
   	clusterClassToFinalClassPtr[currentCluster->clusterNumber] = finalClusterNumber;
      currentCluster = currentCluster->next;
      
      finalClusterNumber++;
   		
   	}	// end "while (currentCluster != NULL)" 
   	
   gClusterSpecsPtr->numberFinalClusters = finalClusterNumber;
   	
   return (TRUE);
   	
}	// end "SetUpClassToFinalClassPtr" 


#if defined multispec_mac
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void SpinCursorTimer
//
//	Software purpose:	The purpose of this routine is to spin the cursor under
//							system control. It was created for testing. It needs more work.
//
//	Parameters in:		Event Loop Timer
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: ??/??/????
//	Revised By:			Larry L. Biehl			Date: ??/??/????	

pascal void SpinCursorTimer (
				EventLoopTimerRef					inTimer,
				void*									inUserData)
				
{
	gSpinCursorIndex++;
	if (gSpinCursorIndex > kSpinEnd)
		gSpinCursorIndex = kSpin;

	SetCursor (gCursorPtr[gSpinCursorIndex]);

}	// end "SpinCursorTimer"	
#endif	// defined multispec_mac



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean UpdateClusterStdDeviations
//
//	Software purpose:	The purpose of this routine is to update the standard
//							deviations for the clusters.  A new standard deviation
//							will be computed base on the latest counts, sums and sums of
//							squares.
//
//	Parameters in:		ClusterType structure pointer
//
//	Parameters out:	None
//
// Value Returned:	True if everything worked okay
//							False if a problem came up such as shortage of memory
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/10/1990
//	Revised By:			Larry L. Biehl			Date: 08/10/1990	

Boolean UpdateClusterStdDeviations (
				ClusterType*						newCluster)

{
	HDoublePtr							clusterSumPtr,
											clusterSumSquarePtr,
											clusterVariancePtr;
	
	SInt32								numberPixels,
											numberPixelsLessOne;
	
	SInt16								channel,
											numberChannels;
	
	
			// Check input variables.															
			
	if (gClusterSpecsPtr->clusterHead == NULL)
																							return (FALSE);
																					
	if (gClusterSpecsPtr->numberChannels <= 0)
																							return (FALSE);
																					
			// Initialize local variables.													
			
	numberChannels = gClusterSpecsPtr->numberChannels;
	
	do
		{
		clusterSumPtr = newCluster->sumPtr;
		clusterSumSquarePtr = newCluster->sumSquarePtr1;
		clusterVariancePtr = newCluster->variancePtr;
		numberPixels = newCluster->numPixels;
		numberPixelsLessOne = numberPixels - 1;
		
		if (numberPixels > 1)
			{
			for (channel=0; channel < numberChannels; channel++)
				{
				*clusterVariancePtr = (*clusterSumSquarePtr - 
												*clusterSumPtr * *clusterSumPtr/numberPixels)/
																					numberPixelsLessOne;
					      												
				*clusterVariancePtr = sqrt (*clusterVariancePtr);	      												
				
			   clusterSumPtr++;
			   clusterSumSquarePtr++;
			   clusterVariancePtr++;
			   
			   }	// end "for (channel = 0; channel < ..." 
			   
			}	// end "if (numberPixels > 1)" 
			
		else if (numberPixels == 1)
			{
			for (channel=0; channel < numberChannels; channel++)
				{
				*clusterVariancePtr = 0;	      												
				
			   clusterVariancePtr++;
			   
			   }	// end "for (channel = 0; channel < ..." 
			
			}	// end "else if (numberPixels == 1)" 
		   
		newCluster = newCluster->next;
		
		}	while (newCluster != NULL);
		
	return (TRUE);
	
}	//  end "UpdateClusterStdDeviations" 
