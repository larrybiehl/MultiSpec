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
//	File:						SProjectListStatistics.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			02/27/2018
//
//	Language:				C
//
//	System:					Linux, Macintosh and Windows Operating Systems
//
//	Brief description:	This file contains functions that list field and class 
//								statistics.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h" 
 
#if defined multispec_wx
	#include "xStatisticsListDialog.h"
#endif

#if defined multispec_mac || defined multispec_mac_swift
	#define IDC_Fields  						4
	#define IDC_Classes  					5
	#define IDC_MeansStdDev					7
	#define IDC_CovarianceMatrix			8
	#define IDC_CorrelationMatrix			9
	#define IDC_UseTransformation			11  
	#define IDC_MeansStdDevPrompt			12 
	#define IDC_MeansStdDevPrecision		13
	#define IDC_CovariancePrompt			14 
	#define IDC_CovariancePrecision		15
#endif	// defined multispec_mac || defined multispec_mac_swift 
  
#if defined multispec_win  
	#include	"SMultiSpec.h" 
	#include "WStatisticsListDialog.h"
#endif	// defined multispec_win



		// Prototypes for routines in this file that are only called by
		// other routines in this file.													

SInt16	GetMeanStdDevLength (
				SInt16								numberChannels,
				HDoublePtr							meanVectorPtr, 
				HDoublePtr							stdDevVectorPtr);

SInt16	 GetVarianceLength (
				SInt16 								numberChannels,
				HDoublePtr							stdDevVectorPtr);

void 		ListClassStats (
				SInt16								numberChannels,
				SInt16								classNumber,
				HDoublePtr							meanVectorPtr,
				HDoublePtr							stdDevVectorPtr,
				HDoublePtr							minimumVectorPtr,
				HDoublePtr							maximumVectorPtr,
				HDoublePtr							classCovariancePtr,
				HDoublePtr							fieldCovariancePtr);

void 		ListFieldStats (
				SInt16								numberChannels,
				SInt16								classNumber, 
				SInt16								fieldNumber,
				HDoublePtr							meanVectorPtr,
				HDoublePtr							stdDevVectorPtr,
				HDoublePtr							minimumVectorPtr,
				HDoublePtr							maximumVectorPtr,
				HDoublePtr							fieldCovariancePtr);

Boolean 	ListLowerTriangularMatrix (
				HDoublePtr							matrixPtr,
				SInt16								numberChannels, 
				SInt16								fieldSize, 
				SInt16								precision);

void 		ListProjectStats (
				SInt16								numberChannels,
				HDoublePtr							meanVectorPtr,
				HDoublePtr							stdDevVectorPtr,
				HDoublePtr							minimumVectorPtr,
				HDoublePtr							maximumVectorPtr,
				HDoublePtr							classCovariancePtr,
				HDoublePtr							fieldCovariancePtr);

void 		ListStatistics (
				SInt16								numberChannels,
				HDoublePtr							meanVectorPtr,
				HDoublePtr							stdDevVectorPtr,
				HDoublePtr							minimumVectorPtr,
				HDoublePtr							maximumVectorPtr, 
				HCovarianceStatisticsPtr		covariancePtr,
				SInt64								numberPixels);

Boolean	ListStatsDialog (
				SInt16								statsWindowMode);


		// Declare globals that are local to this file.
		
char*			l_CharBufferPtr4 = NULL;
char*			l_CharBufferPtr5 = NULL;



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetMeanStdDevLength
//
//	Software purpose:	The purpose of this routine is to determine the
//							number of digits of the largest mean and standard 
//							deviation.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 04/05/1993
//	Revised By:			Larry L. Biehl			Date: 01/10/2006	

SInt16 GetMeanStdDevLength (
				SInt16								numberChannels, 
				HDoublePtr							meanVectorPtr, 
				HDoublePtr							stdDevVectorPtr)

{
	double								maxValue;
	
	SInt16								channel,
											maximumLength;
	
	
			// Determine length to use to list the means and standard deviations.																		
			
	maximumLength = 3;
	maxValue = -DBL_MAX;
	if (numberChannels > 0 && meanVectorPtr != NULL && stdDevVectorPtr != NULL)
		{
		for (channel=0; channel<numberChannels; channel++)
			{
			maxValue = MAX (maxValue, *meanVectorPtr);
			maxValue = MAX (maxValue, *stdDevVectorPtr);
			
			meanVectorPtr++;
			stdDevVectorPtr++;
											
			}	// end "for (channel=0; channel<numberChannels; channel++)"
			
		}	// end "if (numberChannels > 0 && ..." 
		
	if (maxValue >= kMinValueToListWith_f && maxValue <= kMaxValueToListWith_f)
		{
		maximumLength = GetNumberWholeDigits (maxValue);
		maximumLength = MAX (maximumLength, 3);
		
		}	// end "if (maxValue >= kMinValueToListWith_f && maxValue <= ..."
		
	else	// maxValue < kMinValueToListWith_f || ...
		{
		maximumLength = 7 + 3;		// 3 extra precision values.
		
		}	// end "else value < kMinValueToListWith_f || ..."
		
			// Allow for signed data values.
			
	maximumLength++;
	
			// Allow for decimal places.
		
	maximumLength += gProjectInfoPtr->listMeanStdPrecision + 1;
		
	return (maximumLength);

}	// end "GetMeanStdDevLength" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetVarianceLength
//
//	Software purpose:	The purpose of this routine is to determine the
//							number of digits of the largest variance.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 04/05/1993
//	Revised By:			Larry L. Biehl			Date: 01/10/2006	

SInt16 GetVarianceLength (
				SInt16								numberChannels, 
				HDoublePtr							stdDevVectorPtr)

{
	double								maxValue;
	
	UInt32								channel;
											
	UInt16								maximumLength;
	
	
			// Check input values.  Continue only if input parameters are within	
			// proper rangers																		
			
	maximumLength = 3;
	maxValue = -DBL_MAX;
	if (numberChannels > 0 && stdDevVectorPtr != NULL)  
		{
		for (channel=0; channel<(UInt32)numberChannels; channel++)
			{
			maxValue = MAX (maxValue, *stdDevVectorPtr);
			stdDevVectorPtr++;
											
			}	// end "for (channel=0; channel<numberChannels; channel++)" 
		
		maxValue *= maxValue;
		
		if (maxValue >= kMinValueToListWith_f && maxValue <= kMaxValueToListWith_f)
			{
			//NumToString (maxValue, gTextString3);
			maximumLength = GetNumberWholeDigits (maxValue);
			maximumLength = MAX (maximumLength, 3);
			
			}	// end "if (maxValue >= kMinValueToListWith_f && maxValue <= ..."
			
		else	// maxValue < kMinValueToListWith_f || ...
			{
			maximumLength = 7 + 3;		// 3 extra precision values.
			
			}	// end "else value < kMinValueToListWith_f || ..."
			
		}	// end "if (numberChannels > 0 && ..." 
		
			// Allow for signed data values.
			
	maximumLength++;
	
			// Allow for decimal places.
			
	maximumLength += gProjectInfoPtr->listCovCorPrecision + 1;
		
	return (maximumLength);

}	// end "GetVarianceLength" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ListClassInformation
//
//	Software purpose:	The purpose of this routine is to list the
//							a summary of the information for the class including
//							the class name, the number of training fields and 
//							the total number of pixels in the class.
//
//	Parameters in:		classStorage: index to the class within the
//									PClassNamesPtr structure which is to be listed.
//							numberStatisticsPixels: the current number of
//									training pixels in the class to be
//									listed.												
//
//	Parameters out:	None
//
// Value Returned:	TRUE if statistics data for the class can be listed
//							FALSE if not statistics data for the class can be listed.
// 
// Called By:			HistogramClassStats in SStatHistogram.cpp
//							ListClassStats
//
//	Coded By:			Larry L. Biehl			Date: 04/10/1990
//	Revised By:			Larry L. Biehl			Date: 08/23/2010	

Boolean ListClassInformation (
				SInt16								classStorage, 
				SInt64								numberStatisticsPixels)

