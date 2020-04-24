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
//	File:						SFieldsToThematicFile.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			04/17/2020
//
//	Language:				C
//
//	System:					Linux, Macintosh and Windows Operating Systems
//
//	Brief description:	The purpose of the routines in this file is to convert the
//								training and testing areas for the requested project classes
//								into a Thematic type file with 0 for all non-selected project
//								training/test pixels and the class number for all selected
//								project training/test pixels.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include "SFileStream_class.h"

#if defined multispec_wx
	#include "xFieldsToThematicDialog.h"
#endif	// defined multispec_wx

#if defined multispec_mac || defined multispec_mac_swift
	#define	IDC_TrainingAreas		15 
	#define	IDC_TestingAreas		16
	#define	IDS_Reform29			29
#endif	// defined multispec_mac || defined multispec_mac_swift
  
#if defined multispec_win
	#include "WFieldsToThematicDialog.h"
#endif	// defined multispec_win

#define	kFieldsToThematicNoHeaderOutputFormat		1
#define	kFieldsToThematicERDAS74OutputFormat		2
#define	kFieldsToThematicTIFFGeoTIFFMenuItem		3

				

		// Prototypes for routines in this file that are only called by			
		// other routines in this file.														
		
Boolean ConvertImagePixelsToClassNumbers (
				FileInfoPtr							outFileInfoPtr, 
				ReformatOptionsPtr				reformatOptionsPtr);

Boolean CovertMultispectralToThematic (
				WindowInfoPtr						inputWindowInfoPtr,
				FileInfoPtr							outFileInfoPtr,
				ReformatOptionsPtr				reformatOptionsPtr);

#if defined multispec_mac
	pascal void	DrawDiskFileFormatPopUp (
					DialogPtr							dialogPtr,
					SInt16								itemNumber);
#endif

Boolean ENVI_ROIToThematicConvertPixelsToClassNumbers1 (
				FileInfoPtr							outFileInfoPtr,
				ReformatOptionsPtr				reformatOptionsPtr,
				HParamBlockRec*					paramBlockPtr,
				UCharPtr								inputBuffer,
				UCharPtr								inputStringPtr,
				UCharPtr								classNamePtr,
				ColorSpec*							colorSpecPtr);

Boolean ENVI_ROIToThematicConvertPixelsToClassNumbers2 (
				FileInfoPtr							outFileInfoPtr,
				ReformatOptionsPtr				reformatOptionsPtr,
				HParamBlockRec*					paramBlockPtr,
				UCharPtr								inputBuffer,
				UCharPtr								inputStringPtr,
				UCharPtr								classNamePtr,
				ColorSpec*							colorSpecPtr);

Boolean ENVI_ROIToThematicGetASCIIFile (
				CMFileStream*						inputFileStreamPtr,
				FileInfoPtr							outFileInfoPtr, 
				ReformatOptionsPtr				reformatOptionsPtr,
				HParamBlockRec*					paramBlockPtr,
				UCharPtr								bufferPtr,
				UInt32								bufferLength,
				UCharPtr*							inputStringPtrPtr,
				SInt16*								endOfLineCodePtr,
				UInt32*								numberEndOfLineBytesPtr,
				SInt16*								asciiFormatCodePtr);

void ENVI_ROIToThematicGetClassName (
				UCharPtr								inputStringPtr,
				UCharPtr								classNamePtr);

Boolean ENVI_ROIToThematicLoadSpecs (
				Handle*								reformatOptionsHPtr);
				
Boolean LoadAreasToThematicSpecs (
				Handle*								reformatOptionsHPtr);

Boolean LoadAreasToThematicDialog (
				ReformatOptionsPtr				reformatOptionsPtr);

Boolean MultispectralToThematicDialog (
				ReformatOptionsPtr				reformatOptionsPtr);



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void AreasToThematicFileControl
//
//	Software purpose:	The purpose of this routine is to control the 
//							conversion of the project training/test areas to a
//							Thematic image file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/29/1990
//	Revised By:			Larry L. Biehl			Date: 11/07/2019

void AreasToThematicFileControl (void)

{
	FileInfoPtr							outFileInfoPtr;
	ReformatOptionsPtr				reformatOptionsPtr;
	
	Handle								reformatOptionsHandle;
	
	UInt32								supportFileType;
	
	time_t								startTime;
	
	Boolean								continueFlag;

	
			// If spare memory had to be used to load code resources, then exit	
			// routine.																				
			
	if (gMemoryTypeNeeded < 0)
																							return;
																							
			// Code resources loaded okay, so set flag back for non-Code			
			// resources.																			
			
	gMemoryTypeNeeded = 0;
	
	
			// Check input parameters.															
			
	if (gProjectInfoPtr == NULL)
																							return;
																							
			// Get the reformat structure and initialize it.							
			
	if (!LoadAreasToThematicSpecs (&reformatOptionsHandle))
																							return;
	
			// Note that the following handles were locked in the call to
			// 'LoadAreasToThematicSpecs'
			
	reformatOptionsPtr = (ReformatOptionsPtr)GetHandlePointer (reformatOptionsHandle);

	outFileInfoPtr = (FileInfoPtr)GetHandlePointer (
																reformatOptionsPtr->outFileInfoHandle);
	
			// Present the user with a dialog box to allow a change in the default	
			// specifications.																	
			
	if (LoadAreasToThematicDialog (reformatOptionsPtr))
		{				
		startTime = time (NULL);
									
				// List the processor name, date and time.								

		gOutputForce1Code = 0x0001;
		continueFlag = ListHeaderInfo (NULL, 
													0, 
													&gOutputForce1Code, 
													kNoStatisticsUsed, 
													TRUE);
			
		if (continueFlag)
			{				
					// Update parameters in the structure for the output file.		
					
			UpdateOutputFileStructure (
							outFileInfoPtr,
							reformatOptionsPtr,
							gProjectInfoPtr->startLine, 
							gProjectInfoPtr->startColumn,
							GetFileMapProjectionHandle2 (gProjectInfoPtr->windowInfoHandle));
			
					// Get buffer for output data.													
					
			continueFlag = GetReformatOutputBuffer (outFileInfoPtr, reformatOptionsPtr);
			
			}	// end "if (continueFlag)"
					
				// Set up the disk file to be used for the new reformatted				
				// image file.																			
		
		if (continueFlag)	
			continueFlag = !GetReformatOutputFile (outFileInfoPtr, reformatOptionsPtr);
		
				// Change cursor to watch cursor until done with process.				
		
		MSetCursor (kWait);
		
				// Get the status dialog window.													
		
		gStatusDialogPtr = GetStatusDialog (kUpdateStatsInfoID, TRUE);
		if (continueFlag)	
			continueFlag = (gStatusDialogPtr != NULL);
	
				//	Title for creating background image.										
	
		LoadDItemStringNumber (kReformatStrID,
										IDS_Reform13, 	// "\pCreating Thematic Image Background"
										gStatusDialogPtr, 
										IDC_Status11,
										(Str255*)gTextString);
				
				// Create the base Thematic Image file with all background values.	
				
		if (continueFlag)	
			continueFlag = CreateBackgroundImageFile (
														outFileInfoPtr, 
														reformatOptionsPtr->ioOutBufferPtr,
														kFromReformat,
														1,
														(UInt8)reformatOptionsPtr->backgroundValue,
														TRUE); 
	
				//	Set title and dialog items for changing pixels to class numbers.	
	
		if (continueFlag)
			{
			LoadDItemStringNumber (kReformatStrID,
											IDS_Reform14,  	// "\pFields to Thematic Image"
											gStatusDialogPtr, 
											IDC_Status11,
											(Str255*)gTextString); 
											
			ShowStatusDialogItemSet (kStatusClassA);
			ShowStatusDialogItemSet (kStatusField);
			
			CheckSomeEvents (updateMask);
			
			}	// end "if (continueFlag)" 
				
				// Now change those pixels that belong to one of the requested			
				// project classes.																	
		
		if (continueFlag)	
			continueFlag = ConvertImagePixelsToClassNumbers (
																outFileInfoPtr, reformatOptionsPtr);
		
				// Create the trailer file if needed.									
				// This is not needed the trailer file was created with the
				// background image file.
				
		if (continueFlag && reformatOptionsPtr->headerFormat != kNoneType)
			{
			supportFileType = kICLRFileType;
			if (outFileInfoPtr->format == kErdas74Type)
				supportFileType = kITRLFileType;
			
			continueFlag = CreateThematicSupportFile (
														outFileInfoPtr,
														NULL,
														(UInt16)reformatOptionsPtr->numberClasses,
														reformatOptionsPtr->classPtr,
														(UInt16)reformatOptionsPtr->numberClasses,
														NULL,
														NULL,
														NULL,
														kDefaultColors,
														(UInt16)outFileInfoPtr->numberClasses,
														0,
														kPaletteHistogramClassNames,
														kClassDisplay,
														kCollapseClass,
														supportFileType);
										
			}	// end "if (continueFlag && ...)"
			
				// Close the output file.
	  				
		CloseFile (outFileInfoPtr);
					
				// List Reformat information.												
		
		if (continueFlag)		
			continueFlag = ListReformatResultsInformation (
															reformatOptionsPtr, outFileInfoPtr);

				// List the CPU time taken for reformatting.							
			
		continueFlag = 
					ListCPUTimeInformation (NULL, continueFlag, startTime);
		
		}	// end "if (LoadAreasToThematicDialog (reformatOptionsPtr))"
		
		
	ReleaseReformatOutputFileInfoAndBuffers (reformatOptionsPtr, NULL);
			
			// Release memory used for the reformat structure.							
			
	//ReleaseReformatSpecsMemory (&reformatOptionsHandle, NULL);
					
			// Dispose of updating statistics status dialog box.						
		
	CloseStatusDialog (TRUE);
			
	MInitCursor ();
		
}	// end "AreasToThematicFileControl" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ConvertImagePixelsToClassNumbers
//
//	Software purpose:	The purpose of this routine is to convert all pixels
//							in the specified image file that belong to a selected
//							class to the respective class number.  The pixels in
//							the input image file will have been set to a background
//							value.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			AreasToThematicFileControl
//
//	Coded By:			Larry L. Biehl			Date: 11/30/1990
//	Revised By:			Larry L. Biehl			Date: 03/16/2014

Boolean ConvertImagePixelsToClassNumbers (
				FileInfoPtr							outFileInfoPtr, 
				ReformatOptionsPtr				reformatOptionsPtr)
			
