//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SProjectHistogramStatistics.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			10/22/2018
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	This file contains functions that determine, print and
//								plot the field and class histograms.
//
//	Functions in file:	Boolean 				CheckSizeOfStatHistogramGraphVectors
//								void 					GetSumOfClassFieldColumns
//								SInt16	 			HistogramClassStats
//								SInt16	 			HistogramFieldStats
//								SInt16	 			HistogramProjectStats
//								SInt16	 			HistogramStatsControl
//								void					InitializeStatHistogramBuffers
//								Boolean 				ListChannelInformation
//								void 					ListStatHistogramValues
//								Boolean 				LoadGraphData
//								Boolean				LoadStatHistogramSpecs
//								Boolean 				SetupHistogramStatsMemory
//								Boolean 				StatHistogramDialog
//								UInt32	 			stci_d
//								UInt32 				stc_d
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h" 

#if defined multispec_lin
   #include "LGraphDoc.h"
   #include "LGraphFrame.h"
   #include "LGraphView.h"
   #include "LMultiSpec.h"
   #include "LStatisticsHistogramDialog.h"
#endif  
	
#if defined multispec_mac || defined multispec_mac_swift
	#include	"MGraphView.h"

	#define IDC_ClassesRadio					4
	#define IDC_FieldsRadio						5
	#define IDC_ChannelsPrompt					11
	#define IDC_ListRadio						14
	#define IDC_IncludeEmptyCheck				15
	#define IDC_BlankCheck						16
	#define IDC_PlotRadio						17
	#define IDC_ChannelsRadio					19
	#define IDC_Classes							20
	#define IDC_MatrixRadio						22
	#define IDC_ColumnRadio						23
	#define IDC_UseFeatureTransformation	25
	#define IDC_OverlayCheck					26
#endif	// defined multispec_mac || defined multispec_mac_swift
  
#if defined multispec_win
	#include	"WGraphView.h"
	
	#include "WMultiSpec.h"
	#include "WStatisticsHistogramDialog.h"
	#include "WGraphDoc.h"
#endif	// defined multispec_win  

//#include "SExtGlob.h"

#define	kMatrixList					1

extern void StatHistogramDialogInitialize (
				DialogPtr							dialogPtr,
				FileInfoPtr							fileInfoPtr,
				SInt16								statsWindowMode,
				UInt16*								localFeaturesPtr, 
				UInt16*								localTransformFeaturesPtr,
				Boolean*								histogramClassFlagPtr,
				SInt32*								lineIntervalPtr,
				SInt32*								columnIntervalPtr,
				Boolean*								featureTransformAllowedFlagPtr,
				Boolean*								featureTransformationFlagPtr,
				SInt16*								channelSelectionPtr,
				UInt16*								localNumberChannelsPtr,
				SInt16*								histogramOutputCodePtr,
				Boolean*								includeEmptyBinsFlagPtr,
				Boolean*								blankValuesFlagPtr,
				SInt16*								groupCodePtr,
				SInt16*								columnMatrixCodePtr,
				Boolean*								overlayDFAllowedFlagPtr,
				Boolean*								overlayDensityFunctionFlagPtr);	
												
extern void StatHistogramDialogOK (
				Boolean								histogramClassFlag,
				SInt32								lineInterval,
				SInt32								columnInterval,
				Boolean								featureTransformationFlag,
				SInt16								channelSelection,
				UInt16								localNumberFeatures,
				UInt16*								localFeaturesPtr,
				Boolean								listDataFlag,
				Boolean								includeEmptyBinsFlag,
				Boolean								blankValuesFlag,
				Boolean								plotHistogramFlag,
				Boolean								groupChannelsFlag,
				Boolean								groupFieldsClassesFlag,
				SInt16								columnMatrixCode,
				Boolean								overlayDensityFunctionFlag);

extern void StatHistogramDialogClassesParameters (
				DialogPtr							dialogPtr,
				Boolean								listFlag,
				Boolean								overlayDFAllowedFlag,
				Boolean								overlayDensityFunctionFlag);
							
extern void StatHistogramDialogFieldsParameters (
				DialogPtr							dialogPtr);
							
extern void StatHistogramDialogListParameters (
				DialogPtr							dialogPtr,
				Boolean								histogramClassFlag,
				SInt16								columnMatrixCode);
							
extern void StatHistogramDialogPlotParameters (
				DialogPtr							dialogPtr,
				Boolean								histogramClassFlag,
				Boolean								overlayDensityFunctionFlag);


			// Prototypes for routines in this file that are only called by		
			// other routines in this file.		

Boolean 	CheckIfCanMatrixChannels (
				HistogramSummaryPtr				histogramRangePtr,
				UInt32								numberColumns);

Boolean 	CheckSizeOfStatHistogramGraphVectors (
				GraphPtr								graphRecordPtr,
				HistogramSummary*					histogramRangePtr,
				HUInt32Ptr							histogramDataPtr,
				UInt32								localNumberChannels,
				UInt32								histogramIndex,
				UInt32								numberBins);	

UInt32 	GetBinIndexForStatDataValue (
				double								dataValue,
				HistogramSummaryPtr				histogramSummaryPtr);
							
double 	GetDataValueForStatBinIndex (
				UInt32								binIndex,
				HistogramSummaryPtr				histogramSummaryPtr);	

void 		GetEstimatedTransformedChannelMinMaxes (
				UInt16								numberOutputChannels, 
				UInt16*								channelListPtr, 
				HDoublePtr							channelMinPtr, 
				HDoublePtr							channelMaxPtr,
				double*								overallMinPtr,
				double*								overallMaxPtr);					
			
void	 	GetSumOfClassFieldColumns (
				FileInfoPtr							fileInfoPtr, 
				HistogramSummaryPtr				histogramRangePtr, 
				HUInt32Ptr							histogramDataPtr, 
				UInt32								numberColumns);

void 		GetTransformedChannelMinMaxes (
				UInt16								numberOutputChannels, 
				UInt16*								channelListPtr,
				FileIOInstructionsPtr			fileIOInstructionsPtr, 
				SInt16								outputGroupCode,
				SInt16								statsWindowMode,
				HDoublePtr							channelMinPtr, 
				HDoublePtr							channelMaxPtr,
				double*								overallMinPtr,
				double*								overallMaxPtr);
											
SInt16	HistogramClassStats (
				FileIOInstructionsPtr			fileIOInstructionsPtr, 
				SInt16								classNumber, 
				SInt16								outputGroupCode, 
				UInt32*								indexStartPtr, 
				UInt16								channelStart);

SInt16	HistogramFieldStats (
				FileIOInstructionsPtr			fileIOInstructionsPtr, 
				SInt16								fieldNumber, 
				SInt16								outputGroupCode, 
				UInt32								indexStart, 
				UInt16								channelStart);

SInt16	HistogramProjectStats (
				FileIOInstructionsPtr			fileIOInstructionsPtr,  
				SInt16								outputGroupCode);
							
void 		InitializeStatHistogramBuffers (
				FileInfoPtr							fileInfoPtr,
				SInt16								outputGroupCode,
				UInt32								numberColumns,
				UInt16*								featurePtr,
				UInt32								channelSet,  
				HDoublePtr							channelMinPtr, 
				HDoublePtr							channelMaxPtr);
															
Boolean 	ListChannelInformation (
				FileInfoPtr							fileInfoPtr, 
				SInt32								channelNum,
				Boolean								featureTransformationFlag);

Boolean 	ListStatHistogramValues (
				FileInfoPtr							fileInfoPtr, 
				SInt16*								featurePtr,
				Boolean								featureTransformationFlag, 
				HistogramSummaryPtr				histogramRangePtr, 
				HUInt32Ptr							histogramDataPtr, 
				UInt32								numberColumns, 
				SInt32								totalNumberValues, 
				SInt16								outputCode);

Boolean 	LoadGraphData (
				FileInfoPtr							fileInfoPtr,
				SInt16								indexStart);

void 		LoadProjectClassFieldNames (
				Str31*								stringPtr,
				Boolean								classNameFlag);

Str31* 	LoadClassClassFieldNames (
				Str31*								stringPtr,
				SInt16								classNumber,
				Boolean								classNameFlag);

Boolean	LoadStatHistogramSpecs (
				FileInfoPtr							fileInfoPtr);
																		
Boolean	StatHistogramDialog (
				FileInfoPtr							fileInfoPtr);		

void 		SetSomeStatHistogramGraphParameters (
				GraphPtr								graphRecordPtr,
				SInt16								statsWindowMode);

Boolean	SetupStatHistogramMemory (
				FileInfoPtr							fileInfoPtr, 
				SInt16								statsWindowMode);
							
UInt32 	stc_d (
				char*									out, 
				UInt32						 		in, 
				SInt16			 					outlen, 
				SInt16 								signflag);



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CheckIfCanMatrixChannels
//
//	Software purpose:	The purpose of this routine is to determine if the bin widths
//							and the starting bin value are the same for all channels.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/13/2006
//	Revised By:			Larry L. Biehl			Date: 02/15/2006	

Boolean CheckIfCanMatrixChannels (
				HistogramSummaryPtr				histogramRangePtr,
				UInt32								numberColumns)

{
	UInt32								index;
	
	Boolean								canMatrixChannelsFlag = TRUE;
	
																							
	if (histogramRangePtr == NULL)
																							return (FALSE);
	
			// If data value is bin index, then this is the same for all channels.
																									
	if (histogramRangePtr[0].binType == kDataValueIsBinIndex)
																		return (canMatrixChannelsFlag);
																							
	for (index=1; index<numberColumns; index++)
		{		
		if (histogramRangePtr[0].binFactor != histogramRangePtr[index].binFactor)
			{
			canMatrixChannelsFlag = FALSE;
			break;
			
			}	// end "if (histogramRangePtr[0].binFactor != ..."
					
		}	// end "for (index=0; index<..." 
		
			// Now verify that the starting bin value is the same for all channels.
		
	if (canMatrixChannelsFlag)	
		{
		for (index=1; index<numberColumns; index++)
			{		
			if (histogramRangePtr[0].minNonSatValue != 
															histogramRangePtr[index].minNonSatValue)
				{
				canMatrixChannelsFlag = FALSE;
				break;
				
				}	// end "if (histogramRangePtr[0].binFactor != ..."
						
			}	// end "for (index=0; index<..." 
		
		}	// end "if (canMatrixChannelsFlag)"
		
	return (canMatrixChannelsFlag);
			
}	// end "CheckIfCanMatrixChannels" 


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt32 CheckSizeOfStatHistogramGraphVectors
//
//	Software purpose:	The purpose of this routine is to compute the sum of
//							the columns for class/field grouping.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/28/1999
//	Revised By:			Larry L. Biehl			Date: 01/26/2006	

Boolean CheckSizeOfStatHistogramGraphVectors (
				GraphPtr								graphRecordPtr,
				HistogramSummary*					histogramRangePtr,
				HUInt32Ptr							histogramDataPtr,
				UInt32								localNumberChannels,
				UInt32								histogramIndex,
				UInt32								numberBins)

{	
	SInt32								error;
	
	UInt32								binIndex,
											index,
											maxIndex,
											minIndex,
											numberDataValues;
	
		
	numberDataValues = graphRecordPtr->xVector.size;
						
	for (index=0; index<localNumberChannels; index++)
		{
				// Get the min and max data value index and then just loop throught the
				// histogram data vector between those 2 values to load the graphics
				// vector.
				
		minIndex = GetBinIndexForStatDataValue (histogramRangePtr[index].minValue,
															&histogramRangePtr[index]);
		maxIndex = GetBinIndexForStatDataValue (histogramRangePtr[index].maxValue,
															&histogramRangePtr[index]);
		
		for (binIndex=minIndex; binIndex<=maxIndex; binIndex++)
			{
			if (histogramDataPtr[histogramIndex + binIndex] > 0)
				numberDataValues++;	
			
			}	// end "for (m=minValue; m<=maxValue; m++)"
			
		histogramIndex += numberBins;
		
		}	// end "for (index=0; index<localNumberChannels; index++)"
		
			// Now change the handle size to allow for these new points to be added.
			
	SetGraphVectorSize (&graphRecordPtr->xVector, 
								numberDataValues, 
								&error);
		
			// Now change the handle size to allow for these new points to be added.
			
	if (error == noErr)
		SetGraphVectorSize (&graphRecordPtr->yVector, 
									numberDataValues, 
									&error);
		
	return (error==noErr);
											
}	// end "CheckSizeOfStatHistogramGraphVectors"       



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ForceStatHistogramCodeResourceLoad
//
//	Software purpose:	The purpose of this routine is to force the 'Graphics' code
//							resource to be loaded into memory.  It is called by any
//							processor that needs graphics code resource.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/24/2002
//	Revised By:			Larry L. Biehl			Date: 12/24/2002

void ForceStatHistogramCodeResourceLoad (void)

{
	
			// If spare memory had to be used to load code resources, then exit	
			// routine.																				
			
	if (gMemoryTypeNeeded < 0)
																									return;
																							
			// Code resources loaded okay, so set flag back for non-Code			
			// resources.																			
			
	gMemoryTypeNeeded = 0;

}	// end "ForceStatHistogramCodeResourceLoad"


	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		double GetBinIndexForStatDataValue
//
//	Software purpose:	This routine determines the data value for the input index into
//							the histogram array depending upon the type of array.
//
//	Parameters in:					
//
//	Parameters out:	None				
//
// Value Returned:	Data value	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/26/2006
//	Revised By:			Larry L. Biehl			Date: 01/26/2006

UInt32 GetBinIndexForStatDataValue (
				double								dataValue,
				HistogramSummaryPtr				histogramSummaryPtr)

{
	SInt32								binIndex,
											binType;
											
	
	binType = histogramSummaryPtr->binType;
	
	if (binType == kBinWidthOfOne)
		binIndex = (SInt32)(dataValue - histogramSummaryPtr->minNonSatValue) + 1;
		
	else if (binType == kDataValueIsBinIndex)
		binIndex = (SInt32)dataValue;
		
	else	// binType != kBinWidthOfOne
		binIndex =
			(SInt32)((dataValue - histogramSummaryPtr->minNonSatValue) * 
														histogramSummaryPtr->binFactor) + 1;
			
	binIndex = MAX (0, binIndex);
	binIndex = MIN (binIndex, (SInt32)histogramSummaryPtr->numberBins-1);
		
	return ((UInt32)binIndex);
		
}	// end "GetBinIndexForStatDataValue" 


	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		double GetDataValueForStatBinIndex
//
//	Software purpose:	This routine determines the data value for the input index into
//							the medianArray depending upon the type of array.
//
//	Parameters in:					
//
//	Parameters out:	None				
//
// Value Returned:	Data value	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/26/2006
//	Revised By:			Larry L. Biehl			Date: 02/07/2006

double GetDataValueForStatBinIndex (
				UInt32								binIndex,
				HistogramSummaryPtr				histogramSummaryPtr)

{
	double								dataValue;
	
	UInt32								binType;
	
	
	binType = histogramSummaryPtr->binType;
	
	if (binType == kDataValueIsBinIndex)
		dataValue = (SInt32)binIndex;
		
	else	// binType != kDataValueIsBinIndex
		{
		if (binIndex <= 0)
			dataValue = histogramSummaryPtr->minNonSatValue;
			
		else if (binIndex >= histogramSummaryPtr->numberBins-1)
			dataValue = histogramSummaryPtr->maxNonSatValue;
			
		else	// binIndex > 0 && <...numberBins-1
			{
			binIndex--;
			if (binType == kBinWidthOfOne)
				dataValue = binIndex + histogramSummaryPtr->minNonSatValue;
			
			else	// binType != kBinWidthOfOne
				dataValue = ((double)binIndex+.5)/histogramSummaryPtr->binFactor + 
															histogramSummaryPtr->minNonSatValue;
																		
			}	// end "else binIndex > 0 && <...numberBins-1"
																		
		}	// end "else binType != kDataValueIsBinIndex"
		
	return (dataValue);
		
}	// end "GetDataValueForStatBinIndex" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetEstimatedTransformedChannelMinMaxes
//
//	Software purpose:	The purpose of this routine is to estimate the transformed
//							channel min-maxes.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/12/2006
//	Revised By:			Larry L. Biehl			Date: 02/12/2006	

void GetEstimatedTransformedChannelMinMaxes (
				UInt16								numberOutputChannels, 
				UInt16*								channelListPtr, 
				HDoublePtr							channelMinPtr, 
				HDoublePtr							channelMaxPtr,
				double*								overallMinPtr,
				double*								overallMaxPtr)

{
	double								value;
	
	HDoublePtr							eigenValuePtr,
											offsetVectorPtr;

	UInt32								index;
	

	eigenValuePtr = (HDoublePtr)GetHandlePointer (
														gTransformationMatrix.eigenValueHandle);
													
	offsetVectorPtr = gTransformationMatrix.offsetVectorPtr;
													
			// Initialize the channel min and max vectors.
			
	for (index=0; index<numberOutputChannels; index++)
		{
		channelMinPtr[index] = DBL_MAX;
		channelMaxPtr[index] = -DBL_MAX;
		
		}	// end "for (index=0; index<numberOutputChannels; index++)"
			
	*overallMinPtr = DBL_MAX;
	*overallMaxPtr = -DBL_MAX;
				
	for (index=0; index<numberOutputChannels; index++)
		{
		value = 3 * sqrt (fabs (eigenValuePtr[channelListPtr[index]]));
	
		channelMinPtr[index] = -value + offsetVectorPtr[index];
		channelMaxPtr[index] = value + offsetVectorPtr[index];
		
		}	// end "for (index=0; index<numberOutputChannels; index++)"
			
	for (index=0; index<numberOutputChannels; index++)
		{						
		*overallMinPtr = MIN (*overallMinPtr, channelMinPtr[index]);
		*overallMaxPtr = MAX (*overallMaxPtr, channelMaxPtr[index]);
		
		}	// end "for (index=0; index<numberOutputChannels; index++)"
		
}	// end "GetEstimatedTransformedChannelMinMaxes" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetSumOfClassFieldColumns
//
//	Software purpose:	The purpose of this routine is to compute the sum of
//							the columns for class/field grouping.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/25/1990
//	Revised By:			Larry L. Biehl			Date: 01/29/2006	

void GetSumOfClassFieldColumns (
				FileInfoPtr							fileInfoPtr, 
				HistogramSummaryPtr				histogramRangePtr, 
				HUInt32Ptr							histogramDataPtr, 
				UInt32								numberColumns)

{
	HUInt32Ptr							histogramStartPtr,
											histogramSumPtr,
											tempHistogramDataPtr;
											
	double								maxValue,
											minValue;
											
	UInt32								binIndex,
											columnIndex,
											columnOffsetCount,
											maxIndex,
											minIndex;
	
																							
			// Check input variables.															
			
	if (fileInfoPtr == NULL)
																									return;
	if (histogramDataPtr == NULL)
																									return;
	if (histogramRangePtr == NULL)
																									return;
																							
	columnOffsetCount = gStatHistogramSpecsPtr->initialNumberHistogramDataBins;
															
	histogramStartPtr = &histogramDataPtr [columnOffsetCount];
	
	for (columnIndex=1; columnIndex<numberColumns; columnIndex++)
		{
		minValue = histogramRangePtr[columnIndex].minValue;
		maxValue = histogramRangePtr[columnIndex].maxValue;
		
				// Update minimum and maximum of sum column.								
				
		histogramRangePtr[0].minValue = MIN (histogramRangePtr[0].minValue, minValue);
		histogramRangePtr[0].maxValue = MAX (histogramRangePtr[0].maxValue, maxValue);
		
		minIndex = GetBinIndexForStatDataValue (minValue,
																&histogramRangePtr[columnIndex]);
		maxIndex = GetBinIndexForStatDataValue (maxValue,
																&histogramRangePtr[columnIndex]);
										
		histogramSumPtr = &histogramDataPtr[minIndex];
		tempHistogramDataPtr = &histogramStartPtr[minIndex];
		
		for (binIndex=minIndex; binIndex<=maxIndex; binIndex++)
			{
			*histogramSumPtr += *tempHistogramDataPtr;
			tempHistogramDataPtr++;
			histogramSumPtr++;
			
			}	// end "for (binIndex=minIndex; binIndex<=maxIndex; binIndex++)" 
			
		histogramStartPtr += columnOffsetCount;
					
		}	// end "for (columnIndex=0; columnIndex<..." 
			
}	// end "GetSumOfClassFieldColumns" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetTransformedChannelMinMaxes
//
//	Software purpose:	The purpose of this routine is to estimate the transformed
//							channel min-maxes.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/12/2006
//	Revised By:			Larry L. Biehl			Date: 02/19/2014	

void GetTransformedChannelMinMaxes (
				UInt16								numberOutputChannels, 
				UInt16*								channelListPtr,
				FileIOInstructionsPtr			fileIOInstructionsPtr, 
				SInt16								outputGroupCode,
				SInt16								statsWindowMode,
				HDoublePtr							channelMinPtr, 
				HDoublePtr							channelMaxPtr,
				double*								overallMinPtr,
				double*								overallMaxPtr)

{
	HistogramSummary*					histogramRangePtr;
	
	UInt32								index,
											indexStart;
	
	SInt16								returnCode,
											savedHistogramOutputCode;
	
	
	savedHistogramOutputCode = gStatHistogramSpecsPtr->histogramOutputCode;
	gStatHistogramSpecsPtr->histogramOutputCode = 0;
		
			// Initialize the histogram range buffer.		
			
	histogramRangePtr = gStatHistogramSpecsPtr->histogramRangePtr;								
	
	for (index=0; index<numberOutputChannels; index++)
		{	
		histogramRangePtr[index].maxValue = -DBL_MAX;
		histogramRangePtr[index].minValue = DBL_MAX;
		
		}	// end "for (index=0; index<numberOutputChannels; index++)"
					
			// Intialize the nextTime variables to indicate when the next check	
			// should occur for a command-. and status information.					
			
	gNextTime = TickCount ();
	gNextStatusTime = TickCount ();
				
	switch (statsWindowMode)
		{
		case kClassListMode:
			returnCode = HistogramProjectStats (fileIOInstructionsPtr,
															outputGroupCode);
			break;
			
		case kFieldListMode:
			indexStart = 0;
			returnCode = HistogramClassStats (fileIOInstructionsPtr,
															gProjectInfoPtr->currentClass, 
															outputGroupCode,
															&indexStart,
															0);
			break;
			
		case kCoordinateListMode:
			returnCode = HistogramFieldStats (fileIOInstructionsPtr,
															gProjectInfoPtr->currentField,
															outputGroupCode,
															0,
															0);
			break;
			
		}	// end "switch (statsWindowMode)"
		
			
	*overallMinPtr = DBL_MAX;
	*overallMaxPtr = -DBL_MAX;
				
	for (index=0; index<numberOutputChannels; index++)
		{
		channelMinPtr[index] = histogramRangePtr[index].minValue;
		channelMaxPtr[index] = histogramRangePtr[index].maxValue;
		
		}	// end "for (index=0; index<numberOutputChannels; index++)"
			
	for (index=0; index<numberOutputChannels; index++)
		{						
		*overallMinPtr = MIN (*overallMinPtr, channelMinPtr[index]);
		*overallMaxPtr = MAX (*overallMaxPtr, channelMaxPtr[index]);
		
		}	// end "for (index=0; index<numberOutputChannels; index++)"

	gStatHistogramSpecsPtr->histogramOutputCode = savedHistogramOutputCode;
		
}	// end "GetTransformedChannelMinMaxes" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 HistogramClassStats
//
//	Software purpose:	The purpose of this routine is to determine, list and/or
//							plot the histograms for the class training fields.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			HistogramStatsControl
//							HistogramProjectStats
//
//	Coded By:			Larry L. Biehl			Date: 04/02/1990
//	Revised By:			Larry L. Biehl			Date: 02/28/2018

