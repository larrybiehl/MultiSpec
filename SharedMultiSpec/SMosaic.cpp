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
//	File:						Mosaic.c
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/25/2019
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	This file of routines allows the user to combine two image
//								side by side or top to bottom also called mosaicing them. One
//								can indicate that background pixels should be ignored. This
//								capability was orginally created to combine Landsat images
//								which were in quarter quad files.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#if defined multispec_wx  
	#include "xImageView.h" 
	#include	"xMosaicTwoImagesDialog.h" 
	#include "xMultiSpec.h"
#endif	// defined multispec_wx  

#if defined multispec_mac || defined multispec_mac_swift
	#define	IDC_MosaicDirection					4
	#define	IDC_LeftTopPrompt						5
	#define	IDC_LeftTopImage						6
	#define	IDC_RightPrompt						7
	#define	IDC_RightImageFileList				8
	#define	IDC_BottomPrompt						9
	#define	IDC_BottomImageFileList				10
	#define	IDC_LineStart							15
	#define	IDC_LineEnd								16
	#define	IDC_LineInterval						17
	#define	IDC_ColumnStart						18
	#define	IDC_ColumnEnd							19
	#define	IDC_ColumnInterval					20
	#define	IDC_EntireSelectButton2				21
	#define	IDC_StartEndTitle2					22
	#define	IDC_LinePrompt2						23
	#define	IDC_ColumnPrompt2						24
	#define	IDC_LineStart2							25
	#define	IDC_LineEnd2							26
	#define	IDC_ColumnStart2						27
	#define	IDC_ColumnEnd2							28
	#define	IDC_EntireSelectButton3				29
	#define	IDC_StartEndTitle3					30
	#define	IDC_LinePrompt3						31
	#define	IDC_ColumnPrompt3						32
	#define	IDC_LineStart3							33
	#define	IDC_LineEnd3							34
	#define	IDC_ColumnStart3						35
	#define	IDC_ColumnEnd3							36
	#define	IDC_IgnoreBackgroundValue			38
	#define	IDC_BackgroundValue					39
	#define	IDC_WriteChannelDescriptions		40
	#define	IDC_HeaderFormatList					42

	#define	IDS_Alert149							149
	#define	IDS_Alert150							150
#endif	// defined multispec_mac || defined multispec_mac_swift 
  
#if defined multispec_win   
	#include	"WMosaicTwoImagesDialog.h" 
#endif	// defined multispec_win      



			// Prototypes for routines in this file that are only called by
			// other routines in this file.
			
PascalVoid DrawMosaicDirectionPopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber);

UInt32 FindLeftBytesToMove (
				char*									ioLBuffer2Ptr,
				UInt32								numberBytes,
				char*									backgroundValuePtr,
				UInt32								numberLeftColumns,
				UInt32								columnInterval,
				UInt32								numberChannels,
				UInt32								leftSkip);
							
UInt32 FindRightBytesToMove (
				char*									ioRBuffer2Ptr,
				UInt32								numberBytes,
				char*									backgroundValuePtr,
				UInt32								numberRightColumns,
				UInt32								columnInterval,
				UInt32								numberChannels,
				UInt32								rightSkip);
			
UInt32 GetBackgroundValueForDataTypeCode (
				FileInfoPtr							outFileInfoPtr,
				double								backgroundValue,
				double*								backgroundValueStringPtr);
							
Boolean MosaicTwoImages (
				FileIOInstructionsPtr			fileIOInstructionsLeftPtr,
				FileIOInstructionsPtr			fileIOInstructionsRightPtr,
				FileInfoPtr							outFileInfoPtr,
				ReformatOptionsPtr				reformatOptionsPtr);

Boolean MosaicTwoImagesLeftRight (
				FileIOInstructionsPtr			fileIOInstructionsLeftPtr,
				FileIOInstructionsPtr			fileIOInstructionsRightPtr,
				FileInfoPtr							outFileInfoPtr,
				ReformatOptionsPtr				reformatOptionsPtr,
				UInt32*								numberColumnsReducedPtr);

Boolean MosaicTwoImagesDialog (
				FileInfoPtr 						fileInfoPtr,
				FileInfoPtr 						outFileInfoPtr,
				ReformatOptionsPtr 				reformatOptionsPtr);

Boolean MosaicTwoImagesTopBottom (
				FileIOInstructionsPtr			fileIOInstructionsTopPtr,
				FileIOInstructionsPtr			fileIOInstructionsBottomPtr,
				FileInfoPtr							outFileInfoPtr,
				ReformatOptionsPtr				reformatOptionsPtr);


		// Globals for the file.
		
SInt16				gMosaicDirectionCode;



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawMosaicDirectionPopUp
//
//	Software purpose:	The purpose of this routine is to draw the disk file format options
//							prompt and selection for the disk file format options 
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
//	Coded By:			Larry L. Biehl			Date: 03/15/2014
//	Revised By:			Larry L. Biehl			Date: 03/15/2014	

pascal void DrawMosaicDirectionPopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.									
			
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpMosaicDirectionMenu, 
								gMosaicDirectionCode, 
								TRUE);
	
}	// end "DrawMosaicDirectionPopUp" 
#endif	// defined multispec_mac



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void FindLeftBytesToMove
//
//	Software purpose:	This routine finds the number of bytes to be moved in the left
//							line to be mosaiced.  It does this by checking for background
//							values starting from the end of the line and going towards the
//							first of the line.  The routine stops searching for the end of
//							the data line when it does not find a backgound value in at least
//							one of the channels for a pixel.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/24/2006
//	Revised By:			Larry L. Biehl			Date: 11/25/2019

UInt32 FindLeftBytesToMove (
				char*									ioLBuffer2Ptr,
				UInt32								numberBytes,
				char*									backgroundValuePtr,
				UInt32								numberLeftColumns,
				UInt32								columnInterval,
				UInt32								numberChannels,
				UInt32								leftSkip)

{
	UInt32								channelCount,
											column,
											j,
											leftBytesToMove;
											
	Boolean								backgroundFlag;
		
	
			// Find last valid data value at end of left part of line.	
	
	backgroundFlag = TRUE;
	for (column=numberLeftColumns-1; column<numberLeftColumns; column--)
		{
		j = column * columnInterval * numberBytes;
		channelCount = 0;
		
		while (channelCount<numberChannels && backgroundFlag)
			{
			if (memcmp (&ioLBuffer2Ptr[j], backgroundValuePtr, numberBytes))
				backgroundFlag = FALSE;
				
			j += leftSkip;
			channelCount++;
				
			}	// end "while (channelCount<numberChannels && ..." 
			
		if (!backgroundFlag)
			break;
				
		}	// end "for (column=numberleftColumns; ..." 
		
	leftBytesToMove = (column+1) * numberBytes * columnInterval;
			
	return (leftBytesToMove);
		
}	// end "FindLeftBytesToMove" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void FindRightBytesToMove
//
//	Software purpose:	This routine finds the number of bytes to be moved in the right
//							line to be mosaiced.  It does this by checking for background
//							values starting from the beginning of the line and going towards
//							the end of the line.  The routine stops searching for the start
//							of the data line when it does not find a backgound value in at
//							least one of the channels for a pixel.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/24/2006
//	Revised By:			Larry L. Biehl			Date: 03/24/2006

UInt32 FindRightBytesToMove (
				char*									ioRBuffer2Ptr,
				UInt32								numberBytes,
				char* 								backgroundValuePtr,
				UInt32								numberRightColumns,
				UInt32								columnInterval,
				UInt32								numberChannels,
				UInt32								rightSkip)

{
	UInt32								channelCount,
											column,
											j,
											rightBytesToMove;
											
	Boolean								backgroundFlag;
		
	
			// Find last valid data value at end of left part of line.	
	
	backgroundFlag = TRUE;
	j = 0;
	for (column=0; column<numberRightColumns; column++)
		{
		j = column * columnInterval * numberBytes;
		channelCount = 0;
		
		while (channelCount<numberChannels && backgroundFlag)
			{
			if (memcmp (&ioRBuffer2Ptr[j], backgroundValuePtr, numberBytes))
				backgroundFlag = FALSE;
				
			j += rightSkip;
			channelCount++;
				
			}	// end "while (channelCount<numberChannels && ..." 
			
		if (!backgroundFlag)
			break;
				
		}	// end "for (column=0; ..." 
		
	rightBytesToMove = (numberRightColumns - column) * numberBytes * columnInterval;
			
	return (rightBytesToMove);				
		
}	// end "FindLeftBytesToMove" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetBackgroundValueForDataTypeCode
//
//	Software purpose:	This routine initializes the input line to the requested
//							background value.  It takes into account the type of data value.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/24/2006
//	Revised By:			Larry L. Biehl			Date: 02/27/2018

UInt32 GetBackgroundValueForDataTypeCode (
				FileInfoPtr							outFileInfoPtr, 
				double								backgroundValue,
				double*								backgroundValueStringPtr)

{
	UInt32								backgroundValueByteLength = 0;
	
	float									backgroundFloatValue;
	SInt32								backgroundSInt32Value;
	UInt32								backgroundUInt32Value;
	SInt16								backgroundSInt16Value;
	UInt16								backgroundUInt16Value;
	SInt8									backgroundSInt8Value;
	UInt8									backgroundUInt8Value;
	
			
			// Initialize a background line that will be copied from as			
			// needed.																			
	
	if (outFileInfoPtr->numberBytes == 1)
		{
		if (outFileInfoPtr->signedDataFlag)
			{
			backgroundSInt8Value = (SInt8)backgroundValue;
			memcpy (backgroundValueStringPtr, &backgroundSInt8Value, 1);
			
			}	// end "if (outFileInfoPtr->signedDataFlag)"
			
		else	// !outFileInfoPtr->signedDataFlag
			{
			backgroundUInt8Value = (UInt8)backgroundValue;
			memcpy (backgroundValueStringPtr, &backgroundUInt8Value, 1);
			
			}	// end "else !outFileInfoPtr->signedDataFlag"
			
		backgroundValueByteLength = 1;
			
		}	// end "if (outFileInfoPtr->numberBytes == 1)"
		
	else if (outFileInfoPtr->numberBytes == 2)
		{
		if (outFileInfoPtr->signedDataFlag)
			{
			backgroundSInt16Value = (SInt16)backgroundValue;
			memcpy (backgroundValueStringPtr, &backgroundSInt16Value, 2);
			
			}	// end "if (outFileInfoPtr->signedDataFlag)"
			
		else	// !outFileInfoPtr->signedDataFlag
			{
			backgroundUInt16Value = (UInt16)backgroundValue;
			memcpy (backgroundValueStringPtr, &backgroundUInt16Value, 2);
			
			}	// end "else !outFileInfoPtr->signedDataFlag"
			
		backgroundValueByteLength = 2;
			
		}	// end "else outFileInfoPtr->numberSamples == 2"
		
	else if (outFileInfoPtr->numberBytes == 4)
		{
		if (outFileInfoPtr->dataTypeCode == kIntegerType)
			{
			if (outFileInfoPtr->signedDataFlag)
				{
				backgroundSInt32Value = (SInt32)backgroundValue;
				memcpy (backgroundValueStringPtr, &backgroundSInt32Value, 4);
				
				}	// end "if (outFileInfoPtr->signedDataFlag)"
				
			else	// !outFileInfoPtr->signedDataFlag
				{
				backgroundUInt32Value = (UInt32)backgroundValue;
				memcpy (backgroundValueStringPtr, &backgroundUInt32Value, 4);
			
				}	// end "else !outFileInfoPtr->signedDataFlag"
				
			}	// end "if (outFileInfoPtr->dataTypeCode == kIntegerType)"
			
		else	// outFileInfoPtr->dataTypeCode == kRealType
			{
			backgroundFloatValue = (float)backgroundValue;
			memcpy (backgroundValueStringPtr, &backgroundFloatValue, 4);
			
			}	// end "else outFileInfoPtr->dataTypeCode == kRealType"
			
		backgroundValueByteLength = 4;
			
		}	// end "else outFileInfoPtr->numberBytes == 4"
		
	else if (outFileInfoPtr->numberBytes == 8)
		{
		*backgroundValueStringPtr = backgroundValue;
			
		backgroundValueByteLength = 8;
			
		}	// end "else outFileInfoPtr->numberBytes == 8"
		
	return (backgroundValueByteLength);

}	// end "GetBackgroundValueForDataTypeCode" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean MosaicTwoImages
//
//	Software purpose:	This routine joins the two input images side by
//							side.  If requested the specified background values
//							at the end of the first part of the line and at the
//							beginning of the second part of the line will be
//							ignored.  If background values are left out the
//							header, if present, will be updated to reflect
//							the new number of columns in each line.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/15/1992
//	Revised By:			Larry L. Biehl			Date: 12/09/2014

Boolean MosaicTwoImages (
				FileIOInstructionsPtr			fileIOInstructionsLeftPtr, 
				FileIOInstructionsPtr			fileIOInstructionsRightPtr, 
				FileInfoPtr							outFileInfoPtr, 
				ReformatOptionsPtr				reformatOptionsPtr)

