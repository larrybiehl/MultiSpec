//											MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2018)
//						(c) Purdue Research Foundation
//								All rights reserved.
//
//	File:						SProjectMatrixUtilities.cpp
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
//								provide matrix utility type functions for MultiSpec 
//								that are to be load at all time.
//
//	Functions in file:	SInt32	 				GetClassDataValues
//								SInt32 					GetFieldDataValues
//								double 					InvertLowerTriangularMatrix
//								double 					InvertSymmetricMatrix
//								void 						ReleaseMatrixInversionMemory
//								Boolean 					SetupMatrixInversionMemory
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h" 

#if defined multispec_lin
	#include "SMultiSpec.h"
#endif
	
#if defined multispec_mac
#endif	// defined multispec_mac    

#if defined multispec_win
	#include "CImageWindow.h"
#endif	// defined multispec_win

//#include "SExtGlob.h"	


double FindMaxDiagonalValueInSquareMatrix (
				HDoublePtr 							squareMatrixPtr, 
				UInt32								matrixSize);



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32 FindMaxDiagonalValueInSquareMatrix
//
//	Software purpose:	The purpose of this routine is to find the maximum diagonal 
//							element in the input double square matrix and return that maximum 
//							value.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 06/17/2006
//	Revised By:			Larry L. Biehl			Date: 06/17/2006	

double FindMaxDiagonalValueInSquareMatrix (
				HDoublePtr 							squareMatrixPtr, 
				UInt32								matrixSize)
				
{
	double 								maximum;
	
	UInt32 								i, 
											increment;
							
	
	maximum = *squareMatrixPtr;
	increment = matrixSize + 1;
	
	for (i=1; i<matrixSize; i++)
		{
		squareMatrixPtr += increment;
		if (*squareMatrixPtr > maximum)
			maximum = *squareMatrixPtr;		// new maximum
		
		}	// end "for (i=1; i<matrixSize; i++)"
		
	return (maximum);
	
}	// end "FindMaxDiagonalValueInSquareMatrix"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32 FindMaxValueInVector
//
//	Software purpose:	The purpose of this routine is to find the maximum element
//							in the input double vector and return that maximum value and
//							its index. If there are more that one maximum element, the index
//							of the first one is returned.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			FeatureExtraction in featureExtraction.c
//
//	Coded By:			Larry L. Biehl			Date: 09/05/1997
//	Revised By:			Larry L. Biehl			Date: 09/05/1997	

UInt32 FindMaxValueInVector (
				HDoublePtr 							doubleVectorPtr, 
				UInt32								vectorLength,
				HDoublePtr							maximumPtr)
				
{
	double 								maximum;
	
	UInt32 								i, 
											indexOfMaximum;
							
	
	maximum = doubleVectorPtr[0];
	indexOfMaximum = 0;
	doubleVectorPtr++;
	
	for (i=1; i<vectorLength; i++)
		{
		if (*doubleVectorPtr > maximum)
			{
			maximum = *doubleVectorPtr;		// new maximum
			indexOfMaximum = i;					// index of new maximum
			
			}	// end "if (*doubleVectorPtr > maximum)"
			
		doubleVectorPtr++;
		
		}	// end "for (i=0; i<vectorLength; i++)"
		
	*maximumPtr = maximum;
		
	return (indexOfMaximum);
	
}	// end "FindMaxValueInVector"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32 FindMinValueInVector
//
//	Software purpose:	The purpose of this routine is to find the minimum element
//							in the input double vector and return that minimum value and
//							its index. If there are more that one maximum element, the index
//							of the first one is returned.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			FeatureExtraction in featureExtraction.c
//
//	Coded By:			Larry L. Biehl			Date: 10/01/1997
//	Revised By:			Larry L. Biehl			Date: 10/01/1997	

UInt32 FindMinValueInVector (
				HDoublePtr 							doubleVectorPtr, 
				UInt32								vectorLength,
				HDoublePtr							minimumPtr)
				
