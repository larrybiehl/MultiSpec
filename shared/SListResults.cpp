//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SListResults.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			02/07/2018
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	This file contains functions that relate to listing
//								classification results.
//
//	Functions in file:	pascal void 			DrawDialogClassAreaPopUp
//								void 						ForceListResultsCodeResourceLoad
//								short int 				GetAreaResults
//								Boolean 					GetListResultsStringBuffer
//								Handle	 				GetProbabilityWindowInfoHandle
//								HUInt16Ptr 				GetSymbolToClassVector
//								void						GetTableColumnWidths
//								Boolean 					ListClassificationSummary
//								Boolean					ListFCGPerformanceTable
//								Boolean 					ListLineOfResults
//								Boolean 					ListOverallPerformanceValues
//								void 						ListPerformanceTables
//								Boolean 					ListPerformanceTableTotals
//								Boolean					ListResultsAreasControl
//								void 						ListResultsControl
//								Boolean  				ListResultsDialog
//								Boolean 					ListResultsThematicClasses
//								short int 				ListResultsTrainTestFields
//								void 						LoadAreaString
//								void						LoadClassAreaVector
//								void						LoadClassInGroupOrderVector
//								void 						LoadListResultsSpecs
//								void 						LoadListResultsString
//								Boolean 					LoadListResultsTitle
//								void 						LoadPercentString
//								void 						LoadSamplesString
//
//	Diagram of MultiSpec routine calls for the routines in the file.
//
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
/* Template for debugging
		int numberChars = sprintf ((char*)gTextString3,
												" SLstRslt::xxx (entered routine. %s", 
												gEndOfLine);
		ListString ((char*)gTextString3, numberChars, gOutputTextH);	
*/
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h" 
   
#if defined multispec_lin
	#include "LListResultsDialog.h"
#endif	// defined multispec_lin

#if defined multispec_mac || defined multispec_mac_swift
	#define	IDC_ProbabilityPrompt	32
	#define	IDC_Probability			33
	#define	IDC_LRPercent				38
	#define	IDC_LRdegrees				39
#endif	// defined multispec_mac || defined multispec_mac_swift 
  
#if defined multispec_win 
	#include "WListResultsDialog.h"
#endif	// defined multispec_win 

//#include "SExtGlob.h"
								

#define	kReferenceAccuracyWidth		9
#define	kMinTableWidth					6
#define	kMaxClassGroupNameWidth		12
#define	kThresholdNumberWidth		9
#define	kTotalNumberWidth				7

#define	kClassPerformance				1
#define	kGroupPerformance				2

#define	kIncludeTab						1

extern void 			ListResultsDialogSetThresholdItems (
								DialogPtr							dialogPtr,
								Boolean								thresholdResultsFlag,
								SInt16								thresholdTypeCode);


ListResultsSpecsPtr						gListResultsSpecsPtr;



			// Prototypes for routines in this file that are only called by		
			// other routines in this file.	
			
double 					ComputeKappaStatistic (
								ClassifierVar*						clsfyVariablePtr,
								SInt16*								columnClassPtr, 
								UInt32								numberClasses,
								double*								kappaVariancePtr);

Boolean 					DetermineIfLastClassForGroup (
								ClassifierVar*						clsfyVariablePtr,
								SInt16								groupIndex,
								SInt16								classIndex,
								SInt16*								classPtr, 
								UInt32								numberClasses);										
		
SInt16	 				GetAreaResults (
								SInt16								classPointer, 
								AreaDescriptionPtr				areaDescriptionPtr,
								FileIOInstructionsPtr			fileIOInstructions1Ptr,
								FileIOInstructionsPtr			fileIOInstructions2Ptr,
								ClassifierVarPtr					clsfyVariablePtr, 
								HSInt64Ptr 							countVectorPtr);

UInt32 					GetClassGroupNameMaxLength (
								FileInfoPtr	 						fileInfoPtr,
								Boolean								groupFlag,
								HUInt32Ptr							nameLengthVectorPtr);

SInt32 					GetCountVectorIndex (
								ClassifierVar*						clsfyVariablePtr,
								SInt16*								classAreaPtr,
								UInt16								classIndex,
								Boolean								groupOrderedFlag);
								
SInt64 					GetClassOrGroupNumberOfSamples (
								ClassifierVar*						clsfyVariablePtr,
								HSInt64Ptr							classCountVectorPtr,
								SInt32								countVectorIndex,
								Boolean								outputGroupedFlag,
								Boolean								inputGroupedFlag);
								
SInt32 					GetGroupIndex (
								ClassifierVar*						clsfyVariablePtr,
								UInt32								statClassNumber);
																
Handle	 				GetProbabilityWindowInfoHandle (void);
																
HUInt16Ptr 				GetSymbolToClassVector (
								FileInfoPtr							fileInfoPtr);
																
void						GetTableColumnWidths (
								ClassifierVar*						clsfyVariablePtr,
								SInt16*								classPtr,
								UInt32								numberClasses);

SInt16 					ListAreaValue (
								char*									stringPtr,
								ClassifierVar*						clsfyVariablePtr,
								double								area,
								SInt16								strLength,
								SInt16								decimalPlaces,
								SInt16								areaWidthWithCommas);
													
Boolean 					ListFCGPerformanceTable (
								ClassifierVar*						clsfyVariablePtr, 
								SInt16								fieldType, 
								CMFileStream*						outputFileStreamPtr, 
								SInt16*								outputCodePtr, 
								SInt16*								classPtr, 
								UInt32								numberClasses, 
								SInt16*								classAreaPtr, 
								UInt32								numberClassAreas,
								UInt32								numberTableListColumns);
										
Boolean 					ListLineOfResults (
								ClassifierVar*						clsfyVariablePtr, 
								HSInt64Ptr							classifySubTotalVectorPtr, 
								SInt64								totalNumberSamplesInLine, 
								SInt16*								columnClassPtr, 
								UInt32								numberColumns, 
								SInt32								countVectorIndex, 
								char*									columnNamePtr, 
								CMFileStream*						outputFileStreamPtr, 
								SInt16*								outputCodePtr, 
								Boolean								countTotalFlag);

Boolean 					ListOverallPerformanceValues (
								ClassifierVar* 					clsfyVariablePtr, 
								CMFileStream*						outputFileStreamPtr, 
								SInt16*								outputCodePtr, 
								SInt16*								classPtr, 
								UInt32								numberClasses, 
								SInt16*								classAreaPtr, 
								UInt32								numberClassAreas,
								UInt32								numberTableListColumns,
								SInt16								classGroupCode);
										
Boolean 					ListPerformanceTableTotals (
								ClassifierVar						*clsfyVariablePtr, 
								HSInt64Ptr							countTotalVectorPtr, 
								SInt16*								classPtr, 
								UInt32								numberColumns, 
								CMFileStream*						outputFileStreamPtr, 
								SInt16*								outputCodePtr);

Boolean 					ListReliabilityAccuracyLine (
								ClassifierVar*						clsfyVariablePtr,
								SInt16*								columnClassPtr, 
								UInt32								numberTableListColumns, 
								CMFileStream*						outputFileStreamPtr, 
								SInt16*								outputCodePtr);
			
Boolean					ListResultsAreasControl (
								FileInfoPtr							fileInfoPtr, 
								ClassifierVar*						clsfyVariablePtr);

Boolean  				ListResultsDialog (FileInfoPtr);
										
Boolean 					ListResultsThematicClasses (
								ClassifierVar*						clsfyVariablePtr, 
								FileInfoPtr							fileInfoPtr, 
								SInt16*								classPtr, 
								unsigned char*						symbolsPtr, 
								float*								weightsPtr, 
								UInt16								numberClasses, 
								CMFileStream*						outputFileStreamPtr, 
								SInt16*								outputCodePtr);

SInt16	 				ListResultsTrainTestFields (
								AreaDescriptionPtr				areaDescriptionPtr,
								FileIOInstructionsPtr			fileIOInstructions1Ptr,
								FileIOInstructionsPtr			fileIOInstructions2Ptr,
								ClassifierVarPtr					clsfyVariablePtr, 
								SInt16								fieldType);

Boolean 					ListTableFootNotes (
								ClassifierVar*						clsfyVariablePtr, 
								CMFileStream*						outputFileStreamPtr,
								SInt16*								outputCodePtr);
										
void 						LoadAreaString (
								Ptr*									stringPtrPtr, 
								HSInt64Ptr 							countVectorPtr, 
								SInt16*	 							classPtr, 
								Boolean								thresholdFlag, 
								SInt64								totalNumberPixels, 
								SInt16								numberWidth, 
								SInt16								columnPrec, 
								UInt16								numberClasses, 
								double								areaConversionFactor,
								HUInt32Ptr 							columnWidthVectorPtr);
								
void						LoadClassInGroupOrderVector (
								ClassifierVar*						clsfyVariablePtr,
								FileInfoPtr							fileInfoPtr, 
								SInt16*								classInGroupOrderPtr, 
								SInt16*								classPtr, 
								UInt32								numberClasses);

Boolean 					LoadListResultsSpecs (void);
										
void 						LoadListResultsString (
								ClassifierVar*						clsfyVariablePtr, 
								Ptr*									stringPtrPtr, 
								HSInt64Ptr 							countVectorPtr, 
								SInt16*	 							columnClassPtr, 
								SInt64								totalNumberPixels, 
								UInt32								numberClasses);
									
Boolean 					LoadListResultsTitle (
								ClassifierVar*						clsfyVariablePtr, 
								CMFileStream*						outputFileStreamPtr, 
								SInt16*	 							classPtr, 
								UInt32								numberColumns,
								SInt16								sampleNumberWidth,
								SInt16								percentWidth, 
								SInt16*								outputCodePtr);
										
void 						LoadPercentString (
								Ptr*									stringPtrPtr, 
								HSInt64Ptr 							countVectorPtr, 
								SInt16*	 							classPtr,
								Boolean								thresholdFlag, 
								SInt64								totalNumberPixels, 
								SInt16								numberWidth, 
								SInt16								columnPrec, 
								UInt16								numberClasses,
								HUInt32Ptr 							columnWidthVectorPtr);
										
void 						LoadSamplesString (
								Ptr*									stringPtrPtr, 
								HSInt64Ptr 							countVectorPtr, 
								SInt16*	 							classPtr,
								Boolean								thresholdFlag, 
								SInt64								totalNumberPixels, 
								SInt16								numberWidth, 
								UInt16								numberClasses,
								HUInt32Ptr 							columnWidthVectorPtr);



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		double ComputeKappaStatistic
//
//	Software purpose:	The purpose of this routine is to compute the Kappa statistic
//							for the input confusion matrix.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/24/1997
//	Revised By:			Larry L. Biehl			Date: 08/16/2010	

double ComputeKappaStatistic (
				ClassifierVar*						clsfyVariablePtr,
				SInt16*								classPtr, 
				UInt32								numberClasses,
				double*								kappaVariancePtr)

{   
	double								kappaStatistic,
											kappaVariance,
											tempValue,
											theta1,
											theta2,
											theta3,
											theta4;

	SInt64								totalCorrectSamples,
											totalNumberSamples;
											
	HSInt64Ptr							agreementMatrixPtr,
											classifyTotalVectorPtr,
				 							referenceTotalVectorPtr;
				 							
	UInt32								index,
											index2,
											matrixIndex,
											numberColumns;
											
	Boolean								groupOrderedFlag;
	
	
			// Initialize local variables.													
	
	classifyTotalVectorPtr = &clsfyVariablePtr->countVectorPtr[
											clsfyVariablePtr->totalCountIndexStart];
	referenceTotalVectorPtr = &classifyTotalVectorPtr[1*clsfyVariablePtr->tableWidth];
	agreementMatrixPtr = &classifyTotalVectorPtr[4*clsfyVariablePtr->tableWidth];	
	
			// Determine if this is a group ordered table.								
			// If so get a pointer to the class to group vector and get				
			// proper pointer to the list of classes.														
			
	groupOrderedFlag = (clsfyVariablePtr->tableType == kGroupTable ||
										clsfyVariablePtr->summaryCode == kGroupSummary);
									
	numberColumns = clsfyVariablePtr->numberKappaColumns;
		
			// Get the reference vector totals.
			
	for (index=0; index<numberColumns; index++)
		{
		referenceTotalVectorPtr[index] = 0;
		
		for (index2=0; index2<numberColumns; index2++)
			referenceTotalVectorPtr[index] += agreementMatrixPtr[index2];
			
		agreementMatrixPtr += clsfyVariablePtr->tableWidth;
		
		}	// end "for (index=0; index<..."
		
			// Get the classify vector totals.
			
	for (index=0; index<numberColumns; index++)
		{
		agreementMatrixPtr = &classifyTotalVectorPtr[4*clsfyVariablePtr->tableWidth];	
	
		classifyTotalVectorPtr[index] = 0;
		
		for (index2=0; index2<numberColumns; index2++)
			{
			classifyTotalVectorPtr[index] += agreementMatrixPtr[index];
			agreementMatrixPtr += clsfyVariablePtr->tableWidth;
			
			}	// end "for (index2=0; index2<..."
		
		}	// end "for (index=0; index<..."
		
			// Get the total number of samples in the agreement matrix.
	
	totalNumberSamples = 0;		
	for (index=0; index<numberColumns; index++)
		totalNumberSamples += referenceTotalVectorPtr[index];
		
			// Now get 'Theta1'
	
	agreementMatrixPtr = &classifyTotalVectorPtr[4*clsfyVariablePtr->tableWidth];
	totalCorrectSamples = 0;
	matrixIndex = 0;		
	for (index=0; index<numberColumns; index++)
		{
		totalCorrectSamples += agreementMatrixPtr[matrixIndex];
		matrixIndex += clsfyVariablePtr->tableWidth + 1;
		
		}	// end "for (index=0; index<numberColumns; index++)"
	
	if (totalNumberSamples > 0)	
		theta1 = (double)totalCorrectSamples/totalNumberSamples;
		
	else	// totalNumberSamples <= 0 
		theta1 = 0;
		
			// Now get 'Theta2'
	
	theta2 = 0;
	matrixIndex = 0;		
	for (index=0; index<numberColumns; index++)
		theta2 += (double)referenceTotalVectorPtr[index] * classifyTotalVectorPtr[index];
		          
	if (totalNumberSamples > 0)
		theta2 /= (double)totalNumberSamples * totalNumberSamples;
		
	else	// totalNumberSamples <= 0 
		theta2 = 0;
		
			// Now get 'Theta3'
	
	theta3 = 0;
	matrixIndex = 0;		
	for (index=0; index<numberColumns; index++)
		{
		theta3 += (double)agreementMatrixPtr[matrixIndex] * 
							(referenceTotalVectorPtr[index] + classifyTotalVectorPtr[index]);
							
		matrixIndex += clsfyVariablePtr->tableWidth + 1;
		
		}	// end "for (index=0; index<numberColumns; index++)"
		                  
	if (totalNumberSamples > 0)
		theta3 /= (double)totalNumberSamples * totalNumberSamples;
		
	else	// totalNumberSamples <= 0 
		theta3 = 0;
		
			// Now get 'Theta4'
	
	theta4 = 0;
	matrixIndex = 0;		
	for (index=0; index<numberColumns; index++)
		{
		for (index2=0; index2<numberColumns; index2++)
			{
			tempValue = (double)referenceTotalVectorPtr[index2] + 
																			classifyTotalVectorPtr[index];
			
			theta4 += (double)agreementMatrixPtr[index2] * tempValue * tempValue;
							
			}	// end "for (index2=0; index2<numberColumns; index2++)"
		
		agreementMatrixPtr += clsfyVariablePtr->tableWidth;
		
		}	// end "for (index=0; index<numberColumns; index++)"
		                  
	if (totalNumberSamples > 0)
		theta4 /= (double)totalNumberSamples * totalNumberSamples * totalNumberSamples;
		
	else	// totalNumberSamples <= 0 
		theta4 = 0;
	
			// Now get the Kappa Statistic in units of %.
	
	kappaStatistic = 1.;
	tempValue = 1. - theta2;
	
	if (tempValue != 0)
		kappaStatistic = (theta1 - theta2)/tempValue;
		
	kappaStatistic *= 100.;
	
			// Now get the Kappa Variance.
	
	if (tempValue != 0)
		{
		tempValue *= tempValue;		
		kappaVariance = theta1 * (1-theta1)/tempValue;
		
		tempValue *= (1 - theta2);
		kappaVariance += 2 * (1-theta1) * (2*theta1*theta2-theta3)/tempValue;
		
		tempValue *= (1 - theta2);
		kappaVariance += (1-theta1) * (1-theta1) * (theta4 - 4*theta2*theta2)/tempValue;
									
		if (totalNumberSamples > 0)			
			kappaVariance /= totalNumberSamples;
		
		}	// end "if (tempValue != 0)"
		
	else	// tempValue == 0
		kappaVariance = 0;
	
	*kappaVariancePtr = kappaVariance;
	
	return (kappaStatistic);
	
}	// end "ComputeKappaStatistic"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean DetermineIfLastClassForGroup
//
//	Software purpose:	The purpose of this routine is to determine if the input class
//							is the last class for the group.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/30/1997
//	Revised By:			Larry L. Biehl			Date: 07/17/1998	

Boolean DetermineIfLastClassForGroup (
				ClassifierVar*						clsfyVariablePtr,
				SInt16								groupIndex,
				SInt16								classIndex,
				SInt16*								classPtr, 
				UInt32								numberClasses)

{  
	SInt16*								classToGroupPtr;
	
	UInt32								backgroundIndexOffset,
											fileNumberClasses,
											nextClassToGroupIndex;

	SInt32								nextGroupIndex;
											
	Boolean								lastClassForGroupFlag;
	
	
	lastClassForGroupFlag = FALSE;
	
	if ((UInt32)classIndex == numberClasses-1)
		lastClassForGroupFlag = TRUE;
	
	else	// classIndex < numberClasses-1 
		{		
		fileNumberClasses = GetActiveNumberClasses ();
		classToGroupPtr = GetClassToGroupPointer (gImageFileInfoPtr);
		
		backgroundIndexOffset = clsfyVariablePtr->backgroundIndexOffset;
		
		nextClassToGroupIndex = 
				classPtr[classIndex+1] - 1 + clsfyVariablePtr->backgroundIndexOffset;
		
		if (nextClassToGroupIndex < fileNumberClasses)
			nextGroupIndex = classToGroupPtr[nextClassToGroupIndex];
			
		else	// nextClassToGroupIndex >= fileNumberClasses 
			nextGroupIndex = -2;
	
		if (nextGroupIndex != (SInt32)groupIndex)
			lastClassForGroupFlag = TRUE;
			
		}	// end "else classIndex < numberClasses-1"
		
	return (lastClassForGroupFlag);
	
}	// end "DetermineIfLastClassForGroup"



#if defined multispec_mac
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void DrawDialogClassAreaPopUp
//
//	Software purpose:	The purpose of this routine is to draw the class
//							area prompt and class area set selection for the class
//							area pop up menu.  This routine will also draw a drop
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
//	Coded By:			Larry L. Biehl			Date:  6/09/1993
//	Revised By:			Larry L. Biehl			Date:  6/09/1993	

pascal void DrawDialogClassAreaPopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{	
			// Use the generic pop up drawing routine.									
			
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpAllSubsetMenu, 
								gClassAreaSelection, 
								TRUE);
	
}	// end "DrawDialogClassAreaPopUp"
#endif	// defined multispec_mac 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ForceListResultsCodeResourceLoad
//
//	Software purpose:	The purpose of this routine is to force the 
//							'ListResults' code resource to be loaded into memory.  
//							It is called by 'ClassifyControl' in the classify.c file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 02/20/1991
//	Revised By:			Larry L. Biehl			Date: 02/20/1991

void ForceListResultsCodeResourceLoad (void)

{
			// If spare memory had to be used to load code resources, then exit	
			// routine.																				
			
	if (gMemoryTypeNeeded < 0)
																							return;
																							
			// Code resources loaded okay, so set flag back for non-Code			
			// resources.																			
			
	gMemoryTypeNeeded = 0;

}	// end "ForceListResultsCodeResourceLoad" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetAreaResults
//
//	Software purpose:	The purpose of this routine is to get the results
//							for the specified area and store them into memory.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			ListResultsAreasControl
//							ListResultsTrainTestFields
//
//	Coded By:			Larry L. Biehl			Date: 02/19/1991
//	Revised By:			Larry L. Biehl			Date: 02/16/2014

SInt16 GetAreaResults (
				SInt16								classPointer, 
				AreaDescriptionPtr				areaDescriptionPtr, 
				FileIOInstructionsPtr			fileIOInstructions1Ptr,
				FileIOInstructionsPtr			fileIOInstructions2Ptr,
				ClassifierVarPtr					clsfyVariablePtr, 
				HSInt64Ptr 							countVectorPtr)
					
{
	double								minutesLeft;
	
	FileInfoPtr							fileInfoPtr;
	
	HSInt64Ptr							thresholdClassCountPtr;
	
	HUInt16Ptr							ioBuffer2Ptr,
				 							probabilityBuffer2Ptr;
				 							
	Point									point;
	Ptr									stringPtr;
	RgnHandle							rgnHandle;	
			
	HUInt16Ptr	 						symbolToIndexPtr;
	
	SInt32								byteSkip,
											columnInterval,
											line,
											lineCount,
											lineEnd,
											lineInterval,
											linesLeft,
											numberSamples,
											sample,
											startTick;
	
	SInt16								classIndex,
											classNumber,
											errCode,
											returnCode,			// =0, no error			
																	// =1, no memory for 	
																	// 		text.				
																	// =2, disk file error.	
																	// =3, user quit.
											thresholdCode;
	
	Boolean								polygonFieldFlag,
											thresholdDataFlag,
											updateNumberLinesFlag;
	
	
			// Check some input variables.													
			
	if (areaDescriptionPtr->lineEnd <= 0 || areaDescriptionPtr->columnEnd <= 0)
																						return (0);
	
			// Initialize local variables	
			
	fileInfoPtr = 				fileIOInstructions1Ptr->fileInfoPtr;													
	
	classNumber	= 				areaDescriptionPtr->classNumber;
	columnInterval = 			areaDescriptionPtr->columnInterval;
	lineEnd = 					areaDescriptionPtr->lineEnd;
	lineInterval = 			areaDescriptionPtr->lineInterval;
	linesLeft = 				areaDescriptionPtr->numberLines;
	polygonFieldFlag = 		areaDescriptionPtr->polygonFieldFlag;
	rgnHandle = 				areaDescriptionPtr->rgnHandle;
	
	lineCount = 				0;
	returnCode = 				noErr;
	startTick = 				TickCount ();
	thresholdCode = 			gListResultsSpecsPtr->probabilityThresholdCode;
	thresholdClassCountPtr = &countVectorPtr[0];
	thresholdDataFlag = 		gListResultsSpecsPtr->thresholdFlag;
	symbolToIndexPtr = 		clsfyVariablePtr->symbolToClassPtr;
	
			// Get the byte skip.  Allow for the GAIA case which is two byte		
			// data.  The first byte is the probability byte which is not used.	
	
	byteSkip = columnInterval;
	if (fileInfoPtr->format == kGAIAType || fileInfoPtr->format == kGAIA2Type)
		byteSkip *= 2;
	
			// Get pointer to the probability map buffer incase it is needed.	
			
	probabilityBuffer2Ptr = (HUInt16Ptr)gOutputBuffer2Ptr;
	
			// Get number of samples to be read in one line of image data.			
	
	numberSamples = (areaDescriptionPtr->columnEnd - areaDescriptionPtr->columnStart +
						areaDescriptionPtr->columnInterval)/
											areaDescriptionPtr->columnInterval;
	areaDescriptionPtr->numSamplesPerChan = numberSamples;
																					  
			// Load total number of lines in status information.						
			
	if (classNumber == 0)
		gNextMinutesLeftTime = gNextTime + gNextStatusTimeOffset;
			
			// Load some of the File IO Instructions structure for the classified image
			// file that pertain to the specific area being used.
			
	SetUpFileIOInstructions (fileIOInstructions1Ptr,
										areaDescriptionPtr,
										1,
										NULL,
										kSetSpecialBILFlagFalse);
			
			// Load some of the File IO Instructions structure for the probability image
			// file that pertain to the specific area being used.
	
	if (thresholdDataFlag)		
		SetUpFileIOInstructions (fileIOInstructions2Ptr,
											areaDescriptionPtr,
											1,
											NULL,
											kSetSpecialBILFlagFalse);
											
	updateNumberLinesFlag = TRUE;

			// Loop through lines for area that was classified.							

	for (line=areaDescriptionPtr->lineStart; line<=lineEnd; line+=lineInterval)
		{
		ioBuffer2Ptr = (HUInt16Ptr)gOutputBufferPtr;
	
				// Display line status information.											
				
		lineCount++;
		if (TickCount () >= gNextStatusTime)
			{
			if (updateNumberLinesFlag)
				{
				LoadDItemValue (gStatusDialogPtr, 
										IDC_Status20,
										(SInt32)areaDescriptionPtr->numberLines);
				updateNumberLinesFlag = FALSE;
									
				}	// end "if (updateNumberLinesFlag)"
				
			LoadDItemValue (gStatusDialogPtr, IDC_Status18, lineCount);
			gNextStatusTime = TickCount () + gNextStatusTimeOffset;
			
			}	// end "if (TickCount () >= gNextStatusTime)"
		
				// Get line of classified image data. Return if there is a file IO error.					
				 
		errCode = GetLineOfData (fileIOInstructions1Ptr,
											line,
											(UInt32)areaDescriptionPtr->columnStart,
											(UInt32)areaDescriptionPtr->columnEnd,
											(UInt32)areaDescriptionPtr->columnInterval,
											gInputBufferPtr,
											gOutputBufferPtr);
			
		if (errCode < noErr)
			{
			returnCode = 2;
			break;
			
			}	// end "if (errCode < noErr)"
			
		if (thresholdDataFlag && errCode != kSkipLine)
			{ 
			errCode = GetLineOfData (fileIOInstructions2Ptr,
												line, 
												(UInt32)areaDescriptionPtr->columnStart,
												(UInt32)areaDescriptionPtr->columnEnd,
												(UInt32)areaDescriptionPtr->columnInterval,
												gInputBuffer2Ptr,
												gOutputBuffer2Ptr);	
			
			if (errCode < noErr)
				{
				returnCode = 2;
				break;
				
				}	// end "if (errCode < noErr)"
				
			}	// end "if (thresholdDataFlag && errCode != kSkipLine)"
			
		if (errCode != kSkipLine)
			{ 
					// Get the result for this line of data.								
			
			point.v = (SInt16)line;
			point.h = (SInt16)areaDescriptionPtr->columnStart;
				   
			numberSamples = fileIOInstructions1Ptr->numberOutputBufferSamples;
			   
			if (!thresholdDataFlag)
				{
				for (sample=0; sample<numberSamples; sample++)
					{
					if (!polygonFieldFlag || PtInRgn (point, rgnHandle))
						{
			   		classIndex = symbolToIndexPtr[*ioBuffer2Ptr];
						countVectorPtr[classIndex]++;
						
						}	// end "if (!polygonFieldFlag || PtInRgn (point, rgnHandle))" 
						
					point.h += (SInt16)columnInterval;
					ioBuffer2Ptr++;
					
					}	// end "for (sample=0; sample<count; sample++)" 
					
				}	// end "if (!thresholdDataFlag)" 
			   		
		   else	// thresholdDataFlag 
		   	{
				for (sample=0; sample<numberSamples; sample++)
					{
					if (!polygonFieldFlag || PtInRgn (point, rgnHandle))
						{
						if (probabilityBuffer2Ptr[sample] > (UInt16)thresholdCode)
							{
				   		classIndex = symbolToIndexPtr[*ioBuffer2Ptr];
							countVectorPtr[classIndex]++;
							
							}	// end "if (probabilityBufferPtr[sample] > ..." 
			   			
			   		else	// probabilityBufferPtr[sample] <= thresholdCode 
			   			(*thresholdClassCountPtr)++;
						
						}	// end "if (!polygonFieldFlag || PtInRgn (point, rgnHandle))" 
						
					point.h += (SInt16)columnInterval;
					ioBuffer2Ptr++;
		   			
		   		}	// end "for (sample=0; sample<count; sample++)" 
		   		
		   	}	// end "else thresholdDataFlag"
		   	
		   }	// end "if (errCode != kSkipLine)"
		
				// If this in an image area, update dialog status information.		
		
		if (classNumber == 0)
			{
			linesLeft--;
			
			if (TickCount () >= gNextMinutesLeftTime)
				{
				minutesLeft = (linesLeft * (TickCount () - startTick))/
															(double)(lineCount*kTicksPerMinute);
														
				sprintf ((char*)gTextString, " %.1f", minutesLeft);
				stringPtr = (char*)CtoPstring (gTextString, gTextString);
				LoadDItemString (gStatusDialogPtr, IDC_Status14, (Str255*)gTextString);
					
				gNextMinutesLeftTime = TickCount () + gNextMinutesLeftTimeOffset;
				
				}	// end "if (TickCount () >= gNextMinutesLeftTime)" 
				
			}	// end "if (classNumber == 0)" 
			
				// Exit routine if user has "command period" down						
				
		if (TickCount () >= gNextTime)
			{
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
				{
				returnCode = 3;
				break;
				
				}	// end "if (!CheckSomeEvents (..."
				
			}	// end "if (TickCount () >= gNextTime)" 
			
		if (fileIOInstructions1Ptr->maskBufferPtr != NULL)
			fileIOInstructions1Ptr->maskBufferPtr += 
												fileIOInstructions1Ptr->numberMaskColumnsPerLine;
										
		}	// end "for (line=areaDescriptionPtr->lineStart; line..." 
	
	if (returnCode == 0)	
		LoadDItemValue (gStatusDialogPtr, IDC_Status18, lineCount);
		
			// Close up any File IO Instructions structure that pertain to the 
			// specific area used.
			
	CloseUpFileIOInstructions (fileIOInstructions1Ptr, areaDescriptionPtr);
	
	if (thresholdDataFlag)
		CloseUpFileIOInstructions (fileIOInstructions2Ptr, areaDescriptionPtr);
		
	return (returnCode);
			
}	// end "GetAreaResults" 	  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32 GetClassGroupNameMaxLength
//
//	Software purpose:	The purpose of this routine is to get the length of the 
//							longest class or group name represented by the input file.
//		
//	Parameters in:				
//
//	Parameters out:			
//
// Value Returned:
//
//	Called by:			GetTableColumnWidths in SLstRslt.cpp
//							ListClassificationSummary in SLstRslt.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/31/1997
//	Revised By:			Larry L. Biehl			Date: 09/18/2006

