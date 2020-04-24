///                                     MultiSpec
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
//	File:						SStatisticsImage.cpp
//
//	Authors:					Chulhee Lee
//								Larry L. Biehl
//
//	Revision date:			11/11/2019
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	The purpose of the routines in this file are to create
//								statistics images of the field, class or selected area mean
//								and covariance statistics.  Chulhee Lee developed the idea and
//								the code.
//
//------------------------------------------------------------------------------------
  
#include "SMultiSpec.h"  
	
#ifdef multispec_wx 
   #include "xMultiSpec.h"
   #include "xStatisticsImageDialog.h"
#endif

#if defined multispec_mac || defined multispec_mac_swift
	#define	IDC_ChannelPrompt					3
	#define	IDC_ChannelCombo					4
	#define	IDC_ClassesRadio					6
	#define	IDC_ClassCombo						7
	#define	IDC_FieldsPrompt					8
	#define	IDC_FieldsCombo					9
	#define	IDC_StatPrompt						10
	#define	IDC_SelectedClassRadio			11
	#define	IDC_SelectedFieldRadio			12
	#define	IDC_OverallRadio					14
	#define	IDC_IndividualRadio				15
	#define	IDC_UserSettingRadio				16
	#define	IDC_MinPrompt						17
	#define	IDC_StatisticMin					18
	#define	IDC_MaxPrompt						19
	#define	IDC_StatisticMax					20
	#define	IDC_StartEndInterval				21
	#define	IDC_IntervalPrompt				22
	#define	IDC_LinePrompt						24
	#define	IDC_LineStart						25
	#define	IDC_LineEnd							26
	#define	IDC_LineInterval					27
	#define	IDC_ColumnPrompt					24
	#define	IDC_ColumnStart					28
	#define	IDC_ColumnEnd						29
	#define	IDC_ColumnInterval				30
	#define	IDC_SelectedAreaRadio			33
	#define	IDC_FeatureTransformation		34
#endif	// defined multispec_mac || defined multispec_mac_swift 
  
#if defined multispec_win
	#include "WStatisticsImageDialog.h"
#endif	// defined multispec_win


	
		// Prototypes for routines in this file that are only called by
		// other routines in this file.													

Boolean	CreateStatisticsImages (void);

void 		LoadStatImageInformationStructure (
				CovarianceStatisticsPtr			covarianceStatisticsPtr,
				SInt16								numberInputChannels, 
				SInt16								numberOutputFeatures, 
				SInt16*								featureListPtr, 
				HDoublePtr							eigenVectorPtr, 
				HDoublePtr							tempMatrixPtr,
				HDoublePtr							offsetVectorPtr, 
				SInt16								numberFeatures, 
				HChannelStatisticsPtr			inputChannelStatsPtr, 
				HSumSquaresStatisticsPtr		inputSumSquaresPtr, 
				SInt64								numberPixels, 
				HDoublePtr							outputMeansPtr,
				HDoublePtr							outputVarianceVectorPtr,
				HUCharPtr							outputHalf_imagePtr);

Boolean	LoadStatisticsImageSpecs (
				Handle								windowInfoHandle);

Boolean	StatisticsImageDialog (
				FileInfoPtr							fileInfoPtr);



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CreateStatisticsImages
//
//	Software purpose:	The purpose of this routine is to create the statistics
//							images.  
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 11/15/1991
//	Revised By:			Larry L. Biehl			Date: 08/24/2017

Boolean CreateStatisticsImages (void)