{
	HPClassNamesPtr					classNamesPtr;
	
	SInt16 								covarianceStatsToUse,
											stringCode; 
	
	Boolean								computeCommonCovarianceFlag,
											continueFlag,
											returnFlag;
	
	

			// Check input variables.	
   returnFlag = TRUE;
		
	if (classStorage < 0)
																							return (FALSE);
																					
	if (gProcessorCode != kListStatsProcessor && 
											gProcessorCode != kHistogramStatsProcessor)
																							return (FALSE);
	
	classNamesPtr = gProjectInfoPtr->classNamesPtr;
	covarianceStatsToUse = classNamesPtr[classStorage].covarianceStatsToUse;
	continueFlag = TRUE;
	returnFlag = TRUE;
																					
			// List class name and number of training fields and number				
			// of training pixels.																
						
	if (gProcessorCode == kListStatsProcessor)
		stringCode = IDS_Project33;
		
	if (gProcessorCode == kHistogramStatsProcessor)
		stringCode = IDS_Project34;
			
	continueFlag = ListSpecifiedStringNumber (
											kProjectStrID,
											stringCode,
											NULL, 
											gOutputForce1Code,
											(unsigned char*)&classNamesPtr[classStorage].name,
											continueFlag);
	
	if (gProcessorCode == kListStatsProcessor)
		{
				// Determine if the specified statistics data exists for this class.
				// If not list a message indicating that they do not exist.
		
		returnFlag = DetermineIfSpecifiedStatisticsExist (
																	&classNamesPtr[classStorage],
																	gProjectInfoPtr->statisticsCode,
																	covarianceStatsToUse,
																	&computeCommonCovarianceFlag);
		
				// List the next line only if some statistics are to be listed.
																	
		if (gProjectInfoPtr->listMatrixType & 
											kListMeansStdDev+kListCovariance+kListCorrelation)
			{										
					// List the type of statistics to be list for this class or if they
					// do not exist, list a message indicating that they do not.
					
			stringCode = 0;
			if (returnFlag)
				{
						// Statistics do exist.
						
				if (gProjectInfoPtr->listMatrixType & kListTransformedStats)
							// Statistics to be transformed.
					continueFlag = ListSpecifiedStringNumber (
																		kProjectStrID,
																		IDS_Project57, 
																		(unsigned char*)gTextString, 
																		NULL, 
																		gOutputForce1Code,
																		continueFlag);
				
				else	// !(gProjectInfoPtr->listMatrixType & kListTransformedStats)
							// Statistics to be transformed.
					{
					sprintf ((char*)gTextString, "    ");
					continueFlag = OutputString (NULL, 
															(char*)gTextString,
															0, 
															gOutputForce1Code,
															continueFlag);
					
					}	// end "else !(gProjectInfoPtr->list..."
					
				if (covarianceStatsToUse == kOriginalStats)
					stringCode = IDS_Project51;
				
				else if (covarianceStatsToUse == kLeaveOneOutStats)
					{
					if (classNamesPtr[classStorage].mixingParameterCode ==
																						kComputedOptimum)
						stringCode = IDS_Project52;
						
					else if (classNamesPtr[classStorage].mixingParameterCode == kUserSet)
						stringCode = IDS_Project72;
						
					else if (classNamesPtr[classStorage].mixingParameterCode ==
																						kIdentityMatrix)
						stringCode = IDS_Project71;
					
					}	// end "else if (covarianceStatsToUse == kLeaveOneOutStats)"
					
				else if (covarianceStatsToUse == kEnhancedStats)
					stringCode = IDS_Project53;
					
				}	// end "if (returnFlag)"
					
			else	// !returnFlag
				{
						// Statistics do not exist.
						
				if (covarianceStatsToUse == kOriginalStats)
					stringCode = IDS_Project35;
				
				else if (covarianceStatsToUse == kLeaveOneOutStats)
					stringCode = IDS_Project56;
					
				else if (covarianceStatsToUse == kEnhancedStats)
					stringCode = IDS_Project36;
						
				}	// end "else !returnFlag"
			
			if (stringCode != 0)	
				continueFlag = ListSpecifiedStringNumber (kProjectStrID,
																		stringCode, 
																		(unsigned char*)gTextString, 
																		NULL, 
																		gOutputForce1Code,
																		continueFlag);
				
			}	// end "if (gProjectInfoPtr->listMatrixType & ..."
		
		if (covarianceStatsToUse == kLeaveOneOutStats)
			{
			if (classNamesPtr[classStorage].mixingParameterCode == kComputedOptimum)
				{
				if (classNamesPtr[classStorage].looCovarianceValue >= 0)
					continueFlag = ListSpecifiedStringNumber (
												kProjectStrID,
												IDS_Project55,  
												NULL,
												gOutputForce1Code,
												classNamesPtr[classStorage].looCovarianceValue,
												continueFlag);
				
				else	// classNamesPtr[classStorage].looCovarianceValue < 0
					continueFlag = ListSpecifiedStringNumber (kProjectStrID,
																			IDS_Project59, 
																			(unsigned char*)gTextString, 
																			NULL, 
																			gOutputForce1Code,
																			continueFlag);
				
				}	// end "if (...mixingParameterCode == kComputedOptimum)"
										
			else if (classNamesPtr[classStorage].mixingParameterCode == kUserSet)
				continueFlag = ListSpecifiedStringNumber (
												kProjectStrID,
												IDS_Project70,  
												NULL,
												gOutputForce1Code,
												classNamesPtr[classStorage].userMixingParameter,
												continueFlag);
			
			}	// end "if (covarianceStatsToUse == kLeaveOneOutStats)"
		
		}	// end "if (gProcessorCode == kListStatsProcessor && ..." 
	
	stringCode = 0;
	if (gProcessorCode == kHistogramStatsProcessor)
		{
		if (numberStatisticsPixels <= 0)
			stringCode = 1;
			
		else	// numberStatisticsPixels > 0 
			stringCode = 2;
			
		}	// end "if (gProcessorCode == kHistogramStatsProcessor)" 
	
	else	// gProcessorCode == kListStatsProcessor 
		{
		if (covarianceStatsToUse != kEnhancedStats)
			{
					// List the number of training fields and pixels for original and
					// leave-one-out statistics.
					
			stringCode = 2;
				
			}	// end "if (covarianceStatsToUse == kEnhancedStats)"
					
		}	// end "else gProcessorCode == kListStatsProcessor" 
	
	if (stringCode == 1)						
		continueFlag = ListSpecifiedStringNumber (
									kProjectStrID,
									IDS_Project37,
									NULL, 
									gOutputForce1Code, 
									(SInt32)classNamesPtr[classStorage].numberOfTrainFields, 
									continueFlag);
	
	if (stringCode == 2)						
		continueFlag = ListSpecifiedStringNumber (
												kProjectStrID,
												IDS_Project38,
												NULL, 
												gOutputForce1Code, 
												numberStatisticsPixels, 
												classNamesPtr[classStorage].numberOfTrainFields,
												continueFlag);
	
	if (!continueFlag)
		returnFlag = FALSE;
																					
	return (returnFlag);

}	// end "ListClassInformation" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ListClassStats
//
//	Software purpose:	The purpose of this routine is to list the
//							statistics for the given class into the output
//							window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			ListProjectStats
//							ListStatsControl
//
//	Coded By:			Larry L. Biehl			Date: 11/21/1988
//	Revised By:			Larry L. Biehl			Date: 03/25/2014	

void ListClassStats (
				SInt16								numberChannels, 
				SInt16								classNumber,
				HDoublePtr							meanVectorPtr,
				HDoublePtr							stdDevVectorPtr,
				HDoublePtr							minimumVectorPtr,
				HDoublePtr							maximumVectorPtr,
				HDoublePtr							classCovariancePtr,
				HDoublePtr							fieldCovariancePtr)

{
	HPClassNamesPtr					classNamesPtr;
	HPFieldIdentifiersPtr			fieldIdentPtr;
	
	SInt16								fieldNumber;
	UInt16								classStorage;
	
	Boolean								continueFlag;
	
	
			// Check input values.  Continue only if input parameters are within	
			// proper rangers																		
			
	continueFlag = TRUE;
	if (numberChannels <= 0)  
		continueFlag = FALSE;
		
	if (classNumber < 0 || 
							classNumber >= (SInt16)gProjectInfoPtr->numberStatisticsClasses)
		continueFlag = FALSE;
																						
	if (continueFlag)
		{
				// Force text selection to start from end of present text			
			
		ForceTextToEnd ();
		
				// Initialize local variables.												
						
		classStorage = gProjectInfoPtr->storageClass[classNumber];
		classNamesPtr = gProjectInfoPtr->classNamesPtr;
		fieldIdentPtr = gProjectInfoPtr->fieldIdentPtr;
		
				// List class statistics if requested										
			
		if (gProjectInfoPtr->listClassFlag)
			{
					// List class name and number of training fields and number		
					// of training pixels.														

			if (!ListClassInformation (
									classStorage, 
									classNamesPtr[classStorage].numberStatisticsPixels))
																									return;
			
					// List class statistics as long as the number of training		
					// pixels is more than 0.  No training pixels means that stats	
					// have not been computed for the class yet.							
					// OR																				
					// List enhanced class statistics if those are being				
					// requested.  The number of training pixels are not needed		
					// for the enhanced statistics.											
					// NOTE that 'ListClassInformation' returns FALSE if there is	
					// no statistics to be listed.	
					
			GetTransformedClassCovarianceMatrix (
													numberChannels,
													gTempChannelStatsPtr,
													(HCovarianceStatisticsPtr)classCovariancePtr,
													NULL, 
													classNumber,
													kTriangleOutputMatrix, 
													gProjectInfoPtr->statisticsCode,
													gTransformationMatrix.eigenVectorPtr,
													gTransformationMatrix.tempMatrixPtr,
													numberChannels);
			
					// Get the transformed class mean vector													
			
			GetTransformedMeanVector (gTempChannelStatsPtr, 
												meanVectorPtr,
												numberChannels,
												NULL,
												gTransformationMatrix.eigenVectorPtr,
												gTransformationMatrix.tempMatrixPtr,
												gTransformationMatrix.offsetVectorPtr,
												numberChannels);
							
					// Get the transformed class standard deviation vector													
			
			GetStdDevVectorFromCovariance (classCovariancePtr,
														numberChannels,
														kTriangleOutputMatrix, 
														gProjectInfoPtr->statisticsCode, 
														stdDevVectorPtr);
			
					// Get the minimum and maximum vectors if needed.	
					
			if (minimumVectorPtr != NULL)
				ReduceMinimumVector (gTempChannelStatsPtr, 
											minimumVectorPtr,
											numberChannels,
											NULL);
					
			if (maximumVectorPtr != NULL)
				ReduceMaximumVector (gTempChannelStatsPtr, 
											maximumVectorPtr,
											numberChannels,
											NULL);
												
			ListStatistics (numberChannels, 
									meanVectorPtr,
									stdDevVectorPtr,
									minimumVectorPtr,
									maximumVectorPtr,
									classCovariancePtr,
									classNamesPtr[classStorage].numberStatisticsPixels);
			
			}	// end "if (gProjectInfoPtr->listClassFlag)" 
		
				// List field statistics if requested										
				
		if (gProjectInfoPtr->listFieldFlag)
			{
					// Loop through the fields for the class								
		
			fieldNumber = classNamesPtr[classStorage].firstFieldNumber;
		
			while (fieldNumber != -1)
				{
						// List the field statistics if the field is a training		
					
				if (fieldIdentPtr[fieldNumber].fieldType == kTrainingType)
					ListFieldStats (numberChannels, 
											classNumber, 
											fieldNumber,
											meanVectorPtr,
											stdDevVectorPtr,
											minimumVectorPtr,
											maximumVectorPtr,
											fieldCovariancePtr);
				
				fieldNumber = fieldIdentPtr[fieldNumber].nextField;
			
				}	// end "while (fieldNumber != -1)" 
				
			}	// end "if (gProjectInfoPtr->listFieldFlag)" 
			
		}	// end "if (continueFlag)" 
		
}	// end "ListClassStats" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ListFieldInformation
//
//	Software purpose:	The purpose of this routine is to list the
//							a summary of the information for the field including
//							the class and field names, the number of pixels in 
//							the field and the line-column boundaries of the field.
//
//	Parameters in:		classStorage: index location for class information 
//
//							fieldNumber: index location for field information
//
//							listCode:  	=1, statistics report
//											=2, histogram report
//											=3, cluster report
//
//							numberPixels: number of pixels in the field that was
//															used for the report
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			HistogramFieldStats in SStatHistogram.cpp
//							ListFieldStats
//							ClusterClassification in SCluster.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/10/1990
//	Revised By:			Larry L. Biehl			Date: 07/08/2019