UInt32 GetClassGroupNameMaxLength (
				FileInfoPtr	 						fileInfoPtr,
				Boolean								groupFlag,
				HUInt32Ptr							nameLengthVectorPtr)

{	
	Ptr									descriptionPtr = NULL;
	
	Handle								descriptionHandle;
								
	UInt32								index, 
	                       			maxNameLength,
	                       			numberNames;
									
	
	maxNameLength = 0;  
	                        
	if (groupFlag)
		{
		descriptionHandle = fileInfoPtr->groupNameHandle;
		numberNames = fileInfoPtr->numberGroups;
		
		}	// end "if (groupFlag)"
		
	else	// !groupFlag
		{
		descriptionHandle = fileInfoPtr->classDescriptionH;
		numberNames = fileInfoPtr->numberClasses;
		
		}	// end "else !groupFlag"
		
	descriptionPtr = (char*)GetHandlePointer (descriptionHandle);
		
	if (descriptionPtr != NULL)
		{
		for (index=0; index<numberNames; index++)
			{
			maxNameLength = MAX ((UInt32)descriptionPtr[0], maxNameLength);
			
			if (nameLengthVectorPtr != NULL)	
				nameLengthVectorPtr[index] = descriptionPtr[0];
				
			descriptionPtr += 32;
			
			}	// end "for (index=0; index<numberLines; index++)"
			
		}	// end "if (descriptionPtr != NULL)"
		
	return (maxNameLength);

}	// end "GetClassGroupNameMaxLength" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt64 GetClassOrGroupNumberOfSamples
//
//	Software purpose:	This routine get the total number of samples for the requested
//							class or group within the input class vector..
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	Total number of pixels in input class vector for the requested.
//							class or group.
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/29/1997
//	Revised By:			Larry L. Biehl			Date: 02/13/2014

SInt64 GetClassOrGroupNumberOfSamples (
				ClassifierVar*						clsfyVariablePtr,
				HSInt64Ptr							classCountVectorPtr,
				SInt32								countVectorIndex,
				Boolean								outputGroupedFlag,
				Boolean								inputGroupedFlag)

{
	SInt16*								classToGroupPtr;
	
	SInt64								totalNumberSamples;        
	                                         
	SInt32								groupIndex;
	
	UInt32								classIndex,
											fileNumberClasses; 
	
	
	totalNumberSamples = 0;
	if (countVectorIndex >= 0)
		{
		if (outputGroupedFlag && !inputGroupedFlag)
			{
					// Update the number of correct values.					
			
			fileNumberClasses = GetActiveNumberClasses ();
			classToGroupPtr = GetClassToGroupPointer (gImageFileInfoPtr);
			
			totalNumberSamples = 0;
			groupIndex = countVectorIndex-1;
			for (classIndex=0; 
					classIndex<fileNumberClasses; 
					classIndex++)
				{
				if (classToGroupPtr[classIndex] == groupIndex)
					totalNumberSamples += classCountVectorPtr[classIndex+1];
					
				}	// end "for (	classIndex=0; ..."
			
			}	// end "if (outputGroupedFlag && !inputGroupedFlag)"
			
		else	// !outputGroupedFlag || inputGroupedFlag
			{
			if ((UInt32)countVectorIndex < clsfyVariablePtr->tableWidth)
				totalNumberSamples = classCountVectorPtr[countVectorIndex];
				
			}	// end "else !outputGroupedFlag || inputGroupedFlag"
			
		}	// end "if (countVectorIndex >= 0)" 
		
	return (totalNumberSamples);
			
}	// end "GetClassOrGroupNumberOfSamples" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		double GetDefaultAreaConversionFactor
//
//	Software purpose:	This routine determines the conversion factor needed to change
//							the number of pixels to the user requested area units.
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/31/2002
//	Revised By:			Larry L. Biehl			Date: 10/31/2002

double GetDefaultAreaConversionFactor (
				Handle								mapProjectionHandle,
				Handle								imageWindowInfoHandle)

{
	double								areaConversionFactor,
											conversion;
	
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	SInt16								areaUnitsIndex,
											//displayAreaUnitsIndex,
											planarMapUnitsCode;
		

	areaConversionFactor = 0.;
	
	planarMapUnitsCode = GetPlanarMapUnitsCode (mapProjectionHandle);
	
	if (planarMapUnitsCode >= kKilometersCode)
		{
		mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																					mapProjectionHandle);
												
		areaConversionFactor = 
						fabs (mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize);
		areaConversionFactor *= 
						fabs (mapProjectionInfoPtr->planarCoordinate.verticalPixelSize);
		
				// Convert to hectares unless the display area units options is set
				// to something other than number of pixels.
		
		areaUnitsIndex = GetAreaUnits (imageWindowInfoHandle, FALSE);
		
		//if (displayAreaUnitsIndex >= kSqKilometersUnitsMenuItem)
		//	areaUnitsIndex = displayAreaUnitsIndex;
		
		areaUnitsIndex -= kSqKilometersUnitsMenuItem;
		
		conversion = gDistanceFileConversionFactors[
										planarMapUnitsCode - kKilometersCode] / 
												gDistanceDisplayConversionFactors[areaUnitsIndex];
												
		areaConversionFactor *= conversion * conversion;
	
		}	// end "if (planarMapUnitsCode >= kKilometersCode)"
		
	return (areaConversionFactor);
			
}	// end "GetDefaultAreaConversionFactor" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt32 GetCountVectorIndex
//
//	Software purpose:	This routine determines the count vector index that represents
//							the input class or group.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/29/1997
//	Revised By:			Larry L. Biehl			Date: 02/13/2014

SInt32 GetCountVectorIndex (
				ClassifierVar*						clsfyVariablePtr,
				SInt16*								classAreaPtr,
				UInt16								classIndex,
				Boolean								groupOrderedFlag)

{
	SInt32								countVectorIndex,
											groupIndex,
											statClassNumber;
	
				
			// If this is a group ordered table get a pointer to the class to group 
			//	vector and get proper pointer to the list of classes.		
			
	if (groupOrderedFlag)
		{	
		statClassNumber = classAreaPtr[classIndex] - 1;
		
		groupIndex = GetGroupIndex (clsfyVariablePtr, statClassNumber);
		
		countVectorIndex = 0;
		if (groupIndex >= 0)
			countVectorIndex = groupIndex + 1;
		
		}	// end "if (groupOrderedFlag)"
		
	else	// !groupOrderedFlag
		{	
		countVectorIndex = 
					classAreaPtr[classIndex] + clsfyVariablePtr->backgroundIndexOffset;
		
		}	// end "else !groupOrderedFlag"
		
	return (countVectorIndex);
			
}	// end "GetCountVectorIndex" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt32 GetGroupIndex
//
//	Software purpose:	This routine gets the file information for the
//							probability image that goes with the active image
//							window.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	Group Index code 
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/30/1997
//	Revised By:			Larry L. Biehl			Date: 06/23/1997

SInt32 GetGroupIndex (
				ClassifierVar*						clsfyVariablePtr,
				UInt32								statClassNumber)

{
	SInt16*								classToGroupPtr;
	
	SInt32								groupIndex = -2;
	
	UInt32								backgroundIndexOffset,
											fileNumberClasses;
	
				
	fileNumberClasses = GetActiveNumberClasses ();
	classToGroupPtr = GetClassToGroupPointer (gImageFileInfoPtr);
	
	backgroundIndexOffset = clsfyVariablePtr->backgroundIndexOffset;
				
	if (statClassNumber+backgroundIndexOffset < fileNumberClasses)
		groupIndex = (SInt32)classToGroupPtr[statClassNumber+backgroundIndexOffset];
		
	return (groupIndex);
			
}	// end "GetGroupIndex" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle GetProbabilityWindowInfoHandle
//
//	Software purpose:	This routine get the window information for the
//							probability image than goes with the active image
//							window.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	TRUE if 
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/11/1993
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

Handle GetProbabilityWindowInfoHandle (void)

{
	FileInfoPtr							compareFileInfoPtr;
	WindowInfoPtr						compareWindowInfoPtr;
	WindowPtr							windowPtr;
	
	Handle								compareFileInfoHandle,
											compareWindowInfoHandle;
								
	SInt16								returnCode,
											savedThresholdTypeCode,
											window,
											windowIndex;
											
	SignedByte							handleStatus1,
											handleStatus2;
		
	
	if (gNumberOfIWindows <= 1)
																				return (NULL);
																				
	gTextString3[0] = 0;
	
			// Copy the input file name.														

	FileStringPtr fileNamePtr = (FileStringPtr)GetFileNameCPointerFromFileInfo (
																					gImageFileInfoPtr);
	CtoPstring (fileNamePtr, gTextString3);
	
			// Remove the .GIS and other extensions if it exits at the end of
			// the name.																		
			          
	#if defined multispec_mac   
		RemoveCharsNoCase ((char*)"\0.gis\0", gTextString3);
		RemoveCharsNoCase ((char*)"\0.GAIA\0", gTextString3);
		RemoveCharsNoCase ((char*)"\0.echo_classify\0", gTextString3);
		RemoveCharsNoCase ((char*)"\0.maxlik_classify\0", gTextString3);
		RemoveCharsNoCase ((char*)"\0.tif\0", gTextString3);
	#endif	// defined multispec_mac
  		
   #if defined multispec_win | defined multispec_lin   
		RemoveCharsNoCase ((char*)"\0.gis\0", gTextString3);
		RemoveCharsNoCase ((char*)"\0.GAI\0", gTextString3);
		RemoveCharsNoCase ((char*)"\0.ech\0", gTextString3);
		RemoveCharsNoCase ((char*)"\0.max\0", gTextString3);
		RemoveCharsNoCase ((char*)"\0.tif\0", gTextString3);
	#endif	// defined multispec_win  
		                           
	if (gImageFileInfoPtr->format == kTIFFType ||
													gImageFileInfoPtr->format == kGeoTIFFType)
		ConcatFilenameSuffix ((FileStringPtr)gTextString3, (StringPtr)"\0Prob.tif\0");
	
	else
		ConcatFilenameSuffix ((FileStringPtr)gTextString3, (StringPtr)"\0Prob.gis\0");
	
			// We now have the default name.  Check if it matches any of
			// those images files that are open.
		
	window = 0;
	windowIndex = kImageWindowStart;
	returnCode = 1;
	savedThresholdTypeCode = gListResultsSpecsPtr->thresholdTypeCode;
	
	do
		{
		windowPtr = gWindowList[windowIndex];
			
		if (windowPtr != gActiveImageWindow)
			{                                                       
			compareWindowInfoHandle = (Handle)GetWindowInfoHandle (windowPtr);
			compareWindowInfoPtr = (WindowInfoPtr)GetHandleStatusAndPointer (
														compareWindowInfoHandle, &handleStatus1);
			
			compareFileInfoHandle = GetFileInfoHandle (compareWindowInfoHandle);
			compareFileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (
															compareFileInfoHandle, &handleStatus2);
			
			fileNamePtr = (FileStringPtr)GetFileNameCPointerFromFileInfo (
																					compareFileInfoPtr);
			returnCode = memcmp (fileNamePtr,
										(char*)&gTextString3[1],
										gTextString3[0]+1);
			
			if (returnCode == 0)
				{
				if (!compareFileInfoPtr->thematicType)
					returnCode = 1;
					
				if (compareFileInfoPtr->numberBytes != 1)
					returnCode = 1;
				
						// Determine which type of threshold image it is.
				
				gListResultsSpecsPtr->thresholdTypeCode = 0;
				if (compareFileInfoPtr->numberClasses == 
															(UInt32)gNumberProbabilityClasses+1)
					gListResultsSpecsPtr->thresholdTypeCode = kMaxLikeMode;
					
				else if (compareFileInfoPtr->numberClasses == 102+1)
					gListResultsSpecsPtr->thresholdTypeCode = kCEMMode;
					
				else if (compareFileInfoPtr->numberClasses == 91+1)
					gListResultsSpecsPtr->thresholdTypeCode = kCorrelationMode;
							
				if (gListResultsSpecsPtr->thresholdTypeCode == 0)
					returnCode = 1;
					
						// Now verify that the number of lines and columns are	
						// same.	
						
				if (gImageWindowInfoPtr->maxNumberColumns !=
															compareWindowInfoPtr->maxNumberColumns)
					returnCode = 1;
						
				if (gImageWindowInfoPtr->maxNumberLines !=
															compareWindowInfoPtr->maxNumberLines)
					returnCode = 1;
				
				}	// end "if (returnCode == 0)" 
				
			MHSetState (compareWindowInfoHandle, handleStatus1);
				
			MHSetState (compareFileInfoHandle, handleStatus2);
			
			}	// end "if (windowPtr != gActiveImageWindow)" 
			
		window++;
		windowIndex++;
														
		}		while (window<gNumberOfIWindows && returnCode != 0);
		
	if (returnCode == 0)
		{
		if (savedThresholdTypeCode != gListResultsSpecsPtr->thresholdTypeCode)
			{
					// Initialize the default threshold value.
			
			gListResultsSpecsPtr->probabilityThreshold = 2.0;
					
			if (gListResultsSpecsPtr->thresholdTypeCode == kCEMMode)
				gListResultsSpecsPtr->probabilityThreshold = .5;
				
			else if (gListResultsSpecsPtr->thresholdTypeCode == kCorrelationMode)
				gListResultsSpecsPtr->probabilityThreshold = 5.;
			
			}	// end "if (savedThresholdTypeCode != ...->thresholdTypeCode)"
		
		}	// end "if (returnCode == 0)"
		
	else	// returnCode != 0
		compareWindowInfoHandle = NULL;
		
	return (compareWindowInfoHandle);

}	// end "GetProbabilityWindowInfoHandle" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetListResultsStringBuffer
//
//	Software purpose:	The purpose of this routine is to obtain memory for
//							a string buffer to be used for creating a line
//							of results information.  The global variable
//							'gCharBufferPtr1' will be used.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			ListResultsControl in listResults.c
//							ClassifyAreasControl in classify.c
//
//	Coded By:			Larry L. Biehl			Date: 02/21/1991
//	Revised By:			Larry L. Biehl			Date: 06/23/1998

Boolean GetListResultsStringBuffer (
				FileInfoPtr							fileInfoPtr, 
				ClassifierVar*						clsfyVariablePtr, 
				SInt16								unitsCode, 
				SInt16								maxNumberColumns,
				SInt16*								classPtr,
				UInt32								numberClasses)
	
{
	SInt32								numberBytesNeeded;
											
	SInt16								lenNumber;
	
	
			// Get memory for a character buffer to be used in listing.  			
			// First get the number of digits for the maximum possible pixels		
			// in the image.																		
			
	GetTableColumnWidths (clsfyVariablePtr,
									classPtr,
									numberClasses);
	
	lenNumber = clsfyVariablePtr->classGroupNameWidth + 1;
		
	numberBytesNeeded = 41 + 7 + lenNumber + 5+1 + maxNumberColumns*lenNumber + 10;
	
			// Add some extra.																	
			
	numberBytesNeeded += 10;
	
			// Make certain that there is enough for the title lines.				
			
	numberBytesNeeded = MAX (250, numberBytesNeeded);
	gCharBufferPtr1 = (char*)MNewPointer (numberBytesNeeded);
																				
	return (gCharBufferPtr1 != NULL);
		
}	// end "GetListResultsStringBuffer" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		HUInt16Ptr GetSymbolToClassVector
//
//	Software purpose:	The purpose of this routine is to fill a symbol to
//							class vector.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			ListResultsControl
//
//	Coded By:			Larry L. Biehl			Date: 02/21/1991
//	Revised By:			Larry L. Biehl			Date: 09/18/2006

HUInt16Ptr GetSymbolToClassVector (
				FileInfoPtr							fileInfoPtr)

{
	HUInt16Ptr							lSymbolToClassPtr;
	
	SInt16								*classToGroupPtr;
											
	UInt16								*classSymbolPtr;
	
	SInt32								classGroupIndex,
											index,
											numberClasses;
											
	SInt16								tableType;
	
	
	
			// Check input variables.															
	
	if (fileInfoPtr->asciiSymbols && fileInfoPtr->classDescriptionH == NULL)
																					return (NULL);
		
			// Initialize local variables.													
	
	numberClasses = fileInfoPtr->numberClasses;
	tableType = gListResultsSpecsPtr->tableType;
	
	if (tableType == 2)
		{
				// Get the class to group pointer.  										
							
		classToGroupPtr = GetClassToGroupPointer (fileInfoPtr);
		
		}	// end "if (tableType == 2)" 
	
			// Get pointer to memory for symbol to class vector.						
	
	lSymbolToClassPtr = (HUInt16Ptr)MNewPointerClear (
									(SInt32)fileInfoPtr->numberBins * sizeof (SInt16));
				
	if (lSymbolToClassPtr == NULL)
																					return (NULL);
	
	if (fileInfoPtr->asciiSymbols)
		{
				// Get pointer to class symbol table.										
				// Remember to take into account that classSymbolPtr 					
				// points to a short int vector.	Str16 is used instead of			
				// Str31.																			
		
		classSymbolPtr = (UInt16*)GetHandlePointer (fileInfoPtr->classDescriptionH);
		classSymbolPtr = &classSymbolPtr[fileInfoPtr->numberClasses*sizeof (Str15)];
			
				// Set up vector for the class symbols.									
			
		for (index=0; index<numberClasses; index++)
			{
			if (tableType == 1)
				classGroupIndex = index + 1;
			
			else	// tableType == 2 
				classGroupIndex = classToGroupPtr[index] + 1;
			
			lSymbolToClassPtr[classSymbolPtr[index]] = (SInt16)classGroupIndex;
			
			}	// end "for (index=0; index<fileInfoPtr->..." 
		
		}	// end "if (fileInfoPtr->asciiSymbols)" 
		
	else	// !fileInfoPtr->asciiSymbols 
		{
		for (index=0; index<numberClasses; index++)
			{		
			if (tableType == 1)
				classGroupIndex = index + 1;
		
			else	// tableType == 2 
				classGroupIndex = classToGroupPtr[index] + 1;

			lSymbolToClassPtr[index] = (SInt16)classGroupIndex;
			
			}	// end "for (index=0; index<numberClasses; index++)" 
		
		}	// end "else !fileInfoPtr->asciiSymbols" 
		
	return (lSymbolToClassPtr);
			
}	// end "GetSymbolToClassVector" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetTableColumnWidths
//
//	Software purpose:	The purpose of this routine is to determine the column
//							width to use to ensure enough space for the values.  The
//							routine will also set up the precision value for those
//							units requiring real numbers.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			GetListResultsStringBuffer in SLstRslt.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/26/1991
//	Revised By:			Larry L. Biehl			Date: 12/21/2016	

void GetTableColumnWidths (
				ClassifierVar*						clsfyVariablePtr,
				SInt16*								classPtr,
				UInt32								numberClasses)

{
	HUInt32Ptr							columnWidthVectorPtr;
	
	SInt32								maxNumberPoints;
	
	UInt32								index; 
								
	SInt16								maxNameLength;
	
	
			// Get the maximum number of possible pixels to determine		
			// the width the columns need to be.									
			
	maxNumberPoints = (SInt32)gImageWindowInfoPtr->maxNumberLines * 
														gImageWindowInfoPtr->maxNumberColumns;
											
	NumToString ((UInt32)maxNumberPoints, (UCharPtr)gTextString);
	clsfyVariablePtr->sampleNumberWidth = gTextString[0];
							
	if (clsfyVariablePtr->unitsCode == kSampleUnits)
		{                       
				// Make sure that there is space for the reliability accuracy.
				     
		clsfyVariablePtr->numberWidth = clsfyVariablePtr->sampleNumberWidth;
		clsfyVariablePtr->numberWidth = MAX (clsfyVariablePtr->numberWidth, 5);
		
		clsfyVariablePtr->numberPrecision = 0;
		
		}	// end "if (clsfyVariablePtr->unitsCode == kSampleUnits)" 
							
	if (clsfyVariablePtr->unitsCode == kPercentUnits)
		{
		clsfyVariablePtr->numberWidth = 6;
		clsfyVariablePtr->numberPrecision = 1;

		}	// end "if (clsfyVariablePtr->unitsCode == kPercentUnits)" 
							
	if (clsfyVariablePtr->unitsCode == kAreaUnits || 
									clsfyVariablePtr->summaryUnitsCode == kAreaUnits)
		{
		GetAreaNumberWidths (clsfyVariablePtr->variable1,
										maxNumberPoints,
										&clsfyVariablePtr->areaNumberWidth,
										&clsfyVariablePtr->numberPrecision);
		
				// This will be the default number width for the tables.
															
		clsfyVariablePtr->numberWidth = clsfyVariablePtr->areaNumberWidth;
		
		}	// end "if (clsfyVariablePtr->unitsCode == kAreaUnits || ..." 
		
			// Now get the width of the column names and the length of the longest name.
	
	columnWidthVectorPtr = clsfyVariablePtr->columnWidthVectorPtr;
	
			// Initialize the column width vector to all zeroes.
			
	for (index=0; index<clsfyVariablePtr->tableWidth; index++)
		columnWidthVectorPtr[index] = 0;
		
			// Set the threshold column width.
			
	columnWidthVectorPtr[0] = kThresholdNumberWidth;
	
	maxNameLength = kMinTableWidth;
		
	if (classPtr != NULL)
		{		
		if (gProcessorCode == kClassifyProcessor)
			maxNameLength = (SInt16)GetClassNameMaxLength (numberClasses, 
																			classPtr,
																			&columnWidthVectorPtr[1]);
				
		else	// gProcessorCode != kClassifyProcessor 
			maxNameLength = (SInt16)GetClassGroupNameMaxLength (
														gImageFileInfoPtr, 
														(clsfyVariablePtr->tableType == kGroupTable),
														&columnWidthVectorPtr[1]);
			
		}	// end "if (classPtr != NULL)" 
			
			// Use kMaxClassGroupNameWidth as the maximum for the length of the name
			// that will be listed in the title line.
				
	maxNameLength = MIN (maxNameLength, kMaxClassGroupNameWidth);
	for (index=0; index<clsfyVariablePtr->tableWidth; index++)
		columnWidthVectorPtr[index] = 
									MIN (columnWidthVectorPtr[index], kMaxClassGroupNameWidth);
	
			// Now make sure that the column width can handle the length of the number
			
	maxNameLength = MAX (maxNameLength, clsfyVariablePtr->numberWidth);
	for (index=0; index<clsfyVariablePtr->tableWidth; index++)
		columnWidthVectorPtr[index] = 
					MAX (columnWidthVectorPtr[index], (UInt32)clsfyVariablePtr->numberWidth);
								
			// Now make sure that the column width is at 'kMinTableWidth' columns.
			
	maxNameLength = MAX (maxNameLength, kMinTableWidth);
	for (index=0; index<clsfyVariablePtr->tableWidth; index++)
		columnWidthVectorPtr[index] = 
								MAX (columnWidthVectorPtr[index], kMinTableWidth);
		
	clsfyVariablePtr->classGroupNameWidth = maxNameLength;
	
}	// end "GetTableColumnWidths"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListAreaValue
//
//	Software purpose:	The purpose of this routine is to list the area value within
//							the specific classification summary line.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			ListClassificationSummary in listResults.c
//
//	Coded By:			Larry L. Biehl			Date: 11/01/2002
//	Revised By:			Larry L. Biehl			Date: 11/01/2002

