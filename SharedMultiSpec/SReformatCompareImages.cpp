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
//	File:						SReformatCompareImages.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			03/17/2023
//
//	Include files:			"MultiSpecHeaders"
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	This file contains routine to compare two image files by
//								finding the difference in the image files and writing the
//								difference out to another file.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#if defined multispec_wx
	 #include "xImageView.h"
	 #include "xMultiSpec.h"
	 #include "xReformatCompareImagesDialog.h"
#endif	// defined multispec_wx 
	
#if defined multispec_mac || defined multispec_mac_swift
	#define	IDC_LineInterval						11
	#define	IDC_ColumnInterval					14
	#define	IDC_NonSelectedPixels				18
	#define	IDC_TranslateScaleRotateRadio		21
	#define	IDC_TranslateScaleTitle				22
	#define	IDC_LineTranslateScalePrompt		23
	#define	IDC_ColumnTranslateScalePrompt	24
	#define	IDC_LineOffset							25
	#define	IDC_ColumnOffset						26
	#define	IDC_LineScale							27
	#define	IDC_ColumnScale						28
	#define	IDC_RotationTitle						29
	#define	IDC_RotationClockwisePrompt		30
	#define	IDC_RotationClockwise				31
	#define	IDC_ChannelsPrompt					32
	#define	IDC_Channels							33
	#define	IDC_UseMapOrientationAngle			34
	#define	IDC_ReprojectToRadio					35
	#define	IDC_ReferenceFileListPrompt		36
	#define	IDC_ReferenceFileList				37
	#define	IDC_ResampleMethodPrompt			40
	#define	IDC_ResampleMethod					41
#endif	// defined multispec_mac || defined multispec_mac_swift   
                             
#if defined multispec_win
	 #include "WReformatCompareImagesDlg.h"
#endif	// defined multispec_win 	

#define	kActiveMinusCompare			1
#define	kCompareMinusActive			2

SInt16										gCompareSelection;

								

			// Prototypes for routines in this file that are only called by		
			// other routines in this file.													
			
#if defined multispec_mac
	pascal void DrawResamplePopUp (
					DialogPtr							dialogPtr,
					SInt16								itemNumber);
#endif

Boolean CompareImages (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				FileInfoPtr 						outFileInfoPtr,
				ReformatOptionsPtr 				reformatOptionsPtr,
				UInt16*								firstDifferentLinePtr,
				UInt16*								firstDifferentColumnPtr,
				UInt16*								firstDifferentChannelPtr,
				double*								activeImageValuePtr,
				double*								targetImageValuePtr,
				SInt64*								outputNumberComparisonsPtr,
				SInt64*								outputNumberDifferencesPtr,
				double*								maximumDifferencePtr,
				double*								minimumDifferencePtr);

Boolean CompareImagesDialog (
				FileInfoPtr 						fileInfoPtr,
				FileInfoPtr 						outFileInfoPtr,
				ReformatOptionsPtr 				reformatOptionsPtr);

SInt16 GetCompareToImageList (
				DialogPtr							dialogPtr,
				Handle								windowInfoHandle, 
				Boolean								loadListFlag,
				Handle*								referenceWindowInfoHandlePtr, 
				SInt16*								listCountPtr);

void InitializeLineToBackgroundValue (
				FileInfoPtr							outFileInfoPtr,
				HUInt8Ptr							outputBufferPtr,
				UInt32								countOutBytes,
				double								backgroundValue);

Boolean ListCompareImagesResultsInformation (
				ReformatOptionsPtr				reformatOptionsPtr,
				FileInfoPtr							outFileInfoPtr,
				UInt16								firstDifferentLine,
				UInt16								firstDifferentColumn,
				UInt16								firstDifferentChannel,
				double								activeImageValue,
				double								targetImageValue,
				SInt64								numberComparisons,
				SInt64								numberDifferences,
				double								maximumDifference,
				double								minimumDifference);

void SetUpCompareMethodPopupMenu (
				DialogPtr							dialogPtr,
				MenuHandle							popUpResampleSelectionMenu,
				Boolean								thematicTypeFlag,
				SInt16								compareMethod);


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CompareImages
//
//	Software purpose:	This routine compares the two selected images.
//
//	Parameters in:
//
//	Parameters out:
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/14/2022
//	Revised By:			Larry L. Biehl			Date: 09/03/2022

Boolean CompareImages (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				FileIOInstructionsPtr			fileIOInstructions2Ptr,
				FileInfoPtr							outFileInfoPtr,
				ReformatOptionsPtr				reformatOptionsPtr,
				UInt16*								firstDifferentLinePtr,
				UInt16*								firstDifferentColumnPtr,
				UInt16*								firstDifferentChannelPtr,
				double*								activeImageValuePtr,
				double*								targetImageValuePtr,
				SInt64*								outputNumberComparisonsPtr,
				SInt64*								outputNumberDifferencesPtr,
				double*								maximumDifferencePtr,
				double*								minimumDifferencePtr)