{		
	MFileInfo 							newFileInfo;
	
	CLASS_INFO_STR*					statImageInformationPtr;
	FileIOInstructionsPtr			fileIOInstructionsPtr;
	HCovarianceStatisticsPtr		tempCovarianceStatisticsPtr;
	HChannelStatisticsPtr			channelStatisticsPtr;
	HSumSquaresStatisticsPtr		totalSumSquaresStatsPtr;
	
	SInt16								*channelsPtr,
											*classPtr,
											*statFeaturePtr;
	
	UInt16*								featurePtr;
											
	Handle								totalChanStatsHandle,
											totalSumSquaresStatsHandle;
	
	SInt32								numberOutputCovarianceEntries,
											returnCode;
	
	
	UInt32								sizeOfTempCovariance,
											statChanMeanStart,
											statCovStart;
											
	
	SInt16								areaNumber,
											classStorage,
											errCode,
											index,
											lastClassIndex,
											lastFieldIndex,
											fieldNumber,
											numberFeatures,
											numberFeatureChannels,
											numberClasses,
											numberImagesToStore,
											statClassNumber,
											statImageIndex,
											stringIndex,
											totalNumberAreas;
	
	Boolean								continueFlag;
   
	CMFileStream*						baseFileStreamPtr = NULL;
	
	
			// Initialize local variables.						
			
	channelsPtr = (SInt16*)GetHandlePointer (gStatisticsImageSpecsPtr->channelsHandle);
										
	classPtr = (SInt16*)GetHandlePointer (gStatisticsImageSpecsPtr->classHandle);
	
	featurePtr = (UInt16*)GetHandlePointer (gStatisticsImageSpecsPtr->featureHandle);

   numberClasses = gStatisticsImageSpecsPtr->numberClasses;
	numberFeatureChannels = gStatisticsImageSpecsPtr->numberChannels;
	numberFeatures = gStatisticsImageSpecsPtr->numberFeatures;
	fileIOInstructionsPtr = NULL;
	
	numberOutputCovarianceEntries = (SInt32)numberFeatures * (numberFeatures+1)/2;
									
	sizeOfTempCovariance = (SInt32)numberFeatureChannels * (numberFeatureChannels+1)/2;
	if (gStatisticsImageSpecsPtr->featureTransformationFlag)
		sizeOfTempCovariance = (SInt32)numberFeatureChannels * numberFeatureChannels;
	sizeOfTempCovariance *= sizeof (double);
					
	statImageInformationPtr = NULL;
	tempCovarianceStatisticsPtr = NULL;
	continueFlag = TRUE;
	returnCode = 0;
   
			// Get the number of areas to work with.
			
	totalNumberAreas = 1;
	
	if (gStatisticsImageSpecsPtr->areaCode == kTrainingType)
		{
		if (gStatisticsImageSpecsPtr->perFieldClassCode == 1)
			totalNumberAreas = numberClasses;
			
		else if (gStatisticsImageSpecsPtr->perFieldClassCode == 2)
			totalNumberAreas = GetNumberOfAreas (classPtr, 
																numberClasses, 
																kTrainingType, 
																kIncludeClusterFields);
																
		}	// end "if (gStatisticsImageSpecsPtr->areaCode == kTrainingType)"
   
			// Show status dialog information.
			
	areaNumber = 1;
	
	LoadDItemStringNumber (kStatisticsImageStrID,
									IDS_StatisticsImage8,
									gStatusDialogPtr, 
									IDC_Status11, 
									(Str255*)&gTextString);
   
   LoadDItemValue (gStatusDialogPtr, IDC_Status3, (SInt32)areaNumber);
	LoadDItemValue (gStatusDialogPtr, IDC_Status5, (SInt32)totalNumberAreas);
		
	numberImagesToStore = 1;
   if (gStatisticsImageSpecsPtr->minMaxSettingCode >= 1)
		numberImagesToStore = totalNumberAreas;
   
			// Get memory for the statistics image information structure.
   
	statImageInformationPtr = (CLASS_INFO_STR*)
				MNewPointerClear ((UInt32)numberImagesToStore * sizeof (CLASS_INFO_STR));
	continueFlag = (statImageInformationPtr != NULL);
   
   if (continueFlag)
		{
		for (index=0; index<numberImagesToStore; index++)
			{
			statImageInformationPtr[index].fmean = NULL;
			statImageInformationPtr[index].fvar = NULL;
			statImageInformationPtr[index].half_image = NULL;
			
			}	// end "for (index=0; index<..." 
			
		for (index=0; index<numberImagesToStore; index++)
			{
			statImageInformationPtr[index].fmean = 
									(double*)MNewPointer (numberFeatures * sizeof (double));
			continueFlag = (statImageInformationPtr[index].fmean != NULL);		
					
			if (continueFlag)
				{
				statImageInformationPtr[index].fvar =
									(double*)MNewPointer (numberFeatures * sizeof (double));
				continueFlag = (statImageInformationPtr[index].fvar != NULL);
				
				}	// end "if (continueFlag)" 	
					
			if (continueFlag)
				{
				statImageInformationPtr[index].half_image = (unsigned char*)
								MNewPointer (numberOutputCovarianceEntries * sizeof (char));
				continueFlag = (statImageInformationPtr[index].half_image != NULL);
				
				}	// end "if (continueFlag)" 
				
			if (!continueFlag)
				break;
			
			}	// end "for (index=0; index<..." 
		
		}	// end "if (continueFlag)" 
   
   if (continueFlag && gStatisticsImageSpecsPtr->perFieldClassCode == 1)
		{
					// Get pointer to memory to use for class channel statistics	
					// that will be used in computing the covariance matrix and		
					// getting the data for the class mean vector.						
					
		gTempChannelStatsPtr = (ChannelStatisticsPtr)MNewPointer (
												(UInt32)numberFeatureChannels *
																		sizeof (ChannelStatistics));
		continueFlag = (gTempChannelStatsPtr != NULL);	
		
		}	// end "if (continueFlag && ...)" 
   
   if (continueFlag)
      continueFlag = SetupFeatureTransformationMemory (
										gStatisticsImageSpecsPtr->featureTransformationFlag,
										numberFeatures, 
										numberFeatureChannels, 
										&gTransformationMatrix.eigenVectorPtr, 
										&gTransformationMatrix.tempMatrixPtr,
										&gTransformationMatrix.offsetVectorPtr, 
										&gTransformationMatrix.eigenFeaturePtr,
										kLoadMatricesVectors,
										featurePtr);
								
	statFeaturePtr = (SInt16*)GetStatisticsFeatureVector (
										gStatisticsImageSpecsPtr->featureTransformationFlag,
										featurePtr);
  
			// Now load the statistics information structure if everything is
			// ok.								
	
	if (continueFlag)
		{
				// Make certain that the default volume is correct. If the project
				// statistics are being used, then write files to same folder as the
				// project file. Otherwise, write to same folder as the image file.					
				
		SignedByte handleStatus = -1;
		SInt16 vRefNum = 0;
		
      
		if (gStatisticsImageSpecsPtr->areaCode == kTrainingType)
			{
			baseFileStreamPtr = GetFileStreamPointer (gProjectInfoPtr);
			vRefNum = GetVolumeReferenceNumber (baseFileStreamPtr);
         
			
			}	// end "if (gStatisticsImageSpecsPtr->areaCode == kTrainingType)"
      
			
		if (baseFileStreamPtr == NULL || vRefNum == 0)
			{
			baseFileStreamPtr = GetFileStreamPointer (
													gStatisticsImageSpecsPtr->windowInfoHandle,
													&handleStatus);
			vRefNum = GetVolumeReferenceNumber (baseFileStreamPtr);
			
			}	// end "if (baseFileStreamPtr == NULL || vRefNum == 0)" 
			
		if (vRefNum != 0)
			{
			errCode = SetVolume (baseFileStreamPtr, kErrorMessages);
			if (errCode != noErr)									
																						return (FALSE);
																			
			}	// end "if (vRefNum != 0)"
			
				// Initialize the file information structure for the output			
				// statistics images.															
				
		InitializeFileInfoStructure ((Handle)&newFileInfo, kPointer);
		CMFileStream* newFileStreamPtr = GetFileStreamPointer (&newFileInfo);
		SetVolumeReference (baseFileStreamPtr, newFileStreamPtr);
      
		UnlockFileStream (gStatisticsImageSpecsPtr->windowInfoHandle, handleStatus);
			
		if (gStatisticsImageSpecsPtr->areaCode == kTrainingType)
			{
			if (gStatisticsImageSpecsPtr->minMaxSettingCode == 1)
				{
				if (gStatisticsImageSpecsPtr->perFieldClassCode == 1)
							// Getting covariance for selected classes.
					stringIndex = IDS_StatisticsImage9;
						
				else	// gStatisticsImageSpecsPtr->perFieldClassCode == 2
							// Getting covariance for selected fields
					stringIndex = IDS_StatisticsImage10;
						
				LoadDItemStringNumber (kStatisticsImageStrID,
												stringIndex,
												gStatusDialogPtr, 
												IDC_Status11, 
												(Str255*)&gTextString);
					
				}	// end "if (gStatisticsImageSpecsPtr->minMaxSettingCode == 1)" 
				
			for (index=0; index<numberClasses; index++)
				{
				if (gStatisticsImageSpecsPtr->perFieldClassCode == 1)
					{
					LoadDItemValue (gStatusDialogPtr, IDC_Status3, (SInt32)areaNumber);
		
					if (continueFlag && tempCovarianceStatisticsPtr == NULL)
						{
						tempCovarianceStatisticsPtr = 
													(double*)MNewPointer (sizeOfTempCovariance);
						continueFlag = (tempCovarianceStatisticsPtr != NULL);	
						
						}	// end "if (continueFlag &&..." 
					
					if (gStatisticsImageSpecsPtr->minMaxSettingCode == 2)
						{
						if (gStatisticsImageSpecsPtr->perFieldClassCode == 1)
									// Getting covariance for selected class
							stringIndex = IDS_StatisticsImage11;
								
						if (gStatisticsImageSpecsPtr->perFieldClassCode == 2)
									// Getting covariance for selected field
							stringIndex = IDS_StatisticsImage12;
						
						LoadDItemStringNumber (kStatisticsImageStrID, 
												stringIndex,
												gStatusDialogPtr, 
												IDC_Status11, 
												(Str255*)&gTextString);
									
						}	// end "if (...->minMaxSettingCode == 2)" 
								
					statClassNumber = classPtr[index] - 1;
				
							// Get the class storage number.										
							
					classStorage = gProjectInfoPtr->storageClass[statClassNumber];
				
							// Get the statistics image index.									
							
					statImageIndex = 0;
               if (gStatisticsImageSpecsPtr->minMaxSettingCode >= 1)
						statImageIndex = index;
					
							// Get the class storage index for the default name for the 
							// image.
					
					statImageInformationPtr[statImageIndex].stat_name_storage = 
																								classStorage;
				
							// Get the class covariance matrix .								
						
					if (continueFlag)
						{	
						GetTransformedClassCovarianceMatrix (
																numberFeatureChannels,
																gTempChannelStatsPtr, 
																tempCovarianceStatisticsPtr, 
																(UInt16*)statFeaturePtr, 
																statClassNumber,
																kTriangleOutputMatrix,
																kMeanCovariance,
																gTransformationMatrix.eigenVectorPtr,
																gTransformationMatrix.tempMatrixPtr,
																numberFeatures);
							
						GetDiagonalVectorFromMatrix (
													numberFeatures,
													tempCovarianceStatisticsPtr,
													statImageInformationPtr[statImageIndex].fvar, 
													kTriangleOutputMatrix);
					
						FS_make_half_STI (
											tempCovarianceStatisticsPtr,
											statImageInformationPtr[statImageIndex].fvar,
											statImageInformationPtr[statImageIndex].half_image,
											(SInt32)numberFeatures);
							
								// Get the class mean vector.											
			
						GetTransformedMeanVector (
													gTempChannelStatsPtr,
													statImageInformationPtr[statImageIndex].fmean,
													numberFeatureChannels,
													NULL,
													gTransformationMatrix.eigenVectorPtr,
													gTransformationMatrix.tempMatrixPtr,
													gTransformationMatrix.offsetVectorPtr,
													numberFeatures);
							
						}	// end "if (continueFlag)" 
					/*
							// This was used when a separate image file was created for
							// each class
					if (continueFlag && gStatisticsImageSpecsPtr->minMaxSettingCode >= 2)
						{		
								// "\pCreating image for class statistics."
						LoadDItemStringNumber (kStatisticsImageStrID, 
														IDS_StatisticsImage13,
														gStatusDialogPtr, 
														IDC_Status11, 
														(Str255*)&gTextString);
					 
								// Delete this temp matrix so that the space can be used by
								// the 'FS_make_stat_image_same_scale' routines.
									
						tempCovarianceStatisticsPtr = CheckAndDisposePtr (
																		tempCovarianceStatisticsPtr);
						
 						FS_make_stat_image_same_scale (
												statImageInformationPtr,
												(SInt32)1, 
												(SInt32)numberFeatures,
												&newFileInfo, 
												&returnCode,
												(gStatisticsImageSpecsPtr->minMaxSettingCode==3),
												gStatisticsImageSpecsPtr->userMinimum,
												gStatisticsImageSpecsPtr->userMaximum);
									
						continueFlag = (returnCode == 0);
						
						}	// end "if (continueFlag && ..." 
					*/
					areaNumber++;
								
					}	// end "if (...->perFieldClassCode == 1" 
					
				if (gStatisticsImageSpecsPtr->perFieldClassCode == 2)
					{
					lastClassIndex = -1;
					lastFieldIndex = -1;
					
					do
						{
								// Get the statistics image index.								
								
						statImageIndex = 0;
						if (gStatisticsImageSpecsPtr->minMaxSettingCode == 1)
							statImageIndex = areaNumber - 1;
						
								// Get the next field number.										
								
						fieldNumber = GetNextFieldArea (gProjectInfoPtr,
																	classPtr, 
																	numberClasses, 
																	&lastClassIndex, 
																	lastFieldIndex, 
																	kTrainingType,
																	kIncludeClusterFields);
						
						if (fieldNumber >= 0)
							{							
							LoadDItemValue (
											gStatusDialogPtr, IDC_Status3, (SInt32)areaNumber);
					
									// Get the class storage index for the default name for the 
									// image.
							
							statImageInformationPtr[statImageIndex].stat_name_storage = 
																								fieldNumber;
		
							if (continueFlag && !tempCovarianceStatisticsPtr)
								{
								tempCovarianceStatisticsPtr = 
														(double*)MNewPointer (sizeOfTempCovariance);
								continueFlag = (tempCovarianceStatisticsPtr != NULL);	
								
								}	// end "if (continueFlag &&..." 
								
							if (continueFlag)
								{
										// Get the field mean vector.								
							
								statChanMeanStart = (UInt32)fieldNumber * 
														gProjectInfoPtr->numberStatisticsChannels;
									
								statCovStart =
										fieldNumber * gProjectInfoPtr->numberCovarianceEntries;
                      
								LoadStatImageInformationStructure (
										tempCovarianceStatisticsPtr,
										gProjectInfoPtr->numberStatisticsChannels,
										numberFeatureChannels,
                              (SInt16*)featurePtr,
										gTransformationMatrix.eigenVectorPtr,
										gTransformationMatrix.tempMatrixPtr,
										gTransformationMatrix.offsetVectorPtr,
										numberFeatures, 
										&gProjectInfoPtr->fieldChanStatsPtr[statChanMeanStart], 
										&gProjectInfoPtr->fieldSumSquaresStatsPtr[statCovStart], 
										gProjectInfoPtr->fieldIdentPtr[fieldNumber].numberPixels, 
										statImageInformationPtr[statImageIndex].fmean,
										statImageInformationPtr[statImageIndex].fvar,
										statImageInformationPtr[statImageIndex].half_image);	  

								}	// end "if (continueFlag)" 
							/*
									// This was used when a separate image file was created for
									// each class

							if (continueFlag && 
												gStatisticsImageSpecsPtr->minMaxSettingCode >= 2)
								{						
										// "\pCreating image for field statistics."
								LoadDItemStringNumber (kStatisticsImageStrID, 
																IDS_StatisticsImage14,
																gStatusDialogPtr, 
																IDC_Status11, 
																(Str255*)&gTextString);
									
										// Delete this temp matrix so that the space can be used 
										// by the 'FS_make_stat_image_same_scale' routines.
									
								tempCovarianceStatisticsPtr = CheckAndDisposePtr (
																		tempCovarianceStatisticsPtr);
                        
								#ifdef NetBeansProject
									newFileInfo.fileStreamCPtr = baseFileStreamPtr;
								#endif // end "#ifdef NetBeansProject"
							
								FS_make_stat_image_same_scale (
											statImageInformationPtr, 
											(SInt32)1, 
											(SInt32)numberFeatures,
											&newFileInfo,  
											&returnCode,
											(gStatisticsImageSpecsPtr->minMaxSettingCode==3),
											gStatisticsImageSpecsPtr->userMinimum,
											gStatisticsImageSpecsPtr->userMaximum);

								continueFlag = (returnCode == 0);
								
								}	// end "if (continueFlag && ..." 
							*/
							lastFieldIndex = fieldNumber;
							areaNumber++;
							
							}	// end "if (fieldNumber >= 0)" 
						
						}	while (fieldNumber >= 0);
						
					index = numberClasses;
								
					}	// end "if (...->perFieldClassCode == 2" 
					
				}	// end "for (index=0; index<..." 	
				
			}	// end "if (gStatisticsImageSpecsPtr->areaCode == kTrainingType)"						
			
			// Get the statistics for the area if this is a selected area request.
			
		else if (gStatisticsImageSpecsPtr->areaCode == kAreaType)
			{
					// "\pGetting covariance for selected area."
			LoadDItemStringNumber (kStatisticsImageStrID, 
											IDS_StatisticsImage7,
											gStatusDialogPtr, 
											IDC_Status11, 
											(Str255*)&gTextString);
         
					
			HideStatusDialogItemSet (kStatusClassB);
			
					// Get pointer to memory to use to read an image file line into.
						 						
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
													numberFeatureChannels,
													(HUInt16Ptr)channelsPtr,
													kPackData,				// kDoNotPackData,
													kForceBISFormat,		// kDoNotForceBISFormat,
													kForceReal8Bytes,
													kDoNotAllowForThreadedIO,
													&fileIOInstructionsPtr);
			
			totalChanStatsHandle = NULL;
			totalSumSquaresStatsHandle = NULL;
				
					// Get the covariance for the selected area.		
				
			InitializeAreaDescription (&gAreaDescription, 
													gStatisticsImageSpecsPtr->lineStart, 
													gStatisticsImageSpecsPtr->lineEnd, 
													gStatisticsImageSpecsPtr->columnStart, 
													gStatisticsImageSpecsPtr->columnEnd, 
													gStatisticsImageSpecsPtr->lineInterval, 
													gStatisticsImageSpecsPtr->columnInterval,
													1,
													1,
													0);	
			
			if (continueFlag)	
				continueFlag = GetTotalSumSquares (fileIOInstructionsPtr,
																&totalChanStatsHandle, 
																&totalSumSquaresStatsHandle, 
																(UInt16*)channelsPtr, 
																numberFeatureChannels,
																NULL, 
																0,
																gStatisticsImageSpecsPtr->areaCode,
																0,
																FALSE);
			
					// Dispose of the IO buffer.		
										
			DisposeIOBufferPointers (fileIOInstructionsPtr,
												&gInputBufferPtr, 
												&gOutputBufferPtr);
		
			if (continueFlag && tempCovarianceStatisticsPtr == NULL)
				{
				tempCovarianceStatisticsPtr = (double*)MNewPointer (sizeOfTempCovariance);
				continueFlag = (tempCovarianceStatisticsPtr != NULL);	
				
				}	// end "if (continueFlag &&..." 
			
			if (continueFlag)
				{
				channelStatisticsPtr = (HChannelStatisticsPtr)GetHandlePointer (
																					totalChanStatsHandle);
														
				totalSumSquaresStatsPtr = (HCovarianceStatisticsPtr)GetHandlePointer (
																			totalSumSquaresStatsHandle);
            
            LoadStatImageInformationStructure (
								tempCovarianceStatisticsPtr,
								numberFeatureChannels,
								numberFeatureChannels,
                        NULL,		// (SInt16*)featurePtr,
											// Data is already reduced to feature set selected.
								gTransformationMatrix.eigenVectorPtr,
								gTransformationMatrix.tempMatrixPtr,
								gTransformationMatrix.offsetVectorPtr,
								numberFeatures, 
								channelStatisticsPtr, 
								totalSumSquaresStatsPtr, 
								gAreaDescription.numSamplesPerChan, 
								statImageInformationPtr->fmean,
								statImageInformationPtr->fvar,
								statImageInformationPtr->half_image);
						
				}	// end "if (continueFlag)"									
		
			UnlockAndDispose (totalChanStatsHandle);
			UnlockAndDispose (totalSumSquaresStatsHandle);
					
					// Indicate that this is a selected area statistics image.
			
			statImageInformationPtr->stat_name_storage = -1;
						
			}	// end "else if (gStatisticsImageSpecsPtr->areaCode == kAreaType)"						
			
		}	// end "if (continueFlag)" 
								
   if (continueFlag && (gStatisticsImageSpecsPtr->minMaxSettingCode >= 1 ||
											gStatisticsImageSpecsPtr->areaCode == kAreaType))
		{
		if (gStatisticsImageSpecsPtr->areaCode == kTrainingType)
			{
			if (gStatisticsImageSpecsPtr->perFieldClassCode == 1)
						// "\pCreating images for class statistics."
				stringIndex = IDS_StatisticsImage15;
					
			else	// gStatisticsImageSpecsPtr->perFieldClassCode == 2
					// "\pCreating images for field statistics."
				stringIndex = IDS_StatisticsImage16;
					
			}	// end "if (gStatisticsImageSpecsPtr->areaCode == kTrainingType)"
			
		else	// gStatisticsImageSpecsPtr->areaCode == kAreaType
			{
					// "\pCreating image for selected area."
			stringIndex = IDS_StatisticsImage17;
			
			}	// end "else gStatisticsImageSpecsPtr->areaCode == kAreaType"
			
		LoadDItemStringNumber (kStatisticsImageStrID, 
										stringIndex,
										gStatusDialogPtr, 
										IDC_Status11, 
										(Str255*)&gTextString);
					
		HideStatusDialogItemSet (kStatusClassB);
								
		tempCovarianceStatisticsPtr = CheckAndDisposePtr (tempCovarianceStatisticsPtr);
      
		FS_make_stat_image_same_scale (statImageInformationPtr,
													classPtr,
													(SInt32)numberImagesToStore,
													//channelsPtr,
													featurePtr, 
													(SInt32)numberFeatures,
													&newFileInfo,  
													&returnCode,
													gStatisticsImageSpecsPtr->areaCode,
													gStatisticsImageSpecsPtr->minMaxSettingCode,
													gStatisticsImageSpecsPtr->userMinimum,
													gStatisticsImageSpecsPtr->userMaximum);
		
		}	// end "if (continueFlag && ..." 
	
			// Release memory.															
			
	if (statImageInformationPtr != NULL)
		{
		for (index=0; index<numberImagesToStore; index++)
			{
			statImageInformationPtr[index].fmean = CheckAndDisposePtr (
															statImageInformationPtr[index].fmean);
											
			statImageInformationPtr[index].fvar = CheckAndDisposePtr (
															statImageInformationPtr[index].fvar);
											
			statImageInformationPtr[index].half_image = CheckAndDisposePtr (
														statImageInformationPtr[index].half_image);
			
			}	// end "for (index=0; index<..." 
			
		statImageInformationPtr = (CLASS_INFO_STR*)CheckAndDisposePtr (
																		(Ptr)statImageInformationPtr);
		
		}	// end "if (statImageInformationPtr != NULL)" 
		
	gTempChannelStatsPtr = (ChannelStatisticsPtr)CheckAndDisposePtr (
																			(Ptr)gTempChannelStatsPtr);
										
	tempCovarianceStatisticsPtr = CheckAndDisposePtr (tempCovarianceStatisticsPtr);
	
			// Release memory for transformation matrix information.					
	
	ReleaseFeatureTransformationMemory ();
		
	if (returnCode != 0)
		{
				// " Image Statistics Error = '%ld'\r", returnCode
		continueFlag = ListSpecifiedStringNumber (kStatisticsImageStrID,
																IDS_StatisticsImage18,
																NULL,
																gOutputForce1Code, 
																returnCode, 
																TRUE);
		
		}	// end "if (returnCode != 0)" 
		
	return (TRUE);
	
}	// end "CreateStatisticsImages"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void StatisticsImageControl
//
//	Software purpose:	The purpose of this routine is to handle the
//							"Create Statistics Image" event.  The defaults will 
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
//	Coded By:			Larry L. Biehl			Date: 11/15/1991
//	Revised By:			Larry L. Biehl			Date: 06/16/2016