SInt16 ListAreaValue (
				char*									stringPtr,
				ClassifierVar*						clsfyVariablePtr,
				double								area,
				SInt16								strLength,
				SInt16								decimalPlaces,
				SInt16								areaWidthWithCommas)

{  
	SInt16								numberChars;
	
			
	numberChars = sprintf (&stringPtr[strLength],  
										"\t ");
	strLength += numberChars;
	
	numberChars = sprintf (&stringPtr[strLength],  
									"%*.*f",
									clsfyVariablePtr->areaNumberWidth,
									clsfyVariablePtr->numberPrecision,
									area);
		
	numberChars = InsertCommasInNumberString (&stringPtr[strLength],
															numberChars, 
															decimalPlaces,
															areaWidthWithCommas);
	strLength += numberChars;
										
	return (strLength);
	
}	// end "ListAreaValue"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListClassificationSummary
//
//	Software purpose:	The purpose of this routine is to list a table
//							containing a summary of the number of pixels
//							classified into each class for an image or
//							a selected portion of an image.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			ClassifyAreasControl in classify.c
//							ListResultsAreasControl in listResults.c
//
//	Coded By:			Larry L. Biehl			Date: 03/11/1989
//	Revised By:			Larry L. Biehl			Date: 03/27/2012

Boolean ListClassificationSummary (
				ClassifierVar*						clsfyVariablePtr, 
				CMFileStream*						outputFileStreamPtr, 
				SInt16*								outputCodePtr,
				SInt16*								classPtr, 
				UInt32								numberClasses)

{  
	double								area,
											areaConversionFactor,
											percent;
	
	Ptr									descriptionPtr = NULL,
											namePtr; 
		
	SInt64								count,
											minimumNumberPixels,
											totalNumberPixels;						
	
	SInt16*								columnClassPtr; 
		
	HSInt64Ptr				 			countVectorPtr;
	
	Handle								descriptionHandle;
									
	UInt32								classIndex, 
	                       			classNumber,
	                       			index,
	                       			numberLines,
											statClassNumber;
	                                        
	SInt16								areaWidthWithCommas,
											classStorage,
											columnWidth,
											decimalPlaces,
											maxNameLength,
											numberChars,
											percentPrecision,
											percentWidth,
											sampleNumberWidth,
											samplesWidthWithCommas,
											strLength,
											tableType,
											summaryUnitsCode; 
									              
	Boolean								continueFlag,
											thresholdFlag;  
	
	
			// Initialize local variables.													
			                                            
	columnWidth = clsfyVariablePtr->numberWidth;
	countVectorPtr = clsfyVariablePtr->countVectorPtr;
	tableType = clsfyVariablePtr->tableType;
	thresholdFlag = clsfyVariablePtr->thresholdFlag; 
	summaryUnitsCode = clsfyVariablePtr->summaryUnitsCode;
	areaConversionFactor = clsfyVariablePtr->variable1;
	
	if (gProcessorCode == kClassifyProcessor)
		{
		numberLines = numberClasses;
		columnClassPtr = classPtr;
	
		}	// end "if (gProcessorCode == kClassifyProcessor)" 
		
	else	// gProcessorCode != kClassifyProcessor 
		{
		numberLines = clsfyVariablePtr->tableWidth - 1;
		columnClassPtr = NULL;
	
		}	// end "else gProcessorCode != kClassifyProcessor" 
	
			// Set up structure value.															
			
	clsfyVariablePtr->areaCode = kAreaType; 
	
			// Get a pointer to the class name. Also ...
			// Get the length of the longest class or group name.
			// Force the minimum name length to be 12 characters which allows
			// for the Threshold class name and the title for the column.
													
	maxNameLength = 0;							
	if (gProcessorCode == kListResultsProcessor)
		{  
		maxNameLength = (SInt16)GetClassGroupNameMaxLength (
									gImageFileInfoPtr, (tableType == kGroupTable), NULL);
			
		if (maxNameLength == 0)
																						return (FALSE); 
																						
		}	// end "if (gProcessorCode == kListResultsProcessor)"
		
	else	// gProcessorCode == kClassifyProcessor
		maxNameLength = (SInt16)GetClassNameMaxLength (numberClasses, classPtr, NULL);
		
	maxNameLength = MAX (12, maxNameLength);
	clsfyVariablePtr->classGroupNameWidth = maxNameLength;
																				
			// Get the total number of pixels that were classified and the
			// minimum number of pixels in the category listing.				
	
	totalNumberPixels = 0;
	minimumNumberPixels = SInt64_MAX;
	
	for (classIndex=0; classIndex<numberLines; classIndex++)
		{
		if (columnClassPtr != NULL)
			index = classPtr[classIndex];
		else	// columnClassPtr == NULL
			index = classIndex+1;
		
		totalNumberPixels += countVectorPtr[index];
		
		if (countVectorPtr[index] > 0)
			minimumNumberPixels = MIN (minimumNumberPixels, countVectorPtr[index]);
		
		}	// end "for (classIndex=0; classIndex<numberLines; classIndex++)"
		
	if (thresholdFlag)
		{
		totalNumberPixels += countVectorPtr[0];
		
		if (countVectorPtr[0] > 0)
			minimumNumberPixels = MIN (minimumNumberPixels, countVectorPtr[0]);
		
		}	// end "if (thresholdFlag)"
		
	if (minimumNumberPixels == SInt32_MAX)
		minimumNumberPixels = 1;
		
		// Get some variables needed for listing the number of pixels in text output window.
			
	sampleNumberWidth = sprintf ((char*)gTextString, "%lld", totalNumberPixels);
											
	samplesWidthWithCommas = (sampleNumberWidth-1)/3;
	samplesWidthWithCommas += sampleNumberWidth;
	
	samplesWidthWithCommas = MAX (samplesWidthWithCommas, 8);
				
			// Get the number width and precision for the percent values.
			
	percentPrecision =
		sprintf ((char*)gTextString, "%lld", totalNumberPixels/minimumNumberPixels);
	
			// Allow for two digits represented by percent itself but still allow one
			// more decimal than what is significant for user to have for interpretation.
				
	percentPrecision -= 1;
	
			// Minimum number of decimal digits is 1; Maximum number is 7.
	
	percentPrecision = MAX (1, percentPrecision);
	percentPrecision = MIN (7, percentPrecision);
	
	percentWidth = 6 + percentPrecision;
	
			// Write Table Title.																

	continueFlag = LoadListResultsTitle (clsfyVariablePtr, 
														outputFileStreamPtr, 
														columnClassPtr, 
														numberLines,
														samplesWidthWithCommas,
														percentWidth, 
														outputCodePtr);
									
	if (!continueFlag)
																				return (FALSE);

			// List class or group data.
				
	if (gProcessorCode == kListResultsProcessor)
		{ 
				// Get the description pointer for the List Results Processor.
				 								                                 
		if (tableType == kClassTable)
			descriptionHandle = gImageFileInfoPtr->classDescriptionH;
			
		else	// tableType == kGroupTable) 
			descriptionHandle = gImageFileInfoPtr->groupNameHandle;
			
		descriptionPtr = (char*)GetHandlePointer (descriptionHandle);
												
		}	// end "if (gProcessorCode == kListResultsProcessor)"
		
			// Get some variables needed for listing in text output window.
												
	if (summaryUnitsCode == kAreaUnits)
		{
		areaWidthWithCommas = clsfyVariablePtr->areaNumberWidth - 
															clsfyVariablePtr->numberPrecision;
		if (clsfyVariablePtr->numberPrecision > 0)
			areaWidthWithCommas--;
			
		areaWidthWithCommas = (areaWidthWithCommas-1)/3;
		areaWidthWithCommas += clsfyVariablePtr->areaNumberWidth;
			
		decimalPlaces = clsfyVariablePtr->numberPrecision;
		if (decimalPlaces == 0)
			decimalPlaces = -1;
		
		}	// end "if (summaryUnitsCode == kAreaUnits)"
			
	for (classIndex=0; classIndex<numberLines; classIndex++)
		{
		if (classPtr != NULL)
			{ 		    
					// Handle project class name.	
					
			classNumber = classPtr[classIndex]; 												
					
			statClassNumber = classNumber - 1;
			
					// Get the class storage number.												
							
			classStorage = gProjectInfoPtr->storageClass[statClassNumber];
			
			namePtr = (char*)&gProjectInfoPtr->classNamesPtr[classStorage].name;
			
			}	// end "if (classPtr != NULL)"
			
		else	// classPtr == NULL
			{
			classNumber = classIndex+1;
			namePtr = descriptionPtr;
			descriptionPtr += 32;
			
			}	// end "else classPtr == NULL" 
					
				// Load the class name into a string.					
		
		pstr ((char*)gTextString2, namePtr, &strLength);
			
		count = countVectorPtr[classNumber];
		
		percent = 0;
		if (totalNumberPixels > 0)
			percent = (double)100*count/totalNumberPixels;
		/*
		strLength = sprintf (&gCharBufferPtr1[strLength],
									"%5ld\t%*s\t%*ld\t%*.*f",
									classNumber,
									-maxNameLength,
									gTextString2,
									sampleNumberWidth, 
									count,
									percentWidth,
									percentPrecision,
									(float)percent);
		*/
		strLength = sprintf (gCharBufferPtr1,  
									"%5ld\t%*s",
									classNumber,
									-maxNameLength,
									gTextString2);

		strLength = ListCountValue (gCharBufferPtr1,
												count,
												sampleNumberWidth,
												strLength,
												samplesWidthWithCommas,
												kIncludeTab);
		
		numberChars = sprintf (&gCharBufferPtr1[strLength],  
										"\t%*.*f",
										percentWidth,
										percentPrecision,
										(float)percent);
		strLength += numberChars;
												
		if (summaryUnitsCode == kAreaUnits)
			{
			area = (double)count * areaConversionFactor;

			strLength = ListAreaValue (gCharBufferPtr1,
													clsfyVariablePtr,
													area,
													strLength,
													decimalPlaces,
													areaWidthWithCommas);
													
			}	// end "if (summaryUnitsCode == kAreaUnits)"
			
		numberChars = sprintf (&gCharBufferPtr1[strLength],  
										"%s",
										gEndOfLine);
		strLength += numberChars;
								
		continueFlag = OutputString (outputFileStreamPtr, 
												gCharBufferPtr1, 
												strLength, 
												*outputCodePtr, 
												continueFlag);

		}	// end "for (classIndex=0; classIndex<..." 
		
	if (thresholdFlag)
		{
		percent = 0;
		if (totalNumberPixels > 0)
			percent = (double)100*countVectorPtr[0]/totalNumberPixels;
		/*
		strLength = sprintf (gCharBufferPtr1,  
										"     \t%*s\t%*ld\t%*.*f",
										-maxNameLength,
										"Threshold",
										sampleNumberWidth, 
										countVectorPtr[0],
										percentWidth,
										percentPrecision,
										(float)percent);   
		*/
		strLength = sprintf (gCharBufferPtr1,  
										"     \t%*s",
										-maxNameLength,
										"Threshold");

		strLength = ListCountValue (gCharBufferPtr1,
												countVectorPtr[0],
												sampleNumberWidth,
												strLength,
												samplesWidthWithCommas,
												kIncludeTab);
												
		numberChars = sprintf (&gCharBufferPtr1[strLength],  
										"\t%*.*f",
										percentWidth,
										percentPrecision,
										(float)percent);
		strLength += numberChars;
												
		if (summaryUnitsCode == kAreaUnits)
			{
			area = (double)countVectorPtr[0] * areaConversionFactor;
			strLength = ListAreaValue (gCharBufferPtr1,
													clsfyVariablePtr,
													area,
													strLength,
													decimalPlaces,
													areaWidthWithCommas);
			
			}	// end "if (summaryUnitsCode == kAreaUnits)"
			
		numberChars = sprintf (&gCharBufferPtr1[strLength],  
										"%s",
										gEndOfLine);
		strLength += numberChars;
								
		continueFlag = OutputString (outputFileStreamPtr, 
												gCharBufferPtr1, 
												strLength, 
												*outputCodePtr, 
												continueFlag);
		
		}	// end "if (thresholdFlag)"
	
			// Load the total number of samples into the string to be printed.
	/*
	strLength = sprintf (gCharBufferPtr1,  
								"%s     \t%*s\t%*ld\t%*.*f",
								gEndOfLine,
								-maxNameLength,
								"Total",
								sampleNumberWidth, 
								totalNumberPixels,
								percentWidth,
								percentPrecision,
								(float)100,
								gEndOfLine);   
	*/
	strLength = sprintf (gCharBufferPtr1,
								"%s     \t%*s",
								gEndOfLine,
								-maxNameLength,
								"Total");

	strLength = ListCountValue (gCharBufferPtr1,
											totalNumberPixels,
											sampleNumberWidth,
											strLength,
											samplesWidthWithCommas,
											kIncludeTab);
											
	numberChars = sprintf (&gCharBufferPtr1[strLength],  
									"\t%*.*f",
									percentWidth,
									percentPrecision,
									(float)100);
	strLength += numberChars; 
												
	if (summaryUnitsCode == kAreaUnits)
		{
		area = (double)totalNumberPixels * areaConversionFactor;

		strLength = ListAreaValue (gCharBufferPtr1,
												clsfyVariablePtr,
												area,
												strLength,
												decimalPlaces,
												areaWidthWithCommas);
			
		}	// end "if (summaryUnitsCode == kAreaCode)"
			
	numberChars = sprintf (&gCharBufferPtr1[strLength],  
									"%s%s",
									gEndOfLine,
									gEndOfLine);
	strLength += numberChars;
							
	continueFlag = OutputString (outputFileStreamPtr, 
											gCharBufferPtr1, 
											strLength, 
											*outputCodePtr, 
											continueFlag);
								
	if (continueFlag &&
					gProcessorCode == kClassifyProcessor && 
									gClassifySpecsPtr->createThresholdTableFlag)
		{
		continueFlag = ListAverageDiscriminantValue (outputFileStreamPtr,
																	*outputCodePtr,
																	totalNumberPixels,
																	gTempDoubleVariable1);
		
		}	// end "if (continueFlag && ...createThresholdTable && ...)"
	
	return (continueFlag);

}	// end "ListClassificationSummary"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListFCGPerformanceTable
//
//	Software purpose:	The purpose of this routine is to list a table
//							containing the field, class, or group performance 
//							"classification" results as requested.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/26/1991
//	Revised By:			Larry L. Biehl			Date: 02/07/2018

Boolean ListFCGPerformanceTable (
				ClassifierVar*						clsfyVariablePtr, 
				SInt16								fieldType, 
				CMFileStream*						outputFileStreamPtr, 
				SInt16*								outputCodePtr, 
				SInt16*								classPtr, 
				UInt32								numberClasses, 
				SInt16*								classAreaPtr, 
				UInt32								numberClassAreas,
				UInt32								numberTableListColumns)

{                                        
	SInt64								totalNumberClassSamples,
											totalNumberSamples;
	
	HPClassNamesPtr					classNamesPtr;
	HPFieldIdentifiersPtr			fieldIdentPtr;
	
	char									*groupNamePtr,
											*localGroupNamePtr;
	                      
	SInt16								*classToGroupPtr,
											*columnClassPtr; 

	HSInt64Ptr							agreementMatrixPtr,
											classifySubTotalVectorPtr,
				 							classifyTotalVectorPtr,
				 							correctVectorPtr,
				 							countVectorPtr;
											
	SInt32								classifyVectorIndex;
											
	UInt32								backgroundIndexOffset,
											classifyIndex,
											classIndex,
											column,
											lastClass,
											numberColumns,
											numberKappaColumns,
											statClassNumber,  
											vectorIndex;
									
	SInt16								classifyMode,
											classStorage,
											fieldNumber,
											groupIndex,
											maxNameLength;
	
	Boolean								continueFlag,
											countTotalFlag,
											groupOrderedFlag,
											groupSummaryClassTableFlag,
											initializeFlag,
											listGroupSummaryFlag;
	
	
			// Initialize local variables.													
	
	classNamesPtr = gProjectInfoPtr->classNamesPtr;
	
	classifyTotalVectorPtr = &clsfyVariablePtr->countVectorPtr[
											clsfyVariablePtr->totalCountIndexStart];
	classifySubTotalVectorPtr = &classifyTotalVectorPtr[clsfyVariablePtr->tableWidth];
	correctVectorPtr = &classifyTotalVectorPtr[3*clsfyVariablePtr->tableWidth];
	agreementMatrixPtr = &classifyTotalVectorPtr[4*clsfyVariablePtr->tableWidth];
	
	countTotalFlag = (clsfyVariablePtr->totalNumberSamples == 0);
	initializeFlag = TRUE;
	
			// Determine if this is a group summary and class table.
				
	groupSummaryClassTableFlag = (clsfyVariablePtr->summaryCode == kGroupSummary &&
														clsfyVariablePtr->tableType == kClassTable);
	
	if (gProcessorCode == kClassifyProcessor)
		{
		columnClassPtr = classPtr;
		lastClass = numberColumns = numberClasses;
		
		classifyMode = gClassifySpecsPtr->mode;
		
		}	// end "if (gProcessorCode == kClassifyProcessor)" 
		
	else	// gProcessorCode != kClassifyProcessor 
		{
		columnClassPtr = NULL;
		lastClass = numberColumns = (UInt16)(clsfyVariablePtr->tableWidth - 1);
		
		classifyMode = 0;
		
		}	// end "else gProcessorCode != kClassifyProcessor"
	
			// If the data has been thresholded then indicate that there is another
			// column to list.
				
	if (clsfyVariablePtr->thresholdFlag)
		numberColumns++;
		
			// Set up Kappa Statistics variables.
			
	numberKappaColumns = clsfyVariablePtr->tableWidth - 1;
	if (groupSummaryClassTableFlag)
		numberKappaColumns = gImageFileInfoPtr->numberGroups;
	
			// Initialize agreement matrix.
			
	for (classIndex=0; classIndex<numberColumns; classIndex++)
		{		
		for (column=0; column<clsfyVariablePtr->tableWidth; column++)
			agreementMatrixPtr[column] = 0;
			
		agreementMatrixPtr += clsfyVariablePtr->tableWidth;
			
		}	// end "for (classIndex=0; classIndex<numberColumns; classIndex++)"
		
			// If a background class exists in the data, which it might for thematic
			// images then set the offset to use to get to the start of the class
			// information.
			
	backgroundIndexOffset = clsfyVariablePtr->backgroundIndexOffset;
	
			// Determine if this is a group summary and class table.
				
	groupSummaryClassTableFlag = (clsfyVariablePtr->summaryCode == kGroupSummary &&
														clsfyVariablePtr->tableType == kClassTable);
	
			// Determine if this is a group ordered table.								
			// If so get a pointer to the class to group vector and get				
			// proper pointer to the list of classes.										
			
	groupOrderedFlag = (clsfyVariablePtr->tableType == kGroupTable ||
											clsfyVariablePtr->summaryCode == kGroupSummary);
	
	if (groupOrderedFlag)
		{
		classToGroupPtr = GetClassToGroupPointer (gImageFileInfoPtr);
	
				// Get a pointer to the group names information.						
				
		groupNamePtr = (char*)GetHandlePointer (gImageFileInfoPtr->groupNameHandle);
		
				// Get pointer to the properly ordered list of classes.				
		
		classPtr = &classPtr[gProjectInfoPtr->numberStatisticsClasses];
		
				// All classes will be used if a group list is requested.			
				
		classAreaPtr = classPtr;
		
		}	// end "if (groupOrderedFlag)"
		
	classifyIndex = 0;
	
			// Initialize variables for total column pixel counts.					
			
	for (classIndex=0; classIndex<(UInt16)clsfyVariablePtr->tableWidth; classIndex++)
		{
		classifyTotalVectorPtr[classIndex] = 0;
		correctVectorPtr[classIndex] = 0;
		
		}	// end "for (classIndex=0; ..."
	
			// Get the length of the longest class or group name which will be listed in
			// the first column. The column minimum will be 16 and the maximum will be the
			// length of the longest name.
		
   maxNameLength = 0;
	if (classPtr != NULL)
		{
		if (clsfyVariablePtr->summaryCode != kGroupSummary)
			maxNameLength = (SInt16)GetClassNameMaxLength (numberClassAreas,
																			classAreaPtr,
																			NULL);
			
		else	// clsfyVariablePtr->summaryCode == kGroupSummary 
			maxNameLength = (SInt16)GetClassGroupNameMaxLength (
											gImageFileInfoPtr, 
											(clsfyVariablePtr->summaryCode == kGroupSummary),
											NULL);
			
		}	// end "if (classPtr != NULL)" 
		
	maxNameLength = MAX (16, maxNameLength);
	clsfyVariablePtr->classGroupNameWidth = maxNameLength;
	
			// Write Table Title.																

	continueFlag = LoadListResultsTitle (clsfyVariablePtr, 
														outputFileStreamPtr, 
														columnClassPtr, 
														numberTableListColumns,
														0,
														0, 
														outputCodePtr);
											
	if (!continueFlag)
																					return (FALSE);
		
			// List results by field, class or group for the specificied field 	
			// depending upon the requested grouping.										
	
	for (classIndex=0; classIndex<numberClassAreas; classIndex++)
		{
		if (initializeFlag)
			{
					// Initialize variables for total pixel counts for next group.	
				
			for (column=0; column<(UInt16)clsfyVariablePtr->tableWidth; column++)
				classifySubTotalVectorPtr[column] = 0;
								
			totalNumberSamples = 0;
			
			initializeFlag = FALSE;
			
			}	// end "if (initializeFlag)" 
			
		statClassNumber = classAreaPtr[classIndex] - 1;
		
				// Make certain that project class is within the thematic image	
				// file classes.																	
				
		//if (clsfyVariablePtr->summaryCode == kGroupSummary &&
		//		statClassNumber+backgroundIndexOffset >= gImageFileInfoPtr->numberClasses)
		//	break;
		
				// Get the class storage number.												
						
		classStorage = gProjectInfoPtr->storageClass[statClassNumber];
		
		fieldNumber = classNamesPtr[classStorage].firstFieldNumber;
		
				// Make certain that the countClassIndexPtr has been initialized.	
				// It may not be if the user cancelled before the statistics 		
				// were obtained.																	
				
		if (clsfyVariablePtr->countClassIndexPtr[statClassNumber] < 0)
			break;
		
				// Initialize count vector pointer for the class.						
			
		countVectorPtr = &clsfyVariablePtr->countVectorPtr[
									clsfyVariablePtr->countClassIndexPtr[statClassNumber]];
				
				// Get the class or group index into the count array.
				
		classifyVectorIndex = GetCountVectorIndex (clsfyVariablePtr,
																	classAreaPtr,
																	(UInt16)classIndex,
																	groupOrderedFlag);
		
		totalNumberClassSamples = 0;
		
		while (fieldNumber != -1 && continueFlag)
			{
			fieldIdentPtr = &gProjectInfoPtr->fieldIdentPtr[fieldNumber];
	
					// Make certain that field is field type.								
					
			if (fieldIdentPtr->fieldType == fieldType)
				{
						// Do not include cluster fields in the total as the 			
						// information does not exist as to where the pixels 			
						// making up field came from.											
				
				if (fieldIdentPtr->pointType < kClusterType ||
									(fieldIdentPtr->pointType == kMaskType && 
																	classifyMode != kEchoMode))
					{
							// Update the total for the class/group and total for all 		
							// classes or groups.																
					
					vectorIndex = 1;
					for (column=0; column<numberColumns; column++)
						{
						if (columnClassPtr)
							vectorIndex = columnClassPtr[column];
						
								// Handle the thresholdASCIIFlag case.						
								
						if (column == lastClass)
							vectorIndex = 0;
						
						classifySubTotalVectorPtr[vectorIndex] += 
																	countVectorPtr[vectorIndex];
						classifyTotalVectorPtr[vectorIndex] += 
																	countVectorPtr[vectorIndex];
															
						vectorIndex++;
		
						}	// end "for (column=0; column<..."
						
							// Update count for the total number of samples in the 	
							// class.																
				
					totalNumberSamples += fieldIdentPtr->numberPixels;
					totalNumberClassSamples += fieldIdentPtr->numberPixels;
						
					}	// end "if (fieldIdentPtr->pointType < ..." 
					
							// List the line of data.											
						
				if (clsfyVariablePtr->summaryCode == kFieldSummary)
					{	
					if (!ListLineOfResults (clsfyVariablePtr, 
													countVectorPtr, 
													totalNumberSamples, 
													columnClassPtr, 
													numberTableListColumns, 
													(SInt16)classifyVectorIndex, 
													(char*)&fieldIdentPtr->name, 
													outputFileStreamPtr, 
													outputCodePtr, 
													countTotalFlag))
																		 			return (FALSE);
																		 			
					totalNumberSamples = 0;
																		 			
					}	// end "if (...->summaryCode == kFieldSummary)" 
													
						// Update count vector to be ready for next field.				
						
				if (fieldIdentPtr->pointType != kClusterType)
					countVectorPtr += clsfyVariablePtr->tableWidth;
								
				}	// end "if (fieldIdentPtr->field..." 
				
			fieldNumber = fieldIdentPtr->nextField;
			
			}	// end "while (fieldNumber != -1 && continueFlag)"
			
		if (clsfyVariablePtr->summaryCode == kFieldSummary)
			initializeFlag = TRUE;
		
		if (clsfyVariablePtr->summaryCode == kClassSummary)
			{
			if (!ListLineOfResults (clsfyVariablePtr, 
											classifySubTotalVectorPtr, 
											totalNumberSamples, 
											columnClassPtr, 
											numberTableListColumns, 
											(SInt16)classifyVectorIndex, 
											(char*)&classNamesPtr[classStorage].name, 
											outputFileStreamPtr, 
											outputCodePtr, 
											countTotalFlag))
																		 			return (FALSE);
			
			initializeFlag = TRUE;
			
			}	// end "if (clsfyVariablePtr->summaryCode == kClassSummary)" 
			
		if (clsfyVariablePtr->summaryCode == kGroupSummary)
			{
					// List the information for the group if this is the last 		
					// class for the group.	
					
			groupIndex = (SInt16)GetGroupIndex (clsfyVariablePtr, statClassNumber);
			
			listGroupSummaryFlag = DetermineIfLastClassForGroup (clsfyVariablePtr,
																					groupIndex,
																					(SInt16)classIndex,
																					classAreaPtr, 
																					numberClassAreas);
			
			if (listGroupSummaryFlag) 
				{
				if (groupIndex < 0)
					localGroupNamePtr = NULL;
					
				else	// groupIndex >= 0 
					localGroupNamePtr = &groupNamePtr[32*groupIndex];
				
						// List the line of data.												
						
				if (!ListLineOfResults (clsfyVariablePtr, 
													classifySubTotalVectorPtr, 
													totalNumberSamples, 
													columnClassPtr, 
													numberTableListColumns, 
													(SInt16)classifyVectorIndex, 
													localGroupNamePtr, 
													outputFileStreamPtr, 
													outputCodePtr, 
													countTotalFlag))
																			 			return (FALSE);
																			 			
				initializeFlag = TRUE;
					
				}	// end "if (listGroupSummaryFlag)"
				
			}	// end "if (clsfyVariablePtr->summaryCode == kGroupSummary)"
			
		if (initializeFlag)
			{
					// We are now ready to start a new class or group of information.
					// Save the information from the current class or group for the
					// Reliability measure and the Kappa measures.
					
					// Get and save the number of correctly classified samples for the 
					// class or group. This will be used to compute the "Reliability 
					// Accuracy" line.
					
			if (groupSummaryClassTableFlag)
				{
						// Get the correct total for each class in the current group.
						// Note that the first column is for the number of thresholded pixels.
						
				for (column=1; column<=gImageFileInfoPtr->numberClasses; column++)
					{
					if (classToGroupPtr[column-1] == groupIndex)	 			
						correctVectorPtr[column] += classifySubTotalVectorPtr[column];
	
					}	// end "for (column=1; column<..."
				
				}	// end "if (groupSummaryClassTableFlag"
				
			else	// !groupSummaryClassTableFlag
				{
				vectorIndex = GetCountVectorIndex (
												clsfyVariablePtr,
												classAreaPtr,
												(UInt16)classIndex,
												clsfyVariablePtr->tableType == kGroupTable);	
												
				correctVectorPtr[vectorIndex] += classifySubTotalVectorPtr[vectorIndex];
													
				}	// end "else !groupSummaryClassTableFlag"
			
					// Now save information for the agreement matrix table to be used to
					// compute the Kappa Statistic.
						
			agreementMatrixPtr = &classifyTotalVectorPtr[4*clsfyVariablePtr->tableWidth];
			
					// Note that 'classAreaPtr' is a one based vector with class numbers
					// starting from 1. 'classToGroupPtr' is a 0 based vector with group
					// numbers starting from 0. If the background group is present, then 
					// the background group is 0. 
			
			vectorIndex = classAreaPtr[classIndex] + backgroundIndexOffset - 1;
			if (groupOrderedFlag)
				{
				if (vectorIndex < gImageFileInfoPtr->numberClasses)
					vectorIndex = classToGroupPtr[vectorIndex];
					
				else	// vectorIndex > gImageFileInfoPtr->numberClasses
					vectorIndex = 0;
					
				}	// end "if (groupOrderedFlag)"
				
			agreementMatrixPtr = 
								&agreementMatrixPtr[vectorIndex*clsfyVariablePtr->tableWidth];
								
			vectorIndex = 0;
			for (column=1; column<=clsfyVariablePtr->tableWidth-1; column++)
				{
				if (groupSummaryClassTableFlag)
					{
					if (column <= gImageFileInfoPtr->numberClasses)
						vectorIndex = classToGroupPtr[column-1];
						
					else	// column > gImageFileInfoPtr->numberClasses
						vectorIndex = 0;
						
					}	// end "if (groupSummaryClassTableFlag)"
					 			
				agreementMatrixPtr[vectorIndex] += classifySubTotalVectorPtr[column];
								
				vectorIndex++;

				}	// end "for (column=0; column<..."
				
			if (clsfyVariablePtr->thresholdFlag)
				agreementMatrixPtr[numberKappaColumns] += classifySubTotalVectorPtr[0];
			
			}	// end "if (initializeFlag)"
			
		}	// end "for (classIndex=0; classIndex<..." 
	
	if (clsfyVariablePtr->thresholdFlag)
		{
				// The data has been thresholded; include a reference line of 0's to 
				// repesent the 'threshold' class in the agreement matrix to be used 
				// to compute the Kappa Statistic.
			
		numberKappaColumns++;
			
		}	// end "if (clsfyVariablePtr->thresholdFlag)"
		
	clsfyVariablePtr->numberKappaColumns = numberKappaColumns;
		
			// List the totals.																	
			
	continueFlag = ListPerformanceTableTotals (clsfyVariablePtr, 
																classifyTotalVectorPtr,
																columnClassPtr, 
																numberTableListColumns, 
																outputFileStreamPtr, 
																outputCodePtr);
	
	return (continueFlag);

}	// end "ListFCGPerformanceTable" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListLineOfResults
//
//	Software purpose:	The purpose of this routine is to list the results
//							for a line of the field, class or group performance table.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 02/28/1991
//	Revised By:			Larry L. Biehl			Date: 08/16/2010	

