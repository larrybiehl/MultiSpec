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
//	File:						SFeatureSelection.cpp (Called separability in LARSYS)
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/14/2019
//
//	Language:				C
//
//	System:					Linux, Macintosh and Windows Operating Systems
//
//	Brief description:	This file contains the routines dealing with separability
//								measures of the class statistics.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"      

#if defined multispec_wx        
	#include "xFeatureSelectionDialog.h"
#endif	// defined multispec_wx

#if defined multispec_mac || defined multispec_mac_swift
	#define	IDC_ListTitle						7
	#define	IDC_First							9
	#define	IDC_Last								11
	#define	IDC_Interval						13
	#define	IDC_EnterNewRange					14
	#define	IDC_SelectionCount				16
	
	#define	IDC_FeatureTransformation 		6
	#define	IDC_ChannelPrompt					7
	#define	IDC_NumberChannelsPrompt		10
	#define	IDC_ChannelsPerGroupPrompt		12
	#define	IDC_MaxSearchCombinations		16
	#define	IDC_NumberBestToList				18
	#define	IDC_NumberClassCombinations	22
	
	#define	IDC_SeparabilityTable			 4
	#define	IDC_NumberBestCombinations		 6
	#define	IDC_PossibleCombinations		 7
	#define	IDC_MinDistanceRange				 9
	#define	IDC_MaxDistanceRange				11
	#define	IDC_IncludeChannelComboPrompt	15
	#define	IDC_IncludeChannelCombo			16
	#define	IDC_ExcludeChannelComboPrompt	17
	#define	IDC_ExcludeChannelCombo			18
	#define	IDC_ClassPairThresholdTable	19
	#define	IDC_ClassPairThreshold			20
	#define	IDC_ClassGroupThresholdTable	21
	#define	IDC_ClassGroupThreshold			22
#endif	// defined multispec_mac || defined multispec_mac_swift
  
#if defined multispec_win        
	#include "WFeatureSelectionDialog.h"
#endif	// defined multispec_win 



		// Declarations of variables used only in this file.							
		
		// pointer to separability specification structure								
SeparabilitySpecsPtr					gSeparabilitySpecsPtr;

		// Pointer to the lower triangular portion of the covariance				
		// matrices for the given channels and classes.									
HDoublePtr								gSepCovPtr;

		// Pointer to the log of determinants of the covariance matrices for 	
		// each of the classes.																	
HDoublePtr								gSepLogDetPtr;

		// Pointer to the lower triangular portion of the inverse covariance		
		// matrices for the given channels and classes.									
HDoublePtr								gSepInvPtr;

		// Pointer to the mean vectors for the given channels and  classes.		
HDoublePtr								gSepMeanPtr;

		// Variable indicating what the current channel combination selection 	
		// type is.  It is used by processor dialogs.  									
		//		=1 is all channel combinations, =2 is subset.							
SInt16									gChannelCombinationSelection;



		// Declarations of structures used only in this file.							
		
typedef struct SepDistancesSummary
	{
	double					classPairMean;
	double					classPairMinimum;
	UInt32					channelSetIndex;
	SInt32					classPairDistancesIndex;
	SInt32					previousMean;
	SInt32					nextMean;
	SInt32					previousMinimum;
	SInt32					nextMinimum;
	
	} SepDistancesSummary, *SepDistancesSummaryPtr;


 
		// Function prototypes for routines that are only called by other			
		// routines in this file.	
								
void CalculateSeparabilityControl (void);

Boolean ChannelCombinationsDialog (
				UInt16*								numberOutputChannelCombinationsPtr,
				UInt16*								channelCombinationsPtr,
				UInt32								numberInputChannelCombinations, 
				UInt16*								allChanCombinationsPtr,
				UInt32								contiguousChannelsPerGroup,
				Boolean								featureTransformationFlag);

SInt16 ComputeSeparabilityForAllChannelCombinations (
				UInt32								numberChannelGroupCombinations,
				UInt32								numberClassCombinations, 
				SInt16								numberSeparabilityChannels);

SInt16 ComputeSeparabilityForAllClassCombinations (
				SepDistancesSummaryPtr			distancesSummaryPtr,
				UInt32								numberClassCombinations, 
				UInt32								numberClasses, 
				SInt16								numberSeparabilityChannels);

SInt16 ComputeSeparabilityForStepChannelCombinations (
				UInt32								numberChannelGroupCombinations,
				UInt32								numberClassCombinations, 
				SInt16								numberSeparabilityChannels);

double CovarianceBhattacharyya (
				HDoublePtr							cov1Ptr,
				HDoublePtr							cov2Ptr,				
				double								logDeterminant1,
				double								logDeterminant2,
				SInt16								numberFeatures);

double Divergence (
				HDoublePtr							mean1Ptr,
				HDoublePtr							mean2Ptr,
				HDoublePtr							cov1Ptr,
				HDoublePtr							cov2Ptr,
				HDoublePtr							inv1Ptr,
				HDoublePtr							inv2Ptr,
				HDoublePtr							meanDifPtr,
				SInt16								numberFeatures);

PascalVoid DrawChannelCombinationsPopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber);

double EFBhattacharyya (
				HDoublePtr							mean1Ptr,
				HDoublePtr							mean2Ptr,
				HDoublePtr							cov1Ptr,
				HDoublePtr							cov2Ptr,
				double								logDeterminant1,
				double								logDeterminant2,
				HDoublePtr							meanDifPtr,
				SInt16								numberFeatures);

SInt32 GetCombinationLimit (
				SInt32								total,
				SInt32								numberContiguousPerGroup, 
				Boolean								stepSearchFlag);

UInt32 GetMaxCombinationSelected (
				SInt32								numberSubsets,
				SInt16*								subsetPtr, 
				SInt32								total, 
				SInt32								numberContiguousPerGroup, 
				Boolean								stepSearchFlag,
				SInt16*								numberCombinationReturnCode);

Boolean GetNextCombination (
				SInt16*								listPtr,
				SInt32								subsetNumber, 
				SInt32								totalNumber, 
				SInt32								numberContiguousPerGroup);

Boolean GetNextStepCombination (
				SInt16*								listPtr,
				SInt16*								listUsedPtr,
				SInt16*								nextChannelPtr, 
				SInt16								totalNumber, 
				SInt16								numberContiguousPerGroup);
/*
double GetProductVectorValues (
				HDoublePtr							vectorPtr,
				UInt32								numberFeatures); 
*/
Boolean InvertVectorValues (
				HDoublePtr							inputVectorPtr,
				HDoublePtr							outputVectorPtr, 
				UInt32								numberFeatures);

Boolean ListSeparabilityTitleLine (
				CMFileStream*						resultsFileStreamPtr,
				UInt32								numberClassCombinations, 
				SInt16								numberSeparabilityChannels);

SInt16 ListSeparabilityMeasurements (
				UInt32								numberChannelGroupCombinations,
				UInt32								numberClassCombinations, 
				SInt16								numberSeparabilityChannels);

SInt32 LoadAllChannelCombinationsVector (
				SInt16								totalNumberChannels,
				UInt16*								combinationPtr, 
				SInt32*								maxNumberCombinationsPtr,
				SInt16								numberContiguousPerGroup, 
				Boolean								stepSearchFlag);

void LoadChannelCombinationsVector (
				SeparabilitySpecsPtr				separabilitySpecsPtr,
				UInt16*								channelCombinationsPtr, 
				UInt16*								allChanCombinationsPtr,
				UInt16*								numberChannelGroupCombinationsPtr, 
				UInt32*								maxNumberFeatureCombinationsPtr);

Boolean LoadSeparabilitySpecs (void);

SInt16 LoadSeparabilityStatistics (
				SInt16*								featurePtr,
				SInt16								numberFeatures);

double MeanBhattacharyya (
				HDoublePtr							mean1Ptr,
				HDoublePtr							mean2Ptr,
				HDoublePtr							cov1Ptr, 
				HDoublePtr							cov2Ptr,
				HDoublePtr							meanDifPtr,	
				SInt16								numberFeatures);

double NonCovarianceBhattacharyya (
				HDoublePtr							mean1Ptr,
				HDoublePtr							mean2Ptr,
				HDoublePtr							cov1Ptr,
				HDoublePtr							cov2Ptr,
				HDoublePtr							meanDifPtr,
				UInt32								numberFeatures);

Boolean SeparabilityDialog (
				FileInfoPtr							fileInfoPtr);

void SeparabilityDialogUpdateMaxChannelsPerGroup (
				UInt16								numberFeatures,
				UInt16*								channelCombinationsPtr,
				UInt16*								numberChannelGroupCombinationsPtr,
				SInt32*								maxContiguousChannelsPerGroupPtr); 

Boolean SetupSeparabilityDistanceMemory (
				SInt16								numberSeparabilityChannels,
				SInt16*								featurePtr, 
				SInt16								numberFeatures, 
				UInt32								numberClassCombinations, 
				UInt32								numberChannelGroupCombinations);

Boolean SetupSeparabilityStatMemory (
				HDoublePtr* 						meanPtr,
				HDoublePtr* 						covariancePtr, 
				HDoublePtr* 						inverseCovPtr, 
				HDoublePtr*							logDeterminantPtr, 
				HChannelStatisticsPtr*			classChannelStatsPtr, 
				SInt16								numberFeatures,
				Boolean								covarianceFlag);

double TransformedDivergence (
				HDoublePtr							mean1Ptr,
				HDoublePtr							mean2Ptr, 
				HDoublePtr							cov1Ptr, 
				HDoublePtr							cov2Ptr, 
				HDoublePtr							inv1Ptr, 
				HDoublePtr							inv2Ptr, 
				HDoublePtr							meanDifPtr, 
				SInt16								numberFeatures);

SInt16 UpdateSeparabilityLists (
				SepDistancesSummaryPtr 			distancesSummaryPtr,
				SInt32	 							combinationIndex, 
				UInt32				 				channelCombination, 
				SInt32* 								freeIndex, 
				Boolean 								firstTimeFlag);


                                                                                
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double EFBhattacharyya
//
//	Software purpose:	The purpose of this routine is to calculate
//							the Error Function Bhattacharyya distance between the two 
//							classes represented by their input covariance matrices, 
//							determinants and mean vectors.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:	
//
//	Coded By:			C.H. Lee					Date: 12/04/1988
//	Revised By:			Larry L. Biehl			Date: 06/30/1994	

double EFBhattacharyya (
				HDoublePtr							mean1Ptr, 		// mean vector of class 1 
				HDoublePtr							mean2Ptr, 		// mean vector of class 2 
				HDoublePtr							cov1Ptr, 		// covariance matrix of class 1 
				HDoublePtr							cov2Ptr,			// covariance matrix of class 2 
				double								logDeterminant1, 	// log of determinant of 
																				//	class 1 covariance matrix	
				double								logDeterminant2,	// log of determinant of 
																	//	class 2 covariance matrix	
				HDoublePtr							meanDifPtr, 	// vector storage of class 1 and 	
																// class 2 mean difference. 			
				SInt16							numberFeatures)	// number of features (channels) 
																// represented in the mean vector and 	
																// covariance and inverse covariance 	
																// matrices.																						
											
{
	double								bhattacharyya1,
											bhattacharyya2,
											areaOfSND;
											

	bhattacharyya1 = Bhattacharyya (
									mean1Ptr,
									mean2Ptr,
									cov1Ptr,
									cov2Ptr,
									logDeterminant1,
									logDeterminant2,
									meanDifPtr,
									numberFeatures);
									
			// This is the error function bhattacharyya.  							
			// The value of areaOfSND represents the area of the			
			// standard normal distribution.												
			
	bhattacharyya2 = sqrt (2.*bhattacharyya1);
	Area_Of_SND_by_Direct_Calculation (bhattacharyya2, &areaOfSND);
	bhattacharyya1 = areaOfSND + 0.5;
		
	return (bhattacharyya1);
	
}	// end "EFBhattacharyya" 


                                                                                
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CalculateSeparabilityControl
//
//	Software purpose:	The purpose of this routine is to control the
//							calculation of the separability measures and 
//							listing of the results.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			SeparabilityControl
//
//	Coded By:			Larry L. Biehl			Date: 10/26/1989
//	Revised By:			Larry L. Biehl			Date: 05/01/1998

void CalculateSeparabilityControl (void)

{
	CMFileStream*						resultsFileStreamPtr;
	SInt16*								channelCombinationsPtr;
	
	SInt32								maxNumberOfListCombinations;
											
	UInt32								numberChannelGroupCombinations,
											numberClassCombinations;
	
	SInt16								channelCombinationSet,
											index,
											numberChannelGroupCombinationSets,
											numberCombinationReturnCode,
											numberSeparabilityChannels,
											returnCode;
	
	Boolean								continueFlag;
	

			// Initialize local variables.													
	
	numberChannelGroupCombinationSets = 
								gSeparabilitySpecsPtr->numberChannelGroupCombinations;
	channelCombinationsPtr = (SInt16*)GetHandlePointer (
										gSeparabilitySpecsPtr->channelCombinationsHandle);
	resultsFileStreamPtr = GetResultsFileStreamPtr (0);
	continueFlag = TRUE;
																
			// Get the number of class combinations to be used and check for		
			// errors.																				
			
	numberClassCombinations = GetNumberOfCombinations (
						(SInt16)gSeparabilitySpecsPtr->numberClasses, 
						2, 
						1, 
						FALSE,
						&numberCombinationReturnCode);
												
	if (numberClassCombinations == 0)
		{
		if (numberCombinationReturnCode == -2)
		
				//"  There are too many class combinations to be considered\r"		
		
			continueFlag = ListSpecifiedStringNumber (
										kFeatureSelectionStrID, 
										IDS_FeatureSelection14, 
										(unsigned char*)gTextString, 
										resultsFileStreamPtr, 
										gOutputForce1Code,
										continueFlag);
									
																								return;
			
		}	// end "if (numberClassCombinations == 0)" 
		
			// List the number of class combinations to be considered.				
			
			// " There are %ld class combination(s).\r"									
	
	index = IDS_FeatureSelection15;						
	if (numberClassCombinations > 1)
		index = IDS_FeatureSelection16;										
							
	continueFlag = ListSpecifiedStringNumber (
										kFeatureSelectionStrID, 
										index,
										resultsFileStreamPtr, 
										gOutputForce1Code, 
										(double)numberClassCombinations, 
										continueFlag);
	
			// Get the memory for the mean vectors, covariance matrices and		
			// inverse matrices for the classes and channels to be used in 		
			// in the separability calculations.  Also get memory for 				
			// computing the covariance matrix inverse.  Use the largest number	
			// of channels to be used for separability calculations.					
			// If memory is full, then branch to clean up at the end of this 		
			// routine.																				
		
	if (continueFlag)
		continueFlag = SetupSeparabilityStatMemory (
				&gSepMeanPtr, 
				&gSepCovPtr, 
				&gSepInvPtr, 
				&gSepLogDetPtr, 
				&gTempChannelStatsPtr,
				channelCombinationsPtr[numberChannelGroupCombinationSets-1] *
										gSeparabilitySpecsPtr->numberContiguousPerGroup,
				(gSeparabilitySpecsPtr->distanceMeasure != kNonCovarianceBhattacharyya));
	
	if (continueFlag)	
		continueFlag = SetupFeatureTransformationMemory (
					gSeparabilitySpecsPtr->featureTransformationFlag, 
					channelCombinationsPtr[numberChannelGroupCombinationSets-1] *
							gSeparabilitySpecsPtr->numberContiguousPerGroup, 
					gSeparabilitySpecsPtr->numberChannels, 
					&gTransformationMatrix.eigenVectorPtr, 
					&gTransformationMatrix.tempMatrixPtr,
					&gTransformationMatrix.offsetVectorPtr, 
					&gTransformationMatrix.eigenFeaturePtr,
					kDoNotLoadMatricesVectors,
					NULL);
	
	if (!continueFlag)																		
																						goto cleanUp1;
			
			// We continue here, if memory is not full.									
												
	LoadDItemValue (gStatusDialogPtr, 
							IDC_Status5, 
							(SInt32)numberChannelGroupCombinationSets);
							
			// Make sure that messages for classes will be listed only once.
			
	gListOnlyOneMessagePerClassFlag = TRUE;
			
	for (channelCombinationSet=0; 
			channelCombinationSet<numberChannelGroupCombinationSets;
			channelCombinationSet++)
		{
		LoadDItemValue (gStatusDialogPtr, 
								IDC_Status3, 
								(SInt32)channelCombinationSet+1);
		
				// Make sure that the class list message flag is set so that the
				// first message if any will be listed.
				
		SetClassListMessageFlag (TRUE);
		
		numberSeparabilityChannels = 
					channelCombinationsPtr[channelCombinationSet] * 
										gSeparabilitySpecsPtr->numberContiguousPerGroup;
												
				// Get the number of channel combinations to be used and check		
				// for errors.																		
				
		numberChannelGroupCombinations = GetNumberOfCombinations (
										gSeparabilitySpecsPtr->numberFeatures, 
										channelCombinationsPtr[channelCombinationSet],
										gSeparabilitySpecsPtr->numberContiguousPerGroup,
										(gSeparabilitySpecsPtr->featureSearchCode==2),
										&numberCombinationReturnCode);
												
		if (numberChannelGroupCombinations == 0)
			{
			if (numberCombinationReturnCode == -2)
				// "  There are too many channel combinations to be considered\r"	
		
				continueFlag = ListSpecifiedStringNumber (
										kFeatureSelectionStrID, 
										IDS_FeatureSelection17, 
										(unsigned char*)gTextString, 
										resultsFileStreamPtr, 
										gOutputForce1Code,
										continueFlag);
										
																						goto cleanUp2;
																							
			}	// end "if (numberChannelGroupCombinations == 0)" 
		
				// List the number of channel/feature combinations to be 			
				// considered.																		
				
		maxNumberOfListCombinations = numberChannelGroupCombinations;
		if (gSeparabilitySpecsPtr->featureSearchCode == 2)
			maxNumberOfListCombinations = 
					1 + gSeparabilitySpecsPtr->numberFeatures - 
						channelCombinationsPtr[channelCombinationSet] * 
										gSeparabilitySpecsPtr->numberContiguousPerGroup;
				
		sprintf ((char*)gTextString2, "channel feature"); 
		gTextString2[7] = kNullTerminator;
		
		sprintf ((char*)gTextString, 
			"    There are %u %s combination(s) for %hd group(s) of %hd"
			" contiguous %s(s).%s",
			(int)maxNumberOfListCombinations,
			&gTextString2[gSeparabilitySpecsPtr->featureTransformationFlag*8],
			channelCombinationsPtr[channelCombinationSet],
			gSeparabilitySpecsPtr->numberContiguousPerGroup,
			&gTextString2[gSeparabilitySpecsPtr->featureTransformationFlag*8],
			gEndOfLine);
							
		if (numberChannelGroupCombinations == 1)											
			sprintf ((char*)&gTextString[10], " is");
		gTextString[13] = ' ';
							
		continueFlag = OutputString (resultsFileStreamPtr, 
												(char*)gTextString, 
												0, 
												gOutputCode, 
												continueFlag);
								
		if (gSeparabilitySpecsPtr->distanceMeasure != kNonCovarianceBhattacharyya)
			{
			SInt16* classPtr = (SInt16*)GetHandlePointer (
							gSeparabilitySpecsPtr->classHandle);	
			if (!CheckNumberOfPixelsInClass (
									gSeparabilitySpecsPtr->numberClasses, 
									classPtr, 
									numberSeparabilityChannels))
																						goto cleanUp2;
																						
			}	// end "if (gSeparabilitySpecsPtr->distanceMeasure != ...)" 
			
		if (continueFlag)
			{						
			SInt16* featurePtr = (SInt16*)GetHandlePointer (
							gSeparabilitySpecsPtr->featureHandle);	
																									
			continueFlag = SetupSeparabilityDistanceMemory (
										numberSeparabilityChannels,
										featurePtr,
										gSeparabilitySpecsPtr->numberFeatures, 
										numberClassCombinations, 
										numberChannelGroupCombinations);
										
			}	// end "if (continueFlag)"
		
		if (continueFlag)
			{
			if (gSeparabilitySpecsPtr->featureSearchCode == 1)
				returnCode = ComputeSeparabilityForAllChannelCombinations (
											numberChannelGroupCombinations, 
											numberClassCombinations, 
											numberSeparabilityChannels);
						
			else if (gSeparabilitySpecsPtr->featureSearchCode == 2)
				returnCode = ComputeSeparabilityForStepChannelCombinations (
											numberChannelGroupCombinations, 
											numberClassCombinations, 
											numberSeparabilityChannels);
			
			}	// end "if (continueFlag)" 
			
		cleanUp2:;
		
				// Free memory that has been allocated.									
		
		gSeparabilitySpecsPtr->channelSetsPtr = CheckAndDisposePtr (
										gSeparabilitySpecsPtr->channelSetsPtr);	
		
		gSeparabilitySpecsPtr->distancesPtr = CheckAndDisposePtr (
										gSeparabilitySpecsPtr->distancesPtr);
		
		gSeparabilitySpecsPtr->distancesSummaryPtr = CheckAndDisposePtr (
										gSeparabilitySpecsPtr->distancesSummaryPtr);	
		
		if (returnCode < 0)
			break;
												
		}	// end "for (channelCombinationSet=0; ..." 
		
	cleanUp1:;
							
			// Make sure that messages for classes will always be listed.
			
	gListOnlyOneMessagePerClassFlag = FALSE;
	
			// Release memory that has been allocated.									
		
	gSepMeanPtr = CheckAndDisposePtr (gSepMeanPtr);
	gSepCovPtr = CheckAndDisposePtr (gSepCovPtr);
	gSepInvPtr = CheckAndDisposePtr (gSepInvPtr);
	gSepLogDetPtr = CheckAndDisposePtr (gSepLogDetPtr);
	
	ReleaseMatrixInversionMemory ();
	
	gTempChannelStatsPtr = (HChannelStatisticsPtr)CheckAndDisposePtr (
										(Ptr)gTempChannelStatsPtr);
	
			// Release memory for transformation matrix information.					
			
	ReleaseFeatureTransformationMemory ();
	
}	// end "CalculateSeparabilityControl" 


                                                                                
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ChannelCombinationsDialog
//
//	Software purpose:	The purpose of this routine is to display a dialog
//							with a list of the available number of channel
//							combinations that the user can select from.  I.E
//							1, 2, 3, 4, ... n number of channels from the total
//							n number of channels.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			SeparabilityDialog   in SFeatureSelection.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/02/1989
//	Revised By:			Larry L. Biehl			Date: 02/23/2018

Boolean ChannelCombinationsDialog (
				UInt16*								numberOutputChannelCombinationsPtr, 
				UInt16*								channelCombinationsPtr, 
				UInt32								numberInputChannelCombinations, 
				UInt16*								allChanCombinationsPtr,
				UInt32								contiguousChannelsPerGroup,
				Boolean								featureTransformationFlag)

{
	Boolean								returnFlag = FALSE;
	
	#if defined multispec_mac
		Rect									theBox;
		DialogPtr							dialogPtr;
		Handle								theHandle;
		
		SInt16								theType;

		
				// Get the modal dialog for the channel specifications.					
					
		dialogPtr = LoadRequestedDialog (kChannelSpecificationID, kCopyScrap, 1, 2);
		if (dialogPtr == NULL)													
																						return (FALSE);
		
		GetDialogItem (dialogPtr, 1, &theType, &theHandle, &theBox);
		if (numberInputChannelCombinations <= 0)
			HiliteControl ((ControlHandle)theHandle, 255);
			
				// Get a handle to a one column list.											
				
		gDialogListHandle = GetOneColumnDialogListHandle (dialogPtr, 5);
		
				//	Load address of routine that will load the channel combination list.																					
				
		SetDialogItemDrawRoutine (dialogPtr, 5, gCreateOneColumnList1Ptr);
		
		ChannelCombinationsDialogInitialize (dialogPtr,
															contiguousChannelsPerGroup,
															featureTransformationFlag,
															numberInputChannelCombinations);
															
				// Show the dialog.																	
				
		ShowDialogWindow (
							dialogPtr, kChannelSpecificationID, kDoNotSetUpDFilterTable);
		
				// Make certain that the graph port is set to the channel 				
				// combinations dialog window.													
				
		SetPortDialogPort (dialogPtr);
		
				// Continue if there is a valid dialog list handle.				
		
		if (ChannelCombinationsDialogLoadList (dialogPtr,
																gDialogListHandle,
																*numberOutputChannelCombinationsPtr, 
																channelCombinationsPtr, 
																numberInputChannelCombinations, 
																allChanCombinationsPtr,
																gChannelCombinationSelection))		
			{
			LUpdate (GetPortVisibleRegion (
								GetDialogPort (dialogPtr), gTempRegion1), gDialogListHandle);
			SetEmptyRgn (gTempRegion1);
			
					// Handle modal dialog selections for one column channel 			
					// combination list.																
			
			if (gMemoryError == noErr)
				{
				UInt32 numberOutputChannelCombinations = 
												(UInt32)*numberOutputChannelCombinationsPtr;
				returnFlag = 
					ModalOneColListDialog (dialogPtr, 
										(SInt16)numberInputChannelCombinations,
										&numberOutputChannelCombinations, 
										(SInt16*)channelCombinationsPtr, 
										1); 
										
										
				if (returnFlag)
					ChannelCombinationsDialogOK (numberOutputChannelCombinations,                                   
															numberOutputChannelCombinationsPtr,           
															channelCombinationsPtr,         
															allChanCombinationsPtr);
															
				}	// end "if (gMemoryError == noErr)"
							
					// Make certain that global memory error is set back to 'noErr'.	
					
			gMemoryError = noErr;
			
			if (gDialogListHandle != NULL)  
				LDispose (gDialogListHandle);
			gDialogListHandle = NULL;  
			
			}	// end "if (gDialogListHandle != NULL)" 
			
		CloseRequestedDialog (dialogPtr, kDoNotSetUpDFilterTable);
	#endif	// defined multispec_mac

	#if defined multispec_win   
		/*
		CMFeatureSelDialog*		dialogPtr = NULL;
		
		TRY
			{ 
			dialogPtr = new CMFeatureSelDialog ();
			
			returnFlag = dialogPtr->DoDialog (); 
		                       
			delete dialogPtr;
			}
			
		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			returnFlag = FALSE;
			}
		END_CATCH_ALL  
		*/
		returnFlag = TRUE;
	#endif	// defined multispec_win 
	
	return (returnFlag);
	
}	// end "ChannelCombinationsDialog"



void ChannelCombinationsDialogInitialize (
				DialogPtr							dialogPtr,
				SInt16								contiguousChannelsPerGroup,
				Boolean								featureTransformationFlag,
				UInt32								numberInputChannelCombinations)
	
{
	SInt16								index;
	
	Boolean								returnFlag;
	
	
	if (!featureTransformationFlag)
		{
		if (contiguousChannelsPerGroup <= 1)
			index = IDS_FeatureSelection6;
			
		else	// contiguousChannelsPerGroup > 1
			index = IDS_FeatureSelection8;
			
		}	// end "if (!featureTransformationFlag)"
		
	else	// featureTransformationFlag
		{
		if (contiguousChannelsPerGroup <= 1)
			index = IDS_FeatureSelection7;
			
		else	// contiguousChannelsPerGroup > 1
			index = IDS_FeatureSelection9;
			
		}	// end "else featureTransformationFlag"

	returnFlag = LoadSpecifiedStringNumberLongP (kFeatureSelectionStrID, 
																index, 
																(char*)gTextString, 
																(char*)gTextString2,
																TRUE,
																contiguousChannelsPerGroup,
																0);
	
	SetWTitle (GetDialogWindow (dialogPtr), gTextString);
	
			//	Load title for channel combination list.									

	LoadDItemStringNumber (kFeatureSelectionStrID, 
									IDS_FeatureSelection10, 
									dialogPtr, 
									IDC_ListTitle,
									(Str255*)gTextString);
	
			// Initialize the channel combinations range text edit items.								
			
	LoadDItemValue (dialogPtr, IDC_First, (SInt32)1);
	LoadDItemValue (dialogPtr, IDC_Last, (SInt32)numberInputChannelCombinations);
	LoadDItemValue (dialogPtr, IDC_Interval, (SInt32)1);
	SetDLogControlHilite (dialogPtr, IDC_EnterNewRange, 0);
	
}	// end "ChannelCombinationsDialogInitialize"   