{
	AreaDescription					areaDescription;
	SInt64								writePosOff;
	Point									point;
	RgnHandle							rgnHandle;
	
	CMFileStream*						outFileStreamPtr;
	FileIOInstructionsPtr			fileIOInstructionsPtr;
	HPClassNamesPtr					classNamesPtr;
	HPFieldIdentifiersPtr			fieldIdentPtr;
	
	HUInt16Ptr							maskBufferPtr,
											savedMaskBufferPtr;
											
	HUCharPtr				 			ioOutBufferPtr;
											
	SInt32								columnEnd,
											columnInterval,
											columnStart,
											imageColumnEnd,
											imageColumnStart,
											imageLineNumber,
											lineEnd,
											lineInterval,
											lineStart,
											numberFieldColumns,
											numberImageColumns;

	UInt32								classIndex,
											count,
											fieldColumnEnd,
											fieldColumnStart,
											fieldLineEnd,
											fieldLineStart,
											line,
											maskColumnStart,
											maskRequestValue,
											numberMaskColumns,
											sample;
	
	SInt16								classStorage,
											errCode,
											fieldCount,
											fieldNumber,
											numberFields,
											pointType,
											requestedFieldType,
											statClassIndex,
											statClassNumber;
											
	Boolean								continueFlag,
											includePixelFlag,
											skipLineFlag;

	unsigned char						backgroundValue;


			// Check input parameters.															
			
	if (outFileInfoPtr == NULL || reformatOptionsPtr == NULL)
																						return (FALSE);
	
			// Initialize local variables.

	outFileStreamPtr = GetFileStreamPointer (outFileInfoPtr);													
							
	numberImageColumns = outFileInfoPtr->numberColumns;
	
	lineStart = reformatOptionsPtr->lineStart;
	lineEnd = reformatOptionsPtr->lineEnd;
	lineInterval = reformatOptionsPtr->lineInterval;
	columnStart = reformatOptionsPtr->columnStart;
	columnEnd = reformatOptionsPtr->columnEnd;
	columnInterval = reformatOptionsPtr->columnInterval;
	
	backgroundValue = (UInt8)reformatOptionsPtr->backgroundValue;
	
	requestedFieldType = reformatOptionsPtr->fieldType;
	
	continueFlag = TRUE;
	errCode = noErr;
	
	classNamesPtr = gProjectInfoPtr->classNamesPtr;
	
	InitializeAreaDescription (&areaDescription);
			
			// Set up status dialog.  Load in number of classes.						
				
	LoadDItemValue (gStatusDialogPtr, 
							IDC_Status5, 
							(SInt32)reformatOptionsPtr->numberClasses);
		 
	SetUpGeneralFileIOInstructions (&gFileIOInstructions[0],
												NULL,	
												NULL,
												outFileInfoPtr,
												1,
												NULL,
												(HUCharPtr)reformatOptionsPtr->ioOutBufferPtr,
												(HUCharPtr)reformatOptionsPtr->ioOutBufferPtr,
												NULL,
												0,
												kDoNotPackData,
												kDoNotForceBISFormat,	
												kDoNotForceBytes,
												&fileIOInstructionsPtr);  
	
	SetUpFileIOInstructions (fileIOInstructionsPtr,
										NULL,
										lineStart,
										lineEnd,
										lineInterval,
										columnStart,
										columnEnd,
										columnInterval,
										1,
										NULL,
										kSetSpecialBILFlagFalse);
										
	gNextStatusTime = TickCount ();
	gNextTime = TickCount ();
	
	for (classIndex=0; classIndex<reformatOptionsPtr->numberClasses; classIndex++)
		{
		statClassNumber = reformatOptionsPtr->classPtr[classIndex];
		statClassIndex = statClassNumber - 1;
		
				// Get the class storage number.												
						
		classStorage = gProjectInfoPtr->storageClass[statClassIndex];
		
		fieldNumber = classNamesPtr[classStorage].firstFieldNumber;
		
		numberFields = 0;
		if (requestedFieldType & kTrainingType) 
			numberFields += classNamesPtr[classStorage].numberOfTrainFields;
		
		if (requestedFieldType & kTestingType) 
			numberFields += classNamesPtr[classStorage].numberOfTestFields;
		
				// Set up status dialog.  Load in class count, number of train/	
				// test fields and class name.												
				
		LoadDItemValue (gStatusDialogPtr, IDC_Status3, (SInt32)classIndex+1);
		LoadDItemString (gStatusDialogPtr,
								IDC_Status6, 
								(Str255*)&classNamesPtr[classStorage].name);					
			
		LoadDItemValue (gStatusDialogPtr, IDC_Status10, (SInt32)numberFields);
				
		
		fieldCount = 1;
		
		while ((fieldNumber != -1) && continueFlag)
			{
			if (TickCount () >= gNextStatusTime)
				{
				LoadDItemValue (gStatusDialogPtr, IDC_Status8, (SInt32)fieldCount);
				gNextStatusTime = TickCount () + gNextStatusTimeOffset;
				
				}	// end "if (TickCount () >= gNextStatusTime)"
				
					// Allow for updates in the window or for the user to exit the routine.				
			
			if (TickCount () >= gNextTime)
				{
				continueFlag = CheckSomeEvents (
												osMask+keyDownMask+updateMask+mDownMask+mUpMask);
				if (!continueFlag)
					break;
					
				}	// end "if (TickCount () >= nextTime)" 
							
			fieldIdentPtr = &gProjectInfoPtr->fieldIdentPtr[fieldNumber];
	
					// Make certain that field is of the requested field type.		
					
			if (requestedFieldType & fieldIdentPtr->fieldType)
				{					
						// Check for cluster type of fields; there are no field		
						// definitions for cluster type fields.	
						
				pointType = fieldIdentPtr->pointType;					
						
				if (pointType != kClusterType)
					{
							// Get the field coordinates.										
							
					GetFieldBoundary (gProjectInfoPtr, &areaDescription, fieldNumber);
						
					rgnHandle = areaDescription.rgnHandle;
					
							// Take into account requested image area less than		
							// entire scene.														
							
					fieldLineStart = MAX (areaDescription.lineStart, lineStart);
					fieldLineEnd = MIN (areaDescription.lineEnd, lineEnd);
					fieldColumnStart = MAX (areaDescription.columnStart, columnStart);
					fieldColumnEnd = MIN (areaDescription.columnEnd, columnEnd);
					
					fieldLineStart = 
						((fieldLineStart-1)/lineInterval) * lineInterval + lineInterval;
					fieldColumnStart = ((fieldColumnStart-1)/columnInterval) *
																	columnInterval + columnInterval;
					
					numberFieldColumns = (fieldColumnEnd -
											fieldColumnStart + columnInterval)/columnInterval;
					numberFieldColumns = MAX (0, numberFieldColumns);
					
					imageLineNumber = (fieldLineStart - lineStart)/lineInterval;
					imageColumnStart = (fieldColumnStart - columnStart)/columnInterval;
					imageColumnEnd = imageColumnStart + numberFieldColumns - 1;
						
					maskBufferPtr = NULL;
		
							// Set output file pointer to correct position for 		
							// reading and writing the output data.						
							// The output data is first read as a starting point		
							// so that we handle polygonal areas properly.				
					
					writePosOff = outFileInfoPtr->numberHeaderBytes +
												imageLineNumber*numberImageColumns +
																				imageColumnStart;
							
							// Set up mask parameters if needed.
							
					if (pointType == kMaskType)
						{
						if (areaDescription.maskInfoPtr == NULL)
							{
							continueFlag = FALSE;
							break;
							
							}	// end "if (areaDescription.maskInfoPtr == NULL)"
						
								// Get the mask buffer.
																										
						savedMaskBufferPtr = (HUInt16Ptr)GetHandlePointer (
												areaDescription.maskInfoPtr->maskHandle, kLock);
						
						numberMaskColumns = areaDescription.maskInfoPtr->numberColumns + 1;
												
								// Adjust if needed for any change between columnStart and
								// fieldColumnStart.
								
						maskColumnStart = areaDescription.maskColumnStart + 
												fieldColumnStart - areaDescription.columnStart;
												
								// Adjust maskBufferPtr to correspond with start of field
								// line start.
											
						savedMaskBufferPtr += (areaDescription.maskLineStart + 
										fieldLineStart - areaDescription.lineStart - 1) * 
																						numberMaskColumns;
																						
								// Now make 'numberMaskColumns' represent the number of
								// columns to skip taking into account the line interval.
								
						numberMaskColumns *= lineInterval;
						
								// Get the mask value that corresponds to this class and field.
												
						maskRequestValue = areaDescription.maskValueRequest;
						
						}	// end "if (pointType == kMaskType)"
			
					if (!continueFlag)
						break;

							// Loop through lines for area to be assigned.			
				
					for (line=fieldLineStart; 
							line<=fieldLineEnd; 
							line+=lineInterval)
						{						
						skipLineFlag = FALSE;
						if (pointType == kMaskType)
							{
							maskBufferPtr = savedMaskBufferPtr;
							
							if (*maskBufferPtr == 0)
								skipLineFlag = TRUE;
								
							maskBufferPtr += maskColumnStart;
								
							}	// end "if (pointType == kMaskType)"
						
						if (!skipLineFlag)
							{
							ioOutBufferPtr = (HUCharPtr)reformatOptionsPtr->ioOutBufferPtr;
								
									// If this is a polygonal field, initialize the			
									// output buffer to that already existing for that		
									// area.																
									
							if (pointType == kPolygonType || pointType == kMaskType)
								{
								errCode = GetLine (fileIOInstructionsPtr,
															outFileInfoPtr,
															imageLineNumber+1,
															0,
															(UInt32)imageColumnStart+1,
															(UInt32)imageColumnEnd+1,
															&count,
															(HUCharPtr)ioOutBufferPtr);
									
								}	// end "if (pointType == kPolygonType || ..." 
								
						   		// Set line and column number in point						
										
							point.v = (SInt16)line;
							point.h = (SInt16)fieldColumnStart;
	
									// Loop through the number of samples in the line of 	
									// data for this field.											
							
							for (sample=fieldColumnStart; 
									sample<=fieldColumnEnd; 
									sample+=columnInterval)
								{
								includePixelFlag = FALSE;
								if (pointType == kRectangleType)
									includePixelFlag = TRUE;
									
								else if (pointType == kPolygonType && 
																	PtInRgn (point, rgnHandle))
									includePixelFlag = TRUE;
								
								else if (pointType == kMaskType && 
																*maskBufferPtr == maskRequestValue)
									includePixelFlag = TRUE;
									
								if (includePixelFlag)
									*ioOutBufferPtr = (UInt8)statClassNumber;
										
								ioOutBufferPtr++;
								
								point.h += (SInt16)columnInterval;
								
								if (pointType == kMaskType)
									maskBufferPtr += columnInterval;
										
								}	// end "for (sample=fieldColumnStart; ..."

								// Set pointer to correct location for writing.
						
							if (errCode == noErr)	
								errCode = MSetMarker (outFileStreamPtr, 
																fsFromStart, 
																writePosOff,
																kErrorMessages);
				
									// Write buffer to appropriate location in the 			
									// output disk file.												
									
							ioOutBufferPtr = (HUCharPtr)reformatOptionsPtr->ioOutBufferPtr;
							count = numberFieldColumns;
							
							if (errCode == noErr && count > 0)
								errCode = MWriteData (outFileStreamPtr, 
																&count, 
																ioOutBufferPtr,
																kErrorMessages);
															
							}	// end "if (!skipLineFlag)"
							
								// Ready counter for next position to write to file.	
								
						writePosOff += numberImageColumns;
			
						if (pointType == kMaskType)
							savedMaskBufferPtr += numberMaskColumns;
						
								// Update output image line position counter.			
								
						imageLineNumber++;
							
						if (errCode != noErr)
							{
							continueFlag = FALSE;
							break;
							
							}	// end "if (errCode != noErr)" 
							
						}	// end "for (line=areaDescriptionPtr-> ..." 
					
					if (areaDescription.maskInfoPtr != NULL)																					
						CheckAndUnlockHandle (areaDescription.maskInfoPtr->maskHandle);
					
							// Dispose of the region if it exists.							
	
					CloseUpAreaDescription (&areaDescription);
						
					}	// end "if (pointType != kClusterType)" 
					
				fieldCount++;								
								
				}	// end "if (fieldIdentPtr->field..." 
				
			fieldNumber = fieldIdentPtr->nextField;
			
			}	// end "while ((fieldNumber != -1) && ..." 
			
		if (!continueFlag)
			break;
			
		}	// end "for (classIndex=0; classIndex<..." 
	
	CloseUpFileIOInstructions (fileIOInstructionsPtr, NULL);	

	CloseUpGeneralFileIOInstructions (fileIOInstructionsPtr);
					
	return (continueFlag);
		
}	// end "ConvertImagePixelsToClassNumbers" 


	
#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CovertMultispectralToThematic
//
//	Software purpose:	The purpose of this routine is to convert an input
//							multispectral image to a thematic image with a color
//							palette included in an ERDAS trailer file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None 			
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 01/31/1993
//	Revised By:			Larry L. Biehl			Date: 05/26/2011	

Boolean CovertMultispectralToThematic (
				WindowInfoPtr						inputWindowInfoPtr,
				FileInfoPtr							outFileInfoPtr,
				ReformatOptionsPtr				reformatOptionsPtr)
	
