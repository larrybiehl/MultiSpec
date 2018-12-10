//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//							  (c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SGAIARoutines.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			02/07/2018
//
//	Language:				C
//
//	System:					Linux, Macintosh and Windows Operating Systems
//
//	Brief description:	The routines in this file handle saving and reading
//								Geographic Access Information and Analysis (GAIA) 
//								formatted images. These routinea handle images 
//								in the special format used by the GAIA software software
//								licensed to participating schools by the Island Institute
//								of Rockland, Maine. Features were made available in 
//								MultiSpec during the 1990's to support the 
//								Gaia-Crossroads Program which
//								was sponsered by the Bigelow Laboratory for Ocean
//								Sciences of Toothbay Harbor, Maine. A reference to
//								some of this work is in "MultiSpec in the Classroom" by 
//								Larry Ryan, Viewpoint:EARTH, 66 Fletcher Street, 
//								Kennebunk, ME 04043.
//
//	Functions in file:	Boolean				ConvertLineToGAIAFormat
//								Boolean				CopyToNewGAIAHeader
//								void 					ForceGAIACodeResourceLoad
//								Boolean				GetGAIAClassMeans
//								SInt16	 			GetGAIAHeaderParameters
//								Boolean 				ReadGAIAClassNames
//								SInt16	 			ReadGAIAHeader
//								SInt32 				GetNumberBytesInGAIALine
//								void					InitializeGAIALineBytes
//								Boolean 				WriteGAIAHeader
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
//-----------------------------------------------------------------------------

#include "SMultiSpec.h" 

#if defined multispec_lin
	#include "SMultiSpec.h"
#endif 
                             
#if defined multispec_win
	//#include	"CDatFile.h"
#endif	// defined multispec_win

//#include	"SExtGlob.h"

															
Boolean GetGAIAClassMeans (
					ChannelStatisticsPtr					classChannelStatsPtr, 
					SInt16									numberChannels, 
					SInt16*									channelListPtr, 
					SInt16									classNumber);

SInt16 GetGAIAHeaderParameters (
					 FileInfoPtr							fileInfoPtr,
					 char*									bufferPtr,
					 SInt32*									numberChannelsPtr,
					 SInt32*									numberFieldsPtr,
					 SInt32*									numberPoolsPtr,
					 SInt32*									numberStatChannelsPtr,
					 SInt32*									numberStatClassesPtr);

																
//-----------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							  (c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt32 ConvertLineToGAIAFormat
//
//	Software purpose:	The purpose of this routine is to convert the input
//							1-byte line of data a GAIA formatted line of data. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			WriteClassificationResults in classify.c
//							CovertMultispectralToThematic in fieldsToThematic.c
//
//	Coded By:			Larry L. Biehl			Date: 01/20/1994
//	Revised By:			Larry L. Biehl			Date: 02/03/1994

SInt32 ConvertLineToGAIAFormat (
				HUCharPtr							inputBufferPtr,
				HUInt16Ptr							outputBufferPtr,
				SInt32								numberSamples,
				SInt32								lineNumber)

{
	SInt16								copyType;
	
	HUInt16Ptr							savedOutputBufferPtr;
	
	HUInt16Ptr							outputBuffer2Ptr;
				
	SInt32								firstSample,
											lastSample,
											numberSegments,
											sample,
											samplesToMove;
	
	
	savedOutputBufferPtr = outputBufferPtr;
	
	copyType = 1;
	if (inputBufferPtr == NULL)
		copyType = 2;
	
			// Load the data into GAIA format - 2 byte, byte reversed 			
			// and preline information.													
			// It is assumed that the preline information has already be 		
			// loaded.																			
			// Allow for segmented bytes.	 These will occur where columns		
			// 1016, 2038, 3060, 4082, etc would be.									
		
	outputBufferPtr += 7;
			
	numberSegments = GetNumberGAIALineSegments (numberSamples);
	
	firstSample = numberSegments * 1021 - 5;
	firstSample = MAX (0, firstSample);
	lastSample = numberSamples;
	
	if (copyType == 1)
		inputBufferPtr = &inputBufferPtr[lastSample];
		
	outputBufferPtr = &outputBufferPtr[lastSample];
	outputBuffer2Ptr = outputBufferPtr + numberSegments;

	while (lastSample > 0)
		{
		samplesToMove = lastSample - firstSample + 1;
		
		if (copyType == 1)
			{
			for (sample=0; sample<samplesToMove; sample++)
				{
				inputBufferPtr--;
				outputBuffer2Ptr--;
				*outputBuffer2Ptr = *inputBufferPtr;
				
				}	//	end "for (sample=0; sample<samplesToMove; sample)" 
   
   		}	// end "if (copyType == 1)" 
   			
		else		// copyType != 1 
			{
			outputBufferPtr -= samplesToMove;
			outputBuffer2Ptr -= samplesToMove;
			if (numberSegments > 0)
				BlockMoveData (outputBufferPtr, 
								outputBuffer2Ptr, 
								samplesToMove*2);
	
			}	// end "else copyType != 1" 

		lastSample = firstSample - 1;
		firstSample -= 1021;
		firstSample = MAX (0, firstSample);

		if (lastSample > 0)
			{
			outputBuffer2Ptr--;
			
					// Add the segmented byte.												
				
   		*outputBuffer2Ptr = 0;
   		
   		}	// end "if (lastSample > 0)" 
	
		}	//	end "while (lastSample > 0)" 
   		
   		// Now put the line number in.
   														
   lineNumber = GetLongIntValue ((char*)&lineNumber);	
	inputBufferPtr = (HUCharPtr)savedOutputBufferPtr;
	BlockMoveData ((Ptr)&lineNumber, (Ptr)&inputBufferPtr[10+copyType], 4); 
	
			// Now indicate how many bytes that are actually in the buffer.	
			
	numberSamples += 7 + numberSegments;
	numberSamples *= 2;
	
	return (numberSamples);
		
}	// end "ConvertLineToGAIAFormat" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							  (c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CopyToNewGAIAHeader
//
//	Software purpose:	The purpose of this routine is to write a new GAIA
//							header based on the input GAIA header including changes
//							in the number of lines and columns only. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 10/07/1992
//	Revised By:			Larry L. Biehl			Date: 10/08/1992

Boolean CopyToNewGAIAHeader (
				FileInfoPtr							inFileInfoPtr, 
				FileInfoPtr							outFileInfoPtr, 
				HPtr									headerRecordPtr,
				SInt32								lineInterval, 
				SInt32								columnInterval)

