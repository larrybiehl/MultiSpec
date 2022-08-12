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
//	Revision date:			05/14/2022
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
	 #include "WReformatRectifyDialog.h"
#endif	// defined multispec_win 	

#define	kNearestNeighbor				1
#define	kMajorityRule					2
//#define	kMean							3

SInt16							gCompareSelection;

								

			// Prototypes for routines in this file that are only called by		
			// other routines in this file.													
			
#if defined multispec_mac
	pascal void DrawResamplePopUp (
					DialogPtr							dialogPtr,
					SInt16								itemNumber);
#endif

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

Boolean ListCompareImageResultsInformation (
				ReformatOptionsPtr				reformatOptionsPtr,
				FileInfoPtr							outFileInfoPtr);

Boolean CompareImage (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				FileInfoPtr 						outFileInfoPtr, 
				ReformatOptionsPtr 				reformatOptionsPtr);

Boolean CompareImagesDialog (
				FileInfoPtr 						fileInfoPtr,
				FileInfoPtr 						outFileInfoPtr, 
				ReformatOptionsPtr 				reformatOptionsPtr);

void SetUpCompareMethodPopupMenu (
				DialogPtr							dialogPtr,
				MenuHandle							popUpResampleSelectionMenu,
				Boolean								thematicTypeFlag);



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
//	Revised By:			Larry L. Biehl			Date: 05/25/2012	