void ChannelCombinationsDialogOK (
				UInt16								numberOutputChannelCombinations,                                   
				UInt16*								numberOutputChannelCombinationsPtr,           
				UInt16*								channelCombinationsPtr,         
				UInt16*								allChanCombinationsPtr)
	
{  
	UInt16								cellNumber,
											index;
	
									
	*numberOutputChannelCombinationsPtr = numberOutputChannelCombinations; 
		
			// Make certain that the list of numbers of channels to use			
			// reflects the numbers of channels that are available.  The list	
			// that we get out of ModalOneColListDialog reflects the item		
			// number in the list that was selected.  That may be different 	
			// than the number of channels to use that was associated with it.
					
	for (cellNumber=0; 
			cellNumber<numberOutputChannelCombinations; 
			cellNumber++)
		{
		index = channelCombinationsPtr[cellNumber] - 1;
		channelCombinationsPtr[cellNumber] = allChanCombinationsPtr[index];
				
		}	// end "for (cellNumber=0; ..."  
	
}	// end "ChannelCombinationsDialogOK"



Boolean ChannelCombinationsDialogLoadList (
				DialogPtr							dialogPtr,
				#if defined multispec_wx
					wxListBox*							dialogListHandle,
				#else
					ListHandle							dialogListHandle,
				#endif
				UInt16								numberOutputChannelCombinations, 
				UInt16* 								channelCombinationsPtr, 
				UInt32								numberInputChannelCombinations, 
				UInt16*								allChanCombinationsPtr,
				SInt16								channelCombinationSelection)
				
{
	Cell									cell;
	
	UInt32								cellNumber,
											index;
	
	SInt16								row; 
											
	UInt16								channelCombination;
	
	Boolean								returnFlag = FALSE;
	
		
			// Continue if there is a valid dialog list handle.						
			
	if (dialogListHandle != NULL)
		{
				// Load the channel combinations into the list.							
				
		LSetDrawingMode (FALSE, dialogListHandle);
		
		row = LAddRow ((SInt16)numberInputChannelCombinations, 0, dialogListHandle);
				
				// Select the cell if the channel combination is in the channel 	
				// combination list.																
		
		cell.h = 0;
		index = 0;
		for (cellNumber=0; 
				cellNumber<numberInputChannelCombinations; 
				cellNumber++)
			{
					// Add number channels to the channel combination list.			
						
			cell.v = (SInt16)cellNumber;
			
			channelCombination = allChanCombinationsPtr[cellNumber];
			NumToString ((UInt32)channelCombination, gTextString);
					
					// Load the string into the cell.										
					
			LSetCell ((char*)&gTextString[1], 
								(SInt16)gTextString[0], 
								cell, 
								dialogListHandle);
			
					// Make certain that we didn't have a memory with the last		
					// setting of the list for the cell.									
					
			if (gMemoryError != noErr)
				break;
			
			if (channelCombinationSelection == kAllMenuItem ||
						(index < numberOutputChannelCombinations && 
								channelCombinationsPtr[index] == channelCombination))
				{
				LSetSelect (TRUE, cell, dialogListHandle);
				index++;
				
				}	// end "if (channelCombinationSelection == kAllMenuItem || ..." 
				
			}	// end "for (cellNumber=0; cellNumber...)" 
				
				// Set the number of selected items.										
				
		LoadDItemValue (dialogPtr, IDC_SelectionCount, (SInt32)index);		// 16
			
				// Turn list drawing mode back on.											
	  				
		LSetDrawingMode (TRUE, dialogListHandle);
		
		returnFlag = TRUE;
		
		}	// end "if (dialogListHandle != NULL)"
		
	return (returnFlag);
	
}	// end "ChannelCombinationsDialogLoadList" 


                                                                                
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16  ComputeSeparabilityForAllChannelCombinations
//
//	Software purpose:	The purpose of this routine is to calculate
//							the separability for all channel combinations and
//							all class combinations, sort the results if requested
//							and list the results.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			CalculateSeparabilityControl in SHistogram.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/06/1989
//	Revised By:			Larry L. Biehl			Date: 05/17/1990	

SInt16 ComputeSeparabilityForAllChannelCombinations (
				UInt32								numberChannelGroupCombinations, 
				UInt32								numberClassCombinations, 
				SInt16								numberSeparabilityChannels)

{
	SepDistancesSummaryPtr			distancesSummaryPtr;
	
	SInt16								*channelSetsPtr,
											*currentChannelSetsPtr,
											*featurePtr;
	
	SInt32								freeIndex;
										
	UInt32								channelCombination,
											channelSetsIndex,
											combinationIndex,
											combinationSlots;
										
	Boolean								firstTimeFlag,
											stopFlag;
													
	SInt16								i,
											numberFeatures,
											numberSeparabilityGroups,
											returnCode,
											quitCode;
	
	
			// Check input parameters.															
			// The check for numberClassCombinations being less than 0 because 	
			// as of 11/6/1989, LSC 4 does not handle unsigned long int correctly 	
			// in the 881 Math library.														
	
	if (numberClassCombinations <= 0)										return (-1);
	if (numberChannelGroupCombinations <= 0)								return (-1);
	if (numberSeparabilityChannels <= 0)									return (-1);
	
			// Initialize local variables.													

	channelSetsIndex = 0;
	distancesSummaryPtr = 
				(SepDistancesSummaryPtr)gSeparabilitySpecsPtr->distancesSummaryPtr;
	numberFeatures = gSeparabilitySpecsPtr->numberFeatures;
	combinationSlots = gSeparabilitySpecsPtr->combinationSlots;
	featurePtr = (SInt16*)GetHandlePointer (gSeparabilitySpecsPtr->featureHandle);
	numberSeparabilityGroups =  numberSeparabilityChannels/
										gSeparabilitySpecsPtr->numberContiguousPerGroup;
	
	quitCode = 1;
	firstTimeFlag = TRUE;
	
	freeIndex = -1;
	
			// Initialize the mean and minimum start pointers.							
			
	gSeparabilitySpecsPtr->numberFeatureCombinationsLoaded = 0;
	gSeparabilitySpecsPtr->meanStart = 0;
	gSeparabilitySpecsPtr->minimumStart = 0;
	
			// Initialize the vector for channel combinations. First channel 		
			// combination is first 'numberSeparabilityChannels' channels in the	
			// input channel feature set.  Use 'gTextString3' for storage.			
	
	currentChannelSetsPtr = (SInt16*)MNewPointer (
						(SInt32)numberSeparabilityChannels * sizeof (SInt16));
	if (currentChannelSetsPtr == NULL)
																						return (-1);
																							
	for (i=0; i<numberSeparabilityChannels; i++)
		currentChannelSetsPtr[i] = i;
	
			// Load information into the status dialog.
							
	LoadDItemStringNumber (kFeatureSelectionStrID, 
									IDS_FeatureSelection27,	// "\pComputing Separability."
									gStatusDialogPtr, 
									IDC_Status11, 
									(Str255*)gTextString);
							
	LoadDItemRealValue (gStatusDialogPtr, 
								IDC_Status10,
								(double)numberChannelGroupCombinations,
								0);
					
			// Intialize the nextTime variable to indicate when the next check	
			// should occur for a command-.													
			
	gNextTime = TickCount ();
	gNextStatusTime = TickCount ();
	stopFlag = FALSE;
	
	for (channelCombination=0;
			channelCombination<numberChannelGroupCombinations;
			channelCombination++)
		{																		
				// Load the statistics for the given channel combination.			
		
		if (TickCount () >= gNextStatusTime)
			{
			LoadDItemRealValue (gStatusDialogPtr, 
										IDC_Status8, 
										(double)channelCombination+1,
										0);
									
			gNextStatusTime = TickCount () + gNextStatusTimeOffset;
			
			}	// end "if (TickCount () >= gNextStatusTime)"
	
				// Get the next free index in 'distancesSummaryPtr'.					
		
		if (channelCombination < combinationSlots)
			combinationIndex = channelCombination;
			
		else	// channelCombination >= combinationSlots 
			{
			combinationIndex = freeIndex;
			freeIndex = distancesSummaryPtr[combinationIndex].nextMean;
			
			}	// else channelCombination >= combinationSlots 
			
				// Break out of loop if combinationIndex is out of range.			
				
		if (combinationIndex >= combinationSlots)
			break;
				
				// Convert channel combination numbers to channel numbers			
				// that point to the respective statistics for that channel.		
				// This needs to be done in case the user is only using a 			
				// subset of the channels that are available in the statistics.			
		
		channelSetsIndex = distancesSummaryPtr[combinationIndex].channelSetIndex;
		channelSetsPtr = &gSeparabilitySpecsPtr->channelSetsPtr[channelSetsIndex];
		
		for (i=0; i<numberSeparabilityChannels; i++)
			channelSetsPtr[i] = featurePtr[currentChannelSetsPtr[i]];
								
		returnCode = LoadSeparabilityStatistics (
					channelSetsPtr, 
					numberSeparabilityChannels);
		
				// Compute the separability measures.										
		
		if (returnCode > 0)
			returnCode = ComputeSeparabilityForAllClassCombinations (
						&distancesSummaryPtr[combinationIndex],
						numberClassCombinations, 
						gSeparabilitySpecsPtr->numberClasses,
						numberSeparabilityChannels);
						
		else	// returnCode < 0 
			distancesSummaryPtr[combinationIndex].classPairDistancesIndex = -1;
				
		if (returnCode > 0)
			{
					// Update the list pointers.												
					
			returnCode = UpdateSeparabilityLists (distancesSummaryPtr, 
																	combinationIndex, 
																	channelCombination,
																	&freeIndex,
																	firstTimeFlag);
							
			firstTimeFlag = FALSE;
			
			}	// end "if (returnCode > 0)" 
		
				// Exit loop if user has "command period" down or return code 		
				// indicates that a problem exists.											
			
		if (TickCount () >= gNextTime)
			{
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
				returnCode = -1;
					
			}	// end "if (TickCount () >= nextTime)" 
			
		if (gAlertReturnCode == 1 || returnCode <= 0)
			{
			channelCombination = numberChannelGroupCombinations;
			quitCode = -1;
			
			}	// end "if (stopFlag || returnCode <= 0)" 
			
				// Get the next channel combination.										
				
		if (!GetNextCombination (currentChannelSetsPtr, 
									numberSeparabilityGroups, 
									numberFeatures,
									gSeparabilitySpecsPtr->numberContiguousPerGroup))
			break;
	
		}	// end "for (channelCombination=0; ..." 
		
			// Finish updating the list pointers.											
			
	distancesSummaryPtr[gSeparabilitySpecsPtr->meanEnd].nextMean = -1;
	distancesSummaryPtr[gSeparabilitySpecsPtr->minimumEnd].nextMinimum = -1;
	
	if (returnCode > 0 && (gSeparabilitySpecsPtr->tablesToList & 0x0001))
		{
				// List the results.
							
		LoadDItemStringNumber (kFeatureSelectionStrID, 
						IDS_FeatureSelection28,	// "\pListing Separability Results."
						gStatusDialogPtr, 
						IDC_Status11, 
						(Str255*)gTextString);
				
		returnCode = ListSeparabilityMeasurements (
					numberChannelGroupCombinations, numberClassCombinations, 
																	numberSeparabilityChannels);
			
		}	// end "if (returnCode > 0)" 
		
			// Dispose of memory for 'currentChannelSetsPtr'							
			
	CheckAndDisposePtr ((Ptr)currentChannelSetsPtr);
	
			// If user used 'command .' to quit computing separability measures,	
			// then reflect that case in the return code.  QuitCode was used to	
			// allow the results that had been obtained up to that point to be	
			// listed if requested.																
				
	if (quitCode < 0 || gAlertReturnCode == 1)
		returnCode = -1;
			
	return (returnCode);

}	// end "ComputeSeparabilityForAllChannelCombinations" 


                                                                                 
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ComputeSeparabilityForStepChannelCombinations
//
//	Software purpose:	The purpose of this routine is to calculate
//							the separability for all channel combinations that
//							are included in a step procedure search and
//							all class combinations, sort the results if requested
//							and list the results.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 12/08/1992
//	Revised By:			Larry L. Biehl			Date: 12/10/1992	

SInt16 ComputeSeparabilityForStepChannelCombinations (
				UInt32								numberChannelGroupCombinations, 
				UInt32								numberClassCombinations, 
				SInt16								numberSeparabilityChannels)

{
	SepDistancesSummaryPtr			distancesSummaryPtr;
	
	SInt16								*channelSetsPtr,
											*channelsUsedPtr,
											*currentChannelSetsPtr,
											*featurePtr;
	
	SInt32								channelCombination,
											combinationCount,
											combinationIndex,
											combinationSlots,
											freeIndex,
											groupCombination;
	
	UInt32								channelSetsIndex;
	                    	
	Boolean								continueFlag,
											firstTimeFlag,
											stopFlag;
	
	SInt16								i,
											j,
											nextGroupChannel,
											numberFeatures,
											numberSeparabilityGroups,
											returnCode,
											quitCode;
	
	
			// Check input parameters.															
			// The check for numberClassCombinations being less than 0 because 	
			// as of 11/6/1989, LSC 4 does not handle unsigned long int correctly 	
			// in the 881 Math library.														
	
	if (numberClassCombinations <= 0)										return (-1);
	if (numberChannelGroupCombinations <= 0)							return (-1);
	if (numberSeparabilityChannels <= 0)									return (-1);
	
			// Initialize local variables.													

	distancesSummaryPtr = 
				(SepDistancesSummaryPtr)gSeparabilitySpecsPtr->distancesSummaryPtr;
	numberFeatures = gSeparabilitySpecsPtr->numberFeatures;
	combinationSlots = gSeparabilitySpecsPtr->combinationSlots;
	featurePtr = (SInt16*)GetHandlePointer (
										gSeparabilitySpecsPtr->featureHandle);
	numberSeparabilityGroups =  numberSeparabilityChannels/
										gSeparabilitySpecsPtr->numberContiguousPerGroup;
	
	quitCode = 1;
	returnCode = 1;
	
			// Initialize the vector for channel combinations. First channel 		
			// combination is first 'numberSeparabilityChannels' channels in the	
			// input channel feature set.  Use 'gTextString3' for storage.			
	
	currentChannelSetsPtr = (SInt16*)MNewPointer (
					(SInt32)(numberSeparabilityChannels + numberFeatures) * 
																			sizeof (SInt16));
	if (currentChannelSetsPtr == NULL)
																						return (-1);
																						
	channelsUsedPtr = &currentChannelSetsPtr[numberSeparabilityChannels];
																							
	for (i=0; i<numberSeparabilityChannels; i++)
		currentChannelSetsPtr[i] = i;	
								
	for (i=0; i<numberFeatures; i++)
		channelsUsedPtr[i] = 0;
	
			// Load information into the status dialog.
							
	LoadDItemStringNumber (kFeatureSelectionStrID, 
					IDS_FeatureSelection27,	// "\pComputing Separability."
					gStatusDialogPtr, 
					IDC_Status11, 
					(Str255*)gTextString);
							
	LoadDItemValue (gStatusDialogPtr, 
							IDC_Status10, 
							(SInt32)numberChannelGroupCombinations);
					
			// Intialize the nextTime variable to indicate when the next check	
			// should occur for a command-.													
			
	gNextTime = TickCount ();
	gNextStatusTime = TickCount ();
	stopFlag = FALSE;
	channelCombination = 1;
	numberFeatures = gSeparabilitySpecsPtr->numberContiguousPerGroup;
			
	nextGroupChannel = 1;
	
	for (groupCombination=1;
			groupCombination<=numberSeparabilityGroups;
			groupCombination++)
		{		
		freeIndex = -1;
		combinationCount = 0;
	
				// Initialize the mean and minimum start pointers.						
			
		gSeparabilitySpecsPtr->numberFeatureCombinationsLoaded = 0;
		gSeparabilitySpecsPtr->meanStart = 0;
		gSeparabilitySpecsPtr->minimumStart = 0;
		
		continueFlag = TRUE;
		firstTimeFlag = TRUE;
																
		do
			{							
					// Load the statistics for the given channel combination.		
			
			if (TickCount () >= gNextStatusTime)
				{
				LoadDItemValue (gStatusDialogPtr, 
										IDC_Status8, 
										(SInt32)channelCombination);
				gNextStatusTime = TickCount () + gNextStatusTimeOffset;
				
				}	// end "if (TickCount () >= gNextStatusTime)" 
		
					// Get the next free index in 'distancesSummaryPtr'.				
			
			if (combinationCount < combinationSlots)
				combinationIndex = combinationCount;
				
			else	// combinationCount >= combinationSlots 
				{
				combinationIndex = freeIndex;
				freeIndex = distancesSummaryPtr[combinationIndex].nextMean;
				
				}	// else combinationCount >= combinationSlots 
				
					// Break out of loop if combinationIndex is out of range.		
					
			if (combinationIndex >= combinationSlots)
				break;
					
					// Convert channel combination numbers to channel numbers		
					// that point to the respective statistics for that channel.	
					// This needs to be done in case the user is only using a 		
					// subset of the channel that are available in the statistics.			
			
			channelSetsIndex = distancesSummaryPtr[combinationIndex].channelSetIndex;
			channelSetsPtr = &gSeparabilitySpecsPtr->channelSetsPtr[channelSetsIndex];
			
			for (i=0; i<numberFeatures; i++)
				channelSetsPtr[i] = featurePtr[currentChannelSetsPtr[i]];
						
			if (returnCode > 0)			
				returnCode = LoadSeparabilityStatistics (
						channelSetsPtr, 
						numberFeatures);
			
					// Compute the separability measures.									
			
			if (returnCode > 0)
				returnCode = ComputeSeparabilityForAllClassCombinations (
							&distancesSummaryPtr[combinationIndex],
							numberClassCombinations, 
							gSeparabilitySpecsPtr->numberClasses,
							numberFeatures);
							
			else	// returnCode < 0 
				distancesSummaryPtr[combinationIndex].classPairDistancesIndex = -1;
				
			if (returnCode > 0)
				{
						// Update the list pointers.											
						
				returnCode = UpdateSeparabilityLists (distancesSummaryPtr, 
													combinationIndex, 
													combinationCount,
													&freeIndex,
													firstTimeFlag);
								
				firstTimeFlag = FALSE;
				
				}	// end "if (returnCode > 0)" 
			
					// Exit loop if user has "command period" down or return code 	
					// indicates that a problem exists.										
				
			if (TickCount () >= gNextTime)
				{
				if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
					returnCode = -1;
						
				}	// end "if (TickCount () >= nextTime)" 
				
			if (gAlertReturnCode == 1 || returnCode <= 0)
				{
				channelCombination = numberChannelGroupCombinations;
				quitCode = -1;
				continueFlag = FALSE;
				
				}	// end "if (stopFlag || returnCode <= 0)" 
				
					// Get the next channel combination.									
			
			if (continueFlag)
				continueFlag = GetNextStepCombination (currentChannelSetsPtr,
										channelsUsedPtr, 
										&nextGroupChannel, 
										gSeparabilitySpecsPtr->numberFeatures,
										gSeparabilitySpecsPtr->numberContiguousPerGroup);
			
			channelCombination++;
			combinationCount++;
				
			}	while (continueFlag);
			
		if (groupCombination != numberSeparabilityGroups)
			{
					// Update the channels used list.										
					
			if (gSeparabilitySpecsPtr->sortChannelCombinations == 1)
				combinationIndex = gSeparabilitySpecsPtr->meanStart;
			else if (gSeparabilitySpecsPtr->sortChannelCombinations == 2)
				combinationIndex = gSeparabilitySpecsPtr->minimumStart;
				
			channelSetsIndex = distancesSummaryPtr[combinationIndex].channelSetIndex;
			channelSetsPtr = &gSeparabilitySpecsPtr->channelSetsPtr[channelSetsIndex];
					
					// Update the channels used vector.										
			
			j = 0;				
			for (i=0; i<numberFeatures; i++)
				{
				while (channelSetsPtr[i] != featurePtr[j])
					j++;
				
				if (j < gSeparabilitySpecsPtr->numberFeatures)
					{
					channelsUsedPtr[j] = 1;
					j++;
					
					}	// end "if (j < gSeparabilitySpecsPtr->numberFeatures)" 
				
				}	// end "for (i=0; i<numberFeatures; i++)" 
			
			nextGroupChannel = 0;
			
			if (!GetNextStepCombination (currentChannelSetsPtr,
										channelsUsedPtr, 
										&nextGroupChannel, 
										gSeparabilitySpecsPtr->numberFeatures,
										gSeparabilitySpecsPtr->numberContiguousPerGroup))
				{
				returnCode = 2;
				groupCombination = numberSeparabilityGroups;
				
				}	// end "if (!continueFlag)" 
				
			else	// GetNextStepCombination (...) 
				numberFeatures += gSeparabilitySpecsPtr->numberContiguousPerGroup;
			
			}	// end "if (groupCombination != numberSeparabilityGroups)" 
	
		}	// end "for (groupCombination=1; ..." 
		
			// Finish updating the list pointers.											
			
	distancesSummaryPtr[gSeparabilitySpecsPtr->meanEnd].nextMean = -1;
	distancesSummaryPtr[gSeparabilitySpecsPtr->minimumEnd].nextMinimum = -1;
	
	if (returnCode > 0 && (gSeparabilitySpecsPtr->tablesToList & 0x0001))
		{
				// List the results.			
							
		LoadDItemStringNumber (kFeatureSelectionStrID, 
					IDS_FeatureSelection28,	// "\pListing Separability Results."
					gStatusDialogPtr, 
					IDC_Status11, 
					(Str255*)gTextString);
												
		if (returnCode == 2)
			numberSeparabilityChannels = numberFeatures;
				
		returnCode = ListSeparabilityMeasurements (
					numberChannelGroupCombinations, numberClassCombinations, 
																	numberSeparabilityChannels);
			
		}	// end "if (returnCode > 0)" 
		
			// Dispose of memory for 'currentChannelSetsPtr'							
			
	CheckAndDisposePtr ((Ptr)currentChannelSetsPtr);
	
			// If user used 'command .' to quit computing separability measures,	
			// then reflect that case in the return code.  QuitCode was used to	
			// allow the results that had been obtained up to that point to be	
			// listed if requested.																
				
	if (quitCode < 0)
		returnCode = -1;
			
	return (returnCode);

}	// end "ComputeSeparabilityForStepChannelCombinations" 


                                                                                
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ComputeSeparabilityForAllClassCombinations
//
//	Software purpose:	The purpose of this routine is to calculate
//							the separability for all class combinations for
//							the given channel set.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			ComputeSeparabilityForAllChannelCombinations in SFeatureSelection.cpp
//							ComputeSeparabilityForStepChannelCombinations in SFEatSel.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/06/1989
//	Revised By:			Larry L. Biehl			Date: 03/06/1998	

SInt16 ComputeSeparabilityForAllClassCombinations (
				SepDistancesSummaryPtr			distancesSummaryPtr, 
				UInt32								numberClassCombinations, 
				UInt32								numberClasses, 
				SInt16								numberSeparabilityChannels)

{
	double								classPairMinimum,
											classPairMean,
											distance;
											
	HDoublePtr							meanDifPtr;
	HFloatPtr							distancesPtr;
	HSInt16Ptr							weightsPtr;
	
	UInt32								classSet,
											cov1Index,
											cov2Index,
											mean1Index,
											mean2Index,
											numberCovEntries;
											
	SInt32								classPairDistancesIndex;
	
	SInt16								classPtr[2],
											previousClass1;
	
	
			// Check input parameters.															
			// The check for numberClassCombinations being less than 0 because 	
			// as of 11/6/1989, LSC 4 does not handle unsigned long int correctly 	
			// in the 881 Math library.														
	
	if (numberClassCombinations <= 0)
																					return (-1);
	
			// Initialize local variables.													
										
	numberCovEntries = (UInt32)numberSeparabilityChannels;
	if ((gSeparabilitySpecsPtr->distanceMeasure != kNonCovarianceBhattacharyya))
		{
		numberCovEntries *= (numberSeparabilityChannels+1);
		numberCovEntries /= 2;
		
		}	// end "if (covarinaceFlag)" 
						
	classPairMinimum = DBL_MAX;
	classPairMean = 0;
	
	classPairDistancesIndex = distancesSummaryPtr->classPairDistancesIndex;
	distancesPtr = &gSeparabilitySpecsPtr->distancesPtr[classPairDistancesIndex];
	weightsPtr = (SInt16*)GetHandlePointer (
										gProjectInfoPtr->classPairWeightsHandle);
	
	meanDifPtr = gInverseMatrixMemory.pivotPtr;

	classPtr[0] = 0;
	classPtr[1] = 1;
	previousClass1 = 0;
	mean1Index = 0;
	mean2Index = numberSeparabilityChannels;
	cov1Index = 0;
	cov2Index = numberCovEntries;
	
	for (classSet=0; classSet<numberClassCombinations; classSet++)
		{
		if (weightsPtr[classSet] > 0)
			{
			switch (gSeparabilitySpecsPtr->distanceMeasure)
				{
				case kBhattacharyya:
					distance = Bhattacharyya (
										&gSepMeanPtr[mean1Index], 
										&gSepMeanPtr[mean2Index], 
										&gSepCovPtr[cov1Index], 
										&gSepCovPtr[cov2Index],
										gSepLogDetPtr[*classPtr], 
										gSepLogDetPtr[classPtr[1]],
										meanDifPtr, 
										numberSeparabilityChannels);
					break;
					
				case kEFBhattacharyya:
					distance = EFBhattacharyya (
										&gSepMeanPtr[mean1Index], 
										&gSepMeanPtr[mean2Index], 
										&gSepCovPtr[cov1Index], 
										&gSepCovPtr[cov2Index],
										gSepLogDetPtr[*classPtr], 
										gSepLogDetPtr[classPtr[1]],
										meanDifPtr, 
										numberSeparabilityChannels);
					break;
					
				case kMeanBhattacharyya:
					distance = MeanBhattacharyya (
										&gSepMeanPtr[mean1Index], 
										&gSepMeanPtr[mean2Index], 
										&gSepCovPtr[cov1Index], 
										&gSepCovPtr[cov2Index],
										meanDifPtr, 
										numberSeparabilityChannels);
					break;
					
				case kCovarianceBhattacharyya:
					distance = CovarianceBhattacharyya (
										&gSepCovPtr[cov1Index], 
										&gSepCovPtr[cov2Index],
										gSepLogDetPtr[*classPtr], 
										gSepLogDetPtr[classPtr[1]],
										numberSeparabilityChannels);
					break;
						
				case kNonCovarianceBhattacharyya:
					distance = NonCovarianceBhattacharyya (
										&gSepMeanPtr[mean1Index], 
										&gSepMeanPtr[mean2Index], 
										&gSepCovPtr[cov1Index], 
										&gSepCovPtr[cov2Index],
										meanDifPtr, 
										(UInt32)numberSeparabilityChannels);
					break;
					
				case kTransformedDivergence:
					distance = TransformedDivergence (
										&gSepMeanPtr[mean1Index], 
										&gSepMeanPtr[mean2Index], 
										&gSepCovPtr[cov1Index], 
										&gSepCovPtr[cov2Index],
										&gSepInvPtr[cov1Index], 
										&gSepInvPtr[cov2Index],
										meanDifPtr, 
										numberSeparabilityChannels);
					break;
						
				case kDivergence:
					distance = Divergence (
										&gSepMeanPtr[mean1Index], 
										&gSepMeanPtr[mean2Index], 
										&gSepCovPtr[cov1Index], 
										&gSepCovPtr[cov2Index],
										&gSepInvPtr[cov1Index], 
										&gSepInvPtr[cov2Index],
										meanDifPtr, 
										numberSeparabilityChannels);
					break;
					
				}	// end "switch (gSeparabilitySpecsPtr->distanceMeasure)" 
									
			if (gOperationCanceledFlag)
																						return (-1);
											
			classPairMean += distance * weightsPtr[classSet];
			classPairMinimum = MIN (distance, classPairMinimum);
			
			if (gSeparabilitySpecsPtr->listClassPairDistances)
				distancesPtr[classSet] = (float)distance;
				
			}	// end "if (weightsPtr[classSet] > 0)" 
		
		previousClass1 = classPtr[0];
		
				// Get the next class combination.												
			
		if (!GetNextCombination (classPtr, 2, numberClasses, 1))
			break;
			
		if (classPtr[0] != previousClass1)
			{
			mean1Index += numberSeparabilityChannels;
			cov1Index += numberCovEntries;
			
			}	// end "if (class1 != previousClass1)" 
			
		mean2Index = numberSeparabilityChannels * (classPtr[1]);
		cov2Index = numberCovEntries * (classPtr[1]);
		
		}	// end "for (classSet=0; ..." 
		
	distancesSummaryPtr->classPairMean = 
									classPairMean/gProjectInfoPtr->classPairWeightSum;
	distancesSummaryPtr->classPairMinimum = classPairMinimum;
		
	return (1);

}	// end "ComputeSeparabilityForAllClassCombinations" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double CovarianceBhattacharyya
//
//	Software purpose:	The purpose of this routine is to calculate
//							the Bhattacharyya distance between the two classes 
//							represented by their input covariance matrices and 
//							inverse covariance matrices. The mean vectors are not used.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:	
//
//	Coded By:			C.H. Lee					Date: 03/05/1998
//	Revised By:			Larry L. Biehl			Date: 06/17/2006	

