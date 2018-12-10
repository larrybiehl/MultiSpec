//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SClusterSinglePass.cpp
//
//	Authors:					Eric E. Demaree, Larry L. Biehl
//
//	Revision date:			12/21/2017
//
//	Language:				C
//
//	System:					Linux, Macintosh and Windows Operating Systems
//
//	Brief description:	This file contains routines that will cluster image data
//								with one pass.  The routines in the file were written
//								by Eric E. Demaree for an EE577 class project during
//								the spring semester of 1989 under the direction of
//								Professor Phil Swain.
//
//	Functions in file:	void 				ClusterOnePassControl
//								Boolean			GetOnePassClusterCenters
//								SInt16	 		OnePassCluster
//								Boolean 			OnePassClusterDialog
//								Boolean 			OnePassClusterAreas
//
//	Diagram of MultiSpec routine calls for the routines in the file.
//
//		OnePassClusterDialog
//			ProcessorDialogFilter (in MDialogUtilities.c)
//
//
//		ClusterOnePassControl
//			InitializeClusterMemory (in SCluster.cpp)
//
//			OnePassCluster
//				OnePassClusterAreas
//
//				UpdateClusterStdDeviations (in SCluster.cpp)
//
//				ListClusterStatistics (in SCluster.cpp)
//
//			SetUpClassToFinalClassPtr (in SCluster.cpp)
//
//			ClusterClassification (in SCluster.cpp)
//
//			SaveClusterStatistics (in SCluster.cpp)
//
//
//		GetOnePassClusterCenters
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"    

#if defined multispec_lin  
	#include "LClusterSinglePassDialog.h"   
	#include "LImageView.h"
#endif	// defined multispec_lin

#if defined multispec_mac || defined multispec_mac_swift
	#define IDC_ClusterTrainingAreas			12 
	#define IDC_LineStart						19 
	#define IDC_LineEnd							20 
	#define IDC_ColumnStart						22 
	#define IDC_ColumnEnd						23         
	#define IDC_ClassPrompt						27 
	#define IDC_ClassCombo						28
#endif	// defined multispec_mac || defined multispec_mac_swift

#if defined multispec_win    
	#include "WImageView.h"
	#include "WClusterSinglePassDialog.h" 
#endif	// defined multispec_win    

//#include "SExtGlob.h"



//------------------------------------------------------------------------------------
//
// M a c r o s 
//
//------------------------------------------------------------------------------------

#define AddNewCluster(pix, gClusterHead)													\
	{																									\
   short int 				channelTemp;														\
   ClusterType*			newCluster;															\
	HCTypePtr  				pixPtr1;																\
	HDoublePtr				clusterSumSquarePtr;												\
																										\
																										\
	newCluster = (ClusterType*)MNewPointer ((SInt32)sizeof (ClusterType) +		\
					sizeof (CMeanType)*gClusterSpecsPtr->numberChannels +				\
										sizeof (double)*gSizeOfDoubleVector);				\
	if (newCluster == NULL)									return (FALSE);				\
	newCluster->next = gClusterHead;															\
	gClusterHead = newCluster;																	\
	gClusterHead->numPixInCluster = 0;														\
	gClusterHead->numPixels = 1;																\
	gClusterHead->sumPtr = NULL;																\
	gClusterHead->meanPtr = (HCMeanTypePtr)&newCluster[1];							\
	gClusterHead->sumPtr = (HDoublePtr)&gClusterHead->meanPtr[						\
												gClusterSpecsPtr->numberChannels];			\
	gClusterHead->variancePtr = &gClusterHead->sumPtr[numberChannels];			\
	clusterSumSquarePtr = gClusterHead->sumSquarePtr1 = 								\
											&gClusterHead->variancePtr[numberChannels];	\
	gClusterHead->sumSquarePtr2 = NULL;														\
	pixPtr1 = pix;																					\
	for (channelTemp = 0; channelTemp < numberChannels; channelTemp++)			\
		{																								\
		*(gClusterHead->meanPtr + channelTemp) = *pixPtr1;					   		\
		*(gClusterHead->sumPtr + channelTemp) = *pixPtr1;								\
		*clusterSumSquarePtr = (double)*pixPtr1 * *pixPtr1;							\
		pixPtr1++;																					\
		clusterSumSquarePtr++;																	\
		}																								\
	gClusterHead->varianceComputed = FALSE;												\
																										\
	gTotalNumberOfClusters++;																	\
	gClusterHead->clusterNumber = gTotalNumberOfClusters;								\
	gClusterHead->projectStatClassNumber = -1;											\
      																								\
	}

//------------------------------------------------------------------------------------

#define Distance(cluster, pix, distance)													\
	{																									\
	short int    			channelTemp;														\
	CMeanType2   			normDistanceTemp;													\
	HCTypePtr  				pixPtr;																\
	HCMeanTypePtr			clusterMeanPtr;													\
																										\
	pixPtr = pix;																					\
	clusterMeanPtr = cluster->meanPtr;														\
	distance = 0;																					\
	for (channelTemp = 0; channelTemp < numberChannels; channelTemp++)			\
		{																								\
		normDistanceTemp = (CMeanType2)*pixPtr - 											\
										(CMeanType2)*clusterMeanPtr;							\
		distance += (double)normDistanceTemp * normDistanceTemp;						\
		pixPtr++;																					\
		clusterMeanPtr++;																			\
	   }																								\
	}

//------------------------------------------------------------------------------------

#define UpdateClusterMean(pix, cluster)													\
	{																									\
	HDoublePtr   			clusterSumPtr;														\
	HDoublePtr				clusterSumSquarePtr;												\
	short int 				channelTemp;														\
	SInt32					numPixTemp;															\
	HCTypePtr  				pixPtr1;																\
	HCMeanTypePtr   		clusterMeanPtr;													\
																										\
	clusterMeanPtr = cluster->meanPtr;														\
	clusterSumPtr = cluster->sumPtr;															\
	clusterSumSquarePtr = cluster->sumSquarePtr1;										\
	(cluster->numPixels)++;																		\
	numPixTemp = cluster->numPixels;															\
	pixPtr1 = pix;																					\
	for (channelTemp = 0; channelTemp < numberChannels; channelTemp++)			\
		{																								\
	   *clusterSumPtr += *pixPtr1; 			 												\
	   																								\
	   	*clusterMeanPtr = *clusterSumPtr/(double)numPixTemp;						\
	   																								\
/*	   	*clusterMeanPtr =                  												\
	   		(CMeanType)((*clusterSumPtr/(double)numPixTemp) + 0.5);	*/			\
	   																								\
	   clusterMeanPtr++;																			\
	   clusterSumPtr++;																			\
																										\
		*clusterSumSquarePtr += (double)*pixPtr1 * *pixPtr1;							\
		clusterSumSquarePtr++;																	\
	   pixPtr1++;																					\
	   }																								\
	cluster->varianceComputed = FALSE;														\
	}

//------------------------------------------------------------------------------------



		// Prototypes for file routines that are only called from other
		// routines in this file.

SInt16 OnePassCluster (
				FileIOInstructionsPtr			fileIOInstructionsPtr);

Boolean OnePassClusterAreas (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				HUInt16Ptr*							dataClassPtrPtr,
				SInt16								firstLineCode);



// Global variables for this file.													

