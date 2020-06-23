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
//	File:						SOther.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			05/31/2020
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	This file contains routines for listing the image
//								description, evaluating the covariance matrices and 
//								evaluating the transformation matrix.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#if defined multispec_wx
   #include "xEvaluateCovariancesDialog.h"
   #include "xEvaluateTransformationDialog.h"
#endif // defined multispec_wx
	
#if defined multispec_mac || defined multispec_mac_swift
	#define IDC_ListEigenvalues		4
	#define IDC_ListTransform			5
	#define IDC_CheckTransform			6
	#define IDC_ListCheckMatrix		7
#endif	// defined multispec_mac || defined multispec_mac_swift

#if defined multispec_win 
	#include "WImageView.h"
	#include "WEvaluateCovarianceDialog.h"
	#include "WEvaluateTransformationDialog.h"
#endif	// defined multispec_win	

#define	kDoNotIncludeUTM						0
#define	kIncludeUTM								1



			// Prototype descriptions for routines in this file that are only		
			// called by routines in this file.

Boolean EvaluateCovariancesDialog (
				EvaluateCovarianceSpecsPtr		evaluateCovarianceSpecsPtr, 
				FileInfoPtr							fileInfoPtr);

Boolean EvaluateTransformationDialog (
				EvaluateTransformSpecsPtr		evaluateTransformSpecsPtr);

void ListCovarianceMatrix (
				HDoublePtr 							covariancePtr, 
				SInt16								numberChannels,
				SInt16*								featurePtr,
				SInt16*								channelsPtr,
				SInt16								matrixType,
				Boolean								transformationFlag);

void ListInstrumentName (
				FileInfoPtr							fileInfoPtr);
								
Boolean ListLARSYSMISTHeaderInformation (
				FileInfoPtr 						fileInfoPtr);
							
Boolean ListMapParameters (
				FileInfoPtr							fileInfoPtr,
				Boolean								listAllInformationFlag);

Boolean LoadEvaluateCovariancesSpecs (
				FileInfoPtr							fileInfoPtr);

Boolean LoadEvaluateTransformationSpecs (void);



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void EvaluateCovariancesControl
//
//	Software purpose:	The purpose of this routine is to control the user
//							requests for displaying classification results.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			OnProcUtilCheckCovariances in xMainFrame.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/14/1988
//	Revised By:			Larry L. Biehl			Date: 05/03/2019

void EvaluateCovariancesControl (void)