{
	PictInfo								thePictInfo;
	
	LongRect								longDestinationRect,
											longSourceRect;
	
	Rect									destinationRect,
											sourceRect;
											
	RGBColor								theColor;
	
	CGrafPtr								savedPort;
	DisplaySpecsPtr					displaySpecsPtr;
											
	UCharPtr								ioBuffer1Ptr,
								 			offScreen8BitBufferPtr;
											
	WindowInfoPtr						windowInfoPtr;
								 			
	GDHandle								savedDevice;
	
	GWorldFlags							GWorldFlags24Bit,
											GWorldFlags8Bit;
	
	Handle								displaySpecsH,
											windowInfoHandle;
	
	PixMapHandle						offScreenMapH8Bit,
											inputOffScreenMapHandle;
	
	SInt32								line,
											sample;
	
	UInt32								count,
											rowBytes;
	
	SInt16								errCode,
											numberColors;
	
	Boolean								continueFlag;
											
				
			// Initialize local variables.												
	
	continueFlag = TRUE;						
	errCode = noErr;								
	displaySpecsH = NULL;
	windowInfoPtr = NULL;
	windowInfoHandle = NULL;
	thePictInfo.thePalette = NULL;
	offScreenMapH8Bit = NULL;
	inputOffScreenMapHandle = (PixMapHandle)inputWindowInfoPtr->offScreenMapHandle;
		
	longSourceRect.top = reformatOptionsPtr->lineStart - 1;
	longSourceRect.bottom = reformatOptionsPtr->lineEnd;
	longSourceRect.left = reformatOptionsPtr->columnStart - 1;
	longSourceRect.right = reformatOptionsPtr->columnEnd;
		
	longDestinationRect.top = 0;
	longDestinationRect.bottom = longSourceRect.bottom - longSourceRect.top;
	longDestinationRect.left = 0;
	longDestinationRect.right = longSourceRect.right - longSourceRect.left;
	
			// Put appropriate string in the status dialog.							
	
	ShowStatusDialogItemSet (kStatusTitle2);
	LoadDItemStringNumber (kReformatStrID,
									3, 
									gStatusDialogPtr, 
									IDC_Status21, 
									(Str255*)gTextString);

	GetGWorld (&savedPort, &savedDevice);
	GWorldFlags24Bit = GetPixelsState ((PixMapHandle)inputOffScreenMapHandle);	
										
	SetGWorld (inputWindowInfoPtr->offscreenGWorld, NULL);
							
	if (LockPixels ((PixMapHandle)inputOffScreenMapHandle))
		{
		errCode = GetPixMapInfo (
							inputOffScreenMapHandle, 
							&thePictInfo, 
							returnPalette+suppressBlackAndWhite,
							reformatOptionsPtr->numberClasses-1,
							reformatOptionsPtr->convertType,
							0);
				
		if (!(GWorldFlags24Bit & pixelsLocked))							
			UnlockPixels (inputOffScreenMapHandle);
				
		}	// end "if (LockPixels (inputOffScreenMapHandle))" 
					
	continueFlag = (errCode == noErr);
		
	if (continueFlag)
		{		
				// List the number of colors that were found.						
		
		sprintf ((char*)gTextString, 
					"\r  %ld unique colors were found.\r",
					thePictInfo.uniqueColors);
		continueFlag = ListString ((char*)gTextString, 
											strlen ((char*)gTextString),
											gOutputTextH);
											
				// List the number of classes that were generated.					
		
		sprintf ((char*)gTextString, 
					"  %d classes were generated (including a background class).\r",
					(SInt16)(*thePictInfo.thePalette)->pmEntries+1);
		continueFlag = ListString ((char*)gTextString, 
											strlen ((char*)gTextString),
											gOutputTextH);
											
		}	// end "if (continueFlag)" 
	
			// Put appropriate string in the status dialog.							
	
	SetGWorld (savedPort, savedDevice);
	LoadDItemStringNumber (kReformatStrID,
									4,
									gStatusDialogPtr,
									IDC_Status21, 
									(Str255*)gTextString);
							
	if (continueFlag)
		{		
				// Get memory for new offscreen map to convert from 24 			
				// bit color to 8 bit color.												
		
		windowInfoHandle = InitializeWindowInfoStructure (
							NULL, kNotPointer, NULL, kThematicImageType, kThematicWindowType);
														
		if (windowInfoHandle != NULL)
			{
			windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
															windowInfoHandle, kLock);
															
			windowInfoPtr->grafPortType = kCGrafType;
		
			displaySpecsH = GetDisplaySpecsStructure ((Handle)NULL, TRUE);
			if (displaySpecsH != NULL)
				{ 
				displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (displaySpecsH, kLock);
															
				displaySpecsPtr->pixelSize = 8;
				displaySpecsPtr->paletteObject = thePictInfo.thePalette;
				displaySpecsPtr->paletteOffset = USHRT_MAX;
				
				}	// end "if (displaySpecsH != NULL)"
			
			}	// end "if (windowInfoHandle != NULL)" 
			
		continueFlag = (windowInfoHandle != NULL);
		
		}	// end "if (continueFlag)" 
	
	if (continueFlag)
		{
		errCode = GetOffscreenGWorld (
						windowInfoPtr, displaySpecsPtr, &longDestinationRect, &rowBytes);
		continueFlag = (errCode == noErr);
		
		}	// end "if (continueFlag)" 
						
	if (continueFlag)
		{													
		offScreenMapH8Bit = (PixMapHandle)windowInfoPtr->offScreenMapHandle;
		GWorldFlags8Bit = GetPixelsState ((PixMapHandle)offScreenMapH8Bit);
	
		if (LockPixels ((PixMapHandle)inputOffScreenMapHandle))
			{
			if (LockPixels ((PixMapHandle)offScreenMapH8Bit))
				{
				SetGWorld (windowInfoPtr->offscreenGWorld, NULL);
				
				destinationRect.left = (SInt16)longDestinationRect.left;
				destinationRect.top = (SInt16)longDestinationRect.top;
				destinationRect.right = (SInt16)longDestinationRect.right;
				destinationRect.bottom = (SInt16)longDestinationRect.bottom;
				
				sourceRect.left = (SInt16)longSourceRect.left;
				sourceRect.top = (SInt16)longSourceRect.top;
				sourceRect.right = (SInt16)longSourceRect.right;
				sourceRect.bottom = (SInt16)longSourceRect.bottom;
		
				CopyBits ((BitMap*)*inputOffScreenMapHandle, 
								(BitMap*)*offScreenMapH8Bit, 
								&sourceRect, 
								&destinationRect, 
								srcCopy, 
								NIL);
				
				}	// end "if (LockPixels (..." 
				
			if (!(GWorldFlags24Bit & pixelsLocked))							
				UnlockPixels (inputOffScreenMapHandle);
				
			}	// end "if (LockPixels (inputOffScreenMapHandle))" 
		
		}	// end "if (continueFlag)" 
	
	if (continueFlag)
		{
				// Update the number of classes in case the number of palette	
				// entries is less than what was requested.							
				// Allow for the first item in the palette to be black.			
				
		outFileInfoPtr->numberClasses = (*thePictInfo.thePalette)->pmEntries + 1;
		outFileInfoPtr->maxClassNumberValue = outFileInfoPtr->numberClasses - 1;
		
				// Store the palette handle in an unused global variable so		
				// that it can be accessed by the routine that writes the 		
				// header.																		
				
		gCharBufferHandle3 = (Handle)thePictInfo.thePalette;
		
				// If everything is okay to here, write out the new output 		
				// file.  Write a header if requested.									
			
		continueFlag = WriteNewImageHeader (NULL,
														outFileInfoPtr,
														(char*)gTextString,
														displaySpecsPtr,
														kFromReformat,
														reformatOptionsPtr->outputFileCode, 
														reformatOptionsPtr->lineInterval,
														reformatOptionsPtr->columnInterval,
														kClassDisplay,
														kNoPaletteColorsDefined);
		
		gCharBufferHandle3 = NULL;
					
		if (continueFlag && outFileInfoPtr->format == kGAIAType)
			{
					// Get buffer to use to create a line of GAIA data.				
					
			count = GetNumberBytesInGAIALine (destinationRect.right);
			gOutputBufferPtr = (HUCharPtr)MNewPointer (count);
			continueFlag = (gOutputBufferPtr != NULL);
			
			if (continueFlag)
				InitializeGAIALineBytes ((UInt16*)gOutputBufferPtr,
													destinationRect.right,
													1);
			
			}	// end "if (continueFlag && ..." 
					
		if (continueFlag)
			{	
			offScreen8BitBufferPtr = 
					(unsigned char*)GetPixBaseAddr ((PixMapHandle)offScreenMapH8Bit);
						
			rowBytes = ((*offScreenMapH8Bit)->rowBytes & 0x7fff);
						
					// Write the converted data to the output file.							
					
			for (line=0; line<destinationRect.bottom; line++)
				{
				count = destinationRect.right;
				
				for (sample=0; sample<count; sample++)
					offScreen8BitBufferPtr[sample]++;
					
				ioBuffer1Ptr = offScreen8BitBufferPtr;
					
				if (outFileInfoPtr->format == kGAIAType)
					{
							// Load the data into GAIA format - 2 byte, byte  				
							// reversed and preline information.								
					
					count = ConvertLineToGAIAFormat (ioBuffer1Ptr,
																(UInt16*)gOutputBufferPtr,
																count,
																line+1);
			
							// The preline information has been set up such that when 	
							// the buffer point to the output data is increase by 1, 	
							// the data will be byte reversed as the GAIA format 			
							// requires.																
									
					ioBuffer1Ptr = (unsigned char*)gOutputBufferPtr;
					ioBuffer1Ptr++;
					
					}	// end "if (outFileInfoPtr->format == kGAIAType)" 
				
				CMFileStream* outFileStreamPtr = GetFileStreamPointer (outFileInfoPtr);	
				errCode = MWriteData (outFileStreamPtr, 
												&count, 
												ioBuffer1Ptr,
												kErrorMessages);
				if (errCode != noErr)
					break;
					
				offScreen8BitBufferPtr += rowBytes;
				
				}	// end "for (line=0; line<destinationRect.bottom; line++)" 
		
			}	// end "if (continueFlag)" 
									
		UnlockPixels ((PixMapHandle)offScreenMapH8Bit);
		
		gOutputBufferPtr = (HUCharPtr)CheckAndDisposePtr ((Ptr)gOutputBufferPtr);
			
		if (continueFlag)
			{
	
					// Put appropriate string in the status dialog.					
	
			SetGWorld (savedPort, savedDevice);
			LoadDItemStringNumber (kReformatStrID,
											5, 
											gStatusDialogPtr, 
											IDC_Status21, 
											(Str255*)gTextString);
							
			continueFlag = CreateThematicSupportFile (
											outFileInfoPtr,
											NULL, 
											(*thePictInfo.thePalette)->pmEntries, 
											NULL,
											0,
											NULL, 
											thePictInfo.thePalette,
											NULL, 
											0,
											(*thePictInfo.thePalette)->pmEntries, 
											0,
											kPaletteOnly,
											kClassDisplay,
											kCollapseClass,
											kITRLFileType);
											
			}	// end "if (continueFlag && ..." 
		
		}	// end "if (continueFlag)" 
		
	DisposeOfImageWindowSupportMemory (windowInfoPtr);
	displaySpecsH = UnlockAndDispose (displaySpecsH);
	windowInfoHandle = UnlockAndDispose (windowInfoHandle);
	
	if (continueFlag && thePictInfo.thePalette && 
													reformatOptionsPtr->channelDescriptions)
		{
				// Write a copy of the color palette to the output text window.
	
		LoadDItemStringNumber (kReformatStrID, 
										6, 
										gStatusDialogPtr, 
										IDC_Status21, 
										(Str255*)gTextString);
		
		numberColors = (*thePictInfo.thePalette)->pmEntries;
		
		sprintf ((char*)gTextString, "   0, 255, 255, 255\r");
		continueFlag = ListString ((char*)gTextString, 20, gOutputTextH);
		
		for (sample=0; sample<numberColors; sample++)
			{		
			GetEntryColor (thePictInfo.thePalette, sample, &theColor);
					
			theColor.red = (theColor.red >> 8);
			theColor.green = (theColor.green >> 8);
			theColor.blue = (theColor.blue >> 8);
			
			sprintf ((char*)gTextString,
							"%4ld,%4d,%4d,%4d\r", 
							sample+1, 
							theColor.red, 
							theColor.green,
							theColor.blue);
				
			if (continueFlag)		
				continueFlag = ListString ((char*)gTextString, 20, gOutputTextH);
				
			}	// end "for (sample=0; sample<numberColors; sample++)" 
		
		sprintf ((char*)gTextString, "%4d,   0,   0,   0\r", numberColors+1);
		continueFlag = ListString ((char*)gTextString, 20, gOutputTextH);
		
		}	// end "if (continueFlag && thePictInfo.thePalette ...)" 
	
	if (thePictInfo.thePalette)
		DisposePalette (thePictInfo.thePalette);
		
	SetGWorld (savedPort, savedDevice);
		
	return (continueFlag);
				
}	// end "CovertMultispectralToThematic" 	
#endif		// defined multispec_mac


	
#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				void CovertMultispectralToThematicControl
//
//	Software purpose:	The purpose of this routine is to convert an input
//							multispectral image to a thematic image with a color
//							palette included in an ERDAS trailer file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/31/1993
//	Revised By:			Larry L. Biehl			Date: 03/10/2014	

void CovertMultispectralToThematicControl (
				WindowInfoPtr						inputWindowInfoPtr)
	