{
			// Declare local variables & structures
			
	SInt64								numberComparisons,
											numberDifferences;
											
	double								minimumDifference,
											maximumDifference,
											output8BytePixelValue;
												
	CMFileStream*						outFileStreamPtr;
	FileInfoPtr							fileInfoPtr;
											
	RefCompareImagesOptionsPtr		compareImagesOptionsPtr;
	
	HDoublePtr							file8BytePtr,
											compareFile8BytePtr,
											ioOut8BufferPtr;
	
	unsigned char						*ioOutBufferPtr;
					 						
	SInt32								*ioOut4BufferPtr;
					 					
	SInt16								*compareImagesChannelsPtr;
	
	SInt32								column,
											columnByteSkip,
											countInBytes,
											countOutBytes,
											lastOutputWrittenLine,
											limitIoOutBytes,
											line,
											numberOutputColumns,
											output4BytePixelValue,
											percentComplete,
											totalIOOutBytes,
											writePosOff;
	
	UInt32								channel,
											columnEnd,
											columnStart,
											inOffsetBytes,
											inOffsetIncrement,
											lastLineRead,
											lineEnd,
											lineStart,
											numberBytes,
											numberColumnsChannels,
											numberOutputLines,
											outChannelByteIncrement,
											outNumberBytesPerLineAndChannel,
											outOffsetIncrement,
											startColumn,
											startLine,
											stopColumn,
											stopLine;
	
	SInt16								errCode,
											compareMethod,
											lastPercentComplete,
											numberReadChannels;
												
	UInt16								forceByteCode,
											numberOutChannels;
	
	Boolean								createOutputImageFileFlag,
											continueFlag,
											forceBISFlag;
	
		
			// Initialize local variables.
			
	continueFlag = TRUE;
	maximumDifference = -DBL_MAX;
	minimumDifference = DBL_MAX;
	numberComparisons = 0;
	numberDifferences = 0;
	errCode = noErr;
	writePosOff = 0;
	fileInfoPtr = fileIOInstructionsPtr->fileInfoPtr;
	forceByteCode = reformatOptionsPtr->forceByteCode;
	forceBISFlag = (outFileInfoPtr->bandInterleave == kBIS);
	outFileStreamPtr = GetFileStreamPointer (outFileInfoPtr);
	
	compareImagesChannelsPtr = reformatOptionsPtr->channelPtr;
	compareImagesOptionsPtr = reformatOptionsPtr->compareImagesOptionsPtr;
	createOutputImageFileFlag = compareImagesOptionsPtr->createOutputImageFileFlag;
	compareMethod = compareImagesOptionsPtr->compareMethod;
					
	ioOutBufferPtr = (unsigned char*)reformatOptionsPtr->ioOutBufferPtr;
	ioOut4BufferPtr = (SInt32*)ioOutBufferPtr;
	ioOut8BufferPtr = (double*)ioOutBufferPtr;
				
	lastPercentComplete = -1;
	
	columnStart = reformatOptionsPtr->columnStart;
	columnEnd =  reformatOptionsPtr->columnEnd;
	lineStart = reformatOptionsPtr->lineStart;
	lineEnd =  reformatOptionsPtr->lineEnd;
	
	startColumn = reformatOptionsPtr->startColumn;
	stopColumn = gImageFileInfoPtr->numberColumns;
	startLine = reformatOptionsPtr->startLine;
	stopLine = gImageFileInfoPtr->numberLines;
		
	numberBytes = outFileInfoPtr->numberBytes;
															
	numberOutChannels = reformatOptionsPtr->numberChannels;
	numberOutputColumns = outFileInfoPtr->numberColumns;
	numberOutputLines = outFileInfoPtr->numberLines;
	
	numberReadChannels = reformatOptionsPtr->numberChannels;
	
			// Write a header if requested.
			
	if (createOutputImageFileFlag)
		continueFlag = WriteNewImageHeader (fileInfoPtr,
														outFileInfoPtr,
														(char*)ioOutBufferPtr,
														NULL,
														kFromReformat,
														reformatOptionsPtr->outputFileCode,
														reformatOptionsPtr->lineInterval,
														reformatOptionsPtr->columnInterval,
														kClassDisplay,
														kNoPaletteColorsDefined);
	
			// Return if there was an IO error
			
	if (continueFlag)
		{
				// Set up limit of output buffer to know when to write buffer to
				// the disk file. Make certain which buffer is the output buffer.
				
		countOutBytes = reformatOptionsPtr->countOutBytes;
		//countOutBytes = 0;
		limitIoOutBytes = reformatOptionsPtr->ioOutBufferBytes - countOutBytes;
		totalIOOutBytes = 0;
															
				// Get bytes to skip each time through the column loop for the
				// output buffer.
				
		columnByteSkip = 1;
		if (forceBISFlag)
			columnByteSkip = numberOutChannels;
		columnByteSkip *= numberBytes;
		
				// Get total number of input bytes for one channel.
				// Make certain that this is not more than the number of bytes
				// allowed for in the output file.
				
		countInBytes = stopColumn - startColumn + 1;
		countInBytes *= numberBytes;
		
		countInBytes = MIN (countInBytes,
			(SInt32)(numberBytes * outFileInfoPtr->numberColumns));
						
				// Get increment to skip each time through the channel loop
				// for the input buffer.
				
		inOffsetIncrement = columnEnd - columnStart + 1;
		if (forceBISFlag || fileInfoPtr->bandInterleave == kBIS)
			inOffsetIncrement = 1;
		
				// Get count of bytes to skip each time through the channel
				// loop for the input buffer.
				
		inOffsetBytes = inOffsetIncrement * numberBytes;
		
				// Get count of bytes to skip each time through the channel
				// loop for the output buffer.
										
		outChannelByteIncrement = countOutBytes/numberOutChannels;
		
		if (outFileInfoPtr->bandInterleave == kBIS)
			outChannelByteIncrement /= numberOutputColumns;
		
		else if (outFileInfoPtr->bandInterleave == kBSQ)
			outChannelByteIncrement =
										reformatOptionsPtr->ioOutBufferBytes/numberOutChannels;
						
				// Get increment to skip each time through the channel loop
				// for the output buffer. I.E. allow for the byte count of
				// the output data.
				// Note that this could  be cleaned up some.  outOffsetIncrement and
				// outChannelByteIncrement are really the same thing. One is in values
				// and the other is in bytes.  They are used in two different places.
				
		outOffsetIncrement = outChannelByteIncrement/numberBytes;
		
				// Get the number of bytes per line (all columns) and one band
				// of data.
		
		outNumberBytesPerLineAndChannel = countOutBytes/numberOutChannels;
		if (outFileInfoPtr->bandInterleave == kBIS)
			outNumberBytesPerLineAndChannel /= numberOutputColumns;
							
				// Intialize the nextTime variable to indicate when the next
				// check should occur for a command-.
				
		gNextTime = TickCount ();
		
				// Get the total number of samples in a line of data.
				// It will be used when adjusting the data if request.
				
		numberColumnsChannels =
								((SInt32)columnEnd - columnStart + 1) * numberReadChannels;
									
				// Load some of the File IO Instructions structure that pertain
				// to the specific area being used.
		
		if (continueFlag)
			errCode = SetUpFileIOInstructions (fileIOInstructionsPtr,
															NULL,
															startLine,
															stopLine,
															1,
															columnStart,
															columnEnd,
															1,
															numberReadChannels,
															(UInt16*)compareImagesChannelsPtr,
															kDetermineSpecialBILFlag);
															
		if (errCode == noErr)
			SetUpFileIOInstructions (fileIOInstructions2Ptr,
												NULL,
												startLine,
												lineEnd,
												1,
												columnStart,
												columnEnd,
												1,
												numberReadChannels,
												(UInt16*)compareImagesChannelsPtr,
												kDetermineSpecialBILFlag);
												
		if (errCode != noErr)
			continueFlag = FALSE;
		
		line = 1;
		lastLineRead = -1;
		lastOutputWrittenLine = 0;
		
		if (continueFlag)
			{
					// Turn spin cursor on

			gPresentCursor = kSpin;
			
			for (line=startLine; line<=lineEnd; line++)
				{
				file8BytePtr = (HDoublePtr)gOutputBufferPtr;
				compareFile8BytePtr = (HDoublePtr)gOutputBuffer2Ptr;
				//ioOut1ByteBufferPtr = savedOutBufferPtr;
				
				if (continueFlag)
					{
						  // Get all requested channels for line of
						  // image data. Return if there is a file IO error.

					errCode = GetLineOfData (fileIOInstructionsPtr,
													  line,
													  columnStart,
													  columnEnd,
													  1,
													  gInputBufferPtr,
													  gOutputBufferPtr);
																								 
					if (errCode != noErr)
						 {
						 continueFlag = FALSE;
																							 break;
						 
						 }	// end "if (errCode != noErr)"

				  errCode = GetLineOfData (fileIOInstructions2Ptr,
													 line,
													 columnStart,
													 columnEnd,
													 1,
													 gInputBuffer2Ptr,
													 gOutputBuffer2Ptr);
															  
					if (errCode != noErr)
					  {
					  continueFlag = FALSE;
																						  break;
					  
					  }	// end "if (errCode != noErr)"
							  
					//columnOutBufferPtr = &savedOutBufferPtr;
											
					for (column=1; column<=numberOutputColumns; column++)
						{
								// Set output buffer pointers.
		
						//ioOut1ByteBufferPtr = columnOutBufferPtr;
											
						for (channel=1; channel<=numberOutChannels; channel++)
							{
							output8BytePixelValue = *file8BytePtr - *compareFile8BytePtr;
							//if (*file8BytePtr != *compareFile8BytePtr)
							if (output8BytePixelValue != 0)
								{
								if (numberDifferences == 0)
									{
									*firstDifferentLinePtr = lineStart + line - 1;
									*firstDifferentColumnPtr = columnStart + column - 1;
									*firstDifferentChannelPtr = reformatOptionsPtr->channelPtr[channel-1] + 1;
									*activeImageValuePtr = *file8BytePtr;
									*targetImageValuePtr = *compareFile8BytePtr;
									
									}	// end "if (numberDifferences == 0)"

								numberDifferences++;
								maximumDifference = MAX (maximumDifference, output8BytePixelValue);
								minimumDifference = MIN (minimumDifference, output8BytePixelValue);
								
								}	// if (*file8BytePtr != *compareFile8BytePtr)
								
							numberComparisons++;
							
							if (createOutputImageFileFlag)
								{
								if (compareMethod != kActiveMinusCompare)
									output8BytePixelValue = -output8BytePixelValue;
									
								if (numberBytes == 4)
									{
									output4BytePixelValue = (SInt32)output8BytePixelValue;
									*ioOut4BufferPtr = output4BytePixelValue;
									ioOut4BufferPtr++;
									
									}	// end "if (numberBytes == 4)"
									
								else	// numberBytes != 4
									{
									*ioOut8BufferPtr = output8BytePixelValue;
									ioOut8BufferPtr++;
										
									}	// end "else numberBytes != 4"
								
								}	// end "if (createOutputImageFileFlag)"
								
							file8BytePtr++;
							compareFile8BytePtr++;
								
							}	// end "for (channel=1; channel<=numberOutChannels; channel++)"
	
								// Check if user wants to abort processing.
							
						if (TickCount () >= gNextTime)
							{
							if (!CheckSomeEvents (
													osMask+keyDownMask+updateMask+mDownMask+mUpMask))
								continueFlag = FALSE;
								
							}	// end "if (TickCount () >= nextTime)"
							
						}	// end "for (column=1; column<=numberOutputColumns; ..."
						
					}	// end "if (continueFlag)"
		
						// Write line(s), channel(s) of data when needed.
				
				totalIOOutBytes += countOutBytes;
				
				if (continueFlag && createOutputImageFileFlag && (totalIOOutBytes > limitIoOutBytes))
					{
					errCode = WriteOutputDataToFile (outFileInfoPtr,
																	outFileStreamPtr,
																	ioOutBufferPtr,
																	compareImagesChannelsPtr,
																	numberOutChannels,
																	lastOutputWrittenLine,
																	outNumberBytesPerLineAndChannel,
																	numberOutputLines,
																	outChannelByteIncrement,
																	totalIOOutBytes,
																	reformatOptionsPtr,
																	1);
					
					totalIOOutBytes = 0;
					ioOut4BufferPtr = (SInt32*)ioOutBufferPtr;
					ioOut8BufferPtr = (double*)ioOutBufferPtr;
					
					lastOutputWrittenLine = line;
																	
					if (errCode != noErr)
						{
						continueFlag = FALSE;
																										break;
						
						}	// end "if (errCode != noErr)"
					
					}	// end "if (... && (totalIOOutBytes > limitIoOutBytes))"
				
						// Update status dialog box.
						
				percentComplete = 100 * line/numberOutputLines;
				if (percentComplete != lastPercentComplete)
					{
					LoadDItemValue (gStatusDialogPtr,
											IDC_ShortStatusValue,
											(SInt32)percentComplete);
					lastPercentComplete = (SInt16)percentComplete;
					
					}	// end "if (percentComplete != lastPercentComplete)"
					
				}	// end "for (line=startLine; line<=lineEnd; line++)"
			
					// Flush output buffer if needed.
			
			if (continueFlag && createOutputImageFileFlag && totalIOOutBytes > 0)
				{
				errCode = WriteOutputDataToFile (outFileInfoPtr,
																outFileStreamPtr,
																ioOutBufferPtr,
																compareImagesChannelsPtr,
																numberOutChannels,
																lastOutputWrittenLine,
																outNumberBytesPerLineAndChannel,
																numberOutputLines,
																outChannelByteIncrement,
																totalIOOutBytes,
																reformatOptionsPtr,
																1);
																
				if (errCode != noErr)
					continueFlag = FALSE;
				
				}	// end "if (continueFlag && createOutputImageFileFlag && ...)"
					
			}	// end "if (continueflag)
		
				// Close up any File IO Instructions structure that pertain to the
				// specific area used for image file comparisons.
				
		CloseUpFileIOInstructions (fileIOInstructionsPtr, NULL);
				
		CloseUpFileIOInstructions (fileIOInstructions2Ptr, NULL);

				// Turn spin cursor off
	
		gPresentCursor = kWait;
		
				// Write channel discriptions at the end of the file if needed.
		
		if (continueFlag && createOutputImageFileFlag && reformatOptionsPtr->channelDescriptions)
			WriteChannelDescriptionsAndValues (
									outFileInfoPtr,
									NULL,
									outFileInfoPtr->numberChannels,
									(reformatOptionsPtr->outputFileCode == kAppendToMenuItem));
		/*
		if (continueFlag && createOutputImageFileFlag && outFileInfoPtr->thematicType)
			{
			supportFileType = kITRLFileType;
			if (outFileInfoPtr->format != kErdas74Type)
				supportFileType = kICLRFileType;
			                                
			continueFlag = CreateThematicSupportFile (
															outFileInfoPtr,
															NULL,
															(UInt16)outFileInfoPtr->numberClasses,
															NULL,
															0,
															NULL,
															GetPaletteHandle (),
															NULL,		// newPaletteIndexPtr,
															GetPaletteType (),
															(UInt16)outFileInfoPtr->numberClasses,
															kFromDescriptionCode,
															kPaletteHistogramClassNames,
															kClassDisplay,
															kCollapseClass,
															supportFileType);
				
			}	// end "if (continueFlag && createOutputImageFileFlag && ...)"
		*/
		}	// end "if (continueFlag)"
	
	*outputNumberComparisonsPtr = numberComparisons;
	*outputNumberDifferencesPtr = numberDifferences;
	
	*maximumDifferencePtr = maximumDifference;
	*minimumDifferencePtr = minimumDifference;
		
	return (!continueFlag);
			
}	// end "CompareImages"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CompareImagesControl
//
//	Software purpose:	This routine controls the process of rectifying the
//							input image.
//
//	Parameters in:
//
//	Parameters out:
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/14/2022
//	Revised By:			Larry L. Biehl			Date: 09/02/2022