double CovarianceBhattacharyya (
				HDoublePtr							cov1Ptr, 			// covariance matrix of class 1 
				HDoublePtr							cov2Ptr,				// covariance matrix of class 2 
				double								logDeterminant1,  // log of determinant of 
																				//	class 1 covariance matrix	
				double								logDeterminant2,	// log of determinant of 
																				//	class 2 covariance matrix		
				SInt16								numberFeatures)	// number of features (channels) 
																				// represented in the mean vector  	
																				// and covariance and inverse  	
																				// covariance matrices.																						
											
{
	double								bhattacharyya,
//											determinant,
											logDeterminant;
	
	HDoublePtr							temp1Ptr,
											temp2Ptr,
											tInversePtr;
											
	UInt32								channel1,
											channel2,
											tmpIndex1,
											tmpIndex2;
	
			
			// Get 1/2 of the sum of the two covariances.
			// The 1/2 will be taken care of by subtracting the
			// constant 'numberFeature * ln (2)'.						
	
	tInversePtr = gInverseMatrixMemory.inversePtr;
	temp1Ptr = cov1Ptr;
	temp2Ptr = cov2Ptr;
	
	for (channel1=0; channel1<(UInt32)numberFeatures; channel1++)
		{
		tmpIndex1 = (UInt32)channel1*numberFeatures;
		tmpIndex2 = channel1;
		
		for (channel2=0; channel2<channel1; channel2++)
			{	
			tInversePtr[tmpIndex1] = tInversePtr[tmpIndex2] = 
																(*temp1Ptr + *temp2Ptr);
			
			temp1Ptr++;
			temp2Ptr++;
		
			tmpIndex1++;
			tmpIndex2 += numberFeatures;
			
			}	// end "for (channel2=1; channel2<channel1; channel2++)" 
			
		tInversePtr[tmpIndex1] = (*temp1Ptr + *temp2Ptr);
			
		temp1Ptr++;
		temp2Ptr++;
			
		}	// end "for (channel1=1; channel1<=numOutFeatures; channel1++)" 
		
	InvertSymmetricMatrix (tInversePtr, 
									numberFeatures, 
									gInverseMatrixMemory.pivotPtr, 
									gInverseMatrixMemory.indexRowPtr, 
									gInverseMatrixMemory.indexColPtr, 
									gInverseMatrixMemory.ipvotPtr,
									NULL,
									&logDeterminant,
									kReturnMatrixInverse);
									
	if (gOperationCanceledFlag)
																						return (0);

	bhattacharyya = .5 * (logDeterminant - 
						numberFeatures*kLN2 - .5*(logDeterminant1 + logDeterminant2));
		
	return (bhattacharyya);
	
}	// end "CovarianceBhattacharyya" 


                                                                                
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double Divergence
//
//	Software purpose:	The purpose of this routine is to calculate
//							the divergence distance between the two classes 
//							represented by their input covariance matrices, 
//							inverse covariance matrices and mean vectors.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			C.H. Lee					Date: 12/04/1988
//	Revised By:			Larry L. Biehl			Date: 10/28/1991	

double Divergence (
				HDoublePtr							mean1Ptr, 	// class 1 mean vector 				
				HDoublePtr							mean2Ptr, // class 2 mean vector 				
				HDoublePtr							cov1Ptr, // covariance matrix of class 1 
				HDoublePtr							cov2Ptr, // covariance matrix of class 2 
				HDoublePtr							inv1Ptr, // inverse of class 1 covariance 
				HDoublePtr							inv2Ptr,  // inverse of class 2 covariance 
				HDoublePtr							meanDifPtr, // vector storage of class 1 and 		
																// class 2 mean difference. 				
				SInt16								numberFeatures)	// number of features (channels) 
																// represented in the mean vector and 	
																// covariance and inverse covariance 	
																//	matrices.									

{
	double								divergence;
	
	HDoublePtr							tInv1Ptr,
											tInv2Ptr,
											tCov1Ptr,
											tCov2Ptr;
	
	SInt16			 					i,
						 					j;
	
	
			// Initialize local variables.													
			
	divergence = 0;
	tInv1Ptr = inv1Ptr;
	tInv2Ptr = inv2Ptr;
	tCov1Ptr = cov1Ptr;
	tCov2Ptr = cov2Ptr;
	
			// Get the mean difference vector.												
			
	for (i=0; i<numberFeatures; i++)
		meanDifPtr[i] = mean1Ptr[i] - mean2Ptr[i];
	
			// Get i=j parts of the components.												
		
	for (i=0; i<numberFeatures; i++)
		{
		divergence += 	(*tCov1Ptr - *tCov2Ptr) * 
								(*tInv2Ptr - *tInv1Ptr)  +
							(*tInv1Ptr + *tInv2Ptr) *
										meanDifPtr[i] * meanDifPtr[i];
										
		tCov1Ptr += i + 2;
		tCov2Ptr += i + 2;
		tInv1Ptr += i + 2;
		tInv2Ptr += i + 2;
		
		}	// end "for (i=0; i<numberFeatures; i++)" 
		
	divergence /= 2;
	
			// Get i!=j parts of the components.											
	
	tInv1Ptr = inv1Ptr + 1;
	tInv2Ptr = inv2Ptr + 1;
	tCov1Ptr = cov1Ptr + 1;
	tCov2Ptr = cov2Ptr + 1;
	for (i=1; i<numberFeatures; i++)
		{
		for (j=0; j<i; j++)
			{
			divergence += (*tCov1Ptr - *tCov2Ptr) * 
									(*tInv2Ptr - *tInv1Ptr) +
							  (*tInv1Ptr + *tInv2Ptr) *
										meanDifPtr[i] * meanDifPtr[j];
								
			tInv1Ptr++;
			tInv2Ptr++;
			tCov1Ptr++;
			tCov2Ptr++;
	         
	      }	// end "for (j=0; j<i; j++)" 
								
		tInv1Ptr++;
		tInv2Ptr++;
		tCov1Ptr++;
		tCov2Ptr++;
	      
	   }	// end "for (i=1; i<numberFeatures; i++)" 
	
	return (divergence);
	
}	// end "Divergence" 



#if defined multispec_mac                                                              
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawChannelCombinationsPopUp
//
//	Software purpose:	The purpose of this routine is to draw the 
//							channel combinations prompt and channel combinations
//							set selection for channel combinations
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
//	Coded By:			Larry L. Biehl			Date: 10/16/1989
//	Revised By:			Larry L. Biehl			Date: 10/30/1990	

pascal void DrawChannelCombinationsPopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)
                             			
{
			// Use the generic pop up drawing routine.									
			
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpAllPossibleSubsetMenu, 
								gChannelCombinationSelection, 
								TRUE);
	
}	// end "DrawChannelCombinationsPopUp" 
#endif	// defined multispec_mac


                                                                                
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt32 GetCombinationLimit
//
//	Software purpose:	The purpose of this routine is to determine the
//							limit on "m" groups of "x" contiguous items out of 
//							a total of "n" items such
//							that the number of combinations will be less than
//							LONG_MAX (2147483647).
//
//							Two search algorithms are allowed - a search through
//							all combinations (!stepSearchFlag) or a stepwise 
//							search to first find best 1 group, then using the
//							best 1 group find the best 2nd group, etc.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			LoadAllChannelCombinationsVector in SFeatureSelection.cpp
//
//	Coded By:			C.H. Lee					Date: 11/10/1989
//	Revised By:			Larry L. Biehl			Date: 04/24/1998	

SInt32 GetCombinationLimit (
				SInt32								total, 
				SInt32								numberContiguousPerGroup, 
				Boolean								stepSearchFlag)

{
			// Declare local variables and structures									

	SInt32								limit,
											numberCombinations,
											numberLoops;
											
	SInt16								numberCombinationReturnCode;
	 
	
	if (!stepSearchFlag)
		{
				// Initialize local variables.												
				
		numberLoops = total/(1+numberContiguousPerGroup);
		numberLoops = MAX (1,numberLoops);
			
				// Loop for numberLoops times.  The number of combinations is 		
				// symmetric about 'numberLoops' times.									
					
		for (limit=1; limit <= numberLoops; limit++)
			{
			numberCombinations = GetNumberOfCombinations (total, 
																			limit, 
																			numberContiguousPerGroup, 
																			FALSE,
																			&numberCombinationReturnCode);
																			
			if (numberCombinationReturnCode == -2)
																			return (--limit);
																				
			if (numberCombinations == 0)
																			return (-1);
																				
			}	// end "for (limit=1; limit <= numberLoops; limit++)" 
			
		}	// end "if (!stepSearchFlag)" 
		
	else	// stepSearchFlag 
		{
				// Initialize local variables.												
				
		numberLoops = total/numberContiguousPerGroup;
		numberLoops = MAX (1,numberLoops);
			
				// Loop for numberLoops times.												
					
		for (limit=1; limit <= numberLoops; limit++)
			{
			numberCombinations = GetNumberOfCombinations (
														total, 
														limit, 
														numberContiguousPerGroup, 
														stepSearchFlag,
														&numberCombinationReturnCode);
														
			if (numberCombinationReturnCode == -2)
																					return (--limit);
																				
			if (numberCombinations == 0)
																					return (-1);
																				
			}	// end "for (limit=1; limit <= numberLoops; limit++)" 
			
		}	// end "else stepSearchFlag" 
			
	return (numberLoops);
	
}	// end "GetCombinationLimit" 


                                                                                
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		UInt32 GetMaxCombinationSelected
//
//	Software purpose:	The purpose of this routine is to determine the
//							the maximum number of combinations that exist for
//							the input vector of "m" groups of items with "x"
//							contiguous items per group out of a total on "n"
//							items.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			SeparabilityDialog in SFeatureSelection.cpp
//
//	Coded By:			C.H. Lee					Date: 11/10/1989
//	Revised By:			Larry L. Biehl			Date: 10/08/1998	

UInt32 GetMaxCombinationSelected (
				SInt32								numberSubsets, 
				SInt16*								subsetPtr, 
				SInt32								total, 
				SInt32								numberContiguousPerGroup, 
				Boolean								stepSearchFlag,
				SInt16*								numberCombinationReturnCodePtr)

{
			// Declare local variables and structures										

	SInt32								difference,
											index,
											lastDifference,
											subsetMax;
											
	UInt32								numberCombinations;
	 
	
			// Check input parameters.															
	
	*numberCombinationReturnCodePtr = 0;
			
	if (numberSubsets < 0 || numberSubsets > total)						
		*numberCombinationReturnCodePtr = -1;
		
	if (total <= 0)																
		*numberCombinationReturnCodePtr = -1;
		
	if (subsetPtr == NULL)														
		*numberCombinationReturnCodePtr = -1;
		
	if (numberContiguousPerGroup <= 0)
		*numberCombinationReturnCodePtr = -1;
		
	if (*numberCombinationReturnCodePtr == -1)
																						return (0);
		
	if (numberSubsets == 0)														
																						return (0);
	
			// Initialize local variables.													
	
	if (!stepSearchFlag)
		{	
		subsetMax = total/(numberContiguousPerGroup+1);
		lastDifference = total;
		index = 0;
		
		while (index < numberSubsets)
			{
			difference = labs (subsetMax-(SInt32)subsetPtr[index]);
			
			if (difference > lastDifference)
				break;
				
			lastDifference = difference;
			index++;
			
			}	// end "while (index < numberSubsets)" 
			
		}	// end "if (!stepSearchFlag)" 
		
	else	// stepSearchFlag 
		index = numberSubsets;
		
	numberCombinations = 0;
	
	if (index > 0)
		numberCombinations = GetNumberOfCombinations (total, 
																		subsetPtr[--index], 
																		numberContiguousPerGroup, 
																		stepSearchFlag,
																		numberCombinationReturnCodePtr);
												
	return (numberCombinations);
	
}	// end "GetMaxCombinationSelected" 


                                                                                
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetNextCombination
//
//	Software purpose:	The purpose of this routine is to determine the 
//							next combination of subsetNumber from totalNumber.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/06/1989
//	Revised By:			Larry L. Biehl			Date: 11/30/1992	

Boolean GetNextCombination (
				SInt16*								listPtr, 
				SInt32								subsetNumber, 
				SInt32								totalNumber, 
				SInt32								numberContiguousPerGroup)
                                   	
{
	SInt32								i,
											ip,
											is,
											j,
											lastIndex;
	
	
			// Initialize local variables.													
			
	ip = 0;
	lastIndex = subsetNumber * numberContiguousPerGroup - 1;

	for (i=lastIndex; 
			i>=0; 
			i-=numberContiguousPerGroup)
		{
		if (listPtr[i] < totalNumber-ip-1)
			{
			i -= numberContiguousPerGroup - 1;
			is = listPtr[i];
			for (j=i; 
					j<=lastIndex; 
					j++)
				listPtr[j] = (SInt16)(is + 1 + (j-i));
				
			return (TRUE);
			
			}	// end "if (listPtr[i] < totalNumber-ip-1)" 
		
		ip += numberContiguousPerGroup;
		
		}	// end "for (i=subsetNumber-1; i>=0; i--)" 
		
	return (FALSE);
	
}	// end "GetNextCombination" 


                                                                                
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetNextStepCombination
//
//	Software purpose:	The purpose of this routine is to determine the 
//							next combination of subsetNumber from totalNumber
//							using the step search approach.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/10/1992
//	Revised By:			Larry L. Biehl			Date: 12/11/1992	

Boolean GetNextStepCombination (
				SInt16*								listPtr, 
				SInt16*								listUsedPtr,
				SInt16*								nextChannelPtr, 
				SInt16								totalNumber, 
				SInt16								numberContiguousPerGroup)

{
	Boolean								nextSetFlag;
	                      	
	SInt32								i,
											ip,
											j,
											lastIndex;
									
	
			// Initialize local variables.													
			
			// Loop through all possible numberContiguousPerGroup combinations	
			// to find the next new combination that does not overlap with			
			// any in the 'used list'.															
			
	for (i=*nextChannelPtr;
			i<=totalNumber-numberContiguousPerGroup; 
			i++)
		{
		nextSetFlag = FALSE;
		lastIndex = i + numberContiguousPerGroup - 1;
		
		for (j=i;
				j<=lastIndex;
				j++)
			{
			if (listUsedPtr[j])
				{
				nextSetFlag = TRUE;
				break;
				
				}	// end "if (listUsedPtr[j])" 
				
			}	// end "for (j=i; ..." 
		
		if (!nextSetFlag)
			{
			ip = 0;
			for (j=0; 
					j<totalNumber; 
					j++)
				{
				if (listUsedPtr[j] || (j>=i && j<=lastIndex))
					{
					listPtr[ip] = (SInt16)j;
					ip++;
					
					}	// end "if (listUsedPtr[j] || ..." 
					
				}	// end "for (j=0; ..." 
				
			*nextChannelPtr = (SInt16)(i + 1);
				
			return (TRUE);
			
			}	// end "if (!nextSetFlag)" 
		
		}	// end "for (i=*lastChannelPtr+1; ..." 
		
	return (FALSE);
	
}	// end "GetNextStepCombination" 


/*                                                                               
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double GetProductVectorValues
//
//	Software purpose:	The purpose of this routine is to compute the
//							product of the input vector values.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			LoadSeparabilityStatistics in SFeatureSelection.cpp
//
//	Coded By:			Larry L. Biehl			Date: 07/30/1992
//	Revised By:			Larry L. Biehl			Date: 07/30/1992	

double GetProductVectorValues (
				HDoublePtr							vectorPtr, 
				UInt32								numberFeatures)

{
	double								product;
	
	UInt32								index;
	
	
			// Check input values.  Continue only if input parameters are within	
			// proper rangers																		
			
	product = 0.;
	if (numberFeatures > 0 && vectorPtr)
		{
		product = *vectorPtr;
	
		for (index=1; index<numberFeatures; index++)
			{
			vectorPtr++;
			product *= *vectorPtr;
			
			}	// end "for (index=0; index<numberFeatures; index++)" 
			
		}	// end "if (numberFeatures > 0 && vectorPtr)" 
			
	return (product);
					
}	// end "GetProductVectorValues" 
*/

                                                                                
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean InvertVectorValues
//
//	Software purpose:	The purpose of this routine is to invert each of the
//							values in the input vector.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			LoadSeparabilityStatistics in SFeatureSelection.cpp
//
//	Coded By:			Larry L. Biehl			Date: 07/30/1992
//	Revised By:			Larry L. Biehl			Date: 05/24/1993	

Boolean InvertVectorValues (
				HDoublePtr							inputVectorPtr, 
				HDoublePtr							outputVectorPtr, 
				UInt32								numberFeatures)

{
	UInt32								index;
	
	
			// Check input values.  Continue only if input parameters are within	
			// proper rangers																		
			
	if (numberFeatures > 0 && inputVectorPtr && outputVectorPtr)
		{
		for (index=0; index<numberFeatures; index++)
			{
			if (*inputVectorPtr != 0.)
				{
				*outputVectorPtr = 1./(*inputVectorPtr);
				inputVectorPtr++;
				outputVectorPtr++;
				
				}	// end "if (*inputVectorPtr > 0)" 
				
			else	// *inputVectorPtr == 0. 
																					return (FALSE);
			
			}	// end "for (index=0; index<numberFeatures; index++)" 
			
		}	// end "if (numberFeatures > 0 && inputVectorPtr && ..." 
		
	return (TRUE);
					
}	// end "InvertVectorValues" 


                                                                                
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ListSeparabilityMeasurements
//
//	Software purpose:	The purpose of this routine is to list the 
//							separability results in the output text window
//							and/or a disk file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 11/06/1989
//	Revised By:			Larry L. Biehl			Date: 03/14/2012

SInt16 ListSeparabilityMeasurements (
				UInt32								numberChannelGroupCombinations, 
				UInt32								numberClassCombinations, 
				SInt16								numberSeparabilityChannels)

{
	double								maxDistanceRangeToList,
											minDistanceRangeToList; 
											
	CMFileStream*						resultsFileStreamPtr; 
	SepDistancesSummaryPtr			distancesSummaryPtr;
	char*									stringPtr1;
	float*								distancesPtr;
	
	SInt16								*channelSetsPtr,
											*weightsPtr;
	
	SInt32								channelCombination,
											numberCombinationsToList;
	 
	UInt32								channelSetIndex, 
											classPair,
											combinationIndex,
											numberBytes,
											stringLength;
											
	SInt32								distancesIndex;
	
	SInt16								channel,               
											fieldSize,
											precision,
											numberCharacters, 
											numberSpaces,
											channelNumberWidth,
											returnCode,
											savedNumberSpaces;
	
	Boolean								continueFlag,
											stopFlag;
													
	
			// Check input parameters.															
			// The check for numberClassCombinations being less than 0 because 	
			// as of 11/6/1989, LSC 4 does not handle unsigned long int correctly 	
			// in the 881 Math library.														
	
	if (numberSeparabilityChannels < 0)									return (-1);
	
			// Initialize local variables.													

	continueFlag = TRUE; 
	returnCode = -1;
	channelSetIndex = 0;
	distancesSummaryPtr = 
				(SepDistancesSummaryPtr)gSeparabilitySpecsPtr->distancesSummaryPtr;
	weightsPtr = (SInt16*)GetHandlePointer (
										gProjectInfoPtr->classPairWeightsHandle);
 	resultsFileStreamPtr = GetResultsFileStreamPtr (0);
				
	numberCombinationsToList = 
			MIN (gSeparabilitySpecsPtr->numberCombinationsToList, 
								gSeparabilitySpecsPtr->numberFeatureCombinationsLoaded);
	
	maxDistanceRangeToList = gSeparabilitySpecsPtr->maxDistanceRangeToList;														
	minDistanceRangeToList = gSeparabilitySpecsPtr->minDistanceRangeToList;
	
			// Set the distance measures field size and precision depending on	
			// the type of measure.																
			
	switch (gSeparabilitySpecsPtr->distanceMeasure)
		{
		case kBhattacharyya: 
		case kEFBhattacharyya:
		case kMeanBhattacharyya:
		case kCovarianceBhattacharyya:
		case kNonCovarianceBhattacharyya:
			fieldSize = 4;
			precision = 2;
			break;
			
		case kTransformedDivergence:
		case kDivergence:
			fieldSize = 4;
			precision = 0;
			break;
						
		}	// end "switch gSeparabilitySpecsPtr->distanceMeasure)" 
	
			// Get memory for a character buffer to be used in listing.  			
			// 	Allow 12 digits for the minimum and average,							
			// 	allow 6 digits for each class pair,										
			// 	allow 4 digits for each channel used in a set, 						
			// 	and allow 10 characters for labels and spaces.						
	
	numberBytes = 25 + 4*numberSeparabilityChannels + 6*2;
	
	if (gSeparabilitySpecsPtr->listClassPairDistances)
		{
		if (6*numberClassCombinations < 50)
			numberBytes += 50;
			
		else	// 6*numberClassCombinations >= 50 
			numberBytes += 6*numberClassCombinations;
			
		}	// end "if (gSeparabilitySpecsPtr->listClassPairDistances)" 
		
	gCharBufferPtr1 = (char*)MNewPointer (numberBytes);
	
	if (gCharBufferPtr1 != NULL)
		{
				// Indicate that at this point there are no problems to indicate	
				// upon returning from this routine.										
				
		returnCode = 1;
	
				// Initialize the channel combination index depending on the		
				// sorting method requested.													
				
		combinationIndex = 0;
		if (gSeparabilitySpecsPtr->sortChannelCombinations == 1)
			combinationIndex = gSeparabilitySpecsPtr->meanStart;
		else if (gSeparabilitySpecsPtr->sortChannelCombinations == 2)
			combinationIndex = gSeparabilitySpecsPtr->minimumStart;
		
		LoadDItemValue (gStatusDialogPtr, 
								IDC_Status10, 
								(SInt32)numberCombinationsToList);
		
				// List the header lines.														
				
		if (!ListSeparabilityTitleLine (resultsFileStreamPtr, 
														numberClassCombinations, 
														numberSeparabilityChannels))
			{
			gCharBufferPtr1 = CheckAndDisposePtr (gCharBufferPtr1);
			
																						return (-1);
			
			}	// end "if (!ListSeparabilityTitleLine (..." 
			
				// Get the channel number width.												
		
		channelNumberWidth = 3;
		SInt16* channelsPtr = (SInt16*)GetHandlePointer (
						gSeparabilitySpecsPtr->channelsHandle);	
		if (channelsPtr[gSeparabilitySpecsPtr->numberFeatures-1] >= 100)
			channelNumberWidth = 4;		
		if (channelsPtr[gSeparabilitySpecsPtr->numberFeatures-1] >= 1000)
			channelNumberWidth = 5;	
					
					// Put spaces if needed.													
		
		numberSpaces = channelNumberWidth * numberSeparabilityChannels;
		savedNumberSpaces = 8 - numberSpaces;
					
				// Intialize the nextTime variable to indicate when the next 		
				// check should occur for a command-.										
				
		gNextTime = TickCount ();
		gNextStatusTime = TickCount ();
		stopFlag = FALSE;
	
		for (channelCombination=1;
				channelCombination<=numberCombinationsToList;
				channelCombination++)
			{
				// List status of listing.														
		
			if (TickCount () >= gNextStatusTime)
				{
				LoadDItemValue (gStatusDialogPtr, 
										IDC_Status8, 
										channelCombination);
				gNextStatusTime = TickCount () + gNextStatusTimeOffset;
				
				}	// end "if (TickCount () >= gNextStatusTime)" 
			
					// Get the channel set description.										
					
			channelSetIndex = 
							distancesSummaryPtr[combinationIndex].channelSetIndex;
			channelSetsPtr = 
							&gSeparabilitySpecsPtr->channelSetsPtr[channelSetIndex];
							
					// List channel set number listed.										
			
			stringPtr1 = gCharBufferPtr1;
			sprintf (stringPtr1, "    %3d.\t", (int)channelCombination);
			stringPtr1 += 9;
		
					// List channels used.														
		   
			for (channel=0; channel<numberSeparabilityChannels; channel++)
				{
				sprintf (stringPtr1, "%*d", channelNumberWidth,
					(int)(gProjectInfoPtr->channelsPtr[channelSetsPtr[channel]]+1));
				stringPtr1 += channelNumberWidth;
				
				}	// end "for (channel=0; ..." 
					
					// Add spaces if needed.													
			
			numberSpaces = savedNumberSpaces;		
			while (numberSpaces > 0)
				{
				sprintf (stringPtr1, " ");
				stringPtr1++;
				numberSpaces--;
				
				}	// end "while (numberSpaces > 0)" 
			
					// If distance measurements are available, then list the			
					// class pair minimum and average.										
					
			distancesIndex = 
						distancesSummaryPtr[combinationIndex].classPairDistancesIndex;
			if (distancesIndex >= 0 && 
					distancesSummaryPtr[combinationIndex].classPairMinimum >= 
																		minDistanceRangeToList)
				{
						// List the minimum for this channel set.							
						
				sprintf (stringPtr1, "\t%7.*f",
							precision, 
							distancesSummaryPtr[combinationIndex].classPairMinimum);
				stringPtr1 += 8;
					
						// List the average for this channel set.							
						
				sprintf (stringPtr1, "\t%7.*f  ",
							precision, 
							distancesSummaryPtr[combinationIndex].classPairMean);
				stringPtr1 += 10;
							
						// List the class pair distances if requested.					
				
				if (gSeparabilitySpecsPtr->listClassPairDistances)
					{
							// Get the channel set description.								
							
					distancesPtr = 
									&gSeparabilitySpecsPtr->distancesPtr[distancesIndex];
						
					for (classPair=0; classPair<numberClassCombinations; classPair++)
						{
						if (weightsPtr[classPair] > 0)
							{
							if (*distancesPtr < maxDistanceRangeToList)
								sprintf (stringPtr1, "\t%4.*f", precision, *distancesPtr);
							
							else	// classPairDistance >= maxDistanceRangeToList 
								sprintf (stringPtr1, "\t ...");
								
							}	// end "if (weightsPtr[classPair] > 0)" 
							
						else	// weightsPtr[classPair] == 0 
							sprintf (stringPtr1, "\t    ");
							
						stringPtr1 += 5;
						distancesPtr++;
					
						}	// end "for (classPair=0; classPair<..." 
						
					}	// end "if (!gSeparabilitySpecsPtr->..." 
					
				}	// end "if (distancesIndex > 0)" 
				
					// Put carriage return at the end of the line and output the 	
					// string.																		
						                       
			numberCharacters = sprintf (stringPtr1, "%s", gEndOfLine);
			stringPtr1 += numberCharacters; 
			if (channelCombination == numberCombinationsToList)
				{
				numberCharacters = sprintf (stringPtr1, "%s", gEndOfLine);
				stringPtr1 += numberCharacters;
				
				}	// end "if (channelCombination == numberCombinationsToList)" 
				
			stringLength = (UInt32)((Ptr)stringPtr1 - (Ptr)gCharBufferPtr1);
			continueFlag = OutputString (resultsFileStreamPtr, 
													gCharBufferPtr1, 
													stringLength, 
													gOutputCode, 
													continueFlag);
			
					// Get the next channel combination.									
			
			switch (gSeparabilitySpecsPtr->sortChannelCombinations)
				{
				case 0:		// No sorting. In order computed. 
					combinationIndex = channelCombination;
					break;
					
				case 1:		// Sort by distance class pair average. 
					combinationIndex = 
									distancesSummaryPtr[combinationIndex].nextMean;
					break;
					
				case 2:		// Sort by distance class pair average. 
					combinationIndex = 
								distancesSummaryPtr[combinationIndex].nextMinimum;
					break;
								
				}	// end "switch (...->sortChannelCombinations)" 
			
					// Exit routine if user has "command period" down or error	
					// on listing output.														
			
			if (TickCount () >= gNextTime)
				{
				if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
					continueFlag = FALSE;
					
				}	// end "if (TickCount () >= nextTime)"
			
			
			if (!continueFlag)
				{
				returnCode = -1;
				channelCombination = numberCombinationsToList + 1;
				
				}	// end "if (!continueFlag)" 
		
					// Check if we are at the end of the channel combinations.		
					
			if (combinationIndex == -1)
				break;
	
			}	// end "for (channelCombination=1; ..." 
			
		LoadDItemValue (gStatusDialogPtr, 
								IDC_Status8, 
								(SInt32)numberCombinationsToList);
	
				// Make certain that the maximum number of characters in a line of
				// text takes into account the tab characters.							
		
		gMaxCharsInLine = MAX (gMaxCharsInLine, (SInt32)numberBytes);
			
		}	// end "if (gCharBufferHandle1)" 
			
	gCharBufferPtr1 = CheckAndDisposePtr (gCharBufferPtr1);
	
	return (returnCode);

}	// end "ListSeparabilityMeasurements" 


                                                                                
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ListSeparabilityTitleLine
//
//	Software purpose:	The purpose of this routine is to list the 
//							separability title lines.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/10/1989
//	Revised By:			Larry L. Biehl			Date: 02/14/2000	