{
	Rect									sourceRect;
	
	FileInfoPtr							outFileInfoPtr;
	ReformatOptionsPtr				reformatOptionsPtr;
		
	Handle								outputFileInfoHandle,
											reformatOptionsH;
	
	PixMapHandle						inputOffScreenMapHandle;
	
	time_t								startTime;
	
	Boolean								continueFlag;
	
	
			// If spare memory had to be used to load code resources, then 	
			// exit routine.																	
			
	if (gMemoryTypeNeeded < 0)
																						return;
																							
			// Code resources loaded okay, so set flag back for non-Code		
			// resources.																		
			
	gMemoryTypeNeeded = 0;
	
	if (inputWindowInfoPtr == NULL)
																						return;
	
			// Initialize local variables.												
										
	outFileInfoPtr = NULL;
	outputFileInfoHandle = NULL;
	reformatOptionsH = NULL;
	inputOffScreenMapHandle = (PixMapHandle)inputWindowInfoPtr->offScreenMapHandle;
	sourceRect = ((PixMap*)*inputOffScreenMapHandle)->bounds;
	
	continueFlag = GetReformatAndFileInfoStructures (
											&outputFileInfoHandle, &reformatOptionsH);
	
	if (continueFlag)
		{		
				// Set up some information for the reformat structure.			
				
		reformatOptionsPtr = (ReformatOptionsPtr)GetHandlePointer (
																			reformatOptionsH, kLock);
		reformatOptionsPtr->numberChannels = 1;
		
		reformatOptionsPtr->numberClasses = 253;
		reformatOptionsPtr->convertType = 2;
		reformatOptionsPtr->headerFormat = kErdas74Type;
		
		continueFlag = MultispectralToThematicDialog (reformatOptionsPtr);
		
		}	// end "if (continueFlag)" 
									
	if (continueFlag)
		{		
				// List the processor name, date and time.								

		gOutputForce1Code = 0x0001;
		continueFlag = ListHeaderInfo (NULL, 
													0, 
													&gOutputForce1Code, 
													kNoStatisticsUsed, 
													continueFlag);
		
		}	// end "if (continueFlag)" 
									
	if (continueFlag)
		{											
				// Set up some information for the output file							
				
		outFileInfoPtr = (FileInfoPtr)GetHandlePointer (outputFileInfoHandle, kLock);
		outFileInfoPtr->numberClasses = reformatOptionsPtr->numberClasses;
		outFileInfoPtr->maxClassNumberValue = outFileInfoPtr->numberClasses - 1;
		outFileInfoPtr->thematicType = TRUE;
														
				// Update parameters in the structure for the output file.			
		
		UpdateOutputFileStructure (
					outFileInfoPtr, 
					reformatOptionsPtr, 
					1, 
					1, 
					GetFileMapProjectionHandle (inputWindowInfoPtr->fileInfoHandle));
		
				// Set up the disk file to be used for the new reformatted			
				// image file.																		
					
		continueFlag = !GetReformatOutputFile (outFileInfoPtr, reformatOptionsPtr);
		
		}	// end "if (continueFlag)" 
		
			// Get the best palette for the requested number of colors.				
	
	if (continueFlag)
		{				
		MSetCursor (kWait);
		
		startTime = time (NULL);
					
				// Get status dialog. Also call routine to do							
				// activate/update events.														
			
		gStatusDialogPtr = GetStatusDialog (kUpdateStatsInfoID, TRUE);
		continueFlag = (gStatusDialogPtr != NULL);
		
		if (continueFlag)
			{
			ShowStatusDialogItemSet (kStatusTitle1);
			LoadDItemStringNumber (kReformatStrID,
											2, 
											gStatusDialogPtr, 
											IDC_Status11, 
											(Str255*)gTextString);
							
			}	// end "if (continueFlag)" 
		
		if (continueFlag)
			continueFlag = CovertMultispectralToThematic (inputWindowInfoPtr,
																			outFileInfoPtr, 
																			reformatOptionsPtr);
			
		if (continueFlag)				
			LoadDItemStringNumber (kReformatStrID, 
											20, 
											gStatusDialogPtr, 
											IDC_Status21, 
											(Str255*)gTextString);
					
				// List Reformat information.												
		
		if (continueFlag)		
			continueFlag = ListReformatResultsInformation (
																reformatOptionsPtr, outFileInfoPtr);
	
				// List the CPU time taken for reformatting.							
			
		continueFlag = ListCPUTimeInformation (NULL, continueFlag, startTime);
		
		}	// end "if (continueFlag)" 
											
	CloseFile (outFileInfoPtr);
	
	UnlockAndDispose (outputFileInfoHandle);
	UnlockAndDispose (reformatOptionsH);
			
			// Scroll output window to the selection and adjust the 				
			// scroll bar.																		
	
	UpdateOutputWScrolls (gOutputWindow, 1, kDisplayMessage);
								
			// Dispose of status dialog													
	
	CloseStatusDialog (TRUE);
		
	MInitCursor ();
				
}	// end "CovertMultispectralToThematicControl"	
#endif	// defined multispec_mac



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawDiskFileFormatPopUp
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
//	Coded By:			Larry L. Biehl			Date: 03/06/2014
//	Revised By:			Larry L. Biehl			Date: 03/10/2014	

pascal void DrawDiskFileFormatPopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.									
			
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpDiskFileFormatMenu, 
								gOutputFormatCode, 
								TRUE);
	
}	// end "DrawDiskFileFormatPopUp" 
#endif	// defined multispec_mac


	
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ENVI_ROIToThematicFileControl
//
//	Software purpose:	The purpose of this routine is to control the 
//							conversion of the ENVI ASCII ROI file to a
//							Thematic image file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/11/1997
//	Revised By:			Larry L. Biehl			Date: 04/16/2020

void ENVI_ROIToThematicFileControl (void)

{
	//CMFileStream						inputFileStream;
	MFileInfo							inputFileInfo;
	HParamBlockRec						paramBlock;
	UInt8									inputBuffer[256];
	
	UCharPtr								inputStringPtr;
	CMFileStream*						inputFileStreamPtr;
	ColorSpec*							colorSpecPtr = NULL;
	FileInfoPtr							outFileInfoPtr;
	Handle								reformatOptionsHandle;
	ReformatOptionsPtr				reformatOptionsPtr;
	UCharPtr								classNamePtr;
	
	UInt32								count,
											numberBytes,
											numberEndOfLineBytes;
	
	time_t								startTime;
	
	SInt16								asciiFormatCode,
											endOfLineCode,
											errCode;								
	
	Boolean								changedFlag,
											continueFlag;
	
	
			// If spare memory had to be used to load code resources, then exit	
			// routine.																				
			
	if (gMemoryTypeNeeded < 0)
																							return;
																							
			// Code resources loaded okay, so set flag back for non-Code			
			// resources.																			
			
	gMemoryTypeNeeded = 0;
																							
			// Get the reformat structure and initialize it.							
			
	if (!ENVI_ROIToThematicLoadSpecs (&reformatOptionsHandle))
																							return;
	
			// Assume earlier format to start with.
			
	asciiFormatCode = 0;
	
			// Note that the following handles were locked in the call to
			// 'ENVI_ROIToThematicLoadSpecs'
			
	reformatOptionsPtr = (ReformatOptionsPtr)GetHandlePointer (
										reformatOptionsHandle);

	outFileInfoPtr = (FileInfoPtr)GetHandlePointer (
														reformatOptionsPtr->outFileInfoHandle);
					
			// Get memory to store the class names into. Allow for 255 classes
			// plus the background class.
	
	classNamePtr = (UCharPtr)CheckHandleSize (
										&outFileInfoPtr->classDescriptionH, 
										&continueFlag, 
										&changedFlag, 
										256 * 32);

	if (continueFlag)
		{
				// Load the background class name in.
		
		classNamePtr[0] = 10;		
		BlockMoveData ("background", (Ptr)&classNamePtr[1], 10);
		classNamePtr += 32;
				
		//inputFileStreamPtr = &inputFileStream;
		InitializeFileInfoStructure (&inputFileInfo, kPointer);
		inputFileStreamPtr = inputFileInfo.fileStreamCPtr;
		//InitializeFileStream (inputFileStreamPtr);

				// Allow the user to select the ENVI ROI text file.
		
		inputStringPtr = NULL;
		continueFlag = ENVI_ROIToThematicGetASCIIFile (inputFileStreamPtr,
																		outFileInfoPtr, 
																		reformatOptionsPtr,
																		&paramBlock,
																		inputBuffer,
																		256,
																		&inputStringPtr,
																		&endOfLineCode,
																		&numberEndOfLineBytes,
																		&asciiFormatCode);
															
		}	// end "if (continueFlag)"
	
   startTime = time (NULL);
	if (continueFlag)
		{
				// Update parameters in the structure for the output file.				
		
		UpdateOutputFileStructure (outFileInfoPtr, reformatOptionsPtr, 1, 1, NULL);
		
				// Get buffer for thematic file. Store entire image in memory.
				// When getting the buffer, make sure there is enough to right
				// a 128 byte header.
	
		numberBytes = outFileInfoPtr->numberColumns * outFileInfoPtr->numberLines;
		reformatOptionsPtr->ioOutBufferPtr = 
												(HUCharPtr)MNewPointer (MAX (numberBytes, 128));
		if (reformatOptionsPtr->ioOutBufferPtr != NULL)
			{
			reformatOptionsPtr->countOutBytes = numberBytes;
			reformatOptionsPtr->ioOutBufferBytes = numberBytes;
			
			}	// end "if (reformatOptionsPtr->ioOutBufferPtr != NULL)"
		
				// Set up the disk file to be used for the new reformatted				
				// image file.																			

		gImageFileInfoPtr = &inputFileInfo;
		continueFlag = !GetReformatOutputFile (outFileInfoPtr, reformatOptionsPtr);
		gImageFileInfoPtr = NULL;
		
				// Get memory for a color table. Allow space for the maximum number
				// of classes ... 256
				
		if (continueFlag)
			{
			colorSpecPtr = (ColorSpec*)MNewPointer (256 * sizeof (ColorSpec));
			
			if (colorSpecPtr != NULL)
				{
						// Set the background color to white.
						
				colorSpecPtr[0].value = 0;
				#ifndef multispec_wx
					colorSpecPtr[0].rgb.red = (255 << 8);
					colorSpecPtr[0].rgb.green = (255 << 8);
					colorSpecPtr[0].rgb.blue = (255 << 8);
				#else
					colorSpecPtr[0].rgb.red = 255;
					colorSpecPtr[0].rgb.green = 255;
					colorSpecPtr[0].rgb.blue = 255;
				#endif
					
				}	// end "if (colorSpecPtr != NULL)"
				
			else	// colorSpecPtr == NULL
				continueFlag = FALSE;
			
			}	// end "if (continueFlag)"
				
		if (continueFlag)													
			{
					// List the processor name, date and time.							
				
			gOutputForce1Code = 0x0001;
			continueFlag = ListHeaderInfo (NULL, 
														kLInputParametersLineOnly, 
														&gOutputForce1Code, 
														kNoStatisticsUsed, 
														continueFlag);
							
		if (continueFlag)													
			{							
					// List name of the input ROI file.
					
			char* fileNamePtr =
								(char*)GetFileNameCPointerFromFileStream (inputFileStreamPtr);
			sprintf ((char*)gTextString, 
							"    ROI text file = '%s'%s",
							fileNamePtr,
							gEndOfLine);
			
			continueFlag = OutputString (inputFileStreamPtr, 
													(char*)gTextString, 
													0, 
													gOutputForce1Code,
													continueFlag,
													kUTF8CharString);
			
			}	// end "if (continueFlag)" 
												
					// Change cursor to watch cursor until done with process.				
			
			MSetCursor (kWait);
		
				// Get the status dialog window.	
				
			gStatusDialogPtr = GetStatusDialog (kUpdateStatsInfoID, TRUE);
			continueFlag = (gStatusDialogPtr != NULL);
			
			}	// end "if (continueFlag)"
			
		if (continueFlag)
			{
			ShowStatusDialogItemSet (kStatusTitle1);
			
					//	Title for creating background image.										
	
			LoadDItemStringNumber (kReformatStrID,
											IDS_Reform13, 	// "\pCreating Thematic Image Background"
											gStatusDialogPtr, 
											IDC_Status11,
											(Str255*)gTextString);
						
			CheckSomeEvents (updateMask + activMask);
				
					// Create the base Thematic Image file with all background values.	
				
			HUInt8Ptr bufferPtr = reformatOptionsPtr->ioOutBufferPtr;
			UInt8 backgroundValue = (UInt8)reformatOptionsPtr->backgroundValue;
			
			while (numberBytes != 0)
				{
				*bufferPtr = backgroundValue;
				bufferPtr++;
				numberBytes--;
				
				}	// end "while (numberBytes != 0)"
	
					//	Set title and dialog items for changing pixels to class numbers.										
	
			LoadDItemStringNumber (kReformatStrID,
											IDS_Reform29, 	// "\pConverting ENVI ASCII ROI to Thematic Image"
											gStatusDialogPtr, 
											IDC_Status11,
											(Str255*)gTextString);	
	
			CheckSomeEvents (updateMask);
				
					// Now change those pixels that belong to one of the requested			
					// project classes.		
																				
			if (asciiFormatCode == 1)
				continueFlag = ENVI_ROIToThematicConvertPixelsToClassNumbers1 (
																						outFileInfoPtr,
																						reformatOptionsPtr,
																						&paramBlock,
																						inputBuffer,
																						inputStringPtr,
																						classNamePtr,
																						colorSpecPtr);
			
			else	// asciiFormatCode == 2
				continueFlag = ENVI_ROIToThematicConvertPixelsToClassNumbers2 (
																						outFileInfoPtr,
																						reformatOptionsPtr,
																						&paramBlock,
																						inputBuffer,
																						inputStringPtr,
																						classNamePtr,
																						colorSpecPtr);
			
			}	// end "if (continueFlag)" 
		
		if (continueFlag)
			{
					// Write a Erdas header if requested.
					
			if (reformatOptionsPtr->headerFormat == kErdas74Type)
				continueFlag = WriteNewErdasHeader (
													outFileInfoPtr, 
													(SInt16*)inputBuffer, 
													kErdas74Type);
				
					// Write buffer to the output disk file.
					
			count = reformatOptionsPtr->countOutBytes;
			CMFileStream* outFileStreamPtr = GetFileStreamPointer (outFileInfoPtr);
			errCode = MWriteData (outFileStreamPtr, 
												&count, 
												reformatOptionsPtr->ioOutBufferPtr,
												kErrorMessages);
			
			if (errCode != noErr)									
				continueFlag = FALSE;
												
			}	// end "if (continueFlag)"
			
				// The input file is not needed any more.
				
		CloseFile (inputFileStreamPtr); 
		
		CloseStatusDialog (TRUE);
																
				// Create the ERDAS Trailer file if needed.									
		
		if (continueFlag && reformatOptionsPtr->headerFormat == kErdas74Type)
			{
			continueFlag = CreateThematicSupportFile (
														outFileInfoPtr,
														NULL,
														(UInt16)outFileInfoPtr->numberClasses,
														NULL,
														0,
														colorSpecPtr,
														NULL,
														NULL,
														0,
														(UInt16)outFileInfoPtr->numberClasses,
														kFromDescriptionCode,
														kPaletteHistogramClassNames,
														kClassDisplay,
														kCollapseClass,
														kITRLFileType);
			
			}	// end "if (continueFlag && ...)" 
			
		CheckAndDisposePtr ((Ptr)colorSpecPtr);
				  		
	  					// Close the output file.													
	  				
		CloseFile (outFileInfoPtr);
		
		}	// end "if (continueFlag)"
					
			// List Reformat information.												
	
	if (continueFlag)		
		continueFlag = ListReformatResultsInformation (
														reformatOptionsPtr, outFileInfoPtr);

			// List the CPU time taken for reformatting.							
		
	continueFlag = ListCPUTimeInformation (NULL, continueFlag, startTime);
				
	
	ReleaseReformatOutputFileInfoAndBuffers (reformatOptionsPtr, NULL);
					
			// Dispose of updating statistics status dialog box.						
		
	CloseStatusDialog (TRUE);
			
	MInitCursor ();
		
}	// end "ENVI_ROIToThematicFileControl" 


							
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ENVI_ROIToThematicGetASCIIFile
//
//	Software purpose:	The purpose of this routine is to convert all pixels
//							in the specified image file that are listed in the
//							ENVI ASCII ROI file to the respective class number. The
//							class number is assigned by increasing value starting at
//							1. The pixels in the input image file will have been set 
//							to a background value.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			ENVI_ROIToThematicFileControl
//
//	Coded By:			Larry L. Biehl			Date: 07/24/2011
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