SInt16			gTotalNumberOfClusters;
UInt32			gSizeOfDoubleVector;
ClusterType*	gClusterHead; // ptr to head of cluster list.



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ClusterOnePassControl
//
//	Software purpose:	The purpose of this routine is to control the user
//							requests for cluster operations on an image file.
//
//	Parameters in:		Pointer to image file information structure
//
//	Parameters out:	None
//
// Value Returned:	True if everything worked okay
//							False if a problem came up such as shortage of memory
// 
// Called By:			ClusterControl in cluster.c
//
//	Coded By:			Eric E. Demaree		Date: Spring 1989
//	Revised By:			Larry L. Biehl			Date: 02/23/2012	

Boolean ClusterOnePassControl (
				FileInfoPtr							fileInfoPtr)
{
   ClusterType							*currentCluster,
											*lastCluster;

   CMFileStream*						resultsFileStreamPtr;
   FileIOInstructionsPtr			fileIOInstructionsPtr;
   HUCharPtr							classifyBufferPtr;

   SInt32								numberSumSquares;

   SInt16								numClusters;

	Boolean								continueFlag;


			// Initialize variables.

   continueFlag = TRUE;
   gTotalNumberOfClusters = 0;
   fileIOInstructionsPtr = NULL;

   numberSumSquares = (SInt32)gClusterSpecsPtr->numberChannels;

   gSizeOfDoubleVector = 2 * gClusterSpecsPtr->numberChannels + numberSumSquares;
   gClusterHead = NULL;
   resultsFileStreamPtr = GetResultsFileStreamPtr (0);

			// Get memory and initialize some variables to be used by single
			// pass clustering.

   continueFlag = InitializeClusterMemory (&fileIOInstructionsPtr);

   if (continueFlag)
		{
				// Initialize some global variables to allow the user to pause
				// or cancel the classification.

      InitializeGlobalAlertVariables (kContinueStopAlertID,
													  kAlertStrID,
													  IDS_Alert123);

      numClusters = OnePassCluster (fileIOInstructionsPtr);
      continueFlag = (numClusters > 0);

				// Reset the cancel operation globals.

      ClearGlobalAlertVariables ();

		}	// end "if (continueFlag)"

			// Set up the cluster class to final class vector if needed.

   if (continueFlag)
      continueFlag = SetUpClassToFinalClassPtr ();

			// Create a cluster classification map if requested.

   if (gClusterSpecsPtr->classificationArea != 0 && continueFlag)
		{
				// Get the memory for the classification buffer.

      classifyBufferPtr = (HUCharPtr)MNewPointerClear (
							gImageWindowInfoPtr->maxNumberColumns +
											gNumberOfEndOfLineCharacters + 1);
      continueFlag = (classifyBufferPtr != NULL);

      if (continueFlag)
         continueFlag = ClusterClassification (
												  fileIOInstructionsPtr,
												  gClusterSpecsPtr->numberFinalClusters,
												  classifyBufferPtr);

				// Release memory for the classification buffer.

      classifyBufferPtr = CheckAndDisposePtr (classifyBufferPtr);

		}	// end "if (gClusterSpecsPtr->classificationArea != 0 && "

			// Create a cluster mask file if requested.

   if (continueFlag && (gClusterSpecsPtr->outputStorageType & kClusterMaskCode))
      continueFlag = CreateClusterMaskFile ();

			// Save the statistics if requested.										

   if (continueFlag)
      continueFlag = SaveClusterStatistics (
					fileIOInstructionsPtr, gClusterSpecsPtr->numberFinalClusters);

			// Release memory for the pixel cluster class assignments.			

   gClusterSpecsPtr->dataClassPtr = CheckAndDisposePtr (
													gClusterSpecsPtr->dataClassPtr);

			// Free memory that the clusters used.										

   currentCluster = gClusterSpecsPtr->clusterHead;
   while (currentCluster != NULL)
		{
      lastCluster = currentCluster;
      currentCluster = currentCluster->next;
      CheckAndDisposePtr ((char*)lastCluster);

		}	// end "while (currentCluster != NULL)"

   gClusterSpecsPtr->clusterHead = NULL;

			// Dispose of the IO buffer.		

   DisposeIOBufferPointers (fileIOInstructionsPtr,
									  &gInputBufferPtr,
									  &gOutputBufferPtr);

   gClusterSpecsPtr->clusterClassToFinalClassPtr = CheckAndDisposePtr (
												gClusterSpecsPtr->clusterClassToFinalClassPtr);

   return (TRUE);

}	// end Function "ClusterOnePassControl"  	



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetOnePassClusterCenters
//
//	Software purpose:	The purpose of this routine is to determine the
//							clusters for ISODATA clustering using the one-pass
//							cluster algorithm technique.
//
//	Parameters in:		Pointer to image file information structure
//
//	Parameters out:	None
//
// Value Returned:	True if everything worked okay
//							False if a problem came up such as shortage of memory
// 
// Called By:			InitializeClusterCenters in clusterISODATA.c
//
//	Coded By:			Larry L. Biehl			Date: 01/14/1991
//	Revised By:			Larry L. Biehl			Date: 10/25/1999	

Boolean GetOnePassClusterCenters (
				FileIOInstructionsPtr			fileIOInstructionsPtr)
{
   ClusterType*						currentCluster;
   HDoublePtr							doubleVectorPtr;

   SInt32								endIndexCount,
											index,
											numberSumSquares;

   SInt16								numberChannels;

   Boolean								continueFlag;


   gTotalNumberOfClusters = 0;
   numberChannels = gClusterSpecsPtr->numberChannels;

   numberSumSquares = (SInt32)numberChannels;
	//numberSumSquares = (SInt32)numberChannels * (numberChannels+1)/2;

   gSizeOfDoubleVector = 2 * numberChannels + numberSumSquares;
   gClusterHead = NULL;

			// "Determining Initial Clusters (One-Pass)"									

   LoadDItemStringNumber (kClusterStrID,
									  IDS_Cluster23,
									  gStatusDialogPtr,
									  IDC_Status11,
									  (Str255*)gTextString);

			// Create new clusters from first line of data in each cluster area.	

   continueFlag = OnePassClusterAreas (fileIOInstructionsPtr, NULL, 1);

			// Continue clustering using only part 										

   if (continueFlag)
      continueFlag = OnePassClusterAreas (fileIOInstructionsPtr, NULL, 2);

			// Store values in cluster specifications structure.

   gClusterSpecsPtr->clusterHead = gClusterHead;
   gClusterSpecsPtr->numberClusters = gTotalNumberOfClusters;

			// Clear the cluster statistics to be ready for the ISODATA step		
			// of clustering.																		

   if (continueFlag)
		{
      endIndexCount = numberChannels;

      currentCluster = gClusterHead;
      while (currentCluster != NULL)
			{
         currentCluster->numPixels = 0;

         doubleVectorPtr = (double*)currentCluster->sumPtr;
         for (index=0; index<numberChannels; index++)
				{
            *doubleVectorPtr = 0;
            doubleVectorPtr++;

				}	// end "for (index=0; index<numberChannels; index++)"

         doubleVectorPtr = (double*)currentCluster->sumSquarePtr1;
         for (index=0; index<endIndexCount; index++)
				{
            *doubleVectorPtr = 0;
            doubleVectorPtr++;

         	}	// end "for (index=0; index<numberChannels; index++)" 

         currentCluster = currentCluster->next;

      	}	// end "while (currentCluster != NULL)" 

   	}	// end "if (continueFlag)" 

   return (continueFlag);

}	// end "GetOnePassClusterCenters"    



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 OnePassCluster
//
//	Software purpose:	The purpose of this routine is to perform the
//							one pass cluster on the requested set of data.
//
//	Parameters in:		Pointer to image file information structure
//
//	Parameters out:	None
//
// Value Returned:	-1 if user cancels the operation or a problem
//											occurred.
//							The number of clusters found.
// 
// Called By:			ClusterOnePassControl in clusterSinglePass.c
//
//	Coded By:			Eric E. Demaree		Date: Spring 1989
//	Revised By:			Larry L. Biehl			Date: 05/09/2000