{
	double 								minimum;
	
	UInt32 								i, 
											indexOfMinimum;
							
	
	minimum = doubleVectorPtr[0];
	indexOfMinimum = 0;
	doubleVectorPtr++;
	
	for (i=1; i<vectorLength; i++)
		{
		if (*doubleVectorPtr < minimum)
			{
			minimum = *doubleVectorPtr;		// new minimum
			indexOfMinimum = i;					// index of new minimum
			
			}	// end "if (*doubleVectorPtr < minimum)"
			
		doubleVectorPtr++;
		
		}	// end "for (i=0; i<vectorLength; i++)"
		
	*minimumPtr = minimum;
		
	return (indexOfMinimum);
	
}	// end "FindMinValueInVector"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt32 GetClassDataValues
//
//	Software purpose:	The purpose of this routine is to load the input floating point 
//							vector with the data values for the specified class stored in 
//							BIS format.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			FeatureExtraction in featureExtraction.c
//							UpdateClassLOOStats in SLooCov.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/29/1992
//	Revised By:			Larry L. Biehl			Date: 08/26/2010	

SInt64 GetClassDataValues (
				FileIOInstructionsPtr			fileIOInstructionsPtr, 
				SInt16								classNumber, 
				UInt16*								channelsPtr,
				SInt16								numberChannels, 
				HDoublePtr							doubleDataValuePtr,
				HDoublePtr							transformMatrixPtr,
				HDoublePtr							transformFeatureMeansPtr,
				UInt16								numberFeatures,
				SInt16								optionsCode)

{
	SInt64								numberPixels,
											totalNumberPixels;
	
	HPClassNamesPtr					classNamesPtr;
	HPFieldIdentifiersPtr			fieldIdentPtr;

	SInt16								classStorage,
											fieldNumber;
	
	Boolean								continueFlag;
	
	
			// Initialize local variables.													
			
	classNamesPtr = gProjectInfoPtr->classNamesPtr;
	fieldIdentPtr = gProjectInfoPtr->fieldIdentPtr;
	continueFlag = TRUE;
	totalNumberPixels = 0; 
	
			// Initialize time to check for command period.								
			
	gNextTime = TickCount ();
	
			// Get the class storage number.													
						
	classStorage = gProjectInfoPtr->storageClass[classNumber];
	
			// Continue only if the number of fields in the class is one or more	
			// and if the class statistics are not up-to-date							
	
	if (classNamesPtr[classStorage].numberOfTrainFields > 0)
		{
				// Determine the interval to use.											
		
		numberPixels = 0;									
		fieldNumber = classNamesPtr[classStorage].firstFieldNumber;
		while (fieldNumber != -1)
			{
					// Make certain that field is training field							
					
			if (fieldIdentPtr[fieldNumber].fieldType == kTrainingType &&
							fieldIdentPtr[fieldNumber].pointType != kClusterType)
				numberPixels += fieldIdentPtr[fieldNumber].numberPixels;
				
			fieldNumber = fieldIdentPtr[fieldNumber].nextField;
			
			}	// end "while (fieldNumber != -1)" 
		
		if (gFeatureExtractionSpecsPtr != NULL && (optionsCode & kComputeColumnInterval))
			{	
			gAreaDescription.columnInterval = (SInt32)((double)numberPixels/
							gFeatureExtractionSpecsPtr->maximumPixelsPerClass + 0.5);
			gAreaDescription.columnInterval = MAX (gAreaDescription.columnInterval, 1);
													
			}	// end "if (gFeatureExtractionSpecsPtr != NULL)"
			
		else	// gFeatureExtractionSpecsPtr == NULL
			{	
			gAreaDescription.columnInterval = 1;
			gAreaDescription.lineInterval = 1;
													
			}	// end "if (gFeatureExtractionSpecsPtr != NULL)"
												
		fieldNumber = classNamesPtr[classStorage].firstFieldNumber;
		
		while (fieldNumber != -1 && continueFlag)
			{
					// Make certain that field is training field
					// Do not include the field if it has not been included in the 
					// statistics.						
					
			if (fieldIdentPtr[fieldNumber].fieldType == kTrainingType &&
							fieldIdentPtr[fieldNumber].pointType != kClusterType &&
												fieldIdentPtr[fieldNumber].loadedIntoClassStats)
				{
				GetFieldBoundary (gProjectInfoPtr, &gAreaDescription, fieldNumber);
					
				numberPixels = GetFieldDataValues (&gAreaDescription, 
																fileIOInstructionsPtr, 
																channelsPtr, 
																numberChannels,
																NULL, 
																doubleDataValuePtr,
																transformMatrixPtr,
																transformFeatureMeansPtr,
																numberFeatures,
																optionsCode);
											
				if (numberPixels >= 0)
					{
					if (transformMatrixPtr != NULL && numberFeatures > 0)
								// Result is transformed data values						
						doubleDataValuePtr += numberPixels * numberFeatures;
						
					else	// Data values were not transformed		
						doubleDataValuePtr += numberPixels * numberChannels;
						
					totalNumberPixels += numberPixels;
					
					}	// end "if (numberPixels >= 0)"
					
				else	// numberPixels < 0
					continueFlag = FALSE;
						
						// Dispose of the region if it exists.						

				CloseUpAreaDescription (&gAreaDescription);
								
				}	// end "if (fieldIdentPtr[fieldNumber].field..." 
				
			fieldNumber = fieldIdentPtr[fieldNumber].nextField;
			
			}	// end "while (fieldNumber != -1 && continueFlag)" 
			
		}	// end "if (classNamesPtr[classStorage].number ..." 
		
	if (!continueFlag)
		totalNumberPixels = -1;
	
			// Return the status of continueFlag.										
			
	return (totalNumberPixels);
		
}	// end "GetClassDataValues" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt32 GetFieldDataValues
//
//	Software purpose:	The purpose of this routine is to load the input
//							short integer vector with the data values for
//							the specified field stored in BIS format.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			GetClassDataValues in featureExtraction.c
//							StatisticsEnhance in statisticsEnhancement.c
//
//	Coded By:			Larry L. Biehl			Date: 11/02/1992
//	Revised By:			Larry L. Biehl			Date: 06/17/2006