Boolean ENVI_ROIToThematicGetASCIIFile (
				CMFileStream*						inputFileStreamPtr,
				FileInfoPtr							outFileInfoPtr, 
				ReformatOptionsPtr				reformatOptionsPtr,
				HParamBlockRec*					paramBlockPtr,
				UCharPtr								bufferPtr,
				UInt32								bufferLength,
				UCharPtr*							inputStringPtrPtr,
				SInt16*								endOfLineCodePtr,
				UInt32*								numberEndOfLineBytesPtr,
				SInt16*								asciiFormatCodePtr)
			
{
	SInt32								numberClasses;
	
	UInt32								numberColumns,
											numberLines;
	
	SInt16								errCode,
											returnCode;
										
	Boolean								continueFlag;


			// Check input parameters.															
			
	if (outFileInfoPtr == NULL || reformatOptionsPtr == NULL)
																					return (FALSE);
																				
	continueFlag = TRUE;
	errCode = noErr;
																					
	if (continueFlag)
		{	
				// Get the input file.
				
		errCode = GetFile (inputFileStreamPtr, 
										-1, 
										NULL, 
										NULL, 
										NULL,
										NULL, 
										IDS_FileIO146);
										
		continueFlag = (errCode == noErr) & FileExists (inputFileStreamPtr);
		
		}	// end "if (continueFlag)"	
		
			// If file has been set up, then prepare to start reading the text
			// file one line at a time.																		

	if (continueFlag)
		{
		errCode = PrepareToReadTextFile (paramBlockPtr,
														inputFileStreamPtr,
														bufferPtr,
														bufferLength,
														endOfLineCodePtr,
														numberEndOfLineBytesPtr);
														
		continueFlag = (errCode == noErr);
					
		}	// end "if (continueFlag)"  
	
	if (continueFlag)
		{
				// Now determine whether this is the older or newer format for ROI's
				
		errCode = GetNextLine ((ParmBlkPtr)paramBlockPtr, inputStringPtrPtr);
		continueFlag = (errCode == noErr);
		if (continueFlag)
			{
			if (CompareStringsNoCase (
							*inputStringPtrPtr, (UCharPtr)"; ENVI Output of ROIs:", 19) == 0)
				*asciiFormatCodePtr = 2;
			
			else	// Maybe older format
				*asciiFormatCodePtr = 1;

			}	// end "if (continueFlag)"
			
		}	// end "if (continueFlag)"
			
	if (*asciiFormatCodePtr == 1)
		{
				// Assuming that the file is of syle 1
				// Check if a File Dimension line is at the beginning of the file.
				
		returnCode = sscanf ((char*)*inputStringPtrPtr,
										#ifndef multispec_wx
											"File Dimension: %ld x %ld",
										#else
											"File Dimension: %d x %d",
										#endif
										&numberColumns,
										&numberLines);
										
		if (returnCode == 2)
			{
			reformatOptionsPtr->columnEnd = numberColumns;
			reformatOptionsPtr->lineEnd = numberLines;
			
			}	// end "if (returnCode == 2)"
			
		else	// returnCode != 2
			{
					// File Dimension line was not there.
					// Reset to start reading at the beginning of the file again.
				
			#if defined multispec_mac
				paramBlockPtr->ioParam.ioPosOffset = 0;
			#endif	// defined multispec_mac
		
			#if defined multispec_win || defined multispec_wx
				paramBlockPtr->ioPosOffset = 0;
			#endif	// defined multispec_win 
			
			*inputStringPtrPtr = NULL;
			
			}	// end "else returnCode != 2"
		
		}	// end "if (*asciiFormatCodePtr == 1)"
			
	if (*asciiFormatCodePtr == 2)
		{
				// Get number lines, columns and classes from the beginning of the file.

		errCode = GetNextLine ((ParmBlkPtr)paramBlockPtr, inputStringPtrPtr);
		continueFlag = (errCode == noErr);
			
		if (continueFlag)
			{
			returnCode = sscanf ((char*)*inputStringPtrPtr, 											
											#ifndef multispec_wx
												"; Number of ROIs: %ld",
											#else
												"; Number of ROIs: %d",
											#endif
											&numberClasses);
												
			continueFlag = (returnCode == 1);
			
			}	// end "if (continueFlag)"
			
		if (continueFlag)
			{
			errCode = GetNextLine ((ParmBlkPtr)paramBlockPtr, inputStringPtrPtr);
			continueFlag = (errCode == noErr);
			
			if (continueFlag)
				{
				returnCode = sscanf ((char*)*inputStringPtrPtr,
												#ifndef multispec_wx
													"; File Dimension: %ld x %ld",
												#else
													"; File Dimension: %d x %d",
												#endif
												&numberColumns,
												&numberLines);
										
				if (returnCode == 2)
					{
					reformatOptionsPtr->columnEnd = numberColumns;
					reformatOptionsPtr->lineEnd = numberLines;
					
					}	// end "if (returnCode == 2)"

				continueFlag = (returnCode == 2);

				}	// end "if (continueFlag)"
			
			}	// if (continueFlag)
			
		if (continueFlag)
			outFileInfoPtr->numberClasses = MIN (numberClasses, 255);

		}	// end "if (*asciiFormatCodePtr == 2)"
		
	return (continueFlag);
		
}	// end "ENVI_ROIToThematicGetASCIIFile" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ENVI_ROIToThematicConvertPixelsToClassNumbers1
//
//	Software purpose:	The purpose of this routine is to convert all pixels
//							in the specified image file that are listed in the
//							ENVI ASCII ROI file to the respective class number. The
//							class number is assigned by increasing value starting at
//							1. The pixels in the input image file will have been set 
//							to a background value.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			ENVI_ROIToThematicFileControl
//
//	Coded By:			Larry L. Biehl			Date: 03/11/1997
//	Revised By:			Larry L. Biehl			Date: 03/17/2020

Boolean ENVI_ROIToThematicConvertPixelsToClassNumbers1 (
				FileInfoPtr							outFileInfoPtr, 
				ReformatOptionsPtr				reformatOptionsPtr,
				HParamBlockRec*					paramBlockPtr,
				UCharPtr								inputBuffer,
				UCharPtr								inputStringPtr,
				UCharPtr								classNamePtr,
				ColorSpec*							colorSpecPtr)
			
{
	UCharPtr								roiName = (UCharPtr)"ROI Name",
											roiPixel = (UCharPtr)"(";

	HUInt8Ptr							outputImageBuffer;

	UInt32								classNumber,
											column,
											index,
											line,
											numberImageColumns,
											numberImageLines;
	
	SInt16								errCode,
											returnCode,
											stringCompare;
											
	UInt16								blue,
											green,
											red;
										
	Boolean								continueFlag,
											notDoneFlag;


			// Check input parameters.															
			
	if (outFileInfoPtr == NULL || reformatOptionsPtr == NULL)
																					return (FALSE);
																				
	continueFlag = TRUE;
	errCode = noErr;
	numberImageColumns = outFileInfoPtr->numberColumns;
	numberImageLines  = outFileInfoPtr->numberLines;
	outputImageBuffer = reformatOptionsPtr->ioOutBufferPtr;
	classNumber = 0;
																													
	notDoneFlag = TRUE;
	while (notDoneFlag)
		{			
		errCode = GetNextLine ((ParmBlkPtr)paramBlockPtr, &inputStringPtr);
		if (errCode == eofErr)
			{
			notDoneFlag = FALSE;
			break;
			
			}	// end "if (errCode == eofErr)"
			
		continueFlag = (errCode == noErr);
	
		if (continueFlag)
			{
					// If the first character is the newline character, skip it.
					
			if (*inputStringPtr == '\n')
				inputStringPtr++;
			
			stringCompare = strncmp ((char*)inputStringPtr, (char*)roiName, 8);
			
			if (stringCompare == 0)
				{
						// Make sure that we are not over the limit of class numbers.
						
				if (classNumber < 255)
					{
					classNumber++;
					ENVI_ROIToThematicGetClassName (inputStringPtr, classNamePtr);
					classNamePtr += 32;
					
					}	// end "if (classNumber < 255)"
						
				else	// classNumber == 255)
					{
					notDoneFlag = FALSE;
					break;
					
					}	// else classNumber == 255

				}	// end "if (stringCompare == 0)"
				
			else if (strncmp ((char*)inputStringPtr, (char*)"ROI Color", 9) == 0)
				{
				returnCode = sscanf ((char*)inputStringPtr, 
												"ROI Color: (%hd,%hd,%hd)",
												&red, 
												&green,
												&blue);
				
				if (returnCode == 3)	
					{
					colorSpecPtr[classNumber].value = (SInt16)classNumber;
					#ifndef multispec_wx
						colorSpecPtr[classNumber].rgb.red = (red << 8);
						colorSpecPtr[classNumber].rgb.green = (green << 8);
						colorSpecPtr[classNumber].rgb.blue = (blue << 8);
					#else
						colorSpecPtr[classNumber].rgb.red = red;
						colorSpecPtr[classNumber].rgb.green = green;
						colorSpecPtr[classNumber].rgb.blue = blue;
					#endif
               					
					}	// end "if (returnCode == 3)"
					
				else	// returnCode != 3
					continueFlag = FALSE;
				
				}	// end "else if (strncmp (..."
				
			else if (strncmp ((char*)inputStringPtr, (char*)roiPixel, 1) == 0)
				{
				returnCode = sscanf ((char*)inputStringPtr, 
											//#ifndef multispec_wx
											//	"( %d ) , ( %d )",
											//#else
												"( %d ) , ( %d )",
											//#endif
											&column,
											&line);
										
				if (returnCode != 2)	
					continueFlag = FALSE;
					
				if (continueFlag)
					{
					if (column <= numberImageColumns && line <= numberImageLines)
						{
						index = numberImageColumns * (line - 1) + column - 1;
						outputImageBuffer[index] = (UInt8)classNumber;
						
						}	// end "if (column <= numberImageColumns && ..."
						
					else	// column > numberImageColumns || ...
						continueFlag = FALSE;
					
					}	// end "if (continueFlag)"
				
				}	// end "else if (strncmp (..."
				
			}	// end "if (continueFlag)"
		
		if (!continueFlag)	
			notDoneFlag = FALSE;
			
		}	// end "while (notDoneFlag)"
		
	if (continueFlag)
		{
				// Save the number of classes. Allow for the background class.
				 
		outFileInfoPtr->numberClasses = classNumber + 1;
		outFileInfoPtr->maxClassNumberValue = outFileInfoPtr->numberClasses - 1;
		
		}	// end "if (continueFlag)"
					
	return (continueFlag);
		
}	// end "ENVI_ROIToThematicConvertPixelsToClassNumbers1"


							
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ENVI_ROIToThematicConvertPixelsToClassNumbers2
//
//	Software purpose:	The purpose of this routine is to convert all pixels
//							in the specified image file that are listed in version 2 of the
//							ENVI ASCII ROI file to the respective class number. The
//							class number is assigned by increasing value starting at
//							1. The pixels in the input image file will have been set 
//							to a background value.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			ENVI_ROIToThematicFileControl
//
//	Coded By:			Larry L. Biehl			Date: 07/26/2011
//	Revised By:			Larry L. Biehl			Date: 04/17/2020