SInt16 HistogramClassStats (
				FileIOInstructionsPtr			fileIOInstructionsPtr, 
				SInt16								classNumber, 
				SInt16								outputGroupCode, 
				UInt32*								indexStartPtr, 
				UInt16								channelStart)

{
	FileInfoPtr							fileInfoPtr;
	HPClassNamesPtr					classNamesPtr;
	HPFieldIdentifiersPtr			fieldIdentPtr;
	SInt16*								fieldPtr;
	
	UInt32								index,
											numberChannelSets,
											numberOfTrainFields;
	
	SInt16								classStorage,
											fieldCount,
											fieldNumber,
											indexStart;

	UInt16								channelSet;
	
	
			// Initialize local variables.	
	
	fileInfoPtr = fileIOInstructionsPtr->fileInfoPtr;									
	classNamesPtr = gProjectInfoPtr->classNamesPtr;
	indexStart = (SInt16)*indexStartPtr;

	fieldPtr = (SInt16*)GetHandlePointer (gStatHistogramSpecsPtr->fieldHandle);
	
			// Get the class storage number.													
						
	classStorage = gProjectInfoPtr->storageClass[classNumber];
	
	numberOfTrainFields = classNamesPtr[classStorage].numberOfTrainFields;
	
			// Save the class number for this class in case it is needed later as
			// "the last class used".
			
	gStatHistogramSpecsPtr->lastClassNumberUsed = classNumber;
	
			// Continue only if the number of fields in the class is one or more	
			// and if the class statistics are not up-to-date							
	
	if (numberOfTrainFields > 0)
		{
				// Set up status dialog. Class name, and number of						
				// training fields for the class.											
				
		LoadDItemString (gStatusDialogPtr, 
								IDC_Status6, 
								(Str255*)&classNamesPtr[classStorage].name);
		LoadDItemValue (gStatusDialogPtr, 
								IDC_Status10, 
								(SInt32)numberOfTrainFields);
				
		if (gStatHistogramSpecsPtr->histogramOutputCode != 0 &&
					gProjectInfoPtr->histogramFieldFlag &&
							outputGroupCode == kClassFieldGrouping)
			{
			fieldCount = 1;
			if (!ListClassFieldsUsed (classNumber+1,
												NULL,
												kTrainingType, 
												NULL, 
												&gOutputForce1Code,
												IDS_Project3,
												&fieldCount,
												FALSE))
																								return (0);
											
			}	// end "if (gProjectInfoPtr->histogramFieldFlag && ...)" 
					
				// Set up to data in columnar list if requested.						
			
		numberChannelSets = 1;
		if (gStatHistogramSpecsPtr->histogramOutputCode == kListData &&
					gProjectInfoPtr->histogramFieldFlag && 
							outputGroupCode == kClassFieldGrouping)
			numberChannelSets = gStatHistogramSpecsPtr->numberFeatures;
		
		for (channelSet=0; channelSet<numberChannelSets; channelSet++)
			{
						// Update message in histogram status dialog.					
						
			if (gStatHistogramSpecsPtr->histogramOutputCode == kListData &&
						gProjectInfoPtr->histogramFieldFlag && 
								outputGroupCode == kClassFieldGrouping)
				{
				MGetString (gTextString, kProjectStrID, 7);
				index = gTextString[0] + 1;
	
				sprintf ((char*)&gTextString[index], 
								"%3d of %3d", 
								channelSet+1, 
								(unsigned int)numberChannelSets);
				gTextString[0] += 10; 
						
				LoadDItemString (gStatusDialogPtr, 
										IDC_Status11, 
										(Str255*)gTextString);
				
				}	// end "if (...->histogramOutputCode == kListData && ..." 
				
					// Initialize the buffers if this is a class histogram and		
					// channel grouping.
					
			if (gStatHistogramSpecsPtr->histogramOutputCode == kListData)
				{
				if (outputGroupCode == kChannelGrouping)
					{
							// Data are to be grouped by channels.
							
					if (gProjectInfoPtr->histogramClassFlag)
						{
						InitializeStatHistogramBuffers (
															fileInfoPtr,
															outputGroupCode,
															gStatHistogramSpecsPtr->numberColumns,
															gStatHistogramSpecsPtr->featurePtr,
															channelSet,
															gStatHistogramSpecsPtr->channelMinPtr, 
															gStatHistogramSpecsPtr->channelMaxPtr);
					
						indexStart = 0;
				
						channelStart = channelSet;
						
						}	//end "if (gProjectInfoPtr->histogramFieldFlag)"
					
					}	// end "if (outputGroupCode == kChannelGrouping)"
					
				else	// outputGroupCode == kClassFieldGrouping
					{
							// Data are to be grouped by classes or fields.
							
					if (gProjectInfoPtr->histogramFieldFlag)
						{
								// Data are to be grouped by fields.
			
						InitializeStatHistogramBuffers (
															fileInfoPtr,
															outputGroupCode,
															gStatHistogramSpecsPtr->numberColumns,
															gStatHistogramSpecsPtr->featurePtr,
															channelSet, 
															gStatHistogramSpecsPtr->channelMinPtr, 
															gStatHistogramSpecsPtr->channelMaxPtr);
					
								// Set the start index into the buffers to allow for the 
								// first column to be a total column.	
					
						indexStart = 0;
						if (numberOfTrainFields > 1)
							indexStart = 1;	
				
						channelStart = channelSet;
						
						}	// end "if (gProjectInfoPtr->histogramFieldFlag)"
					
					}	// end "else outputGroupCode == kClassFieldGrouping"
				
				}	// end "if (...->histogramOutputCode == kListData)"
				
			else if (gStatHistogramSpecsPtr->histogramOutputCode == kPlotData)
				{
				if (gProjectInfoPtr->histogramClassFlag)
					InitializeStatHistogramBuffers (fileInfoPtr,
																kChannelGrouping,
																gStatHistogramSpecsPtr->numberColumns,
																gStatHistogramSpecsPtr->featurePtr,
																channelSet, 
																gStatHistogramSpecsPtr->channelMinPtr, 
																gStatHistogramSpecsPtr->channelMaxPtr);
				
				}	// end "else if ...->histogramOutputCode == kPlotData"	
				
			fieldNumber = classNamesPtr[classStorage].firstFieldNumber;
			
			fieldCount = 1;
			while (fieldNumber != -1)
				{
				fieldIdentPtr = &gProjectInfoPtr->fieldIdentPtr[fieldNumber];
	
						// Make certain that field is training field.					
						
				if (fieldIdentPtr->fieldType == kTrainingType)
					{
							// Put message in histogram status dialog if these are	
							// field histograms.													
							
					if (!gProjectInfoPtr->histogramClassFlag && 
																outputGroupCode == kChannelGrouping)
						LoadDItemStringNumber (kProjectStrID,
														IDS_Project80,	// Computing histograms
														gStatusDialogPtr,
														IDC_Status11,
														(Str255*)gTextString);
					
							// Set up field count in status dialog.						
					
					LoadDItemValue (gStatusDialogPtr, 
											IDC_Status8, 
											(SInt32)fieldCount);
						
							// Load the field histogram information.						
						
					if (HistogramFieldStats (fileIOInstructionsPtr,
														fieldNumber, 
														outputGroupCode,
														indexStart,
														channelStart) == 0)
																								return (0);
					
					fieldCount++;
					
							// Ready the start index into the buffers for the next column or
							// vector if field grouping is being used.											
							
					if (gProjectInfoPtr->histogramFieldFlag && 
															outputGroupCode == kClassFieldGrouping)
						indexStart++;
									
					}	// end "if (fieldIdentPtr->field..." 
					
				fieldNumber = fieldIdentPtr->nextField;
				
				}	// end "while (fieldNumber != -1)" 
				
			if (gStatHistogramSpecsPtr->histogramOutputCode == kListData)
				{
						// List the histogram values if this is a class histogram and	
						// channel grouping is requested.										
				
				if (gProjectInfoPtr->histogramClassFlag && 
														outputGroupCode == kChannelGrouping)
					{
							// List class name and number of training fields and number	
							// of training pixels.													
				
					if (!ListClassInformation (classStorage,
														gStatHistogramSpecsPtr->totalNumberValues))
																								return (0);
										
					if (gStatHistogramSpecsPtr->totalNumberValues > 0)
						{																																
						if (!ListStatHistogramValues (
												fileInfoPtr,
												(SInt16*)gStatHistogramSpecsPtr->featurePtr,
												gStatHistogramSpecsPtr->featureTransformationFlag,
												gStatHistogramSpecsPtr->histogramRangePtr,
												gStatHistogramSpecsPtr->histogramDataPtr, 
												gStatHistogramSpecsPtr->numberFeatures, 
												gStatHistogramSpecsPtr->totalNumberValues, 
												gStatHistogramSpecsPtr->outputFormatCode))
																								return (0);
																							
						}	// end "if (...->totalNumberValues > 0)" 
												
					}	// end "if (gProjectInfoPtr->histogramClassFlag && ...)" 
			
						// List the histogram values if this is a field histogram 		
						// and field grouping is requested.										
				
				if (gProjectInfoPtr->histogramFieldFlag && 
															outputGroupCode == kClassFieldGrouping)
					{						
					if (gStatHistogramSpecsPtr->totalNumberValues > 0)
						{
								// List channel information.										
								
						if (!ListChannelInformation (
											fileInfoPtr, 
											gStatHistogramSpecsPtr->featurePtr[channelStart],
											gStatHistogramSpecsPtr->featureTransformationFlag))
																								return (0);
																								
								// "fieldCount" which is used as the number of columns
								// is currently set as one more that the number of fields
								// in the list to allow for the sum column. Decrement this
								// count if the number of fields is only 1. No sum column
								// will be used.
								
						if (fieldCount == 2)
							fieldCount--;
																								
						GetSumOfClassFieldColumns (
														fileInfoPtr,
														gStatHistogramSpecsPtr->histogramRangePtr,
														gStatHistogramSpecsPtr->histogramDataPtr, 
														fieldCount);
						
						if (!ListStatHistogramValues (
												fileInfoPtr,
												fieldPtr,
												gStatHistogramSpecsPtr->featureTransformationFlag, 
												gStatHistogramSpecsPtr->histogramRangePtr,
												gStatHistogramSpecsPtr->histogramDataPtr, 
												fieldCount, 
												gStatHistogramSpecsPtr->totalNumberValues, 
												gStatHistogramSpecsPtr->outputFormatCode))
																								return (0);
																							
						}	// end "if (...->totalNumberValues > 0)" 
												
					}	// end "if (gProjectInfoPtr->histogramFieldFlag && ...)"
					
				}	// end "if (...->histogramOutputCode == kListData)"
		
					// Load the histogram values into the graph structure if the data is to
					// be graphed.																			
			
			if (gStatHistogramSpecsPtr->histogramOutputCode == kPlotData && 
																gProjectInfoPtr->histogramClassFlag)
				{
				if (!LoadGraphData (fileInfoPtr, indexStart)) 
																								return (0);
				
				}	// end "if (...->histogramOutputCode == kPlotData && ..."
			
			}	// end "for (channelSet=0; ...)" 
			
		}	// end "if (classNamesPtr[classStorage].number ..."
		
			// Save the current "indexStart" setting. It may be needed for histogram
			// plots.
	
	if (gStatHistogramSpecsPtr->histogramOutputCode == kPlotData)	
		*indexStartPtr = indexStart;
	
			// Indicate that routine completed normally.								
			
	return (1);
		
}	// end "HistogramClassStats" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 HistogramFieldStats
//
//	Software purpose:	The purpose of this routine is to obtain the histogram
//							information for the given field.
//							If outputGroupCode == 1, then the histogram statistics are listed
//								for each individual field.
//							If outputGroupCode == 2, then the histogram statistics are just
//								being computed so that they can be added to the other fields 
//								in the class is listed later as class histogram statistics.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			histogramStatsControl
//							histogramClassStats
//
//	Coded By:			Larry L. Biehl			Date: 04/02/1990
//	Revised By:			Larry L. Biehl			Date: 02/19/2014	

SInt16 HistogramFieldStats (
				FileIOInstructionsPtr			fileIOInstructionsPtr, 
				SInt16								fieldNumber, 
				SInt16								outputGroupCode, 
				UInt32								indexStart, 
				UInt16								channelStart)

{
	double								maxAllowedValue,
											minAllowedValue,
											value;
	
	Point									point;
	
	FileInfoPtr							fileInfoPtr;
	HistogramSummary*					histogramRangePtr;
											
	HDoublePtr							bufferPtr,
											buffer8Ptr;
	
	HUInt32Ptr							histogramDataPtr;
	SInt16*								featurePtr;
	UInt16*								localChannelsPtr;
	
	RgnHandle							rgnHandle;
	
	SInt32								column,
											countSkip,
											line,
											lineEnd,
											lineInterval,
											lineStart,
											numberSamples,
											totalNumberPixels;
	
	UInt32								binIndex,
											columnEnd,
											columnInterval,
											columnStart,
											histogramIndex,
											index,
											initialHistogramIndex,
											lineCount,
											numberBins,
											storageColumnEnd,
											storageColumnStart;
											
	SInt16								errCode,
											featureNumber,
											localNumberChannels,
											numberFeatures,
											returnCode;
											
	Boolean								eigenvectorTypeFlag,
											histogramIndexSkipFlag,
											polygonFieldFlag,
											transformDataFlag,
											updateNumberLinesFlag;
	
	
			// Make certain that input values make sense									
	
	returnCode = 1;	
	if (fieldNumber < 0 || fieldNumber >= gProjectInfoPtr->numberStorageFields)
		returnCode = 0;
		
	else if (fileIOInstructionsPtr->fileInfoPtr == NULL)
		returnCode = 0;
		
	else if (gProjectInfoPtr->fieldIdentPtr[fieldNumber].fieldType != kTrainingType)	
		returnCode = 0;
	
	if (returnCode == 0)	
																								return (0);
	
			// Initialize local variables.
	
	fileInfoPtr = fileIOInstructionsPtr->fileInfoPtr;		
	transformDataFlag = gStatHistogramSpecsPtr->featureTransformationFlag;	
	numberFeatures = gStatHistogramSpecsPtr->numberFeatures;
	eigenvectorTypeFlag = (gTransformationMatrix.createdByCode < 16);												
			
	localChannelsPtr = (UInt16*)GetHandlePointer (
															gStatHistogramSpecsPtr->channelsHandle);
			
	featurePtr = (SInt16*)GetHandlePointer (gStatHistogramSpecsPtr->featureHandle);
	
	if (!transformDataFlag)													
		localChannelsPtr = &localChannelsPtr[channelStart];
	
	columnInterval = gStatHistogramSpecsPtr->columnInterval;
	lineInterval = gStatHistogramSpecsPtr->lineInterval;
	numberBins = gStatHistogramSpecsPtr->initialNumberHistogramDataBins;
	histogramDataPtr = gStatHistogramSpecsPtr->histogramDataPtr;
	histogramRangePtr = gStatHistogramSpecsPtr->histogramRangePtr;
	totalNumberPixels = 0;
	
			// Save the field number for this field in case it is needed later as
			// "the last field used".
			
	gStatHistogramSpecsPtr->lastFieldNumberUsed = fieldNumber;
	
			// Check if the is a cluster field.  We can not histogram cluster		
			// fields since we do not know which pixels were used to create the	
			// cluster statistics.																
																						
	if (gProjectInfoPtr->fieldIdentPtr[fieldNumber].pointType == kClusterType)
		{
			// List the class and field name if this is a field summary and		
			// channel grouping.																	
	
		if (!gProjectInfoPtr->histogramClassFlag && 
															outputGroupCode == kChannelGrouping)
			{
					// List class name and field name.										
					
			if (!ListFieldInformation (
					fieldNumber, totalNumberPixels, 0, lineInterval, columnInterval, 2))
																								return (0);
																						
			}	// end "if (!...histogramClassFlag && outputGroupCode == ..." 
			
																								return (1);
		}	// end "if ...pointType == kClusterType)"
	
	if (gStatHistogramSpecsPtr->histogramOutputCode == kListData)
		{
		if (outputGroupCode == kChannelGrouping)
			{
			if (!gProjectInfoPtr->histogramClassFlag)
				{
						// Set parameters for listing field data in a channel grouping.
								
				featureNumber = 0;		
				channelStart = 0;
				localNumberChannels = gStatHistogramSpecsPtr->numberChannels;
													
				storageColumnStart = 0;
				storageColumnEnd = gStatHistogramSpecsPtr->numberFeatures;	
				
				InitializeStatHistogramBuffers (fileInfoPtr,
															kChannelGrouping,
															gStatHistogramSpecsPtr->numberColumns,
															gStatHistogramSpecsPtr->featurePtr,
															channelStart,
															gStatHistogramSpecsPtr->channelMinPtr, 
															gStatHistogramSpecsPtr->channelMaxPtr);
				
				histogramIndex = 0;
				
				}	// end "if (outputGroupCode == kChannelGrouping)"
				
			else	// gProjectInfoPtr->histogramClassFlag
				{
						// Set parameters for listing class data in a channel grouping.
						// Note that the input value is used for channelStart.
						
				featureNumber = channelStart;	
				localNumberChannels = gStatHistogramSpecsPtr->numberChannels;	
												
				storageColumnStart = indexStart;
				storageColumnEnd = gStatHistogramSpecsPtr->numberFeatures;
				
				histogramIndex = 0;
				
				}	// end "else gProjectInfoPtr->histogramClassFlag"
			
			}	// end "if (outputGroupCode == kChannelGrouping)"
				
		else	// outputGroupCode == kClassFieldGrouping
			{
					// Set parameters for listing class or field data in a 
					// field/class grouping.
					// Note that the input value is used for channelStart.	
			
			featureNumber = channelStart;	
			
			localNumberChannels = 1;
			if (transformDataFlag)	
				localNumberChannels = gStatHistogramSpecsPtr->numberChannels;
			
			storageColumnStart = indexStart;
			storageColumnEnd = indexStart + 1;
			
			histogramIndex = indexStart * numberBins;
			
			}	// end "if (outputGroupCode == kClassFieldGrouping)" 
		
		}	// end "if (...->histogramOutputCode == kListData)"
	
	else if (gStatHistogramSpecsPtr->histogramOutputCode == kPlotData)
		{	
		featureNumber = 0;		
		channelStart = 0;
		localNumberChannels = gStatHistogramSpecsPtr->numberChannels;
											
		storageColumnStart = 0;
		storageColumnEnd = gStatHistogramSpecsPtr->numberFeatures;
		
		if (!gProjectInfoPtr->histogramClassFlag)
			{
					// Set parameters for plotting field data.	
			
			InitializeStatHistogramBuffers (fileInfoPtr,
														kChannelGrouping,
														gStatHistogramSpecsPtr->numberColumns,
														gStatHistogramSpecsPtr->featurePtr,
														channelStart,
														gStatHistogramSpecsPtr->channelMinPtr, 
														gStatHistogramSpecsPtr->channelMaxPtr);
			
			}	// end "if (!gProjectInfoPtr->histogramClassFlag)"
		
		histogramIndex = 0;
		
		}	// end "if (...->histogramOutputCode == kPlotData)"
				
	else	// gStatHistogramSpecsPtr->histogramOutputCode == 0
		{
		localNumberChannels = gStatHistogramSpecsPtr->numberChannels;
		storageColumnStart = 0;
		storageColumnEnd = gStatHistogramSpecsPtr->numberFeatures;
		
		}	// end "else ...->histogramOutputCode == 0
		
			// Determine if field is described by a polygon or a rectangle.  If	
			// the field is described by a polygon then create a region for the	
			// field.																				
	
	GetFieldBoundary (gProjectInfoPtr, &gAreaDescription, fieldNumber);
			
	polygonFieldFlag = gAreaDescription.polygonFieldFlag;
	rgnHandle = gAreaDescription.rgnHandle;
	lineStart = gAreaDescription.lineStart;
	lineEnd = gAreaDescription.lineEnd;
	columnStart = gAreaDescription.columnStart;
	columnEnd = gAreaDescription.columnEnd;
		
	if (polygonFieldFlag && rgnHandle == NULL)
																								return (0);
	
			// Get the buffer sample interval.											
	
	countSkip = localNumberChannels;
	
	lineCount = 0;
			
			// Load some of the File IO Instructions structure that pertain
			// to the specific area being used.
			
	errCode = SetUpFileIOInstructions (fileIOInstructionsPtr,
													&gAreaDescription, 
													localNumberChannels, 
													localChannelsPtr,
													kDetermineSpecialBILFlag);
	
			// Allow for signed data if needed.
			
	maxAllowedValue = fileInfoPtr->maxUsableDataValue;
	minAllowedValue = fileInfoPtr->minUsableDataValue;
	if (transformDataFlag)
		{
		maxAllowedValue = DBL_MAX;
		minAllowedValue = -DBL_MAX;
		
		}	// end "if (transformDataFlag)"
		
			// Save the initial histogram index value for possible later use when the
			// histogram for more than one channel is being obtained.
				
	initialHistogramIndex = histogramIndex;	
		
			// Set flag indicating whether the histogram	index should be advanced.
			
	histogramIndexSkipFlag = FALSE;
	if (outputGroupCode == kChannelGrouping || 
				gStatHistogramSpecsPtr->histogramOutputCode == kPlotData || 
						gStatHistogramSpecsPtr->histogramOutputCode == 0)
		histogramIndexSkipFlag = TRUE;	
		
	updateNumberLinesFlag = TRUE;
	
			// Loop through the lines for the field.										
	
	for (line=lineStart; line<=lineEnd; line+=lineInterval)
		{
				// Load the line count into the status dialog.							
		
		lineCount++;	
		
		if (TickCount () >= gNextStatusTime)
			{
			if (updateNumberLinesFlag)
				{
						// Load the total number of lines into the status dialog.				
							
				LoadDItemValue (gStatusDialogPtr, 
										IDC_Status20, 
										(SInt32)(lineEnd-lineStart+lineInterval)/lineInterval);
				updateNumberLinesFlag = FALSE;
				
				}	// end "if (updateNumberLinesFlag)"
				
			LoadDItemValue (gStatusDialogPtr, IDC_Status18, (SInt32)lineCount);
			gNextStatusTime = TickCount () + gNextStatusTimeOffset;
			
			}	// end "if (TickCount () >= gNextStatusTime)" 
		
		point.v = (SInt16)line;
		point.h = (SInt16)columnStart;
			
				// Get data for the requested channels for the line of image 		
				// data.  Return if there is a file IO error.							
			 
		errCode = GetLineOfData (fileIOInstructionsPtr,
											line, 
											columnStart,
											columnEnd,
											columnInterval,
											gInputBufferPtr,
											gOutputBufferPtr);
			
		if (errCode < noErr)
			{
			returnCode = 0;
			break;
			
			}	// end "if (errCode < noErr)"
				
		if (errCode != kSkipLine)
			{
		   bufferPtr = (HDoublePtr)gOutputBufferPtr;
			   
			numberSamples = fileIOInstructionsPtr->numberOutputBufferSamples;
				
			for (column=0; column<numberSamples; column++)
				{
				if (!polygonFieldFlag || PtInRgn (point, rgnHandle))
					{
					featureNumber = channelStart;
				
					if (histogramIndexSkipFlag)
						histogramIndex = initialHistogramIndex;
						
					buffer8Ptr = bufferPtr;	
					
					for (index=storageColumnStart; index<storageColumnEnd; index++)
						{
						if (transformDataFlag)
							{	
							value = TransformDataVector (
															buffer8Ptr,
															gTransformationMatrix.eigenVectorPtr,
															gTransformationMatrix.offsetVectorPtr,
															localNumberChannels,
															eigenvectorTypeFlag,
															featureNumber,
															1);
														
							//value = (SInt32)(doubleValue + SIGN2 (.5, doubleValue));
														
							}	// end "if (transformDataFlag)"
														
						else	// !transformDataFlag								
							value = *buffer8Ptr;
										
								// Make certain the value is within the proper range.		
	
						if (value < minAllowedValue)
							{
							histogramRangePtr[index].badValues++;
							value = minAllowedValue;
							
							}	// end "if (value < minAllowedValue)"
						
						else if (value > maxAllowedValue)	
							{
							histogramRangePtr[index].badValues++;
							value = maxAllowedValue;
							
							}	// end "if (value < minAllowedValue)"
							
								// Update the minimum and maximum data values for 		
								// the channel.													
						
						histogramRangePtr[index].minValue =
									MIN (value, histogramRangePtr[index].minValue);
						
						histogramRangePtr[index].maxValue =
									MAX (value, histogramRangePtr[index].maxValue);
						
						if (gStatHistogramSpecsPtr->histogramOutputCode != 0)
							{			
									// Update data value count.	
									
							binIndex = GetBinIndexForStatDataValue (
																			value,
																			&histogramRangePtr[index]);						
						
							histogramDataPtr[histogramIndex + binIndex]++;
							
							}	// end "if (...->histogramOutputCode != 0)"
							
						if (histogramIndexSkipFlag)
							{
							histogramIndex += numberBins;
							featureNumber++;
							
							}	// end "if (histogramIndexSkipFlag)"
							
						if (!transformDataFlag)	
							buffer8Ptr++;
							
						}	// end "for (index=storageColumnStart; index<..." 
						
					totalNumberPixels++;
					
					}	// end "if (!polygonField || PtInRgn (point, rgnHandle))" 
					
				point.h += (SInt16)columnInterval;
				bufferPtr += countSkip;
						
				} 		// end "for (column=0; column<..." 
				
			}	// end "if (errCode != kSkipLine)"
			
				// Exit routine if user has "command period" down					
				
		if (TickCount () >= gNextTime)
			{
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
				{
				returnCode = 0;
				lineEnd = lineCount;
				break;
				
				}	// end "if (!CheckSomeEvents (osMask..." 
					
			}	// end "if (TickCount () >= nextTime)" 
			
		if (fileIOInstructionsPtr->maskBufferPtr != NULL)
			fileIOInstructionsPtr->maskBufferPtr += 
												fileIOInstructionsPtr->numberMaskColumnsPerLine;
	      
		}	// end "for (line=lineStart; line<=lineEnd; line+=...)" 
		
	// LoadDItemValue (gStatusDialogPtr, IDC_Status18, lineEnd);
		
			// Close up any File IO Instructions structure that pertain to the 
			// specific area used for the biplot.
			
	CloseUpFileIOInstructions (fileIOInstructionsPtr, &gAreaDescription);
	
	CloseUpAreaDescription (&gAreaDescription);
		
	if (returnCode == 0)
																								return (0);
																								
	if (gStatHistogramSpecsPtr->histogramOutputCode == 0)
																								return (1);
		
				// Update the total number of pixels in the histogram;				
				
	gStatHistogramSpecsPtr->totalNumberValues += totalNumberPixels;
	
	if ((gProjectInfoPtr->histogramFieldFlag && outputGroupCode == 1) || 
					(gProjectInfoPtr->statsWindowMode == kCoordinateListMode &&
							gStatHistogramSpecsPtr->histogramOutputCode == kPlotData))							
		{		
				// List class name and field name.				
				
		if (!ListFieldInformation (
				fieldNumber, totalNumberPixels, 0, lineInterval, columnInterval, 2))
																								return (0);
																								
																					
		}	// end "if ((gProjectInfoPtr->histogramFieldFlag && ..."
		
	if (gStatHistogramSpecsPtr->totalNumberValues > 0)
		{		
				// List the histogram values if this is a field summary and channel	
				// grouping.																			
		
		if (gStatHistogramSpecsPtr->histogramOutputCode == kListData && 
					gProjectInfoPtr->histogramFieldFlag && 
							outputGroupCode == kChannelGrouping)
			{
			if (!ListStatHistogramValues (
										fileInfoPtr, 
										featurePtr,
										gStatHistogramSpecsPtr->featureTransformationFlag,
										histogramRangePtr,
										histogramDataPtr, 
										gStatHistogramSpecsPtr->numberFeatures, 
										gStatHistogramSpecsPtr->totalNumberValues,
										gStatHistogramSpecsPtr->outputFormatCode))
																							return (0);
										
			}	// end "if (...->histogramOutputCode == kListData && ...)" 
		
				// Load the histogram values into the graph structure if the data is to
				// be graphed.																			
		
		if (gStatHistogramSpecsPtr->histogramOutputCode == kPlotData && 
					gProjectInfoPtr->histogramFieldFlag)
			{
			if (!LoadGraphData (fileInfoPtr, (SInt16)indexStart))
																								return (0);
																							
			}	// end "if (...->histogramOutputCode == kPlotData && ..."
																							
		}	// end "if (...->totalNumberValues > 0)" 
		
	return (1);
	
}	// end "HistogramFieldStats" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 HistogramProjectStats
//
//	Software purpose:	The purpose of this routine is to determine, list and/or
//							plot the histograms for the project training fields.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			HistogramStatsControl
//
//	Coded By:			Larry L. Biehl			Date: 04/02/1990
//	Revised By:			Larry L. Biehl			Date: 10/26/1999	