Boolean ListFieldInformation (
				SInt16								fieldNumber, 
				SInt64								numberPixels, 
				SInt64								numberPixelsUsedForStats, 
				SInt32								lineInterval, 
				SInt32								columnInterval, 
				SInt16								listCode)

{
	HPFieldIdentifiersPtr			fieldIdentPtr;
	HPFieldPointsPtr					fieldPointsPtr;
	char*									stringPtr;
	
	SInt16								classStorage,
											point,
											pointIndex,
											strLength;
											
	Boolean								continueFlag;
	

			// Check input variables.															
			
	if (fieldNumber < 0)
																						return (FALSE);
	if (listCode < 1 || listCode > 3)
																						return (FALSE);
																					
			// Initialize local variables.													
	
	continueFlag = TRUE;
	fieldPointsPtr = gProjectInfoPtr->fieldPointsPtr;
	fieldIdentPtr = &gProjectInfoPtr->fieldIdentPtr[fieldNumber];
	classStorage = fieldIdentPtr->classStorage;
	if (classStorage < 0)
																						return (FALSE);
	
			// List class name and field name.												
	 
	pstr ((char*)gTextString2,
			(char*)&gProjectInfoPtr->classNamesPtr[classStorage].name, 
			&strLength);
	pstr ((char*)gTextString3,
			(char*)&fieldIdentPtr->name, 
			&strLength);
							
	sprintf ((char*)gTextString,
					"    Class '%s', Field '%s' ",
					(char*)gTextString2,
					(char*)gTextString3);
							
	if (listCode == 1)
		stringPtr = strcat ((char*)gTextString, "Statistics");
							
	else if (listCode == 2)
		stringPtr = strcat ((char*)gTextString, "Histogram");
							
	else if (listCode == 3)
		stringPtr = strcat ((char*)gTextString, "Clusters");
	
	stringPtr = strcat ((char*)gTextString, (char*)&gEndOfLine);
				
	continueFlag = OutputString ((CMFileStream*)NULL, 
											(char*)gTextString, 
											0, 
											gOutputForce1Code, 
											continueFlag);
	
			// List field coordinates and number of points.								

	pointIndex = fieldIdentPtr->firstPolygonPoint;
	
			// List information dependent upon the type of the field and the list code.	
	
	if (fieldIdentPtr->pointType == kMaskType && listCode >= 3)
		{
		sprintf ((char*)gTextString,
					"     Lines %d to %d by %d.  Columns %d to %d by %d.",
					(int)gAreaDescription.lineStart,
					(int)gAreaDescription.lineEnd,
					(int)lineInterval,
					(int)gAreaDescription.columnStart,
					(int)gAreaDescription.columnEnd,
					(int)columnInterval);
		
		continueFlag = OutputString ((CMFileStream*)NULL, 
											(char*)gTextString, 
											0, 
											gOutputForce1Code, 
											continueFlag);
		/*
		continueFlag = ListLineColumnIntervalString (NULL,
																	gOutputForce1Code,
																	gAreaDescription.lineStart, 
																	gAreaDescription.lineEnd, 
																	lineInterval,
																	gAreaDescription.columnStart, 
																	gAreaDescription.columnEnd, 
																	columnInterval, 
									 								continueFlag);
		*/
		}	// end "if (fieldIdentPtr->pointType == kMaskType && listCode >= 3)"				
	
	else if (fieldIdentPtr->pointType == kRectangleType || listCode >= 3)
		{
		sprintf ((char*)gTextString,
					"     Lines %d to %d by %d.  Columns %d to %d by %d.",
					(int)fieldPointsPtr[pointIndex].line,
					(int)fieldPointsPtr[pointIndex+1].line,
					(int)lineInterval,
					(int)fieldPointsPtr[pointIndex].col,
					(int)fieldPointsPtr[pointIndex+1].col,
					(int)columnInterval);
		
		continueFlag = OutputString ((CMFileStream*)NULL, 
											(char*)gTextString, 
											0, 
											gOutputForce1Code, 
											continueFlag);
		/*
				// Format not quite right in the function below. Needs to be made more
				// general.
		continueFlag = ListLineColumnIntervalString (NULL,
																	gOutputForce1Code,
																	fieldPointsPtr[pointIndex].line, 
																	fieldPointsPtr[pointIndex+1].line, 
																	lineInterval,
																	fieldPointsPtr[pointIndex].col, 
																	fieldPointsPtr[pointIndex+1].col, 
																	columnInterval, 
									 								continueFlag);
		*/
		}	// end "if (fieldIdentPtr->..." 
		
	else if (fieldIdentPtr->pointType == kPolygonType && listCode == 1)
		{		
				// field is polygon type && listCode is for statistics list.
				// Allow for the first two points which represent the bounding
				// rectangle.
				
		pointIndex += 2; 
				
		for (point=0; 
				point<fieldIdentPtr->numberOfPolygonPoints;
					point++)
			{
			sprintf ((char*)gTextString,
						"     Line %d, Column %d%s",
						(int)fieldPointsPtr[pointIndex].line,
						(int)fieldPointsPtr[pointIndex].col,
						gEndOfLine);
			
			continueFlag = OutputString ((CMFileStream*)NULL, 
													(char*)gTextString,
													0, 
													gOutputForce1Code, 
													continueFlag);
			pointIndex++;
			
			if (!continueFlag)
				break;
			
			}	// end "for (i=0; i<fieldIdentPtr"
			
		sprintf ((char*)gTextString, "      ");
			
		continueFlag = OutputString ((CMFileStream*)NULL,
												(char*)gTextString,
												0,
												gOutputForce1Code,
												continueFlag);
		
		}	// end "else if (fieldIdentPtr->pointType ..." 
	
	else if (fieldIdentPtr->pointType == kClusterType && listCode == 1)
		{
				// Field is cluster type && listCode is for statistics histogram list.
				
		sprintf ((char*)gTextString, "     Cluster defined.");
				
		continueFlag = OutputString ((CMFileStream*)NULL, 
												(char*)gTextString,
												0, 
												gOutputForce1Code, 
												continueFlag);
		
		}	// end "else if (fieldIdentPtr->..."  
	
	else if (fieldIdentPtr->pointType == kMaskType && listCode == 1)
		{
				// Field is cluster type && listCode is for statistics 				
				// histogram list. 																
				
		sprintf ((char*)gTextString, "     Mask defined.");
				
		continueFlag = OutputString ((CMFileStream*)NULL, 
												(char*)gTextString,
												0, 
												gOutputForce1Code, 
												continueFlag);
		
		}	// end "else if (fieldIdentPtr->..." 
	
	else if (fieldIdentPtr->pointType == kClusterType && listCode == 2)
		{
				// Field is cluster type && listCode is for statistics 				
				// histogram list. 																
				
		sprintf ((char*)gTextString, "     Cluster field; cannot histogram.");
				
		continueFlag = OutputString ((CMFileStream*)NULL, 
												(char*)gTextString,
												0, 
												gOutputForce1Code, 
												continueFlag);
		
		}	// end "else if (fieldIdentPtr->..." 
	
			// Include the number of pixels in the field if it exists.				
			
	gTextString[0] = 0;
	strLength = 0;
	if (numberPixels > 0)
		{
		strLength = sprintf ((char*)gTextString,
									" Number of samples = %lld",
									numberPixels);
		
		sprintf ((char*)&gTextString[strLength],
					" Number of samples used for statistics = %lld",
					numberPixelsUsedForStats);
		
		}	// end "if (numberPixels > 0)"
																				
			// Append a carriage return to the end of the line.
	
	stringPtr = strcat ((char*)gTextString, (char*)&gEndOfLine);
	
	if (!fieldIdentPtr->statsUpToDate)
		{
		stringPtr = strcat ((char*)gTextString, (char*)&gEndOfLine);
		stringPtr = strcat ((char*)gTextString, (char*)&gEndOfLine);
		
		}	// end "if (!fieldIdentPtr->statsUpToDate)"
				
	continueFlag = OutputString ((CMFileStream*)NULL, 
											(char*)gTextString, 
											0, 
											gOutputForce1Code, 
											continueFlag);
	
			// Check if statistics will be listed for the field.
			
	if (listCode == 1 && fieldIdentPtr->statsUpToDate)
		{
				// List the type of statistics to be listed (always orginal statistics
				// for field listings); however, they may be transformed.
							
		if (gProjectInfoPtr->listMatrixType & kListTransformedStats)
					// Statistics to be transformed.
			continueFlag = ListSpecifiedStringNumber (kProjectStrID,
																	IDS_Project57, 
																	(unsigned char*)gTextString, 
																	NULL, 
																	gOutputForce1Code,
																	continueFlag);
		
		else	// !(gProjectInfoPtr->listMatrixType & kListTransformedStats)
					// Statistics to be transformed.
			{
			sprintf ((char*)gTextString, "    ");
			continueFlag = OutputString (NULL, 
													(char*)gTextString,
													0, 
													gOutputForce1Code,
													continueFlag); 
														
			}	// end "else !(gProjectInfoPtr->..."
					
		continueFlag = ListSpecifiedStringNumber (kProjectStrID,
																IDS_Project50, 
																(unsigned char*)gTextString, 
																NULL, 
																gOutputForce1Code,
																continueFlag);
		
		}	// end "if (listCode == 1 && fieldIdentPtr->statsUpToDate)"
		
	return (continueFlag);

}	// end "ListFieldInformation" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ListFieldStats
//
//	Software purpose:	The purpose of this routine is to list the statistics for the
//							specified field.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 11/20/1988
//	Revised By:			Larry L. Biehl			Date: 07/08/2019