SInt16 OnePassCluster (
				FileIOInstructionsPtr			fileIOInstructionsPtr)
{
			// Define local structures and variables.

   CMFileStream*						clResultsFileStreamPtr;
   HUInt16Ptr							dataClassPtr;

   SInt16								minimumClusterSize,
											numberChannels;

   Boolean								continueFlag;


			// Initialize local variables. 													

   continueFlag = TRUE;
   numberChannels = gClusterSpecsPtr->numberChannels;
   minimumClusterSize = gClusterSpecsPtr->minClusterSize;
   clResultsFileStreamPtr = GetResultsFileStreamPtr (0);

			// Create new clusters from first line of data in each cluster area.	

			// "Clustering First Line(s)."													

   LoadDItemStringNumber (kClusterStrID,
								  IDS_Cluster20,
								  gStatusDialogPtr,
								  IDC_Status11,
								  (Str255*)gTextString);
   dataClassPtr = gClusterSpecsPtr->dataClassPtr;
   if (!OnePassClusterAreas (fileIOInstructionsPtr, &dataClassPtr, 1))
																							return (-1);

			// Continue clustering using only part 										
			// (gClusterSpecsPtr->clusterColumnInterval) of image. 					

   LoadDItemStringNumber (kClusterStrID,
								  IDS_Cluster21, // "Clustering Rest of Lines."
								  gStatusDialogPtr,
								  IDC_Status11,
								  (Str255*)gTextString);
   if (!OnePassClusterAreas (fileIOInstructionsPtr, &dataClassPtr, 2))
																							return (-1);

			// Delete those clusters with too few pixels. 								
			// Find first cluster class that is large enough.							

			// "Checking Cluster Sizes."														

   LoadDItemStringNumber (kClusterStrID,
								  IDS_Cluster22,
								  gStatusDialogPtr,
								  IDC_Status11,
								  (Str255*)gTextString);

   gClusterSpecsPtr->numberClusters = gTotalNumberOfClusters;
   gClusterSpecsPtr->clusterHead = gClusterHead;
   gClusterSpecsPtr->numberFinalClusters =
           DeleteSpecifiedClusters (gClusterSpecsPtr->numberClusters,
												minimumClusterSize);

			// Store cluster head in cluster specifications structure.				

   if (gClusterSpecsPtr->numberFinalClusters == 0)
      gClusterSpecsPtr->clusterHead = NULL;

   gClusterHead = gClusterSpecsPtr->clusterHead;

			// List message if all cluster classes are too small.						

   if (gClusterHead == NULL)
		{
   			// "All clusters too small (try increasing critical distance).\r"	

      if (ListSpecifiedStringNumber (kClusterStrID,
												  IDS_Cluster26,
												  (unsigned char*)gTextString,
												  clResultsFileStreamPtr,
												  gOutputForce1Code,
												  continueFlag))
																							return (0);
      else // !ListSpecifiedStringNumber (...
																							return (-1);

   	}	// end "if (gClusterHead == NULL)" 

			// List number of clusters found and final number of clusters used.	

   if (!ListSpecifiedStringNumber (kClusterStrID,
											  IDS_Cluster37,
											  clResultsFileStreamPtr,
											  gOutputForce1Code,
											  (SInt32)gTotalNumberOfClusters,
											  (SInt32)gClusterSpecsPtr->numberFinalClusters,
											  continueFlag))
													return (gClusterSpecsPtr->numberFinalClusters);

   if ((gClusterSpecsPtr->numberFinalClusters > kMaxNumberStatClasses - 1) &&
					(gClusterSpecsPtr->saveStatisticsCode > 0 ||
								gClusterSpecsPtr->classificationArea != 0))
		{
   			// Only keep the the 'maxNumberClusters' that have the largest number
   			// of pixels.

      KeepLargestClusters (kMaxNumberStatClasses - 1);

   			// Make sure that we have the current cluster head.

      gClusterHead = gClusterSpecsPtr->clusterHead;

   			// "Only the largest %ld classes will be used.\r"							

      if (!ListSpecifiedStringNumber (kClusterStrID,
												  IDS_Cluster5,
												  clResultsFileStreamPtr,
												  gOutputForce1Code,
												  (SInt32)(kMaxNumberStatClasses - 1),
												  continueFlag))
												return (gClusterSpecsPtr->numberFinalClusters);

   	}	// end "if (clusterCount >(kMaxNumberStatClasses-1) && ..." 

			// Get the cluster standard deviations.										

   if (!UpdateClusterStdDeviations (gClusterSpecsPtr->clusterHead))
																								return (-1);

			// List final cluster statistics. 												

   if (!ListClusterStatistics (clResultsFileStreamPtr, &gOutputForce1Code, TRUE))
													return (gClusterSpecsPtr->numberFinalClusters);

			// Return number of clusters found. 											

   return (gClusterSpecsPtr->numberFinalClusters);

}	// end Function "OnePassCluster" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean OnePassClusterAreas
//
//	Software purpose:	The purpose of this routine is to determine the
//							clusters in the specified lines of data in each
//							of the cluster areas.  If first line code is =1,
//							then only the first line is clustered (all columns)
//							using critical distance 1.  If first line code =2,
//							then the rest of the lines are clustered.
//
//							Note that the data values are multiplied by 10 so that
//							all calculations for cluster means are done to one 
//							decimal place accuracy and still use integer multiplication 
//							and division.
//
//							Note that code is included so that the distance
//							measure can include the  standard deviation of the
//							cluster class.  So far this option has not worked
//							well.  The code is still included but has been
//							commented out.
//
//	Parameters in:		Pointer to image file information structure
//							Pointer to vector which identifies which cluster
//									class each pixel belongs to.
//							First line code;  if =1 then cluster only the
//								first line, all columns; if =2 then cluster
//								all lines with the specified column and line
//								interval.
//
//	Parameters out:	None
//
// Value Returned:	True if everything worked okay
//							False if a problem came up such as shortage of memory
// 
// Called By:			OnePassCluster in clusterSinglePass.c
//							GetOnePassClusterCenters in clusterSinglePass.c
//
//	Coded By:			Eric E. Demaree		Date: Spring 1989
//	Revised By:			Larry L. Biehl			Date: 05/12/2017	

