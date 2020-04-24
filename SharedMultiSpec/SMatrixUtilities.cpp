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
//	File:						SMatrixUtilities.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			09/13/2019
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	The purpose of the routines in this file is to
//								provide matrix utility type functions for MultiSpec 
//								that are to be loaded all the time.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include "Ssvm.h"

#if defined multispec_wx
#endif

#if defined multispec_mac
	#define	IDS_Utility11			11
#endif	// defined multispec_mac    

#if defined multispec_win
	#include "SImageWindow_class.h"
#endif	// defined multispec_win

#include 	"errno.h"



void ReduceMatrix1 (
				HDoublePtr							inputMatrixPtr,
				HDoublePtr							outputMatrixPtr, 
				SInt16								inputSize1, 
				SInt16								inputSize2, 
				SInt16								inputSize3, 
				SInt16*								featureListPtr);

void ReduceMatrix2 (
				HDoublePtr							inputMatrixPtr,
				HDoublePtr							outputMatrixPtr, 
				UInt32								inputSize1, 
				UInt32								inputSize2, 
				SInt16*								featureListPtr);

Boolean ListEigenTransformationInformation (
				HDoublePtr							covariancePtr,
				HDoublePtr							eigenVectorPtr, 
				SInt32								numberIterations, 
				double								smallestElement, 
				UInt16*								channelsPtr, 
				UInt16								numberChannels,
				UInt16								numberComponents, 
				Boolean								listEigenvalueFlag, 
				Boolean								listEigenvectorFlag);

Boolean ListOffsetGainTransformationInformation (
				HDoublePtr							offsetVectorPtr,
				HDoublePtr							gainVectorPtr,
				UInt16								numberChannels,
				Boolean								listOffsetGainFlag);



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void AddToClassStatistics
//
//	Software purpose:	The purpose of this routine is to add the statistics
//							from the input to that for the output.  This is used
//							for adding a field to a class or merging two classes.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			GetTotalSumSquares in SMatrixUtilities.cpp
//							GetClassSumsSquares in SProjectComputeStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/17/1988
//	Revised By:			Larry L. Biehl			Date: 07/30/1992	
														
void AddToClassStatistics (
				UInt16								numberOutputChannels, 
				HChannelStatisticsPtr			outChannelStatsPtr, 
				HSumSquaresStatisticsPtr		outputSumSquaresPtr, 
				UInt16								numberInputChannels, 
				UInt16*								channelListPtr, 
				HChannelStatisticsPtr			inChannelStatsPtr, 
				HSumSquaresStatisticsPtr		inputSumSquaresPtr, 
				Boolean								squareOutputMatrixFlag, 
				SInt16								inputStatisticsCode, 
				SInt16								outputStatisticsCode)

{
	HChannelStatisticsPtr			lInChannelStatsPtr; 
	
	UInt32								lowerLeftIndexSkip,
											channel,
											channelNum2,
											covChan,
											inputMatrixSkip;
	
	UInt16								channelListIndexLimit,
											channelListIndex1,
											channelListIndex2,
											channelNum1; 
	
	
			// Initialize local variables.													
				
	channelListIndexLimit = numberOutputChannels - 1;
	lowerLeftIndexSkip = numberOutputChannels - 1;
	channelNum1 = 0;
	channelListIndex1 = 0;
	inputMatrixSkip = 1;
	lInChannelStatsPtr = inChannelStatsPtr;
																						
	if (numberOutputChannels == numberInputChannels)
		channelListPtr = NULL;
	
	for (channel=0; channel<numberInputChannels; channel++)
		{
		if (channelListPtr)
			{
			channelNum1 = channelListPtr[channelListIndex1];
			lInChannelStatsPtr = &inChannelStatsPtr[channelNum1];
			
			}	// end "if (channelListPtr)" 
			
		if (channel == channelNum1)
			{
			outChannelStatsPtr->sum += lInChannelStatsPtr->sum;
			
			outChannelStatsPtr->minimum = 
				MIN (outChannelStatsPtr->minimum, lInChannelStatsPtr->minimum);
			
			outChannelStatsPtr->maximum = 
				MAX (outChannelStatsPtr->maximum, lInChannelStatsPtr->maximum);
				
			if (outputStatisticsCode == kMeanStdDevOnly)
				{
				inputSumSquaresPtr += (inputMatrixSkip-1);
				*outputSumSquaresPtr += *inputSumSquaresPtr;
				outputSumSquaresPtr++;
				inputSumSquaresPtr++;
				
				}	// end "if (outputStatisticsCode == kMeanStdDevOnly)" 
				
			else	// outputStatisticsCode == kMeanCovariance 
				{
				channelListIndex2 = 0;
				channelNum2 = 0;
				for (covChan=channelNum2; covChan<=channel; covChan++)
					{	
					if (channelListPtr)
						channelNum2 = channelListPtr[channelListIndex2];
					
					if (covChan == channelNum2)
						{
						*outputSumSquaresPtr += *inputSumSquaresPtr;
						outputSumSquaresPtr++;
			
						if (channelListIndex2 < channelListIndexLimit)
						 	channelListIndex2++;
						 	
						 }	// end "if (covChan == channelNum2)" 
										
					inputSumSquaresPtr++;
			 		channelNum2++;
				
					}	// end "for (covChan=channelNum2; covChan<=..." 
				
				if (squareOutputMatrixFlag)
					{
					outputSumSquaresPtr += lowerLeftIndexSkip;
					lowerLeftIndexSkip--; 
					
					}	// end "if (squareOutputMatrixFlag)" 
					
				}	// end "else outputStatisticsCode == kMeanCovariance" 
			
			if (channelListIndex1 < channelListIndexLimit)
				channelListIndex1++;
				
			outChannelStatsPtr++;
			
			}	// end "if (channel == channelNum1)" 
			
		else	// channel != channelNum1 
			inputSumSquaresPtr += inputMatrixSkip;
			
		channelNum1++;
		lInChannelStatsPtr++;
		if (inputStatisticsCode == kMeanCovariance)
			inputMatrixSkip++;
			
		}	// end "for (channel=0; channel<=..." 

}	// end "AddToClassStatistics"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void AddVectors
//
//	Software purpose:	The purpose of this routine is to add inputVector1 
//							to inputVector2 and put the result in 
//							outputVector. If inputVector1 and outputVector 
//							(or inputVector2 and outputVector) point to the same 
//							place, the addition is done "in place."
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Joe Hoffbeck			Date: 10/09/1997
//	Revised By:			Larry L. Biehl			Date: 10/09/1997	

void AddVectors (
				HDoublePtr 							inputVector1Ptr, 
				HDoublePtr 							inputVector2Ptr, 
				HDoublePtr							outputVectorPtr,
				UInt32								numberVectorRows)
				
{
	UInt32								i;
													
	
	for (i=0; i<numberVectorRows; i++)
		{
		*outputVectorPtr = *inputVector1Ptr + *inputVector2Ptr;
		
		inputVector1Ptr++;
		inputVector2Ptr++;
		outputVectorPtr++;
		
		}	// end "for (i=0; i<numberVectorRows; i++)"
		
}	// end "AddVectors"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				void ClearTransformationMatrix
//
//	Software purpose:			The purpose of this routine is to clear the transformation
// 								matrix from memory.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/12/1993
//	Revised By:			Larry L. Biehl			Date: 05/04/2000

void ClearTransformationMatrix (
				Boolean								notifyUserFlag)

{
	SInt16								returnCode;
	
	Boolean								continueFlag = TRUE;
	
	
	returnCode = 1;
	
	if (notifyUserFlag)
		returnCode = DisplayAlert (
					kYesCancelAlertID, kCautionAlert, kAlertStrID, IDS_Alert21, 0, NULL);
	
	if (returnCode == 1)
		{
		gTransformationMatrix.eigenValueHandle = 
						UnlockAndDispose (gTransformationMatrix.eigenValueHandle);
		gTransformationMatrix.eigenVectorHandle = 
						UnlockAndDispose (gTransformationMatrix.eigenVectorHandle);
		gTransformationMatrix.eigenFeatureHandle = 
						UnlockAndDispose (gTransformationMatrix.eigenFeatureHandle);
		gTransformationMatrix.preProcBandGroupingHandle = 
				UnlockAndDispose (gTransformationMatrix.preProcBandGroupingHandle);
		
		gTransformationMatrix.createdByCode = 0;
		gTransformationMatrix.numberChannels = 0;
		gTransformationMatrix.numberFeatures = 0;
		gTransformationMatrix.eigenVectorPtr = NULL; 
		gTransformationMatrix.tempMatrixPtr = NULL; 
		gTransformationMatrix.eigenFeaturePtr = NULL;
		gTransformationMatrix.preProcBandGroupingHandle = NULL;
		
		gUpdateFileMenuItemsFlag = TRUE;
		gUpdateEditMenuItemsFlag = TRUE;
		gUpdateProcessorMenuItemsFlag = TRUE;
		
		if (notifyUserFlag)
			{
					// Force text selection to start from end of present text.			
							
			ForceTextToEnd (); 
		
					// List the dashed line separator string to indicate start clearing
					// transformation output description.
					
			continueFlag = ListSpecifiedStringNumber (
													kSharedStrID, 
													IDS_Shared5, 
													(unsigned char*)gTextString, 
													NULL, 
													gOutputForce1Code, 
													continueFlag);
				
			ListSpecifiedStringNumber (
										kFileIOStrID, 
										IDS_FileIO132, 
										(unsigned char*)gTextString, 
										NULL,
										gOutputForce1Code,
										continueFlag);
						
					// Scroll output window to the selection and adjust the 				
					// scroll bar.																		
			
			UpdateOutputWScrolls (gOutputWindow, 1, kDisplayMessage);
			
			}	// end "if (notifyUserFlag)"
 		
		}	// end "if (returnCode == 1)" 
	
}	// end "ClearTransformationMatrix"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ComputeCorrelationMatrix
//
//	Software purpose:	The purpose of this routine is to compute a
//							square correlation matrix from the input sums and
//							sums of squares.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 08/26/1997
//	Revised By:			Larry L. Biehl			Date: 08/20/2010	

void ComputeCorrelationMatrix (
				UInt16								numberOutputChannels, 
				HCovarianceStatisticsPtr		correlationPtr, 
				UInt16								numberInputChannels, 
				UInt16*								channelListPtr, 
				HSumSquaresStatisticsPtr		sumSquaresPtr,
				SInt64								numberPixels, 
				Boolean								squareOutputMatrixFlag)

{	
	HCovarianceStatisticsPtr		savedCorrelationPtr;
	
	UInt32								channel,
											covChan,
											lowerLeftIndexSkip;
												
	Boolean								squareInputMatrixFlag;
	
	
			// Check input values.  Continue only if input parameters are within	
			// proper rangers																		
			
	if (numberOutputChannels == 0 || numberInputChannels == 0 || 
												!correlationPtr || numberPixels == 0) 
																						return;
																						
	savedCorrelationPtr = correlationPtr;
	
			// Compute the correlation matrix.												
			
			// If the number of input and output channels are the same then		
			// all the channels will be used.  Indicate that the channel			
			// list will not need to be used.												
			
	if (numberOutputChannels == numberInputChannels)
		channelListPtr = NULL;
		
			// If a subset of the channels are to be used, then copy that			
			// subset of the sums of squares to the output covariance 				
			// matrix.																				
			
	if (channelListPtr && (numberOutputChannels < numberInputChannels))
		{
		ReduceInputMatrix (numberOutputChannels, 
									correlationPtr,
									numberInputChannels,	
									channelListPtr, 
									sumSquaresPtr,
									squareOutputMatrixFlag);
									
		sumSquaresPtr = correlationPtr;
									
		}	// end "if (channelListPtr && ..." 
		
	squareInputMatrixFlag = FALSE;
	if ((sumSquaresPtr == correlationPtr) && squareOutputMatrixFlag)
		squareInputMatrixFlag = TRUE;
			
			// Initialize local variables.
		
	lowerLeftIndexSkip = numberOutputChannels - 1;
	
	for (channel=0; channel<numberOutputChannels; channel++)
		{
		for (covChan=0; covChan<=channel; covChan++)
			{	
			*correlationPtr = *sumSquaresPtr / numberPixels;
		 								
		 	sumSquaresPtr++;
		 	correlationPtr++;
		
			}	// end "for (covChan=0; covChan<=..." 
			
		if (squareOutputMatrixFlag)
			{
			correlationPtr += lowerLeftIndexSkip;
				
			if (squareInputMatrixFlag)	
		 		sumSquaresPtr += lowerLeftIndexSkip;
			 		
			lowerLeftIndexSkip--; 
			
			}	// end "if (squareOutputMatrixFlag)" 
			
		}	// end "for (channel=0; channel<numberOutputChannels; ..." 
	
			// Now copy the lower left part of the matrix to the upper right		
			// part if the output is to be a square matrix.								
				
	if (squareOutputMatrixFlag)
		{
		correlationPtr = savedCorrelationPtr;	
		CopyLowerToUpperSquareMatrix (numberOutputChannels, correlationPtr);
		
		}	// end "if (squareOutputMatrixFlag)" 
					
}	// end "ComputeCorrelationMatrix" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ComputeCorrelationCoefficientMatrix
//
//	Software purpose:	The purpose of this routine is to compute a
//							square correlation matrix from the input sums and
//							sums of squares.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			GetEigenvectorClusterCenters in SClusterIsodata.cpp
//							PrincipalComponentAnalysis in SPrincipalComponents.cpp
//							ListStatistics in SProjectListStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/01/1989
//	Revised By:			Larry L. Biehl			Date: 08/20/2010	

void ComputeCorrelationCoefficientMatrix (
				UInt16								numberOutputChannels, 
				HCovarianceStatisticsPtr		correlationPtr, 
				UInt16								numberInputChannels, 
				UInt16*								channelListPtr, 
				HChannelStatisticsPtr			channelStatsPtr, 
				HSumSquaresStatisticsPtr		sumSquaresPtr,
				SInt64								numberPixels, 
				Boolean								squareOutputMatrixFlag)

{	
	double								channelMean,
											channelStdDev1,
											channelStdDev2;

	SInt64								numberPixelsLessOne;
											
	HChannelStatisticsPtr			lChannelStatsPtr1,
											lChannelStatsPtr2;
											
	HCovarianceStatisticsPtr		savedCorrelationPtr;
	
	UInt32								channel,
											covChan,
											lowerLeftIndexSkip;

	Boolean								squareInputMatrixFlag;
	
	
			// Check input values.  Continue only if input parameters are within	
			// proper rangers																		
			
	if (numberOutputChannels == 0 || numberInputChannels == 0 || 
							!channelStatsPtr || !correlationPtr || numberPixels == 0) 
																						return;
																						
	savedCorrelationPtr = correlationPtr;
																						
			// Compute the mean vector if needed.											
	
	if (channelStatsPtr[0].standardDev < 0)
		ComputeMeanStdDevVector (channelStatsPtr, 
											sumSquaresPtr, 
											numberInputChannels, 
											numberPixels, 
											2,
											kTriangleInputMatrix);
	
			// Compute the covariance matrix.												
			
			// If the number of input and output channels are the same then		
			// all the channels will be used.  Indicate that the channel			
			// list will not need to be used.												
			
	if (numberOutputChannels == numberInputChannels)
		channelListPtr = NULL;
		
			// If a subset of the channels are to be used, then copy that			
			// subset of the sums of squares to the output covariance 				
			// matrix.																				
			
	if (channelListPtr && (numberOutputChannels < numberInputChannels))
		{
		ReduceInputMatrix (numberOutputChannels, 
									correlationPtr,
									numberInputChannels,	
									channelListPtr, 
									sumSquaresPtr,
									squareOutputMatrixFlag);
									
		sumSquaresPtr = correlationPtr;
									
		}	// end "if (channelListPtr && ..." 
		
	squareInputMatrixFlag = FALSE;
	if ((sumSquaresPtr == correlationPtr) && squareOutputMatrixFlag)
		squareInputMatrixFlag = TRUE;
			
			// Initialize local variables.													
	
	numberPixelsLessOne = numberPixels;	
	if (numberPixels > 1)
		numberPixelsLessOne--;
		
	lChannelStatsPtr1 = channelStatsPtr;
	lowerLeftIndexSkip = numberOutputChannels - 1;
	
	for (channel=0; channel<numberOutputChannels; channel++)
		{
		if (channelListPtr)
			lChannelStatsPtr1 = &channelStatsPtr[channelListPtr[channel]];
			
		channelMean = lChannelStatsPtr1->mean;
		channelStdDev1 = lChannelStatsPtr1->standardDev;
		
		if (channelStdDev1 > 0)
			{
			lChannelStatsPtr2 = channelStatsPtr;
			
			for (covChan=0; covChan<=channel; covChan++)
				{	
				channelStdDev2 = lChannelStatsPtr2->standardDev;
				if (channelStdDev2 > 0)
					{
					if (channelListPtr)
						lChannelStatsPtr2 = &channelStatsPtr[channelListPtr[covChan]];
					
					*correlationPtr = 
						(*sumSquaresPtr - channelMean * lChannelStatsPtr2->sum)/
																			numberPixelsLessOne;
																			
					*correlationPtr /= (channelStdDev1 * channelStdDev2);
		  						
		  			}	// end "if (channelStdDev2 > 0)" 
		 								
			 	else	// channelStdDev2 == 0 
					*correlationPtr = 0;
			 								
			 	sumSquaresPtr++;
			 	correlationPtr++;
			 	
			 	lChannelStatsPtr2++;
			
				}	// end "for (covChan=0; covChan<=..." 
				
			}	// end "if (channelStdDev1 > 0)" 
		
		else	// channelStdDev1 == 0 
			{
			for (covChan=0; covChan<=channel; covChan++)
				{
				*correlationPtr = 0;
			 	sumSquaresPtr++;
			 	correlationPtr++;
				
				}	// end "for (covChan=channelIndex; ..." 
			
			}	// end "else channelStdDev1 == 0" 
			
		if (squareOutputMatrixFlag)
			{
			correlationPtr += lowerLeftIndexSkip;
				
			if (squareInputMatrixFlag)	
		 		sumSquaresPtr += lowerLeftIndexSkip;
			 		
			lowerLeftIndexSkip--; 
			
			}	// end "if (squareOutputMatrixFlag)" 
		
		lChannelStatsPtr1++;
			
		}	// end "for (channel=0; channel<numberOutputChannels; ..." 
	
			// Now copy the lower left part of the matrix to the upper right		
			// part if the output is to be a square matrix.								
				
	if (squareOutputMatrixFlag)
		{
		correlationPtr = savedCorrelationPtr;	
		CopyLowerToUpperSquareMatrix (numberOutputChannels, correlationPtr);
		
		}	// end "if (squareOutputMatrixFlag)" 
					
}	// end "ComputeCorrelationCoefficientMatrix" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ComputeCovarianceMatrix
//
//	Software purpose:	The purpose of this routine is to compute the
//							covariance matrix from the input sums and
//							sums of squares.  If the statistics code is 1, then
//							only the variance vector is computed.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/01/1989
//	Revised By:			Larry L. Biehl			Date: 08/20/2010	

void ComputeCovarianceMatrix (
				UInt16								numberOutputChannels, 
				HCovarianceStatisticsPtr		covariancePtr,
				UInt16								numberInputChannels, 
				UInt16*								channelListPtr, 
				HChannelStatisticsPtr			channelStatsPtr, 
				HSumSquaresStatisticsPtr		sumSquaresPtr, 
				SInt64 								numberPixels, 
				Boolean								squareOutputMatrixFlag)