Boolean ENVI_ROIToThematicConvertPixelsToClassNumbers2 (
				FileInfoPtr							outFileInfoPtr, 
				ReformatOptionsPtr				reformatOptionsPtr,
				HParamBlockRec*					paramBlockPtr,
				UCharPtr								inputBuffer,
				UCharPtr								inputStringPtr,
				UCharPtr								classNamePtr,
				ColorSpec*							colorSpecPtr)
			
{
	UInt32*								pointsPerClassVector;
	HUInt8Ptr							outputImageBuffer;
	
	UInt32								classNumber,
											classPixelCount,
											column,
											index,
											line,
											numberClasses,
											numberImageColumns,
											numberImageLines;
	
	SInt16								errCode,
											returnCode;
											
	UInt16								blue,
											green,
											red;
	
	Boolean								continueFlag,
											lineNotFoundFlag,
											notDoneFlag;


			// Initialize some local variables.
			
	numberClasses = outFileInfoPtr->numberClasses;
	numberImageColumns = outFileInfoPtr->numberColumns;
	numberImageLines  = outFileInfoPtr->numberLines;
	outputImageBuffer = reformatOptionsPtr->ioOutBufferPtr;
	continueFlag = TRUE;

			// Get memory for storage of the number of points per class and the
			// class color.
	
	pointsPerClassVector = (UInt32*)MNewPointer (numberClasses*sizeof (UInt32));
	
	if (pointsPerClassVector != NULL)
		{	
		for (index=0; index<numberClasses; index++)
			{
					// Find line with ; ROI name:
			
			lineNotFoundFlag = TRUE;
			while (continueFlag && lineNotFoundFlag)
				{
				errCode = GetNextLine ((ParmBlkPtr)paramBlockPtr, &inputStringPtr);
			
				if (errCode == noErr)
					{
					if (CompareStringsNoCase (
												inputStringPtr, (UCharPtr)"; ROI name:", 11) == 0)
						{
						ENVI_ROIToThematicGetClassName (inputStringPtr, classNamePtr);
						classNamePtr += 32;
						lineNotFoundFlag = FALSE;
						
						}	// end "if (errCode == noErr && ..."
						
					}	// end "if (errCode == noErr)"
					
				else	// errCode != noErr
					continueFlag = FALSE;

				}	// end "while (continueFlag && lineNotFoundFlag)"
				
			if (continueFlag)
				{
				errCode = GetNextLine ((ParmBlkPtr)paramBlockPtr, &inputStringPtr);
				continueFlag = (errCode == noErr);
			
					// Find line with ; ROI rgb value:
					
				if (continueFlag && CompareStringsNoCase (
											inputStringPtr, (UCharPtr)"; ROI rgb value:", 16) == 0)
					{
					returnCode = sscanf ((char*)inputStringPtr, 
													"; ROI rgb value: {%hd, %hd, %hd}",
													&red,
													&green,
													&blue);
														
					if (returnCode == 3)
						{
						colorSpecPtr[index+1].value = (SInt16)(index + 1);
						#ifndef multispec_wx
							colorSpecPtr[index+1].rgb.red = (red << 8);
							colorSpecPtr[index+1].rgb.green = (green << 8);
							colorSpecPtr[index+1].rgb.blue = (blue << 8);
						#else
							colorSpecPtr[index+1].rgb.red = red;
							colorSpecPtr[index+1].rgb.green = green;
							colorSpecPtr[index+1].rgb.blue = blue;    
						#endif
						
						}	// end "if (returnCode == 3)"
						
					else	// returnCode != 3
						continueFlag = FALSE;

					}	// end "if (continueFlag && CompareStringsNoCase (inputStringPtr, ..."
				
				}	// end "if (continueFlag)"
				
			if (continueFlag)
				{
				errCode = GetNextLine ((ParmBlkPtr)paramBlockPtr, &inputStringPtr);
				continueFlag = (errCode == noErr);
			
					// Find line with ; ROI npts:
					
				if (continueFlag && CompareStringsNoCase (
												inputStringPtr, (UCharPtr)"; ROI npts:", 11) == 0)
					{
					returnCode = sscanf ((char*)inputStringPtr, 
													"; ROI npts: %d",
													&pointsPerClassVector[index]);
														
					continueFlag = (returnCode == 1);

					}	// end "if (CompareStringsNoCase (inputStringPtr, ..."
				
				}	// end "if (continueFlag)"
				
			if (!continueFlag)
				break;
			
			}	// end "for (index=0; index<numberClasses; index++)"
						
		if (continueFlag)
			{
					// Skip the next line. It should contain ";   X    Y"
				
			errCode = GetNextLine ((ParmBlkPtr)paramBlockPtr, &inputStringPtr);
			continueFlag = (errCode == noErr);
				
					// Get the points for each class.
					
			notDoneFlag = TRUE;
			classPixelCount = 0;
			classNumber = 1;
			while (continueFlag && notDoneFlag)
				{			
				errCode = GetNextLine ((ParmBlkPtr)paramBlockPtr, &inputStringPtr);
			
				if (errCode == noErr)
					{
					returnCode = sscanf ((char*)inputStringPtr, 
												//#ifndef multispec_wx
												//	"%d %d\r",
												//#else
													"%d %d\r",
												//#endif
												&column, 
												&line);
												
					if (returnCode == 2)	
						{
						if (column <= numberImageColumns && line <= numberImageLines)
							{
							index = numberImageColumns * (line - 1) + column - 1;
							outputImageBuffer[index] = (UInt8)classNumber;
							
							}	// end "if (column <= numberImageColumns && ..."
							
						else	// column > numberImageColumns || ...
							continueFlag = FALSE;
							
						classPixelCount++;
						
						}	// end "if (returnCode == 2)"
						
					else	// returnCode != 2
						{
								// This should be the blank line between the class points.
								// Continue if it is.
								
						if (classPixelCount != 0)
							continueFlag = FALSE;
							
						}	// end "else returnCode != 2"
					
					}	// end "if (errCode == noErr)"
					
				else	// errCode != noErr
					{
					if (errCode != eofErr)
						continueFlag = FALSE;
					
					}	// end "else errCode != noErr"
					
				if (classPixelCount >= pointsPerClassVector[classNumber-1])
					{
					classNumber++;
					classPixelCount = 0;
					
					}	// end "if (classPixelCount >= pointsPerClassVector..."
				
				if (errCode == eofErr)	
					notDoneFlag = FALSE;
					
				}	// end "while (continueFlag && notDoneFlag)"
		
			if (continueFlag)
				{
						// Save the number of classes. Allow for the background class.
						 
				outFileInfoPtr->maxClassNumberValue = outFileInfoPtr->numberClasses - 1;
				
						// Write a Erdas header if requested.
						
				if (reformatOptionsPtr->headerFormat == kErdas74Type)
					continueFlag = WriteNewErdasHeader (outFileInfoPtr,
																		(SInt16*)inputBuffer, 
																		kErdas74Type);
				
						// Write buffer to the output disk file.
				
				if (continueFlag)
					{
					UInt32 count = reformatOptionsPtr->countOutBytes;
					CMFileStream* outFileStreamPtr = GetFileStreamPointer (outFileInfoPtr);
					errCode = MWriteData (outFileStreamPtr,
													&count,
													outputImageBuffer,
													kErrorMessages);
					
					if (errCode != noErr)									
						continueFlag = FALSE;
						
					}	// end "if (continueFlag)"
													
				}	// end "if (continueFlag)"
				
			}	// end "if (continueFlag)"
			
		}	// end "if (pointsPerClassVector != NULL)"
		
	CheckAndDisposePtr (pointsPerClassVector);
		
	if (continueFlag)
		{
				// Update the number of classes. Allow for the background class.
				 
		outFileInfoPtr->numberClasses = numberClasses + 1;
		outFileInfoPtr->maxClassNumberValue = outFileInfoPtr->numberClasses - 1;
		
		}	// end "if (continueFlag)"

	return (continueFlag);
		
}	// end "ENVI_ROIToThematicConvertPixelsToClassNumbers2"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ENVI_ROIToThematicGetClassName
//
//	Software purpose:	The purpose of this routine is to load the Reformat 
//							Specification structure with the default set of
//							parameters for converting project train/test area
//							to a thematic image file.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			ENVI_ROIToThematicConvertPixelsToClassNumbers1
//							ENVI_ROIToThematicConvertPixelsToClassNumbers2
//
//	Coded By:			Larry L. Biehl			Date: 07/25/2011
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

void ENVI_ROIToThematicGetClassName (
				UCharPtr								inputStringPtr,
				UCharPtr								classNamePtr)

{
	UInt32								index;
	
	
			// Go to first nonblank character after the ':'. It should be
			// within the first 16 characters or so.
	
	index = 0;
	while (*inputStringPtr != ':' && index < 20)
		{
		inputStringPtr++;
		index++;
		
		}	// end "while (*inputStringPtr != ':' && index < 20)"
			
	if (index <= 20)
		{
		while (*inputStringPtr == ' ' || *inputStringPtr == ':')
			inputStringPtr++;
	
				// The beginning of the class name has been found.
				// Copy until a blank or carriage return has been found.
	
		index = 0;
		while (index < 31 && *inputStringPtr != '\r' && *inputStringPtr != '\n')
			{	
			index++;		
			classNamePtr[index] = *inputStringPtr;
			inputStringPtr++;
		
			}	// end "while (*inputStringPtr != ' ' || ..."
			
		}	// end "if (index <= 20)"
		
	else	// Start of name was not found.
		index = 0;
	
			// Store the number of characters in the name.
				
	*classNamePtr = (UInt8)index;
		
}	// end "ENVI_ROIToThematicGetClassName"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ENVI_ROIToThematicLoadSpecs
//
//	Software purpose:	The purpose of this routine is to load the Reformat 
//							Specification structure with the default set of
//							parameters for converting project train/test area
//							to a thematic image file.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			AreasToThematicFileControl
//
//	Coded By:			Larry L. Biehl			Date: 03/11/1997
//	Revised By:			Larry L. Biehl			Date: 01/31/2013

Boolean ENVI_ROIToThematicLoadSpecs (
				Handle*								reformatOptionsHPtr)

{
	FileInfoPtr							outFileInfoPtr;
	ReformatOptionsPtr				reformatOptionsPtr;
	
	Handle								outFileInfoHandle;
	
	Boolean								continueFlag;			
	
																				
	outFileInfoHandle = NULL;
	reformatOptionsPtr = NULL;
	
			// Get a handle to a block of memory to be used for						
			// file information for the new image file.									
			// Lock the memory until the information has been loaded					
			// in. Then get the pointer to the file information block				
			
			// Get a handle to a block of memory to use for the reformatting		
			// options.  Then lock it and get a pointer to the block.				
		
	continueFlag = GetReformatAndFileInfoStructures (
														&outFileInfoHandle, reformatOptionsHPtr);
	
	if (continueFlag)
		{														
		reformatOptionsPtr = (ReformatOptionsPtr)GetHandlePointer (
																		*reformatOptionsHPtr, kLock);

		outFileInfoPtr = (FileInfoPtr)GetHandlePointer (outFileInfoHandle, kLock);
	
				// Use active image information to set up the reformat structure.

		reformatOptionsPtr->lineEnd = 500;
		reformatOptionsPtr->columnEnd = 500;
		reformatOptionsPtr->numberChannels = 1;
		
				// Initialize the format information for the output file.			
				
		outFileInfoPtr->bandInterleave		= kBIL;    			// kBIL  
		outFileInfoPtr->numberHeaderBytes 	= 128;
		outFileInfoPtr->numberTrailerBytes 	= 0;
		outFileInfoPtr->numberPreLineBytes 	= 0;
		outFileInfoPtr->numberPostLineBytes = 0;
		outFileInfoPtr->numberPreChannelBytes 	= 0;
		outFileInfoPtr->numberPostChannelBytes = 0;
		
		outFileInfoPtr->numberChannels 		= 1;
		outFileInfoPtr->numberClasses  		= 0;
		outFileInfoPtr->maxClassNumberValue = 0;
										
		outFileInfoPtr->numberBytes  			= 1;
		outFileInfoPtr->numberBits 			= 8;
			
		outFileInfoPtr->swapBytesFlag 		= FALSE;
		outFileInfoPtr->descriptionsFlag 	= FALSE;
		outFileInfoPtr->channelDescriptionH = NULL;
		outFileInfoPtr->classDescriptionH 	= NULL;
		outFileInfoPtr->thematicType 			= TRUE;
			
		}	// end "if (continueFlag)" 
	
			// If unable to set up information for reformat								
			// specifications, release memory allocated to it (if any).				
			
	else	// !continueFlag
		{
		ReleaseReformatOutputFileInfoAndBuffers (reformatOptionsPtr, NULL);
		ReleaseReformatSpecsMemory (reformatOptionsHPtr);
		
		}	// end "else !continueFlag"
		
	return (continueFlag);

}	// end "ENVI_ROIToThematicLoadSpecs"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean LoadAreasToThematicDialog
//
//	Software purpose:	The purpose of this routine is to handle the modal
//							dialog for confirming the specification for creating
//							a Thematic image file from the project fields.  The 
//							modal dialog template is in the resource file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 12/03/1990
//	Revised By:			Larry L. Biehl			Date: 03/21/2014	