{
			// Declare local variables & structures										
	
	FileInfoPtr							leftFileInfoPtr;
	
	HUCharPtr							ioOutBufferPtr;

	UInt32								leftColumnEnd,
											leftColumnStart,
											numberColumnsReduced,
											numberOutputChannels,
											bottomOrRightColumnEnd,
											bottomOrRightColumnStart,
											supportFileType;
	
	SInt16								errCode;
	
	Boolean								continueFlag;
												
		
			// Initialize local variables.													
			
	errCode = noErr;  
	leftFileInfoPtr = fileIOInstructionsLeftPtr->fileInfoPtr;
					
	ioOutBufferPtr = reformatOptionsPtr->ioOutBufferPtr;
					
	leftColumnStart = reformatOptionsPtr->columnStart;
	leftColumnEnd =  reformatOptionsPtr->columnEnd;
	
	bottomOrRightColumnStart = reformatOptionsPtr->startColumn;
	bottomOrRightColumnEnd =  reformatOptionsPtr->stopColumn;
	
	numberOutputChannels = reformatOptionsPtr->numberChannels;
	numberColumnsReduced = 0;

			// Write a header if requested.													
			
	continueFlag = WriteNewImageHeader (
							leftFileInfoPtr,
							outFileInfoPtr, 
							(char*)ioOutBufferPtr,
							NULL,
							kFromReformat,						
							reformatOptionsPtr->outputFileCode, 
							1,
							1,
							kClassDisplay,
							kNoPaletteColorsDefined);
			
	if (continueFlag)
		{
				// Intialize the nextTime variable to indicate when the next 		
				// check should occur for a command-.										
				
		gNextTime = TickCount ();
		
				// Load some of the File IO Instructions structure that pertain
				// to the specific area being used for the left or top image.
				
		errCode = SetUpFileIOInstructions (fileIOInstructionsLeftPtr,
														NULL, 
														reformatOptionsPtr->lineStart,
														reformatOptionsPtr->lineEnd,
														1,
														leftColumnStart,
														leftColumnEnd,
														1,
														(UInt16)numberOutputChannels,
														(UInt16*)reformatOptionsPtr->channelPtr,
														kDetermineSpecialBILFlag);
			
				// Load some of the File IO Instructions structure that pertain
				// to the specific area being used for the right or bottom image.
		
		if (errCode == noErr)		
			errCode = SetUpFileIOInstructions (fileIOInstructionsRightPtr,
														NULL, 
														reformatOptionsPtr->startLine,
														reformatOptionsPtr->stopLine,
														1,
														bottomOrRightColumnStart,
														bottomOrRightColumnEnd,
														1,
														(UInt16)numberOutputChannels,
														(UInt16*)reformatOptionsPtr->channelPtr,
														kDetermineSpecialBILFlag);
														
		continueFlag = (errCode == noErr);
														
		if (continueFlag)
			{
			if (reformatOptionsPtr->mosaicDirectionCode == kMosaicLeftRight)
				continueFlag = MosaicTwoImagesLeftRight (fileIOInstructionsLeftPtr,
																		fileIOInstructionsRightPtr,
																		outFileInfoPtr,
																		reformatOptionsPtr,
																		&numberColumnsReduced);
														
			else	// reformatOptionsPtr->mosaicDirectionCode == kMosaicTopBottom
				continueFlag = MosaicTwoImagesTopBottom (fileIOInstructionsLeftPtr,
																		fileIOInstructionsRightPtr,
																		outFileInfoPtr,
																		reformatOptionsPtr);
																		
			}	// end "if (continueFlag)"
		
				// Close up any File IO Instructions structure that pertain to the 
				// left image.
				
		CloseUpFileIOInstructions (fileIOInstructionsLeftPtr, NULL);
		
				// Close up any File IO Instructions structure that pertain to the 
				// right image.
				
		CloseUpFileIOInstructions (fileIOInstructionsRightPtr, NULL);
			
				// Update the header if needed.												
				
		if (continueFlag && numberColumnsReduced != 0)
			continueFlag = WriteNewImageHeader (
											leftFileInfoPtr,
											outFileInfoPtr, 
											(char*)ioOutBufferPtr,
											NULL,
											kFromReformat,
											reformatOptionsPtr->outputFileCode, 
											1,
											1,
											kClassDisplay,
											kNoPaletteColorsDefined);
		
				// Write channel discriptions at the end of the file if needed.	
		
		if (continueFlag && reformatOptionsPtr->channelDescriptions)	
			WriteChannelDescriptionsAndValues (outFileInfoPtr,
															reformatOptionsPtr->channelPtr,
															reformatOptionsPtr->numberChannels,
															FALSE);

		if (continueFlag && outFileInfoPtr->thematicType)
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
											
			}	// end "if (continueFlag && outFileInfoPtr->thematicType)" 
			
		}	// end "if (continueFlag)" 
		
	return (continueFlag);
			
}	// end "MosaicTwoImages" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean MosaicTwoImagesLeftRight
//
//	Software purpose:	This routine joins the two input images side by
//							side.  If requested the specified background values
//							at the end of the first part of the line and at the
//							beginning of the second part of the line will be
//							ignored.  If background values are left out the
//							header, if present, will be updated to reflect
//							the new number of columns in each line.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/15/1992
//	Revised By:			Larry L. Biehl			Date: 03/15/2014

Boolean MosaicTwoImagesLeftRight (
				FileIOInstructionsPtr			fileIOInstructionsLeftPtr, 
				FileIOInstructionsPtr			fileIOInstructionsRightPtr,
				FileInfoPtr							outFileInfoPtr, 
				ReformatOptionsPtr				reformatOptionsPtr,
				UInt32*								numberColumnsReducedPtr)

{
			// Declare local variables & structures										
	
	CMFileStream*						outFileStreamPtr;
												
	FileInfoPtr							leftFileInfoPtr,
											rightFileInfoPtr;
	
	HUCharPtr							ioLBuffer1Ptr,
											ioLBuffer2Ptr,
											ioOutBufferPtr,
											ioRBuffer1Ptr,
											ioRBuffer2Ptr;
	
	unsigned char 						*ioOutLBufferPtr,
											*savedOutBufferPtr;
											
	double								backgroundString,
											backgroundValue;
	
	SInt32								channelCount,
											countOutBytes,
											lastOutputWrittenLine,
											lastPercentComplete,
											limitIoOutBytes,
											numberOutputChannels,
											percentComplete,
											preLineBytes,
											totalIOOutBytes;
	
	UInt32								backgroundByteLength,
											columnBytes,
											columnInterval,
											inputBufferIndex,
											leftBytesToMove,
											leftColumnEnd,
											leftColumnStart,
											leftSkip,
											line,
											line1,
											line2,
											numberColumnsReduced,
											numberLeftBytesPerChannel,
											numberLeftColumns,
											numberOutputLines,
											outNumberBytesPerLineAndChannel,
											numberRightBytesPerChannel,
											numberRightColumns,
											rightBytesToMove,
											rightColumnEnd,
											rightColumnStart,
											rightSkip,
											outChannelByteIncrement,
											outLineByteIncrement;
	
	SInt16								errCode,
											GAIAPrelineString[7] = 
													{	0x0300, 0x0600, 0x0000, 0x0100, 
														0x0000, 0x0000, 0x0000};
	
	UInt16								numberOutputBytes;
	
	Boolean								continueFlag,
											forceBISFlag,
											ignoreBackgroundFlag;
												
		
			// Initialize local variables.													
			
	errCode = noErr;  
	continueFlag = TRUE;
	lastOutputWrittenLine = 0;
	numberColumnsReduced = 0;
	forceBISFlag = (outFileInfoPtr->bandInterleave == kBIS);
	preLineBytes = outFileInfoPtr->numberPreLineBytes;
	leftFileInfoPtr = fileIOInstructionsLeftPtr->fileInfoPtr;
	rightFileInfoPtr = fileIOInstructionsRightPtr->fileInfoPtr;
					
	ioOutBufferPtr = reformatOptionsPtr->ioOutBufferPtr;
				
	lastPercentComplete = -1;
	
	ignoreBackgroundFlag =  reformatOptionsPtr->ignoreBackgroundFlag;
	backgroundValue = reformatOptionsPtr->backgroundValue;
	
	leftColumnStart = reformatOptionsPtr->columnStart;
	leftColumnEnd =  reformatOptionsPtr->columnEnd;
	numberLeftColumns = leftColumnEnd - leftColumnStart + 1;
	numberLeftBytesPerChannel = numberLeftColumns * outFileInfoPtr->numberBytes;
	
	rightColumnStart = reformatOptionsPtr->startColumn;
	rightColumnEnd =  reformatOptionsPtr->stopColumn;
	numberRightColumns = rightColumnEnd - rightColumnStart + 1;
	numberRightBytesPerChannel = numberRightColumns * outFileInfoPtr->numberBytes;
	
	numberOutputLines = 
					reformatOptionsPtr->lineEnd - reformatOptionsPtr->lineStart + 1;
															
	numberOutputChannels = reformatOptionsPtr->numberChannels;
	numberOutputBytes = outFileInfoPtr->numberBytes;
		
	outFileStreamPtr = GetFileStreamPointer (outFileInfoPtr);
	
	if (forceBISFlag)
		{
		columnInterval = numberOutputChannels;
		leftSkip = numberOutputBytes;
		rightSkip = numberOutputBytes;
		
		}	// end "if (forceBISFlag)" 
	
	else	// !forceBISFlag 
		{
		columnInterval = 1;
		leftSkip = numberLeftColumns * numberOutputBytes;
		rightSkip = numberRightColumns * numberOutputBytes;
		
		}	// end "else !forceBISFlag" 

			// Set up output working buffer pointer.  This pointer will			
			// change as the output buffer fills up									
	
	ioOutLBufferPtr = (unsigned char *)ioOutBufferPtr;
	savedOutBufferPtr = ioOutLBufferPtr;
	
			// Set up limit of output buffer to know when to write buffer to	
			// the disk file. 																
			
	countOutBytes = reformatOptionsPtr->countOutBytes;
	limitIoOutBytes = reformatOptionsPtr->ioOutBufferBytes - countOutBytes;
	totalIOOutBytes = 0;
	
			// Get count of bytes that are loaded into the output buffer each	
			// time through the channel loop.										
			
	GetOutputBufferParameters (outFileInfoPtr, 
										reformatOptionsPtr,
										&outChannelByteIncrement,
										&outLineByteIncrement,
										&outNumberBytesPerLineAndChannel);

			// Initialize the buffers to load the left part of image data 		
			// into.																				

	ioLBuffer1Ptr = gInputBufferPtr;
	ioLBuffer2Ptr = gOutputBufferPtr;
	
			// Initialize the buffers to load the right part of image data 	
			// into.																				
			// 'leftBytesToMove' is a temporary variable here.						
	
	ioRBuffer1Ptr = gInputBuffer2Ptr;
	ioRBuffer2Ptr = gOutputBuffer2Ptr;
		
			// Get the number of bytes to move for the left and right side		
			// of the images each time through the channel loop.					
			
	leftBytesToMove = 
			numberLeftColumns * columnInterval * numberOutputBytes;
	rightBytesToMove =
			numberRightColumns * columnInterval * numberOutputBytes;									
			
	if (ignoreBackgroundFlag)
		backgroundByteLength = GetBackgroundValueForDataTypeCode (
													outFileInfoPtr, 
													backgroundValue,
													&backgroundString);
	
	line = 0;
	line1 = reformatOptionsPtr->lineStart;
	line2 = reformatOptionsPtr->startLine;
	while (line < numberOutputLines && continueFlag)
		{
				// Add the preline calibration bytes if any.  For now this is	
				// only handled for GAIA data.											
				
		if (preLineBytes)
			{
			BlockMoveData (&GAIAPrelineString, ioOutLBufferPtr, preLineBytes);
			ioOutLBufferPtr += 14;
			
			}	// end "if (preLineBytes)" 
			
				// Get all requested channels for first part of line of image 	
				// data.  Get all requested channels for second part of line 	
				// of image data.  Return if there is a file IO error.			
			 
		errCode = GetLineOfData (fileIOInstructionsLeftPtr,
											line1, 
											leftColumnStart,
											leftColumnEnd,
											1,
											(HUCharPtr)ioLBuffer1Ptr,  
											(HUCharPtr)ioLBuffer2Ptr);
										
		if (errCode != noErr)
			{
			continueFlag = FALSE;
																						break;
																						
			}	// end "if (errCode != noErr)"
			 
		errCode = GetLineOfData (fileIOInstructionsRightPtr,
											line2, 
											rightColumnStart,
											rightColumnEnd,
											1,
											(HUCharPtr)ioRBuffer1Ptr,  
											(HUCharPtr)ioRBuffer2Ptr);
										
		if (errCode != noErr)
			{
			continueFlag = FALSE;
																						break;
																						
			}	// end "if (errCode != noErr)"
		
		columnBytes = 0;																
		if (ignoreBackgroundFlag)
			{				
			leftBytesToMove = FindLeftBytesToMove (
													(char*)ioLBuffer2Ptr,
													numberOutputBytes,
													(char*)&backgroundString,
													numberLeftColumns,
													columnInterval,
													numberOutputChannels,
													leftSkip);	
													
					// Find first valid data value at beginning of right part 	
					// of line.		
					
			rightBytesToMove = FindRightBytesToMove (
													(char*)ioRBuffer2Ptr,
													numberOutputBytes,
													(char*)&backgroundString,
													numberRightColumns,
													columnInterval,
													numberOutputChannels,
													rightSkip);
													
			columnBytes = numberRightColumns * numberOutputBytes - 
															(rightBytesToMove/columnInterval);
															
			if (forceBISFlag)
				columnBytes *= numberOutputChannels;
								
					// If this is the data for the first line, then update		
					// the number of bytes that will be moved to the output		
					// buffer in case there are some samples that will be			
					// ignored.  NOTE - The results for the first line will		
					// determine the number of columns in the output line.		
					
			if (line == 0)
				{
				numberColumnsReduced = outFileInfoPtr->numberColumns - 
									(leftBytesToMove + rightBytesToMove) /
																columnInterval / numberOutputBytes;
																		
				outFileInfoPtr->numberColumns -= numberColumnsReduced;

						// Update some intermediate values used to read data in and to 
						// compute the size of the image.
																		
				IntermediateFileUpdate (outFileInfoPtr);
				
				reformatOptionsPtr->countOutBytes -= numberColumnsReduced * 
														numberOutputChannels * numberOutputBytes;
					
				reformatOptionsPtr->ioOutBufferBytes = outFileInfoPtr->numberColumns *
									reformatOptionsPtr->numberOutputBufferLines *
														numberOutputChannels * numberOutputBytes;
												
				GetOutputBufferParameters (outFileInfoPtr, 
													reformatOptionsPtr,
													&outChannelByteIncrement,
													&outLineByteIncrement,
													&outNumberBytesPerLineAndChannel);
													
				countOutBytes = reformatOptionsPtr->countOutBytes;
		
				}	// end "if (line == 0)" 
				
			}	// end "if (ignoreBackgroundFlag)" 
			
				// Now move the data to the output buffer.							
				
		channelCount = 0;
		while (channelCount<numberOutputChannels && continueFlag)
			{
					// Set input buffer pointer for the channel to be handled.					
			
			inputBufferIndex = channelCount * numberLeftBytesPerChannel;
				
					// Move the left part of the line in.								
					
			BlockMoveData (&ioLBuffer2Ptr[inputBufferIndex], 
									ioOutLBufferPtr, 
									leftBytesToMove);
			
					// Set input buffer pointer for the channel to be handled	
					// for the right part of the image.													
			
			if (forceBISFlag)
				inputBufferIndex = columnBytes + channelCount * numberOutputBytes;
				
			else	// !forceBISFlag 
				inputBufferIndex =  channelCount * numberRightBytesPerChannel + 
																							columnBytes;
				
					// Move the right part of the line in.								
					
			BlockMoveData (&ioRBuffer2Ptr[inputBufferIndex], 
								&ioOutLBufferPtr[leftBytesToMove], 
								rightBytesToMove);
			
			channelCount++;
			
			ioOutLBufferPtr += outChannelByteIncrement;
			
			if (forceBISFlag)
				channelCount = numberOutputChannels;
			
			}	// end "while (channelCount<numberOutputChannels..." 
		
				// Update count of lines.							
				
		line++;
		
		ioOutLBufferPtr = &savedOutBufferPtr [outLineByteIncrement];
		savedOutBufferPtr = ioOutLBufferPtr;
			
				// Write line(s), channel(s) of data when needed.					
		
		totalIOOutBytes += countOutBytes;
		
		if (continueFlag && (totalIOOutBytes > limitIoOutBytes))
			{
			errCode = WriteOutputDataToFile (outFileInfoPtr,
															outFileStreamPtr,
															ioOutBufferPtr,
															reformatOptionsPtr->channelPtr,
															numberOutputChannels,
															lastOutputWrittenLine,
															outNumberBytesPerLineAndChannel,
															numberOutputLines,
															outChannelByteIncrement,
															totalIOOutBytes,
															reformatOptionsPtr,
															1);
			
			totalIOOutBytes = 0;
			ioOutLBufferPtr = (unsigned char *)ioOutBufferPtr;
			savedOutBufferPtr = ioOutLBufferPtr; 
			
			lastOutputWrittenLine = line;
															
			if (errCode != noErr)
				{
				continueFlag = FALSE;
																						break;
				
				}	// end "if (errCode != noErr)"
				
			}	// end "if (totalIOOutBytes > limitIoOutBytes)" 
		
				// Check if user wants to abort processing.							
				
		if (TickCount () >= gNextTime)
			{
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
				continueFlag = FALSE;
				
			}	// end "if (TickCount () >= nextTime)"
		
				// Update status dialog box.												
				
		percentComplete = 100 * line/numberOutputLines;
		if (percentComplete != lastPercentComplete)
			{
			LoadDItemValue (
						gStatusDialogPtr, IDC_ShortStatusValue, (SInt32)percentComplete);
			lastPercentComplete = percentComplete;
			
			}	// end "if (percentComplete != lastPercentComplete)" 
		
				// Adjust line depending line interval.								
				
		line1++;
		line2++;
			
		}	// end "while (line < numberOutputLines && continueFlag)" 
	
			// Flush output buffer if needed.											
	
	if (continueFlag && totalIOOutBytes > 0)
		{		
		errCode = WriteOutputDataToFile (outFileInfoPtr,
													outFileStreamPtr,
													ioOutBufferPtr,
													reformatOptionsPtr->channelPtr,
													numberOutputChannels,
													lastOutputWrittenLine,
													outNumberBytesPerLineAndChannel,
													numberOutputLines,
													outChannelByteIncrement,
													totalIOOutBytes,
													reformatOptionsPtr,
													1);
														
		if (errCode != noErr)	
			continueFlag = FALSE;							
		
		}	// end "if (continueFlag && totalIOOutBytes > 0)" 
		
	*numberColumnsReducedPtr = numberColumnsReduced;
		
	return (continueFlag);
			
}	// end "MosaicTwoImagesLeftRight"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean MosaicTwoImagesTopBottom
//
//	Software purpose:	This routine joins the two input images side by
//							side.  If requested the specified background values
//							at the end of the first part of the line and at the
//							beginning of the second part of the line will be
//							ignored.  If background values are left out the
//							header, if present, will be updated to reflect
//							the new number of columns in each line.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/15/2014
//	Revised By:			Larry L. Biehl			Date: 03/15/2014