{
	double								channelMean;

	SInt64								numberPixelsLessOne;
	
	HChannelStatisticsPtr			lChannelStatsPtr1,
											lChannelStatsPtr2;
											
	HCovarianceStatisticsPtr		savedCovariancePtr;
	
	UInt32								channel,
											covChan,
											lowerLeftIndexSkip;
	
	Boolean								squareInputMatrixFlag;
													
	
			// Check input values.  Continue only if input parameters are within	
			// proper rangers																		
			
	if (numberOutputChannels > 0 && channelStatsPtr && 
					numberOutputChannels > 0 && covariancePtr && numberPixels > 0) 
		{
		savedCovariancePtr = covariancePtr; 
		
		squareInputMatrixFlag = FALSE;
		if ((sumSquaresPtr == covariancePtr) && squareOutputMatrixFlag)
			squareInputMatrixFlag = TRUE;
		
				// Compute the mean vector if needed.										
		
		if (channelStatsPtr[0].standardDev < 0)
			ComputeMeanStdDevVector (channelStatsPtr, 
												sumSquaresPtr, 
												numberInputChannels, 
												numberPixels, 
												2,
												squareInputMatrixFlag);
		
				// Compute the covariance matrix.											
				
				// If the number of input and output channels are the same			
				// then all the channels will be used.  Indicate that the 			
				// channel list will not need to be used.									
				
		if (numberOutputChannels == numberInputChannels)
			channelListPtr = NULL;
		
				// If a subset of the channels are to be used, then copy that		
				// subset of the sums of squares to the output covariance 			
				// matrix.																			
				
		if (channelListPtr && (numberOutputChannels < numberInputChannels))
			{
			ReduceInputMatrix (numberOutputChannels, 
										covariancePtr,
										numberInputChannels,	
										channelListPtr, 
										sumSquaresPtr,
										squareOutputMatrixFlag);
										
			sumSquaresPtr = covariancePtr;
										
			}	// end "if (channelListPtr && ..."
		
		squareInputMatrixFlag = FALSE;
		if ((sumSquaresPtr == covariancePtr) && squareOutputMatrixFlag)
			squareInputMatrixFlag = TRUE;
		
				// Initialize local variables.												
		
		numberPixelsLessOne = numberPixels;
		if (numberPixels > 1)
			numberPixelsLessOne--;
			
		lChannelStatsPtr1 = channelStatsPtr;
		lowerLeftIndexSkip = numberOutputChannels - 1;
		
		for (channel=0; channel<numberOutputChannels; channel++)
			{
			if (channelListPtr)
				channelMean = channelStatsPtr[channelListPtr[channel]].mean;
				
			else	// channelListPtr == NULL 
				channelMean = lChannelStatsPtr1->mean;
			
			lChannelStatsPtr2 = channelStatsPtr;
			for (covChan=0; covChan<=channel; covChan++)
				{	
				if (channelListPtr)
					lChannelStatsPtr2 = &channelStatsPtr[channelListPtr[covChan]];
				
				*covariancePtr = 
					(*sumSquaresPtr - (channelMean * lChannelStatsPtr2->sum))/
																			numberPixelsLessOne;
				/*
						Testing for numerical problems with large UInt32 values.
				dValue = channelMean * lChannelStatsPtr2->sum;
				*covariancePtr = (*sumSquaresPtr - dValue)/numberPixelsLessOne;
				// Test
				if (gClassifySpecsPtr != NULL && gClassifySpecsPtr->mode == 5)
					{
					if (channel != covChan)
						*covariancePtr = 0;

					}	// end "if (gClassifySpecsPtr != NULL && ..."
				// end Test
				*/
			 	sumSquaresPtr++;
			 	covariancePtr++;
			 	
			 	lChannelStatsPtr2++;
			
				}	// end "for (covChan=0; covChan<=..." 
			
			if (squareOutputMatrixFlag)
				{
				covariancePtr += lowerLeftIndexSkip;
				
				if (squareInputMatrixFlag)	
			 		sumSquaresPtr += lowerLeftIndexSkip;
				
				lowerLeftIndexSkip--;
				
				}	// end "if (squareOutputMatrixFlag)" 
			
			lChannelStatsPtr1++;
				
			}	// end "for (channel=0; channel<numberOutputChannels; ..."
	
				// Now copy the lower left part of the matrix to the upper right	
				// part if the output is to be a square matrix.							
		
		if (squareOutputMatrixFlag)
			{
			covariancePtr = savedCovariancePtr;	
			CopyLowerToUpperSquareMatrix (numberOutputChannels, covariancePtr);
			
			}	// end "if (squareOutputMatrixFlag)" 
			
		}	// end "if (numberOutputChannels > 0 ..." 
					
}	// end "ComputeCovarianceMatrix" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ComputeEigenectors
//
//	Software purpose:	The purpose of this routine is to compute the
//							eigenvalues and eigenvectors for the input real
//							symmetric matrix.
//		
//	Parameters in:		matrixPtr points to the input real symmetric matrix.
//							x is a work vector of doubles
//							ih is	a work vector of SInt16's
//							matrixSize is the order of the real symmetric matrix
//							requestCode is 0 when only eigenvalues are to be found.
//										   is 1 when eigenvalues & eigenvectors are
//											to be found.
//
//	Parameters out:	x[0] contains the number of iterations completed.
//							x[1] contains the largest offdiagonal value that
//									existed when iteration was stopped either
//									due to iteration count or the threshold, 'epsi'
//									being reached.	
//							eigenvectorPtr points to the eigenvector matrix.
//							matrixPtr points to the eigenvalues.  They are on
//							the diagonal.
//
// Value Returned: 
//
// Called By:			GetEigenvectorClusterCenters in SClusterIsodata.cpp
//							FS_eigen_inverse_SwSb_gen in SFeatureExtractionMath.cpp
//							FS_decision_boundary in SFeatureExtractionMath.cpp
//							FS_optimize_2_class in SFeatureExtractionMath.cpp
//							PrincipalComponentAnalysis in SPrincipalComponents.cpp
//
//	Coded By:			?							Date: ?
//	Revised By:			C.H. LEE					Date: 11/03/1988
//	Revised By:			Larry L. Biehl			Date: 06/19/2006

Boolean ComputeEigenvectors (
				HDoublePtr							matrixPtr,			// Input: Input matrix;  
				UInt16								covarianceSize, 	// Size of square matrix 
				HDoublePtr							eigenvectorPtr, 	// Eigenvector storage; at
																				// least same size as 
																				// 'matrix'.							
				SInt16*								ih, 					// Work vector for storage
																				// of maximum element 
																				// locations.  At least			
																				// 'covarianceSize' in 
																				// length.			
				HDoublePtr							x, 					// Work vector for storage
																				// of maximums. At least 
																				// 'covarianceSize' in 
																				// length; the minimum 
																				// length is 2.				
				SInt16								requestCode)		// Bit 0:										
																// 	0 eigenvalues alone to be found 	
																// 	1 eigenvalues and eigenvectors	
																// 			are to be found.				
																// Bit 1:										
																//		0 leave eigenvalues in matrix		
																//				form along diagonal.			
																//		1 pack eigenvalues in vector		
																// 			form.								


{
	double 								cosn,
			 								epsi,
			 								qii,
											sine,
			 								t,
			 								tang,
				 							temp,
			 								tempipip,
											tempipjp,
											tempjpjp,
			 								xmax,
			 								y;
	
	HDoublePtr							tempMatrixPtr1,
											tempMatrixPtr2,
											tempxPtr;
									
	UInt16*								tempihPtr;
	
	UInt32	 							i,
											ip,
											iterationCount,
				 							j,
											jp,
											k,
											matrixSize,
				 							mi,
				 							mj,
											maximumIterations;

	Boolean								stopFlag;
	
	
			// Initialize local variables.
			// I tried different settings for epsi to 10-19 for 68881 and 10-15 for
			// others. It did not make any difference in the precision for the
			// eigenvectors but did make a significant difference in the time to compute.
			// For 220 channels, PPC 8100/80 and computing SVD for matrix inversion. The
			// average error represent the covariance time inverse as different from
			// identity matrix. The 'standard' matrix inversion always had an ave error
			// of an order of magnitude better.
			// 	10e-16 = 97 seconds, ave error = 4.41252e-13
			//		10e-15 = 78 seconds, ave error = 4.41771e-13
			//		10e-10 = 66 seconds, ave error = 5.86121e-13
		
	#ifdef _80BitDoubles_
		#ifdef _MC68881_
			epsi = 1e-10;
		#endif
		#ifndef _MC68881_
					// This was changed from 1e-19 to 1e-15 on 3/3/1997 so that
					// the program would not go into an infinite loop when
					// running under MAE.
			epsi = 1e-10;
		#endif
	#else
		epsi = 1e-10;
	#endif												
			
	matrixSize = covarianceSize;
	maximumIterations = 30000000;
	gTextString[0] = 8;

			// Next statements for setting initial values of eigenvector matrix.	

	if (requestCode & 0x0001)
		{
		tempMatrixPtr1 = eigenvectorPtr;
		for (i=0; i<matrixSize; i++)
			for (j=0; j<matrixSize; j++)
				{
	   		if (i==j)
	    			*tempMatrixPtr1 = 1.0;
	   		else
	    			*tempMatrixPtr1 = 0.;
	    			
	    		tempMatrixPtr1++;
	    		
	    		}	// end "for (j=0; j<matrixSize; j++)" 
	    		
		}	// end "if (requestCode & 0x0001)"
  
  			// Intialize iteration count.														
  			
	iterationCount = 0;		// line 15 

			// Next statements scan for largest off diag. elem. in each row 		
			// x (i) contains largest element in ith row ih (i) holds second
			// subscript defining position of element n 									

	mi = matrixSize - 1;
	tempMatrixPtr1 = matrixPtr;
	tempxPtr = x;
	for (i=0; i<mi; i++)
		{
	 	*tempxPtr = 0.;
	 	mj = i+1;
	 	tempMatrixPtr1 += mj;
	 	for (j=mj; j<matrixSize; j++)
	 		{
	  		y = *tempMatrixPtr1;
	  		if (*tempxPtr <= fabs (y)) 
	  			{
	    		*tempxPtr = fabs (y);
	    		ih[i] = (SInt16)j;
	    		
	    		}	// end "if (*tempxPtr <= fabs (y)) " 
	    		
	    	tempMatrixPtr1++;
	    		
	    	}	// end "for (j=mj; j<matrixSize; j++)" 
	    	
	    tempxPtr++;
	    	
	    }	// end "for (i=0; i<mi; i++))" 
	
			// Find largest covariance value.  This will be used to determine
			// the value to use for epsi.  It will be set to 12 orders (base 10)
			// or 27 orders (base e) smaller than the largest value.
	
	xmax = -DBL_MAX;		
	tempMatrixPtr1 = matrixPtr;
	mi = matrixSize + 1;
	for (i=0; i<matrixSize; i++)
		{
		xmax = MAX (xmax, fabs (*tempMatrixPtr1));		// This is the largest diagonal
		tempMatrixPtr1 += mi;
		
		}	// end "for (i=0; i<matrixSize; i++)"
		
	if (xmax > 0)
		{
		temp = log (xmax) - 27;
		temp = exp (temp);
		if (temp < epsi)
			epsi = temp;
		
		}	// end "if (xmax > 0)"
					
			// Intialize the nextTime variable to indicate when the next check	
			// should occur for a command-.													
			
	gNextTime = TickCount ();	
	    
			// Next statements find for maximum of x9i0s for pivot element.  		
			
	mi = matrixSize - 1;
	for (; ;)	
		{
				// Next statements find maximum off diagonal element to				
				// determine pivot element.  													
				
	   xmax = x[0];
	   ip = 0;
	   jp = ih[0]; 	
		tempxPtr = &x[1];
		for (i=1; i<mi; i++)
			{
	  		if (xmax < *tempxPtr) 
	  			{	 
			   xmax = *tempxPtr;
			   ip = i;
			   jp = ih[i]; 	
	   		
	   		}	// end "if (xmax < x[i])" 
	   		
	   	tempxPtr++;
	   		
	   	}	// end "for (i=1; i<mi; i++)" 
	   	
		if (TickCount () >= gNextTime)
			{
	   			// This code checks to see if the user has requested 				
	   			// computation to stop; i.e. the user has entered command-.		
	   			// on the keyboard.	
   			
			stopFlag = 
				!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask);
				
			if (gStatusDialogPtr != NULL)
				{
						// This code will give the user an indication where the routine
						// is in computing the eigenvectors. The value xmax continually 
						// gets smaller.
						
				sprintf ((char*)&gTextString[1], "%8.1e", xmax); 
				//LoadDItemString (gStatusDialogPtr,
				//							IDC_Status14,
				//							gTextString);
				
				}	// end "if (gStatusDialogPtr != NULL)" 
				
			}	// end "if (TickCount () >= nextTime)"

				// Next statements test for xmax. if less than epsi return; 		
				// or exit routine if beyond iteration limit.							
		
		if (xmax < epsi || (iterationCount >= maximumIterations) || stopFlag)
			{
		 	stopFlag = stopFlag || (iterationCount >= maximumIterations);
		
					// Order the eigenvectors and eigenvalues. Use the 'x' vector
					// for temporary holding.

			if (!stopFlag)
				OrderEigenvaluesAndEigenvectors (matrixPtr, 
															eigenvectorPtr,
															x,	// tempVector
															covarianceSize,
															requestCode & 0x0001);
		
					// Pack the eigenvalues in vector form if requested.				
					
			if (!stopFlag && (requestCode & 0x0002))
				{
				tempxPtr = matrixPtr;
				tempMatrixPtr2 = matrixPtr;
				jp = matrixSize + 1;
				
				for (i=1; i<matrixSize; i++)
					{
					tempxPtr++;
					tempMatrixPtr2 += jp;
					
					*tempxPtr = *tempMatrixPtr2;
						
					}	// end "for (i=1; i<=..." 
					
				}	// end "if (!stopFlag && (requestCode & 0x0002))" 
														
		 	x[0] = iterationCount;
		 	x[1] = xmax;
																					return (!stopFlag);
		 	
		 	}	// end "if (xmax<epsi || ...)" 
	
		iterationCount++;	// line number 148 

				// Next 11 statements for computing tang, sin and cos				
		
		tempipip = matrixPtr[matrixSize*ip+ip];
		tempjpjp = matrixPtr[matrixSize*jp+jp];
		tempipjp = matrixPtr[matrixSize*ip+jp];
		
		if (tempipip < tempjpjp) 
	 		tang = -2.* tempipjp;
		else 
	 		tang = 2.* tempipjp;

		t = temp = tempipip - tempjpjp;
		t *= t;
		t += 4. * tempipjp * tempipjp;
		t = sqrt (t);
		t += fabs (temp);

		tang /= t; 

		cosn = 1./sqrt (1.+tang*tang);
		sine = tang*cosn;
		qii = tempipip;

		tempipip = cosn * cosn;
		tempipip *= qii + tang * (2.*tempipjp + tang * tempjpjp);

		t = tempjpjp;
		tempjpjp  = cosn * cosn;
		tempjpjp *= t - tang * (2.*tempipjp - tang*qii);

		tempipjp = 0;

				// Next 4 statement for pseudo rank the eignevalues.					

		if (tempipip < tempjpjp)
			{ 						// pair 152 
	 		temp = tempipip;
	 		tempipip = tempjpjp;
	 		tempjpjp = temp;

	 				// Next statement adjust sin cos for computation.					

	 		if (sine<0)
	  			temp = cosn;
	 		else 
	  			temp = -cosn;

	 		cosn = fabs (sine);
	 		sine = temp;
	 		
	 		}	// end "if (tempipip < tempjpjp)" 
	 		
	 			// Update the matrix.															
	 			
		matrixPtr[matrixSize*ip+ip] = tempipip;
		matrixPtr[matrixSize*jp+jp] = tempjpjp;
		matrixPtr[matrixSize*ip+jp] = tempipjp;

	 				// Next 10 statements.														
	 				
		tempxPtr = x;
		tempihPtr = (UInt16*)ih;
 		for (i=0; i<mi; i++)		// line number 153 
 			{
  			if (i<ip || (i>ip && i!=jp))
  				{
   			if (*tempihPtr==ip || *tempihPtr==jp)
   				{
   				k = *tempihPtr;
   				tempMatrixPtr1 = &matrixPtr[matrixSize*i+k];
    				temp = *tempMatrixPtr1;
    				*tempMatrixPtr1 = 0;
    				mj = i + 1;
    				*tempxPtr = 0;
    				tempMatrixPtr2 = &matrixPtr[matrixSize*i+mj];
    				for (j=mj; j<matrixSize; j++)
    					{
    					t = fabs (*tempMatrixPtr2);
     					if (*tempxPtr <= t) 
     						{
      					*tempxPtr = t;
      					*tempihPtr = (SInt16)j;	 	 	  
      					
      					}	// end "if (*tempxPtr <= t) " 
      					
      				tempMatrixPtr2++;
      					
      				}	// end "for (j=mj; j<matrixSize; j++)" 

    				*tempMatrixPtr1 = temp;
    					
    				}	// end "if (*tempihPtr==ip || *tempihPtr==jp)" 
    				
    			}	// end "if (i<ip || (i>ip && i!=jp))" 
    			
    			tempxPtr++;
    			tempihPtr++;
    			
    		}	// end "for (i=0; i<mi; i++)" 

		x[ip] = x[jp] = 0;

					// Next 30 statements.														

		tempxPtr = x;
		tempihPtr = (UInt16*)ih;
		for (i=0; i<matrixSize; i++)	// start 530 
			{
 			if (i<ip)	
 				{
   			tempMatrixPtr1 = &matrixPtr[matrixSize*i+ip];
   			tempMatrixPtr2 = &matrixPtr[matrixSize*i+jp];
 				temp = *tempMatrixPtr1;
 				
  				*tempMatrixPtr1 = cosn*temp + sine* *tempMatrixPtr2;

				t = fabs (*tempMatrixPtr1);
  				if (*tempxPtr < t) 
  					{
   				*tempxPtr = t;
   				*tempihPtr = (SInt16)ip;
   						
   				}	// end "if (x[i] < t)" 

 				*tempMatrixPtr2 = -sine*temp + cosn* *tempMatrixPtr2;

				t = fabs (*tempMatrixPtr2);
  				if (*tempxPtr < t)
  					{
   				*tempxPtr = t;
   				*tempihPtr = (SInt16)jp;
   					
   				}	// end "if (x[i] < t)" 
   					
   			}	// end "if i<ip" 

 			else if (i>ip && i<jp)
 				{
   			tempMatrixPtr1 = &matrixPtr[matrixSize*ip+i];
   			tempMatrixPtr2 = &matrixPtr[matrixSize*i+jp];
   			temp = *tempMatrixPtr1;
   			
   			*tempMatrixPtr1 = cosn*temp + sine* *tempMatrixPtr2;
   			
   			t = fabs (*tempMatrixPtr1);
   			if (x[ip] < t)
   				{
    				x[ip] = t;
    				ih[ip] = (SInt16)i;
    						
    				}	// end "if (x[ip] < t)" 

  				*tempMatrixPtr2 = -sine*temp + cosn* *tempMatrixPtr2;

   			t = fabs (*tempMatrixPtr2);
  				if (*tempxPtr < t)
  					{
  					*tempxPtr = t;
   				*tempihPtr = (SInt16)jp; 
   						
   				}	// end "if (x[i] < t)" 
   						
   			}	// end "else if (i>ip && i<jp)" 

			else if (i>ip && i>jp) 
				{
   			tempMatrixPtr1 = &matrixPtr[matrixSize*ip+i];
   			tempMatrixPtr2 = &matrixPtr[matrixSize*jp+i];
   			temp = *tempMatrixPtr1;
   			
   			*tempMatrixPtr1 = cosn*temp + sine* *tempMatrixPtr2;
   			
   			t = fabs (*tempMatrixPtr1);
   			if (x[ip] < t)
   				{
    				x[ip] = t;
    				ih[ip] = (SInt16)i; 
    						
    				}	// end "if (x[ip] < t)" 

  				*tempMatrixPtr2 = -sine*temp + cosn* *tempMatrixPtr2;
  				
   			t = fabs (*tempMatrixPtr2);
  				if (x[jp] < t) 
  					{
   				x[jp] = t;
   				ih[jp] = (SInt16)i;
   						
   				}	// end "if (x[jp] < t)" 
   						
   			}	// end "else if (i>ip && i>jp)" 
   			
   		tempxPtr++;
   		tempihPtr++;
   			
   		}	// end "for (i=0; i<matrixSize; i++)" 

		if (requestCode & 0x0001)
 			for (i=0; i<matrixSize; i++)
 				{
   			tempMatrixPtr1 = &eigenvectorPtr[ip*matrixSize+i];
   			tempMatrixPtr2 = &eigenvectorPtr[jp*matrixSize+i];
   			
  				temp = *tempMatrixPtr1;
  				*tempMatrixPtr1 = cosn*temp + sine* *tempMatrixPtr2; 
  				*tempMatrixPtr2 = -sine*temp + cosn* *tempMatrixPtr2; 
  							
  				}	// end "for (i=0; i<matrixSize; i++)" 
				
		}	// end "for (; ;)" 
		
	return (FALSE);

}	// end "ComputeEigenvectors" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ComputeMeanStdDevVector
//
//	Software purpose:	The purpose of this routine is to compute the
//							mean and standard deviation vector 
//							for the input intermediate statistics.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			SaveClusterStatistics in SCluster.cpp
//							ComputeCorrelationCoefficientMatrix in SMatrixUtilities.cpp
//							ComputeCovarianceMatrix in SMatrixUtilities.cpp
//							ReadProjectFile in SProject.cpp
//							UpdateFieldStats in SProjectComputeStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/31/1992
//	Revised By:			Larry L. Biehl			Date: 08/20/2010	

