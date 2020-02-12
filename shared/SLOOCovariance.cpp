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
//	File:						SLOOCovariance.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			12/21/2017
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	The purpose of the routines in this file is to
//								implements the Leave-One-Out Covariance (LOOC) Estimate..
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"    

#if defined multispec_wx
#endif
	
#if defined multispec_mac
#endif	// defined multispec_mac

#if defined multispec_win
#endif	// defined multispec_win



void AddScalarTimesDiagonalMatrixToMatrix (
				double								scalarValue,
				HDoublePtr 							inputMatrix1Ptr, 
				HDoublePtr 							inputMatrix2Ptr, 
				HDoublePtr							outputMatrixPtr,
				UInt32								matrixSize,
				Boolean								squareMatrixFlag);
				
void AddScalarTimesMatrixToScalarTimesMatrix (
				double								scalarValue1,
				double								scalarValue2,
				HDoublePtr 							inputMatrix1Ptr, 
				HDoublePtr 							inputMatrix2Ptr, 
				HDoublePtr							outputMatrixPtr,
				UInt32								matrixSize,
				Boolean								squareInputMatrix1Flag,
				Boolean								squareMatrixFlag);
				
void AddScalarXDiagonalMatrixToScalarXMatrix (
				double				 				scalarValue1,
				double				 				scalarValue2, 
				HDoublePtr 							inputMatrix1Ptr, 
				HDoublePtr 							inputMatrix2Ptr, 
				HDoublePtr 							outputMatrixPtr, 
				UInt32 								matrixSize,
				Boolean								squareInputMatricesFlag,
				Boolean								squareOutputMatrixFlag);
				
void CopyFloatVectorToDoubleVector (
				HFloatPtr 							inputVectorPtr, 
				HDoublePtr 							outputVectorPtr,
				UInt32								numberVectorRows);
				
double GetLOOLikelihoodValue (
				double								a,	
				HDoublePtr							savedDataValuesPtr,
				SInt32								numberClassSamples,
				SInt32								numberFeatures,
				HDoublePtr							sampleCovariancePtr,
				HDoublePtr							commonCovariancePtr,
				SInt32								numberCommonCovarianceClasses,
				HDoublePtr							looCovariancePtr);
				
double GetLOOCMixingValue (
				HDoublePtr							dataValuesPtr,
				SInt32								numberClassSamples,
				SInt32								numberFeatures,
				HDoublePtr							sampleCovariancePtr,
				HDoublePtr							commonCovariancePtr,
				SInt32								numberCommonCovarianceClasses,
				HDoublePtr							looCovariancePtr);
				
void MultiplyScalarTimesMatrix (
				double				 				scalarValue, 
				HDoublePtr 							inputMatrixPtr,
				HDoublePtr 							outputMatrixPtr, 
				UInt32 								numberRows, 
				UInt32								numberColumns,
				Boolean								fullMatrixFlag);
				
void SubtractVectors (
				HDoublePtr 							inputVector1Ptr, 
				HDoublePtr 							inputVector2Ptr, 
				HDoublePtr							outputVectorPtr,
				UInt32								numberVectorRows);

SInt16 UpdateClassLOOStats (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				HChannelStatisticsPtr			classChannelStatsPtr, 
				ClassInfoPtr						classInfoPtr,
				UInt16								classNumber,
				UInt32								numberLOOSamples);



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void AddScalarTimesDiagonalMatrixToMatrix
//
//	Software purpose:	The purpose of this routine is to add the scalar times the diagonal
//							elements for inputMatrix1 to inputMatrix2 and put the result in
//							the outputMatrix. The sizes of the matrices have to be square
//							with matrixSize rows and columns. The input and output matrices
//							may also be of lower triangular form if the squareMatrixFlag
//							is FALSE. If inputMatrix1 and outputMatrix 
//							(or inputMatrix2 and outputMatrix) point to the same 
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
//	Coded By:			Larry L. Biehl			Date: 09/05/1997
//	Revised By:			Larry L. Biehl			Date: 09/05/1997

void AddScalarTimesDiagonalMatrixToMatrix (
				double								scalarValue,
				HDoublePtr 							inputMatrix1Ptr, 
				HDoublePtr 							inputMatrix2Ptr, 
				HDoublePtr							outputMatrixPtr,
				UInt32								matrixSize,
				Boolean								squareMatrixFlag)
				
{
	UInt32								i,
											indexSkip;
								
								
			// Check input values.  Continue only if input parameters are within	
			// proper rangers									
								
	if (matrixSize <= 0 || 
			inputMatrix1Ptr == NULL ||
					inputMatrix2Ptr == NULL ||
							outputMatrixPtr == NULL)
																								return;
						
																		
	indexSkip = 1;
	if (squareMatrixFlag)
		indexSkip += matrixSize;											
		
	for (i=0; i<matrixSize; i++)
		{
		*outputMatrixPtr = *inputMatrix1Ptr * scalarValue + *inputMatrix2Ptr;
		
		if (!squareMatrixFlag)
			indexSkip++;
		
		inputMatrix1Ptr += indexSkip;
		inputMatrix2Ptr += indexSkip;
		outputMatrixPtr += indexSkip;
		
		}	// end "for (i=0; i<matrixSize; i++)"
		
}	// end "AddScalarTimesDiagonalMatrixToMatrix"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void AddScalarTimesMatrixToScalarTimesMatrix
//
//	Software purpose:	The purpose of this routine is to add the scalar1 times 
//								inputMatrix1 to the scalar2 times inputMatrix2 and put the 
//								result in the outputMatrix.
// 
//							The sizes of the matrices may be:
//								all square with matrixSize rows and columns.
//								all in lower triangular form.
//								input matrix1 may be in lower triangular form and input
//									matrix2 and the output matrix in full square form. The
//									squareOutputMatrixFlag flag actually specifies the format
//									for input matrix2 and the output matrix.
//
//							If inputMatrix1 and outputMatrix (or inputMatrix2 and outputMatrix) 
//								point to the same place, the addition is done "in place."
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 09/05/1997
//	Revised By:			Larry L. Biehl			Date: 09/24/1997

void AddScalarTimesMatrixToScalarTimesMatrix (
				double								scalarValue1,
				double								scalarValue2,
				HDoublePtr 							inputMatrix1Ptr, 
				HDoublePtr 							inputMatrix2Ptr, 
				HDoublePtr							outputMatrixPtr,
				UInt32								matrixSize,
				Boolean								squareInputMatrix1Flag,
				Boolean								squareOutputMatrixFlag)
				
