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
//	File:						SReformatChangeImageFileFormat.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			05/28/2020
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	The routines in this file handle the Processor->Reformat->
//								Change Image File Format menu command.							
//
//------------------------------------------------------------------------------------
 
#include "SMultiSpec.h"
	 
#if defined multispec_wx
	#include "xMultiSpec.h"
	#include "xReformatChangeDialog.h"
	#include "xImageView.h"
#endif	// defined multispec_wx
	
#if defined multispec_mac || defined multispec_mac_swift
	#define IDOK									1
	#define IDC_Header							4
	#define IDC_ChangeImageFileFormat		5
	#define IDC_MosaicImage						6
	#define IDC_ConvertFieldsToThematic		7
	#define IDC_RectifyImage					8
	#define IDC_MultispectralToThematic		9
	#define IDC_ENVIROIToThematic				10
	#define IDC_ModifyChanDescriptions		11
	#define IDC_RecodeThematic					12
	#define IDC_ChanDescriptionChoice		13
	
	#define IDC_InputFileName					5
	#define IDC_ChangeFormatInLines			7
	#define IDC_ChangeFormatInColumns		9
	#define IDC_ChannelsClassesLabel			10
	#define IDC_ChangeFormatInChan			11
	#define IDC_OutputFileSelection			19
	#define IDC_TransformData					29
	#define IDC_MinimumBitsPrompt				30
	#define IDC_DataValueType					33
	#define IDC_BandInterleaveLabel			34
	#define IDC_BandInterleave					35
	#define IDC_ChannelsLabel					36
	//#define IDC_OutChannels						37
	#define IDC_ChannelCombo						37
	#define IDC_SwapBytes						41
	#define IDC_WriteChanDescriptions		42
	#define IDC_ChangeHeader					44
	#define IDC_OutputInWavelengthOrder		47
	
	#define IDS_DialogInsertHeader			6
	#define IDS_DialogChangeHeader			7
	#define IDS_DialogModChanDesc				8
	#define IDS_DialogAddChanDesc				9
#endif	// defined multispec_mac || defined multispec_mac_swift  
                             
#if defined multispec_win
	//#include "WRfrmDlg.h"
	#include "WReformatChangeDialog.h" 
#endif	// defined multispec_win 



			// Prototypes for routines in this file that are only called by
			// other routines in this file.													
			
void		AddWhiteGaussianNoise (
				HUCharPtr							ioOut1ByteBufferPtr,
				HUCharPtr							ioIn1ByteBufferPtr,
				SInt32								numberColumns, 
				double								offset, 
				double								scale);

void 		AdjustDataForChangeFormat (
				ReformatOptionsPtr				reformatOptionsPtr,
				HUInt8Ptr							ioBufferPtr,
				UInt32								numberSamples,
				double								offsetValue, 
				double								multiplier, 
				double								divisor);

Boolean 	ChangeFormatToBILorBISorBSQ (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				FileIOInstructionsPtr			fileIOInstructions2Ptr,
				FileInfoPtr							outFileInfoPtr, 
				ReformatOptionsPtr				reformatOptionsPtr,
				Boolean								forceBISFlag);

void 		ChangeImageFileFormat (void);

Boolean 	ChangeImageFormatDialog (
				FileInfoPtr							fileInfoPtr,
				FileInfoPtr							outFileInfoPtr, 
				ReformatOptionsPtr				reformatOptionsPtr);

void 		ChangeImageFormatDialogUpdateSwapBytes (
				DialogPtr							dialogPtr,
				FileInfoPtr							fileInfoPtr,
				SInt16								dataValueTypeSelection,
				SInt16								headerOptionsSelection);

void 		CheckForSaturation (
				HUInt8Ptr							ioBufferPtr,
				ReformatOptionsPtr				reformatOptionsPtr,
				UInt16								dataTypeCode, 
				UInt32								numberSamples, 
				double								lowSaturatedValue,
				double								highSaturatedValue);

void 		CleanUpChangeFormat (
				UInt16*								newPaletteIndexPtr,
				UInt16*								symbolToOutputBinPtr, 
				HDoublePtr							tempBufferPtr);

void		ConvertDataValueToBinValue (
				HUInt8Ptr							ioBufferPtr,
				SInt32								signedValueOffset,
				double								binFactor,
				double								minValue,
				UInt32								maxBin,
				UInt32								numberSamples);

void 		ConvertSymbolsToBinary (
				HUCharPtr							savedOutBufferPtr,
				UInt16*								symbolToOutputBinPtr,
				Boolean								callConvertDataValueToBinValueFlag, 
				SInt16								numberBytes,
				SInt32								numberSamples);

UInt16* 	CreateNewToOldPaletteVector (
				FileInfoPtr							outFileInfoPtr,
				Boolean								asciiSymbolsFlag);

void 		CreatePCImage (
				HDoublePtr							tempBufferPtr,
				HDoublePtr							inputBufferPtr, 
				ReformatOptionsPtr				reformatOptionsPtr, 
				UInt32								numberSamples,
				Boolean								inputBISFlag);

HUInt16Ptr CreateSymbolToBinaryTable (
				FileInfoPtr							outFileInfoPtr,
				Boolean								compactFlag);

HUInt16Ptr CreateClassToGroupTable (
				FileInfoPtr							inFileInfoPtr,
				FileInfoPtr							outFileInfoPtr,
				UInt16								imageType,
				HUInt16Ptr							symbolToOutputBinPtr,
				HUInt16Ptr							newPaletteIndexPtr);

HUInt16Ptr CreateDataValueToClassTable (
				FileInfoPtr							inFileInfoPtr,
				FileInfoPtr							outFileInfoPtr,
				HUInt16Ptr							symbolToOutputBinPtr,
				HUInt16Ptr							newPaletteIndexPtr);

#if defined multispec_mac
	pascal void	DrawChanDescriptionPopUp (
					DialogPtr 							dialogPtr,
					SInt16	 							itemNumber);

	pascal void	DrawFunctionPopUp (
					DialogPtr 							dialogPtr,
					SInt16	 							itemNumber);
	
	pascal void	DrawPCChannelsPopUp (
					DialogPtr 							dialogPtr,
					SInt16	 							itemNumber);
#endif	// defined multispec_mac 

double	Find_kth_SmallestElementInList (
				double*								vectorOfValuesPtr,
				SInt32								numberElements, 
				SInt32								kthElement);

void		FunctionOfChannels (
				UInt32*								inputCountBufferPtr,
				HUInt8Ptr							ioBufferPtr, 
				ReformatOptionsPtr				reformatOptionsPtr, 
				UInt32								numberSamples,
				double								noDataValue,
				Boolean								noDataValueFlag,
				Boolean								inputBISFlag);

SInt16	GetAdjustBufferData (
				ReformatOptionsPtr				reformatOptionsPtr,
				HUCharPtr							ioBufferPtr2,
				HDoublePtr							ioOutAdjustBufferPtr,
				FileIOInstructionsPtr			fileIOInstructions2Ptr,
				UInt32								numberColumnBytes,
				UInt32								line,
				UInt32								columnStart,
				UInt32								columnEnd,
				UInt32								outputColumnInterval,
				UInt32								numberColumns,
				UInt32								numberChannels,
				Boolean								inputBISFlag);

SInt32 	GetNumberHeaderBytes (
				FileInfoPtr							fileInfoPtr);
							
Boolean	LoadReformatOptionsSpecs (
				WindowInfoPtr						windowInfoPtr);
							
void 		ReformatControl_Old (void);

void		SaveAlgebraicTransformationFunction (
				UInt32								numberChannels,
				SInt16								instrumentCode,
				ReformatOptionsPtr				reformatOptionsPtr);
				
void SetRadiantTemperatureConstants (
				FileInfoPtr							imageFileInfoPtr,
				ReformatOptionsPtr				reformatOptionsPtr);

void 		TransformAdjustChannelsByChannel (
				ReformatOptionsPtr				reformatOptionsPtr,
				HDoublePtr							ioInDoubleBufferPtr,
				HDoublePtr							adjustChannelPtr,
				double								factor,
				SInt16								numberChannels,
				UInt32								numberColumns,
				Boolean								inputBISFlag);

void 		TransformData (
				HUInt8Ptr							ioBufferPtr,
				ReformatOptionsPtr				reformatOptionsPtr, 
				UInt32								numberSamples,
				double								highSaturatedValue, 
				Boolean								inputBISFlag);

							
SInt16	gFunctionSelection = 0;


#define SWAP_ELEMENTS(a,b) {double t=(a);(a)=(b);(b)=t;}


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean AddWhiteGaussianNoise
//
//	Software purpose:	This routine adds the specified amount of white
//							Gaussian noise to a line of image data.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/19/1990
//	Revised By:			Larry L. Biehl			Date: 06/20/1990

void AddWhiteGaussianNoise (
				HUCharPtr							ioOut1ByteBufferPtr, 
				HUCharPtr							ioIn1ByteBufferPtr, 
				SInt32								numberColumns, 
				double								offset, 
				double								scale)

{
	double								dDataValue,
											lOffset,
											lScale;
	
	HUCharPtr							endOut1ByteBufferPtr;
	
	SInt32								randomNumber;
	
	SInt16								dataValue,
											i,
											numberOfAdds = 20;

	
	endOut1ByteBufferPtr = ioOut1ByteBufferPtr + numberColumns;
	
	lOffset = offset*numberOfAdds;
	lScale = scale*numberOfAdds;
	randomNumber = 0;
						
	for (; ioOut1ByteBufferPtr<endOut1ByteBufferPtr; ioOut1ByteBufferPtr++)
		{
		dDataValue = *ioIn1ByteBufferPtr;
		
		randomNumber = 0;
		for (i=0; i<numberOfAdds; i++)
			randomNumber += rand ();
		
		dDataValue += (randomNumber - lOffset)/lScale;
		
		dataValue = (SInt16)(dDataValue + 0.5);
		
		if (dataValue < 0)
			dataValue = 0;
		
		if (dataValue > 255)
			dataValue = 255;
		
		*ioOut1ByteBufferPtr = (UInt8)dataValue;
		
		ioIn1ByteBufferPtr++;
		
		}	// end "for (; ioOut1ByteBufferPtr<ioOut1ByteBufferPtr; ...)" 
							
}	// end "AddWhiteGaussianNoise"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void AdjustDataForChangeFormat
//
//	Software purpose:	This routine adjusts the data for the change image
//							format reformat processor.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:			ChangeFormatToBILorBISorBSQ
//
//	Coded By:			Larry L. Biehl			Date: 01/08/1992
//	Revised By:			Larry L. Biehl			Date: 02/08/2006

void AdjustDataForChangeFormat (
				ReformatOptionsPtr				reformatOptionsPtr,
				HUInt8Ptr							ioBufferPtr,
				UInt32								numberSamples,
				double								offsetValue, 
				double								multiplier, 
				double								divisor)

{
			// Declare local variables & structures										
	
	//double								dataValue;
			
	HDoublePtr	 						ioInDoubleBufferPtr;
	                                  
	UInt32								j;
	
	
	ioInDoubleBufferPtr = (HDoublePtr)ioBufferPtr;
			
	for (j=0; j<numberSamples; j++)
		{
		*ioInDoubleBufferPtr *= multiplier;
		*ioInDoubleBufferPtr /= divisor;
		*ioInDoubleBufferPtr += offsetValue;
		
		ioInDoubleBufferPtr++;
			
		}	// end "for (j=0; j<numberSamples; j++)" 
		
}	// end "AdjustDataForChangeFormat" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				Boolean ChangeFormatToBILorBISorBSQ
//
//	Software purpose:	This routine converts the present active image file
//							to BIL or BIS format with changes to input file as 
//							specified.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/06/1988
//	Revised By:			Larry L. Biehl			Date: 07/09/2018

Boolean ChangeFormatToBILorBISorBSQ (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				FileIOInstructionsPtr			fileIOInstructions2Ptr,
				FileInfoPtr							outFileInfoPtr, 
				ReformatOptionsPtr				reformatOptionsPtr,
				Boolean								forceBISFlag)

{
			// Declare local variables & structures	
												
	CMFileStream*						outFileStreamPtr;
	
	DisplaySpecsPtr					displaySpecsPtr;
	
	FileInfoPtr							fileInfoPtr;
	
	HDoublePtr							tempBufferPtr;
	
	HistogramSummaryPtr				histogramSummaryPtr;
	
	HUCharPtr			 				ioBufferPtr1,
											ioBufferPtr2,
											ioOut1ByteBufferPtr,
								 			ioIn1ByteBufferPtr,	
					 						savedOutBufferPtr;
											
	HDoublePtr							inputDoublePtr,
											ioOutAdjustBufferPtr,
											outputDoublePtr;
					 						
	HFloatPtr							outputFloatPtr;
						
	HSInt8Ptr							outputSInt8Ptr;			
						
	HSInt16Ptr							outputSInt16Ptr;
	
	HSInt32Ptr							inputSInt32Ptr,
											outputSInt32Ptr;	
					 						
	HUInt8Ptr							inputUInt8Ptr,
											outputUInt8Ptr;						
	
	HUInt16Ptr							inputUInt16Ptr,
											outputUInt16Ptr;
											
	HUInt32Ptr							inputUInt32Ptr,
											outputUInt32Ptr;
	
	UInt16								*channelPtr,
											*channelOutOrderPtr,
											*fileWavelengthOrderPtr,
											*newPaletteIndexPtr,
											*symbolToOutputBinPtr;
												
	UInt8*								ioOutBufferPtr;
	
	double								binFactor,
											divisor,
											doubleValue,
											minValue,
											multiplier,
											offsetValue,
											transformAdjustSelectedChannelsFactor;
											
	Handle								displaySpecsHandle,
											histogramSummaryHandle;
	
	SInt32								channelCount,
											columnInterval,
											countOutBytes,
											lastOutputWrittenLine,
											lastPercentComplete,
											limitIoOutBytes,
											lineInterval,
											outOffsetBytes,
											percentComplete,
											preLineBytes,
											totalIOOutBytes,
											writePosOff;	
	
	UInt32								column,
											columnEnd,
											columnStart,
											count,
											fromNumberBytes,
											j,
											line,
											lineCount,
											lineEnd,
											lineStart,
											maxBin,
											numberColumnBytes,
											numberColumns,
											numberGetLineCalls,
											numberInsideLoops,
											numberLines,
											numberOutColumnsChannels,
											numberOutsideLoops,
											outBSQOffsetIncrement,
											outputColumnInterval,
											outsideLoopChannel,
											outSkip,
											supportFileType,
											totalGetLineCalls;
	
	SInt16								errCode,
											thematicListType,
											numberOutChannels,
											numberReadChannels;
												
	UInt16								channelIdentifier,
											forceByteCode;				
	
	Boolean								inputBISFlag,
											compactFlag,
											continueFlag,
											callConvertDataValueToBinValueFlag,
											differentBuffersFlag,
											symbolToBinaryFlag;
	
	
	outFileStreamPtr = GetFileStreamPointer (outFileInfoPtr);
	if (outFileStreamPtr == NULL)									
																							return (FALSE);
		
			// Initialize local variables.													
			
	tempBufferPtr = NULL;
	newPaletteIndexPtr = NULL;
	symbolToOutputBinPtr = NULL;
	
	errCode = noErr;  
	writePosOff = 0;
	divisor = reformatOptionsPtr->adjustDivisor;
	multiplier = reformatOptionsPtr->adjustFactor;
	offsetValue = reformatOptionsPtr->adjustOffset;
	transformAdjustSelectedChannelsFactor = 
								reformatOptionsPtr->transformAdjustSelectedChannelsFactor;
	forceByteCode = reformatOptionsPtr->forceByteCode;
	differentBuffersFlag = (gInputBufferPtr != gOutputBufferPtr);
	preLineBytes = outFileInfoPtr->numberPreLineBytes;
	fileInfoPtr = fileIOInstructionsPtr->fileInfoPtr;
	thematicListType = kClassDisplay;
	fromNumberBytes = reformatOptionsPtr->convertFromNumberBytes;
	
			// Get the channel wavelength order in case it is needed.
			// Use reformat channel list if the user is not requesting that the data
			// be put into wavelength order.

	fileWavelengthOrderPtr = (UInt16*)reformatOptionsPtr->channelPtr;
	if (reformatOptionsPtr->outputInWavelengthOrderFlag)
		fileWavelengthOrderPtr = (UInt16*)
			&reformatOptionsPtr->channelPtr[gImageWindowInfoPtr->totalNumberChannels];
	
			// Get tables for converting Thematic Image files if needed.
		         
	symbolToBinaryFlag = FALSE;
	callConvertDataValueToBinValueFlag = FALSE;
	if (outFileInfoPtr->thematicType)
		{
		newPaletteIndexPtr = CreateNewToOldPaletteVector (
													outFileInfoPtr, fileInfoPtr->asciiSymbols);
		if (newPaletteIndexPtr == NULL)
																					return (FALSE);
			                        
		if (fileInfoPtr->asciiSymbols)
			{
			compactFlag = TRUE;
			symbolToBinaryFlag = TRUE;
			symbolToOutputBinPtr = CreateSymbolToBinaryTable (outFileInfoPtr,
																				compactFlag);
			
			}	// end "if (fileInfoPtr->asciiSymbols)"
			
		if (gImageWindowInfoPtr->imageType == kMultispectralImageType &&
							gImageWindowInfoPtr->windowType == kThematicWindowType)
			{			
			symbolToBinaryFlag = TRUE;
			
			histogramSummaryHandle = GetHistogramSummaryHandle (gImageWindowInfoPtr);
			histogramSummaryPtr = (HistogramSummaryPtr)GetHandlePointer (
																			histogramSummaryHandle);
																			
			displaySpecsHandle = GetDisplaySpecsHandle (gImageWindowInfoPtr);
			displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (displaySpecsHandle);
			
			if (histogramSummaryPtr != NULL && displaySpecsPtr != NULL)
				{	
				GetOneChannelThematicDisplayConversionValues (
										gImageWindowInfoPtr,
										&histogramSummaryPtr[*(reformatOptionsPtr->channelPtr)],
										displaySpecsPtr,
										&binFactor,
										&minValue,
										&maxBin);

				symbolToOutputBinPtr = CreateDataValueToClassTable (
																			fileInfoPtr,
																			outFileInfoPtr,
																			symbolToOutputBinPtr,
																			newPaletteIndexPtr);
					
				}	// end "if (histogramSummaryPtr != NULL && ..."
														
			callConvertDataValueToBinValueFlag = TRUE;
														
			}	// end "if (gImageWindowInfoPtr->imageType == ..."
			
		if (symbolToBinaryFlag && symbolToOutputBinPtr == NULL)
			{	
			CleanUpChangeFormat (newPaletteIndexPtr, 
											symbolToOutputBinPtr, 
											tempBufferPtr);	
																						return (FALSE);
																				
			}	// end "if (symbolToOutputBinPtr == NULL)"
		
				// Load the information to convert the thematic classes to groups if
				// the image is being displayed in the group mode. In other words
				// recode the thematic image to groups.
				
		thematicListType = GetActiveLegendListType ();
		if (thematicListType > kClassDisplay)
			symbolToOutputBinPtr = CreateClassToGroupTable (fileInfoPtr,
																			outFileInfoPtr,
																			gImageWindowInfoPtr->imageType,
																			symbolToOutputBinPtr,
																			newPaletteIndexPtr);
		
		if (symbolToOutputBinPtr != NULL)
			symbolToBinaryFlag = TRUE;
		
		}	// end "if (fileInfoPtr->thematicType)"
	
	//swapBytesFlag = fileInfoPtr->swapBytesFlag;
	
	inputBISFlag = (gImageWindowInfoPtr->bandInterleave == kBIS || forceBISFlag);
					
	ioOutBufferPtr = (UInt8*)reformatOptionsPtr->ioOutBufferPtr;
	ioOutAdjustBufferPtr = (HDoublePtr)reformatOptionsPtr->ioOutAdjustBufferPtr;
				
	lastPercentComplete = -1;
	
	lineStart = reformatOptionsPtr->lineStart;
	lineEnd =  reformatOptionsPtr->lineEnd;
	lineInterval = reformatOptionsPtr->lineInterval;
	if (reformatOptionsPtr->bottomToTop)  
		lineInterval = -lineInterval;
	
	columnStart = reformatOptionsPtr->columnStart;
	columnEnd =  reformatOptionsPtr->columnEnd;
	outputColumnInterval = reformatOptionsPtr->columnInterval;
	
	numberColumns = 
				(columnEnd - columnStart + outputColumnInterval)/outputColumnInterval;
	numberLines = (lineEnd - lineStart + reformatOptionsPtr->lineInterval)/
																reformatOptionsPtr->lineInterval;
															
	numberOutChannels = reformatOptionsPtr->numberChannels;
	if (reformatOptionsPtr->transformDataCode == kTransformChannels ||
							reformatOptionsPtr->transformDataCode == kFunctionOfChannels)
		numberOutChannels = 1;
	if (reformatOptionsPtr->transformDataCode == kCreatePCImage)
		numberOutChannels = reformatOptionsPtr->numberPCChannels;
	numberOutColumnsChannels = numberOutChannels * numberColumns;
	
	numberColumnBytes = fromNumberBytes * numberColumns;
	
	numberReadChannels = reformatOptionsPtr->numberChannels;
	
	reformatOptionsPtr->startColumn = 0;
	if (reformatOptionsPtr->rightToLeft) 
		reformatOptionsPtr->startColumn = numberColumns -1;
	
	if (inputBISFlag && numberOutChannels > 1)
		{	
		columnInterval = reformatOptionsPtr->numberChannels;
		reformatOptionsPtr->startColumn *= reformatOptionsPtr->numberChannels;
		
		}	// end "if (inputBISFlag && ..." 
	
	else	// !inputBISFlag || ...
		columnInterval = 1;
		
	if (reformatOptionsPtr->rightToLeft)  
		columnInterval = -columnInterval;
	
	outSkip = 1;
	if (forceBISFlag) 
			//&&  reformatOptionsPtr->functionCode == kNoFunction)
		outSkip *= numberOutChannels;
		
			// Get a temp buffer for the transformation if needed.	
			// Also get the low saturated data value to use.																		
			
	if (reformatOptionsPtr->transformDataCode == kCreatePCImage || 
				(reformatOptionsPtr->transformDataCode == kFunctionOfChannels &&
									reformatOptionsPtr->functionCode == kFunctionAverage))
		{
		if (reformatOptionsPtr->transformDataCode == kCreatePCImage)
			tempBufferPtr =
							(HDoublePtr)MNewPointer (numberOutChannels * sizeof (double));
			
		else	// reformatOptionsPtr->transformDataCode == kFunctionOfChannels
			tempBufferPtr =
							(HDoublePtr)MNewPointer (numberColumns * sizeof (UInt32));
			
		if (tempBufferPtr == NULL)
			{
			CleanUpChangeFormat (newPaletteIndexPtr, 
											symbolToOutputBinPtr, 
											tempBufferPtr);
																						return (FALSE);
			}	// end "if (tempBufferPtr == NULL)"
																																	
		}	// end "if (reformatOptionsPtr->transformDataCode == kCreatePCImage)" 
	
			// Write a header if requested.													
			
	continueFlag = WriteNewImageHeader (fileInfoPtr,
													outFileInfoPtr, 
													(HPtr)ioOutBufferPtr,
													NULL,
													kFromReformat,
													reformatOptionsPtr->outputFileCode, 
													reformatOptionsPtr->lineInterval,
													reformatOptionsPtr->columnInterval,
													thematicListType,
													kNoPaletteColorsDefined);
	
			// If the header was created okay and the output interleave is to be BSQ,
			// set the end of the file to the size of the file. This is done because
			// the data will not be written to the file in sequential order.
			
	if (continueFlag && outFileInfoPtr->bandInterleave == kBSQ)
		{
		SInt64 numberBytes = (SInt64)outFileInfoPtr->bytesPer1line1chan *
		outFileInfoPtr->numberLines * outFileInfoPtr->numberChannels +
																outFileInfoPtr->numberHeaderBytes;
												
		errCode = MSetSizeOfFile (outFileStreamPtr,
											numberBytes,
											kErrorMessages);
			
				// Now make sure the marker is set after the header. Have found that on
				// windows the marker is at the end of the header.

		if (errCode == noErr)	
			errCode = MSetMarker (outFileStreamPtr, 
											fsFromStart,
											outFileInfoPtr->numberHeaderBytes,
											kErrorMessages);
											
		if (errCode != noErr)
			continueFlag = FALSE;
		
		}	// end "if (errCode == noErr && ...->bandInterleave == kBSQ && ..."
					
			// Return if there was an IO error									
			
	if (continueFlag)
		{
				// Set up output working buffer pointer.  This pointer will			
				// change as the output buffer fills up									
		
		ioOut1ByteBufferPtr = (HUCharPtr)ioOutBufferPtr;
		savedOutBufferPtr = ioOut1ByteBufferPtr;
		
				// Determine if outside channel loop is to be used. This is done for band
				// sequential data when multiple channels do not need to be available at
				// the same time to speed the disk access up.
				
		numberGetLineCalls = 0;
		numberOutsideLoops = 1;
		numberInsideLoops = numberOutChannels;
		totalGetLineCalls = numberLines;
		
		channelPtr = (UInt16*)reformatOptionsPtr->channelPtr;
		channelOutOrderPtr = fileWavelengthOrderPtr;
		
		countOutBytes = reformatOptionsPtr->countOutBytes;
		if (reformatOptionsPtr->transformDataCode == kNoTransform &&
					fileInfoPtr->bandInterleave == kBSQ &&
							outFileInfoPtr->bandInterleave == kBSQ &&
									numberOutChannels > 1)
			{
			numberOutsideLoops = numberOutChannels;
			channelPtr = &channelIdentifier;
			channelOutOrderPtr = &channelIdentifier;
			numberReadChannels = 1;
			numberInsideLoops = 1;
			totalGetLineCalls *= numberOutsideLoops;
			
					// This was originally set up for all channels being done for each line
					// instead of all lines for each channel.
			
			countOutBytes /= numberOutChannels;
			numberOutColumnsChannels /= numberOutChannels;
			
			}	// end "if (reformatOptionsPtr->transformDataCode == kNoTransform && ..."
		
				// Set up limit of output buffer to know when to write buffer to	
				// the disk file. Make certain which buffer is the output buffer.	
				
		limitIoOutBytes = reformatOptionsPtr->ioOutBufferBytes - countOutBytes;
		totalIOOutBytes = 0;
		
				// Get count of bytes that are loaded into the output buffer each	
				// time through the channel loop.											
				
		outOffsetBytes = countOutBytes;
		if (outFileInfoPtr->format == kMatlabType)
			outOffsetBytes = countOutBytes*outFileInfoPtr->numberBytes/8;
														
		if (reformatOptionsPtr->convertType != 1)
			{
					// Note that BSQ format is never convertType == 1
					
			if (forceBISFlag && numberOutChannels > 1)
				outOffsetBytes = (outOffsetBytes-preLineBytes)/numberOutColumnsChannels;
				
			else	// !forceBISFlag || ...
				outOffsetBytes = (outOffsetBytes-preLineBytes)/numberInsideLoops;
				
			}	// end "if (reformatOptionsPtr->convertType != 1)" 
			
				// Get the output offset increment for each channel if the output band
				// interleave is BSQ
		
		outBSQOffsetIncrement = 0;
		if (outFileInfoPtr->bandInterleave == kBSQ ||
													outFileInfoPtr->bandInterleave == kBNonSQ)
			outBSQOffsetIncrement = 
									reformatOptionsPtr->ioOutBufferBytes/numberOutChannels;
										
		if (numberOutsideLoops > 1)
			outBSQOffsetIncrement = outOffsetBytes;
		
				// Initialize the buffer to load the data into.  Assume here that	
				// the conversion type is not 1.  											

		ioBufferPtr1 = gInputBufferPtr;
		ioBufferPtr2 = gOutputBufferPtr;
					
				// Intialize the nextTime variable to indicate when the next 		
				// check should occur for a command-.										
				
		gNextTime = TickCount ();					
		for (outsideLoopChannel=0;
				outsideLoopChannel<numberOutsideLoops;
					outsideLoopChannel++)
			{
			if (numberOutsideLoops > 1)
				channelIdentifier = fileWavelengthOrderPtr[outsideLoopChannel];
			
					// Load some of the File IO Instructions structure that pertain
					// to the specific area being used.
					
			SetUpFileIOInstructions (fileIOInstructionsPtr,
												NULL,
												reformatOptionsPtr->startLine,
												reformatOptionsPtr->lineEnd,
												reformatOptionsPtr->lineInterval,
												columnStart,
												columnEnd,
												outputColumnInterval,
												numberReadChannels,
												channelPtr,
												kDetermineSpecialBILFlag);		
												
			if (reformatOptionsPtr->transformDataCode == kAdjustChannelsByChannel &&
															reformatOptionsPtr->locationInList < 0)
				SetUpFileIOInstructions (fileIOInstructions2Ptr,
													NULL,
													reformatOptionsPtr->startLine,
													reformatOptionsPtr->lineEnd,
													reformatOptionsPtr->lineInterval,
													columnStart,
													columnEnd,
													outputColumnInterval,
													1,
													fileIOInstructions2Ptr->channelListPtr,
													kDetermineSpecialBILFlag);	
			
			lineCount = 0;
			line = reformatOptionsPtr->startLine;
			lastOutputWrittenLine = 0;
			
			while (lineCount < numberLines && continueFlag)
				{		
						// Add the preline calibration bytes if any.  For now this is	
						// only handled for GAIA data.											
						
				if (preLineBytes)
					{
					InitializeGAIALineBytes ((HUInt16Ptr)ioOut1ByteBufferPtr,
														numberColumns,
														0);
														
					ioOut1ByteBufferPtr += 14;
					
					}	// end "if (preLineBytes)" 
						
						// If the conversion type is 1, we can load the input data 		
						// directly into the output buffer.  Update the buffer to load	
						// data into.																	
																					
				if (reformatOptionsPtr->convertType == 1)
					{
					if (!differentBuffersFlag)
						ioBufferPtr1 = ioOut1ByteBufferPtr;
					ioBufferPtr2 = ioOut1ByteBufferPtr;
					
					}	// end "if (reformatOptionsPtr->convertType == 1)" 
				
						// Get all requested channels for line of image data.  Return 	
						// if there is a file IO error.											
					 
				errCode = GetLineOfData (fileIOInstructionsPtr,
													line,
													columnStart,
													columnEnd,
													outputColumnInterval,
													ioBufferPtr1,  
													ioBufferPtr2);
				if (errCode != noErr)
					{		
					CleanUpChangeFormat (newPaletteIndexPtr, 
													symbolToOutputBinPtr, 
													tempBufferPtr);	
					CloseUpFileIOInstructions (fileIOInstructionsPtr, NULL);					
																					return (FALSE);
					
					}	// end "if (errCode != noErr)"
					
						// Get data for channel that is to be used to adjust the selected
						// channels if needed.
					
				if (reformatOptionsPtr->transformDataCode == kAdjustChannelsByChannel)
					{
					errCode = GetAdjustBufferData (reformatOptionsPtr,
																ioBufferPtr2,
																ioOutAdjustBufferPtr,
																fileIOInstructions2Ptr,
																numberColumnBytes,
																line,
																columnStart,
																columnEnd,
																outputColumnInterval,
																numberColumns,
																numberOutChannels,
																inputBISFlag);
					/*
					if (reformatOptionsPtr->locationInList >= 0)
						{
								// Copy data to ioOutAdjustBufferPtr
						
						if (inputBISFlag)
							{
							
							}
							
						else	// !inputBISFlag
							{
							inputBufferOffset = 
										reformatOptionsPtr->locationInList * numberColumnBytes;
							memcpy ((char*)ioOutAdjustBufferPtr, 
										&ioBufferPtr2[inputBufferOffset], 
										numberColumnBytes);
							
							}	// end "else !inputBISFlag"

						}	// end "if (reformatOptionsPtr->locationInList >= 0)"
						
					else	// reformatOptionsPtr->locationInList < 0
						{
								// The data for the channel to be used to adjust the selected 
								// channels has not been read. Read the data directly to the 
								// adjust buffer location.
																	
						errCode = GetLineOfData (fileIOInstructions2Ptr,
															line,
															columnStart,
															columnEnd,
															outputColumnInterval,
															gInputBuffer2Ptr,  
															(UCharPtr)ioOutAdjustBufferPtr);
					*/
					if (errCode != noErr)
						{
						CleanUpChangeFormat (newPaletteIndexPtr, 
														symbolToOutputBinPtr, 
														tempBufferPtr);	
						CloseUpFileIOInstructions (fileIOInstructionsPtr, NULL);					
																						return (FALSE);
						
						}	// end "if (errCode != noErr)"
						
						//}	// end "else reformatOptionsPtr->locationInList < 0"
					
					}	// end "if (...->transformDataCode == kAdjustChannelsByChannel)"
					
				numberGetLineCalls++;
				
						// Adjust the data if needed.
						
				if (reformatOptionsPtr->transformDataCode == kAdjustChannel)
					AdjustDataForChangeFormat (reformatOptionsPtr,
														ioBufferPtr2, 
														numberOutColumnsChannels, 
														offsetValue, 
														multiplier, 
														divisor);											
					
				else if (reformatOptionsPtr->transformDataCode == kAdjustChannelsByChannel)
					TransformAdjustChannelsByChannel (
														reformatOptionsPtr,
														(HDoublePtr)ioBufferPtr2,
														ioOutAdjustBufferPtr, 
														transformAdjustSelectedChannelsFactor,
														numberOutChannels,
														numberColumns, 
														inputBISFlag);
									
				else if (reformatOptionsPtr->transformDataCode == kCreatePCImage)
					CreatePCImage (tempBufferPtr,
										(HDoublePtr)ioBufferPtr2, 
										reformatOptionsPtr, 
										numberColumns,
										inputBISFlag);
											
				else if (reformatOptionsPtr->transformDataCode == kTransformChannels)
					TransformData (ioBufferPtr2,
										reformatOptionsPtr,
										numberColumns,
										outFileInfoPtr->maxUsableDataValue,
										inputBISFlag);
					
				else if (reformatOptionsPtr->transformDataCode == kFunctionOfChannels)
					FunctionOfChannels (
									(UInt32*)tempBufferPtr,
									ioBufferPtr2, 
									reformatOptionsPtr, 
									numberColumns, 
									outFileInfoPtr->noDataValue, 
									outFileInfoPtr->noDataValueFlag, 
									inputBISFlag);
									
				else if (callConvertDataValueToBinValueFlag)
					ConvertDataValueToBinValue (
									ioBufferPtr2,
									fileInfoPtr->signedValueOffset,
									binFactor,
									minValue,
									maxBin, 
									numberColumns); 

				if (reformatOptionsPtr->checkForSaturationFlag)
					CheckForSaturation (
									ioBufferPtr2, 
									reformatOptionsPtr,
									reformatOptionsPtr->workingDataTypeCode, 
									numberOutColumnsChannels, 
									outFileInfoPtr->minUsableDataValue,
									outFileInfoPtr->maxUsableDataValue);
						
				channelCount = 0;
				while (channelCount<(SInt32)numberInsideLoops && continueFlag)
					{
							// Set input buffer pointer for the channel to be handled.
					
					if (inputBISFlag)
						ioIn1ByteBufferPtr =
							(HUCharPtr)&ioBufferPtr2[channelCount*fromNumberBytes];
						
					else	// !inputBISFlag 
						ioIn1ByteBufferPtr = 
							(HUCharPtr)&ioBufferPtr2[channelCount*numberColumnBytes];
						
							// Initialize column start.											
													
					column = reformatOptionsPtr->startColumn;
							
							// Update output buffer pointers to point to start of			
							// next channel of data.												
					
					if (forceBISFlag)
						ioOut1ByteBufferPtr = &savedOutBufferPtr[
											preLineBytes + channelCount * outOffsetBytes];
				
							// Switch on conversion type											
						
					switch (reformatOptionsPtr->convertType)
						{
						case 1:
									// Options for this section are:								
									//		Number input bytes = number output bytes.			
									//  	Not right-to-left											
									//		BIL, BSQ -> BIL											
									//		BIL, BSQ, BIS -> BIS										
									
									// NOTE:  The data has already been loaded into the		
									// output buffer.													
									
									// Update these counts so that we will not go through	
									// the channel loop anymore. The channel loop has		
									// been completed.												
									
							channelCount = numberInsideLoops - 1;
							break;
							
						case k8BitTo8Bit:
						case k8BitIntSignedTo8BitIntUnsigned:
						case k8BitIntUnsignedTo8BitIntSigned:
									// Options for this section are:								
									//  	1 byte->1 byte												
									//		right-to-left	
							outputUInt8Ptr = ioOut1ByteBufferPtr;
							inputUInt8Ptr = ioIn1ByteBufferPtr;
									
							for (j=0; j<numberColumns; j++)
								{
								*outputUInt8Ptr = inputUInt8Ptr[column];
								outputUInt8Ptr += outSkip;
								column += columnInterval;
								
								}	// end "for (j=0; j<numberColumns; j++)"
							break;
							
						case k8BitIntSignedTo16BitIntSigned: 
						case k8BitIntUnsignedTo16BitIntUnsigned: 
						case k8BitIntSignedTo16BitIntUnsigned:
						case k8BitIntUnsignedTo16BitIntSigned:
									// Options for this section are:
									//		-> BSQ								
									//  	1 byte->2 byte											
									//		right-to-left	
									// The data are already converted to 2 byte format. Just put
									// in correct order.
							
						case k16BitTo16Bit:
						case k16BitIntSignedTo16BitIntUnsigned:
						case k16BitIntUnsignedTo16BitIntSigned:
									// Options for this section are:								
									//  	2 byte->2 byte												
									//		right-to-left		
							outputUInt16Ptr = (HUInt16Ptr)ioOut1ByteBufferPtr;
							inputUInt16Ptr = (HUInt16Ptr)ioIn1ByteBufferPtr;
									
							for (j=0; j<numberColumns; j++)
								{
								*outputUInt16Ptr = inputUInt16Ptr[column];
								outputUInt16Ptr += outSkip;
								column += columnInterval;
								
								}	// end "for (j=0; j<numberColumns; j++)"
							break;
							
						case k8BitIntSignedTo32BitIntSigned: 
						case k8BitIntUnsignedTo32BitIntUnsigned: 
						case k8BitIntSignedTo32BitIntUnsigned:
						case k8BitIntUnsignedTo32BitIntSigned:
						case k8BitIntSignedTo32BitReal: 
						case k8BitIntUnsignedTo32BitReal: 
						case k16BitIntSignedTo32BitIntSigned: 
						case k16BitIntUnsignedTo32BitIntUnsigned: 
						case k16BitIntSignedTo32BitIntUnsigned:
						case k16BitIntUnsignedTo32BitIntSigned:
						case k16BitIntSignedTo32BitReal: 
						case k16BitIntUnsignedTo32BitReal: 
									// Options for this section are:
									//		-> BSQ								
									//  	1 or 2 byte->4 byte											
									//		right-to-left	
									// The data are already converted to 4 byte format. Just put
									// in correct order.
							
						case k32BitTo32Bit:
						case k32BitIntSignedTo32BitIntUnsigned:
						case k32BitIntUnsignedTo32BitIntSigned:
									// Options for this section are:								
									//  	4 byte->4 byte												
									//		right-to-left	
							outputUInt32Ptr = (HUInt32Ptr)ioOut1ByteBufferPtr;
							inputUInt32Ptr = (HUInt32Ptr)ioIn1ByteBufferPtr;
									
							for (j=0; j<numberColumns; j++)
								{
								*outputUInt32Ptr = inputUInt32Ptr[column];
								outputUInt32Ptr += outSkip;
								column += columnInterval;
								
								}	// end "for (j=0; j<numberColumns; j++)"
							break;
							
						case k8BitIntSignedTo64BitReal: 
						case k8BitIntUnsignedTo64BitReal:
						case k16BitIntSignedTo64BitReal: 
						case k16BitIntUnsignedTo64BitReal: 
						case k32BitIntSignedTo64BitReal:
						case k32BitIntUnsignedTo64BitReal:
						case k32BitRealTo64BitReal: 
									// Options for this section are:
									//		-> BSQ								
									//  	1, 2 or 4 byte->8 byte											
									//		right-to-left	
									// The data are already converted to 8 byte format. Just put
									// in correct order.
							
						case k64BitTo64Bit:
									// Options for this section are:								
									//  	8 byte->8 byte												
									//		right-to-left
							outputDoublePtr = (HDoublePtr)ioOut1ByteBufferPtr;
							inputDoublePtr = (HDoublePtr)ioIn1ByteBufferPtr;
									
							for (j=0; j<numberColumns; j++)
								{
								*outputDoublePtr = inputDoublePtr[column];
								outputDoublePtr += outSkip;
								column += columnInterval;
								
								}	// end "for (j=0; j<numberColumns; j++)"
							break;
						/*
						case k16BitIntSignedTo8BitIntUnsigned:
						case k16BitIntUnsignedTo8BitIntUnsigned:
						case k16BitIntUnsignedTo8BitIntSigned:
									// Options for this section are:								
									//  	2 byte->1 byte												
									//		right-to-left
							outputUInt8Ptr = (HUInt8Ptr)ioOut1ByteBufferPtr;
							inputUInt16Ptr = (HUInt16Ptr)ioIn1ByteBufferPtr;
									
							for (j=0; j<numberColumns; j++)
								{
								*outputUInt8Ptr = inputUInt16Ptr[column];
								outputUInt8Ptr += outSkip;
								column += columnInterval;
								
								}	// end "for (j=0; j<numberColumns; j++)"
							break;
							
						case k16BitIntSignedTo8BitIntSigned:
									// Options for this section are:								
									//  	2 byte->1 byte												
									//		right-to-left	
							outputSInt8Ptr = (HSInt8Ptr)ioOut1ByteBufferPtr;
							inputSInt16Ptr = (HSInt16Ptr)ioIn1ByteBufferPtr;
									
							for (j=0; j<numberColumns; j++)
								{
								*outputSInt8Ptr = inputSInt16Ptr[column];
								outputSInt8Ptr += outSkip;
								column += columnInterval;
								
								}	// end "for (j=0; j<numberColumns; j++)"
							break;
						*/
						case k32BitIntSignedTo8BitIntUnsigned:
						case k32BitIntUnsignedTo8BitIntUnsigned:
						case k32BitIntUnsignedTo8BitIntSigned:
									// Options for this section are:								
									//  	2 byte->1 byte												
									//		right-to-left		
							outputUInt8Ptr = (HUInt8Ptr)ioOut1ByteBufferPtr;
							inputUInt32Ptr = (HUInt32Ptr)ioIn1ByteBufferPtr;
									
							for (j=0; j<numberColumns; j++)
								{
								*outputUInt8Ptr = (UInt8)inputUInt32Ptr[column];
								outputUInt8Ptr += outSkip;
								column += columnInterval;
								
								}	// end "for (j=0; j<numberColumns; j++)"
							break;
							
						case k32BitIntSignedTo8BitIntSigned:
									// Options for this section are:								
									//  	2 byte->1 byte												
									//		right-to-left
							outputSInt8Ptr = (HSInt8Ptr)ioOut1ByteBufferPtr;
							inputSInt32Ptr = (HSInt32Ptr)ioIn1ByteBufferPtr;
									
							for (j=0; j<numberColumns; j++)
								{
								*outputSInt8Ptr = (SInt8)inputSInt32Ptr[column];
								outputSInt8Ptr += outSkip;
								column += columnInterval;
								
								}	// end "for (j=0; j<numberColumns; j++)"
							break;
							
						case k32BitIntSignedTo16BitIntUnsigned:
						case k32BitIntUnsignedTo16BitIntUnsigned:
						case k32BitIntUnsignedTo16BitIntSigned:
									// Options for this section are:								
									//  	4 byte->2 byte												
									//		right-to-left	
							outputUInt16Ptr = (HUInt16Ptr)ioOut1ByteBufferPtr;
							inputUInt32Ptr = (HUInt32Ptr)ioIn1ByteBufferPtr;
									
							for (j=0; j<numberColumns; j++)
								{
								*outputUInt16Ptr = (UInt16)inputUInt32Ptr[column];
								outputUInt16Ptr += outSkip;
								column += columnInterval;
								
								}	// end "for (j=0; j<numberColumns; j++)"
							break;
							
						case k32BitIntSignedTo16BitIntSigned:
									// Options for this section are:								
									//  	4 byte->2 byte												
									//		right-to-left
							outputSInt16Ptr = (HSInt16Ptr)ioOut1ByteBufferPtr;
							inputSInt32Ptr = (HSInt32Ptr)ioIn1ByteBufferPtr;
									
							for (j=0; j<numberColumns; j++)
								{
								*outputSInt16Ptr = (SInt16)inputSInt32Ptr[column];
								outputSInt16Ptr += outSkip;
								column += columnInterval;
								
								}	// end "for (j=0; j<numberColumns; j++)"
							break;
							
						case k64BitRealTo8BitIntUnsigned:
									// Options for this section are:								
									//  	8 byte->1 byte												
									//		right-to-left	
							outputUInt8Ptr = (HUInt8Ptr)ioOut1ByteBufferPtr;
							inputDoublePtr = (HDoublePtr)ioIn1ByteBufferPtr;
									
							for (j=0; j<numberColumns; j++)
								{
								*outputUInt8Ptr = (UInt8)(inputDoublePtr[column] + .5);
								outputUInt8Ptr += outSkip;
								column += columnInterval;
								
								}	// end "for (j=0; j<numberColumns; j++)"
							break;
							
						case k64BitRealTo8BitIntSigned:
									// Options for this section are:								
									//  	8 byte->1 byte												
									//		right-to-left	
							outputSInt8Ptr = (HSInt8Ptr)ioOut1ByteBufferPtr;
							inputDoublePtr = (HDoublePtr)ioIn1ByteBufferPtr;
									
							for (j=0; j<numberColumns; j++)
								{
								doubleValue = inputDoublePtr[column];
								*outputSInt8Ptr = (SInt8)(doubleValue + SIGN2 (.5, doubleValue));
								outputSInt8Ptr += outSkip;
								column += columnInterval;
								
								}	// end "for (j=0; j<numberColumns; j++)"
							break;
							
						case k64BitRealTo16BitIntUnsigned:
									// Options for this section are:								
									//  	8 byte->2 byte												
									//		right-to-left	
							outputUInt16Ptr = (HUInt16Ptr)ioOut1ByteBufferPtr;
							inputDoublePtr = (HDoublePtr)ioIn1ByteBufferPtr;
									
							for (j=0; j<numberColumns; j++)
								{
								*outputUInt16Ptr = (UInt16)(inputDoublePtr[column] + .5);
								outputUInt16Ptr += outSkip;
								column += columnInterval;
								
								}	// end "for (j=0; j<numberColumns; j++)"
							break;
							
						case k64BitRealTo16BitIntSigned:
									// Options for this section are:								
									//  	8 byte->2 byte												
									//		right-to-left	
							outputSInt16Ptr = (HSInt16Ptr)ioOut1ByteBufferPtr;
							inputDoublePtr = (HDoublePtr)ioIn1ByteBufferPtr;
									
							for (j=0; j<numberColumns; j++)
								{
								doubleValue = inputDoublePtr[column];
								*outputSInt16Ptr =
												(SInt16)(doubleValue + SIGN2 (.5, doubleValue));
								outputSInt16Ptr += outSkip;
								column += columnInterval;
								
								}	// end "for (j=0; j<numberColumns; j++)"
							break;
							
						case k64BitRealTo32BitIntUnsigned:
									// Options for this section are:								
									//  	8 byte->4 byte												
									//		right-to-left	
							outputUInt32Ptr = (HUInt32Ptr)ioOut1ByteBufferPtr;
							inputDoublePtr = (HDoublePtr)ioIn1ByteBufferPtr;
									
							for (j=0; j<numberColumns; j++)
								{
								*outputUInt32Ptr = (UInt32)(inputDoublePtr[column] + .5);
								outputUInt32Ptr += outSkip;
								column += columnInterval;
								
								}	// end "for (j=0; j<numberColumns; j++)"
							break;
							
						case k64BitRealTo32BitIntSigned:
									// Options for this section are:								
									//  	8 byte->4 byte												
									//		right-to-left	
							outputSInt32Ptr = (HSInt32Ptr)ioOut1ByteBufferPtr;
							inputDoublePtr = (HDoublePtr)ioIn1ByteBufferPtr;
									
							for (j=0; j<numberColumns; j++)
								{
								doubleValue = inputDoublePtr[column];
								*outputSInt32Ptr =
												(SInt32)(doubleValue + SIGN2 (.5, doubleValue));
								outputSInt32Ptr += outSkip;
								column += columnInterval;
								
								}	// end "for (j=0; j<numberColumns; j++)"
							break;
							
						case k64BitRealTo32BitReal:
									// Options for this section are:								
									//  	8 byte->4 byte-real										
									//		right-to-left	
							outputFloatPtr = (HFloatPtr)ioOut1ByteBufferPtr;
							inputDoublePtr = (HDoublePtr)ioIn1ByteBufferPtr;
									
							for (j=0; j<numberColumns; j++)
								{
								*outputFloatPtr = (float)inputDoublePtr[column];
								outputFloatPtr += outSkip;
								column += columnInterval;
								
								}	// end "for (j=0; j<numberColumns; j++)"
							break;
							
						}	// end "switch (reformatOptionsPtr->convertType)"  
					/*
					if (swapBytesFlag)
						{
						ioOut2ByteBufferPtr -= numberColumns * outSkip;
						for (j=0; j<numberColumns; j++)
							{
							*ioOut2ByteBufferPtr = 
									((*ioOut2ByteBufferPtr & 0xff00) >> 8) | 
												((*ioOut2ByteBufferPtr & 0x00ff) << 8);
							ioOut2ByteBufferPtr += outSkip;
							
							}	// end "for (j=0; j<numberColumns; j++" 
						
						}	// end "if (swapBytesFlag)"
					*/
					channelCount++;											
					
					if (outFileInfoPtr->bandInterleave == kBSQ ||
											outFileInfoPtr->bandInterleave == kBNonSQ)
						ioOut1ByteBufferPtr = &savedOutBufferPtr[
															channelCount * outBSQOffsetIncrement];										
					
					else if (outFileInfoPtr->bandInterleave == kBIL)
						ioOut1ByteBufferPtr = &savedOutBufferPtr[
															channelCount * outOffsetBytes];
					
					}	// end "while (channelCount<numberInsideLoops..." 
					
				if (symbolToBinaryFlag)
					ConvertSymbolsToBinary (savedOutBufferPtr,
													symbolToOutputBinPtr,
													callConvertDataValueToBinValueFlag,
													outFileInfoPtr->numberBytes,
													numberOutColumnsChannels);
				/*
				if (outFileInfoPtr->format == kMatlabType)
					ConvertToMatlabFormat (savedOutBufferPtr, 
													outFileInfoPtr->numberBytes,
													outFileInfoPtr->signedDataFlag,
													numberOutColumnsChannels);
				*/
				if (outFileInfoPtr->format == kGAIAType)
					ConvertLineToGAIAFormat (
											NULL,
											(UInt16*)savedOutBufferPtr,
											numberColumns,
											line);
				
				if (outFileInfoPtr->bandInterleave == kBSQ ||
											outFileInfoPtr->bandInterleave == kBNonSQ)
					savedOutBufferPtr = &savedOutBufferPtr[outOffsetBytes];
				
				else	// outFileInfoPtr->bandInterleave != kBSQ
					savedOutBufferPtr = &savedOutBufferPtr[countOutBytes];

				ioOut1ByteBufferPtr = savedOutBufferPtr;

						// Write line(s), channel(s) of data when needed.					
				
				totalIOOutBytes += countOutBytes;
				
				if (totalIOOutBytes > limitIoOutBytes)
					{		
					errCode = WriteOutputDataToFile (outFileInfoPtr,
																	outFileStreamPtr,
																	ioOutBufferPtr,
																	(SInt16*)channelOutOrderPtr,
																	numberInsideLoops,	// numberOutChannels,
																	lastOutputWrittenLine,
																	outOffsetBytes,
																	numberLines,
																	outBSQOffsetIncrement,
																	totalIOOutBytes,
																	reformatOptionsPtr,
																	numberOutsideLoops);
					
					if (errCode != noErr)
						break;
						
					totalIOOutBytes = 0;
					ioOut1ByteBufferPtr = (HUCharPtr)ioOutBufferPtr;
					savedOutBufferPtr = ioOut1ByteBufferPtr;
					
					lastOutputWrittenLine = lineCount+1;
					
					}	// end "if (totalIOOutBytes > limitIoOutBytes)" 
				
						// Check if user wants to abort processing.							
						
				if (TickCount () >= gNextTime)
					{
					if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
						continueFlag = FALSE;
						
					}	// end "if (TickCount () >= nextTime)" 
				
						// Update count of line for status update.							
						
				lineCount++;
				
						// Update status dialog box.												
						
				percentComplete = 100 * numberGetLineCalls/totalGetLineCalls;
				if (percentComplete != lastPercentComplete)
					{
					LoadDItemValue (gStatusDialogPtr, 
										IDC_ShortStatusValue, 
										(SInt32)percentComplete);
					lastPercentComplete = percentComplete;
					
					}	// end "if (percentComplete != lastPercentComplete)" 
				
						// Adjust line depending line interval.								
						
				line += lineInterval;
					
				}	// end "while (lineCount < numberLines && continueFlag)" 
		
					// Close up any File IO Instructions structure.
					
			CloseUpFileIOInstructions (fileIOInstructionsPtr, NULL);
			
			}	// end "for (outsideLoopChannel=0; outsideLoopChannel<..."
			
				// Make sure that the channel list pointer in the FileIOInstructions
				// structure set correctly. It may have changed if the outside channel
				// loop was used.
				
		fileIOInstructionsPtr->numberChannels = numberOutChannels;
		fileIOInstructionsPtr->channelListPtr = (UInt16*)reformatOptionsPtr->channelPtr;
					
				// Flush output buffer if needed.											
		
		if (errCode == noErr && totalIOOutBytes > 0)
			{						
			count = totalIOOutBytes;  
			errCode = WriteOutputDataToFile (outFileInfoPtr,
															outFileStreamPtr,
															ioOutBufferPtr,
															(SInt16*)channelOutOrderPtr,
															numberInsideLoops,		// numberOutChannels,
															lastOutputWrittenLine,
															outOffsetBytes,
															numberLines,
															outBSQOffsetIncrement,
															totalIOOutBytes,
															reformatOptionsPtr,
															numberOutsideLoops);
															
			}	// end "if (errCode == noErr && totalIOOutBytes > 0)" 
															
		if (errCode != noErr)
			continueFlag = FALSE;
		
				// Write channel discriptions at the end of the file if needed.	
		
		if (continueFlag && reformatOptionsPtr->channelDescriptions)	
			WriteChannelDescriptionsAndValues (
										outFileInfoPtr,
										(SInt16*)fileWavelengthOrderPtr,
										reformatOptionsPtr->numberChannels,
										(reformatOptionsPtr->outputFileCode == kAppendToMenuItem));
			
		}	// end "if (continueFlag)" 

	if (continueFlag && outFileInfoPtr->thematicType)
		{  
		supportFileType = kITRLFileType;
		if (outFileInfoPtr->format != kErdas74Type)
			supportFileType = kICLRFileType;
			
		continueFlag = CreateThematicSupportFile (outFileInfoPtr,
																NULL,
																(UInt16)outFileInfoPtr->numberClasses,
																NULL,
																0,
																NULL,
																GetPaletteHandle (),
																newPaletteIndexPtr,
																GetPaletteType (),
																(UInt16)fileInfoPtr->numberClasses,
																kFromDescriptionCode,
																kPaletteHistogramClassNames,
																kClassDisplay,
																kCollapseClass,
																supportFileType);
										
		}	// end "if (continueFlag && outFileInfoPtr->thematicType)" 
		
	CleanUpChangeFormat (newPaletteIndexPtr, 
									symbolToOutputBinPtr, 
									tempBufferPtr);
	
	return (TRUE);
			
}	// end "ChangeFormatToBILorBISorBSQ" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		ChangeImageFileFormat
//
//	Software purpose:	This routine converts the present active image file
//							to a new file with specified format changes.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/15/1988
//	Revised By:			Larry L. Biehl			Date: 08/02/2013

void ChangeImageFileFormat ()

{
			// Declare local variables & structures	
			
	FileInfoPtr							outFileInfoPtr = NULL;
	
	FileIOInstructionsPtr			fileIOInstructionsPtr,
											fileIOInstructions2Ptr;
											
	ReformatOptionsPtr				reformatOptionsPtr = NULL;	
	
	Handle								outFileInfoHandle = NULL,
											reformatOptionsH = NULL;
	
	time_t								startTime;
	
	UInt16								adjustChannel;
	
	Boolean								continueFlag,
											forceBISFlag,
											returnFlag;
											//savedSwapBytesFlag;

	
			// Initialize local variables.													
			
	continueFlag = TRUE;
	fileIOInstructionsPtr = NULL;
	fileIOInstructions2Ptr = NULL;
	
			// Get a handle to a block of memory to be used for						
			// file information for the new image window.								
			// Lock the memory until the information has been loaded					
			// in. Then get the pointer to the file information block				
		
	continueFlag = GetReformatAndFileInfoStructures (
													&outFileInfoHandle, &reformatOptionsH);
	if (continueFlag)
		{		
		outFileInfoPtr = (FileInfoPtr)GetHandlePointer (outFileInfoHandle, kLock);
		outFileInfoHandle = NULL;	
		
		reformatOptionsPtr =
							(ReformatOptionsPtr)GetHandlePointer (reformatOptionsH, kLock);
			
		if (gImageFileInfoPtr->format != kGAIAType && 
												gImageFileInfoPtr->format != kGAIA2Type)
			reformatOptionsPtr->swapBytesFlag = gImageFileInfoPtr->swapBytesFlag;
			
		reformatOptionsPtr->signedOutputDataFlag = gImageFileInfoPtr->signedDataFlag;
			
		gReformatOptionsPtr = reformatOptionsPtr;
		
		}	// end "if (continueFlag)" 

			// Branch to end of routine if memory is full and do cleanup of		
			// handles.																				
			
	if (continueFlag)
		{
				// Initialize the format information for the output file			
				
		InitializeOutputFileInformation (gImageWindowInfoPtr,
													gImageFileInfoPtr,
													reformatOptionsPtr,
													outFileInfoPtr,
													kDoNotKeepSameFormatAsInput);	
													
				// Get Changes that the user wants to make in the file format		
				
		if (ChangeImageFormatDialog (
									gImageFileInfoPtr, outFileInfoPtr, reformatOptionsPtr))
			{
					// List the processor name, date and time.							
				
			gOutputForce1Code = 0x0001;
			continueFlag = ListHeaderInfo (NULL, 
														kLImageInfo, 
														&gOutputForce1Code, 
														kNoStatisticsUsed, 
														continueFlag);
														
					// Update parameters in the structure for the output file.		
			
			UpdateOutputFileStructure (outFileInfoPtr, 
													reformatOptionsPtr, 
													gImageFileInfoPtr->startLine, 
													gImageFileInfoPtr->startColumn,
													gImageFileInfoPtr->mapProjectionHandle);
			
			if (outFileInfoPtr->thematicType)
				{	
						// Verify that we have a palette for any thematic images.
												
				CMPaletteInfo paletteObject = GetPaletteHandle ();
				if (paletteObject == NULL)
					{
					DisplayAlert (kErrorAlertID, 
										kStopAlert, 
										kAlertStrID, 
										IDS_Alert127,
										0, 
										NULL);
										
					continueFlag = FALSE;
																									
					}	// end "if (paletteObject == NULL)"
																									
				}	// end "if (outFileInfoPtr->thematicType)"
				
					// Determine if BIS order is to be forced.
					
			forceBISFlag = (outFileInfoPtr->bandInterleave == kBIS);
			if (reformatOptionsPtr->transformDataCode == kFunctionOfChannels &&
					(reformatOptionsPtr->functionCode == kFunctionMedian ||
							reformatOptionsPtr->functionCode == kFunctionKthSmallestElement))
				forceBISFlag = TRUE;
			
					// Get pointer to memory to use to read an image file line into. 																		
			
			if (continueFlag)	 						
				continueFlag = GetIOBufferPointers (
														&gFileIOInstructions[0],
														gImageWindowInfoPtr,
														gImageLayerInfoPtr,
														gImageFileInfoPtr,
														&gInputBufferPtr, 
														&gOutputBufferPtr,
														reformatOptionsPtr->columnStart,
														reformatOptionsPtr->columnEnd,
														reformatOptionsPtr->columnInterval,
														reformatOptionsPtr->numberChannels,
														(UInt16*)reformatOptionsPtr->channelPtr, 
														kPackData, 
														forceBISFlag, 
														reformatOptionsPtr->forceByteCode,
														kDoNotAllowForThreadedIO,
														&fileIOInstructionsPtr);
			
					// If the request is to adjust the selected channels by a selected
					// channel check if separate read of the file will be required. This
					// occurs if the the selected channel to adjust the channels by is not
					// included in the selected channels to be adjust.
					
			if (reformatOptionsPtr->transformDataCode == kAdjustChannelsByChannel)
				{
						// Determine if adjust selected channel is included in the selected
						// channels to be adjusted. Note that channelPtr list is 0 based
						// while transformAdjustSelectedChannel is 1 based.
				
				reformatOptionsPtr->locationInList = CheckIfValueInList (
										reformatOptionsPtr->channelPtr,
										reformatOptionsPtr->numberChannels,
										reformatOptionsPtr->transformAdjustSelectedChannel - 1);
						
				
				}	// end "if (...->transformDataCode == kAdjustChannelsByChannel)"
				
			if (continueFlag && reformatOptionsPtr->locationInList < 0)
				{
						// Need to set up a second set of buffers to read the data for the 
						// channel being used to adjust the selected set of channels.
						
				adjustChannel = reformatOptionsPtr->transformAdjustSelectedChannel - 1;
				continueFlag = GetIOBufferPointers (&gFileIOInstructions[1],
																gImageWindowInfoPtr,
																gImageLayerInfoPtr,
																gImageFileInfoPtr,
																&gInputBuffer2Ptr, 
																&gOutputBuffer2Ptr,
																reformatOptionsPtr->columnStart,
																reformatOptionsPtr->columnEnd,
																reformatOptionsPtr->columnInterval,
																1,
																&adjustChannel, 
																kPackData, 
																forceBISFlag, 
																reformatOptionsPtr->forceByteCode,
																kDoNotAllowForThreadedIO,
																&fileIOInstructions2Ptr);
				
				}	// end "if (continueFlag && reformatOptionsPtr->locationInList < 0)"
	
					// Get buffer for output data.											
					
			if (continueFlag)
				continueFlag = GetReformatOutputBuffer (outFileInfoPtr, reformatOptionsPtr);
		
				// Branch to end of routine if memory is full and do cleanup of	
				// handles.																			
				
			if (continueFlag)
				{		
						// Make certain that reformatting stop line and column		
						// are compatible with the requested interval.  They 			
						// need to end on an even multiple.									
						
				reformatOptionsPtr->lineEnd = reformatOptionsPtr->lineStart +
							(outFileInfoPtr->numberLines-1) *
											reformatOptionsPtr->lineInterval;
						
				reformatOptionsPtr->columnEnd = reformatOptionsPtr->columnStart +
							(outFileInfoPtr->numberColumns-1) *
											reformatOptionsPtr->columnInterval;
					
						// Set up start and stop line depending on user request.	
				
				if (reformatOptionsPtr->bottomToTop)
					{
					reformatOptionsPtr->startLine = reformatOptionsPtr->lineEnd;
					reformatOptionsPtr->stopLine =
														reformatOptionsPtr->lineStart - 1;
														
					}	// if (reformatOptionsPtr->bottomToTop) 
					
				else	// !reformatOptionsPtr->bottomToTop 
					{
					reformatOptionsPtr->startLine = reformatOptionsPtr->lineStart;		
					reformatOptionsPtr->stopLine = reformatOptionsPtr->lineEnd + 1;
					
					}	// end "else !reformatOptionsPtr->bottomToTop" 
					
						// Set up start and stop column depending on user 				
						//	request.  Note that the start and stop columns				
						// represent input file columns reference to a 0 column		
						// starting point.  Only the requested column range is		
						// read from the input file.											
				
				if (reformatOptionsPtr->rightToLeft)
					{
					reformatOptionsPtr->startColumn =
											reformatOptionsPtr->columnEnd -
															reformatOptionsPtr->columnStart;
					reformatOptionsPtr->stopColumn = 0;
					
					}	// if (reformatOptionsPtr->rightToLeft) 
					
				else	// !reformatOptionsPtr->rightToLeft 
					{
					reformatOptionsPtr->startColumn = 0;	
					reformatOptionsPtr->stopColumn =
											reformatOptionsPtr->columnEnd -
														reformatOptionsPtr->columnStart;
					
					}	// end "else !reformatOptionsPtr->rightToLeft" 
						
						// Set up the disk file to be used for the new reformatted	
						// image file.																
						
				continueFlag = 
						!GetReformatOutputFile (outFileInfoPtr, reformatOptionsPtr);
						
				}	// end "if (continueFlag)"  
			
			if (continueFlag)
				{
						// Change cursor to watch cursor until done with process.	
				
				MSetCursor (kWait);
					
						// Get status dialog. Also call routine to do					
						// activate/update events.												
					
				gStatusDialogPtr = GetStatusDialog (kShortStatusInfoID, FALSE);
				if (gStatusDialogPtr)
					{
							// Initialize variables for lowest and highest value in	
							// case it is used.													
					
					reformatOptionsPtr->minimumValue = DBL_MAX;
					reformatOptionsPtr->maximumValue = (double)-DBL_MAX;
					
							// Lock Principal Component information if it will be 	
							// used.																	
					
					if (reformatOptionsPtr->transformDataCode == kCreatePCImage)
						{
						reformatOptionsPtr->eigenValuePtr = (double*)GetHandlePointer (
															reformatOptionsPtr->eigenValueHandle,
															kLock);
						reformatOptionsPtr->eigenValuePtr = 
									&reformatOptionsPtr->eigenValuePtr[
												reformatOptionsPtr->numberPCComponents];
							
						reformatOptionsPtr->eigenVectorPtr = (double*)GetHandlePointer (
														reformatOptionsPtr->eigenVectorHandle, 
														kLock);
														
						if (gTransformationMatrix.createdByCode < 16)
							{
							reformatOptionsPtr->transformOffset = 0;
							if (!outFileInfoPtr->signedDataFlag)
								reformatOptionsPtr->transformOffset = 
														(outFileInfoPtr->maxUsableDataValue+1)/2;
							
							}	// end "if (gTransformationMatrix.createdByCode < 16)"
																
						else	// gTransformationMatrix.createdByCode >= 16
							reformatOptionsPtr->transformOffset = 0;
						
						}	// end "if (...->transformDataCode == kCreatePCImage)" 
						
							// Save the input swap byte flag and set the input			
							// file swap bytes flag to that requested by the			
							// reformat options.  The swapping is done when the		
							// input data is read in.			
							// If the data is to be transformed in some way then make
							// sure that the data is swapped or not-swapped when read 
							// in such that the math using the data values are correct.									
							// If the data file is being converted to GAIA format		
							// and the input file is only one byte, then set the		
							// swap flag to TRUE to force the output data to be		
							// FALSE.															
					/*			
					savedSwapBytesFlag = gImageFileInfoPtr->swapBytesFlag;
					if (reformatOptionsPtr->transformDataCode != kNoTransform)
						reformatOptionsPtr->swapBytesFlag = FALSE;
						
					gImageFileInfoPtr->swapBytesFlag = reformatOptionsPtr->swapBytesFlag;
					*/
					if (outFileInfoPtr->format == kGAIAType && 
													gImageFileInfoPtr->numberBytes == 1)
						gImageFileInfoPtr->swapBytesFlag = TRUE;
					
					//if (reformatOptionsPtr->swapBytesFlag)	
					//	outFileInfoPtr->swapBytesFlag = !savedSwapBytesFlag;
						
					outFileInfoPtr->swapBytesFlag = reformatOptionsPtr->swapBytesFlag;
					
					MGetString ((UCharPtr)gTextString, kReformatStrID, IDS_PercentComplete);
					LoadDItemString (
								gStatusDialogPtr, IDC_ShortStatusText, (Str255*)gTextString);
					LoadDItemValue (gStatusDialogPtr, IDC_ShortStatusValue, (SInt32)0);
					ShowStatusDialogWindow (kShortStatusInfoID);
					
							// Everything has now been set up.								
							// Do reformatting for output files here.				
							
					startTime = time (NULL);
														
					if (outFileInfoPtr->bandInterleave == kBIL ||
								outFileInfoPtr->bandInterleave == kBIS ||
										outFileInfoPtr->bandInterleave == kBSQ ||
											outFileInfoPtr->bandInterleave == kBNonSQ)
						returnFlag = ChangeFormatToBILorBISorBSQ (
													fileIOInstructionsPtr,
													fileIOInstructions2Ptr,
													outFileInfoPtr, 
													reformatOptionsPtr,
													forceBISFlag);
													
							// Restore the input swap byte flag.							
							
					//gImageFileInfoPtr->swapBytesFlag = savedSwapBytesFlag;
				
								// Unlock Principal Component information if it was 	
								// used.																
						
					if (reformatOptionsPtr->transformDataCode == kCreatePCImage)
						{
						CheckAndUnlockHandle (reformatOptionsPtr->eigenVectorHandle);
						CheckAndUnlockHandle (reformatOptionsPtr->eigenValueHandle);
						
						}	// end "if (...->transformDataCode == kCreatePCImage)" 
					
							// List Reformat information.										
							
					continueFlag = ListReformatResultsInformation (
													reformatOptionsPtr, outFileInfoPtr);
				
							// List the CPU time taken for reformatting.					
								
					continueFlag = ListCPUTimeInformation (NULL, 
																	continueFlag, 
																	startTime);
								
							// Dispose of status dialog										
							
					CloseStatusDialog (TRUE);
						
					}	// end "if (gStatusDialogPtr)" 
			
						// Reset cursor back to arrow to indicate that process		
						// is done																	
					
				MInitCursor ();
					
				}	// end "if (continueFlag)" 
				
					// Dispose of the IO buffers. 		
										
			DisposeIOBufferPointers (fileIOInstructionsPtr,
												&gInputBufferPtr, 
												&gOutputBufferPtr);		
										
			DisposeIOBufferPointers (fileIOInstructions2Ptr,
												&gInputBuffer2Ptr, 
												&gOutputBuffer2Ptr);
				
			}	// end "if (ChangeImageFormatDialog (gImageFileInfoPtr, ... "
			
		}	// end "if (continueFlag)" 
		
			// Save the algebraic formula information in case it can be used again
			// in a later call.
	
	if (reformatOptionsPtr->transformDataCode == kTransformChannels)
		SaveAlgebraicTransformationFunction (
								gImageWindowInfoPtr->totalNumberChannels,
								gImageFileInfoPtr->instrumentCode,
								gReformatOptionsPtr);
							
	ReleaseReformatOutputFileInfoAndBuffers (reformatOptionsPtr, gImageFileInfoPtr);
	
	gReformatOptionsPtr = NULL;
					
}	// end "ChangeImageFileFormat" 


                       
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ChangeImageFormatDialog
//
//	Software purpose:	This routine gets the image file format changes
//							that the user wants to use from a dialog box.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 10/05/1988
//	Revised By:			Larry L. Biehl			Date: 10/22/2018

Boolean ChangeImageFormatDialog (
				FileInfoPtr							fileInfoPtr, 
				FileInfoPtr							outFileInfoPtr, 
				ReformatOptionsPtr				reformatOptionsPtr)

{                
	Boolean								OKFlag = FALSE;
	
#if defined multispec_mac		
	UInt8									inputBandInterleaveString[64],
											inputDataValueTypeString[64],
											tiffMenuNameString[16];
								
	DialogSelectArea					dialogSelectArea;
	
	DialogPtr							dialogPtr;
								
	Handle								okHandle,
											theHandle;
	
	Rect									bytesBox,
											fileBox,
											headerBox,
											interleaveBox,
											theBox;
			
	UserItemUPP							drawDataValueTypePopUpPtr = NULL,
											drawHeaderOptionsPopUpPtr = NULL,
											drawOutputFilePopUpPtr = NULL;
	
	SInt32								theNum;
	
	SInt16								itemHit,
											itemHit2,
											noTransformDataValueTypeSelection,
											numChars,
											recommendNumberOfBits, 
											savedDataValueTypeSelection,
											suggestedDataValueTypeSelection,
											theType;
											
	Boolean								bottomToTopFlag,
											channelDescriptionAllowedFlag,
											channelThematicDisplayFlag,
											GAIAFormatAllowedFlag,
											modalDone,
											outputInWavelengthOrderFlag,
											outputInWavelengthOrderAllowedFlag,
											rightToLeftFlag,
											savedChannelDescriptionFlag,
											sessionUserSetDataValueTypeSelectionFlag,
											setChannelDescriptionFlag,
											dialogWindowShownFlag,
											swapBytesFlag;
	

			// Get the modal dialog for the image file format description			
				
	dialogPtr = LoadRequestedDialog (kChangeImageFormatDialogID, kCopyScrap, 1, 2);
	if (dialogPtr == NULL)													
																						return (FALSE);
		
			// Intialize local user item proc pointers.
			
	drawHeaderOptionsPopUpPtr = NewUserItemUPP (DrawHeaderOptionsPopUp);
	drawOutputFilePopUpPtr = NewUserItemUPP (DrawOutputFilePopUp);
	drawDataValueTypePopUpPtr = NewUserItemUPP (DrawDataValueTypePopUp);
								
	setChannelDescriptionFlag = FALSE;

	ChangeImageFormatDialogInitialize (dialogPtr,
													gImageWindowInfoPtr,
													fileInfoPtr,
													reformatOptionsPtr,
													&dialogSelectArea,
													inputBandInterleaveString,
													inputDataValueTypeString,
													tiffMenuNameString,
													&gOutputFileSelection,
													&gBandInterleaveSelection,
													&gDataValueTypeSelection,
													&savedDataValueTypeSelection,
													&gChannelSelection,
													&bottomToTopFlag,
													&rightToLeftFlag,
													&swapBytesFlag,
													&channelDescriptionAllowedFlag,
													&savedChannelDescriptionFlag,
													&outputInWavelengthOrderAllowedFlag,
													&outputInWavelengthOrderFlag,
													&gHeaderOptionsSelection,
													&GAIAFormatAllowedFlag,
													&channelThematicDisplayFlag);
													
			// Save handle for the OK button for use later.								
			
	GetDialogItem (dialogPtr, 1,  &theType, &okHandle, &theBox);
	
	LoadDItemString (dialogPtr, 13, (Str255*)inputBandInterleaveString);
	
	LoadDItemString (dialogPtr, 17, (Str255*)inputDataValueTypeString);
	
			// Items 18 & 19: "Output file" popup box.									
				
	SetDialogItemDrawRoutine (dialogPtr, 19, &fileBox, drawOutputFilePopUpPtr);
	
			// Item 29: Transform data check box.											
 
	SetDLogControl (dialogPtr, IDC_TransformData, 0);
	if (reformatOptionsPtr->transformDataCode != kNoTransform)
		SetDLogControl (dialogPtr, IDC_TransformData, 1);
	
			//	Set the draw routine for the bytes per data value popup box.		
			
	SetDialogItemDrawRoutine (dialogPtr, 33, &bytesBox, drawDataValueTypePopUpPtr);
	
	DisableMenuItem (gPopUpDataValueTypeMenu, k4BitUnsignedIntegerMenuItem);
	noTransformDataValueTypeSelection = savedDataValueTypeSelection;
	sessionUserSetDataValueTypeSelectionFlag = FALSE;
	
			//	Set the draw routine for the band interleave popup box.				
			
	SetDialogItemDrawRoutine (
							dialogPtr, 35, &interleaveBox, gDrawBandInterleavePopUpPtr);
					
			// Make certain that the band interleave for BSQ is enabled				
			
	EnableMenuItem (gPopUpBandInterleaveMenu, kBSQ);
					
			// Disable the Band Non-sequential, Band Non-sequential Blocked, and
			// Band Interleaved by Block options.				
			
	DisableMenuItem (gPopUpBandInterleaveMenu, kBNonSQ);
	DisableMenuItem (gPopUpBandInterleaveMenu, kBNonSQBlocked);
	DisableMenuItem (gPopUpBandInterleaveMenu, kBIBlock);
	
			//	Set the draw routine for the channel popup box.							
			
	if (gImageWindowInfoPtr->windowType == kImageWindowType)
		SetDialogItemDrawRoutine (dialogPtr, 37, gDrawChannelsPopUpPtr);
	
			// "Invert bottom to top" check box.											
 
	SetDLogControl (dialogPtr, 39, bottomToTopFlag);
	
			// "Invert right to left" check box.											
 
	SetDLogControl (dialogPtr, 40, rightToLeftFlag);
	
			// "Swap bytes" check box.															
 
	if (gImageWindowInfoPtr->numberBytes >= 2)
		SetDLogControl (dialogPtr, 41, swapBytesFlag);
		
	if (gImageWindowInfoPtr->numberBytes == 1 || 
						fileInfoPtr->format == kGAIAType || 
													fileInfoPtr->format == kGAIA2Type)  
		SetDLogControlHilite (dialogPtr, 41, 255);
	
			// "Write channel description" check box.	
			
	SetDLogControl (dialogPtr, 42, savedChannelDescriptionFlag);
	
			// "Image Header" popup box.														
	
	SetDialogItemDrawRoutine (dialogPtr, 44, &headerBox, drawHeaderOptionsPopUpPtr);
								
	if (!GAIAFormatAllowedFlag)
		DisableMenuItem (gPopUpHeaderOptionsMenu, kGAIAMenuItem);
		
			// Center the dialog and then show it.		
			
	dialogWindowShownFlag = FALSE;
	if (gMultiSpecWorkflowInfo.workFlowCode != 1 && 
														gMultiSpecWorkflowInfo.workFlowCode != 2)									
		{
		ShowDialogWindow (dialogPtr, kChangeImageFormatDialogID, kSetUpDFilterTable);
		
		dialogWindowShownFlag = TRUE;	
	
				// Set default text selection to first edit text item						
			
		SelectDialogItemText (dialogPtr, 23, 0, INT16_MAX);
		
		
		itemHit = 0;
	
		}	// end "if (gMultiSpecWorkflowInfo.workFlowCode != 1 && ..."
		
	else	// gMultiSpecWorkflowInfo.workFlowCode == 1 || ...
		{
		reformatOptionsPtr->transformDataCode = kFunctionOfChannels;
		gDataValueTypeSelection = k2ByteSignedIntegerMenuItem;
		itemHit = gMultiSpecWorkflowInfo.workFlowCode;
		
		}	// end "else gMultiSpecWorkflowInfo.workFlowCode == 1 || ..."
								
	modalDone = FALSE;
	do 
		{
		if (dialogWindowShownFlag)
			ModalDialog (gProcessorDialogFilterPtr, &itemHit);
			
		if (itemHit > 2)
			{
					// If itemHit was a radio button make appropriate control		
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
				case 19:				// "Output file" popup box 
					
					itemHit2 = StandardPopUpMenu (dialogPtr, 
															18, 
															19, 
															gPopUpOutputFileMenu, 
															gOutputFileSelection,
															kPopUpOutputFileMenuID);
													
					if (itemHit2 != 0)
						gOutputFileSelection = itemHit2;
	
							// Make certain that the correct label is drawn in the	
							// output file pop up box.											
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
					
				case 45:				// Entire area to selected area switch.
				case 46:				// Entire area to selected area switch. (Appearance Manager)			
				case 23:				//	 lineStart  
				case 24:				//	 lineEnd  
				case 25:				//	 lineInterval  	
				case 26:				//	 columnStart  
				case 27:				//	 columnEnd  
				case 28:				//	 columnInterval
					DialogLineColumnHits (&dialogSelectArea,
													dialogPtr, 
													itemHit,
													theHandle,
													theNum);
					break;
					
				case 29:				// Check box for transform data
					HiliteControl ((ControlHandle)okHandle, 255);
					if (ChangeImageTransformDialog (
												reformatOptionsPtr, &recommendNumberOfBits))
						{
						setChannelDescriptionFlag = TRUE;

						if (reformatOptionsPtr->transformDataCode == kNoTransform)
							{
							SetDLogControl (dialogPtr, IDC_TransformData, 0);
							HideDialogItem (dialogPtr, IDC_MinimumBitsPrompt);
							gDataValueTypeSelection = noTransformDataValueTypeSelection;
							
							if (outputInWavelengthOrderAllowedFlag)
								{
								SetDLogControl (dialogPtr,
														IDC_OutputInWavelengthOrder,
														outputInWavelengthOrderFlag);
								SetDLogControlHilite (
														dialogPtr, IDC_OutputInWavelengthOrder, 0);
								
								}	// end "if (outputInWavelengthOrderAllowedFlag)"
							
							//if (gImageWindowInfoPtr->numberBytes >= 2)
							//	SetDLogControlHilite (dialogPtr, 41, 0);
							
							}	// end "if (...->transformDataCode == kNoTransform)" 
							
						else	// reformatOptionsPtr->transformDataCode != kNoTransform 
							{
							SetDLogControl (dialogPtr, IDC_TransformData, 1);
							ShowDialogItem (dialogPtr, IDC_MinimumBitsPrompt);
							
							if (recommendNumberOfBits > 16)
								{
								numChars = sprintf ((char*)&gTextString2[1],
															"Recommended minimum data bits: %d-Real",
															recommendNumberOfBits);
											
								suggestedDataValueTypeSelection = k4ByteRealMenuItem;
											
								}	// end "if (recommendNumberOfBits > 16)"
							
							else	// recommendNumberOfBits <= 16
								{
								numChars = sprintf ((char*)&gTextString2[1],
															"Recommended minimum data bits: %d",
															recommendNumberOfBits);
											
								suggestedDataValueTypeSelection = k2ByteSignedIntegerMenuItem;
											
								}	// end "else recommendNumberOfBits <= 16"
							
									// The suggested data value type selection will be forced
									// if the user has not set it before and the suggested
									// setting contains more bits than the previous setting.
									// Also the header format setting will be verified to match
									// the new data value type setting.
									
							if (!sessionUserSetDataValueTypeSelectionFlag &&
									!reformatOptionsPtr->userSetDataValueTypeSelectionFlag &&
									gDataValueTypeSelection == savedDataValueTypeSelection &&
									suggestedDataValueTypeSelection > gDataValueTypeSelection)
								{
								gDataValueTypeSelection = suggestedDataValueTypeSelection;
							
								ChangeImageFormatDialogUpdateHeaderMenu (
																dialogPtr,
																fileInfoPtr,
																tiffMenuNameString,
																&gOutputFileSelection,
																&gBandInterleaveSelection,
																&gHeaderOptionsSelection,
																gPopUpOutputFileMenu,
																gPopUpHeaderOptionsMenu,
																reformatOptionsPtr->transformDataCode,
																gDataValueTypeSelection,
																GAIAFormatAllowedFlag);
																	
								}	// end "if (!sessionUserSetDataValueTypeSelectionFlag && ..."
												
							gTextString2[0] = numChars;			
							LoadDItemString (
									dialogPtr, IDC_MinimumBitsPrompt, (Str255*)gTextString2);
							
							//SetDLogControl (dialogPtr, 41, 0);
							//SetDLogControlHilite (dialogPtr, 41, 255);
							
							if (outputInWavelengthOrderAllowedFlag)
								{
								SetDLogControl (dialogPtr,
														IDC_OutputInWavelengthOrder,
														0);
								SetDLogControlHilite (
													dialogPtr, IDC_OutputInWavelengthOrder, 255);
								
								}	// end "if (outputInWavelengthOrderAllowedFlag)"
							
							}	// end "else ...->transformDataCode != kNoTransform" 
							
						InvalWindowRect (GetDialogWindow (dialogPtr), &bytesBox);

								// Input channels will be defined by the transformation.

						if (reformatOptionsPtr->transformDataCode == kNoTransform ||
								reformatOptionsPtr->transformDataCode == kAdjustChannel ||
									reformatOptionsPtr->transformDataCode ==
																			kAdjustChannelsByChannel ||
										reformatOptionsPtr->transformDataCode ==
																						kFunctionOfChannels)
							ShowDialogItems (dialogPtr, 36, 37);

						else	// ...->transformDataCode == kTransformChannels ||
									//			transformDataCode == kCreatePCImage)
							HideDialogItems (dialogPtr, 36, 37);
							
						}	// end "if (ChangeImageTransformDialog..." 
						
					HiliteControl ((ControlHandle)okHandle, 0);
					break;
					
				case 33:		// Data value type popup
					if (gDataValueTypeSelection > 0)
						{
						itemHit2 = StandardPopUpMenu (dialogPtr, 
																itemHit-1, 
																itemHit, 
																gPopUpDataValueTypeMenu, 
																gDataValueTypeSelection, 
																kPopUpDataValueTypeMenuID);
														
						if (itemHit2 > 0 && itemHit2 != gDataValueTypeSelection)
							{
							gDataValueTypeSelection = itemHit2;
							
							ChangeImageFormatDialogUpdateHeaderMenu (
																dialogPtr,
																fileInfoPtr,
																tiffMenuNameString,
																&gOutputFileSelection,
																&gBandInterleaveSelection,
																&gHeaderOptionsSelection,
																gPopUpOutputFileMenu,
																gPopUpHeaderOptionsMenu,
																reformatOptionsPtr->transformDataCode,
																gDataValueTypeSelection,
																GAIAFormatAllowedFlag);
							
							}	// end "if (itemHit2 > 0 && itemHit2 != ...)"
							
						if (reformatOptionsPtr->transformDataCode == kNoTransform)
							noTransformDataValueTypeSelection = gDataValueTypeSelection;
							
								// Indicate that the user did make a selection in the box even
								// if it was the same as the original
						
						sessionUserSetDataValueTypeSelectionFlag = TRUE;
							
						}	// end "if (gDataValueTypeSelection > 0)" 
					break;
					
				case 35:				// Band interleave popup 
					itemHit2 = StandardPopUpMenu (dialogPtr, 
															itemHit-1, 
															itemHit, 
															gPopUpBandInterleaveMenu, 
															gBandInterleaveSelection, 
															kPopUpBandInterleaveMenuID);
													
					if (itemHit2 > 0 && gBandInterleaveSelection != itemHit2)
						{
						gBandInterleaveSelection = itemHit2;

						ChangeImageFormatDialogVerifyHeaderSetting (
																	dialogPtr,
																	fileInfoPtr,
																	gBandInterleaveSelection,
																	gDataValueTypeSelection,
																	gPopUpOutputFileMenu,
																	gPopUpHeaderOptionsMenu,
																	&gOutputFileSelection,
																	&gHeaderOptionsSelection);
						
						}	// end "if (itemHit2 > 0 && ..."
					break;
					
				case 37:				//	 Channels  	
					itemHit = StandardPopUpMenu (dialogPtr, 
															itemHit-1, 
															itemHit, 
															gPopUpAllSubsetMenu, 
															gChannelSelection, 
															kPopUpAllSubsetMenuID);
					if (itemHit == 2)
						{
								// Subset of channels to be used.							
								
						HiliteControl ((ControlHandle)okHandle, 255);
						
						ChannelsDialog ((SInt16*)&reformatOptionsPtr->numberChannels,
												reformatOptionsPtr->channelPtr,
												gImageLayerInfoPtr,
												fileInfoPtr,
												1,
												kNoTransformation,
												NIL,
												gImageWindowInfoPtr->totalNumberChannels,
												gChannelSelection);
						
						HiliteControl ((ControlHandle)okHandle, 0);
													
						if (reformatOptionsPtr->numberChannels ==
														gImageWindowInfoPtr->totalNumberChannels)
							itemHit = 1;
						
						}	// end "if (itemHit == 2)" 
					
					if (itemHit != 0)
						gChannelSelection = itemHit;
	
							// Make certain that the correct label is drawn in the	
							// channel pop up box.												
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
					
				case 39:				// "Invert bottom to top" Check box 
				case 40:				// "Invert right to left" Check box 
				case 41:				// "Swap Bytes" Check box
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					break;

				case 42:				// "Write channel descriptions" Check box 
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					savedChannelDescriptionFlag = !savedChannelDescriptionFlag;
					break;
					
				case 44:				// "Write Header" popup box
					itemHit2 = StandardPopUpMenu (dialogPtr, 
															itemHit-1, 
															itemHit, 
															gPopUpHeaderOptionsMenu, 
															gHeaderOptionsSelection,
															kPopUpHeaderOptionsMenuID);
													
					if (itemHit2 > 0 && gHeaderOptionsSelection != itemHit2)
						{
						gHeaderOptionsSelection = itemHit2;
						
						ChangeImageFormatDialogUpdateHeaderOptions (
																	dialogPtr,
																	fileInfoPtr,
																	gHeaderOptionsSelection,
																	&gOutputFileSelection,
																	&gBandInterleaveSelection,
																	&gDataValueTypeSelection,
																	&setChannelDescriptionFlag,
																	gPopUpOutputFileMenu,
																	channelDescriptionAllowedFlag,
																	channelThematicDisplayFlag);
																	
						}	// end "if (itemHit2 > 0 && ..."
					break;

				case 47:				// "Output in wavelength order" Check box
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					outputInWavelengthOrderFlag = !outputInWavelengthOrderFlag;
					break;
						
				}	// end "switch (itemHit)" 
				
			if (setChannelDescriptionFlag)
				{
				if (reformatOptionsPtr->transformDataCode == kNoTransform ||
									reformatOptionsPtr->transformDataCode == kAdjustChannel)
					{
					if (channelDescriptionAllowedFlag)
						{
						SetDLogControl (dialogPtr, 42, savedChannelDescriptionFlag);
						SetDLogControlHilite (dialogPtr, 42, 0);
						
						}	// end "if (channelDescriptionAllowedFlag)" 

					}	// end "if (...->transformDataCode == kNoTransform || ...)"

				else	// ...->transformDataCode == kTransformChannels ||
							//			transformDataCode == kCreatePCImage)
					{
					SetDLogControl (dialogPtr, 42, 0);
					SetDLogControlHilite (dialogPtr, 42, 255);

					}	// end "else ...->transformDataCode == kTransformChannels ..."
					
				setChannelDescriptionFlag = FALSE;
				
				}	// end "if (setChannelDescriptionFlag)" 
				
			}	// end "if (itemHit > 2)" 
			
		else	// itemHit <= 2 
			{
					// If item hit is 1, check if display line-column values make 	
					// sense.  If they don't, sound an alert and make item hit 		
					// equal to 0 to allow user to make changes.							
					
			if (itemHit == 1)
				itemHit = CheckLineColValues (&dialogSelectArea,
														reformatOptionsPtr->lineStart,
														reformatOptionsPtr->columnStart,
														dialogPtr);
			
			if	(itemHit == 1)      // User selected OK for information 
				{
				modalDone = TRUE;  

				OKFlag = ChangeImageFormatDialogOK (
														dialogPtr,
														gImageWindowInfoPtr,
														gImageLayerInfoPtr,
														gImageFileInfoPtr,
														outFileInfoPtr,
														reformatOptionsPtr,
														&dialogSelectArea,
														gOutputFileSelection,
														gBandInterleaveSelection,
														gDataValueTypeSelection,
														sessionUserSetDataValueTypeSelectionFlag,
														gChannelSelection,
														GetDLogControl (dialogPtr, 39),
														GetDLogControl (dialogPtr, 40),
														GetDLogControl (dialogPtr, 41),
														GetDLogControl (dialogPtr, 42),
														GetDLogControl (dialogPtr, 47),
														gHeaderOptionsSelection,
														channelThematicDisplayFlag);
				
				}	// end "if (itemHit == 1)" 
				
			if	(itemHit == 2)      // User selected Cancel for information 
				{
				modalDone = TRUE;
				OKFlag = FALSE;
				
				}	// end "if	(itemHit == 2)" 
				
			}	// end "else itemHit <= 2" 
				
		} while (!modalDone);
		
	DisposeUserItemUPP (drawDataValueTypePopUpPtr);	
	DisposeUserItemUPP (drawHeaderOptionsPopUpPtr);
	DisposeUserItemUPP (drawOutputFilePopUpPtr);
		
	CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
#endif	// defined multispec_mac 

	#if defined multispec_win 		
		CMChangeFormatDlg*		dialogPtr = NULL; 
		
		TRY
			{ 
			dialogPtr = new CMChangeFormatDlg (); 
			                                                        
			OKFlag = dialogPtr->DoDialog (outFileInfoPtr, reformatOptionsPtr); 
		                      
			delete dialogPtr;
			}
			
		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			}
		END_CATCH_ALL  
	#endif	// defined multispec_win
	
	#if defined multispec_wx
		CMChangeFormatDlg* dialogPtr = NULL;

		dialogPtr = new CMChangeFormatDlg (reformatOptionsPtr, NULL);

		OKFlag = dialogPtr->DoDialog (outFileInfoPtr);

		delete dialogPtr;
	#endif	// defined multispec_win

	return (OKFlag);
	
}	// end "ChangeImageFormatDialog"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ChangeImageFormatDialogInitialize
//
//	Software purpose:	
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 01/21/2006
//	Revised By:			Larry L. Biehl			Date: 05/12/2020

void ChangeImageFormatDialogInitialize (
				DialogPtr							dialogPtr,
				WindowInfoPtr						windowInfoPtr,
				FileInfoPtr							fileInfoPtr,
				ReformatOptionsPtr				reformatOptionsPtr,
				DialogSelectArea*					dialogSelectAreaPtr,
				UCharPtr								inputBandInterleaveStringPtr,
				UCharPtr								inputDataValueTypeStringPtr,
				UCharPtr								tiffMenuNameStringPtr,
				SInt16*								outputFileSelectionPtr,
				SInt16*								bandInterleaveSelectionPtr,
				SInt16*								dataValueTypeSelectionPtr,
				SInt16*								savedDataValueTypeSelectionPtr,
				SInt16*								channelSelectionPtr,
				Boolean*								bottomToTopFlagPtr,
				Boolean*								rightToLeftFlagPtr,
				Boolean*								swapBytesFlagPtr,
				Boolean*								channelDescriptionAllowedFlagPtr,
				Boolean*								savedChannelDescriptionFlagPtr,
				Boolean*								outputInWavelengthOrderAllowedFlagPtr,
				Boolean*								outputInWavelengthOrderFlagPtr,
				SInt16*								headerOptionsSelectionPtr,
				Boolean*								GAIAFormatAllowedFlagPtr,
				Boolean*								channelThematicDisplayFlagPtr)

{	
	SInt16								entireIconItem,
											thematicListType;
	
	UInt16								dataTypeCode,
											numberBits;
	
	Boolean								mapInfoExistsFlag,
											signedDataFlag;
		 
	#if defined multispec_wx
		wxChoice* comboBoxPtr;
				
		comboBoxPtr = (wxChoice*)dialogPtr->FindWindow (IDC_ChangeHeader);
	#endif	// defined multispec_win

	#if defined multispec_win  
		CComboBox*						comboBoxPtr;

		comboBoxPtr = (CComboBox*)dialogPtr->GetDlgItem (IDC_ChangeHeader);

		USES_CONVERSION;
	#endif
	
			// Initialize selected area structure.			

	entireIconItem = 45;
	if (gAppearanceManagerFlag)
		entireIconItem = 46;
	
	InitializeDialogSelectArea (dialogSelectAreaPtr,
											windowInfoPtr,
											gActiveImageWindow,
											reformatOptionsPtr->columnStart,
											reformatOptionsPtr->columnEnd,
											reformatOptionsPtr->columnInterval,
											reformatOptionsPtr->lineStart,
											reformatOptionsPtr->lineEnd,
											reformatOptionsPtr->lineInterval,
											23,
											entireIconItem,
											kDontAdjustToBaseImage);

			//	Initialize the image file format change specification dialog		
	
			// item = 5: Input image file name.												
	
	FileStringPtr fileNamePtr =
							(FileStringPtr)GetFileNamePPointerFromFileInfo (fileInfoPtr);
	#if defined multispec_mac
		if (CreateUnicodeStaticTextControl (dialogPtr,
														&fileNamePtr[1],
														fileNamePtr[0],
														IDC_InputFileName, NULL) != noErr)
			LoadDItemString (dialogPtr, IDC_InputFileName, (Str255*)fileNamePtr);
	#endif	// defined multispec_mac 

	#if defined multispec_win || defined multispec_wx
		LoadDItemString (dialogPtr, IDC_InputFileName, (char*)&fileNamePtr[2]);
	#endif
	
			// item = 7: Number of lines in input image.									
			
	LoadDItemValue (dialogPtr,
							IDC_ChangeFormatInLines,
							(SInt32)gImageWindowInfoPtr->maxNumberLines);
	
			// item = 9: Number of columns in input image.								
			
	LoadDItemValue (dialogPtr,
							IDC_ChangeFormatInColumns,
							(SInt32)gImageWindowInfoPtr->maxNumberColumns);
	
			// item = 11: Number of channels in input multispectral image.			
			// 			  Number of classes in input thematic image.					
			
	if (windowInfoPtr->windowType == kImageWindowType)
		{
		LoadDItemStringNumber (kDialogStrID,
										IDS_Dialog17,
										dialogPtr,
										IDC_ChannelsClassesLabel,		// Channels:
										(Str255*)gTextString); 
								
		LoadDItemValue (dialogPtr,
								IDC_ChangeFormatInChan, 
								(SInt32)gImageWindowInfoPtr->totalNumberChannels);
		
		}	// end "if (!fileInfoPtr->thematicType)" 
		
	else	// windowInfoPtr->windowType == kThematicWindowType 
		{
		thematicListType = GetActiveLegendListType ();
		if (thematicListType <= kClassDisplay)
			{
			LoadDItemStringNumber (kDialogStrID,
											IDS_Dialog28,
											dialogPtr,
											IDC_ChannelsClassesLabel,		// Classes:
											(Str255*)gTextString);
							
			LoadDItemValue (dialogPtr, 
									IDC_ChangeFormatInChan, 
									(SInt32)fileInfoPtr->numberClasses);
			
			}	// end "if (thematicListType <= kClassDisplay)"
			
		else	// thematicListType == group display
			{
			LoadDItemStringNumber (kDialogStrID,
											IDS_Dialog29,
											dialogPtr,
											IDC_ChannelsClassesLabel,	// Groups:
											(Str255*)gTextString);
							
			LoadDItemValue (dialogPtr, 
									IDC_ChangeFormatInChan, 
									(SInt32)fileInfoPtr->numberGroups);
			
			}	// end "else thematicListType == group display"
		
		}	// end "else fileInfoPtr->thematicType" 
	
			// Item 13: Input image file format.	
			
	inputBandInterleaveStringPtr[0] = 0;
	inputBandInterleaveStringPtr[1] = 0;
					
	if (gImageWindowInfoPtr->numberImageFiles == 1 && 
														fileInfoPtr->gdalBandInterleave > 0)
		{
		if (fileInfoPtr->gdalBandInterleave == kBIL)
			CopyPToP (inputBandInterleaveStringPtr, (UCharPtr)"\0BIL");
			
		else if (fileInfoPtr->gdalBandInterleave == kBSQ)  
			CopyPToP (inputBandInterleaveStringPtr, (UCharPtr)"\0BSQ");
			
		else if (fileInfoPtr->gdalBandInterleave == kBIS)
			CopyPToP (inputBandInterleaveStringPtr, (UCharPtr)"\0BIS");
		
		}	// end "if (... && fileInfoPtr->gdalBandInterleave > 0)"
				
	else	// fileInfoPtr->gdalBandInterleave <= 0 || ...->numberImageFiles > 1
		{
		if (gImageWindowInfoPtr->bandInterleave == kBIL)
			CopyPToP (inputBandInterleaveStringPtr, (UCharPtr)"\0BIL");
			
		else if (gImageWindowInfoPtr->bandInterleave == kBSQ)  
			CopyPToP (inputBandInterleaveStringPtr, (UCharPtr)"\0BSQ");
			
		else if (gImageWindowInfoPtr->bandInterleave == kBIS)
			CopyPToP (inputBandInterleaveStringPtr, (UCharPtr)"\0BIS");
			
		else if (gImageWindowInfoPtr->bandInterleave == kBNonSQ)
			CopyPToP (inputBandInterleaveStringPtr, (UCharPtr)"\0BNonSQ");
			
		else if (gImageWindowInfoPtr->bandInterleave == kBNonSQBlocked)
			CopyPToP (inputBandInterleaveStringPtr, (UCharPtr)"\0BNonSQBlocked");
			
		else if (gImageWindowInfoPtr->bandInterleave == kBIS)
			CopyPToP (inputBandInterleaveStringPtr, (UCharPtr)"\0BIBlock");
			
		else if (gImageWindowInfoPtr->bandInterleave == kMixed)
			CopyPToP (inputBandInterleaveStringPtr, (UCharPtr)"\0Mixed");
			
		}	// end "else fileInfoPtr->gdalBandInterleave <= 0 || ..."
		
	#if defined multispec_mac  
				// Number of bytes in input image.								
				
		HideDialogItem (dialogPtr, 14);
		HideDialogItem (dialogPtr, 15);
	#endif	// defined multispec_mac  	
	
			// Input file data value type.	
			// Note that *dataValueTypeSelectionPtr will be set later for the output
			// data type.
			
	inputDataValueTypeStringPtr[0] = 0;
	inputDataValueTypeStringPtr[1] = 0;
	
	*dataValueTypeSelectionPtr = FileSpecificationDialogSetDataType (
																			fileInfoPtr->dataTypeCode,
																			fileInfoPtr->numberBits,
																			fileInfoPtr->signedDataFlag);
			
	#if defined multispec_mac  
		GetMenuItemText (gPopUpDataValueTypeMenu, 
								*dataValueTypeSelectionPtr, 
								(UCharPtr)inputDataValueTypeStringPtr);
	#endif	// defined multispec_mac  
                       
	#if defined multispec_win || defined multispec_wx
		MGetString ((UCharPtr)inputDataValueTypeStringPtr,
							0,
							IDS_DataType01+*dataValueTypeSelectionPtr-1,
							63);
	#endif	// defined multispec_win || defined multispec_wx
	
			// "Output file" popup box.
												
	if (reformatOptionsPtr->headerFormat == kTIFFType)
		reformatOptionsPtr->outputFileCode = kNewFileMenuItem;
	
	*outputFileSelectionPtr = reformatOptionsPtr->outputFileCode;
	
	#if defined multispec_mac 
		if (gAppearanceManagerFlag)
			HideDialogItem (dialogPtr, 45);
		else	// !gAppearanceManagerFlag
			HideDialogItem (dialogPtr, 46);
	#endif	// defined multispec_mac  

			// To entire image icon.															
			//	Reformat area																		
			
	LoadLineColumnItems (dialogSelectAreaPtr, 
									dialogPtr, 
									kInitializeLineColumnValues, 
									kIntervalEditBoxesExist,
									1);

			// Handle case where the last transform was to create a PC image but now
			// no transformation matrix exists to do this.

	if (reformatOptionsPtr->transformDataCode == kCreatePCImage)
		{
				// Verify that a transformation matrix still exists

		UInt16						numberEigenvectors;
		EigenvectorInfoExists (kNoProject, NULL, &numberEigenvectors);
		if (numberEigenvectors == 0)
			reformatOptionsPtr->transformDataCode = kNoTransform;
		
		}	// end "if (reformatOptionsPtr->transformDataCode == kCreatePCImage)"
	
			// Transformation is not allowed for thematic type multispectral
			// displays.
	
	*channelThematicDisplayFlagPtr = FALSE;
	if (windowInfoPtr->imageType == kMultispectralImageType &&
							windowInfoPtr->windowType == kThematicWindowType)	
		{
		*channelThematicDisplayFlagPtr = TRUE;
		MHideDialogItem (dialogPtr, IDC_TransformData);
		
		}	// end "if (windowInfoPtr->imageType == kMultispectralImageType && ..."
			
	#if defined multispec_mac  
				//	bits per data value popup box is not used now.	
				
		HideDialogItem (dialogPtr, 31); // 31
	#endif	// defined multispec_mac  	

	MHideDialogItem (dialogPtr, IDC_MinimumBitsPrompt); // 30
	
			// Output file data value type.	
	
	dataTypeCode = fileInfoPtr->dataTypeCode;
	numberBits = fileInfoPtr->numberBits;
	signedDataFlag = fileInfoPtr->signedDataFlag;
	
	if (*channelThematicDisplayFlagPtr)	
		{
		dataTypeCode = kIntegerType;
		numberBits = 8;
		signedDataFlag = FALSE;	
			
		}	// end "if (*channelThematicDisplayFlagPtr)"
	
	if (reformatOptionsPtr->dataValueTypeSelection == 0)
		*dataValueTypeSelectionPtr = FileSpecificationDialogSetDataType (dataTypeCode,
																							  numberBits,
																							  signedDataFlag);	
	else	// reformatOptionsPtr->dataValueTypeSelection != 0
		*dataValueTypeSelectionPtr = reformatOptionsPtr->dataValueTypeSelection;
	
	*savedDataValueTypeSelectionPtr = *dataValueTypeSelectionPtr;
	if (fileInfoPtr->format == kGAIAType || 
				fileInfoPtr->format == kGAIA2Type ||
							*channelThematicDisplayFlagPtr)
		*dataValueTypeSelectionPtr = -(*dataValueTypeSelectionPtr);
		
			// Make sure band interleave format fits with initial header format default.
			
	if (reformatOptionsPtr->headerFormat == kErdas74Type)
		reformatOptionsPtr->bandInterleaveSelection = kBIL;
	else if (reformatOptionsPtr->headerFormat == kTIFFType && 
											reformatOptionsPtr->bandInterleaveSelection == kBIL)
		reformatOptionsPtr->bandInterleaveSelection = kBSQ;
						
	*bandInterleaveSelectionPtr = reformatOptionsPtr->bandInterleaveSelection;
				
	if (windowInfoPtr->windowType == kThematicWindowType)
		{ 	
		MHideDialogItem (dialogPtr, IDC_BandInterleaveLabel);
		MHideDialogItem (dialogPtr, IDC_BandInterleave);
		MHideDialogItem (dialogPtr, IDC_ChannelsLabel);
		MHideDialogItem (dialogPtr, IDC_ChannelCombo);
		
		}	// end "if (fileInfoPtr->thematicType)"

	*channelSelectionPtr = kAllMenuItem;
	if (*channelThematicDisplayFlagPtr)
		{
		reformatOptionsPtr->numberChannels = 1;
		reformatOptionsPtr->channelPtr[0] = gToDisplayLevels.channel1;
		
		if (gImageWindowInfoPtr->totalNumberChannels > 1)
			*channelSelectionPtr = kSubsetMenuItem;
		
		}	// end "if (*channelThematicDisplayFlagPtr)"
		
	else	// !*channelThematicDisplayFlagPtr
		{
		if (reformatOptionsPtr->numberChannels < 
													gImageWindowInfoPtr->totalNumberChannels)
			*channelSelectionPtr = kSubsetMenuItem;
			
		}	// end "else !*channelThematicDisplayFlagPtr"

			// "Invert bottom to top" check box.											
 
	*bottomToTopFlagPtr = FALSE;
	
			// "Invert right to left" check box.											
 
	*rightToLeftFlagPtr = FALSE;
	
			// "Swap bytes" check box.															
 
 	*swapBytesFlagPtr = reformatOptionsPtr->swapBytesFlag;
		
	if (gImageWindowInfoPtr->numberBytes == 1 || 
				fileInfoPtr->format == kGAIAType ||
							fileInfoPtr->format == kGAIA2Type ||
										*channelThematicDisplayFlagPtr)
		SetDLogControlHilite (dialogPtr, IDC_SwapBytes, 255);
	
			// "Write channel description" check box.										
 
 	*channelDescriptionAllowedFlagPtr = (!fileInfoPtr->thematicType &&
														!*channelThematicDisplayFlagPtr &&
															gImageWindowInfoPtr->descriptionCode);
	*savedChannelDescriptionFlagPtr = *channelDescriptionAllowedFlagPtr;
							
	if (!*channelDescriptionAllowedFlagPtr)
		SetDLogControlHilite (dialogPtr, IDC_WriteChanDescriptions, 255);
	
			// "Output in wavelength order" check box
	
	*outputInWavelengthOrderFlagPtr =
											reformatOptionsPtr->outputInWavelengthOrderFlag;
	
	*outputInWavelengthOrderAllowedFlagPtr =
		(gImageWindowInfoPtr->channelsInWavelengthOrderCode == kNotInOrder &&
															*channelSelectionPtr == kAllMenuItem);
	
	if (*outputInWavelengthOrderAllowedFlagPtr)
		{
		if (reformatOptionsPtr->transformDataCode != kNoTransform)
			SetDLogControlHilite (dialogPtr, IDC_SwapBytes, 255);
		
		}	// end "if (*outputInWavelengthOrderAllowedFlagPtr)"
	
	if (!*outputInWavelengthOrderAllowedFlagPtr)
		{
		MHideDialogItem (dialogPtr, IDC_OutputInWavelengthOrder);
		*outputInWavelengthOrderFlagPtr = FALSE;
		
		}	// end "if (!*outputInWavelengthOrderAllowedFlagPtr"

			// Make sure that all header choices are enabled to start with.
			
	InitializeHeaderPopupMenu (dialogPtr, gPopUpHeaderOptionsMenu);
	
			// Adjust header format from the default if needed.
			
	*headerOptionsSelectionPtr = kERDAS74MenuItem;
	if (reformatOptionsPtr->headerFormat == kNoneMenuItem)
		*headerOptionsSelectionPtr = kNoneType;	
	else if (reformatOptionsPtr->headerFormat == kArcViewType)
		*headerOptionsSelectionPtr = kArcViewMenuItem;
	else if (reformatOptionsPtr->headerFormat == kErdas74Type)
		*headerOptionsSelectionPtr = kERDAS74MenuItem;
	else if (reformatOptionsPtr->headerFormat == kGAIAType)
		*headerOptionsSelectionPtr = kGAIAMenuItem;
	else if (reformatOptionsPtr->headerFormat == kTIFFType)
		*headerOptionsSelectionPtr = kTIFFGeoTIFFMenuItem;
	else if (reformatOptionsPtr->headerFormat == kMatlabType)
		*headerOptionsSelectionPtr = kMatlabMenuItem;
	
	*GAIAFormatAllowedFlagPtr = FALSE;
	if (fileInfoPtr->format == kGAIAType ||
				fileInfoPtr->format == kGAIA2Type ||
					(gProjectInfoPtr != NULL &&
							fileInfoPtr->thematicType && 
								fileInfoPtr->numberClasses == 
									gProjectInfoPtr->numberStatisticsClasses + 1))
		{
		*GAIAFormatAllowedFlagPtr = TRUE;
		
		if (fileInfoPtr->format == kGAIAType || fileInfoPtr->format == kGAIA2Type)
			*headerOptionsSelectionPtr = kGAIAMenuItem;
		
		}	// end "if (fileInfoPtr->format ==  kGAIAType || ...)" 
													
			// Set text indicating whether the output file format could be GeoTIFF
			// or TIFF
	
	mapInfoExistsFlag = FindIfMapInformationExists (windowInfoPtr);
		 
	#if defined multispec_win  
		comboBoxPtr->DeleteString (kTIFFGeoTIFFMenuItem);
	#endif	// defined multispec_win  

   #if defined multispec_wx
		comboBoxPtr->Delete (kTIFFGeoTIFFMenuItem);
   #endif
	
	if (mapInfoExistsFlag)
		CtoPstring ((UCharPtr)"GeoTIFF format", tiffMenuNameStringPtr, 14);
		
	else	// !mapInfoExistsFlag
		CtoPstring ((UCharPtr)"TIFF format", tiffMenuNameStringPtr, 14);

	#if defined multispec_mac  
		SetMenuItemText (gPopUpHeaderOptionsMenu, 
								kTIFFGeoTIFFMenuItem,
								(UCharPtr)tiffMenuNameStringPtr);
	#endif	// defined multispec_mac 
	 
	#if defined multispec_win  
		comboBoxPtr->InsertString (
				kTIFFGeoTIFFMenuItem, (LPCTSTR)A2T((LPCSTR)&tiffMenuNameStringPtr[1]));

		comboBoxPtr->SetItemData (kTIFFGeoTIFFMenuItem, kTIFFGeoTIFFMenuItem);
	#endif	// defined multispec_win  

   #if defined multispec_wx
      wxString tiffmenustring (&tiffMenuNameStringPtr[1], wxConvUTF8);
      comboBoxPtr->Insert (
			tiffmenustring, kTIFFGeoTIFFMenuItem, (void*)(SInt64)gTIFFGeoTIFFMenuItem);
		comboBoxPtr->SetClientData (
								kTIFFGeoTIFFMenuItem, (void*)(SInt64)kTIFFGeoTIFFMenuItem);
   #endif
	
			// Now that settings have been initialized, make sure the "Output file" popup box
			// is set up correctly for the requested output format.
												
	if (*headerOptionsSelectionPtr == kTIFFGeoTIFFMenuItem)
		ChangeImageFormatDialogUpdateTIFFHeader (dialogPtr,
																fileInfoPtr,
																outputFileSelectionPtr,
																bandInterleaveSelectionPtr,
																gPopUpOutputFileMenu);
	
}	// end "ChangeImageFormatDialogInitialize"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ChangeImageFormatDialogOK
//
//	Software purpose:	
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 01/21/2006
//	Revised By:			Larry L. Biehl			Date: 07/11/2018

Boolean ChangeImageFormatDialogOK (
				DialogPtr							dialogPtr,
				WindowInfoPtr						imageWindowInfoPtr,
				LayerInfoPtr						imageLayerInfoPtr,
				FileInfoPtr							imageFileInfoPtr,
				FileInfoPtr							outFileInfoPtr,
				ReformatOptionsPtr				reformatOptionsPtr,
				DialogSelectArea*					dialogSelectAreaPtr,
				SInt16								outputFileSelection,
				SInt16								bandInterleaveSelection,
				SInt16								dataValueTypeSelection,
				Boolean								sessionUserSetDataValueTypeSelectionFlag,
				SInt16								channelSelection,
				Boolean								bottomToTopFlag,
				Boolean								rightToLeftFlag,
				Boolean								swapBytesFlag,
				Boolean								channelDescriptionsFlag,
				Boolean								outputInWavelengthOrderFlag,
				SInt16								headerOptionsSelection,
				Boolean								channelThematicDisplayFlag)

{	
	float*								channelValuePtr;
	
	SInt16*								channelWavelengthOrderPtr;
	
	UInt32								index,
											inputByteCode,
											inputSignCode;
							
	UInt16								dataValueTypeToBitsCode[9] = {4,8,8,16,16,32,32,32,64},
											dataValueTypeToBytesCode[9] = {1,1,1,2,2,4,4,4,8},
											dataValueTypeToDataTypeCode[9] = {0,0,0,0,0,0,0,1,1},
											outByteCode[9] = {kForce1Byte, 
																	kForce1Byte, 
																	kForce1Byte,
																	kForce2Bytes,
																	kForce2Bytes,
																	kForce4Bytes,
																	kForce4Bytes,
																	kForceReal4Bytes,
																	kForceReal8Bytes};
	
	Boolean								OKFlag,
											outputSignedFlag[9] = {0,1,0,1,0,1,0,1,1};
			
			
	OKFlag = TRUE;
	
			// Output file code.										
			
	if (headerOptionsSelection == kMatlabMenuItem)
		outputFileSelection = kNewFileMenuItem;
	reformatOptionsPtr->outputFileCode = outputFileSelection;
	
			// Selected reformat area.												
	
	reformatOptionsPtr->lineStart = dialogSelectAreaPtr->lineStart;
	reformatOptionsPtr->lineEnd = dialogSelectAreaPtr->lineEnd;
	reformatOptionsPtr->lineInterval = 
											dialogSelectAreaPtr->lineInterval;
	reformatOptionsPtr->columnStart = dialogSelectAreaPtr->columnStart;
	reformatOptionsPtr->columnEnd = dialogSelectAreaPtr->columnEnd;
	reformatOptionsPtr->columnInterval = 
											dialogSelectAreaPtr->columnInterval;

			// Item 33: Data value type.	
			// Make sure data value selection is positive and 0-based.
			
	dataValueTypeSelection = abs (dataValueTypeSelection);

	if (sessionUserSetDataValueTypeSelectionFlag)
		reformatOptionsPtr->userSetDataValueTypeSelectionFlag = TRUE;
	reformatOptionsPtr->dataValueTypeSelection = dataValueTypeSelection;
	#if defined multispec_mac  
		dataValueTypeSelection--;
	#endif	// defined multispec_mac 	
	
	outFileInfoPtr->numberBits = dataValueTypeToBitsCode[dataValueTypeSelection];
	outFileInfoPtr->numberBytes = dataValueTypeToBytesCode[dataValueTypeSelection];
	outFileInfoPtr->dataTypeCode = dataValueTypeToDataTypeCode[dataValueTypeSelection];

			// Image line-channel format.								

	outFileInfoPtr->bandInterleave = bandInterleaveSelection;
		
			// Convert the channel pointer vector to point to actual		
			// channel numbers, not indices to a channels vector.			
	
	switch (reformatOptionsPtr->transformDataCode)
		{
		case kNoTransform:
		case kAdjustChannel:
			if (channelSelection == kAllMenuItem)
				{
				reformatOptionsPtr->numberChannels = 
														imageWindowInfoPtr->totalNumberChannels;
				for (index=0; index<reformatOptionsPtr->numberChannels; index++)
					reformatOptionsPtr->channelPtr[index] = (SInt16)index;
					
				}	// end "if (channelSelection == kAllMenuItem)"
			break;
			
		case kTransformChannels:
			if (GetTransformChannelList (reformatOptionsPtr) == 0)
				OKFlag = FALSE;
			break;
			
		case kCreatePCImage:
			if (GetPCChannelList (reformatOptionsPtr) == 0)
				OKFlag = FALSE;
			break;
			
		}	// end "switch (reformatOptionsPtr->transformDataCode)" 

			// "Invert bottom to top" check box.								

	reformatOptionsPtr->bottomToTop = bottomToTopFlag;

			// "Invert right to left" check box.								

	reformatOptionsPtr->rightToLeft = rightToLeftFlag;

			// "Swap bytes" check box.												

	reformatOptionsPtr->swapBytesFlag = swapBytesFlag;

			// "Write Channel descriptions" check box.						

	reformatOptionsPtr->channelDescriptions = channelDescriptionsFlag;
												
			// Channel descriptions are not to be written for transformed
			// data, matlab output files or files that are being modified.
												
	if (reformatOptionsPtr->transformDataCode == kTransformChannels ||
			reformatOptionsPtr->transformDataCode == kCreatePCImage ||
					headerOptionsSelection == kMatlabMenuItem ||
							reformatOptionsPtr->outputFileCode == kModifyPartMenuItem)
		reformatOptionsPtr->channelDescriptions = FALSE;

			// "Image Header" popup box.	
			
	reformatOptionsPtr->headerFormat = 
							GetHeaderFormatFromPopUpSelection (headerOptionsSelection);
						
	if (reformatOptionsPtr->headerFormat <= kNoneType)	// kNoneType == 0
		outFileInfoPtr->numberHeaderBytes = 0;
		
			// Get the file information for the selected channels.  The	
			// information will be loaded into the window information	
			// structure.																
		
	GetFileInformationForChannelList (imageWindowInfoPtr,
													imageLayerInfoPtr, 
													imageFileInfoPtr,
													reformatOptionsPtr->channelPtr, 
													reformatOptionsPtr->numberChannels);
	
			// Determine the conversion type										
	
	reformatOptionsPtr->convertType = 0;
			
			// Get the resulting byte code (forceOutByteCode) to be used when 
			// reading the data from the image file.
			// Also get the conversion code to be used in the Reformat
			// processing routine to put data in the user define format.
			
	if (reformatOptionsPtr->transformDataCode != kNoTransform ||
																		channelThematicDisplayFlag)
		{
				// If the data is to be transformed, then the input bytes will
				//	always be converted to 8 real bytes. This allows for signed 
				// 1-byte, 2-byte and 4-byte integer input data. This will be
				// the format of the data for the transformation algorithms.
				// The computed data will then be converted to the requested
				// data format for the new file.						
			
		reformatOptionsPtr->forceByteCode = kForceReal8Bytes;
		
				// For files generated from thematic type image windows, the
				// intermediate data value type will be unsigned 16-bit integer.
				
		if (channelThematicDisplayFlag)
			dataValueTypeSelection = 4;
	
		reformatOptionsPtr->convertType = GetDataConversionCode (
													kRealType,
													8,		// number bytes
													TRUE,	// signed data flag
													outByteCode[dataValueTypeSelection],
													outputSignedFlag[dataValueTypeSelection],
													&reformatOptionsPtr->checkForSaturationFlag);
		
		reformatOptionsPtr->convertFromNumberBytes = 8;
		
				// Force check for saturation so the we know what the max and min
				// numbers are. 
				// Will not need to do this for thematic type multispectral
				// displays since all "saturated" values are assigned to min and
				// max bin by definition.
				
		reformatOptionsPtr->checkForSaturationFlag = TRUE;
		if (channelThematicDisplayFlag)
			reformatOptionsPtr->checkForSaturationFlag = FALSE;	
		
		}	// end "if (reformatOptionsPtr->transformDataCode != kNoTransform)"
		
	else	// reformatOptionsPtr->transformDataCode == kNoTransform
		{
		reformatOptionsPtr->forceByteCode = outByteCode[dataValueTypeSelection];
	
		reformatOptionsPtr->convertType = GetDataConversionCode (
												imageFileInfoPtr->dataTypeCode,
												imageFileInfoPtr->numberBytes,
												imageFileInfoPtr->signedDataFlag,
												reformatOptionsPtr->forceByteCode,
												outputSignedFlag[dataValueTypeSelection],
												&reformatOptionsPtr->checkForSaturationFlag);
		
		reformatOptionsPtr->convertFromNumberBytes = imageFileInfoPtr->numberBytes;
		
				// Note that if output band interleave format is to be BSQ and or
				// columns are to be read from right to left, then
								
		}	// end "else reformatOptionsPtr->transformDataCode == kNoTransform"
		
			// Make sure any function of channel paramaters make sense
			
	if (reformatOptionsPtr->transformDataCode == kFunctionOfChannels &&
					reformatOptionsPtr->functionCode == kFunctionKthSmallestElement)
		reformatOptionsPtr->kthSmallestValue = MIN (reformatOptionsPtr->kthSmallestValue, 
																	reformatOptionsPtr->numberChannels);
	
	if (!reformatOptionsPtr->checkForSaturationFlag && 
				!reformatOptionsPtr->rightToLeft &&
						outFileInfoPtr->bandInterleave != kBSQ &&
						 		!(imageFileInfoPtr->bandInterleave == kBIS && 
						 							outFileInfoPtr->bandInterleave == kBIL) &&
										!channelThematicDisplayFlag)
		reformatOptionsPtr->convertType = 1;
		
	else	// ...->checkForSaturationFlag || ...->rightToLeft || ...== kBSQ
		{
		if (reformatOptionsPtr->checkForSaturationFlag)
			{
					// Get the working data type that will be used for checking for
					// data values that are out of range & and make sure that the
					// force byte code is correct to read the data in. Real will
					// be forced to 64-bit real and integer will be to 32-bit integer
					// and like sign.
					
			inputByteCode = (reformatOptionsPtr->convertType & 0x0ff00000);
			inputSignCode = (reformatOptionsPtr->convertType & 0x000f0000);
			
			if (inputByteCode == 0x00800000)
				{
				reformatOptionsPtr->workingDataTypeCode = k4ByteSignedIntegerMenuItem;
				reformatOptionsPtr->forceByteCode = kForce4Bytes;
				reformatOptionsPtr->convertFromNumberBytes = 4;
				
				}	// end "if (inputByteCode == 0x00800000)"
			
			else if (inputByteCode == 0x01000000)
				{
				reformatOptionsPtr->workingDataTypeCode = k4ByteSignedIntegerMenuItem;
				reformatOptionsPtr->forceByteCode = kForce4Bytes;
				reformatOptionsPtr->convertFromNumberBytes = 4;
				
				}	// end "else if (inputByteCode == 0x01000000)"
			
			else if (inputByteCode == 0x02000000 && inputSignCode != 0x00030000)
				{
				reformatOptionsPtr->workingDataTypeCode = k4ByteSignedIntegerMenuItem;
				reformatOptionsPtr->forceByteCode = kForce4Bytes;
				reformatOptionsPtr->convertFromNumberBytes = 4;
				
				}	// end "else if (inputByteCode == 0x02000000 && ...)"
			
			else if (inputByteCode == 0x02000000 && inputSignCode == 0x00030000)
				{
				reformatOptionsPtr->workingDataTypeCode = k8ByteRealMenuItem;
				reformatOptionsPtr->forceByteCode = kForceReal8Bytes;
				reformatOptionsPtr->convertFromNumberBytes = 8;
				
				}	// end "else if (inputByteCode == 0x02000000 && ...)"
			
			else if (inputByteCode == 0x04000000)
				{
				reformatOptionsPtr->workingDataTypeCode = k8ByteRealMenuItem;
				reformatOptionsPtr->forceByteCode = kForceReal8Bytes;
				reformatOptionsPtr->convertFromNumberBytes = 8;
				
				}	// end "else if (inputByteCode == 0x04000000)"
			
			if (reformatOptionsPtr->workingDataTypeCode < k4ByteRealMenuItem &&
																		inputSignCode != 0x00010000)
				reformatOptionsPtr->workingDataTypeCode++;
				
			}	// end "if (reformatOptionsPtr->checkForSaturationFlag)"
			
		else	// ...->rightToLeft || ...== kBSQ
			{
			reformatOptionsPtr->convertFromNumberBytes = outFileInfoPtr->numberBytes;
				
			}	// end "else ...->rightToLeft || ...== kBSQ"
			
		}	// end "else ...->checkForSaturationFlag || ...->rightToLeft || ...== kBSQ
		
			// Get the output sign flag.
	
	reformatOptionsPtr->signedOutputDataFlag =
														outputSignedFlag[dataValueTypeSelection];
	
			// Get the "output in wavelength order flag"
	
	if (reformatOptionsPtr->transformDataCode == kNoTransform &&
			channelSelection == kAllMenuItem &&
				outputInWavelengthOrderFlag)
		{
		channelValuePtr = (float*)GetHandlePointer (
															imageFileInfoPtr->channelValuesHandle);
		
		if (channelValuePtr != NULL)
			{
			//fileWavelengthOrderPtr =
			//					(UInt16*)&channelValuePtr[2*imageFileInfoPtr->numberChannels];
		
					// Initialize the Reformat channel order vector
		
			channelWavelengthOrderPtr =
				&reformatOptionsPtr->channelPtr[imageWindowInfoPtr->totalNumberChannels];
		
					// Just make copy of the image channel wavelength order
			
			for (index=0; index<imageWindowInfoPtr->totalNumberChannels; index++)
				//channelWavelengthOrderPtr[index] = fileWavelengthOrderPtr[index];
				channelWavelengthOrderPtr[index] =
													imageLayerInfoPtr[index+1].wavelengthOrder;
			
			}	// end "if (channelValuePtr != NULL)"

		reformatOptionsPtr->outputInWavelengthOrderFlag = outputInWavelengthOrderFlag;
		
		}	// end "if (...->transformDataCode == kNoTransform && ..."
		
	else	// ...->transformDataCode == kNoTransform || ...
		reformatOptionsPtr->outputInWavelengthOrderFlag = FALSE;
														
	if (reformatOptionsPtr->convertType == 0)  
		OKFlag = FALSE;
	
	return (OKFlag);
	
}	// end "ChangeImageFormatDialogOK"


                       
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ChangeImageFormatDialogUpdateHeaderMenu
//
//	Software purpose:	
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:			OnSelendokDataValueType in xReformatChangeDialog.cpp
//							OnTransformData in xReformatChangeDialog.cpp
//							ChangeImageFormatDialog
//
//	Coded By:			Larry L. Biehl			Date: 12/12/2005
//	Revised By:			Larry L. Biehl			Date: 11/04/2017

void ChangeImageFormatDialogUpdateHeaderMenu (
				DialogPtr							dialogPtr,
				FileInfoPtr							fileInfoPtr,
				UCharPtr								tiffMenuNameStringPtr,
				SInt16*								outputFileSelectionPtr,
				SInt16*								bandInterleaveSelectionPtr,
				SInt16*								headerOptionsSelectionPtr,
				MenuHandle							popUpOutputFileMenu,
				MenuHandle							popUpHeaderOptionsMenu,
				SInt16								transformDataCode,
				SInt16								dataValueTypeSelection,
				Boolean								GAIAFormatAllowedFlag)

{
	#if defined multispec_win | defined multispec_wx
		Boolean								menuItemList[6];


		menuItemList[kNoneMenuItem] = TRUE;
		menuItemList[kArcViewMenuItem] = TRUE;
		menuItemList[kERDAS74MenuItem] = TRUE;
		menuItemList[kGAIAMenuItem] = GAIAFormatAllowedFlag;
		menuItemList[kTIFFGeoTIFFMenuItem] = TRUE;
		menuItemList[kMatlabMenuItem] = TRUE;
	#endif	// defined multispec_win | defined multispec_wx


	#if defined multispec_win
		USES_CONVERSION;
	#endif
	/*
	if (dataValueTypeSelection == k8ByteRealMenuItem)
		EnableMenuItem (popUpHeaderOptionsMenu, kMatlabMenuItem);
	
	else	// dataValueTypeSelection < k8ByteRealMenuItem
		DisableMenuItem (popUpHeaderOptionsMenu, kMatlabMenuItem);
	*/
	if (dataValueTypeSelection >= k8ByteRealMenuItem)
		{
				// 8-byte Reals not handled for ArcView type images
				
		#if defined multispec_mac
			DisableMenuItem (popUpHeaderOptionsMenu, kArcViewMenuItem);
		#endif	// defined multispec_mac	
		
		#if defined multispec_win | defined multispec_wx
			menuItemList[kArcViewMenuItem] = FALSE;
		#endif	// defined multispec_win | defined multispec_wx
		
		}	// end "if (dataValueTypeSelection >= k4ByteRealMenuItem)"
		
	if (dataValueTypeSelection >= k4ByteSignedIntegerMenuItem)
		{
				// If 4-byte or 8-byte output is requested, turn off some options.
		
		#if defined multispec_mac
			DisableMenuItem (popUpHeaderOptionsMenu, kERDAS74MenuItem);
		
			if (dataValueTypeSelection <= k4ByteRealMenuItem)
				EnableMenuItem (popUpHeaderOptionsMenu, kArcViewMenuItem);
		#endif	// defined multispec_mac
		
		#if defined multispec_win | defined multispec_wx
			menuItemList[kERDAS74MenuItem] = FALSE;
		#endif	// defined multispec_win | defined multispec_wx
		
		}	// end "if (dataValueTypeSelection >= k4ByteSignedIntegerMenuItem)"
	
	else	// dataValueTypeSelection < k4ByteSignedIntegerMenuItem
		{
		#if defined multispec_mac
			EnableMenuItem (popUpHeaderOptionsMenu, kArcViewMenuItem);
			EnableMenuItem (popUpHeaderOptionsMenu, kERDAS74MenuItem);
		#endif	// defined multispec_mac
		
		}	// end "else dataValueTypeSelection < k4ByteSignedIntegerMenuItem"
					
	#if defined multispec_win
		SInt16			index = 0,
							numberComboItems;
							
		CComboBox* comboBoxPtr = (CComboBox*)(dialogPtr->GetDlgItem (IDC_ChangeHeader));
		
				// Remove all but the first item in the list.

		numberComboItems = comboBoxPtr->GetCount ();
		while (numberComboItems > 1)
			{
			comboBoxPtr->DeleteString (numberComboItems-1);
			numberComboItems--;

			}	// end "while (numberComboItems > 1)"

		//comboBoxPtr->SetItemData (0, kNoneMenuItem);

		if (menuItemList[kArcViewMenuItem])
			{
			comboBoxPtr->InsertString (-1, (LPCTSTR)_T("ArcView format"));
			comboBoxPtr->SetItemData (numberComboItems, kArcViewMenuItem);
			numberComboItems++;

			}	// end "if (menuItemList[kArcViewMenuItem])"

		if (menuItemList[kERDAS74MenuItem])
			{
			comboBoxPtr->InsertString (-1, (LPCTSTR)_T("ERDAS 74 format"));
			comboBoxPtr->SetItemData (numberComboItems, kERDAS74MenuItem);
			numberComboItems++;

			}	// end "if (menuItemList[kERDAS74MenuItem])"
		
		if (menuItemList[kGAIAMenuItem])
			{
			comboBoxPtr->InsertString (-1, (LPCTSTR)_T("GAIA format"));
			comboBoxPtr->SetItemData (numberComboItems, kGAIAMenuItem);
			numberComboItems++;

			}	// end "if (menuItemList[kGAIAMenuItem])"
		
		if (menuItemList[kTIFFGeoTIFFMenuItem])
			{
			comboBoxPtr->InsertString (
										-1, (LPCTSTR)A2T((LPCSTR)&tiffMenuNameStringPtr[1]));
			comboBoxPtr->SetItemData (numberComboItems, kTIFFGeoTIFFMenuItem);
			numberComboItems++;

			}	// end "if (menuItemList[kTIFFGeoTIFFMenuItem])"
		
		if (menuItemList[kMatlabMenuItem])
			{
			comboBoxPtr->InsertString (-1, (LPCTSTR)_T("Matlab format"));
			comboBoxPtr->SetItemData (numberComboItems, kMatlabMenuItem);

			}	// end "if (menuItemList[kMatlabMenuItem])"
	#endif	// defined multispec_win

	#if defined multispec_wx
		SInt16			numberComboItems;
							
		wxChoice* comboBoxPtr =
									(wxChoice*)(dialogPtr->FindWindow (IDC_ChangeHeader));
		
				// Remove all but the first item in the list.

		numberComboItems = comboBoxPtr->GetCount ();
		while (numberComboItems > 1)
			{
			comboBoxPtr->Delete (numberComboItems-1);
			numberComboItems--;

			}	// end "while (numberComboItems > 1)"

		//comboBoxPtr->SetItemData (0, kNoneMenuItem);

		if (menuItemList[kArcViewMenuItem])
			{
			comboBoxPtr->Append (wxT("ArcView format"), (void*)&gArcViewMenuItem);
			comboBoxPtr->SetClientData (numberComboItems, (void*)kArcViewMenuItem);
			numberComboItems++;

			}	// end "if (menuItemList[kArcViewMenuItem])"

		if (menuItemList[kERDAS74MenuItem])
			{
			comboBoxPtr->Append (wxT("ERDAS 74 format"), (void*)&gERDAS74MenuItem);
			comboBoxPtr->SetClientData (numberComboItems, (void*)kERDAS74MenuItem);
			numberComboItems++;

			}	// end "if (menuItemList[kERDAS74MenuItem])"
		
		if (menuItemList[kGAIAMenuItem])
			{
			comboBoxPtr->Append (wxT("GAIA format"), (void*)&gGAIAMenuItem);
			comboBoxPtr->SetClientData (numberComboItems, (void*)kGAIAMenuItem);
			numberComboItems++;

			}	// end "if (menuItemList[kGAIAMenuItem])"
		
		if (menuItemList[kTIFFGeoTIFFMenuItem])
			{
         // Making use of wxwidgets 2.9 feature that automatically converts 
         // char* to wxString
			comboBoxPtr->Append (&tiffMenuNameStringPtr[1], (void*)&gTIFFGeoTIFFMenuItem);
			comboBoxPtr->SetClientData (numberComboItems, (void*)kTIFFGeoTIFFMenuItem);
			numberComboItems++;

			}	// end "if (menuItemList[kTIFFGeoTIFFMenuItem])"
		
		if (menuItemList[kMatlabMenuItem])
			{
			comboBoxPtr->Append (wxT("Matlab format"),(void*)&gMatlabMenuItem);
			comboBoxPtr->SetClientData (numberComboItems, (void*)kMatlabMenuItem);

			}	// end "if (menuItemList[kMatlabMenuItem])"
	#endif	// defined multispec_wx
	
	ChangeImageFormatDialogVerifyHeaderSetting (dialogPtr,
																fileInfoPtr,
																*bandInterleaveSelectionPtr,
																dataValueTypeSelection,
																popUpOutputFileMenu,
																popUpHeaderOptionsMenu,
																outputFileSelectionPtr,
																headerOptionsSelectionPtr);
	
	ChangeImageFormatDialogUpdateSwapBytes (dialogPtr,
															fileInfoPtr,
															dataValueTypeSelection,
															*headerOptionsSelectionPtr);
	
}	// end "ChangeImageFormatDialogUpdateHeaderMenu"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ChangeImageFormatDialogUpdateHeaderOptions
//
//	Software purpose:	
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:			OnSelendokHeader in xReformatChangeDialog.cpp
//							ChangeImageFormatDialog
//
//	Coded By:			Larry L. Biehl			Date: 01/20/2006
//	Revised By:			Larry L. Biehl			Date: 11/04/2019

void ChangeImageFormatDialogUpdateHeaderOptions (
				DialogPtr							dialogPtr,
				FileInfoPtr							fileInfoPtr,
				SInt16								headerOptionsSelection,
				SInt16*								outputFileSelectionPtr,
				SInt16*								bandInterleaveSelectionPtr,
				SInt16*								dataValueTypeSelectionPtr,
				Boolean*								setChannelDescriptionFlagPtr,
				MenuHandle							popUpOutputFileMenu,
				Boolean								channelDescriptionAllowedFlag,
				Boolean								channelThematicDisplayFlag)

{
	SInt16								numberBits;

	Boolean								setFileFlag = FALSE,
											setSwapByteFlag = FALSE;
											
											
	*setChannelDescriptionFlagPtr = FALSE;
	
	#if defined multispec_win
		CComboBox* comboBoxPtr = (CComboBox*)dialogPtr->
															GetDlgItem (IDC_OutputFileSelection);
	#endif	// defined multispec_win  
											
   #if defined multispec_wx
		wxChoice* comboBoxPtr =
							(wxChoice*)dialogPtr->FindWindow (IDC_OutputFileSelection);
   #endif
						
	switch (headerOptionsSelection)
		{
		case kNoneMenuItem:						
			setFileFlag = TRUE;
			setSwapByteFlag = TRUE;
			*setChannelDescriptionFlagPtr = channelDescriptionAllowedFlag;
			break;
			
		case kArcViewMenuItem:
			/*
			if (*dataValueTypeSelectionPtr > k4ByteRealMenuItem)
				{
				numberBits = MIN (fileInfoPtr->numberBits, 32);
				*dataValueTypeSelectionPtr = FileSpecificationDialogSetDataType (
																		kRealType,
																		numberBits,
																		fileInfoPtr->signedDataFlag);

				}	// end "if (*dataValueTypeSelectionPtr > ...)"
			*/
			setFileFlag = TRUE;
			*setChannelDescriptionFlagPtr = channelDescriptionAllowedFlag;
			break;
			
		case kERDAS74MenuItem:
			if (*dataValueTypeSelectionPtr > k2ByteUnsignedIntegerMenuItem)
				{
				numberBits = MIN (fileInfoPtr->numberBits, 16);
				*dataValueTypeSelectionPtr = FileSpecificationDialogSetDataType (
																		kIntegerType,
																		numberBits,
																		fileInfoPtr->signedDataFlag);

				}	// end "if (*dataValueTypeSelectionPtr > ...)"

			*bandInterleaveSelectionPtr = kBILMenuItem;
			
			setFileFlag = TRUE;
			*setChannelDescriptionFlagPtr = channelDescriptionAllowedFlag;
			/*
					// Force 2-byte data on Macintosh to be swapped. Do not
					// force 2-byte data on Windows to be swapped. ERDAS74 format
					// for 2-byte data is little endian as standard
			
			if (*dataValueTypeSelectionPtr >= k2ByteSignedIntegerMenuItem)
				{
				#if defined multispec_mac
					SetDLogControl (dialogPtr, IDC_SwapBytes, 1);
					SetDLogControlHilite (dialogPtr, IDC_SwapBytes, 255);
				#endif	// defined multispec_mac
			
				#if defined multispec_win
					SetDLogControl (dialogPtr, IDC_SwapBytes, 0);
					SetDLogControlHilite (dialogPtr, IDC_SwapBytes, 255);
				#endif	// defined multispec_win
				
				}	// end "if (... >= k2ByteSignedIntegerMenuItem)"
				
			else	// *dataValueTypeSelectionPtr < k2ByteSignedIntegerMenuItem
				{ 
				SetDLogControl (dialogPtr, IDC_SwapBytes, 0);
				SetDLogControlHilite (dialogPtr, IDC_SwapBytes, 255);
					
				}	// end "else *dataValueTypeSelectionPtr < k2ByteSignedIntegerMenuItem"
			*/
			//setSwapByteFlag = TRUE;
			break;
			
		case kGAIAMenuItem:
			*dataValueTypeSelectionPtr = k2ByteUnsignedIntegerMenuItem;
			
			SetDLogControl (dialogPtr, IDC_SwapBytes, 0);
			SetDLogControlHilite (dialogPtr, IDC_SwapBytes, 255);
			SetDLogControl (dialogPtr, IDC_WriteChanDescriptions, 0);
			SetDLogControlHilite (dialogPtr, IDC_WriteChanDescriptions, 255);
			
			setFileFlag = TRUE;
			break;
			
		case kMatlabMenuItem:
			*outputFileSelectionPtr = kNewFileMenuItem;
			
			#if defined multispec_mac
				DisableMenuItem (popUpOutputFileMenu, kAppendToMenuItem);
				DisableMenuItem (popUpOutputFileMenu, kModifyPartMenuItem);
			#endif	// defined multispec_mac  
		
			#if defined multispec_win
				if (comboBoxPtr->GetCount () > 1)
					{                             
					comboBoxPtr->DeleteString (kModifyPartMenuItem-1);
					comboBoxPtr->DeleteString (kAppendToMenuItem-1);
			
         		}	// end "comboBoxPtr->GetCount () == 1"
			#endif	// defined multispec_win  
				
         #if defined multispec_wx
            if (comboBoxPtr->GetCount () > 1) 
					{
					comboBoxPtr->Delete (kModifyPartMenuItem - 1);
					comboBoxPtr->Delete (kAppendToMenuItem - 1);

					}	// end "comboBoxPtr->GetCount () == 1"
         #endif	// defined multispec_wx

			*dataValueTypeSelectionPtr = k8ByteRealMenuItem;
			
			*bandInterleaveSelectionPtr = kBISMenuItem;
			
					// Force bytes to be swapped if one needs to read the data properly
					// from the input file.
			/*
			SetDLogControl (dialogPtr, IDC_SwapBytes, 0);
			if (fileInfoPtr->swapBytesFlag)
				SetDLogControl (dialogPtr, IDC_SwapBytes, 1);
			SetDLogControlHilite (dialogPtr, IDC_SwapBytes, 255);
			*/
			SetDLogControl (dialogPtr, IDC_WriteChanDescriptions, 0);
			SetDLogControlHilite (dialogPtr, IDC_WriteChanDescriptions, 255);
			break;
			
		case kTIFFGeoTIFFMenuItem:
			ChangeImageFormatDialogUpdateTIFFHeader (dialogPtr,
																	fileInfoPtr,
																	outputFileSelectionPtr,
																	bandInterleaveSelectionPtr,
																	gPopUpOutputFileMenu);
			*setChannelDescriptionFlagPtr = channelDescriptionAllowedFlag;
			break;
	
		}	// end "switch (gHeaderOptionsSelection)" 
		
		// Handle case for thematic multispectral type display.
		
	if (channelThematicDisplayFlag)
		{
		numberBits = 8;
		if (gHeaderOptionsSelection == kMatlabMenuItem)
			numberBits = 64;

		*dataValueTypeSelectionPtr = -FileSpecificationDialogSetDataType (kIntegerType,
																								numberBits,
																								FALSE);
		
		}	// end "if (channelThematicDisplayFlag)"
		
	if (setFileFlag)
		{
		#if defined multispec_mac
			EnableMenuItem (popUpOutputFileMenu, kAppendToMenuItem);
			EnableMenuItem (popUpOutputFileMenu, kModifyPartMenuItem);
		#endif	// defined multispec_mac  
		
		#if defined multispec_win
			if (comboBoxPtr->GetCount () == 1)
				{                             
				comboBoxPtr->AddString ((LPCTSTR)_T("Append To"));
				comboBoxPtr->AddString ((LPCTSTR)_T("Modify Part"));
			
         	}	// end "comboBoxPtr->GetCount () == 1"
		#endif	// defined multispec_win  
		
		#if defined multispec_wx
			if (comboBoxPtr->GetCount () == 1)
				{
				comboBoxPtr->Append ("Append To");
				comboBoxPtr->Append ("Modify Part");

				}	// end "comboBoxPtr->GetCount () == 1"
		#endif	// defined multispec_wx

		}	// end "if (setFileFlag)" 
		
	ChangeImageFormatDialogUpdateSwapBytes (dialogPtr,
															fileInfoPtr,
															*dataValueTypeSelectionPtr,
															headerOptionsSelection);
	/*
	if (setSwapByteFlag)
		{
		//SetDLogControl (dialogPtr, IDC_SwapBytes, (reformatOptionsPtr->swapBytesFlag));

		if (gImageWindowInfoPtr->numberBytes == 2) 
			SetDLogControlHilite (dialogPtr, IDC_SwapBytes, 0);
		
		}	// end "if (setSwapByteFlag)" 
	*/
			// Make certain that the correct label is drawn in the pop up boxes.
	
	#if defined multispec_mac  
		InvalidateDialogItemRect (dialogPtr, IDC_OutputFileSelection);
		InvalidateDialogItemRect (dialogPtr, IDC_DataValueType);
		InvalidateDialogItemRect (dialogPtr, IDC_BandInterleave);
		InvalidateDialogItemRect (dialogPtr, IDC_ChangeHeader);
	#endif	// defined multispec_mac 
	
}	// end "ChangeImageFormatDialogUpdateHeaderOptions"


                       
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ChangeImageFormatDialogUpdateSwapBytes
//
//	Software purpose:	
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 04/14/2006
//	Revised By:			Larry L. Biehl			Date: 04/14/2006

void ChangeImageFormatDialogUpdateSwapBytes (
				DialogPtr							dialogPtr,
				FileInfoPtr							fileInfoPtr,
				SInt16								dataValueTypeSelection,
				SInt16								headerOptionsSelection)

{    				
			
			// Force 2-byte data on Macintosh to be swapped. Do not
			// force 2-byte data on Windows to be swapped. ERDAS74 format
			// for 2-byte data is little endian as standard
	
	if (dataValueTypeSelection >= k2ByteSignedIntegerMenuItem)
		{
		switch (headerOptionsSelection)
			{
			case kERDAS74MenuItem:
				#if defined multispec_mac
					SetDLogControl (dialogPtr, IDC_SwapBytes, 1);
					SetDLogControlHilite (dialogPtr, IDC_SwapBytes, 255);
				#endif	// defined multispec_mac
			
				#if defined multispec_win
					SetDLogControl (dialogPtr, IDC_SwapBytes, 0);
					SetDLogControlHilite (dialogPtr, IDC_SwapBytes, 255);
				#endif	// defined multispec_win
				break;
			/*
			case kTIFFGeoTIFFMenuItem:
				SetDLogControl (dialogPtr, IDC_SwapBytes, 0);
				if (fileInfoPtr->swapBytesFlag)
					SetDLogControl (dialogPtr, IDC_SwapBytes, 1);
				SetDLogControlHilite (dialogPtr, IDC_SwapBytes, 255);
				break;
				
			case kMatlabMenuItem:
				
						// Force bytes to be swapped if one needs to to read the
						// data properly from the input file.
						// Don't think this is needed now (4/14/2006)
						
				SetDLogControl (dialogPtr, IDC_SwapBytes, 0);
				if (fileInfoPtr->swapBytesFlag)
					SetDLogControl (dialogPtr, IDC_SwapBytes, 1);
				SetDLogControlHilite (dialogPtr, IDC_SwapBytes, 255);
				break;
			*/
			default:
				SetDLogControl (dialogPtr, IDC_SwapBytes, 0);
				SetDLogControlHilite (dialogPtr, IDC_SwapBytes, 0);
			
			}	// end "switch (headerOptionsSelection)"
		
		}	// end "if (... >= k2ByteSignedIntegerMenuItem)"
		
	else	// *dataValueTypeSelectionPtr < k2ByteSignedIntegerMenuItem
		{ 
		SetDLogControl (dialogPtr, IDC_SwapBytes, 0);
		SetDLogControlHilite (dialogPtr, IDC_SwapBytes, 255);
			
		}	// end "else *dataValueTypeSelectionPtr < k2ByteSignedIntegerMenuItem"
	
}	// end "ChangeImageFormatDialogUpdateSwapBytes"


                       
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ChangeImageFormatDialogUpdateTIFFHeader
//
//	Software purpose:	
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 12/12/2005
//	Revised By:			Larry L. Biehl			Date: 11/04/2019

void ChangeImageFormatDialogUpdateTIFFHeader (
				DialogPtr							dialogPtr,
				FileInfoPtr							fileInfoPtr,
				SInt16*								outputFileSelectionPtr,
				SInt16*								bandInterleaveSelectionPtr,
				MenuHandle							popUpOutputFileMenu)

{    			
			// Appending to  or modifying a TIFF file is not allowed for now since
			//	gdal is being used to read the files.
			
	*outputFileSelectionPtr = kNewFileMenuItem;
			
	#if defined multispec_mac					
		DisableMenuItem (popUpOutputFileMenu, kAppendToMenuItem);
		DisableMenuItem (popUpOutputFileMenu, kModifyPartMenuItem);
		InvalidateDialogItemRect (dialogPtr, IDC_OutputFileSelection);
	#endif	// defined multispec_mac  

	#if defined multispec_win 
		CComboBox* comboBoxPtr =
							(CComboBox*)dialogPtr->GetDlgItem (IDC_OutputFileSelection);
      if (comboBoxPtr->GetCount () > 1)
			{                             
			comboBoxPtr->DeleteString (kModifyPartMenuItem-1);
			comboBoxPtr->DeleteString (kAppendToMenuItem-1);
	
   		}	// end "comboBoxPtr->GetCount () > 1"
	#endif	// defined multispec_win  
   
   #if defined multispec_wx 
      wxChoice* comboBoxPtr =
							(wxChoice*)dialogPtr->FindWindow (IDC_OutputFileSelection);
      if (comboBoxPtr->GetCount () > 1)
         {
         comboBoxPtr->Delete (kModifyPartMenuItem-1);
         comboBoxPtr->Delete (kAppendToMenuItem-1);
         
         }	// end "comboBoxPtr->GetCount () > 1"
   #endif	// defined multispec_wx  
	 	
	if (bandInterleaveSelectionPtr != NULL && 
					gImageWindowInfoPtr->windowType == kImageWindowType &&
								*bandInterleaveSelectionPtr == kBILMenuItem)
		{
		*bandInterleaveSelectionPtr = kBSQMenuItem;
		#if defined multispec_mac  
			InvalidateDialogItemRect (dialogPtr, IDC_BandInterleave);
		#endif	// defined multispec_mac 
		
		}	// end "if (bandInterleaveSelectionPtr != NULL && ...)"
	
}	// end "ChangeImageFormatDialogUpdateTIFFHeader"


                       
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ChangeImageFormatDialogVerifyHeaderSetting
//
//	Software purpose:	
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:			OnSelendokBandInterleave in xReformatChangeDialog.cpp
//							ChangeImageFormatDialog
//
//	Coded By:			Larry L. Biehl			Date: 01/19/2006
//	Revised By:			Larry L. Biehl			Date: 11/12/2019

void ChangeImageFormatDialogVerifyHeaderSetting (
				DialogPtr							dialogPtr,
				FileInfoPtr							fileInfoPtr,
				SInt16								bandInterleaveSelection,
				SInt16								dataValueTypeSelection,
				MenuHandle							popUpOutputFileMenu,
				MenuHandle							popUpHeaderOptionsMenu,
				SInt16*								outputFileSelectionPtr,
				SInt16*								headerOptionsSelectionPtr)

{
	SInt16								inputHeaderOptionsSelections;
	
	
	inputHeaderOptionsSelections = *headerOptionsSelectionPtr;
	
	switch (bandInterleaveSelection)
		{
		case kBILMenuItem:
			if (*headerOptionsSelectionPtr == kNoneMenuItem ||
						*headerOptionsSelectionPtr == kMatlabMenuItem ||
								*headerOptionsSelectionPtr == kTIFFGeoTIFFMenuItem)
				{
				if (dataValueTypeSelection < k4ByteSignedIntegerMenuItem)
					*headerOptionsSelectionPtr = kERDAS74MenuItem;
					
				else if (dataValueTypeSelection <= k8ByteRealMenuItem)
					*headerOptionsSelectionPtr = kArcViewMenuItem;
					
				else	// dataValueTypeSelection > k4ByteRealMenuItem
					*headerOptionsSelectionPtr = kNoneMenuItem;
				
				}	// end "if (*headerOptionsSelectionPtr == kNoneMenuItem || ..."
			/*	
			else if (*headerOptionsSelectionPtr == kArcViewMenuItem)
				{
				if (dataValueTypeSelection >= k8ByteRealMenuItem)
					*headerOptionsSelectionPtr = kNoneMenuItem;
				
				}	// end "else if (*headerOptionsSelectionPtr == kArcViewMenuItem)"
			*/	
			else if (*headerOptionsSelectionPtr == kERDAS74MenuItem)
				{
				if (dataValueTypeSelection >= k4ByteRealMenuItem)
					*headerOptionsSelectionPtr = kArcViewMenuItem;
				/*	
				if (dataValueTypeSelection >= k8ByteRealMenuItem)
					*headerOptionsSelectionPtr = kNoneMenuItem;
				*/
				}	// end "else if (*headerOptionsSelectionPtr == kERDAS74MenuItem)"
			break;
			
		case kBSQMenuItem:
			if (*headerOptionsSelectionPtr == kNoneMenuItem ||
						*headerOptionsSelectionPtr == kERDAS74MenuItem ||
								*headerOptionsSelectionPtr == kMatlabMenuItem)
				*headerOptionsSelectionPtr = kTIFFGeoTIFFMenuItem;
				
			else if (*headerOptionsSelectionPtr == kArcViewMenuItem)
				{
				if (dataValueTypeSelection >= k8ByteRealMenuItem)
					*headerOptionsSelectionPtr = kTIFFGeoTIFFMenuItem;
				
				}	// end "else if (*headerOptionsSelectionPtr == kArcViewMenuItem)"
			break;
			
		case kBISMenuItem:
			if (*headerOptionsSelectionPtr == kNoneMenuItem ||
						*headerOptionsSelectionPtr == kERDAS74MenuItem) // ||
								//*headerOptionsSelectionPtr == kTIFFGeoTIFFMenuItem)
				{
				if (dataValueTypeSelection <= k4ByteRealMenuItem)
					*headerOptionsSelectionPtr = kArcViewMenuItem;
					
				else	// dataValueTypeSelection >= k4ByteRealMenuItem
					*headerOptionsSelectionPtr = kNoneMenuItem;
				
				}	// end "if (*headerOptionsSelectionPtr == kMatlabMenuItem || ..."
				
			else if (*headerOptionsSelectionPtr == kArcViewMenuItem)
				{
				if (dataValueTypeSelection > k4ByteRealMenuItem)
					*headerOptionsSelectionPtr = kNoneMenuItem;
				
				}	// end "else if (*headerOptionsSelectionPtr == kArcViewMenuItem)"
				
			else if (*headerOptionsSelectionPtr == kMatlabMenuItem)
				{
				if (dataValueTypeSelection >= k4ByteRealMenuItem)
					*headerOptionsSelectionPtr = kNoneMenuItem;
				
				else if (dataValueTypeSelection < k4ByteRealMenuItem)
					*headerOptionsSelectionPtr = kArcViewMenuItem;
				
				}	// end "else if (*headerOptionsSelectionPtr == kArcViewMenuItem)"
			break;
						
		}	// end "switch (bandInterleaveSelection)"
	/*
	if (dataValueTypeSelection >= k2ByteSignedIntegerMenuItem)  
		SetDLogControlHilite (dialogPtr, IDC_SwapBytes, 0);
	
	else	// dataValueTypeSelection < k2ByteSignedIntegerMenuItem
		SetDLogControlHilite (dialogPtr, IDC_SwapBytes, 255);
	*/
	if (inputHeaderOptionsSelections != *headerOptionsSelectionPtr &&
										*headerOptionsSelectionPtr == kTIFFGeoTIFFMenuItem)
		ChangeImageFormatDialogUpdateTIFFHeader (dialogPtr,
																fileInfoPtr,
																outputFileSelectionPtr,
																NULL,
																popUpOutputFileMenu);
	
	ChangeImageFormatDialogUpdateSwapBytes (dialogPtr,
															fileInfoPtr,
															dataValueTypeSelection,
															*headerOptionsSelectionPtr);
	
	#if defined multispec_wx
				// Reset the file format selection. It may have changed above.
	
   	int headerListSelection = GetComboListSelection (dialogPtr,
   																		IDC_ChangeHeader,
																			*headerOptionsSelectionPtr);
   	wxChoice* fileFormatCtrl = (wxChoice*)dialogPtr->FindWindow (IDC_ChangeHeader);
   	fileFormatCtrl->SetSelection (headerListSelection);
	#endif
			
	#if defined multispec_mac  
		InvalidateDialogItemRect (dialogPtr, IDC_ChangeHeader);
	#endif	// defined multispec_mac 
	
}	// end "ChangeImageFormatDialogVerifyHeaderSetting"


                      
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		ChangeImageTransformDialog
//
//	Software purpose:	This routine gets the change image transform
//							specifications that will be used to adjust or
//							change the channel data values.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/06/1992
//	Revised By:			Larry L. Biehl			Date: 02/28/2019

Boolean ChangeImageTransformDialog (
				ReformatOptionsPtr				reformatOptionsPtr, 
				SInt16*								recommendedNumberOfBitsPtr)

{
	Boolean								OKFlag = FALSE;
	
#if defined multispec_mac
	double								lastAdjustFactor,
											lastAdjustDivisor,
											lastAdjustOffset,
											lastAdjustSelectedChannelsFactor,
											lastFunctionFactor,
											lastScaleFactor,
											lastTransformOffset,
											lastTransformFactor,
											maxAdjustDivisor,
											maxAdjustFactor,
											maxAdjustOffset,
											maxValue,
											minValue,
											realValue;
	
	Rect									theBox;
	
	DialogPtr							dialogPtr;
	
	UserItemUPP							drawFunctionPopUpPtr,
											drawPCChannelsPopUpPtr;
	
	Handle								okHandle,
											theHandle;
	
	SInt32								lastKthSmallestValue,
											theNum;
	
	SInt16								eigenSource,
											item,
											itemHit,
											lastAdjustSelectedChannel,
											minimumNumberBits,
											minimumSelectedNumberBits,
											numberTerms,
											stringID,
											theType;
	
	UInt16								numberEigenvectors;
	
	Boolean								modalDone,
											updateNumberBitsFlag;
	

			// Get the modal dialog for the image file format description			
				
	dialogPtr = LoadRequestedDialog (kChangeImageTransformID, kCopyScrap, 1, 2);
	if (dialogPtr == NULL)													
																						return (FALSE);
		
			// Intialize local user item proc pointers.									
			
	drawFunctionPopUpPtr = NewUserItemUPP (DrawFunctionPopUp);
	drawPCChannelsPopUpPtr = NewUserItemUPP (DrawPCChannelsPopUp);
	
			// Save handle for the OK button for use later.								
			
	GetDialogItem (dialogPtr, 1,  &theType, &okHandle, &theBox);
	
	minimumSelectedNumberBits = 8;

			//	Initialize the change image transform specification dialog	
	
			// Item 4: Adjust data radio button.											
			// Hide related items until the box is checked.								

	lastAdjustOffset = reformatOptionsPtr->adjustOffset;
	lastAdjustFactor = reformatOptionsPtr->adjustFactor;
	lastAdjustDivisor = reformatOptionsPtr->adjustDivisor;
	
	maxAdjustOffset = FLT_MAX;
	maxAdjustFactor = FLT_MAX;
	maxAdjustDivisor = FLT_MAX;
	
	maxValue = FLT_MAX;
	minValue = -FLT_MAX;
	
	itemHit = 0;
	
	SetDLogControl (dialogPtr, 
							4,
							(reformatOptionsPtr->transformDataCode == kAdjustChannel));
 
	if (reformatOptionsPtr->transformDataCode != kAdjustChannel)
		{
		HideDialogItems (dialogPtr, 5, 10);
		HideDialogItem (dialogPtr, 31);	
		
		}	// end "if (...->transformDataCode != kAdjustChannel)" 
		
	else	// reformatOptionsPtr->transformDataCode == kAdjustChannel 
		itemHit = 6;
	
	LoadDItemRealValue (dialogPtr, 6, lastAdjustOffset, 2);
	LoadDItemRealValue (dialogPtr, 8, lastAdjustFactor, 2);
	LoadDItemRealValue (dialogPtr, 10, lastAdjustDivisor, 2);
	
			// Use transformation radio button.		
	
	EigenvectorInfoExists (kNoProject, &eigenSource, &numberEigenvectors);
	
	lastScaleFactor = reformatOptionsPtr->pcScaleFactor;
	if (numberEigenvectors > 0)
		{
		SetDLogControl (dialogPtr, 
								11,
								(reformatOptionsPtr->transformDataCode == kCreatePCImage));
		
		if (eigenSource != 1)
			HideDialogItem (dialogPtr, 27);	
			
		if (eigenSource != 2)
			HideDialogItem (dialogPtr, 28);
			
		if (eigenSource < 16)
			HideDialogItem (dialogPtr, 32);
				
		if (reformatOptionsPtr->transformDataCode != kCreatePCImage)
			{
			HideDialogItems (dialogPtr, 12, 15);
			HideDialogItems (dialogPtr, 29, 30);
			
			}	// end "if (...->transformDataCode != kCreatePCImage && ...)" 
			
		else	// ...->transformDataCode == kCreatePCImage 
			itemHit = 30;
			
		//if (eigenSource == 2)
		//	lastScaleFactor = 10;
			
		SetDialogItemDrawRoutine (dialogPtr, 12, drawPCChannelsPopUpPtr);
		
		minimumNumberBits = DeterminePCMinimumNumberBits (
																lastScaleFactor, eigenSource, 0);
		LoadDItemValue (dialogPtr, 14, (SInt32)minimumNumberBits);
		
		minimumSelectedNumberBits = DeterminePCMinimumNumberBits (
																lastScaleFactor,
																eigenSource,
																reformatOptionsPtr->pcChannelPtr[0]);
		LoadDItemValue (dialogPtr, 15, (SInt32)minimumSelectedNumberBits);
			
		}	// end "if (numberEigenvectors > 0)" 
		
	else	// numberEigenvectors <= 0
		{
		HideDialogItems (dialogPtr, 11, 15);
		HideDialogItems (dialogPtr, 27, 30);
		HideDialogItem (dialogPtr, 32);
		
		}	// end "else numberEigenvectors <= 0" 
		
	gPCChannelSelection = kAllMenuItem;
	if (reformatOptionsPtr->numberPCChannels < numberEigenvectors)
		gPCChannelSelection = kSubsetMenuItem;
	
			// Use general algebraic transformation radio button.					
			
	if (gImageFileInfoPtr->thematicType)
		HideDialogItem (dialogPtr, 16);
	
	SetDLogControl (dialogPtr, 
							16,
							(reformatOptionsPtr->transformDataCode == kTransformChannels));
	if (reformatOptionsPtr->transformDataCode != kTransformChannels)
		HideDialogItems (dialogPtr, 17, 24);
		
	else	// reformatOptionsPtr->transformDataCode == kTransformChannels 
		itemHit = 18;
	
	if (!reformatOptionsPtr->algebraicStringsLoadedFlag)
		{
		if (!GetDefaultBandRatio (
								gImageWindowInfoPtr, gImageFileInfoPtr, reformatOptionsPtr))
			{
			reformatOptionsPtr->transformFactor = 1.;
			reformatOptionsPtr->transformOffset = 0.;
		
			}	// end "if (!GetDefaultBandRatio (gImageWindowInfoPtr, ..."
		
		reformatOptionsPtr->algebraicStringsLoadedFlag = TRUE;
		
		}	// end "if (!reformatOptionsPtr->algebraicStringSetFlag)"
		
	lastTransformOffset = reformatOptionsPtr->transformOffset;
	LoadDItemRealValue (dialogPtr, 18, (SInt32)lastTransformOffset, 3);
	
	LoadDItemString (dialogPtr, 21, (Str255*)&reformatOptionsPtr->numeratorString);
	
	LoadDItemString (dialogPtr, 22, (Str255*)&reformatOptionsPtr->denominatorString);
	
	lastTransformFactor = reformatOptionsPtr->transformFactor;
	LoadDItemRealValue (dialogPtr, 24, (SInt32)lastTransformFactor, 3);
		
	SetDLogControl (dialogPtr, 
							25,
							(reformatOptionsPtr->transformDataCode == kNoTransform));
			
	if (gImageFileInfoPtr->thematicType)
		HideDialogItem (dialogPtr, 16);
		
	SetDLogControl (dialogPtr, 
							26,
							(reformatOptionsPtr->transformDataCode == kFunctionOfChannels));
	if (reformatOptionsPtr->transformDataCode != kFunctionOfChannels)
		HideDialogItems (dialogPtr, 33, 35);
		
	else	// reformatOptionsPtr->transformDataCode == kTransformChannels 
		itemHit = 35;

	LoadDItemRealValue (dialogPtr, 30, lastScaleFactor, 1);
	
	HideDialogItem (dialogPtr, 31);
	
	gFunctionSelection = reformatOptionsPtr->functionCode;
	SetDialogItemDrawRoutine (dialogPtr, 33, drawFunctionPopUpPtr);
				
	lastFunctionFactor = reformatOptionsPtr->functionFactor;
	LoadDItemRealValue (dialogPtr, 35, lastFunctionFactor, 3);
	
	lastKthSmallestValue = reformatOptionsPtr->kthSmallestValue;
	LoadDItemValue (dialogPtr, 37, lastKthSmallestValue);
	if (reformatOptionsPtr->functionCode != kFunctionKthSmallestElement)
		{
		HideDialogItem (dialogPtr, 36);
		HideDialogItem (dialogPtr, 37);
		
		}	// end "if (...->functionCode != kFunctionKthSmallestElement)"
		
	SetDLogControl (
					dialogPtr,
					38,
					(reformatOptionsPtr->transformDataCode == kAdjustChannelsByChannel));
	if (reformatOptionsPtr->transformDataCode != kAdjustChannelsByChannel)
		HideDialogItems (dialogPtr, 39, 44);
		
	else	// reformatOptionsPtr->transformDataCode == kTransformChannels 
		itemHit = 42;

	lastAdjustSelectedChannelsFactor =
									reformatOptionsPtr->transformAdjustSelectedChannelsFactor;
	LoadDItemRealValue (dialogPtr, 42, lastAdjustSelectedChannelsFactor, 3);

	lastAdjustSelectedChannel = reformatOptionsPtr->transformAdjustSelectedChannel;
	LoadDItemValue (dialogPtr, 44, lastAdjustSelectedChannel);
				
			// Center the dialog and then show it.											
			
	ShowDialogWindow (dialogPtr, kChangeImageTransformID, kSetUpDFilterTable);
	
	gDialogItemDescriptorPtr[6] = kDItemReal + kDItemMinus;
	gDialogItemDescriptorPtr[8] = kDItemReal + kDItemMinus;
	gDialogItemDescriptorPtr[10] = kDItemReal + kDItemMinus;
	gDialogItemDescriptorPtr[18] = kDItemReal + kDItemMinus;
	gDialogItemDescriptorPtr[21] = kDItemString + kDItemSType1;
	gDialogItemDescriptorPtr[22] = kDItemString + kDItemSType1;
	gDialogItemDescriptorPtr[24] = kDItemReal + kDItemMinus;
	gDialogItemDescriptorPtr[30] = kDItemReal;
	gDialogItemDescriptorPtr[35] = kDItemReal;
	gDialogItemDescriptorPtr[42] = kDItemReal + kDItemMinus;
	
			// Set default text selection to first edit text item						
	
	if (itemHit > 0)
		SelectDialogItemText (dialogPtr, itemHit, 0, INT16_MAX);
							
	updateNumberBitsFlag = FALSE;	
	modalDone = FALSE;
	itemHit = 0;
	do 
		{
	   ModalDialog (gProcessorDialogFilterPtr, &itemHit);
		if (itemHit > 2)
			{
					// If itemHit was a radio button make appropriate control		
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
				case 4:	// Adjust selected channels
					SetControlValue ((ControlHandle)theHandle, 1);
					SetDLogControl (dialogPtr, 11, 0);
					SetDLogControl (dialogPtr, 16, 0);
					SetDLogControl (dialogPtr, 25, 0);
					SetDLogControl (dialogPtr, 26, 0);
					SetDLogControl (dialogPtr, 38, 0);
					
					ShowDialogItems (dialogPtr, 5, 10);
					HideDialogItems (dialogPtr, 12, 15);
					HideDialogItems (dialogPtr, 17, 24);
					HideDialogItems (dialogPtr, 29, 30);
					HideDialogItems (dialogPtr, 33, 37);
					HideDialogItems (dialogPtr, 39, 44);
						
					SelectDialogItemText (dialogPtr, 6, 0, INT16_MAX);						
					break;
					
				case 6:	// Offset to add 
					realValue = GetDItemRealValue (dialogPtr, 6);
					
							// Inititially did check here for bad values but decided not
							// to since bad values could arise as one was entering valid
							// values.

					//if (theNum < -maxAdjustOffset || theNum > maxAdjustOffset)
					//	NumberErrorAlert ((SInt32)lastAdjustOffset, dialogPtr, itemHit);
	
					//else	// theNum >= -maxAdjustOffset && ...
					lastAdjustOffset = realValue;
					break;
					
				case 8:	// Number to multiply by 
					realValue = GetDItemRealValue (dialogPtr, 8);
					lastAdjustFactor = realValue;
					break;
					
				case 10:				// Number to divide by 
					realValue = GetDItemRealValue (dialogPtr, 10);
					if (realValue == 0)
						RealNumberErrorAlert (lastTransformOffset, dialogPtr, 10, 2);
					else	// realValue != 0
						lastAdjustDivisor = realValue;
					break;
					
				case 11:				// New Channel from Principal Eigenvector 				
					SetControlValue ((ControlHandle)theHandle, 1);
					SetDLogControl (dialogPtr, 4, 0);
					SetDLogControl (dialogPtr, 16, 0);
					SetDLogControl (dialogPtr, 25, 0);
					SetDLogControl (dialogPtr, 26, 0);
					SetDLogControl (dialogPtr, 38, 0);					
					
					HideDialogItems (dialogPtr, 5, 10);
					ShowDialogItems (dialogPtr, 12, 15);
					HideDialogItems (dialogPtr, 17, 24);
					HideDialogItems (dialogPtr, 33, 37);
					HideDialogItems (dialogPtr, 39, 44);
					
					if (eigenSource == 1)
						{
						ShowDialogItem (dialogPtr, 27);
						HideDialogItem (dialogPtr, 28);		
						HideDialogItem (dialogPtr, 32);
						
						}	// end "if (eigenSource == 1)" 
						
					else if (eigenSource == 2)
						{
						HideDialogItem (dialogPtr, 27);
						ShowDialogItem (dialogPtr, 28);	
						HideDialogItem (dialogPtr, 32);	
						
						}	// end "else eigenSource == 2" 
						
					else	// eigenSource > 2
						{
						HideDialogItem (dialogPtr, 27);
						HideDialogItem (dialogPtr, 28);	
						ShowDialogItem (dialogPtr, 32);	
						
						}	// end "else eigenSource == 2" 
				
					ShowDialogItems (dialogPtr, 29, 30);
						
					SelectDialogItemText (dialogPtr, 30, 0, INT16_MAX);
					break;
					
				case 12:				// Principal Component Channels 					
					itemHit = StandardPopUpMenu (dialogPtr, 
															itemHit-1, 
															itemHit, 
															gPopUpAllSubsetMenu, 
															gPCChannelSelection, 
															kPopUpAllSubsetMenuID);
					if (itemHit == 2)
						{
								// Create subset of PC channels.							
								
						HiliteControl ((ControlHandle)okHandle, 255);
						
						ChannelsDialog ((SInt16*)&reformatOptionsPtr->numberPCChannels,
												reformatOptionsPtr->pcChannelPtr,
												NIL,
												NIL,
												4,
												kUseTransformation,
												NIL,
												numberEigenvectors,
												gPCChannelSelection);
								
						HiliteControl ((ControlHandle)okHandle, 0);
													
						if (reformatOptionsPtr->numberPCChannels == 
																gTransformationMatrix.numberFeatures)
							itemHit = 1;
						
						}	// end "if (itemHit == 2)" 
					
					if (itemHit != 0)
						gPCChannelSelection = itemHit;
						
					updateNumberBitsFlag = TRUE;
	
							// Make certain that the correct label is drawn in the	
							// channel pop up box.												
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
					
				case 16:				// New Channel from general Algebraic equation 				
					SetControlValue ((ControlHandle)theHandle, 1);
					SetDLogControl (dialogPtr, 4, 0);
					SetDLogControl (dialogPtr, 11, 0);
					SetDLogControl (dialogPtr, 25, 0);
					SetDLogControl (dialogPtr, 26, 0);
					SetDLogControl (dialogPtr, 38, 0);	
										
					HideDialogItems (dialogPtr, 5, 10);
					HideDialogItems (dialogPtr, 12, 15);
					ShowDialogItems (dialogPtr, 17, 24);
					HideDialogItems (dialogPtr, 29, 30);
					HideDialogItems (dialogPtr, 33, 37);					
					HideDialogItems (dialogPtr, 39, 44);
						
					SelectDialogItemText (dialogPtr, 18, 0, INT16_MAX);
					break;
					
				case 18:				// Transform Offset	
					realValue = GetDItemRealValue (dialogPtr, 18);
					if (realValue > maxValue || realValue < minValue)
						RealNumberErrorAlert (lastTransformOffset, dialogPtr, 18, 3);
					
					else	// realValue <= maxValue && realValue >= minValue 
						lastTransformOffset = realValue;
					break;
					
				case 21:				// Numerator	
					break;
					
				case 22:				// Denominator	
					break;
					
				case 24:				// Transform Factor	
					realValue = GetDItemRealValue (dialogPtr, 24);
					if (realValue > maxValue || realValue < minValue)
						RealNumberErrorAlert (lastTransformFactor, dialogPtr, 24, 3);
					
					else	// realValue <= maxValue && realValue >= minValue 
						lastTransformFactor = realValue;						
					break;
					
				case 25:				// No transformation 				
					SetControlValue ((ControlHandle)theHandle, 1);
					SetDLogControl (dialogPtr, 4, 0);
					SetDLogControl (dialogPtr, 11, 0);
					SetDLogControl (dialogPtr, 16, 0);
					SetDLogControl (dialogPtr, 26, 0);
					SetDLogControl (dialogPtr, 38, 0);
					
					HideDialogItems (dialogPtr, 5, 10);
					HideDialogItems (dialogPtr, 12, 15);
					HideDialogItems (dialogPtr, 17, 24);
					HideDialogItems (dialogPtr, 29, 30);
					HideDialogItems (dialogPtr, 33, 37);	
					HideDialogItems (dialogPtr, 39, 44);						
					break;
					
				case 26:				// Function of Selected Channels 				
					SetControlValue ((ControlHandle)theHandle, 1);
					SetDLogControl (dialogPtr, 4, 0);
					SetDLogControl (dialogPtr, 11, 0);
					SetDLogControl (dialogPtr, 16, 0);
					SetDLogControl (dialogPtr, 25, 0);
					SetDLogControl (dialogPtr, 38, 0);						
					
					HideDialogItems (dialogPtr, 5, 10);
					HideDialogItems (dialogPtr, 12, 15);
					HideDialogItems (dialogPtr, 17, 24);
					HideDialogItems (dialogPtr, 29, 30);
					ShowDialogItems (dialogPtr, 33, 35);
					HideDialogItems (dialogPtr, 39, 44);
					
					if (gFunctionSelection == kFunctionKthSmallestElement)
						ShowDialogItems (dialogPtr, 36, 37);
					
					else	// gFunctionSelection != kFunctionKthSmallestElement
						HideDialogItems (dialogPtr, 36, 37);
					break;
					
				case 30:				// Principal component scale factor. 
					realValue = GetDItemRealValue (dialogPtr, 30);
					if (realValue > maxValue)
						RealNumberErrorAlert (lastScaleFactor, dialogPtr, itemHit, 1);
							
					else	// realValue <= maxValue)
						{
						lastScaleFactor = realValue;
						updateNumberBitsFlag = TRUE;
						
						}	// end "else realValue <= maxValue)" 
					break;
					
				case 33:				// Function of Selected Channels 
					itemHit = StandardPopUpMenu (dialogPtr, 
															itemHit-1, 
															itemHit, 
															gPopUpFunctionMenu, 
															gFunctionSelection, 
															kPopUpFunctionMenuID);
					
					if (itemHit != 0)
						gFunctionSelection = itemHit;
						
					if (gFunctionSelection == kFunctionKthSmallestElement)
						{
						ShowDialogItem (dialogPtr, 36);
						ShowDialogItem (dialogPtr, 37);
						
						}	// end "if (gFunctionSelection == kFunctionKthSmallestElement)"
						
					else	// gFunctionSelection != kFunctionKthSmallestElement
						{
						HideDialogItem (dialogPtr, 36);
						HideDialogItem (dialogPtr, 37);
						
						}	// end "else gFunctionSelection != kFunctionKthSmallestElement"
						
							// Make certain that the correct label is drawn in the	
							// channel pop up box.												
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
					
				case 35:				// Function Factor	
					realValue = GetDItemRealValue (dialogPtr, 35);
					if (realValue > maxValue || realValue < minValue)
						RealNumberErrorAlert (lastFunctionFactor, dialogPtr, 35, 3);
					
					else	// realValue <= maxValue && realValue >= minValue 
						lastFunctionFactor = realValue;						
					break;
					
				case 37:				// kth smallest value	
					theNum = GetDItemValue (dialogPtr, 37);
					if (theNum > gImageWindowInfoPtr->totalNumberChannels)
						NumberErrorAlert (lastKthSmallestValue, dialogPtr, 37);
					
					else	// value <= gImageWindowInfoPtr->totalNumberChannels 
						lastKthSmallestValue = theNum;
					break;
					
				case 38:				// Adjust Selected Channels by Selected Channel				
					SetControlValue ((ControlHandle)theHandle, 1);
					SetDLogControl (dialogPtr, 4, 0);
					SetDLogControl (dialogPtr, 11, 0);
					SetDLogControl (dialogPtr, 16, 0);
					SetDLogControl (dialogPtr, 25, 0);
					SetDLogControl (dialogPtr, 26, 0);						
					
					HideDialogItems (dialogPtr, 5, 10);
					HideDialogItems (dialogPtr, 12, 15);
					HideDialogItems (dialogPtr, 17, 24);
					HideDialogItems (dialogPtr, 29, 30);
					HideDialogItems (dialogPtr, 33, 37);
					ShowDialogItems (dialogPtr, 39, 44);
					
					SelectDialogItemText (dialogPtr, 42, 0, INT16_MAX);
					break;
					
				case 42:				// Adjust selected channels with selected channel	
					realValue = GetDItemRealValue (dialogPtr, 42);
					if (realValue > maxValue || realValue < minValue)
						RealNumberErrorAlert (
											lastAdjustSelectedChannelsFactor, dialogPtr, 42, 3);
					
					else	// realValue <= maxValue && realValue >= minValue 
						lastAdjustSelectedChannelsFactor = realValue;						
					break;
					
				case 44:				// Channel to use for adjustment	
					theNum = GetDItemValue (dialogPtr, 44);
					if (theNum <= 0 || theNum > gImageWindowInfoPtr->totalNumberChannels)
						NumberErrorAlert (lastAdjustSelectedChannel, dialogPtr, 44);
					
					else	// value > 0 && <= gImageWindowInfoPtr->totalNumberChannels 
						lastAdjustSelectedChannel = theNum;
					break;
						
				}	// end "switch (itemHit)" 
				
			if (updateNumberBitsFlag)
				{
				minimumNumberBits = DeterminePCMinimumNumberBits (
															lastScaleFactor, eigenSource, 0);
				LoadDItemValue (dialogPtr, 14, (SInt32)minimumNumberBits);
			
				minimumSelectedNumberBits = DeterminePCMinimumNumberBits (
																lastScaleFactor, 
																eigenSource, 
																reformatOptionsPtr->pcChannelPtr[0]);
				LoadDItemValue (dialogPtr, 15, (SInt32)minimumSelectedNumberBits);
				
				updateNumberBitsFlag = FALSE;
				
				}	// end "updateNumberBitsFlag)" 
				
			}	// end "if (itemHit > 2)" 
			
		else	// itemHit <= 2 
			{																			
			if (itemHit == 1)
				{
				if (lastScaleFactor <= 0)
					{
					RealNumberErrorAlert (0, dialogPtr, 30, -1);
					itemHit = 0;
					
					}	// end "if (lastLineScaleFactor <= 0)" 
					
				}	// end "if (itemHit == 1)"
					
			if	(itemHit == 1)      // User selected OK for information 
				{
				modalDone = TRUE;
				OKFlag = TRUE;
	
						// Items 4,11,16: Get transform data code.									
 
				reformatOptionsPtr->transformDataCode = kNoTransform;
				if (GetDLogControl (dialogPtr, 4))
					reformatOptionsPtr->transformDataCode = kAdjustChannel;
					
				if (GetDLogControl (dialogPtr, 11))
					reformatOptionsPtr->transformDataCode = kCreatePCImage;
					
				if (GetDLogControl (dialogPtr, 16))
					reformatOptionsPtr->transformDataCode = kTransformChannels;
										
				if (GetDLogControl (dialogPtr, 26))
					reformatOptionsPtr->transformDataCode = kFunctionOfChannels;
										
				if (GetDLogControl (dialogPtr, 38))
					reformatOptionsPtr->transformDataCode = kAdjustChannelsByChannel;
			
						// Item 6: Number to offset data values by.						
						
				reformatOptionsPtr->adjustOffset = lastAdjustOffset;
			
						// Item 8: Number to multiply data values by.					
						
				reformatOptionsPtr->adjustFactor = lastAdjustFactor;
			
						// Item 10: Number to divide data values by.						
						
				reformatOptionsPtr->adjustDivisor = lastAdjustDivisor;
				
						// Set recommended number of bits.
				
				if (reformatOptionsPtr->transformDataCode == kAdjustChannel ||
							reformatOptionsPtr->transformDataCode == kFunctionOfChannels)
					minimumSelectedNumberBits = gImageFileInfoPtr->numberBits;
				
						// Get the list of principal component channels to create	
						// if transform code 3 has been selected.							
						
				if (reformatOptionsPtr->transformDataCode == kCreatePCImage)
					{
					reformatOptionsPtr->pcScaleFactor = lastScaleFactor;
					
					if (gPCChannelSelection == kAllMenuItem)
						{
						reformatOptionsPtr->numberPCChannels = 
															gTransformationMatrix.numberFeatures;
						for (item=0; item<gTransformationMatrix.numberFeatures; item++)
							reformatOptionsPtr->pcChannelPtr[item] = item;
								
						}	// end "if (gPCChannelSelection == kAllMenuItem)" 
						
					}	// end "if (...->transformDataCode == kCreatePCImage)" 
				
						// Decode the numerator and denominator if transform code	
						// 2 has been selected.													
						
				if (reformatOptionsPtr->transformDataCode == kTransformChannels)
					{
							// Item 18: Transform offset.										
						
					reformatOptionsPtr->transformOffset = lastTransformOffset;
				
							// Item 22: Decode the denominator.								
							
					GetDialogItem (dialogPtr, 22, &theType, &theHandle, &theBox);
					GetDialogItemText (theHandle, reformatOptionsPtr->denominatorString);
						
					item = 0;
					numberTerms = DecodeAlgebraicFormula (
										(unsigned char*)reformatOptionsPtr->denominatorString, 
										reformatOptionsPtr->coefficientsPtr, 
										reformatOptionsPtr->transformChannelPtr,
										reformatOptionsPtr->transformOperatorPtr);

					if (numberTerms >= 0)
						gReformatOptionsPtr->numberDenominatorTerms = numberTerms;
					else	// numberTerms < 0)
						item = 22;
									
							// Item 21: Decode the numerator.								
					
					if (item == 0)
						{	
						GetDialogItem (dialogPtr, 21, &theType, &theHandle, &theBox);
						GetDialogItemText (theHandle, reformatOptionsPtr->numeratorString);	
						
						numberTerms = DecodeAlgebraicFormula (
									(unsigned char*)reformatOptionsPtr->numeratorString, 
									&reformatOptionsPtr->coefficientsPtr[
													reformatOptionsPtr->numberDenominatorTerms], 
									&reformatOptionsPtr->transformChannelPtr[
													reformatOptionsPtr->numberDenominatorTerms],
									&reformatOptionsPtr->transformOperatorPtr[
													reformatOptionsPtr->numberDenominatorTerms]);
									
						if (numberTerms >= 0)
							gReformatOptionsPtr->numberNumeratorTerms = numberTerms;
						else	// numberTerms < 0)
							item = 21;
							
								// Estimate recommendation for minimum number bits needed.
								
						if (gReformatOptionsPtr->numberDenominatorTerms > 0)
							{
							if (lastTransformFactor <= 10)
										// This implies 32-bit real
								minimumSelectedNumberBits = 32;
							
							else if (lastTransformFactor <= 100)
								minimumSelectedNumberBits = 8;
							
							else
								minimumSelectedNumberBits = 16;
								
							}	// end "if (...->numberDenominatorTerms > 0)"
						
						}	// end "if (item == 0)" 
					
					if (item > 0)
						{
						modalDone = FALSE;

						stringID = IDS_Alert118 + abs (numberTerms) - 1;
								
						HiliteControl ((ControlHandle)okHandle, 255);
						DisplayAlert (kErrorAlert2ID, 
											kStopAlert,
											kAlertStrID,
											stringID,
											0,
											NIL);
						HiliteControl ((ControlHandle)okHandle, 0);
								
						SelectDialogItemText (dialogPtr, item, 0, INT16_MAX);
						
						}	// end "if (item > 0)" 
												
							// Item 24: Transform factor.										
						
					reformatOptionsPtr->transformFactor = lastTransformFactor;
					
					reformatOptionsPtr->transformChannelPtrSetFlag = FALSE;
					
					}	// end "if (...->transformDataCode == kTransformChannels)" 
					
				reformatOptionsPtr->eigenSource = eigenSource;
				
				reformatOptionsPtr->functionCode = gFunctionSelection;
				
				reformatOptionsPtr->functionFactor = lastFunctionFactor;
				
				*recommendedNumberOfBitsPtr = minimumSelectedNumberBits;
				
				reformatOptionsPtr->kthSmallestValue = lastKthSmallestValue; 
				
						// Adjust selected channels by selected channel.													
						
				if (reformatOptionsPtr->transformDataCode == kAdjustChannelsByChannel)
					{
					reformatOptionsPtr->transformAdjustSelectedChannelsFactor =
																	lastAdjustSelectedChannelsFactor;
				
					reformatOptionsPtr->transformAdjustSelectedChannel = 
																			lastAdjustSelectedChannel;
					
					}	// end "if (reformatOptionsPtr->transformDataCode == ..."
											
				}	// end "if (itemHit == 1)" 
				
			if	(itemHit == 2)      // User selected Cancel for information 
				{
				modalDone = TRUE;
				OKFlag = FALSE;
				
				}	// end "if	(itemHit == 2)" 
				
			}	// end "else itemHit <= 2" 
				
		}	while (!modalDone);
		
	DisposeUserItemUPP (drawPCChannelsPopUpPtr);
	DisposeUserItemUPP (drawFunctionPopUpPtr);
	
	CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
#endif	// defined multispec_mac
	
	return (OKFlag);
	
}	// end "ChangeImageTransformDialog" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				void CheckForSaturation
//
//	Software purpose:			This routine checks data values in an input line of data
//									for saturation, sets
//									those data value to the minimum or maximum value, counts
//									the number of saturations and notes what the min and max
//									values are.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/12/2006
//	Revised By:			Larry L. Biehl			Date: 06/23/2010

void CheckForSaturation (
				HUInt8Ptr							ioBufferPtr, 
				ReformatOptionsPtr				reformatOptionsPtr,
				UInt16								dataTypeCode, 
				UInt32								numberSamples, 
				double								lowSaturatedValue,
				double								highSaturatedValue)

{								
	double								highestValue,
											lowestValue;
	
	HSInt32Ptr							ioSInt32Ptr;
	HUInt32Ptr							ioUInt32Ptr;
	HDoublePtr 							ioDoublePtr;
	
	SInt32								highSaturationCount,
											lowSaturationCount;
	
	UInt32								j,
											numberChannels;
									
	
			// Declare local variables.														
	
	numberChannels = reformatOptionsPtr->numberChannels;
	lowestValue = reformatOptionsPtr->minimumValue;
	highestValue = reformatOptionsPtr->maximumValue;
	highSaturationCount = 0;
	lowSaturationCount = 0;
	
	switch (dataTypeCode)
		{
		case k4ByteSignedIntegerMenuItem:
		
			ioSInt32Ptr = (HSInt32Ptr)ioBufferPtr;
			
					// This makes sure that the low and high values are initialized.
					
			if (*ioSInt32Ptr < lowestValue)
				lowestValue = *ioSInt32Ptr;
			
			if (*ioSInt32Ptr > highestValue)
				highestValue = *ioSInt32Ptr;
			
			for (j=0; j<numberSamples; j++)
				{
				if (*ioSInt32Ptr < lowestValue)
					lowestValue = *ioSInt32Ptr;
				
				else if (*ioSInt32Ptr > highestValue)
					highestValue = *ioSInt32Ptr;
					
				if (*ioSInt32Ptr < lowSaturatedValue)
					{
					*ioSInt32Ptr = (SInt32)lowSaturatedValue;
					lowSaturationCount++;
					
					}	// end "if (*ioSInt32Ptr <= lowSaturatedValue)" 
					
				else if (*ioSInt32Ptr > highSaturatedValue)
					{
					*ioSInt32Ptr = (SInt32)highSaturatedValue;
					highSaturationCount++;
					
					}	// end "else if (*ioSInt32Ptr > highSaturatedValue)" 
					
				ioSInt32Ptr++;
				
				}	// end "for (j=0; j<numberSamples; j++)"
				
			break;
			
		case k4ByteUnsignedIntegerMenuItem:
		
			ioUInt32Ptr = (HUInt32Ptr)ioBufferPtr;
			
					// This makes sure that the low and high values are initialized.
					
			if (*ioUInt32Ptr < lowestValue)
				lowestValue = *ioUInt32Ptr;
			
			if (*ioUInt32Ptr > highestValue)
				highestValue = *ioUInt32Ptr;
			
			for (j=0; j<numberSamples; j++)
				{
				if (*ioUInt32Ptr < lowestValue)
					lowestValue = *ioUInt32Ptr;
				
				else if (*ioUInt32Ptr > highestValue)
					highestValue = *ioUInt32Ptr;
					
				if (*ioUInt32Ptr < lowSaturatedValue)
					{
					*ioUInt32Ptr = (UInt32)lowSaturatedValue;
					lowSaturationCount++;
					
					}	// end "if (*ioUInt32Ptr <= lowSaturatedValue)" 
					
				else if (*ioUInt32Ptr > highSaturatedValue)
					{
					*ioUInt32Ptr = (UInt32)highSaturatedValue;
					highSaturationCount++;
					
					}	// end "else if (*ioUInt32Ptr > highSaturatedValue)" 
					
				ioUInt32Ptr++;
				
				}	// end "for (j=0; j<numberSamples; j++)"
				
			break;

		case k8ByteRealMenuItem:
		
			ioDoublePtr = (HDoublePtr)ioBufferPtr;
			
					// Was going to do this on a pixel basis but decided to stay with
					// a data value basis for now.
					
			//channelInterval = numberSamples;
			//if (inputBISFlag)
			//	channelInterval = 1;
							
					// This makes sure that the low and high values are initialized.
					
			if (*ioDoublePtr < lowestValue)
				lowestValue = *ioDoublePtr;
			
			if (*ioDoublePtr > highestValue)
				highestValue = *ioDoublePtr;
			
			for (j=0; j<numberSamples; j++)
				{
				if (*ioDoublePtr < lowestValue)
					lowestValue = *ioDoublePtr;
				
				else if (*ioDoublePtr > highestValue)
					highestValue = *ioDoublePtr;
					
				if (*ioDoublePtr <= lowSaturatedValue)
					{
					*ioDoublePtr = lowSaturatedValue;
					lowSaturationCount++;
					
					}	// end "if (*ioDoublePtr <= lowSaturatedValue)" 
					
				else if (*ioDoublePtr >= highSaturatedValue)
					{
					*ioDoublePtr = highSaturatedValue;
					highSaturationCount++;
					
					}	// end "else if (*ioDoublePtr >= highSaturatedValue)" 
					
				ioDoublePtr++;
				
				}	// end "for (j=0; j<numberSamples; j++)"
				
			break;
			
		}	// end "switch (dataTypeCode)"
		
	reformatOptionsPtr->highSaturationCount += highSaturationCount;
	reformatOptionsPtr->lowSaturationCount += lowSaturationCount;
	reformatOptionsPtr->minimumValue = lowestValue;
	reformatOptionsPtr->maximumValue = highestValue;

}	// end "CheckForSaturation"   



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CleanUpChangeFormat
//
//	Software purpose:	The purpose of this routine is to dispose of the memory
//							represented by the input pointers for the Change Format
//							to BIL or BIS routine.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/08/1995
//	Revised By:			Larry L. Biehl			Date: 10/24/2013

void CleanUpChangeFormat (
				UInt16*						newPaletteIndexPtr,
				UInt16*						symbolToOutputBinPtr, 
				HDoublePtr					tempBufferPtr)

{ 
	CheckAndDisposePtr (tempBufferPtr);
	CheckAndDisposePtr (symbolToOutputBinPtr);
	CheckAndDisposePtr (newPaletteIndexPtr);
			
}	// end "CleanUpChangeFormat" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ConvertDataValueToBinValue
//
//	Software purpose:	This routine converts the input data value in a thematic type
//							multispectral display to the bin value used to display the data.
//							This is only needed for 32-bit and 64-bit data.  The final
//							conversion to class number is done in "ConvertSymbolsToBinary"
//							routine.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:			ChangeFormatToBILorBISorBSQ in SReformatChangeImageFileFormat.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/01/2006
//	Revised By:			Larry L. Biehl			Date: 10/24/2013

void ConvertDataValueToBinValue (
				HUInt8Ptr							ioBufferPtr,
				SInt32								signedValueOffset,
				double								binFactor,
				double								minValue,
				UInt32								maxBin,
				UInt32								numberSamples)

{
			// Declare local variables & structures	
			
	HDoublePtr	 						ioInDoubleBufferPtr;
												
	double								doubleBinIndex;
	UInt32								j;
	
	
			// For now we will keep in double format. It will be converted to
			// unsigned byte format at a later step to fit with the reformat
			// process that has been set up.  Some processing time could be saved
			// by converting to integer value here but will make the later steps
			// more complex.
			// Note that this code is almost the very same code in 
			// "Display1Channel4Byte8BitLine". The two sets of code need to stay
			// in sync.
			
	ioInDoubleBufferPtr = (HDoublePtr)ioBufferPtr;
			
	for (j=0; j<numberSamples; j++)
		{
		doubleBinIndex = 
				(*ioInDoubleBufferPtr+signedValueOffset - minValue)*binFactor + 1.00000001;
		if (doubleBinIndex < 0)
			doubleBinIndex = 0;
			
		else if (doubleBinIndex > (double)maxBin)
			doubleBinIndex = maxBin;
			
		else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin1
			{			
					// This will catch case when doubleBinIndex is nan or -nan. The
					// conversion from this can be a large unsigned 32-bit int.
					
			doubleBinIndex = MIN (doubleBinIndex, maxBin);
			
			}	// end "else doubleBinIndex >= 0 && doubleBinIndex <= maxBin1"
		
		*ioInDoubleBufferPtr = floor (doubleBinIndex);
		ioInDoubleBufferPtr++;
			
		}	// end "for (j=0; j<numberSamples; j++)" 
		
}	// end "ConvertDataValueToBinValue"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				void ConvertSymbolsToBinary
//
//	Software purpose:			This routine converts the input line of thematic
//									data which is an ascii representation of the
//									classes in the layer to a binary representation
//									which starts at data value 1.  A symbol of blank
//									will be converted to binary 0 by assuming that
//									this is a background class.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/07/1994
//	Revised By:			Larry L. Biehl			Date: 11/01/2006

void ConvertSymbolsToBinary (
				HUCharPtr							savedOutBufferPtr,
				UInt16*								symbolToOutputBinPtr, 
				Boolean								callConvertDataValueToBinValueFlag,
				SInt16								numberBytes,
				SInt32								numberSamples)

{
	SInt32								i;
	
	
	if (callConvertDataValueToBinValueFlag)
		{
		HUCharPtr					ioOut1BufferPtr;
		HUInt16Ptr					ioIn2BufferPtr;
		
		ioIn2BufferPtr = (HUInt16Ptr)savedOutBufferPtr;
		ioOut1BufferPtr = (HUCharPtr)savedOutBufferPtr;
			
		for (i=0; i<numberSamples; i++)
			{
			*ioOut1BufferPtr = (unsigned char)symbolToOutputBinPtr[*ioIn2BufferPtr];
						
			ioIn2BufferPtr++;
			ioOut1BufferPtr++;
			
			}	// end "for (i=0; i<numberSamples; i++)" 
		
		}	// end "if (callConvertDataValueToBinValueFlag)"
		
	else	// !callConvertDataValueToBinValueFlag
		{
		if (numberBytes == 1)
			{
			HUCharPtr					ioOut1BufferPtr;
		
			ioOut1BufferPtr = (HUCharPtr)savedOutBufferPtr;
			
			for (i=0; i<numberSamples; i++)
				{
				*ioOut1BufferPtr = (unsigned char)symbolToOutputBinPtr[*ioOut1BufferPtr];
				ioOut1BufferPtr++;
				
				}	// end "for (i=0; i<numberSamples; i++)" 
		
			}	// end "if (numberBytes == 1)" 
		
		else	// numberBytes != 1, assume numberBytes == 2 
			{
			HUInt16Ptr				ioOut2BufferPtr;
		
			ioOut2BufferPtr = (HUInt16Ptr)savedOutBufferPtr;
			
			for (i=0; i<numberSamples; i++)
				{
				*ioOut2BufferPtr = (UInt16)symbolToOutputBinPtr[*ioOut2BufferPtr];
				ioOut2BufferPtr++;
									
				}	// end "for (i=0; i<numberSamples; i++)" 
		
			}	// end "else numberBytes != 1" 
			
		}	// end "else !callConvertDataValueToBinValueFlag"
			
}	// end "ConvertSymbolsToBinary"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				HUInt16Ptr CreateClassToGroupTable
//
//	Software purpose:			This routine creates a table to be used to convert the
//									class value to the requested group value.
//
//	Parameters in:
//
//	Parameters out:
//
// Value Returned:
//
// Called By:			ChangeFormatToBILorBISorBSQ in SReformatChangeImageFileFormat.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/23/2004
//	Revised By:			Larry L. Biehl			Date: 01/09/2020

HUInt16Ptr CreateClassToGroupTable (
				FileInfoPtr							inFileInfoPtr,
				FileInfoPtr							outFileInfoPtr,
				UInt16								imageType,
				HUInt16Ptr							symbolToOutputBinPtr,
				HUInt16Ptr							newPaletteIndexPtr)

{
	HUCharPtr							inClassNamePtr;
	
	HUInt16Ptr							classToGroupPtr,
											groupToPalettePtr,
											inClassSymbolPtr,
											outClassSymbolPtr;
	
	Str31									*groupNamePtr,
											*outClassNamePtr;
	
	Handle								outDescriptionHandle;
	
	UInt32								index,
											numberBins;
	
	UInt16								classNumber,
											classOffset,
											groupNumber;
	
	Boolean								asciiSymbolConversionFlag,
											continueFlag;
	
	
	continueFlag = TRUE;
	outDescriptionHandle = NULL;
	
	asciiSymbolConversionFlag = FALSE;
	if (symbolToOutputBinPtr != NULL)
		asciiSymbolConversionFlag = TRUE;
	
				// Get vector to store the symbol to binary table in.
	
	if (symbolToOutputBinPtr == NULL)
		symbolToOutputBinPtr = GetSymbolToBinaryVector (outFileInfoPtr);
	
	if (symbolToOutputBinPtr == NULL)
		continueFlag = FALSE;
	
	if (continueFlag)
		{
		if (outFileInfoPtr->classDescriptionH == NULL ||
					outFileInfoPtr->classDescriptionH == inFileInfoPtr->classDescriptionH)
			
					// Get handle to store class description information in. The current
					// description handle in the outFileInfoPtr structure is the same
					// one as is used for the input MFileInfo structure.  A new one needs
					// to be created for the output file because the order and number
					// of the classes could be different.
			
			outDescriptionHandle = MNewHandle (
											(SInt32)(outFileInfoPtr->numberClasses + 1) *
																(sizeof (Str31) + sizeof (UInt16)));
		
		else	// outFileInfoPtr->classDescriptionH != NULL && ...
			outDescriptionHandle = outFileInfoPtr->classDescriptionH;
		
		if (outDescriptionHandle == NULL)
			continueFlag = FALSE;
		
		}	// end "if (continueFlag)"
	
	if (continueFlag)
		{
				// Get copy of group tables handle for output file so that the correct
				// palette can be generated. This is used by the TIFF/GeoTIFF format
				// generator.
		
		outFileInfoPtr->groupTablesHandle = inFileInfoPtr->groupTablesHandle;
		if (HandToHand (&outFileInfoPtr->groupTablesHandle) != noErr)
			continueFlag = FALSE;
		
		}	// end "if (continueFlag)"
	
   groupNamePtr = NULL;
   if (continueFlag)
		groupNamePtr = (Str31*)GetHandlePointer (inFileInfoPtr->groupNameHandle,
																kLock);
	
	if (groupNamePtr == NULL)
		continueFlag = FALSE;
	
	if (continueFlag)
		{
		outClassNamePtr = (Str31*)GetHandlePointer (outDescriptionHandle, kLock);
		
				// Get pointer to class to group vector.
		
		classToGroupPtr = (UInt16*)GetClassToGroupPointer (inFileInfoPtr);
		
				// Set up vector for the class values.
		
		if (asciiSymbolConversionFlag)
			{
					// Get pointer to class symbol table.
			
			inClassNamePtr = (HUCharPtr)GetHandlePointer (
																	inFileInfoPtr->classDescriptionH,
																	kLock);
			
			inClassSymbolPtr = (HUInt16Ptr)&inClassNamePtr[
													inFileInfoPtr->numberClasses*sizeof (Str31)];
			
			classOffset = 0;
			if (inClassSymbolPtr[0] != ' ' && inClassSymbolPtr[0] != 0)
					// This indicates that the background class was added for the
					// output file class/group vectors. Allow for this in when
					// indexing into the input file class to group vector.
				classOffset = 1;
			
			if (imageType == kMultispectralImageType)
				{
						// For multispectral image files being displayed as thematic images
						// use the number of bins being used for the display.
				
				numberBins = gToDisplayLevels.bytesOffset;
				numberBins = MIN (inFileInfoPtr->numberBins, numberBins);
			
				for (index=0; index<numberBins; index++)
					{
					classNumber = symbolToOutputBinPtr[index];
					groupNumber = classToGroupPtr[classNumber-classOffset];
					symbolToOutputBinPtr[index] = groupNumber;
					
					}	// end "for (index=0; index<numberBins; index++)"
				
				}	// end "if (imageType == kMultispectralImageType)"
			
			else	// imageType == kThematicImageType
				{
						// For thematic image file use the number of classes.
				
				for (index=0; index<outFileInfoPtr->numberClasses; index++)
					{
					classNumber = symbolToOutputBinPtr[inClassSymbolPtr[index]];
					groupNumber = classToGroupPtr[classNumber-classOffset];
					symbolToOutputBinPtr[inClassSymbolPtr[index]] = groupNumber;
					
					}	// end "for (index=0; index<numberBins; index++)"
				
				}	// end "else imageType == kThematicImageType"

					// Copy group name from input group structure to output file
					// class name structure.
			
			for (groupNumber=0; groupNumber<inFileInfoPtr->numberGroups; groupNumber++)
				BlockMoveData (&groupNamePtr[groupNumber],
									&outClassNamePtr[groupNumber],
									32);
			
			CheckAndUnlockHandle (inFileInfoPtr->classDescriptionH);
			
			}	// end "if (asciiSymbolConversionFlag)"
		
		else	// !asciiSymbolConversionFlag
			{
			for (index=0; index<outFileInfoPtr->numberClasses; index++)
				{
				groupNumber = classToGroupPtr[index];
				symbolToOutputBinPtr[index] = groupNumber;
				
						// Copy group name from input group structure to output file
						// class name structure.
				
		 		BlockMoveData (&groupNamePtr[groupNumber],
									&outClassNamePtr[groupNumber],
									32);
				
				}	// end "for (index=0; index<outFileInfoPtr->..."
			
			}	// end "else !asciiSymbolConversionFlag"
		
				// Save the new description handle for the output image data.
		
		outFileInfoPtr->classDescriptionH = outDescriptionHandle;
		
				// Indicate that the number of classes is now the number of groups.
		
		outFileInfoPtr->numberClasses = inFileInfoPtr->numberGroups;
		
				// The expectation is for there to be a class symbol vector stored
				// after the end of the class names. Create the class symbol vector
				// to indicate that all classes (actually groups being converted to
				// classes) will be used.
		
		outClassSymbolPtr = (UInt16*)&outClassNamePtr[outFileInfoPtr->numberClasses];
		for (index=0; index<outFileInfoPtr->numberClasses; index++)
			outClassSymbolPtr[index] = (UInt16)index;
	
		CheckAndUnlockHandle (inFileInfoPtr->groupNameHandle);
		CheckAndUnlockHandle (outDescriptionHandle);
		
				// Update the information to create the correct palette for the groups.
		
		groupToPalettePtr = (HUInt16Ptr)GetGroupToPalettePointer (inFileInfoPtr);
		
				// The classToGroup pointer will need to be adjusted to reflect that
				// the output file will have only 'number of group' classes.
				// Therefore, the location of the classToGroup pointer will have to be
				// change to reflect this. Also the classToGroup pointer will need
				// to just assume that the classes and groups are the same.
		
		UInt16 paletteOffset = GetPaletteOffset ();
		for (index=0; index<outFileInfoPtr->numberClasses; index++)
			newPaletteIndexPtr[index] = groupToPalettePtr[index] + paletteOffset;
		
		UnlockGroupTablesHandle (inFileInfoPtr);
		UnlockGroupTablesHandle (outFileInfoPtr);
	
		}	// end "if (continueFlag)"
	
	else	// !continueFlag
		{
		if (!asciiSymbolConversionFlag)
			{
			symbolToOutputBinPtr = CheckAndDisposePtr (symbolToOutputBinPtr);
			outDescriptionHandle = UnlockAndDispose (outDescriptionHandle);
			
			}	// end "if (!asciiSymbolConversionFlag)"
		
		}	// end "else !continueFlag"
	
	return (symbolToOutputBinPtr);
	
}	// end "CreateClassToGroupTable"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				HUInt16Ptr CreateDataValueToClassTable
//
//	Software purpose:			This routine creates a table to be used to convert the
//									data value to the class value for thematic multispectral
//									type of image displays.
//
//	Parameters in:
//
//	Parameters out:
//
// Value Returned:
//
// Called By:			ChangeFormatToBILorBISorBSQ in SReformatChangeImageFileFormat.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/26/2006
//	Revised By:			Larry L. Biehl			Date: 12/11/2012

HUInt16Ptr CreateDataValueToClassTable (
				FileInfoPtr							inFileInfoPtr,
				FileInfoPtr							outFileInfoPtr,
				HUInt16Ptr							symbolToOutputBinPtr,
				HUInt16Ptr							newPaletteIndexPtr)

{
	HUCharPtr							dataDisplayPtr;
	
	UInt32								index,
											numberBins;
	
	Boolean								continueFlag;
	
	
	continueFlag = TRUE;
	
				// Get vector to store the symbol to binary table in.
	
	if (symbolToOutputBinPtr == NULL)
		symbolToOutputBinPtr = GetSymbolToBinaryVector (inFileInfoPtr);
	
	if (symbolToOutputBinPtr == NULL)
		continueFlag = FALSE;
	
	if (gToDisplayLevels.vectorHandle == NULL)
		continueFlag = FALSE;

	if (continueFlag)
		{
		dataDisplayPtr = (HUCharPtr)GetHandlePointer (gToDisplayLevels.vectorHandle);
		
		numberBins = gToDisplayLevels.bytesOffset;
		numberBins = MIN (inFileInfoPtr->numberBins, numberBins);
	
		for (index=0; index<numberBins; index++)
			symbolToOutputBinPtr[index] = dataDisplayPtr[index];
		
		}	// end "if (continueFlag)"
	
	else	// !continueFlag
		{
		symbolToOutputBinPtr = CheckAndDisposePtr (symbolToOutputBinPtr);
		
		}	// end "else !continueFlag"
	
	return (symbolToOutputBinPtr);
	
}	// end "CreateDataValueToClassTable"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				UInt16* CreateNewToOldPaletteVector
//
//	Software purpose:			This routine creates a table to be used to
//									converts an ascii representation of the thematic
//									data to a binary representation which starts at 
//									data value 1.  The blank symbol will be converted 
//									to binary 0 under the assumption that
//									this is a background class.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:			ChangeFormatToBILorBISorBSQ in SReformatChangeImageFileFormat.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/07/1995
//	Revised By:			Larry L. Biehl			Date: 01/08/2020

UInt16* CreateNewToOldPaletteVector (
				FileInfoPtr							outFileInfoPtr,
				Boolean								asciiSymbolsFlag)

{  
	CMPaletteInfo 						paletteObject;
	
	UInt16								*inClassSymbolPtr,
											*newPaletteIndexPtr; 
	
	unsigned char*						inClassNamePtr;
										
	UInt32								index,
											newPaletteNumberClasses;
												
	UInt16								newPaletteIndex,
											numberPaletteEntries,
											oldPaletteIndex,
											paletteOffset;
				
	
	newPaletteIndexPtr = NULL;  
	
	paletteObject = GetPaletteHandle ();
	if (paletteObject == NULL)
																							return (NULL);
	
				// Get vector to store the symbol to binary table in.				
	
	if (outFileInfoPtr->classDescriptionH != NULL)
		{
		newPaletteNumberClasses = outFileInfoPtr->numberClasses + 1;
		newPaletteIndexPtr = (UInt16*)MNewPointer (
													newPaletteNumberClasses * sizeof (UInt16));
							
		}	// end "if (outFileInfoPtr->classDescriptionH != NULL)"
																				
	if (newPaletteIndexPtr != NULL)
		{					                                         
		paletteOffset = 0;					
		oldPaletteIndex = 0;							
		numberPaletteEntries = MGetNumberPaletteEntries (paletteObject);	
		
				// Force the last entry to be 0 in case it does not get set later.
				
		newPaletteIndexPtr[newPaletteNumberClasses-1] = 0;
						
		if (asciiSymbolsFlag)
			{
					// Get pointer to class symbol table.									
					// Remember to take into account that classSymbolPtr 				
					// points to a short int vector.										
			
			inClassNamePtr = (unsigned char*)GetHandlePointer (
															outFileInfoPtr->classDescriptionH, 
															kLock);
			inClassSymbolPtr = (UInt16*)&inClassNamePtr[
													outFileInfoPtr->numberClasses*sizeof (Str31)];
				
					// Set up  new palette vector.								
			
			newPaletteIndex = 0;
			if (inClassSymbolPtr[0] != ' ' && inClassSymbolPtr[0] != 0)
				{
						// Need to add entry for background since white will be entered
						// for it in the color table that will be use.
						
				newPaletteIndexPtr[newPaletteIndex] = 0;
				newPaletteIndex++;
				paletteOffset = 1;
				
				}	// end "if (inClassSymbolPtr[0] != ' ' && inClassSymbolPtr[0] != 0)"
			
			for (index=0; index<outFileInfoPtr->numberClasses; index++)
				{
						// Make certain that the palette index is within the 	
						// proper range.													
						
				if (oldPaletteIndex >= numberPaletteEntries)
					oldPaletteIndex = (oldPaletteIndex % numberPaletteEntries);
				/*
						// If this is the first class in the list (index is 0) and the 
						// symbol is blank then assume that this is the background class. 
						// Otherwise assume that the input data represents binary values 
						// and that class 32 (ascii blank) is a real class.
								
				if (index == 0 && 
							(inClassSymbolPtr[index] == ' ' || inClassSymbolPtr[index] == 0))
					{
					newPaletteIndexPtr[0] = oldPaletteIndex + paletteOffset;
					newPaletteIndex++;
					
					}	// end "if (index == 0 && inClassSymbolPtr[index] == ' ')" 
				
				else	// index != 0 || inClassSymbolPtr[index] != ' ' 
					{
					newPaletteIndexPtr[newPaletteIndex] = oldPaletteIndex + paletteOffset;
					newPaletteIndex++;
					
					}	// end "else index != 0 || inClassSymbolPtr[index] != ' '"
				*/
				newPaletteIndexPtr[newPaletteIndex] = oldPaletteIndex + paletteOffset;
					
				oldPaletteIndex++;
				newPaletteIndex++;
				
				}	// end "for (index=0; index<outFileInfoPtr->..." 
		
			CheckAndUnlockHandle (outFileInfoPtr->classDescriptionH);
			
			}	// end "if (asciiSymbolsFlag)"
			
		else	// !asciiSymbolsFlag
			{
			for (index=0; index<newPaletteNumberClasses; index++)
				{
						// Make certain that the palette index is within the 	
						// proper range.													
						
				if (oldPaletteIndex >= numberPaletteEntries)
					oldPaletteIndex = (oldPaletteIndex % numberPaletteEntries);
					
				newPaletteIndexPtr[index] = oldPaletteIndex + paletteOffset; 
				oldPaletteIndex++;
				
				}	// end "for (index=0; index<outFileInfoPtr->..."
			
			}	// end "else !asciiSymbolsFlag"
	
		}	// end "if (newPaletteIndexPtr != NULL)" 
		
	return (newPaletteIndexPtr);
			
}	// end "CreateNewToOldPaletteVector" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CreatePCImage
//
//	Software purpose:	This routine converts the input channels of data
//							into the requested Principal Component images
//							based on the saved eigenvalues and eigenvectors
//							found using the Principal Component processor.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:			ChangeFormatToBILorBISorBSQ in SReformatChangeImageFileFormat.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/18/1992
//	Revised By:			Larry L. Biehl			Date: 01/18/2006

void CreatePCImage (
				HDoublePtr							tempBufferPtr, 
				HDoublePtr							inputBufferPtr, 
				ReformatOptionsPtr				reformatOptionsPtr, 
				UInt32								numberSamples, 
				Boolean								inputBISFlag)

{
	double								value,
											scaleFactor,
											transformOffset;
											
	double*								channelMeanPtr;
	
	HDoublePtr							eigenVectorPtr,
											ioDoubleBufferPtr,
											lTempBufferPtr,
											outputBufferPtr;
		
	SInt16*								pcChannelPtr;
	
	SInt32								inputSampleInterval;
	
	UInt32								channelInterval,
											j,
											numberChannels,
											numberOutputChannels,
											numberPCComponents,
											outChan,
											outputSampleInterval;
											
	Boolean								eigenvectorTypeFlag;
	
	
			// Declare local variables.														
	
	scaleFactor = reformatOptionsPtr->pcScaleFactor;
	transformOffset = reformatOptionsPtr->transformOffset;
	numberChannels = reformatOptionsPtr->numberChannels;
	outputBufferPtr = (HDoublePtr)inputBufferPtr;
	eigenvectorTypeFlag = (gTransformationMatrix.createdByCode < 16);
	
	pcChannelPtr = reformatOptionsPtr->pcChannelPtr;
	numberOutputChannels = reformatOptionsPtr->numberPCChannels;
	numberPCComponents = reformatOptionsPtr->numberPCComponents;
	channelMeanPtr = reformatOptionsPtr->eigenValuePtr;
	eigenVectorPtr = reformatOptionsPtr->eigenVectorPtr;
	
	channelInterval = numberSamples;
	inputSampleInterval = 1;
	outputSampleInterval = 1;
	if (inputBISFlag)
		{
		channelInterval = 1;
		inputSampleInterval = numberChannels;
		outputSampleInterval = numberChannels;
		
		if (reformatOptionsPtr->convertType == 1)
					// Samples need to be packed ready to dump to disk for this	
					// conversion type.														
			outputSampleInterval = numberOutputChannels;
		
		}	// end "if (inputBISFlag)" 
	
	for (j=0; j<numberSamples; j++)
		{
		lTempBufferPtr = tempBufferPtr;
		
		for (outChan=0; outChan<numberOutputChannels; outChan++)
			{
			value = TransformDataVector (inputBufferPtr, 
													eigenVectorPtr,
													channelMeanPtr,
													numberChannels,
													eigenvectorTypeFlag,
													pcChannelPtr[outChan],
													channelInterval);
										
			value *= scaleFactor;
			value += transformOffset;
			*lTempBufferPtr = value;
			lTempBufferPtr++;
		
			}	// end "for (outChan=0; outChan<..." 
			
				// Store new output data back to ioBufferPtr location.				
				
		lTempBufferPtr = tempBufferPtr;
		ioDoubleBufferPtr = outputBufferPtr;
		for (outChan=0; outChan<numberOutputChannels; outChan++)
			{
			*ioDoubleBufferPtr = *lTempBufferPtr;
			ioDoubleBufferPtr += channelInterval;
			lTempBufferPtr++;
		
			}	// end "for (outChan=0; outChan<..." 
		
		inputBufferPtr += inputSampleInterval;
		outputBufferPtr += outputSampleInterval;
		
		}	// end "for (j=0; j<numberSamples; j++)" 
		
}	// end "CreatePCImage" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				HUInt16Ptr CreateSymbolToBinaryTable
//
//	Software purpose:			This routine creates a table to be used to
//									converts an ascii representation of the thematic
//									data to a binary representation which starts at 
//									data value 1.  The blank symbol will be converted 
//									to binary 0 under the assumption that
//									this is a background class.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:			ChangeFormatToBILorBISorBSQ in SReformatChangeImageFileFormat.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/07/1994
//	Revised By:			Larry L. Biehl			Date: 05/25/2012

HUInt16Ptr CreateSymbolToBinaryTable (
				FileInfoPtr							outFileInfoPtr,
				Boolean								compactFlag)

{
	HUCharPtr							outDescriptionPtr,
											inClassNamePtr,
											outClassNamePtr;
												
	HUInt16Ptr							inClassSymbolPtr,
											outClassSymbolPtr,
											symbolToOutputBinPtr; 
											
	Handle								outDescriptionHandle;
	
	UInt32								index,
											finalNumberClasses,
											outputNumberClasses;	
																				
	UInt16								binaryValue;
	
	Boolean								backgroundExistsFlag;
				
	
	symbolToOutputBinPtr = NULL;
	outDescriptionHandle = NULL;
	
				// Get vector to store the symbol to binary table in.				
	
	if (outFileInfoPtr->classDescriptionH != NULL)
		symbolToOutputBinPtr = GetSymbolToBinaryVector (outFileInfoPtr);
																				
	if (symbolToOutputBinPtr != NULL)
		
				// Get handle to store class description information in.			
				// The current description handle in the outFileInfoPtr			
				// structure is the same one as is used for the input MFileInfo	
				// structure.  A new one needs to be created for the output		
				// file because the order and number of the classes could be 	
				// different. Allow for a possible increase in the number of
				// classes to include a background class if it does not already
				// exist.															
				
		outDescriptionHandle = MNewHandle (
											(SInt32)(outFileInfoPtr->numberClasses + 1) * 
																(sizeof (Str31) + sizeof (UInt16)));
								
	if (outDescriptionHandle != NULL)
		{
				// Get pointer to class symbol table.									
				// Remember to take into account that classSymbolPtr 				
				// points to a short int vector.	Str15 is used instead of		
				// Str31.																		
		
		inClassNamePtr = (HUCharPtr)GetHandlePointer (
																outFileInfoPtr->classDescriptionH, 
																kLock);
		inClassSymbolPtr = (HUInt16Ptr)&inClassNamePtr[
												outFileInfoPtr->numberClasses*sizeof (Str31)];
									
		outDescriptionPtr = (HUCharPtr)GetHandlePointer (outDescriptionHandle, 
																			kLock);
																
				// Take into account here whether the number of classes will be increased
				// to allow for a background class which currently does not exist.
				
		finalNumberClasses = outFileInfoPtr->numberClasses;
		if (inClassSymbolPtr[0] != ' ' && inClassSymbolPtr[0] != 0)
			finalNumberClasses++;
			
		outClassSymbolPtr = (HUInt16Ptr)&outDescriptionPtr[
																finalNumberClasses*sizeof (Str31)];
									
		if (compactFlag)
			outputNumberClasses = outFileInfoPtr->numberClasses;
			
		else	// !compactFlag
			outputNumberClasses = inClassSymbolPtr[outFileInfoPtr->numberClasses-1];
			
				// Set up vector for the class symbols.								
		
		binaryValue = 1;
		backgroundExistsFlag = FALSE;
		outClassSymbolPtr[0] = 0;
		
		for (index=0; index<outputNumberClasses; index++)
			{		
					// If this is the first class in the list (index is 0) and the symbol
					// is blank then assume that this is the background class. Otherwise
					// assume that the input data represents binary values and that class
					// 32 (ascii blank) is a real class.
				
			if (index == 0 && 
							(inClassSymbolPtr[index] == ' ' || inClassSymbolPtr[index] == 0))
				{
				symbolToOutputBinPtr[inClassSymbolPtr[index]] = 0;
				backgroundExistsFlag = TRUE;
				outClassNamePtr = outDescriptionPtr;
				
				}	// end "if (index == 0 && inClassSymbolPtr[index] == ' ')" 
			
			else	// index != 0 || inClassSymbolPtr[index] != ' ' 
				{
				symbolToOutputBinPtr[inClassSymbolPtr[index]] = binaryValue;
				outClassNamePtr = &outDescriptionPtr[binaryValue*sizeof (Str31)];
				outClassSymbolPtr[binaryValue] = binaryValue;
				binaryValue++;
				
				}	// end "else index != 0 || inClassSymbolPtr[index] != ' ' " 
				
					// Copy class name from input to output file structures.		
				
	 		BlockMoveData (inClassNamePtr, outClassNamePtr, 32);
		 	inClassNamePtr += 32;
			
			}	// end "for (index=0; index<outFileInfoPtr->..." 
			
		if (!backgroundExistsFlag)
			{
			outFileInfoPtr->numberClasses++;
			BlockMoveData ("background", &outDescriptionPtr[1], 10);
			outDescriptionPtr[0] = 10;
			
			}	// end "if (!backgroundExistsFlag)"
	
		CheckAndUnlockHandle (outFileInfoPtr->classDescriptionH);
		CheckAndUnlockHandle (outDescriptionHandle);
		
				// Save the new description handle for the output image data.	
				
		outFileInfoPtr->classDescriptionH = outDescriptionHandle;
	
		}	// end "if (outDescriptionHandle != NULL)" 
		
	else	// outDescriptionHandle == NULL 
		symbolToOutputBinPtr = CheckAndDisposePtr (symbolToOutputBinPtr);
		
	return (symbolToOutputBinPtr);
			
}	// end "CreateSymbolToBinaryTable"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 DecodeAlgebraicFormula
//
//	Software purpose:	The purpose of this routine is to decode the input
//							string and set up the coefficients and channel numbers.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/13/1992
//	Revised By:			Larry L. Biehl			Date: 06/13/2006

SInt16 DecodeAlgebraicFormula (
				unsigned char*						stringPtr, 
				double*								coefficientsPtr, 
				SInt16*								transformChannelPtr,
				SInt16*								transformOperatorPtr)

{
	double								dValue;
	
	unsigned char*						nextStringPtr;
	
	SInt32								coefIndex,
											lValue,
											maxNumberOfChannels;
	

			// The input string is a pascal string.  Add the C terminating 			
			// to the end.																				
			
	maxNumberOfChannels = gImageWindowInfoPtr->totalNumberChannels;
	stringPtr[0] = MIN (stringPtr[0], 254);
	stringPtr[stringPtr[0]+1] = kNullTerminator;
	stringPtr++;
	coefIndex = 0;
	nextStringPtr = stringPtr;
	
	while (*nextStringPtr != kNullTerminator)
		{
		if (coefIndex > maxNumberOfChannels)
																							return (-2);
																						
				// Initialize a set for [operator]x.xCnn
				//   	Operator is + or -
				//		coefficient is 1
				//		channel number is -1 (not filled)
				
		transformOperatorPtr[coefIndex] = 1;
		coefficientsPtr[coefIndex] = 1;
		transformChannelPtr[coefIndex] = -1;
		
		stringPtr = nextStringPtr;
		
		if (*nextStringPtr == '*')
			{
			if (coefIndex == 0)
																							return (-2);
			
			transformOperatorPtr[coefIndex] = 2;
			nextStringPtr++;
			stringPtr = nextStringPtr;
			
			}	// end "if (*nextStringPtr == '*')"
																						
				// Get the coefficient if it exists including the sign (+ or -)
		
		dValue = strtod ((char*)stringPtr, (char**)&nextStringPtr);
		
		if (nextStringPtr != stringPtr)
			coefficientsPtr[coefIndex] = dValue;
			
		else	// nextStringPtr == stringPtr 
			{
			//if (*nextStringPtr == '+' || *nextStringPtr == '-' || *nextStringPtr == '*')
			if (*nextStringPtr == '+' || *nextStringPtr == '-')
				{
						// Note that coefficientsPtr[] has already been initialized for '+'
						
				if (*nextStringPtr == '-')
					coefficientsPtr[coefIndex] = -1;
					
				//else if (*nextStringPtr == '*')
				//	transformOperatorPtr[coefIndex] = 2;
					
				nextStringPtr++;
				
				}	// end "if (*nextStringPtr == '+' || *nextStringPtr == '-')" 
				
			if (*nextStringPtr != 'C' && *nextStringPtr != 'c')
																							return (-2);
																					
			}	// end "else nextStringPtr == stringPtr" 
		
				// Get the channel number if the next character signifies that a		
				// channel number follows.															
		
		if (*nextStringPtr == 'C' || *nextStringPtr == 'c')
			{
			nextStringPtr++;
			stringPtr = nextStringPtr;
			lValue = strtol ((char*)stringPtr, (char**)&nextStringPtr, 10);
			if (lValue > maxNumberOfChannels || lValue <= 0)
																							return (-1);
			if (nextStringPtr != stringPtr)
				transformChannelPtr[coefIndex] = (SInt16)lValue;
			
			else	// nextStringPtr == stringPtr 
																							return (-2);
																					
			if (*nextStringPtr == 'C' || *nextStringPtr == 'c')
																							return (-2);
				
			}	// end "if (*nextStringPtr == 'C' || *nextStringPtr == 'c')" 
			
		coefIndex++;
		
		}	// end "while (*nextStringPtr != kNullTerminator)" 
	
	return ((SInt16)coefIndex);
	
}	// end "DecodeAlgebraicFormula" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 DeterminePCMinimumNumberBits
//
//	Software purpose:	The purpose of this routine is to determine the
//							recommended minimum number of bits to properly
//							represent the input principal component.
//
//	Parameters in:		Principal Component.
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 05/21/1992
//	Revised By:			Larry L. Biehl			Date: 01/18/2006

SInt16 DeterminePCMinimumNumberBits (
				double								scaleFactor,
				SInt16								eigenSource, 
				SInt16								pcNumber)

{
	double								value;
	
	HDoublePtr							eigenValuePtr;
	
	int									exponent;
	
	SInt16								numberBits;


	if (eigenSource == 1)
		{
		eigenValuePtr = (HDoublePtr)GetHandlePointer (
														gTransformationMatrix.eigenValueHandle);
		value = sqrt (fabs (eigenValuePtr[pcNumber])) * 6 * scaleFactor;
		
		}	// end "if (eigenSource == 1)" 
		
	else	// eigenSource != 1 
		value = gImageFileInfoPtr->maxUsableDataValue - 
															gImageFileInfoPtr->minUsableDataValue;
	
			// Determine the recommended number of bits									
		
	value *= scaleFactor;
	value = frexp (value, &exponent);
	exponent++;
	
	if (exponent <= 8)
		numberBits = 8;
	
	else if (exponent > 8 && exponent <= 16)
		numberBits = 16;
	
	else if (exponent > 16 && exponent <= 32)
		numberBits = 32;
	
	else	// exponent > 32
		numberBits = 64;

	return (numberBits);
	
}	// end "DeterminePCMinimumNumberBits" 



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawChanDescriptionPopUp
//
//	Software purpose:	The purpose of this routine is to draw the channel
//							description popup for channel description options 
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
//	Coded By:			Larry L. Biehl			Date: 11/19/1991
//	Revised By:			Larry L. Biehl			Date: 11/19/1991	

pascal void DrawChanDescriptionPopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.									
			
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpChanDescriptionMenu, 
								gChanDescriptionSelection, 
								TRUE);
	
}	// end "DrawChanDescriptionPopUp" 
#endif	// defined multispec_mac



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawFunctionPopUp
//
//	Software purpose:	The purpose of this routine is to draw the principal
//							component channel set selection for the PC channel
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
//	Coded By:			Larry L. Biehl			Date: 12/04/2012
//	Revised By:			Larry L. Biehl			Date: 12/04/2012	

pascal void DrawFunctionPopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.									
			
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpFunctionMenu, 
								gFunctionSelection, 
								TRUE);
	
}	// end "DrawFunctionPopUp" 
#endif	// defined multispec_mac	



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawPCChannelsPopUp
//
//	Software purpose:	The purpose of this routine is to draw the principal
//							component channel set selection for the PC channel
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
//	Coded By:			Larry L. Biehl			Date: 05/20/1992
//	Revised By:			Larry L. Biehl			Date: 05/20/1992	

pascal void DrawPCChannelsPopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.									
			
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpAllSubsetMenu, 
								gPCChannelSelection, 
								TRUE);
	
}	// end "DrawPCChannelsPopUp" 
#endif	// defined multispec_mac	



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double Find_kth_SmallestElementInList
//
//	Software purpose:	The purpose of this routine is to find the kth smallest element
//							in the input vector of values.
//							Use the median () macro defined below to get the median
//							#define median (a,n)
//												kth_smallest (a,n,(((n)&1)?((n)/2):(((n)/2)-1)))
//
//							Algorithm from N. Wirth's book, implementation by N. Devillard.
//							This code in public domain.
//
//							Reference follows
//							Author: Wirth, Niklaus 
//                   Title: Algorithms + data structures = programs 
//							Publisher: Englewood Cliffs: Prentice-Hall, 1976 
//							Physical description: 366 p. 
//							Series: Prentice-Hall Series in Automatic Computation 
//
//	Parameters in:		Vector of values,
//							Number of elements in the vector,
//							The kth smallest element to be found.
//
//	Parameters out:	The value of the kth smallest element
//
// Value Returned:	None			
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/28/2013
//	Revised By:			Larry L. Biehl			Date: 02/28/2013

double Find_kth_SmallestElementInList (
				double*								vectorOfValuesPtr, 
				SInt32								numberElements, 
				SInt32								kthElement)
			
{
	SInt32								i,
											j,
											l,
											m;
											
	double								x;


	l = 0; 
	m = numberElements - 1;
	while (l<m) 
		{
		x = vectorOfValuesPtr[kthElement];
		i = l;
		j = m;
		do 
			{
			while (vectorOfValuesPtr[i]<x) 
				i++;
				
			while (x<vectorOfValuesPtr[j]) 
				j--;
				
			if (i<=j) 
				{
				SWAP_ELEMENTS (vectorOfValuesPtr[i],vectorOfValuesPtr[j]);
				i++; 
				j--;
				
				}	// end "if (i<=j)"
				
			} while (i<=j);
			
		if (j<kthElement) 
			l=i;
			
		if (kthElement<i) 
			m=j;
		
		}	// end "while (l<m)"
    
	return vectorOfValuesPtr[kthElement];
		
}	// end "Find_kth_SmallestElementInList"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				void FunctionOfChannels
//
//	Software purpose:			This routine finds the function of the input channels of 
//									data to one channel of output data. Examples are MIN, MAX
//									SUM, AVERAGE.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/11/2012
//	Revised By:			Larry L. Biehl			Date: 08/02/2013

void FunctionOfChannels (
				UInt32*								inputCountBufferPtr, 
				HUInt8Ptr							ioBufferPtr, 
				ReformatOptionsPtr				reformatOptionsPtr, 
				UInt32								numberSamples,
				double								noDataValue,
				Boolean								noDataValueFlag,
				Boolean								inputBISFlag)

{								
	double								*ioDoubleBufferPtr,
											*ioOutDoubleBufferPtr;
											
	double								functionFactor,
											medianValue,
											medianValue2;
											
	UInt32*								countVectorPtr;
	
	UInt32								bufferInterval,
											channel,
											j,
											kTH,
											kTH2,
											numberChannels;
											
	UInt32								minDataValue;
											
	SInt16								saveFunctionCode;
	
	
			// Declare local variables.														
	
	numberChannels = reformatOptionsPtr->numberChannels;
	functionFactor = reformatOptionsPtr->functionFactor;
	
	bufferInterval = 1;
	if (inputBISFlag)
		bufferInterval = numberChannels;
	
	ioDoubleBufferPtr = (HDoublePtr)ioBufferPtr;
	ioOutDoubleBufferPtr = (HDoublePtr)ioBufferPtr;
	
	saveFunctionCode = reformatOptionsPtr->functionCode;
	
			// This is here for special U2U Growing Degree Day case.
			
	//noDataValueFlag = TRUE;
	//noDataValue = 367;
	
	if (gMultiSpecWorkflowInfo.workFlowCode != 0)
		{
		minDataValue = gMultiSpecWorkflowInfo.thresholdValue;
		saveFunctionCode = gMultiSpecWorkflowInfo.functionCode;
		
		}	// end "if (gMultiSpecWorkflowInfo.workFlowCode != 0)"
	
	switch (saveFunctionCode)
		{
		case kFunctionMin:
				
					// Initialize new values with those for the first channel.
					// Note that since the output buffer being used is the same as
					// the input buffer, the first channel values are already loaded
					// into the correct locations. 
			/*	
			for (j=0; j<numberSamples; j++)
				{
				*ioOutDoubleBufferPtr = *ioDoubleBufferPtr;
					
				ioDoubleBufferPtr += bufferInterval;
				ioOutDoubleBufferPtr++;	
				
				}	// end "for (j=0; j<numberSamples; j++)"
			*/	
			ioDoubleBufferPtr += numberSamples;
			for (channel=1; channel<numberChannels; channel++)
				{
						// Find minimum of all channels.
					
				if (inputBISFlag)
					ioDoubleBufferPtr = (HDoublePtr)ioBufferPtr + channel;
				ioOutDoubleBufferPtr = (HDoublePtr)ioBufferPtr;
					
				for (j=0; j<numberSamples; j++)
					{	
					if (!noDataValueFlag || *ioDoubleBufferPtr != noDataValue)
						*ioOutDoubleBufferPtr = 
												MIN (*ioOutDoubleBufferPtr, *ioDoubleBufferPtr);
						
					ioDoubleBufferPtr += bufferInterval;
					ioOutDoubleBufferPtr += bufferInterval;	
						
					}	// end "for (j=0; j<numberSamples; j++)"
						
				}	// end "for (channel=0; channel<numberChannels; channel++)"	
			break;
				
		case kFunctionMax:
				
					// Initialize new values with those for the first channel.
					// Note that since the output buffer being used is the same as
					// the input buffer, the first channel values are already loaded
					// into the correct locations. 

			ioDoubleBufferPtr += numberSamples;
			for (channel=1; channel<numberChannels; channel++)
				{
						// Find maximum of all channels.
					
				if (inputBISFlag)
					ioDoubleBufferPtr = (HDoublePtr)ioBufferPtr + channel;					
				ioOutDoubleBufferPtr = (HDoublePtr)ioBufferPtr;
					
				for (j=0; j<numberSamples; j++)
					{	
					if (!noDataValueFlag || *ioDoubleBufferPtr != noDataValue)
						*ioOutDoubleBufferPtr = 
												MAX (*ioOutDoubleBufferPtr, *ioDoubleBufferPtr);
						
					ioDoubleBufferPtr += bufferInterval;
					ioOutDoubleBufferPtr += bufferInterval;	
						
					}	// end "for (j=0; j<numberSamples; j++)"
						
				}	// end "for (channel=0; channel<numberChannels; channel++)"	
			break;
				
		case kFunctionSum:
				
					// Initialize new values with those for the first channel.
					// Note that since the output buffer being used is the same as
					// the input buffer, the first channel values are already loaded
					// into the correct locations.
								
			ioDoubleBufferPtr += numberSamples;
			for (channel=1; channel<numberChannels; channel++)
				{
						// Find sum of all channels.
				
				if (inputBISFlag)
					ioDoubleBufferPtr = (HDoublePtr)ioBufferPtr + channel;	
				ioOutDoubleBufferPtr = (HDoublePtr)ioBufferPtr;
					
				for (j=0; j<numberSamples; j++)
					{	
					if (!noDataValueFlag || *ioDoubleBufferPtr != noDataValue)
						*ioOutDoubleBufferPtr += *ioDoubleBufferPtr;
						
					ioDoubleBufferPtr += bufferInterval;
					ioOutDoubleBufferPtr += bufferInterval;	
						
					}	// end "for (j=0; j<numberSamples; j++)"
						
				}	// end "for (channel=0; channel<numberChannels; channel++)"
			break;
			
		case kFunctionAverage:
				
					// Now verify that the first channel data is okay to use relative to
					// noDataFlag and initialize the count vector.
					// And initialize the count vector.
				
			countVectorPtr = inputCountBufferPtr;
			for (j=0; j<numberSamples; j++, countVectorPtr++)
				{
				if (!noDataValueFlag || *ioDoubleBufferPtr != noDataValue)
					*countVectorPtr = 1;
					
				else	// noDataValueFlag && *ioDoubleBufferPtr == noDataValue"
					{
					*ioDoubleBufferPtr = 0;
					*countVectorPtr = 0;
					
					}	// end "else noDataValueFlag && *ioDoubleBufferPtr == noDataValue"
					
				ioDoubleBufferPtr += bufferInterval;

				}	// end "for (j=0; j<numberSamples; j++, countVectorPtr++)"
									
			for (channel=1; channel<numberChannels; channel++)
				{
						// Find sum of all channels.
				
				if (inputBISFlag)
					ioDoubleBufferPtr = (HDoublePtr)ioBufferPtr + channel;	
				ioOutDoubleBufferPtr = (HDoublePtr)ioBufferPtr;
					
				countVectorPtr = inputCountBufferPtr;
				for (j=0; j<numberSamples; j++, countVectorPtr++)
					{	
					if (!noDataValueFlag || *ioDoubleBufferPtr != noDataValue)
						{
						*ioOutDoubleBufferPtr += *ioDoubleBufferPtr;
						(*countVectorPtr)++;
						
						}	// end "if (!noDataValueFlag || *ioDoubleBufferPtr != ..."
						
					ioDoubleBufferPtr += bufferInterval;
					ioOutDoubleBufferPtr += bufferInterval;	
						
					}	// end "for (j=0; j<numberSamples; j++, countVectorPtr++)"
						
				}	// end "for (channel=0; channel<numberChannels; channel++)"
				
					// Now get the average.
				
			ioOutDoubleBufferPtr = (HDoublePtr)ioBufferPtr;	
			countVectorPtr = inputCountBufferPtr;			
			for (j=0; j<numberSamples; j++, countVectorPtr++)
				{
				if (!noDataValueFlag || *ioOutDoubleBufferPtr != noDataValue)
					*ioOutDoubleBufferPtr /= *countVectorPtr;
					
				ioOutDoubleBufferPtr += bufferInterval;	
				
				}	// end "for (j=0; j<numberSamples; j++, countVectorPtr++)"
			break;
				
		case kFunctionMedian:
				
					// Determine the first middle kth value to use.
					// And the second middle kth value if needed. This occurs when the
					// number of channels is even. The median will be the average
					// of the 1st and 2nd median values.
					
			kTH = (numberChannels+1)/2 - 1;
			
			kTH2 = 0;
			if (numberChannels%2 == 0)
				kTH2 = kTH + 1;
				
			for (j=0; j<numberSamples; j++)
				{
				if (!noDataValueFlag || *ioDoubleBufferPtr != noDataValue)
					{
					medianValue = Find_kth_SmallestElementInList (
																ioDoubleBufferPtr, 
																numberChannels, 
																kTH);
							
							// If the number of channels is even, .
							
					if (kTH2 > 0)
						{
						medianValue2 = Find_kth_SmallestElementInList (
																	ioDoubleBufferPtr, 
																	numberChannels, 
																	kTH2);
																	
						medianValue = (medianValue + medianValue2)/2;
								
						}	// and "if (kTH2 > 0)"
					
					*ioOutDoubleBufferPtr = medianValue;
					
					}	// end "if (!noDataValueFlag || *ioDoubleBufferPtr != noDataValue)"
					
				else	// noDataValueFlag && *ioDoubleBufferPtr == noDataValue
					*ioOutDoubleBufferPtr = *ioDoubleBufferPtr;
					
				ioDoubleBufferPtr += bufferInterval;
				ioOutDoubleBufferPtr++;	
				
				}	// end "for (j=0; j<numberSamples; j++)"
			break;
				
		case kFunctionKthSmallestElement:
				
					// Determine the requested kth smallest value in the list of channel 
					// values.
					
			for (j=0; j<numberSamples; j++)
				{
				if (!noDataValueFlag || *ioDoubleBufferPtr != noDataValue)
					*ioOutDoubleBufferPtr = Find_kth_SmallestElementInList (
															ioDoubleBufferPtr, 
															numberChannels, 
															reformatOptionsPtr->kthSmallestValue-1);
					
				else	// noDataValueFlag && *ioDoubleBufferPtr == noDataValue
					*ioOutDoubleBufferPtr = *ioDoubleBufferPtr;
										
				ioDoubleBufferPtr += bufferInterval;
				ioOutDoubleBufferPtr++;	
				
				}	// end "for (j=0; j<numberSamples; j++)"
			break;
			
		case 98:		// special case
				
					// This case is to find the channels with the latest lowest value 
					// within the first 183 channels. The channel number (or Day of Year) 
					// is stored in the output file. This is for the Growing Degree Day 
					// data.
					
					// Initialize output data using just the first channel (day of year).
					
			numberChannels = MIN (numberChannels, 183);
			for (j=0; j<numberSamples; j++)
				{
				if (!noDataValueFlag || *ioDoubleBufferPtr != noDataValue)
					{
					if (*ioDoubleBufferPtr <= minDataValue)
						*ioOutDoubleBufferPtr = 1;
						
					else	// *ioDoubleBufferPtr > minDataValue
						*ioOutDoubleBufferPtr = 0;
						
					}	// end "if (!noDataValueFlag || *ioDoubleBufferPtr != noDataValue)"
					
				else	// noDataValueFlag && *ioDoubleBufferPtr == noDataValue
					*ioOutDoubleBufferPtr = 0;
					
				ioDoubleBufferPtr += bufferInterval;
				ioOutDoubleBufferPtr++;	
				
				}	// end "for (j=0; j<numberSamples; j++)"
				
					// Now go through the rest of the channels to find the latest channel
					// with a data value less than or equal to the defined minimum data
					// value.
	
			for (channel=1; channel<numberChannels; channel++)
				{					
				if (inputBISFlag)
					ioDoubleBufferPtr = (HDoublePtr)ioBufferPtr + channel;
				ioOutDoubleBufferPtr = (HDoublePtr)ioBufferPtr;
					
				for (j=0; j<numberSamples; j++)
					{	
					if (!noDataValueFlag || *ioDoubleBufferPtr != noDataValue)
						{
						if (*ioDoubleBufferPtr <= minDataValue)
							*ioOutDoubleBufferPtr = channel + 1;
							
						}	// end "if (!noDataValueFlag || *ioDoubleBufferPtr != ..."
						
					ioDoubleBufferPtr += bufferInterval;
					ioOutDoubleBufferPtr += bufferInterval;	
						
					}	// end "for (j=0; j<numberSamples; j++)"
						
				}	// end "for (channel=0; channel<numberChannels; channel++)"	
			break;
			
		case 99:		// special case
				
					// This case is to find the channels with the earliest lowest value 
					// within the last 182 or 183 channels. The channel number 
					// (or Day of Year) is stored in the output file. This is for the 
					// Growing Degree Day data.
					
					// Initialize the output data.
					
			for (j=0; j<numberSamples; j++)
				{
				*ioOutDoubleBufferPtr = 367;
					
				ioDoubleBufferPtr += bufferInterval;
				ioOutDoubleBufferPtr++;	
				
				}	// end "for (j=0; j<numberSamples; j++)"
				
					// Now go through the all of the channels to find the earliest channel 
					// between 183 and 365/366 with a data value less than or equal to the 
					// defined minimum data value.
	
			for (channel=1; channel<numberChannels; channel++)
				{
				if (inputBISFlag)
					ioDoubleBufferPtr = (HDoublePtr)ioBufferPtr + channel;
				ioOutDoubleBufferPtr = (HDoublePtr)ioBufferPtr;
					
				for (j=0; j<numberSamples; j++)
					{
					if (*ioOutDoubleBufferPtr == 367 && channel >= 183)
						{
						if (!noDataValueFlag || *ioDoubleBufferPtr != noDataValue)
							{
							if (*ioDoubleBufferPtr <= minDataValue)
								*ioOutDoubleBufferPtr = channel + 1;
								
							}	// end "if (!noDataValueFlag || ..."
							
						}	// end "if (*ioOutDoubleBufferPtr == 367 && channel >= 183)"
						
					ioDoubleBufferPtr += bufferInterval;
					ioOutDoubleBufferPtr += bufferInterval;	
						
					}	// end "for (j=0; j<numberSamples; j++)"
						
				}	// end "for (channel=0; channel<numberChannels; channel++)"	
			break;
			
		}	// end switch (saveFunctionCode)
		
	if (saveFunctionCode != kFunctionMedian && 
				saveFunctionCode != kFunctionKthSmallestElement && 
						inputBISFlag)
		{
				// The output values are in the first channel locations by every 
				// numberChannels since the input data is in BIS mode. Pack the data 
				// so that the columns are packed next to each other.		
					
		ioDoubleBufferPtr = (HDoublePtr)ioBufferPtr + bufferInterval;
		ioOutDoubleBufferPtr = (HDoublePtr)ioBufferPtr + 1;
			
		for (j=1; j<numberSamples; j++)
			{	
			*ioOutDoubleBufferPtr = *ioDoubleBufferPtr;
				
			ioDoubleBufferPtr += bufferInterval;	
			ioOutDoubleBufferPtr++;
				
			}	// end "for (j=0; j<numberSamples; j++)"
		
		}	// end "if (saveFunctionCode != kFunctionMedian && ..."
		
	if (functionFactor != 1)
		{
		ioOutDoubleBufferPtr = (HDoublePtr)ioBufferPtr;				
		for (j=0; j<numberSamples; j++)
			{
			if (!noDataValueFlag || *ioDoubleBufferPtr != noDataValue)
				*ioOutDoubleBufferPtr *= functionFactor;
			
			ioOutDoubleBufferPtr++;	
			
			}	// end "for (j=0; j<numberSamples; j++)"
		
		}	// end "if (functionFactor != 1)"		
	
}	// end "FunctionOfChannels" 	
		                                        


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetAdjustBufferData
//
//	Software purpose:	The purpose of this routine is to fill the adjust buffer with
//							the user requested data that will be used to adjust the selected
//							channels.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			GetReformatAndFileInfoStructures
//
//	Coded By:			Larry L. Biehl			Date: 04/11/2013
//	Revised By:			Larry L. Biehl			Date: 04/11/2013	

SInt16 GetAdjustBufferData (
				ReformatOptionsPtr				reformatOptionsPtr,
				HUCharPtr							ioBufferPtr2,
				HDoublePtr							ioOutAdjustBufferPtr,
				FileIOInstructionsPtr			fileIOInstructions2Ptr,
				UInt32								numberColumnBytes,
				UInt32								line,
				UInt32								columnStart,
				UInt32								columnEnd,
				UInt32								outputColumnInterval,
				UInt32								numberColumns,
				UInt32								numberChannels,
				Boolean								inputBISFlag)

{ 
	HDoublePtr							ioDoubleBufferPtr2;
	
	UInt32								column,
											inputBufferOffset;
	
	SInt16								errCode = noErr;
	
	
	if (reformatOptionsPtr->locationInList >= 0)
		{
				// Copy data to ioOutAdjustBufferPtr
		
		if (inputBISFlag)
			{
			ioDoubleBufferPtr2 = (HDoublePtr)ioBufferPtr2;
			ioDoubleBufferPtr2 += reformatOptionsPtr->locationInList;
			
			for (column=0; column<numberColumns; column++)
				{
				*ioOutAdjustBufferPtr = *ioDoubleBufferPtr2;
				
				ioDoubleBufferPtr2 += numberChannels;
				ioOutAdjustBufferPtr++;
				
				}	// end "for (column=0; column<numberColumns; column++)"
			
			}	// end "if (inputBISFlag)"
			
		else	// !inputBISFlag
			{
			inputBufferOffset = reformatOptionsPtr->locationInList * numberColumnBytes;
			memcpy ((char*)ioOutAdjustBufferPtr, 
						&ioBufferPtr2[inputBufferOffset], 
						numberColumnBytes);
			
			}	// end "else !inputBISFlag"

		}	// end "if (reformatOptionsPtr->locationInList >= 0)"
		
	else	// reformatOptionsPtr->locationInList < 0
		{
				// The data for the channel to be used to adjust the selected channels 
				// has not been read. Read the data directly to the adjust buffer location.
													
		errCode = GetLineOfData (fileIOInstructions2Ptr,
											line,
											columnStart,
											columnEnd,
											outputColumnInterval,
											gInputBuffer2Ptr,  
											(UCharPtr)ioOutAdjustBufferPtr);
		
		}	// end "else reformatOptionsPtr->locationInList < 0"	
		
	return (errCode);
	
}	// end "GetAdjustBufferData" 	
		                                        


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetDefaultBandRatio
//
//	Software purpose:	The purpose of this routine is to control the 
//							reformating utilities
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			GetReformatAndFileInfoStructures
//
//	Coded By:			Larry L. Biehl			Date: 03/25/2006
//	Revised By:			Larry L. Biehl			Date: 07/12/2018

Boolean GetDefaultBandRatio (
				WindowInfoPtr						windowInfoPtr,
				FileInfoPtr							fileInfoPtr,
				ReformatOptionsPtr				reformatOptionsPtr)

{ 
	Boolean								savedStringUsedFlag = FALSE,
											stringsSetFlag = FALSE;
	
	
	if (fileInfoPtr == NULL || windowInfoPtr == NULL)
																						return (FALSE);
																		
			// Load from saved structure if it has been set.
			
	if (gAlgebraicTransformationFunction.denominatorString[0] > 0)
		{
				// A transformation has been saved. Use it if the instrument code and
				// number of channels for the input image agree with that in the 
				// saved file.
				
		if (fileInfoPtr->instrumentCode == 
										gAlgebraicTransformationFunction.instrumentCode &&
				windowInfoPtr->totalNumberChannels == 
										gAlgebraicTransformationFunction.numberChannels)
			{
			CopyPToP (reformatOptionsPtr->denominatorString,
											gAlgebraicTransformationFunction.denominatorString);
			CopyPToP (reformatOptionsPtr->numeratorString,
											gAlgebraicTransformationFunction.numeratorString);
			reformatOptionsPtr->transformFactor = 
											gAlgebraicTransformationFunction.transformFactor;
			reformatOptionsPtr->transformOffset = 
											gAlgebraicTransformationFunction.transformOffset;
			
			stringsSetFlag = TRUE;
			savedStringUsedFlag = TRUE;
			
			}	// end "if (fileInfoPtr->instrumentCode == ..."
		
		}	// end "if (gAlgebraicTransformationFunction.denominatorString[0] > 0)"
		
	if (!stringsSetFlag)
		{
		SetRadiantTemperatureConstants (fileInfoPtr, reformatOptionsPtr);
		
		switch (fileInfoPtr->instrumentCode)
			{
			case kLandsatMSS:
			case kLandsatMSS5:
				if (windowInfoPtr->totalNumberChannels >= 4)
					{ 
					CopyPToP (reformatOptionsPtr->numeratorString, 
									(UCharPtr)"\0001.0C4-1.0C2\0");
					CopyPToP (reformatOptionsPtr->denominatorString, 
									(UCharPtr)"\0001.0C2+1.0C4\0");
					stringsSetFlag = TRUE;
					
					}	// end "if (windowInfoPtr->totalNumberChannels >= 4)"
				break;
				
			case kLandsatTM:
			case kLandsatTM7:
				if (windowInfoPtr->totalNumberChannels >= 4)
					{
					CopyPToP (reformatOptionsPtr->numeratorString, 
									(UCharPtr)"\0001.0C4-1.0C3\0");
					CopyPToP (reformatOptionsPtr->denominatorString, 
									(UCharPtr)"\0001.0C3+1.0C4\0");  
					stringsSetFlag = TRUE;
					
					}	// end "if (windowInfoPtr->totalNumberChannels >= 4)"
				break;
				
			case kLandsatLC8_OLI_TIRS:
			case kLandsatLC8_OLI:
				if (windowInfoPtr->totalNumberChannels >= 5)
					{
					CopyPToP (reformatOptionsPtr->numeratorString, 
									(UCharPtr)"\0001.0C5-1.0C4\0");
					CopyPToP (reformatOptionsPtr->denominatorString, 
									(UCharPtr)"\0001.0C4+1.0C5\0");  
					stringsSetFlag = TRUE;
					
					}	// end "if (windowInfoPtr->totalNumberChannels >= 5)"
				break;
				
			case kSPOT:
			case kSPOT4:
			case kSPOT5:
				if (windowInfoPtr->totalNumberChannels >= 3)
					{
					CopyPToP (reformatOptionsPtr->numeratorString, 
									(UCharPtr)"\0001.0C3-1.0C2\0");
					CopyPToP (reformatOptionsPtr->denominatorString, 
									(UCharPtr)"\0001.0C2+1.0C3\0");  
					stringsSetFlag = TRUE;
					
					}	// end "if (windowInfoPtr->totalNumberChannels >= 3)"
				break;
				
			case kSentinel2A_MSI:
			case kSentinel2B_MSI:
				if (windowInfoPtr->totalNumberChannels == 4)
					{
					CopyPToP (reformatOptionsPtr->numeratorString, 
									(UCharPtr)"\0001.0C4-1.0C3\0");
					CopyPToP (reformatOptionsPtr->denominatorString, 
									(UCharPtr)"\0001.0C3+1.0C4\0");  
					stringsSetFlag = TRUE;
					
					}	// end "if (windowInfoPtr->totalNumberChannels == 4)"

				else if (windowInfoPtr->totalNumberChannels == 13)
					{
					CopyPToP (reformatOptionsPtr->numeratorString, 
									(UCharPtr)"\0001.0C8-1.0C4\0");
					CopyPToP (reformatOptionsPtr->denominatorString, 
									(UCharPtr)"\0001.0C4+1.0C8\0");  
					stringsSetFlag = TRUE;
					
					}	// end "else if (windowInfoPtr->totalNumberChannels == 13)"
				break;
				
			case kPeruSat:
				if (windowInfoPtr->totalNumberChannels == 4)
					{
					if (windowInfoPtr->channelsInWavelengthOrderCode == kNotInOrder)
						{
						CopyPToP (reformatOptionsPtr->numeratorString,
										(UCharPtr)"\0001.0C4-1.0C1\0");
						CopyPToP (reformatOptionsPtr->denominatorString, 
										(UCharPtr)"\0001.0C1+1.0C4\0");
						stringsSetFlag = TRUE;
						
						}	// end "if (windowInfoPtr->channelsInWavelengthOrderCode == ..."
					
					else if (windowInfoPtr->channelsInWavelengthOrderCode == kInOrder)
						{
						CopyPToP (reformatOptionsPtr->numeratorString,
										(UCharPtr)"\0001.0C4-1.0C3\0");
						CopyPToP (reformatOptionsPtr->denominatorString, 
										(UCharPtr)"\0001.0C3+1.0C4\0");
						stringsSetFlag = TRUE;
						
						}	// end "if (windowInfoPtr->channelsInWavelengthOrderCode == ..."
					
					}	// end "if (windowInfoPtr->totalNumberChannels == 4)"
				break;
			
			}	// end "switch (fileInfoPtr->instrumentCode)"
			
		if (!stringsSetFlag)
			{
			switch (windowInfoPtr->totalNumberChannels)
				{
				case 1: 
					CopyPToP (reformatOptionsPtr->numeratorString, 
									(UCharPtr)"\0001.0C1\0");
					CopyPToP (reformatOptionsPtr->denominatorString, 
									(UCharPtr)"\0001.0C1\0"); 
					break;
					
				case 2: 
					CopyPToP (reformatOptionsPtr->numeratorString, 
									(UCharPtr)"\0001.0C2-1.0C1\0");
					CopyPToP (reformatOptionsPtr->denominatorString, 
									(UCharPtr)"\0001.0C1+1.0C2\0"); 
					break;
					
				case 3:
					CopyPToP (reformatOptionsPtr->numeratorString, 
									(UCharPtr)"\0001.0C3-1.0C2\0");
					CopyPToP (reformatOptionsPtr->denominatorString, 
									(UCharPtr)"\0001.0C2+1.0C3\0");  
					break;
					
				case 4:
				case 5:
					CopyPToP (reformatOptionsPtr->numeratorString, 
									(UCharPtr)"\0001.0C4-1.0C2\0");
					CopyPToP (reformatOptionsPtr->denominatorString, 
									(UCharPtr)"\0001.0C2+1.0C4\0");  
					break;
					
				case 6:
				case 7:
				case 8:
					CopyPToP (reformatOptionsPtr->numeratorString, 
									(UCharPtr)"\0001.0C4-1.0C3\0");
					CopyPToP (reformatOptionsPtr->denominatorString, 
									(UCharPtr)"\0001.0C3+1.0C4\0");  
					break;
					
				case 9:		// Assume Advance Land Imager
					CopyPToP (reformatOptionsPtr->numeratorString, 
									(UCharPtr)"\0001.0C5-1.0C4\0");
					CopyPToP (reformatOptionsPtr->denominatorString, 
									(UCharPtr)"\0001.0C4+1.0C5\0");  
					break;
					
				case 12:		// Assume Michigan Scanner
					CopyPToP (reformatOptionsPtr->numeratorString, 
									(UCharPtr)"\0001.0C11-1.0C9\0");
					CopyPToP (reformatOptionsPtr->denominatorString, 
									(UCharPtr)"\0001.0C9+1.0C11\0");  
					break;
					
				case 120:		// Assume ITD Imaging Spectrometer
					CopyPToP (reformatOptionsPtr->numeratorString, 
									(UCharPtr)"\0001.0C90-1.0C68\0");
					CopyPToP (reformatOptionsPtr->denominatorString, 
									(UCharPtr)"\0001.0C68+1.0C90\0");  
					break;
					
				case 126:		// Assume HYMAP Imaging Spectrometer
					CopyPToP (reformatOptionsPtr->numeratorString, 
									(UCharPtr)"\0001.0C32-1.0C16\0");
					CopyPToP (reformatOptionsPtr->denominatorString, 
									(UCharPtr)"\0001.0C16+1.0C32\0");  
					break;
					
				case 191:		// Assume HYDICE Imaging Spectrometer
				case 210:		// Assume HYDICE Imaging Spectrometer
					CopyPToP (reformatOptionsPtr->numeratorString, 
									(UCharPtr)"\0001.0C63-1.0C52\0");
					CopyPToP (reformatOptionsPtr->denominatorString, 
									(UCharPtr)"\0001.0C52+1.0C63\0");  
					break;
					
				case 220:		// Assume AVIRIS Imaging Spectrometer
				case 224:		// Assume AVIRIS Imaging Spectrometer
					CopyPToP (reformatOptionsPtr->numeratorString, 
									(UCharPtr)"\0001.0C50-1.0C27\0");
					CopyPToP (reformatOptionsPtr->denominatorString, 
									(UCharPtr)"\0001.0C27+1.0C50\0");  
					break;
					
				default:
					CopyPToP (reformatOptionsPtr->numeratorString, 
									(UCharPtr)"\0001.0C4-1.0C3\0");
					CopyPToP (reformatOptionsPtr->denominatorString, 
									(UCharPtr)"\0001.0C3+1.0C4\0");  
					
				}	// end "switch (windowInfoPtr->totalNumberChannels)"
			
			}	// end "if (!stringsSetFlag)"
			
		}	// end "if (!stringsSetFlag)"
		
	return (savedStringUsedFlag);
		
}	// end "GetDefaultBandRatio" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetHeaderFormatFromPopUpSelection
//
//	Software purpose:	This routine returns the header format code based on the input
//							header popup selection value.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/24/2006
//	Revised By:			Larry L. Biehl			Date: 03/24/2006

SInt16 GetHeaderFormatFromPopUpSelection (
				SInt16		 						headerOptionsSelection)

{	
	SInt16								headerFormat;
	

	headerFormat = kNoneType;
	
	if (headerOptionsSelection == kArcViewMenuItem)
		headerFormat = kArcViewType;
	
	else if (headerOptionsSelection == kERDAS74MenuItem)
		headerFormat = kErdas74Type;
	
	else if (headerOptionsSelection == kGAIAMenuItem)
		headerFormat = kGAIAType;
	
	else if (headerOptionsSelection == kTIFFGeoTIFFMenuItem)
		headerFormat = kTIFFType;
	
	else if (headerOptionsSelection == kMatlabMenuItem)
		headerFormat = kMatlabType;
	
	return (headerFormat);
	
}	// end "GetHeaderFormatFromPopUpSelection" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt32 GetNumberHeaderBytes
//
//	Software purpose:	This routine determines the number of bytes needed
//							when writing the specified image header.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/09/1992
//	Revised By:			Larry L. Biehl			Date: 10/09/1992

SInt32 GetNumberHeaderBytes (
				FileInfoPtr 						outFileInfoPtr)

{
	SInt32								numberHeaderBytes;
	
	
	numberHeaderBytes = 0;
	
	switch (outFileInfoPtr->format)
		{
		//case kErdas73Type:	// Not allow ERDAS73 format now.
		case kErdas74Type:
			numberHeaderBytes = 128;
			break;
			
		case kVICARType:
			break;
			
		case kGAIAType:
					// This is unknown until the file is actually read.				
					// 10,000 allows for 70 channels and 256 classes in the			
					// GAIA statistics deck.													
					
			numberHeaderBytes = 10000;
			break;
			
		case kMatlabType:
					// This is an estimate.														
			numberHeaderBytes = 160;
			break;
			
		case kTIFFType:
		case kGeoTIFFType:
					// This is an estimate.														
			numberHeaderBytes = 500;
			break;
			
		}	// end "switch (outFileInfoPtr->format)" 
	
	return (numberHeaderBytes);
	
}	// end "GetNumberHeaderBytes" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetPCChannelList
//
//	Software purpose:	The purpose of this routine is to determine the
// 						channels that need to be read to create a principal
//							component transformation of the data.  This
//							is the same channel list as that used for the last
//							run of the principal component processor.  This
//							routine also sets up the handles to the eigenvectors
//							and eigenvalues that will be used.
//
//	Parameters in:		Pointer to reformat instruction structure.
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			ChangeImageFormatDialog
//
//	Coded By:			Larry L. Biehl			Date: 05/19/1992
//	Revised By:			Larry L. Biehl			Date: 04/26/1993

SInt16 GetPCChannelList (
				ReformatOptionsPtr				reformatOptionsPtr)

{
	SInt16								*channelPtr,
											*pcChannelsPtr;
	
	SInt32								index,
											maxNumberOfChannels;


	maxNumberOfChannels = gTransformationMatrix.numberChannels;
	reformatOptionsPtr->numberPCComponents = gTransformationMatrix.numberFeatures;
	reformatOptionsPtr->eigenValueHandle = gTransformationMatrix.eigenValueHandle;
	reformatOptionsPtr->eigenVectorHandle = gTransformationMatrix.eigenVectorHandle;
	pcChannelsPtr = (SInt16*)GetHandlePointer (
													gTransformationMatrix.eigenFeatureHandle);
	
	channelPtr = reformatOptionsPtr->channelPtr;
	for (index=0; index<maxNumberOfChannels; index++)
		{
		*channelPtr = *pcChannelsPtr;
		channelPtr++;
		pcChannelsPtr++;
		
		}	// end "for (index=0; index<; index++)" 
	
	reformatOptionsPtr->numberChannels = (UInt16)maxNumberOfChannels;
		
	return (1);
	
}	// end "GetPCChannelList" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetReformatAndFileInfoStructures
//
//	Software purpose:	The purpose of this routine is to get the memory
//							for the reformat options structure and the file
//							information structure that will be used for an
//							output file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None 				
// 
// Called By:			CovertMultispectralToThematicControl in SFieldsToThematicFile.cpp
//							LoadAreasToThematicSpecs in SFieldsToThematicFile.cpp 
//							MosaicTwoImagesControl in SMosaic.cpp
//							RectifyImageControl in SRectifyImage.cpp
//							ChangeImageFileFormat in SReformatChangeImageFileFormat.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/31/1993
//	Revised By:			Larry L. Biehl			Date: 02/26/2013

Boolean GetReformatAndFileInfoStructures (
				Handle*								fileInfoHPtr, 
				Handle*								reformatOptionsHPtr)

{
	ReformatOptionsPtr				reformatOptionsPtr;
	
	Boolean								continueFlag;


	if (reformatOptionsHPtr == NULL)
																						return (FALSE);

	*reformatOptionsHPtr = NULL;
	continueFlag = TRUE;
	
	if (fileInfoHPtr != NULL)
		{
				// Get a handle to a block of memory to be used for					
				// file information for the output file.									
				// Lock the memory until the information has been loaded				
				// in.										
			
				// Initialize the variables and handles in the structure.			
				
		*fileInfoHPtr = InitializeFileInfoStructure (*fileInfoHPtr, kNotPointer);
			 
		if (*fileInfoHPtr == NULL)
			continueFlag = FALSE;
			
		}	// end "if (fileInfoHPtr != NULL)" 
			
			// Get a handle to a block of memory to use for the reformatting		
			// options.  Then lock it and get a pointer to the block.				
	
	if (continueFlag)
		{
		continueFlag = LoadReformatOptionsSpecs (gImageWindowInfoPtr);

		*reformatOptionsHPtr = gNonProjProcessorSpecs.reformatOptionsSpecsH;	
		reformatOptionsPtr = (ReformatOptionsPtr)GetHandlePointer (
																		*reformatOptionsHPtr, kLock);
			
		if (continueFlag && fileInfoHPtr != NULL)
			reformatOptionsPtr->outFileInfoHandle = *fileInfoHPtr;
					
		}	// end "if (continueFlag)" 
		
	return (continueFlag);
		
}	// end "GetReformatAndFileInfoStructures" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetReformatOutputBuffer
//
//	Software purpose:	The purpose of this routine is to obtain a buffer to be 
//							used for the output image file. Try to get as large a 
//							buffer as possible.  Leave 20000 bytes free if possible.
//							The smallest unit will be one line of all columns and all
//							channels.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			ChangeImageFileFormat in SReformatChangeImageFileFormat.cpp
//							AreasToThematicFileControl in SFieldsToThematicFile.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/29/1990
//	Revised By:			Larry L. Biehl			Date: 03/22/2019

Boolean GetReformatOutputBuffer (
				FileInfoPtr							outFileInfoPtr, 
				ReformatOptionsPtr				reformatOptionsPtr)

{
	SInt64								lContBlock;
	
	SInt32								extraBufferBytesNeeded;
	
	UInt32								countOutBytes,
											numberBytes,
											numberOutputBufferLines,
											totalNumberBytes;
	
	Boolean								continueFlag;


			// Check input parameters.															
			
	if (outFileInfoPtr == NULL)
																						return (FALSE);
		
			// Initialize local variables.													
																							
	continueFlag = TRUE;
	
			// First get number of bytes for minimum image group.  This 			
			// will be one line and one channel for BSQ formats and					
			// One line, all channels for BIL and BIS formats.							
			// Also get the maximum number of bytes that can be used.				
			
	if (outFileInfoPtr->bandInterleave != kBSQ)
		{
		countOutBytes = (SInt32)outFileInfoPtr->numberColumns *
				outFileInfoPtr->numberChannels * outFileInfoPtr->numberBytes +
						outFileInfoPtr->numberPreLineBytes + 
														outFileInfoPtr->numberPostLineBytes;
														
		if (outFileInfoPtr->format == kMatlabType)
			countOutBytes *= 8/outFileInfoPtr->numberBytes;
		
		totalNumberBytes = countOutBytes;
		if (gProcessorCode == kRefChangeFileFormatProcessor ||
					gProcessorCode == kRectifyImageProcessor ||
							gProcessorCode == kRefMosaicImagesProcessor ||
									gProcessorCode == kRefShapeToThematicFileProcessor)
			totalNumberBytes *= outFileInfoPtr->numberLines;
	
		}	// end "if (outFileInfoPtr->bandInterleave != kBSQ)" 
	
	else	// outFileInfoPtr->bandInterleave == kBSQ 
		{
		countOutBytes = (SInt32)outFileInfoPtr->numberColumns *
								outFileInfoPtr->numberBytes + 
										outFileInfoPtr->numberPreLineBytes + 
													outFileInfoPtr->numberPostLineBytes;
	
		totalNumberBytes = countOutBytes * outFileInfoPtr->numberChannels *
																		outFileInfoPtr->numberLines;
												
		if (gProcessorCode == kRefChangeFileFormatProcessor ||
					gProcessorCode == kRectifyImageProcessor ||
							gProcessorCode == kRefMosaicImagesProcessor ||
									gProcessorCode == kRefShapeToThematicFileProcessor)
			countOutBytes *= outFileInfoPtr->numberChannels;
		
		}	// end "else outFileInfoPtr->bandInterleave == BSQ" 
		
	if (totalNumberBytes == 0)
																						return (FALSE);
		
			// Get any extra bytes that may be needed, such as for when reading	
			// data directly into the output buffer when there are more input		
			// bytes than output bytes because the data will be packed.				
		
	extraBufferBytesNeeded = 0;
	if (gProcessorCode == kRefChangeFileFormatProcessor)
		{
		if (outFileInfoPtr->bandInterleave != kBSQ && 
				reformatOptionsPtr->convertType == 1)
			{
			extraBufferBytesNeeded = MGetPointerSize ((Ptr)gInputBufferPtr);
			if (extraBufferBytesNeeded <= 0)
																						return (FALSE);
			
			}	// end "if (outFileInfoPtr->bandInterleave != kBSQ ..."

		else if (outFileInfoPtr->numberBytes == 1 && 
				reformatOptionsPtr->convertType == k64BitRealTo16BitIntUnsigned)
			{
					// This is case for handling a thematic displayed type of
					// multispectral image window. Extra space is need for the
					// 2-byte integer intermediate type for one line.

			extraBufferBytesNeeded = outFileInfoPtr->numberColumns;	

			}	// end "else if (outFileInfoPtr->numberBytes == 1 && ..."
			
		if (reformatOptionsPtr->transformDataCode == kAdjustChannelsByChannel)
			{
					// Allow space for the data for the selected channel to be used
					// for the adjust of the selected channels.
					
			extraBufferBytesNeeded += (outFileInfoPtr->numberColumns + 1) * 8;
			
			}	// end "...->transformDataCode == kAdjustChannelsByChannel"
	
		}	// end "if (gProcessorCode == kRefChangeFileFormatProcessor)" 
		
	if (gProcessorCode == kRectifyImageProcessor)
		extraBufferBytesNeeded = countOutBytes;
	
	totalNumberBytes += extraBufferBytesNeeded;
	numberBytes = totalNumberBytes;
	
			// Assume for now that all lines will fit into the outut buffer.
			
	numberOutputBufferLines = outFileInfoPtr->numberLines;
                                        
	MGetFreeMemory (&lContBlock);
	
			// Keep 40000 bytes free if possible.
			
	lContBlock -= 40000;
	
			// Limit the number of bytes for the output buffer to be less than
			// 16,000,000 bytes as long as the minimum amount needed (countOutBytes +
			// extraBufferBytesNeeded) is less than that. Cases have been found
			// when using more than 16,000,000 which cause the write to disk to
			// lock up the system or fail with a -36 disk I/O error.
			
	lContBlock = MIN (16000000, lContBlock);
	
	if (totalNumberBytes > lContBlock)
		{
		numberOutputBufferLines = (lContBlock-extraBufferBytesNeeded)/countOutBytes;
		numberBytes = countOutBytes * numberOutputBufferLines + extraBufferBytesNeeded;
														
		}	// end "if (totalNumberBytes > lContBlock)"
		 
	numberBytes = MAX (numberBytes, countOutBytes + extraBufferBytesNeeded);
	
			// Allow for the number of bytes that will be needed to load the	
			// header.																			
			
	lContBlock = GetNumberHeaderBytes (outFileInfoPtr);
	numberBytes = MAX (numberBytes, lContBlock);
	
	reformatOptionsPtr->ioOutBufferPtr = (HUCharPtr)MNewPointer (numberBytes);
	if (reformatOptionsPtr->ioOutBufferPtr != NULL)
		{
		reformatOptionsPtr->countOutBytes = countOutBytes;
		reformatOptionsPtr->numberOutputBufferLines = numberOutputBufferLines;
		reformatOptionsPtr->ioOutBufferBytes = numberBytes - extraBufferBytesNeeded;
		
		if (reformatOptionsPtr->transformDataCode == kAdjustChannelsByChannel)
			{
					// Make multiple of 8 so that adjust buffer starts on 8-byte boundary.
					
			numberBytes -= (outFileInfoPtr->numberColumns + 1) * 8;
			numberBytes = (UInt32)((numberBytes+7)/8) * 8;
			reformatOptionsPtr->ioOutAdjustBufferPtr = 
												&reformatOptionsPtr->ioOutBufferPtr[numberBytes];
				
			}	// end "if (...->transformDataCode == kAdjustChannelsByChannel)"
		
		}	// end "if (reformatOptionsPtr->ioOutBufferPtr != NULL)" 
		
	else		
		continueFlag = FALSE;
		
	return (continueFlag);
				
}	// end "GetReformatOutputBuffer" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetTransformChannelList
//
//	Software purpose:	The purpose of this routine is to determine the
// 						channels that need to be read to transform the
//							data as requested for a new channel.
//
//	Parameters in:		Pointer to reformat instruction structure.
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/14/1992
//	Revised By:			Larry L. Biehl			Date: 03/22/2013

SInt16 GetTransformChannelList (
				ReformatOptionsPtr				reformatOptionsPtr)

{
	SInt16								*channelPtr,
											*transformChannelPtr;
	
	SInt32								countIndex,
											index,
											index2,
											maxNumberOfChannels,
											totalTransformChannels;


	if (!reformatOptionsPtr->transformChannelPtrSetFlag)
		{
		maxNumberOfChannels = gImageWindowInfoPtr->totalNumberChannels;
		channelPtr = reformatOptionsPtr->channelPtr;
		for (index=0; index<maxNumberOfChannels; index++)
			{
			*channelPtr = 0;
			channelPtr++;
			
			}	// end "for (index=0; index<; index++)" 
			
		channelPtr = reformatOptionsPtr->channelPtr;
		transformChannelPtr = reformatOptionsPtr->transformChannelPtr;
		totalTransformChannels = reformatOptionsPtr->numberDenominatorTerms + 
														reformatOptionsPtr->numberNumeratorTerms;
		for (index=0; index<totalTransformChannels; index++)
			{
			if (*transformChannelPtr >= 1 && *transformChannelPtr <= maxNumberOfChannels)
				channelPtr[*transformChannelPtr-1] = 1;
			transformChannelPtr++;
			
			}	// end "for (index=0; index<; index++)" 
			
		countIndex = 0;
		for (index=0; index<maxNumberOfChannels; index++)
			{
			if (channelPtr[index] > 0)
				{
				channelPtr[countIndex] = (SInt16)index;
				countIndex++;
				
				}	// end "if (channelPtr[index] > 0)" 
			
			}	// end "for (index=0; index<; index++)" 
		
		reformatOptionsPtr->numberChannels = (UInt16)countIndex;
		
				// Now reset the transform channel list to conform to the location	
				// of the channels actually read into memory.								
				// Take into account that the channelPtr starts at 0 and					
				// transformChannelPtr starts at 1.												
				
		transformChannelPtr = reformatOptionsPtr->transformChannelPtr;
		for (index=0; index<totalTransformChannels; index++)
			{
			if (*transformChannelPtr > 0)
				{
				for (index2=0; index2<countIndex; index2++)
					{
					if (*transformChannelPtr == channelPtr[index2]+1)
						{
						*transformChannelPtr = (UInt16)index2;
						break;
						
						}	// end "if (*transformChannelPtr == channelPtr[index2])" 
						
					}	// end "for (index2=0; index2<countIndex; index2++)" 
					
				if (index2 >= countIndex)
																								return (0);
																							
				}	// end "if (*transformChannelPtr > 0)" 
				
			transformChannelPtr++;
			
			}	// end "for (index=0; index<; index++)"
			
		reformatOptionsPtr->transformChannelPtrSetFlag = TRUE;
		
		}	// end "if (!reformatOptionsPtr->transformChannelPtrSetFlag)"
		
	return (1);
	
}	// end "GetTransformChannelList" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		InitializeHeaderPopupMenu
//
//	Software purpose:	The purpose of this routine is to initialize the header popup
//							menu so that all menu items are enabled.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/28/2001
//	Revised By:			Larry L. Biehl			Date: 03/18/2014	

void InitializeHeaderPopupMenu (
				DialogPtr							dialogPtr,
				MenuHandle							popUpHeaderOptionsMenu)

{
	#if defined multispec_mac
		if (popUpHeaderOptionsMenu != NULL)
			{
					// Make sure that all header choices are enabled to start with.
					
		   EnableMenuItem (popUpHeaderOptionsMenu, kNoneMenuItem);
			EnableMenuItem (popUpHeaderOptionsMenu, kArcViewMenuItem);
			EnableMenuItem (popUpHeaderOptionsMenu, kERDAS74MenuItem);
			EnableMenuItem (popUpHeaderOptionsMenu, kGAIAMenuItem);
			EnableMenuItem (popUpHeaderOptionsMenu, kMatlabMenuItem);
			EnableMenuItem (popUpHeaderOptionsMenu, kTIFFGeoTIFFMenuItem);
		
			}	// end "if (popUpHeaderOptionsMenu != NULL)"
	#endif	// defined multispec_mac
					
	#if defined multispec_win
		int			comboBoxHeaderItem;

		comboBoxHeaderItem = IDC_ChangeHeader;
		if (popUpHeaderOptionsMenu != NULL)
			comboBoxHeaderItem = (int)popUpHeaderOptionsMenu;

		CComboBox* comboBoxPtr = 
									(CComboBox*)(dialogPtr->GetDlgItem (comboBoxHeaderItem));

		comboBoxPtr->SetItemData (0, kNoneMenuItem);
		comboBoxPtr->SetItemData (1, kArcViewMenuItem);
		comboBoxPtr->SetItemData (2, kERDAS74MenuItem);
		comboBoxPtr->SetItemData (3, kGAIAMenuItem);
		comboBoxPtr->SetItemData (4, kTIFFGeoTIFFMenuItem);
		comboBoxPtr->SetItemData (5, kMatlabMenuItem);
	#endif	// defined multispec_win
		
	#if defined multispec_wx
      UInt16			comboBoxHeaderItem;

		comboBoxHeaderItem = IDC_ChangeHeader;
		if (popUpHeaderOptionsMenu != NULL)
			comboBoxHeaderItem = *(UInt16*)popUpHeaderOptionsMenu;
      
		 wxChoice* comboBoxPtr =
								(wxChoice*)(dialogPtr->FindWindow (comboBoxHeaderItem));

		 comboBoxPtr->SetClientData (0, (void*)(SInt64)gNoneMenuItem);
		 comboBoxPtr->SetClientData (1, (void*)(SInt64)gArcViewMenuItem);
		 comboBoxPtr->SetClientData (2, (void*)(SInt64)gERDAS74MenuItem);
		 comboBoxPtr->SetClientData (3, (void*)(SInt64)gGAIAMenuItem);
		 comboBoxPtr->SetClientData (4, (void*)(SInt64)gTIFFGeoTIFFMenuItem);
		 comboBoxPtr->SetClientData (5, (void*)(SInt64)gMatlabMenuItem);
	#endif	// defined multispec_wx

}	// end "InitializeHeaderPopupMenu" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		InitializeOutputFileInformation
//
//	Software purpose:	The purpose of this routine is to initialize the output file
//							information structure.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			Menus   in MMenus.c
//
//	Coded By:			Larry L. Biehl			Date: 07/12/2005
//	Revised By:			Larry L. Biehl			Date: 03/15/2014	

void InitializeOutputFileInformation (
				WindowInfoPtr						inputWindowInfoPtr,
				FileInfoPtr							inputFileInfoPtr,
				ReformatOptionsPtr				reformatOptionsPtr,
				FileInfoPtr							outFileInfoPtr,
				Boolean								keepSameFormatAsInputFlag)

{
	SInt16								errCode;
	
	
			// Initialize the format information for the output file				

	outFileInfoPtr->numberChannels 		= reformatOptionsPtr->numberChannels;
	outFileInfoPtr->numberClasses  		= inputFileInfoPtr->numberClasses;
	outFileInfoPtr->dataTypeCode 			= inputWindowInfoPtr->dataTypeCode;
	outFileInfoPtr->numberBytes  			= inputWindowInfoPtr->numberBytes;
	outFileInfoPtr->numberBits 			= inputWindowInfoPtr->numberBits;
	outFileInfoPtr->swapBytesFlag 		= inputFileInfoPtr->swapBytesFlag;
	outFileInfoPtr->noDataValueFlag 		= inputFileInfoPtr->noDataValueFlag;
	outFileInfoPtr->noDataValue			= inputFileInfoPtr->noDataValue;

	if (keepSameFormatAsInputFlag)
		{
		outFileInfoPtr->bandInterleave		= inputFileInfoPtr->bandInterleave;
		if (inputFileInfoPtr->gdalDataSetH != NULL)
			outFileInfoPtr->bandInterleave	= inputFileInfoPtr->gdalBandInterleave;
		if (outFileInfoPtr->bandInterleave < kBIL || 
															outFileInfoPtr->bandInterleave > kBIS)
			outFileInfoPtr->bandInterleave	= kBSQ;
		outFileInfoPtr->numberHeaderBytes 	= inputFileInfoPtr->numberHeaderBytes;
		outFileInfoPtr->format 					= inputFileInfoPtr->format;
		outFileInfoPtr->ancillaryInfoformat = inputFileInfoPtr->ancillaryInfoformat;
		
		}	// end "if (keepSameFormatAsInputFlag)"
		
	else	// !keepSameFormatAsInputFlag
		{
		outFileInfoPtr->bandInterleave		= kBIL;
		outFileInfoPtr->numberHeaderBytes 	= 128;
		outFileInfoPtr->format 					= kErdas74Type;
		outFileInfoPtr->ancillaryInfoformat = kErdas74Type; 
		if (reformatOptionsPtr->headerFormat == kTIFFType)
			{
			outFileInfoPtr->bandInterleave		= kBSQ;
			outFileInfoPtr->numberHeaderBytes 	= 0; 
			outFileInfoPtr->format 					= kTIFFType;
			outFileInfoPtr->ancillaryInfoformat = kTIFFType;

			}	// end "if (reformatOptionsPtr->headerFormat == kTIFFType)"
			
		}	// end "else !keepSameFormatAsInputFlag"
	
			// Note that the instrument code will also be verified later in the
			// UpdateOutputFileStructure routine after the user has selected the
			// desired items in the dialog box.
			
	outFileInfoPtr->instrumentCode = inputFileInfoPtr->instrumentCode;

	outFileInfoPtr->numberTrailerBytes 	= 0;
	outFileInfoPtr->numberPreLineBytes 	= 0;
	outFileInfoPtr->numberPostLineBytes = 0;
	outFileInfoPtr->numberPreChannelBytes 	= 0;
	outFileInfoPtr->numberPostChannelBytes = 0;
	
			// Get a copy of any control points.
	
	if (inputFileInfoPtr->controlPointsHandle != NULL)
		{
		outFileInfoPtr->controlPointsHandle = inputFileInfoPtr->controlPointsHandle;
		errCode = HandToHand (&outFileInfoPtr->controlPointsHandle);
		if (errCode != noErr)
			outFileInfoPtr->controlPointsHandle = NULL;
			
		}	// end "if (inputFileInfoPtr->controlPointsHandle != NULL)"
	
	outFileInfoPtr->thematicType = inputFileInfoPtr->thematicType;
	if (inputWindowInfoPtr->imageType == kMultispectralImageType &&
								inputWindowInfoPtr->windowType == kThematicWindowType)
		{
		outFileInfoPtr->thematicType 	= TRUE;
		outFileInfoPtr->numberChannels = 1;
		
		outFileInfoPtr->numberBytes = 1;
		outFileInfoPtr->numberBits = 8;
		outFileInfoPtr->dataTypeCode = kIntegerType;
		outFileInfoPtr->swapBytesFlag = FALSE;
		
		}	// end "if (...->imageType == kMultispectralImageType && ..."
	
	if (outFileInfoPtr->thematicType)
		{
		outFileInfoPtr->descriptionsFlag = inputFileInfoPtr->descriptionsFlag;
		outFileInfoPtr->classDescriptionH = inputFileInfoPtr->classDescriptionH;
		outFileInfoPtr->asciiSymbols = inputFileInfoPtr->asciiSymbols;
		
		}	// end "if (outFileInfoPtr->thematicType)"
		
}	// end "InitializeOutputFileInformation" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void InitializeReformatStructure
//
//	Software purpose:	The purpose of this routine is to the reformat
//							specification structure.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			GetReformatAndFileInfoStructures
//
//	Coded By:			Larry L. Biehl			Date: 12/06/1991
//	Revised By:			Larry L. Biehl			Date: 05/28/2020

void InitializeReformatStructure (
				ReformatOptionsPtr				reformatOptionsPtr)

{  
	if (reformatOptionsPtr != NULL)
		{													                   
		reformatOptionsPtr->pcScaleFactor = 1.0;   
		reformatOptionsPtr->modifyAppendBytesSkip = 0;
		reformatOptionsPtr->channelHandle = NULL;
		reformatOptionsPtr->coefficientsHandle = NULL;
		reformatOptionsPtr->transformChannelHandle = NULL;
		reformatOptionsPtr->transformOperatorHandle = NULL;
		reformatOptionsPtr->eigenValueHandle = NULL;
		reformatOptionsPtr->eigenVectorHandle = NULL;
		reformatOptionsPtr->inputWindowInfoHandle = NULL;
		reformatOptionsPtr->outFileInfoHandle = NULL;
		reformatOptionsPtr->pcChannelHandle = NULL;
		reformatOptionsPtr->rightBottomMosaicWindowInfoHandle = NULL;
		reformatOptionsPtr->rectifyImageOptionsPtr = NULL;
		reformatOptionsPtr->ioOutAdjustBufferPtr = NULL;
		reformatOptionsPtr->ioOutBufferPtr = NULL;
		reformatOptionsPtr->fileInfoListPtr = NULL;
		reformatOptionsPtr->coefficientsPtr = NULL;
		reformatOptionsPtr->channelPtr = NULL;
		reformatOptionsPtr->eigenValuePtr = NULL;
		reformatOptionsPtr->eigenVectorPtr = NULL;
		reformatOptionsPtr->pcChannelPtr = NULL;
		reformatOptionsPtr->transformChannelPtr = NULL;
		reformatOptionsPtr->transformOperatorPtr = NULL;
		reformatOptionsPtr->classPtr = NULL;
		reformatOptionsPtr->countOutBytes = 0;
		reformatOptionsPtr->highSaturationCount = 0;
		reformatOptionsPtr->ioOutBufferBytes = 0;
		reformatOptionsPtr->numberOutputBufferLines = 0;
		reformatOptionsPtr->lowSaturationCount = 0;
		reformatOptionsPtr->minimumValue = 0.;
		reformatOptionsPtr->maximumValue = 0.;
		reformatOptionsPtr->lineStart = 1;
		reformatOptionsPtr->lineEnd = 1;
		reformatOptionsPtr->lineInterval = 1;
		reformatOptionsPtr->columnStart = 1;
		reformatOptionsPtr->columnEnd = 1;
		reformatOptionsPtr->columnInterval = 1;
		reformatOptionsPtr->functionCode = kFunctionMin;
		
				// Transform (dialog) parameters
				//		These will now be set in the transform dialog routine.
				
		//reformatOptionsPtr->transformFactor = 1.;
		//reformatOptionsPtr->transformOffset = 0.;
		//GetDefaultBandRatio (
		//					gImageWindowInfoPtr, gImageFileInfoPtr, reformatOptionsPtr);
		
		reformatOptionsPtr->transformDataCode = kNoTransform;
		reformatOptionsPtr->adjustDivisor = 1;
		reformatOptionsPtr->adjustFactor = 1;
		reformatOptionsPtr->adjustOffset = 0;
		reformatOptionsPtr->algebraicTransformK1Value = 1;
		reformatOptionsPtr->algebraicTransformK2Value = 1;
		reformatOptionsPtr->mosaicDirectionCode = 1;
		reformatOptionsPtr->numberDenominatorTerms = 0;
		reformatOptionsPtr->numberNumeratorTerms = 0;
		reformatOptionsPtr->numberPCChannels = 0;
		reformatOptionsPtr->numberPCComponents = 0;		
		reformatOptionsPtr->functionFactor = 1.;
		reformatOptionsPtr->kthSmallestValue = 1;
		reformatOptionsPtr->transformAdjustSelectedChannelsFactor = -1.;
		reformatOptionsPtr->transformAdjustSelectedChannel = 1;
		
				// Set radiant temperature conversion constants based on instrument
				
		//SetRadiantTemperatureConstants (gImageFileInfoPtr, reformatOptionsPtr);
		
		if (gImageWindowInfoPtr != NULL)
			{
			reformatOptionsPtr->lineEnd = gImageWindowInfoPtr->maxNumberLines;
			reformatOptionsPtr->columnEnd = gImageWindowInfoPtr->maxNumberColumns;
			
			}	// end "gImageWindowInfoPtr != NULL" 
			
		reformatOptionsPtr->headerFormat = kTIFFType;
		reformatOptionsPtr->bandInterleaveSelection = kBSQ;
		if (gProcessorCode == kRefChangeFileFormatProcessor ||
						gProcessorCode == kRectifyImageProcessor ||
									gProcessorCode == kRefMosaicImagesProcessor)
			{
			/*
			if (//!gImageFileInfoPtr->thematicType &&
							(gImageFileInfoPtr->format == kGeoTIFFType ||
								gImageFileInfoPtr->format == kTIFFType ||
										FindIfMapInformationExists (gImageWindowInfoPtr)))
				{
				reformatOptionsPtr->headerFormat = kTIFFType;
				reformatOptionsPtr->bandInterleaveSelection = kBSQ;
				
				}	// end "if (gImageFileInfoPtr->format == kGeoTIFFType || ..."
			*/
			if (gImageFileInfoPtr->format == kArcViewType)
				{
				reformatOptionsPtr->headerFormat = kArcViewType;
				reformatOptionsPtr->bandInterleaveSelection = 
														gImageFileInfoPtr->bandInterleave;
				
				}	// end "else if (gImageFileInfoPtr->format == kArcViewType)"
				
			else if (reformatOptionsPtr->headerFormat == kErdas74Type &&
													gImageFileInfoPtr->numberBytes <= 2)
				{
				reformatOptionsPtr->headerFormat = kErdas74Type;
				reformatOptionsPtr->bandInterleaveSelection = kBIL;
				
				}	// end "else if (...->headerFormat == kErdas74Type && ..."
			
			}	// end "if (gProcessorCode == kRefChangeFileFormatProcessor || ..."
		
		reformatOptionsPtr->outputFileCode = kNewFileMenuItem;
		
		if (gImageFileInfoPtr != NULL && gImageWindowInfoPtr != NULL)
			{
			reformatOptionsPtr->lastInputFileDataTypeCode = 
															gImageFileInfoPtr->dataTypeCode;
			reformatOptionsPtr->lastInputFileSignedDataFlag = 
															gImageFileInfoPtr->signedDataFlag;
			reformatOptionsPtr->lastInputWindowNumberBytes = 
															gImageWindowInfoPtr->numberBytes;
			reformatOptionsPtr->lastInputWindowNumberChannels = 
															gImageWindowInfoPtr->totalNumberChannels;
			
			}	// end "if (gImageFileInfoPtr != NULL && gImageWindowInfoPtr != NULL)"
			
		reformatOptionsPtr->lastInputImageType = kNoImageType;
		reformatOptionsPtr->lastInputWindowType = -1;
		reformatOptionsPtr->lastReformatProcessorCode = gProcessorCode;
		
		reformatOptionsPtr->startLine = 1;
		reformatOptionsPtr->stopLine = 1;
		reformatOptionsPtr->startColumn = 1;
		reformatOptionsPtr->stopColumn = 1;
		reformatOptionsPtr->convertType = 0;
		reformatOptionsPtr->eigenSource = 0;
		reformatOptionsPtr->backgroundValue = 0;
		reformatOptionsPtr->channelSet = kAllMenuItem;
		reformatOptionsPtr->classSet = kAllMenuItem;
		reformatOptionsPtr->dataValueTypeSelection = 0;
		reformatOptionsPtr->fieldType = kTrainingType + kTestingType;
		reformatOptionsPtr->numberChannels = 0;
		reformatOptionsPtr->numberClasses = 0;
		reformatOptionsPtr->bottomToTop = FALSE;
		reformatOptionsPtr->channelDescriptions = FALSE;
		reformatOptionsPtr->checkForSaturationFlag = FALSE;
		reformatOptionsPtr->forceByteCode = kDoNotForceBytes;
		reformatOptionsPtr->ignoreBackgroundFlag = FALSE;
		
				// The assumption is that the output will be in wavelength order unless
				// it is know for sure that the data are not in wavelength order.
				// The 'Change File Format' processor may force a change in the output.
			
		reformatOptionsPtr->outputInWavelengthOrderFlag = TRUE;
		//if (gProcessorCode == kRefChangeFileFormatProcessor ||
		//				gProcessorCode == kRectifyImageProcessor ||
		//							gProcessorCode == kRefMosaicImagesProcessor)
		//	{
		if (gImageWindowInfoPtr != NULL &&
					gImageWindowInfoPtr->channelsInWavelengthOrderCode == kNotInOrder)
			{
			reformatOptionsPtr->outputInWavelengthOrderFlag = FALSE;
		
			}	// end "if (gImageWindowInfoPtr->channelsInWavelengthOrderCode != ..."
		
		//	}	// end "if (gProcessorCode == kRefChangeFileFormatProcessor || ..."

		reformatOptionsPtr->rightToLeft = FALSE;
		reformatOptionsPtr->signedOutputDataFlag = FALSE;
		reformatOptionsPtr->swapBytesFlag = FALSE;
		reformatOptionsPtr->transformChannelPtrSetFlag = FALSE;
		reformatOptionsPtr->userSetDataValueTypeSelectionFlag = FALSE;
		
				// Load user selected rectangle information into reformat 			
				// specification information if the selection rectangle has been 	
				// set up.																			

		GetSelectedAreaInfo (gActiveImageWindow,
									gImageWindowInfoPtr,
									&reformatOptionsPtr->lineStart,
									&reformatOptionsPtr->lineEnd,
									&reformatOptionsPtr->columnStart,
									&reformatOptionsPtr->columnEnd,
									kDontClearSelectionArea,
									kUseThreshold,
									kDontAdjustToBaseImage);
		
		}	// end "reformatOptionsPtr != NULL" 
		
}	// end "InitializeReformatStructure"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean LoadReformatOptionsSpecs
//
//	Software purpose:	The purpose of this routine is to load the Reformat Options 
//							Specification structure with the default set of parameters.
//
//	Parameters in:		.
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/31/2013
//	Revised By:			Larry L. Biehl			Date: 04/16/2020

Boolean LoadReformatOptionsSpecs (
				WindowInfoPtr						windowInfoPtr)
			
{
	ReformatOptionsPtr				reformatOptionsPtr;
	
	Handle								windowInfoHandle;
	
	SInt32								index,
											numberBytes;
		
	Boolean								changedFlag,
											continueFlag,
											releaseReformatMemoryFlag,
											returnFlag;
	
	
			// Assume routine will work okay.  If there isn't enough memory		
			// then the flag will be changed.												
			
	continueFlag = returnFlag = TRUE;
	releaseReformatMemoryFlag = FALSE;
	
	windowInfoHandle = GetWindowInfoHandle (windowInfoPtr);
	
			// If a reformat options structure already exists, then check if it is		
			// for the current window.  If not release the old structure and		
			// initialize a new one.															
			
	if (gNonProjProcessorSpecs.reformatOptionsSpecsH != NULL)
		{
				// Lock the handle to the reformat options specifications and get			
				// a pointer to the structure.												
		
		reformatOptionsPtr = (ReformatOptionsPtr)GetHandlePointer (
								gNonProjProcessorSpecs.reformatOptionsSpecsH, kLock, kMoveHi);
	
		reformatOptionsPtr->firstTimeFlag = FALSE;
		
		if (reformatOptionsPtr->lastReformatProcessorCode != gProcessorCode ||
						gProcessorCode != kRefChangeFileFormatProcessor)
			releaseReformatMemoryFlag = TRUE; 
		
		if (!releaseReformatMemoryFlag &&
				(gImageFileInfoPtr->dataTypeCode !=
												reformatOptionsPtr->lastInputFileDataTypeCode ||
					gImageFileInfoPtr->signedDataFlag !=
												reformatOptionsPtr->lastInputFileSignedDataFlag ||
						reformatOptionsPtr->lastInputWindowNumberBytes !=
																		windowInfoPtr->numberBytes ||
							GetImageType (windowInfoHandle) !=
														reformatOptionsPtr->lastInputImageType ||
								GetWindowType (windowInfoHandle) !=
															reformatOptionsPtr->lastInputWindowType))
			releaseReformatMemoryFlag = TRUE; 
			
				// If the input file contains fewer lines or columns than what was used
				// by the last reformat task, then clear the reformat options structure 
				// and start again.
				//		Decided not to do this check for now to see how it works.
		/*		
		if (!releaseReformatMemoryFlag && 
					(reformatOptionsPtr->lineEnd > (SInt32)windowInfoPtr->maxNumberLines ||
							reformatOptionsPtr->columnEnd > 
															(SInt32)windowInfoPtr->maxNumberColumns))
			releaseReformatMemoryFlag = TRUE; 
		*/	
				// If there was previous transformation and the number of channels in this input image
				// differs from the that in the last image used, then clear the
				// reformat options structure and start again.
				// Decided that needed to clear when the number of channels differed period.
		/*
		if (!releaseReformatMemoryFlag && 
				((reformatOptionsPtr->transformDataCode == kCreatePCImage ||
						reformatOptionsPtr->transformDataCode == kTransformChannels) &&
								reformatOptionsPtr->lastInputWindowNumberChannels != 
																windowInfoPtr->totalNumberChannels))
		*/
		if (windowInfoPtr != NULL &&
				reformatOptionsPtr->lastInputWindowNumberChannels != windowInfoPtr->totalNumberChannels)
			releaseReformatMemoryFlag = TRUE; 
			
		if (releaseReformatMemoryFlag)
			{
			ReleaseReformatSpecsMemory (&gNonProjProcessorSpecs.reformatOptionsSpecsH);
			reformatOptionsPtr = NULL;
			
			}	// end "if (releaseReformatMemoryFlag)"
			
		else	// !releaseReformatMemoryFlag
			{
					// Load user selected rectangle information into reformat 			
					// specification information if the selection rectangle has been 	
					// set up. Want to make sure it matches any new image being used.																			

			if (!GetSelectedAreaInfo (gActiveImageWindow,
												windowInfoPtr,
												&reformatOptionsPtr->lineStart,
												&reformatOptionsPtr->lineEnd,
												&reformatOptionsPtr->columnStart,
												&reformatOptionsPtr->columnEnd,
												kDontClearSelectionArea,
												kUseThreshold,
												kDontAdjustToBaseImage))
				{
						// No user selection made. Make sure that the default reformat
						// default area includes the entire image.
					
				reformatOptionsPtr->lineStart = 1;
				reformatOptionsPtr->lineEnd = windowInfoPtr->maxNumberLines;
				reformatOptionsPtr->columnStart = 1;
				reformatOptionsPtr->columnEnd = windowInfoPtr->maxNumberColumns;
				
				}	// end "if (!GetSelectedAreaInfo (..."
				
					// Make sure counts, etc are initialized for this run					
			
			reformatOptionsPtr->highSaturationCount = 0;
			reformatOptionsPtr->lowSaturationCount = 0;
				
			}	// end "else !releaseReformatMemoryFlag"
				
		}	// end "else gNonProjProcessorSpecs.reformatOptionsSpecsH != NULL" 
	
	if (gNonProjProcessorSpecs.reformatOptionsSpecsH == NULL)
		{
				// Set up handle to project file information								
			
		gNonProjProcessorSpecs.reformatOptionsSpecsH = 
															MNewHandle (sizeof (ReformatOptions));
		if (gNonProjProcessorSpecs.reformatOptionsSpecsH != NULL)
			{
			reformatOptionsPtr = (ReformatOptionsPtr)GetHandlePointer (
								gNonProjProcessorSpecs.reformatOptionsSpecsH, kLock, kMoveHi);
								
			InitializeReformatStructure (reformatOptionsPtr);
				
			reformatOptionsPtr->firstTimeFlag = TRUE;
			reformatOptionsPtr->inputWindowInfoHandle = windowInfoHandle;
			reformatOptionsPtr->lastInputImageType = GetImageType (windowInfoHandle);
			reformatOptionsPtr->lastInputWindowType = GetWindowType (windowInfoHandle);
			reformatOptionsPtr->fileInfoVersion = GetFileInfoVersion (windowInfoPtr);
			
			}	// end "if (gNonProjProcessorSpecs.reformatOptionsSpecsH != NULL)" 
		
		else	// else gNonProjProcessorSpecs.reformatOptionsSpecsH == NULL 
			returnFlag = FALSE;
			
		}	// end "if (gNonProjProcessorSpecs.reformatOptionsSpecsH == NULL)"  
		
	if (returnFlag)
		{
		if (gProcessorCode == kRefChangeFileFormatProcessor)
			{
					// Set up memory for reformatting channels vector.
					// Space is also being allow for a wavelength order vector in
					// case it is needed.
			
			if (continueFlag)
				{
				numberBytes = 2 *
						(SInt32)gImageWindowInfoPtr->totalNumberChannels * sizeof (SInt16);
				reformatOptionsPtr->channelPtr = (SInt16*)CheckHandleSize (
																&reformatOptionsPtr->channelHandle,
																&continueFlag,
																&changedFlag,
																numberBytes);
														
						// Fill in vector of all channels.											
				
				if (continueFlag && (reformatOptionsPtr->firstTimeFlag || changedFlag))
					{
					reformatOptionsPtr->numberChannels = 
														gImageWindowInfoPtr->totalNumberChannels;
					for (index=0; index<reformatOptionsPtr->numberChannels; index++)
						reformatOptionsPtr->channelPtr[index] = (SInt16)index;
						
					}	// end "if (reformatOptionsPtr->firstTimeFlag || changedFlag)"
				
				}	// end "if (continueFlag)" 
							
					// Set up memory for transform coefficients vector.						
			
			if (continueFlag)
				{
				numberBytes = (SInt32)(gImageWindowInfoPtr->totalNumberChannels + 1) * 
																					2 * sizeof (double);
				reformatOptionsPtr->coefficientsPtr = (double*)CheckHandleSize (
														&reformatOptionsPtr->coefficientsHandle,
														&continueFlag,
														&changedFlag,
														numberBytes);
				
				}	// end "if (continueFlag)" 
							
					// Set up memory for reformatting PC image channels vector.				
			
			if (continueFlag)
				{
				numberBytes =
					(SInt32)gImageWindowInfoPtr->totalNumberChannels * sizeof (SInt16);
				reformatOptionsPtr->pcChannelPtr = (SInt16*)CheckHandleSize (
															&reformatOptionsPtr->pcChannelHandle,
															&continueFlag,
															&changedFlag,
															numberBytes);
			
						// Fill in vector of all principal component channels.										
				
				if (continueFlag && (reformatOptionsPtr->firstTimeFlag || changedFlag))
					{
					reformatOptionsPtr->numberPCChannels = 
															gImageWindowInfoPtr->totalNumberChannels;
					for (index=0; index<reformatOptionsPtr->numberPCChannels; index++)
						reformatOptionsPtr->pcChannelPtr[index] = (SInt16)index;
						
					}	// end "if (reformatOptionsPtr->firstTimeFlag || changedFlag)"
				
				}	// end "if (continueFlag)"
							
					// Set up memory for transform channels vector.								
			
			if (continueFlag)
				{
				numberBytes = (SInt32)(gImageWindowInfoPtr->totalNumberChannels + 1) * 
																					2 * sizeof (SInt16);
				reformatOptionsPtr->transformChannelPtr = (SInt16*)CheckHandleSize (
														&reformatOptionsPtr->transformChannelHandle,
														&continueFlag,
														&changedFlag,
														numberBytes);
				
				}	// end "if (continueFlag)" 
							
					// Set up memory for transform operator vector.								
			
			if (continueFlag)
				{
				numberBytes = (SInt32)(gImageWindowInfoPtr->totalNumberChannels + 1) * 
																					2 * sizeof (SInt16);
				reformatOptionsPtr->transformOperatorPtr = (SInt16*)CheckHandleSize (
													&reformatOptionsPtr->transformOperatorHandle,
													&continueFlag,
													&changedFlag,
													numberBytes);
				
				}	// end "if (continueFlag)" 
				
			returnFlag = continueFlag;
			
					// Indicate that no default (last) algebraic transformations have
					// been loaded.
			
			reformatOptionsPtr->algebraicStringsLoadedFlag = FALSE;
				
			}	// end "if (gProcessorCode == kRefChangeFileFormatProcessor)"
			
		}	// end "if (returnFlag)" 
	
			// If unable to set up information for reformat options							
			// specifications, release memory allocated to it (if any).				
			
	if (!returnFlag) 
		{
		ReleaseReformatSpecsMemory (&gNonProjProcessorSpecs.reformatOptionsSpecsH);
		reformatOptionsPtr = NULL;
		
		}	// end "if (!returnFlag)" 
		
	return (returnFlag);

}	// end "LoadReformatOptionsSpecs"
		                                        


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		ReformatControl
//
//	Software purpose:	The purpose of this routine is to control the 
//							reformating utilities
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			Menus   in MMenus.c
//
//	Coded By:			Larry L. Biehl			Date: 08/28/1988
//	Revised By:			Larry L. Biehl			Date: 02/07/2018

void ReformatControl (
				SInt16								reformatRequest)

{
	Handle								windowInfoHandle;
	
	SInt16								activeImageGlobalHandleStatus = 0;
	
	
			// If spare memory had to be used to load code resources, then exit	
			// routine.																				
			
	if (gMemoryTypeNeeded < 0)
																									return;
																							
			// Code resources loaded okay, so set flag back for non-Code			
			// resources.																			
			
	gMemoryTypeNeeded = 0;
	
	if (reformatRequest != kReformatConvertENVIASCIIRequest)
		{
				// Lock the information handles for the active image window and		
				// get pointers to the information.	
				// Active image information handles are not needed for
				// converting an ENVI text ROI file to an image.											
		
		if (!SetUpActiveImageInformationGlobals (&windowInfoHandle, 
					&activeImageGlobalHandleStatus, kDoNotUseProject) &&  
																	gProjectInfoPtr == NULL)
																									return;
																							
		}	// end "if (reformatRequest != kReformatConvertENVIASCIIRequest)"
	
	if (reformatRequest > 0)
		{
				// Force text selection to start from end of present text.			
					
		ForceTextToEnd ();
		
		switch (reformatRequest)
			{
			case kReformatChangeHeaderRequest:
				gProcessorCode = kRefChangeHeaderProcessor;
				ChangeErdasHeader ();
				break;
			
			case kReformatChangeImageRequest:
				gProcessorCode = kRefChangeFileFormatProcessor;
				ChangeImageFileFormat ();
				break;
				
			case kReformatConvertMultispectralRequest:
		      gMemoryTypeNeeded = 1;
				gProcessorCode = kMultispecToThematicProcessor;
				ForceDisplayCodeResourceLoad ();
								
				if (gMemoryTypeNeeded == 0)
					{
		      	gMemoryTypeNeeded = 1;
					CovertMultispectralToThematicControl (gImageWindowInfoPtr);
					
					}	// end "if (gMemoryTypeNeeded == 0)"
				break;
				
			case kReformatConvertProjectRequest:
		      gMemoryTypeNeeded = 1;
				gProcessorCode = kRefFieldsToThematicFileProcessor;
				AreasToThematicFileControl ();            
				break;
				
			case kReformatConvertShapeRequest:
		      gMemoryTypeNeeded = 1;
				gProcessorCode = kRefShapeToThematicFileProcessor;
				ShapeToThematicFileControl ();            
				break;
				
			case kReformatModifyChannelRequest:
				gProcessorCode = kRefModifyChanDescriptionsProcessor;
				ModifyChannelDescriptions (1);
				break;
			
			case kReformatMosaicImagesRequest:
		      gMemoryTypeNeeded = 1;
				gProcessorCode = kRefMosaicImagesProcessor;
				MosaicTwoImagesControl ();
				break;
				
			case kReformatRecodeThematicRequest:
		      gMemoryTypeNeeded = 1;
				gProcessorCode = kRecodeThematicImageProcessor;
				RecodeThematicImageControl ();
				break;
				
			case kReformatRectifyImageRequest:
		      gMemoryTypeNeeded = 1;
				gProcessorCode = kRectifyImageProcessor;
				RectifyImageControl ();             
				break;
				
			case kReformatConvertENVIASCIIRequest:
				gProcessorCode = kENVIROItoThematicProcessor;
				ENVI_ROIToThematicFileControl ();
				break;
				
			}	// end "switch (reformatRequest)" 
			
				// Scroll output window to the selection and adjust the 			
				// scroll bar.																	
		
		UpdateOutputWScrolls (gOutputWindow, 1, kDisplayMessage);
			
		}	// end "if (reformatRequest > 0)" 
		
	gMemoryTypeNeeded = 0;					
	gProcessorCode = 0;
				
			// Unlock the information handles for the active window.		
	
	UnlockActiveImageInformationGlobals (activeImageGlobalHandleStatus);
		
}	// end "ReformatControl"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				void ReleaseReformatOutputFileInfoAndBuffers
//
//	Software purpose:			The purpose of this routine is to close the file
//									and release the memory held by the output file information 
//									structure. If the input file information pointer is given 
//									make sure that the channel description handle or class 
//									description handle is not disposed of if it is that same 
//									as that for the input file information structure.
//									Also release memory allocated to the reformat buffer.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/31/2013
//	Revised By:			Larry L. Biehl			Date: 04/11/2013

void ReleaseReformatOutputFileInfoAndBuffers (
				ReformatOptionsPtr				reformatOptionsPtr,
				FileInfoPtr							inputFileInfoPtr)

{			
	FileInfoPtr							outputFileInfoPtr;
	
	
	if (inputFileInfoPtr != NULL)
		{										
		outputFileInfoPtr = (FileInfoPtr)GetHandlePointer (
															reformatOptionsPtr->outFileInfoHandle);
		
		if (outputFileInfoPtr != NULL)
			{
			if (outputFileInfoPtr->channelDescriptionH == 
															inputFileInfoPtr->channelDescriptionH)
				outputFileInfoPtr->channelDescriptionH = NULL;
			
			if (outputFileInfoPtr->classDescriptionH == 
															inputFileInfoPtr->classDescriptionH)
				outputFileInfoPtr->classDescriptionH = NULL;	
		
			}	// end "if (outputFileInfoPtr != NULL)"
		
		}	// end "if (inputFileInfoPtr != NULL)"
		
			// Release memory that was allocated for the output file 			
			// information.	
	 
	DisposeFileInfoHandle (reformatOptionsPtr->outFileInfoHandle);
	reformatOptionsPtr->outFileInfoHandle = NULL;
	
			// Release memory for the reformat output IO buffer.																		
			
	reformatOptionsPtr->ioOutBufferPtr = (HUCharPtr)CheckAndDisposePtr (
														(Ptr)reformatOptionsPtr->ioOutBufferPtr);
	reformatOptionsPtr->ioOutAdjustBufferPtr = NULL;
	
}	// end "ReleaseReformatOutputFileInfoAndBuffers" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				void SaveAlgebraicTransformationFunction
//
//	Software purpose:			This routine saves the algebraic transformation function
//									for possible reuse later during the session
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/20/2012
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

void SaveAlgebraicTransformationFunction (
				UInt32								numberChannels,
				SInt16								instrumentCode,
				ReformatOptionsPtr				reformatOptionsPtr)

{				
	CopyPToP (gAlgebraicTransformationFunction.denominatorString,
													reformatOptionsPtr->denominatorString);
	CopyPToP (gAlgebraicTransformationFunction.numeratorString,
													reformatOptionsPtr->numeratorString);
	gAlgebraicTransformationFunction.transformFactor = 
															reformatOptionsPtr->transformFactor;
	gAlgebraicTransformationFunction.transformOffset = 
															reformatOptionsPtr->transformOffset;
	gAlgebraicTransformationFunction.instrumentCode = instrumentCode;
	gAlgebraicTransformationFunction.numberChannels = (SInt16)numberChannels;
		
}	// end "SaveAlgebraicTransformationFunction"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetRadiantTemperatureConstants
//
//	Software purpose:	This routine sets the radiant temperature constants for later
//							use if requested in reformat transformations.
//
//	Parameters in:
//
//	Parameters out:
//
// Value Returned:
//
// Called By:			ChangeFormatToBILorBISorBSQ
//
//	Coded By:			Larry L. Biehl			Date: 05/06/2020
//	Revised By:			Larry L. Biehl			Date: 05/06/2020

void SetRadiantTemperatureConstants (
				FileInfoPtr							imageFileInfoPtr,
				ReformatOptionsPtr				reformatOptionsPtr)

{
	switch (imageFileInfoPtr->instrumentCode)
		{
		case kLandsatTM:
			reformatOptionsPtr->defaultThermalChannel = 7;
			reformatOptionsPtr->algebraicTransformK1Value = 607.76;
			reformatOptionsPtr->algebraicTransformK2Value = 1260.56;
			reformatOptionsPtr->algebraicTransformRadianceMult = .055376;
			reformatOptionsPtr->algebraicTransformRadianceOffset = 1.18;
			break;
				
		case kLandsatTM7:
			reformatOptionsPtr->defaultThermalChannel = 7;
			reformatOptionsPtr->algebraicTransformK1Value = 666.09;
			reformatOptionsPtr->algebraicTransformK2Value = 1282.71;
			reformatOptionsPtr->algebraicTransformRadianceMult = 6.7087E-02;
			reformatOptionsPtr->algebraicTransformRadianceOffset = -0.06709;
			break;
				
		case kASTER:
			reformatOptionsPtr->defaultThermalChannel = 1;
			reformatOptionsPtr->algebraicTransformK1Value = 641.32;
			reformatOptionsPtr->algebraicTransformK2Value = 1271.22;
			reformatOptionsPtr->algebraicTransformRadianceMult = 0.005225;
			reformatOptionsPtr->algebraicTransformRadianceOffset = 0;
			break;
			
		case kLandsatLC8_OLI_TIRS:
			reformatOptionsPtr->defaultThermalChannel = 9;
			reformatOptionsPtr->algebraicTransformK1Value = 774.89;
			reformatOptionsPtr->algebraicTransformK2Value = 1321.08;
			reformatOptionsPtr->algebraicTransformRadianceMult = 0.0003342;
			reformatOptionsPtr->algebraicTransformRadianceOffset = 0.1;
			break;
		
		case kLandsatLC8_TIRS:
			reformatOptionsPtr->defaultThermalChannel = 1;
			reformatOptionsPtr->algebraicTransformK1Value = 774.89;
			reformatOptionsPtr->algebraicTransformK2Value = 1321.08;
			reformatOptionsPtr->algebraicTransformRadianceMult = 0.0003342;
			reformatOptionsPtr->algebraicTransformRadianceOffset = 0.1;
			break;
			
		default:
			reformatOptionsPtr->defaultThermalChannel = 1;
			reformatOptionsPtr->algebraicTransformK1Value = 1;
			reformatOptionsPtr->algebraicTransformK2Value = 1;
			reformatOptionsPtr->algebraicTransformRadianceMult = 1;
			reformatOptionsPtr->algebraicTransformRadianceOffset = 0;
			
		}	// end "switch (imageFileInfoPtr->instrumentCode)"
	
}	// end "SetRadiantTemperatureConstants"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void TransformAdjustChannelsByChannel
//
//	Software purpose:	This routine adjusts the data for the change image
//							format reformat processor.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:			ChangeFormatToBILorBISorBSQ
//
//	Coded By:			Larry L. Biehl			Date: 04/10/2013
//	Revised By:			Larry L. Biehl			Date: 04/11/2013

void TransformAdjustChannelsByChannel (
				ReformatOptionsPtr				reformatOptionsPtr,
				HDoublePtr							ioInDoubleBufferPtr,
				HDoublePtr							adjustChannelPtr,
				double								factor,
				SInt16								numberChannels,
				UInt32								numberColumns,
				Boolean								inputBISFlag)

{
			// Declare local variables & structures	
			
	HDoublePtr							savedAdjustChannelPtr,
											savedioInDoubleBufferPtr;
	
	UInt32								bufferInterval,
											channel,
											columnIndex;
	
	
	savedAdjustChannelPtr = adjustChannelPtr;
	savedioInDoubleBufferPtr = ioInDoubleBufferPtr;
	
			// Multiply factor times adjust data so that it is not repeated for
			// each channel.
			
	for (columnIndex=0; columnIndex<numberColumns; columnIndex++)
		{
		*adjustChannelPtr *= factor;
		adjustChannelPtr++;
		
		}	// end "for (columnIndex=0; columnIndex<numberColumns; columnIndex++)"
	
	bufferInterval = 1;
	if (inputBISFlag)
		bufferInterval = numberChannels;
	
	for (channel=0; channel<(UInt16)numberChannels; channel++)
		{
		adjustChannelPtr = savedAdjustChannelPtr;
		
		if (inputBISFlag)
			ioInDoubleBufferPtr = savedioInDoubleBufferPtr + channel;
		
		for (columnIndex=0; columnIndex<numberColumns; columnIndex++)
			{
			*ioInDoubleBufferPtr += *adjustChannelPtr;
			
			ioInDoubleBufferPtr += bufferInterval;
			adjustChannelPtr++;
			
			}	// end "for (columnIndex=0; columnIndex<numberColumns; columnIndex++)"
			
		}	// end "for (channel=0; channel<numberChannels; channel++)" 
		
}	// end "TransformAdjustChannelsByChannel" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				void TransformData
//
//	Software purpose:			This routine transforms the input channels of data
//									to one channel of output data.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/05/1992
//	Revised By:			Larry L. Biehl			Date: 05/05/2020

void TransformData (
				HUInt8Ptr							ioBufferPtr, 
				ReformatOptionsPtr				reformatOptionsPtr, 
				UInt32								numberSamples,
				double								highSaturatedValue, 
				Boolean								inputBISFlag)

{								
	double								channelValue,
											denominator,
											k1Value,
											k2Value,
											numerator,
											radiantTemperature,
											transformFactor,
											transformOffset;
									
	double*								coefficientsPtr;
	HDoublePtr 							ioDoubleBufferPtr;
	
	SInt16								*transChannelPtr,
											*transOperatorPtr;
	
	UInt32								bufferInterval,
											index,
											j,
											k,
											numberChannels,
											numberDenominatorTerms,
											numberNumeratorTerms;
											
	SInt16								algebraicTransformOption;
									
	
			// Declare local variables.														
	
	numberDenominatorTerms = reformatOptionsPtr->numberDenominatorTerms;
	numberNumeratorTerms = reformatOptionsPtr->numberNumeratorTerms;
	transformFactor = reformatOptionsPtr->transformFactor;
	transformOffset = reformatOptionsPtr->transformOffset;
	numberChannels = reformatOptionsPtr->numberChannels;
	algebraicTransformOption = reformatOptionsPtr->algebraicTransformOption;
	k1Value = reformatOptionsPtr->algebraicTransformK1Value;
	k2Value = reformatOptionsPtr->algebraicTransformK2Value;
	
	bufferInterval = 1;
	if (inputBISFlag)
		bufferInterval = numberChannels;
	
	ioDoubleBufferPtr = (HDoublePtr)ioBufferPtr;
		
	for (j=0; j<numberSamples; j++)
		{
				// Get the denominator.															
				
		transChannelPtr = reformatOptionsPtr->transformChannelPtr;
		coefficientsPtr = reformatOptionsPtr->coefficientsPtr;
		transOperatorPtr = reformatOptionsPtr->transformOperatorPtr;
		
		denominator = 0;
		for (k=1; k<=numberDenominatorTerms; k++)
			{		
			channelValue = 1;
			if (*transChannelPtr >= 0)
				{ 
				index = *transChannelPtr;
				if (!inputBISFlag)
					index *= numberSamples;
					
				channelValue = ioDoubleBufferPtr[index];
					
				}	// end "if (*transChannelPtr >= 0)" 
			
			if (*transOperatorPtr == 2)	
				denominator *= *coefficientsPtr * channelValue;
				
			else	// *transOperatorPtr != 2
				denominator += *coefficientsPtr * channelValue;
			
			coefficientsPtr++;
			transChannelPtr++;
			transOperatorPtr++;
			
			}	// end "for (j=0; j<numberDenominatorTerms; j++)" 
			
		if (denominator != 0)
			{
					// Get the numerator.													
			
			numerator = 0;	
			for (k=1; k<=numberNumeratorTerms; k++)
				{		
				channelValue = 1;
				if (*transChannelPtr >= 0)
					{
					index = *transChannelPtr;
					if (!inputBISFlag)
						index *= numberSamples;
						
					channelValue = ioDoubleBufferPtr[index];
						
					}	// end "if (*transChannelPtr >= 0)" 
			
				if (*transOperatorPtr == 2)	
					numerator *= *coefficientsPtr * channelValue;
					
				else	// *transOperatorPtr != 2
					numerator += *coefficientsPtr * channelValue;
				
				coefficientsPtr++;
				transChannelPtr++;
				transOperatorPtr++;
				
				}	// end "for (j=0; j<numberDenominatorTerms; j++)" 
		
			numerator /= denominator;
			numerator *= transformFactor;
			numerator += transformOffset;
			*ioDoubleBufferPtr = numerator;
				
			if (algebraicTransformOption >= kAlgebraicTransformThermal_K)
				{
						// Convert to radiant temperature
				
				if (*ioDoubleBufferPtr > 0)
					radiantTemperature = k2Value / log (k1Value / *ioDoubleBufferPtr + 1);
					
				else	// *ioDoubleBufferPtr <= 0
					radiantTemperature = 0;
				
				if (algebraicTransformOption >= kAlgebraicTransformThermal_C)
					{
					radiantTemperature -= 273.15;
						
					if (algebraicTransformOption == kAlgebraicTransformThermal_F)
						radiantTemperature = 1.8 * radiantTemperature + 32;
						
					}	// end "if (algebraicTransformOption >= kAlgebraicTransformThermal_C)"
						
				*ioDoubleBufferPtr = radiantTemperature;
				
				}	// end "if (algebraicTransformOption >= kAlgebraicTransformThermal_K)"
			
			}	// end "if (denominator != 0)" 
			
		else	// denominator == 0 
			*ioDoubleBufferPtr = highSaturatedValue;
			
		ioDoubleBufferPtr += bufferInterval;
		
		}	// end "for (j=0; j<numberSamples; j++)" 
		
}	// end "TransformData"   
