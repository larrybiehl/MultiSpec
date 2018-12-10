//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SRecodeThematicImage.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			01/05/2018
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems	
//
//	Brief description:	This file contains functions that relate to listing
//								classification results.
//
//	Functions in file:
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"   

#if defined multispec_lin
	#define	IDOK									1
	#include "LRecodeThematicImageDialog.h"
#endif	// defined multispec_lin

#if defined multispec_mac || defined multispec_mac_swift
	#define	IDOK									1
	#define	IDC_InputFileNameText			5	
	#define	IDC_ThresholdValueEdit			8	
	#define	IDC_ThresholdFileCombo			10	
#endif	// defined multispec_mac || defined multispec_mac_swift
  
#if defined multispec_win
	#include "CImageWindow.h"
	#include "WRecodeThematicImageDialog.h"
#endif	// defined multispec_win 

//#include "SExtGlob.h"

extern void 		RecodeThematicImageDialogInitialize (
							DialogPtr							dialogPtr, 
							RecodeThematicImagePtr			recodeThematicImagePtr,
							Handle*								thresholdFileInfoHandlePtr,
							SInt32*								recodedValuePtr,
							SInt32*								thresholdValuePtr,
							SInt16*								compareParameterCodePtr,
							SInt16*								thresholdImageSelectionPtr);

extern void 		RecodeThematicImageDialogOK (
							RecodeThematicImagePtr			recodeThematicImagePtr,
							Handle								thresholdFileInfoHandle,
							SInt32								recodedValue,
							SInt32								thresholdValue,
							SInt16								compareParameterCode,
							SInt16								thresholdImageSelection);
							
extern SInt16 		RecodeThematicImageDialogSelectThresholdItem (
							Handle*								thresholdFileInfoHandlePtr,
							DialogPtr							dialogPtr,
							MenuHandle							popUpSelectThresholdImageMenu,
							SInt16								itemHit,
							SInt16								thresholdImageSelection,
							SInt16								selectStringNumber,
							SInt16								thresholdPopupItemNumber);


								// Global variables for this routine.
								
SInt16				gCompareParameterCode = 1;
SInt16				gThresholdImageSelection = 1;



			// Prototypes for routines in this file that are only called by		
			// other routines in this file.	
			
PascalVoid 			DrawCompareParameterPopUp (
							DialogPtr							dialogPtr, 
							SInt16								itemNumber);
							
PascalVoid 			DrawSelectThresholdImagePopUp (
							DialogPtr							dialogPtr, 
							SInt16								itemNumber);

Boolean 				LoadRecodeThematicImageSpecs (
							Handle*								reformatOptionsHPtr);

Boolean 				DoRecodeThematicImage (
							FileIOInstructionsPtr			fileIOInstructions1Ptr,
							FileIOInstructionsPtr			fileIOInstructions2Ptr,
							RecodeThematicImagePtr			recodeThematicImagePtr,
							UInt32*								outputValuesChangedPtr);

Boolean 				RecodeThematicImageDialog (
							ReformatOptionsPtr				reformatOptionsPtr);
							
void 					RecodeThematicImageDialogSetThresholdFileName (
							UCharPtr								fileNamePPointer,
							MenuHandle							popUpSelectThresholdImageMenu,
							DialogPtr							dialogPtr,
							SInt16								thresholdPopupItemNumber,
							SInt16								menuItemNumber);

void 					RecodeThematicImageDialogSetThresholdItems (
							MenuHandle							popUpSelectThresholdImageMenu,
							DialogPtr							dialogPtr,
							SInt16								thresholdPopupItemNumber,
							SInt16								thresholdImageSelection,
							Handle								thresholdFileInfoHandle);	



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean DoRecodeThematicImage
//
//	Software purpose:	The purpose of this routine is to create a new thematic image
//							based on an input thematic image and another image which is
//							called the threshold file.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 07/14/1999
//	Revised By:			Larry L. Biehl			Date: 11/04/1999

Boolean DoRecodeThematicImage (
				FileIOInstructionsPtr			fileIOInstructions1Ptr,
				FileIOInstructionsPtr			fileIOInstructions2Ptr,
				RecodeThematicImagePtr			recodeThematicImagePtr,
				UInt32*								outputValuesChangedPtr)
					