{
	CMFileStream						*inFileStreamPtr,
											*outFileStreamPtr;
	
	SInt32								interval,
											tempLongInt,
											numberFields,
											numberChannels,
											numberGAIAClasses,
											numberPools;
	
	UInt32								count;
	
	SInt16								inErrCode,
											outErrCode,
											index;
	
	Boolean								releaseHeaderRecordPtrFlag;
	
	
	if (!headerRecordPtr || !inFileInfoPtr || !outFileInfoPtr)
																						return (FALSE);
	
	releaseHeaderRecordPtrFlag = FALSE;
	inFileStreamPtr = GetFileStreamPointer (inFileInfoPtr);
	outFileStreamPtr = GetFileStreamPointer (outFileInfoPtr);
	
			// Set the file position to read the beginning of the header 			
			// record.																				
	
	inErrCode = MSetMarker (inFileStreamPtr, 
									fsFromStart, 
									0,
									kErrorMessages);
	
	if (inErrCode == noErr)                                
		outErrCode = MSetMarker (outFileStreamPtr, 
										 fsFromStart, 
										 0,
										 kErrorMessages);
	
	if (inErrCode == noErr && outErrCode == noErr)
		{
				// Read the larsys type 1 record.											
		
		count = 58;
		inErrCode = MReadData (inFileStreamPtr, 
									  &count, 
									  headerRecordPtr,
									  kErrorMessages);
		
		if (inErrCode == noErr)
			{
					// Write the LARSYS type 1 record.										
			
			count = 58;		
			outErrCode = MWriteData (outFileStreamPtr, 
											&count, 
											headerRecordPtr,
											kErrorMessages);
			
			}	// end "if (errCode == noErr)" 
		
		}	// end "if (inErrCode == noErr && ...)" 
	
	if (inErrCode == noErr && outErrCode == noErr)
		{
				// Read the larsys type 2 record.											
		
		count = 26;
		inErrCode = MReadData (inFileStreamPtr, 
									  &count, 
									  headerRecordPtr,
									  kErrorMessages);
		
		if (inErrCode == noErr)
			{
					// Get number of classes in the thematic image file.				
			
			numberGAIAClasses = GetLongIntValue ((char*)&headerRecordPtr[10]);
			
					// Get number of channels used in the classification of the 	
					// thematic image file.														
			
			numberChannels = GetLongIntValue ((char*)&headerRecordPtr[14]);
			
					// Get number of fields.													
			
			numberFields = GetLongIntValue ((char*)&headerRecordPtr[18]);
			
					// Get number of pools.														
			
			numberPools = GetLongIntValue ((char*)&headerRecordPtr[22]);
			
					// Read the rest of the type 2 record.									
			
			count = (2 + 2 + 4 + 4)*numberChannels + 18*numberGAIAClasses + 20;
			inErrCode = MReadData (inFileStreamPtr, 
										  &count, 
										  &headerRecordPtr[26],
										  kErrorMessages);
			
					// Write the LARSYS type 2 record.										
			
			count += 26;
			if (inErrCode == noErr)
				outErrCode = MWriteData (outFileStreamPtr, 
													&count, 
													headerRecordPtr,
													kErrorMessages);
			
			}	// end "if (errCode == noErr)" 
		
		}	// end "if (inErrCode == noErr && ...)" 
	
	if (inErrCode == noErr && outErrCode == noErr)
		{
				// Now verify that the size of headerRecordPtr is large enough		
				// for reading and writing the rest of the header.						
		
		tempLongInt = 4 * numberChannels*(numberChannels+1)/2;
		tempLongInt = MAX (
								tempLongInt, 46 + 12*numberChannels + 18*numberGAIAClasses);
		tempLongInt = MAX (tempLongInt, 800);
		
		if (tempLongInt > MGetPointerSize (headerRecordPtr))
			{
			headerRecordPtr = (HPtr)MNewPointer (tempLongInt);
			if (headerRecordPtr)
				releaseHeaderRecordPtrFlag = TRUE;
			
			else	// !headerRecordPtr
				inErrCode = 1;
			
			}	// end "if (tempLongInt < GetPtrSize (headerRecordPtr))" 
		
		}	// end "if (inErrCode == noErr && ...)" 
	
	if (inErrCode == noErr && outErrCode == noErr)
		{
				// Read and write all type 3 records.										
		
		count = 90;
		inErrCode = MReadData (inFileStreamPtr, 
									  &count, 
									  headerRecordPtr,
									  kErrorMessages);
		
		while (inErrCode == noErr && outErrCode == noErr && 
				 headerRecordPtr[2] == 0x03)
			{
					// Write the LARSYS type 3 record.										
			
			outErrCode = MWriteData (outFileStreamPtr, 
												&count, 
												headerRecordPtr,
												kErrorMessages);
			
			if (outErrCode == noErr)
				inErrCode = MReadData (inFileStreamPtr, 
											  &count, 
											  headerRecordPtr,
											  kErrorMessages);
			
			}	// end "while (errCode == noErr && ..." 
		
		}	// end "if (inErrCode == noErr && ...)" 
	
	if (inErrCode == noErr && outErrCode == noErr)
		{
				// Write type 4 header record.												
		
		count = 10;
		outErrCode = MWriteData (outFileStreamPtr, 
											&count, 
											headerRecordPtr,
											kErrorMessages);
		
				// Set file position back to the end of the type 4 header.			
		
		SInt64 markerCount = -80;
		if (outErrCode == noErr)                                                 
			inErrCode = MSetMarker (inFileStreamPtr, 
											fsFromMark, 
											markerCount,
											kErrorMessages);
		
		}	// end "if (inErrCode == noErr && ...)" 
	
	if (inErrCode == noErr && outErrCode == noErr)
		{
				// Type 4 record continued.													
				// Compute the number of bytes for the pooled covariance 			
				// statistics for one class.													
		
		count = numberChannels*(numberChannels+1)/2;
		count *= 4;
		
		for (index=1; index<=numberGAIAClasses; index++)
			{
					// Read then write the pooled covariances.							
			
			inErrCode = MReadData (inFileStreamPtr, 
										  &count, 
										  headerRecordPtr,
										  kErrorMessages);
			
			if (inErrCode == noErr)
				outErrCode = MWriteData (outFileStreamPtr, 
													&count, 
													headerRecordPtr,
													kErrorMessages);
			
			if (inErrCode != noErr || outErrCode != noErr)
				break;
			
			}	// end "for (index=1; index<=...)" 
		
		}	// end "if (inErrCode == noErr && ...)" 
	
	if (inErrCode == noErr && outErrCode == noErr)
		{
				// Type 4 record continued.													
				// Compute the number of bytes for the pooled mean 					
				// statistics for one class.													
		
		count = numberChannels * 4;
		
		for (index=1; index<=numberGAIAClasses; index++)
			{	
						// Read then write the pooled means.									
				
				inErrCode = MReadData (inFileStreamPtr, 
											  &count, 
											  headerRecordPtr,
											  kErrorMessages);
				
				if (inErrCode == noErr)
					outErrCode = MWriteData (outFileStreamPtr, 
														&count, 
														headerRecordPtr,
														kErrorMessages);
				
				if (inErrCode != noErr || inErrCode != noErr)
					break;
				
			}	// end "for (index=1; index<=...)" 
		
		}	// end "if (inErrCode == noErr && ...)" 
	
	if (inErrCode == noErr && outErrCode == noErr)
		{
				// Read and write any bytes due to segmented records.					
		
		count = (numberChannels*(numberChannels+1)/2) * 4 + numberChannels * 4;
		count *= numberPools;
		count = (count/1872) * 2;
		
		if (count > 0)
			{
			inErrCode = MReadData (inFileStreamPtr, 
										  &count, 
										  headerRecordPtr,
										  kErrorMessages);
			
			if (inErrCode == noErr)
				outErrCode = MWriteData (outFileStreamPtr, 
													&count, 
													headerRecordPtr,
													kErrorMessages);
			
			}	// end "if (count > 0)" 
		
		}	// end "if (inErrCode == noErr && ...)" 
	
	if (inErrCode == noErr && outErrCode == noErr)
		{
				// The Type 5 record. header in INFO array.								
		
		count = 86;		
		inErrCode = MReadData (inFileStreamPtr, 
									  &count, 
									  headerRecordPtr,
									  kErrorMessages);
		
				// Modify the number of columns per line.									
		
		tempLongInt = GetLongIntValue ((char*)&outFileInfoPtr->numberColumns);
		BlockMoveData ((char*)&tempLongInt, &headerRecordPtr[10], 4);
		
				// Modify the number of lines.												
		
		tempLongInt = GetLongIntValue ((char*)&outFileInfoPtr->numberLines);
		BlockMoveData ((char*)&tempLongInt, &headerRecordPtr[14], 4);
		
				// Modify INFO(4) = First line of area classified.						
		
		tempLongInt = outFileInfoPtr->startLine;
		tempLongInt = GetLongIntValue ((char*)&tempLongInt);
		BlockMoveData ((char*)&tempLongInt, &headerRecordPtr[30], 4);
		
				// Modify INFO(5) = Last line of area classified.						
				// Get the original line interval.											
				// And adjust for the compounded line interval, if any.				
		
		interval = GetLongIntValue (&headerRecordPtr[38]);
		interval *= lineInterval;
		
		tempLongInt = outFileInfoPtr->startLine + interval * 
		(outFileInfoPtr->numberLines - 1);
		tempLongInt = GetLongIntValue ((char*)&tempLongInt);
		BlockMoveData ((char*)&tempLongInt, &headerRecordPtr[34], 4);
		
				// Modify INFO(6) = Line interval used for classification.			
		
		tempLongInt = GetLongIntValue ((char*)&interval);
		BlockMoveData ((char*)&tempLongInt, &headerRecordPtr[38], 4);
		
				// Modify INFO(7) = First column of area classified.					
		
		tempLongInt = outFileInfoPtr->startColumn;
		tempLongInt = GetLongIntValue ((char*)&tempLongInt);
		BlockMoveData ((char*)&tempLongInt, &headerRecordPtr[42], 4);
		
				// Modify INFO(8) = Last column of area classified.					
				// Get the original column interval.										
				// And adjust for the compounded column interval, if any.			
		
		interval = GetLongIntValue (&headerRecordPtr[50]);
		interval *= columnInterval;
		
		tempLongInt = outFileInfoPtr->startColumn + interval * 
		(outFileInfoPtr->numberColumns - 1);
		tempLongInt = GetLongIntValue ((char*)&tempLongInt);
		BlockMoveData ((char*)&tempLongInt, &headerRecordPtr[46], 4);
		
				// Modify INFO(9) = Column interval used for classification.		
		
		tempLongInt = GetLongIntValue ((char*)&interval);
		BlockMoveData ((char*)&tempLongInt, &headerRecordPtr[50], 4);
		
		if (inErrCode == noErr)
			outErrCode = MWriteData (outFileStreamPtr, 
												&count, 
												headerRecordPtr,
												kErrorMessages);
		
		}	// end "if (inErrCode == noErr && ...)" 
	
	if (inErrCode == noErr && outErrCode == noErr)
		{
				// Type 5 continued.  Read then write the ID array.					
		
		count = 800;
		inErrCode = MReadData (inFileStreamPtr, 
									  &count, 
									  headerRecordPtr,
									  kErrorMessages);
		
		if (inErrCode == noErr)
			outErrCode = MWriteData (outFileStreamPtr, 
												&count, 
												headerRecordPtr,
												kErrorMessages);
		
		}	// end "if (inErrCode == noErr && ...)" 
	
	if (inErrCode == noErr && outErrCode == noErr)
		{
				// Type 5 continued.  Read then write the CSET array.					
		
		count = 360;	
		inErrCode = MReadData (inFileStreamPtr, 
									  &count, 
									  headerRecordPtr,
									  kErrorMessages);
		
		if (inErrCode == noErr)
			outErrCode = MWriteData (outFileStreamPtr, 
												&count, 
												headerRecordPtr,
												kErrorMessages);
		
		}	// end "if (inErrCode == noErr && ...)" 
	
	if (releaseHeaderRecordPtrFlag)
		CheckAndDisposePtr (headerRecordPtr);
	
	return (inErrCode == noErr && outErrCode == noErr);
	
}	// end "CopyToNewGAIAHeader"


