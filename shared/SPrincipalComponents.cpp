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
//	File:						SPrincipalComponents.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			10/19/2018
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	This file contains routines that allow the user to
//								run a principal components analysis of selected data
//								for a selected area or classes and selected channels 
//								to the output text window or a disk file.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"    

#if defined multispec_wx
	#include "xMultiSpec.h"
	#include "xImageView.h"
	#include "xPrincipalComponentsDialog.h"
#endif
  
#if defined multispec_win 
	#include "WPrincipalComponentsDialog.h"
#endif	// defined multispec_win 



		// Pointer to the Principal Components analysis information structure.	
		
PrincipalCompSpecsPtr		gPrincipalCompSpecsPtr;
								
	
	
			// Prototypes for routines in this file that are only called by		
			// other routines in this file.													

Boolean LoadPrincipalComponentSpecs (
				Handle								windowInfoHandle);

Boolean PrincipalComponentAnalysis (
				FileInfoPtr							fileInfoPtr);

Boolean PrincipalComponentDialog (
				FileInfoPtr							fileInfoPtr);



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean LoadPrincipalComponentSpecs
//
//	Software purpose:	The purpose of this routine is to load the Principal 
//							Component Specification structure with the default set of
//							parameters.
//
//	Parameters in:		.
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			PrincipalComponentControl
//
//	Coded By:			Larry L. Biehl			Date: 12/28/1990
//	Revised By:			Larry L. Biehl			Date: 03/19/1999

Boolean LoadPrincipalComponentSpecs (
				Handle								windowInfoHandle)
			
{
	SInt16								*channels,
											*classPtr,
											*featurePtr;
	
	SInt32								bytesNeeded;
	
	SInt16								index,
											lastClassIndex;
											
	Boolean								changedFlag,
											projectFlag,
											returnFlag;
	
	
			// Assume routine will work okay.  If there isn't enough memory		
			// then the flag will be changed.												
			
	returnFlag = TRUE;
	
			// Determine if information exists for at least one project field		
			// in one project class.															
			
	projectFlag = (gProjectInfoPtr != NULL);
	
	if (projectFlag)
		{
		lastClassIndex	= -1;			
		index = GetNextFieldArea (gProjectInfoPtr,
											NULL,
											gProjectInfoPtr->numberStatisticsClasses,
											&lastClassIndex,
											-1,
											kTrainingType,
											kIncludeClusterFields);
		
		if (index == -1)
			projectFlag = FALSE;
										
		}	// end "if (projectFlag)" 
		
			// If a principal component structure already exists, then check if 	
			// it is for the current window.  If not release the old structure 	
			// and initialize a new one.														
			
	if (gNonProjProcessorSpecs.principalCompSpecsH != NULL)
		{
				// Lock the handle to the principal component specifications and 	
				// get a pointer to the structure.											
				
		gPrincipalCompSpecsPtr = (PrincipalCompSpecsPtr)GetHandlePointer (
							gNonProjProcessorSpecs.principalCompSpecsH, kLock, kMoveHi);
							
		if (gPrincipalCompSpecsPtr->windowInfoHandle != windowInfoHandle || 
			gImageWindowInfoPtr->fileInfoVersion != 
													gPrincipalCompSpecsPtr->fileInfoVersion)
			{
			ReleasePrincipalCompSpecsMemory (
											&gNonProjProcessorSpecs.principalCompSpecsH);
			gPrincipalCompSpecsPtr = NULL;
			
			}	// end "if (gPrincipalCompSpecsPtr->fileInfoHandle != ..." 
			
		else	// gPrincipalCompSpecsPtr->fileInfoHandle == fileInfoHandle && ... 
			{
					// Make certain that number of channels make sense in case		
					// a project file was closed since the last time.					
					
			if (gPrincipalCompSpecsPtr->projectFlag && !projectFlag)
				{
				gPrincipalCompSpecsPtr->channelSet = kAllMenuItem;
				gPrincipalCompSpecsPtr->dataCode = kAreaType;
				
				}	// end "if (gPrincipalCompSpecsPtr->projectFlag && ..." 
				
			gPrincipalCompSpecsPtr->projectFlag = projectFlag;
				
			}	// end "else gPrincipalCompSpecsPtr->fileInfoHandle ..." 
				
		}	// end "else gNonProjProcessorSpecs.principalCompSpecsH != NULL" 
	
	if (gNonProjProcessorSpecs.principalCompSpecsH == NULL)
		{
				// Set up handle to project file information								
			
		gNonProjProcessorSpecs.principalCompSpecsH = 
														MNewHandle (sizeof (PrincipalCompSpecs));
		if (gNonProjProcessorSpecs.principalCompSpecsH != NULL)
			{
			gPrincipalCompSpecsPtr = (PrincipalCompSpecsPtr)GetHandlePointer (
								gNonProjProcessorSpecs.principalCompSpecsH, kLock, kMoveHi);
			
					// Initialize the principal component specification structure.	
			
			if (projectFlag && gProjectInfoPtr->numberStatisticsClasses > 0)
				gPrincipalCompSpecsPtr->dataCode = kTrainingType;
			
			else	// !projectFlag || gProjectInfoPtr->...) 
				gPrincipalCompSpecsPtr->dataCode = kAreaType;
				
			gPrincipalCompSpecsPtr->eigenValueHandle = NULL;
			gPrincipalCompSpecsPtr->eigenVectorHandle = NULL;
			gPrincipalCompSpecsPtr->projectFlag = projectFlag;
			gPrincipalCompSpecsPtr->firstTime = TRUE;
			gPrincipalCompSpecsPtr->windowInfoHandle = windowInfoHandle;
			gPrincipalCompSpecsPtr->fileInfoVersion = 
																gImageWindowInfoPtr->fileInfoVersion;
			gPrincipalCompSpecsPtr->eigenvectorCode = 1;
			gPrincipalCompSpecsPtr->equalVariancesFlag = FALSE;
			gPrincipalCompSpecsPtr->classSet = kAllMenuItem;
			gPrincipalCompSpecsPtr->channelSet = kAllMenuItem;
			gPrincipalCompSpecsPtr->lineStart = 1;
			gPrincipalCompSpecsPtr->lineEnd = gImageWindowInfoPtr->maxNumberLines;
			gPrincipalCompSpecsPtr->lineInterval = 1;
			gPrincipalCompSpecsPtr->columnStart = 1;
			gPrincipalCompSpecsPtr->columnEnd = gImageWindowInfoPtr->maxNumberColumns;
			gPrincipalCompSpecsPtr->columnInterval = 1;
			gPrincipalCompSpecsPtr->outputStorageType = 1;
			gPrincipalCompSpecsPtr->outputFormatCode = 0x0001;
			gPrincipalCompSpecsPtr->featureHandle = NULL;
			gPrincipalCompSpecsPtr->channelsHandle = NULL;
			gPrincipalCompSpecsPtr->classHandle = NULL;
			gPrincipalCompSpecsPtr->fieldTypeCode = kTrainingType;
			gPrincipalCompSpecsPtr->numberChannels = 
														gImageWindowInfoPtr->totalNumberChannels;
			
			if (projectFlag && gProjectInfoPtr != NULL)
				gPrincipalCompSpecsPtr->numberClasses =
												gProjectInfoPtr->numberStatisticsClasses;
				
			else	// !projectFlag || gProjectInfoPtr == NULL 
				gPrincipalCompSpecsPtr->numberClasses = 0;
				
			gPrincipalCompSpecsPtr->totalSumSquaresStatsHandle = NULL;
			gPrincipalCompSpecsPtr->totalChanStatsHandle = NULL;
			
			gPrincipalCompSpecsPtr->listEigenvectorFlag = TRUE;
			if (gPrincipalCompSpecsPtr->numberChannels > 50)
				gPrincipalCompSpecsPtr->listEigenvectorFlag = FALSE;
			
			gPrincipalCompSpecsPtr->saveEigenvectorFlag = FALSE;
			
			}	// end "if (gPrincipalCompSpecsH != NULL)" 
		
		else	// else gPrincipalCompSpecsH == NULL 
			returnFlag = FALSE;
			
		}	// end "if (fileInfoPtr != NULL)" 
		
	if (returnFlag && 
			gPrincipalCompSpecsPtr->dataCode == kAreaType &&
				gImageFileInfoPtr == NULL)
		returnFlag = FALSE;
					
			// Check memory for statistics histogram classes vector.					
		
	if (returnFlag && gPrincipalCompSpecsPtr->projectFlag)
		{	
		bytesNeeded =
				(SInt32)gProjectInfoPtr->numberStatisticsClasses * sizeof (SInt16);
		classPtr = (SInt16*)CheckHandleSize (&gPrincipalCompSpecsPtr->classHandle,
															&returnFlag,  
															&changedFlag, 
															bytesNeeded);
		if (changedFlag)
			gPrincipalCompSpecsPtr->classSet = kAllMenuItem;
			
		}	// end "if (returnFlag)" 
			
			// If memory is not full, check memory for principal component			
			// feature vector pointer.															

	if (returnFlag)
		{
		bytesNeeded =
				(SInt32)gImageWindowInfoPtr->totalNumberChannels * sizeof (SInt16);
		featurePtr = (SInt16*)CheckHandleSize (&gPrincipalCompSpecsPtr->featureHandle,
															&returnFlag,
															&changedFlag,
															bytesNeeded);
									
		if (changedFlag)
			gPrincipalCompSpecsPtr->channelSet = kAllMenuItem;
			
		}	// end "if (returnFlag)" 
			
			// If memory is not full, check memory for principal component			
			// channels vector.																	

	if (returnFlag)
		{
		channels = (SInt16*)CheckHandleSize (&gPrincipalCompSpecsPtr->channelsHandle,
															&returnFlag,   
															&changedFlag, 
															bytesNeeded);
			
		}	// end "if (returnFlag)" 
		
	if (returnFlag)
		{
				// Load selection rectangle information into principal component	
				// specification information if the selection rectangle has 		
				// been set up																		

		GetSelectedAreaInfo (gActiveImageWindow,
									gImageWindowInfoPtr,
									&gPrincipalCompSpecsPtr->lineStart,
									&gPrincipalCompSpecsPtr->lineEnd,
									&gPrincipalCompSpecsPtr->columnStart,
									&gPrincipalCompSpecsPtr->columnEnd,
									kDontClearSelectionArea,
									kUseThreshold,
									kDontAdjustToBaseImage);
			
				// Make certain that feature and class vectors are						
				// filled in properly.															
		
		if (gPrincipalCompSpecsPtr->channelSet == kAllMenuItem)		
			LoadFeatureVector (kNoTransformation,
										(SInt16*)&gPrincipalCompSpecsPtr->numberChannels,
										featurePtr,
										gImageWindowInfoPtr->totalNumberChannels);
															
		LoadChannelsVector (kNoProject,
									kNoTransformation,
									gPrincipalCompSpecsPtr->numberChannels, 
									featurePtr, 
									(SInt16*)&gPrincipalCompSpecsPtr->numberChannels,
									channels);
		
		if (gPrincipalCompSpecsPtr->classSet == kAllMenuItem && 
														gPrincipalCompSpecsPtr->projectFlag)								
			LoadClassVector (&gPrincipalCompSpecsPtr->numberClasses, classPtr);
			
		}	// end "if (returnFlag)" 
	
			// If unable to set up information for principal component				
			// specifications, release memory allocated to it (if any).				
			
	else	// !returnFlag 
		{
		ReleasePrincipalCompSpecsMemory (&gNonProjProcessorSpecs.principalCompSpecsH);
		gPrincipalCompSpecsPtr = NULL;
		
		}	// end "else !returnFlag" 
		
	return (returnFlag);

}	// end "LoadPrincipalComponentSpecs" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean PrincipalComponentAnalysis
//
//	Software purpose:	The purpose of this routine is to do the principal
//							component analysis.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			PrincipalComponentControl in SPrincipalComponents.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/04/1991
//	Revised By:			Larry L. Biehl			Date: 12/29/2005