{
	//double								minutesLeft;	
												
	CMFileStream*						outFileStreamPtr;
	
	HUInt16Ptr							ioBuffer2Ptr,
				 							thresholdBuffer2Ptr;
				 							
	//Ptr									stringPtr;	
	
	SInt32								startTick;
											
	UInt32								columnEnd,
											columnStart,
											count,
											lastPercentComplete,
											line,
											lineCount,
											lineEnd,
											//linesLeft,
											lineStart,
											numberColumns,
											numberLines,
											numberSamples,
											percentComplete,
											previousValuesChanged,
											sample,
											tempCount,
											thresholdColumnEnd,
											thresholdColumnStart,
											thresholdLine,
											thresholdLineStart,
											valuesChanged,
											writePosOff;
	
	SInt16								errCode,
											returnCode;			// =0, no error			
																	// =1, no memory for 	
																	// 		text.				
																	// =2, disk file error.	
																	// =3, user quit.
																	
	UInt16								newRecodedValue,
											thresholdValue;
	
	Boolean								differentThresholdFileFlag,
											endHalfByte;
	
	
			// Verify that some variables are valid.
			
	outFileStreamPtr = GetFileStreamPointer (gImageFileInfoPtr);
	if (outFileStreamPtr == NULL)									
																							return (FALSE);
	
			// Initialize local variables														
	
	lineStart =					1;
	lineEnd = 					gImageFileInfoPtr->numberLines;
	
	columnStart =				1;
	columnEnd = 				gImageFileInfoPtr->numberColumns;
	
	lineCount = 				0;
	returnCode = 				noErr;
	startTick = 				TickCount ();
	
	newRecodedValue = 		(UInt16)recodeThematicImagePtr->newRecodedValue;
	thresholdValue = 			(UInt16)recodeThematicImagePtr->thresholdValue;

	lastPercentComplete = 0;
	
	differentThresholdFileFlag = 
									(recodeThematicImagePtr->thresholdImageSelection == 3);

	if (differentThresholdFileFlag)
		{
				// Get the areas of the thematic image file and threshold file
				// that are common. Only work with these areas.
				
		if (!GetCommonArea (gImageFileInfoPtr->startLine,
									gImageFileInfoPtr->startColumn,
									1, 
									1, 
									&lineStart, 
									&lineEnd, 
									&columnStart, 
									&columnEnd,
									gImageFileInfo2Ptr->startLine,
									gImageFileInfo2Ptr->startColumn, 
									&thresholdLineStart, 
									&thresholdColumnStart,
									gImageFileInfo2Ptr->numberLines, 
									gImageFileInfo2Ptr->numberColumns))
																						return (FALSE);
							
		}	// end "if (differentThresholdFileFlag)"
			
			// Close the file and set file back for write permission					 
			//	Then reopen to prepare for changing the file								
			
	errCode = SetFileWriteEnabled (outFileStreamPtr);
	if (errCode != noErr)															
																						return (FALSE);
	
			// Set up some more variables to use for reading the data.
			
	numberColumns = columnEnd - columnStart + 1;
	count = numberColumns * gImageFileInfoPtr->numberBytes;
	
	thresholdColumnEnd = thresholdColumnStart + numberColumns - 1;
						
	numberLines = lineEnd - lineStart + 1;
	
	valuesChanged = 0;
	
			// Get 2-byte pointers to the output buffers.	
			
	ioBuffer2Ptr = (HUInt16Ptr)gOutputBufferPtr;
	
	thresholdBuffer2Ptr = (HUInt16Ptr)gOutputBuffer2Ptr;
	if (!differentThresholdFileFlag)	
		thresholdBuffer2Ptr = (HUInt16Ptr)ioBuffer2Ptr;	
																					  
			// Load total number of lines in status information.						
			
	//linesLeft = lineEnd - lineStart + 1;
	//LoadDItemValue (gStatusDialogPtr, 
	//						IDC_Status20, 
	//						(SInt32)linesLeft);
		
			// Load some of the File IO Instructions structure that pertain
			// to the specific area being used.
			
	SetUpFileIOInstructions (fileIOInstructions1Ptr,
										NULL,
										lineStart,
										lineEnd,
										1,
										columnStart,
										columnEnd,
										1,
										1,
										NULL,
										kSetSpecialBILFlagFalse);
		
			// Load some of the File IO Instructions structure that pertain
			// to the specific area being used.
			
	SetUpFileIOInstructions (fileIOInstructions2Ptr,
										NULL,
										thresholdLineStart,
										thresholdLineStart+lineEnd-lineStart,
										1, 
										thresholdColumnStart,
										thresholdColumnEnd,
										1,
										1,
										NULL,
										kSetSpecialBILFlagFalse);
	
	//writePosOff = gImageFileInfoPtr->numberHeaderBytes + ((lineStart - 1) * 
	//				gImageFileInfoPtr->numberColumns + (columnStart - 1)) * 
	//															gImageFileInfoPtr->numberBytes;
					
			// Intialize the nextTime variable to indicate when the next check	
			// should occur for a command-.													
			
	gNextTime = TickCount ();
	gNextStatusTime = gNextTime;
	gNextMinutesLeftTime = gNextTime + gNextStatusTimeOffset;

			// Loop through lines for input image.							

	thresholdLine = thresholdLineStart;
	for (line=lineStart; line<=lineEnd; line++)
		{
		previousValuesChanged = valuesChanged;
	
				// Display line status information.											
				
		lineCount++;
		/*
		if (TickCount () >= gNextStatusTime)
			{
			LoadDItemValue (gStatusDialogPtr, IDC_Status18, lineCount);
			gNextStatusTime = TickCount () + gNextStatusTimeOffset;
			
			}	// end "if (TickCount () >= gNextStatusTime)" 
		*/
				// Get line of thematic image data. Return if there is a file IO error.					
				 
		errCode = GetLineOfData (fileIOInstructions1Ptr,
											line, 
											columnStart,
											columnEnd,
											1,
											gInputBufferPtr,
											gOutputBufferPtr);
			
		if (errCode < noErr)
			{
			returnCode = 2;
			break;
			
			}	// end "if (errCode < noErr)"
			
		if (differentThresholdFileFlag)
			{ 
			errCode = GetLineOfData (fileIOInstructions2Ptr,
												thresholdLine, 
												thresholdColumnStart,
												thresholdColumnEnd,
												1,
												gInputBuffer2Ptr,
												gOutputBuffer2Ptr);	
			
			if (errCode < noErr)
				{
				returnCode = 2;
				break;
				
				}	// end "if (errCode < noErr)"
				
			}	// end "if (differentThresholdFileFlag)"
			
		switch (recodeThematicImagePtr->compareParameterCode)
			{
			case 1:		// <=
				for (sample=0; sample<numberColumns; sample++)
					{
					if (thresholdBuffer2Ptr[sample] <= thresholdValue)
						{
						if (ioBuffer2Ptr[sample] != newRecodedValue)
							{
							ioBuffer2Ptr[sample] = newRecodedValue;
							valuesChanged++;
							
							}	// end "if (ioBuffer2Ptr[sample] != newRecodedValue)"
						
						}	// end "if (thresholdBuffer2Ptr[sample] <= thresholdValue)" 
		   			
		   		}	// end "for (sample=0; sample<numberColumns; sample++)"
				break;
				
			case 2:		// >=
				for (sample=0; sample<numberColumns; sample++)
					{
					if (thresholdBuffer2Ptr[sample] >= thresholdValue)
						{
						if (ioBuffer2Ptr[sample] != newRecodedValue)
							{
							ioBuffer2Ptr[sample] = newRecodedValue;
							valuesChanged++;
							
							}	// end "if (ioBuffer2Ptr[sample] != newRecodedValue)"
						
						}	// end "if (thresholdBuffer2Ptr[sample] >= thresholdValue)" 
		   			
		   		}	// end "for (sample=0; sample<numberColumns; sample++)"
				break;
				
			case 3:		// =
				for (sample=0; sample<numberColumns; sample++)
					{
					if (thresholdBuffer2Ptr[sample] == thresholdValue)
						{
						if (ioBuffer2Ptr[sample] != newRecodedValue)
							{
							ioBuffer2Ptr[sample] = newRecodedValue;
							valuesChanged++;
							
							}	// end "if (ioBuffer2Ptr[sample] != newRecodedValue)"
						
						}	// end "if (thresholdBuffer2Ptr[sample] = thresholdValue)" 
		   			
		   		}	// end "for (sample=0; sample<numberColumns; sample++)"
				break;
				
			case 4:		// ~=
				for (sample=0; sample<numberColumns; sample++)
					{
					if (thresholdBuffer2Ptr[sample] != thresholdValue)
						{
						if (ioBuffer2Ptr[sample] != newRecodedValue)
							{
							ioBuffer2Ptr[sample] = newRecodedValue;
							valuesChanged++;
							
							}	// end "if (ioBuffer2Ptr[sample] != newRecodedValue)"
						
						}	// end "if (thresholdBuffer2Ptr[sample] != thresholdValue)" 
		   			
		   		}	// end "for (sample=0; sample<numberColumns; sample++)"
				break;
			
			}	// end "switch (recodeThematicImagePtr->compareParameterCode)"
			
		if (valuesChanged != previousValuesChanged)
			{			
					// Write revised data back out to file.
					
					// Now pack the 2-byte data to 1-byte data if needed.
			
			if (gImageFileInfoPtr->numberBytes == 1)
				{									
				for (sample=0; sample<numberColumns; sample++)
					gOutputBufferPtr[sample] = (UInt8)ioBuffer2Ptr[sample];
						
				}	// end "if (gImageFileInfoPtr->numberBytes == 1)"
				
			else if (gImageFileInfoPtr->numberBytes == 2 &&
																gImageFileInfoPtr->swapBytesFlag)
				Swap2Bytes (ioBuffer2Ptr, numberColumns);
			
					// Next set pointer to correct location for writing.	
						
			writePosOff = (UInt32)GetFilePositionOffset (fileIOInstructions1Ptr,
																		gImageFileInfoPtr,
																		line, 
																		0, 
																		columnStart, 
																		columnEnd,
																		&numberSamples,
																		&tempCount,
																		&endHalfByte);		
				
			errCode = MSetMarker (outFileStreamPtr, 
												fsFromStart, 
												writePosOff,
												kErrorMessages);
			
			if (errCode == noErr)
				errCode = MWriteData (outFileStreamPtr, 
												&count, 
												gOutputBufferPtr, 
												kErrorMessages);
											
			if (errCode != noErr)
				{
				returnCode = 2;
				break;
				
				}	// end "if (errCode != noErr)"
				
			}	// end "if (valuesChanged != previousValuesChanged)"
		
				// Update dialog status information.		
	
		//linesLeft--;
		
		if (TickCount () >= gNextMinutesLeftTime)
			{
			percentComplete = 100 * lineCount/numberLines;
			if (percentComplete != lastPercentComplete)
				{
				LoadDItemValue (gStatusDialogPtr, 
									IDC_ShortStatusValue, 
									(SInt32)percentComplete);
				lastPercentComplete = percentComplete;
				
				}	// end "if (percentComplete != lastPercentComplete)" 
			/*				
			minutesLeft = (linesLeft * (TickCount () - startTick))/
														(double)(lineCount*kTicksPerMinute);
													
			sprintf ((char*)gTextString, " %.1f", minutesLeft);
			stringPtr = (char*)CtoPstring ((char*)gTextString, (char*)gTextString);
			LoadDItemString (gStatusDialogPtr, IDC_Status14, (Str255*)gTextString);
			*/				
			gNextMinutesLeftTime = TickCount () + gNextMinutesLeftTimeOffset;
			
			}	// end "if (TickCount () >= gNextMinutesLeftTime)"
			
				// Exit routine if user has "command period" down						
				
		if (TickCount () >= gNextTime)
			{
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
				{
				returnCode = 3;
				break;
				
				}	// end "if (!CheckSomeEvents (..."
				
			}	// end "if (TickCount () >= gNextTime)"
			
		thresholdLine++;
										
		}	// end "for (line=1; line<=lineEnd; line++)" 
	
	//if (returnCode == 0)
	//	LoadDItemValue (gStatusDialogPtr, IDC_Status18, lineCount);
																	
	percentComplete = 100 * lineCount/numberLines;
	LoadDItemValue (gStatusDialogPtr, IDC_ShortStatusValue, (SInt32)percentComplete);
		
			// Set the file back to read only permission.
					
	SetFileReadOnly (outFileStreamPtr);
	
	*outputValuesChangedPtr = valuesChanged;
		
	return (returnCode == 0);
		
}	// end "DoRecodeThematicImage" 
							