Boolean MosaicTwoImagesTopBottom (
				FileIOInstructionsPtr			fileIOInstructionsTopPtr, 
				FileIOInstructionsPtr			fileIOInstructionsBottomPtr,
				FileInfoPtr							outFileInfoPtr, 
				ReformatOptionsPtr				reformatOptionsPtr)

{
			// Declare local variables & structures										
	
	CMFileStream*						outFileStreamPtr;
												
	FileInfoPtr							bottomFileInfoPtr,
											topFileInfoPtr;
	
	HUCharPtr							ioBuffer1Ptr,
											ioBuffer2Ptr;
	
	unsigned char 						*ioOutBufferPtr,
											*savedOutBufferPtr;
	
	SInt32								countOutBytes,
											lastOutputWrittenLine,
											lastPercentComplete,
											limitIoOutBytes,
											percentComplete,
											preLineBytes,
											totalIOOutBytes;
	
	UInt32								bottomLine,
											bottomColumnEnd,
											bottomColumnStart,
											bytesToMove,
											channelCount,
											columnInterval,
											inputBufferIndex,
											line,
											numberBottomLines,
											numberBytesPerChannel,
											numberOutputColumns,
											numberOutputLines,
											numberOutputChannels,
											numberTopLines,
											outNumberBytesPerLineAndChannel,
											outChannelByteIncrement,
											outLineByteIncrement,
											topColumnEnd,
											topColumnStart,
											topLine;
	
	SInt16								errCode,
											GAIAPrelineString[7] = 
													{	0x0300, 0x0600, 0x0000, 0x0100, 
														0x0000, 0x0000, 0x0000};
	
	UInt16								numberOutputBytes;
	
	Boolean								continueFlag,
											forceBISFlag;
												
		
			// Initialize local variables.													
			
	errCode = noErr;  
	continueFlag = TRUE;
	lastOutputWrittenLine = 0;
	forceBISFlag = (outFileInfoPtr->bandInterleave == kBIS);
	preLineBytes = outFileInfoPtr->numberPreLineBytes;
	topFileInfoPtr = fileIOInstructionsTopPtr->fileInfoPtr;
	bottomFileInfoPtr = fileIOInstructionsBottomPtr->fileInfoPtr;
					
	ioOutBufferPtr = reformatOptionsPtr->ioOutBufferPtr;
				
	lastPercentComplete = -1;
	
	topColumnStart = reformatOptionsPtr->columnStart;
	topColumnEnd =  reformatOptionsPtr->columnEnd;
	numberTopLines = reformatOptionsPtr->lineEnd - reformatOptionsPtr->lineStart + 1;
	
	bottomColumnStart = reformatOptionsPtr->startColumn;
	bottomColumnEnd =  reformatOptionsPtr->stopColumn;
	numberBottomLines =
							reformatOptionsPtr->stopLine - reformatOptionsPtr->startLine + 1;
	
	numberOutputColumns = topColumnEnd - topColumnStart + 1;
	numberBytesPerChannel = numberOutputColumns * outFileInfoPtr->numberBytes;
	numberOutputLines = numberTopLines + numberBottomLines;
	
	numberOutputChannels = reformatOptionsPtr->numberChannels;
	numberOutputBytes = outFileInfoPtr->numberBytes;
		
	outFileStreamPtr = GetFileStreamPointer (outFileInfoPtr);
	
	if (forceBISFlag)
		{
		columnInterval = numberOutputChannels;
		
		}	// end "if (forceBISFlag)" 
	
	else	// !forceBISFlag 
		{
		columnInterval = 1;
		
		}	// end "else !forceBISFlag" 

			// Set up output working buffer pointer.  This pointer will			
			// change as the output buffer fills up									
	
	savedOutBufferPtr = (unsigned char *)ioOutBufferPtr;
	
			// Set up limit of output buffer to know when to write buffer to	
			// the disk file. 																
			
	countOutBytes = reformatOptionsPtr->countOutBytes;
	limitIoOutBytes = reformatOptionsPtr->ioOutBufferBytes - countOutBytes;
	totalIOOutBytes = 0;
	
			// Get count of bytes that are loaded into the output buffer each	
			// time through the channel loop.										
			
	GetOutputBufferParameters (outFileInfoPtr, 
										reformatOptionsPtr,
										&outChannelByteIncrement,
										&outLineByteIncrement,
										&outNumberBytesPerLineAndChannel);

			// Initialize the buffers to load the left part of image data 		
			// into.																				

	ioBuffer1Ptr = gInputBufferPtr;
	ioBuffer2Ptr = gOutputBufferPtr;
		
			// Get the number of bytes to move each time through the channel loop.					
			
	bytesToMove = numberOutputColumns * columnInterval * numberOutputBytes;	
			
			// Get the top part of the new image file.
	
	line = 0;
	topLine = reformatOptionsPtr->lineStart;
	while (line < numberTopLines && continueFlag)
		{
				// Add the preline calibration bytes if any.  For now this is	
				// only handled for GAIA data.											
				
		if (preLineBytes)
			{
			BlockMoveData (&GAIAPrelineString, ioOutBufferPtr, preLineBytes);
			ioOutBufferPtr += 14;
			
			}	// end "if (preLineBytes)" 
			
				// Get all requested channels for line in the top image 	
				// file. Return if there is a file IO error.			
			 
		errCode = GetLineOfData (fileIOInstructionsTopPtr,
											topLine, 
											topColumnStart,
											topColumnEnd,
											1,
											(HUCharPtr)ioBuffer1Ptr,  
											(HUCharPtr)ioBuffer2Ptr);
											
		if (errCode != noErr)
			{
			continueFlag = FALSE;
																						break;
																						
			}	// end "if (errCode != noErr)"
		
				// Now move the data to the output buffer.							
				
		channelCount = 0;
		while (channelCount<numberOutputChannels && continueFlag)
			{
					// Set input buffer pointer for the channel to be handled.					
			
			inputBufferIndex = channelCount * numberBytesPerChannel;
				
					// Move the line to the output buffer.								
					
			BlockMoveData (&ioBuffer2Ptr[inputBufferIndex], 
									ioOutBufferPtr, 
									bytesToMove);
			
			channelCount++;
			
			ioOutBufferPtr += outChannelByteIncrement;
			
			if (forceBISFlag)
				channelCount = numberOutputChannels;
			
			}	// end "while (channelCount<numberOutputChannels..." 
		
				// Update count of lines.							
				
		line++;
		
		ioOutBufferPtr = &savedOutBufferPtr [outLineByteIncrement];
		savedOutBufferPtr = ioOutBufferPtr;
			
				// Write line(s), channel(s) of data when needed.					
		
		totalIOOutBytes += countOutBytes;
		
		if (continueFlag && (totalIOOutBytes > limitIoOutBytes))
			{
			ioOutBufferPtr = (unsigned char *)reformatOptionsPtr->ioOutBufferPtr;
			errCode = WriteOutputDataToFile (outFileInfoPtr,
															outFileStreamPtr,
															ioOutBufferPtr,
															reformatOptionsPtr->channelPtr,
															numberOutputChannels,
															lastOutputWrittenLine,
															outNumberBytesPerLineAndChannel,
															numberOutputLines,
															outChannelByteIncrement,
															totalIOOutBytes,
															reformatOptionsPtr,
															1);
			
			totalIOOutBytes = 0;
			savedOutBufferPtr = ioOutBufferPtr; 
			
			lastOutputWrittenLine = line;
															
			if (errCode != noErr)
				{
				continueFlag = FALSE;
																						break;
				
				}	// end "if (errCode != noErr)"
				
			}	// end "if (totalIOOutBytes > limitIoOutBytes)" 
		
				// Check if user wants to abort processing.							
				
		if (TickCount () >= gNextTime)
			{
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
				continueFlag = FALSE;
				
			}	// end "if (TickCount () >= nextTime)"
		
				// Update status dialog box.												
				
		percentComplete = 100 * line/numberOutputLines;
		if (percentComplete != lastPercentComplete)
			{
			LoadDItemValue (
							gStatusDialogPtr, IDC_ShortStatusValue, (SInt32)percentComplete);
			lastPercentComplete = percentComplete;
			
			}	// end "if (percentComplete != lastPercentComplete)" 
		
				// Adjust line depending line interval.								
				
		topLine++;
			
		}	// end "while (line < numberTopLines && continueFlag)"
		
			// Now get the bottom part of the new image file.
	
	bottomLine = reformatOptionsPtr->startLine;
	while (line < numberOutputLines && continueFlag)
		{
				// Add the preline calibration bytes if any.  For now this is	
				// only handled for GAIA data.											
				
		if (preLineBytes)
			{
			BlockMoveData (&GAIAPrelineString, ioOutBufferPtr, preLineBytes);
			ioOutBufferPtr += 14;
			
			}	// end "if (preLineBytes)" 
			
				// Get all requested channels for line in the bottom image 	
				// file. Return if there is a file IO error.
			 
		errCode = GetLineOfData (fileIOInstructionsBottomPtr,
											bottomLine, 
											bottomColumnStart,
											bottomColumnEnd,
											1,
											(HUCharPtr)ioBuffer1Ptr,  
											(HUCharPtr)ioBuffer2Ptr);
											
		if (errCode != noErr)
			{
			continueFlag = FALSE;
																						break;
																						
			}	// end "if (errCode != noErr)"
		
				// Now move the data to the output buffer.							
				
		channelCount = 0;
		while (channelCount<numberOutputChannels && continueFlag)
			{
					// Set input buffer pointer for the channel to be handled.					
			
			inputBufferIndex = channelCount * numberBytesPerChannel;
				
					// Move the left part of the line in.								
					
			BlockMoveData (&ioBuffer2Ptr[inputBufferIndex], 
									ioOutBufferPtr, 
									bytesToMove);
			
			channelCount++;
			
			ioOutBufferPtr += outChannelByteIncrement;
			
			if (forceBISFlag)
				channelCount = numberOutputChannels;
			
			}	// end "while (channelCount<numberOutputChannels..." 
		
				// Update count of lines.							
				
		line++;
		
		ioOutBufferPtr = &savedOutBufferPtr [outLineByteIncrement];
		savedOutBufferPtr = ioOutBufferPtr;
			
				// Write line(s), channel(s) of data when needed.					
		
		totalIOOutBytes += countOutBytes;
		
		if (continueFlag && (totalIOOutBytes > limitIoOutBytes))
			{
			ioOutBufferPtr = (unsigned char *)reformatOptionsPtr->ioOutBufferPtr;
			errCode = WriteOutputDataToFile (outFileInfoPtr,
															outFileStreamPtr,
															ioOutBufferPtr,
															reformatOptionsPtr->channelPtr,
															numberOutputChannels,
															lastOutputWrittenLine,
															outNumberBytesPerLineAndChannel,
															numberOutputLines,
															outChannelByteIncrement,
															totalIOOutBytes,
															reformatOptionsPtr,
															1);
			
			totalIOOutBytes = 0;
			savedOutBufferPtr = ioOutBufferPtr; 
			
			lastOutputWrittenLine = line;
															
			if (errCode != noErr)
				{
				continueFlag = FALSE;
																						break;
				
				}	// end "if (errCode != noErr)"
				
			}	// end "if (totalIOOutBytes > limitIoOutBytes)" 
		
				// Check if user wants to abort processing.							
				
		if (TickCount () >= gNextTime)
			{
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
				continueFlag = FALSE;
				
			}	// end "if (TickCount () >= nextTime)"
		
				// Update status dialog box.												
				
		percentComplete = 100 * line/numberOutputLines;
		if (percentComplete != lastPercentComplete)
			{
			LoadDItemValue (
						gStatusDialogPtr, IDC_ShortStatusValue, (SInt32)percentComplete);
			lastPercentComplete = percentComplete;
			
			}	// end "if (percentComplete != lastPercentComplete)" 
		
				// Adjust line depending line interval.								
				
		bottomLine++;
			
		}	// end "while (line < numberBottomLines && continueFlag)"
				
			// Flush output buffer if needed.											
	
	if (continueFlag && totalIOOutBytes > 0)
		{		
		errCode = WriteOutputDataToFile (outFileInfoPtr,
													outFileStreamPtr,
													reformatOptionsPtr->ioOutBufferPtr,
													reformatOptionsPtr->channelPtr,
													numberOutputChannels,
													lastOutputWrittenLine,
													outNumberBytesPerLineAndChannel,
													numberOutputLines,
													outChannelByteIncrement,
													totalIOOutBytes,
													reformatOptionsPtr,
													1);
														
		if (errCode != noErr)	
			continueFlag = FALSE;							
		
		}	// end "if (continueFlag && totalIOOutBytes > 0)" 
		
	return (continueFlag);
			
}	// end "MosaicTwoImagesTopBottom"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		MosaicTwoImagesControl
//
//	Software purpose:	This routine controls the process of mosaicing
//							two images side by side.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/05/1993
//	Revised By:			Larry L. Biehl			Date: 03/15/2014

