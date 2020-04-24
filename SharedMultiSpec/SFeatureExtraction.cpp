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
//	File:						SFeatureExtraction.cpp
//
//	Authors:					Chulhee Lee
//								Larry L. Biehl
//
//	Revision date:			11/09/2019
//
//	Language:				C
//
//	System:					Linux, Macintosh and Windows Operating Systems
//
//	Brief description:	The purpose of the routines in this file are to extract
//								a set of new feature that will give nearly the same
//								performance of the input features but ideally with
//								significantly fewer.  Chulhee Lee developed the ideas for
//								the decision boundary method and the code.
//
//	Diagram of MultiSpec routine calls for the routines in the file.
//
//		FeatureExtractionControl
//			GetProjectImageFileInfo (in SProject.cpp)*
//			LoadFeatureExtractionSpecs
//			
//			FeatureExtractionDialog
//			
//			GetProjectImageFileInfo (in SProject.cpp)*
//
//			VerifyProjectStatsUpToDate (in SProjectUtilities.cpp)*
//			LoadClassPairWeightVector
//			GetStatusDialog (in SDialogUtilities.cpp)
//			ListHeaderInfo (in SStrings.cpp)
//			CheckClassEnhancedStats (in SProjectUtilities.cpp)
//			ListChannelsUsed (in SStrings.cpp)
//			GetProjectClassWeightsIndex (in SProjectUtilities.cpp)
//			ListClassesUsed (in SProjectUtilities.cpp)
//			CheckNumberOfPixelsInClass (in SProjectUtilities.cpp)
//
//			FeatureExtraction
//				GetMemoryForListTransformation (in SMatrixUtilities.cpp)
//				ListPPOptions
//
//				Preprocess (in SProjectionPursuit.cpp)
//
//				ListFeatureExtractionOptions
//				ListSpecifiedStringNumber (in SStrings.cpp)
//				GetClassInfoStructure (in SProjectUtilities.cpp)
//				AssignClassInfoMemory (in SProjectUtilities.cpp)
//				GetIOBufferPointers (in SMemoryUtilities.cpp)
//				InitializeAreaDescription (in SUtilities.cpp)*
//				GetTransformedClassCovarianceMatrix (in StatCom.cpp)
//				GetTransformedMeanVector (in SMatrixUtilities.cpp)
//				CopySquareToTriangleMatrix (in SMatrixUtilities.cpp)
//				GetClassDataValues (in SProjectMatrixUtilities.cpp)
//				DisposeIOBufferPointers (in SMemoryUtilities.cpp)
//
//				FS_quick_feature_extraction (SFeatureExtractionMath.cpp)
//					FS_discriminant_analysis (SFeatureExtractionMath.cpp)
//					FS_decision_boundary (SFeatureExtractionMath.cpp)
//
//					NWFE (SFeatureExtractionMath.cpp)
//						SetupMatrixInversionMemory (SProjectMatrixUtilities.cpp)
//						GetProjectClassWeightsIndex (in SProjectUtilities.cpp)
//						GetTotalProbability (in SProjectUtilities.cpp)
//						ZeroMatrix (SMatrixUtilities.cpp)
//						NWFE_SumInvDistXLocalMean
//							NWFE_GetLocalMean
//							CheckSomeEvents (in MMultiSpec.c or SStubs.cpp)
//
//						NWFE_GetScatterMatrixWeight
//						NWFE_AddToScatterMatrixForClass_i
//						AddBxSymMatrixToSymMatrix
//							CopyLowerToUpperSquareMatrix (SMatrixUtilities.cpp)
//
//						RegularizeSymMatrix
//							CopyLowerToUpperSquareMatrix (SMatrixUtilities.cpp)
//
//						InvertSymmetricMatrix (SProjectMatrixUtilities.cpp)
//						MatrixMultiply (SMatrixUtilities.cpp)
//						CheckSomeEvents (in MMultiSpec.c or SStubs.cpp)
//						MatlabEigFunction
//						ReleaseMatrixInversionMemory (SProjectMatrixUtilities.cpp)
//
//				MatrixMultiply (in SMatrixUtilities.cpp)
//				BlockMoveData (SStubs.cpp)
//				GetClassMeanVector (in SProjectComputeStatistics.cpp)
//				GetTranformationFeatureMeans
//				ReleaseClassInfoMemory (in SProjectUtilities.cpp)
//				ListTransformationInformation (in SMatrixUtilities.cpp)
//				SaveTransformationMatrix (in SMatrixUtilities.cpp)
//
//			ListCPUTimeInformation (in SStrings.cpp)
//			UpdateOutputWScrolls (in MControls.c and)
//			CloseStatusDialog (in SDialogUtilities.cpp)
//			MInitCursor (in SUtility.cpp)
//			CheckAndUnlockHandle (in SMemoryUtilities.cpp)
//			UnlockProjectWindowInfoHandles (in SProject.cpp)
//
/*	
			Template used for debugging.
			int numberChars = sprintf ((char*)gTextString3,
														" SFeatExt: (): %s", 
														gEndOfLine);
			ListString ((char*)gTextString3, numberChars, gOutputTextH); 
*/																		
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"    

#if defined multispec_wx
	#include "xFeatureExtractionDialog.h"
	#include "xProjectionPursuitDialog.h"
#endif	// defined multispec_wx 

#if defined multispec_mac || defined multispec_mac_swift
	#define	IDC_WeightsPrompt				8
	#define	IDC_WeightsCombo				9
	#define	IDC_ClassPairWeightsPrompt	10 
	#define	IDC_ClassPairWeightsCombo	11
	#define	IDC_SpecialOptions			12
	#define	IDC_WithinClassThreshold	14
	#define	IDC_PercentAccuracyPrompt	20
	#define	IDC_PercentAccuracy			21

	#define	IDC_PP_Algorithm						5
	#define	IDC_FS_Algorithm						6
	#define	IDC_NumericalOptimization			7
	#define	IDC_OptimizationThresholdPrompt	8
	#define	IDC_OptimizationThreshold			9
	#define	IDC_UniformGrouping					12
	#define	IDC_LastFinalGrouping				13
	#define	IDC_EditNumberFeatures				14
	#define	IDC_SetFeatures						16
	#define	IDC_Uniform								20
	#define	IDC_TopDownMethod						21
	#define	IDC_TopDownBottomUpMethod			22
	#define	IDC_TopDownThresholdPrompt			23
	#define	IDC_TopDownThreshold					24
	#define	IDC_BottomUpThresholdPrompt		26
	#define	IDC_BottomUpThreshold				27
	#define	IDC_BothChoicesLabel1				29
	#define	IDC_BothChoicesUpToValue			30
	#define	IDC_BothChoicesLabel2				31
	
	SInt16	gFEAlgorithmCode = 0;
#endif	// defined multispec_mac || defined multispec_mac_swift
  
#if defined multispec_win   
	#include "WFeatureExtractionDialog.h"
	#include "WProjectionPursuitDialog.h"
#endif	// defined multispec_win    


	
			// Prototypes for routines in this file that are only called by		
			// other routines in this file.		
			
PascalVoid	 		DrawFEAlgorithmPopUp (
							DialogPtr							dialogPtr, 
							SInt16								itemNumber);		
			
PascalVoid	 		DrawPreprocessingPopUp (
							DialogPtr							dialogPtr, 
							SInt16								itemNumber);	

Boolean	 			FeatureExtraction (void);

Boolean				FeatureExtractionDialog (void); 
							
Boolean 				ListFeatureExtractionOptions (
							CMFileStream*						resultsFileStreamPtr);
							
Boolean 				ListPPOptions (
							CMFileStream*						resultsFileStreamPtr);
				
Boolean	 			LoadFeatureExtractionSpecs (void);



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawFEAlgorithmPopUp
//
//	Software purpose:	The purpose of this routine is to draw the
//							prompt and selection for the feature extraction algorithm
//							pop up menu.  This routine will also draw a drop
//							shadow box around the pop up selection.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/06/2001
//	Revised By:			Larry L. Biehl			Date: 11/06/2001	

PascalVoid DrawFEAlgorithmPopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{	
			// Use the generic pop up drawing routine.									
			
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpFEAlgorithmMenu, 
								gFEAlgorithmCode, 
								TRUE);
	
}	// end "DrawFEAlgorithmPopUp" 
#endif	// defined multispec_mac  



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawPreprocessingPopUp
//
//	Software purpose:	The purpose of this routine is to draw the weights
//							prompt and selection for the preprocessing
//							pop up menu.  This routine will also draw a drop
//							shadow box around the pop up selection.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/24/96
//	Revised By:			Larry L. Biehl			Date: 07/24/96	

PascalVoid DrawPreprocessingPopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{	
			// Use the generic pop up drawing routine.									
			
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpPreprocessMenu, 
								gPreprocessSelection, 
								TRUE);
	
}	// end "DrawPreprocessingPopUp" 
#endif	// defined multispec_mac  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean FeatureExtraction
//
//	Software purpose:	The purpose of this routine is to control the
//							computation for feature extraction. This routine
//							creates the data and loads the structures needed for
//							Chulhees routines.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	True if everything completed ok
//							False if something was wrong
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/22/1992
//	Revised By:			Larry L. Biehl			Date: 08/26/2010

Boolean FeatureExtraction (void)