{
	HDoublePtr							savedOutputMatrixPtr;
				
	UInt32								i,
											j,
											upperRightIndexSkip;
								
								
			// Check input values.  Continue only if input parameters are within	
			// proper rangers									
								
	if (matrixSize > 0 && 
						inputMatrix1Ptr != NULL && 
											inputMatrix2Ptr != NULL && 
																outputMatrixPtr != NULL)  
		{
				// Initialize local variables.
			
		upperRightIndexSkip = matrixSize - 1;					
		savedOutputMatrixPtr = outputMatrixPtr;											
			
		for (i=0; i<matrixSize; i++)
			{
			for (j=0; j<=i; j++)
				{
				*outputMatrixPtr = 
						scalarValue1 * *inputMatrix1Ptr + scalarValue2 * *inputMatrix2Ptr;
			
				inputMatrix1Ptr++;
				inputMatrix2Ptr++;
				outputMatrixPtr++;
				
				}	// end "for (j=0; j<i; j++)"
			
			if (squareOutputMatrixFlag)
				{
				inputMatrix2Ptr += upperRightIndexSkip;
				outputMatrixPtr += upperRightIndexSkip;
					
				if (squareInputMatrix1Flag)	
					inputMatrix1Ptr += upperRightIndexSkip;
				 		
				upperRightIndexSkip--;
				
				}	// end "if (squareOutputMatrixFlag)" 
			
			}	// end "for (i=0; i<matrixSize; i++)"
	
				// Now copy the lower left part of the matrix to the upper right		
				// part if the output is to be a square matrix.								
					
		if (squareOutputMatrixFlag)
			{
			outputMatrixPtr = savedOutputMatrixPtr;
			CopyLowerToUpperSquareMatrix ((UInt16)matrixSize, outputMatrixPtr);
			
			}	// end "if (squareOutputMatrixFlag)" 
			
		}	// end "if (matrixSize > 0 && ..."
		
}	// end "AddScalarTimesMatrixToScalarTimesMatrix"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void AddScalarXDiagonalMatrixToScalarXMatrix
//
//	Software purpose:	The purpose of this routine is to multiply scalar1 times the
//							diagonal component of inputMatrix1 (numberRows by numberColumns) 
//							and add the result to scalar2 times inputMatrix2  (numberRows by 
//							numberColumns), and put the result in outputMatrix (numberRows by
//							numberColumns).
// 
//							The sizes of the matrices may be:
//								all square with matrixSize rows and columns.
//								all in lower triangular form.
//								input matrix1 and input matrix 2 may be in lower triangular
//									form and the output matrix in full square form.
//
//							If inputMatrix1, inputMatrix2 and outputMatrix 
//								point to the same place, the operation is done "in place."
//
//							All input matrices are assumed to be symmetric.
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Joe Hoffbeck			Date: 09/24/1997
//	Revised By:			Larry L. Biehl			Date: 09/24/1997	

void AddScalarXDiagonalMatrixToScalarXMatrix (
				double				 				scalarValue1,
				double				 				scalarValue2, 
				HDoublePtr 							inputMatrix1Ptr, 
				HDoublePtr 							inputMatrix2Ptr, 
				HDoublePtr 							outputMatrixPtr, 
				UInt32 								matrixSize,
				Boolean								squareInputMatricesFlag,
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
			*outputMatrixPtr = *inputMatrix2Ptr * scalarValue2;
			
			inputMatrix2Ptr++;
			outputMatrixPtr++;
			
			}	// end "for (j=0; j<numberColumns; j++)"
				
		inputMatrix1Ptr += i;
				
				// This is the diagonal position;
					
		*outputMatrixPtr = 
						scalarValue1 * *inputMatrix1Ptr + scalarValue2 * *inputMatrix2Ptr;
						
		inputMatrix1Ptr++;
		inputMatrix2Ptr++;
		outputMatrixPtr++;
			
		if (squareOutputMatrixFlag)
			{
			outputMatrixPtr += upperRightIndexSkip;
				
			if (squareInputMatricesFlag)	
				{
				inputMatrix1Ptr += upperRightIndexSkip;
				inputMatrix2Ptr += upperRightIndexSkip;
				
				}	// end "if (squareInputMatricesFlag)"
			 		
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
		
}	// end "AddScalarXDiagonalMatrixToScalarXMatrix"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CopyFloatVectorToDoubleVector
//
//	Software purpose:	The purpose of this routine is to subtract inputMatrix2 
//							(m by n) from inputMatrix1 (m by n), and puts the result in 
//							outputMatrix (m by n). If inputMatrix1 and outputMatrix 
//							(or inputMatrix2 and outputMatrix) point to the same 
//							place, the subtraction is done "in place."
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Joe Hoffbeck			Date: 09/24/1997
//	Revised By:			Larry L. Biehl			Date: 09/24/1997	

void CopyFloatVectorToDoubleVector (
				HFloatPtr 							inputVectorPtr, 
				HDoublePtr 							outputVectorPtr,
				UInt32								numberVectorRows)
				
{
	UInt32								i;
													
	
	for (i=0; i<numberVectorRows; i++)
		{
		*outputVectorPtr = *inputVectorPtr;
		
		inputVectorPtr++;
		outputVectorPtr++;
		
		}	// end "for (i=0; i<numberVectorRows; i++)"
		
}	// end "CopyFloatVectorToDoubleVector"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean DetermineIfLOOCProjectMemoryNeeded
//
//	Software purpose:	The purpose of this routine is to determine if memory is needed
//							for the leave-one-out statistics in the project. Currently
//							this includes the memory needed to compute and store the
//							common covariance matrix. Memory is needed if any of the 
//							LOOC statistics are used for any of the classes or if the
//							LOOC mixing parameter exists for any of the classes.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			SetupStatsMemory in SProjectComputeStatistics.cpp
//
//	Coded By:			Joe Hoffbeck			Date: 09/24/1997
//	Revised By:			Larry L. Biehl			Date: 12/03/1999	

Boolean DetermineIfLOOCProjectMemoryNeeded (void)
				
{
	HPClassNamesPtr					classNamesPtr;
	
	UInt32								classIndex,
											classStorage,
											numberClasses;
						
	Boolean								projectLOOCMemoryNeededFlag = FALSE;
	
	
	if (gProjectInfoPtr->useCommonCovarianceInLOOCFlag)
		{	
				// First check if LOOC stats are to be used for any of the classes.
					
		classNamesPtr = gProjectInfoPtr->classNamesPtr;
		numberClasses = gProjectInfoPtr->numberStatisticsClasses;
	
		for (classIndex=0; classIndex<numberClasses; classIndex++)
			{
					// Get the class storage number.											
						
			classStorage = gProjectInfoPtr->storageClass[classIndex];
			
					// Check if class covariance is represented by Leave One Out method.
						
			if (classNamesPtr[classStorage].covarianceStatsToUse == kLeaveOneOutStats)
				{
				projectLOOCMemoryNeededFlag = TRUE;
				break;
				
				}	// end "if (...covarianceStatsToUse == kLeaveOneOutStats)"
			
			}	// end "for (classIndex=0; ... 
		
				// Now check if the LOOC mixing parameter exists for some of the
				// classes which implies that the memory may be needed at some point.
					
		if (!projectLOOCMemoryNeededFlag)
			projectLOOCMemoryNeededFlag = DetermineIfLOOExists (-1);
			
		}	// end "if (gProjectInfoPtr->useCommonCovarianceInLOOCFlag)"
		
	return (projectLOOCMemoryNeededFlag);
		
}	// end "DetermineIfLOOCProjectMemoryNeeded"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean DetermineIfLOOExists
//
//	Software purpose:	The purpose of this routine is to determine if the leave-one-out
//							parameter exists for any class in the project or for the selected
//							input class.
//
//	Parameters in:		Selected class. If = -1, then check all classes.
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			SetupStatsMemory in SProjectComputeStatistics.cpp
//
//	Coded By:			Joe Hoffbeck			Date: 02/04/1999
//	Revised By:			Larry L. Biehl			Date: 02/04/1999	