Boolean LoadAreasToThematicDialog (
				ReformatOptionsPtr				reformatOptionsPtr)

{
	Boolean								returnFlag;
				
	#if defined multispec_mac
		DialogSelectArea					dialogSelectArea;
		
		Rect									//headerBox,
												theBox;
		
		DialogPtr							dialogPtr;
				
		UserItemUPP							drawDiskFileFormatPopUpPtr = NULL;
		
		UInt16								*localClassPtr = NULL;
		
		Handle								okHandle,
												theHandle;
		
		SInt32								theNum;
		
		UInt32								localNumberClasses;
		
		SInt16								itemHit,
												theType;
		
		Boolean								modalDone,
												testTypeFlag,
												trainTypeFlag;
		
		
				// Initialize local variables.													
				
		dialogPtr = NULL;
			
				// Get the modal dialog for the image display specification				
					
		dialogPtr = LoadRequestedDialog (kfieldsToImageID, kCopyScrap, 1, 2);
		if (dialogPtr == NULL)													
																							return (FALSE);
			
				// Intialize local user item proc pointers.
				
		drawDiskFileFormatPopUpPtr = NewUserItemUPP (DrawDiskFileFormatPopUp);
		
				// Set Procedure pointers for those dialog items that need them.			
		
		SetDialogItemDrawRoutine (dialogPtr, 18, gDrawDialogClassPopUpPtr);
		SetDialogItemDrawRoutine (dialogPtr, 20, drawDiskFileFormatPopUpPtr);
		
				// Initialize dialog variables.

		LoadAreasToThematicDialogInitialize (dialogPtr,
															reformatOptionsPtr,
															&dialogSelectArea,
															&trainTypeFlag,
															&testTypeFlag,
															&gClassSelection,
															&localClassPtr,
															&localNumberClasses,
															&gOutputFormatCode);
		
				// Save handle for the OK button for use later.								
				
		GetDialogItem (dialogPtr, 1,  &theType, &okHandle, &theBox);
		
				// Use training and/or testing fields.						
						
		SetDLogControl (dialogPtr, 15, trainTypeFlag);
		SetDLogControl (dialogPtr, 16, testTypeFlag);
		
				// Center the dialog and then show it.											
				
		ShowDialogWindow (dialogPtr, kfieldsToImageID, kSetUpDFilterTable);
		
				// Set default text selection to first edit text item						
				
		SelectDialogItemText (dialogPtr, 8, 0, INT16_MAX);
		
		modalDone = FALSE;
		itemHit = 0;
		do 
			{
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
					case 6:				// Entire area to selected area switch.
					case 22:				// Entire area to selected area switch. (Appearance Manager)			
					case 8:				//	 Display start line of image  
					case 9:				//	 Display end line of image  
					case 10:				//	 Display line interval of image  
					case 11:				//	 Display start column of image  
					case 12:				//	 Display end column of image  
					case 13:				//	 Display column interval of image  
						DialogLineColumnHits (&dialogSelectArea,
														dialogPtr, 
														itemHit,
														theHandle,
														theNum);
						break;
						
					case 15:			// Check box for 'Use training fields/areas'.		
					case 16:			// Check box for 'Use testing fields/areas'.	
						ChangeDLogCheckBox ((ControlHandle)theHandle);
						break;
						
					case 18:		// Classes 
						itemHit = StandardPopUpMenu (dialogPtr, 
																17, 
																18, 
																gPopUpAllSubsetMenu, 
																gClassSelection, 
																kPopUpAllSubsetMenuID);
						if (itemHit == kSubsetMenuItem)
							{
									// Subset of classes to be used.								
									
							itemHit = ClassDialog (
												&localNumberClasses,
												(SInt16*)localClassPtr, 
												1,
												gProjectInfoPtr->numberStatisticsClasses,
												gClassSelection,
												okHandle);
								
							}	// end "if (itemHit == kSubsetMenuItem)" 
						
						if (itemHit != 0)
							gClassSelection = itemHit;
		
								// Make certain that the correct label is drawn in the	
								// class pop up box.													
						
						InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
						break;		
							
					case 20:			// Popup box for disk file format	
						
						itemHit = StandardPopUpMenu (dialogPtr, 
																itemHit-1, 
																itemHit, 
																gPopUpDiskFileFormatMenu, 
																gOutputFormatCode,
																kPopUpDiskFileFormatMenuID);
																
						if (itemHit > 0)
							gOutputFormatCode = itemHit;
						break;
							
					}	// end "switch (itemHit)" 
					
				}	// end "if (itemHit > 2)" 
				
			else if (itemHit > 0) 	// and itemHit <= 2 
				{
						// If item hit is 1, check if line-column values make 			
						// sense.  If they don't, sound an alert and make item hit 		
						// equal to 0 to allow user to make changes.							
						
				if (itemHit == 1)
					itemHit = CheckLineColValues (
											&dialogSelectArea,
											reformatOptionsPtr->lineStart, 
											reformatOptionsPtr->columnStart, 
											dialogPtr);
					
				if	(itemHit == 1)      // User selected OK for information 
					{
					modalDone = TRUE;
					returnFlag = TRUE;

					LoadAreasToThematicDialogOK (reformatOptionsPtr,
															&dialogSelectArea,
															(GetDLogControl (dialogPtr, 15) == 1),
															(GetDLogControl (dialogPtr, 16) == 1),
															gClassSelection,
															localNumberClasses,
															gOutputFormatCode);

					}	// end "if	(itemHit == 1)" 
					
				if	(itemHit == 2)      // User selected Cancel for information 
					{
					modalDone = TRUE;		
					returnFlag = FALSE;
					
					}	// end "if	(itemHit == 2)" 
					
				}	// end "else if (itemHit > 0) and itemHit <= 2" 
					
			} while (!modalDone);  
			 
		DisposeUserItemUPP (drawDiskFileFormatPopUpPtr);                 
			
		CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
	#endif	// defined multispec_mac

	#if defined multispec_win
		CMFieldsToThematicDialog*		dialogPtr = NULL;
		
		TRY
			{ 
			dialogPtr = new CMFieldsToThematicDialog ();
			
			returnFlag = dialogPtr->DoDialog (reformatOptionsPtr); 
		                       
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
		CMFieldsToThematicDialog*		dialogPtr = NULL;
		
		try{ 
			dialogPtr = new CMFieldsToThematicDialog ();
			
			returnFlag = dialogPtr->DoDialog (reformatOptionsPtr); 
		                       
			delete dialogPtr;
			}
			
		catch (int e)
			{
			MemoryMessage (0, kObjectMessage);
			returnFlag = FALSE;
			}			
	#endif	// defined multispec_wx 

	CheckAndUnlockHandle (gProjectInfoPtr->windowInfoHandle);
	
	return (returnFlag);
	
}	// end "LoadAreasToThematicDialog"



void LoadAreasToThematicDialogInitialize (
				DialogPtr							dialogPtr,
				ReformatOptionsPtr				reformatOptionsPtr,
				DialogSelectArea*					dialogSelectAreaPtr,
				Boolean*								trainTypeFlagPtr,
				Boolean*								testTypeFlagPtr,
				SInt16*								classSelectionPtr,
				UInt16**								localClassPtrPtr,
				UInt32*								localNumberClassesPtr,
				SInt16*								outputFormatCodePtr)

{	
	SInt16								entireIconItem;
	
	
			// Update the specification dialog with the information given in 		
			// the reformat structure.													
				
			// Initialize selected area structure.	
	
	entireIconItem = 6;
	if (gAppearanceManagerFlag)
		entireIconItem = 22;
		
	InitializeDialogSelectArea (dialogSelectAreaPtr,
											NULL,
											gProjectSelectionWindow,
											reformatOptionsPtr->columnStart,
											reformatOptionsPtr->columnEnd,
											reformatOptionsPtr->columnInterval,
											reformatOptionsPtr->lineStart,
											reformatOptionsPtr->lineEnd,
											reformatOptionsPtr->lineInterval,
											8,
											entireIconItem,
											kAdjustToBaseImage);										
			
	dialogSelectAreaPtr->imageWindowInfoPtr = (WindowInfoPtr)GetHandlePointer (
														gProjectInfoPtr->windowInfoHandle, kLock);
								
	#if defined multispec_mac 
		if (gAppearanceManagerFlag)
			HideDialogItem (dialogPtr, 6);
		else	// !gAppearanceManagerFlag
			HideDialogItem (dialogPtr, 22);
	#endif	// defined multispec_mac

	LoadLineColumnItems (dialogSelectAreaPtr, 
									dialogPtr, 
									kInitializeLineColumnValues, 
									kIntervalEditBoxesExist,
									1);
	
			// Use training and/or testing fields.						
	
	if (gProjectInfoPtr->numberStatTrainFields > 0 &&
								    (reformatOptionsPtr->fieldType & kTrainingType))
		*trainTypeFlagPtr = TRUE;
	
	else	// ...numberStatTrainFields <= 0 && (...fieldType & kTrainingType)
		{
		*trainTypeFlagPtr = FALSE;
		if (gProjectInfoPtr->numberStatTrainFields <= 0)
			SetDLogControlHilite (dialogPtr, IDC_TrainingAreas, 255);
		
		}	// end "if (...->numberStatTrainFields <= 0 && ..." 					
	
	if (gProjectInfoPtr->numberStatTestFields > 0 &&
								    (reformatOptionsPtr->fieldType & kTestingType))
		*testTypeFlagPtr = TRUE;
	
	else	// ...numberStatTrainFields <= 0 && (...fieldType & kTrainingType)
		{
		*testTypeFlagPtr = FALSE;
		if (gProjectInfoPtr->numberStatTestFields <= 0)
			SetDLogControlHilite (dialogPtr, IDC_TestingAreas, 255);
		
		}	// end "if (...->numberStatTrainFields <= 0 && ..."                    
				
			// Display all or subset of classes/groups.
	
	*classSelectionPtr = reformatOptionsPtr->classSet;
	*localClassPtrPtr = (UInt16*)reformatOptionsPtr->classPtr;
	*localNumberClassesPtr = reformatOptionsPtr->numberClasses;	
													
	SetTIFF_GeoTIFF_MenuItemString (dialogSelectAreaPtr->imageWindowInfoPtr,
												dialogPtr,
												#if defined multispec_mac  
													gPopUpDiskFileFormatMenu,
												#endif	// defined multispec_mac
												#if defined multispec_win  
													IDC_OutputFormatCombo,
												#endif	// defined multispec_win
												#if defined multispec_wx
                                       NULL,
												#endif	// defined multispec_wx  
												kFieldsToThematicTIFFGeoTIFFMenuItem);
		
			// Set the default output format file type.
			 
	*outputFormatCodePtr = kFieldsToThematicTIFFGeoTIFFMenuItem;	
	if (reformatOptionsPtr->headerFormat == kErdas74Type)
		*outputFormatCodePtr = kFieldsToThematicERDAS74OutputFormat;
		
}	// end "LoadAreasToThematicDialogInitialize"



void LoadAreasToThematicDialogOK (
				ReformatOptionsPtr				reformatOptionsPtr,
				DialogSelectArea*					dialogSelectAreaPtr,
				Boolean								trainTypeFlag,
				Boolean								testTypeFlag,
				SInt16								classSelection,
				UInt32								localNumberClasses,
				SInt16								headerFormatCode)

{	
			// Selected reformat area.												
	
	reformatOptionsPtr->lineStart = dialogSelectAreaPtr->lineStart;
	reformatOptionsPtr->lineEnd = dialogSelectAreaPtr->lineEnd;
	reformatOptionsPtr->lineInterval = dialogSelectAreaPtr->lineInterval;
	reformatOptionsPtr->columnStart = dialogSelectAreaPtr->columnStart;
	reformatOptionsPtr->columnEnd = dialogSelectAreaPtr->columnEnd;
	reformatOptionsPtr->columnInterval = dialogSelectAreaPtr->columnInterval;
	
			// Use training and/or testing fields/areas.	
			
	reformatOptionsPtr->fieldType = 0;
	if (trainTypeFlag)
		reformatOptionsPtr->fieldType += kTrainingType;
	if (testTypeFlag)
		reformatOptionsPtr->fieldType += kTestingType;
		
			// Classes				

	reformatOptionsPtr->classSet = classSelection;
	reformatOptionsPtr->numberClasses = localNumberClasses;
	if (classSelection == kAllMenuItem)		// All classes 
		LoadClassVector (&reformatOptionsPtr->numberClasses,
								reformatOptionsPtr->classPtr);

			// "Write Erdas Header" check box.						

	reformatOptionsPtr->headerFormat = kNoneType;
	if (headerFormatCode == kFieldsToThematicERDAS74OutputFormat)
	 	reformatOptionsPtr->headerFormat = kErdas74Type;
	else if (headerFormatCode == kFieldsToThematicTIFFGeoTIFFMenuItem)
	 	reformatOptionsPtr->headerFormat = kTIFFType;
	
}	// end "LoadAreasToThematicDialogOK"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean LoadAreasToThematicSpecs
//
//	Software purpose:	The purpose of this routine is to load the Reformat 
//							Specification structure with the default set of
//							parameters for converting project train/test area
//							to a thematic image file.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			AreasToThematicFileControl
//
//	Coded By:			Larry L. Biehl			Date: 11/29/1990
//	Revised By:			Larry L. Biehl			Date: 03/11/2014