Boolean ListLineOfResults (
				ClassifierVar*						clsfyVariablePtr, 
				HSInt64Ptr							classifySubTotalVectorPtr, 
				SInt64								totalNumberSamplesInLine, 
				SInt16*								columnClassPtr, 
				UInt32								numberColumns, 
				SInt32								countVectorIndex, 
				char*									columnNamePtr, 
				CMFileStream*						outputFileStreamPtr, 
				SInt16*								outputCodePtr, 
				Boolean								countTotalFlag)

{
	Ptr									stringPtr;
	
	double								percentCorrect;

	SInt64								numberCorrectSamples;
	
	SInt32								identifier; 
	                                         
	SInt16								strLength;
	
	Boolean								continueFlag;
	
	
			// Initialize local variables.													
	
	stringPtr = gCharBufferPtr1;
				
			// Load the class name into a string.									
	
	sprintf ((char*)gTextString2, "                                  ");
	if (columnNamePtr != NULL)
		{
		pstr ((char*)gTextString2, columnNamePtr, &strLength);
		gTextString2[strLength] = ' ';
		
		}	// end "if (columnNamePtr != NULL)"
		
	gTextString2[clsfyVariablePtr->classGroupNameWidth] = 0;
	
			// Convert the identifier to ascii string.								
	
	identifier = countVectorIndex;
	if (clsfyVariablePtr->tableType == kClassTable && 
											clsfyVariablePtr->summaryCode != kGroupSummary)
		identifier -= clsfyVariablePtr->backgroundIndexOffset;
			
	if (identifier >= 0)
		sprintf ((char*)gTextString3, 
						"%4ld", 
						identifier);
		
	else	// identifier < 0 
		sprintf ((char*)gTextString3, "    ");
	
	if (totalNumberSamplesInLine > 0)
		{
				// Load the class name, class number, into the string to be 	
				// printed.																		
				
		sprintf (stringPtr, "    %s \t%s \t", gTextString2, gTextString3);
		stringPtr += clsfyVariablePtr->classGroupNameWidth + 12;
		
			// Get the total correct samples if needed.								
		
		if (countTotalFlag || clsfyVariablePtr->unitsCode == kSampleUnits)
			{
			numberCorrectSamples = GetClassOrGroupNumberOfSamples (
												clsfyVariablePtr,
												classifySubTotalVectorPtr,
												countVectorIndex,
												clsfyVariablePtr->summaryCode == kGroupSummary,
												clsfyVariablePtr->tableType == kGroupTable);
														
			}	// end "if (countTotalFlag || ..." 
			
				// Add the percent column if needed.									
				
		if (clsfyVariablePtr->unitsCode == kSampleUnits)
			{
					// Compute the percent correct for the class.					
			
			percentCorrect = 0;
			if (totalNumberSamplesInLine > 0)	
				percentCorrect = (double)numberCorrectSamples * 
																	100/totalNumberSamplesInLine;
									
					// Load the percent correct into the string to be 				
					// listed.																	
					
			sprintf (stringPtr, "  %5.1f  \t", percentCorrect);
			stringPtr += kReferenceAccuracyWidth + 1;
			
			}	// end "if (clsfyVariablePtr->unitsCode == ..." 

				// Load rest of the string for this line.								
				
		LoadListResultsString (	clsfyVariablePtr, 
										&stringPtr, 
										classifySubTotalVectorPtr, 
										columnClassPtr, 
										totalNumberSamplesInLine, 
										numberColumns);
			
				// Update count for total samples and total correct 				
				// samples.																		
	
		if (countTotalFlag)
			{
			clsfyVariablePtr->totalNumberSamples += totalNumberSamplesInLine;
			clsfyVariablePtr->totalCorrectSamples += numberCorrectSamples;
										
			}	// end "if (countTotalFlag)" 
			
		}	// end "if (totalNumberSamplesInLine > 0)" 
				
	else	// if (totalNumberSamplesInLine == 0) 
		{
				// Load the class name and class number and message. 				
		
		sprintf (gCharBufferPtr1, "    %s \t%s      "
					"\tResults are not available.%s",
					gTextString2,
					gTextString3,
					gEndOfLine);
		
		}	// end "else if (totalNumberSamplesInLine == 0)" 
	
	continueFlag = OutputString (
						outputFileStreamPtr, gCharBufferPtr1, 0, *outputCodePtr, TRUE);
		
	return (continueFlag);
			
}	// end "ListLineOfResults" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListOverallPerformanceValues
//
//	Software purpose:	The purpose of this routine is to list the overall classification
//							performance values.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			ListPerformanceTables in SLstRslt.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/31/1997
//	Revised By:			Larry L. Biehl			Date: 01/08/1998

Boolean ListOverallPerformanceValues (
				ClassifierVar* 					clsfyVariablePtr, 
				CMFileStream*						outputFileStreamPtr, 
				SInt16*								outputCodePtr, 
				SInt16*								classPtr, 
				UInt32								numberClasses, 
				SInt16*								classAreaPtr, 
				UInt32								numberClassAreas,
				UInt32								numberTableListColumns,
				SInt16								classGroupCode)

{
	SInt16								correctNumberWidth,
											stringIndex,
											totalNumberWidth;
											
	Boolean								continueFlag = TRUE;
	
		
			// List the overall performance.													
	
	if (clsfyVariablePtr->totalNumberSamples > 0)
		{
				// List the Reliability or User Accuracy Line.
				
		continueFlag = ListReliabilityAccuracyLine (clsfyVariablePtr,
																	classPtr,
																	numberTableListColumns, 
																	outputFileStreamPtr, 
																	outputCodePtr);
		
				// Get the correct string.
		
		stringIndex = IDS_ListResult40;
		if (classGroupCode == kGroupPerformance)
			stringIndex = IDS_ListResult41;
			
		continueFlag = GetSpecifiedStringNumber (kListResultsStrID,
																stringIndex,
																gTextString2, 
																continueFlag);
		
				// Get the number widths.
				
		correctNumberWidth = sprintf ((char*)gTextString, 
													"%lld", 
													clsfyVariablePtr->totalCorrectSamples); 

		//NumToString (clsfyVariablePtr->totalCorrectSamples, gTextString);
		//correctNumberWidth = gTextString[0];
		
		totalNumberWidth = sprintf ((char*)gTextString, 
													"%lld", 
													clsfyVariablePtr->totalNumberSamples);
				
		//NumToString (clsfyVariablePtr->totalNumberSamples, gTextString);
		//totalNumberWidth = gTextString[0];
	
		if (continueFlag)
			sprintf ((char*)gCharBufferPtr1, 
							//"   OVERALL PERFORMANCE (%*lld / %*lld) = %5.1f%%",
							(char*)&gTextString2[1],
							correctNumberWidth,
							clsfyVariablePtr->totalCorrectSamples, 
							totalNumberWidth,
							clsfyVariablePtr->totalNumberSamples,
							(double)clsfyVariablePtr->totalCorrectSamples /
														clsfyVariablePtr->totalNumberSamples*100);
												
		continueFlag = OutputString (outputFileStreamPtr,
												gCharBufferPtr1,
												0, 
												*outputCodePtr, 
												continueFlag);
		
				// Compute the Kappa Statistic and list it.
		
		double kappaVariance;								
		double kappaStatistic = ComputeKappaStatistic (clsfyVariablePtr,
																		classPtr,
																		numberClasses,
																		&kappaVariance);
		
		continueFlag = GetSpecifiedStringNumber (kListResultsStrID,
																IDS_ListResult46,
																gTextString2, 
																continueFlag);
		
		sprintf ((char*)gCharBufferPtr1, 
								//"   Kappa Statistic (X100) = %5.1f%%. Kappa Variance = %f.",
								(char*)&gTextString2[1],
								kappaStatistic,
								kappaVariance);
												
		continueFlag = OutputString (outputFileStreamPtr,
												gCharBufferPtr1,
												0, 
												*outputCodePtr, 
												continueFlag);
		
		}	// end "if (clsfyVariablePtr->totalNumberSamples > 0 ...)"
		
	return (continueFlag);
			
}	// end "ListOverallPerformanceValues" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListPerformanceTables
//
//	Software purpose:	The purpose of this routine is to list the 
//							classification field and class performance tables.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			ClassifyTrainTestFields in classify.c
//							ListResultsTrainTestFields in listResults.c
//
//	Coded By:			Larry L. Biehl			Date: 11/02/1990
//	Revised By:			Larry L. Biehl			Date: 01/07/1998

Boolean ListPerformanceTables (
				ClassifierVar* 					clsfyVariablePtr, 
				SInt16								fieldType, 
				CMFileStream*						outputFileStreamPtr, 
				SInt16*								outputCodePtr, 
				SInt16*								classPtr, 
				UInt32								numberClasses, 
				SInt16*								classAreaPtr, 
				UInt32								numberClassAreas)

{
	UInt32								numberTableListColumns;
	
	SInt16								savedSummaryCode;
											
	Boolean								continueFlag;
	
	
			// Initialize local variables.													
			
	savedSummaryCode = clsfyVariablePtr->summaryCode;
	continueFlag = TRUE;
	
			// Set up structure value.															
			
	clsfyVariablePtr->areaCode = fieldType;
	clsfyVariablePtr->totalCorrectSamples = 0;
	clsfyVariablePtr->totalNumberSamples = 0;
	
			// Get the number of columns in the output tables.
	
	if (gProcessorCode == kClassifyProcessor)
		numberTableListColumns = numberClasses;
		
	else	// gProcessorCode != kClassifyProcessor 
		{
		numberTableListColumns = gImageFileInfoPtr->numberClasses;
		
		if (clsfyVariablePtr->tableType == kGroupTable)
			numberTableListColumns = gImageFileInfoPtr->numberGroups;
		
		}	// end "else gProcessorCode != kClassifyProcessor"
	
			// List the field performance table.											
	
	if (savedSummaryCode & kFieldSummary)
		{
		clsfyVariablePtr->summaryCode = kFieldSummary;
		continueFlag = ListFCGPerformanceTable (clsfyVariablePtr,
																fieldType,
																outputFileStreamPtr, 
																outputCodePtr, 
																classPtr, 
																numberClasses, 
																classAreaPtr, 
																numberClassAreas,
																numberTableListColumns);
		
		}	// end "if ((savedSummaryCode & kFieldSummary) ..." 
											
			// List the class performance table.											
			
	if ((savedSummaryCode & kClassSummary) && continueFlag)
		{
		clsfyVariablePtr->summaryCode = kClassSummary;
		continueFlag = ListFCGPerformanceTable (clsfyVariablePtr,
																fieldType,
																outputFileStreamPtr, 
																outputCodePtr, 
																classPtr, 
																numberClasses, 
																classAreaPtr, 
																numberClassAreas,
																numberTableListColumns);
		
		}	// end "if ((savedSummaryCode & kClassSummary) ..."
		
	if ((savedSummaryCode & kFieldSummary+kClassSummary) && continueFlag)
		{
		if (!(savedSummaryCode & kGroupSummary) || 
														clsfyVariablePtr->tableType == kClassTable)
			{
			continueFlag = ListOverallPerformanceValues (clsfyVariablePtr,
																		outputFileStreamPtr,
																		outputCodePtr, 
																		classPtr, 
																		numberClasses, 
																		classAreaPtr, 
																		numberClassAreas,
																		numberTableListColumns,
																		kClassPerformance);
			
			}	// end "if (clsfyVariablePtr->tableType == kClassTable)"
		
		}	// end "if ((savedSummaryCode & kFieldSummary+kClassSummary) && ..."
											
			// List the group performance table.											
			
	if ((savedSummaryCode & kGroupSummary) && continueFlag)
		{
				// Force the totals to be recounted in case they were used above to count
				// the total for a class table.
				
		clsfyVariablePtr->totalCorrectSamples = 0;
		clsfyVariablePtr->totalNumberSamples = 0;
	
		clsfyVariablePtr->summaryCode = kGroupSummary;
		continueFlag = ListFCGPerformanceTable (clsfyVariablePtr,
																fieldType,
																outputFileStreamPtr, 
																outputCodePtr, 
																classPtr, 
																numberClasses, 
																classAreaPtr, 
																numberClassAreas,
																numberTableListColumns);
		
				// List the overall performance.	
		
		if (continueFlag)		
			continueFlag = ListOverallPerformanceValues (clsfyVariablePtr,
																		outputFileStreamPtr,
																		outputCodePtr, 
																		classPtr, 
																		numberClasses, 
																		classAreaPtr, 
																		numberClassAreas,
																		numberTableListColumns,
																		kGroupPerformance);	
		
		}	// end "if ((savedSummaryCode & kGroupSummary) ..." 
	
	if (continueFlag)	
		continueFlag = ListTableFootNotes (clsfyVariablePtr,
														outputFileStreamPtr,
														outputCodePtr);
	
			// Restore the summary code.														
			
	clsfyVariablePtr->summaryCode = savedSummaryCode;
	
	return (continueFlag);

}	// end "ListPerformanceTables" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListPerformanceTableTotals
//
//	Software purpose:	The purpose of this routine is to list the total
//							line for the field, class or group performance table.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/28/1991
//	Revised By:			Larry L. Biehl			Date: 02/09/2000	

Boolean ListPerformanceTableTotals (
				ClassifierVar*						clsfyVariablePtr, 
				HSInt64Ptr							classifyTotalVectorPtr, 
				SInt16*								classPtr, 
				UInt32								numberColumns, 
				CMFileStream*						outputFileStreamPtr, 
				SInt16*								outputCodePtr)

{
	Boolean								continueFlag;
	Ptr									stringPtr;
	
		
			// List the totals.		
			
	stringPtr = gCharBufferPtr1;
			
	MGetString (gTextString, kListResultsStrID, IDS_ListResult22);
	
	sprintf (stringPtr, "%s", &gTextString[1]);
	stringPtr += MIN (gTextString[0], 
				4 + clsfyVariablePtr->classGroupNameWidth + gNumberOfEndOfLineCharacters); 
				
	sprintf (stringPtr, " TOTAL\t\t");
	stringPtr += 8;
	
	if (clsfyVariablePtr->unitsCode == kSampleUnits)
		{
		sprintf (stringPtr, "\t         ");
		stringPtr = (Ptr)&gCharBufferPtr1[strlen ((char*)gCharBufferPtr1)];
		
		}	// end "if (clsfyVariablePtr->unitsCode == kSampleUnits)" 
	
			// Load rest of the string for this line.										
			
	LoadListResultsString (clsfyVariablePtr, 
									&stringPtr, 
									classifyTotalVectorPtr, 
									classPtr, 
									clsfyVariablePtr->totalNumberSamples, 
									numberColumns);
		
	continueFlag = OutputString (
						outputFileStreamPtr, gCharBufferPtr1, 0, *outputCodePtr, TRUE);
		
	return (continueFlag);
		
}	// end "ListPerformanceTableTotals" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListReliabilityAccuracyLine
//
//	Software purpose:	The purpose of this routine is to list the line containing the
//							reliability accuracy, also called user's accuracy which is
//							100 - commission error.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/06/1998
//	Revised By:			Larry L. Biehl			Date: 08/13/2010	

Boolean ListReliabilityAccuracyLine (
				ClassifierVar*						clsfyVariablePtr,
				SInt16*								classPtr,
				UInt32								numberTableListColumns,
				CMFileStream*						outputFileStreamPtr, 
				SInt16*								outputCodePtr)

{
	double								percentCorrect;
	
	HSInt64Ptr							classifyTotalVectorPtr,
											correctVectorPtr;
											
	HUInt32Ptr							columnWidthVectorPtr;
											
	Ptr									stringPtr;
	
	SInt16*								columnClassPtr;

	SInt64								numberClassifiedSamples,
											numberCorrectSamples;
	
	UInt32								classifyVectorIndex,
											columnIndex, 
											index;
											
	SInt16								columnWidth,
											extraSpaces; 
	
	Boolean								continueFlag = TRUE,
											groupOrderedFlag;
	
	
			// Do not list this line if it is a percent or area table.
			
	if (clsfyVariablePtr->unitsCode != kSampleUnits)
																					return (continueFlag);
	
			// Initialize local variables.													
	
	stringPtr = gCharBufferPtr1;			
	
	classifyTotalVectorPtr = &clsfyVariablePtr->countVectorPtr[
															clsfyVariablePtr->totalCountIndexStart];
	
	correctVectorPtr = &classifyTotalVectorPtr[3*clsfyVariablePtr->tableWidth];
	
	columnWidthVectorPtr = clsfyVariablePtr->columnWidthVectorPtr;
	
	if (gProcessorCode == kClassifyProcessor)
		columnClassPtr = classPtr;
		
	else	// gProcessorCode != kClassifyProcessor 
		columnClassPtr = NULL;
	
			// Determine if this is a group ordered table.								
			// If so get a pointer to the class to group vector and get				
			// proper pointer to the list of classes.										
			
	groupOrderedFlag = (clsfyVariablePtr->tableType == kGroupTable ||
												clsfyVariablePtr->summaryCode == kGroupSummary);
		
			// List the totals.	
			
	MGetString (gTextString, kListResultsStrID, IDS_ListResult22);
	
	extraSpaces = MAX (0, clsfyVariablePtr->numberWidth-7);
	sprintf (stringPtr, "%s", &gTextString[1]);
	stringPtr += MIN (gTextString[0], 
			4 + clsfyVariablePtr->classGroupNameWidth + gNumberOfEndOfLineCharacters - 2 +
				extraSpaces); 
	
	MGetString (gTextString, kListResultsStrID, IDS_ListResult45);
	sprintf (stringPtr, "%s", &gTextString[1]);
	stringPtr += gTextString[0];      

			// List class/group reliability or user's accuracy.																	
			
	for (columnIndex=0; columnIndex<numberTableListColumns; columnIndex++)
		{
		if (columnIndex == 0 && clsfyVariablePtr->backgroundIndexOffset == 1)
			{
			*stringPtr = '\t';
			stringPtr++;
										
			columnWidth = (SInt16)columnWidthVectorPtr[1];
			
			for (index=0; index<(UInt32)columnWidth; index++)
				{
				*stringPtr = ' ';
				stringPtr++;
				
				}	// end "for (index=0; index<columnWidth; index++)"
			
			}	// end "if (columnIndex == 0 && ..."
			
		else	// columnIndex != 0 || ...->backgroundIndexOffset != 1
			{		
			if (columnClassPtr != NULL)
				classifyVectorIndex = columnClassPtr[columnIndex];
				
			else	// columnClassPtr == NULL 
				classifyVectorIndex = columnIndex + 1;
				
			numberCorrectSamples = correctVectorPtr[classifyVectorIndex];
				
			numberClassifiedSamples = classifyTotalVectorPtr[classifyVectorIndex];
															
					// Compute the reliability or user accuracy percent correct for the
					// class or group.
			
			percentCorrect = 0;
			if (numberClassifiedSamples > 0)	
				percentCorrect = (double)numberCorrectSamples * 100/numberClassifiedSamples;
				
			columnWidth = (SInt16)columnWidthVectorPtr[classifyVectorIndex];
			sprintf (stringPtr,  "\t%*.1f", columnWidth, percentCorrect);
			
			stringPtr += columnWidth + 1;
			
			}	// end "else columnIndex != 0 || ..."

		}	// end "for (columnIndex=0; columnIndex<..." 
									
			// Add carriage return to the end of the line.								

	sprintf (stringPtr, gEndOfLine);
		
	continueFlag = OutputString (
						outputFileStreamPtr, gCharBufferPtr1, 0, *outputCodePtr, TRUE);
		
	return (continueFlag);
		
}	// end "ListReliabilityAccuracyLine" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListResultsAreasControl
//
//	Software purpose:	The purpose of this routine is to control the
//							listing the results of the requested areas.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			ListResultsControl
//
//	Coded By:			Larry L. Biehl			Date: 02/19/1991
//	Revised By:			Larry L. Biehl			Date: 02/14/2014

Boolean ListResultsAreasControl (
				FileInfoPtr							fileInfoPtr, 
				ClassifierVar*						clsfyVariablePtr)