void CompareImagesControl (void)

{
			// Declare local variables & structures
			
	double								activeImageValue,
											maximumDifference,
											minimumDifference,
											targetImageValue;
											
	SInt64								numberComparisons,
											numberDifferences;
			
	FileIOInstructionsPtr			fileIOInstructionsPtr,
											fileIOInstructions2Ptr;
	
	Handle								outFileInfoHandle,
											reformatOptionsH;
	
	FileInfoPtr							outFileInfoPtr;
	RefCompareImagesOptionsPtr		compareImagesOptionsPtr;
	ReformatOptionsPtr				reformatOptionsPtr;
	
	time_t								startTime;
											
	SInt16								compareImageHandleStatus;
	
	UInt16								firstDifferentChannel,
											firstDifferentColumn,
											firstDifferentLine;
											
	Boolean								continueFlag,
											filesAreTheSameFlag,
											returnFlag;
	
	
			// If spare memory had to be used to load code resources, then exit
			// routine.
			
	if (gMemoryTypeNeeded < 0)
																								return;
																							
			// Code resources loaded okay, so set flag back for non-Code
			// resources.
			
	gMemoryTypeNeeded = 0;
	
			// Initialize local variables.
			
	continueFlag = TRUE;
	filesAreTheSameFlag = FALSE;
	compareImagesOptionsPtr = NULL;
	outFileInfoHandle = NULL;
	reformatOptionsH = NULL;
	fileIOInstructionsPtr = NULL;
	fileIOInstructions2Ptr = NULL;
	reformatOptionsPtr = NULL;
	
			// Get a handle to a block of memory to be used for
			// file information for the new image file.
			// Lock the memory until the information has been loaded
			// in. Then get the pointer to the file information block
	
	continueFlag = GetReformatAndFileInfoStructures (
														&outFileInfoHandle, &reformatOptionsH);
			
			// Get a handle to a block of memory to use for the reformatting
			// options.  Then lock it and get a pointer to the block.
	
	if (continueFlag)
		{
		reformatOptionsPtr = (ReformatOptionsPtr)GetHandlePointer (
																			reformatOptionsH, kLock);
														
		outFileInfoPtr =  (FileInfoPtr)GetHandlePointer (outFileInfoHandle, kLock);
			
		reformatOptionsPtr->swapBytesFlag = gImageFileInfoPtr->swapBytesFlag;
			
		//reformatOptionsPtr->signedOutputDataFlag = gImageFileInfoPtr->signedDataFlag;
		reformatOptionsPtr->signedOutputDataFlag = TRUE;
			
		outFileInfoHandle = NULL;
		if (!gImageFileInfoPtr->thematicType && gImageWindowInfoPtr->descriptionCode)
			reformatOptionsPtr->channelDescriptions = TRUE;
													
		}	// end "if (continueFlag)"
			
			// Get a block of memory to use for the image comparison options.
			
	if (continueFlag)
		{
		if (reformatOptionsPtr->compareImagesOptionsPtr == NULL)
			{
			compareImagesOptionsPtr =
							(RefCompareImagesOptionsPtr)MNewPointer (sizeof (RefCompareImagesOptions));
			if (compareImagesOptionsPtr != NULL)
				{
				reformatOptionsPtr->compareImagesOptionsPtr = compareImagesOptionsPtr;
						
				//compareImagesOptionsPtr->channelsPtr = NULL;
				//compareImagesOptionsPtr->numberChannels =
				//											gImageWindowInfoPtr->totalNumberChannels;
		
						// Get handle to window information for reference image
				
				compareImagesOptionsPtr->compareImageWindowInfoHandle = NULL;
				
				compareImagesOptionsPtr->channelSet = kAllMenuItem;
				compareImagesOptionsPtr->compareMethod = kActiveMinusCompare;
				
				}	// end "if (rectifyImageOptionsPtr != NULL)"
				
			}	// end "if (reformatOptionsPtr->compareImagesOptionsPtr == NULL)"
			
		else	// reformatOptionsPtr->compareImagesOptionsPtr != NULL
			compareImagesOptionsPtr = reformatOptionsPtr->compareImagesOptionsPtr;
			
		if (compareImagesOptionsPtr == NULL)
			continueFlag = FALSE;
			
		}	// end "if (continueFlag)"
		
			// If memory is okay to this point then initialize the vectors.
			
	if (continueFlag)
		{
				// Fill in vector of all image comparison channels.
		
		//for (index=0; index<compareImagesOptionsPtr->numberChannels; index++)
		//  compareImagesOptionsPtr->channelsPtr[index] = (SInt16)index;
			
				// Initialize the format information for the output file
				
		InitializeOutputFileInformation (gImageWindowInfoPtr,
													gImageFileInfoPtr,
													reformatOptionsPtr,
													outFileInfoPtr,
													kKeepSameFormatAsInput);
													
				// The output file will not need to have the bytes swapped.  If the input
				// file contains data with bytes swapped, they will be swapped when read
				// in before writing to the new file.  The user does not have the option
				// to force the output file to have bytes swapped.
													
		outFileInfoPtr->swapBytesFlag = FALSE;
													
				// Get Changes that the user wants to make in the file format
				
		if (CompareImagesDialog (gImageFileInfoPtr,
											outFileInfoPtr,
											reformatOptionsPtr))
			{
					// List the processor name, date and time.
				
			gOutputForce1Code = 0x0001;
			continueFlag = ListHeaderInfo (NULL,
														kLImageInfo,
														&gOutputForce1Code,
														kNoStatisticsUsed,
														continueFlag);

			if (continueFlag)
				{
						// Update parameters in the structure for the output file.
				
				UpdateOutputFileStructure (outFileInfoPtr,
													reformatOptionsPtr,
													1,
													1,
													gImageFileInfoPtr->mapProjectionHandle);
				
						// Get pointer to memory to use to read an image file line
						// into.
						
				UInt16		forceOutputFormatCode = kBIS;		// kDoNotForceFormat;
				//if (outFileInfoPtr->bandInterleave == kBIS)
				//	forceOutputFormatCode = kBIS;
			 						
				continueFlag = GetIOBufferPointers (&gFileIOInstructions[0],
																gImageWindowInfoPtr,
																gImageLayerInfoPtr,
																gImageFileInfoPtr,
																&gInputBufferPtr,
																&gOutputBufferPtr,
																reformatOptionsPtr->columnStart,
										 						reformatOptionsPtr->columnEnd,
										 						1,
										 						reformatOptionsPtr->numberChannels,
										 						(UInt16*)reformatOptionsPtr->channelPtr,
																kPackData,
																forceOutputFormatCode,
																kForceReal8Bytes,
																kDoNotAllowForThreadedIO,
																&fileIOInstructionsPtr);
										
				}	// end "if (continueFlag)"
					
				if (continueFlag)
					{
							// Need to set up a second set of buffers to read the data for the
							// image being compared to.
							
							// Get handle to window information for the active image
							
					continueFlag = GetImageInformationPointers (
											&compareImageHandleStatus,
											compareImagesOptionsPtr->compareImageWindowInfoHandle,
											&gImageWindowInfo2Ptr,
											&gImageLayerInfo2Ptr,
											&gImageFileInfo2Ptr);
							
							// Get pointer to memory to use to read an image file line
							// into.
							
					UInt16		forceOutputFormatCode = kBIS;		// kDoNotForceFormat;
					//if (outFileInfoPtr->bandInterleave == kBIS)
					//	forceOutputFormatCode = kBIS;
							
					continueFlag = GetIOBufferPointers (&gFileIOInstructions[1],
																	gImageWindowInfo2Ptr,
																	gImageLayerInfo2Ptr,
																	gImageFileInfo2Ptr,
																	&gInputBuffer2Ptr,
																	&gOutputBuffer2Ptr,
																	reformatOptionsPtr->columnStart,
																	reformatOptionsPtr->columnEnd,
																	1,
																	reformatOptionsPtr->numberChannels,
																	(UInt16*)reformatOptionsPtr->channelPtr,
																	kPackData,
																	forceOutputFormatCode,
																	kForceReal8Bytes,
																	kDoNotAllowForThreadedIO,
																	&fileIOInstructions2Ptr);
					
					}	// end "if (continueFlag)"
	
					// Get buffer for output data.
					
			if (continueFlag && compareImagesOptionsPtr->createOutputImageFileFlag)
				{
				continueFlag =
							GetReformatOutputBuffer (outFileInfoPtr, reformatOptionsPtr);
		
						// Branch to end of routine if memory is full and do cleanup of handles.
				
				if (continueFlag)
							// Set up the disk file to be used for the new reformatted
							// image file.
					continueFlag =
						!GetReformatOutputFile (outFileInfoPtr, reformatOptionsPtr);
						
				}	// end "if (continueFlag && compareImagesOptionsPtr->createOutputImageFileFlag)"
						
			if (continueFlag)
				{
						// Change cursor to watch cursor until done with process.
				
				MSetCursor (kWait);
					
						// Get status dialog. Also call routine to do
						// activate/update events.
					
				gStatusDialogPtr = GetStatusDialog (kShortStatusInfoID, FALSE);
				if (gStatusDialogPtr)
					{
					MGetString (gTextString, kReformatStrID, IDS_PercentComplete);
					LoadDItemString (
								gStatusDialogPtr, IDC_ShortStatusText, (Str255*)gTextString);
					LoadDItemValue (gStatusDialogPtr, IDC_ShortStatusValue, (SInt32)0);
					ShowStatusDialogWindow (kShortStatusInfoID);
					
							// Everything has now been set up.
							// Do reformatting for BIL output files here.
							
					startTime = time (NULL);
					
					returnFlag = CompareImages (fileIOInstructionsPtr,
															fileIOInstructions2Ptr,
															outFileInfoPtr,
															reformatOptionsPtr,
															&firstDifferentLine,
															&firstDifferentColumn,
															&firstDifferentChannel,
															&activeImageValue,
															&targetImageValue,
															&numberComparisons,
															&numberDifferences,
															&maximumDifference,
															&minimumDifference);
					
							// List Reformat information.
							
					continueFlag = ListCompareImagesResultsInformation (reformatOptionsPtr,
																							outFileInfoPtr,
																							firstDifferentLine,
																							firstDifferentColumn,
																							firstDifferentChannel,
																							activeImageValue,
																							targetImageValue,
																							numberComparisons,
																							numberDifferences,
																							maximumDifference,
																							minimumDifference);
				
							// List the CPU time taken for reformatting.
								
					continueFlag = ListCPUTimeInformation (NULL, continueFlag, startTime);
								
							// Dispose of status dialog
					
					CloseStatusDialog (TRUE);
						
					}	// end "if (gStatusDialogPtr)"
		  		
		  				// Close the output file
		  				
				CloseFile (outFileInfoPtr);
			
						// Reset cursor back to arrow to indicate that process
						// is done
					
				MInitCursor ();
					
				}	// end "if (continueFlag)"
				
					// Dispose of the IO buffer.
										
			DisposeIOBufferPointers (fileIOInstructionsPtr,
												&gInputBufferPtr,
												&gOutputBufferPtr);
																			
			DisposeIOBufferPointers (fileIOInstructions2Ptr,
												&gInputBuffer2Ptr,
												&gOutputBuffer2Ptr);
			
			}	// end "if (ChangeImageFormatDialog (gImageFileInfoPtr, ... "
						
		}	// end "if (continueFlag)"
		
	ReleaseReformatOutputFileInfoAndBuffers (reformatOptionsPtr, gImageFileInfoPtr);
					
}	// end "CompareImagesControl"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		CompareImagesDialog
//
//	Software purpose:	This routine handle the compare images dialog box.
//
//	Parameters in:
//
//	Parameters out:
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/14/2022
//	Revised By:			Larry L. Biehl			Date: 09/01/2022