Boolean OnePassClusterAreas (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				HUInt16Ptr*							dataClassPtrPtr,
				SInt16								firstLineCode) 
{
			// Define local structures and variables.	

	ldiv_t								lDivideStruct;
   Point									point;

   double								criticalDistance,
											currentDistance, // Distance from pix to current cluster.
											closestDistance, // Distance from pix to closest cluster.
											minutesLeft;

   ClusterType							*closestCluster,	// cluster closest to current pixel.
											*currentCluster; // Cluster currently working on.

   HCMeanTypePtr						closestMeanPtr; // Vector holds mean of closest	cluster.

   HCTypePtr							currentPixel, // Vector pointer for current pixel.
											outputBufferPtr; // ptr to all pixels.

   HUInt16Ptr							localDataClassPtr,
											savedDataClassPtr;

   HPtr									offScreenBufferPtr;
   ImageOverlayInfoPtr				imageOverlayInfoPtr;
   Ptr									stringPtr;
   UInt16*								channelsPtr;
   WindowPtr							windowPtr;

   //CMeanType2						closestDistance,	// Distance from pix to closest cluster.
   //										currentDistance;	// Distance from pix to current cluster.
   RgnHandle							rgnHandle;

   SInt32								line,
											lineCount,
											linesDone,
											lineEnd,
											lineInterval,
											linesLeft,
											lineStart,
											numberColumns,
											startTick,
											linuxtick;

   //UInt32				 				criticalDistance1Squared;	// Used for standard deviations only.
   UInt32								column,
											columnEnd,
											columnInterval,
											columnStart,
											columnWidth,
											firstColumn,
											numberSamples,
											sample,
											startSample;

   SInt16								areaNumber,
											channel, // Counting variable for channels
											distanceType = 1,
											errCode,
											fieldNumber,
											lastClassIndex,
											lastFieldIndex,
											numberChannels,
											pointType,
											totalNumberAreas;

   //UInt16								maskRequestValue;

   Boolean								createImageOverlayFlag,
											polygonFieldFlag,
											returnFlag = TRUE;


			// Initialize local variables. 													

   numberChannels = gClusterSpecsPtr->numberChannels;
   totalNumberAreas = gClusterSpecsPtr->totalNumberAreas;
   columnInterval = gClusterSpecsPtr->clusterColumnInterval;
   lineInterval = gClusterSpecsPtr->clusterLineInterval;
   channelsPtr = (UInt16*)GetHandlePointer (gClusterSpecsPtr->channelsHandle);
   outputBufferPtr = (CType*)gOutputBufferPtr;

   criticalDistance = gClusterSpecsPtr->criticalDistance1;

   windowPtr = FindProjectBaseImageWindowPtr ();

   imageOverlayInfoPtr = GetImageOverlayInfoPtr (gClusterSpecsPtr->imageOverlayIndex,
																  kLock,
																  NULL);

   if (gClusterSpecsPtr->clustersFrom == kTrainingType)
      LoadDItemValue (gStatusDialogPtr,
								IDC_Status5,
								(SInt32)gClusterSpecsPtr->totalNumberAreas);

   if (firstLineCode != 1) 
		{
      criticalDistance = gClusterSpecsPtr->criticalDistance2;

      ShowStatusDialogItemSet (kStatusField);
      ShowStatusDialogItemSet (kStatusCluster);

      if (gClusterSpecsPtr->clustersFrom == kTrainingType)
         HideStatusDialogItemSet (kStatusMinutes);

      if (gClusterSpecsPtr->clustersFrom == kAreaType)
         ShowStatusDialogItemSet (kStatusMinutes);

		}	// end "if (firstLineCode != 1)" 

   if (distanceType == 2)
      criticalDistance *= criticalDistance;

			//	Used for standard deviation type only.													
   //
   //criticalDistance1Squared = gClusterSpecsPtr->criticalDistance1 *
   //   													gClusterSpecsPtr->criticalDistance1;
   //criticalDistance1Squared *= criticalDistance;

   createImageOverlayFlag = FALSE;
   localDataClassPtr = NULL;
   if (dataClassPtrPtr != NULL) 
		{
      localDataClassPtr = *dataClassPtrPtr;

      if (localDataClassPtr != NULL)
         createImageOverlayFlag = (gOutputCode & kCreateImageOverlayCode);

		}	// end "if (dataClassPtrPtr != NULL)"

   fieldNumber = 0;
   lastClassIndex = -1;
   lastFieldIndex = -1;

			// Intialize the nextTime variable to indicate when the next check	
			// should occur for a command-.													

   gNextTime = TickCount ();
   linuxtick = 0;

			// Save the pointer to the cluster class assignments in case it is needed
			// for the offscreen buffer assignments.

   savedDataClassPtr = localDataClassPtr;

   offScreenBufferPtr = GetImageOverlayOffscreenPointer (imageOverlayInfoPtr);

			// Loop by number of cluster areas.												

   for (areaNumber=1; areaNumber<=totalNumberAreas; areaNumber++)
		{
				// Initialize status variables.												

      lineCount = 0;
      linesDone = 0;

				// Get information for next cluster area.									

      if (!GetNextClusterArea (gProjectInfoPtr,
										  channelsPtr,
										  numberChannels,
										  areaNumber,
										  &gNextMinutesLeftTime,
										  &lastClassIndex,
										  &lastFieldIndex,
										  &linesLeft))
																								return (FALSE);

				// Initialize local image area variables.

      lineStart = gAreaDescription.lineStart;
      lineEnd = gAreaDescription.lineEnd;
      columnStart = gAreaDescription.columnStart;
      columnEnd = gAreaDescription.columnEnd;
      columnWidth = columnEnd - columnStart + 1;
      polygonFieldFlag = gAreaDescription.polygonFieldFlag;
      rgnHandle = gAreaDescription.rgnHandle;
      pointType = gAreaDescription.pointType;

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

      numberColumns = (columnEnd - columnStart + columnInterval) / columnInterval;

   			// Set up status and check time variables.

      gNextStatusTime = TickCount ();

      startTick = TickCount ();
      gNextMinutesLeftTime = startTick + 3 * gNextStatusTimeOffset;

   			// Load some of the File IO Instructions structure that pertain
   			// to the specific area being used.

      errCode = SetUpFileIOInstructions (fileIOInstructionsPtr,
													  &gAreaDescription,
													  numberChannels,
													  channelsPtr,
													  kDetermineSpecialBILFlag);

   			// If lineEnd is 0, then this implies that there are no requested mask
   			// values in the area being used. Force looping through the lines to be
   			// skipped.

      if (lineEnd == 0)
         lineStart = 1;

				// Loop by rest of lines for cluster area.								

      for (line=lineStart; line<=lineEnd; line+=lineInterval)
			{
      			// Update dialog status information if needed.						

         lineCount++;
         if (TickCount () >= gNextStatusTime) 
				{
            LoadDItemValue (gStatusDialogPtr,
										IDC_Status8,
										(SInt32)lineCount);
            LoadDItemValue (gStatusDialogPtr,
										IDC_Status16,
										(SInt32)gTotalNumberOfClusters);
            gNextStatusTime = TickCount () + gNextStatusTimeOffset;

				}	// end "if (TickCount () >= gNextStatusTime)"

					// Exit routine if user has "command period" down					
			#ifndef multispec_lin
				if (TickCount () >= gNextTime) 
			#else
				if (TickCount () > gNextTime + linuxtick) 
			#endif
					{
					if (gOutputCode & kCreateImageOverlayCode && line > lineStart) 
						{
						InvalidateWindow (windowPtr, kImageArea, FALSE);

						#if defined multispec_win  
							windowPtr->UpdateWindow ();
						#endif	// defined multispec_win  

						#if defined multispec_lin
							linuxtick++;
							gActiveImageWindow->OnUpdate (NULL, NULL);
							(gActiveImageWindow->m_Canvas)->Update ();
						#endif

						}	// end "if (gOutputCode & kCreateImageOverlayCode && ..."

			if (!CheckSomeEvents (osMask + keyDownMask + updateMask + mDownMask + mUpMask))
				{
				returnFlag = FALSE;
				break;

				}	// end "if (!CheckSomeEvents (..."

			}	// end "if (TickCount () >= nextTime)"

					// Set vertical (line) point in case it is needed for polygonal areas.	

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
            returnFlag = FALSE;
            break;

				}	// end "if (errCode < noErr)"

         if (errCode != kSkipLine)
				{
            currentPixel = outputBufferPtr;

            startSample = 1;

            if (pointType == kMaskType)
               numberSamples = fileIOInstructionsPtr->numberOutputBufferSamples;

            else // pointType != kMaskType
               numberSamples = (columnEnd - firstColumn + columnInterval) / columnInterval;

            column = firstColumn;

         			// Make certain that we have a valid clusterhead alocated.		

            while (gClusterHead == NULL)
					{
               point.h = (SInt16)column;

               if (!polygonFieldFlag || PtInRgn (point, rgnHandle))
						{
                  AddNewCluster (currentPixel, gClusterHead);

                  if (localDataClassPtr != NULL)
							{
                     *localDataClassPtr = gClusterHead->clusterNumber;
                     localDataClassPtr++;

                  	}	// end "if (localDataClassPtr != NULL)"

               	}	// end "if (includePixelFlag)" 

               currentPixel += numberChannels;

               column += columnInterval;
               startSample++;

               if (startSample > numberSamples)
                  break;

            	}	// end "while (gClusterHead == NULL)" 

            for (sample=startSample; sample<=numberSamples; sample++)
					{
               point.h = (SInt16)column;

               if (!polygonFieldFlag || PtInRgn (point, rgnHandle))
						{
                  closestCluster = gClusterHead;
                  currentCluster = gClusterHead->next;
                  Distance (closestCluster, currentPixel, closestDistance);

               			// Find closest cluster in absolute sense (Euclidean 		
               			// distance). 															

                  while (currentCluster != NULL)
							{
                     Distance (currentCluster, currentPixel, currentDistance);

                     if (currentDistance < closestDistance)
								{
                        closestCluster = currentCluster;
                        closestDistance = currentDistance;

                     	}	// end "if (currentDistance < closestDistance)" 

                     currentCluster = currentCluster->next;

                  	}	// end " while (currentCluster != NULL)" 

               			// Make sure pixel is within critical distance from the 	
               			// chosen cluster. 													
               			// Two different distance measures are supported here.	
               			//		distance1 is just linear distance.						
               			//		!distance1 is the number of standard deviations.	

                  //if (distanceType == 1)
                  //	{
                  closestMeanPtr = closestCluster->meanPtr;
                  for (channel = 0; channel < numberChannels; channel++)
							{
                     //if (labs (*(closestMeanPtr+channel) -
                     //			10 * (SInt32)*(currentPixel+channel)) >
                     //				      								criticalDistance)
                     if (fabs (*(closestMeanPtr + channel) -
												*(currentPixel + channel)) > criticalDistance)
								{
                        closestCluster = NULL;
                        break;

                     	}	// end "if (labs (*(closestMean+channel) - ..."

                  	}	// end "for (channel=0; channel<...)" 

                  //}		// end "if (distanceType == 1)"

                  //else	// distanceType != 1
							/*
							{
									// First make sure that the variances for the closest
									// cluster are up-to-date.
					   	   		
							if (!closestCluster->varianceComputed)
								{
								closestVariancePtr = closestCluster->variancePtr;
			
										// This will handle the case when there is only
										// one pixel in the class.	 We will revert back
										// to the distance1 measure used for the first
										// line of pixels.									
					
								if (closestCluster->numPixels <= 1)
									for (channel=0; channel<numberChannels; channel++)
										{
										*closestVariancePtr =  criticalDistance1Squared;
										closestVariancePtr++;
						      		
										}	// end "for (channel = 0; ..."
						      		
								else	// closestCluster->numPixels > 1
									{
									closestSumPtr = closestCluster->sumPtr;
									closestSumSquarePtr = closestCluster->sumSquarePtr1;
									numberPixels = closestCluster->numPixels;
									numberPixelsLessOne = numberPixels - 1;
									for (channel=0; channel<numberChannels; channel++)
										{
										*closestVariancePtr = (*closestSumSquarePtr -
												*closestSumPtr * *closestSumPtr/numberPixels)/
																						numberPixelsLessOne;
										*closestVariancePtr *= criticalDistance;
										if (*closestVariancePtr == 0)
											*closestVariancePtr = criticalDistance1Squared;
										closestVariancePtr++;
										closestSumSquarePtr++;
										closestSumPtr++;
				
										}	// end "for (channel = 0; channel < ..."
					 
									closestCluster->varianceComputed = TRUE;
								
									}	// end "else closestCluster->numPixels > 1"
					      	
								}	// end "if (!closestCluster->varianceComputed)"
					      	
							closestMeanPtr = closestCluster->meanPtr;
							closestVariancePtr = closestCluster->variancePtr;
							for (channel = 0; channel < numberChannels; channel++)
								{
								distance = 
								*closestMeanPtr - 10 * *(currentPixel+channel);
								if (distance * distance > 100 * *closestVariancePtr)
									{
									closestCluster = NULL;
									break;
				
									}	// end "if (distance * distance > ..."
			 
								closestMeanPtr++;
								closestVariancePtr++;
				
								}	// end "for (channel=0; channel<...)"
			
							}	// end "else distanceType != 1"
							*/
                  if (closestCluster == NULL)
							{
                     AddNewCluster (currentPixel, gClusterHead);
                     closestCluster = gClusterHead;
							}
						
						else
							{
                     UpdateClusterMean (currentPixel, closestCluster);
							}

                  if (localDataClassPtr)
							{
                     *localDataClassPtr = closestCluster->clusterNumber;
                     localDataClassPtr++;

                  	}	// end "if (localDataClassPtr)"

               	}	// end "if (includePixelFlag)" 

               currentPixel += numberChannels;

               column += columnInterval;

            	}	// end "for (sample=startSample; sample<=numberSamples; sample++)" 

            if (createImageOverlayFlag) 
					{
               CopyToOffscreenBuffer (fileIOInstructionsPtr,
													imageOverlayInfoPtr,
													gClusterSpecsPtr->imageOverlayIndex,
													FindProjectBaseImageWindowInfoHandle (),
													line,
													firstColumn,
													columnInterval,
													numberSamples,
													lineStart,
													rgnHandle,
													(HUCharPtr)savedDataClassPtr,
													offScreenBufferPtr,
													1,
													TRUE);

               savedDataClassPtr = localDataClassPtr;

					}	// end "if (createImageOverlayFlag)"

            if (pointType != kMaskType) 
					{
            			// Update firstColumn so that columnInterval samples are			
            			// skipped from the end of one line to the beginning of the 	
            			// next line.  This will allow the user to use samples from		
            			// all columns if a column interval other than one is used.		

               firstColumn = columnStart + column - columnEnd - 1;
               if (firstColumn > columnEnd)
						{
                  lDivideStruct = ldiv (firstColumn - columnStart, columnWidth);
                  firstColumn = columnStart + lDivideStruct.rem;

               	}	// end "if (firstColumn > columnEnd)"

            	}	// end "if (pointType != kMaskType)"

         	}	// end "if (errCode != kSkipLine)"

         linesDone++;
         linesLeft--;
         if (TickCount () >= gNextMinutesLeftTime)
				{
            minutesLeft = (linesLeft * (TickCount () - startTick)) /
														(double)(lineCount * kTicksPerMinute);

            sprintf ((char*)gTextString, " %.1f", minutesLeft);
            stringPtr = (char*)CtoPstring (gTextString, gTextString);
            LoadDItemString (gStatusDialogPtr, IDC_Status14, (Str255*)gTextString);

            gNextMinutesLeftTime = TickCount () + gNextMinutesLeftTimeOffset;

         	}	// end "if (TickCount () >= gNextMinutesLeftTime)" 

         if (pointType == kMaskType)
            fileIOInstructionsPtr->maskBufferPtr +=
										fileIOInstructionsPtr->numberMaskColumnsPerLine;

      	}	// end "for (line=lineStart; line<=lineEnd; ...)" 

   			// Force overlay to be drawn if it has not been already.

      if (createImageOverlayFlag)
			{
         InvalidateWindow (windowPtr, kImageArea, FALSE);

			#if defined multispec_win
				windowPtr->UpdateWindow ();
			#endif	// defined multispec_win

			#if defined multispec_lin
				gActiveImageWindow->OnUpdate (NULL, NULL);
				(gActiveImageWindow->m_Canvas)->Update ();
			#endif

         if (!CheckSomeEvents (
									osMask + keyDownMask + updateMask + mDownMask + mUpMask))
				{
            returnFlag = FALSE;
            break;

         	}	// end "if (!CheckSomeEvents (..."

      	}	// end "if (createImageOverlayFlag)"

      if (returnFlag)
			{
         LoadDItemValue (gStatusDialogPtr, IDC_Status8, lineCount);
         LoadDItemValue (gStatusDialogPtr,
									IDC_Status16,
									(SInt32)gTotalNumberOfClusters);

      	}	// end "if (returnFlag)"

   			// Close up any File IO Instructions structure that pertain to the 
   			// specific area used for the list data.

      CloseUpFileIOInstructions (fileIOInstructionsPtr, &gAreaDescription);

   			// Dispose of the region if it exists.						

      CloseUpAreaDescription (&gAreaDescription);

      if (!returnFlag)
         break;

   	}	// end "for (areaNumber=1; areaNumber<=totolNumberAreas; ...)" 

   UnlockImageOverlayOffscreenBuffer (imageOverlayInfoPtr);

   if (!gOSXCoreGraphicsFlag) 
		{
      Handle activeImageWindowInfoHandle = FindProjectBaseImageWindowInfoHandle ();
      WindowInfoPtr imageWindowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																		activeImageWindowInfoHandle);
      if (imageWindowInfoPtr != NULL)
         imageWindowInfoPtr->drawBaseImageFlag = TRUE;

   	}	// end "if (!gOSXCoreGraphicsFlag)"

   if (dataClassPtrPtr != NULL)
      *dataClassPtrPtr = localDataClassPtr;

   return (returnFlag);

}	// end "OnePassClusterAreas" 	



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean OnePassClusterDialog
//
//	Software purpose:	The purpose of this routine is to handle the 		
//							modal dialog for selecting the one pass cluster
//							specifications. 
//
//	Parameters in:		Pointer to image file information structure
//
//	Parameters out:	None
//
// Value Returned:	True if user selected OK in dialog box.
//							False if user selected cancel or memory was short.
//
// Called By:			ClusterDialog   in SCluster.cpp
//
//	Coded By:			Larry L. Biehl			Date: 09/15/1989
//	Revised By:			Larry L. Biehl			Date: 12/16/2016	