/*
//-----------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ForceGAIACodeResourceLoad
//
//	Software purpose:	The purpose of this is to force the 'GAIA' code
//							resource to be loaded into memory.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 01/21/1994
//	Revised By:			Larry L. Biehl			Date: 03/23/2005

void ForceGAIACodeResourceLoad (void)

{
			// If spare memory had to be used to load code resources, then exit	
			// routine.																				
			
	if (gMemoryTypeNeeded > 0)
		{								
				// Code resources loaded okay, so set flag back for non-Code			
				// resources.																			
				
		gMemoryTypeNeeded = 0;
			
		}	// end "if (gMemoryTypeNeeded >= 0)"

}	// end "ForceGAIACodeResourceLoad" 
*/
																																
    
#if defined multispec_mac || defined multispec_mac_swift
//-----------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							  (c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetGAIAClassMeans
//
//	Software purpose:	The purpose of this routine is to write the GAIA header.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			WriteGAIAHeader in gaiaRoutines.c
//
//	Coded By:			Larry L. Biehl			Date: 01/20/1994
//	Revised By:			Larry L. Biehl			Date: 01/20/1994

Boolean GetGAIAClassMeans (
					ChannelStatisticsPtr		classChannelStatsPtr, 
					SInt16						numberChannels, 
					SInt16*						channelListPtr, 
					SInt16						classNumber)

{
	RGBColor				theColor;
	Boolean				continueFlag;
	
	
	continueFlag = TRUE;
	
			// Get the mean vector for the class.										
	
	if (gProcessorCode != kMultispecToThematicProcessor)
		continueFlag = GetClassChannelStatistics (
							numberChannels, 
							classChannelStatsPtr, 
							(UInt16*)channelListPtr, 
							classNumber);
	
	#if defined multispec_mac
		else	// gProcessorCode == kMultispecToThematicProcessor
			{
					// Copy the number of colors need from the resouce 				
					// palette to the display palette.  									
					// The palette handle is store in a temp handle.					
					
			GetEntryColor ((PaletteHandle)gCharBufferHandle3, classNumber, &theColor);
						
			classChannelStatsPtr[0].mean = (theColor.blue >> 8);
			classChannelStatsPtr[1].mean = (theColor.green >> 8);
			classChannelStatsPtr[2].mean = (theColor.red >> 8);
			
			}	// end "else ... == kMultispecToThematicProcessor"
	#endif
		
	return (continueFlag);
		
}	// end "GetGAIAClassMeans"
#endif	// defined multispec_mac || defined multispec_mac_swift



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetGAIAHeaderParameters
//
//	Software purpose:	The purpose of this routine is to get some variables
//							in the GAIA header which are needed to location
//							specific data within the header.  These variables
//							are number of classes, number of class pools, number of
//							fields, number of channels, number of statistics channels
//							and number of statistics classes.
//
//	Parameters in:		window
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
// Called By:			LoadClassNameDescriptions in fileIO.c
//
//	Coded By:			Larry L. Biehl			Date: 02/08/1994
//	Revised By:			Larry L. Biehl			Date: 02/08/1994	

SInt16 GetGAIAHeaderParameters (
				FileInfoPtr							fileInfoPtr,
				char*									bufferPtr,
				SInt32*								numberChannelsPtr,
				SInt32*								numberFieldsPtr,
				SInt32*								numberPoolsPtr,
				SInt32*								numberStatChannelsPtr,
				SInt32*								numberStatClassesPtr)

{
	CMFileStream*						fileStreamPtr;

	SInt64								filePosition;
	
	SInt32								numberGAIAClasses;
	
	UInt32								count;
	
	SInt16								errCode;
	
	
	if (	fileInfoPtr == NULL || bufferPtr == NULL)
																								return (1);
	
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
	if (fileStreamPtr == NULL)
																								return (1);
	
			// Load in the first buffer of data.											
	
	filePosition = 72;
	errCode = MSetMarker (fileStreamPtr, 
								 fsFromStart, 
								 filePosition, 
								 kErrorMessages);
	
	if (errCode == noErr)
		{
		count = 12;
		errCode = MReadData (fileStreamPtr, 
									&count, 
									bufferPtr, 
									kErrorMessages);
		
		}	// end "if (errCode == noErr)" 
	
	if (errCode == noErr)
		{
				// Get the number of classes.  This is already known.					
		
		numberGAIAClasses = fileInfoPtr->numberClasses - 1;
		
				// Get number of channels used in the classification of the 		
				// thematic image file.															
		
		*numberChannelsPtr = GetLongIntValue (bufferPtr);
		*numberChannelsPtr = (*numberChannelsPtr & 0x000000ff);
		
				// Get number of fields.														
		
		*numberFieldsPtr = GetLongIntValue (&bufferPtr[4]);
		*numberFieldsPtr = (*numberFieldsPtr & 0x000000ff);
		
				// Get number of pools.															
		
		*numberPoolsPtr = GetLongIntValue (&bufferPtr[8]);
		*numberPoolsPtr = (*numberPoolsPtr & 0x000000ff);
		
		}	// end "if (errCode == noErr)" 
	
			// Read in a buffer of data which should contain the summary.			
	
	if (errCode == noErr && numberStatChannelsPtr != NULL)
		{
				// Determine the number of bytes to skip to get to the 				
				// statistics deck summary information.									
				// Skip Type 1 record.															
		
		filePosition = 58; 
		
				// Skip Type 2 record.															
		
		filePosition += 46 + 12 * *numberChannelsPtr + 
		2 * numberGAIAClasses + 16 * *numberPoolsPtr; 
		
				// First part of type 3 record.												
		
		filePosition += 90 + 90 * *numberFieldsPtr + 90 * numberGAIAClasses; 
		filePosition += 10; 
		
				// Set the file position to read in the statistics deck 				
				// summary.																			
		
		errCode = MSetMarker (fileStreamPtr, 
									 fsFromStart, 
									 filePosition, 
									 kErrorMessages);
		
		if (errCode == noErr)
			{
			count = 35;
			errCode = MReadData (fileStreamPtr, 
										&count, 
										bufferPtr, 
										kErrorMessages);
			
			}	// end "if (errCode == noErr)" 
		
				// Note below that if errCode contains 1 on return from sscanf, 	
				// then there is no error.														
		
		if (errCode == noErr)
			{
			errCode = sscanf (bufferPtr, 
									"%ld", 
									numberStatClassesPtr);
			
			errCode -= 1;
			
			}	// end "if (errCode == noErr)" 
		
		if (errCode == noErr)
			{
			bufferPtr += 22;
			errCode = sscanf (bufferPtr, 
									"%ld",
									numberStatChannelsPtr);
			
			errCode -= 1;
				
			}	// end "if (errCode == noErr)" 
		
		}	// end "if (errCode == noErr && ...)" 
		
	return (errCode);
	
}	// end "GetGAIAHeaderParameters"

																
																
//-----------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							  (c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetNumberBytesInGAIALine
//
//	Software purpose:	The purpose of this routine is to determine the
//							number of bytes in a line of GAIA image data.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/02/1994
//	Revised By:			Larry L. Biehl			Date: 02/02/1994

SInt32 GetNumberBytesInGAIALine (
				SInt32								numberColumns)

{
	SInt32								numberOutputBytes;
	
		
	numberOutputBytes = numberColumns;
	numberOutputBytes *= 2;
	
			// Really only need 14 more bytes unless line is to be offset		
			// to allow a quick way to swap the bytes - then need 15 bytes.	
			// Will allow 18 for some extra.												
			
	numberOutputBytes += 18;
		
			// Allow for 'segmented record bytes'.										
				
	numberOutputBytes += GetNumberGAIALineSegments (numberColumns) * 2;
	
	return (numberOutputBytes);
		
}	// end "GetNumberBytesInGAIALine" 

																
																
//-----------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							  (c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void InitializeGAIALineBytes
//
//	Software purpose:	The purpose of this routine is to initialize the
//							GAIA preline bytes for one line of data.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			WriteGAIAHeader in gaiaRoutines.c
//
//	Coded By:			Larry L. Biehl			Date: 01/20/1994
//	Revised By:			Larry L. Biehl			Date: 01/20/1994