SInt16 HistogramProjectStats (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				SInt16								outputGroupCode)

{
	FileInfoPtr							fileInfoPtr;
	SInt16*								classPtr;
	
	SInt32								channelStart,
											channelSet,
											classIndex,
											classStorage,
											numberChannelSets,
											numberClasses;
	
	UInt32								index,
											indexStart;
	
	
			// Initialize local variables.													
	
	fileInfoPtr = fileIOInstructionsPtr->fileInfoPtr;		
	numberClasses = gProjectInfoPtr->numberStatisticsClasses;
	indexStart = 0;
	channelStart = 0;

	classPtr = (SInt16*)GetHandlePointer (gStatHistogramSpecsPtr->classHandle);
	
			// Continue only if number of classes is one or more.						
	
	if (numberClasses > 0)
		{
				// Set up data in columnar list if requested.							
			
		numberChannelSets = 1;
		if (outputGroupCode == kClassFieldGrouping && 
				gProjectInfoPtr->histogramClassFlag)
			{
			if (!ListClassesUsed (NULL,
											fileInfoPtr, 
											classPtr, 
											kListNoSymbols, 
											kListNoWeights, 
											gStatHistogramSpecsPtr->numberClasses, 
											NULL, 
											&gOutputForce1Code,
											FALSE))
																								return (0);
			
			if (gStatHistogramSpecsPtr->histogramOutputCode == kListData)									
				numberChannelSets = gStatHistogramSpecsPtr->numberFeatures;
			
			}	// end "if (outputGroupCode == kClassFieldGrouping && ..." 
			
				// Set up status dialog.  Load in number of classes.					
				
		LoadDItemValue (gStatusDialogPtr, IDC_Status5, numberClasses);
		
		for (channelSet=0; channelSet<numberChannelSets; channelSet++)
			{
					// Initialize the buffers if reqest is for class grouping.		
					
			if (gProjectInfoPtr->histogramClassFlag && 
														outputGroupCode == kClassFieldGrouping)
				{
				if (gStatHistogramSpecsPtr->histogramOutputCode == kListData)
					{
							// Update message in histogram status dialog.					
				
					MGetString (gTextString, kProjectStrID, 7);
					index = gTextString[0] + 1;
		
					sprintf ((char*)&gTextString[index], 
									"%3d of %3d", 
									(int)(channelSet+1),
									(int)numberChannelSets);
					gTextString[0] += 10; 
							
					LoadDItemString (gStatusDialogPtr, 
											IDC_Status11, 
											(Str255*)gTextString);
			
					InitializeStatHistogramBuffers (
														fileInfoPtr,
														kClassFieldGrouping,
														gStatHistogramSpecsPtr->numberColumns,
														gStatHistogramSpecsPtr->featurePtr,
														channelSet, 
														gStatHistogramSpecsPtr->channelMinPtr, 
														gStatHistogramSpecsPtr->channelMaxPtr);
					
						// Set the start index into the buffers to allow for the first
						// column to be a total column if group request is class grouping
						// and the histogram is being listed.													
					
					indexStart = 0;
					if (gStatHistogramSpecsPtr->numberColumns > 1)	
						indexStart = 1;
				
					}	// end "if (...->histogramOutputCode == kListData)"
				
				channelStart = channelSet;
					
				}	// end "if (gProjectInfoPtr->histogramClassFlag && ..." 
			
			for (classIndex=0; classIndex<numberClasses; classIndex++)
				{
						// Put message in histogram status dialog if these are		
						// class histograms.														
						
				if (gProjectInfoPtr->histogramClassFlag && outputGroupCode == 1)
					LoadDItemStringNumber (kProjectStrID,
													IDS_Project80,	// Computing histograms
													gStatusDialogPtr,
													IDC_Status11,
													(Str255*)gTextString);
								
						// Set up status dialog.  Load in class count.					
					
				LoadDItemValue (gStatusDialogPtr, 
										IDC_Status3, 
										(SInt32)classIndex+1);
				
						// Get the class storage number.										
							
				classStorage = gProjectInfoPtr->storageClass[classIndex];
				
						// Histogram class training fields.									
							
				if (HistogramClassStats (fileIOInstructionsPtr, 
													(SInt16)classIndex, 
													outputGroupCode, 
													&indexStart, 
													(SInt16)channelStart) == 0)
																								return (0);
					
						// Ready the start index into the buffers for the next column or
						// vector if class grouping is being used.											
						
				if (gProjectInfoPtr->histogramClassFlag && 
														outputGroupCode == kClassFieldGrouping)
					indexStart++;
				
				}	// end "for (classIndex=0; ... 
	
			if (gStatHistogramSpecsPtr->histogramOutputCode == kListData &&
						gProjectInfoPtr->histogramClassFlag && 
								outputGroupCode == kClassFieldGrouping)
				{
						// List channel number.													
						
				if (!ListChannelInformation (
										fileInfoPtr,
										gStatHistogramSpecsPtr->featurePtr[channelStart],
										gStatHistogramSpecsPtr->featureTransformationFlag))
																								return (0);
																						
				GetSumOfClassFieldColumns (
									fileInfoPtr, 
									gStatHistogramSpecsPtr->histogramRangePtr,
									gStatHistogramSpecsPtr->histogramDataPtr, 
									gStatHistogramSpecsPtr->numberColumns);
				
				if (gStatHistogramSpecsPtr->totalNumberValues > 0)
					{																			
					if (!ListStatHistogramValues (
											fileInfoPtr,
											classPtr,
											gStatHistogramSpecsPtr->featureTransformationFlag, 
											gStatHistogramSpecsPtr->histogramRangePtr,
											gStatHistogramSpecsPtr->histogramDataPtr, 
											gStatHistogramSpecsPtr->numberColumns, 
											gStatHistogramSpecsPtr->totalNumberValues, 
											gStatHistogramSpecsPtr->outputFormatCode))
																								return (0);
																						
					}	// end "if (...->totalNumberValues > 0)" 
											
				}	// end "if (gProjectInfoPtr->histogramClassFlag && ...)" 
				
			}	// end "for (channelSet=0; ...)" 
			
		}	// end "if (numberClasses > 0" 
	
			// Indicate that routine completed normally.									
			
	return (1);
		
}	// end "HistogramProjectStats" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 HistogramStatsControl
//
//	Software purpose:	The purpose of this routine is to handle the
//							"Histogram Statistics Control" event depending on
//							the statistics window mode.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			StatisticsWControlEvent
//
//	Coded By:			Larry L. Biehl			Date: 04/02/1990
//	Revised By:			Larry L. Biehl			Date: 04/06/2018

SInt16 HistogramStatsControl (
				SInt16								statsWindowMode, 
				SInt16								histogramRequest)

