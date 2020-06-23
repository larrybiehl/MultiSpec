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
//	File:						SClassify.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			05/12/2020
//
//	Language:				C
//
//	System:					Linux, Macintosh and Windows Operating Systems
//
//	Brief description:	This file contains functions that do classification
//								processes.
//
//	Diagram of MultiSpec routine calls for the routines in the file.
//
//		ClassifyControl
//			GetProjectImageFileInfo (in SProject.cpp)*
//			LoadClassifySpecs*
//
//			ClassifyDialog
//				SetDLogControl (in SDialogUtilities.cpp)
//				SetDLogControlHilite (in SDialogUtilities.cpp)
//				LoadDItemValue (in SDialogUtilities.cpp)
//				CheckSomeEvents (in MMultiSpec.c & SStubs.cpp)
//				HiliteOKButton (in dialogUtilities.c)
//				ProcessorDialogFilter
//				StandardPopUpMenu (MacOS routine)
//				ChannelsDialog (MacOS routine)
//				ClassDialog (MacOS routine)
//				NumberErrorAlert (in SDialogUtilities.cpp)
//				LoadFeatureVector (in SUtilities.cpp)
//				LoadTrainClassVector (in SUtilities.cpp)
//				GetDLogControl (MacOS routine)
//				ChangeDLogCheckBox (MacOS routine)
//
//			VerifyProjectStatsUpToDate (in SProjectUtilities.cpp)*
//			CheckClassifyTextWindowSpaceNeeded*
//			UnlockProjectWindowInfoHandles (in SProject.cpp)*
//			GetImageInformationPointers (in SUtilities.cpp)*
//			InitializeAreaDescription (in SUtilities.cpp)*
//			CreateResultsDiskFiles (in SUtilities.cpp)*
//			GetProjectFieldsBoundingArea (in SProjectUtilities.cpp)*
//			GetStatusDialog (in SDialogUtilities.cpp)
//			MSetCursor (in SUtility.cpp)
//			ForceTextToEnd (in SStrings.cpp)
//			ListClassifyInputParameters
//			CheckNumberOfPixelsInClass (in SProjectUtilities.cpp)
//			ListSpecifiedStringNumber (in SStrings.cpp)
//			DisplayAlert (in SDialogUtilities.cpp)
//			ListSpecifiedStringNumber (in SStrings.cpp)
//			CreateResultsDiskFiles (in SUtilities.cpp)
//			InitializeGlobalAlertVariables (in SProjectUtilities.cpp)
//
//			MaxLikeClsfierControl
//				InitializeClassifierVarStructure
//				SetupClsfierMemory
//				GetStatisticsFeatureVector (in SProjectComputeStatistics.cpp)
//				GetTransformedClassCovarianceMatrix (in SProjectComputeStatistics.cpp)
//				ReduceMeanVector (in SProjectComputeStatistics.cpp)
//				InvertSymmetricMatrix  (SProjectMatrixUtilities.cpp)
//				MatrixMultiply  (in SMatrixUtilities.cpp)
//				SquareToTriangularMatrix (in SProjectUtilities.cpp)
//				CreateThresholdTables
//				ClassifyAreasControl
//					GetListResultsStringBuffer (in SListResults.cpp)
//					LoadDItemString (in SDialogUtilities.cpp)
//					CheckSomeEvents (in MMultiSpec.c or SStubs.cpp)
//					OutputString (in SStrings.cpp)
//						ListString (in SStrings.cpp)
//					ClassifyTrainTestFields
//						LoadDItemValue (in SDialogUtilities.cpp)
//						OutputString (in SStrings.cpp)
//							ListString (in SStrings.cpp)
//						GetFieldBoundary (in SProjectUtilities.cpp)
//						ClassifyArea
//							ClassifyPerPointArea
//								GetLineOfData (in SFileIO.cpp)
//								MaximumLikelihoodClassifier
//								FisherClassifier
//								EuclideanClassifier
//								CorrelationClassifier
//								CEMClassifier
//								WriteClassificationResults
//									OutputString (in SStrings.cpp)
//								CopyToOffscreenBuffer
//								UnlockImageOverlayOffscreenBuffer
//								CloseUpFileIOInstructions (in SFileIO.cpp)
//								ListTrainTestSummary
//
//							EchoClassifier (in SClassifyEcho.cpp)
//
//					ClassifyArea
//						"see above"
//
//					ListClassificationSummary (in )
//
//			FisherClsfierControl
//				SetupClsfierMemory
//				ReduceMeanVector (in SUtilities.cpp)
//				ClassifyAreasControl
//					"See above"
//
//			EuclideanClsfierControl
//
//			EchoClsfierControl (in SClassifyEchoControl.cpp)
//				InitializeClassifierVarStructure
//				initialize_common_info (in SEMemory.cpp)
//				Interface (in SClassifyEchoControl.cpp)
//				SetupClsfierStats (in SClassifyEchoControl.cpp)
//				setup_basic_echo_information (in SAuxEcho.cpp)
//				CreateThresholdTables
//				ClassifyAreasControl
//					"See above"
//					...
//					EchoClassifier (in SClassifyEcho.cpp)
//						setup_echo_image_selection (in SAuxEcho.cpp)
//						malloc_epix (in SAuxEcho.cpp)
//						free_epix (in SAuxEcho.cpp)
//						SetUpFileIOInstructions (in SFileIO.cpp)
//						ShowStatusDialogItemSet (in SDialogUtilities.cpp)
//						phase1 (in SClassifyEchoPhase.cpp)
//							read_lines_of_data1 (in SAuxSpec.cpp)
//							loglik_echo (in SClassifyEchoPhase.cpp)
//							log_lik_ratio (in SAuxEcho.cpp)
//							BlockMoveData (in SStubs.cpp)
//							stuffing (SAuxEcho.cpp)
//							CheckSomeEvents (in MMultiSpec.c or SStubs.cpp)
//							subtract_log_lik (in SAuxEcho.cpp)
//							ClassifyNonHomogeneousCells (in SClassifyEchoPhase.cpp)
//								classify_pixel_using_ML (in SClassifyEchoPhase.cpp)
//							SaveProbabilityInformation (in SClassifyEcho.cpp)
//							
//						Write_Homogeneous_Fields_File (in SClassifyEcho.cpp)
//						Write_Classified_Fields_File (in SClassifyEcho.cpp)
//						PostEchoClassifier (in SClassifyEchoControl.cpp)
//						free_epix (in SAuxEcho.cpp)
//
//				free_class_stat (in SEMemory.cpp)
//
//			CorrelationClsfierControl
//
//			CEMClsfierControl
//				InitializeAreaDescription
//				GetProjectImageFileInfo
//				GetIOBufferPointers
//				DrawDialog
//				GetTotalSumSquares
//				DisposeIOBufferPointers (in SMemoryUtilities.cpp)
//				GetTotalTime
//				SetupClsfierMemory
//				ComputeCorrelationMatrix
//				SetupFeatureTransformationMemory
//				TransformSymmetricMatrix (in SMatrixUtilities.cpp)
//				InvertSymmetricMatrix (SProjectMatrixUtilities.cpp)
//				MatrixMultiply (in SMatrixUtilities.cpp)
//				MatrixMultiply (in SMatrixUtilities.cpp)
//				GetStatisticsFeatureVector
//				GetClassMeanVector
//				MatrixMultiply (in SMatrixUtilities.cpp)
//				MatrixMultiply (in SMatrixUtilities.cpp)
//				ReleaseMatrixInversionMemory
//				ReleaseFeatureTransformationMemory
//				ClassifyAreasControl
//					"See above"
//					...
//					CEMClassifier
//						CheckSomeEvents
//
//			ClearGlobalAlertVariables (in SProjectUtilities.cpp)
//			UpdateOverlayControl (in MControls.c and)
//			UnlockImageOverlayInfoHandle (in SImageOverlays.cpp)
//			ListSpecifiedStringNumber (in SStrings.cpp)
//			ListCPUTimeInformation (in SStrings.cpp)
//			UpdateOutputWScrolls (in MControls.c and)
//			UnlockImageInformationHandles (in SUtility.cpp)
//			CloseResultsFiles (in SUtility.cpp)
//			CloseStatusDialog (in SDialogUtilities.cpp)
//			MInitCursor (in SUtility.cpp)
//			CheckAndUnlockHandle (in SMemoryUtilities.cpp)
//			UnlockProjectWindowInfoHandles (in SProject.cpp)
//
//	Include files:			"MultiSpecHeaders"
//								"SMultiSpec.h"
//
//------------------------------------------------------------------------------------

#include	"SMultiSpec.h"
#include	"Ssvm.h"

#if defined multispec_wx
	#include "SMultiSpec.h"
	#include "xImageView.h"
#endif

#if defined multispec_mac || defined multispec_mac_swift
	#define	IDS_Classify67				67
	#define	IDS_Classify68				68
	#define	IDS_Classify69				69
	#define	IDS_Classify70				70
#endif	// defined multispec_mac || defined multispec_mac_swift  
  
#if defined multispec_win  
	#include "WImageView.h"
#endif	// defined multispec_win  

#define	kDoNotIncludeTab						0


/*
bool Question1 (
				int 											column,
				double 										value,
				double*										data)

{
	if (data[column] >= value)
		return true;
    
	else
		return false;
 
}	// end "Question1"



void classify (
				double*										data,
				RFNode*										root,
				int 											col,
				double*										label)

{
	if (root != NULL)
		{
		col --;
		if (Question1 (col, root->data, data))
			{
					// reach leaf node
			if (root->left == NULL)
				{
				//std::cout << "left:" << root->data << "," << col << "," << data[col] << std::endl;
				*label = root->data;
				//return root->data;
 
				}
 
			classify (data, root->left, col, label);
 
	  		}
		else
	  		{
			if (root->right == NULL)
				{
				//std::cout << "right:" << root->data << "," << col <<"," << data[col] << std::endl;
				*label = root->data;
				//return root->data;
				
				}
			
			classify(data, root->right, col, label);
	  		
			}
		
		}
 
}	// end "classify"
*/


extern void	ListString (
				const char*							textBuffer);



			// Prototypes for routines in this file that are only called by		
			// other routines in this file.	
			
void CheckAndUnlockCEMParameterHandles (void);
								
SInt16 CEMClassifier (
				AreaDescriptionPtr				areaDescriptionPtr,
				FileInfoPtr							fileInfoPtr,
				ClassifierVarPtr					clsfyVariablePtr, 
				HUCharPtr							outputBuffer1Ptr, 
				HUCharPtr							probabilityBufferPtr, 
				HSInt64Ptr							countVectorPtr, 
				Point									point);

void CEMClsfierControl (
				FileInfoPtr							fileInfoPtr);
	
Boolean CheckClassifyTextWindowSpaceNeeded (void);
		
SInt16 ClassifyArea (
				SInt16								classPointer,
				AreaDescriptionPtr				areaDescriptionPtr, 
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				LCToWindowUnitsVariables* 		lcToWindowUnitsVariablesPtr,
				ClassifierVarPtr					clsfyVariablePtr, 
				HSInt64Ptr							countVectorPtr);

SInt16 ClassifyPerPointArea (
				SInt16								classPointer,
				AreaDescriptionPtr				areaDescriptionPtr, 
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				LCToWindowUnitsVariables* 		lcToWindowUnitsVariablesPtr,
				ClassifierVarPtr					clsfyVariablePtr, 
				HSInt64Ptr							countVectorPtr);

SInt16 ClassifyTrainTestFields (
				AreaDescriptionPtr				areaDescriptionPtr,
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				LCToWindowUnitsVariables* 		lcToWindowUnitsVariablesPtr,
				ClassifierVarPtr					clsfyVariablePtr, 
				SInt16								fieldType);

Boolean CreateCorrelationThresholdTable (
				double*								thresholdTablePtr);
									
SInt16 CorrelationClassifier (
				AreaDescriptionPtr				areaDescriptionPtr,
				FileInfoPtr							fileInfoPtr,
				ClassifierVarPtr					clsfyVariablePtr, 
				HUCharPtr							outputBuffer1Ptr, 
				HUCharPtr							probabilityBufferPtr, 
				HSInt64Ptr							countVectorPtr, 
				Point									point);

void CorrelationClsfierControl (
				FileInfoPtr							fileInfoPtr);

UInt32 CorrelationThresholdClass (
				double								dValue,
				double*								thresholdTablePtr);
										
Boolean CreateChiSquareThresholdTable (
				SInt32 								degreesOfFreedom,
				double* 								thresholdTablePtr);

Boolean CreateSupportVectorMachineModel (
				ClassifySpecsPtr					classifySpecsPtr);

Boolean CreateTrailerFiles (
				ClassifierVarPtr 					clsfyVariablePtr,
				SInt16 								numberClasses, 
				SInt16* 								classPtr,
				SInt16								numberListClasses);

SInt16 FisherClassifier (
				AreaDescriptionPtr				areaDescriptionPtr,
				FileInfoPtr							fileInfoPtr,
				ClassifierVarPtr					clsfyVariablePtr, 
				HUCharPtr							outputBuffer1Ptr, 
				HUCharPtr							probabilityBufferPtr, 
				HSInt64Ptr							countVectorPtr, 
				Point									point);

void FisherClsfierControl (
				FileInfoPtr							fileInfoPtr);

Boolean  GetClassifyDataBuffers (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				WindowInfoPtr						windowInfoPtr,
				LayerInfoPtr						layerInfoPtr, 
				FileInfoPtr							fileInfoPtr, 
				EchoClassifierVarPtr 			echoClassifierVarPtr,
				FileIOInstructionsPtr*			outputFileIOInstructionsPtrPtr);

Boolean GetThresholdTableMemory (void);

Boolean GetTransformedCommonCovariance (
				UInt16								numberOutputChannels,
				HChannelStatisticsPtr			classChannelStatsPtr,
				HDoublePtr							covariancePtr,
				UInt16*								channelListPtr,
				UInt16*								classPtr,  
				UInt32								numberClasses,
				Boolean								squareOutputMatrixFlag,
				HDoublePtr							eigenVectorPtr,
				HDoublePtr							tempMatrixPtr,
				UInt16								numberFeatureChannels);

Boolean  GetTtInvTransformedCommonCovarianceT (
				UInt16								numberOutputChannels,
				HChannelStatisticsPtr			classChannelStatsPtr,
				HDoublePtr							covariancePtr,
				UInt16*								channelListPtr,
				UInt16*								classPtr,  
				UInt32								numberClasses,
				Boolean								squareOutputMatrixFlag,
				HDoublePtr							eigenVectorPtr,
				HDoublePtr							tempMatrixPtr,
				UInt16								numberFeatures,
				double*								logDeterminantPtr,
				Boolean*								determinantOKFlagPtr);

Boolean GetTtInvTransformedCovarianceT (
				UInt16								numberOutputChannels,
				HChannelStatisticsPtr			classChannelStatsPtr,
				HDoublePtr							covariancePtr,
				UInt16*								channelListPtr, 
				UInt16								statClassNumber,
				Boolean								squareOutputMatrixFlag, 
				SInt16								outputStatisticsCode,
				HDoublePtr							eigenVectorPtr,
				HDoublePtr							tempMatrixPtr,
				UInt16								numberFeatures,
				double*								logDeterminantPtr,
				Boolean*								determinantOKFlagPtr);

Boolean LoadClassifySpecs (
				FileInfoPtr							fileInfoPtr);

Boolean LoadProbabilityTable (void);

Boolean LoadThresholdTable (
				SInt16 								numberDegreesOfFreedom,
				SInt16 								tableCode);

SInt16 EuclideanClassifier (
				AreaDescriptionPtr				areaDescriptionPtr,
				FileInfoPtr							fileInfoPtr,
				ClassifierVarPtr					clsfyVariablePtr, 
				HUCharPtr							outputBuffer1Ptr, 
				HUCharPtr							probabilityBufferPtr, 
				HSInt64Ptr							countVectorPtr, 
				Point									point);

void EuclideanClsfierControl (
				FileInfoPtr							fileInfoPtr);

void KNearestNeighborClassifierControl (
				FileInfoPtr							fileInfoPtr);

SInt16 KNNClassifier (
				AreaDescriptionPtr            areaDescriptionPtr,
				FileInfoPtr                   fileInfoPtr,
				ClassifierVarPtr              clsfyVariablePtr,
				HUCharPtr                     outputBuffer1Ptr,
				HUCharPtr                     thresholdBufferPtr,
				HSInt64Ptr                    countVectorPtr,
				Point                         point);

Boolean ListClassifyInputParameters (
				FileInfoPtr							fileInfoPtr,
				CMFileStream*						resultsFileStreamPtr);

Boolean ListClassifySVMInputParameters (
				FileInfoPtr							fileInfoPtr,
				CMFileStream*						resultsFileStreamPtr);

Boolean ListNumberOfSameDistanceSamples (
				CMFileStream*						resultsFileStreamPtr,
				SInt64								totalSameDistanceSamples);

void MaxLikeClsfierControl (
				FileInfoPtr							fileInfoPtr);

SInt16 MaximumLikelihoodClassifier (
				AreaDescriptionPtr				areaDescriptionPtr,
				FileInfoPtr							fileInfoPtr,
				ClassifierVarPtr					clsfyVariablePtr, 
				HUCharPtr							outputBuffer1Ptr, 
				HUCharPtr							probabilityBufferPtr, 
				HSInt64Ptr							countVectorPtr, 
				Point									point);

SInt16 ParallelPipedClassifier (
				AreaDescriptionPtr				areaDescriptionPtr,
				FileInfoPtr							fileInfoPtr,
				ClassifierVarPtr					clsfyVariablePtr, 
				HUCharPtr							outputBuffer1Ptr, 
				HUCharPtr							probabilityBufferPtr, 
				HSInt64Ptr							countVectorPtr, 
				Point									point);

void ParallelPipedClsfierControl (
				FileInfoPtr							fileInfoPtr);
													
Boolean SetupClsfierMemory (
				HDoublePtr*							channelMeanPtr,
				HDoublePtr*							covariancePtr, 
				double**								classConstantPtr,
				double**								classConstantLOO1PtrPtr,
				double**								classConstantLOO2PtrPtr,
				double**								classConstantLOO3PtrPtr,
				double**								classConstantLOO4PtrPtr,
				double**								classConstantLOO5PtrPtr,
				HChannelStatisticsPtr*			classChannelStatsPtr, 
				HDoublePtr*							inverseCovPtr, 
				HDoublePtr*							pivotPtr, 
				SInt16**								indexRowPtr, 
				SInt16**								indexColPtr, 
				SInt16**								ipvotPtr);

SInt16 SupportVectorMachineClassifier (
				AreaDescriptionPtr            areaDescriptionPtr,
				FileInfoPtr                   fileInfoPtr,
				ClassifierVarPtr              clsfyVariablePtr,
				HUCharPtr                     outputBuffer1Ptr,
				HUCharPtr                     probabilityBufferPtr,
				HSInt64Ptr                    countVectorPtr,
				Point                         point);

void SupportVectorMachineClassifierControl (
				FileInfoPtr							fileInfoPtr);
									
void WriteProbabilityGrouping (
				SInt16								classificationMode);



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CEMClsfierControl
//
//	Software purpose:	The purpose of this routine is to classify the
//							specified data using the point CEM (matched filter) 
//							classifer.
//
//	Parameters in:		Pointer to image file information structure
//
//	Parameters out:	None
//
// Value Returned:	Flag indicating whether output to text window
//								can still be generated.
// 
// Called By:			ClassifyControl
//
//	Coded By:			Larry L. Biehl			Date: 08/21/1997
//	Revised By:			Larry L. Biehl			Date: 12/22/2005

void CEMClsfierControl (
				FileInfoPtr							fileInfoPtr)

{
	ClassifierVar						classifierVar;
	
	double								determinant,
											logDeterminant;
												
	CEMParametersPtr 					cemParametersPtr;
	
	CMFileStream*						resultsFileStreamPtr;
	FileInfoPtr							savedFileInfoPtr;
	FileIOInstructionsPtr			fileIOInstructionsPtr;
	LayerInfoPtr						savedLayerInfoPtr;
	WindowInfoPtr						savedWindowInfoPtr;
												
	Handle								totalChanStatsHandle = NULL,
											totalSumSquaresStatsHandle = NULL;
												
	HChannelStatisticsPtr			classChannelStatsPtr = NULL;
	
	HSumSquaresStatisticsPtr		totalSumSquaresStatsPtr;
												
	HDoublePtr							classifyChanMeanPtr = NULL,
											correlationPtr = NULL,
											inverseCorPtr = NULL,
											tempChanMeanPtr;
												
	ImageSecondOrderStatsPtr		imageSecondOrderStatsPtr;
	
	SInt16								*statFeaturePtr = NULL;
											
	UInt16 								*channelsPtr,
											*correlationMatrixChannelsPtr = NULL,
											*correlationMatrixClassPtr,
											*correlationMatrixFeaturePtr = NULL,
											*featurePtr;
	
	UInt32								clsfyCEMOperatorStart;
	
	UInt16								classIndex,
											numberCorrelationMatrixChannels,
											numberFeatureChannels,
											numberFeatures,
											statClassNumber;
												
	Boolean								continueFlag,
											determinantOKFlag = TRUE,
											restoreTargetGlobalInfoPointersFlag;
	
		
			// Initialize local variables														
	
	continueFlag = TRUE;
	restoreTargetGlobalInfoPointersFlag = FALSE;
	numberFeatureChannels = gClassifySpecsPtr->numberChannels;
	numberFeatures = gClassifySpecsPtr->numberFeatures;
 	resultsFileStreamPtr = GetResultsFileStreamPtr (0);
	fileIOInstructionsPtr = NULL; 
 	
			// Get pointer to the image second order statistics.
	
	imageSecondOrderStatsPtr = &gImageWindowInfoPtr->imageSecondOrderStats;
	
			// Get pointer to the channels vector.
											
	channelsPtr = (UInt16*)GetHandlePointer (gClassifySpecsPtr->channelsHandle);
	
			// Get pointer to the channel feature vector.
											
	featurePtr = (UInt16*)GetHandlePointer (gClassifySpecsPtr->featureHandle);
	
			// Put description in the status dialog.										
					
	LoadDItemStringNumber (kClassifyStrID, 
									IDS_Classify20, 
									gStatusDialogPtr, 
									IDC_Status11, 
									(Str255*)gTextString);
		
	InitializeClassifierVarStructure (&classifierVar);
	
			// Get pointer to the CEM parameter structure
			
	cemParametersPtr = (CEMParametersPtr)GetHandlePointer (
																gClassifySpecsPtr->cemParametersH,
																kLock);
	
			// Get the pointer to the class areas to be used for computing the
			// correlation matrix. Note that the handle is already locked.
													
	correlationMatrixClassPtr = (UInt16*)GetHandlePointer (
											cemParametersPtr->correlationMatrixClassHandle);
	
			// Get the Correlation Matrix
											
	Boolean 		computeNewSumSquaresMatrixFlag = FALSE;
		
	if (cemParametersPtr->correlationMatrixCode & kTrainingType)
		{
		computeNewSumSquaresMatrixFlag = TRUE;
		
				// Set the line and column intervals to 1 in case the stats need to be
				// computed.
				
		gAreaDescription.lineInterval = 	1;
		gAreaDescription.columnInterval = 1;
		
				// Only the channels to be used for the classification will be obtained.
				
		correlationMatrixFeaturePtr = featurePtr;
		correlationMatrixChannelsPtr = channelsPtr;
		numberCorrelationMatrixChannels = gClassifySpecsPtr->numberChannels;
		
		}	// end "if (cemParametersPtr->correlationMatrixCode & kTrainingType)"
		
	else	// cemParametersPtr->correlationMatrixCode & kAreaType
		{				
		if (imageSecondOrderStatsPtr->totalPixels > 0)
			{
			if ((imageSecondOrderStatsPtr->lineStart != cemParametersPtr->lineStart) ||
					(imageSecondOrderStatsPtr->lineEnd != cemParametersPtr->lineEnd) ||
					(imageSecondOrderStatsPtr->lineInterval != cemParametersPtr->lineInterval) ||
					(imageSecondOrderStatsPtr->columnStart != cemParametersPtr->columnStart) ||
					(imageSecondOrderStatsPtr->columnEnd != cemParametersPtr->columnEnd) ||
					(imageSecondOrderStatsPtr->columnInterval != cemParametersPtr->columnInterval))
				computeNewSumSquaresMatrixFlag = TRUE;
			
			}	// end "if (secondOrderImageStatsPtr->totalPixels > 0)"
			
		else	// secondOrderImageStatsPtr->totalPixels == 0
			computeNewSumSquaresMatrixFlag = TRUE;
			
		if (computeNewSumSquaresMatrixFlag)
			{
			InitializeAreaDescription (&gAreaDescription, 
													cemParametersPtr->lineStart, 
													cemParametersPtr->lineEnd, 
													cemParametersPtr->columnStart, 
													cemParametersPtr->columnEnd, 
													cemParametersPtr->lineInterval, 
													cemParametersPtr->columnInterval,
													1,
													1,
													0);
													
					// The sums of squares for all available channels will be computed so 
					// that it can be saved in the image second order statistics structure.
					
			correlationMatrixFeaturePtr = NULL;
			correlationMatrixChannelsPtr = NULL;
			numberCorrelationMatrixChannels = gImageFileInfoPtr->numberChannels;
			
			totalSumSquaresStatsHandle = imageSecondOrderStatsPtr->sumSquaresHandle;
			
			}	// end "if (computeNewSumSquaresMatrixFlag)"
											
		}	// end "if (cemParametersPtr->correlationMatrixCode & kAreaType)"
	
	if (computeNewSumSquaresMatrixFlag)
		{		
				// Get the global pointers to the project base image if the target image
				// is different than the project base image. Be sure to restore the global
				// pointers after the new sum squares matrix has been computed.
									
		if (gClassifySpecsPtr->targetWindowInfoHandle != 
																	gProjectInfoPtr->windowInfoHandle)
			{
			savedWindowInfoPtr = gImageWindowInfoPtr;
			savedLayerInfoPtr = gImageLayerInfoPtr;
			savedFileInfoPtr = gImageFileInfoPtr;
			
			gImageWindowInfoPtr = NULL;
			gImageLayerInfoPtr = NULL;
			gImageFileInfoPtr = NULL;
										
			continueFlag = GetProjectImageFileInfo (kPrompt, kSetupGlobalInfoPointers);
			
			restoreTargetGlobalInfoPointersFlag = TRUE;
			
			}	// end "if (gClassifySpecsPtr->targetWindowInfoHandle != ..."
		
				// Get pointer to memory to use to read an image file line into to
				// compute the correlation matrix.																			
				 		
		if (continueFlag)	
			continueFlag = GetIOBufferPointers (
													&gFileIOInstructions[0],
													gImageWindowInfoPtr,
													gImageLayerInfoPtr,
													gImageFileInfoPtr,
													&gInputBufferPtr, 
													&gOutputBufferPtr,
													1,
													gImageWindowInfoPtr->maxNumberColumns,
													1,
													numberCorrelationMatrixChannels,
													(HUInt16Ptr)correlationMatrixChannelsPtr,
													kPackData,
													kForceBISFormat,
													kForceReal8Bytes,
													kDoNotAllowForThreadedIO,
													&fileIOInstructionsPtr);
			
				// Get the sums of squares					
				// Get the start time.															
						
		time_t startTime = time (NULL);

		if (continueFlag)
			{
			if (cemParametersPtr->correlationMatrixCode & kTrainingType)
				ShowStatusDialogItemSet (kStatusClassB);
			ShowStatusDialogItemSet (kStatusLine);
			ShowStatusDialogItemSet (kStatusCommand);
			
			if (gStatusDialogPtr != NULL)
				DrawDialog (gStatusDialogPtr);
			
			continueFlag = GetTotalSumSquares (
											fileIOInstructionsPtr,
											&totalChanStatsHandle,
											&totalSumSquaresStatsHandle,
											correlationMatrixChannelsPtr,
											numberCorrelationMatrixChannels,
											(SInt16*)correlationMatrixClassPtr, 
											cemParametersPtr->numbercorrelationMatrixClasses,
											cemParametersPtr->correlationMatrixCode,
											cemParametersPtr->correlationMatrixCode,
											FALSE);
									
			HideStatusDialogItemSet (kStatusLine);
			HideStatusDialogItemSet (kStatusClassB);
									
			}	// end "if (continueFlag)"
				
				// Dispose of the IO buffer.		
									
		DisposeIOBufferPointers (fileIOInstructionsPtr,
											&gInputBufferPtr, 
											&gOutputBufferPtr);	
		
		if (restoreTargetGlobalInfoPointersFlag)
			{
			UnlockProjectWindowInfoHandles ();
			
			gImageWindowInfoPtr = savedWindowInfoPtr;
			gImageLayerInfoPtr = savedLayerInfoPtr;
			gImageFileInfoPtr = savedFileInfoPtr;
			
			}	// end "if (restoreTargetGlobalInfoPointersFlag)"
		
		startTime = GetTotalTime (startTime);
	
				// List the total number of pixels used to compute the sums of squares.
				// 	"\r %ld pixels used for covariance matrix.\r"
		continueFlag = ListSpecifiedStringNumber (
											kPrincipalCompStrID, 
											IDS_PrincipalComponent1,
											resultsFileStreamPtr, 
											gOutputCode,
											gAreaDescription.numSamplesPerChan, 
											continueFlag);
		
				// List the total time used to computer the covariance matrix.	
				//		" %lu CPU seconds for computing correlation matrix.\r"
		continueFlag = ListSpecifiedStringNumber (
											kPrincipalCompStrID, 
											IDS_PrincipalComponent2,
											resultsFileStreamPtr, 
											gOutputCode,
											(SInt32)startTime, 
											continueFlag); 
		
				// If the sum squares were collected for the image and not from the class
				// information then save this information in the second order image 
				// statistics structure so that it can be used later.
				
		if (continueFlag && cemParametersPtr->correlationMatrixCode & kAreaType)
			{
			imageSecondOrderStatsPtr->sumSquaresHandle = totalSumSquaresStatsHandle;
			imageSecondOrderStatsPtr->lineStart = gAreaDescription.lineStart; 						
			imageSecondOrderStatsPtr->lineEnd = gAreaDescription.lineEnd;				
			imageSecondOrderStatsPtr->lineInterval = gAreaDescription.lineInterval; 					
			imageSecondOrderStatsPtr->columnStart = gAreaDescription.columnStart; 					
			imageSecondOrderStatsPtr->columnEnd = gAreaDescription.columnEnd;				
			imageSecondOrderStatsPtr->columnInterval = gAreaDescription.columnInterval;							
			imageSecondOrderStatsPtr->totalPixels = gAreaDescription.numSamplesPerChan;												
			imageSecondOrderStatsPtr->numberChannels = numberCorrelationMatrixChannels;
			
			}	// end "if (continueFlag && ...->correlationMatrixCode & kAreaType)"
						
					// Release memory used for image mean vector.
					
		totalChanStatsHandle = UnlockAndDispose (totalChanStatsHandle);
		
		}	// end "if (computeNewSumSquaresMatrixFlag)"
		
	else	// !computeNewSumSquaresMatrixFlag
		{
		totalSumSquaresStatsHandle = imageSecondOrderStatsPtr->sumSquaresHandle;
		MHLock (totalSumSquaresStatsHandle);
		
		}	// end "else !computeNewSumSquaresMatrixFlag"
	
			// Get the memory for the mean vectors			
			// for the classes and channels to be used in the classification.		
	
	if (continueFlag)			 
		continueFlag = SetupClsfierMemory (&classifyChanMeanPtr, 
														&correlationPtr,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL, 
														&classChannelStatsPtr, 
														&gInverseMatrixMemory.inversePtr, 
														&gInverseMatrixMemory.pivotPtr, 
														&gInverseMatrixMemory.indexRowPtr, 
														&gInverseMatrixMemory.indexColPtr, 
														&gInverseMatrixMemory.ipvotPtr);
								
	inverseCorPtr = gInverseMatrixMemory.inversePtr;
		
			// Get pointer to the image statistics. Note that the handle is
			// already locked.
		
	totalSumSquaresStatsPtr = (HCovarianceStatisticsPtr)GetHandlePointer (
																			totalSumSquaresStatsHandle);	
		
			// Convert the triangular covariance matrix to a square matrix.		
			
	if (continueFlag)
		ComputeCorrelationMatrix (gClassifySpecsPtr->numberChannels,
											inverseCorPtr,
											numberCorrelationMatrixChannels,
											featurePtr,
											totalSumSquaresStatsPtr, 
											gAreaDescription.numSamplesPerChan,
											kSquareOutputMatrix);
						
				// Release memory used for triangular correlation matrix.				
	
	if (imageSecondOrderStatsPtr->sumSquaresHandle != totalSumSquaresStatsHandle)		
		UnlockAndDispose (totalSumSquaresStatsHandle);
		
	else	// imageSecondOrderStatsPtr->sumSquaresHandle == totalSumSquaresStatsHandle
		CheckAndUnlockHandle (totalSumSquaresStatsHandle);
	
	totalSumSquaresStatsHandle = NULL;	
	totalSumSquaresStatsPtr = NULL;
		
	if (continueFlag)			
		continueFlag = SetupFeatureTransformationMemory (
													gClassifySpecsPtr->featureTransformationFlag, 
													numberFeatures, 
													numberFeatureChannels, 
													&gTransformationMatrix.eigenVectorPtr, 
													&gTransformationMatrix.tempMatrixPtr,
													NULL, 
													&gTransformationMatrix.eigenFeaturePtr,
													kLoadMatricesVectors,
													featurePtr);
								
	if (continueFlag && gClassifySpecsPtr->featureTransformationFlag)
		{                                                 
				// Get TCor matrix (Transformed Correlation Matrix)
				                 
		TransformSymmetricMatrix (gTransformationMatrix.eigenVectorPtr,
											inverseCorPtr, 
											correlationPtr, 
											inverseCorPtr, 
											numberFeatures,
											numberFeatureChannels,
											kSquareOutputMatrix);
			
		}	// end "if (continueFlag && ..."
		
	if (continueFlag)
		{										
				// Get the inverse of the correlation matrix
				//		InvCor or InvTCor				
					
		determinantOKFlag = InvertSymmetricMatrix (inverseCorPtr, 
																	numberFeatures, 
																	gInverseMatrixMemory.pivotPtr, 
																	gInverseMatrixMemory.indexRowPtr, 
																	gInverseMatrixMemory.indexColPtr, 
																	gInverseMatrixMemory.ipvotPtr, 
																	&determinant,
																	&logDeterminant,
																	kReturnMatrixInverse); 
			
		if (determinantOKFlag)
			{			
					// This transformation will take care of transforming the mean
					// and data vector if the data is to be transformed.
						
			if (gClassifySpecsPtr->featureTransformationFlag)
				{
						// Get Tt_InvTCov_T
						
				MatrixMultiply (gTransformationMatrix.eigenVectorPtr, 
										inverseCorPtr, 
										gTransformationMatrix.tempMatrixPtr, 
										numberFeatureChannels,
										numberFeatures,
										numberFeatures,
										kTransposeMat1ByMat2);
										
				MatrixMultiply (gTransformationMatrix.tempMatrixPtr,
										gTransformationMatrix.eigenVectorPtr, 
										inverseCorPtr, 
										numberFeatureChannels,
										numberFeatures,
										numberFeatureChannels,
										kMat1ByMat2);
										
				}	// end "if (gClassifySpecsPtr->featureTransformationFlag)"
				
					// Loop through the classes and for classes to be used in 			
					// classification get the:	
					//		CEM operator vector for each class										
			
			clsfyCEMOperatorStart = 0;
			
			statFeaturePtr = (SInt16*)GetStatisticsFeatureVector (
												gClassifySpecsPtr->featureTransformationFlag, 
												featurePtr);
											
			tempChanMeanPtr = gInverseMatrixMemory.pivotPtr;
			
			for (classIndex=0; 
					classIndex<gClassifySpecsPtr->numberClasses; 
					classIndex++)
				{
				statClassNumber = gClassifySpecsPtr->classPtr[classIndex] - 1;
					
						// Get the class mean vector
					
				GetClassMeanVector (numberFeatureChannels,
											classChannelStatsPtr, 
											tempChanMeanPtr,
											(UInt16*)statFeaturePtr, 
											statClassNumber);
				
						// Now get Mt_InvCor or Mt_Tt_InvTCor_T
						
				MatrixMultiply (tempChanMeanPtr,
										inverseCorPtr,  
										&classifyChanMeanPtr[clsfyCEMOperatorStart],
										1,
										numberFeatureChannels, 
										numberFeatureChannels,
										kMat1ByMat2);	
						
						// Now get Mt_InvCor_M or Mt_Tt_InvTCor_T_M
				
				double constant;							
				MatrixMultiply (&classifyChanMeanPtr[clsfyCEMOperatorStart],
										tempChanMeanPtr, 
										&constant, 
										1,
										numberFeatureChannels,
										1,
										kMat1ByMat2);
											
						// Now get the CEM operator 
						//		Mt_InvCor / Mt_InvCor_M or
						//		Mt_Tt_InvTCor_T / Mt_Tt_InvTCor_T_M
				
				UInt32 		i;
				for (i=0; i<numberFeatureChannels; i++)
					{
					classifyChanMeanPtr[clsfyCEMOperatorStart] /= constant;
					clsfyCEMOperatorStart++;
					
					}	// end "for (i=0; i<numberFeatureChannels; i++)"
				
				}	// end "for (class=0; class<Classify..." 
							
			}	// end "if (determinantOKFlag)"
			
		}	// end "if (continueFlag)" 
			
			// Release memory for the matrix inversion work areas.					
		
	ReleaseMatrixInversionMemory ();
	inverseCorPtr = NULL;
	
			// Release memory for the classify channels statistics.					
			
	classChannelStatsPtr = (HChannelStatisticsPtr)CheckAndDisposePtr (
																		(Ptr)classChannelStatsPtr);
	CheckAndDisposePtr (correlationPtr);
	
			// Release memory for transformation matrix information.					
			
	ReleaseFeatureTransformationMemory ();
	
	if (continueFlag)
		{
		
				// Classify the requested areas.	First load the maximum				
				// likelihood variable structure with need pointers					
		
		classifierVar.chanMeanPtr = classifyChanMeanPtr;
		
		if (determinantOKFlag)
			ClassifyAreasControl (fileInfoPtr, &classifierVar);
				
		else	// !determinantOKFlag 
			{
					// List message indicating that classification was cancelled	
					// because correlation matrix for the image could not be inverted.																
		
			continueFlag = ListSpecifiedStringNumber (kClassifyStrID, 
																	IDS_Classify4, 
																	(unsigned char*)gTextString, 
																	resultsFileStreamPtr, 
																	gOutputForce1Code,
																	continueFlag);	
															
			SysBeep (10);
			
			}	// else !determinantOKFlag 
			
		}	// end "if (continueFlag)" 
			
			// Release memory for class mean vectors, class covariance				
			// matrices, matrix inversion work areas, and class constants.										
			
	CheckAndDisposePtr (classifyChanMeanPtr);

}	// end "CEMClsfierControl" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 CEMClassifier
//
//	Software purpose:	The purpose of this routine is to classify the
//							input line of data using the constrained energy minimization 
//							(CEM) classifier.
//
//	Parameters in:		Pointer to area description structure
//							Pointer to image file information structure
//							Pointer to temporary classification variable structure
//							Pointer to buffer to be used to store the classification
//								results in for a line.
//							Pointer to a buffer to be used to store the probability
//								of correct classification information in for a line.
//							Pointer to a vector to be used to store the count of the
//								number of pixels classified into each class
//							A structure which defines the start line and column values
//								that the classification will begin at.	
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			ClassifyPerPointArea
//
//	Coded By:			Larry L. Biehl			Date: 08/21/1997
//	Revised By:			Larry L. Biehl			Date: 03/27/2012

SInt16 CEMClassifier (
				AreaDescriptionPtr				areaDescriptionPtr, 
				FileInfoPtr							fileInfoPtr,
				ClassifierVarPtr					clsfyVariablePtr, 
				HUCharPtr							outputBuffer1Ptr, 
				HUCharPtr							probabilityBufferPtr, 
				HSInt64Ptr							countVectorPtr, 
				Point									point)

{
	double								closestValue,
											difference,
											discriminantClosest,
											dValue;
												
	RgnHandle							rgnHandle;
	
	SInt16*	 							classPtr;
												            
	HDoublePtr 							chanMeanPtr,
											ioBufferReal8Ptr,
											savedBufferReal8Ptr;
												 
	UInt32								classIndex,
											closestClass,
											feature,
											numberChannels,
											numberClasses,
											numberSamplesPerChan,
											sameDistanceCount,
											sample;
												
	Boolean								polygonField,
											createCEMValuesFlag;
	
	
			// Initialize local variables.														
	
	numberChannels = 			gClassifySpecsPtr->numberChannels;
	numberClasses = 			gClassifySpecsPtr->numberClasses;
	numberSamplesPerChan = 	(UInt32)areaDescriptionPtr->numSamplesPerChan;
	polygonField =				areaDescriptionPtr->polygonFieldFlag;
	rgnHandle = 				areaDescriptionPtr->rgnHandle;
	classPtr = 					gClassifySpecsPtr->classPtr;
	savedBufferReal8Ptr = 	(HDoublePtr)outputBuffer1Ptr;
	closestClass = 			0;
	
	createCEMValuesFlag = 	gClassifySpecsPtr->createThresholdTableFlag;
		
			// Loop through the number of samples in the line of data				
	
	for (sample=0; sample<numberSamplesPerChan; sample++)
		{
		if (!polygonField || PtInRgn (point, rgnHandle))
			{	
					// Loop through the classes and get the discriminant value for	
					// each class.																	
		
			discriminantClosest = 1e200;
			chanMeanPtr = clsfyVariablePtr->chanMeanPtr;
			
			for (classIndex=0; classIndex<numberClasses; classIndex++)
				{
						// Loop through the channels (features) and get the transpose
						// of the CEM operator times the data vector.		
						
				ioBufferReal8Ptr = savedBufferReal8Ptr;
				dValue = 0;
				for (feature=0; feature<numberChannels; feature++)
					{
					dValue += *ioBufferReal8Ptr * *chanMeanPtr;
					
					ioBufferReal8Ptr++;
					chanMeanPtr++;
					
					}	// end "for (feature=0; feature<..." 
				
						// Check if discriminant value for this class is the closest to 1.  
						// If so, save the value and the class number.
				
				difference = fabs (dValue - 1.);
						
				if (difference < discriminantClosest)
					{
					discriminantClosest = difference;
					closestClass = classIndex;
					closestValue = dValue;
					sameDistanceCount = 0;
					
					}	// end "if (dValue < discriminantClosest)"
					
				else if (difference == discriminantClosest)
					sameDistanceCount++;
	
				}	// end "for (classIndex=0; classIndex<..."
				
			classIndex = classPtr[closestClass];
		   countVectorPtr[classIndex]++;
		   *outputBuffer1Ptr = (UInt8)classIndex;
		   
		   if (createCEMValuesFlag)
		   	{
		   	if (closestValue < 0)
		   		*probabilityBufferPtr = 1;
		   		
		   	else if (closestValue == 1)
		   		*probabilityBufferPtr = 101;
		   	
		   	else if (closestValue > 2)
		   		*probabilityBufferPtr = 102;
		   	
		   	else	// 0 >= discriminantMax	< 1.0
		   		*probabilityBufferPtr = (UInt8)(closestValue*50 + 2);
					
				probabilityBufferPtr++;
				
				gTempDoubleVariable1 += closestValue;
		   	
		   	}	// end "if (createCEMValuesFlag)"
		   
		   }	// end "if (!polygonField || PtInRgn (point, rgnHandle))" 
		   
		else	// polygonField && !PtInRgn (point, rgnHandle)
			{ 
			*outputBuffer1Ptr = 0; 
			
		   if (createCEMValuesFlag)
		   	{
				*probabilityBufferPtr = 0;
				probabilityBufferPtr++;
				
				}	// end "if (createCEMValuesFlag)"
				
			sameDistanceCount = 0;
				
			}	// end "else polygonField && !PtInRgn (point, rgnHandle)" 
			
		point.h++;
		savedBufferReal8Ptr += numberChannels;
		outputBuffer1Ptr++;
		
		clsfyVariablePtr->totalSameDistanceSamples += sameDistanceCount;
			
				// Exit routine if user selects "cancel" or "command period".		
		
		if (TickCount () >= gNextTime)
			{
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
																								return (3);
				
			}	// end "if (TickCount () >= gNextTime)" 
				
		}	// end "for (sample=0; sample<..." 
	
			// Write carriage return and null character to classification record	
			
   *outputBuffer1Ptr = kCarriageReturn;
   outputBuffer1Ptr++;  
                      
	#if defined multispec_win  
   	*outputBuffer1Ptr = kLineFeed;
  		outputBuffer1Ptr++; 
	#endif	// defined multispec_win  
	
   *outputBuffer1Ptr = kNullTerminator;
   
   return (0);

}	// end "CEMClassifier"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CheckAndUnlockCEMParameterHandles
//
//	Software purpose:	The purpose of this routine is to check if there is
// 						enough memory to list the classification results in 
//							the output text window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 08/29/1997
//	Revised By:			Larry L. Biehl			Date: 08/29/1997

void CheckAndUnlockCEMParameterHandles (void)

{                                              	
	CEMParametersPtr cemParametersPtr = (CEMParametersPtr)GetHandlePointer (
																gClassifySpecsPtr->cemParametersH);
															
	if (cemParametersPtr != NULL)				
		CheckAndUnlockHandle (cemParametersPtr->correlationMatrixClassHandle);
	
}	// end "CheckAndUnlockCEMParameterHandles" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CheckClassifyTextWindowSpaceNeeded
//
//	Software purpose:	The purpose of this routine is to check if there is
// 						enough memory to list the classification results in 
//							the output text window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	True: there is enough memory available.
//							False: there probably is not enough memory available.
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 03/29/1990
//	Revised By:			Larry L. Biehl			Date: 07/26/2005

Boolean CheckClassifyTextWindowSpaceNeeded (void)

{
	double								floatNumberBytesNeeded;
	
	UInt32								numberBytesNeeded,
											numberColumns,
											numberLines,
											numberTextLines;
	
	Boolean								returnFlag;
	
			// Initialize local variables.													
			
	numberLines = 0;
	
			// Get the bytes needed for the classify header and descriptions.		
			
	floatNumberBytesNeeded = 1000 + (gClassifySpecsPtr->numberClasses + 
											gClassifySpecsPtr->numberChannels) * 30;	
	numberTextLines = 10 + gClassifySpecsPtr->numberClasses + 
														gClassifySpecsPtr->numberChannels;	
	
	if (gClassifySpecsPtr->trainingFldsResubstitutionFlag)
		{
				// This is a very gross estimate.											
				
		floatNumberBytesNeeded += gClassifySpecsPtr->numberClassAreas * (7 * 7 + 5);
		numberTextLines += gClassifySpecsPtr->numberClassAreas * (7 + 5);
		
		}	// end "if (gClassifySpecsPtr->trainingFldsResubstitution)" 	
	
	if (gClassifySpecsPtr->trainingFldsLOOFlag)
		{
				// This is a very gross estimate.											
				
		floatNumberBytesNeeded += gClassifySpecsPtr->numberClassAreas * (7 * 7 + 5);
		numberTextLines += gClassifySpecsPtr->numberClassAreas * (7 + 5);
		
		}	// end "if (gClassifySpecsPtr->trainingFldsLOO)"  
	
	if (gClassifySpecsPtr->testFldsFlag)
		{
		floatNumberBytesNeeded += gClassifySpecsPtr->numberClassAreas * (7 * 7 + 5);
		numberTextLines += 5 + gClassifySpecsPtr->numberClassAreas * (7 + 5);
		
		}	// end "if (gClassifySpecsPtr->testFlds)"
	
	if (gClassifySpecsPtr->imageAreaFlag)
		{
				// Get the bytes needed for the image description, image file and	
				// the performance table.														
				
		floatNumberBytesNeeded += 200;		
		
		numberLines = (gClassifySpecsPtr->imageLineEnd -
									gClassifySpecsPtr->imageLineStart +
											gClassifySpecsPtr->imageLineInterval)/
													gClassifySpecsPtr->imageLineInterval;
			
		numberColumns = (gClassifySpecsPtr->imageColumnEnd -
									gClassifySpecsPtr->imageColumnStart +
											gClassifySpecsPtr->imageColumnInterval)/
													gClassifySpecsPtr->imageColumnInterval;
													
		floatNumberBytesNeeded += (double)numberLines * (numberColumns + 1);
				
		floatNumberBytesNeeded += gClassifySpecsPtr->numberClasses * 10;
													
		}	// end "if (gClassifySpecsPtr->imageArea)" 
		
			// Allow space for the structure overhead for formatting the 			
			// text.  This is on a line basis of around 25 bytes per line.			
			// And also 6 bytes for control characters such as tab and				
			// return.																				
												
	floatNumberBytesNeeded += (numberLines + numberTextLines + 5) * 10;	// For Waste.
	
	floatNumberBytesNeeded += (numberLines + numberTextLines + 5) * 6;
		
			// Allow for status dialog box.
			
	if (floatNumberBytesNeeded < UInt32_MAX-gMinMemoryForDrawingDialog)
		numberBytesNeeded = (UInt32)floatNumberBytesNeeded + gMinMemoryForDrawingDialog;
	else	// floatNumberBytesNeeded >= UInt32_MAX-gMinMemoryForDrawingDialog
		numberBytesNeeded = UInt32_MAX;
			
	returnFlag = CheckTextWindowSpaceNeeded (numberBytesNeeded);
		
	return (returnFlag);

}	// end "CheckClassifyTextWindowSpaceNeeded" 




//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ClassifyArea
//
//	Software purpose:	The purpose of this routine is to classify the
//							area specified, store the classified data to a disk 
//							file and write the classified data to a color
//							window for Mac II systems.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:			
// 
// Called By:			ClassifyAreasControl
//							ClassifyTrainTestFields
//
//	Coded By:			Larry L. Biehl			Date: 12/15/1988
//	Revised By:			Jeon						Date: ??/??/????
//	Revised By:			Larry L. Biehl			Date: 04/24/2019

SInt16 ClassifyArea (
				SInt16								classPointer, 
				AreaDescriptionPtr				areaDescriptionPtr, 
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				LCToWindowUnitsVariables* 		lcToWindowUnitsVariablesPtr,
				ClassifierVarPtr					clsfyVariablePtr, 
				HSInt64Ptr							countVectorPtr)
					
{		
	SInt16								returnCode;
	
	
			// Initialize gTempDoubleVariable1;  it is used in determining the	
			// average probability if it is to be calculated.							
			
	gTempDoubleVariable1 = 0.;		
							
	switch (gClassifySpecsPtr->mode)
		{
		case kMaxLikeMode:
		case kMahalanobisMode:	
		case kFisherMode:
      case kSupportVectorMachineMode:
      case kKNearestNeighborMode:
		case kEuclideanMode:
		case kCorrelationMode:
		case kCEMMode:
		case kParallelPipedMode:
			returnCode = ClassifyPerPointArea (classPointer,
															areaDescriptionPtr, 
															fileIOInstructionsPtr,
															lcToWindowUnitsVariablesPtr,
															clsfyVariablePtr, 
															countVectorPtr);
			break;
			
		case kEchoMode:						
			HideStatusDialogItemSet (kStatusMinutes);
			returnCode = EchoClassifier (classPointer,
													areaDescriptionPtr, 
													fileIOInstructionsPtr,
													lcToWindowUnitsVariablesPtr,
													clsfyVariablePtr,
													countVectorPtr);
			break;
			
		}	// end "switch (gClassifySpecsPtr->mode)" 

	return (returnCode);

}	// end "ClassifyArea" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ClassifyAreasControl
//
//	Software purpose:	The purpose of this routine is to control the
//							classification of the areas requested according to 
//							the selected classifier.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			MaxLikeClsfierControl
//
//	Coded By:			Larry L. Biehl			Date: 12/15/1988
//	Revised By:			Larry L. Biehl			Date: 04/15/2020

void ClassifyAreasControl (
				FileInfoPtr							fileInfoPtr, 
				ClassifierVarPtr					clsfyVariablePtr)
	
{
	LCToWindowUnitsVariables 		lcToWindowUnitsVariables;
	
	double								percent;
	
	CEMParametersPtr					cemParametersPtr;
	CMFileStream*						resultsFileStreamPtr;
	EchoClassifierVarPtr				echoClassifierVarPtr;
	FileIOInstructionsPtr			fileIOInstructionsPtr;
	HUInt16Ptr							ioBuffer2Ptr;
	
	Handle								overlayImageWindowInfoHandle,
											countVectorHandle = NULL;
	
	SInt64								ipixels,
											number_of_homogeneous_cells;
	
	SInt32								statClassNumber;
	
	UInt32								index;
	
	SInt16								cell_size,
											cell_width,
											numberChars,
											returnCode,				// =0, no error			
																		// =1, no memory for 	
																		// 		text.				
																		// =2, disk file error.	
																		// =3, user quit.
											strLength;
	
	Boolean								continueFlag;
	
		
			// Initialize local variables.													
			
	resultsFileStreamPtr = GetResultsFileStreamPtr (0);
	countVectorHandle = NULL;
	cemParametersPtr = NULL;
	echoClassifierVarPtr = NULL;
	returnCode = noErr;
	continueFlag = TRUE;
	
	if (gClassifySpecsPtr->mode == kEchoMode)
		{
		echoClassifierVarPtr = (EchoClassifierVarPtr)GetHandlePointer (
														gClassifySpecsPtr->echoClassifierVarH,
														kLock);
													
		if (echoClassifierVarPtr == NULL)
																								return;
													
		}	// end "if (gClassifySpecsPtr->mode == kEchoMode)"
	 
			// Initialize some of 'clsfyVariable' variables.							
			
	clsfyVariablePtr->countClassIndexPtr = NULL;
	clsfyVariablePtr->tableWidth = gProjectInfoPtr->numberStatisticsClasses + 1;
	clsfyVariablePtr->unitsCode = kSampleUnits;
	clsfyVariablePtr->summaryCode = kFieldSummary+kClassSummary;
	clsfyVariablePtr->tableType = 1;
	
	clsfyVariablePtr->thresholdFlag = gClassifySpecsPtr->thresholdFlag;
	if (gClassifySpecsPtr->mode == kParallelPipedMode)
				// It is possible for pixels for parallel piped classification to be
				// thresholded as a part of the classification algorithm. Pixels are
				// either in a "box" or they are not.
		clsfyVariablePtr->thresholdFlag = TRUE;
		
	clsfyVariablePtr->backgroundIndexOffset = 0;
	
	clsfyVariablePtr->useLeaveOneOutMethodFlag = FALSE;
			
			// Initialize the gAreaDescription structure in case it is used for
			// classifying the training and/or test fields. 

	InitializeAreaDescription (&gAreaDescription);
	/*	
			// Get the pointer to the channel list. The handle is already locked.
													
	SInt16* channelsPtr = (SInt16*)GetHandlePointer (
																gClassifySpecsPtr->channelsHandle);
	*/	
			// Get vector to use for count of the number of pixels classified		
			// into each class.  A vector of long int's will be use.					
	
	if (continueFlag)
		{
				// Get memory for the table to store he field totals and the agreement
				// matrix in.								
				
		countVectorHandle = GetCountVectorTableMemory (
									clsfyVariablePtr,
									gClassifySpecsPtr->trainingFldsResubstitutionFlag ||
															gClassifySpecsPtr->trainingFldsLOOFlag,
									gClassifySpecsPtr->testFldsFlag);
									
		continueFlag = (countVectorHandle != NULL);
			
		}	// end "if (continueFlag)" 
	
			// Get memory for a character buffer to be used in listing.  			
			// The table number widths will be set up by 								
			// 'GetListResultsStringBuffer'.	
			// 'GetListResultsStringBuffer' gets memory for gCharBufferPtr1; it
			// will need to be released at the end of this routine.											
	
	if (continueFlag)
		continueFlag = GetListResultsStringBuffer (
														fileInfoPtr, 
														clsfyVariablePtr,
														0x0001, 
														gProjectInfoPtr->numberStatisticsClasses, 
														gClassifySpecsPtr->classPtr, 
														gClassifySpecsPtr->numberClasses);	
	
			// Get buffer to read data from image file into.							
			// This buffer will be used for both the input data and the 			
			// classified output data.  First get number of bytes needed for		
			// input data and then make sure that that will be enough room for	
			// the classified data, including the probability map if requested.	
	
	if (continueFlag)
		continueFlag = GetClassifyDataBuffers (&gFileIOInstructions[0],
																gImageWindowInfoPtr,
																gImageLayerInfoPtr, 
																fileInfoPtr,
																echoClassifierVarPtr,
																&fileIOInstructionsPtr);
		
			// Get vector for storing the index into countVectorPtr at which		
			// the counts for a class start.													
	
	if (continueFlag)
		{		
		clsfyVariablePtr->countClassIndexPtr = (SInt32*)MNewPointer (
					(SInt32)gProjectInfoPtr->numberStatisticsClasses * sizeof (SInt32));
		continueFlag = (clsfyVariablePtr->countClassIndexPtr != NULL);
			
		}	// end "if (continueFlag)" 
		
	if (continueFlag)
		{		
				// List "  Output Information:".
				
		continueFlag = ListSpecifiedStringNumber (kSharedStrID, 
																IDS_Shared8, 
																(unsigned char*)gTextString, 
																resultsFileStreamPtr, 
																gOutputForce1Code, 
																continueFlag);
		
		if (continueFlag && gClassifySpecsPtr->mode == kEchoMode)
			{
			if (!echoClassifierVarPtr->fixed_homogeneityThreshold_option) 
				{	
				continueFlag = ListSpecifiedStringNumber (kClassifyStrID, 
																		IDS_Classify64, 
																		(unsigned char*)gTextString, 
																		resultsFileStreamPtr, 
																		gOutputForce1Code, 
																		continueFlag);
										
				for (index=0; index<gClassifySpecsPtr->numberClasses; index++)
					{
					statClassNumber = gClassifySpecsPtr->classPtr[index];
					
					if (echoClassifierVarPtr->thresholds_phase1[index] != -DBL_MAX)
						sprintf ((char*)gTextString,
										"      \tClass %d\t%.2f%s",
										(int)statClassNumber,
										echoClassifierVarPtr->thresholds_phase1[index],
										gEndOfLine);
								
					else	// echoClassifierVarPtr->thresholds_phase1[index] == -DBL_MAX
						sprintf ((char*)gTextString,
										"      \tClass %d:\tno thresholding%s",
										(int)statClassNumber,
										gEndOfLine);
								
					continueFlag = OutputString (resultsFileStreamPtr, 
													(char*)gTextString, 
													0, 
													gOutputForce1Code, 
													continueFlag);
							
					}	// end "for (index=0; index...)"	
		
						// Insert a blank line after the table.
				
				continueFlag = OutputString (resultsFileStreamPtr, 
														gEndOfLine, 
														gNumberOfEndOfLineCharacters, 
														gOutputForce1Code, 
														continueFlag);	
					
				}	// "end else !...fixed_homogeneityThreshold_option"
				
			}	// end "if (continueFlag && ...->mode == kEchoMode)"
		
		if (continueFlag)
			{	
					// Show status fields in status dialog that apply for training,
					// test and image classifications.										
				
			ShowStatusDialogItemSet (kStatusCommand);
			ShowStatusDialogItemSet (kStatusLine);
												
				// Show status fields in status dialog that apply for training,	
				// test field classification only.											
		
			if (gClassifySpecsPtr->trainingFldsResubstitutionFlag ||
						gClassifySpecsPtr->trainingFldsLOOFlag || 
								gClassifySpecsPtr->testFldsFlag)
				{
				ShowStatusDialogItemSet (kStatusClassA);
				ShowStatusDialogItemSet (kStatusField);
			
				}	// end "if (gClassifySpecsPtr->trainingFldsResubstitutionFlag ||...)" 
				
			}	// end "if (continueFlag)"
	
				// Set some variables for use when drawing image overlays.

		overlayImageWindowInfoHandle = gProjectInfoPtr->overlayImageWindowInfoHandle;
		SetLCToWindowUnitVariables (overlayImageWindowInfoHandle,
												kToImageWindow,
												FALSE,
												&lcToWindowUnitsVariables);
	
		if (gOutputCode & kCreateImageOverlayCode)
			{
					// Set the draw base image flag depending on whether the overlay
					// will cover the entire base image.
	
			ImageOverlayInfoPtr imageOverlayInfoPtr = GetImageOverlayInfoPtr (
															gClassifySpecsPtr->imageOverlayIndex,
															kNoLock,
															NULL);
			
			WindowInfoPtr imageWindowInfoPtr =
							(WindowInfoPtr)GetHandlePointer (overlayImageWindowInfoHandle);
			imageWindowInfoPtr->drawBaseImageFlag = FALSE;
			if (imageOverlayInfoPtr->drawBaseImageCode != 15)
				{
				WindowPtr windowPtr = GetWindowPtr (overlayImageWindowInfoHandle);
				imageWindowInfoPtr->drawBaseImageFlag = TRUE;
				InvalidateWindow (windowPtr, kImageFrameArea, FALSE);
				
				}	// end "if (imageOverlayInfoPtr->drawBaseImageCode != 15)"
			
			}	// if (if (gOutputCode & kCreateImageOverlayCode)
			
				// Classify the training fields (resubstitution) if requested								
			
		if (gClassifySpecsPtr->trainingFldsResubstitutionFlag && continueFlag)
			{
			LoadDItemStringNumber (kClassifyStrID, 
											IDS_Classify15,
											gStatusDialogPtr, 
											IDC_Status11, 
											(Str255*)&gTextString);
					
			if (gStatusDialogPtr)
				DrawDialog (gStatusDialogPtr);
		
			continueFlag = ListSpecifiedStringNumber (kClassifyStrID, 
																	IDS_Classify12, 
																	(unsigned char*)gTextString, 
																	resultsFileStreamPtr, 
																	gOutputForce1Code,
																	continueFlag);
										
			if (continueFlag)
				{
				clsfyVariablePtr->summaryCode = (gProjectInfoPtr->
								listResultsTrainingCode & (kFieldSummary+kClassSummary));
				
				if (gClassifySpecsPtr->mode != kSupportVectorMachineMode)
					clsfyVariablePtr->totalSameDistanceSamples = 0;
	
				returnCode = ClassifyTrainTestFields (&gAreaDescription, 
																	fileIOInstructionsPtr,
																	&lcToWindowUnitsVariables,
																	clsfyVariablePtr, 
																	kTrainingType);
															
				}	// end "if (continueFlag)"
														
			continueFlag = (returnCode != 1);
			
			if (continueFlag)
				continueFlag = ListNumberOfSameDistanceSamples (
													resultsFileStreamPtr,
													clsfyVariablePtr->totalSameDistanceSamples);
			
			}	// end "if (gClassifySpecsPtr->trainingFldsResubstitutionFlag)" 
			
				// Classify the training fields (leave-one-out) if requested								
			
		if (gClassifySpecsPtr->trainingFldsLOOFlag && (returnCode <= 1))
			{
			LoadDItemStringNumber (kClassifyStrID, 
											IDS_Classify47,
											gStatusDialogPtr, 
											IDC_Status11, 
											(Str255*)gTextString);
					
			if (gStatusDialogPtr)
				DrawDialog (gStatusDialogPtr);
		
			continueFlag = ListSpecifiedStringNumber (kClassifyStrID, 
																	IDS_Classify48, 
																	(unsigned char*)gTextString, 
																	resultsFileStreamPtr, 
																	gOutputForce1Code,
																	continueFlag);
										
			if (continueFlag)
				{
				clsfyVariablePtr->summaryCode = (gProjectInfoPtr->
								listResultsTrainingCode & (kFieldSummary+kClassSummary));
				
				if (gClassifySpecsPtr->mode != kSupportVectorMachineMode)
					clsfyVariablePtr->totalSameDistanceSamples = 0;
								
				clsfyVariablePtr->useLeaveOneOutMethodFlag = TRUE;
				
						// Intialize the nextTime variable to indicate when the next check	
						// should occur for a command-.
					
				gNextTime = TickCount ();
				gNextStatusTime = gNextTime;
	
				returnCode = ClassifyTrainTestFields (&gAreaDescription, 
																	fileIOInstructionsPtr,
																	&lcToWindowUnitsVariables,
																	clsfyVariablePtr, 
																	kTrainingType);
								
				clsfyVariablePtr->useLeaveOneOutMethodFlag = FALSE;
															
				}	// end "if (continueFlag)"
														
			continueFlag = (returnCode != 1);
			
			if (continueFlag)
				continueFlag = ListNumberOfSameDistanceSamples (
													resultsFileStreamPtr,
													clsfyVariablePtr->totalSameDistanceSamples);
			
			}	// end "if (gClassifySpecsPtr->trainingFldsLOOFlag && ..." 
					
				// Classify the test fields if requested									
			
		if (gClassifySpecsPtr->testFldsFlag && (returnCode <= 1))
			{
			LoadDItemStringNumber (kClassifyStrID, 
											IDS_Classify16,
											gStatusDialogPtr, 
											IDC_Status11, 
											(Str255*)&gTextString);
			if (gStatusDialogPtr)
				DrawDialog (gStatusDialogPtr);
		
			continueFlag = ListSpecifiedStringNumber (kClassifyStrID, 
																	IDS_Classify13, 
																	(unsigned char*)gTextString, 
																	resultsFileStreamPtr, 
																	gOutputForce1Code,
																	continueFlag);
		
					// List the testing fields/areas used.									
						
			if (continueFlag)							
				continueFlag = ListProjectFieldsUsed (fileInfoPtr, 
																	gClassifySpecsPtr->classPtr, 
																	gClassifySpecsPtr->numberClasses, 
																	NULL, 
																	kTestingType, 
																	resultsFileStreamPtr, 
																	&gOutputForce1Code,
																	FALSE);
			
			if (continueFlag)
				{
				clsfyVariablePtr->summaryCode = (gProjectInfoPtr->
								listResultsTestCode & (kFieldSummary+kClassSummary));
				
				if (gClassifySpecsPtr->mode != kSupportVectorMachineMode)
					clsfyVariablePtr->totalSameDistanceSamples = 0;
								
						// Intialize the nextTime variable to indicate when the next check	
						// should occur for a command-.
					
				gNextTime = TickCount ();
				gNextStatusTime = gNextTime;
								
				returnCode = ClassifyTrainTestFields (&gAreaDescription, 
																	fileIOInstructionsPtr,
																	&lcToWindowUnitsVariables,
																	clsfyVariablePtr, 
																	kTestingType);
															
				}	// end "if (continueFlag)"
			
			continueFlag = (returnCode != 1);
			
			if (continueFlag)
				continueFlag = ListNumberOfSameDistanceSamples (
													resultsFileStreamPtr,
													clsfyVariablePtr->totalSameDistanceSamples);
			
			}	// end "if (gClassifySpecsPtr->testingFlds && ...)" 
		
				// Classify the image area selection if requested						
				
		if (gClassifySpecsPtr->imageAreaFlag && (returnCode <= 1))
			{
			LoadDItemStringNumber (kClassifyStrID, 
											IDS_Classify17,
											gStatusDialogPtr, 
											IDC_Status11, 
											(Str255*)&gTextString);
											
			HideStatusDialogItemSet (kStatusClassA);
			HideStatusDialogItemSet (kStatusField);
			ShowStatusDialogItemSet (kStatusMinutes);
			if (gStatusDialogPtr)
				DrawDialog (gStatusDialogPtr);
				
			InitializeAreaDescription (&gAreaDescription, 
													gClassifySpecsPtr->imageLineStart, 
													gClassifySpecsPtr->imageLineEnd, 
													gClassifySpecsPtr->imageColumnStart, 
													gClassifySpecsPtr->imageColumnEnd, 
													gClassifySpecsPtr->imageLineInterval, 
													gClassifySpecsPtr->imageColumnInterval,
													1,
													1,
													0);
													
					// Clear the memory for the counts.										
			 
			for (index=0; index<=gProjectInfoPtr->numberStatisticsClasses; index++)
				clsfyVariablePtr->countVectorPtr[index] = 0;
				
			CMFileStream* resultsFileStreamPtr = GetResultsFileStreamPtr (0);
		
			if (resultsFileStreamPtr == NULL)
						// List "Area classification not saved to disk"
				continueFlag = ListSpecifiedStringNumber (kClassifyStrID, 
																		IDS_Classify62,
																		(unsigned char*)gTextString, 
																		resultsFileStreamPtr, 
																		gOutputForce1Code,
																		continueFlag);
										
			else	// resultsFileStreamPtr != NULL
				{
						// List "Area classification was saved to disk"
				FileStringPtr filePathPtr =
							(FileStringPtr)GetFilePathPPointerFromFileStream (resultsFileStreamPtr);
				continueFlag = ListSpecifiedStringNumber (kClassifyStrID, 
																		IDS_Classify61, 
																		resultsFileStreamPtr, 
																		gOutputForce1Code,
																		(char*)&filePathPtr[2],
																		continueFlag,
																		kUTF8CharString);
				
				}	// end "else resultsFileStreamPtr != NULL"
			
					// List information on area probability map if it was saved to disk
			
			CMFileStream* probabilityFileStreamPtr = GetResultsFileStreamPtr (1);
			if (probabilityFileStreamPtr != NULL)
				{
				FileStringPtr filePathPtr =
						(FileStringPtr)GetFilePathPPointerFromFileStream (probabilityFileStreamPtr);
				continueFlag = ListSpecifiedStringNumber (kClassifyStrID,
																		IDS_Classify98,
																		resultsFileStreamPtr,
																		gOutputForce1Code,
																		(char*)&filePathPtr[2],
																		continueFlag,
																		kUTF8CharString);
				
				}	// end "else resultsFileStreamPtr != NULL"
		
			continueFlag = ListSpecifiedStringNumber (kClassifyStrID, 
																	IDS_Classify14, 
																	(unsigned char*)gTextString, 
																	resultsFileStreamPtr, 
																	gOutputForce1Code,
																	continueFlag);
										
					// Insert 6 space to line before the line column string.
			
			sprintf ((char*)gTextString, "    ");
			continueFlag = OutputString (resultsFileStreamPtr, 
													(char*)gTextString, 
													0, 
													gOutputForce1Code,
													continueFlag);
	
					// List the line and column number of area being classified.
					// "  Lines %ld to %ld by %ld.  Columns %ld to %ld by %ld"
					
			continueFlag = ListLineColumnIntervalString (
																	resultsFileStreamPtr, 
																	gOutputForce1Code,
																	gAreaDescription.lineStart, 
																	gAreaDescription.lineEnd, 
																	gAreaDescription.lineInterval,
																	gAreaDescription.columnStart, 
																	gAreaDescription.columnEnd, 
																	gAreaDescription.columnInterval, 
																	continueFlag);
								
			if (continueFlag && (gOutputFormatCode == kGAIAType))
				{
						// Now initialize the fixed output data for GAIA formatted 		
						// files. Do for both buffers if threaded IO may be use.																		
				
				ioBuffer2Ptr = (HUInt16Ptr)fileIOInstructionsPtr->outputBufferPtrs[0];
				ioBuffer2Ptr = &ioBuffer2Ptr[gClassifySpecsPtr->outputBufferOffset];
				
				InitializeGAIALineBytes (ioBuffer2Ptr, 
													(SInt32)gAreaDescription.numSamplesPerChan,
													1);
				
				if (fileIOInstructionsPtr->bufferOffset > 0)																		
					{
					ioBuffer2Ptr = (HUInt16Ptr)fileIOInstructionsPtr->outputBufferPtrs[1];
					ioBuffer2Ptr = &ioBuffer2Ptr[gClassifySpecsPtr->outputBufferOffset];
					
					InitializeGAIALineBytes (ioBuffer2Ptr, 
														(SInt32)gAreaDescription.numSamplesPerChan,
														1);
														
					}	// end "if (fileIOInstructionsPtr->bufferOffset > 0)"
					
				}	// end "if (continueFlag && (gOutputFormatCode..." 
			
			if (continueFlag)
				{
				if (gClassifySpecsPtr->mode != kSupportVectorMachineMode)
					clsfyVariablePtr->totalSameDistanceSamples = 0;
				
						// Intialize the nextTime variable to indicate when the next check	
						// should occur for a command-.
					
				gNextTime = TickCount ();
				gNextStatusTime = gNextTime;
		
				returnCode = ClassifyArea (-1, 
													&gAreaDescription, 
													fileIOInstructionsPtr,
													&lcToWindowUnitsVariables,
													clsfyVariablePtr, 
													&clsfyVariablePtr->countVectorPtr[0]);
				continueFlag = (returnCode != 1);
				
				}	// end "if (continueFlag)" 
								
			if (continueFlag && gClassifySpecsPtr->mode == kEchoMode)
				{
				ipixels 		= echoClassifierVarPtr->ipixels;
				cell_width 	= echoClassifierVarPtr->cell_width;
				cell_size 	= echoClassifierVarPtr->cell_size;
				number_of_homogeneous_cells =
												echoClassifierVarPtr->number_of_homogeneous_cells;
					
				percent = (double)number_of_homogeneous_cells * cell_size * 100 / ipixels;
				
						// List homogeneous cell information
						
				strLength = sprintf ((char*)gTextString, 
											"%s    ",
											gEndOfLine);
				
				strLength = ListCountValue ((char*)gTextString,
														number_of_homogeneous_cells,
														-1,
														strLength,
														-1,
														kDoNotIncludeTab);
				
				numberChars = sprintf ((char*)&gTextString[strLength], 
												" homogeneous cells (%hd by %hd) ",
												cell_width,
												cell_width);
				strLength += numberChars;
				
				strLength = ListCountValue ((char*)gTextString,
														number_of_homogeneous_cells * cell_size,
														-1,
														strLength,
														-1,
														kDoNotIncludeTab);
				
				numberChars = sprintf ((char*)&gTextString[strLength], " / ");
				strLength += numberChars;
				
				strLength = ListCountValue ((char*)gTextString,
														ipixels,
														-1,
														strLength,
														-1,
														kDoNotIncludeTab);
				
				numberChars = sprintf ((char*)&gTextString[strLength], 
												" pixels (%.2f%%)%s",
												percent,
												gEndOfLine);
				strLength += numberChars;

				continueFlag = OutputString (resultsFileStreamPtr,
														(char*)gTextString, 
														strLength, 
														gOutputForce1Code,
														continueFlag);
				
						// List homogeneous field information
				
				percent = 0;
				
				if (echoClassifierVarPtr->number_of_fields > 0) 
					percent = (double)number_of_homogeneous_cells * cell_width *
										cell_width / echoClassifierVarPtr->number_of_fields;
									
				strLength = sprintf ((char*)gTextString, "    ");
				
				strLength = ListCountValue (
										(char*)gTextString,
										(SInt64)echoClassifierVarPtr->current_max_field_number,
										-1,
										strLength,
										-1,
										kDoNotIncludeTab);
				
				numberChars = sprintf ((char*)&gTextString[strLength], 
												" homogeneous fields. ");
				strLength += numberChars;
				
				strLength = ListCountValue (
												(char*)gTextString,
												(SInt64)echoClassifierVarPtr->number_of_fields,
												-1,
												strLength,
												-1,
												kDoNotIncludeTab);
				
				numberChars = sprintf ((char*)&gTextString[strLength], 
												" homogeneous fields after merging. Average ");
				strLength += numberChars;
				
				strLength = ListCountValue ((char*)gTextString,
														(SInt64)percent,
														-1,
														strLength,
														-1,
														kDoNotIncludeTab);
				
				numberChars = sprintf ((char*)&gTextString[strLength], 
												" pixels per merged field.%s",
												gEndOfLine);
				strLength += numberChars;

				continueFlag = OutputString (resultsFileStreamPtr,
														(char*)gTextString, 
														strLength, 
														gOutputForce1Code,
														continueFlag);
				
						// List Homogeneous Fields image file name if one was created.
				
				FileInfoPtr homogeneousFieldsFileInfoPtr = GetResultsFilePtr (2);
				if (homogeneousFieldsFileInfoPtr != NULL)
					{
					FileStringPtr filePathPtr = (FileStringPtr)GetFilePathPPointerFromFileInfo (
																		homogeneousFieldsFileInfoPtr);
					
					continueFlag = ListSpecifiedStringNumber (kClassifyStrID,
																			IDS_Classify96,
																			resultsFileStreamPtr,
																			gOutputForce1Code,
																			(char*)&filePathPtr[2],
																			continueFlag,
																			kUTF8CharString);
					
					}	// end "homogeneousFieldsFileInfoPtr != NULL"
				
						// List Homogeneous Fields image file name if one was created.
				
				FileInfoPtr classifiedFieldsFileInfoPtr = GetResultsFilePtr (3);
				if (classifiedFieldsFileInfoPtr != NULL)
					{
					FileStringPtr filePathPtr = (FileStringPtr)GetFilePathPPointerFromFileInfo (
																		classifiedFieldsFileInfoPtr);
					
					continueFlag = ListSpecifiedStringNumber (kClassifyStrID,
																			IDS_Classify97,
																			resultsFileStreamPtr,
																			gOutputForce1Code,
																			(char*)&filePathPtr[2],
																			continueFlag,
																			kUTF8CharString);
					
					}	// end "classifiedFieldsFileInfoPtr != NULL"
				
				if (echoClassifierVarPtr->tempDiskFileWasUsedFlag)
					{
					strLength = sprintf (
								(char*)gTextString,
								"    Temporary disk file was used for the classification.%s%s",
								gEndOfLine,
								gEndOfLine);
					
					continueFlag = OutputString (resultsFileStreamPtr,
														  (char*)gTextString,
														  strLength,
														  gOutputForce1Code,
														  continueFlag);
					
					}	// end "if (echoClassifierVarPtr->tempDiskFileWasUsedFlag)"

				}	//	"if (continueFlag && ... == kEchoMode)"	
				
			HideStatusDialogItemSet (kStatusTitle2);
			HideStatusDialogItemSet (kStatusLine);
					
					// List table with class distribution.									
			
			if (continueFlag)									
				continueFlag = ListClassificationSummary (
																clsfyVariablePtr,
																resultsFileStreamPtr, 
																&gOutputForce1Code,
																gClassifySpecsPtr->classPtr,  
																gClassifySpecsPtr->numberClasses);
			
			if (continueFlag)
				continueFlag = ListNumberOfSameDistanceSamples (
														resultsFileStreamPtr,
														clsfyVariablePtr->totalSameDistanceSamples);
			
					// Write probability grouping to end of probability file if
					// one is being created.
									
			if (continueFlag)					
				WriteProbabilityGrouping (gClassifySpecsPtr->mode);
															
					// Create the trailer files if needed.							
			
			if (returnCode <= 1)
				continueFlag = CreateTrailerFiles (
													clsfyVariablePtr,
													gProjectInfoPtr->numberStatisticsClasses,
													gClassifySpecsPtr->classPtr,  
													(SInt16)gClassifySpecsPtr->numberClasses);
			
			}	// end "if (gClassifySpecsPtr->imageArea && ...)"
															
		}	// end "if (continueFlag)" 
				
			// Dispose of the IO buffer.		
								
	DisposeIOBufferPointers (fileIOInstructionsPtr, 
										&gOutputBufferPtr,
										&gInputBufferPtr);
	
	clsfyVariablePtr->countClassIndexPtr = CheckAndDisposePtr (
															clsfyVariablePtr->countClassIndexPtr);
		
	UnlockAndDispose (countVectorHandle);
	
	gCharBufferPtr1 = CheckAndDisposePtr (gCharBufferPtr1);
			
}	// end "ClassifyAreasControl"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		ClassifyControl
//
//	Software purpose:	The purpose of this routine is to control the user
//							requests for classification operations on an image 
//							file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			OnProcClassify in xMainFrame.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/06/1988
//	Revised By:			Larry L. Biehl			Date: 12/06/2019

void ClassifyControl (void)

{
	LongRect								boundingClassifyArea;
	
	time_t								startTime;
	                                    
	CMFileStream*						resultsFileStreamPtr; 
	FileInfoPtr							fileInfoPtr;
	SInt16*								classAreaPtr;
	
	SInt32								minimumNumberTrainPixels;
	
	Boolean								continueFlag = TRUE; 
			
	SInt16								fieldType,
											overlayIndex,
											targetImageInfoHandleStatus;
	
	
			// If spare memory had to be used to load code resources, then exit	
			// routine.																				
			
	if (gMemoryTypeNeeded < 0)
																									return;
		    		
			// Code resources loaded okay, so set flag back for non-Code			
			// resources.																			
			
	gMemoryTypeNeeded = 0;
	
			// Check handle to image file information.  If handle to image			
			// file information doesn't exists. Find the	image file and get		
			// the information for it.															
				
	if (GetProjectImageFileInfo (kPrompt, kSetupGlobalInfoPointers))
		{
		fileInfoPtr = gImageFileInfoPtr;
		
				// Set up classification specification structure						
				
		if (LoadClassifySpecs (fileInfoPtr)) 
			{
			if (ClassifyDialog (fileInfoPtr))
				{
						// Initialize some global variables pertaining to output.	
				
				gOutputTextOKFlag = TRUE;
				gOutputCode = gClassifySpecsPtr->outputStorageType;
				gOutputForce1Code = (gOutputCode | 0x0001);
				
						// Note that statistics are not needed for the Support Vector
						// Machine or the K-Nearest Neighbor classifiers. The training
						// data values are used.
				
				if (gClassifySpecsPtr->mode == kSupportVectorMachineMode ||
												gClassifySpecsPtr->mode == kKNearestNeighborMode)
					{
							// Update statistics for project if needed
					
					SInt16 savedStatisticsCode = gProjectInfoPtr->statisticsCode;
					gProjectInfoPtr->statisticsCode = kPixelValuesOnly;
						
					continueFlag = VerifyProjectTrainingPixelsLoaded (
															&gClassifySpecsPtr->numberClasses,
															gClassifySpecsPtr->classPtr,
															//2,
															1,
															kSetupGlobalInfoPointers,
															&minimumNumberTrainPixels);
					gProjectInfoPtr->statisticsCode = savedStatisticsCode;
					
							// Verify that the kNN value for k Nearest Neighbor Classifier
							// is less than the total number of training pixels being used.
					
					if (continueFlag && gClassifySpecsPtr->nearestNeighborKValue >
																			gProjectInfoPtr->knnCounter)
						gClassifySpecsPtr->nearestNeighborKValue =
																			gProjectInfoPtr->knnCounter;
					
					//if (continueFlag &&
					//		gClassifySpecsPtr->mode == kSupportVectorMachineMode &&
					//			!gClassifySpecsPtr->supportVectorMachineModelAvailableFlag)
					//	continueFlag = CreateSupportVectorMachineModel (gClassifySpecsPtr);
					
					}	// end "if (...->mode == kSupportVectorMachineMode || ..."
					
				else	// gClassifySpecsPtr->mode != kSupportVectorMachineMode && ...
					{
							// Statistics are needed for the classifier being used.
							// Update statistics for project if needed
						
					continueFlag = VerifyProjectStatsUpToDate (
															&gClassifySpecsPtr->numberClasses,
															gClassifySpecsPtr->classPtr,
															1,
															gProjectInfoPtr->statisticsCode,
															gProjectInfoPtr->covarianceStatsToUse, 
															kSetupGlobalInfoPointers,
															&minimumNumberTrainPixels);
					
					}	// end "else gClassifySpecsPtr->mode != kKNearestNeighborMode && ..."
				
						// Update fileInfoPtr in case it was changed.
					
				fileInfoPtr = gImageFileInfoPtr;
					
				if (continueFlag)
					{
							// If classification is to go to the output text window,	
							// check if there is enough memory for it.					
							
					if (gClassifySpecsPtr->outputStorageType & 0x0001)	
						continueFlag = CheckClassifyTextWindowSpaceNeeded ();
						
					}	// end "if (continueFlag)" 
				
				if (continueFlag)
					{			
							// Set up the pointers to the target file information.	
							
					if (gClassifySpecsPtr->targetWindowInfoHandle != 
																	gProjectInfoPtr->windowInfoHandle)
						{
						UnlockProjectWindowInfoHandles ();

						GetImageInformationPointers (
														&targetImageInfoHandleStatus,
														gClassifySpecsPtr->targetWindowInfoHandle,
														&gImageWindowInfoPtr, 
														&gImageLayerInfoPtr, 
														&gImageFileInfoPtr);
						
						fileInfoPtr = gImageFileInfoPtr;
						
						}	// end "if (...->targetWindowInfoHandle != ..." 
						
							// If classification is to go the a disk file, open the	
							// disk file.															
					
					if (gClassifySpecsPtr->outputStorageType & 
											(kResultsFilesCode+kEchoFieldsCode+kEchoClassesCode))
						{
						InitializeAreaDescription (&gAreaDescription,
															gClassifySpecsPtr->imageLineStart, 
															gClassifySpecsPtr->imageLineEnd, 
															gClassifySpecsPtr->imageColumnStart, 
															gClassifySpecsPtr->imageColumnEnd, 
															gClassifySpecsPtr->imageLineInterval, 
															gClassifySpecsPtr->imageColumnInterval,
															(SInt32)fileInfoPtr->startLine,
															(SInt32)fileInfoPtr->startColumn,
															gClassifySpecsPtr->diskFileFormat);
						
								// Create the ascii output file now if it is being requested.
															
						if (gClassifySpecsPtr->outputStorageType & kAsciiFormatCode)	
							continueFlag = CreateResultsDiskFiles (
											gClassifySpecsPtr->outputStorageType,
											(SInt16)gProjectInfoPtr->numberStatisticsClasses,
											(SInt16)gClassifySpecsPtr->numberProbabilityClasses);
												
						}	// end "if (...->outputStorageType & (..." 
												
 					resultsFileStreamPtr = GetResultsFileStreamPtr (0);
				
					if (gOutputCode & kCreateImageOverlayCode)
						{
								// Get memory needed for the overlay image.
								
						overlayIndex = -1;
						if (gClassifySpecsPtr->imageOverlayIndex >= 0 && 
								gClassifySpecsPtr->imageOverlayIndex < 
																(SInt16)gImageOverlayHandleListLength)
							overlayIndex = gClassifySpecsPtr->imageOverlayIndex;
						
								// Get the bounding area to be classified.
						
						classAreaPtr = (SInt16*)GetHandlePointer (
																gClassifySpecsPtr->classAreaHandle);
						
						fieldType = 0;	
						if (gClassifySpecsPtr->imageAreaFlag)
							fieldType += kAreaType;
							
						if (gClassifySpecsPtr->trainingFldsResubstitutionFlag ||
															gClassifySpecsPtr->trainingFldsLOOFlag)
							fieldType += kTrainingType;
							
						if (gClassifySpecsPtr->testFldsFlag)
							fieldType += kTestingType;
											
						if (GetProjectFieldsBoundingArea (
															fieldType, 
															classAreaPtr, 
															gClassifySpecsPtr->numberClassAreas, 
															gClassifySpecsPtr->imageLineStart,
															gClassifySpecsPtr->imageLineEnd,
															gClassifySpecsPtr->imageColumnStart,
															gClassifySpecsPtr->imageColumnEnd,
															&boundingClassifyArea) != noErr)
							continueFlag = FALSE;
						
						if (continueFlag)
							gClassifySpecsPtr->imageOverlayIndex = 
										SetUpImageOverlayInformation (
												gProjectInfoPtr->overlayImageWindowInfoHandle,
												overlayIndex,
												gProjectInfoPtr->numberStatisticsClasses,
												&boundingClassifyArea,
												gProjectInfoPtr->imagePalettePopupMenuSelection,
												0,
												fieldType,
												gClassifySpecsPtr->imageColumnInterval,
												gClassifySpecsPtr->imageLineInterval);
					
						if (!continueFlag || gClassifySpecsPtr->imageOverlayIndex == -1)
									// Not possible to create an image overlay.
							gOutputCode -= kCreateImageOverlayCode;
							
						if (gOutputCode == 0)
							continueFlag = FALSE;
						
						}	// end "if (gOutputCode & kCreateImageOverlayCode)"
					
					}	// end "if (continueFlag)" 
					
						// Get dialog box to display classification status.					
				
				if (continueFlag)
					gStatusDialogPtr = GetStatusDialog (kUpdateStatsInfoID, TRUE);
					
				continueFlag = (gStatusDialogPtr != NULL);
				
						// Make call relative to classifier specified if project 	
						// stats are up-to-date or if user requested that stats not	
						//	be brought up-to-date.												
				
				if (continueFlag)
					{
							// Change cursor to watch cursor until done with process	
						
					MSetCursor (kWait);
			
							// Force text selection to start from end of present text
						
					ForceTextToEnd ();
					
					continueFlag = ListClassifyInputParameters (fileInfoPtr,
																				resultsFileStreamPtr);
					
					if (continueFlag &&
							gClassifySpecsPtr->mode == kSupportVectorMachineMode &&
								!gClassifySpecsPtr->supportVectorMachineModelAvailableFlag)
						continueFlag = CreateSupportVectorMachineModel (gClassifySpecsPtr);
					
					Boolean checkNumberOfPixelsFlag = FALSE;
					if (gClassifySpecsPtr->mode == kMaxLikeMode || 
														gClassifySpecsPtr->mode == kMahalanobisMode)
						checkNumberOfPixelsFlag = TRUE;
						
					else if (gClassifySpecsPtr->mode == kEchoMode)
						{
						EchoClassifierVarPtr echoClassifierVariablePtr = 
										(EchoClassifierVarPtr)GetHandlePointer (
															gClassifySpecsPtr->echoClassifierVarH);
																
						if (echoClassifierVariablePtr->algorithmCode == kMaxLikeMode ||
								echoClassifierVariablePtr->algorithmCode == kMahalanobisMode)
							checkNumberOfPixelsFlag = TRUE;	
						
						}	// end "else if (gClassifySpecsPtr->mode == kEchoMode)"
						
					if (continueFlag && checkNumberOfPixelsFlag)	
						{		
						continueFlag = CheckNumberOfPixelsInClass (
																gClassifySpecsPtr->numberClasses,
																gClassifySpecsPtr->classPtr, 
																gClassifySpecsPtr->numberFeatures);
							
						if (!continueFlag)
							{
									// List message indicating that classification was 
									//	cancelled because correlation matrix for the image 
									// could not be inverted.
							
							ListSpecifiedStringNumber (kClassifyStrID, 
																IDS_Classify4, 
																(unsigned char*)gTextString, 
																resultsFileStreamPtr, 
																gOutputForce1Code,
																TRUE);				

							DisplayAlert (kErrorAlertID, 
												kStopAlert, 
												kAlertStrID, 
												IDS_Alert74,
												0, 
												NULL);
																
							}	// end "if (!continueFlag)"	
													
						}	// end "if (continueFlag && checkNumberOfPixelsFlag)"
				
					if (continueFlag && 
								minimumNumberTrainPixels < 3 && 
											gClassifySpecsPtr->trainingFldsLOOFlag)
						{
						gClassifySpecsPtr->trainingFldsLOOFlag = FALSE;
		
						continueFlag = ListSpecifiedStringNumber (
																		kClassifyStrID,
																		IDS_Classify63, 
																		(unsigned char*)gTextString, 
																		resultsFileStreamPtr, 
																		gOutputForce1Code,
																		continueFlag);
						
						if (!gClassifySpecsPtr->imageAreaFlag &&
								!gClassifySpecsPtr->trainingFldsResubstitutionFlag &&
									!gClassifySpecsPtr->testFldsFlag)
							continueFlag = FALSE;
						
						}	// end "if (continueFlag && minimumNumberTrainPixels < 3 && ..."
						
							// Create the output files now if they are being requested.
														
					if (continueFlag && 
								!(gClassifySpecsPtr->outputStorageType & kAsciiFormatCode)	)
						continueFlag = CreateResultsDiskFiles (
											gClassifySpecsPtr->outputStorageType,
											(SInt16)gProjectInfoPtr->numberStatisticsClasses,
											(SInt16)gClassifySpecsPtr->numberProbabilityClasses);
										
							// Initialize some global variables to allow the user to pause	
							// or cancel the classification.														
					
					if (continueFlag)
						InitializeGlobalAlertVariables (kContinueStopAlertID, 
																	kAlertStrID, 
																	IDS_Alert122);
											
					startTime = time (NULL);
					
					if (continueFlag)
						{
								// Turn spin cursor on
				
						gPresentCursor = kSpin;

						switch (gClassifySpecsPtr->mode)
							{	
							case kMaxLikeMode:		// Maximum likelihood 	
							case kMahalanobisMode:		// Mahalanobis Classifer
								MaxLikeClsfierControl (fileInfoPtr);
								break;
								
							case kFisherMode:		// Fisher discriminant
								FisherClsfierControl (fileInfoPtr);
								break;
								
							case kEchoMode:			// Echo
	      					EchoClsfierControl (fileInfoPtr);
								break;
							
							case kSupportVectorMachineMode:	// Support Vector Machine
								SupportVectorMachineClassifierControl (fileInfoPtr);
								break;
							
                     case kKNearestNeighborMode:
                        KNearestNeighborClassifierControl (fileInfoPtr);
                        break;
								
							case kEuclideanMode:		// Euclidean minimum distance 
								EuclideanClsfierControl (fileInfoPtr);
								break;
								
							case kCorrelationMode:	// Correlation classifier
								CorrelationClsfierControl (fileInfoPtr);
								break;
								
							case kCEMMode:	// CEM classifier
								CEMClsfierControl (fileInfoPtr);
								break;
								
							case kParallelPipedMode:	// Parallel Piped classifier
								ParallelPipedClsfierControl (fileInfoPtr);
								break;
								
							}	// end "switch (classifyMode)" 
												
								// Reset the cancel operation globals.	
						
						ClearGlobalAlertVariables ();
							
						if (gOutputCode & kCreateImageOverlayCode)
							UpdateOverlayControl (
								GetWindowPtr (gProjectInfoPtr->overlayImageWindowInfoHandle));
							
								// Set drawBaseImageFlag to proper setting for general updates.
						
						Handle activeImageWindowInfoHandle =
														FindProjectBaseImageWindowInfoHandle ();
						WindowInfoPtr imageWindowInfoPtr =
								(WindowInfoPtr)GetHandlePointer (activeImageWindowInfoHandle);
						if (imageWindowInfoPtr != NULL)
							imageWindowInfoPtr->drawBaseImageFlag = GetDrawBaseImageFlag (
																gClassifySpecsPtr->imageOverlayIndex);
	
								// Make sure that any image overlay handle is unlocked.
								
						UnlockImageOverlayInfoHandle (gClassifySpecsPtr->imageOverlayIndex,
																0);

								// Turn spin cursor off
					
						gPresentCursor = kWait; 
		
						continueFlag = ListSpecifiedStringNumber (
																kClassifyStrID,
																IDS_Classify30+gClassifySpecsPtr->mode, 
																(unsigned char*)gTextString, 
																resultsFileStreamPtr, 
																gOutputForce1Code,
																continueFlag);
						
						}	// end "if (continueFlag)" 
						
							// List message if memory was not available to complete the
							// process
							
					ListMemoryMessage (resultsFileStreamPtr);
						
							// List the CPU time taken for the classification.		
						
					ListCPUTimeInformation (resultsFileStreamPtr,
													TRUE, 
													startTime);
			
							// Scroll output window to the selection and adjust the 	
							// scroll bar.															
					
					UpdateOutputWScrolls (gOutputWindow, 1, kDisplayMessage);
						
					}	// end "if (continueFlag)" 
					
				if (gClassifySpecsPtr->targetWindowInfoHandle != 
																	gProjectInfoPtr->windowInfoHandle)
					{
					UnlockImageInformationHandles (targetImageInfoHandleStatus, 
									gClassifySpecsPtr->targetWindowInfoHandle);
					gImageWindowInfoPtr = NULL; 
					gImageLayerInfoPtr = NULL; 
					gImageFileInfoPtr = NULL;
					
					}	// end "if (...->targetWindowInfoHandle != ..." 
					
						// Close the classification results files if needed and		
						// release the memory assigned to it/them.						
						
				CloseResultsFiles ();
				
						// Dispose of classification status dialog box.					
				
				CloseStatusDialog (TRUE);
					
						// Change cursor back to pointer.									
							
				MInitCursor ();
				
				}	// end "if (ClassifyDialog ())" 
						
					// This window information handle is not valid any more.
			
			gProjectInfoPtr->overlayImageWindowInfoHandle = NULL;
				
			}	// end "if (LoadClassifySpecs ())" 
			
					// Unlock the memory for the classification specifications.		
		
		if (gClassifySpecsPtr != NULL)
			{
			CheckAndUnlockHandle (gClassifySpecsPtr->featureHandle);
			
			CheckAndUnlockHandle (gClassifySpecsPtr->channelsHandle);
			
			CheckAndUnlockHandle (gClassifySpecsPtr->classAreaHandle);
			
			CheckAndUnlockHandle (gClassifySpecsPtr->classHandle);
			gClassifySpecsPtr->classPtr = NULL;
			gClassifySpecsPtr->classVectorPtr = NULL;
			
			CheckAndUnlockHandle (gClassifySpecsPtr->symbolsHandle);
			gClassifySpecsPtr->symbolsPtr = NULL;
			
			CheckAndUnlockHandle (gClassifySpecsPtr->thresholdTableHandle);
			gClassifySpecsPtr->thresholdTablePtr = NULL;
			
			CheckAndUnlockHandle (gClassifySpecsPtr->thresholdProbabilityHandle);
			gClassifySpecsPtr->thresholdProbabilityPtr = NULL;
			
			CheckAndUnlockCEMParameterHandles ();
			
			CheckAndUnlockHandle (gClassifySpecsPtr->cemParametersH);
			CheckAndUnlockHandle (gClassifySpecsPtr->decisionTreeVarH);
			CheckAndUnlockHandle (gClassifySpecsPtr->echoClassifierVarH);
			
			}	// end "if (gClassifySpecsPtr != NULL)" 
					
		CheckAndUnlockHandle (gProjectInfoPtr->classifySpecsH);
		gClassifySpecsPtr = NULL;
		
		UnlockProjectWindowInfoHandles ();
		
		gOperationCanceledFlag = FALSE;
		
		}	// end "if (GetProjectImageFileInfo (TRUE, TRUE))" 

}	// end "ClassifyControl" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ClassifyPerPointArea
//
//	Software purpose:	The purpose of this routine is to classify the
//							area specified, store the classified data to a disk 
//							file and write the classified data to a color
//							window for Mac II systems.
//
//	Parameters in:		Class number for which the input area belongs to or =0
//								if the input area is a selected area to be classified.
//							Pointer to the area description structure
//							Pointer to image file information structure
//							Pointer to temporary classification variable structure
//							Pointer to vector to be used to store the count of the number
//								of pixels classified into each class.
//
//	Parameters out:	None
//
// Value Returned:		
// 
// Called By:			ClassifyAreas
//
//	Coded By:			Larry L. Biehl			Date: 12/15/1988
//	Revised By:			Jeon						Date: 03/23/2006
//	Revised By:			Larry L. Biehl			Date: 05/12/2020

SInt16 ClassifyPerPointArea (
				SInt16								classPointer, 
				AreaDescriptionPtr				areaDescriptionPtr, 
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				LCToWindowUnitsVariables* 		lcToWindowUnitsVariablesPtr,
				ClassifierVarPtr					clsfyVariablePtr, 
				HSInt64Ptr							countVectorPtr)
	
{
	double								minutesLeft;
	
	LongRect								sourceRect;
								
	Point									point;		
	
	CMFileStream						*clProbabilityFileStreamPtr,
											*resultsFileStreamPtr;
											
	FileInfoPtr							fileInfoPtr;
	                   
	HUCharPtr	 						inputBufferPtr,
											outputBufferPtr,
											probabilityBufferPtr;
											
	HPtr									offScreenBufferPtr;	
											
	ImageOverlayInfoPtr				imageOverlayInfoPtr;
	Ptr									stringPtr;
	SInt16								*classPtr;
	UInt16								*channelsPtr;
	WindowInfoPtr 						imageWindowInfoPtr;
	WindowPtr							windowPtr;
	
	time_t								lineLoopNextTime;
	
	int									nextStatusAtLeastLine,
											nextStatusAtLeastLineIncrement;
	
	SInt32								displayBottomMax,
   										lineCount,
											line,
											lineEnd,
											lineInterval;
	
	UInt32								linesLeft,
											skipCount,
											startTick;
	
	SInt16								classNumber,
					 						errCode,
											returnCode;			// =0, no error			
																	// =1, no memory for 	
																	// 		text.				
																	// =2, disk file error.	
																	// =3, user quit.
																	
	Boolean								updateNumberLinesFlag;
	
	
			// Initialize local variables														
	
	channelsPtr = (UInt16*)GetHandlePointer (gClassifySpecsPtr->channelsHandle);
	classNumber	= areaDescriptionPtr->classNumber;
	lineEnd = areaDescriptionPtr->lineEnd;
	lineInterval = areaDescriptionPtr->lineInterval;
	lineCount = 0;
	linesLeft = areaDescriptionPtr->numberLines;
	returnCode = noErr;
	startTick = TickCount ();
	resultsFileStreamPtr = GetResultsFileStreamPtr (0);
	clProbabilityFileStreamPtr = GetResultsFileStreamPtr (1);
	classPtr = gClassifySpecsPtr->classPtr;
	fileInfoPtr = fileIOInstructionsPtr->fileInfoPtr;
	windowPtr = GetWindowPtr (gProjectInfoPtr->overlayImageWindowInfoHandle);
	imageWindowInfoPtr = (WindowInfoPtr)GetHandlePointer (
													gProjectInfoPtr->overlayImageWindowInfoHandle);
	
			// Set up source rect. This will indicate the lines and columns to
			// be updated when one does a copy to the image window.

	sourceRect.left = areaDescriptionPtr->columnStart - 1;
	sourceRect.right = areaDescriptionPtr->columnEnd;
	sourceRect.top = areaDescriptionPtr->lineStart - 1;
	sourceRect.bottom = lineEnd;
	displayBottomMax = sourceRect.bottom;
	
	imageOverlayInfoPtr = GetImageOverlayInfoPtr (gClassifySpecsPtr->imageOverlayIndex,
																	kLock,
																	NULL);
		
			// Load some of the File IO Instructions structure that pertain
			// to the specific area being classified.
			
	errCode = SetUpFileIOInstructions (fileIOInstructionsPtr,
													areaDescriptionPtr,
													gClassifySpecsPtr->numberChannels,
													channelsPtr,
													kDetermineSpecialBILFlag);
													
			// Get the input and output buffers to be used.
	
	inputBufferPtr = fileIOInstructionsPtr->inputBufferPtrs[
															fileIOInstructionsPtr->bufferUsedForIO];
	outputBufferPtr = fileIOInstructionsPtr->outputBufferPtrs[
															fileIOInstructionsPtr->bufferUsedForIO];
	
			// Get pointer to the probability map buffer in case it is needed.		
			
	probabilityBufferPtr = 
							&outputBufferPtr[gClassifySpecsPtr->probabilityBufferOffset];
	
			// This is for Quiog's special case.
	
	if (gTestFlag)		
		SetUpMaskAreaDescriptionParameters (&clsfyVariablePtr->maskAreaDescription, 
														areaDescriptionPtr);
	
	if (classNumber == 0)	
		{
				// Set parameter to control with time left is updated.													
			
		gNextMinutesLeftTime = gNextTime + 3*gNextStatusTimeOffset;
		
		}	// end "if (classNumber == 0)"
		
	lineLoopNextTime = gNextTime;
	
	offScreenBufferPtr = GetImageOverlayOffscreenPointer (imageOverlayInfoPtr);
	
			// These variables are to make sure the display window is not being updated
			// after a very few lines are loaded in. It will override the time interval
			// which is currently every 1 second.
	
	double magnification = lcToWindowUnitsVariablesPtr->magnification;
	nextStatusAtLeastLineIncrement = (int)((10 * lineInterval) / magnification);
	nextStatusAtLeastLineIncrement = MAX (nextStatusAtLeastLineIncrement, 10);
	nextStatusAtLeastLine = nextStatusAtLeastLineIncrement;
		
			// The purpose of skipCount is to only allow updates in drawing the
			// image overlay every 2 cycles of gNextTime.
	
	skipCount = 0;
	
	updateNumberLinesFlag = TRUE;

			// Loop through lines for area to be classified.							

	for (line=areaDescriptionPtr->lineStart; line<=lineEnd; line+=lineInterval)
		{
				// Display line status information.											
				
		lineCount++;
		if (TickCount () >= gNextStatusTime)
			{
			if (updateNumberLinesFlag)
				{
				LoadDItemValue (
					gStatusDialogPtr, IDC_Status20, (SInt32)areaDescriptionPtr->numberLines);
				updateNumberLinesFlag = FALSE;
				
				}	// end "if (updateNumberLinesFlag)"
				
			LoadDItemValue (gStatusDialogPtr, IDC_Status18, lineCount);
			gNextStatusTime = TickCount () + gNextStatusTimeOffset;
			
			}	// end "if (TickCount () >= gNextStatusTime)" 
			
					// Exit routine if user has "command period" down				
			
		if (TickCount () >= lineLoopNextTime)
			{
			skipCount++;
			
			if (gOutputCode & kCreateImageOverlayCode && 
							line > areaDescriptionPtr->lineStart &&
											lineCount >= nextStatusAtLeastLine &&
														skipCount >= 2)
				{
				sourceRect.bottom = line;

				InvalidateImageSegment (imageWindowInfoPtr,
												//displaySpecsPtr,
												lcToWindowUnitsVariablesPtr,
												&sourceRect,
												displayBottomMax);

				#if defined multispec_win  
					windowPtr->UpdateWindow ();
				#endif	// defined multispec_win  
			
				nextStatusAtLeastLine = lineCount + nextStatusAtLeastLineIncrement;
				nextStatusAtLeastLine = MIN (nextStatusAtLeastLine, lineEnd);
					
				skipCount = 0;

				}	// end "if (gOutputCode & kCreateImageOverlayCode && ..."
				
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
				{
				returnCode = 3;
				break;
				
				}	// end "if (!CheckSomeEvents (..."
			
			if (gOutputCode & kCreateImageOverlayCode)
				imageWindowInfoPtr->drawBaseImageFlag = FALSE;
		
			lineLoopNextTime = gNextTime;
				
			}	// end "if (TickCount () >= lineLoopNextTime)" 
		
				// Get all requested channels for line of image data.  Return 		
				// if there is a file IO error.												
				 
		errCode = GetLineOfData (fileIOInstructionsPtr,
											line, 
											(UInt32)areaDescriptionPtr->columnStart,
											(UInt32)areaDescriptionPtr->columnEnd,
											(UInt32)areaDescriptionPtr->columnInterval,
											inputBufferPtr,
											outputBufferPtr);
			
		if (errCode < noErr)
			{
			returnCode = 2;
			break;
			
			}	// end "if (errCode < noErr)"
			
		if (errCode != kSkipLine)
			{ 		
					// Classify the line of data with specified classifier.				
			
			areaDescriptionPtr->line = line;
			areaDescriptionPtr->numSamplesPerChan = 
												fileIOInstructionsPtr->numberOutputBufferSamples;
			point.v = (SInt16)line;
			point.h = (SInt16)areaDescriptionPtr->columnStart;									
			switch (gClassifySpecsPtr->mode)
				{
				case kMaxLikeMode:
				case kMahalanobisMode:
					returnCode = MaximumLikelihoodClassifier (areaDescriptionPtr,
																			fileInfoPtr,
																			clsfyVariablePtr,
																			outputBufferPtr,
																			probabilityBufferPtr, 
																			countVectorPtr,  
																			point);
					break;

				case kFisherMode:
					returnCode = FisherClassifier (areaDescriptionPtr,
																fileInfoPtr,
																clsfyVariablePtr,
																outputBufferPtr,
																probabilityBufferPtr, 
																countVectorPtr,  
																point);
					break;
            
				case kSupportVectorMachineMode:
					returnCode = SupportVectorMachineClassifier (areaDescriptionPtr,
																				 fileInfoPtr,
																				 clsfyVariablePtr,
																				 outputBufferPtr,
																				 probabilityBufferPtr,
																				 countVectorPtr,
																				 point);
               break;
					
				case kKNearestNeighborMode:
					returnCode = KNNClassifier (areaDescriptionPtr,
                                              fileInfoPtr,
                                              clsfyVariablePtr,
                                              outputBufferPtr,
                                              probabilityBufferPtr,
                                              countVectorPtr,
                                              point);
					break;

					
				case kEuclideanMode:
					returnCode = EuclideanClassifier (areaDescriptionPtr,
																	fileInfoPtr,
																	clsfyVariablePtr,
																	outputBufferPtr,
																	probabilityBufferPtr,
																	countVectorPtr,  
																	point);
					break;
					
				case kCorrelationMode:
					returnCode = CorrelationClassifier (areaDescriptionPtr,
																	fileInfoPtr,
																	clsfyVariablePtr,
																	outputBufferPtr,
																	probabilityBufferPtr,
																	countVectorPtr,  
																	point);
					break;
					
				case kCEMMode:
					returnCode = CEMClassifier (areaDescriptionPtr,
															fileInfoPtr,
															clsfyVariablePtr,
															outputBufferPtr,
															probabilityBufferPtr,
															countVectorPtr,  
															point);
					break;
					
				case kParallelPipedMode:
					returnCode = ParallelPipedClassifier (areaDescriptionPtr,
																		fileInfoPtr,
																		clsfyVariablePtr,
																		outputBufferPtr,
																		probabilityBufferPtr,
																		countVectorPtr,  
																		point);
					break;
					
				}	// end "switch (gClassifySpecsPtr->mode)" 
				
			if (returnCode == 3)
				break;
										
			if (fileIOInstructionsPtr->maskBufferPtr == NULL)
				returnCode = WriteClassificationResults (outputBufferPtr,
																		areaDescriptionPtr, 
																		resultsFileStreamPtr, 
																		clProbabilityFileStreamPtr, 
																		countVectorPtr, 
																		lineCount);
			if (returnCode == 1)
				break;
		  			
			if (gOutputCode & kCreateImageOverlayCode)
				{
				CopyToOffscreenBuffer (fileIOInstructionsPtr,
												imageOverlayInfoPtr,
												gClassifySpecsPtr->imageOverlayIndex,
												gProjectInfoPtr->overlayImageWindowInfoHandle,
												line,
												(UInt32)areaDescriptionPtr->columnStart,
												(UInt32)areaDescriptionPtr->columnInterval,
												(UInt32)areaDescriptionPtr->numSamplesPerChan,
												areaDescriptionPtr->lineStart,
												areaDescriptionPtr->rgnHandle,
												outputBufferPtr,
												offScreenBufferPtr,
												1,
												FALSE);
													
				}	// end "if (gOutputCode & kCreateImageOverlayCode)"
				
			}	// end "if (errCode != kSkipLine)"
		
				// If this in an image area, update dialog status information.		
		
		if (classNumber == 0)
			{
			linesLeft--;
			
			if (TickCount () >= gNextMinutesLeftTime)
				{
				minutesLeft = ((double)linesLeft * (TickCount () - startTick))/
																((double)lineCount*kTicksPerMinute);
				
				sprintf ((char*)gTextString, " %.1f", minutesLeft);
				stringPtr = (char*)CtoPstring (gTextString, gTextString);
				LoadDItemString (gStatusDialogPtr, IDC_Status14, (Str255*)gTextString);
				
				#if defined multispec_wx
					CheckSomeEvents (osMask+updateMask);
				#endif
					
				gNextMinutesLeftTime = TickCount () + gNextMinutesLeftTimeOffset;
				
				}	// end "if (TickCount () >= gNextMinutesLeftTime)" 
				
			}	// end "if (classNumber == 0)"
				
		if (gUseThreadedIOFlag)
			{
					// Switch buffers to be ready for the next set to use.
	
			inputBufferPtr = fileIOInstructionsPtr->inputBufferPtrs[
															fileIOInstructionsPtr->bufferUsedForIO];
			outputBufferPtr = fileIOInstructionsPtr->outputBufferPtrs[
															fileIOInstructionsPtr->bufferUsedForIO];
			probabilityBufferPtr = 
							&outputBufferPtr[gClassifySpecsPtr->probabilityBufferOffset];
			
			}	// end "if (gUseThreadedIOFlag)"
			
		if (fileIOInstructionsPtr->maskBufferPtr != NULL)
			fileIOInstructionsPtr->maskBufferPtr += 
												fileIOInstructionsPtr->numberMaskColumnsPerLine;
										
		}	// end "for (line=areaDescriptionPtr->lineStart; line..." 
		
	LoadDItemValue (gStatusDialogPtr, IDC_Status18, (SInt32)lineCount);
	  	
  			// Force overlay to be drawn if it has not been already.
  				
	if (gOutputCode & kCreateImageOverlayCode)
		{
		sourceRect.bottom = displayBottomMax;

		InvalidateImageSegment (imageWindowInfoPtr,
										lcToWindowUnitsVariablesPtr,
										&sourceRect,
										displayBottomMax);

		if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
			returnCode = 3;
			
		if (imageWindowInfoPtr != NULL)
			imageWindowInfoPtr->drawBaseImageFlag = FALSE;
			
		}	// end "if (gOutputCode & kCreateImageOverlayCode)"
  		
  	UnlockImageOverlayOffscreenBuffer (imageOverlayInfoPtr);
	/*
	if (!gOSXCoreGraphicsFlag)
		{
		Handle activeImageWindowInfoHandle =
													gProjectInfoPtr->overlayImageWindowInfoHandle;
		WindowInfoPtr imageWindowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																		activeImageWindowInfoHandle);
		if (imageWindowInfoPtr != NULL)
			imageWindowInfoPtr->drawBaseImageFlag = TRUE;
		
		}	// end "if (!gOSXCoreGraphicsFlag)"
	*/
	if (gTestFlag)	
		{	
				// Unlock handle related to any mask area that was used.
		
		CheckAndUnlockHandle (
								clsfyVariablePtr->maskAreaDescription.maskInfoPtr->maskHandle);
		clsfyVariablePtr->maskAreaDescription.maskInfoPtr->maskPointer = NULL;
		
		}	// end "if (gTestFlag)"
		
			// Close up any File IO Instructions structure that pertain
			// to the specific area that was classified.
			
	CloseUpFileIOInstructions (fileIOInstructionsPtr, areaDescriptionPtr);
		
			// If area is a training or test field, list summary of 					
			// classification results.															 
	
	if (classNumber > 0 && returnCode == noErr)
		returnCode = ListTrainTestSummary (classPointer,
														resultsFileStreamPtr, 
														areaDescriptionPtr, 
														countVectorPtr,
														clsfyVariablePtr, 
														gTempDoubleVariable1);
	
	return (returnCode);
			
}	// end "ClassifyPerPointArea" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ClassifyTrainTestFields
//
//	Software purpose:	The purpose of this routine is to classify the
//							training or test fields.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:		
// 
// Called By:			ClassifyAreasControl
//
//	Coded By:			Larry L. Biehl			Date: 12/15/1988
//	Revised By:			Larry L. Biehl			Date: 05/04/2019

SInt16 ClassifyTrainTestFields (
				AreaDescriptionPtr				areaDescriptionPtr, 
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				LCToWindowUnitsVariables* 		lcToWindowUnitsVariablesPtr,
				ClassifierVarPtr					clsfyVariablePtr, 
				SInt16								fieldType)
				
{
	CMFileStream*						classifyFileStreamPtr;
	HPClassNamesPtr					classNamesPtr;
	HPFieldIdentifiersPtr			fieldIdentPtr;
	SInt16*								classAreaPtr;
	
	SInt32								countVectorIndex,
											statClassNumber;
	
	UInt32								totalCountFields;
	
	SInt16								classStorage,
											fieldCount,
											fieldNumber,
											numberFields,
											returnCode,			// =0, no error			
															// =1, no memory for 	
															// 		text.				
															// =2, disk file error.	
															// =3, user quit.			
											savedOutputCode,
											stringNumber,
											strLength;
	
	UInt16								classIndex;
	
	Boolean								continueFlag,
											listMaskEchoInfoFlag;
	

			// Initialize local variables.													
			// Force output storage flag to only list to the output text window	
			// or an ASCII disk file.  Training field and test field data will	
			// not be listed in an ERDAS formatted disk file.							
			
	savedOutputCode = gOutputCode;
	gOutputCode = (savedOutputCode & 0x0083);
	listMaskEchoInfoFlag = FALSE;
	returnCode = noErr;
	classifyFileStreamPtr = GetResultsFileStreamPtr (0);
	countVectorIndex = 0;
	classNamesPtr = gProjectInfoPtr->classNamesPtr;
	classAreaPtr = (SInt16*)GetHandlePointer (gClassifySpecsPtr->classAreaHandle);
	continueFlag = TRUE;
			
			// Clear the memory for the counts.												
	
	totalCountFields = clsfyVariablePtr->totalCountIndexStart +
													gProjectInfoPtr->numberStatisticsClasses + 1;
	for (classIndex=0; classIndex<totalCountFields; classIndex++)
		clsfyVariablePtr->countVectorPtr[classIndex] = 0;
		
		
	clsfyVariablePtr->areaCode = fieldType;
		
			// Initialize memory for countClassIndexPtr.									
			// Initialize the entire vector even if only a subset of the classes	
			// will be used.  This is for debugging purposes.							
			
	for (classIndex=0; 
			classIndex<gProjectInfoPtr->numberStatisticsClasses; 
				classIndex++)
		clsfyVariablePtr->countClassIndexPtr[classIndex] = -1;
		
			// Set up status dialog.  Load in number of classes.
				
	LoadDItemValue (
			gStatusDialogPtr, IDC_Status5, (SInt32)gClassifySpecsPtr->numberClassAreas);
	
	for (classIndex=0; 
			classIndex<(UInt16)gClassifySpecsPtr->numberClassAreas; 
				classIndex++)
		{
		statClassNumber = classAreaPtr[classIndex] - 1;
		
				// Get the class storage number.												
						
		classStorage = gProjectInfoPtr->storageClass[statClassNumber];
		
		fieldNumber = classNamesPtr[classStorage].firstFieldNumber;
		
				// Save the count vector index for the start of this class.			
				
		clsfyVariablePtr->countClassIndexPtr[statClassNumber] = countVectorIndex;
		
				// Set up status dialog.  Load in class count, number of train/	
				// test fields and class name.												
				
		LoadDItemValue (gStatusDialogPtr, IDC_Status3, (SInt32)classIndex+1);
		LoadDItemString (
			gStatusDialogPtr, IDC_Status6, (Str255*)&classNamesPtr[classStorage].name);
					
		if (fieldType == kTrainingType) 
			numberFields = classNamesPtr[classStorage].numberOfTrainFields;
		else
			numberFields = classNamesPtr[classStorage].numberOfTestFields;
			
		LoadDItemValue (gStatusDialogPtr, IDC_Status10, (SInt32)numberFields);
				
				// Print the class name and number											
				
		pstr ((char*)gTextString2,
							(char*)&classNamesPtr[classStorage].name,&strLength);
		sprintf ((char*)gTextString, 
					"     Class number = %d '%s'%s",
					(int)(statClassNumber+1),
					(char*)gTextString2,
					gEndOfLine);
		continueFlag = OutputString (classifyFileStreamPtr, 
												(char*)gTextString,
												0, 
												gOutputCode, 
												continueFlag);
		returnCode = !continueFlag;
		
		fieldCount = 1;
		
		while ((fieldNumber != -1) && (returnCode == noErr))
			{
			fieldIdentPtr = &gProjectInfoPtr->fieldIdentPtr[fieldNumber];
	
					// Make certain that field is field type.								
					
			if (fieldIdentPtr->fieldType == fieldType)
				{
						// Set up field count in status dialog.							
				
				LoadDItemValue (gStatusDialogPtr, IDC_Status8, (SInt32)fieldCount);
				
						// If training fields are being classified and the field		
						// has been defined as a training field but the statistics	
						// for this field have not been included yet, then do not	
						// include this field in the classification.						
						
				if (fieldType == kTrainingType && (!fieldIdentPtr->loadedIntoClassStats &&
						gClassifySpecsPtr->mode != kSupportVectorMachineMode &&
								gClassifySpecsPtr->mode != kKNearestNeighborMode))
					break;
				
						// List the field name and number									
				
				pstr ((char*)gTextString2, (char*)&fieldIdentPtr->name, &strLength);
				if (fieldType == kTrainingType)
					sprintf ((char*)gTextString, 
								"      Training field number = %d '%s'%s",
								(int)(fieldNumber+1),
								(char*)gTextString2,
								gEndOfLine);
								
				else if (fieldType == kTestingType)
					sprintf ((char*)gTextString, 
								"      Test field number = %d '%s'%s",
								(int)(fieldNumber+1),
								(char*)gTextString2,
								gEndOfLine);
				
				continueFlag = OutputString (classifyFileStreamPtr, 
														(char*)gTextString,
														0,
														gOutputCode,
														continueFlag);
				
				if (continueFlag)
					{
							// if the field is a cluster type of field, do not classify it
							// since there are no coordinates to define it.
							
					if (fieldIdentPtr->pointType < kClusterType || 
								(fieldIdentPtr->pointType == kMaskType && 
															gClassifySpecsPtr->mode != kEchoMode))
						{
								// Get the field coordinates.									
								
						GetFieldBoundary (gProjectInfoPtr, areaDescriptionPtr, fieldNumber);
						
								// Classify the field.											
						
						returnCode = ClassifyArea (
										classAreaPtr[classIndex],
										areaDescriptionPtr, 
										fileIOInstructionsPtr,
										lcToWindowUnitsVariablesPtr,
										clsfyVariablePtr,
										&clsfyVariablePtr->countVectorPtr[countVectorIndex]);
						
								// Dispose of the region if it exists.						
		
						if (areaDescriptionPtr->rgnHandle != NULL)
							{
							DisposeRgn (areaDescriptionPtr->rgnHandle);
							areaDescriptionPtr->rgnHandle = NULL;
							
							}	// end "if (areaDescriptionPtr->rgnHandle != NULL)" 
							
						}	// end "if (....pointType != ..." 
						
					else	// ....pointType == kClusterType || ...
						{
						stringNumber = IDS_Classify3;
						if (fieldIdentPtr->pointType == kMaskType)
							{
							stringNumber = IDS_Classify60;
							listMaskEchoInfoFlag = TRUE;
							
							}	// end "if (fieldIdentPtr->pointType == kMaskType)"
							
						continueFlag = ListSpecifiedStringNumber (
																			kClassifyStrID,
																			stringNumber, 
																			(unsigned char*)gTextString, 
																			classifyFileStreamPtr, 
																			gOutputCode, 
																			continueFlag);
						
						}	// end "else ....pointType == kClusterType || ..."
						
					}	// end "if (continueFlag)" 
															
						// Update count vector index to be ready for next field.		
						
				countVectorIndex += gProjectInfoPtr->numberStatisticsClasses + 1;
					
				fieldCount++;								
								
				}	// end "if (fieldIdentPtr->field..." 
				
			fieldNumber = fieldIdentPtr->nextField;
			returnCode = !continueFlag + returnCode;
			
			}	// end "while ((fieldNumber != -1) && ..." 
			
		if (returnCode != noErr)
			break;
			
		}	// end "for (classIndex=0; classIndex<..." 
		
	if (listMaskEchoInfoFlag)
		continueFlag = ListSpecifiedStringNumber (kClassifyStrID,
																IDS_Classify65, 
																(unsigned char*)gTextString, 
																classifyFileStreamPtr, 
																gOutputForce1Code, 
																continueFlag);
	
			// List the performance table if there was not a file read error or	
			// listing error.																		
	
	if (returnCode != 1)
		returnCode = !ListPerformanceTables (clsfyVariablePtr,
															fieldType, 
															classifyFileStreamPtr, 
															&gOutputForce1Code,
															gClassifySpecsPtr->classPtr,
															gClassifySpecsPtr->numberClasses,
															classAreaPtr,
															gClassifySpecsPtr->numberClassAreas);
	
			// Restore output storage flag.													
			// Allow for text window or ascii file not being able to write to now.
	
	savedOutputCode = (savedOutputCode & 0xfffc);		
	gOutputCode = (savedOutputCode | gOutputCode);
		
	return (returnCode);

}	// end "ClassifyTrainTestFields"   



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 CorrelationClassifier
//
//	Software purpose:	The purpose of this routine is to classify the
//							input line of data using the correlation classifier.
//
//	Parameters in:		Pointer to area description structure
//							Pointer to image file information structure
//							Pointer to temporary classification variable structure
//							Pointer to buffer to be used to store the classification
//								results in for a line.
//							Pointer to a buffer to be used to store the probability
//								of correct classification information in for a line.
//							Pointer to a vector to be used to store the count of the
//								number of pixels classified into each class
//							A structure which defines the start line and column values
//								that the classification will begin at.	
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			ClassifyPerPointArea
//
//	Coded By:			Larry L. Biehl			Date: 08/13/1997
//	Revised By:			Larry L. Biehl			Date: 03/27/2012

SInt16 CorrelationClassifier (
				AreaDescriptionPtr				areaDescriptionPtr, 
				FileInfoPtr							fileInfoPtr,
				ClassifierVarPtr					clsfyVariablePtr, 
				HUCharPtr							outputBuffer1Ptr, 
				HUCharPtr							thresholdBufferPtr, 
				HSInt64Ptr							countVectorPtr, 
				Point									point)

{
	double								correlationCoefficientThreshold,
											discriminantMax,
											dValue,
											xTranspose_x,
											xTranspose_m;
												
	RgnHandle							rgnHandle;
	
	double								*classConstantPtr,
											*thresholdTablePtr;
	
	SInt16*	 							classPtr; 
												            
	HDoublePtr 							chanMeanPtr,
											covariancePtr,
											ioBufferReal8Ptr,
											savedBufferReal8Ptr;
												 
	UInt32								classIndex,
											feat1,
											feat2,
											maxClass,
											numberChannels,
											numberClasses,
											numberSamplesPerChan,
											sameDistanceCount,
											sample;
												
	Boolean								correlationCovarianceFlag,
											polygonField,
											createCorrelationValuesFlag;
	
	
			// Initialize local variables.														
	
	numberChannels = 			gClassifySpecsPtr->numberChannels;
	numberClasses = 			gClassifySpecsPtr->numberClasses;
	numberSamplesPerChan = 	(UInt32)areaDescriptionPtr->numSamplesPerChan;
	polygonField =				areaDescriptionPtr->polygonFieldFlag;
	rgnHandle = 				areaDescriptionPtr->rgnHandle;
	classPtr = 					gClassifySpecsPtr->classPtr;
	savedBufferReal8Ptr = 	(HDoublePtr)outputBuffer1Ptr;
	maxClass = 					0;
	
	correlationCoefficientThreshold = 
										gClassifySpecsPtr->correlationCoefficientThreshold;
	correlationCovarianceFlag = 
					(gClassifySpecsPtr->correlationCovarianceCode > kNoCovarianceUsed);
	
	classConstantPtr = clsfyVariablePtr->classConstantPtr;
	
	createCorrelationValuesFlag = gClassifySpecsPtr->createThresholdTableFlag;
	thresholdTablePtr = gClassifySpecsPtr->thresholdTablePtr;
		
			// Loop through the number of samples in the line of data				
	
	for (sample=0; sample<numberSamplesPerChan; sample++)
		{
		if (!polygonField || PtInRgn (point, rgnHandle))
			{
			covariancePtr = clsfyVariablePtr->covariancePtr;
			
			if (!correlationCovarianceFlag)
				{
				xTranspose_x = 0.;
				
				if (!gClassifySpecsPtr->featureTransformationFlag)
					{
							// Get the square root of the square of the pixel vector.
					
					ioBufferReal8Ptr = savedBufferReal8Ptr;
					for (feat1=0; feat1<numberChannels; feat1++)
						{
						xTranspose_x += *ioBufferReal8Ptr * *ioBufferReal8Ptr;
						ioBufferReal8Ptr++;
						
						}	// end "for (feature=0; feature<..."
						
					}	// end "if (!gClassifySpecsPtr->featureTransformationFlag)"
						
				else	// gClassifySpecsPtr->featureTransformationFlag
					{
					for (feat1=0; feat1<numberChannels; feat1++)
						{
						ioBufferReal8Ptr = savedBufferReal8Ptr;
						dValue = 0.;					
						for (feat2=0; feat2<feat1; feat2++)
							{
							dValue += *covariancePtr * *ioBufferReal8Ptr;
							covariancePtr++;
							ioBufferReal8Ptr++;			
						
							}	// end "for (feat2=0; feat2<..." 
							
						dValue += dValue;
						xTranspose_x += *ioBufferReal8Ptr *
													(dValue + *covariancePtr * *ioBufferReal8Ptr);
						covariancePtr++;
						
						}	// end "for (feat1=0; feat1<..." 
					
					}	// end "else gClassifySpecsPtr->featureTransformationFlag"
					
				xTranspose_x = sqrt (xTranspose_x);
				
				}	// end "if (!correlationCovarianceFlag)"	
					
					// Loop through the classes and get the discriminant value for	
					// each class.																	
		
			discriminantMax = -1e200;
			chanMeanPtr = clsfyVariablePtr->chanMeanPtr;
			
			for (classIndex=0; classIndex<numberClasses; classIndex++)
				{
				if (correlationCovarianceFlag)
					{
							// Get the square root of the square of the pixel vector.
						
					xTranspose_x = 0.;
					
					for (feat1=0; feat1<numberChannels; feat1++)
						{
						ioBufferReal8Ptr = savedBufferReal8Ptr;
						dValue = 0.;					
						for (feat2=0; feat2<feat1; feat2++)
							{
							dValue += *covariancePtr * *ioBufferReal8Ptr;
							covariancePtr++;
							ioBufferReal8Ptr++;			
						
							}	// end "for (feat2=0; feat2<..." 
							
						dValue += dValue;
						xTranspose_x += *ioBufferReal8Ptr *
													(dValue + *covariancePtr * *ioBufferReal8Ptr);
						covariancePtr++;
						
						}	// end "for (feat1=0; feat1<..." 
						
					xTranspose_x = sqrt (xTranspose_x);
					
					}	// end "if (correlationCovarianceFlag)"	
			
						// Loop through the channels (features) and get the transpose
						// of the data vector times the class mean vector.		
						
				ioBufferReal8Ptr = savedBufferReal8Ptr;
				xTranspose_m = 0;
				for (feat1=0; feat1<numberChannels; feat1++)
					{
					xTranspose_m += *ioBufferReal8Ptr * *chanMeanPtr;
					
					ioBufferReal8Ptr++;
					chanMeanPtr++;
					
					}	// end "for (feature=0; feature<..." 
					
						// Compute discriminant value for class "statClassNumber"
				
				if (xTranspose_x > 0 && classConstantPtr[classIndex] > 0)	
					dValue = xTranspose_m/(xTranspose_x * classConstantPtr[classIndex]);
					
				else	// xTranspose_x == 0 || classConstantPtr[classIndex] == 0
					dValue = 0.;
				
						// Check if discriminant value for this class is largest.  If	
						// so, save the value and the class number.
						
				if (dValue > discriminantMax)
					{
					discriminantMax = dValue;
					maxClass = classIndex;
					sameDistanceCount =  0;
					
					}	// end "if (dValue1 > discriminantMax)"
					
				else if (dValue == discriminantMax)
					sameDistanceCount++;
	
				}	// end "for (classIndex=0; classIndex<..."
				
			classIndex = classPtr[maxClass];
		   countVectorPtr[classIndex]++;
		   *outputBuffer1Ptr = (UInt8)classIndex;
		   
		   if (createCorrelationValuesFlag)
		   	{
		   			// Get the threshold table index.									
					
				classIndex = CorrelationThresholdClass (discriminantMax, 
																		thresholdTablePtr);
				
				*thresholdBufferPtr = (UInt8)classIndex;
				thresholdBufferPtr++;
				
				gTempDoubleVariable1 += discriminantMax;
		   	
		   	}	// end "if (createCorrelationValuesFlag)"	
		   
		   }	// end "if (!polygonField || PtInRgn (point, rgnHandle))" 
		   
		else	// polygonField && !PtInRgn (point, rgnHandle)
			{ 
			*outputBuffer1Ptr = 0; 
			
		   if (createCorrelationValuesFlag)
		   	{
				*thresholdBufferPtr = 0;
				thresholdBufferPtr++;
				
				}	// end "if (createCorrelationValuesFlag)"
				
			sameDistanceCount = 0;
				
			}	// end "else polygonField && !PtInRgn (point, rgnHandle)" 
			
		point.h++;
		savedBufferReal8Ptr += numberChannels;
		outputBuffer1Ptr++;
		
		clsfyVariablePtr->totalSameDistanceSamples += sameDistanceCount;
			
				// Exit routine if user selects "cancel" or "command period".		
		
		if (TickCount () >= gNextTime)
			{
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
																								return (3);
				
			}	// end "if (TickCount () >= gNextTime)" 
				
		}	// end "for (sample=0; sample<..." 
	
			// Write carriage return and null character to classification record	
			
   *outputBuffer1Ptr = kCarriageReturn;
   outputBuffer1Ptr++;  
                      
	#if defined multispec_win
   	*outputBuffer1Ptr = kLineFeed;
  		outputBuffer1Ptr++;
	#endif	// defined multispec_win  
	
   *outputBuffer1Ptr = kNullTerminator;
   
   return (0);

}	// end "CorrelationClassifier"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CorrelationClsfierControl
//
//	Software purpose:	The purpose of this routine is to classify the
//							specified data using the correlation classifer.
//
//	Parameters in:		Pointer to image file information structure
//
//	Parameters out:	None
//
// Value Returned:	Flag indicating whether output to text window
//								can still be generated.
// 
// Called By:			ClassifyControl
//
//	Coded By:			Larry L. Biehl			Date: 08/12/1997
//	Revised By:			Larry L. Biehl			Date: 03/27/2012

void CorrelationClsfierControl (
				FileInfoPtr							fileInfoPtr)

{
	ClassifierVar						classifierVar;
	
	double								logDeterminant;
	
	CMFileStream*						resultsFileStreamPtr;
	
	double*								classConstantPtr = NULL;
												
	HChannelStatisticsPtr			classChannelStatsPtr = NULL;
												
	HDoublePtr							classifyChanMeanPtr = NULL,
											classifyCovPtr = NULL,
											inverseCovPtr = NULL,
											tempChanMeanPtr;
	
	SInt16								*statFeaturePtr = NULL;
	
	UInt32								clsfyCovStart,
											clsfyChanMeanStart,
											numberClsfyCovEntries;
												
	Boolean								continueFlag,
											determinantOKFlag;
	
	UInt16								classIndex,
											numberFeatureChannels,
											numberFeatures,
											statClassNumber;
	
		
			// Initialize local variables														
	
	continueFlag = TRUE;
	numberFeatureChannels = gClassifySpecsPtr->numberChannels;
	numberFeatures = gClassifySpecsPtr->numberFeatures;
 	resultsFileStreamPtr = GetResultsFileStreamPtr (0);
	
			// Put description in the status dialog.										
					
	LoadDItemStringNumber (kClassifyStrID, 
									IDS_Classify20, 
									gStatusDialogPtr, 
									IDC_Status11, 
									(Str255*)gTextString);
		
	InitializeClassifierVarStructure (&classifierVar);
	
			// Get the memory for the mean vectors and covariance matrices			
			// for the classes and channels to be used in the classification.		
	
	if ((gClassifySpecsPtr->correlationCovarianceCode > kNoCovarianceUsed) || 
													gClassifySpecsPtr->featureTransformationFlag)
		continueFlag = SetupClsfierMemory (&classifyChanMeanPtr,
														&classifyCovPtr,
														&classConstantPtr,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL, 
														&classChannelStatsPtr, 
														&gInverseMatrixMemory.inversePtr,
														&gInverseMatrixMemory.pivotPtr, 
														&gInverseMatrixMemory.indexRowPtr, 
														&gInverseMatrixMemory.indexColPtr, 
														&gInverseMatrixMemory.ipvotPtr);
	
	else	// ...->correlationCovarianceCode == kNoCovarianceUsed && ...		 
		continueFlag = SetupClsfierMemory (&classifyChanMeanPtr,
														NULL,
														&classConstantPtr,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL, 
														&classChannelStatsPtr, 
														NULL, 
														NULL, 
														NULL, 
														NULL, 
														NULL);
	
	inverseCovPtr = gInverseMatrixMemory.inversePtr;
											
	UInt16* featurePtr = (UInt16*)GetHandlePointer (gClassifySpecsPtr->featureHandle);
		
	if (continueFlag)			
		continueFlag = SetupFeatureTransformationMemory (
													gClassifySpecsPtr->featureTransformationFlag,
													numberFeatures, 
													numberFeatureChannels, 
													&gTransformationMatrix.eigenVectorPtr, 
													&gTransformationMatrix.tempMatrixPtr,
													NULL, 
													&gTransformationMatrix.eigenFeaturePtr,
													kLoadMatricesVectors,
													featurePtr);
	
	if (continueFlag && gClassifySpecsPtr->featureTransformationFlag)
		{   
		/*                                              
		HDoublePtr eigenVectorPtr = (HDoublePtr)GetHandlePointer (
											gTransformationMatrix.eigenVectorHandle,
											kLock);
											
		ReduceMatrix (eigenVectorPtr, 
							gTransformationMatrix.eigenVectorPtr, 
							gTransformationMatrix.numberFeatures, 
							numberFeatureChannels, 
							numberFeatures, 
							(SInt16*)featurePtr);
							
		CheckAndUnlockHandle (gTransformationMatrix.eigenVectorHandle);
		*/		
				// Now if the covariance matrix is not being used create a
				// matrix in place of the covariance matrix which will be used
				// to transform the data vector.
				
		if (gClassifySpecsPtr->correlationCovarianceCode == kNoCovarianceUsed)
			{
					// Get Tt_T
		
			MatrixMultiply (gTransformationMatrix.eigenVectorPtr,
									gTransformationMatrix.eigenVectorPtr,
									inverseCovPtr, 
									numberFeatureChannels,
									numberFeatures,
									numberFeatureChannels,
									kTransposeMat1ByMat2);
												
					// Copy the full form (square) matrix back to triangular		
					// form storage. This takes less space and makes the 			
					// classification algorithm a little easier to implement 	
					// and quicker.											
					
			SquareToTriangularMatrix (inverseCovPtr,
												classifyCovPtr,
												numberFeatureChannels);
			
			}	// end "if (...->correlationCovarianceCode == kNoCovarianceUsed)"
		
		}	// end "if (continueFlag && ...)"
	
			// Continue with classification, if memory is not full.					
	
	if (continueFlag)
		{
				// Loop through the classes and for classes to be used in 			
				// classification get the:														
				//		mean vector for class.													
				//		covariance matrix for class if needed.						
				//		inverse of covariance matrix for class.
				//		square root of square of mean vectors.							
		
		clsfyChanMeanStart = 0;
		clsfyCovStart = 0;
											
		numberClsfyCovEntries = (UInt32)numberFeatureChannels * 
																	(numberFeatureChannels+1)/2;
		
		statFeaturePtr = (SInt16*)GetStatisticsFeatureVector (
												gClassifySpecsPtr->featureTransformationFlag,
												featurePtr);
										
		determinantOKFlag = TRUE;
		
				// Now get the common covariance matrix if it is to be used.
				
		if (gClassifySpecsPtr->correlationCovarianceCode == kCommonCovariance)
			continueFlag = GetTtInvTransformedCommonCovarianceT (
															numberFeatureChannels,
															classChannelStatsPtr,
															inverseCovPtr,
															(UInt16*)statFeaturePtr,
															(UInt16*)gClassifySpecsPtr->classPtr,
															gClassifySpecsPtr->numberClasses,
															kSquareOutputMatrix,
															gTransformationMatrix.eigenVectorPtr,
															classifyCovPtr,
															numberFeatures, 
															&logDeterminant,
															&determinantOKFlag);
		
		}	// end "if (continueFlag)"
		
	if (continueFlag)
		{
		for (classIndex=0; 
				classIndex<gClassifySpecsPtr->numberClasses; 
					classIndex++)
			{
			statClassNumber = gClassifySpecsPtr->classPtr[classIndex] - 1;
			tempChanMeanPtr = gInverseMatrixMemory.pivotPtr;
				
			if (gClassifySpecsPtr->correlationCovarianceCode == kNoCovarianceUsed)
				{				
				if (!gClassifySpecsPtr->featureTransformationFlag)
					tempChanMeanPtr = &classifyChanMeanPtr[clsfyChanMeanStart];
					
						// Get the class mean vector
					
				GetClassMeanVector (numberFeatureChannels,
											classChannelStatsPtr, 
											tempChanMeanPtr,
											(UInt16*)statFeaturePtr, 
											statClassNumber);
					
				if (gClassifySpecsPtr->featureTransformationFlag)
					{
							// Now get Tt_T_M
							
					MatrixMultiply (inverseCovPtr, 
											tempChanMeanPtr,
											&classifyChanMeanPtr[clsfyChanMeanStart],
											numberFeatureChannels,
											numberFeatureChannels, 
											1,
											kMat1ByMat2);
												
					}	// end "if (!gClassifySpecsPtr->featureTransformationFlag)"
									
						// Now get  Mt_M or Mt_Tt_T_M
							
				MatrixMultiply (tempChanMeanPtr, 
										&classifyChanMeanPtr[clsfyChanMeanStart],
										&classConstantPtr[classIndex], 
										1,
										numberFeatureChannels,
										1,
										kTransposeMat1ByMat2);
					
				}	// end "if (...->correlationCovarianceCode == kNoCovarianceUsed)"
			
			else	// ...->correlationCovarianceCode != kNoCovarianceUsed
				{
				clsfyCovStart = classIndex * numberClsfyCovEntries;
				
				if (gClassifySpecsPtr->correlationCovarianceCode == kSampleCovariance)
					{		
							// Get InvCov or Tt_InvTCov_T	

					continueFlag = GetTtInvTransformedCovarianceT (
																numberFeatureChannels,
																classChannelStatsPtr, 
																inverseCovPtr,
																(UInt16*)statFeaturePtr, 
																statClassNumber,
																kSquareOutputMatrix,
																kMeanCovariance,
																gTransformationMatrix.eigenVectorPtr,
																gTransformationMatrix.tempMatrixPtr,
																numberFeatures,
																&logDeterminant,
																&determinantOKFlag);
						
					if (!continueFlag)
						break;
							
					}	// end "if (...->correlationCovarianceCode == kSampleCovariance)" 
					
				if (determinantOKFlag)
					{	
							// Get the class mean vector
						
					GetClassMeanVector (numberFeatureChannels,
												classChannelStatsPtr, 
												tempChanMeanPtr,
												(UInt16*)statFeaturePtr, 
												statClassNumber);
						
							// Now get Mt_InvCov_M or Mt_Tt_InvTCov_T_M
							
					MatrixMultiply (tempChanMeanPtr, 
											inverseCovPtr,
											&classifyCovPtr[clsfyCovStart], 
											1,
											numberFeatureChannels,
											numberFeatureChannels,
											kTransposeMat1ByMat2);
						
					MatrixMultiply (&classifyCovPtr[clsfyCovStart],
											tempChanMeanPtr,
											&classConstantPtr[classIndex], 
											1,
											numberFeatureChannels,
											1,
											kMat1ByMat2);
				
							// Now get InvCov_M or Tt_InvTCov_T_M
							
					MatrixMultiply (inverseCovPtr, 
											tempChanMeanPtr,
											&classifyChanMeanPtr[clsfyChanMeanStart],
											numberFeatureChannels,
											numberFeatureChannels, 
											1,
											kMat1ByMat2);	
												
							// Copy the full form (square) matrix back to triangular		
							// form storage. This takes less space and makes the 			
							// classification algorithm a little easier to implement 	
							// and quicker.											
							
					SquareToTriangularMatrix (inverseCovPtr,
														&classifyCovPtr[clsfyCovStart],
														numberFeatureChannels);
							
					}	// end "if (determinantOKFlag)"
				
				else	// !determinantOKFlag
					{
							// List message that covariance matrix for this class 		
							// could not be inverted.												
						
					continueFlag = ListClassInformationMessage (kProjectStrID,
																				IDS_Project31,
																				resultsFileStreamPtr, 
																				gOutputForce1Code,
																				statClassNumber, 
																				continueFlag);
					
					}	// end "else !determinantOKFlag" 
										
				}	// end "else ...->correlationCovarianceCode != kNoCovarianceUsed"
				
			classConstantPtr[classIndex] = sqrt (classConstantPtr[classIndex]);
									
			clsfyChanMeanStart += numberFeatureChannels;
				
			}	// end "for (class=0; class<Classify..." 
			
		}	// end "if (continueFlag)" 
			
			// Release memory for the matrix inversion work areas.					
	
	ReleaseMatrixInversionMemory ();	
	inverseCovPtr = NULL;
	
			// Release memory for the classify channels statistics.					
			
	classChannelStatsPtr = (HChannelStatisticsPtr)CheckAndDisposePtr (
																			(Ptr)classChannelStatsPtr);
	
			// Release memory for transformation matrix information.					
			
	ReleaseFeatureTransformationMemory ();
	
	if (continueFlag)
		{
		
				// Classify the requested areas.	First load the maximum				
				// likelihood variable structure with need pointers					
		
		classifierVar.chanMeanPtr = 			classifyChanMeanPtr;
		classifierVar.covariancePtr = 		classifyCovPtr;
		classifierVar.classConstantPtr = 	classConstantPtr;
		
		if (determinantOKFlag)
			{
					// If thresholding is to be used, get table of threshold 		
					// values corresponding to the cosine of each angle from 1 to 90
					// degrees.								

			if (gClassifySpecsPtr->createThresholdTableFlag)
				continueFlag = CreateThresholdTables (0 ,0);
													
			if (continueFlag)
				ClassifyAreasControl (fileInfoPtr, &classifierVar);
			
			}	// end "determinantOK" 
				
		else	// !determinantOKFlag 
			{
					// List message indicating that classification was cancelled	
					// because covariance matrix for at least one class could not	
					// be inverted.																
		
			continueFlag = ListSpecifiedStringNumber (kClassifyStrID, 
																	IDS_Classify4,
																	(unsigned char*)gTextString, 
																	resultsFileStreamPtr, 
																	gOutputForce1Code,
																	continueFlag);
			
			SysBeep (10);
			
			}	// else !determinantOKFlag 
			
		}	// end "if (continueFlag)" 
			
			// Release memory for class mean vectors, class covariance				
			// matrices, matrix inversion work areas, and class constants.										
			
	CheckAndDisposePtr (classifyChanMeanPtr);
	CheckAndDisposePtr (classifyCovPtr);
	CheckAndDisposePtr (classConstantPtr);

}	// end "CorrelationClsfierControl" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		UInt32 CorrelationThresholdClass
//
//	Software purpose:	The purpose of this routine is to determine the threshold class
//							for the given correlation value.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/13/1998
//	Revised By:			Larry L. Biehl			Date: 02/07/2018

UInt32 CorrelationThresholdClass (
				double								dValue,
				double*								thresholdTablePtr)
	
{							
	UInt16								offsetCorrelation[7] = {45, 22, 11, 6, 3, 2, 1};
	
	UInt16*								offsetPtr;
	
	UInt32								index,
											step;
		
   
   index = 0;
	if (thresholdTablePtr != NULL) 
		{
				// Set up local variables.														

		offsetPtr = offsetCorrelation;
		
	   		// Get the threshold table index.										
	   			
		index = *offsetPtr;
		offsetPtr++;
		for (step=1; step<=6; step++)
			{
	   	if (dValue >= thresholdTablePtr[index])
	   		index += *offsetPtr;
	   			
	   	else	// dValue < thresholdTablePtr[index] 
	   		index -= *offsetPtr;
	   		
	   	offsetPtr++;
	   		
	   	}	// end "for (step=1; step<=6; step++)" 
		
		index++;
			
		}	// end "if (thresholdTablePtr != NULL)" 
   	
   return (index);

}	// end "CorrelationThresholdClass" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CreateChiSquareThresholdTable
//
//	Software purpose:	The purpose of this routine is to create the
//							chi square threshold table to be used to in creating
//							the classification probability map.
//
//	Parameters in:			
//
//	Parameters out:	None
//
// Value Returned: 		
//
// Called By:			LoadThresholdTable in SClassify.cpp
//							LoadThresholdTable in SClassify.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/17/1990
//	Revised By:			Larry L. Biehl			Date: 12/21/2016	

Boolean CreateChiSquareThresholdTable (
				SInt32 								degreesOfFreedom, 
				double*								thresholdTablePtr)
									
{                    
	double								chiSquaredValue,
											factor1,
											factor2,
											oneOverGammaOfHalfDF,
											probability,
											probabilityStep;
	
	SInt16								index,
											indexChange,
											numberTableElements;
											
	Boolean								listChiSquaredTableFlag;
	

			// Get constants for calculations that only depend on the number of 	
			// degrees of freedom.																
	
	ComputeChiSquaredConstants (degreesOfFreedom, 
											&factor1, 
											&factor2, 
											&oneOverGammaOfHalfDF);
			
			// List header for the chi-squared table to the output text window.	
	
	//listChiSquaredTableFlag = gTestFlag;
	listChiSquaredTableFlag = FALSE;
	if (listChiSquaredTableFlag)
		{	
		if (!ListSpecifiedStringNumber (kClassifyStrID, 
													IDS_Classify6,
													(unsigned char*)gTextString3, 
													NULL, 
													gOutputForce1Code,
													TRUE))
																							return (FALSE);
		
		}	// end "if (listChiSquaredTableFlag)" 
					
			// Intialize the nextTime variable to indicate when the next check	
			// should occur for a command-.													
			
	gNextTime = TickCount ();
	
			// Get Table of threshold values corresponding to fractions				
			// rejected from 0.1 to 1.0 percent by 0.1 percent 						
			// increments and from 1.0 to 99 percent by 1.0 percent					
			// increments.																			
			
	probabilityStep = 1e-16;
	chiSquaredValue = 0.;
	numberTableElements = gNumberProbabilityClasses - 1;
	thresholdTablePtr[0] = 0;
	indexChange = 13;
	
	for (index=1; index<=numberTableElements; index++)
		{
		if (index <= indexChange)
			{
			probabilityStep *= 10;
			probability = 1.0 - probabilityStep;
				
			}	// end "if (index <= indexChange)" 
			
		else	// index > indexChange 
			{
			probability -= probabilityStep;
			
			if (index == 22)
				probabilityStep = 0.01;
				
			if (index == 120)
				probabilityStep = 0.01;
				
			}	// end "else index > indexChange" 
		
		chiSquaredValue = ComputeChiSquaredValue (degreesOfFreedom, 
																factor1,
																factor2,
																oneOverGammaOfHalfDF,
																probability);
		
				// The value saved in the table is r**2/2.								
			
		thresholdTablePtr[index] = chiSquaredValue/2;
		
		if (listChiSquaredTableFlag)
			{		
					// Print the chi-squared value to the output text window.		
					
			sprintf ((char*)gTextString3, 
							" %3d    %18.16f %15.7f%s", 
							index, 
							probability, 
							chiSquaredValue,
							gEndOfLine);
			if (!ListString (
					(char*)gTextString3,(UInt32)strlen ((char*)gTextString3),gOutputTextH))
																							return (FALSE);
				
					// Exit routine if user has "command period" down				
																					
			}	// end "if (listChiSquaredTableFlag)" 
			
		if (TickCount () >= gNextTime)
			{
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
																							return (FALSE);
					
			}	// end "if (TickCount () >= nextTime)" 
					
		}	// end "for (index=1; index<numberTableElements; index++)" 
		
	return (TRUE);
			
}	// end "CreateChiSquareThresholdTable" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CreateCorrelationThresholdTable
//
//	Software purpose:	The purpose of this routine is to create the
//							chi square threshold table to be used to in creating
//							the classification probability map.
//
//	Parameters in:			
//
//	Parameters out:	None
//
// Value Returned: 		
//
// Called By:			CreateThresholdTable
//							LoadThresholdTable
//
//	Coded By:			Larry L. Biehl			Date: 01/13/1998
//	Revised By:			Larry L. Biehl			Date: 01/13/1998

Boolean CreateCorrelationThresholdTable (
				double*								thresholdTablePtr)
									
{                    
	UInt32								index,
											numberTableElements;
											
											
			// Get Table of threshold values corresponding to the cosine of each degree
			// from 1 to 90 degrees.																			
			
	numberTableElements = 91;
	thresholdTablePtr[0] = 0;
	
	for (index=0; index<numberTableElements; index++)
		{
		thresholdTablePtr[index] = cos ((double)(90-index) * kDegreesToRadians);
					
		}	// end "for (index=1; index<numberTableElements; index++)" 
		
	return (TRUE);
			
}	// end "CreateCorrelationThresholdTable"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CreateSupportVectorMachineModel
//
//	Software purpose:	The purpose of this routine is to create any needed
//							trailer files that are associated with the output
//							classification files.
//
//	Parameters in:
//
//	Parameters out:	None
//
// Value Returned:
//
// Called By:			ClassifyAreasControl in SClassify.cpp
//
//	Coded By:			Tsung Tai Yeh			Date: 09/??/2019
//	Revised By:			Larry L. Biehl			Date: 09/20/2019

Boolean CreateSupportVectorMachineModel (
				ClassifySpecsPtr					classifySpecsPtr)

{
	struct svm_node*					x_space;
	struct svm_parameter 			param;
	struct svm_problem 				prob;
	
	double*								inputDataVectorPtr;
	
	double								totalProbability;
	
   SInt16                        *channelsPtr,
   										*classVectorPtr;
	
	UInt32								baseIndex,
											feat,
											index,
											numberChannels,
											numberClasses,
											numberPixels,
											numberProjectChannels,
											pixelCount;
	
	Boolean								continueFlag = TRUE,
											returnFlag = TRUE;
	
	
			// Set SVM training weight parameters

	param.svm_type = classifySpecsPtr->svm_type;
	param.kernel_type = classifySpecsPtr->svm_kernel_type;
	param.degree = classifySpecsPtr->svm_degree;
	
			// Recommendation for gamma is 1/numberChannels which does cause nearly
			// perfect training but not very good test. Currently 0.0001 is used.
	param.gamma = classifySpecsPtr->svm_gamma;
	
	param.coef0 = classifySpecsPtr->svm_coef0;	// 0;
	param.nu = classifySpecsPtr->svm_nu;	// 0.5;
	param.cache_size = classifySpecsPtr->svm_cache_size;	//100;
	param.C = classifySpecsPtr->svm_cost;
	param.eps = classifySpecsPtr->svm_eps;	// 0.1;
	param.p = classifySpecsPtr->svm_p;	// 0.0001;
	param.shrinking = classifySpecsPtr->svm_shrinking;	// 1;
	param.probability = classifySpecsPtr->svm_probability;	// 0;

			// Initialize class weight parameters.
	
	param.nr_weight = 0;
	param.weight_label = NULL;
	param.weight = NULL;

	numberPixels = gProjectInfoPtr->knnCounter;
	numberChannels = classifySpecsPtr->numberChannels;
	channelsPtr = (SInt16*)GetHandlePointer (classifySpecsPtr->featureHandle);
   numberProjectChannels = gProjectInfoPtr->numberStatisticsChannels;
   classVectorPtr = classifySpecsPtr->classVectorPtr;
   numberClasses = classifySpecsPtr->numberClasses;
	
	if (gProjectInfoPtr->svmModel != NULL)
		{
		svm_free_and_destroy_model (&gProjectInfoPtr->svmModel);
		gProjectInfoPtr->svmModel = NULL;
		
		}	// end "if (gProjectInfoPtr->svmModel != NULL)"
	
	if (gProjectInfoPtr->svm_x != NULL)
		{
		free (gProjectInfoPtr->svm_x);
		gProjectInfoPtr->svm_x = NULL;
		
		}	// end "if (gProjectInfoPtr->svm_x != NULL)"
	
	prob.x = NULL;
	prob.y = NULL;
	x_space = NULL;
	
	prob.y = (double*)malloc (numberPixels * sizeof (double));
	
	if (prob.y != NULL)
		prob.x = (struct svm_node**)malloc (numberPixels * sizeof (struct svm_node*));
	
	if (prob.x != NULL)
		x_space = (struct svm_node*)malloc (
								numberPixels * (numberChannels+1) * sizeof (struct svm_node));
		 
	returnFlag = (x_space != NULL);
	
	if (returnFlag)
		{
				// Set up class weights if this is for the C-SVC model and the weights are
				// not equal
		
		if (classifySpecsPtr->svm_type == 0 &&
					gProjectInfoPtr->classWeightSet == kUnequalWeights)
			{
			param.nr_weight = numberClasses;
			param.weight_label =
							(int*)malloc (numberClasses * sizeof (int));
			
			if (param.weight_label != NULL)
				param.weight =
						(double*)malloc (numberClasses * sizeof (double));
			
			returnFlag = (param.weight != NULL);
			
			if (returnFlag)
				{
						// Load weight vectors
				
				totalProbability = GetTotalProbability (
														(UInt16*)classifySpecsPtr->classPtr,
														(SInt16)numberClasses,
														0);	// code for unequal weights
				
				for (int i=0; i<numberClasses; i++)
					{
					param.weight_label[i] = classifySpecsPtr->classPtr[i];
					
					param.weight[i] = GetClassWeightValue (
														(SInt16)classifySpecsPtr->classPtr[i]-1,
														0,	// code for unequal weights
														totalProbability);
				
					}	// end "for (int i=0; i<numberClasses; i++)"
				
				}	// end "if (returnFlag)"
			
			}	// end "if (classifySpecsPtr->svm_type == 0 && ..."
		
		}	// end "if (returnFlag)"
	
	if (returnFlag)
		{
		continueFlag = ListSpecifiedStringNumber (
														kClassifyStrID,
														IDS_Classify95,
														(unsigned char*)gTextString,
														NULL,
														gOutputForce1Code,
														continueFlag);
		
				// Present info in status dialog box indicating that SVM model is being
				// created

		if (gStatusDialogPtr != NULL)
			{
					// Load "Creating SVM model" in the status dialog
			
			int stringNumber = IDS_Project91;
			LoadDItemStringNumber (kProjectStrID,
											stringNumber,
											gStatusDialogPtr,
											IDC_Status11,
											(Str255*)gTextString);
			
			//HideStatusDialogItemSet (kStatusField);
			//HideStatusDialogItemSet (kStatusLine);
			//HideStatusDialogItemSet (kStatusCommand);
			//HideStatusDialogItemSet (kStatusClassA);
			
			//ShowDialogWindow (gStatusDialogPtr,
			//						kUpdateStatsInfoID,
			//						kDoNotSetUpDFilterTable);
			CheckSomeEvents (updateMask + activMask);
			
			}	// end "if (gStatusDialogPtr != NULL)"
		
				// Set print string routine for SVM routines.
		
		svm_set_print_string_function (&ListString);
		
				// Copy the labels
		
		int jj = 0;
		baseIndex = 0;
		pixelCount = 0;
		inputDataVectorPtr = gProjectInfoPtr->knnDataValuesPtr;
		for (int i=0; i<numberPixels; i++)
			{
			if (classVectorPtr[gProjectInfoPtr->knnLabelsPtr[i]])
				{
				prob.y[pixelCount] = (double)(gProjectInfoPtr->knnLabelsPtr[i]);
				prob.x[pixelCount] = &x_space[jj];
				for (int j=0; j<numberChannels; j++)
					{
							// Copy index and value of features in each channel
					
					index = baseIndex + channelsPtr[j];
					x_space[jj].index = j + 1;
					x_space[jj].value = inputDataVectorPtr[index];
					++jj;
					//inputDataVectorPtr++;
					
					}	// end "for (int j=0; j<numberChannels; j++)"
				
				x_space[jj++].index = -1;
				
				pixelCount++;
				
				}	// end "if (classVectorPtr[gProjectInfoPtr->knnLabelsPtr[i]])"
				
			baseIndex += numberProjectChannels;
			
			}	// end "for (int i=0; i<numberPixels; i++))"
		
				// SVM training
		
		prob.l = pixelCount;
		gProjectInfoPtr->svmModel = svm_train (&prob, &param);
		returnFlag = (gProjectInfoPtr->svmModel != NULL);
	
				// Insert a blank line after the SVM training output.
		
		continueFlag = OutputString (NULL,
												gEndOfLine,
												gNumberOfEndOfLineCharacters,
												gOutputForce1Code,
												continueFlag);
		
		}	// end "if (returnFlag)"

			// Get memory to use for classification
	
	if (returnFlag)
		{
		gProjectInfoPtr->svm_x =
				(struct svm_node*)malloc ((numberChannels+1)* sizeof (struct svm_node));
		returnFlag = (gProjectInfoPtr->svm_x != NULL);
	
		if (returnFlag)
			{
					// Only need to load these values one time.
			
			for (feat=0; feat<numberChannels; feat++)
				gProjectInfoPtr->svm_x[feat].index = feat + 1;
		
					// the end index of one channel = -1
		
			gProjectInfoPtr->svm_x[numberChannels].index = -1;
			
			}	// end "if (returnFlag)"
	
		}	// end "if (returnFlag)"

	classifySpecsPtr->supportVectorMachineModelAvailableFlag = returnFlag;
	
			// Free some local vectors.
	
	free (prob.x);
	free (prob.y);
	if (param.weight_label != NULL)
		free (param.weight_label);
	if (param.weight != NULL)
		free (param.weight);
	
			// Free memory if needed
	
	if (!returnFlag)
		{
		if (gProjectInfoPtr->svm_x)
			free (gProjectInfoPtr->svm_x);
		
		}	// end "if (!returnFlag)"
	
	return (returnFlag);
	
}	// end "CreateSupportVectorMachineModel"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CreateThresholdTables
//
//	Software purpose:	The purpose of this routine is to create the 
//							threshold tables.
//
//	Parameters in:		Number of degrees of freedom to use in creating the table.
//
//	Parameters out:	None
//
// Value Returned:  	TRUE if tables were set up okay
//							FALSE if not.
//
// Called By:			FisherClsfierControl
//							MaxLikeClsfierControl
//							EchoLikeClsfierControl in SClassifyEchoControl.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/27/1993
//	Revised By:			Larry L. Biehl			Date: 01/13/1998

Boolean CreateThresholdTables (
				SInt16								pixelDegreesOfFreedom, 
				SInt16								cellDegreesOfFreedom)

{
	Boolean								continueFlag;
	
	
			// Put description in the status dialog.										
			
	LoadDItemStringNumber (kClassifyStrID, 
									IDS_Classify18, 
									gStatusDialogPtr, 
									IDC_Status11, 
									(Str255*)gTextString);
			
	continueFlag = GetThresholdTableMemory ();
	
	if (continueFlag)
		{
		if (gClassifySpecsPtr->mode == kCorrelationMode)
			continueFlag = CreateCorrelationThresholdTable (
															gClassifySpecsPtr->thresholdTablePtr);
		
		else	// gClassifySpecsPtr->mode != kCorrelationMode
			{	
			continueFlag = LoadThresholdTable (pixelDegreesOfFreedom, 1);
		
			if (continueFlag && (gClassifySpecsPtr->mode == kEchoMode))	
				continueFlag = LoadThresholdTable (cellDegreesOfFreedom, 2);
		
			if (continueFlag)	
				continueFlag = LoadProbabilityTable ();
				
			}	// end "else gClassifySpecsPtr->mode != kCorrelationMode"
			
		}	// end "if (continueFlag)"
		
	return (continueFlag);
			
}	// end "CreateThresholdTables" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CreateTrailerFiles
//
//	Software purpose:	The purpose of this routine is to create any needed
//							trailer files that are associated with the output
//							classification files.
//
//	Parameters in:			
//
//	Parameters out:	None
//
// Value Returned:		
//
// Called By:			ClassifyAreasControl
//
//	Coded By:			Larry L. Biehl			Date: 04/07/1992
//	Revised By:			Larry L. Biehl			Date: 04/15/2020

Boolean CreateTrailerFiles (
				ClassifierVarPtr					clsfyVariablePtr, 
				SInt16								numberClasses, 
				SInt16*								classPtr,
				SInt16								numberListClasses)

{
	FileInfoPtr							resultsFilePtr;
	
	UInt32								supportFileType;
	
	Boolean								continueFlag;
	
	
	continueFlag = TRUE;
	
			// Create the support file.
	
	if (gOutputFormatCode == kErdas74Type || gOutputFormatCode == kTIFFType)
		{
		supportFileType = kITRLFileType;
		if (gOutputFormatCode != kErdas74Type)
			supportFileType = kICLRFileType;											
			
		resultsFilePtr = GetResultsFilePtr (0);
		continueFlag = CreateThematicSupportFile (
												resultsFilePtr,
												NULL,
												numberClasses,
												classPtr,
												numberListClasses,
												NULL,
												NULL,
												NULL,
												gProjectInfoPtr->imagePalettePopupMenuSelection,
												numberClasses,
												kClassifyFileCode,
												kPaletteHistogramClassNames,
												kClassDisplay,
												kCollapseClass,
												supportFileType);
		
		}	// end "if (gOutputFormatCode == kErdas74Type || ...)" 
					
			// Create theERDAS Trailer file for the probability map if		
			// needed.																		
							
	if (continueFlag && (gOutputCode & kProbFormatCode))
		{
		resultsFilePtr = GetResultsFilePtr (1);
		
		supportFileType = kITRLFileType;
		if (resultsFilePtr->format != kErdas74Type)
			supportFileType = kICLRFileType;	
		
		SInt16 probabilityColors = kProbablilityColors;
		if (gClassifySpecsPtr->mode == kCEMMode)
			probabilityColors = kProbablilityColors2;
		else if (gClassifySpecsPtr->mode == kKNearestNeighborMode)
			probabilityColors = kClassify_Info_Colors;
			
		continueFlag = CreateThematicSupportFile (
										resultsFilePtr,
										NULL, 
										(UInt16)gClassifySpecsPtr->numberProbabilityClasses,
										NULL,
										0, 
										NULL,
										NULL,
										NULL, 
										probabilityColors,
										(UInt16)gClassifySpecsPtr->numberProbabilityClasses,
										kProbFormatCode,
										kPaletteHistogramClassNames,
										kClassDisplay,
										kCollapseClass,
										supportFileType);
									
		}	// end "if (... && (gOutputCode & kProbFormatCode))" 
					
			// Create theERDAS Trailer file for the echo fields map if		
			// needed.																		
							
	if (continueFlag && (gOutputCode & kEchoFieldsCode))
		{
		resultsFilePtr = GetResultsFilePtr (2);
		
				// Only generate a trailer file if the number of fields is less
				// than the global class legend display limit.
		
		if (resultsFilePtr->numberClasses <= gClassListLimit)
			{
			supportFileType = kITRLFileType;
			if (resultsFilePtr->format != kErdas74Type)
				supportFileType = kICLRFileType;
			
			continueFlag = CreateThematicSupportFile (
														resultsFilePtr,
														NULL, 
														(UInt16)(resultsFilePtr->numberClasses-1), 
														NULL,
														0,
														NULL,
														NULL,
														NULL, 
														kDefaultColors,
														(UInt16)(resultsFilePtr->numberClasses-1), 
														kEchoFieldsCode,
														kPaletteHistogramClassNames,
														kClassDisplay,
														kCollapseClass,
														supportFileType);
			
			}	// end "if (resultsFilePtr->numberClasses <= gClassListLimit)"
									
		}	// end "if (... && (gOutputCode & kEchoFieldsCode))" 
					
			// Create theERDAS Trailer file for the echo classified fields 
			// map if needed.																
							
	if (continueFlag && (gOutputCode & kEchoClassesCode))
		{
		resultsFilePtr = GetResultsFilePtr (3);
		
		supportFileType = kITRLFileType;
		if (resultsFilePtr->format != kErdas74Type)
			supportFileType = kICLRFileType;	
			
		continueFlag = CreateThematicSupportFile (
												resultsFilePtr,
												NULL, 
												numberClasses,
												classPtr,
												numberListClasses,
												NULL,
												NULL,
												NULL,
												gProjectInfoPtr->imagePalettePopupMenuSelection,
												numberClasses,
												kEchoClassesCode,
												kPaletteHistogramClassNames,
												kClassDisplay,
												kCollapseClass,
												supportFileType);
		
		}	// end "if (... && (gOutputCode & kEchoClassesCode))" 
		
	return (continueFlag);
		
}	// end "CreateTrailerFiles"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void EuclideanClassifier
//
//	Software purpose:	The purpose of this routine is to classify the
//							input line of data using the per-point Euclidean minimum
//							distance classifer.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			ClassifyArea
//
//	Coded By:			Larry L. Biehl			Date: 12/20/1988
//	Revised By:			Larry L. Biehl			Date: 03/27/2012

SInt16 EuclideanClassifier (
				AreaDescriptionPtr				areaDescriptionPtr, 
				FileInfoPtr							fileInfoPtr,
				ClassifierVarPtr					clsfyVariablePtr, 
				HUCharPtr							outputBuffer1Ptr, 
				HUCharPtr							probabilityBufferPtr, 
				HSInt64Ptr							countVectorPtr, 
				Point									point)

{
	double								discriminantMax,
											dValue1,
											rrDivide2;
										
	RgnHandle							rgnHandle;
	
	HDoublePtr							ioBufferReal8Ptr,
											savedChanMeanPtr,
											savedBufferReal8Ptr;
	
	double								*chanMeanPtr;
	
	SInt16								*classPtr;
	
	UInt32								classIndex,
											feat,
											maxClass,
											numberChannels,
											numberClasses,
											numberSamplesPerChan,
											sameDistanceCount,
											sample;
												
	Boolean								polygonField;
	
	
			// Initialize local variables.														
	
	numberChannels = 			gClassifySpecsPtr->numberChannels;
	numberClasses = 			gClassifySpecsPtr->numberClasses;
	numberSamplesPerChan = 	(UInt32)areaDescriptionPtr->numSamplesPerChan;
	rgnHandle = 				areaDescriptionPtr->rgnHandle;
	polygonField =				areaDescriptionPtr->polygonFieldFlag;
	classPtr = gClassifySpecsPtr->classPtr;
	chanMeanPtr = clsfyVariablePtr->chanMeanPtr;
	savedChanMeanPtr = chanMeanPtr;
	savedBufferReal8Ptr = (HDoublePtr)outputBuffer1Ptr;
		
			// Loop through the number of samples in the line of data				
		
	maxClass = 0;
	for (sample=0; sample<numberSamplesPerChan; sample++)
		{
		if (!polygonField || PtInRgn (point, rgnHandle))
			{
					// Loop through the classes and get the discriminant value for	
					// each class.																	
		
			discriminantMax = -DBL_MAX;
			chanMeanPtr = savedChanMeanPtr;
				
			for (classIndex=0; classIndex<numberClasses; classIndex++)
				{
				ioBufferReal8Ptr = savedBufferReal8Ptr;
				
					
						// Compute discriminant value for class "statClassNumber".		
						// Subtract off the channel mean for the channel and square		
						// the distance. The discriminant is the sum of the squares.	
				
				rrDivide2 = 0;
				for (feat=0; feat<numberChannels; feat++)
					{
					dValue1 = *ioBufferReal8Ptr - *chanMeanPtr;
					rrDivide2 += dValue1 * dValue1;
					
					ioBufferReal8Ptr++;
					chanMeanPtr++;
					
					}	// end "for (feat=0; feat<..." 
				
				rrDivide2 *= .5;
				
				if (clsfyVariablePtr->useLeaveOneOutMethodFlag && 
										classPtr[classIndex] == areaDescriptionPtr->classNumber)
					rrDivide2 *= clsfyVariablePtr->classConstantLOO1Ptr[classIndex];
					
				dValue1 = -rrDivide2;
				
						// Check if discriminant value for this class is smallest.  
						// If so, save the value and the class number.					
						
				if (dValue1 > discriminantMax)
					{
					discriminantMax = dValue1;
					maxClass = classIndex;
					sameDistanceCount = 0;
					
					}	// end "if (dValue < discriminantMin)"
					
				else if (dValue1 == discriminantMax)
					sameDistanceCount++;
	
				}	// end "for (classIndex=0; classIndex<..." 
				
		   countVectorPtr[classPtr[maxClass]]++;
		   *outputBuffer1Ptr = (UInt8)classPtr[maxClass];
		   
		   }	// end "if (!polygonField || PtInRgn (point, rgnHandle))" 
		   
		else	// polygonField && !PtInRgn (point, rgnHandle) 
			{
			*outputBuffer1Ptr = 0; 
			sameDistanceCount = 0;
			
			}	// end "else polygonField && !PtInRgn (point, rgnHandle)"
			
		point.h++;
		outputBuffer1Ptr++;
		savedBufferReal8Ptr += numberChannels;
		
		clsfyVariablePtr->totalSameDistanceSamples += sameDistanceCount;
			
				// Exit routine if user selects "cancel" or "command period".		
		
		if (TickCount () >= gNextTime)
			{
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
																								return (3);
				
			}	// end "if (TickCount () >= gNextTime)" 
				
		}	// end "for (sample=0; sample<..." 
	
			// Write carriage return and null character to classification record	
			
   *outputBuffer1Ptr = kCarriageReturn;
   outputBuffer1Ptr++;    
                      
	#if defined multispec_win
   	*outputBuffer1Ptr = kLineFeed;
  		outputBuffer1Ptr++;
	#endif	// defined multispec_win  
	
   *outputBuffer1Ptr = kNullTerminator;
   
   return (0);

}	// end "EuclideanClassifier" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void EuclideanClsfierControl
//
//	Software purpose:	The purpose of this routine is to control the
//							classification of the specified data using the point 
//							Euclidean distance classifier.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			ClassifyPerPointArea
//
//	Coded By:			Larry L. Biehl			Date: 12/20/1988
//	Revised By:			Larry L. Biehl			Date: 03/27/2012

void EuclideanClsfierControl (
				FileInfoPtr							fileInfoPtr)

{
	double*								classConstantLOO1Ptr;
											
	HChannelStatisticsPtr			classChannelStatsPtr;
	ClassifierVar						classifierVar;
	
	HDoublePtr 							classifyChanMeanPtr;
	
	UInt32								clsfyChanMeanStart,
											classIndex,
											numberClsfyChannels,
											statClassNumber;
											
	Boolean								continueFlag;
	
		
			// Initialize local variables														
			
	numberClsfyChannels = gClassifySpecsPtr->numberChannels;
	
	InitializeClassifierVarStructure (&classifierVar);
	
			// Get the memory for the mean vectors and covariance matrices			
			// for the classes and channels to be used in the classification.		
				 
	continueFlag = SetupClsfierMemory (&classifyChanMeanPtr,
													NULL,
													NULL,
													&classConstantLOO1Ptr,
													NULL,
													NULL,
													NULL,
													NULL, 
													&classChannelStatsPtr, 
													NULL, 
													NULL, 
													NULL, 
													NULL, 
													NULL);
	
			// Continue with classification, if memory is not full.					
	
	if (continueFlag)
		{
				// Loop through the classes and for classes to be used in 			
				// classification get the:														
				//		mean vector constant.													
		
		clsfyChanMeanStart = 0;											
			
		UInt16* featurePtr = (UInt16*)GetHandlePointer (
																gClassifySpecsPtr->featureHandle);
		
		for (classIndex=0; 
				classIndex<gClassifySpecsPtr->numberClasses; 
					classIndex++)
			{
			statClassNumber = gClassifySpecsPtr->classPtr[classIndex] - 1;
							
					// Get the class mean vector	
					
			GetClassMeanVector ((UInt16)numberClsfyChannels,
										classChannelStatsPtr, 
										&classifyChanMeanPtr[clsfyChanMeanStart], 
										featurePtr, 
										(UInt16)statClassNumber);
			
			clsfyChanMeanStart += numberClsfyChannels;
		
					// Variables to use in case the leave-one-out method is used to
					// compute the training statistics.
										
			UInt16 classStorage = gProjectInfoPtr->storageClass[statClassNumber];
			SInt64 numberTrainPixels = 
						gProjectInfoPtr->classNamesPtr[classStorage].numberStatisticsPixels;
			
			if (numberTrainPixels > 1)
				classConstantLOO1Ptr [classIndex] = 
								(double)numberTrainPixels*numberTrainPixels/
													((numberTrainPixels-1)*(numberTrainPixels-1));
												
			else	// numberTrainPixels <= 1
				classConstantLOO1Ptr [classIndex] = 0;
				
			}	// end "for (classIndex=0; classIndex<Classify..." 
			
				// Release memory for unneeded work areas.								
		
		classChannelStatsPtr = (HChannelStatisticsPtr)CheckAndDisposePtr (
																			(Ptr)classChannelStatsPtr);
		
				// Classify the requested areas.	First load the maximum				
				// likelihood variable structure with need pointers					
		
		classifierVar.chanMeanPtr = classifyChanMeanPtr;
		classifierVar.classConstantLOO1Ptr = classConstantLOO1Ptr;
		
		if (continueFlag)
			ClassifyAreasControl (fileInfoPtr, &classifierVar);
			
		}	// end "if (continueFlag)" 
			
			// Release memory.										
			
	CheckAndDisposePtr (classifyChanMeanPtr);
	CheckAndDisposePtr (classConstantLOO1Ptr);

}	// end "EuclideanClsfierControl" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void FisherClassifier
//
//	Software purpose:	The purpose of this routine is to classify the
//							input line of data using the per-point fisher 
//							discriminant classifer.
//
//	Parameters in:		Pointer to area description structure
//							Pointer to image file information structure
//							Pointer to temporary classification variable structure
//							Pointer to buffer to be used to store the classification
//								results in for a line.
//							Pointer to a buffer to be used to store the probability
//								of correct classification information in for a line.
//							Pointer to a vector to be used to store the count of the
//								number of pixels classified into each class
//							A structure which defines the start line and column values
//								that the classification will begin at.	
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			ClassifyPerPointArea
//
//	Coded By:			Larry L. Biehl			Date: 03/29/1997
//	Revised By:			Larry L. Biehl			Date: 03/27/2012

SInt16 FisherClassifier (
				AreaDescriptionPtr				areaDescriptionPtr,
				FileInfoPtr							fileInfoPtr,
				ClassifierVarPtr					clsfyVariablePtr,
				HUCharPtr							outputBuffer1Ptr,
				HUCharPtr							probabilityBufferPtr,
				HSInt64Ptr							countVectorPtr,
				Point									point)

{
	Boolean								polygonField,
											createProbabilities;
	RgnHandle							rgnHandle;
	
	double								*classConstantPtr,
											*thresholdTablePtr;
	
	double								discriminantMax,
											dValue;
	
	SInt16								*classPtr,
											*thresholdProbabilityPtr;
	
	HDoublePtr							meanXInvCovPtr,
											ioBufferReal8Ptr,
											savedBufferReal8Ptr;
	
	UInt32								classIndex,
											feature,
											maxClass,
											numberChannels,
											numberClasses,
											numberSamplesPerChan,
											sameDistanceCount,
											sample;
	
	
			// Initialize local variables.														
	
	numberChannels = 			gClassifySpecsPtr->numberChannels;
	numberClasses = 			gClassifySpecsPtr->numberClasses;
	numberSamplesPerChan = 	(UInt32)areaDescriptionPtr->numSamplesPerChan;
	polygonField =				areaDescriptionPtr->polygonFieldFlag;
	rgnHandle = 				areaDescriptionPtr->rgnHandle;
	classPtr = 					gClassifySpecsPtr->classPtr;
	savedBufferReal8Ptr = 	(HDoublePtr)outputBuffer1Ptr;
	maxClass = 					0;
	
	createProbabilities = 	gClassifySpecsPtr->createThresholdTableFlag;
	
	classConstantPtr = 		clsfyVariablePtr->classConstantPtr;
	thresholdTablePtr =	 	gClassifySpecsPtr->thresholdTablePtr;
	thresholdProbabilityPtr = gClassifySpecsPtr->thresholdProbabilityPtr;
		
			// Loop through the number of samples in the line of data				
	
	for (sample=0; sample<numberSamplesPerChan; sample++)
		{
		if (!polygonField || PtInRgn (point, rgnHandle))
			{
					// Loop through the classes and get the discriminant value for	
					// each class.																	
		
			discriminantMax = -1e200;
			meanXInvCovPtr = clsfyVariablePtr->chanMeanPtr;
			
			for (classIndex=0; classIndex<numberClasses; classIndex++)
				{
						// Compute discriminant value for class "statClassNumber"
						
				ioBufferReal8Ptr = savedBufferReal8Ptr;
				dValue = 0.;
				for (feature=0; feature<numberChannels; feature++)
					{
					dValue += *ioBufferReal8Ptr * *meanXInvCovPtr;
					
					ioBufferReal8Ptr++;
					meanXInvCovPtr++;
					
					}	// end "for (feature=0; feature<..." 	
					
				dValue -= classConstantPtr[classIndex];
				
						// Check if discriminant value for this class is largest.  If	
						// so, save the value and the class number.							
						
				if (dValue > discriminantMax)
					{
					discriminantMax = dValue;
					maxClass = classIndex;
					sameDistanceCount = 0;
					
					}	// end "if (dValue < discriminantMin)" 
					
				else if (dValue == discriminantMax)
					sameDistanceCount++;
	
				}	// end "for (classIndex=0; classIndex<..."
		   
		   		// Get distance measure if requested.									
		   		// dValue1 is the variable for the probability.						
		   
		   if (createProbabilities)
		   	{
		   	double*		doubleDataPtr = clsfyVariablePtr->workVectorPtr;
		   	
		   			// Copy the data into the double vector.
		   			
				ioBufferReal8Ptr = savedBufferReal8Ptr;
				for (feature=0; feature<numberChannels; feature++)
					{
					*doubleDataPtr = (double)*ioBufferReal8Ptr;
					
					ioBufferReal8Ptr++;
					doubleDataPtr++;
					
					}	// end "for (feature=0; feature<..."
		   	
				TransformSymmetricMatrix (clsfyVariablePtr->workVectorPtr,
													clsfyVariablePtr->covariancePtr, 
													clsfyVariablePtr->workVector2Ptr, 
													&dValue, 
													1,
													(UInt16)numberChannels,
													kSquareOutputMatrix);
				
				discriminantMax += -.5 * dValue;
									
		   			// dValue represents r**2/2											
		   			
		   	dValue = clsfyVariablePtr->classConstant2Ptr[maxClass] - discriminantMax;
		   	
		   			// Get the threshold table index.									
					
				classIndex = GetThresholdClass (dValue, thresholdTablePtr);
				
				*probabilityBufferPtr = (UInt8)classIndex;
				probabilityBufferPtr++;
				
				gTempDoubleVariable1 += thresholdProbabilityPtr[classIndex];
				
				}	// end "if (createProbabilities)"  
				
			classIndex = classPtr[maxClass];
		   countVectorPtr[classIndex]++;
		   *outputBuffer1Ptr = (UInt8)classIndex;
		   
		   }	// end "if (!polygonField || PtInRgn (point, rgnHandle))" 
		   
		else	// polygonField && !PtInRgn (point, rgnHandle) 
			{
			*outputBuffer1Ptr = 0;
		   if (createProbabilities)
		   	{
				*probabilityBufferPtr = 0;
				probabilityBufferPtr++;
				
				}	// end "if (createProbabilities)"
				
			sameDistanceCount = 0;
			
			}	// end "else polygonField && !PtInRgn (point, rgnHandle)" 
			
		point.h++;
		savedBufferReal8Ptr += numberChannels;
		outputBuffer1Ptr++;
		
		clsfyVariablePtr->totalSameDistanceSamples += sameDistanceCount;
			
				// Exit routine if user selects "cancel" or "command period".		
		
		if (TickCount () >= gNextTime)
			{
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
																								return (3);
				
			}	// end "if (TickCount () >= gNextTime)" 
				
		}	// end "for (sample=0; sample<..." 
	
			// Write carriage return and null character to classification record	
			
   *outputBuffer1Ptr = kCarriageReturn;
   outputBuffer1Ptr++;  
                      
	#if defined multispec_win
   	*outputBuffer1Ptr = kLineFeed;
  		outputBuffer1Ptr++;
	#endif	// defined multispec_win  
	
   *outputBuffer1Ptr = kNullTerminator;
   
   return (0);

}	// end "FisherClassifier" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void FisherClsfierControl
//
//	Software purpose:	The purpose of this routine is to classify the
//							specified data using the point Fisher Discriminant 
//							classifer.
//
//	Parameters in:		Pointer to image file information structure
//
//	Parameters out:	None
//
// Value Returned:	Flag indicating whether output to text window
//								can still be generated.
// 
// Called By:			ClassifyControl
//
//	Coded By:			Larry L. Biehl			Date: 03/28/1997
//	Revised By:			Larry L. Biehl			Date: 06/17/2006

void FisherClsfierControl (
				FileInfoPtr							fileInfoPtr)

{
	Boolean								continueFlag;
	
	HChannelStatisticsPtr			classChannelStatsPtr;
	ClassifierVar						classifierVar;
	
	double								*classConstantPtr,
											logDeterminant,
											weightValue;
												
	HDoublePtr							classifyChanMeanPtr,
											classifyCovPtr,
											inverseCovPtr,
											tempChanMeanPtr;
										
	SInt16*								statFeaturePtr;
	
	float									totalProbability;
	
	UInt32								classIndex,
											clsfyChanMeanStart,
											numberFeatureChannels,
											numberFeatures,
											statClassNumber;
	
	SInt16								weightsIndex;
						
	CMFileStream*						resultsFileStreamPtr;
	
	Boolean								determinantOKFlag;
	
		
			// Initialize local variables														
	
	continueFlag = TRUE;
	numberFeatureChannels = gClassifySpecsPtr->numberChannels;
	numberFeatures = gClassifySpecsPtr->numberFeatures;
 	resultsFileStreamPtr = GetResultsFileStreamPtr (0);
	
			// Put description in the status dialog.										
					
	LoadDItemStringNumber (kClassifyStrID, 
									IDS_Classify20, 
									gStatusDialogPtr, 
									IDC_Status11, 
									(Str255*)gTextString);
		
	InitializeClassifierVarStructure (&classifierVar);
	
			// Get the memory for the mean vectors and covariance matrices			
			// for the classes and channels to be used in the classification.		
				 
	continueFlag = SetupClsfierMemory (&tempChanMeanPtr,
													&classifyCovPtr,
													&classConstantPtr,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL, 
													&classChannelStatsPtr, 
													&gInverseMatrixMemory.inversePtr,
													&gInverseMatrixMemory.pivotPtr, 
													&gInverseMatrixMemory.indexRowPtr, 
													&gInverseMatrixMemory.indexColPtr, 
													&gInverseMatrixMemory.ipvotPtr);
	inverseCovPtr = gInverseMatrixMemory.inversePtr;
											
	UInt16* featurePtr = (UInt16*)GetHandlePointer (gClassifySpecsPtr->featureHandle);
		
	if (continueFlag)			
		continueFlag = SetupFeatureTransformationMemory (
													gClassifySpecsPtr->featureTransformationFlag,
													(SInt16)numberFeatures, 
													(SInt16)numberFeatureChannels, 
													&gTransformationMatrix.eigenVectorPtr, 
													&gTransformationMatrix.tempMatrixPtr,
													NULL, 
													&gTransformationMatrix.eigenFeaturePtr,
													kLoadMatricesVectors,
													featurePtr);
	
			// Continue with classification, if memory is not full.					
	
	if (continueFlag)
		{
				// Normalize a priori probabilities.										
				
		weightsIndex = GetProjectClassWeightsIndex ();
				
		totalProbability = (float)GetTotalProbability (
														(UInt16*)gClassifySpecsPtr->classPtr,
														(SInt16)gClassifySpecsPtr->numberClasses,
														weightsIndex);
		
		classifierVar.classConstant2Ptr = 
							(double*)&classConstantPtr [gClassifySpecsPtr->numberClasses];
		
				// Loop through the classes to be used in classification and get the:														
				//		mean vector for class.													
				//		common covariance matrix for all classes to be used.											
				//		inverse of common covariance matrix.							
				//		discriminant constant for class.	
				
				// The first part of the tempChanMean vector will be used
				// as a temporary vector. The last part is for storage of the
				// product of the M and Covariance matrices for each class.							
		
		clsfyChanMeanStart = 0;
		
		classifyChanMeanPtr = &tempChanMeanPtr [numberFeatureChannels];
		
		statFeaturePtr = (SInt16*)GetStatisticsFeatureVector (
													gClassifySpecsPtr->featureTransformationFlag,
													featurePtr);
										
		continueFlag = GetTtInvTransformedCommonCovarianceT (
																(SInt16)numberFeatureChannels,
																classChannelStatsPtr,
																inverseCovPtr,
																(UInt16*)statFeaturePtr,
																(UInt16*)gClassifySpecsPtr->classPtr,
																gClassifySpecsPtr->numberClasses,
																kSquareOutputMatrix,
																gTransformationMatrix.eigenVectorPtr,
																classifyCovPtr,
																(SInt16)numberFeatures, 
																&logDeterminant,
																&determinantOKFlag);
		
		}			// end "if (continueFlag)"
		
	if (continueFlag)
		{										
		if (determinantOKFlag)
			{			
			for (classIndex=0; 
					classIndex<gClassifySpecsPtr->numberClasses; 
						classIndex++)
				{
				statClassNumber = gClassifySpecsPtr->classPtr[classIndex] - 1;
		
						// Get the class mean vector
					
				GetClassMeanVector ((SInt16)numberFeatureChannels,
											classChannelStatsPtr, 
											tempChanMeanPtr, 
											(UInt16*)statFeaturePtr, 
											(SInt16)statClassNumber);
				
				if (gOperationCanceledFlag)
					{
					continueFlag = FALSE;
					break;
					
					}	// end "if (gOperationCanceledFlag)"
					
						// Get the constant offset for this class.
						
				MatrixMultiply (tempChanMeanPtr, 
										inverseCovPtr,
										classifyCovPtr, 
										1,
										(SInt16)numberFeatureChannels,
										(SInt16)numberFeatureChannels,
										kTransposeMat1ByMat2);
				
				MatrixMultiply (classifyCovPtr,
										tempChanMeanPtr,
										&classConstantPtr[classIndex], 
										1,
										(SInt16)numberFeatureChannels,
										1,
										kMat1ByMat2);
											
				classConstantPtr[classIndex] *= .5;
				
						// Now get InvCov_M or Tt_InvTCov_T_M
						
				MatrixMultiply (inverseCovPtr, 
										tempChanMeanPtr,
										&classifyChanMeanPtr[clsfyChanMeanStart],
										(SInt16)numberFeatureChannels,
										(SInt16)numberFeatureChannels, 
										1,
										kMat1ByMat2);	
				
						// Now get the value for the class constants provided by the
						// class weights.				
						
				weightValue = GetClassWeightValue (
										(SInt16)statClassNumber, weightsIndex, totalProbability);
										
				weightValue = log (weightValue);
				classConstantPtr[classIndex] -= weightValue;
				classifierVar.classConstant2Ptr[classIndex] = weightValue;
				
						// Ready index for the next class.
										
				clsfyChanMeanStart += numberFeatureChannels;
				
				}	// end "for (class=0; class<Classify..." 
							
			}	// end "if (determinantOKFlag)" 
					
		else	// !determinantOKFlag 
			{
					// List message that the common covariance matrix could not be inverted.												
				
			continueFlag = ListClassInformationMessage (kProjectStrID,
																		IDS_Project69,
																		resultsFileStreamPtr, 
																		gOutputForce1Code,
																		0, 
																		continueFlag);
			
					// Do not continue Fisher classification.
												
			continueFlag = FALSE;
			
			}	// end "else !determinantOKFlag" 
			
		}	// end "if (continueFlag)" 
			
			// Release memory for class covariance matrices.
			
	classifyCovPtr = (double*)CheckAndDisposePtr ((Ptr)classifyCovPtr);
	classChannelStatsPtr = (HChannelStatisticsPtr)CheckAndDisposePtr (
																		(Ptr)classChannelStatsPtr);
			
			// Release memory for the matrix inversion work areas.					
		
	gInverseMatrixMemory.indexRowPtr =
										CheckAndDisposePtr (gInverseMatrixMemory.indexRowPtr);
	gInverseMatrixMemory.indexColPtr =
										CheckAndDisposePtr (gInverseMatrixMemory.indexColPtr);
	gInverseMatrixMemory.ipvotPtr =
										CheckAndDisposePtr (gInverseMatrixMemory.ipvotPtr);
	
			// Release memory for transformation matrix information.					
			
	ReleaseFeatureTransformationMemory ();
	
	if (continueFlag)
		{
				// Classify the requested areas.	First load the maximum				
				// likelihood variable structure with need pointers					
		
		classifierVar.chanMeanPtr = 			classifyChanMeanPtr;
		classifierVar.covariancePtr = 		inverseCovPtr;
		classifierVar.classConstantPtr = 	classConstantPtr;
		classifierVar.workVectorPtr = 		gInverseMatrixMemory.pivotPtr;
		classifierVar.workVector2Ptr = 		tempChanMeanPtr; 
		
				// If thresholding is to be used, get table of threshold 		
				// values corresponding to fractions rejected from 0.1 to 		
				// 1.0 percent by 0.1 percent increments and from 1.0 to 		
				// 99 percent by 1.0 percent increments.								

		if (gClassifySpecsPtr->createThresholdTableFlag)
			continueFlag = CreateThresholdTables ((SInt16)numberFeatures, 0);
		
		if (continueFlag)
			ClassifyAreasControl (fileInfoPtr, &classifierVar);
			
		}	// end "if (continueFlag)" 
			
			// Release memory for class mean vectors, class covariance				
			// matrices, matrix inversion work areas
			
	CheckAndDisposePtr (tempChanMeanPtr);
	CheckAndDisposePtr (classConstantPtr);
	
	ReleaseMatrixInversionMemory ();

}	// end "FisherClsfierControl" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetClassifyDataBuffers
//
//	Software purpose:	The purpose of this routine is to obtain the buffer
//							to read the input image file data into, input buffer, 
//							and the buffer to pack the image data into, output
//							buffer.  For Classification
//							the input image data will be forced to be 4 byte, BIS
//							data with no unused data. If columns are skip, those
//							samples will not be packed.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			ClassifyAreasControl
//
//	Coded By:			Larry L. Biehl			Date: 08/18/1993
//	Revised By:			Larry L. Biehl			Date: 12/29/2005

Boolean GetClassifyDataBuffers (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				WindowInfoPtr						windowInfoPtr,
				LayerInfoPtr						layerInfoPtr,
				FileInfoPtr							fileInfoPtr, 
				EchoClassifierVarPtr				echoClassifierVarPtr,
				FileIOInstructionsPtr*			outputFileIOInstructionsPtrPtr)
	
{
	HUCharPtr							bufferPtr,
											tiledBufferPtr;
											
	UInt16* 								channelListPtr;
	
	UInt32								bufferOffset,
											endChannel,
											numberInputBytes,
											numberInputPackedBytes,
											numberOutputBytes,
											numberTileBytes,
											startChannel;
	
	Boolean								changedFlag,
											continueFlag;
	
	
			// Initialize local variables.
			
	numberTileBytes = 0;
	tiledBufferPtr = NULL;
	numberInputBytes =
						gImageWindowInfoPtr->maxNumberColumns * fileInfoPtr->numberBytes;
				
			// Get the pointer to the channel list. The handle is already locked.
													
	channelListPtr = (UInt16*)GetHandlePointer (gClassifySpecsPtr->channelsHandle);
						
	if (fileInfoPtr->bandInterleave == kBIL)
		{
				// Allow for possible BIL special case when more channels
				// are read in than actually used.
				
		startChannel = channelListPtr[0];
		endChannel = channelListPtr[gClassifySpecsPtr->numberChannels-1];
						
		numberInputBytes *= (endChannel - startChannel + 1);
		
		}	// end "if (fileInfoPtr->bandInterleave == kBIL)"
						
	else if (fileInfoPtr->bandInterleave == kBIS)
		numberInputBytes *= fileInfoPtr->numberChannels;
			
	else	// fileInfoPtr->bandInterleave != kBIS 
		numberInputBytes *= gClassifySpecsPtr->numberChannels;
		
			// The input data will be forced to 8 byte real, BIS format.					
	
	numberInputPackedBytes = gImageWindowInfoPtr->maxNumberColumns * 
															gClassifySpecsPtr->numberChannels * 8;
										
	if (echoClassifierVarPtr != NULL)		
		numberInputPackedBytes *= (SInt32)echoClassifierVarPtr->cell_width;
			
	numberInputBytes += numberInputPackedBytes;
	
			// Allow for the longest line which will be for MS Windows version. 
			// Carriage return, line feed, and null character.
					
	numberOutputBytes = gImageWindowInfoPtr->maxNumberColumns + 3;
	
	numberInputBytes = MAX (numberInputBytes, numberOutputBytes);
	
			// Force 'numberInputBytes' to be a multiple of 8, so that the
			// probability buffer offset below will start on a multiple of 8.
			
	numberInputBytes = 8 * ((numberInputBytes + 7)/8);
								
	if (gClassifySpecsPtr->createThresholdTableFlag)
		{
		gClassifySpecsPtr->probabilityBufferOffset = numberInputBytes;
		
		if (echoClassifierVarPtr)
					// This will allow the probabilities for one line of cells		
					// to be stored before listing to the probability file.			
					
			numberInputBytes += 
						echoClassifierVarPtr->cell_width * numberOutputBytes;
			
		else	// !echoClassifierVarPtr 
			numberInputBytes += numberOutputBytes;
		
		}	// end "if (gClassifySpecsPtr->createThresholdTableFlag)" 
		
	if (gOutputCode & kEchoFieldsCode)
		numberInputBytes = MAX (numberInputBytes, 2*numberOutputBytes);
	
	if (gOutputFormatCode == kGAIAType)	
		{
				// Force input bytes to be a multiple of 2.								
				
		numberInputBytes = ((numberInputBytes+1) / 2) * 2;
		gClassifySpecsPtr->outputBufferOffset = numberInputBytes/2;
			
		numberOutputBytes = GetNumberBytesInGAIALine (
															gImageWindowInfoPtr->maxNumberColumns);
		
		numberInputBytes += numberOutputBytes;
		
		}	// end "if (gOutputFormatCode == kGAIAType)" 
			
			// Force number of bytes to be a multiple of 8 bytes.
			
	numberInputBytes = 8 * ((numberInputBytes + 7)/8);
	
			// If Thread manager is present then set up to use threaded IO in
			// case it can be used.
	
	bufferOffset = 0;			
	if (gHasThreadManager)
		{
		bufferOffset = numberInputBytes;
		numberInputBytes *= 2;
		
		}	// end "if (gHasThreadManager)"
		
			// Get the number of bytes needed for tile file IO.
			// Force it to be a multiple of 8 bytes. 
			// For now tile buffer bytes will only be set up for the first buffer set.
		
	numberTileBytes = GetSetTiledIOBufferBytes (layerInfoPtr,
																fileInfoPtr, 
																channelListPtr, 
																gClassifySpecsPtr->numberChannels,
																NULL);
	
	continueFlag = CheckPointerSize ((Ptr*)&gOutputBufferPtr, 
													numberInputBytes+numberTileBytes,
													&changedFlag);
	
	if (continueFlag)
		{
		bufferPtr = (HUCharPtr)gOutputBufferPtr;
		gInputBufferPtr = (HUCharPtr)&bufferPtr[numberInputPackedBytes];
		
		if (numberTileBytes > 0)
			tiledBufferPtr = &bufferPtr[numberInputBytes];
		
		}	// end "if (continueFlag)" 
		
			// Load some of the File IO Instructions structure that pertain
			// to the overall image file being classified.
		
	if (continueFlag)
		SetUpGeneralFileIOInstructions (fileIOInstructionsPtr,
													windowInfoPtr,	
													layerInfoPtr,
													fileInfoPtr,
													gClassifySpecsPtr->numberChannels,
													channelListPtr,
													gInputBufferPtr,
													gOutputBufferPtr,
													tiledBufferPtr,
													bufferOffset,
													kPackData,
													kForceBISFormat,	
													kForceReal8Bytes,
													outputFileIOInstructionsPtrPtr);
													
	else	// !continueFlag
		*outputFileIOInstructionsPtrPtr = NULL;
		
	return (continueFlag);
			
}	// end "GetClassifyDataBuffers" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Handle GetTargetOverlayImageWindowInfoHandle
//
//	Software purpose:	The purpose of this routine is to get the image window 
//							information handle for target image window to be classified. It
//							may not be the same as the image window information handle being
//							used as the base for the project file.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/23/2006
//	Revised By:			Larry L. Biehl			Date: 03/23/2006

Handle GetTargetOverlayImageWindowInfoHandle (
				SInt16								fileNamesSelection,
				Handle								targetWindowInfoHandle)
	
{	
	Handle								targetOverlyWindowInfoHandle;
	
	
	if (fileNamesSelection <= 1)
		targetOverlyWindowInfoHandle = FindProjectBaseImageWindowInfoHandle ();	
		
	else	// fileNamesSelection >= 2
		targetOverlyWindowInfoHandle = targetWindowInfoHandle;	
	
	return (targetOverlyWindowInfoHandle);
			
}	// end "GetTargetOverlayImageWindowInfoHandle" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetThresholdClass
//
//	Software purpose:	The purpose of this routine is to determine the
//							probability class for the given likelihood value
//							and threshold table.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/26/1993
//	Revised By:			Larry L. Biehl			Date: 02/07/2018

UInt16 GetThresholdClass (
				double								dValue,
				double*								thresholdTablePtr)
	
{							
	UInt16								offsetChiSquared[7] = {60, 30, 15, 8, 4, 2, 1};
	
	UInt16*								offsetPtr;
	
	UInt32								index,
											step;
		

   index = 0;
	if (thresholdTablePtr != NULL)
		{
				// Set up local variables.														

		offsetPtr = offsetChiSquared;
		
	   		// Get the threshold table index.										
	   			
		index = *offsetPtr;
		offsetPtr++;
		for (step=1; step<=6; step++)
			{
	   	if (dValue < thresholdTablePtr[index])
	   		index += *offsetPtr;
	   			
	   	else	// dValue >= thresholdTablePtr[index] 
	   		index -= *offsetPtr;
	   		
	   	offsetPtr++;
	   		
	   	}	// end "for (step=1; step<=6; step++)" 
	
		index = MAX (1, index);
	   if (dValue < thresholdTablePtr[index])
			index++;
			
		}	// end "if (thresholdTablePtr != NULL)" 
   	
   return ((UInt16)index);

}	// end "GetThresholdClass" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetThresholdTableMemory
//
//	Software purpose:	The purpose of this routine is to get the
//							memory needed for the probability threshold tables.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:		
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 05/27/1993
//	Revised By:			Larry L. Biehl			Date: 01/13/1998

Boolean GetThresholdTableMemory (void)

{
	SInt32								bytesNeeded;
	
	Boolean								changedFlag,
											continueFlag;
	
								
			// Verify that memory is available for the threshold table.	
			// gNumberProbabilityClasses is 122 which is more than enough for the
			// correlation table if it is needed (91 classes).			
	
	bytesNeeded = 	gNumberProbabilityClasses * sizeof (double);
	if (gClassifySpecsPtr->mode == kEchoMode)
		bytesNeeded *= 2;
		
	gClassifySpecsPtr->thresholdTablePtr = (double*)CheckHandleSize (
															&gClassifySpecsPtr->thresholdTableHandle,
															&continueFlag, 
															&changedFlag, 
															bytesNeeded);
	
	if (continueFlag && gClassifySpecsPtr->mode != kCorrelationMode)
		{
		bytesNeeded = 	(gNumberProbabilityClasses + 1) * sizeof (SInt16);
		
		gClassifySpecsPtr->thresholdProbabilityPtr = (SInt16*)CheckHandleSize (
													&gClassifySpecsPtr->thresholdProbabilityHandle,
													&continueFlag, 
													&changedFlag, 
													bytesNeeded);
		
		}	// end "if (continueFlag && ..." 
		
	return (continueFlag);
			
}	// end "GetThresholdTableMemory" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetTransformedCommonCovariance
//
//	Software purpose:	The purpose of this routine is to compute the common
//							covariance for the input set of classes and class
//							weights.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/02/1997
//	Revised By:			Larry L. Biehl			Date: 07/13/2009

Boolean GetTransformedCommonCovariance (
				UInt16								numberOutputChannels,
				HChannelStatisticsPtr			classChannelStatsPtr,
				HDoublePtr							covariancePtr,
				UInt16*								channelListPtr,
				UInt16*								classPtr,
				UInt32								numberClasses,
				Boolean								squareOutputMatrixFlag,
				HDoublePtr							eigenVectorPtr,
				HDoublePtr							tempMatrixPtr,
				UInt16								numberFeatures)
	
{ 
	Boolean								continueFlag,
											savedZeroVarianceFlag;
									
									
	savedZeroVarianceFlag = gProjectInfoPtr->setZeroVarianceFlag;
	gProjectInfoPtr->setZeroVarianceFlag = FALSE;
										
			// Get the common covariance matrix.
			
	continueFlag = GetCommonCovariance (covariancePtr,
													tempMatrixPtr,
													classChannelStatsPtr,
													classPtr,
													channelListPtr,  
													numberClasses,
													numberOutputChannels,
													squareOutputMatrixFlag,
													kMeanCovariance,
													kMixedStats,
													FALSE);
	
	if (continueFlag && eigenVectorPtr != NULL && tempMatrixPtr != NULL)
		{                                        
		TransformSymmetricMatrix (gTransformationMatrix.eigenVectorPtr,
											covariancePtr, 
											tempMatrixPtr, 
											covariancePtr, 
											numberFeatures,
											numberOutputChannels,
											kSquareOutputMatrix);
			
		}	// end "if (continueFlag && ..."
		
	gProjectInfoPtr->setZeroVarianceFlag = savedZeroVarianceFlag;
	
	if (continueFlag)	
		continueFlag = CheckMatrix (covariancePtr, 
												squareOutputMatrixFlag,
												kMeanCovariance, 
												numberFeatures,
												0,
												IDS_Project68,
												IDS_Project79,
												kDoNotUseListOneMessagePerClassFlag);
	
	return (continueFlag);
			
}	// end "GetTransformedCommonCovariance" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetTtInvTransformedCommonCovarianceT
//
//	Software purpose:	The purpose of this routine is to compute the inverse 
//							of the common covariance for the input set of classes and 
//							class weights. If requested the common covariance will be
//							transformed before the inverse is take. Also the
//							transform of the inverse will be taken.
//
//							The output is: InvCommonCovariance or
//												TtInvTCommonCovarianceT
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			CorrelationClsfierControl
//							FisherClsfierControl
//
//	Coded By:			Larry L. Biehl			Date: 09/02/1997
//	Revised By:			Larry L. Biehl			Date: 09/02/1997

Boolean GetTtInvTransformedCommonCovarianceT (
				UInt16								numberOutputChannels,
				HChannelStatisticsPtr			classChannelStatsPtr,
				HDoublePtr							covariancePtr,
				UInt16*								channelListPtr,
				UInt16*								classPtr,  
				UInt32								numberClasses,
				Boolean								squareOutputMatrixFlag,
				HDoublePtr							eigenVectorPtr,
				HDoublePtr							tempMatrixPtr,
				UInt16								numberFeatures,
				double*								logDeterminantPtr,
				Boolean*								determinantOKFlagPtr)
	
{ 
	double								determinant;
	HDoublePtr							inverseCovPtr;
	Boolean								continueFlag = TRUE;
	
	
	GetTransformedCommonCovariance (numberOutputChannels,
												classChannelStatsPtr,
												covariancePtr,
												channelListPtr,
												classPtr,  
												numberClasses,
												squareOutputMatrixFlag,
												eigenVectorPtr,
												tempMatrixPtr,
												numberFeatures);
				
			// Get the inverse of the covariance matrix and the				
			// determinant.																
	
	inverseCovPtr = covariancePtr;		
	*determinantOKFlagPtr = InvertSymmetricMatrix (inverseCovPtr,
																	numberFeatures, 
																	gInverseMatrixMemory.pivotPtr, 
																	gInverseMatrixMemory.indexRowPtr, 
																	gInverseMatrixMemory.indexColPtr, 
																	gInverseMatrixMemory.ipvotPtr,
																	&determinant, 
																	logDeterminantPtr,
																	kReturnMatrixInverse);
	
	if (gOperationCanceledFlag)
		continueFlag = FALSE;
		
	if (continueFlag)
		{
		if (*determinantOKFlagPtr)
			{		
			if (eigenVectorPtr != NULL && tempMatrixPtr != NULL)
				{
				MatrixMultiply (eigenVectorPtr, 
										inverseCovPtr, 
										tempMatrixPtr, 
										numberOutputChannels,
										numberFeatures,
										numberFeatures,
										kTransposeMat1ByMat2);
										
				MatrixMultiply (tempMatrixPtr,
										eigenVectorPtr, 
										inverseCovPtr, 
										numberOutputChannels,
										numberFeatures,
										numberOutputChannels,
										kMat1ByMat2);
				
				}	// end "if (eigenVectorPtr != NULL && tempMatrixPtr != NULL)" 
				
			}	// end "if (*determinantOKFlagPtr)"
			
		}	// end "if (continueflag)"
					
	return (continueFlag);
			
}	// end "GetTtInvTransformedCommonCovarianceT" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetTtInvTransformedCovarianceT
//
//	Software purpose:	The purpose of this routine is to compute the inverse 
//							of the sample covariance for the input class. If requested 
//							the sample covariance will be transformed before the inverse is 
//							taken. Also the transform of the inverse will be taken.
//
//							The output is: InvCovariance or
//												TtInvTCovarianceT
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			CorrelationClsfierControl
//							MaxLikeClsfierControl
//
//	Coded By:			Larry L. Biehl			Date: 09/02/1997
//	Revised By:			Larry L. Biehl			Date: 06/17/2006

Boolean GetTtInvTransformedCovarianceT (
				UInt16								numberOutputChannels,
				HChannelStatisticsPtr			classChannelStatsPtr,
				HDoublePtr							covariancePtr,
				UInt16*								channelListPtr, 
				UInt16								statClassNumber,
				Boolean								squareOutputMatrixFlag, 
				SInt16								outputStatisticsCode,
				HDoublePtr							eigenVectorPtr,
				HDoublePtr							tempMatrixPtr,
				UInt16								numberFeatures,
				double*								logDeterminantPtr,
				Boolean*								determinantOKFlagPtr)
	
{ 
	double								determinant;
	HDoublePtr							inverseCovPtr;
	Boolean								continueFlag = TRUE;
	
	
	GetTransformedClassCovarianceMatrix (numberOutputChannels,
														classChannelStatsPtr,
														covariancePtr,
														channelListPtr, 
														statClassNumber,
														squareOutputMatrixFlag, 
														outputStatisticsCode,
														eigenVectorPtr,
														tempMatrixPtr,
														numberFeatures);
	
			// Get the inverse of the covariance matrix and the				
			// determinant.																
	
	inverseCovPtr = covariancePtr;		
	*determinantOKFlagPtr = InvertSymmetricMatrix (inverseCovPtr,
																	numberFeatures, 
																	gInverseMatrixMemory.pivotPtr, 
																	gInverseMatrixMemory.indexRowPtr, 
																	gInverseMatrixMemory.indexColPtr, 
																	gInverseMatrixMemory.ipvotPtr, 
																	&determinant,
																	logDeterminantPtr,
																	kReturnMatrixInverse);
	
	if (gOperationCanceledFlag)
		continueFlag = FALSE;
		
	if (continueFlag)
		{
		if (*determinantOKFlagPtr)
			{		
			if (eigenVectorPtr != NULL && tempMatrixPtr != NULL)
				{
				MatrixMultiply (eigenVectorPtr, 
										inverseCovPtr, 
										tempMatrixPtr, 
										numberOutputChannels,
										numberFeatures,
										numberFeatures,
										kTransposeMat1ByMat2);
										
				MatrixMultiply (tempMatrixPtr,
										eigenVectorPtr, 
										inverseCovPtr, 
										numberOutputChannels,
										numberFeatures,
										numberOutputChannels,
										kMat1ByMat2);
				
				}	// end "if (eigenVectorPtr != NULL && tempMatrixPtr != NULL)" 
				
			}	// end "if (*determinantOKFlagPtr)"
			
		}	// end "if (continueflag)"
					
	return (continueFlag);
			
}	// end "GetTtInvTransformedCovarianceT" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void InitializeClassifierVarStructure
//
//	Software purpose:	The purpose of this routine is to initialize memory for
//							the classifier variable structure.
//
//	Parameters in:					
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/01/1992
//	Revised By:			Larry L. Biehl			Date: 03/27/2012	

void InitializeClassifierVarStructure (
				ClassifierVar						*classifierVarPtr)

{
	if (classifierVarPtr != NULL)
		{
		classifierVarPtr->variable1 = 0;
		classifierVarPtr->unitsCode = kSampleUnits;
		classifierVarPtr->summaryUnitsCode = kPercentUnits;
		
		classifierVarPtr->variable1 = GetDefaultAreaConversionFactor (
																	gImageFileInfoPtr->mapProjectionHandle,
																	//FindProjectBaseImageWindowInfoHandle ());
																	gClassifySpecsPtr->targetWindowInfoHandle);
		
		if (classifierVarPtr->variable1 > 0.)													
			classifierVarPtr->summaryUnitsCode = kAreaUnits;
			
		classifierVarPtr->variable2 = 0;
		classifierVarPtr->variable3 = 0;
		classifierVarPtr->chanMeanPtr = NULL;
		classifierVarPtr->classConstantPtr = NULL;
		classifierVarPtr->classConstant2Ptr = NULL;
		classifierVarPtr->classConstantLOO1Ptr = NULL;
		classifierVarPtr->classConstantLOO2Ptr = NULL;
		classifierVarPtr->classConstantLOO3Ptr = NULL;
		classifierVarPtr->classConstantLOO4Ptr = NULL;
		classifierVarPtr->classConstantLOO5Ptr = NULL;
		classifierVarPtr->covariancePtr = NULL;
		classifierVarPtr->workVectorPtr = NULL;
		classifierVarPtr->workVector2Ptr = NULL;
		classifierVarPtr->countVectorPtr = NULL;
		classifierVarPtr->countClassIndexPtr = NULL;
		classifierVarPtr->totalCorrectSamples = 0;
		classifierVarPtr->totalNumberSamples = 0;
		classifierVarPtr->totalSameDistanceSamples = -1;
		classifierVarPtr->tableWidth = 0;
		classifierVarPtr->totalCountIndexStart = 0;
		classifierVarPtr->areaCode = 0;
		classifierVarPtr->backgroundIndexOffset = 0;
		classifierVarPtr->numberWidth = 0;
		classifierVarPtr->numberPrecision = 0;
		classifierVarPtr->summaryCode = 0;
		classifierVarPtr->tableType = 0;
		classifierVarPtr->thresholdFlag = FALSE;
		
		}	// end "if (classifierVarPtr != NULL)" 
			
}	// end "InitializeClassifierVarStructure"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void KNNClassifier
//
//	Software purpose:	The purpose of this routine is to classify the input line
//							of data using the K Nearest Neighbor Classifier.
//
//	Parameters in:					
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Tsung Tai				Date: 04/01/2019
//	Revised By:			Larry L. Biehl			Date: 08/15/2019
/*
		// Sorting operator for knn

bool comparison (knnType a, knnType b)

{
   return (a.distance < b.distance);
	
}	// end "comparison"
*/


SInt16 KNNClassifier (
				AreaDescriptionPtr            areaDescriptionPtr,
				FileInfoPtr                   fileInfoPtr,
				ClassifierVarPtr              clsfyVariablePtr,
				HUCharPtr                     outputBuffer1Ptr,
				HUCharPtr							thresholdBufferPtr,
				HSInt64Ptr							countVectorPtr,
				Point									point)

{
	
   RgnHandle                     rgnHandle;
	
   HDoublePtr                    ioBufferReal8Ptr,
   										savedBufferReal8Ptr;
	
   SInt16                        *channelsPtr,
   										*classVectorPtr;
	
	double								dDistance,
											dValue;

	int									topK[257],
											topKTemp[257];
	
	SInt16								kValue;
	
   UInt32                        baseIndex,
   										equalClassCount,
   										feat,
											index,
											maxClass,
											numberChannels,
											numberProjectChannels,
											numberProjectClasses,
											numberSamplesPerChan,
											//sameDistanceCount,
											sample;
	
   Boolean                       createThresholdFlag,
   										polygonField;
	
	
   		// Initialize local variables.
	
   numberChannels =        gClassifySpecsPtr->numberChannels;
	channelsPtr = (SInt16*)GetHandlePointer (gClassifySpecsPtr->featureHandle);
   numberProjectClasses =	gProjectInfoPtr->numberStatisticsClasses;
   numberProjectChannels = gProjectInfoPtr->numberStatisticsChannels;
   numberSamplesPerChan =  (UInt32)areaDescriptionPtr->numSamplesPerChan;
   rgnHandle =					areaDescriptionPtr->rgnHandle;
   polygonField =				areaDescriptionPtr->polygonFieldFlag;
   //classPtr = 					gClassifySpecsPtr->classPtr;
   classVectorPtr = 			gClassifySpecsPtr->classVectorPtr;
	savedBufferReal8Ptr = 	(HDoublePtr)outputBuffer1Ptr;
	kValue =						gClassifySpecsPtr->nearestNeighborKValue;
	createThresholdFlag = 	gClassifySpecsPtr->createThresholdTableFlag;
			
	int knnPixelSize = gProjectInfoPtr->knnCounter;
	//int knnPixelSize = gProjectInfoPtr->knnCounter;
	
   		// Loop through the number of samples in the line of data
	
   for (sample=0; sample<numberSamplesPerChan; sample++)
   	{
      if (!polygonField || PtInRgn (point, rgnHandle))
      	{
   		maxClass = 0;
			equalClassCount = 0;
         //int topK[numberClasses+1];
         for (int i=0; i<=numberProjectClasses; i++)
            topK[i] = 0;
		
			baseIndex = 0;
         for (int i=0; i<knnPixelSize; i++)
         	{
         			// Only use those pixels for classes which are being used
         			// in the classification.
				
				if (classVectorPtr[gProjectInfoPtr->knnLabelsPtr[i]])
					{
					ioBufferReal8Ptr = savedBufferReal8Ptr;
					
							// Euclidean distance
					
					dDistance = 0;
					for (feat=0; feat<numberChannels; feat++)
						{
						index = baseIndex + channelsPtr[feat];
						dValue = gProjectInfoPtr->knnDataValuesPtr[index] - *ioBufferReal8Ptr;
						dDistance += dValue * dValue;
						
						//index++;
						ioBufferReal8Ptr++;
						
						}	// end "for (feat=0; feat<numberChannels; feat++)"
					
					gProjectInfoPtr->knnDistancesPtr[i].distance = dDistance;
					gProjectInfoPtr->knnDistancesPtr[i].index = i;
					
					}	// end "if (classVectorPtr[gProjectInfoPtr->knnLabelsPtr[i]])"
				
				baseIndex += numberProjectChannels;
				
         	}	// end " for (int i=0; i<knnPixelSize; i++)"
			
					// Find the topK points with min distance
					// Note that iterators do not work in Windows version. Suspect the
					// reason is that the knn_distances vector was not loaded using
					// push, etc.

         knnType it;
         for (int i=0; i<kValue; i++)
         	{
				double minTempDistance = DBL_MAX;
				int minTempLabel = 0;
				int minTempIndex = 0;
					//for (it = gProjectInfoPtr->knn_distances.begin ();
					//		it != gProjectInfoPtr->knn_distances.end ();
					//    it++)
				for (int pixelIndex=0;
						pixelIndex<knnPixelSize;
						pixelIndex++)
					{
         		if (classVectorPtr[gProjectInfoPtr->knnLabelsPtr[pixelIndex]])
         			{
						it = gProjectInfoPtr->knnDistancesPtr[pixelIndex];
						if (it.distance < minTempDistance)
							{
							minTempDistance = it.distance;
							minTempIndex = it.index;
							minTempLabel = gProjectInfoPtr->knnLabelsPtr[minTempIndex];
							
							}	// end "if (it->distance < minTempDistance)"
						
						}	// end "if (classVectorPtr[gProjectInfoPtr->knnLabelsPtr[i]])"
						
					}	// end "for (it=gProjectInfoPtr->knn_labels.begin (); ..."
				
				topKTemp[i] = minTempLabel;
				gProjectInfoPtr->knnDistancesPtr[minTempIndex].distance = DBL_MAX;
				
         	}	// end "for (int i=0; i<kValue; i++)"

         		// Find the first class which has the topK closest distances to the
					// pixel being classified.
			
			for (int i=0; i<kValue; i++)
				{
            topK[topKTemp[i]]++;
				
				}	// end "for (int i=0; i<kValue; i++)"

					// Find the label with the maximum count
			
         int max = 0;
         for (int i=0; i<=numberProjectClasses; i++)
         	{
            if (topK[i] > max)
            	{
               maxClass = i;
               max = topK[i];
               equalClassCount = 0;
					
            	}	// end "if (topK[i] > max)"
				
				else if (topK[i] != 0 && topK[i] == max)
               equalClassCount++;
				
            topK[i] = 0;
				
         	}	// end "for (int i=0; i<=numberClasses; i++)"
			
         countVectorPtr[maxClass]++;
         *outputBuffer1Ptr = (UInt8)maxClass;
	
      	clsfyVariablePtr->totalSameDistanceSamples += equalClassCount;
			
		   if (createThresholdFlag)
		   	{
		   			// Get the threshold table index.
				
				*thresholdBufferPtr = (UInt8)max;
				thresholdBufferPtr++;
				
						// This is used to determine the average number of k nearest
						// neighbor values
				
				gTempDoubleVariable1 += max;
				
		   	}	// end "if (createThresholdFlag)"
		
      	} 	// end "if (!polygonField || PtInRgn (point, rgnHandle))"
	
      else	// polygonField && !PtInRgn (point, rgnHandle)
      	{
         *outputBuffer1Ptr = 0;
		
      	}	// end "else polygonField && !PtInRgn (point, rgnHandle)"
	
      point.h++;
      outputBuffer1Ptr++;
      savedBufferReal8Ptr += numberChannels;
	
      		// Exit routine if user selects "cancel" or "command period".
	
      if (TickCount () >= gNextTime)
      	{
         if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
            return (3);
		
      	}	// end "if (TickCount () >= gNextTime)"
	
   	}	// end "for (sample=0; sample<..."
	
   		// Write carriage return and null character to classification record
	
   *outputBuffer1Ptr = kCarriageReturn;
   outputBuffer1Ptr++;
	
	#if defined multispec_win
		*outputBuffer1Ptr = kLineFeed;
		outputBuffer1Ptr++;
	#endif   // defined multispec_win
	
   *outputBuffer1Ptr = kNullTerminator;
	
   return (0);
	
}   // end "KNNClassifier"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void KNearestNeighborClassifierControl
//
//	Software purpose:	The purpose of this routine is to control the classification of
//							the specified data using the point k Nearest Neighbor classifier.
//
//	Parameters in:
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			ClassifyPerPointArea
//
//	Coded By:			Larry L. Biehl			Date: 04/25/2019
//	Revised By:			Larry L. Biehl			Date: 06/04/2019

void KNearestNeighborClassifierControl (
				FileInfoPtr							fileInfoPtr)

{
	ClassifierVar						classifierVar;
	
	UInt32								numberClsfyChannels;
	
	Boolean								continueFlag;
	
	
			// Initialize local variables
	
	numberClsfyChannels = gClassifySpecsPtr->numberChannels;
	
	InitializeClassifierVarStructure (&classifierVar);
	
			// Get the memory for the mean vectors and covariance matrices
			// for the classes and channels to be used in the classification.
	
	continueFlag = SetupClsfierMemory (NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL);
	
			// Continue with classification, if memory is not full.
	
	if (continueFlag)
		{
				// Classify the requested areas.	First load the maximum
				// likelihood variable structure with need pointers
		
		classifierVar.chanMeanPtr = NULL;
		classifierVar.classConstantLOO1Ptr = NULL;
		
		ClassifyAreasControl (fileInfoPtr, &classifierVar);
		
		}	// end "if (continueFlag)"

}	// end "KNearestNeighborClassifierControl"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ListAverageDiscriminantValue
//
//	Software purpose:	The purpose of this routine is to list the average
//							input discriminant value.
//
//	Parameters in:			
//
//	Parameters out:	None
//
// Value Returned:	
//
// Called By:			ListTrainTestSummary
//							ListClassificationSummary in SListResults.cpp
//							
//
//	Coded By:			Larry L. Biehl			Date: 09/03/1997
//	Revised By:			Larry L. Biehl			Date: 06/29/2019

Boolean ListAverageDiscriminantValue (
				CMFileStream*					resultsFileStreamPtr,
				SInt16							outputCode,
				SInt64							numberPixels,
				double							discriminantTotal)

{  
	Boolean continueFlag = TRUE;
	
	if (numberPixels > 0)
		discriminantTotal /= numberPixels;
		
	UInt16 stringIndex = IDS_Classify41;
	if (gClassifySpecsPtr->mode == kCorrelationMode)
		stringIndex = IDS_Classify58;
	
	else if (gClassifySpecsPtr->mode == kCEMMode)
		stringIndex = IDS_Classify53;
	
	else if (gClassifySpecsPtr->mode == kKNearestNeighborMode)
		stringIndex = IDS_Classify73;
		
	else	// gClassifySpecsPtr->mode == kMaxLikeMode, kMahalanobisMode, kFisherMode,
			// or kEchoMode
		discriminantTotal /= 100.;
							
	continueFlag = ListSpecifiedStringNumber (kClassifyStrID,
															stringIndex,
															resultsFileStreamPtr, 
															outputCode, 
															discriminantTotal, 
															continueFlag);
	
	if (gClassifySpecsPtr->mode == kCorrelationMode)
		{											
		double averageAngle = acos (discriminantTotal) * kRadiansToDegrees;
							
		continueFlag = ListSpecifiedStringNumber (kClassifyStrID,
																IDS_Classify59,
																resultsFileStreamPtr, 
																outputCode, 
																averageAngle, 
																continueFlag);
		
		}	// end "if (gClassifySpecsPtr->mode == kCorrelationMode)"
	
	return (continueFlag);
		
}	// end "ListAverageDiscriminantValue" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ListClassifyInputParameters
//
//	Software purpose:	The purpose of this routine is to list the input parameters for
//							the respective classifier.
//
//	Parameters in:			
//
//	Parameters out:	None
//
// Value Returned:	
//
// Called By:			ClassifyControl
//							
//
//	Coded By:			Larry L. Biehl			Date: 02/10/2000
//	Revised By:			Larry L. Biehl			Date: 09/20/2019

Boolean ListClassifyInputParameters (
				FileInfoPtr							fileInfoPtr,
				CMFileStream*						resultsFileStreamPtr)
				
{
	CEMParametersPtr					cemParametersPtr;
	EchoClassifierVarPtr				echoClassifierVarPtr;
	
	UInt32								index,
											numberClasses;
	
	SInt16								localWeightsIndex;
	
	Boolean								continueFlag = TRUE;
	
	
	cemParametersPtr = NULL;
	echoClassifierVarPtr = NULL;
	
	if (gClassifySpecsPtr->mode == kEchoMode)
		{
		echoClassifierVarPtr = (EchoClassifierVarPtr)GetHandlePointer (
															gClassifySpecsPtr->echoClassifierVarH,
															kLock);
													
		if (echoClassifierVarPtr == NULL)
																							return (FALSE);
													
		}	// end "if (gClassifySpecsPtr->mode == kEchoMode)"
		
	else if (gClassifySpecsPtr->mode == kCEMMode)
		{
		cemParametersPtr = (CEMParametersPtr)GetHandlePointer (
															gClassifySpecsPtr->cemParametersH,
															kLock);
													
		if (cemParametersPtr == NULL)
																							return (FALSE);
													
		}	// end "if (gClassifySpecsPtr->mode == kEchoMode)"
	
			// List the classification header information.				
			
	continueFlag = ListClassificationHeaderInfo (resultsFileStreamPtr,
																&gOutputForce1Code,
																gProjectInfoPtr->covarianceStatsToUse,
																continueFlag);
	
			// If enhanced statistics to be used, verify that they exist
			// for the requested classes. If not list a message.

	if (continueFlag && gProjectInfoPtr->covarianceStatsToUse == kEnhancedStats)
		continueFlag = CheckClassEnhancedStats (gClassifySpecsPtr->numberClasses,
																gClassifySpecsPtr->classPtr);
	
			// List the target image file.									
	
	char* fileNamePtr = (char*)GetFileNameCPointerFromFileInfo (gImageFileInfoPtr);
	sprintf ((char*)gTextString,
				"    Target image file = '%s'%s", 
				fileNamePtr,
				gEndOfLine);
	
	continueFlag = OutputString (resultsFileStreamPtr,
											(char*)gTextString, 
											0, 
											gOutputForce1Code,
											continueFlag,
											kUTF8CharString);

			// List the classifier algorithm used.
			
	continueFlag = ListSpecifiedStringNumber (kClassifyStrID,
															IDS_Classify20+gClassifySpecsPtr->mode, 
															(unsigned char*)gTextString, 
															resultsFileStreamPtr, 
															gOutputForce1Code,
															continueFlag);
	
	if (continueFlag && gClassifySpecsPtr->mode == kEchoMode) 
		{
				// List the ECHO parameter values.
				//  Echo algorithm	
		
		index = IDS_Classify49;				
		if (echoClassifierVarPtr->algorithmCode == kMahalanobisMode)  
			index = IDS_Classify70;
		else if (echoClassifierVarPtr->algorithmCode == kFisherMode) 
			index = IDS_Classify50;		
	
		continueFlag = ListSpecifiedStringNumber (kClassifyStrID,
																(SInt16)index, 
																(unsigned char*)gTextString, 
																resultsFileStreamPtr, 
																gOutputForce1Code,
																continueFlag);
		
				//  Cell Width														
						
		continueFlag = ListSpecifiedStringNumber (
															kClassifyStrID,
															IDS_Classify8,
															resultsFileStreamPtr, 
															gOutputForce1Code, 
															(SInt32)echoClassifierVarPtr->cell_width, 
															continueFlag);	
		
				//  Cell Homogeneity Test Threshold
		/*
		continueFlag = ListSpecifiedStringNumber (kClassifyStrID,
																IDS_Classify5, 
																(unsigned char*)gTextString, 
																resultsFileStreamPtr, 
																gOutputForce1Code,
																continueFlag);
		*/
		index = IDS_Classify9;				
		if (!echoClassifierVarPtr->fixed_homogeneityThreshold_option) 
			index = IDS_Classify10;
			
		continueFlag = ListSpecifiedStringNumber (
														kClassifyStrID,
														(SInt16)index,
														resultsFileStreamPtr, 
														gOutputForce1Code, 
														echoClassifierVarPtr->homogeneityThreshold, 
														continueFlag);
									
				//  Annexation Threshold	
			
		continueFlag = ListSpecifiedStringNumber (
														kClassifyStrID,
														IDS_Classify11,
														resultsFileStreamPtr, 
														gOutputForce1Code, 
														echoClassifierVarPtr->annexationThreshold, 
														continueFlag);
		
		}	// end "if (... && gClassifySpecsPtr->mode == kEchoMode)"

	if (continueFlag && gClassifySpecsPtr->mode == kSupportVectorMachineMode)
		{
				// List the SVM parameters used for the classification.
		
		continueFlag = ListClassifySVMInputParameters (fileInfoPtr, resultsFileStreamPtr);
		
		}	// end "if (... && gClassifySpecsPtr->mode == kSupportVectorMachineMode)"

	if (continueFlag && gClassifySpecsPtr->mode == kKNearestNeighborMode)
		{
				// List the k value used for the classification.
		
		continueFlag = ListSpecifiedStringNumber (
														kClassifyStrID,
														IDS_Classify71,
														resultsFileStreamPtr,
														gOutputForce1Code,
														(SInt32)gClassifySpecsPtr->nearestNeighborKValue,
														continueFlag);
		
		}	// end "if (... && gClassifySpecsPtr->mode == kKNearestNeighborMode)"

	if (continueFlag && gClassifySpecsPtr->mode == kCorrelationMode) 
		{	
				// List statement that covariance information will be used
				// for the correlation classifier (SAM).
	
		if (gClassifySpecsPtr->correlationCovarianceCode > kNoCovarianceUsed)
			{
			UInt16 stringIndex = IDS_Classify51;
			if (gClassifySpecsPtr->correlationCovarianceCode == kSampleCovariance)
				stringIndex = IDS_Classify52;
				
			continueFlag = ListSpecifiedStringNumber (kClassifyStrID,
																	stringIndex, 
																	(unsigned char*)gTextString, 
																	resultsFileStreamPtr, 
																	gOutputForce1Code,
																	continueFlag);
			
			}	// end "if (...->correlationCovarianceCode > kNoCovarianceUsed)"
		
		}	// end "if (... && gClassifySpecsPtr->mode == kCorrelationMode)" 

	if (continueFlag && gClassifySpecsPtr->mode == kCEMMode) 
		{
		UInt16 stringIndex = IDS_Classify56;
		if (cemParametersPtr->correlationMatrixCode == kAreaType)
			stringIndex = IDS_Classify57;
			
		continueFlag = ListSpecifiedStringNumber (kClassifyStrID,
																stringIndex, 
																(unsigned char*)gTextString, 
																resultsFileStreamPtr, 
																gOutputForce1Code,
																continueFlag);
		
		if (cemParametersPtr->correlationMatrixCode == kTrainingType)
			{			
			UInt16* correlationMatrixClassPtr = (UInt16*)GetHandlePointer (
												cemParametersPtr->correlationMatrixClassHandle);
							
			continueFlag =  ListClassesUsed (
												NULL,
												fileInfoPtr,
												(SInt16*)correlationMatrixClassPtr, 
												NULL,
												-1, 
												cemParametersPtr->numbercorrelationMatrixClasses, 
												resultsFileStreamPtr, 
												&gOutputForce1Code,
												FALSE);
				
			}	// end "if (cemParametersPtr->correlationMatrixCode == kTrainingType)"
								
		else	// cemParametersPtr->correlationMatrixCode == kAreaType)
			{
					// Insert 6 space to line before the line column string.
			
			sprintf ((char*)gTextString, "      ");
			continueFlag = OutputString (resultsFileStreamPtr, 
													(char*)gTextString, 
													0, 
													gOutputForce1Code,
													continueFlag);
			
			continueFlag = ListLineColumnIntervalString (resultsFileStreamPtr,
																		gOutputForce1Code,
																		cemParametersPtr->lineStart, 
																		cemParametersPtr->lineEnd, 
																		cemParametersPtr->lineInterval,
																		cemParametersPtr->columnStart, 
																		cemParametersPtr->columnEnd, 
																		cemParametersPtr->columnInterval, 
																		continueFlag);
			
			}	// end "else ...->correlationMatrixCode == kAreaType)"
		
		}	// end "if (... && gClassifySpecsPtr->mode == kCEMMode)"

	if (continueFlag && gClassifySpecsPtr->mode == kParallelPipedMode) 
		{	
				// List method used for parallel piped.
	
		UInt16 stringIndex = IDS_Classify68;
		if (gClassifySpecsPtr->parallelPipedCode == kPPStandardDeviationCode)
			stringIndex = IDS_Classify69;
			
		continueFlag = ListSpecifiedStringNumber (kClassifyStrID,
																stringIndex, 
																(unsigned char*)gTextString, 
																resultsFileStreamPtr, 
																gOutputForce1Code,
																continueFlag);
			
		}	// end "if (... && gClassifySpecsPtr->mode == kParallelPipedMode)"
			
			// List the threshold value if being used.	

	if (continueFlag)
		continueFlag = ListThresholdValue (gClassifySpecsPtr->probabilityThreshold,
														gClassifySpecsPtr->correlationAngleThreshold,
														gClassifySpecsPtr->cemThreshold,
														gClassifySpecsPtr->knnThreshold,
														resultsFileStreamPtr,
														&gOutputForce1Code,
														gClassifySpecsPtr->thresholdFlag,
														gClassifySpecsPtr->mode);	
	
			// Insert a blank line after the table.
	
	continueFlag = OutputString (resultsFileStreamPtr, 
											gEndOfLine, 
											gNumberOfEndOfLineCharacters, 
											gOutputForce1Code, 
											continueFlag);
			
			// List the channels used.											
	
	if (continueFlag)
		{
				// Get the pointer to the channel & feature lists. The handles 
				// are already locked.
														
		SInt16* channelsPtr = (SInt16*)GetHandlePointer (
																	gClassifySpecsPtr->channelsHandle);
											
		SInt16* featurePtr = (SInt16*)GetHandlePointer (
																	gClassifySpecsPtr->featureHandle);
												
		continueFlag = ListChannelsUsed (fileInfoPtr,
													featurePtr,
													channelsPtr,
													gClassifySpecsPtr->numberFeatures, 
													resultsFileStreamPtr, 
													&gOutputForce1Code,
													gClassifySpecsPtr->featureTransformationFlag);
		
		}	// end "if (continueFlag)"

			// List the number of classes.									
	
	numberClasses = (UInt32)gProjectInfoPtr->numberStatisticsClasses;
	if (gClassifySpecsPtr->thresholdFlag)
		numberClasses++;
	MGetString (gTextString2, kFileIOStrID, IDS_NumberClasses);
	sprintf ((char*)gTextString, 
					"    %s %d%s",
					&gTextString2[1], 
					(unsigned int)numberClasses,
					gEndOfLine);
	continueFlag = OutputString (resultsFileStreamPtr, 
											(char*)gTextString,
											0,
											gOutputForce1Code,
											continueFlag);

			// List the number of classes used.								
	
	MGetString (gTextString2, kFileIOStrID, IDS_FileIO53);
	sprintf ((char*)gTextString, 
					"    %s %u%s",
					&gTextString2[1], 
					gClassifySpecsPtr->numberClasses,
					gEndOfLine);
	continueFlag = OutputString (resultsFileStreamPtr, 
											(char*)gTextString, 
											0, 
											gOutputForce1Code, 
											continueFlag);
			
	if (continueFlag)
		{
				// List the classes used.														
				// Weights will be listed for Maximum likelihood classification.	
				
		localWeightsIndex = -1;
		if (gClassifySpecsPtr->mode == kMaxLikeMode || 
				gClassifySpecsPtr->mode == kMahalanobisMode ||
					gClassifySpecsPtr->mode == kEchoMode ||
						gClassifySpecsPtr->mode == kFisherMode ||
							(gClassifySpecsPtr->mode == kSupportVectorMachineMode &&
									gClassifySpecsPtr->svm_type == 0))
			{
			localWeightsIndex = GetProjectClassWeightsIndex ();
			
					// Weights will only be listed for SVM classifier when
					// they are unequal. If so the list will not be the set
					// for enhanced statistics.
			
			if (gClassifySpecsPtr->mode == kSupportVectorMachineMode)
				{
				if (localWeightsIndex == 2)
					localWeightsIndex = -1;
				
				else if (localWeightsIndex == 1)
					localWeightsIndex = 0;
				
				}	// end "if (gClassifySpecsPtr->mode == kSupportVectorMachineMode)"
			
			}	// end "if (gClassifySpecsPtr->mode == kMaxLikeMode || ..."
			
				// The symbols will be included in the list only when they are used.
		
		unsigned char* symbolsPtr = NULL;
		if (gClassifySpecsPtr->diskFileFormat == kMultiSpecClassificationType)
			symbolsPtr = gClassifySpecsPtr->symbolsPtr;
		if (gClassifySpecsPtr->outputStorageType & 0x0001)
			symbolsPtr = gClassifySpecsPtr->symbolsPtr;
			
		continueFlag =  ListClassesUsed (NULL,
													fileInfoPtr, 
													gClassifySpecsPtr->classPtr, 
													symbolsPtr,
													localWeightsIndex, 
													gClassifySpecsPtr->numberClasses, 
													resultsFileStreamPtr, 
													&gOutputForce1Code,
													gClassifySpecsPtr->thresholdFlag);
		
				// List the training fields/areas used.									
					
		if (continueFlag)							
			continueFlag = ListProjectFieldsUsed (fileInfoPtr,
																gClassifySpecsPtr->classPtr, 
																gClassifySpecsPtr->numberClasses, 
																NULL, 
																kTrainingType, 
																resultsFileStreamPtr, 
																&gOutputForce1Code,
																TRUE);
		
		}	// end "if (continueFlag)"
			
	return (continueFlag);
		
}	// end "ListClassifyInputParameters"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ListClassifySVMInputParameters
//
//	Software purpose:	The purpose of this routine is to list the input parameters for
//							the Support Vector Machine classifier.
//
//	Parameters in:
//
//	Parameters out:	None
//
// Value Returned:
//
// Called By:			ClassifyControl in SClassify.cpp
//
//
//	Coded By:			Larry L. Biehl			Date: 09/19/2019
//	Revised By:			Larry L. Biehl			Date: 09/21/2019

Boolean ListClassifySVMInputParameters (
				FileInfoPtr							fileInfoPtr,
				CMFileStream*						resultsFileStreamPtr)

{
	Boolean								continueFlag = TRUE;
	
	
			// List the SVM parameters used for the classification.

	continueFlag = ListSpecifiedStringNumber (
													kClassifyStrID,
													IDS_Classify75+gClassifySpecsPtr->svm_type,
													(unsigned char*)gTextString,
													resultsFileStreamPtr,
													gOutputForce1Code,
													continueFlag);

	continueFlag = ListSpecifiedStringNumber (
												kClassifyStrID,
												IDS_Classify80+gClassifySpecsPtr->svm_kernel_type,
												(unsigned char*)gTextString,
												resultsFileStreamPtr,
												gOutputForce1Code,
												continueFlag);
	
	if (gClassifySpecsPtr->svm_kernel_type == kPolynomialKernel)
		continueFlag = ListSpecifiedStringNumber (
														kClassifyStrID,
														IDS_Classify84,
														resultsFileStreamPtr,
														gOutputForce1Code,
														(SInt32)gClassifySpecsPtr->svm_degree,
														continueFlag);
	
	if (gClassifySpecsPtr->svm_kernel_type == kPolynomialKernel ||
				gClassifySpecsPtr->svm_kernel_type == kRBFKernel ||
						gClassifySpecsPtr->svm_kernel_type == kSigmoidKernel)
		continueFlag = ListSpecifiedStringNumber (
														kClassifyStrID,
														IDS_Classify85,
														resultsFileStreamPtr,
														gOutputForce1Code,
														gClassifySpecsPtr->svm_gamma,
														continueFlag);
	
	if (gClassifySpecsPtr->svm_kernel_type == kPolynomialKernel ||
						gClassifySpecsPtr->svm_kernel_type == kSigmoidKernel)
		continueFlag = ListSpecifiedStringNumber (
														kClassifyStrID,
														IDS_Classify86,
														resultsFileStreamPtr,
														gOutputForce1Code,
														gClassifySpecsPtr->svm_coef0,
														continueFlag);

	continueFlag = ListSpecifiedStringNumber (
													kClassifyStrID,
													IDS_Classify87,
													resultsFileStreamPtr,
													gOutputForce1Code,
													gClassifySpecsPtr->svm_cache_size,
													continueFlag);

	continueFlag = ListSpecifiedStringNumber (
													kClassifyStrID,
													IDS_Classify88,
													resultsFileStreamPtr,
													gOutputForce1Code,
													gClassifySpecsPtr->svm_eps,
													continueFlag);
	
	if (gClassifySpecsPtr->svm_type == kC_SVC_Type ||
				gClassifySpecsPtr->svm_type == kEpsilon_SVR_Type ||
						gClassifySpecsPtr->svm_type == kNU_SVR_Type)
		continueFlag = ListSpecifiedStringNumber (
														kClassifyStrID,
														IDS_Classify89,
														resultsFileStreamPtr,
														gOutputForce1Code,
														gClassifySpecsPtr->svm_cost,
														continueFlag);
	
	if (gClassifySpecsPtr->svm_type == kNU_SVC_Type ||
				gClassifySpecsPtr->svm_type == kOne_Class_Type ||
						gClassifySpecsPtr->svm_type == kNU_SVR_Type)
		continueFlag = ListSpecifiedStringNumber (
														kClassifyStrID,
														IDS_Classify91,
														resultsFileStreamPtr,
														gOutputForce1Code,
														gClassifySpecsPtr->svm_nu,
														continueFlag);
	
	if (gClassifySpecsPtr->svm_type == kEpsilon_SVR_Type)
		continueFlag = ListSpecifiedStringNumber (
														kClassifyStrID,
														IDS_Classify92,
														resultsFileStreamPtr,
														gOutputForce1Code,
														gClassifySpecsPtr->svm_p,
														continueFlag);
	
	continueFlag = ListSpecifiedStringNumber (
													kClassifyStrID,
													IDS_Classify93,
													resultsFileStreamPtr,
													gOutputForce1Code,
													(SInt32)gClassifySpecsPtr->svm_shrinking,
													continueFlag);
	
	
	if (gClassifySpecsPtr->svm_type == kC_SVC_Type ||
			gClassifySpecsPtr->svm_type == kNU_SVC_Type ||
				gClassifySpecsPtr->svm_type == kEpsilon_SVR_Type ||
					gClassifySpecsPtr->svm_type == kNU_SVR_Type)
		continueFlag = ListSpecifiedStringNumber (
													kClassifyStrID,
													IDS_Classify94,
													resultsFileStreamPtr,
													gOutputForce1Code,
													(SInt32)gClassifySpecsPtr->svm_probability,
													continueFlag);

	return (continueFlag);
	
}	// end "ListClassifySVMInputParameters"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ListNumberOfSameDistanceSamples
//
//	Software purpose:	The purpose of this routine is to list the number of same
//							distance samples or equal number of classes. In other words
//							this is the number of samples for which there is no clear
//                   class to assign the sample to.
//
//	Parameters in:
//
//	Parameters out:	None
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/08/2019
//	Revised By:			Larry L. Biehl			Date: 07/08/2019

Boolean ListNumberOfSameDistanceSamples (
				CMFileStream*						resultsFileStreamPtr,
				SInt64								totalSameDistanceSamples)

{
	SInt16								strLength;
	
	Boolean								continueFlag = TRUE;
	
	
	if (totalSameDistanceSamples >= 0)
		{
		strLength = ListCountValue ((char*)gTextString3,
												totalSameDistanceSamples,
												-1,
												0,
												-1,
												kDoNotIncludeTab);
		
		int	stringNumber = IDS_Classify67;
		if (gProjectInfoPtr->includesStatisticsFromClusterOperationFlag ||
									gClassifySpecsPtr->mode == kKNearestNeighborMode)
			stringNumber = IDS_Classify72;
		
		continueFlag = ListSpecifiedStringNumber (kClassifyStrID,
																stringNumber,
																resultsFileStreamPtr,
																gOutputForce1Code,
																(char*)gTextString3,
																continueFlag);
		
		}	// end "if (totalSameDistanceSamples >= 0)"
	
	return (continueFlag);

}	// end "ListNumberOfSameDistanceSamples"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ListTrainTestSummary
//
//	Software purpose:	The purpose of this routine is to list the summary
//							of the classification results for the input field.
//
//	Parameters in:			
//
//	Parameters out:	None
//
// Value Returned:	
//
// Called By:			ClassifyPerPointArea in SClassify.cpp
//							PostEchoClassifier in SClassifyEchoControl.cpp
//							
//
//	Coded By:			Larry L. Biehl			Date: 04/08/1992
//	Revised By:			Larry L. Biehl			Date: 02/28/2018

SInt16 ListTrainTestSummary (
				SInt16								classPointer, 
				CMFileStream*						resultsFileStreamPtr, 
				AreaDescriptionPtr				areaDescriptionPtr, 
				HSInt64Ptr							countVectorPtr, 
				ClassifierVarPtr 					clsfyVariablePtr, 
				double								probabilityTotal)

{                   
	double								percentCorrect;
	
	SInt64								numberPixels;
	
	Boolean								continueFlag,
											listFlag;
									
	SInt16								fieldNumber,
											lOutputCode,
											strLength;
									
									
	continueFlag = TRUE;
									
	
			// If the classification is being written as ascii to the output
			// text window or to a disk file then always list the summary.
			// If not then list the summary only if the user requests it to be
			// listed.
	
	listFlag = GetListResultsFlag (clsfyVariablePtr->areaCode, kAreasSummary);
	if (gOutputCode & (kOutputWindowFormatCode + kAsciiFormatCode))			
		listFlag = TRUE;
	
	if (listFlag)
		{
		fieldNumber = areaDescriptionPtr->fieldNumber - 1;
		lOutputCode = gOutputCode;
		if (gClassifySpecsPtr->createThresholdTableFlag)
			lOutputCode = gOutputForce1Code;
		
		numberPixels = gProjectInfoPtr->fieldIdentPtr[fieldNumber].numberPixels;
		percentCorrect = 0;
		
		if (numberPixels > 0)
			percentCorrect = (double)countVectorPtr[classPointer] * 100/numberPixels;
		pstr ((char*)gTextString2,
					(char*)&gProjectInfoPtr->fieldIdentPtr[fieldNumber].name, 
					&strLength);
		sprintf ((char*)gTextString, 
					"      Field %d '%s' Percent correct %5.1f%s",
					(int)(fieldNumber+1),
					(char*)gTextString2,
					percentCorrect,
					gEndOfLine);
											
		continueFlag = OutputString (resultsFileStreamPtr, 
												(char*)gTextString, 
												0, 
												lOutputCode, 
												continueFlag);
									
		if (continueFlag && gClassifySpecsPtr->createThresholdTableFlag)
			{
			continueFlag = ListAverageDiscriminantValue (resultsFileStreamPtr,
																		lOutputCode,
																		numberPixels,
																		probabilityTotal);
			
			}	// end "if (continueFlag && ...createThresholdTableFlag..." 
			
		if (numberPixels > 0)
			continueFlag = OutputString (resultsFileStreamPtr, 
													(char*)&gEndOfLine, 
													gNumberOfEndOfLineCharacters, 
													lOutputCode, 
													continueFlag);
													
		}	// end "if (listFlag)"
	
	return (!continueFlag);
		
}	// end "ListTrainTestSummary"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean LoadClassifySpecs
//
//	Software purpose:	The purpose of this routine is to load the Classify 
//							Specification structure with the default set of
//							parameters.
//
//	Parameters in:		Pointer to image file information structure (MFileInfo)
//
//	Parameters out:	None
//
// Value Returned:	True if the default values for the structure were loaded okay.
//							False indicates that memory was not available for a vector that
//								was needed.
// 
// Called By:			ClassifyControl
//
//	Coded By:			Larry L. Biehl			Date: 12/07/1988
//	Revised By:			Larry L. Biehl			Date: 09/19/2019

Boolean LoadClassifySpecs (
				FileInfoPtr							fileInfoPtr)

{
	SInt16								*channelsPtr,
											*classAreaPtr,
											*featurePtr;
	
	//Handle								baseImageWindowHandle;
	SInt32								bytesNeeded;
	
	Boolean								changedFlag,
											continueFlag;
	
	
			// Assume routine will work okay.  If there isn't enough memory		
			// then the flag will be changed.												
			
	continueFlag = TRUE;
	
	if (gProjectInfoPtr->classifySpecsH == NULL)
		{
				// Set up handle to project file information								
			
		gProjectInfoPtr->classifySpecsH = MNewHandle (sizeof (ClassifySpecs));
		if (gProjectInfoPtr->classifySpecsH != NULL)
			{
			gClassifySpecsPtr = (ClassifySpecsPtr)GetHandlePointer (
												gProjectInfoPtr->classifySpecsH, kLock, kMoveHi);
			
					// Initialize the classification specification structure			
					
			gClassifySpecsPtr->probabilityThreshold = 2.;
			gClassifySpecsPtr->cemThreshold = .5;
			gClassifySpecsPtr->correlationAngleThreshold = 5.;
			gClassifySpecsPtr->correlationCoefficientThreshold = .9962;
			gClassifySpecsPtr->cemParametersH = NULL;
			gClassifySpecsPtr->channelsHandle = NULL;
			gClassifySpecsPtr->classAreaHandle = NULL;
			gClassifySpecsPtr->classHandle = NULL;
			gClassifySpecsPtr->cemParametersH = NULL;
			gClassifySpecsPtr->decisionTreeVarH = NULL;
			gClassifySpecsPtr->echoClassifierVarH = NULL;
			gClassifySpecsPtr->featureHandle = NULL;
			gClassifySpecsPtr->symbolsHandle = NULL;
			gClassifySpecsPtr->targetWindowInfoHandle = 
															gProjectInfoPtr->windowInfoHandle;
			gClassifySpecsPtr->thresholdProbabilityHandle = NULL;
			gClassifySpecsPtr->thresholdTableHandle = NULL;
			gClassifySpecsPtr->thresholdTablePtr = NULL;
			gClassifySpecsPtr->classPtr = NULL;
			gClassifySpecsPtr->classVectorPtr = NULL;
			gClassifySpecsPtr->thresholdProbabilityPtr = NULL;
			gClassifySpecsPtr->symbolsPtr = NULL;
			gClassifySpecsPtr->imageAreaFlag = TRUE;
			
			gClassifySpecsPtr->supportVectorMachineModelAvailableFlag = FALSE;
			
			gClassifySpecsPtr->testFldsFlag = FALSE;
			if (gProjectInfoPtr->numberStatTestFields > 0)
				gClassifySpecsPtr->testFldsFlag = TRUE;
				
			gClassifySpecsPtr->trainingFldsResubstitutionFlag = FALSE;
			gClassifySpecsPtr->trainingFldsLOOFlag = FALSE;
			if (gProjectInfoPtr->numberStatTrainFields > 0)
				{
				gClassifySpecsPtr->trainingFldsResubstitutionFlag = TRUE;
				gClassifySpecsPtr->trainingFldsLOOFlag = FALSE;
				
				}	// end "if (gProjectInfoPtr->numberStatTrainFields > 0)"
				
			gClassifySpecsPtr->classAreaSet = kAllMenuItem;
			gClassifySpecsPtr->classSet = kAllMenuItem;
			gClassifySpecsPtr->diskFileFormat = 0;
			gClassifySpecsPtr->channelSet = kAllMenuItem;
			gClassifySpecsPtr->symbolSet = kDefaultMenuItem;
			gClassifySpecsPtr->numberChannels =
													gProjectInfoPtr->numberStatisticsChannels;
			gClassifySpecsPtr->numberClassAreas =
													gProjectInfoPtr->numberStatisticsClasses;
			gClassifySpecsPtr->numberClasses =
													gProjectInfoPtr->numberStatisticsClasses;
			gClassifySpecsPtr->numberProbabilityClasses = 0;
			gClassifySpecsPtr->numberFeatures = 
													gProjectInfoPtr->numberStatisticsChannels;
			gClassifySpecsPtr->mode = 1;
			gClassifySpecsPtr->parallelPipedCode = kPPStandardDeviationCode;	// kPPMinMaxCode;
			gClassifySpecsPtr->parallelPipedStanDevFactor = 2;
			gClassifySpecsPtr->imageLineStart = 1;
			gClassifySpecsPtr->imageLineInterval = 1;
			gClassifySpecsPtr->imageColumnStart = 1;
			gClassifySpecsPtr->imageColumnInterval = 1;
			
			WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
															gProjectInfoPtr->windowInfoHandle);
			gClassifySpecsPtr->imageLineEnd = windowInfoPtr->maxNumberLines;
			gClassifySpecsPtr->imageColumnEnd = windowInfoPtr->maxNumberColumns;
				
			gClassifySpecsPtr->outputStorageType = 0;
			gClassifySpecsPtr->correlationCovarianceCode = kNoCovarianceUsed;

			gClassifySpecsPtr->svm_type = 0;
			gClassifySpecsPtr->svm_kernel_type = 2;
			gClassifySpecsPtr->svm_degree = 3;
			gClassifySpecsPtr->svm_gamma = 0.0001;
			gClassifySpecsPtr->svm_coef0 = 0;
			gClassifySpecsPtr->svm_nu = 0.5;
			gClassifySpecsPtr->svm_cache_size = 100;
			gClassifySpecsPtr->svm_cost = 10;
			gClassifySpecsPtr->svm_eps = 0.1;
			gClassifySpecsPtr->svm_p = 0.0001;
			gClassifySpecsPtr->svm_shrinking = 1;
			gClassifySpecsPtr->svm_probability = 0;
			//gClassifySpecsPtr->svm_nr_weight = 0;
			
			gClassifySpecsPtr->nearestNeighborKValue = 5;
			gClassifySpecsPtr->knnThreshold = 2;
			
			gClassifySpecsPtr->featureTransformationFlag = FALSE;
			gClassifySpecsPtr->createThresholdTableFlag = FALSE;
			gClassifySpecsPtr->thresholdFlag = FALSE;
			gClassifySpecsPtr->outputBufferOffset = 0;
			gClassifySpecsPtr->probabilityBufferOffset = 0;
			gClassifySpecsPtr->pixelThresholdDegreesOfFreedom = 0;
			gClassifySpecsPtr->cellThresholdDegreesOfFreedom = 0;
			
			gClassifySpecsPtr->imageOverlayIndex = -1;
																
			UInt32 numberAreas = GetNumberOfAreas (NULL,
																gClassifySpecsPtr->numberClasses, 
																kTrainingType, 
																kDontIncludeClusterFields);
			if (numberAreas > 100)
				gProjectInfoPtr->listResultsTrainingCode = kClassSummary;
																
			numberAreas = GetNumberOfAreas (NULL,
														gClassifySpecsPtr->numberClasses, 
														kTestingType, 
														kDontIncludeClusterFields);
			if (numberAreas > 100)
				gProjectInfoPtr->listResultsTestCode = kClassSummary;
			
			}	// end "if (gProjectInfoPtr->classifySpecsH != NULL)" 
		
		else	// else gProjectInfoPtr->classifySpecsH == NULL 
			continueFlag = FALSE;
			
		}	// end "if (gProjectInfoPtr->classifySpecsH == NULL)" 
		
	else	// gProjectInfoPtr->classifySpecsH != NULL 
		{
				// Lock the handle to the classification specifications and get	
				// a pointer to the structure.												
				
		gClassifySpecsPtr = (ClassifySpecsPtr)GetHandlePointer (
												gProjectInfoPtr->classifySpecsH, kLock, kMoveHi);
		
				// Make sure that the featureTransformationFlag makes sense.
										
		if (!EigenvectorInfoExists (kProject))
			gClassifySpecsPtr->featureTransformationFlag = FALSE;
		
				// Force the default for the popup overlay to be a new overlay.
				
		gClassifySpecsPtr->imageOverlayIndex = -1;
									
				// Make sure that the number of channels in the classify structure
				// is not more than the number of statistics channels available.
				// It could have changed if the base image has been changed.
		/*
		gClassifySpecsPtr->numberChannels = MIN (
														gClassifySpecsPtr->numberChannels,
														gProjectInfoPtr->numberStatisticsClasses);
				
		gClassifySpecsPtr->numberFeatures = MIN (
														gClassifySpecsPtr->numberFeatures,
														gProjectInfoPtr->numberStatisticsClasses);
		*/
		}	// end "else gProjectInfoPtr->classifySpecsH != NULL" 
	                     	
	if (continueFlag)
		{	
				// Make certain that default classifier mode is consistant with	
				// the statistics that are available.										
				
		if (gProjectInfoPtr->statisticsCode != kMeanCovariance && 
							(gClassifySpecsPtr->mode == 1 ||
										gClassifySpecsPtr->mode == kEchoMode ||
															gClassifySpecsPtr->mode == 5))
			gClassifySpecsPtr->mode = 2;
					
				// Check memory for classification classes vector.						
			
		bytesNeeded =
				(SInt32)2 * gProjectInfoPtr->numberStatisticsClasses * sizeof (SInt16) + 1;
		gClassifySpecsPtr->classPtr = (SInt16*)CheckHandleSize (
																		&gClassifySpecsPtr->classHandle,
																		&continueFlag, 
																		&changedFlag, 
																		bytesNeeded);
		
		if (gClassifySpecsPtr->classPtr != NULL)
			gClassifySpecsPtr->classVectorPtr =
					&gClassifySpecsPtr->classPtr[gProjectInfoPtr->numberStatisticsClasses];

		if (changedFlag)
			gClassifySpecsPtr->classSet = kAllMenuItem;
			
		}	// end "if (continueFlag)" 
			
			// If memory is not full, check memory for class area vector 			
			// pointer.																				

	if (continueFlag)
		{
		bytesNeeded = (SInt32)gProjectInfoPtr->numberStatisticsClasses * sizeof (SInt16);
		classAreaPtr = (SInt16*)CheckHandleSize (&gClassifySpecsPtr->classAreaHandle,
																&continueFlag, 
																&changedFlag, 
																bytesNeeded);
		if (changedFlag)
			gClassifySpecsPtr->classAreaSet = kAllMenuItem;
			
		}	// end "if (continueFlag)" 
			
			// Check memory for classification feature vector pointer.															

	if (continueFlag)
		{
		bytesNeeded =
					(SInt32)gProjectInfoPtr->numberStatisticsChannels * sizeof (SInt16);
		featurePtr = (SInt16*)CheckHandleSize (&gClassifySpecsPtr->featureHandle,
															&continueFlag, 
															&changedFlag, 
															bytesNeeded);
		
		if (changedFlag)
			gClassifySpecsPtr->channelSet = kAllMenuItem;
			
		}	// end "if (continueFlag)" 
			
			// Check memory for classification channelsPtr vector.																	

	if (continueFlag)
		channelsPtr = (SInt16*)CheckHandleSize (&gClassifySpecsPtr->channelsHandle,
																&continueFlag, 
																&changedFlag, 
																bytesNeeded);

	if (continueFlag)
		{		
				// Check memory for class symbols vector.		
				
		bytesNeeded = (SInt32)(gProjectInfoPtr->numberStatisticsClasses+1) *
																							sizeof (char);
		gClassifySpecsPtr->symbolsPtr = (UInt8*)CheckHandleSize (
								&gClassifySpecsPtr->symbolsHandle,
								&continueFlag, 
								&changedFlag, 
								bytesNeeded);
		if (changedFlag)
			gClassifySpecsPtr->symbolSet = kDefaultMenuItem;
			
		}	// end "if (continueFlag)" 
		                  
	if (continueFlag)
		{
				// Load selection rectangle information into classification 		
				// specification information if the selection rectangle has 		
				// been set up																		

		GetSelectedAreaInfo (gProjectSelectionWindow,
									gImageWindowInfoPtr,
									&gClassifySpecsPtr->imageLineStart,
									&gClassifySpecsPtr->imageLineEnd,
									&gClassifySpecsPtr->imageColumnStart,
									&gClassifySpecsPtr->imageColumnEnd,
									kDontClearSelectionArea,
									kUseThreshold,
									kAdjustToBaseImage);
		
				// Make certain that feature, class and weight vectors are			
				// filled in properly.															
		
		if (gClassifySpecsPtr->channelSet == kAllMenuItem)
			LoadFeatureVector (gClassifySpecsPtr->featureTransformationFlag,
										(SInt16*)&gClassifySpecsPtr->numberFeatures,
										featurePtr,
										gProjectInfoPtr->numberStatisticsChannels);
															
		LoadChannelsVector (kProject,
									gClassifySpecsPtr->featureTransformationFlag,
									gClassifySpecsPtr->numberFeatures, 
									featurePtr, 
									&gClassifySpecsPtr->numberChannels,
									channelsPtr);
		
		if (gClassifySpecsPtr->classSet == kAllMenuItem)								
			LoadTrainClassVector (
								&gClassifySpecsPtr->numberClasses,
								&gClassifySpecsPtr->classSet, gClassifySpecsPtr->classPtr);
		
		if (gClassifySpecsPtr->classAreaSet == kAllMenuItem)	
			LoadClassAreaVector (&gClassifySpecsPtr->numberClassAreas, classAreaPtr);
			
		if (gClassifySpecsPtr->symbolSet == kDefaultMenuItem)
			LoadClassSymbolVector (gClassifySpecsPtr->symbolsPtr, 
											IDS_Symbol1, 
											gProjectInfoPtr->numberStatisticsClasses);
			
		}	// end "if (continueFlag)" 
	
			// If unable to set up information for classification						
			// specifications, release memory allocated to it (if any).				
			
	else	// !continueFlag 
		ReleaseClassifySpecsMemory (&gProjectInfoPtr->classifySpecsH);
	                      	                  
	return (continueFlag);

}	// end "LoadClassifySpecs" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean LoadProbabilityTable
//
//	Software purpose:	The purpose of this routine is to load the probability
//							table that will be used to compute the average
//							classification probability for the area.
//
//	Parameters in:			
//
//	Parameters out:	None
//
// Value Returned: 
//
// Called By:			CreateThresholdTables in SClassify.cpp
//
//	Coded By:			Larry L. Biehl			Date: 07/17/1992
//	Revised By:			Larry L. Biehl			Date: 03/29/1994	

Boolean LoadProbabilityTable (void)

{
	SInt16					*thresholdProbabilityPtr;
	
	Boolean					continueFlag;
	UInt16					index;
	SInt16					probability,
								probabilityStep;
	
																			
	continueFlag = (gClassifySpecsPtr->thresholdProbabilityPtr != NULL);
			
	if (continueFlag)
		{
		thresholdProbabilityPtr = gClassifySpecsPtr->thresholdProbabilityPtr;
		
		*thresholdProbabilityPtr = 0;
		thresholdProbabilityPtr++;
			
		probability = 0;
		probabilityStep = 0;
		
		for (index=1; index<=gNumberProbabilityClasses; index++)
			{
			if (index == 12)
				probability = 1;
				
			if (index == 13)
				{
				probability = 5;
				probabilityStep = 10;
					
				}	// end "if (index == 13)" 
				
			if (index == 23)
				{
				probability = 150;
				probabilityStep = 100;
					
				}	// end "if (index == 23)" 
				
			*thresholdProbabilityPtr = probability;
			probability += probabilityStep;
			thresholdProbabilityPtr++;
				
			}	// end "for (index=1; index<=..." 
			
		}	// end "if (continueFlag)" 
		
	return (continueFlag);
			
}	// end "LoadProbabilityTable" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean LoadThresholdTable
//
//	Software purpose:	The purpose of this routine is to create the threshold
//							tables that will be used for the classification
//							probability map.
//
//	Parameters in:		Number of degrees of freedom to use in creating the table.
//
//	Parameters out:		
//
// Value Returned:  	None
//
// Called By:			CreateThresholdTables in SClassify.cpp
//
//	Coded By:			Larry L. Biehl			Date: 07/17/1992
//	Revised By:			Larry L. Biehl			Date: 01/13/1998	

Boolean LoadThresholdTable (
				SInt16 								numberDegreesOfFreedom, 
				SInt16								tableCode)

{
	SInt16*								tableDegreesOfFreedomPtr;
	double*								thresholdTablePtr;
	
	Boolean								continueFlag;
	
	
	continueFlag = (numberDegreesOfFreedom > 0);
		
	if (continueFlag)																			
		continueFlag = (gClassifySpecsPtr->thresholdTablePtr != NULL);

			// Put description in the status dialog.										
					
	LoadDItemStringNumber (kClassifyStrID, 
									IDS_Classify19, 
									gStatusDialogPtr, 
									IDC_Status11, 
									(Str255*)gTextString);
			
	if (continueFlag)
		{
				// Get pointers to the threshold table and the current degrees of 
				// freedom represented by the table											
				
		if (tableCode == 1)
			{
			thresholdTablePtr = gClassifySpecsPtr->thresholdTablePtr;
			tableDegreesOfFreedomPtr =
											&gClassifySpecsPtr->pixelThresholdDegreesOfFreedom;
			
			}	// end "if (tableCode == 1)" 
			
		else	// tableCode != 1 
			{
			thresholdTablePtr = 
							&gClassifySpecsPtr->thresholdTablePtr[gNumberProbabilityClasses];
			tableDegreesOfFreedomPtr = &gClassifySpecsPtr->cellThresholdDegreesOfFreedom;
			
			}	// end "else tableCode != 1" 
			
				// Create the table if needed.												
		
		if (*tableDegreesOfFreedomPtr != numberDegreesOfFreedom)
			continueFlag = CreateChiSquareThresholdTable (numberDegreesOfFreedom,
																			thresholdTablePtr);
		
		if (continueFlag)
			*tableDegreesOfFreedomPtr = numberDegreesOfFreedom;
	
		}	// end "if (continueFlag)" 
		
	return (continueFlag);
			
}	// end "LoadThresholdTable"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void MaximumLikelihoodClassifier
//
//	Software purpose:	The purpose of this routine is to classify the
//							input line of data using the per-point maximum 
//							likelihood classifer.
//
//	Parameters in:		Pointer to area description structure
//							Pointer to image file information structure
//							Pointer to temporary classification variable structure
//							Pointer to buffer to be used to store the classification
//								results in for a line.
//							Pointer to a buffer to be used to store the probability
//								of correct classification information in for a line.
//							Pointer to a vector to be used to store the count of the
//								number of pixels classified into each class
//							A structure which defines the start line and column values
//								that the classification will begin at.	
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			ClassifyPerPointArea
//
//	Coded By:			Larry L. Biehl			Date: 12/15/1988
//	Revised By:			Larry L. Biehl			Date: 03/27/2012

SInt16 MaximumLikelihoodClassifier (
				AreaDescriptionPtr				areaDescriptionPtr, 
				FileInfoPtr							fileInfoPtr,
				ClassifierVarPtr					clsfyVariablePtr, 
				HUCharPtr							outputBuffer1Ptr, 
				HUCharPtr							probabilityBufferPtr, 
				HSInt64Ptr							countVectorPtr, 
				Point									point)

{
	Boolean								polygonField,
											continueFlag,
											createProbabilities;
	RgnHandle							rgnHandle;
	
	double								*classConstantPtr,
											*thresholdTablePtr,
						 					*workVectorPtr;
	
	double								discriminantMax,
											dValue1,
											dValue2,
											neg_rrDivide2,
											neg_rrDivide2Max;
	
	SInt16	 							*classPtr,
											*thresholdProbabilityPtr; 
												            
	HDoublePtr 							chanMeanPtr, 
											covariancePtr,
											ioBufferReal8Ptr,
											savedBufferReal8Ptr;
												 
	double*		 						meanDiffPtr;
	
	UInt32								classIndex,
											feat1,
											feat2,
											maxClass,
											numberChannels,
											numberClasses,
											numberSamplesPerChan,
											sameDistanceCount,
											sample;
											
			// For Quiong special case
			
	AreaDescription					projectAreaDescription;
	double								*workVector2Ptr;
	
	
			// Initialize local variables.														
	
	numberChannels = 			gClassifySpecsPtr->numberChannels;
	numberClasses = 			gClassifySpecsPtr->numberClasses;
	numberSamplesPerChan = 	(UInt32)areaDescriptionPtr->numSamplesPerChan;
	polygonField =				areaDescriptionPtr->polygonFieldFlag;
	rgnHandle = 				areaDescriptionPtr->rgnHandle;
	classPtr = 					gClassifySpecsPtr->classPtr;
	savedBufferReal8Ptr = 	(HDoublePtr)outputBuffer1Ptr;
	maxClass = 					0;
	
	createProbabilities = 	gClassifySpecsPtr->createThresholdTableFlag;
	
	classConstantPtr = 		clsfyVariablePtr->classConstantPtr;
	workVectorPtr = 			clsfyVariablePtr->workVectorPtr;
	thresholdTablePtr =	 	gClassifySpecsPtr->thresholdTablePtr;
	thresholdProbabilityPtr = gClassifySpecsPtr->thresholdProbabilityPtr;
	
	sameDistanceCount =		0;
	
	continueFlag = TRUE;
	
  	if (gTestFlag)
  		{
				// For special case for Quiong
				
		workVector2Ptr = clsfyVariablePtr->workVector2Ptr;
		
				// Initialize project area description in case it is needed.
			
		InitializeAreaDescription (&projectAreaDescription, 
												1, 
												1, 
												1, 
												1, 
												1, 
												1,
												1,
												1,
												0);
												
		}	// end "if (gTestFlag)"
		
			// Loop through the number of samples in the line of data				
	
	for (sample=0; sample<numberSamplesPerChan; sample++)
		{
		if (!polygonField || PtInRgn (point, rgnHandle))
			{
					// Loop through the classes and get the discriminant value for	
					// each class.																	
		
			discriminantMax = -1e200;
			covariancePtr = clsfyVariablePtr->covariancePtr;
			chanMeanPtr = clsfyVariablePtr->chanMeanPtr;
			
			for (classIndex=0; classIndex<numberClasses; classIndex++)
				{
						// Loop through the channels (features) and get the vector  
						// of data values with the class means subtracted out.		
						
				ioBufferReal8Ptr = savedBufferReal8Ptr;
				meanDiffPtr = workVectorPtr;
				for (feat1=0; feat1<numberChannels; feat1++)
					{
					*meanDiffPtr = *ioBufferReal8Ptr - *chanMeanPtr;
					
					ioBufferReal8Ptr++;
					chanMeanPtr++;
					meanDiffPtr++;
					
					}	// end "for (feature=0; feature<..." 
					
						// Compute discriminant value for class "statClassNumber"		
				
				dValue1 = 0.;
				
				for (feat1=0; feat1<numberChannels; feat1++)
					{
					meanDiffPtr = workVectorPtr;
					dValue2 = 0.;					
					for (feat2=0; feat2<feat1; feat2++)
						{
						dValue2 -= *covariancePtr * *meanDiffPtr;
						covariancePtr++;
						meanDiffPtr++;	
					
						}	// end "for (feat2=0; feat2<..." 
						
					dValue2 += dValue2;
					dValue1 += *meanDiffPtr * (dValue2 - *covariancePtr * *meanDiffPtr);
					covariancePtr++;
					
					}	// end "for (feat1=0; feat1<..." 
					
				neg_rrDivide2 = dValue1/2;
				dValue2 = classConstantPtr[classIndex];
				
				if (clsfyVariablePtr->useLeaveOneOutMethodFlag && 
										classPtr[classIndex] == areaDescriptionPtr->classNumber)
					{
					dValue1 = -dValue1;
					
					dValue2 = clsfyVariablePtr->classConstantLOO1Ptr[classIndex]*dValue1 +
						clsfyVariablePtr->classConstantLOO5Ptr[classIndex] * dValue1*dValue1;
											
					dValue2 /= 2 * (clsfyVariablePtr->classConstantLOO2Ptr[classIndex] -
								clsfyVariablePtr->classConstantLOO5Ptr[classIndex] * dValue1);
					
					neg_rrDivide2 -= dValue2;
					
					dValue2 = classConstantPtr[classIndex];
											
					dValue2 -= 
							.5 * log (1 - clsfyVariablePtr->classConstantLOO3Ptr[classIndex] *
												dValue1);
											
					dValue2 -= .5 * clsfyVariablePtr->classConstantLOO4Ptr[classIndex];
					
					}	// end "if (clsfyVariablePtr->useLeaveOneOutMethodFlag)"
					
				dValue2 += neg_rrDivide2;
				
						// Check if discriminant value for this class is largest.  If	
						// so, save the value and the class number.							
						
				if (dValue2 > discriminantMax)
					{
					discriminantMax = dValue2;
					neg_rrDivide2Max = neg_rrDivide2;
					maxClass = classIndex;
					sameDistanceCount = 0;
					
					}	// end "if (dValue1 > discriminantMax)"
					
				else if (dValue2 == discriminantMax)
					sameDistanceCount++;
				
				//if (gTestFlag)	
				//	workVector2Ptr[classIndex] = dValue2;
	
				}	// end "for (classIndex=0; classIndex<..."
			/*         
         if (gTestFlag)
         	{	
				if (IsProjectData (&clsfyVariablePtr->maskAreaDescription,
											&projectAreaDescription,
											areaDescriptionPtr->line, 
											(UInt32)point.h))
					dValue1 = 1.;
					
				else	// !IsProjectData (...
					{										
		          		// Adjust the gaussian exponent values down so that the likelihood
		          		// value can be computed with the fewest numeric problems.
	         	
	         	dValue2 = 0.;		
		         for (classIndex=0; classIndex<numberClasses; classIndex++) 
		         	{	
						workVector2Ptr[classIndex] -= discriminantMax;
						
						dValue2 += exp (0.5 * workVector2Ptr[classIndex]);
						
		          	}	// end "for (classIndex=0; classIndex<..."
		          	
					dValue1 = 1./dValue2;
		          	
		        	}	// end "else !IsProjectData (..."
	          	
	         		// Get the ratio of the likelihood value.
		
				sprintf ((char*)gTextString, 
					"%10.8f%s", 
					dValue1, 
					gEndOfLine);
								
				continueFlag = OutputString (NULL, 
														(char*)gTextString, 
														0, 
														gOutputForce1Code, 
														continueFlag);
	          	
	          }	// end "if (gTestFlag)"
			*/				
			classIndex = classPtr[maxClass];
		   countVectorPtr[classIndex]++;
		   *outputBuffer1Ptr = (UInt8)classIndex;
		   
		   		// Get distance measure if requested.									
		   		// dValue1 is the variable for the probability.						
		   
		   if (createProbabilities)
		   	{
		   			// Get the threshold table index.									
					
				classIndex = GetThresholdClass (-neg_rrDivide2Max, 
															thresholdTablePtr);
				
				*probabilityBufferPtr = (UInt8)classIndex;
				probabilityBufferPtr++;
				
				gTempDoubleVariable1 += thresholdProbabilityPtr[classIndex];
				
				}	// end "if (createProbabilities)" 
		   
		   }	// end "if (!polygonField || PtInRgn (point, rgnHandle))" 
		   
		else	// polygonField && !PtInRgn (point, rgnHandle) 
			{
			*outputBuffer1Ptr = 0;
		   if (createProbabilities)
		   	{
				*probabilityBufferPtr = 0;
				probabilityBufferPtr++;
				
				}	// end "if (createProbabilities)" 
				
			sameDistanceCount = 0;
			
			}	// end "else polygonField && !PtInRgn (point, rgnHandle)" 
			
		point.h++;
		savedBufferReal8Ptr += numberChannels;
		outputBuffer1Ptr++;
		
		clsfyVariablePtr->totalSameDistanceSamples += sameDistanceCount;
			
				// Exit routine if user selects "cancel" or "command period".		
		
		if (TickCount () >= gNextTime)
			{
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
																						return (3);
				
			}	// end "if (TickCount () >= gNextTime)" 
				
		}	// end "for (sample=0; sample<..." 
	
			// Write carriage return and null character to classification record	
			
   *outputBuffer1Ptr = kCarriageReturn;
   outputBuffer1Ptr++;  
                      
	#if defined multispec_win
   	*outputBuffer1Ptr = kLineFeed;
  		outputBuffer1Ptr++;
	#endif	// defined multispec_win  
	
   *outputBuffer1Ptr = kNullTerminator;
   
   return (0);

}	// end "MaximumLikelihoodClassifier" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void MaxLikeClsfierControl
//
//	Software purpose:	The purpose of this routine is to classify the
//							specified data using the per point maximum likelihood 
//							classifer.
//
//	Parameters in:		Pointer to image file information structure
//
//	Parameters out:	None
//
// Value Returned:	Flag indicating whether output to text window
//								can still be generated.
// 
// Called By:			ClassifyControl
//
//	Coded By:			Larry L. Biehl			Date: 12/10/1988
//	Revised By:			Larry L. Biehl			Date: 01/21/2016

void MaxLikeClsfierControl (
				FileInfoPtr							fileInfoPtr)

{
	ClassifierVar						classifierVar;
	
	double								clsfierConstant,
											logDeterminant,
											weightValue;
	
	CMFileStream*						resultsFileStreamPtr;
	
	double								*classConstantLOO1Ptr,
											*classConstantLOO2Ptr,
											*classConstantLOO3Ptr,
											*classConstantLOO4Ptr,
											*classConstantLOO5Ptr,
											*classConstantPtr;
												
	HChannelStatisticsPtr			classChannelStatsPtr;
												
	HDoublePtr							classifyChanMeanPtr,
											classifyCovPtr,
											inverseCovPtr;
	
	float									totalProbability;
	
	SInt16*								statFeaturePtr;
	
	UInt32								clsfyCovStart,
											clsfyChanMeanStart,
											numberClsfyCovEntries;
												
	SInt16								weightsIndex;
	
	UInt16								numberFeatureChannels,
											numberFeatures;
	
	UInt16								classIndex;
	UInt16								statClassNumber;
	
	Boolean								continueClassifyFlag,
											continueFlag,
											determinantOKFlag;
		
		
			// Initialize local variables														
	
	continueFlag = TRUE;
	numberFeatureChannels = gClassifySpecsPtr->numberChannels;
	numberFeatures = gClassifySpecsPtr->numberFeatures;
 	resultsFileStreamPtr = GetResultsFileStreamPtr (0);
	
			// Put description in the status dialog.										
					
	LoadDItemStringNumber (kClassifyStrID, 
									IDS_Classify20, 
									gStatusDialogPtr, 
									IDC_Status11, 
									(Str255*)gTextString);
		
	InitializeClassifierVarStructure (&classifierVar);
	
			// Get the memory for the mean vectors and covariance matrices			
			// for the classes and channels to be used in the classification.		
				 
	continueFlag = SetupClsfierMemory (&classifyChanMeanPtr,
													&classifyCovPtr,
													&classConstantPtr,
													&classConstantLOO1Ptr,
													&classConstantLOO2Ptr,
													&classConstantLOO3Ptr,
													&classConstantLOO4Ptr,
													&classConstantLOO5Ptr, 
													&classChannelStatsPtr, 
													&gInverseMatrixMemory.inversePtr,
													&gInverseMatrixMemory.pivotPtr, 
													&gInverseMatrixMemory.indexRowPtr, 
													&gInverseMatrixMemory.indexColPtr, 
													&gInverseMatrixMemory.ipvotPtr);
	inverseCovPtr = gInverseMatrixMemory.inversePtr;
											
	UInt16* featurePtr = (UInt16*)GetHandlePointer (gClassifySpecsPtr->featureHandle);
		
	if (continueFlag)			
		continueFlag = SetupFeatureTransformationMemory (
													gClassifySpecsPtr->featureTransformationFlag,
													numberFeatures, 
													numberFeatureChannels, 
													&gTransformationMatrix.eigenVectorPtr, 
													&gTransformationMatrix.tempMatrixPtr,
													NULL, 
													&gTransformationMatrix.eigenFeaturePtr,
													kLoadMatricesVectors,
													featurePtr);
	
			// Continue with classification, if memory is not full.					
	
	if (continueFlag)
		{
				// Normalize a priori probabilities.										
				
		weightsIndex = GetProjectClassWeightsIndex ();
				
		totalProbability = (float)GetTotalProbability (
														(UInt16*)gClassifySpecsPtr->classPtr,
														(SInt16)gClassifySpecsPtr->numberClasses,
														weightsIndex);
		
				// Get the constant for the classification, i.e.						
				// (number-of-features/2 * log (2pi))										
				
		clsfierConstant = (double)numberFeatures/2 * log (kTWO_PI);
		
				// Loop through the classes and for classes to be used in 			
				// classification get the:														
				//		mean vector for class.													
				//		covariance matrix for class.											
				//		determinant of covariance matrix	for class.						
				//		inverse of covariance matrix for class.							
				//		discriminant constant for class.										
		
		clsfyChanMeanStart = 0;
		clsfyCovStart = 0;
											
		numberClsfyCovEntries = (UInt32)numberFeatureChannels * 
																		(numberFeatureChannels+1)/2;
		
		statFeaturePtr = (SInt16*)GetStatisticsFeatureVector (
													gClassifySpecsPtr->featureTransformationFlag,
													featurePtr);
		
		determinantOKFlag = TRUE;
		continueClassifyFlag = TRUE;
		for (classIndex=0; 
				classIndex<gClassifySpecsPtr->numberClasses; 
					classIndex++)
			{
			statClassNumber = gClassifySpecsPtr->classPtr[classIndex] - 1;
			
					// Get InvCov or Tt_InvTCov_T	

			continueFlag = GetTtInvTransformedCovarianceT (
																numberFeatureChannels,
																classChannelStatsPtr, 
																inverseCovPtr,
																(UInt16*)statFeaturePtr, 
																statClassNumber,
																kSquareOutputMatrix,
																kMeanCovariance,
																gTransformationMatrix.eigenVectorPtr,
																gTransformationMatrix.tempMatrixPtr,
																numberFeatures,
																&logDeterminant,
																&determinantOKFlag);
			
			if (!continueFlag)
				break;
				
					// Get the class mean vector												
			
			ReduceMeanVector (classChannelStatsPtr, 
									&classifyChanMeanPtr[clsfyChanMeanStart],
									numberFeatureChannels,
									NULL);
									
			clsfyChanMeanStart += numberFeatureChannels;
			
			if (determinantOKFlag)
				{					
				if (gClassifySpecsPtr->mode == kMahalanobisMode)
					logDeterminant = 0;
						
						// Copy the full form (square) matrix back to triangular		
						// form storage. This takes less space and makes the 			
						// classification algorithm a little easier to implement 	
						// and quicker.															
				
				clsfyCovStart = classIndex * numberClsfyCovEntries;
				SquareToTriangularMatrix (inverseCovPtr,
													&classifyCovPtr[clsfyCovStart],
													numberFeatureChannels);
							
						// Get the constant for the class									
						
				weightValue = GetClassWeightValue (
												statClassNumber, weightsIndex, totalProbability);
						
				classConstantPtr [classIndex] =
										log (weightValue) - clsfierConstant - logDeterminant/2;
		
						// Variables to use in case the leave-one-out method is used to
						// compute the training statistics.
											
				UInt16 classStorage = gProjectInfoPtr->storageClass[statClassNumber];
				SInt64 numberTrainPixels = 
						gProjectInfoPtr->classNamesPtr[classStorage].numberStatisticsPixels;
							
				if (numberTrainPixels >= 3)
					{
					classConstantLOO1Ptr[classIndex] = (double)(
						(numberTrainPixels*numberTrainPixels - 3*numberTrainPixels + 1))/
																					(numberTrainPixels-1);
					classConstantLOO2Ptr[classIndex] = 
										(double)(numberTrainPixels-1) * (numberTrainPixels-1);
												
					classConstantLOO3Ptr[classIndex] = numberTrainPixels/
																	classConstantLOO2Ptr[classIndex];
																	
					classConstantLOO4Ptr[classIndex] = 
									numberFeatures * log (1 + (double)1/(numberTrainPixels-2));
					
					}	// end "if (numberTrainPixels >= 3)"
					
				else	// numberTrainPixels < 3
					{
					classConstantLOO1Ptr[classIndex] = 0;
					classConstantLOO2Ptr[classIndex] = 0;
					classConstantLOO3Ptr[classIndex] = 0;
					classConstantLOO4Ptr[classIndex] = 0;
					
					}	// end "else numberTrainPixels < 3"
					
				classConstantLOO5Ptr[classIndex] = (double)numberTrainPixels;
							
				}	// end "if (determinantOKFlag)" 
				
			else	// !determinantOKFlag 
				{
						// List message that covariance matrix for this class 		
						// could not be inverted.												
					
				continueFlag = ListClassInformationMessage (kProjectStrID,
																			IDS_Project31,
																			resultsFileStreamPtr, 
																			gOutputForce1Code,
																			statClassNumber, 
																			continueFlag);
				
				continueClassifyFlag = FALSE;
				
				}	// end "else !determinantOKFlag" 
				
			}	// end "for (class=0; class<Classify..." 
			
		}	// end "if (continueFlag)" 
			
			// Release memory for the matrix inversion work areas.					
		
	gInverseMatrixMemory.indexRowPtr =
										CheckAndDisposePtr (gInverseMatrixMemory.indexRowPtr);
	gInverseMatrixMemory.indexColPtr =
										CheckAndDisposePtr (gInverseMatrixMemory.indexColPtr);
	gInverseMatrixMemory.inversePtr =
										CheckAndDisposePtr (gInverseMatrixMemory.inversePtr);
	gInverseMatrixMemory.ipvotPtr = CheckAndDisposePtr (gInverseMatrixMemory.ipvotPtr);
	
			// Release memory for the classify channels statistics.					
			
	classChannelStatsPtr = 
					(HChannelStatisticsPtr)CheckAndDisposePtr ((Ptr)classChannelStatsPtr);
	
			// Release memory for transformation matrix information.					
			
	ReleaseFeatureTransformationMemory ();
					
			// Get memory for test likelihood ratio if needed.
	
	if (gTestFlag)		
		classifierVar.workVector2Ptr = (HDoublePtr)MNewPointer (
											gClassifySpecsPtr->numberClasses * sizeof (double));
	
	if (continueFlag)
		{
		
				// Classify the requested areas.	First load the maximum				
				// likelihood variable structure with need pointers					
		
		classifierVar.chanMeanPtr = 			classifyChanMeanPtr;
		classifierVar.covariancePtr = 		classifyCovPtr;
		classifierVar.classConstantPtr = 	classConstantPtr;
		classifierVar.classConstantLOO1Ptr = classConstantLOO1Ptr;
		classifierVar.classConstantLOO2Ptr = classConstantLOO2Ptr;
		classifierVar.classConstantLOO3Ptr = classConstantLOO3Ptr;
		classifierVar.classConstantLOO4Ptr = classConstantLOO4Ptr;
		classifierVar.classConstantLOO5Ptr = classConstantLOO5Ptr;
		classifierVar.workVectorPtr = 		gInverseMatrixMemory.pivotPtr;
		
		if (continueClassifyFlag)
			{
					// If thresholding is to be used, get table of threshold 		
					// values corresponding to fractions rejected from 0.1 to 		
					// 1.0 percent by 0.1 percent increments and from 1.0 to 		
					// 99 percent by 1.0 percent increments.								

			if (gClassifySpecsPtr->createThresholdTableFlag)
				continueFlag = CreateThresholdTables (numberFeatures, 0);
			
			if (continueFlag)
				ClassifyAreasControl (fileInfoPtr, &classifierVar);
			
			}	// end "continueClassifyFlag" 
				
		else	// !continueClassifyFlag 
			{
					// List message indicating that classification was cancelled	
					// because covariance matrix for at least one class could not	
					// be inverted.																
		
			continueFlag = ListSpecifiedStringNumber (kClassifyStrID, 
																	IDS_Classify4,
																	(unsigned char*)gTextString, 
																	resultsFileStreamPtr, 
																	gOutputForce1Code,
																	continueFlag);
			
			SysBeep (10);
			
			}	// else !continueClassifyFlag 
			
		}	// end "if (continueFlag)" 
			
			// Release memory for class mean vectors, class covariance				
			// matrices, matrix inversion work areas, and class constants.										
			
	CheckAndDisposePtr (classifyChanMeanPtr);
	CheckAndDisposePtr (classifyCovPtr);
	CheckAndDisposePtr (classConstantPtr);
	CheckAndDisposePtr (classConstantLOO1Ptr);
	
	ReleaseMatrixInversionMemory ();
	
	if (gTestFlag)	
		classifierVar.workVector2Ptr = CheckAndDisposePtr (classifierVar.workVector2Ptr);

}	// end "MaxLikeClsfierControl" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ParallelPipedClassifier
//
//	Software purpose:	The purpose of this routine is to classify the
//							input line of data using the per-point Parallel Piped classifer.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			ClassifyArea
//
//	Coded By:			Larry L. Biehl			Date: 03/29/2012
//	Revised By:			Larry L. Biehl			Date: 03/30/2012

SInt16 ParallelPipedClassifier (
				AreaDescriptionPtr				areaDescriptionPtr, 
				FileInfoPtr							fileInfoPtr,
				ClassifierVarPtr					clsfyVariablePtr, 
				HUCharPtr							outputBuffer1Ptr, 
				HUCharPtr							probabilityBufferPtr, 
				HSInt64Ptr							countVectorPtr, 
				Point									point)

{
	RgnHandle							rgnHandle;
	
	HDoublePtr							ioBufferReal8Ptr,
											savedBufferReal8Ptr;
	
	double								*chanMaxPtr,
											*chanMinPtr;
	
	SInt16								*classPtr;
	
	UInt32								classIndex,
											feat,
											numberChannels,
											numberClasses,
											numberSamplesPerChan,
											sameDistanceCount,
											sample;
										
	SInt16								inboxClass;
												
	Boolean								outsideBoxFlag,
											polygonFieldFlag;
	
	
			// Initialize local variables.														
	
	numberChannels = 			gClassifySpecsPtr->numberChannels;
	numberClasses = 			gClassifySpecsPtr->numberClasses;
	numberSamplesPerChan = 	(UInt32)areaDescriptionPtr->numSamplesPerChan;
	rgnHandle = 				areaDescriptionPtr->rgnHandle;
	polygonFieldFlag =		areaDescriptionPtr->polygonFieldFlag;
	classPtr =					gClassifySpecsPtr->classPtr;
	savedBufferReal8Ptr =	(HDoublePtr)outputBuffer1Ptr;
		
			// Loop through the number of samples in the line of data				
		
	for (sample=0; sample<numberSamplesPerChan; sample++)
		{
		if (!polygonFieldFlag || PtInRgn (point, rgnHandle))
			{
					// Loop through the classes and determine which the first class
					// for which the pixel is within the parallel piped "box".																
		
			chanMinPtr = clsfyVariablePtr->chanMeanPtr;
			chanMaxPtr = clsfyVariablePtr->covariancePtr;
				
			inboxClass = -1;
			sameDistanceCount = 0;
			for (classIndex=0; classIndex<numberClasses; classIndex++)
				{
				ioBufferReal8Ptr = savedBufferReal8Ptr;
				outsideBoxFlag = FALSE;
				
						// Determine if the pixel is between min and max for all channels	
				
				for (feat=0; feat<numberChannels; feat++)
					{
					if (*ioBufferReal8Ptr < *chanMinPtr || *ioBufferReal8Ptr > *chanMaxPtr)
						{
						chanMinPtr += numberChannels-feat;
						chanMaxPtr += numberChannels-feat;
						outsideBoxFlag = TRUE;
						break;
						
						}	// end "for (feat=0; feat<numberChannels; feat++)"
					
					ioBufferReal8Ptr++;
					chanMinPtr++;
					chanMaxPtr++;
											
					}	// end "for (feat=0; feat<..."
				
				if (!outsideBoxFlag)
					{
					if (inboxClass == -1)
						inboxClass = (SInt16)classIndex;
					
					else	// inboxClass already set
						sameDistanceCount++;
						
					}	// end "if (!outsideBoxFlag)"
	
				}	// end "for (classIndex=0; classIndex<..."
				
			if (inboxClass >= 0)
				{
				countVectorPtr[classPtr[inboxClass]]++;
				*outputBuffer1Ptr = (UInt8)classPtr[inboxClass];
				
				}	// end "if (inboxClass > 0)"
				
			else	// inboxClass == -1. Not in any class
				{
				countVectorPtr[0]++;
				*outputBuffer1Ptr = 0;
				
				}	// end "else inboxClass == -1. Not in any class"

		   }	// end "if (!polygonField || PtInRgn (point, rgnHandle))" 
		   
		else	// polygonField && !PtInRgn (point, rgnHandle) 
			*outputBuffer1Ptr = 0; 
			
		point.h++;
		outputBuffer1Ptr++;
		savedBufferReal8Ptr += numberChannels;
		
		clsfyVariablePtr->totalSameDistanceSamples += sameDistanceCount;
			
				// Exit routine if user selects "cancel" or "command period".		
		
		if (TickCount () >= gNextTime)
			{
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
																								return (3);
				
			}	// end "if (TickCount () >= gNextTime)" 
				
		}	// end "for (sample=0; sample<..." 
	
			// Write carriage return and null character to classification record	
			
   *outputBuffer1Ptr = kCarriageReturn;
   outputBuffer1Ptr++;    
                      
	#if defined multispec_win  
   	*outputBuffer1Ptr = kLineFeed;
  		outputBuffer1Ptr++; 
	#endif	// defined multispec_win  
	
   *outputBuffer1Ptr = kNullTerminator;
   
   return (0);

}	// end "ParallelPipedClassifier" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ParallelPipedClsfierControl
//
//	Software purpose:	The purpose of this routine is to control the
//							classification of the specified data using the point 
//							Parallel Piped classifier.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			ClassifyPerPointArea
//
//	Coded By:			Larry L. Biehl			Date: 03/29/2012
//	Revised By:			Larry L. Biehl			Date: 03/30/2012

void ParallelPipedClsfierControl (
				FileInfoPtr							fileInfoPtr)

{
	double								meanOffset,
											standardDeviationFactor;
	
	HChannelStatisticsPtr			channelStatsPtr;
	ClassifierVar						classifierVar;
	
	HDoublePtr 							classChanMaximumPtr,
											classChanMinimumPtr,
											covariancePtr;
	
	UInt32								clsfyChanStart,
											classIndex,
											feat,
											numberClsfyChannels,
											statClassNumber,
											doubleVectorLength;
											
	Boolean								continueFlag;
	
		
			// Initialize local variables														
			
	numberClsfyChannels = gClassifySpecsPtr->numberChannels;
	
	InitializeClassifierVarStructure (&classifierVar);
	
			// Get the memory for the mean vectors and covariance matrices			
			// for the classes and channels to be used in the classification.		
				 
	continueFlag = SetupClsfierMemory (&classChanMinimumPtr, 
													&covariancePtr,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL, 
													&channelStatsPtr, 
													NULL, 
													NULL, 
													NULL, 
													NULL, 
													NULL);
	
			// Continue with classification, if memory is not full.					
	
	if (continueFlag)
		{
				// Get the pointer for the maximum vector
				// Also get pointers for the mean and standard deviation vectors to have
				// read if needed.
				
		doubleVectorLength = gClassifySpecsPtr->numberClasses * 
																	gClassifySpecsPtr->numberChannels;
				
		classChanMaximumPtr = &classChanMinimumPtr[doubleVectorLength];
		
				// Save the pointers needed for the parallel piped classification					
		
		classifierVar.chanMeanPtr = classChanMinimumPtr;
		classifierVar.covariancePtr = classChanMaximumPtr;
		
				// Loop through the classes and for classes to be used in 			
				// classification get the:														
				//		mean vector constant.													
		
		clsfyChanStart = 0;											
			
		UInt16* featurePtr = (UInt16*)GetHandlePointer (
																	gClassifySpecsPtr->featureHandle);
			
		if (gClassifySpecsPtr->parallelPipedCode == kPPMinMaxCode)
			{		
			for (classIndex=0; 
					classIndex<gClassifySpecsPtr->numberClasses; 
						classIndex++)
				{
				statClassNumber = gClassifySpecsPtr->classPtr[classIndex] - 1;
				
						// Get the class minimum and maximum vectors	
							
				GetClassMinimumVector ((UInt16)numberClsfyChannels,
												channelStatsPtr, 
												&classChanMinimumPtr[clsfyChanStart], 
												featurePtr, 
												(UInt16)statClassNumber);
				
				GetClassMaximumVector ((UInt16)numberClsfyChannels,
												channelStatsPtr, 
												&classChanMaximumPtr[clsfyChanStart], 
												featurePtr, 
												(UInt16)statClassNumber);
				
				clsfyChanStart += numberClsfyChannels;
				
				}	// end "for (classIndex=0; classIndex<Classify..."
				
			}	// end "if (gClassifySpecsPtr->parallelPipedCode == kPPMinMaxCode)"
				
		else	// ...parallelPipedCode == kPPStandardDeviationCode
			{
			standardDeviationFactor = gClassifySpecsPtr->parallelPipedStanDevFactor;
			
			for (classIndex=0; 
					classIndex<gClassifySpecsPtr->numberClasses; 
						classIndex++)
				{
				statClassNumber = gClassifySpecsPtr->classPtr[classIndex] - 1;
				
						// Get the class mean and standard deviation vectors	
						
				GetClassCovarianceMatrix ((UInt16)numberClsfyChannels,
													channelStatsPtr, 
													covariancePtr, 
													featurePtr, 
													(UInt16)statClassNumber,
													kTriangleOutputMatrix,
													kMeanStdDevOnly,
													kOriginalStats);
				
				for (feat=0; feat<numberClsfyChannels; feat++)
					{
					meanOffset = standardDeviationFactor * channelStatsPtr[feat].standardDev;
					
					*classChanMinimumPtr = channelStatsPtr[feat].mean - meanOffset;
					*classChanMaximumPtr = channelStatsPtr[feat].mean + meanOffset;
											
					classChanMinimumPtr++;
					classChanMaximumPtr++;
												
					}	// end "for (feat=0; feat<numberClsfyChannels; feat++)"
								
				clsfyChanStart += numberClsfyChannels;
				
				}	// end "for (classIndex=0; classIndex<Classify..."
				
			}	// end "else ...parallelPipedCode == kStandardDeviationCode"
	
				// Release memory for vector/matrices not needed any more.					
			
		channelStatsPtr = 
						(HChannelStatisticsPtr)CheckAndDisposePtr ((Ptr)channelStatsPtr);
			
		covariancePtr = (double*)CheckAndDisposePtr ((Ptr)covariancePtr);
		
				// Classify the requested areas.					
		
		if (continueFlag)
			ClassifyAreasControl (fileInfoPtr, &classifierVar);
			
		}	// end "if (continueFlag)" 
			
			// Release memory.										
			
	CheckAndDisposePtr (classifierVar.chanMeanPtr);

}	// end "ParallelPipedClsfierControl" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean SetupClsfierMemory
//
//	Software purpose:	The purpose of this routine is to setup the 
//							memory allocated for the classification statistics.
//
//	Parameters in:					
//
//	Parameters out:	None
//
// Value Returned:	
// 
// Called By:			CEMClsfierControl in SClassify.cpp
//							CorrelationClsfierControl in SClassify.cpp
//							EuclideanClsfierControl in SClassify.cpp
//							FisherClsfierControl in SClassify.cpp
//							MaxLikeClsfierControl in SClassify.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/13/1988
//	Revised By:			Larry L. Biehl			Date: 08/27/1997	

Boolean SetupClsfierMemory (
				HDoublePtr*							channelMeanPtr,
				HDoublePtr*							covariancePtr,
				double**								classConstantPtr,
				double**								classConstantLOO1PtrPtr,
				double**								classConstantLOO2PtrPtr,
				double**								classConstantLOO3PtrPtr,
				double**								classConstantLOO4PtrPtr,
				double**								classConstantLOO5PtrPtr,
				HChannelStatisticsPtr*			classChannelStatsPtr,
				HDoublePtr*							inverseCovPtr,
				HDoublePtr*							pivotPtr,
				SInt16**								indexRowPtr,
				SInt16**								indexColPtr,
				SInt16**								ipvotPtr)

{
	Boolean								continueFlag;
	UInt32								bytesNeeded;
	
	
			// Initialize parameters															
			
	continueFlag = TRUE;
	
	if (channelMeanPtr)
		*channelMeanPtr = 		NULL;
		
	if (covariancePtr)
		*covariancePtr = 			NULL;
		
	if (classConstantPtr)
		*classConstantPtr = 		NULL;
		
	if (classConstantLOO1PtrPtr)
		*classConstantLOO1PtrPtr = NULL;
		
	if (classConstantLOO2PtrPtr)
		*classConstantLOO2PtrPtr = NULL;
		
	if (classConstantLOO3PtrPtr)
		*classConstantLOO3PtrPtr = NULL;
		
	if (classConstantLOO4PtrPtr)
		*classConstantLOO4PtrPtr = NULL;
		
	if (classConstantLOO5PtrPtr)
		*classConstantLOO5PtrPtr = NULL;
		
	if (classChannelStatsPtr)
		*classChannelStatsPtr = NULL;
	
	if (inverseCovPtr)
		*inverseCovPtr = 			NULL;
	
	if (pivotPtr)
		*pivotPtr = 				NULL;
	
	if (indexRowPtr)
		*indexRowPtr =				NULL;
	
	if (indexColPtr)
		*indexColPtr = 			NULL;
	
	if (ipvotPtr)
		*ipvotPtr = 				NULL;
	
			// Get pointer to memory to use for class mean vectors.					
	
	if (channelMeanPtr != NULL)
		{		
		UInt32 numberClasses = (UInt32)gClassifySpecsPtr->numberClasses;
		if (gClassifySpecsPtr->mode == kFisherMode)
			numberClasses++;
			
		if (gClassifySpecsPtr->mode == kParallelPipedMode)
					// Allow for minimum, maximum, mean & std dev vectors
			numberClasses *= 2;
			
		bytesNeeded = (UInt32)numberClasses *
										gClassifySpecsPtr->numberChannels * sizeof (double);
		*channelMeanPtr = (HDoublePtr)MNewPointer (bytesNeeded);
		continueFlag = (*channelMeanPtr != NULL);
			
		}	// end "if (channelMeanPtr != NULL)" 
		
	if (continueFlag && covariancePtr != NULL)
		{
				// Get pointer to memory to use for class covariance matrix.
		
		if (gClassifySpecsPtr->mode == kFisherMode || 
						gClassifySpecsPtr->mode == kCEMMode || 
									gClassifySpecsPtr->mode == kParallelPipedMode)			
			bytesNeeded = (UInt32)gClassifySpecsPtr->numberChannels *
											gClassifySpecsPtr->numberChannels * sizeof (double);
		
		else if (gClassifySpecsPtr->mode == kCorrelationMode)			
			bytesNeeded = (UInt32)(gClassifySpecsPtr->numberClasses+1) * 
									gClassifySpecsPtr->numberChannels *
												(gClassifySpecsPtr->numberChannels + 1)/2 *
																							sizeof (double);
		
		else	// ...->mode != kFisherMode || ->mode != kCorrelationMode || ...			
			bytesNeeded = (UInt32)gClassifySpecsPtr->numberClasses * 
									gClassifySpecsPtr->numberChannels *
											(gClassifySpecsPtr->numberChannels + 1)/2 *
																							sizeof (double);
									
		*covariancePtr = (HDoublePtr)MNewPointer (bytesNeeded);
		continueFlag = (*covariancePtr != NULL);
			
		}	// end "if (continueFlag && covariancePtr != NULL)" 
		
	if (continueFlag && classConstantPtr != NULL)
		{
				// Get pointer to memory to use for class contants.
		
		UInt32		factor = 1;		
		if (gClassifySpecsPtr->mode == kFisherMode)
			factor = 2;				
					
		bytesNeeded = factor * gClassifySpecsPtr->numberClasses * sizeof (double);
		*classConstantPtr = (double *)MNewPointer (bytesNeeded);
		continueFlag = (*classConstantPtr != NULL);
			
		}	// end "if (continueFlag && classConstantPtr)"  
		
	if (continueFlag && classConstantLOO1PtrPtr != NULL)
		{
				// Get pointer to memory to use for class leave-one-out contants.
				// Currently the LOO contants 1 and 2 will always exist together,
				// so get the memory as one chunk and split up via the pointers.
					
		bytesNeeded = 5*gClassifySpecsPtr->numberClasses * sizeof (double);
		*classConstantLOO1PtrPtr = (double*)MNewPointer (bytesNeeded);
		continueFlag = (*classConstantLOO1PtrPtr != NULL);
		
		if (continueFlag && classConstantLOO2PtrPtr != NULL)
			*classConstantLOO2PtrPtr = 
							&(*classConstantLOO1PtrPtr)[gClassifySpecsPtr->numberClasses];
		
		if (continueFlag && classConstantLOO3PtrPtr != NULL)
			*classConstantLOO3PtrPtr = 
							&(*classConstantLOO1PtrPtr)[2*gClassifySpecsPtr->numberClasses];
		
		if (continueFlag && classConstantLOO4PtrPtr != NULL)
			*classConstantLOO4PtrPtr = 
							&(*classConstantLOO1PtrPtr)[3*gClassifySpecsPtr->numberClasses];
		
		if (continueFlag && classConstantLOO5PtrPtr != NULL)
			*classConstantLOO5PtrPtr = 
							&(*classConstantLOO1PtrPtr)[4*gClassifySpecsPtr->numberClasses];
			
		}	// end "if (continueFlag && classConstantLOO1PtrPtr != NULL)" 
		
	if (continueFlag && classChannelStatsPtr != NULL)
		{
					// Get pointer to memory to use for class channel statistics	
					// that will be used in computing the covariance matrix and		
					// getting the data for the class mean vector.						
					
		bytesNeeded = (UInt32)gClassifySpecsPtr->numberChannels *
																			sizeof (ChannelStatistics);
									
		*classChannelStatsPtr = (HChannelStatisticsPtr)MNewPointer (bytesNeeded);
		continueFlag = (*classChannelStatsPtr != NULL);
			
		}	// end "if (continueFlag && classChannelStatsPtr != NULL)" 
		
			// Get the memory needed for matrix inversion.								
			 
	if (continueFlag)
		continueFlag = SetupMatrixInversionMemory (gClassifySpecsPtr->numberChannels,
																	TRUE,
																	inverseCovPtr, 
																	pivotPtr, 
																	indexRowPtr, 
																	indexColPtr, 
																	ipvotPtr);
	
	return (continueFlag);
		
}	// end "SetupClsfierMemory"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 SupportVectorMachineClassifier
//
//	Software purpose:	The purpose of this routine is to classify the input line of
//							data using the per-point support vector machine classifer.
//
//	Parameters in:
//
//	Parameters out:	None
//
// Value Returned:	0: completed with no error
//
// Called By:			ClassifyPerPointArea
//
//	Coded By:			Tsung Tai Yeh			Date: 08/??/2019
//	Revised By:			Larry L. Biehl			Date: 09/30/2019

SInt16 SupportVectorMachineClassifier (
				AreaDescriptionPtr 				areaDescriptionPtr,
				FileInfoPtr                   fileInfoPtr,
				ClassifierVarPtr              clsfyVariablePtr,
				HUCharPtr                     outputBuffer1Ptr,
				HUCharPtr                     probabilityBufferPtr,
				HSInt64Ptr                    countVectorPtr,
				Point                         point)

{
	RgnHandle                     rgnHandle;
	
   HDoublePtr                    ioBufferReal8Ptr,
											savedBufferReal8Ptr;
	
   int									maxClass;
	
   SInt16                        *classPtr;
	
   UInt32                        feat,
											numberChannels,
											numberClasses,
											numberProjectClasses,
											numberSamplesPerChan,
											sample;
	
   Boolean                       polygonField;
	
	
   		// Initialize local variables.
	
   numberChannels = 			gClassifySpecsPtr->numberChannels;
   numberClasses =			gClassifySpecsPtr->numberClasses;
   numberSamplesPerChan =	(UInt32)areaDescriptionPtr->numSamplesPerChan;
   rgnHandle = 				areaDescriptionPtr->rgnHandle;
   polygonField = 			areaDescriptionPtr->polygonFieldFlag;
   numberProjectClasses =	gProjectInfoPtr->numberStatisticsClasses;
   classPtr = 					gClassifySpecsPtr->classPtr;
   savedBufferReal8Ptr = (HDoublePtr)outputBuffer1Ptr;
	
   		// Loop through the number of samples in the line of data
	
   maxClass = 0;

   for (sample=0; sample<numberSamplesPerChan; sample++)
   	{
      if (!polygonField || PtInRgn (point, rgnHandle))
      	{
         		// Loop through the classes and get the discriminant value for
         		// each class.
			
         ioBufferReal8Ptr = savedBufferReal8Ptr;
		
         for (feat=0; feat<numberChannels; feat++)
         	{
            gProjectInfoPtr->svm_x[feat].value = *ioBufferReal8Ptr;
            ioBufferReal8Ptr++;
				
         	}	// end "for (feat=0; feat<numberChannels; feat++)"
			
         		// the end index of one channel = -1
			
         //gProjectInfoPtr->svm_x[numberChannels].index = -1;
			
         		// SVM classifies data
			
         maxClass = (int)(svm_predict (gProjectInfoPtr->svmModel, gProjectInfoPtr->svm_x));
         if (maxClass > 0 && maxClass <= numberProjectClasses)
         	{
         	countVectorPtr[maxClass]++;
         	*outputBuffer1Ptr = (UInt8)maxClass;
				
				}	// end "if (maxClass > 0 && maxClass <= numberProjectClasses)"
			
			else	// maxClass <= 0 || maxClass > numberClasses)
         	*outputBuffer1Ptr = 0;
			
      	}   // end "if (!polygonField || PtInRgn (point, rgnHandle))"
	
      else   // polygonField && !PtInRgn (point, rgnHandle)
      	{
         *outputBuffer1Ptr = 0;
		
      	}   // end "else polygonField && !PtInRgn (point, rgnHandle)"
	
      point.h++;
      outputBuffer1Ptr++;
      savedBufferReal8Ptr += numberChannels;
	
      		// Exit routine if user selects "cancel" or "command period".
	
      if (TickCount () >= gNextTime)
      	{
         if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
            return (3);
		
      	}   // end "if (TickCount () >= gNextTime)"
	
   	}   // end "for (sample=0; sample<..."
	
   		// Write carriage return and null character to classification record
	
   *outputBuffer1Ptr = kCarriageReturn;
   outputBuffer1Ptr++;
	
	#if defined multispec_win
		*outputBuffer1Ptr = kLineFeed;
		outputBuffer1Ptr++;
	#endif   // defined multispec_win
	
   *outputBuffer1Ptr = kNullTerminator;

   return (0);
	
}   // end "SupportVectorMachineClassifier"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SupportVectorMachineClassifierControl
//
//	Software purpose:	The purpose of this routine is to control the classification of
//							the specified data using the point support vector machine
//							classifier.
//
//	Parameters in:
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			ClassifyPerPointArea
//
//	Coded By:			Larry L. Biehl			Date: 09/13/2019
//	Revised By:			Larry L. Biehl			Date: 09/13/2019

void SupportVectorMachineClassifierControl (
				FileInfoPtr							fileInfoPtr)

{
	ClassifierVar						classifierVar;
	
	UInt32								numberClsfyChannels;
	
	Boolean								continueFlag;
	
	
			// Initialize local variables
	
	numberClsfyChannels = gClassifySpecsPtr->numberChannels;
	
	InitializeClassifierVarStructure (&classifierVar);
	
			// Get the memory for the mean vectors and covariance matrices
			// for the classes and channels to be used in the classification.
	
	continueFlag = SetupClsfierMemory (NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL);
	
			// Continue with classification, if memory is not full.
	
	if (continueFlag)
		{
				// Classify the requested areas.	First load the maximum
				// likelihood variable structure with need pointers
		
		classifierVar.chanMeanPtr = NULL;
		classifierVar.classConstantLOO1Ptr = NULL;
		
		ClassifyAreasControl (fileInfoPtr, &classifierVar);
		
		}	// end "if (continueFlag)"

}	// end "SupportVectorMachineClassifierControl"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 WriteClassificationResults
//
//	Software purpose:	The purpose of this routine is to write the classification
//							results to the request files and windows.
//
//	Parameters in:			
//
//	Parameters out:	None
//
// Value Returned:
//
// Called By:			ClassifyPerPointArea in SClassify.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/08/1992
//	Revised By:			Larry L. Biehl			Date: 08/26/2010	

SInt16 WriteClassificationResults (
				HUCharPtr							outputBufferPtr,
				AreaDescriptionPtr				areaDescriptionPtr, 
				CMFileStream*						resultsFileStreamPtr, 
				CMFileStream*						clProbabilityFileStreamPtr, 
				HSInt64Ptr							countVectorPtr, 
				SInt32								lineNumber)

{
	SInt64*								thresholdClassCountPtr;
	
	unsigned char*						symbolsPtr;
	
	HUCharPtr							ioBuffer1Ptr,
											probabilityBufferPtr;
	HUInt16Ptr							ioBuffer2Ptr;
	
	UInt32								count,
											sample;
										
	SInt16								errCode,
											thresholdCode;
	
			
	count 			=  (UInt32)areaDescriptionPtr->numSamplesPerChan;
	ioBuffer1Ptr 	= outputBufferPtr;
	probabilityBufferPtr = &ioBuffer1Ptr[gClassifySpecsPtr->probabilityBufferOffset];
	thresholdClassCountPtr = &countVectorPtr[0];
	thresholdCode = gClassifySpecsPtr->probabilityThresholdCode;
	 
	 if (gClassifySpecsPtr->thresholdFlag && thresholdCode > 0) 
	 	{
	 			// Threshold the classification if requested.						
	 			//	If a pixel is thresholded then update the table.				
	 			
		for (sample=0; sample<count; sample++)
			{
			if (probabilityBufferPtr[sample] <= thresholdCode)
   			{
   			countVectorPtr[ioBuffer1Ptr[sample]]--;
   			(*thresholdClassCountPtr)++;
   			ioBuffer1Ptr[sample] = 0;
   			
   			}	// end "if (probabilityBufferPtr[sample] <= ..." 
   			
   		}	// end "for (sample=0; sample<count; sample++)" 
   		
   	}	// end "if (...->thresholdFlag && thresholdCode > 0)" 
								
			// Write classification line to ERDAS (binary) disk file if requested.
	
	if (gOutputCode & kClassifyFileCode)
		{
		if (gOutputFormatCode == kGAIAType)
			{
					// Load the data into GAIA format - 2 byte, byte reversed 	
					// and preline information.											
					
			ioBuffer2Ptr = (HUInt16Ptr)outputBufferPtr;
			ioBuffer2Ptr = &ioBuffer2Ptr[gClassifySpecsPtr->outputBufferOffset];
			
			count = ConvertLineToGAIAFormat (ioBuffer1Ptr,
														ioBuffer2Ptr,
														count,
														lineNumber);
	
					// The preline information has been set up such that when 		
					// the buffer point to the output data is increase by 1, 		
					// the data will be byte reversed as the GAIA format requires.	
							
			ioBuffer1Ptr = (HUCharPtr)ioBuffer2Ptr;
			ioBuffer1Ptr++;
			
			}	// end "if (gOutputFormatCode == kGAIAType)" 
		
		errCode = MWriteData (resultsFileStreamPtr, 
										&count, 
										ioBuffer1Ptr,
										kErrorMessages);	
		if (errCode != noErr)
			{
			gOutputCode = (gOutputCode & 0xfff3);
			gOutputForce1Code = gOutputCode;
			if (!gOutputCode)
																								return (1);
			
			}	// end "if (errCode = FSWrite (..." 
																				
		ioBuffer1Ptr 	= (HUCharPtr)outputBufferPtr;
		count 			=  (UInt32)areaDescriptionPtr->numSamplesPerChan;
			
		}	// end "if (gOutputCode & kClassifyFileCode)" 
			
	if (gOutputCode & kProbFormatCode)
		{
		errCode = MWriteData (clProbabilityFileStreamPtr, 
										&count, 
										probabilityBufferPtr,
										kErrorMessages);	
		if (errCode != noErr)
			{
			gOutputCode = (gOutputCode & 0xfff3);
			gOutputForce1Code = gOutputCode;
			if (!gOutputCode)
																								return (1);
			
			}	// end "if (errCode != noErr" 
			
		}	// end "if (gOutputCode & kProbFormatCode)" 
			
			// Check if classified data needs to be converted to ASCII symbol	
			// format.  If so then convert the data and write the data out as	
			// requested.																		
	
	if (gOutputCode & 0x0003)
		{
		symbolsPtr = gClassifySpecsPtr->symbolsPtr;
		
		for (sample=0; sample<count; sample++)
	   	ioBuffer1Ptr[sample] = symbolsPtr[ioBuffer1Ptr[sample]];
	   
				// Write classification line to output window if requested.		 
                      
		#if defined multispec_mac
			count++;
		#endif	// defined multispec_mac 	 
                      
		#if defined multispec_win
	   	count += 2;
		#endif	// defined multispec_win 

		if (!OutputString (resultsFileStreamPtr, 
									(HPtr)ioBuffer1Ptr,
									count,
									gOutputCode, 
									TRUE))
																								return (1);
				
		}	// end "if (gOutputCode & 0x0003)" 
		
	return (0);
			
}	// end "WriteClassificationResults" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void WriteProbabilityGrouping
//
//	Software purpose:	The purpose of this routine is to write the probability
//							grouping information to the end of the probability image
//							file if one is being created.
//
//	Parameters in:		Probability file stream pointer	
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			in SClassify.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/10/1996
//	Revised By:			Larry L. Biehl			Date: 01/22/2016	

void WriteProbabilityGrouping (
				SInt16								classificationMode)

{
			
	if (gOutputCode & kProbFormatCode && (
				classificationMode == kMaxLikeMode ||
						classificationMode == kMahalanobisMode ||
								classificationMode == kFisherMode ||
										classificationMode == kEchoMode))
		{
		CMFileStream*		clProbabilityFileStreamPtr;
		
		
		clProbabilityFileStreamPtr = 	GetResultsFileStreamPtr (1);
		
		if (clProbabilityFileStreamPtr != NULL)
			{
			StringHandle		stringHandle = NULL;
			CharPtr				stringPtr = NULL;
			UInt32				count;
			SInt16				errCode;
			
		
					// Get the string with the probability grouping information.
					
			count = GetSpecifiedString (137, &stringHandle, &stringPtr);
			
			if (count > 0)
				{
				count--;	
				errCode = MWriteData (clProbabilityFileStreamPtr, 
												&count,
												&stringPtr[1],
												kErrorMessages);
											
				}	// end "if (count > 0)"
		
			CheckAndUnlockHandle ((Handle)stringHandle);
			ReleaseResource ((Handle)stringHandle);
				
			}	// end "if (clProbabilityFileStreamPtr != NULL)" 
				
		}	// end "if (gOutputCode & kProbFormatCode && ..." 
			
}	// end "WriteProbabilityGrouping"