#if defined multispec_mac
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void DrawCompareParameterPopUp
//
//	Software purpose:	The purpose of this routine is to draw the image mask
//							selection for the select image mask popup menu.  This routine 
//							will also draw a drop shadow box around the pop up selection.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 07/15/1999
//	Revised By:			Larry L. Biehl			Date: 07/15/1999	

pascal void DrawCompareParameterPopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{	
			// Use the generic pop up drawing routine.									
			
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpCompareParameterMenu, 
								gCompareParameterCode, 
								true);
	
}	// end "DrawCompareParameterPopUp"
#endif	// defined multispec_mac 



#if defined multispec_mac
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void DrawSelectThresholdImagePopUp
//
//	Software purpose:	The purpose of this routine is to draw the threshold image
//							selection for the select threshold image popup menu.  This 
//							routine will also draw a drop shadow box around the pop up 
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
//	Coded By:			Larry L. Biehl			Date: 07/15/1999
//	Revised By:			Larry L. Biehl			Date: 07/15/1999	

pascal void DrawSelectThresholdImagePopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{	
			// Use the generic pop up drawing routine.									
			
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpSelectThresholdImageMenu, 
								gThresholdImageSelection, 
								true);
	
}	// end "DrawSelectThresholdImagePopUp"
#endif	// defined multispec_mac 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean LoadRecodeThematicImageSpecs
//
//	Software purpose:	The purpose of this routine is to load the Reformat 
//							Specification structure with the default set of
//							parameters for recoding a thematic image file.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			RecodeThematicImageControl
//
//	Coded By:			Larry L. Biehl			Date: 07/15/1999
//	Revised By:			Larry L. Biehl			Date: 01/31/2013