{
	CMGraphView*						graphViewCPtr = NULL;
	FileInfoPtr							fileInfoPtr;
	FileIOInstructionsPtr			fileIOInstructionsPtr;
	SInt16*								classListPtr;
	UInt16*								channelsPtr;
	
	time_t								startTime;
	
	Handle								graphRecHandle;
	
	SInt32								error;
		
	UInt32								indexStart,
											numberClasses;
	
	SInt16								classList,
											outputGroupCode,
											returnCode,
											stringID,
											stringNumber;
											
	UInt16								xDataTypeCode;
	
	Boolean								continueFlag;
	  
	SignedByte							handleStatus;
	
	
			// If spare memory had to be used to load code resources, then exit	
			// routine.																				
			
	if (gMemoryTypeNeeded < 0)
																						return (FALSE);
																							
	if (!gCPPNewCalledFlag)
		{
		SInt32* longIntPtr = new SInt32;
		
		if (longIntPtr != NULL)
			delete longIntPtr;
			
		gCPPNewCalledFlag = TRUE;
		
		}	// end "if (!gCPPNewCalledFlag)"
																							
			// Code resources loaded okay, so set flag back for non-Code			
			// resources.																			
			
	gMemoryTypeNeeded = 0;

	returnCode = -1;
	fileIOInstructionsPtr = NULL;
	if (statsWindowMode >= kClassListMode && statsWindowMode <= kCoordinateListMode)
		{
				// Continue if handle exists to image file information.				
		
		returnCode = 0;
		
				// Check handle to image file information.  If handle to image		
				// file information doesn't exists. Find the	image file and get	
				// the information for it.														
					
		if (GetProjectImageFileInfo (kPrompt, kSetupGlobalInfoPointers))
			{
			fileInfoPtr = gImageFileInfoPtr;
		
					// Set up statistics histogram specification structure.			
					
			if (LoadStatHistogramSpecs (fileInfoPtr))
				{
				continueFlag = TRUE;
				
				if (histogramRequest == 1)
					{
					gProjectInfoPtr->histogramClassFlag = TRUE;
					gProjectInfoPtr->histogramFieldFlag = FALSE;
					
					if (gStatHistogramSpecsPtr->histogramOutputCode == kListData)
						{
						if (statsWindowMode == kFieldListMode)
							{
							gStatHistogramSpecsPtr->outputFormatCode |= 0x0100;
							gStatHistogramSpecsPtr->outputFormatCode &= 0xfdff;
							
							}	// end "if (statsWindowMode == kFieldListMode)"
							
						}	// end "if (...->histogramOutputCode == kListData)"
						
					else	// ...->histogramOutputCode == kPlotData
						{
						gStatHistogramSpecsPtr->outputFormatCode |= 0x0200;
						gStatHistogramSpecsPtr->outputFormatCode &= 0xfeff;
							
						}	// end "else ...->histogramOutputCode == kPlotData"
					
					}	// end "if (histogramRequest == 1)"
					
				else if (histogramRequest == 2)
					{
					gProjectInfoPtr->histogramClassFlag = FALSE;
					gProjectInfoPtr->histogramFieldFlag = TRUE;
					
					if (gStatHistogramSpecsPtr->histogramOutputCode == kListData)
						{
						if (statsWindowMode == kCoordinateListMode)
							{
							gStatHistogramSpecsPtr->outputFormatCode |= 0x0100;
							gStatHistogramSpecsPtr->outputFormatCode &= 0xfdff;
							
							}	// end "if (statsWindowMode == kCoordinateListMode)"
							
						}	// end "if (...->histogramOutputCode == kListData)"
						
					else	// ...->histogramOutputCode == kPlotData
						{
						gStatHistogramSpecsPtr->outputFormatCode |= 0x0200;
						gStatHistogramSpecsPtr->outputFormatCode &= 0xfeff;
							
						}	// end "else ...->histogramOutputCode == kPlotData"
					
					}	// end "if (histogramRequest == 1)"
					
				else	// histogramRequest != 1 and 2
					continueFlag = StatHistogramDialog (fileInfoPtr);
				
				if (continueFlag && fileInfoPtr->numberBytes >= 4)
					{
							// Update statistics for project if needed.	This is required for
							// 4 byte or more data to be able to know what the min and max 
							// values are to be able to bin the data.
					
					if (statsWindowMode == kClassListMode)
						{
						numberClasses = gProjectInfoPtr->numberStatisticsClasses;	
						classListPtr = NULL;
						
						}	// end "if (statsWindowMode == kClassListMode)"
					
					else	// statsWindowMode != kClassListMode
						{
						numberClasses = 1;
						classList = gProjectInfoPtr->currentClass + 1;
						classListPtr = &classList;
						
						}	// end "else statsWindowMode != kClassListMode"
						
					continueFlag = VerifyProjectStatsUpToDate (
											&numberClasses,
											classListPtr,
											numberClasses,
											kNoStatisticsUsed, 
											kSetupGlobalInfoPointers,
											NULL);
											
					}	// end "if (continueFlag && fileInfoPtr->numberBytes >= 4)"
					
						// Change cursor to watch cursor until done with process.	
							
				MSetCursor (kWait);
				
						// Initialize some global variables pertaining to output.	
				
				gOutputTextOKFlag = TRUE;
				gOutputCode = gOutputForce1Code = 1;
					
						// Get status dialog box.				
				
				if (continueFlag)
					{
					gStatusDialogPtr = GetStatusDialog (kUpdateStatsInfoID, FALSE);
				
					continueFlag = (gStatusDialogPtr != NULL);
					
					}	// end "if (continueFlag)"
					
				if (continueFlag)
					{
							// Put message in histogram status dialog.
							
					LoadDItemStringNumber (kProjectStrID,
													IDS_Project81,		// Histograms of Training Areas
													gStatusDialogPtr,
													IDC_Status11,
													(Str255*)gTextString);
					ShowStatusDialogItemSet (kStatusField);
					ShowStatusDialogItemSet (kStatusLine);
					ShowStatusDialogItemSet (kStatusCommand);
					
					switch (statsWindowMode)
						{
						case kClassListMode:
							ShowStatusDialogItemSet (kStatusClassA);
							break;
							
						case kFieldListMode:
							ShowStatusDialogItemSet (kStatusClassA);
							LoadDItemValue (gStatusDialogPtr, IDC_Status3, 1);
							LoadDItemValue (gStatusDialogPtr, IDC_Status5, 1);
							break;
							
						case kCoordinateListMode:
							LoadDItemValue (gStatusDialogPtr, IDC_Status8, 1);
							LoadDItemValue (gStatusDialogPtr, IDC_Status10, 1);
							break;
							
						}	// end "switch (statsWindowMode)" 
						
					ShowDialogWindow (gStatusDialogPtr, 
												kUpdateStatsInfoID, 
												kDoNotSetUpDFilterTable);
					CheckSomeEvents (updateMask + activMask);
					
							// Force text selection to start from end of present 		
							// text.																	
						
					ForceTextToEnd ();
					
					}	// end "if (continueFlag)"
	
						// Get the graph window if needed.										
					
				if (continueFlag && 
									gStatHistogramSpecsPtr->histogramOutputCode == kPlotData)
					{
							// Open a graph window 
								
					#if defined multispec_mac
						graphViewCPtr = CreateGraphWindow ();
					#endif	// defined multispec_mac
					
					#if defined multispec_win
						CMultiDocTemplate* graphDocTemplatePtr = 
								((CMultiSpecApp*)AfxGetApp ())->GetGraphDocTemplate ();
						CMGraphDoc* graphDocCPtr = 
								(CMGraphDoc*)graphDocTemplatePtr->OpenDocumentFile (NULL);
						graphViewCPtr = graphDocCPtr->GetGraphViewCPtr ();
					#endif	// defined multispec_win

					#if defined multispec_lin
						wxDocument*  graph_doc =
											((CMultiSpecApp*)wxTheApp)->ActivateGraphView ();
						 
						graphViewCPtr = ((CMGraphDoc*)graph_doc)->GetGraphViewCPtr ();
						
								// Assign window ID for Histogram window
						((wxWindow*)graphViewCPtr)->SetId (GR_HISTOGRAM);                     
					#endif	// defined multispec_lin
					
					continueFlag = (graphViewCPtr != NULL);
					
					if (continueFlag)
						{
						gGraphRecordPtr = GetGraphRecordPtr (graphViewCPtr, 
																			&handleStatus, 
																			&graphRecHandle);	

						SetSomeStatHistogramGraphParameters (gGraphRecordPtr,
																			statsWindowMode);
							
							// Do not allow data to be plotted until it has been loaded.
														
						gGraphRecordPtr->drawGraphCode = (SInt16)0x8000;
						
						}	// end "if (continueFlag)"
																				
					}	// end "if (continueFlag && ..."	
				 				
				if (continueFlag)	
					{
							// Get buffer to read data from image file into.		

					channelsPtr = (UInt16*)GetHandlePointer (
															gStatHistogramSpecsPtr->channelsHandle);	
																		
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
															gStatHistogramSpecsPtr->numberChannels,
															channelsPtr,
															kPackData,
															kForceBISFormat,
															kForceReal8Bytes,
															kDoNotAllowForThreadedIO,
															&fileIOInstructionsPtr);
					
					}	// end "if (continueFlag)"
					
						// Set the line and column interval to be used.
								
				gAreaDescription.lineInterval = gStatHistogramSpecsPtr->lineInterval;
				gAreaDescription.columnInterval = gStatHistogramSpecsPtr->columnInterval;
			
						// Make certain that memory for statistics information is
						// sufficient.
				
				if (continueFlag)
					continueFlag = !SetupStatHistogramMemory (fileInfoPtr, statsWindowMode);
		
				if (continueFlag)
					{			
					continueFlag = SetupFeatureTransformationMemory (
												gStatHistogramSpecsPtr->featureTransformationFlag, 
												gStatHistogramSpecsPtr->numberFeatures, 
												gStatHistogramSpecsPtr->numberChannels, 
												&gTransformationMatrix.eigenVectorPtr, 
												&gTransformationMatrix.tempMatrixPtr,
												&gTransformationMatrix.offsetVectorPtr,
												&gTransformationMatrix.eigenFeaturePtr,
												kLoadMatricesVectors,
												(UInt16*)gStatHistogramSpecsPtr->featurePtr);
					/*
							// Take into account the offset to be used to force the 
							// transformed data to be in the middle of the allowable range.
													
					if (gTransformationMatrix.offsetVectorPtr != NULL &&
									gTransformationMatrix.createdByCode < 16 && 
												!fileInfoPtr->signedDataFlag)
						{
						offset = fileInfoPtr->numberBins/2;
						
						for (index=0; index<gStatHistogramSpecsPtr->numberFeatures; index++)
							gTransformationMatrix.offsetVectorPtr[index] -= offset;
																
						}	// end "if (....offsetVectorPtr != NULL && ..."
					*/
					}	// end "if (continueFlag)"
				
						// If results are to go the a disk file, 							
						// open the	disk file.	Trick 'CreateResultsDiskFiles' to 	
						// estimate the number of bytes on 'pseudo' lines and 		
						// columns.  Assume 128K bytes needed for now.					
				
				if (continueFlag && (gStatHistogramSpecsPtr->outputStorageType & 0x0002))
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
												gStatHistogramSpecsPtr->outputStorageType, 0, 0);
											
					}	// end "if (continueFlag && ..."
					
						// Make sure the gAreaDescription structure has been initialized.
				
				InitializeAreaDescription (&gAreaDescription); 
				
				stringID = IDS_Project82;	// Computing training area histograms
				if (gStatHistogramSpecsPtr->featureTransformationFlag)
							// Finding feature transformation data value limits
					stringID = IDS_Project84;
					
				LoadDItemStringNumber (kProjectStrID,
												stringID,	
												gStatusDialogPtr,
												IDC_Status11,
												(Str255*)gTextString);
			
						// Get start time.													
			
				startTime = time (NULL);	
											
						// Set line and column interval back to that requested by the user
						// in the area description structure.
						
				gAreaDescription.lineInterval = gStatHistogramSpecsPtr->lineInterval;
				gAreaDescription.columnInterval = gStatHistogramSpecsPtr->columnInterval;
								
						// List the processor name, date, time and project info.	
	
				continueFlag = ListHeaderInfo (
													NULL,
													kLImageInfo+kLProjectName+kLProjectImageName, 
													&gOutputForce1Code, 
													kNoStatisticsUsed, 
													continueFlag);
												
				stringNumber = IDS_Project65;
				if (gStatHistogramSpecsPtr->histogramOutputCode == kPlotData)
					stringNumber = IDS_Project66;
		
				continueFlag = ListSpecifiedStringNumber (kProjectStrID,
																		stringNumber, 
																		(unsigned char*)gTextString, 
																		NULL, 
																		gOutputForce1Code,
																		continueFlag);
				
						// List "  Output Information:"
						
				if (gStatHistogramSpecsPtr->histogramOutputCode != kPlotData)
					continueFlag = ListSpecifiedStringNumber (kSharedStrID, 
																			IDS_Shared8, 
																			(unsigned char*)gTextString, 
																			NULL, 
																			gOutputForce1Code, 
																			continueFlag);
																			
				if (continueFlag && DetermineFieldTypes () == 4)
					{
							// All training areas are defined by clusters. Listing or plotting
							// histograms will not be possible.
							
					continueFlag = ListSpecifiedStringNumber (kProjectStrID,
																			IDS_Project89, 
																			(unsigned char*)gTextString, 
																			NULL, 
																			gOutputForce1Code,
																			continueFlag);
					
					continueFlag = FALSE;
					
					}	// end "if (DetermineFieldTypes () == 4)"
	
						// Now get the memory for and load some of the graph window
						// structures.
					
				if (continueFlag && 
									gStatHistogramSpecsPtr->histogramOutputCode == kPlotData)
					{
					xDataTypeCode = gImageFileInfoPtr->dataTypeCode;
					if (gStatHistogramSpecsPtr->featureTransformationFlag)
						xDataTypeCode = kRealType;
					
					continueFlag = graphViewCPtr->FinishGraphRecordSetUp (
								NULL,
								0,
								(SInt32)gStatHistogramSpecsPtr->initialNumberHistogramDataBins,
								(SInt32)gStatHistogramSpecsPtr->numberVectors,
								gStatHistogramSpecsPtr->numberFeatures,
								xDataTypeCode,
								kIntegerType);

							// Lock handles and get pointers to the graph vectors to
							// be ready to load data into.
							// Ignore errors here. Memory for the x and y vectors may need
							// to be gotten later when we have a count for the number
							// of points needed.
								
					if (continueFlag)
						{
						LockAndGetVectorPointer (&gGraphRecordPtr->xVector, &error);
						LockAndGetVectorPointer (&gGraphRecordPtr->yVector, &error);
						
								// Lock handle to the graph lengths vector.
								
						GetHandlePointer (gGraphRecordPtr->vectorLengthsHandle, kLock);
						
								// Lock handle to the graph offset vector.
						
						GetHandlePointer (gGraphRecordPtr->xVectorDataHandle, kLock);
							
						}	// end "if (continueFlag)"
					
					}	// end "if (... && ...->histogramOutputCode == kPlotData)" 
				
				if (continueFlag)
					{
							// Get the group code.												
							
					outputGroupCode = 
						((gStatHistogramSpecsPtr->outputFormatCode & 0x0f00) >> 8);
						
							// Turn spin cursor on
			
					gPresentCursor = kSpin;
						
							// Get project channel minimum and maximums if needed. They
							// are needed when the data type contains 4 or more bytes.
							// These values are used to translate the data values into
							// bin numbers.
							
					if (fileInfoPtr->numberBytes >= 4 ||
												gStatHistogramSpecsPtr->featureTransformationFlag)
						{
						if (gStatHistogramSpecsPtr->featureTransformationFlag)
							{
							/*
		 					GetEstimatedTransformedChannelMinMaxes (
															gStatHistogramSpecsPtr->numberFeatures, 
															gStatHistogramSpecsPtr->featurePtr, 
															gStatHistogramSpecsPtr->channelMinPtr, 
															gStatHistogramSpecsPtr->channelMaxPtr,
															&gStatHistogramSpecsPtr->minValue,
															&gStatHistogramSpecsPtr->maxValue);
							*/
							GetTransformedChannelMinMaxes (
															gStatHistogramSpecsPtr->numberFeatures, 
															gStatHistogramSpecsPtr->featurePtr,
															fileIOInstructionsPtr, 
															outputGroupCode,
															statsWindowMode, 
															gStatHistogramSpecsPtr->channelMinPtr, 
															gStatHistogramSpecsPtr->channelMaxPtr,
															&gStatHistogramSpecsPtr->minValue,
															&gStatHistogramSpecsPtr->maxValue);
															
							}	// end "if (...->featureTransformationFlag)"		
						
						else	// !gStatHistogramSpecsPtr->featureTransformationFlag							
							GetProjectChannelMinMaxes (
															gStatHistogramSpecsPtr->numberFeatures,
															gStatHistogramSpecsPtr->channelStatsPtr, 
															gStatHistogramSpecsPtr->featurePtr, 
															gStatHistogramSpecsPtr->channelMinPtr, 
															gStatHistogramSpecsPtr->channelMaxPtr,
															&gStatHistogramSpecsPtr->minValue,
															&gStatHistogramSpecsPtr->maxValue);
					
						gStatHistogramSpecsPtr->channelStatsPtr = (HChannelStatisticsPtr)
						CheckAndDisposePtr ((Ptr)gStatHistogramSpecsPtr->channelStatsPtr);
						
						}	// end "if (fileInfoPtr->numberBytes >= 4 || ..."
					
							// Computing training area histograms
					
					LoadDItemStringNumber (kProjectStrID,
													IDS_Project82,
													gStatusDialogPtr,
													IDC_Status11,
													(Str255*)gTextString);
						
							// Get the maximum number of points per class in the project.
							
					gStatHistogramSpecsPtr->maxNumberPixelsInClass = 
												GetMaximumPixelsPerClass (
																			NULL,
																			0, 
																			kTrainingType,
																			kOriginalStats, 
																			kDontIncludeClusterFields);
					
							// Intialize the nextTime variables to indicate when the next check	
							// should occur for a command-. and status information.					
							
					gNextTime = TickCount ();
					gNextStatusTime = TickCount ();
					
					switch (statsWindowMode)
						{
						case kClassListMode:
							returnCode = 
								HistogramProjectStats (fileIOInstructionsPtr, 
																outputGroupCode);
							break;
							
						case kFieldListMode:
							indexStart = 0;
							returnCode = HistogramClassStats (fileIOInstructionsPtr,
																			gProjectInfoPtr->currentClass, 
																			outputGroupCode,
																			&indexStart,
																			0);
							break;
							
						case kCoordinateListMode:
							returnCode = HistogramFieldStats (fileIOInstructionsPtr,
																			gProjectInfoPtr->currentField,
																			outputGroupCode,
																			0,
																			0);
							break;
							
						}	// end "switch (statsWindowMode)"

							// Turn spin cursor off
				
					gPresentCursor = kWait;
						
					if (returnCode == 0)
						continueFlag = FALSE; 
						
					}	// end "if (continueFlag)" 
				
						// Dispose of the IO buffer.		
											
				DisposeIOBufferPointers (fileIOInstructionsPtr,
													&gInputBufferPtr, 
													&gOutputBufferPtr);
	
						// Release any memory used for transformation matrix information.				
						
				ReleaseFeatureTransformationMemory ();
				
				if (gStatHistogramSpecsPtr != NULL)
					{
					CheckAndUnlockHandle (gStatHistogramSpecsPtr->featureHandle);
					gStatHistogramSpecsPtr->featurePtr = NULL;
					
					CheckAndUnlockHandle (gStatHistogramSpecsPtr->channelsHandle);
			
					CheckAndUnlockHandle (gStatHistogramSpecsPtr->classHandle);
			
					CheckAndUnlockHandle (gStatHistogramSpecsPtr->fieldHandle);
						
					gStatHistogramSpecsPtr->histogramDataHandle = 
						UnlockAndDispose (gStatHistogramSpecsPtr->histogramDataHandle);
					gStatHistogramSpecsPtr->histogramRangePtr = NULL;
					gStatHistogramSpecsPtr->histogramDataPtr = NULL;
					
					gStatHistogramSpecsPtr->channelStatsPtr = (HChannelStatisticsPtr)
							CheckAndDisposePtr ((Ptr)gStatHistogramSpecsPtr->channelStatsPtr);
					
					gStatHistogramSpecsPtr->channelMinPtr = 
							CheckAndDisposePtr (gStatHistogramSpecsPtr->channelMinPtr);
					
					gStatHistogramSpecsPtr->channelMaxPtr = 
							CheckAndDisposePtr (gStatHistogramSpecsPtr->channelMaxPtr);
						
					}	// end "if (gStatHistogramSpecsPtr != NULL)" 
					
						// Get the end time and print the time elapsed in the 	
						// output window.														
							
				continueFlag = ListCPUTimeInformation (NULL, continueFlag, startTime);
			
						// Scroll output window to the selection and adjust the 			
						// scroll bar.																	
						
				UpdateOutputWScrolls (gOutputWindow, 1, kDisplayMessage);
					
						// Dispose of updating statistics status dialog box.				
						
				CloseStatusDialog (TRUE);
	
						// Create the graph window.										
					
				if (gStatHistogramSpecsPtr->histogramOutputCode == kPlotData)
					{
							// Verify that we have some points in the graph window.
					
					if (continueFlag)
						{
						if (gGraphRecordPtr->xVector.numberPoints <= 0)
							continueFlag = FALSE;
						
						}	// end if (continueFlag)""		
							
					if (continueFlag)
						{
								// Set graph data vector handles to just the size that was used.
						
						if (gGraphRecordPtr->xVector.numberPoints < 
																		gGraphRecordPtr->xVector.size)
							{
							SInt32				bytesNeeded;
							
							bytesNeeded = 
									gGraphRecordPtr->xVector.numberPoints * sizeof (GRAPHDATA);
							
							MSetHandleSize (
											&gGraphRecordPtr->xVector.baseHandle, bytesNeeded);
							MSetHandleSize (
											&gGraphRecordPtr->yVector.baseHandle, bytesNeeded);
							
							LockAndGetVectorPointer (&gGraphRecordPtr->xVector, &error);
							LockAndGetVectorPointer (&gGraphRecordPtr->yVector, &error);
							
							}	// end "if (gGraphRecordPtr->xVector.numberPoints < ..."
							
						VerifyNeedForSelectVectorControl (gGraphRecordPtr->window);
						
						GetGraphLabels (gGraphRecordPtr);
							
								// Now allow the graph to be drawn.
														 
						gGraphRecordPtr->drawGraphCode = gGraphRecordPtr->graphCodesAvailable;
					
						continueFlag = CreateGraph (gGraphRecordPtr->graphViewCPtr,
																gGraphRecordPtr, 
																1, 
																1, 
																0, 
																0);
						
						if (continueFlag)
							{
									// Now move the controls to the proper location.
										
							DrawGraphControls (gGraphRecordPtr->window);
							
									// Set the proper hide/show and hilighting for the controls.
									
							#if defined multispec_mac
								UpdateGraphControls (gGraphRecordPtr->window);
								SelectWindow (gGraphRecordPtr->window);
							#endif	// defined multispec_mac

							#if defined multispec_win
								gGraphRecordPtr->graphViewCPtr->UpdateGraphControls ();
							#endif	// defined multispec_win

                     #if defined multispec_lin
                        gGraphRecordPtr->graphViewCPtr->UpdateGraphControls ();
                     #endif

							MHSetState (graphRecHandle, handleStatus);
							
							}	// end "if (continueFlag)"
												
						}	// end "if (continueFlag)"
						
					if (!continueFlag)
						{
						if (gGraphRecordPtr != NULL)
							CloseGraphicsWindow (gGraphRecordPtr->window);
							
						}	// end "if (!continueFlag)"
					
					gGraphRecordPtr = NULL;
											
					}	// end "if (continueFlag && ... == kPlotData)"
					
				CheckAndUnlockHandle (gProjectInfoPtr->statHistogramSpecsH);
				gStatHistogramSpecsPtr = NULL;
					
				}	// end "if (LoadStatHistogramSpecs (fileInfoPtr))" 
		
			UnlockProjectWindowInfoHandles ();
			
			}	// end "if (gProjectInfoPtr->imageFileInfoH != NULL)" 
			
		MInitCursor ();
			
		}	// end "if (statsWindowMode >= kClassListMode && ..." 
		
	return (returnCode);

}	// end "HistogramStatsControl" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void InitializeStatHistogramBuffers
//
//	Software purpose:	This routine initializes the buffers used in compiling
//							the histograms statistics.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			PrintHistogram
//
//	Coded By:			Larry L. Biehl			Date: 04/24/1990
//	Revised By:			Larry L. Biehl			Date: 06/14/2006	

void InitializeStatHistogramBuffers (
				FileInfoPtr							fileInfoPtr,
				SInt16								outputGroupCode,
				UInt32								numberColumns,
				UInt16*								featurePtr,
				UInt32								channelSet, 
				HDoublePtr							channelMinPtr, 
				HDoublePtr							channelMaxPtr)

{
	double								binWidth,
											binWidthFactor,
											maxBinValue,
											minBinValue,
											range;
	
	HistogramSummary*					histogramRangePtr;
	
	HUInt32Ptr							endHistogramDataPtr,
											histogramDataPtr;
											
	SInt32								binWidthExponent;
	
	UInt32								channelIndex,
											dataTypeCode,
											index,
											numberBytes,
											finalNumberHistogramBins,
											numberHistogramBins;
											
	
			// For data with 4 bytes or more, allow for an average of 10 pixels per bin,
			// but use a minimum of 32 bins.
			
	numberBytes = fileInfoPtr->numberBytes;
	dataTypeCode = fileInfoPtr->dataTypeCode;
	if (gStatHistogramSpecsPtr->featureTransformationFlag)
		{
		numberBytes = 8;
		dataTypeCode = kRealType;
		
		}	// end "if (gStatHistogramSpecsPtr->featureTransformationFlag)"
	
	numberHistogramBins = gStatHistogramSpecsPtr->initialNumberHistogramDataBins;
	if (numberBytes > 2)
		{
		numberHistogramBins = (UInt32)gStatHistogramSpecsPtr->maxNumberPixelsInClass/10;
		numberHistogramBins = MAX (numberHistogramBins, 64);
		numberHistogramBins = MIN (numberHistogramBins, 1024);
			
		}	// end "if (numberBytes > 2)"
		
	finalNumberHistogramBins = numberHistogramBins;
	
	histogramDataPtr = gStatHistogramSpecsPtr->histogramDataPtr;
	histogramRangePtr = gStatHistogramSpecsPtr->histogramRangePtr;
	endHistogramDataPtr = &histogramDataPtr [
			(UInt32)numberColumns*gStatHistogramSpecsPtr->initialNumberHistogramDataBins];
	
			// Initialize the histogram data buffer.										
			
	for (; histogramDataPtr<endHistogramDataPtr; histogramDataPtr++)
		*histogramDataPtr = 0;
		
	gStatHistogramSpecsPtr->maxNumberEDecimalDigits = 0;
	gStatHistogramSpecsPtr->maxNumberFDecimalDigits = 0;
		
			// Initialize the histogram range buffer.										
	
	for (index=0; index<numberColumns; index++)
		{	
		histogramRangePtr[index].averageValue = 0.;
		histogramRangePtr[index].binFactor = 1.;
		histogramRangePtr[index].maxNonSatValue = 0.;
		histogramRangePtr[index].maxValue = -DBL_MAX;
		histogramRangePtr[index].medianValue = 0.;
		histogramRangePtr[index].minNonSatValue = 0.;
		histogramRangePtr[index].minValue = DBL_MAX;
		histogramRangePtr[index].stdDeviation = 0.;
		histogramRangePtr[index].badValues = 0;
		histogramRangePtr[index].numberEDecimalDigits = 0;
		histogramRangePtr[index].numberFDecimalDigits = 0;
																
		histogramRangePtr[index].availableFlag = FALSE;
		
		if (numberBytes <= 2)
			{
			if (fileInfoPtr->signedDataFlag)
				{
				histogramRangePtr[index].binType = kBinWidthOfOne;
				histogramRangePtr[index].minNonSatValue = fileInfoPtr->minUsableDataValue;
				
				}	// end "if (fileInfoPtr->signedDataFlag)"
				
			else	// !fileInfoPtr->signedDataFlag
				histogramRangePtr[index].binType = kDataValueIsBinIndex;
			
			}	// end "if (numberBytes <= 2)"
			
		else	// numberBytes > 2
			{
			channelIndex = index;
			if (outputGroupCode == kClassFieldGrouping)
				channelIndex = channelSet;
				
			minBinValue = channelMinPtr[channelIndex];
			maxBinValue = channelMaxPtr[channelIndex];
													
			range = maxBinValue - minBinValue;
			
			finalNumberHistogramBins = numberHistogramBins;
			
			if (range <= 0)
				range = 1;
													
			if (dataTypeCode == kIntegerType && range <= numberHistogramBins)
				histogramRangePtr[index].binType = kBinWidthOfOne;
				
			else	// dataTypeCode != kIntegerType || range > numberHistogramBins
				{
				histogramRangePtr[index].binType = kBinWidthNotOne;
				binWidth = range/(double)(numberHistogramBins-3);
				binWidthFactor = ConvertToScientificFormat (binWidth, &binWidthExponent);
				binWidth = floor (binWidthFactor);
				binWidth = MIN (5, binWidth);
				binWidth *= pow ((double)10, (double)binWidthExponent);
				
						// Now adjust the min/max to fit with the bin width.
						
				minBinValue = SIGN (floor ((fabs (minBinValue) + binWidth/2)/binWidth),
											channelMinPtr[channelIndex]);
				minBinValue = minBinValue*binWidth - binWidth/2;
						
				maxBinValue = SIGN (floor ((fabs (maxBinValue) + binWidth/2)/binWidth),
											channelMaxPtr[channelIndex]);
				maxBinValue = maxBinValue*binWidth + binWidth/2;
				
				finalNumberHistogramBins =
											(UInt32)((maxBinValue - minBinValue)/binWidth + 2);
				
				if (finalNumberHistogramBins > 2048)
					{
					binWidthFactor = ceil ((double)(finalNumberHistogramBins-2)/2046);
					binWidth *= binWidthFactor;
					
					finalNumberHistogramBins -= 2;
					finalNumberHistogramBins =
												(UInt32)(finalNumberHistogramBins/binWidthFactor);
					finalNumberHistogramBins += 2;
					
					}	// end "if (finalNumberHistogramBins > 2046)"
				
				histogramRangePtr[index].binFactor = 1/binWidth;
				
				}	// end "else dataTypeCode != kIntegerType || ..."
				
			histogramRangePtr[index].minNonSatValue = minBinValue;
			histogramRangePtr[index].maxNonSatValue = maxBinValue;
												
			}	// end "else numberBytes > 2"
			
		histogramRangePtr[index].numberBins = finalNumberHistogramBins;
														
				// Get the number of decimal digits to use when listing data values.
						
		GetNumberDecimalDigits ((UInt16)dataTypeCode,
										histogramRangePtr[index].minNonSatValue,
										histogramRangePtr[index].maxNonSatValue,
										histogramRangePtr[index].minNonSatValue,
										&histogramRangePtr[index].numberEDecimalDigits,
										&histogramRangePtr[index].numberFDecimalDigits);
																
		gStatHistogramSpecsPtr->maxNumberEDecimalDigits = MAX (
											gStatHistogramSpecsPtr->maxNumberEDecimalDigits,
											histogramRangePtr[index].numberEDecimalDigits);
																
		gStatHistogramSpecsPtr->maxNumberFDecimalDigits = MAX (
											gStatHistogramSpecsPtr->maxNumberFDecimalDigits,
											histogramRangePtr[index].numberFDecimalDigits);
		
		}	// end "for (index=0; index<numberColumns; index++)" 
		
	gStatHistogramSpecsPtr->totalNumberValues = 0;
			
}	// end "InitializeStatHistogramBuffers" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListStatHistogramValues
//
//	Software purpose:	This routine prints the histograms of the selected
//							training field/classes to the output window.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None	
//
// Called By:			HistogramClassStats in statHistogram.c
//							HistogramProjectStats in statHistogram.c
//
//	Coded By:			Larry L. Biehl			Date: 04/09/1990
//	Revised By:			Larry L. Biehl			Date: 12/21/2016	

Boolean ListStatHistogramValues (
				FileInfoPtr							fileInfoPtr, 
				SInt16*								featurePtr,
				Boolean								featureTransformationFlag, 
				HistogramSummaryPtr				histogramRangePtr, 
				HUInt32Ptr							histogramDataPtr, 
				UInt32								numberColumns, 
				SInt32								totalNumberValues, 
				SInt16								outputCode)