{     
	double								determinant,
											error,
											logDeterminant,
											maxError,
											theValue;

	time_t								endTime,
											initialStartTime,
											startTime;
											                          
	HChannelStatisticsPtr			classChannelStatsPtr;
	FileInfoPtr							fileInfoPtr;
			 									
	HDoublePtr							invertedCovPtr,
											outputMatrixPtr,
											pivotPtr,
											savedOutputMatrixPtr,
											tempMatrixPtr,
											utilityCovPtr;
	
	SInt16								*indexColPtr,
				 							*indexRowPtr,
				 							*ipvotPtr,
											*statFeaturePtr;
											
	EvaluateCovarianceSpecsPtr		evaluateCovarianceSpecsPtr; 
	
	UInt32								bytesNeeded,
											chan1,
											chan2,
											index,
											squareMatrixBytesNeeded; 
	
	SInt16								classStorage,
											numberFeatureChannels,
											statClassNumber,
											strLength;
												
	UInt16								classIndex,
											numberFeatures;
											
	Boolean								continueFlag,
											eigenvectorMethodFlag = FALSE;
	
		
			// If spare memory had to be used to load code resources, then exit	
			// routine.																			
			
	if (gMemoryTypeNeeded < 0)
																									return;
																							
			// Code resources loaded okay, so set memory flag back for non-Code	
			// resources.																			
			
	gMemoryTypeNeeded = 0;
	
	if (gProjectInfoPtr == NULL)
																									return;
	
			// Initialize some local variables.												
	
	utilityCovPtr = 	NULL; 
	outputMatrixPtr = NULL;
	classChannelStatsPtr = NULL;
	tempMatrixPtr = NULL;
	
	invertedCovPtr = 	NULL;
	pivotPtr = 			NULL;
	indexRowPtr =		NULL;
	indexColPtr = 		NULL;
	ipvotPtr = 			NULL;

			// Force text selection to start from end of present text.				
					
	ForceTextToEnd ();
	
			// Check handle to image file information.  If handle to image			
			// file information doesn't exists. Find the	image file and get		
			// the information for it. Do not prompt for the information since
			// it is not required for this function to work. The only information
			// that it is needed for is the channel descriptions in the channel
			// list.															
						
	GetProjectImageFileInfo (kDoNotPrompt, kSetupGlobalInfoPointers);
	
			// Lock handle to file information and get pointer to it.				
		
	fileInfoPtr = gImageFileInfoPtr;
	
			// Set up evaluate covariance specification structure.					
	
	if (LoadEvaluateCovariancesSpecs (fileInfoPtr))
		{
		evaluateCovarianceSpecsPtr = (EvaluateCovarianceSpecsPtr)GetHandlePointer (
													gProjectInfoPtr->evalCovarianceSpecsHandle);

		continueFlag = EvaluateCovariancesDialog (evaluateCovarianceSpecsPtr, 
																	fileInfoPtr);
			
		if (continueFlag)
			{
					// Initialize some global variables pertaining to output.		
			
			gOutputTextOKFlag = TRUE;
			gOutputCode = 1;
			gOutputForce1Code = gOutputCode;
				
					// Update statistics for project if needed.	
						
			continueFlag = VerifyProjectStatsUpToDate (
													&evaluateCovarianceSpecsPtr->numberClasses,
													evaluateCovarianceSpecsPtr->classPtr,
													1,
													gProjectInfoPtr->statisticsCode,
													kNoStatisticsUsed,
													kSetupGlobalInfoPointersIfCan,
													NULL);
									
			}	// end "if (continueFlag)" 
										
				// Get fileInfoPtr again in case it was changed.
		
		fileInfoPtr = gImageFileInfoPtr;					
			
		if (continueFlag)
			{
					// Change cursor to watch cursor until done with process.		
		
			MSetCursor (kWait);
				
					// List the processor name, date and time.							

			continueFlag = ListHeaderInfo (NULL, 
														kLStatType, 
														&gOutputForce1Code, 
														gProjectInfoPtr->covarianceStatsToUse, 
														continueFlag);

					// Initialize local variables												
				
			numberFeatureChannels = evaluateCovarianceSpecsPtr->numberChannels;
			numberFeatures = evaluateCovarianceSpecsPtr->numberFeatures;
			
					// Get the memory for the covariance matrices and work areas 	
					// to be used in the matrix inversions.								
			
			if (continueFlag)	 
				continueFlag = SetupMatrixInversionMemory (numberFeatures,
																			TRUE, 
																			&invertedCovPtr, 
																			&pivotPtr, 
																			&indexRowPtr, 
																			&indexColPtr, 
																			&ipvotPtr);
				
					// Get pointer to memory to use for storage of reduced 			
					// covariance matrix in square form.									
			 
			squareMatrixBytesNeeded = (UInt32)numberFeatureChannels *
															numberFeatureChannels * sizeof (double);
			if (continueFlag)
				{		
				utilityCovPtr = (HDoublePtr)MNewPointer (squareMatrixBytesNeeded);
				continueFlag = (utilityCovPtr != NULL);
				
				}	// end "if (continueFlag)" 
														
					// If memory not full, allocate memory for a matrix  for the	
					// result of the multiplication of the covariance and inverted	
					// covariance matrices.														
			
			if (continueFlag)
				{
				outputMatrixPtr = (HDoublePtr)MNewPointer (squareMatrixBytesNeeded);
				continueFlag = (outputMatrixPtr != NULL);

				savedOutputMatrixPtr = outputMatrixPtr;
				
				}	// end "if (continueFlag)" 
				
					// Get pointer to memory to use for temporary storage for svd 			
					// matrix inversion method.
					
			if (continueFlag)
				{		
				tempMatrixPtr = (HDoublePtr)MNewPointer (squareMatrixBytesNeeded);
				continueFlag = (tempMatrixPtr != NULL);
				
				}	// end "if (continueFlag)"
														
					// If memory not full, allocate memory for a the class 			
					// channel statistics.														
			
			if (continueFlag)
				{
				bytesNeeded = (UInt32)numberFeatureChannels * sizeof (ChannelStatistics);
				classChannelStatsPtr = (HChannelStatisticsPtr)MNewPointer (bytesNeeded);
				continueFlag = (classChannelStatsPtr != NULL);
				
				}	// end "if (continueFlag)" 
		
			if (continueFlag)			
				continueFlag = SetupFeatureTransformationMemory (
										evaluateCovarianceSpecsPtr->featureTransformationFlag, 
										numberFeatures, 
										numberFeatureChannels, 
										&gTransformationMatrix.eigenVectorPtr, 
										NULL,
										NULL, 
										&gTransformationMatrix.eigenFeaturePtr,
										kLoadMatricesVectors,
										(UInt16*)evaluateCovarianceSpecsPtr->featurePtr);
										
			}	// if (continueFlag) 
			
		if (continueFlag)
			{
					// Get dialog box to display covariance check status.				
			
			gStatusDialogPtr = GetStatusDialog (kUpdateStatsInfoID, TRUE);
			
			continueFlag = (gStatusDialogPtr != NULL);
			
					// Evaluating Class Covariances
					
			LoadDItemStringNumber (kUtilitiesStrID, 
											IDS_Utility6, 
											gStatusDialogPtr, 
											IDC_Status11, 
											(Str255*)gTextString);
											
					// Class
					
			LoadDItemStringNumber (kUtilitiesStrID, 
											IDS_Utility7, 
											gStatusDialogPtr, 
											IDC_Status2, 
											(Str255*)gTextString);
											
			LoadDItemValue (gStatusDialogPtr, 
									IDC_Status5, 
									(SInt32)evaluateCovarianceSpecsPtr->numberClasses);
					
			ShowStatusDialogItemSet (kStatusClassA);
			ShowStatusDialogItemSet (kStatusCommand);
			
			}	// end "if (continueFlag)" 
	
				// Continue with check, if memory is not full.							
	
		if (continueFlag)
					// List the channels used.		
			continueFlag = ListChannelsUsed (
										fileInfoPtr,
										evaluateCovarianceSpecsPtr->featurePtr,
										evaluateCovarianceSpecsPtr->channelsPtr,
										evaluateCovarianceSpecsPtr->numberFeatures, 
										NULL, 
										&gOutputForce1Code,
										evaluateCovarianceSpecsPtr->featureTransformationFlag);
					
		if (continueFlag)
			{	
			statFeaturePtr = (SInt16*)GetStatisticsFeatureVector (
										evaluateCovarianceSpecsPtr->featureTransformationFlag,
										(UInt16*)evaluateCovarianceSpecsPtr->featurePtr);
	
			if (evaluateCovarianceSpecsPtr->listOriginalMatrixFlag ||
					evaluateCovarianceSpecsPtr->listInvertedMatrixFlag ||
					evaluateCovarianceSpecsPtr->listOriginalXInvertedMatrixFlag ||
					evaluateCovarianceSpecsPtr->listInvertedInvertedMatrixFlag)
				{
						// Get memory for a character buffer to be used in listing.  		
						//		Allow 11 (12) digits for 'numberChannels' + 1 (left column for 	
						//		channel index.	 
					
				continueFlag = GetMemoryForListTransformation (numberFeatures, TRUE);
				
				}	// end "if (gCharBufferPtr1 == NULL)" 
				
					// Get the number of bytes in a square transformed 				
					// class covariance matrix.												
			 
			squareMatrixBytesNeeded = 
									(UInt32)numberFeatures * numberFeatures * sizeof (double);
										
					// List "  Output Information:"
					
			continueFlag = ListSpecifiedStringNumber (kSharedStrID, 
																		IDS_Shared8, 
																		(unsigned char*)gTextString, 
																		NULL, 
																		gOutputForce1Code, 
																		continueFlag);
																		
			}	// end "if (continueFlag)"
			
		initialStartTime = time (NULL);
			
		if (continueFlag)
			{				
					// Loop through for all the classes.									
					
			for (classIndex=0; 
						classIndex<evaluateCovarianceSpecsPtr->numberClasses;
						classIndex++)
				{
				statClassNumber = evaluateCovarianceSpecsPtr->classPtr[classIndex] - 1;
				
						// Get the class storage number.										
							
				classStorage = gProjectInfoPtr->storageClass[statClassNumber];
				
						// Update status dialog box.											
						
				LoadDItemValue (gStatusDialogPtr, IDC_Status3, (SInt32)classIndex+1);
				LoadDItemString (
						gStatusDialogPtr,
						IDC_Status6, 
						(Str255*)&gProjectInfoPtr->classNamesPtr[classStorage].name);
						
						// List the class name.												
						
				sprintf ((char*)gTextString2, "                               ");
				pstr ((char*)gTextString2,
						(char*)&gProjectInfoPtr->classNamesPtr[classStorage].name, 
						&strLength);
				gTextString2[strLength] = ' ';
				gTextString2[kMaxClassFieldNameLength] = kNullTerminator;
				sprintf ((char*)gTextString, 
								"    Class %3d: %s%s",
								classIndex+1, 
								gTextString2,
								gEndOfLine);
											
				continueFlag = ListString ((char*)gTextString, 
													(UInt32)strlen ((char*)gTextString),  
													gOutputTextH);

						// Get the class covariance matrix - full form, not 			
						// triangular.																

				GetTransformedClassCovarianceMatrix (
															numberFeatureChannels, 
															classChannelStatsPtr, 
															utilityCovPtr, 
															(UInt16*)statFeaturePtr, 
															statClassNumber,
															kSquareOutputMatrix,
															kMeanCovariance,
															gTransformationMatrix.eigenVectorPtr,
															outputMatrixPtr,
															numberFeatures);
								
						// Exit loop if 'command period' is down.							
						
				if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
					break;
					
						// List the covariance statistics.									
						
				if (evaluateCovarianceSpecsPtr->listOriginalMatrixFlag && continueFlag)
					{
							// List " Original Covariance Matrix\r");

					continueFlag = ListSpecifiedStringNumber (kUtilitiesStrID, 
																			IDS_Utility8, 
																			(UCharPtr)gTextString, 
																			NULL, 
																			kOutputWindowFormatCode, 
																			continueFlag);
				
					if (continueFlag)
						ListCovarianceMatrix (
										utilityCovPtr, 
										numberFeatures,
										evaluateCovarianceSpecsPtr->featurePtr,
										evaluateCovarianceSpecsPtr->channelsPtr, 
										1,
										evaluateCovarianceSpecsPtr->featureTransformationFlag);
								
							// Exit loop if 'command period' is down.						
						
					if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
						break;
						
					}	// end "if (evaluateCovarianceSpecsPtr->listOriginalMatrixFlag)" 
				
							// Get another copy of the covariance matrix for 			
							// inverting.															
				
				BlockMoveData (utilityCovPtr, invertedCovPtr, squareMatrixBytesNeeded);
								
						// Exit loop if 'command period' is down.							
						
				if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
					break;
					
						// Get start time of inversion.										
						
				startTime = time (NULL);
			
						// Invert the covariance matrix.										
						
				InvertSymmetricMatrix (invertedCovPtr, 
												numberFeatures, 
												pivotPtr, 
												indexRowPtr, 
												indexColPtr, 
												ipvotPtr, 
												&determinant,
												&logDeterminant,
												kReturnMatrixInverse);
									
				if (gOperationCanceledFlag)
					break;
											
						// Get the end time for the inversion.								
			
				endTime = time (NULL);
			
						// List the inversion statistics.									
	
				sprintf ((char*)gTextString,
								"%s    Inverted Covariance Matrix.%s"
								"     Det = %14.9e%s"
								"     Log Det = %14.9g%s",
								gEndOfLine,
								gEndOfLine,
								determinant,
								gEndOfLine, 
								logDeterminant,
								gEndOfLine);
					
				if (continueFlag)
					continueFlag = ListString ((char*)gTextString,
															(UInt32)strlen ((char*)gTextString),
															gOutputTextH);
													
				sprintf ((char*)gTextString,
								"     Rank of matrix = %hd%s", 
								ipvotPtr[0],
								gEndOfLine);
					
				if (continueFlag)
					continueFlag = ListString ((char*)gTextString,
															(UInt32)strlen ((char*)gTextString),
															gOutputTextH);
				
				index = ipvotPtr[0] - 1;
				if (ipvotPtr[0] < (SInt16)numberFeatures)
					index = ipvotPtr[0];
					 									
				sprintf ((char*)gTextString,
								"     Smallest pivot value = %14.9g%s", 
								pivotPtr[index],
								gEndOfLine);
					
				if (continueFlag)
					continueFlag = ListString ((char*)gTextString,
															(UInt32)strlen ((char*)gTextString),  
															gOutputTextH);
					
						// List the time to do the matrix inversion to nearest 		
						// second.																	
				
				sprintf ((char*)gTextString, 
							"     %u CPU seconds for matrix inversion.%s",
							(unsigned int)(endTime-startTime),
							gEndOfLine);
							
				if (continueFlag)
					continueFlag = ListString ((char*)gTextString,
															(UInt32)strlen ((char*)gTextString),  
															gOutputTextH);
								
						// Exit loop if 'command period' is down.							
						
				if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
					break;
				
						// List the inverted covariance matrix if requested.			
						
				if (evaluateCovarianceSpecsPtr->listInvertedMatrixFlag && continueFlag)
					ListCovarianceMatrix (
										invertedCovPtr, 
										numberFeatures,
										evaluateCovarianceSpecsPtr->featurePtr,
										evaluateCovarianceSpecsPtr->channelsPtr, 
										1,
										evaluateCovarianceSpecsPtr->featureTransformationFlag);
				
						// Continue matrix evaluation if everything is ok.				
						
				if (continueFlag)
					{
							// Multiply the two matrices together to check for the	
							// identity matrix.													
								
					MatrixMultiply (utilityCovPtr,
											invertedCovPtr, 
											outputMatrixPtr, 
											numberFeatures,
											numberFeatures,
											numberFeatures,
											kMat1ByMat2);
											
							// Get an estimate of the error of the matrix inversion.	
					
					error = 0.;
					maxError = 0.;
					for (chan1=0; chan1<numberFeatures; chan1++)
						{
						for (chan2=0; chan2<numberFeatures; chan2++)
							{
							if (chan1 == chan2)
								theValue = fabs (1. - *outputMatrixPtr);
							
							else	// chan1 != chan2 
								theValue = fabs (0. - *outputMatrixPtr);
								
							error += theValue;
							maxError = MAX (maxError, theValue);
								
							outputMatrixPtr++;
							
							}	// end "for (chan2=0; chan2<..." 
							
						}	// end "for (chan1=0; chan1<numberFeatures;..." 
			
							// List the result of the multiplied matrices.				
	
					sprintf ((char*)gTextString, 
									"%s    Multiplied matrices."
									"%s     Total error = %14.9e."
									"%s     Average error = %14.9e."
									"%s     Maximum error = %10.5e.%s",
									gEndOfLine,
									gEndOfLine,
									error,
									gEndOfLine, 
									error/numberFeatures/numberFeatures,
									gEndOfLine, 
									maxError,
									gEndOfLine);
						
					if (continueFlag)
						continueFlag = ListString ((char*)gTextString,
																(UInt32)strlen ((char*)gTextString),  
																gOutputTextH);
								
							// Exit loop if 'command period' is down.						
						
					if (continueFlag)
						continueFlag = CheckSomeEvents (
												osMask+keyDownMask+updateMask+mDownMask+mUpMask);

					outputMatrixPtr = savedOutputMatrixPtr;
												
					if (continueFlag && 
								evaluateCovarianceSpecsPtr->listOriginalXInvertedMatrixFlag)
						ListCovarianceMatrix (
										outputMatrixPtr, 
										numberFeatures,
										evaluateCovarianceSpecsPtr->featurePtr,
										evaluateCovarianceSpecsPtr->channelsPtr, 
										1,
										evaluateCovarianceSpecsPtr->featureTransformationFlag);
						
							// Invert the inverted matrix to get the original again.	
							
					if (continueFlag)
						{
								// Get start time of inversion.								
								
						startTime = time (NULL);
						
						InvertSymmetricMatrix (invertedCovPtr, 
														numberFeatures, 
														pivotPtr, 
														indexRowPtr, 
														indexColPtr, 
														ipvotPtr, 
														&determinant,
														&logDeterminant,
														kReturnMatrixInverse);
											
								// Get the end time for the inversion						
					
						endTime = time (NULL);
						
						}	// end "if (continueFlag)" 
									
					if (gOperationCanceledFlag)
						break;
				
							// List the inverted-inverted covariance matrix.			
		
					sprintf ((char*)gTextString,
									"%s    Inverted-Inverted Covariance Matrix.%s"
									"     Det = %14.9e.%s"
									"     Log Det = %14.9g%s", 
									gEndOfLine,
									gEndOfLine,
									determinant,
									gEndOfLine,
									logDeterminant,
									gEndOfLine);
						
					if (continueFlag)
						continueFlag = ListString ((char*)gTextString,
																(UInt32)strlen ((char*)gTextString),  
																gOutputTextH);
					
							// List the time to do the matrix inversion to  			
							// nearest second.													
														
					sprintf ((char*)gTextString, 
									"     %u CPU seconds for matrix inversion.%s",
									(unsigned int)(endTime-startTime),
									gEndOfLine);
								
					if (continueFlag)
						continueFlag = ListString ((char*)gTextString,
																(UInt32)strlen ((char*)gTextString),  
																gOutputTextH);
								
							// Exit loop if 'command period' is down.						
						
					if (continueFlag)
						continueFlag = CheckSomeEvents (
												osMask+keyDownMask+updateMask+mDownMask+mUpMask);
													
					if (continueFlag && 
								evaluateCovarianceSpecsPtr->listInvertedInvertedMatrixFlag)
						ListCovarianceMatrix (
										invertedCovPtr, 
										numberFeatures,
										evaluateCovarianceSpecsPtr->featurePtr,
										evaluateCovarianceSpecsPtr->channelsPtr, 
										1,
										evaluateCovarianceSpecsPtr->featureTransformationFlag);
					
					}	// end "if (continueFlag)" 
		
						// Exit loop if user has "command period" down.					
					
				if (!continueFlag || 
						!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
					break; 
					
				if (eigenvectorMethodFlag)
					{
							// List the covariance statistics.									
							
					if (evaluateCovarianceSpecsPtr->listOriginalMatrixFlag && continueFlag)
						{
								// List " Original Covariance Matrix\r");

						continueFlag = ListSpecifiedStringNumber (kUtilitiesStrID, 
																				IDS_Utility8, 
																				(UCharPtr)gTextString, 
																				NULL, 
																				kOutputWindowFormatCode, 
																				continueFlag);
					
						if (continueFlag)
							ListCovarianceMatrix (
										utilityCovPtr, 
										numberFeatures,
										evaluateCovarianceSpecsPtr->featurePtr,
										evaluateCovarianceSpecsPtr->channelsPtr, 
										1,
										evaluateCovarianceSpecsPtr->featureTransformationFlag);
									
								// Exit loop if 'command period' is down.						
							
						if (!CheckSomeEvents (
												osMask+keyDownMask+updateMask+mDownMask+mUpMask))
							break;
							
						}	// end "if (evaluateCovarianceSpecsPtr->listOriginalMatrixFlag)"
					
							// Get another copy of the covariance matrix for inverting.															
					
					BlockMoveData (utilityCovPtr, invertedCovPtr, squareMatrixBytesNeeded);
					
					startTime = time (NULL);
						
							// Now compute inverse using the eigenvector method.
		
					ComputeEigenvectors (invertedCovPtr,
													numberFeatures, 
													outputMatrixPtr,
													ipvotPtr,
													pivotPtr,
													1);
										
							// Get the determinant and log determinant and invert the
							// eigenvalues.	
					
					logDeterminant = 0.;
					determinant = 1.;
					for (chan1=0; chan1<numberFeatures; chan1++)
						{
						index = chan1 * numberFeatures + chan1;
						theValue = invertedCovPtr[index];
						determinant *= theValue;
						
						if (theValue > 0)
							{
							logDeterminant += log (theValue);
							pivotPtr[0] = theValue;
						
							theValue = 1./theValue;
							
							}	// end "if (theValue > 0)"
							
						else	// if (theValue <= 0)
							break;
								
						index = chan1;
						for (chan2=0; chan2<numberFeatures; chan2++)
							{
							tempMatrixPtr[index] = *outputMatrixPtr * theValue;
							index += numberFeatures;
							outputMatrixPtr++;
							
							}	// end "for (chan2=0; chan2<..."
							
						}	// end "for (chan1=0; chan1<numberFeatures;..." 					
					
					outputMatrixPtr = savedOutputMatrixPtr;				
					MatrixMultiply (tempMatrixPtr,
											outputMatrixPtr, 
											invertedCovPtr, 
											numberFeatures,
											numberFeatures,
											numberFeatures,
											kMat1ByMat2);
											
					if (gOperationCanceledFlag)
						break;
												
							// Get the end time for the inversion.								
				
					endTime = time (NULL);
				
							// List the inversion statistics.									
		
					sprintf (
							(char*)gTextString,
							"%s    Inverted Covariance Matrix using Eigenvector Analysis.%s"
							"     Det = %14.9e%s  Log Det = %14.9g%s", 
							gEndOfLine,
							gEndOfLine,
							determinant,
							gEndOfLine, 
							logDeterminant,
							gEndOfLine);
						
					if (continueFlag)
						continueFlag = ListString ((char*)gTextString,
																(UInt32)strlen ((char*)gTextString),  
																gOutputTextH);
						 									
					sprintf ((char*)gTextString,
									"     Smallest pivot value = %14.9g%s", 
									pivotPtr[0],
									gEndOfLine);
						
					if (continueFlag)
						continueFlag = ListString ((char*)gTextString,
																(UInt32)strlen ((char*)gTextString),  
																gOutputTextH);
						
							// Print the time to do the matrix inversion to nearest 		
							// second.																	
														
					sprintf ((char*)gTextString, 
								"     %u CPU seconds for matrix inversion.%s",
								(unsigned int)(endTime-startTime),
								gEndOfLine);
								
					if (continueFlag)
						continueFlag = ListString ((char*)gTextString,
																(UInt32)strlen ((char*)gTextString),  
																gOutputTextH);
									
							// Exit loop if 'command period' is down.							
							
					if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
						break;
					
							// List the inverted covariance matrix if requested.			
							
					if (evaluateCovarianceSpecsPtr->listInvertedMatrixFlag && continueFlag)
						ListCovarianceMatrix (
										invertedCovPtr, 
										numberFeatures,
										evaluateCovarianceSpecsPtr->featurePtr,
										evaluateCovarianceSpecsPtr->channelsPtr, 
										1,
										evaluateCovarianceSpecsPtr->featureTransformationFlag);
					
							// Continue matrix evaluation if everything is ok.				
							
					if (continueFlag)
						{		
								// Multiply the two matrices together to check for the	
								// identity matrix.													
									
						MatrixMultiply (utilityCovPtr,
												invertedCovPtr, 
												outputMatrixPtr, 
												numberFeatures,
												numberFeatures,
												numberFeatures,
												kMat1ByMat2);
												
								// Get an estimate of the error of the matrix inversion.	
						
						error = 0.;
						maxError = 0.;
						for (chan1=0; chan1<numberFeatures; chan1++)
							{
							for (chan2=0; chan2<numberFeatures; chan2++)
								{
								if (chan1 == chan2)
									theValue = fabs (1. - *outputMatrixPtr);
								
								else	// chan1 != chan2 
									theValue = fabs (0. - *outputMatrixPtr);
									
								error += theValue;
								maxError = MAX (maxError, theValue);
									
								outputMatrixPtr++;
								
								}	// end "for (chan2=0; chan2<..." 
								
							}	// end "for (chan1=0; chan1<numberFeatures;..." 
				
								// List the result of the multiplied matrices.				
		
						sprintf ((char*)gTextString, 
										"%s    Multiplied matrices.%s"
										"     Total error = %10.5e.%s"
										"     Average error = %10.5e.%s"
										"     Maximum error = %10.5e.%s", 
										gEndOfLine,
										gEndOfLine,
										error,
										gEndOfLine, 
										error/numberFeatures/numberFeatures,
										gEndOfLine, 
										maxError,
										gEndOfLine);
							
						if (continueFlag)
							continueFlag = ListString ((char*)gTextString,
																(UInt32)strlen ((char*)gTextString),
																gOutputTextH);
						
						outputMatrixPtr = savedOutputMatrixPtr;
						if (continueFlag && 
								evaluateCovarianceSpecsPtr->listOriginalXInvertedMatrixFlag)
							ListCovarianceMatrix (
										outputMatrixPtr, 
										numberFeatures,
										evaluateCovarianceSpecsPtr->featurePtr,
										evaluateCovarianceSpecsPtr->channelsPtr, 
										1,
										evaluateCovarianceSpecsPtr->featureTransformationFlag);
													
						}	// end "if (continueFlag)"
								
							// Exit loop if 'command period' is down.						
						
					if (continueFlag)
						continueFlag = CheckSomeEvents (
												osMask+keyDownMask+updateMask+mDownMask+mUpMask);
						
					}	// end "if (eigenvectorMethodFlag)"
		
				if (classIndex != evaluateCovarianceSpecsPtr->numberClasses-1)
					{
							// Insert a two blank lines
						
					continueFlag = OutputString (NULL, 
															(char*)gEndOfLine, 
															0, 
															gOutputForce1Code,
															continueFlag); 
						
					continueFlag = OutputString (NULL, 
															(char*)gEndOfLine, 
															0, 
															gOutputForce1Code,
															continueFlag);
															
					}	// end "if (classIndex != 0" 
				
				}	// end "for (classIndex != ...->numberClasses-1)" 
				
			gCharBufferPtr1 = CheckAndDisposePtr (gCharBufferPtr1);
			/*		
					// Do several matrix inversions to test timing						
					// Get start time for loop.												
			
			loopCount = 100;		
			startTime = time (NULL);
			
			for (index=0; index<loopCount; index++)
				{
				determinant = InvertSymmetricMatrix (utilityCovPtr, 
									numberUtilityChannels, pivotPtr, indexRowPtr, 
																	indexColPtr, ipvotPtr);
									
				if (gOperationCanceledFlag)
					break;
					
				}
			
			endTime = time (NULL);
			sprintf ((char*)gTextString, 
							"%ld CPU seconds for %ld original matrix inversions\r",
							endTime-startTime, loopCount);
			if (continueFlag)
				continueFlag = ListString ((char*)gTextString,
												strlen ((char*)gTextString),  gOutputTextH);		
			*/
					// Add blank line at the end of the text window.
						
			sprintf ((char*)gTextString, "%s", gEndOfLine);
			continueFlag = OutputString ((CMFileStream*)NULL, 
													(char*)gTextString, 
													gNumberOfEndOfLineCharacters, 
													gOutputForce1Code, 
													continueFlag);
					
					// List the CPU time taken for the checking covariances.			
					
			continueFlag = ListCPUTimeInformation (
														NULL, continueFlag, initialStartTime);
										
			}	// end "if (continueFlag)"
			
				// Free other memory.															
			
		CheckAndDisposePtr ((Ptr)classChannelStatsPtr);
		CheckAndDisposePtr (tempMatrixPtr);
		CheckAndDisposePtr (outputMatrixPtr);
		CheckAndDisposePtr (utilityCovPtr);
			
				// Free memory used for matrix inversions.							
				
		CheckAndDisposePtr (indexRowPtr);
		CheckAndDisposePtr (indexColPtr);
		CheckAndDisposePtr (ipvotPtr);
		CheckAndDisposePtr (invertedCovPtr);
		CheckAndDisposePtr (pivotPtr); 
	
				// Release memory for transformation matrix information.				
				
		ReleaseFeatureTransformationMemory ();
		
		CheckAndUnlockHandle (evaluateCovarianceSpecsPtr->featureHandle);
		CheckAndUnlockHandle (evaluateCovarianceSpecsPtr->channelsHandle);
		CheckAndUnlockHandle (evaluateCovarianceSpecsPtr->classHandle);
			
		}	// end "if (LoadEvaluateCovariancesSpecs (fileInfoPtr))" 
		
	UnlockProjectWindowInfoHandles ();
	
			// Unlock memory for eval transform structure.										
			
	CheckAndUnlockHandle (gProjectInfoPtr->evalCovarianceSpecsHandle);
			
			// Scroll output window to the selection and adjust the 					
			// scroll bar.																			
	
	UpdateOutputWScrolls (gOutputWindow, 1, kDisplayMessage);
				
			// Dispose of status dialog box.													
	
	CloseStatusDialog (TRUE);
	
	gOperationCanceledFlag = FALSE;
				
			// Change cursor back to pointer.												
							
	MInitCursor ();

}	// end "EvaluateCovariancesControl" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean EvaluateCovariancesDialog
//
//	Software purpose:	The purpose of this routine is to handle the 		
//							modal dialog for selecting the specifications
//							for evaluating the covariance matrices.
//
//	Parameters in:		pointer to the image file information structure.
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			EvaluateCovariancesControl   in SOther.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/29/1990
//	Revised By:			Larry L. Biehl			Date: 06/10/1998	

Boolean EvaluateCovariancesDialog (
				EvaluateCovarianceSpecsPtr		evaluateCovarianceSpecsPtr, 
				FileInfoPtr							fileInfoPtr)

{
	Boolean								returnFlag;
	
#if defined multispec_mac
	Rect									theBox;
	
	DialogPtr							dialogPtr;
	UserItemUPP							drawChannelsPopUp2Ptr;

	Handle								okHandle,
											theHandle;
									
	
	SInt16								itemHit,
											theType;

	UInt16								numberEigenvectors;

	Boolean								featureTransformAllowedFlag,
											featureTransformationFlag,
											modalDone;

	
			// Get the modal dialog for the reformat specification					
				
	dialogPtr = LoadRequestedDialog (
							kEvaluateCovarianceSpecificationID, kCopyScrap, 1, 2);
	if (dialogPtr == NULL)											
																						return (FALSE);
		
			// Intialize local user item proc pointers.									

	drawChannelsPopUp2Ptr = NewUserItemUPP (DrawChannelsPopUp2);
	
			// Save handle for the OK button for use later.								
			
	GetDialogItem (dialogPtr, 1,  &theType, &okHandle, &theBox);
	
			// Set flag for listing the original covariance matrix.		
			
	SetDLogControl (dialogPtr, 
							4, 
							(SInt16)evaluateCovarianceSpecsPtr->listOriginalMatrixFlag);
	
			// Set flag for listing the inverted matrix.					
			
	SetDLogControl (dialogPtr, 
							5, 
							(SInt16)evaluateCovarianceSpecsPtr->listInvertedMatrixFlag);
	
			// Set flag for listing the original covariance matrix		
			//          multiplied by the inverted matrix, i.e. identity matrix.	
			
	SetDLogControl (
					dialogPtr,
					6,
					(SInt16)evaluateCovarianceSpecsPtr->listOriginalXInvertedMatrixFlag);
	
			// Set flag for listing the inverted-inverted covariance matrix.																	
			
	SetDLogControl (
					dialogPtr,
					7,
					(SInt16)evaluateCovarianceSpecsPtr->listInvertedInvertedMatrixFlag);
		
			// Set the check box for the feature transformation option.				
			
	EigenvectorInfoExists (kProject, NULL, &numberEigenvectors);
	featureTransformAllowedFlag = (numberEigenvectors > 0);
	if (numberEigenvectors <= 0)
		{		
		featureTransformationFlag = FALSE;
		HideDialogItem (dialogPtr, 8);
		
		}	// end "if (numberEigenvectors <= 0)" 
		
	else	// numberEigenvectors > 0 
		featureTransformationFlag =
										evaluateCovarianceSpecsPtr->featureTransformationFlag;
									
	CheckFeatureTransformationDialog (dialogPtr, 
													featureTransformAllowedFlag,
													8, 
													9, 
													&featureTransformationFlag);
	
			//	Set drawing routine for the channel popup box.							
			//	Make all channels the default													
			
	SetDialogItemDrawRoutine (dialogPtr, 10, drawChannelsPopUp2Ptr);
	gChannelSelection = evaluateCovarianceSpecsPtr->channelSet;
	
			//	Set drawing routine for the class popup box.								
			//	Make all classes the default													
			
	SetDialogItemDrawRoutine (dialogPtr, 12, gDrawDialogClassPopUpPtr);
	gClassSelection = evaluateCovarianceSpecsPtr->classSet;
		
			// Center the dialog and then show it.											
			
	ShowDialogWindow (
				dialogPtr, kEvaluateCovarianceSpecificationID, kDoNotSetUpDFilterTable);
	
	modalDone = FALSE;
	itemHit = 0;
	do 
		{
	   ModalDialog (gProcessorDialogFilterPtr, &itemHit);
		if (itemHit > 2)
			{
					// Get the handle to the itemHit.										
					
			GetDialogItem (dialogPtr, itemHit, &theType, &theHandle, &theBox);
			
			switch (itemHit)
				{
				case 4:				// List original matrix. 
				case 5:				// List inverted matrix. 
				case 6:				// List original x inverted matrix. 
				case 7:				// List inverted-inverted matrix. 
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					break;
						
				case 8:		// Feature transformation flag. 
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					featureTransformationFlag = !featureTransformationFlag;
					CheckFeatureTransformationDialog (dialogPtr, 
																	featureTransformAllowedFlag,
																	8, 
																	9, 
																	&featureTransformationFlag);
					break;
					
				case 10:		// Channels/features 
					itemHit = StandardPopUpMenu (dialogPtr, 
															9, 
															10, 
															gPopUpAllAvailableSubsetMenu, 
															gChannelSelection, 
															kPopUpAllAvailableSubsetMenuID);
													
					if (itemHit == kSubsetMenuItem)
						{
								// Subset of channels to be used.							
								
						HiliteControl ((ControlHandle)okHandle, 255);
						
						ChannelsDialog (
										(SInt16*)&evaluateCovarianceSpecsPtr->numberFeatures,
										evaluateCovarianceSpecsPtr->featurePtr,
										gImageLayerInfoPtr,
										fileInfoPtr,
										1,
										featureTransformationFlag,
										(SInt16*)gProjectInfoPtr->channelsPtr,
										gProjectInfoPtr->numberStatisticsChannels,
										gChannelSelection);
							
						HiliteControl ((ControlHandle)okHandle, 0);
						
						if (featureTransformationFlag && 
								(evaluateCovarianceSpecsPtr->numberFeatures ==
												gTransformationMatrix.numberFeatures))
							itemHit = kAllMenuItem;
													
						if (!featureTransformationFlag &&
								(evaluateCovarianceSpecsPtr->numberFeatures ==
													gProjectInfoPtr->numberStatisticsChannels))
							itemHit = kAllMenuItem;
							
						}	// end "if (itemHit == kSubsetMenuItem)" 
					
					if (itemHit != 0)
						gChannelSelection = itemHit;
	
							// Make certain that the correct label is drawn in the	
							// channel pop up box.												
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
					
				case 12:		// Classes 
					itemHit = StandardPopUpMenu (dialogPtr, 
															11, 
															12, 
															gPopUpAllSubsetMenu, 
															gClassSelection, 
															kPopUpAllSubsetMenuID);
													
					if (itemHit == kSubsetMenuItem)
						{
								// Subset of classes to be used.
						
						itemHit = ClassDialog (&evaluateCovarianceSpecsPtr->numberClasses,
														evaluateCovarianceSpecsPtr->classPtr, 
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
			
		else	// itemHit <= 2 
			{
				
			if	(itemHit == 1)      // User selected OK for information 
				{
				modalDone = TRUE;
				returnFlag = TRUE;   	

				EvaluateCovariancesDialogOK (evaluateCovarianceSpecsPtr,
														GetDLogControl (dialogPtr, 4),
														GetDLogControl (dialogPtr, 5),
														GetDLogControl (dialogPtr, 6),
														GetDLogControl (dialogPtr, 7),
														featureTransformationFlag);
																
				}	// end "if (itemHit == 1)" 
				
			if	(itemHit == 2)      // User selected Cancel for information 
				{
				modalDone = TRUE;
				returnFlag = FALSE;
				
				}	// end "if	(itemHit == 2)" 
			
			}	// end "else itemHit <= 2" 
				
		}	while (!modalDone);
		
	DisposeUserItemUPP (drawChannelsPopUp2Ptr);
		
	CloseRequestedDialog (dialogPtr, kDoNotSetUpDFilterTable);
#endif	// defined multispec_mac

	#if defined multispec_win 	
		CMEvalCovarianceDialog*		dialogPtr = NULL;
			
		TRY
			{ 
			dialogPtr = new CMEvalCovarianceDialog (); 
				
			returnFlag = dialogPtr->DoDialog (evaluateCovarianceSpecsPtr); 
			                       
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
   	CMEvalCovarianceDialog*		dialogPtr = NULL;  
   
      dialogPtr = new CMEvalCovarianceDialog (); 
				
		returnFlag = dialogPtr->DoDialog (evaluateCovarianceSpecsPtr); 
	
		delete dialogPtr;
	#endif
	
	return (returnFlag);

}	// end "EvaluateCovariancesDialog"  

									

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void EvaluateCovariancesDialogOK
//
//	Software purpose:	The purpose of this routine is to set up the utility
//							structure parameters that deal with evaluating
//							covariance matrixes based on the selections that the
//							user made in the dialog box.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			DoDialog in xEvaluateCovariancesDialog.cpp
//							EvaluateCovariancesDialog
//
//	Coded By:			Larry L. Biehl			Date: 04/26/1996
//	Revised By:			Larry L. Biehl			Date: 05/13/1998	

void EvaluateCovariancesDialogOK (
				EvaluateCovarianceSpecsPtr		evaluateCovarianceSpecsPtr, 
				Boolean								listOriginalMatrixFlag,
				Boolean								listInvertedMatrixFlag,
				Boolean								listOriginalXInvertedMatrixFlag,
				Boolean								listInvertedInvertedMatrixFlag,
				Boolean								featureTransformationFlag)

{                                     
    if (evaluateCovarianceSpecsPtr != NULL) 
    	{
		evaluateCovarianceSpecsPtr->listOriginalMatrixFlag = listOriginalMatrixFlag;

		evaluateCovarianceSpecsPtr->listInvertedMatrixFlag = listInvertedMatrixFlag;

		evaluateCovarianceSpecsPtr->listOriginalXInvertedMatrixFlag = 
																	listOriginalXInvertedMatrixFlag;

		evaluateCovarianceSpecsPtr->listInvertedInvertedMatrixFlag = 
																	listInvertedInvertedMatrixFlag;
				
				// Feature transformation option.									
						
		evaluateCovarianceSpecsPtr->featureTransformationFlag = 
																	featureTransformationFlag;
					
				// Channels	or features													

		evaluateCovarianceSpecsPtr->channelSet = gChannelSelection;
		if (gChannelSelection == kAllMenuItem)	// All channels or features
			LoadFeatureVector (featureTransformationFlag,
										(SInt16*)&evaluateCovarianceSpecsPtr->numberFeatures,
										evaluateCovarianceSpecsPtr->featurePtr,
										gProjectInfoPtr->numberStatisticsChannels);
															
		LoadChannelsVector (kProject,
									featureTransformationFlag,
									evaluateCovarianceSpecsPtr->numberFeatures, 
									evaluateCovarianceSpecsPtr->featurePtr,
									(SInt16*)&evaluateCovarianceSpecsPtr->numberChannels, 
									evaluateCovarianceSpecsPtr->channelsPtr);
					
				// Classes																	

		evaluateCovarianceSpecsPtr->classSet = gClassSelection;
		if (gClassSelection == kAllMenuItem)		// All classes 
			LoadTrainClassVector (&evaluateCovarianceSpecsPtr->numberClasses,
											&evaluateCovarianceSpecsPtr->classSet, 
											evaluateCovarianceSpecsPtr->classPtr);
											
		}	// end "if (evaluateCovarianceSpecsPtr != NULL)"

}	// end "EvaluateCovariancesDialogOK"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void EvaluateTransformationControl
//
//	Software purpose:	The purpose of this routine is to evaluate
//							the saved transformation matrix.  The matrix
//							is generated by an eigenvector analysis and
//							therefore when multiplied my its transpose
//							should be the identity matrix.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/15/1993
//	Revised By:			Larry L. Biehl			Date: 03/13/2015

void EvaluateTransformationControl (void)

{
	double								error;

	time_t								startTime;
	                 
	HDoublePtr							eigenvaluePtr,
											eigenvectorPtr,
											outputMatrixPtr,
											savedOutputMatrixPtr;
	
	SInt16*								channelsPtr;
	EvaluateTransformSpecsPtr		evalTransformationSpecsPtr;
	
	UInt32								chan1,
											chan2, 
											numberBytes;
												
	UInt16								numberChannels,
											numberFeatures;
											
	Boolean								continueFlag;
	
		
			// If spare memory had to be used to load code resources, then exit	
			// routine.																				
			
	if (gMemoryTypeNeeded < 0 || gTransformationMatrix.numberChannels <= 0)
																								return;
																							
			// Code resources loaded okay, so set memory flag back for non-Code	
			// resources.																			
			
	gMemoryTypeNeeded = 0;
	
			// Initialize some local variables.												
	
	continueFlag = TRUE;
	outputMatrixPtr = NULL;
	evalTransformationSpecsPtr = NULL;
	
			// Set up evaluate transformation specification structure.				
	
	if (LoadEvaluateTransformationSpecs ())
		{
		evalTransformationSpecsPtr = (EvaluateTransformSpecsPtr)GetHandlePointer (
												gNonProjProcessorSpecs.evalTransformationSpecsH,
												kLock);

      if (EvaluateTransformationDialog (evalTransformationSpecsPtr))
			{
					// Initialize local variables												
						
			numberChannels = gTransformationMatrix.numberChannels;
			numberFeatures = gTransformationMatrix.numberFeatures;
	
					// Get pointer to buffer to use to store data values 				
					// to be listed. 
					
			if (continueFlag)
				continueFlag = GetMemoryForListTransformation (
						numberChannels, 
						(evalTransformationSpecsPtr->listTransformFlag ||
								evalTransformationSpecsPtr->listTransformXTposedTransformFlag));
			
			if (continueFlag && evalTransformationSpecsPtr->checkTransformationFlag)
				{		
						// Get pointer to memory to use for storage multiplied matrix.																	
					
				numberBytes = (UInt32)numberChannels * numberChannels * sizeof (double);
				outputMatrixPtr = (HDoublePtr)MNewPointer (numberBytes);
				continueFlag = (outputMatrixPtr != NULL);

				savedOutputMatrixPtr = outputMatrixPtr;
				
				}	// end "if (continueFlag && ...)" 
				
			eigenvectorPtr	= (HDoublePtr)GetHandlePointer (
															gTransformationMatrix.eigenVectorHandle,
															kLock);
				
			eigenvaluePtr= (HDoublePtr)GetHandlePointer (
															gTransformationMatrix.eigenValueHandle,
															kLock);
			
			channelsPtr = (SInt16*)GetHandlePointer (
															gTransformationMatrix.eigenFeatureHandle,
															kLock);
				
					// Force text selection to start from end of present text.		
							
			ForceTextToEnd ();
			
					// Initialize some global variables pertaining to output.		
			
			gOutputTextOKFlag = TRUE;
			gOutputCode = 1;
			gOutputForce1Code = gOutputCode;
			
					// Change cursor to watch cursor until done with process.		
		
			MSetCursor (kWait);
				
					// List the processor name, date and time.							
		
			continueFlag = ListHeaderInfo (NULL, 
														0, 
														&gOutputForce1Code, 
														kNoStatisticsUsed, 
														continueFlag);
										
					// List "  Output Information:"
					
			continueFlag = ListSpecifiedStringNumber (kSharedStrID, 
																	IDS_Shared8, 
																	(unsigned char*)gTextString, 
																	NULL, 
																	gOutputForce1Code, 
																	continueFlag);
														
					// List processor that transformation was generated from.
							
			SInt16 stringNumber = IDS_Utility4;
			if (gTransformationMatrix.createdByCode == 2)
				stringNumber = IDS_Utility5;
				
			else if (gTransformationMatrix.createdByCode == 16)
				stringNumber = IDS_Utility9;
				
			else if (gTransformationMatrix.createdByCode == 17)
				stringNumber = IDS_Utility10;	
					
			continueFlag = ListSpecifiedStringNumber (kUtilitiesStrID, 
																	stringNumber, 
																	(unsigned char*)gTextString, 
																	NULL, 
																	gOutputForce1Code, 
																	continueFlag);
																		
			startTime = time (NULL);	
			
					// List the eigenvalues and transformation matrix as requested.																	
			
			if (continueFlag)	
				continueFlag = ListTransformationInformation (
											eigenvaluePtr,
											eigenvectorPtr,
											0,
											0,
											(UInt16*)channelsPtr,
											numberChannels,
											numberFeatures,
											evalTransformationSpecsPtr->listEigenvaluesFlag,
											evalTransformationSpecsPtr->listTransformFlag,
											gTransformationMatrix.createdByCode);

			if (evalTransformationSpecsPtr->checkTransformationFlag)
				{
				if (continueFlag)
					{	
							// Multiply the two matrices together to check for the	
							// identity matrix.													
								
					MatrixMultiply (eigenvectorPtr,
											eigenvectorPtr, 
											outputMatrixPtr, 
											numberFeatures,
											numberChannels,
											numberFeatures,
											kMat1ByTransposeMat2);
											
					CheckAndUnlockHandle (gTransformationMatrix.eigenVectorHandle);
														
							// Get an estimate of the error of the matrix inversion.	
					
					error = 0.;		
					for (chan1=0; chan1<numberFeatures; chan1++)
						{ 
						for (chan2=0; chan2<numberFeatures; chan2++)
							{
							if (chan1 == chan2) 
								error += fabs (1. - *outputMatrixPtr);
							
							else	// chan1 != chan2 
								error += fabs (0. - *outputMatrixPtr);
								
							outputMatrixPtr++;
							
							}	// end "for (chan2=0; chan2<..."
							
						}	// end "for (chan1=0; chan1<numberFeatures;..." 
						
							// List the result of the multiplied matrices.				
			
					sprintf ((char*)gTextString, 
						"%s    Transposed transformation matrix times transformation matrix.%s"
						"      Total error = %10.5e.   Average error = %10.5e.%s",
								gEndOfLine,
								gEndOfLine, 
								error, 
								error/numberFeatures/numberFeatures,
								gEndOfLine);
							
					if (continueFlag)
						continueFlag = ListString ((char*)gTextString,
															(UInt32)strlen ((char*)gTextString),  
															gOutputTextH);

					outputMatrixPtr = savedOutputMatrixPtr;
															
					if (continueFlag && 
								evalTransformationSpecsPtr->listTransformXTposedTransformFlag)
						ListCovarianceMatrix (outputMatrixPtr, 
														numberFeatures,
														NULL,
														NULL, 
														1,
														TRUE);
						
							// Add blank line at the end of the text window.		
							                                 
					sprintf ((char*)gTextString, "%s", gEndOfLine);
					continueFlag = OutputString ((CMFileStream*)NULL, 
															(char*)gTextString, 
															gNumberOfEndOfLineCharacters, 
															gOutputForce1Code, 
															continueFlag);
					
							// List the CPU time taken for the checking transformation matrix.			
							
					continueFlag = ListCPUTimeInformation (NULL, continueFlag, startTime);
												
					}	// end "if (continueFlag)" 
					
				}	// end "if (evalTransformationSpecsPtr->checkTransformationFlag)" 
					
					// Free temporary memory.													
			
			gCharBufferPtr1 = CheckAndDisposePtr (gCharBufferPtr1);
			
			outputMatrixPtr = CheckAndDisposePtr (outputMatrixPtr);
			
			CheckAndUnlockHandle (gTransformationMatrix.eigenValueHandle);
			CheckAndUnlockHandle (gTransformationMatrix.eigenVectorHandle);
			CheckAndUnlockHandle (gTransformationMatrix.eigenFeatureHandle);
					
					// Scroll output window to the selection and adjust the 			
					// scroll bar.																	
			
			UpdateOutputWScrolls (gOutputWindow, 1, kDisplayMessage);
						
					// Change cursor back to pointer.										
									
			MInitCursor ();
			

			}	// end "if (EvaluateTransformationDialog (..." 

			
		}	// end "if (LoadEvaluateTransformationSpecs (..." 
	
			// Unlock memory for eval transform structure.										
			
	CheckAndUnlockHandle (gNonProjProcessorSpecs.evalTransformationSpecsH);

}	// end "EvaluateTransformationControl" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean EvaluateTransformationDialog
//
//	Software purpose:	The purpose of this routine is to handle the 		
//							modal dialog for selecting the specifications
//							for evaluating the transformation matrix.
//
//	Parameters in:		pointer to the image file information structure.
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			EvaluateTransformationControl   in SOther.cpp
//
//	Coded By:			Larry L. Biehl			Date: 06/27/1994
//	Revised By:			Larry L. Biehl			Date: 05/13/1998	

Boolean EvaluateTransformationDialog (
				EvaluateTransformSpecsPtr		evalTransformationSpecsPtr)

{
	Boolean								returnFlag;

#if defined multispec_mac
	Rect									theBox;
	
	DialogPtr							dialogPtr;

	Handle								theHandle;
	
	SInt16								itemHit,
											theType;
	
	Boolean								checkTransformationFlag,
											listEigenvaluesFlag,
											listTransformFlag,
											listTransformXTposedTransformFlag,
											modalDone;

	
			// Get the modal dialog for the reformat specification					
				
	dialogPtr = LoadRequestedDialog (
								kEvaluateTransformSpecificationID, kDoNotCopyScrap, 1, 2);
	if (dialogPtr == NULL)											
																						return (FALSE);
	
			// Initialize dialog variables.

	EvaluateTransformationInitialize (dialogPtr,
													evalTransformationSpecsPtr,
													&listEigenvaluesFlag,
													&listTransformFlag,
													&checkTransformationFlag,
													&listTransformXTposedTransformFlag);
	
			// Set flag for listing the eigenvalues.										
			
	SetDLogControl (dialogPtr, 4, (SInt16)listEigenvaluesFlag);
	
			// Set flag for listing the transformation matrix.							
			
	SetDLogControl (dialogPtr, 5, (SInt16)listTransformFlag);
	
			// Set flag for checking transformation matrix.								
		
	SetDLogControl (dialogPtr, 6, (SInt16)checkTransformationFlag);
	
			// Set flag for listing the transposed transformation matrix			
			// multiplied by the transformation matrix.									
			
	SetDLogControl (dialogPtr, 7, (SInt16)listTransformXTposedTransformFlag);
		
			// Center the dialog and then show it.											
			
	ShowDialogWindow (
				dialogPtr, kEvaluateTransformSpecificationID, kDoNotSetUpDFilterTable);
	
	modalDone = FALSE;
	itemHit = 0;
	do 
		{
	   ModalDialog (gProcessorDialogFilterPtr, &itemHit);
		if (itemHit > 2)
			{
					// Get the handle to the itemHit.										
					
			GetDialogItem (dialogPtr, itemHit, &theType, &theHandle, &theBox);
			
			switch (itemHit)
				{
				case 4:		// List eigenvalues. 											
				case 5:		// List transformation matrix. 								
				case 6:		// Check transformation matrix. 								
				case 7:		// List transposed transformation x transformation 	
								// matrix. 															
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					
					if (itemHit == 6)
						{
						checkTransformationFlag = !checkTransformationFlag;
						ShowHideDialogItem (dialogPtr, 7, checkTransformationFlag);
						
						}	// end "if (itemHit == 6)" 
						
					break;
						
				}	// end "switch (itemHit)" 
				
			}	// end "if (itemHit > 2)" 
			
		else	// itemHit <= 2 
			{
			if	(itemHit == 1)      // User selected OK for information 
				{
				modalDone = TRUE;
				returnFlag = TRUE;
				
				EvaluateTransformationOK (evalTransformationSpecsPtr,
													GetDLogControl (dialogPtr, 4),
													GetDLogControl (dialogPtr, 5),
													GetDLogControl (dialogPtr, 6),
													GetDLogControl (dialogPtr, 7));
													
				}	// end "if (itemHit == 1)" 
				
			if	(itemHit == 2)      // User selected Cancel for information 
				{
				modalDone = TRUE;
				returnFlag = FALSE;
				
				}	// end "if	(itemHit == 2)" 
			
			}	// end "else itemHit <= 2" 
				
		}	while (!modalDone);
		
	CloseRequestedDialog (dialogPtr, kDoNotSetUpDFilterTable);
#endif	// defined multispec_mac 

	#if defined multispec_win 
		CMEvalTransformDialog*		dialogPtr = NULL;
			
		TRY
			{ 
			dialogPtr = new CMEvalTransformDialog (); 
				
			returnFlag = dialogPtr->DoDialog (evalTransformationSpecsPtr); 
			                       
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
   	CMEvalTransformDialog*		dialogPtr = NULL;
      
   	dialogPtr = new CMEvalTransformDialog (); 
				
   	returnFlag = dialogPtr->DoDialog (evalTransformationSpecsPtr); 
			                       
   	delete dialogPtr;  
	#endif
	
	return (returnFlag);
	
}	// end "EvaluateTransformationDialog"



void EvaluateTransformationInitialize (
				DialogPtr							dialogPtr,
				EvaluateTransformSpecsPtr		evalTransformationSpecsPtr,
				Boolean*								listEigenvaluesFlagPtr,
				Boolean*								listTransformFlagPtr,
				Boolean*								checkTransformationFlagPtr,
				Boolean*								listTransformXTposedTransformFlagPtr)

{
			// Set flag for listing the eigenvalues.										
	
	*listEigenvaluesFlagPtr	= evalTransformationSpecsPtr->listEigenvaluesFlag;
	
			// Set flag for listing the transformation matrix.							
	
	*listTransformFlagPtr = evalTransformationSpecsPtr->listTransformFlag;
	
			// Set flag for checking transformation matrix.								
	
	*checkTransformationFlagPtr = evalTransformationSpecsPtr->checkTransformationFlag;	
	
			// Set flag for listing the transposed transformation matrix			
			// multiplied by the transformation matrix.									
	
	*listTransformXTposedTransformFlagPtr = 
								evalTransformationSpecsPtr->listTransformXTposedTransformFlag;
												
	if (gTransformationMatrix.createdByCode < 16)
		{	
		CtoPstring ((UCharPtr)"List Eigenvalues", gTextString);
		
		ShowHideDialogItem (dialogPtr, 
									IDC_ListCheckMatrix, 
									evalTransformationSpecsPtr->checkTransformationFlag);
		
		}	// end "if (gTransformationMatrix.createdByCode < 16)"
		
	else	// gTransformationMatrix.createdByCode >= 16)
		{
		CtoPstring ((UCharPtr)"List Offset & Gain Values", gTextString);
									
		HideDialogItem (dialogPtr, IDC_ListTransform);
		HideDialogItem (dialogPtr, IDC_CheckTransform);
		HideDialogItem (dialogPtr, IDC_ListCheckMatrix);
		
		}	// end "else gTransformationMatrix.createdByCode >= 16)"
										
	SetDLogControlTitle (dialogPtr, 
									IDC_ListEigenvalues, 
									(CharPtr)gTextString,
									kASCIICharString);

}	// end "EvaluateTransformationInitialize"



void EvaluateTransformationOK (
				EvaluateTransformSpecsPtr		evalTransformationSpecsPtr,
				Boolean								listEigenvaluesFlag,
				Boolean								listTransformFlag,
				Boolean								checkTransformationFlag,
				Boolean								listTransformXTposedTransformFlag)

{		
			// List options.	
																	
	evalTransformationSpecsPtr->listEigenvaluesFlag = listEigenvaluesFlag;
	
	evalTransformationSpecsPtr->listTransformFlag = listTransformFlag;
	
	evalTransformationSpecsPtr->checkTransformationFlag = checkTransformationFlag;
	
	if (checkTransformationFlag)
		evalTransformationSpecsPtr->listTransformXTposedTransformFlag = 
															listTransformXTposedTransformFlag;

}	// end "EvaluateTransformationOK"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ListCovarianceMatrix
//
//	Software purpose:	The purpose of this routine is to list the 
//							covariance matrix to the output window in either
//							square form or triangular form based on input
//							'type' flag.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			EvaluateCovariancesControl in SOther.cpp
//							EvaluateTransformationControl in SOther.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/16/1988
//	Revised By:			Larry L. Biehl			Date: 03/13/2015

void ListCovarianceMatrix (
				HDoublePtr 							covariancePtr, 
				SInt16								numberChannels,
				SInt16*								featurePtr,
				SInt16*								channelsPtr,
				SInt16								matrixType,
				Boolean								transformationFlag)

{
	Ptr									stringPtr;
	
	SInt32								channel,
											covChan,
											numCovChannels;
											
	UInt32								channelNumber,
											covIndex;

	Boolean								continueFlag;
	

			// Initialize local variables														
	
	if (gCharBufferPtr1 != NULL)
		{
				// List the channel headings above the matrix.							
									
		sprintf (gCharBufferPtr1, "%s    Channel", gEndOfLine);
		for (channel=0; channel<numberChannels; channel++)
			{
			stringPtr = (Ptr)&gCharBufferPtr1[strlen (gCharBufferPtr1)];
			
			if (transformationFlag && featurePtr != NULL)
				channelNumber = (UInt32)featurePtr[channel] + 1;
				
			else if (!transformationFlag && channelsPtr != NULL)
				channelNumber = (UInt32)channelsPtr[channel] + 1;
				
			else	// channelsPtr == NULL && featurePtr == NULL
				channelNumber = channel + 1;
					
			#if defined multispec_mac || defined multispec_wx                                          
				sprintf (stringPtr,  "\t%11u", (unsigned int)channelNumber);
			#endif	// defined multispec_mac || defined multispec_wx 
					
			#if defined multispec_win                                                  
				sprintf (stringPtr,  "\t%12ld", channelNumber);   
			#endif	// defined multispec_win 
			
			}	// end "for (channel=1; channel<=numberChannels; ..." 
		
		stringPtr = (Ptr)&gCharBufferPtr1[strlen (gCharBufferPtr1)];
		sprintf ((char*)stringPtr, "%s", gEndOfLine);
		continueFlag = ListString (gCharBufferPtr1, 
											(UInt32)strlen (gCharBufferPtr1), 
											gOutputTextH);

				// List the covariance matrix square matrix form.						
					
		covIndex = 0;
		if (continueFlag)
			for (channel=0; channel<numberChannels; channel++)
				{
				if (transformationFlag && featurePtr != NULL)
					channelNumber = (UInt32)featurePtr[channel] + 1;
					
				else if (!transformationFlag && channelsPtr != NULL)
					channelNumber = (UInt32)channelsPtr[channel] + 1;
					
				else	// channelsPtr == NULL && featurePtr == NULL
					channelNumber = channel + 1;
				
				sprintf (gCharBufferPtr1, "    %7u", (unsigned int)channelNumber);
		        
				numCovChannels = channel;	
				if (matrixType == 1)
					numCovChannels = numberChannels;
				
				for (covChan=0; covChan<numCovChannels; covChan++)
					{
					stringPtr = (Ptr)&gCharBufferPtr1[strlen (gCharBufferPtr1)];
					
					#if defined multispec_mac || defined multispec_wx
						sprintf ((char*)stringPtr, "\t%11.4e", covariancePtr[covIndex]);
					#endif	// defined multispec_mac || defined multispec_wx
					
					#if defined multispec_win   
						sprintf ((char*)stringPtr, "\t%12.4e", covariancePtr[covIndex]);  
					#endif	// defined multispec_win 
					
					covIndex++;
					
					}	// end "for (covChan=0; ..." 
					
				stringPtr = (Ptr)&gCharBufferPtr1[strlen (gCharBufferPtr1)];
				sprintf ((char*)stringPtr, "%s", gEndOfLine);
				continueFlag = ListString ((char*)gCharBufferPtr1, 
													(UInt32)strlen (gCharBufferPtr1), 
													gOutputTextH);
					
				if (!continueFlag)
					break;
					
				}	// end "for (channel=0; channel<numberChannels..." 
				
		}	// end "if (gCharBufferHandle1)" 

}	// end "ListCovarianceMatrix" 
																
																

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ListDescriptionInformation
//
//	Software purpose:	The purpose of this routine is to list the descriptive
//							information for the active image window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 03/01/1993
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

void ListDescriptionInformation (void)

{
	char									formatName[32];
	FileInfoPtr							localFileInfoPtr;
	
	Handle								windowInfoHandle;
	
	SInt32								stringLength;
	
	SInt16								handleStatus;
	UInt16								index;
	
	Boolean								continueFlag,
											listAllInformationFlag;
	
		
			// If spare memory had to be used to load code resources, then exit	
			// routine.																			
			
	if (gMemoryTypeNeeded < 0)
																							return;
																							
			// Code resources loaded okay, so set memory flag back for non-Code	
			// resources.																			
			
	gMemoryTypeNeeded = 0;
	
			// Check if the option key is down. If so then this is signal to do
			// a complete listing of information for the file ... text for
			// all hdf/netcdf data sets or all tags for TIFF/GeoTIFF files.
			
	listAllInformationFlag = FALSE;
		
	#if defined multispec_mac
		if ((gEventRecord.modifiers & (optionKey+shiftKey)) > 0)
	#endif	// defined multispec_mac
		
	#if defined multispec_win                                  
		if (GetKeyState (VK_SHIFT) & 0x8000)
	#endif	// defined multispec_win 
		
	#if defined multispec_wx                                  
		if (wxGetKeyState (WXK_SHIFT))
	#endif	// defined multispec_wx 
			listAllInformationFlag = TRUE; 
	
	continueFlag = SetUpActiveImageInformationGlobals (
										&windowInfoHandle, &handleStatus, kDoNotUseProject);
								
	if (!continueFlag)
																							return;
	
	gOutputCode = kOutputWindowFormatCode;
	gOutputForce1Code = (gOutputCode | 0x0001);
			
			// Force text selection to start from end of present text.			
		
	ForceTextToEnd ();
	
			// List the dashed line separator string to indicate start of processor
			// output.
			
	ListSpecifiedStringNumber (kSharedStrID, 
										IDS_Shared5, 
										(unsigned char*)gTextString, 
										NULL, 
										gOutputForce1Code, 
										continueFlag);
	
	if (gImageWindowInfoPtr->numberImageFiles > 1)	
		{
		if (continueFlag)
			{	
					// List the linked image window name.	
					
			GetActiveImageWindowTitle (gTextString2);	
			
			gTextString2 [gTextString2[0]+1] = 0;								
			sprintf ((char*)gTextString,
						"Description information for linked image window: '%s'%s",
						&gTextString2[1],
						gEndOfLine);
			continueFlag = ListString ((char*)gTextString,  
													(UInt32)strlen ((char*)gTextString),  
													gOutputTextH,
													kUTF8CharString);
			
			}	// end "if (continueFlag)" 
			
		if (continueFlag)
			{
					// List the number of lines in the linked image.				
					
			sprintf ((char*)gTextString,
						"  Number of lines in linked image:     %6u%s",
						(unsigned int)gImageWindowInfoPtr->maxNumberLines,
						gEndOfLine);
			continueFlag = ListString ((char*)gTextString,
												(UInt32)strlen ((char*)gTextString),
												gOutputTextH);
			
			}	// end "if (continueFlag)" 
			
		if (continueFlag)
			{
					// List the number of columns in the linked image.				
					
			sprintf ((char*)gTextString,
						"  Number of columns in linked image:   %6u%s",
						(unsigned int)gImageWindowInfoPtr->maxNumberColumns,
						gEndOfLine);
			continueFlag = ListString ((char*)gTextString,  
												(UInt32)strlen ((char*)gTextString),  
												gOutputTextH);
			
			}	// end "if (continueFlag)" 
			
		if (continueFlag && gImageWindowInfoPtr->windowType == kImageWindowType)
			{
						// List the number of channels in the linked image.			
						
			sprintf ((char*)gTextString,
						"  Number of channels in linked image:    %4d%s", 
						gImageWindowInfoPtr->totalNumberChannels,
						gEndOfLine);
			continueFlag = ListString ((char*)gTextString,  
												(UInt32)strlen ((char*)gTextString),  
												gOutputTextH);
			
			}	// end "if (continueFlag && ...)" 
			
		if (continueFlag)
			{
						// List the maximum number of bytes in the linked image.		
							
			sprintf ((char*)gTextString,
						"  Maximum number of bytes in linked image: %2d%s", 
						gImageWindowInfoPtr->numberBytes,
						gEndOfLine);
			continueFlag = ListString ((char*)gTextString,  
												(UInt32)strlen ((char*)gTextString),  
												gOutputTextH);
			
			}	// end "if (continueFlag)" 
			
		if (continueFlag)
			{
						// List the maximum number of bits in the linked image.		
							
			sprintf ((char*)gTextString,
						"  Maximum number of bits in linked image:  %2d%s", 
						gImageWindowInfoPtr->numberBits,
						gEndOfLine);
			continueFlag = ListString ((char*)gTextString,  
												(UInt32)strlen ((char*)gTextString),  
												gOutputTextH);
			
			}	// end "if (continueFlag)" 
			
		if (continueFlag)
			{
					// List the number of linked files.										
					
			sprintf ((char*)gTextString,
						"  Number of linked files:               %5d%s", 
						gImageWindowInfoPtr->numberImageFiles,
						gEndOfLine);
			continueFlag = ListString ((char*)gTextString,  
												(UInt32)strlen ((char*)gTextString),  
												gOutputTextH);
		
			}	// end "if (continueFlag)" 
		
				// Insert a blank line
			
		continueFlag = OutputString (NULL, 
												gEndOfLine, 
												gNumberOfEndOfLineCharacters, 
												gOutputForce1Code, 
												continueFlag);
			
		}	// end "if (gImageWindowInfoPtr->numberImageFiles > 1)" 
		
	for (index=0; index<gImageWindowInfoPtr->numberImageFiles; index++)
		{
		localFileInfoPtr = &gImageFileInfoPtr[index];
		
		if (continueFlag)
			{
			FileStringPtr fileNamePtr = 
						(FileStringPtr)GetFileNameCPointerFromFileInfo (localFileInfoPtr);
			
			if (gImageWindowInfoPtr->numberImageFiles == 1)
				{
				sprintf ((char*)gTextString,
							"Description information for");
				stringLength = 27;
				
				}	// end "if (gImageWindowInfoPtr->numberImageFiles == 1)"
					
			else	// gImageWindowInfoPtr->numberImageFiles > 1)
				{
				sprintf ((char*)gTextString,
							"  Description information for");
				stringLength = 29;
				
				sprintf ((char*)&gTextString[stringLength],
							" file %3d", 
							index+1);
				stringLength += 9;
				
				}	// end "if (gImageWindowInfoPtr->numberImageFiles > 1)" 
			
			sprintf ((char*)&gTextString[stringLength],
						": '%s'%s", 
						(char*)fileNamePtr,
						gEndOfLine);
			
			continueFlag = ListString ((char*)gTextString,  
												(UInt32)strlen ((char*)gTextString),  
												gOutputTextH,
												kUTF8CharString);
	
			}	// end "if (continueFlag)" 
			
		continueFlag = ListDescriptionInformationForFile (
																localFileInfoPtr, formatName, 32);			
		if (continueFlag && localFileInfoPtr->format == kLARSYSMISTType)
			continueFlag = ListLARSYSMISTHeaderInformation (localFileInfoPtr);
		
				// List the channels used if descriptions exist.						
				
		if (continueFlag)
			{
			if (localFileInfoPtr->channelDescriptionH && !localFileInfoPtr->thematicType)
				continueFlag = ListChannelsUsed (localFileInfoPtr, 
															NULL,
															NULL, 
															localFileInfoPtr->numberChannels, 
															NULL, 
															&gOutputCode,
															FALSE);
															
			else	// ...->channelDescriptionH == NULL || localFileInfoPtr->thematicType
						// Insert a blank line
				continueFlag = OutputString (NULL, 
														gEndOfLine, 
														gNumberOfEndOfLineCharacters, 
														gOutputForce1Code, 
														continueFlag);
														
			}	// end "if (continueFlag)"
			
		if (continueFlag && (localFileInfoPtr->format == kTIFFType || 
														localFileInfoPtr->format == kGeoTIFFType))
				{
						// List any TIFF/GeoTIFF ascii tag information.
						
				continueFlag = ListTiffTextDescriptionParameters (localFileInfoPtr);
																
				}	// end "if (continueFlag && (...->format == kTIFFType || ..."
		
		#if include_hdf_capability	
			if (continueFlag && (localFileInfoPtr->format == kHDF4Type || 
														localFileInfoPtr->format == kNETCDFType))
				{
						// List the HDF data set attributes.
						
				continueFlag = ListHDF4DataSetAttributes (localFileInfoPtr, 
																		formatName, 
																		listAllInformationFlag);
																
				}	// end "if (continueFlag && (...->format == kHDF4Type || ..."
		#endif		// include_hdf_capability
		
		#if include_gdal_capability
			if (continueFlag)
				continueFlag = ListGDALDataSetAttributes (localFileInfoPtr, 
																		listAllInformationFlag);
		#endif	// include_gdal_capability
			
				// List the map parameters if any.
		
		if (continueFlag)
			continueFlag = ListMapParameters (localFileInfoPtr, listAllInformationFlag);
							
		if (!continueFlag)
			break;
		
		}	// end "for (index=0; index<...->numberImageFiles..." 
		
			// List the dashed line separator string to indicate end of processor
			// output.
			
	continueFlag = ListSpecifiedStringNumber (kSharedStrID, 
															IDS_Shared6, 
															(unsigned char*)gTextString, 
															NULL, 
															gOutputForce1Code, 
															continueFlag);
			
			// Scroll output window to the selection and adjust the scroll bar.																			
	
	UpdateOutputWScrolls (gOutputWindow, 1, kDisplayNoMessage);
		
	UnlockActiveImageInformationGlobals (handleStatus);

}	// end "ListDescriptionInformation" 
																
																

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ListDescriptionInformationForFile
//
//	Software purpose:	The purpose of this routine is to list the descriptive
//							information for the active image window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 06/28/2010
//	Revised By:			Larry L. Biehl			Date: 05/31/2020

Boolean ListDescriptionInformationForFile (
				FileInfoPtr							fileInfoPtr,
				char*									formatNamePtr,
				SInt16								maxFormatNameLength)

{
	unsigned char						formatName[256];
	
	SInt16								format;
	UInt16								stringIndex;
	
	Boolean								continueFlag = TRUE;
	
	
	if (fileInfoPtr != NULL)
		{
		format = MAX (fileInfoPtr->format, 0);
		if (continueFlag)
			{
					// List the file format type.		

			format += IDS_FileType01;
			
			continueFlag = GetSpecifiedStringNumber (
														kFileTypeStrID, format, formatName, TRUE);
			
			if (!continueFlag)
				{				
						// Try again with format of 0 or unknown		

				#if defined multispec_mac 
					format = 1;
				#endif	// defined multispec_mac

				#if defined multispec_win || defined multispec_wx 
					format = IDS_FileType01;
				#endif	// defined multispec_win || ...

				continueFlag = GetSpecifiedStringNumber (
														kFileTypeStrID, format, formatName, TRUE);
					
				}	// end "if (!continueFlag)"
				
			if (continueFlag)
				{				
				sprintf ((char*)gTextString,
							"    File format:              %s%s",
							&formatName[1],
							gEndOfLine);
				continueFlag = ListString ((char*)gTextString,  
													(UInt32)strlen ((char*)gTextString),  
													gOutputTextH);
				
						// Save format name for possible use later. Make sure no more than 
						// the max number of characters is copied.
							
				formatName[maxFormatNameLength-1] = 0;
				strcpy (formatNamePtr, (char*)&formatName);
				
				}	// end "if (GetSpecifiedStringNumber (kFileTypeStrID, format+1, ..." 
				
			}	// end "if (continueFlag && format > 0)" 
			
		if (continueFlag && fileInfoPtr->hdfHandle != NULL)
			{
			GetHdfDataSetName (fileInfoPtr, 
										fileInfoPtr->hdfDataSetSelection,
										(StringPtr)gTextString2,
										NULL);
				
			sprintf ((char*)gTextString,
						"      %s data set name:         %s%s",
						&formatNamePtr[1],
						(char*)&gTextString2[1],
						gEndOfLine);
			continueFlag = ListString ((char*)gTextString,  
												(UInt32)strlen ((char*)gTextString),  
												gOutputTextH);
				
			if (fileInfoPtr->format != kHDF5Type && fileInfoPtr->format != kNETCDF2Type)
				{
						// I have not run into a case where there is a separate header file 
						// for HDF5 format.
						
				GetHdfHeaderFileName (fileInfoPtr, gTextString2);
					
				sprintf ((char*)gTextString,
							"      %s header file name:      %s%s",
							&formatNamePtr[1],
							(char*)&gTextString2[1],
							gEndOfLine);
					
				continueFlag = ListString ((char*)gTextString,  
													(UInt32)strlen ((char*)gTextString),  
													gOutputTextH);
					
				}	// end "if (fileInfoPtr->format != kHDF5Type && ..."
			
			}	// end "if (continueFlag && fileInfoPtr->hdfHandle != NULL)"
			
		if (continueFlag)
			{			
					// List the image type.														
					
			sprintf ((char*)gTextString,"    Image type:                  ");
			
			if (fileInfoPtr->thematicType)
				sprintf ((char*)&gTextString[30],
							"Thematic%s",
							gEndOfLine);
			else	// !fileInfoPtr->thematicType 
				sprintf ((char*)&gTextString[30],
							"Multispectral%s",
							gEndOfLine);
				
			continueFlag = ListString ((char*)gTextString,  
												(UInt32)strlen ((char*)gTextString),  
												gOutputTextH);
		
			}	// end "if (continueFlag)" 
			
		ListInstrumentName (fileInfoPtr);
			
		if (continueFlag)
			{
					// List the band interleave format.	
					
			if (fileInfoPtr->gdalBandInterleave > 0)
				format = fileInfoPtr->gdalBandInterleave;
				
			else	// fileInfoPtr->gdalBandInterleave <= 0
				format = MAX (0, fileInfoPtr->bandInterleave);									
			
			#if defined multispec_mac 
				format += 1;
			#endif	// defined multispec_mac

			#if defined multispec_win | defined multispec_wx
				format += IDS_BandInterleave01;
			#endif	// defined multispec_win	

			continueFlag = GetSpecifiedStringNumber (
												kBandInterleaveStrID, format, formatName, TRUE);
			
			if (!continueFlag)
				{				
						// Try again with format of 0 or unknown
						
				#if defined multispec_mac 
					format = 1;
				#endif	// defined multispec_mac

				#if defined multispec_win || defined multispec_wx
					format = IDS_BandInterleave01;
				#endif	// defined multispec_win || ...

				continueFlag = GetSpecifiedStringNumber (
												kBandInterleaveStrID, format, formatName, TRUE);
					
				}	// end "if (!continueFlag)"
				
			if (continueFlag)
				{
				sprintf ((char*)gTextString,
							"    Band interleave format:   %s%s", 
							&formatName[1],
							gEndOfLine);
				continueFlag = ListString ((char*)gTextString,  
													(UInt32)strlen ((char*)gTextString),
													gOutputTextH);
	
				}	// end "if (continueFlag)" 
		
			}	// end "if (continueFlag)" 
			
				// Write the data type.
				
		if (continueFlag)
			{
			sprintf ((char*)gTextString,"    Data type:                   "); 
			
			if (fileInfoPtr->dataTypeCode == kIntegerType)
				sprintf ((char*)&gTextString[30],"Integer%s", gEndOfLine);
			else	// fileInfoPtr->dataTypeCode == kRealType
				sprintf ((char*)&gTextString[30],"Real%s", gEndOfLine);
				
			continueFlag = ListString ((char*)gTextString,
												(UInt32)strlen ((char*)gTextString),
												gOutputTextH);
			
			}	// end "if (continueFlag)"
			
		if (continueFlag && fileInfoPtr->numberBytes > 1)
			{			
			sprintf ((char*)gTextString,"    Swap bytes:                  "); 
			
			if (fileInfoPtr->swapBytesFlag)
				sprintf ((char*)&gTextString[30],"Yes%s", gEndOfLine);
			else	// !fileInfoPtr->swapBytesFlag 
				sprintf ((char*)&gTextString[30],"No%s", gEndOfLine);
				
			continueFlag = ListString ((char*)gTextString,
												(UInt32)strlen ((char*)gTextString),
												gOutputTextH);
		
			}	// end "if (continueFlag && ...)" 
			
		if (continueFlag && !fileInfoPtr->thematicType)
			{			
			sprintf ((char*)gTextString,"    Signed data:                 ");
			
			if (fileInfoPtr->signedDataFlag)
				sprintf ((char*)&gTextString[30],"Yes%s", gEndOfLine);
			else	// !fileInfoPtr->signedDataFlag 
				sprintf ((char*)&gTextString[30],"No%s", gEndOfLine);
				
			continueFlag = ListString ((char*)gTextString,
												(UInt32)strlen ((char*)gTextString),
												gOutputTextH);
		
			}	// end "if (continueFlag && ...)" 
			
		if (continueFlag && fileInfoPtr->dataCompressionCode != kNoCompression)
			{			
			sprintf ((char*)gTextString,"    Compression Algorithm:       ");
			
			stringIndex = IDS_Compression01+fileInfoPtr->dataCompressionCode-1;
			
			continueFlag = GetSpecifiedStringNumber (kCompressionStrID, 
																	stringIndex, 
																	gTextString2, 
																	continueFlag);
			
			if (continueFlag)									
				sprintf ((char*)&gTextString[30],"%s%s", &gTextString2[1], gEndOfLine);
				
			continueFlag = ListString ((char*)gTextString,
												(UInt32)strlen ((char*)gTextString),  
												gOutputTextH);
		
			}	// end "if (continueFlag && ...)" 
			
		if (continueFlag && fileInfoPtr->blockedFlag)
			{			
			sprintf ((char*)gTextString, 
						"    Tiled format:             Yes%s", 
						gEndOfLine);
				
			continueFlag = ListString ((char*)gTextString,  
												(UInt32)strlen ((char*)gTextString),
												gOutputTextH);
		
			}	// end "if (continueFlag && fileInfoPtr->blockedFlag)" 
			
		if (continueFlag)
			{
					// List the number of lines in the image file.					
					
			sprintf ((char*)gTextString,
						"    Number of lines:              %10u%s",
						(unsigned int)fileInfoPtr->numberLines,
						gEndOfLine);
			continueFlag = ListString ((char*)gTextString,
												(UInt32)strlen ((char*)gTextString),
												gOutputTextH);
			
			}	// end "if (continueFlag)" 
			
		if (continueFlag)
			{
					// List the number of columns in the image file.				
					
			sprintf ((char*)gTextString,
						"    Number of columns:            %10u%s",
						(unsigned int)fileInfoPtr->numberColumns,
						gEndOfLine);
			continueFlag = ListString ((char*)gTextString,
												(UInt32)strlen ((char*)gTextString),
												gOutputTextH);
			
			}	// end "if (continueFlag)" 
			
		if (continueFlag)
			{
						// List the number of channels in the image file.			
			
			if (fileInfoPtr->thematicType)	
				sprintf ((char*)gTextString,
							"    Number of classes:            %10u%s",
							(unsigned int)fileInfoPtr->numberClasses,
							gEndOfLine);
			else	// !fileInfoPtr->thematicType 		
				sprintf ((char*)gTextString,
							"    Number of channels:           %10d%s",   
							fileInfoPtr->numberChannels,
							gEndOfLine);	
			
			continueFlag = ListString ((char*)gTextString,
												(UInt32)strlen ((char*)gTextString),
												gOutputTextH);
			
			}	// end "if (continueFlag)" 
			
		if (continueFlag)
			{
						// List the number of bytes in the image file.				
							
			sprintf ((char*)gTextString,
						"    Number of bytes:              %10d%s", 
						fileInfoPtr->numberBytes,
						gEndOfLine);
			continueFlag = ListString ((char*)gTextString,
												(UInt32)strlen ((char*)gTextString),
												gOutputTextH);
			
			}	// end "if (continueFlag)" 
			
		if (continueFlag)
			{
						// List the number of bits in the image file.				
							
			sprintf ((char*)gTextString,
						"    Number of bits:               %10d%s", 
						fileInfoPtr->numberBits,
						gEndOfLine);
			continueFlag = ListString ((char*)gTextString,
												(UInt32)strlen ((char*)gTextString),
												gOutputTextH);
			
			}	// end "if (continueFlag)"
			
		if (continueFlag && (fileInfoPtr->bandInterleave == kBIL ||
										fileInfoPtr->bandInterleave == kBSQ ||
										fileInfoPtr->bandInterleave == kBIS ||
										fileInfoPtr->bandInterleave == kBIBlock))
			{			
						// List the number of header bytes in the image file.	
						
			sprintf ((char*)gTextString,
						"    Number of header bytes:       %10u%s",
						(unsigned int)fileInfoPtr->numberHeaderBytes,
						gEndOfLine);
			continueFlag = ListString ((char*)gTextString,
												(UInt32)strlen ((char*)gTextString),
												gOutputTextH);
		
			}	// end "if (continueFlag)" 
			
		if (continueFlag && !fileInfoPtr->blockedFlag &&
									(fileInfoPtr->bandInterleave == kBIL ||
										fileInfoPtr->bandInterleave == kBSQ ||
										fileInfoPtr->bandInterleave == kBIS ||
										fileInfoPtr->bandInterleave == kBNonSQ))
			{			
						// List the number of pre-line bytes in the image file.	
						
			sprintf ((char*)gTextString,
						"    Number of pre-line bytes:     %10u%s"
						"    Number of post-line bytes:    %10u%s",
						(unsigned int)fileInfoPtr->numberPreLineBytes,
						gEndOfLine, 
						(unsigned int)fileInfoPtr->numberPostLineBytes,
						gEndOfLine);
			continueFlag = ListString ((char*)gTextString,  
												(UInt32)strlen ((char*)gTextString),  
												gOutputTextH);
		
			}	// end "if (continueFlag && ..." 
		
		if (continueFlag && !fileInfoPtr->thematicType &&  
										(fileInfoPtr->bandInterleave == kBIL ||
											fileInfoPtr->bandInterleave == kBSQ ||
											fileInfoPtr->bandInterleave == kBNonSQ))
				{			
				sprintf ((char*)gTextString,
							"    Number of pre-channel bytes:  %10u%s"
							"    Number of post-channel bytes: %10u%s",
							(unsigned int)fileInfoPtr->numberPreChannelBytes,
							gEndOfLine, 
							(unsigned int)fileInfoPtr->numberPostChannelBytes,
							gEndOfLine);
				continueFlag = ListString ((char*)gTextString,
													(UInt32)strlen ((char*)gTextString),
													gOutputTextH);
				
			}	// end "if (continueFlag && !...->thematicType && ..." 
			
		if (continueFlag && (fileInfoPtr->blockedFlag || fileInfoPtr->blockHeight > 0))
			{
			UInt32			blockHeight,
								blockOffset,
								blockSize,
								blockWidth;
			
			GetBlockInformation (fileInfoPtr, 
										&blockHeight, 
										&blockWidth,
										&blockSize,
										&blockOffset);
						
			sprintf ((char*)gTextString,
						"    Block height:                 %10u%s"
						"    Block width:                  %10u%s",
						(unsigned int)blockHeight, gEndOfLine,
						(unsigned int)blockWidth, gEndOfLine);
			continueFlag = ListString ((char*)gTextString,
												(UInt32)strlen ((char*)gTextString),
												gOutputTextH);
				
			if (fileInfoPtr->blockedFlag)
				{
				sprintf ((char*)gTextString,
							"    Block size:                   %10u%s"
							"    Block offset:                 %10u%s",
							(unsigned int)blockSize, gEndOfLine,
							(unsigned int)blockOffset, gEndOfLine);
				continueFlag = ListString ((char*)gTextString,
													(UInt32)strlen ((char*)gTextString),
													gOutputTextH);
				
				}	// end "if (fileInfoPtr->blockedFlag)"
		
			}	// end "if (continueFlag && ..." 
			
		if (continueFlag)
			{			
			sprintf ((char*)gTextString,
						"    Line start:                   %10u%s"
						"    Column start:                 %10u%s",
						(unsigned int)fileInfoPtr->startLine,
						gEndOfLine,
						(unsigned int)fileInfoPtr->startColumn,
						gEndOfLine);
			continueFlag = ListString ((char*)gTextString, 
												(UInt32)strlen ((char*)gTextString),
												gOutputTextH);
		
			}	// end "if (continueFlag)" 
			
		if (continueFlag && fileInfoPtr->noDataValueFlag)
			{
			sprintf ((char*)gTextString,
						"    No Data (or Fill) Value: %15.9g%s",	// 14.7e%s used in past.
																				// Will try g for now.
						fileInfoPtr->noDataValue,
						gEndOfLine);
			continueFlag = ListString ((char*)gTextString,
												(UInt32)strlen ((char*)gTextString),
												gOutputTextH);
		
			}	// end "if (continueFlag && fileInfoPtr->noDataValueFlag)" 

			
		if (continueFlag && fileInfoPtr->treatLinesAsBottomToTopFlag)
			{
			sprintf ((char*)gTextString,
						"    Lines stored from bottom to top (rather than top to bottom)%s",
						gEndOfLine);
				
			continueFlag = ListString ((char*)gTextString,
												(UInt32)strlen ((char*)gTextString),
												gOutputTextH);
			
			}	// end "if (continueFlag && fileInfoPtr->treatLinesAsBottomToTopFlag)"
			
		}	// end "if (fileInfoPtr != NULL)"
		
	return (continueFlag);

}	// end "ListDescriptionInformationForFile"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ListInstrumentName
//
//	Software purpose:	The purpose of this routine is to list the instrument
//							name if the instrument code has been set.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/20/2013
//	Revised By:			Larry L. Biehl			Date: 03/01/2013

void ListInstrumentName (
				FileInfoPtr							fileInfoPtr)

{
	SInt16								stringIndex;
	
	Boolean								continueFlag = TRUE;
	
	
	if (fileInfoPtr->instrumentCode > 0)
		{			
		sprintf ((char*)gTextString,"    Instrument Name:          ");
		
		stringIndex = IDS_InstrumentName01+fileInfoPtr->instrumentCode-1;
		
		continueFlag = GetSpecifiedStringNumber (kInstrumentNameStrID, 
															  stringIndex, 
															  gTextString2, 
															  continueFlag);
		
		if (continueFlag)
			{
			sprintf ((char*)&gTextString[30],"%s%s", &gTextString2[1], gEndOfLine);		
			ListString ((char*)gTextString,  
							(UInt32)strlen ((char*)gTextString),
							gOutputTextH);
							
			}	// end "if (continueFlag)"
		
		}	// end "if (fileInfoPtr->instrumentCode > 0)"  
				
}	// end "ListInstrumentName" 
																
																

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ListLARSYSMISTHeaderInformation
//
//	Software purpose:	The purpose of this routine is to list the descriptive
//							information in the LARSYS MIST header.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/15/1993
//	Revised By:			Larry L. Biehl			Date: 12/05/2011

Boolean ListLARSYSMISTHeaderInformation (
				FileInfoPtr							fileInfoPtr)

{
	CMFileStream*						fileStreamPtr;
	HUCharPtr							stringPtr;
	SInt32*								headerRecordPtr;

	StringHandle						stringHandle;
	
	UInt32								count;
	
	SInt16								errCode,
											index;
	
	UInt16								conversionIndex;
	
	Boolean								continueFlag;
	
	
			// Set up swap bytes flag depending upon the system architecture                                
									  
	gSwapBytesFlag = !gBigEndianFlag;
	
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);

			// Get address of character buffer to use to read header into.			
			
	headerRecordPtr = (SInt32*)gTextString3;
	
	count = 0;
	errCode = MSetMarker (fileStreamPtr, 
									fsFromStart, 
									count,
									kNoErrorMessages);
	if (errCode == noErr)
		{
		count = 80;
		errCode = MReadData (fileStreamPtr, 
									&count, 
									headerRecordPtr,
									kNoErrorMessages);
		
		}	// end "if (errCode == noErr)" 
		
	if (errCode != noErr)
																							return (TRUE);
																						
			// Get the string to use to convert from EBCDIC to ASCII.			
			
	conversionIndex = (UInt16)GetSpecifiedString (
														128, &stringHandle, (char**)&stringPtr);
	
	if (stringHandle == NULL)
																							return (TRUE);
		
			// LARSYS Run Number.															
					
	if (gSwapBytesFlag)
		Swap4Bytes ((HUInt32Ptr)&headerRecordPtr[2], 1);
																				
	sprintf ((char*)gTextString,
				"%s    LARSYS Run Number:    %8u%s",
				gEndOfLine,  
				(unsigned int)headerRecordPtr[2],
				gEndOfLine);
	continueFlag = ListString ((char*)gTextString,
										(UInt32)strlen ((char*)gTextString), 
										gOutputTextH);
		
			// Flightline description.														
		
	if (continueFlag)
		{
		BlockMoveData (&headerRecordPtr[6], (char*)gTextString2, 16);
		gTextString2[16] = kNullTerminator;
		
		for (index=0; index<16; index++)
			{
			conversionIndex = gTextString2[index];
			gTextString2[index] = stringPtr[conversionIndex];
			
			}	// end "for (index=0; index<16; index++)" 
		
		sprintf ((char*)gTextString,
					"    LARSYS Description:   %s%s", 
					gTextString2,
					gEndOfLine);
		continueFlag = ListString ((char*)gTextString,
											(UInt32)strlen ((char*)gTextString),
											gOutputTextH);
		
		}	// end "if (continueFlag)" 
		
			// Date data collected.																
		
	if (continueFlag)
		{					
		if (gSwapBytesFlag)
			{
			Swap4Bytes ((HUInt32Ptr)&headerRecordPtr[10], 1);
			Swap4Bytes ((HUInt32Ptr)&headerRecordPtr[11], 1);
			Swap4Bytes ((HUInt32Ptr)&headerRecordPtr[12], 1);
			
			}	// end "if (gSwapBytesFlag)"
		
		sprintf ((char*)gTextString,
					"    Date data collected:  %2u-%2u-%u%s",
					(unsigned int)headerRecordPtr[10],
					(unsigned int)headerRecordPtr[11],
					(unsigned int)headerRecordPtr[12],
					gEndOfLine);
		continueFlag = ListString ((char*)gTextString, 
											(UInt32)strlen ((char*)gTextString), 
											gOutputTextH);
		
		}	// end "if (continueFlag)" 
		
			// Time data collected.																
		
	if (continueFlag)
		{
		BlockMoveData (&headerRecordPtr[13], (char*)gTextString2, 4);
		gTextString2[4] = kNullTerminator;
		
		for (index=0; index<4; index++)
			{
			conversionIndex = gTextString2[index];
			gTextString2[index] = stringPtr[conversionIndex];
			
			}	// end "for (index=0; index<4; index++)" 
		
		sprintf ((char*)gTextString,
					"    Time data collected:  %s%s", 
					(char*)gTextString2,
					gEndOfLine);
		continueFlag = ListString ((char*)gTextString, 
											(UInt32)strlen ((char*)gTextString), 
											gOutputTextH);
		
		}	// end "if (continueFlag)" 
		
			// Platform altitude.																
		
	if (continueFlag)
		{
		if (gSwapBytesFlag)
			Swap4Bytes ((HUInt32Ptr)&headerRecordPtr[14], 1);
			
		sprintf ((char*)gTextString,
					"    Altitude of platform: %u%s",
					(unsigned int)headerRecordPtr[14],
					gEndOfLine);
		continueFlag = ListString ((char*)gTextString, 
											(UInt32)strlen ((char*)gTextString), 
											gOutputTextH);
		
		}	// end "if (continueFlag)" 
		
			// Ground heading.																	
		
	if (continueFlag)
		{
		if (gSwapBytesFlag)
			Swap4Bytes ((HUInt32Ptr)&headerRecordPtr[15], 1);
			
		sprintf ((char*)gTextString,
					"    Ground heading:       %u%s",
					(unsigned int)headerRecordPtr[15],
					gEndOfLine);
		continueFlag = ListString ((char*)gTextString, 
											(UInt32)strlen ((char*)gTextString), 
											gOutputTextH);
		
		}	// end "if (continueFlag)" 
		
			// Reformatting date.																
		
	if (continueFlag)
		{
		BlockMoveData (&headerRecordPtr[16], (char*)gTextString2, 12);
		gTextString2[12] = kNullTerminator;
		
		for (index=0; index<12; index++)
			{
			conversionIndex = gTextString2[index];
			gTextString2[index] = stringPtr[conversionIndex];
			
			}	// end "for (index=0; index<12; index++)" 
		
		sprintf ((char*)gTextString,
					"    Reformatting date:    %s%s", 
					gTextString2,
					gEndOfLine);
		continueFlag = ListString ((char*)gTextString,
											(UInt32)strlen ((char*)gTextString), 
											gOutputTextH);
		
		}	// end "if (continueFlag)" 
		
			// LARSYS Tape Number.																
		
	if (continueFlag)
		{
		if (gSwapBytesFlag)
			Swap4Bytes ((HUInt32Ptr)&headerRecordPtr[0], 1);
			
		sprintf ((char*)gTextString,
					"    LARSYS Tape Number:   %u%s",
					(unsigned int)headerRecordPtr[0],
					gEndOfLine);
		continueFlag = ListString ((char*)gTextString,
											(UInt32)strlen ((char*)gTextString), 
											gOutputTextH);
		
		}	// end "if (continueFlag)" 
		
			// LARSYS File Number.																
		
	if (continueFlag)
		{
		if (gSwapBytesFlag)
			Swap4Bytes ((HUInt32Ptr)&headerRecordPtr[1], 1);
			
		sprintf ((char*)gTextString,
					"    LARSYS File Number:   %u%s%s",
					(unsigned int)headerRecordPtr[1],
					gEndOfLine,
					gEndOfLine);
		continueFlag = ListString ((char*)gTextString,
											(UInt32)strlen ((char*)gTextString), 
											gOutputTextH);
		
		}	// end "if (continueFlag)" 
		
	gSwapBytesFlag = FALSE;
		
	CheckAndUnlockHandle ((Handle)stringHandle);
	ReleaseResource ((Handle)stringHandle);
	index = ResError ();
	
	return (continueFlag);

}	// end "ListLARSYSMISTHeaderInformation" 
																
																

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ListMapParameters
//
//	Software purpose:	The purpose of this routine is to list the descriptive
//							information for the active image window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 11/28/2000
//	Revised By:			Larry L. Biehl			Date: 02/28/2018

Boolean ListMapParameters (
				FileInfoPtr							fileInfoPtr,
				Boolean								listAllInformationFlag)

{
	unsigned char*						stringPtr;
	ControlPointsPtr					controlPointsPtr;
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	double								projAzimuthAngle,
											projCenterLat,
											projCenterLong,
											projFalseEasting,
											projFalseNorthing,
											projFalseOriginLat,
											projFalseOriginLong,
											projFalseOriginEasting,
											projFalseOriginNorthing,
											projNatOriginLat,
											projNatOriginLong,
											projScaleAtNatOrigin,
											projStdParallel1,
											projStdParallel2,
											projStraightVertPoleLongitude;
											
	UInt32								index;
	
	SInt16								numberChars,
											numberDigits,
											numberSizeDigits,
											referenceSystemCode;
	
	Boolean								continueFlag = TRUE;
	
	char									direction;
	

				
	mapProjectionInfoPtr = (MapProjectionInfoPtr)
										GetHandlePointer (fileInfoPtr->mapProjectionHandle,
																	kLock);
	
	if (mapProjectionInfoPtr != NULL)
		{
		numberDigits = 6;
		numberSizeDigits = 10;
		if (mapProjectionInfoPtr->planarCoordinate.mapUnitsCode == kDecimalDegreesCode)
			{
			numberDigits = 8;
			numberSizeDigits = 12;
			
			}	// end "if (...->planarCoordinate.mapUnitsCode == kDecimalDegreesCode)"
		
				// List planar map parameters.
				
		sprintf ((char*)gTextString,
						"    Planar Map Parameters%s",
						gEndOfLine);
				
		continueFlag = ListString ((char*)gTextString,
											(UInt32)strlen ((char*)gTextString),
											gOutputTextH);
			
				// List the x-map coordinate for pixel (1,1).				
				
		numberChars = sprintf (
									(char*)gTextString2,
									"%21.*f",
									numberDigits,
									mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11);	
										
		numberChars = InsertCommasInNumberString ((char*)gTextString2, 
																numberChars, 
																numberDigits,
																numberChars);			
				
		sprintf ((char*)gTextString,
					"      x-map coordinate for center of upper-left pixel (1,1): %23s%s",
					gTextString2,
					gEndOfLine);
				
		continueFlag = ListString ((char*)gTextString,
											(UInt32)strlen ((char*)gTextString),  
											gOutputTextH);
			
				// List the y-map coordinate for pixel (1,1).				
				
		numberChars = sprintf ((char*)gTextString2,
										"%21.*f",
										numberDigits,
										mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11);	
										
		numberChars = InsertCommasInNumberString ((char*)gTextString2, 
																numberChars, 
																numberDigits,
																numberChars);							
				
		sprintf ((char*)gTextString,
					"      y-map coordinate for center of upper-left pixel (1,1): %23s%s",
					gTextString2,
					gEndOfLine);
		
		if (continueFlag)		
			continueFlag = ListString ((char*)gTextString,
												(UInt32)strlen ((char*)gTextString),  
												gOutputTextH);
			
				// List the horizontal pixel size.					
				
		numberChars = sprintf (
								(char*)gTextString2,
								"%21.*f",
								numberSizeDigits,
								mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize);	
										
		numberChars = InsertCommasInNumberString ((char*)gTextString2, 
																numberChars, 
																numberSizeDigits,
																numberChars);									
				
		sprintf ((char*)gTextString,
					"      horizontal pixel size per pixel:                       %23s%s",
					gTextString2,
					gEndOfLine);
				
		if (continueFlag)	
			continueFlag = ListString ((char*)gTextString,
												(UInt32)strlen ((char*)gTextString),  
												gOutputTextH);
			
				// List the vertical pixel size.						
				
		numberChars = sprintf (
								(char*)gTextString2,
								"%21.*f",
								numberSizeDigits,
								mapProjectionInfoPtr->planarCoordinate.verticalPixelSize);	
										
		numberChars = InsertCommasInNumberString (
									(char*)gTextString2, 
									numberChars, 
									numberSizeDigits,
									numberChars);					
				
		sprintf ((char*)gTextString,
					"      vertical pixel size per pixel:                         %23s%s",
					gTextString2,
					gEndOfLine);
			
		if (continueFlag)		
			continueFlag = ListString ((char*)gTextString,
												(UInt32)strlen ((char*)gTextString),  
												gOutputTextH);
			
				// List the units.	
				
		#if defined multispec_mac 
			GetMenuItemText (gPopUpMapUnitsMenu, 
									mapProjectionInfoPtr->planarCoordinate.mapUnitsCode+1, 
									gTextString2);	
			gTextString2[gTextString2[0]+1] = 0;
		#endif	// defined multispec_mac   
                          
		#if defined multispec_win | defined multispec_wx
			MGetString (
					gTextString2,
					0,
					IDS_MapUnits01+mapProjectionInfoPtr->planarCoordinate.mapUnitsCode);
		#endif	// defined multispec_win

		sprintf ((char*)gTextString,
						"      map units:       %s%s",
						&gTextString2[1],
						gEndOfLine);
				
		if (continueFlag)	
			continueFlag = ListString ((char*)gTextString,  
												(UInt32)strlen ((char*)gTextString),
												gOutputTextH);
				
				// List z units (such as for DEM data)
				
		if (mapProjectionInfoPtr->planarCoordinate.zMapUnitsCode != kUnknownCode)
			{					
			#if defined multispec_mac 
				GetMenuItemText (gPopUpMapUnitsMenu, 
										mapProjectionInfoPtr->planarCoordinate.zMapUnitsCode+1, 
										gTextString2);	
				gTextString2[gTextString2[0]+1] = 0;
			#endif	// defined multispec_mac   
									  
			#if defined multispec_win | defined multispec_wx
				MGetString (
						gTextString2,
						0,
						IDS_MapUnits01+mapProjectionInfoPtr->planarCoordinate.zMapUnitsCode);
			#endif	// defined multispec_win

			sprintf ((char*)gTextString,
							"      z map units:     %s%s",
							&gTextString2[1],
							gEndOfLine);
					
			if (continueFlag)	
				continueFlag = ListString ((char*)gTextString,
													(UInt32)strlen ((char*)gTextString),
													gOutputTextH);
			
			}	// end "if (...->planarCoordinate.zMapUnitsCode != kUnknownCode)"
				
				// List the map orientation angle if other than 0
				
		if (mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle != 0)
			{
			sprintf ((char*)gTextString,
						"      map orientation angle:       %10.7f%s",
						mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle *
																						kRadiansToDegrees,
						gEndOfLine);
					
			if (continueFlag)	
				continueFlag = ListString ((char*)gTextString,
													(UInt32)strlen ((char*)gTextString),
													gOutputTextH);
				
			}	// end "if (...->planarCoordinate.mapOrientationAngle != 0)"
			
				// List the control points if there are any.
				
		controlPointsPtr = GetControlPointVectorPointers (
																fileInfoPtr->controlPointsHandle,
																kNoLock);
				
		if (controlPointsPtr != NULL)
			{
			if (controlPointsPtr->count <= 20 || listAllInformationFlag)
				{
				sprintf ((char*)gTextString,
							"%s      Control Points:  %u%s          Column       Line         MapX          MapY%s",
							gEndOfLine,
							(unsigned int)controlPointsPtr->count,
							gEndOfLine,
							gEndOfLine);
						
				if (continueFlag)	
					continueFlag = ListString ((char*)gTextString,
														(UInt32)strlen ((char*)gTextString),
														gOutputTextH);
								
				for (index=0; index<(UInt32)controlPointsPtr->count; index++)
					{
					sprintf ((char*)gTextString,
								"      %10.3f %10.3f %13.6f %13.6f%s",
								controlPointsPtr->easting1Ptr[index],
								controlPointsPtr->northing1Ptr[index],
								controlPointsPtr->easting2Ptr[index],
								controlPointsPtr->northing2Ptr[index],
								gEndOfLine);
						
					if (continueFlag)	
						continueFlag = ListString ((char*)gTextString,
															(UInt32)strlen ((char*)gTextString),
															gOutputTextH);
					
					}	// end "for (index=0; index<controlPointsPtr->count; index++)"
					
				}	// end "if (controlPointsPtr->count <= 20 && listAllInformationFlag)"
				
			else if (controlPointsPtr->count > 20)
				{
				sprintf ((char*)gTextString,
							"%s      There are %u control points. Use shift key with List Image Description... to force them to be listed.%s",
							gEndOfLine,
							(unsigned int)controlPointsPtr->count,
							gEndOfLine);
						
				if (continueFlag)	
					continueFlag = ListString ((char*)gTextString,
														(UInt32)strlen ((char*)gTextString),
														gOutputTextH);
				
				}	// end "else if (controlPointsPtr->count > 20)"
				
			CloseControlPointVectorPointers (fileInfoPtr->controlPointsHandle);
			
			}	// end "if (controlPointsPtr != NULL)"
			
				// List the polygon coefficients if there are any.
															
		if (mapProjectionInfoPtr->planarCoordinate.polynomialOrder > 0)
			{
			GetCoefficientsVectorPointers (mapProjectionInfoPtr);
						
			sprintf ((char*)gTextString,
						"%s      Coordinate Polygon Model (order = %d)%s"
						"                             Column/Line To Map           Map to Column/Line%s"
						"                            Easting      Northing       Easting      Northing%s",
						gEndOfLine,
						mapProjectionInfoPtr->planarCoordinate.polynomialOrder,
						gEndOfLine,
						gEndOfLine,
						gEndOfLine);
					
			if (continueFlag)	
				continueFlag = ListString ((char*)gTextString,
													(UInt32)strlen ((char*)gTextString),
													gOutputTextH);
							
			for (index=0; index<mapProjectionInfoPtr->numberCoefficients; index++)
				{
				sprintf ((char*)gTextString,
							"        Coefficient%2d %13.6g %13.6g %13.6g %13.6g%s",
							(unsigned int)index,
							mapProjectionInfoPtr->planarCoordinate.easting1CoefficientsPtr[index], 
							mapProjectionInfoPtr->planarCoordinate.northing1CoefficientsPtr[index], 
							mapProjectionInfoPtr->planarCoordinate.easting2CoefficientsPtr[index], 
							mapProjectionInfoPtr->planarCoordinate.northing2CoefficientsPtr[index],
							gEndOfLine);
					
				if (continueFlag)	
					continueFlag = ListString ((char*)gTextString,
														(UInt32)strlen ((char*)gTextString),
														gOutputTextH);
				
				}	// end "for (index=0; index<...->numberCoefficients; index++)"
															
			CloseCoefficientsVectorPointers (mapProjectionInfoPtr);
			
			}	// end "if (mapProjectionInfoPtr->planarCoordinate.polynomialOrder > 0)"
				
				// List Grid Coordinate System Parameters
				
		sprintf ((char*)gTextString,
					"%s    Grid Coordinate System Parameters%s",
					gEndOfLine,
					gEndOfLine);
				
		if (continueFlag)	
			continueFlag = ListString ((char*)gTextString,
												(UInt32)strlen ((char*)gTextString),
												gOutputTextH);
				
		continueFlag = ListMapReferenceSystemString (fileInfoPtr->mapProjectionHandle,
																	(char*)"      ", 
																	continueFlag);
								 								
		referenceSystemCode = mapProjectionInfoPtr->gridCoordinate.referenceSystemCode;

		if (referenceSystemCode > 0)
			{	
			gTextString[0] = 0;		
			if (referenceSystemCode == kByEPSGCodeCode)
				{
						// List the EPSG Code and Name
						
				sprintf ((char*)gTextString,
							"      EPSG Code:        %d  %s%s",
							mapProjectionInfoPtr->projectedCSTypeGeoKey,
							&mapProjectionInfoPtr->gridCoordinate.epsgName[1],
							gEndOfLine);
						
				}	// end "if (referenceSystemCode == kByEPSGCodeCode)"
	
			else if (mapProjectionInfoPtr->gridCoordinate.gridZone != 0)	
				{
				switch (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode)
					{																
					case kStatePlaneNAD27RSCode:							
					case kStatePlaneNAD83RSCode:
						sprintf ((char*)gTextString,
									"      FIPS Zone:        %d  %s%s",
									abs (mapProjectionInfoPtr->gridCoordinate.gridZone),
									&mapProjectionInfoPtr->gridCoordinate.epsgName[1],
									gEndOfLine);
						break;
											
					case kGaussKrugerRSCode:
						sprintf ((char*)gTextString,
									"      Zone:             %d%s",
									abs (mapProjectionInfoPtr->gridCoordinate.gridZone),
									gEndOfLine);
						break;
											
					case kGDA94RSCode:
						sprintf ((char*)gTextString,
									"      MGA Zone:         %d%s",
									abs (mapProjectionInfoPtr->gridCoordinate.gridZone),
									gEndOfLine);
						break;
						
					case kUTM_NAD27RSCode:
					case kUTM_NAD83RSCode:
					case kUTM_SAD69RSCode:
					case kUTM_WGS72RSCode:
					case kUTM_WGS84RSCode:
						direction = 'N';
						if (mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 < 0 ||
												mapProjectionInfoPtr->gridCoordinate.gridZone < 0)
							direction = 'S';
							
						sprintf ((char*)gTextString,
									"      Zone:             %d%c%s",
									abs (mapProjectionInfoPtr->gridCoordinate.gridZone),
									direction,
									gEndOfLine);
						break;
						
					}	// end "switch (...->gridCoordinate.referenceSystemCode)"
							
				}	// end "if (...->gridCoordinate.gridZone != 0)"
							
			if (continueFlag && gTextString[0] > 0)	
				continueFlag = ListString ((char*)gTextString,
													(UInt32)strlen ((char*)gTextString),
													gOutputTextH);
				
			continueFlag = ListMapProjectionString (fileInfoPtr->mapProjectionHandle,
																	(char*)"      ", 
																	continueFlag);
			
			GetProjectionParameters (mapProjectionInfoPtr,
										kIncludeUTM,
										&projStdParallel1,
										&projStdParallel2,
										&projNatOriginLong,
										&projNatOriginLat,
										&projFalseEasting,
										&projFalseNorthing,
										&projFalseOriginLong,
										&projFalseOriginLat,
										&projFalseOriginEasting,
										&projFalseOriginNorthing,
										&projCenterLong,
										&projCenterLat,
										&projScaleAtNatOrigin,
										&projAzimuthAngle,
										&projStraightVertPoleLongitude);
			
			if (projStdParallel1 != -9999)	
				{		
				sprintf ((char*)gTextString,
							"        Standard Parallel1:          %15.6f%s",
							projStdParallel1,
							gEndOfLine);
						
				if (continueFlag)	
					continueFlag = ListString ((char*)gTextString,
														(UInt32)strlen ((char*)gTextString),
														gOutputTextH);
						
				}	// end "if (projStdParallel1 != -9999)"
			
			if (projStdParallel2 != -9999)	
				{		
				sprintf ((char*)gTextString,
							"        Standard Parallel2:          %15.6f%s",
							projStdParallel2,
							gEndOfLine);
						
				if (continueFlag)	
					continueFlag = ListString ((char*)gTextString,
														(UInt32)strlen ((char*)gTextString),
														gOutputTextH);
						
				}	// end "if (projStdParallel1 != -9999)"
			
			if (projNatOriginLat != -9999)	
				{		
				sprintf ((char*)gTextString,
							"        Natural Origin Latitude:     %15.6f%s",
							projNatOriginLat,
							gEndOfLine);
						
				if (continueFlag)	
					continueFlag = ListString ((char*)gTextString,
														(UInt32)strlen ((char*)gTextString),
														gOutputTextH);
						
				}	// end "if (projNatOriginLat != -9999)"
			
			if (projNatOriginLong != -9999)	
				{		
				sprintf ((char*)gTextString,
							"        Natural Origin Longitude:    %15.6f%s",
							projNatOriginLong,
							gEndOfLine);
						
				if (continueFlag)	
					continueFlag = ListString ((char*)gTextString,
														(UInt32)strlen ((char*)gTextString),
														gOutputTextH);
						
				}	// end "if (projNatOriginLong != -9999)"
			
			if (projFalseEasting != -9999)	
				{			
				sprintf ((char*)gTextString,
							"        False Easting:               %15.6f%s",
							projFalseEasting,
							gEndOfLine);
						
				if (continueFlag)	
					continueFlag = ListString ((char*)gTextString,
														(UInt32)strlen ((char*)gTextString),
														gOutputTextH);
						
				}	// end "if (projFalseEasting != -9999)"
			
			if (projFalseNorthing != -9999)	
				{		
				sprintf ((char*)gTextString,
							"        False Northing:              %15.6f%s",
							projFalseNorthing,
							gEndOfLine);
									
				if (continueFlag)	
					continueFlag = ListString ((char*)gTextString,
														(UInt32)strlen ((char*)gTextString),
														gOutputTextH);
						
				}	// end "if (projFalseNorthing != -9999)"
			
			if (projFalseOriginLong != -9999)	
				{		
				sprintf ((char*)gTextString,
							"        False Origin Longitude:      %15.6f%s",
							projFalseOriginLong,
							gEndOfLine);
						
				if (continueFlag)	
					continueFlag = ListString ((char*)gTextString,  
														(UInt32)strlen ((char*)gTextString),
														gOutputTextH);
						
				}	// end "if (projFalseOriginLong != -9999)"
			
			if (projFalseOriginLat != -9999)	
				{		
				sprintf ((char*)gTextString,
							"        False Origin Latitude:       %15.6f%s",
							projFalseOriginLat,
							gEndOfLine);
						
				if (continueFlag)	
					continueFlag = ListString ((char*)gTextString,
														(UInt32)strlen ((char*)gTextString),
														gOutputTextH);
						
				}	// end "if (projFalseOriginLat != -9999)"
			
			if (projFalseOriginEasting != -9999)	
				{			
				sprintf ((char*)gTextString,
							"        False Origin Easting:        %15.6f%s",
							projFalseOriginEasting,
							gEndOfLine);
						
				if (continueFlag)	
					continueFlag = ListString ((char*)gTextString,
														(UInt32)strlen ((char*)gTextString),
														gOutputTextH);
						
				}	// end "if (projFalseOriginEasting != -9999)"
			
			if (projFalseOriginNorthing != -9999)	
				{		
				sprintf ((char*)gTextString,
							"        False Origin Northing:       %15.6f%s",
							projFalseOriginNorthing,
							gEndOfLine);
									
				if (continueFlag)	
					continueFlag = ListString ((char*)gTextString,
														(UInt32)strlen ((char*)gTextString),
														gOutputTextH);
						
				}	// end "if (projFalseOriginNorthing != -9999)"
			
			if (projCenterLong != -9999)	
				{		
				sprintf ((char*)gTextString,
							"        Longitude Central Meridian:  %15.6f%s",
							projCenterLong,
							gEndOfLine);
						
				if (continueFlag)	
					continueFlag = ListString ((char*)gTextString,  
														(UInt32)strlen ((char*)gTextString),
														gOutputTextH);
						
				}	// end "if (projCenterLong != -9999)"
			
			if (projCenterLat != -9999)	
				{			
				sprintf ((char*)gTextString,
							"        Latitude of Origin:          %15.6f%s",
							projCenterLat,
							gEndOfLine);
						
				if (continueFlag)	
					continueFlag = ListString ((char*)gTextString,
														(UInt32)strlen ((char*)gTextString),
														gOutputTextH);
						
				}	// end "if (projCenterLat != -9999)"
			
			if (projScaleAtNatOrigin != -9999)	
				{			
				sprintf ((char*)gTextString,
							"        Scale Factor at Origin:      %13.10f%s",
							projScaleAtNatOrigin,
							gEndOfLine);
						
				if (continueFlag)	
					continueFlag = ListString ((char*)gTextString,
														(UInt32)strlen ((char*)gTextString),
														gOutputTextH);
						
				}	// end "if (projScaleAtNatOrigin != -9999)"
			
			if (projAzimuthAngle != -9999)	
				{			
				sprintf ((char*)gTextString,
							"        Projected Azimuth Angle:     %15.6f%s",
							projAzimuthAngle,
							gEndOfLine);
						
				if (continueFlag)	
					continueFlag = ListString ((char*)gTextString,
														(UInt32)strlen ((char*)gTextString),
														gOutputTextH);
						
				}	// end "if (projAzimuthAngle != -9999)"
			
			if (projStraightVertPoleLongitude != -9999)	
				{		
				sprintf ((char*)gTextString,
							"        Longitude Central Meridian:  %15.6f%s",
							projStraightVertPoleLongitude,
							gEndOfLine);
						
				if (continueFlag)	
					continueFlag = ListString ((char*)gTextString,
														(UInt32)strlen ((char*)gTextString),
														gOutputTextH);
						
				}	// end "if (projStraightVertPoleLongitude != -9999)"
				
			}	// end "if (gridCoordinateCode > 0)"
				
		if (mapProjectionInfoPtr->geodetic.spheroidCode > 0 ||
					mapProjectionInfoPtr->geodetic.datumCode > 0 ||
								referenceSystemCode == kByEPSGCodeCode)
			{
					// List Geodetic Model Parameters
					
			sprintf ((char*)gTextString,
						"%s    Geodetic Model Parameters%s",
						gEndOfLine,
						gEndOfLine);
					
			if (continueFlag)	
				continueFlag = ListString ((char*)gTextString,
													(UInt32)strlen ((char*)gTextString),
													gOutputTextH);
			
			if (mapProjectionInfoPtr->geodetic.datumCode > 0 || 
										referenceSystemCode == kByEPSGCodeCode)
				{
				if (referenceSystemCode == kByEPSGCodeCode)
					stringPtr = &mapProjectionInfoPtr->gridCoordinate.datumName[1];
				
				else	// referenceSystemCode != kByEPSGCodeCode
					{
					#if defined multispec_mac 
						GetMenuItemText (gPopUpHorizontalDatumMenu, 
												mapProjectionInfoPtr->geodetic.datumCode+1, 
												gTextString2);
						gTextString2[gTextString2[0]+1] = 0;
					#endif	// defined multispec_mac   
										  
					#if defined multispec_win | defined multispec_wx
						MGetString (gTextString2,
								0,
							IDS_Datum00+mapProjectionInfoPtr->geodetic.datumCode);
					#endif	// defined multispec_win
					
					stringPtr = &gTextString2[1];
					
					}	// end "else referenceSystemCode != kByEPSGCodeCode"
									
				sprintf ((char*)gTextString,
							"      Horizontal Datum: %s%s",
							stringPtr,
							gEndOfLine);
						
				if (continueFlag)	
					continueFlag = ListString ((char*)gTextString,
														(UInt32)strlen ((char*)gTextString),
														gOutputTextH);
				
				}	// end "if (mapProjectionInfoPtr->geodetic.datumCode > 0 || ..."
			
			if (mapProjectionInfoPtr->geodetic.spheroidCode > 0 || 
															referenceSystemCode == kByEPSGCodeCode)
				{
				if (referenceSystemCode == kByEPSGCodeCode)
					stringPtr = &mapProjectionInfoPtr->gridCoordinate.ellipsoidName[1];
				
				else	// referenceSystemCode != kByEPSGCodeCode
					{
					#if defined multispec_mac 
						GetMenuItemText (gPopUpEllipsoidMenu, 
												mapProjectionInfoPtr->geodetic.spheroidCode+1, 
												gTextString2);
						gTextString2[gTextString2[0]+1] = 0;
					#endif	// defined multispec_mac   
										  
					#if defined multispec_win || defined multispec_wx
						MGetString (
								gTextString2,
								0,
								IDS_Ellipsoid01+mapProjectionInfoPtr->geodetic.spheroidCode);
					#endif	// defined multispec_win	
					
					stringPtr = &gTextString2[1];
					
					}	// end "else referenceSystemCode != kByEPSGCodeCode"		
									
				sprintf ((char*)gTextString,
							"      Ellipsoid:        %s%s",
							stringPtr,
							gEndOfLine);
						
				if (continueFlag)	
					continueFlag = ListString ((char*)gTextString,
														(UInt32)strlen ((char*)gTextString),
														gOutputTextH);
						
				if (mapProjectionInfoPtr->geodetic.spheroidCode == kSphereEllipsoidCode)
					{
					sprintf ((char*)gTextString,
								"        Radius:           %13.5f%s",
								mapProjectionInfoPtr->geodetic.radiusSpheroid,
								gEndOfLine);
						
					if (continueFlag)	
						continueFlag = ListString ((char*)gTextString,
															(UInt32)strlen ((char*)gTextString),
															gOutputTextH);
					
					}	// end "if (...->geodetic.spheroidCode == kSphereEllipsoidMenuItem)"
											
				else	// ...->geodetic.spheroidCode != kSphereEllipsoidMenuItem
					{
					sprintf ((char*)gTextString,
								"        Equatorial radius:  %13.5f%s",
								mapProjectionInfoPtr->geodetic.semiMajorAxis,
								gEndOfLine);
						
					if (continueFlag)	
						continueFlag = ListString ((char*)gTextString,
															(UInt32)strlen ((char*)gTextString),
															gOutputTextH);
							
					sprintf ((char*)gTextString,
								"        Polar radius:       %13.5f%s",
								mapProjectionInfoPtr->geodetic.semiMinorAxis,
								gEndOfLine);
						
					if (continueFlag)	
						continueFlag = ListString ((char*)gTextString,
															(UInt32)strlen ((char*)gTextString),
															gOutputTextH);
						
					}	// end "else ...->geodetic.spheroidCode != kSphereEllipsoidMenuItem"
				
				}	// end "if (mapProjectionInfoPtr->geodetic.spheroidCode > 0)"
			
			}	// end "mapProjectionInfoPtr->geodetic.spheroidCode > 0 || ..."
			
		CheckAndUnlockHandle (fileInfoPtr->mapProjectionHandle);
				
		if (continueFlag)	
			continueFlag = ListString (
						(char*)gEndOfLine,  gNumberOfEndOfLineCharacters,  gOutputTextH);
			
		}	// end "if (mapProjectionInfoPtr != NULL)"
		
	return (continueFlag);

}	// end "ListMapParameters" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean LoadUtilityEvaluationSpecs
//
//	Software purpose:	The purpose of this routine is to load the utility 
//							structure with the default set of
//							Evaluate Covariance Specification parameters.
//
//	Parameters in:		Pointer to the image file information structure.
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			EvaluateCovariancesControl
//
//	Coded By:			Larry L. Biehl			Date: 10/18/1990
//	Revised By:			Larry L. Biehl			Date: 05/13/1998

Boolean LoadEvaluateCovariancesSpecs (
				FileInfoPtr							fileInfoPtr)

{
	EvaluateCovarianceSpecsPtr		evaluateCovarianceSpecsPtr;
	
	SInt32								bytesNeeded;
	
	Boolean								changedFlag,
											returnFlag;
	
	
			// Assume routine will work okay.  If there isn't enough memory		
			// then the flag will be changed.												
			
	returnFlag = TRUE;
		
			// Get a handle to the utility structure if needed.									
	
	if (gProjectInfoPtr->evalCovarianceSpecsHandle == NULL)
		{
		gProjectInfoPtr->evalCovarianceSpecsHandle = 
													MNewHandle (sizeof (EvaluateCovarianceSpecs));
		
		evaluateCovarianceSpecsPtr = (EvaluateCovarianceSpecsPtr)GetHandlePointer (
								gProjectInfoPtr->evalCovarianceSpecsHandle, kLock, kMoveHi);
		
		if (evaluateCovarianceSpecsPtr != NULL)
			{
			
					// Initialize the utility specification structure.	
					
			evaluateCovarianceSpecsPtr->classPtr = NULL;
			evaluateCovarianceSpecsPtr->featurePtr = NULL;
			evaluateCovarianceSpecsPtr->channelsPtr = NULL;
					
			evaluateCovarianceSpecsPtr->featureHandle = NULL;
			evaluateCovarianceSpecsPtr->channelsHandle = NULL;
			evaluateCovarianceSpecsPtr->classHandle = NULL;				
					
			evaluateCovarianceSpecsPtr->classSet = kAllMenuItem;
			evaluateCovarianceSpecsPtr->channelSet = kAllMenuItem;
			evaluateCovarianceSpecsPtr->numberChannels =
													gProjectInfoPtr->numberStatisticsChannels;
			evaluateCovarianceSpecsPtr->numberClasses =
													gProjectInfoPtr->numberStatisticsClasses;
			evaluateCovarianceSpecsPtr->numberFeatures = 
													gProjectInfoPtr->numberStatisticsChannels;
													
			evaluateCovarianceSpecsPtr->featureTransformationFlag = FALSE;
			
			evaluateCovarianceSpecsPtr->listOriginalMatrixFlag = FALSE;
			evaluateCovarianceSpecsPtr->listInvertedMatrixFlag = FALSE;
			evaluateCovarianceSpecsPtr->listOriginalXInvertedMatrixFlag = FALSE;
			evaluateCovarianceSpecsPtr->listInvertedInvertedMatrixFlag = FALSE;
			
			}	// end "if (evaluateCovarianceSpecsPtr != NULL)" 
		
		else	// else *evaluateCovarianceSpecsPtr == NULL 
			returnFlag = FALSE;
			
		}	// end "if (gProjectInfoPtr->evalCovarianceSpecsHandle == NULL)" 
		
	else	// gProjectInfoPtr->evalCovarianceSpecsHandle != NULL
		{
				// Lock the handle to the utility specifications and get				
				// a pointer to the structure.												
				
		evaluateCovarianceSpecsPtr = (EvaluateCovarianceSpecsPtr)GetHandlePointer (
								gProjectInfoPtr->evalCovarianceSpecsHandle, kLock, kMoveHi);
		
				// Make sure that the featureTransformationFlag makes sense.
										
		if (!EigenvectorInfoExists (kProject))
			evaluateCovarianceSpecsPtr->featureTransformationFlag = FALSE;
				
		}	// end "else gProjectInfoPtr->evalCovarianceSpecsHandle != NULL" 
					
			// Check memory for utility classes handle.									
		
	if (returnFlag)
		{	
		bytesNeeded = 
					(SInt32)gProjectInfoPtr->numberStatisticsClasses * sizeof (SInt16);
		evaluateCovarianceSpecsPtr->classPtr = (SInt16*)CheckHandleSize (
														&evaluateCovarianceSpecsPtr->classHandle,
														&returnFlag,  
														&changedFlag, 
														bytesNeeded);
									
		if (changedFlag)
			evaluateCovarianceSpecsPtr->classSet = kAllMenuItem;
			
		}	// end "if (returnFlag)" 
			
			// If memory is not full, check memory for utility feature handle.															

	if (returnFlag)
		{
		bytesNeeded =
					(SInt32)gProjectInfoPtr->numberStatisticsChannels * sizeof (SInt16);
			
		evaluateCovarianceSpecsPtr->featurePtr = (SInt16*)CheckHandleSize (
														&evaluateCovarianceSpecsPtr->featureHandle, 
														&returnFlag,   
														&changedFlag, 
														bytesNeeded);
									
		if (changedFlag)
			evaluateCovarianceSpecsPtr->channelSet = kAllMenuItem;
			
		}	// end "if (returnFlag)" 
			
			// If memory is not full, check memory for utility channels vector.	

	if (returnFlag)
		{
		evaluateCovarianceSpecsPtr->channelsPtr = (SInt16*)CheckHandleSize (
														&evaluateCovarianceSpecsPtr->channelsHandle,
														&returnFlag,   
														&changedFlag, 
														bytesNeeded);
			
		}	// end "if (returnFlag)" 
		
	if (returnFlag)
		{
				// Make certain that feature and class vectors are filled in 		
				// properly.																		
		
		if (evaluateCovarianceSpecsPtr->channelSet == kAllMenuItem)		
			LoadFeatureVector (evaluateCovarianceSpecsPtr->featureTransformationFlag,
										(SInt16*)&evaluateCovarianceSpecsPtr->numberChannels,
										evaluateCovarianceSpecsPtr->featurePtr,
										gProjectInfoPtr->numberStatisticsChannels);
															
		LoadChannelsVector (kProject,
									evaluateCovarianceSpecsPtr->featureTransformationFlag,
									evaluateCovarianceSpecsPtr->numberFeatures, 
									evaluateCovarianceSpecsPtr->featurePtr,
									(SInt16*)&evaluateCovarianceSpecsPtr->numberChannels, 
									evaluateCovarianceSpecsPtr->channelsPtr);
		
		if (evaluateCovarianceSpecsPtr->classSet == kAllMenuItem)								
			LoadTrainClassVector (&evaluateCovarianceSpecsPtr->numberClasses,
											&evaluateCovarianceSpecsPtr->classSet, 
											evaluateCovarianceSpecsPtr->classPtr);
			
		}	// end "if (returnFlag)" 
	
			// If unable to set up information for utility								
			// specifications, release memory allocated to it (if any).				
			
	else	// !returnFlag 
		ReleaseEvalCovarianceHandleMemory (&gProjectInfoPtr->evalCovarianceSpecsHandle);
		
	return (returnFlag);

}	// end "LoadUtilityEvaluationSpecs" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean LoadEvaluateTransformationSpecs
//
//	Software purpose:	The purpose of this routine is to load the utility 
//							structure with the default set of
//							Evaluate Transformation Specification parameters.
//
//	Parameters in:		Pointer to the image file information structure.
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			EvaluateCovariancesControl
//
//	Coded By:			Larry L. Biehl			Date: 06/27/1994
//	Revised By:			Larry L. Biehl			Date: 05/13/1998

Boolean LoadEvaluateTransformationSpecs (void)

{
	EvaluateTransformSpecsPtr		evaluateTransformSpecsPtr;
	
	Boolean								returnFlag;
	
	
			// Assume routine will work okay.  If there isn't enough memory		
			// then the flag will be changed.												
			
	returnFlag = TRUE;
		
			// Get a handle to the utility structure if needed.									
	
	if (gNonProjProcessorSpecs.evalTransformationSpecsH == NULL)
		{
		gNonProjProcessorSpecs.evalTransformationSpecsH = 
													MNewHandle (sizeof (EvaluateTransformSpecs));
		
		evaluateTransformSpecsPtr = (EvaluateTransformSpecsPtr)GetHandlePointer (
						gNonProjProcessorSpecs.evalTransformationSpecsH, kLock, kMoveHi);
		
		if (evaluateTransformSpecsPtr != NULL)
			{
					// Initialize the utility specification structure.					
					
			evaluateTransformSpecsPtr->checkTransformationFlag = FALSE;
			evaluateTransformSpecsPtr->listEigenvaluesFlag = TRUE;
			evaluateTransformSpecsPtr->listTransformFlag = TRUE;
			evaluateTransformSpecsPtr->listTransformXTposedTransformFlag = TRUE;
			
			}	// end "if (evaluateTransformSpecsPtr != NULL)" 
		
		else	// else evaluateTransformSpecsPtr == NULL 
			returnFlag = FALSE;
			
		}	// end "if (gNonProjProcessorSpecs.evalTransformationSpecsH == NULL)"
		
	return (returnFlag);

}	// end "LoadEvaluateTransformationSpecs" 