{
	CMFileStream*						resultsFileStreamPtr = NULL;
	
	ChannelStatisticsPtr				channelStatsPtr;
										
	ClassInfoPtr						classInfoPtr = NULL;
	
	FileIOInstructionsPtr			fileIOInstructionsPtr;
	
	HDoublePtr							eigenValuePtr = NULL,
											eigenVectorPtr = NULL,
											preProcessTransformValuePtr = NULL,
											preProcessTransformVectorPtr = NULL,
											tempMatrixPtr = NULL;
										
	ProjectionPursuitSpecsPtr		projectionPursuitSpecsPtr;
	
	SInt16*								classPtr;
	
	UInt16 								*channelsPtr,
											*featurePtr;
	
	Handle								preProcBandGroupingHandle = NULL;
	
	SInt32								errorCode;
	
	UInt32								bytesNeeded,
											classStorage,
											index,
											numberChannels,
											numberClasses,
											numberFEFeatures,
											numberOutputCovarianceEntries,
											statClassNumber; 
											
	SInt16								covarianceCode,
											doubleDataValueCode,
											stringNumber;
	
	Boolean								changedFlag,
											continueFlag = TRUE;
										
										
			// Initialize local variables.
	
	fileIOInstructionsPtr = NULL;		
	projectionPursuitSpecsPtr = 
									&gFeatureExtractionSpecsPtr->projectionPursuitParams;
	
	classPtr = (SInt16*)GetHandlePointer (
												gFeatureExtractionSpecsPtr->classHandle);
			
	featurePtr = (UInt16*)GetHandlePointer (
												gFeatureExtractionSpecsPtr->featureHandle);
			
	channelsPtr = (UInt16*)GetHandlePointer (
												gFeatureExtractionSpecsPtr->channelsHandle);
													
	numberChannels = gFeatureExtractionSpecsPtr->numberChannels;
	numberFEFeatures = numberChannels;
	numberClasses = gFeatureExtractionSpecsPtr->numberClasses;
	numberOutputCovarianceEntries = numberChannels * numberChannels;
	
			// Release memory used by last set of eigenvalues/eigenvectors			
			// if they exist.																		
			
	gFeatureExtractionSpecsPtr->eigenValueHandle = 
					UnlockAndDispose (gFeatureExtractionSpecsPtr->eigenValueHandle);
	gFeatureExtractionSpecsPtr->eigenVectorHandle = 
					UnlockAndDispose (gFeatureExtractionSpecsPtr->eigenVectorHandle);
	
			// Get pointer to buffer to use to store data values 				
			// to be listed. 
			
	if (continueFlag)
		continueFlag = GetMemoryForListTransformation (
										(UInt16)numberChannels, 
										gFeatureExtractionSpecsPtr->listTransformationFlag);
							
			// List the Feature Extraction Preprocessing specifications.	
				
	if (continueFlag)
		continueFlag = ListPPOptions (resultsFileStreamPtr);
		
	if (continueFlag &&
						gFeatureExtractionSpecsPtr->preprocessCode != kNoPreprocess)
		{
		if (gFeatureExtractionSpecsPtr->preprocessCode == kComputeNew)
			{
					// Run the preprocessing algorithm.
					           
			#if defined multispec_mac 
		   	gMemoryTypeNeeded = 1;
				continueFlag = Preprocess (); 
			#endif	// defined multispec_mac
									
			#if defined multispec_win || defined multispec_wx
				continueFlag = Preprocess ();
			#endif	// defined multispec_win  || defined multispec_wx
			
			}	// end "if (...->preprocessCode == kComputeNew)"
			
		numberFEFeatures = projectionPursuitSpecsPtr->finalNumberFeatures;
		
		if (gFeatureExtractionSpecsPtr->preprocessCode == kUseLast)
			numberChannels = projectionPursuitSpecsPtr->numberFeatureChannels;
		
		}	// end "if (continueFlag && ...->preprocessCode != kNoPreprocess)"
		
			// Get memory for eigenvectors.	
								
	bytesNeeded = numberOutputCovarianceEntries * sizeof (double);
	if (continueFlag)
		eigenVectorPtr = (HDoublePtr)CheckHandleSize (
								&gFeatureExtractionSpecsPtr->eigenVectorHandle,
								&continueFlag,
								&changedFlag, 
								bytesNeeded);
	
			// Get memory for eigenvalues.													
			
	bytesNeeded = (SInt32)numberChannels * 2 * sizeof (double);
	if (continueFlag)
		eigenValuePtr = (HDoublePtr)CheckHandleSize (
								&gFeatureExtractionSpecsPtr->eigenValueHandle,
								&continueFlag,
								&changedFlag, 
								bytesNeeded);
	
			// Set eigenValue vector to 0's	
						
	if (continueFlag)	
		ZeroMatrix (eigenValuePtr, 1, 2*numberChannels, TRUE);
	
			// Get the preprocess transformation vector and value pointer 
			// if it is to be used.
			
	if (gFeatureExtractionSpecsPtr->preprocessCode != kNoPreprocess)
		{							
		preProcessTransformVectorPtr = (HDoublePtr)GetHandlePointer (
							projectionPursuitSpecsPtr->transformVectorHandle,
							kLock);
							
		preProcessTransformValuePtr = (HDoublePtr)GetHandlePointer (
							projectionPursuitSpecsPtr->transformValueHandle,
							kLock);
							
		}	// end "if (...->preprocessCode != kNoPreprocess)" 
		  
	if (continueFlag)
		continueFlag = ListFeatureExtractionOptions (resultsFileStreamPtr);
										
			// List "  Output Information:".
			
	continueFlag = ListSpecifiedStringNumber (kSharedStrID, 
																IDS_Shared8, 
																(unsigned char*)gTextString, 
																resultsFileStreamPtr, 
																gOutputForce1Code, 
																continueFlag);
	
	if (continueFlag)
		{
				// Get memory for the structure which contains pointers to the 	
				// class statistics and data values.	
				
		classInfoPtr = GetClassInfoStructure (
												(UInt16)gFeatureExtractionSpecsPtr->numberClasses);
		continueFlag = (classInfoPtr != NULL);
		
		}	// end "if (continueFlag)" 
	
	if (continueFlag)
		{
				// Assign memory for classInfo structure 
				
		doubleDataValueCode = 0;
		if (gFeatureExtractionSpecsPtr->algorithmCode == kDecisionBoundary ||
					gFeatureExtractionSpecsPtr->algorithmCode == kNonparametricWeighted)
			doubleDataValueCode = kAssignMemory+kNumberFeatures;	
				
		covarianceCode = 0;
		if (gFeatureExtractionSpecsPtr->algorithmCode != kPreprocessingOnly &&
				gFeatureExtractionSpecsPtr->algorithmCode != kNonparametricWeighted)
			covarianceCode = 
						kAssignMemory+kNumberFeatures+kSquare+kAllowLoadingSquare;	
				
		SInt16 inverseCode = 0;
		if (gFeatureExtractionSpecsPtr->algorithmCode == kDecisionBoundary &&
				gFeatureExtractionSpecsPtr->optimizeClassFlag)
			inverseCode = kAssignMemory+kNumberFeatures+kTriangular;
			
		SInt16 meanCode = kAssignMemory+kNumberChannels;	
		//if (gFeatureExtractionSpecsPtr->algorithmCode != kNonparametricWeighted)
		//	meanCode = kAssignMemory+kNumberChannels;
		
		continueFlag = AssignClassInfoMemory (
											classInfoPtr,
											classPtr, 
											(SInt32)numberClasses,
											(SInt32)numberChannels,
											(SInt32)numberFEFeatures,
											doubleDataValueCode,	// double data Values
											covarianceCode,		// covariances - square matrix
											inverseCode,			// inverses - triangle matrix
											meanCode, 				// means
											0,							// no transformed cov
											0);						// no transformed means
		
		}	// end "if (continueFlag)" 
			
	if (continueFlag)
		{
					// Get pointer to memory to use for class channel statistics	
					// that will be used in computing the covariance matrix and		
					// getting the data for the class mean vector.						
					
		channelStatsPtr = (ChannelStatisticsPtr)MNewPointer (
											numberChannels * sizeof (ChannelStatistics));
		continueFlag = (channelStatsPtr != NULL);	
		
		}	// end "if (continueFlag)"
	
	if (continueFlag && 
						gFeatureExtractionSpecsPtr->algorithmCode != kPreprocessingOnly)
		{	
		ShowStatusDialogItemSet (kStatusTitle2);
		HideDialogItem (gStatusDialogPtr, IDC_Status22);
		LoadDItemStringNumber (kFeatureExtractStrID, 
										IDS_FeatureExtract12,
										gStatusDialogPtr, 
										IDC_Status21, 
										(Str255*)gTextString);
	
		if (continueFlag && doubleDataValueCode > 0)
			{			
					// Get buffers to read data from image file into.					
				 				
			continueFlag = GetIOBufferPointers (&gFileIOInstructions[0],
															gImageWindowInfoPtr,
															gImageLayerInfoPtr,
															gImageFileInfoPtr,
															&gInputBufferPtr,
															&gOutputBufferPtr,
															1,
															gImageWindowInfoPtr->maxNumberColumns,
															1,
															(UInt16)numberChannels,
															channelsPtr,
															kPackData,
															kForceBISFormat,
															kForceReal8Bytes,
															kDoNotAllowForThreadedIO,
															&fileIOInstructionsPtr);
															
			}	// end "if (continueFlag && doubleDataValueCode > 0)"
		
		if (continueFlag)
			{	
			if (preProcessTransformVectorPtr != NULL)
				{
						// Get memory for temporary matrix needed to transform the
						// covariance matrices.
						
				tempMatrixPtr = (HDoublePtr)MNewPointer (
							(UInt32)numberChannels * numberChannels * sizeof (double));
				continueFlag = (tempMatrixPtr != NULL);
				
				}	// end "if (if (preProcessTransformVectorPtr != NULL))"
				
			}	// end "if (continueFlag)"
							
		if (continueFlag)
			{	
					// Turn spin cursor on

			gPresentCursor = kSpin;

					// Load the class covariance matrix and mean vector.
					
			InitializeAreaDescription (&gAreaDescription);

			ShowStatusDialogItemSet (kStatusClassA);
			LoadDItemValue (gStatusDialogPtr, IDC_Status5, (SInt32)numberClasses);
			
			for (index=0; index<numberClasses; index++)
				{
				statClassNumber = classPtr[index] - 1;
				classStorage = gProjectInfoPtr->storageClass[statClassNumber];
							
				LoadDItemValue (gStatusDialogPtr, IDC_Status3, (SInt32)statClassNumber+1);
				LoadDItemString (
						gStatusDialogPtr,
						IDC_Status6,
						(Str255*)gProjectInfoPtr->classNamesPtr[classStorage].name);
					
				if (covarianceCode > 0)
					{
					GetTransformedClassCovarianceMatrix (
															(UInt16)numberChannels,
															channelStatsPtr,
															classInfoPtr[index].covariancePtr,
															featurePtr,
															(UInt16)statClassNumber,
															kSquareOutputMatrix,
															kMeanCovariance,
															preProcessTransformVectorPtr,
															tempMatrixPtr,
															(UInt16)numberFEFeatures);
								
							// Get the transformed class mean vector													
					
					GetTransformedMeanVector (channelStatsPtr, 
														classInfoPtr[index].meanPtr,
														(SInt16)numberChannels,
														NULL,
														preProcessTransformVectorPtr,
														tempMatrixPtr,
														NULL,
														(SInt16)numberFEFeatures);
								
					if (gFeatureExtractionSpecsPtr->algorithmCode == kDecisionBoundary &&  
												gFeatureExtractionSpecsPtr->optimizeClassFlag)
						CopySquareToTriangleMatrix (
											(UInt16)numberFEFeatures,
											classInfoPtr[index].covariancePtr, 
											classInfoPtr[index].inversePtr);
											
					}	// end "if (covarianceCode > 0)"
										
						// Get the number of pixels in the class.							
							
				classInfoPtr[index].no_samples = 
						gProjectInfoPtr->classNamesPtr[classStorage].numberStatisticsPixels;
									
						// Get the pixels values for the the class.							
				
				if (doubleDataValueCode > 0)
					{
					classInfoPtr[index].no_samples = GetClassDataValues (
												fileIOInstructionsPtr,
												(SInt16)statClassNumber,
												channelsPtr,
												(SInt16)numberChannels,
												classInfoPtr[index].dataValuesPtr,
												preProcessTransformVectorPtr,
												&preProcessTransformValuePtr[numberFEFeatures],
												(UInt16)numberFEFeatures,
												kComputeColumnInterval);
							
					continueFlag = (classInfoPtr[index].no_samples >= 0);
							
					if (classInfoPtr[index].no_samples == 0)
						{										
						continueFlag = ListSpecifiedStringNumber (kFeatureExtractStrID,
																				IDS_FeatureExtract11,
																				NULL,
																				gOutputForce1Code,
																				(SInt32)classPtr[index],
																				continueFlag);
							
						gOperationCanceledFlag = TRUE;
							
						}	// end "if (classInfoPtr[index].no_samples == 0)" 
							
					if (continueFlag)
						{
								// Adjust the pointer size if needed.							
							
						if (classInfoPtr[index].no_samples < 
								gProjectInfoPtr->classNamesPtr[classStorage].numberStatisticsPixels)
							MSetPointerSize ((Ptr*)&classInfoPtr[index].dataValuesPtr,
										(SInt64)classInfoPtr[index].no_samples * 
															numberFEFeatures * sizeof (double));
															
						}	// end "if (continueFlag)" 
															
					}	// end "if (doubleDataValueCode > 0)" 
					
				if (!continueFlag)
					break;
							
				}	// end "for (index=0; index<numberClasses; index++)"
				
			HideStatusDialogItemSet (kStatusClassA);
				
			}	// end "if (continueFlag)" 
	
		if (continueFlag)
			continueFlag = !gOperationCanceledFlag;
				
				// Dispose of the IO buffer.		
									
		DisposeIOBufferPointers (fileIOInstructionsPtr,
											&gInputBufferPtr, 
											&gOutputBufferPtr);
	
		if (continueFlag)
			{		
			HDoublePtr		feEigenVectorPtr = eigenVectorPtr;
			if (tempMatrixPtr != NULL)
				feEigenVectorPtr = tempMatrixPtr;
				
			FS_quick_feature_extraction (
							numberFEFeatures,
							numberClasses,
							classInfoPtr,
							(SInt32)gFeatureExtractionSpecsPtr->algorithmCode,
							feEigenVectorPtr,
							eigenValuePtr,
							&errorCode);
							
			stringNumber = 0;
			if (errorCode > 600)
				stringNumber = IDS_FeatureExtract66;
				
			else if (errorCode == 590)
				stringNumber = IDS_FeatureExtract67;  
			
			if (stringNumber > 0)
				continueFlag = ListSpecifiedStringNumber (
												kFeatureExtractStrID, 
												stringNumber, 
												(unsigned char*)gTextString,
												resultsFileStreamPtr, 
												gOutputForce1Code,
												continueFlag);
			
			if (continueFlag)	
				continueFlag = (errorCode == 0 || errorCode == 590);
				
						// Get the combined Preprocess and Feature Extraction 
						// Transformation Matrix.
				
			if (continueFlag && tempMatrixPtr != NULL)
				MatrixMultiply (tempMatrixPtr, 
										preProcessTransformVectorPtr, 
										eigenVectorPtr,
										(UInt16)numberFEFeatures, 
										(UInt16)numberFEFeatures, 
										(UInt16)numberChannels, 
										kMat1ByMat2);
			
			}	// end "if (continueFlag)"
									
		}	// end "if (continueFlag && ..."
		
	if (continueFlag && 
						gFeatureExtractionSpecsPtr->algorithmCode == kPreprocessingOnly)
		{
				// Copy preprocess transformation matrix to final feature
				// extraction transformation matrix location.
				
		UInt32 numberBytes = numberChannels * numberFEFeatures * sizeof (double);
		BlockMoveData (preProcessTransformVectorPtr, eigenVectorPtr, numberBytes);
		
				// Copy preprocess factor vector to final feature
				// extraction factor vector location.
				
		numberBytes = numberFEFeatures * sizeof (double);
		BlockMoveData (preProcessTransformValuePtr, eigenValuePtr, numberBytes);
		
		preProcBandGroupingHandle = projectionPursuitSpecsPtr->bandGroupingHandle;
				
		}	// end "else ...->algorithmCode == kPreprocessingOnly"
		
	if (continueFlag)
		{
				// Get the original class mean vectors	
			
		for (index=0; index<numberClasses; index++)
			{
			statClassNumber = classPtr[index] - 1;
				
			GetClassMeanVector ((UInt16)numberChannels, 
										channelStatsPtr, 
										classInfoPtr[index].meanPtr, 
										featurePtr, 
										(UInt16)statClassNumber);							
							
			classStorage = gProjectInfoPtr->storageClass[statClassNumber];
			classInfoPtr[index].no_samples = 
					gProjectInfoPtr->classNamesPtr[classStorage].numberStatisticsPixels;
						
			}	// end "for (index=0; index<numberClasses; index++)" 
			
		}	// end "if (continueFlag)"
		
			// Dispose of memory that we do not need any more.
				
	channelStatsPtr = (ChannelStatisticsPtr)CheckAndDisposePtr (
																(Ptr)channelStatsPtr);
				
	tempMatrixPtr = CheckAndDisposePtr (tempMatrixPtr);
	CheckAndUnlockHandle (projectionPursuitSpecsPtr->transformVectorHandle);
	CheckAndUnlockHandle (projectionPursuitSpecsPtr->transformValueHandle);
	preProcessTransformVectorPtr = NULL;
			
	if (continueFlag)
		{
				// Save the transformation feature means.
		
		GetTranformationFeatureMeans (classInfoPtr,
												(UInt16)numberClasses,
												(UInt16)numberChannels,
												(UInt16)numberFEFeatures,
												eigenVectorPtr,
												&eigenValuePtr[numberFEFeatures]);
												
		}	// end "if (continueFlag)"

			// Turn spin cursor off

	gPresentCursor = kWait;
	
	ReleaseClassInfoMemory (classInfoPtr, (UInt16)numberClasses);
	
	if (continueFlag)
		{
		if (gCharBufferPtr1 != NULL)
			{
			Boolean  listEigenValueFlag = TRUE;
			
			eigenValuePtr = (HDoublePtr)GetHandlePointer (
													gFeatureExtractionSpecsPtr->eigenValueHandle);
													
			eigenVectorPtr = (HDoublePtr)GetHandlePointer (
													gFeatureExtractionSpecsPtr->eigenVectorHandle);
			
			continueFlag = ListSpecifiedStringNumber (
									kFeatureExtractStrID, 
									IDS_FeatureExtract36, 
									(unsigned char*)gTextString,
									resultsFileStreamPtr, 
									gOutputForce1Code,
									continueFlag);
			
			if (continueFlag)
				{
				UInt32 numberFeaturesToList = numberFEFeatures;
				if (gFeatureExtractionSpecsPtr->algorithmCode == kDiscriminantAnalysis)
					numberFeaturesToList = MIN (numberFEFeatures, numberClasses-1);
				
				continueFlag = ListTransformationInformation (
									eigenValuePtr, 
									eigenVectorPtr, 
									0, 
									0,
									channelsPtr,
									(UInt16)numberChannels,
									(UInt16)numberFeaturesToList,
									listEigenValueFlag,
									gFeatureExtractionSpecsPtr->listTransformationFlag,
									2);
									
				}	// end "if (continueFlag)"
			
			}	// end "if (gCharBufferPtr1 != NULL)" 
		
				// Save the new transformation matrix information.							
				
		continueFlag = SaveTransformationMatrix (
													2,
													gFeatureExtractionSpecsPtr->eigenVectorHandle,
													gFeatureExtractionSpecsPtr->eigenValueHandle,
													gFeatureExtractionSpecsPtr->channelsHandle,
													preProcBandGroupingHandle,
													gFeatureExtractionSpecsPtr->numberChannels,
													(UInt16)numberFEFeatures);
		
		}	// end "if (continueFlag)"
			
			// Dispose of the buffer used to list a line of information.																			
			
	gCharBufferPtr1 = CheckAndDisposePtr (gCharBufferPtr1);
		
			// Only dispose of the eigenvalue and eigenvector handles if they
			// were not saved to the global transformation matrix. 
	
	if (!continueFlag)
		{
		UnlockAndDispose (gFeatureExtractionSpecsPtr->eigenValueHandle);
		UnlockAndDispose (gFeatureExtractionSpecsPtr->eigenVectorHandle);
		
		}	// end "if (!continueFlag)"
		
	gFeatureExtractionSpecsPtr->eigenValueHandle = NULL;
	gFeatureExtractionSpecsPtr->eigenVectorHandle = NULL;
		
	return (continueFlag);

}	// end "FeatureExtraction" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void FeatureExtractionControl
//
//	Software purpose:	The purpose of this routine is to handle the
//							"Feature Extraction" event.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/22/1992
//	Revised By:			Larry L. Biehl			Date: 05/03/2019