Boolean LoadRecodeThematicImageSpecs (
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

		reformatOptionsPtr->lineEnd = gImageFileInfoPtr->numberLines;
		reformatOptionsPtr->columnEnd = gImageFileInfoPtr->numberColumns;
		
				// Initialize the format information for the output file.			
				
		outFileInfoPtr->bandInterleave		= kBIL;    			// kBIL  
		outFileInfoPtr->numberHeaderBytes 	= 128;
		outFileInfoPtr->numberTrailerBytes 	= 0;
		outFileInfoPtr->numberPreLineBytes 	= 0;
		outFileInfoPtr->numberPostLineBytes = 0;
		outFileInfoPtr->numberPreChannelBytes 	= 0;
		outFileInfoPtr->numberPostChannelBytes = 0;
		
		outFileInfoPtr->numberChannels 		= 1;
		outFileInfoPtr->numberClasses  		= gImageFileInfoPtr->numberClasses;
										
		outFileInfoPtr->numberBytes  			= gImageFileInfoPtr->numberBytes;
		outFileInfoPtr->numberBits 			= gImageFileInfoPtr->numberBits;
			
		outFileInfoPtr->swapBytesFlag 		= FALSE;
		outFileInfoPtr->descriptionsFlag 	= FALSE;
		outFileInfoPtr->channelDescriptionH = NULL;
		outFileInfoPtr->classDescriptionH 	= NULL;
		outFileInfoPtr->thematicType 			= TRUE;
		
		reformatOptionsPtr->recodeThematicImage.thresholdFileInfoHandle = NULL;
		reformatOptionsPtr->recodeThematicImage.newRecodedValue = 0;
		reformatOptionsPtr->recodeThematicImage.thresholdValue = 0;
		reformatOptionsPtr->recodeThematicImage.compareParameterCode = 3;
		reformatOptionsPtr->recodeThematicImage.thresholdImageSelection = 1;
		
		}	// end "if (continueFlag)" 
	
			// If unable to set up information for reformat								
			// specifications, release memory allocated to it (if any).				
			
	else	// !continueFlag 
		{
		ReleaseReformatOutputFileInfoAndBuffers (reformatOptionsPtr, NULL);
		ReleaseReformatSpecsMemory (reformatOptionsHPtr);
		
		}	// end "else !continueFlag"
		
	return (continueFlag);

}	// end "LoadRecodeThematicImageSpecs" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void RecodeThematicImageControl
//
//	Software purpose:	The purpose of this routine is to control recoding the input 
//							thematic image to a new thematic image file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/15/1999
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

void RecodeThematicImageControl (void)