void ListFieldStats (
				SInt16								numberChannels, 
				SInt16								classNumber, 
				SInt16								fieldNumber,
				HDoublePtr							meanVectorPtr,
				HDoublePtr							stdDevVectorPtr,
				HDoublePtr							minimumVectorPtr,
				HDoublePtr							maximumVectorPtr,
				HDoublePtr							fieldCovariancePtr)

{
	HChannelStatisticsPtr			fieldChanPtr;
	HCovarianceStatisticsPtr		fieldSumSquaresPtr;
	HPFieldIdentifiersPtr			fieldIdentPtr;
	
	UInt32								fieldStatsNumber;
	
	Boolean								continueFlag;
	
	
			// Check input values.  Continue only if input parameters are within	
			// proper ranges, the field statistics are to be listed, and the		
			// field statistics are up to date.												
			// Also do not try to list field statistics if only the class			
			// statistics are saved.															
			
	continueFlag = TRUE;
				
	if (!gProjectInfoPtr->listFieldFlag)	 
		continueFlag = FALSE;
	
	if (numberChannels <= 0)  
		continueFlag = FALSE;
		
	if (fieldNumber < 0 || fieldNumber >= gProjectInfoPtr->numberStorageFields)
		continueFlag = FALSE;
		
	if (classNumber < 0 ||
						classNumber >= (SInt16)gProjectInfoPtr->numberStatisticsClasses)
		continueFlag = FALSE;
	/*
	if (gProjectInfoPtr->keepClassStatsOnlyFlag)
		continueFlag = FALSE;
	*/
	if (continueFlag)
		{
				// Force text selection to start from end of present text			
			
		ForceTextToEnd ();
		
				// Initialize local variables.												
						
		fieldIdentPtr = gProjectInfoPtr->fieldIdentPtr;
		
				// List class name and field name.											
				
		if (!ListFieldInformation (fieldNumber, 
											fieldIdentPtr[fieldNumber].numberPixels,
											fieldIdentPtr[fieldNumber].numberPixelsUsedForStats, 
											1, 
											1, 
											1))
																									return;
	
				// Get field statistics set number.											
			
		fieldStatsNumber = fieldIdentPtr[fieldNumber].trainingStatsNumber;
			
				// List field statistics as long as the stats for the field are
				// up to date.												
					
		if (fieldIdentPtr[fieldNumber].statsUpToDate)
			{
					// List the channel means, standard deviations, covariance		
					// matrix, and correlation matrix for the field						
				
			GetProjectStatisticsPointers (kFieldStatsOnly, 
													fieldStatsNumber,
													&fieldChanPtr, 
													&fieldSumSquaresPtr,
													NULL,
													NULL);

			GetTransformedCovarianceMatrix (numberChannels,
														fieldCovariancePtr,
														numberChannels, 
														NULL, 
														fieldChanPtr, 
														fieldSumSquaresPtr, 
														fieldIdentPtr[fieldNumber].numberPixels,
														kTriangleOutputMatrix,
														gTransformationMatrix.eigenVectorPtr,
														gTransformationMatrix.tempMatrixPtr,
														numberChannels);
			
					// Get the transformed field mean vector													
			
			GetTransformedMeanVector (fieldChanPtr, 
												meanVectorPtr,
												numberChannels,
												NULL,
												gTransformationMatrix.eigenVectorPtr,
												gTransformationMatrix.tempMatrixPtr,
												gTransformationMatrix.offsetVectorPtr,
												numberChannels);
							
					// Get the transformed field standard deviation vector
					
			GetStdDevVectorFromCovariance (fieldCovariancePtr,
														numberChannels,
														kTriangleOutputMatrix, 
														gProjectInfoPtr->statisticsCode, 
														stdDevVectorPtr);
			
					// Get the minimum and maximum vectors if needed.	
					
			if (minimumVectorPtr != NULL)
				ReduceMinimumVector (fieldChanPtr,
											//gTempChannelStatsPtr,
											minimumVectorPtr,
											numberChannels,
											NULL);
					
			if (maximumVectorPtr != NULL)
				ReduceMaximumVector (fieldChanPtr,
											//gTempChannelStatsPtr,
											maximumVectorPtr,
											numberChannels,
											NULL);
			
			ListStatistics (numberChannels, 
									meanVectorPtr,
									stdDevVectorPtr,
									minimumVectorPtr,
									maximumVectorPtr, 
									fieldCovariancePtr,
									fieldIdentPtr[fieldNumber].numberPixels);
			
			}	// end "if (fieldIdentPtr[fieldNumber]..." 
		
		}	// end "if (continueFlag)" 
		
}	// end "ListFieldStats" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ListLowerTriangularMatrix
//
//	Software purpose:	The purpose of this routine is to list the input
//							triangular matrix in lower triangular form.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/21/1988
//	Revised By:			Larry L. Biehl			Date: 02/27/2018

Boolean ListLowerTriangularMatrix (
				HDoublePtr							matrixPtr, 
				SInt16								numberChannels, 
				SInt16								fieldSize, 
				SInt16								precision)

{
	Ptr									stringPtr1;
	
	SInt32								stringLength;
	
	SInt16								channel,
											covChan,
											stringSkip;
	
		
	stringPtr1 = gCharBufferPtr1;
	sprintf (gCharBufferPtr1, "    Channel");
	stringPtr1 += 11;
	
	stringSkip = fieldSize + 1;
	
	for (channel=0; channel<numberChannels; channel++)
		{
		sprintf (stringPtr1,
					"\t%*d",
					fieldSize,
					gProjectInfoPtr->channelsPtr[channel]+1);
		stringPtr1 += stringSkip;
			
		}	// end "for (channel=0; channel<numberChannels; ..." 
				
	sprintf (stringPtr1, "%s", gEndOfLine);
	stringLength = (SInt32)((Ptr)stringPtr1 - (Ptr)gCharBufferPtr1 + 
																		gNumberOfEndOfLineCharacters);
	
	if (!ListString (gCharBufferPtr1,  stringLength,  gOutputTextH))
																						return (FALSE);
			
	for (channel=0; channel<numberChannels; channel++)
		{
		stringPtr1 = gCharBufferPtr1;
		sprintf (gCharBufferPtr1, "    %7d", gProjectInfoPtr->channelsPtr[channel]+1);
		stringPtr1 += 11;
      		
		for (covChan=0; covChan<=channel; covChan++)
			{
			LoadRealValueString (stringPtr1,
										*matrixPtr,
										fieldSize,
										precision,
										precision+3,
										(char*)"\t",
										(char*)"");
												
			stringPtr1 += stringSkip;
			matrixPtr++;
			
			}	// end "for (covChan=0; ..." 
			
		sprintf (stringPtr1, "%s", gEndOfLine);
		stringLength = (SInt32)((Ptr)stringPtr1 - (Ptr)gCharBufferPtr1 + 
																	gNumberOfEndOfLineCharacters);
		
		if (!ListString (gCharBufferPtr1, stringLength, gOutputTextH))
																						return (FALSE);
			
		}	// end "for (channel=0; channel<numberChannels..." 
		
	sprintf (gCharBufferPtr1, "%s", gEndOfLine);
	if (!ListString (gCharBufferPtr1, 
							gNumberOfEndOfLineCharacters, 
							gOutputTextH))
																						return (FALSE);
																					
	return (TRUE);

}	// end "ListLowerTriangularMatrix" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ListProjectStats
//
//	Software purpose:	The purpose of this routine is to list the
//							statistics for the project into the output window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/21/1988
//	Revised By:			Larry L. Biehl			Date: 03/24/2014	

void ListProjectStats (
				SInt16								numberChannels,
				HDoublePtr							meanVectorPtr,
				HDoublePtr							stdDevVectorPtr,
				HDoublePtr							minimumVectorPtr,
				HDoublePtr							maximumVectorPtr,
				HDoublePtr							classCovariancePtr,
				HDoublePtr							fieldCovariancePtr)