void FeatureExtractionControl (void)

{
	CMFileStream*						resultsFileStreamPtr;
	
	SInt16								*channelsPtr,
											*classPairWeightsPtr,
											*classPtr,
											*weightsListPtr;
	
	time_t								startTime;
	
	SInt16								localWeightsIndex;
	
	Boolean								continueFlag = TRUE;
	
		    		
			// If spare memory had to be used to load code resources, then exit	
			// routine.																				
			
	if (gMemoryTypeNeeded < 0)
																							return;
																							
			// Code resources loaded okay, so set flag back for non-Code			
			// resources.																			
			
	gMemoryTypeNeeded = 0;
 			
 			// Initialize variables.															
 			
 	gStatusDialogPtr = NULL;
 	gFeatureExtractionSpecsPtr = NULL;
 	resultsFileStreamPtr = NULL;
 	weightsListPtr = NULL;
	
			// Check handle to image file information.  If handle to image			
			// file information doesn't exists. Find the	image file and get		
			// the information for it.															
				
	if (GetProjectImageFileInfo (kDoNotPrompt, kSetupGlobalInfoPointersIfCan))
		{
				// Set up feature extraction specification structure.					
				
		if (LoadFeatureExtractionSpecs ())
			{																					
			continueFlag = FeatureExtractionDialog ();
			
			if (continueFlag)
				continueFlag = 
					(gFeatureExtractionSpecsPtr->preprocessCode != kNoPreprocess) ||
					(gFeatureExtractionSpecsPtr->algorithmCode != kPreprocessingOnly);
			
			if (continueFlag)
				{
						// If the decision boundary algorithm is to be used then
						// make sure that we have access to the base image file.
						// Prompt the user for it, if it can not be found.
						
				if (gFeatureExtractionSpecsPtr->algorithmCode == kDecisionBoundary)
					continueFlag = GetProjectImageFileInfo (kPrompt, 
																kSetupGlobalInfoPointers);
					
				}	// end "if (FeatureExtractionDialog ())"
	
			if (continueFlag)
				{
						// Initialize some global variables pertaining to output.	
				
				gOutputTextOKFlag = TRUE;
				gOutputCode = 0x0001;
				gOutputForce1Code = (gOutputCode | 0x0001);
				continueFlag = TRUE;
				
				weightsListPtr = (SInt16*)GetHandlePointer (
														gProjectInfoPtr->classPairWeightsListHandle);
														
				classPtr = (SInt16*)GetHandlePointer (
														gFeatureExtractionSpecsPtr->classHandle);
														
				classPairWeightsPtr = (SInt16*)GetHandlePointer (
														gProjectInfoPtr->classPairWeightsHandle);
														
				channelsPtr = (SInt16*)GetHandlePointer (
														gFeatureExtractionSpecsPtr->channelsHandle);
					
						// Update statistics for project if needed	
						
				continueFlag = VerifyProjectStatsUpToDate (
									&gFeatureExtractionSpecsPtr->numberClasses,
									classPtr,
									2,
									gProjectInfoPtr->statisticsCode,
									gProjectInfoPtr->covarianceStatsToUse, 
									kSetupGlobalInfoPointers,
									NULL);
									
				if (continueFlag)						
					LoadClassPairWeightVector (
									classPairWeightsPtr,
									(SInt32)gProjectInfoPtr->numberProjectClassPairs, 
									gProjectInfoPtr->defaultClassPairWeight,
									&gProjectInfoPtr->classPairWeightSum,
									gProjectInfoPtr->classPairWeightSet,
									weightsListPtr,
									classPtr,
									(UInt16)gFeatureExtractionSpecsPtr->numberClasses);
	 		
						// Continue feature extraction if everything is okay.			
				
				if (continueFlag)
					{
							// Change cursor to watch cursor until done with process.
						
					MSetCursor (kWait);
			
							// Force text selection to start from end of present text.
							
					ForceTextToEnd ();
									
					}	// end "if (continueFlag)" 
						
						// Get dialog box to display feature extraction status.			
				
				if (continueFlag)
					{
					gStatusDialogPtr = GetStatusDialog (kUpdateStatsInfoID, TRUE);
					
					continueFlag = (gStatusDialogPtr != NULL);
					
					}	// end "if (continueFlag)" 
					
				if (continueFlag)
					LoadDItemStringNumber (kFeatureExtractStrID,
													IDS_FeatureExtract3,
													gStatusDialogPtr, 
													IDC_Status11, 
													(Str255*)gTextString);
					
						// List the processor name, date, time and project info.			

				continueFlag = ListHeaderInfo (
							resultsFileStreamPtr, 
							kLImageInfo+kLProjectName+kLProjectImageName+kLStatType, 
							&gOutputForce1Code, 
							gProjectInfoPtr->covarianceStatsToUse, 
							continueFlag);
							
						// If enhanced statistics to be used, verify that they exist 
						// for the requested classes. If not list a message.
				
				if (continueFlag &&
										gProjectInfoPtr->covarianceStatsToUse == kEnhancedStats)
					continueFlag = CheckClassEnhancedStats (
								gFeatureExtractionSpecsPtr->numberClasses,
								classPtr); 
		
						// Insert a blank line after the table.
				
				continueFlag = OutputString (resultsFileStreamPtr, 
														gEndOfLine, 
														gNumberOfEndOfLineCharacters, 
														gOutputForce1Code, 
														continueFlag);
				
							// List the channels used.											
				
				if (continueFlag)
					continueFlag = ListChannelsUsed (
														gImageFileInfoPtr,
														NULL,
														channelsPtr,
														gFeatureExtractionSpecsPtr->numberChannels, 
														resultsFileStreamPtr,
														&gOutputForce1Code,
														kNoTransformation);
				
						// List the classes used.												
						
				localWeightsIndex = GetProjectClassWeightsIndex ();
									
				if (continueFlag)		
					continueFlag =  ListClassesUsed (NULL,
															gImageFileInfoPtr,
															classPtr, 
															kListNoSymbols,
															localWeightsIndex, 
															gFeatureExtractionSpecsPtr->numberClasses, 
															resultsFileStreamPtr, 
															&gOutputForce1Code,
															FALSE);
				
						// List Interclass weights.
						
				if (continueFlag && 
					(gFeatureExtractionSpecsPtr->algorithmCode != kDiscriminantAnalysis))
					{	
					if (gProjectInfoPtr->classPairWeightSet == kEqualWeights)
						continueFlag = ListSpecifiedStringNumber (
												kFeatureExtractStrID, 
												IDS_FeatureExtract37, 		// "    All class pairs will be used.%s"
												(unsigned char*)gTextString,
												resultsFileStreamPtr, 
												gOutputForce1Code,
												continueFlag);
									
					else	// gProjectInfoPtr->classPairWeightSet != kEqualWeights
						continueFlag = ListSpecifiedStringNumber (
												kFeatureExtractStrID, 
												IDS_FeatureExtract38, 	// "    A subset of the class pairs will be used.%s"
												(unsigned char*)gTextString,
												resultsFileStreamPtr, 
												gOutputForce1Code,
												continueFlag);
							
					}	// end "if (continueFlag && ..."
							
				if (continueFlag && 
						(gFeatureExtractionSpecsPtr->algorithmCode == kDecisionBoundary) &&
								(gFeatureExtractionSpecsPtr->preprocessCode == kNoPreprocess))
					continueFlag = CheckNumberOfPixelsInClass (
													gFeatureExtractionSpecsPtr->numberClasses, 
													classPtr, 
													gFeatureExtractionSpecsPtr->numberChannels);
				
				if (continueFlag)
					{
					startTime = time (NULL);
					
					continueFlag = FeatureExtraction ();
						
							// List the CPU time taken for feature extraction.			
							
					continueFlag = ListCPUTimeInformation (
												resultsFileStreamPtr, TRUE, startTime);
							
					}	// end "if (continueFlag)" 
				
						// Scroll output window to the selection and adjust the 		
						// scroll bar.																
				
				UpdateOutputWScrolls (gOutputWindow, 1, kDisplayMessage);
					
						// Dispose of status dialog box.										
				
				CloseStatusDialog (TRUE);
				
						// Change cursor back to pointer.									
							
				MInitCursor ();
					
				}	// end "if (FeatureExtractionDialog ())" 
				
			}	// end "if (continueFlag)" 
					
		if (gFeatureExtractionSpecsPtr)
			{	
			CheckAndUnlockHandle (gFeatureExtractionSpecsPtr->classHandle);
			CheckAndUnlockHandle (gFeatureExtractionSpecsPtr->channelsHandle);
			CheckAndUnlockHandle (gFeatureExtractionSpecsPtr->featureHandle);
			CheckAndUnlockHandle (gFeatureExtractionSpecsPtr->windowInfoHandle);
			
			CheckAndUnlockHandle (gProjectInfoPtr->classPairWeightsHandle);
			CheckAndUnlockHandle (gProjectInfoPtr->classPairWeightsListHandle);
				
			}	// end "if (gFeatureExtractionSpecsPtr)" 
			
				// Unlock memory for the feature extraction specifications.		
			
		CheckAndUnlockHandle (gProjectInfoPtr->featureExtractionSpecsH);
		gFeatureExtractionSpecsPtr = NULL;
			
		UnlockProjectWindowInfoHandles ();
		
		gOperationCanceledFlag = FALSE;
		
		}	// end "if (GetProjectImageFileInfo (..." 

}	// end "FeatureExtractionControl" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean FeatureExtractionDialog
//
//	Software purpose:	The purpose of this routine is to obtain the
//							user specifications for feature extraction processor.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	True if user selects OK in dialog box
//							False if user selects Cancel in dialog box
//
// Called By:			FeatureExtractionControl
//
//	Coded By:			Larry L. Biehl			Date: 11/02/1992
//	Revised By:			Larry L. Biehl			Date: 12/28/1999
	
Boolean FeatureExtractionDialog (void)