Boolean CompareImagesDialog (
				FileInfoPtr							fileInfoPtr,
				FileInfoPtr							outFileInfoPtr,
				ReformatOptionsPtr				reformatOptionsPtr)

{
	Boolean								OKFlag;
										
#if defined multispec_mac
	// The following section of code for multispec_mac has not been updated
	// for the Compare Images feature. multispec_mac is not being used any more.
	DialogSelectArea					dialogSelectArea;
	
	Rect									headerBox,
											resampleBox,
											theBox;
	
	DialogPtr							dialogPtr;
	RefCompareImagesOptionsPtr		compareImagesOptionsPtr;
	
	SInt16								*compareImagesChannelsPtr;
									
	UserItemUPP							drawHeaderOptionsPopUpPtr,
											drawResamplePopUpPtr;
	
	double								lastBackgroundValue,
											lastColumnScaleFactor,
											lastLineScaleFactor,
											lastRotationAngle,
											mapOrientationAngle,
											realValue;
	
	Handle								okHandle,
											referenceWindowInfoHandle,
											theHandle;
	
	SInt32								lastColumnShift,
											lastLineShift,
											theNum;
	
	SInt16								itemHit,
											itemHit2,
											numberDecimalPlaces,
											compareMethod,
											theType;
											
	//SignedByte							handleStatus;
	
	Boolean								blankOutsideSelectedAreaFlag,
											dialogWindowShownFlag,
											modalDone;
											

			// Get the modal dialog for the image file format description
				
	dialogPtr = LoadRequestedDialog (kRectifyImageDialogID, kCopyScrap, 1, 2);
	if (dialogPtr == NULL)
																						return (FALSE);
	
	referenceWindowInfoHandle = NULL;
		
			// Intialize local user item proc pointers.
			
	drawHeaderOptionsPopUpPtr = NewUserItemUPP (DrawHeaderOptionsPopUp);
			
	drawResamplePopUpPtr = NewUserItemUPP (DrawResamplePopUp);
												
	SetDialogItemDrawRoutine (dialogPtr, 37, gDrawImageFilePopUpPtr);
	
	CompareImagesDialogInitialize (dialogPtr,
												fileInfoPtr,
												&dialogSelectArea,
												reformatOptionsPtr,
												&gHeaderOptionsSelection,
												&gChannelSelection,
												&blankOutsideSelectedAreaFlag,
												&lastBackgroundValue,
												&compareMethod,
												&gCompareSelection,
												&gFileNamesSelection,
												&referenceWindowInfoHandle,
												&lastLineShift,
												&lastColumnShift,
												&lastLineScaleFactor,
												&lastColumnScaleFactor,
												&lastRotationAngle,
												&mapOrientationAngle);
												
			// Save handle for the OK button for use later.
			
	GetDialogItem (dialogPtr, 1,  &theType, &okHandle, &theBox);
	
	compareImagesOptionsPtr = reformatOptionsPtr->compareImagesOptionsPtr;
	
	compareImagesChannelsPtr = compareImagesOptionsPtr->compareImagesChannelsPtr;

			// To entire image icon.
			//	Reformat area
			
	LoadLineColumnItems (&dialogSelectArea,
									dialogPtr,
									kInitializeLineColumnValues,
									kIntervalEditBoxesExist,
									1);
	
			// "Image Header" popup box.
				
	SetDialogItemDrawRoutine (dialogPtr, 17, &headerBox, drawHeaderOptionsPopUpPtr);
	
			// Make sure that all header choices are enabled to start with.
			
	InitializeHeaderPopupMenu (dialogPtr, gPopUpHeaderOptionsMenu);
	DisableMenuItem (gPopUpHeaderOptionsMenu, kMatlabMenuItem);
	
	if (fileInfoPtr->format != kGAIAType && fileInfoPtr->format != kGAIA2Type)
		DisableMenuItem (gPopUpHeaderOptionsMenu, kGAIAMenuItem);
		
			// Blank pixels outside of selected area check box.
   
	SetDLogControl (dialogPtr, 18, blankOutsideSelectedAreaFlag);
   
	SetDLogControl (dialogPtr,
							IDC_TranslateScaleRotateRadio,
							(compareMethod == kTranslateScaleRotate));
							
	SetDLogControl (dialogPtr,
							IDC_ReprojectToRadio,
							(compareMethod == kReprojectToReferenceImage));
			
	lastBackgroundValue = rectifyImageOptionsPtr->backgroundValue;

	LoadDItemRealValue (dialogPtr, 20, lastBackgroundValue, 0);
   
   		// Load the default rectification settings.
			
	LoadDItemValue (dialogPtr, 25, lastLineShift);
	
	LoadDItemValue (dialogPtr, 26, lastColumnShift);
			
	LoadDItemRealValue (dialogPtr, 27, lastLineScaleFactor, 2);
			
	LoadDItemRealValue (dialogPtr, 28, lastColumnScaleFactor, 2);
	
	LoadDItemRealValue (dialogPtr, 31, lastRotationAngle, 1);

			//	Set the draw routine for the rectification channel popup box.
			
	if (!fileInfoPtr->thematicType)
		{
		SetDialogItemDrawRoutine (dialogPtr, 33, gDrawChannelsPopUpPtr);
		
		}	// end "if (!fileInfoPtr->thematicType)"
				
	SetDialogItemDrawRoutine (
							dialogPtr, 41, &resampleBox, drawResamplePopUpPtr);
	DisableMenuItem (gPopUpCompareImagesMenu, kCompareMinusActive);
		
			// Center the dialog and then show it.
	
	dialogWindowShownFlag = FALSE;
	if (gMultiSpecWorkflowInfo.workFlowCode != 1 &&
														gMultiSpecWorkflowInfo.workFlowCode != 2)
		{
		ShowDialogWindow (dialogPtr, kRectifyImageDialogID, kSetUpDFilterTable);
		
		dialogWindowShownFlag = TRUE;
		
		gDialogItemDescriptorPtr[20] = kDItemReal + kDItemMinus;
		gDialogItemDescriptorPtr[25] = kDItemMinus;
		gDialogItemDescriptorPtr[26] = kDItemMinus;
		gDialogItemDescriptorPtr[27] = kDItemReal;
		gDialogItemDescriptorPtr[28] = kDItemReal;
		gDialogItemDescriptorPtr[31] = kDItemReal + kDItemMinus;
		
				// Set default text selection to first edit text item
				
		SelectDialogItemText (dialogPtr, 25, 0, INT16_MAX);
	
		itemHit = 0;
		
		}	// end "if (gMultiSpecWorkflowInfo.workFlowCode != 1 && ..."
		
	else	// gMultiSpecWorkflowInfo.workFlowCode == 1 || ...
		itemHit = gMultiSpecWorkflowInfo.workFlowCode;
								
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
				case 6:			// Entire area to selected area switch.
				case 42:			// Entire area to selected area switch. (Appearance Manager)
				case 9:			//	 lineStart
				case 10:			//	 lineEnd
				case 12:			//	 columnStart
				case 13:			//	 columnEnd
					DialogLineColumnHits (&dialogSelectArea,
													dialogPtr,
													itemHit,
													theHandle,
													theNum);
					break;
					
				case 17:				// "Write Erdas Header" popup box
					itemHit2 = StandardPopUpMenu (dialogPtr,
															itemHit-1,
															itemHit,
															gPopUpHeaderOptionsMenu,
															gHeaderOptionsSelection,
															kPopUpHeaderOptionsMenuID);
													
					if (itemHit2 != 0)
						gHeaderOptionsSelection = itemHit2;
	
							// Make certain that the correct label is drawn in the
							// header options pop up box.
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
					
				case 18:				// "nonselected pixels to background" Check box
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					blankOutsideSelectedAreaFlag = GetDLogControl (dialogPtr, 18);
					break;
					
				case 20:				//	 background value
					realValue = GetDItemRealValue (dialogPtr, itemHit);
					if (realValue < minBackgroundValue || realValue > maxBackgroundValue)
						RealNumberErrorAlert (lastBackgroundValue, dialogPtr, itemHit, 0);
										
					else	// lastBackgroundValue >=0 && ...
						lastBackgroundValue = realValue;
					break;
					
				case 21:				// Translate, scale, rotate image
					SetDLogControl (dialogPtr, IDC_TranslateScaleRotateRadio, 1);
					SetDLogControl (dialogPtr, IDC_ReprojectToRadio, 0);
					compareMethod = kTranslateScaleRotate;
					
					RectifyImageDialogOnReferenceFile (dialogPtr,
																	compareMethod,
																	gFileNamesSelection,
																	&referenceWindowInfoHandle,
																	&dialogSelectArea);
	
							// Set default text selection to first edit text item
							
					SelectDialogItemText (dialogPtr, 25, 0, INT16_MAX);
					break;
					
				case 25:				//	 line shift
					if (labs (theNum) >
										kMaxNumberLines - gImageWindowInfoPtr->maxNumberLines)
						NumberErrorAlert (lastLineShift, dialogPtr, itemHit);
										
					else	// labs (theNum) <= ...->maxNumberColumns
						lastLineShift = theNum;
					break;
					
				case 26:				//	 column shift
					if (labs (theNum) >
									kMaxNumberLines - gImageWindowInfoPtr->maxNumberColumns)
						NumberErrorAlert (lastColumnShift, dialogPtr, itemHit);
										
					else	// labs (theNum) <= ...->maxNumberColumns
						lastColumnShift = theNum;
					break;
					
				case 27:				//	 line scale factor
					lastLineScaleFactor = GetDItemRealValue (dialogPtr, itemHit);
					break;
					
				case 28:				//	 column scale factor
					lastColumnScaleFactor = GetDItemRealValue (dialogPtr, itemHit);
					break;
					
				case 31:				//	 rotation angle
					realValue = GetDItemRealValue (dialogPtr, itemHit);
					if (realValue > 180 || realValue < -180)
						RealNumberErrorAlert (lastRotationAngle, dialogPtr, itemHit, 1);
					
					else	// realValue <= 180 && realValue >= -180
						lastRotationAngle = realValue;
					
					if (mapOrientationAngle != 0)
						{
						if (lastRotationAngle == mapOrientationAngle)
							SetDItemValue (dialogPtr, 34, 1);
						
						else	// lastRotationAngle != mapOrientationAngle
							SetDItemValue (dialogPtr, 34, 0);
						
						}	// end "if (mapOrientationAngle != 0)"
					break;
					
				case 33:				//	 Channels
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
						ChannelsDialog (
								(SInt16*)&rectifyImageOptionsPtr->numberChannelsToRectify,
								rectifyChannelPtr,
								gImageLayerInfoPtr,
								fileInfoPtr,
								1,
								kNoTransformation,
								NIL,
								gImageWindowInfoPtr->totalNumberChannels,
								gChannelSelection);
													
						if (rectifyImageOptionsPtr->numberChannelsToRectify ==
														gImageWindowInfoPtr->totalNumberChannels)
							itemHit = 1;
								
						HiliteControl ((ControlHandle)okHandle, 0);
						
						}	// end "if (itemHit == 2)"
					
					if (itemHit != 0)
						gChannelSelection = itemHit;
	
							// Make certain that the correct label is drawn in the
							// channel pop up box.
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
					
				case 34:				// "use map orientation angle" Check box
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					lastRotationAngle = 0;
					numberDecimalPlaces = 1;
					if (GetDLogControl (dialogPtr, 34))
						{
						lastRotationAngle = mapOrientationAngle;
						numberDecimalPlaces = 6;
						
						}	// end "if (GetDLogControl (dialogPtr, 34))"
						
					LoadDItemRealValue (
									dialogPtr, 31, lastRotationAngle, numberDecimalPlaces);
					SelectDialogItemText (dialogPtr, 31, 0, INT16_MAX);
					break;
					
				case 35:				// Compare images
					SetDLogControl (dialogPtr, IDC_TranslateScaleRotateRadio, 0);
					SetDLogControl (dialogPtr, IDC_ReprojectToRadio, 1);
					compareMethod = kReprojectToReferenceImage;
					
					RectifyImageDialogOnReferenceFile (dialogPtr,
																	compareMethod,
																	gFileNamesSelection,
																	&referenceWindowInfoHandle,
																	&dialogSelectArea);
	
							// Set default text selection to first edit text item
							
					SelectDialogItemText (dialogPtr, 9, 0, INT16_MAX);
					break;
					
				case 37:				// Reference image to register against popup box
					if (gFileNamesSelection<0)
						break;
						
					itemHit2 = StandardPopUpMenu (dialogPtr,
															itemHit-1,
															itemHit,
															gPopUpTemporaryMenu,
															gFileNamesSelection,
															kColorMenuID);
													
					if (itemHit2 != gFileNamesSelection && itemHit2 != 0)
						gFileNamesSelection = RectifyImageDialogOnReferenceFile (
																		dialogPtr,
																		compareMethod,
																		itemHit2,
																		&referenceWindowInfoHandle,
																		&dialogSelectArea);
	
							// Make certain that the correct label is drawn in the
							// reference image pop up box.
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
					
				case 41:				// "Resample Code" popup box
					
					itemHit2 = StandardPopUpMenu (dialogPtr,
															itemHit-1,
															itemHit,
															gPopUpCompareImagesMenu,
															gCompareSelection,
															kPopUpResampleMenuID);
													
					if (itemHit2 != 0)
						gCompareSelection = itemHit2;
	
							// Make certain that the correct label is drawn in the
							// resample code pop up box.
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
						
				}	// end "switch (itemHit)"
				
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
			
			if (itemHit == 1)
				{
				itemHit2 = 0;
				if (lastLineScaleFactor <= 0)
					itemHit2 = 27;
				
				if (itemHit2 ==0 && lastColumnScaleFactor <= 0)
					itemHit2 = 28;
					
				if (itemHit2 != 0)
					{
					RealNumberErrorAlert (0, dialogPtr, itemHit2, -1);
													
					itemHit = 0;
					
					}	// end "if (itemHit2 != 0)"
				
				}	// end "if (itemHit == 1)"
				
			if	(itemHit == 1)      // User selected OK for information
				{
				modalDone = TRUE;
				OKFlag = TRUE;
				
				CompareImageDialogOK (dialogPtr,
												outFileInfoPtr,
												fileInfoPtr,
												gImageWindowInfoPtr,
												gImageLayerInfoPtr,
												&dialogSelectArea,
												reformatOptionsPtr,
												gHeaderOptionsSelection,
												GetDLogControl (dialogPtr, 18),
												gChannelSelection,
												lastBackgroundValue,
												compareMethod,
												gCompareSelection,
												referenceWindowInfoHandle,
												lastLineShift,
												lastColumnShift,
												lastLineScaleFactor,
												lastColumnScaleFactor,
												lastRotationAngle);
										
				}	// end "if (itemHit == 1)"
				
			if	(itemHit == 2)      // User selected Cancel for information
				{
				modalDone = TRUE;
				OKFlag = FALSE;
				
				}	// end "if	(itemHit == 2)"
				
			}	// end "else itemHit <= 2"
				
		} while (!modalDone);
		
			// Remove file names from temporary popup menu.

	for (itemHit=0; itemHit<=gNumberOfIWindows; itemHit++)
		DeleteMenuItem (gPopUpTemporaryMenu, 1);
		
	DisposeUserItemUPP (drawHeaderOptionsPopUpPtr);
		
	CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
