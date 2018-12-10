//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SClusterIsodata.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			05/21/2018
//
//	Language:				C
//
//	System:					Linux, Macintosh and Windows Operating Systems
//
//	Functions in file:	Boolean 				GetCentersFromEigenvectorVolume
//								Boolean 				GetClassMeanClusterCenters
//								Boolean				GetEigenvectorClusterCenters
//								ClusterType* 		GetMemoryForClusters
//								Boolean 				InitializeClusterCenters
//								short int 			ISODATACluster
//								void 					ISODATAClusterControl
//								Boolean 				ISODATAClusterDialog
//								short int 			ISODATAClusterPass
//								Boolean		 		UpdateClusterMeans
//
//	Brief description:	This file contains routines that will cluster image data
//								using the ISODATA algorithm that was implemented in the
//								LARSYS routines.  The routines in the file were adapted
//								from the LARSYS fortran routines, Byungyoung Kim C routines
//								and Terry Parr C routines (written for Prof. Phil Swain's
//								EE577 class in the spring of 1989).
//
//	Diagram of MultiSpec routine calls for the routines in the file.
//
//		ISODATAClusterDialog
//			ProcessorDialogFilter
//
//		ISODATAClusterControl
//			InitializeClusterMemory (in cluster.c)
//
//			InitializeClusterCenters
//				GetMemoryForClusters
//
//				GetEigenvectorClusterCenters
//					GetTotalSumSquares (in matrixUtilities.c)
//					ComputeCovarianceMatrix (in matrixUtilities.c)
//					ComputeCorrelationCoefficientMatrix (in matrixUtilities.c)
//					ComputeEigenvectors (in matrixUtilities.c)
//					GetCentersFromEigenvectorVolume
//
//				GetOnePassClusterCenters (in clusterSinglePass.c)
//
//			ListClusterStatistics (in cluster.c)
//
//			ISODATACluster
//				ISODATAClusterPass
//				UpdateClusterMeans
//
//			UpdateClusterStdDeviations (in cluster.c)
//
//			ListClusterStatistics (in cluster.c)
//
//			SetUpClassToFinalClassPtr (in cluster.c)
//
//			ClusterClassification (in cluster.c)
//
//			SaveClusterStatistics (in cluster.c)
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
//------------------------------------------------------------------------------------

#include	"SMultiSpec.h"     

#if defined multispec_lin  
	#include "LClusterIsodataDialog.h"  
	#include "LImageView.h"
#endif	// defined multispec_lin 

#if defined multispec_mac || defined multispec_mac_swift
	#define IDC_ProjectClassMeans				8
	#define IDC_ClusterTrainingAreas			10          
	#define IDC_ClassPrompt						12 
	#define IDC_ClassCombo						13 
	#define IDC_LineStart						18 
	#define IDC_LineEnd							19 
	#define IDC_ColumnStart						21 
	#define IDC_ColumnEnd						22 
	#define IDC_NumberClustersPrompt			25 
	#define IDC_NumberClusters					26
	#define IDC_Distance1Prompt				31
	#define IDC_Distance1						32
	#define IDC_Distance2Prompt				33
	#define IDC_Distance2						34
#endif	// defined multispec_mac || defined multispec_mac_swift  
  
#if defined multispec_win  
	#include "WImageView.h"
	#include "WClusterIsodataDialog.h" 
#endif	// defined multispec_win  

//#include "SExtGlob.h" 