SInt32 GetFieldDataValues (
				AreaDescriptionPtr				areaDescriptionPtr, 
				FileIOInstructionsPtr			fileIOInstructionsPtr, 
				UInt16*								channelsPtr, 
				SInt16								numberChannels, 
				HDoublePtr							dataValuePtr,   
				HDoublePtr							doubleDataValuePtr, 
				HDoublePtr							transformMatrixPtr,
				HDoublePtr							transformFeatureMeansPtr, 
				SInt16								numberFeatures,
				SInt16								optionsCode)

{
	AreaDescription					maskAreaDescription,
											projectAreaDescription;
	
	Point									point;
	
	HDoublePtr							bufferValuePtr;
	
	RgnHandle							rgnHandle;
	
	SInt32								returnCode;
										
	UInt32								bytesToMove,
											column,
											columnInterval,
											line,
											lineEnd,
											lineInterval,
											numberSamples,
											pixelCount,
											sample;
	
	SInt16								channel,
											errCode;
											
	Boolean								checkIfTrainSampleFlag,
											includeSampleFlag,
											polygonFieldFlag,
											transformDataFlag;
	
	
	if (areaDescriptionPtr->lineStart <= 0)
																								return (0);
																		
	ShowStatusDialogItemSet (kStatusCommand);
	
			// Initialize local variables.
			
	polygonFieldFlag = areaDescriptionPtr->polygonFieldFlag;
	rgnHandle = areaDescriptionPtr->rgnHandle;
	columnInterval = areaDescriptionPtr->columnInterval;
	lineEnd = areaDescriptionPtr->lineEnd;
	lineInterval = areaDescriptionPtr->lineInterval;
	pixelCount = 0;
	bytesToMove = numberChannels * 8;
	returnCode = 0;
		
	if (polygonFieldFlag && rgnHandle == NULL)
																								return (0);
	
	transformDataFlag = 
		(transformMatrixPtr != NULL && doubleDataValuePtr != NULL && numberFeatures > 0);

			// Initialize project area description in case it is needed.
		
	InitializeAreaDescription (&projectAreaDescription);
			
			// Load some of the File IO Instructions structure that pertain
			// to the specific area being used.
			
	errCode = SetUpFileIOInstructions (fileIOInstructionsPtr,
													areaDescriptionPtr, 
													numberChannels,
													channelsPtr,
													kDetermineSpecialBILFlag);
	
			// For now assume that all samples will be allowed. If call is
			// from statistics enhancement, the sample will need be be verified
			// that it is not a training sample.
											
	includeSampleFlag = TRUE;
	checkIfTrainSampleFlag = (optionsCode & kNoTrainingSamples);
	
			// If each pixel is to be checked to see whether it is a training sample,
			// then set up some mask variables if a training mask exists.
			
	SetUpMaskAreaDescriptionParameters (&maskAreaDescription, 
														areaDescriptionPtr);

			// Loop through lines for area to be classified.							

	for (line=areaDescriptionPtr->lineStart; line<=lineEnd; line+=lineInterval)
		{
				// Get all requested channels for line of image data.  Return 		
				// if there is a file IO error.												
				 
		errCode = GetLineOfData (fileIOInstructionsPtr,
											line, 
											areaDescriptionPtr->columnStart,
											areaDescriptionPtr->columnEnd,
											areaDescriptionPtr->columnInterval,
											gInputBufferPtr,
											gOutputBufferPtr);
								
		if (errCode < noErr)
			{
			returnCode = -1;
			break;											
			
			}	// end "if (errCode < noErr)"
			
		if (errCode != kSkipLine)
			{
			column = areaDescriptionPtr->columnStart;
						
			point.v = (SInt16)line;
			
			bufferValuePtr = (HDoublePtr)gOutputBufferPtr;
			
			numberSamples = fileIOInstructionsPtr->numberOutputBufferSamples;
			
					// Loop through the number of samples in the line of data			
			
			for (sample=1; sample<=numberSamples; sample++)
				{
				if (checkIfTrainSampleFlag)
							// The input field boundary is a user specified area. For this
							// option, the user does not want to use any samples that are 
							// being used for training. This option is used by the Statistics 
							// Enhancement processor.
							
					//#ifndef multispec_lin
						// TODO: For Linux, add when IsProjectData function has been defined
					includeSampleFlag = !IsProjectData (&maskAreaDescription,
																		&projectAreaDescription,
																		line, 
																		column);
					//#endif
				
				if (includeSampleFlag)
					{	
					point.h = (SInt16)column;
			
					if (!polygonFieldFlag || PtInRgn (point, rgnHandle))
						{
						if (transformDataFlag)
							{
							TransformDataVector (bufferValuePtr, 
														transformMatrixPtr,
														transformFeatureMeansPtr, 
														doubleDataValuePtr,
														numberChannels,
														numberFeatures);
														
							doubleDataValuePtr += numberFeatures;
							
							}	// end "if (transformDataFlag)"
							
						else	// !transformDataFlag
							{
							if (dataValuePtr != NULL)
								{
								BlockMoveData (bufferValuePtr, dataValuePtr, bytesToMove);
								dataValuePtr += numberChannels;
								
								}	// end "if (dataValuePtr != NULL)"
								
							if (doubleDataValuePtr != NULL)
								{
								for (channel=0; channel<numberChannels; channel++)
									doubleDataValuePtr[channel] = 
																	(double)bufferValuePtr[channel];
									
								doubleDataValuePtr += numberChannels;
									
								}	// end "if (doubleDataValuePtr != NULL)"
							
							}	// end "else !transformDataFlag"
							
						pixelCount++;
					   
					   }	// end "if (!polygonField || PtInRgn (point, rgnHandle))"
					   
					}	// end "if (includeSampleFlag)" 
				
				column += columnInterval;
				   
				bufferValuePtr += numberChannels;
						
				}	// end "for (sample=0; sample<..." 
				
			}	// end "if (errCode != kSkipLine)"
			
				// Exit routine if user has "command period" down					
				
		if (TickCount () >= gNextTime)
			{
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
				{
				returnCode = -1;
				break;											
				
				}	// end "if (!CheckSomeEvents (..."
				
			}	// end "if (TickCount () >= nextTime)" 
			
		if (fileIOInstructionsPtr->maskBufferPtr != NULL)
			fileIOInstructionsPtr->maskBufferPtr += 
												fileIOInstructionsPtr->numberMaskColumnsPerLine;
			
		}	// end "for (line=areaDescriptionPtr->lineStart..." 
		
			// Close up any File IO Instructions structure that pertain to the 
			// specific area used for obtaining the field data values.
			
	CloseUpFileIOInstructions (fileIOInstructionsPtr, areaDescriptionPtr);
	
			// Unlock handle related to any mask area that was used.
	
	if (maskAreaDescription.maskInfoPtr != NULL)
		{
		CheckAndUnlockHandle (maskAreaDescription.maskInfoPtr->maskHandle);
		maskAreaDescription.maskInfoPtr->maskPointer = NULL;
		
		}	// end "if (maskAreaDescription.maskInfoPtr != NULL)"
		
	if (returnCode == 0)
		returnCode = pixelCount;
		
	return (returnCode);
		
}	// end "GetFieldDataValues" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		double InvertLowerTriangularMatrix
//
//	Software purpose:	The purpose of this routine is to return the
//							determinant of a symmetric matrix in lower triangular
//							form and the inverse of the symmetric matrix in 
//							lower triangular form.
//
//	Parameters in:					
//
//	Parameters out:	The pivot values are returned in pivot.
//							The inverse of the matrix is returned in matrixPtr.
//							The rank of the matrix is returned in ipvot[0].
//
// Value Returned: 	The determinant of the matrix.
//
// Called By:			LoadSeparabilityStatistics in separability.c
//							behzad_ModifyStatistics in statisticsEnhancement.c
//							LoadStatEnhanceClassStatistics in statisticsEnhancement.c
//
//	Coded By:			Larry L. Biehl			Date: 11/29/1993
//	Revised By:			Larry L. Biehl			Date: 06/17/2006