Boolean PrincipalComponentAnalysis (
				FileInfoPtr							fileInfoPtr)
			
{
	CMFileStream*						resultsFileStreamPtr;
	FileIOInstructionsPtr			fileIOInstructionsPtr;
	HChannelStatisticsPtr			channelStatisticsPtr;
	HSumSquaresStatisticsPtr		totalSumSquaresStatsPtr;
	
	HDoublePtr							covariancePtr,
											doubleWorkVectorPtr,
											eigenVectorPtr;
	
	SInt16								*channelsPtr,
											*classPtr,
											*shortIntWorkVectorPtr;
	
	time_t								startTime;
	
	SInt32								numberChannels;
	
	UInt32								numberBytes;
	
	SInt16								index1,
											index2,
											vectorLength;
	
	Boolean								continueFlag,
											eigenvectorOKFlag;
	
	
			// Initialize local variables.												
	
	covariancePtr = NULL;
	eigenVectorPtr = NULL;
	doubleWorkVectorPtr = NULL;
	shortIntWorkVectorPtr = NULL;
	resultsFileStreamPtr = GetResultsFileStreamPtr (0);
	channelsPtr = (SInt16*)GetHandlePointer (gPrincipalCompSpecsPtr->channelsHandle);
	classPtr = (SInt16*)GetHandlePointer (gPrincipalCompSpecsPtr->classHandle);
	eigenvectorOKFlag = FALSE;
	continueFlag = TRUE;
	fileIOInstructionsPtr = NULL;
			
			// Get pointer to memory to use to read an image file line into.																	
				 			
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
														gPrincipalCompSpecsPtr->numberChannels,
														(HUInt16Ptr)channelsPtr,
														kDoNotPackData,
														kForceBISFormat,
														kForceReal8Bytes,
														kDoNotAllowForThreadedIO,
														&fileIOInstructionsPtr);
				
			// Get the total covariance.	
			
	InitializeAreaDescription (&gAreaDescription, 
											gPrincipalCompSpecsPtr->lineStart, 
											gPrincipalCompSpecsPtr->lineEnd, 
											gPrincipalCompSpecsPtr->columnStart, 
											gPrincipalCompSpecsPtr->columnEnd, 
											gPrincipalCompSpecsPtr->lineInterval, 
											gPrincipalCompSpecsPtr->columnInterval,
											1,
											1,
											0);
											
			// Get the start time.															
					
	startTime = time (NULL);

	if (continueFlag)
		continueFlag = GetTotalSumSquares (
											fileIOInstructionsPtr,
											&gPrincipalCompSpecsPtr->totalChanStatsHandle, 
											&gPrincipalCompSpecsPtr->totalSumSquaresStatsHandle, 
											(UInt16*)channelsPtr, 
											gPrincipalCompSpecsPtr->numberChannels,
											classPtr, 
											gPrincipalCompSpecsPtr->numberClasses,
											gPrincipalCompSpecsPtr->dataCode,
											gPrincipalCompSpecsPtr->fieldTypeCode,
											FALSE);
	
	startTime = GetTotalTime (startTime);	
	
			// List the total number of pixels used for the covariance matrix.
			//	"\r %ld pixels used for covariance matrix.\r"
			
	continueFlag = ListSpecifiedStringNumber (kPrincipalCompStrID, 
															IDS_PrincipalComponent1,
															resultsFileStreamPtr, 
															gOutputCode,
															gAreaDescription.numSamplesPerChan, 
															continueFlag);
		
			// List the total time used to computer the covariance matrix.	
			// " %lu CPU seconds for computing covariance matrix.\r"
			
	continueFlag = ListSpecifiedStringNumber (kPrincipalCompStrID, 
															IDS_PrincipalComponent2,
															resultsFileStreamPtr, 
															gOutputCode,
															(SInt32)startTime, 
															continueFlag);
				
			// Dispose of the IO buffer.		
								
	DisposeIOBufferPointers (fileIOInstructionsPtr,
										&gInputBufferPtr, 
										&gOutputBufferPtr);
	
	if (continueFlag)
		{																	
				// Hide the status dialog items pertaining to getting the total	
				// covariance.																		
				
		ShowStatusDialogItemSet (kStatusMinutes);
		HideStatusDialogItemSet (kStatusLine);
		HideStatusDialogItemSet (kStatusClassB);
		
				// "\pLargest element:"
		LoadDItemStringNumber (kPrincipalCompStrID, 
										IDS_PrincipalComponent8,
										gStatusDialogPtr, 
										IDC_Status13, 
										(Str255*)gTextString);
	
				//	"\pComputing Eigenvalues and Eigenvectors");
		LoadDItemStringNumber (kPrincipalCompStrID, 
										IDS_PrincipalComponent9,
										gStatusDialogPtr, 
										IDC_Status11, 
										(Str255*)gTextString);
	
				// Get pointer to memory to use for square covariance or 			
				// correlation matrix.															
					
		numberBytes = (UInt32)gPrincipalCompSpecsPtr->numberChannels *
									gPrincipalCompSpecsPtr->numberChannels * sizeof (double);
		numberBytes = MAX (numberBytes, 2*sizeof (double));
		gPrincipalCompSpecsPtr->eigenValueHandle = MNewHandle (numberBytes);
		if (gPrincipalCompSpecsPtr->eigenValueHandle)
			{  
			covariancePtr = (HDoublePtr)GetHandlePointer (
														gPrincipalCompSpecsPtr->eigenValueHandle,
														kLock);
			
			}	// end "if (gPrincipalCompSpecsPtr->eigenVectorHandle)" 
			
		else	// gPrincipalCompSpecsPtr->eigenValueHandle == NULL 
			continueFlag = FALSE;
			
		}	// end "if (continueFlag)" 
		
	totalSumSquaresStatsPtr = (HCovarianceStatisticsPtr)GetHandlePointer (
											gPrincipalCompSpecsPtr->totalSumSquaresStatsHandle);
		
			// Convert the correlation matrix to one with equal variances.	
		
	channelStatisticsPtr = (HChannelStatisticsPtr)GetHandlePointer (
											gPrincipalCompSpecsPtr->totalChanStatsHandle);		
			
	if (continueFlag && gPrincipalCompSpecsPtr->equalVariancesFlag)
		{
		ComputeCorrelationCoefficientMatrix (gPrincipalCompSpecsPtr->numberChannels,
															covariancePtr, 
															gPrincipalCompSpecsPtr->numberChannels,
															NULL,
															channelStatisticsPtr, 
															(HDoublePtr)totalSumSquaresStatsPtr,
															gAreaDescription.numSamplesPerChan,
															kSquareOutputMatrix);
						
		}	// end "if (continueFlag && ..." 
		
			// Convert the triangular covariance matrix to a square matrix.		
			
	if (continueFlag && !gPrincipalCompSpecsPtr->equalVariancesFlag)
		ComputeCovarianceMatrix (gPrincipalCompSpecsPtr->numberChannels,
											covariancePtr,
											gPrincipalCompSpecsPtr->numberChannels,
											NULL, 
											channelStatisticsPtr, 
											totalSumSquaresStatsPtr, 
											gAreaDescription.numSamplesPerChan,
											kSquareOutputMatrix);
						
				// Release memory used for triangular covariance matrix.				
				
	gPrincipalCompSpecsPtr->totalSumSquaresStatsHandle = 
				UnlockAndDispose (gPrincipalCompSpecsPtr->totalSumSquaresStatsHandle);
	totalSumSquaresStatsPtr = NULL;
		
				// Get pointer to memory to use for eigenVector.						

	if (continueFlag && gPrincipalCompSpecsPtr->eigenvectorCode)
		{		
		gPrincipalCompSpecsPtr->eigenVectorHandle = MNewHandle (numberBytes);
		if (gPrincipalCompSpecsPtr->eigenVectorHandle != NULL)
			eigenVectorPtr = (HDoublePtr)GetHandlePointer (
												gPrincipalCompSpecsPtr->eigenVectorHandle,
												kLock);
			
		else	// gPrincipalCompSpecsPtr->eigenVectorHandle == NULL 
			continueFlag = FALSE;
			
		}	// end "if (continueFlag && ...)" 
			
					// Get pointer to memory to use for double work vector.			
	
	if (continueFlag)
		{		
		vectorLength = gPrincipalCompSpecsPtr->numberChannels;
		vectorLength = MAX (2, vectorLength);
		numberBytes = (UInt32)vectorLength * sizeof (double);
		doubleWorkVectorPtr = (double *)MNewPointer (numberBytes);
		if (doubleWorkVectorPtr == NULL)  
			continueFlag = FALSE;
			
		}	// end "if (continueFlag && ...)" 
			
			// Get pointer to memory to use for short int work vector.			

	if (continueFlag)
		{		
		numberBytes = (UInt32)gPrincipalCompSpecsPtr->numberChannels * sizeof (SInt16);
		shortIntWorkVectorPtr = (SInt16*)MNewPointer (numberBytes);
		if (shortIntWorkVectorPtr == NULL)  
			continueFlag = FALSE;
			
		}	// end "if (continueFlag && ...)" 
	
			// Compute the eigenvalues and eigenvectors.									
			
	if (continueFlag)
		{	
				// Get the start time.															
						
		startTime = time (NULL);
	
		if (!ComputeEigenvectors (covariancePtr,
											gPrincipalCompSpecsPtr->numberChannels, 
											eigenVectorPtr,
											shortIntWorkVectorPtr,
											doubleWorkVectorPtr,
											gPrincipalCompSpecsPtr->eigenvectorCode + 2))	
						
			{
					// Indicate that computation of eigenvector was stopped due		
					// to iteration limit.														
					
			//"\r Eigenvalue computation was stopped.\r");
			continueFlag = ListSpecifiedStringNumber (kPrincipalCompStrID, 
																	IDS_PrincipalComponent3,
																	(UCharPtr)gCharBufferPtr1,
																	resultsFileStreamPtr, 
																	gOutputCode,
																	continueFlag);
			
			}	// end "if (!ComputeEigenvectors (..." 

		
				// Get memory for matrix inversions and eigenvector computation.
				// This was done for some testing with singular value decomposition
				// routine
		/*	
		if (SetupMatrixInversionMemory ((UInt16)gPrincipalCompSpecsPtr->numberChannels,
														kMeanCovariance,
														&gInverseMatrixMemory.inversePtr, 
														&gInverseMatrixMemory.pivotPtr, 
														&gInverseMatrixMemory.indexRowPtr, 
														&gInverseMatrixMemory.indexColPtr, 
														NULL))
			{												
			if (ComputeEigenvectorsSVD (
										covariancePtr, 
										gPrincipalCompSpecsPtr->numberChannels, 
										gPrincipalCompSpecsPtr->numberChannels, 
										gInverseMatrixMemory.inversePtr, 	// eigen_values
										eigenVectorPtr,
										gInverseMatrixMemory.pivotPtr,
										(double**)gInverseMatrixMemory.indexRowPtr,
										(double**)gInverseMatrixMemory.indexColPtr) != noErr)
						
				{
						// Indicate that computation of eigenvector was stopped due		
						// to iteration limit.														
						
				//"\r Eigenvalue computation was stopped.\r");
				continueFlag = ListSpecifiedStringNumber (
													kPrincipalCompStrID, 
													IDS_PrincipalComponent3,
													(UCharPtr)gCharBufferPtr1,
													resultsFileStreamPtr, 
													gOutputCode,
													continueFlag);
				
				}	// end "if (!ComputeEigenvectors (..." 
				
			BlockMoveData (gInverseMatrixMemory.inversePtr, 
									covariancePtr, 
									gPrincipalCompSpecsPtr->numberChannels*sizeof (double));
				
			ReleaseMatrixInversionMemory ();
			
			}	// end "if (SetupMatrixInversionMemory ..." 
		*/			
		eigenvectorOKFlag = TRUE;
									
		}	// end "if (continueFlag)" 
		
	HideStatusDialogItemSet (kStatusMinutes);
						
	if (eigenvectorOKFlag)
		{
		HDoublePtr		eigenValuePtr;
		
		
		numberChannels = gPrincipalCompSpecsPtr->numberChannels;
			
				// Save the PC channel means.													
				
		eigenValuePtr = (HDoublePtr)GetHandlePointer (
														gPrincipalCompSpecsPtr->eigenValueHandle);
		eigenValuePtr = &eigenValuePtr[numberChannels];
		
				// Initialize (set to zero) the PC channel mean vector.
						
		ZeroMatrix (eigenValuePtr,
						numberChannels,
						1,
						TRUE);
						
		for (index1=0; index1<numberChannels; index1++)
			{
			channelStatisticsPtr = (HChannelStatisticsPtr)GetHandlePointer (
													gPrincipalCompSpecsPtr->totalChanStatsHandle);
			
			for (index2=0; index2<numberChannels; index2++)
				{
				*eigenValuePtr += channelStatisticsPtr->mean * *eigenVectorPtr;
				
				channelStatisticsPtr++;
				eigenVectorPtr++;
				
				}	// end "for (index2=0; index2<..."
				
			eigenValuePtr++; 
				
			}	// end "for (index1=1; index1<=..." 
			
		eigenVectorPtr = (HDoublePtr)GetHandlePointer (
														gPrincipalCompSpecsPtr->eigenVectorHandle);
			
		}	// end "if (eigenvectorOKFlag)"
				
			// List the Principal Components results.										
			
	if (gStatusDialogPtr && continueFlag)
		//"\pListing Eigenvalues and Eigenvectors");
		LoadDItemStringNumber (kPrincipalCompStrID, 
										IDS_PrincipalComponent10,
										gStatusDialogPtr, 
										IDC_Status11, 
										(Str255*)gTextString);
			
	if (continueFlag)
		continueFlag = ListTransformationInformation (
													covariancePtr, 
													eigenVectorPtr, 
													(SInt32)doubleWorkVectorPtr[0], 
													doubleWorkVectorPtr[1],
													(UInt16*)channelsPtr,
													gPrincipalCompSpecsPtr->numberChannels,
													gPrincipalCompSpecsPtr->numberChannels,
													TRUE,
													gPrincipalCompSpecsPtr->listEigenvectorFlag,
													1);
							
			// Release memory for  work vectors.											
			
	CheckAndDisposePtr (shortIntWorkVectorPtr);
	CheckAndDisposePtr (doubleWorkVectorPtr);
				
			// List the time for principal components analysis.						
						
	continueFlag = ListCPUTimeInformation (resultsFileStreamPtr, 
															continueFlag, 
															startTime);
						
	if (eigenvectorOKFlag && gPrincipalCompSpecsPtr->saveEigenvectorFlag)
		{
		CheckAndUnlockHandle (gPrincipalCompSpecsPtr->eigenVectorHandle);
		CheckAndUnlockHandle (gPrincipalCompSpecsPtr->eigenValueHandle);
		numberBytes = (SInt32)gPrincipalCompSpecsPtr->numberChannels *
																			 2 * sizeof (double);
		MSetHandleSize (&gPrincipalCompSpecsPtr->eigenValueHandle, numberBytes);
		
				// Save the new transformation matrix information.							
				
		eigenvectorOKFlag = SaveTransformationMatrix (
														1, 
														gPrincipalCompSpecsPtr->eigenVectorHandle,
														gPrincipalCompSpecsPtr->eigenValueHandle,
														gPrincipalCompSpecsPtr->channelsHandle,
														NULL,
														gPrincipalCompSpecsPtr->numberChannels,
														gPrincipalCompSpecsPtr->numberChannels);
		
		}	// end "...saveEigenvectorFlag" 
		
	if (!eigenvectorOKFlag || !gPrincipalCompSpecsPtr->saveEigenvectorFlag)
		{
		UnlockAndDispose (gPrincipalCompSpecsPtr->eigenValueHandle);
		UnlockAndDispose (gPrincipalCompSpecsPtr->eigenVectorHandle);
		
		}	// end "if (!eigenvectorOKFlag || !...saveEigenvectorFlag" 
		
	gPrincipalCompSpecsPtr->eigenValueHandle = NULL;
	gPrincipalCompSpecsPtr->eigenVectorHandle = NULL;
						
			// Release memory used for mean vector.										
		
	gPrincipalCompSpecsPtr->totalChanStatsHandle = 
							UnlockAndDispose (gPrincipalCompSpecsPtr->totalChanStatsHandle);
							
	return (continueFlag);

}	// end "PrincipalComponentAnalysis" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void PrincipalComponentControl
//
//	Software purpose:	The purpose of this routine is to handle the
//							"Principal Component Analysis" event.  The defaults will 
//							be set according to the statistics window mode and selection.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/28/1990
//	Revised By:			Larry L. Biehl			Date: 09/22/1997