#endif	// defined multispec_mac
	
	#if defined multispec_win
		CMReformatCompareImagesDlg*		dialogPtr = NULL;
		
		TRY
			{
			dialogPtr = new CMReformatCompareImagesDlg ();
			                                                                     
			OKFlag = dialogPtr->DoDialog (outFileInfoPtr,
													fileInfoPtr,
													gImageWindowInfoPtr,
													gImageLayerInfoPtr,
													reformatOptionsPtr);
			
			delete dialogPtr;
			}
			
		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			}
		END_CATCH_ALL
	#endif	// defined multispec_win

	#if defined multispec_wx
		CMReformatCompareImagesDlg*		dialogPtr = NULL;
	
		dialogPtr = new CMReformatCompareImagesDlg (NULL);
		
		OKFlag = dialogPtr->DoDialog (outFileInfoPtr,
														fileInfoPtr,
														gImageWindowInfoPtr,
														gImageLayerInfoPtr,
														reformatOptionsPtr);
			
		delete dialogPtr;
	#endif	// defined multispec_wx
	
	return (OKFlag);
	
}	// end "CompareImagesDialog"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CompareImagesDialogInitialize
//
//	Software purpose:	This routine handles the compare images dialog box.
//
//	Parameters in:
//
//	Parameters out:
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/14/2022
//	Revised By:			Larry L. Biehl			Date: 03/17/2023