Boolean OnePassClusterDialog (
				FileInfoPtr							fileInfoPtr,
				DialogPtr							parentDialogPtr) 
{
   Boolean								returnFlag;

	#if defined multispec_mac
		DialogSelectArea					dialogSelectArea;

		DialogPtr							dialogPtr;

		double								criticalDistance1,
												criticalDistance2;

		UInt16*								localClassAreaPtr = NULL;

		Handle								okHandle,
												theHandle;

		Rect									theBox;

		SInt32								minClusterSize,
												theNum;

		UInt32								localNumberClassAreas;

		SInt16								clustersFrom,
												itemHit,
												theType;

		UInt16								distanceDecimalDigits;

		Boolean								modalDone;


				// Initialize local variables.													

		dialogPtr = NULL;

		returnFlag = GetDialogLocalVectors (NULL,
														  NULL,
														  NULL,
														  &localClassAreaPtr,
														  NULL,
														  NULL,
														  NULL,
														  NULL);

				// Get the modal dialog for the one pass cluster specification.		

		if (returnFlag)
			dialogPtr = LoadRequestedDialog (
								kOnePassClusterSpecificationID, kCopyScrap, 1, 2);

		if (dialogPtr == NULL)
			{
			ReleaseDialogLocalVectors (NULL,
												  NULL,
												  NULL,
												  localClassAreaPtr,
												  NULL,
												  NULL,
												  NULL,
												  NULL);
																							return (FALSE);

			}	// end "if (dialogPtr == NULL)" 

				// Set Procedure pointers for those dialog items that need them.								

		SetDialogItemDrawRoutine (dialogPtr, 26, gHiliteOKButtonPtr);

		SetDialogItemDrawRoutine (dialogPtr, 28, gDrawDialogClassPopUpPtr);

		OnePassClusterDialogInitialize (dialogPtr,
												  localClassAreaPtr,
												  &minClusterSize,
												  &criticalDistance1,
												  &criticalDistance2,
												  &distanceDecimalDigits,
												  &clustersFrom,
												  &gClassSelection,
												  &localNumberClassAreas,
												  &dialogSelectArea);

				// Save handle for the OK button for use later.								

		GetDialogItem (dialogPtr, 1, &theType, &okHandle, &theBox);

				//	Load default Minimum Cluster Size.											

		LoadDItemValue (dialogPtr, 6, minClusterSize);

				//	Load default Critical Distance 1.											

		LoadDItemRealValue (dialogPtr, 8, criticalDistance1, distanceDecimalDigits);

				//	Load default Critical Distance 2.											

		LoadDItemRealValue (dialogPtr, 10, criticalDistance2, distanceDecimalDigits);

				// Training areas.																	

		SetDLogControl (dialogPtr, 12, clustersFrom == kTrainingType);

				// Selected Image area.																

		SetDLogControl (dialogPtr, 13, clustersFrom == kAreaType);

				// Center the dialog and then show it.											

		ShowDialogWindow (
				  dialogPtr, kOnePassClusterSpecificationID, kSetUpDFilterTable);

		gDialogItemDescriptorPtr[8] = kDItemReal;
		gDialogItemDescriptorPtr[10] = kDItemReal;

				// Set default text selection to first edit text item						

		SelectDialogItemText (dialogPtr, 6, 0, INT16_MAX);

		modalDone = FALSE;
		itemHit = 0;
		do {
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
					case 6: // Minimum cluster size
						//if (theNum == 0 || theNum > LONG_MAX)
						//	NumberErrorAlert ((SInt32)gClusterSpecsPtr->minClusterSize,
						//						 		dialogPtr,
						//						 		itemHit);
						break;

					case 8: // Critical distance 1
						//if (theNum == 0 || theNum > LONG_MAX)
						//	NumberErrorAlert (gClusterSpecsPtr->criticalDistance1,
						//						 		dialogPtr,
						//						 		itemHit);
						break;

					case 10: // Critical distance 2
						//if (theNum == 0 || theNum > LONG_MAX)
						//	NumberErrorAlert (gClusterSpecsPtr->criticalDistance2,
						//						 		dialogPtr,
						//						 		itemHit);
						break;

					case 12: // Clusters from training fields
						clustersFrom = kTrainingType;
						SetControlValue ((ControlHandle)theHandle, 1);
						SetDLogControl (dialogPtr, 13, 0);

						OnePassClusterDialogOnTrainingAreas (dialogPtr);
						break;

					case 13: // Clusters from selected image area.
						clustersFrom = kAreaType;
						SetControlValue ((ControlHandle)theHandle, 1);
						SetDLogControl (dialogPtr, 12, 0);

						OnePassClusterDialogOnImageArea (dialogPtr);
						break;

					case 19: //	 cluster from lineStart  
					case 20: //	 cluster from lineEnd  
					case 21: //	 cluster from lineInterval  
					case 22: //	 cluster from columnStart  
					case 23: //	 cluster from columnEnd  
					case 24: //	 cluster from columnInterval  		
					case 25: // Entire area to selected area switch.	
					case 26: // Entire area to selected area switch.
						DialogLineColumnHits (&dialogSelectArea,
													  dialogPtr,
													  itemHit,
													  theHandle,
													  theNum);
						break;

					case 28: // Classes 
						itemHit = StandardPopUpMenu (dialogPtr,
															  27,
															  28,
															  gPopUpAllSubsetMenu,
															  gClassSelection,
															  kPopUpAllSubsetMenuID);

						if (itemHit == kSubsetMenuItem)
							{
									// Subset of classes to be used.								

							itemHit = ClassDialog (
												  &localNumberClassAreas,
												  (SInt16*)localClassAreaPtr,
												  1,
												  gProjectInfoPtr->numberStatisticsClasses,
												  gClassSelection,
												  okHandle);

							}	// end "if (itemHit == kSubsetMenuItem)" 

						if (itemHit != 0)
							gClassSelection = itemHit;

								// Make certain that the correct label is drawn in the	
								// class pop up box.													

						InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
						break;

					}	// end "switch (itemHit)" 

				}	// end "if (itemHit > 2)"

			else if (itemHit > 0) // and itemHit <= 2 
				{
						// Check minimum cluster size

				if (itemHit == 1)
					itemHit = CheckMaxValue (dialogPtr,
													  6,
													  1,
													  LONG_MAX,
													  kDisplayRangeAlert);

						// Check critical distance 1

				if (itemHit == 1)
					itemHit = CheckMaxRealValue (dialogPtr,
														  8,
														  ULONG_MAX,
														  kZeroNotAllowed,
														  kDisplayRangeAlert);

						// Check critical distance 2

				if (itemHit == 1)
					itemHit = CheckMaxRealValue (dialogPtr,
														  10,
														  ULONG_MAX,
														  kZeroNotAllowed,
														  kDisplayRangeAlert);

						// If item hit is 1, check if cluster from area line-column 	
						// values make sense.  If they don't, sound an alert and make 	
						// item  hit equal to 0 to allow user to make changes.			

				if (itemHit == 1 && clustersFrom == kAreaType)
					itemHit = CheckLineColValues (
													  &dialogSelectArea,
													  gClusterSpecsPtr->clusterLineStart,
													  gClusterSpecsPtr->clusterColumnStart,
													  dialogPtr);

				if (itemHit == 1) // User selected OK for information 
					{
					modalDone = TRUE;
					returnFlag = TRUE;

					OnePassClusterDialogOK (GetDItemValue (dialogPtr, 6),
													  GetDItemRealValue (dialogPtr, 8),
													  GetDItemRealValue (dialogPtr, 10),
													  clustersFrom,
													  gClassSelection,
													  localClassAreaPtr,
													  localNumberClassAreas,
													  &dialogSelectArea);

					}	// end "if (itemHit == 1)" 

				if (itemHit == 2) // User selected Cancel for information 
					{
					modalDone = TRUE;
					returnFlag = FALSE;
					}

				}	// end "else if (itemHit > 0) and itemHit <= 2" 

			} while (!modalDone);

		ReleaseDialogLocalVectors (NULL,
											  NULL,
											  NULL,
											  localClassAreaPtr,
											  NULL,
											  NULL,
											  NULL,
											  NULL);

		CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
	#endif	// defined multispec_mac

	#if defined multispec_win
		CMSinglePassClusterDialog* dialogPtr = NULL;

		TRY
			{
			dialogPtr = new CMSinglePassClusterDialog ();

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

	#if defined multispec_lin
		CMSinglePassClusterDialog* dialogPtr = NULL;

		//dialogPtr = new CMSinglePassClusterDialog (wxTheApp->GetTopWindow ());
		dialogPtr = new CMSinglePassClusterDialog (parentDialogPtr);

		returnFlag = dialogPtr->DoDialog ();

		delete dialogPtr;
	#endif	// defined multispec_lin

   return (returnFlag);

}	// end "OnePassClusterDialog"    



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void OnePassClusterDialogInitialize
//
//	Software purpose:	The purpose of this routine is to handle the initialization ot
//							parameters for the ISODATA dialog box. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			ClusterDialog   in cluster.c
//
//	Coded By:			Larry L. Biehl			Date: 03/23/1999
//	Revised By:			Larry L. Biehl			Date: 09/05/2017	