{
	CMFileStream*						listResultsFileStreamPtr;
	
	FileIOInstructionsPtr			fileIOInstructions1Ptr,
											fileIOInstructions2Ptr;
	
	Handle								countVectorHandle = NULL;			
	
	//SInt32								numberInputBytesNeeded;
	
	UInt32								index;
	
	SInt16								returnCode;				// =0, no error			
																		// =1, no memory for 	
																		// 		text.				
																		// =2, disk file error.	
																		// =3, user quit.
																		
	Boolean								continueFlag; 
	               
		
			// Initialize local variables. 									
			
	listResultsFileStreamPtr = GetResultsFileStreamPtr (0);
	countVectorHandle = NULL;
	returnCode = noErr;
	continueFlag = TRUE;
	clsfyVariablePtr->countClassIndexPtr = NULL; 
	fileIOInstructions1Ptr = NULL;
	fileIOInstructions2Ptr = NULL;
	
			// Get a vector to map the class symbols to the class numbers.			
		
	clsfyVariablePtr->symbolToClassPtr = GetSymbolToClassVector (fileInfoPtr);
	if (clsfyVariablePtr->symbolToClassPtr == NULL)
																					return (FALSE);
	
			// Get memory for a character buffer to be used in listing.  			
			// The table number widths will be set up by 								
			// 'GetListResultsStringBuffer'.		
			
	SInt16* classPtr = (SInt16*)GetHandlePointer (gListResultsSpecsPtr->classHandle);
	
			// Get vector to use for count of the number of pixels classified		
			// into each class.  A vector of long int's will be use.					
	
	if (continueFlag)
		{
				// Save width of table for one field and the start of the 			
				// storage for totals.  Allow for threshold class.				
		
		clsfyVariablePtr->tableWidth = fileInfoPtr->numberClasses + 1;
		
				// Make sure there is space for case when the thematic image has
				// fewer classes than the project image file.
						
		if (gProjectInfoPtr != NULL)
			clsfyVariablePtr->tableWidth = MAX (
										clsfyVariablePtr->tableWidth,
										(UInt32)(gProjectInfoPtr->numberStatisticsClasses + 1));
		
		if (gListResultsSpecsPtr->tableType == kGroupTable)
			clsfyVariablePtr->tableWidth = fileInfoPtr->numberGroups + 1;
			
		countVectorHandle = GetCountVectorTableMemory (
												clsfyVariablePtr,
												gListResultsSpecsPtr->areaCode & kTrainingType,
												gListResultsSpecsPtr->areaCode & kTestingType);
		
		continueFlag = (countVectorHandle != NULL);
			
		}	// end "if (continueFlag)" 										
	
	if (continueFlag)
		continueFlag = GetListResultsStringBuffer (fileInfoPtr,
																	clsfyVariablePtr, 
																	gListResultsSpecsPtr->unitsCode, 
																	(SInt16)fileInfoPtr->numberClasses,
																	classPtr,
																	gListResultsSpecsPtr->numberClasses);
	
			// Get buffer to read data from classified file into. Force bytes read in
			// to be 2.
			
	if (continueFlag)
		continueFlag = GetIOBufferPointers (&gFileIOInstructions[0],
														gImageWindowInfoPtr,
														gImageLayerInfoPtr,
														gImageFileInfoPtr,
														&gInputBufferPtr, 
														&gOutputBufferPtr,
														1,
														gImageWindowInfoPtr->maxNumberColumns,
														gListResultsSpecsPtr->columnInterval,
														1,
														NULL,
														kPackData,
														kDoNotForceBISFormat,
														kForce2Bytes,
														kDoNotAllowForThreadedIO,
														&fileIOInstructions1Ptr);
	
	if (continueFlag && gListResultsSpecsPtr->thresholdFlag)
		{				
				// Note for now the image size of the probability file has to be
				// the same as that for the classified file.
		
		continueFlag = GetIOBufferPointers (&gFileIOInstructions[1],
														gImageWindowInfo2Ptr,
														gImageLayerInfo2Ptr,
														gImageFileInfo2Ptr,
														&gInputBuffer2Ptr, 
														&gOutputBuffer2Ptr,
														1,
														gImageWindowInfo2Ptr->maxNumberColumns,
														gListResultsSpecsPtr->columnInterval,
														1,
														NULL,
														kPackData,
														kDoNotForceBISFormat,
														kForce2Bytes,
														kDoNotAllowForThreadedIO,
														&fileIOInstructions2Ptr);
		
		}	// end "if (continueFlag && gListResultsSpecsPtr->thresholdFlag)"
		
			// Get vector for storing the index into countVectorPtr at which		
			// the counts for a class start.													
	
	if (continueFlag && (gListResultsSpecsPtr->areaCode & (kTrainingType+kTestingType)))
		{		
		clsfyVariablePtr->countClassIndexPtr = (SInt32*)MNewPointer (
					(SInt32)gProjectInfoPtr->numberStatisticsClasses * sizeof (SInt32));
		continueFlag = (clsfyVariablePtr->countClassIndexPtr != NULL);
			
		}	// end "if (continueFlag && ...)" 
			
			// Continue if okay to here.														
			
	if (continueFlag)
		{
				// List the threshold value used if being used.		

		continueFlag = ListThresholdValue (gListResultsSpecsPtr->probabilityThreshold,
														gListResultsSpecsPtr->probabilityThreshold,
														gListResultsSpecsPtr->probabilityThreshold,
														listResultsFileStreamPtr,
														&gOutputForce1Code,
														gListResultsSpecsPtr->thresholdFlag,
														gListResultsSpecsPtr->thresholdTypeCode);	
		
				// Show status fields in status dialog that apply for training,	
				// test and image results listings.											
		
		if (continueFlag)
			{
			ShowStatusDialogItemSet (kStatusLine);
			ShowStatusDialogItemSet (kStatusCommand);
			
			}	// end "if (continueFlag)" 
												
				// Show status fields in status dialog that apply for training,	
				// test field classification only.											
		
		if (continueFlag &&
							(gListResultsSpecsPtr->areaCode & (kTrainingType+kTestingType)))
			{
			ShowStatusDialogItemSet (kStatusClassA);
			ShowStatusDialogItemSet (kStatusField);
			
			}	// end "if (continueFlag && ...)" 
		
		if (continueFlag && gStatusDialogPtr)	
			DrawDialog (gStatusDialogPtr);
			
				// List results for the training fields if requested.					
			
		if (continueFlag && (gListResultsSpecsPtr->areaCode & kTrainingType))
			{         
			LoadDItemStringNumber (kListResultsStrID, 
											IDS_ListResult31,	//"\pListing: Training Fields"
											gStatusDialogPtr, 
											IDC_Status11, 
											(Str255*)gTextString);
											
					// Intialize the nextTime variable to indicate when the next check	
					// should occur for a command-.
	
			gNextTime = TickCount ();
			gNextStatusTime = gNextTime;
			
			returnCode = ListResultsTrainTestFields (
											&gAreaDescription, 
											fileIOInstructions1Ptr,
											fileIOInstructions2Ptr,
											clsfyVariablePtr, 
											kTrainingType);
					
			continueFlag = (returnCode == 0);
			
			}	// end "if (...areaCode & kTrainingType)))" 
					
				// List results the test fields if requested								
			
		if (continueFlag && (gListResultsSpecsPtr->areaCode & kTestingType))
			{  
			LoadDItemStringNumber (kListResultsStrID, 
											IDS_ListResult32,	// "\pListing: Test Fields"
											gStatusDialogPtr, 
											IDC_Status11, 
											(Str255*)gTextString);    
					// Intialize the nextTime variable to indicate when the next check	
					// should occur for a command-.
	
			gNextTime = TickCount ();
			gNextStatusTime = gNextTime;
			
			returnCode = ListResultsTrainTestFields (&gAreaDescription,
																	fileIOInstructions1Ptr,
																	fileIOInstructions2Ptr,
																	clsfyVariablePtr, 
																	kTestingType);
				
			continueFlag = (returnCode == 0);
			
			}	// end "if (...areaCode & kTestingType)))" 
		
				// Get the summary of the image area selection if requested.		
				
		if (continueFlag && (gListResultsSpecsPtr->areaCode & kAreaType))
			{    			
			LoadDItemStringNumber (kListResultsStrID, 
											IDS_ListResult33,	// "\pListing: Image Area"
											gStatusDialogPtr, 
											IDC_Status11, 
											(Str255*)gTextString);
											
			if (gStatusDialogPtr != NULL)
				{
				HideStatusDialogItemSet (kStatusClassA);
				HideStatusDialogItemSet (kStatusField);
				ShowStatusDialogItemSet (kStatusMinutes);
				DrawDialog (gStatusDialogPtr);
				
				}	// end "if (gStatusDialogPtr != NULL)" 
				
			InitializeAreaDescription (&gAreaDescription, 
													gListResultsSpecsPtr->lineStart, 
													gListResultsSpecsPtr->lineEnd, 
													gListResultsSpecsPtr->columnStart, 
													gListResultsSpecsPtr->columnEnd, 
													gListResultsSpecsPtr->lineInterval, 
													gListResultsSpecsPtr->columnInterval,
													1,
													1,
													0);
													
					// Clear the memory for the counts.										
			 
			for (index=0; index<clsfyVariablePtr->tableWidth; index++)
				clsfyVariablePtr->countVectorPtr[index] = 0;
				
					// Intialize the nextTime variable to indicate when the next check	
					// should occur for a command-.
	
			gNextTime = TickCount ();
			gNextStatusTime = gNextTime;
			
			returnCode = GetAreaResults (-1, 
													&gAreaDescription, 
													fileIOInstructions1Ptr,
													fileIOInstructions2Ptr,
													clsfyVariablePtr, 
													&clsfyVariablePtr->countVectorPtr[0]);
					
					// List table with class distribution.									
			
			if (returnCode != 1)									
				continueFlag = ListClassificationSummary (clsfyVariablePtr,
																		listResultsFileStreamPtr, 
																		&gOutputCode,
																		NULL, 
																		0);
			
			}	// end "if (...->areaCode & kAreaType))" 
		
		}	// end "if (continueFlag)" 
			
			// Dispose of temporary vectors.													
			
	clsfyVariablePtr->symbolToClassPtr = CheckAndDisposePtr (
															clsfyVariablePtr->symbolToClassPtr);
		
	clsfyVariablePtr->countClassIndexPtr = CheckAndDisposePtr (
														clsfyVariablePtr->countClassIndexPtr);
				
			// Dispose of the IO buffers.		
								
	DisposeIOBufferPointers (fileIOInstructions1Ptr,
										&gInputBufferPtr, 
										&gOutputBufferPtr);
								
	DisposeIOBufferPointers (fileIOInstructions2Ptr,
										&gInputBuffer2Ptr, 
										&gOutputBuffer2Ptr);
		
	UnlockAndDispose (countVectorHandle);
	
	gCharBufferPtr1 = CheckAndDisposePtr (gCharBufferPtr1);
	
	return (continueFlag);
			
}	// end "ListResultsAreasControl" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		ListResultsControl
//
//	Software purpose:	The purpose of this routine is to control the user
//							requests for list results operations on a thematic image 
//							file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 02/14/1991
//	Revised By:			Larry L. Biehl			Date: 09/18/2006	

void ListResultsControl (void)

{                                        
	ClassifierVar						clsfyVariable;
	CMFileStream*						resultsFileStreamPtr;
	Ptr									ptr;         
	
	SInt16*								classInGroupOrderPtr;
	
	UInt16*								classSymbolPtr;
	
	Handle								windowInfoHandle;
	
	time_t								startTime;
	
	SInt16								activeImageHandleStatus,
											probabilityHandleStatus; 
	
	Boolean								continueFlag,
											listProjectNameFlag,
											statusDialogFlag = TRUE,
											timeFlag = TRUE;
	
	
			// If spare memory had to be used to load code resources, then exit	
			// routine.																				
			
	if (gMemoryTypeNeeded < 0)
																							return;
		    		
			// Code resources loaded okay, so set flag back for non-Code			
			// resources.																			
			
	gMemoryTypeNeeded = 0;
	
			// Initialize local variables.													
			
	gStatusDialogPtr = NULL;
	listProjectNameFlag = FALSE; 
			
	continueFlag = SetUpActiveImageInformationGlobals (
					&windowInfoHandle, &activeImageHandleStatus, kDoNotUseProject);
	                                                    
			// Set up list results specification structure
			
	if (continueFlag)
		continueFlag = LoadListResultsSpecs ();
		
	if (continueFlag)
		{
		if (ListResultsDialog (gImageFileInfoPtr))
			{
					// Initialize some global variables pertaining to output.		
			
			gOutputTextOKFlag = TRUE;
			gOutputCode = gListResultsSpecsPtr->outputStorageType;
			gOutputForce1Code = (gOutputCode | 0x0001);
			
						// If the results are to go the a disk file, open the			
						// disk file.																
				
			if (continueFlag)
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
				
				continueFlag = CreateResultsDiskFiles (gOutputCode, 0, 0);
												
				}	// end "if (continueFlag)" 
											
			resultsFileStreamPtr = GetResultsFileStreamPtr (0);
				
					// Get dialog box to display list results status.					
			
			if (continueFlag && statusDialogFlag)
				{
				gStatusDialogPtr = GetStatusDialog (kUpdateStatsInfoID, TRUE);
				
				continueFlag = (gStatusDialogPtr != NULL);
					
				}	// end "if (continueFlag && statusDialogFlag)" 
			
					// If everything is okay to here list titles for summary			
					// and list the requested summaries themselves.						
			
			if (continueFlag)
				{
						// Lock handles for those items that may be used later.		
				
				ptr = GetHandlePointer (
									gImageFileInfoPtr->classDescriptionH, kLock, kMoveHi);
				
				ptr = GetHandlePointer (
									gImageFileInfoPtr->groupNameHandle, kLock, kMoveHi);
				
				ptr = GetHandlePointer (
									gImageFileInfoPtr->groupTablesHandle, kLock, kMoveHi);
				
				if (gListResultsSpecsPtr->thresholdFlag)
					{
							// Set up file pointers for probability file.
					/*
					ptr = GetHandleStatusAndPointer (
									gListResultsSpecsPtr->probabilityWindowInfoHandle,
									&handleStatus,
									kMoveHi);
					*/
					if (!GetImageInformationPointers (
									&probabilityHandleStatus, 
									gListResultsSpecsPtr->probabilityWindowInfoHandle,
									&gImageWindowInfo2Ptr, 
									&gImageLayerInfo2Ptr, 
									&gImageFileInfo2Ptr))
						gListResultsSpecsPtr->thresholdFlag = FALSE;
									
					}	// end "if (gListResultsSpecsPtr->thresholdFlag)"
					
						// Get class pointer
						
				SInt16* classPtr = (SInt16*)GetHandlePointer (
																gListResultsSpecsPtr->classHandle);
					
						// Change cursor to watch cursor until done with process		
					
				MSetCursor (kWait);
		
						// Force text selection to start from end of present text	
					
				ForceTextToEnd ();
				
						// Initialize some of classifyVariable structure 				
						// parameters.																
						
				clsfyVariable.unitsCode = gListResultsSpecsPtr->unitsCode;
				clsfyVariable.summaryUnitsCode = gListResultsSpecsPtr->summaryUnitsCode;
				clsfyVariable.summaryCode = gListResultsSpecsPtr->summaryCode;
				clsfyVariable.tableType = gListResultsSpecsPtr->tableType;
				clsfyVariable.thresholdFlag = gListResultsSpecsPtr->thresholdFlag;
				clsfyVariable.variable1 = gListResultsSpecsPtr->areaConversionFactor;
				clsfyVariable.useLeaveOneOutMethodFlag = FALSE;
	
						// Determine if a background class exists - either 			
						// class 0 for binary data or the 'blank' class for 			
						// ascii data.																

				clsfyVariable.backgroundIndexOffset = 0;
				classSymbolPtr = NULL;
					
				if (gImageFileInfoPtr->classDescriptionH != NULL)
					classSymbolPtr = (unsigned short int*)GetHandlePointer (
															gImageFileInfoPtr->classDescriptionH);
					
				if (classSymbolPtr != NULL)
					classSymbolPtr = &classSymbolPtr[gImageFileInfoPtr->numberClasses*16];
					
				if (classSymbolPtr != NULL && 
										(classSymbolPtr[0] == 0 || classSymbolPtr[0] == ' '))
					clsfyVariable.backgroundIndexOffset = 1;
				
						// Get a list of classes in group order if needed.				
						
				if (continueFlag && 
						(gListResultsSpecsPtr->areaCode & (kTrainingType+kTestingType)))
					{
					if ((gListResultsSpecsPtr->tableType & kGroupTable) ||
								(gListResultsSpecsPtr->summaryCode & kGroupSummary))
						{
						classInGroupOrderPtr = 
											&classPtr[gProjectInfoPtr->numberStatisticsClasses];
							
						LoadClassInGroupOrderVector (&clsfyVariable,
											gImageFileInfoPtr,
											classInGroupOrderPtr,
											classPtr,
											gListResultsSpecsPtr->numberClasses);
						
						}	// end "if (...tableType & kGroupTable) || ...)" 
						
					listProjectNameFlag = TRUE;
						
					}	// end "if (continueFlag && ..." 
					
						// List the processor name, date, time and project info.			
         
         	SInt16 covarianceStatsToUse = kNoStatisticsUsed;
         	if (gProjectInfoPtr != NULL)
         		covarianceStatsToUse = gProjectInfoPtr->covarianceStatsToUse;			

				continueFlag = ListHeaderInfo (resultsFileStreamPtr,
															kLImageInfo+2*listProjectNameFlag, 
															&gOutputForce1Code, 
															covarianceStatsToUse, 
															continueFlag);
				
						// List area conversion factor if being used.
						
				if (gListResultsSpecsPtr->unitsCode & kAreaUnits)
					continueFlag = ListSpecifiedStringNumber (
												kListResultsStrID, 
												IDS_ListResult47,
												resultsFileStreamPtr, 
												gOutputForce1Code, 
												gListResultsSpecsPtr->areaConversionFactor, 
												continueFlag);	
									
						// List the project classes used.									
				
				if (continueFlag && 
							(gListResultsSpecsPtr->areaCode & (kTrainingType+kTestingType)))
					continueFlag = ListClassesUsed (&clsfyVariable,
																gImageFileInfoPtr, 
																classPtr, 
																kListNoSymbols, 
																kListNoWeights, 
																gListResultsSpecsPtr->numberClasses, 
																resultsFileStreamPtr, 
																&gOutputForce1Code,
																FALSE);
				
						// List the thematic classes if needed. 
							
				if (continueFlag)				
					continueFlag = ListResultsThematicClasses (
														&clsfyVariable,
														gImageFileInfoPtr, 
														NULL, 
														NULL, 
														NULL, 
														(UInt16)gImageFileInfoPtr->numberClasses, 
														resultsFileStreamPtr, 
														&gOutputForce1Code);
				
						// List "  Output Information:".
						
				continueFlag = ListSpecifiedStringNumber (kSharedStrID, 
																			IDS_Shared8,
																			(unsigned char*)gTextString, 
																			resultsFileStreamPtr, 
																			gOutputForce1Code, 
																			continueFlag);
						
						// Get the start time.													
				
				startTime = time (NULL);
				
						// List results for selected areas.									
						
				if (continueFlag)
					{
							// Turn spin cursor on
			
					gPresentCursor = kSpin;
				
					continueFlag = 
								ListResultsAreasControl (gImageFileInfoPtr, &clsfyVariable);
						
							// Change cursor to watch cursor until done with process	
						
					MSetCursor (kWait);
					
					}	// end "if (continueFlag)"
				
						// Unlock handles.														
				
				CheckAndUnlockHandle (gImageFileInfoPtr->classDescriptionH);
				CheckAndUnlockHandle (gImageFileInfoPtr->groupNameHandle);
				CheckAndUnlockHandle (gImageFileInfoPtr->groupTablesHandle);
				
				if (gListResultsSpecsPtr->thresholdFlag)
					{
					UnlockImageInformationHandles (
										probabilityHandleStatus, 
										gListResultsSpecsPtr->probabilityWindowInfoHandle);
										
					gImageWindowInfo2Ptr = NULL; 
					gImageLayerInfo2Ptr = NULL; 
					gImageFileInfo2Ptr = NULL;
										
					}	// end "if (gListResultsSpecsPtr->thresholdFlag)"
				
						// Print the CPU time taken for the listing.						
				
				if (timeFlag)
					continueFlag = ListCPUTimeInformation (
												resultsFileStreamPtr, continueFlag, startTime);
			
						// Scroll output window to the selection and adjust the 		
						// scroll bar.																
				
				UpdateOutputWScrolls (gOutputWindow, 1, kDisplayMessage);
				
						// Change cursor back to pointer.									
							
				MInitCursor ();
					
				}	// end "if (continueFlag)" 
			
					// Dispose of status dialog box.											
			
			CloseStatusDialog (TRUE);
				
					// Close the results files if needed and								
					// release the memory assigned to it.									
					
			CloseResultsFiles ();
			
			}	// end "if (ListResultsDialog ())"
			
		}	// end "if (continueFlag)" 
		
				// Unlock the memory for the list results specifications.		
				
	if (gListResultsSpecsPtr != NULL)
		{
		CheckAndUnlockHandle (gListResultsSpecsPtr->classAreaHandle);
		CheckAndUnlockHandle (gListResultsSpecsPtr->classHandle);
			
		}	// end "if (gListResultsSpecsPtr != NULL)" 
				
	CheckAndUnlockHandle (gNonProjProcessorSpecs.listResultsSpecsH);
	gListResultsSpecsPtr = NULL;
			
				// Unlock the structure handles.	
		
	UnlockActiveImageInformationGlobals (activeImageHandleStatus);	

}	// end "ListResultsControl" 


                  
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListResultsDialog
//
//	Software purpose:	The purpose of this routine is to handle the 		
//							modal dialog for selecting the list results
//							specifications. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			ListResultsControl   in listResults.c
//
//	Coded By:			Larry L. Biehl			Date: 02/14/1991
//	Revised By:			Larry L. Biehl			Date: 09/05/2017	

Boolean ListResultsDialog (
				FileInfoPtr							fileInfoPtr)