void MosaicTwoImagesControl (void)

{
			// Declare local variables & structures
	
	FileInfoPtr							outFileInfoPtr,
											rightFileInfoPtr;
	
	FileIOInstructionsPtr			fileIOInstructionsLeftPtr,
											fileIOInstructionsRightPtr;
												
	LayerInfoPtr						rightLayerInfoPtr;
	ReformatOptionsPtr				reformatOptionsPtr;
	WindowInfoPtr						rightWindowInfoPtr;
	
	Handle								outFileInfoHandle = NULL,
											reformatOptionsH = NULL;
	
	time_t								startTime;
	
	SInt16								rightBottomImageHandleStatus;
			
	Boolean								continueFlag,
											savedSwapBytesFlag;

	
			// If spare memory had to be used to load code resources, then exit	
			// routine.																				
			
	if (gMemoryTypeNeeded < 0)
																							return;
																							
			// Code resources loaded okay, so set flag back for non-Code			
			// resources.																			
			
	gMemoryTypeNeeded = 0;
																							
			// Check input global variables.													
			
	if (gImageWindowInfoPtr == NULL || gImageFileInfoPtr == NULL)
																							return;
																						
			// Initialize local variables.													
			
	continueFlag = TRUE;
	fileIOInstructionsLeftPtr = NULL;
	fileIOInstructionsRightPtr = NULL;
	reformatOptionsPtr = NULL; 
	rightBottomImageHandleStatus = -1;
	
			// Get a handle to a block of memory to be used for						
			// file information for the new image window.								
			// Lock the memory until the information has been loaded					
			// in. Then get the pointer to the file information block				
			
			// Get a handle to a block of memory to use for the reformatting		
			// options.  Then lock it and get a pointer to the block.				
		
	continueFlag = GetReformatAndFileInfoStructures (
													&outFileInfoHandle, &reformatOptionsH);
	
	if (continueFlag)
		{												
		outFileInfoPtr = (FileInfoPtr)GetHandlePointer (outFileInfoHandle, kLock);
												
		reformatOptionsPtr =
							(ReformatOptionsPtr)GetHandlePointer (reformatOptionsH, kLock);
			
		if (gImageFileInfoPtr->format != kGAIAType &&
												gImageFileInfoPtr->format != kGAIA2Type)
			reformatOptionsPtr->swapBytesFlag = gImageFileInfoPtr->swapBytesFlag;
			
		reformatOptionsPtr->signedOutputDataFlag = gImageFileInfoPtr->signedDataFlag;
			
		outFileInfoHandle = NULL;
	
				// Branch to end of routine if memory is full and do cleanup of handles.
			
		reformatOptionsPtr->numberChannels = gImageWindowInfoPtr->totalNumberChannels;
			
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
				
		if (MosaicTwoImagesDialog (
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
			
					// Get pointer to memory to use to read left image file line		
					// into. 																		
			
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
														1,
														reformatOptionsPtr->numberChannels,
														(UInt16*)reformatOptionsPtr->channelPtr,
														kPackData,
														outFileInfoPtr->bandInterleave == kBIS,
														reformatOptionsPtr->forceByteCode,
														kDoNotAllowForThreadedIO,
														&fileIOInstructionsLeftPtr);

					// Get the file information pointer for the right file.					

			if (continueFlag)	
				GetImageInformationPointers (
										&rightBottomImageHandleStatus,
										reformatOptionsPtr->rightBottomMosaicWindowInfoHandle,
										&rightWindowInfoPtr,
										&rightLayerInfoPtr,
										&rightFileInfoPtr);
			
					// Get pointer to memory to use to read right image file line		
					// into. 																								
			
			if (continueFlag)	 						
				continueFlag = GetIOBufferPointers (
										&gFileIOInstructions[1],
										rightWindowInfoPtr,
										rightLayerInfoPtr,
										rightFileInfoPtr,
										&gInputBuffer2Ptr, 
										&gOutputBuffer2Ptr,
										reformatOptionsPtr->startColumn,
				 						reformatOptionsPtr->stopColumn,
				 						1,
				 						reformatOptionsPtr->numberChannels,
				 						(UInt16*)reformatOptionsPtr->channelPtr, 
										kPackData, 
										outFileInfoPtr->bandInterleave == kBIS, 
										reformatOptionsPtr->forceByteCode,
										kDoNotAllowForThreadedIO,
										&fileIOInstructionsRightPtr);
	
					// Get buffer for output data.											
					
			if (continueFlag)
				continueFlag = GetReformatOutputBuffer (outFileInfoPtr, reformatOptionsPtr);
		
					// Branch to end of routine if memory is full and do cleanup of
					// handles.
				
			if (continueFlag)
				{		
						// Set up the disk file to be used for the new reformatted	
						// image file.																
						
				continueFlag = !GetReformatOutputFile (outFileInfoPtr, reformatOptionsPtr);
						
				}	// end "if (continueFlag)"  
						
			if (continueFlag)
				{
						// Change cursor to watch cursor until done with
						// MosaicTwoImagesControl process.
				
				MSetCursor (kWait);
					
						// Get status dialog. Also call routine to do					
						// activate/update events.												
					
				gStatusDialogPtr = GetStatusDialog (kShortStatusInfoID, FALSE);
				
				if (gStatusDialogPtr)
					{
							// Save the input swap byte flag and set the input			
							// file swap bytes flag to that requested by the			
							// reformat options.  The swapping is done when the		
							// input data is read in.											
							// If the data file is being converted to GAIA format		
							// and the input file is only one byte, then set the		
							// swap flag to TRUE to force the output data to be		
							// FALSE.																
								
					savedSwapBytesFlag = gImageFileInfoPtr->swapBytesFlag;
					gImageFileInfoPtr->swapBytesFlag = reformatOptionsPtr->swapBytesFlag;
					
					if (outFileInfoPtr->format == kGAIAType && 
													gImageFileInfoPtr->numberBytes == 1)
						gImageFileInfoPtr->swapBytesFlag = TRUE;
					
					MGetString (gTextString, kReformatStrID, IDS_PercentComplete);
					LoadDItemString (
								gStatusDialogPtr, IDC_ShortStatusText, (Str255*)gTextString);
					LoadDItemValue (gStatusDialogPtr, IDC_ShortStatusValue, (SInt32)0);
					ShowStatusDialogWindow (kShortStatusInfoID);
					
							// Everything has now been set up.								
							// Do reformatting for BIL output files here.				
							
					startTime = time (NULL);
														
					continueFlag = MosaicTwoImages (fileIOInstructionsLeftPtr,
																fileIOInstructionsRightPtr,
																outFileInfoPtr, 
																reformatOptionsPtr);
																	
							// Restore the input swap byte flag.							
							
					gImageFileInfoPtr->swapBytesFlag = savedSwapBytesFlag;
					
							// List Reformat information.										
						
					if (continueFlag)
						continueFlag = ListReformatResultsInformation (
																reformatOptionsPtr, outFileInfoPtr);
				
							// List the CPU time taken for reformatting.					
								
					ListCPUTimeInformation (NULL, TRUE, startTime);
								
							// Dispose of status dialog										
					
					CloseStatusDialog (TRUE);
						
					}	// end "if (gStatusDialogPtr)" 
		  		
		  				// Close the output file		
		  				
				CloseFile (outFileInfoPtr);
			
						// Reset cursor back to arrow to indicate that process		
						// is done																	
					
				MInitCursor ();
					
				}	// end "if (continueFlag)" 
				
					// Dispose of the IO buffer for right image.		
										
			DisposeIOBufferPointers (fileIOInstructionsRightPtr,
												&gInputBuffer2Ptr, 
												&gOutputBuffer2Ptr);
		
			UnlockImageInformationHandles (
										rightBottomImageHandleStatus,
										reformatOptionsPtr->rightBottomMosaicWindowInfoHandle);
				
					// Dispose of the IO buffer for left image.		
										
			DisposeIOBufferPointers (fileIOInstructionsLeftPtr,
												&gInputBufferPtr, 
												&gOutputBufferPtr);
				
			}	// end "if (ChangeImageFormatDialog (gImageFileInfoPtr, ... "
			
		}	// end "if (continueFlag)" 
		
	ReleaseReformatOutputFileInfoAndBuffers (reformatOptionsPtr, gImageFileInfoPtr);
	
}	// end "MosaicTwoImagesControl"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean MosaicTwoImagesDialog
//
//	Software purpose:	This routine gets the mosaic images side by side
//							specifications that the user wants to use from 
//							a dialog box.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/05/1993
//	Revised By:			Larry L. Biehl			Date: 12/16/2016