void OnePassClusterDialogInitialize (
				DialogPtr							dialogPtr,
				UInt16*								localClassAreaPtr,
				SInt32*								minClusterSizePtr,
				double*								criticalDistance1Ptr,
				double*								criticalDistance2Ptr,
				UInt16*								distanceDecimalDigitsPtr,
				SInt16*								clustersFromPtr,
				SInt16*								classAreaSelectionPtr,
				UInt32*								numberClassAreasPtr,
				DialogSelectArea*					dialogSelectAreaPtr)
{
   SInt16								fieldsExistCode;


			// Load the dialog local vectors

   LoadDialogLocalVectors (NULL,
									  NULL,
									  NULL,
									  NULL,
									  NULL,
									  NULL,
									  NULL,
									  localClassAreaPtr,
									  gClusterSpecsPtr->clusterClassHandle,
									  gClusterSpecsPtr->numberClusterClasses,
									  NULL,
									  NULL,
									  NULL,
									  NULL,
									  NULL,
									  0,
									  NULL);

			//	Load default Minimum Cluster Size.											

   *minClusterSizePtr = gClusterSpecsPtr->minClusterSize;

			//	Load default Critical Distances 1 & 2.										

   *criticalDistance1Ptr = gClusterSpecsPtr->criticalDistance1;
   *criticalDistance2Ptr = gClusterSpecsPtr->criticalDistance2;

			// Get the number of decimal digits to use for the distance measure.

   *distanceDecimalDigitsPtr = 1;
   if (MIN (GetNumberWholeDigits (gClusterSpecsPtr->criticalDistance1),
				GetNumberWholeDigits (gClusterSpecsPtr->criticalDistance2)) == 0)
      *distanceDecimalDigitsPtr = MAX (
           GetNumberLeadingDecimalZeros (gClusterSpecsPtr->criticalDistance1),
           GetNumberLeadingDecimalZeros (gClusterSpecsPtr->criticalDistance2));

			// Training areas.																	

   *clustersFromPtr = gClusterSpecsPtr->clustersFrom;

   SInt16 lastField = -1;
   fieldsExistCode = GetNextFieldArea (gProjectInfoPtr,
													  NULL,
													  gProjectInfoPtr->numberStatisticsClasses,
													  &lastField,
													  -1,
													  kTrainingType,
													  kDontIncludeClusterFields);

   if (fieldsExistCode == -1)
		{
      SetDLogControlHilite (dialogPtr, IDC_ClusterTrainingAreas, 255);
      if (gClusterSpecsPtr->clustersFrom == kTrainingType)
         *clustersFromPtr = kAreaType;

   	}	// end "if (fieldsExistCode == -1)" 

			// Classes to use.											
			//	Set routine to draw the class popup box.								
			//	Make all classes the default												

   if (fieldsExistCode >= 0)
		{
      *classAreaSelectionPtr = gClusterSpecsPtr->clusterClassSet;
      *numberClassAreasPtr = gClusterSpecsPtr->numberClusterClasses;

   	}	// end "if (fieldsExistCode >= 0)"

			// Initialize selected area structure.	

   SInt16 entireIconItem = 25;
   if (gAppearanceManagerFlag)
      entireIconItem = 26;

   InitializeDialogSelectArea (dialogSelectAreaPtr,
										  gImageWindowInfoPtr,
										  gProjectSelectionWindow,
										  gClusterSpecsPtr->clusterColumnStart,
										  gClusterSpecsPtr->clusterColumnEnd,
										  gClusterSpecsPtr->clusterColumnInterval,
										  gClusterSpecsPtr->clusterLineStart,
										  gClusterSpecsPtr->clusterLineEnd,
										  gClusterSpecsPtr->clusterLineInterval,
										  19,
										  entireIconItem,
										  kAdjustToBaseImage);

			// To entire image icon.															
			//	Cluster selected area	

	#if defined multispec_mac
		LoadLineColumnItems (dialogSelectAreaPtr, 
									dialogPtr, 
									kInitializeLineColumnValues, 
									kIntervalEditBoxesExist,
									1);

		if (gAppearanceManagerFlag)
			HideDialogItem (dialogPtr, 25);
		else // !gAppearanceManagerFlag
			HideDialogItem (dialogPtr, 26);
	#endif	// defined multispec_mac

	#if defined multispec_win
		LoadLineColumnItems (dialogSelectAreaPtr,
									dialogPtr, 
									kDoNotInitializeLineColumnValues, 
									kIntervalEditBoxesExist,
									1);
	#endif	// defined multispec_win

			// Hide/Show dialog items relative to training areas or selected area.												

   if (*clustersFromPtr == kTrainingType)
		{
   			// If clusters from training areas, then only allow line and 		
   			// column intervals to show for definition.								

      OnePassClusterDialogOnTrainingAreas (dialogPtr);

		}	// end "if (*clustersFromPtr == ..."

   else // *clustersFromPtr != kTrainingType 
		{
   			// If clusters from selection area, then only allow line and 		
   			// column start, end and intervals to show for definition.			

      OnePassClusterDialogOnImageArea (dialogPtr);

   	}	// end "else *clustersFromPtr != ..." 

}	// end "OnePassClusterDialogInitialize"   



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void OnePassClusterDialogOK
//
//	Software purpose:	The purpose of this routine is to handle the initialization ot
//							parameters for the ISODATA dialog box. 
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			OnePassClusterDialog   in cluster.c
//
//	Coded By:			Larry L. Biehl			Date: 03/25/1999
//	Revised By:			Larry L. Biehl			Date: 12/22/2005	