{
			// Declare parameters
	
	char									string[500];
		
	double								binValue,
											maxAllowedValue,
											maxValue,
											minAllowedValue,
											minValue;
	
	GrafPtr								savedPort;
	Ptr									charPtr;
	
	HUInt32Ptr							histogramStartPtr,
											tempHistogramDataPtr;
	
	SInt32								columnOffsetCount,
											value,
											valueAfter,
											valueBefore;
	
	UInt32								charIndex,
											colMaxIndex,
											colMinIndex,
											columnEnd,
											columnIndex,
											columnStart,
											k,
											lenText,
											m,
											maxIndex,
											maxNumber,
											minIndex,
											numberBytes,
											totalLength;
	
	SInt16								countFlag,
											listNumber,
											numberColumnarLists,
											outputFormatCode,
											outputGroupCode,
											stringNumber;
	
	UInt16								ePrecision,
											fPrecision,
											numberFieldSize,
											valueFieldSize,
											valueFieldSize2;																
	
	Boolean								badValuesFlag,
											blankValuesFlag,
											continueFlag,
											includeEmptyBinsFlag;
	
	
			// Check input values.																
			
	if (fileInfoPtr == NULL ||
			histogramRangePtr == NULL ||
			histogramDataPtr == NULL ||
			numberColumns <= 0)
																							return (FALSE);
		
			// Put message in histogram status dialog.									
			
	LoadDItemStringNumber (kProjectStrID,
									IDS_Project83,		// Listing histogram values
									gStatusDialogPtr,
									IDC_Status11,
									(Str255*)gTextString);
						
			// Initialize local variables.													
		
	includeEmptyBinsFlag = (outputCode & 0x0001);
	blankValuesFlag = (outputCode & 0x0002);
	outputFormatCode = ((outputCode & 0x00f0) >> 4);
	outputGroupCode = ((outputCode & 0x0f00) >> 8);
	numberFieldSize = 0;
	continueFlag = TRUE;
	
			// Get the maximum length of the field expected for the data value.
			
	ePrecision = gStatHistogramSpecsPtr->maxNumberEDecimalDigits;
	fPrecision = gStatHistogramSpecsPtr->maxNumberFDecimalDigits;
	if (fileInfoPtr->numberBytes <= 2 && !featureTransformationFlag)
		{
		NumToString ((UInt32)fileInfoPtr->numberBins, (UCharPtr)string);
		valueFieldSize = string[0];
		
		}	// end "if (fileInfoPtr->numberBytes <= 2)"
		
	else	// fileInfoPtr->numberBytes > 2 || ...->featureTransformationFlag
		{	
		valueFieldSize = FormatHistogramSummaryString (string,
																		gStatHistogramSpecsPtr->maxValue,
																		ePrecision,
																		fPrecision,
																		FALSE);
			
		valueFieldSize2 = FormatHistogramSummaryString (string,
																		gStatHistogramSpecsPtr->minValue,
																		ePrecision,
																		fPrecision,
																		FALSE);
			
		valueFieldSize = MAX (valueFieldSize, valueFieldSize2);
		
		}	// end "if (fileInfoPtr->numberBytes > 2 || ...)"
		
	if (blankValuesFlag)
		valueFieldSize = MAX (5, valueFieldSize);
		
			// Make sure that one can matrix the channels if requested.
			// If one can not, then do a column list.
	
	if (outputFormatCode == kMatrixList && outputGroupCode == kChannelGrouping)
		{
		if (!CheckIfCanMatrixChannels (histogramRangePtr, numberColumns))
			{
			outputFormatCode = kColumnList;			
	
			gStatHistogramSpecsPtr->outputFormatCode -= 16;
			gStatHistogramSpecsPtr->outputFormatCode += 32;		
			
			}	// end "if (!CheckIfCanMatrixChannels (..."
																
		}	// end "if (outputFormatCode == kMatrixList && ..."
						
			// Get the number of channels and the channel offset bytes to be		
			// used for the median array data												
			
	columnOffsetCount = gStatHistogramSpecsPtr->initialNumberHistogramDataBins;
	
			// Set up to data in columnar list if requested.							
			
	numberColumnarLists = 0;
	if (outputFormatCode == kColumnList)
		{
		numberColumnarLists = (SInt16)numberColumns;
		numberColumns = 1;
		
		}	// end "if (outputFormatCode == kColumnList)" 
		
	numberColumnarLists = MAX (1, numberColumnarLists);
	
			// Get pointer to buffer to use to store count of data values to be	
			// printed.  Make certain that there is a minimum of 64 bytes and at	
			// least 7 digits per channel.													
	
	NumToString ((UInt32)totalNumberValues, gTextString);
	lenText = gTextString[0];
	numberBytes = valueFieldSize + numberColumns * (lenText+1) + 5;
	numberBytes = MAX (numberBytes, numberColumns*7 + valueFieldSize + 5);
	numberBytes = MAX (numberBytes, 64);
	gCharBufferPtr1 = MNewPointer (numberBytes);
	
			// Exit routine if out of memory.												
			
	if (gCharBufferPtr1 == NULL)										
																							return (FALSE);
		
			// Make certain that the output window is the active port.				
			
	GetPort (&savedPort);
	SetPortWindowPort (gOutputWindow);
	
			// If values outside of range are to be blanked out, then this also	
			// means that the columns should line up.  Find the maximum value so	
			// that we can find the number of spaces to allow for the maximum 	
			// number.  Note if this is a class/field group then all we have to	
			// check is the sum column.														
			
	if (blankValuesFlag)
		{
		maxNumber = 0;
		histogramStartPtr = histogramDataPtr;
		
		for (columnIndex=0; columnIndex<numberColumns; columnIndex++)
			{
			minIndex = GetBinIndexForStatDataValue (
															histogramRangePtr[columnIndex].minValue,
															&histogramRangePtr[columnIndex]);
			maxIndex = GetBinIndexForStatDataValue (
															histogramRangePtr[columnIndex].maxValue,
															&histogramRangePtr[columnIndex]);
			
			tempHistogramDataPtr = &histogramStartPtr[minIndex];
			
			for (m=minIndex; m<=maxIndex; m++)
				{
				maxNumber = MAX (maxNumber, *tempHistogramDataPtr);
				tempHistogramDataPtr++;
				
				}	// end "for (m=minValue; m<=maxValue; m++)" 
				
			histogramStartPtr += columnOffsetCount;
						
			}	// end "for (columnIndex=0; columnIndex<..." 
		
		NumToString ((UInt32)maxNumber, gTextString);
		numberFieldSize = gTextString[0];
		
				// Create a string with a tab and 'numberFieldSize' blanks. 		
				
		gTextString3[0] = gTextTab;
		for (m=1; m<=numberFieldSize; m++)
			gTextString3[m] = ' ';
		
		}	// end "if (blankValuesFlag)" 
					
			// Intialize the nextTime variable to indicate when the next check	
			// should occur for a command-.													
			
	gNextTime = TickCount ();
		
	gTextString[0] = gTextTab;
		
	for (listNumber=0; listNumber<numberColumnarLists; listNumber++)
		{		
				// List the Histogram title													
				
		stringNumber = IDS_Project4;
		if (featureTransformationFlag)
			stringNumber = IDS_Project64;
			
		if (outputGroupCode == kClassFieldGrouping && 
																gProjectInfoPtr->histogramClassFlag)
			stringNumber = IDS_Project5;
			
		else if (outputGroupCode == kClassFieldGrouping && 
																gProjectInfoPtr->histogramFieldFlag)
			stringNumber = IDS_Project6;		

		continueFlag = ListSpecifiedStringNumber (kProjectStrID,
																stringNumber, 
																gTextString2, 
																NULL, 
																gOutputForce1Code, 
																continueFlag);
			
		charPtr = strcpy (gCharBufferPtr1, "    Value ");
		charIndex = 10;
		
				// List the channel/class/field numbers.									
		
		if (continueFlag)
			{
					// Add label for sum value for class/field grouping.  Only do	
					// this for the first column be printed if in columnar list 	
					// mode.																			
					
			if (outputGroupCode == kClassFieldGrouping && 
							numberColumns > 2 && 
										listNumber == 0)
				{
						// Handle case when blankValuesFlag is FALSE.					
						
				if (numberFieldSize == 0)
					numberFieldSize = 3;
					
				sprintf ((char*)gTextString2, "Sum");
				sprintf (&gCharBufferPtr1[charIndex],
							"\t%*s",
							numberFieldSize,
							gTextString2);
				charIndex += numberFieldSize + 1;
				
				}	// end "if (outputGroupCode == kClassFieldGrouping ...)"
				
			if (outputFormatCode != kColumnList ||
						outputGroupCode != kClassFieldGrouping || 
								listNumber > 0)
				{	
						// Add label for channel number or class/field number.		
				
				columnStart = 0;	
				columnEnd = numberColumns;
				if (outputGroupCode == kClassFieldGrouping && numberColumns > 2)
							// Take into account the sum column.							
					columnEnd--;
					
				if (outputFormatCode == kColumnList)
					{
					if (outputGroupCode == kChannelGrouping)
						{
						columnStart = listNumber;
						columnEnd = columnStart + 1;
						
						}	// end "if (outputGroupCode == kChannelGrouping)" 
					
					else	// outputGroupCode == kClassFieldGrouping 
						{
						columnStart = listNumber - 1;
						columnEnd = columnStart + 1;
						
						}	// end "else outputGroupCode == kClassFieldGrouping" 
					
					}	// end "if (outputFormatCode == kColumnList)" 
				
				if (!blankValuesFlag)
					for (columnIndex=columnStart; 
							columnIndex<columnEnd; 
							columnIndex++)
						{
						if (outputGroupCode == kChannelGrouping)
							{
							if (!featureTransformationFlag)
								value = gProjectInfoPtr->channelsPtr[featurePtr[columnIndex]];
								
							else	// featureTransformationFlag
								value = featurePtr[columnIndex];
								
							value++;
								
							}	// end "if (outputGroupCode == kChannelGrouping)"
							
						else	// outputGroupCode == kClassFieldGrouping 
							value = columnIndex + 1;
							
						lenText = (UInt32)stc_d (
										(char*)&gTextString[1], (UInt32)value, kMaxLength, FALSE);
						charPtr = strcpy (&gCharBufferPtr1[charIndex], (char*)gTextString);
						charIndex += lenText + 1;
					
						}	// end "for (columnIndex=0; columnIndex<..." 
						
				else	// blankValuesFlag 
					for (columnIndex=listNumber; 
							columnIndex<columnEnd; 
							columnIndex++)
						{
						if (outputGroupCode == kChannelGrouping)
							{
							if (!featureTransformationFlag)
								value = gProjectInfoPtr->channelsPtr[featurePtr[columnIndex]];
								
							else	// featureTransformationFlag
								value = featurePtr[columnIndex];
								
							value++;
								
							}	// end "if (outputGroupCode == kChannelGrouping)"
							
						else	// outputGroupCode == kClassFieldGrouping 
							value = columnIndex + 1;
							
						sprintf (
								&gCharBufferPtr1[charIndex], "\t%*d", numberFieldSize, (int)value);
						charIndex += numberFieldSize + 1;
					
						}	// end "for (columnIndex=0; columnIndex<..." 
				
				}	// end "if (outputFormatCode != kColumnList || ..." 
					
			continueFlag = ListString (gCharBufferPtr1, charIndex, gOutputTextH);
				
			}	// end "if (continueFlag)" 
		
				// Print line of tabs - the same number as was listing above 		
				// with the channel numbers.  This is to help when copying 			
				//	histogram data to CricketGraph.											
		
		if (continueFlag)
			{	 
			gTextString[1] = ' ';
			gTextString[2] = 0;
			charPtr = strcpy (&gCharBufferPtr1[charIndex], gEndOfLine);
			charIndex = gNumberOfEndOfLineCharacters + 1;
			for (columnIndex=0; columnIndex<numberColumns; columnIndex++)
				{
				charPtr = strcpy (&gCharBufferPtr1[charIndex], (char*)gTextString);
				charIndex += 2;
				
				}	// end "for (columnIndex=0; columnIndex<..." 
				
			charPtr = strcpy (&gCharBufferPtr1[charIndex], gEndOfLine);
			charIndex += gNumberOfEndOfLineCharacters;
				
			continueFlag = ListString (gCharBufferPtr1, charIndex, gOutputTextH);
			
			}	// end "if (continueFlag)" 
		
				// Determine the the minimum and maximum data values for all		 
				// channels combined.															
		 
		if (outputFormatCode == 1)
			{
			minValue = DBL_MAX;
			maxValue = -DBL_MAX;
			
			for (columnIndex=0; columnIndex<numberColumns; columnIndex++)
				{
				if (histogramRangePtr[columnIndex].minValue < minValue)
					minValue = histogramRangePtr[columnIndex].minValue;
						
				if (histogramRangePtr[columnIndex].maxValue > maxValue)
					maxValue = histogramRangePtr[columnIndex].maxValue;
						
				}	// end "for (columnIndex=0; columnIndex<..." 
				
			}	// end "if (outputFormatCode == 1)" 
					
		else	// outputFormatCode == 2 
			{
			minValue = histogramRangePtr[listNumber].minValue;
			maxValue = histogramRangePtr[listNumber].maxValue;
			
			}	// end "else outputFormatCode == 2" 
			
				// Make certain that the minimum and maximum values are not out 	
				// of range.	
	
		maxAllowedValue = fileInfoPtr->maxUsableDataValue;
		minAllowedValue = fileInfoPtr->minUsableDataValue;
		if (featureTransformationFlag)
			{
			maxAllowedValue = DBL_MAX;
			minAllowedValue = -DBL_MAX;
			
			}	// end "if (featureTransformationFlag)"																	
				
		maxValue = MIN (maxValue, maxAllowedValue);
		minValue = MAX (minValue, minAllowedValue);
		
		minIndex = GetBinIndexForStatDataValue (minValue,
																&histogramRangePtr[listNumber]);
		maxIndex = GetBinIndexForStatDataValue (maxValue,
																&histogramRangePtr[listNumber]);
		
		histogramStartPtr = &histogramDataPtr[listNumber*columnOffsetCount +  minIndex];
						
				// Print the data values and histogram values. It is recognized 	
				// that m-1 and m+1 for min and max values equal to 0 and 255 or 	
				// 4095 will access data outside the range of the structure.  It 	
				// was decided that that was okay since the count for those 		
				// values will be printed anyhow.  A check would only slow the 	
				// system down.					
				
		ePrecision = histogramRangePtr[listNumber].numberEDecimalDigits;	
		fPrecision = histogramRangePtr[listNumber].numberEDecimalDigits;												
	
		for (m=minIndex; m<=maxIndex; m++)
			{
			if (includeEmptyBinsFlag)
				countFlag = 1;
				
			else	// !includeEmptyBinsFlag 
				{
				countFlag = 0;
				tempHistogramDataPtr = histogramStartPtr;
				for (k=0; k<numberColumns; k++)
					{
							// Set up channel pointer for histogram array values.		
								
					valueBefore = tempHistogramDataPtr[-1];
					value			= *tempHistogramDataPtr;
					valueAfter  = tempHistogramDataPtr[1];
					
					if (valueBefore>0 && value==0)
						break;
					if (value==0 && valueAfter>0)
						break;
					if (value > 0)
						break;
					
					tempHistogramDataPtr += columnOffsetCount;
					
					}	//	End "for (k=0; k<numberColumns..;" loop 
					
				if (k < (UInt32)numberColumns)
					countFlag++;
				
				}	// end "else !includeEmptyBinsFlag..." 
			
			if (countFlag > 0)
				{
				binValue = GetDataValueForStatBinIndex (m,
																		&histogramRangePtr[listNumber]);
				charIndex = 0;
				LoadRealValueString (gCharBufferPtr1,
												binValue,
												valueFieldSize,
												fPrecision,
												ePrecision,
												(char*)"    ",
												(char*)" ");
										
				charIndex += valueFieldSize + 5;
				
				columnEnd = numberColumns;
				if (outputFormatCode == kColumnList)
					columnEnd = listNumber + 1;
			
				tempHistogramDataPtr = histogramStartPtr;
				for (columnIndex=listNumber; 
						columnIndex<columnEnd; 
						columnIndex++)
					{
							// Set up channel pointer for histogram vector.				
					
					if (blankValuesFlag)
						{
						colMinIndex = GetBinIndexForStatDataValue (
													histogramRangePtr[columnIndex].minValue,
													&histogramRangePtr[columnIndex]);
		
						colMaxIndex = GetBinIndexForStatDataValue (
													histogramRangePtr[columnIndex].maxValue,
													&histogramRangePtr[columnIndex]);
																
						if (m < colMinIndex || m > colMaxIndex)
							{
							charPtr = strncpy (&gCharBufferPtr1[charIndex], 
														(char*)gTextString3, 
														numberFieldSize+1);
							
							}	// end "if (m < colMinIndex || m > colMaxIndex)" 
					
						else	// m >= colMinIndex && ... 
							{
							sprintf (&gCharBufferPtr1[charIndex], 
											"\t%*lu", 
											numberFieldSize, 
											*tempHistogramDataPtr);
				
							}	// end "else m >= colMinIndex && ... " 
							
						charIndex += numberFieldSize + 1;
							
						}	// end "if (blankValuesFlag)" 
						
					else	// !blankValuesFlag 
						{
						lenText = stc_d (
							(char*)&gTextString[1], *tempHistogramDataPtr, kMaxLength, FALSE);
						charPtr = 
							strcpy (&gCharBufferPtr1[charIndex], (char*)gTextString);
						charIndex += lenText + 1;
						
						}	// end "else !blankValuesFlag || (m >= ... 
					
					tempHistogramDataPtr += columnOffsetCount;
					
					}	// end "for (columnIndex=listNumber; columnIndex<..." 
					
				charPtr = strncpy (&gCharBufferPtr1[charIndex], 
											(char*)&gEndOfLine, 
											gNumberOfEndOfLineCharacters);
				totalLength = charIndex + gNumberOfEndOfLineCharacters;
				
				if (continueFlag)
					continueFlag = ListString (gCharBufferPtr1, totalLength, gOutputTextH);
													
				if (!continueFlag)
					break;
					
				}	// End "if (countFlag>0)" 
													
			if (!continueFlag)
				break;
				
			histogramStartPtr++;
			
					// Exit routine if user has "command period" down					
					
			if (TickCount () >= gNextTime)
				{
				if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
					{
					continueFlag = FALSE;
					break;
					
					}	// end "if (!CheckSomeEvents (osMask..." 
				
				}	// end "if (TickCount () >= gNextTime)" 
				
			}	// End "for (m=minValue;.." loop 
			
		strncpy (gCharBufferPtr1, (char*)&gEndOfLine, gNumberOfEndOfLineCharacters);
		charIndex = gNumberOfEndOfLineCharacters;
		if (continueFlag)	
			continueFlag = ListString (gCharBufferPtr1, charIndex, gOutputTextH);
			
		if (!continueFlag)
			break;
		
		}	// end "for (listNumber=0; listNumber<numberColumnarLists; ...)" 
									
			// List summary of bad values if there are any.								
	
	badValuesFlag = FALSE;	
	for (columnIndex=0; columnIndex<numberColumns; columnIndex++)
		if (histogramRangePtr[columnIndex].badValues != 0)
			badValuesFlag = TRUE;
	
	if (badValuesFlag && continueFlag)
		{
		charPtr = strcpy (gCharBufferPtr1, "    Number of bad values");
		charIndex = 24;
		
		for (columnIndex=0; columnIndex<numberColumns; columnIndex++)
			{
			strncpy (&gCharBufferPtr1[charIndex], (char*)&gTextTab, 1);
			charIndex++;
			NumToString ((UInt32)histogramRangePtr[columnIndex].badValues, gTextString);
			strncpy (
					&gCharBufferPtr1[charIndex], (char*)&gTextString[1], gTextString[0]);
			charIndex += gTextString[0];
			
			}	// end "for (columnIndex=0; columnIndex<..." 
		
		strncpy (&gCharBufferPtr1[charIndex], 
						(char*)&gEndOfLine, 
						gNumberOfEndOfLineCharacters);
		charIndex += gNumberOfEndOfLineCharacters;
			
		if (continueFlag)	
			continueFlag = ListString (gCharBufferPtr1, charIndex, gOutputTextH);
			
		}	// end "if (badValuesFlag && continueFlag)" 
	
	gCharBufferPtr1 = CheckAndDisposePtr (gCharBufferPtr1);
	
	SetPort (savedPort);
	
	return (continueFlag);
		
}	// end "ListStatHistogramValues" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListChannelInformation
//
//	Software purpose:	The purpose of this routine is to list the channel
//							information in a single line.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 04/25/1990
//	Revised By:			Larry L. Biehl			Date: 09/18/2006	

Boolean ListChannelInformation (
				FileInfoPtr							fileInfoPtr, 
				SInt32								featureNumber,
				Boolean								featureTransformationFlag)

{
			// Declare local variables and structures										
	
	ChannelDescriptionPtr			channelDescriptionPtr;
	
	SInt16								numberCharacters;
	
	Boolean								continueFlag;
	
	
	channelDescriptionPtr = NULL;
		
			// Print a list of the channels used.											
	
	if (fileInfoPtr != NULL && 
				fileInfoPtr->descriptionsFlag && 
							!featureTransformationFlag)
		{
		channelDescriptionPtr = (ChannelDescriptionPtr)GetHandlePointer (
																	fileInfoPtr->channelDescriptionH,
																	kLock);
																	
		gTextString2[16] = 0;
		
		}	// end "if (fileInfoPtr != NULL && ..."
	
	if (!featureTransformationFlag)	
		numberCharacters = sprintf (
										(char*)gTextString,
										"%s    Channel  %ld",
										gEndOfLine, 
										(SInt32)gProjectInfoPtr->channelsPtr[featureNumber]+1);
	
	else	// featureTransformationFlag 
		numberCharacters = sprintf ((char*)gTextString, 
												"%s    Feature  %ld",
												gEndOfLine, 
												featureNumber+1);
	
	if (channelDescriptionPtr == NULL)
		sprintf ((char*)&gTextString[numberCharacters], 
					"%s",
					gEndOfLine);
	
	else	// channelDescriptionPtr != NULL
		{
		BlockMoveData (&channelDescriptionPtr[featureNumber], gTextString3, 16);
		gTextString3[16] = 0;
		
		sprintf ((char*)&gTextString[numberCharacters],
					": %s%s",
					gTextString3,
					gEndOfLine);
		
		}	// end "else channelDescriptionPtr != NULL" 
		
	continueFlag = ListString ((char*)gTextString, 
										(UInt32)strlen ((char*)gTextString), 
										gOutputTextH);
	
	if (channelDescriptionPtr != NULL)	
		CheckAndUnlockHandle (fileInfoPtr->channelDescriptionH);
										
	return (continueFlag);
											
}	// end "ListChannelInformation" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void LoadClassMeanAndStdDevData
//
//	Software purpose:	The purpose of this routine is to load the requested class
//							mean and standard deviation data into the graph class statistics
//							vector.
//
//	Parameters in:		.
//
//	Parameters out:	
//
// Value Returned:	None		
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/04/1999
//	Revised By:			Larry L. Biehl			Date: 02/23/2016

Boolean LoadClassMeanAndStdDevData (
				GraphPtr								graphRecordPtr,
				SInt16								statsWindowMode)