Boolean MosaicTwoImagesDialog (
				FileInfoPtr							fileInfoPtr, 
				FileInfoPtr							outFileInfoPtr, 
				ReformatOptionsPtr				reformatOptionsPtr)

{
	Boolean								returnFlag = TRUE;
	
#if defined multispec_mac
	DialogSelectArea					leftTopDialogSelectArea,
											rightBottomDialogSelectArea;
	
	Rect									theBox;
	
	DialogPtr							dialogPtr;
			
	UserItemUPP							drawHeaderOptionsPopUpPtr,
											drawMosaicDirectionPopUpPtr;
								
	Handle								okHandle,
											rightOrBottomMosaicWindowInfoHandle,
											theHandle;
	
	SInt32								maxDataValue,
											newBackgroundValue,
											theNum;
								
	SInt16								itemHit,
											itemHit2,
											theType;

	Boolean								ignoreBackgroundFlag,
											modalDone,
											updateLineColumnEndFlag;
	
	SignedByte							handleStatus;
									

			// Get the modal dialog for the image file format description			
				
	dialogPtr = LoadRequestedDialog (kMosaicDialogID, kCopyScrap, 1, 2);
	if (dialogPtr == NULL)													
																			return (FALSE);
		
			// Intialize local user item proc pointers.
			
	drawMosaicDirectionPopUpPtr = NewUserItemUPP (DrawMosaicDirectionPopUp);
	drawHeaderOptionsPopUpPtr = NewUserItemUPP (DrawHeaderOptionsPopUp);
	
			// Initialize dialog variables.

	MosaicTwoImagesDialogInitialize (dialogPtr,
														reformatOptionsPtr,
														fileInfoPtr,
														outFileInfoPtr,
														&gMosaicDirectionCode,
														&leftTopDialogSelectArea,
														&rightBottomDialogSelectArea,
														&gFileNamesSelection,
														&rightOrBottomMosaicWindowInfoHandle,
														&ignoreBackgroundFlag,
														&maxDataValue,
														&newBackgroundValue,
														&gHeaderOptionsSelection);

			// Save handle for the OK button for use later.								
			
	GetDialogItem (dialogPtr, 1,  &theType, &okHandle, &theBox);

			//	Initialize the dialog.	
			// "Mosaic Direction" popup box.														
	
	SetDialogItemDrawRoutine (dialogPtr, 4, drawMosaicDirectionPopUpPtr);	
	
			// "Right Image file" popup box.													
			
	SetDialogItemDrawRoutine (dialogPtr, 8, gDrawImageFilePopUpPtr);
	
			// "Bottom Image file" popup box.													
			
	SetDialogItemDrawRoutine (dialogPtr, 10, gDrawImageFilePopUpPtr);

			//	Flag indicating whether right or bottom column/line ends need to 
			// be checked.																
			
	updateLineColumnEndFlag = FALSE;
			
			// "Image Header" popup box.														
	
	SetDialogItemDrawRoutine (dialogPtr, 42, drawHeaderOptionsPopUpPtr);
		
			// Center the dialog and then show it.											
			
	ShowDialogWindow (dialogPtr, kMosaicDialogID, kSetUpDFilterTable);
	
			// Set default text selection to first edit text item						
			
	SelectDialogItemText (dialogPtr, 15, 0, INT16_MAX);
								
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
				case 4:			// Popup box for mosaic direction
					itemHit = StandardPopUpMenu (dialogPtr, 
															itemHit-1, 
															itemHit, 
															gPopUpMosaicDirectionMenu, 
															gMosaicDirectionCode,
															kPopUpMosaicDirectionMenuID);
															
					if (itemHit > 0 && gMosaicDirectionCode != itemHit)
						{
						gMosaicDirectionCode = itemHit;
						MosaicTwoImagesDialogUpdateDirectionItems (
															dialogPtr,
															&leftTopDialogSelectArea,
															&rightBottomDialogSelectArea,
															rightOrBottomMosaicWindowInfoHandle,
															gMosaicDirectionCode,
															ignoreBackgroundFlag);
																					
						}	// end "if (itemHit > 0 && gMosaicDirectionCode != itemHit)"
					break;
					
				case 8:				// "Right Image File" popup box 
					itemHit2 = StandardPopUpMenu (dialogPtr, 
															itemHit-1,
															itemHit, 
															gPopUpTemporaryMenu, 
															gFileNamesSelection,
															kColorMenuID);
													
					if (itemHit2 != gFileNamesSelection && itemHit2 != 0)
						{
						gFileNamesSelection = itemHit2;
						MosaicTwoImagesDialogOnSelectRightBottomImage (
															dialogPtr,
															&leftTopDialogSelectArea,
															&rightBottomDialogSelectArea,
															gFileNamesSelection,
															&rightOrBottomMosaicWindowInfoHandle,
															gMosaicDirectionCode);
								
								// Force lines on right image to be checked against the left
								//image.

						//itemHit = 25;
						//updateLineColumnEndFlag = TRUE;
						
						}	// end "if (itemHit2 != gFileNamesSelection ..." 
	
							// Make certain that the correct label is drawn in the	
							// header options pop up box.										
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
					
				case 10:				// "Bottom Image File" popup box 
					itemHit2 = StandardPopUpMenu (dialogPtr, 
															itemHit-1,
															itemHit, 
															gPopUpTemporaryMenu, 
															gFileNamesSelection,
															kColorMenuID);
													
					if (itemHit2 != gFileNamesSelection && itemHit2 != 0)
						{
						gFileNamesSelection = itemHit2;

						MosaicTwoImagesDialogOnSelectRightBottomImage (
															dialogPtr,
															&leftTopDialogSelectArea,
															&rightBottomDialogSelectArea,
															gFileNamesSelection,
															&rightOrBottomMosaicWindowInfoHandle,
															gMosaicDirectionCode);
								
								// Force columns on bottom image to be checked against the
								// top image.
						
						//itemHit = 35;
						//updateLineColumnEndFlag = TRUE;
						
						}	// end "if (itemHit2 != gFileNamesSelection ..." 
	
							// Make certain that the correct label is drawn in the	
							// header options pop up box.										
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;

					
				case 11:		// Entire area to selected area switch.
				case 43:		// Entire area to selected area switch. (Appearance Manager)
				case 15:		//	 left image lineStart
				case 16:		//	 left image lineEnd
				case 18:		//	 left image columnStart
				case 19:		//	 left image columnEnd
					DialogLineColumnHits (&leftTopDialogSelectArea,
													dialogPtr, 
													itemHit,
													theHandle,
													theNum);
					
					if (gMosaicDirectionCode == kMosaicLeftRight &&
									(leftTopDialogSelectArea.lineEnd -
														leftTopDialogSelectArea.lineStart) != 
						  			(rightBottomDialogSelectArea.lineEnd -
															rightBottomDialogSelectArea.lineStart))
						updateLineColumnEndFlag = TRUE;
					
					else if (gMosaicDirectionCode == kMosaicTopBottom &&
									(leftTopDialogSelectArea.columnEnd -
														leftTopDialogSelectArea.columnStart) != 
						  			(rightBottomDialogSelectArea.columnEnd -
															rightBottomDialogSelectArea.columnStart))
						updateLineColumnEndFlag = TRUE;						
					break;
					
				case 25:				//	 right image lineStart  
				case 27:				//	 right image columnStart  
				case 28:				//	 right image columnEnd  
				
				case 33:				//	 bottom image lineStart  
				case 34:				//	 bottom image lineEnd  
				case 35:				//	 bottom image columnStart 
					handleStatus = HGetState (rightOrBottomMosaicWindowInfoHandle);
					HLock (rightOrBottomMosaicWindowInfoHandle);
					rightBottomDialogSelectArea.imageWindowInfoPtr = 
											(WindowInfoPtr)*rightOrBottomMosaicWindowInfoHandle;
					
					DialogLineColumnHits (&rightBottomDialogSelectArea,
													dialogPtr, 
													itemHit,
													theHandle,
													theNum,
													FALSE);
													
					HSetState (rightOrBottomMosaicWindowInfoHandle, handleStatus);
					rightBottomDialogSelectArea.imageWindowInfoPtr = NULL;
					if (itemHit == 25)
						updateLineColumnEndFlag = TRUE;
					if (itemHit == 35)
						updateLineColumnEndFlag = TRUE;
					break;
					
				case 38:				// "Ignore background values" Check box 
					ignoreBackgroundFlag = !ignoreBackgroundFlag;
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					MosaicTwoImagesDialogUpdateBackGroundValuesItem (
																	dialogPtr,
																	ignoreBackgroundFlag);
					break;
					
				case 39:				// background data value 
					if (theNum < 0 || theNum > maxDataValue)
						NumberErrorAlert ((SInt32)newBackgroundValue, dialogPtr, itemHit);
					else
						newBackgroundValue = theNum;
					break;
					
				case 40:				// "Write channel descriptions" Check box 
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					break;
					
				case 42:				// "Write Header" popup box 
					itemHit2 = StandardPopUpMenu (dialogPtr, 
															itemHit-1,
															itemHit, 
															gPopUpHeaderOptionsMenu, 
															gHeaderOptionsSelection,
															kPopUpHeaderOptionsMenuID);
													
					if (itemHit2 != 0 && gHeaderOptionsSelection != itemHit2)
						{
						gHeaderOptionsSelection = itemHit2;
						
						MosaicTwoImagesDialogUpdateChannelDescriptionItem (
									dialogPtr,
									GetHeaderFormatFromPopUpSelection (gHeaderOptionsSelection),
									outFileInfoPtr->thematicType);
							
						}	// end "if (itemHit2 != 0 && gHeaderOptionsSelection != ..."
	
							// Make certain that the correct label is drawn in the	
							// header options pop up box.										
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
						
				}	// end "switch (itemHit)" 
				
			if (updateLineColumnEndFlag)
				{								
				MosaicTwoImagesDialogUpdateLineColumnEnds (
																dialogPtr,
																&leftTopDialogSelectArea,
																&rightBottomDialogSelectArea,
																gMosaicDirectionCode,
																rightOrBottomMosaicWindowInfoHandle,
																itemHit);

				updateLineColumnEndFlag = FALSE;
				
				}	// end "if (updateLineColumnEndFlag)"
							
			}	// end "if (itemHit > 2)" 
			
		else	// itemHit <= 2 
			{
					// If item hit is 1, check if display line-column values make 	
					// sense.  If they don't, sound an alert and make item hit 		
					// equal to 0 to allow user to make changes.							
					
			if (itemHit == 1)
				itemHit = CheckLineColValues (&leftTopDialogSelectArea,
														reformatOptionsPtr->lineStart,
														reformatOptionsPtr->columnStart,
														dialogPtr);
					
			if (itemHit == 1)
				itemHit = CheckLineColValues (&rightBottomDialogSelectArea,
														1,
														1,
														dialogPtr);
										
			if (itemHit == 1)
				{
						// Verify that the number of lines for right/left or number
						// of columns for top/bottom are the same. Present an alert
						// if they are not.
						
				itemHit = MosaicTwoImagesDialogVerifyLineColumnSettings (
																		&leftTopDialogSelectArea,
																		&rightBottomDialogSelectArea,
																		gMosaicDirectionCode);
										
				}	// end "if (itemHit == 1)"
					
			if	(itemHit == 1)      // User selected OK for information 
				{
				modalDone = TRUE;
				returnFlag = TRUE;

				MosaicTwoImagesDialogOK (reformatOptionsPtr,
													outFileInfoPtr,
													gMosaicDirectionCode,
													rightOrBottomMosaicWindowInfoHandle,
													&leftTopDialogSelectArea,
													&rightBottomDialogSelectArea,
													ignoreBackgroundFlag,
													newBackgroundValue,
													(GetDLogControl (dialogPtr, 40) == 1),
													gHeaderOptionsSelection);

				}	// end "if (itemHit == 1)" 
				
			if	(itemHit == 2)      // User selected Cancel for information 
				{
				modalDone = TRUE;
				returnFlag = FALSE;
				
				}	// end "if	(itemHit == 2)" 
				
			}	// end "else itemHit <= 2" 
				
		} while (!modalDone);
		
			// Remove file names from temporary popup menu.								

	for (itemHit=1; itemHit<=gNumberOfIWindows; itemHit++)
		DeleteMenuItem (gPopUpTemporaryMenu, 1);
		
	DisposeUserItemUPP (drawHeaderOptionsPopUpPtr);
	DisposeUserItemUPP (drawMosaicDirectionPopUpPtr);
		
	CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