{  
	Boolean								returnFlag;
	   
#if defined multispec_mac
	DialogSelectArea					dialogSelectArea;
	
	Rect									theBox,
											theClassAreaBox;
	
	double								areaConversionFactor,
											maxThresholdValue,
											thresholdValue,
											saveThresholdValue,
											saveAreaConversionFactor;
	
	DialogPtr							dialogPtr;
	/*
	#ifdef powerc
		RoutineDescriptor				drawDialogClassAreaPopUpRD = 
													BUILD_ROUTINE_DESCRIPTOR (
																uppUserItemProcInfo, 
																DrawDialogClassAreaPopUp);
	#endif
	*/
	UserItemUPP							drawDialogClassAreaPopUpPtr;
	
	Handle								okHandle,
											theHandle;
	
	SInt32								theNum;
	
	SInt16								controlValue,
											entireIconItem,
											itemHit,
											itemNumber,
											localAreaCode,
											localSummaryCode,
											nonClusterFieldTypeCode,
											okHandleHilite,
											theType;
	
	Boolean								checkOKFlag,
											modalDone,
											projectWindowFlag;
	
	
			// Initialize local variables.													
			
	localAreaCode = gListResultsSpecsPtr->areaCode;
	localSummaryCode = gListResultsSpecsPtr->summaryCode;
	checkOKFlag = FALSE;
	projectWindowFlag = (gProjectInfoPtr != NULL);
	nonClusterFieldTypeCode = 0;
	
	maxThresholdValue = kMaxThreshold;
	if (gListResultsSpecsPtr->thresholdTypeCode == kCEMMode)
		maxThresholdValue = 1.0;
		
	else if (gListResultsSpecsPtr->thresholdTypeCode == kCorrelationMode)
		maxThresholdValue = 90.0;
	
			// Get the modal dialog for the reformat specification					
				
	dialogPtr = LoadRequestedDialog (kListResultsDialogID, kCopyScrap, 1, 2);
	if (dialogPtr == NULL)											
																						return (FALSE);
		
			// Intialize local user item proc pointers.									
	/*
	#ifdef powerc
		drawDialogClassAreaPopUpPtr = &drawDialogClassAreaPopUpRD;
	#else
		drawDialogClassAreaPopUpPtr = DrawDialogClassAreaPopUp;
	#endif
	*/
	drawDialogClassAreaPopUpPtr = NewUserItemUPP (DrawDialogClassAreaPopUp);
				
			// Initialize selected area structure.		
			
	entireIconItem = 10;
	if (gAppearanceManagerFlag)
		entireIconItem = 40;
		
	InitializeDialogSelectArea (&dialogSelectArea,
											gImageWindowInfoPtr,
											gActiveImageWindow,
											gListResultsSpecsPtr->columnStart,
											gListResultsSpecsPtr->columnEnd,
											gListResultsSpecsPtr->columnInterval,
											gListResultsSpecsPtr->lineStart,
											gListResultsSpecsPtr->lineEnd,
											gListResultsSpecsPtr->lineInterval,
											12,
											entireIconItem,
											kAdjustToBaseImage);
											
			// Save handle for the OK button for use later.								
			
	GetDialogItem (dialogPtr, 1,  &theType, &okHandle, &theBox);
	
			// Determine if there are any non-cluster fields in the project.		
			// If so then a performance table can be set up that is not empty.	
			
	if (projectWindowFlag)
		nonClusterFieldTypeCode = DetermineFieldTypes ();
		
	nonClusterFieldTypeCode &= 0x0003;
	
			// Class areas to use.																
			//	Get the rectangles for the class prompt and popup boxes.				
			//	Make all classes the default													
	
	gClassAreaSelection = gListResultsSpecsPtr->classAreaSet;
	SetDialogItemDrawRoutine (
						dialogPtr, 5, &theClassAreaBox, drawDialogClassAreaPopUpPtr);
			
			// If the train/test areas are to be summarized by groups then all	
			// classes will need to be used.													
									
	if (gListResultsSpecsPtr->summaryCode & kGroupSummary)
		gClassAreaSelection = -kAllMenuItem;
	
	if (projectWindowFlag)
		{		
			// List results for training areas. Unhilite this control if there 	
			// are no training areas for the project. 									
			
		if (!(nonClusterFieldTypeCode & kTrainingType))
			SetDLogControlHilite (dialogPtr, 6, 255);
		
		else	// nonClusterFieldTypeCode & kTrainingType
			{
    		controlValue = (gListResultsSpecsPtr->areaCode & kTrainingType)  ? 1 : 0;
			SetDLogControl (dialogPtr, 6, controlValue);
			
			}	// end "else nonClusterFieldTypeCode & kTrainingType"
	
				// List results for test areas. Unhilite this control if there 	
				// are no test areas for the project.										
			
		if (!(nonClusterFieldTypeCode & kTestingType))
			SetDLogControlHilite (dialogPtr, 7, 255);
			
		else	// nonClusterFieldTypeCode & kTestingType 
			{
    		controlValue = (gListResultsSpecsPtr->areaCode & kTestingType)  ? 1 : 0;
			SetDLogControl (dialogPtr, 7, controlValue);
					
			}	// end "else nonClusterFieldTypeCode & kTestingType"
		
		if (GetDLogControl (dialogPtr,6) == 0 && GetDLogControl (dialogPtr,7) == 0)
			HideDialogItem (dialogPtr,5);
					
		}	// end "if (projectWindowFlag)" 
	
	if (!projectWindowFlag)
		HideDialogItems (dialogPtr, 5, 7);
	
			// List results for selected area.												
			
	controlValue = ((gListResultsSpecsPtr->areaCode & kAreaType) ||
		 												(nonClusterFieldTypeCode == 0))  ? 1 : 0;
	SetDLogControl (dialogPtr, 8, controlValue);

			// To entire image icon.															
			//	Selected list results area														

	if (gAppearanceManagerFlag)
		HideDialogItem (dialogPtr, 10);
	else	// !gAppearanceManagerFlag
		HideDialogItem (dialogPtr, 40);
		
	LoadLineColumnItems (&dialogSelectArea, 
									dialogPtr, 
									kInitializeLineColumnValues, 
									kIntervalEditBoxesExist,
									1);
				
			// Image file																			
			
	if (gListResultsSpecsPtr->areaCode & kAreaType || nonClusterFieldTypeCode == 0)
		{
		ShowDialogItem (dialogPtr, 9);
		ShowDialogItem (dialogPtr, entireIconItem);
		ShowDialogItems (dialogPtr, 11, 17);
		
		}	// end "if (gListResultsSpecsPtr->areaCode & kAreaType || ..."
		
	else	// !(gListResultsSpecsPtr->areaCode & kAreaType) && ...
		{
		HideDialogItem (dialogPtr, 9);
		HideDialogItem (dialogPtr, entireIconItem);
		HideDialogItems (dialogPtr, 11, 17);
		
		}	// end "else !(gListResultsSpecsPtr->areaCode & kAreaType) && ..."
	
			// Summarize by field.																
			// Summarize by class. 																
			// Summarize by group. 																
	
	if (projectWindowFlag)
		{
		if (nonClusterFieldTypeCode && 
							(gListResultsSpecsPtr->areaCode & (kTrainingType+kTestingType)))
			{
    		controlValue = (gListResultsSpecsPtr->summaryCode & kFieldSummary)  ? 1 : 0;
			SetDLogControl (dialogPtr, 19, controlValue);
			
    		controlValue = (gListResultsSpecsPtr->summaryCode & kClassSummary)  ? 1 : 0;
			SetDLogControl (dialogPtr, 20, controlValue);
			
    		controlValue = (gListResultsSpecsPtr->summaryCode & kGroupSummary)  ? 1 : 0;
			SetDLogControl (dialogPtr, 21, controlValue);
							
			}	// end "if (nonClusterFieldTypeCode && ...)" 
		
		if (fileInfoPtr->numberGroups == 0)
			SetDLogControlHilite (dialogPtr, 21, 255);
			
		if (gListResultsSpecsPtr->areaCode == kAreaType || 
														nonClusterFieldTypeCode == 0)
			{
			SetDLogControlHilite (dialogPtr, 19, 255);
			SetDLogControlHilite (dialogPtr, 20, 255);
			SetDLogControlHilite (dialogPtr, 21, 255);
			
			}	// end "if (gListResultsSpecsPtr->areaCode == ...)" 
		
		}	// end "if (projectWindowFlag)" 
		
	else	// !projectWindowFlag 
		{
		HideDialogItems (dialogPtr, 18, 21);
		
		}	// end "else !projectWindowFlag" 
	
			// Class type table. 																
			// Group type table. 																
			
	controlValue = (gListResultsSpecsPtr->tableType == kClassTable)  ? 1 : 0;
	SetDLogControl (dialogPtr, 23, controlValue);
			
	controlValue = (gListResultsSpecsPtr->tableType == kGroupTable)  ? 1 : 0;
	SetDLogControl (dialogPtr, 24, controlValue);
	
	if (fileInfoPtr->numberGroups == 0)
		SetDLogControlHilite (dialogPtr, 24, 255);
	
			// Sample units. 																		
			// Percent units. 																	
			// Area units. 																		
			
	controlValue = (gListResultsSpecsPtr->unitsCode & kSampleUnits)  ? 1 : 0;
	SetDLogControl (dialogPtr, 26, controlValue);
	
	controlValue = (gListResultsSpecsPtr->unitsCode & kPercentUnits)  ? 1 : 0;
	SetDLogControl (dialogPtr, 27, controlValue);
			
	controlValue = (gListResultsSpecsPtr->unitsCode & kAreaUnits)  ? 1 : 0;
	SetDLogControl (dialogPtr, 28, controlValue);
	
	if (!(gListResultsSpecsPtr->unitsCode & 0x0004))
		HideDialogItems (dialogPtr, 29, 30);
	
			// Area conversion factor.															
		
	saveAreaConversionFactor = gListResultsSpecsPtr->areaConversionFactor;
	LoadDItemRealValue (dialogPtr, 30, saveAreaConversionFactor, 6);
	
			// Threshold the results.															
			// Threshold percent.																
		
	SetDLogControl (dialogPtr, 31, (gListResultsSpecsPtr->thresholdFlag));
	if (gListResultsSpecsPtr->probabilityWindowInfoHandle == NULL)
		SetDLogControlHilite (dialogPtr, 31, 255);
		
	saveThresholdValue = gListResultsSpecsPtr->probabilityThreshold;
	LoadDItemRealValue (dialogPtr, 33, saveThresholdValue, 6);
	
	ListResultsDialogSetThresholdItems (dialogPtr,
													gListResultsSpecsPtr->thresholdFlag,
													gListResultsSpecsPtr->thresholdTypeCode);
	
			// List results to output text window.											
			// List results to disk file.														
			
	controlValue = (gListResultsSpecsPtr->outputStorageType & 0x0001)  ? 1 : 0;
	SetDLogControl (dialogPtr, 35, controlValue);
	
	controlValue = (gListResultsSpecsPtr->outputStorageType & 0x0002)  ? 1 : 0;
	SetDLogControl (dialogPtr, 36, controlValue);
	okHandleHilite = 0;
		
			// Center the dialog and then show it.											
			
	ShowDialogWindow (dialogPtr, kListResultsDialogID, kSetUpDFilterTable);
		
	gDialogItemDescriptorPtr[30] = kDItemReal;
	gDialogItemDescriptorPtr[33] = kDItemReal;
	
			// Set default text selection to first edit text item						
			
	SelectDialogItemText (dialogPtr, 12, 0, INT16_MAX);
	
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
				case 5:		// Class areas 
					if (gClassAreaSelection > 0)
						{
						itemHit = StandardPopUpMenu (dialogPtr, 
																4, 
																5, 
																gPopUpAllSubsetMenu, 
																gClassAreaSelection, 
																kPopUpAllSubsetMenuID);
													
						if (itemHit == kSubsetMenuItem)
							{
									// Subset of classes to be used.								
									
							itemHit = ClassDialog (
											&gListResultsSpecsPtr->numberClassAreas,
											(SInt16*)*gListResultsSpecsPtr->classAreaHandle, 
											1,
											gProjectInfoPtr->numberStatisticsClasses,
											gClassAreaSelection,
											okHandle);
								
							}	// end "if (itemHit == kSubsetMenuItem)" 
						
						if (itemHit != 0)
							gClassAreaSelection = itemHit;
		
								// Make certain that the correct label is drawn in the	
								// class area pop up box.											
						
						InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
						
						}	// end "if (gClassAreaSelection > 0)"
					break;
					
				case 6:				// List results for training areas. 
				case 7:				// List results for test areas. 
				case 8:				// List results for selected area. 
				
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					localAreaCode =  GetDLogControl (dialogPtr, 6) +
												2 * GetDLogControl (dialogPtr, 7) +
													4 * GetDLogControl (dialogPtr, 8);
				
					
					if (itemHit == 8)
						{
						if (GetControlValue ((ControlHandle)theHandle))
							{
							ShowDialogItem (dialogPtr, 9);
							ShowDialogItem (dialogPtr, entireIconItem);
							ShowDialogItems (dialogPtr, 11, 17);
							SelectDialogItemText (dialogPtr, 12, 0, INT16_MAX);
							
							}	// end "if (GetControlValue ((ControlHandle)..." 
							
						else
							{
							HideDialogItem (dialogPtr, 9);
							HideDialogItem (dialogPtr, entireIconItem);
							HideDialogItems (dialogPtr, 11, 17);
							
							}	// end "else !GetControlValue ((ControlHandle)..." 
							
						}	// end "if (itemHit == 8)" 
						
					checkOKFlag = TRUE;
					
					break;
					
				case 10:				// Entire area to selected area switch.
				case 40:				// Entire area to selected area switch. (Appearance Manager)			
				case 12:				//	 lineStart  
				case 13:				//	 lineEnd  
				case 14:				//	 lineInterval  	
				case 15:				//	 columnStart  
				case 16:				//	 columnEnd  
				case 17:				//	 columnInterval  	
					DialogLineColumnHits (&dialogSelectArea,
													dialogPtr, 
													itemHit,
													theHandle,
													theNum);
					break;
						
				case 19:		// List field summary 
				case 20:		// List class summary 
				case 21:		// List group summary
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					localSummaryCode =  
						GetDLogControl (dialogPtr, 19) * kFieldSummary +
							GetDLogControl (dialogPtr, 20) * kClassSummary +
								GetDLogControl (dialogPtr, 21) * kGroupSummary;
					checkOKFlag = TRUE;
					break;
					
				case 23:		// Class type table
					SetControlValue ((ControlHandle)theHandle, 1);
					SetDLogControl (dialogPtr, 24, 0);
					break;
					
				case 24:		// Class type table
					SetControlValue ((ControlHandle)theHandle, 1);
					SetDLogControl (dialogPtr, 23, 0);
					break;
					
				case 26:		// Sample units 
				case 27:		// Percent units 
				case 28:		// Area units
					SetControlValue ((ControlHandle)theHandle, 1);
					
					for (itemNumber=26; itemNumber<=28; itemNumber++)
						{
						if (itemHit != itemNumber)
							{
							GetDialogItem (dialogPtr, 
											itemNumber, &theType, &theHandle, &theBox);
							SetControlValue ((ControlHandle)theHandle, 0);
							
							}	// end "if (itemHit != itemNumber)" 
							
						}	// end "for (itemNumber=26, itemNumber<=28, ..." 
						
					if (itemHit == 28)
						ShowDialogItems (dialogPtr, 29, 30);
												
					else	// itemHit != 28 
						HideDialogItems (dialogPtr, 29, 30);
					
					break;
					
				case 30:				// Area conversion factor
					areaConversionFactor = GetDItemRealValue (dialogPtr, 30);
					if (areaConversionFactor < 0)
						RealNumberErrorAlert (saveAreaConversionFactor, dialogPtr, 30, 6);
								
					else	// areaConversionFactor >= 0 
						saveAreaConversionFactor = areaConversionFactor;
					break;
					
				case 31:		// Threshold the results. 
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					ListResultsDialogSetThresholdItems (dialogPtr,
												GetControlValue ((ControlHandle)theHandle),
												gListResultsSpecsPtr->thresholdTypeCode);
					break;
					
				case 33:				// Threshold percent	
					thresholdValue = GetDItemRealValue (dialogPtr, 33);
					if (thresholdValue > maxThresholdValue)
						RealNumberErrorAlert (saveThresholdValue, dialogPtr, 33, 6);
					if (thresholdValue >= 0 && thresholdValue <= maxThresholdValue)
						saveThresholdValue = thresholdValue;
					break;
					
				case 35:		// List results to output text window 
				case 36:		// List results to disk file
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					break;
						
				}	// end "switch (itemHit)" 
					
			if (checkOKFlag)
				{
				Boolean		tempFlag = TRUE;
				
				if (projectWindowFlag)
					{		
					tempFlag = (localSummaryCode != 0);
					
					if (localAreaCode == kAreaType || localAreaCode == 0)
						{
						SetDLogControl (dialogPtr, 19, 0);
						SetDLogControlHilite (dialogPtr, 19, 255);
						SetDLogControl (dialogPtr, 20, 0);
						SetDLogControlHilite (dialogPtr, 20, 255);
						SetDLogControl (dialogPtr, 21, 0);
						SetDLogControlHilite (dialogPtr, 21, 255);
						
						HideDialogItem (dialogPtr,5);
						
						}	// end "if (localAreaCode == kAreaType || ...)" 
						
					else	// localAreaCode != kAreaType && ... 
						{
						controlValue = (localSummaryCode & kFieldSummary)  ? 1 : 0;
						SetDLogControl (dialogPtr, 19, controlValue);
						SetDLogControlHilite (dialogPtr, 19, 0);
						
						controlValue = (localSummaryCode & kClassSummary)  ? 1 : 0;
						SetDLogControl (dialogPtr, 20, controlValue);
						SetDLogControlHilite (dialogPtr, 20, 0);
		
						if (fileInfoPtr->numberGroups > 0)
							{
							controlValue = (localSummaryCode & kGroupSummary)  ? 1 : 0;
							SetDLogControl (dialogPtr, 21, controlValue);
							SetDLogControlHilite (dialogPtr, 21, 0);
							
							if (localSummaryCode & kGroupSummary)
								gClassAreaSelection = -kAllMenuItem;
								
							else	// !(localSummaryCode & kGroupSummary)
								gClassAreaSelection = abs (gClassAreaSelection);
								
							InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
							
							}	// end "if (fileInfoPtr->numberGroups > 0)" 
							
						ShowDialogItem (dialogPtr, 5);
						
						}	// end "else localAreaCode != kAreaType && ..." 
						
					}	// end "if (projectWindowFlag)" 
															
				if ((localAreaCode != 0) && tempFlag)
					okHandleHilite = 0;
											
				else 		// localAreaCode == 0 || !tempFlag 
					okHandleHilite = 255;
				
				HiliteControl ((ControlHandle)okHandle, okHandleHilite);
					
				checkOKFlag = FALSE;
					
				}	// end "if (checkOKFlag)" 
				
			}	// end "if (itemHit > 2)" 
			
		else	// itemHit <= 2 
			{
					// If item hit is 1, check if list results area line-column 	
					// values make sense.  If they don't, sound an alert and make 	
					// item hit equal to 0 to allow user to make changes.				
					
			if (itemHit == 1 && GetDLogControl (dialogPtr, 8) == 1)
				itemHit = CheckLineColValues (&dialogSelectArea,
														gListResultsSpecsPtr->lineStart,
														gListResultsSpecsPtr->columnStart,
														dialogPtr);

			if (itemHit == 1 && GetDLogControl (dialogPtr, 28))
				itemHit = CheckRealValue (dialogPtr,
													30,
													0,
													3);
				
			if	(itemHit == 1)      // User selected OK for information 
				{
				modalDone = TRUE;
				returnFlag = TRUE;
					
						// Class areas																

				gListResultsSpecsPtr->classAreaSet = abs (gClassAreaSelection);
				if (gListResultsSpecsPtr->classAreaSet == kAllMenuItem && projectWindowFlag)
					LoadClassAreaVector (&gListResultsSpecsPtr->numberClassAreas,
												(SInt16*)*gListResultsSpecsPtr->classAreaHandle);
					
						// Areas to use to list results.										
				
				gListResultsSpecsPtr->areaCode = 0;
				if (GetDLogControl (dialogPtr, 6))
					gListResultsSpecsPtr->areaCode += kTrainingType;
					
				if (GetDLogControl (dialogPtr, 7))
					gListResultsSpecsPtr->areaCode += kTestingType;
					
				if (GetDLogControl (dialogPtr, 8))
					gListResultsSpecsPtr->areaCode += kAreaType;
				
						// Selected list results area.										
				
				gListResultsSpecsPtr->lineStart = dialogSelectArea.lineStart;
				gListResultsSpecsPtr->lineEnd = dialogSelectArea.lineEnd;
				gListResultsSpecsPtr->lineInterval =  dialogSelectArea.lineInterval;
				gListResultsSpecsPtr->columnStart = dialogSelectArea.columnStart;
				gListResultsSpecsPtr->columnEnd = dialogSelectArea.columnEnd;
				gListResultsSpecsPtr->columnInterval = dialogSelectArea.columnInterval;
					
						// Summarize by.															
				
				if (nonClusterFieldTypeCode)
					{
					gListResultsSpecsPtr->summaryCode = 0;
					if (GetDLogControl (dialogPtr, 19))
						gListResultsSpecsPtr->summaryCode += kFieldSummary;
						
					if (GetDLogControl (dialogPtr, 20))
						gListResultsSpecsPtr->summaryCode += kClassSummary;
						
					if (GetDLogControl (dialogPtr, 21))
						gListResultsSpecsPtr->summaryCode += kGroupSummary;
						
					}	// end "if (nonClusterFieldTypeCode)" 
					
						// Table type.																
					
				gListResultsSpecsPtr->tableType = 0;
				if (GetDLogControl (dialogPtr, 23))
					gListResultsSpecsPtr->tableType = 1;
					
				if (GetDLogControl (dialogPtr, 24))
					gListResultsSpecsPtr->tableType = 2;
					
						// Table units.															
				
				gListResultsSpecsPtr->unitsCode = 0;
				if (GetDLogControl (dialogPtr, 26))
					gListResultsSpecsPtr->unitsCode += kSampleUnits;
					
				if (GetDLogControl (dialogPtr, 27))
					gListResultsSpecsPtr->unitsCode += kPercentUnits;
					
				if (GetDLogControl (dialogPtr, 28))
					gListResultsSpecsPtr->unitsCode += kAreaUnits;
					
				if (gListResultsSpecsPtr->unitsCode == kAreaUnits)
					gListResultsSpecsPtr->summaryUnitsCode = kAreaUnits;
					
				else	// ...->unitsCode != kAreaUnits
					{
					if (GetDefaultAreaConversionFactor (
														gImageFileInfoPtr->mapProjectionHandle,
														gImageWindowInfoPtr->windowInfoHandle) == 0)
						gListResultsSpecsPtr->summaryUnitsCode = kPercentUnits;
															
					}	// end "else	// ...->unitsCode != kAreaUnits"
	
						// Area conversion factor.												
		
				if (saveAreaConversionFactor > 0)
					gListResultsSpecsPtr->areaConversionFactor = saveAreaConversionFactor;
						
						// Threshold results flag.												
						
				gListResultsSpecsPtr->thresholdFlag = GetDLogControl (dialogPtr, 31);
				
				if (gListResultsSpecsPtr->thresholdFlag)
					{
							// Get probability threshold.										
						
					gListResultsSpecsPtr->probabilityThreshold = saveThresholdValue;
					
							// Get the image data value which corresponds to the given
							// threshold value.
							
					Handle fileInfoHandle = GetFileInfoHandle (
											gListResultsSpecsPtr->probabilityWindowInfoHandle);
								
					gListResultsSpecsPtr->probabilityThresholdCode = GetThresholdCode (
													saveThresholdValue,
													fileInfoHandle,
													gListResultsSpecsPtr->thresholdTypeCode);
															
					}	// end "if (gListResultsSpecsPtr->thresholdFlag)" 
					
						// Write list results to output window.							
						// Write list results to disk file.									
						
				gListResultsSpecsPtr->outputStorageType = 0;							
				if (GetDLogControl (dialogPtr, 35) == 1)				
					gListResultsSpecsPtr->outputStorageType += 1;
												
				if (GetDLogControl (dialogPtr, 36) == 1)	
					gListResultsSpecsPtr->outputStorageType += 2;
												
				}	// end "if (itemHit == 1)" 
				
			if	(itemHit == 2)      // User selected Cancel for information 
				{
				modalDone = TRUE;
				returnFlag = FALSE;
				
				}	// end "if	(itemHit == 2)" 
			
			}	// end "else itemHit <= 2" 
				
		} while (!modalDone);
		
	DisposeUserItemUPP (drawDialogClassAreaPopUpPtr);
		
	CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);	
#endif	// defined multispec_mac


	#if defined multispec_win     
		CMListResultsDialog*		dialogPtr = NULL;
		
		TRY
			{ 
			dialogPtr = new CMListResultsDialog ();
			
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
	
	#if defined multispec_lin
      CMListResultsDialog*		dialogPtr = NULL;		
		try
			{
			dialogPtr = new CMListResultsDialog ();
			returnFlag = dialogPtr->DoDialog (); 		                       
			delete dialogPtr;
			}			
		catch (int e)
			{
			MemoryMessage (0, kObjectMessage);
			returnFlag = FALSE;
			}
	#endif
	
	return (returnFlag);  
	
}	// end "ListResultsDialog"



void ListResultsDialogSetThresholdItems (
				DialogPtr							dialogPtr,
				Boolean								thresholdResultsFlag,
				SInt16								thresholdTypeCode)
				
{				
	if (thresholdResultsFlag)
		{
		if (thresholdTypeCode == kMaxLikeMode)
			{
			ShowDialogItem (dialogPtr, IDC_ProbabilityPrompt);	// 32
			ShowDialogItem (dialogPtr, IDC_Probability);			// 33
			ShowDialogItem (dialogPtr, IDC_LRPercent);			// 38
			HideDialogItem (dialogPtr, IDC_LRdegrees);			// 39
			LoadDItemString (dialogPtr, IDC_ProbabilityPrompt, (Str255*)"\0Probabilities <");
			
			}	// end "if (gClassificationProcedure == kCorrelationMode)"
			
		else if (thresholdTypeCode == kCEMMode)
			{
			ShowDialogItem (dialogPtr, IDC_ProbabilityPrompt);	// 32
			ShowDialogItem (dialogPtr, IDC_Probability);			// 33
			HideDialogItem (dialogPtr, IDC_LRPercent);			// 38
			HideDialogItem (dialogPtr, IDC_LRdegrees);			// 39
			LoadDItemString (dialogPtr, IDC_ProbabilityPrompt, (Str255*)"\0CEM Values <");
			
			}	// end "else  ...!= kCorrelationMode"
			
		else	// thresholdTypeCode == kCorrelationMode
			{
			ShowDialogItem (dialogPtr, IDC_ProbabilityPrompt);	// 32
			ShowDialogItem (dialogPtr, IDC_Probability);			// 33
			HideDialogItem (dialogPtr, IDC_LRPercent);			// 38
			ShowDialogItem (dialogPtr, IDC_LRdegrees);			// 39
			LoadDItemString (dialogPtr, IDC_ProbabilityPrompt, (Str255*)"\0SAM Values >");
			
			}	// end "else  ...!= kCorrelationMode"
			
		SelectDialogItemText (dialogPtr, IDC_Probability, 0, SHRT_MAX);
		
		}	// end "if (thresholdResultsFlag && ...)" 
		
	else	// !thresholdResultsFlag 
		{
		HideDialogItem (dialogPtr, IDC_ProbabilityPrompt);	// 32
		HideDialogItem (dialogPtr, IDC_Probability);			// 33
		HideDialogItem (dialogPtr, IDC_LRPercent);			// 38
		HideDialogItem (dialogPtr, IDC_LRdegrees);			// 39
		
		}	// end "if (thresholdResultsFlag)"
	
}	// end "ListResultsDialogSetThresholdItems" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListResultsThematicClasses
//
//	Software purpose:	The purpose of this routine is to list the thematic
//							classes used in the list results listing.  The output
//							will be listed in the output text window and/or the 
//							outputdisk file as specified.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/01/1991
//	Revised By:			Larry L. Biehl			Date: 03/13/2017	

Boolean ListResultsThematicClasses (
				ClassifierVar*						clsfyVariablePtr, 
				FileInfoPtr							fileInfoPtr, 
				SInt16*								classPtr, 
				unsigned char*						symbolsPtr, 
				float*								weightsPtr, 
				UInt16								numberClasses, 
				CMFileStream*						outputFileStreamPtr, 
				SInt16*								outputCodePtr)

{
			// Declare local variables and structures	
	
	char									*classNamePtr,
											*groupNamePtr,
											*stringPtr;
									
	SInt16								*classToGroupPtr;
	
	unsigned char						*classSymbolsPtr;
									
	SInt16								groupIndex,
											identifierOffset,
											strLength;
	
	UInt16								classNumber,
											classIndex,
											startClassIndex;
											
	Boolean								continueFlag,
											foundFlag,
											groupOrderedFlag,
											listGroupColumnFlag;
	
		
			// Initialize local variables.													
	
	continueFlag = TRUE;		
	classNamePtr = (char*)GetHandlePointer (fileInfoPtr->classDescriptionH);
	classSymbolsPtr = (unsigned char*)&classNamePtr[numberClasses*32];
		
	listGroupColumnFlag = (clsfyVariablePtr->summaryCode & kGroupSummary) || 
												(clsfyVariablePtr->tableType & kGroupTable);
		
	groupOrderedFlag = (clsfyVariablePtr->tableType & kGroupTable);
	
	if (groupOrderedFlag || listGroupColumnFlag)
		{
		classToGroupPtr = GetClassToGroupPointer (fileInfoPtr);
	
				// Get a pointer to the group names information.						
				
		groupNamePtr = (char*)GetHandlePointer (fileInfoPtr->groupNameHandle);
		
		}	// end "if (groupOrderedFlag || listGroupColumnFlag)" 
		
			// Get the basic string to use.													
			// "Thematic Image Classes used"													

	MGetString (gTextString, kListResultsStrID, IDS_ListResult48);
	stringPtr = (char*)&gTextString[1];
	//sprintf (stringPtr, "    Thematic Image Classes                ");
	stringPtr += gTextString[0];
	
			// Add group column if appropriate.												

	if (listGroupColumnFlag)
		{
		sprintf (stringPtr, "\t   Group                       ");
		stringPtr += 32;
		
		}	// end "groupOrderedFlag" 
		
			// Add weight column if appropriate.											
	
	if (weightsPtr != NULL)
		{
		sprintf (stringPtr, "\t Weight");
		stringPtr += 8;
		
		}	// end "if (weightsPtr != NULL)" 
		
			// Add threshold column if appropriate.										
	/*
	if (clsfyVariablePtr->thresholdFlag)
		{
		sprintf (stringPtr, "\t Threshold");
		stringPtr += 11;
		
		}	// end "if (clsfyVariablePtr->thresholdFlag)" 
	*/
			// Add symbol column if appropriate.											
	
	if (symbolsPtr != NULL)
		{
		sprintf (stringPtr, "\tSymbol");
		stringPtr += 7;
		
		}	// end "if (symbolsPtr != NULL)" 
		
	sprintf (stringPtr, gEndOfLine);																					
	continueFlag = OutputString (outputFileStreamPtr, 
											(char*)&gTextString[1], 
											0, 
											*outputCodePtr, 
											continueFlag); 
																				
			// List the thematic image class information.								
	
	groupIndex = 0;
	startClassIndex = 0;
	
	identifierOffset = 1;
	if (classSymbolsPtr[0] == 0 || classSymbolsPtr[0] == ' ')
		identifierOffset = 0;
																	
	for (classNumber=0; classNumber<numberClasses; classNumber++)
		{         
		classIndex = classNumber;
		
		if (groupOrderedFlag)
			{
			foundFlag = FALSE;
			do
				{       
				if (startClassIndex >= numberClasses)
					{
					startClassIndex = 0;
					groupIndex++;
					
					}	// end "if (startClassIndex >= numberClasses)" 
				                       
				for (classIndex=startClassIndex; classIndex<numberClasses; classIndex++)
					{
					if (classToGroupPtr[classIndex] == groupIndex)
						{
						foundFlag = TRUE;
						startClassIndex = classIndex + 1;
						break;
					
						}	// end "if (classToGroupPtr[classIndex] == ..." 
							
					}	// end "for (classIndex=startClassIndex; ..." 
					
				if (classIndex == numberClasses)
					startClassIndex = numberClasses;
					
				}	while (!foundFlag);
				
			}	// end "if (groupOrderedFlag)" 
				
		sprintf ((char*)gTextString2, "                               ");
		pstr ((char*)gTextString2, (char*)&classNamePtr[classIndex*32], &strLength);
		gTextString2[strLength] = ' ';
		gTextString2[kMaxClassFieldNameLength] = kNullTerminator;
		stringPtr = (char*)&gTextString;
		sprintf (stringPtr,
						"    %4d: %s",
						classIndex+identifierOffset,
						(char*)gTextString2);
		stringPtr += 41;
					
				// Add group name if appropriate.											
				
		if (listGroupColumnFlag)
			{
			groupIndex = classToGroupPtr[classIndex];
			sprintf ((char*)gTextString2, "                               ");
			pstr ((char*)gTextString2, &groupNamePtr[groupIndex*32], &strLength);
			gTextString2[strLength] = ' ';
			gTextString2[kMaxClassFieldNameLength] = kNullTerminator;
			sprintf (stringPtr, "\t%3d: %s", groupIndex+1, (char*)gTextString2);
			stringPtr += 37;
			
			}	// end "if (groupOrderedFlag)" 
													
				// Add the weights if available.												
				
		if (weightsPtr != NULL)
			{
			sprintf (stringPtr, "\t%5.1f", fabs ((double)weightsPtr[classIndex]));
			stringPtr += 6;
			
			}	// end "if (weightsPtr != NULL)" 
		
				// Add symbol column if appropriate.
	
		if (symbolsPtr != NULL)
			{	
			sprintf (stringPtr, "\t %c", symbolsPtr[classIndex+1]);
			stringPtr += 3;
													
			}	// end "if (symbolsPtr != NULL)" 
			
		sprintf (stringPtr, gEndOfLine);
																										
		continueFlag = OutputString (outputFileStreamPtr, 
												(char*)gTextString, 
												0, 
												*outputCodePtr, 
												continueFlag);
		
		}	// end "for (classNumber=0;..." 
	
			// Insert a blank line.
				
	continueFlag = OutputString (outputFileStreamPtr, 
												(char*)gEndOfLine, 
												0, 
												gOutputForce1Code,
												continueFlag); 	
										
	return (continueFlag);
											
}	// end "ListResultsThematicClasses" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListResultsTrainTestFields
//
//	Software purpose:	The purpose of this routine is to ready the
//							selected training or test fields storing their
//							results into memory.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			ListResultsAreasControl
//
//	Coded By:			Larry L. Biehl			Date: 02/19/1991
//	Revised By:			Larry L. Biehl			Date: 02/13/2014