Boolean ListSeparabilityTitleLine (
				CMFileStream*						resultsFileStreamPtr, 
				UInt32								numberClassCombinations, 
				SInt16								numberSeparabilityChannels)

{
	char									*savedStringPtr1,
											*stringPtr1;
										
	unsigned char*						symbolsPtr;
	
	SInt16 								*channelsPtr,
											*classPtr,
											*classPair2Ptr,
											*weightsPtr; 
										
	UInt32								classSet, 
											numberClasses;
	
	SInt16								classPairPtr[2],
											numberCharacters,
											numberSpaces;
	
		
			// Initialize local variables.													
	
	classPtr = (SInt16*)GetHandlePointer (
											gSeparabilitySpecsPtr->classHandle);
	symbolsPtr = (unsigned char*)GetHandlePointer (
											gSeparabilitySpecsPtr->symbolsHandle);
	weightsPtr = (SInt16*)GetHandlePointer (
											gProjectInfoPtr->classPairWeightsHandle);
	classPair2Ptr = &classPairPtr[1];
	numberClasses = gSeparabilitySpecsPtr->numberClasses;
		
			// List header line 1.  List symbol pairs.									
											
	stringPtr1 = gCharBufferPtr1;
	numberCharacters = (SInt16)sprintf (stringPtr1, "%s    \t\t\t", gEndOfLine);
	stringPtr1 += numberCharacters;
	
	numberSpaces = 3;
	channelsPtr = (SInt16*)GetHandlePointer (
						gSeparabilitySpecsPtr->channelsHandle);	
	if (channelsPtr[gSeparabilitySpecsPtr->numberFeatures-1] >= 100)
		numberSpaces = 4;
	numberSpaces *= numberSeparabilityChannels;
	numberSpaces = MAX (numberSpaces, 8);
	
	while (numberSpaces > 0)
		{
		sprintf (stringPtr1, " ");
		stringPtr1++;
		numberSpaces--;
		
		}	// end "while (numberSpaces > 0)" 
		
	savedStringPtr1 = stringPtr1;
	
			// Add the description info for class pair symbol line.					
			
	sprintf (stringPtr1, "class pair symbols >");
	stringPtr1 += 20;
	
	classPairPtr[0] = 0;
	classPairPtr[1] = 1;
	if (gSeparabilitySpecsPtr->listClassPairDistances)
		for (classSet=0; classSet<numberClassCombinations; classSet++)
			{
			sprintf (stringPtr1, "\t %1c%1c ", 
				symbolsPtr[classPtr[*classPairPtr]], 
				symbolsPtr[classPtr[*classPair2Ptr]]);
				
			stringPtr1 += 5;
		
					// Get the next class combination.											
			
			if (!GetNextCombination (classPairPtr, 2, numberClasses, 1))
				break;
			
			}	// end "for (classSet=0; ..." 
			
	if (!OutputString (resultsFileStreamPtr, 
								gCharBufferPtr1, 
								0, 
								gOutputCode, 
								TRUE))
																					return (FALSE);
		
			// List header line 2.  Class pair weights.									
						
	stringPtr1 = savedStringPtr1;
	
			// Add the description info for class pair symbol line.					
			
	sprintf (stringPtr1, "  weighting factor >");
	stringPtr1 += 20;
		
	if (gSeparabilitySpecsPtr->listClassPairDistances)
		for (classSet=0; classSet<numberClassCombinations; classSet++)
			{
			sprintf (stringPtr1, "\t(%2d)", weightsPtr[classSet]);
			stringPtr1 += 5;
			
			}	// end "for (classSet=0; ..." 
		
	if (!OutputString (resultsFileStreamPtr, 
								gCharBufferPtr1, 
								0, 
								gOutputCode, 
								TRUE))
																					return (FALSE);
	
			// List header line 3.																
			
	stringPtr1 = gCharBufferPtr1;
	
	if (gSeparabilitySpecsPtr->featureTransformationFlag)
		numberCharacters = 
						sprintf (stringPtr1, "%s          \tFeatures", gEndOfLine);
	else	// ...->featureTransformationFlag 
		numberCharacters = 
						sprintf (stringPtr1, "%s          \tChannels", gEndOfLine);
		
	stringPtr1 += numberCharacters;
	
	numberSpaces = 3;
	channelsPtr = (SInt16*)GetHandlePointer (
						gSeparabilitySpecsPtr->channelsHandle);	
	if (channelsPtr[gSeparabilitySpecsPtr->numberFeatures-1] >= 100)
		numberSpaces = 4;
	numberSpaces *= numberSeparabilityChannels;
	numberSpaces -= 9;
	while (numberSpaces > 0)
		{
		sprintf (stringPtr1, " ");
		stringPtr1++;
		numberSpaces--;
		
		}	// end "while (numberSpaces > 0)" 
		
	sprintf (stringPtr1, "\t  Min. \t  Ave. ");
	stringPtr1 += 16;
	
	if (gSeparabilitySpecsPtr->listClassPairDistances)
		{
		numberCharacters =
			sprintf (stringPtr1, "  \tWeighted Interclass Distance Measures");
		stringPtr1 += numberCharacters;
		
		}	// end "if (gSeparabilitySpecsPtr->listClassPairDistances)" 
		
	sprintf (stringPtr1, "%s", gEndOfLine);
	if (!OutputString (resultsFileStreamPtr, 
									gCharBufferPtr1, 
									0, 
									gOutputCode, 
									TRUE))
																					return (FALSE);

	return (TRUE);

}	// end "ListSeparabilityTitleLine" 


                                                                                
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadAllChannelCombinationsVector
//
//	Software purpose:	The purpose of this routine is to load the channels
//							combinations vector with a list of the number of
//							channels that can be used as a subset of the total
//							number of channels available and not go over a
//							maximum number of combinations.
//		
//	Parameters in:				
//
//	Parameters out:			
//
// Value Returned: 		
//
// Called By:			LoadSeparabilitySpecs in SFeatureSelection.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/10/1989
//	Revised By:			Larry L. Biehl			Date: 05/01/1998

SInt32 LoadAllChannelCombinationsVector (
				SInt16								totalNumberChannels, 
				UInt16*								combinationPtr, 
				SInt32*								maxNumberCombinationsPtr,
				SInt16								numberContiguousPerGroup, 
				Boolean								stepSearchFlag)

{
	SInt32								channelCombination,
											count,
											limit,
											maxNumberGroups,
											middleNumberChannels;
	
	SInt16								numberCombinationReturnCode;
	
	
	maxNumberGroups = totalNumberChannels/numberContiguousPerGroup;
		
			// Get the limit, if any, for the number of channels in a channel		
			// combination.																		
			
	limit = GetCombinationLimit (
					totalNumberChannels, numberContiguousPerGroup, stepSearchFlag);
	
	*maxNumberCombinationsPtr = GetNumberOfCombinations (totalNumberChannels, 
																			limit,
																			numberContiguousPerGroup, 
																			stepSearchFlag,
																			&numberCombinationReturnCode);
			
	middleNumberChannels = totalNumberChannels/(numberContiguousPerGroup+1);
	
	count = 0;
	if (limit > 0)
		{
		limit = middleNumberChannels - limit;
	
		for (channelCombination=1; 
				channelCombination<=maxNumberGroups; 
				channelCombination++)
			{
			if (labs (middleNumberChannels-channelCombination) >= limit)
				{
				combinationPtr[count] = (SInt16)channelCombination;
				count++;
					
				}	// end "if (abs (middleNumberChannels-..." 
				
			if (middleNumberChannels == channelCombination)
				middleNumberChannels = (maxNumberGroups+1)/2;
				
			}	// end "for (channelCombination=1; channel...)" 
			
		}	// end "if (limit > 0)" 
			
	return (count);
							
}	// end "LoadAllChannelCombinationsVector" 


                                                                                
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadChannelCombinationsVector
//
//	Software purpose:	The purpose of this routine is to load the input channels
//							combinations vector with the list of all possible
//							channel combinations.
//		
//	Parameters in:				
//
//	Parameters out:			
//
// Value Returned: 		
//
// Called By:			LoadSeparabilitySpecs in SFeatureSelection.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/27/1998
//	Revised By:			Larry L. Biehl			Date: 04/28/1998

void LoadChannelCombinationsVector (
				SeparabilitySpecsPtr				separabilitySpecsPtr,
				UInt16*								channelCombinationsPtr, 
				UInt16*								allChanCombinationsPtr,
				UInt16*								numberChannelGroupCombinationsPtr, 
				UInt32*								maxNumberFeatureCombinationsPtr)

{
	UInt32								index,
											numberChannelGroupCombinations;
											
											
	numberChannelGroupCombinations = 
								separabilitySpecsPtr->numberAllChanCombinations;
								
	for (index=0; index<numberChannelGroupCombinations; index++)
		channelCombinationsPtr[index] = allChanCombinationsPtr[index];
							
	*numberChannelGroupCombinationsPtr = 
								separabilitySpecsPtr->numberAllChanCombinations;
															
	*maxNumberFeatureCombinationsPtr =
							separabilitySpecsPtr->maxNumberAllChanCombinations;
							
}	// end "LoadChannelCombinationsVector" 


                                                                                
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean LoadSeparabilitySpecs
//
//	Software purpose:	The purpose of this routine is to load the 
//							Separability specification structure with the 
//							default set of parameters.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			SeparabilityControl
//
//	Coded By:			Larry L. Biehl			Date: 10/25/1989
//	Revised By:			Larry L. Biehl			Date: 04/01/2000

Boolean LoadSeparabilitySpecs (void)