Boolean InvertLowerTriangularMatrix (
				HDoublePtr							inputTriangularMatrixPtr, 
				HDoublePtr							outputTriangularMatrixPtr, 
				SInt16								numberFeatures, // Size of square matrix 
				double*								determinantPtr,
				double*								logDeterminantPtr, // Log Determinant of input matrix
				Boolean								inverseRequestedFlag) 

{
	Boolean								determinantOKFlag = FALSE;
	
	
	if (inputTriangularMatrixPtr != NULL)
		{
		CopyTriangleToSquareMatrix (numberFeatures, 
												inputTriangularMatrixPtr,
												gInverseMatrixMemory.inversePtr);
				
		determinantOKFlag = InvertSymmetricMatrix (gInverseMatrixMemory.inversePtr, 
																	numberFeatures, 
																	gInverseMatrixMemory.pivotPtr, 
																	gInverseMatrixMemory.indexRowPtr, 
																	gInverseMatrixMemory.indexColPtr, 
																	gInverseMatrixMemory.ipvotPtr,
																	determinantPtr, 
																	logDeterminantPtr,
																	inverseRequestedFlag);
		
		if (outputTriangularMatrixPtr != NULL &&
					inverseRequestedFlag &&
							determinantOKFlag)
			{														
					// Copy the full form (square) matrix back to triangular		
					// form storage. This takes less space and makes the 			
					// separability algorithm a little easier to implement 		
					// and quicker.															
			
			SquareToTriangularMatrix (gInverseMatrixMemory.inversePtr, 
												outputTriangularMatrixPtr, 
												numberFeatures);
												 		
			}	// end " outputTriangularMatrixPtr != NULL && ..." 
			
		}	// end "if (inputTriangularMatrixPtr != NULL)" 
		
	return (determinantOKFlag);

}	// end "InvertLowerTriangularMatrix" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		double InvertSymmetricMatrix
//
//	Software purpose:	The purpose of this routine is to return the
//							determinant of a symmetric matrix and the inverse 
//							of the symmetric matrix.
//
//	Parameters in:					
//
//	Parameters out:	The pivot values are returned in pivot.
//							The inverse of the matrix is returned in matrixPtr.
//							The rank of the matrix is returned in ipvot[0].
//
// Value Returned: 	The determinant of the matrix.
//
// Called By:			MaxLikeClsfierControl in classify.c
//							SetupClsfierStats in echo_classify.c
//							FS_decision_boundary in mul.c
//							FS_optimize_2_class in mul.c
//							EvaluateCovariancesControl in other.c
//							Bhattacharyya in separability.c
//
//       written ??/??/?? by ?
//	Coded By:			C.H. LEE					Date: 11/03/1988
//	Revised By:			Larry L. Biehl			Date: 06/17/2006