{
	HChannelStatisticsPtr			tempChannelStatsPtr;
	
	HDoublePtr							classCovariancePtr,
											classStatisticsPtr,
											outputVectorPtr,
											savedOutputVectorPtr;
	
	HPClassNamesPtr					classNamesPtr;
	UInt16*								statFeaturePtr;
	
	SInt32								numberBytes;
	
	UInt32								classCount,
											classIndex,
											classStorage,
											feature,
											numberClasses,
											numberFeatures,
											startIndex,
											statIndex,
											statIndexIncrement;
											
	Boolean								continueFlag;
	
	
	if (!gStatHistogramSpecsPtr->overlayDensityFunctionFlag)
																							return (FALSE);
	
			// Initialize local variables.	
			
	classNamesPtr = gProjectInfoPtr->classNamesPtr;
	numberFeatures = gStatHistogramSpecsPtr->numberFeatures;
	
	classStatisticsPtr = NULL;
	classCovariancePtr = NULL;
	savedOutputVectorPtr = NULL;		
	tempChannelStatsPtr = NULL;
					
	switch (statsWindowMode)
		{
		case kClassListMode:
			startIndex = 0;					
			numberClasses = gProjectInfoPtr->numberStatisticsClasses;
			break;
			
		case kFieldListMode:
			startIndex = gProjectInfoPtr->currentClass;					
			numberClasses = startIndex + 1;
			break;
			
		case kCoordinateListMode:
																							return (FALSE);
			break;
			
		}	// end "switch (statsWindowMode)" 
		
			// Get temporary memory to be used for the class channels statistics.												
			
	numberBytes = (SInt32)gProjectInfoPtr->numberStatisticsChannels * 
																			sizeof (ChannelStatistics);
																			
	tempChannelStatsPtr = (HChannelStatisticsPtr)MNewPointer (numberBytes);	
		
			// Get temporary memory for the output vectors.												
			
	numberBytes = (SInt32)gProjectInfoPtr->numberStatisticsChannels * sizeof (double);
	
	if (tempChannelStatsPtr != NULL)
		savedOutputVectorPtr = (HDoublePtr)MNewPointer (numberBytes);
		
			// Get temporary memory for covariance matrix.
															
	numberBytes = (gStatHistogramSpecsPtr->numberChannels + 1) *
															gStatHistogramSpecsPtr->numberChannels/2;
	if (gStatHistogramSpecsPtr->featureTransformationFlag)
		numberBytes = gStatHistogramSpecsPtr->numberChannels *
															gStatHistogramSpecsPtr->numberChannels;
	numberBytes *= sizeof (double);
	
	if (savedOutputVectorPtr != NULL)
		classCovariancePtr = (HDoublePtr)MNewPointer (numberBytes);
	
			// Get pointer to the graph class statistics structure;
			
	if (classCovariancePtr != NULL)
		classStatisticsPtr = (HDoublePtr)GetHandlePointer (
															graphRecordPtr->classStatisticsHandle,
															kLock);
	
	statIndexIncrement = graphRecordPtr->classStatsIncrement * 
																		graphRecordPtr->numberVectors;
		
	continueFlag = (classStatisticsPtr != NULL);
		
	if (continueFlag)
		{
				// Get the feature vector to use. It depends on whether the data
				//	is to be transformed.
				
		statFeaturePtr = GetStatisticsFeatureVector (
											gStatHistogramSpecsPtr->featureTransformationFlag, 
											gStatHistogramSpecsPtr->featurePtr);
											
		classCount = 0;
		for (classIndex=startIndex; classIndex<numberClasses; classIndex++)
			{
					// Get the class storage number.										
						
			classStorage = gProjectInfoPtr->storageClass[classIndex];
			
					// Get the class channel statistics.
					
			GetTransformedClassCovarianceMatrix (
												gStatHistogramSpecsPtr->numberChannels,
												tempChannelStatsPtr,
												classCovariancePtr,
												statFeaturePtr, 
												(UInt16)classIndex,
												kTriangleOutputMatrix, 
												gProjectInfoPtr->statisticsCode,
												gTransformationMatrix.eigenVectorPtr,
												gTransformationMatrix.tempMatrixPtr,
												(UInt16)gStatHistogramSpecsPtr->numberFeatures);
											
					// Get the transformed class mean vector	
					// Note that the channel vector has already been reduced.												
			
			outputVectorPtr = savedOutputVectorPtr;
			GetTransformedMeanVector (tempChannelStatsPtr, 
												outputVectorPtr,
												(SInt16)gStatHistogramSpecsPtr->numberChannels,
												NULL,
												gTransformationMatrix.eigenVectorPtr,
												gTransformationMatrix.tempMatrixPtr,
												gTransformationMatrix.offsetVectorPtr,
												(SInt16)gStatHistogramSpecsPtr->numberFeatures);
											
					// Now load the mean data values.
			
			statIndex = classCount * graphRecordPtr->classStatsIncrement;		
			for (feature=0; feature<numberFeatures; feature++)
				{
							// Check for bad data. This may occur for very large numbers.
							
				#if defined multispec_mac 
					if (isnan (*outputVectorPtr))
				#endif	// defined multispec_mac 
				#if defined multispec_win 
					if (_isnan (*outputVectorPtr))
				#endif	// defined multispec_win  	 	
            #if defined multispec_lin
                  if (std::isnan (*outputVectorPtr))
				#endif
						{
						continueFlag = FALSE;
						break;
						
						}	// end "if (isnan (*outputVectorPtr))"
					
				if (gTransformationMatrix.offsetVectorPtr == NULL)
					classStatisticsPtr[statIndex] = *outputVectorPtr;
					
				else	// gTransformationMatrix.offsetVectorPtr != NULL
					classStatisticsPtr[statIndex] = *outputVectorPtr - 
													gTransformationMatrix.offsetVectorPtr[feature];
				
				outputVectorPtr++;
				statIndex += statIndexIncrement;
				
				}	// end "for (feature=0; feature<numberFeatures; feature++)"
				
			if (!continueFlag)
				break;
							
					// Get the transformed class standard deviation vector													
			
			outputVectorPtr = savedOutputVectorPtr;
			
			GetStdDevVectorFromCovariance (classCovariancePtr,
														gStatHistogramSpecsPtr->numberChannels,
														kTriangleOutputMatrix, 
														gProjectInfoPtr->statisticsCode, 
														outputVectorPtr);
			
					// Now load the standard deviation data values.
			
			statIndex = classCount * graphRecordPtr->classStatsIncrement + 1;		
			for (feature=0; feature<numberFeatures; feature++)
				{
				classStatisticsPtr[statIndex] = *outputVectorPtr;
				
				outputVectorPtr++;
				statIndex += statIndexIncrement;
				
				}	// end "for (feature=0; feature<numberFeatures; feature++)"
											
			classCount++;
			
			}	// end "for (classIndex=startIndex; ... 
			
		}	// end "if (continueFlag)"
		
	if (!continueFlag)
		{
		gStatHistogramSpecsPtr->overlayDensityFunctionFlag = FALSE;
		if (graphRecordPtr->graphCodesAvailable & 0x0004)
			graphRecordPtr->graphCodesAvailable -= 4;
		
		}	// end "if (!continueFlag)"
		
	CheckAndDisposePtr ((Ptr)tempChannelStatsPtr);
	CheckAndDisposePtr (savedOutputVectorPtr);
	CheckAndDisposePtr (classCovariancePtr);
	
	CheckAndUnlockHandle (graphRecordPtr->classStatisticsHandle);
	
	return (continueFlag);
											
}	// end "LoadClassMeanAndStdDevData" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void LoadGraphData
//
//	Software purpose:	The purpose of this routine is to load the Statistics 
//							Histogram Specification structure with the default set of
//							parameters.
//
//	Parameters in:		.
//
//	Parameters out:	
//
// Value Returned:	None		
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/22/1998
//	Revised By:			Larry L. Biehl			Date: 02/13/2006

Boolean LoadGraphData (
				FileInfoPtr							fileInfoPtr,
				SInt16								indexStart)

{
	double								value;
	
	double*								histogramBinWidthPtr;
	
	HistogramSummary*					histogramRangePtr;
											
	HSInt32Ptr							vectorLengthsPtr,
											vectorDataPtr;
	
	HUInt32Ptr							histogramDataPtr;
	
	SInt32								error,
											signedValueOffset;
	
	UInt32								binIndex,
											graphIndex,
											histogramIndex,
											index,
											maxIndex,
											minIndex,
											numberFeatures,
											numberBins,
											numberDataValuesInChannel,
											vectorIndex;
											
	Boolean								continueFlag;


			// Initialize local variables.

	numberBins = gStatHistogramSpecsPtr->initialNumberHistogramDataBins;
	signedValueOffset = fileInfoPtr->signedValueOffset;
	numberFeatures = gStatHistogramSpecsPtr->numberFeatures;
	histogramDataPtr = gStatHistogramSpecsPtr->histogramDataPtr;
	histogramRangePtr = gStatHistogramSpecsPtr->histogramRangePtr;
	continueFlag = TRUE;
	error = noErr;
	/*
			// Get the min and max allowed values	
			
	maxAllowedValue = fileInfoPtr->maxUsableDataValue;
	minAllowedValue = fileInfoPtr->minUsableDataValue;
	if (gStatHistogramSpecsPtr->featureTransformationFlag)
		{
		maxAllowedValue = DBL_MAX;
		minAllowedValue = -DBL_MAX;
		
		}	// end "if (gStatHistogramSpecsPtr->featureTransformationFlag)"
	*/
			// Get the starting histogram index. Take into account whether the
			// data are signed.
	
	histogramIndex = 0;

			// Check if the x and y graph vector sizes need to be changed. If so
			// count the number of points that it will need to be enlarged by.
			// Then adjust the size of the x and y vectors.
			
	if (gGraphRecordPtr->xVector.numberPoints >= gGraphRecordPtr->xVector.size)
		continueFlag = CheckSizeOfStatHistogramGraphVectors (gGraphRecordPtr,
																				histogramRangePtr,
																				histogramDataPtr,
																				numberFeatures,
																				histogramIndex,
																				numberBins);
	
	if (continueFlag)
		{																				
				// Get the graph lengths vector. Handle is already locked.
				
		vectorLengthsPtr = (HSInt32Ptr)GetHandlePointer (
															gGraphRecordPtr->vectorLengthsHandle);
		
				// Get the graph offset vector. The offsets will be the same for the x
				// and y data for histogram type plots. Handle is already locked.
		
		vectorDataPtr = (HSInt32Ptr)GetHandlePointer (
															gGraphRecordPtr->xVectorDataHandle);
		
				// Get the histgram bin width vector. Handle is already locked.
		
		histogramBinWidthPtr = (double*)GetHandlePointer (
															gGraphRecordPtr->histogramBinWidthHandle);
												
				// Initialize the index into the graph vectors.
				
		graphIndex = gGraphRecordPtr->xVector.numberPoints;
							
		for (index=0; index<numberFeatures; index++)
			{
					// Get the min and max data value and then just loop throught the
					// histogram data vector between those 2 values to load the graphics
					// vector.
					
			minIndex = GetBinIndexForStatDataValue (histogramRangePtr[index].minValue,
																	&histogramRangePtr[index]);
			maxIndex = GetBinIndexForStatDataValue (histogramRangePtr[index].maxValue,
																	&histogramRangePtr[index]);
			
			numberDataValuesInChannel = 0;
			
			if (minIndex <= maxIndex)
				{
				for (binIndex=minIndex; binIndex<=maxIndex; binIndex++)
					{
					if (histogramDataPtr[histogramIndex+binIndex] > 0)
						{
								// Get the data value for the bin index.
								
						value = GetDataValueForStatBinIndex (binIndex,
																			&histogramRangePtr[index]);
						
						SetV (&gGraphRecordPtr->xVector, 
								graphIndex, 
								(double)(value), 
								&error);
							
						SetV (&gGraphRecordPtr->yVector, 
								graphIndex, 
								(double)histogramDataPtr[histogramIndex+binIndex], 
								&error);
								
						graphIndex++;
						
						if (error != noErr)
							break;
						
						numberDataValuesInChannel++;	
						
						}	// end "if (histogramDataPtr[histogramIndex+binIndex] > 0)"
					
					}	// end "for (binIndex=minIndex; binIndex<=maxIndex; binIndex++)"
					
				}	// end "if (minIndex <= maxIndex)"
				
			vectorIndex = index*gGraphRecordPtr->numberVectors + indexStart;
		
			vectorLengthsPtr[vectorIndex] = numberDataValuesInChannel;
					
			vectorDataPtr[vectorIndex] = gGraphRecordPtr->xVector.numberPoints;
			
			histogramBinWidthPtr[index] = 1;		
			if (histogramRangePtr[index].binType == kBinWidthNotOne)
				histogramBinWidthPtr[index] = 1./histogramRangePtr[index].binFactor;
	
			gGraphRecordPtr->xVector.numberPoints += numberDataValuesInChannel;
			gGraphRecordPtr->yVector.numberPoints += numberDataValuesInChannel;
				
			histogramIndex += numberBins;
						
			if (error != noErr)
				break;
			
			}	// end "for (index=0; index<numberFeatures; index++)"
			
		}	// end "if (continueFlag)"
					
	if (error != noErr)
		continueFlag = FALSE;
		
	return (continueFlag);
											
}	// end "LoadGraphData" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void LoadListOfClassFieldNames
//
//	Software purpose:	The purpose of this routine is to load either the specified
//							training class or field names into the input list.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/21/1999
//	Revised By:			Larry L. Biehl			Date: 01/21/1999

void LoadListOfClassFieldNames (
				Str31*								stringPtr, 
				SInt16								statsWindowMode,
				Boolean								classNameFlag)

{	
	switch (statsWindowMode)
		{
		case kClassListMode:
			LoadProjectClassFieldNames (stringPtr, classNameFlag);
			break;
			
		case kFieldListMode:
			LoadClassClassFieldNames (stringPtr,
												gProjectInfoPtr->currentClass, 
												classNameFlag);
			break;
			
		}	// end "switch (statsWindowMode)"

}	// end "LoadListOfClassNames" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 LoadProjectClassFieldNames
//
//	Software purpose:	The purpose of this routine is to load the project class or field
//							names into the input string vector.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/26/1999
//	Revised By:			Larry L. Biehl			Date: 02/03/1999	

void LoadProjectClassFieldNames (
				Str31*								stringPtr,
				Boolean								classNameFlag)

{
	HPClassNamesPtr					classNamesPtr;
	
	UInt32								classIndex,
											classStorage,
											numberClasses;
	
	
			// Initialize local variables.													
			
	classNamesPtr = gProjectInfoPtr->classNamesPtr;
	numberClasses = gProjectInfoPtr->numberStatisticsClasses;					
	
	for (classIndex=0; classIndex<numberClasses; classIndex++)
		{
				// Get the class storage number.										
					
		classStorage = gProjectInfoPtr->storageClass[classIndex];
	
			// Add names for classes or fields as requested.
			
		if (classNameFlag)
			{
			BlockMoveData ((char*)&classNamesPtr[classStorage].name,
								(char*)stringPtr,
								32);
									
			stringPtr++;
			
			}	// end "if (classNameFlag)"
			
		else	// !classNameFlag
			{
					// Load the training field names.									
						
			stringPtr = LoadClassClassFieldNames (stringPtr,
																(SInt16)classIndex,
																classNameFlag);
			
			}	// end "else !classNameFlag"
		
		}	// end "for (classIndex=0; ... 
		
}	// end "LoadProjectClassFieldNames" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void LoadClassClassFieldNames
//
//	Software purpose:	The purpose of this routine is to load the field names for the
//							input class into the input vector string.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/26/1999
//	Revised By:			Larry L. Biehl			Date: 01/26/1999	

Str31* LoadClassClassFieldNames (
				Str31*								stringPtr,
				SInt16								classNumber,
				Boolean								classNameFlag)

{
	HPClassNamesPtr					classNamesPtr;
	HPFieldIdentifiersPtr			fieldIdentPtr;
	
	UInt32								classStorage;
	
	SInt32								fieldNumber;
	
	
			// Initialize local variables.													
			
	classNamesPtr = gProjectInfoPtr->classNamesPtr;
	
			// Get the class storage number.													
						
	classStorage = gProjectInfoPtr->storageClass[classNumber];					
	
	if (classNamesPtr[classStorage].numberOfTrainFields > 0)
		{		
		fieldNumber = classNamesPtr[classStorage].firstFieldNumber;
		
		while (fieldNumber != -1)
			{
			fieldIdentPtr = &gProjectInfoPtr->fieldIdentPtr[fieldNumber];

					// Make certain that field is training field.					
					
			if (fieldIdentPtr->fieldType == kTrainingType)
				{
				BlockMoveData ((char*)&fieldIdentPtr->name,
									(char*)stringPtr,
									32);
										
				stringPtr++;
								
				}	// end "if (fieldIdentPtr->field..." 
				
			fieldNumber = fieldIdentPtr->nextField;
			
			}	// end "while (fieldNumber != -1)" 
			
		}	// end "if (classNamesPtr[classStorage].number ..." 
	
			// Indicate that routine completed normally.								
			
	return (stringPtr);
		
}	// end "LoadClassClassFieldNames" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean LoadStatHistogramSpecs
//
//	Software purpose:	The purpose of this routine is to load the Statistics 
//							Histogram Specification structure with the default set of
//							parameters.
//
//	Parameters in:		.
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			StatHistogramControl
//
//	Coded By:			Larry L. Biehl			Date: 04/02/1990
//	Revised By:			Larry L. Biehl			Date: 07/28/2017

Boolean LoadStatHistogramSpecs (
				FileInfoPtr							fileInfoPtr)

{
	SInt16								*channelsPtr,
											*classPtr,
											*fieldPtr;
	
	SInt32								bytesNeeded;
	
	Boolean								changedFlag,
											returnFlag;
	
	
			// Assume routine will work okay.  If there isn't enough memory		
			// then the flag will be changed.												
			
	returnFlag = TRUE;
	
	if (gProjectInfoPtr->statHistogramSpecsH == NULL)
		{
				// Set up handle to project file information								
			
		gProjectInfoPtr->statHistogramSpecsH = MNewHandle (sizeof (StatHistogramSpecs));
		if (gProjectInfoPtr->statHistogramSpecsH != NULL)
			{
			gStatHistogramSpecsPtr = (StatHistogramSpecsPtr)GetHandlePointer (
																gProjectInfoPtr->statHistogramSpecsH,
																kLock, 
																kMoveHi);
			
					// Initialize the statistics histogram specification				
					// structure.		
					
			gStatHistogramSpecsPtr->channelMaxPtr = NULL;
			gStatHistogramSpecsPtr->channelMinPtr = NULL;
			gStatHistogramSpecsPtr->channelStatsPtr = NULL;															
					
			gStatHistogramSpecsPtr->histogramRangePtr = NULL;
			gStatHistogramSpecsPtr->histogramDataPtr = NULL;
			gStatHistogramSpecsPtr->featurePtr = NULL;
			
			gStatHistogramSpecsPtr->featureHandle = NULL;
			gStatHistogramSpecsPtr->channelsHandle = NULL;
			gStatHistogramSpecsPtr->classHandle = NULL;
			gStatHistogramSpecsPtr->fieldHandle = NULL;
			gStatHistogramSpecsPtr->histogramDataHandle = NULL;
			
			gStatHistogramSpecsPtr->columnInterval = 1;
			gStatHistogramSpecsPtr->lineInterval = 1;
			gStatHistogramSpecsPtr->numberClasses =
														gProjectInfoPtr->numberStatisticsClasses;
			gStatHistogramSpecsPtr->totalNumberValues = 0;
			
			gStatHistogramSpecsPtr->classSet = kAllMenuItem;
			gStatHistogramSpecsPtr->channelSet = kAllMenuItem;
			gStatHistogramSpecsPtr->fieldSet = 0;
			
			gStatHistogramSpecsPtr->histogramOutputCode = kPlotData;
			if (DetermineFieldTypes () == 4)
				gStatHistogramSpecsPtr->histogramOutputCode = kListData;
			
			gStatHistogramSpecsPtr->lastClassNumberUsed = 0;
			gStatHistogramSpecsPtr->lastFieldNumberUsed = 0;
			gStatHistogramSpecsPtr->numberColumns = 0;
			gStatHistogramSpecsPtr->numberVectors = 0;
			
			gStatHistogramSpecsPtr->outputFormatCode = 0x0010 | 0x0200;
			gStatHistogramSpecsPtr->outputStorageType = 1;
			
			gStatHistogramSpecsPtr->initialNumberHistogramDataBins = 256;
			gStatHistogramSpecsPtr->numberChannels =
														gProjectInfoPtr->numberStatisticsChannels;
			gStatHistogramSpecsPtr->numberFeatures =
														gProjectInfoPtr->numberStatisticsChannels;
			gStatHistogramSpecsPtr->numberFields = 0;
			
			gStatHistogramSpecsPtr->featureTransformationFlag = FALSE;
			gStatHistogramSpecsPtr->overlayDensityFunctionFlag = TRUE;
			
			}	// end "if (gProjectInfoPtr->statHistogramSpecsH != NULL)" 
		
		else	// else gProjectInfoPtr->statHistogramSpecsH == NULL 
			returnFlag = FALSE;
			
		}	// end "if (gProjectInfoPtr->statHistogramSpecsH == NULL)" 
		
	else	// gProjectInfoPtr->statHistogramSpecsH != NULL 
		{
				// Lock the handle to the classification specifications and get	
				// a pointer to the structure.												
				
		gStatHistogramSpecsPtr = (StatHistogramSpecsPtr)GetHandlePointer (
															gProjectInfoPtr->statHistogramSpecsH,
															kLock, 
															kMoveHi);
		
				// Make sure that the featureTransformationFlag makes sense.
										
		if (!EigenvectorInfoExists (kProject))
			gStatHistogramSpecsPtr->featureTransformationFlag = FALSE;
				
		}	// end "else gProjectInfoPtr->statHistogramSpecsH != NULL" 
					
			// Check memory for statistics histogram classes vector.					
	
	if (returnFlag)
		{	
		bytesNeeded =
					(SInt32)gProjectInfoPtr->numberStatisticsClasses * sizeof (SInt16);
		classPtr = (SInt16*)CheckHandleSize (&gStatHistogramSpecsPtr->classHandle,
															&returnFlag, 
															&changedFlag, 
															bytesNeeded);
		if (changedFlag)
			gStatHistogramSpecsPtr->classSet = kAllMenuItem;
			
		}	// end "if (returnFlag)" 
			
			// If memory is not full, check memory for histogram training fields	
			// vector.  For now we will allow enough space for all training 		
			// fields to belong to one class.  This should not be too wastefull.	

	if (returnFlag)
		{
		bytesNeeded =
				(SInt32)gProjectInfoPtr->numberStatTrainFields * sizeof (SInt16);
		fieldPtr = (SInt16*)CheckHandleSize (&gStatHistogramSpecsPtr->fieldHandle,
															&returnFlag, 
															&changedFlag, 
															bytesNeeded);
		if (changedFlag)
			gStatHistogramSpecsPtr->fieldSet = 0;
			
		}	// end "if (returnFlag)" 
			
			// If memory is not full, check memory for statistics histogram		
			// feature vector pointer.															

	if (returnFlag)
		{
		bytesNeeded =
					(SInt32)gProjectInfoPtr->numberStatisticsChannels * sizeof (SInt16);
		gStatHistogramSpecsPtr->featurePtr = 
								(UInt16*)CheckHandleSize (
															&gStatHistogramSpecsPtr->featureHandle,
															&returnFlag,  
															&changedFlag, 
															bytesNeeded);
		if (changedFlag)
			gStatHistogramSpecsPtr->channelSet = kAllMenuItem;
			
		}	// end "if (returnFlag)" 
			
			// If memory is not full, check memory for statistics histogram		
			// channels vector.																	

	if (returnFlag)
		channelsPtr = (SInt16*)CheckHandleSize (
														&gStatHistogramSpecsPtr->channelsHandle,
														&returnFlag,   
														&changedFlag, 
														bytesNeeded);
	
	if (returnFlag)
		{
		
				// Make certain that feature and class vectors are						
				// filled in properly.															
		
		if (gStatHistogramSpecsPtr->channelSet == kAllMenuItem)
			LoadFeatureVector (gStatHistogramSpecsPtr->featureTransformationFlag,
										(SInt16*)&gStatHistogramSpecsPtr->numberFeatures,
										(SInt16*)gStatHistogramSpecsPtr->featurePtr,
										gProjectInfoPtr->numberStatisticsChannels);
															
		LoadChannelsVector (kProject, 
									gStatHistogramSpecsPtr->featureTransformationFlag,
									(SInt16)gStatHistogramSpecsPtr->numberFeatures, 
									(SInt16*)gStatHistogramSpecsPtr->featurePtr, 
									(SInt16*)&gStatHistogramSpecsPtr->numberChannels,
									channelsPtr);
		
		if (gStatHistogramSpecsPtr->classSet == kAllMenuItem)								
			LoadClassVector (&gStatHistogramSpecsPtr->numberClasses, classPtr);
		
		if (gStatHistogramSpecsPtr->fieldSet == 0)								
			LoadFieldVector (&gStatHistogramSpecsPtr->numberFields,
																fieldPtr, kTrainingType);
															
				// Make certain that parameters make sense for the stat window		
				// mode that was used.															
		
		if (gProjectInfoPtr->statsWindowMode == kCoordinateListMode)
			{
			gStatHistogramSpecsPtr->outputFormatCode = 
									(gStatHistogramSpecsPtr->outputFormatCode & 0x0dff);
			gStatHistogramSpecsPtr->outputFormatCode = 
									(gStatHistogramSpecsPtr->outputFormatCode | 0x0100);
									
			}	// end "if (gProjectInfoPtr->statsWindowMode == kCoordinateListMode)" 
			
		}	// end "if (returnFlag)" 
	
			// If unable to set up information for classification						
			// specifications, release memory allocated to it (if any).				
			
	else	// !returnFlag 
		ReleaseStatHistogramSpecsMemory (&gProjectInfoPtr->statHistogramSpecsH);
		
	return (returnFlag);

}	// end "LoadStatHistogramSpecs" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetSomeStatHistogramGraphParameters
//
//	Software purpose:	This routine gets the title for the graph for the
//							biplot graphics window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			HistogramStatsControl
//
//	Coded By:			Larry L. Biehl			Date: 02/04/1999
//	Revised By:			Larry L. Biehl			Date: 02/05/1999			

void SetSomeStatHistogramGraphParameters (
				GraphPtr								graphRecordPtr,
				SInt16								statsWindowMode)
											