{
	UInt16								classIndex;
	
	Boolean								continueFlag;
	
	
			// Check input values.  Continue only if input parameters are within	
			// proper ranges and statistics are up to date.								
			
	continueFlag = TRUE;
	if (numberChannels <= 0)  
		continueFlag = FALSE;
		
	if (gProjectInfoPtr->numberStatisticsClasses <= 0)  
		continueFlag = FALSE;
	
	if (continueFlag)
		{
		for (classIndex=0; 
				classIndex < gProjectInfoPtr->numberStatisticsClasses; 
					classIndex++)
			ListClassStats (numberChannels, 
									classIndex,
									meanVectorPtr,
									stdDevVectorPtr,
									minimumVectorPtr,
									maximumVectorPtr,
									classCovariancePtr,
									fieldCovariancePtr);
			
		}	// end "if (continueFlag)" 
		
}	// end "ListProjectStats" 




//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ListStatistics
//
//	Software purpose:	The purpose of this routine is to list the
//							mean, standard deviation, correlation matrix, and
//							covariance matrix.  This routine is used for both
//							class statistics and field statistics.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			ListClassStats
//							ListFieldStats
//
//	Coded By:			Larry L. Biehl			Date: 11/21/1988
//	Revised By:			Larry L. Biehl			Date: 03/25/2014	

void ListStatistics (
				SInt16								numberChannels, 
				HDoublePtr							meanVectorPtr,
				HDoublePtr							stdDevVectorPtr,
				HDoublePtr							minimumVectorPtr,
				HDoublePtr							maximumVectorPtr, 
				HCovarianceStatisticsPtr		covariancePtr,
				SInt64								numberPixels)

{
	HDoublePtr							lStdDevVectorPtr;
	
	Ptr									stringPtr1,
											stringPtr2,
											stringPtr3,
											stringPtr4,
											stringPtr5;
	
	SInt32								stringLength;
	
	SInt16								channel,
											fieldLength,
											numberLength,
											numberPrecision;
	
	Boolean								continueFlag,
											minMaxFlag;
	
	
			// Check input values.  Continue only if input parameters are within	
			// proper rangers																		
			
	numberLength = -1;
	continueFlag = (numberChannels > 0);
	
	if ((gProjectInfoPtr->listMatrixType & kListCovariance+kListCorrelation) && 
																				covariancePtr == NULL)
		continueFlag = FALSE;
		
	if (numberPixels <= 0)
		continueFlag = FALSE;
																						
	if (continueFlag && (gProjectInfoPtr->listMatrixType & kListMeansStdDev))
		{
				// List channel means and standard deviations
				
		continueFlag = (meanVectorPtr != NULL);
		minMaxFlag = (minimumVectorPtr != NULL && maximumVectorPtr != NULL);
		
		if (continueFlag)
			{			
					//	Get the maximum length of the number to allow for.
				
			numberLength = 
					GetMeanStdDevLength (numberChannels, meanVectorPtr, stdDevVectorPtr);
		
			continueFlag = (numberLength > 0 && 
					numberLength < (SInt16)(13 + gProjectInfoPtr->listMeanStdPrecision));
			
			}	// end "if (continueFlag)"
																						
		if (continueFlag)
			{		
			stringPtr1 = gCharBufferPtr1;
			stringPtr2 = gCharBufferPtr2;
			stringPtr3 = gCharBufferPtr3;
			stringPtr4 = l_CharBufferPtr4;
			stringPtr5 = l_CharBufferPtr5;
			
			sprintf (gCharBufferPtr1, "    Channel            ");
			sprintf (gCharBufferPtr2, "    Mean               ");
			sprintf (gCharBufferPtr3, "    Standard Deviation ");
			
			stringPtr1 += 23;
			stringPtr2 += 23;
			stringPtr3 += 23;
			
			if (minMaxFlag)
				{
				sprintf (l_CharBufferPtr4, "    Minimum            ");
				sprintf (l_CharBufferPtr5, "    Maximum            ");
				
				stringPtr4 += 23;
				stringPtr5 += 23;
				
				}	// end "if (minimumVectorPtr != NULL && maximumVectorPtr != NULL)"
			
			fieldLength = numberLength + 1;
			lStdDevVectorPtr = stdDevVectorPtr;
			numberPrecision = gProjectInfoPtr->listMeanStdPrecision;
			for (channel=0; channel<numberChannels; channel++)
				{
				sprintf (stringPtr1, 
							"\t%*d",
							numberLength, 
							gProjectInfoPtr->channelsPtr[channel]+1);
							
				LoadRealValueString (stringPtr2,
											*meanVectorPtr,
											numberLength,
											numberPrecision,
											numberPrecision+3,
											(char*)"\t",
											(char*)"");
								
				LoadRealValueString (stringPtr3,
											*lStdDevVectorPtr,
											numberLength,
											numberPrecision,
											numberPrecision+3,
											(char*)"\t",
											(char*)"");
			
				stringPtr1 += fieldLength;
				stringPtr2 += fieldLength;
				stringPtr3 += fieldLength;
				
				meanVectorPtr++;
				lStdDevVectorPtr++;
				
				if (minMaxFlag)
					{
					LoadRealValueString (stringPtr4,
												*minimumVectorPtr,
												numberLength,
												numberPrecision,
												numberPrecision+3,
												(char*)"\t",
												(char*)"");
									
					LoadRealValueString (stringPtr5,
												*maximumVectorPtr,
												numberLength,
												numberPrecision,
												numberPrecision+3,
												(char*)"\t",
												(char*)"");
													
					stringPtr4 += fieldLength;
					stringPtr5 += fieldLength;
					
					minimumVectorPtr++;
					maximumVectorPtr++;
						
					}	// end "if (minMaxFlag)"
												
				}	// end "for (channel=0; channel<numberChannels; channel++)" 
			
			sprintf (stringPtr1, "%s", gEndOfLine);
			stringLength = (SInt32)((Ptr)stringPtr1 - (Ptr)gCharBufferPtr1 + 
																	gNumberOfEndOfLineCharacters);
			if (!ListString (gCharBufferPtr1, stringLength, gOutputTextH))
																									return;
																								
			sprintf (stringPtr2, "%s", gEndOfLine);		
			stringLength = (SInt32)((Ptr)stringPtr2 - (Ptr)gCharBufferPtr2 + 
																	gNumberOfEndOfLineCharacters);
			if (!ListString (gCharBufferPtr2, stringLength, gOutputTextH))
																									return;
			
			if (minMaxFlag)
				{
				sprintf (stringPtr3, "%s", gEndOfLine);	
				stringLength = (SInt32)((Ptr)stringPtr3 - (Ptr)gCharBufferPtr3 + 
																	gNumberOfEndOfLineCharacters);
				if (!ListString (gCharBufferPtr3, stringLength, gOutputTextH))
																								return;
																								
				sprintf (stringPtr4, "%s", gEndOfLine);	
				stringLength = (SInt32)((Ptr)stringPtr4 - (Ptr)l_CharBufferPtr4 + 
																	gNumberOfEndOfLineCharacters);
				if (!ListString (l_CharBufferPtr4, stringLength, gOutputTextH))
																								return;
																								
				sprintf (stringPtr5, "%s%s", gEndOfLine, gEndOfLine);
				stringLength = (SInt32)((Ptr)stringPtr5 - (Ptr)l_CharBufferPtr5 + 
																	2*gNumberOfEndOfLineCharacters);
				if (!ListString (l_CharBufferPtr5, stringLength, gOutputTextH))
																								return;
				
				}	// end "if (minMaxFlag)"
				
			else	// !minMaxFlag
				{
				sprintf (stringPtr3, "%s%s", gEndOfLine, gEndOfLine);
				stringLength = (SInt32)((Ptr)stringPtr3 - (Ptr)gCharBufferPtr3 + 
																	2*gNumberOfEndOfLineCharacters);
				if (!ListString (gCharBufferPtr3, stringLength, gOutputTextH))
																								return;
																								
				}	// end "else !minMaxFlag"
			
			}	// end "if (continueFlag)"
																							
		}	// end "if (continueFlag && (...->listMatrixType & kListMeansStdDev))"
		
	if (continueFlag && numberPixels > 0)
		{
					// List the channel covariance statistics if requested.			
		
		if (gProjectInfoPtr->listMatrixType & kListCovariance)
			{
			sprintf (gCharBufferPtr1, "    Covariance Matrix%s", gEndOfLine);
			if (!ListString (gCharBufferPtr1,
									(UInt32)strlen (gCharBufferPtr1),  
									gOutputTextH))
																									return;
																							
			numberLength = GetVarianceLength (numberChannels, stdDevVectorPtr);
			
			if (numberLength > 0 && 
						numberLength < (SInt16)(13 + gProjectInfoPtr->listCovCorPrecision))
				{																				
				if (!ListLowerTriangularMatrix (covariancePtr,
															numberChannels, 
															numberLength, 
															gProjectInfoPtr->listCovCorPrecision))
																									return;
																							
				}	// end "if (numberLength > 0 && numberLength < 14)" 
			
			}	// end "if (gProjectInfoPtr->listMatrixType & kListCovariance)" 
		
					// List the channel correlation statistics if requested.		
		
		if (gProjectInfoPtr->listMatrixType & kListCorrelation)
			{
			ComputeCorrelationFromCovMatrix (numberChannels, 
														covariancePtr, 
														numberChannels, 
														NULL, 
														stdDevVectorPtr,
														kTriangleOutputMatrix);
						
			sprintf (gCharBufferPtr1, "    Correlation Matrix%s", gEndOfLine);
			if (!ListString (gCharBufferPtr1, 
									(UInt32)strlen (gCharBufferPtr1), 
									gOutputTextH))
																									return;

			numberLength = 3 + gProjectInfoPtr->listCovCorPrecision;
																							
			if (!ListLowerTriangularMatrix (covariancePtr, 
														numberChannels, 
														numberLength, 
														gProjectInfoPtr->listCovCorPrecision))
																									return;
				
			}	// end "if (gProjectInfoPtr->listMatrixType & kListCorrelation)"
			
		}	// end "if (continueFlag && numberPixels > 0)"
		
			// Insert a blank line
		
	continueFlag = OutputString (NULL, 
											(char*)gEndOfLine,
											0, 
											gOutputForce1Code,
											continueFlag); 

}	// end "ListStatistics" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ListStatsControl
//
//	Software purpose:	The purpose of this routine is to list the field and class 
//							statistics information. Note that minimum and maximum values will 
//							not be listed for transformed data since there is no guarantee 
//							that transforming the minimum and maximum values really represent 
//							the true minimums and maximums in the transformed data.	
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			StatisticsWControlEvent
//
//	Coded By:			Larry L. Biehl			Date: 11/21/1988
//	Revised By:			Larry L. Biehl			Date: 04/26/2019