void OnePassClusterDialogOK (
				SInt16								minClusterSize,
				double								criticalDistance1,
				double								criticalDistance2,
				SInt16								clustersFrom,
				SInt16								classAreaSelection,
				UInt16*								localClassAreaPtr,
				UInt32								localNumberClassAreas,
				DialogSelectArea*					dialogSelectAreaPtr)
{
   SInt16*								classAreaPtr;

   UInt32								index;


			// Minimum cluster size.									

   gClusterSpecsPtr->minClusterSize = minClusterSize;

			// Items 8; Minimum cluster size.									

   gClusterSpecsPtr->criticalDistance1 = criticalDistance1;

			// Items 10; Minimum cluster size.									

   gClusterSpecsPtr->criticalDistance2 = criticalDistance2;

			// Clusters from definitions.						

   gClusterSpecsPtr->clustersFrom = clustersFrom;

			// Cluster class areas.											

   gClusterSpecsPtr->clusterClassSet = classAreaSelection;
   if (gClusterSpecsPtr->clustersFrom == kTrainingType)
		{
      classAreaPtr = (SInt16*)GetHandlePointer (
												gClusterSpecsPtr->clusterClassHandle);

      if (classAreaSelection == kAllMenuItem) // All classes 
         LoadClassAreaVector (&gClusterSpecsPtr->numberClusterClasses,
										classAreaPtr);

      else // classAreaSelection == kSubsetMenuItem
			{
         gClusterSpecsPtr->numberClusterClasses = localNumberClassAreas;
         for (index=0; index<localNumberClassAreas; index++)
            classAreaPtr[index] = (SInt16)localClassAreaPtr[index];

      	}	// end "else classAreaSelection == kSubsetMenuItem" 

   	}	// end "if (gClusterSpecsPtr->clustersFrom == kTrainingType)"

			// Cluster selected area.

   gClusterSpecsPtr->clusterLineStart = dialogSelectAreaPtr->lineStart;
   gClusterSpecsPtr->clusterLineEnd = dialogSelectAreaPtr->lineEnd;
   gClusterSpecsPtr->clusterLineInterval = dialogSelectAreaPtr->lineInterval;

   gClusterSpecsPtr->clusterColumnStart = dialogSelectAreaPtr->columnStart;
   gClusterSpecsPtr->clusterColumnEnd = dialogSelectAreaPtr->columnEnd;
   gClusterSpecsPtr->clusterColumnInterval = dialogSelectAreaPtr->columnInterval;

}	// end "OnePassClusterDialogOK"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void OnePassClusterDialogOnImageArea
//
//	Software purpose:	The purpose of this routine is to handle the changes in
//							the dialog box due to a click in the cluster area radio
//							button. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			ClusterDialog   in cluster.c
//
//	Coded By:			Larry L. Biehl			Date: 03/25/1999
//	Revised By:			Larry L. Biehl			Date: 03/25/1999	