SInt16 ListResultsTrainTestFields (
				AreaDescriptionPtr				areaDescriptionPtr, 
				FileIOInstructionsPtr			fileIOInstructions1Ptr,
				FileIOInstructionsPtr			fileIOInstructions2Ptr,
				ClassifierVarPtr					clsfyVariablePtr, 
				SInt16								fieldType)

{
	CMFileStream*						listResultsFileStreamPtr;
	HPClassNamesPtr					classNamesPtr;
	HPFieldIdentifiersPtr			fieldIdentPtr;	
	
	SInt32								countVectorIndex;
	
	SInt16								*classAreaPtr,
											*classPtr;
	
	SInt16								classStorage,
											fieldCount,
											fieldNumber,
											numberFields,
											returnCode,				// =0, no error			
																		// =1, no memory for 	
																		// 		text.				
																		// =2, disk file error.	
																		// =3, user quit.	
											statClassNumber;
	
	UInt16								classIndex,
											totalCountFields;
	
	Boolean								continueFlag;
	

			// Initialize local variables.													
			
	returnCode = noErr;
	listResultsFileStreamPtr = GetResultsFileStreamPtr (0);
	continueFlag = TRUE;
	classNamesPtr = gProjectInfoPtr->classNamesPtr;
	fieldIdentPtr = gProjectInfoPtr->fieldIdentPtr;
	classAreaPtr = (SInt16*)GetHandlePointer (gListResultsSpecsPtr->classAreaHandle);
	
			// Get the appropriate class list depending on the summary and table	
			// requested.																			
			
	classPtr = (SInt16*)GetHandlePointer (gListResultsSpecsPtr->classHandle);
			
			// Clear the memory for the counts.												
	
	totalCountFields = (UInt16)
			(clsfyVariablePtr->totalCountIndexStart + clsfyVariablePtr->tableWidth); 
	for (classIndex=0; classIndex<totalCountFields; classIndex++)
		clsfyVariablePtr->countVectorPtr[classIndex] = 0;
		
			// Initialize memory for countClassIndexPtr.									
			
	for (classIndex=0; 
			classIndex<gProjectInfoPtr->numberStatisticsClasses; 
			classIndex++)
		clsfyVariablePtr->countClassIndexPtr[classIndex] = -1;
								
			// Get offsets to use for lines and columns in case needed for project fields.	

	SetAreaDescriptionOffsetVariables (areaDescriptionPtr,
														gImageWindowInfoPtr,
														fileIOInstructions1Ptr->fileInfoPtr);
			
			// Set up status dialog.  Load in number of classes.						
				
	LoadDItemValue (
		gStatusDialogPtr, IDC_Status5, (SInt32)gListResultsSpecsPtr->numberClassAreas);
		
			// Intialize the nextTime variable to indicate when the next check	
			// should occur for a command-.
	
	countVectorIndex = 0;
	for (classIndex=0; 
			classIndex<(UInt16)gListResultsSpecsPtr->numberClassAreas; 
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
		else	// fieldType == kTestType 
			numberFields = classNamesPtr[classStorage].numberOfTestFields;
			
		LoadDItemValue (gStatusDialogPtr, IDC_Status10, (SInt32)numberFields);
		LoadDItemValue (gStatusDialogPtr, IDC_Status8, (SInt32)0);
		
		fieldCount = 1;
		
		while ((fieldNumber != -1) && (returnCode == noErr))
			{
					// Make certain that field is field type.								
					
			if (fieldIdentPtr[fieldNumber].fieldType == fieldType)
				{
						// Set up field count in status dialog.							
				
				LoadDItemValue (gStatusDialogPtr, IDC_Status8, (SInt32)fieldCount);
					
						// if the field is a cluster type of field, do not classify	
						// it since there are no coordinates to define it.				
						
				if (fieldIdentPtr[fieldNumber].pointType != kClusterType)
					{
							// Get the field coordinates.										
							
					GetFieldBoundary (gProjectInfoPtr, areaDescriptionPtr, fieldNumber);
					areaDescriptionPtr->lineInterval = 1;
					areaDescriptionPtr->columnInterval = 1;
					
							// columnStart == 0 means that the field is not within the
							// area of the current image file.
										
					if (areaDescriptionPtr->columnStart > 0)
					
								// Get the results for the field.								
							
						returnCode = GetAreaResults (
											classAreaPtr[classIndex], 
											areaDescriptionPtr, 
											fileIOInstructions1Ptr,
											fileIOInstructions2Ptr,
											clsfyVariablePtr,
											&clsfyVariablePtr->countVectorPtr[countVectorIndex]);
					
							// Dispose of the region if it exists.							
	
					if (areaDescriptionPtr->rgnHandle != NULL)
						{
						DisposeRgn (areaDescriptionPtr->rgnHandle);
						areaDescriptionPtr->rgnHandle = NULL;
						
						}	// end "if (areaDescriptionPtr->rgnHandle != NULL)" 
															
							// Update count vector index to be ready for next field.	
							
					countVectorIndex += (SInt32)clsfyVariablePtr->tableWidth;
						
					}	// end "if (fieldIdentPtr[fieldNumber].pointType != ..." 
					
				fieldCount++;								
								
				}	// end "if (fieldIdentPtr[fieldNumber].field..." 
				
			fieldNumber = fieldIdentPtr[fieldNumber].nextField;
			
			}	// end "while ((fieldNumber != -1) && ..." 
			
		if (returnCode != noErr)
			break;
			
		}	// end "for (classIndex=0; classIndex<..." 	
		
	ClearAreaDescriptionOffsetVariables (areaDescriptionPtr);
		
			// List the performance table.													
	
	if (returnCode != 1)
		{
		continueFlag = ListPerformanceTables (clsfyVariablePtr,
															fieldType,
															listResultsFileStreamPtr, 
															&gOutputCode,
															classPtr,
															gListResultsSpecsPtr->numberClasses,
															classAreaPtr,
															gListResultsSpecsPtr->numberClassAreas);
	
		if (!continueFlag)
			returnCode = 1;
			
		}	// end "if (returnCode != 1)" 
		
	return (returnCode);

}	// end "ListResultsTrainTestFields" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListTableFootNotes
//
//	Software purpose:	The purpose of this routine is to load a string
//							with the title for the list results table.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/07/1998
//	Revised By:			Larry L. Biehl			Date: 01/09/1998

Boolean ListTableFootNotes (
				ClassifierVar*						clsfyVariablePtr, 
				CMFileStream*						outputFileStreamPtr,
				SInt16*								outputCodePtr)

{                                       
	Boolean								continueFlag = TRUE;
	
			
	if (clsfyVariablePtr->unitsCode == kSampleUnits &&	
														clsfyVariablePtr->totalNumberSamples > 0)
		{
		MGetString (gTextString, kListResultsStrID, IDS_ListResult43);
		continueFlag = OutputString (outputFileStreamPtr, 
												(char*)&gTextString[1], 
												0, 
												*outputCodePtr, 
												TRUE);
							
		MGetString (gTextString, kListResultsStrID, IDS_ListResult44);
		if (continueFlag)
			continueFlag = OutputString (outputFileStreamPtr, 
													(char*)&gTextString[1],
													0, 
													*outputCodePtr, 
													TRUE);
											
		}	// end "if (clsfyVariablePtr->unitsCode == kSampleUnits && ..."
											
	return (continueFlag);
																				
}	// end "ListTableFootNotes"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListThresholdValue
//
//	Software purpose:	The purpose of this routine is to list the threshold value
//							being used.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/14/1998
//	Revised By:			Larry L. Biehl			Date: 01/14/1998

Boolean ListThresholdValue (
				double								probabilityThreshold, 
				double								correlationAngleThreshold,
				double								cemThreshold,
				CMFileStream*						outputFileStreamPtr,
				SInt16*								outputCodePtr,
				Boolean								thresholdFlag,
				SInt16								thresholdTypeCode)

{
	double								value;
	
	Boolean								continueFlag = TRUE;
											
			
			// List the threshold value if being used.								
	
	if (thresholdFlag)
		{
		UInt16 stringIndex = IDS_Classify7;
		value = probabilityThreshold;
		
		if (thresholdTypeCode == kCorrelationMode)
			{
			stringIndex = IDS_Classify54;
			value = correlationAngleThreshold;
			
			}	// end "if (thresholdTypeCode == kCorrelationMode)"
			
		else if (thresholdTypeCode == kCEMMode)
			{
			stringIndex = IDS_Classify55;
			value = cemThreshold;
			
			}	// end "if (thresholdTypeCode == kCEMMode)"
				
		continueFlag = ListSpecifiedStringNumber (kClassifyStrID,
																stringIndex,
																outputFileStreamPtr, 
																*outputCodePtr, 
																value, 
																continueFlag);
		
		}	// end "if (thresholdFlag)" 
		
	return (continueFlag);
																				
}	// end "ListThresholdValue"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void LoadAreaString
//
//	Software purpose:	The purpose of this routine is to load a string
//							with the area assigned to each class.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			LoadListResultsString in SLstRslt.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/25/1991
//	Revised By:			Larry L. Biehl			Date: 08/16/2010

void LoadAreaString (
				Ptr*									stringPtrPtr, 
				HSInt64Ptr 							countVectorPtr, 
				SInt16*	 							classPtr, 
				Boolean								thresholdFlag, 
				SInt64								totalNumberPixels, 
				SInt16								numberWidth, 
				SInt16								columnPrec, 
				UInt16								numberClasses, 
				double								areaConversionFactor,
				HUInt32Ptr 							columnWidthVectorPtr)

{
	double								area;
	
	UInt32								classIndex,
											vectorIndex;
											
	SInt16								columnWidth;
											

			// List total data.																	
			
	columnWidth = (SInt16)MAX (numberWidth, kTotalNumberWidth);
	sprintf (*stringPtrPtr,
					"%*.*f ",
					columnWidth,
					columnPrec,
					totalNumberPixels * areaConversionFactor);
	*stringPtrPtr += columnWidth + 1;

			// List class data.																	
			
	for (classIndex=0; classIndex<numberClasses; classIndex++)
		{
		vectorIndex = classIndex + 1;
		if (classPtr != NULL)
			vectorIndex =  classPtr[classIndex];
			
		area = (double)countVectorPtr[vectorIndex] * areaConversionFactor;
			
		columnWidth = (SInt16)columnWidthVectorPtr[vectorIndex];
		sprintf (*stringPtrPtr,  "\t%*.*f", columnWidth, columnPrec, area);
			
		*stringPtrPtr += columnWidth + 1;

		}	// end "for (classIndex=0; classIndex<..." 
		
			// List thresholded data if requested.											
			
	if (thresholdFlag)
		{
		area = (double)countVectorPtr[0] * areaConversionFactor;
												
		columnWidth = (SInt16)columnWidthVectorPtr[0];					
		sprintf (*stringPtrPtr, "\t%*.*f", columnWidth, columnPrec, area);
		*stringPtrPtr += columnWidth + 1;
		
		}	// end "if (thresholdFlag)" 
			
}	// end "LoadAreaString" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void LoadClassInGroupOrderVector
//
//	Software purpose:	The purpose of this routine is to fill a class list
//							in group order vector.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			ListResultsControl
//
//	Coded By:			Larry L. Biehl			Date: 02/27/1991
//	Revised By:			Larry L. Biehl			Date: 11/10/1999

void LoadClassInGroupOrderVector (
				ClassifierVar*						clsfyVariablePtr,
				FileInfoPtr							fileInfoPtr, 
				SInt16*								classInGroupOrderPtr, 
				SInt16*								classPtr, 
				UInt32								numberClasses)

{  
	SInt16*								classToGroupPtr;
	                
	SInt32								group,
											numberGroups;  
									
	UInt32								classGroupIndex,
											classIndex,
											classNumber;
			
	SInt16								classToGroupOffset; 
	
	
			// Check input variables.														
	
	if (fileInfoPtr == NULL || classInGroupOrderPtr == NULL || classPtr == NULL)
																									return;
		
			// Initialize local variables.												
	
	numberGroups = fileInfoPtr->numberGroups;
	classGroupIndex = 0;
	
			// Get the class to group pointer.  										
			
	classToGroupPtr = GetClassToGroupPointer (fileInfoPtr);
	
			// Determine starting class value depending on whether the first	
			// class in the class list is the background class.  The 			
			// background class will be skipped.										
			
	classToGroupOffset = clsfyVariablePtr->backgroundIndexOffset;
			
				// Set up vector for the class symbols.								
				
	for (group=0; group<numberGroups; group++)
		{	
		for (classIndex=0; classIndex<numberClasses; classIndex++)
			{
			classNumber = classPtr[classIndex];
			
			if (classNumber <= fileInfoPtr->numberClasses-classToGroupOffset)
				{
				if (classToGroupPtr[classNumber-1+classToGroupOffset] == group)
					{
					classInGroupOrderPtr[classGroupIndex] = (SInt16)classNumber;
					classGroupIndex++;
				
					}	// end "if (classToGroupPtr... == group)" 
					
				}	// end "if (classNumber <= fileInfoPtr->..." 
			
			}	// end "for (index=0; index<numberClasses..." 
			
		}	// end "for (group=0; group<fileInfoPtr->..." 
		
			// Fill rest of 'classToGroupPtr' with 0's if it is not full.		
			
	for (; classGroupIndex<numberClasses; classGroupIndex++)
		classInGroupOrderPtr[classGroupIndex] = (SInt16)(classGroupIndex + 1);
			
}	// end "LoadClassInGroupOrderVector" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void LoadClassAreaVector
//
//	Software purpose:	The purpose of this routine is to load the class
//							area vector with a list of all class numbers in the 
//							project.
//		
//	Parameters in:				
//
//	Parameters out:			
//
// Value Returned:  	
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 06/09/1993
//	Revised By:			Larry L. Biehl			Date: 06/09/1993	

void LoadClassAreaVector (
				UInt32*								numberClassAreasPtr, 
				SInt16*								classAreaPtr)

{
	UInt32								classIndex;


			// Initialize local variables.												
			
	*numberClassAreasPtr = gProjectInfoPtr->numberStatisticsClasses;
	
	for (classIndex=0; classIndex<*numberClassAreasPtr; classIndex++)
		classAreaPtr[classIndex] = (SInt16)(classIndex + 1);
					
}	// end "LoadClassAreaVector" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean LoadListResultsSpecs
//
//	Software purpose:	The purpose of this routine is to load the List Results 
//							Specification structure with the default set of
//							parameters.
//
//	Parameters in:		Window pointer to the grafport of the active image
//							window.
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			ListResultsControl
//
//	Coded By:			Larry L. Biehl			Date: 02/14/1991
//	Revised By:			Larry L. Biehl			Date: 11/16/1999

Boolean LoadListResultsSpecs (void)

{  
	double								savedAreaConversionFactor;
	
	SInt16								*classAreaPtr,
											*classPtr;
	                                      
	UInt32								bytesNeeded,
											savedNumberClassAreas,
											savedNumberClasses;
											
	SInt16								nonClusterFieldTypeCode,
											savedAreaCode,
											savedSummaryCode,
											savedTableType,
											savedUnitsCode;
	                                     
	Boolean								changedFlag,
											projectWindowFlag,
											returnFlag,
											usePreviousSettingFlag;       
	
	
			// Initialize local variables.													
			
	returnFlag = TRUE;
	projectWindowFlag = (gProjectInfoPtr != NULL);
	usePreviousSettingFlag = FALSE;
		
			// If a list results structure already exists, then check if it is	
			// for the current window.  If not release the old structure and		
			// initialize a new one.															
			
	if (gNonProjProcessorSpecs.listResultsSpecsH != NULL)
		{
				// Lock the handle to the list results specifications and get	
				// a pointer to the structure.												
				
		gListResultsSpecsPtr = (ListResultsSpecsPtr)GetHandlePointer (
							gNonProjProcessorSpecs.listResultsSpecsH, kLock, kMoveHi);
							
				// Save those parameters that are not necessarily image dependent.
				
		savedAreaCode = gListResultsSpecsPtr->areaCode;
		savedNumberClassAreas = gListResultsSpecsPtr->numberClassAreas;
		savedNumberClasses = gListResultsSpecsPtr->numberClasses;
		
		savedSummaryCode = gListResultsSpecsPtr->summaryCode;
		savedUnitsCode = gListResultsSpecsPtr->unitsCode;
		savedTableType = gListResultsSpecsPtr->tableType;
		savedAreaConversionFactor = gListResultsSpecsPtr->areaConversionFactor;
		usePreviousSettingFlag = TRUE;
							
		if (gListResultsSpecsPtr->fileInfoHandle != gActiveImageFileInfoH || 
				gImageWindowInfoPtr->fileInfoVersion != 
													gListResultsSpecsPtr->fileInfoVersion)
			{
			ReleaseListResultsSpecsMemory (&gNonProjProcessorSpecs.listResultsSpecsH);
			gListResultsSpecsPtr = NULL;
			
			}	// end "if (gListResultsSpecsPtr->fileInfoHandle != ..." 
			
		else	// gListResultsSpecsPtr->fileInfoHandle == ... 
			{
			if (!projectWindowFlag)
				{
				gListResultsSpecsPtr->areaCode = kAreaType;
				gListResultsSpecsPtr->numberClasses = 0;
				
				}	// end "if (!projectWindowFlag)" 
				
			}	// end "else gListResultsSpecsPtr->fileInfoHandle == ..." 
				
		}	// end "else gNonProjProcessorSpecs.listResultsSpecsH != NULL"
	
	if (gNonProjProcessorSpecs.listResultsSpecsH == NULL)
		{
				// Set up handle to list results information.							
			
		gNonProjProcessorSpecs.listResultsSpecsH = 
													MNewHandle (sizeof (ListResultsSpecs));
		if (gNonProjProcessorSpecs.listResultsSpecsH != NULL)
			{
			gListResultsSpecsPtr = (ListResultsSpecsPtr)GetHandlePointer (
							gNonProjProcessorSpecs.listResultsSpecsH, kLock, kMoveHi);
			
					// Initialize the classification specification structure			
					
			gListResultsSpecsPtr->classAreaHandle = NULL;
			gListResultsSpecsPtr->classHandle = NULL;
					
			if (projectWindowFlag)
				{
						// Get information about what field types are available.
						
				nonClusterFieldTypeCode = DetermineFieldTypes ();
	
				gListResultsSpecsPtr->areaCode = 0;
				if (nonClusterFieldTypeCode & kTrainingType)
					gListResultsSpecsPtr->areaCode += kTrainingType;
	
				if (nonClusterFieldTypeCode & kTestingType)
					gListResultsSpecsPtr->areaCode += kTestingType;
					
				gListResultsSpecsPtr->numberClassAreas = 
											gProjectInfoPtr->numberStatisticsClasses;
				gListResultsSpecsPtr->numberClasses = 
											gProjectInfoPtr->numberStatisticsClasses;
											
				}	// end "if (projectWindowFlag)" 
				
			else	// !projectWindowFlag 
				{
				gListResultsSpecsPtr->areaCode = kAreaType;
				gListResultsSpecsPtr->numberClassAreas = 0;
				gListResultsSpecsPtr->numberClasses = 0;
				
				}	// end "else !projectWindowFlag" 
				
			gListResultsSpecsPtr->fileInfoHandle = gActiveImageFileInfoH;
			gListResultsSpecsPtr->fileInfoVersion = gImageWindowInfoPtr->fileInfoVersion;
				
			gListResultsSpecsPtr->thresholdFlag = FALSE;
			gListResultsSpecsPtr->classAreaSet = kAllMenuItem;
			gListResultsSpecsPtr->classSet = kAllMenuItem;
			gListResultsSpecsPtr->lineStart = 1;
			gListResultsSpecsPtr->lineEnd = gImageWindowInfoPtr->maxNumberLines;
			gListResultsSpecsPtr->lineInterval = 1;
			gListResultsSpecsPtr->columnStart = 1;
			gListResultsSpecsPtr->columnEnd = gImageWindowInfoPtr->maxNumberColumns;
			gListResultsSpecsPtr->columnInterval = 1;
			gListResultsSpecsPtr->outputStorageType = 1;
			gListResultsSpecsPtr->probabilityBufferOffset = 0;
			gListResultsSpecsPtr->probabilityThreshold = 2.;
			gListResultsSpecsPtr->probabilityThresholdCode = 0;
			gListResultsSpecsPtr->thresholdTypeCode = 0;
			gListResultsSpecsPtr->summaryUnitsCode = kPercentUnits;
			
			if (usePreviousSettingFlag)
				{
				gListResultsSpecsPtr->summaryCode = savedSummaryCode;
				gListResultsSpecsPtr->unitsCode = savedUnitsCode;
				gListResultsSpecsPtr->tableType = savedTableType;
				
				if (gImageFileInfoPtr->numberGroups == 0)
					{
					gListResultsSpecsPtr->summaryCode &= (kFieldSummary + kClassSummary);
					if (gListResultsSpecsPtr->summaryCode == 0)
						gListResultsSpecsPtr->summaryCode = kClassSummary;
					
					gListResultsSpecsPtr->tableType = kClassTable;
					
					}	// end "if (gImageFileInfoPtr->numberGroups == 0)"
					
				if (projectWindowFlag)
					{
					gListResultsSpecsPtr->areaCode = savedAreaCode;
					gListResultsSpecsPtr->numberClassAreas = savedNumberClassAreas;
					gListResultsSpecsPtr->numberClasses = savedNumberClasses;
					
					}	// end "if (projectWindowFlag)"
					
				}	// end "if (usePreviousSettingFlag)"
				
			else	// !usePreviousSettingFlag
				{
				gListResultsSpecsPtr->summaryCode = kClassSummary;
				gListResultsSpecsPtr->unitsCode = kSampleUnits;
			
				gListResultsSpecsPtr->tableType = kClassTable;
				if (gImageFileInfoPtr->numberGroups > 0)
					gListResultsSpecsPtr->tableType = kGroupTable;
				
				}	// end "else !usePreviousSettingFlag"
			
			}	// end "if (...listResultsSpecsH != NULL)" 
		
		else	// else gNonProjProcessorSpecs.listResultsSpecsH == NULL 
			returnFlag = FALSE;
			
		}	// end "if (gNonProjProcessorSpecs.listResultsSpecsH == NULL)" 
	
			// Determine if there is a probability image window open. Also set the
			// default threshold value if needed.
				
	gListResultsSpecsPtr->probabilityWindowInfoHandle = 
																GetProbabilityWindowInfoHandle ();
																
			// Get the default area conversion factor based on the map settings in the
			// image window.

	gListResultsSpecsPtr->areaConversionFactor = GetDefaultAreaConversionFactor (
															gImageFileInfoPtr->mapProjectionHandle,
															gImageWindowInfoPtr->windowInfoHandle);
									
	if (gListResultsSpecsPtr->areaConversionFactor == 0.)
		{
		gListResultsSpecsPtr->areaConversionFactor = 1.;
		
		if (usePreviousSettingFlag)
			gListResultsSpecsPtr->areaConversionFactor = savedAreaConversionFactor;
		
		}	// end "if (gListResultsSpecsPtr->areaConversionFactor == 0.)"
		
	else	// gListResultsSpecsPtr->areaConversionFactor != 0.
		gListResultsSpecsPtr->summaryUnitsCode = kAreaUnits;
					
			// Check memory for list results classes vector.							
			// Allow enough space for a class list in group order.					
		
	if (projectWindowFlag && returnFlag)
		{
		bytesNeeded = (SInt32)2 *
			gProjectInfoPtr->numberStatisticsClasses * sizeof (SInt16);
		classPtr = (SInt16*)CheckHandleSize (&gListResultsSpecsPtr->classHandle,
															&returnFlag,  
															&changedFlag, 
															bytesNeeded);
		
		gListResultsSpecsPtr->classSet = kAllMenuItem;
			
		}	// end "if (projectWindowFlag && returnFlag)" 
					
			// Check memory for list results class areas vector.						
		
	if (projectWindowFlag && returnFlag)
		{
		bytesNeeded = (SInt32)gProjectInfoPtr->numberStatisticsClasses * sizeof (SInt16);
		classAreaPtr = (SInt16*)CheckHandleSize (
														&gListResultsSpecsPtr->classAreaHandle,
														&returnFlag,   
														&changedFlag, 
														bytesNeeded);
		
		if (changedFlag)									
			gListResultsSpecsPtr->classAreaSet = kAllMenuItem;
			
		}	// end "if (projectWindowFlag && returnFlag)" 
		
	if (returnFlag)
		{
				// Load selection rectangle information into list results 			
				// specification information if the selection rectangle has 		
				// been set up																		

		GetSelectedAreaInfo (gActiveImageWindow,
									gImageWindowInfoPtr,
									&gListResultsSpecsPtr->lineStart,
									&gListResultsSpecsPtr->lineEnd,
									&gListResultsSpecsPtr->columnStart,
									&gListResultsSpecsPtr->columnEnd,
									kDontClearSelectionArea,
									kUseThreshold,
									kDontAdjustToBaseImage);
		
				// Make certain that the class vector is filled in properly.		
		
		if (projectWindowFlag && gListResultsSpecsPtr->classSet == kAllMenuItem)								
			LoadClassVector (&gListResultsSpecsPtr->numberClasses, classPtr);
			
				// Make certain that the class area vector is filled in properly.	
		
		if (projectWindowFlag && gListResultsSpecsPtr->classAreaSet == kAllMenuItem)		
			LoadClassAreaVector (
							&gListResultsSpecsPtr->numberClassAreas, classAreaPtr);
			
		}	// end "if (returnFlag)" 
	
			// If unable to set up information for list results						
			// specifications, release memory allocated to it (if any).				
			
	else	// !returnFlag 
		{
		ReleaseListResultsSpecsMemory (&gNonProjProcessorSpecs.listResultsSpecsH);
		gListResultsSpecsPtr = NULL;
		
		}	// end "else !returnFlag" 
		
	return (returnFlag);

}	// end "LoadListResultsSpecs" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void LoadListResultsString
//
//	Software purpose:	The purpose of this routine is to load a string
//							with the class values as specified by the units code.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/25/1991
//	Revised By:			Larry L. Biehl			Date: 08/13/2010	