{
	Boolean								returnFlag;
									
	#if defined multispec_mac
		Rect									theBox;
											
		double								optimizeThreshold,
												outlierThreshold1,
												outlierThreshold2,
												variable;
		
		DialogPtr							dialogPtr;
		FileInfoPtr							fileInfoPtr;
		float*								localClassWeightsPtr = NULL;
		
		SInt16								*localClassPairWeightsListPtr = NULL; 
		
		UInt16								*classPtr,
												*channelsPtr,
												*featurePtr,
												*localClassPtr = NULL,
												*localFeaturesPtr = NULL;
		
		ProjectionPursuitSpecsPtr 		projectionPursuitSpecsPtr;
		
		UserItemUPP							drawInterClassWeightsPopUpPtr,
												drawChannelsPopUp2Ptr,
												drawFEAlgorithmPopUpPtr,
												drawPreprocessPopUpPtr;
		
		Handle								okHandle,
												theHandle;
		
		SInt32								maximumClassNumber,
												theNum;
												
		UInt32								localNumberClasses,
												maxPixelsPerClass,
												minThresholdNumber;
												
		SInt16								channelListType,
												itemHit,
												localDefaultClassPairWeight,
												theType;
												
		UInt16								localNumberChannels;
		
		Boolean								listTransformationFlag,
												modalDone,
												optimizeClassFlag,
												specialOptionsFlag,
												tempFlag,
												updateSpecialOptionsFlag;

		
		fileInfoPtr = gImageFileInfoPtr;
		dialogPtr = NULL;
					
		returnFlag = GetDialogLocalVectors (&localFeaturesPtr,
															NULL,
															&localClassPtr,
															NULL,		// &localClassAreaPtr,
															&localClassWeightsPtr,
															NULL,
															NULL,
															&localClassPairWeightsListPtr);
															
		if (returnFlag)		
			dialogPtr = LoadRequestedDialog (
												kFeatureExtractionDialogID, kCopyScrap, 1, 2);
				
		if (dialogPtr == NULL)
			{
			ReleaseDialogLocalVectors (localFeaturesPtr,
												NULL,
												localClassPtr,
												NULL,
												localClassWeightsPtr,
												NULL,
												NULL,
												localClassPairWeightsListPtr);											
																					return (FALSE);
																					
			}	// end "if (dialogPtr == NULL)" 
			
				// Intialize local user item proc pointers.	
				
		drawInterClassWeightsPopUpPtr = NewUserItemUPP (DrawInterClassWeightsPopUp);
		drawChannelsPopUp2Ptr = NewUserItemUPP (DrawChannelsPopUp2);
		drawPreprocessPopUpPtr = NewUserItemUPP (DrawPreprocessingPopUp);
		drawFEAlgorithmPopUpPtr = NewUserItemUPP (DrawFEAlgorithmPopUp);
							
		//weightsListPtr = NULL;
		//if (gProjectInfoPtr->classPairWeightsListHandle != NULL)
		//	weightsListPtr = (UInt16*)GetHandlePointer (
		//												gProjectInfoPtr->classPairWeightsListHandle);
		
		classPtr = (UInt16*)*gFeatureExtractionSpecsPtr->classHandle;
		featurePtr = (UInt16*)*gFeatureExtractionSpecsPtr->featureHandle;
		channelsPtr = (UInt16*)*gFeatureExtractionSpecsPtr->channelsHandle;
				
		projectionPursuitSpecsPtr = 
							&gFeatureExtractionSpecsPtr->projectionPursuitParams;
		
				// Set Procedure pointers for those dialog items that need them.	
		
		SetDialogItemDrawRoutine (dialogPtr, 4, drawFEAlgorithmPopUpPtr);												
				
		SetDialogItemDrawRoutine (dialogPtr, 9, gDrawWeightsPopUpPtr);							
				
		SetDialogItemDrawRoutine (dialogPtr, 11, drawInterClassWeightsPopUpPtr);
		
		SetDialogItemDrawRoutine (dialogPtr, 26, drawChannelsPopUp2Ptr);
			
		SetDialogItemDrawRoutine (dialogPtr, 28, gDrawDialogClassPopUpPtr);	
		
		SetDialogItemDrawRoutine (dialogPtr, 30, drawPreprocessPopUpPtr);

		FeatureExtractionDialogInitialize (dialogPtr,
															gFeatureExtractionSpecsPtr,
															localFeaturesPtr,
															localClassPtr,
															&localClassPairWeightsListPtr,
															&gFEAlgorithmCode,
															&gWeightsSelection,
															&gInterClassWeightsSelection,
															&localDefaultClassPairWeight,
															&specialOptionsFlag,
															&outlierThreshold1,
															&outlierThreshold2,
															&minThresholdNumber,
															&optimizeClassFlag,
															&optimizeThreshold,
															&maxPixelsPerClass,
															&gChannelSelection,
															&localNumberChannels,
															&channelListType,
															&gClassSelection,
															&localNumberClasses,
															&gPreprocessSelection,
															&listTransformationFlag);			
																
				// Save handle for the OK button for use later.								
				
		GetDialogItem (dialogPtr, 1,  &theType, &okHandle, &theBox);
		
				// Set the algorithm controls	
		
		//DisableMenuItem (gPopUpFEAlgorithmMenu, kNonparametricWeighted);
		FeatureExtractionDialogAlgorithm (dialogPtr, gFEAlgorithmCode);
		
		SetDLogControl (dialogPtr, 12, specialOptionsFlag);
		
				// outlierThreshold1 variable; used for decision boundary.				
			
		LoadDItemRealValue (dialogPtr, 14, outlierThreshold1, 2);
		
				// outlierThreshold2 variable; used for decision boundary.				
			
		LoadDItemRealValue (dialogPtr, 16, outlierThreshold2, 2);
		
				// minimumThresholdNumber variable; used for decision boundary.		
			
		LoadDItemValue (dialogPtr, 18, minThresholdNumber);
		
				// Set 'Optimize class flag'.														
				
		SetDLogControl (dialogPtr, 19, (optimizeClassFlag));
		
				// optimizeClassThreshold variable; used for decision boundary.		
			
		LoadDItemRealValue (dialogPtr, 21, optimizeThreshold, 1);
		
				// Set maximum number of pixels desired per class.							
			
		maximumClassNumber = 100000;
		LoadDItemValue (dialogPtr, 23, maxPixelsPerClass);	
		
				// Preprocessing selection		
		
		if (projectionPursuitSpecsPtr->finalNumberFeatures > 0)
			EnableMenuItem (gPopUpPreprocessMenu, kUseLast+1);

		else	// projectionPursuitSpecsPtr->finalNumberFeatures == 0
			DisableMenuItem (gPopUpPreprocessMenu, kUseLast+1);
		
				// Set 'transformation matrix' flag.										
				
		SetDLogControl (dialogPtr, 31, listTransformationFlag);
			
				// Update the modal dialog with the default settings	
				// Center the dialog and then show it.											
				
		ShowDialogWindow (
							dialogPtr, kFeatureExtractionDialogID, kSetUpDFilterTable);
			
		gDialogItemDescriptorPtr[14] = kDItemReal;
		gDialogItemDescriptorPtr[16] = kDItemReal;
		gDialogItemDescriptorPtr[21] = kDItemReal;
			
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
					case 4:		// Feature Extraction algorithm.
						itemHit = StandardPopUpMenu (dialogPtr, 
																3, 
																4, 
																gPopUpFEAlgorithmMenu, 
																gFEAlgorithmCode, 
																kPopUpFEAlgorithmMenuMenuID);
																
						if (itemHit != 0 && itemHit != gFEAlgorithmCode)
							{								
							gFEAlgorithmCode = itemHit;
							FeatureExtractionDialogAlgorithm (dialogPtr, gFEAlgorithmCode);
							updateSpecialOptionsFlag = TRUE;
							
							}	// end "if (itemHit != 0 && itemHit != gFEAlgorithmCode)"
							
						InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
							
						break;
						
					case 9:		// Use equal class weights 
						itemHit = StandardPopUpMenu (dialogPtr, 
																8, 
																9, 
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
						
					case 11:		// Interclass Weights 
						itemHit = StandardPopUpMenu (dialogPtr, 
																10, 
																11, 
																gPopUpWeightsMenu, 
																gInterClassWeightsSelection, 
																kPopUpWeightsMenuID);
														
						if (itemHit == kUnequalWeightMenuItem)
							{
									// Unequal weights to be used.								
									
							HiliteControl ((ControlHandle)okHandle, 255);
							
							gInterClassWeightsSelection = ClassPairWeightsDialog (
												(UInt16)gProjectInfoPtr->numberStatisticsClasses,
												(SInt16**)&localClassPairWeightsListPtr,
												gInterClassWeightsSelection,
												&localDefaultClassPairWeight);
							
							HiliteControl ((ControlHandle)okHandle, 0);
									
							itemHit = gInterClassWeightsSelection;
								
							}	// end "if (itemHit == kUnequalWeightMenuItem)" 
						
						if (itemHit != 0)
							gInterClassWeightsSelection = itemHit;
		
								// Make certain that the correct label is drawn in the	
								// class pop up box.													
						
						InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
						break;
						
					case 12:		// check box for "special options" 
						specialOptionsFlag = !specialOptionsFlag;
						SetControlValue (
										(ControlHandle)theHandle, (SInt16)specialOptionsFlag);
						updateSpecialOptionsFlag = TRUE;
						break;
						
					case 14:				// OutlierThreshold1 variable	
						variable = GetDItemRealValue (dialogPtr, itemHit);
						if (variable > 1.0)
							RealNumberErrorAlert (outlierThreshold1, dialogPtr, itemHit, 2);
						if (variable > 0 && variable <= 1.0)
							outlierThreshold1 = variable;
						break;
						
					case 16:				// OutlierThreshold2 variable	
						variable = GetDItemRealValue (dialogPtr, itemHit);
						if (variable > 1.0)
							RealNumberErrorAlert (outlierThreshold2, dialogPtr, itemHit, 2);
						if (variable > 0 && variable <= 1.0)
							outlierThreshold2 = variable;
						break;
						
					case 18:				// Minimum threshold number variable	
						if (theNum == 0 || theNum > SInt16_MAX)	
							NumberErrorAlert ((SInt32)minThresholdNumber, dialogPtr, itemHit);
							
						else
							minThresholdNumber = theNum;		
						break;
						
					case 19:		// check box for "optimize class flag" 
						optimizeClassFlag = !optimizeClassFlag;
						FeatureExtractionDialogOptimizeClass (dialogPtr, optimizeClassFlag);
						break;
						
					case 21:				// Optimize threshold variable	
						variable = GetDItemRealValue (dialogPtr, itemHit);
						if (variable > 100.0)
							RealNumberErrorAlert (optimizeThreshold, dialogPtr, itemHit, 1);
						if (variable > 0 && variable <= 100.0)
							optimizeThreshold = variable;
						break;
						
					case 23:		// maximum number of pixels per class" 
						if (theNum == 0 || theNum > maximumClassNumber)	
							NumberErrorAlert (maxPixelsPerClass, dialogPtr, itemHit);
							
						else
							maxPixelsPerClass = theNum;	
						break;
						
					case 26:		// Channels 
						itemHit = ChannelsPopUpMenu (dialogPtr,
																26,
																1, 
																gChannelSelection, 
																channelListType);
																
						if (itemHit == kSubsetMenuItem || 
								(itemHit == 1 && channelListType == kSelectedItemsListOnly))
							{
									// Subset of channels to be used.							
									
							HiliteControl ((ControlHandle)okHandle, 255);
							
							ChannelsDialog ((SInt16*)&localNumberChannels,
													(SInt16*)localFeaturesPtr,
													gImageLayerInfoPtr,
													fileInfoPtr,
													channelListType,
													kNoTransformation,
													(SInt16*)gProjectInfoPtr->channelsPtr,
													gProjectInfoPtr->numberStatisticsChannels,
													gChannelSelection);
							
							HiliteControl ((ControlHandle)okHandle, 0);
														
							if (localNumberChannels ==
													gProjectInfoPtr->numberStatisticsChannels)
								itemHit = 1;
								
							}	// end "if (itemHit == kSubsetMenuItem || ...)" 
						
						if (channelListType == kSelectItemsList && itemHit != 0)
							gChannelSelection = itemHit;
		
								// Make certain that the correct label is drawn in the	
								// channel pop up box.												
						
						InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
						break;
						
					case 28:		// Classes 
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
										
							LoadTrainClassVector (&gFeatureExtractionSpecsPtr->numberClasses,
															&gClassSelection, 
															(SInt16*)localClassPtr);
		
								// Make certain that the correct label is drawn in the	
								// class pop up box.													
						
						InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
						break;
						
					case 30:		// Preprocessing options
						itemHit = StandardPopUpMenu (dialogPtr, 
																29, 
																30, 
																gPopUpPreprocessMenu, 
																gPreprocessSelection, 
																kPopUpPreprocessMenuID);
											
						tempFlag = (itemHit != 0);					
						if (itemHit == 2)
							{
									// Get preprocessing options.								
									
							HiliteControl ((ControlHandle)okHandle, 255);
							tempFlag = ProjectionPursuitDialog ();
							HiliteControl ((ControlHandle)okHandle, 0);
								
							}	// end "if (itemHit == 2 || ...)"
														
						if (tempFlag)
							{					
							gPreprocessSelection = itemHit;
								
							if (gPreprocessSelection-1 == kUseLast)
								channelListType = kSelectedItemsListOnly;
								
							else	// gPreprocessSelection-1 != kUseLast)
								channelListType = kSelectItemsList;
								
							}	// end "if (tempFlag)"
		
								// Make certain that the correct label is drawn in the	
								// class pop up box.													
						
						InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
						break;
						
					case 31:		// List Transformation Matrix 
						ChangeDLogCheckBox ((ControlHandle)theHandle);
						break;
							
					}	// end "switch (itemHit)"
					
				if (updateSpecialOptionsFlag)
					{
					FeatureExtractionDialogUpdateSpecialOptions (dialogPtr,
																				gFEAlgorithmCode,
																				specialOptionsFlag,
																				optimizeClassFlag);
					
					updateSpecialOptionsFlag = FALSE;
						
					}	// end "if (updateSpecialOptionsFlag)" 
					
				}	// end "if (itemHit > 2)" 
				
			else if (itemHit > 0) 	// and itemHit <= 2 
				{	
				modalDone = TRUE;
					
				if (itemHit == 1)
					{
					FeatureExtractionDialogOK (gFeatureExtractionSpecsPtr,
															gFEAlgorithmCode,
															gWeightsSelection,
															localClassWeightsPtr,
															gInterClassWeightsSelection,
															localClassPairWeightsListPtr,
															localDefaultClassPairWeight,
															specialOptionsFlag,
															outlierThreshold1,
															outlierThreshold2,
															minThresholdNumber,
															optimizeThreshold,
															(GetDLogControl (dialogPtr, 19)),
															maxPixelsPerClass,
															gChannelSelection,
															localNumberChannels,
															localFeaturesPtr,
															gClassSelection,
															localNumberClasses,
															localClassPtr,
															(GetDLogControl (dialogPtr, 31)),
															gPreprocessSelection);
															
					returnFlag = TRUE;

					}	// end if (itemHit == 1) 
				
				if (itemHit == 2)
					returnFlag = FALSE;
					
				}	// end "else if (itemHit > 0) and itemHit <= 2" 
					
			}	while (!modalDone);
		
		DisposeUserItemUPP (drawInterClassWeightsPopUpPtr);
		DisposeUserItemUPP (drawChannelsPopUp2Ptr);
		DisposeUserItemUPP (drawPreprocessPopUpPtr);
		DisposeUserItemUPP (drawFEAlgorithmPopUpPtr);
			
		ReleaseDialogLocalVectors (localFeaturesPtr,
											NULL,
											localClassPtr,
											NULL,			// localClassAreaPtr,
											localClassWeightsPtr,
											NULL,
											NULL,
											localClassPairWeightsListPtr);
			
		CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
	#endif	// defined multispec_mac
									
	#if defined multispec_win
		CMFeatureExtractionDialog*		dialogPtr = NULL;
		
		TRY
			{ 
			dialogPtr = new CMFeatureExtractionDialog ();
			
			returnFlag = dialogPtr->DoDialog (gFeatureExtractionSpecsPtr); 
		                       
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
		CMFeatureExtractionDialog*		dialogPtr = NULL;
		
		try
			{
			dialogPtr = new CMFeatureExtractionDialog ();
			
			returnFlag = dialogPtr->DoDialog (gFeatureExtractionSpecsPtr); 
		                       
			delete dialogPtr;
			}
			
		catch (int e)
			{
			MemoryMessage (0, kObjectMessage);
			returnFlag = FALSE;
			}
	#endif	// defined multispec_wx
	
	return (returnFlag);

}	// end "FeatureExtractionDialog" 


                   
//------------------------------------------------------------------------------------

void FeatureExtractionDialogAlgorithm (
				DialogPtr							dialogPtr, 
				SInt16								algorithmCode)

{		
	#if defined multispec_mac		
		//SetDLogControl (dialogPtr, 4, (algorithmCode == kDecisionBoundary));
		//SetDLogControl (dialogPtr, 5, (algorithmCode == kDiscriminantAnalysis));
		//SetDLogControl (dialogPtr, 6, (algorithmCode == kPreprocessingOnly));
	#endif	// defined multispec_mac
					
	if (algorithmCode == kNonparametricWeighted)
		{
		SetDLogControlHilite (dialogPtr, IDC_SpecialOptions, 0);
		ShowDialogItem (dialogPtr, IDC_WeightsPrompt); 
		ShowDialogItem (dialogPtr, IDC_WeightCombo);
		ShowDialogItem (dialogPtr, IDC_ClassPairWeightsPrompt); 
		ShowDialogItem (dialogPtr, IDC_ClassPairWeightsCombo);
		
		}	// end "if (algorithmCode == kNonparametricWeighted)" 
					
	else if (algorithmCode == kDecisionBoundary)
		{
		SetDLogControlHilite (dialogPtr, IDC_SpecialOptions, 0);
		ShowDialogItem (dialogPtr, IDC_WeightsPrompt); 
		ShowDialogItem (dialogPtr, IDC_WeightCombo);
		ShowDialogItem (dialogPtr, IDC_ClassPairWeightsPrompt); 
		ShowDialogItem (dialogPtr, IDC_ClassPairWeightsCombo);
		
		}	// end "if (algorithmCode == kDecisionBoundary)" 
		
	else if (algorithmCode == kDiscriminantAnalysis)
		{
		SetDLogControlHilite (dialogPtr, IDC_SpecialOptions, 255); 
		ShowDialogItem (dialogPtr, IDC_WeightPrompt);
		ShowDialogItem (dialogPtr, IDC_WeightsCombo); 
		HideDialogItem (dialogPtr, IDC_ClassPairWeightsPrompt); 
		HideDialogItem (dialogPtr, IDC_ClassPairWeightsCombo);
		
		}	// end "else algorithmCode == kDiscriminantAnalysis"  
		
	else	// algorithmCode == kPreprocessingOnly
		{
		SetDLogControlHilite (dialogPtr, IDC_SpecialOptions, 255); 
		HideDialogItem (dialogPtr, IDC_WeightsPrompt); 
		HideDialogItem (dialogPtr, IDC_WeightCombo);
		ShowDialogItem (dialogPtr, IDC_ClassPairWeightsPrompt); 
		ShowDialogItem (dialogPtr, IDC_ClassPairWeightsCombo);
		
		}	// end "else algorithmCode == kPreprocessingOnly" 

}	// end "FeatureExtractionDialogAlgorithm"	


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void FeatureExtractionDialogInitialize
//
//	Software purpose:	The purpose of this routine is to set the selected 
//							in the feature extraction structure.
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:  	None
//
// Called By:			FeatureExtractionDialog
//
//	Coded By:			Larry L. Biehl			Date: 03/02/1999
//	Revised By:			Larry L. Biehl			Date: 12/28/1999	