void OnePassClusterDialogOnImageArea (
				DialogPtr							dialogPtr)
{
   HideDialogItem (dialogPtr, IDC_ClassPrompt);
   HideDialogItem (dialogPtr, IDC_ClassCombo);

   ShowDialogItem (dialogPtr, IDC_LineStart);
   ShowDialogItem (dialogPtr, IDC_LineEnd);
   ShowDialogItem (dialogPtr, IDC_ColumnStart);
   ShowDialogItem (dialogPtr, IDC_ColumnEnd);


	#if defined multispec_mac
		ShowDialogItem (dialogPtr, 14);
		ShowDialogItem (dialogPtr, 15);

		if (gAppearanceManagerFlag)
			ShowDialogItem (dialogPtr, 26);
		else // !gAppearanceManagerFlag
			ShowDialogItem (dialogPtr, 25);
	#endif	// defined multispec_mac

}	// end "OnePassClusterDialogOnImageArea" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void OnePassClusterDialogOnTrainingAreas
//
//	Software purpose:	The purpose of this routine is to handle the changes in
//							the dialog box due to a click in the cluster training
//							areas radio button. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			ClusterDialog   in cluster.c
//
//	Coded By:			Larry L. Biehl			Date: 03/25/1999
//	Revised By:			Larry L. Biehl			Date: 03/25/1999	

void OnePassClusterDialogOnTrainingAreas (
				DialogPtr							dialogPtr)
{
   ShowDialogItem (dialogPtr, IDC_ClassPrompt);
   ShowDialogItem (dialogPtr, IDC_ClassCombo);

   HideDialogItem (dialogPtr, IDC_LineStart);
   HideDialogItem (dialogPtr, IDC_LineEnd);
   HideDialogItem (dialogPtr, IDC_ColumnStart);
   HideDialogItem (dialogPtr, IDC_ColumnEnd);

	#if defined multispec_mac
		HideDialogItem (dialogPtr, 14);
		HideDialogItem (dialogPtr, 15);
		HideDialogItem (dialogPtr, 25);
		HideDialogItem (dialogPtr, 26);
	#endif	// defined multispec_mac

	#if defined multispec_win
		HideDialogItem (dialogPtr, IDEntireImage);
		HideDialogItem (dialogPtr, IDSelectedImage);
	#endif	// defined multispec_win

}	// end "OnePassClusterDialogOnTrainingAreas" 