//------------------------------------------------------------------------------------
//
// M a c r o s 
//
//------------------------------------------------------------------------------------
/*
#define Distance(cluster, pix, distance)													\
	{																									\
	short int    			channelTemp;														\
	CMeanType2   			normDistanceTemp;													\
	HCTypePtr  				pixPtr;																\
	HCMeanTypePtr   		clusterMeanPtr;													\
																										\
	pixPtr = pix;																					\
	clusterMeanPtr = cluster->meanPtr;														\
	distance = 0;																					\
	for (channelTemp = 0; channelTemp < numberChannels; channelTemp++)			\
		{																								\
		normDistanceTemp = 10 * (CMeanType2)*pixPtr - 									\
										(CMeanType2)*clusterMeanPtr;							\
		distance += (double)normDistanceTemp * normDistanceTemp;						\
		pixPtr++;																					\
		clusterMeanPtr++;																			\
	   }																								\
	}

#define DoubleDistance(cluster, pix, distance)											\
	{																									\
	UInt32	    			channelTemp;														\
	CMeanType2   			normDistanceTemp;													\
	HDoublePtr  			pixPtr;																\
	HCMeanTypePtr   		clusterMeanPtr;													\
																										\
	pixPtr = pix;																					\
	clusterMeanPtr = cluster->meanPtr;														\
	distance = 0;																					\
	for (channelTemp=0; channelTemp<numberChannels; channelTemp++)					\
		{																								\
		normDistanceTemp = (CMeanType2)(10 * *pixPtr) - 								\
										(CMeanType2)*clusterMeanPtr;							\
		distance += (double)normDistanceTemp * normDistanceTemp;						\
		pixPtr++;																					\
		clusterMeanPtr++;																			\
	   }																								\
	}
*/
#define Distance(cluster, pix, distance)													\
	{																									\
	short int    			channelTemp;														\
	CMeanType2   			normDistanceTemp;													\
	HCTypePtr  				pixPtr;																\
	HCMeanTypePtr   		clusterMeanPtr;													\
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

#define DoubleDistance(cluster, pix, distance)											\
	{																									\
	UInt32	    			channelTemp;														\
	CMeanType2   			normDistanceTemp;													\
	HDoublePtr  			pixPtr;																\
	HCMeanTypePtr   		clusterMeanPtr;													\
																										\
	pixPtr = pix;																					\
	clusterMeanPtr = cluster->meanPtr;														\
	distance = 0;																					\
	for (channelTemp=0; channelTemp<numberChannels; channelTemp++)					\
		{																								\
		normDistanceTemp = (CMeanType2)(*pixPtr) - 										\
										(CMeanType2)*clusterMeanPtr;							\
		distance += (double)normDistanceTemp * normDistanceTemp;						\
		pixPtr++;																					\
		clusterMeanPtr++;																			\
	   }																								\
	}

//------------------------------------------------------------------------------------

#define AddClusterStats(pix, cluster)														\
	{																									\
	HDoublePtr   			clusterSumPtr;														\
	HDoublePtr				clusterSumSquarePtr;												\
	short int 				channelTemp;														\
	SInt32					numPixTemp;															\
	HCTypePtr  				pixPtr1;																\
																										\
	clusterSumPtr = cluster->sumPtr;															\
	clusterSumSquarePtr = cluster->sumSquarePtr1;										\
	(cluster->numPixels)++;																		\
	numPixTemp = cluster->numPixels;															\
	pixPtr1 = pix;																					\
	for (channelTemp = 0; channelTemp < numberChannels; channelTemp++)			\
		{																								\
	   *clusterSumPtr += *pixPtr1; 			 												\
	   clusterSumPtr++;																			\
																										\
		*clusterSumSquarePtr += (double)*pixPtr1 * *pixPtr1;							\
		clusterSumSquarePtr++;																	\
	   pixPtr1++;																					\
	   }																								\
	cluster->varianceComputed = FALSE;														\
	}

//------------------------------------------------------------------------------------

#define SubtractClusterStats(pix, cluster)												\
	{																									\
	HDoublePtr   			clusterSumPtr;														\
	HDoublePtr				clusterSumSquarePtr;												\
	short int 				channelTemp;														\
	SInt32					numPixTemp;															\
	HCTypePtr  				pixPtr1;																\
																										\
	clusterSumPtr = cluster->sumPtr;															\
	clusterSumSquarePtr = cluster->sumSquarePtr1;										\
	(cluster->numPixels)--;																		\
	numPixTemp = cluster->numPixels;															\
	pixPtr1 = pix;																					\
	for (channelTemp = 0; channelTemp < numberChannels; channelTemp++)			\
		{																								\
	   *clusterSumPtr -= *pixPtr1; 			 												\
	   clusterSumPtr++;																			\
																										\
		*clusterSumSquarePtr -= (double)*pixPtr1 * *pixPtr1;							\
		clusterSumSquarePtr++;																	\
	   pixPtr1++;																					\
	   }																								\
	cluster->varianceComputed = FALSE;														\
	}

//------------------------------------------------------------------------------------

			

			// Prototypes for file routines that are only called from other 		
			// routines in this file.															


Boolean						DetermineIfCheckToBeMadeForFillData (
									Handle								windowInfoHandle, 
									UInt16*								channelsPtr, 
									UInt16								numberChannels,
									UInt32								numberClusters);

Boolean 						GetCentersFromEigenvectorVolume (
									HDoublePtr							eigenVectorPtr, 
									HChannelStatisticsPtr			totalChanStatsPtr);
									
Boolean 						GetClassMeanClusterCenters (
									FileInfoPtr							fileInfoPtr);

Boolean						GetEigenvectorClusterCenters (
									FileIOInstructionsPtr			fileIOInstructionsPtr);

ClusterType* 				GetMemoryForClusters (
									SInt16								numberClusters);

Boolean 						InitializeClusterCenters (
									FileIOInstructionsPtr			fileIOInstructionsPtr);

SInt16						ISODATACluster (
									FileIOInstructionsPtr			fileIOInstructionsPtr);

SInt16	 					ISODATAClusterPass (
									FileIOInstructionsPtr			fileIOInstructionsPtr,
									Ptr*									clusterAddressesPtr, 
									SInt64*								numberClassChanges, 
									Boolean								firstPassFlag);
										
Boolean		 				UpdateClusterMeans (
									ClusterType*						newCluster,
									SInt32*								activeNumberClustersPtr);


	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Booelan DetermineIfCheckToBeMadeForFillData
//
//	Software purpose:	This routine determines if the data values in the image should
//							be checked for "fill" data values. I.E. those that are 
//							statistically very far from the rest of the data values.
//
//	Parameters in:		Pointer to the window information structure			
//
//	Parameters out:	Vector with minimum non-saturated data values
//							Vector with maximum non-saturated data values
//
// Value Returned:	Flag indicated whether the vectors were filled.	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/31/2012
//	Revised By:			Larry L. Biehl			Date: 02/01/2012

Boolean DetermineIfCheckToBeMadeForFillData (
				Handle								windowInfoHandle, 
				UInt16*								channelsPtr, 
				UInt16								numberChannels,
				UInt32								numberClusters)

{
	double								maxTailDifference,
											minTailDifference,
											nonSatClusterDistance;
											
	Handle								histogramSummaryHandle;
											
	HistogramSummaryPtr				histogramSummaryPtr;
	
	UInt32								channel,
											channelIndex;
	
	Boolean								checkForFillDataFlag = FALSE;
	
	
	if (GetHistogramLoadedFlag (windowInfoHandle))
		{
		histogramSummaryHandle = GetHistogramSummaryHandle (windowInfoHandle);
		
		histogramSummaryPtr = (HistogramSummaryPtr)GetHandlePointer (
																				histogramSummaryHandle);
		
		if (histogramSummaryPtr != NULL)
			{	
			for (channel=0; channel<numberChannels; channel++)
				{
				channelIndex = channelsPtr[channel];
				
				minTailDifference = histogramSummaryPtr[channelIndex].minNonSatValue - 
													histogramSummaryPtr[channelIndex].minValue;
				
				maxTailDifference = histogramSummaryPtr[channelIndex].maxValue - 
												histogramSummaryPtr[channelIndex].maxNonSatValue;
													
				nonSatClusterDistance = (histogramSummaryPtr[channelIndex].maxNonSatValue -
							histogramSummaryPtr[channelIndex].minNonSatValue)/numberClusters;		
													
				if (minTailDifference > 3 * nonSatClusterDistance ||
													maxTailDifference > 3 * nonSatClusterDistance)
					{
					checkForFillDataFlag = TRUE;
					break;
					
					}	// end "if (totalDistance > 2 * nonSatDistance)"
				
				}	// end "for (channel=0; channel<numberChannels; channel++)"
								
			}	// end "if (histogramSummaryPtr != NULL)"
			
		}	// end "if (GetHistogramLoadedFlag (windowInfoHandle))"
		
	return (checkForFillDataFlag);
							
}	// end "DetermineIfCheckToBeMadeForFillData"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetCentersFromEigenvectorVolume
//
//	Software purpose:	The purpose of this routine is to determine the
//							initial cluster centers within the first three
//							principal eigenvectors.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/10/1991
//	Revised By:			Larry L. Biehl			Date: 04/12/2002	

Boolean GetCentersFromEigenvectorVolume (
				HDoublePtr							eigenVectorPtr, 
				HChannelStatisticsPtr			totalChanStatsPtr)

{
	double								center,
											difference1,
											difference2,
											endSpan1,
											endSpan2,
											endSpan3,
											portion1,
											portionOffset2,
											portionOffset3,
											spanCenter1,
											spanCenter2,
											spanCenter3,
											spanIncrement1,
											startSpan1,
											startSpan2,
											startSpan3;
	
	ClusterType							*currentCluster;
	
	HDoublePtr							eigenVector1Ptr,
											eigenVector2Ptr,
											eigenVector3Ptr;
	
	SInt16								channel,
											cluster,
											eigenIndex,
											halfDistance,
											numberChannels,
											numberClusters,
											numberEigenvectors,
											numberSign3,
											numberStartSpan,
											sign2,
											sign3;
											
		
			// Check input parameters.															
			
	if (eigenVectorPtr == NULL || totalChanStatsPtr == NULL)
																					return (FALSE);
																					
			// Initialize variables.															
	
	numberChannels = gClusterSpecsPtr->numberChannels;
	numberClusters = gClusterSpecsPtr->maxNumberClusters;
	portionOffset2 = .35;
	portionOffset3 = portionOffset2 * (1 - portionOffset2);
	
			// Get the total span of the data that will fit within the				
			// data minimum and maximums when projected onto the first				
			// one, two or three principal eigenvectors.									
	
	eigenVector3Ptr = eigenVectorPtr;
	numberEigenvectors = MIN (3, numberChannels);
	if (gClusterSpecsPtr->initializationOption == 1)
		numberEigenvectors = 1;
	
	for (eigenIndex=0; eigenIndex<numberEigenvectors; eigenIndex++)
		{
		startSpan3 = DBL_MAX;
		endSpan3 = -DBL_MAX;
				
		for (channel=0; channel<numberChannels; channel++)
			{
			difference1 = ((double)totalChanStatsPtr[channel].maximum - 
							totalChanStatsPtr[channel].mean);
							
			difference2 = ((double)totalChanStatsPtr[channel].minimum - 
							totalChanStatsPtr[channel].mean);
							
			if (*eigenVector3Ptr != 0)
				{
				difference1 /= *eigenVector3Ptr;
				difference2 /= *eigenVector3Ptr;
				
				}	// end "if (*eigenVector3Ptr != 0)"
							
			if (gClusterSpecsPtr->useCorrelationMatrixFlag)
				{
						// For principal eigenvector from correlation matrix.			
				
				if (totalChanStatsPtr[channel].standardDev > 0)
					{	
					difference1 /= (double)totalChanStatsPtr[channel].standardDev;
					difference2 /= (double)totalChanStatsPtr[channel].standardDev;
					
					}	// end "if (totalChanStatsPtr[channel].standardDev > 0)"
				
				}	// end "if (gClusterSpecsPtr->useCorrelationMatrixFlag)" 
			
			if (difference1 < 0)
				{
				startSpan3 = MIN (startSpan3, difference2);
				endSpan3 = 	MAX (endSpan3, difference1);
				
				}	// end "if (difference1 < 0)" 
				
			else if (difference1 > 0)
				{
				startSpan3 = MIN (startSpan3, difference1);
				endSpan3 = 	MAX (endSpan3, difference2);
				
				}	// end "else if (difference1 > 0)" 
				
			//eigenVector3Ptr += numberChannels;
			eigenVector3Ptr++;
				
			}	// end "for (channel=0; channel<numberChannels; channel++)" 
			
				// Check if the variance is 0 for all of the channels
				
		if (startSpan3 == DBL_MAX)
			{
			startSpan3 = 0;
			endSpan3 = 0;
			
			}	// end "if (startSpan3 == DBL_MAX)"
		
		if (eigenIndex == 0)
			{
			startSpan1 = startSpan3;
			endSpan1 = endSpan3;
			//eigenVector3Ptr = &eigenVectorPtr[1];
			eigenVector3Ptr = &eigenVectorPtr[numberChannels];
			
			}	// end "if (eigenIndex == 0)" 
		
		if (eigenIndex == 1)
			{
			startSpan2 = startSpan3;
			endSpan2 = endSpan3;
			//eigenVector3Ptr = &eigenVectorPtr[2];
			eigenVector3Ptr = &eigenVectorPtr[2*numberChannels];
			
			}	// end "if (eigenIndex == 1)" 
		
		}	// end "for (eigenIndex=0; eigenIndex<..." 
		
			// Initialize the cluster centers.												
		
	numberStartSpan = numberClusters/2;
	
	halfDistance = 0;
	if (numberStartSpan * 2 != numberClusters)
		halfDistance = 1;
		
	eigenVector1Ptr = eigenVectorPtr;
	//eigenVector2Ptr = &eigenVectorPtr[1];
	//eigenVector3Ptr = &eigenVectorPtr[2];
	eigenVector2Ptr = &eigenVectorPtr[numberChannels];
	eigenVector3Ptr = &eigenVectorPtr[2*numberChannels];
	
	for (channel=0; channel<numberChannels; channel++)
		{
		spanIncrement1 = startSpan1/((double)numberStartSpan + halfDistance/2);
		spanCenter1 = startSpan1 - 0.5 * spanIncrement1;
		
		sign2 = +1;
		sign3 = +1;
		numberSign3 = 1;
			
		currentCluster = gClusterSpecsPtr->clusterEigenCenterHead;
		for (cluster=1; cluster<=numberClusters; cluster++)
			{
					// Initialize cluster center with 1st eigenvector					
					// component.																	
						
			if (!gClusterSpecsPtr->useCorrelationMatrixFlag)
				center = totalChanStatsPtr[channel].mean + 
												spanCenter1 * *eigenVector1Ptr;
				
			else	// gClusterSpecsPtr->useCorrelationMatrixFlag
				center = totalChanStatsPtr[channel].mean + 
								(double)totalChanStatsPtr[channel].standardDev *
												spanCenter1 * *eigenVector1Ptr;
				
			if (cluster > 1 && cluster < numberClusters)
				{
						// Add offset for 2nd eigenvector.									
							
				if (numberEigenvectors >= 2)
					{		
					if (cluster <= numberStartSpan)
						{
						portion1 = startSpan1 - spanCenter1;
						if (startSpan1 != 0)
							portion1 /= startSpan1;
							
						}	// end "if (cluster <= numberStartSpan)"
						
					else	// cluster > numberStartSpan 
						{
						portion1 = endSpan1 - spanCenter1;
						if (endSpan1 != 0)
							portion1 /= endSpan1;
						
						}	// end "else cluster > numberStartSpan"
					
					if (sign2 > 0)
						spanCenter2 = portionOffset2 * portion1 * startSpan2;
						
					else	// sign2 < 0 
						spanCenter2 = portionOffset2 * portion1 * endSpan2;
						
					if (!gClusterSpecsPtr->useCorrelationMatrixFlag)
						center += spanCenter2 * *eigenVector2Ptr;
					
					else	// gClusterSpecsPtr->useCorrelationMatrixFlag 
						center += (double)totalChanStatsPtr[channel].standardDev *
													spanCenter2 * *eigenVector2Ptr;
				
							// Change the sign for the 2nd eigenvector component.		
							
					sign2 = -sign2;
					
					}	// end "if (numberEigenvectors >= 2)" 
							
						// Add offset for 3rd eigenvector.									
				
				if (numberEigenvectors >= 3)
					{		
					if (sign3 > 0)
						spanCenter3 = portionOffset3 * portion1 * startSpan3;
						
					else	// sign3 < 0 
						spanCenter3 = portionOffset3 * portion1 * endSpan3;
						
					if (!gClusterSpecsPtr->useCorrelationMatrixFlag)
						center += spanCenter3 * *eigenVector3Ptr;
					
					else	// gClusterSpecsPtr->useCorrelationMatrixFlag 
						center += (double)totalChanStatsPtr[channel].standardDev *
													spanCenter3 * *eigenVector3Ptr;
				
							// Change the sign for the 3rd eigenvector component.		
					
					numberSign3++;
					if (numberSign3 == 2)
						{
						sign3 = -sign3;
						numberSign3 = 0;
						
						}	// end "if (numberSign3 == 3)" 
					
					}	// end "if (numberEigenvectors >= 3)" 
					
				}	// end "if (cluster > 1 && cluster < numberClusters)" 
				
			currentCluster->meanPtr[channel] = (CMeanType)center;
			
			if (cluster == numberStartSpan)
				{
						// Change the increment for the negative side of 				
						// of the span of the 1st eigenvector.								
						
				spanIncrement1 = 
								-endSpan1/((double)numberStartSpan + halfDistance/2);
				
				if (halfDistance == 1)
					spanCenter1 = 0;
				
				else	// halfDistance == 0 
					spanCenter1 = -0.5 * spanIncrement1;
					
				}	// end "if (cluster == numberStartSpan)" 
				
			else	// cluster != numberStartSpan 
				spanCenter1 -= spanIncrement1;
			
			currentCluster = currentCluster->next;
			
			}	// end "for (cluster=1; cluster<=numberClusters; ..." 
		
//		eigenVector1Ptr += numberChannels;
//		eigenVector2Ptr += numberChannels;
//		eigenVector3Ptr += numberChannels;
		eigenVector1Ptr++;
		eigenVector2Ptr++;
		eigenVector3Ptr++;
			
		}	// end "for (channel=0; channel<numberChannels; channel++)" 
		
	return (TRUE);
			
}	// end "GetCentersFromEigenvectorVolume" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetClassMeanClusterCenters
//
//	Software purpose:	The purpose of this routine is to determine the
//							initial cluster centers from the current project class
//							means. If more classes are requested than the number
//							of project classes available then other centers are added
//							from the eigenvector volume.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/16/1997
//	Revised By:			Larry L. Biehl			Date: 02/05/1998	

Boolean GetClassMeanClusterCenters (
				FileInfoPtr							fileInfoPtr)

{
	double								closestDistance,
											currentDistance;
										
	HChannelStatisticsPtr			channelStatsPtr;
	
	HDoublePtr							channelMeanPtr;
											
	ClusterType							*closestCluster,
											*currentCluster,
											*projectClassCluster;

	SInt16* 								initializationClassPtr;
	
	UInt16* 								featurePtr;
	
	UInt32								channel,
											cluster,
											lastClusterNumber,
											numberChannels,
											numberProjectClassClusters;
	
	Boolean								continueFlag;
	
	
			// Check input variables.															
	
	if (gClusterSpecsPtr == NULL || gClusterSpecsPtr->clusterHead == NULL)
																				return (FALSE);
	
			// Initialize local variables.													
			
	continueFlag = TRUE;
	channelMeanPtr = NULL;
	channelStatsPtr = NULL;
	numberProjectClassClusters = gClusterSpecsPtr->numberInitializationClasses;
	lastClusterNumber = 
						gClusterSpecsPtr->maxNumberClusters + numberProjectClassClusters;
						
	numberChannels = gClusterSpecsPtr->numberChannels;
	featurePtr = (UInt16*)GetHandlePointer (gClusterSpecsPtr->featureHandle);
									
	initializationClassPtr = (SInt16*)GetHandlePointer (
												gClusterSpecsPtr->initializationClassHandle);
									
			// Get pointer to memory to use for class mean vectors.
									
	UInt32 bytesNeeded = numberChannels * sizeof (double);
	channelMeanPtr = (HDoublePtr)MNewPointer (bytesNeeded);
	continueFlag = (channelMeanPtr != NULL);
									
			// Get pointer to memory to use for class channel statistics	
			// that will be used in computing the covariance matrix and		
			// getting the data for the class mean vector.						
	
	if (continueFlag)
		{			
		UInt32 bytesNeeded = numberChannels * sizeof (ChannelStatistics);
		channelStatsPtr = (HChannelStatisticsPtr)MNewPointer (bytesNeeded);
		continueFlag = (channelStatsPtr != NULL);
		
		}	// end "if (continueFlag)"
	
	if (continueFlag)
		{
				// Get the cluster structure to start loading the project class
				// mean information in.
				
		projectClassCluster = gClusterSpecsPtr->clusterHead;
		      		
//		currentCluster = gClusterSpecsPtr->clusterHead;
//	   while (currentCluster != NULL && 
//	   		currentCluster->clusterNumber != gClusterSpecsPtr->maxNumberClusters)
//	   	{
//	      currentCluster = currentCluster->next;
//	      
//	  		}	// end " while (currentCluster != NULL)"
//	  		
//	  	projectClassCluster = currentCluster->next;
  	
		for (cluster=0; cluster<numberProjectClassClusters; cluster++)
			{	
			UInt16 statClassNumber = initializationClassPtr[cluster] - 1;
	     	 
	     	projectClassCluster->projectStatClassNumber = (SInt16)statClassNumber;
							
					// Get the class mean vector	
					
			GetClassMeanVector (
							(UInt16)numberChannels, 
							channelStatsPtr, 
							channelMeanPtr, 
							featurePtr, 
							(UInt16)statClassNumber);
			
					// Store the mean in the cluster structure.
									
			for (channel=0; channel<numberChannels; channel++)
				{									
				projectClassCluster->meanPtr[channel] = 
													(CMeanType)channelMeanPtr[channel];
				
				}	// end "for (channel=0; channel<numberChannels; channel++)"
	
   				// Find closest eigenvector cluster center in absolute sense 
   				// (Euclidean distance). 															
							
   		closestCluster = gClusterSpecsPtr->clusterEigenCenterHead;
   		currentCluster = gClusterSpecsPtr->clusterEigenCenterHead->next;
   		DoubleDistance (closestCluster, channelMeanPtr, closestDistance);
	      		
	      while (currentCluster != NULL && 
	      			currentCluster->clusterNumber <= (SInt32)lastClusterNumber)
	      	{
	         DoubleDistance (currentCluster, channelMeanPtr, currentDistance);
	
	         if (currentDistance < closestDistance)
	         	{
	            closestCluster = currentCluster;
	            closestDistance = currentDistance;
	            
	         	}	// end "if (currentDistance < closestDistance)" 
	
	         currentCluster = currentCluster->next;
	         
	     		}	// end " while (currentCluster != NULL && ...)"
	     		
	     			// Make the number of pixels in the closest cluster number 
	     			// negative to indicate that it will be deleted since it is closest 
	     			// to a defined project class.
	     		
	     	closestCluster->numPixels = -1;
			
			projectClassCluster = projectClassCluster->next;
			
			}	// end "for (cluster=0; cluster<numberProjectClassClusters; ..."
			
				// Now remove those eigenvector centers that are closest to the
				// project class means.
   		 
   	gClusterSpecsPtr->numberClusters = DeleteSpecifiedClusters (
						   									gClusterSpecsPtr->numberClusters,
						   									0);
   	
				// Now update the cluster numbers so that they are in order.
		      		
		currentCluster = gClusterSpecsPtr->clusterHead;
		cluster = 1;
	   while (currentCluster != NULL)
	   	{
	   	currentCluster->clusterNumber = (SInt16)cluster;
	   	cluster++;
	   	
	      currentCluster = currentCluster->next;
	      
	  		}	// end " while (currentCluster != NULL)"
			
		}	// end "if (continueFlag)"
		
			// Release memory for obtaining project class means.								
			
	CheckAndDisposePtr ((Ptr)channelStatsPtr);
	CheckAndDisposePtr (channelMeanPtr);
		
	return (continueFlag);

}	// end "GetClassMeanClusterCenters" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetEigenvectorClusterCenters
//
//	Software purpose:	The purpose of this routine is to determine the
//							initial cluster centers along the principal eigenvector.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			InitializeClusterCenters in SClustID.cpp
//
//	Coded By:			Larry L. Biehl			Date: 08/07/1990
//	Revised By:			Larry L. Biehl			Date: 02/01/2012	

Boolean GetEigenvectorClusterCenters (
				FileIOInstructionsPtr			fileIOInstructionsPtr)

{
	HChannelStatisticsPtr			totalChanStatsPtr;
	
	HDoublePtr							covariancePtr,
											eigenVectorPtr,
											workVectorPtr;
												
	SInt16*								shortIntWorkVectorPtr;
	
	UInt32								bytesNeeded;
	
	SInt16								returnCode;
	
	Boolean								checkForFillDataFlag,
											continueFlag;
	
	
			// Check input variables.															
	
	if (gClusterSpecsPtr == NULL || gClusterSpecsPtr->clusterHead == NULL)
																				return (FALSE);
	
			// Initialize local variables.													
			
	continueFlag = TRUE;
	covariancePtr = NULL;
	eigenVectorPtr = NULL;
	workVectorPtr = NULL;
	shortIntWorkVectorPtr = NULL;
	
	LoadDItemStringNumber (kClusterStrID, 
									IDS_Cluster16, 
									gStatusDialogPtr, 
									IDC_Status11, 
									(Str255*)&gTextString);
	
	ShowStatusDialogItemSet (kStatusLine);
		
			// Get the total covariance for the requested.
				
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
											
	UInt16* channelsPtr = (UInt16*)GetHandlePointer (
												gClusterSpecsPtr->channelsHandle);
	
	SInt16* clusterClassPtr = (SInt16*)GetHandlePointer (
												gClusterSpecsPtr->clusterClassHandle);
									
	checkForFillDataFlag = DetermineIfCheckToBeMadeForFillData (
											GetActiveImageWindowInfoHandle (), 
											channelsPtr, 
											gClusterSpecsPtr->numberChannels, 
											gClusterSpecsPtr->numberClusters);
			
	continueFlag = GetTotalSumSquares (
											fileIOInstructionsPtr,
											&gClusterSpecsPtr->totalChanStatsHandle, 
											&gClusterSpecsPtr->totalSumSquaresStatsHandle, 
											channelsPtr, 
											gClusterSpecsPtr->numberChannels,
											clusterClassPtr, 
											gClusterSpecsPtr->numberClusterClasses, 
											gClusterSpecsPtr->clustersFrom,
											kTrainingType,
											checkForFillDataFlag);
											
			// Hide the status dialog item pertaining to getting the total			
			// covariance.																			
		
	HideStatusDialogItemSet (kStatusLine);						
					
	LoadDItemStringNumber (kClusterStrID,
						IDS_Cluster17, // "Determining Initial Clusters (P. Eigenvector)"	
						gStatusDialogPtr, 
						IDC_Status11, 
						(Str255*)&gTextString);
											
				// Get memory for matrices and vectors to be used to compute		
				// the principal eigenvector.													
				
	if (continueFlag)
		{
				// Get pointer to memory to use for square covariance.				
					
		bytesNeeded = (UInt32)gClusterSpecsPtr->numberChannels *
										gClusterSpecsPtr->numberChannels * sizeof (double);
		covariancePtr = (HDoublePtr)MNewPointer (bytesNeeded);
		if (covariancePtr == NULL)  
			continueFlag = FALSE;
			
		}	// end "if (continueFlag)" 
	
	if (continueFlag)
		{
					// Get pointer to memory to use for eigenVector.					
					
		bytesNeeded = (UInt32)gClusterSpecsPtr->numberChannels *
										gClusterSpecsPtr->numberChannels * sizeof (double);
		eigenVectorPtr = (HDoublePtr)MNewPointer (bytesNeeded);
		if (eigenVectorPtr == NULL)  
			continueFlag = FALSE;
			
		}	// end "if (continueFlag)" 
		
	if (continueFlag)
		{
					// Get pointer to memory to use for work space for					
					// computing the eigenvector.												
					
		bytesNeeded = (UInt32)MAX (2,gClusterSpecsPtr->numberChannels) *
																				sizeof (double);
		workVectorPtr = (HDoublePtr)MNewPointer (bytesNeeded);
		if (workVectorPtr == NULL)  
			continueFlag = FALSE;
			
		}	// end "if (continueFlag)" 
			
				// Get pointer to memory to use for short int work vector.			

	if (continueFlag)
		{		
		bytesNeeded = (UInt32)gClusterSpecsPtr->numberChannels *
																			sizeof (SInt16);
		shortIntWorkVectorPtr = (SInt16*)MNewPointer (bytesNeeded);
		if (shortIntWorkVectorPtr == NULL)  
			continueFlag = FALSE;
			
		}	// end "if (continueFlag && ...)" 
		
	gClusterSpecsPtr->totalSumSquaresStatsPtr = 
			(HCovarianceStatisticsPtr)GetHandlePointer (
								gClusterSpecsPtr->totalSumSquaresStatsHandle);
				
	totalChanStatsPtr = 
			(HChannelStatisticsPtr)GetHandlePointer (
										gClusterSpecsPtr->totalChanStatsHandle);
					
	if (continueFlag)
		{	
		if (!gClusterSpecsPtr->useCorrelationMatrixFlag)	
			ComputeCovarianceMatrix (
				gClusterSpecsPtr->numberChannels,
				covariancePtr, 
				gClusterSpecsPtr->numberChannels,
				NULL,
				totalChanStatsPtr, 
				(HDoublePtr)gClusterSpecsPtr->totalSumSquaresStatsPtr,
				gAreaDescription.numSamplesPerChan,
				kSquareOutputMatrix);
						
		else 		// gClusterSpecsPtr->useCorrelationMatrixFlag
			ComputeCorrelationCoefficientMatrix (
				gClusterSpecsPtr->numberChannels,
				covariancePtr, 
				gClusterSpecsPtr->numberChannels,
				NULL,
				totalChanStatsPtr, 
				(HDoublePtr)gClusterSpecsPtr->totalSumSquaresStatsPtr,
				gAreaDescription.numSamplesPerChan,
				kSquareOutputMatrix);
									
		returnCode = ComputeEigenvectors (covariancePtr, 
									gClusterSpecsPtr->numberChannels, 
									eigenVectorPtr,
									shortIntWorkVectorPtr,
									workVectorPtr,
									1);
									
		if (returnCode < 0)
			continueFlag = FALSE;
		
		}	// end "if (continueFlag)" 
		
			// Now get the initial cluster centers by scattering along the			
			// principal eigenvector.														
			
	LoadDItemStringNumber (kClusterStrID,
									IDS_Cluster18, // "Determining Initial Clusters"
									gStatusDialogPtr, 
									IDC_Status11, 
									(Str255*)&gTextString);
					
	continueFlag = 
				GetCentersFromEigenvectorVolume (eigenVectorPtr, totalChanStatsPtr);
				
			// Release handles for total covariance information.						
	
	gClusterSpecsPtr->totalChanStatsHandle = 
						UnlockAndDispose (gClusterSpecsPtr->totalChanStatsHandle);
	gClusterSpecsPtr->totalSumSquaresStatsHandle = 
						UnlockAndDispose (gClusterSpecsPtr->totalSumSquaresStatsHandle);

	gClusterSpecsPtr->totalChanStatsPtr = NULL;
	gClusterSpecsPtr->totalSumSquaresStatsPtr = NULL;
		
			// Release memory for eigenvector information.								
			
	covariancePtr = CheckAndDisposePtr (covariancePtr);
	eigenVectorPtr = CheckAndDisposePtr (eigenVectorPtr);
	workVectorPtr = CheckAndDisposePtr (workVectorPtr);
	shortIntWorkVectorPtr = CheckAndDisposePtr (shortIntWorkVectorPtr);
		
	return (continueFlag);

}	// end "GetEigenvectorClusterCenters" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		ClusterType* GetMemoryForClusters
//
//	Software purpose:	The purpose of this routine is to get the memory
//							for a new cluster structure and initialize it.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/08/1990
//	Revised By:			Larry L. Biehl			Date: 02/07/2018

ClusterType* GetMemoryForClusters (
				SInt16								numberClusters)

{
	ClusterType							*clusterHead,
   										*newCluster;
	
	HDoublePtr							sumPtr;
	HCMeanTypePtr   					meanPtr;
   
  	SInt32								channelMeansIndex,
   										channelSumSquaresIndex,
   										cluster,
   										numberBytes,
   										numberChannelSumsSquares;
	
   SInt16								numberChannels;
	
	
			// Check input variables.															
			
	if (numberClusters <= 0 || gClusterSpecsPtr->numberChannels <= 0)
																					return (NULL);
																					
			// Initialize local variables.													
			
	numberChannels = gClusterSpecsPtr->numberChannels;
	
			// Get memory for the cluster structure;										
					
	clusterHead = (ClusterType*)MNewPointer (
										(size_t)sizeof (ClusterType) * numberClusters);
	if (clusterHead == NULL)
																				return (NULL);
																				
			// Get memory for cluster means.													
			
	numberBytes = (SInt32)numberClusters * numberChannels * sizeof (CMeanType);
	meanPtr = (HCMeanTypePtr) MNewPointerClear (numberBytes);
	
   sumPtr = NULL;
	if (meanPtr != NULL)
		{
				// Get memory for cluster sums, variances and sum of squares.		
				// Memory will be allocated for cross sum of squares if the			
				// statistics are to be saved to a project file.						
		
		numberChannelSumsSquares = 3 * numberChannels;
		numberBytes = 
			(SInt32)numberClusters * numberChannelSumsSquares  * sizeof (double);
		sumPtr = (HDoublePtr) MNewPointerClear (numberBytes);
		
		}	// end "if (meanPtr != NULL)" 
		
			// Release memory if there is not enough for all requests.				
			
	if (sumPtr == NULL)
		{
		clusterHead = (ClusterType*)CheckAndDisposePtr ((Ptr)clusterHead);
		meanPtr = (HCMeanTypePtr)CheckAndDisposePtr ((Ptr)meanPtr);
																				return (NULL);
		
		}	// end "if (sumPtr == NULL)" 
																	
			// Intialize cluster structure.													
	
	channelMeansIndex = 0;
	channelSumSquaresIndex = 0;
	
	for (cluster=1; cluster<=numberClusters; cluster++)
		{
		newCluster = &clusterHead[cluster-1];
	
		newCluster->clusterNumber = (SInt16)cluster;
		newCluster->projectStatClassNumber = -1;
		newCluster->numPixels = 0;
		newCluster->numPixInCluster = 0;
		newCluster->varianceComputed = FALSE;
		newCluster->meanPtr = &meanPtr[channelMeansIndex];
		newCluster->sumPtr = &sumPtr[channelSumSquaresIndex];
      												
		newCluster->variancePtr = &newCluster->sumPtr[numberChannels];
		newCluster->sumSquarePtr1 = &newCluster->variancePtr[numberChannels];
		newCluster->sumSquarePtr2 = NULL;
		
		if (cluster != numberClusters)
			newCluster->next = &clusterHead[cluster];
			
		else	// cluster == numberClusters 
			newCluster->next = NULL;
		
		channelMeansIndex += numberChannels;
		channelSumSquaresIndex += numberChannelSumsSquares;
		
		}	// end "for (cluster=1; cluster<=numberClusters; cluster++)" 
		
	return (clusterHead);
	
}	//  end "GetMemoryForClusters" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean InitializeClusterCenters
//
//	Software purpose:	The purpose of this routine is to determine the
//							initial cluster centers to start the clustering
//							algorithm at.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/07/1990
//	Revised By:			Larry L. Biehl			Date: 11/16/1999	

Boolean InitializeClusterCenters (
				FileIOInstructionsPtr			fileIOInstructionsPtr)

{
											
	ClusterType							*currentCluster;
	
	UInt32								numberProjectClassClusters;
											
	Boolean								continueFlag;
	
	
			// Initialize local variables.												
			
	continueFlag = TRUE;

	switch (gClusterSpecsPtr->initializationOption)
		{
		case 1:		// Along principal eigenvector
		case 2:		// Within eigenvector volume 
	
					// Get memory for the stats for the number of cluster 		
					// classes that have been requested.	
			
			gClusterSpecsPtr->clusterHead = 
					gClusterSpecsPtr->clusterMemoryHead =
									GetMemoryForClusters (gClusterSpecsPtr->numberClusters);
			if (gClusterSpecsPtr->clusterHead == NULL)
																				return (FALSE);
			
					// Get the cluster to start loading the eigenvector cluster
					// centers at. We will load any project class means at the 
					// beginning.
																						
			gClusterSpecsPtr->clusterEigenCenterHead = gClusterSpecsPtr->clusterHead;
			if (gClusterSpecsPtr->projectClassMeansCode == 1)
				{	
				numberProjectClassClusters = gClusterSpecsPtr->numberInitializationClasses;
				
				currentCluster = gClusterSpecsPtr->clusterHead;
	  			while (currentCluster != NULL && 
	   				currentCluster->clusterNumber != (SInt32)numberProjectClassClusters)
	     			currentCluster = currentCluster->next;
	     			
	     		gClusterSpecsPtr->clusterEigenCenterHead = currentCluster->next;
	     			
	     		}	// end "if (gClusterSpecsPtr->projectClassMeansCode == 1)"
		
			continueFlag = GetEigenvectorClusterCenters (fileIOInstructionsPtr);
			break;
		
		case 3:		// One pass cluster centers 
			continueFlag = GetOnePassClusterCenters (fileIOInstructionsPtr);
			break;
		
		}	// end "switch (gClusterSpecsPtr->initializationOption)" 
		
	if (gClusterSpecsPtr->projectClassMeansCode == 1)
		{
				// Include project class means
		
		if (continueFlag)																
			continueFlag = 
							GetClassMeanClusterCenters (fileIOInstructionsPtr->fileInfoPtr);
		
		}	// end "if (gClusterSpecsPtr->projectClassMeansCode == 1)" 
		
	return (continueFlag);

}	// end "InitializeClusterCenters" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 ISODATACluster
//
//	Software purpose:	The purpose of this routine is to perform the
//							ISODATA cluster on the requested set of data.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/06/1990
//	Revised By:			Larry L. Biehl			Date: 05/18/2018

SInt16 ISODATACluster (
				FileIOInstructionsPtr			fileIOInstructionsPtr)

{		
			// Define local structures and variables.
	
	char									percentNotChangedString[32],
											savedPercentNotChangedString[32];
			
	Str255								numberChangesString,
											totalNumberClusterPixelsString;
    
	double								constant,
											percentNotChanged;
	
	SInt64								changeThreshold,
											numberChanges;
											//savedNumberChanges;
											//stepDifference;
   		
	ClusterType							*currentCluster;	// Cluster currently working on.	
									
   CMFileStream*						clResultsFileStreamPtr;
   
	Ptr									*clusterAddressesPtr;
	
	SInt32								numberActiveClusters,
											passNumber;
	
	UInt32								numberDecimalPlaces;
   		
	SInt16	           				minimumClusterSize,
 											numberChannels,
 											returnCode;									
	
	Boolean								continueFlag,
											firstPassFlag;
   

			// Initialize local variables. 													
   
   continueFlag = TRUE;
   clusterAddressesPtr = NULL;
   numberChannels = gClusterSpecsPtr->numberChannels;
	clResultsFileStreamPtr = GetResultsFileStreamPtr (0);
	minimumClusterSize = gClusterSpecsPtr->minClusterSize;
	passNumber = 0;
	returnCode = 1;
										
	changeThreshold = (SInt64)(((double)100 - gClusterSpecsPtr->convergence) * 
						gClusterSpecsPtr->totalNumberClusterPixels / 100);
										
			// Get the memory to store the cluster structure address is.			
			
	clusterAddressesPtr = (Ptr*)MNewPointerClear (
				((SInt32)gClusterSpecsPtr->numberClusters + 1) * sizeof (Ptr));
	if (clusterAddressesPtr == NULL)
																						return (-1);
		
			// Load vector of cluster structure addresses into 						
			// 'clusterAddressesPtr'.															
			
	currentCluster = gClusterSpecsPtr->clusterHead;
	while (currentCluster != NULL)
     	{
     	clusterAddressesPtr[currentCluster->clusterNumber] = (Ptr)currentCluster;
     	
		currentCluster = currentCluster->next;
		
		}	// end "while (currentCluster != NULL)" 
	
			// Update status dialog box if needed.											
			
	if (gStatusDialogPtr != NULL)
		{
				// Put number of training areas in the dialog box.						
			 
		if (gClusterSpecsPtr->clustersFrom == kTrainingType)
			LoadDItemValue (gStatusDialogPtr, 
									IDC_Status5, 
									(SInt32)gClusterSpecsPtr->totalNumberAreas);
		
		LoadDItemStringNumber (kClusterStrID,
										IDS_Cluster19, // Percent of pixels not changed:
										gStatusDialogPtr, 
										IDC_Status15, 
										(Str255*)&gTextString);
										
		LoadDItemStringNumber (kDialogStrID,
										IDS_Dialog18,		// Blank string
										gStatusDialogPtr, 
										IDC_Status16, 
										(Str255*)&gTextString);
		
		ShowStatusDialogItemSet (kStatusField);
		ShowStatusDialogItemSet (kStatusCluster);
		ShowStatusDialogItemSet (kStatusCommand);
		
		if (gClusterSpecsPtr->clustersFrom == kAreaType)
			ShowStatusDialogItemSet (kStatusMinutes);
			
		}	// end "if (gStatusDialogPtr)" 
	
   		// Continue clustering until the number of class changes is less		
   		// than the convergence threshold.												
   
   firstPassFlag = TRUE;
   numberActiveClusters = gClusterSpecsPtr->numberClusters;
	constant = (double)100/gClusterSpecsPtr->totalNumberClusterPixels;
	savedPercentNotChangedString[0] = 0;
	numberDecimalPlaces = 0;
   do
   	{
   			// Update dialog box.															
   	
   	passNumber++;	
   	gTextString[0] = (SInt8)sprintf ((char*)&gTextString[1], 
   					"ISODATA Cluster - Pass %ld  with %ld active clusters.", 
   					passNumber,
   					numberActiveClusters);
		LoadDItemString (gStatusDialogPtr, IDC_Status11, (Str255*)gTextString);
		
		numberChanges = 0;						
   	returnCode = ISODATAClusterPass (
   		fileIOInstructionsPtr, clusterAddressesPtr, &numberChanges, firstPassFlag);
   	
   	if (returnCode >= 0)			
   		if (!UpdateClusterMeans (gClusterSpecsPtr->clusterHead,
   											&numberActiveClusters))
   			returnCode = -1;
   			
   			// Update status dialog with the percent that did not change.
				// Note that the decimal place calculation will not be correct if
				// the number of pixel changes is more than max int. Need a
				// 64 bit abs function.
   	
   	percentNotChanged = 
   		((double)gClusterSpecsPtr->totalNumberClusterPixels - numberChanges) *
																									constant;
		
   	percentNotChangedString[0] = sprintf (&percentNotChangedString[1],
															"%.*f",
															(int)numberDecimalPlaces,
															percentNotChanged);
		
		if (!strcmp (percentNotChangedString, savedPercentNotChangedString))
			{
			numberDecimalPlaces++;
			numberDecimalPlaces = MIN (numberDecimalPlaces, 7);
			percentNotChangedString[0] = sprintf (&percentNotChangedString[1],
																"%.*f",
																(int)numberDecimalPlaces,
																percentNotChanged);
			
			}	// end "if (strcmp (percentNotChangedString, ..."

		LoadDItemString (gStatusDialogPtr, IDC_Status16, (Str255*)percentNotChangedString);

		strcpy (savedPercentNotChangedString, percentNotChangedString);
		
	  	firstPassFlag = FALSE;
	  		
	  	}	while (numberChanges > changeThreshold && returnCode == 1);
	
	HideStatusDialogItemSet (kStatusMinutes);
   
	if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
		returnCode = -1;
	
   		// Change returnCode to indicate to continue if the user just requested
   		// to quit after completion of the current iteration. The user still
   		// wants the output.
   			
	if (gAlertReturnCode == 1 && returnCode == 0)
		{
		gAlertReturnCode = 0;
		returnCode = 1;
		
		}	// end "if (gAlertReturnCode == 1 && returnCode == 0)" 

   		// Delete those clusters with too few pixels.
   
   if (returnCode != -1)		 
   	gClusterSpecsPtr->numberFinalClusters = 
   								DeleteSpecifiedClusters (gClusterSpecsPtr->numberClusters,
   																	minimumClusterSize);	
   											
   		// Check if output needs to be generated.
   		
   if (returnCode == -1) 
   	continueFlag = FALSE;

			// List message concerning the completion of the clustering.
			
	CreateNumberWithCommasInString ((char*)numberChangesString, 
												numberChanges);		
			
	CreateNumberWithCommasInString ((char*)totalNumberClusterPixelsString, 
												gClusterSpecsPtr->totalNumberClusterPixels);				
			 
	sprintf ((char*)gTextString,
		"    Clustering completed after %ld passes and %s of %s pixels changed.%s",
			passNumber, 
			numberChangesString, 
			totalNumberClusterPixelsString,
			gEndOfLine);
	
	if (!OutputString (clResultsFileStreamPtr, 
								(char*)gTextString, 
								0, 
								gOutputForce1Code, 
								continueFlag))
   	returnCode = -1;

			// List message if number of cluster classes were reduced because		
			// there were no points assigned to the class.								
			 
   if (gClusterSpecsPtr->numberFinalClusters < gClusterSpecsPtr->numberClusters)
   	{				
		if (!ListSpecifiedStringNumber (
										kClusterStrID, 
										IDS_Cluster4,
										clResultsFileStreamPtr, 
										gOutputForce1Code, 
										(SInt32)gClusterSpecsPtr->numberFinalClusters, 
										continueFlag))
   		returnCode = -1;
      
  		}	// end "if (...->numberFinalClusters < ...->numberClusters)" 
   																	
	if (	(gClusterSpecsPtr->numberFinalClusters > kMaxNumberStatClasses-1) && 
			(gClusterSpecsPtr->saveStatisticsCode > 0 || 
											gClusterSpecsPtr->classificationArea != 0))
		{
				// Only keep the the 'maxNumberClusters' that have the largest number
				// of pixels.
				
		KeepLargestClusters (kMaxNumberStatClasses - 1);
		
		if (!ListSpecifiedStringNumber (
										kClusterStrID, 
										IDS_Cluster5,
										clResultsFileStreamPtr, 
										gOutputForce1Code, 
										(SInt32)(kMaxNumberStatClasses-1), 
										continueFlag))
   		returnCode = -1;
		
		}	// end "if ((clusterCount > kMaxNumberStatClasses-1) && ..." 
	
	clusterAddressesPtr = (Ptr*)CheckAndDisposePtr ((Ptr)clusterAddressesPtr);

   		// Return number of clusters found. 											
   		
   return (returnCode);

}     // end Function "ISODATACluster" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ISODATAClusterControl
//
//	Software purpose:	The purpose of this routine is to control the user
//							requests for ISODATA cluster operations on an image file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/06/1990
//	Revised By:			Larry L. Biehl			Date: 10/26/1999	

Boolean ISODATAClusterControl (
				FileInfoPtr							fileInfoPtr)

{
	CMFileStream*						resultsFileStreamPtr;
	
	ClusterType      					*currentCluster,
											*lastCluster;
											
	FileIOInstructionsPtr			fileIOInstructionsPtr;
								      
	HUCharPtr							classifyBufferPtr;
	
	Boolean								continueFlag;
	
   
   		// Initialize variables.															
   				
			// Get the number of clusters. Allow for inclusion of
			// current project class means.
			
	gClusterSpecsPtr->numberClusters = gClusterSpecsPtr->maxNumberClusters;
	if (gClusterSpecsPtr->projectClassMeansCode == 1)
		gClusterSpecsPtr->numberClusters +=	
									(SInt16)gClusterSpecsPtr->numberInitializationClasses;	
				
	continueFlag = TRUE;
	classifyBufferPtr = NULL;
   resultsFileStreamPtr = GetResultsFileStreamPtr (0);
	fileIOInstructionsPtr = NULL;
	
	if (continueFlag)																					
		continueFlag = InitializeClusterMemory (&fileIOInstructionsPtr);
   		
   if (continueFlag) 
		{			
				// Initialize the cluster centers according to the request.			
				
		continueFlag = InitializeClusterCenters (fileIOInstructionsPtr);

   			// List initial cluster statistics. 										
   	
				//"\rInitial cluster class centers.\r"										
						
		continueFlag = ListSpecifiedStringNumber (
										kClusterStrID, 
										IDS_Cluster13, 
										(unsigned char*)gTextString, 
										resultsFileStreamPtr, 
										gOutputForce1Code,
										continueFlag);
   	if (continueFlag)
			continueFlag = ListClusterStatistics (
										resultsFileStreamPtr, 
										&gOutputForce1Code, 
										FALSE);
										
				// Initialize some global variables to allow the user to cancel	
				// the operation at the end of the current iteration instead of	
				// canceling immediately.														
		
		if (continueFlag)
			InitializeGlobalAlertVariables (kFinishDontCancelAlertID, 
													kAlertStrID, 
													IDS_Alert49);
		
				// Do the clustering if everything is okay to here.															
				
		if (continueFlag)
   		continueFlag = (ISODATACluster (fileIOInstructionsPtr) >= 0);
												
				// Reset the cancel operation globals.	
													
		ClearGlobalAlertVariables ();
   	
   			// Get the cluster standard deviations.									
   			
   	if (continueFlag)
   		continueFlag = UpdateClusterStdDeviations (gClusterSpecsPtr->clusterHead);

   			// List final cluster statistics. 											
   	
				// "\rFinal cluster class statistics.\r"									
						
		continueFlag = ListSpecifiedStringNumber (
										kClusterStrID, 
										IDS_Cluster14, 
										(unsigned char*)gTextString, 
										resultsFileStreamPtr, 
										gOutputForce1Code,
										continueFlag);
										
   	if (continueFlag)
			continueFlag = ListClusterStatistics (
									resultsFileStreamPtr, &gOutputForce1Code, TRUE);
   	
   			// Set up the cluster class to final class vector if needed.		
   			
   	if (continueFlag)
			continueFlag = SetUpClassToFinalClassPtr ();
   	
   			// Create a cluster classification map if requested.					
   	
   	if (continueFlag && gClusterSpecsPtr->classificationArea != 0)
			{
					// Put the label for final number of clusters in the status 	
					// dialog.																		
					
			LoadDItemStringNumber (kClusterStrID, 
											IDS_Cluster15, 
											gStatusDialogPtr, 
											IDC_Status15, 
											(Str255*)&gTextString);
			
					// Get the memory for the classification buffer.					
				
			classifyBufferPtr = (HUCharPtr)MNewPointerClear (
						gImageWindowInfoPtr->maxNumberColumns + 
														gNumberOfEndOfLineCharacters + 1);
			continueFlag = (classifyBufferPtr != NULL);
				
			if (continueFlag)
				{
   			continueFlag = ClusterClassification (
   											fileIOInstructionsPtr, 
   											gClusterSpecsPtr->numberFinalClusters, 
   											classifyBufferPtr);
   										
   					// If user cancelled the cluster classification. Allow the
   					// statistics to be saved if requested in case the user still
   					// wants them.
   					
   			if (gOperationCanceledFlag)
   				{
   				gOperationCanceledFlag = FALSE;
   				continueFlag = TRUE;
   				
   				}	// end "if (gOperationCanceledFlag)"
   				
   			}	// end "if (continueFlag)"
	  	
			 		// Release memory for the classification buffer.					
			
			classifyBufferPtr = CheckAndDisposePtr (classifyBufferPtr);
   									
   		}	// end "if (continueFlag && ..." 
   		
   			// Create a cluster mask file if requested.
   			
   	if (continueFlag && (gClusterSpecsPtr->outputStorageType & kClusterMaskCode))
   		continueFlag = CreateClusterMaskFile ();
   	
				// Save the statistics if requested.										
			
	   if (continueFlag)
	   	{
			continueFlag = SaveClusterStatistics (fileIOInstructionsPtr, 
																	gClusterSpecsPtr->numberFinalClusters);
									
   		if (gOperationCanceledFlag)
   			gOperationCanceledFlag = FALSE;
									
			}	// end "if (continueFlag)"
		  	
		 		// Release memory for the pixel cluster class assignments.			
		 
		gClusterSpecsPtr->dataClassPtr = CheckAndDisposePtr (
															gClusterSpecsPtr->dataClassPtr);
			
				// Free memory that the clusters used.										
		
   	if (gClusterSpecsPtr->clusterMemoryHead != NULL)
   		{
   				// Free memory for eigenvector cluster centers.						
   				
			gClusterSpecsPtr->clusterMemoryHead->meanPtr = 
						(CMeanType*)CheckAndDisposePtr (
									(Ptr)gClusterSpecsPtr->clusterMemoryHead->meanPtr);

			gClusterSpecsPtr->clusterMemoryHead->sumPtr = 
						CheckAndDisposePtr (
									gClusterSpecsPtr->clusterMemoryHead->sumPtr);

			gClusterSpecsPtr->clusterMemoryHead = 
						(ClusterType*)CheckAndDisposePtr (
									(Ptr)gClusterSpecsPtr->clusterMemoryHead);

   		gClusterSpecsPtr->clusterHead = NULL;
   		
   		}	// end "while (gClusterSpecsPtr->clusterMemoryHead != NULL)" 
		
		else	// gClusterSpecsPtr->clusterMemoryHead == NULL 
			{
					// Free memory for one pass cluster centers.							
					
   		currentCluster = gClusterSpecsPtr->clusterHead;
   		while (currentCluster != NULL)
   			{
				lastCluster = currentCluster;
      		currentCluster = currentCluster->next;
				lastCluster = (ClusterType*)CheckAndDisposePtr ((Ptr)lastCluster);
   		
   			}	// end "while (currentCluster != NULL)" 
   			
   		gClusterSpecsPtr->clusterHead = NULL;
   		
   		}	// end "else	// gClusterSpecsPtr->clusterMemoryHead == NULL" 
   		
   	}	// end "if (continueFlag)" 
				
			// Dispose of the IO buffer.		
								
	DisposeIOBufferPointers (fileIOInstructionsPtr,
										&gInputBufferPtr, 
										&gOutputBufferPtr);
	
	gClusterSpecsPtr->clusterClassToFinalClassPtr = 
						CheckAndDisposePtr (
								gClusterSpecsPtr->clusterClassToFinalClassPtr);
	
	return (TRUE);
   
}     // end Function "ISODATAClusterControl" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ISODATAClusterDialog
//
//	Software purpose:	The purpose of this routine is to handle the 		
//							modal dialog for selecting the ISODATA cluster
//							specifications. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			ClusterDialog   in cluster.c
//
//	Coded By:			Larry L. Biehl			Date: 08/06/1990
//	Revised By:			Larry L. Biehl			Date: 12/16/2016	

Boolean ISODATAClusterDialog (
				FileInfoPtr							fileInfoPtr,
				DialogPtr							parentDialogPtr)

{
	Boolean								returnFlag;	
			
#if defined multispec_mac
								
	DialogSelectArea					dialogSelectArea;
	
	Rect									theBox;
   
	double								convergence,
											criticalDistance1,
											criticalDistance2,
											saveConvergence;
	
	DialogPtr							dialogPtr;
	
	UInt16								*localClassAreaPtr = NULL;
	
	Handle								okHandle,
											theHandle;
	
	SInt32								maxNumberClusters,
											minClusterSize,
											theNum;	
					
	UInt32								localNumberClassAreas;
	
	SInt16								clustersFrom,
											initializationOption,
											itemHit,
											savedProjectClassMeansCode,
											theType;
											
	UInt16								distanceDecimalDigits;
	
	Boolean								modalDone,
											savedUseCorrelationMatrixFlag;
	
	
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
		dialogPtr = LoadRequestedDialog (kISODATADialogID, kCopyScrap, 1, 2);
		
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
			
//	SetDialogItemDrawRoutine (dialogPtr, 13, gDrawDialogClassPopUpPtr);

	ISODATAClusterDialogInitialize (dialogPtr,
													localClassAreaPtr,
													&initializationOption,
													&savedUseCorrelationMatrixFlag,
													&savedProjectClassMeansCode,
													&clustersFrom,
													&gClassSelection,
													&localNumberClassAreas,
													&dialogSelectArea,
													&maxNumberClusters,
													&saveConvergence,
													&minClusterSize,
													&criticalDistance1,
													&criticalDistance2,
													&distanceDecimalDigits);
											
			// Save handle for the OK button for use later.								
			
	GetDialogItem (dialogPtr, 1,  &theType, &okHandle, &theBox);
	
			// Initialization options.															
			
	SetDLogControl (dialogPtr, 4, initializationOption==1);
	SetDLogControl (dialogPtr, 5, initializationOption==2);
	SetDLogControl (dialogPtr, 6, initializationOption==3);
	
			// Equalize variances (use correlation matrix)
		
	SetDLogControl (dialogPtr, 7, savedUseCorrelationMatrixFlag);
	
			// Use current project class means
			
	SetDLogControl (dialogPtr, 8, savedProjectClassMeansCode==1);
	
			// Training areas.																	
	
	SetDLogControl (dialogPtr, 10, clustersFrom==kTrainingType);
	
			// Selected Image area.																
			
	SetDLogControl (dialogPtr, 11, clustersFrom==kAreaType);
																		
			//	Set the draw routine for the class popup box.
			
	SetDialogItemDrawRoutine (dialogPtr, 
										&theBox,
										12,
										13,
										NULL,
										&gDrawDialogClassPopUpPtr,
										37,
										gPopUpAllSubsetMenu,
										gClassSelection); 
		
			//	Load default NumberClusters.													
	
	LoadDItemValue (dialogPtr, 26, maxNumberClusters);
			
			//	Load default Convergence.			
	
	LoadDItemRealValue (dialogPtr, 28, saveConvergence, 1);
		
			//	Load default Minimum Cluster Size.											
	
	LoadDItemValue (dialogPtr, 30, minClusterSize);
		
			//	Load default Critical Distances 1 & 2.										
	
	LoadDItemRealValue (dialogPtr, 32, criticalDistance1, distanceDecimalDigits);
	LoadDItemRealValue (dialogPtr, 34, criticalDistance2, distanceDecimalDigits);
				
			// Center the dialog and then show it.											
			
	ShowDialogWindow (dialogPtr, kISODATADialogID, kSetUpDFilterTable);
		
	gDialogItemDescriptorPtr[28] = kDItemReal;
	gDialogItemDescriptorPtr[32] = kDItemReal;
	gDialogItemDescriptorPtr[34] = kDItemReal; 
	
			// Set default text selection to first edit text item						
			
	if (initializationOption != 3)
		SelectDialogItemText (dialogPtr, 26, 0, INT16_MAX);
		
	else	// initializationOption == 3 
		SelectDialogItemText (dialogPtr, 28, 0, INT16_MAX);
		
	
	modalDone = FALSE;
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
				case 4:
				case 5:
				case 6:
					initializationOption = itemHit - 3;
					SetDLogControl (dialogPtr, 4, (itemHit==4));
					SetDLogControl (dialogPtr, 5, (itemHit==5));
					SetDLogControl (dialogPtr, 6, (itemHit==6));
	
					if (itemHit == 6)
						{
						HideDialogItems (dialogPtr, 25, 26);
						ShowDialogItems (dialogPtr, 31, 34);
						SetDLogControlHilite (dialogPtr, 7, 255);
						SetDLogControl (dialogPtr, 7, 0);
						SetDLogControlHilite (dialogPtr, 8, 255);
						SetDLogControl (dialogPtr, 8, 0);
						
						}	// end "if (itemHit == 6)" 
						
					else	// itemHit != 6 
						{
						ShowDialogItems (dialogPtr, 25, 26);
						HideDialogItems (dialogPtr, 31, 34);
						SetDLogControlHilite (dialogPtr, 7, 0);	
						SetDLogControl (dialogPtr, 7, savedUseCorrelationMatrixFlag);
						SetDLogControlHilite (dialogPtr, 8, 0);	
						SetDLogControl (dialogPtr, 8, savedProjectClassMeansCode==1);
						
						}	// end "else itemHit != 6" 
					break;
					
				case 7:
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					savedUseCorrelationMatrixFlag = !savedUseCorrelationMatrixFlag;
					break;
					
				case 8:
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					savedProjectClassMeansCode = GetDLogControl (dialogPtr, 8);
					break;
					
				case 10:		// Clusters from training fields 
				
					clustersFrom = kTrainingType;
					SetControlValue ((ControlHandle)theHandle, 1);
					SetDLogControl (dialogPtr, 11, 0);
					
					ISODATAClusterDialogOnTrainingAreas (dialogPtr);
					break;
					
				case 11:		// Clusters from selected image area. 
				
					clustersFrom = kAreaType;
					SetControlValue ((ControlHandle)theHandle, 1);
					SetDLogControl (dialogPtr, 10, 0);
					
					ISODATAClusterDialogOnImageArea (dialogPtr);
					break;
					
				case 13:		// Classes 
				case 37:		// Classes 
					
					if (gAppearanceManagerFlag)
						itemHit = GetDItemValue (dialogPtr, 37);
						
					else	// !gAppearanceManagerFlag)
						{
						itemHit = StandardPopUpMenu (dialogPtr, 
																12, 
																13, 
																gPopUpAllSubsetMenu, 
																gClassSelection, 
																kPopUpAllSubsetMenuID);
															
						}	// end "else !gAppearanceManagerFlag)"
													
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
							
						if (itemHit == kAllMenuItem && gAppearanceManagerFlag)
							SetDItemValue (dialogPtr, 37, kAllMenuItem);
								
								// Force the main dialog box to be redrawn.
								
						InvalWindowRect (GetDialogWindow (dialogPtr), 
											GetPortBounds (GetDialogPort (dialogPtr), &gTempRect));
							
						}	// end "if (itemHit == kSubsetMenuItem)" 
					
					if (itemHit != 0)
						gClassSelection = itemHit;
	
							// Make certain that the correct label is drawn in the	
							// class pop up box.													
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
					
				case 16:				// Entire area to selected area switch.			
				case 18:				//	 cluster from lineStart  
				case 19:				//	 cluster from lineEnd  
				case 20:				//	 cluster from lineInterval  
				case 21:				//	 cluster from columnStart  
				case 22:				//	 cluster from columnEnd  
				case 23:				//	 cluster from columnInterval  
				case 36:				// Entire area to selected area switch.		

					DialogLineColumnHits (&dialogSelectArea,
													dialogPtr, 
													itemHit,
													theHandle,
													theNum);
					break;
					
				case 26:				//	 Number Clusters  
//				   if (theNum == 0 || theNum >= kMaxNumberStatClasses)	
//						NumberErrorAlert ((SInt32)gClusterSpecsPtr->maxNumberClusters,
//						 							dialogPtr, 
//						 							itemHit);
					break;
					
				case 28:				//	 Convergence 
					convergence = GetDItemRealValue (dialogPtr, 28);
					if (convergence > kMaxThreshold)
						RealNumberErrorAlert (saveConvergence, dialogPtr, 28, 1);
					if (convergence > 0 && convergence <= kMaxThreshold)
						saveConvergence = convergence;
					break;
					
				case 30:				//	 Minimum cluster size  
//				   if (theNum == 0 || theNum > LONG_MAX)	
//						NumberErrorAlert ((SInt32)gClusterSpecsPtr->minClusterSize,
//						 							dialogPtr, 
//						 							itemHit);
					break;
					
				case 32:				//	 Critical distance 1  
//				   if (theNum == 0 || theNum > LONG_MAX)	
//						NumberErrorAlert (gClusterSpecsPtr->criticalDistance1,
//						 							dialogPtr, 
//						 							itemHit);
					break;
					
				case 34:				//	 Critical distance 2 
//				   if (theNum == 0 || theNum > LONG_MAX)	
//						NumberErrorAlert (gClusterSpecsPtr->criticalDistance2,
//						 							dialogPtr, 
//						 							itemHit);
					break;
						
				}	// end "switch (itemHit)" 
				
			}	// end "if (itemHit > 2)" 
			
		else if (itemHit > 0) 	// and itemHit <= 2
			{
					// Check number clusters
					
			if (itemHit == 1 && initializationOption != 3)
				itemHit = CheckMaxValue (dialogPtr, 
													26, 
													1,
													kMaxNumberStatClasses, 
													kDisplayRangeAlert);
			
					// Check minimum cluster size
					
			if (itemHit == 1)
				itemHit = CheckMaxValue (dialogPtr, 
													30, 
													1,
													LONG_MAX, 
													kDisplayRangeAlert);
			
					// Check critical distance 1
					
			if (itemHit == 1 && initializationOption == 3)
				itemHit = CheckMaxRealValue (dialogPtr, 
													32,
													ULONG_MAX, 
													kZeroNotAllowed, 
													kDisplayRangeAlert);
			
					// Check critical distance 2
					
			if (itemHit == 1 && initializationOption == 3)
				itemHit = CheckMaxRealValue (dialogPtr, 
													34, 
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
			
					// If item hit is 1 and the area for clustering is a selected
					// area, verify that the number of pixels selected is not more
					// than the current limit. There may still be too many but that
					// message will be displayed when the memory request is attempted.
					// This check is in place to allow the user to know what the absolute
					// maximum is.
			
			itemHit = ISODATAClusterDialogCheckNumberPixels (itemHit,
																				clustersFrom,
																				&dialogSelectArea); 			
				
			if	(itemHit == 1)      // User selected OK for information 
				{
				modalDone = TRUE;
				returnFlag = TRUE;

				ISODATAClusterDialogOK (initializationOption,
													(GetDLogControl (dialogPtr, 7)==1),
													(GetDLogControl (dialogPtr, 8)==1),
													clustersFrom,
													gClassSelection,
													localClassAreaPtr,
													localNumberClassAreas,
													&dialogSelectArea,
													GetDItemValue (dialogPtr, 26),
													saveConvergence,
													GetDItemValue (dialogPtr, 30),
													GetDItemRealValue (dialogPtr, 32),
													GetDItemRealValue (dialogPtr, 34));
													
				}	// end "if (itemHit == 1)" 
				
			if	(itemHit == 2)      // User selected Cancel for information 
				{
				modalDone = TRUE;
				returnFlag = FALSE;
				
				}	// end "if	(itemHit == 2)" 
			
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
	
	UnSetDialogItemDrawRoutine (dialogPtr, 
											NULL, 
											37,
											gPopUpAllSubsetMenu);
		
	CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
#endif	// defined multispec_mac
									
	#if defined multispec_win                  
		CMISODATAClusterDialog*		dialogPtr = NULL;
		
		TRY
			{ 
			dialogPtr = new CMISODATAClusterDialog (); 
			
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
		CMISODATAClusterDialog* dialogPtr = NULL;

		//dialogPtr = new CMISODATAClusterDialog (wxTheApp->GetTopWindow ());
		dialogPtr = new CMISODATAClusterDialog (parentDialogPtr);

		returnFlag = dialogPtr->DoDialog ();

		delete dialogPtr;
	#endif	// defined multispec_lin
	
	return (returnFlag);
	
}	// end "ISODATAClusterDialog" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ISODATAClusterDialogInitialize
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
//	Revised By:			Larry L. Biehl			Date: 09/05/2012	

void ISODATAClusterDialogInitialize (
				DialogPtr							dialogPtr,
				UInt16*								localClassAreaPtr,
				SInt16*								initializationOptionPtr,
				Boolean*								useCorrelationMatrixFlagPtr,
				SInt16*								projectClassMeansCodePtr,
				SInt16*								clustersFromPtr,
				SInt16*								classAreaSelectionPtr,
				UInt32*								numberClassAreasPtr,
				DialogSelectArea*					dialogSelectAreaPtr,
				SInt32*								maxNumberClustersPtr,
				double*								convergencePtr,
				SInt32*								minClusterSizePtr,
				double*								criticalDistance1Ptr,
				double*								criticalDistance2Ptr,
				UInt16*								distanceDecimalDigitsPtr)

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
	
			// Initialization options.				
	
	*initializationOptionPtr = gClusterSpecsPtr->initializationOption;
	
			// Equalize variances (use correlation matrix)
	
	*useCorrelationMatrixFlagPtr = gClusterSpecsPtr->useCorrelationMatrixFlag;
	
	#if defined multispec_mac 
		HideDialogItem (dialogPtr, 7);
		if (*initializationOptionPtr == 3)
			SetDLogControlHilite (dialogPtr, 7, 255);
	#endif	// defined multispec_mac 
	
			// Use current project class means
			
	*projectClassMeansCodePtr = gClusterSpecsPtr->projectClassMeansCode;
	
	if (gProjectInfoPtr->numberStatTrainClasses <= 0)
		HideDialogItem (dialogPtr, IDC_ProjectClassMeans);
	
	if (gClusterSpecsPtr->initializationOption == 3)
		SetDLogControlHilite (dialogPtr, IDC_ProjectClassMeans, 255);
	
			// Training areas.																	
	
	*clustersFromPtr = gClusterSpecsPtr->clustersFrom;
	                          
	SInt16	lastField	= -1;		
	fieldsExistCode = GetNextFieldArea (
										gProjectInfoPtr,
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
			
	SInt16 entireIconItem = 16;
	if (gAppearanceManagerFlag)
		entireIconItem = 36;
	
	InitializeDialogSelectArea (dialogSelectAreaPtr,
											gImageWindowInfoPtr,
											gProjectSelectionWindow,
											gClusterSpecsPtr->clusterColumnStart,
											gClusterSpecsPtr->clusterColumnEnd,
											gClusterSpecsPtr->clusterColumnInterval,
											gClusterSpecsPtr->clusterLineStart,
											gClusterSpecsPtr->clusterLineEnd,
											gClusterSpecsPtr->clusterLineInterval,
											18,
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
			HideDialogItem (dialogPtr, 16);
			
		else	// !gAppearanceManagerFlag
			HideDialogItem (dialogPtr, 36);
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
			
		ISODATAClusterDialogOnTrainingAreas (dialogPtr);
		
		}	// end "if (*clustersFromPtr == ...)" 
		
	else	// *clustersFromPtr != kTrainingType 
		{
				// If clusters from selection area, then only allow line and 		
				// column start, end and intervals to show for definition.			
			 
		ISODATAClusterDialogOnImageArea (dialogPtr);
		
		}	// end "else *clustersFromPtr != ..." 
		
			//	Load default NumberClusters.													
	
	*maxNumberClustersPtr = gClusterSpecsPtr->maxNumberClusters;
	if (gClusterSpecsPtr->initializationOption == 3)
		{
		HideDialogItem (dialogPtr, IDC_NumberClustersPrompt);
		HideDialogItem (dialogPtr, IDC_NumberClusters);
		
		}	// end "if (gClusterSpecsPtr->initializationOption == 3)"
			
			//	Load default Convergence.														
	
	*convergencePtr = gClusterSpecsPtr->convergence;
		
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
	
	if (gClusterSpecsPtr->initializationOption != 3)
		{
		HideDialogItem (dialogPtr, IDC_Distance1Prompt);
		HideDialogItem (dialogPtr, IDC_Distance1);
		HideDialogItem (dialogPtr, IDC_Distance2Prompt);
		HideDialogItem (dialogPtr, IDC_Distance2);
		
		}	// end "if (gClusterSpecsPtr->initializationOption != 3)"
	
}	// end "ISODATAClusterDialogInitialize"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ISODATAClusterDialogCheckNumberPixels
//
//	Software purpose:	The purpose of this routine is to verify that the number
//							of pixels to be clustered are within the absolute limits
//							of what is possible relative to the amount of memory that
//							can be requested. Being less than the limit though does
//							not guarantee that the memory will be available when needed.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			ClusterDialog   in SCluster.cpp
//
//	Coded By:			Larry L. Biehl			Date: 08/25/2010
//	Revised By:			Larry L. Biehl			Date: 08/25/2010	

SInt16 ISODATAClusterDialogCheckNumberPixels (
				SInt16								itemHit,
				SInt16								clustersFrom,
				DialogSelectArea*					dialogSelectAreaPtr)

{
	SInt64								limit,
											numberPixels;
	
	
	if (itemHit == 1 && clustersFrom == kAreaType)
		{
		numberPixels = GetNumberOfSelectedPixels (dialogSelectAreaPtr);
		limit = SInt32_MAX/sizeof (SInt16);

		if (numberPixels > limit)
			{
			itemHit = DisplayAlert (
								kErrorAlertID, 1, kClusterStrID, IDS_Cluster51, 0, NULL);
			itemHit = 0;
			
			}	// end "if (numberPixels > limit)"
		
		}	// end "if (itemHit == 1 && clustersFrom == kAreaType)"
	
	return (itemHit);

}	// end "ISODATAClusterDialogCheckNumberPixels"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ISODATAClusterDialogOK
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
//	Coded By:			Larry L. Biehl			Date: 03/24/1999
//	Revised By:			Larry L. Biehl			Date: 03/24/1999	

void ISODATAClusterDialogOK (
				SInt16								initializationOption,
				Boolean								useCorrelationMatrixFlag,
				Boolean								projectClassMeansFlag,
				SInt16								clustersFrom,
				SInt16								classAreaSelection,
				UInt16*								localClassAreaPtr,
				UInt32								localNumberClassAreas,
				DialogSelectArea*					dialogSelectAreaPtr,
				SInt16								maxNumberClusters,
				double								convergence,
				SInt16								minClusterSize,
				double								criticalDistance1,
				double								criticalDistance2)

{
	SInt16								*classAreaPtr;
	
	UInt32								index;
	
					
			// Initialization options.					
						
	gClusterSpecsPtr->initializationOption = initializationOption;
		
			// User correlation matrix flag
			
	gClusterSpecsPtr->useCorrelationMatrixFlag = useCorrelationMatrixFlag;
		
			// Include project class means
			
	gClusterSpecsPtr->projectClassMeansCode = 0;
	if (projectClassMeansFlag)
		gClusterSpecsPtr->projectClassMeansCode = 1;
		
			// Clusters from definitions.						
					
	gClusterSpecsPtr->clustersFrom = clustersFrom;
		
			// Cluster class areas.											
	
	gClusterSpecsPtr->clusterClassSet = classAreaSelection;
	if (gClusterSpecsPtr->clustersFrom == kTrainingType)																
		{
		classAreaPtr = (SInt16*)GetHandlePointer (
												gClusterSpecsPtr->clusterClassHandle);

		if (classAreaSelection == kAllMenuItem)		// All classes 
			LoadClassAreaVector (&gClusterSpecsPtr->numberClusterClasses,
											classAreaPtr); 
											
		else	// classAreaSelection == kSubsetMenuItem
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
		
			// Number Clusters.											

	gClusterSpecsPtr->maxNumberClusters = maxNumberClusters;
		
			// Convergence.												

	gClusterSpecsPtr->convergence = (float)convergence;
										
			// Minimum cluster size.									

	gClusterSpecsPtr->minClusterSize = minClusterSize;
		
			// Critical distance 1.										

	gClusterSpecsPtr->criticalDistance1 = criticalDistance1;
		
			// Critical distance 2.										

	gClusterSpecsPtr->criticalDistance2 = criticalDistance2;
	
}	// end "ISODATAClusterDialogOK"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ISODATAClusterDialogOnImageArea
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
//	Coded By:			Larry L. Biehl			Date: 03/24/1999
//	Revised By:			Larry L. Biehl			Date: 03/24/1999	

void ISODATAClusterDialogOnImageArea (
				DialogPtr							dialogPtr)
				
{                                               
	                                                   
	ShowDialogItem (dialogPtr, IDC_LineStart);
	ShowDialogItem (dialogPtr, IDC_LineEnd);  
	ShowDialogItem (dialogPtr, IDC_ColumnStart);  
	ShowDialogItem (dialogPtr, IDC_ColumnEnd); 
									
	#if defined multispec_mac  
		HideDialogItem (dialogPtr, IDC_ClassPrompt); 
		HideDialogItem (dialogPtr, IDC_ClassCombo);
		HideDialogItem (dialogPtr, 37);
			
		ShowDialogItem (dialogPtr, 14);
		ShowDialogItem (dialogPtr, 15);
		
		if (gAppearanceManagerFlag)
			ShowDialogItem (dialogPtr, 36);
			
		else	// !gAppearanceManagerFlag
			ShowDialogItem (dialogPtr, 16); 
	#endif	// defined multispec_mac
									
	#if defined multispec_win || defined multispec_lin         
		HideDialogItem (dialogPtr, IDC_ClassPrompt); 
		HideDialogItem (dialogPtr, IDC_ClassCombo);
	#endif	// defined multispec_win || defined multispec_lin      
		
}	// end "ISODATAClusterDialogOnImageArea" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ISODATAClusterDialogOnTrainingAreas
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
//	Coded By:			Larry L. Biehl			Date: 03/24/1999
//	Revised By:			Larry L. Biehl			Date: 03/01/2002	

void ISODATAClusterDialogOnTrainingAreas (
				DialogPtr									dialogPtr)
				
{                                                          
	HideDialogItem (dialogPtr, IDC_LineStart);
	HideDialogItem (dialogPtr, IDC_LineEnd);  
	HideDialogItem (dialogPtr, IDC_ColumnStart);  
	HideDialogItem (dialogPtr, IDC_ColumnEnd); 
									
	#if defined multispec_mac  
		if (gAppearanceManagerFlag)
			{
			HideDialogItem (dialogPtr, IDC_ClassPrompt); 
			HideDialogItem (dialogPtr, IDC_ClassCombo);
			ShowDialogItem (dialogPtr, 37);
			
			}	// end "if (gAppearanceManagerFlag)" 
			
		else	// !gAppearanceManagerFlag
			{       
			ShowDialogItem (dialogPtr, IDC_ClassPrompt); 
			ShowDialogItem (dialogPtr, IDC_ClassCombo);
			HideDialogItem (dialogPtr, 37);
	
			}	// end "else !gAppearanceManagerFlag"
			
		HideDialogItem (dialogPtr, 14);
		HideDialogItem (dialogPtr, 15);
		HideDialogItem (dialogPtr, 16);
		HideDialogItem (dialogPtr, 36);
	#endif	// defined multispec_mac
									
	#if defined multispec_win || defined multispec_lin       
		ShowDialogItem (dialogPtr, IDC_ClassPrompt); 
		ShowDialogItem (dialogPtr, IDC_ClassCombo);
		
		HideDialogItem (dialogPtr, IDEntireImage);
		HideDialogItem (dialogPtr, IDSelectedImage);
	#endif	// defined multispec_win || defined multispec_lin           
	
}	// end "ISODATAClusterDialogOnTrainingAreas" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ISODATAClusterPass
//
//	Software purpose:	The purpose of this routine is to classify the
//							input pixels into the nearest cluster class and
//							note the number of class changes.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/06/1990
//	Revised By:			Larry L. Biehl			Date: 05/21/2018	

SInt16 ISODATAClusterPass (
				FileIOInstructionsPtr			fileIOInstructionsPtr, 
				Ptr*									clusterAddressesPtr, 
				SInt64*								numberClassChanges, 
				Boolean								firstPassFlag)

{
			// Define local structures and variables.
			
	ldiv_t								lDivideStruct; 
   
	double								closestDistance,	// Distance from pix to  		
																	// closest Cluster.				
											currentDistance,	// Distance from pix to  		
																	//	current Cluster.				
											minutesLeft;
	
   ClusterType 						*closestCluster,	// cluster closest to current 	
   																// pixel.
											*clusterHead, 								
  											*currentCluster;	// Cluster currently working on.
  											
   Point									point;
									
   			
	HCTypePtr     						currentPixel,		// Vector pointer for current 	
																	//	pixel.
   			  							outputBufferPtr;	// ptr to all pixels.
   			  					
	HUInt16Ptr							dataClassPtr,
											savedDataClassPtr;
											
	HPtr									offScreenBufferPtr;	
											
	ImageOverlayInfoPtr				imageOverlayInfoPtr;
	Ptr									stringPtr;
	UInt16*								channelsPtr;
	WindowPtr							windowPtr;
	
	Handle								activeImageWindowInfoHandle;
	RgnHandle							rgnHandle;
	
   SInt32	        					line,
											lineCount,
											lineEnd,
											lineInterval,
											linesLeft,
				 							lineStart,
											numberColumns,
											startTick;
	
	UInt32								column,				// Counting variable for columns 
 											columnEnd,
 											columnInterval,
							 				columnStart,
							 				columnWidth,
											firstColumn,
											numberSamples,
											sample;
	
   SInt16								areaNumber,
											errCode,
											fieldNumber,
											lastClassIndex,
											lastFieldIndex,
											numberChannels,
											returnCode,
											totalNumberAreas;										
   
   Boolean								polygonFieldFlag,
											pointType;		   
   

			// Initialize local variables. 													
   
	channelsPtr = 	(UInt16*)GetHandlePointer (
													gClusterSpecsPtr->channelsHandle);
								
	clusterHead = 			gClusterSpecsPtr->clusterHead;
	columnInterval = 		gClusterSpecsPtr->clusterColumnInterval;
	dataClassPtr = 		gClusterSpecsPtr->dataClassPtr;
	lineInterval = 		gClusterSpecsPtr->clusterLineInterval;
	totalNumberAreas = 	gClusterSpecsPtr->totalNumberAreas;
   numberChannels = 		gClusterSpecsPtr->numberChannels;
   outputBufferPtr = 	(CType*)gOutputBufferPtr;
	fieldNumber = 0;
	lastClassIndex = -1;
	lastFieldIndex = -1;
  	returnCode = 1;
	activeImageWindowInfoHandle = FindProjectBaseImageWindowInfoHandle ();
	windowPtr = GetWindowPtr (activeImageWindowInfoHandle);
	
	imageOverlayInfoPtr = GetImageOverlayInfoPtr (
										gClusterSpecsPtr->imageOverlayIndex,
										kLock,
										NULL);
					
			// Intialize the nextTime variable to indicate when the next 			
			// check should occur for a command-.											
			
	gNextTime = TickCount ();
	
			// Save the pointer to the cluster class assignments in case it is needed
			// for the offscreen buffer assignments.
			
	savedDataClassPtr = dataClassPtr;
	
	offScreenBufferPtr = GetImageOverlayOffscreenPointer (imageOverlayInfoPtr);
		
			// Loop by number of cluster areas.												
			
	for (areaNumber=1; areaNumber<=totalNumberAreas; areaNumber++)
		{
				// Initialize status variables.												
				
		lineCount = 0;
		gNextStatusTime = TickCount ();
		
				// Get information for next cluster area.									
				
		if (!GetNextClusterArea (gProjectInfoPtr, 
										channelsPtr, 
										numberChannels, 
										areaNumber, 
										&gNextMinutesLeftTime, 
										&lastClassIndex, 
										&lastFieldIndex, 
										&linesLeft))
//										&BILSpecialFlag))
																							return (-1);
			
		lineStart = gAreaDescription.lineStart;
		lineEnd = gAreaDescription.lineEnd;
  		columnStart = gAreaDescription.columnStart;
   	columnEnd = gAreaDescription.columnEnd;
		columnWidth = columnEnd - columnStart + 1;
		polygonFieldFlag = gAreaDescription.polygonFieldFlag;
		rgnHandle = gAreaDescription.rgnHandle;
		pointType =  gAreaDescription.pointType;
			
				// Get first sample.																
	   	
		firstColumn = columnStart;
											
				// Check if first column will change from line to line.				
		
		numberColumns = (columnEnd - columnStart + columnInterval)/columnInterval;
		
				// Initialize minutes left variable variables.
				// The first time left message will be after 1 second.
		
		startTick = TickCount ();		
		gNextMinutesLeftTime = startTick + 3*gNextStatusTimeOffset;
			
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
					// Update dialog status information.									
				
			lineCount++;
			if (TickCount () >= gNextStatusTime)
				{
				LoadDItemValue (gStatusDialogPtr, IDC_Status8, lineCount);
				gNextStatusTime = TickCount () + gNextStatusTimeOffset;
				
				}	// end "if (TickCount () >= gNextStatusTime)" 
			
					// Exit routine if user has "command period" down				
			
			if (TickCount () >= gNextTime)
				{
				if (gOutputCode & kCreateImageOverlayCode && line > lineStart)
					{
					InvalidateWindow (windowPtr, kImageArea, FALSE);

					#if defined multispec_win  
						windowPtr->UpdateWindow ();
					#endif	// defined multispec_win  
					 
					#if defined multispec_lin
						gActiveImageWindow->OnUpdate (NULL, NULL);
						//(gActiveImageWindow->m_Canvas)->Update ();
					#endif

					}	// end "if (gOutputCode & kCreateImageOverlayCode && ..."
					
				if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
					{
					returnCode = -1;
					break;
					
					}	// end "if (!CheckSomeEvents (..."
					
				}	// end "if (TickCount () >= nextTime)" 
				
			column = firstColumn;
									
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
				returnCode = -1;
				break;	
																								
				}	// end "if (errCode < noErr)"
			
			if (errCode != kSkipLine)
				{
		     	currentPixel = outputBufferPtr;
		     	
				if (pointType == kMaskType)
					numberSamples = fileIOInstructionsPtr->numberOutputBufferSamples;
					
				else	// pointType != kMaskType
					numberSamples = (columnEnd - firstColumn + columnInterval)/columnInterval;
		   		
		   	for (sample=1; sample<=numberSamples; sample++)
					{									
							// Set horizotal (column) point in case it is needed for			
							// polygonal field.	

				   point.h = (SInt16)column;
				   
					if (!polygonFieldFlag || PtInRgn (point, rgnHandle))
		   			{
		      		closestCluster = clusterHead;
		      		currentCluster = clusterHead->next;
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
				     		
				     			// Update the cluster class statistics if needed.			
				     		
				     	if (*dataClassPtr != (UInt16)closestCluster->clusterNumber)
				     		{
				         AddClusterStats (currentPixel, closestCluster);
				         
				         if (!firstPassFlag)
				         	{
				         	currentCluster = 
				         				(ClusterType*)clusterAddressesPtr[*dataClassPtr];
				         	SubtractClusterStats (currentPixel, currentCluster);
				         	
				         	}	// end "if (!firstPassFlag)" 
					      		
					      *dataClassPtr = closestCluster->clusterNumber;
					      
					      (*numberClassChanges)++;
					      
				         }	// end "if (*dataClassPtr != ..." 
				         
				         	// Ready for next pixel.											
				         	
						dataClassPtr++;
				         
				      }	// end "if (!polygonFieldFlag || PtInRgn (point, rgnHandle))" 
						
					currentPixel += numberChannels;													
							
				   column += columnInterval;
		         
		  			}	// end "for (sample=1; sample<=numberSamples; sample++)"
		  			
				if (gOutputCode & kCreateImageOverlayCode)
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
													
					savedDataClassPtr = dataClassPtr;
					
					}	// end "if (gOutputCode & kCreateImageOverlayCode)"
		  			
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
			
			linesLeft--;
			if (TickCount () >= gNextMinutesLeftTime)
				{
				minutesLeft = (linesLeft * (TickCount () - startTick))/
																(double)(lineCount*kTicksPerMinute);
				
				sprintf ((char*)gTextString, " %.1f", minutesLeft);
				stringPtr = (char*)CtoPstring (gTextString, gTextString);
				LoadDItemString (gStatusDialogPtr, IDC_Status14, (Str255*)gTextString);
					
				gNextMinutesLeftTime = TickCount () + gNextMinutesLeftTimeOffset;
				
				}	// end "if (TickCount () >= nextMinutesLeftTime)" 
			
			if (pointType == kMaskType)
				fileIOInstructionsPtr->maskBufferPtr += 
												fileIOInstructionsPtr->numberMaskColumnsPerLine;
	  			
	  		}	// end "for (line=lineStart; line<=lineEnd; line+=lineInterval)"
	  	
	  			// Force overlay to be drawn if it has not been already.
	  				
		if (gOutputCode & kCreateImageOverlayCode)
			{
			InvalidateWindow (windowPtr, kImageArea, FALSE);

			#if defined multispec_win  
				windowPtr->UpdateWindow ();
			#endif	// defined multispec_win  
					 
			#if defined multispec_lin
				//gActiveImageWindow->OnUpdate (NULL, NULL);
				//(gActiveImageWindow->m_Canvas)->Update ();
			#endif
					
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
				{
				returnCode = -1;
				break;
				
				}	// end "if (!CheckSomeEvents (..."
				
			}	// end "if (gOutputCode & kCreateImageOverlayCode)"
	  	
	  	if (returnCode != -1)
			LoadDItemValue (gStatusDialogPtr, IDC_Status8, lineCount);
		
				// Close up any File IO Instructions structure that pertain to the 
				// specific area used for the list data.
				
		CloseUpFileIOInstructions (fileIOInstructionsPtr, &gAreaDescription);
						
				// Dispose of the region if it exists.						

		CloseUpAreaDescription (&gAreaDescription);
		
		if (returnCode == -1)
			break;
  			
  		}	// end "for (areaNumber=1; areaNumber<=totalNumberAreas; ...)" 

	UnlockImageOverlayOffscreenBuffer (imageOverlayInfoPtr);	
							
	if (!gOSXCoreGraphicsFlag)
		{
		WindowInfoPtr imageWindowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																	activeImageWindowInfoHandle);
		if (imageWindowInfoPtr != NULL)
			imageWindowInfoPtr->drawBaseImageFlag = TRUE;
		
		}	// end "if (!gOSXCoreGraphicsFlag)"

			// Make sure the latest status dialog box changes have been displayed.
	
	if (returnCode != -1)
		CheckSomeEvents (mUpMask);
			
	if (returnCode != -1 && gAlertReturnCode == 1)
		returnCode = 0;
  	
  	return (returnCode);
  		
}	// end "ISODATAClusterPass" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean UpdateClusterMeans
//
//	Software purpose:	The purpose of this routine is to update the means
//							for the clusters.  A new mean will be
//							computed based on the latest counts, sums.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			ISODATACluster in SClustID.cpp
//
//	Coded By:			Larry L. Biehl			Date: 08/10/1990
//	Revised By:			Larry L. Biehl			Date: 10/05/2000	