#endif	// defined multispec_mac


	#if defined multispec_win   	
		CMMosaicTwoImagesDialog*		dialogPtr = NULL;
		
		TRY
			{ 
			dialogPtr = new CMMosaicTwoImagesDialog ();
			
			returnFlag = dialogPtr->DoDialog (fileInfoPtr,
															outFileInfoPtr,
															reformatOptionsPtr); 
		                       
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
		CMMosaicTwoImagesDialog*		dialogPtr = NULL;

		dialogPtr = new CMMosaicTwoImagesDialog (NULL);

		returnFlag = dialogPtr->DoDialog (fileInfoPtr,
													 outFileInfoPtr,
													 reformatOptionsPtr); 
      
		delete dialogPtr;
   #endif 
	
	return (returnFlag);
	
}	// end "MosaicTwoImagesDialog" 



void MosaicTwoImagesDialogInitialize (
				DialogPtr							dialogPtr,
				ReformatOptionsPtr				reformatOptionsPtr,
				FileInfoPtr							leftTopFileInfoPtr,
				FileInfoPtr							outFileInfoPtr,
				SInt16*								mosaicDirectionCodePtr,
				DialogSelectArea*					leftTopDialogSelectAreaPtr,
				DialogSelectArea*					rightBottomDialogSelectAreaPtr,
				SInt16*								fileNamesSelectionPtr,
				Handle*								rightOrBottomWindowInfoHandlePtr,
				Boolean*								ignoreBackgroundFlagPtr,
				SInt32*								maxDataValuePtr,
				SInt32*								newBackgroundValuePtr,
				SInt16*								headerOptionsSelectionPtr)

{	
	#if defined multispec_win
		CComboBox* 							comboBoxPtr;
	#endif	// defined multispec_win

	#if defined multispec_wx
		wxChoice*							listCtrl;
	#endif	// defined multispec_win

	UInt32								windowIndex;

	SInt16								entireIconItem,
											listCount,
											menuTIFFGeoTIFFIndex;
	
	
			// Update the specification dialog with the information given in 		
			// the reformat structure.						
				
			// Initialize left selected area structure.									
			// The line interval will be fixed at 1.										
			// The column interval will be fixed at 1.
			
	entireIconItem = 11;
	if (gAppearanceManagerFlag)
		entireIconItem = 43;
	
	InitializeDialogSelectArea (leftTopDialogSelectAreaPtr,
											gImageWindowInfoPtr,
											gActiveImageWindow,
											reformatOptionsPtr->columnStart,
											reformatOptionsPtr->columnEnd,
											1,
											reformatOptionsPtr->lineStart,
											reformatOptionsPtr->lineEnd,
											1,
											IDC_LineStart,
											entireIconItem,
											kDontAdjustToBaseImage);

	#if defined multispec_mac 
		if (gAppearanceManagerFlag)
			HideDialogItem (dialogPtr, 11);
		else	// !gAppearanceManagerFlag
			HideDialogItem (dialogPtr, 43);
	#endif	// defined multispec_mac  
		
			// Left/Top 'to entire image icon'.													
			//	Left/Top mosaic area																
			
	LoadLineColumnItems (leftTopDialogSelectAreaPtr, 
									dialogPtr, 
									kInitializeLineColumnValues, 
									kIntervalEditBoxesExist,
									1);

			// Initialize right and bottom selected area structure.														
			
	Handle activeImageWindowInfoH = GetActiveImageWindowInfoHandle ();
	*fileNamesSelectionPtr = GetImageList (dialogPtr,
														activeImageWindowInfoH,
														IDC_RightImageFileList,
														TRUE,
														&listCount);

	#if defined multispec_mac 
		windowIndex = kImageWindowStart + *fileNamesSelectionPtr - 1;
	#endif	// defined multispec_mac
	
	#if defined multispec_win  
				// Windows interface requires two separate lists which are associate
				// with each control.
		*fileNamesSelectionPtr = GetImageList (dialogPtr,
															activeImageWindowInfoH,
															IDC_BottomImageFileList,
															TRUE,
															&listCount);

		comboBoxPtr = (CComboBox*)dialogPtr->GetDlgItem (IDC_RightImageFileList);

				// Note that the item data for the list already contains
				// 'kImageWindowStart'.

		windowIndex = (UInt32)comboBoxPtr->GetItemData (*fileNamesSelectionPtr-1);
	#endif	// defined multispec_win

   #if defined multispec_wx
   	*fileNamesSelectionPtr = GetImageList (dialogPtr,
															activeImageWindowInfoH,
															IDC_BottomImageFileList,
															TRUE,
															&listCount);
	
		listCtrl = (wxChoice*)wxWindow::FindWindowById (IDC_RightImageFileList);
		SInt64 windowIndex64 = (SInt64)((int*)listCtrl->
													GetClientData (*fileNamesSelectionPtr - 1));
      windowIndex = (UInt32)windowIndex64;
   #endif
	
	*rightOrBottomWindowInfoHandlePtr =
				(Handle)GetWindowInfoHandle (gWindowList[windowIndex]);
	*mosaicDirectionCodePtr = reformatOptionsPtr->mosaicDirectionCode;
   
											
			// Set up the line - column dialog items for the right image ... the default.
			// Note that the line end will be determined by the start line and	
			// the number of selected lines in the left image.							
			// The line interval will be fixed at 1.										
			// The column interval will be fixed at 1.	
   
	MosaicTwoImagesDialogSetUpRightBottomSelectArea (
												dialogPtr,
												leftTopDialogSelectAreaPtr,
												rightBottomDialogSelectAreaPtr,
												reformatOptionsPtr->mosaicDirectionCode,
												*rightOrBottomWindowInfoHandlePtr);

	MosaicTwoImagesDialogUpdateDirectionItems (dialogPtr, 
															leftTopDialogSelectAreaPtr,
															rightBottomDialogSelectAreaPtr,
															*rightOrBottomWindowInfoHandlePtr,
															reformatOptionsPtr->mosaicDirectionCode,
															reformatOptionsPtr->ignoreBackgroundFlag);
														
			// Left/Top image file name.															

	FileStringPtr fileNamePtr =
						(FileStringPtr)GetFileNamePPointerFromFileInfo (leftTopFileInfoPtr);
	#if defined multispec_mac
		if (CreateUnicodeStaticTextControl (dialogPtr,
														&fileNamePtr[2],
														GetFileStringLength (fileNamePtr),
														IDC_LeftTopImage, NULL) != noErr)
			LoadDItemString (dialogPtr, IDC_LeftTopImage, (Str255*)fileNamePtr);
	#endif

	#if defined multispec_wx || defined multispec_win
		LoadDItemString (dialogPtr, IDC_LeftTopImage, (Str255*)fileNamePtr);
	#endif

			// Left 'to entire image icon'.													
			//	Left reformat area	
			// Will not use column and line intervals for now															
			
	HideDialogItem (dialogPtr, IDC_LineInterval);
	HideDialogItem (dialogPtr, IDC_ColumnInterval);		
	
			// Ignore background value check box.											
 
	*ignoreBackgroundFlagPtr = reformatOptionsPtr->ignoreBackgroundFlag;
	SetDLogControl (dialogPtr,
							IDC_IgnoreBackgroundValue,
							reformatOptionsPtr->ignoreBackgroundFlag);

			// Background data value.													
		
	*maxDataValuePtr = gImageFileInfoPtr->numberBins - 1;
	*newBackgroundValuePtr = reformatOptionsPtr->backgroundValue;																	
	LoadDItemValue (dialogPtr,
							IDC_BackgroundValue,
							(SInt32)reformatOptionsPtr->backgroundValue);
	HideDialogItem (dialogPtr, IDC_BackgroundValue);
		
			// Make sure that all header choices are enabled to start with.

	menuTIFFGeoTIFFIndex = 5;

	#if defined multispec_mac
		InitializeHeaderPopupMenu (dialogPtr, gPopUpHeaderOptionsMenu);
		DisableMenuItem (gPopUpHeaderOptionsMenu, kGAIAMenuItem);
		DisableMenuItem (gPopUpHeaderOptionsMenu, kMatlabMenuItem);

		if (outFileInfoPtr->numberBytes > 2)
			DisableMenuItem (gPopUpHeaderOptionsMenu, kERDAS74MenuItem);
		
		if (outFileInfoPtr->numberBytes > 4)
			DisableMenuItem (gPopUpHeaderOptionsMenu, kArcViewMenuItem);
	#endif	// defined multispec_mac   
                             
	#if defined multispec_win
				// Set the item data values, then remove those menus that
				// will not be used.

		InitializeHeaderPopupMenu (dialogPtr, (MenuHandle)IDC_HeaderFormatList);
		comboBoxPtr = (CComboBox*)(dialogPtr->GetDlgItem (IDC_HeaderFormatList));
		comboBoxPtr->SetItemData (0, kNoneMenuItem);
		comboBoxPtr->SetItemData (1, kArcViewMenuItem);
		comboBoxPtr->SetItemData (2, kERDAS74MenuItem);
		comboBoxPtr->SetItemData (3, kTIFFGeoTIFFMenuItem);

		if (outFileInfoPtr->numberBytes > 2)
			comboBoxPtr->DeleteString (kERDAS74MenuItem);
		
		if (outFileInfoPtr->numberBytes > 4)
			comboBoxPtr->DeleteString (kArcViewMenuItem);

		menuTIFFGeoTIFFIndex = comboBoxPtr->GetCount ();
	#endif	// defined multispec_win 

   #if defined multispec_wx
		wxChoice*		comboPtr;
		comboPtr = (wxChoice*)wxWindow::FindWindowById (IDC_HeaderFormatList);
		menuTIFFGeoTIFFIndex = comboPtr->GetCount ();
     
		if (outFileInfoPtr->numberBytes > 2)
			menuTIFFGeoTIFFIndex -= 1;
		if (outFileInfoPtr->numberBytes > 4)
			menuTIFFGeoTIFFIndex -= 1;
   #endif
													
			// Set text indicating whether the output file format could be GeoTIFF
			// or TIFF
			
	SetTIFF_GeoTIFF_MenuItemString (gImageWindowInfoPtr,
												dialogPtr,
												#if defined multispec_mac  
													gPopUpHeaderOptionsMenu,
												#endif	// defined multispec_mac
												#if defined multispec_win 
													IDC_HeaderFormatList,
												#endif	// defined multispec_win
												#if defined multispec_wx 
													NULL,
												#endif	// defined multispec_wx  
												menuTIFFGeoTIFFIndex);
			
	if (outFileInfoPtr->format == kArcViewType)
		*headerOptionsSelectionPtr = kArcViewMenuItem;
			
	else if (outFileInfoPtr->format == kErdas74Type)
		*headerOptionsSelectionPtr = kERDAS74MenuItem;
			
	else	// ...->format != kArcViewType, kErdas74Type
		*headerOptionsSelectionPtr = kTIFFGeoTIFFMenuItem;
	
	MosaicTwoImagesDialogUpdateChannelDescriptionItem (
							dialogPtr,
							GetHeaderFormatFromPopUpSelection (*headerOptionsSelectionPtr),
							outFileInfoPtr->thematicType);

}	// end "MosaicTwoImagesInitialize"



void MosaicTwoImagesDialogOK (
				ReformatOptionsPtr				reformatOptionsPtr,
				FileInfoPtr							outFileInfoPtr,
				SInt16								mosaicDirectionCode,
				Handle								rightBottomMosaicWindowInfoHandle,
				DialogSelectArea*					leftTopDialogSelectAreaPtr,
				DialogSelectArea*					rightBottomDialogSelectAreaPtr,
				Boolean								ignoreBackgroundFlag,
				SInt32								newBackgroundValue,
				Boolean								channelDescriptionsFlag,
				SInt16								headerOptionsSelection)