void ComputeMeanStdDevVector (
				HChannelStatisticsPtr			channelStatsPtr, 
				HSumSquaresStatisticsPtr		sumSquaresPtr, 
				UInt16								numberChannels, 
				SInt64								numberPixels, 
				SInt16								statCode, 
				Boolean								squareSumSquaresMatrixFlag)

{
			// Check input values.  Continue only if input parameters are within	
			// proper rangers																		
			
	if (numberChannels > 0 && channelStatsPtr && sumSquaresPtr && numberPixels > 0)
		{
		ComputeMeanVector (channelStatsPtr, numberChannels, numberPixels);
		
		ComputeStdDevVector (channelStatsPtr, 
										sumSquaresPtr, 
										numberChannels, 
										numberPixels, 
										statCode, 
										squareSumSquaresMatrixFlag);
			
		}	// end "numberChannels > 0 ... " 
					
}	// end "ComputeMeanStdDevVector" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ComputeMeanVector
//
//	Software purpose:	The purpose of this routine is to compute the
//							mean vector for the input intermediate statistics.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/07/1992
//	Revised By:			Larry L. Biehl			Date: 08/20/2010	

void ComputeMeanVector (
				HChannelStatisticsPtr			channelStatsPtr, 
				UInt16								numberChannels, 
				SInt64								numberPixels)

{
	UInt32								channel;
	
	
			// Check input values.  Continue only if input parameters are within	
			// proper rangers																		
			
	if (numberChannels > 0 && channelStatsPtr && numberPixels > 0)
		{
		for (channel=0; channel<numberChannels; channel++)
			{
			channelStatsPtr->mean = channelStatsPtr->sum/numberPixels;
			channelStatsPtr++;
			
			}	// end "for (channel=0; channel<numberChannels; channel++)" 
			
		}	// end "numberChannels > 0 ..." 
					
}	// end "ComputeMeanVector" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ComputeStdDevVector
//
//	Software purpose:	The purpose of this routine is to compute the
//							standard deviation vector 
//							for the input intermediate statistics.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			ComputeMeanStdDevVector in SMatrixUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/31/1992
//	Revised By:			Larry L. Biehl			Date: 08/20/2010	

void ComputeStdDevVector (
				HChannelStatisticsPtr			channelStatsPtr, 
				HSumSquaresStatisticsPtr		sumSquaresPtr, 
				UInt16								numberChannels, 
				SInt64								numberPixels, 
				SInt16								statCode, 
				Boolean								squareSumSquaresMatrixFlag)

{
	double								variance;

	SInt64								numberPixelsLessOne;
	
	UInt32								channel,
											indexSkip;
	
	
			// Check input values.  Continue only if input parameters are within	
			// proper rangers																		
			
	if (numberChannels > 0 && channelStatsPtr && sumSquaresPtr && numberPixels > 0)
		{
		numberPixelsLessOne = numberPixels - 1;
		
		if (numberPixels > 1)
			{
			indexSkip = 1;
			
			if (statCode == 2 && squareSumSquaresMatrixFlag)
				indexSkip = numberChannels +1;
			
			for (channel=0; channel<numberChannels; channel++)
				{
				variance = 
					(*sumSquaresPtr - channelStatsPtr->mean * channelStatsPtr->sum)/
																					numberPixelsLessOne;
																				
				channelStatsPtr->standardDev = sqrt (fabs (variance));
				
				channelStatsPtr++;
				
				if (statCode == 2 && !squareSumSquaresMatrixFlag)
					indexSkip++;
					
				sumSquaresPtr += indexSkip;
				
				}	// end "for (channel=0; channel<numberChannels; channel++)" 
				
			}	// end "if (numberPixels > 1)" 
			
		else	// numberPixels <= 1 
			{
			for (channel=0; channel<numberChannels; channel++)
				{															
				channelStatsPtr->standardDev = 0;
				channelStatsPtr++;
				
				}	// end "for (channel=0; channel<numberChannels; channel++)" 
			
			}	// end "else numberPixels <= 1 " 
			
		}	// end "numberChannels > 0 ..." 
					
}	// end "ComputeStdDevVector" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double ConvertToScientificFormat
//
//	Software purpose:	The purpose of this routine is to convert the input value into
//							the two scientific format parts.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/08/2006
//	Revised By:			Larry L. Biehl			Date: 02/08/2006	

double ConvertToScientificFormat (
				double								value, 
				SInt32*								base10ExponentPtr)
				
{
	double								exponent,
											logValue,
											mantissa;
											

	logValue = log10 (value);
	mantissa = fabs (logValue);
	exponent = floor (mantissa);
	
	mantissa -= exponent;
	if (value < 1)
		{
		mantissa = 1 - mantissa;
		exponent++;
		
		}	// end "if (value < 1)"
		
	mantissa = pow ((double)10,mantissa);
	
	*base10ExponentPtr = (SInt32)SIGN (exponent,logValue);
	
	return (mantissa);
					
}	// end "ConvertToScientificFormat" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CopyLowerToUpperSquareMatrix
//
//	Software purpose:	The purpose of this routine is to copy the lower
//							left triangular portion of a square matrix to the
//							upper right portion of a square matrix.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/03/1992
//	Revised By:			Larry L. Biehl			Date: 02/09/1992	

void CopyLowerToUpperSquareMatrix (
				UInt16								numberChannels, 
				HCovarianceStatisticsPtr		squareMatrixPtr)

{
	HCovarianceStatisticsPtr		lowerLeftPtr,
											upperRightPtr;
	
	SInt32								lowerLeftIndexSkip,
											upperRightIndexSkip;
											
	UInt32								channel,
											covChan;
	
	
			// Check input values.  Continue only if input parameters are within	
			// proper rangers																		
			
	if (numberChannels == 0 || squareMatrixPtr == NULL)  
																							return;
	
			// Copy the lower left part of the matrix to the upper right			
			// part.																					
				
	lowerLeftIndexSkip = (SInt32)numberChannels * (2 - (SInt32)numberChannels) + 1;
	upperRightIndexSkip = 2;
			
	lowerLeftPtr = squareMatrixPtr + numberChannels;
	upperRightPtr = squareMatrixPtr + 1;
	
	for (channel=1; channel<numberChannels; channel++)
		{
		for (covChan=channel; covChan<numberChannels; covChan++)
			{	
		 	*upperRightPtr = *lowerLeftPtr;
		 					
		 	upperRightPtr++;
		 	lowerLeftPtr += numberChannels;
		
			}	// end "for (covChan=0; covChan<=..." 
					
		lowerLeftPtr += lowerLeftIndexSkip;
		lowerLeftIndexSkip += numberChannels; 
		
		upperRightPtr += upperRightIndexSkip;
		upperRightIndexSkip++;
			
		}	// end "for (channel=1; channel<numberChannels; channel++)" 
					
}	// end "CopyLowerToUpperSquareMatrix"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CopySquareToTriangleMatrix
//
//	Software purpose:	The purpose of this routine is to copy the input
//							square matrix to the output triangle matrix.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 07/03/1996
//	Revised By:			Larry L. Biehl			Date: 07/03/1996	

void CopySquareToTriangleMatrix (
				UInt16								numberChannels,
				HCovarianceStatisticsPtr		inputSquareMatrixPtr, 
				HCovarianceStatisticsPtr		outputTriangleMatrixPtr)

{
	UInt32								channel,
											covChan,
											lowerLeftIndexSkip;
	
	
			// Check input values.  Continue only if input parameters are within	
			// proper rangers																		
			
	if (numberChannels == 0 || 
			inputSquareMatrixPtr == NULL ||
			outputTriangleMatrixPtr == NULL)  
																								return;
	
			// Copy the lower left part of the input square matrix to the 
			// output triangular matrix.																					
				
	lowerLeftIndexSkip = (SInt32)numberChannels - 1;
	
	for (channel=0; channel<numberChannels; channel++)
		{
		for (covChan=0; covChan<=channel; covChan++)
			{	
		 	*outputTriangleMatrixPtr = *inputSquareMatrixPtr;
		 					
		 	inputSquareMatrixPtr++;
		 	outputTriangleMatrixPtr++;
		
			}	// end "for (covChan=0; covChan<=..." 
		
		inputSquareMatrixPtr += lowerLeftIndexSkip;
		lowerLeftIndexSkip--;
			
		}	// end "for (channel=0; channel<numberChannels; channel++)" 
					
}	// end "CopySquareToTriangleMatrix"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CopyTriangleToSquareMatrix
//
//	Software purpose:	The purpose of this routine is to copy the input
//							triangular matrix to the output square matrix.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 06/19/1996
//	Revised By:			Larry L. Biehl			Date: 06/19/1996	

void CopyTriangleToSquareMatrix (
				UInt16								numberChannels, 
				HCovarianceStatisticsPtr		inputTriangleMatrixPtr,
				HCovarianceStatisticsPtr		outputSquareMatrixPtr)

{
	HDoublePtr							outputLowerLeftPtr,
											outputUpperRightPtr;
								
	UInt32								channel,
											covChan,
											lowerLeftIndexSkip;
	
	
			// Check input values.  Continue only if input parameters are within	
			// proper rangers																		
			
	if (numberChannels == 0 || 
			inputTriangleMatrixPtr == NULL ||
			outputSquareMatrixPtr == NULL)  
																								return;
	
	outputLowerLeftPtr = outputSquareMatrixPtr;
				
	lowerLeftIndexSkip = numberChannels;
	
	for (channel=0; channel<numberChannels; channel++)
		{
		outputUpperRightPtr = outputSquareMatrixPtr + channel;
		
		for (covChan=0; covChan<channel; covChan++)
			{	
			*outputLowerLeftPtr = *inputTriangleMatrixPtr;
			*outputUpperRightPtr = *inputTriangleMatrixPtr;
			
			outputLowerLeftPtr++;
			outputUpperRightPtr += numberChannels;
			inputTriangleMatrixPtr++;
		
			}	// end "for (covChan=0; covChan<=..."
			
		*outputLowerLeftPtr = *inputTriangleMatrixPtr; 
		
		inputTriangleMatrixPtr++;	
		outputLowerLeftPtr += lowerLeftIndexSkip;
		
		lowerLeftIndexSkip--;
			
		}	// end "for (channel=0; channel<numberInputChannels; channel++)" 
					
}	// end "CopyTriangleToSquareMatrix" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void EigenvectorInfoExists
//
//	Software purpose:	The purpose of this routine is to determine if
//							a transformation matrix exists for the
//							active image window. 
//
//	Parameters in:		None.
//
//	Parameters out:	Code indicating which procedure created the matrix 
//							Number of eigenvectors in tranformation matrix
//
// Value Returned:  None
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 04/01/2000
//	Revised By:			Larry L. Biehl			Date: 04/01/2000

Boolean EigenvectorInfoExists (
				Boolean								projectFlag)

{
	SInt16								eigenSource;
	
	UInt16								numberEigenvectors;
	
	Boolean								featureTransformationFlag = FALSE;
	 
											
	EigenvectorInfoExists (kProject, &eigenSource, &numberEigenvectors);
	
	if (numberEigenvectors > 0)
		featureTransformationFlag = TRUE;
		
	return (featureTransformationFlag);
					
}	// end "EigenvectorInfoExists" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void EigenvectorInfoExists
//
//	Software purpose:	The purpose of this routine is to determine if
//							a transformation matrix exists for the
//							active image window. 
//
//	Parameters in:		None.
//
//	Parameters out:	Code indicating which procedure created the matrix 
//							Number of eigenvectors in tranformation matrix
//
// Value Returned:  None
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 05/19/1992
//	Revised By:			Larry L. Biehl			Date: 05/02/2000

void EigenvectorInfoExists (
				Boolean								projectFlag, 
				SInt16*								createdByCodePtr, 
				UInt16*								numberEigenvectorsPtr)

{
	UInt16*								eigenFeaturePtr;
	
	UInt32								index;
	
	UInt16								index2;
	
	
	if (createdByCodePtr != NULL)
		*createdByCodePtr =  gTransformationMatrix.createdByCode;
		
	*numberEigenvectorsPtr = gTransformationMatrix.numberFeatures;
	
			// Verify that the number of channels has been set and that the			
			// transformation matrix handles exist.											
	
	if (gTransformationMatrix.numberChannels <= 0 || 
			gTransformationMatrix.numberFeatures <= 0 || 
			!gTransformationMatrix.eigenValueHandle ||
			!gTransformationMatrix.eigenVectorHandle ||
			!gTransformationMatrix.eigenFeatureHandle)
		*numberEigenvectorsPtr = 0;
		
	if (*numberEigenvectorsPtr > 0)
		{
		if (projectFlag)
			{
			if (gProjectInfoPtr != NULL)
				{                            
				eigenFeaturePtr = (UInt16*)GetHandlePointer (
														gTransformationMatrix.eigenFeatureHandle);
				index2 = 0;
						
				if (gTransformationMatrix.createdByCode < 16)
					{
							// The transformation is an eigenvector type of transformation.
							// Verify that the channels in the transformation matrix are	
							// a subset of those available in the project statistics.		
					
					if (gTransformationMatrix.numberChannels <= 
												(UInt16)gProjectInfoPtr->numberStatisticsChannels)
						{ 			
						for (index=0;
								index<(UInt32)gTransformationMatrix.numberChannels;
								index++)
							{
							while ((index2 <
										(UInt16)gProjectInfoPtr->numberStatisticsChannels) &&
											(eigenFeaturePtr[index] != 
															gProjectInfoPtr->channelsPtr[index2]))
								{
								index2++;
								
								}	// end "while (..." 
								
							if (index2 >= (UInt16)gProjectInfoPtr->numberStatisticsChannels)
								*numberEigenvectorsPtr = 0;
							
							}	// end "for (index=0; index..." 
							
						}	// end "if (*numberEigenvectorsPtr <= ..." 
						
					else	// ...numberChannels > ...->numberStatisticsChannels 
						*numberEigenvectorsPtr = 0;
						
					}	// end "if (gTransformationMatrix.createdByCode < 16)"
					
				else	// gTransformationMatrix.createdByCode >= 16
					{
							// The transformation is an offset-gain type of transformation.
							// Verify that the channels in the project statistics are a
							// subset of those available in the transformation matrix.		
					
					if (*numberEigenvectorsPtr >= 
												(UInt16)gProjectInfoPtr->numberStatisticsChannels)
						{  			
						for (index=0; 
								index<(UInt16)gProjectInfoPtr->numberStatisticsChannels; 
								index++)
							{
							while ((index2 < *numberEigenvectorsPtr) &&
										(gProjectInfoPtr->channelsPtr[index] != 
																				eigenFeaturePtr[index2]))
								{
								index2++;
								
								}	// end "while (..." 
								
							if (index2 >= *numberEigenvectorsPtr)
								*numberEigenvectorsPtr = 0;
							
							}	// end "for (index=0; index..." 
							
						}	// end "if (*numberEigenvectorsPtr >= ..." 
						
					else	// *numberEigenvectorsPtr < ...->numberStatisticsChannels 
						*numberEigenvectorsPtr = 0;
					
					}	// end "else gTransformationMatrix.createdByCode >= 16"
				
				}	// end "if (gProjectInfoPtr != NULL)" 
			
			}	// end "if (projectFlag)" 
		
		else	// !projectFlag 
			{
					// Verify that the channels in the transformation matrix				
					// are a subset of the channels that are available in the image	
					// file.																				
			                                                                      
			eigenFeaturePtr = (UInt16*)GetHandlePointer (
													gTransformationMatrix.eigenFeatureHandle);
			if (gImageWindowInfoPtr->totalNumberChannels < 
						(UInt16)eigenFeaturePtr[gTransformationMatrix.numberChannels-1] + 1)
				*numberEigenvectorsPtr = 0;
				
			}	// end "else !projectFlag" 
			
		}	// end "if (*numberEigenvectorsPtr > 0)" 
	
}	// end "EigenvectorInfoExists"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetAreaStats
//
//	Software purpose:	The purpose of this routine is to get the
//							statistics for the given area.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	O if not okay.
//							1 if okay.
//							-1 if user requested to stop with 'command-.'.
// 
// Called By:			UpdateFieldStats in SProjectComputeStatistics.cpp
//							GetTotalSumSquares in SMatrixUtilities.cpp
//							ShowGraphWindowSelection in SSelectionGraph.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/16/1988
//	Revised By:			Larry L. Biehl			Date: 11/15/2019

SInt16 GetAreaStats (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				HChannelStatisticsPtr			areaChanPtr, 
				HSumSquaresStatisticsPtr		areaSumSquaresPtr, 
				UInt16*								channelsPtr, 
				UInt16								numberChannels, 
				Boolean								checkForThresholdDataFlag, 
				SInt16								statCode,
				HDoublePtr							minThresholdValuesPtr,
				HDoublePtr							maxThresholdValuesPtr)