Boolean DetermineIfLOOExists (
				SInt16								selectedClass)
				
{
	HPClassNamesPtr					classNamesPtr;
	
	UInt32								classIndex,
											classStorage,
											numberClasses,
											startIndex;
						
	Boolean								looStatsExistFlag = FALSE;
	
						
	classNamesPtr = gProjectInfoPtr->classNamesPtr;
	
	if (selectedClass <= -1)
		{
		startIndex = 0;					
		numberClasses = gProjectInfoPtr->numberStatisticsClasses;
		
		}	// end "if (selectedClass <= -1)"
		
	else	// selectedClass >= 0
		{
		startIndex = selectedClass;					
		numberClasses = selectedClass + 1;
		
		}	// end "else selectedClass >= 0"

	for (classIndex=startIndex; classIndex<numberClasses; classIndex++)
		{
				// Get the class storage number.											
					
		classStorage = gProjectInfoPtr->storageClass[classIndex];
		
				// Check if class covariance is represented by Leave One Out method.
					
		if (classNamesPtr[classStorage].looCovarianceValue >= 0)
			{
			looStatsExistFlag = TRUE;
			break;
			
			}	// end "if (...covarianceStatsToUse == kLeaveOneOutStats)"
		
		}	// end "for (classIndex=0; ... 
		
	return (looStatsExistFlag);
		
}	// end "DetermineIfLOOExists"
				
				

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double GetLOOLikelihoodValue
//
//	Software purpose:	The purpose of this routine is to compute the leave-out-out 
//							likelihood value for the input mixing value, data values,
//							sample covariance and common covariance.
//
//	Parameters in:		a - mixing value to use.
//							savedDataValuesPtr - samples from ONE class stored in BIS format
//							numberClassSamples - number of samples in dataValuesPtr
//							numberFeatures - number of features used (nf)
//							sampleCovariancePtr - covariance matrix (nf by nf)
//							commonCovariancePtr - common covariance (nf by nf)
//							numberCommonCovarianceClasses - number of classes used to compute
//																the common covariance							
//
//	Parameters out:	looCovariancePtr - loo covariance matrix (nf by nf)
//
// Value Returned:	likelihood value for the input mixing value
// 
// Called By:			GetLOOCMixingValue in SLOOCovariance.cpp
//
//	Coded By:			Joe Hoffbeck			Date: 06/28/1995
//	Revised By:			Larry L. Biehl			Date: 06/17/2006	

double GetLOOLikelihoodValue (
				double								a,	
				HDoublePtr							savedDataValuesPtr,
				SInt32								numberClassSamples,
				SInt32								numberFeatures,
				HDoublePtr							sampleCovariancePtr,
				HDoublePtr							commonCovariancePtr,
				SInt32								numberCommonCovarianceClasses,
				HDoublePtr							looCovariancePtr)
																		
