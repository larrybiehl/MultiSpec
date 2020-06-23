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
//	File:						SDisplayMultispectral.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			05/31/2020
//
//	Language:				C
//
//	System:					Linux, Macintosh and Windows Operating Systems
//
//	Brief description:	This file contains routines to handle displaying multispectral
//								image windows. Some routines which write into the offscreen
//								buffers are in other files.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include "SDisplay_class.h"

#if defined multispec_wx
	#include "SImageWindow_class.h"
	#include "xDisplayMultispectralDialog.h"  
   #include "xGaussianParameterDialog.h"        
	#include "xImageFrame.h"
	#include "xImageView.h"  
	#include "xMultiSpec.h"
#endif // defined multispec_wx  

#if defined multispec_mac || defined multispec_mac_swift
	#define ID3C_LineStart            8
	#define ID3C_Magnification			17
	#define ID3C_NumberOfLevels		27
	#define ID3C_Channels            29
	#define ID3C_RedPrompt 				30
	#define ID3C_GreenPrompt 			31
	#define ID3C_BluePrompt 			32
	#define ID3C_GrayChannel			33
	#define ID3C_RedChannel 			34
	#define ID3C_GreenChannel 			35
	#define ID3C_BlueChannel 			36
	#define ID3C_GrayPrompt				37
	#define ID3C_ChannelDescriptions 38
	#define ID3C_NewHistogram			39
	#define IDC_RedChannelInvert		40
	#define IDC_GreenChannelInvert	41
	#define IDC_BlueChannelInvert		42
	#define IDC_LegendFactorPrompt	45
	#define IDC_LegendFactor			46
	#define IDC_VectorOverlays			47

	#define IDC_TableTitle				 4	
	#define IDC_RedChannelPrompt		 5
	#define IDC_RedChannel				 6
	#define IDC_RedChannelMin			 7
	#define IDC_RedChannelMax			 8
	#define IDC_RedChannelMinE			 9 
	#define IDC_RedChannelMaxE	  		10
	#define IDC_GreenChannelPrompt	11
	#define IDC_GreenChannel			12 
	#define IDC_GreenChannelMin		13 
	#define IDC_GreenChannelMax		14 
	#define IDC_GreenChannelMinE		15 
	#define IDC_GreenChannelMaxE	  	16
	#define IDC_BlueChannelPrompt		17
	#define IDC_BlueChannel			   18 
	#define IDC_BlueChannelMin			19 
	#define IDC_BlueChannelMax			20 
	#define IDC_BlueChannelMinE		21 
	#define IDC_BlueChannelMaxE	  	22
	#define IDC_PerChannel				23
	#define IDC_EntireRange				24
	#define IDC_UserSpecified			25 
	#define IDC_ThematicDefault		26
	#define IDC_PercentClipped			27

	#define IDC_NotCalculatedMessage 28
	#define IDC_NotCalculatedMessage2 29

	#define	IDS_Dialog30				30
	#define	kGaussianParameterID		156
#endif	// defined multispec_mac || defined multispec_mac_swift

#if defined multispec_win
	#include "WImageView.h"                                                           
	#include "WDisplayMultispectralDialog.h" 
	#include "WDisplayMinMaxDialog.h" 
	#include "WGaussianParameterDialog.h"	
#endif	// defined multispec_win



SInt16 gBitsOfColorSelection;
SInt16 gDisplayTypeMenuSelection;
SInt16 gEnhanceStretchSelection;
SInt16 gEnhanceMinMaxMenuSelection;
SInt16 gEnhanceZeroAsSelection;



		// Prototypes for routines in this file that are only called by
		// other routines in this file.										

Boolean DisplayMultispectralDialog (
				DisplaySpecsPtr					displaySpecsPtr);

PascalVoid DrawBitsColorPopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber);

PascalVoid DrawDisplayTypePopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber);

PascalVoid DrawMinMaxPopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber);

PascalVoid DrawStretchPopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber);

PascalVoid DrawZeroAsPopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber);

SInt16 EnhanceMinMaxPopUpMenu (
				DialogPtr							dialogPtr,
				SInt16								dialogItem,
				SInt16								rgbColors,
				SInt16								displayType,
				SInt16								numberLevels,
				Handle								okHandle,
				SInt16								minMaxSelection,
				double*								minMaxValuesPtr,
				SInt16*								percentTailsClippedPtr);

void Display1Channel8BitLine (
				SInt16								displayCode,
				UInt32								numberSamples,
				UInt32								interval,
				FileInfoPtr							fileInfoPtr,
				HUCharPtr							ioBufferPtr,
				HUCharPtr							dataDisplayPtr,
				UInt32								maxValue,
				HUCharPtr							offScreenPtr);

void Display2Channel8BitLine (
				UInt32								numberSamples,
				UInt32								interval,
				Boolean								bytesEqualOneFlag1,
				Boolean								bytesEqualOneFlag2,
				UInt16								backgroundValueCode,
				HFileIOBufferPtr					ioBuffer1Ptr,
				HFileIOBufferPtr					ioBuffer2Ptr,
				HUCharPtr							dataDisplay1Ptr,
				HUCharPtr							dataDisplay2Ptr,
				UInt32								maxBin1,
				UInt32								maxBin2,
				HUCharPtr							offScreenPtr);

void Display3Channel8BitLine (
				UInt32								numberSamples,
				UInt32								interval,
				Boolean								bytesEqualOneFlag1,
				Boolean								bytesEqualOneFlag2,
				Boolean								bytesEqualOneFlag3,
				UInt16								backgroundValueCode,
				HFileIOBufferPtr					ioBuffer1Ptr,
				HFileIOBufferPtr					ioBuffer2Ptr,
				HFileIOBufferPtr					ioBuffer3Ptr,
				HUCharPtr							dataDisplay1Ptr,
				HUCharPtr							dataDisplay2Ptr,
				HUCharPtr							dataDisplay3Ptr,
				UInt32								maxBin1,
				UInt32								maxBin2,
				UInt32								maxBin3,
				HUCharPtr							offScreenPtr);

void DisplayMultispectralDialogSetDefaultSelection (
				DialogPtr							dialogPtr,
				SInt16								rgbColors,
				SInt16								displayType);

Boolean EqualAreaDataToDisplayLevels (
				HistogramSpecsPtr					histogramSpecsPtr,
				HUCharPtr							dataDisplayPtr,
				DisplaySpecsPtr					displaySpecsPtr,
				UInt32								dataDisplayVectorLength,
				SInt16								numberChannels,
				UInt16*								channelsPtr);

Boolean FillDataToDisplayLevels (
				HistogramSpecsPtr					histogramSpecsPtr,
				HUCharPtr							dataDisplayPtr,
				DisplaySpecsPtr					displaySpecsPtr,
				UInt32								dataDisplayVectorLength,
				SInt16								numberChannels,
				UInt16*								channelsPtr);

Boolean GaussianToDisplayLevels (
				HistogramSpecsPtr					histogramSpecsPtr,
				HUCharPtr							dataDisplayPtr,
				DisplaySpecsPtr					displaySpecsPtr,
				UInt32								dataDisplayVectorLength,
				SInt16								numberChannels,
				UInt16*								channelsPtr);

Boolean GetHistogramRequiredFlag (
				SInt16								displayType,
				SInt16								enhanceStretchSelection,
				SInt16								lMinMaxCode);

void GetMinMaxValuesIndices (
				DisplaySpecsPtr					displaySpecsPtr,
				HistogramSpecsPtr					histogramSpecsPtr,
				HistogramSummaryPtr				histogramSummaryPtr,
				SInt16								channel,
				SInt16								channelNumberIndex,
				Boolean								histLoadedFlag,
				SInt32								signedValueOffset,
				UInt32								numberBins,
				UInt16								numberClasses,
				SInt32*								minBinIndexPtr,
				SInt32*								maxBinIndexPtr,
				SInt16*								classForMinDataValuePtr,
				SInt16*								classForMaxDataValuePtr);

SInt16 GetThematicClassForDataValue (
				double								dataValue,
				double								minThematicRange,
				double								maxThematicRange,
				SInt16								numberThematicClasses);

void GetThematicTypeMinMaxIndices (
				SInt16								numberClassBins,
				SInt16								channelNumber,
				double*								minThematicValuePtr,
				double*								maxThematicValuePtr,
				SInt32*								minValueIndexPtr,
				SInt32*								maxValueIndexPtr);

Boolean HistogramVector (
				DisplaySpecsPtr					displaySpecsPtr,
				HistogramSpecsPtr					histogramSpecsPtr);

DisplaySpecsPtr LoadMultispectralDisplaySpecs (void);

void SaveDisplayStructureSettings (
				DisplaySpecsPtr					displaySpecsPtr,
				FileInfoPtr							imageFileInfoPtr,
				WindowInfoPtr						imageWindowInfoPtr);

void SetUpMinMaxPopUpMenu (
				DialogPtr							dialogPtr,
				SInt16								displayType);

void UpdatePopUpDisplayTypeMenu (
				UInt16								totalNumberChannels,
				SInt16								maxSystemPixelSize);

void UpdateMinMaxValueIndices (
				DisplaySpecsPtr					displaySpecsPtr,
				HistogramSummaryPtr				histogramSummaryPtr,
				SInt16								numberChannels,
				UInt16*								channelsPtr,
				SInt32								signedValueOffset);

void UpdateThematicTypeMinMaxes (
				SInt16								numberClassBins,
				SInt16								channelNumber,
				double*								minBinIndexPtr,
				double*								maxBinIndexPtr);



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DisplayImagesSideBySide
//
//	Software purpose:	The purpose of this routine is to display selected
//							channels of an image side by side in color in a window 
//							on the screen.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None			
// 
// Called By:			DisplayColorImage in SDisplay.cpp
//
//	Coded By:			Larry L. Biehl			Date: 06/26/1990
//	Revised By:			Larry L. Biehl			Date: 04/24/2019

void DisplayImagesSideBySide (
				DisplaySpecsPtr					displaySpecsPtr,
				FileInfoPtr							fileInfoPtr,
				HPtr									offScreenBufferPtr,
				UInt32								pixRowBytes,
				PixMapHandle						savedPortPixMapH,
				PixMapHandle						offScreenPixMapH,
				LongRect*							rectPtr,
				LCToWindowUnitsVariables* 		lcToWindowUnitsVariablesPtr)

{
	LongRect								longSourceRect;

	#if defined multispec_mac
		Rect									sourceRect;
	#endif	// defined multispec_mac

	FileInfoPtr							localFileInfoPtr;
	FileIOInstructionsPtr			fileIOInstructionsPtr;

	HUCharPtr							base_ioBufferPtr,
											buffer1Ptr,
											dataDisplayPtr,
											dataToLevelPtr,
											ioBufferPtr,
											offScreenLinePtr,
											offScreenPtr,
											savedOffScreenLinePtr;

	HUInt16Ptr							buffer2Ptr;

	UInt16*								channelsPtr;
	
	int									nextStatusAtLeastLine,
											nextStatusAtLeastLineIncrement;

	SInt32								displayBottomMax;

	UInt32								bytesOffset,
											columnEnd,
											columnIntervalUsed,
											columnStart,
											dataValue,
											firstColumnIndex,
											interval,
											line,
											lineCount,
											lineEnd,
											lineInterval,
											lineStart,
											j,
											maxValue,
											numberSamples;
	
	#if defined multispec_wx
		UInt32								lineBytesOffset = 0;
		UInt32								savedLineBytesOffset = 0;
	#endif

	SInt16								channel,
											channelNumber,
											errCode,
											imageFileNumberChannels;

	UInt16								channelsIndex,
											fileInfoIndex,
											numberChannels;

	Boolean								BILSpecialFlag,
											BISFlag,
											packDataFlag,
											stopFlag;

	SInt8									//addressMode,
											separatorByte;


			// Initialize local variables.

	firstColumnIndex = displaySpecsPtr->columnStart - 1;
	columnStart = displaySpecsPtr->columnStart;
	columnEnd = displaySpecsPtr->columnEnd;
	lineStart = displaySpecsPtr->lineStart;
	lineEnd = displaySpecsPtr->lineEnd;
	lineInterval = displaySpecsPtr->lineInterval;
	numberChannels = displaySpecsPtr->numberChannels;
	maxValue = gImageWindowInfoPtr->numberBins - 1;
	channelsIndex = 0;
	stopFlag = FALSE;
	fileIOInstructionsPtr = NULL;

	separatorByte = (char)0xFF;

	#if defined multispec_wx
		separatorByte = (char)0xFF;
	#endif

			// Lock channel list handle and get a pointer.
			// This handle is already locked.

	channelsPtr = (UInt16*)GetHandlePointer (displaySpecsPtr->channelsHandle);

	#if defined multispec_mac
		GrafPtr savedPort;

				// Force any updates to be done.

		if (!CheckSomeDisplayEvents (gImageWindowInfoPtr,
												displaySpecsPtr,
												lcToWindowUnitsVariablesPtr,
												savedPortPixMapH,
												offScreenPixMapH,
												&longSourceRect,
												-1))
																									return;

			// Force the title line to be drawn.

		PixMapHandle imageWindowPortPixMap;

		if (!gOSXCoreGraphicsFlag)
			{
			imageWindowPortPixMap = GetPortPixMap (GetWindowPort (gActiveImageWindow));
			CopyHandleToHandle ((Handle)savedPortPixMapH, (Handle)imageWindowPortPixMap);

			}	// end "if (!gOSXCoreGraphicsFlag)"

		GetWindowClipRectangle (gActiveImageWindow, kImageFrameArea, &sourceRect);
		sourceRect.top = gImageWindowInfoPtr->coordinateHeight;
		sourceRect.bottom = gImageWindowInfoPtr->imageTopOffset;
		GetPort (&savedPort);
		SetPortWindowPort (gActiveImageWindow);
		InvalWindowRect (gActiveImageWindow, &sourceRect);
		SetPort (savedPort);

		if (!gOSXCoreGraphicsFlag)
			CopyHandleToHandle ((Handle)offScreenPixMapH, (Handle)imageWindowPortPixMap);
	#endif	// defined multispec_mac

			// Get display bottom maximum of image window

	longSourceRect = *rectPtr;

	displayBottomMax = longSourceRect.bottom;

			// Get handle to block of memory to use as file IO buffer for the
			// requested channels of data.  If the input band interleave format
			// is BIS,  then request memory for all channels in the file.

	if (!GetIOBufferPointers (&gFileIOInstructions[0],
										gImageWindowInfoPtr,
										gImageLayerInfoPtr,
										fileInfoPtr,
										&base_ioBufferPtr,
										&base_ioBufferPtr,
										columnStart,
										columnEnd,
										1,
										numberChannels,
										channelsPtr,
										kDoNotPackData,
										kDoNotForceBISFormat,
										kDoNotForceBytes,
										gHasThreadManager,
										&fileIOInstructionsPtr))
																										return;

	dataDisplayPtr = (HUCharPtr)GetHandlePointer (
										gToDisplayLevels.vectorHandle, kLock);

			// Make certain that we have valid 32 bit pointers in case we go to
			// 32 bit mode.

			// Get the offset in the vector for each channel of data.  The
			// offset will be 0 if the 'use overall min-max or entire data
			// range is used'

	bytesOffset = gToDisplayLevels.bytesOffset;
	if (displaySpecsPtr->enhancementCode == kLinearStretch &&
												displaySpecsPtr->minMaxCode == kEntireDataRange)
		bytesOffset = 0;

			// Draw the image in the image window. Note that Windows DIB
			// images are loaded with first line at the end of the bitmap
			// to last line at the beginning of the bitmap.

	#if defined multispec_mac || defined multispec_wx
		savedOffScreenLinePtr = (HUCharPtr)offScreenBufferPtr;
	#endif	// defined multispec_mac || defined multispec_wx

	#if defined multispec_win
		UInt32 numberLines = (lineEnd - lineStart + lineInterval) / lineInterval;
		savedOffScreenLinePtr = 
						(HUCharPtr)(offScreenBufferPtr + (numberLines - 1) * pixRowBytes);
	#endif	// defined multispec_win

			// Intialize the nextTime variable to indicate when the next check
			// should occur for a command-.

	gNextTime = TickCount () + kDisplayTimeOffset;
		
			// These variables are to make sure the display window is not being updated
			// after a very few lines are loaded in. It will override the time interval
			// which is currently every 1 second.
	
	nextStatusAtLeastLineIncrement =
								(int)((10 * lineInterval) / displaySpecsPtr->magnification);
	nextStatusAtLeastLineIncrement = MAX (nextStatusAtLeastLineIncrement, 10);
	nextStatusAtLeastLine = displaySpecsPtr->lineStart + nextStatusAtLeastLineIncrement;

			// Loop through the image files.

	for (fileInfoIndex = 0;
			fileInfoIndex < gImageWindowInfoPtr->numberImageFiles;
			fileInfoIndex++)
		{
		longSourceRect.top = rectPtr->top;
		
		displaySpecsPtr->drawSideBySideTitleFlag = TRUE;

				// Get the number of channels used in this image file.

		for (imageFileNumberChannels = 0;
				 channelsIndex < numberChannels;
				 channelsIndex++)
			{
			channelNumber = channelsPtr[channelsIndex] + 1;

			if (fileInfoIndex != gImageLayerInfoPtr[channelNumber].fileInfoIndex)
				 break;

			imageFileNumberChannels++;

			}	// end "for (imageFileNumberChannels = 0; ..."

		if (imageFileNumberChannels > 0)
			{
					// Now load the side by side images for this file.

			localFileInfoPtr = &fileInfoPtr[fileInfoIndex];

					// Set flag to indicate whether entire line of data for all
					// channels can be read in at one time, rather than separate
					// reads for each channel.

			BILSpecialFlag = GetBILSpecial (
										  fileIOInstructionsPtr,
										  &channelsPtr[channelsIndex - imageFileNumberChannels],
										  imageFileNumberChannels,
										  columnStart,
										  columnEnd,
										  displaySpecsPtr->columnInterval,
										  kDetermineSpecialBILFlag);

			if (localFileInfoPtr->bandInterleave == kBIS)
				{
				interval = imageFileNumberChannels;
				numberSamples = ((UInt32)displaySpecsPtr->columnEnd -
						displaySpecsPtr->columnStart + displaySpecsPtr->columnInterval) /
																		displaySpecsPtr->columnInterval;
				numberSamples *= imageFileNumberChannels;
				columnIntervalUsed = displaySpecsPtr->columnInterval;
				packDataFlag = TRUE;
				BISFlag = TRUE;

				}	// end "if (localFileInfoPtr->bandInterleave == kBIS)"

			else	// localFileInfoPtr->bandInterleave != kBIS
            {
				if (BILSpecialFlag)
					{
					interval = 1;
					numberSamples = ((UInt32)displaySpecsPtr->columnEnd -
							 displaySpecsPtr->columnStart + displaySpecsPtr->columnInterval) /
										displaySpecsPtr->columnInterval;
					columnIntervalUsed = displaySpecsPtr->columnInterval;
					packDataFlag = TRUE;

					}	// end "if (BILSpecialFlag)"

				else	// !BILSpecialFlag
					{
					interval = displaySpecsPtr->columnInterval;
					numberSamples = (UInt32)displaySpecsPtr->columnEnd -
																	displaySpecsPtr->columnStart + 1;
					columnIntervalUsed = 1;
					packDataFlag = FALSE;

					}	// end "else !BILSpecialFlag"

				BISFlag = FALSE;

            }	// end "else localFileInfoPtr->bandInterleave != kBIS"

			offScreenLinePtr = savedOffScreenLinePtr;

			lineCount = 0;

					// Set some of the File IO Instructions parameters.

			fileIOInstructionsPtr->packDataFlag = packDataFlag;

			errCode = SetUpFileIOInstructions (
						  fileIOInstructionsPtr,
						  NULL,
						  displaySpecsPtr->lineStart,
						  lineEnd,
						  lineInterval,
						  columnStart,
						  columnEnd,
						  columnIntervalUsed,
						  imageFileNumberChannels,
						  (HUInt16Ptr)&channelsPtr[channelsIndex - imageFileNumberChannels],
						  kDetermineSpecialBILFlag);

			ioBufferPtr = base_ioBufferPtr;

			for (line = lineStart; line <= lineEnd; line += lineInterval)
				{
						// Get all channels for the line of image data.  Return if
						// there is a file IO error.

				errCode = GetLineOfData (fileIOInstructionsPtr,
													line,
													columnStart,
													columnEnd,
													columnIntervalUsed,
													ioBufferPtr,
													ioBufferPtr);

				if (errCode != noErr)
					break;

				else	// errCode == noErr
					{
					offScreenPtr = offScreenLinePtr;

							//	This loop will draw the image lines for one byte data.

					if (localFileInfoPtr->numberBytes == 1)
						{
						dataToLevelPtr = dataDisplayPtr;

						for (channel=0; channel<imageFileNumberChannels; channel++)
							{
							if (BISFlag)
								buffer1Ptr = (HUCharPtr)&ioBufferPtr[channel];

							else	// !BISFlag
								buffer1Ptr = (HUCharPtr)&ioBufferPtr[channel * numberSamples];

							for (j = 0; j < numberSamples; j += interval)
								{
								#if defined multispec_wxmac_alpha
											// Skip first (alpha) byte in wxBitmap
									offScreenPtr++;
								#endif
							
								dataValue = *buffer1Ptr;
								*offScreenPtr = (SInt8)(dataToLevelPtr[dataValue]);

								offScreenPtr++;
								#if defined multispec_wx
									*offScreenPtr = (SInt8)(dataToLevelPtr[dataValue]);

									offScreenPtr++;
									*offScreenPtr = (SInt8)(dataToLevelPtr[dataValue]);

								  offScreenPtr++;
								
									#if defined multispec_wxlin_alpha
												// Skip last (alpha) byte in wxBitmap
										offScreenPtr++;
									#endif
								#endif	// defined multispec_wx
								buffer1Ptr += interval;

								}	// end "for (j=0;..."
							
							#if defined multispec_wxmac_alpha
										// Skip first (alpha) byte in wxBitmap
								offScreenPtr++;
							#endif

							*offScreenPtr = separatorByte;
							offScreenPtr++;
							#if defined multispec_wx
								*offScreenPtr = separatorByte;

								offScreenPtr++;
								*offScreenPtr = separatorByte;
								
								#if defined multispec_wxlin_alpha
											// Skip last (alpha) byte in wxBitmap
									offScreenPtr++;
								#endif
							
								#if defined multispec_wxmac_alpha
											// Skip first (alpha) byte in wxBitmap
									offScreenPtr++;
								#endif

								offScreenPtr++;
								*offScreenPtr = separatorByte;

								offScreenPtr++;
								*offScreenPtr = separatorByte;

								offScreenPtr++;
							#endif	// defined multispec_wx
							
							*offScreenPtr = separatorByte;
							offScreenPtr++;
								
							#if defined multispec_wxlin_alpha
										// Skip last (alpha) byte in wxBitmap
								offScreenPtr++;
							#endif

							dataToLevelPtr += bytesOffset;

							}	// end "for (channel=0; channel<..."

						}	// end "if (localFileInfoPtr->numberBytes == 1)"

							// This loop will draw the image lines for two byte data

					else if (localFileInfoPtr->numberBytes == 2)
						{
						dataToLevelPtr = dataDisplayPtr;

						for (channel=0; channel<imageFileNumberChannels; channel++)
							{
							if (BISFlag)
								buffer2Ptr = (HUInt16Ptr)&ioBufferPtr[2*channel];

							else	// !BISFlag
								buffer2Ptr =
										(HUInt16Ptr)&ioBufferPtr[2*channel*numberSamples];

							for (j=0; j<numberSamples; j+=interval)
								{
								#if defined multispec_wxmac_alpha
											// Skip first (alpha) byte in wxBitmap
									offScreenPtr++;
								#endif
							
								dataValue = (UInt32)*buffer2Ptr;
								if (dataValue > maxValue)
									dataValue = 0;
								*offScreenPtr = (SInt8)(dataToLevelPtr[dataValue]);

								offScreenPtr++;
								#if defined multispec_wx
									*offScreenPtr = (SInt8)(dataToLevelPtr[dataValue]);

									offScreenPtr++;
									*offScreenPtr = (SInt8)(dataToLevelPtr[dataValue]);

								  	offScreenPtr++;
								
									#if defined multispec_wxlin_alpha
												// Skip last (alpha) byte in wxBitmap
										offScreenPtr++;
									#endif
								#endif
								
								buffer2Ptr += interval;

								}	// end "for (j=0;..."
							
							#if defined multispec_wxmac_alpha
										// Skip first (alpha) byte in wxBitmap
								offScreenPtr++;
							#endif

							*offScreenPtr = separatorByte;
							offScreenPtr++;
							#if defined multispec_wx
								*offScreenPtr = separatorByte;
							
								offScreenPtr++;
								*offScreenPtr = separatorByte;
							
								#if defined multispec_lin_alpha
											// Skip last (alpha) byte in wxBitmap
									offScreenPtr++;
								#endif
							
								#if defined multispec_wxmac_alpha
											// Skip first (alpha) byte in wxBitmap
									offScreenPtr++;
								#endif

								offScreenPtr++;
								*offScreenPtr = separatorByte;

								offScreenPtr++;
								*offScreenPtr = separatorByte;

								offScreenPtr++;
							#endif	// defined multispec_wx
							
							*offScreenPtr = separatorByte;
							offScreenPtr++;
							
							#if defined multispec_wxlin_alpha
										// Skip last (alpha) byte in wxBitmap
								offScreenPtr++;
							#endif

							dataToLevelPtr += bytesOffset;

							}	// end "for (channel=0; channel<=..."

						}	// end "else if (localFileInfoPtr->numberBytes == 2)"

							// Copy a portion of the image and
							// check if user wants to exit drawing								

					lineCount++;
					if (TickCount () >= gNextTime && lineCount >= nextStatusAtLeastLine)
						{
						#if defined multispec_wx
							displaySpecsPtr->updateEndLine = lineCount;
						#endif
					
						longSourceRect.bottom = lineCount;
						if (!CheckSomeDisplayEvents (gImageWindowInfoPtr,
																displaySpecsPtr,
																lcToWindowUnitsVariablesPtr,
																savedPortPixMapH,
																offScreenPixMapH,
																&longSourceRect,
																displayBottomMax))
							{
							stopFlag = TRUE;
							break;

							}	// end "if (!CheckSomeEvents (osMask..."
						
						#if defined multispec_wx
							displaySpecsPtr->updateStartLine = lineCount;

									// Get the bitmap raw data pointer again. It may have
									// changed.

							offScreenLinePtr =
										(unsigned char*)gImageWindowInfoPtr->imageBaseAddressH;
							offScreenLinePtr +=
										lineBytesOffset + (SInt64)(lineCount-1) * pixRowBytes;
						#endif
					
						nextStatusAtLeastLine = lineCount + nextStatusAtLeastLineIncrement;
						nextStatusAtLeastLine = MIN (nextStatusAtLeastLine, lineEnd);
						
						displaySpecsPtr->drawSideBySideTitleFlag = FALSE;

						}	// end "if (TickCount () >= gNextTime)"

					if (line == lineStart)
						{
						#if defined multispec_wx
									// Also get the number bytes offset in case needed for
									// wx version
						
							savedLineBytesOffset += (offScreenPtr - savedOffScreenLinePtr);
						#endif
						
						savedOffScreenLinePtr = offScreenPtr;
						
						}	// end "if (line == lineStart)"

					#if defined multispec_mac || defined multispec_wx
						offScreenLinePtr += pixRowBytes;
					#endif	// defined multispec_mac || defined multispec_wx

					#if defined multispec_win
						offScreenLinePtr -= pixRowBytes;
					#endif	// defined multispec_win

					}	// end "if (errCode == noErr)"

				if (gUseThreadedIOFlag)
					{
							// Switch buffers.

					ioBufferPtr = fileIOInstructionsPtr->inputBufferPtrs[
															fileIOInstructionsPtr->bufferUsedForIO];

					}	// end "if (gUseThreadedIOFlag)"

				}	// end "for (line=lineStart; ..."

					// Force last lines in image window for image file to be updated.

			longSourceRect.bottom = lineCount;
			
			#if defined multispec_wx
				displaySpecsPtr->updateEndLine = lineCount;
			#endif
			
			if (!CheckSomeDisplayEvents (gImageWindowInfoPtr,
													displaySpecsPtr,
													lcToWindowUnitsVariablesPtr,
													savedPortPixMapH,
													offScreenPixMapH,
													&longSourceRect,
													displayBottomMax))
				{
				stopFlag = TRUE;
				break;

				}	// end "if (!CheckSomeEvents (osMask..."
			
			#if defined multispec_wx
				displaySpecsPtr->updateStartLine = lineCount;
			#endif

			dataDisplayPtr += bytesOffset*imageFileNumberChannels;

			CloseUpFileIOInstructions (fileIOInstructionsPtr, NULL);

			}	// end "if (imageFileNumberChannels > 0)"

		if (stopFlag)
			break;
		
		#if defined multispec_wx
			lineBytesOffset = savedLineBytesOffset;

			displaySpecsPtr->updateStartLine = lineCount;
		
					// Get the bitmap raw data pointer again. It may have changed.
					// Also need to adjust for the column offset.

			savedOffScreenLinePtr = lineBytesOffset +
								(unsigned char*)gImageWindowInfoPtr->imageBaseAddressH;
		#endif
		
		displaySpecsPtr->drawSideBySideTitleFlag = TRUE;

		}	// end "for (imageFile=1; ..."

			// Set up return for inSourceRect to indicate if last few lines need
			// to be drawn

	rectPtr->top = longSourceRect.top;
	rectPtr->bottom = longSourceRect.bottom;

	CheckSizeAndUnlockHandle (gToDisplayLevels.vectorHandle);

	DisposeIOBufferPointers (fileIOInstructionsPtr,
										&base_ioBufferPtr,
										&base_ioBufferPtr);

}	// end "DisplayImagesSideBySide"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void Display4_8ByteImagesSideBySide
//
//	Software purpose:	The purpose of this routine is to display selected
//							channels of an image side by side in color in a window 
//							on the screen.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None			
// 
// Called By:			DisplayColorImage in SDisplay.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/04/2006
//	Revised By:			Larry L. Biehl			Date: 04/15/2019

void Display4_8ByteImagesSideBySide (
				DisplaySpecsPtr					displaySpecsPtr,
				FileInfoPtr							fileInfoPtr,
				HistogramSummaryPtr				histogramSummaryPtr,
				HPtr									offScreenBufferPtr,
				UInt32								pixRowBytes,
				PixMapHandle						savedPortPixMapH,
				PixMapHandle						offScreenPixMapH,
				LongRect*							rectPtr,
				LCToWindowUnitsVariables* 		lcToWindowUnitsVariablesPtr)

{
	LongRect								longSourceRect;

	#if defined multispec_mac
		Rect									sourceRect;
	#endif	// defined multispec_mac

	FileInfoPtr							localFileInfoPtr;
	FileIOInstructionsPtr			fileIOInstructionsPtr;

	HDoublePtr							buffer8BytePtr,
											ioBufferPtr;

	HUCharPtr							base_ioBufferPtr,
											dataDisplayPtr,
											dataToLevelPtr,
											input_ioBufferPtr,
											offScreenLinePtr,
											offScreenPtr,
											savedOffScreenLinePtr;

	UInt16*								channelsPtr;

	double								binFactor,
											doubleBinIndex,
											minValue;

	SInt32								displayBottomMax;

	UInt32								binIndex,
											bytesOffset,
											chanIndex,
											channelIndexStart,
											columnEnd,
											columnIntervalUsed,
											columnStart,
											firstColumnIndex,
											interval,
											line,
											lineCount,
											lineEnd,
											lineInterval,
											lineStart,
											j,
											maxBin,
											numberSamples;
	
	#if defined multispec_wxlin
		UInt32								lineBytesOffset = 0;
	#endif

	SInt16								channel,
											channelNumber,
											errCode,
											imageFileNumberChannels;

	UInt16								channelsIndex,
											fileInfoIndex,
											numberChannels;

	Boolean								BILSpecialFlag,
											BISFlag,
											packDataFlag,
											stopFlag;

	SInt8									separatorByte;


			// Initialize local variables.

	firstColumnIndex = displaySpecsPtr->columnStart - 1;
	columnStart = displaySpecsPtr->columnStart;
	columnEnd = displaySpecsPtr->columnEnd;
	lineStart = displaySpecsPtr->lineStart;
	lineEnd = displaySpecsPtr->lineEnd;
	lineInterval = displaySpecsPtr->lineInterval;
	numberChannels = displaySpecsPtr->numberChannels;
	channelsIndex = 0;
	stopFlag = FALSE;
	fileIOInstructionsPtr = NULL;

	separatorByte = (char)0xFF;
	#if defined multispec_wx
		separatorByte = (char)0xFF;
	#endif

			// Lock channel list handle and get a pointer.
			// This handle is already locked.	

	channelsPtr = (UInt16*)GetHandlePointer (displaySpecsPtr->channelsHandle);

	#if defined multispec_mac
		GrafPtr savedPort;

				// Force any updates to be done.

		if (!CheckSomeDisplayEvents (gImageWindowInfoPtr,
												displaySpecsPtr,
												lcToWindowUnitsVariablesPtr,
												savedPortPixMapH,
												offScreenPixMapH,
												&longSourceRect,
												-1))
																									return;

				// Force the title line to be drawn.

		PixMapHandle imageWindowPortPixMap;

		if (!gOSXCoreGraphicsFlag)
			{
			imageWindowPortPixMap = GetPortPixMap (GetWindowPort (gActiveImageWindow));
			CopyHandleToHandle ((Handle)savedPortPixMapH, (Handle)imageWindowPortPixMap);

			}	// end "if (!gOSXCoreGraphicsFlag)"

		GetWindowClipRectangle (gActiveImageWindow, kImageFrameArea, &sourceRect);
		sourceRect.top = gImageWindowInfoPtr->coordinateHeight;
		sourceRect.bottom = gImageWindowInfoPtr->imageTopOffset;
		GetPort (&savedPort);
		SetPortWindowPort (gActiveImageWindow);
		InvalWindowRect (gActiveImageWindow, &sourceRect);
		SetPort (savedPort);

		if (!gOSXCoreGraphicsFlag)
			CopyHandleToHandle ((Handle)offScreenPixMapH, (Handle)imageWindowPortPixMap);
	#endif	// defined multispec_mac

			// Get display bottom maximum of image window

	longSourceRect = *rectPtr;

	displayBottomMax = longSourceRect.bottom;

			// Get handle to block of memory to use as file IO buffer for the
			// requested channels of data.  If the input band interleave format 	
			// is BIS,  then request memory for all channels in the file.			

	if (!GetIOBufferPointers (&gFileIOInstructions[0],
										gImageWindowInfoPtr,
										gImageLayerInfoPtr,
										fileInfoPtr,
										&input_ioBufferPtr,
										&base_ioBufferPtr,
										columnStart,
										columnEnd,
										1,
										numberChannels,
										channelsPtr,
										kPackData,
										kDoNotForceBISFormat,
										kForceReal8Bytes,
										gHasThreadManager,
										&fileIOInstructionsPtr))
																									return;

	dataDisplayPtr = (HUCharPtr)GetHandlePointer (
								gToDisplayLevels.vectorHandle, kLock);

			// Get the offset in the vector for each channel of data.  The
			// offset will be 0 if the 'use overall min-max or entire data			
			// range is used'																		

	bytesOffset = gToDisplayLevels.bytesOffset;
	if (displaySpecsPtr->enhancementCode == kLinearStretch &&
            displaySpecsPtr->minMaxCode == kEntireDataRange)
		bytesOffset = 0;

			// Draw the image in the image window. Note that Windows DIB
			// images are loaded with first line at the end of the bitmap 
			// to last line at the beginning of the bitmap.

	#if defined multispec_mac || defined multispec_wx
		savedOffScreenLinePtr = (HUCharPtr)offScreenBufferPtr;
	#endif	// defined multispec_mac || defined multispec_wx

	#if defined multispec_win
		UInt32 numberLines = (lineEnd - lineStart + lineInterval) / lineInterval;
		savedOffScreenLinePtr = 
						(HUCharPtr)(offScreenBufferPtr + (numberLines - 1) * pixRowBytes);
	#endif	// defined multispec_win

			// Intialize the nextTime variable to indicate when the next check
			// should occur for a command-.													

	gNextTime = TickCount () + kDisplayTimeOffset;

			// Loop through the image files.

	for (fileInfoIndex = 0;
            fileInfoIndex < gImageWindowInfoPtr->numberImageFiles;
            fileInfoIndex++)
		{
		longSourceRect.top = rectPtr->top;

				// Get the number of channels used in this image file.

		for (imageFileNumberChannels = 0;
				 channelsIndex < numberChannels;
				 channelsIndex++)
			{
			channelNumber = channelsPtr[channelsIndex] + 1;

			if (fileInfoIndex != gImageLayerInfoPtr[channelNumber].fileInfoIndex)
				break;

			imageFileNumberChannels++;

			}	// end "for (imageFileNumberChannels = 0; ..."

		if (imageFileNumberChannels > 0)
			{
					// Now load the side by side images for this file.

			localFileInfoPtr = &fileInfoPtr[fileInfoIndex];

			channelIndexStart = channelsIndex - imageFileNumberChannels;

					// Set flag to indicate whether entire line of data for all
					// channels can be read in at one time, rather than separate
					// reads for each channel.

			BILSpecialFlag = GetBILSpecial (
										  fileIOInstructionsPtr,
										  &channelsPtr[channelIndexStart],
										  imageFileNumberChannels,
										  columnStart,
										  columnEnd,
										  displaySpecsPtr->columnInterval,
										  kDetermineSpecialBILFlag);

			if (localFileInfoPtr->bandInterleave == kBIS)
				{
				interval = imageFileNumberChannels;
				numberSamples = ((UInt32)displaySpecsPtr->columnEnd -
						displaySpecsPtr->columnStart + displaySpecsPtr->columnInterval) /
																		displaySpecsPtr->columnInterval;
				numberSamples *= imageFileNumberChannels;
				columnIntervalUsed = displaySpecsPtr->columnInterval;
				packDataFlag = TRUE;
				BISFlag = TRUE;

				}	// end "if (localFileInfoPtr->bandInterleave == kBIS)"

			else	// localFileInfoPtr->bandInterleave != kBIS
				{
				if (BILSpecialFlag)
					{
					interval = 1;
					numberSamples = ((UInt32)displaySpecsPtr->columnEnd -
							displaySpecsPtr->columnStart + displaySpecsPtr->columnInterval) /
																	displaySpecsPtr->columnInterval;
					columnIntervalUsed = displaySpecsPtr->columnInterval;
					packDataFlag = TRUE;

					}	// end "if (BILSpecialFlag)"

				else	// !BILSpecialFlag
					{
					interval = displaySpecsPtr->columnInterval;
					numberSamples = (UInt32)displaySpecsPtr->columnEnd -
																	displaySpecsPtr->columnStart + 1;
					columnIntervalUsed = 1;
					packDataFlag = FALSE;

					}	// end "else !BILSpecialFlag"

				BISFlag = FALSE;

            }	// end "else localFileInfoPtr->bandInterleave != kBIS"

			offScreenLinePtr = savedOffScreenLinePtr;

			lineCount = 0;

					// Set some of the File IO Instructions parameters.

			fileIOInstructionsPtr->packDataFlag = packDataFlag;

			errCode = SetUpFileIOInstructions (
						  fileIOInstructionsPtr,
						  NULL,
						  displaySpecsPtr->lineStart,
						  lineEnd,
						  lineInterval,
						  columnStart,
						  columnEnd,
						  columnIntervalUsed,
						  imageFileNumberChannels,
						  (HUInt16Ptr)&channelsPtr[channelsIndex - imageFileNumberChannels],
						  kDetermineSpecialBILFlag);

			ioBufferPtr = (HDoublePtr)base_ioBufferPtr;

			for (line=lineStart; line<=lineEnd; line+=lineInterval)
				{
						// Get all channels for the line of image data.  Return if
						// there is a file IO error.											

				errCode = GetLineOfData (fileIOInstructionsPtr,
													line,
													columnStart,
													columnEnd,
													columnIntervalUsed,
													input_ioBufferPtr,
													(HUCharPtr)ioBufferPtr);

				if (errCode != noErr)
					break;

				else	// errCode == noErr
					{
					offScreenPtr = offScreenLinePtr;

							//	This loop will draw the image lines.

					dataToLevelPtr = dataDisplayPtr;
					chanIndex = channelIndexStart;

					for (channel = 0; channel < imageFileNumberChannels; channel++)
						{
						binFactor = histogramSummaryPtr[channelsPtr[chanIndex]].binFactor;
						minValue =
								histogramSummaryPtr[channelsPtr[chanIndex]].minNonSatValue;
						maxBin = histogramSummaryPtr[channelsPtr[chanIndex]].numberBins - 1;

						if (BISFlag)
							buffer8BytePtr = &ioBufferPtr[channel];

						else	// !BISFlag 
							buffer8BytePtr = &ioBufferPtr[channel * numberSamples];

						for (j = 0; j < numberSamples; j += interval)
							{
							doubleBinIndex = (*buffer8BytePtr - minValue) * binFactor;
							if (doubleBinIndex < 0)
								binIndex = 0;

							else if (doubleBinIndex > (double)maxBin)
								binIndex = maxBin;

							else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin
								{
								binIndex = (UInt32)doubleBinIndex;

										// This will catch case when doubleBinIndex is nan or 
										// -nan. The conversion from this can be a large 
										// unsigned 32-bit int.

								binIndex = MIN (binIndex, maxBin);

								}	// end "else doubleBinIndex >= 0 && ..."
								
							#if defined multispec_wxmac_alpha
										// Skip first (alpha) byte in wxBitmap
								offScreenPtr++;
							#endif

							*offScreenPtr = (SInt8)dataToLevelPtr[binIndex];

							#if defined multispec_wx
								offScreenPtr++;
								*offScreenPtr = (SInt8)(dataToLevelPtr[binIndex]);

								offScreenPtr++;
								*offScreenPtr = (SInt8)(dataToLevelPtr[binIndex]);
								
								#if defined multispec_wxlin_alpha
											// Skip last (alpha) byte in wxBitmap
									offScreenPtr++;
								#endif
							#endif

							offScreenPtr++;
							buffer8BytePtr += interval;

							}	// end "for (j=0;..."
								
						#if defined multispec_wxmac_alpha
									// Skip first (alpha) byte in wxBitmap
							offScreenPtr++;
						#endif

						*offScreenPtr = separatorByte;
						#if defined multispec_wx
							offScreenPtr++;
							*offScreenPtr = separatorByte;

							offScreenPtr++;
							*offScreenPtr = separatorByte;
								
							#if defined multispec_wxlin_alpha
										// Skip last (alpha) byte in wxBitmap
								offScreenPtr++;
							#endif
								
							#if defined multispec_wxmac_alpha
										// Skip first (alpha) byte in wxBitmap
								offScreenPtr++;
							#endif

							offScreenPtr++;
							*offScreenPtr = separatorByte;

							offScreenPtr++;
							*offScreenPtr = separatorByte;
						#endif

						offScreenPtr++;
						*offScreenPtr = separatorByte;
								
						#if defined multispec_wxlin_alpha
									// Skip last (alpha) byte in wxBitmap
							offScreenPtr++;
						#endif

						offScreenPtr++;
						dataToLevelPtr += bytesOffset;
						chanIndex++;

						}	// end "for (channel=0; channel<..."

							// Copy a portion of the image and check if user wants to 
							// exit drawing

					lineCount++;
					if (TickCount () >= gNextTime)
						{
						longSourceRect.bottom = lineCount;
						if (!CheckSomeDisplayEvents (gImageWindowInfoPtr,
																displaySpecsPtr,
																lcToWindowUnitsVariablesPtr,
																savedPortPixMapH,
																offScreenPixMapH,
																&longSourceRect,
																displayBottomMax))
							{
							stopFlag = TRUE;
							break;

							}	// end "if (!CheckSomeEvents (osMask..."
						
						#if defined multispec_wx
									// Get the bitmap raw data pointer again. It may have
									// changed.

							offScreenLinePtr =
										(unsigned char*)gImageWindowInfoPtr->imageBaseAddressH;
							offScreenLinePtr += size_t((lineCount-1) * pixRowBytes);
						#endif

						}	// end "if (TickCount () >= gNextTime)"

					if (line == lineStart)
						{
						#if defined multispec_wxlin
									// Also get the number bytes offset in case needed for
									// wxlin version
						
							lineBytesOffset += (offScreenPtr - savedOffScreenLinePtr);
						#endif
						
						savedOffScreenLinePtr = offScreenPtr;

						}	// end "if (line == lineStart)"

					#if defined multispec_mac || defined multispec_wx
						offScreenLinePtr += pixRowBytes;
					#endif	// defined multispec_mac || defined multispec_wx

					#if defined multispec_win
						offScreenLinePtr -= pixRowBytes;
					#endif	// defined multispec_win

					}	// end "if (errCode == noErr)"

				if (gUseThreadedIOFlag)
					{
							// Switch buffers.

					ioBufferPtr = (HDoublePtr)fileIOInstructionsPtr->inputBufferPtrs[
															fileIOInstructionsPtr->bufferUsedForIO];

					}	// end "if (gUseThreadedIOFlag)"

            }	// end "for (line=lineStart; ..."

					// Force last lines in image window for image file to be updated.

			longSourceRect.bottom = lineCount;
			if (!CheckSomeDisplayEvents (gImageWindowInfoPtr,
													displaySpecsPtr,
													lcToWindowUnitsVariablesPtr,
													savedPortPixMapH,
													offScreenPixMapH,
													&longSourceRect,
													displayBottomMax))
				{
				stopFlag = TRUE;
				break;

				}	// end "if (!CheckSomeEvents (osMask..."
			
			#if defined multispec_wxlin
						// Get the bitmap raw data pointer again. It may have changed.

				savedOffScreenLinePtr = lineBytesOffset +
										(unsigned char*)gImageWindowInfoPtr->imageBaseAddressH;
			#endif

			dataDisplayPtr += bytesOffset*imageFileNumberChannels;

			CloseUpFileIOInstructions (fileIOInstructionsPtr, NULL);

			}	// end "if (imageFileNumberChannels > 0)"

		if (stopFlag)
			break;

		}	// end "for (imageFile=1; ..."

			// Set up return for inSourceRect to indicate if last few lines need
			// to be drawn																			

	rectPtr->top = longSourceRect.top;
	rectPtr->bottom = longSourceRect.bottom;

	CheckSizeAndUnlockHandle (gToDisplayLevels.vectorHandle);

	DisposeIOBufferPointers (fileIOInstructionsPtr,
										&input_ioBufferPtr,
										&base_ioBufferPtr);

}	// end "Display4_8ByteImagesSideBySide"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DisplayMultispectralImage
//
//	Software purpose:	The purpose of this routine is to control drawing
//							images in the window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			DisplayImage in SDisplay.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/11/1988
//	Revised By:			Ravi S. Budruk			Date: 06/17/1988	
//	Revised By:			Larry L. Biehl			Date: 05/11/2020

Boolean DisplayMultispectralImage (void)

{
	GrafPtr								savedPort;

	time_t								startTime;

	DisplaySpecsPtr					displaySpecsPtr = NULL;
	FileInfoPtr							fileInfoPtr;
	HistogramSpecsPtr					histogramSpecsPtr = NULL;

	Boolean								continueFlag = TRUE,
											imageDisplayedFlag = FALSE,
											savePaletteChangedFlag;

	SInt16								windowType;

	SignedByte							histogramHandleStatus = -1;


			// If spare memory had to be used to load code resources, then exit routine.

	if (gMemoryTypeNeeded < 0)
		return (imageDisplayedFlag);

			// Force the 'Histogram' code resource to be loaded.

	gMemoryTypeNeeded = 1;
	ForceHistogramCodeResourceLoad ();

			// If spare memory had to be used to load 'Histogram' code
			// resources, then exit routine.

	if (gMemoryTypeNeeded < 0)
		return (imageDisplayedFlag);

			// Code resources loaded okay, so set flag back for non-Code resources.

	gMemoryTypeNeeded = 0;

			// Make certain that memory has been assigned for the histogram
			// structure and if the histogram information has been loaded, set  	
			// up the memory that can be used to compute enhancement information.

	histogramSpecsPtr = SetUpHistogramSpecsPtr (
													kSetUpHistogramArray, &histogramHandleStatus);
	continueFlag = (histogramSpecsPtr != NULL);
	
	if (continueFlag)
		{
		displaySpecsPtr = LoadMultispectralDisplaySpecs ();
		continueFlag = (displaySpecsPtr != NULL);

		}	// end "if (continueFlag)"

			// Handle to file information should already be locked.  Get pointer
			// to file information.																

	fileInfoPtr = gImageFileInfoPtr;
 
	if (continueFlag && gCallProcessorDialogFlag)
			continueFlag = DisplayMultispectralDialog (displaySpecsPtr);
		
	if (continueFlag)
		{
				// Initialize some global variables pertaining to output.

		gOutputTextOKFlag = TRUE;
		gOutputForce1Code = gOutputForce1CodeSetting;

				// Force text selection to start from end of present text.

		ForceTextToEnd ();

		continueFlag = ListHeaderInfo (NULL,
													0,
													&gOutputForce1Code,
													kNoStatisticsUsed,
													continueFlag);

				// Determine if Histogram control needs to be called.

		if (GetHistogramRequiredFlag (
								displaySpecsPtr->displayType,
								displaySpecsPtr->enhancementCode,
								displaySpecsPtr->minMaxCode) ||
								displaySpecsPtr->histogramCompute == kLoadHistogramFromDisk)
			{
					// Indicate that the histogram array does not need to be read in now.

			histogramSpecsPtr->histogramArrayPtr = NULL;

			gMemoryTypeNeeded = 1;
			continueFlag = HistogramControl (displaySpecsPtr->histogramCompute);
			gMemoryTypeNeeded = 0;

					// Get the histogram array pointer again.

			histogramSpecsPtr->histogramArrayPtr =
                    (UInt32*)GetHandlePointer (histogramSpecsPtr->histogramArrayH);

			}	// end "if (displaySpecsPtr->enhancementCode == ..."

				// Get start time for the processor.

		startTime = time (NULL);

				//	Make certain that we have a histogram for the image

		if (continueFlag)
			{
					// Set Window title to reflect channel(s) displayed

			SetImageWTitle (gActiveImageWindow, displaySpecsPtr, fileInfoPtr);
			
			if (gCallProcessorDialogFlag)
				{
                // Not calling the display processor implies that the arrow keys
                // are being used to go to the previous or next channels. Do not
                // need change the cursor to a wait status.
				
				MSetCursor (kWait);
				UpdateOutputWScrolls (gOutputWindow, 1, kDisplayMessage);
				
				#if defined multispec_win
							// For the wxWidgets versions, this causes the entire image
							// window to be update even when only a few lines are to be
							// displayed. Do not know why.
							// It may also not be needed for the Windows version. Will
							// have to test.
					CheckSomeEvents (osMask + updateMask);
				#endif
				
				}	// end "if (gCallProcessorDialogFlag)"
			
					// Set up the vector to convert image data values to display
					// level values

			if (HistogramVector (displaySpecsPtr, histogramSpecsPtr))
				{
				GetPort (&savedPort);
				SetPortWindowPort (gActiveImageWindow);

				savePaletteChangedFlag = !displaySpecsPtr->paletteUpToDateFlag;

						// Create the palette

				windowType = kImageWindowType;
				if (displaySpecsPtr->displayType == k1_ChannelThematicDisplayType)
					{
					windowType = kThematicWindowType;

					if (!displaySpecsPtr->paletteUpToDateFlag)
						ReleaseDisplayPaletteMemory (displaySpecsPtr);

					}	// end "if (...->displayType == k1_ChannelThematicDisplayType)"

				continueFlag = CreatePalette (gImageWindowInfoPtr->windowInfoHandle,
														displaySpecsPtr,
														windowType);

				//UpdateActiveImageWScrolls (displaySpecsPtr->magnification);

						// Draw the image as requested

				if (continueFlag)
					{
					#if defined multispec_wx
						gActiveImageViewCPtr->m_Scale = displaySpecsPtr->magnification;
					#endif	// defined multispec_wx
					UpdateImageZoomControls (
                            gActiveImageWindow, displaySpecsPtr->magnification, TRUE);

							// If the data is signed then adjust data so that it is
							// all positive. The histogram is based on all positive	
							// data.																	

					gConvertSignedDataFlag = TRUE;

							// Remove the selection rectangle if it exists and the
							// area to be displayed is now different.						

					if (displaySpecsPtr->displayedLineStart !=
                            displaySpecsPtr->lineStart ||
                            displaySpecsPtr->displayedLineEnd !=
                            displaySpecsPtr->lineEnd ||
                            displaySpecsPtr->displayedLineInterval !=
                            displaySpecsPtr->lineInterval ||
                            displaySpecsPtr->displayedColumnStart !=
                            displaySpecsPtr->columnStart ||
                            displaySpecsPtr->displayedColumnEnd !=
                            displaySpecsPtr->columnEnd ||
                            displaySpecsPtr->displayedColumnInterval !=
                            displaySpecsPtr->columnInterval)
						ClearSelectionArea (gActiveImageWindow);

							// Save displayed lines and columns.

					displaySpecsPtr->displayedLineStart = displaySpecsPtr->lineStart;
					displaySpecsPtr->displayedLineEnd = displaySpecsPtr->lineEnd;
					displaySpecsPtr->displayedLineInterval = displaySpecsPtr->lineInterval;

					displaySpecsPtr->displayedColumnStart = displaySpecsPtr->columnStart;
					displaySpecsPtr->displayedColumnEnd = displaySpecsPtr->columnEnd;
					displaySpecsPtr->displayedColumnInterval = 
																		displaySpecsPtr->columnInterval;

					displaySpecsPtr->displayedNumberChannels = 
																		displaySpecsPtr->numberChannels;

							// Indicate that class names for the legend for 1-channel 
							// thematic displays will need to be re-created.

					gImageFileInfoPtr->classNamesLoadedFlag = FALSE;

							// Get start time for the actual time to display the image.

					startTime = time (NULL);
                    
					DisplayColorImage (displaySpecsPtr, histogramSpecsPtr);

					imageDisplayedFlag = TRUE;

					gConvertSignedDataFlag = FALSE;

							// Save some of the last display structure settings.

					SaveDisplayStructureSettings (displaySpecsPtr,
															 gImageFileInfoPtr,
															 gImageWindowInfoPtr);

					}	// end "if (continueFlag)"

				UpdateActiveImageWScrolls (displaySpecsPtr->magnification);

				SetPort (savedPort);

            }	// end "if (HistogramVector (displaySpecsPtr))"

					// Indicate that the entire image can be selected if it is the
					// active window

			gUpdateEditMenuItemsFlag = TRUE;

			}	// end 'if (continueFlag)'

		if (gCallProcessorDialogFlag)
			MInitCursor ();

				// Get the end time and print the time elapsed in the output window.

		continueFlag = ListCPUTimeInformation (NULL, continueFlag, startTime);

				// Scroll output window to the selection and adjust the scroll bar.
		
		UpdateOutputWScrolls (gOutputWindow, 1, kDisplayMessage);

				// Unlock display specifications handles.

		CheckAndUnlockHandle (displaySpecsPtr->channelsHandle);

		}	// end 'if (continueFlag)'

	Handle displaySpecsHandle = GetActiveDisplaySpecsHandle ();
	CheckAndUnlockHandle (displaySpecsHandle);

			// Make certain that the parameters used to read in the histogram
			// array have been closed up.														

	CloseUpHistogramArrayReadParameters ();
	ReleaseHistogramSpecsPtr (gImageWindowInfoPtr);

	return (imageDisplayedFlag);

}	// end "DisplayMultispectralImage"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DisplayCImage
//
//	Software purpose:	The purpose of this routine is to display the two
//							and three channel images in color in a window on the
//							screen.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None				
// 
// Called By:			DisplayColorImage in SDisplay.cpp
//
//	Coded By:			Larry L. Biehl			Date: 07/12/1988
//	Revised By:			Larry L. Biehl			Date: 11/02/2019

void DisplayCImage (
				DisplaySpecsPtr					displaySpecsPtr,
				HistogramSpecsPtr					histogramSpecsPtr,
				FileInfoPtr							fileInfoPtr,
				HPtr									offScreenBufferPtr,
				UInt32								pixRowBytes,
				PixMapHandle						savedPortPixMapH,
				PixMapHandle						offScreenPixMapH,
				LongRect*							rectPtr,
				SInt16								numberChannels,
				SInt16								displayCode,
				LCToWindowUnitsVariables* 		lcToWindowUnitsVariablesPtr)

{
	double								binFactor1,
											binFactor2,
											binFactor3,
											minValue1,
											minValue2,
											minValue3;

	LongRect								longSourceRect;

	FileInfoPtr							localFileInfoPtr1,
											localFileInfoPtr2,
											localFileInfoPtr3;

	FileIOInstructionsPtr			fileIOInstructionsPtr;

	HFileIOBufferPtr					inputBufferPtr,
											ioBuffer1Ptr,
											ioBuffer2Ptr,
											ioBuffer3Ptr,
											outputBufferPtr;

	HistogramSummaryPtr				histogramSummaryPtr;

	HUCharPtr							dataDisplay1Ptr,
											dataDisplay2Ptr,
											dataDisplay3Ptr,
											offScreenLinePtr,
											offScreenPtr;

	UInt16*								channelListPtr;
	
	int									nextStatusAtLeastLine,
											nextStatusAtLeastLineIncrement;

	SInt32								displayBottomMax;

	UInt32								buffer1Offset,
											buffer2Offset,
											buffer3Offset,
											bytesOffset,
											columnInterval,
											interval,
											line,
											lineCount,
											lineEnd,
											lineInterval,
											maxBin1,
											maxBin2,
											maxBin3,
											numberBytes,
											numberSamples,
											endColumn,
											startColumn;

	SInt16								backgroundValueCode,
											errCode,
											maxChannel,
											minChannel,
											numberListChannels;

	UInt16								channelPtr[3],
											forceOutputByteCode,
											sortedChannelPtr[3];

	Boolean								BILSpecialFlag,
											bytesEqualOneFlag1,
											bytesEqualOneFlag2,
											bytesEqualOneFlag3,
											forceBISflag,
											packDataFlag;
	
	/*
	#if defined multispec_wx
				// This did not work.
		wxBusyCursor wait;
	#endif
	*/
			 // Set up display rectangle for copy bits, rect.bottom will be reset later

	longSourceRect = *rectPtr;
	displayBottomMax = longSourceRect.bottom;

			// Get the sorted channel order.

	if (numberChannels == 1)
		{
		channelPtr[0] = gToDisplayLevels.channel1;
		channelPtr[1] = gToDisplayLevels.channel1;
		channelPtr[2] = gToDisplayLevels.channel1;

		sortedChannelPtr[0] = channelPtr[0];

		minChannel = maxChannel = sortedChannelPtr[0];

		}	// end "if (numberChannels == 1)"

	else if (numberChannels == 2)
		{
		channelPtr[0] = gToDisplayLevels.channel1;
		channelPtr[1] = gToDisplayLevels.channel2;
		channelPtr[2] = gToDisplayLevels.channel1;

		sortedChannelPtr[0] = MIN (channelPtr[0], channelPtr[1]);
		sortedChannelPtr[1] = MAX (channelPtr[0], channelPtr[1]);

		minChannel = sortedChannelPtr[0];
		maxChannel = sortedChannelPtr[1];

		}	// end "else if (numberChannels == 2)"

	else	// numberChannels == 3
		{
		sortedChannelPtr[0] = channelPtr[0] = gToDisplayLevels.channel1;
		sortedChannelPtr[1] = channelPtr[1] = gToDisplayLevels.channel2;
		sortedChannelPtr[2] = channelPtr[2] = gToDisplayLevels.channel3;

		if (sortedChannelPtr[1] < sortedChannelPtr[0])
			{
			sortedChannelPtr[0] = channelPtr[1];
			sortedChannelPtr[1] = channelPtr[0];

			}	// end "if (sortedChannelPtr[1] < sortedChannelPtr[0])"

		if (sortedChannelPtr[2] < sortedChannelPtr[1])
			{
			maxChannel = sortedChannelPtr[1];
			sortedChannelPtr[1] = sortedChannelPtr[2];
			sortedChannelPtr[2] = maxChannel;

			}	// end "if (sortedChannelPtr[2] < sortedChannelPtr[1])"

		if (sortedChannelPtr[1] < sortedChannelPtr[0])
			{
			maxChannel = sortedChannelPtr[0];
			sortedChannelPtr[0] = sortedChannelPtr[1];
			sortedChannelPtr[1] = maxChannel;

			}	// end "if (sortedChannelPtr[1] < sortedChannelPtr[0])"

		minChannel = sortedChannelPtr[0];
		maxChannel = sortedChannelPtr[2];

		}	// end "else numberChannels == 3"

	numberListChannels = numberChannels;

			// Get the pointers to the file information for each channel.

	localFileInfoPtr1 =
            &fileInfoPtr[gImageLayerInfoPtr[channelPtr[0] + 1].fileInfoIndex];

	localFileInfoPtr2 =
            &fileInfoPtr[gImageLayerInfoPtr[channelPtr[1] + 1].fileInfoIndex];

	localFileInfoPtr3 =
            &fileInfoPtr[gImageLayerInfoPtr[channelPtr[2] + 1].fileInfoIndex];

	bytesEqualOneFlag1 = (localFileInfoPtr1->numberBytes == 1) ? TRUE : FALSE;
	bytesEqualOneFlag2 = (localFileInfoPtr2->numberBytes == 1) ? TRUE : FALSE;
	bytesEqualOneFlag3 = (localFileInfoPtr3->numberBytes == 1) ? TRUE : FALSE;
	//bytesEqualOneFlag1 = FALSE;
	//bytesEqualOneFlag2 = FALSE;
	//bytesEqualOneFlag3 = FALSE;

	histogramSummaryPtr = histogramSpecsPtr->histogramSummaryPtr;
	binFactor1 = histogramSummaryPtr[channelPtr[0]].binFactor;
	binFactor2 = histogramSummaryPtr[channelPtr[1]].binFactor;
	binFactor3 = histogramSummaryPtr[channelPtr[2]].binFactor;

	minValue1 = histogramSummaryPtr[channelPtr[0]].minNonSatValue;
	minValue2 = histogramSummaryPtr[channelPtr[1]].minNonSatValue;
	minValue3 = histogramSummaryPtr[channelPtr[2]].minNonSatValue;

	maxBin1 = histogramSummaryPtr[channelPtr[0]].numberBins - 1;
	maxBin2 = histogramSummaryPtr[channelPtr[1]].numberBins - 1;
	maxBin3 = histogramSummaryPtr[channelPtr[2]].numberBins - 1;

	if (displayCode == 101)
		{
		if (histogramSummaryPtr[channelPtr[0]].binType == kBinWidthOfOne)
			{
					// Set up so that the minNonSatValue starts at bin 1 not 0

			minValue1 -= 1;

			}	// end "if (...[channelPtr[0]].binType == kBinWidthOfOne)"

		}	// end "if (displayCode == 101)"

	else if (displayCode == 151)
		{
				// Note that the correct info for the displayCode of 51 was
				// loaded above.

		GetOneChannelThematicDisplayConversionValues (
															 gImageWindowInfoPtr,
															 &histogramSummaryPtr[channelPtr[0]],
															 displaySpecsPtr,
															 &binFactor1,
															 &minValue1,
															 &maxBin1);
		/*
		if (histogramSummaryPtr[channelPtr[0]].binType == kBinWidthOfOne)
			{
			binFactor1 = displaySpecsPtr->thematicBinWidth;
			minValue1 = displaySpecsPtr->thematicTypeMinMaxValues[0];
			maxBin1 = displaySpecsPtr->numberLevels - 1;

			}	// end "if (...[channelPtr[0]].binType == kBinWidthOfOne)"

		else if (histogramSummaryPtr[channelPtr[0]].binType == kBinWidthNotOne)
			{
			minValue1 = displaySpecsPtr->thematicTypeMinMaxValues[0];
			binFactor1 = displaySpecsPtr->thematicBinWidth;

			}	// end "else if (...[channelPtr[0]].binType == kBinWidthNotOne)"
		*/
		}	// end "else if (displayCode == 151"

			// Set up the general parameters in the File IO Instructions structure.

	if (displayCode < 100)
		{
		forceOutputByteCode = kDoNotForceBytes;
		//forceOutputByteCode = kForce2Bytes;
		forceBISflag = kDoNotForceBISFormat;
		packDataFlag = kDoNotPackData;
		columnInterval = 1;

		}	// end "if (displayCode < 100)"

	else	// displayCode >= 100
		{
		forceOutputByteCode = kForceReal8Bytes;
		forceBISflag = kDoNotForceBISFormat;
		packDataFlag = kPackData;
		columnInterval = displaySpecsPtr->columnInterval;

		}	// end "if (displayCode < 100)"

			// Get handle to block of memory to use as file IO buffer for three
			// channels of data.  If the input band interleave format is BIS,
			// then request memory for all channels in the file.

	startColumn = displaySpecsPtr->columnStart;
	endColumn = displaySpecsPtr->columnEnd;

	numberSamples = (endColumn - startColumn + columnInterval) / columnInterval;

	if (!GetIOBufferPointers (&gFileIOInstructions[0],
										gImageWindowInfoPtr,
										gImageLayerInfoPtr,
										gImageFileInfoPtr,
										(HUCharPtr*)&inputBufferPtr,
										(HUCharPtr*)&outputBufferPtr,
										startColumn,
										endColumn,
										columnInterval,
										numberChannels,
										sortedChannelPtr,
										packDataFlag,
										forceBISflag,
										forceOutputByteCode,
										kDoNotAllowForThreadedIO,
										&fileIOInstructionsPtr))
		{
		gUseThreadedIOFlag = FALSE;
																									return;

		}	// end "if (!GetIOBufferPointers (..."

	BILSpecialFlag = GetBILSpecial (fileIOInstructionsPtr,
												sortedChannelPtr,
												numberListChannels,
												displaySpecsPtr->columnStart,
												displaySpecsPtr->columnEnd,
												columnInterval,
												kDetermineSpecialBILFlag);

	channelListPtr = channelPtr;
	if (BILSpecialFlag || localFileInfoPtr1->gdalDataSetH != NULL)
		channelListPtr = sortedChannelPtr;

	bytesOffset = gToDisplayLevels.bytesOffset;

	dataDisplay1Ptr = (HUCharPtr) GetHandlePointer (
													gToDisplayLevels.vectorHandle, kLock);

	dataDisplay2Ptr = (HUCharPtr)&dataDisplay1Ptr[bytesOffset];

	dataDisplay3Ptr = (HUCharPtr)&dataDisplay1Ptr [2 * bytesOffset];

	backgroundValueCode = displaySpecsPtr->backgroundValueCode;

	lineCount = 0;
	longSourceRect.bottom = kCopyInterval;
	lineEnd = displaySpecsPtr->lineEnd;
	lineInterval = displaySpecsPtr->lineInterval;

			 // Draw the image in the image window. Note that Windows DIB
			 // images are loaded with first line at the end of the bitmap 
			 // to last line at the beginning of the bitmap.	

	#if defined multispec_mac || defined multispec_wx
		offScreenLinePtr = (HUCharPtr)offScreenBufferPtr;
	#endif	// defined multispec_mac || defined multispec_wx

	#if defined multispec_win
		UInt32 numberLines =
            (lineEnd - displaySpecsPtr->lineStart + lineInterval) / lineInterval;
		offScreenLinePtr = 
						(HUCharPtr)(offScreenBufferPtr + (numberLines - 1) * pixRowBytes);
	#endif	// defined multispec_win

	errCode = SetUpFileIOInstructions (fileIOInstructionsPtr,
													NULL,
													displaySpecsPtr->lineStart,
													lineEnd,
													lineInterval,
													startColumn,
													endColumn,
													columnInterval,
													numberListChannels,
													channelListPtr,
													kDetermineSpecialBILFlag);

	if (errCode == noErr) 
		{
		if (localFileInfoPtr1->bandInterleave == kBIL) 
			{
			if (BILSpecialFlag && forceOutputByteCode != kForceReal8Bytes) 
				{
				numberBytes = (displaySpecsPtr->columnStart - 1) * 		// 2;
															localFileInfoPtr1->numberBytes;
				
				buffer1Offset =
					//(UInt32)(channelPtr[0] - minChannel) * localFileInfoPtr1->numberBytes *
					(UInt32)(channelPtr[0] - minChannel) * 2 *
                        gImageWindowInfoPtr->maxNumberColumns + numberBytes;

				buffer2Offset =
					(UInt32)(channelPtr[1] - minChannel) * localFileInfoPtr1->numberBytes *
					//(UInt32)(channelPtr[1] - minChannel) * 2 *
                        gImageWindowInfoPtr->maxNumberColumns + numberBytes;

				buffer3Offset =
					(UInt32)(channelPtr[2] - minChannel) * localFileInfoPtr1->numberBytes *
					//(UInt32)(channelPtr[2] - minChannel) * 2 *
                        gImageWindowInfoPtr->maxNumberColumns + numberBytes;
					
				}	// end "if (BILSpecialFlag)"

			else	// !BILSpecialFlag
				{
				buffer1Offset = 0;
				if (forceOutputByteCode == kDoNotForceBytes)
				//if (forceOutputByteCode == kForce2Bytes)
					{
					buffer2Offset = numberSamples * localFileInfoPtr1->numberBytes;
					//buffer2Offset = numberSamples * 2;
					buffer3Offset =
							buffer2Offset + numberSamples * localFileInfoPtr2->numberBytes;
							//buffer2Offset + numberSamples * 2;

					}	// end "if (forceOutputByteCode == kDoNotForceBytes)"

				else	// forceOutputByteCode == kForceReal8Bytes
					{
					buffer2Offset = numberSamples * 8;
					buffer3Offset = buffer2Offset + numberSamples * 8;

					}	// end "else forceOutputByteCode == kForceReal8Bytes"

            }	// end "else !BILSpecialFlag"

			interval = displaySpecsPtr->columnInterval;
			if (packDataFlag)
				interval = 1;

			}	// end "else localFileInfoPtr1->bandInterleave == kBIL"

		else if (localFileInfoPtr1->bandInterleave == kBIS)
			{
			if (forceOutputByteCode == kDoNotForceBytes)
			//if (forceOutputByteCode == kForce2Bytes)
				{
				buffer1Offset = (channelPtr[0]) * localFileInfoPtr1->numberBytes;
				buffer2Offset = (channelPtr[1]) * localFileInfoPtr1->numberBytes;
				buffer3Offset = (channelPtr[2]) * localFileInfoPtr1->numberBytes;
				//buffer1Offset = (channelPtr[0]) * 2;
				//buffer2Offset = (channelPtr[1]) * 2;
				//buffer3Offset = (channelPtr[2]) * 2;

            }	// end "if (forceOutputByteCode == kDoNotForceBytes)"

			else	// forceOutputByteCode == kForceReal8Bytes
            {
				buffer1Offset = 0;
				buffer2Offset = 8;
				buffer3Offset = 16;

            }	// end "else forceOutputByteCode == kForceReal8Bytes"

					// Set the interval to use to skip through the data for each sample.
					// Adjust number of samples to reflect the number of samples that
					// will be read in one line of data for all channels in the image
					// that were read in.

			if (packDataFlag)
				{
				interval = numberListChannels;
				numberSamples *= numberListChannels;

            }	// end "if (packDataFlag)"

			else	// !packDataFlag
				{
				interval = 
						displaySpecsPtr->columnInterval * localFileInfoPtr1->numberChannels;
				numberSamples *= localFileInfoPtr1->numberChannels;

            }	// end "else !packDataFlag"

			}	// end "if (localFileInfoPtr1->bandInterleave == kBIS)"

		else	// localFileInfoPtr1->bandInterleave != kBIL && ... != kBIS
			{
			if (localFileInfoPtr1->gdalDataSetH != NULL)
				{
				UInt32 channelToSortIndex;
				UInt32 gdalNumberOfChannels = fileIOInstructionsPtr->numberGdalChannels;

						// Get channel list to sorted list index

				channelToSortIndex = 1;
				if (channelPtr[0] == minChannel)
					channelToSortIndex = 0;
				else if (channelPtr[0] == maxChannel)
					channelToSortIndex = gdalNumberOfChannels - 1;

				buffer1Offset = (UInt32)channelToSortIndex * numberSamples;

				channelToSortIndex = 1;
				if (channelPtr[1] == minChannel)
					channelToSortIndex = 0;
				else if (channelPtr[1] == maxChannel)
					channelToSortIndex = gdalNumberOfChannels - 1;

				buffer2Offset = (UInt32)channelToSortIndex *numberSamples;

				channelToSortIndex = 1;
				if (channelPtr[2] == minChannel)
					channelToSortIndex = 0;
				else if (channelPtr[2] == maxChannel)
					channelToSortIndex = gdalNumberOfChannels - 1;

				buffer3Offset = (UInt32)channelToSortIndex * numberSamples;

				if (forceOutputByteCode == kDoNotForceBytes)
				//if (forceOutputByteCode == kForce2Bytes)
					{
					buffer1Offset *= localFileInfoPtr1->numberBytes;
					buffer2Offset *= localFileInfoPtr2->numberBytes;
					buffer3Offset *= localFileInfoPtr3->numberBytes;
					//buffer1Offset *= 2;
					//buffer2Offset *= 2;
					//buffer3Offset *= 2;

					}	// end "if (forceOutputByteCode == kDoNotForceBytes)"

				else	// forceOutputByteCode == kForceReal8Bytes
					{
					buffer1Offset *= 8;
					buffer2Offset *= 8;
					buffer3Offset *= 8;

					}	// end "else forceOutputByteCode == kForceReal8Bytes"

            }	// end "if (localFileInfoPtr1->gdalDataSetH != NULL)"

			else	// localFileInfoPtr1->gdalDataSetH == NULL
            {
				buffer1Offset = 0;
				if (forceOutputByteCode == kDoNotForceBytes)
				//if (forceOutputByteCode == kForce2Bytes)
					{
					buffer2Offset = numberSamples * localFileInfoPtr1->numberBytes;
					buffer3Offset =
							buffer2Offset + numberSamples * localFileInfoPtr2->numberBytes;
					//buffer2Offset = numberSamples * 2;
					//buffer3Offset =
					//		buffer2Offset + numberSamples * 2;

					}	// end "if (forceOutputByteCode == kDoNotForceBytes)"

				else	// forceOutputByteCode == kForceReal8Bytes
					{
					buffer2Offset = numberSamples * 8;
					buffer3Offset = buffer2Offset + numberSamples * 8;

					}	// end "else forceOutputByteCode == kForceReal8Bytes"

            }	// end "else localFileInfoPtr1->gdalDataSetH == NULL"

			interval = displaySpecsPtr->columnInterval;
			if (packDataFlag)
				interval = 1;

			}	// end "else localFileInfoPtr1->bandInterleave != kBIL || ... != kBIS"


		//#ifndef multispec_wx
			ioBuffer1Ptr = (HFileIOBufferPtr)
												&outputBufferPtr->data.onebyte[buffer1Offset];
			ioBuffer2Ptr = (HFileIOBufferPtr)
												&outputBufferPtr->data.onebyte[buffer2Offset];
			ioBuffer3Ptr = (HFileIOBufferPtr)
												&outputBufferPtr->data.onebyte[buffer3Offset];
		//#else	// not defined multispec_wx
		//	ioBuffer1Ptr = (HFileIOBufferPtr)
		//				((unsigned char*)(outputBufferPtr->data.onebyte) + buffer1Offset);
		//	ioBuffer2Ptr = (HFileIOBufferPtr)
		//				((unsigned char*)(outputBufferPtr->data.onebyte) + buffer2Offset);
		//	ioBuffer3Ptr = (HFileIOBufferPtr)
		//				((unsigned char*)(outputBufferPtr->data.onebyte) + buffer3Offset);
		//#endif

		}	// end "if (errCode == noErr)"

	if (errCode == noErr)
		{
				// Intialize the nextTime variable to indicate when the next check
				// should occur for a command-.													

		gNextTime = TickCount() + kDisplayTimeOffset;
		
				// These variables are to make sure the display window is not being updated
				// after a very few lines are loaded in. It will override the time interval
				// which is currently every 1 second.
		
		nextStatusAtLeastLineIncrement = (int)((10 * lineInterval) / displaySpecsPtr->magnification);
		nextStatusAtLeastLineIncrement = MAX (nextStatusAtLeastLineIncrement, 10);
		nextStatusAtLeastLine = displaySpecsPtr->lineStart + nextStatusAtLeastLineIncrement;

		for (line=displaySpecsPtr->lineStart; line<=lineEnd; line+=lineInterval)
			{
					// Get the three channels for the line of image data.  Return
					// if there is a file IO error.

			errCode = GetLineOfData (fileIOInstructionsPtr,
											  line,
											  startColumn,
											  endColumn,
											  columnInterval,
											  (HUInt8Ptr)inputBufferPtr,
											  (HUInt8Ptr)outputBufferPtr);

			if (errCode != noErr)
				break;

			else	// errCode == noErr
            {
						//	Draw the line of data

				offScreenPtr = offScreenLinePtr;
				
				switch (displayCode)
					{
					case 1:
					case 51:
						Display1Channel8BitLine (displayCode,
															numberSamples,
															interval,
															localFileInfoPtr1,
															(HUCharPtr) ioBuffer1Ptr,
															dataDisplay1Ptr,
															maxBin1,
															offScreenPtr);
						break;

					case 2:
						Display2Channel8BitLine (numberSamples,
														  interval,
														  bytesEqualOneFlag1,
														  bytesEqualOneFlag2,
														  backgroundValueCode,
														  ioBuffer1Ptr,
														  ioBuffer2Ptr,
														  dataDisplay1Ptr,
														  dataDisplay2Ptr,
														  maxBin1,
														  maxBin2,
														  offScreenPtr);
						break;

					case 3:
						gImageWindowInfoPtr->windowType = kImageWindowType;
						Display3Channel8BitLine (numberSamples,
														  interval,
														  bytesEqualOneFlag1,
														  bytesEqualOneFlag2,
														  bytesEqualOneFlag3,
														  backgroundValueCode,
														  ioBuffer1Ptr,
														  ioBuffer2Ptr,
														  ioBuffer3Ptr,
														  dataDisplay1Ptr,
														  dataDisplay2Ptr,
														  dataDisplay3Ptr,
														  maxBin1,
														  maxBin2,
														  maxBin3,
														  offScreenPtr);
						break;

					case 12:
						Display1Channel16BitLine (numberSamples,
														  interval,
														  bytesEqualOneFlag1,
														  backgroundValueCode,
														  ioBuffer1Ptr,
														  dataDisplay1Ptr,
														  maxBin1,
														  (HUInt16Ptr)offScreenPtr);
						break;
					/*
							Option removed in 11/2019
					case 22:
						Display2Channel16BitLine (numberSamples,
														  interval,
														  bytesEqualOneFlag1,
														  bytesEqualOneFlag2,
														  backgroundValueCode,
														  ioBuffer1Ptr,
														  ioBuffer2Ptr,
														  dataDisplay1Ptr,
														  dataDisplay2Ptr,
														  maxBin1,
														  maxBin2,
														  (HUInt16Ptr) offScreenPtr,
														  displaySpecsPtr->rgbColors);
						break;
					*/
					/*
							Option removed in 11/2019
					case 23:
						Display2Channel24BitLine (numberSamples,
														  interval,
														  bytesEqualOneFlag1,
														  bytesEqualOneFlag2,
														  backgroundValueCode,
														  ioBuffer1Ptr,
														  ioBuffer2Ptr,
														  dataDisplay1Ptr,
														  dataDisplay2Ptr,
														  maxBin1,
														  maxBin2,
														  offScreenPtr,
														  displaySpecsPtr->rgbColors);
						break;
					*/
					case 32:
						Display3Channel16BitLine (numberSamples,
														  interval,
														  bytesEqualOneFlag1,
														  bytesEqualOneFlag2,
														  bytesEqualOneFlag3,
														  backgroundValueCode,
														  ioBuffer1Ptr,
														  ioBuffer2Ptr,
														  ioBuffer3Ptr,
														  dataDisplay1Ptr,
														  dataDisplay2Ptr,
														  dataDisplay3Ptr,
														  maxBin1,
														  maxBin2,
														  maxBin3,
														  (HUInt16Ptr)offScreenPtr);
						break;

					case 33:
						Display3Channel24BitLine (numberSamples,
														  interval,
														  bytesEqualOneFlag1,
														  bytesEqualOneFlag2,
														  bytesEqualOneFlag3,
														  backgroundValueCode,
														  ioBuffer1Ptr,
														  ioBuffer2Ptr,
														  ioBuffer3Ptr,
														  dataDisplay1Ptr,
														  dataDisplay2Ptr,
														  dataDisplay3Ptr,
														  maxBin1,
														  maxBin2,
														  maxBin3,
														  offScreenPtr);
						break;
						
					case 101:
					case 151:
						Display1Channel4Byte8BitLine (displayCode,
																numberSamples,
																  interval,
																  minValue1,
																  binFactor1,
																  (HDoublePtr)ioBuffer1Ptr,
																  dataDisplay1Ptr,
																  maxBin1,
																  offScreenPtr);
						break;

					case 102:
						Display2Channel4Byte8BitLine (numberSamples,
																  interval,
																  minValue1,
																  minValue2,
																  binFactor1,
																  binFactor2,
																  backgroundValueCode,
																  (HDoublePtr)ioBuffer1Ptr,
																  (HDoublePtr)ioBuffer2Ptr,
																  dataDisplay1Ptr,
																  dataDisplay2Ptr,
																  maxBin1,
																  maxBin2,
																  offScreenPtr);
						break;

					case 103:
						Display3Channel4Byte8BitLine (numberSamples,
																  interval,
																  minValue1,
																  minValue2,
																  minValue3,
																  binFactor1,
																  binFactor2,
																  binFactor3,
																  backgroundValueCode,
																  (HDoublePtr)ioBuffer1Ptr,
																  (HDoublePtr)ioBuffer2Ptr,
																  (HDoublePtr)ioBuffer3Ptr,
																  dataDisplay1Ptr,
																  dataDisplay2Ptr,
																  dataDisplay3Ptr,
																  maxBin1,
																  maxBin2,
																  maxBin3,
																  offScreenPtr);
						break;

					/*
							Option removed in 11/2019
					case 122:
						Display2Channel4Byte16BitLine (numberSamples,
																  interval,
																  minValue1,
																  minValue2,
																  binFactor1,
																  binFactor2,
																  backgroundValueCode,
																  (HDoublePtr)ioBuffer1Ptr,
																  (HDoublePtr)ioBuffer2Ptr,
																  dataDisplay1Ptr,
																  dataDisplay2Ptr,
																  maxBin1,
																  maxBin2,
																  (HUInt16Ptr)offScreenPtr,
																  displaySpecsPtr->rgbColors);
					*/
					/*
							Option removed in 11/2019
					case 123:
						Display2Channel4Byte24BitLine (numberSamples,
																  interval,
																  minValue1,
																  minValue2,
																  binFactor1,
																  binFactor2,
																  backgroundValueCode,
																  (HDoublePtr)ioBuffer1Ptr,
																  (HDoublePtr)ioBuffer2Ptr,
																  dataDisplay1Ptr,
																  dataDisplay2Ptr,
																  maxBin1,
																  maxBin2,
																  offScreenPtr,
																  displaySpecsPtr->rgbColors);
						break;
					*/
					case 132:
						Display3Channel4Byte16BitLine (numberSamples,
																  interval,
																  minValue1,
																  minValue2,
																  minValue3,
																  binFactor1,
																  binFactor2,
																  binFactor3,
																  backgroundValueCode,
																  (HDoublePtr)ioBuffer1Ptr,
																  (HDoublePtr)ioBuffer2Ptr,
																  (HDoublePtr)ioBuffer3Ptr,
																  dataDisplay1Ptr,
																  dataDisplay2Ptr,
																  dataDisplay3Ptr,
																  maxBin1,
																  maxBin2,
																  maxBin3,
																  (HUInt16Ptr)offScreenPtr);
						break;

					case 133:
						Display3Channel4Byte24BitLine (numberSamples,
																  interval,
																  minValue1,
																  minValue2,
																  minValue3,
																  binFactor1,
																  binFactor2,
																  binFactor3,
																  backgroundValueCode,
																  (HDoublePtr)ioBuffer1Ptr,
																  (HDoublePtr)ioBuffer2Ptr,
																  (HDoublePtr)ioBuffer3Ptr,
																  dataDisplay1Ptr,
																  dataDisplay2Ptr,
																  dataDisplay3Ptr,
																  maxBin1,
																  maxBin2,
																  maxBin3,
																  offScreenPtr);
						break;

					}	// end "switch (displayCode)"
				
						// Copy a portion of the image and
						// check if user wants to exit drawing

				lineCount++;
				if (TickCount() >= gNextTime && lineCount >= nextStatusAtLeastLine)
					{
					#if defined multispec_wx
						displaySpecsPtr->updateEndLine = lineCount;
					#endif
				
					longSourceRect.bottom = lineCount;
					if (!CheckSomeDisplayEvents (gImageWindowInfoPtr,
															 displaySpecsPtr,
															 lcToWindowUnitsVariablesPtr,
															 savedPortPixMapH,
															 offScreenPixMapH,
															 &longSourceRect,
															 displayBottomMax))
						break;
				
					#if defined multispec_wx
						displaySpecsPtr->updateStartLine = lineCount;

						if (gImageWindowInfoPtr->offscreenMapSize == 0)
							{
									// Get the bitmap raw data pointer again. It may have changed.
									// Only do this for multispectral images.

							offScreenLinePtr = (unsigned char*)gImageWindowInfoPtr->imageBaseAddressH;
							offScreenLinePtr += (SInt64)(lineCount-1) * pixRowBytes;
							
							}	// end "if (gImageWindowInfoPtr->offscreenMapSize == 0)"
					#endif
					
					nextStatusAtLeastLine = lineCount + nextStatusAtLeastLineIncrement;
					nextStatusAtLeastLine = MIN (nextStatusAtLeastLine, lineEnd);

					}	// end "if (TickCount() >= gNextTime && lineCount >= nextStatusAtLeastLine)"

				#if defined multispec_mac || defined multispec_wx
					offScreenLinePtr += pixRowBytes;
				#endif	// defined multispec_mac

				#if defined multispec_win
					offScreenLinePtr -= pixRowBytes;
				#endif	// defined multispec_win

				}	// end "else errCode == noErr"

			if (gUseThreadedIOFlag)
				{
						// Switch buffers.

				numberBytes = 0;
				if (fileIOInstructionsPtr->bufferUsedForIO == 1)
					numberBytes = fileIOInstructionsPtr->bufferOffset;

				ioBuffer1Ptr = (HFileIOBufferPtr)&outputBufferPtr->data.onebyte[
						buffer1Offset + numberBytes];
				ioBuffer2Ptr = (HFileIOBufferPtr)&outputBufferPtr->data.onebyte[
						buffer2Offset + numberBytes];
				ioBuffer3Ptr = (HFileIOBufferPtr)&outputBufferPtr->data.onebyte[
						buffer3Offset + numberBytes];

            }	// end "if (gUseThreadedIOFlag)"

			}	// end "for (line=displaySpecsPtr->lineStart;..."

				// Set up return for inSourceRect to indicate if last few lines need
				// to be drawn

		rectPtr->top = longSourceRect.top;
		
		if (longSourceRect.bottom != -1)
			longSourceRect.bottom = lineCount;
		rectPtr->bottom = longSourceRect.bottom;
		
		#if defined multispec_wx
			displaySpecsPtr->updateEndLine = lineCount;
		#endif

		}	// end "if (errCode == noErr)"

	CloseUpFileIOInstructions (fileIOInstructionsPtr, NULL);

	CheckSizeAndUnlockHandle (gToDisplayLevels.vectorHandle);

	CloseUpGeneralFileIOInstructions (fileIOInstructionsPtr);

	CheckAndDisposePtr ((Ptr)inputBufferPtr);

}	// end "DisplayCImage"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean DisplayMultispectralDialog
//
//	Software purpose:	The purpose of this routine is to handle the modal
//							dialog for confirming the image display specification.
//  						The modal dialog template is in the resource file.  
//							The default specification comes from the image file
//							specification.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/19/1988
//	Revised By:			Larry L. Biehl			Date: 09/01/2017	

Boolean DisplayMultispectralDialog (
				DisplaySpecsPtr					displaySpecsPtr)
				
{
	Boolean								returnFlag = FALSE;

#if defined multispec_mac
	 DialogSelectArea					dialogSelectArea;

	 Rect									bitsOfColorBox,
											minMaxPopupBox,
											theBox;

	 double								localGaussianStretch,
											localMinMaxValues[6],
											localThematicValueFactor,
											magnification,
											saveMagnification;

	 DialogPtr							dialogPtr;
	 FileInfoPtr						fileInfoPtr;

	 UInt16								*featurePtr;

	 UserItemUPP						drawBitsColorPopUpPtr = NULL,
											drawDisplayTypePopUpPtr = NULL,
											drawMinMaxPopUpPtr = NULL,
											drawStretchPopUpPtr = NULL,
											drawZeroAsPopUpPtr = NULL;

	 WindowInfoPtr						windowInfoPtr;

	 Handle								okHandle,
											theHandle;

	 SInt32								interval,
											theNum;

	 SInt16								dialogSpecificationID,
											duplicateChannelCode,
											enhanceMinMaxOptionCode,
											itemHit,
											itemHit2,
											localBlueChannelNumber,
											localGrayChannelNumber,
											localGreenChannelNumber,
											localColorLevelsMax[3][3],
											localNumberChannels,
											localNumberLevels,
											localPercentClip,
											localRedChannelNumber,
											localrgbColors,
											maxSystemPixelSize,
											maxValue,
											pixelSizeVector[3],
											savedBitsOfColor,
											tempInt,
											theType;

	 UInt16								localDisplayType;

	 Boolean								blueChannelInvertFlag,
											checkMinMaxSettingFlag,
											checkNumberLevelsFlag,
											continueFlag,
											dialogWindowShownFlag,
											greenChannelInvertFlag,
											includeVectorOverlaysFlag,
											modalDone,
											optionKeyFlag,
											redChannelInvertFlag,
											setUpDFilterTableFlag,
											thematicDisplayWithUserSettingFlag,
											userComputeFlag,
											updateBitsOfColorFlag,
											updateComputeHistogramFlag;


			// Check status of file information structure.								

	if (HGetState (gActiveImageFileInfoH) >= 0)
																							return (FALSE);

			// Initialize local variables.													

	dialogPtr = NULL;

			// Handle to file information should already be locked.  Get pointer 
			// to file information.																

	fileInfoPtr = gImageFileInfoPtr;

			// Handle to window information should already be locked.  Get 		
			// pointer to window information.												

	windowInfoPtr = gImageWindowInfoPtr;

			// Get pointer to memory for temporary storage of channel list.		

	localNumberChannels = windowInfoPtr->totalNumberChannels;
	featurePtr = (UInt16*)MNewPointer (
            (SInt32)localNumberChannels * sizeof (short int));
	continueFlag = (featurePtr != NULL);

			// Get the modal dialog for the image display specification				

	if (featurePtr != NULL) 
		{
		dialogSpecificationID = kDisplaySpecificationID;
		if (gAppearanceManagerFlag)
			dialogSpecificationID = kAppDisplaySpecificationID;

		dialogPtr = LoadRequestedDialog (dialogSpecificationID, kCopyScrap, 1, 2);

		}	// end "if (featurePtr != NULL)"

	if (dialogPtr == NULL) 
		{
		CheckAndDisposePtr ((Ptr)featurePtr);
		return (FALSE);

		}	// end "if (dialogPtr == NULL)"

			// Intialize local user item proc pointers.									

	//drawEnhancementPopUpPtr = NewUserItemUPP (DrawEnhancementPopUp);

			// Load current feature pointer and number channels.  					

	//localNumberChannels = displaySpecsPtr->numberChannels;
	//channels = (short int*)*displaySpecsPtr->channelsHandle;
	//for (index=0; index<displaySpecsPtr->numberChannels; index++)
	//	featurePtr[index] = channels[index];

	DisplayMultispectralDialogInitialize (dialogPtr,
														displaySpecsPtr,
														windowInfoPtr,
														&dialogSelectArea,
														featurePtr,
														&localNumberChannels,
														&localPercentClip,
														&localMinMaxValues[0],
														&duplicateChannelCode,
														&localGrayChannelNumber,
														&localRedChannelNumber,
														&localGreenChannelNumber,
														&localBlueChannelNumber,
														&localrgbColors,
														&localDisplayType,
														&maxSystemPixelSize,
														&gDisplayTypeMenuSelection,
														&gBitsOfColorSelection,
														&savedBitsOfColor,
														&saveMagnification,
														&pixelSizeVector[0],
														&localGaussianStretch,
														&gEnhanceStretchSelection,
														&enhanceMinMaxOptionCode,
														&gEnhanceMinMaxMenuSelection,
														&thematicDisplayWithUserSettingFlag,
														&gEnhanceZeroAsSelection,
														&localColorLevelsMax[0][0],
														&localNumberLevels,
														&userComputeFlag,
														&gChannelSelection,
														&redChannelInvertFlag,
														&greenChannelInvertFlag,
														&blueChannelInvertFlag,
														&localThematicValueFactor,
														&includeVectorOverlaysFlag);

			// Save handle for the OK button for use later.								

	GetDialogItem (dialogPtr, 1, &theType, &okHandle, &theBox);
	
			// Load name of image window into the dialog box.
	
	FileStringPtr fileNamePtr = 
							(FileStringPtr)GetFileNamePPointerFromFileInfo (fileInfoPtr);
	sprintf ((char*)gTextString, 
				"Set Display Specifications for '%s'", 
				(char*)&fileNamePtr[1]);
	
	if (CreateUnicodeStaticTextControl (dialogPtr,
													gTextString,
													strlen ((char*)gTextString),
													3,
													NULL) != noErr)
		ParamText ((ConstStr255Param)fileNamePtr, "\p", "\p", "\p");
		
			// The following did not work.
		
	//osErr = SetControlTitleWithCFString (theControl, cfStringRef);
	
	/*
	SetDialogItem (dialogPtr, 3, kControlDialogItem, theHandle, &theBox);
	osErr = ::GetDialogItemAsControl (dialogPtr, itemNumber, &theControl);
	osErr = GetDialogItemAsControl (dialogPtr, 3, &controlRef);
	if (osErr == noErr)
		{
		osErr = GetSuperControl (controlRef, &parentControlRef);
		//DisposeControl (controlRef);
		HideDialogItem (dialogPtr, 3);
		RemoveDialogItems (dialogPtr, 3, 1, TRUE); 
		osErr = CreateStaticTextControl (GetDialogWindow (dialogPtr), 
													&theBox, 
													CFSTR("This is a test"), 
													NULL, 
													&controlRef);
		//osErr = EmbedControl (controlRef, parentControlRef);
		ShowControl (controlRef);
		SetDialogItem (dialogPtr, 3, theType, (Handle)controlRef, &theBox);
		"Set Display Specifications for "  "
		}
	*/
	SetDialogItemDrawRoutine (dialogPtr,
										&theBox,
										14,
										15,
										(UserItemProcPtr)&DrawDisplayTypePopUp,
										&drawDisplayTypePopUpPtr,
										15,
										gPopUpDisplayTypeMenu,
										gDisplayTypeMenuSelection);

	SetDialogItemDrawRoutine (dialogPtr,
										&bitsOfColorBox,
										18,
										19,
										(UserItemProcPtr)&DrawBitsColorPopUp,
										&drawBitsColorPopUpPtr,
										19,
										gPopUpBitsOfColorMenu,
										gBitsOfColorSelection);

			// Enhance stretch to be used.				

	SetDialogItemDrawRoutine (dialogPtr,
										&theBox,
										20,
										21,
										(UserItemProcPtr)&DrawStretchPopUp,
										&drawStretchPopUpPtr,
										21,
										gPopUpStretchMenu,
										gEnhanceStretchSelection);

	SetDialogItemDrawRoutine (dialogPtr,
										&minMaxPopupBox,
										22,
										23,
										(UserItemProcPtr)&DrawMinMaxPopUp,
										&drawMinMaxPopUpPtr,
										23,
										gPopUpMinMaxMenu,
										gEnhanceMinMaxMenuSelection);

	checkMinMaxSettingFlag = FALSE;

	SetDialogItemDrawRoutine (dialogPtr,
										&theBox,
										24,
										25,
										(UserItemProcPtr)&DrawZeroAsPopUp,
										&drawZeroAsPopUpPtr,
										25,
										gPopUpTreatZeroAsMenu,
										gEnhanceZeroAsSelection);

			//	Set the draw routine for the channel popup box.							
			// Draw the default channel selection.				

	SetDialogItemDrawRoutine (dialogPtr,
										&theBox,
										0, // Do not hide the prompt which is item 26.
										29,
										NULL,
										&gDrawChannelsPopUpPtr,
										29,
										gPopUpAllSubsetMenu,
										gChannelSelection);

	if (gAppearanceManagerFlag)
		SetDLogControl (dialogPtr, 29, gChannelSelection);

			// Channel number to be displayed for 1-channel display					

	LoadDItemValue (dialogPtr, 33, (SInt32)localGrayChannelNumber);

			// Channel to be displayed as red part of 3-channel display 			

	LoadDItemValue (dialogPtr, 34, (SInt32)localRedChannelNumber);

			// Channel to be displayed as green part of 3-channel display			

	LoadDItemValue (dialogPtr, 35, (SInt32)localGreenChannelNumber);

			// Channel to be displayed as blue part of 3-channel display			

	LoadDItemValue (dialogPtr, 36, (SInt32)localBlueChannelNumber);

			// Set requests of inverting data value with respect to the display color.

	SetDLogControl (dialogPtr, IDC_RedChannelInvert, redChannelInvertFlag);
	SetDLogControl (dialogPtr, IDC_GreenChannelInvert, greenChannelInvertFlag);
	SetDLogControl (dialogPtr, IDC_BlueChannelInvert, blueChannelInvertFlag);

	LoadDItemRealValue (dialogPtr, IDC_LegendFactor, localThematicValueFactor, 5);

			// Center the dialog and then show it.											

	dialogWindowShownFlag = FALSE;
	if (gMultiSpecWorkflowInfo.workFlowCode != 1 &&
            gMultiSpecWorkflowInfo.workFlowCode != 2) 
		{
		setUpDFilterTableFlag = kSetUpDFilterTable;
		ShowDialogWindow (dialogPtr, dialogSpecificationID, kSetUpDFilterTable);

		if (gAppearanceManagerFlag)
			SetDLogControl (dialogPtr, 15, gDisplayTypeMenuSelection);

		gDialogItemDescriptorPtr[ID3C_Magnification] = kDItemReal;
		gDialogItemDescriptorPtr[IDC_LegendFactor] = kDItemReal;

		itemHit = 0;

		dialogWindowShownFlag = TRUE;

		}	// end "if (gMultiSpecWorkflowInfo.workFlowCode != 1 && ..."

	else	// gMultiSpecWorkflowInfo.workFlowCode == 1 || ...
		{
		setUpDFilterTableFlag = kDoNotSetUpDFilterTable;
		itemHit = gMultiSpecWorkflowInfo.workFlowCode;

		}	// end "else gMultiSpecWorkflowInfo.workFlowCode == 1 || ..."

	updateComputeHistogramFlag = FALSE;

	DisplayMultispectralDialogSetDefaultSelection (
            dialogPtr, localrgbColors, localDisplayType);
	modalDone = FALSE;
	do {
		if (dialogWindowShownFlag)
			{
			/*
			// This did not work to add ... to Gaussian when it was needed.
			if (gAppearanceManagerFlag && (gEventRecord.modifiers & optionKey) > 0)
				{
				optionKeyFlag = TRUE;
				SetMenuItemText (gPopUpStretchMenu,
										kGaussianStretch,
										"\pGaussian...");
				}
			*/	
			ModalDialog (gProcessorDialogFilterPtr, &itemHit);
			/*
			if (optionKeyFlag)
				{
				SetMenuItemText (gPopUpStretchMenu,
										kGaussianStretch,
										"\pGaussian");
				optionKeyFlag = FALSE;
										
				}
			*/
			}

		if (itemHit > 2) 
			{
					// If itemHit was a number item, check for bad values.			
					// If itemHit was a radio button make appropriate control		
					// settings to indicate to the user the present selection.		
					// Get the handle to the itemHit.										
					// For number value items, get the string and convert it to		
					// a number.																	

			GetDialogItem (dialogPtr, itemHit, &theType, &theHandle, &theBox);
			if (theType == kEditTextDialogItem) 
				{
				GetDialogItemText (theHandle, gTextString);
				StringToNum (gTextString, &theNum);

				}	// end "if (theType == 16)" 

			switch (itemHit) 
				{
				case 5: // Entire area to selected area switch.			
				case 8: //	 Display start line of image  
				case 9: //	 Display end line of image  
				case 10: //	 Display line interval of image  
				case 11: //	 Display start column of image  
				case 12: //	 Display end column of image  
				case 13: //	 Display column interval of image  
					DialogLineColumnHits (&dialogSelectArea,
													dialogPtr,
													itemHit,
													theHandle,
													theNum);
					break;

				case 15:
					if (gDisplayTypeMenuSelection < 0)
						break;

					if (gAppearanceManagerFlag)
						itemHit2 = GetDItemValue (dialogPtr, 15);

					else	// !gAppearanceManagerFlag)
						itemHit2 = StandardPopUpMenu (dialogPtr,
																 14,
																 15,
																 gPopUpDisplayTypeMenu,
																 gDisplayTypeMenuSelection,
																 kPopUpDisplayTypeMenuID);

					if (itemHit2 > 0 && itemHit2 != gDisplayTypeMenuSelection) 
						{
						updateBitsOfColorFlag = TRUE;

						gDisplayTypeMenuSelection = 
													DisplayMultispectralDialogUpdateDisplayType (
																	  displaySpecsPtr,
																	  dialogPtr,
																	  gDisplayTypeMenuSelection,
																	  itemHit2,
																	  savedBitsOfColor,
																	  &localDisplayType,
																	  &localrgbColors,
																	  &gBitsOfColorSelection,
																	  maxSystemPixelSize,
																	  &localNumberLevels,
																	  GetDItemValue (dialogPtr, 34),
																	  GetDItemValue (dialogPtr, 35),
																	  GetDItemValue (dialogPtr, 36),
																	  &localColorLevelsMax[0][0],
																	  &duplicateChannelCode,
																	  &checkMinMaxSettingFlag);

						if (gAppearanceManagerFlag)
							SetDLogControl (dialogPtr, 19, gBitsOfColorSelection);

						updateComputeHistogramFlag = TRUE;

						InvalWindowRect (GetDialogWindow (dialogPtr), &bitsOfColorBox);

						}	// end "if (itemHit > 0 && ..." 

							// Make certain that the correct label is drawn in the	
							// display type pop up box.										

					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;

				case 17: // Magnification	
					magnification = GetDItemRealValue (dialogPtr, 17);
					if (magnification > gMaxMagnification)
						RealNumberErrorAlert (saveMagnification, dialogPtr, 17, 3);
					if (magnification > gMinMagnification && 
																magnification <= gMaxMagnification)
						saveMagnification = magnification;
					break;

				case 19: // Get bits of color to use. 
					if (gBitsOfColorSelection < 0)
						break;

					if (gAppearanceManagerFlag)
						itemHit2 = GetDItemValue (dialogPtr, 19);

					else	// !gAppearanceManagerFlag)
						{
						itemHit2 = StandardPopUpMenu (dialogPtr,
																  18,
																  19,
																  gPopUpBitsOfColorMenu,
																  gBitsOfColorSelection,
																  kPopUpBitsOfColorMenuID);

						}	// end "else !gAppearanceManagerFlag)"

					if (itemHit2 > 0 && itemHit2 != gBitsOfColorSelection) 
						{
						checkNumberLevelsFlag = TRUE;

								// New selection has been made.								

						gBitsOfColorSelection = itemHit2;
						savedBitsOfColor = gBitsOfColorSelection;

						}	// end "if (itemHit > 0 && ..."

							// Make certain that the correct label is drawn in the	
							// display type pop up box.										

					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;

				case 21: // Get enhancement stretch to use. 

							// If the option key is down, add the ... after Gaussian to
							// allow the user to change the Gaussian setting.

					optionKeyFlag = FALSE;
					if ((gEventRecord.modifiers & optionKey) > 0)
						optionKeyFlag = TRUE;

					if (gAppearanceManagerFlag)
						itemHit2 = GetDItemValue (dialogPtr, 21);

					else	// !gAppearanceManagerFlag)
						{
								// If the option key is down, add the ... after Gaussian to
								// allow the user to change the Gaussian setting.

						if (optionKeyFlag)
							SetMenuItemText (gPopUpStretchMenu,
													kGaussianStretch,
													"\pGaussian...");

						itemHit2 = StandardPopUpMenu (dialogPtr,
																  20,
																  21,
																  gPopUpStretchMenu,
																  gEnhanceStretchSelection,
																  kPopUpStretchMenuID);

						if (optionKeyFlag)
							SetMenuItemText (gPopUpStretchMenu,
													kGaussianStretch,
													"\pGaussian");

						}	// end "else !gAppearanceManagerFlag"

					if (itemHit2 == kGaussianStretch && optionKeyFlag)
						{
						HiliteControl ((ControlHandle)okHandle, 255);
						GaussianParameterDialog (&localGaussianStretch);
						HiliteControl ((ControlHandle)okHandle, 0);

						SetMenuItemText (gPopUpStretchMenu,
												kGaussianStretch,
												"\pGaussian");

						}	// end "if (itemHit2 == kGaussianStretch && optionKeyFlag)"

					if (itemHit2 > 0 && itemHit2 != gEnhanceStretchSelection)
						{
								// New selection has been made.
	
						gEnhanceStretchSelection = itemHit2;

						}	// end "if (itemHit > 0 && ..."

					updateComputeHistogramFlag = TRUE;

							// Make certain that the correct label is drawn in the
							// enhanceme stretch pop up box.

					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;

				case 23: // Min & Max
					localNumberLevels = GetDItemValue (dialogPtr, 27);
					itemHit2 = EnhanceMinMaxPopUpMenu (dialogPtr,
																	 23,
																	 localrgbColors,
																	 localDisplayType,
																	 localNumberLevels,
																	 okHandle,
																	 enhanceMinMaxOptionCode,
																	 localMinMaxValues,
																	 &localPercentClip);

					if (itemHit2 > 0)
						{
						enhanceMinMaxOptionCode = itemHit2;

						if (itemHit2 == kPerChannelMinMax && (localPercentClip != 0 &&
																				localPercentClip != 2))
							itemHit2 = kUserSpecified;

						if (itemHit2 == kPerChannelMinMax && localPercentClip == 0)
							itemHit2 = kPerChannelMinMaxNoClip;

						gEnhanceMinMaxMenuSelection = itemHit2;

						}	// end "if (itemHit2 > 0)"

							// Make certain that the correct label is drawn in the
							// enhance min-max pop up box.	

					if (gAppearanceManagerFlag)
						SetDItemValue (dialogPtr, 23, gEnhanceMinMaxMenuSelection);

					updateComputeHistogramFlag = TRUE;

					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;

				case 25:	// Treat 0's as
					if (gAppearanceManagerFlag)
						itemHit2 = GetDItemValue (dialogPtr, 25);

					else	// !gAppearanceManagerFlag)
						itemHit2 = StandardPopUpMenu (dialogPtr,
																24,
																25,
																gPopUpTreatZeroAsMenu,
																gEnhanceZeroAsSelection,
																kPopUpTreatZeroAsMenuID);

					if (itemHit2 > 0 && itemHit2 != gEnhanceZeroAsSelection)
						{
								// New selection has been made.

						gEnhanceZeroAsSelection = itemHit2;

						}	// end "if (itemHit > 0 && ..."

							// Make certain that the correct label is drawn in the
							// enhance treat 0's as pop up box.

					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;

				case 27:	// Number of display levels
					maxValue = localColorLevelsMax[abs (gBitsOfColorSelection) - 1][0];

					//if (localDisplayType == k2_ChannelDisplayType)
					//	maxValue = localColorLevelsMax[gBitsOfColorSelection - 1][1];

					if (localDisplayType == k3_ChannelDisplayType)
						{
						if (duplicateChannelCode == 0)
							maxValue = localColorLevelsMax[gBitsOfColorSelection - 1][2];
						else
							maxValue = localColorLevelsMax[gBitsOfColorSelection - 1][1];

						}	// end "if (localDisplayType == k3_ChannelDisplayType)"

					if (theNum == 0 || theNum > maxValue)
						NumberErrorAlert ((SInt16)maxValue, dialogPtr, itemHit);
					break;

				case 29:	// Selected channels
					if (gAppearanceManagerFlag)
						itemHit = GetDItemValue (dialogPtr, 29);

					else	// !gAppearanceManagerFlag)
						{
						itemHit = StandardPopUpMenu (dialogPtr,
															  28,
															  29,
															  gPopUpAllSubsetMenu,
															  gChannelSelection,
															  kPopUpAllSubsetMenuID);

						}	// end "else !gAppearanceManagerFlag)"

					if (itemHit == 2)
						{
								// Subset of channels to be used.

						HiliteControl ((ControlHandle)okHandle, 255);
						ChannelsDialog (&localNumberChannels,
											  (SInt16*)featurePtr,
											  gImageLayerInfoPtr,
											  fileInfoPtr,
											  1,
											  kNoTransformation,
											  NULL,
											  windowInfoPtr->totalNumberChannels,
											  gChannelSelection);

						if (localNumberChannels == windowInfoPtr->totalNumberChannels)
							{
							itemHit = 1;

							if (gAppearanceManagerFlag)
								SetDItemValue (dialogPtr, 29, 1);

							}	// end "if (localNumberChannels == ...->totalNumberChannels)"

						HiliteControl ((ControlHandle)okHandle, 0);

								// Force the main dialog box to be redrawn.

						InvalWindowRect (GetDialogWindow (dialogPtr),
								  GetPortBounds (GetDialogPort (dialogPtr), &gTempRect));

						}	// end "if (itemHit == 2)"

					if (itemHit == 1)
						localNumberChannels = windowInfoPtr->totalNumberChannels;

					if (itemHit != 0)
						gChannelSelection = itemHit;

							// Make certain that the correct label is drawn in the
							// channel pop up box.

					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;

				case 33: // channel to display for 1-channel display
					//if (theNum == 0 || theNum > windowInfoPtr->totalNumberChannels)
					//		NumberErrorAlert (
					//					(SInt32)displaySpecsPtr->channelNumber,
					//					gTextString,
					//					dialogPtr,
					//					itemHit);
					break;

				case 34: // channel to display for red part 3-channel display
				case 35: // channel to display for green part 3-channel display
				case 36: // channel to display for blue part 3-channel display
					checkNumberLevelsFlag = TRUE;
					break;

				case 38: // Display channel descriptions.
					HiliteControl ((ControlHandle)okHandle, 255);
					returnFlag = ChannelsDialog (&tempInt,
															 NIL,
															 gImageLayerInfoPtr,
															 fileInfoPtr,
															 2,
															 kNoTransformation,
															 NIL,
															 windowInfoPtr->totalNumberChannels,
															 kSubsetMenuItem);
					HiliteControl ((ControlHandle)okHandle, 0);

							// Force the main dialog box to be redrawn.

					InvalWindowRect (GetDialogWindow (dialogPtr),
											GetPortBounds (GetDialogPort (dialogPtr),
											&gTempRect));
					break;

				case 39:	// Check box for compute histogram
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					userComputeFlag = !userComputeFlag;
					break;

				case IDC_RedChannelInvert: // Check box for compute histogram
				case IDC_GreenChannelInvert: // Check box for compute histogram
				case IDC_BlueChannelInvert: // Check box for compute histogram
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					break;

				case IDC_LegendFactor:
					localThematicValueFactor =
								 GetDItemRealValue (dialogPtr, IDC_LegendFactor);
					break;

				case IDC_VectorOverlays: // Check box for overlays
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					break;

				}	// end "switch (itemHit)"

			if (checkNumberLevelsFlag)
				{
				DisplayMultispectralDialogCheckDisplayLevels (
																		displaySpecsPtr,
																		dialogPtr,
																		localDisplayType,
																		GetDItemValue (dialogPtr, 34),
																		GetDItemValue (dialogPtr, 35),
																		GetDItemValue (dialogPtr, 36),
																		&localColorLevelsMax[0][0],
																		&duplicateChannelCode,
																		gBitsOfColorSelection,
																		&localNumberLevels);

				checkNumberLevelsFlag = FALSE;

				}	// end "if (checkNumberLevelsFlag)"

			if (checkMinMaxSettingFlag)
				{
				DisplayMultispectralDialogCheckMinMaxSettings (
																gDisplayTypeMenuSelection,
																gEnhanceMinMaxMenuSelection,
																&gEnhanceMinMaxMenuSelection,
																&enhanceMinMaxOptionCode,
																&thematicDisplayWithUserSettingFlag);

						// Make certain that the correct label is drawn in the
						// enhance min-max pop up box.

				if (gAppearanceManagerFlag)
					SetDItemValue (dialogPtr, 23, gEnhanceMinMaxMenuSelection);

				InvalWindowRect (GetDialogWindow (dialogPtr), &minMaxPopupBox);

				checkMinMaxSettingFlag = FALSE;

				}	// end "if (checkMinMaxSettingFlag)"

			if (updateBitsOfColorFlag)
				updateBitsOfColorFlag =
					  DisplayMultispectralDialogUpdateBitsOfColor (gPopUpBitsOfColorMenu,
																				  localDisplayType);

					// Indicate whether the compute histogram box needs to
					// be checked and hilited.

			if (updateComputeHistogramFlag)
				{
				DisplayMultispectralDialogUpdateComputeHistogram (
																			displaySpecsPtr,
																			userComputeFlag,
																			gDisplayTypeMenuSelection,
																			gEnhanceStretchSelection,
																			enhanceMinMaxOptionCode,
																			dialogPtr,
																			39);

				updateComputeHistogramFlag = FALSE;

				}	// end "if (updateComputeHistogramFlag)"

			}	// end "if (itemHit > 2)"

		else if (itemHit > 0) // and itemHit <= 2
			{
					// If item hit is 1, check if display line-column values make
					// sense.  If they don't, sound an alert and make item hit
					// equal to 0 to allow user to make changes.

			if (itemHit == 1)
				itemHit = CheckLineColValues (&dialogSelectArea,
														  displaySpecsPtr->lineStart,
														  displaySpecsPtr->columnStart,
														  dialogPtr);

					// Check channels used for red, green and blue or gray scale.

			if (itemHit == 1 && localDisplayType <= k1_ChannelGrayLevelDisplayType)
				itemHit = CheckMaxValue (dialogPtr,
												  33,
												  1,
												  gImageWindowInfoPtr->totalNumberChannels,
												  kDisplayRangeAlert);

			if (itemHit == 1 && localrgbColors & 0x0004)
				itemHit = CheckMaxValue (dialogPtr,
												  34,
												  1,
												  gImageWindowInfoPtr->totalNumberChannels,
												  kDisplayRangeAlert);

			if (itemHit == 1 && localrgbColors & 0x0002)
				itemHit = CheckMaxValue (dialogPtr,
												  35,
												  1,
												  gImageWindowInfoPtr->totalNumberChannels,
												  kDisplayRangeAlert);

			if (itemHit == 1 && localrgbColors & 0x0001)
				 itemHit = CheckMaxValue (dialogPtr,
												  36,
												  1,
												  gImageWindowInfoPtr->totalNumberChannels,
												  kDisplayRangeAlert);

			if (itemHit == 1)
				{
				interval = GetDItemValue (dialogPtr, 10);
				itemHit = CheckNumberDisplayLines (dialogSelectArea.lineStart,
																dialogSelectArea.lineEnd,
																interval,
																dialogPtr,
																8);

				}	// end "if (itemHit == 1)"

			if (itemHit == 1)
				{
				interval = GetDItemValue (dialogPtr, 13);
				itemHit = CheckNumberDisplayColumns (
												(UInt32)dialogSelectArea.columnStart,
												(UInt32)dialogSelectArea.columnEnd,
												(UInt32)interval,
												pixelSizeVector[abs (gBitsOfColorSelection) - 1],
												localDisplayType,
												localNumberChannels,
												dialogPtr,
												11);

				}	// end "if (itemHit == 1)"

			if (itemHit == 1)	// User selected OK for information
            {
				modalDone = TRUE;

				DisplayMultispectralDialogOK (
											dialogPtr,
											displaySpecsPtr,
											windowInfoPtr,
											fileInfoPtr,
											&dialogSelectArea,
											featurePtr,
											localNumberChannels,
											localPercentClip,
											&localMinMaxValues[0],
											duplicateChannelCode,
											(SInt16)GetDItemValue (dialogPtr, 33),
											(SInt16)GetDItemValue (dialogPtr, 34),
											(SInt16)GetDItemValue (dialogPtr, 35),
											(SInt16)GetDItemValue (dialogPtr, 36),
											localrgbColors,
											localDisplayType,
											gDisplayTypeMenuSelection,
											gBitsOfColorSelection,
											saveMagnification,
											&pixelSizeVector[0],
											localGaussianStretch,
											gEnhanceStretchSelection,
											enhanceMinMaxOptionCode,
											gEnhanceZeroAsSelection,
											GetDItemValue (dialogPtr, 27),
											&localColorLevelsMax[0][0],
											GetDLogControl (dialogPtr, 39),
											gChannelSelection,
											GetDLogControl (dialogPtr, IDC_RedChannelInvert),
											GetDLogControl (dialogPtr, IDC_GreenChannelInvert),
											GetDLogControl (dialogPtr, IDC_BlueChannelInvert),
											localThematicValueFactor,
											GetDLogControl (dialogPtr, IDC_VectorOverlays));

				returnFlag = TRUE;

				}	// end "if (itemHit == 1)"

			if (itemHit == 2) // User selected Cancel for information
            {
				modalDone = TRUE;
				returnFlag = FALSE;

            }	// end "if	(itemHit == 2)"

			}	// end "else if (itemHit > 0) and itemHit <= 2"

		} while (!modalDone);

	featurePtr = CheckAndDisposePtr (featurePtr);

	UnSetDialogItemDrawRoutine (dialogPtr,
											drawDisplayTypePopUpPtr,
											15,
											gPopUpDisplayTypeMenu);

	UnSetDialogItemDrawRoutine (dialogPtr,
											drawBitsColorPopUpPtr,
											19,
											gPopUpBitsOfColorMenu);

	UnSetDialogItemDrawRoutine (dialogPtr,
											drawStretchPopUpPtr,
											21,
											gPopUpStretchMenu);

	UnSetDialogItemDrawRoutine (dialogPtr,
											drawMinMaxPopUpPtr,
											23,
											gPopUpMinMaxMenu);

	UnSetDialogItemDrawRoutine (dialogPtr,
											drawZeroAsPopUpPtr,
											25,
											gPopUpTreatZeroAsMenu);

	UnSetDialogItemDrawRoutine (dialogPtr,
											NULL,
											29,
											gPopUpAllSubsetMenu);

	CloseRequestedDialog (dialogPtr, setUpDFilterTableFlag);
#endif	// defined multispec_mac

	#if defined multispec_win
		CMDisplaySpecsDlg* dialogPtr = NULL;

		TRY
			{
			dialogPtr = new CMDisplaySpecsDlg ();

			gActiveImageViewCPtr->m_displayMultiCPtr->SetDisplaySpecsPtr (
																							displaySpecsPtr);

			returnFlag = dialogPtr->DoDialog (displaySpecsPtr);

			delete dialogPtr;
			}

		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			}
		END_CATCH_ALL
	#endif	// defined multispec_win

	#if defined multispec_wx
		CMDisplaySpecsDlg* dialogPtr = NULL;
		try
			{
         dialogPtr = new CMDisplaySpecsDlg (NULL);
			gActiveImageViewCPtr->m_displayMultiCPtr->SetDisplaySpecsPtr (
																						displaySpecsPtr);

			returnFlag = dialogPtr->DoDialog (displaySpecsPtr);

			delete dialogPtr;
			}
		catch (int e)
			{
			MemoryMessage (0, kObjectMessage);
			}
	#endif

	return (returnFlag);

}	// end "DisplayMultispectralDialog"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DisplayMultispectralDialogCheckDisplayLevels
//
//	Software purpose:	The purpose of this routine is to update settings related to
//							changes in the display type selection by the user.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None				
// 
// Called By:			DisplayMultispectralDialog in SDisplayMultispectral.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/22/2006
//	Revised By:			Larry L. Biehl			Date: 12/01/2006

void DisplayMultispectralDialogCheckDisplayLevels (
				DisplaySpecsPtr					displaySpecsPtr,
				DialogPtr							dialogPtr,
				SInt16								displayType,
				SInt16								redChannel,
				SInt16								greenChannel,
				SInt16								blueChannel,
				SInt16*								localColorLevelsMaxPtr,
				SInt16*								duplicateChannelCodePtr,
				SInt16								bitsOfColorSelection,
				SInt16*								numberDisplayLevelsPtr)

{
	*duplicateChannelCodePtr = 0;

	if (displayType == k3_ChannelDisplayType)
		{
		if (greenChannel == blueChannel)
			*duplicateChannelCodePtr = kRGColor;

		if (redChannel == blueChannel)
			*duplicateChannelCodePtr = kGBColor;

		if (redChannel == greenChannel)
			*duplicateChannelCodePtr = kRBColor;

		}	// end "if (displayType == k3_ChannelDisplayType)"

	DisplayMultispectralDialogUpdateDisplayLevels (dialogPtr,
																	displaySpecsPtr,
																	localColorLevelsMaxPtr,
																	displayType,
																	*duplicateChannelCodePtr,
																	bitsOfColorSelection,
																	numberDisplayLevelsPtr);

}	// end "DisplayMultispectralDialogCheckDisplayLevels"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DisplayMultispectralDialogCheckMinMaxSettings
//
//	Software purpose:	The purpose of this routine is to update settings related to
//							changes in the display type selection by the user.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None				
// 
// Called By:			DisplayMultispectralDialog in SDisplayMultispectral.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/22/2006
//	Revised By:			Larry L. Biehl			Date: 11/22/2006

void DisplayMultispectralDialogCheckMinMaxSettings (
				SInt16								displayTypeSelection,
				SInt16								inputEnhanceMinMaxMenuSelection,
				SInt16*								enhanceMinMaxMenuSelectionPtr,
				SInt16*								enhanceMinMaxOptionPtr,
				Boolean*								thematicDisplayWithUserSettingFlagPtr)

{
	if (inputEnhanceMinMaxMenuSelection == kThematicDefault &&
										displayTypeSelection != k1_ChannelThematicDisplayType)
		{
		*enhanceMinMaxMenuSelectionPtr = kPerChannelMinMax;
		*enhanceMinMaxOptionPtr = kPerChannelMinMax;

		}	// end "if (inputEnhanceMinMaxMenuSelection == kThematicDefault && ..."

	else if (inputEnhanceMinMaxMenuSelection == kUserSpecified &&
					displayTypeSelection != k1_ChannelThematicDisplayType &&
							*thematicDisplayWithUserSettingFlagPtr)
		{
		*enhanceMinMaxMenuSelectionPtr = kPerChannelMinMax;
		*enhanceMinMaxOptionPtr = kPerChannelMinMax;
		*thematicDisplayWithUserSettingFlagPtr = FALSE;

		}	// end "if (inputEnhanceMinMaxMenuSelection == kThematicDefault && ..."

	else if (inputEnhanceMinMaxMenuSelection != kThematicDefault &&
									displayTypeSelection == k1_ChannelThematicDisplayType)
		{
		*enhanceMinMaxMenuSelectionPtr = kThematicDefault;
		*enhanceMinMaxOptionPtr = kThematicDefault;

		}	// end "if (inputEnhanceMinMaxMenuSelection == kThematicDefault && ..."

	else if (inputEnhanceMinMaxMenuSelection == kUserSpecified &&
										displayTypeSelection == k1_ChannelThematicDisplayType)
		{
		*thematicDisplayWithUserSettingFlagPtr = TRUE;

		}	// end "if (inputEnhanceMinMaxMenuSelection == kThematicDefault && ..."

}	// end "DisplayMultispectralDialogCheckMinMaxSettings"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DisplayMultispectralDialogUpdateComputeHistogram
//
//	Software purpose:	The purpose of this routine is to draw the enhancement
//							prompt and enhancement selection for the enhancement
//							pop up menu.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/05/1991
//	Revised By:			Larry L. Biehl			Date: 05/01/2007	

Boolean DisplayMultispectralDialogUpdateComputeHistogram (
				DisplaySpecsPtr					displaySpecsPtr,
				Boolean								userComputeFlag,
				SInt16								displayType,
				SInt16								enhanceStretchSelection,
				SInt16								lMinMaxCode,
				DialogPtr							dialogPtr,
				SInt16								itemNumber)

{
	Boolean								histogramRequiredFlag,
											setComputeItemFlag;


			// Determine if histogram is required for the stretch enhancement

	histogramRequiredFlag = GetHistogramRequiredFlag (displayType,
																		enhanceStretchSelection,
																		lMinMaxCode);

	setComputeItemFlag = FALSE;
	if (!displaySpecsPtr->defaultHistogramFileFlag)
		setComputeItemFlag = histogramRequiredFlag;

	if (setComputeItemFlag) 
		{
		SetDLogControl (dialogPtr, itemNumber, 1);
		SetDLogControlHilite (dialogPtr, itemNumber, 255);

		}	// end "if ((gEnhancementSelection != 1 ||..." 

	else	// !setComputeItemFlag 
		{
		if (!userComputeFlag || !histogramRequiredFlag)
			SetDLogControl (dialogPtr, itemNumber, 0);

		if (histogramRequiredFlag)
					// This allow user to request a new histogram to display the data
			SetDLogControlHilite (dialogPtr, itemNumber, 0);

		else	// !histogramRequiredFlag
			SetDLogControlHilite (dialogPtr, itemNumber, 255);

		}	// end "else !setComputeItemFlag"

	return (setComputeItemFlag);

}	// end "DisplayMultispectralDialogUpdateComputeHistogram" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean DisplayMultispectralDialogInitialize
//
//	Software purpose:	The purpose of this routine is to initialize variables that
//							will be used for the display specification dialog box.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/22/2006
//	Revised By:			Larry L. Biehl			Date: 11/12/2019

void DisplayMultispectralDialogInitialize (
				DialogPtr							dialogPtr,
				DisplaySpecsPtr					displaySpecsPtr,
				WindowInfoPtr						windowInfoPtr,
				DialogSelectArea*					dialogSelectAreaPtr,
				UInt16*								localFeaturesPtr,
				SInt16*								localNumberChannelsPtr,
				SInt16*								localPercentClipPtr,
				double*								localMinMaxValuesPtr,
				SInt16*								duplicateChannelCodePtr,
				SInt16*								localGrayChannelNumberPtr,
				SInt16*								localRedChannelNumberPtr,
				SInt16*								localGreenChannelNumberPtr,
				SInt16*								localBlueChannelNumberPtr,
				SInt16*								localrgbColorsPtr,
				UInt16*								localDisplayTypePtr,
				SInt16*								maxSystemPixelSizePtr,
				SInt16*								displayTypeMenuSelectionPtr,
				SInt16*								bitsOfColorSelectionPtr,
				SInt16*								savedBitsOfColorPtr,
				double*								saveMagnificationPtr,
				SInt16*								pixelSizeVectorPtr,
				double*								localGaussianStretchPtr,
				SInt16*								enhanceStretchSelectionPtr,
				SInt16*								enhanceMinMaxOptionCodePtr,
				SInt16*								enhanceMinMaxMenuSelectionPtr,
				Boolean*								thematicDisplayWithUserSettingFlagPtr,
				SInt16*								enhanceZeroAsSelectionPtr,
				SInt16*								localColorLevelsMaxPtr,
				SInt16*								numberDisplayLevelsPtr,
				Boolean*								userComputeFlagPtr,
				SInt16*								channelSelectionPtr,
				Boolean*								redChannelInvertFlagPtr,
				Boolean*								greenChannelInvertFlagPtr,
				Boolean*								blueChannelInvertFlagPtr,
				double*								localThematicValueFactorPtr,
				Boolean*								includeVectorOverlaysFlagPtr)

{
			// Initialize selected area structure.		

	InitializeDialogSelectArea (dialogSelectAreaPtr,
											windowInfoPtr,
											gActiveImageWindow,
											displaySpecsPtr->columnStart,
											displaySpecsPtr->columnEnd,
											displaySpecsPtr->columnInterval,
											displaySpecsPtr->lineStart,
											displaySpecsPtr->lineEnd,
											displaySpecsPtr->lineInterval,
											8,
											5,
											kDontAdjustToBaseImage);

			// Load the dialog local vectors

	*localNumberChannelsPtr = displaySpecsPtr->numberChannels;
	LoadDialogLocalVectors (localFeaturesPtr,
										NULL,
										displaySpecsPtr->channelsHandle,
										displaySpecsPtr->numberChannels,
										NULL,
										NULL,
										NULL,
										NULL,
										NULL,
										NULL,
										NULL,
										NULL,
										NULL,
										NULL,
										NULL,
										0,
										NULL);

	*localPercentClipPtr = displaySpecsPtr->percentTailsClipped;

	localMinMaxValuesPtr[0] = displaySpecsPtr->minMaxValues[2][0];
	localMinMaxValuesPtr[1] = displaySpecsPtr->minMaxValues[2][1];
	localMinMaxValuesPtr[2] = displaySpecsPtr->minMaxValues[1][0];
	localMinMaxValuesPtr[3] = displaySpecsPtr->minMaxValues[1][1];
	localMinMaxValuesPtr[4] = displaySpecsPtr->minMaxValues[0][0];
	localMinMaxValuesPtr[5] = displaySpecsPtr->minMaxValues[0][1];

	if (displaySpecsPtr->displayType == k1_ChannelThematicDisplayType) 
		{
		localMinMaxValuesPtr[0] = displaySpecsPtr->thematicTypeMinMaxValues[0];
		localMinMaxValuesPtr[1] = displaySpecsPtr->thematicTypeMinMaxValues[1];

		}	// end "if (...->displayType == k1_ChannelThematicDisplayType)"

			// Initialize the variable indicating if 3-channel display contains	
			// duplicate channels.																

	*duplicateChannelCodePtr = 0;

	*localGrayChannelNumberPtr = displaySpecsPtr->channelNumber;
	*localRedChannelNumberPtr = displaySpecsPtr->redChannelNumber;
	*localGreenChannelNumberPtr = displaySpecsPtr->greenChannelNumber;
	*localBlueChannelNumberPtr = displaySpecsPtr->blueChannelNumber;

			// Update the display specification dialog with the information		
			// given in the image specification												

			// To entire image icon.															
			//	Selected display area															

	LoadLineColumnItems (dialogSelectAreaPtr, 
								dialogPtr, 
								kInitializeLineColumnValues, 
								kIntervalEditBoxesExist,
								1);

	*localrgbColorsPtr = displaySpecsPtr->rgbColors;

			// Set up diplay type.  First get default values to
			// use for number display values and palette type.							

	GetDefaultPaletteSpecs (displaySpecsPtr->displayType,
									displaySpecsPtr->pixelSize,
									windowInfoPtr->totalNumberChannels,
									localDisplayTypePtr,
									maxSystemPixelSizePtr);

	if (*localDisplayTypePtr == k3_2_ChannelDisplayType)
		*localDisplayTypePtr = k3_ChannelDisplayType;

	*displayTypeMenuSelectionPtr = displaySpecsPtr->displaySet;
	if (*localDisplayTypePtr == k1_ChannelGrayLevelDisplayType && 
																	*displayTypeMenuSelectionPtr != 2)
		*displayTypeMenuSelectionPtr = 2;

			// Enable/disable appropriate items in the display type pop up menu.	

	UpdatePopUpDisplayTypeMenu (windowInfoPtr->totalNumberChannels, 
											*maxSystemPixelSizePtr);

			// Update the variable indicating which colors are to be used.			

			// 1-channel color display.													

	if (*localDisplayTypePtr == k1_ChannelThematicDisplayType ||
				*localDisplayTypePtr == k1_ChannelGrayLevelDisplayType ||
						*localDisplayTypePtr == kSideSideChannelDisplayType)
		*localrgbColorsPtr = 8;

			// 2-channel display radio button.										

	//if (*localDisplayTypePtr == k2_ChannelDisplayType && *localrgbColorsPtr >= 7)
	//	*localrgbColorsPtr = kRGColor;

			// 3-channel display.														

	if (*localDisplayTypePtr == k3_ChannelDisplayType) 
		{
		*localrgbColorsPtr = 7;
		if (*localGreenChannelNumberPtr == *localBlueChannelNumberPtr)
			*duplicateChannelCodePtr = kRGColor;

		if (*localRedChannelNumberPtr == *localBlueChannelNumberPtr)
			*duplicateChannelCodePtr = kGBColor;

		if (*localRedChannelNumberPtr == *localGreenChannelNumberPtr)
			*duplicateChannelCodePtr = kRBColor;

		}	// end "if (*localDisplayTypePtr == 3 || ...)" 

			// Number of bits of color to use for display.
	/*
	updateBitsOfColorFlag = FALSE;

			// Set menu item 2 to reflect the actual number of bits to be 		
			// used if the max number of system color bits is > 1 and <= 8		

	if (maxSystemPixelSize == 2)
		SetMenuItemText (gPopUpBitsOfColorMenu, 1, "\p2");

	else if (maxSystemPixelSize == 4)
	 SetMenuItemText (gPopUpBitsOfColorMenu, 1, "\p4");

	else if (maxSystemPixelSize >= 8)
	 SetMenuItemText (gPopUpBitsOfColorMenu, 1, "\p8");
	*/
			// Get the default number of bits of color to use.						

	*bitsOfColorSelectionPtr = 1;

	*savedBitsOfColorPtr = *bitsOfColorSelectionPtr;
	if (displaySpecsPtr->lastColorPixelSize == 16)
		*savedBitsOfColorPtr = 2;

	else if (displaySpecsPtr->lastColorPixelSize == 24 ||
							displaySpecsPtr->lastColorPixelSize == 32)
		#if defined multispec_mac 
			*savedBitsOfColorPtr = 3;
		#endif	// defined multispec_mac 
		#if defined multispec_win || defined multispec_wx
			*savedBitsOfColorPtr = 2;
		#endif	// defined multispec_win || defined multispec_wx

	switch (*localDisplayTypePtr) 
		{
		//case 1:
		//			This one is not used any more.
		//	*bitsOfColorSelectionPtr = -1;
		//	savedBitsOfColor = *bitsOfColorSelectionPtr;
		//	break;
		/*		
		case k1_ChannelThematicDisplayType:
		case k1_ChannelGrayLevelDisplayType:
		case kSideSideChannelDisplayType:
			if (maxSystemPixelSize <= 2)
				{
				*bitsOfColorSelectionPtr = -1;
				savedBitsOfColor = *bitsOfColorSelectionPtr;
		
				}	// end "if (maxSystemPixelSize <= 2)"
			break;
		*/
		case k3_ChannelDisplayType:
		//case k2_ChannelDisplayType:		// Option removed 11/2019
		case k3_2_ChannelDisplayType:
			if (displaySpecsPtr->pixelSize == 16)
				*bitsOfColorSelectionPtr = 2;

			if (displaySpecsPtr->pixelSize == 24 ||
							displaySpecsPtr->pixelSize == 32)
			#if defined multispec_mac 
				*bitsOfColorSelectionPtr = 3;
			#endif	// defined multispec_mac 
			#if defined multispec_win || defined multispec_wx
				*bitsOfColorSelectionPtr = 2;
			#endif	// defined multispec_win || defined multispec_wx
			break;

		}	// end "switch (*localDisplayTypePtr)" 

	#if defined multispec_mac  	
		DisplayMultispectralDialogUpdateBitsOfColor (gPopUpBitsOfColorMenu,
																	*localDisplayTypePtr);
	#endif	// defined multispec_mac 


			// Magnification of image from bit/pix map to window.			

	*saveMagnificationPtr = displaySpecsPtr->magnification;
	LoadDItemRealValue (dialogPtr, ID3C_Magnification, *saveMagnificationPtr, 3);

			// Initialize pixel size vector.													

	pixelSizeVectorPtr[0] = 8;
	pixelSizeVectorPtr[1] = 16;
	#if defined multispec_mac 
		pixelSizeVectorPtr[2] = 32;
	#endif	// defined multispec_mac 
	#if defined multispec_win || defined multispec_wxlin
		pixelSizeVectorPtr[2] = 24;
	#endif	// defined multispec_win || defined multispec_wxlin
	#if defined multispec_wxmac
		pixelSizeVectorPtr[2] = 32;
	#endif	// defined multispec_wxmac

			// Enhance stretch to be used.				

	*localGaussianStretchPtr = displaySpecsPtr->gaussianStretch;

	*enhanceStretchSelectionPtr = displaySpecsPtr->enhancementCode;

			// Enhance min and max values to be used. We keep track of two items here, 
			// one is the min/max selection (enhanceMinMaxOptionCode) of 
			//		- per channel clipped, 
			//		- entire range 
			// 	- user specified
			//		- thematic default. 
			// The popup menu selection (gEnhanceMinMaxMenuSelection) will indicate
			//		- per channel clipped at 0%
			//		- per channel clipped at 2%
			//		- entire range for each channel
			//		- "user specified" if the user set the min-max or the selection is 
			//			clipped and the clip percent is not 0% or 2%.
			//		- thematic default which is 0 to something just above the max value
			//				for "nice" class or bin widths or from a non-saturated negative 
			//				value to the non-saturated maximum value.

	*enhanceMinMaxOptionCodePtr = displaySpecsPtr->minMaxCode;
	*enhanceMinMaxMenuSelectionPtr = displaySpecsPtr->minMaxCode;
	if (displaySpecsPtr->minMaxCode == kPerChannelMinMax &&
														displaySpecsPtr->percentTailsClipped == 0)
		*enhanceMinMaxMenuSelectionPtr = kPerChannelMinMaxNoClip;

	if (displaySpecsPtr->minMaxCode == kPerChannelMinMax &&
					(displaySpecsPtr->percentTailsClipped != 0 &&
											displaySpecsPtr->percentTailsClipped != 2))
		*enhanceMinMaxMenuSelectionPtr = kUserSpecified;

			// Make note of whether the input setting is Thematic type min-max with user
			// setting.  If it is then if the display type changes to something else
			// we need to set the min-max setting to kPerChannelMinMax.

	*thematicDisplayWithUserSettingFlagPtr = FALSE;
	if (*localDisplayTypePtr == k1_ChannelThematicDisplayType &&
												*enhanceMinMaxOptionCodePtr == kUserSpecified)
		*thematicDisplayWithUserSettingFlagPtr = TRUE;

			//	checkMinMaxSettingFlag = FALSE;

	SetUpMinMaxPopUpMenu (dialogPtr, *localDisplayTypePtr);

			// Directions on how to treat minimum data value (like 0).

	*enhanceZeroAsSelectionPtr = displaySpecsPtr->backgroundValueCode + 1;

			// Number of display levels.											

			// Initialize local varables for maximum number of levels dependant	
			// upon the display type.															
			// Default set for 1-bit black and white displays.	Not used any more.						

	//localColorLevelsMaxPtr[0][0] = 16;
	//localColorLevelsMaxPtr[1][0] = 16;
	//localColorLevelsMaxPtr[2][0] = 16;

			// Default set for 8-bit color displays.										

	////localColorLevelsMaxPtr[0] = kColorLevelsMax; // [0][0]
	localColorLevelsMaxPtr[0] = 256; // [0][0]
	localColorLevelsMaxPtr[1] = kColor2LevelsMax; // [1][0]
	localColorLevelsMaxPtr[2] = kColor3LevelsMax; // [2][0]

	if (*maxSystemPixelSizePtr == 2) 
		{
		localColorLevelsMaxPtr[0] = 4; // [0][0]
		localColorLevelsMaxPtr[1] = 4; // [1][0]
		localColorLevelsMaxPtr[2] = 4; // [2][0]

		}	// end "if (*maxSystemPixelSizePtr == 2)" 

	else if (*maxSystemPixelSizePtr == 4) 
		{
		localColorLevelsMaxPtr[0] = 14; // [0][0]
		localColorLevelsMaxPtr[1] = 3; // [1][0]
		localColorLevelsMaxPtr[2] = 2; // [2][0]

		}	// end "else if (*maxSystemPixelSizePtr == 4)" 

	// Default set for 16-bit color displays. 									

	localColorLevelsMaxPtr[3] = 32; // [0][1]
	localColorLevelsMaxPtr[4] = 32; // [1][1]
	localColorLevelsMaxPtr[5] = 32; // [2][1]

			// Default set for 24-bit color displays. 									

	localColorLevelsMaxPtr[6] = 256; // [0][2]
	localColorLevelsMaxPtr[7] = 256; // [1][2]
	localColorLevelsMaxPtr[8] = 256; // [2][2]

	DisplayMultispectralDialogUpdateDisplayLevels (dialogPtr,
																	displaySpecsPtr,
																	localColorLevelsMaxPtr,
																	*localDisplayTypePtr,
																	*duplicateChannelCodePtr,
																	*bitsOfColorSelectionPtr,
																	numberDisplayLevelsPtr);

			// Compute histogram check box.										

	*userComputeFlagPtr = FALSE;
	*userComputeFlagPtr = DisplayMultispectralDialogUpdateComputeHistogram (
																	displaySpecsPtr,
																	*userComputeFlagPtr,
																	*localDisplayTypePtr,
																	*enhanceStretchSelectionPtr,
																	*enhanceMinMaxOptionCodePtr,
																	dialogPtr,
																	ID3C_NewHistogram);

			// Display channel descriptions.										

	if (windowInfoPtr->descriptionCode != 0)
		SetDLogControl (dialogPtr, ID3C_ChannelDescriptions, 0);

	else	// windowInfoPtr->descriptionCode == 0
		SetDLogControlHilite (dialogPtr, ID3C_ChannelDescriptions, 255);

	*channelSelectionPtr = displaySpecsPtr->channelSet;

			// Set requests of inverting data value with respect to the display color.

	*redChannelInvertFlagPtr = displaySpecsPtr->invertValuesFlag[2];
	*greenChannelInvertFlagPtr = displaySpecsPtr->invertValuesFlag[1];
	*blueChannelInvertFlagPtr = displaySpecsPtr->invertValuesFlag[0];

			// Set up edit text fields for channels to be used for diplay			

			// Set the color text item 														
	#ifndef multispec_wx
		LoadDItemString (dialogPtr, ID3C_GrayPrompt, (Str255*)"\0Grey  ");
	#endif
	UpdateDialogChannelItems (dialogPtr, *localrgbColorsPtr, *localDisplayTypePtr);

			// Set legend factor			

	*localThematicValueFactorPtr = displaySpecsPtr->thematicValueFactor;

			// Only show option to overlay vectors if no image currently exists
			// for the window and shape files do exist.

	SetVectorOverlays (windowInfoPtr,
							dialogPtr,
							IDC_VectorOverlays,
							includeVectorOverlaysFlagPtr);
	
}	// end "DisplayMultispectralDialogInitialize"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean DisplayMultispectralDialogOK
//
//	Software purpose:	The purpose of this routine is to set the display specification
//							structure variables based on the user selection in the dialog
//							box.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/22/2006
//	Revised By:			Larry L. Biehl			Date: 05/11/2016

void DisplayMultispectralDialogOK (
				DialogPtr							dialogPtr,
				DisplaySpecsPtr					displaySpecsPtr,
				WindowInfoPtr						windowInfoPtr,
				FileInfoPtr							fileInfoPtr,
				DialogSelectArea*					dialogSelectAreaPtr,
				UInt16*								localFeaturesPtr,
				SInt16								localNumberChannels,
				SInt16								localPercentClip,
				double*								localMinMaxValuesPtr,
				SInt16								duplicateChannelCode,
				SInt16								localGrayChannelNumber,
				SInt16								localRedChannelNumber,
				SInt16								localGreenChannelNumber,
				SInt16								localBlueChannelNumber,
				SInt16								localrgbColors,
				UInt16								localDisplayType,
				SInt16								displayTypeMenuSelection,
				SInt16								bitsOfColorSelection,
				double								saveMagnification,
				SInt16*								pixelSizeVectorPtr,
				double								localGaussianStretch,
				SInt16								enhanceStretchSelection,
				SInt16								enhanceMinMaxOptionCode,
				SInt16								enhanceZeroAsSelection,
				SInt16								numberDisplayLevels,
				SInt16*								localColorLevelsMaxPtr,
				Boolean								userComputeFlag,
				SInt16								channelSelection,
				Boolean								redChannelInvertFlag,
				Boolean								greenChannelInvertFlag,
				Boolean								blueChannelInvertFlag,
				double								localThematicValueFactor,
				Boolean								includeVectorOverlaysFlag)

{
	UInt16*								channelsPtr;

	UInt32								index;


			// Selected display area.												

	displaySpecsPtr->lineStart = dialogSelectAreaPtr->lineStart;
	displaySpecsPtr->lineEnd = dialogSelectAreaPtr->lineEnd;
	displaySpecsPtr->lineInterval = dialogSelectAreaPtr->lineInterval;
	displaySpecsPtr->columnStart = dialogSelectAreaPtr->columnStart;
	displaySpecsPtr->columnEnd = dialogSelectAreaPtr->columnEnd;
	displaySpecsPtr->columnInterval = dialogSelectAreaPtr->columnInterval;

			// Display channel type:							
			//		1-channel B&W,														 
			// 	1-channel color,													
			//		2-channel color,													
			//		3-channel color													

	if (localDisplayType != displaySpecsPtr->displayType)
		{
		displaySpecsPtr->displayType = localDisplayType;
		displaySpecsPtr->paletteUpToDateFlag = FALSE;

		}	// end "if (localDisplayType != ...)" 

	displaySpecsPtr->displaySet = abs (displayTypeMenuSelection);

			// Bits of color to use in the display.			

	displaySpecsPtr->pixelSize =
			pixelSizeVectorPtr[abs (bitsOfColorSelection) - 1];

			// Enhancement options to use.					

	if (displaySpecsPtr->enhancementCode != enhanceStretchSelection)
		gToDisplayLevels.upToDateFlag = FALSE;

	if (displaySpecsPtr->minMaxCode != enhanceMinMaxOptionCode)
		gToDisplayLevels.upToDateFlag = FALSE;

	if (displaySpecsPtr->percentTailsClipped != localPercentClip)
		gToDisplayLevels.upToDateFlag = FALSE;

	if (displaySpecsPtr->gaussianStretch != localGaussianStretch)
		gToDisplayLevels.upToDateFlag = FALSE;

	displaySpecsPtr->enhancementCode = enhanceStretchSelection;
	displaySpecsPtr->minMaxCode = enhanceMinMaxOptionCode;
	displaySpecsPtr->percentTailsClipped = localPercentClip;
	displaySpecsPtr->gaussianStretch = localGaussianStretch;
	displaySpecsPtr->backgroundValueCode = enhanceZeroAsSelection - 1;

			// Number of display levels								

	if (displaySpecsPtr->numberLevels != numberDisplayLevels)
	  displaySpecsPtr->paletteUpToDateFlag = FALSE;

	displaySpecsPtr->numberLevels = numberDisplayLevels;

			// Channel to display for 1-channel display

	displaySpecsPtr->channelNumber = localGrayChannelNumber;

			// Get min max values to use for enhancement.  This is needed
			// here so that the legend list gets loaded correctly.
			// Decided not to do here.

	//if (displaySpecsPtr->minMaxCode == kThematicDefault)
	//	UpdateThematicTypeMinMaxes (displaySpecsPtr->numberLevels,
	//											displaySpecsPtr->channelNumber,
	//											&localMinMaxValues[0],
	//											&localMinMaxValues[1]);

			// Determine if the user specified min and max values changed.

	if (displaySpecsPtr->minMaxCode == kUserSpecified)
		{
		if (displaySpecsPtr->minMaxValues[0][0] != localMinMaxValuesPtr[4])
			gToDisplayLevels.upToDateFlag = FALSE;

		if (displaySpecsPtr->minMaxValues[0][1] != localMinMaxValuesPtr[5])
			gToDisplayLevels.upToDateFlag = FALSE;

		if (displaySpecsPtr->minMaxValues[1][0] != localMinMaxValuesPtr[2])
			gToDisplayLevels.upToDateFlag = FALSE;

		if (displaySpecsPtr->minMaxValues[1][1] != localMinMaxValuesPtr[3])
			gToDisplayLevels.upToDateFlag = FALSE;

		if (displaySpecsPtr->minMaxValues[2][0] != localMinMaxValuesPtr[0])
			gToDisplayLevels.upToDateFlag = FALSE;

		if (displaySpecsPtr->minMaxValues[2][1] != localMinMaxValuesPtr[1])
			gToDisplayLevels.upToDateFlag = FALSE;

		}	// end "if (...->minMaxCode == kUserSpecified)"

	displaySpecsPtr->minMaxValues[0][0] = localMinMaxValuesPtr[4];
	displaySpecsPtr->minMaxValues[0][1] = localMinMaxValuesPtr[5];
	displaySpecsPtr->minMaxValues[1][0] = localMinMaxValuesPtr[2];
	displaySpecsPtr->minMaxValues[1][1] = localMinMaxValuesPtr[3];
	displaySpecsPtr->minMaxValues[2][0] = localMinMaxValuesPtr[0];
	displaySpecsPtr->minMaxValues[2][1] = localMinMaxValuesPtr[1];

			// Magnification to use

	displaySpecsPtr->magnification = saveMagnification;

			// Compute Histogram Flag

	if (userComputeFlag)
		displaySpecsPtr->histogramCompute = kComputeHistogram;

	else if (displaySpecsPtr->histogramCompute == kComputeHistogram)
				// Histogram is not required.  Do not force to be computed.
		displaySpecsPtr->histogramCompute = kDoNotComputeHistogram;

			// Check if the new settings require a histogram to be computed.

	if (displaySpecsPtr->histogramCompute != kComputeHistogram)
		displaySpecsPtr->histogramCompute = GetHistogramComputeCode (displaySpecsPtr);

			// Channels

	channelsPtr = (UInt16*)GetHandlePointer (displaySpecsPtr->channelsHandle);
	if (channelSelection == kAllMenuItem) // All channels 
		{
		displaySpecsPtr->numberChannels = windowInfoPtr->totalNumberChannels;
		for (index = 0; index < windowInfoPtr->totalNumberChannels; index++)
			channelsPtr[index] = (UInt16)index;

		}	// end "if (channelSelection == kAllMenuItem)" 

	else	// channelSelection != kAllMenuItem 
		{
		displaySpecsPtr->numberChannels = localNumberChannels;
		for (index = 0; index < displaySpecsPtr->numberChannels; index++)
			channelsPtr[index] = localFeaturesPtr[index];

		}	// end "else channelSelection != kAllMenuItem" 

	displaySpecsPtr->channelSet = channelSelection;

			//	red channel to display for 3-channel display.	

	displaySpecsPtr->redChannelNumber = localRedChannelNumber;

			// green channel to display for 3-channel display.																	

	displaySpecsPtr->greenChannelNumber = localGreenChannelNumber;

			// blue channel to display for 3-channel display.

	displaySpecsPtr->blueChannelNumber = localBlueChannelNumber;

			// Get requests for invertion of date values for each display color.

	if (displaySpecsPtr->invertValuesFlag[2] != redChannelInvertFlag)
		{
				// Note that the invert flag for thematic type window
				// for a channel is stored in location 2.

		if (displaySpecsPtr->displayType == k1_ChannelThematicDisplayType)
			displaySpecsPtr->paletteUpToDateFlag = FALSE;

		else	// ...->displayType == k1_ChannelThematicDisplayType
			gToDisplayLevels.upToDateFlag = FALSE;

		}	// end "if (...->invertValuesFlag[2] != redChannelInvertFlag)"

	displaySpecsPtr->invertValuesFlag[2] = redChannelInvertFlag;

	if (displaySpecsPtr->invertValuesFlag[1] != greenChannelInvertFlag)
		gToDisplayLevels.upToDateFlag = FALSE;
	displaySpecsPtr->invertValuesFlag[1] = greenChannelInvertFlag;

	if (displaySpecsPtr->invertValuesFlag[0] != blueChannelInvertFlag)
		gToDisplayLevels.upToDateFlag = FALSE;
	displaySpecsPtr->invertValuesFlag[0] = blueChannelInvertFlag;

	if (displaySpecsPtr->rgbColors != localrgbColors)
		displaySpecsPtr->paletteUpToDateFlag = FALSE;

	displaySpecsPtr->rgbColors = localrgbColors;

			// If the display type is a 3-channel display, determine		
			// if there are any duplicate channels.							

	if (displaySpecsPtr->displayType == k3_ChannelDisplayType &&
															duplicateChannelCode != 0)
		{
		displaySpecsPtr->rgbColors = duplicateChannelCode;
		displaySpecsPtr->paletteUpToDateFlag = FALSE;
		displaySpecsPtr->displayType = k3_2_ChannelDisplayType;

		}	// end "if (displaySpecsPtr->displayType == 3 && ...)"

			// Also get the number of palette levels per channel. It	
			// may be different than the number of display levels per	
			// channel if 16 or 24 bit color is being used.	
			// Note that the numberPaletteLevels for 16 or 24 bit displayed
			// data is only used for generating a palette which contains
			// fewer than 256 colors for a palette window if shown.				

	displaySpecsPtr->numberPaletteLevels = displaySpecsPtr->numberLevels;
	if (displaySpecsPtr->pixelSize > 8)
		{
		if (//displaySpecsPtr->displayType == 4 ||
						displaySpecsPtr->displayType == k3_2_ChannelDisplayType)
			displaySpecsPtr->numberPaletteLevels =
												localColorLevelsMaxPtr[1]; // [1][0]

		else	// displaySpecsPtr->displayType != k3_2_ChannelDisplayType
			displaySpecsPtr->numberPaletteLevels =
												localColorLevelsMaxPtr[2]; // [2][0]

		displaySpecsPtr->numberPaletteLevels = MIN (
				 displaySpecsPtr->numberPaletteLevels, displaySpecsPtr->numberLevels);

		displaySpecsPtr->invertValues = FALSE;

		}	// end "if (displaySpecsPtr->pixelSize > 8)"

	if (//localDisplayType == k2_ChannelDisplayType ||
												localDisplayType == k3_ChannelDisplayType)
		displaySpecsPtr->lastColorPixelSize = displaySpecsPtr->pixelSize;

	if (localDisplayType == k1_ChannelThematicDisplayType) 
		{
		displaySpecsPtr->thematicValueFactor = localThematicValueFactor;

		fileInfoPtr->numberClasses = displaySpecsPtr->numberPaletteLevels;
		fileInfoPtr->maxClassNumberValue = fileInfoPtr->numberClasses - 1;

		if (displaySpecsPtr->numberDisplayClasses != fileInfoPtr->numberClasses)
			{
					// Force colors to be reloaded if the number of classes has changed.
					// Use that last specified by user if can for class palette.

			if (gDisplaySpecsDefault.structureLoadedFlag &&
					gDisplaySpecsDefault.oneChannelThematicClassPaletteType > kNoPaletteColorsDefined &&
						gDisplaySpecsDefault.oneChannelThematicClassPaletteType < kFalseColors)
				displaySpecsPtr->thematicClassPaletteType =
										gDisplaySpecsDefault.oneChannelThematicClassPaletteType;

			else	// !gDisplaySpecsDefault.structureLoadedFlag
				displaySpecsPtr->thematicClassPaletteType = kCorrelationMatrixColors;

			displaySpecsPtr->thematicGroupPaletteType = kCorrelationMatrixColors;

			}	// end "if (displaySpecsPtr->numberDisplayClasses != ..."

		displaySpecsPtr->numberDisplayClasses = fileInfoPtr->numberClasses;
		displaySpecsPtr->classGroupCode = kClassDisplay;

		}	// end "if (localDisplayType == k1_ChannelThematicDisplayType)"

	displaySpecsPtr->includeVectorOverlaysFlag = includeVectorOverlaysFlag;

}	// end "DisplayMultispectralDialogOK"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DisplayMultispectralDialogSetDefaultSelection
//
//	Software purpose:	The purpose of this routine is to determine the default selected
//							item in the display multispectral dialog box and make that
//							selection.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/09/2011
//	Revised By:			Larry L. Biehl			Date: 12/16/2016	

void DisplayMultispectralDialogSetDefaultSelection (
				DialogPtr							dialogPtr,
				SInt16								rgbColors,
				SInt16								displayType)

{
	SInt16 hiliteItem;


			// Set default text selection.												

	if (displayType == kSideSideChannelDisplayType)
		hiliteItem = ID3C_LineStart;

	else	// displayType != kSideSideChannelDisplayType 
		{
		hiliteItem = ID3C_GrayChannel;
		if (rgbColors & 4)
			hiliteItem = ID3C_RedChannel;

		else if (rgbColors & 2)
			hiliteItem = ID3C_GreenChannel;

		}	// end "else displayType != kSideSideChannelDisplayType" 

	SelectDialogItemText (dialogPtr, hiliteItem, 0, SInt16_MAX);

}	// end "DisplayMultispectralDialogSetDefaultSelection" 



#if defined multispec_mac  
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean DisplayMultispectralDialogUpdateBitsOfColor
//
//	Software purpose:	The purpose of this routine is to handle the modal
//							dialog for confirming the image display specification.
//  						The modal dialog template is in the resource file.  
//							The default specification comes from the image file
//							specification.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/18/2001
//	Revised By:			Larry L. Biehl			Date: 09/11/2006	

Boolean DisplayMultispectralDialogUpdateBitsOfColor (
				MenuHandle							popUpBitsOfColorMenu,
				SInt16								localDisplayType)

{
			// Update number of color bits popup menu options.	

	if (localDisplayType == 1) 
		{
		DisableMenuItem (popUpBitsOfColorMenu, 1);
		DisableMenuItem (popUpBitsOfColorMenu, 2);
		DisableMenuItem (popUpBitsOfColorMenu, 3);

		}	// end "else if (localDisplayType == 2 || ..."

	else if (localDisplayType == k1_ChannelThematicDisplayType ||
					localDisplayType == k1_ChannelGrayLevelDisplayType ||
								localDisplayType == kSideSideChannelDisplayType) 
		{
		EnableMenuItem (popUpBitsOfColorMenu, 1);
		DisableMenuItem (popUpBitsOfColorMenu, 2);
		DisableMenuItem (popUpBitsOfColorMenu, 3);

		}	// end "else if (localDisplayType == 2 || ..."

	else	// localDisplayType == 3, 4, 5, or 6
		{
		EnableMenuItem (popUpBitsOfColorMenu, 1);

		if (gQD32IsImplemented) 
			{
			EnableMenuItem (popUpBitsOfColorMenu, 2);
			EnableMenuItem (popUpBitsOfColorMenu, 3);

			}	// end "if (gQD32IsImplemented)" 

		else	// !gQD32IsImplemented
			{
			DisableMenuItem (popUpBitsOfColorMenu, 2);
			DisableMenuItem (popUpBitsOfColorMenu, 3);

			}	// end "else !gQD32IsImplemented" 

		if (gOSXFlag && !gOSXCoreGraphicsFlag)
			DisableMenuItem (popUpBitsOfColorMenu, 1);

		}	// end "else localDisplayType == 3, 4, 5, or 6" 

	return (TRUE);

}	// end "DisplayMultispectralDialogUpdateBitsOfColor"
#endif	// defined multispec_mac  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 DisplayMultispectralDialogUpdateDisplayType
//
//	Software purpose:	The purpose of this routine is to update settings related to
//							changes in the display type selection by the user.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None				
// 
// Called By:			DisplayMultispectralDialog in SDisplayMultispectral.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/22/2006
//	Revised By:			Larry L. Biehl			Date: 11/22/2006

SInt16 DisplayMultispectralDialogUpdateDisplayType (
				DisplaySpecsPtr					displaySpecsPtr,
				DialogPtr							dialogPtr,
				SInt16								currentDisplayTypeMenuSelection,
				SInt16								newDisplayTypeMenuSelection,
				SInt16								savedBitsOfColor,
				UInt16*								localDisplayTypePtr,
				SInt16*								localRGBColorsPtr,
				SInt16*								bitsOfColorSelectionPtr,
				SInt16								maxSystemPixelSize,
				SInt16*								numberDisplayLevelsPtr,
				SInt32								redChannel,
				SInt32								greenChannel,
				SInt32								blueChannel,
				SInt16*								localColorLevelsMaxPtr,
				SInt16*								duplicateChannelCodePtr,
				Boolean*								checkMinMaxSettingFlagPtr)

{
			// Set text fields for channels to be displayed and	
			// number of bits variables.									

	switch (newDisplayTypeMenuSelection) 
		{
		case 1: // 1-channel thematic display 
		case 2: // 1-channel gray level display 
		//case 7: // Side by Side channels
		case 4: // Side by Side channels
			*localDisplayTypePtr = newDisplayTypeMenuSelection;

					// Indicate that only one color plane will be used.														

			*localRGBColorsPtr = 8;

			if (gOSXFlag && !gOSXCoreGraphicsFlag) 
				{
						// Force 16-bit color to be used. 					

				if (maxSystemPixelSize != 2)
					*bitsOfColorSelectionPtr = 2;

				}	// end "if (gOSXFlag && !gOSXCoreGraphicsFlag)"

			else	// !gOSXFlag || gOSXCoreGraphicsFlag
				{
						// Force 8-bit color to be used. 					

				*bitsOfColorSelectionPtr = 1;

				}	// end "else !gOSXFlag || gOSXCoreGraphicsFlag"
			break;
		/*
		// These options were removed in 11/2019
		case 3: // 2-channel Red-Green display 
			*localDisplayTypePtr = k2_ChannelDisplayType;
			*localRGBColorsPtr = kRGColor;
			*bitsOfColorSelectionPtr = savedBitsOfColor;
			break;

		case 4: // 2-channel Green-Blue display 
			*localDisplayTypePtr = k2_ChannelDisplayType;
			*localRGBColorsPtr = kGBColor;
			*bitsOfColorSelectionPtr = savedBitsOfColor;
			break;

		case 5: // 2-channel Red-Blue display 
			*localDisplayTypePtr = k2_ChannelDisplayType;
			*localRGBColorsPtr = kRBColor;
			*bitsOfColorSelectionPtr = savedBitsOfColor;
			break;
		*/
		//case 6: // 3-channel display
		case 3: // 3-channel display
			*localDisplayTypePtr = k3_ChannelDisplayType;

					// Indicate that red, green, blues color planes will be used.											

			*localRGBColorsPtr = kRGBColor;
			*bitsOfColorSelectionPtr = savedBitsOfColor;
			break;

		}	// end "switch (newDisplayType)" 

			// Check the min-max setting if going from thematic type
			// display to other or vice versa

	if ((currentDisplayTypeMenuSelection != k1_ChannelThematicDisplayType &&
							newDisplayTypeMenuSelection == k1_ChannelThematicDisplayType) ||
			(currentDisplayTypeMenuSelection == k1_ChannelThematicDisplayType &&
							newDisplayTypeMenuSelection != k1_ChannelThematicDisplayType))
		*checkMinMaxSettingFlagPtr = TRUE;

	UpdateDialogChannelItems (dialogPtr,
										*localRGBColorsPtr,
										*localDisplayTypePtr);

			// Change settings in min-max popup menu if needed.

	SetUpMinMaxPopUpMenu (dialogPtr, *localDisplayTypePtr);

	DisplayMultispectralDialogCheckDisplayLevels (displaySpecsPtr,
																	dialogPtr,
																	*localDisplayTypePtr,
																	(SInt16)redChannel,
																	(SInt16)greenChannel,
																	(SInt16)blueChannel,
																	localColorLevelsMaxPtr,
																	duplicateChannelCodePtr,
																	*bitsOfColorSelectionPtr,
																	numberDisplayLevelsPtr);

	return (newDisplayTypeMenuSelection);

}	// end "DisplayMultispectralDialogUpdateDisplayType"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DisplayMultispectralDialogUpdateDisplayLevels
//
//	Software purpose:	The purpose of this routine is to update the display
//							levels dialog item in the multispectral display dialog.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/24/1993
//	Revised By:			Larry L. Biehl			Date: 12/01/2006	

void DisplayMultispectralDialogUpdateDisplayLevels (
				DialogPtr							dialogPtr,
				DisplaySpecsPtr					displaySpecsPtr,
				SInt16*								colorLevelsMaxPtr,
				SInt16								displayType,
				SInt16								duplicateChannelCode,
				SInt16								bitsOfColorSelection,
				SInt16*								numberDisplayLevelsPtr)

{
	SInt32								colorLevelsIndex,
											localDisplayLevels,
											numberChannelsIndex;


	switch (displayType) 
		{
		case k1_ChannelThematicDisplayType:
		case k1_ChannelGrayLevelDisplayType:
		case kSideSideChannelDisplayType:
			numberChannelsIndex = 0;
			break;

		case k3_ChannelDisplayType:
			if (duplicateChannelCode)
				numberChannelsIndex = 1;

			else
				numberChannelsIndex = 2;
			break;
		/*
				This option was removed in 11/2019
		case k2_ChannelDisplayType:
			numberChannelsIndex = 1;
			break;
		*/
		}	// end "switch (displayType)" 

	localDisplayLevels = displaySpecsPtr->numberLevels;

	if (displayType == k1_ChannelThematicDisplayType) 
		{
				// If displayLevels is equal to kColorLevelsMax or 256,
				// then assume that user will want the number of display levels
				// to be a smaller value.

		if (localDisplayLevels == kColorLevelsMax || localDisplayLevels == 256)
			localDisplayLevels = gDisplaySpecsDefault.numberThematicLevels;

		}	// end "displayType == k1_ChannelThematicDisplayType"

	else	// displayType != k1_ChannelThematicDisplayType
		{
		#if defined multispec_win || defined multispec_wx
					// For windows and linux, take into account that there are only 
					// 2 bits of color choices 8 and 24 not 8, 16 and 24.  Bits of Color 
					// of 2 actually is is an index of 3 into the color levels max vector.
			if (bitsOfColorSelection == 2)
				bitsOfColorSelection = 3;
		#endif	// defined multispec_win 

		colorLevelsIndex = 3 * (abs (bitsOfColorSelection) - 1) + numberChannelsIndex;
		if (displaySpecsPtr->displayType == k1_ChannelThematicDisplayType ||
							localDisplayLevels > colorLevelsMaxPtr[colorLevelsIndex])
			localDisplayLevels = colorLevelsMaxPtr[colorLevelsIndex];

		}	// end "else displayType != k1_ChannelThematicDisplayType"

	LoadDItemValue (dialogPtr, ID3C_NumberOfLevels, localDisplayLevels);

	*numberDisplayLevelsPtr = (SInt16)localDisplayLevels;

}	// end "DisplayMultispectralDialogUpdateDisplayLevels"




//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void Display1Channel8BitLine
//
//	Software purpose:	The purpose of this routine is to copy the input
//							line of data to the offscreen buffer for a 3-channel
//							color image.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None				
// 
// Called By:			DisplayColorImage in SDisplay.cpp
//
//	Coded By:			Larry L. Biehl			Date: 06/01/1996
//	Revised By:			Larry L. Biehl			Date: 12/13/2018

void Display1Channel8BitLine (
				SInt16								displayCode,
				UInt32								numberSamples,
				UInt32								interval,
				FileInfoPtr							fileInfoPtr,
				HUCharPtr							ioBuffer1Ptr,
				HUCharPtr							dataDisplayPtr,
				UInt32								maxValue,
				HUCharPtr							offScreenPtr)

{
	UInt32								dataValue,
											j;

	#if defined multispec_mac || defined multispec_win
	
				//	 This loop will draw the image lines for one byte data		

		if (fileInfoPtr->numberBytes == 1) 
			{
			for (j = 0; j < numberSamples; j += interval) 
				{
				dataValue = ioBuffer1Ptr[j];
				*offScreenPtr = dataDisplayPtr[dataValue];
				offScreenPtr++;

				}	// end "for (j=0;..." 

			}	// end "if (fileInfoPtr->numberBytes == 1)" 
	
				// This loop will draw the image lines for two byte data 		

		else if (fileInfoPtr->numberBytes == 2)
			{
			HUInt16Ptr ioBuffer2Ptr = (HUInt16Ptr)ioBuffer1Ptr;

			for (j = 0; j < numberSamples; j += interval) 
				{
				dataValue = ioBuffer2Ptr[j];
				if (dataValue > maxValue)
					 dataValue = 0;
				*offScreenPtr = dataDisplayPtr[dataValue];
				offScreenPtr++;

				}	// end "for (j=0;..." 

			}	// end "else if (fileInfoPtr->numberBytes == 2)"
	#endif
	
	#if defined multispec_wx
				//	 This loop will draw the image lines for one byte data
	
		if (fileInfoPtr->numberBytes == 1) 
			{
			if (displayCode == 1)
				{
				for (j = 0; j < numberSamples; j += interval)
					{
					#if defined multispec_wxmac_alpha
						offScreenPtr++;
					#endif
					
					dataValue = ioBuffer1Ptr[j];
					
							// Red value
					*offScreenPtr = (Byte)dataDisplayPtr[dataValue];
					offScreenPtr++;
					
							// Green Value
					*offScreenPtr = (Byte)dataDisplayPtr[dataValue];
					offScreenPtr++;
					
							// Blue value
					*offScreenPtr = (Byte)dataDisplayPtr[dataValue];
					offScreenPtr++;
					
					#if defined multispec_wxlin_alpha
								// Skip first (alpha) byte in wxBitmap
						offScreenPtr++;
					#endif

					}	// end "for (j=0;..."
				
				}	// end "if (displayCode == 1)"
		 
			else	// displayCode == 51
				{
						// This is for one channel thematic images for wxWidgets
						// interface. The thematic value is loaded into the buffer.
				
				for (j = 0; j < numberSamples; j += interval)
					{
					dataValue = ioBuffer1Ptr[j];
					
					*offScreenPtr = (Byte)dataDisplayPtr[dataValue];
					offScreenPtr++;

					}	// end "for (j=0;..."
				
				}	// end "else displayCode == 51"

			}	// end "if (fileInfoPtr->numberBytes == 1)"
	
				// This loop will draw the image lines for two byte data

		else if (fileInfoPtr->numberBytes == 2)
			{
			HUInt16Ptr ioBuffer2Ptr = (HUInt16Ptr)ioBuffer1Ptr;
			
			if (displayCode == 1)
				{
				for (j = 0; j < numberSamples; j += interval)
					{
					#if defined multispec_wxmac_alpha
						offScreenPtr++;
					#endif
					
					dataValue = ioBuffer2Ptr[j];
					if (dataValue > maxValue)
						dataValue = 0;
					
							// Red Value
					*offScreenPtr = dataDisplayPtr[dataValue];
					offScreenPtr++;
					
							// Green Value
					*offScreenPtr = dataDisplayPtr[dataValue];
					offScreenPtr++;
					
							// Blue Value
					*offScreenPtr = dataDisplayPtr[dataValue];
					offScreenPtr++;
					
					#if defined multispec_wxlin_alpha
								// Skip first (alpha) byte in wxBitmap
						offScreenPtr++;
					#endif

					}	// end "for (j=0;..."
			
				}	// end "if (displayCode == 1)"
	
			else	// displayCode == 51
				{
						// This is for one channel thematic images for wxWidgets
						// interface. The thematic value is loaded into the buffer.
				
				for (j = 0; j < numberSamples; j += interval)
					{
					dataValue = ioBuffer2Ptr[j];
					if (dataValue > maxValue)
						dataValue = 0;
					
					*offScreenPtr = (Byte)dataDisplayPtr[dataValue];
					offScreenPtr++;

					}	// end "for (j=0;..."
				
				}	// end "else displayCode == 51"

			}	// end "else if (fileInfoPtr->numberBytes == 2)"
	#endif

}	// end "Display1Channel8BitLine"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void Display2Channel8BitLine
//
//	Software purpose:	The purpose of this routine is to copy the input
//							line of data to the offscreen buffer for a 2-channel
//							color image.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None				
// 
// Called By:			DisplayColorImage in SDisplay.cpp
//
//	Coded By:			Larry L. Biehl			Date: 07/12/1988
//	Revised By:			Larry L. Biehl			Date: 12/12/2018

void Display2Channel8BitLine (
				UInt32								numberSamples,
				UInt32								interval,
				Boolean								bytesEqualOneFlag1,
				Boolean								bytesEqualOneFlag2,
				UInt16								backgroundValueCode,
				HFileIOBufferPtr					ioBuffer1Ptr,
				HFileIOBufferPtr					ioBuffer2Ptr,
				HUCharPtr							dataDisplay1Ptr,
				HUCharPtr							dataDisplay2Ptr,
				UInt32								maxValue1,
				UInt32								maxValue2,
				HUCharPtr							offScreenPtr)

{
				UInt32								backgroundValue,
														dataValue,
														j;


	for (j = 0; j < numberSamples; j += interval) 
		{
		dataValue = backgroundValue = (bytesEqualOneFlag1) ?
			(UInt32)ioBuffer1Ptr->data.onebyte[j] : (UInt32)ioBuffer1Ptr->data.twobyte[j];
		if (dataValue > maxValue1)
			dataValue = backgroundValue = 0;
		*offScreenPtr = dataDisplay1Ptr[dataValue];

		dataValue = (bytesEqualOneFlag2) ?
			(UInt32)ioBuffer2Ptr->data.onebyte[j] : (UInt32)ioBuffer2Ptr->data.twobyte[j];
		if (dataValue > maxValue2)
			dataValue = 0;
		backgroundValue += dataValue;
		*offScreenPtr += dataDisplay2Ptr[dataValue];

		if (backgroundValueCode && !backgroundValue) 
			{
			if (backgroundValueCode == 1)
				*offScreenPtr = 255;
			else	// backgroundValueCode == 2 
				*offScreenPtr = 0;

			}	// end "if (backgroundValueCode && !backgroundValue)"

		offScreenPtr++;

		}	// end "for (j=0; ..."  

}	// end "Display2Channel8BitLine"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void Display3Channel8BitLine
//
//	Software purpose:	The purpose of this routine is to copy the input
//							line of data to the offscreen buffer for a 3-channel
//							color image.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None				
// 
// Called By:			DisplayColorImage in SDisplay.cpp
//
//	Coded By:			Larry L. Biehl			Date: 07/12/1988
//	Revised By:			Larry L. Biehl			Date: 10/24/2009

void Display3Channel8BitLine (
				UInt32								numberSamples,
				UInt32								interval,
				Boolean								bytesEqualOneFlag1,
				Boolean								bytesEqualOneFlag2,
				Boolean								bytesEqualOneFlag3,
				UInt16								backgroundValueCode,
				HFileIOBufferPtr					ioBuffer1Ptr,
				HFileIOBufferPtr					ioBuffer2Ptr,
				HFileIOBufferPtr					ioBuffer3Ptr,
				HUCharPtr							dataDisplay1Ptr,
				HUCharPtr							dataDisplay2Ptr,
				HUCharPtr							dataDisplay3Ptr,
				UInt32								maxValue1,
				UInt32								maxValue2,
				UInt32								maxValue3,
				HUCharPtr							offScreenPtr)

{
	UInt32								backgroundValue,
											dataValue,
											j;


	for (j = 0; j < numberSamples; j += interval) 
		{
				// Blue

		dataValue = backgroundValue = (bytesEqualOneFlag1) ?
				(UInt32)ioBuffer1Ptr->data.onebyte[j] : 
																(UInt32)ioBuffer1Ptr->data.twobyte[j];
		if (dataValue > maxValue1)
			dataValue = backgroundValue = 0;
		*offScreenPtr = dataDisplay1Ptr[dataValue];

				// Green

		dataValue = (bytesEqualOneFlag2) ?
				(UInt32)ioBuffer2Ptr->data.onebyte[j] : 
																(UInt32)ioBuffer2Ptr->data.twobyte[j];
		if (dataValue > maxValue2)
			dataValue = 0;
		backgroundValue += dataValue;
		*offScreenPtr += dataDisplay2Ptr[dataValue];

				// Red

		dataValue = (bytesEqualOneFlag3) ?
				(UInt32)ioBuffer3Ptr->data.onebyte[j] : 
																(UInt32)ioBuffer3Ptr->data.twobyte[j];
		if (dataValue > maxValue3)
			dataValue = 0;
		backgroundValue += dataValue;
		*offScreenPtr += dataDisplay3Ptr [dataValue];

		if (backgroundValueCode && !backgroundValue) 
			{
			if (backgroundValueCode == 1)
				*offScreenPtr = 255;
			else	// backgroundValueCode == 2 
				*offScreenPtr = 0;

			}	// end "if (backgroundValueCode && !backgroundValue)"

		offScreenPtr++;

		}	// end "for (j=0; ..." 

}	// end "Display3Channel8BitLine"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoNextDisplayChannelEvent
//
//	Software purpose:	This routine handles mouse down events in the
//							grow box for non-graphics windows.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			main
//
//	Coded By:			Larry L. Biehl			Date: 08/01/2013
//	Revised By:			Larry L. Biehl			Date: 07/19/2019

void DoNextDisplayChannelEvent (
            WindowPtr                     window,
            char                          theChar)

{
   DisplaySpecsPtr               displaySpecsPtr;

   Handle                        displaySpecsHandle,
                                 windowInfoHandle;

   UInt32                        totalNumberChannels;
   SInt16                        windowType;

   Boolean                       callDisplayProcessorFlag = FALSE,
                                 shiftKeyFlag;


			// Note. Do not do the next channel if the gProcessCode is not 0. This implies
			// that a display event is already in process.
	
   if (window != NULL && gProcessorCode == 0)
      {
            // Get window handle and window type.

      windowInfoHandle = GetWindowInfoHandle (window);
      windowType = GetWindowType (windowInfoHandle);
      if ((windowType == kImageWindowType || windowType == kThematicWindowType) &&
														OffscreenImageMapExists (windowInfoHandle))
         {
         displaySpecsHandle = GetDisplaySpecsHandle (windowInfoHandle);
         displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (displaySpecsHandle);
         totalNumberChannels = GetTotalNumberOfChannels (windowInfoHandle);

         if (displaySpecsPtr != NULL &&
					(displaySpecsPtr->displayType == k1_ChannelThematicDisplayType ||
						displaySpecsPtr->displayType == k1_ChannelGrayLevelDisplayType) &&
								totalNumberChannels > 1)
            {
				#if defined multispec_mac || defined multispec_mac_swift
               if (theChar == kLeftArrowCharCode)
				#endif	// defined multispec_mac || defined multispec_mac_swift
				#if defined multispec_win || defined multispec_wx
               if (theChar == 0X25)
				#endif	// defined multispec_win || defined multispec_wx
               {
               if (displaySpecsPtr->channelNumber > 1)
                  {
                  displaySpecsPtr->channelNumber--;
                  callDisplayProcessorFlag = TRUE;

                  }	// end "if (displaySpecsPtr->channelNumber > 1)"

               }	// end "if (theChar == left arrow)"

				#if defined multispec_mac || defined multispec_mac_swift
               else if (theChar == kRightArrowCharCode)
				#endif	// defined multispec_mac || defined multispec_mac_swift
				#if defined multispec_win || defined multispec_wx
               else if (theChar == 0X27)
				#endif	// defined multispec_win || defined multispec_wx
                  {
                  if (displaySpecsPtr->channelNumber < (SInt16)totalNumberChannels)
                     {
                     displaySpecsPtr->channelNumber++;
                     callDisplayProcessorFlag = TRUE;

                     }	// end "if (displaySpecsPtr->channelNumber > 1)"

                  }	// end "else theChar == right arrow"

            else	// theChar == up arrow || down arrow
               {
							// The shift key needs to also be down for up or down arrow.

               #if defined multispec_mac
                  CGEventSourceStateID eventSource = 
															kCGEventSourceStateCombinedSessionState;
                  shiftKeyFlag = (CGEventSourceKeyState (eventSource, 0x38));
               #endif	// defined multispec_mac
               #if defined multispec_mac_swift
                  CGEventSourceStateID eventSource = 
															kCGEventSourceStateCombinedSessionState;
                  shiftKeyFlag = FALSE;
               #endif	// defined multispec_mac_swift
               #if defined multispec_win
                  shiftKeyFlag = (GetKeyState (VK_SHIFT) & 0x8000);
               #endif	// defined multispec_win
               #if defined multispec_wx
                  shiftKeyFlag = wxGetKeyState (WXK_SHIFT);
               #endif	// defined multispec_wx

               if (shiftKeyFlag)
                  {
						#if defined multispec_mac || defined multispec_mac_swift
                     if (theChar == kUpArrowCharCode)
						#endif	// defined multispec_mac || defined multispec_mac_swift
						#if defined multispec_win || defined multispec_wx
                     if (theChar == 0X26)
						#endif	// defined multispec_win || defined multispec_wx
                        {
                        if (displaySpecsPtr->channelNumber < 
																			(SInt16)totalNumberChannels)
                           {
                           displaySpecsPtr->channelNumber = 
																			(SInt16)totalNumberChannels;
                           callDisplayProcessorFlag = TRUE;

                           }	// end "if (displaySpecsPtr->channelNumber < ..."

                        }	// end "if (theChar == up arrow)"

                     else	// theChar == down arrow
                        {
                        if (displaySpecsPtr->channelNumber > 1)
                           {
                           displaySpecsPtr->channelNumber = 1;
                           callDisplayProcessorFlag = TRUE;

                           }	// end "if (displaySpecsPtr->channelNumber > 1)"

                        }	// end "else theChar == down arrow"

                  }	// end "if (shiftKeyFlag)"

               }	// end "else theChar == kUpArrowCharCode || kDownArrowCharCode"

            if (callDisplayProcessorFlag)
               {
               gOutputForce1CodeSetting = 0;
               gCallProcessorDialogFlag = FALSE;
					#if defined multispec_mac
                  SInt32 menuData;
                  menuData = (kProcessorMenuID << 16) + kDisplaysMItem;
                  Menus (menuData);
					#endif	// defined multispec_mac
					#if defined multispec_win || defined multispec_wx
                  DisplayImage ();
					#endif	// defined multispec_win || defined multispec_wx

               gCallProcessorDialogFlag = TRUE;
               gOutputForce1CodeSetting = 0x0001;

               }	// end "if (callDisplayProcessorFlag)"

            }	// end "if (displaySpecsPtr != NULL && ..."

         }	// end "if (windowType == kImageWindowType && ..."

      }	// end "if (window != NULL)"

}	// end "DoNextDisplayChannelEvent"



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawBitsColorPopUp
//
//	Software purpose:	The purpose of this routine is to draw the bits of color
//							prompt and bits of color selection for bits of color
//							pop up menu.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 07/01/1990
//	Revised By:			Larry L. Biehl			Date: 10/30/1990	

PascalVoid DrawBitsColorPopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.									

	DrawPopUpMenuItem (dialogPtr, 
								itemNumber,
								gPopUpBitsOfColorMenu, 
								gBitsOfColorSelection, 
								TRUE);

}	// end "DrawBitsColorPopUp" 
#endif	// defined multispec_mac	



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawDisplayTypePopUp
//
//	Software purpose:	The purpose of this routine is to draw the display type
//							prompt and display type selection for display type
//							pop up menu.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/28/1990
//	Revised By:			Larry L. Biehl			Date: 10/30/1990	

PascalVoid DrawDisplayTypePopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber)

{

			// Use the generic pop up drawing routine.									

	DrawPopUpMenuItem (dialogPtr, 
								itemNumber,
								gPopUpDisplayTypeMenu, 
								gDisplayTypeMenuSelection, 
								TRUE);

}	// end "DrawDisplayTypePopUp" 
#endif	// defined multispec_mac	


/*
#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawEnhancementPopUp
//
//	Software purpose:	The purpose of this routine is to draw the enhancement
//							prompt and enhancement selection for the enhancement
//							pop up menu.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/01/1990
//	Revised By:			Larry L. Biehl			Date: 10/30/1990	

PascalVoid DrawEnhancementPopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.									

	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpEnhancementMenu, 
								gEnhancementSelection, 
								TRUE);

}	// end "DrawEnhancementPopUp" 
#endif	// defined multispec_mac	
*/


#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawMinMaxPopUp
//
//	Software purpose:	The purpose of this routine is to draw the enhancement
//							prompt and enhancement selection for the enhancement
//							pop up menu.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/26/2001
//	Revised By:			Larry L. Biehl			Date: 10/26/2001	

PascalVoid DrawMinMaxPopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.									

	DrawPopUpMenuItem (dialogPtr, 
								itemNumber,
								gPopUpMinMaxMenu, 
								gEnhanceMinMaxMenuSelection, 
								TRUE);

}	// end "DrawMinMaxPopUp" 
#endif	// defined multispec_mac	



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawStretchPopUp
//
//	Software purpose:	The purpose of this routine is to draw the enhancement
//							prompt and enhancement selection for the enhancement
//							pop up menu.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/26/2001
//	Revised By:			Larry L. Biehl			Date: 10/26/2001	

PascalVoid DrawStretchPopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.									

	DrawPopUpMenuItem (dialogPtr, 
								itemNumber,
								gPopUpStretchMenu, 
								gEnhanceStretchSelection, 
								TRUE);

}	// end "DrawStretchPopUp" 
#endif	// defined multispec_mac	



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawZeroAsPopUp
//
//	Software purpose:	The purpose of this routine is to draw the enhancement
//							prompt and enhancement selection for the enhancement
//							pop up menu.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/26/2001
//	Revised By:			Larry L. Biehl			Date: 10/26/2001	

PascalVoid DrawZeroAsPopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.									

	DrawPopUpMenuItem (dialogPtr, 
								itemNumber,
								gPopUpTreatZeroAsMenu, 
								gEnhanceZeroAsSelection, 
								TRUE);

}	// end "DrawZeroAsPopUp" 
#endif	// defined multispec_mac	


/*
#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 EnhancementPopUpMenu
//
//	Software purpose:	The purpose of this routine is to handle a 
//							mousedown in the enhancement pop up menu location.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/01/1990
//	Revised By:			Larry L. Biehl			Date: 07/14/1998

SInt16 EnhancementPopUpMenu (
				DialogPtr							dialogPtr, 
				SInt16								dialogItem, 
				SInt16								rgbColors, 
				SInt16								displayType,
				Handle								okHandle, 
				SInt16*								minMaxSelectionPtr, 
				SInt32*								minMaxValuesPtr, 
				SInt16*								percentTailsClippedPtr, 
				Boolean*								zeroAsBackgroundFlagPtr)

{
	SInt16								channels[3],
											itemHit,
											minMaxPopupCode;


			// Set up menu.																		

	EnableMenuItem (gPopUpEnhancementMenu, 1);
	EnableMenuItem (gPopUpEnhancementMenu, 2);
	EnableMenuItem (gPopUpEnhancementMenu, 4);
	EnableMenuItem (gPopUpEnhancementMenu, 5);
	EnableMenuItem (gPopUpEnhancementMenu, 6);

	CheckMenuItem (gPopUpEnhancementMenu, gEnhancementSelection, TRUE);

	minMaxPopupCode = GetMinMaxPopupCode (*minMaxSelectionPtr,
														*percentTailsClippedPtr);

	CheckMenuItem (gPopUpEnhancementMenu, minMaxPopupCode+4, TRUE);

	if (*zeroAsBackgroundFlagPtr)
		{
		CheckMenuItem (gPopUpEnhancementMenu, 8, TRUE);
		CheckMenuItem (gPopUpEnhancementMenu, 9, FALSE);

		}	// end "if (*zeroAsBackgroundFlagPtr)" 

	else	// !*zeroAsBackgroundFlagPtr 
		{
		CheckMenuItem (gPopUpEnhancementMenu, 8, FALSE);
		CheckMenuItem (gPopUpEnhancementMenu, 9, TRUE);

		}	// end "else !*zeroAsBackgroundFlagPtr" 

	itemHit = StandardPopUpMenu (dialogPtr, 
											dialogItem-1, 
											dialogItem, 
											gPopUpEnhancementMenu, 
											gEnhancementSelection, 
											kPopUpEnhancementMenuID);
											
			// Remove the check marks.															

	CheckMenuItem (gPopUpEnhancementMenu, gEnhancementSelection, FALSE);
	CheckMenuItem (gPopUpEnhancementMenu, minMaxPopupCode+4, FALSE);
	CheckMenuItem (gPopUpEnhancementMenu, 8, FALSE);
	CheckMenuItem (gPopUpEnhancementMenu, 9, FALSE);

			// Get the item hit.																	
					
	if (itemHit >= 1 && itemHit <= 2)
		gEnhancementSelection = itemHit;

	else if (itemHit == 4)
		{
		*minMaxSelectionPtr = kPerChannelMinMax;
		*percentTailsClippedPtr = 2;

		}	// end "else if (itemHit == 4)"

	else if (itemHit == 5)
		*minMaxSelectionPtr = kEntireDataRange;

	else if (itemHit == 6)
		{
		HiliteControl ((ControlHandle)okHandle, 255);								

		SInt16 itemNumber = 32;
		if (displayType <= k1_ChannelGrayLevelDisplayType)
			itemNumber = 31;

		channels[0] = (SInt16)GetDItemValue (dialogPtr, itemNumber);
		channels[1] = (SInt16)GetDItemValue (dialogPtr, 33);
		channels[2] = (SInt16)GetDItemValue (dialogPtr, 34);

		MinMaxEnhancementDialog (channels,
											rgbColors,
											displayType,
											percentTailsClippedPtr, 
											minMaxSelectionPtr, 
											minMaxValuesPtr);
											
		HiliteControl ((ControlHandle)okHandle, 0);

		}	// end "if (itemHit == 6)"

	else if (itemHit == 8)
		*zeroAsBackgroundFlagPtr = TRUE;

	else if (itemHit == 9)
		*zeroAsBackgroundFlagPtr = FALSE;

			// Return the selection that was made.											

	return (itemHit);

}	// end "EnhancementPopUpMenu" 
#endif	// defined multispec_mac	
*/


#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 EnhanceMinMaxPopUpMenu
//
//	Software purpose:	The purpose of this routine is to handle a 
//							mousedown in the enhancement pop up menu location.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/25/2001
//	Revised By:			Larry L. Biehl			Date: 11/13/2019

SInt16 EnhanceMinMaxPopUpMenu (
				DialogPtr							dialogPtr,
				SInt16								dialogItem,
				SInt16								rgbColors,
				SInt16								displayType,
				SInt16								numberLevels,
				Handle								okHandle,
				SInt16								minMaxSelection,
				double*								minMaxValuesPtr,
				SInt16*								percentTailsClippedPtr)

{
	SInt16								channels[3],
											itemHit,
											minMaxPopupCode;


	minMaxPopupCode = GetMinMaxPopupCode (minMaxSelection,
														*percentTailsClippedPtr);

	CheckMenuItem (gPopUpMinMaxMenu, minMaxPopupCode + 1, TRUE);

	if (gAppearanceManagerFlag)
	  itemHit = GetDItemValue (dialogPtr, dialogItem);

	else	// !gAppearanceManagerFlag)
	  itemHit = StandardPopUpMenu (dialogPtr,
												dialogItem - 1,
												dialogItem,
												gPopUpMinMaxMenu,
												minMaxPopupCode + 1,
												kPopUpMinMaxMenuID);

			// Remove the check marks.															

	CheckMenuItem (gPopUpMinMaxMenu, minMaxPopupCode + 1, FALSE);

			// Get the item hit.																	

	if (itemHit == kPerChannelMinMaxNoClip) 
		{
		minMaxSelection = kPerChannelMinMax;
		*percentTailsClippedPtr = 0;

		}	// end "else if (itemHit == kPerChannelMinMaxNoClip)"															

	else if (itemHit == kPerChannelMinMax) 
		{
		minMaxSelection = kPerChannelMinMax;
		*percentTailsClippedPtr = 2;

		}	// end "else if (itemHit == kPerChannelMinMaxNoClip)"

	else if (itemHit == kEntireDataRange)
		minMaxSelection = kEntireDataRange;

	else if (itemHit == kUserSpecified) 
		{
		HiliteControl ((ControlHandle)okHandle, 255);

		SInt16 itemNumber = 34;
		if (displayType <= k1_ChannelGrayLevelDisplayType)
			itemNumber = 33;

		channels[0] = (SInt16)GetDItemValue (dialogPtr, itemNumber);
		channels[1] = (SInt16)GetDItemValue (dialogPtr, 35);
		channels[2] = (SInt16)GetDItemValue (dialogPtr, 36);

		MinMaxEnhancementDialog (channels,
										 rgbColors,
										 displayType,
										 numberLevels,
										 percentTailsClippedPtr,
										 &minMaxSelection,
										 minMaxValuesPtr);

		HiliteControl ((ControlHandle)okHandle, 0);

		if (!gAppearanceManagerFlag)
			SetDLogControlHilite (dialogPtr, 2, 0);

				// Force the main dialog box to be redrawn.

		InvalWindowRect (GetDialogWindow (dialogPtr),
								GetPortBounds (GetDialogPort (dialogPtr), 
								&gTempRect));

		}	// end "if (itemHit == kUserSpecified)"

	else if (itemHit == kThematicDefault) 
		{
		minMaxSelection = kThematicDefault;

		}	// end "else if (itemHit == kThematicDefault)"

	if (itemHit != 0)
		itemHit = minMaxSelection;

			// Return the selection that was made.											

	return (itemHit);

}	// end "EnhanceMinMaxPopUpMenu" 
#endif	// defined multispec_mac	



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean EqualAreaDataToDisplayLevels
//
//	Software purpose:	The purpose of this routine is to set the vector 
//		which indicates how to map the image data values to the palette levels
//		using equal count of pixels in each display level as the criterion.
//		This routine will use the histogram of the image and the number of 
//		display levels as the input.
//
//	Functional description:	The steps in this function follow.
//			1) Determine the number of image data pixels to be displayed within each 
//				display level based on the histogram and the number of display 
//				values in the pallete.  This will be a real number value.
//
//			2) For each possible image data value, determine the display level to
//				use on the computer screen.
//
//				Also if the number of display levels is greater than the total number 
//				of image data values, the image data values should be spread
//				out over the display level range.  For example, a display levels 
//				range could be 0, 2, 4, 6, ....252, 254.
//
//	Parameters in:			
//
//	Parameters out:	None
//
// Value Returned:  	True if vector loaded okay.
//							False if vector did not load okay.
//
// Called By:			DisplayImage in SDisplay.cpp
//
//	Coded By:			Larry L. Biehl			Date:	07/03/1990
//	Revised By:			Larry L. Biehl			Date: 05/08/2020

Boolean EqualAreaDataToDisplayLevels (
				HistogramSpecsPtr					histogramSpecsPtr,
				HUCharPtr							dataDisplayPtr,
				DisplaySpecsPtr					displaySpecsPtr,
				UInt32								dataDisplayVectorLength,
				SInt16								numberChannels,
				UInt16*								channelsPtr)

{
	double								displayAreaInterval,
											floatMultiplier,
											nextDisplayLevelChange;

	HistogramSummaryPtr				histogramSummaryPtr;

	HUInt32Ptr							histogramArrayPtr;

	SInt32								dataLevel,
											displayLevelInterval,
											firstIndexForBin,
											lastDataLevel,
											smallestIndexInterval,
											maxValueIndex,
											minValueIndex;

	UInt32								displayLevel,
											displayValue,
											highLimitDisplayLevel,
											highValue,
											initialValue,
											invertValueIndex,
											lastDisplayLevel,
											lastValue,
											lowLimitDisplayLevel,
											numberDataValues,
											numberLevels,
											numberValuesInDisplayLevel,
											rgbMultiplier,
											rgbOffset,
											totalPixels;

	SInt16								channel,
											classForMaxDataValue,
											//classForMaxThematicValue,
											classForMinDataValue;
											//classForMinThematicValue;

	Boolean								continueFlag = FALSE;


			// Check input variables.															

	if (histogramSpecsPtr == NULL || dataDisplayPtr == NULL)
																							return (FALSE);
																										
	continueFlag = TRUE;
	histogramSummaryPtr = histogramSpecsPtr->histogramSummaryPtr;
	histogramArrayPtr = histogramSpecsPtr->histogramArrayPtr;
	totalPixels = 0;

	if (continueFlag) 
		{
				// Initialize local variables.												

		numberLevels = displaySpecsPtr->numberLevels;
		numberDataValues = gImageWindowInfoPtr->numberBins;

		if (displaySpecsPtr->pixelSize <= 8) 
			{
			////rgbOffset = 1;
			rgbOffset = 0;
			initialValue = rgbOffset;
			floatMultiplier = 1;
			highValue = numberDataValues;

			}	// end "if (displaySpecsPtr->pixelSize <= 8)"

		else	// displaySpecsPtr->pixelSize > 8 
			{
			rgbOffset = 0;

			if (numberLevels > 1) 
				{
				initialValue = 0;

				if (displaySpecsPtr->pixelSize == 16)
					highValue = 31;

				else	// displaySpecsPtr->pixelSize >= 24
					highValue = 255;

				floatMultiplier = (double)highValue / (double)(numberLevels - 1);

				}	// end "if (numberLevels > 1)"

			else	// numberLevels == 1
				{
				if (displaySpecsPtr->pixelSize == 16)
					initialValue = 15;

				else	// displaySpecsPtr->pixelSize >= 24
					initialValue = 127;

				highValue = initialValue;

				floatMultiplier = 1;

				}	// end "else numberLevels == 1"

			}	// end "else displaySpecsPtr->pixelSize > 8"

		rgbMultiplier = 1;
		/*		
		if (displaySpecsPtr->minMaxCode == kEntireDataRange)
			{
					// Set min-max for entire range option.								

			minValueIndex = 0;
			maxValueIndex = gImageWindowInfoPtr->numberBins - 1;

			}	// end "if (displaySpecsPtr->minMaxCode == kEntireDataRange)" 
		*/
				// Loop through the channels requested.									

		for (channel = 0; channel < numberChannels; channel++) 
			{
			continueFlag = GetHistogramVectorForChannel (histogramSpecsPtr,
																		  &channelsPtr[channel],
																		  1);

			if (!continueFlag)
				break;

			GetMinMaxValuesIndices (displaySpecsPtr,
											  histogramSpecsPtr,
											  &histogramSummaryPtr[channelsPtr[channel]],
											  channel,
											  channelsPtr[channel],
											  TRUE,
											  gImageFileInfoPtr->signedValueOffset,
											  numberDataValues,
											  (UInt16)numberLevels,
											  &minValueIndex,
											  &maxValueIndex,
											  //&classForMinThematicValue,
											  //&classForMaxThematicValue,
											  &classForMinDataValue,
											  &classForMaxDataValue);

					// Set the multiplier for this channel to be used to determine 
					// the look up table for the data values to color value table.	

			if (displaySpecsPtr->displayType != kSideSideChannelDisplayType &&
								channel > 0 &&
											displaySpecsPtr->pixelSize <= 8)
				rgbMultiplier *= numberLevels;

					// Get the number of pixels within the requested range.			

			totalPixels = 0;
			for (dataLevel = minValueIndex; dataLevel <= maxValueIndex; dataLevel++)
				totalPixels += histogramArrayPtr[dataLevel];

					// If the difference of the max and min value is 1, then this
					// indicates that all of the pixels are of one value. Do not
					// flag this as an error.

			if (totalPixels <= 0) 
				{
				continueFlag = FALSE;
				break;

				}	// end "if (totalPixels <= 0)"

					// Now get the number of pixels to be displayed for each 		
					// display level.																

			if (displaySpecsPtr->displayType == k1_ChannelThematicDisplayType) 
				{
				displayAreaInterval = totalPixels;
				if (numberLevels >= 3)
					displayAreaInterval = (double)totalPixels / (numberLevels - 2);

				}	// end "if (...->displayType == k1_ChannelThematicDisplayType)"

			else	// displaySpecsPtr->displayType != k1_ChannelThematicDisplayType
				 displayAreaInterval = (double)totalPixels / numberLevels;

					// Initialize the begin display level.  The first level in the	
					// palette is reserved white and the last level in the palette	
					// is reserved for black.  Therefore there are numberLevels + 	
					// 2 in the entire palette.  The first level will be skipped 	
					// in the image drawing programs because of the 'rgbOffset' 	
					// parameter used here.														

			nextDisplayLevelChange = displayAreaInterval;

			numberValuesInDisplayLevel = 0;
			lastDataLevel = dataDisplayVectorLength - 1;

			if (displaySpecsPtr->displayType == kSideSideChannelDisplayType)
				invertValueIndex = 2;
			else	// displaySpecsPtr->displayType != kSideSideChannelDisplayType
				invertValueIndex = displaySpecsPtr->minMaxPointers[channel];

					// Note that inverted data value for thematic type windows are
					// handled in the order of the colors in the palette.

			if (displaySpecsPtr->displayType == k1_ChannelThematicDisplayType ||
											!displaySpecsPtr->invertValuesFlag[invertValueIndex]) 
				{
				displayLevel = 0;
				displayLevelInterval = 1;
				displayValue = initialValue;
				lastDisplayLevel = numberLevels - 1;
				lastValue = highValue;

				displaySpecsPtr->thematicBinWidth =
										histogramSummaryPtr[channelsPtr[channel]].binFactor;

				lowLimitDisplayLevel = 0;

				highLimitDisplayLevel = numberLevels - 1;
				if (displaySpecsPtr->displayType == k1_ChannelThematicDisplayType)
					highLimitDisplayLevel--;

				}	// end "if (...->displayType == k1_ChannelThematicDisplayType || ..."

			else	// ...->displayType != k1_ChannelThematicDisplayType &&
					//		...->invertValuesFlag[invertValueIndex]
				{
				displayLevel = numberLevels - 1;
				displayLevelInterval = -1;
				displayValue = displayLevel + initialValue;
				lastDisplayLevel = 0;
				lastValue = 0;

				lowLimitDisplayLevel = 1;
				if (displaySpecsPtr->displayType == k1_ChannelThematicDisplayType)
					lowLimitDisplayLevel++;

				highLimitDisplayLevel = numberLevels;

				}	// end "else ...->displayType != k1_ChannelThematicDisplayType && ..."

					// Set the display level for the lower saturated data levels.

			for (dataLevel = 0; dataLevel < minValueIndex; dataLevel++)
				dataDisplayPtr[dataLevel] = (UInt8)displayValue;

			if (displaySpecsPtr->pixelSize <= 8) 
				{
				if (displaySpecsPtr->displayType == k1_ChannelThematicDisplayType) 
					{
					if (numberLevels >= 2)
						displayLevel += displayLevelInterval;

					displayValue = displayLevel * rgbMultiplier + rgbOffset;

					}	// end "if (...->displayType == k1_ChannelThematicDisplayType)"

						// These "displayValues" need to range between 0 to "numberLevels-1"

				firstIndexForBin = minValueIndex;
				smallestIndexInterval = SInt32_MAX;
				for (dataLevel = minValueIndex; dataLevel <= maxValueIndex; dataLevel++) 
					{
					dataDisplayPtr[dataLevel] = (UInt8)displayValue;
					numberValuesInDisplayLevel += histogramArrayPtr[dataLevel];
					while (numberValuesInDisplayLevel >= nextDisplayLevelChange &&
									displayLevel >= lowLimitDisplayLevel &&
												displayLevel < highLimitDisplayLevel) 
						{
						if (numberLevels >= 2)
								displayLevel += displayLevelInterval;

						displayValue = displayLevel * rgbMultiplier + rgbOffset;
						nextDisplayLevelChange += displayAreaInterval;

						if (dataLevel > firstIndexForBin) 
							{
							smallestIndexInterval = MIN (smallestIndexInterval,
																	dataLevel - firstIndexForBin);
							firstIndexForBin = dataLevel;

							}	// end "if (dataLevel > firstDataLevelForBin)"

						}	// end "while (numberValuesInDisplayLevel > ..." 

					}	// end "for (dataLevel=minValueIndex+1; ..." 

				lastValue = lastDisplayLevel * rgbMultiplier + rgbOffset;

				if (displaySpecsPtr->displayType == k1_ChannelThematicDisplayType)
					displaySpecsPtr->smallestIndexInterval = smallestIndexInterval;

				}	// end "if (displaySpecsPtr->pixelSize <= 8)"

			else	// displaySpecsPtr->pixelSize > 8 
				{
						// These "displayValues" need to range between 0 to 31 for 16-bit
						// color display and 0 to 255 for 24 bit color display. Missing 
						// number of levels will just be values skipped between.

				for (dataLevel = minValueIndex; dataLevel <= maxValueIndex; dataLevel++) 
					{
					dataDisplayPtr[dataLevel] = (UInt8)displayValue;
					numberValuesInDisplayLevel += histogramArrayPtr[dataLevel];
					while (numberValuesInDisplayLevel >= nextDisplayLevelChange &&
									displayLevel >= lowLimitDisplayLevel &&
												displayLevel < highLimitDisplayLevel) 
						{
						displayLevel += displayLevelInterval;
						displayValue = (UInt32)(displayLevel * floatMultiplier);
						nextDisplayLevelChange += displayAreaInterval;

						}	// end "while (numberValuesInDisplayLevel > ..." 

					}	// end "for (dataLevel=minValueIndex; ..."

				}	// end "else displaySpecsPtr->pixelSize > 8"

					// Set the display level for the upper saturated data levels.	

			for (dataLevel = maxValueIndex + 1; dataLevel <= lastDataLevel; dataLevel++)
				dataDisplayPtr[dataLevel] = (UInt8)lastValue;

					// Set up for treating data value of '0' as a background value	
					// for one channel displays.	

			if (displaySpecsPtr->backgroundValueCode == 1)
				dataDisplayPtr[0] = (UChar)(numberLevels + 1);

			else if (displaySpecsPtr->backgroundValueCode == 2)
				dataDisplayPtr[0] = 0;

					// Get the starting point for the look up table for the next channel.																		

			dataDisplayPtr = (HUCharPtr)&dataDisplayPtr[dataDisplayVectorLength];

					// Change the rgbOffset and initialValue for the rest of the
					// channels for 8-bit pixel sizes.

			if (displaySpecsPtr->pixelSize <= 8) 
				{
				if (displaySpecsPtr->displayType != kSideSideChannelDisplayType)
					rgbOffset = 0;

				initialValue = rgbOffset;

				}	// end "if (displaySpecsPtr->pixelSize <= 8)"

			}	// end "for (channel=0; channel<numberChannels; ...)" 

		}	// end "if (continueFlag)" 

	if (totalPixels == 0)
		DisplayAlert (kErrorAlertID,
							kStopAlert,
							kAlertStrID,
							IDS_Alert18,
							0,
							NULL);

	return (continueFlag);

}	// end "EqualAreaDataToDisplayLevels" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void FillDataToDisplayLevels
//
//	Software purpose:	The purpose of this routine is to set the vector 
//		which indicates how to map the image data values to the palette levels.
//		This routine will use the histogram of the image and the number of 
//		display levels as the input.
//
//	Functional description:	The steps in this function follow.
//
//			1) Determine if the gToDisplayLevels.upToDateFlag is FALSE.  If it
//				is TRUE, exit the function.  If it is FALSE fill the gToDisplayLevels
//				structure for channel channelNumber.
//
//			2) Determine the image data interval to be displayed within each 
//				display level based on the histogram minimum and maximum for the
//				specified channel and the number of display values in the pallete.  
//				This will be a real number value.
//
//			3) For each possible image data value, determine the display level to
//				use on the computer screen.  Assume image data values from 0 to
//				the minimum histogram value will be displayed as the "darkest" 
//				display value and image data values from the maximum histogram value
//				will be displayed as the "brightest" display value.
//
//				For now assume an equal interval spacing of image data values to
//				display levels.  Also if the number of display levels is greater than
//				the image data value range, the image data values should be spread
//				out over the display level range.  For example, a data value
//				range of 101 to 228 should be displayed as display levels 0, 2, 4,
//				6, ....252, 254.
//
//	Parameters in:		&gActiveImageWindow
//							channelNumber
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			DisplayImage in SDisplay.cpp
//
//	Coded By:			Larry L. Biehl			Date:	04/21/1988
//	Revised By:			Larry L. Biehl			Date: 05/31/2020

Boolean FillDataToDisplayLevels (
				HistogramSpecsPtr					histogramSpecsPtr,
				HUCharPtr							dataDisplayPtr,
				DisplaySpecsPtr					displaySpecsPtr,
				UInt32								dataDisplayVectorLength,
				SInt16								numberChannels,
				UInt16*								channelsPtr)

{
	double								floatDisplayLevel,
											floatMultiplier,
											displayInterval,
											thematicInterval,
											thematicRange;

	HistogramSummaryPtr				histogramSummaryPtr;

	SInt32								dataLevel,
											maxBinIndex,
											minBinIndex,
											startValue;

	UInt32								invertValueIndex,
											numberBins;

	SInt16								channel,
											channelNumberIndex,
											classForMaxDataValue,
											//classForMaxThematicValue,
											classForMinDataValue;
											//classForMinThematicValue;

	UInt16								displayLevel,
											indexForMaxDataValue,
											indexForMinDataValue,
											initialValue,
											numberLevels,
											rgbMultiplier;
											//rgbOffset;

	Boolean								invertValuesFlag;


			// Check input variables.

	if (displaySpecsPtr == NULL || dataDisplayPtr == NULL)
																							return (FALSE);

	if (histogramSpecsPtr == NULL && displaySpecsPtr->minMaxCode == kPerChannelMinMax)
																							return (FALSE);

			// Initialize local variables.

	numberLevels = (UInt16)displaySpecsPtr->numberLevels;

	numberBins = gImageWindowInfoPtr->numberBins;
	if (gImageWindowInfoPtr->numberBytes > 2)
		numberBins = histogramSpecsPtr->maxNumberBins;

	histogramSummaryPtr = histogramSpecsPtr->histogramSummaryPtr;

			// Get the min-max to be used for the data range depending upon the
			// user request.

	if (displaySpecsPtr->pixelSize <= 8)
		{
		//rgbOffset = 1;
		//rgbOffset = 0;
		floatMultiplier = 1;

		}	// end "if (displaySpecsPtr->pixelSize <= 8)"

	else	// displaySpecsPtr->pixelSize > 8 
		{
		//rgbOffset = 0;

		if (numberLevels > 1)
			{
			if (displaySpecsPtr->pixelSize == 16)
				floatMultiplier = 31. / (double)(numberLevels - 1);

			else	// displaySpecsPtr->pixelSize >= 24
				floatMultiplier = 255. / (double)(numberLevels - 1);

			}	// end "if (numberLevels > 1)"

		else	// numberLevels == 1
			floatMultiplier = 1;

		}	// end "else displaySpecsPtr->pixelSize > 8 "

	rgbMultiplier = 1;

	displayInterval = 0;

	for (channel=0; channel<numberChannels; channel++)
		{
				// Set initial value
				// Note that inverted data value for thematic type windows are
				// handled in the order of the colors in the palette.

		invertValuesFlag = FALSE;
		if (displaySpecsPtr->displayType != k1_ChannelThematicDisplayType)
			{
			if (displaySpecsPtr->displayType == kSideSideChannelDisplayType)
				invertValueIndex = 2;
			else	// displaySpecsPtr->displayType != kSideSideChannelDisplayType
				invertValueIndex = displaySpecsPtr->minMaxPointers[channel];

			invertValuesFlag = displaySpecsPtr->invertValuesFlag[invertValueIndex];

			}	// end "if (...->displayType != k1_ChannelThematicDisplayType)"

		if (displaySpecsPtr->pixelSize <= 8)
			{
			initialValue = 0;

			if (invertValuesFlag)
				initialValue = numberLevels - 1;

			}	// end "if (displaySpecsPtr->pixelSize <= 8)"

		else	// displaySpecsPtr->pixelSize > 8
			{
			if (numberLevels > 1)
				{
				initialValue = 0;
				if (invertValuesFlag)
					initialValue = numberLevels - 1;

				}	// end "if (numberLevels > 1)"

			else	// numberLevels == 1
				{
				if (displaySpecsPtr->pixelSize == 16)
					initialValue = 15;

				else	// displaySpecsPtr->pixelSize >= 24
					initialValue = 127;

				}	// end "else numberLevels == 1"

			}	// end "else displaySpecsPtr->pixelSize > 8 "

		channelNumberIndex = channelsPtr[channel];

		GetMinMaxValuesIndices (displaySpecsPtr,
										 histogramSpecsPtr,
										 &histogramSummaryPtr[channelNumberIndex],
										 channel,
										 channelNumberIndex,
										 FALSE,
										 gImageFileInfoPtr->signedValueOffset,
										 numberBins,
										 numberLevels,
										 &minBinIndex,
										 &maxBinIndex,
										 //&classForMinThematicValue,
										 //&classForMaxThematicValue,
										 &classForMinDataValue,
										 &classForMaxDataValue);

				// Set the multiplier for this channel to be used to determine
				// the look up table for the data values to color value table.

		if (displaySpecsPtr->displayType != kSideSideChannelDisplayType &&
								channel > 0 &&
												displaySpecsPtr->pixelSize <= 8)
			rgbMultiplier *= numberLevels;

				// Initialize the begin display level.  The first level in the
				// palette is black and the last level is white.

		floatDisplayLevel = (double)initialValue;
		displayLevel = initialValue;

		for (dataLevel = 0; dataLevel <= minBinIndex; dataLevel++)
			dataDisplayPtr[dataLevel] = (UInt8)displayLevel;

		if (displaySpecsPtr->displayType == k1_ChannelThematicDisplayType)
			{
					// Set floatDisplayLevel to represent display class for the 
					// minBinIndex. It will be updated below if it needs to be for cases 
					// where minimum data value in the image is not less than the 
					// requested minimum thematic value.

			if (numberLevels >= 2)
				floatDisplayLevel += 1;

			if (numberLevels >= 3)
				{
				thematicRange = displaySpecsPtr->thematicTypeMinMaxValues[1] -
													displaySpecsPtr->thematicTypeMinMaxValues[0];

				displayInterval = (double)(numberLevels - 2) / thematicRange;
				displayInterval /= histogramSummaryPtr[channelNumberIndex].binFactor;

				thematicInterval = thematicRange / (numberLevels - 2);

				floatDisplayLevel = 1;

				if (gImageWindowInfoPtr->localMaxNumberBytes >= 4)
					{
					displayInterval = 1;
					maxBinIndex = numberLevels - 1;

					}	// end "if (gImageWindowInfoPtr->localMaxNumberBytes >= 4)"

				if (histogramSummaryPtr[channelNumberIndex].binType == kBinWidthOfOne)
					{
					if (numberLevels - 2 == thematicRange + 1)
						thematicInterval = 1;

					else if (numberLevels - 2 == thematicRange)
						{
						thematicInterval = 1;

						if (gImageWindowInfoPtr->localMaxNumberBytes >= 4)
							maxBinIndex = numberLevels - 2;

						}	// end "else if (numberLevels-2 == thematicRange)"

					}	// end "if (...[channelNumberIndex].binType == kBinWidthOfOne)"

				displaySpecsPtr->thematicBinWidth = 1 / thematicInterval;

				if (gImageWindowInfoPtr->localMaxNumberBytes >= 4)
					minBinIndex = 1;

				}	// end "if (numberLevels >= 3)"

			for (dataLevel=minBinIndex; dataLevel<=maxBinIndex; dataLevel++)
				{
				dataDisplayPtr[dataLevel] = (UInt8)((UInt16)(MAX (0, floatDisplayLevel)));
				floatDisplayLevel += displayInterval;
				floatDisplayLevel = MIN (floatDisplayLevel, 255);

				}	// end "for (dataLevel=minValueIndex; ......

					// Set display level for those data values larger than the maximum
					// requested thematic value.

			displayLevel = numberLevels - 1;

			}	// end "displaySpecsPtr->displayType == k1_ChannelThematicDisplayType"

		else	// displaySpecsPtr->displayType != k1_ChannelThematicDisplayType
			{
			if (numberLevels > 1)
				{
				if ((SInt32)numberLevels > (maxBinIndex - minBinIndex))
					{
					displayInterval = (double)(numberLevels - 1) / 2;
					if (maxBinIndex > minBinIndex)
						displayInterval = (double)(numberLevels - 1) /
														((double)maxBinIndex - (double)minBinIndex);

					}	// end "if ((SInt32)numberLevels > (maxBinIndex - minBinIndex))"

				else	// numberLevels <= (maxValue - minValue)
				  displayInterval = (double)numberLevels /
						((double)maxBinIndex - (double)minBinIndex + 1);

				if (invertValuesFlag)
				  displayInterval = -displayInterval;

				}	// end "if (numberLevels > 1)"

			if (displaySpecsPtr->pixelSize <= 8)
				{
				if (minBinIndex == maxBinIndex)
					{
							// For this case force the data value in the middle of the 
							// minimum value and the maximum value to be in the middle color 
							// range. Note that for this case the for loop after this will 
							// skipped.

					floatDisplayLevel += displayInterval;
					displayLevel = MIN ((UInt16)(floatDisplayLevel) * rgbMultiplier, 255);
					dataDisplayPtr[minBinIndex] = (UInt8)displayLevel;

					floatDisplayLevel += displayInterval;

					}	// end "if (minBinIndex == maxBinIndex)"

				for (dataLevel=minBinIndex+1; dataLevel<=maxBinIndex; dataLevel++)
					{
					floatDisplayLevel += displayInterval;
					displayLevel = MIN ((UInt16)(floatDisplayLevel) * rgbMultiplier, 255);
					dataDisplayPtr[dataLevel] = (UInt8)displayLevel;

					}	// end "for (dataLevel=minValueIndex+1; ......

				displayLevel = MIN ((UInt16)(floatDisplayLevel) * rgbMultiplier, 255);

				}	// end "if (displaySpecsPtr->pixelSize <= 8)"

			else	// displaySpecsPtr->pixelSize > 8
				{
				if (minBinIndex == maxBinIndex)
					{
							// For this case force the data value in the middle of the 
							// minimum value and the maximum value to be in the middle color 
							// range. Note that for this case the for loop after this will 
							// skipped.

					floatDisplayLevel += displayInterval;
					dataDisplayPtr[minBinIndex] =
										(UInt8)((UInt16)(floatDisplayLevel) * floatMultiplier);

					floatDisplayLevel += displayInterval;

					}	// end "if (minBinIndex == maxBinIndex)"

				for (dataLevel=minBinIndex+1; dataLevel<=maxBinIndex; dataLevel++)
					{
					floatDisplayLevel += displayInterval;
					dataDisplayPtr[dataLevel] =
										(UInt8)((UInt16)(floatDisplayLevel) * floatMultiplier);

					//sprintf ((char*)gTextString,
					//					"  %d = %d%s",
					//					dataLevel,
					//					dataDisplayPtr[dataLevel],
					//					gEndOfLine);
					//						
					//continueFlag = ListString (
					//		(char*)gTextString, strlen ((char*)gTextString), gOutputTextH);

					}	// end "for (dataLevel=minValueIndex+1; ......

				displayLevel = (UInt16)((UInt16)floatDisplayLevel * floatMultiplier);

				//sprintf ((char*)gTextString,
				//					"  %d = %d%s%s",
				//					dataLevel,
				//					displayLevel,
				//					gEndOfLine,
				//					gEndOfLine);
				//						
				//	continueFlag = ListString (
				//			(char*)gTextString, strlen ((char*)gTextString), gOutputTextH);

				}	// end "else displaySpecsPtr->pixelSize > 8"

			}	// end "else ...->displayType != k1_ChannelThematicDisplayType"

		startValue = maxBinIndex + 1;
		if (startValue < (SInt32)dataDisplayVectorLength)
			{
			for (dataLevel=startValue;
					  dataLevel<(SInt32)dataDisplayVectorLength;
							dataLevel++)
				dataDisplayPtr[dataLevel] = (UInt8)displayLevel;

			}	// end "if (startValue < dataDisplayVectorLength)"

		if (displaySpecsPtr->displayType == k1_ChannelThematicDisplayType)
			{
					// Make sure the class assignments for the min and max data
					// value are correct when they fall within the thematic range.
			/*
			if (histogramSummaryPtr[channelNumberIndex].binType == kDataValueIsBinIndex)
				{
				indexForMinDataValue = 
											(UInt16)displaySpecsPtr->thematicTypeMinMaxValues[0];
				indexForMaxDataValue = 
											(UInt16)displaySpecsPtr->thematicTypeMinMaxValues[1];

				}	// end "if (...[channelNumberIndex].binType == kDataValueIsBinIndex)"
			*/
			if (histogramSummaryPtr[channelNumberIndex].binType == kBinWidthNotOne)
				{
				indexForMinDataValue = 0;
				indexForMaxDataValue = (UInt16)(dataDisplayVectorLength - 1);

				if ((!gImageFileInfoPtr->noDataValueFlag ||
						(gImageFileInfoPtr->noDataValueFlag &&
								gImageFileInfoPtr->noDataValue >
										histogramSummaryPtr[channelNumberIndex].maxValue)) &&
							(displaySpecsPtr->thematicTypeMinMaxValues[0] <
												histogramSummaryPtr[channelNumberIndex].minValue))
					dataDisplayPtr[indexForMinDataValue] = (UInt8)classForMinDataValue;


				if ((!gImageFileInfoPtr->noDataValueFlag ||
						(gImageFileInfoPtr->noDataValueFlag &&
								gImageFileInfoPtr->noDataValue <
								histogramSummaryPtr[channelNumberIndex].minValue)) &&
							(displaySpecsPtr->thematicTypeMinMaxValues[1] >
												histogramSummaryPtr[channelNumberIndex].maxValue))
					dataDisplayPtr[indexForMaxDataValue] = (UInt8)classForMaxDataValue;

				}	// end "if (histogramSummaryPtr[channelNumberIndex].binType == kBinWidthNotOne)"

			}	// end "if (...->displayType == k1_ChannelThematicDisplayType)"

				// Set up for treating data value of '0' as a background value
				// for one channel displays.

		if (displaySpecsPtr->backgroundValueCode == 1)
			dataDisplayPtr[0] = numberLevels + 1;

		else if (displaySpecsPtr->backgroundValueCode == 2)
			dataDisplayPtr[0] = 0;

				// Get the starting point for the look up table for the next channel.

		dataDisplayPtr = &dataDisplayPtr[dataDisplayVectorLength];

		}	// end "for (channel=0; channel<numberChannels; ...)"

	return (TRUE);

}	// end "FillDataToDisplayLevels"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GaussianParameterDialog
//
//	Software purpose:	The purpose of this routine is to handle the modal dialog for 
//							setting the Gaussian stretch parameter.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			OnEnhancementComboSelendok in xDisplayMultispectralDialog.cpp
//							DisplayMultispectralDialog in SDisplayMultispectral.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/06/2003
//	Revised By:			Larry L. Biehl			Date: 12/16/2016

Boolean GaussianParameterDialog (
				double*								gaussianStretchPtr)

{
	Boolean continueFlag = FALSE;

	#if defined multispec_mac
		Rect									theBox;

		double								localGaussianStretch;

		DialogPtr							dialogPtr;

		Handle								theHandle;

		SInt16								itemHit,
												theType;

		Boolean								modalDone;


				// Get the modal dialog for the reformat specification

		dialogPtr = LoadRequestedDialog (kGaussianParameterID, kCopyScrap, 1, 2);
		if (dialogPtr == NULL)
																								return (0);

				// Set current gaussian stretch

		LoadDItemRealValue (dialogPtr, 4, *gaussianStretchPtr, 1);

				// Center the dialog and then show it.

		ShowDialogWindow (dialogPtr, kGaussianParameterID, kSetUpDFilterTable);

		gDialogItemDescriptorPtr[4] = kDItemReal;

				// Set default text selection

		SelectDialogItemText (dialogPtr, 4, 0, INT16_MAX);

		modalDone = FALSE;
		itemHit = 0;
		do {
			ModalDialog (gProcessorDialogFilterPtr, &itemHit);
			if (itemHit > 2)
				{
						// If itemHit was a radio button make appropriate control
						// settings to indicate to the user the present selection.		
						// Get the handle to the itemHit.										

				GetDialogItem (dialogPtr, itemHit, &theType, &theHandle, &theBox);

				switch (itemHit)
					{
					case 4:
						break;

					}	// end "switch (itemHit)"

				}	// end "if (itemHit > 2)"

		  else if (itemHit > 0)	// and <= 2
				{
						// Check gaussian parameter.

				if (itemHit == 1)
					{
					localGaussianStretch = GetDItemRealValue (dialogPtr, 4);
					if (localGaussianStretch <= 0)
						itemHit = 0;

					}	// end "if (itemHit == 1)"

				if (itemHit > 0)
					modalDone = TRUE;

				if (itemHit == 1)
					{
					*gaussianStretchPtr = localGaussianStretch;

					continueFlag = TRUE;

					}	// end "if (itemHit == 1)"

				}	// end "else if (itemHit > 0)"

			} while (!modalDone);

		CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
	#endif	// defined multispec_mac

	#if defined multispec_win
		CMGaussianParameterDlg* dialogPtr = NULL;

		TRY
			{
			dialogPtr = new CMGaussianParameterDlg ();

			continueFlag = dialogPtr->DoDialog (gaussianStretchPtr);

			delete dialogPtr;
			}

		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			}
		END_CATCH_ALL
	#endif // defined multispec_win

   #if defined multispec_wx
		CMGaussianParameterDlg* dialogPtr = NULL;

		try 
			{
			dialogPtr = new CMGaussianParameterDlg ();

			continueFlag = dialogPtr->DoDialog (gaussianStretchPtr);

			delete dialogPtr;
			}
			
		catch (int e)
			{
			MemoryMessage (0, kObjectMessage);
			}
	#endif // defined multispec_wx

	return (continueFlag);

}	// end "GaussianParameterDialog"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GaussianToDisplayLevels
//
//	Software purpose:	The purpose of this routine is to set the vector which indicates 
//								how to map the image data values to the palette levels
//								using a guassian distribution count of pixels in each display 
//								level as the criterion. This routine will use the histogram 
//								of the image and the number of display levels as the input.
//
//	Functional description:	The steps in this function follow.
//			1) Determine the number of image data pixels to be displayed within each 
//				display level based on the histogram and a breakdown of a gaussian
//				distribution for 3 standard deviations into the number of display levels.
//
//			2) For each possible image data value, determine the display level to
//				use on the computer screen.
//
//				Also if the number of display levels is greater than the total number 
//				of image data values, the image data values should be spread
//				out over the display level range.  For example, a display levels 
//				range could be 0, 2, 4, 6, ....252, 254.
//
//	Parameters in:			
//
//	Parameters out:	None
//
// Value Returned:  	True if vector loaded okay.
//							False if vector did not load okay.
//
// Called By:			DisplayImage in SDisplay.cpp
//
//	Coded By:			Larry L. Biehl			Date:	05/02/2003
//	Revised By:			Larry L. Biehl			Date: 05/08/2020

Boolean GaussianToDisplayLevels (
				HistogramSpecsPtr					histogramSpecsPtr,
				HUCharPtr							dataDisplayPtr,
				DisplaySpecsPtr					displaySpecsPtr,
				UInt32								dataDisplayVectorLength,
				SInt16								numberChannels,
				UInt16*								channelsPtr)

{
	double								constant1,
											constant2,
											exponent,
											gaussianValue[256],
											increment,
											floatMultiplier,
											nextDisplayLevelChange,
											total,
											value;

	HistogramSummaryPtr				histogramSummaryPtr;
	HUInt32Ptr							histogramArrayPtr;

	SInt32								dataLevel,
											displayLevelInterval,
											firstIndexForBin,
											lastDataLevel,
											smallestIndexInterval,
											maxValueIndex,
											minValueIndex;

	UInt32								displayLevel,
											displayValue,
											highLimitDisplayLevel,
											highValue,
											initialValue,
											invertValueIndex,
											lastDisplayLevel,
											lastValue,
											lowLimitDisplayLevel,
											numberDataValues,
											numberLevels,
											numberValuesInDisplayLevel,
											rgbMultiplier,
											rgbOffset,
											totalPixels;

	SInt16								channel,
											classForMaxDataValue,
											//classForMaxThematicValue,
											classForMinDataValue;
											//classForMinThematicValue;

	Boolean								continueFlag = FALSE;


			// Check input variables.															

	if (histogramSpecsPtr == NULL || dataDisplayPtr == NULL)
																							return (FALSE);
	continueFlag = TRUE;
	histogramSummaryPtr = histogramSpecsPtr->histogramSummaryPtr;
	histogramArrayPtr = histogramSpecsPtr->histogramArrayPtr;
	totalPixels = 0;

	if (continueFlag) 
		{
				// Initialize local variables.												

		numberLevels = displaySpecsPtr->numberLevels;
		numberDataValues = gImageWindowInfoPtr->numberBins;

		if (displaySpecsPtr->pixelSize <= 8) 
			{
			////rgbOffset = 1;
			rgbOffset = 0;
			initialValue = rgbOffset;
			floatMultiplier = 1;
			highValue = numberDataValues;

			}	// end "if (displaySpecsPtr->pixelSize <= 8)"

		else	// displaySpecsPtr->pixelSize > 8 
			{
			rgbOffset = 0;

			if (numberLevels > 1) 
				{
				initialValue = 0;

				if (displaySpecsPtr->pixelSize == 16)
					highValue = 31;

				else	// displaySpecsPtr->pixelSize >= 24
					highValue = 255;

				floatMultiplier = (double)highValue / (double)(numberLevels - 1);

				}	// end "if (numberLevels > 1)"

			else	// numberLevels == 1
				{
				if (displaySpecsPtr->pixelSize == 16)
					initialValue = 15;

				else	// displaySpecsPtr->pixelSize >= 24
					initialValue = 127;

				highValue = initialValue;

				floatMultiplier = 1;

				}	// end "else numberLevels == 1"

			}	// end "else displaySpecsPtr->pixelSize > 8"

		rgbMultiplier = 1;

		if (displaySpecsPtr->minMaxCode == kEntireDataRange) 
			{
					// Set min-max for entire range option.								

			minValueIndex = 0;
			maxValueIndex = gImageWindowInfoPtr->numberBins - 1;

			}	// end "if (displaySpecsPtr->minMaxCode == kEntireDataRange)"

				// Fill the gaussian curve vector based on the number of levels.
				// The vector is long enough for half of a gaussian curve since it
				// is symmetric.

		constant1 = 1. / kSQRT2PI;
		constant2 = 1. / 2.;
		value = -displaySpecsPtr->gaussianStretch;
		total = 0;

		increment = (double)2 * fabs (value);
		if (numberLevels > 1) 
			{
			if (displaySpecsPtr->displayType == k1_ChannelThematicDisplayType) 
				{
				if (numberLevels > 3)
					increment /= (numberLevels - 3);

				}	// end "if (...->displayType == k1_ChannelThematicDisplayType)"

			else	// displaySpecsPtr->displayType != k1_ChannelThematicDisplayType
				increment /= (numberLevels - 1);

			}	// end "if (numberLevels > 1)"

		for (displayLevel = 0; displayLevel < numberLevels; displayLevel++) 
			{
			if (displaySpecsPtr->displayType != k1_ChannelThematicDisplayType ||
									(displayLevel > 0 && displayLevel < numberLevels - 1)) 
				{
				exponent = value;
				exponent *= exponent;
				exponent *= constant2;
				gaussianValue[displayLevel] = constant1 * exp (-exponent);

				total += gaussianValue[displayLevel];

				value += increment;

				}	// end "if (...->displayType !="

			else	// ...->displayType == k1_ChannelThematicDisplayType &&
				gaussianValue[displayLevel] = 0;

			}	// end "for (displayLevel=0; displayLevel<..."

				// Now normalize the distribution. Force the sum to be 1.

		//	double	newTotal = 0;
		if (total > 0) 
			{
			for (displayLevel=0; displayLevel<numberLevels; displayLevel++) 
				{
				gaussianValue[displayLevel] /= total;
				/*
				newTotal += gaussianValue[displayLevel];
							
					  // List the gaussian table to the output text window.	

				numberChars = sprintf ((char*)gTextString3, 
												  " %3ld  %15.7f  %15.7f%s", 
												  displayLevel, 
												  gaussianValue[displayLevel],
												  newTotal,
												  gEndOfLine);
				if (!ListString ((char*)gTextString3, numberChars, gOutputTextH))
																							return (FALSE);
				*/
				}	// end "for (displayLevel=0; displayLevel<..."

			}	// end "if (total > 0)"

				// Loop through the channels requested.									

		for (channel = 0; channel < numberChannels; channel++) 
			{
			continueFlag = GetHistogramVectorForChannel (
												  histogramSpecsPtr,
												  &channelsPtr[channel],
												  1);

			if (!continueFlag)
				break;

			GetMinMaxValuesIndices (displaySpecsPtr,
											  histogramSpecsPtr,
											  &histogramSummaryPtr[channelsPtr[channel]],
											  channel,
											  channelsPtr[channel],
											  TRUE,
											  gImageFileInfoPtr->signedValueOffset,
											  numberDataValues,
											  (UInt16)numberLevels,
											  &minValueIndex,
											  &maxValueIndex,
											  &classForMinDataValue,
											  &classForMaxDataValue);

					// Set the multiplier for this channel to be used to determine 
					// the look up table for the data values to color value table.	

			if (displaySpecsPtr->displayType != kSideSideChannelDisplayType &&
									channel > 0 &&
												displaySpecsPtr->pixelSize <= 8)
				rgbMultiplier *= numberLevels;

					// Get the number of pixels within the requested range.			

			totalPixels = 0;
			for (dataLevel=minValueIndex; dataLevel<=maxValueIndex; dataLevel++)
				totalPixels += histogramArrayPtr[dataLevel];

					// If the difference of the max and min value is 1, then this
					// indicates that all of the pixels are of one value. Do not
					// flag this as an error.

			if (totalPixels <= 0) 
				{
				continueFlag = FALSE;
				break;

				}	// end "if (totalPixels <= 0)"

					// Initialize the begin display level.  The first level in the	
					// palette is reserved white and the last level in the palette	
					// is reserved for black.  Therefore there are numberLevels + 	
					// 2 in the entire palette.  The first level will be skipped 	
					// in the image drawing programs because of the 'rgbOffset' 	
					// parameter used here.													

			nextDisplayLevelChange = gaussianValue[0] * totalPixels;
			numberValuesInDisplayLevel = 0;
			lastDataLevel = dataDisplayVectorLength - 1;

			if (displaySpecsPtr->displayType == kSideSideChannelDisplayType)
				invertValueIndex = 2;
			else	// displaySpecsPtr->displayType != kSideSideChannelDisplayType
				invertValueIndex = displaySpecsPtr->minMaxPointers[channel];

					// Note that inverted data value for thematic type windows are
					// handled in the order of the colors in the palette.

			if (displaySpecsPtr->displayType == k1_ChannelThematicDisplayType ||
										!displaySpecsPtr->invertValuesFlag[invertValueIndex]) 
				{
				displayLevel = 0;
				displayLevelInterval = 1;
				displayValue = initialValue;
				lastDisplayLevel = numberLevels - 1;
				lastValue = highValue;

				displaySpecsPtr->thematicBinWidth =
										histogramSummaryPtr[channelsPtr[channel]].binFactor;

				lowLimitDisplayLevel = 0;

				highLimitDisplayLevel = numberLevels - 1;
				if (displaySpecsPtr->displayType == k1_ChannelThematicDisplayType)
					highLimitDisplayLevel--;

				}	// end "if (...->displayType == k1_ChannelThematicDisplayType || ..."

			else	// ...->displayType != k1_ChannelThematicDisplayType &&
					//		...->invertValuesFlag[invertValueIndex]
				{
				displayLevel = numberLevels - 1;
				displayLevelInterval = -1;
				displayValue = displayLevel + initialValue;
				lastDisplayLevel = 0;
				lastValue = 0;

				lowLimitDisplayLevel = 1;
				if (displaySpecsPtr->displayType == k1_ChannelThematicDisplayType)
					lowLimitDisplayLevel++;

				highLimitDisplayLevel = numberLevels;

				}	// end "else ...->displayType != k1_ChannelThematicDisplayType && ...

			Boolean listFlag = FALSE;
			SInt32 testCount = 0;
			SInt16 numberChars;

					// Set the display level for the lower saturated data levels.

			for (dataLevel=0; dataLevel<minValueIndex; dataLevel++) 
				{
				dataDisplayPtr[dataLevel] = (UInt8)displayValue;

				if (listFlag && dataLevel == 0) 
					{
					numberChars = sprintf ((char*)gTextString3,
													 " %3d   %3d   %3d%s",
													 (int)dataLevel,
													 (int)dataDisplayPtr[dataLevel],
													 (int)numberValuesInDisplayLevel,
													 gEndOfLine);
					if (!ListString ((char*)gTextString3, numberChars, gOutputTextH))
																							return (FALSE);

					}	// end "if (listFlag)"

				}	// end "for (dataLevel=0; dataLevel<=minValueIndex; dataLevel++)"

			if (displaySpecsPtr->pixelSize <= 8) 
				{
				if (displaySpecsPtr->displayType == k1_ChannelThematicDisplayType) 
					{
					if (numberLevels >= 2)
						displayLevel += displayLevelInterval;

					displayValue = displayLevel * rgbMultiplier + rgbOffset;

					nextDisplayLevelChange += gaussianValue[displayLevel] * totalPixels;

					}	// end "if (...->displayType == k1_ChannelThematicDisplayType)"

						// These "displayValues" need to range between 0 to "numberLevels-1"

				firstIndexForBin = minValueIndex;
				smallestIndexInterval = SInt32_MAX;
				for (dataLevel=minValueIndex; dataLevel<=maxValueIndex; dataLevel++) 
					{
					dataDisplayPtr[dataLevel] = (UInt8)displayValue;
					numberValuesInDisplayLevel += histogramArrayPtr[dataLevel];

					if (listFlag) 
						{
						testCount += histogramArrayPtr[dataLevel];
						/*						
						numberChars = sprintf ((char*)gTextString3, 
														  " %3ld   %3ld   %3ld%s", 
														  dataLevel, 
														  dataDisplayPtr[dataLevel],
														  testCount,
														  gEndOfLine);
						if (!ListString ((char*)gTextString3, numberChars, gOutputTextH))
																							return (FALSE);
						*/
						}	// end "if (listFlag)"

					while (numberValuesInDisplayLevel >= nextDisplayLevelChange &&
									displayLevel >= lowLimitDisplayLevel &&
											displayLevel < highLimitDisplayLevel) 
						{
						if (listFlag) 
							{
							numberChars = sprintf ((char*)gTextString3,
															" %3d   %3d   %3d%s",
															(int)dataLevel,
															(int)dataDisplayPtr[dataLevel],
															(int)testCount,
															gEndOfLine);
							if (!ListString ((char*)gTextString3, numberChars, gOutputTextH))
																							return (FALSE);

							}	// end "if (listFlag)"

						displayLevel += displayLevelInterval;
						displayValue = displayLevel * rgbMultiplier + rgbOffset;
						nextDisplayLevelChange += gaussianValue[displayLevel] * totalPixels;

						testCount = 0;

						if (dataLevel > firstIndexForBin) 
							{
							smallestIndexInterval = MIN (smallestIndexInterval,
																	dataLevel - firstIndexForBin);
							firstIndexForBin = dataLevel;

							}	// end "if (dataLevel > firstDataLevelForBin)"

						}	// end "while (numberValuesInDisplayLevel > ..." 

					}	// end "for (dataLevel=minValueIndex; dataLevel<..." 

				if (listFlag) 
					{
					numberChars = sprintf ((char*)gTextString3,
													 " %3d   %3d   %3d%s",
													 (int)dataLevel,
													 (int)dataDisplayPtr[dataLevel - 1],
													 (int)testCount,
													 gEndOfLine);
					if (!ListString ((char*)gTextString3, numberChars, gOutputTextH))
																							return (FALSE);

					}	// end "if (listFlag)"

				lastValue = lastDisplayLevel * rgbMultiplier + rgbOffset;

				if (displaySpecsPtr->displayType == k1_ChannelThematicDisplayType)
					displaySpecsPtr->smallestIndexInterval = smallestIndexInterval;

				}	// end "if (displaySpecsPtr->pixelSize <= 8)"

			else	// displaySpecsPtr->pixelSize > 8 
				{
						// These "displayValues" need to range between 0 to 31 for 16-bit
						// color display and 0 to 255 for 24 bit color display. Missing number
						// of levels will just be values skipped between.

				for (dataLevel = minValueIndex; dataLevel <= maxValueIndex; dataLevel++) 
					{
					dataDisplayPtr[dataLevel] = (UInt8)displayValue;
					numberValuesInDisplayLevel += histogramArrayPtr[dataLevel];

					if (listFlag) 
						{
						testCount += histogramArrayPtr[dataLevel];
						/*						
						numberChars = sprintf ((char*)gTextString3, 
														" %3ld   %3ld   %3ld%s", 
														dataLevel, 
														dataDisplayPtr[dataLevel],
														testCount,
														gEndOfLine);
						if (!ListString ((char*)gTextString3, numberChars, gOutputTextH))
																							return (FALSE);
						*/
						}	// end "if (listFlag)"

					while (numberValuesInDisplayLevel >= nextDisplayLevelChange &&
									displayLevel >= lowLimitDisplayLevel &&
											displayLevel < highLimitDisplayLevel) 
						{
						if (listFlag) 
							{
							numberChars = sprintf ((char*)gTextString3,
															" %3d   %3d   %3d%s",
															(int)dataLevel,
															(int)dataDisplayPtr[dataLevel],
															(int)testCount,
															gEndOfLine);
							if (!ListString ((char*)gTextString3, numberChars, gOutputTextH))
																							return (FALSE);

							}	// end "if (listFlag)"

						displayLevel += displayLevelInterval;
						displayValue = (UInt32)(displayLevel * floatMultiplier);
						nextDisplayLevelChange += gaussianValue[displayLevel] * totalPixels;

						testCount = 0;

						}	// end "while (numberValuesInDisplayLevel > ..." 

					}	// end "for (dataLevel=minValueIndex; ..." 

				}	// end "else displaySpecsPtr->pixelSize > 8"

					// Set the display level for the upper saturated data levels.	

			testCount = histogramArrayPtr[maxValueIndex];
			for (dataLevel=maxValueIndex+1; dataLevel<=lastDataLevel; dataLevel++) 
				{
				dataDisplayPtr[dataLevel] = (UInt8)lastValue;

				if (listFlag && dataLevel == lastDataLevel) 
					{
					numberChars = sprintf ((char*)gTextString3,
													" %3d   %3d   %3d%s",
													(int)dataLevel,
													(int)dataDisplayPtr[dataLevel],
													(int)testCount,
													gEndOfLine);
					if (!ListString ((char*)gTextString3, numberChars, gOutputTextH))
																							return (FALSE);

					testCount = 0;

					}	// end "if (listFlag)"

				}	// end "for (dataLevel=maxValueIndex+1; ..."

					// Set up for treating data value of '0' as a background value	
					// for one channel displays.												

			if (displaySpecsPtr->backgroundValueCode == 1)
				dataDisplayPtr[0] = (UChar)(numberLevels + 1);

			else if (displaySpecsPtr->backgroundValueCode == 2)
				dataDisplayPtr[0] = 0;
			/*			
					// List the table.

			for (dataLevel=0; dataLevel<lastDataLevel; dataLevel++)	
				{		
				numberChars = sprintf ((char*)gTextString3, 
												" %3ld   %3ld%s", 
												dataLevel, 
												dataDisplayPtr[dataLevel],
												gEndOfLine);
				if (!ListString ((char*)gTextString3, numberChars, gOutputTextH))
																							return (FALSE);
										
				}	// end "for (dataLevel=1; dataLevel<lastDataLevel; ..."
			*/
					// Change the rgbOffset and initialValue for the rest of the
					// channels for 8-bit pixel sizes.

			if (displaySpecsPtr->pixelSize <= 8) 
				{
				if (displaySpecsPtr->displayType != kSideSideChannelDisplayType)
					rgbOffset = 0;

				initialValue = rgbOffset;

				}	// end "if (displaySpecsPtr->pixelSize <= 8)"
				
					// Get the starting point for the look up table for the next channel.																		

			dataDisplayPtr = (HUCharPtr)&dataDisplayPtr[dataDisplayVectorLength];

			}	// end "for (channel=0; channel<numberChannels; ...)" 

		}	// end "if (continueFlag)" 

	if (totalPixels == 0)
		DisplayAlert (kErrorAlertID,
							kStopAlert,
							kAlertStrID,
							IDS_Alert18,
							0,
							NULL);

	return (continueFlag);

}	// end "GaussianToDisplayLevels" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetDefaultPaletteSpecs
//
//	Software purpose:	The purpose of this routine is to determine the 
// 						default palette specifications to be used in the
//							display dialog based on the current settings and the
//							next image to be displayed.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			DisplayMultispectralDialog in SDisplayMultispectral.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/07/1988
//	Revised By:			Larry L. Biehl			Date: 09/24/1993	

void GetDefaultPaletteSpecs (
				SInt16								currentDisplayType,
				SInt16								currentPixelSize,
				SInt16								numberChannels,
				UInt16*								newDisplayTypePtr,
				SInt16*								maxSystemPixelSizePtr)

{
	SInt16								newImagePixelSize;


			// Set up/check display palette type.  Only need to worry about 		
			// systems with color quickdraw and available bits of color more		
			// than 1, systems with only one bit of "color" have only one type 	
			// and it has already been set during initializion of application.  	
			// It can not be changed.															

	*newDisplayTypePtr = currentDisplayType;
	*maxSystemPixelSizePtr = GetMaxSystemPixelSize ();
	newImagePixelSize = GetDefaultImagePixelSize (*maxSystemPixelSizePtr);

			// If the default pixel size is one and										
			// the current display type is not 1, then the current window			
			// was created when at least one of the monitors was set to more 		
			// than one byte of color.  Set default pixel size to the current		
			// pixel size.																			

	if (*maxSystemPixelSizePtr == 1 && currentDisplayType != 1)
		newImagePixelSize = currentPixelSize;

	if (newImagePixelSize > 1) 
		{
		if (numberChannels == 1 && newImagePixelSize > 8) 
			{
			if (gOSXFlag && !gOSXCoreGraphicsFlag)
				newImagePixelSize = 16;

			else	// !gOSXFlag || gOSXCoreGraphicsFlag 
				newImagePixelSize = 8;

			}	// end "if (numberChannels == 1 && newImagePixelSize > 8)"

				// Check if display type makes sense with number of channels. 		

		if (numberChannels == 1 && *newDisplayTypePtr > k1_ChannelGrayLevelDisplayType)
			*newDisplayTypePtr = k1_ChannelGrayLevelDisplayType;

				// Make certain that the display type make sense relative to the	
				// number of color bits.														

		if (*maxSystemPixelSizePtr == 2 &&
				 *newDisplayTypePtr != k1_ChannelThematicDisplayType &&
						*newDisplayTypePtr != k1_ChannelGrayLevelDisplayType &&
								*newDisplayTypePtr != kSideSideChannelDisplayType)
			*newDisplayTypePtr = k1_ChannelGrayLevelDisplayType;

		}	// end "if (newImagePixelSize > 1)" 

}	// end "GetDefaultPaletteSpecs" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetHistogramComputeCode
//
//	Software purpose:	The purpose of this routine is to obtain the histogram compute
//							code that is best to use based on the current display and
//							histogram settings.
//
//	Parameters in:		pointer to display specification structure.
//
//	Parameters out:	None
//
// Value Returned:	Histogram compute code.			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/14/1998 
//	Revised By:			Larry L. Biehl			Date: 05/01/2007	

SInt16 GetHistogramComputeCode (
				DisplaySpecsPtr					displaySpecsPtr)

{
	HistogramSpecsPtr					histogramSpecsPtr;

	Handle								histogramSpecsHandle;
	SInt16								histogramComputeCode;


			// Determine whether histogram statistics are loaded into memory	
			// or whether they can be read from disk or will need to be			
			// computed.																		  

	histogramComputeCode = kComputeHistogram;

	histogramSpecsHandle = GetHistogramSpecsHandle (gImageWindowInfoPtr);

	histogramSpecsPtr = (HistogramSpecsPtr) GetHandlePointer (histogramSpecsHandle);

	if (histogramSpecsPtr != NULL && histogramSpecsPtr->loadedFlag)
		histogramComputeCode = kDoNotComputeHistogram;

	if (!GetHistogramRequiredFlag (displaySpecsPtr->displayType,
												displaySpecsPtr->enhancementCode,
												displaySpecsPtr->minMaxCode))
		histogramComputeCode = kDoNotComputeHistogram;

	if (histogramComputeCode == kComputeHistogram &&
				displaySpecsPtr->defaultHistogramFileFlag)
		histogramComputeCode = kLoadHistogramFromDisk;

			// If the histogram information is not loaded and it exists then
			// force the information to be loaded from disk if the current setting
			// is to not compute the histogram.

	if (histogramSpecsPtr != NULL &&
			!histogramSpecsPtr->loadedFlag &&
					displaySpecsPtr->defaultHistogramFileFlag &&
							histogramComputeCode == kDoNotComputeHistogram)
		histogramComputeCode = kLoadHistogramFromDisk;

	return (histogramComputeCode);

}	// end "GetHistogramComputeCode"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetHistogramRequiredFlag
//
//	Software purpose:	The purpose of this routine is to determine whether a histogram
//							is required to be able to display the image withe the requested
//							parameters.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:	Histogram compute code.			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/01/2007 
//	Revised By:			Larry L. Biehl			Date: 05/01/2007	

Boolean GetHistogramRequiredFlag (
				SInt16								displayType,
				SInt16								enhanceStretchSelection,
				SInt16								lMinMaxCode)

{
	Boolean								histogramRequiredFlag;


	histogramRequiredFlag = TRUE;
	if (displayType != k1_ChannelThematicDisplayType &&
            enhanceStretchSelection == kLinearStretch &&
						lMinMaxCode != kPerChannelMinMax)
		histogramRequiredFlag = FALSE;

	return (histogramRequiredFlag);

}	// end "GetHistogramRequiredFlag"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetMinMaxPopupCode
//
//	Software purpose:	The purpose of this routine is to determine the code for the
//							min-max popup menu
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:	Code for min-max menu item to be used.			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/01/2007 
//	Revised By:			Larry L. Biehl			Date: 05/01/2007	



SInt16 GetMinMaxPopupCode (
				SInt16								minMaxCode,
				SInt16								percentClip) 
{
	SInt16								minMaxPopupCode;


	minMaxPopupCode = 3;
	if (minMaxCode == kPerChannelMinMax && percentClip == 0)
		minMaxPopupCode = 0;

	else if (minMaxCode == kPerChannelMinMax && percentClip == 2)
		minMaxPopupCode = 1;

	else if (minMaxCode == kEntireDataRange)
		minMaxPopupCode = 2;

	else if (minMaxCode == kUserSpecified)
		minMaxPopupCode = 3;

	else if (minMaxCode == kThematicDefault)
		minMaxPopupCode = 4;

	return (minMaxPopupCode);

}	// end "GetMinMaxPopupCode"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetMinMaxValuesIndices
//
//	Software purpose:	The purpose of this routine is to get the min and max values or
//							indices to be used for image enhancement operations to create 
//							the data value to display level vector.
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/22/2006 
//	Revised By:			Larry L. Biehl			Date: 12/09/2011	

void GetMinMaxValuesIndices (
				DisplaySpecsPtr					displaySpecsPtr,
				HistogramSpecsPtr					histogramSpecsPtr,
				HistogramSummaryPtr				histogramSummaryPtr,
				SInt16								channel,
				SInt16								channelNumberIndex,
				Boolean								histLoadedFlag,
				SInt32								signedValueOffset,
				UInt32								numberBins,
				UInt16								numberClasses,
				SInt32*								minBinIndexPtr,
				SInt32*								maxBinIndexPtr,
				SInt16*								classForMinDataValuePtr,
				SInt16*								classForMaxDataValuePtr)

{
	double								maxThematicValue,
											minThematicValue;

	SInt32								maxBinIndex,
											minBinIndex;


	minBinIndex = 0;
	maxBinIndex = 0;
	minThematicValue = 0;
	maxThematicValue = 0;

	*classForMinDataValuePtr = 0;
	*classForMaxDataValuePtr = 0;

	// Set min-max for entire range option.										

	if (displaySpecsPtr->minMaxCode == kEntireDataRange) 
		{
		minBinIndex = 0;
		maxBinIndex = histogramSummaryPtr->numberBins - 1;

		}	// end "if (displaySpecsPtr->minMaxCode == kEntireDataRange)" 

	else if (displaySpecsPtr->minMaxCode == kPerChannelMinMax) 
		{
				// Get the min-max based on a percentage of data levels		
				// being clipped.															

		GetClippedMinMaxValueIndices (histogramSpecsPtr,
												 displaySpecsPtr->percentTailsClipped,
												 channelNumberIndex,
												 histogramSummaryPtr->numberBins,
												 &minBinIndex,
												 &maxBinIndex,
												 histLoadedFlag,
												 FALSE);

		}	// end "else if (...->minMaxCode == kPerChannelMinMax)" 

	else if (displaySpecsPtr->minMaxCode == kUserSpecified) 
		{
				// Get the min-max based on user specified values.				

		minBinIndex = displaySpecsPtr->minMaxValuesIndex[
									displaySpecsPtr->minMaxPointers[channel]][0];
		maxBinIndex = displaySpecsPtr->minMaxValuesIndex[
									displaySpecsPtr->minMaxPointers[channel]][1];

		}	// end "if (...->minMaxCode == kUserSpecified)" 

	else if (displaySpecsPtr->minMaxCode == kThematicDefault) 
		{
				// Get the min-max based on the thematic type display default.		

		GetThematicTypeMinMaxIndices ((SInt16)displaySpecsPtr->numberLevels,
												 channelNumberIndex + 1,
												 &minThematicValue,
												 &maxThematicValue,
												 &minBinIndex,
												 &maxBinIndex);

		}	// end "if (...->minMaxCode == kThematicDefault)" 

			// Make sure that index values make sense.

	minBinIndex = MAX (0, minBinIndex);
	maxBinIndex = MAX (0, maxBinIndex);

			// Make certain that the min and max values are consistant		
			// with the specified number of data values in the data.			

	minBinIndex = MIN (minBinIndex, (SInt32)(numberBins - 1));
	maxBinIndex = MIN (maxBinIndex, (SInt32)(numberBins - 1));

	if (minBinIndex > maxBinIndex) 
		{
		minBinIndex = 0;
		maxBinIndex = numberBins - 1;

		}	// end "if (minBinIndex > maxBinIndex)"

	if (displaySpecsPtr->displayType == k1_ChannelThematicDisplayType) 
		{
				// Also need the min/max data values for thematic type multispectral 
				// displays.	

		if (displaySpecsPtr->minMaxCode == kEntireDataRange ||
												displaySpecsPtr->minMaxCode == kPerChannelMinMax) 
			{
			minThematicValue = GetDataValueForBinIndex (minBinIndex,
																	  histogramSummaryPtr,
																	  signedValueOffset);

			maxThematicValue = GetDataValueForBinIndex (maxBinIndex,
																	  histogramSummaryPtr,
																	  signedValueOffset);

			}	// end "if (displaySpecsPtr->minMaxCode == kEntireDataRange || ..." 

		else if (displaySpecsPtr->minMaxCode == kUserSpecified) 
			{
					// Get the min-max based on user specified values.				

			minThematicValue = displaySpecsPtr->minMaxValues[
												displaySpecsPtr->minMaxPointers[channel]][0];
			maxThematicValue = displaySpecsPtr->minMaxValues[
												displaySpecsPtr->minMaxPointers[channel]][1];

			}	// end "if (...->minMaxCode == kUserSpecified)" 

		//else displaySpecsPtr->minMaxCode == kThematicDefault
		// Already obtained above

		displaySpecsPtr->thematicTypeMinMaxValues[0] = minThematicValue;
		displaySpecsPtr->thematicTypeMinMaxValues[1] = maxThematicValue;

		if (histogramSummaryPtr->binType == kBinWidthNotOne) 
			{
					// Classes are filled for the entire data display vector (except for
					// the ends which are for min and max data values) for case
					// where vector indices do not represent the data values themselves.

			minBinIndex = 1;
			maxBinIndex = histogramSummaryPtr->numberBins - 2;

			}	// end "if (histogramSummaryPtr->binType == kBinWidthNotOne)"

		//else  histogramSummaryPtr->binType == kDataValueIsBinIndex ||
		//								histogramSummaryPtr->binType == kBinWidthOfOne	
		// Already set.

		displaySpecsPtr->thematicTypeMinMaxIndices[0] = minBinIndex;
		displaySpecsPtr->thematicTypeMinMaxIndices[1] = maxBinIndex;
		/*		
				// Get the min-max based on the thematic type display default.	
			
		*classForMinThematicValuePtr = 2;	
		if (minBinIndex == 0)
			{
			if (minThematicValue < histogramSummaryPtr->minValue)
				*classForMinThematicValuePtr = 0;

			else if (minThematicValue < histogramSummaryPtr->minNonSatValue)
				{
				*classForMinThematicValuePtr = 1;
				minBinIndex = 1;
			
				}	// end "else if (minThematicValue < ...->minNonSatValue)"

			}	// end "if (minBinIndex == 0)"
								
		*classForMaxThematicValuePtr = numberClasses - 3;	
		if (maxBinIndex == numberBins - 1)
			{
			if (maxThematicValue > histogramSummaryPtr->maxValue)
				*classForMaxThematicValuePtr = numberClasses - 1;

			else if (maxThematicValue > histogramSummaryPtr->maxNonSatValue)
				*classForMaxThematicValuePtr = numberClasses - 2;

			}	// end "if (maxBinIndex == numberBins - 1)"			
		*/
		*classForMinDataValuePtr = GetThematicClassForDataValue (
													 histogramSummaryPtr->minValue,
													 minThematicValue,
													 maxThematicValue,
													 numberClasses);

		*classForMaxDataValuePtr = GetThematicClassForDataValue (
													 histogramSummaryPtr->maxValue,
													 minThematicValue,
													 maxThematicValue,
													 numberClasses);

		}	// end "if (displaySpecsPtr->displayType == k1_ChannelThematicDisplayType)"

	*minBinIndexPtr = minBinIndex;
	*maxBinIndexPtr = maxBinIndex;

}	// end "GetMinMaxValuesIndices"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetThematicClassForDataValue
//
//	Software purpose:	The purpose of this routine is to find the thematic class for
//							the input data value. This is done in a brute force way so that
//							that data bin to class index vector can be filled. It was written
//							specifically for find the thematic class for the min and max
//							data values in the image.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 06/12/2011
//	Revised By:			Larry L. Biehl			Date: 06/12/2011	

SInt16 GetThematicClassForDataValue (
				double								dataValue,
				double								minThematicRange,
				double								maxThematicRange,
				SInt16								numberThematicClasses)

{
	double								thematicClassBinMaximum,
											thematicClassDataBinWidth;

	UInt32								classIndex;

	SInt16								classNumber;


	classNumber = 0;
	if (numberThematicClasses == 2) 
		{
		if (dataValue > minThematicRange)
			classNumber = 1;

		}	// end "if (numberThematicClasses == 2)"

	else	// numberThematicClasses >= 3
		{
		if (dataValue < minThematicRange)
			classNumber = 0;

		else if (dataValue > maxThematicRange)
			classNumber = numberThematicClasses - 1;

		else	// minThematicRange <= dataValue <= maxThematicRange
			{
					// Find class data value will fall in.

			thematicClassBinMaximum = minThematicRange;
			thematicClassDataBinWidth =
					  (maxThematicRange - minThematicRange) / (numberThematicClasses - 2);

			for (classIndex=1; classIndex<(UInt32)numberThematicClasses; classIndex++) 
				{
				thematicClassBinMaximum += thematicClassDataBinWidth;

				if (dataValue <= thematicClassBinMaximum) 
					{
					classNumber = (SInt16)classIndex;
					break;

					}	// end "if (dataValue <= thematicClassBinMaximum)"

				}	// end "for (classIndex=1; classIndex<numberThematicClasses)"

			}	// end "else minThematicRange <= dataValue <= maxThematicRange"

		}	// end "else numberThematicClasses >= 3"

	return (classNumber);

}	// end "GetThematicClassForDataValue"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetThematicTypeMinMaxIndices
//
//	Software purpose:	The purpose of this routine is to update the enhancement 
//							minimum and maximum values for thematic type display of 
//							multispectral images.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 10/21/2006
//	Revised By:			Larry L. Biehl			Date: 02/07/2018

void GetThematicTypeMinMaxIndices (
				SInt16								numberClassBins,
				SInt16								channelNumber,
				double*								minThematicValuePtr,
				double*								maxThematicValuePtr,
				SInt32*								minValueIndexPtr,
				SInt32*								maxValueIndexPtr)

{
	double								//maxThematicValue,
											maxThematicValueWithSignedOffset,
											//minThematicValue,
											minThematicValueWithSignedOffset;

	HistogramSpecsPtr					histogramSpecsPtr = NULL;
	HistogramSummaryPtr				histogramSummaryPtr;

	Handle								histogramSpecsHandle;

	UInt32								channelIndex;


	*minValueIndexPtr = 0;
	*maxValueIndexPtr = 0;

	histogramSpecsHandle = GetHistogramSpecsHandle (gImageWindowInfoPtr);

	if (histogramSpecsHandle != NULL)
		{
				// The handle is already locked.

		histogramSpecsPtr = (HistogramSpecsPtr)GetHandlePointer (histogramSpecsHandle);

		if (!histogramSpecsPtr->loadedFlag)
			histogramSpecsPtr = NULL;

		}	// end "if (histogramSpecsHandle != NULL)" 

	if (histogramSpecsPtr != NULL) 
		{
		if (histogramSpecsPtr->histogramSummaryPtr != NULL) 
			{
			channelIndex = channelNumber - 1;

			histogramSummaryPtr =
					  &histogramSpecsPtr->histogramSummaryPtr[channelIndex];

			UpdateThematicTypeMinMaxes (numberClassBins,
												  channelNumber,
												  minThematicValuePtr,
												  maxThematicValuePtr);

			minThematicValueWithSignedOffset =
							*minThematicValuePtr + gImageFileInfoPtr->signedValueOffset;
			*minValueIndexPtr = GetBinIndexForDataValue (
																  minThematicValueWithSignedOffset,
																  histogramSummaryPtr);

			maxThematicValueWithSignedOffset =
							*maxThematicValuePtr + gImageFileInfoPtr->signedValueOffset;
			*maxValueIndexPtr = GetBinIndexForDataValue (
																  maxThematicValueWithSignedOffset,
																  histogramSummaryPtr);

			}	// "if (histogramSpecsPtr->histogramSummaryPtr != NULL)"

		}	// end "if (histogramSpecsPtr != NULL)"

}	// end "GetThematicTypeMinMaxIndices"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:			Boolean HistogramVector
//
//	Software purpose:		The purpose of this routine is to set the vector 
//		which indicates how to map the image data values to the palette levels.
//		This routine will use the histogram of the image and the number of 
//		display levels as the input.
//
//	Functional description:	The steps in this function follow.
//
//			1) Determine if the gToDisplayLevels.upToDateFlag is FALSE.  If it
//				is TRUE, exit the function.  If it is FALSE fill the gToDisplayLevels
//				structure for channel channelNumber.
//
//			2) Determine the image data interval to be displayed within each 
//				display level based on the histogram minimum and maximum for the
//				specified channel and the number of display values in the pallete.  
//				This will be a real number value.
//
//			3) For each possible image data value, determine the display level to
//				use on the computer screen.  Assume image data values from 0 to
//				the minimum histogram value will be displayed as the "darkest" 
//				display value and image data values from the maximum histogram value
//				will be displayed as the "brightest" display value.
//
//				For now assume an equal interval spacing of image data values to
//				display levels.  Also if the number of display levels is greater than
//				the image data value range, the image data values should be spread
//				out over the display level range.  For example, a data value
//				range of 101 to 228 should be displayed as display levels 0, 2, 4,
//				6, ....252, 254.
//
//	Parameters in:		&gActiveImageWindow
//							channelNumber
//
//	Parameters out:	None
//
// Value Returned:  	True if vector loaded okay.
//							False if vector was not loaded.
//
// Called By:			DisplayImage in SDisplay.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/21/1988
//	Revised By:			Larry L. Biehl			Date: 11/13/2019

Boolean HistogramVector (
				DisplaySpecsPtr					displaySpecsPtr,
				HistogramSpecsPtr					histogramSpecsPtr)

{
	Boolean								continueFlag,
											redoFlag,
											returnFlag = TRUE;

	HistogramSummaryPtr				histogramSummaryPtr;

	HUCharPtr							dataDisplayPtr;

	UInt16*								channelsPtr;

	SInt32								vectorBytes;

	UInt32								numberValues;

	SInt16								histogramDisplayType,
											numberChannels;

	UInt16								channelNumber[3];


			// Check input variables.															

	if (gImageWindowInfoPtr == NULL)
																							return (FALSE);

	histogramSummaryPtr = histogramSpecsPtr->histogramSummaryPtr;

			// Initialize local variables.													

	redoFlag = FALSE;

			// Set up the channels pointer. channelsHandle is already locked.												

	channelsPtr = (UInt16*)GetHandlePointer (displaySpecsPtr->channelsHandle);

	if (displaySpecsPtr->displayType != kSideSideChannelDisplayType)
		channelsPtr = channelNumber;

	histogramDisplayType = displaySpecsPtr->displayType;
	if (displaySpecsPtr->pixelSize > 8 &&
							displaySpecsPtr->displayType == k3_2_ChannelDisplayType)
		histogramDisplayType = k3_ChannelDisplayType;

			// Initialize channels to be used for setting up display vector		

	switch (histogramDisplayType) 
		{
		case k1_ChannelThematicDisplayType: // 1-channel thematic display
		case k1_ChannelGrayLevelDisplayType: // 1-channel gray level
			numberChannels = 1;
			channelNumber[0] = displaySpecsPtr->channelNumber - 1;
			channelNumber[1] = 0;
			channelNumber[2] = 0;
			displaySpecsPtr->minMaxPointers[0] = 2;
			break;

		case k3_ChannelDisplayType: // 3-channel display
			numberChannels = 3;
			channelNumber[0] = displaySpecsPtr->blueChannelNumber - 1;
			channelNumber[1] = displaySpecsPtr->greenChannelNumber - 1;
			channelNumber[2] = displaySpecsPtr->redChannelNumber - 1;
			displaySpecsPtr->minMaxPointers[0] = 0;
			displaySpecsPtr->minMaxPointers[1] = 1;
			displaySpecsPtr->minMaxPointers[2] = 2;
			break;

		//case 4: // 2-channel display
		case k3_2_ChannelDisplayType: // 3-channel display with 2 channels repeated.
			numberChannels = 2;
			channelNumber[0] = 0;
			channelNumber[1] = 0;
			if (displaySpecsPtr->rgbColors == kRGColor) 
				{
				channelNumber[0] = displaySpecsPtr->greenChannelNumber - 1;
				channelNumber[1] = displaySpecsPtr->redChannelNumber - 1;
				displaySpecsPtr->minMaxPointers[0] = 1;
				displaySpecsPtr->minMaxPointers[1] = 2;
				}

			if (displaySpecsPtr->rgbColors == kGBColor) 
				{
				channelNumber[0] = displaySpecsPtr->blueChannelNumber - 1;
				channelNumber[1] = displaySpecsPtr->greenChannelNumber - 1;
				displaySpecsPtr->minMaxPointers[0] = 0;
				displaySpecsPtr->minMaxPointers[1] = 1;
				}

			if (displaySpecsPtr->rgbColors == kRBColor) 
				{
				channelNumber[0] = displaySpecsPtr->blueChannelNumber - 1;
				channelNumber[1] = displaySpecsPtr->redChannelNumber - 1;
				displaySpecsPtr->minMaxPointers[0] = 0;
				displaySpecsPtr->minMaxPointers[1] = 2;
				}

			channelNumber[2] = 0;
			break;

		case kSideSideChannelDisplayType: // Channels side by side.
			numberChannels = 1;
			channelNumber[0] = *channelsPtr;
			channelNumber[1] = 0;
			channelNumber[2] = 0;
			if (displaySpecsPtr->enhancementCode == kEqualAreaStretch ||
					  displaySpecsPtr->enhancementCode == kGaussianStretch ||
							displaySpecsPtr->minMaxCode != kEntireDataRange)
				numberChannels = displaySpecsPtr->numberChannels;

					// Force display level vector to be redone for side by side		
					// channel case.																

			redoFlag = TRUE;
			break;

		}	// end "switch (displaySpecsPtr->displayType)" 

			// Fill vector to be used to assign data values to image display		
			// levels if it is not up-to-date												

	if (gToDisplayLevels.vectorHandle == NULL)
		redoFlag = TRUE;

	#if defined multispec_mac 
		else if (gToDisplayLevels.window != gActiveImageWindow)
			redoFlag = TRUE;
	#endif	// defined multispec_mac

	#if defined multispec_win || defined multispec_wx
		else if (gToDisplayLevels.window != gActiveImageViewCPtr)
			redoFlag = TRUE;
	#endif	// defined multispec_win

	else if (gToDisplayLevels.numberLevels != displaySpecsPtr->numberLevels)
		redoFlag = TRUE;
	else if (gToDisplayLevels.numberBytes != gImageWindowInfoPtr->numberBytes)
		redoFlag = TRUE;
	else if (gToDisplayLevels.pixelSize != displaySpecsPtr->pixelSize)
		redoFlag = TRUE;

	else if (gToDisplayLevels.channel1 != channelNumber[0])
		redoFlag = TRUE;
	else if (numberChannels >= 2 && gToDisplayLevels.channel2 != channelNumber[1])
		redoFlag = TRUE;
	else if (numberChannels >= 3 && gToDisplayLevels.channel3 != channelNumber[2])
		redoFlag = TRUE;
	else if (gToDisplayLevels.minMaxCode != displaySpecsPtr->minMaxCode)
		redoFlag = TRUE;
	else if (gToDisplayLevels.enhancementCode != displaySpecsPtr->enhancementCode)
		redoFlag = TRUE;
	else if (gToDisplayLevels.percentTailsClipped !=
															displaySpecsPtr->percentTailsClipped)
		redoFlag = TRUE;
	else if (gToDisplayLevels.gaussianStretch !=
															displaySpecsPtr->gaussianStretch)
		redoFlag = TRUE;
	else if (gToDisplayLevels.backgroundValueCode !=
															displaySpecsPtr->backgroundValueCode)
		redoFlag = TRUE;
	else if (!gToDisplayLevels.upToDateFlag)
		redoFlag = TRUE;
	else if (histogramSpecsPtr->histogramChangedSinceDisplayFlag)
		redoFlag = TRUE;

	numberValues = 256;
	if (gImageWindowInfoPtr->numberBytes == 2)
		numberValues = gImageWindowInfoPtr->numberBins;
	else	// gImageWindowInfoPtr->numberBytes > 2
		numberValues = histogramSpecsPtr->maxNumberBins;

	vectorBytes = (SInt32)numberChannels * numberValues;

	if (gToDisplayLevels.vectorBytes != vectorBytes)
		redoFlag = TRUE;

	if (redoFlag) 
		{
		returnFlag = FALSE;

		if (gToDisplayLevels.vectorHandle)
			gToDisplayLevels.vectorHandle =
								UnlockAndDispose (gToDisplayLevels.vectorHandle);

		gToDisplayLevels.vectorHandle = (Handle)MNewHandle (vectorBytes);

		if (gToDisplayLevels.vectorHandle != NULL) 
			{
					// Get the memory needed for the histogram array and open the	
					// statistics file if needed for determining the enhancement	
					// min and max based on a percent being cliped.						

			continueFlag = TRUE;
			if (displaySpecsPtr->enhancementCode == kEqualAreaStretch ||
					  displaySpecsPtr->enhancementCode == kGaussianStretch ||
							(displaySpecsPtr->minMaxCode == kPerChannelMinMax &&
									displaySpecsPtr->percentTailsClipped > 0)) 
				{
				continueFlag = SetUpToReadHistogramArray (histogramSpecsPtr);

				if (!continueFlag && gMemoryError == noErr) 
					{
					DisplayAlert (kErrorAlertID,
										kStopAlert,
										kAlertStrID,
										IDS_Alert22,
										0,
										NULL);

					displaySpecsPtr->enhancementCode = kLinearStretch;
					displaySpecsPtr->minMaxCode = kPerChannelMinMax;
					displaySpecsPtr->percentTailsClipped = 0;

					continueFlag = TRUE;

					}	// end "if (!continueFlag && gMemoryError == noErr)"

				}	// end "if (...->minMaxCode == kPerChannelMinMax && ..." 

			if (continueFlag) 
				{
						// Make sure the min/max value indices are correct.

				UpdateMinMaxValueIndices (displaySpecsPtr,
													histogramSummaryPtr,
													numberChannels,
													channelsPtr,
													gImageFileInfoPtr->signedValueOffset);

				dataDisplayPtr = (HUCharPtr)GetHandlePointer (
									gToDisplayLevels.vectorHandle, kLock);

				if (displaySpecsPtr->enhancementCode == kLinearStretch)
					returnFlag = FillDataToDisplayLevels (histogramSpecsPtr,
																		dataDisplayPtr,
																		displaySpecsPtr,
																		numberValues,
																		numberChannels,
																		channelsPtr);

				else if (displaySpecsPtr->enhancementCode == kEqualAreaStretch)
					returnFlag = EqualAreaDataToDisplayLevels (
																		histogramSpecsPtr,
																		dataDisplayPtr,
																		displaySpecsPtr,
																		numberValues,
																		numberChannels,
																		channelsPtr);

				else if (displaySpecsPtr->enhancementCode == kGaussianStretch)
					returnFlag = GaussianToDisplayLevels (histogramSpecsPtr,
																		dataDisplayPtr,
																		displaySpecsPtr,
																		numberValues,
																		numberChannels,
																		channelsPtr);

				CheckSizeAndUnlockHandle (gToDisplayLevels.vectorHandle);

				gToDisplayLevels.backgroundValueCode =
																displaySpecsPtr->backgroundValueCode;

				#if defined multispec_mac 
					gToDisplayLevels.window = gActiveImageWindow;
				#endif	// defined multispec_mac

				#if defined multispec_win || defined multispec_wx
					gToDisplayLevels.window = gActiveImageViewCPtr;
				#endif	// defined multispec_win || defined multispec_wx

				gToDisplayLevels.numberLevels = displaySpecsPtr->numberLevels;
				gToDisplayLevels.numberBytes = gImageWindowInfoPtr->numberBytes;
				gToDisplayLevels.pixelSize = displaySpecsPtr->pixelSize;
				gToDisplayLevels.channel1 = channelNumber[0];
				gToDisplayLevels.channel2 = channelNumber[1];
				gToDisplayLevels.channel3 = channelNumber[2];
				gToDisplayLevels.minMaxCode = displaySpecsPtr->minMaxCode;
				gToDisplayLevels.enhancementCode = displaySpecsPtr->enhancementCode;
				gToDisplayLevels.gaussianStretch = displaySpecsPtr->gaussianStretch;
				gToDisplayLevels.percentTailsClipped = 
																displaySpecsPtr->percentTailsClipped;
				gToDisplayLevels.vectorBytes = vectorBytes;
				gToDisplayLevels.bytesOffset = numberValues;

				}	// end "if (continueFlag)" 

			}	// end "if (gToDisplayLevels.vectorHandle != NULL)" 

		else	// gToDisplayLevels.vectorHandle == NULL 
			returnFlag = FALSE;

		}	// end "if (redoFlag)" 

			// Indicated that the vector is up-to-date									

	gToDisplayLevels.upToDateFlag = returnFlag;
	histogramSpecsPtr->histogramChangedSinceDisplayFlag = !returnFlag;

	CloseUpHistogramArrayReadParameters ();

	return (returnFlag);

}	// end "HistogramVector"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean LoadMultispectralDisplaySpecs
//
//	Software purpose:	The purpose of this routine is to load the Display 
//							Specification structure with the default set of
//							parameters for a new multispectral image file which 
//							has been opened.
//
//	Parameters in:		Window pointer to the grafport of the image window.
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/20/1988
//	Revised By:			Ravi S. Budruk			Date: 08/09/1988	
//	Revised By:			Larry L. Biehl			Date: 05/11/2020

DisplaySpecsPtr LoadMultispectralDisplaySpecs (void)

{
   LongRect                      longRect;

   DisplaySpecsPtr               displaySpecsPtr;
   UInt16*                       channelsPtr;

   Handle                        displaySpecsHandle;

   SInt32                        numberBytes;

   SInt16                        bandInterleaveCode,
                                 dataBandInterleave,
                                 projectedCSTypeGeoKey;

   UInt16                        index;

   Boolean                       continueFlag,
                                 pointerChanged,
                                 useNaturalColorBandsFlag;


         // Setup and initialize the structure if need.

   displaySpecsHandle = GetActiveDisplaySpecsHandle ();

   displaySpecsHandle = GetDisplaySpecsStructure (displaySpecsHandle, TRUE);

   SetActiveDisplaySpecsHandle (displaySpecsHandle);

   if (displaySpecsHandle == NULL)
																							return (NULL);

         // Lock handle to display specification structure and get pointer to
         // it																					

   displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
                                          displaySpecsHandle, kLock, kMoveHi);

         // Make sure channel list handle is up to date.

   numberBytes =
            (SInt32)gImageWindowInfoPtr->totalNumberChannels * sizeof (SInt16);
   channelsPtr = (UInt16*)CheckHandleSize (&displaySpecsPtr->channelsHandle,
															&continueFlag,
															&pointerChanged,
															numberBytes);

   if (channelsPtr == NULL)
																							return (NULL);

   if (pointerChanged ||
            channelsPtr[displaySpecsPtr->numberChannels - 1] >
								gImageWindowInfoPtr->totalNumberChannels)
      {
      displaySpecsPtr->channelSet = kAllMenuItem;
      displaySpecsPtr->numberChannels = gImageWindowInfoPtr->totalNumberChannels;
      for (index=0; index<gImageWindowInfoPtr->totalNumberChannels; index++)
         channelsPtr[index] = index;

      }	// end "if (pointerChanged || ...)"

   if (displaySpecsPtr->firstTime)
      {
				// Set up the "first time" parameters for multispectral type
				// images.
		
				// The default is to use color IR band set as the default.
				// But if the file is from the tool parameter environment variable
				// with the Online Version use the natural color bands.
		
		useNaturalColorBandsFlag = FALSE;
		if (gFromToolParameterFileFlag)
			useNaturalColorBandsFlag = TRUE;

				// Get pointer to palette class with space for 256 entries.

		#if defined multispec_win // || defined multispec_wx
         if (displaySpecsPtr->paletteObject == NULL)
            displaySpecsPtr->paletteObject = new CMPalette;

         if (displaySpecsPtr->paletteObject == NULL)
            {
            CheckAndUnlockHandle (displaySpecsPtr->channelsHandle);
																							return (NULL);

            }	// end "if (displaySpecsPtr->paletteObject == NULL)"
		#endif	// defined multispec_win || defined multispec_wx

				// Set initial palette type setting.
				// This will be use for 1-channel thematic type display

      displaySpecsPtr->thematicClassPaletteType = kCorrelationMatrixColors;
      displaySpecsPtr->thematicGroupPaletteType = kCorrelationMatrixColors;

				// Set background color for TIFF/GeoTIFF/JPEG files.

      if (gImageFileInfoPtr->format == kTIFFType ||
                gImageFileInfoPtr->format == kGeoTIFFType ||
								gImageFileInfoPtr->format == kJPEGType)
			displaySpecsPtr->backgroundValueCode = 0;

               // Get the band interleave code for later use.

		#if include_gdal_capability
         bandInterleaveCode = gImageFileInfoPtr->gdalBandInterleave;
		#endif	// !include_gdal_capability
		#if !include_gdal_capability
         bandInterleaveCode = gImageFileInfoPtr->bandInterleave;
		#endif	// !include_gdal_capability

      displaySpecsPtr->channelNumber = 1;

      if (gImageWindowInfoPtr->totalNumberChannels == 1)
         {
         displaySpecsPtr->blueChannelNumber = 1;
         displaySpecsPtr->greenChannelNumber = 1;
         displaySpecsPtr->redChannelNumber = 1;

         displaySpecsPtr->displayType = k1_ChannelGrayLevelDisplayType;

         }	// end "if (windowInfoPtr->totalNumberChannels == 1)"

      else if (gImageWindowInfoPtr->totalNumberChannels == 2)
         {
         displaySpecsPtr->blueChannelNumber = 1;
         displaySpecsPtr->greenChannelNumber = 1;
         displaySpecsPtr->redChannelNumber = 2;

         displaySpecsPtr->displayType = k3_2_ChannelDisplayType;

         displaySpecsPtr->rgbColors = kRGColor;

         }	// end "else if (gImageWindowInfoPtr->totalNumberChannels == 2)"

      else if (gImageWindowInfoPtr->totalNumberChannels == 3)
         {
         if ((gImageFileInfoPtr->format == kPNGType ||
                    gImageFileInfoPtr->format == kTIFFType ||
                    gImageFileInfoPtr->format == kGeoTIFFType ||
                    gImageFileInfoPtr->format == kJPEGType) &&
                    bandInterleaveCode != kBSQ)
            {
            displaySpecsPtr->blueChannelNumber = 3;
            displaySpecsPtr->greenChannelNumber = 2;
            displaySpecsPtr->redChannelNumber = 1;

            displaySpecsPtr->displayType = k3_ChannelDisplayType;
            displaySpecsPtr->rgbColors = kRGBColor;

            }	// end "else if (gImageFileInfoPtr->format == kTIFFType ..."

         else	// gImageFileInfoPtr->format != kPNGType ...
            {
            displaySpecsPtr->blueChannelNumber = 2;
            displaySpecsPtr->greenChannelNumber = 2;
            displaySpecsPtr->redChannelNumber = 3;
            displaySpecsPtr->displayType = k3_2_ChannelDisplayType;
            displaySpecsPtr->rgbColors = kRGColor;

            if (displaySpecsPtr->pixelSize >= 16)
               {
               displaySpecsPtr->blueChannelNumber = 1;
               displaySpecsPtr->greenChannelNumber = 2;
               displaySpecsPtr->redChannelNumber = 3;
               displaySpecsPtr->displayType = k3_ChannelDisplayType;
               displaySpecsPtr->rgbColors = kRGBColor;

               }	// end "if (displaySpecsPtr->pixelSize >= 16)"

            }	// end "else gImageFileInfoPtr->format != kPNGType"

         }	// end "else if (gImageWindowInfoPtr->totalNumberChannels == 3)"
		
		else if (gImageWindowInfoPtr->totalNumberChannels == 4 && 
															gImageFileInfoPtr->format == kPNGType)
			{
					// Assume channel 4 is a mask channel

			displaySpecsPtr->blueChannelNumber = 3;
			displaySpecsPtr->greenChannelNumber = 2;
			displaySpecsPtr->redChannelNumber = 1;

			displaySpecsPtr->displayType = k3_ChannelDisplayType;
			displaySpecsPtr->rgbColors = kRGBColor;

			}	// end "else if (gImageFileInfoPtr->format == kTIFFType ..."

      else	// (gImageWindowInfoPtr->totalNumberChannels >= 4)
         {
         displaySpecsPtr->displayType = k3_2_ChannelDisplayType;
         displaySpecsPtr->rgbColors = kRGColor;

         if (displaySpecsPtr->pixelSize >= 16)
            {
            displaySpecsPtr->displayType = k3_ChannelDisplayType;
            displaySpecsPtr->rgbColors = kRGBColor;

            }	// end "if (displaySpecsPtr->pixelSize >= 16)"

         projectedCSTypeGeoKey = GetProjectedCSTypeGeoKey (gImageFileInfoPtr);
         if (gImageWindowInfoPtr->totalNumberChannels == 4 &&
                    (gImageFileInfoPtr->format == kTIFFType ||
                    gImageFileInfoPtr->format == kGeoTIFFType ||
                    gImageFileInfoPtr->format == kJPEGType) &&
                    projectedCSTypeGeoKey >= 2965 &&
                    projectedCSTypeGeoKey <= 2968)
            {
                  // Assume 2011-2013 Indiana Orthodata which is stored as
                  // red, green, blue, infrared. Set default of color IR.

            if (useNaturalColorBandsFlag)
               {
               displaySpecsPtr->blueChannelNumber = 3;
               displaySpecsPtr->greenChannelNumber = 2;
               displaySpecsPtr->redChannelNumber = 1;
               }
               
            else	// !useNaturalColorBandsFlag
               {
               displaySpecsPtr->blueChannelNumber = 2;
               displaySpecsPtr->greenChannelNumber = 1;
               displaySpecsPtr->redChannelNumber = 4;
               }

            }	// end "if (...->totalNumberChannels == 4 && "

         else if (gImageWindowInfoPtr->totalNumberChannels == 4 ||
                    gImageWindowInfoPtr->totalNumberChannels == 5)
            {
            if (gImageFileInfoPtr->instrumentCode == kSPOT ||
                  gImageFileInfoPtr->instrumentCode == kSPOT4)
               {
               displaySpecsPtr->blueChannelNumber = 2;
               displaySpecsPtr->greenChannelNumber = 2;
               displaySpecsPtr->redChannelNumber = 3;
               if (displaySpecsPtr->pixelSize >= 16)
                  displaySpecsPtr->blueChannelNumber = 1;

               }	// end "if (gImageFileInfoPtr->instrumentCode == kSPOT

            else if (gImageFileInfoPtr->instrumentCode == kLandsatMSS)
               {
               displaySpecsPtr->blueChannelNumber = 2;
               displaySpecsPtr->greenChannelNumber = 2;
               displaySpecsPtr->redChannelNumber = 4;
               if (displaySpecsPtr->pixelSize >= 16)
                  displaySpecsPtr->blueChannelNumber = 1;

               }	// end "else if (gImageFileInfoPtr->instrumentCode == kLandsatMSS)

            else if (gImageFileInfoPtr->instrumentCode == kSentinel2A_MSI ||
								gImageFileInfoPtr->instrumentCode == kSentinel2B_MSI)
               {
							// This assumes the 10 meter spatial bands
					
					if (useNaturalColorBandsFlag)
						{
						displaySpecsPtr->blueChannelNumber = 2;
						displaySpecsPtr->greenChannelNumber = 2;
						displaySpecsPtr->redChannelNumber = 3;
						if (displaySpecsPtr->pixelSize >= 16)
							displaySpecsPtr->blueChannelNumber = 1;
						
						}	// end "if (useNaturalColorBandsFlag)"
					
					else	// use color infrared
						{
						displaySpecsPtr->blueChannelNumber = 3;
						displaySpecsPtr->greenChannelNumber = 3;
						displaySpecsPtr->redChannelNumber = 4;
						if (displaySpecsPtr->pixelSize >= 16)
							displaySpecsPtr->blueChannelNumber = 2;
							
						}	// end "else !useNaturalColorBandsFlag"

               }	// end "else if (gImageFileInfoPtr->instrumentCode == kSentinel2A_MSI ...

            else if (gImageFileInfoPtr->instrumentCode == kPeruSat)
               {
					if (gImageWindowInfoPtr->channelsInWavelengthOrderCode == kNotInOrder)
						{
						if (useNaturalColorBandsFlag)
							{
							displaySpecsPtr->blueChannelNumber = 3;
							displaySpecsPtr->greenChannelNumber = 2;
							displaySpecsPtr->redChannelNumber = 1;
							
							}	// end "if (useNaturalColorBandsFlag)"
						
						else	// !useNaturalColorBandsFlag
							{
							displaySpecsPtr->blueChannelNumber = 1;
							displaySpecsPtr->greenChannelNumber = 1;
							displaySpecsPtr->redChannelNumber = 4;
							if (displaySpecsPtr->pixelSize >= 16)
								displaySpecsPtr->blueChannelNumber = 2;
								
							}	// end "else !useNaturalColorBandsFlag"
						
						}	// end "if (...->channelsInWavelengthOrderCode == kNotInOrder)"
					
					else	// channelsInWavelengthOrderCode == kInOrder or not known
						{
						if (useNaturalColorBandsFlag)
							{
							displaySpecsPtr->blueChannelNumber = 1;
							displaySpecsPtr->greenChannelNumber = 2;
							displaySpecsPtr->redChannelNumber = 3;
							
							}	// end "if (useNaturalColorBandsFlag)"
						
						else	// !useNaturalColorBandsFlag
							{
							displaySpecsPtr->blueChannelNumber = 3;
							displaySpecsPtr->greenChannelNumber = 3;
							displaySpecsPtr->redChannelNumber = 4;
							if (displaySpecsPtr->pixelSize >= 16)
								displaySpecsPtr->blueChannelNumber = 2;
								
							}	// end "else !useNaturalColorBandsFlag"
						
						}	// end "else channelsInWavelengthOrderCode == kInOrder or ..."

               }	// end "else if (gImageFileInfoPtr->instrumentCode == kPeruSat)

            else	// ...->instrumentCode != kSPOT && != kLandsatMSS != kSentinel2A_MSI
               {
               if (gImageWindowInfoPtr->totalNumberChannels == 4)
                  {
                        // Assume 4 channel IKONOS data.

                  displaySpecsPtr->blueChannelNumber = 3;
                  displaySpecsPtr->greenChannelNumber = 3;
                  displaySpecsPtr->redChannelNumber = 4;
                  if (displaySpecsPtr->pixelSize >= 16)
                      displaySpecsPtr->blueChannelNumber = 2;

                  }	// end "if (gImageWindowInfoPtr->totalNumberChannels == 4)"

               else	// gImageWindowInfoPtr->totalNumberChannels == 5
                  {
                        // 5 channel MSS data.

                  displaySpecsPtr->blueChannelNumber = 2;
                  displaySpecsPtr->greenChannelNumber = 2;
                  displaySpecsPtr->redChannelNumber = 4;
                  if (displaySpecsPtr->pixelSize >= 16)
                     displaySpecsPtr->blueChannelNumber = 1;

                  }	// end "else gImageWindowInfoPtr->totalNumberChannels == 5"

               }	// end "else gImageFileInfoPtr->instrumentCode != kSPOT"

            }	// end "if (gImageWindowInfoPtr->totalNumberChannels == 4 || ..."

         else if (gImageWindowInfoPtr->totalNumberChannels == 6 &&
							(gImageFileInfoPtr->instrumentCode == kSentinel2A_MSI ||
									gImageFileInfoPtr->instrumentCode == kSentinel2B_MSI))
            {
						// This assumes the 20 meter spatial bands
				
				displaySpecsPtr->blueChannelNumber = 3;
				displaySpecsPtr->greenChannelNumber = 3;
				displaySpecsPtr->redChannelNumber = 5;
				if (displaySpecsPtr->pixelSize >= 16)
					displaySpecsPtr->blueChannelNumber = 6;

            }	// end "if (windowInfoPtr->totalNumberChannels == 6 ...)"

         else if (gImageWindowInfoPtr->totalNumberChannels == 7 &&
                    gImageFileInfoPtr->instrumentCode == kLandsatLC8_OLI)
            {
                  // Assume Landsat 8 surface reflectance

            if (useNaturalColorBandsFlag)
               {
               displaySpecsPtr->blueChannelNumber = 3;
               displaySpecsPtr->greenChannelNumber = 3;
               displaySpecsPtr->redChannelNumber = 4;
               if (displaySpecsPtr->pixelSize >= 16)
                  displaySpecsPtr->blueChannelNumber = 2;
               }
               
            else  // use color IR bands
               {
               displaySpecsPtr->blueChannelNumber = 4;
               displaySpecsPtr->greenChannelNumber = 4;
               displaySpecsPtr->redChannelNumber = 5;
               if (displaySpecsPtr->pixelSize >= 16)
                  displaySpecsPtr->blueChannelNumber = 3;
               }

            }	// end "if (windowInfoPtr->totalNumberChannels == 7 && ..."

         else if (gImageWindowInfoPtr->totalNumberChannels == 8 &&
                    gImageFileInfoPtr->instrumentCode != kLandsatLC8_OLI_TIRS &&
                    gImageFileInfoPtr->instrumentCode != kLandsatLC8_OLI)
            {
                  // Assume Landsat 7

            if (useNaturalColorBandsFlag)
               {
               displaySpecsPtr->blueChannelNumber = 2;
               displaySpecsPtr->greenChannelNumber = 2;
               displaySpecsPtr->redChannelNumber = 3;
               if (displaySpecsPtr->pixelSize >= 16)
                  displaySpecsPtr->blueChannelNumber = 1;
               }
               
            else  // use color IR bands
               {
               displaySpecsPtr->blueChannelNumber = 3;
               displaySpecsPtr->greenChannelNumber = 3;
               displaySpecsPtr->redChannelNumber = 4;
               if (displaySpecsPtr->pixelSize >= 16)
                  displaySpecsPtr->blueChannelNumber = 2;
               }

            }	// end "if (windowInfoPtr->totalNumberChannels >= 8 && ..."

         else if (gImageWindowInfoPtr->totalNumberChannels >= 8 &&
                    gImageWindowInfoPtr->totalNumberChannels <= 10)
            {
                  // Assume Advance Land Imager (ALI) data or
                  // Landsat 8
               
            if (useNaturalColorBandsFlag)
               {
               displaySpecsPtr->blueChannelNumber = 3;
               displaySpecsPtr->greenChannelNumber = 3;
               displaySpecsPtr->redChannelNumber = 4;
               if (displaySpecsPtr->pixelSize >= 16)
                  displaySpecsPtr->blueChannelNumber = 2;
               }
               
            else	// use color IR bands
               {
               displaySpecsPtr->blueChannelNumber = 4;
               displaySpecsPtr->greenChannelNumber = 4;
               displaySpecsPtr->redChannelNumber = 5;
               if (displaySpecsPtr->pixelSize >= 16)
                  displaySpecsPtr->blueChannelNumber = 3;
               }

            }	// end "if (windowInfoPtr->totalNumberChannels >= 8 && <= 10)"

         else if (gImageWindowInfoPtr->totalNumberChannels == 12)
            {
                  // Assume Michigan Scanner data
               
            if (useNaturalColorBandsFlag)
               {
               displaySpecsPtr->blueChannelNumber = 7;
               displaySpecsPtr->greenChannelNumber = 7;
               displaySpecsPtr->redChannelNumber = 9;
               if (displaySpecsPtr->pixelSize >= 16)
                  displaySpecsPtr->blueChannelNumber = 3;
               }
               
            else	// use color IR bands
               {
               displaySpecsPtr->blueChannelNumber = 9;
               displaySpecsPtr->greenChannelNumber = 9;
               displaySpecsPtr->redChannelNumber = 11;
               if (displaySpecsPtr->pixelSize >= 16)
                  displaySpecsPtr->blueChannelNumber = 7;
               }

            }	// end "if (windowInfoPtr->totalNumberChannels == 12)"

         else if (gImageWindowInfoPtr->totalNumberChannels == 13 &&
							(gImageFileInfoPtr->instrumentCode == kSentinel2A_MSI ||
									gImageFileInfoPtr->instrumentCode == kSentinel2B_MSI))
            {
						// This assumes a combined 10, 20 and 60 meter spatial bands
						
            if (useNaturalColorBandsFlag)
					{
               displaySpecsPtr->blueChannelNumber = 3;
               displaySpecsPtr->greenChannelNumber = 3;
               displaySpecsPtr->redChannelNumber = 4;
               if (displaySpecsPtr->pixelSize >= 16)
                  displaySpecsPtr->blueChannelNumber = 2;
               }
               
            else	// use color IR bands
               {
               displaySpecsPtr->blueChannelNumber = 4;
               displaySpecsPtr->greenChannelNumber = 4;
               displaySpecsPtr->redChannelNumber = 8;
               if (displaySpecsPtr->pixelSize >= 16)
                  displaySpecsPtr->blueChannelNumber = 3;
               }

            }	// end "if (windowInfoPtr->totalNumberChannels == 13 ...)"

         else if (gImageWindowInfoPtr->totalNumberChannels == 15)
            {
                  // Assume Atlas Scanner data

				displaySpecsPtr->blueChannelNumber = 4;
				displaySpecsPtr->greenChannelNumber = 4;
				displaySpecsPtr->redChannelNumber = 6;
				if (displaySpecsPtr->pixelSize >= 16)
					displaySpecsPtr->blueChannelNumber = 2;

            }	// end "if (windowInfoPtr->totalNumberChannels == 15)"

         else if (gImageWindowInfoPtr->totalNumberChannels == 120)
            {
                  // Assume ITD Imaging Spectrometer data.

            displaySpecsPtr->blueChannelNumber = 68;
            displaySpecsPtr->greenChannelNumber = 68;
            displaySpecsPtr->redChannelNumber = 90;
            if (displaySpecsPtr->pixelSize >= 16)
               displaySpecsPtr->blueChannelNumber = 29;

            }	// end "else if (gImageWindowInfoPtr->totalNumberChannels == 120)"

         else if (gImageWindowInfoPtr->totalNumberChannels == 126)
            {
                  // Assume HYMAP Imaging Spectrometer data.

            displaySpecsPtr->blueChannelNumber = 16;
            displaySpecsPtr->greenChannelNumber = 16;
            displaySpecsPtr->redChannelNumber = 32;
            if (displaySpecsPtr->pixelSize >= 16)
               displaySpecsPtr->blueChannelNumber = 8;

            }	// end "else if (gImageWindowInfoPtr->totalNumberChannels == 126)"

         else if (gImageWindowInfoPtr->totalNumberChannels == 210 ||
                 gImageWindowInfoPtr->totalNumberChannels == 191)
            {
                  // Assume HYDICE Imaging Spectrometer data.

				displaySpecsPtr->blueChannelNumber = 52;
				displaySpecsPtr->greenChannelNumber = 52;
				displaySpecsPtr->redChannelNumber = 63;
				if (displaySpecsPtr->pixelSize >= 16)
					displaySpecsPtr->blueChannelNumber = 36;

            }	// end "else if (...->totalNumberChannels == 210 || ..."

         else if (gImageWindowInfoPtr->totalNumberChannels >= 270 && 
								gImageWindowInfoPtr->totalNumberChannels <= 280)
            {
                  // Assume Headwall Imaging Spectrometer data.

				displaySpecsPtr->blueChannelNumber = 118;
				displaySpecsPtr->greenChannelNumber = 118;
				displaySpecsPtr->redChannelNumber = 160;
				if (displaySpecsPtr->pixelSize >= 16) 
					displaySpecsPtr->blueChannelNumber = 73;

            }	// end "else if (...->totalNumberChannels == 278"

         else if (gImageWindowInfoPtr->totalNumberChannels == 384)
            {
                  // Assume ??? Imaging Spectrometer data.

            displaySpecsPtr->blueChannelNumber = 57;
            displaySpecsPtr->greenChannelNumber = 57;
            displaySpecsPtr->redChannelNumber = 93;
            if (displaySpecsPtr->pixelSize >= 16)
               displaySpecsPtr->blueChannelNumber = 36;

            }	// end "else if (...->totalNumberChannels == 384)"

         else if (gImageWindowInfoPtr->totalNumberChannels == 512 ||
                 gImageWindowInfoPtr->totalNumberChannels == 640)
            {
                  // Assume GER 1500 Spectrometer data.

            displaySpecsPtr->blueChannelNumber = 225;
            displaySpecsPtr->greenChannelNumber = 225;
            displaySpecsPtr->redChannelNumber = 348;
            if (displaySpecsPtr->pixelSize >= 16)
               displaySpecsPtr->blueChannelNumber = 157;

            }	// end "else if (...->totalNumberChannels == 512 || ..."

         else if (gImageWindowInfoPtr->totalNumberChannels < 100)
            {
            displaySpecsPtr->blueChannelNumber = 3;
            displaySpecsPtr->greenChannelNumber = 3;
            displaySpecsPtr->redChannelNumber = 4;
            if (displaySpecsPtr->pixelSize >= 16)
               displaySpecsPtr->blueChannelNumber = 2;

            }	// end "if (windowInfoPtr->totalNumberChannels < 100)"

         else if (gImageWindowInfoPtr->totalNumberChannels <= 128)
            {
            displaySpecsPtr->blueChannelNumber = 60;
            displaySpecsPtr->greenChannelNumber = 60;
            displaySpecsPtr->redChannelNumber = 90;
            if (displaySpecsPtr->pixelSize >= 16)
               displaySpecsPtr->blueChannelNumber = 40;

            }	// end "if (windowInfoPtr->totalNumberChannels <= 128)"

         else if (gImageWindowInfoPtr->totalNumberChannels == 136)
            {
                  // Assume Nano Imaging Spectrometer data.
				
				if (gCallProcessorDialogFlag)
					{
							// Color IR image
					
					displaySpecsPtr->blueChannelNumber = 64;
					displaySpecsPtr->greenChannelNumber = 64;
					displaySpecsPtr->redChannelNumber = 84;
					if (displaySpecsPtr->pixelSize >= 16)
						displaySpecsPtr->blueChannelNumber = 35;
					
					}	// end "if (gCallProcessorDialogFlag)"
				
				else	// !gCallProcessorDialogFlag
					{
							// Natural color image
					
					displaySpecsPtr->blueChannelNumber = 35;
					displaySpecsPtr->greenChannelNumber = 35;
					displaySpecsPtr->redChannelNumber = 64;
					if (displaySpecsPtr->pixelSize >= 16)
						displaySpecsPtr->blueChannelNumber = 14;
					
					}	// end "else !gCallProcessorDialogFlag"

            }	// end "if (windowInfoPtr->totalNumberChannels == 136)"

         else	// gImageWindowInfoPtr->totalNumberChannels > 128 
            {
                  // Assume AVIRIS Imaging Spectrometer data.

            displaySpecsPtr->blueChannelNumber = 27;
            displaySpecsPtr->greenChannelNumber = 27;
            displaySpecsPtr->redChannelNumber = 50;
            if (displaySpecsPtr->pixelSize >= 16)
               displaySpecsPtr->blueChannelNumber = 17;

            }	// end "else ...->totalNumberChannels > 128" 

         }	// end "else gImageWindowInfoPtr->totalNumberChannels >= 4"

            // Initialize pop up menu selections.

      displaySpecsPtr->displaySet = displaySpecsPtr->displayType;
      if (displaySpecsPtr->displayType == k3_ChannelDisplayType ||
             displaySpecsPtr->displayType == k3_2_ChannelDisplayType)
         displaySpecsPtr->displaySet = k3_ChannelDisplayType;
		
		else if (displaySpecsPtr->displayType == kSideSideChannelDisplayType)
         displaySpecsPtr->displaySet = kSideSideChannelDisplayType;

      displaySpecsPtr->enhancementCode = kLinearStretch;

      displaySpecsPtr->minMaxCode = kPerChannelMinMax;

      if (gImageFileInfoPtr->gdalBandInterleave > 0)
         dataBandInterleave = gImageFileInfoPtr->gdalBandInterleave;

      else	// gImageFileInfoPtr->gdalBandInterleave <= 0
         dataBandInterleave = MAX (0, gImageFileInfoPtr->bandInterleave);
         
      if ((gImageFileInfoPtr->format == kTIFFType ||
             gImageFileInfoPtr->format == kGeoTIFFType ||
             gImageFileInfoPtr->format == kJPEGType ||
             gImageFileInfoPtr->format == kPNGType) &&
             dataBandInterleave == kBIS &&
             gImageFileInfoPtr->numberBytes == 1 &&
             gImageFileInfoPtr->numberChannels <= 3)
         displaySpecsPtr->minMaxCode = kEntireDataRange;

      displaySpecsPtr->minMaxValuesIndex[0][0] = 0;
      displaySpecsPtr->minMaxValuesIndex[0][1] =
													(SInt32)gImageWindowInfoPtr->numberBins - 1;
      displaySpecsPtr->minMaxValuesIndex[1][0] = 0;
      displaySpecsPtr->minMaxValuesIndex[1][1] =
													(SInt32)gImageWindowInfoPtr->numberBins - 1;
      displaySpecsPtr->minMaxValuesIndex[2][0] = 0;
      displaySpecsPtr->minMaxValuesIndex[2][1] =
													(SInt32)gImageWindowInfoPtr->numberBins - 1;

      if (!CheckIfOffscreenImageExists (gImageWindowInfoPtr))
         SetDisplayImageWindowSizeVariables (displaySpecsPtr);

      displaySpecsPtr->thematicValueFactor = 1.0;

            // Make changes in the default set if the last display was for an image
            // with similar data value types and the number of channels are the same.

      if (gDisplaySpecsDefault.structureLoadedFlag)
         {
               // Now verify that the data value types are the same between the new
               // image and that for the saved setting.

         if (gImageWindowInfoPtr->dataTypeCode == gDisplaySpecsDefault.dataTypeCode &&
					gImageWindowInfoPtr->numberBytes == gDisplaySpecsDefault.numberBytes &&
					gImageFileInfoPtr->signedDataFlag ==
															gDisplaySpecsDefault.signedDataFlag &&
					gImageWindowInfoPtr->channelsInWavelengthOrderCode ==
										gDisplaySpecsDefault.channelsInWavelengthOrderCode &&
					gImageWindowInfoPtr->totalNumberChannels ==
																gDisplaySpecsDefault.numberChannels)
            {
            if (gDisplaySpecsDefault.lastDisplayType == k1_ChannelThematicDisplayType)
               {
               displaySpecsPtr->thematicTypeMinMaxValues[0] =
												gDisplaySpecsDefault.thematicTypeMinMaxValues[0];
               displaySpecsPtr->thematicTypeMinMaxValues[1] =
												gDisplaySpecsDefault.thematicTypeMinMaxValues[1];

               displaySpecsPtr->numberLevels = 
												gDisplaySpecsDefault.numberThematicLevels;
               displaySpecsPtr->channelNumber = gDisplaySpecsDefault.channelNumber;
               displaySpecsPtr->enhancementCode = 
												gDisplaySpecsDefault.enhancementCode;
               displaySpecsPtr->minMaxCode = gDisplaySpecsDefault.minMaxCode;
               displaySpecsPtr->percentTailsClipped = 
												gDisplaySpecsDefault.percentTailsClipped;
					displaySpecsPtr->pixelSize = gDisplaySpecsDefault.pixelSize;
					
               displaySpecsPtr->displayType = gDisplaySpecsDefault.lastDisplayType;
               displaySpecsPtr->thematicValueFactor = 
												gDisplaySpecsDefault.thematicValueFactor;
               displaySpecsPtr->invertValuesFlag[2] = 
												gDisplaySpecsDefault.invertValuesFlag[2];
               displaySpecsPtr->thematicClassPaletteType =
										gDisplaySpecsDefault.oneChannelThematicClassPaletteType;
               displaySpecsPtr->magnification = gDisplaySpecsDefault.magnification;
               displaySpecsPtr->displaySet = gDisplaySpecsDefault.displaySet;

               }	// end "if (displaySpecsPtr->displayType == ..."

            else if (gDisplaySpecsDefault.lastDisplayType ==
																		k1_ChannelGrayLevelDisplayType)
               {
               displaySpecsPtr->numberLevels = gDisplaySpecsDefault.numberLevels;
               displaySpecsPtr->channelNumber = gDisplaySpecsDefault.channelNumber;
               displaySpecsPtr->enhancementCode = 
															gDisplaySpecsDefault.enhancementCode;
               displaySpecsPtr->minMaxCode = gDisplaySpecsDefault.minMaxCode;
               displaySpecsPtr->percentTailsClipped = 
															gDisplaySpecsDefault.percentTailsClipped;
					displaySpecsPtr->pixelSize = gDisplaySpecsDefault.pixelSize;
					
               displaySpecsPtr->displayType = gDisplaySpecsDefault.lastDisplayType;
               displaySpecsPtr->displaySet = gDisplaySpecsDefault.displaySet;
               displaySpecsPtr->invertValuesFlag[2] = 
															gDisplaySpecsDefault.invertValuesFlag[2];
               displaySpecsPtr->backgroundValueCode = 
															gDisplaySpecsDefault.backgroundValueCode;
               displaySpecsPtr->backgroundDataValue = 
															gDisplaySpecsDefault.backgroundDataValue;

               }	// end "if (displaySpecsPtr->displayType == k1_ChannelGrayLevelDisplayType)"

            else if (gDisplaySpecsDefault.lastDisplayType == k3_ChannelDisplayType ||
                     gDisplaySpecsDefault.lastDisplayType == k3_2_ChannelDisplayType) // ||
                     //gDisplaySpecsDefault.lastDisplayType == k2_ChannelDisplayType)
               {
               displaySpecsPtr->numberLevels = gDisplaySpecsDefault.numberLevels;
               displaySpecsPtr->redChannelNumber = 
														gDisplaySpecsDefault.redChannelNumber;
               displaySpecsPtr->invertValuesFlag[2] = 
														gDisplaySpecsDefault.invertValuesFlag[2];
               displaySpecsPtr->greenChannelNumber = 
														gDisplaySpecsDefault.greenChannelNumber;
               displaySpecsPtr->invertValuesFlag[1] = 
														gDisplaySpecsDefault.invertValuesFlag[1];
               displaySpecsPtr->blueChannelNumber = 
														gDisplaySpecsDefault.blueChannelNumber;
               displaySpecsPtr->invertValuesFlag[0] = 
														gDisplaySpecsDefault.invertValuesFlag[0];
               displaySpecsPtr->enhancementCode = 
														gDisplaySpecsDefault.enhancementCode;
               displaySpecsPtr->minMaxCode = gDisplaySpecsDefault.minMaxCode;
               displaySpecsPtr->percentTailsClipped = 
														gDisplaySpecsDefault.percentTailsClipped;
					displaySpecsPtr->pixelSize = gDisplaySpecsDefault.pixelSize;
               displaySpecsPtr->displayType = gDisplaySpecsDefault.lastDisplayType;
               displaySpecsPtr->displaySet = gDisplaySpecsDefault.displaySet;
               displaySpecsPtr->rgbColors = gDisplaySpecsDefault.rgbColors;
               displaySpecsPtr->backgroundValueCode = 
														gDisplaySpecsDefault.backgroundValueCode;
               displaySpecsPtr->backgroundDataValue = 
														gDisplaySpecsDefault.backgroundDataValue;

               }	// end "if (displaySpecsPtr->displayType == k3_ChannelDisplayType || ..."

            }	// end "if (gImageFileInfoPtr->dataTypeCode == ..."

         }	// end "if (gDisplaySpecsDefault.structureLoadedFlag)"

            // Indicate that the display specification structure has been
            // set up the first time.														

      displaySpecsPtr->firstTime = FALSE;

      }	// end "if (displaySpecsPtr->firstTime)"

   else	// "!displaySpecsPtr->firstTime

            // The display specifications have been set up at least once by
            // the user for this image window.  Just check to see if the item 
            // make sense.																		
      {
      if (displaySpecsPtr->lineStart > gImageWindowInfoPtr->maxNumberLines)
         displaySpecsPtr->lineStart = 1;
      if (displaySpecsPtr->lineEnd > gImageWindowInfoPtr->maxNumberLines)
         displaySpecsPtr->lineEnd = gImageWindowInfoPtr->maxNumberLines;
      if (displaySpecsPtr->lineInterval > gImageWindowInfoPtr->maxNumberLines)
         displaySpecsPtr->lineInterval = 1;

      if (displaySpecsPtr->columnStart > gImageWindowInfoPtr->maxNumberColumns)
         displaySpecsPtr->columnStart = 1;
      if (displaySpecsPtr->columnEnd > gImageWindowInfoPtr->maxNumberColumns)
         displaySpecsPtr->columnEnd = gImageWindowInfoPtr->maxNumberColumns;
      if (displaySpecsPtr->columnInterval > gImageWindowInfoPtr->maxNumberColumns)
         displaySpecsPtr->columnInterval = 1;

      if (displaySpecsPtr->channelNumber >
											(SInt16)gImageWindowInfoPtr->totalNumberChannels)
         displaySpecsPtr->channelNumber = 1;

            // Do not load user selected rectangle information into display
            // specification information; just set up information so that the user
            // can use the entire/subset button to choose the selected area. We will 
            // assume that the user just made a selection
            // for zooming and otherwise and does not really want to display
            // a subset of the image.				

      GetSelectedAreaInfo (gActiveImageWindow,
									 gImageWindowInfoPtr,
									 (SInt32*)&longRect.top,
									 (SInt32*)&longRect.bottom,
									 (SInt32*)&longRect.left,
									 (SInt32*)&longRect.right,
									 kDontClearSelectionArea,
									 kUseThreshold,
									 kDontAdjustToBaseImage);

      }	// end "else !displaySpecsPtr->firstTime"

            // Set flag for default histogram file.

   displaySpecsPtr->defaultHistogramFileFlag = GetDefaultSupportFile (
																gImageWindowInfoPtr->windowInfoHandle,
																kISTAFileType);

         // Determine whether histogram statistics are loaded into memory
         // or whether they can be read from disk or will need to be			
         // computed.

   displaySpecsPtr->histogramCompute = GetHistogramComputeCode (displaySpecsPtr);

   return (displaySpecsPtr);

}	// end "LoadMultispectralDisplaySpecs"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean MinMaxEnhancementDialog
//
//	Software purpose:	The purpose of this routine is to handle the modal
//							dialog that allows the user to specify the minimum
//							and maximum data values to use the the display
//							enhancement.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			EnhancementPopUpMenu in SDisplayMultispectral.cpp
//
//	Coded By:			Larry L. Biehl			Date: 09/29/1993
//	Revised By:			Larry L. Biehl			Date: 12/16/2016	

Boolean MinMaxEnhancementDialog (
				SInt16*								channelsPtr,
				SInt16								rgbColors,
				SInt16								displayType,
				SInt16								numberLevels,
				SInt16*								percentTailsClippedPtr,
				SInt16*								minMaxSelectionPtr,
				double*								minMaxValuesPtr)

{
	Boolean								returnFlag;

	#if defined multispec_mac
		Rect									theBox;

		double								blueMaxValue,
												blueMinValue,
												greenMaxValue,
												greenMinValue,
												maxValue,
												minValue,
												redMaxValue,
												redMinValue,
												theRealValue;

		DialogPtr							dialogPtr;
		HistogramSpecsPtr					histogramSpecsPtr;
		HistogramSummaryPtr				histogramSummaryPtr;
		double*								valuePtr;

		Handle								theHandle;

		SInt32								theNum;

		SInt16								summaryIndex,
												itemHit,
												localMinMaxCode,
												numberEDecimalDigits,
												numberFDecimalDigits,
												savedMinMaxCode,
												theType;

		Boolean								checkValueFlag,
												histogramAvailableFlag = TRUE,
												modalDone,
												reloadValuesFlag,
												updateMinMaxFlag,
												valueChangedFlag,
												valueErrorFlag;


				// Get the modal dialog for the image display specification				

		dialogPtr = LoadRequestedDialog (kMinMaxDialogID, kCopyScrap, 1, 2);
		if (dialogPtr == NULL)
																							return (FALSE);

		MinMaxEnhancementDialogInitialize (dialogPtr,
														channelsPtr,
														displayType,
														numberLevels,
														rgbColors,
														*minMaxSelectionPtr,
														minMaxValuesPtr,
														&histogramSpecsPtr,
														&histogramSummaryPtr,
														&minValue,
														&maxValue,
														&localMinMaxCode,
														percentTailsClippedPtr,
														&histogramAvailableFlag);

				// Get the current min-max settings.

		redMinValue = GetDItemRealValue (dialogPtr, IDC_RedChannelMinE);
		redMaxValue = GetDItemRealValue (dialogPtr, IDC_RedChannelMaxE);
		greenMinValue = GetDItemRealValue (dialogPtr, IDC_GreenChannelMinE);
		greenMaxValue = GetDItemRealValue (dialogPtr, IDC_GreenChannelMaxE);
		blueMinValue = GetDItemRealValue (dialogPtr, IDC_BlueChannelMinE);
		blueMaxValue = GetDItemRealValue (dialogPtr, IDC_BlueChannelMaxE);

		checkValueFlag = FALSE;
		reloadValuesFlag = FALSE;
		updateMinMaxFlag = FALSE;
		valueChangedFlag = FALSE;
		valueErrorFlag = FALSE;

				// Set the default min-max code.													
				//		= 1: Use per channel min-max with % clip.									
				//		= 2: Use entire data range control.											
				//		= 3: User specified min-max.													

		SetDLogControl (
					dialogPtr, IDC_PerChannel, (localMinMaxCode == kPerChannelMinMax));
		ShowHideDialogItem (
					dialogPtr, IDC_PercentClipped, (localMinMaxCode == kPerChannelMinMax));

		SetDLogControl (
					dialogPtr, IDC_EntireRange, (localMinMaxCode == kEntireDataRange));

		SetDLogControl (
					dialogPtr, IDC_UserSpecified, (localMinMaxCode == kUserSpecified));

		SetDLogControl (
					dialogPtr, IDC_ThematicDefault, (localMinMaxCode == kThematicDefault));

				//	Percent to use for clipping. 													

		LoadDItemValue (
					dialogPtr, IDC_PercentClipped, (SInt32)*percentTailsClippedPtr);

				// Center the dialog and then show it.											

		ShowDialogWindow (dialogPtr, kMinMaxDialogID, kSetUpDFilterTable);

				// Allow negative data for the minimums if the data is signed.

		if (gImageFileInfoPtr->signedDataFlag) 
			{
			gDialogItemDescriptorPtr[9] = kDItemMinus + kDItemReal;
			gDialogItemDescriptorPtr[10] = kDItemMinus + kDItemReal;
			gDialogItemDescriptorPtr[15] = kDItemMinus + kDItemReal;
			gDialogItemDescriptorPtr[16] = kDItemMinus + kDItemReal;
			gDialogItemDescriptorPtr[21] = kDItemMinus + kDItemReal;
			gDialogItemDescriptorPtr[22] = kDItemMinus + kDItemReal;

			}	// end "if (gImageFileInfoPtr->signedDataFlag)"

				// Set default text selection to first edit text item						

		itemHit = 9;
		if (localMinMaxCode == kPerChannelMinMax)
			itemHit = IDC_PercentClipped;
		SelectDialogItemText (dialogPtr, itemHit, 0, INT16_MAX);

		modalDone = FALSE;
		itemHit = 0;
		do {
			ModalDialog (gProcessorDialogFilterPtr, &itemHit);
			if (itemHit > 2) 
				{
						// If itemHit was a number item, check for bad values.			
						// If itemHit was a radio button make appropriate control		
						// settings to indicate to the user the present selection.		
						// Get the handle to the itemHit.										
						// For number value items, get the string and convert it to		
						// a number.																	

				GetDialogItem (dialogPtr, itemHit, &theType, &theHandle, &theBox);
				if (theType == 16) 
					{
					GetDialogItemText (theHandle, gTextString);
					StringToNum (gTextString, &theNum);

					}	// end "if (theType == 16)" 

				switch (itemHit) 
					{
					case 9: //	 Minimum for channel to be used for red
						checkValueFlag = TRUE;
						valuePtr = &redMinValue;
						summaryIndex = channelsPtr[0] - 1;
						break;

					case 10: //	 Maximum for channel to be used for red 
						checkValueFlag = TRUE;
						valuePtr = &redMaxValue;
						summaryIndex = channelsPtr[0] - 1;
						break;

					case 15: //	 Minimum for channel to be used for green  
						checkValueFlag = TRUE;
						valuePtr = &greenMinValue;
						summaryIndex = channelsPtr[1] - 1;
						break;

					case 16: //	 Maximum for channel to be used for green  
						checkValueFlag = TRUE;
						valuePtr = &greenMaxValue;
						summaryIndex = channelsPtr[1] - 1;
						break;

					case 21: //	 Minimum for channel to be used for blue  
						checkValueFlag = TRUE;
						valuePtr = &blueMinValue;
						summaryIndex = channelsPtr[2] - 1;
						break;

					case 22: //	 Maximum for channel to be used for blue
						checkValueFlag = TRUE;
						valuePtr = &blueMaxValue;
						summaryIndex = channelsPtr[2] - 1;
						break;

					case IDC_PerChannel:
					case IDC_EntireRange:
					case IDC_UserSpecified:
					case IDC_ThematicDefault:
						SetDLogControl (
								dialogPtr, IDC_PerChannel, (itemHit == IDC_PerChannel));
						SetDLogControl (
								dialogPtr, IDC_EntireRange, (itemHit == IDC_EntireRange));
						SetDLogControl (
								dialogPtr, IDC_UserSpecified, (itemHit == IDC_UserSpecified));
						SetDLogControl (dialogPtr, 
												IDC_ThematicDefault, 
												(itemHit == IDC_ThematicDefault));

						savedMinMaxCode = localMinMaxCode;

						if (MinMaxEnhancementDialogOnMinMaxCode (
																		dialogPtr,
																		&localMinMaxCode,
																		itemHit - 21, // newMinMaxCode
																		*percentTailsClippedPtr,
																		histogramSummaryPtr,
																		histogramAvailableFlag)) 
							{
							UpdateEnhancementMinMaxes (histogramSpecsPtr,
																  dialogPtr,
																  localMinMaxCode,
																  *percentTailsClippedPtr,
																  numberLevels,
																  minMaxValuesPtr,
																  channelsPtr,
																  FALSE);

							reloadValuesFlag = TRUE;

							}	// end "if (MinMaxEnhancementDialogOnMinMaxCode (..."

						MinMaxEnhancementDialogSetSelection (
																	dialogPtr,
																	savedMinMaxCode,
																	localMinMaxCode); // newMinMaxCode
						break;

					case IDC_PercentClipped:	// Percent to use to clip at  
						if (theNum > 100)
							NumberDefault (theHandle, 
												(SInt32)*percentTailsClippedPtr, 
												(Str255*)gTextString);

						else	// theNum <= 100 
							{
							if (*percentTailsClippedPtr != theNum && histogramAvailableFlag)
								updateMinMaxFlag = TRUE;

							*percentTailsClippedPtr = theNum;

							if (*percentTailsClippedPtr == 0 && !histogramAvailableFlag)
								updateMinMaxFlag = TRUE;

							}	// end "else theNum <=> 100" 
						break;

					}	// end "switch (itemHit)" 

				if (checkValueFlag) 
					{
					theRealValue = GetDItemRealValue (dialogPtr, itemHit);

					if (theRealValue < minValue) 
						{
						valueErrorFlag = TRUE;
						//NumberDefault (theHandle, minValue, gTextString);
						theRealValue = minValue;

						}	// end "if (theNum < minValue)"

					else if (theRealValue > maxValue) 
						{
						valueErrorFlag = TRUE;
						//NumberDefault (theHandle, maxValue, gTextString);
						theRealValue = maxValue;

						}	// end "else if (theNum > maxValue)"

					if (valueErrorFlag) 
						{
						if (histogramSummaryPtr != NULL) 
							{
							numberEDecimalDigits = 
										histogramSummaryPtr[summaryIndex].numberEDecimalDigits;
							numberFDecimalDigits = 
										histogramSummaryPtr[summaryIndex].numberFDecimalDigits;

							}	// end "if (histogramSummaryPtr != NULL)"

						else	// histogramSummaryPtr == NULL
							{
							numberEDecimalDigits = 
													gImageFileInfoPtr->maxNumberEDecimalDigits;
							numberFDecimalDigits = 
													gImageFileInfoPtr->maxNumberFDecimalDigits;

							}	// end "else histogramSummaryPtr == NULL"

						RealNumberErrorAlert (theRealValue,
													 dialogPtr,
													 itemHit,
													 numberFDecimalDigits,
													 numberEDecimalDigits);
						valueErrorFlag = FALSE;

						}	// end "if (valueErrorFlag)"

					if (theRealValue != *valuePtr) 
						{
						valueChangedFlag = TRUE;
						*valuePtr = theRealValue;

						}	// end "if (theRealValue != *ValuePtr)"

					if (valueChangedFlag) 
						{
						SetDLogControl (dialogPtr, IDC_PerChannel, 0);
						SetDLogControl (dialogPtr, IDC_EntireRange, 0);
						SetDLogControl (dialogPtr, IDC_UserSpecified, 1);
						SetDLogControl (dialogPtr, IDC_ThematicDefault, 0);

						HideDialogItem (dialogPtr, IDC_PercentClipped);

						SetDLogControlHilite (dialogPtr, IDC_UserSpecified, 0);

						localMinMaxCode = kUserSpecified;

						valueChangedFlag = FALSE;

						}	// end "if (valueChangedFlag)"

					checkValueFlag = FALSE;

					}	// end "if (checkValueFlag)" 

				if (updateMinMaxFlag) 
					{
					UpdateEnhancementMinMaxes (histogramSpecsPtr,
														dialogPtr,
														localMinMaxCode,
														*percentTailsClippedPtr,
														numberLevels,
														minMaxValuesPtr,
														channelsPtr,
														FALSE);

					updateMinMaxFlag = FALSE;

					reloadValuesFlag = TRUE;

					}	// end "if (updateMinMaxFlag)" 

				if (reloadValuesFlag) 
					{
							// Get the current min-max settings.

					redMinValue = GetDItemRealValue (dialogPtr, IDC_RedChannelMinE);
					redMaxValue = GetDItemRealValue (dialogPtr, IDC_RedChannelMaxE);
					greenMinValue = GetDItemRealValue (dialogPtr, IDC_GreenChannelMinE);
					greenMaxValue = GetDItemRealValue (dialogPtr, IDC_GreenChannelMaxE);
					blueMinValue = GetDItemRealValue (dialogPtr, IDC_BlueChannelMinE);
					blueMaxValue = GetDItemRealValue (dialogPtr, IDC_BlueChannelMaxE);

					reloadValuesFlag = FALSE;

					}	// end "if (reloadValuesFlag)"

				}	// end "if (itemHit > 2)" 

			else if (itemHit > 0) // and itemHit <= 2 
				{
				modalDone = TRUE;

				if (itemHit == 1) // User selected OK for information 
					{
					MinMaxEnhancementDialogOK (
											histogramSummaryPtr,
											channelsPtr,
											minMaxValuesPtr,
											minMaxSelectionPtr,
											percentTailsClippedPtr,
											GetDItemRealValue (dialogPtr, IDC_RedChannelMinE),
											GetDItemRealValue (dialogPtr, IDC_RedChannelMaxE),
											GetDItemRealValue (dialogPtr, IDC_GreenChannelMinE),
											GetDItemRealValue (dialogPtr, IDC_GreenChannelMaxE),
											GetDItemRealValue (dialogPtr, IDC_BlueChannelMinE),
											GetDItemRealValue (dialogPtr, IDC_BlueChannelMaxE),
											localMinMaxCode,
											GetDItemValue (dialogPtr, IDC_PercentClipped));

					returnFlag = TRUE;

					}	// end "if (itemHit == 1)" 

				if (itemHit == 2) // User selected Cancel for information 
					returnFlag = FALSE;

				}	// end "else if (itemHit > 0)" 

			} while (!modalDone);

				// Handle needs to stay locked. It will be unlocked later.

		//CheckAndUnlockHandle (gImageWindowInfoPtr->histogramSpecsHandle);
		CheckAndUnlockHandle (gImageWindowInfoPtr->histogramSummaryHandle);

		CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
	#endif	// defined multispec_mac	

	#if defined multispec_win   
		CMDisplayMinMaxDialog* dialogPtr = NULL;

		TRY
			{
			dialogPtr = new CMDisplayMinMaxDialog ();

			returnFlag = dialogPtr->DoDialog (channelsPtr,
															rgbColors,
															displayType,
															numberLevels,
															percentTailsClippedPtr,
															minMaxSelectionPtr,
															minMaxValuesPtr);

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
		CMDisplayMinMaxDialog* dialogPtr = NULL;

		dialogPtr = new CMDisplayMinMaxDialog (NULL);
      
		returnFlag = dialogPtr->DoDialog (channelsPtr,
														rgbColors,
														displayType,
														numberLevels,
														percentTailsClippedPtr,
														minMaxSelectionPtr,
														minMaxValuesPtr);

		delete dialogPtr;
	#endif

	return (returnFlag);

}	// end "MinMaxEnhancementDialog" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void MinMaxEnhancementDialogInitialize
//
//	Software purpose:	The purpose of this routine is to initialize the variables
//							and settings to be used for the MinMaxEnhancement dialog box.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			MinMaxEnhancementDialog in SDisplayMultispectral.cpp
//
//	Coded By:			Larry L. Biehl			Date: 09/29/1993
//	Revised By:			Larry L. Biehl			Date: 11/13/2019

void MinMaxEnhancementDialogInitialize (
				DialogPtr							dialogPtr,
				SInt16*								channelsPtr,
				SInt16								displayType,
				SInt16								numberLevels,
				SInt16								rgbColors,
				SInt16								minMaxSelection,
				double*								minMaxValuesPtr,
				HistogramSpecsPtr*				histogramSpecsPtrPtr,
				HistogramSummaryPtr*				histogramSummaryPtrPtr,
				double*								minValuePtr,
				double*								maxValuePtr,
				SInt16*								localMinMaxCodePtr,
				SInt16*								percentTailsClippedPtr,
				Boolean*								histogramAvailableFlagPtr)

{
	HistogramSummaryPtr				histogramSummaryPtr;

	Handle								histogramSpecsHandle;

	double								blueMinValue = 0,
											blueMaxValue = 0,
											greenMinValue = 0,
											greenMaxValue = 0,
											redMinValue = 0,
											redMaxValue = 0;

	SInt16								blueChannel,
											greenChannel,
											redSingleChannel;


	redSingleChannel = channelsPtr[0];
	greenChannel = channelsPtr[1];
	blueChannel = channelsPtr[2];

			// Get the histogram information for the channels.							

	*histogramSpecsPtrPtr = NULL;
	*histogramSummaryPtrPtr = NULL;

	histogramSpecsHandle = GetHistogramSpecsHandle (gImageWindowInfoPtr);

	if (histogramSpecsHandle != NULL) 
		{
				// The handle is already locked.

		*histogramSpecsPtrPtr = 
								(HistogramSpecsPtr)GetHandlePointer (histogramSpecsHandle);

		*histogramSummaryPtrPtr = (*histogramSpecsPtrPtr)->histogramSummaryPtr;

		if (!(*histogramSpecsPtrPtr)->loadedFlag) 
			{
			*histogramSpecsPtrPtr = NULL;
			*histogramSummaryPtrPtr = NULL;

			}	// end "if (!(*histogramSpecsPtrPtr)->loadedFlag)"

		}	// end "if (histogramSpecsHandle != NULL)" 

			// Initialize other local variables.												

	*minValuePtr = gImageFileInfoPtr->minUsableDataValue;
	*maxValuePtr = gImageFileInfoPtr->maxUsableDataValue;

			// Set local min-max code.	
			//		= 1: Use per channel min-max with % clip.									
			//		= 2: Use entire data range control.											
			//		= 3: User specified min-max.		

	*localMinMaxCodePtr = minMaxSelection;

	if (*localMinMaxCodePtr == kPerChannelMinMax) 
		{
				// Verify that that the support information exists.

		if (!CheckIfDefaultHistogramInfoExists (gImageWindowInfoPtr)) 
			{
			*percentTailsClippedPtr = 0;
			*histogramAvailableFlagPtr = FALSE;

			}	// end "if (!CheckIfDefaultHistogramInfoExists (gImageWindowInfoPtr))"

		}	// end "if (*localMinMaxCodePtr == kPerChannelMinMax)	"

			// Display appropriate dialog items depending on the type of display.	

	HideDialogItem (dialogPtr, IDC_NotCalculatedMessage);
	HideDialogItem (dialogPtr, IDC_NotCalculatedMessage2);

	switch (displayType) 
		{
		case k1_ChannelThematicDisplayType: // 1 channel thematic display
		case k1_ChannelGrayLevelDisplayType: // 1 channel grey display
			LoadDItemStringNumber (kDialogStrID,
										  IDS_Dialog30,
										  dialogPtr,
										  IDC_RedChannelPrompt,
										  (Str255*)gTextString);

			if (*histogramSummaryPtrPtr != NULL) 
				{
				redMinValue = (*histogramSummaryPtrPtr)[
														redSingleChannel - 1].minValue;

				redMaxValue = (*histogramSummaryPtrPtr)[
														redSingleChannel - 1].maxValue;

				}	// end "if (*histogramSummaryPtrPtr != NULL)" 

			channelsPtr[1] = -1;
			channelsPtr[2] = -1;
			break;

		case k3_ChannelDisplayType: // 3 channel display
		//case 4: // 2 channel display

					// red channel number													

			if (rgbColors & 4) 
				{
				if (*histogramSummaryPtrPtr != NULL) 
					{
					redMinValue = (*histogramSummaryPtrPtr)[redSingleChannel - 1].minValue;

					redMaxValue = (*histogramSummaryPtrPtr)[redSingleChannel - 1].maxValue;

					}	// end "if (*histogramSummaryPtrPtr != NULL)" 

				}	// end "if (rgbColors & 4)" 

			else	// !(rgbColors & 4) 
				channelsPtr[0] = -1;

					// green channel number													

			if (rgbColors & 2) 
				{
				if (*histogramSummaryPtrPtr != NULL) 
					{
					greenMinValue = (*histogramSummaryPtrPtr)[greenChannel - 1].minValue;

					greenMaxValue = (*histogramSummaryPtrPtr)[greenChannel - 1].maxValue;

					}	// end "if (*histogramSummaryPtrPtr != NULL)" 

				}	// end "if (rgbColors & 2)" 

			else	// !(rgbColors & 2) 
				channelsPtr[1] = -1;

					// blue channel number														

			if (rgbColors & 1) 
				{
				if (*histogramSummaryPtrPtr != NULL) 
					{
					blueMinValue = (*histogramSummaryPtrPtr)[blueChannel - 1].minValue;

					blueMaxValue = (*histogramSummaryPtrPtr)[blueChannel - 1].maxValue;

					}	// end "if (*histogramSummaryPtrPtr != NULL)" 

				}	// end "if (rgbColors & 1)" 

			else	// !(rgbColors & 1) 
				channelsPtr[2] = -1;
			break;

		case kSideSideChannelDisplayType:
			HideDialogItem (dialogPtr, IDC_TableTitle);
			HideDialogItem (dialogPtr, IDC_UserSpecified);
			channelsPtr[0] = -1;
			channelsPtr[1] = -1;
			channelsPtr[2] = -1;
			break;

		}	// end "switch (displayType)"

			// Hide specific dialog items depending upon the channels not used.

	if (channelsPtr[0] == -1) 
		{
		HideDialogItem (dialogPtr, IDC_RedChannelPrompt);
		HideDialogItem (dialogPtr, IDC_RedChannel);
		HideDialogItem (dialogPtr, IDC_RedChannelMin);
		HideDialogItem (dialogPtr, IDC_RedChannelMax);
		HideDialogItem (dialogPtr, IDC_RedChannelMinE);
		HideDialogItem (dialogPtr, IDC_RedChannelMaxE);

		}	// end "if (channelsPtr[0] == -1 || ...)"

	if (channelsPtr[1] == -1) 
		{
		HideDialogItem (dialogPtr, IDC_GreenChannelPrompt);
		HideDialogItem (dialogPtr, IDC_GreenChannel);
		HideDialogItem (dialogPtr, IDC_GreenChannelMin);
		HideDialogItem (dialogPtr, IDC_GreenChannelMax);
		HideDialogItem (dialogPtr, IDC_GreenChannelMinE);
		HideDialogItem (dialogPtr, IDC_GreenChannelMaxE);

		}	// end "if (channelsPtr[1] == -1 || ...)"

	if (channelsPtr[2] == -1) 
		{
		HideDialogItem (dialogPtr, IDC_BlueChannelPrompt);
		HideDialogItem (dialogPtr, IDC_BlueChannel);
		HideDialogItem (dialogPtr, IDC_BlueChannelMin);
		HideDialogItem (dialogPtr, IDC_BlueChannelMax);
		HideDialogItem (dialogPtr, IDC_BlueChannelMinE);
		HideDialogItem (dialogPtr, IDC_BlueChannelMaxE);

		}	// end "if (channelsPtr[2] == -1 || ...)"

	if (*histogramSummaryPtrPtr == NULL) 
		{
		if (*localMinMaxCodePtr == kPerChannelMinMax) 
			{
			HideDialogItem (dialogPtr, IDC_RedChannelMinE);
			HideDialogItem (dialogPtr, IDC_RedChannelMaxE);
			HideDialogItem (dialogPtr, IDC_GreenChannelMinE);
			HideDialogItem (dialogPtr, IDC_GreenChannelMaxE);
			HideDialogItem (dialogPtr, IDC_BlueChannelMinE);
			HideDialogItem (dialogPtr, IDC_BlueChannelMaxE);

			ShowDialogItem (dialogPtr, IDC_NotCalculatedMessage2);

			}	// end "if (*localMinMaxCodePtr == kPerChannelMinMax)"

		else if (*localMinMaxCodePtr == kThematicDefault) 
			{
			HideDialogItem (dialogPtr, IDC_RedChannelMinE);
			HideDialogItem (dialogPtr, IDC_RedChannelMaxE);

			ShowDialogItem (dialogPtr, IDC_NotCalculatedMessage2);

			}	// end "if (*localMinMaxCodePtr == kThematicDefault)"

		HideDialogItem (dialogPtr, IDC_RedChannelMin);
		HideDialogItem (dialogPtr, IDC_RedChannelMax);
		HideDialogItem (dialogPtr, IDC_GreenChannelMin);
		HideDialogItem (dialogPtr, IDC_GreenChannelMax);
		HideDialogItem (dialogPtr, IDC_BlueChannelMin);
		HideDialogItem (dialogPtr, IDC_BlueChannelMax);

		ShowDialogItem (dialogPtr, IDC_NotCalculatedMessage);

		}	// end "if (*histogramSummaryPtrPtr == NULL)" 

			// Load the fixed text items.
	if (channelsPtr[0] >= 0)
		LoadDItemValue (dialogPtr, IDC_RedChannel, (SInt32)channelsPtr[0]);

	if (channelsPtr[1] >= 0)
		LoadDItemValue (dialogPtr, IDC_GreenChannel, (SInt32)channelsPtr[1]);

	if (channelsPtr[2] >= 0)
		LoadDItemValue (dialogPtr, IDC_BlueChannel, (SInt32)channelsPtr[2]);

	if (*histogramSummaryPtrPtr != NULL) 
		{
		histogramSummaryPtr = *histogramSummaryPtrPtr;

		if (channelsPtr[0] >= 0) 
			{
			LoadDItemRealValue (
								dialogPtr,
								IDC_RedChannelMin,
								redMinValue,
								histogramSummaryPtr[channelsPtr[0] - 1].numberFDecimalDigits,
								histogramSummaryPtr[channelsPtr[0] - 1].numberEDecimalDigits);
			LoadDItemRealValue (
								dialogPtr,
								IDC_RedChannelMax,
								redMaxValue,
								histogramSummaryPtr[channelsPtr[0] - 1].numberFDecimalDigits,
								histogramSummaryPtr[channelsPtr[0] - 1].numberEDecimalDigits);

			}	// end "if (channelsPtr[0] >= 0)"

		if (channelsPtr[1] >= 0) 
			{
			LoadDItemRealValue (
								dialogPtr,
								IDC_GreenChannelMin,
								greenMinValue,
								histogramSummaryPtr[channelsPtr[1] - 1].numberFDecimalDigits,
								histogramSummaryPtr[channelsPtr[1] - 1].numberEDecimalDigits);
			LoadDItemRealValue (
								dialogPtr,
								IDC_GreenChannelMax,
								greenMaxValue,
								histogramSummaryPtr[channelsPtr[1] - 1].numberFDecimalDigits,
								histogramSummaryPtr[channelsPtr[1] - 1].numberEDecimalDigits);

			}	// end "if (channelsPtr[1] >= 0)"

		if (channelsPtr[2] >= 0) 
			{
			LoadDItemRealValue (
								dialogPtr,
								IDC_BlueChannelMin,
								blueMinValue,
								histogramSummaryPtr[channelsPtr[2] - 1].numberFDecimalDigits,
								histogramSummaryPtr[channelsPtr[2] - 1].numberEDecimalDigits);
			LoadDItemRealValue (
								dialogPtr,
								IDC_BlueChannelMax,
								blueMaxValue,
								histogramSummaryPtr[channelsPtr[2] - 1].numberFDecimalDigits,
								histogramSummaryPtr[channelsPtr[2] - 1].numberEDecimalDigits);

			}	// end "if (channelsPtr[2] >= 0)"

		}	// end "if (*histogramSummaryPtrPtr != NULL)"

	UpdateEnhancementMinMaxes (*histogramSpecsPtrPtr,
										dialogPtr,
										*localMinMaxCodePtr,
										*percentTailsClippedPtr,
										numberLevels,
										minMaxValuesPtr,
										channelsPtr,
										TRUE);

			// Set default min-max code items.													
			//		= 1: Use per channel min-max with % clip.									
			//		= 2: Use entire data range control.											
			//		= 3: User specified min-max.	
			//		= 4: Use thematic default.												

	ShowHideDialogItem (dialogPtr,
								IDC_PercentClipped,
								(*localMinMaxCodePtr == kPerChannelMinMax));

	if (minMaxSelection != kUserSpecified)
		SetDLogControlHilite (dialogPtr, IDC_UserSpecified, 255);

	ShowHideDialogItem (dialogPtr,
								IDC_ThematicDefault,
								(displayType == k1_ChannelThematicDisplayType));

}	// end "MinMaxEnhancementDialogInitialize"



void MinMaxEnhancementDialogOK (
				HistogramSummaryPtr				histogramSummaryPtr,
				SInt16*								channelsPtr,
				double*								minMaxValuesPtr,
				SInt16*								minMaxSelectionPtr,
				SInt16*								percentTailsClippedPtr,
				double								redMinValue,
				double								redMaxValue,
				double								greenMinValue,
				double								greenMaxValue,
				double								blueMinValue,
				double								blueMaxValue,
				SInt16								localMinMaxCode,
				SInt16								percentTailsClipped)
 
{
	minMaxValuesPtr[0] = redMinValue;
	minMaxValuesPtr[1] = redMaxValue;
	minMaxValuesPtr[2] = greenMinValue;
	minMaxValuesPtr[3] = greenMaxValue;
	minMaxValuesPtr[4] = blueMinValue;
	minMaxValuesPtr[5] = blueMaxValue;

	*minMaxSelectionPtr = localMinMaxCode;

	*percentTailsClippedPtr = percentTailsClipped;

}	// end "MinMaxEnhancementDialogOK"  



Boolean MinMaxEnhancementDialogOnMinMaxCode (
				DialogPtr							dialogPtr,
				SInt16*								currentMinMaxCodePtr,
				SInt16								newMinMaxCode,
				SInt16								percentTailsClipped,
				HistogramSummaryPtr				histogramSummaryPtr,
				Boolean								histogramAvailableFlag)

{
	Boolean								updateMinMaxFlag = FALSE;


	if (newMinMaxCode != *currentMinMaxCodePtr) 
		{
		updateMinMaxFlag = TRUE;

		if (*currentMinMaxCodePtr == kUserSpecified) 
			{
					// Unhilite the user specified setting.

			SetDLogControlHilite (dialogPtr, IDC_UserSpecified, 255);

			}	// end "if (savedMinMaxCode == kUserSpecified)"

		}	// end "if (newMinMaxCode != *currentMinMaxCodePtr)"   

	ShowHideDialogItem (dialogPtr,
								IDC_PercentClipped,
								(newMinMaxCode == kPerChannelMinMax));

	if (histogramSummaryPtr == NULL) 
		{
		if (newMinMaxCode == kPerChannelMinMax && percentTailsClipped > 0) 
			{
			updateMinMaxFlag = FALSE;

			if (*currentMinMaxCodePtr != kPerChannelMinMax) 
				{
				HideDialogItem (dialogPtr, IDC_RedChannelMinE);
				HideDialogItem (dialogPtr, IDC_RedChannelMaxE);
				HideDialogItem (dialogPtr, IDC_GreenChannelMinE);
				HideDialogItem (dialogPtr, IDC_GreenChannelMaxE);
				HideDialogItem (dialogPtr, IDC_BlueChannelMinE);
				HideDialogItem (dialogPtr, IDC_BlueChannelMaxE);

				ShowDialogItem (dialogPtr, IDC_NotCalculatedMessage2);

				}	// end "if (*currentMinMaxCodePtr != kPerChannelMinMax)"  

			}	// end "if (newMinMaxCode == kPerChannelMinMax ..."  

		if (*currentMinMaxCodePtr == kPerChannelMinMax &&
				 newMinMaxCode != kPerChannelMinMax) 
			{
			ShowDialogItem (dialogPtr, IDC_RedChannelMinE);
			ShowDialogItem (dialogPtr, IDC_RedChannelMaxE);
			ShowDialogItem (dialogPtr, IDC_GreenChannelMinE);
			ShowDialogItem (dialogPtr, IDC_GreenChannelMaxE);
			ShowDialogItem (dialogPtr, IDC_BlueChannelMinE);
			ShowDialogItem (dialogPtr, IDC_BlueChannelMaxE);

			HideDialogItem (dialogPtr, IDC_NotCalculatedMessage2);

			}	// end "if (*currentMinMaxCodePtr == kPerChannelMinMax && ..."    

		}	// end "if (histogramSummaryPtr == NULL)"

	*currentMinMaxCodePtr = newMinMaxCode;

	return (updateMinMaxFlag);

}	// end "MinMaxEnhancementDialogOnMinMaxCode"  



void MinMaxEnhancementDialogSetSelection (
				DialogPtr							dialogPtr,
				SInt16								lastMinMaxCode,
				SInt16								newMinMaxCode)

{
	SInt16								selectedItem = 0;


	if (lastMinMaxCode != kPerChannelMinMax &&
												newMinMaxCode == kPerChannelMinMax)
		selectedItem = IDC_PercentClipped;

	else if (lastMinMaxCode != kEntireDataRange &&
												newMinMaxCode == kEntireDataRange)
		selectedItem = IDC_RedChannelMinE;

	if (selectedItem != 0)
		SelectDialogItemText (dialogPtr, selectedItem, 0, SInt16_MAX);

}	// end "MinMaxEnhancementDialogSetSelection"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetImageWTitle
//
//	Software purpose:	The purpose of this routine is to put the channels
//							being displayed into the window title
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
//	Called By:			DisplayMultispectralImage in SDisplayMultispectral.cpp
//
//	Coded By:			Ravi Budruk 			Date: 07/11/1988
//	Revised By:			Larry L. Biehl			Date: 09/05/2017	

void SetImageWTitle (
				WindowPtr							theWindow,
				DisplaySpecsPtr					displaySpecsPtr,
				FileInfoPtr							fileInfoPtr)

{
	GetImageWindowName (displaySpecsPtr,
								fileInfoPtr,
								gTextString,
								FALSE);

	#if defined multispec_mac
		MSetWindowTitle (theWindow, gTextString);

				// Update the image window name in the Window menu.				

		MSetMenuItemText (gMultiSpecMenus[kWindowM],
									gImageWindowInfoPtr->windowMenuItem,
									gTextString,
									kASCIICharString);
	#endif	// defined multispec_mac    

	#if defined multispec_win
		MSetWindowTitle ((CDocument*)gActiveImageViewCPtr->GetDocument (),
								&gTextString[0]);
	#endif	// defined multispec_win	

	#if defined multispec_wx
		wxString ntitle = wxString::FromUTF8 ((char*)&gTextString[1]);
		(theWindow->m_frame)->SetTitle (ntitle);
	#endif

}	// end "SetImageWTitle" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SaveDisplayStructureSettings
//
//	Software purpose:	The purpose of this routine is to save some of the settings
//							used to display the current image.  They may be used as the
//							defaults when a new image is displayed.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None				
// 
// Called By:			DisplayMultispectralImage in SDisplayMultispectral.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/05/2006
//	Revised By:			Larry L. Biehl			Date: 05/11/2020

void SaveDisplayStructureSettings (
				DisplaySpecsPtr					displaySpecsPtr,
				FileInfoPtr							imageFileInfoPtr,
				WindowInfoPtr						imageWindowInfoPtr)

{
			// Currently the settings saved are for thematic type displays.

	gDisplaySpecsDefault.lastDisplayType = displaySpecsPtr->displayType;
	gDisplaySpecsDefault.displaySet = displaySpecsPtr->displaySet;
	gDisplaySpecsDefault.rgbColors = displaySpecsPtr->rgbColors;

	gDisplaySpecsDefault.enhancementCode = displaySpecsPtr->enhancementCode;
	gDisplaySpecsDefault.minMaxCode = displaySpecsPtr->minMaxCode;
	gDisplaySpecsDefault.percentTailsClipped = displaySpecsPtr->percentTailsClipped;
	gDisplaySpecsDefault.pixelSize = displaySpecsPtr->pixelSize;

	gDisplaySpecsDefault.dataTypeCode = imageWindowInfoPtr->dataTypeCode;
	gDisplaySpecsDefault.numberBytes = imageWindowInfoPtr->numberBytes;
	gDisplaySpecsDefault.numberChannels = imageWindowInfoPtr->totalNumberChannels;
	gDisplaySpecsDefault.signedDataFlag = imageFileInfoPtr->signedDataFlag;
	gDisplaySpecsDefault.backgroundValueCode = displaySpecsPtr->backgroundValueCode;
	gDisplaySpecsDefault.backgroundDataValue = displaySpecsPtr->backgroundDataValue;

	gDisplaySpecsDefault.magnification = displaySpecsPtr->magnification;

	if (displaySpecsPtr->displayType == k1_ChannelThematicDisplayType) 
		{
		gDisplaySpecsDefault.thematicTypeMinMaxValues[0] =
													displaySpecsPtr->thematicTypeMinMaxValues[0];
		gDisplaySpecsDefault.thematicTypeMinMaxValues[1] =
													displaySpecsPtr->thematicTypeMinMaxValues[1];

		gDisplaySpecsDefault.channelNumber = displaySpecsPtr->channelNumber;
		gDisplaySpecsDefault.numberThematicLevels = displaySpecsPtr->numberLevels;
		gDisplaySpecsDefault.thematicValueFactor = displaySpecsPtr->thematicValueFactor;
		gDisplaySpecsDefault.invertValuesFlag[2] = displaySpecsPtr->invertValuesFlag[2];
		gDisplaySpecsDefault.oneChannelThematicClassPaletteType =
														displaySpecsPtr->thematicClassPaletteType;

		gDisplaySpecsDefault.structureLoadedFlag = TRUE;

		}	// end "if (displaySpecsPtr->displayType == k1_ChannelThematicDisplayType)"

	else if (displaySpecsPtr->displayType == k1_ChannelGrayLevelDisplayType) 
		{
		gDisplaySpecsDefault.channelNumber = displaySpecsPtr->channelNumber;
		gDisplaySpecsDefault.numberLevels = displaySpecsPtr->numberLevels;
		gDisplaySpecsDefault.invertValuesFlag[2] = displaySpecsPtr->invertValuesFlag[2];

		gDisplaySpecsDefault.structureLoadedFlag = TRUE;

		}	// end "if (displaySpecsPtr->displayType == k1_ChannelGrayLevelDisplayType)"

	else if (displaySpecsPtr->displayType == k3_ChannelDisplayType ||
				displaySpecsPtr->displayType == k3_2_ChannelDisplayType) // ||
			//gDisplaySpecsDefault.lastDisplayType == k2_ChannelDisplayType)
		{
		gDisplaySpecsDefault.redChannelNumber = displaySpecsPtr->redChannelNumber;
		gDisplaySpecsDefault.greenChannelNumber = displaySpecsPtr->greenChannelNumber;
		gDisplaySpecsDefault.blueChannelNumber = displaySpecsPtr->blueChannelNumber;
		gDisplaySpecsDefault.channelsInWavelengthOrderCode =
											imageWindowInfoPtr->channelsInWavelengthOrderCode;
		gDisplaySpecsDefault.invertValuesFlag[0] = displaySpecsPtr->invertValuesFlag[0];
		gDisplaySpecsDefault.invertValuesFlag[1] = displaySpecsPtr->invertValuesFlag[1];
		gDisplaySpecsDefault.invertValuesFlag[2] = displaySpecsPtr->invertValuesFlag[2];
		gDisplaySpecsDefault.numberLevels = displaySpecsPtr->numberLevels;

		gDisplaySpecsDefault.structureLoadedFlag = TRUE;

		}	// end "if (displaySpecsPtr->displayType == k3_ChannelDisplayType || ..."

	else	// 
		gDisplaySpecsDefault.structureLoadedFlag = FALSE;

}	// end "SaveDisplayStructureSettings"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetUpMinMaxPopUpMenu
//
//	Software purpose:	The purpose of this routine is to set up the min-max popup menu
//							for the display dialog.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/05/2006
//	Revised By:			Larry L. Biehl			Date: 01/03/2020

void SetUpMinMaxPopUpMenu (
				DialogPtr							dialogPtr,
				SInt16								displayType)

{
	#if defined multispec_mac
		EnableMenuItem (gPopUpMinMaxMenu, kPerChannelMinMaxNoClip);
		EnableMenuItem (gPopUpMinMaxMenu, kPerChannelMinMax);
		EnableMenuItem (gPopUpMinMaxMenu, kEntireDataRange);
		EnableMenuItem (gPopUpMinMaxMenu, kUserSpecified);

		if (displayType == k1_ChannelThematicDisplayType)
			EnableMenuItem (gPopUpMinMaxMenu, kThematicDefault);
	
		else	// displayType != k1_ChannelThematicDisplayType
			DisableMenuItem (gPopUpMinMaxMenu, kThematicDefault);
	#endif	// defined multispec_mac 

	#if defined multispec_win
		CComboBox* comboBoxPtr =
								(CComboBox*)(dialogPtr->GetDlgItem (ID3C_MinMaxValues));

		if (comboBoxPtr != NULL) 
			{
			comboBoxPtr->DeleteString (4);

			if (displayType == k1_ChannelThematicDisplayType)
				comboBoxPtr->InsertString (4, (LPCTSTR)_T("Thematic Default"));

			}	// end "if (comboBoxPtr != NULL)"
	#endif	// defined multispec_win 

	#if defined multispec_wx
		#if defined multispec_wxlin
			wxComboBox* comboBoxPtr =
								(wxComboBox*)(dialogPtr->FindWindow (ID3C_MinMaxValues));
		#endif
		#if defined multispec_wxmac
			wxChoice* comboBoxPtr =
								(wxChoice*)(dialogPtr->FindWindow (ID3C_MinMaxValues));
		#endif
	
		if (comboBoxPtr != NULL) 
			{
			if (comboBoxPtr->GetCount () > 4)
				comboBoxPtr->Delete (4);

			if (displayType == k1_ChannelThematicDisplayType)
				comboBoxPtr->Insert (wxT("Thematic Default"), 4);

			}	// end "if (comboBoxPtr != NULL)"
	#endif	// defined multispec_wx 

}	// end Routine "SetUpMinMaxPopUpMenu" 



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetDialogItemDrawRoutine
//
//	Software purpose:	The purpose of this routine is to set the input draw
//							routine for the specified dialog and item number.
//
//	Parameters in:		The dialog pointer.
//							The dialog item number.
//							Address to the draw routine.
//
//	Parameters out:	Rectangle for the input item number.
//
// Value Returned:	None.			
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 09/09/2001
//	Revised By:			Larry L. Biehl			Date: 11/02/2001	

void UnSetDialogItemDrawRoutine (
				DialogPtr							dialogPtr,
				UserItemUPP							userItemProcPtr,
				SInt16								appearanceItemNumber,
				MenuHandle							inputPopupMenuHandle)

{
	ControlHandle						theControl;
	MenuHandle							nullPopupMenuHandle = NULL,
											popupMenuHandle;
	OSErr									errorCode;
	Size									size;


	if (gAppearanceManagerFlag)
		{
				// The following is done to be sure that the popup menus that were loaded
				// in the MultiSpec initialization are not released.

		errorCode = ::GetDialogItemAsControl (dialogPtr, 
															appearanceItemNumber, 
															&theControl);

		if (errorCode == noErr)
			errorCode = GetControlData (
									 theControl,
									 0,
									 kControlPopupButtonMenuHandleTag,
									 sizeof (MenuHandle),
									 (Ptr)&popupMenuHandle,
									 &size);

		if (errorCode == noErr && inputPopupMenuHandle == popupMenuHandle)
			{
			errorCode = SetControlData (
									  theControl,
									  0,
									  kControlPopupButtonMenuHandleTag,
									  sizeof (MenuHandle),
									  (Ptr)&nullPopupMenuHandle);

			}	// end "if (errorCode == noErr && ..."

		}	// end "if (gAppearanceManagerFlag)"

	else	// !gAppearanceManagerFlag
		{
		if (userItemProcPtr != NULL)
			DisposeUserItemUPP (userItemProcPtr);

		}	// end "else !gAppearanceManagerFlag"

}	// end "UnSetDialogItemDrawRoutine"
#endif	// defined multispec_mac	



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void UpdateDialogChannelItems
//
//	Software purpose:	The purpose of this routine is to update the dialog 
//							channel items in the multispectral display dialog.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/24/1993
//	Revised By:			Larry L. Biehl			Date: 11/12/2019

void UpdateDialogChannelItems (
				DialogPtr							dialogPtr,
				//SInt16								displayTypeSelection,
				SInt16								rgbColors,
				SInt16								displayType)

{
			// text fields for channels to be displayed

			// Side by side channel display.	
	#if defined multispec_mac				
		if (gAppearanceManagerFlag) 
			{
			ShowHideDialogItem (
								dialogPtr, 28, (displayType != kSideSideChannelDisplayType));
			ShowHideDialogItem (dialogPtr, 
										ID3C_Channels, 
										(displayType == kSideSideChannelDisplayType));

			}	// end "if (gAppearanceManagerFlag)"

		else	// !gAppearanceManagerFlag)
	#endif	// defined multispec_mac	
	  
	ShowHideDialogItem (
				dialogPtr, ID3C_Channels, (displayType == kSideSideChannelDisplayType));

	switch (displayType) 
		{
		case k1_ChannelThematicDisplayType:
			ShowDialogItem (dialogPtr, ID3C_GrayChannel);
			HideDialogItem (dialogPtr, ID3C_GrayPrompt);
			HideDialogItem (dialogPtr, ID3C_RedPrompt);
			HideDialogItem (dialogPtr, ID3C_RedChannel);
			ShowDialogItem (dialogPtr, IDC_RedChannelInvert);
			HideDialogItem (dialogPtr, ID3C_GreenPrompt);
			HideDialogItem (dialogPtr, ID3C_GreenChannel);
			HideDialogItem (dialogPtr, IDC_GreenChannelInvert);
			HideDialogItem (dialogPtr, ID3C_BluePrompt);
			HideDialogItem (dialogPtr, ID3C_BlueChannel);
			HideDialogItem (dialogPtr, IDC_BlueChannelInvert);
			ShowDialogItem (dialogPtr, IDC_LegendFactorPrompt);
			ShowDialogItem (dialogPtr, IDC_LegendFactor);
			break;

		case k1_ChannelGrayLevelDisplayType:
			ShowDialogItem (dialogPtr, ID3C_GrayChannel);
			ShowDialogItem (dialogPtr, ID3C_GrayPrompt);
			HideDialogItem (dialogPtr, ID3C_RedPrompt);
			HideDialogItem (dialogPtr, ID3C_RedChannel);
			ShowDialogItem (dialogPtr, IDC_RedChannelInvert);
			HideDialogItem (dialogPtr, ID3C_GreenPrompt);
			HideDialogItem (dialogPtr, ID3C_GreenChannel);
			HideDialogItem (dialogPtr, IDC_GreenChannelInvert);
			HideDialogItem (dialogPtr, ID3C_BluePrompt);
			HideDialogItem (dialogPtr, ID3C_BlueChannel);
			HideDialogItem (dialogPtr, IDC_BlueChannelInvert);
			HideDialogItem (dialogPtr, IDC_LegendFactorPrompt);
			HideDialogItem (dialogPtr, IDC_LegendFactor);
			break;

		//case k2_ChannelDisplayType:
		case k3_ChannelDisplayType:
		case k3_2_ChannelDisplayType:
			HideDialogItem (dialogPtr, ID3C_GrayChannel);
			HideDialogItem (dialogPtr, ID3C_GrayPrompt);

					// red channel number								

			ShowHideDialogItem (dialogPtr, ID3C_RedPrompt, (rgbColors & 4));
			ShowHideDialogItem (dialogPtr, ID3C_RedChannel, (rgbColors & 4));
			ShowHideDialogItem (dialogPtr, IDC_RedChannelInvert, (rgbColors & 4));

					// green channel number															

			ShowHideDialogItem (dialogPtr, ID3C_GreenPrompt, (rgbColors & 2));
			ShowHideDialogItem (dialogPtr, ID3C_GreenChannel, (rgbColors & 2));
			ShowHideDialogItem (dialogPtr, IDC_GreenChannelInvert, (rgbColors & 2));

					// blue channel number															

			ShowHideDialogItem (dialogPtr, ID3C_BluePrompt, (rgbColors & 1));
			ShowHideDialogItem (dialogPtr, ID3C_BlueChannel, (rgbColors & 1));
			ShowHideDialogItem (dialogPtr, IDC_BlueChannelInvert, (rgbColors & 1));

			HideDialogItem (dialogPtr, IDC_LegendFactorPrompt);
			HideDialogItem (dialogPtr, IDC_LegendFactor);
			break;

		case kSideSideChannelDisplayType:
			HideDialogItem (dialogPtr, ID3C_GrayChannel);
			ShowDialogItem (dialogPtr, ID3C_GrayPrompt);
			HideDialogItem (dialogPtr, ID3C_RedPrompt);
			HideDialogItem (dialogPtr, ID3C_RedChannel);
			ShowDialogItem (dialogPtr, IDC_RedChannelInvert);
			HideDialogItem (dialogPtr, ID3C_GreenPrompt);
			HideDialogItem (dialogPtr, ID3C_GreenChannel);
			HideDialogItem (dialogPtr, IDC_GreenChannelInvert);
			HideDialogItem (dialogPtr, ID3C_BluePrompt);
			HideDialogItem (dialogPtr, ID3C_BlueChannel);
			HideDialogItem (dialogPtr, IDC_BlueChannelInvert);
			HideDialogItem (dialogPtr, IDC_LegendFactorPrompt);
			HideDialogItem (dialogPtr, IDC_LegendFactor);
			break;

		}	// end "switch (displayType)"

			// Set default text selection.

	DisplayMultispectralDialogSetDefaultSelection (dialogPtr, rgbColors, displayType);

}	// end "UpdateDialogChannelItems"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void UpdateEnhancementMinMaxes
//
//	Software purpose:	The purpose of this routine is to update the
//							enhancement minimum and maximum values based on
//							the current user selection.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 10/01/1993
//	Revised By:			Larry L. Biehl			Date: 09/13/2007	

void UpdateEnhancementMinMaxes (
				HistogramSpecsPtr					histogramSpecsPtr,
				DialogPtr							dialogPtr,
				SInt16								localMinMaxCode,
				SInt16								percentTailsClipped,
				SInt16								numberLevels,
				double*								minMaxValuesPtr,
				SInt16*								channelsPtr,
				Boolean								updateUserSpecifiedFlag)

{
	double								blueMaxDataValue,
											blueMinDataValue,
											greenMaxDataValue,
											greenMinDataValue,
											redMaxDataValue,
											redMinDataValue;

	HistogramSummaryPtr				histogramSummaryPtr;

	SInt32								signedValueOffset;

	SInt16								blueNumberEDecimalDigits,
											blueNumberFDecimalDigits,
											greenNumberEDecimalDigits,
											greenNumberFDecimalDigits,
											redNumberEDecimalDigits,
											redNumberFDecimalDigits;

	Boolean								loadBlueChannelLimitsFlag = FALSE,
											loadGreenChannelLimitsFlag = FALSE,
											loadRedChannelLimitsFlag = FALSE;


	histogramSummaryPtr = NULL;

	if (channelsPtr[0] < 0 && channelsPtr[1] < 0 && channelsPtr[2] < 0)
		localMinMaxCode = -1;

	if (histogramSpecsPtr != NULL)
		histogramSummaryPtr = histogramSpecsPtr->histogramSummaryPtr;

	if (histogramSummaryPtr != NULL) 
		{
		signedValueOffset = gImageFileInfoPtr->signedValueOffset;

		if (localMinMaxCode == kPerChannelMinMax) 
			{
					//	Enhancement minimum and maximum for red channel. 					

			if (channelsPtr[0] >= 0) 
				{
				if (GetClippedMinMaxValues (
											histogramSpecsPtr,
											percentTailsClipped,
											channelsPtr[0] - 1,
											histogramSummaryPtr[channelsPtr[0] - 1].numberBins,
											&redMinDataValue,
											&redMaxDataValue,
											FALSE,
											FALSE))
					loadRedChannelLimitsFlag = TRUE;

				}	// end "if (channelsPtr[0] >= 0)" 

					//	Enhancement minimum and maximum for green channel. 				

			if (channelsPtr[1] >= 0) 
				{
				if (GetClippedMinMaxValues (
											histogramSpecsPtr,
											percentTailsClipped,
											channelsPtr[1] - 1,
											histogramSummaryPtr[channelsPtr[1] - 1].numberBins,
											&greenMinDataValue,
											&greenMaxDataValue,
											FALSE,
											FALSE))
					loadGreenChannelLimitsFlag = TRUE;

				}	// end "if (channelsPtr[1] >= 0)" 

					//	Enhancement minimum and maximum for blue channel. 					

			if (channelsPtr[2] >= 0) 
				{
				if (GetClippedMinMaxValues (
											histogramSpecsPtr,
											percentTailsClipped,
											channelsPtr[2] - 1,
											histogramSummaryPtr[channelsPtr[2] - 1].numberBins,
											&blueMinDataValue,
											&blueMaxDataValue,
											FALSE,
											FALSE))
					loadBlueChannelLimitsFlag = TRUE;

				}	// end "if (channelsPtr[2] >= 0)" 

					// Indicate that any offset has been taken into account.

			signedValueOffset = 0;

			}	// end "if (localMinMaxCode == kPerChannelMinMax)" 

		else if (localMinMaxCode == kEntireDataRange) 
			{
					//	Enhancement minimum and maximum for red channel. 	

			if (channelsPtr[0] >= 0) 
				{
				redMinDataValue = histogramSummaryPtr[channelsPtr[0] - 1].minValue;
				redMaxDataValue = histogramSummaryPtr[channelsPtr[0] - 1].maxValue;
				loadRedChannelLimitsFlag = TRUE;

				}	// end "if (channelsPtr[0] >= 0)"

					//	Enhancement minimum and maximum for green channel. 				

			if (channelsPtr[1] >= 0) 
				{
				greenMinDataValue = histogramSummaryPtr[channelsPtr[1] - 1].minValue;
				greenMaxDataValue = histogramSummaryPtr[channelsPtr[1] - 1].maxValue;
				loadGreenChannelLimitsFlag = TRUE;

				}	// end "if (channelsPtr[1] >= 0)"

					//	Enhancement minimum and maximum for blue channel. 					

			if (channelsPtr[2] >= 0) 
				{
				blueMinDataValue = histogramSummaryPtr[channelsPtr[2] - 1].minValue;
				blueMaxDataValue = histogramSummaryPtr[channelsPtr[2] - 1].maxValue;
				loadBlueChannelLimitsFlag = TRUE;

				}	// end "if (channelsPtr[2] >= 0)"

			}	// end "else if (localMinMaxCode == kEntireDataRange)" 

		else if (localMinMaxCode == kUserSpecified && updateUserSpecifiedFlag) 
			{
					//	Enhancement minimum and maximum for red channel. 					

			if (channelsPtr[0] >= 0) 
				{
				redMinDataValue = minMaxValuesPtr[0];
				redMaxDataValue = minMaxValuesPtr[1];
				loadRedChannelLimitsFlag = TRUE;

				}	// end "if (channelsPtr[0] >= 0)"

					//	Enhancement minimum and maximum for green channel. 				

			if (channelsPtr[1] >= 0) 
				{
				greenMinDataValue = minMaxValuesPtr[2];
				greenMaxDataValue = minMaxValuesPtr[3];
				loadGreenChannelLimitsFlag = TRUE;

				}	// end "if (channelsPtr[1] >= 0)"

					//	Enhancement minimum and maximum for blue channel. 					

			if (channelsPtr[1] >= 0) 
				{
				blueMinDataValue = minMaxValuesPtr[4];
				blueMaxDataValue = minMaxValuesPtr[5];
				loadBlueChannelLimitsFlag = TRUE;

				}	// end "if (channelsPtr[2] >= 0)"

			}	// end "else if (localMinMaxCode == kUserSpecified && ...)" 

		else if (localMinMaxCode == kThematicDefault) 
			{
					//	Enhancement minimum and maximum for red channel. 

			UpdateThematicTypeMinMaxes (numberLevels,
													channelsPtr[0],
													&redMinDataValue,
													&redMaxDataValue);

			loadRedChannelLimitsFlag = TRUE;

			}	// end "else if (localMinMaxCode == kThematicDefault)" 

				// Min and max data values for Red Channel

		if (loadRedChannelLimitsFlag) 
			{
			redNumberEDecimalDigits = 
								histogramSummaryPtr[channelsPtr[0] - 1].numberEDecimalDigits;
			redNumberFDecimalDigits = 
								histogramSummaryPtr[channelsPtr[0] - 1].numberFDecimalDigits;

			}	// end "if (loadRedChannelLimitsFlag)"

				// Min and max data values for Green Channel

		if (loadGreenChannelLimitsFlag) 
			{
			greenNumberEDecimalDigits = 
								histogramSummaryPtr[channelsPtr[1] - 1].numberEDecimalDigits;
			greenNumberFDecimalDigits = 
								histogramSummaryPtr[channelsPtr[1] - 1].numberFDecimalDigits;

			}	// end "if (loadGreenChannelLimitsFlag)"

				// Min and max data values for Blue Channel

		if (loadBlueChannelLimitsFlag) 
			{
			blueNumberEDecimalDigits = 
								histogramSummaryPtr[channelsPtr[2] - 1].numberEDecimalDigits;
			blueNumberFDecimalDigits = 
								histogramSummaryPtr[channelsPtr[2] - 1].numberFDecimalDigits;

			}	// end "if (loadBlueChannelLimitsFlag)"

		}	// end "if (histogramSummaryPtr != NULL)"

	else	// histogramSummaryPtr == NULL 
		{
		if (localMinMaxCode == kEntireDataRange) 
			{
			redMinDataValue = gImageFileInfoPtr->minUsableDataValue;
			redMaxDataValue = gImageFileInfoPtr->maxUsableDataValue;
			loadRedChannelLimitsFlag = TRUE;

			greenMinDataValue = gImageFileInfoPtr->minUsableDataValue;
			greenMaxDataValue = gImageFileInfoPtr->maxUsableDataValue;
			loadGreenChannelLimitsFlag = TRUE;

			blueMinDataValue = gImageFileInfoPtr->minUsableDataValue;
			blueMaxDataValue = gImageFileInfoPtr->maxUsableDataValue;
			loadBlueChannelLimitsFlag = TRUE;

			}	//  end "if (localMinMaxCode == kEntireDataRange)"

		else if (localMinMaxCode == kUserSpecified && updateUserSpecifiedFlag) 
			{
					//	Enhancement minimum and maximum for red channel. 					

			redMinDataValue = minMaxValuesPtr[0] - gImageFileInfoPtr->signedValueOffset;
			redMaxDataValue = minMaxValuesPtr[1] - gImageFileInfoPtr->signedValueOffset;
			loadRedChannelLimitsFlag = TRUE;

					//	Enhancement minimum and maximum for green channel. 				

			greenMinDataValue = minMaxValuesPtr[2] - gImageFileInfoPtr->signedValueOffset;
			greenMaxDataValue = minMaxValuesPtr[3] - gImageFileInfoPtr->signedValueOffset;
			loadGreenChannelLimitsFlag = TRUE;

					//	Enhancement minimum and maximum for blue channel. 					

			blueMinDataValue = minMaxValuesPtr[4] - gImageFileInfoPtr->signedValueOffset;
			blueMaxDataValue = minMaxValuesPtr[5] - gImageFileInfoPtr->signedValueOffset;
			loadBlueChannelLimitsFlag = TRUE;

			}	// end "else if (localMinMaxCode == kUserSpecified && ...)" 

		redNumberEDecimalDigits = gImageFileInfoPtr->maxNumberEDecimalDigits;
		redNumberFDecimalDigits = gImageFileInfoPtr->maxNumberFDecimalDigits;
		greenNumberEDecimalDigits = gImageFileInfoPtr->maxNumberEDecimalDigits;
		greenNumberFDecimalDigits = gImageFileInfoPtr->maxNumberFDecimalDigits;
		blueNumberEDecimalDigits = gImageFileInfoPtr->maxNumberEDecimalDigits;
		blueNumberFDecimalDigits = gImageFileInfoPtr->maxNumberFDecimalDigits;

		}	// end "else histogramSummaryPtr == NUL"

	if (loadRedChannelLimitsFlag) 
		{
				// Min and max data values for Red Channel

		LoadDItemRealValue (dialogPtr,
									IDC_RedChannelMinE,
									redMinDataValue,
									redNumberFDecimalDigits,
									redNumberEDecimalDigits);

		LoadDItemRealValue (dialogPtr,
									IDC_RedChannelMaxE,
									redMaxDataValue,
									redNumberFDecimalDigits,
									redNumberEDecimalDigits);

		}	// end "if (loadRedChannelLimitsFlag)"

	if (loadGreenChannelLimitsFlag) 
		{
				// Min and max data values for Green Channel

		LoadDItemRealValue (dialogPtr,
									IDC_GreenChannelMinE,
									greenMinDataValue,
									greenNumberFDecimalDigits,
									greenNumberEDecimalDigits);

		LoadDItemRealValue (dialogPtr,
									IDC_GreenChannelMaxE,
									greenMaxDataValue,
									greenNumberFDecimalDigits,
									greenNumberEDecimalDigits);

		}	// end "if (loadGreenChannelLimitsFlag)"

	if (loadBlueChannelLimitsFlag) 
		{
				// Min and max data values for Blue Channel

		LoadDItemRealValue (dialogPtr,
									IDC_BlueChannelMinE,
									blueMinDataValue,
									blueNumberFDecimalDigits,
									blueNumberEDecimalDigits);

		LoadDItemRealValue (dialogPtr,
									IDC_BlueChannelMaxE,
									blueMaxDataValue,
									blueNumberFDecimalDigits,
									blueNumberEDecimalDigits);

		}	// end "if (loadBlueChannelLimitsFlag)"

}	// end "UpdateEnhancementMinMaxes"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void UpdateMinMaxValueIndices
//
//	Software purpose:	The purpose of this routine is to update the items
//							in the popup display type menu.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:						
//
//	Global Data:
//
//	Coded By:			Larry L. Biehl			Date: 10/21/2006
//	Revised By:			Larry L. Biehl			Date: 10/22/2006	

void UpdateMinMaxValueIndices (
				DisplaySpecsPtr					displaySpecsPtr,
				HistogramSummaryPtr				histogramSummaryPtr,
				SInt16								numberChannels,
				UInt16*								channelsPtr,
				SInt32								signedValueOffset)

{
	UInt32								index;


	if (displaySpecsPtr != NULL && histogramSummaryPtr != NULL) 
		{
		if (numberChannels >= 1) 
			{
			index = displaySpecsPtr->minMaxPointers[0];

			displaySpecsPtr->minMaxValuesIndex[index][0] = GetBinIndexForDataValue (
							  displaySpecsPtr->minMaxValues[index][0] + signedValueOffset,
							  &histogramSummaryPtr[channelsPtr[0]]);

			displaySpecsPtr->minMaxValuesIndex[index][1] = GetBinIndexForDataValue (
							  displaySpecsPtr->minMaxValues[index][1] + signedValueOffset,
							  &histogramSummaryPtr[channelsPtr[0]]);

			}	// end "if (numberChannels >= 1)"

		if (numberChannels >= 2) 
			{
			index = displaySpecsPtr->minMaxPointers[1];

			displaySpecsPtr->minMaxValuesIndex[index][0] = GetBinIndexForDataValue (
							  displaySpecsPtr->minMaxValues[index][0] + signedValueOffset,
							  &histogramSummaryPtr[channelsPtr[1]]);

			displaySpecsPtr->minMaxValuesIndex[index][1] = GetBinIndexForDataValue (
							  displaySpecsPtr->minMaxValues[index][1] + signedValueOffset,
							  &histogramSummaryPtr[channelsPtr[1]]);

			}	// end "if (numberChannels >= 2)"

		if (numberChannels == 3) 
			{
			index = displaySpecsPtr->minMaxPointers[2];

			displaySpecsPtr->minMaxValuesIndex[index][0] = GetBinIndexForDataValue (
							  displaySpecsPtr->minMaxValues[index][0] + signedValueOffset,
							  &histogramSummaryPtr[channelsPtr[2]]);

			displaySpecsPtr->minMaxValuesIndex[index][1] = GetBinIndexForDataValue (
							  displaySpecsPtr->minMaxValues[index][1] + signedValueOffset,
							  &histogramSummaryPtr[channelsPtr[2]]);

			}	// end "if (numberChannels == 3)"

		}	// end "if (displaySpecsPtr != NULL && histogramSummaryPtr != NULL)"

}	// end "UpdateMinMaxValueIndices"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void UpdatePopUpDisplayTypeMenu
//
//	Software purpose:	The purpose of this routine is to update the items
//							in the popup display type menu.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:						
//
//	Global Data:
//
//	Coded By:			Larry L. Biehl			Date: 09/24/1993
//	Revised By:			Larry L. Biehl			Date: 12/29/2006	

void UpdatePopUpDisplayTypeMenu (
				UInt16								totalNumberChannels,
				SInt16								maxSystemPixelSize)

{
	#if defined multispec_mac
		EnableMenuItem (gPopUpDisplayTypeMenu, 1);

		//if (totalNumberChannels > 1)
		//	EnableMenuItem (gPopUpDisplayTypeMenu, 6);

		//else	// totalNumberChannels <= 1 
		//	DisableMenuItem (gPopUpDisplayTypeMenu, 6);

		if (maxSystemPixelSize <= 2) 
			{
			DisableMenuItem (gPopUpDisplayTypeMenu, 2);
			DisableMenuItem (gPopUpDisplayTypeMenu, 3);
			DisableMenuItem (gPopUpDisplayTypeMenu, 4);
			DisableMenuItem (gPopUpDisplayTypeMenu, 5);
			DisableMenuItem (gPopUpDisplayTypeMenu, 6);

			}	// end "if (maxSystemPixelSize <= 2)" 

		else	// maxSystemPixelSize > 2 
			{
			EnableMenuItem (gPopUpDisplayTypeMenu, 2);
			EnableMenuItem (gPopUpDisplayTypeMenu, 3);
			EnableMenuItem (gPopUpDisplayTypeMenu, 4);
			EnableMenuItem (gPopUpDisplayTypeMenu, 5);
			EnableMenuItem (gPopUpDisplayTypeMenu, 6);

			}	// end "else maxSystemPixelSize > 2" 
	#endif	// defined multispec_mac	

}	// end "UpdatePopUpDisplayTypeMenu" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 Update8_16_24BitsOfColorIndex
//
//	Software purpose:	The purpose of this routine is to update the bits of color index
//							for the multispec image display.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 10/02/2006
//	Revised By:			Larry L. Biehl			Date: 02/27/2018

SInt16 Update8_16_24BitsOfColorIndex (
				SInt16 								menuBitsOfColorIndex)

{
    // Get index into 8, 16, 24 bits of color vector.

    SInt16 bitsOfColorIndex = menuBitsOfColorIndex;
    if (bitsOfColorIndex == 1)
        bitsOfColorIndex = 2;

    return (bitsOfColorIndex);

}	// end "Update8_16_24BitsOfColorIndex"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void UpdateThematicTypeMinMaxes
//
//	Software purpose:	The purpose of this routine is to update the enhancement 
//							minimum and maximum values for thematic type display of 
//							multispectral images.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 10/02/2006
//	Revised By:			Larry L. Biehl			Date: 02/27/2018

void UpdateThematicTypeMinMaxes (
				SInt16								numberClassBins,
				SInt16								channelNumber,
				double*								minValuePtr,
				double*								maxValuePtr)
				
{
	double								classBinWidth,
											difference,
											logMaxValue,
											maxValue,
											minValue,
											newMaxValue;

   HistogramSpecsPtr					histogramSpecsPtr = NULL;
   HistogramSummaryPtr				histogramSummaryPtr;

   Handle								histogramSpecsHandle;

   UInt32								channelIndex;


	 histogramSpecsHandle = GetHistogramSpecsHandle (gImageWindowInfoPtr);

	 if (histogramSpecsHandle != NULL) 
		{
				// The handle is already locked.

		histogramSpecsPtr = (HistogramSpecsPtr)GetHandlePointer (histogramSpecsHandle);

		if (!histogramSpecsPtr->loadedFlag)
			histogramSpecsPtr = NULL;

		}	// end "if (histogramSpecsHandle != NULL)" 

	if (histogramSpecsPtr != NULL) 
		{
		if (histogramSpecsPtr->histogramSummaryPtr != NULL) 
			{
			channelIndex = channelNumber - 1;

			histogramSummaryPtr =
						  &histogramSpecsPtr->histogramSummaryPtr[channelIndex];

			minValue = 0;
			if (histogramSummaryPtr->minNonSatValue < 0)
				minValue = histogramSummaryPtr->minNonSatValue;

					// Now determine if minValue should actually be the minNotSatValue
					// to provide a better display range of class differences.

			if (numberClassBins <= 2) 
				{
				minValue = (histogramSummaryPtr->minNonSatValue +
											histogramSummaryPtr->maxNonSatValue) / 2;
				maxValue = minValue;
				classBinWidth = 0;

				}	// end "if (numberClassBins <= 2)"

			else	// numberClassBins > 2
				{
				classBinWidth = (histogramSummaryPtr->maxNonSatValue - minValue) / 
																					(numberClassBins - 2);

				if (histogramSummaryPtr->minNonSatValue > minValue + 2 * classBinWidth)
					minValue = histogramSummaryPtr->minNonSatValue;

						// Check if power number like 1, 10, 100, etc will be within 15%
						// of the maximum value.

				maxValue = histogramSummaryPtr->maxNonSatValue;
				logMaxValue = (SInt32)(log10 (maxValue) + 1);
				newMaxValue = pow ((double)10, logMaxValue);
				difference = fabs (maxValue - newMaxValue);
				if (difference / maxValue < .15)
					maxValue = newMaxValue;

				else	// ratio is > .15
					{
							// Now try half of the power number ... .5, 5, 50, etc.

					newMaxValue = newMaxValue / 2;
					difference = fabs (maxValue - newMaxValue);
					if (difference / maxValue < .15)
						maxValue = newMaxValue;

					}	// else "else ratio is > .15"

				classBinWidth = (maxValue - minValue) / (numberClassBins - 2);

				if (histogramSummaryPtr->numberFDecimalDigits == 0)
					classBinWidth = ceil (classBinWidth);

				}	// end "else numberClassBins > 2"

			maxValue = minValue + (numberClassBins - 2) * classBinWidth;

			*minValuePtr = minValue;
			*maxValuePtr = MIN (maxValue, histogramSummaryPtr->maxValue);

			}	// "if (histogramSpecsPtr->histogramSummaryPtr != NULL)"

		}	// end "if (histogramSpecsPtr != NULL)"

}	// end "UpdateThematicTypeMinMaxes"