void PrincipalComponentControl (void)

{	
	Handle								activeWindowInfoHandle;
	CMFileStream*						resultsFileStreamPtr;
	SInt16								activeImageGlobalHandleStatus;
	Boolean								continueFlag;
	
		    		
			// If spare memory had to be used to load code resources, then exit	
			// routine.																				
			
	if (gMemoryTypeNeeded < 0)
																									return;
																							
			// Code resources loaded okay, so set flag back for non-Code			
			// resources.																			
			
	gMemoryTypeNeeded = 0;
 			
 			// Initialize variables.															
 			
 	gStatusDialogPtr = NULL;
 	gPrincipalCompSpecsPtr = NULL;
 	
 			// Get image window globals.
 	
	continueFlag = SetUpActiveImageInformationGlobals (&activeWindowInfoHandle, 
																		&activeImageGlobalHandleStatus, 
																		kUseProject);
	
			// Allow for case when we have a project file with no access
			// to an image file. We may still be able to compute the
			// eigenvectors from the already computed project classes.
												
	if (!continueFlag && activeWindowInfoHandle != NULL)
		{
		GetImageInformationPointers (&activeImageGlobalHandleStatus, 
												activeWindowInfoHandle,
												&gImageWindowInfoPtr, 
												&gImageLayerInfoPtr, 
												&gImageFileInfoPtr);
									
		continueFlag = TRUE;
		
		}	// end "if (!continueFlag && activeWindowInfoHandle != NULL)"
		
			// Set up principal component specification structure.					
			
	if (continueFlag && LoadPrincipalComponentSpecs (activeWindowInfoHandle))
		{
		if (gImageWindowInfoPtr == NULL)
																									return;
																							
		if (PrincipalComponentDialog (gImageFileInfoPtr))
			{
					// Initialize some global variables pertaining to output.		
			
			gOutputTextOKFlag = TRUE;
			gOutputCode = gPrincipalCompSpecsPtr->outputStorageType;
			gOutputForce1Code = (gOutputCode | 0x0001);
			continueFlag = TRUE;
			
			gAreaDescription.lineInterval = gPrincipalCompSpecsPtr->lineInterval;
			gAreaDescription.columnInterval = gPrincipalCompSpecsPtr->columnInterval;
			
					// Release memory used by last set of eigenvalues/eigenvectors	
					// if they exist.																
					
			gPrincipalCompSpecsPtr->eigenValueHandle = 
								UnlockAndDispose (gPrincipalCompSpecsPtr->eigenValueHandle);
			gPrincipalCompSpecsPtr->eigenVectorHandle = 
								UnlockAndDispose (gPrincipalCompSpecsPtr->eigenVectorHandle);
	
					// Get pointer to buffer to use to store data values 				
					// to be listed. 
					
			if (continueFlag)
				continueFlag = GetMemoryForListTransformation (
													gPrincipalCompSpecsPtr->numberChannels, 
													gPrincipalCompSpecsPtr->listEigenvectorFlag);
												
					// If list data results are to go the a disk file, 				
					// open the	disk file.	Trick 'CreateResultsDiskFiles' to 		
					// estimate the number of bytes on 'pseudo' lines and 			
					// columns.  Assume 128K bytes needed for now.						
			
			if (gPrincipalCompSpecsPtr->outputStorageType & 0x0002)
				{
				InitializeAreaDescription (&gAreaDescription, 
													1, 
													16000, 
													1, 
													8, 
													1, 
													1,
													1,
													1,
													0);
										
				continueFlag = CreateResultsDiskFiles (
												gPrincipalCompSpecsPtr->outputStorageType, 0, 0);
				
				}	// end "if (...->outputStorageType & 0x0002)" 
		
	 		resultsFileStreamPtr = GetResultsFileStreamPtr (0);
 		
					// Continue list data if everything is okay.							
			
			if (continueFlag)
				{
						// Change cursor to watch cursor until done with process.	
					
				MSetCursor (kWait);
		
						// Force text selection to start from end of present text.	
						
				ForceTextToEnd ();
								
				}	// end "if (continueFlag)" 
					
					// List the processor name, date, time and image info.
         	
			continueFlag = ListHeaderInfo (
								resultsFileStreamPtr, 
								kLImageInfo +
									2*gPrincipalCompSpecsPtr->projectFlag + 
											8*(gPrincipalCompSpecsPtr->dataCode == kTrainingType), 
								&gOutputForce1Code, 
								kNoStatisticsUsed, 
								continueFlag);
														
					// List the Principal Component specifications.						
					// 	Principal Component analysis type.								
			
					// " Covariance matrix will be used.\r"
			SInt16 stringIndex = IDS_PrincipalComponent4;		
			if (gPrincipalCompSpecsPtr->equalVariancesFlag)
						//" Correlation matrix will be used.\r"
				stringIndex = IDS_PrincipalComponent5;
				
			continueFlag = ListSpecifiedStringNumber (kPrincipalCompStrID, 
																	stringIndex,
																	gTextString,
																	resultsFileStreamPtr, 
																	gOutputForce1Code,
																	continueFlag);
			
			if (continueFlag && gPrincipalCompSpecsPtr->dataCode == kTrainingType)
						// " For selected project classes and fields.\r"
				continueFlag = ListSpecifiedStringNumber (kPrincipalCompStrID, 
																		IDS_PrincipalComponent6,
																		gTextString,
																		resultsFileStreamPtr, 
																		gOutputForce1Code,
																		continueFlag);
							
			else	// gPrincipalCompSpecsPtr->dataCode == kAreaType 
				{
						// " For selected area.\r"
				continueFlag = ListSpecifiedStringNumber (kPrincipalCompStrID, 
																		IDS_PrincipalComponent7,
																		gTextString,
																		resultsFileStreamPtr, 
																		gOutputForce1Code,
																		continueFlag);
														
						// "  Lines:   %5ld to %5ld by %ld\r"
				continueFlag = ListSpecifiedStringNumber (
															kSharedStrID, 
															IDS_Shared1,
															resultsFileStreamPtr, 
															gOutputForce1Code,
															gPrincipalCompSpecsPtr->lineStart,
															gPrincipalCompSpecsPtr->lineEnd,
															gPrincipalCompSpecsPtr->lineInterval, 
															continueFlag);
														
						//	"  Columns: %5ld to %5ld by %ld\r"
				continueFlag = ListSpecifiedStringNumber (
															kSharedStrID, 
															IDS_Shared2,
															resultsFileStreamPtr, 
															gOutputForce1Code,
															gPrincipalCompSpecsPtr->columnStart,
															gPrincipalCompSpecsPtr->columnEnd,
															gPrincipalCompSpecsPtr->columnInterval, 
															continueFlag);
														
				}	// end "else gPrincipalCompSpecsPtr->dataCode == ..." 
		
					// Insert a blank line after the table.
			
			continueFlag = OutputString (resultsFileStreamPtr, 
													gEndOfLine, 
													gNumberOfEndOfLineCharacters, 
													gOutputForce1Code, 
													continueFlag);
				
					// List the channels used.													
			
			if (continueFlag && 
						gImageFileInfoPtr != NULL &&
							!gImageFileInfoPtr->thematicType)
				{
				SInt16* channelsPtr = (SInt16*)GetHandlePointer (
														gPrincipalCompSpecsPtr->channelsHandle);
										
				continueFlag = ListChannelsUsed (gImageFileInfoPtr,
															NULL,
															channelsPtr,
															gPrincipalCompSpecsPtr->numberChannels, 
															resultsFileStreamPtr,
															&gOutputForce1Code,
															kNoTransformation);
									
				}	// end "if (continueFlag && !gImageFileInfoPtr->thematicType)"
			
			if (continueFlag && gPrincipalCompSpecsPtr->dataCode == kTrainingType)
				{
						// List the classes used.
						
				SInt16* classPtr = (SInt16*)GetHandlePointer (
															gPrincipalCompSpecsPtr->classHandle);												
												
				if (continueFlag)
					continueFlag =  ListClassesUsed (NULL,
																gImageFileInfoPtr, 
																classPtr, 
																kListNoSymbols,
																kListNoWeights, 
																gPrincipalCompSpecsPtr->numberClasses, 
																resultsFileStreamPtr, 
																&gOutputForce1Code,
																FALSE);
		
						// List the fields/areas used.										
							
				if (continueFlag)							
					continueFlag = ListProjectFieldsUsed (
															gImageFileInfoPtr,
															classPtr,
															gPrincipalCompSpecsPtr->numberClasses,
															NULL,
															gPrincipalCompSpecsPtr->fieldTypeCode,
															resultsFileStreamPtr,
															&gOutputForce1Code,
															TRUE);
						
				}	// end "if (gPrincipalCompSpecsPtr->dataCode == ...)" 
										
					// List "  Output Information:".
					
			continueFlag = ListSpecifiedStringNumber (kSharedStrID, 
																	IDS_Shared8, 
																	(unsigned char*)gTextString, 
																	resultsFileStreamPtr, 
																	gOutputForce1Code, 
																	continueFlag);
					
					// Get dialog box to display list data status.						
			
			if (continueFlag)
				gStatusDialogPtr = GetStatusDialog (kUpdateStatsInfoID, TRUE);
				
			if (gStatusDialogPtr == NULL) 
				continueFlag = FALSE;
				
			if (continueFlag)
				{
				ShowStatusDialogItemSet (kStatusLine);
				ShowStatusDialogItemSet (kStatusCommand);
			
				if (gPrincipalCompSpecsPtr->dataCode == kTrainingType)
					{
							// "\pGetting covariance for project classes."
					LoadDItemStringNumber (kPrincipalCompStrID, 
													IDS_PrincipalComponent11,
													gStatusDialogPtr, 
													IDC_Status11, 
													(Str255*)gTextString);
													
							// "\pArea");
					LoadDItemStringNumber (kPrincipalCompStrID, 
													IDS_PrincipalComponent12,
													gStatusDialogPtr, 
													IDC_Status2, 
													(Str255*)gTextString);
					ShowStatusDialogItemSet (kStatusClassB);
				
					}	// end "if (gPrincipalCompSpecsPtr->dataCode == ..." 
				
				if (gPrincipalCompSpecsPtr->dataCode == kAreaType)
							// "\pGetting covariance for selected area."
					LoadDItemStringNumber (kPrincipalCompStrID, 
													IDS_PrincipalComponent13,
													gStatusDialogPtr, 
													IDC_Status11, 
													(Str255*)gTextString);
				if (gStatusDialogPtr)
					DrawDialog (gStatusDialogPtr);
									
				}	// end "if (continueFlag)" 
			
			if (continueFlag)
				{			
						// Turn spin cursor on
		
				gPresentCursor = kSpin;
							
				continueFlag = PrincipalComponentAnalysis (gImageFileInfoPtr);

						// Turn spin cursor off
			
				gPresentCursor = kWait;
				
				}	// end "if (continueFlag)"
			
					// Scroll output window to the selection and adjust the 			
					// scroll bar.																	
			
			UpdateOutputWScrolls (gOutputWindow, 1, kDisplayMessage);
				
					// Dispose of status dialog box.											
			
			CloseStatusDialog (TRUE);
			
					// Change cursor back to pointer.										
						
			MInitCursor ();
					
					// Close the  results file if needed and								
					// release the memory assigned to it.									
						
			CloseResultsFiles ();
			
					// Dispose of the buffer used to read a line of information 	
					// into.																			
					
			gCharBufferPtr1 = CheckAndDisposePtr (gCharBufferPtr1);
				
			}	// end "if (PrincipalComponentDialog ())" 
			
		}	// end "if (continueFlag && LoadPrincipalComponentSpecs ())" 
			
				// Unlock handles.
				
	UnlockImageInformationHandles (activeImageGlobalHandleStatus, 
												activeWindowInfoHandle);
				
	if (gPrincipalCompSpecsPtr != NULL)
		{	
		CheckAndUnlockHandle (gPrincipalCompSpecsPtr->classHandle);
		CheckAndUnlockHandle (gPrincipalCompSpecsPtr->channelsHandle);
		CheckAndUnlockHandle (gPrincipalCompSpecsPtr->featureHandle);
			
		}	// end "if (gPrincipalCompSpecsPtr != NULL)" 
		
			// Unlock memory for the principal component specifications.			
		
	CheckAndUnlockHandle (gNonProjProcessorSpecs.principalCompSpecsH);
	
	gPrincipalCompSpecsPtr = NULL;
	
	gImageWindowInfoPtr = NULL;
	gImageFileInfoPtr = NULL;	
	gImageLayerInfoPtr = NULL;	

}	// end "PrincipalComponentControl" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean PrincipalComponentDialog
//
//	Software purpose:	The purpose of this routine is to obtain the
//							user specifications for principal component analysis
//							of the image file data.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	True if user selects OK in dialog box
//							False if user selects Cancel in dialog box
//
// Called By:			PrincipalComponentControl
//
//	Coded By:			Larry L. Biehl			Date: 12/28/1990
//	Revised By:			Larry L. Biehl			Date: 10/19/2018
	