void CompareImagesDialogInitialize (
				DialogPtr							dialogPtr,
				FileInfoPtr							fileInfoPtr,
				DialogSelectArea*					dialogSelectAreaPtr,
				ReformatOptionsPtr				reformatOptionsPtr,
				SInt16*								headerOptionsSelectionPtr,
				SInt16*								channelSelectionPtr,
				Boolean*								createOutputImageFileFlagPtr,
				SInt16*								compareAlgorithmCodePtr,
				SInt16*								fileNamesSelectionPtr,
				Handle*								referenceWindowInfoHandlePtr)

{
	RefCompareImagesOptionsPtr		compareImagesOptionsPtr;
	SInt16								*compareChannelsPtr;
	
	SInt16								entireIconItem,
											listCount;
	
				
			// Initialize selected area structure.

	entireIconItem = 6;
	if (gAppearanceManagerFlag)
		entireIconItem = 42;
	
	InitializeDialogSelectArea (dialogSelectAreaPtr,
											gImageWindowInfoPtr,
											gActiveImageWindow,
											reformatOptionsPtr->columnStart,
											reformatOptionsPtr->columnEnd,
											reformatOptionsPtr->columnInterval,
											reformatOptionsPtr->lineStart,
											reformatOptionsPtr->lineEnd,
											reformatOptionsPtr->lineInterval,
											9,
											entireIconItem,
											kDontAdjustToBaseImage);

	FileStringPtr fileNamePtr =
		(FileStringPtr)GetFileNamePPointerFromFileInfo (fileInfoPtr);

	#if defined multispec_win || defined multispec_wx
		LoadDItemString (dialogPtr, IDC_InputFileName, (char*)&fileNamePtr[2]);
	#endif
	
	compareImagesOptionsPtr = reformatOptionsPtr->compareImagesOptionsPtr;
	compareChannelsPtr = reformatOptionsPtr->channelPtr;

			// To entire image icon.
			//	Reformat area

	#if defined multispec_mac
		if (gAppearanceManagerFlag)
			HideDialogItem (dialogPtr, 6);
		else	// !gAppearanceManagerFlag
			HideDialogItem (dialogPtr, 42);
	#endif	// defined multispec_mac
		
	LoadLineColumnItems (dialogSelectAreaPtr,
								dialogPtr,
								kInitializeLineColumnValues,
								kIntervalEditBoxesDoNotExist,
								1);
	HideDialogItem (dialogPtr, IDC_LineInterval);
	HideDialogItem (dialogPtr, IDC_ColumnInterval);
	
			// Adjust header format from the default if needed.
			
	*headerOptionsSelectionPtr = kERDAS74MenuItem;
	if (reformatOptionsPtr->headerFormat == kTIFFType)
		*headerOptionsSelectionPtr = kTIFFGeoTIFFMenuItem;
	else if (fileInfoPtr->format == kGAIAType || fileInfoPtr->format == kGAIA2Type)
		*headerOptionsSelectionPtr = kGAIAMenuItem;

			//	Set the draw routine for the rectification channel popup box.
			
	if (fileInfoPtr->thematicType)
		{
		HideDialogItem (dialogPtr, IDC_ChannelsPrompt);
		HideDialogItem (dialogPtr, IDC_Channels);
		
		}	// end "if (fileInfoPtr->thematicType)"

	*channelSelectionPtr = compareImagesOptionsPtr->channelSet;
	/*
	*channelSelectionPtr = kAllMenuItem;
	if (reformatOptionsPtr->numberChannels <
												gImageWindowInfoPtr->totalNumberChannels)
		*channelSelectionPtr = kSubsetMenuItem;
	*/
	*createOutputImageFileFlagPtr = compareImagesOptionsPtr->createOutputImageFileFlag;
	
			// Check if any images can be used for reference images.
			
	*fileNamesSelectionPtr = GetCompareToImageList (dialogPtr,
																		gActiveImageWindowInfoH,
																		TRUE,
																		referenceWindowInfoHandlePtr,
																		&listCount);
																		
			// Set sampling procedure
			
	*compareAlgorithmCodePtr = compareImagesOptionsPtr->compareMethod;
	SetUpCompareMethodPopupMenu (dialogPtr,
											gPopUpCompareImagesMenu,
											fileInfoPtr->thematicType,
											compareImagesOptionsPtr->compareMethod);
	
}	// end "CompareImagesDialogInitialize"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CompareImagesDialogOK
//
//	Software purpose:	This routine handle the compare images dialog box.
//
//	Parameters in:
//
//	Parameters out:
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/14/2022
//	Revised By:			Larry L. Biehl			Date: 08/31/2022

void CompareImagesDialogOK (
				DialogPtr							dialogPtr,
				FileInfoPtr							outFileInfoPtr,
				FileInfoPtr							fileInfoPtr,
				WindowInfoPtr						windowInfoPtr,
				LayerInfoPtr						layerInfoPtr,
				DialogSelectArea*					dialogSelectAreaPtr,
				ReformatOptionsPtr				reformatOptionsPtr,
				SInt16								headerOptionsSelection,
				SInt16								channelSelection,
				Boolean								createOutputImageFileFlag,
				SInt16								algorithmCode,
				Handle								compareImageWindowInfoHandle)

{
	RefCompareImagesOptionsPtr	      compareImagesOptionsPtr;
	SInt16									*compareImagesChannelPtr;
	
	SInt32									index;
											
	
	compareImagesOptionsPtr = reformatOptionsPtr->compareImagesOptionsPtr;
	compareImagesChannelPtr = reformatOptionsPtr->channelPtr;
	
			// Compare image algorithm.
											
	compareImagesOptionsPtr->compareMethod = algorithmCode;
	compareImagesOptionsPtr->compareImageWindowInfoHandle = compareImageWindowInfoHandle;
	
			// Get write output file option.
			
	compareImagesOptionsPtr->createOutputImageFileFlag = createOutputImageFileFlag;
	
			// Selected area for output file.
	
	reformatOptionsPtr->lineStart = (SInt32)dialogSelectAreaPtr->lineStart;
	reformatOptionsPtr->lineEnd = (SInt32)dialogSelectAreaPtr->lineEnd;
	reformatOptionsPtr->lineInterval = 1;
	reformatOptionsPtr->columnStart = (SInt32)dialogSelectAreaPtr->columnStart;
	reformatOptionsPtr->columnEnd = (SInt32)dialogSelectAreaPtr->columnEnd;
	reformatOptionsPtr->columnInterval = 1;

			// Item 17: "Write Erdas Header" popup box.
						
	reformatOptionsPtr->headerFormat =
								GetHeaderFormatFromPopUpSelection (headerOptionsSelection);
						
	if (reformatOptionsPtr->headerFormat == kNoneType)
		outFileInfoPtr->numberHeaderBytes = 0;
	
			// Set the band interleave depending on the file format.

	outFileInfoPtr->bandInterleave = kBIS;
	if (reformatOptionsPtr->headerFormat == kArcViewType)
		outFileInfoPtr->bandInterleave = kBIS;
			
	//else if (reformatOptionsPtr->headerFormat == kErdas74Type)
	//	outFileInfoPtr->bandInterleave = kBIL;
			
	else if (reformatOptionsPtr->headerFormat == kTIFFType)
		outFileInfoPtr->bandInterleave = kBIS;

			// Channels
			
	if (channelSelection == kAllMenuItem)
		{
		reformatOptionsPtr->numberChannels = gImageWindowInfoPtr->totalNumberChannels;
		for (index=0; index<reformatOptionsPtr->numberChannels; index++)
			reformatOptionsPtr->channelPtr[index] = (SInt16)index;
			
		}	// end "if (channelSelection == kAllMenuItem)"

	compareImagesOptionsPtr->channelSet = channelSelection;
					
			// Load the compare images data channels vector to be used by GetLineOfData.
															
	LoadChannelsVector (kNoProject,
								kNoTransformation,
								reformatOptionsPtr->numberChannels,
								reformatOptionsPtr->channelPtr,
								(SInt16*)&reformatOptionsPtr->numberChannels,
								reformatOptionsPtr->channelPtr);

			// Get the file information for the selected channels.  The
			// information will be loaded into the window information
			// structure.
		
	GetFileInformationForChannelList (windowInfoPtr,
													layerInfoPtr,
													fileInfoPtr,
													NULL,
													(SInt16)reformatOptionsPtr->numberChannels);
	
}	// end "CompareImagesDialogOK"


/*
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CompareImagesDialogOnAlgorithmCode
//
//	Software purpose:	This routine handles showing and hiding options relative to
//							the comparison algorithm that the user has selected.
//
//	Parameters in:
//
//	Parameters out:
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/24/2007
//	Revised By:			Larry L. Biehl			Date: 08/26/2022

void CompareImagesDialogOnAlgorithmCode (
				DialogPtr							dialogPtr,
				SInt16								rectifyCode,
				Boolean								blankOutsideSelectedAreaFlag,
				double								mapOrientationAngle)

{
	SetDLogControl (dialogPtr,
							IDC_NonSelectedPixels,
							blankOutsideSelectedAreaFlag);
											
	ShowDialogItem (dialogPtr, IDC_ReferenceFileListPrompt);
	ShowDialogItem (dialogPtr, IDC_ReferenceFileList);
	ShowDialogItem (dialogPtr, IDC_CompareAlgorithmPrompt);
	ShowDialogItem (dialogPtr, IDC_CompareAlgorithm);
			
}	// end "CompareImagesDialogOnAlgorithmCode"
*/


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CompareImagesDialogOnCreateImageFile
//
//	Software purpose:	This routine handles showing and hiding options relative to
//							the rectification code that the user has selected.
//
//	Parameters in:
//
//	Parameters out:
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/01/2022
//	Revised By:			Larry L. Biehl			Date: 09/01/2022