{
	knnType 								knnSamp;
	
	double								dValue,
											maxDataValue,
											minDataValue,
											noDataValue;
	
	SInt64								numberPixels;
	
	double*								knnDataValuesPtr;
													
	HChannelStatisticsPtr			lAreaChanPtr;
	
	HDoublePtr							bufferPtr,
											bufferPtr2,
											tOutputBufferPtr;
											
	HSumSquaresStatisticsPtr		lAreaSumSquaresPtr;
	
	HUCharPtr							inputBufferPtr,
											outputBufferPtr;
	
	Point									point;
	RgnHandle							rgnHandle;
	
	UInt32								channel,
											columnEnd,
											columnInterval,
											columnPtr,
											columnStart,
											covChan,
											line,
											lineCount,
											lineEnd,
											lineInterval,
											lineStart,
											numberSamples;
	
	SInt16								classNumber,
											errCode,
											pointType,
											returnCode;
	
	UInt16								eventCode;

	Boolean								checkForBadDataFlag = FALSE,
											checkForNoDataFlag = FALSE,
											dataOkayFlag,
											polygonFieldFlag;
	
	
			// Make certain that input values make sense									
			
	if (fileIOInstructionsPtr->fileInfoPtr == NULL)													
																							return (0);
	
			// Initialize local variables.													
			
	inputBufferPtr = gInputBufferPtr;
	outputBufferPtr = gOutputBufferPtr;
	returnCode = 1;
	
	classNumber = gAreaDescription.classNumber;
	
	polygonFieldFlag = gAreaDescription.polygonFieldFlag;
	rgnHandle = gAreaDescription.rgnHandle;
	
	lineStart = gAreaDescription.lineStart;
	lineEnd = gAreaDescription.lineEnd;
	lineInterval = gAreaDescription.lineInterval;
	
	columnStart = gAreaDescription.columnStart;
	columnEnd = gAreaDescription.columnEnd;
	columnInterval = gAreaDescription.columnInterval;
	
	numberSamples = (columnEnd - columnStart + columnInterval)/columnInterval;
	
	numberPixels = 0;		
	
			// Get the point type.						
	
	pointType = gAreaDescription.pointType;
	
			// Initialize the event code for checking some events
	
	eventCode = osMask+keyDownMask+updateMask+mDownMask+mUpMask;
	
			// Check for case when this is a polygon point type but there are
			// no pixels within the polygon because of shape and size of the 
			// polygon. Do not give an error return. Just return.
			
	if (polygonFieldFlag && lineStart == 0 && lineEnd == 0 &&
										columnStart == 0 && columnEnd == 0) 
		{
		gAreaDescription.numSamplesPerChan = numberPixels;			
																				return (returnCode);
																				
		}	// end "if (polygonFieldFlag && ..."

	if (polygonFieldFlag && rgnHandle == NULL)			
																						return (0);
	
			// Load the total number of lines into the status dialog. It may
			// be possible for this routine to be called	while loading in
			// a project for a graph window to be updated which causes this
			// routine to be called when the project status dialog box is showing.
			// (It is not to supposed to but it has happened in the past!!)			

	if (gStatusIDNumber == kUpdateStatsInfoID)		
		LoadDItemValue (gStatusDialogPtr, 
								IDC_Status20, 
								(lineEnd-lineStart+lineInterval)/lineInterval);
	lineCount = 0;
									
			// Determine if data values need to be checked.								
	
	if (gProcessorCode == kClusterProcessor)
		{	
		if (minThresholdValuesPtr == NULL || maxThresholdValuesPtr == NULL)
			checkForThresholdDataFlag = FALSE;
		
		}	// end "if (gProcessorCode != kClusterProcessor)"
	
	//else if (gProcessorCode == kSelectionGraphStatsProcess)
				// Set eventCode for selection graph operations to only allow
				// update events
	//	eventCode = osMask+updateMask;
	
	else	// gProcessorCode != kClusterProcessor)
		{
		checkForBadDataFlag = checkForThresholdDataFlag;
		checkForThresholdDataFlag = FALSE;
		
		if (fileIOInstructionsPtr->fileInfoPtr->noDataValueFlag)
			{
					// Want to ignore these data values.
					
			noDataValue = fileIOInstructionsPtr->fileInfoPtr->noDataValue;
			checkForNoDataFlag = TRUE;
			checkForBadDataFlag = FALSE;
			 
			if (noDataValue >= 0)
				{
				maxDataValue = 1.00000001 * noDataValue;
				minDataValue = 0.99999999 * noDataValue;
				
				}	// end "if (noDataValue) >= 0)"
			 
			else if (noDataValue < 0)
				{
				minDataValue = 1.00000001 * noDataValue;
				maxDataValue = 0.99999999 * noDataValue;
				
				}	// end "if (noDataValue) < 0)"
			
			}	// end "if (fileIOInstructionsPtr->fileInfoPtr->noDataValueFlag)"
			
		else	// !...->noDataValueFlag
			{		
					// Don't really need to check if the number of bits is not less
					// than the number bits possible within the number of bytes.
					
			if (gImageWindowInfoPtr->numberBytes * 8 == gImageWindowInfoPtr->numberBits)
				checkForBadDataFlag = FALSE;
				
			maxDataValue = gImageWindowInfoPtr->maxUsableDataValue;
			minDataValue = gImageWindowInfoPtr->minUsableDataValue;
			
			}	// end "else !...->noDataValueFlag"
		
		}	// end "if (gProcessorCode != kClusterProcessor)"
		
			// For now the min and max vectors are only set up for clustering
			
			// Intialize the nextTime variables to indicate when the next check	
			// should occur for a command-. and status information. This is not needed
			// called for updating train or test area statistics. These have been set in
			// the calling routine.
	
	if (classNumber == 0)
		{			
		gNextTime = TickCount ();
		gNextStatusTime = TickCount ();
		
		}	// end "if (classNumber == 0)"
	
	if (statCode == kPixelValuesOnly)
		{
		//knnDataValuesPtr = gProjectInfoPtr->knnDataValuesPtr.data ();
		knnDataValuesPtr =
			&gProjectInfoPtr->knnDataValuesPtr [
													numberChannels * gProjectInfoPtr->knnCounter];
		
		}	// end "if (statCode == kPixelValuesOnly)"
			
			// Load some of the File IO Instructions structure that pertain
			// to the specific area being used.
			
	errCode = SetUpFileIOInstructions (fileIOInstructionsPtr,
													&gAreaDescription, 
													numberChannels,
													channelsPtr,
													kDetermineSpecialBILFlag);
	
			// Loop through the lines for the field.										
			
	for (line=lineStart; line<=lineEnd; line+=lineInterval)
		{
				// Load the line count into the status dialog.							
		
		lineCount++;
		
		if (TickCount () >= gNextStatusTime)
			{
			LoadDItemValue (gStatusDialogPtr, IDC_Status18, (SInt32)lineCount);
			gNextStatusTime = TickCount () + gNextStatusTimeOffset;
			
			}	// end "if (TickCount () >= gNextStatusTime)" 
		
		point.v = (SInt16)line;
		point.h = (SInt16)columnStart;
			
				// Get all channels for the line of image data.  Return if			
				// there is a file IO error.													
			
		errCode = GetLineOfData (fileIOInstructionsPtr,
											line,
											columnStart,
											columnEnd,
											columnInterval,
											(HUCharPtr)inputBufferPtr,
											(HUCharPtr)outputBufferPtr);
					
		if (errCode < noErr)
			{
			CloseUpFileIOInstructions (fileIOInstructionsPtr, &gAreaDescription);										
																							return (0);
			
			}	// end "if (errCode < noErr)"
				
		if (errCode != kSkipLine)
			{
		   tOutputBufferPtr = (HDoublePtr)outputBufferPtr;
			   
			numberSamples = fileIOInstructionsPtr->numberOutputBufferSamples;
		   
			for (columnPtr=0; columnPtr<numberSamples; columnPtr++)
				{
				if (!polygonFieldFlag || PtInRgn (point, rgnHandle))
					{
			   	dataOkayFlag = TRUE;
			   	
			   			// Check for bad data if requested.										
			   			
			   	if (checkForThresholdDataFlag)
			   		{
								// From Cluster Processor
								
			   		bufferPtr = tOutputBufferPtr;
						for (channel=0; channel<numberChannels; channel++)
							{
			   			if (*bufferPtr < minThresholdValuesPtr[channel] || 
													*bufferPtr > maxThresholdValuesPtr[channel])
			   				{
			   				dataOkayFlag = FALSE;
			   				break;
			   				
			   				}	// end "if (*bufferPtr > maxDataValue)" 
			   				
			   			bufferPtr++;
			   				
			   			}	// end "for (channel=0; channel<numberChannels; ..." 
			   		
			   		}	// end "if (checkForThresholdDataFlag)"									
			   			
			   	else if (checkForBadDataFlag)
			   		{
								// From other Processors
								
			   		bufferPtr = tOutputBufferPtr;
						for (channel=0; channel<numberChannels; channel++)
							{
			   			if (*bufferPtr < minDataValue || *bufferPtr > maxDataValue)
			   				{
			   				dataOkayFlag = FALSE;
			   				break;
			   				
			   				}	// end "if (*bufferPtr > maxDataValue)" 
			   				
			   			bufferPtr++;
			   				
			   			}	// end "for (channel=0; channel<numberChannels; ..." 
			   		
			   		}	// end "else if (checkForBadDataFlag)" 								
			   			
			   	else if (checkForNoDataFlag)
			   		{
								// From other Processors
								
			   		bufferPtr = tOutputBufferPtr;
						for (channel=0; channel<numberChannels; channel++)
							{
			   			if (*bufferPtr > minDataValue && *bufferPtr < maxDataValue)
			   				{
			   				dataOkayFlag = FALSE;
			   				break;
			   				
			   				}	// end "if (*bufferPtr > minDataValue && ..." 
			   				
			   			bufferPtr++;
			   				
			   			}	// end "for (channel=0; channel<numberChannels; ..." 
			   		
			   		}	// end "else if (checkForNoDataFlag)" 
			   	
			   	if (dataOkayFlag)
			   		{
			   		bufferPtr = (HDoublePtr)tOutputBufferPtr;
						
						if (statCode == kPixelValuesOnly)
							{
									// SVM training samples
							
							//sample_type samp;
							//samp.set_size (gClassifySpecsPtr->numberChannels);
							//int sampleCount = 0;
							
									// KNN training samples

							for (channel=0; channel<numberChannels; channel++)
								{
										// Move sample data of each channel to dlib variable
										// for SVM training
								
								//samp (channel) = *bufferPtr;
								
										// KNN sample data in the training
								
								//gProjectInfoPtr->knnDataValuesPtr.push_back (*bufferPtr);
								*knnDataValuesPtr = *bufferPtr;

				      		bufferPtr++;
				      		knnDataValuesPtr++;
								
								}	// end "for (channel=1; channel<numberChannels..."
							
									// KNN labelling in the training phase
							
							knnSamp.distance = 0;
							knnSamp.index = 0;

							gProjectInfoPtr->knnDistancesPtr[gProjectInfoPtr->knnCounter] =
																									knnSamp;
							gProjectInfoPtr->knnLabelsPtr[gProjectInfoPtr->knnCounter] =
																								classNumber;
							gProjectInfoPtr->knnCounter++;
							
							}	// end "if (statCode == kPixelValuesOnly)"
						
						else	// statCode != kPixelValuesOnly
							{
							lAreaChanPtr = areaChanPtr;
							lAreaSumSquaresPtr = areaSumSquaresPtr;

							for (channel=0; channel<numberChannels; channel++)
								{
								dValue = *bufferPtr;
							
										// Get the minimum and maximum value and sum.
								
								lAreaChanPtr->minimum = MIN (lAreaChanPtr->minimum, dValue);
								lAreaChanPtr->maximum = MAX (lAreaChanPtr->maximum, dValue);
								lAreaChanPtr->sum += dValue;
								
								lAreaChanPtr++;
								
								if (statCode == kMeanCovariance)
									{
											// Accumulate the channel covariance statistics
									
									bufferPtr2 = tOutputBufferPtr;
									
									for (covChan=0; covChan<channel; covChan++)
										{
										*lAreaSumSquaresPtr += dValue * *bufferPtr2;
										bufferPtr2++;
										lAreaSumSquaresPtr++;
										
										}	// end "for (covChan=channel+1; ..."
										
									}	// end "if (statCode == kMeanCovariance)"
								
								*lAreaSumSquaresPtr += dValue * dValue;
								lAreaSumSquaresPtr++;

				      		bufferPtr++;
								
								}	// end "for (channel=1; channel<numberChannels..."
							
							}	// end "else statCode != kPixelValuesOnly"
						
						numberPixels++;
						
						}	// end "if (dataOkayFlag)"
					
					}	// end "if (!polygonField || PtInRgn (point, rgnHandle))" 
							
				tOutputBufferPtr += numberChannels;
				
				point.h += (SInt16)columnInterval;
				
						// Exit routine if user has "command period" down.					
						
				if (TickCount () >= gNextTime)
					{
					if (!CheckSomeEvents (eventCode))
						{
						returnCode = -1;
						break;
						
						}	// end "if (!CheckSomeEvents (osMask + ..." 
						
					}	// end "if (TickCount () >= nextTime)" 
						
				} 	// end "for (columnPtr=0; columnPtr<..."
				
			}	// end "if (errCode != kSkipLine)"
			
		if (returnCode < 0)
			break;
			
		if (pointType == kMaskType)
			fileIOInstructionsPtr->maskBufferPtr += 
												fileIOInstructionsPtr->numberMaskColumnsPerLine;
	      
		}	// end "for (line=lineStart; line<=lineEnd; line++)" 
		
	if (returnCode > 0)
		LoadDItemValue (gStatusDialogPtr, IDC_Status18, (SInt32)lineCount);
		
			// Close up any File IO Instructions structure that pertain to the 
			// specific area used for the list data.
			
	CloseUpFileIOInstructions (fileIOInstructionsPtr, &gAreaDescription);
		
	gAreaDescription.numSamplesPerChan = numberPixels;
	
	if (returnCode > 0 && numberPixels > 0 && statCode == kPixelValuesOnly)
		gProjectInfoPtr->pixelDataLoadedFlag = TRUE;
	
			// Indicate that routine completed normally.								
			
	return (returnCode);
		
}	// end "GetAreaStats"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetDiagonalVectorFromMatrix
//
//	Software purpose:	The purpose of this routine is to get the diagonal
//							vector from the input matrix.  The input matrix
//							can be either in lower triangular form or square
//							form.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			CreateStatisticsImages in SStatisticsImage.cpp
//
//	Coded By:			Chulee Lee				Date: 09/03/1992
//	Revised By:			Larry L. Biehl			Date: 09/08/1992	

void GetDiagonalVectorFromMatrix (
				UInt16								matrixSize,
				HDoublePtr							inputMatrixPtr, 
				HDoublePtr							outputVectorPtr, 
				Boolean								squareInputMatrixFlag)

{
	UInt32								i,
											indexSkip;
	
	
			// Check input values.  Continue only if input parameters are within	
			// proper rangers																		
			
	if (matrixSize <= 0 || 
				inputMatrixPtr == NULL ||
							outputVectorPtr == NULL)
																								return;
																							
	indexSkip = 1;
	if (squareInputMatrixFlag)
		indexSkip += matrixSize;
	
	for (i=0; i<matrixSize; i++)
		{
		*outputVectorPtr = *inputMatrixPtr;
		
		outputVectorPtr++;
		
		if (!squareInputMatrixFlag)
			indexSkip++;
			
		inputMatrixPtr += indexSkip;
		
		}	// end "for (i=0; i<matrixSize; i++)" 
					
}	// end "GetDiagonalVectorFromMatrix" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetMemoryForListTransformation
//
//	Software purpose:	The purpose of this routine is to get a buffer of memory to
//							use to list the transformation information. The pointer to
//							the memory is returned in 'gCharBufferPtr1'
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			FeatureExtraction in SFeatureExtraction.cpp
//							EvaluateTransformationControl in SOther.cpp
//							PrincipalComponentControl in SPrincipalComponents.cpp
//
//	Coded By:			Larry L. Biehl			Date: 07/17/1996
//	Revised By:			Larry L. Biehl			Date: 06/17/2006	
//TODO: For Linux (Assigning same memory as windows. 
//                   Couldnt find details for linux)
Boolean GetMemoryForListTransformation (
				UInt16								numberChannels, 
				Boolean	 							listTransformationFlag)
				
{
	UInt32								bytesNeeded;
	Boolean								continueFlag = FALSE;
	
	
	if (gCharBufferPtr1 == NULL)
		{ 				
				// Get pointer to buffer to use to store data values 					
				// to be listed.  Make certain that there is a minimum of 300 		
				// bytes and at least 15 digits per channel.
		
		bytesNeeded = 0;										
		if (listTransformationFlag)
			{
			#if defined multispec_mac                         
				bytesNeeded = 15 * (numberChannels + 1); 
			#endif	// defined multispec_mac 
					
			#if defined multispec_win || defined multispec_wx
						// Allow for E+xxx not E+xx                         
				bytesNeeded = 16 * (numberChannels + 1);   
			#endif	// defined multispec_win || lin
			
			}	// end "if (...->listTransformationFlag)"
			
		bytesNeeded = MAX (bytesNeeded, 300);
		gCharBufferPtr1 = (HPtr)MNewPointer (bytesNeeded);
		
		continueFlag = (gCharBufferPtr1 != NULL);
																	
		}	// end "if (gCharBufferPtr1 == NULL)"
		
	return (continueFlag);
					
}	// end "GetMemoryForListTransformation" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		UInt16* GetStatisticsFeatureVector
//
//	Software purpose:	The purpose of this routine is to get a pointer
//							to the statistics feature vector.  The vector
//							will depend upon whether a transformation matrix
//							is being used.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 04/30/1993
//	Revised By:			Larry L. Biehl			Date: 04/30/1993	

UInt16* GetStatisticsFeatureVector (
				Boolean								featureTransformationFlag, 
				UInt16*								featurePtr)

{
	UInt16* 								returnFeaturePtr;
	
		
	if (featureTransformationFlag)
		returnFeaturePtr = (UInt16*)gTransformationMatrix.eigenFeaturePtr;
		
	else	// !featureTransformationFlag 
		returnFeaturePtr = featurePtr;
																	
	return (returnFeaturePtr);
			
}	// end "GetStatisticsFeatureVector" 


	
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Booelan GetDataThresholdValues
//
//	Software purpose:	This routine gets the minimum and maximum threshold values to
//							use when data are to be screened for fill or no_data values. The
//							test being used may need to be refined. It needs to stay in sync
//							with what is now used in the isodata cluster processor to
//							determine whether data values are to be thresholded.
//
//	Parameters in:		Pointer to the window information structure			
//
//	Parameters out:	Vector with minimum non-saturated data values
//							Vector with maximum non-saturated data values
//
// Value Returned:	Flag indicated whether the vectors were filled.	
//
// Called By:			GetTotalSumSquares in SMatrixUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/29/2012
//	Revised By:			Larry L. Biehl			Date: 12/04/2014

Boolean GetDataThresholdValues (
				Handle								windowInfoHandle, 
				UInt16*								channelsPtr, 
				UInt16								numberChannels,
				double								*minThresholdValuesPtr,
				double								*maxThresholdValuesPtr,
				UInt32								numberClusters)