void StatisticsImageControl (void)

{
	CMFileStream*						resultsFileStreamPtr;
	
	SInt16								*classPtr,
											*featurePtr,
											*channelsPtr;

	time_t								startTime;
	
	Handle								activeWindowInfoHandle;
	
	SInt16								activeImageGlobalHandleStatus,
											covarianceStatsToUse,
											listCode,
											stringIndex;
	
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
 	gStatisticsImageSpecsPtr = NULL;
 	resultsFileStreamPtr = NULL;
	
			// Get image window globals.
 			
 	if (gProjectInfoPtr != NULL)
 		{
				// Make certain the we have access to the file.						
				
		GetProjectImageFileInfo (kPrompt, kDoNotSetupGlobalInfoPointers);
		
		activeWindowInfoHandle = gProjectInfoPtr->windowInfoHandle;
	
			// Allow for case when we have a project file with no access
			// to an image file. We may still be able to create the
			// statistics image from the already computed project classes.
												
		GetImageInformationPointers (&activeImageGlobalHandleStatus,
												activeWindowInfoHandle,
												&gImageWindowInfoPtr, 
												&gImageLayerInfoPtr, 
												&gImageFileInfoPtr);
		
		}	// end "if (gProjectInfoPtr != NULL)"
		
	else	// gProjectInfoPtr == NULL
		continueFlag = SetUpActiveImageInformationGlobals (
																	&activeWindowInfoHandle,
																	&activeImageGlobalHandleStatus, 
																	kDoNotUseProject);
	
			// Set up statistics images specification structure.						
			
	if (continueFlag && LoadStatisticsImageSpecs (activeWindowInfoHandle))
		{
		if (gImageWindowInfoPtr == NULL)
																									return;
																							
		if (StatisticsImageDialog (gImageFileInfoPtr))
			{
					// Initialize some global variables pertaining to output.		
			
			gOutputTextOKFlag = TRUE;
			gOutputForce1Code = (gOutputCode | 0x0001);
			continueFlag = TRUE;
			
			featurePtr = (SInt16*)GetHandlePointer (
														gStatisticsImageSpecsPtr->featureHandle,
														kLock);
			
			channelsPtr = (SInt16*)GetHandlePointer (
														gStatisticsImageSpecsPtr->channelsHandle,
														kLock);
			
			classPtr = (SInt16*)GetHandlePointer (
														gStatisticsImageSpecsPtr->classHandle,
														kLock);
			
					// Update statistics for project if needed	
			
			if (gStatisticsImageSpecsPtr->areaCode != kAreaType)
				continueFlag = VerifyProjectStatsUpToDate (
											&gStatisticsImageSpecsPtr->numberClasses,
											//(SInt16*)gStatisticsImageSpecsPtr->classHandle,
											classPtr,
											1,
											gProjectInfoPtr->statisticsCode,
											gProjectInfoPtr->covarianceStatsToUse, 
											kSetupGlobalInfoPointers,
											NULL);
										
					// This is initialize here because it is used in the list		
					// fields used routine.														
					
			gAreaDescription.lineInterval = 1;
			gAreaDescription.columnInterval = 1;
									
			startTime = time (NULL);
 		
					// Continue statistics image if everything is okay.				
			
			if (continueFlag)
				{
						// Change cursor to watch cursor until done with process.	
					
				MSetCursor (kWait);
		
						// Force text selection to start from end of present text.	
						
				ForceTextToEnd ();
								
				}	// end "if (continueFlag)" 
					
					// List the processor name, date, time and project info.			

			covarianceStatsToUse = 0;
			listCode = kLImageInfo;
			if (gStatisticsImageSpecsPtr->areaCode == kTrainingType)
				{
				covarianceStatsToUse = gProjectInfoPtr->covarianceStatsToUse;
				listCode += kLProjectName+kLProjectImageName+kLStatType;
				
				}	// end "if (gStatisticsImageSpecsPtr->areaCode == kTrainingType)"
				
			continueFlag = ListHeaderInfo (resultsFileStreamPtr,
														listCode, 
														&gOutputForce1Code, 
														covarianceStatsToUse, 
														continueFlag);
			
					// List the Statistics Image specifications.							
						
			if (continueFlag && gStatisticsImageSpecsPtr->areaCode == kTrainingType)
				{					
				if (gStatisticsImageSpecsPtr->perFieldClassCode == 1)
							//	" Statistics Image will be created for each selected class.\r"
					stringIndex = IDS_StatisticsImage1;
				
				else	// gStatisticsImageSpecsPtr->perFieldClassCode == 2 
							//	" Statistics Image will be created for each selected field.\r"
					stringIndex = IDS_StatisticsImage2;
				
				continueFlag = ListSpecifiedStringNumber (kStatisticsImageStrID,
																		stringIndex,
																		gTextString,
																		resultsFileStreamPtr, 
																		gOutputForce1Code,
																		continueFlag);
				
				if (gStatisticsImageSpecsPtr->minMaxSettingCode == 1)
							//	" The images will be based on overall min-max values.\r"
					stringIndex = IDS_StatisticsImage3;
						
				else	// gStatisticsImageSpecsPtr->minMaxSettingCode == 2 
							//	" Each image will be based on respective statistics' min-max values.\r"
					stringIndex = IDS_StatisticsImage4;
				
				continueFlag = ListSpecifiedStringNumber (kStatisticsImageStrID,
																		stringIndex,
																		gTextString,
																		resultsFileStreamPtr, 
																		gOutputForce1Code,
																		continueFlag);
		
				continueFlag = OutputString (resultsFileStreamPtr, 
														(char*)gEndOfLine,
														0, 
														gOutputForce1Code,
														continueFlag); 	
						
						// List the classes used.													
													
				if (continueFlag)
					continueFlag =  ListClassesUsed (
														NULL,
														gImageFileInfoPtr,
														(SInt16*)classPtr,
														kListNoSymbols,
														kListNoWeights, 
														gStatisticsImageSpecsPtr->numberClasses, 
														resultsFileStreamPtr, 
														&gOutputForce1Code,
														FALSE);
		
						// List the fields/areas used.											
							
				if (continueFlag)
					continueFlag = ListProjectFieldsUsed (
														gImageFileInfoPtr,
														(SInt16*)classPtr,
														gStatisticsImageSpecsPtr->numberClasses, 
														NULL, 
														gStatisticsImageSpecsPtr->fieldTypeCode, 
														resultsFileStreamPtr, 
														&gOutputForce1Code,
														FALSE);
					
				}	// end "if (gStatisticsImageSpecsPtr->areaCode == kTrainingType)" 
							
			else	// gStatisticsImageSpecsPtr->areaCode == kAreaType 
				{
						// " Statistics Image will be created for the selected area.\r"
				continueFlag = ListSpecifiedStringNumber (kStatisticsImageStrID,
																		IDS_StatisticsImage5,
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
															gStatisticsImageSpecsPtr->lineStart,
															gStatisticsImageSpecsPtr->lineEnd,
															gStatisticsImageSpecsPtr->lineInterval, 
															continueFlag);
				
						//	"  Columns: %5ld to %5ld by %ld\r"
				continueFlag = ListSpecifiedStringNumber (
															kSharedStrID,
															IDS_Shared2,
															resultsFileStreamPtr, 
															gOutputForce1Code,
															gStatisticsImageSpecsPtr->columnStart,
															gStatisticsImageSpecsPtr->columnEnd,
															gStatisticsImageSpecsPtr->columnInterval, 
															continueFlag);
				
				}	// end "else gStatisticsImageSpecsPtr->areaCode == kAreaType" 
				
					// List the channels used.													
			
			if (continueFlag)
				{
            continueFlag = ListChannelsUsed (
											gImageFileInfoPtr,
											featurePtr,
											channelsPtr,
											gStatisticsImageSpecsPtr->numberFeatures, 
											resultsFileStreamPtr,
											&gOutputForce1Code,
											gStatisticsImageSpecsPtr->featureTransformationFlag);
				
						// List "  Output Information:"
					
            continueFlag = ListSpecifiedStringNumber (kSharedStrID, 
																		IDS_Shared8, 
																		(unsigned char*)gTextString, 
																		resultsFileStreamPtr, 
																		gOutputForce1Code, 
																		continueFlag);
				}	// end "if (continueFlag)"
			
					// Get dialog box to display statistics image status.				
			
			if (continueFlag)
				{
				gStatusDialogPtr = GetStatusDialog (kUpdateStatsInfoID, TRUE);
				
				continueFlag = (gStatusDialogPtr != NULL);
				
				}	// end "if (continueFlag)" 
				
			if (continueFlag)
				{
				if (gStatisticsImageSpecsPtr->areaCode == kTrainingType)
					LoadDItemStringNumber (kStatisticsImageStrID, 
													IDS_StatisticsImage6,
													gStatusDialogPtr, 
													IDC_Status2, 
													(Str255*)&gTextString);
								
				ShowStatusDialogItemSet (kStatusClassB);
				ShowStatusDialogItemSet (kStatusCommand);
				DrawDialog (gStatusDialogPtr);
								
				}	// end "if (continueFlag)" 
			
			if (continueFlag)
				continueFlag = CreateStatisticsImages ();
						
					// List the CPU time taken for feature extraction.			
					
			if (continueFlag)
				continueFlag = ListCPUTimeInformation (
														resultsFileStreamPtr, TRUE, startTime);
			
					// Scroll output window to the selection and adjust the 			
					// scroll bar.																	
			
			UpdateOutputWScrolls (gOutputWindow, 1, kDisplayMessage);
				
					// Dispose of status dialog box.											
			
			CloseStatusDialog (TRUE);
			
					// Change cursor back to pointer.										
						
			MInitCursor ();
				
			}	// end "if (StatisticsImageDialog ())" 
			
		}	// end "if (LoadStatisticsImageSpecs ())"  
			
				// Unlock handles.
				
	UnlockImageInformationHandles (activeImageGlobalHandleStatus,
												activeWindowInfoHandle);
	
	if (gStatisticsImageSpecsPtr != NULL)
		{	
		CheckAndUnlockHandle (gStatisticsImageSpecsPtr->classHandle);
		CheckAndUnlockHandle (gStatisticsImageSpecsPtr->fieldHandle);
		CheckAndUnlockHandle (gStatisticsImageSpecsPtr->channelsHandle);
		CheckAndUnlockHandle (gStatisticsImageSpecsPtr->featureHandle);
		CheckAndUnlockHandle (gStatisticsImageSpecsPtr->windowInfoHandle);
			
		}	// end "if (gStatisticsImageSpecsPtr != NULL)" 
		
			// Unlock memory for the create statistics images specifications.		
		
	CheckAndUnlockHandle (gNonProjProcessorSpecs.statisticsImageSpecsH);
	
	gStatisticsImageSpecsPtr = NULL;
	
	gImageWindowInfoPtr = NULL;
	gImageFileInfoPtr = NULL;	
	gImageLayerInfoPtr = NULL;
   
}	// end "StatisticsImageControl"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadStatImageInformationStructure
//
//	Software purpose:	The purpose of this routine is to load the Statistics 
//							Image Information structure that will be used by the
//							FS_make_stat_image_same_scale routines.
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:	None			
// 
// Called By:			CreateStatisticsImages
//
//	Coded By:			Larry L. Biehl			Date: 03/17/1998
//	Revised By:			Larry L. Biehl			Date: 08/23/2010