{
	SInt16								*channelsPtr,
											*classPtr,
											*featurePtr,
											*weightsPtr;
											
	UInt16								*allChanCombinationsPtr,
											*channelCombinationsPtr;
											
	
	unsigned char						*symbolsPtr; 
	
	SInt32								bytesNeeded;
	
	SInt16								numberPairReturnCode; 
		
	Boolean								changedFlag,
											continueFlag;
	
	
			// Assume routine will work okay.  If there isn't enough memory		
			// then the flag will be changed.												
			
	continueFlag = TRUE;
	
	if (gProjectInfoPtr->separabilitySpecsH == NULL)
		{
				// Set up handle to project file information								
			
		gProjectInfoPtr->separabilitySpecsH =
													MNewHandle (sizeof (SeparabilitySpecs));
		if (gProjectInfoPtr->separabilitySpecsH != NULL)
			{
			gSeparabilitySpecsPtr = (SeparabilitySpecsPtr)GetHandlePointer (
										gProjectInfoPtr->separabilitySpecsH, 
										kLock, 
										kMoveHi);
			
					// Initialize the classification specification structure			
					
			gSeparabilitySpecsPtr->distanceMeasure = kBhattacharyya;
			gSeparabilitySpecsPtr->featureSearchCode = 1;
			gSeparabilitySpecsPtr->outputStorageType = 1;
			gSeparabilitySpecsPtr->channelSet = kAllMenuItem;
			gSeparabilitySpecsPtr->classSet = kAllMenuItem;
			gSeparabilitySpecsPtr->symbolSet = kDefaultMenuItem;
			gSeparabilitySpecsPtr->numberChannels =
													gProjectInfoPtr->numberStatisticsChannels;
			gSeparabilitySpecsPtr->numberClasses =
													gProjectInfoPtr->numberStatisticsClasses;
			gSeparabilitySpecsPtr->numberFeatures = 
													gProjectInfoPtr->numberStatisticsChannels;
			gSeparabilitySpecsPtr->channelCombinationSet = kSubsetMenuItem;
			gSeparabilitySpecsPtr->numberChannelGroupCombinations = 0;
			gSeparabilitySpecsPtr->tablesToList = 1;
			
			gSeparabilitySpecsPtr->listClassPairDistances = TRUE;
			if (gSeparabilitySpecsPtr->numberClasses > 5)
				gSeparabilitySpecsPtr->listClassPairDistances = FALSE;
				
			gSeparabilitySpecsPtr->numberCombinationsToList = 20;
			gSeparabilitySpecsPtr->numberContiguousPerGroup = 1;
			gSeparabilitySpecsPtr->sortChannelCombinations = 2;
			gSeparabilitySpecsPtr->numberShowCombinations = 0;
			gSeparabilitySpecsPtr->numberExcludeCombinations = 0;
			gSeparabilitySpecsPtr->minDistanceRangeToList = (float)0.;
			gSeparabilitySpecsPtr->maxDistanceRangeToList = (float)30000.;
			gSeparabilitySpecsPtr->distancesLessThanToList = (float)1500.;
			gSeparabilitySpecsPtr->distancesLessThanToGroup = (float)1500.;		
			gSeparabilitySpecsPtr->featureHandle = NULL;
			gSeparabilitySpecsPtr->channelsHandle = NULL;
			gSeparabilitySpecsPtr->classHandle = NULL;
			gSeparabilitySpecsPtr->symbolsHandle = NULL;
			gSeparabilitySpecsPtr->channelCombinationsHandle = NULL;
			gSeparabilitySpecsPtr->allChanCombinationsHandle = NULL;
			gSeparabilitySpecsPtr->showCombinationsH = NULL;
			gSeparabilitySpecsPtr->excludeCombinationsH = NULL;
			gSeparabilitySpecsPtr->showExcludeCombinationsH = NULL;
			
			gSeparabilitySpecsPtr->channelSetsPtr = NULL;
			gSeparabilitySpecsPtr->distancesPtr = NULL;
			gSeparabilitySpecsPtr->distancesSummaryPtr = NULL;
			gSeparabilitySpecsPtr->maxNumberFeatureCombinations = 0;
			gSeparabilitySpecsPtr->numberClassCombinations = 0;
			gSeparabilitySpecsPtr->numberFeatureCombinationsLoaded = 0;
			gSeparabilitySpecsPtr->combinationsToStore = 20;
			gSeparabilitySpecsPtr->maxCombinationsToStore = 20;
			gSeparabilitySpecsPtr->combinationSlots = 0;
	
			gSeparabilitySpecsPtr->minimumStart = 0;
			gSeparabilitySpecsPtr->minimumEnd = 0;
			gSeparabilitySpecsPtr->meanStart = 0;
			gSeparabilitySpecsPtr->meanEnd = 0;
			
			gSeparabilitySpecsPtr->featureTransformationFlag = FALSE;
			
			}	// end "if (gProjectInfoPtr->separabilitySpecsH != NULL)" 
		
		else	// else gProjectInfoPtr->separabilitySpecsH == NULL 
			continueFlag = FALSE;
			
		}	// end "if (gProjectInfoPtr->separabilitySpecsH == NULL)" 
		
	else	// gProjectInfoPtr->separabilitySpecsH != NULL 
		{
				// Lock the handle to the classification specifications and get	
				// a pointer to the structure.												
				
		gSeparabilitySpecsPtr = (SeparabilitySpecsPtr)GetHandlePointer (
										gProjectInfoPtr->separabilitySpecsH, 
										kLock, 
										kMoveHi);
		
				// Make sure that the featureTransformationFlag makes sense.
										
		if (!EigenvectorInfoExists (kProject))
			gSeparabilitySpecsPtr->featureTransformationFlag = FALSE;
				
		}	// end "else gProjectInfoPtr->gSeparabilitySpecsPtr != NULL" 
					
			// Set up memory for separability classes vector.							
	
	if (continueFlag)
		{	
		if (gProjectInfoPtr->statisticsCode != kMeanCovariance)
			gSeparabilitySpecsPtr->distanceMeasure = kNonCovarianceBhattacharyya;
				
		bytesNeeded =
			(SInt32)gProjectInfoPtr->numberStatisticsClasses * sizeof (SInt16);
		classPtr = (SInt16*)CheckHandleSize (
									&gSeparabilitySpecsPtr->classHandle,
									&continueFlag,  
									&changedFlag, 
									bytesNeeded);
		if (changedFlag)
			gSeparabilitySpecsPtr->classSet = kAllMenuItem;
			
		}	// end "if (continueFlag)" 
			
			// If memory is not full, set up memory for separability					
			// feature vector pointer.															

	if (continueFlag)
		{
		bytesNeeded =
			(SInt32)gProjectInfoPtr->numberStatisticsChannels * sizeof (SInt16);
		featurePtr = (SInt16*)CheckHandleSize (&gSeparabilitySpecsPtr->featureHandle,
															&continueFlag,   
															&changedFlag, 
															bytesNeeded);
		if (changedFlag)
			gSeparabilitySpecsPtr->channelSet = kAllMenuItem;
			
		}	// end "if (continueFlag)" 
			
			// If memory is not full, set up memory for separability					
			// channels vector.																	

	if (continueFlag)
		channelsPtr = (SInt16*)CheckHandleSize( &gSeparabilitySpecsPtr->channelsHandle,
																&continueFlag,    
																&changedFlag, 
																bytesNeeded);
	
			// If memory is not full, set up memory for separability					
			// channel combinations vector.													

	if (continueFlag)
		channelCombinationsPtr = (UInt16*)CheckHandleSize (
											&gSeparabilitySpecsPtr->channelCombinationsHandle,
											&continueFlag,    
											&changedFlag, 
											bytesNeeded);
	
			// If memory is not full, set up memory for separability					
			// vector for all possible channels combinations.							

	if (continueFlag)
		allChanCombinationsPtr = (UInt16*)CheckHandleSize (
											&gSeparabilitySpecsPtr->allChanCombinationsHandle,
											&continueFlag,    
											&changedFlag, 
											bytesNeeded);
	
			// If memory is not full, set up memory for class symbols				
			// vector.																				

	if (continueFlag)
		{						
		bytesNeeded = (SInt32)(gProjectInfoPtr->numberStatisticsClasses+1)
																			* sizeof (char);
		symbolsPtr = (UCharPtr)CheckHandleSize (&gSeparabilitySpecsPtr->symbolsHandle,
																&continueFlag,     
																&changedFlag, 
																bytesNeeded);
		if (changedFlag)
			gSeparabilitySpecsPtr->symbolSet = kDefaultMenuItem;
			
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
		weightsPtr = (SInt16*)CheckHandleSize (&gProjectInfoPtr->classPairWeightsHandle,
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
		
		if (gSeparabilitySpecsPtr->channelSet == kAllMenuItem)		
			LoadFeatureVector (gSeparabilitySpecsPtr->featureTransformationFlag,
										(SInt16*)&gSeparabilitySpecsPtr->numberFeatures,
										featurePtr,
										gProjectInfoPtr->numberStatisticsChannels);
															
		LoadChannelsVector (kProject,
									gSeparabilitySpecsPtr->featureTransformationFlag,
									gSeparabilitySpecsPtr->numberFeatures, 
									featurePtr, 
									(SInt16*)&gSeparabilitySpecsPtr->numberChannels,
									channelsPtr);
		
		if (gSeparabilitySpecsPtr->classSet == kAllMenuItem)								
			LoadTrainClassVector (&gSeparabilitySpecsPtr->numberClasses,
											&gSeparabilitySpecsPtr->classSet, 
											classPtr);
						
		if (gSeparabilitySpecsPtr->symbolSet == kDefaultMenuItem)
			LoadClassSymbolVector (symbolsPtr, 
								IDS_Symbol1, gProjectInfoPtr->numberStatisticsClasses);
		
				// Load the all possible channel combination vector.					
				
		gSeparabilitySpecsPtr->numberAllChanCombinations =
				(SInt16)LoadAllChannelCombinationsVector (
							gSeparabilitySpecsPtr->numberFeatures, 
							allChanCombinationsPtr,
							&gSeparabilitySpecsPtr->maxNumberAllChanCombinations,
							gSeparabilitySpecsPtr->numberContiguousPerGroup,
							(gSeparabilitySpecsPtr->featureSearchCode==2));
		
				// Load the selected channels combinations vector if needed.		
							
		if (gSeparabilitySpecsPtr->channelCombinationSet == kAllMenuItem)
			LoadChannelCombinationsVector (
								gSeparabilitySpecsPtr,
								channelCombinationsPtr, 
								allChanCombinationsPtr,
								&gSeparabilitySpecsPtr->numberChannelGroupCombinations, 
								(UInt32*)&gSeparabilitySpecsPtr->maxNumberFeatureCombinations);
									
				// Get the number of class combinations currently selected.			
				
		gSeparabilitySpecsPtr->numberClassCombinations = 
			GetNumberOfCombinations ((SInt16)gSeparabilitySpecsPtr->numberClasses, 
												2, 
												1, 
												FALSE,
												&numberPairReturnCode);
			
		}	// end "if (continueFlag)" 
	
			// If unable to set up information for separability						
			// specifications, release memory allocated to it (if any).				
			
	else	// !continueFlag 
		ReleaseSeparabilitySpecsMemory (&gProjectInfoPtr->separabilitySpecsH);
		
	return (continueFlag);

}	// end "LoadSeparabilitySpecs" 


                                                                                
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 LoadSeparabilityStatistics
//
//	Software purpose:	The purpose of this routine is to load the
//							covariance matrix, mean vector, and inverse
//							covariance matrix into the memory allocated for
//							the separability statistics for the specified
//							set of channels for all classes.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	-1 if statistics loading is not okay.
//							1 if statistics is okay.
// 
// Called By:			ComputeSeparabilityForAllChannelCombinations in SFeatureSelection.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/03/1989
//	Revised By:			Larry L. Biehl			Date: 06/17/2006

SInt16  LoadSeparabilityStatistics (
				SInt16*								featurePtr, 
				SInt16								numberFeatures)

{
	CMFileStream*						resultsFileStreamPtr;
	
	HDoublePtr							inversePtr;
	
	SInt16								*classPtr,
											*statFeaturePtr;
	
	UInt32								classIndex,
											numberSepCovEntries,
											sepCovStart,
											sepMeanStart;
	
	SInt16								numberFeatureChannels,
											statClassNumber,
											statisticsCode,
											stringIndex;
	
	Boolean								continueFlag,
											determinantOKFlag,
											getInverseMatrixFlag,
											listInversionMessage;


			// Check input parameters.															
			
	if (featurePtr == NULL)														return (-1);
	
	if (numberFeatures <= 0 || numberFeatures > kMaxNumberStatChannels)
																						return (-1);
	
	numberFeatureChannels = numberFeatures;
	
			// Determine if the covariance matrices are needed.						
			
	statisticsCode = kMeanCovariance;
	if (gSeparabilitySpecsPtr->distanceMeasure == kNonCovarianceBhattacharyya)
		statisticsCode = kMeanStdDevOnly;
								
	if (gSeparabilitySpecsPtr->featureTransformationFlag)
		{
		numberFeatureChannels = gSeparabilitySpecsPtr->numberChannels;
		
				// Get the transformation matrix that will be used for this			
				// set of features.									
		
		HDoublePtr eigenVectorPtr = (HDoublePtr)GetHandlePointer (
											gTransformationMatrix.eigenVectorHandle,
											kLock);
		
		ReduceMatrix (eigenVectorPtr, 
							gTransformationMatrix.eigenVectorPtr, 
							gTransformationMatrix.numberFeatures, 
							numberFeatureChannels, 
							numberFeatures, 
							featurePtr,
							(gTransformationMatrix.createdByCode >= 16));
							
		CheckAndUnlockHandle (gTransformationMatrix.eigenVectorHandle);
		
		}	// end "if (...->featureTransformationFlag)" 
		
	statFeaturePtr = (SInt16*)GetStatisticsFeatureVector (
									gSeparabilitySpecsPtr->featureTransformationFlag, 
									(UInt16*)featurePtr);
	
			// Initialize indices for the storage of the mean vectors,				
			// covariance matrices and inverse covariance matrices for all			
			// the classes.																		
			
	sepMeanStart = 0;
	sepCovStart = 0;
										
	numberSepCovEntries = (UInt32)numberFeatures;
	if (statisticsCode == kMeanCovariance)
		{
		numberSepCovEntries *= (numberFeatures+1);
		numberSepCovEntries /= 2;
		
		}	// end "if (statisticsCode == kMeanCovariance)" 
									
			// Initialize other local variables.											
			
	classPtr = (SInt16*)GetHandlePointer (
											gSeparabilitySpecsPtr->classHandle,
											kLock);

			// Loop through the classes to be used in the separability 				
			// calculations and get the:														
			//		mean vector for class.														
			//		covariance matrix for class.												
			//		determinant of covariance matrix	for class.							
			//		inverse of covariance matrix for class.								
	
	continueFlag = TRUE;
	determinantOKFlag = TRUE;
	
	getInverseMatrixFlag = kGetDeterminantOnly;
	if (gSeparabilitySpecsPtr->distanceMeasure == kTransformedDivergence ||
			gSeparabilitySpecsPtr->distanceMeasure == kDivergence)
		getInverseMatrixFlag = kReturnMatrixInverse;
	
	inversePtr = NULL;
	
	for (classIndex=0; 
			classIndex<gSeparabilitySpecsPtr->numberClasses; 
			classIndex++)
		{
		listInversionMessage = FALSE;
		statClassNumber = classPtr[classIndex] - 1;
			
					// Get the class covariance matrix - triangular	form. 			

		GetTransformedClassCovarianceMatrix (
										numberFeatureChannels, 
										gTempChannelStatsPtr, 
										&gSepCovPtr[sepCovStart], 
										(UInt16*)statFeaturePtr, 
										statClassNumber,
										kTriangleOutputMatrix,
										statisticsCode,
										gTransformationMatrix.eigenVectorPtr,
										gTransformationMatrix.tempMatrixPtr,
										numberFeatures);
							
				// Get the class mean vector													
		
		GetTransformedMeanVector (gTempChannelStatsPtr, 
										&gSepMeanPtr[sepMeanStart],
										numberFeatureChannels,
										NULL,
										gTransformationMatrix.eigenVectorPtr,
										gTransformationMatrix.tempMatrixPtr,
										gTransformationMatrix.offsetVectorPtr,
										numberFeatures);
		
		if (gSepInvPtr != NULL)
			inversePtr = &gSepInvPtr[sepCovStart];
		
				// Get the inverse of the class covariances or variances	and the
				// log of the determinant if needed.
				
		if (gSeparabilitySpecsPtr->distanceMeasure != kMeanBhattacharyya)
			{
			if (statisticsCode == kMeanCovariance)
				{
						// Get the inverse of the covariance matrix and the				
						// determinant.																
						
				determinantOKFlag = InvertLowerTriangularMatrix (
																	&gSepCovPtr[sepCovStart], 
																	inversePtr,
																	numberFeatures, 
																	NULL,
																	&gSepLogDetPtr[classIndex],
																	getInverseMatrixFlag);
										
				if (gOperationCanceledFlag)
					continueFlag = FALSE;
					
				else	// !gOperationCanceledFlag 
					{
					if (!determinantOKFlag)
						listInversionMessage = TRUE;
					
					}	// end "else !gOperationCanceledFlag" 
					
				}	// end "if (statisticsCode == kMeanCovariance)" 
				
			else	// statisticsCode != kMeanCovariance 
				{
				if (!InvertVectorValues (&gSepCovPtr[sepCovStart], 
													inversePtr, 
													(UInt32)numberFeatures))
					listInversionMessage = TRUE;
				
				}	// end "else statisticsCode != kMeanCovariance" 
			
			if (listInversionMessage)
				{
						// List message that covariance matrix or variance vector 			
						// could not be inverted for the requested class and features.		
						
				resultsFileStreamPtr = GetResultsFileStreamPtr (0);
						
				if (statisticsCode == kMeanCovariance)
					stringIndex = 31;
							
				else	// statisticsCode != kMeanCovariance 
					stringIndex = 32;
						
				continueFlag = ListClassInformationMessage (
												kProjectStrID, 
												stringIndex,
												resultsFileStreamPtr, 
												gOutputForce1Code,
												statClassNumber, 
												continueFlag);
						
				determinantOKFlag = FALSE;
				
				}	// end "if (listInversionMessage)" 
				
			}	// end "if (...->distanceMeasure != kMeanBhattacharyya)"
		
		sepMeanStart += numberFeatures;
		sepCovStart += numberSepCovEntries;
		
		if (!continueFlag)
			break;
			
		}	// end "for (classIndex=0; classIndex<..." 	
		
	if (!determinantOKFlag)			
		SysBeep (10);
		
	if (!determinantOKFlag || !continueFlag)		
																							return (-1);
																					
	return (1);
				
}	// end "LoadSeparabilityStatistics" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double MeanBhattacharyya
//
//	Software purpose:	The purpose of this routine is to calculate
//							the Bhattacharyya distance between the two classes 
//							represented by their input covariance matrices, 
//							inverse covariance matrices and mean vectors.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:	
//
//	Coded By:			C.H. Lee					Date: 03/06/1998
//	Revised By:			Larry L. Biehl			Date: 03/06/1998

double MeanBhattacharyya (
				HDoublePtr							mean1Ptr, 			// mean vector of class 1 
				HDoublePtr							mean2Ptr, 			// mean vector of class 2 
				HDoublePtr							cov1Ptr, 			// covariance matrix of class 1 
				HDoublePtr							cov2Ptr,				// covariance matrix of class 2
				HDoublePtr							meanDifPtr, 		// vector storage of class 1 and 	
																				// class 2 mean difference. 			
				SInt16								numberFeatures)	// number of features (channels) 
																				// represented in the mean vector  	
																				// and covariance and inverse  	
																				// covariance matrices.																						
											
{
	double								bhattacharyya1,
											bhattacharyya2,
											logDeterminant,
											meanDifValue;
	
	HDoublePtr							temp1Ptr,
											temp2Ptr,
											tMeanDifPtr,
											tInversePtr;
											
	UInt32								channel1,
											channel2,
											tmpIndex1,
											tmpIndex2;
	
			
			// Get 1/2 of the sum of the two covariances.
			// The 1/2 will be taken care of in the mean part by only dividing
			// by 4 instead of 8.						
	
	tInversePtr = gInverseMatrixMemory.inversePtr;
	temp1Ptr = cov1Ptr;
	temp2Ptr = cov2Ptr;
	
	for (channel1=0; channel1<(UInt32)numberFeatures; channel1++)
		{
		tmpIndex1 = (UInt32)channel1*numberFeatures;
		tmpIndex2 = channel1;
		
		for (channel2=0; channel2<channel1; channel2++)
			{	
			tInversePtr[tmpIndex1] = tInversePtr[tmpIndex2] = 
																(*temp1Ptr + *temp2Ptr);
			
			temp1Ptr++;
			temp2Ptr++;
		
			tmpIndex1++;
			tmpIndex2 += numberFeatures;
			
			}	// end "for (channel2=1; channel2<channel1; channel2++)" 
			
		tInversePtr[tmpIndex1] = (*temp1Ptr + *temp2Ptr);
			
		temp1Ptr++;
		temp2Ptr++;
			
		}	// end "for (channel1=1; channel1<=numOutFeatures; channel1++)" 
		
	InvertSymmetricMatrix (
									tInversePtr, 
									numberFeatures, 
									gInverseMatrixMemory.pivotPtr, 
									gInverseMatrixMemory.indexRowPtr, 
									gInverseMatrixMemory.indexColPtr, 
									gInverseMatrixMemory.ipvotPtr,
									NULL,
									&logDeterminant,
									kReturnMatrixInverse);
									
	if (gOperationCanceledFlag)
																						return (0);
	
			// Get the mean difference vector.		
	
	tMeanDifPtr = meanDifPtr;
	temp1Ptr = mean1Ptr;
	temp2Ptr = mean2Ptr;
			
	for (channel1=0; channel1<(UInt32)numberFeatures; channel1++)
		{
		*tMeanDifPtr = *temp1Ptr - *temp2Ptr;
		tMeanDifPtr++;
		temp1Ptr++;
		temp2Ptr++;
		
		}	// end "for (channel1=0; channel1<numberFeatures; channel1++)" 
			
			// Get the i=j component items of the mean part of the 					
			// Bhattacharyya distance.															
							
	tMeanDifPtr = meanDifPtr;			
	bhattacharyya1 = 0;
	tmpIndex2 = numberFeatures + 1;
	for (channel1=0; channel1<(UInt32)numberFeatures; channel1++)
		{
		meanDifValue = *tMeanDifPtr;
		bhattacharyya1 += *tInversePtr * meanDifValue * meanDifValue;
		tInversePtr += tmpIndex2;
		tMeanDifPtr++;
		
		}	// end "for (channel1=0; channel1<numberFeatures; channel1++)" 
		
	bhattacharyya1 *= .25;
			
			// Get the i!=j component items of the mean part of the 				
			// Bhattacharyya distance.		
			// Note that the i!=j component is double the value computed for
			// bhattacharyya2.
			// This is taken care of by dividing by 2 instead of 4 at the end of the
			// computation.																	
	
	tInversePtr = &gInverseMatrixMemory.inversePtr[numberFeatures];
	temp1Ptr = meanDifPtr + 1;			
	bhattacharyya2 = 0;
	tmpIndex2 = numberFeatures - 1;
	for (channel1=1; channel1<(UInt32)numberFeatures; channel1++)
		{
		meanDifValue = *temp1Ptr;
		temp2Ptr = meanDifPtr;			
		for (channel2=0; channel2<channel1; channel2++)
			{ 
			bhattacharyya2 += *tInversePtr * meanDifValue * *temp2Ptr;
			tInversePtr++;
			temp2Ptr++;
			
			}	// end "for (channel2=0; channel2<numberFeatures; channel2++)" 
		
		temp1Ptr++;
		tInversePtr += tmpIndex2;
		tmpIndex2--;
		
		}	// end "for (channel1=1; channel1<numberFeatures; channel1++)" 

	bhattacharyya1 += .5 * bhattacharyya2;
		
	return (bhattacharyya1);
	
}	// end "MeanBhattacharyya" 


                                                                                
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double NonCovarianceBhattacharyya
//
//	Software purpose:	The purpose of this routine is to calculate
//							the Bhattacharyya distance between the two classes 
//							represented by their input variance and mean vectors.
//							Note that this is not the same as the 'mean' portion of the
//							Bhattacharyya distance measure.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			C.H. Lee					Date: 12/04/1988
//	Revised By:			Larry L. Biehl			Date: 03/05/1998	

double NonCovarianceBhattacharyya (
				HDoublePtr							mean1Ptr, 		// mean vector of class 1 
				HDoublePtr							mean2Ptr, 		// mean vector of class 2 
				HDoublePtr							cov1Ptr, 		// covariance matrix of class 1 
				HDoublePtr							cov2Ptr,			// covariance matrix of class 2
				HDoublePtr							meanDifPtr, 	// vector storage of class 1 and 		
																			// class 2 mean difference. 				
				UInt32								numberFeatures)	// number of features (channels)  		
																		// represented in the mean vector and 	
																		// covariance and inverse covariance 	
																		// matrices.									
											
{
	double								bhattacharyya1,
											meanDifValue;
	
	double								*tInversePtr,
						     				*temp1Ptr,
											*temp2Ptr;
	
	UInt32								channel1;
	
			
			// Get 1/2 of the sum of the two variances.		
			// The 1/2 will be taken care of in the mean part by only dividing
			// by 4 instead of 8.										
	
	tInversePtr = gInverseMatrixMemory.inversePtr;
	temp1Ptr = cov1Ptr;
	temp2Ptr = cov2Ptr;
	
	for (channel1=0; channel1<numberFeatures; channel1++)
		{
		*tInversePtr = *temp1Ptr + *temp2Ptr;
			
		temp1Ptr++;
		temp2Ptr++;
		tInversePtr++;
			
		}	// end "for (channel1=1; channel1<=numOutFeatures; channel1++)" 
		
			// Get the product of the mean of the two variances.						
			
	//tInversePtr = gInverseMatrixMemory.inversePtr;
	//determinant = *tInversePtr;
	//for (channel1=1; channel1<numberFeatures; channel1++)
	//	{
	//	tInversePtr++;
	//	determinant *= *tInversePtr;
	//			
	//	}	// end "for (channel1=1; channel1<=numOutFeatures; channel1++)"
	
			// Get the mean difference vector.												
			// Don't get confused with tInversePtr being used here.  This is		
			// temporary to take advantage of the register variable pointer.		
	
	tInversePtr = meanDifPtr;
	temp1Ptr = mean1Ptr;
	temp2Ptr = mean2Ptr;
			
	for (channel1=0; channel1<numberFeatures; channel1++)
		{
		*tInversePtr = *temp1Ptr - *temp2Ptr;
		tInversePtr++;
		temp1Ptr++;
		temp2Ptr++;
		
		}	// end "for (channel1=0; channel1<numberFeatures; channel1++)" 
			
			// Get the i=j component items of the mean part of the 					
			// Bhattacharyya distance.															
							
	tInversePtr = gInverseMatrixMemory.inversePtr;
	temp1Ptr = meanDifPtr;			
	bhattacharyya1 = 0;
	for (channel1=0; channel1<numberFeatures; channel1++)
		{
		meanDifValue = *temp1Ptr;
		bhattacharyya1 += (1./(*tInversePtr)) * meanDifValue * meanDifValue;
		tInversePtr++;
		temp1Ptr++;
		
		}	// end "for (channel1=0; channel1<numberFeatures; channel1++)"
		
	bhattacharyya1 *= .25;
	
	return (bhattacharyya1);
	
}	// end "NonCovarianceBhattacharyya" 
													

                                                                                
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SeparabilityControl
//
//	Software purpose:	The purpose of this routine is to control the user
//							requests for separability operations on an image file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 09/10/1989
//	Revised By:			Larry L. Biehl			Date: 05/03/2019

void SeparabilityControl (void)

{
			// Define local variables and structures.										
	
	
	FileInfoPtr							fileInfoPtr;
	CMFileStream*						resultsFileStreamPtr;
	
	SInt16								*classPtr,
											*weightsListPtr;
							
	time_t								startTime; 
	
	SInt16								numberPairReturnCode;
							
	UInt16 								index;  
	
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
 	weightsListPtr = NULL;
	
			// Check handle to image file information.  If handle to image			
			// file information doesn't exists. Find the	image file and get		
			// the information for it.	  It is not mandantory that we have the	
			// information.																		
	
	if (GetProjectImageFileInfo (kDoNotPrompt, kSetupGlobalInfoPointersIfCan))
		{
				// Lock handle to file information and get pointer to it				
		
		fileInfoPtr = gImageFileInfoPtr;
		
				// Set up separability specification structure.							
				
		if (LoadSeparabilitySpecs ())
			{
			if (SeparabilityDialog (fileInfoPtr))
				{
						// Initialize some global variables pertaining to output.	
				
				gOutputTextOKFlag = TRUE;
				gOutputCode = gSeparabilitySpecsPtr->outputStorageType;
				gOutputForce1Code = (gOutputCode | 0x0001);
				continueFlag = TRUE;
				
				weightsListPtr = (SInt16*)GetHandlePointer (
											gProjectInfoPtr->classPairWeightsListHandle);
											
				classPtr = (SInt16*)GetHandlePointer (
						gSeparabilitySpecsPtr->classHandle);

						// Update statistics for project if needed		
						
				continueFlag = VerifyProjectStatsUpToDate (
								&gSeparabilitySpecsPtr->numberClasses,
								classPtr,
								2,
								gProjectInfoPtr->statisticsCode,
								gProjectInfoPtr->covarianceStatsToUse, 
								kSetupGlobalInfoPointersIfCan,
								NULL);	
										
							// Get fileInfoPtr again in case it was changed.

				if (continueFlag)
					{							
					fileInfoPtr = gImageFileInfoPtr;
									
							// Get the number of class combinations currently 			
							// selected.															
						
					gSeparabilitySpecsPtr->numberClassCombinations = 
							GetNumberOfCombinations (
											(SInt16)gSeparabilitySpecsPtr->numberClasses, 
											2,
											1,
											FALSE,
											&numberPairReturnCode);
								
					SInt16* classPairWeightsPtr = (SInt16*)GetHandlePointer (
											gProjectInfoPtr->classPairWeightsHandle);
												
					LoadClassPairWeightVector (
								classPairWeightsPtr,
								gSeparabilitySpecsPtr->numberClassCombinations, 
								gProjectInfoPtr->defaultClassPairWeight,
								&gProjectInfoPtr->classPairWeightSum,
								gProjectInfoPtr->classPairWeightSet,
								weightsListPtr,
								classPtr,
								(UInt16)gSeparabilitySpecsPtr->numberClasses);
									
					}	// end "if (continueFlag)"
					
				if (continueFlag)
					{
							// If separability results are to go the a disk file, 	
							// open the	disk file.	Trick 'CreateResultsDiskFiles' to
							// estimate the number of bytes on 'pseudo' lines and 	
							// columns.  Assume 128K bytes needed for now.				
					
					if (gSeparabilitySpecsPtr->outputStorageType & 0x0002)
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
														gSeparabilitySpecsPtr->outputStorageType,
														0,
														0);
											
						}	// end "if (...->outputStorageType & 0x0002)" 
											
	 				resultsFileStreamPtr = GetResultsFileStreamPtr (0);
	 				
	 				}	// end "if (continueFlag)" 
 				
						// Continue separability if everything is okay.					
				
				if (continueFlag)
					{
							// Change cursor to watch cursor until done with process	
						
					MSetCursor (kWait);
			
							// Force text selection to start from end of present text
						
					ForceTextToEnd ();
					
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
															gSeparabilitySpecsPtr->numberClasses,
															classPtr);
														
							// List the separability specifications.
							//   First list the algorithm to be used.														
			
					switch (gSeparabilitySpecsPtr->distanceMeasure)
						{
						case kBhattacharyya: 
							index = IDS_FeatureSelection1;
							break;
							
						case kEFBhattacharyya:
							index = IDS_FeatureSelection5;
							break;
							
						case kMeanBhattacharyya:
							index = IDS_FeatureSelection33;
							break;
							
						case kCovarianceBhattacharyya:
							index = IDS_FeatureSelection32;
							break;
							
						case kNonCovarianceBhattacharyya:
							index = IDS_FeatureSelection4;
							break;
							
						case kTransformedDivergence:
							index = IDS_FeatureSelection2;
							break;
							
						case kDivergence:
							index = IDS_FeatureSelection3;
							break;
										
						}	// end "switch gSeparabilitySpecsPtr->distanceMeasure)" 
		
					continueFlag = ListSpecifiedStringNumber (
													kFeatureSelectionStrID, 
													index, 
													(unsigned char*)gTextString, 
													resultsFileStreamPtr, 
													gOutputForce1Code,
													continueFlag);					
														
							// " List best %ld combinations\r"								
							
					continueFlag = ListSpecifiedStringNumber (
												kFeatureSelectionStrID,
												IDS_FeatureSelection18,
												resultsFileStreamPtr, 
												gOutputForce1Code, 
												gSeparabilitySpecsPtr->numberCombinationsToList, 
												continueFlag);
					
							// " Minimum value to be listed: %g\r"							
				
					continueFlag = ListSpecifiedStringNumber (
												kFeatureSelectionStrID,
												IDS_FeatureSelection19,
												resultsFileStreamPtr, 
												gOutputForce1Code, 
												gSeparabilitySpecsPtr->minDistanceRangeToList, 
												continueFlag);
					
							// " Maximum value to be listed: %g\r"							
				
					continueFlag = ListSpecifiedStringNumber (
												kFeatureSelectionStrID,
												IDS_FeatureSelection20,
												resultsFileStreamPtr, 
												gOutputForce1Code, 
												gSeparabilitySpecsPtr->maxDistanceRangeToList, 
												continueFlag);
					
					if (gSeparabilitySpecsPtr->tablesToList & 0x0001)
						{								
								// " List separability table.\r"	
				
						continueFlag = ListSpecifiedStringNumber (
												kFeatureSelectionStrID,
												IDS_FeatureSelection21, 
												(unsigned char*)gTextString,
												resultsFileStreamPtr, 
												gOutputForce1Code, 
												continueFlag);
						
						}	// end "if (gSeparabilitySpecsPtr->tablesToList)"
														
					if (gSeparabilitySpecsPtr->tablesToList & 0x0002)
						{							
								// " List table of class pairs with distance less than %g\r"
				
						continueFlag = ListSpecifiedStringNumber (
												kFeatureSelectionStrID,
												IDS_FeatureSelection22,
												resultsFileStreamPtr, 
												gOutputForce1Code, 
												gSeparabilitySpecsPtr->distancesLessThanToList, 
												continueFlag);
						
						}	// end "if (gSeparabilitySpecsPtr->tablesToList)"
														
					if (gSeparabilitySpecsPtr->tablesToList & 0x0004)
						{
								// " List grouping table with a threshold of %g\r"
								
						continueFlag = ListSpecifiedStringNumber (
												kFeatureSelectionStrID,
												IDS_FeatureSelection23,
												resultsFileStreamPtr, 
												gOutputForce1Code, 
												gSeparabilitySpecsPtr->distancesLessThanToGroup, 
												continueFlag);
						
						}	// end "if (gSeparabilitySpecsPtr->tablesToList)"
					
							// List number of contiguous channels per group.
							// " %hd contiguous channels in each channel combination group\r"

					continueFlag = ListSpecifiedStringNumber (
									kFeatureSelectionStrID,
									IDS_FeatureSelection24,
									resultsFileStreamPtr, 
									gOutputForce1Code, 
									(SInt32)gSeparabilitySpecsPtr->numberContiguousPerGroup, 
									continueFlag);
					
							// List the search algorithm to be used.	
							// " All possible channel combinations will be searched\r");
							// or
							// " Step search of channel combination groups will be used\r");					
					
					index	= IDS_FeatureSelection25;
					if (gSeparabilitySpecsPtr->featureSearchCode == 2)
						index	= IDS_FeatureSelection26;
						
					continueFlag = ListSpecifiedStringNumber (kFeatureSelectionStrID,
																			index, 
																			(unsigned char*)gTextString,
																			resultsFileStreamPtr, 
																			gOutputForce1Code,
																			continueFlag);
		
							// Insert a blank line
						
					continueFlag = OutputString (resultsFileStreamPtr, 
															(char*)gEndOfLine,
															0,
															gOutputForce1Code,
															continueFlag);
						
							// List the channels used.											
					
					if (continueFlag)
						{
						SInt16* featurePtr = (SInt16*)GetHandlePointer (
															gSeparabilitySpecsPtr->featureHandle);
												
						SInt16* channelsPtr = (SInt16*)GetHandlePointer (
															gSeparabilitySpecsPtr->channelsHandle);
												
						continueFlag = ListChannelsUsed (
												fileInfoPtr,
												featurePtr,
												channelsPtr,
												gSeparabilitySpecsPtr->numberFeatures, 
												resultsFileStreamPtr, 
												&gOutputForce1Code,
												gSeparabilitySpecsPtr->featureTransformationFlag);
						
						}	// end "if (continueFlag)"
								
							// List the classes used.											
		
					if (continueFlag)
						{
						UCharPtr symbolsPtr = (UCharPtr)GetHandlePointer (
															gSeparabilitySpecsPtr->symbolsHandle);
												
						continueFlag =  ListClassesUsed (
															NULL,
															fileInfoPtr,
															classPtr, 
															symbolsPtr,
															kListNoWeights,
															gSeparabilitySpecsPtr->numberClasses, 
															resultsFileStreamPtr, 
															&gOutputForce1Code,
															FALSE);
						
						}	// end "if (continueFlag)"
										
							// List "  Output Information:"
							
					continueFlag = ListSpecifiedStringNumber (kSharedStrID, 
																			IDS_Shared8,
																			(unsigned char*)gTextString,
																			resultsFileStreamPtr,
																			gOutputForce1Code,
																			continueFlag);
						
							// Get dialog box to display separability status.			
					
					if (continueFlag)
						gStatusDialogPtr = GetStatusDialog (kUpdateStatsInfoID, TRUE);
						
					continueFlag = (gStatusDialogPtr != NULL);
							
					LoadDItemStringNumber (kFeatureSelectionStrID, 
													IDS_FeatureSelection29,	// "\pSet"
													gStatusDialogPtr, 
													IDC_Status2, 
													(Str255*)gTextString);
					
					LoadDItemStringNumber (kFeatureSelectionStrID, 
													IDS_FeatureSelection30,	// "\pCombination"
													gStatusDialogPtr, 
													IDC_Status7, 
													(Str255*)gTextString);
					
					ShowStatusDialogItemSet (kStatusClassB);
					ShowStatusDialogItemSet (kStatusField);
					ShowStatusDialogItemSet (kStatusCommand);
					if (gStatusDialogPtr)
						DrawDialog (gStatusDialogPtr);
					
					startTime = time (NULL);
							
							// Initialize some global variables to allow the user 	
							// to cancel the operation at the end of the current 		
							// iteration instead of canceling immediately.				
							
					InitializeGlobalAlertVariables (kFinishDontCancelAlertID, 
																kAlertStrID, 
																IDS_Alert24);
									
					if (continueFlag)
						{
								// Turn spin cursor on
				
						gPresentCursor = kSpin;
							
						CalculateSeparabilityControl ();
						
								// List message if memory was not available to complete
								// the process
								
						ListMemoryMessage (resultsFileStreamPtr);

								// Turn spin cursor off
					
						gPresentCursor = kWait;
						
						}	// end "if (continueFlag)"
					
							// List the CPU time taken for the separability.			
							
					continueFlag = ListCPUTimeInformation (
													resultsFileStreamPtr, continueFlag, startTime);
												
							// Reset the cancel operation globals.
												
					ClearGlobalAlertVariables ();
			
							// Scroll output window to the selection and adjust the 	
							// scroll bar.															
					
					UpdateOutputWScrolls (gOutputWindow, 1, kDisplayMessage);
				
							// Dispose of status dialog box.									
					
					CloseStatusDialog (TRUE);
					
							// Change cursor back to pointer.								
								
					MInitCursor ();
						
					}	// end "if (continueFlag)" 
					
						// Close the separability results file if needed and			
						// release the memory assigned to it.								
						
				CloseResultsFiles ();
				
				}	// end "if (SeparabilityDialog (fileInfoPtr))" 
				
			}	// end "if (LoadSeparabilitySpecs ())" 
			
					// Unlock the memory for the separability specifications.		
		
		if (gSeparabilitySpecsPtr != NULL)
			{
			CheckAndUnlockHandle (gSeparabilitySpecsPtr->featureHandle);
			CheckAndUnlockHandle (gSeparabilitySpecsPtr->channelsHandle);
			CheckAndUnlockHandle (gSeparabilitySpecsPtr->classHandle);
			CheckAndUnlockHandle (gSeparabilitySpecsPtr->symbolsHandle);
			CheckAndUnlockHandle (gSeparabilitySpecsPtr->channelCombinationsHandle);
			CheckAndUnlockHandle (gSeparabilitySpecsPtr->allChanCombinationsHandle);
				
			CheckAndUnlockHandle (gProjectInfoPtr->classPairWeightsHandle);
			CheckAndUnlockHandle (gProjectInfoPtr->classPairWeightsListHandle);
			
			}	// end "if (gSeparabilitySpecsPtr != NULL" 
					
		CheckAndUnlockHandle (gProjectInfoPtr->separabilitySpecsH);
		gSeparabilitySpecsPtr = NULL;
		
		UnlockProjectWindowInfoHandles ();
		
		gOperationCanceledFlag = FALSE;
		
		}	// end "if (gProjectInfoPtr != NULL && ..." 

}	// end "SeparabilityControl" 


                                                                                
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean SeparabilityDialog
//
//	Software purpose:	The purpose of this routine is to handle the 		
//							modal dialog for selecting the separability
//							specifications. 
//
//	Parameters in:		image file information pointer
//
//	Parameters out:	None
//
// Value Returned:  	TRUE if user clicked OK.
//							FALSE if user clicked Cancel.
//
// Called By:			SeparabilityControl   in SFeatureSelection.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/25/1989
//	Revised By:			Larry L. Biehl			Date: 12/16/2016

Boolean SeparabilityDialog (
				FileInfoPtr							fileInfoPtr)