{
	UInt32								classStorage;
	
	
	if (graphRecordPtr != NULL)
		{
		graphRecordPtr->flag = NU_HISTOGRAM_PLOT;
		
				// Set the graph codes available for the histogram graphs.
													
		graphRecordPtr->graphCodesAvailable = 1;
		
		if (gStatHistogramSpecsPtr->overlayDensityFunctionFlag)
			{
			if (statsWindowMode == kClassListMode)
				{
				if (gProjectInfoPtr->statsUpToDate)
					{
					graphRecordPtr->graphCodesAvailable += 4;
					/*
					if (DetermineIfLOOExists (-1))
						graphRecordPtr->graphCodesAvailable += 8;
			
					if (gProjectInfoPtr->enhancedStatsExistFlag)
						graphRecordPtr->graphCodesAvailable += 16;
					*/
					}	// end "if (gProjectInfoPtr->statsUpToDate)"
					
				}	// end "if (statsWindowMode == kClassListMode)"
					
			else if (statsWindowMode == kFieldListMode)
				{
				classStorage = 
							gProjectInfoPtr->storageClass[gProjectInfoPtr->currentClass];
				if (gProjectInfoPtr->classNamesPtr[classStorage].statsUpToDate)
					{
					graphRecordPtr->graphCodesAvailable += 4;
					/*
					if (DetermineIfLOOExists (gProjectInfoPtr->currentClass))
						graphRecordPtr->graphCodesAvailable += 8;
			
					classStorage =
							gProjectInfoPtr->storageClass[gProjectInfoPtr->currentClass];
					if (gProjectInfoPtr->classNamesPtr[classStorage].modifiedStatsFlag)
						graphRecordPtr->graphCodesAvailable += 16;
					*/
					}	// end "if (...->classNamesPtr[classStorage].statsUpToDate)"
					
				}	// end "else if (statsWindowMode == kFieldListMode)"
				
			}	// end "if (gStatHistogramSpecsPtr->overlayDensityFunctionFlag)"
			
				// Set the class statistics increment variable.
				
		graphRecordPtr->classStatsIncrement = 0;
		
		if (graphRecordPtr->graphCodesAvailable & 0x0004)
			graphRecordPtr->classStatsIncrement = 2;
		
		if (graphRecordPtr->graphCodesAvailable & 0x0008)
			graphRecordPtr->classStatsIncrement = 3;
		
		if (graphRecordPtr->graphCodesAvailable & 0x0010)
			graphRecordPtr->classStatsIncrement = 5;
			
				// Get the 'Set Code'.
		
		graphRecordPtr->setCode = 1;
		if (gStatHistogramSpecsPtr->featureTransformationFlag)
			graphRecordPtr->setCode = 2;
			
		}	// end "if (graphRecordPtr != NULL)"

}	// end "SetSomeStatHistogramGraphParameters" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetStatHistogramGraphTitles
//
//	Software purpose:	This routine gets the title for the graph for the
//							biplot graphics window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			GetGraphTitle in graphicsUtilities.c
//
//	Coded By:			Larry L. Biehl			Date: 01/23/1999
//	Revised By:			Larry L. Biehl			Date: 02/03/1999			

void SetStatHistogramGraphTitles (
				GraphPtr								graphRecordPtr)
											
{
	SInt16								classIndex;
	
	
	if (graphRecordPtr != NULL)
		{
				// Create title line 1.															
								
		graphRecordPtr->title[0] = 0;
		graphRecordPtr->title[1] = 0;
		
		if (gProjectInfoPtr->histogramClassFlag)
			{
			if (gProjectInfoPtr->statsWindowMode == kFieldListMode || 
																graphRecordPtr->numberVectors == 1)
				{
				classIndex = gStatHistogramSpecsPtr->lastClassNumberUsed;
				if (gProjectInfoPtr->statsWindowMode == kFieldListMode)
					classIndex = gProjectInfoPtr->currentClass;
				
				SInt16 classStorage = gProjectInfoPtr->storageClass[classIndex];
				
				sprintf (
					(char*)&graphRecordPtr->title[1],
					"Histogram of Class '%s'",
					(unsigned char*)&gProjectInfoPtr->classNamesPtr[classStorage].name[1]);
								
				}	// end "if (gProjectInfoPtr->statsWindowMode == kFieldListMode)"
								
			else	// gProjectInfoPtr->statsWindowMode != kFieldListMode && ...
				sprintf ((char*)&graphRecordPtr->title[1],
								"Histogram of Classes");
								
			}	// end "if (gProjectInfoPtr->histogramClassFlag)"
								
		else	// !gProjectInfoPtr->histogramClassFlag
			{
			if (gProjectInfoPtr->statsWindowMode == kCoordinateListMode || 
																graphRecordPtr->numberVectors == 1)
				{
				SInt16 fieldNumber = gStatHistogramSpecsPtr->lastFieldNumberUsed;
															
				sprintf ((char*)&graphRecordPtr->title[1],
								"Histogram of Field '%s'",
								&gProjectInfoPtr->fieldIdentPtr[fieldNumber].name[1]);
								
				}	// end "if (gProjectInfoPtr->statsWindowMode == kCoordinateListMode)"
								
			else if (gProjectInfoPtr->statsWindowMode == kFieldListMode)
				{
				SInt16 classStorage = gProjectInfoPtr->storageClass[
															gProjectInfoPtr->currentClass];
				
				sprintf (
					(char*)&graphRecordPtr->title[1],
					"Histogram of Fields for '%s'",
					(unsigned char*)&gProjectInfoPtr->classNamesPtr[classStorage].name[1]);
								
				}	// end "else if (gProjectInfoPtr->statsWindowMode == kFieldListMode)"
								
			else	// gProjectInfoPtr->statsWindowMode == kClassListMode
				sprintf ((char*)&graphRecordPtr->title[1],
								"Histogram of Fields");
								
			}	// end "else !gProjectInfoPtr->histogramClassFlag"
								
		graphRecordPtr->title[0] = (UInt8)strlen ((char*)&graphRecordPtr->title[1]);
		
				// Create title line 2.	
				
		SetStatHistogramGraphTitle2 (graphRecordPtr);
		
		}	// end "if (graphRecordPtr != NULL)" 

}	// end "SetStatHistogramGraphTitles" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean SetupStatHistogramMemory
//
//	Software purpose:	The purpose of this routine is to check the 
//							memory allocated for the field and class histograms.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/02/1990
//	Revised By:			Larry L. Biehl			Date: 02/14/2006	

Boolean SetupStatHistogramMemory (
				FileInfoPtr							fileInfoPtr, 
				SInt16								statsWindowMode)

{
	Ptr									histogramBufferPtr;
	
	SInt32								numberVectors;

	UInt32								bytesNeeded,
											freeBytesPossible,
											longestContBlock,
											numberBins;
	
	SInt16								classStorage,
											index,
											outputGroupCode;
									
	Boolean								changedFlag,
											continueFlag;
										
	
	continueFlag = TRUE;
	bytesNeeded = 0;	
				
	numberBins = fileInfoPtr->numberBins;
	if (gStatHistogramSpecsPtr->featureTransformationFlag)	
		numberBins = 2048;
	
	gStatHistogramSpecsPtr->initialNumberHistogramDataBins = numberBins;
	
			// Get the number of 'columns' of histogram information.					
			// First we will need the output grouping request.							
				
	outputGroupCode = ((gStatHistogramSpecsPtr->outputFormatCode & 0x0f00) >> 8);
	
	if (gStatHistogramSpecsPtr->histogramOutputCode == kListData)
		{
		if (outputGroupCode == kChannelGrouping)
					// The channels are listed side by side for each class or field.
			gStatHistogramSpecsPtr->numberColumns = 
														gStatHistogramSpecsPtr->numberFeatures;
													
		else	// outputGroupCode == kClassFieldGrouping 
			{
					// The data for a given channel for the classes or fields are 
					// listed side by side.
					// Number of columns will be either number of classes or maximum
					// number of training fields in a class.									
		
			if (gProjectInfoPtr->histogramClassFlag)
				gStatHistogramSpecsPtr->numberColumns = 
														gProjectInfoPtr->numberStatisticsClasses;
													
			else	// !gProjectInfoPtr->histogramClassFlag 
				{
				if (statsWindowMode == kClassListMode)
					{
							// All fields for all classes are listed.
							// Loop to find the class with the most training fields.		
					
					gStatHistogramSpecsPtr->numberColumns = 0;
					for (index=0; 
							index<gProjectInfoPtr->numberStatisticsClasses;
							index++)
						{
								// Get the class storage number.									
						
						classStorage = gProjectInfoPtr->storageClass[index];
	
						gStatHistogramSpecsPtr->numberColumns = MAX (	
							gProjectInfoPtr->classNamesPtr[classStorage].numberOfTrainFields, 
							gStatHistogramSpecsPtr->numberColumns);
										
						}	// end "for (index=0; index<..." 
						
					}	// end "if (statsWindowMode == kClassListMode)" 
					
				else	// statsWindowMode != kClassListMode 
					{
							// All fields for the active class are listed.
					classStorage = 
							gProjectInfoPtr->storageClass[gProjectInfoPtr->currentClass];
					
					gStatHistogramSpecsPtr->numberColumns = 
							gProjectInfoPtr->classNamesPtr[classStorage].numberOfTrainFields;
					
					}	// end "else statsWindowMode != kClassListMode" 
								
				}	// end "else !gProjectInfoPtr->histogramClassFlag" 
				
					// Add one column for sum of other columns if there is more than
					// one column.								
			
			if (gStatHistogramSpecsPtr->numberColumns > 1)		
				gStatHistogramSpecsPtr->numberColumns++;
			
			}	// end "else outputGroupCode == kClassFieldGrouping"
	
				// Get number of bytes needed.		
				
		bytesNeeded = (UInt32)gStatHistogramSpecsPtr->numberColumns * 
																numberBins * sizeof (UInt32) +
								(UInt32)gStatHistogramSpecsPtr->numberColumns *
																			sizeof (HistogramSummary);
	
				// Change size of handle for histogram information if needed.		
			
		histogramBufferPtr = CheckHandleSize (
													&gStatHistogramSpecsPtr->histogramDataHandle,
													&continueFlag, 
													&changedFlag, 
													bytesNeeded);
		
				// If memory is not full, get pointers to the data buffer.					
				
		if (continueFlag)
			{
			gStatHistogramSpecsPtr->histogramDataPtr = 
				(HUInt32Ptr)&histogramBufferPtr[
				(SInt32)gStatHistogramSpecsPtr->numberColumns*sizeof (HistogramSummary)];
				
			gStatHistogramSpecsPtr->histogramRangePtr = 
															(HistogramSummaryPtr)histogramBufferPtr;
			
			}	// end "if (continueFlag)" 	
			
		}	// end "if (gStatHistogramSpecsPtr->histogramOutputCode == kListData)" 
		
	if (gStatHistogramSpecsPtr->histogramOutputCode == kPlotData)
		{
		gStatHistogramSpecsPtr->numberColumns =  
													gStatHistogramSpecsPtr->numberFeatures;
			
				// Get number of bytes needed to load histogram buffer for one set
				// of all requested channels of data.	
				
		bytesNeeded = (SInt32)gStatHistogramSpecsPtr->numberFeatures * 
																numberBins * sizeof (UInt32) +
							(SInt32)gStatHistogramSpecsPtr->numberFeatures *
																			sizeof (HistogramSummary);
																			
				// Change size of handle for histogram information if needed.		
			
		histogramBufferPtr = CheckHandleSize (
													&gStatHistogramSpecsPtr->histogramDataHandle,
													&continueFlag, 
													&changedFlag, 
													bytesNeeded);
		
				// If memory is not full, get pointers to the data buffer. This may be
				// needed to count the number of data values to be used.
				
		if (continueFlag)
			{
			gStatHistogramSpecsPtr->histogramDataPtr = 
				(HUInt32Ptr)&histogramBufferPtr[
				(SInt32)gStatHistogramSpecsPtr->numberFeatures*sizeof (HistogramSummary)];
				
			gStatHistogramSpecsPtr->histogramRangePtr = 
															(HistogramSummaryPtr)histogramBufferPtr;
			
			}	// end "if (continueFlag)" 	
		
				// If memory is not full, get pointers to the data buffer.					
				
		if (continueFlag)
			{			
					// Now check on memory for the graph vectors.
							
			numberVectors = 1;		
			if (gProjectInfoPtr->histogramClassFlag)
				{
				if (statsWindowMode == kClassListMode)
					numberVectors = gProjectInfoPtr->numberStatisticsClasses;
																
				else if (statsWindowMode == kFieldListMode)
					numberVectors = 1;
				
				}	// end "if (gProjectInfoPtr->histogramClassFlag)"
				
			else	// !gProjectInfoPtr->histogramClassFlag
				{
				if (statsWindowMode == kClassListMode)
					numberVectors = gProjectInfoPtr->numberStatTrainFields;
																
				else if (statsWindowMode == kFieldListMode)
					{
							// All fields for the active classare listed.
							
					classStorage = 
							gProjectInfoPtr->storageClass[gProjectInfoPtr->currentClass];
					
					numberVectors = 
							gProjectInfoPtr->classNamesPtr[classStorage].numberOfTrainFields;
							
					}	// end "else if (statsWindowMode == kFieldListMode)"
																
				else	// statsWindowMode == kCoordinateListMode
					numberVectors = 1;
				
				}	// end "else statsWindowMode == kCoordinateListMode"
				
			gStatHistogramSpecsPtr->numberVectors = (SInt16)numberVectors;
			
					// Get the default size of the x and y vector needed for the
					// worst case when something exists for every bin. The memory
					// will actually be aquired later when the graph vectors are
					// set up.
					
			gGraphRecordPtr->xVector.size = 
						numberVectors * gStatHistogramSpecsPtr->numberFeatures * numberBins;
		
					// Get number of bytes needed for the worst case.			
					
			bytesNeeded = 2 * gGraphRecordPtr->xVector.size * sizeof (GRAPHDATA);
			
					// Check if this amount of memory is available for the worst case.
			
			freeBytesPossible = MGetFreeMemory (&longestContBlock);
			
			if (bytesNeeded+40000 > longestContBlock)
				{
						// Memory may not be available. Setting xVector.size to zero will
						// force the data to be read first for each vector and then then
						// memory for the xVector and yVector will be obtained.

				gGraphRecordPtr->xVector.size = 0;
				
				}	// end "else bytesNeeded+40000 > longestContBlock"
				
			}	// end "if (continueFlag)"
				
				// Memory for the graph will be accessed later.				
				
		if (!continueFlag)
			{
					// Make sure that these pointers are NULL.
					
			gStatHistogramSpecsPtr->histogramDataPtr = NULL;
			gStatHistogramSpecsPtr->histogramRangePtr = NULL;
			
			}	// end "if (!continueFlag)"
		
		}	// end "if (gStatHistogramSpecsPtr->histogramOutputCode == kPlotData)"
	
	if (continueFlag && (fileInfoPtr->numberBytes >= 4 ||
											gStatHistogramSpecsPtr->featureTransformationFlag))
		{	
				// Get memory needed to determine the minimum and maximum data values
				// for case when the number of bytes of data is 4 or more.			
					
		bytesNeeded = (UInt32)gProjectInfoPtr->numberStatisticsChannels * 
																			sizeof (ChannelStatistics);
		gStatHistogramSpecsPtr->channelStatsPtr = 
											(HChannelStatisticsPtr)MNewPointer (bytesNeeded);
		
		bytesNeeded = (UInt32)gProjectInfoPtr->numberStatisticsChannels * 
																						sizeof (double);
		
		if (gStatHistogramSpecsPtr->channelStatsPtr != NULL)
			gStatHistogramSpecsPtr->channelMinPtr = (double*)MNewPointer (bytesNeeded);
		
		if (gStatHistogramSpecsPtr->channelMinPtr != NULL)
			gStatHistogramSpecsPtr->channelMaxPtr = (double*)MNewPointer (bytesNeeded);
			
		if (gStatHistogramSpecsPtr->channelMaxPtr == NULL)
			continueFlag = FALSE;
		
		}	// end "if (continueFlag && (fileInfoPtr->numberBytes >= 4 || ...)"
		
	return (!continueFlag);
		
}	// end "SetupStatHistogramMemory" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean StatHistogramDialog
//
//	Software purpose:	The purpose of this routine is to obtain the
//							user specifications for the histograms of the training
//							and/or test fields for the project.
//
//	Parameters in:		fileInfoPtr
//
//	Parameters out:	None
//
// Value Returned:  	True if user selects OK in dialog box
//							False if user selects Cancel in dialog box
//
// Called By:			HistogramStatsControl
//
//	Coded By:			Larry L. Biehl			Date: 04/09/1990
//	Revised By:			Larry L. Biehl			Date: 10/22/2018
	
Boolean StatHistogramDialog (
				FileInfoPtr							fileInfoPtr)
	
{
	Boolean								returnFlag;
	
#if defined multispec_mac
	Rect									theBox,
											theChannelPopupBox;
	
	DialogPtr							dialogPtr;
	
	UInt16								*localActiveFeaturesPtr = NULL,
											*localFeaturesPtr = NULL,
											*localTransformFeaturesPtr = NULL;
	/*
	#ifdef powerc
		RoutineDescriptor					drawChannelsPopUp2RD = 
															BUILD_ROUTINE_DESCRIPTOR (
																	uppUserItemProcInfo, 
																	DrawChannelsPopUp2);
	#endif
	*/
	UserItemUPP							drawChannelsPopUp2Ptr;
										
	Handle								okHandle,
											theHandle;
	
	SInt32								columnInterval,
											lineInterval,
											theNum;
	
	SInt16								columnMatrixCode,
											itemHit,
											groupCode,
											outputCode,
											stringNumber,
											theType;
								
	UInt16								localActiveNumberFeatures,
											localNumberFeatures,
											localNumberTransformFeatures;
	
	Boolean								blankValuesFlag,
											checkFeatureTransformFlag,
											featureTransformAllowedFlag,
											featureTransformationFlag,
											histogramClassFlag,
											includeEmptyBinsFlag,
											listFlag,
											modalDone,
											overlayDensityFunctionFlag,
											overlayDFAllowedFlag;
	
	
			// Initialize local variables.													
			
	dialogPtr = NULL;
	
	returnFlag = GetDialogLocalVectors (&localFeaturesPtr,
													&localTransformFeaturesPtr,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL);

	
			// Get the modal dialog for the histogram specification.					
	
	if (returnFlag)			
		dialogPtr = LoadRequestedDialog (kStatHistogramSpecificationID, kCopyScrap, 1, 2);
		
	if (dialogPtr == NULL)
		{			
		ReleaseDialogLocalVectors (localFeaturesPtr,
											localTransformFeaturesPtr,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);										
																							return (FALSE);
																					
		}	// end "if (dialogPtr == NULL)"
		
			// Intialize local user item proc pointers.									
	/*
	#ifdef powerc
		drawChannelsPopUp2Ptr = &drawChannelsPopUp2RD;
	#else
		drawChannelsPopUp2Ptr = DrawChannelsPopUp2;
	#endif
	*/
	drawChannelsPopUp2Ptr = NewUserItemUPP (DrawChannelsPopUp2);
	
			// Set Procedure pointers for those dialog items that need them.
			
	SetDialogItemDrawRoutine (dialogPtr, 12, &theChannelPopupBox, drawChannelsPopUp2Ptr);

			// Initialize dialog variables.

	StatHistogramDialogInitialize (dialogPtr,
												fileInfoPtr,
												gProjectInfoPtr->statsWindowMode,
												localFeaturesPtr, 
												localTransformFeaturesPtr,
												&histogramClassFlag,
												&lineInterval,
												&columnInterval,
												&featureTransformAllowedFlag,
												&featureTransformationFlag,
												&gChannelSelection,
												&localActiveNumberFeatures,
												&outputCode,
												&includeEmptyBinsFlag,
												&blankValuesFlag,
												&groupCode,
												&columnMatrixCode,
												&overlayDFAllowedFlag,
												&overlayDensityFunctionFlag);
	
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
	
			// Save handle for the OK button for use later.								
			
	GetDialogItem (dialogPtr, 1,  &theType, &okHandle, &theBox);
	
	listFlag = (outputCode == kListData);
	
			// Set radio buttons for "Classes" and "Fields".												
			
	if (gProjectInfoPtr->statsWindowMode != kCoordinateListMode)
		SetDLogControl (dialogPtr, 4, (SInt16)histogramClassFlag);				
	
	SetDLogControl (dialogPtr, 5, (SInt16)!histogramClassFlag);
	
			// Default value for line and column interval.												
			
	LoadDItemValue (dialogPtr,  9, lineInterval);
	LoadDItemValue (dialogPtr, 10, columnInterval);					
	
	SetDLogControl (dialogPtr, 14, (SInt16)(outputCode == kListData));
	SetDLogControl (dialogPtr, 17, (SInt16)(outputCode == kPlotData));
	
	SetDLogControl (dialogPtr, 15, (SInt16)includeEmptyBinsFlag);
	SetDLogControl (dialogPtr, 16, (SInt16)blankValuesFlag);
			
	SetDLogControl (dialogPtr, 19, (SInt16)(groupCode == 1));
	SetDLogControl (dialogPtr, 20, (SInt16)(groupCode == 2));
	
	if (listFlag)
		{		
		SetDLogControl (dialogPtr, 22, (SInt16)(columnMatrixCode == 1));
		SetDLogControl (dialogPtr, 23, (SInt16)(columnMatrixCode == 2));
		
		}	// end "if (listFlag)"
		
	else	// !listFlag
		{
		SetDLogControl (dialogPtr, 22, 0);
		SetDLogControl (dialogPtr, 23, 0);
		
		}	// end "else !listFlag"
		
	SetDLogControl (dialogPtr, 25, (SInt16)featureTransformationFlag);
		
	SetDLogControl (dialogPtr, 26, (SInt16)overlayDensityFunctionFlag);
	
			// Center the dialog and then show it.											
			
	ShowDialogWindow (dialogPtr, kStatHistogramSpecificationID, kSetUpDFilterTable);
	
			// Set default text selection to first edit text item						
			
	SelectDialogItemText (dialogPtr, 9, 0, INT16_MAX);
	
	checkFeatureTransformFlag = FALSE;
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
					SetControlValue ((ControlHandle)theHandle, 1);
					SetDLogControl (dialogPtr, 5, 0);
					histogramClassFlag = TRUE;
					
					StatHistogramDialogClassesParameters (dialogPtr, 
																		listFlag, 
																		overlayDFAllowedFlag,
																		overlayDensityFunctionFlag);
					break;
					
				case 5:
					SetControlValue ((ControlHandle)theHandle, 1);
					SetDLogControl (dialogPtr, 4, 0);
					histogramClassFlag = FALSE;
					
					StatHistogramDialogFieldsParameters (dialogPtr);
					break;
					
				case 6:
					break;
	
				case 9:		//	 lineInterval
					lineInterval = CheckIntervalValue (theNum,
																	lineInterval, 
																	1,  
																	gImageWindowInfoPtr->maxNumberLines, 
																	dialogPtr, 
																	itemHit,
																	theHandle); 
					break;
					
				case 10:		//	 columnInterval
					columnInterval = CheckIntervalValue (
																theNum,
																columnInterval, 
																1,  
																gImageWindowInfoPtr->maxNumberColumns, 
																dialogPtr, 
																itemHit,
																theHandle);
					break;
					
				case 12:		// Channels 
					itemHit = StandardPopUpMenu (dialogPtr, 
															11, 
															12, 
															gPopUpAllAvailableSubsetMenu, 
															gChannelSelection, 
															kPopUpAllAvailableSubsetMenuID);
													
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
						
						}	// end "if (itemHit != 0)"
					break;
					
				case 14:		// radio button for "list histogram"
					listFlag = TRUE; 
					SetControlValue ((ControlHandle)theHandle, 1);
					SetDLogControl (dialogPtr, 17, 0);
					
					StatHistogramDialogListParameters (
												dialogPtr, histogramClassFlag, columnMatrixCode);
					break;
					
				case 15:		// check button for "include empty bins." 
				case 16:		// check button for "blank values outside of range." 
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					break;
					
				case 17:		// Plot the histogram
					listFlag = FALSE; 
					SetControlValue ((ControlHandle)theHandle, 1);
					SetDLogControl (dialogPtr, 14, 0);
					
					StatHistogramDialogPlotParameters (dialogPtr, 
																	histogramClassFlag,
																	overlayDensityFunctionFlag);
					break;
					
				case 19:		// radio button for "group channels" 
					SetControlValue ((ControlHandle)theHandle, 1);
					SetDLogControl (dialogPtr, 20, 0);
					break;
					
				case 20:		// radio button for "group fields/classes" 
					SetControlValue ((ControlHandle)theHandle, 1);
					SetDLogControl (dialogPtr, 19, 0);
					break;
					
				case 22:		// radio button for "matrix format" 
					SetControlValue ((ControlHandle)theHandle, 1);
					SetDLogControl (dialogPtr, 23, 0);
					columnMatrixCode = 1;
					break;
					
				case 23:		// radio button for "columnar list format" 
					SetControlValue ((ControlHandle)theHandle, 1);
					SetDLogControl (dialogPtr, 22, 0);
					columnMatrixCode = 2;
					break;
					
				case 25:		// check button for "use feature transformation." 
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					featureTransformationFlag = !featureTransformationFlag;
					checkFeatureTransformFlag = TRUE;
					break;
					
				case 26:		// check button for "overlay density function graph." 
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					overlayDensityFunctionFlag = !overlayDensityFunctionFlag;
					break;
						
				}	// end "switch (itemHit)" 
				
			if (checkFeatureTransformFlag)
				{
				checkFeatureTransformFlag = CheckFeatureTransformationDialog (
																			dialogPtr,
																			featureTransformAllowedFlag,
																			25, 
																			11, 
																			&featureTransformationFlag); 

				gChannelSelection = UpdateDialogFeatureParameters (
															featureTransformationFlag, 
															&localActiveNumberFeatures,
															&localActiveFeaturesPtr, 
															localNumberFeatures,
															localFeaturesPtr,
															gProjectInfoPtr->numberStatisticsChannels, 
															localNumberTransformFeatures,
															localTransformFeaturesPtr,
															gTransformationMatrix.numberFeatures);
						
				stringNumber = IDS_Dialog23;
				if (featureTransformationFlag)
					stringNumber = IDS_Dialog22;
					
				SetDLogControlTitleNumber (kDialogStrID, 
													stringNumber,
													dialogPtr, 
													19, 
													(Str255*)&gTextString);
				
						// Force the channel popup box to be updated.
						
				InvalWindowRect (GetDialogWindow (dialogPtr), &theChannelPopupBox);
															
				}	// end "if (checkFeatureTransformFlag)"
				
			}	// end "if (itemHit > 2)" 
			
		else	// itemHit <= 2 
			{	
			if (itemHit == 1)
				{
				modalDone = TRUE;

				StatHistogramDialogOK (GetDLogControl (dialogPtr, 4),	// histogramClassFlag
												lineInterval,
												columnInterval,
												featureTransformationFlag,
												gChannelSelection,
												localActiveNumberFeatures,
												localActiveFeaturesPtr,
												GetDLogControl (dialogPtr, 14),	// listDataFlag
												GetDLogControl (dialogPtr, 15),	// includeEmptyBinsFlag
												GetDLogControl (dialogPtr, 16),	// blankValuesFlag
												GetDLogControl (dialogPtr, 17),	// plot histogram flag
												GetDLogControl (dialogPtr, 19),	// group channels flag
												GetDLogControl (dialogPtr, 20),	// group fields classes
												columnMatrixCode,
												GetDLogControl (dialogPtr, 26));	// overlayDensityFunctionFlag
			
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
		
	ReleaseDialogLocalVectors (localFeaturesPtr,
											localTransformFeaturesPtr,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);
		
	CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
#endif // defined multispec_mac
	
	#if defined multispec_win 
		CMStatHistogramSpecsDlg*		dialogPtr = NULL;
		
		TRY
			{ 
			dialogPtr = new CMStatHistogramSpecsDlg (); 
			                  
			returnFlag = dialogPtr->DoDialog (gStatHistogramSpecsPtr,
															fileInfoPtr);
		                       
			delete dialogPtr;
			}
			
		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			}
		END_CATCH_ALL 
	#endif // defined multispec_win 

   #if defined multispec_lin
      CMStatHistogramSpecsDlg*		dialogPtr = NULL;
      
      //dialogPtr = new CMStatHistogramSpecsDlg ((wxWindow*)GetMainFrame ());
      dialogPtr = new CMStatHistogramSpecsDlg (NULL);
			                  
	   returnFlag = dialogPtr->DoDialog (gStatHistogramSpecsPtr, fileInfoPtr);
		                       
      delete dialogPtr;
   #endif
	
	return (returnFlag);

}	// end "StatHistogramDialog"

 