void LoadStatImageInformationStructure (
				CovarianceStatisticsPtr			covarianceStatisticsPtr, 
				SInt16								numberInputChannels, 
				SInt16								numberOutputFeatures, 
				SInt16*								featureListPtr, 
				HDoublePtr							eigenVectorPtr, 
				HDoublePtr							tempMatrixPtr,
				HDoublePtr							offsetVectorPtr, 
				SInt16								numberFeatures, 
				HChannelStatisticsPtr			inputChannelStatsPtr, 
				HSumSquaresStatisticsPtr		inputSumSquaresPtr, 
				SInt64								numberPixels, 
				HDoublePtr							outputMeansPtr,
				HDoublePtr							outputVarianceVectorPtr,
				HUCharPtr							outputHalf_imagePtr)			

{
	HSInt16Ptr featurePtr = (HSInt16Ptr)GetHandlePointer (
														gStatisticsImageSpecsPtr->featureHandle,
														kLock);
    
	GetTransformedCovarianceMatrix (numberOutputFeatures,
												covarianceStatisticsPtr,
												numberInputChannels, 
												featurePtr,
												inputChannelStatsPtr, 
												inputSumSquaresPtr, 
												numberPixels,
												kTriangleOutputMatrix,
												eigenVectorPtr,
												tempMatrixPtr,
												numberFeatures);
					
	GetTransformedMeanVector (inputChannelStatsPtr, 
										outputMeansPtr,
										numberOutputFeatures,
										featureListPtr,
										eigenVectorPtr,
										tempMatrixPtr,
										offsetVectorPtr,
										numberFeatures);
			
	GetDiagonalVectorFromMatrix (numberFeatures,
											covarianceStatisticsPtr, 
											outputVarianceVectorPtr, 
											kTriangleOutputMatrix);
				
	FS_make_half_STI (covarianceStatisticsPtr,
							outputVarianceVectorPtr,
							outputHalf_imagePtr,
							(SInt32)numberFeatures);

}	// end "LoadStatImageInformationStructure"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean LoadStatisticsImageSpecs
//
//	Software purpose:	The purpose of this routine is to load the Statistics 
//							Image Specification structure with the default set of
//							parameters.
//
//	Parameters in:				.
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			StatisticsImageControl
//
//	Coded By:			Larry L. Biehl			Date: 11/15/1991
//	Revised By:			Larry L. Biehl			Date: 12/14/2005