void InitializeGAIALineBytes (
				HUInt16Ptr							buffer2Ptr,
				SInt32								numSamplesPerChan,
				SInt16								offset)

{
	char									GAIAPrelineString[14] = 
													{	0x03, 0x00, 0x06, 0x00, 0x00, 0x00, 
														0x01,	0x00, 0x00, 0x00, 0x00, 0x00, 
														0x00, 0x00};
														
	HUCharPtr							buffer1Ptr;
													
	SInt32								index,
											segmentedPixels;
	
	
	buffer1Ptr = (HUCharPtr)buffer2Ptr;									
	
	if (offset == 0)
		BlockMoveData (&GAIAPrelineString, buffer1Ptr, 14);
		
	else	// offset != 0
		{							
	   buffer1Ptr[0] = 0x00;	
		BlockMoveData (&GAIAPrelineString, &buffer1Ptr[1], 13);
		
		}	// end "else offset != 0" 
														
			// Make certain that the last byte after the last sample				
			// is zero.  The actual data to be written to disk will 				
			// start at the first byte of the array not the zero byte 			
			// so that the bytes will not have to be swapped.						
		
   buffer2Ptr += 7 + numSamplesPerChan;
   *buffer2Ptr = 0;
   buffer2Ptr++;
	   	
   		// Handle any segmented bytes.												
   		// 'ipixels is used as a count of the number of segmented			
   		// short (2 byte) words.														
   			
   segmentedPixels = GetNumberGAIALineSegments (numSamplesPerChan);
	for (index=0; index<segmentedPixels; index++)
		{
   	*buffer2Ptr = 0;
   	buffer2Ptr++;
   		
   	}	// end "for (index=0; index<segmentedPixels; index++)" 
		
}	// end "InitializeGAIALineBytes" 



//-------------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ReadGAIAClassNames
//
//	Software purpose:	The purpose of this routine is to read the class
//							names from a GAIA classification file.
//
//	Parameters in:		window
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			LoadClassNameDescriptions in fileIO.c
//
//	Coded By:			Larry L. Biehl			Date: 09/30/1992
//	Revised By:			Larry L. Biehl			Date: 02/07/2018

Boolean ReadGAIAClassNames (
				FileInfoPtr							fileInfoPtr, 
				UCharPtr								classNamePtr)

{
	CMFileStream*						fileStreamPtr;
	
	UInt32								classIndex,
											count,
											numberGAIAClasses;

	SInt64								filePosition;
	
	SInt32								numberChannels,
											numberFields,
											numberPools;
	
	UInt16*								classSymbolPtr;
	
	SInt16								errCode;
	
	
	if (fileInfoPtr == NULL || classNamePtr == NULL)
		return (FALSE);
	
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
	if (fileStreamPtr == NULL)
																							return (FALSE);
	
	gSwapBytesFlag = gBigEndianFlag;
	
			// Get some variables to be used to determine where the class names	
			// are within the file.																
	
	errCode = GetGAIAHeaderParameters (fileInfoPtr,
												  (char*)gTextString3,
												  &numberChannels,
												  &numberFields,
												  &numberPools,
												  NULL,
												  NULL);
	
   numberGAIAClasses = 0;
	if (errCode == noErr)
		{
				// Position the file pointer to where the class names should be	
				// located within the GAIA file.												
		
		numberGAIAClasses = fileInfoPtr->numberClasses - 1;
		filePosition = 
		210 + numberGAIAClasses*2 + numberPools*16 + numberChannels*12;
		
				// Get a pointer to storage for the class symbols.						
		
		classSymbolPtr = (UInt16*)&classNamePtr[
														fileInfoPtr->numberClasses*sizeof (Str31)];
		
				// Load the class name for the background class.							
		
		CopyPToP (classNamePtr, (UCharPtr)"\0background\0");
		
		classNamePtr += 32;
		
				// Load the assumed class symbol.											
		
		classSymbolPtr[0] = 0;
		
		}	// end "if (errCode == noErr)" 
	
	classIndex = 1;
	while (classIndex <= numberGAIAClasses && errCode == noErr)
		{
		errCode = MSetMarker (fileStreamPtr, 
									 fsFromStart, 
									 filePosition, 
									 kNoErrorMessages);
		
				// Read in a buffer of data.													
		
		if (errCode == noErr)
			{
			count = 31;
			errCode = MReadData (fileStreamPtr, 
										&count, 
										&classNamePtr[1], 
										kNoErrorMessages);
			
			filePosition += 180;
			
			}	// end "if (errCode == noErr)" 
		
		if (errCode == noErr)
			{
					// Copy the class name.														
			
			classNamePtr[0] = 31;
			
					// Load the assumed class symbol.										
			
			classSymbolPtr[classIndex] = (UInt16)classIndex;
			
			classNamePtr += 32;
			classIndex++;
			
			}	// end "while (classIndex < fileInfoPtr->..." 
		
		}	// end "while (classIndex < fileInfoPtr->numberClasses ..." 
	
			// If there was an error before all the class names could be 			
			// loaded in, finish the names with a default set.							
	
	while (classIndex <= numberGAIAClasses)
		{
		CreateDefaultClassName (classNamePtr, classIndex);
		
		classIndex++;
		classNamePtr += 32;
		
		}	// end "while (classIndex < fileInfoPtr->numberClasses)"
	
	gSwapBytesFlag = FALSE; 
	
	return (TRUE);
	
}	// end "ReadGAIAClassNames" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							  (c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 ReadGAIAHeader
//
//	Software purpose:	The purpose of this routine is to read the GAIA header
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/22/1992
//	Revised By:			Larry L. Biehl			Date: 09/08/2006

SInt16 ReadGAIAHeader (
				FileInfoPtr							fileInfoPtr,
				char*									headerRecordPtr,
				SInt16								formatOnlyCode)