void FeatureExtractionDialogInitialize (
				DialogPtr							dialogPtr,
				FeatureExtractionSpecsPtr		featureExtractionSpecsPtr,
				UInt16*								localFeaturesPtr,
				UInt16*								localClassPtr,
				SInt16**								localClassPairWeightsListPtrPtr,
				SInt16*								algorithmCodePtr,
				SInt16*								weightsSelectionPtr,
				SInt16*								interClassWeightsSelectionPtr,
				SInt16*								defaultClassPairWeightPtr,
				Boolean*								specialOptionsFlagPtr,
				double*								outlierThreshold1Ptr,
				double*								outlierThreshold2Ptr,
				UInt32*								minimumThresholdNumberPtr,
				Boolean*								optimizeClassFlagPtr,
				double*								optimizeThresholdPtr,
				UInt32*								maxPixelsPerClassPtr,
				SInt16*								channelSelectionPtr,
				UInt16*								localNumberChannelsPtr,
				SInt16*								channelListTypePtr,
				SInt16*								classSelectionPtr,
				UInt32*								localNumberClassesPtr,
				SInt16*								preprocessSelectionPtr,
				Boolean*								listTransformationFlagPtr)

{
			// Load the dialog local vectors

	LoadDialogLocalVectors (localFeaturesPtr,
										NULL,
										featureExtractionSpecsPtr->featureHandle,
										featureExtractionSpecsPtr->numberChannels,
										localClassPtr,
										featureExtractionSpecsPtr->classHandle,
										featureExtractionSpecsPtr->numberClasses,
										NULL,		// classArea
										NULL,
										0,
										NULL,		// symbol
										NULL,
										0,
										NULL,
										NULL,
										0,
										localClassPairWeightsListPtrPtr);
										
	*algorithmCodePtr = featureExtractionSpecsPtr->algorithmCode;
	
			// Class weights.																		
			//	Make equal weights the default	
	
	*weightsSelectionPtr = UpdateDialogClassWeightsInfo (
								kEqualWeights,
								gProjectInfoPtr->covarianceStatsToUse == kEnhancedStats,
								NULL,
								TRUE);
	
			// Interclass weights.																
			//	Make equal interclass weights the default		
			
	*interClassWeightsSelectionPtr = gProjectInfoPtr->classPairWeightSet;
	*defaultClassPairWeightPtr = gProjectInfoPtr->defaultClassPairWeight;
	
			// Special options flag.
	
	*specialOptionsFlagPtr = featureExtractionSpecsPtr->specialOptionsFlag;
	
			// outlierThreshold1 variable; used for decision boundary.				
		
	*outlierThreshold1Ptr = featureExtractionSpecsPtr->outlierThreshold1;
	
			// outlierThreshold2 variable; used for decision boundary.				
		
	*outlierThreshold2Ptr = featureExtractionSpecsPtr->outlierThreshold2;
	
			// minimumThresholdNumber variable; used for decision boundary.		
		
	*minimumThresholdNumberPtr = featureExtractionSpecsPtr->minimumThresholdNumber;
	
			// Set 'Optimize class flag'.														
			
	*optimizeClassFlagPtr = featureExtractionSpecsPtr->optimizeClassFlag;
	
			// optimizeClassThreshold variable; used for decision boundary.		
		
	*optimizeThresholdPtr = featureExtractionSpecsPtr->optimizeThreshold;
	
			// Set maximum number of pixels desired per class.							
		
	*maxPixelsPerClassPtr = featureExtractionSpecsPtr->maximumPixelsPerClass;
	
			// Hide/Show special options.
	
	FeatureExtractionDialogUpdateSpecialOptions (dialogPtr, 
																	*algorithmCodePtr, 
																	*specialOptionsFlagPtr,
																	*optimizeClassFlagPtr);
	
			// Note that the user does not have a choice of channels if the last 
			// preprocessing information is to be used.						
			
	*channelSelectionPtr = featureExtractionSpecsPtr->channelSet;
	
	*localNumberChannelsPtr = featureExtractionSpecsPtr->numberChannels;
							
	*channelListTypePtr = kSelectItemsList;
	if (gFeatureExtractionSpecsPtr->preprocessCode == kUseLast)						
		*channelListTypePtr = kSelectedItemsListOnly;
	
			// Classes to use.																	
			
	*classSelectionPtr = featureExtractionSpecsPtr->classSet;
	
	*localNumberClassesPtr = featureExtractionSpecsPtr->numberClasses;	
	
			// Preprocessing selection							
			
	*preprocessSelectionPtr = featureExtractionSpecsPtr->preprocessCode + 1;
	
			// Set 'transformation matrix' flag.										
			
	*listTransformationFlagPtr = featureExtractionSpecsPtr->listTransformationFlag;
	
}	// end "FeatureExtractionDialogInitialize"


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void FeatureExtractionDialogOK
//
//	Software purpose:	The purpose of this routine is to set the selected 
//							in the feature extraction structure.
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:  	None
//
// Called By:			FeatureExtractionDialog
//
//	Coded By:			Larry L. Biehl			Date: 03/02/1999
//	Revised By:			Larry L. Biehl			Date: 12/28/1999
	
void FeatureExtractionDialogOK (
				FeatureExtractionSpecsPtr		featureExtractionSpecsPtr,
				SInt16								algorithmCode,
				SInt16								weightsSelection,
				float*								localClassWeightsPtr,
				SInt16								interClassWeightsSelection,
				SInt16*								localClassPairWeightsListPtr,
				SInt16								localDefaultClassPairWeight,
				Boolean								specialOptionsFlag,
				double								outlierThreshold1,
				double								outlierThreshold2,
				UInt32								minThresholdNumber,
				double								optimizeThreshold,
				Boolean								optimizeClassFlag,
				UInt32								maxPixelsPerClass,
				SInt16								channelSelection,
				UInt16								localNumberFeatures,
				UInt16*								localFeaturesPtr,
				SInt16								classSelection,
				UInt32								localNumberClasses,
				UInt16*								localClassPtr,
				Boolean								listTransformationFlag,
				SInt16								preprocessSelection)

{
	featureExtractionSpecsPtr->algorithmCode = algorithmCode;
				
			// Class Weights.															
			
	UpdateProjectClassWeights (
					gProjectInfoPtr->covarianceStatsToUse == kEnhancedStats,
					weightsSelection,
					localClassWeightsPtr);
				
			// Interclass Weights.													

	gProjectInfoPtr->classPairWeightSet = interClassWeightsSelection;
	
			// Special options flag.
	
	featureExtractionSpecsPtr->specialOptionsFlag = specialOptionsFlag;

			// Outlier threshold 1 variable to use.							
			
	featureExtractionSpecsPtr->outlierThreshold1 = (float)outlierThreshold1;

			// Outlier threshold 2 variable to use.							
			
	featureExtractionSpecsPtr->outlierThreshold2 = (float)outlierThreshold2;
		
			// Saved minimum threshold number.									
			
	featureExtractionSpecsPtr->minimumThresholdNumber = (SInt32)minThresholdNumber;

			// Optimize threshold variable to use.								
			
	featureExtractionSpecsPtr->optimizeThreshold = (float)optimizeThreshold;

			// Optimize class flag.													
															
	featureExtractionSpecsPtr->optimizeClassFlag = optimizeClassFlag;
										
			// Save features flag.													
	
	featureExtractionSpecsPtr->maximumPixelsPerClass = maxPixelsPerClass;
														
			// Load some common processor parameters
			// Channels
			// Classes
			
	LoadProcessorVectorsFromDialogLocalVectors (
												channelSelection,
												FALSE,
												gProjectInfoPtr->numberStatisticsChannels,
												localNumberFeatures,
												localFeaturesPtr,
												&featureExtractionSpecsPtr->channelSet,
												&featureExtractionSpecsPtr->numberChannels,
												featureExtractionSpecsPtr->featureHandle,
												&featureExtractionSpecsPtr->numberChannels,
												featureExtractionSpecsPtr->channelsHandle,
												classSelection,
												localNumberClasses,
												localClassPtr,
												&featureExtractionSpecsPtr->classSet,
												&featureExtractionSpecsPtr->numberClasses,
												featureExtractionSpecsPtr->classHandle,
												0,
												NULL,
												NULL,
												NULL,
												interClassWeightsSelection,
												localClassPairWeightsListPtr,
												&gProjectInfoPtr->classPairWeightSet);
	
			// List Transformation Matrix flag.									
					
	featureExtractionSpecsPtr->listTransformationFlag = listTransformationFlag;

			// Use Projection Pursuit flag.								
					
	featureExtractionSpecsPtr->preprocessCode = preprocessSelection - 1;
													
			// Default class pair weight.
			
	gProjectInfoPtr->defaultClassPairWeight = localDefaultClassPairWeight;

}	// end "FeatureExtractionDialogOK"	



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void FeatureExtractionDialogOptimizeClass
//
//	Software purpose:	The purpose of this routine is to set the dialog items dealing
//							with the optimize class options.
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:  	None
//
// Called By:			FeatureExtractionDialog
//
//	Coded By:			Larry L. Biehl			Date: 03/04/1999
//	Revised By:			Larry L. Biehl			Date: 03/04/1999
	
void FeatureExtractionDialogOptimizeClass (
				DialogPtr							dialogPtr,
				Boolean								optimizeClassFlag)

{
	#if defined multispec_mac 
		SetDLogControl (dialogPtr, 19, (optimizeClassFlag));
	#endif	// defined multispec_mac
		
	if (optimizeClassFlag)
		{
		ShowDialogItem (dialogPtr, IDC_PercentAccuracyPrompt);
		ShowDialogItem (dialogPtr, IDC_PercentAccuracy);
		
		}	// end "if (optimizeClassFlag)" 
								
	else	// !optimizeClassFlag 
		{
		HideDialogItem (dialogPtr, IDC_PercentAccuracyPrompt);
		HideDialogItem (dialogPtr, IDC_PercentAccuracy);
		
		}	// end "else !optimizeClassFlag" 

}	// end "FeatureExtractionDialogOptimizeClass"	



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void FeatureExtractionDialogUpdateSpecialOptions
//
//	Software purpose:	The purpose of this routine is to set the dialog items dealing
//							with the special options.
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:  	None
//
// Called By:			FeatureExtractionDialog
//
//	Coded By:			Larry L. Biehl			Date: 03/03/1999
//	Revised By:			Larry L. Biehl			Date: 09/28/1999
	
void FeatureExtractionDialogUpdateSpecialOptions (
				DialogPtr							dialogPtr,
				SInt16								algorithmCode,
				Boolean								specialOptionsFlag,
				Boolean								optimizeClassFlag)

{
	if (algorithmCode != kDecisionBoundary)
		SetDLogControlHilite (dialogPtr, IDC_SpecialOptions, 255);
		
	else	// algorithmCode == kDecisionBoundary
		SetDLogControlHilite (dialogPtr, IDC_SpecialOptions, 0);
	
	if (algorithmCode != kDecisionBoundary || !specialOptionsFlag)
		{                         
		SetDLogControl (dialogPtr, IDC_SpecialOptions, 0);
			
		#if defined multispec_mac 
			//SetDLogControl (dialogPtr, 12, 0);
			HideDialogItems (dialogPtr, 13, 23);
		#endif	// defined multispec_mac
									
		#if defined multispec_win  || defined multispec_wx
			HideDialogItem (dialogPtr, IDC_WithinClassThresholdPrompt); 
			HideDialogItem (dialogPtr, IDC_WithinClassThreshold); 
			HideDialogItem (dialogPtr, IDC_InterclassThresholdPrompt); 
			HideDialogItem (dialogPtr, IDC_InterclassThreshold);  
			HideDialogItem (dialogPtr, IDC_MinThresholdNumberPrompt);      
			HideDialogItem (dialogPtr, IDC_MinThresholdNumber);
			HideDialogItem (dialogPtr, IDC_OptimizeClasses);   
			HideDialogItem (dialogPtr, IDC_PercentAccuracyPrompt);
			HideDialogItem (dialogPtr, IDC_PercentAccuracy);      
			HideDialogItem (dialogPtr, IDC_MaxPixelsPrompt);      
			HideDialogItem (dialogPtr, IDC_MaxPixels); 
		#endif	// defined multispec_win
		
		}	// end "if (algorithmCode != kDecisionBoundary || !specialOptionsFlag)" 
		
	else	// algorithmCode == kDecisionBoundary && specialOptionsFlag
		{                
		SetDLogControl (dialogPtr, IDC_SpecialOptions, (specialOptionsFlag));
		
		#if defined multispec_mac 
//			SetDLogControl (dialogPtr, 12, 1);
			ShowDialogItems (dialogPtr, 13, 19);
			
			FeatureExtractionDialogOptimizeClass (dialogPtr, optimizeClassFlag);
			 
			ShowDialogItems (dialogPtr, 22, 23);
		#endif	// defined multispec_mac
									
		#if defined multispec_win   || defined multispec_wx
			ShowDialogItem (dialogPtr, IDC_WithinClassThresholdPrompt); 
			ShowDialogItem (dialogPtr, IDC_WithinClassThreshold); 
			ShowDialogItem (dialogPtr, IDC_InterclassThresholdPrompt); 
			ShowDialogItem (dialogPtr, IDC_InterclassThreshold);  
			ShowDialogItem (dialogPtr, IDC_MinThresholdNumberPrompt);      
			ShowDialogItem (dialogPtr, IDC_MinThresholdNumber);
			ShowDialogItem (dialogPtr, IDC_OptimizeClasses); 
			
			FeatureExtractionDialogOptimizeClass (dialogPtr, optimizeClassFlag);
				
			ShowDialogItem (dialogPtr, IDC_MaxPixelsPrompt);      
			ShowDialogItem (dialogPtr, IDC_MaxPixels); 
		#endif	// defined multispec_win || defined multispec_wx
			
		SelectDialogItemText (dialogPtr, IDC_WithinClassThreshold, 0, (SInt16)SInt16_MAX);
		
		}	// end "else algorithmCode == kDecisionBoundary && ..." 

}	// end "FeatureExtractionDialogUpdateSpecialOptions"	



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetTranformationFeatureMeans
//
//	Software purpose:	The purpose of this routine is to get the
//							feature means for the input transformation
//							and classes.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	True if operation should continue
//							False if operation should not continue
//
// Called By:			FeatureExtractionControl
//
//	Coded By:			Larry L. Biehl			Date: 07/15/1996
//	Revised By:			Larry L. Biehl			Date: 08/26/2010
	
void GetTranformationFeatureMeans (
					ClassInfoPtr					classInfoPtr,
					UInt16							numberClasses,
					UInt16							numberChannels,
					UInt16							numberFeatures,
					HDoublePtr						eigenVectorPtr,
					HDoublePtr						eigenFeatureMeanPtr)