void ListStatsControl (
				SInt16								statsWindowMode,
				SInt16								listMenuItem)

{
	HDoublePtr							maximumVectorPtr = NULL,
											meanVectorPtr = NULL,
											minimumVectorPtr = NULL,
											stdDevVectorPtr = NULL,
											classCovariancePtr = NULL,
											fieldCovariancePtr = NULL;
											
	HPClassNamesPtr					classNamesPtr;
	
	time_t								startTime;
	
	SInt32								numberBytes;
	
	SInt16								classStorage,
											returnCode,
											updateStatsWindowMode;

	Boolean								continueFlag;
	

	if (statsWindowMode >= 2 && statsWindowMode <= 4)
		{
				// Initialize some global variables pertaining to output.			
		
		gOutputTextOKFlag = TRUE;
		gOutputCode = gOutputForce1Code = 1;
			                                         
		returnCode = 1;
		continueFlag = TRUE;
		
				// Get user specified settings.
				
		switch (listMenuItem)
			{
			case 1:
				gProjectInfoPtr->listClassFlag = TRUE;
				gProjectInfoPtr->listFieldFlag = FALSE;
				break;
				
			case 2:			
				gProjectInfoPtr->listClassFlag = TRUE;
				gProjectInfoPtr->listFieldFlag = TRUE;
				break;
				
			case 3:			
				gProjectInfoPtr->listClassFlag = FALSE;
				gProjectInfoPtr->listFieldFlag = TRUE;
				break;
				
			case 4:			
				continueFlag = ListStatsDialog (statsWindowMode);
				break;
			
			}	// end "switch (listMenuItem)"
				
				// Change cursor to watch cursor until done with process.			
						
		MSetCursor (kWait); 
			
				// Force text selection to start from end of present text.		
				
		ForceTextToEnd ();	
			
				// Get start time.													
	
		startTime = time (NULL);
							
		classNamesPtr = gProjectInfoPtr->classNamesPtr;
		
				// Get memory to be used for temporary storage of a covariance or	
				// correlation matrix if needed.	Allow for a square matrix if a
				// transformation will be used.											
		
		if (continueFlag && gProjectInfoPtr->listMatrixType & 
										(kListMeansStdDev+kListCovariance+kListCorrelation))
			{
			UInt32	numberCovarianceEntries = gProjectInfoPtr->numberCovarianceEntries;
			if (gProjectInfoPtr->listMatrixType & kListTransformedStats)
				numberCovarianceEntries = gProjectInfoPtr->numberStatisticsChannels *
														gProjectInfoPtr->numberStatisticsChannels;
													
			numberBytes = numberCovarianceEntries * sizeof (double);
				
			fieldCovariancePtr = (HDoublePtr)MNewPointer (numberBytes);
			continueFlag = (fieldCovariancePtr != NULL);
				
			}	// end "if (continueFlag && gProjectInfoPtr->listMatrixType & ..."
			
				// Get memory for mean vector if needed.
				
		if (continueFlag && (gProjectInfoPtr->listMatrixType & kListMeansStdDev))
			{
			numberBytes = gProjectInfoPtr->numberStatisticsChannels * sizeof (double);
			
			meanVectorPtr = (HDoublePtr)MNewPointer (numberBytes);
			continueFlag = (meanVectorPtr != NULL);
			
			}	// end "if (continueFlag && (gProjectInfoPtr->listMatrixType & ..."
			
				// Get memory for standard deviation vector if needed.
				
		if (continueFlag && (gProjectInfoPtr->listMatrixType & 
											kListMeansStdDev+kListCovariance+kListCorrelation))
			{
			numberBytes = gProjectInfoPtr->numberStatisticsChannels * sizeof (double);
			
			stdDevVectorPtr = (HDoublePtr)MNewPointer (numberBytes);
			continueFlag = (stdDevVectorPtr != NULL);
			
			}	// end "if (continueFlag && (gProjectInfoPtr->listMatrixType & ..."
			
				// Get memory for minimum vector if needed.
				
		if (continueFlag && (gProjectInfoPtr->listMatrixType & kListMeansStdDev) &&
									!(gProjectInfoPtr->listMatrixType & kListTransformedStats))
			{
			numberBytes = gProjectInfoPtr->numberStatisticsChannels * sizeof (double);
			
			minimumVectorPtr = (HDoublePtr)MNewPointer (numberBytes);
			continueFlag = (minimumVectorPtr != NULL);
			
			}	// end "if (continueFlag && (gProjectInfoPtr->listMatrixType & ..."
			
				// Get memory for maximum vector if needed.
				
		if (continueFlag && (gProjectInfoPtr->listMatrixType & kListMeansStdDev) &&
									!(gProjectInfoPtr->listMatrixType & kListTransformedStats))
			{
			numberBytes = gProjectInfoPtr->numberStatisticsChannels * sizeof (double);
			
			maximumVectorPtr = (HDoublePtr)MNewPointer (numberBytes);
			continueFlag = (maximumVectorPtr != NULL);
			
			}	// end "if (continueFlag && (gProjectInfoPtr->listMatrixType & ..."
			
				// Get memory for feature transformation if needed.
		
		if (continueFlag)			
			continueFlag = SetupFeatureTransformationMemory (
									(gProjectInfoPtr->listMatrixType & kListTransformedStats),
									gProjectInfoPtr->numberStatisticsChannels, 
									gProjectInfoPtr->numberStatisticsChannels, 
									&gTransformationMatrix.eigenVectorPtr, 
									&gTransformationMatrix.tempMatrixPtr,
									&gTransformationMatrix.offsetVectorPtr, 
									&gTransformationMatrix.eigenFeaturePtr,
									kLoadMatricesVectors,
									gProjectInfoPtr->channelsPtr);
								
		if (statsWindowMode == 2 || statsWindowMode == 3)
			{
					// Get memory to be used for temporary storage of class			
					// sums of squares.			
															
			UInt32	numberCovarianceEntries = gProjectInfoPtr->numberCovarianceEntries;
			if (gProjectInfoPtr->listMatrixType & kListTransformedStats)
				numberCovarianceEntries = gProjectInfoPtr->numberStatisticsChannels *
														gProjectInfoPtr->numberStatisticsChannels;
													
			numberBytes = numberCovarianceEntries * sizeof (double);
				
			if (continueFlag)
				classCovariancePtr = (HDoublePtr)MNewPointer (numberBytes);
		
					// Also get memory to be used for temporary storage of 			
					// class channels statistics.												
					
			numberBytes = (SInt32)gProjectInfoPtr->numberStatisticsChannels * 
																		sizeof (ChannelStatistics);
			if (classCovariancePtr != NULL)
				gTempChannelStatsPtr = (HChannelStatisticsPtr)MNewPointer (numberBytes);
				
			continueFlag = (gTempChannelStatsPtr != NULL);
				
			}	// end "if (statsWindowMode == 2 || statsWindowMode == 3)" 
	
				// Get memory for 3 character buffers to be used in listing.  		
				// Allow 12 digits plus the precision for each channel and 50 characters 
				// for the labels for the covariance and correlation lines - first
				// buffer.  
				// Allow 12 digits plus the precision for each channel for the mean and
				// standard deviation lines - buffers 1, 2 and 3.					
		
		UInt32 numberCovCorDigits = 13 + gProjectInfoPtr->listCovCorPrecision;
		UInt32 numberMeanStdDevDigits = 13 + gProjectInfoPtr->listMeanStdPrecision;
		numberCovCorDigits = MAX (numberCovCorDigits, numberMeanStdDevDigits);
		
		numberBytes = (SInt32)50 + 
							gProjectInfoPtr->numberStatisticsChannels * numberCovCorDigits;
		if (continueFlag)
			gCharBufferHandle1 = (Handle)MNewHandle (numberBytes);
		
		numberBytes = (SInt32)50 + 
					gProjectInfoPtr->numberStatisticsChannels * numberMeanStdDevDigits;
		if (gCharBufferHandle1 != NULL)
			gCharBufferHandle2 = (Handle)MNewHandle (numberBytes);
			
		if (gCharBufferHandle2 != NULL)
			gCharBufferHandle3 = (Handle)MNewHandle (numberBytes);
			
		if (gCharBufferHandle3 == NULL)
			continueFlag = FALSE;
			
		if (continueFlag && minimumVectorPtr != NULL)
			{
					// Will be listing minimum and maximum values.
					
			l_CharBufferPtr4 = (char*)MNewPointer (numberBytes);
			
			if (l_CharBufferPtr4 != NULL && maximumVectorPtr != NULL)
				l_CharBufferPtr5 = (char*)MNewPointer (numberBytes);
				
			if (l_CharBufferPtr5 == NULL)
				continueFlag = FALSE;
				
			}	// end "if (continueFlag && minimumVectorPtr != NULL)"
		
		if (continueFlag)
			{
					// Check handle to image file information.  If handle to image	
					// file information doesn't exists. Find the	image file and 	
					// get the information for it.											
			
			GetProjectImageFileInfo (kDoNotPrompt, kSetupGlobalInfoPointers);
					
					// List the processor name, date, time and project info.			

			continueFlag = ListHeaderInfo (
									NULL,
									kLImageInfo+kLProjectName+kLProjectImageName+kLStatType, 
									&gOutputForce1Code, 
									gProjectInfoPtr->covarianceStatsToUse, 
									continueFlag);
			
					// List the channels used.													
			
			if (continueFlag)	
				continueFlag = ListChannelsUsed (
														gImageFileInfoPtr,
														NULL,
														(SInt16*)gProjectInfoPtr->channelsPtr,
														gProjectInfoPtr->numberStatisticsChannels, 
														NULL, 
														&gOutputForce1Code,
														kNoTransformation); 
			
					// List "  Output Information:"
					
			continueFlag = ListSpecifiedStringNumber (kSharedStrID, 
																	IDS_Shared8,
																	(unsigned char*)gTextString, 
																	NULL, 
																	gOutputForce1Code, 
																	continueFlag);
			
			if (!continueFlag)
				statsWindowMode = 0;
			
					// Update the statistics if needed.										
					// Statistics do not need to be updated if the enhanced			
					// statistics are to be listed.											
			
			if (gProjectInfoPtr->covarianceStatsToUse != kEnhancedStats)
				{								
						// Check if the project statistics need to be updated for the 
						// LOOC even if only the class stats mode has been requested.
						// The common covariance may be needed which involves all
						// classes.
						
				updateStatsWindowMode = statsWindowMode;
				
				if (updateStatsWindowMode == kUpdateClass)
					{
					classStorage = gProjectInfoPtr->storageClass[
																		gProjectInfoPtr->currentClass];
						
					if (classNamesPtr[classStorage].covarianceStatsToUse == 
																				kLeaveOneOutStats &&
												gProjectInfoPtr->useCommonCovarianceInLOOCFlag)
						updateStatsWindowMode = kUpdateProject;
					
					}	// end "if (updateStatsWindowMode == kUpdateClass)"
				
				switch (updateStatsWindowMode)
					{
					case kUpdateProject:
						if (!gProjectInfoPtr->statsUpToDate)
							returnCode = UpdateStatsControl (kUpdateProject, TRUE, FALSE);
						break;
						
					case kUpdateClass:                            
						if (!gProjectInfoPtr->classNamesPtr[classStorage].statsUpToDate)
							returnCode = UpdateStatsControl (kUpdateClass, TRUE, FALSE);
						break;
						
					case kUpdateField:
						if (!gProjectInfoPtr->keepClassStatsOnlyFlag &&
										!gProjectInfoPtr->fieldIdentPtr[
												gProjectInfoPtr->currentField].statsUpToDate)
							returnCode = UpdateStatsControl (kUpdateField, TRUE, FALSE);
						break;
						
					}	// end "switch (updateStatsWindowMode)" 
					
				if (returnCode == 3)
							// User requests that statistics not be updated.			
							// List them as they currently are.								
					returnCode = 1;
					
				}	// end "if (gProjectInfoPtr->covarianceStatsToUse != kEnhancedStats)" 
			
					// Get pointer to strings to use for listing information.		
					
			gCharBufferPtr1 =  (HPtr)GetHandlePointer (gCharBufferHandle1,
																		kLock);
			
			gCharBufferPtr2 =  (HPtr)GetHandlePointer (gCharBufferHandle2,
																		kLock);
			
			gCharBufferPtr3 =  (HPtr)GetHandlePointer (gCharBufferHandle3,
																		kLock);
			
					// Now list the appropriate statistics.								
					
			if (returnCode == 1)
				{
				switch (statsWindowMode)
					{            
					case kUpdateProject:
						ListProjectStats (gProjectInfoPtr->numberStatisticsChannels,
												meanVectorPtr,
												stdDevVectorPtr,
												minimumVectorPtr,
												maximumVectorPtr,
												classCovariancePtr,
												fieldCovariancePtr);
						break;
						
					case kUpdateClass:
						ListClassStats (gProjectInfoPtr->numberStatisticsChannels,
												gProjectInfoPtr->currentClass,
												meanVectorPtr,
												stdDevVectorPtr,
												minimumVectorPtr,
												maximumVectorPtr,
												classCovariancePtr,
												fieldCovariancePtr);
						break;
						
					case kUpdateField:
						ListFieldStats (gProjectInfoPtr->numberStatisticsChannels,
												gProjectInfoPtr->currentClass, 
												gProjectInfoPtr->currentField,
												meanVectorPtr,
												stdDevVectorPtr,
												minimumVectorPtr,
												maximumVectorPtr,
												fieldCovariancePtr);
						break;
						
					}	// end "switch (statsWindowMode)" 
					
				}	// end "if (returnCode == 1)" 
		
			UnlockProjectWindowInfoHandles ();
					
					// Get the end time and print the time elapsed in the 	
					// output window.														
						
			continueFlag = ListCPUTimeInformation (NULL, continueFlag, startTime);
				
					// Scroll output window to the selection and adjust the 			
					// scroll bar.																	
						
			UpdateOutputWScrolls (gOutputWindow, 1, kDisplayMessage);
			
			}	// end "if (gCharBufferHandle3)" 
	
				// Release memory for transformation matrix information.					
				
		ReleaseFeatureTransformationMemory ();
			
		gCharBufferHandle1 = UnlockAndDispose (gCharBufferHandle1);
		gCharBufferHandle2 = UnlockAndDispose (gCharBufferHandle2);
		gCharBufferHandle3 = UnlockAndDispose (gCharBufferHandle3);
		l_CharBufferPtr4 = CheckAndDisposePtr (l_CharBufferPtr4);
		l_CharBufferPtr5 = CheckAndDisposePtr (l_CharBufferPtr5);
		
		gCharBufferPtr1 = NULL;
		gCharBufferPtr2 = NULL;
		gCharBufferPtr3 = NULL;
		l_CharBufferPtr4 = NULL;
		l_CharBufferPtr5 = NULL;
		
		gTempChannelStatsPtr = (ChannelStatisticsPtr)CheckAndDisposePtr (
																			(Ptr)gTempChannelStatsPtr);
												
		CheckAndDisposePtr (classCovariancePtr);
		CheckAndDisposePtr (fieldCovariancePtr);
		CheckAndDisposePtr (meanVectorPtr);
		CheckAndDisposePtr (stdDevVectorPtr);
		CheckAndDisposePtr (minimumVectorPtr);
		CheckAndDisposePtr (maximumVectorPtr);
			
		MInitCursor ();
			
		}	// end "if (statsWindowMode >= 2 && statsWindowMode <= 4)" 

}	// end "ListStatsControl" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ListStatsDialog
//
//	Software purpose:	The purpose of this routine is to obtain the
//							user specifications for listing of the project statistics.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:  	True if user selects OK in dialog box
//							False if user selects Cancel in dialog box
//
// Called By:			ListStatsControl
//
//	Coded By:			Larry L. Biehl			Date: 10/09/1997
//	Revised By:			Larry L. Biehl			Date: 12/16/2016
	