{   
	UInt16								*allChanCombinationsPtr = NULL;
	
	Boolean								returnFlag; 
	
			
	allChanCombinationsPtr = (UInt16*)GetHandlePointer (
												gSeparabilitySpecsPtr->allChanCombinationsHandle);
	
	#if defined multispec_mac
		Rect									theBox,
												theChannelPopupBox; 
									
		DialogPtr							dialogPtr;
		
		SInt16								*localClassPairWeightsListPtr = NULL; 
		
		UInt16								*localActiveFeaturesPtr = NULL,
												*localChannelCombinationsPtr = NULL,
												*localClassPtr = NULL,
												*localFeaturesPtr = NULL,
												*localTransformFeaturesPtr = NULL;

		unsigned char*						localSymbolsPtr = NULL;
										
		UserItemUPP							drawSeparabilityDistancePopUpPtr,
												drawChannelsPopUp2Ptr,
												drawChannelCombinationsPopUpPtr,
												drawInterClassWeightsPopUpPtr;
												
		Handle								okHandle,
												theHandle;
		
		SInt32								localCombinationsToList,
												maxContiguousChannelsPerGroup,
												numberClassCombinations,
												savedContiguousChannelsPerGroup,
												theNum;
												
		UInt32								localNumberClasses;
		
		SInt16								index,
												itemHit,
												itemHit2,
												localDefaultClassPairWeight,
												theType;
									
		UInt16								localNumberChannelGroupCombinations,
												localActiveNumberFeatures,
												localNumberFeatures,
												localNumberTransformFeatures,
												numberEigenvectors,
												savedNumberChannelGroupCombinations; 

		Boolean								checkFeatureTransformFlag,
												checkTextItemsFlag,
												diskFileFlag,
												featureTransformAllowedFlag,
												featureTransformationFlag,
												modalDone,
												savedSearchFlag,
												textWindowFlag,
												updateChannelCombinationFlag,
												updateChannelCombinationVectorFlag;
		
		
				// Initialize local variables.													
				
		dialogPtr = NULL;
					
		returnFlag = GetDialogLocalVectors (&localFeaturesPtr,
															&localTransformFeaturesPtr,
															&localClassPtr,
															NULL,		// &localClassAreaPtr,
															NULL,		// &localClassWeightsPtr,
															&localSymbolsPtr,
															&localChannelCombinationsPtr,
															&localClassPairWeightsListPtr);
		
				// Get the modal dialog for the separability specification.				
		
		if (returnFlag)			
			dialogPtr = LoadRequestedDialog (kSeparabilitySpecificationID, kCopyScrap, 1, 2);
				
		if (dialogPtr == NULL)
			{
			ReleaseDialogLocalVectors (localFeaturesPtr,
												localTransformFeaturesPtr,
												localClassPtr,
												NULL,			// localClassAreaPtr,
												NULL,			// localClassWeightsPtr,
												localSymbolsPtr,
												localChannelCombinationsPtr,
												localClassPairWeightsListPtr);											
																					return (FALSE);
																					
			}	// end "if (dialogPtr == NULL)" 
			
				// Intialize local user item proc pointers.									

		drawSeparabilityDistancePopUpPtr = NewUserItemUPP (DrawSeparabilityDistancePopUp);
		drawChannelsPopUp2Ptr = NewUserItemUPP (DrawChannelsPopUp2);
		drawChannelCombinationsPopUpPtr = NewUserItemUPP (DrawChannelCombinationsPopUp);
		drawInterClassWeightsPopUpPtr = NewUserItemUPP (DrawInterClassWeightsPopUp);
							
		//weightsListPtr = NULL;
		//if (gProjectInfoPtr->classPairWeightsListHandle != NULL)
		//	weightsListPtr = (UInt16*)GetHandlePointer (
		//				gProjectInfoPtr->classPairWeightsListHandle);
			
		updateChannelCombinationFlag = FALSE;
		updateChannelCombinationVectorFlag = FALSE;
		
				// Set Procedure pointers for those dialog items that need them.
				
		SetDialogItemDrawRoutine (dialogPtr, 5, drawSeparabilityDistancePopUpPtr);	
				
		SetDialogItemDrawRoutine (dialogPtr, 8, &theChannelPopupBox, drawChannelsPopUp2Ptr);		
				
		SetDialogItemDrawRoutine (dialogPtr, 11, drawChannelCombinationsPopUpPtr);
				
		SetDialogItemDrawRoutine (dialogPtr, 21, gDrawDialogClassPopUpPtr);											
				
		SetDialogItemDrawRoutine (dialogPtr, 25, drawInterClassWeightsPopUpPtr);																
				
		SetDialogItemDrawRoutine (dialogPtr, 27, gDrawSymbolsPopUpPtr);	
				
		SetDialogItemDrawRoutine (dialogPtr, 32, gHiliteOKButtonPtr);
		
				// Initialize dialog variables.

		SeparabilityDialogInitialize (dialogPtr,
													gSeparabilitySpecsPtr,
													localFeaturesPtr,
													localTransformFeaturesPtr,
													localClassPtr,
													localSymbolsPtr,
													localChannelCombinationsPtr,
													&localClassPairWeightsListPtr,
													&gSeparabilityDistance,
													&numberEigenvectors,
													&featureTransformAllowedFlag,
													&featureTransformationFlag,
													&gChannelSelection,
													&localActiveNumberFeatures,
													&gChannelCombinationSelection,
													&localNumberChannelGroupCombinations,
													&savedNumberChannelGroupCombinations,
													&savedContiguousChannelsPerGroup,
													&savedSearchFlag,
													&maxContiguousChannelsPerGroup,
													&localCombinationsToList,
													&gClassSelection,
													&localNumberClasses,
													&numberClassCombinations,
													&gInterClassWeightsSelection,
													&localDefaultClassPairWeight,
													&gSymbolSelection,
													&textWindowFlag,
													&diskFileFlag);
		
				// Save handle for the OK button for use later.								
				
		GetDialogItem (dialogPtr, 1, &theType, &okHandle, &theBox);
			
				// Set the check box for the feature transformation option.
		
		SetDLogControl (dialogPtr, 6, featureTransformationFlag);
		
				// Number of contiguous channels per group.									
				
		LoadDItemValue (dialogPtr, 13, savedContiguousChannelsPerGroup);
		
				// Feature search algorithm to use.												
		
		SetDLogControl (dialogPtr, 14, savedSearchFlag);
		
				// Current maximum number of channel combinations.	

		LoadDItemRealValue (dialogPtr, 
								IDC_MaxSearchCombinations, 
								(double)gSeparabilitySpecsPtr->maxNumberFeatureCombinations,
								0);
		
				// Number of channel combinations to list.									
				// For now the number of combinations to list will be the same as		
				// the number of channel combinations to store in memory.				
				// "gSeparabilitySpecsPtr->combinationsToStore".							
		
		LoadDItemValue (dialogPtr, 18, localCombinationsToList);
		
				// Current number of class combinations.										
		
		LoadDItemValue (dialogPtr, 22, gSeparabilitySpecsPtr->numberClassCombinations);
		
				// Separability results to text output window.								
		
		SetDLogControl (dialogPtr, 30, textWindowFlag);		
		
				// Separability results to disk file.											
				
		SetDLogControl (dialogPtr, 31, diskFileFlag);
		
				// Set feature/transform feature parameters  

		InitializeDialogFeatureParameters (featureTransformationFlag,
														localActiveNumberFeatures, 
														gProjectInfoPtr->numberStatisticsChannels, 
														gTransformationMatrix.numberFeatures,
														localFeaturesPtr,
														localTransformFeaturesPtr,
														&localNumberFeatures,
														&localNumberTransformFeatures,
														&localActiveFeaturesPtr);
														
		checkTextItemsFlag = FALSE;
			
				// Center the dialog and then show it.											
				
		ShowDialogWindow (dialogPtr, kSeparabilitySpecificationID, kSetUpDFilterTable);
		
				// Set default text selection to first edit text item						
				
		SelectDialogItemText (dialogPtr, 13, 0, INT16_MAX);
		
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
						// Get the handle to the itemHit.  										
						
				GetDialogItem (dialogPtr, itemHit, &theType, &theHandle, &theBox);
				if (theType == 16)
					{
					GetDialogItemText (theHandle, gTextString);	
					StringToNum (gTextString, &theNum);
					
					}	// end "if (theType == 16)" 
				
				switch (itemHit)
					{
					case 5:		// Separability distance measure.	
						itemHit = StandardPopUpMenu (dialogPtr, 
																4, 
																5, 
																gPopUpSeparabilityDistanceMenu, 
																gSeparabilityDistance, 
																kPopUpSeparabilityDistanceMenuID);
						
						if (itemHit != 0)
							gSeparabilityDistance = itemHit;
		
								// Make certain that the correct label is drawn in the	
								// class pop up box.													
						
						InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
						
						featureTransformAllowedFlag =
									SeparabilityDialogGetFeatureTransformAllowedFlag (
																			gSeparabilityDistance,
																			numberEigenvectors);	
																			
						checkFeatureTransformFlag = TRUE;
						
						if (featureTransformationFlag && !featureTransformAllowedFlag)
							{											
							checkTextItemsFlag = TRUE;
							updateChannelCombinationFlag = TRUE;
							updateChannelCombinationVectorFlag = TRUE;
							
							}	// end "if (featureTransformationFlag && ..."
						break;
							
					case 6:		// Feature transformation flag. 
						ChangeDLogCheckBox ((ControlHandle)theHandle);
						featureTransformationFlag = !featureTransformationFlag;
						checkFeatureTransformFlag = TRUE;
						checkTextItemsFlag = TRUE;
						updateChannelCombinationFlag = TRUE;
						updateChannelCombinationVectorFlag = TRUE;
						break;
						
					case 8:		// Channels or Features 
						itemHit = StandardPopUpMenu (dialogPtr, 
																7, 
																8, 
																gPopUpAllAvailableSubsetMenu, 
																gChannelSelection, 
																kPopUpAllAvailableSubsetMenuID);
														
						if (itemHit == kSubsetMenuItem)
							{
									// Subset of channels to be used.							
									
							HiliteControl ((ControlHandle)okHandle, 255);
							
							ChannelsDialog (
											(SInt16*)&localActiveNumberFeatures,
											(SInt16*)localActiveFeaturesPtr,
											gImageLayerInfoPtr,
											fileInfoPtr,
											1,
											featureTransformationFlag,
											(SInt16*)gProjectInfoPtr->channelsPtr,
											gProjectInfoPtr->numberStatisticsChannels,
											gChannelSelection);

							HiliteControl ((ControlHandle)okHandle, 0);
							
							}	// end "if (itemHit == kSubsetMenuItem)" 
						
						if (itemHit != 0)
							{
							gChannelSelection = UpdateDialogNumberFeatureParameters (
																featureTransformationFlag, 
																&localActiveNumberFeatures,
																&localNumberFeatures,
																gProjectInfoPtr->numberStatisticsChannels, 
																&localNumberTransformFeatures,
																gTransformationMatrix.numberFeatures,
																itemHit);
		
									// Make certain that the correct label is drawn in the	
									// channel pop up box.												
							
							InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
							
							updateChannelCombinationFlag = TRUE;
							updateChannelCombinationVectorFlag = TRUE;
							
							}	// end "if (itemHit != 0)"
						break;
						
					case 11:		// Channel combinations.	
						itemHit = StandardPopUpMenu (dialogPtr, 
																10, 
																11, 
																gPopUpAllPossibleSubsetMenu, 
																gChannelCombinationSelection, 
																kPopUpAllPossibleSubsetMenuID);
														
						if (itemHit == kSubsetMenuItem)
							{
									// Subset of channel combinations to be used.			
									
							HiliteControl ((ControlHandle)okHandle, 255);
							
							ChannelCombinationsDialog (
									&localNumberChannelGroupCombinations,
									localChannelCombinationsPtr,
									(UInt32)gSeparabilitySpecsPtr->numberAllChanCombinations,
									allChanCombinationsPtr,
									savedContiguousChannelsPerGroup,
									featureTransformationFlag);

							HiliteControl ((ControlHandle)okHandle, 0);
											
							if (localNumberChannelGroupCombinations ==
											gSeparabilitySpecsPtr->numberAllChanCombinations)
								itemHit = kAllMenuItem;
								
							}	// end "if (itemHit == kSubsetMenuItem)" 
						
						if (itemHit != 0)
							gChannelCombinationSelection = itemHit;
		
								// Make certain that the correct label is drawn in the	
								// class pop up box.													
						
						InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
						
						updateChannelCombinationFlag = TRUE;
						break;
						
					case 13:		// Number of contiguous channels per group.				
						if (theNum == 0 || theNum > maxContiguousChannelsPerGroup)	
							NumberErrorAlert ((SInt32)savedContiguousChannelsPerGroup,
														dialogPtr, 
														itemHit);
																
						else	// theNum > 0 || ... 
							{
							if (savedContiguousChannelsPerGroup != theNum)
								{
								savedContiguousChannelsPerGroup = theNum;
									
								updateChannelCombinationFlag = TRUE;
								updateChannelCombinationVectorFlag = TRUE;
								checkTextItemsFlag = TRUE;
								
								}	// end "if (savedContiguousChannelsPerGroup != ...)" 
							
							}	// end "else theNum > 0 || ..."
						break;
						
					case 14:		// Step feature search.			
						ChangeDLogCheckBox ((ControlHandle)theHandle);
						savedSearchFlag = !savedSearchFlag;
						updateChannelCombinationFlag = TRUE;
						updateChannelCombinationVectorFlag = TRUE;
						break;
						
					case 18:		// Number of combinations to list.							
						if (theNum <= 0 || theNum > SInt16_MAX) 
							NumberErrorAlert (localCombinationsToList, dialogPtr, itemHit);
																
						else	// theNum > 0 || ... 
							localCombinationsToList = theNum;
						break;
						
					case 21:		// Classes 
						itemHit = StandardPopUpMenu (dialogPtr, 
																20, 
																21, 
																gPopUpAllSubsetMenu, 
																gClassSelection, 
																kPopUpAllSubsetMenuID);
														
						if (itemHit == kSubsetMenuItem)
							{
									// Subset of classes to be used.	
							
							itemHit = ClassDialog (
										&localNumberClasses,
										(SInt16*)localClassPtr, 
										2,
										gProjectInfoPtr->numberStatisticsClasses,
										gClassSelection,
										okHandle);
								
							}	// end "if (itemHit == kSubsetMenuItem)" 
						
						if (itemHit != 0)
							gClassSelection = itemHit;
		
								// Make certain that the correct label is drawn in the	
								// class pop up box.													
						
						InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
						
								// Update the number of class combinations.					
						
						SeparabilityDialogUpdateNumberClassCombintations (
															dialogPtr,
															gClassSelection,
															&localNumberClasses,
															localClassPtr);
						break;
						
					case 25:		// Weights 
						itemHit = StandardPopUpMenu (dialogPtr, 
																24, 
																25, 
																gPopUpWeightsMenu, 
																gInterClassWeightsSelection, 
																kPopUpWeightsMenuID);
														
						if (itemHit == kUnequalWeightMenuItem)
							{
									// Unequal weights to be used.								
									
							HiliteControl ((ControlHandle)okHandle, 255);
							
							gInterClassWeightsSelection = 
										ClassPairWeightsDialog (
												gProjectInfoPtr->numberStatisticsClasses,
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
						
					case 27:		// Symbol selection. 
						itemHit2 = StandardPopUpMenu (dialogPtr, 
																26, 
																27, 
																gPopUpSymbolsMenu, 
																gSymbolSelection, 
																kPopUpSymbolsMenuID);
														
						if (itemHit2 == kUserMenuItem)
							{
									// User specified symbols to be used.						
									
							HiliteControl ((ControlHandle)okHandle, 255);
							
							if (!SymbolsDialog (localNumberClasses,
																(SInt16*)localClassPtr,
																(unsigned char*)localSymbolsPtr,
																FALSE))
								itemHit2 = gSymbolSelection;

							HiliteControl ((ControlHandle)okHandle, 0);
								
							}	// end "if (itemHit2 == kUserMenuItem)" 
						
						if (itemHit2 != 0)
							gSymbolSelection = itemHit2;
		
								// Make certain that the correct label is drawn in the	
								// class pop up box.													
						
						InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
						break;
						
					case 28:
								// Update the combinations to be listed.
																		
						HiliteControl ((ControlHandle)okHandle, 255);
						
						SeparabilityListDialog (&localCombinationsToList);
						
						HiliteControl ((ControlHandle)okHandle, 0);
						
						LoadDItemValue (dialogPtr, 18, localCombinationsToList);
						break;
						
					case 30:		// Write separability results to output window 
					case 31:		// Write separability results to disk file 
						ChangeDLogCheckBox ((ControlHandle)theHandle);
						break;
							
					}	// end "switch (itemHit)" 
					
				if (checkFeatureTransformFlag)
					{
					SeparabilityDialogCheckFeatureTransformation (
																			dialogPtr,
																			&featureTransformationFlag,
																			&localActiveNumberFeatures,
																			&localActiveFeaturesPtr,
																			featureTransformAllowedFlag,
																			6, 
																			7,
																			&gChannelSelection,
																			localNumberFeatures,
																			localFeaturesPtr, 
																			localNumberTransformFeatures,
																			localTransformFeaturesPtr);
					
							// Force the channel popup box to be updated.
							
					InvalWindowRect (GetDialogWindow (dialogPtr), &theChannelPopupBox);
																			
					checkFeatureTransformFlag = FALSE;
																
					}	// end "if (checkFeatureTransformFlag)"
				
				if (updateChannelCombinationFlag)
					{
					SeparabilityDialogUpdateChannelCombinationItems (
															dialogPtr,
															gSeparabilitySpecsPtr,
															updateChannelCombinationVectorFlag,
															localActiveNumberFeatures,
															allChanCombinationsPtr,
															savedContiguousChannelsPerGroup,
															savedSearchFlag,
															gChannelCombinationSelection,
															localChannelCombinationsPtr,
															&localNumberChannelGroupCombinations,
															&savedNumberChannelGroupCombinations,
															&localCombinationsToList,
															&maxContiguousChannelsPerGroup);
															
					updateChannelCombinationFlag = FALSE;
					updateChannelCombinationVectorFlag = FALSE;
									
					}	// end "if (updateChannelCombinationFlag) " 

				if (checkTextItemsFlag)
					{
					SeparabilityDialogUpdateChannelFeatureGroupText (
									dialogPtr, 
									savedContiguousChannelsPerGroup, 
									featureTransformationFlag);
					checkTextItemsFlag = FALSE;
					
					}	// end "if (checkTextItemsFlag)" 
					
				}	// end "if (itemHit > 2)" 
				
			else if (itemHit > 0) 	// and itemHit <= 2 
				{
				if (itemHit == 1 &&
								gSeparabilitySpecsPtr->maxNumberFeatureCombinations <= 0)
					{
					index = IDS_FeatureSelection11;
					
					if (featureTransformationFlag)
						index = IDS_FeatureSelection12;
					
					if (savedContiguousChannelsPerGroup > 1)
						index = IDS_FeatureSelection13;
						
					HiliteControl ((ControlHandle)okHandle, 255);
					
					itemHit = DisplayAlert (
									kErrorAlertID, 1, kFeatureSelectionStrID, index, 0, NULL);
					itemHit = 0;
					
					HiliteControl ((ControlHandle)okHandle, 0);
					
					}	// end "if (itemHit == 1 && ..." 

				if	(itemHit == 1)      // User selected OK for information 
					{
					modalDone = TRUE;
					returnFlag = TRUE;
									
					SeparabilityDialogOK (gSeparabilitySpecsPtr,
													gSeparabilityDistance,
													featureTransformationFlag,
													gChannelSelection,
													localActiveNumberFeatures,
													localActiveFeaturesPtr,
													gChannelCombinationSelection,
													localNumberChannelGroupCombinations,
													localChannelCombinationsPtr,
													savedContiguousChannelsPerGroup,
													savedSearchFlag,
													GetDItemValue (dialogPtr, 18),
													gClassSelection,
													localNumberClasses,
													localClassPtr,
													gInterClassWeightsSelection,
													localClassPairWeightsListPtr,
													localDefaultClassPairWeight,
													gSymbolSelection,
													localSymbolsPtr,
													(GetDLogControl (dialogPtr, 30) == 1),
													(GetDLogControl (dialogPtr, 31) == 1));
																	
					}	// end "if (itemHit == 1)" 
					
				if	(itemHit == 2)      // User selected Cancel for information 
					{
					modalDone = TRUE;
					returnFlag = FALSE;
					
					}	// end "if	(itemHit == 2)"
				
				}	// end "else if (itemHit > 0) and itemHit <= 2" 
					
			} while (!modalDone);	
		
		DisposeUserItemUPP (drawSeparabilityDistancePopUpPtr);
		DisposeUserItemUPP (drawChannelsPopUp2Ptr);
		DisposeUserItemUPP (drawChannelCombinationsPopUpPtr);
		DisposeUserItemUPP (drawInterClassWeightsPopUpPtr);	
			
		ReleaseDialogLocalVectors (localFeaturesPtr,
											localTransformFeaturesPtr,
											localClassPtr,
											NULL,			// localClassAreaPtr,
											NULL,			// localClassWeightsPtr,
											localSymbolsPtr,
											localChannelCombinationsPtr,
											localClassPairWeightsListPtr);
			
		CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
	#endif	// defined multispec_mac

	#if defined multispec_win   
		CMFeatureSelectionDialog*		dialogPtr = NULL;
		
		TRY
			{ 
			dialogPtr = new CMFeatureSelectionDialog ();
			
			returnFlag = dialogPtr->DoDialog (gSeparabilitySpecsPtr,
															allChanCombinationsPtr); 
		                       
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
		CMFeatureSelectionDialog* dialogPtr = NULL;

		try 
			{
			dialogPtr = new CMFeatureSelectionDialog ();
			returnFlag = dialogPtr->DoDialog (gSeparabilitySpecsPtr, allChanCombinationsPtr);
			delete dialogPtr;
			
			} 
			  
		catch (int e) 
			{
			MemoryMessage (0, kObjectMessage);
			returnFlag = FALSE;
			
			}
	#endif
	
	return (returnFlag);
	
}	// end "SeparabilityDialog"  



Boolean SeparabilityDialogGetFeatureTransformAllowedFlag (
				SInt16								separabilityDistance,
				UInt16								numberEigenvectors)
	
{								
	Boolean featureTransformAllowedFlag = 
					((separabilityDistance != kNonCovarianceBhattacharyya) && 
																	(numberEigenvectors > 0));
																	
	return (featureTransformAllowedFlag);
																	
}	// end "SeparabilityDialogGetFeatureTransformAllowedFlag"



void SeparabilityDialogInitialize (
				DialogPtr							dialogPtr,
				SeparabilitySpecsPtr				separabilitySpecsPtr,
				UInt16*								localFeaturesPtr,
				UInt16*								transformFeaturesPtr,
				UInt16*								localClassPtr,
				unsigned char*						localSymbolPtr,
				UInt16*								localChannelCombinationsPtr,
				SInt16**								localClassPairWeightsListPtrPtr,
				SInt16*								separabilityDistancePtr,
				UInt16*								numberEigenvectorsPtr,
				Boolean*								featureTransformAllowedFlagPtr,
				Boolean*								featureTransformationFlagPtr,
				SInt16*								channelSelectionPtr,
				UInt16*								localNumberFeaturesPtr,
				SInt16*								channelCombinationSelectionPtr,
				UInt16*								numberChannelGroupCombinationsPtr, 
				UInt16*								savedNumberChannelGroupCombinationsPtr,
				SInt32*								savedContiguousChannelsPerGroupPtr,
				Boolean*								savedSearchFlagPtr,
				SInt32*								maxContiguousChannelsPerGroupPtr,
				SInt32*								localCombinationsToListPtr,
				SInt16*								classSelectionPtr,
				UInt32*								localNumberClassesPtr,
				SInt32*								numberClassCombinationsPtr,
				SInt16*								interClassWeightsSelectionPtr,
				SInt16*								defaultClassPairWeightPtr,
				SInt16*								symbolSelectionPtr,
				Boolean*								textWindowFlagPtr,
				Boolean*								diskFileFlagPtr)

{
	SInt16								eigenSource;
	
		
			// Load the dialog local vectors

	LoadDialogLocalVectors (localFeaturesPtr,
										transformFeaturesPtr,
										separabilitySpecsPtr->featureHandle,
										separabilitySpecsPtr->numberChannels,
										localClassPtr,
										separabilitySpecsPtr->classHandle,
										separabilitySpecsPtr->numberClasses,
										NULL,		// classArea
										NULL,
										0,
										localSymbolPtr,		// symbol
										separabilitySpecsPtr->symbolsHandle,
										separabilitySpecsPtr->numberClasses,
										localChannelCombinationsPtr,
										separabilitySpecsPtr->channelCombinationsHandle,
										separabilitySpecsPtr->numberChannelGroupCombinations,
										localClassPairWeightsListPtrPtr);
	
			// Distance measure.												
			//	Get the rectangles for the distance measure prompt and popup 		
			// boxes. 																				
			
	*separabilityDistancePtr = separabilitySpecsPtr->distanceMeasure;
	
	#if defined multispec_win                                            
		CComboBox* comboBoxPtr = 
					(CComboBox*)dialogPtr->GetDlgItem (IDC_DistanceMeasureCombo);
		
		comboBoxPtr->SetItemData (0, kBhattacharyya);
		comboBoxPtr->SetItemData (1, kEFBhattacharyya);
		comboBoxPtr->SetItemData (2, kMeanBhattacharyya);
		comboBoxPtr->SetItemData (3, kCovarianceBhattacharyya);
		comboBoxPtr->SetItemData (4, kNonCovarianceBhattacharyya);
		comboBoxPtr->SetItemData (5, kTransformedDivergence);
		comboBoxPtr->SetItemData (6, kDivergence);
	#endif	// defined multispec_win  		 				                 

	#if defined multispec_wx                                           
		wxChoice* distanceMeasureCtrl =
								(wxChoice*)dialogPtr->FindWindow (IDC_DistanceMeasureCombo);
		distanceMeasureCtrl->SetClientData (0, (void*)kBhattacharyya);
		distanceMeasureCtrl->SetClientData (1, (void*)kEFBhattacharyya);
		distanceMeasureCtrl->SetClientData (2, (void*)kMeanBhattacharyya);
		distanceMeasureCtrl->SetClientData (3, (void*)kCovarianceBhattacharyya);
		distanceMeasureCtrl->SetClientData (4, (void*)kNonCovarianceBhattacharyya);
		distanceMeasureCtrl->SetClientData (5, (void*)kTransformedDivergence);
		distanceMeasureCtrl->SetClientData (6, (void*)kDivergence);
	#endif		                 
	
	if (gProjectInfoPtr->statisticsCode != kMeanCovariance) 
		{
		#if defined multispec_mac 
			DisableMenuItem (gPopUpSeparabilityDistanceMenu, kBhattacharyya);
			DisableMenuItem (gPopUpSeparabilityDistanceMenu, kEFBhattacharyya);
			DisableMenuItem (gPopUpSeparabilityDistanceMenu, kMeanBhattacharyya);
			DisableMenuItem (gPopUpSeparabilityDistanceMenu, kCovarianceBhattacharyya);
			DisableMenuItem (gPopUpSeparabilityDistanceMenu, kTransformedDivergence);
			DisableMenuItem (gPopUpSeparabilityDistanceMenu, kDivergence);
		#endif	// defined multispec_mac  
		
		#if defined multispec_win 
			comboBoxPtr->DeleteString (kDivergence-1);
			comboBoxPtr->DeleteString (kTransformedDivergence-1);
			comboBoxPtr->DeleteString (kCovarianceBhattacharyya-1);
			comboBoxPtr->DeleteString (kMeanBhattacharyya-1);
			comboBoxPtr->DeleteString (kEFBhattacharyya-1);
			comboBoxPtr->DeleteString (kBhattacharyya-1);
		#endif	// defined multispec_win  

		#if defined multispec_wx
			distanceMeasureCtrl->Delete (kDivergence - 1);
			distanceMeasureCtrl->Delete (kTransformedDivergence - 1);
			distanceMeasureCtrl->Delete (kCovarianceBhattacharyya - 1);
			distanceMeasureCtrl->Delete (kMeanBhattacharyya - 1);
			distanceMeasureCtrl->Delete (kEFBhattacharyya - 1);
			distanceMeasureCtrl->Delete (kBhattacharyya - 1);
		#endif	
		
		}	// end "if (...->statisticsCode != kMeanCovariance)" 
		
			// Set the check box for the feature transformation option.				
	
	EigenvectorInfoExists (kProject, &eigenSource, numberEigenvectorsPtr);
					
	*featureTransformAllowedFlagPtr =
								SeparabilityDialogGetFeatureTransformAllowedFlag (
																		*separabilityDistancePtr,
																		*numberEigenvectorsPtr);
																		
	if (*numberEigenvectorsPtr <= 0 || gProjectInfoPtr->statisticsCode != kMeanCovariance)
		{		
		*featureTransformationFlagPtr = FALSE;
		HideDialogItem (dialogPtr, IDC_FeatureTransformation);
		
		}	// end "if (numberEigenvectors <= 0 || ...)" 
		
	else	// numberEigenvectors > 0 && ... 
		*featureTransformationFlagPtr = separabilitySpecsPtr->featureTransformationFlag;
									
	if (*featureTransformAllowedFlagPtr)
		SetDLogControlHilite (dialogPtr, IDC_FeatureTransformation, 0);
		
	else	// !featureTransformAllowedFlag 
		SetDLogControlHilite (dialogPtr, IDC_FeatureTransformation, 255);
		
	if (*featureTransformationFlagPtr)
		LoadDItemStringNumber (kDialogStrID, 
										IDS_Dialog16,		// "\pFeatures:"
										dialogPtr, 
										IDC_ChannelPrompt, 
										(Str255*)gTextString);
	
			// Channels to consider. 															
			//	Get the rectangles for the channel prompt and popup boxes.			
			// Draw the default channel selection.											
			
	*channelSelectionPtr = separabilitySpecsPtr->channelSet;
	*localNumberFeaturesPtr = separabilitySpecsPtr->numberFeatures;
	
			// Channel combinations to use.													
			//	Get the rectangles for the channel combinations prompt and popup 	
			// boxes.  Draw the default channel combination selection.				
			
	*channelCombinationSelectionPtr =
										separabilitySpecsPtr->channelCombinationSet;
	*numberChannelGroupCombinationsPtr = 
									separabilitySpecsPtr->numberChannelGroupCombinations;
	*savedNumberChannelGroupCombinationsPtr = *numberChannelGroupCombinationsPtr;
	
			// Number of contiguous channels per group.									
			
	*savedContiguousChannelsPerGroupPtr = 
										separabilitySpecsPtr->numberContiguousPerGroup;
	
			// Feature search algorithm to use.												
	
	*savedSearchFlagPtr = (separabilitySpecsPtr->featureSearchCode == 2);
		
			// Update maximum channels per group parameter.
			
	SeparabilityDialogUpdateMaxChannelsPerGroup (
														separabilitySpecsPtr->numberFeatures,
														localChannelCombinationsPtr,
														numberChannelGroupCombinationsPtr,
														maxContiguousChannelsPerGroupPtr);
	
			// Number of channel combinations to list.									
			// For now the number of combinations to list will be the same as		
			// the number of channel combinations to store in memory.				
			// "gSeparabilitySpecsPtr->combinationsToStore".							
	
	*localCombinationsToListPtr = separabilitySpecsPtr->numberCombinationsToList;
	
			// Classes to use.																	
			//	Get the rectangles for the class prompt and popup boxes.				
			//	Make all classes the default													
			
	*classSelectionPtr = separabilitySpecsPtr->classSet;
	*localNumberClassesPtr = separabilitySpecsPtr->numberClasses;
	
	*numberClassCombinationsPtr = separabilitySpecsPtr->numberClassCombinations;
	
			// Class pair weights.																
			//	Make equal weights the default												
			
	*interClassWeightsSelectionPtr = gProjectInfoPtr->classPairWeightSet;
	*defaultClassPairWeightPtr = gProjectInfoPtr->defaultClassPairWeight;
	
			// Symbols to use.																	
			
	*symbolSelectionPtr = separabilitySpecsPtr->symbolSet;
	
			// Separability results to text output window and or disk file.
	
	*textWindowFlagPtr = FALSE;	
	if (separabilitySpecsPtr->outputStorageType & 0x0001)	
		*textWindowFlagPtr = TRUE;	
		
	*diskFileFlagPtr = FALSE;	         
	if (separabilitySpecsPtr->outputStorageType & 0x0002) 
		*diskFileFlagPtr = TRUE;		
	
			// Update some text items in the dialog relative to channels, 			
			// features and groups.																

	SeparabilityDialogUpdateChannelFeatureGroupText (
				dialogPtr, 
				(SInt16)*savedContiguousChannelsPerGroupPtr, 
				*featureTransformationFlagPtr);
	
}	// end "SeparabilityDialogInitialize" 



void SeparabilityDialogOK (
				SeparabilitySpecsPtr				separabilitySpecsPtr,
				SInt16								separabilityDistance,
				Boolean								featureTransformationFlag,
				SInt16								channelSelection,
				UInt16								localNumberFeatures,
				UInt16*								localFeaturesPtr,
				SInt16								channelCombinationSelection,
				UInt16								localNumberChannelGroupCombinations,
				UInt16*								localChannelCombinationsPtr,
				SInt32								contiguousChannelsPerGroup,
				Boolean								searchFlag,
				SInt32								numberCombinationsToList,
				SInt16								classSelection,
				UInt32								localNumberClasses,
				UInt16*								localClassPtr,
				SInt16								interClassWeightsSelection,
				SInt16*								localClassPairWeightsListPtr,
				SInt16								localDefaultClassPairWeight,
				SInt16								symbolSelection,
				unsigned char*						localSymbolsPtr,
				Boolean								outputTextWindowFlag,
				Boolean								diskFileFlag)

{
	UInt32								index;
	
					
			// Separability algorithm option.									
			
	separabilitySpecsPtr->distanceMeasure = separabilityDistance;
				
			// Feature transformation option.									
			
	separabilitySpecsPtr->featureTransformationFlag = 
														featureTransformationFlag;	
														
			// Load some common processor parameters
			// Channels
			// Classes	
			// Class symbols	
			
	LoadProcessorVectorsFromDialogLocalVectors (
											channelSelection,
											featureTransformationFlag,
											gProjectInfoPtr->numberStatisticsChannels,
											localNumberFeatures,
											localFeaturesPtr,
											&separabilitySpecsPtr->channelSet,
											(UInt16*)&separabilitySpecsPtr->numberFeatures,
											separabilitySpecsPtr->featureHandle,
											&separabilitySpecsPtr->numberChannels,
											separabilitySpecsPtr->channelsHandle,
											classSelection,
											localNumberClasses,
											localClassPtr,
											&separabilitySpecsPtr->classSet,
											&separabilitySpecsPtr->numberClasses,
											separabilitySpecsPtr->classHandle,
											symbolSelection,
											localSymbolsPtr,
											&separabilitySpecsPtr->symbolSet,
											separabilitySpecsPtr->symbolsHandle,
											interClassWeightsSelection,
											localClassPairWeightsListPtr,
											&gProjectInfoPtr->classPairWeightSet);
	
			// Channel combinations.													

	separabilitySpecsPtr->channelCombinationSet = channelCombinationSelection;	
													
	separabilitySpecsPtr->numberChannelGroupCombinations = 
													localNumberChannelGroupCombinations;

	UInt16* channelCombinationsPtr = (UInt16*)GetHandlePointer (
									separabilitySpecsPtr->channelCombinationsHandle);
												
	for (index=0; index<localNumberChannelGroupCombinations; index++)
		channelCombinationsPtr[index] = localChannelCombinationsPtr[index];											
		
			// Number of contiguous channel groups.																			
			
	separabilitySpecsPtr->numberContiguousPerGroup = (SInt16)contiguousChannelsPerGroup;
	
			// Feature search algorithm.											
			
	separabilitySpecsPtr->featureSearchCode = 1;
	if (searchFlag)
		separabilitySpecsPtr->featureSearchCode = 2;
		
			// Combinations to list.												
			
	separabilitySpecsPtr->numberCombinationsToList = numberCombinationsToList;
	
			// Get the maximum number of combinations to store.		
	
	separabilitySpecsPtr->maxCombinationsToStore	= MIN (SInt16_MAX, 
											separabilitySpecsPtr->maxNumberFeatureCombinations);
																
	if (searchFlag)
		separabilitySpecsPtr->maxCombinationsToStore =  1 + localNumberFeatures - 
								localChannelCombinationsPtr[0] * contiguousChannelsPerGroup;
									
	separabilitySpecsPtr->combinationsToStore =  MIN (numberCombinationsToList, 
													separabilitySpecsPtr->maxCombinationsToStore);
													
			// Default class pair weight.
			
	gProjectInfoPtr->defaultClassPairWeight = localDefaultClassPairWeight;
	
			// Write separability results to output window.					
			
	separabilitySpecsPtr->outputStorageType = 0;							
	if (outputTextWindowFlag)				
		separabilitySpecsPtr->outputStorageType += 1;
		
			// Write separability results to disk file.						
									
	if (diskFileFlag)				
		separabilitySpecsPtr->outputStorageType += 2;
	
}	// end "SeparabilityDialogOK"



void SeparabilityDialogUpdateChannelCombinationItems (
				DialogPtr							dialogPtr,
				SeparabilitySpecsPtr				separabilitySpecsPtr,
				Boolean								updateChannelCombinationVectorFlag,
				UInt16								numberFeatures,
				UInt16*								allChanCombinationsPtr,
				SInt32								contiguousChannelsPerGroup,
				Boolean								searchFlag,
				SInt16								channelCombinationSelection,
				UInt16*								channelCombinationsPtr,
				UInt16*								numberChannelGroupCombinationsPtr,
				UInt16*								savedNumberChannelGroupCombinationsPtr,
				SInt32*								combinationsToListPtr,
				SInt32*								maxContiguousChannelsPerGroupPtr)

{	
	SInt16								numberCombinationReturnCode;
	
									
			// Update the all possible channels combinations vector.															
					
	if (updateChannelCombinationVectorFlag)	
		separabilitySpecsPtr->numberAllChanCombinations =
				(SInt16)LoadAllChannelCombinationsVector (
											numberFeatures, 
											allChanCombinationsPtr,
											&separabilitySpecsPtr->maxNumberAllChanCombinations,
											(SInt16)contiguousChannelsPerGroup,
											searchFlag);
							
			// If all possible combinations to be used, make certain		
			// that all combinations are loaded in.							
			
	if (channelCombinationSelection == kAllMenuItem)
		LoadChannelCombinationsVector (
								separabilitySpecsPtr,
								channelCombinationsPtr, 
								allChanCombinationsPtr,
								numberChannelGroupCombinationsPtr, 
								(UInt32*)&separabilitySpecsPtr->maxNumberFeatureCombinations);
	
	else	// gChannelCombinationSelection != kAllMenuItem
		{
		if (*numberChannelGroupCombinationsPtr == 0)
			*numberChannelGroupCombinationsPtr = *savedNumberChannelGroupCombinationsPtr;
			
				// Update the max number of channel combinations.	
							 		
		separabilitySpecsPtr->maxNumberFeatureCombinations = 
														GetMaxCombinationSelected (
																*numberChannelGroupCombinationsPtr, 
																(SInt16*)channelCombinationsPtr, 
																numberFeatures,
																contiguousChannelsPerGroup,
																searchFlag,
																&numberCombinationReturnCode);
						
		if (numberCombinationReturnCode < 0)
			{
			if (*numberChannelGroupCombinationsPtr > 0)
				*savedNumberChannelGroupCombinationsPtr = *numberChannelGroupCombinationsPtr;
				
			*numberChannelGroupCombinationsPtr = 0;
			
			separabilitySpecsPtr->maxNumberFeatureCombinations = 0;
			
			}	// end "if (numberCombinationReturnCode < 0)"
			
		}	// end "else gChannelCombinationSelection != kAllMenuItem"

	LoadDItemRealValue (dialogPtr, 
							IDC_MaxSearchCombinations, 
							(double)separabilitySpecsPtr->maxNumberFeatureCombinations,
							0);
		
	SeparabilityDialogUpdateMaxChannelsPerGroup (numberFeatures,
																channelCombinationsPtr,
																numberChannelGroupCombinationsPtr,
																maxContiguousChannelsPerGroupPtr);
	
}	// end "SeparabilityDialogUpdateChannelCombinationItems"

 

void SeparabilityDialogUpdateMaxChannelsPerGroup (
				UInt16								numberFeatures,
				UInt16*								channelCombinationsPtr,
				UInt16*								numberChannelGroupCombinationsPtr,
				SInt32*								maxContiguousChannelsPerGroupPtr)

{	

			// Update max number of channels per group.
			
	*maxContiguousChannelsPerGroupPtr = numberFeatures;
	if (*numberChannelGroupCombinationsPtr > 0)
		*maxContiguousChannelsPerGroupPtr = *maxContiguousChannelsPerGroupPtr /
								channelCombinationsPtr[*numberChannelGroupCombinationsPtr-1];
								
	*maxContiguousChannelsPerGroupPtr = MAX (1, *maxContiguousChannelsPerGroupPtr);
		
}	// end "SeparabilityDialogUpdateMaxChannelsPerGroup" 


                                                              
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SeparabilityDialogUpdateChannelFeatureGroupText
//
//	Software purpose:	The purpose of this routine is to update the text
//							in the respective dialog items to reflect whether
//							channels, features or groups are being used.
//
//	Parameters in:					
//
//	Parameters out:	None
//
// Value Returned:	FALSE indicating the no more changes need to be made.
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 02/11/1995
//	Revised By:			Larry L. Biehl			Date: 04/24/1998	

void SeparabilityDialogUpdateChannelFeatureGroupText (
				DialogPtr							dialogPtr, 
				SInt16								contiguousChannelsPerGroup,
				Boolean								featureTransformationFlag)

{	
	if (contiguousChannelsPerGroup == 1)
		{
		if (featureTransformationFlag)
			CtoPstring ((UCharPtr)"Number of features:", gTextString);
			
		else	// !featureTransformationFlag 
			CtoPstring ((UCharPtr)"Number of channels:", gTextString);
			
		}	// end "if (contiguousChannelsPerGroup == 1)" 
		
	else	// contiguousChannelsPerGroup > 1 
		CtoPstring ((UCharPtr)"Number of groups:", gTextString);
		
	LoadDItemString (dialogPtr, IDC_NumberChannelsPrompt, (Str255*)gTextString);
		
	if (featureTransformationFlag)
		CtoPstring ((UCharPtr)"Contiguous features per group:", gTextString);
		
	else	// !featureTransformationFlag 
		CtoPstring ((UCharPtr)"Contiguous channels per group:", gTextString);
		
	LoadDItemString (dialogPtr, IDC_ChannelsPerGroupPrompt, (Str255*)gTextString);
		
}	// end "SeparabilityDialogUpdateChannelFeatureGroupText" 



void SeparabilityDialogUpdateNumberClassCombintations (
				DialogPtr							dialogPtr,
				SInt16								classSelection,
				UInt32*								numberClassesPtr,
				UInt16*								classPtr)
				
{  
	SInt32							numberClassCombinations;
	
	SInt16							numberPairReturnCode;
	
					
			// Update the number of class combinations.					
					                       
					
	if (classSelection == kAllMenuItem)
					// Make sure that the class settings are setup for all
					// classes in case they will be used by the symbols
					// or weights popup menu.
					
		LoadTrainClassVector (numberClassesPtr,
										&classSelection, 
										(SInt16*)classPtr);
						
	numberClassCombinations = GetNumberOfCombinations (*numberClassesPtr, 
																			2, 
																			1, 
																			FALSE,
																			&numberPairReturnCode);
						
	LoadDItemValue (dialogPtr, IDC_NumberClassCombinations, numberClassCombinations);
	
}	// end "SeparabilityDialogUpdateNumberClassCombintations" 



void SeparabilityDialogCheckFeatureTransformation (
				DialogPtr							dialogPtr,
				Boolean*								featureTransformationFlagPtr,
				UInt16*								localActiveNumberFeaturesPtr,
				UInt16**								localActiveFeaturePtrPtr,
				Boolean								featureTransformAllowedFlag,
				SInt16								featureTransformItem, 
				SInt16								channelFeatureItem,
				SInt16*								channelSelectionPtr, 
				SInt16								localNumberFeatures,
				UInt16*								localFeaturesPtr, 
				SInt16								localNumberTransformedFeatures,
				UInt16*								localTransformFeaturesPtr)
				
{  
	CheckFeatureTransformationDialog (dialogPtr,
													featureTransformAllowedFlag,
													featureTransformItem, 
													channelFeatureItem, 
													featureTransformationFlagPtr); 

	*channelSelectionPtr = UpdateDialogFeatureParameters (
												*featureTransformationFlagPtr, 
												localActiveNumberFeaturesPtr,
												localActiveFeaturePtrPtr, 
												localNumberFeatures,
												localFeaturesPtr,
												gProjectInfoPtr->numberStatisticsChannels, 
												localNumberTransformedFeatures,
												localTransformFeaturesPtr,
												gTransformationMatrix.numberFeatures);
												
}	// end "SeparabilityDialogCheckFeatureTransformation" 
		

                                                                                
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SeparabilityListDialog
//
//	Software purpose:	The purpose of this routine is to handle the 		
//							modal dialog for selecting the separability
//							list specifications. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			SeparabilityDialog   in SFeatureSelection.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/25/1989
//	Revised By:			Larry L. Biehl			Date: 05/06/1998

void SeparabilityListDialog (
				SInt32*								combinationsToListPtr)

{
	#if defined multispec_mac
		Rect									theBox; 
		
		DialogPtr							dialogPtr;
		
		double								localDistancesLessThanToGroup,
												localDistancesLessThanToList,
												localMinDistanceRangeToList,
												localMaxDistanceRangeToList,
												realValue; 
									
		Handle								okHandle,
												theHandle;
		
		SInt32								localCombinationsToList,
												theNum; 
		
		SInt16								itemHit,
												itemNumber,
												sortChannelCombinations,
												theType; 
		
		Boolean								modalDone,
												separabilityTableFlag,
												thresholdedClassPairTableFlag,
												thresholdedGroupTableFlag,
												listClassPairDistancesFlag; 

		
				// Get the modal dialog for the separability list specification.		
					
		dialogPtr = LoadRequestedDialog (kSepListSpecificationID, kCopyScrap, 1, 2);
		if (dialogPtr == NULL)													return;
		
				// Save handle for the OK button for use later.								
				
		GetDialogItem (dialogPtr, 1, &theType, &okHandle, &theBox);
		
				// Set routine to draw outline on "OK" box.									
				
		SetDialogItemDrawRoutine (dialogPtr, 23, gHiliteOKButtonPtr);
		
				// Initialize dialog variables.

		SeparabilityListDialogInitialize (dialogPtr,
														gSeparabilitySpecsPtr,
														*combinationsToListPtr,
														&separabilityTableFlag,
														&localCombinationsToList,
														&localMinDistanceRangeToList,
														&localMaxDistanceRangeToList,
														&sortChannelCombinations,
														&thresholdedClassPairTableFlag,
														&localDistancesLessThanToList,
														&thresholdedGroupTableFlag,
														&localDistancesLessThanToGroup,
														&listClassPairDistancesFlag);
		
				// List separability table.											
					
		SetDLogControl (dialogPtr, 4, separabilityTableFlag);
			
				// Load number of best channel combinations to be listed.	
		
		LoadDItemValue (dialogPtr, IDC_NumberBestCombinations, localCombinationsToList);
			
				// Load minimum distance to be listed.							
		
		LoadDItemRealValue (dialogPtr, 
									IDC_MinDistanceRange, 
									(double)localMinDistanceRangeToList,
									1);
			
				// Load maximum distance to be listed.							
		
		LoadDItemRealValue (dialogPtr, 
									IDC_MaxDistanceRange, 
									(double)localMaxDistanceRangeToList,
									1);
			
				// Sort channel combinations by average class distance.					
				// Sort channel combinations by minimum class distance.
				// Sort channel combinations in order computed.												
					
		SetDLogControl (dialogPtr, 13, (sortChannelCombinations==1));
					
		SetDLogControl (dialogPtr, 14, (sortChannelCombinations==2));
				
		SetDLogControl (dialogPtr, 25, (sortChannelCombinations==0));
		
				// List table of class pairs < threshold distance.							
					
		SetDLogControl (dialogPtr, 19, thresholdedClassPairTableFlag);
		
				// Threshold distance for table of class.										
		
		LoadDItemRealValue (dialogPtr, 
									IDC_ClassPairThreshold,
									(double)localDistancesLessThanToList,
									1);
		
				// List grouping table.																
					
		SetDLogControl (dialogPtr, 21, thresholdedGroupTableFlag);
		
				// Threshold distance for grouping table.										
					
		LoadDItemRealValue (dialogPtr, 
									IDC_ClassGroupThreshold,
									(double)localDistancesLessThanToGroup,
									1);
		
				// Flag to list class pair distances.							
		
		SetDLogControl (dialogPtr, 24, listClassPairDistancesFlag);
		
				// Hide some unused dialog items.
				
		HideDialogItem (dialogPtr, 3);
		HideDialogItem (dialogPtr, 26);
		HideDialogItem (dialogPtr, 27);
			
				// Center the dialog and then show it.											
				
		ShowDialogWindow (dialogPtr, kSepListSpecificationID, kSetUpDFilterTable);
		
		gDialogItemDescriptorPtr[9] = kDItemReal;
		gDialogItemDescriptorPtr[11] = kDItemReal;
		gDialogItemDescriptorPtr[20] = kDItemReal;
		gDialogItemDescriptorPtr[22] = kDItemReal;
		
				// Set default text selection to first edit text item						
				
		SelectDialogItemText (dialogPtr, 6, 0, INT16_MAX);
		
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
					case 4:		// Separability table. 
					case 19:		// Class pair table. 
					case 21:		// Class grouping table. 
					case 24:		// List average/minimum distance only. 
						ChangeDLogCheckBox ((ControlHandle)theHandle);
								
								// Update hilite of OK control									
														
						if (!GetDLogControl (dialogPtr, 4) && 
								!GetDLogControl (dialogPtr, 19) &&
								!GetDLogControl (dialogPtr, 21))
							HiliteControl ((ControlHandle)okHandle, 255);
						else
							HiliteControl ((ControlHandle)okHandle, 0);
						break;
						
					case 6:		// Best number of combinations. 							
						if (theNum <= 0 || theNum > SInt16_MAX) 
							NumberErrorAlert (localCombinationsToList, 
														dialogPtr, 
														itemHit);
																
						else	// theNum > 0 || ... 
							localCombinationsToList = theNum;
						break;
						
					case 9:		// Minimum distance range to be listed.
						realValue = GetDItemRealValue (dialogPtr, itemHit);
						if (realValue > INT_MAX)	
							RealNumberErrorAlert (
										localMinDistanceRangeToList,
										dialogPtr, 
										itemHit,
										1);
										
						if (realValue >= 0 && realValue <= INT_MAX)
							localMinDistanceRangeToList = realValue;
						break;
						
					case 11:		// Maximum distance range to be listed. 
						realValue = GetDItemRealValue (dialogPtr, itemHit);
						if (realValue > INT_MAX)	
							RealNumberErrorAlert (
										localMaxDistanceRangeToList,
										dialogPtr, 
										itemHit,
										1);
										
						if (realValue >= 0 && realValue <= INT_MAX)
							localMaxDistanceRangeToList = realValue;
						break;
						
					case 13:		// Sort by average class distance.  
					case 14:		// Sort by minimum class distance. 
					case 25:		// Do not sort distance measures. 
						SetControlValue ((ControlHandle)theHandle, 1);
						
						for (itemNumber=13; itemNumber<=15; itemNumber++)
							{
							if (itemNumber == 15)  itemNumber = 25;
							if (itemHit != itemNumber)
								{
								GetDialogItem (dialogPtr,
														itemNumber,
														&theType,
														&theHandle,
														&theBox);
								SetControlValue ((ControlHandle)theHandle, 0);
								
								}	// end "if (itemHit != itemNumber)" 
								
							}	// end "for (itemNumber=13, itemNumber<=15, ..." 
						break;
						
					case 16:		// Include channel combinations.  
						break;
						
					case 18:		// Exclude channel combinations.  
						break;
						
					case 20:		// Class pair distance threshold. 
						realValue = GetDItemRealValue (dialogPtr, itemHit);
						if (realValue > INT_MAX)	
							RealNumberErrorAlert (
										localDistancesLessThanToList,
										dialogPtr, 
										itemHit,
										1);
										
						if (realValue >= 0 && realValue <= INT_MAX)
							localDistancesLessThanToList = realValue;
						break;
						
					case 22:		// Class grouping threshold value. 
						realValue = GetDItemRealValue (dialogPtr, itemHit);
						if (realValue > INT_MAX)	
							RealNumberErrorAlert (
										localDistancesLessThanToGroup,
										dialogPtr, 
										itemHit,
										1);
										
						if (realValue >= 0 && realValue <= INT_MAX)
							localDistancesLessThanToGroup = realValue;
							
						break;
							
					}	// end "switch (itemHit)" 
					
				}	// end "if (itemHit > 2)" 
				
			else	// itemHit <= 2 
				{
				if	(itemHit == 1)      // User selected OK for information 
					{
					modalDone = TRUE;
					
					SeparabilityListDialogOK (gSeparabilitySpecsPtr,
														combinationsToListPtr,
														(GetDLogControl (dialogPtr, 4) == 1),
														localCombinationsToList,
														GetDItemRealValue (dialogPtr, 9),
														GetDItemRealValue (dialogPtr, 11),
														(GetDLogControl (dialogPtr, 13) == 1),
														(GetDLogControl (dialogPtr, 14) == 1),
														(GetDLogControl (dialogPtr, 25) == 1),
														(GetDLogControl (dialogPtr, 19) == 1),
														GetDItemRealValue (dialogPtr, 20),
														(GetDLogControl (dialogPtr, 21) == 1),
														GetDItemRealValue (dialogPtr, 22),
														(GetDLogControl (dialogPtr, 24) == 1));
																						
					}	// end "if (itemHit == 1)" 
					
				if	(itemHit == 2)      // User selected Cancel for information 
					modalDone = TRUE;
				
				}	// end "else itemHit <= 2" 
					
			} while (!modalDone);
			
		CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
	#endif	// defined multispec_mac

	#if defined multispec_win   
		CMFeatureSelectionListDialog*		dialogPtr = NULL;
		
		TRY
			{ 
			dialogPtr = new CMFeatureSelectionListDialog ();
			
			dialogPtr->DoDialog (gSeparabilitySpecsPtr, combinationsToListPtr); 
		                       
			delete dialogPtr;
			}
			
		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			}
		END_CATCH_ALL  
	#endif	// defined multispec_win  

	#if defined multispec_wx   
		CMFeatureSelectionListDialog* dialogPtr = NULL;

		try 
			{
			dialogPtr = new CMFeatureSelectionListDialog ();
			dialogPtr->DoDialog (gSeparabilitySpecsPtr, combinationsToListPtr);
			delete dialogPtr;
			} 
		catch (int e) 
			{
			MemoryMessage (0, kObjectMessage);
			}
	#endif	
	
}	// end "SeparabilityListDialog" 