{
	CMFileStream*						fileStreamPtr;
	SInt16*								ioBufferPtr;

	SInt64								filePosition;
	
	UInt32								count;
	
	SInt32								fileType,
											numberChannels,
											numberClasses,
											numberFields,
											numberPools,
											numberStatChannels,
											numberStatClasses,
											systemFileCreator,
											systemFileType,
											tempLongInt;
	
	SInt16								errCode,
											returnCode;
	
	
	if (headerRecordPtr == NULL || fileInfoPtr == NULL)
																								return (0);
	
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
	if (fileStreamPtr == NULL)
																								return (0);
	
	returnCode = 0;
	fileType = 0;		
	
			// Check if this is a GAIA formatted image file.
	
	systemFileType = GetFileType (fileStreamPtr);
	systemFileCreator = GetFileCreator (fileStreamPtr);
	
	if (systemFileCreator == kGAIACreator && systemFileType == kGAIAFileType)
		fileType = kGAIAType;
	
	if (headerRecordPtr[18] != 0x03)
		fileType = 0;						
	
	if (fileType == 0)
																								return (0);	
	
	if (formatOnlyCode != kLoadHeader)
		{
		fileInfoPtr->format = (SInt16)fileType;
		fileInfoPtr->thematicType = TRUE;
		
																								return (0);
		
		}	// end "if (formatOnlyCode != kLoadHeader)"	
	
	gSwapBytesFlag = gBigEndianFlag;
	
	if (gGetFileImageType == 0)
		gGetFileImageType = kThematicImageType;
	
			// Get number of classes in the thematic image file.
			// Allow for the background class.
	
	numberClasses = GetLongIntValue ((char*)&headerRecordPtr[68]);
	numberClasses = (numberClasses & 0x000000ff);
	fileInfoPtr->numberClasses = numberClasses;
	fileInfoPtr->numberClasses++;
	fileInfoPtr->maxClassNumberValue = fileInfoPtr->numberClasses - 1;
	
			// Get some variables to be used to determine where other records		
			// are within the file.																
	
	errCode = GetGAIAHeaderParameters (fileInfoPtr,
												  headerRecordPtr,
												  &numberChannels,
												  &numberFields,
												  &numberPools,
												  &numberStatChannels,
												  &numberStatClasses);
	
	if (errCode == noErr)
		{
				// Determine the number of bytes to skip to get to the statistics
				// deck summary information.
				// Type 1 record.
		
		filePosition = 58; 
		
				// Type 2 record.
		
		filePosition += 46 + 12*numberChannels + 2*numberClasses + 16*numberPools; 
		
				// First part of type 3 record.
		
		filePosition += 90 + 90 * numberFields + 90 * numberClasses; 
		filePosition += 10; 
		
				// Determine the number of bytes to skip to get to the number of
				// lines and number of columns in the file.
				// Skip rest of the statistics deck summary line.
		
		filePosition += 80; 
		
				// Skip the channel wavelength and calibration information.
		
		filePosition += 90 * numberStatChannels; 
		
				// Skip the number of training pixels per class information.
				// There will be 7 classes per line.
		
		count = (numberStatClasses + 6)/7;
		filePosition += 90 * count; 
		
				// Skip the statistics deck class mean vectors.
				//	There will be 5 mean values per line.
		
		count = (numberStatChannels + 4)/5;
		filePosition += 90 * count * numberStatClasses; 
		
				// Skip the statistics deck class covariances.							
				//	There will be 5 covariance values per line.							
		
		count = (numberStatChannels*(numberStatChannels+1)/2 + 4)/5;
		filePosition += 90 * count * numberStatClasses;
		
				// Skip the last card of the statistics deck.							
		
		filePosition += 90;
		
				// Skip the pooled statistics Type 4 record header.					
		
		filePosition += 10;
		
				// Skip the pooled covariance statistics.									
		
		count = numberChannels*(numberChannels+1)/2;
		filePosition += numberPools * count * 4;
		
				// Skip the pooled mean statistics.											
		
		filePosition += numberPools * numberChannels * 4;
		
				// Skip bytes for 'segmented type 4 records'.  I do not 				
				// understand the reason for this.  GAIA requires this.  I 			
				// assume that it has something to do with old tape file records	
				// or older disk file records which could not be longer than		
				// 1872 bytes.  Larry L. Biehl - 9-30-92									
		
		count = (numberChannels*(numberChannels+1)/2) * 4 + numberChannels * 4;
		count *= numberPools;
		count = (count/1872) * 2;
		filePosition += count;
		
				// Skip the Type 5 record header.											
		
		filePosition += 10;
		
				// Set the file position to read in the number of line & columns.	
		
		errCode = MSetMarker (fileStreamPtr, fsFromStart, filePosition, kErrorMessages);
		
		if (errCode == noErr)
			{
					// Read in a buffer of data which should contain the line &			
					// column information.															
			
			count = 44;
			errCode = MReadData (fileStreamPtr, 
										&count, 
										headerRecordPtr, 
										kErrorMessages);
			
			if (errCode == noErr)
				{
						// Get the number of columns in the image.						
				
				BlockMoveData ((char*)headerRecordPtr, (char*)&tempLongInt, 4);
				fileInfoPtr->numberColumns = GetLongIntValue ((char*)&tempLongInt);
				
						// Get the number of lines in the image.							
				
				BlockMoveData ((char*)&headerRecordPtr[4], (char*)&tempLongInt, 4);
				fileInfoPtr->numberLines = GetLongIntValue ((char*)&tempLongInt);
				
						// Get the start line.													
				
				BlockMoveData ((char*)&headerRecordPtr[20], (char*)&tempLongInt, 4);
				fileInfoPtr->startLine = GetLongIntValue ((char*)&tempLongInt);
				
						// Get the start column.												
				
				BlockMoveData ((char*)&headerRecordPtr[32], (char*)&tempLongInt, 4);
				fileInfoPtr->startColumn = GetLongIntValue ((char*)&tempLongInt);
				
				}	// end "if (errCode == noErr)" 
			
			}	// end "if (errCode == noErr)" 
		
		}	// end "if (errCode == noErr)" 
	
	if (errCode == noErr)
		{
				// Skip the number of lines and columns in the file.					
		
		filePosition += 8;
		
				// Skip the INFO vector - 17 4-byte words.								
		
		filePosition += 4 * 17;
		
				// Skip the ID vector - 800 bytes.											
		
		filePosition += 800;
		
				// Skip the CSET vector - 360 bytes.										
		
		filePosition += 360;
		
		fileInfoPtr->numberHeaderBytes = (UInt32)filePosition;
		
				// Load the rest of the information for GAIA formatted files.		
		
		fileInfoPtr->numberBytes = 2;
		fileInfoPtr->numberBits = 8;
		fileInfoPtr->numberBins = 256;
		fileInfoPtr->numberChannels = 1;
		fileInfoPtr->format = kGAIA2Type;
		fileInfoPtr->ancillaryInfoformat = kGAIA2Type;
		fileInfoPtr->swapBytesFlag = TRUE;
		fileInfoPtr->asciiSymbols = FALSE;
		fileInfoPtr->bandInterleave = kBIL;
		fileInfoPtr->numberTrailerBytes = 0;
		fileInfoPtr->numberPreLineBytes = 14;
		fileInfoPtr->numberPostLineBytes = 
		GetNumberGAIALineSegments (fileInfoPtr->numberColumns) * 2;
		fileInfoPtr->numberPreChannelBytes = 0;
		fileInfoPtr->numberPostChannelBytes = 0;
		
		if (fileInfoPtr->numberPostLineBytes > 0)
			{
					// Check if this file is of MultiSpec's original 'wrong' 	
					// GAIA format.  First read the 'correct' trailer byte		
					// and the 'wrong' trailer byte;	 If the 'correct' trailer	
					// byte is not 0 and the 'wrong' trailer byte is 0, then		
					// this is the 'wrong' formatted file.								
			
			//fileInfoPtr->format = kGAIAType;
			
			filePosition += fileInfoPtr->numberPreLineBytes + 1015 * 2;
			errCode = MSetMarker (fileStreamPtr, 
										 fsFromStart, 
										 filePosition, 
										 kNoErrorMessages);
			
			if (errCode == noErr)
				{
						// Read in the first 'correct' trailer byte.														
				
				count = 2;
				ioBufferPtr = (SInt16*)headerRecordPtr;
				errCode = MReadData (fileStreamPtr, 
											&count, 
											ioBufferPtr, 
											kNoErrorMessages);
				
				}	// end "if (errCode == noErr)" 
			
			if (errCode == noErr && *ioBufferPtr != 0)
				{
						// Get number of bytes in 1 line and 1 channel
				
				UInt32 bytesPer1line1chan = fileInfoPtr->numberPreChannelBytes + 
				fileInfoPtr->numberColumns * fileInfoPtr->numberBytes +
				fileInfoPtr->numberPostChannelBytes;
				
				filePosition += bytesPer1line1chan - 1015 * 2;
				errCode = MSetMarker (fileStreamPtr, 
											 fsFromStart, 
											 filePosition, 
											 kNoErrorMessages);
				
				if (errCode == noErr)
					{
							// Read in the first 'wrong' trailer byte.				
					
					count = 2;
					ioBufferPtr = (SInt16*)headerRecordPtr;
					errCode = MReadData (fileStreamPtr, 
												&count, 
												ioBufferPtr, 
												kNoErrorMessages);
					
					}	// end "if (errCode == noErr)" 
				
				if (errCode == noErr && *ioBufferPtr == 0)
					{
					fileInfoPtr->format = kGAIAType;
					fileInfoPtr->ancillaryInfoformat = kGAIAType;
					
					}	// end "if (errCode == noErr && *ioBufferPtr == 0)"
				
				}	// end "if (errCode == noErr && *ioBufferPtr != 0)" 
			
			}	// end "if (fileInfoPtr->numberPostLineBytes > 0)" 
		
		fileInfoPtr->thematicType = FALSE;
		if (gGetFileImageType == kThematicImageType)
			fileInfoPtr->thematicType = TRUE;
		
		}	// end "if (errCode == noErr)"										
	
	gSwapBytesFlag = FALSE; 
	
	if (errCode != noErr)												
		returnCode = 1;
	
	return (returnCode);
	
}	// end "ReadGAIAHeader"


			             													
//-----------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							  (c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void WriteGAIAHeader
//
//	Software purpose:	The purpose of this routine is to write the GAIA header.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			CreateResultsDiskFile in multiSpecUtilities.c
//							WriteNewImageHeader in reformat.c
//
//	Coded By:			Larry L. Biehl			Date: 09/18/1992
//	Revised By:			Larry L. Biehl			Date: 09/18/2006

Boolean WriteGAIAHeader (
				FileInfoPtr							fileInfoPtr, 
				HPtr									headerRecordPtr, 
				SInt16								numberChannels, 
				SInt16*								channelListPtr, 
				UInt32								numberClasses, 
				SInt16*								classPtr)