{
	SInt64								totalNumberPixels;

	double								weightedMean;
	
	HDoublePtr							savedEigenFeatureMeanPtr,
											savedEigenVectorPtr;
	
	UInt32								channel,
											index;
	
	
	savedEigenFeatureMeanPtr = eigenFeatureMeanPtr;
	savedEigenVectorPtr = eigenVectorPtr;
	
			// Initialize eigenFeatureMeanPtr
			
	ZeroMatrix (eigenFeatureMeanPtr, numberFeatures, 1, TRUE);
	
			// Get the total number of samples in all classes.

	totalNumberPixels = 0;
	for (index=0; index<numberClasses; index++)
		totalNumberPixels += classInfoPtr[index].no_samples;
	
			// Now loop through for each channel and add the portion 
			// contributed by each channel to each feature.
			
	for (channel=0; channel<numberChannels; channel++)
		{
				// Now get the weighted mean for the channel.	
		
		weightedMean = 0;		
		for (index=0; index<numberClasses; index++)
			weightedMean += classInfoPtr[index].meanPtr[channel] * 
														classInfoPtr[index].no_samples;
														
		weightedMean /= totalNumberPixels;
		
		eigenFeatureMeanPtr = savedEigenFeatureMeanPtr;
		eigenVectorPtr = savedEigenVectorPtr + channel;
		
		for (index=0; index<numberFeatures; index++)
			{
			*eigenFeatureMeanPtr += weightedMean * *eigenVectorPtr;
			
			eigenVectorPtr += numberChannels;
			eigenFeatureMeanPtr++;
			
			}	// end "for (index=0; index<..." 
			
		}	// end "for (channel=0; channel<..." 
		
}	// end "GetTranformationFeatureMeans" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ListFeatureExtractionOptions
//
//	Software purpose:	The purpose of this routine is to list the options
//							selected for feature extraction.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	True if operation should continue
//							False if operation should not continue
//
// Called By:			FeatureExtractionControl
//
//	Coded By:			Larry L. Biehl			Date: 10/22/1992
//	Revised By:			Larry L. Biehl			Date: 07/16/1996
	
Boolean ListFeatureExtractionOptions (
				CMFileStream*						resultsFileStreamPtr)

{				
	Boolean								continueFlag = TRUE;
	

	if (gFeatureExtractionSpecsPtr->algorithmCode == kPreprocessingOnly)
		continueFlag = ListSpecifiedStringNumber (
								kFeatureExtractStrID, 
								IDS_FeatureExtract39, 
								(unsigned char*)gTextString,
								resultsFileStreamPtr, 
								gOutputForce1Code,
								continueFlag);
				
	else	// gFeatureExtractionSpecsPtr->algorithmCode != kPreprocessingOnly
		continueFlag = ListSpecifiedStringNumber (
								kFeatureExtractStrID, 
								IDS_FeatureExtract40, 
								(unsigned char*)gTextString,
								resultsFileStreamPtr, 
								gOutputForce1Code,
								continueFlag);
								
			// 	List the feature extraction algorithm.							
	
	if (gFeatureExtractionSpecsPtr->algorithmCode == kDiscriminantAnalysis)
		continueFlag = ListSpecifiedStringNumber (
									kFeatureExtractStrID, 
									IDS_FeatureExtract4, 
									(unsigned char*)gTextString, 
									resultsFileStreamPtr, 
									gOutputForce1Code,
									continueFlag);
					
	else if (gFeatureExtractionSpecsPtr->algorithmCode == kDecisionBoundary) 
		{		
		continueFlag = ListSpecifiedStringNumber (
									kFeatureExtractStrID, 
									IDS_FeatureExtract5, 
									(unsigned char*)gTextString, 
									resultsFileStreamPtr, 
									gOutputForce1Code,
									continueFlag);
												
		continueFlag = ListSpecifiedStringNumber (
						kFeatureExtractStrID, 
						IDS_FeatureExtract6, 
						resultsFileStreamPtr, 
						gOutputForce1Code,
						(SInt32)gFeatureExtractionSpecsPtr->minimumThresholdNumber,
						continueFlag);
												
		continueFlag = ListSpecifiedStringNumber (
						kFeatureExtractStrID, 
						IDS_FeatureExtract7, 
						resultsFileStreamPtr, 
						gOutputForce1Code,
						gFeatureExtractionSpecsPtr->outlierThreshold1,
						continueFlag);
												
		continueFlag = ListSpecifiedStringNumber (
						kFeatureExtractStrID, 
						IDS_FeatureExtract8, 
						resultsFileStreamPtr, 
						gOutputForce1Code,
						gFeatureExtractionSpecsPtr->outlierThreshold2,
						continueFlag);
												
		continueFlag = ListSpecifiedStringNumber (
						kFeatureExtractStrID, 
						IDS_FeatureExtract9, 
						resultsFileStreamPtr, 
						gOutputForce1Code,
						gFeatureExtractionSpecsPtr->optimizeThreshold,
						continueFlag);
												
		continueFlag = ListSpecifiedStringNumber (
						kFeatureExtractStrID, 
						IDS_FeatureExtract10, 
						resultsFileStreamPtr, 
						gOutputForce1Code,
						(SInt32)gFeatureExtractionSpecsPtr->maximumPixelsPerClass,
						continueFlag);
			
		}	// end "else if (...->algorithmCode == kDecisionBoundary)" 
					
	else if (gFeatureExtractionSpecsPtr->algorithmCode == kNonparametricWeighted) 
		{		
		continueFlag = ListSpecifiedStringNumber (
									kFeatureExtractStrID, 
									IDS_FeatureExtract73, 
									(unsigned char*)gTextString, 
									resultsFileStreamPtr, 
									gOutputForce1Code,
									continueFlag);
			
		}	// end "else if (...->algorithmCode == kDecisionBoundary)" 
		
	continueFlag = OutputString (resultsFileStreamPtr, 
											(char*)gEndOfLine,
											0, 
											gOutputForce1Code,
											continueFlag); 	
													
	return (continueFlag);

}	// End "ListFeatureExtractionOptions"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean LoadFeatureExtractionSpecs
//
//	Software purpose:	The purpose of this routine is to load the Feature 
//							Extraction Specification structure with the default set of
//							parameters.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			FeatureExtractionControl
//
//	Coded By:			Larry L. Biehl			Date: 10/22/1992
//	Revised By:			Larry L. Biehl			Date: 03/03/1999

Boolean LoadFeatureExtractionSpecs (void)			

{
	SInt16								*channels,
											*classPairWeightsPtr,
											*classPtr,
											*featurePtr;
											
	UInt32								bytesNeeded;
												
	SInt16								fileInfoVersion,
											numberPairReturnCode;
	
	Boolean								changedFlag,
											continueFlag;
	
	
			// Check important input variable.												
			
	if (gProjectInfoPtr == NULL)
																					return (FALSE);
	
			// Assume routine will work okay.  If there isn't enough memory		
			// then the flag will be changed.												
			
	continueFlag = TRUE;
	
			// Initialize fileInfoVersion
				
	fileInfoVersion = 0;
	if (gImageWindowInfoPtr != NULL)
		fileInfoVersion = gImageWindowInfoPtr->fileInfoVersion;
		
			// If a feature extraction structure already exists, then check if 	
			// it is for the current window.  If not release the old structure 	
			// and initialize a new one.														
			
	if (gProjectInfoPtr->featureExtractionSpecsH != NULL)
		{
				// Lock the handle to the feature extraction specifications and 	
				// get a pointer to the structure.											
		
		gFeatureExtractionSpecsPtr = 
				(FeatureExtractionSpecsPtr)GetHandlePointer (
							gProjectInfoPtr->featureExtractionSpecsH, kLock, kMoveHi);	
		
				// Check if the the current feature extraction structure
				// settings make sense for the current project file.
				
		if (gImageWindowInfoPtr == NULL)
			fileInfoVersion = gFeatureExtractionSpecsPtr->fileInfoVersion;
									
		if (gFeatureExtractionSpecsPtr->windowInfoHandle != 
													gProjectInfoPtr->windowInfoHandle || 
				gFeatureExtractionSpecsPtr->fileInfoVersion != fileInfoVersion)
			{
			ReleaseFeatureExtractionSpecsMemory (
										&gProjectInfoPtr->featureExtractionSpecsH);
			gFeatureExtractionSpecsPtr = NULL;
			
			}	// end "if (gImageWindowInfoPtr != NULL && ..." 
				
		}	// end "if (...->featureExtractionSpecsH != NULL)" 
	
	if (gProjectInfoPtr->featureExtractionSpecsH == NULL)
		{
				// Set up handle to feature extraction information						
			
		gProjectInfoPtr->featureExtractionSpecsH = 
											MNewHandle (sizeof (FeatureExtractionSpecs));
		if (gProjectInfoPtr->featureExtractionSpecsH)
			{
			gFeatureExtractionSpecsPtr = 
				(FeatureExtractionSpecsPtr)GetHandlePointer (
							gProjectInfoPtr->featureExtractionSpecsH, kLock, kMoveHi);	
			
					// Initialize the feature extraction specification structure.
			
			ProjectionPursuitSpecsPtr projectionPursuitSpecsPtr = 
									&gFeatureExtractionSpecsPtr->projectionPursuitParams;
											
			projectionPursuitSpecsPtr->tdThreshold = 0.0250 * 100;
			projectionPursuitSpecsPtr->buThreshold = 0.0050 * 100;
			projectionPursuitSpecsPtr->optimizationThreshold = 0.01 * 100;
			projectionPursuitSpecsPtr->bandGroupingHandle = NULL;
			projectionPursuitSpecsPtr->transformValueHandle = NULL;
			projectionPursuitSpecsPtr->transformVectorHandle = NULL;
			projectionPursuitSpecsPtr->minimumNumberSamples = 0;
			projectionPursuitSpecsPtr->algorithm = kProjectionPursuit;
			projectionPursuitSpecsPtr->firstStageMethod = kTopDown;	
			projectionPursuitSpecsPtr->initialGroupingCode = kUniformGrouping;
			projectionPursuitSpecsPtr->bothOddChoicesNumberFeatures = 0;
			projectionPursuitSpecsPtr->finalNumberFeatures = 0;
			projectionPursuitSpecsPtr->initialNumberFeatures = 1;
			projectionPursuitSpecsPtr->maximumNumberFeatures = 
												gProjectInfoPtr->numberStatisticsChannels;
			projectionPursuitSpecsPtr->numberFeatureChannels = 0;
			projectionPursuitSpecsPtr->numericalOptimizationFlag = FALSE;
			projectionPursuitSpecsPtr->savePPTransformationFlag = FALSE;
			projectionPursuitSpecsPtr->startWithCurrentBandGroupFlag = FALSE;
			
					// Now get the last band grouping vector and final number of
					// features if they exist and match info for the current
					// project.
			
			SInt16		errCode,
							transformationSource;
							
			UInt16		numberFeatures;
								
			EigenvectorInfoExists (kProject, &transformationSource, &numberFeatures);
			if (transformationSource == 2 && 
					numberFeatures > 0 &&
					gTransformationMatrix.preProcBandGroupingHandle != NULL)
				{
				projectionPursuitSpecsPtr->bandGroupingHandle =
											gTransformationMatrix.preProcBandGroupingHandle;
				errCode = HandToHand (
											&projectionPursuitSpecsPtr->bandGroupingHandle);
				
				if (errCode == noErr)
					projectionPursuitSpecsPtr->finalNumberFeatures = numberFeatures;
					
				else	// errCode != noErr
					{
					projectionPursuitSpecsPtr->bandGroupingHandle = NULL;
					continueFlag = FALSE;
					
					}	// end "else errCode != noErr"
				
				}	// end "if (transformationSource == 2 && ..."
				
			gFeatureExtractionSpecsPtr->channelsHandle = NULL;
			gFeatureExtractionSpecsPtr->classHandle = NULL;
			gFeatureExtractionSpecsPtr->eigenValueHandle = NULL;
			gFeatureExtractionSpecsPtr->eigenVectorHandle = NULL;
			gFeatureExtractionSpecsPtr->featureHandle = NULL;
			gFeatureExtractionSpecsPtr->windowInfoHandle = 
														gProjectInfoPtr->windowInfoHandle;
	
			gFeatureExtractionSpecsPtr->specialOptionsFlag = FALSE;
			gFeatureExtractionSpecsPtr->optimizeThreshold = (float)95.0;
			gFeatureExtractionSpecsPtr->outlierThreshold1 = (float)0.95;
			gFeatureExtractionSpecsPtr->outlierThreshold2 = (float)0.95;
			gFeatureExtractionSpecsPtr->maximumPixelsPerClass = 1000;
			gFeatureExtractionSpecsPtr->minimumThresholdNumber = 5;
			gFeatureExtractionSpecsPtr->algorithmCode = kDecisionBoundary;
			gFeatureExtractionSpecsPtr->channelSet = kAllMenuItem;
			gFeatureExtractionSpecsPtr->classSet = kAllMenuItem;
			gFeatureExtractionSpecsPtr->fileInfoVersion = fileInfoVersion;
			gFeatureExtractionSpecsPtr->outputStorageType = kClassifyFileCode;
			gFeatureExtractionSpecsPtr->preprocessCode = kNoPreprocess;
			gFeatureExtractionSpecsPtr->numberChannels = 
												gProjectInfoPtr->numberStatisticsChannels;
			gFeatureExtractionSpecsPtr->numberClasses =
												gProjectInfoPtr->numberStatisticsClasses;
			gFeatureExtractionSpecsPtr->firstTime = TRUE;
			gFeatureExtractionSpecsPtr->listTransformationFlag = FALSE;
			gFeatureExtractionSpecsPtr->optimizeClassFlag = TRUE;
			if (gProjectInfoPtr->numberStatisticsClasses == 2)
				gFeatureExtractionSpecsPtr->optimizeClassFlag = FALSE;
			
			}	// end "if (gProjectInfoPtr->featureExtractionSpecsH)" 
		
		else	// else !gProjectInfoPtr->featureExtractionSpecsH 
			continueFlag = FALSE;
			
		}	// end "if (....featureExtractionSpecsH == NULL)" 
					
			// Check memory for feature extraction classes vector.					
		
	if (continueFlag)
		{	
		bytesNeeded = (UInt32)gProjectInfoPtr->numberStatisticsClasses * sizeof (SInt16);
		classPtr = (SInt16*)CheckHandleSize (
										&gFeatureExtractionSpecsPtr->classHandle,
										&continueFlag,
										&changedFlag, 
										bytesNeeded);
		if (changedFlag)
			gFeatureExtractionSpecsPtr->classSet = kAllMenuItem;
			
		}	// end "if (continueFlag)" 
			
			// If memory is not full, check memory for feature extraction			
			// feature vector pointer.															

	if (continueFlag)
		{
		bytesNeeded = (UInt32)gProjectInfoPtr->numberStatisticsChannels * sizeof (SInt16);
		featurePtr = (SInt16*)CheckHandleSize (
										&gFeatureExtractionSpecsPtr->featureHandle, 
										&continueFlag,
										&changedFlag, 
										bytesNeeded);
		if (changedFlag)
			gFeatureExtractionSpecsPtr->channelSet = kAllMenuItem;
			
		}	// end "if (continueFlag)" 
			
			// If memory is not full, check memory for feature extraction			
			// channels vector.																	

	if (continueFlag)
		{
		channels = (SInt16*)CheckHandleSize (
										&gFeatureExtractionSpecsPtr->channelsHandle,
										&continueFlag, 
										&changedFlag, 
										bytesNeeded);
			
		}	// end "if (continueFlag)" 
			
			// If memory is not full, set up memory for class pair weights			
			// vector.																				

	if (continueFlag)
		{
		gProjectInfoPtr->numberProjectClassPairs = GetNumberOfCombinations (
											gProjectInfoPtr->numberStatisticsClasses, 
											2, 
											1,
											FALSE,
											&numberPairReturnCode);
											
		bytesNeeded = gProjectInfoPtr->numberProjectClassPairs * sizeof (SInt16);
		classPairWeightsPtr = (SInt16*)CheckHandleSize (
													&gProjectInfoPtr->classPairWeightsHandle,
													&continueFlag, 
													&changedFlag, 
													bytesNeeded);
		
		}	// end "if (continueFlag)" 
	
			//	Make sure that classPairWeightsListHandle is locked if it exists.
			 	
	if (continueFlag)
		GetHandlePointer (gProjectInfoPtr->classPairWeightsListHandle, 
										kLock, 
										kMoveHi);
		
	if (continueFlag)
		{
				// Make certain that feature and class vectors are						
				// filled in properly.															
		
		if (gFeatureExtractionSpecsPtr->channelSet == kAllMenuItem)		
			LoadFeatureVector (kNoTransformation,
							(SInt16*)&gFeatureExtractionSpecsPtr->numberChannels,
							featurePtr,
							gProjectInfoPtr->numberStatisticsChannels);
		
		if (gFeatureExtractionSpecsPtr->classSet == kAllMenuItem)								
			LoadClassVector (&gFeatureExtractionSpecsPtr->numberClasses,
									classPtr);
			
		}	// end "if (continueFlag)" 
	
			// If unable to set up information for feature extraction				
			// specifications, release memory allocated to it (if any).				
			
	else	// !continueFlag 
		{
		ReleaseFeatureExtractionSpecsMemory (
										&gProjectInfoPtr->featureExtractionSpecsH);
		gFeatureExtractionSpecsPtr = NULL;
		
		}	// end "else !continueFlag" 
		
	return (continueFlag);

}	// end "LoadFeatureExtractionSpecs" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ListPPOptions
//
//	Software purpose:	The purpose of this routine is to list the options
//							selected for projection pursuit.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	True if operation should continue
//							False if operation should not continue
//
// Called By:			FeatureExtractionControl
//
//	Coded By:			Larry L. Biehl			Date: 05/14/1996
//	Revised By:			Larry L. Biehl			Date: 05/20/1996
	