{
	time_t								startTime;

	char*									fileNamePtr;
	//FileInfoPtr							outFileInfoPtr = NULL;

	FileIOInstructionsPtr			fileIOInstructions1Ptr,
											fileIOInstructions2Ptr;
											
	RecodeThematicImagePtr			recodeThematicImagePtr;
	ReformatOptionsPtr				reformatOptionsPtr;
	
	Handle								reformatOptionsHandle,
											thresholdWindowInfoHandle = NULL;
											
	UInt32								valuesChanged = 0;
											
	SInt16								stringNumber,
											thresholdInfoHandleStatus;
	
	Boolean								continueFlag = TRUE,
											doneFlag;

	
			// If spare memory had to be used to load code resources, then exit	
			// routine.																				
		
	if (gMemoryTypeNeeded < 0)
																								return;
																							
			// Code resources loaded okay, so set flag back for non-Code			
			// resources.																			
			
	gMemoryTypeNeeded = 0;
																							
			// Get the reformat structure and initialize it.							
			
	if (!LoadRecodeThematicImageSpecs (&reformatOptionsHandle))
																								return;
		
			// Note that the following handles were locked in the call to
			// 'LoadAreasToThematicSpecs'
			
	reformatOptionsPtr = (ReformatOptionsPtr)GetHandlePointer (reformatOptionsHandle);

	//outFileInfoPtr = (FileInfoPtr)GetHandlePointer (
	//														reformatOptionsPtr->outFileInfoHandle);
					
	recodeThematicImagePtr = &reformatOptionsPtr->recodeThematicImage;
																		
	startTime = time (NULL);	
		
			// List the processor name, date and time.					
			
	gOutputTextOKFlag = TRUE;
	gOutputForce1Code = 0x0001;
	continueFlag = ListHeaderInfo (NULL, 
												0, 
												&gOutputForce1Code, 
												kNoStatisticsUsed, 
												continueFlag);
		
			// List input parameters to output text window.
			// List the file name that was changed.
			
	fileNamePtr = (char*)GetFileNamePPointerFromFileInfo (gImageFileInfoPtr);
	continueFlag = ListSpecifiedStringNumber (
												kReformatStrID, 
												IDS_Reform16,
												NULL, 
												gOutputForce1Code, 
												fileNamePtr, 
												continueFlag,
												kUTF8CharString);	
		
			// Insert a blank line after the table.
	
	continueFlag = OutputString (NULL, 
											gEndOfLine, 
											gNumberOfEndOfLineCharacters, 
											gOutputForce1Code, 
											continueFlag);
	
			// Allow multiple loops through the "Recode Thematic Image Option".
	
	doneFlag = FALSE;
	while (!doneFlag)
		{	
		MInitCursor ();
	
				// Present the user with a dialog box to allow a change in the default	
				// specifications.																	
			
		if (RecodeThematicImageDialog (reformatOptionsPtr))
			{
         
         
			fileIOInstructions1Ptr = NULL;
			fileIOInstructions2Ptr = NULL;
			/*												
				// Update parameters in the structure for the output file.				
			
			UpdateOutputFileStructure (outFileInfoPtr, 
													reformatOptionsPtr, 
													gImageFileInfoPtr->startLine, 
													gImageFileInfoPtr->startColumn);
			*/			
					// Get pointer to memory to use to read an image file line		
					// into. 																		
			
			if (continueFlag)	 						
				continueFlag = GetIOBufferPointers (&gFileIOInstructions[0],
																gImageWindowInfoPtr,
																gImageLayerInfoPtr,
																gImageFileInfoPtr,
																&gInputBufferPtr, 
																&gOutputBufferPtr,
																1,
																gImageFileInfoPtr->numberColumns,
																1,
																1,
																(UInt16*)NULL, 
																kDoNotPackData, 
																kDoNotForceBISFormat, 
																kForce2Bytes,
																kDoNotAllowForThreadedIO,
																&fileIOInstructions1Ptr);
         	
			if (continueFlag && recodeThematicImagePtr->thresholdImageSelection == 3)
				{																		
						// Get the window and layer structures for the threshold file.
				
				thresholdWindowInfoHandle = GetWindowInfoStructures (
												recodeThematicImagePtr->thresholdFileInfoHandle,
												kThematicImageType,
												kThematicWindowType);
				
				continueFlag = (thresholdWindowInfoHandle != NULL);
				
				if (continueFlag)
					{	
					GetInformationPointers (&thresholdInfoHandleStatus, 
													thresholdWindowInfoHandle,
													&gImageWindowInfo2Ptr, 
													&gImageLayerInfo2Ptr, 
													&gImageFileInfo2Ptr);
											
							// Note for now the image size of the probability file has to be
							// the same as that for the classified file.
					
					continueFlag = GetIOBufferPointers (
															&gFileIOInstructions[1],
															gImageWindowInfo2Ptr,
															gImageLayerInfo2Ptr,
															gImageFileInfo2Ptr,
															&gInputBuffer2Ptr, 
															&gOutputBuffer2Ptr,
															1,
															gImageWindowInfo2Ptr->maxNumberColumns,
															1,
															1,
															NULL,
															kDoNotPackData,
															kDoNotForceBISFormat,
															kForce2Bytes,
															kDoNotAllowForThreadedIO,
															&fileIOInstructions2Ptr);
						
					}	// end "if (continueFlag)"
				
				}	// end "if (continueFlag && reformatOptionsPtr->..."
			/*
					// For now the input file will be changed directly		
					// Get buffer for output data.													
					
			continueFlag = GetReformatOutputBuffer (outFileInfoPtr, reformatOptionsPtr);
						
					// Set up the disk file to be used for the new reformatted				
					// image file.																			
			
			if (continueFlag)	
				continueFlag = !GetReformatOutputFile (outFileInfoPtr, reformatOptionsPtr);
			*/														
					// List the threshold image file if one was used.
			
			if (recodeThematicImagePtr->thresholdFileInfoHandle != NULL)
				{
				fileNamePtr = (char*)GetFileNameCPointerFromFileInfo (gImageFileInfo2Ptr);
				continueFlag = ListSpecifiedStringNumber (kReformatStrID,
																		IDS_Reform27,
																		NULL, 
																		gOutputForce1Code, 
																		fileNamePtr,
																		continueFlag,
																		kUTF8CharString);
				
				}	// end "if (...->thresholdFileInfoHandle != NULL)"
														
					// List the old or threshold value depending upon whether
					// a threshold file was used.
																	
			stringNumber = IDS_Reform17;
			if (recodeThematicImagePtr->thresholdFileInfoHandle != NULL)	
				stringNumber = IDS_Reform18;
																	
			continueFlag = ListSpecifiedStringNumber (
														kReformatStrID, 
														stringNumber,
														NULL, 
														gOutputForce1Code, 
														recodeThematicImagePtr->thresholdValue, 
														continueFlag);
														
					// List the compare parameter that was used.
																
			SInt16 stringNumber = IDS_Reform23;
			if (recodeThematicImagePtr->compareParameterCode == 2)	
				stringNumber = IDS_Reform24;
			else if (recodeThematicImagePtr->compareParameterCode == 3)	
				stringNumber = IDS_Reform25;
			else if (recodeThematicImagePtr->compareParameterCode == 4)	
				stringNumber = IDS_Reform26;
				
			continueFlag = GetSpecifiedStringNumber (kReformatStrID,
																	stringNumber, 
																	gTextString3, 
																	continueFlag);
			
			continueFlag = ListSpecifiedStringNumber (
														kReformatStrID, 
														IDS_Reform19,
														NULL, 
														gOutputForce1Code, 
														(UCharPtr)gTextString3, 
														continueFlag);
								
					// List the new recode value that was used.
																	
			continueFlag = ListSpecifiedStringNumber (
														kReformatStrID, 
														IDS_Reform20,
														NULL, 
														gOutputForce1Code, 
														recodeThematicImagePtr->newRecodedValue, 
														continueFlag);
														
					// Change cursor to watch cursor until done with process.				
			
			MSetCursor (kWait);
			
					// Get the status dialog window.													
			
			gStatusDialogPtr = GetStatusDialog (kShortStatusInfoID, FALSE);
			if (continueFlag)	
				continueFlag = (gStatusDialogPtr != NULL);
		
					//	Set title and dialog items for changing pixels to class numbers.	
		
			if (continueFlag)
				{						
				MGetString ((UCharPtr)gTextString, kReformatStrID, IDS_PercentComplete);
				LoadDItemString (
							gStatusDialogPtr, IDC_ShortStatusText, (Str255*)gTextString);
				LoadDItemValue (gStatusDialogPtr, IDC_ShortStatusValue, (SInt32)0);
				ShowStatusDialogWindow (kShortStatusInfoID);
				
				CheckSomeEvents (updateMask);
				
				}	// end "if (continueFlag)" 
					
					// Now change those pixels that belong to one of the requested			
					// project classes.																	
			
			if (continueFlag)	
				continueFlag = DoRecodeThematicImage (fileIOInstructions1Ptr,
																	fileIOInstructions2Ptr,
																	recodeThematicImagePtr,
																	&valuesChanged);
													
			if (continueFlag)
				{
						// Indicate that any displayed image windows representing the
						// thematic image file that was changed may not correctly reflect
						// the data in the file.
				
				SetImageWindowIsUpToDateFlag (gImageWindowInfoPtr, FALSE);
				
				}	// end "if (continueFlag)"
																	
					// Create the ERDAS Trailer file if needed.									
			/*		
			if (continueFlag && (reformatOptionsPtr->headerFormat == kErdas73Type ||
												reformatOptionsPtr->headerFormat == kErdas74Type))
				{
				continueFlag = CreateThematicSupportFile (
											outFileInfoPtr,
											(UInt16)reformatOptionsPtr->numberClasses,
											reformatOptionsPtr->classPtr,
											NULL,
											NULL,
											NULL,
											kDefaultColors,
											1,
											0,
											kPaletteHistogramClassNames,
											kITRLFileType);
											
				}	// end "if (continueFlag && ...)" 
			*/				  		
		  					// Close the output file.													
		  				
			//CloseFile (outFileInfoPtr);
														
					// List the number of values that were changed.
																	
			continueFlag = ListSpecifiedStringNumber (kReformatStrID,
																	IDS_Reform21,
																	NULL, 
																	gOutputForce1Code, 
																	(SInt32)valuesChanged, 
																	continueFlag);
			
					// List an error message if continueFlag is false.
					
			if (!continueFlag)	
				ListSpecifiedStringNumber (kReformatStrID, 
														IDS_Reform22,
														NULL, 
														gOutputForce1Code, 
														fileNamePtr, 
														TRUE);	
		
					// Insert a blank line after the table.
			
			continueFlag = OutputString (NULL, 
													gEndOfLine, 
													gNumberOfEndOfLineCharacters, 
													gOutputForce1Code, 
													continueFlag);
					
					// Dispose of the IO buffers.		
										
			DisposeIOBufferPointers (fileIOInstructions1Ptr,
												&gInputBufferPtr, 
												&gOutputBufferPtr);	
										
			DisposeIOBufferPointers (fileIOInstructions2Ptr,
												&gInputBuffer2Ptr, 
												&gOutputBuffer2Ptr);
				
			if (thresholdWindowInfoHandle != NULL)
				{
				#if defined multispec_mac
					UnlockAndDispose (gImageWindowInfo2Ptr->layerInfoHandle);
					UnlockAndDispose (thresholdWindowInfoHandle); 
				#endif	// defined multispec_mac 
		
				#if defined multispec_win 
							// Make sure that the file information handle is not deleted yet.
					gImageWindowInfo2Ptr->fileInfoHandle = NULL;

					delete GetWindowClassPointer (thresholdWindowInfoHandle);
				#endif	// defined multispec_win 

            #if defined multispec_lin
               gImageWindowInfo2Ptr->fileInfoHandle = NULL;
               thresholdWindowInfoHandle = NULL;
            #endif
									
				gImageWindowInfo2Ptr = NULL; 
				gImageLayerInfo2Ptr = NULL; 
				gImageFileInfo2Ptr = NULL;
									
				}	// end "if (thresholdWindowInfoHandle != NULL)"
			
			}	// end "if (RecodeThematicImageDialog (reformatOptionsPtr))"
			
		else	// !RecodeThematicImageDialog (reformatOptionsPtr)
			doneFlag = TRUE;
												
			if (!continueFlag)
				doneFlag = TRUE;
		
		}	// end "while (!doneFlag)"
				
			// List the CPU time taken for reformatting.					
				
	continueFlag = ListCPUTimeInformation (NULL, 
														continueFlag, 
														startTime);
		
			// Dispose of handle for threshold file structures if needed.
			
	DisposeFileInfoHandle (recodeThematicImagePtr->thresholdFileInfoHandle);
	recodeThematicImagePtr->thresholdFileInfoHandle = NULL;
	
	ReleaseReformatOutputFileInfoAndBuffers (reformatOptionsPtr, NULL);
			
			// Release memory used for the reformat structure.							
			
	//ReleaseReformatSpecsMemory (&reformatOptionsHandle, NULL);
					
			// Dispose of updating statistics status dialog box.						
		
	CloseStatusDialog (TRUE);
				
	MInitCursor ();
		
}	// end "RecodeThematicImageControl" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 RecodeThematicImageDialog
//
//	Software purpose:	The purpose of this routine is to handle the 		
//							modal dialog for selecting the statistics function
//							to be done. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	0 if user clicked on cancel
//							1 if user clicked on ok.
//
// Called By:			StatisticsControl   in statistics.c
//
//	Coded By:			Larry L. Biehl			Date: 07/15/1999
//	Revised By:			Larry L. Biehl			Date: 12/16/2016