Boolean InvertSymmetricMatrix (
				HDoublePtr							matrixPtr, // Input: Input matrix; 
																		// Output: Inverted matrix 
				UInt16								matrixSize, // Size of square matrix 
				double*								pivot, 		// Work array 
				SInt16*								indexRow, 	// Work array 
				SInt16*								indexCol, 	// Work array 
				SInt16* 								ipvot, 		// Work array 
				double*								determinantPtr, // Determinant of input matrix
				double*								logDeterminantPtr, // Log Determinant of input matrix
				Boolean								inverseRequestedFlag)  // Request for inverse matrix

{
	double								determinant,  // Determinant of input matrix 
											logDeterminant,  // Log Determinant of input matrix 
				 							logMaxElement,
				 							maxElement,
				 							smallestPivot,
				 							temp;
	
	HDoublePtr							savedMatrixPtr2,
											tempMatrixPtr1,
											tempMatrixPtr2;
	
	UInt32								i,
											icol,
											index,
											irow,
											j,
											k;
											
	Boolean								determinantOKFlag,
											stopFlag;
	

	determinant = 1.0;
	logDeterminant = 0;
	determinantOKFlag = FALSE;
	stopFlag = FALSE;
	
			// Get smallest pivot value allowed.  It will be based on the number of 
			// unique digits in a double value. I.E. Max diagonal element in input 
			// matrix less the number of unique digits.
			
	maxElement = fabs (FindMaxDiagonalValueInSquareMatrix (matrixPtr, matrixSize));
	
	logMaxElement = 0;
	if (maxElement > 0)
		logMaxElement = log10 (maxElement);
	
	smallestPivot = ceil (logMaxElement) - DBL_DIG;
	
	smallestPivot = pow ((double)10, smallestPivot);

	for (i=0; i<matrixSize; i++)
		ipvot[i]=0;
		
   irow = 0;
   icol = 0;

	for (i=0; i<matrixSize; i++)
		{
				// search for largest element													
				
	  	temp = -1;
	  	tempMatrixPtr1 = tempMatrixPtr2 = matrixPtr;
	  	for (j=0; j<matrixSize; j++)
	  		{
	   	if (!ipvot[j])
	   		{
	   		tempMatrixPtr1 += j;
	       	for (k=j; k<matrixSize; k++)
	       		{
		 			if (!ipvot[k] && (temp<fabs (*tempMatrixPtr1)))
		 				{
		   			irow = j;
		   			icol = k;
		   			temp = fabs (*tempMatrixPtr1);
		   			
		 				}	// end "if (!ipvot[k] && (temp<fabs (..." 
		 			
		 			tempMatrixPtr1++;
		 				
		 			}	// end "for (k=j; k<matrixSize; k++)" 
		 			
		 		}	// end "if (!ipvot[j])" 
		 		
		 	else	// ipvot[j] 
		 		tempMatrixPtr1 += matrixSize;
		 		
		 	}	// end "for (j=0; j<matrixSize; j++)" 
		 				
		ipvot[icol]++;

				// The following statements put pivot element on the diagonal. 	

		index = icol*matrixSize;
		if (irow != icol)
			{
	  		determinant *= -1;
	   	tempMatrixPtr1 = &tempMatrixPtr2[irow*matrixSize];
	   	tempMatrixPtr2 = &tempMatrixPtr2[index];
		  	for (j=0; j<matrixSize; j++)
				{
				temp = *tempMatrixPtr1;
				*tempMatrixPtr1 = *tempMatrixPtr2;
				*tempMatrixPtr2 = temp;
				tempMatrixPtr1++;
				tempMatrixPtr2++;
				
				}	// end "for (j=0; j<matrixSize; j++)" 
				
			}	// end "if (irow != icol)" 
	
		indexRow[i] = (SInt16)irow;
		indexCol[i] = (SInt16)icol;
		temp = pivot[i] = matrixPtr[index+icol];
		if (temp >= smallestPivot)
			{                
	      logDeterminant += log (fabs (temp));
	      
			#if defined multispec_mac        
				determinant *= temp;
			#endif	// defined multispec_mac        
	      
         #if defined multispec_win || defined multispec_lin
				if (logDeterminant <= gMaximumNaturalLogValue)       
					determinant *= temp;
					
				else	// logDeterminant > gMaximumNaturalLogValue       
					determinant = DBL_MAX;
			#endif	// defined multispec_win || defined multispec_lin
		
					// The following statements divide pivot row by pivot element. 	
		
			matrixPtr[index+icol] = 1.0;
			tempMatrixPtr2 = savedMatrixPtr2 = &matrixPtr[index];
			temp = 1./temp;
			for (j=0; j<matrixSize; j++)
				tempMatrixPtr2[j] *= temp;
		
					// Reduce the matrix.    														
					
			tempMatrixPtr1 = matrixPtr;
			for (j=0; j<matrixSize; j++)
				{                 
				if (j != icol)
					{
					temp = tempMatrixPtr1[icol];
					tempMatrixPtr1[icol] = 0;
					
					tempMatrixPtr2 = &matrixPtr[j];
					index = ipvot[j];
					for (k=0; k<j; k++)
						{
						if ((SInt16)index != ipvot[k])
							*tempMatrixPtr1 = -*tempMatrixPtr2;
							
						else	// ipvot[j] == ipvot[k] 
							*tempMatrixPtr1 = *tempMatrixPtr2;
						
						tempMatrixPtr1++;
						tempMatrixPtr2 += matrixSize;
						
						}	// end "for (k=0; k<j; k++)" 
						
					tempMatrixPtr2 = savedMatrixPtr2;
					
					for (; k<matrixSize; k++)
						{               
						*tempMatrixPtr1 -= tempMatrixPtr2[k] * temp;
						tempMatrixPtr1++;
						
						}	// end "for (k=0; k<matrixSize; k++)" 
						
					}	// end "if (j != icol)" 
					
				else	// j == icol 
					tempMatrixPtr1 += matrixSize;
					
				}	// end "for (j=0; j<matrixSize; j++)" 
			
			//ListCovarianceMatrix (matrixPtr, matrixSize, 1);
			
			}	// end "if (temp >= smallestPivot)"
			
		else	// temp < smallestPivot)
			{
					// return rank of matrix, log determinat of less than the
					// smallest allowed and determinant of 0.						
					
			ipvot[0] = (SInt16)i;
			
			if (temp > 0) 
				logDeterminant += log (temp); 
			determinant *= temp;
			stopFlag = TRUE;
			
			}	// end "else temp < smallestPivot)" 
			
				// Exit routine if user has "command period" down					
		
		if (TickCount () >= gNextTime)
			{
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
				{
				determinant = 0;
				stopFlag = TRUE;
					
				}	// end "if (!CheckSomeEvents (..."
			
			}	// end "if (TickCount () >= gNextTime)"
			
		if (stopFlag) 
			break;
				
		}	// end "for (i=0; i<matrixSize; i++)" 

			// The following statements interchange columns to get the final
			// Inverse matrix. Only do this if the inverse matrix is requested. 							
	
	if (!stopFlag)
		{
		if (inverseRequestedFlag)
			{		
			for (j=matrixSize-1; j>= 0; j--)
				{              
				if (indexRow[j] != indexCol[j])
					{
					tempMatrixPtr1 = &matrixPtr [indexRow[j]];
					tempMatrixPtr2 = &matrixPtr [indexCol[j]];
					for (k=indexRow[j]; k<matrixSize; k++)
						{
			      	temp = *tempMatrixPtr1;
			      	*tempMatrixPtr1 = *tempMatrixPtr2;
			      	*tempMatrixPtr2 = temp;
			      	
			      	tempMatrixPtr1 += matrixSize;
			      	tempMatrixPtr2 += matrixSize;
			      	
			    		}	// end "for (k=indexRow[j]; k<matrixSize; k++)" 
			    		
			   	}	// end "if (indexRow[j] != indexCol[j])"
			   	
			   if (j == 0)
			   	break; 
			   	
			   }	// end "for (j=matrixSize-1; j>=0; j--)"
			   
			}	// end "if (inverseRequestedFlag)"
		   	
		ipvot[0] = matrixSize;
		
				// Determine if logDeterminant is greater than specified minimum value
				// for a valid inverse.  This can probably be improved upon (6/17/2006).
				// Currently this is based on log of max pivot value times the matrix
				// size less the user specified differential.
			
		FindMaxValueInVector (pivot, matrixSize, &temp);
		logMaxElement = log (temp) - fabs (gLowerDeterminantOffsetAllowed);
		
		if (logDeterminant >= logMaxElement * matrixSize)
			determinantOKFlag = TRUE;
			
		}	// end "if (!stopFlag)"
		
	if (determinantPtr != NULL)
		*determinantPtr = determinant;
	
	if (logDeterminantPtr != NULL)
		*logDeterminantPtr = logDeterminant;
	
	return (determinantOKFlag);
	
}	// end "InvertSymmetricMatrix" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ReleaseMatrixInversionMemory
//
//	Software purpose:	The purpose of this routine is to release the 
//							memory needed for matrix inversion.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			LoadBiPlotClassStats in biPlotData.c
//							FS_decision_boundary in mul.c
//							CalculateSeparabilityControl in separability.c
//							StatisticsEnhance in statisticsEnhancement.c
//
//	Coded By:			Larry L. Biehl			Date: 05/16/1994
//	Revised By:			Larry L. Biehl			Date: 05/16/1994	