Boolean LoadStatisticsImageSpecs (
				Handle								windowInfoHandle)			

{
   SInt16								*classPtr,
											*featurePtr,
											*channelsPtr;

	SInt32								bytesNeeded;
	
	SInt16								index,
											lastClassIndex;
	
	Boolean								changedFlag,
											projectFlag,
											returnFlag;
   
   bytesNeeded = 0;
      
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
		
			// If a statistics image structure already exists, then check if 		
			// it is for the current window.  If not release the old structure 	
			// and initialize a new one.														
			
	if (gNonProjProcessorSpecs.statisticsImageSpecsH != NULL)
		{
				// Lock the handle to the statistics image specifications and 		
				// get a pointer to the structure.											
	
		gStatisticsImageSpecsPtr = (StatisticsImageSpecsPtr)GetHandlePointer (
							gNonProjProcessorSpecs.statisticsImageSpecsH, kLock, kMoveHi);
							
		if (gStatisticsImageSpecsPtr->windowInfoHandle != windowInfoHandle || 
							gImageWindowInfoPtr->fileInfoVersion !=
														gStatisticsImageSpecsPtr->fileInfoVersion)
			{
			ReleaseStatisticsImageSpecsMemory (
												&gNonProjProcessorSpecs.statisticsImageSpecsH);
			gStatisticsImageSpecsPtr = NULL;
			
			}	// end "if (gStatisticsImagePtr->fileInfoHandle != ..."  
			
		else	// gStatisticsImagePtr->fileInfoHandle == fileInfoHandle && ... 
			{
					// Make certain that number of channels make sense in case		
					// a project file was closed since the last time.					
					
			if (gStatisticsImageSpecsPtr->projectFlag && !projectFlag)
				{
				gStatisticsImageSpecsPtr->channelSet = kAllMenuItem;
				gStatisticsImageSpecsPtr->areaCode = kAreaType;
				
				}	// end "if (gStatisticsImagePtr->projectFlag && ..." 
				
			gStatisticsImageSpecsPtr->projectFlag = projectFlag;
		
					// Make sure that the featureTransformationFlag makes sense.
											
			if (!EigenvectorInfoExists (projectFlag))
				gStatisticsImageSpecsPtr->featureTransformationFlag = FALSE;
				
			}	// end "else gStatisticsImagePtr->fileInfoHandle ..." 
				
		}	// end "else gNonProjProcessorSpecs.statisticsImageSpecsH != NULL" 
	
	if (gNonProjProcessorSpecs.statisticsImageSpecsH == NULL)
		{
				// Set up handle to statistics image information						
			
		gNonProjProcessorSpecs.statisticsImageSpecsH = 
													MNewHandle (sizeof (StatisticsImageSpecs));
		if (gNonProjProcessorSpecs.statisticsImageSpecsH != NULL)
			{
			gStatisticsImageSpecsPtr = (StatisticsImageSpecsPtr)GetHandlePointer (
							gNonProjProcessorSpecs.statisticsImageSpecsH, kLock, kMoveHi);
			
					// Initialize the statistics image specification structure.		
			
					// Initialize the principal component specification structure.	
			
			if (projectFlag && 
					gProjectInfoPtr->numberStatisticsClasses > 0 &&
						gProjectInfoPtr->statisticsCode == kMeanCovariance)
				gStatisticsImageSpecsPtr->areaCode = kTrainingType;
			
			else	// !projectFlag || gProjectInfoPtr->...) 
				gStatisticsImageSpecsPtr->areaCode = kAreaType;
				
			gStatisticsImageSpecsPtr->channelsHandle = NULL;
			gStatisticsImageSpecsPtr->classHandle = NULL;
			gStatisticsImageSpecsPtr->featureHandle = NULL;
			gStatisticsImageSpecsPtr->fieldHandle = NULL;
			gStatisticsImageSpecsPtr->windowInfoHandle = windowInfoHandle;
			gStatisticsImageSpecsPtr->channelSet = kAllMenuItem;
			gStatisticsImageSpecsPtr->classSet = kAllMenuItem;
			gStatisticsImageSpecsPtr->columnEnd = gImageWindowInfoPtr->maxNumberColumns;
			gStatisticsImageSpecsPtr->columnInterval = 1;
			gStatisticsImageSpecsPtr->columnStart = 1;
			
			if (gImageFileInfoPtr != NULL)
				{
				gStatisticsImageSpecsPtr->userMinimum =
															gImageFileInfoPtr->minUsableDataValue;
				gStatisticsImageSpecsPtr->userMaximum =
															gImageFileInfoPtr->maxUsableDataValue;
				
				}	// end "if (gImageFileInfoPtr != NULL)"
				
			else	// gImageFileInfoPtr == NULL
				{
				gStatisticsImageSpecsPtr->userMinimum = 0;
				gStatisticsImageSpecsPtr->userMaximum = gImageWindowInfoPtr->numberBins;
				
				}	// end "else gImageFileInfoPtr == NULL"
				
			gStatisticsImageSpecsPtr->fieldSet = 0;
			gStatisticsImageSpecsPtr->fieldTypeCode = kTrainingType;
			gStatisticsImageSpecsPtr->fileInfoVersion = 
																gImageWindowInfoPtr->fileInfoVersion;
			gStatisticsImageSpecsPtr->lineEnd = gImageWindowInfoPtr->maxNumberLines;
			gStatisticsImageSpecsPtr->lineInterval = 1;
			gStatisticsImageSpecsPtr->lineStart = 1;
			gStatisticsImageSpecsPtr->minMaxSettingCode = 1;
			gStatisticsImageSpecsPtr->outputStorageType = kClassifyFileCode;
			gStatisticsImageSpecsPtr->perFieldClassCode = 1;
			
			if (projectFlag && gProjectInfoPtr != NULL)
				{
				gStatisticsImageSpecsPtr->numberChannels = 
														gProjectInfoPtr->numberStatisticsChannels;
				gStatisticsImageSpecsPtr->numberClasses =
														gProjectInfoPtr->numberStatisticsClasses;
				gStatisticsImageSpecsPtr->numberFeatures = 
														gProjectInfoPtr->numberStatisticsChannels;
												
				}	// end "if (projectFlag && gProjectInfoPtr != NULL)"
				
			else	// !projectFlag || gProjectInfoPtr == NULL
				{
				gStatisticsImageSpecsPtr->numberChannels =
																	gImageFileInfoPtr->numberChannels;
				gStatisticsImageSpecsPtr->numberClasses = 0;
				gStatisticsImageSpecsPtr->numberFeatures =
																	gImageFileInfoPtr->numberChannels;
												
				}	// end "if (projectFlag && gProjectInfoPtr != NULL)"
				
			gStatisticsImageSpecsPtr->numberFields = 0;
			gStatisticsImageSpecsPtr->featureTransformationFlag = FALSE;
			gStatisticsImageSpecsPtr->firstTime = TRUE;
			gStatisticsImageSpecsPtr->projectFlag = projectFlag;
			
			}	// end "if (gNonProjProcessorSpecs.statisticsImageSpecsH != NULL)" 
		
		else	// else gNonProjProcessorSpecs.statisticsImageSpecsH == NULL 
			returnFlag = FALSE;
			
		}	// end "if (fileInfoPtr != NULL)"  
		
			// Make sure that we can do the selected area. There may not be an image file
			// available.
		
	if (returnFlag && 
			gStatisticsImageSpecsPtr->areaCode == kAreaType &&
				gImageFileInfoPtr == NULL)
		returnFlag = FALSE;
					
			// Check memory for statistics image classes vector.					
		
	if (returnFlag && gStatisticsImageSpecsPtr->projectFlag)
		{	
		bytesNeeded =
					(SInt32)gProjectInfoPtr->numberStatisticsClasses * sizeof (SInt16);
		classPtr = (SInt16*)CheckHandleSize (&gStatisticsImageSpecsPtr->classHandle,
															&returnFlag, 
															&changedFlag, 
															bytesNeeded);
		if (changedFlag)
			gStatisticsImageSpecsPtr->classSet = kAllMenuItem;
			
		}	// end "if (returnFlag)" 
			
			// If memory is not full, check memory for statistics image				
			// feature vector pointer.															

	if (returnFlag)
		{
		bytesNeeded =
					(SInt32)gImageWindowInfoPtr->totalNumberChannels * sizeof (SInt16);
		featurePtr = (SInt16*)CheckHandleSize (&gStatisticsImageSpecsPtr->featureHandle,
															&returnFlag, 
															&changedFlag, 
															bytesNeeded);
		if (changedFlag)
			gStatisticsImageSpecsPtr->channelSet = kAllMenuItem;
			
		}	// end "if (returnFlag)" 
			
			// If memory is not full, check memory for statistics image				
			// channels vector.																	

	if (returnFlag)
		channelsPtr = (SInt16*)CheckHandleSize (
														&gStatisticsImageSpecsPtr->channelsHandle,
														&returnFlag, 
														&changedFlag, 
														bytesNeeded);
	
	if (returnFlag)
		{
				// Load selection rectangle information into statistics image		
				// specification information if the selection rectangle has 		
				// been set up																		

		GetSelectedAreaInfo (gActiveImageWindow,
									gImageWindowInfoPtr,
									&gStatisticsImageSpecsPtr->lineStart,
									&gStatisticsImageSpecsPtr->lineEnd,
									&gStatisticsImageSpecsPtr->columnStart,
									&gStatisticsImageSpecsPtr->columnEnd,
									kDontClearSelectionArea,
									kUseThreshold,
									kDontAdjustToBaseImage);
		
				// Make certain that feature and class vectors are						
				// filled in properly.															
		
		if (gStatisticsImageSpecsPtr->channelSet == kAllMenuItem)		
			LoadFeatureVector (gStatisticsImageSpecsPtr->featureTransformationFlag,
										(SInt16*)&gStatisticsImageSpecsPtr->numberChannels,
										featurePtr,
										gImageWindowInfoPtr->totalNumberChannels);
					
				// Load the statistics image channels vector to be used 		
				// by GetLineOfData.	
		
		LoadChannelsVector (kNoProject,
									gStatisticsImageSpecsPtr->featureTransformationFlag,
									gStatisticsImageSpecsPtr->numberChannels, 
									featurePtr, 
									(SInt16*)&gStatisticsImageSpecsPtr->numberChannels,
									channelsPtr);
		
		if (gStatisticsImageSpecsPtr->classSet == kAllMenuItem && 
															gStatisticsImageSpecsPtr->projectFlag)
			LoadClassVector (&gStatisticsImageSpecsPtr->numberClasses, classPtr);
			
		}	// end "if (returnFlag)" 
	
			// If unable to set up information for statistics image					
			// specifications, release memory allocated to it (if any).				
			
	else	// !returnFlag 
		{
		ReleasePrincipalCompSpecsMemory (&gNonProjProcessorSpecs.statisticsImageSpecsH);
		gStatisticsImageSpecsPtr = NULL;
		
		}	// end "else !returnFlag" 
		
	return (returnFlag);
	
}	// end "LoadStatisticsImageSpecs"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean StatisticsImageDialog
//
//	Software purpose:	The purpose of this routine is to obtain the
//							user specifications for create statistics image processor.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	True if user selects OK in dialog box
//							False if user selects Cancel in dialog box
//
// Called By:			StatisticsImageControl 
//
//	Coded By:			Larry L. Biehl			Date: 11/15/1991
//	Revised By:			Larry L. Biehl			Date: 12/16/2016
	