Boolean PrincipalComponentDialog (
				FileInfoPtr							fileInfoPtr)
	
{			
	Boolean								returnFlag;

#if defined multispec_mac
	Boolean								modalDone;
								
	DialogSelectArea					dialogSelectArea;
	
	Handle								okHandle,
											theHandle;
	
	Rect									theBox;
	
	SInt32								theNum;
	
	SInt16								*channels,
											controlValue,
											entireIconItem,
											itemHit,
											*featurePtr,
											selectItem,
											theType;
	
	DialogPtr							dialogPtr;
	SInt16								*classPtr;

	
			// Get the modal dialog for the list data specification.					
				
	dialogPtr = LoadRequestedDialog (kPrincipalComponentsID, kCopyScrap, 1, 2);
	if (dialogPtr == NULL)												
																						return (FALSE);
				
			// Initialize selected area structure.		

	entireIconItem = 10;
	if (gAppearanceManagerFlag)
		entireIconItem = 30;
	
	InitializeDialogSelectArea (&dialogSelectArea,
											gImageWindowInfoPtr,
											gActiveImageWindow,
											gPrincipalCompSpecsPtr->columnStart,
											gPrincipalCompSpecsPtr->columnEnd,
											gPrincipalCompSpecsPtr->columnInterval,
											gPrincipalCompSpecsPtr->lineStart,
											gPrincipalCompSpecsPtr->lineEnd,
											gPrincipalCompSpecsPtr->lineInterval,
											14,
											entireIconItem,
											kAdjustToBaseImage);

	classPtr = (SInt16*)GetHandlePointer (gPrincipalCompSpecsPtr->classHandle);
									
	featurePtr = (SInt16*)GetHandlePointer (gPrincipalCompSpecsPtr->featureHandle);
									
	channels = (SInt16*)GetHandlePointer (gPrincipalCompSpecsPtr->channelsHandle);
	
			// Save handle for the OK button for use later.								
			
	GetDialogItem (dialogPtr, 1,  &theType, &okHandle, &theBox);
		
			// Update the modal dialog with the default settings						
			// Set radio button for "Classes".												
			
			// items = 5 & 6: Classes to use.												
			//	Make all classes the default													
				
	SetDLogControl (
			dialogPtr, 5, (gPrincipalCompSpecsPtr->dataCode==kTrainingType));
				
	if (gPrincipalCompSpecsPtr->projectFlag)
		SetDialogItemDrawRoutine (dialogPtr, 6, gDrawDialogClassPopUpPtr);
	
	else	// !gPrincipalCompSpecsPtr->projectFlag" 
		HideDialogItem (dialogPtr, 5);
		
	gClassSelection = gPrincipalCompSpecsPtr->classSet;

	if (gPrincipalCompSpecsPtr->dataCode == kTrainingType)
		ShowDialogItems (dialogPtr, 6, 8);
		
	else	// gPrincipalCompSpecsPtr->dataCode != kTrainingType" 
		HideDialogItems (dialogPtr, 6, 8);
	
			// Set control for including training fields.								
			
	controlValue = (gPrincipalCompSpecsPtr->fieldTypeCode & kTrainingType)  ? 1 : 0;
	SetDLogControl (dialogPtr, 7, controlValue);
	
			// Set control for including test fields.										
			
	controlValue = (gPrincipalCompSpecsPtr->fieldTypeCode & kTestingType)  ? 1 : 0;
	SetDLogControl (dialogPtr, 8, controlValue);
	if (gProjectInfoPtr && gProjectInfoPtr->numberStatTestFields <= 0)
		SetDLogControlHilite (dialogPtr, 8, 255);
		
	HideDialogItem (dialogPtr, 10);
	//HideDialogItem (dialogPtr, 30);
		
	if (fileInfoPtr == NULL)
		{
		SetDLogControlHilite (dialogPtr, 9, 255);
		HideDialogItem (dialogPtr, 11);
		HideDialogItem (dialogPtr, 13);
		HideDialogItem (dialogPtr, 16);
		HideDialogItem (dialogPtr, 19);
		
		}	// end "if (fileInfoPtr == NULL)"

			// To entire image icon.															
			//	Selected area for principal components										
			
	LoadLineColumnItems (&dialogSelectArea, 
									dialogPtr, 
									kInitializeLineColumnValues, 
									kIntervalEditBoxesExist,
									1);

			// Set radio button for area.														
			
	if (gPrincipalCompSpecsPtr->dataCode == kAreaType)
		{
		SetDLogControl (dialogPtr, 9, 1);
		ShowDialogItem (dialogPtr, entireIconItem);
		ShowDialogItem (dialogPtr, 12);
		ShowDialogItem (dialogPtr, 14);
		ShowDialogItem (dialogPtr, 15);
		ShowDialogItem (dialogPtr, 17);
		ShowDialogItem (dialogPtr, 18);
		
		selectItem = 14;
		
		}	// end "if (gPrincipalCompSpecsPtr->dataCode == kAreaType)" 
					
	else	// gPrincipalCompSpecsPtr->dataCode != kAreaType 
		{
		SetDLogControl (dialogPtr, 9, 0);
		HideDialogItem (dialogPtr, entireIconItem);
		HideDialogItem (dialogPtr, 12);
		HideDialogItem (dialogPtr, 14);
		HideDialogItem (dialogPtr, 15);
		HideDialogItem (dialogPtr, 17);
		HideDialogItem (dialogPtr, 18);
		
		selectItem = 16;
		
		}	// end "else gPrincipalCompSpecsPtr->dataCode != kAreaType" 
	
			// Items = 20 & 21: 																	
			//	Make all channels the default													
	
	SetDialogItemDrawRoutine (dialogPtr, 21, gDrawChannelsPopUpPtr);
	gChannelSelection = gPrincipalCompSpecsPtr->channelSet;
	
			// Item 23.  Set check box for "List eigenvector vectors".				
			
	SetDLogControl (dialogPtr, 23, (gPrincipalCompSpecsPtr->listEigenvectorFlag));
	
			// Item 24.  Set check box for "Equalize variances".						
			
	SetDLogControl (dialogPtr, 24, gPrincipalCompSpecsPtr->equalVariancesFlag);
	
			// Item 25.  Set check box for "Save eigenvector flag".					
			
	SetDLogControl (dialogPtr, 25, gPrincipalCompSpecsPtr->saveEigenvectorFlag);
	
			// Item 27.  Set check box for "text output window".						
			
	controlValue = (gPrincipalCompSpecsPtr->outputStorageType & 0x0001)  ? 1 : 0;
	SetDLogControl (dialogPtr, 27, controlValue);
	
			// Item 28.  Set check box for "disk file".									
			
	controlValue = (gPrincipalCompSpecsPtr->outputStorageType & 0x0002)  ? 1 : 0;
	SetDLogControl (dialogPtr, 28, controlValue);
		
			// Center the dialog and then show it.											
			
	ShowDialogWindow (
						dialogPtr, kPrincipalComponentsID, kSetUpDFilterTable);
	
			// Set default text selection to first edit text item						
			
	SelectDialogItemText (dialogPtr, selectItem, 0, INT16_MAX);
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
				case 5:		// check button for "Classes" 
					SetControlValue ((ControlHandle)theHandle, 1);
					SetDLogControl (dialogPtr, 9, 0);
					ShowDialogItems (dialogPtr, 6, 8);
					HideDialogItem (dialogPtr, entireIconItem);
					HideDialogItem (dialogPtr, 12);
					HideDialogItem (dialogPtr, 14);
					HideDialogItem (dialogPtr, 15);
					HideDialogItem (dialogPtr, 17);
					HideDialogItem (dialogPtr, 18);
					SelectDialogItemText (dialogPtr, 16, 0, INT16_MAX);
					break;
					
				case 6:		// Classes 
					itemHit = StandardPopUpMenu (dialogPtr, 
															5, 
															6, 
															gPopUpAllSubsetMenu, 
															gClassSelection, 
															kPopUpAllSubsetMenuID);
													
					if (itemHit == kSubsetMenuItem)
						{
								// Subset of classes to be used.								
								
						itemHit = ClassDialog (&gPrincipalCompSpecsPtr->numberClasses,
														classPtr, 
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
					
				case 9:		// List data within selected area. 
					SetControlValue ((ControlHandle)theHandle, 1);
					SetDLogControl (dialogPtr, 5, 0);
					HideDialogItems (dialogPtr, 6, 8);
					ShowDialogItem (dialogPtr, entireIconItem);
					ShowDialogItem (dialogPtr, 12);
					ShowDialogItem (dialogPtr, 14);
					ShowDialogItem (dialogPtr, 15);
					ShowDialogItem (dialogPtr, 17);
					ShowDialogItem (dialogPtr, 18);
					SelectDialogItemText (dialogPtr, 14, 0, INT16_MAX);
					break;
					
				case 10:				// Entire area to selected area switch.		
				case 30:				// Entire area to selected area switch. (Appearance Manager)	
				case 14:				//	 lineStart  
				case 15:				//	 lineEnd  
				case 16:				//	 lineInterval  	
				case 17:				//	 columnStart  
				case 18:				//	 columnEnd  
				case 19:				//	 columnInterval  	
					DialogLineColumnHits (&dialogSelectArea,
													dialogPtr, 
													itemHit,
													theHandle,
													theNum);
					break;
					
				case 21:		// Channels 
					itemHit = StandardPopUpMenu (dialogPtr, 
															20, 
															21, 
															gPopUpAllSubsetMenu, 
															gChannelSelection, 
															kPopUpAllSubsetMenuID);
					if (itemHit == 2)
						{
								// Subset of channels to be used.							
								
						HiliteControl ((ControlHandle)okHandle, 255);
						
						ChannelsDialog ((SInt16*)&gPrincipalCompSpecsPtr->numberChannels,
												featurePtr,
												gImageLayerInfoPtr,
												fileInfoPtr,
												1,
												kNoTransformation,
												NULL,
												gImageWindowInfoPtr->totalNumberChannels,
												gChannelSelection);
							
						HiliteControl ((ControlHandle)okHandle, 0);
												
						if (gPrincipalCompSpecsPtr->numberChannels ==
														gImageWindowInfoPtr->totalNumberChannels)
							itemHit = 1;
							
						}	// end "if (itemHit == 2 || ...)" 
					
					if (itemHit != 0)
						gChannelSelection = itemHit;
	
							// Make certain that the correct label is drawn in the	
							// channel pop up box.												
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
					
				case 7:		// check box for "training fields" 
				case 8:		// check box for "test fields" 
				case 23:		// check box for "List eigenvectors" 
				case 24:		// check box for "Equalize variances" 
				case 25:		// check box for "Save eigenvectors" 
				case 27:		// check box for "write results to output window." 
				case 28:		// check box for "write results to disk file." 
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					break;
						
				}	// end "switch (itemHit)" 
				
			}	// end "if (itemHit > 2)" 
			
		else	// itemHit <= 2 
			{	
					// If item hit is 1, check if line-column values make 			
					// sense.  If they don't, sound an alert and make item hit 		
					// equal to 0 to allow user to make changes.							
					
			if (itemHit == 1 && GetDLogControl (dialogPtr, 9) == 1)
				itemHit = CheckLineColValues (
										&dialogSelectArea,
										gPrincipalCompSpecsPtr->lineStart,
										gPrincipalCompSpecsPtr->columnStart,
										dialogPtr);
										
			if (itemHit == 1)
				{
				modalDone = TRUE;
				
				SInt16			dataCode;
				
				if (GetDLogControl (dialogPtr, 5)) 
					dataCode = 0;     
				
				if (GetDLogControl (dialogPtr, 9)) 
					dataCode = 1; 
				
				PrincipalComponentDialogOK (gPrincipalCompSpecsPtr,
														dataCode,
														gClassSelection,
														gPrincipalCompSpecsPtr->numberClasses,
														(UInt16*)classPtr,
														GetDLogControl (dialogPtr, 7),
														GetDLogControl (dialogPtr, 8),
														&dialogSelectArea,
														gChannelSelection,
														gPrincipalCompSpecsPtr->numberChannels,
														(UInt16*)featurePtr,
														GetDLogControl (dialogPtr, 23),
														GetDLogControl (dialogPtr, 24),
														GetDLogControl (dialogPtr, 25),
														GetDLogControl (dialogPtr, 27),
														GetDLogControl (dialogPtr, 28));
				
				returnFlag = TRUE;

				}	// end if (itemHit == 1) 
			
			if (itemHit == 2)
				{
				modalDone = TRUE;
				returnFlag = FALSE;
				
				}	// end "if (itemHit == 2)" 
				
			}	// end "else itemHit <= 2" 
				
		}	while (!modalDone);
		
	CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);	