Boolean ListPPOptions (
				CMFileStream*						resultsFileStreamPtr)

{				
	ProjectionPursuitSpecsPtr 		projectionPursuitSpecsPtr;
	
	Boolean								continueFlag = TRUE;
	
						
			// Initialize local variables.
			
	projectionPursuitSpecsPtr = &gFeatureExtractionSpecsPtr->projectionPursuitParams;
								
	if (gFeatureExtractionSpecsPtr->preprocessCode == kNoPreprocess)
		continueFlag = ListSpecifiedStringNumber (
								kFeatureExtractStrID, 
								IDS_FeatureExtract41, 	//" No Feature Extraction Preprocessing will be done.%s"
								(unsigned char*)gTextString,
								resultsFileStreamPtr, 
								gOutputForce1Code,
								continueFlag);
				
	else if (gFeatureExtractionSpecsPtr->preprocessCode == kUseLast)
		continueFlag = ListSpecifiedStringNumber (
								kFeatureExtractStrID, 
								IDS_FeatureExtract42, 
								(unsigned char*)gTextString,
								resultsFileStreamPtr, 
								gOutputForce1Code,
								continueFlag);
				
	else	// gFeatureExtractionSpecsPtr->preprocessCode == kComputeNew)
		continueFlag = ListSpecifiedStringNumber (
								kFeatureExtractStrID, 
								IDS_FeatureExtract43, 
								(unsigned char*)gTextString,
								resultsFileStreamPtr, 
								gOutputForce1Code,
								continueFlag);
								
	if (gFeatureExtractionSpecsPtr->preprocessCode != kComputeNew)
																		return (continueFlag);
											
			// List the projection pursuit algorithm.
	
	if (projectionPursuitSpecsPtr->algorithm == kProjectionPursuit)
		continueFlag = ListSpecifiedStringNumber (
								kFeatureExtractStrID, 
								IDS_FeatureExtract44, 
								(unsigned char*)gTextString,
								resultsFileStreamPtr, 
								gOutputForce1Code,
								continueFlag);
					
	else	// projectionPursuitSpecsPtr->algorithm == kPPFeatureSelection
		continueFlag = ListSpecifiedStringNumber (
								kFeatureExtractStrID, 
								IDS_FeatureExtract45, 
								(unsigned char*)gTextString,
								resultsFileStreamPtr, 
								gOutputForce1Code,
								continueFlag);
								
			// First Stage Method.				
	
	if (projectionPursuitSpecsPtr->firstStageMethod == kUniformGrouping)
		continueFlag = ListSpecifiedStringNumber (
								kFeatureExtractStrID, 
								IDS_FeatureExtract46, 
								(unsigned char*)gTextString,
								resultsFileStreamPtr, 
								gOutputForce1Code,
								continueFlag);	
			
	else if (projectionPursuitSpecsPtr->firstStageMethod == kTopDown)
		continueFlag = ListSpecifiedStringNumber (
								kFeatureExtractStrID, 
								IDS_FeatureExtract47, 
								(unsigned char*)gTextString,
								resultsFileStreamPtr, 
								gOutputForce1Code,
								continueFlag);
					
	else	// projectionPursuitSpecsPtr->firstStageMethod == kTopDownBottomUp
		continueFlag = ListSpecifiedStringNumber (
								kFeatureExtractStrID, 
								IDS_FeatureExtract48, 
								(unsigned char*)gTextString,
								resultsFileStreamPtr, 
								gOutputForce1Code,
								continueFlag);
								
	if (projectionPursuitSpecsPtr->firstStageMethod != kUniformGrouping)
		{
				// List Thresholds.	
				
		continueFlag = ListSpecifiedStringNumber (
								kFeatureExtractStrID, 
								IDS_FeatureExtract49,
								resultsFileStreamPtr, 
								gOutputForce1Code,
								projectionPursuitSpecsPtr->tdThreshold,
								continueFlag);
					
		if (projectionPursuitSpecsPtr->firstStageMethod == kTopDownBottomUp)
			{	
			continueFlag = ListSpecifiedStringNumber (
									kFeatureExtractStrID, 
									IDS_FeatureExtract50,
									resultsFileStreamPtr, 
									gOutputForce1Code,
									projectionPursuitSpecsPtr->buThreshold,
									continueFlag);
			
			}	// end "if (...->firstStageMethod == kTopDownBottomUp)"
			
		}	// end "if (...->firstStageMethod != kUniformGrouping)"
		
			// Initial band grouping.
	
	SInt32 initialNumber = projectionPursuitSpecsPtr->initialNumberFeatures;		
	if (projectionPursuitSpecsPtr->initialGroupingCode == kLastGrouping)
		initialNumber = projectionPursuitSpecsPtr->finalNumberFeatures;
		
	else if (projectionPursuitSpecsPtr->firstStageMethod == kUniformGrouping)
		initialNumber = projectionPursuitSpecsPtr->maximumNumberFeatures;
												
	continueFlag = ListSpecifiedStringNumber (
							kFeatureExtractStrID, 
							IDS_FeatureExtract51,		// "  Initial number of features: %ld%s"
							resultsFileStreamPtr, 
							gOutputForce1Code,
							initialNumber,
							continueFlag);
												
	if (projectionPursuitSpecsPtr->initialGroupingCode == kLastGrouping)
		{				
		continueFlag = ListSpecifiedStringNumber (
								kFeatureExtractStrID, 
								IDS_FeatureExtract52, 		// "   Begin with last feature grouping.%s"
								(unsigned char*)gTextString,
								resultsFileStreamPtr, 
								gOutputForce1Code,
								continueFlag);
												
		}	// end "if (...->initialGroupingCode == kLastGrouping)"
		
			// List band group splitting method.
								
	if (projectionPursuitSpecsPtr->firstStageMethod != kUniformGrouping)
		{					
		continueFlag = ListSpecifiedStringNumber (
								kFeatureExtractStrID, 
								IDS_FeatureExtract53,				//"  Split feature groups with an odd number of bands:%s"
								(unsigned char*)gTextString,
								resultsFileStreamPtr, 
								gOutputForce1Code,
								continueFlag);
				
		if (projectionPursuitSpecsPtr->bothOddChoicesNumberFeatures == 0)
			continueFlag = ListSpecifiedStringNumber (
								kFeatureExtractStrID, 
								IDS_FeatureExtract54, 							// "   Use random choice.%s"
								(unsigned char*)gTextString,
								resultsFileStreamPtr, 
								gOutputForce1Code,
								continueFlag);
					
		else	// projectionPursuitSpecsPtr->bothOddChoicesNumberFeatures != 0
			continueFlag = ListSpecifiedStringNumber (
						kFeatureExtractStrID, 
						IDS_FeatureExtract55,
						resultsFileStreamPtr, 
						gOutputForce1Code,
						(SInt32)projectionPursuitSpecsPtr->bothOddChoicesNumberFeatures,
						continueFlag);
		
		}	// end "if (...->firstStageMethod != kUniformGrouping)"
		
			// Maximum number of output features.

	continueFlag = ListSpecifiedStringNumber (
						kFeatureExtractStrID, 
						IDS_FeatureExtract56,				// "  Maximum number of output features is: %ld%s"
						resultsFileStreamPtr, 
						gOutputForce1Code,
						(SInt32)projectionPursuitSpecsPtr->maximumNumberFeatures,
						continueFlag);
		
			// Numerical optimization flag
			
	if (projectionPursuitSpecsPtr->numericalOptimizationFlag)
		{
		continueFlag = ListSpecifiedStringNumber (
							kFeatureExtractStrID, 
							IDS_FeatureExtract57, 				// "  Numerical Optimization will be used.%s"
							(unsigned char*)gTextString,
							resultsFileStreamPtr, 
							gOutputForce1Code,
							continueFlag);
											
		continueFlag = ListSpecifiedStringNumber (
							kFeatureExtractStrID, 
							IDS_FeatureExtract58,			// "   Numerical optimization threshold is: %f%%%s"
							resultsFileStreamPtr, 
							gOutputForce1Code,
							projectionPursuitSpecsPtr->optimizationThreshold,
							continueFlag);				
	
		}	// end "if (...->numericalOptimizationFlag)"
		
	continueFlag = OutputString (resultsFileStreamPtr, 
										(char*)gEndOfLine, 
										0, 
										gOutputForce1Code,
										continueFlag); 	
													
	return (continueFlag);

}	// End "ListPPOptions"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ProjectionPursuitDialog
//
//	Software purpose:	The purpose of this routine is to obtain the
//							user specifications for the projection pursuit
//							preprocessor.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	True if user selects OK in dialog box
//							False if user selects Cancel in dialog box
//
// Called By:			FeatureExtractionControl
//
//	Coded By:			Larry L. Biehl			Date: 05/13/1996
//	Revised By:			Larry L. Biehl			Date: 03/29/2007
	
Boolean ProjectionPursuitDialog (void)