void ReleaseMatrixInversionMemory (void)

{
	gInverseMatrixMemory.pivotPtr = CheckAndDisposePtr (
																gInverseMatrixMemory.pivotPtr);
	gInverseMatrixMemory.indexRowPtr = CheckAndDisposePtr (
																gInverseMatrixMemory.indexRowPtr);
	gInverseMatrixMemory.indexColPtr = CheckAndDisposePtr (
																gInverseMatrixMemory.indexColPtr);
	gInverseMatrixMemory.inversePtr = CheckAndDisposePtr (
																gInverseMatrixMemory.inversePtr);
	gInverseMatrixMemory.ipvotPtr = CheckAndDisposePtr (
																gInverseMatrixMemory.ipvotPtr);
		
}	// end "ReleaseMatrixInversionMemory" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean SetupMatrixInversionMemory
//
//	Software purpose:	The purpose of this routine is to setup the 
//							memory needed for matrix inversion.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			SetupClsfierMemory in classifier.c
//							FS_discriminant_analysis in mul.c
//							FS_decision_boundary in mul.c
//							EvaluateCovariancesControl in other.c
//							SetupSeparabilityStatMemory in separability.c
//							StatisticsEnhance in statisticsEnhancement.c
//
//	Coded By:			Larry L. Biehl			Date: 10/29/1990
//	Revised By:			Larry L. Biehl			Date: 02/12/2002	