Boolean RecodeThematicImageDialog (
				ReformatOptionsPtr				reformatOptionsPtr)

{  
	Boolean								returnFlag = FALSE;
	
#if defined multispec_mac
	Handle								thresholdFileInfoHandle;
	
	SInt32								recodedValue,
											thresholdValue;
	
	Rect									theBox;
	
	DialogPtr							dialogPtr;

	UserItemUPP							selectThresholdImagePopUpPtr,
											selectCompareParameterPopUpPtr;
									
	Handle								okHandle,
											theHandle;
	
	SInt16								itemHit,
											theType;
											
	Boolean								modalDone;
											

			// Get the modal dialog for the reformat specification					
				
	dialogPtr = LoadRequestedDialog (kRecodeThematicSpecificationID, kCopyScrap, 1, 2);
	if (dialogPtr == NULL)														
																								return (0);
		
			// Initialize local user item proc pointers.									

	selectCompareParameterPopUpPtr = NewUserItemUPP (DrawCompareParameterPopUp);
	selectThresholdImagePopUpPtr = NewUserItemUPP (DrawSelectThresholdImagePopUp);
	
			// Set Procedure pointers for those dialog items that need them.							
			
	SetDialogItemDrawRoutine (dialogPtr, 7, selectCompareParameterPopUpPtr);										
			
	SetDialogItemDrawRoutine (dialogPtr, 10, selectThresholdImagePopUpPtr);	
	
			// Initialize dialog variables.
			
	RecodeThematicImageDialogInitialize (dialogPtr,
														&reformatOptionsPtr->recodeThematicImage,
														&thresholdFileInfoHandle,
														&recodedValue,
														&thresholdValue,
														&gCompareParameterCode,
														&gThresholdImageSelection);
	
			// Save handle for the OK button for use later.								
			
	GetDialogItem (dialogPtr, 1, &theType, &okHandle, &theBox);
		
	LoadDItemValue (dialogPtr, 4, recodedValue);
	LoadDItemValue (dialogPtr, 8, thresholdValue);
		
			// Center the dialog and then show it.											
			
	ShowDialogWindow (dialogPtr, kRecodeThematicSpecificationID, kSetUpDFilterTable);
	
			// Allow minus number to be entered.
			
	gDialogItemDescriptorPtr[4] = kDItemMinus;
	gDialogItemDescriptorPtr[8] = kDItemMinus;
	
			// Set default text selection to first edit text item						
			
	SelectDialogItemText (dialogPtr, 4, 0, INT16_MAX);
	
	modalDone = false;
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
			
			switch (itemHit)
				{	
				case 7:		// Compare parameter
					itemHit = StandardPopUpMenu (dialogPtr, 
															itemHit-1, 
															itemHit, 
															gPopUpCompareParameterMenu, 
															gCompareParameterCode,
															kPopUpCompareParameterMenuID);
													
					if (itemHit != 0)	
						gCompareParameterCode = itemHit;
					break;
					
				case 10:		// Threshold file
					itemHit = StandardPopUpMenu (dialogPtr, 
															itemHit-1, 
															itemHit, 
															gPopUpSelectThresholdImageMenu, 
															gThresholdImageSelection,
															kPopUpSelectThresholdImageMenuID);
													
					if (itemHit != 0)
						{						
						itemHit = RecodeThematicImageDialogSelectThresholdItem (
																	&thresholdFileInfoHandle,
																	dialogPtr,
																	gPopUpSelectThresholdImageMenu,
																	itemHit,
																	gThresholdImageSelection,
																	IDS_FileIO105,
																	10);	
							
						gThresholdImageSelection = itemHit;
						InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
						
						}	// end "if (itemHit != 0)"
					break;
						
				}	// end "switch (itemHit)" 
				
			}	// end "if (itemHit > 2)" 
			
		else	// itemHit <= 2 
			{
					// Check new recode data value
					
			if (itemHit == 1)
				itemHit = CheckMaxValue (dialogPtr, 
													4, 
													gImageFileInfoPtr->minUsableDataValue,
													gImageFileInfoPtr->maxUsableDataValue,
													kDisplayRangeAlert);
			
					// Check previous data value
					
			if (itemHit == 1)
				itemHit = CheckMaxValue (dialogPtr, 
													8, 
													gImageFileInfoPtr->minUsableDataValue,
													gImageFileInfoPtr->maxUsableDataValue,
													kDisplayRangeAlert);
				
					// User selected OK for information. Verify that the threshold file
					// is okay to use.
			/*		
			if	(itemHit == 1) 
				itemHit = StatisticsDialogMaskCheck (thresholdFileInfoHandle,
																	gThresholdImageSelection);
			*/
			if	(itemHit == 1)       
				{
						// User selected OK for information and any mask file was read
						// into memory okay.
						
				modalDone = TRUE;
				returnFlag = TRUE; 
				
				RecodeThematicImageDialogOK (&reformatOptionsPtr->recodeThematicImage,
														thresholdFileInfoHandle,
														GetDItemValue (dialogPtr, 4),
														GetDItemValue (dialogPtr, 8),
														gCompareParameterCode,
														gThresholdImageSelection);
														
				}	// end "if (itemHit == 1)" 
				
			if	(itemHit == 2)      // User selected Cancel for information 
				{
				modalDone = TRUE;
				returnFlag = FALSE;
				
				}	// end "if	(itemHit == 2)" 
				
			}	// end "else itemHit <= 2" 
				
		}	while (!modalDone);
	
	DisposeUserItemUPP (selectCompareParameterPopUpPtr);
	DisposeUserItemUPP (selectThresholdImagePopUpPtr);
		
	CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