Boolean ListStatsDialog (
				SInt16								statsWindowMode)
	
{
	Boolean								returnFlag = FALSE;
	
	#if defined multispec_mac
		Rect									theBox;
		
		DialogPtr							dialogPtr;
		
		Handle								okHandle,
												theHandle;
												
		SInt32								theNum;
		
		SInt16								itemHit,
												listMeanStdPrecision,
												listCovCorPrecision,
												maxNumberDigits = 10,
												theType;
											
		Boolean								featureTransformationFlag,
												listFieldFlag,
												listClassFlag,
												listMeansStdDevFlag,
												listCovarianceFlag,
												listCorrelationFlag,
												modalDone;

		
				// Get the modal dialog for the histogram specification.					
					
		dialogPtr = LoadRequestedDialog (
											kStatListSpecificationID, kDoNotCopyScrap, 1, 2);
		if (dialogPtr == NULL)												
																							return (FALSE);
		
				// Set routine to draw outline on "OK" box.									
				
		//SetDialogItemDrawRoutine (dialogPtr, 16, gHiliteOKButtonPtr);
		
				// Initialize dialog variables.
																						
		ListStatsDialogInitialize (dialogPtr,
											statsWindowMode,
											&listFieldFlag,
											&listClassFlag,
											&listMeansStdDevFlag,
											&listCovarianceFlag,
											&listCorrelationFlag,
											&featureTransformationFlag,
											&listMeanStdPrecision,
											&listCovCorPrecision);
		
				// Save handle for the OK button for use later.								
				
		GetDialogItem (dialogPtr, 1,  &theType, &okHandle, &theBox);
			
				// Update the modal dialog.
				
				// Set check boxes
			
		SetDLogControl (dialogPtr, 4, (SInt16)listFieldFlag);		
			
		SetDLogControl (dialogPtr, 5, (SInt16)listClassFlag);						
		
		SetDLogControl (dialogPtr, 7, (SInt16)listMeansStdDevFlag);								
		
		SetDLogControl (dialogPtr, 8, (SInt16)listCovarianceFlag);									
		
		SetDLogControl (dialogPtr, 9, (SInt16)listCorrelationFlag);
			
		SetDLogControl (dialogPtr, 11, (SInt16)featureTransformationFlag);
		
				// Set precision values
				
		LoadDItemValue (dialogPtr, 13, listMeanStdPrecision);
		
		LoadDItemValue (dialogPtr, 15,listCovCorPrecision);
			
				// Center the dialog and then show it.											
				
		ShowDialogWindow (dialogPtr, kStatListSpecificationID, kSetUpDFilterTable);
		
				// Select the text in the mean precision box
			
		SelectDialogItemText (dialogPtr, 13, 0, INT16_MAX);
					
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
					case 4:
					case 5:
						ChangeDLogCheckBox ((ControlHandle)theHandle);
						
						if ((GetDLogControl (dialogPtr, 4) == 1) || 
																(GetDLogControl (dialogPtr, 5) == 1))
							HiliteControl ((ControlHandle)okHandle, 0);
							
						else
							HiliteControl ((ControlHandle)okHandle, 255);
						break;
					
					case 7:
					case 8:
					case 9:
					case 11:
						ChangeDLogCheckBox ((ControlHandle)theHandle);
						break;
						
					case 13:		//	 Mean/Std. Dev. Precision.  		
						if (theNum == 0 || theNum > maxNumberDigits)
							NumberErrorAlert (listMeanStdPrecision, dialogPtr, itemHit);
						
						else	// theNum > 0 && theNum <= maxNumberDigits)
							listMeanStdPrecision = theNum;
						break;
						
					case 15:		//	 Covariance/Correlation Precision.  		
						if (theNum == 0 || theNum > maxNumberDigits)
							NumberErrorAlert (listCovCorPrecision, dialogPtr, itemHit);
						
						else	// theNum > 0 && theNum <= maxNumberDigits)
							listCovCorPrecision = theNum;
						break;
							
					}	// end "switch (itemHit)"
					
				}	// end "if (itemHit > 2)" 
				
			else	// itemHit <= 2 
				{	
				if (itemHit == 1)
					{
					modalDone = TRUE;
					
					ListStatsDialogOK ((GetDLogControl (dialogPtr, 4) == 1),
												(GetDLogControl (dialogPtr, 5) == 1),
												(GetDLogControl (dialogPtr, 7) == 1),
												(GetDLogControl (dialogPtr, 8) == 1),
												(GetDLogControl (dialogPtr, 9) == 1),
												(GetDLogControl (dialogPtr, 11) == 1),
												listMeanStdPrecision,
												listCovCorPrecision);
												
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
		CMListStatsDialog*		dialogPtr = NULL;
		
		TRY
			{ 
			dialogPtr = new CMListStatsDialog (); 
			
			returnFlag = dialogPtr->DoDialog (statsWindowMode); 
		                       
			delete dialogPtr;
			}
			
		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			returnFlag = FALSE;
			}
		END_CATCH_ALL
	#endif // defined multispec_win
      
	#if defined multispec_wx
      CMListStatsDialog*		dialogPtr = NULL;
      
      dialogPtr = new CMListStatsDialog (); 
			
		returnFlag = dialogPtr->DoDialog (statsWindowMode); 
		                       
		delete dialogPtr;
	#endif
	
	return (returnFlag);

}	// end "ListStatsDialog" 