#endif	// defined multispec_mac		

	#if defined multispec_win      	
		CMPrincipalCompDialog*		dialogPtr = NULL;
		
		TRY
			{ 
			dialogPtr = new CMPrincipalCompDialog (); 
			
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
		CMPrincipalCompDialog* dialogPtr = NULL;

		dialogPtr = new CMPrincipalCompDialog (NULL);

		returnFlag = dialogPtr->DoDialog ();

		delete dialogPtr;
	#endif

	return (returnFlag);	

}	// end "PrincipalComponentDialog"    

									

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void PrincipalComponentDialogOK
//
//	Software purpose:	The purpose of this routine is to set up the principal
//							component structure parameters based on the selections 
//							that the user made in the dialog box.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			DoDialog in xPrincipalComponentsDialog.cpp
//							PrincipalComponentDialog
//
//	Coded By:			Larry L. Biehl			Date: 05/19/1997
//	Revised By:			Larry L. Biehl			Date: 05/19/1997	

void PrincipalComponentDialogOK (
				PrincipalCompSpecsPtr			principalCompSpecsPtr,
				SInt16								dataCode,
				SInt16								classSelection,
				UInt32								localNumberClasses,
				UInt16*								classListPtr,
				Boolean								trainingFlag,
				Boolean								testFlag,
				DialogSelectArea*					dialogSelectAreaPtr,
				SInt16								channelSelection,
				UInt32								localNumberChannels,
				UInt16*								localChannelsPtr,
				Boolean								listEigenvectorFlag,
				Boolean								equalVariancesFlag,
				Boolean								saveEigenvectorFlag,
				Boolean								textWindowFlag,
				Boolean								diskFileFlag)