{
	double								nonSatClusterDistance,
											tailDifference,
											tailDifference2;
											
	Handle								histogramSummaryHandle;
	HistogramSummaryPtr				histogramSummaryPtr;
	
	UInt32								channel,
											channelIndex;
	
	Boolean								returnFlag = FALSE;
	
	
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
													
				nonSatClusterDistance = (histogramSummaryPtr[channelIndex].maxNonSatValue -
							histogramSummaryPtr[channelIndex].minNonSatValue)/numberClusters;
							
				if (nonSatClusterDistance > 0)
					{				
					tailDifference = histogramSummaryPtr[channelIndex].minNonSatValue - 
														histogramSummaryPtr[channelIndex].minValue;
														
					if (tailDifference > 3 * nonSatClusterDistance)
						minThresholdValuesPtr[channel] =
												histogramSummaryPtr[channelIndex].minNonSatValue;
						
					else	// tailDifference <= 3 * nonSatClusterDistance
						minThresholdValuesPtr[channel] =
												histogramSummaryPtr[channelIndex].minValue;

					tailDifference = histogramSummaryPtr[channelIndex].maxValue - 
												histogramSummaryPtr[channelIndex].maxNonSatValue;
														
					if (tailDifference > 3 * nonSatClusterDistance)
						maxThresholdValuesPtr[channel] =
												histogramSummaryPtr[channelIndex].maxNonSatValue;
						
					else	// tailDifference <= 3 * nonSatClusterDistance
						maxThresholdValuesPtr[channel] =
												histogramSummaryPtr[channelIndex].maxValue;
						
					}	// end "if (nonSatClusterDistance > 0)"
					
				else	// noSatClusterDistance == 0
					{
							// Min and max non-saturated values are the same. Will check if
							// distance from min and min non-saturated is very different from
							// min and max non-saturated and vice versa.
							
					tailDifference = histogramSummaryPtr[channelIndex].minNonSatValue - 
												histogramSummaryPtr[channelIndex].minValue;
														
					tailDifference2 = histogramSummaryPtr[channelIndex].maxValue - 
												histogramSummaryPtr[channelIndex].minNonSatValue;
														
					if (tailDifference > 3 * tailDifference2)
						minThresholdValuesPtr[channel] =
												histogramSummaryPtr[channelIndex].minNonSatValue;
						
					else	// tailDifference <= 3 * tailDifference2
						minThresholdValuesPtr[channel] =
												histogramSummaryPtr[channelIndex].minValue;

					tailDifference = histogramSummaryPtr[channelIndex].maxValue - 
												histogramSummaryPtr[channelIndex].maxNonSatValue;
														
					tailDifference2 = histogramSummaryPtr[channelIndex].maxNonSatValue - 
												histogramSummaryPtr[channelIndex].minValue;
														
					if (tailDifference > 3 * tailDifference2)
						maxThresholdValuesPtr[channel] =
												histogramSummaryPtr[channelIndex].maxNonSatValue;
						
					else	// tailDifference <= 3 * tailDifference2
						maxThresholdValuesPtr[channel] =
												histogramSummaryPtr[channelIndex].maxValue;
														
					}	// else noSatClusterDistance == 0
				
				}	// end "for (channel=0; channel<numberChannels; channel++)"
				
			returnFlag = TRUE;
				
			}	// end "if (histogramSummaryPtr != NULL)"
			
		}	// end "if (GetHistogramLoadedFlag (windowInfoHandle))"
		
	return (returnFlag);
							
}	// end "GetDataThresholdValues"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetTotalSumSquares
//
//	Software purpose:	The purpose of this routine is to determine the
//							covariance for the selected channels and selected 
//							area or the selected set of training areas.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			CEMClsfierControl in SClassify.cpp
//							GetEigenvectorClusterCenters in SClusterIsodata.cpp
//							PrincipalComponentAnalysis in SPrincipalComponents.cpp
//							CreateStatisticsImages in SStatisticsImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 08/07/1990
//	Revised By:			Larry L. Biehl			Date: 01/31/2012	

Boolean GetTotalSumSquares (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				Handle*								totalChanStatsHandlePtr, 
				Handle*								totalSumSquaresStatsHandlePtr, 
				UInt16*								channelsPtr,
				UInt16								numberChannels,
				SInt16*								classPtr, 
				UInt32								numberClasses, 
				SInt16								areaOption, 
				SInt16								fieldType,
				Boolean								checkForFillDataFlag)

{
	SInt64								numberCovariancePixels;
	
	HDoublePtr							maxThresholdValuesPtr,
											minThresholdValuesPtr;
	
	HChannelStatisticsPtr			chanStatsPtr,
											totalChanStatsPtr;
	
	HSumSquaresStatisticsPtr		sumSquaresStatsPtr,
											totalSumSquaresStatsPtr;
															
	UInt16*								projectChannelsPtr;
	
	SInt32								lastAddedStorageIndex,
											longBytesNeeded,
											storageIndex;
	
	SInt16								areaNumber,
											classFieldCode,
											fieldNumber,
											lastClassIndex,
											lastFieldIndex,
											returnCode,
											totalNumberAreas;
	
	UInt16								index,
											index2,
											numberProjectChannels;
	
	Boolean								changedFlag,
											checkForBadDataFlag,
											continueFlag,
											statsUpToDate,
											useProjectStatsFlag;
															

			// Initialize local variables.													
			
	checkForBadDataFlag = TRUE;
	projectChannelsPtr = NULL;
	maxThresholdValuesPtr = NULL;
	minThresholdValuesPtr = NULL;
	
			// Check handle for "total" first order statistics.						
				
	longBytesNeeded = (UInt32)numberChannels * sizeof (ChannelStatistics);
	
	totalChanStatsPtr = (HChannelStatisticsPtr)CheckHandleSize (
																			totalChanStatsHandlePtr,
																			&continueFlag, 
																			&changedFlag, 
																			longBytesNeeded);
	
	if (continueFlag)
		{
				// Change size of handle for class covariance statistics if 		
				// needed.																			
				
		longBytesNeeded = ((UInt32)numberChannels *
							(numberChannels + 1)/2) * sizeof (SumSquaresStatistics);
									
		totalSumSquaresStatsPtr = (HSumSquaresStatisticsPtr)CheckHandleSize (
																			totalSumSquaresStatsHandlePtr,
																			&continueFlag, 
																			&changedFlag, 
																			longBytesNeeded);
		
		}	// end "if (continueFlag)" 
		
	if (continueFlag && areaOption == kTrainingType)
		{
				// Get pointer to memory to use for a project channels list if		
				// the area option is for stats to be totaled from those for		
				// selected classes.																
				
		longBytesNeeded = (UInt32)numberChannels * sizeof (SInt16);
		projectChannelsPtr = (UInt16*)MNewPointer (longBytesNeeded);
		continueFlag = (projectChannelsPtr != NULL);
			
		}	// end "if (continueFlag && areaOption == kTrainingType)" 
			
	if (continueFlag)
		{		
				// Initialize the memory for the field statistics.						
		
		ZeroStatisticsMemory (totalChanStatsPtr, 
										totalSumSquaresStatsPtr, 
										numberChannels, 
										2, 
										kTriangleOutputMatrix);
						
		if (areaOption == kTrainingType)
			{
					// Get covariance for the selected training fields/areas.		
					
			lastClassIndex = -1;
			lastFieldIndex = -1;
			lastAddedStorageIndex = -1;
			numberCovariancePixels = 0;
			numberProjectChannels = gProjectInfoPtr->numberStatisticsChannels;
																
					// Determine if the input channel list is a subset of the		
					// channels currently used for the project statistics.			
			
			index = 0;
			for (index2=0; index2<numberProjectChannels; index2++)
				{
				if (channelsPtr[index] == gProjectInfoPtr->channelsPtr[index2])
					{
					projectChannelsPtr[index] = index2;
					index++;
					
					}	// end "if (...[index] == ...[index2])" 
					
				if (index == numberChannels)
					break;
				
				}	// end "for (index2=0; index2<..." 
			
			useProjectStatsFlag = (index == numberChannels);
				
					// Make certain the minimum and maximum values are available.	
					// Statistics generated and saved before 920226 will not 		
					// contain the min/max values.  These are needed for the 		
					// ISODATA cluster processor.												
			
			if (gProcessorCode == kClusterProcessor)
				{     
				if (gProjectInfoPtr->keepClassStatsOnlyFlag)
					{
					if (gProjectInfoPtr->classChanStatsPtr == NULL ||
							(gProjectInfoPtr->classChanStatsPtr)->minimum == DBL_MAX ||
								(gProjectInfoPtr->classChanStatsPtr)->maximum == -DBL_MAX)
						useProjectStatsFlag = FALSE; 
						
					}	// end "if (gProjectInfoPtr->keepClassStatsOnlyFlag)"
					
				else	// !gProjectInfoPtr->keepClassStatsOnlyFlag
					{
					if (gProjectInfoPtr->fieldChanStatsPtr == NULL ||
							(gProjectInfoPtr->fieldChanStatsPtr)->minimum == DBL_MAX ||
								(gProjectInfoPtr->fieldChanStatsPtr)->maximum == -DBL_MAX)
						useProjectStatsFlag = FALSE;
						
					}	// end "else !gProjectInfoPtr->keepClassStatsOnlyFlag" 
					
				}	// end "if (gProcessorCode == kClusterProcessor)" 
				
					// Can not use project statistics if they include only mean		
					// and standard deviation.													
					
			if (gProjectInfoPtr->statisticsCode != kMeanCovariance)
				useProjectStatsFlag = FALSE;	
			
			totalNumberAreas = GetNumberOfAreas (classPtr, 
																numberClasses, 
																kTrainingType,
																useProjectStatsFlag);
																
			classFieldCode = kFieldStatsOnly;
			if (gProjectInfoPtr->keepClassStatsOnlyFlag)						
				classFieldCode = kClassStatsOnly;
			
					// Show status dialog information.										
			
			if (gStatusDialogPtr)
				{
				LoadDItemValue (gStatusDialogPtr, IDC_Status3, (SInt32)1);
				LoadDItemValue (gStatusDialogPtr, IDC_Status5, (SInt32)totalNumberAreas);
				ShowDialogItem (gStatusDialogPtr, IDC_Status3);
				ShowDialogItem (gStatusDialogPtr, IDC_Status5);
				
				}	// end "gStatusDialogPtr" 
			
					// Loop through the training areas.										
			
			areaNumber = 1;
			do
				{
						// Get the next field number.											
						
				fieldNumber = GetNextFieldArea (gProjectInfoPtr,
															classPtr, 
															numberClasses, 
															&lastClassIndex, 
															lastFieldIndex, 
															fieldType,
															useProjectStatsFlag);
				
				if (fieldNumber >= 0)
					{									
					if (gStatusDialogPtr)
						LoadDItemValue (gStatusDialogPtr, IDC_Status3, (SInt32)areaNumber);
													
					lastFieldIndex = fieldNumber;
					
							// Determine if the statistics are already available.  	
							// If so just add the statistics to the total.				
							
					statsUpToDate = FALSE;
					if (useProjectStatsFlag)
						{
						if (gProjectInfoPtr->keepClassStatsOnlyFlag)
							{
							storageIndex = gProjectInfoPtr->storageClass[
																	classPtr[lastClassIndex]-1];
							statsUpToDate = 
								gProjectInfoPtr->classNamesPtr[storageIndex].statsUpToDate;
							
							if (storageIndex == lastAddedStorageIndex)
								storageIndex = -1;
							
							}	// end "if (...->keepClassStatsOnlyFlag)" 
								
						else	// !gProjectInfoPtr->keepClassStatsOnlyFlag 
							{
							statsUpToDate = 
								gProjectInfoPtr->fieldIdentPtr[fieldNumber].statsUpToDate;
							storageIndex = gProjectInfoPtr->fieldIdentPtr[
															fieldNumber].trainingStatsNumber;
							
							}	// end "else !...->keepClassStatsOnlyFlag" 
								
						}	// end "if (useProjectStatsFlag)" 
								
					if (statsUpToDate)
						{
						if (storageIndex >= 0)
							{
									// Get pointers to the memory for the first order  	
									// and second order class/field statistics.				
									
							GetProjectStatisticsPointers (classFieldCode,
																	storageIndex, 
																	&chanStatsPtr, 
																	&sumSquaresStatsPtr,
																	NULL,
																	NULL);
							
							AddToClassStatistics (numberChannels, 
															totalChanStatsPtr,
															totalSumSquaresStatsPtr, 
															numberProjectChannels, 
															projectChannelsPtr, 
															chanStatsPtr, 
															sumSquaresStatsPtr, 
															kTriangleOutputMatrix,
															kMeanCovariance,
															kMeanCovariance);
														
							lastAddedStorageIndex = storageIndex;
													
							}	// end "if (storageIndex >= 0)"
												
						numberCovariancePixels += gProjectInfoPtr->fieldIdentPtr[
																			fieldNumber].numberPixels;
						
						}	// end "if (...[fieldNumber].statsUpToDate)" 
						
					else	// !...[fieldNumber].statsUpToDate 
						{
								// Get the boundary information for the field.	
								// Note that 'gAreaDescription' was set up in the calling
								// routine.
								
						GetFieldBoundary (gProjectInfoPtr, &gAreaDescription, fieldNumber);
						
								// Add covariance for the next area.						
																		
						returnCode = GetAreaStats (fileIOInstructionsPtr, 
															totalChanStatsPtr, 
															totalSumSquaresStatsPtr, 
															channelsPtr, 
															numberChannels, 
															checkForBadDataFlag, 
															kMeanCovariance,
															NULL,
															NULL);
												
						if (returnCode == 0  || ((returnCode != 1) && 
											(gProcessorCode != kPrincipalComponentsProcessor)))
							{
							fieldNumber = -1;
							continueFlag = FALSE;
							break;
																						
							}	// end "if (returnCode == 0  || ..."
							
								// Verify that the number of pixels computed is the same as
								// that given for the field.
									
						if (gProjectInfoPtr->fieldIdentPtr[fieldNumber].numberPixels !=
																	gAreaDescription.numSamplesPerChan)
							{
							continueFlag = FALSE;
							break;
							
							}	// end "if (...->fieldIdentPtr[fieldNumber].numberPixels != ..."
												
						numberCovariancePixels += gAreaDescription.numSamplesPerChan;
						
								// Dispose of the region if it exists.						
		
						CloseUpAreaDescription (&gAreaDescription);
																						
						}	// end "else !...[fieldNumber].statsUpToDate" 
						
					areaNumber++;
					
					if (returnCode == -1)
						fieldNumber = -1;
					
					}	// end "if (fieldNumber >= 0)" 
				
				}		while (fieldNumber >= 0);
		
			projectChannelsPtr = CheckAndDisposePtr (projectChannelsPtr);
			
			}	// end "if (areaOption == kTrainingType)" 
			
		if (areaOption == kAreaType)
			{
					// Set up the area description for  a selected area, i.e. not 	
					// training classes.															
			
			gAreaDescription.pointType = kRectangleType;
			gAreaDescription.polygonFieldFlag = FALSE;
			gAreaDescription.rgnHandle = NULL;
			gAreaDescription.classNumber = 0;
											
					// Get memory for non saturated data values if needed.										
			
			if (gProcessorCode == kClusterProcessor && checkForFillDataFlag)
				{	
				maxThresholdValuesPtr = (HDoublePtr)MNewPointer (
											gClusterSpecsPtr->numberChannels * sizeof (double));
						
				if (maxThresholdValuesPtr != NULL)
					minThresholdValuesPtr = (HDoublePtr)MNewPointer (
											gClusterSpecsPtr->numberChannels * sizeof (double));
	
				if (minThresholdValuesPtr != NULL)								
					checkForFillDataFlag = GetDataThresholdValues (
																	GetActiveImageWindowInfoHandle (),
																	channelsPtr, 
																	gClusterSpecsPtr->numberChannels,
																	minThresholdValuesPtr,
																	maxThresholdValuesPtr,
																	gClusterSpecsPtr->numberClusters);
					
				else	// minNonSaturatedValuesPtr == NULL
					checkForFillDataFlag = FALSE;
																			
				}	// end "if (gProcessorCode == kClusterProcessor && ...)"
			
					// Get covariance for the selected area.								
															
			returnCode = GetAreaStats (fileIOInstructionsPtr, 
												totalChanStatsPtr, 
												totalSumSquaresStatsPtr, 
												channelsPtr, 
												numberChannels, 
												checkForFillDataFlag, 
												kMeanCovariance,
												minThresholdValuesPtr,
												maxThresholdValuesPtr);
												
			CheckAndDisposePtr (minThresholdValuesPtr);
			CheckAndDisposePtr (maxThresholdValuesPtr);
										
			if (returnCode == 0 || (returnCode != 1 && 
								(gProcessorCode != kPrincipalComponentsProcessor)))
																						return (FALSE);
										
			numberCovariancePixels = gAreaDescription.numSamplesPerChan;
			
			}	// end "if (areaOption == kAreaType)" 
		
				// Verify that there are more than 0 pixels.								
		
		if (gProcessorCode != kClassifyProcessor && numberCovariancePixels <= 0)	
			{
			sprintf ((char*)gTextString, 
						"\rNo pixels were included in the covariance matrix.");
						
			continueFlag = ListString ((HPtr)gTextString,
												(UInt32)strlen ((char*)gTextString),
												gOutputTextH);
			continueFlag = FALSE;
			
			}	// end "if (... && numberCovariancePixels <= 0)" 
			
				// Return the number of covariance pixels.								
				
		gAreaDescription.numSamplesPerChan = numberCovariancePixels;
			
		}	// end "if (continueFlag)" 
		
	return (continueFlag);

}	// end "GetTotalSumSquares" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetTransformedCovarianceMatrix
//
//	Software purpose:	The purpose of this routine is to compute a
//							transformed covariance matrix for the input class.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			CreateStatisticsImages in SStatisticsImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/05/1993
//	Revised By:			Larry L. Biehl			Date: 08/23/2010	

void GetTransformedCovarianceMatrix (
				SInt16								numberOutputChannels, 
				HCovarianceStatisticsPtr		covariancePtr,
				SInt16								numberInputChannels, 
				SInt16*								channelListPtr, 
				HChannelStatisticsPtr			channelStatsPtr, 
				HSumSquaresStatisticsPtr		sumSquaresPtr, 
				SInt64								numberPixels, 
				Boolean								squareOutputMatrixFlag,
				HDoublePtr							eigenVectorPtr, 
				HDoublePtr							tempMatrixPtr, 
				SInt16								numberFeatures)


{
	Boolean								lOutputMatrixFlag;
	
	
	lOutputMatrixFlag = squareOutputMatrixFlag;
	if (eigenVectorPtr && tempMatrixPtr)
		lOutputMatrixFlag = kSquareOutputMatrix;
	
			// Get the covariance matrix.													
								
	ComputeCovarianceMatrix (numberOutputChannels,
										covariancePtr,
										numberInputChannels, 
										(UInt16*)channelListPtr, 
										channelStatsPtr, 
										sumSquaresPtr, 
										numberPixels,
										lOutputMatrixFlag);
	
	if (eigenVectorPtr && tempMatrixPtr)
		{
		MatrixMultiply (eigenVectorPtr, 
								covariancePtr, 
								tempMatrixPtr, 
								numberFeatures,
								numberOutputChannels,
								numberOutputChannels,
								kMat1ByMat2);
								
		MatrixMultiply (tempMatrixPtr,
								eigenVectorPtr, 
								covariancePtr, 
								numberFeatures,
								numberOutputChannels,
								numberFeatures,
								kMat1ByTransposeMat2);
			
		if (squareOutputMatrixFlag == kTriangleOutputMatrix)					
			SquareToTriangularMatrix (covariancePtr, covariancePtr, numberFeatures);
		
		}	// end "if (eigenVectorPtr && tempMatrixPtr)" 

}	// end "GetTransformedCovarianceMatrix" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetTransformedMeanVector
//
//	Software purpose:	The purpose of this routine is to reduce the input
//							mean vector to the requested channels and transform
//							the data if requested.  The output will be
//							vector of double elements
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			LoadBiPlotClassStats in SBiPlotData.cpp
//							FeatureExtraction in SFeatureExtraction.cpp
//							LoadSeparabilityStatistics in SFeatureSelection.cpp
//							ListClassStats in SProjectListStatistics.cpp
//							ListFieldStats in SProjectListStatistics.cpp
//							CreateStatisticsImages in SStatisticsImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/30/1993
//	Revised By:			Larry L. Biehl			Date: 10/09/1997	