Boolean LoadAreasToThematicSpecs (
				Handle*								reformatOptionsHPtr)

{
	FileInfoPtr							outFileInfoPtr;
	ReformatOptionsPtr				reformatOptionsPtr;
	WindowInfoPtr						projectWindowInfoPtr;
	
	Handle								outFileInfoHandle;
	
	SInt32								bytesNeeded,
											fileFormat;
	
	Boolean								continueFlag,
											mapInfoExistsFlag,
											pointerChanged;

	
			// Check input parameters and any globals that will be used.			
	
	if (gProjectInfoPtr == NULL)
																				return (FALSE);
																				
	outFileInfoHandle = NULL;
	reformatOptionsPtr = NULL;
	
			// Get a handle to a block of memory to be used for						
			// file information for the new image file.									
			// Lock the memory until the information has been loaded					
			// in. Then get the pointer to the file information block				
			
			// Get a handle to a block of memory to use for the reformatting		
			// options.  Then lock it and get a pointer to the block.				
		
	continueFlag = GetReformatAndFileInfoStructures (
													&outFileInfoHandle, reformatOptionsHPtr);
	
	if (continueFlag)
		{														
		reformatOptionsPtr = (ReformatOptionsPtr)GetHandlePointer (
																		*reformatOptionsHPtr, kLock);

		outFileInfoPtr = (FileInfoPtr)GetHandlePointer (outFileInfoHandle, kLock);
	
				// Use project information to set up the reformat structure.

		projectWindowInfoPtr = (WindowInfoPtr)GetHandlePointer (
														gProjectInfoPtr->windowInfoHandle, kLock);

		reformatOptionsPtr->lineEnd = projectWindowInfoPtr->maxNumberLines;
		reformatOptionsPtr->columnEnd = projectWindowInfoPtr->maxNumberColumns;
		reformatOptionsPtr->stopLine = projectWindowInfoPtr->maxNumberLines;
		reformatOptionsPtr->stopColumn = projectWindowInfoPtr->maxNumberColumns;
		reformatOptionsPtr->numberClasses = gProjectInfoPtr->numberStatisticsClasses;
		reformatOptionsPtr->numberChannels = 1;
					
				// Check memory for classes vector.							
	
		bytesNeeded =
					(SInt32)gProjectInfoPtr->numberStatisticsClasses * sizeof (SInt16);
		continueFlag = CheckPointerSize ((Ptr*)&reformatOptionsPtr->classPtr,
															bytesNeeded,
															&pointerChanged);
		if (pointerChanged)
			reformatOptionsPtr->classSet = kAllMenuItem;
		
		}	// end "if (continueFlag)" 
		
	if (continueFlag)
		{
				// Load selection rectangle information into reformat 		
				// specification information if the selection rectangle has 		
				// been set up																		
				
		GetSelectedAreaInfo (gProjectSelectionWindow,
										gImageWindowInfoPtr,
										&reformatOptionsPtr->lineStart,
										&reformatOptionsPtr->lineEnd,
										&reformatOptionsPtr->columnStart,
										&reformatOptionsPtr->columnEnd,
										kDontClearSelectionArea,
										kUseThreshold,
										kAdjustToBaseImage);
		
				// Make certain that channel, class and weight vectors are			
				// filled in properly.															
		
		if (reformatOptionsPtr->classSet == kAllMenuItem)								
			LoadClassVector (&reformatOptionsPtr->numberClasses,
										reformatOptionsPtr->classPtr);
			
				// Initialize the format information for the output file.	
				
		fileFormat = GetFileFormatFromWindowHandle (gProjectInfoPtr->windowInfoHandle);
		mapInfoExistsFlag = FindIfMapInformationExists (projectWindowInfoPtr);
		
		reformatOptionsPtr->headerFormat = kGeoTIFFType;
		if (!mapInfoExistsFlag)
			{
			if (fileFormat == kErdas74Type)
				reformatOptionsPtr->headerFormat = kErdas74Type;
			else	// fileFormat != kErdas74Type
				reformatOptionsPtr->headerFormat = kTIFFType; 
				
			}	// end "if (!mapInfoExistsFlag)"
				
		outFileInfoPtr->numberTrailerBytes 	= 0;
		outFileInfoPtr->numberPreLineBytes 	= 0;
		outFileInfoPtr->numberPostLineBytes = 0;
		outFileInfoPtr->numberPreChannelBytes 	= 0;
		outFileInfoPtr->numberPostChannelBytes = 0;
		
		outFileInfoPtr->numberChannels 		= 1;
		outFileInfoPtr->numberClasses  		= 
										gProjectInfoPtr->numberStatisticsClasses + 1;
		outFileInfoPtr->maxClassNumberValue = outFileInfoPtr->numberClasses - 1;
										
		outFileInfoPtr->numberBytes  			= 1;
		outFileInfoPtr->numberBits 			= 8;
		if (outFileInfoPtr->numberClasses > 256)
			{
			outFileInfoPtr->numberBytes = 2;
			outFileInfoPtr->numberBits = 16;
			
			}	// end "if (outFileInfoPtr->numberClasses > 256)" 
			
		outFileInfoPtr->swapBytesFlag 		= FALSE;
		outFileInfoPtr->descriptionsFlag 	= FALSE;
		outFileInfoPtr->channelDescriptionH = NULL;
		outFileInfoPtr->classDescriptionH 	= NULL;
		outFileInfoPtr->thematicType 			= TRUE;
			
		}	// end "if (continueFlag)" 
	
			// If unable to set up information for reformat								
			// specifications, release memory allocated to it (if any).				
			
	else	// !continueFlag 
		{
		ReleaseReformatOutputFileInfoAndBuffers (reformatOptionsPtr, NULL);
		ReleaseReformatSpecsMemory (reformatOptionsHPtr);
		
		}	// end "else !continueFlag"
	
	CheckAndUnlockHandle (gProjectInfoPtr->windowInfoHandle);
		
	return (continueFlag);

}	// end "LoadAreasToThematicSpecs" 



#if defined multispec_mac 
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean MultispectralToThematicDialog
//
//	Software purpose:	The purpose of this routine is to handle the modal
//							dialog for setting the specifications for converting
//							the input multispectral image window to a thematic
//							image file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/18/1993
//	Revised By:			Larry L. Biehl			Date: 12/16/2016

Boolean MultispectralToThematicDialog (
				ReformatOptionsPtr				reformatOptionsPtr)

{
	Rect									theBox;
										
	DialogPtr							dialogPtr;

	UserItemUPP							drawHeaderOptionsPopUpPtr;
	
	Handle								theHandle;
	
	SInt32								lastNumberColors,
											theNum;
	
	SInt16								index,
											itemHit,
											itemHit2,
											theType;
	
	Boolean								modalDone,
											returnFlag;
											
		
			// Get the modal dialog for the image display specification				
				
	dialogPtr = LoadRequestedDialog (kToThematicDialogID, kCopyScrap, 1, 2);
	if (dialogPtr == NULL)													return (FALSE);
		
			// Intialize local user item proc pointers.		
			
	drawHeaderOptionsPopUpPtr = NewUserItemUPP (DrawHeaderOptionsPopUp);

			// Update the specification dialog with the information given in 		
			// the reformat structure.															
			//																							
			// Color sampling method.															
	
	SetDLogControl (dialogPtr, 5, (reformatOptionsPtr->convertType == 0));
	SetDLogControl (dialogPtr, 6, (reformatOptionsPtr->convertType == 1));
	SetDLogControl (dialogPtr, 7, (reformatOptionsPtr->convertType == 2));
						
			//	Display number of colors to convert to.									
	
	lastNumberColors = reformatOptionsPtr->numberClasses;
	LoadDItemValue (dialogPtr, 9, lastNumberColors);
	
			// Output disk file format pop up. 												
			
	gHeaderOptionsSelection = kERDAS74MenuItem;
	SetDialogItemDrawRoutine (dialogPtr, 11, drawHeaderOptionsPopUpPtr);
	
			// Make sure that all header choices are enabled to start with.
			
	InitializeHeaderPopupMenu (dialogPtr, gPopUpHeaderOptionsMenu);
   DisableMenuItem (gPopUpHeaderOptionsMenu, kArcViewMenuItem);
	DisableMenuItem (gPopUpHeaderOptionsMenu, kMatlabMenuItem);
	
	SetDLogControl (dialogPtr, 12, (reformatOptionsPtr->channelDescriptions));
	
			// Center the dialog and then show it.											
			
	ShowDialogWindow (dialogPtr, kToThematicDialogID, kSetUpDFilterTable);
	
			// Set default text selection to first edit text item						
			
	SelectDialogItemText (dialogPtr, 9, 0, INT16_MAX);
	
	modalDone = FALSE;
	itemHit = 0;
	do 
		{
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
				case 5:			// Control button for system sampling method.		
				case 6:			// Control button for popular sampling method.		
				case 7:			// Control button for median sampling method.
					for (index=5; index<=7; index++)
						SetDLogControl (dialogPtr, index, (index == itemHit));
					break;
					
				case 9:				//	 Display number of colors  
					if (theNum > 253)	
						NumberErrorAlert (lastNumberColors, dialogPtr, itemHit);
					else	// theNum <= 253 
						lastNumberColors = theNum;
					break;
					
				case 11:				// "Output format" popup box
					itemHit2 = StandardPopUpMenu (dialogPtr, 
															itemHit-1, 
															itemHit, 
															gPopUpHeaderOptionsMenu, 
															gHeaderOptionsSelection,
															kPopUpHeaderOptionsMenuID);
													
					if (itemHit2 > 0 && gHeaderOptionsSelection != itemHit2)
						{
						gHeaderOptionsSelection = itemHit2;
	
								// Make certain that the correct label is drawn in 	
								// the pop up boxes.												
						
						InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
							
						}	// end "if (itemHit2 > 0 && ..." 
					 
					break;
					
				case 12:			// Check box for 'List color table.		
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					break;
					
				}	// end "switch (itemHit)" 
				
			}	// end "if (itemHit > 2)" 
			
		else	// itemHit <= 2 
			{
			if	(itemHit == 1)      // User selected OK for information 
				{
				modalDone = TRUE;
				returnFlag = TRUE;
				
						// Items 5, 6 & 7; Sampling method to use.						
						
				reformatOptionsPtr->convertType = 0;
				if (GetDLogControl (dialogPtr, 5))
					reformatOptionsPtr->convertType = 0;
				if (GetDLogControl (dialogPtr, 6))
					reformatOptionsPtr->convertType = 1;
				if (GetDLogControl (dialogPtr, 7))
					reformatOptionsPtr->convertType = 2;
				
						// Number of colors to convert to.									
				
				reformatOptionsPtr->numberClasses = lastNumberColors;
	
						// "Output Format" popup box.											
 
				reformatOptionsPtr->headerFormat = 0;
				if (gHeaderOptionsSelection == kERDAS74MenuItem)
					reformatOptionsPtr->headerFormat = kErdas74Type;
				else if (gHeaderOptionsSelection == kGAIAMenuItem)
					reformatOptionsPtr->headerFormat = kGAIAType;
				else if (gHeaderOptionsSelection == kMatlabMenuItem)
					reformatOptionsPtr->headerFormat = kMatlabType;
				else if (gHeaderOptionsSelection == kTIFFGeoTIFFMenuItem)
					reformatOptionsPtr->headerFormat = kTIFFType;
					
				reformatOptionsPtr->channelDescriptions = 
															GetDLogControl (dialogPtr, 12);
					
				}	// end "if	(itemHit == 1)" 
				
			if	(itemHit == 2)      // User selected Cancel for information 
				{
				modalDone = TRUE;		
				returnFlag = FALSE;
				
				}	// end "if	(itemHit == 2)" 
				
			}	// end "else itemHit <= 2" 
				
		} while (!modalDone);
		
	DisposeUserItemUPP (drawHeaderOptionsPopUpPtr);
		
	CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
	
	return (returnFlag);
	
}	// end "MultispectralToThematicDialog"
#endif	// defined multispec_mac 