void CompareImagesDialogOnCreateImageFile (
				DialogPtr							dialogPtr,
				Boolean								createImageFileFlag)

{
	if (createImageFileFlag)
		{
		ShowDialogItem (dialogPtr, IDC_HeaderPrompt);
		ShowDialogItem (dialogPtr, IDC_Header);
		ShowDialogItem (dialogPtr, IDC_CompareAlgorithmPrompt);
		ShowDialogItem (dialogPtr, IDC_CompareAlgorithm);
		
		}	// end "if (createImageFileFlag)"
		
	else	// !createImageFileFlag
		{
		HideDialogItem (dialogPtr, IDC_HeaderPrompt);
		HideDialogItem (dialogPtr, IDC_Header);
		HideDialogItem (dialogPtr, IDC_CompareAlgorithmPrompt);
		HideDialogItem (dialogPtr, IDC_CompareAlgorithm);
		
		}	// else "!createImageFileFlag"
	
}	// end "CompareImagesDialogOnCreateImageFile"


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 CompareImagesDialogOnReferenceFile
//
//	Software purpose:	The purpose of this routine is to handle a user change in the
//							target file dialog selection.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:
//
// Called By:			RectifyImageDialog
//
//	Coded By:			Larry L. Biehl			Date: 02/20/2007
//	Revised By:			Larry L. Biehl			Date: 11/08/2019
	                
SInt16 CompareImagesDialogOnReferenceFile (
				DialogPtr							dialogPtr,
				SInt16								compareMethod,
				SInt16								fileNamesSelection,
				Handle*								referenceWindowInfoHandlePtr,
				DialogSelectArea*					dialogSelectAreaPtr)
	
{
	#if defined multispec_win
		CComboBox* 							comboBoxPtr;
	#endif	// defined multispec_win
   #if defined multispec_wx
		wxChoice* 							referenceListCtrl;
	#endif	// defined multispec_wx
	
	UInt32								windowIndex;
	
	  
	if (fileNamesSelection > 0)
		{
		#if defined multispec_mac
			windowIndex = kImageWindowStart + fileNamesSelection - 1;
		#endif	// defined multispec_mac

		#if defined multispec_win
			comboBoxPtr = (CComboBox*)dialogPtr->GetDlgItem (IDC_ReferenceFileList);
			windowIndex = (UInt32)comboBoxPtr->GetItemData (fileNamesSelection - 1);
		#endif	// defined multispec_win
		
		#if defined multispec_wx
			referenceListCtrl =
							(wxChoice*)wxWindow::FindWindowById (IDC_ReferenceFileList);
			SInt64 windowIndex64 =
					(SInt64)((int*)referenceListCtrl->GetClientData (
																			fileNamesSelection - 1));
			windowIndex = (UInt32)windowIndex64;
		#endif  // defined multispec_wx
				
		*referenceWindowInfoHandlePtr =
												GetWindowInfoHandle (gWindowList[windowIndex]);
		
		}	// end "if (fileNamesSelection > 0)"
		
	else	// fileNamesSelection <= 0
			// This is an error.  For now use the active image window.
		*referenceWindowInfoHandlePtr = gActiveImageWindowInfoH;

	return (fileNamesSelection);
	
}	// end "CompareImagesDialogOnReferenceFile"


#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawResamplePopUp
//
//	Software purpose:	The purpose of this routine is to draw the resample
//							prompt and selection for the resample options 
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
//	Coded By:			Larry L. Biehl			Date: 05/25/2012
//	Revised By:			Larry L. Biehl			Date: 08/26/2012

pascal void DrawResamplePopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.									
			
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpCompareImagesMenu,
								gCompareSelection,
								TRUE);
	
}	// end "DrawResamplePopUp" 
#endif	// defined multispec_mac



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetCompareToImageList
//
//	Software purpose:	The purpose of this routine is to determine the number of image 
//							windows that the file represented by the active image window can
//							be compared to. If the input flag is set, a list of the
//							possible images will be made.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 05/15/2022
//	Revised By:			Larry L. Biehl			Date: 09/01/2022

SInt16 GetCompareToImageList (
				DialogPtr							dialogPtr, 
				Handle								windowInfoHandle, 
				Boolean								loadListFlag,
				Handle*								referenceWindowInfoHandlePtr, 
				SInt16*								listCountPtr)

{
	#if defined multispec_win                                
		CComboBox* 							comboBoxPtr;
	#endif	// defined multispec_win

   #if defined multispec_wx
		wxChoice*							referenceListCtrl;
   #endif	// defined multispec_wx
	
	FileInfoPtr							fileInfoPtr;
	
	WindowInfoPtr						compareWindowInfoPtr,
											windowInfoPtr;
											
	WindowPtr							windowPtr;
	
	Handle								compareWindowInfoHandle;
	
	SInt16								firstEnabledMenuItem,
											imageListLength,
								 			window,
											windowIndex;
	
	Boolean								includeFlag;
	
	SignedByte							handleStatus1,
											handleStatus2,
											handleStatus3;
	
	
	#if defined multispec_win
		USES_CONVERSION;
	#endif

			// Initialize local variables.													
	
	imageListLength = 0;
	*listCountPtr = 0;
	firstEnabledMenuItem = SHRT_MAX;
	
	if (gNumberOfIWindows >= 2 && windowInfoHandle != NULL)
		{
		#if defined multispec_wx
			referenceListCtrl =
								(wxChoice*)wxWindow::FindWindowById (IDC_ReferenceFileList);
		#endif

		window = 0;
		windowIndex = kImageWindowStart;
		
		windowInfoPtr = (WindowInfoPtr)GetHandleStatusAndPointer (
															windowInfoHandle,
															&handleStatus1);

		do
			{
			includeFlag = FALSE;
			
			windowPtr = gWindowList[windowIndex];
			compareWindowInfoHandle = GetWindowInfoHandle (windowPtr);
			
			compareWindowInfoPtr = (WindowInfoPtr)GetHandleStatusAndPointer (
																compareWindowInfoHandle,
																&handleStatus2);
			
			if (compareWindowInfoPtr != NULL &&
						windowInfoHandle != compareWindowInfoHandle)
				includeFlag = TRUE;
				
			if (windowInfoPtr->totalNumberChannels !=
														compareWindowInfoPtr->totalNumberChannels)
				includeFlag = FALSE;
					
			else if (windowInfoPtr->maxNumberLines !=
														compareWindowInfoPtr->maxNumberLines)
				includeFlag = FALSE;
						
			else if (windowInfoPtr->maxNumberColumns !=
														compareWindowInfoPtr->maxNumberColumns)
				includeFlag = FALSE;
				
			if (includeFlag)
				(*listCountPtr)++;
			
			if (loadListFlag)
				{													
				fileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (
																compareWindowInfoPtr->fileInfoHandle,
																&handleStatus3);
					
				#if defined multispec_mac  
					imageListLength++;
					AppendMenu (gPopUpTemporaryMenu, "\pNewFile");
					UCharPtr fileNamePtr =
										(UCharPtr)GetFileNamePPointerFromFileInfo (fileInfoPtr);
					MSetMenuItemText (gPopUpTemporaryMenu,
											imageListLength, 
											fileNamePtr,
											kASCIICharString);
				#endif	// defined multispec_mac   
		
				#if defined multispec_win
					if (includeFlag) 
						{
						imageListLength++;
						comboBoxPtr =
									(CComboBox*)dialogPtr->GetDlgItem (IDC_ReferenceFileList);
						comboBoxPtr->AddString ((LPCTSTR)_T("\0NewFile\0"));
						FileStringPtr fileNamePtr =
									(FileStringPtr)GetFileNamePPointerFromFileInfo (fileInfoPtr);
						dialogPtr->SetComboItemText (IDC_ReferenceFileList, 
																imageListLength-1, 
																&fileNamePtr[2],
																kUTF8CharString);
						comboBoxPtr->SetItemData (imageListLength-1, windowIndex);
																
						}	// end "if (includeFlag)"
				#endif	// defined multispec_win   
				
				#if defined multispec_wx
					if (includeFlag) 
						{
						imageListLength++;
						FileStringPtr fileNamePtr =
								(FileStringPtr)GetFileNamePPointerFromFileInfo (fileInfoPtr);
						referenceListCtrl->Append (&fileNamePtr[2]);
						SInt64 windowIndex64 = windowIndex;
						referenceListCtrl->SetClientData (
														imageListLength-1, (void*)windowIndex64);

						}
				#endif
		
				if (includeFlag)
					{
					if (firstEnabledMenuItem == SHRT_MAX)
						{
						firstEnabledMenuItem = imageListLength;
						*referenceWindowInfoHandlePtr = compareWindowInfoHandle;
						
						}	// end "if (firstEnabledMenuItem == SHRT_MAX)"
						
					}	// end "if (includeFlag)" 
				
				#if defined multispec_mac  
					else	// !includeFlag 
						DisableMenuItem (gPopUpTemporaryMenu, imageListLength);
				#endif	// defined multispec_mac   
			
				MHSetState (compareWindowInfoPtr->fileInfoHandle, handleStatus3);
					
				}	// end "if (loadListFlag)" 
			
			MHSetState (compareWindowInfoHandle, handleStatus2);
			
			window++;
			windowIndex++;
														
			}	while (window<gNumberOfIWindows);
			
		MHSetState (windowInfoHandle, handleStatus1);
			
		}	// end "if (gNumberOfIWindows >= 2 && ..."
		
	if (loadListFlag && firstEnabledMenuItem != SHRT_MAX)
		imageListLength = firstEnabledMenuItem;
		
	return (imageListLength);
			
}	// end "GetCompareToImageList"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				Boolean ListCompareImagesResultsInformation
//
//	Software purpose:	The purpose of this routine is to list the results
//							of the reformat compare images operation.
//
//	Parameters in:		Pointer to reformat instruction structure.
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 08/26/2022
//	Revised By:			Larry L. Biehl			Date: 09/01/2022