void GetTransformedMeanVector (
				HChannelStatisticsPtr			inputChannelStatsPtr, 
				HDoublePtr							outputMeansPtr, 
				SInt16								numberFeatureChannels, 
				SInt16*								featureListPtr, 
				HDoublePtr							eigenVectorPtr, 
				HDoublePtr							tempMatrixPtr,
				HDoublePtr							offsetVectorPtr, 
				SInt16								numberFeatures)

{			
	HDoublePtr			lTempVectorPtr;
	
	
	if (outputMeansPtr != NULL)
		{
		lTempVectorPtr = outputMeansPtr;
		if (eigenVectorPtr != NULL && tempMatrixPtr != NULL)
			lTempVectorPtr = tempMatrixPtr;
		
				// Get the class mean vector														
		
		ReduceMeanVector (inputChannelStatsPtr, 
								lTempVectorPtr,
								numberFeatureChannels,
								featureListPtr);
											
		if (eigenVectorPtr != NULL && tempMatrixPtr != NULL)
			{
			MatrixMultiply (eigenVectorPtr, 
									lTempVectorPtr, 
									outputMeansPtr, 
									numberFeatures,
									numberFeatureChannels,
									1,
									kMat1ByMat2);
									
			if (gTransformationMatrix.createdByCode >= 16 && offsetVectorPtr != NULL)
				{
						// This is an offset/gain type of transformation, now apply the
						// offset.
						
				AddVectors (outputMeansPtr, 
								offsetVectorPtr, 
								outputMeansPtr,
								numberFeatureChannels);
				
				}	// end "if (gTransformationMatrix.createdByCode >= 16)"
			
			}	// end "if (eigenVectorPtr != NULL && tempMatrixPtr != NULL)"
									
		}	// end "if (outputMeansPtr != NULL)"

}	// end "GetTransformedMeanVector" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ListEigenTransformationInformation
//
//	Software purpose:	The purpose of this routine is to list the results
//							of the principal component analysis.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			ListTransformationInformation in SMatrixUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/04/1991
//	Revised By:			Larry L. Biehl			Date: 06/19/2006

Boolean ListEigenTransformationInformation (
				HDoublePtr							covariancePtr, 
				HDoublePtr							eigenVectorPtr, 
				SInt32								numberIterations, 
				double								smallestElement, 
				UInt16*								channelsPtr, 
				UInt16								numberChannels,
				UInt16								numberComponents, 
				Boolean								listEigenvalueFlag, 
				Boolean								listEigenvectorFlag)
			
{ 
	double								cumulativeDeterminant,
											cumulativeLogDeterminant,
											cumulativeVariation,
											totalVariation;
	                         
	CMFileStream*						resultsFileStreamPtr;
	HDoublePtr							savedEigenVectorPtr;
	Ptr									stringPtr;
	
	UInt32								component,
											index,
											indexSkip;
											
	SInt16								numberChars;
	
	UInt16								eFormatDecimalDigits,
											fFormatDecimalDigits,
											numberFieldSize,
											numberWholeDigits;
	
	Boolean								continueFlag;
	
	
			// Check global variables that are needed.									
			
	if (gCharBufferPtr1 == NULL)
																				return (FALSE);
	
			// Intialize local variables.														
			
	resultsFileStreamPtr = GetResultsFileStreamPtr (0);
	savedEigenVectorPtr = eigenVectorPtr;
	continueFlag = TRUE;
	
	indexSkip = 1;

			// List the number of iterations and final smallest off diagonal		
			// element.																				
	
	if (numberIterations > 0)
		{	
		sprintf (gCharBufferPtr1, 
					"%s    After %d iterations, the largest off diagonal was %8.1e.%s",
					gEndOfLine,
					(int)numberIterations,
					smallestElement,
					gEndOfLine);
		continueFlag = OutputString (resultsFileStreamPtr, 
												gCharBufferPtr1, 
												0, 
												gOutputCode, 
												TRUE);
		
		}	// end "if (numberIterations > 0)" 
	
	if (continueFlag && listEigenvalueFlag)
		{
		double					value;
		
				// List the table heading.														
				// "\r Component  \tEigenvalue   \tPercent \tCum. Percent "			
				// \t'Cum. Determinant' /tFeature 'Mean'\r"														
						
		continueFlag = ListSpecifiedStringNumber (kUtilitiesStrID,
																IDS_Utility1, 
																(unsigned char*)gCharBufferPtr1, 
																resultsFileStreamPtr, 
																gOutputCode,
																continueFlag);
		
				// Get the total variance.														
				
		index = 0;
		totalVariation = 0;
		for (component=1; component<=numberComponents; component++)
			{
			totalVariation += covariancePtr[index];
			index += indexSkip;
				
			}	// end "for (component=1; component<=..." 
		
				// Adjust total variation to obtain percent values.					
				
		totalVariation /= 100;
			
					// List eigenvalue information by principal component.			
		
		if (continueFlag)
			{		
			index = 0;
			cumulativeVariation = 0;
			cumulativeLogDeterminant = 0;
			cumulativeDeterminant = 1;
			
					// Get field size to use for Eigenvalue and Feature Mean
					
			value = fabs (covariancePtr[0]);
			if (value >= kMinValueToListWith_f && value <= kMaxValueToListWith_f)
				{
				numberFieldSize = 12;
				numberWholeDigits = GetNumberWholeDigits (covariancePtr[0]);
				fFormatDecimalDigits = numberFieldSize - numberWholeDigits - 1;
				fFormatDecimalDigits = MIN (4, fFormatDecimalDigits);
				//fFormatDecimalDigits = MAX (0, fFormatDecimalDigits);
				eFormatDecimalDigits = fFormatDecimalDigits + 0;
				
				}	// end "if (value >= kMinValueToListWith_f && value <= ...)"
				
			else	// value < kMinValueToListWith_f || ...
				{
				numberFieldSize = 12;
				fFormatDecimalDigits = 4;
				eFormatDecimalDigits = 4;
				
				}	// end "else value < kMinValueToListWith_f || ..."
			
			value = 0.;
				
			for (component=1; component<=numberComponents; component++)
				{
				stringPtr = gCharBufferPtr1;
			
						// Component number.														
						
				sprintf (stringPtr, (char*)"    %9d", (unsigned int)component);
				stringPtr += 13;
				
						// Eigenvalue.																
				
				numberChars = LoadRealValueString (stringPtr,
																covariancePtr[index],
																numberFieldSize,
																fFormatDecimalDigits,
																eFormatDecimalDigits,
																(char*)"\t",
																(char*)"");
				stringPtr += numberChars;
				
						// Percent variance.														
				
				if (totalVariation > 0)
					value = covariancePtr[index]/totalVariation;		
					
				sprintf (stringPtr, "\t%10.4f", value);
				stringPtr += 11;
				
						// Cumulative variance.													
						
				cumulativeVariation += covariancePtr[index];													
				
				if (totalVariation > 0)
					value = cumulativeVariation/totalVariation;
					
				sprintf (stringPtr, "\t%9.4f", value);
				stringPtr += 10;
				
						// Cumulative determinant.	
				
				if (covariancePtr[index] != 0)
					cumulativeDeterminant *= covariancePtr[index];
					
				sprintf (stringPtr, "\t%13.4E", cumulativeDeterminant);
				stringPtr += 14;
				
						// Cumulative log determinant.	
				
				if (covariancePtr[index] != 0)
      			cumulativeLogDeterminant += log (fabs (covariancePtr[index]));
      		
				sprintf (stringPtr, "\t%13.4E", cumulativeLogDeterminant);
				stringPtr += 14;
				
						// Feature 'mean'.	
				
				numberChars = LoadRealValueString (stringPtr,
																covariancePtr[index+numberComponents],
																13,
																4,
																5,
																(char*)"\t",
																(char*)"");
				stringPtr += numberChars;
				
				index += indexSkip;
					
				sprintf (stringPtr, "%s", gEndOfLine);
				continueFlag = OutputString (resultsFileStreamPtr, 
														gCharBufferPtr1, 
														0, 
														gOutputCode, 
														continueFlag);
				
				if (!continueFlag)
					break;
					
				}	// end "for (component=1; component<=..." 
				
			}	// end "if (continueFlag)" 
			
		}	// end "if (continueFlag && listEigenvalueFlag)" 
		
			// List the eigenvectors if requested.											
			
	if (continueFlag && listEigenvectorFlag)
		{
				// "\r Eigenvectors:    Channel\r"											
					
		continueFlag = ListSpecifiedStringNumber (kUtilitiesStrID,
																IDS_Utility2, 
																(unsigned char*)gCharBufferPtr1, 
																resultsFileStreamPtr, 
																gOutputCode,
																continueFlag);
		
		stringPtr = gCharBufferPtr1;
		sprintf (gCharBufferPtr1, "    Component");
		stringPtr += 13;
		
		UInt32 channelNumber = 0;
		for (index=0; index<numberChannels; index++)
			{
			channelNumber++;
			if (channelsPtr != NULL)
				channelNumber = channelsPtr[index]+1;
				
					// Channel number.														
				
			sprintf (stringPtr, "\t%8d", (unsigned int)channelNumber);
			stringPtr += 9;
			
			}	// end "for (index=1; index<=..." 
				
		sprintf (stringPtr, "%s", gEndOfLine);
		if (continueFlag)
			continueFlag = OutputString (resultsFileStreamPtr, 
													gCharBufferPtr1, 
													0, 
													gOutputCode, 
													continueFlag);
		
		if (continueFlag)
			for (component=1; component<=numberComponents; component++)
				{
				stringPtr = gCharBufferPtr1;
				sprintf (stringPtr, "    %9d", (unsigned int)component);
				stringPtr += 13;
				
				for (index=0; index<numberChannels; index++)
					{
					sprintf (stringPtr, "\t%8.5f", *eigenVectorPtr);
					stringPtr += 9;
					eigenVectorPtr++;
						
					}	// end "for (index=0; index<..." 
					
				sprintf (stringPtr, "%s", gEndOfLine);
				continueFlag = OutputString (resultsFileStreamPtr, 
														gCharBufferPtr1,
														0, 
														gOutputCode, 
														continueFlag);
			
						// Exit routine if user has "command period" down.				
						
				if (TickCount () >= gNextTime)
					{
					if (!CheckSomeEvents (keyDownMask+mDownMask+mUpMask))
						{
								// "\r Listing was cancelled.\r" 							
					
						continueFlag = ListSpecifiedStringNumber (
																		kUtilitiesStrID,
																		IDS_Utility3, 
																		(unsigned char*)gCharBufferPtr1, 
																		resultsFileStreamPtr, 
																		gOutputCode,
																		continueFlag);
						
						break;
						
						}	// end "if (!CheckSomeEvents (keyDownMask+mDownMask))" 
						
					}	// end "if (TickCount () >= nextTime)" 
						
				if (!continueFlag)
					break;
						
				}	// end "for (component=0; component<numberComponents; ..." 
		
		}	// end "if (continueFlag && ...)" 
		
	return (continueFlag);
					
}	// end "ListEigenTransformationInformation" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ListOffsetGainTransformationInformation
//
//	Software purpose:	The purpose of this routine is to list the offset-gain
//							transformation.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			ListTransformationInformation in SMatrixUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/20/1998
//	Revised By:			Larry L. Biehl			Date: 02/27/2018

Boolean ListOffsetGainTransformationInformation (
				HDoublePtr							offsetVectorPtr, 
				HDoublePtr							gainVectorPtr,
				UInt16								numberChannels,
				Boolean								listOffsetGainFlag)
			
{ 
	CMFileStream*						resultsFileStreamPtr;
	Ptr									stringPtr;
	
	UInt32								channel;
	
	Boolean								continueFlag;
	
	
			// Check global variables that are needed.									
			
	if (gCharBufferPtr1 == NULL)
																						return (FALSE);
	
			// Intialize local variables.														
			
	resultsFileStreamPtr = GetResultsFileStreamPtr (0);
	continueFlag = TRUE;
	
	if (continueFlag && listOffsetGainFlag)
		{
				// List the table heading.														
				// "\r Channel  \tOffset   \tGain\r"														
						
		continueFlag = ListSpecifiedStringNumber (kUtilitiesStrID,
																IDS_Utility11, 
																(unsigned char*)gCharBufferPtr1, 
																resultsFileStreamPtr, 
																gOutputCode,
																continueFlag); 
		
					// List gain and offset by channel.			
		
		if (continueFlag)
			{
					// The offsets are at the end of the offsetVectorPtr.
					
			offsetVectorPtr = &offsetVectorPtr[numberChannels];
					
			for (channel=1; channel<=numberChannels; channel++)
				{
				stringPtr = gCharBufferPtr1;
			
						// Channel number.														
						
				sprintf (stringPtr, (char*)" %8d", (unsigned int)channel);
				stringPtr += 9;
				
						// Offset.																
				
				sprintf (stringPtr, (char*)"\t%12.5f", *offsetVectorPtr);
				stringPtr += 13;
				
						// Gain.
						
				sprintf (stringPtr, (char*)"\t%12.5f", *gainVectorPtr);
				stringPtr += 13;
					
				sprintf (stringPtr, (char*)"%s", gEndOfLine);
				continueFlag = OutputString (resultsFileStreamPtr, 
														gCharBufferPtr1, 
														0, 
														gOutputCode, 
														continueFlag);
				
				if (!continueFlag)
					break;
				
				offsetVectorPtr++;	
				gainVectorPtr += numberChannels + 1;
					
				}	// end "for (component=1; component<=..." 
				
			}	// end "if (continueFlag)" 
			
		}	// end "if (continueFlag && listOffsetGainFlag)" 
		
	return (continueFlag);
					
}	// end "ListOffsetGainTransformationInformation" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ListTransformationInformation
//
//	Software purpose:	The purpose of this routine is to list the results
//							of the principal component analysis.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			FeatureExtraction in SFeatureExtraction.cpp
//							PrincipalComponentControl in SPrincipalComponents.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/04/1991
//	Revised By:			Larry L. Biehl			Date: 04/20/1998

Boolean ListTransformationInformation (
				HDoublePtr							covariancePtr, 
				HDoublePtr							eigenVectorPtr, 
				SInt32								numberIterations, 
				double								smallestElement, 
				UInt16*								channelsPtr, 
				UInt16								numberChannels,
				UInt16								numberComponents, 
				Boolean								listEigenvalueFlag, 
				Boolean								listEigenvectorFlag, 
				SInt16								createByCode)
			
{ 
	Boolean								continueFlag;
	
	
			// Check global variables that are needed.									
			
	if (gCharBufferPtr1 == NULL)
																					return (FALSE);
	
			// Intialize local variables.														
			
	continueFlag = TRUE;
					
			// Intialize the nextTime variable to indicate when the next check	
			// should occur for a command-.													
			
	gNextTime = TickCount ();	
	
	if (createByCode < 16)
		continueFlag = ListEigenTransformationInformation (covariancePtr,
																			eigenVectorPtr, 
																			numberIterations, 
																			smallestElement, 
																			channelsPtr, 
																			numberChannels,
																			numberComponents, 
																			listEigenvalueFlag, 
																			listEigenvectorFlag);
	
	else if (createByCode >= 16)
		continueFlag = ListOffsetGainTransformationInformation (covariancePtr,
																					eigenVectorPtr,
																					numberChannels,
																					listEigenvalueFlag);
	
	return (continueFlag);
					
}	// end "ListTransformationInformation" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void MatrixMultiply
//
//	Software purpose:	The purpose of this routine is to multiply the two
//							input matrices putting the result into the output
//							matrix.  There are three possible situations.
//							multiplyCode = 1: matrix1 times matrix2.
//								The sizes of the matrix must be: 
//								inputMatrix1: (inputSize1 x inputSize2)
//								inputMatrix2: (inputSize2 x inputSize3).  
//
//							multiplyCode = 2: transpose of matrix1 times matrix2.
//								The sizes of the matrix must be: 
//								inputMatrix1: (inputSize2 x inputSize1)
//								inputMatrix2: (inputSize2 x inputSize3).
//
//							multiplyCode = 3: matrix1 times transpose of matrix2.
//								The sizes of the matrix must be: 
//								inputMatrix1: (inputSize1 x inputSize2)
//								inputMatrix2: (inputSize3 x inputSize2). 
//
//							The outputMatrix must have enough memory allocated
//							for a matrix size of (inputSize1 x inputSize3).
//
//	Parameters in:		Pointer to input matrix 1
//							Pointer to input matrix 2
//							Pointer to output matrix
//							input matrix size 1
//							input matrix size 2
//							input matrix size 3
//							code for type of matrix multiply.
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			MaxLikeClsfierControl in SClassify.cpp
//							SetupClsfierStats in SClassifyEchoControl.cpp
//							FS_eigen_inverse_SwSb_gen in SFeatureExtractionMath.cpp
//							EvaluateCovariancesControl in SFeatureExtraction.cpp
//							EvaluateEigenvectorControl in SOther.cpp
//							GetTransformedClassCovarianceMatrix in SProjectComputeStatistics.cpp
//							GetTransformedMeanVector in SProjectComputeStatistics.cpp
//							GetTransformedCovarianceMatrix in SStatisticsImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/12/1993
//	Revised By:			Larry L. Biehl			Date: 03/16/2012	

void MatrixMultiply (
				HDoublePtr 							inputMatrix1Ptr, 
				HDoublePtr 							inputMatrix2Ptr, 
				HDoublePtr							outputMatrixPtr,
				UInt16								inputSize1, 
				UInt16								inputSize2, 
				UInt16								inputSize3, 
				SInt16								multiplyCode)
				
{
   HDoublePtr							lIn1Ptr,
   										lIn2Ptr;
	
	UInt32								i,
   										j,
   										k,
											matrix1Increment,
											matrix2Increment,
   										matrix1Offset;
								
   
   switch (multiplyCode)
   	{
   	case kMat1ByMat2: // matrix1 times matrix2 
			matrix1Offset = inputSize2;
			matrix1Increment = 1;
			matrix2Increment = inputSize3;
   		break;
   		
   	case kTransposeMat1ByMat2: // transpose of matrix1 times matrix2 
			matrix1Offset = 1;
			matrix1Increment = inputSize1;
			matrix2Increment = inputSize3;
   		break;
   		
   	case kMat1ByTransposeMat2: // matrix1 times transpose of matrix2 
			matrix1Offset = inputSize2;
			matrix1Increment = 1;
			matrix2Increment = 1;
   		break;
   	
   	}	// end "switch (multiplyCode)" 
   
   for (i=0; i<inputSize1; i++) 
   	{
		lIn2Ptr = inputMatrix2Ptr;
			
      for (k=0; k<inputSize3; k++) 
      	{
   		lIn1Ptr = inputMatrix1Ptr;
   		
   		if (multiplyCode != 3)
				lIn2Ptr = inputMatrix2Ptr + k;
			
			*outputMatrixPtr = 0.;
			for (j=0; j<inputSize2; j++) 
				{
	   		*outputMatrixPtr += *lIn1Ptr * *lIn2Ptr;
	   		lIn1Ptr += matrix1Increment;
	    		lIn2Ptr += matrix2Increment;
	    		
				}	// end "for (j=0; j<inputSize2; j++)" 
				
			outputMatrixPtr++;
				
					// This will allow the user to move the status window if they
					// wish. Could also be used to cancel the operation but will not
					// allow this at this point right now. 3/16/2012.					
					
			if (TickCount () >= gNextTime)
				CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask);
			
      	}	// end "for (k=0; k<inputSize3; k++)" 
      	
      inputMatrix1Ptr += matrix1Offset;
      
   	}	// end "for (i=0; i<inputSize1; i++)" 
		
}	// end "MatrixMultiply" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void OrderEigenvaluesAndEigenvectors
//
//	Software purpose:	The purpose of the routine is to order the order the eigenvalues
//							and eigenvectors in decreasing values of the eigenvalues. The
//							eigenvectors are expected to be in row format.
//
//	Parameters in:					.
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/15/2002
//	Revised By:			Larry L. Biehl			Date: 04/11/2002	