{						
	double								k1,
											d,
											likelihood,
											logdet,
											tempFactor,
											tempValue;
												
	HDoublePtr 							tempMatrixPtr,
											tempVectorPtr;
							
	HDoublePtr							dataValuesPtr;
	
	UInt32			 					i;
	
	Boolean								determinantOKFlag;
	
	
	tempVectorPtr = gInverseMatrixMemory.pivotPtr;
	tempMatrixPtr = looCovariancePtr;
				
	if (a >= 0 && a <= 1)
		{
				// compute gPtr = a*(numberClassSamples-1)/(numberClassSamples-2) *
				// 		sampleCovariancePtr + (1-a)*diag (diag (sampleCovariancePtr))
				
		AddScalarXDiagonalMatrixToScalarXMatrix (
										1-a,
										a*(numberClassSamples-1)/(numberClassSamples-2), 
										sampleCovariancePtr, 
										sampleCovariancePtr, 
										looCovariancePtr, 
										numberFeatures,
										kTriangularMatrices,
										kTriangularMatrices);
		
		k1 = a*numberClassSamples/(numberClassSamples-2)/(numberClassSamples-1);
		
		}	// end "if (a >= 0 && a <= 1)"
		
	else if (a > 1 && a <= 2)
		{
				// compute gPtr = ((2-a)*(numberClassSamples-1)/(numberClassSamples-2) + 
				//			(a-1)/numberCommonCovarianceClasses/(numberClassSamples-2)) *
				//									sampleCovariancePtr + (a-1)*commonCovariancePtr
				
		AddScalarTimesMatrixToScalarTimesMatrix (
							((2-a)*(numberClassSamples-1)/(numberClassSamples-2) + 
								(a-1)/numberCommonCovarianceClasses/(numberClassSamples-2)),
							a-1,
							sampleCovariancePtr,
							commonCovariancePtr,
							looCovariancePtr,
							numberFeatures,
							kTriangularInputMatrix1,
							kTriangularMatrices);
												
				// compute k1 = (numberCommonCovarianceClasses*(2-a)*numberClassSamples + 
				// 			(a-1)*numberClassSamples) /numberCommonCovarianceClasses/
				//									(numberClassSamples-2)/(numberClassSamples-1)
				
		k1 = (numberCommonCovarianceClasses*(2-a)*numberClassSamples + 
						(a-1)*numberClassSamples)/numberCommonCovarianceClasses/
													(numberClassSamples-2)/(numberClassSamples-1);
		
		}	// end "else if (a > 1 && a <= 2)"
		
	else if (a > 2 && a <= 3)
		{
				// compute gPtr = (3-a)*commonCovariancePtr + 
				//			(3-a)/numberCommonCovarianceClasses/(numberClassSamples-2) *
				//					sampleCovariancePtr + (a-2)*diag (diag (commonCovariancePtr))
				
		AddScalarTimesMatrixToScalarTimesMatrix (
							3-a, 
							(3-a)/numberCommonCovarianceClasses/(numberClassSamples-2),
							commonCovariancePtr,
							sampleCovariancePtr,
							looCovariancePtr,
							numberFeatures,
							kTriangularInputMatrix1,
							kTriangularMatrices);
		/*
		diag (commonCovariancePtr, tmp_v, numberFeatures, numberFeatures);
		MultiplyScalarTimesMatrix (
							tmp_v, a-2, tmp_v, numberFeatures, 1, kTriangularMatrices);
		diag (tmp_v, tmp_m, numberFeatures, 1);
		AddMatrices (gPtr, tmp_m, gPtr, numberFeatures, numberFeatures);
		*/
		AddScalarTimesDiagonalMatrixToMatrix (a-2,
															commonCovariancePtr,
															looCovariancePtr, 
															looCovariancePtr,
															numberFeatures,
															kTriangularMatrices);
		
		k1 = (3-a) * numberClassSamples/numberCommonCovarianceClasses/
													(numberClassSamples-2)/(numberClassSamples-1);
		
		}	// end "else if (a > 2 && a <= 3)"
		
	else	// a < 0 || a > 3
				// a is not between 0 and 3 inclusive as required. Return the likelihood
				// as a huge negative value.
																					return (-HUGE_VAL);
	
	//logdet = invert_m (gPtr,numberFeatures);	// Replace gPtr with its inverse
	determinantOKFlag = InvertLowerTriangularMatrix (
										looCovariancePtr, 
										NULL, 			// Do not need inverse in triangular form
										(UInt16)numberFeatures, // Size of square matrix
										NULL, 
										&logdet, 		// Log Determinant of input matrix
										kReturnMatrixInverse);
							
	if (gOperationCanceledFlag)
																					return (-HUGE_VAL);
										
	if (determinantOKFlag)
		{
		likelihood = 0.0;			// initialize likelihood
		dataValuesPtr = savedDataValuesPtr;
		tempFactor = (double)numberClassSamples/(numberClassSamples-1);
		tempFactor *= tempFactor;
		
		for (i=0; i<(UInt32)numberClassSamples; i++)		// sample counter
			{
			TransformSymmetricMatrix (dataValuesPtr,
												gInverseMatrixMemory.inversePtr,
												tempMatrixPtr, 
												&d,
												1, 
												(UInt16)numberFeatures,
												kSquareOutputMatrix);
			
			//like[j] = like[j] - logdet - log (1 - k1*d) -
			//			square ((double)numberClassSamples/(numberClassSamples-1)) *
			//																			(d / (1 - k1*d));

			tempValue = 1 - k1*d;
			likelihood -= logdet + log (tempValue) + tempFactor*d/tempValue;

			dataValuesPtr += numberFeatures;
			
					// Exit routine if user has "command period" down					
			
			if (TickCount () >= gNextTime)
				{
				if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
																						return (-HUGE_VAL);
				
				}	// end "if (TickCount () >= gNextTime)"
			
			}	// end "for (i=0; i<numberClassSamples; i++)"
			
		}	// end "if (determinantOKFlag)"
		
	else	// !determinantOKFlag
		likelihood = -HUGE_VAL;

	return (likelihood);

}	// end "GetLOOLikelihoodValue"
				
				

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double GetLOOCMixingValue
//
//	Software purpose:	The purpose of this routine is to compute the leave-out-out 
//							covariance for the input class. The looc estimate is returned in 
//							cloo, where cloo points to a previously allocated block of memory 
//							big enough to hold an (n by n) matrix of doubles.
//							The function returns the value of the mixing parameter (a) that 
//							was selected.
//							BUGS: if c has enough samples to be non-singular, but is indeed 
//							singular, looc doesn't recognize this, and returns incorrect 
//							results.
//
//	Parameters in:		savedDataValuesPtr - samples from ONE class stored in BIS format
//							numberClassSamples - number of samples in dataValuesPtr
//							numberFeatures - number of features used (nf)
//							sampleCovariancePtr - covariance matrix (nf by nf)
//							commonCovariancePtr - common covariance (nf by nf)
//							numberCommonCovarianceClasses - number of classes used to compute
//																the common covariance							
//
//	Parameters out:	looCovariancePtr - loo covariance matrix (nf by nf)
//
// Value Returned:	mixing value that gives the largest likelihood value
// 
// Called By:			UpdateClassLOOStats in SLOOCovariance.cpp
//
//	Coded By:			Joe Hoffbeck			Date: 06/28/1995
//	Revised By:			Larry L. Biehl			Date: 10/08/1997	

double GetLOOCMixingValue (
				HDoublePtr							savedDataValuesPtr,
				SInt32								numberClassSamples,
				SInt32								numberFeatures,
				HDoublePtr							sampleCovariancePtr,
				HDoublePtr							commonCovariancePtr,
				SInt32								numberCommonCovarianceClasses,
				HDoublePtr							looCovariancePtr)
																		