Boolean ListCompareImagesResultsInformation (
				ReformatOptionsPtr				reformatOptionsPtr, 
				FileInfoPtr							outFileInfoPtr,
				UInt16								firstDifferentLine,
				UInt16								firstDifferentColumn,
				UInt16								firstDifferentChannel,
				double								activeImageValue,
				double								targetImageValue,
				SInt64								numberComparisons,
				SInt64								numberDifferences,
				double								maximumDifference,
				double								minimumDifference)

{
	char									numberStringWithCommas[32];
	
	CharPtr								fileNamePtr;
	RefCompareImagesOptionsPtr		compareImagesOptionsPtr;
	//SInt16								*compareImagesChannelPtr;
	
	//UInt32								channelIndex,
	//										index;
	
	Handle								fileInfoHandle;
											
	SInt16								stringLength;
											
	Boolean								continueFlag;
	
	
	continueFlag = TRUE;
	compareImagesOptionsPtr = reformatOptionsPtr->compareImagesOptionsPtr;
	
			// Include compared with image file name
			
	fileInfoHandle = GetFileInfoHandle (compareImagesOptionsPtr->compareImageWindowInfoHandle);
			
	fileNamePtr = (char*)GetFileNameCPointerFromFileHandle (fileInfoHandle);
	sprintf ((char*)gTextString2,
				"    File compared with = '%s'%s",
				fileNamePtr,
				gEndOfLine);
	continueFlag = ListString ((char*)gTextString2,
										(UInt32)strlen ((char*)gTextString2),
										gOutputTextH);

	continueFlag = ListLineColumnIntervalString (NULL, 
																gOutputForce1Code,
																reformatOptionsPtr->lineStart, 
																reformatOptionsPtr->lineEnd, 
																reformatOptionsPtr->lineInterval,
																reformatOptionsPtr->columnStart, 
																reformatOptionsPtr->columnEnd, 
																reformatOptionsPtr->columnInterval, 
								 								continueFlag);
	
	snprintf ((char*)gTextString,
				256,
				"%s"
				"    Compare images options:%s",
				gEndOfLine,
				gEndOfLine);
			
	if (continueFlag)
		continueFlag = ListString ((char*)gTextString,
											(UInt32)strlen ((char*)gTextString),
											gOutputTextH);
				
				// List the channels to be compared if the active image is
				// not a thematic image file
	
	if (continueFlag && !outFileInfoPtr->thematicType)
		{
		continueFlag = ListChannelsUsed (
				gImageFileInfoPtr,
				NULL,
				reformatOptionsPtr->channelPtr,
				(SInt16)reformatOptionsPtr->numberChannels,
				NULL,
				&gOutputForce1Code,
				kNoTransformation);
				
		}	// end "if (continueFlag && !outFileInfoPtr->thematicType)"
												
			// List "  Output Information:"
			
	continueFlag = ListSpecifiedStringNumber (kSharedStrID, 
																IDS_Shared8, 
																(unsigned char*)gTextString, 
																NULL, 
																gOutputForce1Code, 
																continueFlag);
		
			// Insert a blank line
	
	//if (continueFlag)
	//	continueFlag = ListString ((char*)&gEndOfLine,
	//										(UInt32)strlen ((char*)&gEndOfLine),
	//										gOutputTextH);

	if (numberDifferences == 0)
		snprintf ((char*)gTextString,
							256,
							"    The image files are the same.%s",
							gEndOfLine);
							
	else	// numberDifferences != 0
		snprintf ((char*)gTextString,
							256,
							"    The image files are not the same. The first difference is at:%s",
							gEndOfLine);
							
	if (continueFlag)
		continueFlag = ListString ((char*)gTextString,
											(UInt32)strlen ((char*)gTextString),
											gOutputTextH,
											kUTF8CharString);
											
	if (numberDifferences > 0)
		{
		snprintf ((char*)gTextString,
								256,
								"      Line -   %d%s"
								"      Column -   %d%s"
								"      Channel -   %d%s"
								"      Data Value -   %f   %f%s%s",
								firstDifferentLine,
								gEndOfLine,
								firstDifferentColumn,
								gEndOfLine,
								firstDifferentChannel,
								gEndOfLine,
								activeImageValue,
								targetImageValue,
								gEndOfLine,
								gEndOfLine);
														
		if (continueFlag)
			continueFlag = ListString ((char*)gTextString,
												(UInt32)strlen ((char*)gTextString),
												gOutputTextH,
												kUTF8CharString);
												
		snprintf ((char*)gTextString,
								256,
								"      Minimum difference: %f%s"
								"      Maximum difference: %f%s",
								minimumDifference,
								gEndOfLine,
								maximumDifference,
								gEndOfLine);
														
		if (continueFlag)
			continueFlag = ListString ((char*)gTextString,
												(UInt32)strlen ((char*)gTextString),
												gOutputTextH,
												kUTF8CharString);
		
		}	// end "if (numberDifferences > 0)"
		
	CreateNumberWithCommasInString ((char*)numberStringWithCommas,
												numberComparisons);
							
	stringLength = snprintf ((char*)gTextString,
									256,
									"%s    Number comparisons: %s%s",
									gEndOfLine,
									numberStringWithCommas,
									gEndOfLine);
									
	if (numberDifferences > 0)
		{
		CreateNumberWithCommasInString ((char*)numberStringWithCommas,
													numberDifferences);
																			
		sprintf ((char*)&gTextString[stringLength],
						"    Number differences: %s%s",
						numberStringWithCommas,
						gEndOfLine);
						
		}	// end "if (numberDifferences > 0)"
						
	if (continueFlag)
		continueFlag = ListString ((char*)gTextString,
											(UInt32)strlen ((char*)gTextString),
											gOutputTextH,
											kUTF8CharString);
		
			// Insert a blank line
	
	if (continueFlag)
		continueFlag = ListString ((char*)&gEndOfLine,
											(UInt32)strlen ((char*)&gEndOfLine),
											gOutputTextH);
											
	if (compareImagesOptionsPtr->createOutputImageFileFlag)
		{
		fileNamePtr = (char*)GetFileNameCPointerFromFileInfo (outFileInfoPtr);
		snprintf ((char*)gTextString,
						256,
						"    New output image file name: '%s'%s%s",
						fileNamePtr,
						gEndOfLine,
						gEndOfLine);
				
		if (continueFlag)
			continueFlag = ListString ((char*)gTextString,
											(UInt32)strlen ((char*)gTextString),
											gOutputTextH,
											kUTF8CharString);
																	
		stringLength = snprintf ((char*)gTextString,
					256,
					"    Compare image algorithm: ");
					
		if (compareImagesOptionsPtr->compareMethod == kActiveMinusCompare)
			snprintf ((char*)&gTextString[stringLength],
									256-stringLength,
									"Active Image File minus Compared Image File%s%s",
									gEndOfLine,
									gEndOfLine);
					
		else if (compareImagesOptionsPtr->compareMethod == kCompareMinusActive)
			snprintf ((char*)&gTextString[stringLength],
									256-stringLength,
									"Compared Image File minus Active Image File%s%s",
									gEndOfLine,
									gEndOfLine);
				
		if (continueFlag)
			continueFlag = ListString ((char*)gTextString,
												(UInt32)strlen ((char*)gTextString),
												gOutputTextH,
												kUTF8CharString);
																
		}	// end "if (compareImagesOptionsPtr->createOutputImageFileFlag)"
	
	return (continueFlag);
	
}	// end "ListCompareImagesResultsInformation"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetUpCompareMethodPopupMenu
//
//	Software purpose:	The purpose of this routine is to set up the resample
//							method popup menu.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 05/05/2022
//	Revised By:			Larry L. Biehl			Date: 09/01/2022

void SetUpCompareMethodPopupMenu (
				DialogPtr							dialogPtr,
				MenuHandle							popUpCompareMethodMenu,
				Boolean								thematicTypeFlag,
				SInt16								compareMethod)

{	
	#if defined multispec_mac
		if (popUpResampleSelectionMenu != NULL)
			{
					// The sum method has not been initiated yet.
					
			//DeleteMenuItem (popUpCompareMethodMenu, 2);
			//DisableMenuItem (popUpCompareMethodMenu, kCompareMinusActive);
		
			}	// end "if (popUpResampleSelectionMenu != NULL)"
	#endif	// defined multispec_mac
					
	#if defined multispec_win
		CComboBox* comboBoxPtr =
								(CComboBox*)(dialogPtr->GetDlgItem (IDC_CompareAlgorithm));

		comboBoxPtr->SetItemData (0, compareMethod);
	#endif	// defined multispec_win

	#if defined multispec_wx
		wxChoice* compareAlgorithmCtrl =
								(wxChoice*)wxWindow::FindWindowById (IDC_CompareAlgorithm);
		
		compareAlgorithmCtrl->Append ("Active - Compare");
      int m_kActiveMinusCompare = kActiveMinusCompare;
		compareAlgorithmCtrl->SetClientData (0, (void*)(SInt64)m_kActiveMinusCompare);
		
		compareAlgorithmCtrl->Append ("Compare - Active");
      int m_kCompareMinusActive = kCompareMinusActive;
		compareAlgorithmCtrl->SetClientData (1, (void*)(SInt64)m_kCompareMinusActive);
   #endif	// defined multispec_wx
	
}	// end "SetUpCompareMethodPopupMenu" 

 