void OrderEigenvaluesAndEigenvectors (
				HDoublePtr							eigenvaluePtr, 
				HDoublePtr							eigenvectorPtr,
				HDoublePtr							tempVectorPtr,
				UInt32 								numberFeatures,
				SInt16								requestCode)
				
{
	double								x;
	
	UInt32								eigenvalue_i_Index,
											eigenvalue_j_Index,
											eigenvalue_nm_Index,
											i,
											j,
											nm,
											numberBytes;
	
		
			// Order the eigenvectors and eigenvalues. Use the 'tempVectorPtr' vector
			// for temporary holding.
	
	numberBytes = numberFeatures * sizeof (double);
		
	for (i=0; i<numberFeatures; i++)
		{
		nm = i;
		
				// Get Eigenvalue index dependant upon whether they are stored as
				// as vector or as a matrix.
		
		eigenvalue_i_Index = i;	
		if (!(requestCode & 0x0002))
			eigenvalue_i_Index *= numberFeatures + 1;
		
		x = eigenvaluePtr[eigenvalue_i_Index]; 
		
		for (j=i+1; j<numberFeatures; j++)
			{  
			eigenvalue_j_Index = j;	
			if (!(requestCode & 0x0002))
				eigenvalue_j_Index *= numberFeatures + 1;
			 
			if (eigenvaluePtr[eigenvalue_j_Index] > x)
				{
				nm = j;
				eigenvalue_nm_Index = eigenvalue_j_Index;
				x = eigenvaluePtr[eigenvalue_nm_Index]; 
				
				}	// end "if (eigenvaluePtr[j] > w[i])"
				
			}	// end "for (j=i+1; <=m; j++)"
		
		if (nm != i)
			{
					// Interchange the eigenvector rows and eigenvalues so that
					// they are in decending order. 
			
			eigenvaluePtr[eigenvalue_nm_Index] = eigenvaluePtr[eigenvalue_i_Index];
			eigenvaluePtr[eigenvalue_i_Index] = x;
			
			if (requestCode & 0x0001)
				{
				BlockMoveData (&eigenvectorPtr[nm*numberFeatures], 
										tempVectorPtr, 
										numberBytes);
										
				BlockMoveData (&eigenvectorPtr[i*numberFeatures], 
										&eigenvectorPtr[nm*numberFeatures], 
										numberBytes);
										
				BlockMoveData (tempVectorPtr,
										&eigenvectorPtr[i*numberFeatures], 
										numberBytes);
										
				}	// end "if (requestCode & 0x0001)"
			
			}	// end "if (nm != i)"
	   	
	   }	// end "for (i=1; i<=numberFeatures; i++)"
	   
}	// end "OrderEigenvaluesAndEigenvectors"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ReduceInputMatrix
//
//	Software purpose:	The purpose of this routine is to reduce the input
//							lower triangular matrix to just the selected 
//							channels and copy to the lower left triangular 
//							portion of a square matrix or another lower left
//							triangular matrix.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			ComputeCorrelationCoefficientMatrix in SMatrixUtilities.cpp
//							ComputeCovarianceMatrix in SMatrixUtilities.cpp
//							LoadSeparabilityStatistics in SFeatureSelection.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/03/1992
//	Revised By:			Larry L. Biehl			Date: 02/05/1992	

void ReduceInputMatrix (
				UInt16								numberOutputChannels,
				HCovarianceStatisticsPtr		outputSquareMatrixPtr, 
				UInt16								numberInputChannels,
				UInt16*								channelListPtr, 
				HCovarianceStatisticsPtr		inputTriangleMatrixPtr, 
				Boolean								squareOutputMatrixFlag)

{
	UInt32								channel,
											channelListIndexLimit,
											channelListIndex1,
											channelListIndex2,
											channelNum1,
											channelNum2,
											covChan,
											lowerLeftIndexSkip,
											triangleMatrixSkip;
	
	
			// Check input values.  Continue only if input parameters are within	
			// proper rangers																		
			
	if (numberOutputChannels == 0 ||
			!inputTriangleMatrixPtr ||
				numberInputChannels == 0 ||
					!outputSquareMatrixPtr)
																							return;
																						
	if (numberOutputChannels == numberInputChannels)
		channelListPtr = NULL;
				
	channelListIndexLimit = numberOutputChannels - 1;
	lowerLeftIndexSkip = numberOutputChannels - 1;
	
	channelNum1 = 0;
	channelListIndex1 = 0;
	
	triangleMatrixSkip = 1;
	
	for (channel=0; channel<numberInputChannels; channel++)
		{
		if (channelListPtr)
			channelNum1 = channelListPtr[channelListIndex1];
			
		if (channel == channelNum1)
			{
			channelListIndex2 = 0;
			channelNum2 = 0;
			
			for (covChan=0; covChan<=channel; covChan++)
				{	
				if (channelListPtr)
					channelNum2 = channelListPtr[channelListIndex2];
			
				if (covChan == channelNum2)
					{
					*outputSquareMatrixPtr = *inputTriangleMatrixPtr;
					outputSquareMatrixPtr++;
			
					if (channelListIndex2 < channelListIndexLimit)
					 	channelListIndex2++;
					
					}	// end "if (covChan == channelNum2)" 
				
				inputTriangleMatrixPtr++;
			 	channelNum2++;
			
				}	// end "for (covChan=0; covChan<=..." 
				
			if (squareOutputMatrixFlag)
				{
				outputSquareMatrixPtr += lowerLeftIndexSkip;
				lowerLeftIndexSkip--; 
				
				}	// end "if (squareOutputMatrixFlag)" 
			
			if (channelListIndex1 < channelListIndexLimit)
				channelListIndex1++;
			
			}	// end "if (channel == channelNum1)" 
			
		else	// channel != channelNum1 
			inputTriangleMatrixPtr += triangleMatrixSkip;
			
		channelNum1++;
		triangleMatrixSkip++;
			
		}	// end "for (channel=0; channel<numberInputChannels; channel++)" 
					
}	// end "ReduceInputMatrix" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ReduceMatrix
//
//	Software purpose:	The purpose of this routine is to reduce the input
//							matrix to just the selected features in the output
//							matrix.
//
//							input matrix size is inputSize1 by inputSize2;
//							output matrix size is inputSize3 by inputSize2;
//
//							inputSize1 is number of rows in input matrix
//							inputSize2 is number of columns in input and output
//								matrix.
//							inputSize3 is number of row in output matrix
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			BiPlotDataControl in SBiPlotData.cpp
//							LoadBiPlotClassStats in SBiPlotData.cpp
//							LoadSeparabilityStatistics in SFeatureSelection.cpp
//							SetupFeatureTransformationMemory in SMatrixUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/14/1993
//	Revised By:			Larry L. Biehl			Date: 10/10/1997

void ReduceMatrix (
				HDoublePtr							inputMatrixPtr, 
				HDoublePtr							outputMatrixPtr, 
				SInt16								inputSize1, 
				SInt16								inputSize2, 
				SInt16								inputSize3, 
				SInt16*								featureListPtr,
				Boolean								squareFlag)

{
	if (!squareFlag)
		ReduceMatrix1 (inputMatrixPtr, 
								outputMatrixPtr, 
								inputSize1, 
								inputSize2, 
								inputSize3, 
								featureListPtr);
				
	else	// squareFlag
		ReduceMatrix2 (inputMatrixPtr, 
								outputMatrixPtr, 
								inputSize1, 
								inputSize3, 
								featureListPtr);
					
}	// end "ReduceMatrix" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ReduceMatrix1
//
//	Software purpose:	The purpose of this routine is to reduce the input
//							matrix to just the selected features in the output
//							matrix.
//
//							input matrix size is inputSize1 by inputSize2;
//							output matrix size is inputSize3 by inputSize2;
//
//							inputSize1 is number of rows in input matrix
//							inputSize2 is number of columns in input and output
//								matrix.
//							inputSize3 is number of row in output matrix
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			ReduceMatrix in SMatrixUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/13/1997
//	Revised By:			Larry L. Biehl			Date: 10/13/1997

void ReduceMatrix1 (
				HDoublePtr							inputMatrixPtr, 
				HDoublePtr							outputMatrixPtr, 
				SInt16								inputSize1, 
				SInt16								inputSize2, 
				SInt16								inputSize3, 
				SInt16*								featureListPtr)

{
	HDoublePtr							lInputMatrixPtr;
	
	UInt32								element,
											feature,
											featureNum1;
	
	
			// Check input values.  Continue only if input parameters are within	
			// proper ranges.																		
			
	if (inputMatrixPtr == NULL || 
			outputMatrixPtr == NULL ||
			inputSize1 <= 0 || 
			inputSize2 <= 0 || 
			inputSize3 <= 0 ||
			inputSize1 < inputSize3)  
																								return;
																						
	if (inputSize2 == inputSize3)
		featureListPtr = NULL;
	
	featureNum1 = 0;
	
	for (feature=0; feature<(UInt32)inputSize3; feature++)
		{
		if (featureListPtr != NULL)
			featureNum1 = (UInt32)featureListPtr[feature];
			
		else	// featureListPtr == NULL
			featureNum1 = feature;
			
		lInputMatrixPtr = inputMatrixPtr + (featureNum1*inputSize2);
		
		for (element=0; element<(UInt32)inputSize2; element++)
			{	
			*outputMatrixPtr = *lInputMatrixPtr;
			lInputMatrixPtr++;
			outputMatrixPtr++;
		
			}	// end "for (element=0; element<inputSize2; element++)" 
			
		}	// end "for (feature=0; feature<inputSize3; feature++)" 
					
}	// end "ReduceMatrix1"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ReduceMatrix2
//
//	Software purpose:	The purpose of this routine is to reduce the input square matrix 
//							to just the selected features in the output square matrix.
//
//							input matrix size is inputSize1 by inputSize1;
//							output matrix size is inputSize2 by inputSize2;
//
//							inputSize1 is number of rows and columns in input matrix
//							inputSize2 is number of rows and columns in output matrix
//
//							list of features in input matrix to be used in output matrix
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			BiPlotDataControl in SBiPlotData.cpp
//							LoadBiPlotClassStats in SBiPlotData.cpp
//							LoadSeparabilityStatistics in SFeatureSelection.cpp
//							SetupFeatureTransformationMemory in SMatrixUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/13/1997
//	Revised By:			Larry L. Biehl			Date: 10/14/1997

void ReduceMatrix2 (
				HDoublePtr							inputMatrixPtr, 
				HDoublePtr							outputMatrixPtr, 
				UInt32								inputSize1, 
				UInt32								inputSize2, 
				SInt16*								featureListPtr)

{
	HDoublePtr							lInputMatrixPtr;
	
	UInt32								element,
											feature,
											featureNum;
	
	
			// Check input values.  Continue only if input parameters are within	
			// proper ranges.																		
			
	if (inputMatrixPtr == NULL || 
			outputMatrixPtr == NULL ||
				inputSize1 <= 0 ||
					inputSize2 <= 0 ||
						inputSize1 < inputSize2)
																						return;
																						
	if (inputSize1 == inputSize2)
		featureListPtr = NULL;
	
	for (feature=0; feature<inputSize2; feature++)
		{
		if (featureListPtr != NULL)
			featureNum = (UInt32)featureListPtr[feature];
			
		else	// featureListPtr == NULL
			featureNum = feature;
			
		lInputMatrixPtr = inputMatrixPtr + (featureNum*inputSize1);
		
		for (element=0; element<(UInt32)inputSize2; element++)
			{	
			if (featureListPtr != NULL)
				featureNum = (UInt32)featureListPtr[element];
				
			else	// featureListPtr == NULL
				featureNum = element;
				
			*outputMatrixPtr = lInputMatrixPtr[featureNum];
			outputMatrixPtr++;
		
			}	// end "for (element=0; element<inputSize2; element++)" 
			
		}	// end "for (feature=0; feature<inputSize2; feature++)" 
					
}	// end "ReduceMatrix2"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ReduceMaximumVector
//
//	Software purpose:	The purpose of this routine is to copy the maximums
//							for those channels that will be used from the input 
//							vector into the output vector.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			GetClassMaximumVector in SProjectComputeStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/29/2012
//	Revised By:			Larry L. Biehl			Date: 03/29/2012

void ReduceMaximumVector (
				HChannelStatisticsPtr			inputChannelStatsPtr, 
				HDoublePtr							outputMaximumPtr, 
				SInt16								numOutFeatures, 
				SInt16*								featureListPtr)

{
	UInt32								channel,
											channelIndex;
	
	
	if (inputChannelStatsPtr != NULL && outputMaximumPtr != NULL)
		{
		channelIndex = 0;
		
		for (channel=0; channel<(UInt32)numOutFeatures; channel++)
			{
			if (featureListPtr != NULL)
				channelIndex = featureListPtr[channel];
				
			*outputMaximumPtr = inputChannelStatsPtr[channelIndex].maximum;
			
			outputMaximumPtr++;
			channelIndex++;
			
			}	// end "for (channel=0; channel<numOutFeatures; channel++)" 
			
		}	// end "if (inputChannelStatsPtr != NULL && outputMaximumPtr != NULL)"

}	// end "ReduceMaximumVector"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ReduceMeanVector
//
//	Software purpose:	The purpose of this routine is to copy the means
//							for those channels that will be used from the input 
//							vector into the output vector.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			MaxLikeClsfierControl in SClassify.cpp
//							SetupClsfierStats in SClassifyEchoControl.cpp
//							FeatureExtraction in SFeatureExtraction.cpp
//							LoadSeparabilityStatistics in SFeatureSelection.cpp
//							CreateStatisticsImages in SStatisticsImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/15/1988
//	Revised By:			Larry L. Biehl			Date: 07/19/1996

void ReduceMeanVector (
				HChannelStatisticsPtr			inputChannelStatsPtr, 
				HDoublePtr							outputMeansPtr, 
				SInt16								numOutFeatures, 
				SInt16*								featureListPtr)

{
	UInt32								channel,
											channelIndex;
	
	
	if (inputChannelStatsPtr != NULL && outputMeansPtr != NULL)
		{
		channelIndex = 0;
		
		for (channel=0; channel<(UInt32)numOutFeatures; channel++)
			{
			if (featureListPtr != NULL)
				channelIndex = featureListPtr[channel];
				
			*outputMeansPtr = inputChannelStatsPtr[channelIndex].mean;
			
			outputMeansPtr++;
			channelIndex++;
			
			}	// end "for (channel=0; channel<numOutFeatures; channel++)" 
			
		}	// end "if (inputChannelStatsPtr != NULL && outputMeansPtr != NULL)"

}	// end "ReduceMeanVector"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ReduceMinimumVector
//
//	Software purpose:	The purpose of this routine is to copy the minimums
//							for those channels that will be used from the input 
//							vector into the output vector.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			GetClassMinimumVector in SProjectComputeStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/29/2012
//	Revised By:			Larry L. Biehl			Date: 03/29/2012

void ReduceMinimumVector (
				HChannelStatisticsPtr			inputChannelStatsPtr, 
				HDoublePtr							outputMinimumPtr, 
				SInt16								numOutFeatures, 
				SInt16*								featureListPtr)

{
	UInt32								channel,
											channelIndex;
	
	
	if (inputChannelStatsPtr != NULL && outputMinimumPtr != NULL)
		{
		channelIndex = 0;
		
		for (channel=0; channel<(UInt32)numOutFeatures; channel++)
			{
			if (featureListPtr != NULL)
				channelIndex = featureListPtr[channel];
				
			*outputMinimumPtr = inputChannelStatsPtr[channelIndex].minimum;
			
			outputMinimumPtr++;
			channelIndex++;
			
			}	// end "for (channel=0; channel<numOutFeatures; channel++)" 
			
		}	// end "if (inputChannelStatsPtr != NULL && outputMinimumPtr != NULL)"

}	// end "ReduceMinimumVector"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ReleaseFeatureTransformationMemory
//
//	Software purpose:	The purpose of this routine is to release the memory
//							to be used to transform the project statistics.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			MaxLikeClsfierControl in SClassify.cpp
//							SetupClsfierStats in SClassifyEchoControl.cpp
//							EvaluateCovariancesControl in SOther.cpp
//							LoadSeparabilityStatistics in SFeatureSelection.cpp
//							CreateStatisticsImages in SStatisticsImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/11/1994
//	Revised By:			Larry L. Biehl			Date: 10/09/1997	

void ReleaseFeatureTransformationMemory (void)

{
	gTransformationMatrix.tempMatrixPtr = CheckAndDisposePtr (
														gTransformationMatrix.tempMatrixPtr);
											
	gTransformationMatrix.eigenVectorPtr = CheckAndDisposePtr (
														gTransformationMatrix.eigenVectorPtr);
											
	gTransformationMatrix.eigenFeaturePtr = CheckAndDisposePtr (
														gTransformationMatrix.eigenFeaturePtr);
											
	gTransformationMatrix.offsetVectorPtr = CheckAndDisposePtr (
														gTransformationMatrix.offsetVectorPtr);

}	// end "ReleaseFeatureTransformationMemory" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SaveTransformationMatrix
//
//	Software purpose:	The purpose of this routine is save the input
//							transformation matrix information into the global
//							structure.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/23/1993
//	Revised By:			Larry L. Biehl			Date: 02/07/2018

Boolean SaveTransformationMatrix (
				SInt16								createdByCode, 
				Handle								eigenVectorHandle, 
				Handle								eigenValueHandle, 
				Handle								channelsHandle, 
				Handle								preProcBandGroupingHandle,
				UInt16								numberChannels,
				UInt16								numberFeatures)			

{
	SInt16								errCode = -1;
	
	
	if (eigenValueHandle != NULL &&
			eigenVectorHandle != NULL && 
				channelsHandle != NULL)
		{
				// First release memory for the current matrix if any.				
		
		ClearTransformationMatrix (FALSE);	
		
				// Now get a new handle containing the fecture vector.				
		
		gTransformationMatrix.eigenFeatureHandle = channelsHandle;
		errCode = HandToHand (&gTransformationMatrix.eigenFeatureHandle);
								
		if (errCode != noErr)
			gTransformationMatrix.eigenFeatureHandle = NULL;	
		
				// Now get a new handle containing the band grouping
				// vector if needed.				
		
		if (errCode == noErr && preProcBandGroupingHandle != NULL)
			{
			gTransformationMatrix.preProcBandGroupingHandle = 
																preProcBandGroupingHandle;
			errCode = HandToHand (
								&gTransformationMatrix.preProcBandGroupingHandle);
								
			if (errCode != noErr)
				gTransformationMatrix.preProcBandGroupingHandle = NULL;
								
			}	// end "if ((errCode == noErr && ..."
		
		if (errCode == noErr)
			{
			gTransformationMatrix.eigenValueHandle = eigenValueHandle;
			gTransformationMatrix.eigenVectorHandle = eigenVectorHandle;
			gTransformationMatrix.numberChannels = numberChannels;
			gTransformationMatrix.numberFeatures = numberFeatures;
			gTransformationMatrix.createdByCode = createdByCode;
															
			CheckAndUnlockHandle (eigenValueHandle);
			CheckAndUnlockHandle (eigenVectorHandle);
			CheckAndUnlockHandle (gTransformationMatrix.eigenFeatureHandle);
			
			FileStringPtr fileNamePtr = 
						(FileStringPtr)GetFileNameCPointerFromFileInfo (gImageFileInfoPtr);
			CtoPstring (fileNamePtr, gTransformationMatrix.imageFileName);
		
			gUpdateFileMenuItemsFlag = TRUE;
			gUpdateEditMenuItemsFlag = TRUE;
			gUpdateProcessorMenuItemsFlag = TRUE;
			
			}	// end "if (errCode == noErr)" 
	
		}	// end "if (eigenValueHandle && ..." 
		
	return (errCode == noErr);

}	// end "SaveTransformationMatrix"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetIdentityMatrix
//
//	Software purpose:	The purpose of this routine is to set the output matrix to
//							the identity matrix. The routine allows for the output matrix
//							to be triangular or square.
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/24/2000
//	Revised By:			Larry L. Biehl			Date: 02/24/2000	