Boolean StatisticsImageDialog (
				FileInfoPtr							fileInfoPtr)
	
{
   
   Boolean								returnFlag = TRUE;
	
#if defined multispec_mac
	DialogSelectArea					dialogSelectArea;
	
	Rect									theBox;
	
	double								userMaximum,
											userMinimum;
	
	DialogPtr							dialogPtr;

	UserItemUPP							drawChannelsPopUp2Ptr;
	
	UInt32								localNumberClasses;
	
	UInt16								*localActiveFeaturesPtr = NULL,
											*localClassPtr = NULL,
											*localTransformFeaturesPtr = NULL,
											*localFeaturesPtr = NULL,
											*projectChannelsPtr = NULL;
	
	UInt16**								localClassPtrPtr = NULL;
								
	Handle								okHandle,
											theHandle; 
											
	MenuHandle							channelsPopUpMenu; 
	
	SInt32								theNum;
	
	SInt16								areaCode,
											channelsPopUpMenuID,
											itemHit,
											maxNumberChannels,
											minMaxSettingCode,
											selectItem,
											theType;
								
	UInt16								localActiveNumberFeatures,
											localNumberFeatures,
											localNumberTransformFeatures;
	
	Boolean								channelsAllAvailableFlag,
											featureTransformAllowedFlag,
											featureTransformationFlag,
											modalDone;

	
	dialogPtr = NULL;
	
	if (gStatisticsImageSpecsPtr->areaCode == kTrainingType)
		localClassPtrPtr = &localClassPtr;
	
	returnFlag = GetDialogLocalVectors (&localFeaturesPtr,
														&localTransformFeaturesPtr,
														localClassPtrPtr,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL);
		
			// Get the modal dialog for the statistics image specification.		
			
	if (returnFlag)
		dialogPtr = LoadRequestedDialog (kStatisticsImageDialogID, kCopyScrap, 1, 2);
	
	if (dialogPtr == NULL)
		{
		ReleaseDialogLocalVectors (localFeaturesPtr,
											localTransformFeaturesPtr,
											localClassPtr,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);
																						return (FALSE);
		
		}	// end "if (dialogPtr == NULL)"
	
			// Intialize local user item proc pointers.

	drawChannelsPopUp2Ptr = NewUserItemUPP (DrawChannelsPopUp2);
		
	StatisticsImageDialogInitialize (dialogPtr,
												gStatisticsImageSpecsPtr,
												&dialogSelectArea,
												gImageWindowInfoPtr,
												gActiveImageWindow,
												&gChannelSelection,
												&localActiveNumberFeatures,
												localFeaturesPtr,
												localTransformFeaturesPtr,
												&channelsAllAvailableFlag,
												&projectChannelsPtr,
												&maxNumberChannels,
												&gClassSelection,
												&localNumberClasses,
												localClassPtr,
												&minMaxSettingCode,
												&userMinimum,
												&userMaximum,
												&areaCode,
												&selectItem,
												&featureTransformationFlag,
												&featureTransformAllowedFlag);
	
			// Set feature/transform feature parameters  

	InitializeDialogFeatureParameters (featureTransformationFlag,
													localActiveNumberFeatures, 
													maxNumberChannels,
													gTransformationMatrix.numberFeatures,
													localFeaturesPtr,
													localTransformFeaturesPtr,
													&localNumberFeatures,
													&localNumberTransformFeatures,
													&localActiveFeaturesPtr);
	
			// Save handle for the OK button for use later.								
			
	GetDialogItem (dialogPtr, 1, &theType, &okHandle, &theBox);
	
	if (channelsAllAvailableFlag)
		{
		SetDialogItemDrawRoutine (dialogPtr, 4, drawChannelsPopUp2Ptr);
		channelsPopUpMenu = gPopUpAllAvailableSubsetMenu;
		channelsPopUpMenuID = kPopUpAllAvailableSubsetMenuID;
		
		if (localNumberClasses > 0)
			SetDialogItemDrawRoutine (dialogPtr, 7, gDrawDialogClassPopUpPtr);
		
		else	// numberClasses <= 0
			{
			SetDLogControlHilite (dialogPtr, 6, 255);
			
			}	// end "else numberClasses <= 0"
		
		}	// end "channelsAllAvailableFlag"
		
	else	// !channelsAllAvailableFlag
		{
		SetDialogItemDrawRoutine (dialogPtr, 4, gDrawChannelsPopUpPtr);
		channelsPopUpMenu = gPopUpAllSubsetMenu;
		channelsPopUpMenuID = kPopUpAllSubsetMenuID;
		
		}	// end "else !channelsAllAvailableFlag"
		
			// Not implemented.
					
	HideDialogItems (dialogPtr, 8, 9);
	HideDialogItems (dialogPtr, 31, 32);

	if (gStatisticsImageSpecsPtr->areaCode == kTrainingType)
		{
		SetDialogItemDrawRoutine (dialogPtr, 4, drawChannelsPopUp2Ptr);
		channelsPopUpMenu = gPopUpAllAvailableSubsetMenu;
		channelsPopUpMenuID = kPopUpAllAvailableSubsetMenuID;	

		SetDialogItemDrawRoutine (dialogPtr, 7, gDrawDialogClassPopUpPtr);

		}	// end "if (gStatisticsImageSpecsPtr->areaCode == kTrainingType)"
		
	else	// gStatisticsImageSpecsPtr->areaCode != kTrainingType
		{
		SetDialogItemDrawRoutine (dialogPtr, 4, gDrawChannelsPopUpPtr);
		channelsPopUpMenu = gPopUpAllSubsetMenu;
		channelsPopUpMenuID = kPopUpAllSubsetMenuID;
		
		}	// end "else gStatisticsImageSpecsPtr->areaCode != kTrainingType"

			// Center the dialog and then show it.											
			
	ShowDialogWindow (dialogPtr, kStatisticsImageDialogID, kSetUpDFilterTable);
					
	gDialogItemDescriptorPtr[18] = kDItemMinus;
	gDialogItemDescriptorPtr[20] = kDItemMinus;
	
			// Set default text selection to first edit text item						
	
	if (selectItem != 0)	
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
				case 4:		// Channels 
					itemHit = StandardPopUpMenu (dialogPtr, 
															3, 
															4, 
															channelsPopUpMenu, 
															gChannelSelection, 
															channelsPopUpMenuID);
					if (itemHit == kSubsetMenuItem)
						{
								// Subset of channels to be used.							
								
						HiliteControl ((ControlHandle)okHandle, 255);
						ChannelsDialog ((SInt16*)&localActiveNumberFeatures,
												(SInt16*)localActiveFeaturesPtr,
												gImageLayerInfoPtr,
												fileInfoPtr,
												1,
												featureTransformationFlag,
												(SInt16*)projectChannelsPtr,
												maxNumberChannels,
												gChannelSelection);
						HiliteControl ((ControlHandle)okHandle, 0);
						/*
						if (featureTransformationFlag && 
								(localActiveNumberFeatures ==
												gTransformationMatrix.numberFeatures))
							itemHit = 1;
													
						if (!featureTransformationFlag &&
								(localActiveNumberFeatures == maxNumberChannels))
							itemHit = 1;
						*/
						}	// end "if (itemHit == kSubsetMenuItem)" 
					
					if (itemHit != 0)
						{
						gChannelSelection = UpdateDialogNumberFeatureParameters (
															featureTransformationFlag, 
															&localActiveNumberFeatures,
															&localNumberFeatures,
															maxNumberChannels,
															&localNumberTransformFeatures,
															gTransformationMatrix.numberFeatures,
															itemHit);
						//gChannelSelection = itemHit;
	
								// Make certain that the correct label is drawn in the	
								// channel pop up box.												
					
						InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
						
						}
					break;
					
				case 6:		// check button for "Classes" 
					SetControlValue ((ControlHandle)theHandle, 1);
					SetDLogControl (dialogPtr, 33, 0);
					ShowDialogItem (dialogPtr, 7);
					ShowDialogItems (dialogPtr, 10, 12);
					HideDialogItems (dialogPtr, 21, 30);
					break;
					
				case 7:		// Classes 
					itemHit = StandardPopUpMenu (dialogPtr, 
															6, 
															7, 
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
	
							// Make certain that the correct label is drawn in the	
							// class pop up box.													
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
					
				case 9:		// Field selection.												
					break;
					
				case 11:		// Create statistics image for each selected class. 
				case 12:		// Create statistics image for each selected field. 
					SetDLogControl (dialogPtr, 11, (itemHit==11));
					SetDLogControl (dialogPtr, 12, (itemHit==12));
					break;
					
				case 14:		// Use overall min/max values. 
				case 15:		// Use individual min/max values. 
				case 16:		// User specified min/max values. 
					SetDLogControl (dialogPtr, 14, (itemHit==14));
					SetDLogControl (dialogPtr, 15, (itemHit==15));
					SetDLogControl (dialogPtr, 16, (itemHit==16));
					
					if (itemHit == 16)
						{
						ShowDialogItems (dialogPtr, 17, 20);
						SelectDialogItemText (dialogPtr, 18, 0, INT16_MAX);
						
						}	// end "if (itemHit == 16)"
						
					else	// itemHit != 16
						{
						HideDialogItems (dialogPtr, 17, 20);
						
						}	// end "else itemHit != 16"
					break;
					
				case 18:				// User minimum 
				case 20:				// User maximum
					break;
					
				case 23:				// Entire area to selected area switch.			
				case 25:				//	 lineStart  
				case 26:				//	 lineEnd  
				case 27:				//	 lineInterval  	
				case 28:				//	 columnStart  
				case 29:				//	 columnEnd  
				case 30:				//	 columnInterval 
				case 36:				// Entire area to selected area switch. (Appearance Manager) 
					DialogLineColumnHits (&dialogSelectArea,
													dialogPtr, 
													itemHit,
													theHandle,
													theNum);
					break;
					
				case 33:		// Create statistics image for selected area. 
					SetControlValue ((ControlHandle)theHandle, 1);
					SetDLogControl (dialogPtr, 6, 0);
					HideDialogItems (dialogPtr, 7, 12);
					ShowDialogItems (dialogPtr, 21, 30);
					SelectDialogItemText (dialogPtr, 25, 0, INT16_MAX);
					break;
						
				case 34:		// Feature transformation flag. 
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					featureTransformationFlag = !featureTransformationFlag;
					CheckFeatureTransformationDialog (dialogPtr,
																	featureTransformAllowedFlag,
																	34, 
																	IDC_ChannelPrompt, 
																	&featureTransformationFlag);
					//InvalWindowRect (GetDialogWindow (dialogPtr), &theChannelPopupBox);
					break;
						
				}	// end "switch (itemHit)"
				
			}	// end "if (itemHit > 2)" 
			
		else	// itemHit <= 2 
			{	
					// If item hit is 1, check if line-column values make 			
					// sense.  If they don't, sound an alert and make item hit 		
					// equal to 0 to allow user to make changes.							
					
			if (itemHit == 1)
				itemHit = CheckLineColValues (
										&dialogSelectArea,
										gStatisticsImageSpecsPtr->lineStart,
										gStatisticsImageSpecsPtr->columnStart,
										dialogPtr);
					
					// Verify that the minimum and maximum values make sense.
										
			if (itemHit == 1 && GetDLogControl (dialogPtr, 16))
				{
				if (GetDItemRealValue (dialogPtr, 18) >= GetDItemRealValue (dialogPtr, 20))
					{
					Alert (kErrorAlert2ID, NIL);
					SelectDialogItemText (dialogPtr, 18, 0, SHRT_MAX);
					itemHit = 0;
					
					}	// end "if (GetDItemValue (dialogPtr, 18) >= ..."
					
				}	// end "if (itemHit == 1 && ..."
										
			if (itemHit == 1)
				{
				modalDone = TRUE;
				
				StatisticsImageDialogOK (dialogPtr,
													gStatisticsImageSpecsPtr,
													&dialogSelectArea,
													GetDLogControl (dialogPtr, 6),
													GetDLogControl (dialogPtr, 33),
													gChannelSelection,
													featureTransformationFlag,
													(SInt16*)localActiveFeaturesPtr,
													localActiveNumberFeatures,
													maxNumberChannels,
													gClassSelection,
													localNumberClasses,
													(SInt16*)localClassPtr,
													GetDLogControl (dialogPtr, 11),
													GetDLogControl (dialogPtr, 12),
													GetDLogControl (dialogPtr, 14),
													GetDLogControl (dialogPtr, 15),
													GetDLogControl (dialogPtr, 16),
													GetDItemRealValue (dialogPtr, 18),
													GetDItemRealValue (dialogPtr, 20));

				returnFlag = TRUE;

				}	// end if (itemHit == 1) 
			
			if (itemHit == 2)
				{
				modalDone = TRUE;
				returnFlag = FALSE;
				
				}	// end "if (itemHit == 2)" 
				
			}	// end "else itemHit <= 2" 
				
		}	while (!modalDone);
		
	DisposeUserItemUPP (drawChannelsPopUp2Ptr);
		
	CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