void SeparabilityListDialogInitialize (
				DialogPtr							dialogPtr,
				SeparabilitySpecsPtr				separabilitySpecsPtr,
				SInt32								combinationsToList,
				Boolean*								separabilityTableFlagPtr,
				SInt32*								localCombinationsToListPtr,
				double*								localMinDistanceRangeToListPtr,
				double*								localMaxDistanceRangeToListPtr,
				SInt16*								sortChannelCombinationsPtr,
				Boolean*								thresholdedClassPairTableFlagPtr,
				double*								localDistancesLessThanToListPtr,
				Boolean*								thresholdedGroupTableFlagPtr,
				double*								localDistancesLessThanToGroupPtr,
				Boolean*								listClassPairDistancesFlagPtr)

{
			// List separability table flag. Do not allow the user to change.											
	
	*separabilityTableFlagPtr = (separabilitySpecsPtr->tablesToList & 0x0001);     
	SetDLogControlHilite (dialogPtr, IDC_SeparabilityTable, 255); 
		
			// Load number of best channel combinations to be listed.	
	
	*localCombinationsToListPtr = combinationsToList;
		
			// Load minimum distance to be listed.							
	
	*localMinDistanceRangeToListPtr = 
										(double)separabilitySpecsPtr->minDistanceRangeToList;
		
			// Load maximum distance to be listed.							
	
	*localMaxDistanceRangeToListPtr = 
										(double)separabilitySpecsPtr->maxDistanceRangeToList;
		
			// Sort channel combinations by average class distance.					
			// Sort channel combinations by minimum class distance.	
			// Sort channel combinations in order computed.								
	
	*sortChannelCombinationsPtr = separabilitySpecsPtr->sortChannelCombinations;
	
			// Include selected channel combinations.				
			// Hide until implemented.													
	
	HideDialogItem (dialogPtr, IDC_IncludeChannelComboPrompt);
	HideDialogItem (dialogPtr, IDC_IncludeChannelCombo);
	
			// Exclude selected channel combinations.				
			// Hide until implemented.													
			
	HideDialogItem (dialogPtr, IDC_ExcludeChannelComboPrompt);
	HideDialogItem (dialogPtr, IDC_ExcludeChannelCombo);
	
			// List table of class pairs < threshold distance.			
			// Hide until implemented.													
				
	*thresholdedClassPairTableFlagPtr = (separabilitySpecsPtr->tablesToList & 0x0002);
	HideDialogItem (dialogPtr, IDC_ClassPairThresholdTable);
	
			// Threshold distance for table of class.						
			// Hide until implemented.															
	
	*localDistancesLessThanToListPtr = 
										(double)separabilitySpecsPtr->distancesLessThanToList;
	HideDialogItem (dialogPtr, IDC_ClassPairThreshold);
	
			// List grouping table.												
			// Hide until implemented.													
				
	*thresholdedGroupTableFlagPtr = (separabilitySpecsPtr->tablesToList & 0x0004);
	HideDialogItem (dialogPtr, IDC_ClassGroupThresholdTable);
	
			// Threshold distance for grouping table.						
			// Hide until implemented.															
				
	*localDistancesLessThanToGroupPtr = 
									(double)separabilitySpecsPtr->distancesLessThanToGroup;
	HideDialogItem (dialogPtr, IDC_ClassGroupThreshold);
	
			// Flag to list class pair distances.							
	
	*listClassPairDistancesFlagPtr = separabilitySpecsPtr->listClassPairDistances;
	
			// Load the largest number of channel combinations to be	
			// considered with the current selections.
	
	if (MGetString (gTextString2, kFeatureSelectionStrID, IDS_FeatureSelection34))
		{
		sprintf ((char*)gTextString, 
						(char*)&gTextString2[1], 
						(double)separabilitySpecsPtr->maxNumberFeatureCombinations);
						
		CtoPstring (gTextString, gTextString);
						
		LoadDItemString (dialogPtr, IDC_PossibleCombinations, (Str255*)gTextString);
		
		}	// end "if (MGetString (gTextString2, ..."
	
}	// end "SeparabilityListDialogInitialize" 