{						
	double								a,
											increment,
											maximumA,
											minimumA,
												// initial mixing parameter values
											defaultalist[13] = 
														{0, .25, .5, .75, 1, 1.25, 1.5, 1.75, 2, 
																					2.25, 2.5, 2.75, 3},
											like[13];
							
	UInt32								j,
											loop,
											maximumAIndex,
											numalist = 13;	// number of initial mixing parameter 
																// values
	
			
			// loocOneClass must have at least 3 samples to avoid divide by zero
			
	if (numberClassSamples < 3)
		{
				// There are less than 3 samples, so return 3, diag common covariance,
				// if the number of common covariance classes is more than 1 or
				// return 0, diag sample covariance, if the number of common covariance
				// classes is 0 or 1.
				
		a = 3.0;
		
		if (numberCommonCovarianceClasses <= 1)
			a = 0.0;
		
		}	// end "if (numberClassSamples < 3)"
		
	else	// numberClassSamples >= 3
		{
				// Initialize the likelihood vector.
				
		for (j=0; j<numalist; j++)
			like[j] = 1;
			
				// Now loop through 6 times to get a finer and finer estimate for a.
				// 6 time will get the value to the nearest .00390625.
		
		for (loop=1; loop<=6; loop++)
			{
			if (loop == 1)
				{
						// If there is only 1 common covariance class and it is the same
						// as the input sample class then only do the mixing parameters
						// from 0 to 1. All others will be replications.
						
						// Eventually will need to check that actual list of classes for
						// the common covariance.
						
				if (numberCommonCovarianceClasses <= 1)
					{
					numalist = 5;
					
					}	// end "if (numberCommonCovarianceClasses <= 1)"
				
				}	// end "if (loop == 1)"
			
			else	// loop > 1
				{
				if (maximumAIndex == 0)
					{
					minimumA = defaultalist[maximumAIndex];
					maximumA = defaultalist[maximumAIndex + 1];
					
					like[2] = like[1];
					numalist = 3;
					
					}	// end "if (maximumAIndex == 0)"
				
				else if (maximumAIndex == numalist-1)
					{
					minimumA = defaultalist[maximumAIndex - 1];
					maximumA = defaultalist[maximumAIndex];
					
					like[0] = like[maximumAIndex - 1];
					like[2] = like[maximumAIndex];
					
					numalist = 3;
					
					}	// end "else if (maximumAIndex == maxIndex)"
				
				else	// maximumAIndex > 0 && maximumAIndex < numalist-1
					{
					minimumA = defaultalist[maximumAIndex - 1];
					maximumA = defaultalist[maximumAIndex + 1];
					
							// Save like[maximumAIndex+1] to another location
							
					like[5] = like[maximumAIndex+1];
					
					like[0] = like[maximumAIndex-1];
					like[2] = like[maximumAIndex];
					like[4] = like[5];						// like[maximumAIndex+1]
					
					numalist = 5;
					
					}	// end "else maximumAIndex > 0 && maximumAIndex < maxIndex"
				
						// This signifies that defaultalist[1 & 3] need to be calculated.
							
				like[1] = 1;
				like[3] = 1;
					
				increment = (maximumA - minimumA)/(numalist-1);
				
				for (j=0; j<numalist-1; j++)
					defaultalist[j] = minimumA + j*increment; 
					
				defaultalist[numalist-1] = maximumA;
				
				}	// end "if (loop > 1)"
		
			for (j=0; j<numalist; j++)	// loop thru the different values of a
				{
						// Determine if the likelihood needs to be calculated for this
						// a.
						
				if (like[j] > 0)
					{
					a = defaultalist[j];	// for convenience, copy jth value from alist
					
					if (numberClassSamples < numberFeatures+1 && a == 1.0)
						like[j] = -HUGE_VAL;	// skip a=1 case because sample cov must 
													// be singular
						
					else	// numberClassSamples > numberFeatures+1) || a != 1.0
						like[j] = GetLOOLikelihoodValue (a,	
																	savedDataValuesPtr,
																	numberClassSamples,
																	numberFeatures,
																	sampleCovariancePtr,
																	commonCovariancePtr,
																	numberCommonCovarianceClasses,
																	looCovariancePtr);
					
					}	// end "if (like[j] > 0)"
							
				if (gOperationCanceledFlag)
																								return (-1);
					
				}	// end "for (j=0; j<numalist; j++)"
			
			maximumAIndex = FindMaxValueInVector (like, numalist, &maximumA);

					// We can quit if the difference of the min and max is less than 1. 
					
			FindMinValueInVector (like, numalist, &minimumA);
			if (maximumA - minimumA < 1)
				{
				loop = 7;
				
				}	// end "if (maximumA - minimumA)"
			
			}	//	end "for (loop=1; loop<=6; loop++)"
			
		a = defaultalist[maximumAIndex];
	
		}	// end "else numberClassSamples >= 3"
		
	if (a < 0 || a > 3)
		a = 1.0;

	return (a);		// return the best value of the mixing parameter a

}	// end "GetLOOCMixingValue"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetLOOCovariance
//
//	Software purpose:	The purpose of this routine is to compute the leave-out-out 
//							covariance for the input class. The looc estimate is returned in 
//							cloo, where cloo points to a previously allocated block of memory 
//							big enough to hold an (n by n) matrix of doubles.
//							The function returns the value of the mixing parameter (a) that 
//							was selected.
//							BUGS: if c has enough samples to be non-singular, but is indeed 
//							singular, looc doesn't recognize this, and returns incorrect 
//							results.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			GetClassCovarianceMatrix in SProjectComputeStatistics.cpp
//
//	Coded By:			Joe Hoffbeck			Date: 06/28/1995
//	Revised By:			Larry L. Biehl			Date: 02/24/2000	

void GetLOOCovariance (
				SInt16								mixingParameterCode,
				double								loocMixingParameter,
				double								userMixingParameter,
				SInt32								numberFeatures,
				HDoublePtr							sampleCovariancePtr,
				HDoublePtr							commonCovariancePtr,
				HDoublePtr							looCovariancePtr, 
				Boolean								squareOutputMatrixFlag)
																		
{	
	double								mixingParameter;
	
					
	if (mixingParameterCode != kIdentityMatrix)
		{
		if (mixingParameterCode == kComputedOptimum)
			mixingParameter = loocMixingParameter;
			
		else	// mixingParameterCode == kUserSet
			mixingParameter = userMixingParameter;
			
		if (mixingParameter < 0 || mixingParameter > 3)
			mixingParameter = 1.0;
	
		if (mixingParameter >= 0 && mixingParameter <= 1)
			{
					// compute cout = (1-a)*diag (diag (sampleCovariancePtr)) +
					//															a*sampleCovariancePtr
					
			AddScalarXDiagonalMatrixToScalarXMatrix (1-mixingParameter,
																	mixingParameter, 
																	sampleCovariancePtr, 
																	sampleCovariancePtr, 
																	looCovariancePtr, 
																	numberFeatures,
																	squareOutputMatrixFlag,
																	squareOutputMatrixFlag);
			
			}	// end "if (mixingParameter >= 0 && mixingParameter < 1)"
			
		else if (mixingParameter > 1 && mixingParameter <= 2)
			{
					// compute cout = (2-a)*sampleCovariancePtr + 
					//										(a-1)*commonCovariancePtr
				
			AddScalarTimesMatrixToScalarTimesMatrix (mixingParameter-1,
																	2-mixingParameter, 
																	commonCovariancePtr, 
																	sampleCovariancePtr, 
																	looCovariancePtr, 
																	numberFeatures,
																	kTriangularInputMatrix1,
																	squareOutputMatrixFlag);
			
			}	// end "else if (mixingParameter > 1 && (mixingParameter <= 2)"
			
		else if (mixingParameter > 2 && mixingParameter <= 3)
			{
					// compute cout = (3-a)*commonCovariancePtr + 
					//										(a-2)*diag (diag (commonCovariancePtr));
					
			AddScalarXDiagonalMatrixToScalarXMatrix (mixingParameter-2,
																	3-mixingParameter, 
																	commonCovariancePtr, 
																	commonCovariancePtr, 
																	looCovariancePtr, 
																	numberFeatures,
																	kTriangularInputMatrices,
																	squareOutputMatrixFlag);
			
			}	// end "else if (mixingParameter > 2 && mixingParameter <= 3)"
			
		}	// end "if (mixingParameterCode != kIdentityMatrix)"
			
	else	// mixingParameterCode == kIdentityMatrix
		{								
		SetIdentityMatrix (looCovariancePtr, 
									numberFeatures,
									squareOutputMatrixFlag);			
		
		}	// end "else mixingParameterCode == kIdentityMatrix"

}	// end "GetLOOCovariance"


/*
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void MultiplyScalarTimesMatrix
//
//	Software purpose:	The purpose of this routine is to multiply inputMatrix 
//							(m by n) by the scalar k, and put the result in 
//							outputMatrix (m by n). If inputMatrix and outputMatrix 
//							point to the same place, the scalar multiply is done "in place."
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Joe Hoffbeck			Date: 06/28/1995
//	Revised By:			Larry L. Biehl			Date: 09/04/1997	

void MultiplyScalarTimesMatrix (
				double				 				scalarValue, 
				HDoublePtr 							inputMatrixPtr, 
				HDoublePtr 							outputMatrixPtr, 
				UInt32 								numberRows, 
				UInt32								numberColumns,
				Boolean								fullMatrixFlag)
				
{
	UInt32								i,
											stop;
						
																					
	if (numberRows != numberColumns)
		fullMatrixFlag = TRUE;
	
	if (fullMatrixFlag)
		stop = numberRows * numberColumns;
		
	else	// !fullMatrixFlag, i.e. triangular matrix.
		stop = numberRows * (numberColumns+1) / 2;
	
	for (i=0; i<stop; i++)
		{
		*outputMatrixPtr = *inputMatrixPtr * scalarValue;
		outputMatrixPtr++;
		inputMatrixPtr++;
		
		}	// end "for (i=0; i<stop; i++)"
		
}	// end "MultiplyScalarTimesMatrix"
*/



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ResetProjectLOOStats
//
//	Software purpose:	The purpose of this routine is to reset the leave-one-out
//							mixing parameters based on a changed in the leave-one-out
//							parameters.
//
//	Parameters in:		None
//
//	Parameters out:	
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/10/1997
//	Revised By:			Larry L. Biehl			Date: 10/10/1997	