#endif	// defined multispec_mac

	#if defined multispec_win   
		CMStatImageDialog*		dialogPtr = NULL;
		
		TRY
			{ 
			dialogPtr = new CMStatImageDialog ();
			
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
		try
			{
			CMStatImageDialog*   dialogPtr = NULL;
		
			dialogPtr = new CMStatImageDialog (NULL);
		
			returnFlag = dialogPtr->DoDialog (); 
		
			delete dialogPtr;
			
			}
		catch (int e)
			{
			MemoryMessage (0, kObjectMessage);
			returnFlag = FALSE;
			}
	#endif

   return returnFlag;
	
}	// end "StatisticsImageDialog"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean StatisticsImageDialogInitialize
//
//	Software purpose:	The purpose of this routine is to initialize the variables to be 
//							used in the statistics image dialog to be presented to the user.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	True if user selects OK in dialog box
//							False if user selects Cancel in dialog box
//
// Called By:			StatisticsImageDialog 
//
//	Coded By:			Larry L. Biehl			Date: 03/30/2014
//	Revised By:			Larry L. Biehl			Date: 11/11/2019

void StatisticsImageDialogInitialize (
				DialogPtr							dialogPtr,
				StatisticsImageSpecsPtr			statisticsImageSpecsPtr,
				DialogSelectArea*					dialogSelectAreaPtr,
				WindowInfoPtr						imageWindowInfoPtr,
				WindowPtr							activeImageWindow,
				SInt16*								channelSelectionPtr,
				UInt16*								localNumberChannelsPtr,
				UInt16*								localFeaturesPtr, 
				UInt16*								localTransformFeaturesPtr,
				Boolean*								channelsAllAvailableFlagPtr,
				UInt16**								projectChannelsPtrPtr,
				SInt16*								maxNumberChannelsPtr,
				SInt16*								classSelectionPtr,
				UInt32*								localNumberClassesPtr,
				UInt16*								localClassPtr,
				SInt16*								minMaxSettingCodePtr,
				double*								userMinimumPtr,
				double*								userMaximumPtr,
				SInt16*								areaCodePtr,
				SInt16*								perFieldClassCodePtr,
				SInt16*								selectItemPtr,
				Boolean*								featureTransformationFlagPtr,
				Boolean*								featureTransformAllowedFlagPtr)