Boolean UpdateClusterMeans (
				ClusterType*						newCluster,
				SInt32*								activeNumberClustersPtr)

{
	HDoublePtr							clusterSumPtr;
	HCMeanTypePtr   					clusterMeanPtr;
	
	SInt32								numberPixels;
	
	SInt16								channel,
											numberChannels;
	
	
			// Check input variables.															
			
	if (gClusterSpecsPtr->clusterHead == NULL)
																					return (FALSE);
																					
	if (gClusterSpecsPtr->numberChannels <= 0)
																					return (FALSE);
																					
	*activeNumberClustersPtr = 0;
																					
			// Initialize local variables.													
			
	numberChannels = gClusterSpecsPtr->numberChannels;
	
	do
		{
		clusterMeanPtr = newCluster->meanPtr;
		clusterSumPtr = newCluster->sumPtr;
		numberPixels = newCluster->numPixels;
		
		if (numberPixels > 0)
			{
			for (channel=0; channel<numberChannels; channel++)
				{
				*clusterMeanPtr = *clusterSumPtr/(double)numberPixels;

			   clusterMeanPtr++;
			   clusterSumPtr++;
			   
			   }	// end "for (channel=0; channel< ..." 
			   
			(*activeNumberClustersPtr)++;
			   
			}	// end "if (numberPixels > 0)" 
		   
		newCluster = newCluster->next;
		
		}	while (newCluster != NULL);
		
	return (TRUE);
	
}	//  end "UpdateClusterMeans" 