void ResetProjectLOOStats (
				Boolean								newUseCommonCovarianceInLOOCFlag)

{
	HPClassNamesPtr					classNamesPtr;
	
	UInt32								classIndex,
											classStorage,
											numberClasses;
	
	
	if (gProjectInfoPtr->useCommonCovarianceInLOOCFlag ==
															newUseCommonCovarianceInLOOCFlag)
																	
																									return;
	
			// One of the leave-one-out condition parameters have changed, reset
			// any class leave-one-out mixing values.
			
	classNamesPtr = gProjectInfoPtr->classNamesPtr;
	numberClasses = gProjectInfoPtr->numberStatisticsClasses;
		
	for (classIndex=0; classIndex<numberClasses; classIndex++)
		{
				// Get the class storage number.											
					
		classStorage = gProjectInfoPtr->storageClass[classIndex];
		
		classNamesPtr[classStorage].looCovarianceValue = -1;
		
				// Determine if the class and project statistics need to be updated.
		
		if (classNamesPtr[classStorage].covarianceStatsToUse == kLeaveOneOutStats)
			{
			gProjectInfoPtr->statsUpToDate = FALSE;
			gProjectInfoPtr->classNamesPtr[classStorage].statsUpToDate = FALSE;
				
			}	// end "if (....covarianceStatsToUse == kLeaveOneOutStats)"
									
		}	// end "for (classIndex=0; classIndex<numberClasses; classIndex++)"
	
			// Hilite the 'update statistics' control if needed.						
	
	if (!gProjectInfoPtr->statsUpToDate && gProjectInfoPtr->updateControlH)
		MHiliteControl (gProjectWindow, gProjectInfoPtr->updateControlH, 0);
					
}	// end "ResetProjectLOOStats" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetClassMixingParameter
//
//	Software purpose:	The purpose of this routine is to set the leave-one-out
//							mixing parameter for the input class to the input user 
//							requested value.
//
//	Parameters in:		None
//
//	Parameters out:	
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/23/2000
//	Revised By:			Larry L. Biehl			Date: 02/24/2000	

void SetClassMixingParameter (
				SInt16								classNumber,
				SInt16								mixingParameterCode,
				double								userMixingParameter)

{
	HPClassNamesPtr					classNamesPtr;
	
	UInt32								classStorage;
	
	
	if (mixingParameterCode == kUserSet &&
				(userMixingParameter < 0 || userMixingParameter > 3))
																								return;
	
	classNamesPtr = gProjectInfoPtr->classNamesPtr;
		
			// Get the class storage number.											
				
	classStorage = gProjectInfoPtr->storageClass[classNumber];
	
	classNamesPtr[classStorage].mixingParameterCode = mixingParameterCode;
	classNamesPtr[classStorage].userMixingParameter = userMixingParameter;
	
	if (mixingParameterCode == kUserSet && userMixingParameter > 1)
		gProjectInfoPtr->useCommonCovarianceInLOOCFlag = TRUE;
					
}	// end "SetClassMixingParameter" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetProjectMixingParameter
//
//	Software purpose:	The purpose of this routine is to set the leave-one-out
//							mixing parameter for all project classes to the input user 
//							requested value.
//
//	Parameters in:		None
//
//	Parameters out:	
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/23/2000
//	Revised By:			Larry L. Biehl			Date: 02/24/2000	

void SetProjectMixingParameter (
				SInt16								mixingParameterCode,
				double								userMixingParameter)

{
	UInt32								classIndex,
											numberClasses;
	
	
	if (userMixingParameter < 0 || userMixingParameter > 3)
																								return;
																									
	gProjectInfoPtr->mixingParameterCode = mixingParameterCode;				
	gProjectInfoPtr->userMixingParameter = userMixingParameter;
	
	numberClasses = gProjectInfoPtr->numberStatisticsClasses;
		
	for (classIndex=0; classIndex<numberClasses; classIndex++)
		SetClassMixingParameter ((SInt16)classIndex, 
											mixingParameterCode,
											userMixingParameter);
					
}	// end "SetProjectMixingParameter" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SubtractVectors
//
//	Software purpose:	The purpose of this routine is to subtract inputMatrix2 
//							(m by n) from inputMatrix1 (m by n), and puts the result in 
//							outputMatrix (m by n). If inputMatrix1 and outputMatrix 
//							(or inputMatrix2 and outputMatrix) point to the same 
//							place, the subtraction is done "in place."
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Joe Hoffbeck			Date: 06/28/1995
//	Revised By:			Larry L. Biehl			Date: 06/17/2006	

void SubtractVectors (
				HDoublePtr 							inputVector1Ptr, 
				HDoublePtr 							inputVector2Ptr, 
				HDoublePtr							outputVectorPtr,
				UInt32								numberVectorRows)
				
{
	UInt32								i;
													
	
	for (i=0; i<numberVectorRows; i++)
		{
		*outputVectorPtr = *inputVector1Ptr - *inputVector2Ptr;
		
		inputVector1Ptr++;
		inputVector2Ptr++;
		outputVectorPtr++;
		
		}	// end "for (i=0; i<numberVectorRows; i++)"
		
}	// end "SubtractVectors"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 UpdateClassLOOStats
//
//	Software purpose:	The purpose of this routine is to update the leave-one-out 
//							statistics for the given class.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			UpdateProjectLOOStats in SLOOCovariance.cpp
//
//	Coded By:			Larry L. Biehl			Date: 09/17/1997
//	Revised By:			Larry L. Biehl			Date: 12/15/2010	

SInt16 UpdateClassLOOStats (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				HChannelStatisticsPtr			classChannelStatsPtr, 
				ClassInfoPtr						classInfoPtr,
				UInt16								classNumber,
				UInt32								numberLOOSamples)