void SeparabilityListDialogOK (
				SeparabilitySpecsPtr				separabilitySpecsPtr,
				SInt32*								combinationsToListPtr,
				Boolean								separabilityTableFlag,
				SInt32								localCombinationsToList,
				double								localMinDistanceRangeToList,
				double								localMaxDistanceRangeToList,
				Boolean								sortByAverageDistanceFlag,
				Boolean								sortByMinimumDistanceFlag,
				Boolean								sortByOrderComputedFlag,
				Boolean								thresholdedClassPairTableFlag,
				double								localDistancesLessThanToList,
				Boolean								thresholdedGroupTableFlag,
				double								localDistancesLessThanToGroup,
				Boolean								listClassPairDistancesFlag)

{
			// Tables to be listed										
			
	separabilitySpecsPtr->tablesToList = 0;
	
			// 	Separability tables.	
								
	if (separabilityTableFlag)				
		separabilitySpecsPtr->tablesToList += 1;						
		
			// 	List class pair table.									
										
	if (thresholdedClassPairTableFlag)				
		separabilitySpecsPtr->tablesToList += 2;
		
			// 	List class grouping table.								
										
	if (thresholdedGroupTableFlag)				
		separabilitySpecsPtr->tablesToList += 4;
		
			// Number best combinations to list.						
			
	*combinationsToListPtr = localCombinationsToList;
		
			// Minimum distance range to be listed.					
			
	separabilitySpecsPtr->minDistanceRangeToList = 
														(float)localMinDistanceRangeToList;
		
			// Maximum distance range to be listed.				
			
	separabilitySpecsPtr->maxDistanceRangeToList = 
														(float)localMaxDistanceRangeToList;
	
			// Sort request
									
	separabilitySpecsPtr->sortChannelCombinations = 0;
		
			// 	Sort by average class distance.						
			
	if (sortByAverageDistanceFlag)				
		separabilitySpecsPtr->sortChannelCombinations = 1;
		
			// 	Sort by minimum class distance.						
										
	if (sortByMinimumDistanceFlag)				
		separabilitySpecsPtr->sortChannelCombinations = 2;
		
			// 	Do not sort class distances.							
										
	if (sortByOrderComputedFlag)				
		separabilitySpecsPtr->sortChannelCombinations = 0;
		
			// Include channel combinations.	Not implemented						
		
			// Exclude channel combinations.	Not implemented	
		
			// Threshold for class pair table.						
			
	separabilitySpecsPtr->distancesLessThanToList = 
															(float)localDistancesLessThanToList;
		
			// Threshold for class grouping table.					
			
	separabilitySpecsPtr->distancesLessThanToGroup = 
															(float)localDistancesLessThanToGroup;

			// Flag to list average/minimum distance only.	
	
	separabilitySpecsPtr->listClassPairDistances = listClassPairDistancesFlag; 
	
}	// end "SeparabilityListDialogOK" 


                                                                                
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean SetupSeparabilityDistanceMemory
//
//	Software purpose:	The purpose of this routine is to setup the 
//							memory to be used to store the distance measurements
//							for all the selected class pairs.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/03/1989
//	Revised By:			Larry L. Biehl			Date: 03/22/2019

Boolean SetupSeparabilityDistanceMemory (
				SInt16								numberSeparabilityChannels, 
				SInt16*								featurePtr, 
				SInt16								numberFeatures, 
				UInt32								numberClassCombinations, 
				UInt32								numberChannelGroupCombinations)

{
	double								bytesNeededDouble;
	                                             
	SepDistancesSummaryPtr			distancesSummaryPtr;
	SInt16*								channelSetsPtr;
	
	SInt64								lContBlock;
	
	UInt32								bytesNeeded,
											bytesNeededPerCombination,
											channelCombination,
											channelSetIndex,
											combinationSlotsAvailable,
											combinationSlotsNeeded,
											combinationsToBeSaved;
	                              
	SInt32								classPairDistancesIndex;
											
	SInt16								returnCode;

	Boolean								continueFlag;          
	

			// Check input parameters.															
			// The check for numberChannelGroupCombinations and 						
			// numberClassCombinations being less than 0 because as of 11/6/1989 	
			// LSC 4 does not handle unsigned long int correctly in the 881 		
			// Math library.																		
			
	if (numberSeparabilityChannels <= 0 || 
					numberSeparabilityChannels > kMaxNumberStatChannels)		
																					return (FALSE);
																					
	if (numberFeatures <= 0 || numberFeatures > kMaxNumberStatChannels)
																					return (FALSE);
																					
	if (featurePtr == NULL)												
																					return (FALSE);
																					
	if (numberChannelGroupCombinations <= 0)						
																					return (FALSE);
																					
	if (numberClassCombinations <= 0)									
																					return (FALSE);
	
	continueFlag = TRUE;
	
			// Get the number of combination slots of memory that will be needed	
			// to store the desired channel combinations.  This will allow for	
			// the top 'n' combinations for both average and minimum distance		
			// measure lists.  I.E. worst case; the average and minimum	lists 	
			// do not overlap.																	
			
	combinationSlotsNeeded = 
							gSeparabilitySpecsPtr->combinationsToStore * 2  + 1;
	combinationSlotsNeeded = 
						MIN (combinationSlotsNeeded, numberChannelGroupCombinations);
								
			// Check if memory is available for the requested parameters.			
			
	bytesNeededPerCombination = 
			(UInt32)numberSeparabilityChannels * sizeof (SInt16) +
								sizeof (SepDistancesSummary) +
										numberClassCombinations * sizeof (float);
			
	MGetFreeMemory (&lContBlock); 
	
	combinationSlotsAvailable = 
								(lContBlock - kMinFreeMemory)/bytesNeededPerCombination;
	if (combinationSlotsAvailable < combinationSlotsNeeded)
		{
				// Allow user to change the number of combinations to be saved.	
				
		combinationsToBeSaved = (combinationSlotsAvailable - 1)/2;
		
		MInitCursor ();
		continueFlag = LoadSpecifiedStringNumberLongP (
														kFeatureSelectionStrID, 
														IDS_FeatureSelection31, 
														(char*)gTextString,
														(char*)gTextString2,
														continueFlag,
														combinationsToBeSaved,
														0);
														
		if (continueFlag)         
			returnCode = DisplayAlert (kContinueCancelAlertID, 
													3, 
													0, 
													0, 
													0, 
													(UCharPtr)gTextString);
			
		MSetCursor (kWait);
		
		if (combinationsToBeSaved < 1 || returnCode == 2) 
			continueFlag = FALSE;
		
		else	// combinationsToBeSaved > 1 
			{
			gSeparabilitySpecsPtr->combinationsToStore = combinationsToBeSaved;
			combinationSlotsNeeded = combinationsToBeSaved * 2 + 1;
			
			}	// end "else combinationsToBeSaved > 1" 
		
		}	// end "if (combinationSlotsAvailable < ..." 
		
	if (continueFlag)
		{					
				// Get the memory needed for the channel combination list.			
				
		bytesNeededDouble = (double)combinationSlotsNeeded * 
											numberSeparabilityChannels * sizeof (SInt16);
		continueFlag = (bytesNeededDouble < LONG_MAX);
		
		if (continueFlag)
			{
			bytesNeeded = combinationSlotsNeeded * 
											numberSeparabilityChannels * sizeof (SInt16);
			gSeparabilitySpecsPtr->channelSetsPtr = 
													(SInt16*)MNewPointer (bytesNeeded);
													                      
			continueFlag = (gSeparabilitySpecsPtr->channelSetsPtr != NULL);
				
			}	// end "if (continueFlag)"
		
		}	// end "if (continueFlag)" 
		
	if (continueFlag)
		{
				// Get pointer to memory to use for distance summary structure.	
					
		bytesNeededDouble = 
					(double)combinationSlotsNeeded * sizeof (SepDistancesSummary);
		continueFlag = (bytesNeededDouble < LONG_MAX); 
		
		if (continueFlag)
			{ 
			bytesNeeded = combinationSlotsNeeded * sizeof (SepDistancesSummary);
			gSeparabilitySpecsPtr->distancesSummaryPtr = (Ptr)MNewPointer (bytesNeeded);
													                      
			continueFlag = (gSeparabilitySpecsPtr->distancesSummaryPtr != NULL); 
				
			}	// end "if (continueFlag)"
				
		}	// end "if (continueFlag)" 
		
	if (continueFlag && gSeparabilitySpecsPtr->listClassPairDistances)
		{
				// Get pointer to memory to use for class pair distances.			
		
		bytesNeededDouble = (double)combinationSlotsNeeded * 
													numberClassCombinations * sizeof (float);  
		continueFlag = (bytesNeededDouble < LONG_MAX); 
		
		if (continueFlag)
			{ 
			bytesNeeded = combinationSlotsNeeded * 
													numberClassCombinations * sizeof (float);
			gSeparabilitySpecsPtr->distancesPtr = (float *)MNewPointer (bytesNeeded);
													                      
			continueFlag = (gSeparabilitySpecsPtr->distancesPtr != NULL); 
				
			}	// end "if (continueFlag)"
			                
		}	// end "if (continueFlag)" 
		
	if (continueFlag)
		{
				// Save the number of slots of memory that was created.				
				
		gSeparabilitySpecsPtr->combinationSlots = combinationSlotsNeeded;
		
				// Initialize indices in the distances summary structure.			
		
		channelSetsPtr = gSeparabilitySpecsPtr->channelSetsPtr;
		distancesSummaryPtr = 
				(SepDistancesSummaryPtr)gSeparabilitySpecsPtr->distancesSummaryPtr;
				
		channelSetIndex = 0;
		classPairDistancesIndex = 0;
		
		for (channelCombination=0; 
				channelCombination<combinationSlotsNeeded;
				channelCombination++)
			{		
			distancesSummaryPtr[channelCombination].channelSetIndex = 
																				channelSetIndex;
																				
			distancesSummaryPtr[channelCombination].classPairDistancesIndex = 
																		classPairDistancesIndex;
			
			channelSetIndex += numberSeparabilityChannels;
			classPairDistancesIndex += numberClassCombinations;
				
			}	// end "for (channelCombination=0; ..." 
				
		}	// end "if (continueFlag)" 
		
	return (continueFlag);

}	// end "SetupSeparabilityDistanceMemory" 


                                                                                
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean SetupSeparabilityStatMemory
//
//	Software purpose:	The purpose of this routine is to setup the 
//							memory to be allocated for the separability
//							statistics.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/03/1989
//	Revised By:			Larry L. Biehl			Date: 04/30/1993	

Boolean SetupSeparabilityStatMemory (
				HDoublePtr* 						meanPtr, 
				HDoublePtr* 						covariancePtr, 
				HDoublePtr* 						inverseCovPtr, 
				HDoublePtr*							logDeterminantPtr, 
				HChannelStatisticsPtr*			classChannelStatsPtr, 
				SInt16								numberFeatures,
				Boolean								covarianceFlag)

{
	UInt32								bytesNeeded,
											numberChannels;
	
	Boolean								continueFlag;
	
	
			// Initialize parameters															
			
	continueFlag = TRUE;
	numberChannels = gSeparabilitySpecsPtr->numberChannels;
	
	*meanPtr = 					NULL;
	*covariancePtr = 			NULL;
	*inverseCovPtr = 			NULL;
	*logDeterminantPtr =		NULL;
	*classChannelStatsPtr = NULL;
	
			// Get pointer to memory to use for class mean vectors.					
			
	bytesNeeded = (UInt32)gSeparabilitySpecsPtr->numberClasses *
															numberFeatures * sizeof (double);
	*meanPtr = (HDoublePtr)MNewPointer (bytesNeeded);
	continueFlag = (*meanPtr != NULL);
		
	if (continueFlag)
		{
				// Get pointer to memory to use for class covariance matrix.		
				// Allow memory for one square matrix to be used as temporary		
				// memory when loading the class covariance memory.  This is		
				// needed when transformations are to be done.							
					
		bytesNeeded = (UInt32)gSeparabilitySpecsPtr->numberClasses * 
							numberFeatures * sizeof (double);
		if (covarianceFlag)
			{
			bytesNeeded *= (numberFeatures + 1);
			bytesNeeded /= 2;
			
			bytesNeeded += (numberChannels*numberChannels-
								numberFeatures*(numberFeatures+1)/2) * sizeof (double);
			
			}	// end "if (covarianceFlag)" 
			
		*covariancePtr = (HDoublePtr)MNewPointer (bytesNeeded);
		continueFlag = (*covariancePtr != NULL); 
			
		}	// end "if (continueFlag)" 
		
	if (continueFlag && 
		(gSeparabilitySpecsPtr->distanceMeasure == kTransformedDivergence ||
			gSeparabilitySpecsPtr->distanceMeasure == kDivergence))
		{
				// Get pointer to memory to use for inverse covariance matrices	
				// for all classes. This is only needed for divergence and
				// transformed divergence																
					
		bytesNeeded = (UInt32)gSeparabilitySpecsPtr->numberClasses * 
																	numberFeatures * sizeof (double);
		if (covarianceFlag)
			{
			bytesNeeded *= (numberFeatures + 1);
			bytesNeeded /= 2;
			
			}	// end "if (covarianceFlag)" 
			
		*inverseCovPtr = (HDoublePtr)MNewPointer (bytesNeeded);
		continueFlag = (*inverseCovPtr != NULL);
			
		}	// end "if (continueFlag && ...)" 
		
	if (continueFlag)
		{
				// Get pointer to memory to use for storage of class covariance	
				// matrix determinant logs.													
					
		bytesNeeded = (UInt32)gSeparabilitySpecsPtr->numberClasses *
																					sizeof (double);
		*logDeterminantPtr = (HDoublePtr)MNewPointer (bytesNeeded);
		continueFlag = (*logDeterminantPtr != NULL);
			
		}	// end "if (continueFlag)" 
		
	if (continueFlag)
		{
					// Get pointer to memory to use for class channel statistics	
					// that will be used in computing the covariance matrix and		
					// getting the data for the class mean vector.						
					
		bytesNeeded = (UInt32)gSeparabilitySpecsPtr->numberChannels * 
																		sizeof (ChannelStatistics);
		*classChannelStatsPtr = (HChannelStatisticsPtr)MNewPointer (bytesNeeded);
		continueFlag = (*classChannelStatsPtr != NULL) ;
			
		}	// end "if (continueFlag)" 
		
	if (continueFlag)
		{
				// Get pointer to memory to use for inverse matrix computation.	
				
		continueFlag = SetupMatrixInversionMemory (
				numberFeatures,
				covarianceFlag,
				&gInverseMatrixMemory.inversePtr, 
				&gInverseMatrixMemory.pivotPtr, 
				&gInverseMatrixMemory.indexRowPtr, 
				&gInverseMatrixMemory.indexColPtr, 
				&gInverseMatrixMemory.ipvotPtr);
			
		}	// end "if (continueFlag)" 
		
	return (continueFlag);
		
}	// end "SetupSeparabilityStatMemory" 


                                                                                
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double TransformedDivergence
//
//	Software purpose:	The purpose of this routine is to calculate
//							the transformed divergence distance between the 
//							two classes represented by their input covariance 
//							matrices, inverse covariance matrices and mean 
//							vectors.
//
//	Parameters in:		cov1Ptr:			covariance matrix of class 1
//							cov2Ptr:			covariance matrix of class 2
//							inv1Ptr:			inverse of class 1 covariance
//							inv2Ptr:			inverse of class 2 covariance
//							mean1Ptr:		class 1 mean vector
//							mean2Ptr:		class 2 mean vector
//							meanDifPtr:		vector storage of class 1 and
//												class 2 mean difference.
//							numberFeatures: number of features (channels)
//													represented in the mean vector and
//													covariance and inverse covariance
//													matrices.									
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:	
//
//	Coded By:			C.H. Lee					Date: 11/06/1989
//	Revised By:			Larry L. Biehl			Date: 11/06/1989	

double TransformedDivergence (
				HDoublePtr							mean1Ptr, 
				HDoublePtr							mean2Ptr, 
				HDoublePtr							cov1Ptr, 
				HDoublePtr							cov2Ptr, 
				HDoublePtr							inv1Ptr, 
				HDoublePtr							inv2Ptr, 
				HDoublePtr							meanDifPtr, 
				SInt16								numberFeatures)

{
	double								transformedDistance;
	
	
			// First get the normal divergence measurement.								
			
	transformedDistance = Divergence (mean1Ptr, mean2Ptr, 
													cov1Ptr, cov2Ptr,
													inv1Ptr, inv2Ptr,
													meanDifPtr, 
													numberFeatures);
													
	transformedDistance = 2000*(1 - exp (-transformedDistance/8));
	
	return (transformedDistance);
	
}	// end "TransformedDivergence" 


                                                                                 
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 UpdateSeparabilityLists
//
//	Software purpose:	The purpose of this routine is to calculate
//							the separability for all channel combinations that
//							are included in a step procedure search and
//							all class combinations, sort the results if requested
//							and list the results.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			ComputeSeparabilityForAllChannelCombinations in SFeatureSelection.cpp
//							ComputeSeparabilityForStepChannelCombinations in SFeatureSelection.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/09/1992
//	Revised By:			Larry L. Biehl			Date: 12/10/1992	

SInt16 UpdateSeparabilityLists (
				SepDistancesSummaryPtr			distancesSummaryPtr, 
				SInt32								combinationIndex, 
				UInt32								channelCombination, 
				SInt32*								freeIndexPtr, 
				Boolean								firstTimeFlag)

{
	double								classPairMean,
											classPairMinimum;
	
	static double						maxMean,
											maxMinimum,
											minMean,
											minMinimum;
	
	SInt32								combination,
											previousCombination;
	
	static SInt32						lastFreeIndex;
	
	static UInt32						minimumStart,
											minimumEnd,
											meanStart,
											meanEnd;
	
	
	if (distancesSummaryPtr == NULL)												
																					return (-1);
				
	if (firstTimeFlag)
		{
		minimumStart 	= 0;
		minimumEnd 		= 0;
		meanStart 		= 0;
		meanEnd 			= 0;
		maxMean			= 0.;
		minMean			= DBL_MAX;
		maxMinimum		= 0.;
		minMinimum		= DBL_MAX;
		
		}	// end "if (firstTimeFlag)" 
		
			// Update the mean list pointers.												
	
	classPairMean = distancesSummaryPtr[combinationIndex].classPairMean;
	
	if (classPairMean >= maxMean)
		{
		distancesSummaryPtr[meanStart].previousMean = combinationIndex;
		
		distancesSummaryPtr[combinationIndex].nextMean = meanStart;
		distancesSummaryPtr[combinationIndex].previousMean = -1;
		meanStart = combinationIndex;
		maxMean = classPairMean;
		
		if (firstTimeFlag)
			{
			minMean = classPairMean;
			distancesSummaryPtr[0].nextMean = -1;
			
			}	// end "if (firstTimeFlag)" 
		
		}	// end "if (classPairMean >= maxMean" 
	
	else	// classPairMean < maxMean	
		{
		if (classPairMean <= minMean)
			{
			distancesSummaryPtr[meanEnd].nextMean = combinationIndex;
			
			distancesSummaryPtr[combinationIndex].nextMean = -1;
			distancesSummaryPtr[combinationIndex].previousMean = meanEnd;
			meanEnd = combinationIndex;
			minMean = classPairMean;
		
			}	// end "if (classPairMean <= minMean" 
			
		else	// minMean < classPairMean < maxMean 
			{
			combination = distancesSummaryPtr[meanStart].nextMean;
			while (combination != -1)
				{
				if (classPairMean > 
								distancesSummaryPtr[combination].classPairMean)
					{
					previousCombination = 
								distancesSummaryPtr[combination].previousMean;
					distancesSummaryPtr[previousCombination].nextMean = 
																		combinationIndex;
									
					distancesSummaryPtr[combinationIndex].previousMean = 
																		previousCombination;
					distancesSummaryPtr[combinationIndex].nextMean = 
																				combination;
																				
					distancesSummaryPtr[combination].previousMean = 
																		combinationIndex;
					
					combination = meanEnd;
					
					}	// end "classPairMean > distancesSummaryPtr..." 
				
				combination = distancesSummaryPtr[combination].nextMean;
				
				}	// end "while (combination != -1)" 
			
			}	// end "else minMean < classPairMean < maxMean 
			
		}	// end "else classPairMean < maxMean" 
				
			// Update the minimum list pointers.											

	classPairMinimum = distancesSummaryPtr[combinationIndex].classPairMinimum;
					
	if (classPairMinimum >= maxMinimum)
		{
		distancesSummaryPtr[minimumStart].previousMinimum = 
																	combinationIndex;
		
		distancesSummaryPtr[combinationIndex].nextMinimum = minimumStart;
		distancesSummaryPtr[combinationIndex].previousMinimum = -1;
		minimumStart = combinationIndex;
		maxMinimum = classPairMinimum;
		
		if (firstTimeFlag)
			{
			minMinimum = classPairMinimum;
			distancesSummaryPtr[0].nextMinimum = -1;
			
			}	// end "if (firstTimeFlag)" 
		
		}	// end "if (classPairMinimum >= maxMinimum" 
	
	else	// classPairMean < maxMinimum	
		{
		if (classPairMinimum <= minMinimum)
			{
			distancesSummaryPtr[minimumEnd].nextMinimum = combinationIndex;
			
			distancesSummaryPtr[combinationIndex].nextMinimum = -1;
			distancesSummaryPtr[combinationIndex].previousMinimum = 
																					minimumEnd;
			minimumEnd = combinationIndex;
			minMinimum = classPairMinimum;
		
			}	// end "if (classPairMinimum <= minMinimum" 
			
		else	// minMinimum < classPairMinimum < maxMinimum 
			{
			combination = distancesSummaryPtr[minimumStart].nextMinimum;
			while (combination != -1)
				{
				if (classPairMinimum > 
								distancesSummaryPtr[combination].classPairMinimum)
					{
					previousCombination = 
								distancesSummaryPtr[combination].previousMinimum;
					distancesSummaryPtr[previousCombination].nextMinimum = 
																		combinationIndex;
								
					distancesSummaryPtr[combinationIndex].previousMinimum = 
																		previousCombination;
					distancesSummaryPtr[combinationIndex].nextMinimum = 
																				combination;
																				
					distancesSummaryPtr[combination].previousMinimum = 
																		combinationIndex;
					combination = minimumEnd;
					
					}	// end "classPairMean > distancesSummaryPtr..." 
				
				combination = distancesSummaryPtr[combination].nextMinimum;
				
				}	// end "while (combination != -1)" 
			
			}	// end "else minMinimum < classPairMinimum < maxMinimum 
			
		}	// end "else classPairMinimum < maxMinimum" 
			
			// Check if the last mean and minimum need to be removed from			
			// the list.																			
			
	if (channelCombination >= (UInt32)gSeparabilitySpecsPtr->combinationsToStore)
		{
				// Remove the last mean from the list.										
		
		previousCombination = distancesSummaryPtr[meanEnd].previousMean;
		distancesSummaryPtr[previousCombination].nextMean = -1;
		distancesSummaryPtr[meanEnd].nextMean	= -100;
		
		if (distancesSummaryPtr[meanEnd].nextMinimum == -100)
			{
			if (*freeIndexPtr < 0)
				*freeIndexPtr = lastFreeIndex = meanEnd;
				
			else	// *freeIndexPtr >= 0 
				{
				distancesSummaryPtr[lastFreeIndex].nextMean = meanEnd;
				lastFreeIndex = meanEnd;
				
				}	// end "else *freeIndexPtr != -1" 
			
			}	// end "if (distancesSummaryPtr[meanEnd]..." 
			
		meanEnd = previousCombination;
		minMean = distancesSummaryPtr[meanEnd].classPairMean;
			
				// Remove the last minimum from the list.							
		
		previousCombination = 
								distancesSummaryPtr[minimumEnd].previousMinimum;
		distancesSummaryPtr[previousCombination].nextMinimum = -1;
		distancesSummaryPtr[minimumEnd].nextMinimum	= -100;
		
		if (distancesSummaryPtr[minimumEnd].nextMean == -100)
			{
			if (*freeIndexPtr < 0)
				*freeIndexPtr = lastFreeIndex = minimumEnd;
				
			else	// *freeIndexPtr != -1 
				{
				distancesSummaryPtr[lastFreeIndex].nextMean = minimumEnd;
				lastFreeIndex = minimumEnd;
				
				}	// end "else *freeIndexPtr != -1" 
			
			}	// end "if (distancesSummaryPtr[minimumEnd]..." 
			
		minimumEnd = previousCombination;
		minMinimum = distancesSummaryPtr[minimumEnd].classPairMinimum;
		
		}	// end "if (channelCombination >= ...combinationsToStore)" 

			// Update the count of the number of feature combinations  			
			// that have been loaded into memory.										
	
	else	// channelCombination < ...combinationsToStore 	
		gSeparabilitySpecsPtr->numberFeatureCombinationsLoaded++;
	
			// Save mean list pointers.													
		
	gSeparabilitySpecsPtr->meanStart = meanStart;
	gSeparabilitySpecsPtr->meanEnd = meanEnd;
	
			// Save minimum list pointers.												
	
	gSeparabilitySpecsPtr->minimumStart = minimumStart;
	gSeparabilitySpecsPtr->minimumEnd = minimumEnd;
	
	return (1);
				
}	// end "UpdateSeparabilityLists" 