{   
	UInt32								index;
	
	                                  
	if (principalCompSpecsPtr != NULL)
		{      
				// Principal component analysis for project classes data.																		
		
		if (dataCode == 0)                  		                               
			principalCompSpecsPtr->dataCode = kTrainingType;
			 
		else if (dataCode == 1)                  		                               
			principalCompSpecsPtr->dataCode = kAreaType;
					
				// Classes														
				
		principalCompSpecsPtr->classSet = classSelection;
				                               
		SInt16* classPtr = (SInt16*)GetHandlePointer (
															principalCompSpecsPtr->classHandle);
									
		if (gPrincipalCompSpecsPtr->dataCode == kTrainingType)
			{
			if (principalCompSpecsPtr->classSet == kAllMenuItem)
				LoadTrainClassVector (&principalCompSpecsPtr->numberClasses,
											&principalCompSpecsPtr->classSet, 
											classPtr);
											
			else	// ...->classSet == kSubsetMenuItem
				{                                              
				principalCompSpecsPtr->numberClasses = (UInt16)localNumberClasses;
				for (index=0; index<localNumberClasses; index++)
					classPtr[index] = classListPtr[index]; 
			
				}	// end "else ...->classSet == kSubsetMenuItem"
				                                            
			}	// end "if (...->dataCode == kTrainingType)"
													
				// Training and test field flags.					
					
		principalCompSpecsPtr->fieldTypeCode = 0;
		if (trainingFlag)
			principalCompSpecsPtr->fieldTypeCode += kTrainingType;
		if (testFlag)
			principalCompSpecsPtr->fieldTypeCode += kTestingType;
				
				// Selected area for principal components.						
				
		principalCompSpecsPtr->lineStart = dialogSelectAreaPtr->lineStart;
		principalCompSpecsPtr->lineEnd = dialogSelectAreaPtr->lineEnd;
		principalCompSpecsPtr->lineInterval = dialogSelectAreaPtr->lineInterval;
		principalCompSpecsPtr->columnStart = dialogSelectAreaPtr->columnStart;
		principalCompSpecsPtr->columnEnd = dialogSelectAreaPtr->columnEnd;
		principalCompSpecsPtr->columnInterval = dialogSelectAreaPtr->columnInterval;
					
				// Channels.											

		principalCompSpecsPtr->channelSet = channelSelection;											
				                                                  
		SInt16* featurePtr = (SInt16*)GetHandlePointer (
															gPrincipalCompSpecsPtr->featureHandle);
									
		if (principalCompSpecsPtr->channelSet == kAllMenuItem)	
					// All channels or features 
			LoadFeatureVector (kNoTransformation,
										(SInt16*)&principalCompSpecsPtr->numberChannels,
										featurePtr,
										gImageWindowInfoPtr->totalNumberChannels);
				
		else	// principalCompSpecsPtr->channelSet == kSubsetMenuItem 
			{				    		
			principalCompSpecsPtr->numberChannels = (UInt16)localNumberChannels; 
			for (index=0; index<localNumberChannels; index++)
				featurePtr[index] = localChannelsPtr[index];
												
			}	// end "else principalCompSpecsPtr->channelSet == kSubsetMenuItem" 
					
				// Load the principal component channels vector to be used 	
				// by GetLineOfData.	
									
		SInt16* channelsPtr = (SInt16*)GetHandlePointer (
															principalCompSpecsPtr->channelsHandle);													
															
		LoadChannelsVector (kNoProject,
									kNoTransformation,
									principalCompSpecsPtr->numberChannels, 
									featurePtr, 
									(SInt16*)&principalCompSpecsPtr->numberChannels,
									channelsPtr);
													
				// List eigenvectors.										
				
		principalCompSpecsPtr->listEigenvectorFlag = listEigenvectorFlag;  
													
				// Equalize variances.										
				
		principalCompSpecsPtr->equalVariancesFlag = equalVariancesFlag;  
													
				// Equalize variances.										
				
		principalCompSpecsPtr->saveEigenvectorFlag = saveEigenvectorFlag;  
													
				// Write output to output text window.					
						
		principalCompSpecsPtr->outputStorageType = 0;							
		if (textWindowFlag)						
			principalCompSpecsPtr->outputStorageType += 1;
													
				// Write output to disk file.								
				                                     							
		if (diskFileFlag)							
			principalCompSpecsPtr->outputStorageType += 2; 
											
		}	// end "if (principalCompSpecsPtr != NULL)"
	
}	// end "PrincipalComponentDialogOK"  