{
	HCovarianceStatisticsPtr		commonCovariancePtr;
	HPClassNamesPtr					classNamesPtr;
												
	SInt64								numberDataValues;
	
	UInt32								classStorage,
											index;
	
	SInt16								numberChannels;
	
	
			// Do not compute leave-one-out mixing value if covariance matrix is not
			// being used in the project.
			
	if (gProjectInfoPtr->statisticsCode != kMeanCovariance)
																								return (1);
																								
			// Initialize local variables.													
			
	numberChannels = gProjectInfoPtr->numberStatisticsChannels;
	classNamesPtr = gProjectInfoPtr->classNamesPtr;
			
			// Get pointer to memory (already defined) to store the common covariance
			// in.
			// Note: Handle is already locked.
			
	commonCovariancePtr = (HCovarianceStatisticsPtr)GetHandlePointer (
												gProjectInfoPtr->commonCovarianceStatsHandle);
	
			// Get the class storage number.													
						
	classStorage = gProjectInfoPtr->storageClass[classNumber];
	
			// Set up status dialog. Class name.											
			
	LoadDItemString (gStatusDialogPtr, 
							IDC_Status6, 
							(Str255*)&classNamesPtr[classStorage].name);
	
			// Get the data values for the training pixels to be used in the leave-
			// one-out calculation.	
			
	numberDataValues = GetClassDataValues (fileIOInstructionsPtr,
														classNumber, 
														gProjectInfoPtr->channelsPtr,
														numberChannels, 
														classInfoPtr->dataValuesPtr,
														NULL,
														NULL,
														0,
														0);		
	
			// Continue if there are some data values to be used.				
	
	if (numberDataValues > 0)
		{
		GetClassCovarianceMatrix (numberChannels,
											classChannelStatsPtr, 
											classInfoPtr->covariancePtr, 
											NULL, 
											classNumber,
											kTriangleOutputMatrix,
											kMeanCovariance,
											kOriginalStats);
		
				// Copy mean data into mean vector.										
			
		ReduceMeanVector (classChannelStatsPtr,
								classInfoPtr->meanPtr,
								numberChannels,
								NULL);
		
				// Subtract the mean from the data values. Doing this operation here
				// reduces repeated operations later.
			
		HDoublePtr dataValuesPtr = classInfoPtr->dataValuesPtr;
		for (index=0; index<numberDataValues; index++)
			{
			SubtractVectors (dataValuesPtr,
								classInfoPtr->meanPtr, 
								dataValuesPtr, 
								numberChannels);
								
			dataValuesPtr += numberChannels;
								
			}	// end "for (index=0; index<numberDataValues; index++)"
					
		classNamesPtr[classStorage].looCovarianceValue = GetLOOCMixingValue (
										classInfoPtr->dataValuesPtr,
										(SInt32)numberDataValues,
										numberChannels,
										classInfoPtr->covariancePtr,	// sample covariance
										commonCovariancePtr,				// common covariance
										gProjectInfoPtr->numberCommonCovarianceClasses,
										classInfoPtr->transformedCovPtr); // loo covariance
			
		}	// end "if (numberDataValues > 0)"
		
	if (classNamesPtr[classStorage].looCovarianceValue < 0)
																							return (0);
	
			// Indicate that routine completed normally.								
			
	return (1);
		
}	// end "UpdateClassLOOStats" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 UpdateProjectLOOStats
//
//	Software purpose:	The purpose of this routine is to update the
//							statistics for the project.
//
//	Parameters in:		None
//
//	Parameters out:	
//
// Value Returned:	0 = Update did not complete because of a problem
// 						1 = Everything okay, no LOOC stats were updated
// 						2 = Update completed okay and LOOC stats were updated
//							
// Called By:			GetClassCovarianceMatrix in SProjectComputeStatistics.cpp
//							UpdateStatsControl in SProjectComputeStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 09/17/1997
//	Revised By:			Larry L. Biehl			Date: 12/15/2010	

SInt16 UpdateProjectLOOStats (
				SInt16								statsWindowMode,
				FileIOInstructionsPtr			fileIOInstructionsPtr)