{	
	SInt16								eigenSource,
											entireIconItem;
										
	UInt16								numberEigenvectors;
   

			// Load the dialog local vectors

	LoadDialogLocalVectors (localFeaturesPtr,
										localTransformFeaturesPtr,
										statisticsImageSpecsPtr->featureHandle,
										statisticsImageSpecsPtr->numberChannels,
										localClassPtr,
										statisticsImageSpecsPtr->classHandle,
										statisticsImageSpecsPtr->numberClasses,
										NULL,
										NULL,
										0,
										NULL,
										NULL,
										0,
										NULL,
										NULL,
										0,
										NULL);
	
			// Initialize selected area structure.
	
	entireIconItem = 23;
	if (gAppearanceManagerFlag)
		entireIconItem = 36;
	
			// Initialize selected area structure.				
	
	InitializeDialogSelectArea (dialogSelectAreaPtr,
											imageWindowInfoPtr,
											activeImageWindow,
											statisticsImageSpecsPtr->columnStart,
											statisticsImageSpecsPtr->columnEnd,
											statisticsImageSpecsPtr->columnInterval,
											statisticsImageSpecsPtr->lineStart,
											statisticsImageSpecsPtr->lineEnd,
											statisticsImageSpecsPtr->lineInterval,
											25,
											entireIconItem,
											kDontAdjustToBaseImage);
	
	*localNumberClassesPtr = statisticsImageSpecsPtr->numberClasses;
	
			//	Channels to use. Make all channels the default													
	
	*channelSelectionPtr = statisticsImageSpecsPtr->channelSet;
	*localNumberChannelsPtr = statisticsImageSpecsPtr->numberFeatures;
	
	*areaCodePtr = statisticsImageSpecsPtr->areaCode;
	*perFieldClassCodePtr = statisticsImageSpecsPtr->perFieldClassCode;
	
	if (statisticsImageSpecsPtr->projectFlag)
		{
		*channelsAllAvailableFlagPtr = TRUE;
	
				//	Classes to use. Make all classes the default													
					
		SetDLogControl (dialogPtr,
								IDC_ClassesRadio,
								(statisticsImageSpecsPtr->areaCode == kTrainingType));
         
		if (gProjectInfoPtr->keepClassStatsOnlyFlag)
			SetDLogControlHilite (dialogPtr, IDC_SelectedFieldRadio, 255);
								
		*classSelectionPtr = statisticsImageSpecsPtr->classSet;
	
		if (statisticsImageSpecsPtr->areaCode == kTrainingType)
			{
			ShowDialogItem (dialogPtr, IDC_ClassCombo);
			ShowDialogItem (dialogPtr, IDC_StatPrompt);
			ShowDialogItem (dialogPtr, IDC_SelectedClassRadio);
			ShowDialogItem (dialogPtr, IDC_SelectedFieldRadio);
			
			}	// end "if (statisticsImageSpecsPtr->areaCode == kTrainingType)"
			
		else	// statisticsImageSpecsPtr->areaCode != kTrainingType"
			{
			HideDialogItem (dialogPtr, IDC_ClassCombo);
			HideDialogItem (dialogPtr, IDC_StatPrompt);
			HideDialogItem (dialogPtr, IDC_SelectedClassRadio);
			HideDialogItem (dialogPtr, IDC_SelectedFieldRadio);
			
			}	// end "else statisticsImageSpecsPtr->areaCode != kTrainingType"
				
				// Set control for creating one image for each class.						
				
		SetDLogControl (dialogPtr,
								IDC_SelectedClassRadio,
								statisticsImageSpecsPtr->perFieldClassCode == 1);
		
				// Set control for creating one image for each field.						
				
		SetDLogControl (dialogPtr,
								IDC_SelectedFieldRadio,
								statisticsImageSpecsPtr->perFieldClassCode == 2);
									
		*projectChannelsPtrPtr = (UInt16*)gProjectInfoPtr->channelsPtr;
		*maxNumberChannelsPtr = gProjectInfoPtr->numberStatisticsChannels;
									
		}	// end "if (statisticsImageSpecsPtr->areaCode == kTrainingType)"
		
	else	// statisticsImageSpecsPtr->areaCode != kTrainingType
		{
		*channelsAllAvailableFlagPtr = FALSE;
		
		HideDialogItem (dialogPtr, IDC_ClassesRadio);
		HideDialogItem (dialogPtr, IDC_ClassCombo);
		HideDialogItem (dialogPtr, IDC_StatPrompt);
		HideDialogItem (dialogPtr, IDC_SelectedClassRadio);
		HideDialogItem (dialogPtr, IDC_SelectedFieldRadio);
									
		*projectChannelsPtrPtr = NULL;
		*maxNumberChannelsPtr = imageWindowInfoPtr->totalNumberChannels;
									
		}	// end "else gProjectInfoPtr == NULL || ..."
	
			// Set control bullet for "Use overall min/max".							
			
	*minMaxSettingCodePtr = statisticsImageSpecsPtr->minMaxSettingCode;
	SetDLogControl (dialogPtr,
							IDC_OverallRadio,
							(statisticsImageSpecsPtr->minMaxSettingCode == 1));
	
			// Set control bullet for "Use individual min/max".						
			
	SetDLogControl (dialogPtr,
							IDC_IndividualRadio,
							(statisticsImageSpecsPtr->minMaxSettingCode == 2));
	
			// Set control bullet for "User defined min/max".						
			
	SetDLogControl (dialogPtr,
							IDC_UserSettingRadio,
							(statisticsImageSpecsPtr->minMaxSettingCode == 3));
								
	if (statisticsImageSpecsPtr->minMaxSettingCode == 3)
		{
		ShowDialogItem (dialogPtr, IDC_MinPrompt);
		ShowDialogItem (dialogPtr, IDC_StatisticMin);
		ShowDialogItem (dialogPtr, IDC_MaxPrompt);
		ShowDialogItem (dialogPtr, IDC_StatisticMax);
		
		}	// end "if (statisticsImageSpecsPtr->minMaxSettingCode == 3)"
		
	else	// statisticsImageSpecsPtr->minMaxSettingCode != 3
		{
		HideDialogItem (dialogPtr, IDC_MinPrompt);
		HideDialogItem (dialogPtr, IDC_StatisticMin);
		HideDialogItem (dialogPtr, IDC_MaxPrompt);
		HideDialogItem (dialogPtr, IDC_StatisticMax);
		
		}	// end "else statisticsImageSpecsPtr->minMaxSettingCode != 3"
		
			// Set the current user specified min and max values.
			
	*userMinimumPtr = statisticsImageSpecsPtr->userMinimum;
	*userMaximumPtr = statisticsImageSpecsPtr->userMaximum;
	LoadDItemRealValue (
					dialogPtr, IDC_StatisticMin, statisticsImageSpecsPtr->userMinimum, 1);
	LoadDItemRealValue (
					dialogPtr, IDC_StatisticMax, statisticsImageSpecsPtr->userMaximum, 1);

			// To entire image icon.															
			//	Selected Statistics Image area												
			
	LoadLineColumnItems (dialogSelectAreaPtr, 
								dialogPtr,
								kInitializeLineColumnValues, 
								kIntervalEditBoxesExist,
								1);

			// Set radio button for area.														
			
	SetDLogControl (dialogPtr,
							IDC_SelectedAreaRadio,
							(statisticsImageSpecsPtr->areaCode == kAreaType));
	
	if (gImageFileInfoPtr != NULL)
		{
		#if defined multispec_wx
			ShowDialogItem (dialogPtr, IDC_LineColFrame);
			ShowDialogItem (dialogPtr, IDC_StartPrompt);
			ShowDialogItem (dialogPtr, IDC_EndPrompt);
			ShowDialogItem (dialogPtr, IDC_IntervalPrompt);
		#endif
		
		#if defined multispec_mac
			ShowDialogItem (dialogPtr, IDC_StartEndInterval);
			ShowDialogItem (dialogPtr, IDC_IntervalPrompt);
		#endif
		
		#if defined multispec_win
			ShowDialogItem (dialogPtr, IDC_StartEndInterval);
		#endif
		
		ShowDialogItem (dialogPtr, entireIconItem);
		ShowDialogItem (dialogPtr, IDC_LinePrompt);
		ShowDialogItem (dialogPtr, IDC_LineStart);
		ShowDialogItem (dialogPtr, IDC_LineEnd);
		ShowDialogItem (dialogPtr, IDC_LineInterval);
		ShowDialogItem (dialogPtr, IDC_ColumnPrompt);
		ShowDialogItem (dialogPtr, IDC_ColumnStart);
		ShowDialogItem (dialogPtr, IDC_ColumnEnd);
		ShowDialogItem (dialogPtr, IDC_ColumnInterval);
		*selectItemPtr = IDC_LineStart;
		
		}	// end "if (statisticsImageSpecsPtr->areaCode == kAreaType)" 
					
	else	// statisticsImageSpecsPtr->areaCode != kAreaType 
		{
		HideDialogItem (dialogPtr, IDC_SelectedAreaRadio);
		
		#if defined multispec_wx
			HideDialogItem (dialogPtr, IDC_LineColFrame);
			HideDialogItem (dialogPtr, IDC_StartPrompt);
			HideDialogItem (dialogPtr, IDC_EndPrompt);
			HideDialogItem (dialogPtr, IDC_IntervalPrompt);
		#endif
		
		#if defined multispec_mac
			HideDialogItem (dialogPtr, IDC_StartEndInterval);
			HideDialogItem (dialogPtr, IDC_IntervalPrompt);
		#endif
		
		#if defined multispec_win
			HideDialogItem (dialogPtr, IDC_StartEndInterval);
		#endif
		
		HideDialogItem (dialogPtr, entireIconItem);
		HideDialogItem (dialogPtr, IDC_LinePrompt);
		HideDialogItem (dialogPtr, IDC_LineStart);
		HideDialogItem (dialogPtr, IDC_LineEnd);
		HideDialogItem (dialogPtr, IDC_LineInterval);
		HideDialogItem (dialogPtr, IDC_ColumnPrompt);
		HideDialogItem (dialogPtr, IDC_ColumnStart);
		HideDialogItem (dialogPtr, IDC_ColumnEnd);
		HideDialogItem (dialogPtr, IDC_ColumnInterval);
		*selectItemPtr = IDC_StatisticMin;
		
		}	// end "else statisticsImageSpecsPtr->areaCode != kAreaType" 
				
			// Set the check box for the feature transformation option.				
			
	EigenvectorInfoExists (kProject, &eigenSource, &numberEigenvectors);
	*featureTransformAllowedFlagPtr = (numberEigenvectors > 0);
	if (numberEigenvectors <= 0)
		{		
		*featureTransformationFlagPtr = FALSE;
		HideDialogItem (dialogPtr, IDC_FeatureTransformation);
		
		}	// end "if (numberEigenvectors <= 0)" 
		
	else	// numberEigenvectors > 0 
		*featureTransformationFlagPtr = 
									statisticsImageSpecsPtr->featureTransformationFlag;
									
	CheckFeatureTransformationDialog (dialogPtr,
													*featureTransformAllowedFlagPtr,
													IDC_FeatureTransformation,
													IDC_ChannelPrompt,
													featureTransformationFlagPtr);

}	// end "StatisticsImageDialogInitialize"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean StatisticsImageDialogOK
//
//	Software purpose:	The purpose of this routine is to load the variable settings
//							that the user selected into the statistics image structure
//							ready for processing.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	True if user selects OK in dialog box
//							False if user selects Cancel in dialog box
//
// Called By:			StatisticsImageDialog 
//
//	Coded By:			Larry L. Biehl			Date: 03/30/2014
//	Revised By:			Larry L. Biehl			Date: 10/09/2019

void StatisticsImageDialogOK (
				DialogPtr							dialogPtr,
				StatisticsImageSpecsPtr			statisticsImageSpecsPtr,
				DialogSelectArea*					dialogSelectAreaPtr,
				SInt16								channelSelection,
				Boolean								featureTransformationFlag,
				SInt16*								featurePtr,
            SInt16                        localNumberFeatures,
				SInt16								maxNumberChannels,
				SInt16								classSelection,
            SInt32                        localNumberClasses,
				SInt16*								classListPtr,
				SInt16								areaCode,
				SInt16								perFieldClassCode,
				SInt16								overallMinMaxCode,
				SInt16								individualMinMaxCode,
				SInt16								userMinMaxCode,
				double								userMinimum,
				double								userMaximum)

{   
   LoadProcessorVectorsFromDialogLocalVectors (
											channelSelection,
											featureTransformationFlag,
											maxNumberChannels,
											localNumberFeatures,
											(UInt16*)featurePtr,
											&statisticsImageSpecsPtr->channelSet,
											(UInt16*)&statisticsImageSpecsPtr->numberFeatures,
											statisticsImageSpecsPtr->featureHandle,
											(UInt16*)&statisticsImageSpecsPtr->numberChannels,
											statisticsImageSpecsPtr->channelsHandle,
											classSelection,
											localNumberClasses,
											(UInt16*)classListPtr,
											&statisticsImageSpecsPtr->classSet,
											&statisticsImageSpecsPtr->numberClasses,
											statisticsImageSpecsPtr->classHandle,
											0,
											NULL,
											NULL,
											NULL,
											0,
											NULL,
											NULL);
	
	statisticsImageSpecsPtr->areaCode = areaCode;
	
			// Image for class or field statistics.		
	
	statisticsImageSpecsPtr->perFieldClassCode = perFieldClassCode;
										
			// Minimum/Maximum settings to use.				
	
	if (overallMinMaxCode != 0)
		statisticsImageSpecsPtr->minMaxSettingCode = 1;
	
	if (individualMinMaxCode != 0)
		statisticsImageSpecsPtr->minMaxSettingCode = 2;
	
	if (userMinMaxCode != 0)
		statisticsImageSpecsPtr->minMaxSettingCode = 3;

			// User specified minimum and maximum.						
			
	statisticsImageSpecsPtr->userMinimum = userMinimum;
	statisticsImageSpecsPtr->userMaximum = userMaximum;
	
			// Selected area.												
	
	statisticsImageSpecsPtr->lineStart = dialogSelectAreaPtr->lineStart;
	statisticsImageSpecsPtr->lineEnd = dialogSelectAreaPtr->lineEnd;
	statisticsImageSpecsPtr->lineInterval = dialogSelectAreaPtr->lineInterval;
	statisticsImageSpecsPtr->columnStart = dialogSelectAreaPtr->columnStart;
	statisticsImageSpecsPtr->columnEnd = dialogSelectAreaPtr->columnEnd;
	statisticsImageSpecsPtr->columnInterval = dialogSelectAreaPtr->columnInterval;
	
			// Feature transformation option.									
			
	statisticsImageSpecsPtr->featureTransformationFlag = featureTransformationFlag;

}	// end "StatisticsImageDialogOK"