void LoadListResultsString (
				ClassifierVar*						clsfyVariablePtr, 
				Ptr*									stringPtrPtr, 
				HSInt64Ptr 							countVectorPtr, 
				SInt16*	 							columnClassPtr, 
				SInt64								totalNumberPixels, 
				UInt32								numberClasses)

{	
			// For sample units.																
			
	if (clsfyVariablePtr->unitsCode & 0x0001)
		LoadSamplesString (stringPtrPtr, 
									countVectorPtr, 
									columnClassPtr,
									clsfyVariablePtr->thresholdFlag,
									totalNumberPixels, 
									clsfyVariablePtr->numberWidth, 
									(UInt16)numberClasses,
									clsfyVariablePtr->columnWidthVectorPtr);

			// For percent units.															
			
	if (clsfyVariablePtr->unitsCode & 0x0002)
		LoadPercentString (stringPtrPtr, 
									countVectorPtr, 
									columnClassPtr,
									clsfyVariablePtr->thresholdFlag,
									totalNumberPixels, 
									clsfyVariablePtr->numberWidth, 
									clsfyVariablePtr->numberPrecision,
									(UInt16)numberClasses,
									clsfyVariablePtr->columnWidthVectorPtr);
	
			// For user defined area units.												
			
	if (clsfyVariablePtr->unitsCode & 0x0004)
		LoadAreaString (stringPtrPtr,
								countVectorPtr,
								columnClassPtr,
								clsfyVariablePtr->thresholdFlag,
								totalNumberPixels,
								clsfyVariablePtr->numberWidth,
								clsfyVariablePtr->numberPrecision,
								(UInt16)numberClasses,
								clsfyVariablePtr->variable1,
								clsfyVariablePtr->columnWidthVectorPtr);
									
			// Add carriage return to the end of the line.								

	sprintf (*stringPtrPtr, gEndOfLine);
	*stringPtrPtr += gNumberOfEndOfLineCharacters;
			
}	// end "LoadListResultsString" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean LoadListResultsTitle
//
//	Software purpose:	The purpose of this routine is to load a string
//							with the title for the list results table.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/26/1991
//	Revised By:			Larry L. Biehl			Date: 11/23/2016

Boolean LoadListResultsTitle (
				ClassifierVar*						clsfyVariablePtr, 
				CMFileStream*						outputFileStreamPtr, 
				SInt16*					 			classPtr, 
				UInt32								numberColumns,
				SInt16								sampleNumberWidth,
				SInt16								percentWidth, 
				SInt16*								outputCodePtr)

{                                       
	Ptr									descriptionPtr,
											namePtr,
											stringPtr;
	
	Handle								descriptionHandle;
	
	UInt32								column,
											identifier,
											vectorIndex;
									
	SInt16								areaCode,
											classStorage,
											columnWidth,
											extraSpaces,
											index,
											statClassNumber,
											stringIndex1,
											strLength,
											summaryCode,
											summaryUnitsCode,
											tableType,
											unitsCode;
	                             
	Boolean								continueFlag,
											thresholdFlag,
											useLeaveOneOutMethodFlag;
	
	
			// Initialize local variables.													
	
	areaCode = clsfyVariablePtr->areaCode;
	summaryCode = clsfyVariablePtr->summaryCode;
	unitsCode = clsfyVariablePtr->unitsCode;
	summaryUnitsCode = clsfyVariablePtr->summaryUnitsCode;
	tableType = clsfyVariablePtr->tableType;
	thresholdFlag = clsfyVariablePtr->thresholdFlag;
	useLeaveOneOutMethodFlag = clsfyVariablePtr->useLeaveOneOutMethodFlag;
		
			// Write Table Title depending if it is training or test.				
			// First line of title.																
	
	stringIndex1 = IDS_ListResult1;
	
	if (!useLeaveOneOutMethodFlag)
		{
		if (areaCode == kTrainingType && summaryCode == kFieldSummary)
			stringIndex1 = IDS_ListResult1;
			
		else if (areaCode == kTrainingType && summaryCode == kClassSummary)
			stringIndex1 = IDS_ListResult2;
			
		}	// end "if (!useLeaveOneOutMethodFlag)"
		
	else	// useLeaveOneOutMethodFlag
		{
		if (areaCode == kTrainingType && summaryCode == kFieldSummary)
			stringIndex1 = IDS_ListResult38;
			
		else if (areaCode == kTrainingType && summaryCode == kClassSummary)
			stringIndex1 = IDS_ListResult39;
			
		}	// end "else useLeaveOneOutMethodFlag"
		
	if (areaCode == kTrainingType && summaryCode == kGroupSummary)
		stringIndex1 = IDS_ListResult3;
		
	else if (areaCode == kTestingType && summaryCode == kFieldSummary)
		stringIndex1 = IDS_ListResult4;
		
	else if (areaCode == kTestingType && summaryCode == kClassSummary)
		stringIndex1 = IDS_ListResult5;
		
	else if (areaCode == kTestingType && summaryCode == kGroupSummary)
		stringIndex1 = IDS_ListResult6;
		
	else if (areaCode == kAreaType && tableType == kClassTable)
		stringIndex1 = IDS_ListResult7;
		
	else if (areaCode == kAreaType && tableType == kGroupTable)
		stringIndex1 =IDS_ListResult8;
		
	MGetString (gTextString, kListResultsStrID, stringIndex1);
								
	continueFlag = OutputString (outputFileStreamPtr, 
											(char*)&gTextString[1], 
											0, 
											*outputCodePtr, 
											TRUE);
	if (!continueFlag)
																				return (FALSE);
		
			// Second line of title.															
		
	stringPtr = gCharBufferPtr1;
	
	if (areaCode != kAreaType)
		{
		MGetString (gTextString, kListResultsStrID, IDS_ListResult9);
		sprintf (stringPtr, "    %s", &gTextString[1]);
		stringPtr += 4;
		stringPtr += MIN (gTextString[0], clsfyVariablePtr->classGroupNameWidth); 
		
				// Add 6 spaces and 2 tabs.
				
		sprintf (stringPtr, "       \t\t");
		stringPtr += 8;      
		
		if (unitsCode == kSampleUnits)
			{
			MGetString (gTextString, kListResultsStrID, IDS_ListResult42);
			sprintf (stringPtr, "%s", &gTextString[1]);
			stringPtr += gTextString[0];  
			
			}	// end "if (unitsCode == kSampleUnits)"            
		
		stringIndex1 = IDS_ListResult10;
		if (unitsCode == kSampleUnits)
			stringIndex1 = IDS_ListResult10;
			
		else if (unitsCode == kPercentUnits)
			stringIndex1 = IDS_ListResult11;
			
		else if (unitsCode == kAreaUnits)
			stringIndex1 = IDS_ListResult12;
			
		MGetString (gTextString, kListResultsStrID, stringIndex1);
		sprintf (stringPtr, "%s", &gTextString[1]);
		stringPtr += gTextString[0];
			
		stringIndex1 = IDS_ListResult13;
		if (gProcessorCode == kClassifyProcessor)
			{
			if (tableType == kClassTable)
				stringIndex1 = IDS_ListResult13;
				
			else if (tableType == kGroupTable)
				stringIndex1 = IDS_ListResult14;
				
			}	// end "if (gProcessorCode == kClassifyProcessor)"
		
		else if (gProcessorCode == kListResultsProcessor)
			{
			if (tableType == kClassTable)
				stringIndex1 = IDS_ListResult15;
				
			else if (tableType == kGroupTable)
				stringIndex1 = IDS_ListResult16;
				
			}	// end "else if (gProcessorCode == kListResultsProcessor)"
			
		MGetString (gTextString, kListResultsStrID, stringIndex1);
		sprintf (stringPtr, "%s%s", &gTextString[1], gEndOfLine); 
		
		}	// end "if (areaCode != kAreaType)"
		
	else	// areaCode == kAreaType
		{	  																	
			// Write 2nd & 3rd lines of title for selected area summary.
			
				// Get string for class or group.
				                    
		stringIndex1 = IDS_ListResult13;
		if (tableType == kClassTable)
			stringIndex1 = IDS_ListResult13;
				
		else if (tableType == kGroupTable)
			stringIndex1 = IDS_ListResult14;
			
		MGetString (gTextString2, kListResultsStrID, stringIndex1);
		
				// Get string for List Results or Classify Processor
			
		stringIndex1 = IDS_ListResult34;
		if (gProcessorCode == kClassifyProcessor)
			stringIndex1 = IDS_ListResult34; 
		
		else if (gProcessorCode == kListResultsProcessor)
			stringIndex1 = IDS_ListResult35; 
			
		MGetString (gTextString, kListResultsStrID, stringIndex1); 
		                   
		strLength = clsfyVariablePtr->classGroupNameWidth - 12;
		strLength = sprintf (stringPtr, 
										(char*)&gTextString[1],
										strLength,
										"",
										sampleNumberWidth-8,
										"", 
										gEndOfLine,
										&gTextString2[1],
										strLength,
										"",
										sampleNumberWidth-7,
										"",
										percentWidth-7,
										"");
						
		stringPtr += strLength;  
		
				// Get string for Units
			
		stringIndex1 = IDS_ListResult36; 
		if (summaryUnitsCode != kAreaUnits)             
			stringIndex1 = IDS_ListResult36; 
		
		else if (summaryUnitsCode == kAreaUnits)
			stringIndex1 = IDS_ListResult37; 
			
		MGetString (gTextString, kListResultsStrID, stringIndex1); 
		
		strLength = sprintf (stringPtr,
										(char*)&gTextString[1],
										gEndOfLine);
		stringPtr += strLength; 
						
		if (summaryUnitsCode == kAreaUnits)
			{
					//	Get the units for the area string. Note that hectares is used
					// unless the display area units options is set to something other 
					// than number of pixels.
					
			double areaConversionFactor = GetDefaultAreaConversionFactor (
														gImageFileInfoPtr->mapProjectionHandle,
														gImageWindowInfoPtr->windowInfoHandle);
			
			if (fabs (clsfyVariablePtr->variable1-areaConversionFactor) < 0.0000001)
				GetAreaUnitString (gActiveImageWindowInfoH, FALSE, (char*)gTextString);
				
			else	// user supplied conversion factor. Do not know units.
				sprintf ((char*)&gTextString[1], "unknown units");
			
			stringPtr -= gNumberOfEndOfLineCharacters;
			strLength = sprintf (stringPtr,
											" (%s)%s",
											(char*)&gTextString[1],
											gEndOfLine);
			
			}	// end "if (summaryUnitsCode == kAreaUnits)"
						
		}	// end "else areaCode == kAreaType"
								
	continueFlag = OutputString (outputFileStreamPtr, 
												gCharBufferPtr1, 
												0, 
												*outputCodePtr, 
												TRUE);
	if (!continueFlag)
																					return (FALSE);
	
			// If this is the title line for a selected area summary, then
			// we are done.
				
	if (areaCode == kAreaType) 
																					return (TRUE);
		
		
			// Third line of title.	
			
			// Get the pointer to the column widths.
			
	HUInt32Ptr columnWidthVectorPtr = clsfyVariablePtr->columnWidthVectorPtr;
															
	
			// Get pointer to column names for 'List Results' Processor.  When		
			// called by 'Classify' Processor, the project names will be used.		
	
	descriptionHandle = NULL;
	descriptionPtr = NULL;	
	if (gProcessorCode == kListResultsProcessor)
		{
		FileInfoPtr fileInfoPtr = gImageFileInfoPtr;
												
		if (tableType == kClassTable)
			descriptionHandle = fileInfoPtr->classDescriptionH;
			
		else	// tableType == kGroupTable) 
			descriptionHandle = fileInfoPtr->groupNameHandle;
			
		if (descriptionHandle != NULL)
			descriptionPtr = (char*)GetHandlePointer (descriptionHandle);
			
		if (descriptionPtr == NULL)
																						return (FALSE);
		
		}	// end "if (gProcessorCode == kListResultsProcessor)" 
		
	stringPtr = gCharBufferPtr1;
	
	if (areaCode != kAreaType)
		{
				// Load the first part of the title for training/test areas.	
				
		stringIndex1 = IDS_ListResult17;
		if (summaryCode == kFieldSummary)
			stringIndex1 = IDS_ListResult17;
			
		else if (summaryCode == kClassSummary)
			stringIndex1 = IDS_ListResult18;
			
		else if (summaryCode == kGroupSummary)
			stringIndex1 = IDS_ListResult19;
		
		MGetString (gTextString, kListResultsStrID, stringIndex1);
	
		sprintf (stringPtr, "    %s", &gTextString[1]);
		stringPtr += 4;
		stringPtr += MIN (gTextString[0], clsfyVariablePtr->classGroupNameWidth); 
		
		stringIndex1 = IDS_ListResult20;
		if (tableType == kClassTable && summaryCode != kGroupSummary)
			stringIndex1 = IDS_ListResult20;
			
		else	// tableType == kGroupTable || summaryCode == kGroupSummary
			stringIndex1 = IDS_ListResult21;
		
		MGetString (gTextString, kListResultsStrID, stringIndex1);
	
		sprintf (stringPtr, "%s", &gTextString[1]);
		stringPtr += gTextString[0];
		/*
		stringIndex1 = IDS_ListResult17;
		if (summaryCode == kFieldSummary && tableType == kClassTable)
			stringIndex1 = IDS_ListResult17;
			
		if (summaryCode == kClassSummary && tableType == kClassTable)
			stringIndex1 = IDS_ListResult18;
			
		if (summaryCode == kFieldSummary && tableType == kGroupTable)
			stringIndex1 = IDS_ListResult20;
			
		if (summaryCode == kClassSummary && tableType == kGroupTable)
			stringIndex1 = IDS_ListResult21;
			
		if (summaryCode == kGroupSummary)
			stringIndex1 = IDS_ListResult22;
		
		MGetString (gTextString, kListResultsStrID, stringIndex1);
	
		sprintf (stringPtr, "%s", &gTextString[1]);
		stringPtr += gTextString[0];
		*/
				// Load Percent Correct column if needed.									
				
		if (unitsCode == kSampleUnits)
			{
			stringIndex1 = IDS_ListResult23;
			MGetString (gTextString, kListResultsStrID, stringIndex1);
	
			sprintf (stringPtr, "%s", &gTextString[1]);
			stringPtr += gTextString[0];
			
			}	// end "if (unitsCode == kSampleUnits)" 
		
		}	// end "if (areaCode != kAreaType)"
			
			// Load the Total Number or Area column.										
			
	stringIndex1 = IDS_ListResult24;
	if (unitsCode == kSampleUnits || unitsCode == kPercentUnits)
		stringIndex1 = IDS_ListResult24;
		
	if (unitsCode == kAreaUnits)
		stringIndex1 = IDS_ListResult25;
		
	MGetString (gTextString, kListResultsStrID, stringIndex1);
		
			// Determine if extra spaces are needed before the column heading.
	
	extraSpaces = clsfyVariablePtr->numberWidth - 7;
	if (extraSpaces > 0)
		{
		sprintf ((char*)gTextString2, "                         ");
		gTextString2[extraSpaces] = 0;
		sprintf (stringPtr, "%s", (char*)gTextString2);
		stringPtr += extraSpaces;
		
		}	// end "if (extraSpaces > 0)" 
	
	sprintf (stringPtr, "%s", &gTextString[1]);
	stringPtr += gTextString[0];
	
			// Set start value for class header number.  If class list and first	
			// class has a symbol of 0 or blank then assume that this is the		
			// background class.  Start with an index of 0;								
			
	identifier = 1;
	if (clsfyVariablePtr->backgroundIndexOffset == 1 && 
											clsfyVariablePtr->tableType == kClassTable)
		identifier = 0;
		
	for (column=0; column<numberColumns; column++)
		{
		vectorIndex = column + 1;
		if (gProcessorCode == kClassifyProcessor)
			vectorIndex = identifier = classPtr[column];
			
		columnWidth = (SInt16)columnWidthVectorPtr[vectorIndex];
		sprintf (stringPtr,  "\t%*ld", columnWidth, identifier);
			
		stringPtr += columnWidth + 1;
		
		identifier++;
		
		}	// end "for (column=0; column<..." 
	
	if (thresholdFlag)
		{
		sprintf (stringPtr,  "\tThreshold");
		stringPtr += 10;
		
		}	// end "if (thresholdFlag)" 
		
	sprintf (stringPtr, gEndOfLine);
								
	continueFlag = OutputString (outputFileStreamPtr, 
											gCharBufferPtr1, 
											0, 
											*outputCodePtr, 
											TRUE);
	if (!continueFlag)
																				return (FALSE);
		
		
			// Fourth line of title.															
		
	stringPtr = gCharBufferPtr1;
	
	if (areaCode != kAreaType)
		{
		stringIndex1 = IDS_ListResult26;
		MGetString (gTextString, kListResultsStrID, stringIndex1);
	
		sprintf (stringPtr, "    %s", &gTextString[1]);
		stringPtr += 4;
		stringPtr += MIN (gTextString[0], clsfyVariablePtr->classGroupNameWidth);
		
		stringIndex1 = IDS_ListResult27;
		MGetString (gTextString, kListResultsStrID, stringIndex1);
	
		sprintf (stringPtr, "%s", &gTextString[1]);
		stringPtr += gTextString[0]; 
		
				// Load Percent Correct column if needed.									
				
		if (unitsCode == kSampleUnits)
			{
			stringIndex1 = IDS_ListResult28;
			MGetString (gTextString, kListResultsStrID, stringIndex1);
	
			sprintf (stringPtr, "%s", &gTextString[1]);
			stringPtr += gTextString[0];
			
			}	// end "if (unitsCode == kSampleUnits)" 
		
		}	// end "if (areaCode != kAreaType)" 
			
			// Load the Total Number or Area column.										
			
	stringIndex1 = IDS_ListResult29;
	if (unitsCode == kSampleUnits || unitsCode == kPercentUnits)
		stringIndex1 = IDS_ListResult29;
		
	if (unitsCode == kAreaUnits)
		stringIndex1 = IDS_ListResult30;
		
			// Add extra spaces if needed.												
			
	if (extraSpaces > 0)
		{
		sprintf ((char*)gTextString2, "                         ");
		gTextString2[extraSpaces] = 0;
		sprintf (stringPtr, "%s", (char*)gTextString2);
		stringPtr += extraSpaces;
		
		}	// end "if (extraSpaces > 0)" 
		
	MGetString (gTextString, kListResultsStrID, stringIndex1);
	
	sprintf (stringPtr, "%s", &gTextString[1]);
	
	stringPtr += gTextString[0];
	
	for (column=0; column<numberColumns; column++)
		{
		sprintf ((char*)gTextString2, "                ");
			
		if (classPtr != NULL)
			{
					// Handle project class name.													
					
			statClassNumber = classPtr[column] - 1;
			
					// Get the class storage number.												
							
			classStorage = gProjectInfoPtr->storageClass[statClassNumber];
			
			namePtr = (char*)&gProjectInfoPtr->classNamesPtr[classStorage].name;
			
			vectorIndex = classPtr[column];
			
			}	// end "if (classPtr != NULL)" 
			
		else	// classPtr == NULL 
			{
			namePtr = descriptionPtr;
			descriptionPtr += 32;
			
			vectorIndex = column + 1;
			
			}	// end "classPtr == NULL" 
					
				// Load the column name into a string.  Only load the first				
				// 'columnWidth' characters in.  If field name is less than 			
				// 'columnWidth' characters, pad the first part of the string with blanks.					
		
		columnWidth = (SInt16)columnWidthVectorPtr[vectorIndex];
		index = MAX (0, columnWidth - namePtr[0]);
		pstr ((char*)&gTextString2[index], namePtr, &strLength);
		gTextString2[columnWidth] = kNullTerminator;
		
		sprintf (stringPtr, "\t%s", gTextString2);
		
		stringPtr += columnWidth + 1;
		
		}	// end "for (column=0; column<..." 
		
	sprintf (stringPtr, "%s%s", gEndOfLine, gEndOfLine);
								
	continueFlag = OutputString (outputFileStreamPtr, 
											gCharBufferPtr1, 
											0, 
											*outputCodePtr, 
											TRUE);
																				
	return (continueFlag);
	
}	// end "LoadListResultsTitle" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void LoadPercentString
//
//	Software purpose:	The purpose of this routine is to load a string
//							with the percent of total points assigned to each
//							class.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/25/1991
//	Revised By:			Larry L. Biehl			Date: 08/14/2010	

void LoadPercentString (
				Ptr*									stringPtrPtr, 
				HSInt64Ptr 							countVectorPtr, 
				SInt16*	 							classPtr,
				Boolean								thresholdFlag, 
				SInt64								totalNumberPixels, 
				SInt16								numberWidth, 
				SInt16								columnPrec, 
				UInt16								numberClasses,
				HUInt32Ptr 							columnWidthVectorPtr)

{
	double								factor,
											percent;
	
	UInt32								classIndex, 
											vectorIndex;
											
	SInt16								columnWidth;
											
	
			// Initialize local variables.													
	
	factor = 0;
	if (totalNumberPixels > 0)	
		factor = (double)100 / (double)totalNumberPixels;

			// List total data.																	
			
	columnWidth = (SInt16)MAX (numberWidth, kTotalNumberWidth);
	sprintf (*stringPtrPtr, "%*lld ", columnWidth, totalNumberPixels);
	*stringPtrPtr += columnWidth + 1;

			// List class data.																	
			
	for (classIndex=0; classIndex<numberClasses; classIndex++)
		{
		vectorIndex = classIndex + 1;
		if (classPtr != NULL)
			vectorIndex =  classPtr[classIndex];
			
		percent = (double)countVectorPtr[vectorIndex] * factor;
			
		columnWidth = (SInt16)columnWidthVectorPtr[vectorIndex];
		sprintf (*stringPtrPtr,  "\t%*.*f", columnWidth, columnPrec, percent);
		*stringPtrPtr += columnWidth + 1;

		}	// end "for (classIndex=0; classIndex<..." 
		
	if (thresholdFlag)
		{
		percent = (double)countVectorPtr[0] * factor;
																
		columnWidth = (SInt16)columnWidthVectorPtr[0];	
		sprintf (*stringPtrPtr, "\t%*.*f", columnWidth, columnPrec, percent);
		*stringPtrPtr += columnWidth + 1;
		
		}	// end "if (thresholdFlag)" 
			
}	// end "LoadPercentString" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void LoadSamplesString
//
//	Software purpose:	The purpose of this routine is to load a string
//							with the number of samples assigned to each
//							class.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/25/1991
//	Revised By:			Larry L. Biehl			Date: 08/13/2010

void LoadSamplesString (
				Ptr*									stringPtrPtr, 
				HSInt64Ptr 							countVectorPtr, 
				SInt16*	 							classPtr,
				Boolean								thresholdFlag, 
				SInt64								totalNumberPixels, 
				SInt16								numberWidth, 
				UInt16								numberClasses,
				HUInt32Ptr 							columnWidthVectorPtr)

{
	UInt32								classIndex,  
											vectorIndex;
											
	SInt16								columnWidth;
		
	
			// List total data.																	
			
	columnWidth = (SInt16)MAX (numberWidth, kTotalNumberWidth);
	sprintf (*stringPtrPtr, "%*lld ", columnWidth, totalNumberPixels);
	*stringPtrPtr += columnWidth + 1;

			// List class data.																	
			
	for (classIndex=0; classIndex<numberClasses; classIndex++)
		{
		vectorIndex = classIndex + 1;
		if (classPtr != NULL)
			vectorIndex =  classPtr[classIndex];
			
		columnWidth = (SInt16)columnWidthVectorPtr[vectorIndex];
			
		sprintf (*stringPtrPtr,  
						"\t%*lld", 
						columnWidth, 
						countVectorPtr[vectorIndex]);
			
		*stringPtrPtr += columnWidth + 1;

		}	// end "for (classIndex=0; classIndex<..." 
		
	if (thresholdFlag)
		{
		columnWidth = (SInt16)columnWidthVectorPtr[0];
		sprintf (*stringPtrPtr, "\t%*lld", columnWidth, countVectorPtr[0]);
		*stringPtrPtr += columnWidth + 1;
		
		}	// end "if (thresholdFlag)" 
			
}	// end "LoadSamplesString"
 