void SetIdentityMatrix (
				HDoublePtr 							outputMatrixPtr, 
				UInt32 								matrixSize,
				Boolean								squareOutputMatrixFlag)
				
{
	HDoublePtr							savedOutputMatrixPtr;
				
	UInt32								i,
											j,
											upperRightIndexSkip;
											
											
			// Initialize local variables.
											
	upperRightIndexSkip = matrixSize - 1;
	savedOutputMatrixPtr = outputMatrixPtr;
	
	for (i=0; i<matrixSize; i++)
		{
				// These are the lower triangular off diagonal positions.
			
		for (j=0; j<i; j++)
			{
			*outputMatrixPtr = 0.;
			outputMatrixPtr++;
			
			}	// end "for (j=0; j<numberColumns; j++)"
				
				// This is the diagonal position;
					
		*outputMatrixPtr = 1.;
		outputMatrixPtr++;
			
		if (squareOutputMatrixFlag)
			{
			outputMatrixPtr += upperRightIndexSkip;
			upperRightIndexSkip--;
			
			}	// end "if (squareOutputMatrixFlag)"
		
		}	// end "for (i=1; i<=matrixSize; i++)"
	
			// Now copy the lower left part of the matrix to the upper right		
			// part if the output is to be a square matrix.								
				
	if (squareOutputMatrixFlag)
		{
		outputMatrixPtr = savedOutputMatrixPtr;	
		CopyLowerToUpperSquareMatrix ((UInt16)matrixSize, outputMatrixPtr);
		
		}	// end "if (squareOutputMatrixFlag)" 
		
}	// end "SetIdentityMatrix"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean SetupFeatureTransformationMemory
//
//	Software purpose:	The purpose of this routine is to setup the memory
//							to be used to transform the project statistics.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			MaxLikeClsfierControl in SClassify.cpp
//							SetupClsfierStats in SClassifyEchoControl.cpp
//							EvaluateCovariancesControl in SOther.cpp
//							LoadSeparabilityStatistics in SFeatureSelection.cpp
//							CreateStatisticsImages in SStatisticsImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/28/1993
//	Revised By:			Larry L. Biehl			Date: 02/02/1999

Boolean SetupFeatureTransformationMemory (
				Boolean								featureTransformationFlag, 
				SInt16								numberFeatures, 
				SInt16								numberFeatureChannels, 
				HDoublePtr*							eigenVectorPtrPtr, 
				HDoublePtr*							tempMatrixPtrPtr,
				HDoublePtr*							offsetVectorPtrPtr, 
				SInt16**								statEigenFeaturePtrPtr,
				Boolean								loadOutputMatricesFlag,
				UInt16*								outputFeaturePtr)

{
	Boolean								continueFlag;
	
	
	continueFlag = TRUE;
		
	if (eigenVectorPtrPtr != NULL)
		*eigenVectorPtrPtr = NULL;
		
	if (tempMatrixPtrPtr != NULL)
		*tempMatrixPtrPtr = NULL;
		
	if (statEigenFeaturePtrPtr != NULL)
		*statEigenFeaturePtrPtr = NULL;
		
	if (offsetVectorPtrPtr != NULL)
		*offsetVectorPtrPtr = NULL;
	
	if (!eigenVectorPtrPtr || !statEigenFeaturePtrPtr)
		continueFlag = FALSE;
								
	if (continueFlag && featureTransformationFlag)
		{
		continueFlag = (gTransformationMatrix.eigenVectorHandle != NULL);
		
		if (continueFlag && tempMatrixPtrPtr != NULL)
			{
			*tempMatrixPtrPtr = (HDoublePtr)MNewPointer (
							(SInt32)numberFeatureChannels * 
													numberFeatureChannels * sizeof (double));
			continueFlag = (*tempMatrixPtrPtr != NULL);
			
			}	// end "if (continueFlag && tempMatrixPtrPtr)" 
		
		if (continueFlag)
			*eigenVectorPtrPtr = (HDoublePtr)MNewPointer ((SInt32)numberFeatures * 
														numberFeatureChannels * sizeof (double));
		continueFlag = (*eigenVectorPtrPtr != NULL);
		
		if (continueFlag)
			*statEigenFeaturePtrPtr = (SInt16*)MNewPointer (
									(SInt32)numberFeatureChannels * sizeof (SInt16));
		continueFlag = (*statEigenFeaturePtrPtr != NULL);
		
		if (continueFlag && offsetVectorPtrPtr != NULL)
			{
			*offsetVectorPtrPtr = (HDoublePtr)MNewPointer (
											(SInt32)numberFeatureChannels * sizeof (double));
			continueFlag = (*offsetVectorPtrPtr != NULL);
			
			}	// end "if (continueFlag && offsetVectorPtrPtr != NULL)"
		
		if (continueFlag && gProjectInfoPtr != NULL)
			continueFlag = GetEigenStatisticsFeatures (
															(UInt16*)*statEigenFeaturePtrPtr,
															outputFeaturePtr,
															numberFeatures);
															
				// Now if requested, load the output matrices/vectors with the requested
				// data.
								
		if (continueFlag && loadOutputMatricesFlag)
			{                                                 
			HDoublePtr eigenVectorPtr = (HDoublePtr)GetHandlePointer (
												gTransformationMatrix.eigenVectorHandle,
												kLock);
												
			ReduceMatrix (eigenVectorPtr, 
								*eigenVectorPtrPtr, 
								gTransformationMatrix.numberFeatures, 
								numberFeatureChannels,
								numberFeatures, 
								(SInt16*)outputFeaturePtr,
								(gTransformationMatrix.createdByCode >= 16));
								
			CheckAndUnlockHandle (gTransformationMatrix.eigenVectorHandle);
												
			if (offsetVectorPtrPtr != NULL && *offsetVectorPtrPtr != NULL)	
				{
				HDoublePtr inputOffsetPtr = (HDoublePtr)GetHandlePointer (
														gTransformationMatrix.eigenValueHandle,
														kLock);
												
				inputOffsetPtr = &inputOffsetPtr[gTransformationMatrix.numberFeatures];
											
				ReduceMatrix (inputOffsetPtr, 
									*offsetVectorPtrPtr, 
									gTransformationMatrix.numberFeatures, 
									1,
									numberFeatures, 
									(SInt16*)outputFeaturePtr,
									FALSE);		// kNotSquareFlag
								
				CheckAndUnlockHandle (gTransformationMatrix.eigenValueHandle);
				
				}	// end "if (offsetVectorPtrPtr != NULL && ..."
				
			}	// end "if (continueFlag && loadOutputMatricesFlag)"
		
		}	// end "if (continueFlag && ...)" 
		
	return (continueFlag);

}	// end "SetupFeatureTransformationMemory" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SquareToTriangularMatrix
//
//	Software purpose:	The purpose of this routine is to copy a symmetric 
//							matrix stored in square matrix form to storage in
//							lower triangular matrix form.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			MaxLikeClsfierControl   	in SClassify.cpp
//							LoadSeparabilityStatistics in SFeatureSelection.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/19/1988
//	Revised By:			Larry L. Biehl			Date: 04/30/1993

void SquareToTriangularMatrix (
				HDoublePtr							inputSquarePtr, 
				HDoublePtr							outputTrianglePtr,
				SInt16								numOutFeatures)

{
	SInt32								channel,
											covChan,
											inputIndexSkip;

	
			// Initialize local variables.													
			
	inputIndexSkip = numOutFeatures - 1;
		
	for (channel=0; channel<numOutFeatures; channel++)
		{
		for (covChan=0; covChan<=channel; covChan++)
			{
			*outputTrianglePtr = *inputSquarePtr;
			inputSquarePtr++;
			outputTrianglePtr++;
		
			}	// end "for (covChan=0; covChan<=..." 
		
		inputSquarePtr += inputIndexSkip;
		inputIndexSkip--;
					
		}	// end "for (channel=0; ..." 
				
}	// end "SquareToTriangularMatrix" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void TransformDataVector
//
//	Software purpose:	The purpose of this routine is to multiply the two
//							input matrices putting the result into the output
//							matrix.  There are three possible situations.
//							multiplyCode = 1: vector times matrix.
//								The sizes of the matrix must be: 
//								inputVector: (1 x inputSize1)
//								inputMatrix: (inputSize1 x inputSize2).
//
//							The outputMatrix must have enough memory allocated
//							for a vector size of (1 x inputSize2).
//
//	Parameters in:		Pointer to input vector
//							Pointer to input matrix
//							Pointer to output vector
//							input size 1 (length of input vector)
//							input size 2 (length of output vector)
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			GetFieldDataValues in SProjectMatrixUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/12/1993
//	Revised By:			Larry L. Biehl			Date: 06/17/2006	

void TransformDataVector (
				HDoublePtr 							inputDataVectorPtr, 
				HDoublePtr 							inputMatrixPtr,
				HDoublePtr							transformFeatureMeansPtr, 
				HDoublePtr							outputVectorPtr,
				UInt16								inputSize1,
				UInt16								inputSize2)
				
{
	double								doubleValue;
   HDoublePtr							lIn1Ptr;
   
	UInt32								i,
											j;
   
   
   for (i=0; i<inputSize2; i++) 
   	{
   	lIn1Ptr = inputDataVectorPtr;
   	
		doubleValue = 0.;
		for (j=0; j<inputSize1; j++) 
			{
	   	doubleValue += (double)*lIn1Ptr * *inputMatrixPtr;
	   	lIn1Ptr++;
	    	inputMatrixPtr++;
	    		
			}	// end "for (j=0; j<inputSize1; j++)" 
		
				// The only processor that uses this is feature extraction -
				// decision boundary.
				
		*outputVectorPtr = doubleValue;
		outputVectorPtr++;
		
   	}	// end "for (i=0; i<inputSize2; i++)" 
		
}	// end "TransformDataVector" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void TransformDataVector
//
//	Software purpose:	The purpose of this routine is to multiply the two
//							input matrices putting the result into the output
//							matrix.  There are three possible situations.
//							multiplyCode = 1: vector times matrix.
//								The sizes of the matrix must be: 
//								inputVector: (1 x inputSize1)
//								inputMatrix: (inputSize1 x inputSize2).
//
//							The outputMatrix must have enough memory allocated
//							for a vector size of (1 x inputSize2).
//
//	Parameters in:		Pointer to input vector
//							Pointer to input matrix
//							Pointer to output vector
//							input size 1 (length of input vector)
//							Flag indicating whether the eigenvector type transform is to
//								be used or just a gain-offset type of transform
//							output feature number to be returned.
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			CreatePCImage in SReformatChangeImageFileFormat.cpp
//							HistogramFieldStats in SProjectHistogramStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/02/1999
//	Revised By:			Larry L. Biehl			Date: 01/12/2006	

double TransformDataVector (
				HDoublePtr 							inputDataVectorPtr, 
				HDoublePtr 							transformMatrixPtr,
				HDoublePtr							transformFeatureMeansPtr, 
				UInt32								numberChannels,
				Boolean								eigenvectorTypeFlag,
				UInt32								outputFeatureNumber,
				UInt32								inputChannelInterval)
				
{
	HDoublePtr							coefficientsPtr;
	
	double								value;
	
	UInt32								index;
	
	
	coefficientsPtr = &transformMatrixPtr[outputFeatureNumber*numberChannels];

	value = 0.;
							
	if (eigenvectorTypeFlag)
		{
				// Eigenvector type transformation.
				
		for (index=0; index<numberChannels; index++)
			{		
			value += *coefficientsPtr * *inputDataVectorPtr;
			
			inputDataVectorPtr += inputChannelInterval;
			coefficientsPtr++;
			
			}	// end "for (k=0; k<numberChannels; k++)" 
			
		value -= transformFeatureMeansPtr[outputFeatureNumber];
			
		}	// end "if (eigenvectorTypeFlag)"
		
	else	// !eigenvectorTypeFlag
		{
		coefficientsPtr += outputFeatureNumber;
		inputDataVectorPtr += outputFeatureNumber*inputChannelInterval;
		
		value += *coefficientsPtr * *inputDataVectorPtr;
		value += transformFeatureMeansPtr[outputFeatureNumber];
			
		}	// end "else !eigenvectorTypeFlag"
		
	return (value);
			
}	// end "TransformDataVector"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void TransformSymmetricMatrix
//
//	Software purpose:	The purpose of this routine is to transform the input
//							symmetric matrix with the input transformation
//							matrix.
//
//							The algorithm is:
//								transpose of transformation x sym. matrix x transformation.
//
//							The outputMatrix must have enough memory allocated
//							for a matrix size of (inputSize1 x inputSize1).
//
//							The sizes of the matrix must be: 
//								transformation matrix: (inputSize1 x inputSize2)
//								symmetric matrix: (inputSize2 x inputSize2)
//								temporary matrix: (inputSize2 x inputSize1)
//								output matrix: (inputSize1 x inputSize1).
//
//	Parameters in:		Pointer to transformation matrix
//							Pointer to symmetric matrix
//							Pointer to temporary matrix
//							Pointer to output matrix
//							input matrix size 1
//							input matrix size 2
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 06/18/1996
//	Revised By:			Larry L. Biehl			Date: 09/05/1997	

void TransformSymmetricMatrix (
				HDoublePtr 							transformMatrixPtr, 
				HDoublePtr 							symmetricMatrixPtr, 
				HDoublePtr 							tempMatrixPtr, 
				HDoublePtr							outputMatrixPtr,
				UInt16								inputSize1, 
				UInt16								inputSize2,
				Boolean								squareSymmetricMatrixFlag)
				
{
   HDoublePtr							lIn1Ptr,
   										lIn2Ptr,
   										savedOutputMatrixPtr;
								
	UInt32								i,
											j,
											k,
											lowerLeftIndexSkip;
   							
	
	savedOutputMatrixPtr = outputMatrixPtr;
							
			// First get the result of the transpose of the transformation matrix
			// times the symmetric matrix.
										
	MatrixMultiply (transformMatrixPtr, 
								symmetricMatrixPtr, 
								tempMatrixPtr, 
								inputSize1,
								inputSize2,
								inputSize2,
								kMat1ByMat2);
								
			// Now get the result of the temporary matrix computed above times the
			// transformation matrix taking advantage of the fact that the resulting
			// matrix is symmetric.
			
	lowerLeftIndexSkip = inputSize1 - 1;
   
   for (i=0; i<inputSize1; i++) 
   	{
		lIn2Ptr = transformMatrixPtr;
		
      for (k=0; k<=i; k++) 
      	{
   		lIn1Ptr = tempMatrixPtr;
			
			*outputMatrixPtr = 0.;
			for (j=0; j<inputSize2; j++) 
				{
	   		*outputMatrixPtr += *lIn1Ptr * *lIn2Ptr;
	   		lIn1Ptr++;
	    		lIn2Ptr++;
	    		
				}	// end "for (j=0; j<inputSize2; j++)" 
				
			outputMatrixPtr++; 
			
      	}	// end "for (k=0; k<=i; k++)" 
			
		if (squareSymmetricMatrixFlag)
			{
			outputMatrixPtr += lowerLeftIndexSkip;
			lowerLeftIndexSkip--;
			
			}	// end "if (squareSymmetricMatrixFlag)" 
      	
      tempMatrixPtr += inputSize2;
      
   	}	// end "for (i=0; i<inputSize1; i++)" 
	
			// Now copy the lower left part of the matrix to the upper right	
			// part if the output is to be a square matrix.							
	
	if (squareSymmetricMatrixFlag && inputSize1 > 1)
		{
		outputMatrixPtr = savedOutputMatrixPtr;	
		CopyLowerToUpperSquareMatrix (inputSize1, outputMatrixPtr);
		
		}	// end "if (squareSymmetricMatrixFlag && ..." 
		
}	// end "TransformSymmetricMatrix" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ZeroMatrix
//
//	Software purpose:	The purpose of this routine is set memory for the
//							input matrix to 0. If fullMatrixFlag is FALSE then the
//							input matrix is a lower triangular matrix.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			GetClusterProjectStatistics in SCluster.cpp
//							GetTotalSumSquares
//							UpdateFieldStats in SProjectComputeStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 06/19/1996
//	Revised By:			Larry L. Biehl			Date: 09/05/1997	

void ZeroMatrix (
				HDoublePtr							matrixPtr,
				UInt32								numberRows,
				UInt32								numberColumns,
				Boolean								fullMatrixFlag)

{
	UInt32								stop,
											i;
	
	
	if (matrixPtr == NULL)
																					return;
																					
	if (numberRows != numberColumns)
		fullMatrixFlag = TRUE;
	
	if (fullMatrixFlag)
		stop = numberRows * numberColumns;
		
	else	// !fullMatrixFlag, i.e. triangular matrix.
		stop = numberRows * (numberColumns+1) / 2;
								
			// Initialize the memory for the statistics information					
	
	for (i=0; i<stop; i++)
		{
		*matrixPtr = 0;
		matrixPtr++;
			
		}	// end "for (i=0; i<stop; i++)" 

}	// end "ZeroMatrix" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ZeroStatisticsMemory
//
//	Software purpose:	The purpose of this routine is set memory for the
//							statistics information to 0.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			GetClusterProjectStatistics in SCluster.cpp
//							GetTotalSumSquares
//							UpdateFieldStats in SProjectComputeStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/17/1988
//	Revised By:			Larry L. Biehl			Date: 12/31/2005	

void ZeroStatisticsMemory (
				HChannelStatisticsPtr			channelPtr, 
				HCovarianceStatisticsPtr		covariancePtr, 
				UInt16								numberChannels, 
				SInt16								statCode, 
				Boolean								squareMatrixFlag)

{
	UInt32								channel,
											numberRows;
	
								
			// Initialize the memory for the statistics information					
	
	for (channel=0; channel<numberChannels; channel++)
		{
		channelPtr->sum =
			channelPtr->mean = 0;
					
		channelPtr->standardDev = -1;
		channelPtr->minimum = DBL_MAX;
		channelPtr->maximum = -DBL_MAX;
			
		channelPtr++;
			
		}	// end "for (channel=0; channel<..."
		
			// Intialize the variance vector or the covariance matrix depending
			// upon the statCode.
		
	numberRows = numberChannels;
	if (statCode == kMeanStdDevOnly)
		numberRows = 1;
		
	ZeroMatrix (covariancePtr, numberRows, numberChannels, squareMatrixFlag); 

}	// end "ZeroStatisticsMemory" 