void ListStatsDialogInitialize (
				DialogPtr							dialogPtr,
				SInt16								statsWindowMode,
				Boolean*								listFieldFlagPtr,
				Boolean*								listClassFlagPtr,
				Boolean*								listMeansStdDevFlagPtr,
				Boolean*								listCovarianceFlagPtr,
				Boolean*								listCorrelationFlagPtr,
				Boolean*								featureTransformationFlagPtr,
				SInt16*								listMeanStdPrecisionPtr,
				SInt16*								listCovCorPrecisionPtr)
	
{
	HPClassNamesPtr					classNamesPtr;
	
	SInt16								currentStorageClass,
											eigenSource,
											listFieldHiliteValue,
											listOptionsCode;
											
	UInt16								numberEigenvectors;
	
		
			// Class and/or field statistics flags	
	
	*listFieldFlagPtr = gProjectInfoPtr->listFieldFlag;
	*listClassFlagPtr = gProjectInfoPtr->listClassFlag;
	
	listFieldHiliteValue = 0;
	
	switch (statsWindowMode)
		{
		case 2:
		
			if (gProjectInfoPtr->numberStatTrainFields == 0) 
			//if (gProjectInfoPtr->numberStatTrainFields == 0 ||
			//											gProjectInfoPtr->keepClassStatsOnlyFlag)
				listFieldHiliteValue = 255;
			break;
			
		case 3:				
			
			classNamesPtr = gProjectInfoPtr->classNamesPtr;
			currentStorageClass =
							gProjectInfoPtr->storageClass[gProjectInfoPtr->currentClass];
		
			if (classNamesPtr[currentStorageClass].numberOfTrainFields == 0) 
			//if (classNamesPtr[currentStorageClass].numberOfTrainFields == 0 ||
			//											gProjectInfoPtr->keepClassStatsOnlyFlag)
				listFieldHiliteValue = 255;
			
			SetDLogControlTitleNumber (kDialogStrID, 
												IDS_Dialog20,		// Class
												dialogPtr, 
												IDC_Classes,
												(Str255*)&gTextString);
			break;
			
		case 4:
		
			*listClassFlagPtr = FALSE;
			*listFieldFlagPtr = TRUE;
			if (gProjectInfoPtr->keepClassStatsOnlyFlag)
				{        
				HideDialogItem (dialogPtr, IDC_MeansStdDev);
				HideDialogItem (dialogPtr, IDC_CovarianceMatrix);
				HideDialogItem (dialogPtr, IDC_CorrelationMatrix);
				HideDialogItem (dialogPtr, IDC_MeansStdDevPrompt);
				HideDialogItem (dialogPtr, IDC_MeansStdDevPrecision); 
				HideDialogItem (dialogPtr, IDC_CovariancePrompt);
				HideDialogItem (dialogPtr, IDC_CovariancePrecision); 
				HideDialogItem (dialogPtr, IDC_UseTransformation);
				
				}	// end "if (gProjectInfoPtr->keepClassStatsOnlyFlag)"
		
			HideDialogItem (dialogPtr, IDC_Classes);
			
			SetDLogControlTitleNumber (kDialogStrID, 
												IDS_Dialog19,		// Field
												dialogPtr, 
												IDC_Fields, 
												(Str255*)&gTextString);	
			break;
								
		}	// end "switch (statsWindowMode)"												
	
	if (listFieldHiliteValue == 255)
		*listFieldFlagPtr = FALSE; 
		
	SetDLogControlHilite (dialogPtr, IDC_Fields, listFieldHiliteValue);
	
			// Set check box buttons for listing means & standard deviations
			//													covariances
			//													correlations		
			
	listOptionsCode = gProjectInfoPtr->listMatrixType;									
	
	*listMeansStdDevFlagPtr = FALSE;
	if (listOptionsCode&kListMeansStdDev)
		*listMeansStdDevFlagPtr = TRUE;  
		
	*listCovarianceFlagPtr = FALSE;
	if (listOptionsCode&kListCovariance)
		*listCovarianceFlagPtr = TRUE;  
			
	*listCorrelationFlagPtr = FALSE;
	if (listOptionsCode&kListCorrelation)
		*listCorrelationFlagPtr = TRUE;  	
	
			// Set check box button for listing transformed statistics.		
		
	EigenvectorInfoExists (kProject, &eigenSource, &numberEigenvectors);
	if (numberEigenvectors <= 0)
		{		
		*featureTransformationFlagPtr = FALSE; 
		HideDialogItem (dialogPtr, IDC_UseTransformation);
		
		}	// end "if (numberEigenvectors <= 0)"
		
	else	// numberEigenvectors > 0
		{
		*featureTransformationFlagPtr = FALSE;
		if (listOptionsCode & kListTransformedStats)
			*featureTransformationFlagPtr = TRUE;
			
		}	// end "else numberEigenvectors > 0"
	
			// List precision values
			
	*listMeanStdPrecisionPtr = gProjectInfoPtr->listMeanStdPrecision;
	*listCovCorPrecisionPtr = gProjectInfoPtr->listCovCorPrecision; 
	
	if (gProjectInfoPtr->statisticsCode != kMeanCovariance)
		{        
		HideDialogItem (dialogPtr, IDC_CovarianceMatrix);
		HideDialogItem (dialogPtr, IDC_CorrelationMatrix);
		HideDialogItem (dialogPtr, IDC_CovariancePrompt);
		HideDialogItem (dialogPtr, IDC_CovariancePrecision); 								
	
		*listCovarianceFlagPtr = FALSE;	
		*listCorrelationFlagPtr = FALSE;
		
		}	// end "if (gProjectInfoPtr->statisticsCode != kMeanCovariance)"

}	// end "ListStatsDialogInitialize" 



void ListStatsDialogOK (
				Boolean								listFieldFlag,
				Boolean								listClassFlag,
				Boolean								listMeansStdDevFlag,
				Boolean								listCovarianceFlag,
				Boolean								listCorrelationFlag,
				Boolean								featureTransformationFlag,
				SInt16								listMeanStdPrecision,
				SInt16								listCovCorPrecision)
				
{				
			// list classes or fields.						

	gProjectInfoPtr->listFieldFlag = listFieldFlag;
	gProjectInfoPtr->listClassFlag = listClassFlag;
	
			// Include means & standard deviations, covariances and/or 
			// correlations.						

	gProjectInfoPtr->listMatrixType = 0;
	
	if (listMeansStdDevFlag)
		gProjectInfoPtr->listMatrixType += kListMeansStdDev;
	
	if (listCovarianceFlag)
		gProjectInfoPtr->listMatrixType += kListCovariance;
	
	if (listCorrelationFlag)
		gProjectInfoPtr->listMatrixType += kListCorrelation;
		
			// List transformed statistics
	
	if (featureTransformationFlag)
		gProjectInfoPtr->listMatrixType += kListTransformedStats;
		
	gProjectInfoPtr->listMeanStdPrecision = listMeanStdPrecision;
	
	gProjectInfoPtr->listCovCorPrecision = listCovCorPrecision;

}	// end "ListStatsDialogOK"