{	
	reformatOptionsPtr->rightBottomMosaicWindowInfoHandle =
														rightBottomMosaicWindowInfoHandle;
	/*
	if (mosaicDirectionCode	== kMosaicLeftRight)
		{
				// Force the number of selected lines in the left and right images to 
				// be the same. This covers case when the right image has fewer lines
				// than the left image.
			
		if (leftTopDialogSelectAreaPtr->lineEnd -
						leftTopDialogSelectAreaPtr->lineStart >
										rightBottomDialogSelectAreaPtr->lineEnd -
														rightBottomDialogSelectAreaPtr->lineStart)
			leftTopDialogSelectAreaPtr->lineEnd =
						leftTopDialogSelectAreaPtr->lineStart + 
								rightBottomDialogSelectAreaPtr->lineEnd - 
													rightBottomDialogSelectAreaPtr->lineStart;
													
		}	// end "if (mosaicDirectionCode	== kMosaicLeftRight)"
		
	else	// mosaicDirectionCode == kMosaicTopBottom
		{
				// Force the number of selected columns in the top and bottom images to 
				// be the same. This covers case when the top image has fewer columns than
				// the bottom image.					
			
		if (leftTopDialogSelectAreaPtr->columnEnd -
						leftTopDialogSelectAreaPtr->columnStart >
										rightBottomDialogSelectAreaPtr->columnEnd -
													rightBottomDialogSelectAreaPtr->columnStart)
			leftTopDialogSelectAreaPtr->columnEnd =
						leftTopDialogSelectAreaPtr->columnStart + 
								rightBottomDialogSelectAreaPtr->columnEnd - 
													rightBottomDialogSelectAreaPtr->columnStart;
		
		}	// "else mosaicDirectionCode == kMosaicTopBottom"
	*/
			// Selected area for left part of output file.					
	
	reformatOptionsPtr->lineStart = leftTopDialogSelectAreaPtr->lineStart;
	reformatOptionsPtr->lineEnd = leftTopDialogSelectAreaPtr->lineEnd;
	reformatOptionsPtr->lineInterval = 1;
	reformatOptionsPtr->columnStart = leftTopDialogSelectAreaPtr->columnStart;
	reformatOptionsPtr->columnEnd = leftTopDialogSelectAreaPtr->columnEnd;
	reformatOptionsPtr->columnInterval = 1;
	
			// Selected area for right part of output file.					
	
	reformatOptionsPtr->startLine = rightBottomDialogSelectAreaPtr->lineStart;
	reformatOptionsPtr->stopLine = rightBottomDialogSelectAreaPtr->lineEnd;
	reformatOptionsPtr->startColumn = rightBottomDialogSelectAreaPtr->columnStart;
	reformatOptionsPtr->stopColumn = rightBottomDialogSelectAreaPtr->columnEnd;

	reformatOptionsPtr->mosaicDirectionCode = mosaicDirectionCode;	
	reformatOptionsPtr->ignoreBackgroundFlag = ignoreBackgroundFlag;	
	reformatOptionsPtr->backgroundValue = newBackgroundValue;
	reformatOptionsPtr->channelDescriptions = channelDescriptionsFlag;

			// Get image header type from the popup list.	

	reformatOptionsPtr->headerFormat = 
						GetHeaderFormatFromPopUpSelection (headerOptionsSelection);
					
	if (reformatOptionsPtr->headerFormat == kNoneType)
		outFileInfoPtr->numberHeaderBytes = 0;

			// Set the band interleave depending on the file format.
			// The bandInterleave was set in InitializeOutputFileInformation
			// to be the same as that for the active image window. This takes into
			// account limitations for the selected format.
			
	if (reformatOptionsPtr->headerFormat == kErdas74Type)
		outFileInfoPtr->bandInterleave = kBIL;
			
	else if (reformatOptionsPtr->headerFormat == kTIFFType)
		{
		if (outFileInfoPtr->bandInterleave == kBIL)
			outFileInfoPtr->bandInterleave = kBSQ;
		
		}	// end "else if (reformatOptionsPtr->headerFormat == kTIFFType)"
		
			// Get the file information for the selected channels.  The	
			// information will be loaded into the window information	
			// structure.																
		
	GetFileInformationForChannelList (
							gImageWindowInfoPtr, 
							gImageLayerInfoPtr, 
							gImageFileInfoPtr,
							reformatOptionsPtr->channelPtr, 
							reformatOptionsPtr->numberChannels);

			// Set the flag indicating byte count per data value to force the data to.								
			
	reformatOptionsPtr->forceByteCode = kDoNotForceBytes;
	if (gImageWindowInfoPtr->localMaxNumberBytes == 2 && 
														gImageWindowInfoPtr->localBytesDifferFlag)
		reformatOptionsPtr->forceByteCode = kForce2Bytes;
		
	else if (gImageWindowInfoPtr->localMaxNumberBytes == 4 && 
														gImageWindowInfoPtr->localBytesDifferFlag)
		reformatOptionsPtr->forceByteCode = kForce4Bytes;
		
	else if (gImageWindowInfoPtr->localMaxNumberBytes == 8 && 
														gImageWindowInfoPtr->localBytesDifferFlag)
		reformatOptionsPtr->forceByteCode = kForceReal8Bytes;
	
}	// end "MosaicTwoImagesDialogOK"



void MosaicTwoImagesDialogOnSelectRightBottomImage (
				DialogPtr							dialogPtr,
				DialogSelectArea*					leftTopDialogSelectAreaPtr,
				DialogSelectArea*					rightBottomDialogSelectAreaPtr,
				SInt16								fileNamesSelection,
				Handle*								rightOrBottomMosaicWindowInfoHandlePtr,
				SInt16								mosaicDirectionCode)

{	
	#if defined multispec_win                                
		CComboBox* 							comboBoxPtr;
	#endif	// defined multispec_win
	#if defined multispec_wx
		wxChoice*							listCtrl;
	#endif

	WindowInfoPtr						rightOrBottomMosaicWindowInfoPtr;
	
	Handle								rightOrBottomMosaicWindowInfoHandle;
	UInt32								windowIndex;

	
	if (fileNamesSelection > 0)
		{
		#if defined multispec_mac  
			windowIndex = kImageWindowStart + fileNamesSelection - 1;
		#endif	// defined multispec_mac   

		#if defined multispec_win 
			if (mosaicDirectionCode == kMosaicLeftRight)
				comboBoxPtr = (CComboBox*)dialogPtr->GetDlgItem (IDC_RightImageFileList);
			else	// mosaicDirectionCode == kMosaicTopBottom
				comboBoxPtr = (CComboBox*)dialogPtr->GetDlgItem (IDC_BottomImageFileList);

					// Note that the item data for the list already contains
					// 'kImageWindowStart'.

			windowIndex = (UInt32)comboBoxPtr->GetItemData (fileNamesSelection - 1);
		#endif	// defined multispec_win 

		// Note: To double check the GetClientData in linux
		#if defined multispec_wx			
			listCtrl = (wxChoice*)wxWindow::FindWindowById (IDC_RightImageFileList);
			SInt64 windowIndex64 = (SInt64)((int*)listCtrl->
														GetClientData (fileNamesSelection - 1));
			windowIndex = (UInt32)windowIndex64;
		#endif	// defined multispec_wx     
				
		rightOrBottomMosaicWindowInfoHandle =
												GetWindowInfoHandle (gWindowList[windowIndex]);
		
		rightOrBottomMosaicWindowInfoPtr = (WindowInfoPtr)
										GetHandlePointer (rightOrBottomMosaicWindowInfoHandle);
		
		MosaicTwoImagesDialogSetUpRightBottomSelectArea (
																dialogPtr,
																leftTopDialogSelectAreaPtr,
																rightBottomDialogSelectAreaPtr,
																mosaicDirectionCode,
																rightOrBottomMosaicWindowInfoHandle);

		if (rightOrBottomMosaicWindowInfoHandle != NULL)
			*rightOrBottomMosaicWindowInfoHandlePtr = rightOrBottomMosaicWindowInfoHandle;

		}	// end "if (fileNamesSelection > 0)"
						
}	// end "MosaicTwoImagesDialogOnSelectRightBottomImage"



void MosaicTwoImagesDialogUpdateBackGroundValuesItem (
				DialogPtr							dialogPtr,
				Boolean								ignoreBackgroundFlag)

{	
	if (ignoreBackgroundFlag)
		ShowDialogItem (dialogPtr, IDC_BackgroundValue); 
								
	else	// !GetControlValue (theHandle) 
		HideDialogItem (dialogPtr, IDC_BackgroundValue);

}	// end "MosaicTwoImagesDialogUpdateChannelDescriptionItems"



void MosaicTwoImagesDialogUpdateChannelDescriptionItem (
				DialogPtr							dialogPtr,
				SInt16								headerFormat,
				Boolean								thematicTypeFlag)

{	
	if (gImageWindowInfoPtr->descriptionCode && !thematicTypeFlag)
		{
		//if (headerFormat == kTIFFType)
		//	{
		//	SetDLogControl (dialogPtr, IDC_WriteChannelDescriptions, 0);
		//	SetDLogControlHilite (dialogPtr, IDC_WriteChannelDescriptions, 255);
	 			
		//	}	// end "if (headerFormat == kTIFFType)"
			
		//else	// headerFormat != kTIFFType
		//	{
			SetDLogControl (dialogPtr, IDC_WriteChannelDescriptions, 1);
			SetDLogControlHilite (dialogPtr, IDC_WriteChannelDescriptions, 0);
			
		//	}	// end "else headerFormat != kTIFFType"
	
		}	// end "if (gImageWindowInfoPtr->descriptionCode && ..."
		
	else	// !...->descriptionCode || thematicType
		HideDialogItem (dialogPtr, IDC_WriteChannelDescriptions);

}	// end "MosaicTwoImagesDialogUpdateChannelDescriptionItems"



void MosaicTwoImagesDialogUpdateDirectionItems (
				DialogPtr							dialogPtr,
				DialogSelectArea*					leftTopDialogSelectAreaPtr,
				DialogSelectArea*					rightBottomDialogSelectAreaPtr,
				Handle								rightBottomWindowInfoHandle,
				SInt16								mosaicDirectionCode,
				Boolean								ignoreBackgroundFlag)

{	
	if (mosaicDirectionCode == kMosaicLeftRight)
		{
		LoadDItemString (dialogPtr, IDC_LeftTopPrompt, (Str255*)"\5Left:");
	
		ShowDialogItem (dialogPtr, IDC_RightPrompt);
		ShowDialogItem (dialogPtr, IDC_RightImageFileList);
		ShowDialogItem (dialogPtr, IDC_LinePrompt2);
		ShowDialogItem (dialogPtr, IDC_ColumnPrompt2);
		ShowDialogItem (dialogPtr,	IDC_StartEndTitle2);
		ShowDialogItem (dialogPtr, IDC_LineStart2);
		ShowDialogItem (dialogPtr, IDC_LineEnd2);
		ShowDialogItem (dialogPtr, IDC_ColumnStart2);
		ShowDialogItem (dialogPtr, IDC_ColumnEnd2);
		ShowDialogItem (dialogPtr, IDC_IgnoreBackgroundValue);
		#if defined multispec_mac 
			ShowDialogItem (dialogPtr, IDC_EntireSelectButton2);
		#endif	// defined multispec_mac
		#if defined multispec_win
			ShowDialogItem (dialogPtr, IDC_RightImageSettings);
			ShowDialogItem (dialogPtr, IDSelectedImage2);
			ShowDialogItem (dialogPtr, IDEntireImage2);
		#endif	// defined multispec_win
		#if defined multispec_wx
			ShowDialogItem (dialogPtr, IDC_RightImageSettings);
			ShowDialogItem (dialogPtr, IDC_StartTitleMosaic2);
			ShowDialogItem (dialogPtr, IDC_EndTitleMosaic2);
		#endif

		if (ignoreBackgroundFlag)
			ShowDialogItem (dialogPtr, IDC_BackgroundValue);
		
		HideDialogItem (dialogPtr, IDC_BottomPrompt);
		HideDialogItem (dialogPtr, IDC_BottomImageFileList);
		HideDialogItem (dialogPtr, IDC_LinePrompt3);
		HideDialogItem (dialogPtr, IDC_ColumnPrompt3);
		HideDialogItem (dialogPtr,	IDC_StartEndTitle3);
		HideDialogItem (dialogPtr, IDC_LineStart3);
		HideDialogItem (dialogPtr, IDC_LineEnd3);
		HideDialogItem (dialogPtr, IDC_ColumnStart3);
		HideDialogItem (dialogPtr, IDC_ColumnEnd3);

		#if defined multispec_mac 
			HideDialogItem (dialogPtr, IDC_EntireSelectButton3);
		#endif	// defined multispec_mac
		#if defined multispec_win
			HideDialogItem (dialogPtr, IDC_BottomImageSettings);
			HideDialogItem (dialogPtr, IDSelectedImage3);
			HideDialogItem (dialogPtr, IDEntireImage3);
		#endif	// defined multispec_win
		#if defined multispec_wx
			HideDialogItem (dialogPtr, IDC_StartTitleMosaic3);
			HideDialogItem (dialogPtr, IDC_EndTitleMosaic3);
			HideDialogItem (dialogPtr, IDC_BottomImageSettings);
		#endif  

		}	// end "if (mosaicDirectionCode == kLeftToRight)"
		
	else	// mosaicDirectionCode == kMosaicTopBottom
		{
		LoadDItemString (dialogPtr, IDC_LeftTopPrompt, (Str255*)"\4Top:");
		
		HideDialogItem (dialogPtr, IDC_RightPrompt);
		HideDialogItem (dialogPtr, IDC_RightImageFileList);
		HideDialogItem (dialogPtr, IDC_LinePrompt2);
		HideDialogItem (dialogPtr, IDC_ColumnPrompt2);
		HideDialogItem (dialogPtr,	IDC_StartEndTitle2);
		HideDialogItem (dialogPtr, IDC_LineStart2);
		HideDialogItem (dialogPtr, IDC_LineEnd2);
		HideDialogItem (dialogPtr, IDC_ColumnStart2);
		HideDialogItem (dialogPtr, IDC_ColumnEnd2);
		HideDialogItem (dialogPtr, IDC_IgnoreBackgroundValue);
		HideDialogItem (dialogPtr, IDC_BackgroundValue);												
		#if defined multispec_mac 
			HideDialogItem (dialogPtr, IDC_EntireSelectButton2);
		#endif	// defined multispec_mac
		#if defined multispec_win
			HideDialogItem (dialogPtr, IDC_RightImageSettings);
			HideDialogItem (dialogPtr, IDSelectedImage2);
			HideDialogItem (dialogPtr, IDEntireImage2);
		#endif	// defined multispec_win
		#if defined multispec_wx
			HideDialogItem (dialogPtr, IDC_RightImageSettings);
			HideDialogItem (dialogPtr, IDC_StartTitleMosaic2);
			HideDialogItem (dialogPtr, IDC_EndTitleMosaic2);
			
			ShowDialogItem (dialogPtr, IDC_BottomImageSettings);
			ShowDialogItem (dialogPtr, IDC_StartTitleMosaic3);
			ShowDialogItem (dialogPtr, IDC_EndTitleMosaic3);
		#endif
		
		ShowDialogItem (dialogPtr, IDC_BottomPrompt);
		ShowDialogItem (dialogPtr, IDC_BottomImageFileList);
		ShowDialogItem (dialogPtr, IDC_LinePrompt3);
		ShowDialogItem (dialogPtr, IDC_ColumnPrompt3);
		ShowDialogItem (dialogPtr,	IDC_StartEndTitle3);
		ShowDialogItem (dialogPtr, IDC_LineStart3);
		ShowDialogItem (dialogPtr, IDC_LineEnd3);
		ShowDialogItem (dialogPtr, IDC_ColumnStart3);
		ShowDialogItem (dialogPtr, IDC_ColumnEnd3);
		#if defined multispec_mac  
			ShowDialogItem (dialogPtr, IDC_EntireSelectButton3);
		#endif	// defined multispec_mac
		#if defined multispec_win
			ShowDialogItem (dialogPtr, IDC_BottomImageSettings);
			ShowDialogItem (dialogPtr, IDSelectedImage3);
			ShowDialogItem (dialogPtr, IDEntireImage3);
		#endif	// defined multispec_win 

		}	// end "else mosaicDirectionCode == kTopToBottom"
		
	MosaicTwoImagesDialogSetUpRightBottomSelectArea (
											dialogPtr,
											leftTopDialogSelectAreaPtr,
											rightBottomDialogSelectAreaPtr,
											mosaicDirectionCode,
											rightBottomWindowInfoHandle);

}	// end "MosaicTwoImagesDialogUpdateDirectionItems"