{    
#if defined multispec_mac || defined multispec_mac_swift
	CMFileStream*						fileStreamPtr;
												
	ChannelDescriptionPtr			channelDescriptionPtr;
	ChannelStatisticsPtr				classChannelStatsPtr;
	DisplaySpecsPtr					displaySpecsPtr;
	
	double								meanValue; 
	
	SInt64								tempS64Int;
	
	SInt32								classNumber,
											index,
											index2,
											tempLongInt;
												
	UInt32								count;
	
	SInt16								channel,
											channelList[3],
											classNameCode,
											classStorage,
											errCode,
											firstIndex,
											lastIndex,
											numberGAIAClasses,
											tempShortInt,
											*temp2ByteHeaderRecordPtr;
												
	UInt16								classIndex;
	
	char									*tempHeaderRecordPtr;
	
	Boolean								continueFlag,
											releaseHeaderRecordPtrFlag;
	
	
	
		
	if (fileInfoPtr == NULL)
																						return (FALSE);
																				
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
		
	numberGAIAClasses = fileInfoPtr->numberClasses - 1;
	classNameCode = 0;
	
	if (gProcessorCode == kMultispecToThematicProcessor)
		{
				// Update some variable the pertain to converting 						
				// MultiSpectral images to Thematic images processor.					
				
		displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
														gImageWindowInfoPtr->displaySpecsHandle);
		channelList[0] = displaySpecsPtr->blueChannelNumber - 1;
		channelList[1] = displaySpecsPtr->greenChannelNumber - 1;
		channelList[2] = displaySpecsPtr->redChannelNumber - 1;
		channelListPtr = channelList;
		numberChannels = 3;
		numberClasses = fileInfoPtr->numberClasses;
		classNameCode = kClassNumberOnly;
		
		}	// end "if (gProcessorCode == kMultispecToThematicProcessor)" 
											
	releaseHeaderRecordPtrFlag = FALSE;									
	if (!headerRecordPtr)
		{
				// Get some memory to use to load the GAIA header information.		
				
		count = 4 * numberChannels*(numberChannels+1)/2;
		count = MAX (count, 12 * numberChannels);
		count = MAX (count, 18 * numberGAIAClasses);
		count = MAX (count, 800);
		tempLongInt = 92 + numberChannels * sizeof (ChannelStatistics);
		count = MAX (count, tempLongInt);
		
		headerRecordPtr = (char*)MNewPointer (count);
		if (headerRecordPtr)
			releaseHeaderRecordPtrFlag = TRUE;
		
		}	// end "if (!headerRecordPtr)" 
		
	if (!headerRecordPtr)
																						return (FALSE);										
				
	gSwapBytesFlag = TRUE; 
		
			// Get pointer to memory to use for computation of the class mean		
			// vector.																				
			
	classChannelStatsPtr = (ChannelStatistics*)&headerRecordPtr[92];
		
			// Initialize LARSYS type 1 record to 0.										
																					
	memset (headerRecordPtr, 0, 58);
																				
			// Load the Type 1 record header (first 10 bytes).							
	
	headerRecordPtr[0] = 0x03;
	headerRecordPtr[2] = 0x01;
																				
			// Load the Type 1 record.															
			//	Bytes 1-4: Requested tape - not used.										
			//	Bytes 5-8: Requested file - not used.										
			//	Bytes 9-12: Long integer 3, remember byte swapping.					
			
	headerRecordPtr[18] = 0x03;
	
			//	Bytes 13-16: Long integer 0 - not used.									
			//	Bytes 17-20: Serial number - not used.										
			//	Bytes 21-24: Long integer 1, remember byte swapping.					
			
	headerRecordPtr[30] = 0x01;
	
			//	Bytes 25-48: Long integer 0's, - not used.								
	
			// Write the LARSYS type 1 record.												
	
	count = 58;		
	errCode = MWriteData (fileStreamPtr, &count, headerRecordPtr, kErrorMessages);
	
	if (errCode == noErr)
		{																
				// Load the Type 2 record header (first 10 bytes).						
		
		headerRecordPtr[0] = 0x03;
		headerRecordPtr[2] = 0x02;
	
				// Number of classes.															
				
		tempLongInt = numberGAIAClasses;
		tempLongInt = GetLongIntValue ((char*)&tempLongInt);
		BlockMoveData ((char*)&tempLongInt, &headerRecordPtr[10], 4);
		
				// Number of channels.															
				
		tempLongInt = numberChannels;
		tempLongInt = GetLongIntValue ((char*)&tempLongInt);
		BlockMoveData ((char*)&tempLongInt, (char*)&headerRecordPtr[14], 4);
		
				// Number of fields.																
				
		tempLongInt = numberGAIAClasses;
		tempLongInt = GetLongIntValue ((char*)&tempLongInt);
		BlockMoveData ((char*)&tempLongInt, (char*)&headerRecordPtr[18], 4);
		
				// Number of pools.																
				
		tempLongInt = numberGAIAClasses;
		tempLongInt = GetLongIntValue ((char*)&tempLongInt);
		BlockMoveData ((char*)&tempLongInt, (char*)&headerRecordPtr[22], 4);
		
				// Write the first part of the type 2 record to the file.			
		
		count = 26;		
		errCode = MWriteData (fileStreamPtr, &count, headerRecordPtr, kErrorMessages);
		
		}	// end "if (errCode == noErr)" 
	
	if (errCode == noErr)
		{
				// Type 2 record continued.													
				// List of channels used in the classification.							
				
		channel = 0;
		tempHeaderRecordPtr = headerRecordPtr;
		for (index=0; index<numberChannels; index++)
			{
			if (channelListPtr)
				channel = channelListPtr[index];
				
			channel++;
				
			tempShortInt = ((channel & 0xff00) >> 8) | ((channel & 0x00ff) << 8);
			BlockMoveData ((char*)&tempShortInt, tempHeaderRecordPtr, 2);
			
			tempHeaderRecordPtr += 2;
			
			}	// end "for (index=0; index<numberChannels; index++)" 
			
				// List calibration codes for the channels used in the 				
				// classification.  Fill with 0's; these are not used.				
				
		count = 2 * numberChannels;
		memset (tempHeaderRecordPtr, 0, count);
		tempHeaderRecordPtr += count;
			
				// List the lower limits of the spectral bands on the tape.  		
				// Then list the upper limits of the spectral bands on the tape.  
				// Fill with 0's; these are not used.										
				
		count = 8 * numberChannels;
		memset (tempHeaderRecordPtr, 0, count);
	
				// Write the record 2 channel information to the header.				
				
		count = (2 + 2 + 4 + 4) * numberChannels;	
		errCode = MWriteData (fileStreamPtr, &count, headerRecordPtr, kErrorMessages);
		
		}	// end "if (errCode == noErr)" 
	
	if (errCode == noErr)
		{
				// Type 2 record continued.													
				// Write list of pool (class) names.										
				
		tempHeaderRecordPtr = headerRecordPtr;
		count = 8 * numberGAIAClasses;
		memset (tempHeaderRecordPtr, ' ', count);
		
				// Load class to pool information (POLPTR).  							
				// The first value in the pair is the number of classes in the 	
				// pool and the second value is the index into the POLSTK vector	
				// which lists that classes in the pool. - not used					
		
		tempHeaderRecordPtr += count;
		temp2ByteHeaderRecordPtr = (SInt16*)tempHeaderRecordPtr;
		for (index=1; index<=numberGAIAClasses; index++)
			{
			*temp2ByteHeaderRecordPtr = 0x0100;
			temp2ByteHeaderRecordPtr++;
			
			tempShortInt = ((index & 0xff00) >> 8) | ((index & 0x00ff) << 8);
			BlockMoveData ((char*)&tempShortInt, (char*)temp2ByteHeaderRecordPtr, 2);
			temp2ByteHeaderRecordPtr++;
			
			}	// end "for (index=1; index<=...)" 
		
				// Load the list of classes is each pool. - not used					
			
		for (index=1; index<=numberGAIAClasses; index++)
			{
			tempShortInt = ((index & 0xff00) >> 8) | ((index & 0x00ff) << 8);
			BlockMoveData ((char*)&tempShortInt, (char*)temp2ByteHeaderRecordPtr, 2);
			temp2ByteHeaderRecordPtr++;
			
			}	// end "for (index=1; index<=...)" 
			
				// Load the pool (class) prior probabilities - not used.				
				
		count = 4 * numberGAIAClasses;
		memset ((char*)temp2ByteHeaderRecordPtr, 0, count);
	
				// Write the record 2 pool information to the header.					
				
		count = 18 * numberGAIAClasses;
		errCode = MWriteData (fileStreamPtr, &count, headerRecordPtr, kErrorMessages);	
		
		}	// end "if (errCode == noErr)" 
	
	if (errCode == noErr)
		{
				// Type 2 record continued.													
				// Load the date of the classification to the header record.		
			
		GetTime (&gDateTimeRecord);
		sprintf (headerRecordPtr, 
						"%2d-%2d-%4d  %2d:%02d   ", 
						gDateTimeRecord.month,
						gDateTimeRecord.day,
						gDateTimeRecord.year,
						gDateTimeRecord.hour,
						gDateTimeRecord.minute);
	
				// Write the date and time to the header.									
				
		count = 20;		
		errCode = MWriteData (fileStreamPtr, &count, headerRecordPtr, kErrorMessages);
		
		}	// end "if (errCode == noErr)" 
	
	if (errCode == noErr)
		{
				// Load the statistics deck Type 3 header.								
				
		memset (headerRecordPtr, 0, 10);
		headerRecordPtr[0] = 0x03;
		headerRecordPtr[2] = 0x03;
		
				// Load the start of the statistics deck.									

		GetIndString (gTextString, 162, 1);
		BlockMoveData ((char*)&gTextString[1], &headerRecordPtr[10], gTextString[0]);
		
				// Write the LARSYS Statistics Line Identifier.							
				
		count = 90;
		errCode = MWriteData (fileStreamPtr, &count, headerRecordPtr, kErrorMessages);	
		
		}	// end "if (errCode == noErr)" 
	
	if (errCode == noErr)
		{
				// Type 3 record continued.													
				// Load the training field information - not used.						
				
		memset ((char*)&headerRecordPtr[10], ' ', 80);
			
		classIndex = 0;
		for (classNumber=1; classNumber<=numberGAIAClasses; classNumber++)
			{
					// Load class name.															
					
			sprintf (&headerRecordPtr[10], "CLASS ");
			
			tempShortInt = LoadClassName (classNumber, 
										numberClasses, 
										classPtr, 
										&classIndex, 
										&headerRecordPtr[16],
										classNameCode);
										
			headerRecordPtr[16+tempShortInt] = ' ';
					
			count = 90;	
			errCode = MWriteData (fileStreamPtr, &count, headerRecordPtr, kErrorMessages);
			
			if (errCode == noErr)
				{
				memset ((char*)&headerRecordPtr[10], ' ', 38);
			
						// Field description.													
					
				errCode = MWriteData (fileStreamPtr, 
												&count, 
												headerRecordPtr, 
												kErrorMessages);
				
				}	// end "if (errCode == noErr)" 
			
			if (errCode != noErr)
				break;
				
			}	// end "for (classNumber=0; classNumber<...)" 
		
		}	// end "if (errCode == noErr)" 
	
	if (errCode == noErr)
		{
				// Type 3 record continued.													
				// Load the statistics summary information.								
			
		sprintf (&headerRecordPtr[10], 
					" %4d CLASS %4d FIELD %4d CHANNELS", 
					numberGAIAClasses,
					numberGAIAClasses,
					numberChannels);
		headerRecordPtr[46] = ' ';
					
		count = 90;		
		errCode = MWriteData (fileStreamPtr, &count, headerRecordPtr, kErrorMessages);	
		
		}	// end "if (errCode == noErr)" 
	
	if (errCode == noErr)
		{
				// Type 3 record continued.													
				// Load the channel wavelength and calibration information.			
			
		memset (&headerRecordPtr[10], ' ', 36);
		
		channelDescriptionPtr = NULL;
		if (gImageFileInfoPtr && gImageFileInfoPtr->channelDescriptionH && 
																!gImageFileInfoPtr->thematicType)
			{
			MHLock (gImageFileInfoPtr->channelDescriptionH);
			channelDescriptionPtr =
						(ChannelDescriptionPtr)*gImageFileInfoPtr->channelDescriptionH;
			
			}	// end "if (fileInfoPtr != NULL && fileInfoPtr->...)" 
		
		channel = 0;
		for (index=0; index<numberChannels; index++)
			{
			sprintf (&headerRecordPtr[10], 
						"CHAN%3ld WAVELENGTH 0.00- 0.00",
						index+1);
			headerRecordPtr[39] = ' ';
			
			if (channelListPtr)
				channel = channelListPtr[index];
						
			if (channelDescriptionPtr)
				BlockMoveData (&channelDescriptionPtr[channel], &headerRecordPtr[28], 11);
				
			count = 90;	
			errCode = MWriteData (fileStreamPtr, &count, headerRecordPtr, kErrorMessages);
			
			if (errCode != noErr)
				break;
				
			channel++;
				
			}	// end "for (index=0; index<...)" 
			
		CheckAndUnlockHandle (gImageFileInfoPtr->channelDescriptionH);
		
		}	// end "if (errCode == noErr)" 
	
	if (errCode == noErr)
		{
				// Type 3 record continued.													
				// Load the number of training pixels per class information.		
				// 7 classes per line will be loaded.										
				
		memset (&headerRecordPtr[10], ' ', 30);
				
		sprintf (&headerRecordPtr[10], "NO. PTS.");
		headerRecordPtr[18] = ' ';
				
		tempShortInt = (numberGAIAClasses + 6)/7;
		firstIndex = 1;
		classIndex = 0;
			
		for (index=1; index<=tempShortInt; index++)
			{
			tempHeaderRecordPtr = &headerRecordPtr[18];
			lastIndex = MIN (index*7, numberGAIAClasses);
			
			for (index2=firstIndex; index2<=lastIndex; index2++)
				{
				if (classPtr != NULL)
					classNumber = classPtr[classIndex];
					
				else	// classPtr == NULL
					classNumber = index2;
				
				if (classNumber == index2 && 
											gProcessorCode != kMultispecToThematicProcessor)
					{
					classStorage = gProjectInfoPtr->storageClass[classNumber-1];
					tempS64Int =
						gProjectInfoPtr->classNamesPtr[classStorage].numberStatisticsPixels;
				
					if (classIndex < numberClasses-1)
						classIndex++;
						
					}	// end "if (classPtr[classIndex] == index2 && ...)"
					
				else	// classNumber != index2 || ...
					tempLongInt = 0;
								
				sprintf (tempHeaderRecordPtr, "%9ld", tempS64Int);
				tempHeaderRecordPtr += 9;
				
				}	// end "for (index2=Index; index2<=Index; index2++)" 
				
			*tempHeaderRecordPtr = ' ';
				
			count = 90;	
			errCode = MWriteData (fileStreamPtr, &count, headerRecordPtr, kErrorMessages);
			
			if (errCode != noErr)
				break;
				
			firstIndex = lastIndex + 1;
			
			memset (&headerRecordPtr[18], ' ', 72);
				
			}	// end "for (index=1; index<=...)" 
		
		}	// end "if (errCode == noErr)" 
	
	if (errCode == noErr)
		{
				// Type 3 record continued.													
				// Load the class mean vectors.												
			
		memset (&headerRecordPtr[10], ' ', 80);
				
		sprintf (&headerRecordPtr[10], "MN");
		headerRecordPtr[12] = ' ';
				
		tempShortInt = (numberChannels + 4)/5;
		
		for (classNumber=0; classNumber<numberGAIAClasses; classNumber++)
			{
					// Get the mean vector for the class.									

			continueFlag = GetGAIAClassMeans (classChannelStatsPtr, 
															numberChannels, 
															channelListPtr, 
															classNumber);
									
			meanValue = 0.;
									
			firstIndex = 1;
				
			for (index=1; index<=tempShortInt; index++)
				{
				tempHeaderRecordPtr = &headerRecordPtr[12];
				lastIndex = MIN (index*5, numberChannels);
				
				for (index2=firstIndex; index2<=lastIndex; index2++)
					{
					if (continueFlag)
						meanValue = 10 * classChannelStatsPtr[index2-1].mean;
						
					sprintf (tempHeaderRecordPtr, " %013.6E", meanValue);
								
					tempHeaderRecordPtr[3] = tempHeaderRecordPtr[2];
					tempHeaderRecordPtr[2] = '.';
					tempHeaderRecordPtr += 14;
					
					}	// end "for (index2=firstIndex; index2<=..." 
					
				*tempHeaderRecordPtr = ' ';
			
				count = 90;		
				errCode = MWriteData (fileStreamPtr, 
												&count, 
												headerRecordPtr, 
												kErrorMessages);
				
				if (errCode != noErr)
					break;
				
				firstIndex = lastIndex + 1;
			
				memset (&headerRecordPtr[12], ' ', 78);
					
				}	// end "for (index=1; index<=...)" 
					
			}	// end "for (classNumber=0; classNumber<...)" 
		
		}	// end "if (errCode == noErr)" 
	
	if (errCode == noErr)
		{
				// Type 3 record continued.													
				// Load the class covariance matrices.										
			
		memset (&headerRecordPtr[10], ' ', 80);
				
		sprintf (&headerRecordPtr[10], "CV");
		headerRecordPtr[12] = ' ';
				
		tempShortInt = (numberChannels*(numberChannels+1)/2 + 4)/5;
		
		for (index=1; index<=numberGAIAClasses; index++)
			{
			for (index2=1; index2<=tempShortInt; index2++)
				{
				count = 90;		
				errCode = MWriteData (fileStreamPtr, 
												&count, 
												headerRecordPtr, 
												kErrorMessages);
				
				if (errCode != noErr)
					break;
					
				}	// end "for (index2=1; index2<=...)" 
					
			}	// end "for (index=1; index<=...)" 
		
		}	// end "if (errCode == noErr)" 
	
	if (errCode == noErr)
		{
				// Type 3 record continued.													
				// Load the last card of the statistics deck.							
				
		GetIndString (gTextString, 162, 2);
		BlockMoveData ((char*)&gTextString[1], &headerRecordPtr[10], gTextString[0]);
		
				// Write the last LARSYS Statistics Line Identifier.					
				
		count = 90;		
		errCode = MWriteData (fileStreamPtr, &count, headerRecordPtr, kErrorMessages);	
		
		}	// end "if (errCode == noErr)" 
	
	if (errCode == noErr)
		{
				// Load the pooled statistics Type 4 header.								
				
		headerRecordPtr[2] = 0x04;
		
				// Write the Type 4 record header.											
				
		count = 10;		
		errCode = MWriteData (fileStreamPtr, &count, headerRecordPtr, kErrorMessages);	
		
		}	// end "if (errCode == noErr)" 
	
	if (errCode == noErr)
		{
				// Type 4 record continued.													
				// Load the pooled covariance statistics.									
				
		count = numberChannels*(numberChannels+1)/2;
		count *= 4;
		tempHeaderRecordPtr = &headerRecordPtr[10];
		memset (tempHeaderRecordPtr, 0, count);
		
		for (index=1; index<=numberGAIAClasses; index++)
			{	
			errCode = MWriteData (fileStreamPtr, 
											&count, 
											tempHeaderRecordPtr, 
											kErrorMessages);
			
			if (errCode != noErr)
				break;
					
			}	// end "for (index=1; index<=...)" 
		
		}	// end "if (errCode == noErr)" 
	
	if (errCode == noErr)
		{
				// Type 4 record continued.													
				// Load the pooled mean statistics.											
				
		count = numberChannels * 4;
		
		for (index=1; index<=numberGAIAClasses; index++)
			{	
			errCode = MWriteData (fileStreamPtr, 
											&count, 
											tempHeaderRecordPtr, 
											kErrorMessages);
			
			if (errCode != noErr)
				break;
					
			}	// end "for (index=1; index<=...)" 
		
		}	// end "if (errCode == noErr)" 
	
	if (errCode == noErr)
		{
				// Add bytes for 'segmented type 4 records'.  I do not 				
				// understand the reason for this.  GAIA requires this.  I 			
				// assume that it has something to do with old tape file records	
				// or older disk file records which could not be longer than		
				// 1872 bytes.  Larry L. Biehl - 9-30-92									
				
		count = (numberChannels*(numberChannels+1)/2) * 4 + numberChannels * 4;
		count *= numberGAIAClasses;
		count = (count/1872) * 2;
		
		if (count > 0)
			errCode = MWriteData (fileStreamPtr, 
											&count, 
											tempHeaderRecordPtr, 
											kErrorMessages);
		
		}	// end "if (errCode == noErr)" 
			
	
	if (errCode == noErr)
		{
				// Load the Type 5 header.														
				
		headerRecordPtr[2] = 0x05;
		headerRecordPtr[6] = 0x01;
		
				// Load the number of columns per line.									
				
		tempLongInt = GetLongIntValue ((char*)&fileInfoPtr->numberColumns);
		BlockMoveData ((char*)&tempLongInt, &headerRecordPtr[10], 4);
		
				// Load the number of lines.													
				
		tempLongInt = GetLongIntValue ((char*)&fileInfoPtr->numberLines);
		BlockMoveData ((char*)&tempLongInt, &headerRecordPtr[14], 4);
		
				// Load the INFO array.															
				// INFO(1) = LARSYS Run Number - not used.								
				
		memset (&headerRecordPtr[18], 0, 4);
		
				// INFO(2-3) = Field Name or identifier - not used.					
				
		memset (&headerRecordPtr[22], ' ', 8);
		
				// INFO(4) = First line of area classified.								
				
		tempLongInt = fileInfoPtr->startLine;
		tempLongInt = GetLongIntValue ((char*)&tempLongInt);
		BlockMoveData ((char*)&tempLongInt, &headerRecordPtr[30], 4);
		
				// INFO(5) = Last line of area classified.								
				
		tempLongInt = gAreaDescription.lineEnd + 
									(fileInfoPtr->startLine - gAreaDescription.lineStart);
		tempLongInt = GetLongIntValue ((char*)&tempLongInt);
		BlockMoveData ((char*)&tempLongInt, &headerRecordPtr[34], 4);
		
				// INFO(6) = Line interval used for classification.					
				
		tempLongInt = gAreaDescription.lineInterval;
		tempLongInt = GetLongIntValue ((char*)&tempLongInt);
		BlockMoveData ((char*)&tempLongInt, &headerRecordPtr[38], 4);
		
				// INFO(7) = First column of area classified.							
				
		tempLongInt = fileInfoPtr->startColumn;
		tempLongInt = GetLongIntValue ((char*)&tempLongInt);
		BlockMoveData ((char*)&tempLongInt, &headerRecordPtr[42], 4);
		
				// INFO(8) = Last column of area classified.								
				
		tempLongInt = gAreaDescription.columnEnd + 
							(fileInfoPtr->startColumn - gAreaDescription.columnStart);
		tempLongInt = GetLongIntValue ((char*)&tempLongInt);
		BlockMoveData ((char*)&tempLongInt, &headerRecordPtr[46], 4);
		
				// INFO(9) = Column interval used for classification.					
				
		tempLongInt = gAreaDescription.columnInterval;
		tempLongInt = GetLongIntValue ((char*)&tempLongInt);
		BlockMoveData ((char*)&tempLongInt, &headerRecordPtr[50], 4);
		
				// INFO(10-11) = Class Name - not used.									
				// INFO(12-17) = Other information - not used.							
				
		memset (&headerRecordPtr[54], ' ', 32);
				
		count = 86;		
		errCode = MWriteData (fileStreamPtr, &count, headerRecordPtr, kErrorMessages);
		
		}	// end "if (errCode == noErr)" 
	
	if (errCode == noErr)
		{
				// Type 5 record continued.													
				// Load the LARSYS ID record information - 800 bytes total.			
				// ID1-LARS Tape Number - not used.											
				// ID2-File number on this tape - not used.								
				// ID3-LARS run number - not used.											
				// ID4-Continuation Code - not used.										
				// ID5-Number of data channels on A? Data Storage Tape - not used.
				// ID6-Number of data samples on ADST - not used.						
				
		memset (headerRecordPtr, 0, 24);
		
				// ID7-10 - Flight line identification - not used.						
				
		memset (&headerRecordPtr[24], ' ', 16);
		
				// ID11 - Month data was taken - not used.								
				// ID12 - Day data was taken - not used.									
				// ID13 - Year data was taken - not used.									
				// ID14 - Time data was taken - not used.									
				// ID15 - Altitude of instrument above the ground - not used.		
				// ID16 - Ground heading of instrument - not used.						
				
		memset (&headerRecordPtr[40], 0, 24);
		
				// ID17-19 - Date of reformatting - not used.							
				
		memset (&headerRecordPtr[64], ' ', 12);
		
				// ID20 - Last line number in file.											
				
		tempLongInt = GetLongIntValue ((char*)&fileInfoPtr->numberLines);
		BlockMoveData ((char*)&tempLongInt, &headerRecordPtr[76], 4);
		
				// ID21-22 - Not defined - not used.										
				
		memset (&headerRecordPtr[80], 0, 8);
		
				// ID23 - Resolution.															
				
		tempLongInt = 16384;
		tempLongInt = GetLongIntValue ((char*)&tempLongInt);
		BlockMoveData ((char*)&tempLongInt, &headerRecordPtr[88], 4);
		
				// ID24-32 - Not defined - not used.										
				
		memset (&headerRecordPtr[92], 0, 36);
		
				// ID33 - Image origin X.														
				
		tempLongInt = 1;
		tempLongInt = GetLongIntValue ((char*)&tempLongInt);
		BlockMoveData ((char*)&tempLongInt, &headerRecordPtr[128], 4);
		
				// ID34 - Image origin Y.														
				
		BlockMoveData ((char*)&tempLongInt, &headerRecordPtr[132], 4);
		
				// ID35-50 - Not defined - not used.										
				
		memset (&headerRecordPtr[136], 0, 64);
		
				// ID51 - Lower limit of spectral band 1 on tape - not used.		
				// ID52 - Upper limit of spectral band 1 on tape - not used.		
				// ID53 - Value of C0 for band 1 on tape - not used.					
				// ID54 - Value of C1 for band 1 on tape - not used.					
				// ID55 - Value of C2 for band 1 on tape - not used.					
				// ID56-200 - Repeat ID51-55 for ID5 channels - not used.			
				// ID56-200 - 0.0 for nonexisting data channels - not used.			
				
		memset (&headerRecordPtr[200], 0, 600);
				
		count = 800;
		errCode = MWriteData (fileStreamPtr, &count, headerRecordPtr, kErrorMessages);	
		
		}	// end "if (errCode == noErr)" 
	
	if (errCode == noErr)
		{
				// Type 5 record continued.													
				// Load the calibration set vector, CSET (3,30), for the channels
				// used in the classification - 360 bytes total.						
				// This info is not used.														
				
		memset (headerRecordPtr, 0, 360);
				
		count = 360;		
		errCode = MWriteData (fileStreamPtr, &count, headerRecordPtr, kErrorMessages);
		
				// This is the end of the LARSYS type 5 record.							
				// LARSYS type 6 records are the lines of classified data.			
		
		}	// end "if (errCode == noErr)" 									
				
	gSwapBytesFlag = FALSE; 
		
	if (releaseHeaderRecordPtrFlag)
		CheckAndDisposePtr (headerRecordPtr);
		
	return (errCode == noErr);
#endif	// defined multispec_mac || defined multispec_mac_swift

#if defined multispec_win
	return (FALSE);
#endif	// defined multispec_win

}	// end "WriteGAIAHeader" 