pascal void DrawResamplePopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.									
			
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpResampleMenu, 
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
//							be reprojected to. If the input flag is set, a list of the
//							possible images will be made.
//
//							The active image window needs to represent a file that contains
//							map description information such that the latitude-longitude of 
//							each pixel can be calculated and the list of re-project to images
//							must contain map information such that the latitude-longitude
//							can be calculated.
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
//	Revised By:			Larry L. Biehl			Date: 05/15/2022

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
	
	WindowInfoPtr						compareWindowInfoPtr;
											
	WindowPtr							windowPtr;
	
	DoubleRect		 					boundingInputRect;
	
	Handle								compareWindowInfoHandle;
	
	SInt16								firstEnabledMenuItem,
											imageListLength,
								 			window,
											windowIndex;
	
	Boolean								includeFlag;
	
	SignedByte							handleStatus2,
											handleStatus3;
	
	
	#if defined multispec_win
		USES_CONVERSION;
	#endif

			// Initialize local variables.													
	
	imageListLength = 0;
	*listCountPtr = 0;
	firstEnabledMenuItem = SHRT_MAX;
	
	if (gNumberOfIWindows >= 2 && DetermineIfLatLongPossible (windowInfoHandle))
		{
		boundingInputRect.top = gImageWindowInfoPtr->boundingLatLongRectangle.top;
		boundingInputRect.left = gImageWindowInfoPtr->boundingLatLongRectangle.left;
		boundingInputRect.bottom = gImageWindowInfoPtr->boundingLatLongRectangle.bottom;
		boundingInputRect.right = gImageWindowInfoPtr->boundingLatLongRectangle.right;
		
		#if defined multispec_wx
			referenceListCtrl =
								(wxChoice*)wxWindow::FindWindowById (IDC_ReferenceFileList);
		#endif

		window = 0;
		windowIndex = kImageWindowStart;

		do
			{
			includeFlag = FALSE;
			
			windowPtr = gWindowList[windowIndex];
			compareWindowInfoHandle = GetWindowInfoHandle (windowPtr);
			
			compareWindowInfoPtr = (WindowInfoPtr)GetHandleStatusAndPointer (
																compareWindowInfoHandle,
																&handleStatus2);
			
			if (compareWindowInfoPtr != NULL &&
						windowInfoHandle != compareWindowInfoHandle && 
									DetermineIfLatLongPossible (compareWindowInfoHandle) &&
											WindowBoundingAreaAndRectIntersect (
																				compareWindowInfoPtr,
																				&boundingInputRect,
																				FALSE))
				includeFlag = TRUE;
				
		
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
//							of the reformat rectify operation.
//
//	Parameters in:		Pointer to reformat instruction structure.
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 03/03/2007
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

Boolean ListCompareImagesResultsInformation (
				ReformatOptionsPtr				reformatOptionsPtr, 
				FileInfoPtr							outFileInfoPtr)

{  
	char									string[16];
	CharPtr								fileNamePtr;
	RectifyImageOptionsPtr			rectifyImageOptionsPtr;
	SInt16								*rectifyChannelPtr;
	
	UInt32								channelIndex,
											index;
											
	SInt16								stringLength;
											
	Boolean								continueFlag;
	
	
	continueFlag = TRUE;
	rectifyImageOptionsPtr = reformatOptionsPtr->rectifyImageOptionsPtr;

	continueFlag = ListLineColumnIntervalString (NULL, 
																gOutputForce1Code,
																reformatOptionsPtr->lineStart, 
																reformatOptionsPtr->lineEnd, 
																reformatOptionsPtr->lineInterval,
																reformatOptionsPtr->columnStart, 
																reformatOptionsPtr->columnEnd, 
																reformatOptionsPtr->columnInterval, 
								 								continueFlag);
	
	if (rectifyImageOptionsPtr->procedureCode == kTranslateScaleRotate)
		{				
		sprintf ((char*)gTextString,
					"%s"
					"    Rectify option: Translate, scale, and/or rotate%s",
					gEndOfLine,
					gEndOfLine);
		
		if (continueFlag)		
			continueFlag = ListString ((char*)gTextString,
												(UInt32)strlen ((char*)gTextString), 
												gOutputTextH);
					
		sprintf ((char*)gTextString,
					"%s"
					#ifndef multispec_wx
						"    Column shift:        %ld%s"
						"    Line shift:          %ld%s"
					#endif
					#if defined multispec_wx				
						"    Column shift:        %d%s"
						"    Line shift:          %d%s"
					#endif
					"    Column scale factor: %g%s"
					"    Line scale factor:   %g%s"
					"    Rotation angle:      %g%s",
					gEndOfLine,
					rectifyImageOptionsPtr->columnShift,
					gEndOfLine,
					rectifyImageOptionsPtr->lineShift,
					gEndOfLine,
					rectifyImageOptionsPtr->columnScaleFactor,
					gEndOfLine,
					rectifyImageOptionsPtr->lineScaleFactor,
					gEndOfLine,
					rectifyImageOptionsPtr->rotationAngle,
					gEndOfLine);
				
		if (continueFlag)
			continueFlag = ListString ((char*)gTextString,
												(UInt32)strlen ((char*)gTextString),
												gOutputTextH);
		
		}	// end "if (...->procedureCode == kTranslateScaleRotate)"
	
	else if (rectifyImageOptionsPtr->procedureCode == kReprojectToReferenceImage)
		{		
		sprintf ((char*)gTextString,
					"%s"
					"    Rectify option: Re-project to reference image%s",
					gEndOfLine,
					gEndOfLine);
				
		if (continueFlag)
			continueFlag = ListString ((char*)gTextString,
												(UInt32)strlen ((char*)gTextString), 
												gOutputTextH);
		
		stringLength = sprintf ((char*)gTextString,
					"      Resample option: ");
					
		if (rectifyImageOptionsPtr->resampleCode == kNearestNeighbor)
			sprintf ((char*)&gTextString[stringLength],
									"Nearest Neighbor%s",
									gEndOfLine);
					
		else if (rectifyImageOptionsPtr->resampleCode == kMajorityRule)
			sprintf ((char*)&gTextString[stringLength],
									"Majority%s",
									gEndOfLine);
				
		if (continueFlag)
			continueFlag = ListString ((char*)gTextString,
												(UInt32)strlen ((char*)gTextString),
												gOutputTextH);
		
		}	// end "else if (...->procedureCode == kReprojectToReferenceImage)"
								 								
	sprintf (string, "No");
	if (rectifyImageOptionsPtr->blankOutsideSelectedAreaFlag)							
		sprintf (string, "Yes");						
		
	sprintf ((char*)gTextString,
				"    Background value:    %g%s"
				"    Set non-selected pixels to background: %s%s%s",
				rectifyImageOptionsPtr->backgroundValue,
				gEndOfLine,
				(char*)string,
				gEndOfLine,
				gEndOfLine);
			
	if (continueFlag)	
		continueFlag = ListString ((char*)gTextString,
											(UInt32)strlen ((char*)gTextString), 
											gOutputTextH);
				
				// List the channels to be rectified if the active image is
				// not a thematic image file
	
	if (continueFlag && !outFileInfoPtr->thematicType)
		{
				//	Need to change the channel list to represent list of channels
				// not 1 or 0 to represent whether the channel is to be used.
		
		rectifyChannelPtr = rectifyImageOptionsPtr->rectifyChannelPtr;
		channelIndex = 0;		
		for (index=0; 
				index<reformatOptionsPtr->numberChannels; 
				index++)
			{
			if (rectifyChannelPtr[index] == 1)
				{
				rectifyChannelPtr[channelIndex] = (SInt16)index;
				channelIndex++;
				
				}	// end "if (rectifyChannelPtr[index] == 1)"
			
			}	// end "for (index=0; ..."									
			
		continueFlag = ListChannelsUsed (
				gImageFileInfoPtr,
				NULL,
				rectifyChannelPtr,
				rectifyImageOptionsPtr->numberChannelsToRectify, 
				NULL,
				&gOutputForce1Code,
				kNoTransformation);
				
		}	// end "if (continueFlag && !outFileInfoPtr->thematicType)"
		
	if (rectifyImageOptionsPtr->procedureCode == kReprojectToReferenceImage)
		{
		continueFlag = ListMapProjectionString (gImageFileInfoPtr->mapProjectionHandle, 
																(char*)"    ",
																continueFlag);	
		
				// Insert a blank line
		
		if (continueFlag)	
			continueFlag = ListString ((char*)&gEndOfLine,
												(UInt32)strlen ((char*)&gEndOfLine),
												gOutputTextH);
										
		}	// end "if (...->procedureCode == kReprojectToReferenceImage)"
										
			// List "  Output Information:"
			
	continueFlag = ListSpecifiedStringNumber (kSharedStrID, 
																IDS_Shared8, 
																(unsigned char*)gTextString, 
																NULL, 
																gOutputForce1Code, 
																continueFlag);
	
	fileNamePtr = (char*)GetFileNameCPointerFromFileInfo (outFileInfoPtr);
	sprintf ((char*)gTextString,
						"    New output image file name: '%s'%s",
						fileNamePtr,
						gEndOfLine);
				
	if (continueFlag)	
		continueFlag = ListString ((char*)gTextString,
											(UInt32)strlen ((char*)gTextString),
											gOutputTextH,
											kUTF8CharString);

	continueFlag = ListLineColumnIntervalString (NULL, 
																gOutputForce1Code,
																1, 
																outFileInfoPtr->numberLines, 
																1,
																1, 
																outFileInfoPtr->numberColumns, 
																1, 
								 								continueFlag);
		
			// Insert a blank line
	
	if (continueFlag)	
		continueFlag = ListString ((char*)&gEndOfLine,
											(UInt32)strlen ((char*)&gEndOfLine), 
											gOutputTextH);
	
	if (continueFlag && !outFileInfoPtr->thematicType)
		{
		continueFlag = ListChannelsUsed (outFileInfoPtr,
													NULL,
													NULL,
													outFileInfoPtr->numberChannels, 
													NULL,
													&gOutputForce1Code,
													kNoTransformation);
		
		}	// end "if (continueFlag && !outFileInfoPtr->thematicType)"
	
	if (rectifyImageOptionsPtr->procedureCode == kReprojectToReferenceImage)
		{		
		continueFlag = ListMapProjectionString (outFileInfoPtr->mapProjectionHandle, 
																(char*)"    ",
																continueFlag);		
		
		}	// end "if (...->procedureCode == kReprojectToReferenceImage)"
		
	return (continueFlag);
	
}	// end "ListCompareImagesResultsInformation"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CompareImage
//
//	Software purpose:	This routine does the rectification of the image.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/06/1992
//	Revised By:			Larry L. Biehl			Date: 02/26/2013

Boolean CompareImage (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				FileInfoPtr							outFileInfoPtr, 
				ReformatOptionsPtr				reformatOptionsPtr)

{
			// Declare local variables & structures
			
	double								backgroundValue;
												
	CMFileStream*						outFileStreamPtr;
	FileInfoPtr							fileInfoPtr;
	TransMapMatrix*					inverseMapMatrixPtr;
	RectifyImageOptionsPtr			rectifyImageOptionsPtr;
	
	unsigned char						*ioOutBufferPtr,
					 						*columnOutBufferPtr,
					 						*savedOutBufferPtr;
					 					
	SInt16								*rectifyChannelPtr;
	
	unsigned char 						*ioOut1ByteBufferPtr,
								 			*ioIn1ByteBufferPtr;
	
	SInt32								column,
											columnByteSkip,
											countInBytes,
											countOutBytes,
											inputColumn,
											inputMapLine,
											lastOutputWrittenLine,
											limitIoOutBytes,
											line,
											lineShift,
											mapColumnShift,
											mapLineShift,
											nonRectifiedColumnOffset,
											numberOutputColumns,
											percentComplete,
											preLineBytes,
											rectifiedInputOffset,
											rectifiedLine,
											rectifiedOutputOffset,
											totalIOOutBytes,
											writePosOff;
	
	UInt32								channelCount,
											columnEnd,
											columnStart,
											inOffsetBytes,
											inOffsetIncrement,
											inputLine,
											lastLineRead,
											numberBytes,
											numberColumnsChannels,
											numberOutputLines,
											outChannelByteIncrement,
											outNumberBytesPerLineAndChannel,
											outOffsetIncrement,
											rectifyBytesToMove,
											startColumn,
											startLine,
											stopColumn,
											stopLine,
											supportFileType;
	
	SInt16								errCode,
											GAIAPrelineString[7] = 
													{	0x0300, 0x0600, 0x0000, 0x0100, 
														0x0000, 0x0000, 0x0000},
											lastPercentComplete,
											numberReadChannels;
												
	UInt16								forceByteCode,
											numberOutChannels;										
	
	Boolean								continueFlag,
											forceBISFlag,
											loadAllColumnsAtOnceFlag,
											shiftOnlyFlag,
											someNoRectifyChannelsFlag;
	
		
			// Initialize local variables.													
			
	errCode = noErr;
	writePosOff = 0;
	fileInfoPtr = fileIOInstructionsPtr->fileInfoPtr;
	forceByteCode = reformatOptionsPtr->forceByteCode;
	forceBISFlag = (outFileInfoPtr->bandInterleave == kBIS);
	preLineBytes = outFileInfoPtr->numberPreLineBytes;
	outFileStreamPtr = GetFileStreamPointer (outFileInfoPtr);
	
	rectifyImageOptionsPtr = reformatOptionsPtr->rectifyImageOptionsPtr;
	inverseMapMatrixPtr = &rectifyImageOptionsPtr->inverseMapMatrix;
	rectifyChannelPtr = rectifyImageOptionsPtr->rectifyChannelPtr;
	backgroundValue = rectifyImageOptionsPtr->backgroundValue;
	
	someNoRectifyChannelsFlag = (reformatOptionsPtr->numberChannels !=
										rectifyImageOptionsPtr->numberChannelsToRectify);
	shiftOnlyFlag = rectifyImageOptionsPtr->shiftOnlyFlag;
	
	lineShift = MAX (0, rectifyImageOptionsPtr->lineShift);
	
	mapColumnShift = MIN (0, rectifyImageOptionsPtr->columnShift);
	mapLineShift = MIN (0, rectifyImageOptionsPtr->lineShift);
					
	ioOutBufferPtr = (unsigned char*)reformatOptionsPtr->ioOutBufferPtr;
				
	lastPercentComplete = -1;
	
	columnStart = reformatOptionsPtr->columnStart;
	columnEnd =  reformatOptionsPtr->columnEnd;
	
	if (rectifyImageOptionsPtr->blankOutsideSelectedAreaFlag)
		{
		startLine = reformatOptionsPtr->startLine;
		stopLine = reformatOptionsPtr->stopLine;
		startColumn = reformatOptionsPtr->startColumn;
		stopColumn = reformatOptionsPtr->stopColumn;
		
		rectifiedInputOffset = MIN (
						(SInt32)(startColumn-1), rectifyImageOptionsPtr->columnShift);
		rectifiedInputOffset = MAX (0, rectifiedInputOffset);
		rectifiedOutputOffset = MAX (0, rectifyImageOptionsPtr->columnShift);
		
		}	// end "if (...->blankOutsideSelectedAreaFlag)" 
		
	else	// !...->blankOutsideSelectedAreaFlag 
		{
		startLine = reformatOptionsPtr->lineStart;
		stopLine = reformatOptionsPtr->lineEnd;
		startColumn = columnStart;
		stopColumn = columnEnd;
		
		rectifiedInputOffset = 0;
		rectifiedOutputOffset = 0;
		
		}	// end "else !...->blankOutsideSelectedAreaFlag"
		
	numberBytes = outFileInfoPtr->numberBytes;
	
	nonRectifiedColumnOffset = rectifyImageOptionsPtr->nonRectifiedInputOffset;
	nonRectifiedColumnOffset *= numberBytes;
	
	rectifiedInputOffset *= numberBytes;
	rectifiedOutputOffset *= numberBytes;
															
	numberOutChannels = reformatOptionsPtr->numberChannels;
	numberOutputColumns = outFileInfoPtr->numberColumns;
	numberOutputLines = outFileInfoPtr->numberLines;
	
	numberReadChannels = reformatOptionsPtr->numberChannels;
		
			// Determine if the section of code which handles all columns			
			// for a given channel at once needs to be called.							
			
	loadAllColumnsAtOnceFlag = 
					!forceBISFlag && (someNoRectifyChannelsFlag || shiftOnlyFlag);
	
			// Write a header if requested.													
			
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
		limitIoOutBytes = reformatOptionsPtr->ioOutBufferBytes - countOutBytes;
		totalIOOutBytes = 0;
		
				// Initialize a background line that will be copied from as			
				// needed.	
								
		InitializeLineToBackgroundValue (outFileInfoPtr, 
													(HUInt8Ptr)ioOutBufferPtr,
													countOutBytes,
													backgroundValue);	
		
				// Set up output working buffer pointer.  This pointer will			
				// change as the output buffer fills up									
		
		savedOutBufferPtr = &ioOutBufferPtr[countOutBytes];
		
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
										
		outChannelByteIncrement = (countOutBytes-preLineBytes)/numberOutChannels;
		
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
		
		outNumberBytesPerLineAndChannel =
												(countOutBytes-preLineBytes)/numberOutChannels;
		if (outFileInfoPtr->bandInterleave == kBIS)
			outNumberBytesPerLineAndChannel /= numberOutputColumns;
	
		rectifyBytesToMove = (stopColumn - startColumn + 1) * numberBytes;
		rectifyBytesToMove = MIN (outNumberBytesPerLineAndChannel, rectifyBytesToMove);
					
				// Intialize the nextTime variable to indicate when the next 		
				// check should occur for a command-.										
				
		gNextTime = TickCount ();
		
				// Get the total number of samples a line of data.						
				// It will be used when adjusting the data if request.				
				
		numberColumnsChannels = 
								((SInt32)columnEnd - columnStart + 1) * numberReadChannels;
			
				// Load some of the File IO Instructions structure that pertain
				// to the specific area being used.
				
		errCode = SetUpFileIOInstructions (fileIOInstructionsPtr,
														NULL,
														startLine,
														stopLine,
														1,
														columnStart,
														columnEnd,
														1,
														numberReadChannels,
														NULL,
														kDetermineSpecialBILFlag);		
												
		if (errCode != noErr)	
			continueFlag = FALSE;			
		
		line = 1;
		lastLineRead = -1;
		inputLine = reformatOptionsPtr->startLine;
		lastOutputWrittenLine = 0;

				// Turn spin cursor on

		gPresentCursor = kSpin;
		
		while (line <= (SInt32)numberOutputLines && continueFlag)
			{
			ioOut1ByteBufferPtr = savedOutBufferPtr;
			
					// Initialize output line to the background value.	
					
			if (outFileInfoPtr->bandInterleave == kBSQ)
				{
				channelCount = 0;
				while (channelCount<numberOutChannels)
					{	
					BlockMoveData (ioOutBufferPtr, 
										&ioOut1ByteBufferPtr[preLineBytes], 
										outNumberBytesPerLineAndChannel);
										
					ioOut1ByteBufferPtr += outChannelByteIncrement;
					channelCount++;
										
					}	// end "while (channelCount<numberOutChannels)"
					
				ioOut1ByteBufferPtr = savedOutBufferPtr;
					
				}	// end "if (outFileInfoPtr->bandInterleave == kBSQ)"
			
			else	// ...->bandInterleave != kBSQ	
				BlockMoveData (ioOutBufferPtr, 
									&ioOut1ByteBufferPtr[preLineBytes], 
									countOutBytes-preLineBytes);
		
					// Add the preline calibration bytes if any.  For now this is	
					// only handled for GAIA data.											
					
			if (preLineBytes > 0)
				{
				BlockMoveData (&GAIAPrelineString, ioOut1ByteBufferPtr, preLineBytes);
				ioOut1ByteBufferPtr += preLineBytes;
				
				}	// end "if (preLineBytes > 0)" 
				
			if (loadAllColumnsAtOnceFlag)
				{
						// For output files which are in BIL and BSQ formats,			
						// load all columns of data for those channels that do not	
						// need to be rectified.												
						
				ioIn1ByteBufferPtr = (unsigned char*)gOutputBufferPtr;
				ioOut1ByteBufferPtr = &savedOutBufferPtr[preLineBytes];
				channelCount = 0;
						
				if (inputLine >= startLine && inputLine <= stopLine)
					{ 
					if (inputLine != lastLineRead)
						{
								// Get all requested channels for line of image data. 
								// Return if there is a file IO error.						
					 
						errCode = GetLineOfData (fileIOInstructionsPtr,
															inputLine, 
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
																					
						lastLineRead = inputLine;
																					
						}	// end "if (inputLine != lastLineRead)" 
				
					while (channelCount<numberOutChannels)
						{
						if (!rectifyChannelPtr[channelCount])
							{
							if (fileInfoPtr->bandInterleave != kBIS)
								BlockMoveData (&ioIn1ByteBufferPtr[nonRectifiedColumnOffset], 
													ioOut1ByteBufferPtr, 
													countInBytes);
													
							else	// fileInfoPtr->bandInterleave == kBIS
								{
								ioIn1ByteBufferPtr = (unsigned char*)gOutputBufferPtr;
								ioIn1ByteBufferPtr =
												&ioIn1ByteBufferPtr[channelCount*numberBytes];
									
								for (column=0; column<countInBytes; column+=numberBytes)
									{
									memcpy (
										ioOut1ByteBufferPtr, ioIn1ByteBufferPtr, numberBytes);
									ioIn1ByteBufferPtr += numberReadChannels;
									ioOut1ByteBufferPtr += numberBytes;
								
									}	// end "for (column=0; column<=..."	
											
								ioOut1ByteBufferPtr -= countInBytes;
								
								}	// end "else fileInfoPtr->bandInterleave == kBIS"
								
							}	// end "if (!rectifyChannelPtr[channelCount])"
						
						channelCount++;
						
						 		// Not sure about this. It is change above for 1-byte data but
						 		// not for 2-byte data.  Is it really working correctly.
						 	
						ioIn1ByteBufferPtr += inOffsetBytes; 
						ioOut1ByteBufferPtr += outChannelByteIncrement;
					
						}	// end "while (channelCount<numberOutChannels)" 
				
					}	// end "if (inputLine >= startLine && ..." 
					
				if (continueFlag && shiftOnlyFlag)
					{
							// Load all columns of data for those channels for 		
							// which rectification is only a line and/or column 		
							// shift.																
							
					ioIn1ByteBufferPtr = (unsigned char*)gOutputBufferPtr;
					ioOut1ByteBufferPtr = &savedOutBufferPtr[preLineBytes];
					channelCount = 0;
					
					rectifiedLine = inputLine - lineShift;
					if (rectifiedLine >= (SInt32)startLine && 
															rectifiedLine <= (SInt32)stopLine)
						{
								// Line is within input image file range.					
								
						if (rectifiedLine != (SInt32)lastLineRead)
							{
									// Get all requested channels for line of image 	
									// data.  Return if there is a file IO error.		
					 
							errCode = GetLineOfData (fileIOInstructionsPtr,
																rectifiedLine,
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
																					
							lastLineRead = rectifiedLine;
																					
							}	// end "if (rectifiedLine != lastLineRead)" 
							
						while (channelCount<numberOutChannels)
							{
							if (rectifyChannelPtr[channelCount])
								{
								if (fileInfoPtr->bandInterleave != kBIS)
									BlockMoveData (&ioIn1ByteBufferPtr[rectifiedInputOffset], 
													&ioOut1ByteBufferPtr[rectifiedOutputOffset], 
													rectifyBytesToMove);
													
								else	// fileInfoPtr->bandInterleave == kBIS
									{
									ioIn1ByteBufferPtr = (unsigned char*)gOutputBufferPtr;
									ioIn1ByteBufferPtr =
													&ioIn1ByteBufferPtr[channelCount*numberBytes];
										
									for (column=0; column<countInBytes; column+=numberBytes)
										{
										memcpy (
											ioOut1ByteBufferPtr, ioIn1ByteBufferPtr, numberBytes);
										ioIn1ByteBufferPtr += numberReadChannels;
										ioOut1ByteBufferPtr += numberBytes;
									
										}	// end "for (column=0; column<=..."	
										
									ioOut1ByteBufferPtr -= countInBytes;
									
									}	// end "else fileInfoPtr->bandInterleave == kBIS"
											
								}	// end "if (rectifyChannelPtr[channelCount])" 
							
							channelCount++;
							ioIn1ByteBufferPtr += inOffsetBytes;
							ioOut1ByteBufferPtr += outChannelByteIncrement;
						
							}	// end "while (channelCount<numberOutChannels)" 
							
						}	// end "if (rectifiedLine >= startLine && ..."
						
					}	// end "if (continueFlag && shiftOnlyFlag)" 
			
						// Check if user wants to abort processing.							
						
				if (TickCount () >= gNextTime)
					{
					if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
						continueFlag = FALSE; 
							
					}	// end "if (TickCount () >= nextTime)" 
					
				}	// end "if (loadAllColumnsAtOnceFlag)" 
				
			if (continueFlag && (!shiftOnlyFlag || forceBISFlag))
				{
				columnOutBufferPtr = &savedOutBufferPtr[preLineBytes];
										
				for (column=1; column<=numberOutputColumns; column++)
					{
					
					if (inputMapLine >= (SInt32)startLine && 
								inputColumn >= (SInt32)startColumn &&
									inputMapLine <= (SInt32)stopLine && 
											inputColumn <= (SInt32)stopColumn)
						{
						if (inputMapLine != (SInt32)lastLineRead)
							{
									// Get all requested channels for line of 			
									// image data. Return if there is a file IO error.	
						 
							errCode = GetLineOfData (fileIOInstructionsPtr,
																inputMapLine, 
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
																						
							lastLineRead = inputMapLine;
																						
							}	// end "if (inputMapLine != lastLineRead)" 
							
						channelCount = 0;
						inputColumn -= columnStart;
						
								// Adjust column index to allow for band interleave.
								
						if (forceBISFlag || fileInfoPtr->bandInterleave == kBIS)
							inputColumn *= numberReadChannels;
							
								// Set input buffer pointer.												
				
						ioIn1ByteBufferPtr = (unsigned char*)
														&gOutputBufferPtr[inputColumn*numberBytes];
						
							// Set output buffer pointers.								
		
						ioOut1ByteBufferPtr = columnOutBufferPtr;
					
						while (channelCount<numberOutChannels)
							{
							if (rectifyChannelPtr[channelCount])
								memcpy (ioOut1ByteBufferPtr, ioIn1ByteBufferPtr, numberBytes);
						
							channelCount++;
						
							ioIn1ByteBufferPtr += inOffsetBytes;
							ioOut1ByteBufferPtr += outChannelByteIncrement;
					
							}	// end "while (channelCount<..." 
							
						}	// end "else inputMapLine && inputColumn ... 
						
					columnOutBufferPtr += columnByteSkip;
			
							// Check if user wants to abort processing.							
							
					if (TickCount () >= gNextTime)
						{
						if (!CheckSomeEvents (
												osMask+keyDownMask+updateMask+mDownMask+mUpMask))
							continueFlag = FALSE;
							
						}	// end "if (TickCount () >= nextTime)" 
						
					}	// end "for (column=1; column<=numberOutputColumns; ..." 
					
				}	// end "if (... && (!shiftOnlyFlag || forceBISFlag))" 
			
			if (outFileInfoPtr->bandInterleave == kBSQ)
				savedOutBufferPtr = &savedOutBufferPtr[outNumberBytesPerLineAndChannel];
			
			else	// outFileInfoPtr->bandInterleave != kBSQ
				savedOutBufferPtr = &savedOutBufferPtr[countOutBytes];
	
					// Write line(s), channel(s) of data when needed.
			
			totalIOOutBytes += countOutBytes;
			
			if (continueFlag && (totalIOOutBytes > limitIoOutBytes))
				{
				errCode = WriteOutputDataToFile (outFileInfoPtr,
															outFileStreamPtr,
															&ioOutBufferPtr [countOutBytes],
															reformatOptionsPtr->channelPtr,
															numberOutChannels,
															lastOutputWrittenLine,
															outNumberBytesPerLineAndChannel,
															numberOutputLines,
															outChannelByteIncrement,
															totalIOOutBytes,
															reformatOptionsPtr,
															1);
				
				totalIOOutBytes = 0;
				savedOutBufferPtr = (unsigned char*)&ioOutBufferPtr[countOutBytes];
				
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
			
					// Adjust line.																
					
			line++;
			inputLine++;
				
			}	// end "while (line < numberOutputLines && continueFlag)" 
			
				// Flush output buffer if needed.											
		
		if (continueFlag && totalIOOutBytes > 0)
			{				
			errCode = WriteOutputDataToFile (outFileInfoPtr,
														outFileStreamPtr,
														&ioOutBufferPtr [countOutBytes],
														reformatOptionsPtr->channelPtr,
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
			
			}	// end "if (continueFlag && (totalIOOutBytes > 0))" 
		
				// Close up any File IO Instructions structure that pertain to the 
				// specific area used for rectification.
				
		CloseUpFileIOInstructions (fileIOInstructionsPtr, NULL);

				// Turn spin cursor off
	
		gPresentCursor = kWait;
		
				// Write channel discriptions at the end of the file if needed.	
		
		if (continueFlag && reformatOptionsPtr->channelDescriptions)	
			WriteChannelDescriptionsAndValues (
									outFileInfoPtr, 
									NULL, 
									outFileInfoPtr->numberChannels,
									(reformatOptionsPtr->outputFileCode == kAppendToMenuItem));

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
		
	return (!continueFlag);
			
}	// end "CompareImage"


/*
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
//	Revised By:			Larry L. Biehl			Date: 05/22/2022

void CompareImagesControl (void)

{
			// Declare local variables & structures
			
	FileIOInstructionsPtr			fileIOInstructionsPtr;
	
	Handle								outFileInfoHandle,
											reformatOptionsH;
	
	LongRect								inputRectangle,
											outputRectangle;
	
	FileInfoPtr							outFileInfoPtr;	
	RefCompareImagesOptionsPtr		compareImagesOptionsPtr;
	ReformatOptionsPtr				reformatOptionsPtr;
	
	time_t								startTime;
	
	SInt32								index,
											savedOutputRectangleStartColumn,
											savedOutputRectangleStartLine;				
	
	UInt32								numberBytes;					
	
	Boolean								continueFlag,
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
	compareImagesOptionsPtr = NULL;
	outFileInfoHandle = NULL;
	reformatOptionsH = NULL;
	fileIOInstructionsPtr = NULL;
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
			
		reformatOptionsPtr->signedOutputDataFlag = gImageFileInfoPtr->signedDataFlag;
			
		outFileInfoHandle = NULL;
		if (!gImageFileInfoPtr->thematicType && gImageWindowInfoPtr->descriptionCode)
			reformatOptionsPtr->channelDescriptions = TRUE;
		reformatOptionsPtr->numberChannels = gImageWindowInfoPtr->totalNumberChannels;
													
		}	// end "if (continueFlag)" 
			
			// Get a block of memory to use for the reformatting rectification options.
			
	if (continueFlag)
		{		
		compareImagesOptionsPtr =
						(RefCompareImagesOptionsPtr)MNewPointer (sizeof (RefCompareImagesOptions));
		if (compareImagesOptionsPtr)
			{
			compareImagesOptionsPtr->compareImagesChannelPtr = NULL;
			compareImagesOptionsPtr->numberChannelsToCompare =
														gImageWindowInfoPtr->totalNumberChannels;
	
					// Get handle to window information for reference image
			
			compareImagesOptionsPtr->referenceWindowInfoHandle = NULL;
			
			reformatOptionsPtr->compareImagesOptionsPtr = compareImagesOptionsPtr;
			
			}	// end "if (rectifyImageOptionsPtr)" 
			
		else	// !rectifyImageOptionsPtr 
			continueFlag = FALSE;
			
		}	// end "if (continueFlag)" 
					
			// Set up memory for rectification channels vector.						
	
	if (continueFlag)
		{
		numberBytes = (SInt32)gImageWindowInfoPtr->totalNumberChannels * 
																						sizeof (SInt16);
																				
		Boolean tempFlag;
		continueFlag = CheckPointerSize (
											(Ptr*)&compareImagesOptionsPtr->compareImagesChannelPtr,
											numberBytes, 
											&tempFlag);
		
		}	// end "if (continueFlag)" 
	
			// If memory is okay to this point then initialize the vectors.		
			
	if (continueFlag)
		{
				// Fill in vector of all rectification channels.						
		
		for (index=0; index<compareImagesOptionsPtr->numberChannelsToCompare; index++)
         compareImagesOptionsPtr->compareImagesChannelPtr[index] = (SInt16)index;
			
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
													NULL);
				
						// Get pointer to memory to use to read an image file line
						// into.
						
				UInt16		forceOutputFormatCode = kDoNotForceFormat;
				if (outFileInfoPtr->bandInterleave == kBIS)
					forceOutputFormatCode = kBIS;
			 						
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
										 						NULL, 
																kPackData, 
																forceOutputFormatCode,	// outFileInfoPtr->bandInterleave == kBIS,
																reformatOptionsPtr->forceByteCode,
																kDoNotAllowForThreadedIO,
																&fileIOInstructionsPtr);
										
				}	// end "if (continueFlag)" 
	
					// Get buffer for output data.											
					
			if (continueFlag)
				continueFlag = 
							GetReformatOutputBuffer (outFileInfoPtr, reformatOptionsPtr);
		
				// Branch to end of routine if memory is full and do cleanup of handles.
				
			if (continueFlag)
				{		
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
					MGetString (gTextString, kReformatStrID, IDS_PercentComplete);
					LoadDItemString (
								gStatusDialogPtr, IDC_ShortStatusText, (Str255*)gTextString);
					LoadDItemValue (gStatusDialogPtr, IDC_ShortStatusValue, (SInt32)0);
					ShowStatusDialogWindow (kShortStatusInfoID);
					
							// Everything has now been set up.								
							// Do reformatting for BIL output files here.				
							
					startTime = time (NULL);
					
					if (compareImagesOptionsPtr->procedureCode == kTranslateScaleRotate)									
						returnFlag = CompareImage (
								fileIOInstructionsPtr, outFileInfoPtr, reformatOptionsPtr);
					
							// List Reformat information.										
							
					continueFlag = ListCompareImagesResultsInformation (
																reformatOptionsPtr, outFileInfoPtr);
				
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
				
			}	// end "if (ChangeImageFormatDialog (gImageFileInfoPtr, ... " 
						
		}	// end "if (continueFlag)" 
		
	ReleaseReformatOutputFileInfoAndBuffers (reformatOptionsPtr, gImageFileInfoPtr);				
			
	//ReleaseReformatSpecsMemory (&reformatOptionsH, gImageFileInfoPtr);
					
}	// end "CompareImagesControl"
*/


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		CompareImagesDialog
//
//	Software purpose:	This routine handle the rectify image dialog box.
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
//	Revised By:			Larry L. Biehl			Date: 05/14/2022

Boolean CompareImagesDialog (
				FileInfoPtr							fileInfoPtr, 
				FileInfoPtr							outFileInfoPtr, 
				ReformatOptionsPtr				reformatOptionsPtr)

{
	Boolean								OKFlag;
										
#if defined multispec_mac
	DialogSelectArea					dialogSelectArea;
	
	Rect									headerBox,
											resampleBox,
											theBox;
	
	DialogPtr							dialogPtr;
	RectifyImageOptionsPtr			rectifyImageOptionsPtr;
	
	SInt16								*rectifyChannelPtr;
									
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
											procedureCode,
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
	
	RectifyImageDialogInitialize (dialogPtr,
												fileInfoPtr,
												&dialogSelectArea,
												reformatOptionsPtr,
												&gHeaderOptionsSelection,
												&gChannelSelection,
												&blankOutsideSelectedAreaFlag,
												&lastBackgroundValue,
												&procedureCode,
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
	
	rectifyImageOptionsPtr = reformatOptionsPtr->rectifyImageOptionsPtr;
	
	rectifyChannelPtr = rectifyImageOptionsPtr->rectifyChannelPtr;

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
							(procedureCode == kTranslateScaleRotate));	
							
	SetDLogControl (dialogPtr, 
							IDC_ReprojectToRadio, 
							(procedureCode == kReprojectToReferenceImage));
			
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
	//DisableMenuItem (gPopUpResampleMenu, kMean);
		
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
					procedureCode = kTranslateScaleRotate;
					
					RectifyImageDialogOnReferenceFile (dialogPtr,
																	procedureCode,
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
					
				case 35:				// Rectify to reference image 
					SetDLogControl (dialogPtr, IDC_TranslateScaleRotateRadio, 0);
					SetDLogControl (dialogPtr, IDC_ReprojectToRadio, 1);
					procedureCode = kReprojectToReferenceImage;
					
					RectifyImageDialogOnReferenceFile (dialogPtr,
																	procedureCode,
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
																		procedureCode,
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
															gPopUpResampleMenu, 
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
				
				RectifyImageDialogOK (dialogPtr,
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
												procedureCode,
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
		CMReformatRectifyDlg*		dialogPtr = NULL; 
		
		TRY
			{ 
			dialogPtr = new CMReformatRectifyDlg (); 
			                                                                     
			OKFlag = dialogPtr->DoDialog (outFileInfoPtr,
													fileInfoPtr,
													gImageWindowInfoPtr,
													gImageLayerInfoPtr,
													reformatOptionsPtr,
													minBackgroundValue,
													maxBackgroundValue); 
			
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
//	Software purpose:	This routine handle the rectify image dialog box.
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
//	Revised By:			Larry L. Biehl			Date: 05/22/2022

void CompareImagesDialogInitialize (
				DialogPtr							dialogPtr,
				FileInfoPtr							fileInfoPtr,
				DialogSelectArea*					dialogSelectAreaPtr,
				ReformatOptionsPtr				reformatOptionsPtr,
				SInt16*								headerOptionsSelectionPtr,
				SInt16*								channelSelectionPtr,
				SInt16*								procedureCodePtr,
				SInt16*								resampleCodePtr,
				SInt16*								fileNamesSelectionPtr,
				Handle*								referenceWindowInfoHandlePtr)

{
	RectifyImageOptionsPtr			rectifyImageOptionsPtr;
	SInt16								*rectifyChannelPtr;
	
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
	
	rectifyImageOptionsPtr = reformatOptionsPtr->rectifyImageOptionsPtr;
	rectifyChannelPtr = rectifyImageOptionsPtr->rectifyChannelPtr;

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
								kIntervalEditBoxesExist,
								1);
	HideDialogItem (dialogPtr, IDC_LineInterval);
	HideDialogItem (dialogPtr, IDC_ColumnInterval);
	
			// Adjust header format from the default if needed.
			
	*headerOptionsSelectionPtr = kERDAS74MenuItem;
	if (reformatOptionsPtr->headerFormat == kTIFFType)
		*headerOptionsSelectionPtr = kTIFFGeoTIFFMenuItem;
	else if (fileInfoPtr->format == kGAIAType || fileInfoPtr->format == kGAIA2Type)
		*headerOptionsSelectionPtr = kGAIAMenuItem;
	
			// Get the procedure to be used.
	
	*procedureCodePtr = rectifyImageOptionsPtr->procedureCode;

			//	Set the draw routine for the rectification channel popup box.		
			
	if (fileInfoPtr->thematicType)
		{
		HideDialogItem (dialogPtr, IDC_ChannelsPrompt);
		HideDialogItem (dialogPtr, IDC_Channels);
		
		}	// end "if (fileInfoPtr->thematicType)" 
						
	*channelSelectionPtr = kAllMenuItem;
	
			// Check if any images can be used for reference images.
			
	*fileNamesSelectionPtr = GetCompareToImageList (dialogPtr,
																		gActiveImageWindowInfoH, 
																		TRUE, 
																		referenceWindowInfoHandlePtr,
																		&listCount);
																		
	if (listCount <= 0)
		{
		*procedureCodePtr = kTranslateScaleRotate;
		HideDialogItem (dialogPtr, IDC_ReprojectToRadio);
			
				// The initial reference image for now is the active image for use with
				// translate, scale and rotate.
		
		*referenceWindowInfoHandlePtr = gActiveImageWindowInfoH;
		
		}	// end "if (listCount <= 0)"
		
	else	// listCount > 0
		{
		*procedureCodePtr = kReprojectToReferenceImage;
		ShowDialogItem (dialogPtr, IDC_ReprojectToRadio);
		
		}	// else "listCount > 0"
		
			// Set sampling procedure
			
	*resampleCodePtr = kNearestNeighbor;
	SetUpCompareMethodPopupMenu (dialogPtr,
											gPopUpResampleMenu,
											fileInfoPtr->thematicType);
					
	//ompareImagesDialogOnRectifyCode (dialogPtr,
	//											*procedureCodePtr);
	
}	// end "CompareImagesDialogInitialize"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CompareImagesDialogOK
//
//	Software purpose:	This routine handle the rectify image dialog box.
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
//	Revised By:			Larry L. Biehl			Date: 05/22/2022

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
				SInt16								procedureCode,
				Handle								referenceWindowInfoHandle)

{		
	RefCompareImagesOptionsPtr	      compareImagesOptionsPtr;
	SInt16									*compareImagesChannelPtr;
	
	SInt32									index,
												rectifyIndex;
											
	
	compareImagesOptionsPtr = reformatOptionsPtr->compareImagesOptionsPtr;
	compareImagesChannelPtr = compareImagesOptionsPtr->compareImagesChannelPtr;
	
			// Rectify procedure code.
											
	compareImagesOptionsPtr->procedureCode = procedureCode;
	compareImagesOptionsPtr->referenceWindowInfoHandle = referenceWindowInfoHandle;
	
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
						
	if (reformatOptionsPtr->headerFormat == kArcViewType)
		outFileInfoPtr->bandInterleave = fileInfoPtr->bandInterleave;
			
	else if (reformatOptionsPtr->headerFormat == kErdas74Type)
		outFileInfoPtr->bandInterleave = kBIL;
			
	else if (reformatOptionsPtr->headerFormat == kTIFFType)
		outFileInfoPtr->bandInterleave = kBSQ;

			// Item 33; Channels to be rectified before copying to 		
			// output file.															
			// Make the rectifyChannel Vector a vector of 0's or 1's		
			// to indicate which channel is to be rectified.				
			
	if (channelSelection == kAllMenuItem)
		{
		compareImagesOptionsPtr->numberChannelsToCompare =
																reformatOptionsPtr->numberChannels;
		for (index=0; 
				index<compareImagesOptionsPtr->numberChannelsToCompare;
					index++)
			compareImagesChannelPtr[index] = 1;
			
		}	// end "if (channelSelection == kAllMenuItem)" 
				
	if (channelSelection == kSubsetMenuItem)
		{
				// Now set the rectify channels vector to be 0 for 		
				// channel not to be rectified and 1 for channel to be 	
				// rectified.															
		
		rectifyIndex = compareImagesOptionsPtr->numberChannelsToCompare - 1;
		for (index = reformatOptionsPtr->numberChannels-1;
				index >= 0; 
					index--)
			{
			if (compareImagesChannelPtr[rectifyIndex] == index)
				{
				compareImagesChannelPtr[index] = 1;
				
				if (rectifyIndex > 0)
					rectifyIndex--;
				
				}	// end "if (rectifyChannelPtr... == index)" 
				
			else	// rectifyChannelPtr[rectifyIndex] != index 
				compareImagesChannelPtr[index] = 0;
				
			}	// end "for (index=...->numberChannels-1; ..." 
			
		}	// end "if (channelSelection == kSubsetMenuItem)" 
		
			// Get the file information for the selected channels.  The	
			// information will be loaded into the window information	
			// structure.																
		
	GetFileInformationForChannelList (windowInfoPtr,
													layerInfoPtr, 
													fileInfoPtr,
													NULL, 
													(SInt16)reformatOptionsPtr->numberChannels);
	
			// Set the flag indicating byte count per data value to force the data to.								
			
	reformatOptionsPtr->forceByteCode = kDoNotForceBytes;
	if (windowInfoPtr->localMaxNumberBytes == 2 && 
														windowInfoPtr->localBytesDifferFlag)
		reformatOptionsPtr->forceByteCode = kForce2Bytes;
		
	else if (windowInfoPtr->localMaxNumberBytes == 4 && 
														windowInfoPtr->localBytesDifferFlag)
		reformatOptionsPtr->forceByteCode = kForce4Bytes;
		
	else if (windowInfoPtr->localMaxNumberBytes == 8 && 
														windowInfoPtr->localBytesDifferFlag)
		reformatOptionsPtr->forceByteCode = kForceReal8Bytes;
		
	if (compareImagesOptionsPtr->procedureCode == kReprojectToReferenceImage)
		{
				// Save the selected area for the input image.
				
		reformatOptionsPtr->startColumn = reformatOptionsPtr->columnStart;
		reformatOptionsPtr->stopColumn = reformatOptionsPtr->columnEnd;
		reformatOptionsPtr->startLine = reformatOptionsPtr->lineStart;
		reformatOptionsPtr->stopLine = reformatOptionsPtr->lineEnd;
		
				// Set the area for the output image to represent the size of the 
				// reference image.
		
		reformatOptionsPtr->columnStart = 1;
		reformatOptionsPtr->columnEnd = GetMaxNumberColumns (
											compareImagesOptionsPtr->referenceWindowInfoHandle);
		reformatOptionsPtr->lineStart = 1;
		reformatOptionsPtr->lineEnd = GetMaxNumberLines (
											compareImagesOptionsPtr->referenceWindowInfoHandle);
		
		compareImagesOptionsPtr->procedureCode = procedureCode;
		
		}	// end "if (...->procedureCode == kReprojectToReferenceImage)"
	
}	// end "CompareImagesDialogOK"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CompareImagesDialogOnAlgorithmCode
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
//	Coded By:			Larry L. Biehl			Date: 02/24/2007
//	Revised By:			Larry L. Biehl			Date: 07/10/2015

void CompareImagesDialogOnAlgorithmCode (
				DialogPtr							dialogPtr,
				SInt16								rectifyCode,
				Boolean								blankOutsideSelectedAreaFlag,
				double								mapOrientationAngle)

{		
	SetDLogControl (dialogPtr, 
							IDC_NonSelectedPixels, 
							blankOutsideSelectedAreaFlag);
																				
	if (rectifyCode == kTranslateScaleRotate)
		{
		ShowDialogItem (dialogPtr, IDC_TranslateScaleTitle);
		#if defined multispec_wx
			ShowDialogItem (dialogPtr, IDC_TranslateScaleTitle2);
			ShowDialogItem (dialogPtr, IDC_TranslateScaleTitle3);
		#endif	// defined multispec_wx
		ShowDialogItem (dialogPtr, IDC_LineTranslateScalePrompt);
		ShowDialogItem (dialogPtr, IDC_ColumnTranslateScalePrompt);
		ShowDialogItem (dialogPtr, IDC_LineOffset);
		ShowDialogItem (dialogPtr, IDC_ColumnOffset);
		ShowDialogItem (dialogPtr, IDC_LineScale);
		ShowDialogItem (dialogPtr, IDC_ColumnScale);
		ShowDialogItem (dialogPtr, IDC_RotationTitle);
		ShowDialogItem (dialogPtr, IDC_RotationClockwisePrompt);
		ShowDialogItem (dialogPtr, IDC_RotationClockwise);
		
		if (mapOrientationAngle == 0)
			HideDialogItem (dialogPtr, IDC_UseMapOrientationAngle);
			
		else	// mapOrientationAngle != 0
			ShowDialogItem (dialogPtr, IDC_UseMapOrientationAngle);
		
		SetDLogControlHilite (dialogPtr, IDC_NonSelectedPixels, 0); 
		
		HideDialogItem (dialogPtr, IDC_ReferenceFileListPrompt);
		HideDialogItem (dialogPtr, IDC_ReferenceFileList);
		HideDialogItem (dialogPtr, IDC_ResampleMethodPrompt);
		HideDialogItem (dialogPtr, IDC_ResampleMethod);
		
		}	// end "if (listCount <= 0)"
		
	else	// rectifyCode != kTranslateScaleRotate
		{
		HideDialogItem (dialogPtr, IDC_TranslateScaleTitle);
		#if defined multispec_wx
			HideDialogItem (dialogPtr, IDC_TranslateScaleTitle2);
			HideDialogItem (dialogPtr, IDC_TranslateScaleTitle3);
		#endif	// defined multispec_wx
		HideDialogItem (dialogPtr, IDC_LineTranslateScalePrompt);
		HideDialogItem (dialogPtr, IDC_ColumnTranslateScalePrompt);
		HideDialogItem (dialogPtr, IDC_LineOffset);
		HideDialogItem (dialogPtr, IDC_ColumnOffset);
		HideDialogItem (dialogPtr, IDC_LineScale);
		HideDialogItem (dialogPtr, IDC_ColumnScale);
		HideDialogItem (dialogPtr, IDC_RotationTitle);
		HideDialogItem (dialogPtr, IDC_RotationClockwisePrompt);
		HideDialogItem (dialogPtr, IDC_RotationClockwise);
		HideDialogItem (dialogPtr, IDC_UseMapOrientationAngle);
		
		SetDLogControlHilite (dialogPtr, IDC_NonSelectedPixels, 255); 
		
		ShowDialogItem (dialogPtr, IDC_ReferenceFileListPrompt);
		ShowDialogItem (dialogPtr, IDC_ReferenceFileList);
		ShowDialogItem (dialogPtr, IDC_ResampleMethodPrompt);
		ShowDialogItem (dialogPtr, IDC_ResampleMethod);
		
		}	// else "else rectifyCode != kTranslateScaleRotate"
	
}	// end "CompareImagesDialogOnAlgorithmCode"


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 RectifyImageDialogOnReferenceFile
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
				SInt16								procedureCode,
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
	
	  
	if (procedureCode == kTranslateScaleRotate)
		{
		*referenceWindowInfoHandlePtr = gActiveImageWindowInfoH;

		}	// end "if (procedureCode == kTranslateScaleRotate)" 
		
	else	// procedureCode != kTranslateScaleRotate 
		{
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
			
		}	// end "else procedureCode != kTranslateScaleRotate"
	/*			
			// Decided to use the input line and column display in the dialog to always
			// represent that to be used is that is selected in the active image window
			// not that selected in the reference image.  It seems more intuitive this
			// way.
			// Use the selection information from the reference image.
			
	dialogSelectAreaPtr->windowPtr = GetWindowPtr (*referenceWindowInfoHandlePtr);
	dialogSelectAreaPtr->adjustToBaseImageFlag = kDontAdjustToBaseImage;

	dialogSelectAreaPtr->imageWindowInfoPtr = 
				(WindowInfoPtr)GetHandleStatusAndPointer (*referenceWindowInfoHandlePtr,
																		&handleStatus);	
		
			// Get any selected area for the target image.																	
	
	if (!GetSelectedAreaInfo (dialogSelectAreaPtr->windowPtr,
										dialogSelectAreaPtr->imageWindowInfoPtr,
										(SInt32*)&dialogSelectAreaPtr->lineStart,
										(SInt32*)&dialogSelectAreaPtr->lineEnd,
										(SInt32*)&dialogSelectAreaPtr->columnStart,
										(SInt32*)&dialogSelectAreaPtr->columnEnd,
										kDontClearSelectionArea,
										kUseThreshold,
										dialogSelectAreaPtr->adjustToBaseImageFlag))
   	{
		dialogSelectAreaPtr->lineStart = 1;
		dialogSelectAreaPtr->lineEnd = 
										GetMaxNumberLines (*referenceWindowInfoHandlePtr);
		dialogSelectAreaPtr->columnStart = 1;
		dialogSelectAreaPtr->columnEnd = 
										GetMaxNumberColumns (*referenceWindowInfoHandlePtr);
							
		}	// end "if (!GetSelectedAreaInfo (..."

	LoadLineColumnItems (dialogSelectAreaPtr, 
									dialogPtr, 
									kInitializeLineColumnValues, 
									kIntervalEditBoxesExist,
									1);
	
	MHSetState (*referenceWindowInfoHandlePtr, handleStatus);
	
	dialogSelectAreaPtr->imageWindowInfoPtr = NULL;
	
	#if defined multispec_mac 
		InvalWindowRect (
						GetDialogWindow (dialogPtr), &dialogSelectAreaPtr->toSelectionBox);
	#endif	// defined multispec_mac   
	*/	
	return (fileNamesSelection);
	
}	// end "CompareImagesDialogOnReferenceFile"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CompareImages
//
//	Software purpose:	This routine does the rectification of the image.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/02/2006
//	Revised By:			Larry L. Biehl			Date: 07/16/2018

Boolean CompareImages (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				FileInfoPtr							outFileInfoPtr, 
				ReformatOptionsPtr				reformatOptionsPtr)

{
			// Declare local variables & structures
			
	DoubleRect							boundingRect;
			
	double								backgroundValue;
												
	CMFileStream*						outFileStreamPtr;
	FileInfoPtr							fileInfoPtr;
	
	MapProjectionInfoPtr				mapProjectionInfoPtr,
											referenceMapProjectionInfoPtr;
											
	RectifyImageOptionsPtr			rectifyImageOptionsPtr;
	
	unsigned char						*ioOutBufferPtr,
					 						*columnOutBufferPtr,
					 						*savedOutBufferPtr;
	
	UInt32								*histogramVector;
					 					
	SInt16								//*ioIn2ByteBufferPtr,
											*rectifyChannelPtr;
	
	UInt16								*savedOutBuffer2BytePtr;
	
	unsigned char 						*ioOut1ByteBufferPtr,
								 			*ioIn1ByteBufferPtr;
								 			
	Handle								mapProjectionHandle,
											referenceMapProjectionHandle,
											referenceWindowInfoHandle,
											windowInfoHandle;
	
	SInt32								column,
											columnByteSkip,
											countInBytes,
											countOutBytes,
											inputColumn,
											inputLine,
											lastOutputWrittenLine,
											limitIoOutBytes,
											line,
											numberOutputColumns,
											outputPixelValue,
											percentComplete,
											preLineBytes,
											//rectifiedInputOffset,
											//rectifiedOutputOffset,
											totalIOOutBytes,
											writePosOff;
	
	UInt32								channelCount,
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
											rectifyBytesToMove,
											startColumn,
											startLine,
											stopColumn,
											stopLine,
											supportFileType;
	
	SInt16								errCode,
											GAIAPrelineString[7] = 
													  {0x0300, 0x0600, 0x0000, 0x0100,
														0x0000, 0x0000, 0x0000},
											lastPercentComplete,
											numberReadChannels,
											resampleCode;
												
	UInt16								forceByteCode,
											numberOutChannels;										
	
	Boolean								continueFlag,
											forceBISFlag;
	
		
			// Initialize local variables.													
			
	errCode = noErr;
	writePosOff = 0;
	fileInfoPtr = fileIOInstructionsPtr->fileInfoPtr;
	forceByteCode = reformatOptionsPtr->forceByteCode;
	forceBISFlag = (outFileInfoPtr->bandInterleave == kBIS);
	preLineBytes = outFileInfoPtr->numberPreLineBytes;
	outFileStreamPtr = GetFileStreamPointer (outFileInfoPtr);
	histogramVector = NULL;
	
	rectifyImageOptionsPtr = reformatOptionsPtr->rectifyImageOptionsPtr;
	rectifyChannelPtr = rectifyImageOptionsPtr->rectifyChannelPtr;
	backgroundValue = rectifyImageOptionsPtr->backgroundValue;
	resampleCode = rectifyImageOptionsPtr->resampleCode;
					
	ioOutBufferPtr = (unsigned char*)reformatOptionsPtr->ioOutBufferPtr;
				
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
	
			// Get handle to window information for the active image
			
	windowInfoHandle = GetActiveImageWindowInfoHandle ();
	referenceWindowInfoHandle = rectifyImageOptionsPtr->referenceWindowInfoHandle;
		
	if (referenceWindowInfoHandle == NULL || windowInfoHandle == NULL)
																						return (FALSE);
			
			// Set up pointers to map structure information for the active image
			// window
	
	mapProjectionHandle = GetFileMapProjectionHandle2 (windowInfoHandle);
	
	mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																		mapProjectionHandle, kLock);
	
	GetCoefficientsVectorPointers (mapProjectionInfoPtr);					
			
			// Set up pointers to map structure information for the reference image
			
	referenceMapProjectionHandle = 
								GetFileMapProjectionHandle2 (referenceWindowInfoHandle);
	
	referenceMapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
															referenceMapProjectionHandle, kLock);
	
	GetCoefficientsVectorPointers (referenceMapProjectionInfoPtr);
	
			// Write a header if requested.													
			
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
		limitIoOutBytes = reformatOptionsPtr->ioOutBufferBytes - countOutBytes;
		totalIOOutBytes = 0;
		
				// Initialize a background line that will be copied from as			
				// needed.	
								
		InitializeLineToBackgroundValue (outFileInfoPtr, 
													(HUInt8Ptr)ioOutBufferPtr,
													countOutBytes,
													backgroundValue);		
													
				// Set up output working buffer pointer.  This pointer will	 change as
				// the output buffer fills up
		
		savedOutBufferPtr = &ioOutBufferPtr[countOutBytes];
		
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
										
		outChannelByteIncrement = (countOutBytes-preLineBytes)/numberOutChannels;
		
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
		
		outNumberBytesPerLineAndChannel = (countOutBytes-preLineBytes)/numberOutChannels;
		if (outFileInfoPtr->bandInterleave == kBIS)
			outNumberBytesPerLineAndChannel /= numberOutputColumns;
	
		rectifyBytesToMove = (stopColumn - startColumn + 1) * numberBytes;
		rectifyBytesToMove = MIN (outNumberBytesPerLineAndChannel, rectifyBytesToMove);
		
				// Get the bounding latitude-longitude rectangle for the active
				// image window.
				// Note that as of 7/16/2018 this is not being used. Will leave in for
				// a while in case it is found it is needed again. This was used to try
				// to save time by catching when the nearest neighbor calculation did not
				// need to be done; but found this check caused problems by leaving out
				// pixels that could really be used. The bounding rectangle is based on
				// the upper left and lower right point. It is not guaranteed that
				// the top or bottom coordinated does not vary across the width. The
				// same for the left and right.
		
		//if (referenceMapProjectionInfoPtr->gridCoordinate.referenceSystemCode ==
		//																				kGeographicRSCode)
		//	{
			boundingRect.top = gImageWindowInfoPtr->boundingLatLongRectangle.top;
			boundingRect.left = gImageWindowInfoPtr->boundingLatLongRectangle.left;
			boundingRect.bottom = gImageWindowInfoPtr->boundingLatLongRectangle.bottom;
			boundingRect.right = gImageWindowInfoPtr->boundingLatLongRectangle.right;
			
		//	}	// end "...->gridCoordinate.referenceSystemCode == kGeographicRSCode"
			
		//else	// ...->gridCoordinate.referenceSystemCode != kGeographicRSCode
		//	{
		//	boundingRect.top = gImageWindowInfoPtr->boundingMapRectangle.top;
		//	boundingRect.left = gImageWindowInfoPtr->boundingMapRectangle.left;
		//	boundingRect.bottom = gImageWindowInfoPtr->boundingMapRectangle.bottom;
		//	boundingRect.right = gImageWindowInfoPtr->boundingMapRectangle.right;
			
		//	}	// end "else ...->gridCoordinate.referenceSystemCode != kGeographicRSCode"
					
				// Intialize the nextTime variable to indicate when the next 		
				// check should occur for a command-.										
				
		gNextTime = TickCount ();
		
				// Get the total number of samples a line of data.						
				// It will be used when adjusting the data if request.				
				
		numberColumnsChannels = 
								((SInt32)columnEnd - columnStart + 1) * numberReadChannels;
						
		if (resampleCode == kMajorityRule)
			{
					// Get histogram vector to be used.
					
			histogramVector = (UInt32*)MNewPointerClear (256*sizeof (UInt32));
			
			if (histogramVector == NULL)
				continueFlag = FALSE;
						
			}	// end "if (resampleCode == kMajorityRule)"
			
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
															NULL,
															kDetermineSpecialBILFlag);		
												
		if (errCode != noErr)	
			continueFlag = FALSE;			
		
		line = 1;
		lastLineRead = -1;
		lastOutputWrittenLine = 0;

				// Turn spin cursor on

		gPresentCursor = kSpin;
		
		while (line <= (SInt32)numberOutputLines && continueFlag)
			{
			ioOut1ByteBufferPtr = savedOutBufferPtr;
			
					// Initialize output line to the background value.	
					
			if (outFileInfoPtr->bandInterleave == kBSQ)
				{
				channelCount = 0;
				while (channelCount<numberOutChannels)
					{	
					BlockMoveData (ioOutBufferPtr, 
										&ioOut1ByteBufferPtr[preLineBytes], 
										outNumberBytesPerLineAndChannel);
										
					ioOut1ByteBufferPtr += outChannelByteIncrement;
					channelCount++;
										
					}	// end "while (channelCount<numberOutChannels)"
					
				ioOut1ByteBufferPtr = savedOutBufferPtr;
					
				}	// end "if (outFileInfoPtr->bandInterleave == kBSQ)"
			
			else	// ...->bandInterleave != kBSQ	
				BlockMoveData (ioOutBufferPtr, 
									&ioOut1ByteBufferPtr[preLineBytes], 
									countOutBytes-preLineBytes);
		
					// Add the preline calibration bytes if any.  For now this is	
					// only handled for GAIA data.											
					
			if (preLineBytes > 0)
				{
				BlockMoveData (&GAIAPrelineString, ioOut1ByteBufferPtr, preLineBytes);
				ioOut1ByteBufferPtr += preLineBytes;
				
				}	// end "if (preLineBytes > 0)" 
				
			if (continueFlag)
				{
				columnOutBufferPtr = &savedOutBufferPtr[preLineBytes];
										
				for (column=1; column<=numberOutputColumns; column++)
					{
					if (resampleCode == kNearestNeighbor)
						{
							
						if (inputLine >= (SInt32)lineStart && 
									inputColumn >= (SInt32)columnStart &&
										inputLine <= (SInt32)lineEnd &&
											inputColumn <= (SInt32)columnEnd)
							{
							if (inputLine != (SInt32)lastLineRead)
								{
										// Get all requested channels for line of 			
										// image data. Return if there is a file IO error.	
							 
								errCode = GetLineOfData (fileIOInstructionsPtr,
																	inputLine, 
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
																							
								lastLineRead = inputLine;
																							
								}	// end "if (inputLine != lastLineRead)" 
								
							channelCount = 0;
							inputColumn -= columnStart;
							
									// Adjust column index to allow for band interleave.
									
							if (forceBISFlag || fileInfoPtr->bandInterleave == kBIS)
								inputColumn *= numberReadChannels;
								
									// Set input buffer pointer.												
					
							ioIn1ByteBufferPtr = (unsigned char*)
														&gOutputBufferPtr[inputColumn*numberBytes];
							
									// Set output buffer pointers.								
			
							ioOut1ByteBufferPtr = columnOutBufferPtr;
						
							while (channelCount<numberOutChannels)
								{
								if (rectifyChannelPtr[channelCount])
									memcpy (ioOut1ByteBufferPtr,
												ioIn1ByteBufferPtr,
												numberBytes);
							
								channelCount++;
							
								ioIn1ByteBufferPtr += inOffsetBytes;
								ioOut1ByteBufferPtr += outChannelByteIncrement;
						
								}	// end "while (channelCount<..." 
								
							}	// end "else inputLine && inputColumn ... 
						
						}	// end "if (resampleCode == kNearestNeighbor)"
						
					else if (resampleCode == kMajorityRule)
						{																
						if (numberBytes == 1)
							savedOutBufferPtr[column-1] = (UInt8)outputPixelValue;
																
						else if (numberBytes == 2)
							savedOutBuffer2BytePtr[column-1] = (UInt16)outputPixelValue;
						
						}	// end "else if (resampleCode == kMajorityRule)"
						
					columnOutBufferPtr += columnByteSkip;
			
							// Check if user wants to abort processing.							
							
					if (TickCount () >= gNextTime)
						{
						if (!CheckSomeEvents (
												osMask+keyDownMask+updateMask+mDownMask+mUpMask))
							continueFlag = FALSE;
							
						}	// end "if (TickCount () >= nextTime)" 
						
					}	// end "for (column=1; column<=numberOutputColumns; ..." 
					
				}	// end "if (continueFlag)" 
			
			if (outFileInfoPtr->bandInterleave == kBSQ)
				savedOutBufferPtr = &savedOutBufferPtr[outNumberBytesPerLineAndChannel];
			
			else	// outFileInfoPtr->bandInterleave != kBSQ
				savedOutBufferPtr = &savedOutBufferPtr[countOutBytes];
				
			savedOutBuffer2BytePtr = (UInt16*)savedOutBufferPtr;
	
					// Write line(s), channel(s) of data when needed.					
			
			totalIOOutBytes += countOutBytes;
			
			if (continueFlag && (totalIOOutBytes > limitIoOutBytes))
				{
				errCode = WriteOutputDataToFile (outFileInfoPtr,
																outFileStreamPtr,
																&ioOutBufferPtr [countOutBytes],
																reformatOptionsPtr->channelPtr,
																numberOutChannels,
																lastOutputWrittenLine,
																outNumberBytesPerLineAndChannel,
																numberOutputLines,
																outChannelByteIncrement,
																totalIOOutBytes,
																reformatOptionsPtr,
																1);
				
				totalIOOutBytes = 0;
				savedOutBufferPtr = (unsigned char*)&ioOutBufferPtr[countOutBytes];
				
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
			
					// Adjust line.																
					
			line++;
			//inputLine++;
				
			}	// end "while (line < numberOutputLines && continueFlag)" 
			 
		CheckAndDisposePtr (histogramVector);
			
				// Flush output buffer if needed.											
		
		if (continueFlag && totalIOOutBytes > 0)
			{				
			errCode = WriteOutputDataToFile (outFileInfoPtr,
															outFileStreamPtr,
															&ioOutBufferPtr [countOutBytes],
															reformatOptionsPtr->channelPtr,
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
			
			}	// end "if (continueFlag && (totalIOOutBytes > 0))" 
		
				// Close up any File IO Instructions structure that pertain to the 
				// specific area used for rectification.
				
		CloseUpFileIOInstructions (fileIOInstructionsPtr, NULL);

				// Turn spin cursor off
	
		gPresentCursor = kWait;
		
				// Write channel discriptions at the end of the file if needed.	
		
		if (continueFlag && reformatOptionsPtr->channelDescriptions)	
			WriteChannelDescriptionsAndValues (
									outFileInfoPtr, 
									NULL, 
									outFileInfoPtr->numberChannels,
									(reformatOptionsPtr->outputFileCode == kAppendToMenuItem));

		if (continueFlag && outFileInfoPtr->thematicType)
			{           
			supportFileType = kITRLFileType;
			if (outFileInfoPtr->format != kErdas74Type)
				supportFileType = kICLRFileType;
				
			//outFileInfoPtr->asciiSymbols = TRUE;
			                                
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
												
	CloseCoefficientsVectorPointers (mapProjectionInfoPtr);
	CloseCoefficientsVectorPointers (referenceMapProjectionInfoPtr);
	
	CheckAndUnlockHandle (mapProjectionHandle);
	CheckAndUnlockHandle (referenceMapProjectionHandle);
		
	return (!continueFlag);
			
}	// end "CompareImages"



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
//	Coded By:			Larry L. Biehl			Date: 06/07/2012
//	Revised By:			Larry L. Biehl			Date: 11/08/2019

void SetUpCompareMethodPopupMenu (
				DialogPtr							dialogPtr,
				MenuHandle							popUpResampleSelectionMenu,
				Boolean								thematicTypeFlag)

{	
	#if defined multispec_mac
		if (popUpResampleSelectionMenu != NULL)
			{
					// The mean resample method has not been initiated yet.
					
			DeleteMenuItem (popUpResampleSelectionMenu, 2);
			//DisableMenuItem (popUpResampleSelectionMenu, kMean);

			if (!thematicTypeFlag)
						// The majority rule is only for thematic images.
				DisableMenuItem (popUpResampleSelectionMenu, kMajorityRule);
		
			}	// end "if (popUpResampleSelectionMenu != NULL)"
	#endif	// defined multispec_mac
					
	#if defined multispec_win
		CComboBox* comboBoxPtr =
								(CComboBox*)(dialogPtr->GetDlgItem (IDC_ResampleMethod));

		comboBoxPtr->SetItemData (0, kNearestNeighbor);
			
				// The "mean" option is not implemented yet.
				
		comboBoxPtr->DeleteString (1);
		 
		if (thematicTypeFlag)
			comboBoxPtr->SetItemData (1, kMajorityRule);
			
		else	// !thematicTypeFlag
			comboBoxPtr->DeleteString (1);
	#endif	// defined multispec_win

	#if defined multispec_wx
		wxChoice* resampleCtrl =
								(wxChoice*)wxWindow::FindWindowById (IDC_ResampleMethod);
		
		resampleCtrl->Append ("Nearest Neighbor");

      int m_NearestNeighbor = kNearestNeighbor;
		resampleCtrl->SetClientData (0, (void*)(SInt64)m_NearestNeighbor);
		
		if (thematicTypeFlag)
			{
			int m_kMajorityRule = kMajorityRule;
			resampleCtrl->Append ("Majority");
			resampleCtrl->SetClientData (1, (void*)(SInt64)m_kMajorityRule);
			
			}	// end "if (thematicTypeFlag)"
   #endif	// defined multispec_wx
	
}	// end "SetUpCompareMethodPopupMenu" 

 