void MosaicTwoImagesDialogUpdateLineColumnEnds (
				DialogPtr							dialogPtr,
				DialogSelectArea*					leftTopDialogSelectAreaPtr,
				DialogSelectArea*					rightBottomDialogSelectAreaPtr,
				SInt16								mosaicDirectionCode,
				Handle								rightOrBottomMosaicWindowInfoHandle,
				UInt16								itemSelected)

{			
	WindowInfoPtr							leftTopWindowInfoPtr,
												rightBottomWindowInfoPtr;


	leftTopWindowInfoPtr = leftTopDialogSelectAreaPtr->imageWindowInfoPtr;
	rightBottomWindowInfoPtr = (WindowInfoPtr)
										GetHandlePointer (rightOrBottomMosaicWindowInfoHandle);

	if (rightBottomWindowInfoPtr != NULL)
		{
		if (mosaicDirectionCode == kMosaicLeftRight)
			{
			if (itemSelected == IDC_LineStart)
				{
				if (rightBottomWindowInfoPtr->maxNumberLines ==
															leftTopWindowInfoPtr->maxNumberLines)
					{
					rightBottomDialogSelectAreaPtr->lineStart =
															leftTopDialogSelectAreaPtr->lineStart;
					LoadDItemValue (dialogPtr,
											IDC_LineStart2,
											rightBottomDialogSelectAreaPtr->lineStart);
					
					}	// end "if (rightBottomWindowInfoPtr->maxNumberColumns == ..."
				
				}	// end "else if (itemSelected == IDC_LineEnd)"
			
			else if (itemSelected == IDC_LineEnd)
				{
				if (rightBottomWindowInfoPtr->maxNumberLines ==
															leftTopWindowInfoPtr->maxNumberLines)
					{
					rightBottomDialogSelectAreaPtr->lineEnd =
																leftTopDialogSelectAreaPtr->lineEnd;
					LoadDItemValue (dialogPtr,
											IDC_LineEnd2,
											rightBottomDialogSelectAreaPtr->lineEnd);
					
					}	// end "if (rightBottomWindowInfoPtr->maxNumberColumns == ..."
									
				}	// end "else if (itemSelected == IDC_LineEnd)"
			
			else if (itemSelected == IDC_LineStart2)
				{
				rightBottomDialogSelectAreaPtr->lineEnd = MIN (
								rightBottomWindowInfoPtr->maxNumberLines,
								rightBottomDialogSelectAreaPtr->lineStart + 
										leftTopDialogSelectAreaPtr->lineEnd - 
											leftTopDialogSelectAreaPtr->lineStart);

				rightBottomDialogSelectAreaPtr->lineEnd =
											MAX (1, rightBottomDialogSelectAreaPtr->lineEnd);
				LoadDItemValue (dialogPtr,
										IDC_LineEnd2,
										rightBottomDialogSelectAreaPtr->lineEnd);
				
				}	// end "else if (itemSelected == IDC_LineStart2)"

			}	// end "if (mosaicDirectionCode == kMosaicLeftRight)"

		else	// mosaicDirectionCode == kMosaicTopBottom
			{
			if (itemSelected == IDC_ColumnStart)
				{
				if (rightBottomWindowInfoPtr->maxNumberColumns ==
															leftTopWindowInfoPtr->maxNumberColumns)
					{
					rightBottomDialogSelectAreaPtr->columnStart =
															leftTopDialogSelectAreaPtr->columnStart;
					LoadDItemValue (dialogPtr,
											IDC_ColumnStart3,
											rightBottomDialogSelectAreaPtr->columnStart);
					
					}	// end "if (rightBottomWindowInfoPtr->maxNumberColumns == ..."
				
				}	// end "else if (itemSelected == IDC_ColumnStart)"
			
			else if (itemSelected == IDC_ColumnEnd)
				{
				if (rightBottomWindowInfoPtr->maxNumberColumns ==
															leftTopWindowInfoPtr->maxNumberColumns)
					{
					rightBottomDialogSelectAreaPtr->columnEnd =
															leftTopDialogSelectAreaPtr->columnEnd;
					LoadDItemValue (dialogPtr,
											IDC_ColumnEnd3,
											rightBottomDialogSelectAreaPtr->columnEnd);
					
					}	// end "if (rightBottomWindowInfoPtr->maxNumberColumns == ..."
									
				}	// end "else if (itemSelected == IDC_ColumnEnd)"
			
			else if (itemSelected == IDC_ColumnStart3)
				{
				rightBottomDialogSelectAreaPtr->columnEnd = MIN (
								rightBottomWindowInfoPtr->maxNumberColumns,
								rightBottomDialogSelectAreaPtr->columnStart +
										leftTopDialogSelectAreaPtr->columnEnd -
														leftTopDialogSelectAreaPtr->columnStart);

				rightBottomDialogSelectAreaPtr->columnEnd =
											MAX (1, rightBottomDialogSelectAreaPtr->columnEnd);
				LoadDItemValue (dialogPtr,
										IDC_ColumnEnd3,
										rightBottomDialogSelectAreaPtr->columnEnd);
				
				}	// end "else if (itemSelected == IDC_ColumnStart3)"

			}	// end "else mosaicDirectionCode == kMosaicLeftRight"

		}	// end "if (rightOrBottomMosaicWindowInfoPtr != NULL)"

}	// end "MosaicTwoImagesDialogUpdateLineColumnEnds"



void MosaicTwoImagesDialogSetUpRightBottomSelectArea (
				DialogPtr							dialogPtr,
				DialogSelectArea*					leftTopDialogSelectAreaPtr,
				DialogSelectArea*					rightBottomDialogSelectAreaPtr,
				SInt16								mosaicDirectionCode,
				Handle								rightBottomWindowInfoHandle)

{		
	WindowInfoPtr						leftTopWindowInfoPtr,
											rightBottomWindowInfoPtr;
	
	UInt32								rightBottomColumnEnd,
											rightBottomColumnStart,
											rightBottomLineEnd,
											rightBottomLineStart;
											
	UInt16								lineColumnItemStart;
	
						
	rightBottomColumnStart = 1;
	rightBottomColumnEnd = 1;
	rightBottomLineStart = 1;
	rightBottomLineEnd = 1;


	leftTopWindowInfoPtr = leftTopDialogSelectAreaPtr->imageWindowInfoPtr;
	rightBottomWindowInfoPtr =
							(WindowInfoPtr)GetHandlePointer (rightBottomWindowInfoHandle);
   

	if (rightBottomWindowInfoPtr != NULL)
		{
		rightBottomColumnEnd = rightBottomWindowInfoPtr->maxNumberColumns;
		rightBottomLineEnd = rightBottomWindowInfoPtr->maxNumberLines;	

		if (mosaicDirectionCode == kMosaicLeftRight)
			{					
					// If the left and right files have the same number of lines, the
					// default start and end lines for the right file will be the same as
					// for the left file. Otherwise the line end will be determined by the
					// start line and	number of selected lines in the left image.
			
			if (rightBottomWindowInfoPtr->maxNumberLines ==
															leftTopWindowInfoPtr->maxNumberLines)
				{
				rightBottomLineStart = leftTopDialogSelectAreaPtr->lineStart;
				rightBottomLineEnd = leftTopDialogSelectAreaPtr->lineEnd;
				
				}	// end "if (rightBottomWindowInfoPtr->maxNumberLines == ..."
				
			else	// rightBottomWindowInfoPtr->maxNumberLines != ...
				{
				rightBottomLineEnd = 1 + leftTopDialogSelectAreaPtr->lineEnd -
															leftTopDialogSelectAreaPtr->lineStart;
				rightBottomLineEnd = MIN (rightBottomLineEnd,
													rightBottomWindowInfoPtr->maxNumberLines);
				
				}	// end "else rightBottomWindowInfoPtr->maxNumberLines != ..."
																
			}	// end "if (mosaicDirectionCode == kMosaicLeftRight)"
			
		else if (mosaicDirectionCode == kMosaicTopBottom)
			{							
					// If the left and right files have the same number of columns, the
					// default start and end columns for the bottom file will be the same
					// as for the top file. Otherwise the column end will be
					// determined by the start column and number of selected columns in
					// the top image.
			
			if (rightBottomWindowInfoPtr->maxNumberColumns ==
															leftTopWindowInfoPtr->maxNumberColumns)
				{
				rightBottomColumnStart = leftTopDialogSelectAreaPtr->columnStart;
				rightBottomColumnEnd = leftTopDialogSelectAreaPtr->columnEnd;
				
				}	// end "if (rightBottomWindowInfoPtr->maxNumberColumns == ..."
				
			else	// rightBottomWindowInfoPtr->maxNumberColumns != ...
				{
				rightBottomColumnEnd = 1 + leftTopDialogSelectAreaPtr->columnEnd -
															leftTopDialogSelectAreaPtr->columnStart;
				rightBottomColumnEnd = MIN (rightBottomColumnEnd,
														rightBottomWindowInfoPtr->maxNumberColumns);
				
				}	// end "else rightBottomWindowInfoPtr->maxNumberLines != ..."
																
			}	// end "else if (mosaicDirectionCode == kMosaicTopBottom)"
			
		}	// end "if (rightBottomWindowInfoPtr != NULL)"		
		
	if (mosaicDirectionCode == kMosaicLeftRight)
		lineColumnItemStart = IDC_LineStart2;

	else 	// mosaicDirectionCode == kMosaicTopBottom
		lineColumnItemStart = IDC_LineStart3;
	
	InitializeDialogSelectArea (
							rightBottomDialogSelectAreaPtr,
							NULL,
							NULL,
							rightBottomColumnStart,
							rightBottomColumnEnd,
							1,
							rightBottomLineStart,
							rightBottomLineEnd,
							1,
							lineColumnItemStart,
							0,
							kDontAdjustToBaseImage);

			//	Load line and column dialog items for right or bottom mosaic area																
	
	LoadLineColumnItems (rightBottomDialogSelectAreaPtr, 
									dialogPtr, 
									kInitializeLineColumnValues, 
									kIntervalEditBoxesDoNotExist,
									mosaicDirectionCode+1);
}	// end "MosaicTwoImagesDialogSetUpRightBottomSelectArea"



SInt16 MosaicTwoImagesDialogVerifyLineColumnSettings (
				DialogSelectArea*					leftTopDialogSelectAreaPtr,
				DialogSelectArea*					rightBottomDialogSelectAreaPtr,
				SInt16								mosaicDirectionCode)

{	
	if (mosaicDirectionCode	== kMosaicLeftRight)
		{
				// The number of lines in each image need to be the same					
			
		if (leftTopDialogSelectAreaPtr->lineEnd -
					leftTopDialogSelectAreaPtr->lineStart !=
									rightBottomDialogSelectAreaPtr->lineEnd -
														rightBottomDialogSelectAreaPtr->lineStart)
			{
			DisplayAlert (kErrorAlertID, 
								kStopAlert, 
								kAlertStrID, 
								IDS_Alert149,
								0,
								NULL);
																			return (0);
																			
			}	// end "if (...->lineEnd - ...->lineStart != ..."
													
		}	// end "if (mosaicDirectionCode	== kMosaicLeftRight)"
		
	else	// mosaicDirectionCode == kMosaicTopBottom
		{
				// The number of columns in each image need to be the same	
			
		if (leftTopDialogSelectAreaPtr->columnEnd -
					leftTopDialogSelectAreaPtr->columnStart !=
									rightBottomDialogSelectAreaPtr->columnEnd -
													rightBottomDialogSelectAreaPtr->columnStart)
			{
			DisplayAlert (kErrorAlertID, 
								kStopAlert, 
								kAlertStrID, 
								IDS_Alert150,
								0,
								NULL);
																			return (0);
																			
			}	// end "if (...->columnEnd - ...->columnStart != ..."
		
		}	// "else mosaicDirectionCode == kMosaicTopBottom"
		
	return (1);

}	// end "MosaicTwoImagesDialogVerifyLineColumnSettings"