void StatHistogramDialogClassesParameters (
				DialogPtr							dialogPtr,
				Boolean								listFlag,
				Boolean								overlayDFAllowedFlag,
				Boolean								overlayDensityFunctionFlag)
				
{
	if (listFlag && gProjectInfoPtr->statsWindowMode == kFieldListMode)
		{
		SetDLogControl (dialogPtr, IDC_ChannelsRadio, 1);
		SetDLogControl (dialogPtr, IDC_Classes, 0);
		SetDLogControlHilite (dialogPtr, IDC_Classes, 255);
		
		}	// end "if (listFlag && ...->statsWindowMode == kFieldListMode)" 
		
	SetDLogControlTitleNumber (kDialogStrID, 
											IDS_Dialog11,		// Classes
											dialogPtr, 
											IDC_Classes, 
											(Str255*)&gTextString);
	
	#if !defined multispec_lin
		if (!listFlag && overlayDFAllowedFlag)
			{							
			SetDLogControlHilite (dialogPtr, IDC_OverlayCheck, 0);
			SetDLogControl (
						dialogPtr, IDC_OverlayCheck, (SInt16)overlayDensityFunctionFlag);
			
			}	// end "if (!listFlag && overlayDFAllowedFlag)"
	#endif
						
}	// end "if (StatHistogramDialogClassesParameters)"

 

void StatHistogramDialogFieldsParameters (
				DialogPtr							dialogPtr)
				
{				
	if (gProjectInfoPtr->statsWindowMode == kFieldListMode)
		SetDLogControlHilite (dialogPtr, IDC_Classes, 0);
		
	SetDLogControlTitleNumber (kDialogStrID, 
											IDS_Dialog21,		// Fields
											dialogPtr, 
											IDC_Classes, 
											(Str255*)&gTextString);
											
	SetDLogControlHilite (dialogPtr, IDC_OverlayCheck, 255);
	SetDLogControl (dialogPtr, IDC_OverlayCheck, 0);
						
}	// end "StatHistogramDialogFieldsParameters"

 

void StatHistogramDialogListParameters (
				DialogPtr							dialogPtr,
				Boolean								histogramClassFlag,
				SInt16								columnMatrixCode)
				
{				
	SetDLogControl (dialogPtr, IDC_IncludeEmptyCheck, 
					gStatHistogramSpecsPtr->outputFormatCode & 0x0001);
	SetDLogControlHilite (dialogPtr, IDC_IncludeEmptyCheck, 0);
	SetDLogControl (dialogPtr, IDC_BlankCheck, 
					gStatHistogramSpecsPtr->outputFormatCode & 0x0002);
	SetDLogControlHilite (dialogPtr, IDC_BlankCheck, 0);
	
	SetDLogControlHilite (dialogPtr, IDC_ChannelsRadio, 0);

	if (gProjectInfoPtr->statsWindowMode == kCoordinateListMode || 
			(gProjectInfoPtr->statsWindowMode == kFieldListMode && 
				histogramClassFlag))
		{
		SetDLogControl (dialogPtr, IDC_ChannelsRadio, 1);
		
		SetDLogControl (dialogPtr, IDC_Classes, 0);
		SetDLogControlHilite (dialogPtr, IDC_Classes, 255);
		
		}	// end "if (...->statsWindowMode == kCoordinateListMode || ..."

	SetDLogControl (dialogPtr, IDC_MatrixRadio, (columnMatrixCode==1));
	SetDLogControlHilite (dialogPtr, IDC_MatrixRadio, 0);
	
	SetDLogControl (dialogPtr, IDC_ColumnRadio, (columnMatrixCode==2));
	SetDLogControlHilite (dialogPtr, IDC_ColumnRadio, 0);
	
	SetDLogControl (dialogPtr, IDC_OverlayCheck, 0);
	SetDLogControlHilite (dialogPtr, IDC_OverlayCheck, 255);
						
}	// end "StatHistogramDialogListParameters"
 

void StatHistogramDialogPlotParameters (
				DialogPtr							dialogPtr,
				Boolean								histogramClassFlag,
				Boolean								overlayDensityFunctionFlag)
				
{					
	SetDLogControl (dialogPtr, IDC_IncludeEmptyCheck, 0);
	SetDLogControlHilite (dialogPtr, IDC_IncludeEmptyCheck, 255);
	
	SetDLogControl (dialogPtr, IDC_BlankCheck, 0);
	SetDLogControlHilite (dialogPtr, IDC_BlankCheck, 255);
	
	SetDLogControl (dialogPtr, IDC_ChannelsRadio, 0);
	SetDLogControlHilite (dialogPtr, IDC_ChannelsRadio, 255);
	
	SetDLogControl (dialogPtr, IDC_Classes, 1);
	SetDLogControlHilite (dialogPtr, IDC_Classes, 0);

	SetDLogControl (dialogPtr, IDC_MatrixRadio, 0);
	SetDLogControlHilite (dialogPtr, IDC_MatrixRadio, 255);
	
	SetDLogControl (dialogPtr, IDC_ColumnRadio, 0);
	SetDLogControlHilite (dialogPtr, IDC_ColumnRadio, 255);
	/*
	if (histogramClassFlag)
		{
		SetDLogControl (dialogPtr, IDC_OverlayCheck, (SInt16)overlayDensityFunctionFlag);
		SetDLogControlHilite (dialogPtr, IDC_OverlayCheck, 0);
		
		}	// end "if (histogramClassFlag)"
	*/
	#if defined multispec_lin
		if (histogramClassFlag){
			SetDLogControl (
							dialogPtr, IDC_OverlayCheck, (SInt16)overlayDensityFunctionFlag);
			SetDLogControlHilite (dialogPtr, IDC_OverlayCheck, 0);
			}	// end "if (histogramClassFlag)"
	#else
		if (histogramClassFlag){
			SetDLogControl (
							dialogPtr, IDC_OverlayCheck, (SInt16)overlayDensityFunctionFlag);
			SetDLogControlHilite (dialogPtr, IDC_OverlayCheck, 0);
			}	// end "if (histogramClassFlag)"
	#endif
						
}	// end "StatHistogramDialogPLotParameters"



void StatHistogramDialogInitialize (
				DialogPtr							dialogPtr,
				FileInfoPtr							fileInfoPtr,
				SInt16								statsWindowMode,
				UInt16*								localFeaturesPtr, 
				UInt16*								localTransformFeaturesPtr,
				Boolean*								histogramClassFlagPtr,
				SInt32*								lineIntervalPtr,
				SInt32*								columnIntervalPtr,
				Boolean*								featureTransformAllowedFlagPtr,
				Boolean*								featureTransformationFlagPtr,
				SInt16*								channelSelectionPtr,
				UInt16*								localNumberChannelsPtr,
				SInt16*								histogramOutputCodePtr,
				Boolean*								includeEmptyBinsFlagPtr,
				Boolean*								blankValuesFlagPtr,
				SInt16*								groupCodePtr,
				SInt16*								columnMatrixCodePtr,
				Boolean*								overlayDFAllowedFlagPtr,
				Boolean*								overlayDensityFunctionFlagPtr)
	
{
	SInt16								classStorage,
											eigenSource,
											stringNumber;
											
	UInt16								numberEigenvectors;
											
	
			// Load the dialog local vectors

	LoadDialogLocalVectors (localFeaturesPtr,
									localTransformFeaturesPtr,
									gStatHistogramSpecsPtr->featureHandle,
									gStatHistogramSpecsPtr->numberChannels,
									NULL,
									NULL,
									0,
									NULL,
									NULL,
									NULL,
									NULL,
									NULL,
									0,
									NULL,
									NULL,
									0,
									NULL);
	
			// Class or Field flag.												
	
	*histogramClassFlagPtr = gProjectInfoPtr->histogramClassFlag;
		
	if (statsWindowMode == kCoordinateListMode)
		SetDLogControlHilite (dialogPtr, IDC_ClassesRadio, 255);
		
			// Set title of this control to be 'Class'. The default is Classes.
			
	if (statsWindowMode >= kFieldListMode || 
												gProjectInfoPtr->numberStatTrainClasses <= 1)
		SetDLogControlTitleNumber (kDialogStrID, 
											IDS_Dialog20,		// Class
											dialogPtr, 
											IDC_ClassesRadio, 
											(Str255*)&gTextString);
	
			// Set title of this control to be 'Field'. The default is Fields.
			
	if (statsWindowMode == kCoordinateListMode)
		SetDLogControlTitleNumber (kDialogStrID, 
											IDS_Dialog19,		// Field
											dialogPtr, 
											IDC_FieldsRadio, 
											(Str255*)&gTextString);
	
			// Default value for line interval.
			
	*lineIntervalPtr = gStatHistogramSpecsPtr->lineInterval;
	*columnIntervalPtr = gStatHistogramSpecsPtr->columnInterval;
		
			// Set the check box for the feature transformation option.				
			
	EigenvectorInfoExists (kProject, &eigenSource, &numberEigenvectors);
	
	if (numberEigenvectors <= 0)
		{		
		*featureTransformAllowedFlagPtr = FALSE;
		*featureTransformationFlagPtr = FALSE;
		HideDialogItem (dialogPtr, IDC_UseFeatureTransformation);
		
		}	// end "else numberEigenvectors <= 0" 
		
	else	// numberEigenvectors > 0 
		{
		*featureTransformAllowedFlagPtr = TRUE;
		*featureTransformationFlagPtr = gStatHistogramSpecsPtr->featureTransformationFlag;
		
		}	// end "else numberEigenvectors > 0"
	
	if (*featureTransformationFlagPtr)
		{
		LoadDItemStringNumber (kDialogStrID, 
										IDS_Dialog16,		// Features:
										dialogPtr, 
										IDC_ChannelsPrompt, 
										(Str255*)gTextString);
					
		SetDLogControlTitleNumber (kDialogStrID, 
											IDS_Dialog22,		// Features:
											dialogPtr, 
											IDC_ChannelsRadio, 
											(Str255*)&gTextString);
										
		}	// end "if (*featureTransformationFlagPtr)"
																								
			//	Make all channels the default													
	
	*channelSelectionPtr = gStatHistogramSpecsPtr->channelSet;
	*localNumberChannelsPtr = gStatHistogramSpecsPtr->numberFeatures;
	
			// Set check box for "list histogram".											
			// Set check box for "include empty bins".									
			// Set check box for "blank values outside of range".
			
	*histogramOutputCodePtr = gStatHistogramSpecsPtr->histogramOutputCode;
	
			// Get the flag indicating whether the density function overlay is allowed.
			// Note that if 4-byte or more data, the density function will be allowed
			// since the statistics have to be computed.
			
	*overlayDFAllowedFlagPtr = TRUE;
	
	switch (gProjectInfoPtr->statsWindowMode)
		{
		case kClassListMode:
			if (!gProjectInfoPtr->statsUpToDate)
				*overlayDFAllowedFlagPtr = FALSE;
			break;
			
		case kFieldListMode:
			classStorage = gProjectInfoPtr->storageClass[gProjectInfoPtr->currentClass];
			if (!gProjectInfoPtr->classNamesPtr[classStorage].statsUpToDate)
				*overlayDFAllowedFlagPtr = FALSE;
			break;
			
		case kCoordinateListMode:
			*overlayDFAllowedFlagPtr = FALSE;
			break;
			
		}	// end "switch (gProjectInfoPtr->statsWindowMode)"
		
	if (fileInfoPtr->numberBytes >= 4)
		*overlayDFAllowedFlagPtr = TRUE;
		
			// If all fields are defined by cluster statistics then unhighlight the
			// plot option and force only the ListData option.
			
	if (DetermineFieldTypes () == 4)
		{
		gStatHistogramSpecsPtr->histogramOutputCode = kListData;
		SetDLogControlHilite (dialogPtr, IDC_PlotRadio, 255);
		
		}	// end "if (DetermineFieldTypes () == 4)"
			
							
	if (*histogramOutputCodePtr == kListData)
		{
				// Include empty bins check box.
				
		*includeEmptyBinsFlagPtr = FALSE;
		if (gStatHistogramSpecsPtr->outputFormatCode & 0x0001)
			*includeEmptyBinsFlagPtr = TRUE;
		SetDLogControlHilite (dialogPtr, IDC_IncludeEmptyCheck, 0);
		
				// Blank values check box.
				
		*blankValuesFlagPtr = FALSE;
		if (gStatHistogramSpecsPtr->outputFormatCode & 0x0002)
			*blankValuesFlagPtr = TRUE;
		SetDLogControlHilite (dialogPtr, IDC_BlankCheck, 0);	
					
				// Group by channels or fields/classes.																				
		
		*groupCodePtr = (gStatHistogramSpecsPtr->outputFormatCode & 0x0f00);
		*groupCodePtr = (*groupCodePtr >> 8);
				
		if (gProjectInfoPtr->statsWindowMode == kCoordinateListMode || 
				(gProjectInfoPtr->statsWindowMode == kFieldListMode && 
					*histogramClassFlagPtr))
			SetDLogControlHilite (dialogPtr, IDC_Classes, 255);
		else	// gProjectInfoPtr->statsWindowMode != kCoordinateListMode && ...
			SetDLogControlHilite (dialogPtr, IDC_Classes, 0);
		
		SetDLogControlHilite (dialogPtr, IDC_OverlayCheck, 255);
		
		*overlayDensityFunctionFlagPtr = FALSE;
			
		}	// end "if (*histogramOutputCodePtr == kListData)" 
		
			// Plot the data.	
	
	if (*histogramOutputCodePtr == kPlotData)
		{
		*includeEmptyBinsFlagPtr = FALSE;
		SetDLogControlHilite (dialogPtr, IDC_IncludeEmptyCheck, 255);
		
		*blankValuesFlagPtr = FALSE;
		SetDLogControlHilite (dialogPtr, IDC_BlankCheck, 255);
		
		*groupCodePtr = 2;
		SetDLogControlHilite (dialogPtr, IDC_ChannelsRadio, 255);
		
		SetDLogControlHilite (dialogPtr, IDC_MatrixRadio, 255);
		SetDLogControlHilite (dialogPtr, IDC_ColumnRadio, 255);
		
		SetDLogControlHilite (dialogPtr, IDC_OverlayCheck, 0);
		
		*overlayDensityFunctionFlagPtr =
											gStatHistogramSpecsPtr->overlayDensityFunctionFlag;
		if (!*overlayDFAllowedFlagPtr)
			*overlayDensityFunctionFlagPtr = FALSE;
			
		else if (!*histogramClassFlagPtr)
			*overlayDensityFunctionFlagPtr = FALSE;
		
		if (!*histogramClassFlagPtr)
			SetDLogControlHilite (dialogPtr, IDC_OverlayCheck, 255);
		
		}	// end "if (*histogramOutputCodePtr == kPlotData)"
		
	if (!*overlayDFAllowedFlagPtr)
		HideDialogItem (dialogPtr, IDC_OverlayCheck);
					
			// Column or matrix list.	
	
	*columnMatrixCodePtr = (gStatHistogramSpecsPtr->outputFormatCode & 0x00f0);
	*columnMatrixCodePtr = (*columnMatrixCodePtr >> 4);
	
	stringNumber = IDS_Dialog21;
	if (gProjectInfoPtr->histogramClassFlag)
		stringNumber = IDS_Dialog11;	
		
	SetDLogControlTitleNumber (kDialogStrID, 
										stringNumber,		// Class or Field
										dialogPtr, 
										IDC_Classes, 
										(Str255*)&gTextString);

}	// end "StatHistogramDialogInitialize" 


												
void StatHistogramDialogOK (
				Boolean								histogramClassFlag,
				SInt32								lineInterval,
				SInt32								columnInterval,
				Boolean								featureTransformationFlag,
				SInt16								channelSelection,
				UInt16								localNumberFeatures,
				UInt16*								localFeaturesPtr,
				Boolean								listDataFlag,
				Boolean								includeEmptyBinsFlag,
				Boolean								blankValuesFlag,
				Boolean								plotHistogramFlag,
				Boolean								groupChannelsFlag,
				Boolean								groupFieldsClassesFlag,
				SInt16								columnMatrixCode,
				Boolean								overlayDensityFunctionFlag)
	
{
				
			//  Histogram classes or fields.						

	gProjectInfoPtr->histogramClassFlag = histogramClassFlag;
					
	gProjectInfoPtr->histogramFieldFlag = !gProjectInfoPtr->histogramClassFlag;

			// lineInterval.												
			
	gStatHistogramSpecsPtr->lineInterval = lineInterval;

			// column Interval.										
			
	gStatHistogramSpecsPtr->columnInterval = columnInterval;
											
			// Load some common processor parameters
			// Channels
			
	LoadProcessorVectorsFromDialogLocalVectors (
											channelSelection,
											featureTransformationFlag,
											gProjectInfoPtr->numberStatisticsChannels,
											localNumberFeatures,
											(UInt16*)localFeaturesPtr,
											&gStatHistogramSpecsPtr->channelSet,
											(UInt16*)&gStatHistogramSpecsPtr->numberFeatures,
											gStatHistogramSpecsPtr->featureHandle,
											(UInt16*)&gStatHistogramSpecsPtr->numberChannels,
											gStatHistogramSpecsPtr->channelsHandle,
											0,
											0,
											NULL,
											NULL,
											NULL,
											NULL,
											0,
											NULL,
											NULL,
											NULL,
											0,
											NULL,
											NULL);
	
			//  List Histogram.											
	
	if (listDataFlag)			
		gStatHistogramSpecsPtr->histogramOutputCode = kListData;
										
			// Include Empty Bins in the list.						
	
	gStatHistogramSpecsPtr->outputFormatCode = 0;	
	if (includeEmptyBinsFlag)
		gStatHistogramSpecsPtr->outputFormatCode += 1;
										
			// Blank values outside of range.						
						
	if (blankValuesFlag)
		gStatHistogramSpecsPtr->outputFormatCode += 2;
										
			// Plot Histogram.											
	
	if (plotHistogramFlag)			
		gStatHistogramSpecsPtr->histogramOutputCode = kPlotData;
										
			// Group channels.											
	
	if (groupChannelsFlag)
		gStatHistogramSpecsPtr->outputFormatCode += 256;
										
			// Group fields classes.									
	
	if (groupFieldsClassesFlag)
		gStatHistogramSpecsPtr->outputFormatCode += 512;
										
			// Matrix format.												
	
	if (columnMatrixCode == 1)
		gStatHistogramSpecsPtr->outputFormatCode += 16;
										
			// Columnar format.											
	
	if (columnMatrixCode == 2)
		gStatHistogramSpecsPtr->outputFormatCode += 32;								

	gStatHistogramSpecsPtr->featureTransformationFlag = featureTransformationFlag;

	gStatHistogramSpecsPtr->overlayDensityFunctionFlag = overlayDensityFunctionFlag;

}	// end "StatHistogramDialogOK" 



//	(C) Copyright 1986. THINK Technologies, Inc.  All rights reserved. 	

UInt32 stc_d (
				char*									out, 
				UInt32						 		in, 
				SInt16			 					outlen, 
				SInt16 								signflag)
			
{ 
	char 									revnum[8];
	char									*out0 = out;
	SInt16				 				i=0;
	

	if (outlen<=0) 
																								return (0);
	
	if (in == 0) 
		revnum[i++]=0;
		
	else
		while (in)
			{
			revnum[i++] = (char)(in - (in/10)*10);
			in /= 10;
			
			}	// end "while (in)"
	
	if (signflag)
		{
		*out++ = '-';
		outlen--;
		
		}	// end "if (signflag)"
	
	for (; i && outlen; i--, outlen--)
		*out++ = revnum[i-1] + '0';
	
	*out = kNullTerminator;
	
	return ((UInt32)(out-out0));
	
}

/*
UInt32 stci_d (
				char* 								out,
				SInt32						 		in,
				SInt16 								outlen)

{
	return (stc_d (out, (in>=0?in:-in), outlen, in<0));
}
*/