#endif	// defined multispec_mac
                   
	#if defined multispec_win  
		CMRecodeThematicDialog*		dialogPtr = NULL;
		
		TRY
			{ 
			dialogPtr = new CMRecodeThematicDialog (); 
			
			returnFlag = dialogPtr->DoDialog (&reformatOptionsPtr->recodeThematicImage); 
		                       
			delete dialogPtr;
			}
			
		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			returnFlag = FALSE;
			}
		END_CATCH_ALL
	#endif // defined multispec_win  
      
   #if defined multispec_lin
      CMRecodeThematicDialog*		dialogPtr = NULL;
      dialogPtr = new CMRecodeThematicDialog (); 
			
      returnFlag = dialogPtr->DoDialog (&reformatOptionsPtr->recodeThematicImage); 
		                       
		delete dialogPtr;
   #endif

	return (returnFlag);
	
}	// end "RecodeThematicImageDialog" 



void RecodeThematicImageDialogInitialize (
				DialogPtr							dialogPtr, 
				RecodeThematicImagePtr			recodeThematicImagePtr,
				Handle*								thresholdFileInfoHandlePtr,
				SInt32*								recodedValuePtr,
				SInt32*								thresholdValuePtr,
				SInt16*								compareParameterCodePtr,
				SInt16*								thresholdImageSelectionPtr)

{  
	*thresholdFileInfoHandlePtr = recodeThematicImagePtr->thresholdFileInfoHandle;
	
	*recodedValuePtr = recodeThematicImagePtr->newRecodedValue;
	
	*thresholdValuePtr = recodeThematicImagePtr->thresholdValue;
	
	*compareParameterCodePtr = recodeThematicImagePtr->compareParameterCode;
	
	*thresholdImageSelectionPtr = recodeThematicImagePtr->thresholdImageSelection;

			// Load name of image window into the dialog box.	
			
	char* fileNamePtr = (char*)GetFileNamePPointerFromFileInfo (gImageFileInfoPtr);
	LoadSpecifiedStringNumberStringP (kReformatStrID, 
													IDS_Reform15, 
													(char*)gTextString,
													(char*)gTextString2,
													TRUE,
													&fileNamePtr[1]);
	LoadDItemString (dialogPtr, 
							IDC_InputFileNameText, 
							(Str255*)gTextString);
	
	RecodeThematicImageDialogSetThresholdItems (gPopUpSelectThresholdImageMenu,
																dialogPtr,
																IDC_ThresholdFileCombo,
																*thresholdImageSelectionPtr,
																*thresholdFileInfoHandlePtr);
	
}	// end "RecodeThematicImageDialogInitialize"

                   
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void RecodeThematicImageDialogOK
//
//	Software purpose:	The purpose of this routine is to update the parameters that
//							the user specified in the statistics dialog box. 		     
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			StatisticsDialog in statistics.c
//
//	Coded By:			Larry L. Biehl			Date: 07/15/1999
//	Revised By:			Larry L. Biehl			Date: 07/16/1999

void RecodeThematicImageDialogOK (
				RecodeThematicImagePtr			recodeThematicImagePtr,
				Handle								thresholdFileInfoHandle,
				SInt32								recodedValue,
				SInt32								thresholdValue,
				SInt16								compareParameterCode,
				SInt16								thresholdImageSelection)

{
	 recodeThematicImagePtr->thresholdFileInfoHandle = thresholdFileInfoHandle;
	
	 recodeThematicImagePtr->newRecodedValue = recodedValue;
	
	 recodeThematicImagePtr->thresholdValue = thresholdValue;
	
	 recodeThematicImagePtr->compareParameterCode = compareParameterCode;
	
	 recodeThematicImagePtr->thresholdImageSelection = thresholdImageSelection;
		
}	// end "RecodeThematicImageDialogOK"



SInt16 RecodeThematicImageDialogSelectThresholdItem (
				Handle*								thresholdFileInfoHandlePtr,
				DialogPtr							dialogPtr,
				MenuHandle							popUpSelectThresholdImageMenu,
				SInt16								itemHit,
				SInt16								thresholdImageSelection,
				SInt16								selectStringNumber,
				SInt16								thresholdPopupItemNumber)	