Boolean SetupMatrixInversionMemory (
				UInt16								matrixSize, 
				Boolean								covarianceFlag,
				HDoublePtr*							inverseCovPtr, 
				HDoublePtr*							pivotPtr, 
				SInt16**								indexRowPtr, 
				SInt16**								indexColPtr, 
				SInt16**								ipvotPtr)

{
	UInt32								bytesNeeded;
	Boolean								continueFlag;
	
	
			// Initialize parameters															
			
	continueFlag = TRUE;
	
	if (inverseCovPtr)
		*inverseCovPtr = NULL;
	
	if (pivotPtr)
		*pivotPtr = NULL;
	
	if (indexRowPtr)
		*indexRowPtr = NULL;
	
	if (indexColPtr)
		*indexColPtr = NULL;
	
	if (ipvotPtr)
		*ipvotPtr = NULL;
		
	if (continueFlag && inverseCovPtr)
		{
					// Get pointer to memory to use for inverse matrix.				
		
		bytesNeeded = (UInt32)matrixSize * sizeof (double);
		if (covarianceFlag)
			bytesNeeded *= matrixSize;
			
		*inverseCovPtr = (HDoublePtr)MNewPointer (bytesNeeded);
		continueFlag = (*inverseCovPtr != NULL);
			
		}	// end "if (continueFlag && inverseCovPtr)" 
		
	if (continueFlag && pivotPtr)
		{
					// Get pointer to memory to use for work space for matrix		
					// inversion.																	
					
		bytesNeeded = (UInt32)(matrixSize+1) * sizeof (double);
		if (matrixSize == 1)
			bytesNeeded += bytesNeeded;
		*pivotPtr = (HDoublePtr)MNewPointer (bytesNeeded);
		continueFlag = (*pivotPtr != NULL);
			
		}	// end "if (continueFlag && pivotPtr)" 
		
	if (continueFlag && indexColPtr)
		{
					// Get pointer to memory to use for work space for matrix		
					// inversion.																	
					
		bytesNeeded = (UInt32)matrixSize * sizeof (SInt32);
		*indexColPtr = (SInt16*)MNewPointer (bytesNeeded);
		continueFlag = (*indexColPtr != NULL);
			
		}	// end "if (continueFlag && indexColPtr)" 
		
	if (continueFlag && indexRowPtr)
		{
					// Get pointer to memory to use for work space for matrix		
					// inversion.																	
					
		bytesNeeded = (UInt32)(matrixSize+1) * sizeof (SInt32);
		*indexRowPtr = (SInt16*)MNewPointer (bytesNeeded);
		continueFlag = (*indexRowPtr != NULL);
			
		}	// end "if (continueFlag && indexRowPtr)" 
		
	if (continueFlag && ipvotPtr)
		{
					// Get pointer to memory to use for work space for matrix		
					// inversion.																	
					
		bytesNeeded = (UInt32)(matrixSize+1) * sizeof (SInt32);
		*ipvotPtr = (SInt16*)MNewPointer (bytesNeeded);
		continueFlag = (*ipvotPtr != NULL);
			
		}	// end "if (continueFlag && ipvotPtr)" 
		
	return (continueFlag);
		
}	// end "SetupMatrixInversionMemory" 