{
	ClassInfoPtr						classInfoPtr = NULL;
	HChannelStatisticsPtr			tempChannelStatsPtr = NULL;
	HCovarianceStatisticsPtr		commonCovariancePtr;
	HPClassNamesPtr					classNamesPtr;
	UInt16*								classPtr;

	SInt64								maxNumberOfPixels;
	
	UInt32								classIndex,
											classStorage,
											numberChannels,
											numberClasses,
											numberClassesToUpdate;
									
	SInt16								returnCode = 1;
	
	UInt16								classNumber;
									
	Boolean								computeCommonCovarianceFlag,
											continueFlag,
											newInverseMemoryFlag;
	
	
			// Do not compute leave-one-out mixing value if covariance matrix is not
			// being used in the project.
			
	if (gProjectInfoPtr->statisticsCode != kMeanCovariance)
																					return (returnCode);
	
			// Do not need to update the leave-one-out stats if only field statistics
			// are being update.
																								
	if (statsWindowMode == kUpdateField)
																					return (returnCode);
	
			// Initialize local variables.
			
	classNamesPtr = gProjectInfoPtr->classNamesPtr;
	numberChannels = gProjectInfoPtr->numberStatisticsChannels;
	
				// Initialize the number of classes and the class vector to only include 
				// all project classes.
			
	numberClasses = gProjectInfoPtr->numberStatisticsClasses;
	classPtr = NULL;
	
	if (statsWindowMode == kUpdateClass)
		{
				// Set number of classes and the class vector to only include the current
				// class since the statistics mode is 'kUpdateClass'.
			
		numberClasses = 1;
		classNumber = gProjectInfoPtr->currentClass;
		classPtr = &classNumber;
		
		}	// end "if (statsWindowMode == kUpdateClass)"
	
			// Determine if any of the classes, based on the input statistics mode,
			// are using a leave-one-out computed covariance matrix that need updating.
			// Also determine if the common covariance needs to be computed.
		
	numberClassesToUpdate = 0;
	computeCommonCovarianceFlag = FALSE;
	for (classIndex=0; classIndex<numberClasses; classIndex++)
		{                
		if (classPtr == NULL)
			classNumber = (UInt16)classIndex;
	
				// Get the class storage number.											
					
		classStorage = gProjectInfoPtr->storageClass[classNumber];
		
				// Check if class covariance is represented by Leave One Out method.
					
		if (classNamesPtr[classStorage].covarianceStatsToUse == kLeaveOneOutStats)
			{
			if (gProjectInfoPtr->useCommonCovarianceInLOOCFlag &&
										gProjectInfoPtr->numberCommonCovarianceClasses == 0)
				computeCommonCovarianceFlag = TRUE;
				
			if (classNamesPtr[classStorage].looCovarianceValue < 0)
				{
				numberClassesToUpdate++;
				break;
				
				}	// end "if (...[classStorage].looCovarianceValue < 0)"
			
			}	// end "if (...covarianceStatsToUse == kLeaveOneOutStats && ..."
									
		}	// end "for (classIndex=0; classIndex<numberClasses; classIndex++)"
	
			// Continue only if number of classes to be update is one or more.													
	
	if (computeCommonCovarianceFlag || numberClassesToUpdate > 0)
		{
		if (gStatusDialogPtr != NULL)
			{
					// Load "Updating Leave-One-Out Statistics For:" in the status dialog
					
			LoadDItemStringNumber (kProjectStrID, 
											IDS_Project54,
											gStatusDialogPtr, 
											IDC_Status11, 
											(Str255*)gTextString);
			
			HideStatusDialogItemSet (kStatusField);
			HideStatusDialogItemSet (kStatusLine);
			
			}	// end "if (gStatusDialogPtr != NULL)"
					
				// Set up status dialog.  Load in number of classes.					
				
		LoadDItemValue (gStatusDialogPtr, IDC_Status5, (SInt32)numberClasses);
		
				// Get memory for class info structure for one class. The leave-
				// one-out covariances are computed one class at a time.
				
		classInfoPtr = GetClassInfoStructure (1);	
		continueFlag = (classInfoPtr != NULL);
			
				// Allocate memory for variables within the structure.
		
		if (continueFlag)
			continueFlag = AssignClassInfoMemory (
							classInfoPtr,
							NULL, 
							1,									// one class
							numberChannels,
							numberChannels,
							0,									// no float data Values
							kAssignMemory+kTriangular,	// used for sample covariance
							0,									
							kAssignMemory,					// used for class means
							kAssignMemory+kSquare,	// used for loo covariance & temp matrix
							0);								// no transformed means
		
				// Get pointer to memory to use for class channel statistics		
				// that will be used in computing the covariance matrix and			
				// getting the data for the class mean vector.							
		
		if (continueFlag)
			{			
			tempChannelStatsPtr = (ChannelStatisticsPtr)MNewPointer (
												numberChannels * sizeof (ChannelStatistics));
			continueFlag = (tempChannelStatsPtr != NULL);
			
			}	// end "if (continueFlag)"
			
				// Get pointer to memory (already defined) to store the common covariance
				// in.
				// Note: Handle is already locked.
				
		commonCovariancePtr = (HCovarianceStatisticsPtr)GetHandlePointer (
													gProjectInfoPtr->commonCovarianceStatsHandle);
		
		if (gProjectInfoPtr->useCommonCovarianceInLOOCFlag &&
																			commonCovariancePtr == NULL)
			continueFlag = FALSE;
		
		if (continueFlag && gProjectInfoPtr->useCommonCovarianceInLOOCFlag)
			continueFlag = GetCommonCovariance (
									commonCovariancePtr, // used to store common covariance
									classInfoPtr->covariancePtr,
									tempChannelStatsPtr,
									NULL,
									(UInt16*)gProjectInfoPtr->channelsPtr,  
									gProjectInfoPtr->numberStatisticsClasses,
									(UInt16)numberChannels,
									kTriangleOutputMatrix,
									kMeanCovariance,
									kOriginalStats,
									TRUE);
									
		if (!gProjectInfoPtr->useCommonCovarianceInLOOCFlag)
			gProjectInfoPtr->numberCommonCovarianceClasses = 0;
		
		if (continueFlag && numberClassesToUpdate > 0 && fileIOInstructionsPtr != NULL)
			{
					// Get the number of pixels in the class with the largest number of
					// training pixels.
					
			maxNumberOfPixels = GetMaximumPixelsPerClass (
														classPtr, 
														numberClasses, 
														kTrainingType,
														kLeaveOneOutStats, 
														kDontIncludeClusterFields);
														
					// Get memory for the training pixels to be used.
						
			classInfoPtr->dataValuesPtr = 
						(HDoublePtr)MNewPointer (maxNumberOfPixels *
								gProjectInfoPtr->numberStatisticsChannels * sizeof (double));
			
			continueFlag = (classInfoPtr->dataValuesPtr != NULL);
			
			newInverseMemoryFlag = (gInverseMatrixMemory.inversePtr == NULL);
			
			if (continueFlag && newInverseMemoryFlag)
						// Get memory needed for computation of determinant and matrix 
						// inverse. inversePtr and pivotPtr will also be used as a 
						// temporary matrix and vector, respectively, when computing the 
						// leave-one-out mixing value.
				continueFlag = SetupMatrixInversionMemory (
													gProjectInfoPtr->numberStatisticsChannels,
													kMeanCovariance,
													&gInverseMatrixMemory.inversePtr, 
													&gInverseMatrixMemory.pivotPtr, 
													&gInverseMatrixMemory.indexRowPtr, 
													&gInverseMatrixMemory.indexColPtr, 
													&gInverseMatrixMemory.ipvotPtr);
			
			if (continueFlag)
				{		
				for (classIndex=0; classIndex<numberClasses; classIndex++)
					{
					if (classPtr == NULL)
						classNumber = (UInt16)classIndex;
				
							// Set up status dialog.  Load in class count.						
						
					LoadDItemValue (gStatusDialogPtr, IDC_Status3, (SInt32)classIndex+1);
					
							// Get the class storage number.											
								
					classStorage = gProjectInfoPtr->storageClass[classNumber];
					
							// Check if class covariance is represented by Leave One Out 
							// method.
								
					if (classNamesPtr[classStorage].covarianceStatsToUse == 
																					kLeaveOneOutStats &&
											classNamesPtr[classStorage].looCovarianceValue < 0)
						{
								// Leave-One-Out method is used for this class and it needs to 
								// be updated.
											
						returnCode = UpdateClassLOOStats (fileIOInstructionsPtr, 
																		tempChannelStatsPtr,
																		classInfoPtr,
																		(UInt16)classNumber,
																		(UInt32)maxNumberOfPixels);
						
								// Ignore any errors for a given class so that subsequent
								// class mixing parameters can be computed. Unless the user
								// canceled the operation.
																
						//if (returnCode <= 0)
						if (gOperationCanceledFlag)
							{
							continueFlag = FALSE;
							break;
							
							}	// end "if (returnCode <= 0)"
																									
						}	// end "if (...covarianceStatsToUse == kLeaveOneOut && ..."
					
					}	// end "for (classIndex=0; ... 
					
				}	// end "if (continueFlag)"
			
			if (newInverseMemoryFlag)	
				ReleaseMatrixInversionMemory ();
				
			}	// end "if (continueFlag && numberClassesToUpdate > 0)"
			
		CheckAndDisposePtr ((Ptr)tempChannelStatsPtr);
		ReleaseClassInfoMemory (classInfoPtr, 1);
		
				// Indicate that the memory for the local common covariance is not 
				// up to date.
		
		gProjectInfoPtr->localCommonCovarianceLoadedFlag = FALSE;
			
		}	// end "if (computeCommonCovarianceFlag || numberClassesToUpdate > 0)" 
	
			// Indicate that the project stats are up-to-date only if this is a project
			// update. If a class update then need to create a routine to check if all
			// classes are up-to-date before setting the project up-to-date flag.
			// Do not do this now.
					
	//if (statsWindowMode == kUpdateProject)
	//	gProjectInfoPtr->statsUpToDate = TRUE;
	
	gUpdateProjectMenuItemsFlag = TRUE;
	
	if (returnCode > 0)
		returnCode = (SInt16)(1 + numberClassesToUpdate);				
			
	return (returnCode);
		                                        
}	// end "UpdateProjectLOOStats" 

	