{		
	FileStringPtr						fileNamePPointer;
	
	SignedByte							handleStatus;
			
			
	if (itemHit == 2)
		{
				// Select a new threshold file.

		SetDLogControlHilite (dialogPtr, IDOK, 255);
				
		*thresholdFileInfoHandlePtr = GetMaskFile (*thresholdFileInfoHandlePtr,
																	selectStringNumber);

		SetDLogControlHilite (dialogPtr, IDOK, 0);
		
		if (*thresholdFileInfoHandlePtr != NULL)
			{
			fileNamePPointer = (FileStringPtr)GetFileNamePPointerFromFileHandle (
																		*thresholdFileInfoHandlePtr,
																		&handleStatus);

			RecodeThematicImageDialogSetThresholdFileName (
																		fileNamePPointer,
																		popUpSelectThresholdImageMenu,
																		dialogPtr,
																		thresholdPopupItemNumber,
																		3);
			
					// Unlock the class name structure handle if needed.					
			
			MHSetState (*thresholdFileInfoHandlePtr, handleStatus);
				
			itemHit = 3;
			
			}	// end "if (*thresholdFileInfoHandlePtr != NULL)"
			
		else	// *thresholdFileInfoHandlePtr == NULL
					// No change was made.
			itemHit = thresholdImageSelection;
		
		}	// end "if (itemHit == 2)"
		
	return (itemHit);
	
}	// end "RecodeThematicImageDialogSelectThresholdItem"



void RecodeThematicImageDialogSetThresholdFileName (
				UCharPtr								fileNamePPointer,
				MenuHandle							popUpSelectThresholdImageMenu,
				DialogPtr							dialogPtr,
				SInt16								thresholdPopupItemNumber,
				SInt16								menuItemNumber)					

{	
	#if defined multispec_win
		USES_CONVERSION;
	#endif

	if (fileNamePPointer != NULL)
		{
		#if defined multispec_mac
			if (CountMenuItems (popUpSelectThresholdImageMenu) < menuItemNumber)
				InsertMenuItem (popUpSelectThresholdImageMenu, "\p1", menuItemNumber);
			
			SetMenuItemText (popUpSelectThresholdImageMenu, 
									menuItemNumber, 
									(ConstStr255Param)fileNamePPointer);
		#endif	// defined multispec_mac
																			
		#if defined multispec_win
			CComboBox* comboBoxPtr = 
								(CComboBox*)dialogPtr->GetDlgItem (thresholdPopupItemNumber);
			if (comboBoxPtr->GetCount () < menuItemNumber)
				comboBoxPtr->AddString ((LPCTSTR)_T(" "));
			dialogPtr->SetComboItemText (thresholdPopupItemNumber, 
														menuItemNumber-1, 
														&fileNamePPointer[1]);
		#endif	// defined multispec_win

		#if defined multispec_lin
         wxComboBox* comboBoxPtr =
								(wxComboBox*)dialogPtr->FindWindow (thresholdPopupItemNumber);
         if (comboBoxPtr->GetCount () < menuItemNumber)
            comboBoxPtr->Insert (" ", comboBoxPtr->GetCount ());
				
			dialogPtr->SetComboItemText (thresholdPopupItemNumber, 
													menuItemNumber-1,
													(char*)&fileNamePPointer[1]);
		#endif
		}	// end "if (fileNamePPointer != NULL)"
	
}	// end "RecodeThematicImageDialogSetThresholdFileName"



void RecodeThematicImageDialogSetThresholdItems (
				MenuHandle							popUpSelectThresholdImageMenu,
				DialogPtr							dialogPtr,
				SInt16								thresholdPopupItemNumber,
				SInt16								thresholdImageSelection,
				Handle								thresholdFileInfoHandle)					

{
	FileStringPtr						fileNamePPointer;
	
	
			// Set the threshold image dialog items.
			
	fileNamePPointer =
					(FileStringPtr)GetFileNamePPointerFromFileInfo (gImageFileInfoPtr);

	RecodeThematicImageDialogSetThresholdFileName (fileNamePPointer,
																	popUpSelectThresholdImageMenu,
																	dialogPtr,
																	thresholdPopupItemNumber,
																	1);
	
	#if defined multispec_mac
		SetMenuItemText (popUpSelectThresholdImageMenu, 
								2, 
								"\pSelect threshold file...");
	#endif	// defined multispec_mac
																		
	#if defined multispec_win
		CComboBox* comboBoxPtr = 
								(CComboBox*)dialogPtr->GetDlgItem (thresholdPopupItemNumber);
		if (comboBoxPtr->GetCount () < 2)
			comboBoxPtr->AddString ((LPCTSTR)_T(" "));
	
		dialogPtr->SetComboItemText (thresholdPopupItemNumber, 
												1,
												(UCharPtr)"Select threshold file...");
	#endif	// defined multispec_win 

   #if defined multispec_lin
      wxComboBox* comboBoxPtr =
							(wxComboBox*)dialogPtr->FindWindow (thresholdPopupItemNumber);
      if (comboBoxPtr->GetCount () < 2)
         comboBoxPtr->Insert (" ", comboBoxPtr->GetCount ());
      
      dialogPtr->SetComboItemText (thresholdPopupItemNumber, 
												1,
												"Select threshold file...");
   #endif // defined multispec_lin
		
	if (thresholdImageSelection == 1)
		{
		#if defined multispec_lin
         if (comboBoxPtr->GetCount () == 3)
             comboBoxPtr->Delete (2); 
      #endif // defined multispec_lin
		
		#if defined multispec_mac
			DeleteMenuItem (popUpSelectThresholdImageMenu, 3);
		#endif	// defined multispec_mac
																			
		#if defined multispec_win
			if (comboBoxPtr->GetCount () == 3)
				comboBoxPtr->DeleteString (2);
		#endif	// defined multispec_win

		}	// end "if (thresholdImageSelection == 1)"
		
	if (thresholdImageSelection == 3)
		{
		#if defined multispec_win
			SignedByte							handleStatus;
			
			fileNamePPointer = (FileStringPtr)GetFileNamePPointerFromFileHandle (
																				thresholdFileInfoHandle,
																				&handleStatus);

			RecodeThematicImageDialogSetThresholdFileName (fileNamePPointer,
																			popUpSelectThresholdImageMenu,
																			dialogPtr,
																			thresholdPopupItemNumber,
																			3);
		
					// Unlock the class name structure handle if needed.					
			
			MHSetState (thresholdFileInfoHandle, handleStatus);
		#endif	// defined multispec_win

      #if defined multispec_lin
         SignedByte							handleStatus;
			
			fileNamePPointer = (FileStringPtr)GetFileNamePPointerFromFileHandle (
																				thresholdFileInfoHandle,
																				&handleStatus);

			RecodeThematicImageDialogSetThresholdFileName (fileNamePPointer,
																			popUpSelectThresholdImageMenu,
																			dialogPtr,
																			thresholdPopupItemNumber,
																			3);
		
					// Unlock the class name structure handle if needed.					
			
			MHSetState (thresholdFileInfoHandle, handleStatus);
      #endif
		
		}	// end "if (thresholdImageSelection == 3)"
	
}	// end "RecodeThematicImageDialogSetThresholdItems"  