{
	Boolean								returnFlag;
	
	ProjectionPursuitSpecsPtr 		projectionPursuitSpecsPtr;
	
	#if defined multispec_mac
		DialogPtr							dialogPtr;
		
		Handle								okHandle,
												theHandle;
		
		Rect									theBox;
		
		double								variable,
												bottomUpThreshold,
												optimizeThreshold,
												topDownThreshold;
		
		SInt32								maximumNumberFeatures,
												bothOddChoicesNumberFeatures,
												theNum;
		
		SInt16								algorithmCode,
												finalNumberFeatures,
												firstStageMethod,
												initialGroupingCode,
												initialNumberFeatures,
												itemHit,
												theType;
											
		Boolean								modalDone,
												numericalOptimizationFlag;
		
																					
				// Get the modal dialog for the projection pursuit specifications.		
						
		dialogPtr = LoadRequestedDialog (kProjectionPursuitDialogID, kCopyScrap, 1, 2);
			
		if (dialogPtr == NULL)											
																					return (FALSE);
				
		projectionPursuitSpecsPtr = 
							&gFeatureExtractionSpecsPtr->projectionPursuitParams;
								
		ProjectionPursuitDialogInitialize (dialogPtr,
														projectionPursuitSpecsPtr,
														&numericalOptimizationFlag,
														&algorithmCode,
														&initialGroupingCode,
														&initialNumberFeatures,
														&finalNumberFeatures,
														&firstStageMethod,
														&optimizeThreshold,
														&topDownThreshold,
														&bottomUpThreshold,
														&maximumNumberFeatures,
														&bothOddChoicesNumberFeatures);
														
				// The following items are not used any more.  They could be removed
				// from the dialog box but that would change all of the item numbers
				// around. Will wait to do that when needed.
				
		HideDialogItem (dialogPtr, 10);	
		HideDialogItem (dialogPtr, 25);	
		HideDialogItem (dialogPtr, 28);

				// Save handle for the OK button for use later.
				
		GetDialogItem (dialogPtr, 1,  &theType, &okHandle, &theBox);
				
				// Initial grouping options.
		
		LoadDItemValue (dialogPtr, 14, initialNumberFeatures);	
		LoadDItemValue (dialogPtr, 16, finalNumberFeatures);
		
				// Thresholds.				
			
		LoadDItemRealValue (dialogPtr, 9, optimizeThreshold, 2);	
		LoadDItemRealValue (dialogPtr, 24, topDownThreshold, 2);	
		LoadDItemRealValue (dialogPtr, 27, bottomUpThreshold, 2);
		
				// Maximum number features.				
			
		LoadDItemValue (dialogPtr, 18, maximumNumberFeatures); 
		
				// Both odd choices number of features.
				
		LoadDItemValue (dialogPtr, 30, bothOddChoicesNumberFeatures);
		
				// Set the default edit text item.
			
		SelectDialogItemText (dialogPtr, 18, 0, SInt16_MAX);
			
				// Center the dialog and then show it.											
				
		ShowDialogWindow (
							dialogPtr, kProjectionPursuitDialogID, kSetUpDFilterTable);
			
		gDialogItemDescriptorPtr[9] = kDItemReal;
		gDialogItemDescriptorPtr[24] = kDItemReal;
		gDialogItemDescriptorPtr[27] = kDItemReal;
			
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
					case 5:	// First stage algorithm
					case 6:	// PP Feature selection algorithm
						algorithmCode = (itemHit-4);
						ProjectionPursuitDialogAlgorithm (
									dialogPtr, algorithmCode, numericalOptimizationFlag);
						ProjectionPursuitDialogFSOptions (
									dialogPtr, algorithmCode, firstStageMethod);
						break;
					
					case 7:	// Numerical optimization flag
						numericalOptimizationFlag = !numericalOptimizationFlag;
						ProjectionPursuitDialogOptimizeFlag (
														dialogPtr, numericalOptimizationFlag);
						break;
						
					case 9:	// numerical optimization threshold
						variable = GetDItemRealValue (dialogPtr, itemHit);
						if (variable > 100.0)
							RealNumberErrorAlert (optimizeThreshold, dialogPtr, itemHit, 2);
						if (variable >= 0 && variable <= 100.0)
							optimizeThreshold = variable;
						break;
						
					case 12:		// uniform grouping
					case 13:		// previous final grouping
						initialGroupingCode = (itemHit-11);
						ProjectionPursuitDialogInitialGrouping (
																	dialogPtr, initialGroupingCode);
						break;			
						
					case 14:		// initial number of features
						if (theNum > gProjectInfoPtr->numberStatisticsChannels)
							NumberErrorAlert (initialNumberFeatures, 
													dialogPtr, 
													itemHit);
						if (theNum > 0 &&
											theNum <= gProjectInfoPtr->numberStatisticsChannels)
							initialNumberFeatures = theNum;
						break;
						
					case 18:		// maximum number features
						if (theNum > gProjectInfoPtr->numberStatisticsChannels)
							NumberErrorAlert (maximumNumberFeatures, dialogPtr, itemHit);
						if (theNum > 0 &&
											theNum <= gProjectInfoPtr->numberStatisticsChannels)
							maximumNumberFeatures = theNum;
						break;
						
					case 20:		// Uniform grouping
					case 21:		// Top-down First Stage Method.
					case 22:		// Top-down/Bottom-up First Stage Method.
						firstStageMethod = (itemHit-19);
						ProjectionPursuitDialogFSMethod (dialogPtr, firstStageMethod);
						break;
						
					case 24:		// top-down threshold
						variable = GetDItemRealValue (dialogPtr, itemHit);
						if (variable > 100.0)
							RealNumberErrorAlert (topDownThreshold, dialogPtr, itemHit, 2);
						if (variable >= 0 && variable <= 100.0)
							topDownThreshold = variable;
						break;
						
					case 27:		// bottom-up threshold
						variable = GetDItemRealValue (dialogPtr, itemHit);
						if (variable > 100.0)
							RealNumberErrorAlert (bottomUpThreshold, dialogPtr, itemHit, 2);
						if (variable >= 0 && variable <= 100.0)
							bottomUpThreshold = variable;
						break;
						
					case 30:		// both odd choices number features
						if (theNum > gProjectInfoPtr->numberStatisticsChannels)
							NumberErrorAlert (bothOddChoicesNumberFeatures,
													dialogPtr, 
													itemHit);
						if (theNum >= 0 &&
											theNum <= gProjectInfoPtr->numberStatisticsChannels)
							bothOddChoicesNumberFeatures = theNum;
						break;
							
					}	// end "switch (itemHit)"
					
				}	// end "if (itemHit > 2)" 
				
			else if (itemHit > 0) 	// and itemHit <= 2 
				{	
				modalDone = TRUE;
					
				if (itemHit == 1)
					{					
					ProjectionPursuitDialogOK (projectionPursuitSpecsPtr,
															numericalOptimizationFlag,
															algorithmCode,
															initialGroupingCode,
															initialNumberFeatures,
															firstStageMethod,
															optimizeThreshold,
															topDownThreshold,
															bottomUpThreshold,
															maximumNumberFeatures,
															bothOddChoicesNumberFeatures);

					returnFlag = TRUE;

					}	// end if (itemHit == 1) 
				
				if (itemHit == 2)
					returnFlag = FALSE;
					
				}	// end "else if (itemHit > 0) and itemHit <= 2" 
					
			}	while (!modalDone);
			
		CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
	#endif	// defined multispec_mac
									
	#if defined multispec_win   
		returnFlag = FALSE;            
	
		CMProjectionPursuitDialog*		dialogPtr = NULL;
			
		projectionPursuitSpecsPtr = 
						&gFeatureExtractionSpecsPtr->projectionPursuitParams;
		
		TRY
			{ 
			dialogPtr = new CMProjectionPursuitDialog ();
			
			returnFlag = dialogPtr->DoDialog (projectionPursuitSpecsPtr); 
		                       
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
		returnFlag = FALSE;            
	
		CMProjectionPursuitDialog*		dialogPtr = NULL;
			
		projectionPursuitSpecsPtr = &gFeatureExtractionSpecsPtr->projectionPursuitParams;
		
		try{ 
			dialogPtr = new CMProjectionPursuitDialog ();
			returnFlag = dialogPtr->DoDialog (projectionPursuitSpecsPtr); 		                       
			delete dialogPtr;
			}
			
		catch (int e)
			{
			MemoryMessage (0, kObjectMessage);
			returnFlag = FALSE;
			}						
	#endif	// defined multispec_wx

	return (returnFlag);

}	// end "ProjectionPursuitDialog" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ProjectionPursuitDialogInitialize
//
//	Software purpose:	The purpose of this routine is to initialize the parameters for
//							the projection pursuit preprocessor dialog box.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	
//
// Called By:			ProjectionPursuitDialog
//
//	Coded By:			Larry L. Biehl			Date: 03/06/2007
//	Revised By:			Larry L. Biehl			Date: 03/06/2007
	
void ProjectionPursuitDialogInitialize (
				DialogPtr							dialogPtr,
				ProjectionPursuitSpecsPtr 		projectionPursuitSpecsPtr,
				Boolean*								numericalOptimizationFlagPtr,
				SInt16*								algorithmCodePtr,
				SInt16*								initialGroupingCodePtr,
				SInt16*								initialNumberFeaturesPtr,
				SInt16*								finalNumberFeaturesPtr,
				SInt16*								firstStageMethodPtr,
				double*								optimizeThresholdPtr,
				double*								topDownThresholdPtr,
				double*								bottomUpThresholdPtr,
				SInt32*								numberFeaturesPtr,
				SInt32*								bothOddChoicesNumberFeaturesPtr)

{
						
						
	*numericalOptimizationFlagPtr = 
								projectionPursuitSpecsPtr->numericalOptimizationFlag;	
			
			// Set algorithm and
			// Set numerical optimization flag.							
	
	*algorithmCodePtr = projectionPursuitSpecsPtr->algorithm;	
	ProjectionPursuitDialogAlgorithm (dialogPtr, 
													*algorithmCodePtr, 
													*numericalOptimizationFlagPtr);
			
			// Initial grouping options.
	
	*initialGroupingCodePtr = projectionPursuitSpecsPtr->initialGroupingCode;
	if (projectionPursuitSpecsPtr->finalNumberFeatures == 0)
		{	
		SetDLogControlHilite (dialogPtr, IDC_LastFinalGrouping, 255);
		*initialGroupingCodePtr = kUniformInitGrouping;
		
		}	// end "if (projectionPursuitSpecsPtr->finalNumberFeatures == 0)"
								
	ProjectionPursuitDialogInitialGrouping (dialogPtr, 
															*initialGroupingCodePtr);
														
	*initialNumberFeaturesPtr = projectionPursuitSpecsPtr->initialNumberFeatures;
	
	*finalNumberFeaturesPtr = projectionPursuitSpecsPtr->finalNumberFeatures;
		
			// Band grouping methods.						
	
	*firstStageMethodPtr = projectionPursuitSpecsPtr->firstStageMethod;
	ProjectionPursuitDialogFSOptions (dialogPtr, 
													*algorithmCodePtr, 
													*firstStageMethodPtr);
	
			// Thresholds.				
		
	*optimizeThresholdPtr = projectionPursuitSpecsPtr->optimizationThreshold;
		
	*topDownThresholdPtr = projectionPursuitSpecsPtr->tdThreshold;
		
	*bottomUpThresholdPtr = projectionPursuitSpecsPtr->buThreshold;
	
			// Maximum number features.				
		
	*numberFeaturesPtr = projectionPursuitSpecsPtr->maximumNumberFeatures;
	
			// Both odd choices number of features.
			
	*bothOddChoicesNumberFeaturesPtr = 
							projectionPursuitSpecsPtr->bothOddChoicesNumberFeatures;

}	// end "ProjectionPursuitDialogInitialize" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ProjectionPursuitDialogOK
//
//	Software purpose:	The purpose of this routine is to set the parameters for
//							the projection pursuit preprocessor structure using the user
//							specified item in the dialog box.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	
//
// Called By:			ProjectionPursuitDialog
//
//	Coded By:			Larry L. Biehl			Date: 03/07/2007
//	Revised By:			Larry L. Biehl			Date: 03/07/2007
	
void ProjectionPursuitDialogOK (
				ProjectionPursuitSpecsPtr 		projectionPursuitSpecsPtr,
				Boolean								numericalOptimizationFlag,
				SInt16								algorithmCode,
				SInt16								initialGroupingCode,
				SInt16								initialNumberFeatures,
				SInt16								firstStageMethod,
				double								optimizeThreshold,
				double								topDownThreshold,
				double								bottomUpThreshold,
				SInt32								maximumNumberFeatures,
				SInt32								bothOddChoicesNumberFeatures)

{
	projectionPursuitSpecsPtr->algorithm = algorithmCode;
	
	projectionPursuitSpecsPtr->numericalOptimizationFlag =
														numericalOptimizationFlag;
	
	projectionPursuitSpecsPtr->optimizationThreshold = optimizeThreshold;
	
	projectionPursuitSpecsPtr->initialGroupingCode = initialGroupingCode;
	
	projectionPursuitSpecsPtr->initialNumberFeatures = initialNumberFeatures;
	
	projectionPursuitSpecsPtr->maximumNumberFeatures = (UInt16)maximumNumberFeatures;

	projectionPursuitSpecsPtr->firstStageMethod = firstStageMethod;

	projectionPursuitSpecsPtr->tdThreshold = topDownThreshold;

	projectionPursuitSpecsPtr->buThreshold = bottomUpThreshold;
	
	projectionPursuitSpecsPtr->bothOddChoicesNumberFeatures = 
														(UInt16)bothOddChoicesNumberFeatures;

}	// end "ProjectionPursuitDialogOK" 


	    	
//------------------------------------------------------------------------------------
             	
void ProjectionPursuitDialogAlgorithm (
				DialogPtr							dialogPtr, 
				SInt16								algorithmCode,
				Boolean								numericalOptimizationFlag)

{				
	SetDLogControl (
			dialogPtr, IDC_PP_Algorithm, (algorithmCode == kProjectionPursuit));
	SetDLogControl (
			dialogPtr, IDC_FS_Algorithm, (algorithmCode == kPPFeatureSelection));
			
	ProjectionPursuitDialogOptimizeFlag (
									dialogPtr, numericalOptimizationFlag);

}	// end "ProjectionPursuitDialogAlgorithm" 

	
               
void ProjectionPursuitDialogInitialGrouping (
				DialogPtr							dialogPtr, 
				SInt16								initialGroupingCode)

{				
	SetDLogControl (
			dialogPtr, IDC_UniformGrouping, (initialGroupingCode == kUniformInitGrouping));
	SetDLogControl (
			dialogPtr, IDC_LastFinalGrouping, (initialGroupingCode == kLastGrouping));
			
	if (initialGroupingCode == kUniformInitGrouping)
		{
		ShowDialogItem (dialogPtr, IDC_EditNumberFeatures);
		HideDialogItem (dialogPtr, IDC_SetFeatures);
		
		}	// end "if (initialGroupingCode == kUniformInitGrouping)"
			
	if (initialGroupingCode == kLastGrouping)
		{
		HideDialogItem (dialogPtr, IDC_EditNumberFeatures);
		ShowDialogItem (dialogPtr, IDC_SetFeatures);
		
		}	// end "if (initialGroupingCode == kLastGrouping)"

}	// end "ProjectionPursuitDialogInitialGrouping" 

	

void ProjectionPursuitDialogFSMethod (
				DialogPtr							dialogPtr, 
				SInt16								firstStageMethod)

{					
	SetDLogControl (
			dialogPtr, IDC_Uniform, (firstStageMethod == kUniformGrouping));
	SetDLogControl (
			dialogPtr, IDC_TopDownMethod, (firstStageMethod == kTopDown));
	SetDLogControl (
			dialogPtr, IDC_TopDownBottomUpMethod, (firstStageMethod == kTopDownBottomUp));	
		
	if (firstStageMethod == kUniformGrouping)
		{
		HideDialogItem (dialogPtr, IDC_TopDownThresholdPrompt);
		HideDialogItem (dialogPtr, IDC_TopDownThreshold);
		HideDialogItem (dialogPtr, IDC_BottomUpThresholdPrompt);
		HideDialogItem (dialogPtr, IDC_BottomUpThreshold);
		HideDialogItem (dialogPtr, IDC_BothChoicesLabel1);
		HideDialogItem (dialogPtr, IDC_BothChoicesUpToValue);
		HideDialogItem (dialogPtr, IDC_BothChoicesLabel2);
		
		}	// end "if (firstStageMethod == kUniformGrouping)"
		
	else	// firstStageMethod != kUniformGrouping
		{
		ShowDialogItem (dialogPtr, IDC_TopDownThresholdPrompt);
		ShowDialogItem (dialogPtr, IDC_TopDownThreshold);
			
		if (firstStageMethod == kTopDownBottomUp)
			{
			ShowDialogItem (dialogPtr, IDC_BottomUpThresholdPrompt);
			ShowDialogItem (dialogPtr, IDC_BottomUpThreshold);

			}	// end "if (firstStageMethod == kTopDownBottomUp)"
			
		else	// algorithmCode != kTopDownBottomUp
			{
			HideDialogItem (dialogPtr, IDC_BottomUpThresholdPrompt);
			HideDialogItem (dialogPtr, IDC_BottomUpThreshold);

			}	// end "else algorithmCode != kTopDownBottomUp"
			
		ShowDialogItem (dialogPtr, IDC_BothChoicesLabel1);
		ShowDialogItem (dialogPtr, IDC_BothChoicesUpToValue);
		ShowDialogItem (dialogPtr, IDC_BothChoicesLabel2);
			
		}	// end "else firstStageMethod != kUniformGrouping"		
			
}	// end "ProjectionPursuitDialogFSMethod" 


	
//------------------------------------------------------------------------------------

void ProjectionPursuitDialogOptimizeFlag (
				DialogPtr							dialogPtr,
				Boolean								numericalOptimizationFlag)

{								
	SetDLogControl (dialogPtr, IDC_NumericalOptimization, numericalOptimizationFlag);
		
	if (numericalOptimizationFlag)
		{
		ShowDialogItem (dialogPtr, IDC_OptimizationThresholdPrompt);
		ShowDialogItem (dialogPtr, IDC_OptimizationThreshold);

		}	// end "if (numericalOptimizationFlag)"
		
	else	// !numericalOptimizationFlag
		{
		HideDialogItem (dialogPtr, IDC_OptimizationThresholdPrompt);
		HideDialogItem (dialogPtr, IDC_OptimizationThreshold);

		}	// end "else !numericalOptimizationFlag"

}	// end "ProjectionPursuitDialogOptimizeFlag"  


	          
//------------------------------------------------------------------------------------

void ProjectionPursuitDialogFSOptions (
				DialogPtr							dialogPtr,
				SInt16								algorithmCode, 
				SInt16								firstStageMethod)

{	
	ProjectionPursuitDialogFSMethod (dialogPtr, firstStageMethod);	
			
}	// end "ProjectionPursuitDialogFSOptions" 
