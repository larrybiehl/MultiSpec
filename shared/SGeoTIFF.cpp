//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2018)
//						(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SGeoTIFF.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			06/28/2018
//
//	Language:				C
//
//	System:					Linux, Macintosh and Windows Operating Systems
//
//	Brief description:	This file contains routines which are used to access
//								various disk files.
//
//	Functions in file:	SInt16 				CheckRowsPerStrip
//								SInt16 				GetGeoKeyParameters
//								SInt16	 			GetTiffEntry
//								SInt16 				GetTIFFASCIIParameters
//								SInt16 				GetTIFFDoubleParameters
//								SInt16	 			ReadTIFFHeader
//								SInt16 				GetGeoDoubleParametersFromGeoTIFF
//								SInt16 				SetGeoProjectionFromGeoTIFF
//								void 					SetGeoTiePointsForRasterPixelType
//								SInt16 				SetGeoTiePointsFromGeoTIFF
//								SInt16 				SetPixelScaleParametersFromGeoTIFF
//
//	Diagram of MultiSpec routine calls for the routines in the file.
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
	
#if defined multispec_mac
#endif	// defined multispec_mac    

#if defined multispec_lin
	#include "SMultiSpec.h"
#endif

#if defined multispec_win
#endif	// defined multispec_win

#include 	"errno.h"

//#include "SExtGlob.h"	

	// MultiSpec tiff code will be used for large file sizes. GDAL is very slow when
	// reading large files particular when the files are not striped.
#define use_multispec_tiffcode	1	
							
							

			// Prototypes for routines in this file that are only called by		
			// other routines in this file.		

SInt16 				CheckRowsPerStrip (
							FileInfoPtr							fileInfoPtr,
							CMFileStream* 						fileStreamPtr,
							UInt32								numberStripOffsets,
							UInt32								stripOffsetType,
							UInt32								stripOffsetVector,
							UInt32								rowsPerStrip,
							UInt32								numberStripByteCounts,
							UInt32								stripByteCountType,
							UInt32								stripByteCountVector);

SInt16 				GetBlockSizesVector (
							CMFileStream*						fileStreamPtr,
							UInt32**								blockSizesPtrPtr,
							UInt32								numberStripByteCounts,
							UInt32								stripByteCountType,
							UInt32								stripByteCountVector);

SInt16 				GetGeoKeyParameters (
							FileInfoPtr							fileInfoPtr,
							TIFF_IFD_Ptr						imageFileDirectoryPtr,
							TIFF_IFD_Ptr						geoDoubleParamsPtr,
							TIFF_IFD_Ptr						geoAsciiParamsPtr,
							UInt16*								rasterTypeGeoKeyPtr);

SInt16				GetGTModelTypeGeoKeyFromGeoDirectory (
							FileInfoPtr							fileInfoPtr,
							TIFF_IFD_Ptr						imageFileDirectoryPtr);
							
SInt16	 			GetTiffEntry (
							CMFileStream* 						fileStreamPtr,
							char*									inputBufferPtr,
							TIFF_IFD_Ptr						imageFileDirectoryPtr);

SInt16 				GetTIFFDoubleParameters (
							CMFileStream*						fileStreamPtr,
							TIFF_IFD_Ptr						imageFileDirectoryPtr,
							double*								doubleParameterPtr,
							SInt32								numberParameters);

SInt16 				GetGeoDoubleParametersFromGeoTIFF (
							FileInfoPtr							fileInfoPtr,
							TIFF_IFD_Ptr						imageFileDirectoryPtr,
							double*								geoParametersPtr);

SInt16				GetGeoASCIIStringFromGeoTIFF (
							FileInfoPtr							fileInfoPtr,
							char*									geoASCIIStringPtr,
							UInt32								stringLengthLimit);

SInt16 				GetGeoASCIIStringFromGeoTIFF (
							FileInfoPtr							fileInfoPtr,
							TIFF_IFD_Ptr						geoASCIIStringTagPtr,
							char*									geoASCIIStringPtr,
							UInt32								stringLengthLimit);

Boolean				ListGeoTiffTextDescriptionParameters (
							FileInfoPtr							fileInfoPtr,
							TIFF_IFD_Ptr						imageFileDirectoryPtr,
							TIFF_IFD_Ptr						geoAsciiStringPtr,
							Boolean*								addBlankLineFlagPtr);

SInt16 				LoadHierarchalFileStructure (
							FileInfoPtr 						fileInfoPtr,
							UInt32								numberBlocks,
							UInt32*								channelStartsPtr,
							UInt32*								blockByteCountsPtr,
							UInt32								numberLinesPerStrip,
							UInt32								bytesPerStrip,
							UInt32								lastBytesPerStrip,
							UInt32								bytesPerStripForAllChannels);

SInt16 				SetGeoProjectionFromGeoTIFF (
							FileInfoPtr							fileInfoPtr,
							TIFF_IFD_Ptr						imageFileDirectoryPtr);

void 					SetGeoTiePointsForRasterPixelType (
							FileInfoPtr							fileInfoPtr,
							UInt16								rasterTypeGeoKey);

SInt16 				SetGeoTiePointsFromGeoTIFF (
							FileInfoPtr							fileInfoPtr,
							TIFF_IFD_Ptr						imageFileDirectoryPtr,
							SInt32*								numberControlPointsPtr);

SInt16 				SetModelTransformationParametersFromGeoTIFF (
							FileInfoPtr							fileInfoPtr,
							TIFF_IFD_Ptr						imageFileDirectoryPtr);

SInt16 				SetPixelScaleParametersFromGeoTIFF (
							FileInfoPtr							fileInfoPtr,
							TIFF_IFD_Ptr						imageFileDirectoryPtr);
							

#if use_multispec_tiffcode
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 CheckRowsPerStrip
//
//	Software purpose:	This routine verifies that the strips are in contiguous order. 
//							MultiSpec has not been set up to read non-contiguous strips.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/15/2002
//	Revised By:			Larry L. Biehl			Date: 05/21/2012

SInt16 CheckRowsPerStrip (
				FileInfoPtr							fileInfoPtr,
				CMFileStream* 						fileStreamPtr,
				UInt32								numberStripOffsets,
				UInt32								stripOffsetType,
				UInt32								stripOffsetVector,
				UInt32								rowsPerStrip,
				UInt32								numberStripByteCounts,
				UInt32								stripByteCountType,
				UInt32								stripByteCountVector)

{
	SInt64								savedPosOff;
	
	UInt32								*blockByteCountsPtr,
											*blockStartsPtr;
	
	SInt32								bytesToRead,
											numberBlankLines,
											numberPostLineBytes;
											
	UInt32								bytesPerStrip,
											bytesPerStripForAllChannels,
											channelIndex,
											count,
											index,
											lastBytesPerStrip,
											lastStripOffset,
											stripsPerChannel,
											tempOffset;
	
	SInt16								errCode,
											returnCode;
											
	Boolean								nonContiguousStripsFlag;
	
	
	numberPostLineBytes = 0;
	returnCode = 0;
	nonContiguousStripsFlag = FALSE;
	channelIndex = 0;
	errCode = noErr;
	
	blockByteCountsPtr = NULL;
	blockStartsPtr = NULL;
	
	if (rowsPerStrip == UInt32_MAX)
		rowsPerStrip = fileInfoPtr->numberLines;
	
			// Make certain that the strips are contiguous in 	
			// in memory.  If they are not then call the routine to load the
			// structure to handle this situation.	
			
	switch (fileInfoPtr->bandInterleave)
		{								
		case kBIL:
		case kBIS:	
			bytesPerStrip = rowsPerStrip * 
									fileInfoPtr->numberColumns * 
										fileInfoPtr->numberChannels *
												fileInfoPtr->numberBytes;
			bytesPerStripForAllChannels = bytesPerStrip;
			break;
			
		case kBSQ:
			bytesPerStrip = rowsPerStrip * 
									fileInfoPtr->numberColumns * 
												fileInfoPtr->numberBytes;
			stripsPerChannel = numberStripOffsets/fileInfoPtr->numberChannels;
			
			lastBytesPerStrip = bytesPerStrip;
			lastBytesPerStrip -= (stripsPerChannel * rowsPerStrip - 
															fileInfoPtr->numberLines) *
									fileInfoPtr->numberColumns * fileInfoPtr->numberBytes;
			
			bytesPerStripForAllChannels = bytesPerStrip * 
												fileInfoPtr->numberChannels;
			break;
			
		}	// end "switch (fileInfoPtr->bandInterleave)"

			// Get memory to store the block (strip) start values in.  If no
			// memory is available then we cannot continue with this.
			
	blockStartsPtr = (UInt32*)MNewPointer (numberStripOffsets * sizeof (UInt32));
	if (blockStartsPtr == NULL)
		errCode = -1;
			
	if (errCode == noErr)
		errCode = MGetMarker (fileStreamPtr,
										&savedPosOff,
										kErrorMessages);
	
	if (errCode == noErr)
		errCode = MSetMarker (fileStreamPtr, 
										fsFromStart, 
										stripOffsetVector,
										kErrorMessages);
								
	bytesToRead = 2;
	if (stripOffsetType == kTIFFLong)	
		bytesToRead = 4;
	
	stripOffsetVector = 0;
	for (index=1; index<=numberStripOffsets; index++)
		{
		count = (UInt32)bytesToRead;
		tempOffset = 0;
		
		if (errCode == noErr)
			errCode = MReadData (fileStreamPtr, 
											&count, 
											&tempOffset,
											kErrorMessages);
			
		if (errCode != noErr)
			break;
			
		tempOffset = (UInt32)GetLongIntValue ((char*)&tempOffset);
		blockStartsPtr[index-1] = tempOffset;
				
		if (index == 1)
			{
			fileInfoPtr->numberHeaderBytes = tempOffset;
			stripOffsetVector = tempOffset;
			
			}	// end "if (index == 1)" 
			
		else	// index > 2 
			{		
			stripOffsetVector += bytesPerStrip + numberPostLineBytes;
				
			if (stripOffsetVector != tempOffset)
				{
						// The strips are not contiguous. If the
						// number of strips are equal to the number of
						// lines then check if the offset is the same
						// for all of the lines. If so then set the
						// number of post line bytes to the difference.
						
				if (numberStripOffsets == 
									fileInfoPtr->numberLines &&
											tempOffset > stripOffsetVector)
					{
					if (index == 2)
						{
						numberPostLineBytes = 
											(SInt32)(tempOffset - stripOffsetVector);
											
						stripOffsetVector += numberPostLineBytes;
											
						}	// end "if (index == 2)"
											
					else if (numberPostLineBytes != 
								(SInt32)(tempOffset - stripOffsetVector))
						nonContiguousStripsFlag = TRUE;
							
					}	// end "if (... == ...->numberLines)"
					
						// Check if the strip offsets could be one 
						// channel and line at a time. This would be
						// the case for band sequential data.
						// 'numberPostLineBytes' will be 0 at this
						// time.
					
				else if (numberStripOffsets == 
								fileInfoPtr->numberLines *
												fileInfoPtr->numberChannels)
					{
					if (index == 2)
						{
						stripOffsetVector -= bytesPerStrip;							
				
						bytesPerStrip = rowsPerStrip * 
												fileInfoPtr->numberColumns * 
														fileInfoPtr->numberBytes;
					
						stripOffsetVector += bytesPerStrip;
										
						}	// end "else if (numberStripOffsets == 
						
					if (stripOffsetVector != tempOffset)
						nonContiguousStripsFlag = TRUE;
					
					}	// end "if (stripOffsetVector != tempOffset)"
					
				else	// numberStripOffsets != ...
					nonContiguousStripsFlag = TRUE;
					
				}	// end "if (stripOffsetVector != ..." 
			
			}	// end "else index > 1"
			
		lastStripOffset = tempOffset; 
			
		}	// end "for (index=1, ..." 
										
	if (errCode != noErr)
		returnCode = -8;
					
	if (returnCode == 0)
		{					
		if (nonContiguousStripsFlag)
			{
			if (numberStripByteCounts != numberStripOffsets)
				returnCode = -8;
			
			if (returnCode == 0)
				{
				if (fileInfoPtr->numberChannels == 1 && 
												fileInfoPtr->bandInterleave == kBIS)
							// Change to BSQ so that blocks will be used
							// when computing the position offset for the line.
							// This will also allow linking these type of files.
							// More thought will need to be put into the 
							// "GetFilePositionOffset" routine to handle blocked
							// data with more than one channel and kBIS format.
					fileInfoPtr->bandInterleave = kBSQ;
					
				returnCode = GetBlockSizesVector (fileStreamPtr,
																&blockByteCountsPtr,
																numberStripByteCounts,
																stripByteCountType,
																stripByteCountVector);
				}	// end "if (fileInfoPtr->numberChannels == 1 && ..."
			
			if (returnCode == 0)
				returnCode = LoadHierarchalFileStructure (fileInfoPtr,
																		numberStripOffsets,
																		blockStartsPtr,
																		blockByteCountsPtr,
																		rowsPerStrip,
																		bytesPerStrip,
																		lastBytesPerStrip,
																		bytesPerStripForAllChannels);
			
			}	// end "if (nonContiguousStripsFlag)"
		
		else	// !nonContiguousStripsFlag		
			{				
			if (fileInfoPtr->bandInterleave == kBSQ)
				{
				numberBlankLines = (SInt32)(rowsPerStrip * 
							(numberStripOffsets/fileInfoPtr->numberChannels) - 
										fileInfoPtr->numberLines);
										
				if (numberBlankLines > 0 && fileInfoPtr->numberChannels > 1)
					fileInfoPtr->numberPostChannelBytes = numberBlankLines * 
								fileInfoPtr->numberColumns * fileInfoPtr->numberBytes;
										
				}	// end "if (fileInfoPtr->bandInterleave == kBSQ)"
				
			else	// fileInfoPtr->bandInterleave != kBSQ
				fileInfoPtr->numberPostLineBytes = (UInt32)numberPostLineBytes;
				
			}	// end "else !nonContiguousStripsFlag"	
		
		}	// end "if (returnCode == 0)"
			
	if (errCode == noErr)
		errCode = MSetMarker (fileStreamPtr, 
										fsFromStart, 
										savedPosOff,
										kErrorMessages);
		
	CheckAndDisposePtr (blockStartsPtr);
	CheckAndDisposePtr (blockByteCountsPtr);
		
	return (returnCode);
	
}	// end "CheckRowsPerStrip" 
  


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetBlockSizesVector
//
//	Software purpose:	The purpose of this routine is to read the block sizes vector.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/07/2007
//	Revised By:			Larry L. Biehl			Date: 06/01/2011

SInt16 GetBlockSizesVector (
				CMFileStream*						fileStreamPtr,
				UInt32**								blockSizesPtrPtr,
				UInt32								numberStripByteCounts,
				UInt32								stripByteCountType,
				UInt32								stripByteCountVector)

{	
	UInt32*								localBlockSizesPtr;
	
	SInt32								stripByteCount;
	
	UInt32								bytesToRead,
											count,
											index;
	
	SInt16								errCode,
											returnCode;
	
	
	errCode = noErr;
	returnCode = 0;

			// Get memory to store the block (strip) size values in.  If no
			// memory is available then we cannot continue with this.
			
	*blockSizesPtrPtr = (UInt32*)MNewPointer ((SInt64)numberStripByteCounts * sizeof (UInt32));
	if (*blockSizesPtrPtr == NULL)
		errCode = -1;
	
	if (errCode == noErr)
		errCode = MSetMarker (fileStreamPtr, 
										fsFromStart, 
										stripByteCountVector,
										kErrorMessages);
								
	bytesToRead = 2;
	if (stripByteCountType == kTIFFLong)	
		bytesToRead = 4;
	
	localBlockSizesPtr = *blockSizesPtrPtr;
	
	for (index=0; index<numberStripByteCounts; index++)
		{
		count = (UInt32)bytesToRead;
		stripByteCount = 0;
		
		if (errCode == noErr)
			errCode = MReadData (fileStreamPtr, 
											&count, 
											&stripByteCount,
											kErrorMessages);
			
		if (errCode != noErr)
			break;
			
		localBlockSizesPtr[index] = (UInt32)GetLongIntValue ((char*)&stripByteCount);
		
		}	// end "for (index=0; index<numberStripByteCounts; index++)"
		
	if (errCode != noErr)
		returnCode = -8;
		
	return (returnCode);
	
}	// end "GetBlockSizesVector" 	
#endif	// use_multispec_tiffcode
  


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetGeoASCIIStringFromGeoTIFF
//
//	Software purpose:	The purpose of this routine is to get the text information in
//							the GEOTIFF tag.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	error code
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/18/2012
//	Revised By:			Larry L. Biehl			Date: 02/18/2012

SInt16 GetGeoASCIIStringFromGeoTIFF (
				FileInfoPtr							fileInfoPtr,
				char*									geoASCIIStringPtr,
				UInt32								stringLengthLimit)

{
	TIFFImageFileDirectory			tiffFileDirectory;
	
								
	if (GetSpecifiedTIFFKeyDirectory (fileInfoPtr,
													34737,
													&tiffFileDirectory))
		GetGeoASCIIStringFromGeoTIFF (fileInfoPtr,
													&tiffFileDirectory,
													geoASCIIStringPtr,
													stringLengthLimit);
		
	return (noErr);
	
}	// end "GetGeoASCIIStringFromGeoTIFF"
  


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetGeoASCIIStringFromGeoTIFF
//
//	Software purpose:	The purpose of this routine is to get the textinformation in 
//							the GEOTIFF tag.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 07/27/2007
//	Revised By:			Larry L. Biehl			Date: 07/27/2007

SInt16 GetGeoASCIIStringFromGeoTIFF (
				FileInfoPtr							fileInfoPtr,
				TIFF_IFD_Ptr						geoASCIIStringTagPtr,
				char*									geoASCIIStringPtr,
				UInt32								stringLengthLimit)

{
	CMFileStream*						fileStreamPtr;
	
	UInt32								count;
	
	SInt16								errCode = noErr;
	

	if (geoASCIIStringTagPtr->tag == 34737 && 
				geoASCIIStringTagPtr->count > 0 && 
						geoASCIIStringTagPtr->type == kTIFFASCII)
		{
				// Will read a string up to stringLengthLimit.
		
		fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
		
		count = MIN (geoASCIIStringTagPtr->count, stringLengthLimit);
										
		errCode = GetTIFFASCIIParameters (fileStreamPtr,
														geoASCIIStringTagPtr, 
														geoASCIIStringPtr, 
														(SInt32)count);
		
		if (errCode != noErr)
			geoASCIIStringTagPtr->tag = 0;
			
		}	// end "if (geoDoubleParamsTagPtr->tag == 34736 && ..."
		
	return (noErr);
	
}	// end "GetGeoASCIIStringFromGeoTIFF"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetGTModelTypeGeoKey
//
//	Software purpose:	The purpose of this routine is to get the value of the 
//							GTModelTypeGeoKey. Would be nice to get if from GDAL but have
//							not figured a way to get it yet.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:			ListDescriptionInformation in SOther.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/18/2012
//	Revised By:			Larry L. Biehl			Date: 02/18/2012

SInt16 GetGTModelTypeGeoKey (
				FileInfoPtr 						fileInfoPtr)

{
	TIFFImageFileDirectory			tiffFileDirectory;
	
	SInt16								gtModelTypeGeoKey = 0;
	
								
	if (GetSpecifiedTIFFKeyDirectory (fileInfoPtr, 34735, &tiffFileDirectory))
		gtModelTypeGeoKey = GetGTModelTypeGeoKeyFromGeoDirectory (
																			fileInfoPtr,
																			&tiffFileDirectory);
		
	return (gtModelTypeGeoKey);
	
}	// end "GetGTModelTypeGeoKey"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetGTModelTypeGeoKeyFromGeoDirectory
//
//	Software purpose:	The purpose of this routine is to get the value of the 
//							GTModelTypeGeoKey. Would be nice to get if from GDAL but have
//							not figured a way to get it yet.
//
//	Parameters in:		
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			GetGTModelTypeGeoKey in SGeoTIFF.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/18/2012
//	Revised By:			Larry L. Biehl			Date: 02/18/2012

SInt16 GetGTModelTypeGeoKeyFromGeoDirectory (
				FileInfoPtr							fileInfoPtr,
				TIFF_IFD_Ptr						imageFileDirectoryPtr)

{	
	SInt64								savedPosOff;
											
	CMFileStream*						fileStreamPtr;
	UInt16*								geoKeyRecordPtr;
				
	UInt32								count,
											i,
											index,
											recordIndex;
	
	UInt16								geoTagInformation[100];
	
	SInt16								errCode,
											gtModelTypeGeoKey = 0;
											
	Boolean								foundFlag = FALSE;
	
	
	if (imageFileDirectoryPtr->type == kTIFFShort && 
																imageFileDirectoryPtr->count > 0)
		{																																									
		fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
			
		errCode = MGetMarker (fileStreamPtr, 
											&savedPosOff,
											kErrorMessages);
			
		if (errCode == noErr)
			errCode = MSetMarker (fileStreamPtr, 
												fsFromStart, 
												imageFileDirectoryPtr->value,
												kErrorMessages);
												
		count = MIN (imageFileDirectoryPtr->count, 100);			
		count *= 2;
		if (errCode == noErr)
			errCode = MReadData (fileStreamPtr, 
												&count, 
												(char*)&geoTagInformation,
												kErrorMessages); 
												
		if (errCode == noErr)
			{
					// Convert to number of 2 byte integers.
						
			count /= 2;
			
					//  Convert to number of geo key records.
					
			count /= 4;
			
			index = 0;		
			for (recordIndex=0; recordIndex<count; recordIndex++)
				{
				geoKeyRecordPtr = &geoTagInformation[index];
				for (i=0; i<4; i++, index++)
							// Swap the bytes if needed.
					geoTagInformation[index] = 
									(UInt16)GetShortIntValue ((char*)&geoTagInformation[index]);
			
						// The first geo key record is the version and number of geo keys.
						// We will skip it.
					
				if (recordIndex == 0)
					geoKeyRecordPtr[0] = 0;
									
				switch (geoKeyRecordPtr[0])
					{
					case 1024:	// GTModelTypeGeoKey
						gtModelTypeGeoKey = geoKeyRecordPtr[3];
						foundFlag = TRUE;
						break;
																									
					}	// end "switch (geoKeyRecordPtr[0])"
											
				if (foundFlag)
					break;
									
				}	// end "for (recordIndex=0; recordIndex<count; recordIndex++)"
			
			}	// end "if (errCode == noErr)"
				
		if (errCode == noErr)
			errCode = MSetMarker (fileStreamPtr, 
											fsFromStart, 
											savedPosOff,
											kErrorMessages);
			
		}	// end "if (imageFileDirectoryPtr->type == kTIFFShort && ..."
		
	return (gtModelTypeGeoKey);
		
}	// end "GetGTModelTypeGeoKeyFromGeoDirectory"
  

#if use_multispec_tiffcode
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetGeoDoubleParametersFromGeoTIFF
//
//	Software purpose:	The purpose of this routine is to get the array of double 
//							parameters for the information in the GEOTIFF tag.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/05/2000
//	Revised By:			Larry L. Biehl			Date: 03/04/2004

SInt16 GetGeoDoubleParametersFromGeoTIFF (
				FileInfoPtr							fileInfoPtr,
				TIFF_IFD_Ptr						geoDoubleParamsTagPtr,
				double*								geoParametersPtr)

{
	CMFileStream*						fileStreamPtr;
	
	SInt16								errCode = noErr;
	

	if (geoDoubleParamsTagPtr->tag == 34736 && 
				geoDoubleParamsTagPtr->count <= 20 && 
						geoDoubleParamsTagPtr->type == kTIFFDouble)
		{
			// Will read data for up to 20 parameters.
		
		fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
										
		errCode = GetTIFFDoubleParameters (fileStreamPtr,
														geoDoubleParamsTagPtr, 
														geoParametersPtr, 
														(SInt32)geoDoubleParamsTagPtr->count);
		
		if (errCode != noErr)
			geoDoubleParamsTagPtr->tag = 0;
			
		}	// end "if (geoDoubleParamsTagPtr->tag == 34736 && ..."
		
	return (noErr);
	
}	// end "GetGeoDoubleParametersFromGeoTIFF"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetGeoKeyParameters
//
//	Software purpose:	The purpose of this routine is to read the Geo Key Parameters.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 09/10/2002
//	Revised By:			Larry L. Biehl			Date: 05/02/2012

SInt16 GetGeoKeyParameters (
				FileInfoPtr							fileInfoPtr,
				TIFF_IFD_Ptr						imageFileDirectoryPtr,
				TIFF_IFD_Ptr						geoDoubleParamsPtr,
				TIFF_IFD_Ptr						geoAsciiStringPtr,
				UInt16*								rasterTypeGeoKeyPtr)

{	
	double								geoDoubleParameters[20],
											inverseFlatteningParameter;
	
	SInt64								savedPosOff;
											
	CMFileStream*						fileStreamPtr;
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	UInt16*								geoKeyRecordPtr;
	
	Handle								mapProjectionHandle;
				
	UInt32								count,
											i,
											index,
											recordIndex;
	
	UInt16								geoTagInformation[100],
											projectedCoordTransGeoKey,
											projectedCSTypeGeoKey,
											projectionGeoKey;
	
	SInt16								errCode = -1;
	
	
	if (imageFileDirectoryPtr->type == kTIFFShort && 
																imageFileDirectoryPtr->count > 0)
		{		
		mapProjectionHandle = GetFileMapProjectionHandle (fileInfoPtr);
		
		if (mapProjectionHandle != NULL)
			{ 				
					// Get double parameter information in case it will be needed later.
					
			GetGeoDoubleParametersFromGeoTIFF (fileInfoPtr,
															geoDoubleParamsPtr,
															geoDoubleParameters);
																								
			mapProjectionInfoPtr = (MapProjectionInfoPtr)
												GetHandlePointer (mapProjectionHandle,
																			kLock);
			
			fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
			
			errCode = MGetMarker (fileStreamPtr, 
											&savedPosOff,
											kErrorMessages);
			
			if (errCode == noErr)
				errCode = MSetMarker (fileStreamPtr, 
												fsFromStart, 
												imageFileDirectoryPtr->value,
												kErrorMessages);
												
			count = MIN (imageFileDirectoryPtr->count, 100);			
			count *= 2;
			if (errCode == noErr)
				errCode = MReadData (fileStreamPtr, 
												&count, 
												(char*)&geoTagInformation,
												kErrorMessages); 
												
			if (errCode == noErr)
				{
						// Convert to number of 2 byte integers.
						
				count /= 2;
				
						//  Convert to number of geo key records.
						
				count /= 4;
				
				index = 0;		
 				for (recordIndex=0; recordIndex<count; recordIndex++)
					{
					geoKeyRecordPtr = &geoTagInformation[index];
					for (i=0; i<4; i++, index++)
								// Swap the bytes if needed.
						geoTagInformation[index] = 
										(UInt16)GetShortIntValue ((char*)&geoTagInformation[index]);
				
							// The first geo key record is the version and number of geo keys.
							// We will skip it.
						
					if (recordIndex == 0)
						geoKeyRecordPtr[0] = 0;
										
					switch (geoKeyRecordPtr[0])
						{
						case 1024:	// GTModelTypeGeoKey
							if (geoKeyRecordPtr[3] == 2)
								{
								mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = kGeographicRSCode;
								mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kDecimalDegreesCode;
								
								}	// end "if (geoKeyRecordPtr[3] == 2)"
							break;
							
						case 1025:	// GTRasterTypeGeoKey RasterPixelIsArea (=1) or RasterPixelIsPoint (=2)
							*rasterTypeGeoKeyPtr = geoKeyRecordPtr[3];
							if (*rasterTypeGeoKeyPtr < 1 || *rasterTypeGeoKeyPtr > 2)
								*rasterTypeGeoKeyPtr = 1;
							break;
							
						case 1026:	// GTCitationGeoKey
									// Used when description of image is listed
							break;
							
						case 2048:	// GeographicTypeGeoKey
							if (geoKeyRecordPtr[3] == 4035)
								mapProjectionInfoPtr->geodetic.datumCode = kSphereDatumCode;
								
							else if (geoKeyRecordPtr[3] == 4121)
								mapProjectionInfoPtr->geodetic.datumCode = kGGRS87DatumCode;
								
							else if (geoKeyRecordPtr[3] == 4221)
								mapProjectionInfoPtr->geodetic.datumCode = kCampoCode;
								
							else if (geoKeyRecordPtr[3] == 4267)
								mapProjectionInfoPtr->geodetic.datumCode = kNAD27Code;
								
							else if (geoKeyRecordPtr[3] == 4269)
								mapProjectionInfoPtr->geodetic.datumCode = kNAD83Code;
								
							else if (geoKeyRecordPtr[3] == 4272)
								mapProjectionInfoPtr->geodetic.datumCode = kNewZealandGeodeticDatum1949Code;
								
							else if (geoKeyRecordPtr[3] == 4277)
								mapProjectionInfoPtr->geodetic.datumCode = kDatum_OSGB_1936Code;
								
							else if (geoKeyRecordPtr[3] == 4283)
								mapProjectionInfoPtr->geodetic.datumCode = kGeocentricDatumAustralia1994Code;
								
							else if (geoKeyRecordPtr[3] == 4284)
								mapProjectionInfoPtr->geodetic.datumCode = kPulkovo1942DatumCode;
								
							else if (geoKeyRecordPtr[3] == 4326)
								mapProjectionInfoPtr->geodetic.datumCode = kWGS84Code;
								
							SetEllipsoidFromDatum (mapProjectionInfoPtr);
							break;
							
						case 2049:	// GeogCitationGeoKey
									// Used when description of image is listed
							break;
							
						case 2050:	// GeogGeodeticDatumGeoKey
							if (geoKeyRecordPtr[3] == 6035)
								mapProjectionInfoPtr->geodetic.datumCode = kSphereDatumCode;
								
							else if (geoKeyRecordPtr[3] == 6121)
								mapProjectionInfoPtr->geodetic.datumCode = kGGRS87DatumCode;
								
							else if (geoKeyRecordPtr[3] == 6221)
								mapProjectionInfoPtr->geodetic.datumCode = kCampoCode;
								
							else if (geoKeyRecordPtr[3] == 6267)
								mapProjectionInfoPtr->geodetic.datumCode = kNAD27Code;
								
							else if (geoKeyRecordPtr[3] == 6269)
								mapProjectionInfoPtr->geodetic.datumCode = kNAD83Code;
								
							else if (geoKeyRecordPtr[3] == 6272)
								mapProjectionInfoPtr->geodetic.datumCode = kNewZealandGeodeticDatum1949Code;
								
							else if (geoKeyRecordPtr[3] == 6277)
								mapProjectionInfoPtr->geodetic.datumCode = kDatum_OSGB_1936Code;
								
							else if (geoKeyRecordPtr[3] == 6283)
								mapProjectionInfoPtr->geodetic.datumCode = kGeocentricDatumAustralia1994Code;
								
							else if (geoKeyRecordPtr[3] == 6284)
								mapProjectionInfoPtr->geodetic.datumCode = kPulkovo1942DatumCode;
								
							else if (geoKeyRecordPtr[3] == 6326)
								mapProjectionInfoPtr->geodetic.datumCode = kWGS84Code;
							break;
							
						case 2052:	// GeogLinearUnitsGeoKey
							if (geoKeyRecordPtr[3] == 9001)
								mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kMetersCode;
							else if (geoKeyRecordPtr[3] == 9002)
								mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kFeetCode;
							else if (geoKeyRecordPtr[3] == 9003)
								mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kUSSurveyFeetCode;
							break;
							
						case 2054:	// GeogAngularUnitsGeoKey
							break;
							
						case 2056:	// geogEllipsoidGeoKey
							switch (geoKeyRecordPtr[3])
								{
								case 7001:
									mapProjectionInfoPtr->geodetic.spheroidCode = kAiryEllipsoidCode;
									break;
								case 7003:
									mapProjectionInfoPtr->geodetic.spheroidCode = kAustralianEllipsoidCode;
									break;
								case 7004:
									mapProjectionInfoPtr->geodetic.spheroidCode = kBesselEllipsoidCode;
									break;
								case 7005:
									mapProjectionInfoPtr->geodetic.spheroidCode = kBesselModifiedEllipsoidCode;
									break;
								case 7008:
									mapProjectionInfoPtr->geodetic.spheroidCode = kClarke1866EllipsoidCode;
									break;
								case 7015:
									mapProjectionInfoPtr->geodetic.spheroidCode = kEverestEllipsoidCode;
									break;
								case 7019:
									mapProjectionInfoPtr->geodetic.spheroidCode = kGRS80EllipsoidCode;
									break;
								case 7022:
									mapProjectionInfoPtr->geodetic.spheroidCode = kInternational1909EllipsoidCode;
									break;
								case 7024:
									mapProjectionInfoPtr->geodetic.spheroidCode = kKrassovskyEllipsoidCode;
									break;
								case 7030:
									mapProjectionInfoPtr->geodetic.spheroidCode = kWGS84EllipsoidCode;
									break;
								case 7034:
									mapProjectionInfoPtr->geodetic.spheroidCode = kClarke1880EllipsoidCode;
									break;
								case 7035:
									mapProjectionInfoPtr->geodetic.spheroidCode = kSphereEllipsoidCode;
									break;
								case 7050:
									mapProjectionInfoPtr->geodetic.spheroidCode = kGRS1967ModifiedEllipsoidCode;
									break;
								
								}	// end "switch (geoKeyRecordPtr[3])"
							break;
								
						case 2057:		// GeogSemiMajorAxisGeoKey
							if (geoDoubleParamsPtr->tag == geoKeyRecordPtr[1])
								mapProjectionInfoPtr->geodetic.semiMajorAxis = geoDoubleParameters[geoKeyRecordPtr[3]];
							break;
								
						case 2058:		// GeogSemiMinorAxisGeoKeyCode
							if (geoDoubleParamsPtr->tag == geoKeyRecordPtr[1])
								mapProjectionInfoPtr->geodetic.semiMinorAxis = geoDoubleParameters[geoKeyRecordPtr[3]];
							break;
								
						case 2059:		// ProjNatOriginLongGeoKey
							if (geoDoubleParamsPtr->tag == geoKeyRecordPtr[1])
								inverseFlatteningParameter = geoDoubleParameters[geoKeyRecordPtr[3]];
							break;
							
						case 3072:	// ProjectedCSTypeGeoKey 
							projectedCSTypeGeoKey = geoKeyRecordPtr[3]; 
							if (projectedCSTypeGeoKey > 32200 && 
																		projectedCSTypeGeoKey <= 32260)
								{
								mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = kUTM_WGS72RSCode;
								mapProjectionInfoPtr->gridCoordinate.projectionCode = kTransverseMercatorCode;
								mapProjectionInfoPtr->geodetic.spheroidCode = 
																				kWGS72EllipsoidCode;
								mapProjectionInfoPtr->gridCoordinate.gridZone = 
																	(SInt16)(projectedCSTypeGeoKey - 32200);
								mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kMetersCode;
								
								}	// end "if (projectedCSTypeGeoKey > 32200 && ..."
								
							else if (projectedCSTypeGeoKey > 32300 && 
																		projectedCSTypeGeoKey <= 32360)
								{
								mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = kUTM_WGS72RSCode;
								mapProjectionInfoPtr->gridCoordinate.projectionCode = kTransverseMercatorCode;
								mapProjectionInfoPtr->geodetic.spheroidCode = 
																				kWGS72EllipsoidCode;
								mapProjectionInfoPtr->gridCoordinate.gridZone = 
																	(SInt16)(-(projectedCSTypeGeoKey - 32300));
								mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kMetersCode;
								
								}	// end "if (projectedCSTypeGeoKey > 32600 && ..."
								
							else if (projectedCSTypeGeoKey > 32600 && 
																		projectedCSTypeGeoKey <= 32660)
								{
								mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = kUTM_WGS84RSCode;
								mapProjectionInfoPtr->gridCoordinate.projectionCode = kTransverseMercatorCode;
								mapProjectionInfoPtr->geodetic.datumCode = kWGS84Code;
								mapProjectionInfoPtr->geodetic.spheroidCode = 
																				kWGS84EllipsoidCode;
								mapProjectionInfoPtr->gridCoordinate.gridZone = 
																	(SInt16)(projectedCSTypeGeoKey - 32600);
								mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kMetersCode;
								
								}	// end "if (projectedCSTypeGeoKey > 32600 && ..."
								
							else if (projectedCSTypeGeoKey > 32700 && 
																		projectedCSTypeGeoKey <= 32760)
								{
								mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = kUTM_WGS84RSCode;
								mapProjectionInfoPtr->gridCoordinate.projectionCode = kTransverseMercatorCode;
								mapProjectionInfoPtr->geodetic.datumCode = kWGS84Code;
								mapProjectionInfoPtr->geodetic.spheroidCode = 
																				kWGS84EllipsoidCode;
								mapProjectionInfoPtr->gridCoordinate.gridZone = 
																	(SInt16)(-(projectedCSTypeGeoKey - 32700));
								mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kMetersCode;
								
								}	// end "if (projectedCSTypeGeoKey > 32600 && ..."
								
							else if (projectedCSTypeGeoKey > 26703 && 
																		projectedCSTypeGeoKey <= 26722)
								{
								mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = kUTM_NAD27RSCode;
								mapProjectionInfoPtr->gridCoordinate.projectionCode = kTransverseMercatorCode;
								mapProjectionInfoPtr->geodetic.datumCode = kNAD27Code;
								mapProjectionInfoPtr->geodetic.spheroidCode = 
																				kClarke1866EllipsoidCode;
								mapProjectionInfoPtr->gridCoordinate.gridZone = 
																	(SInt16)(projectedCSTypeGeoKey - 26700);
								mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kMetersCode;
								
								}	// end "if (projectedCSTypeGeoKey > 32600 && ..."
								
							else if (projectedCSTypeGeoKey > 26903 && 
																		projectedCSTypeGeoKey <= 26923)
								{
								mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = kUTM_NAD83RSCode;
								mapProjectionInfoPtr->gridCoordinate.projectionCode = kTransverseMercatorCode;
								mapProjectionInfoPtr->geodetic.datumCode = kNAD83Code;
								mapProjectionInfoPtr->geodetic.spheroidCode = 
																				kGRS80EllipsoidCode;
								mapProjectionInfoPtr->gridCoordinate.gridZone = 
																	(SInt16)(projectedCSTypeGeoKey - 26900);
								mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kMetersCode;
								
								}	// end "if (projectedCSTypeGeoKey > 32600 && ..."
								
							else if ((projectedCSTypeGeoKey >= 26729 && 
																		projectedCSTypeGeoKey <= 26798) ||
										(projectedCSTypeGeoKey >= 32001 && 
																		projectedCSTypeGeoKey <= 32058))
								{
								mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = kStatePlaneNAD27RSCode;
								mapProjectionInfoPtr->geodetic.datumCode = kNAD27Code;
								mapProjectionInfoPtr->geodetic.spheroidCode = 
																				kClarke1866EllipsoidCode;
								mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kUSSurveyFeetCode;
								
								switch (projectedCSTypeGeoKey)
									{
									case 26773:
										mapProjectionInfoPtr->gridCoordinate.gridZone = 1301;
										break;
										
									case 26774:
										mapProjectionInfoPtr->gridCoordinate.gridZone = 1302;
										break;
										
									case 32018:
										mapProjectionInfoPtr->gridCoordinate.gridZone = 3104;
										break;
									
									}	// end "switch (projectedCSTypeGeoKey)"
								
								}	// end "if (projectedCSTypeGeoKey > 32600 && ..."
								
							else if ((projectedCSTypeGeoKey >= 26929 && 
																		projectedCSTypeGeoKey <= 26998) ||
										(projectedCSTypeGeoKey >= 32100 && 
																		projectedCSTypeGeoKey <= 32158))
								{
								mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = kStatePlaneNAD83RSCode;
								mapProjectionInfoPtr->geodetic.datumCode = kNAD83Code;
								mapProjectionInfoPtr->geodetic.spheroidCode = 
																				kGRS80EllipsoidCode;
								mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kMetersCode;
								
								switch (projectedCSTypeGeoKey)
									{
									case 26973:
										mapProjectionInfoPtr->gridCoordinate.gridZone = 1301;
										break;
										
									case 26974:
										mapProjectionInfoPtr->gridCoordinate.gridZone = 1302;
										break;
										
									case 32118:
										mapProjectionInfoPtr->gridCoordinate.gridZone = 3104;
										break;
									
									}	// end "switch (projectedCSTypeGeoKey)"
								
								}	// end "if (projectedCSTypeGeoKey >= 32600 && ..."
								
							else if (projectedCSTypeGeoKey >= 28348 && 
																		projectedCSTypeGeoKey <= 28358)
								{
								mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = kGDA94RSCode;
								mapProjectionInfoPtr->gridCoordinate.projectionCode = kTransverseMercatorCode;
								mapProjectionInfoPtr->geodetic.datumCode = kGeocentricDatumAustralia1994Code;
								mapProjectionInfoPtr->geodetic.spheroidCode = 
																				kGRS80EllipsoidCode;
								mapProjectionInfoPtr->gridCoordinate.gridZone = 
																	(SInt16)(projectedCSTypeGeoKey - 28300);
								mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kMetersCode;
								
								}	// end "if (projectedCSTypeGeoKey >= 28348 && ..."
								
							else if (projectedCSTypeGeoKey >= 28402 && 
																		projectedCSTypeGeoKey <= 28432)
								{
								mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = kGaussKrugerRSCode;
								mapProjectionInfoPtr->gridCoordinate.projectionCode = kTransverseMercatorCode;
								mapProjectionInfoPtr->geodetic.datumCode = kPulkovo1942DatumCode;
								mapProjectionInfoPtr->geodetic.spheroidCode = 
																				kKrassovskyEllipsoidCode;
								mapProjectionInfoPtr->gridCoordinate.gridZone = 
																	(SInt16)(projectedCSTypeGeoKey - 28400);
								mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kMetersCode;
								
								}	// end "if (projectedCSTypeGeoKey >= 28402 && ..."
								
							else if (projectedCSTypeGeoKey >= 29177 && 
																		projectedCSTypeGeoKey <= 29185)
								{
								mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = kUTM_SAD69RSCode;
								mapProjectionInfoPtr->gridCoordinate.projectionCode = kTransverseMercatorCode;
								mapProjectionInfoPtr->geodetic.datumCode = kSAD69DatumCode;
								mapProjectionInfoPtr->geodetic.spheroidCode = 
																				kGRS1967ModifiedEllipsoidCode;
								mapProjectionInfoPtr->gridCoordinate.gridZone = 
																	(SInt16)(-(projectedCSTypeGeoKey - 29160));
								mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kMetersCode;
								
								}	// end "if (projectedCSTypeGeoKey >= 29177 && ..."
								
									// Save this geo key in case needed to write out to a new file.
									
							mapProjectionInfoPtr->projectedCSTypeGeoKey = projectedCSTypeGeoKey;
							break;
							
						case 3073:	// PCSCitationGeoKey
									// Used when description of image is listed
							break;
							
						case 3074:		// ProjectionGeoKey 
							projectionGeoKey = geoKeyRecordPtr[3]; 
							break;
							
						case 3075:		// ProjCoordTransGeoKey 
							projectedCoordTransGeoKey = geoKeyRecordPtr[3]; 
							switch (projectedCoordTransGeoKey)
								{
								case 1:
									mapProjectionInfoPtr->gridCoordinate.projectionCode = kTransverseMercatorCode;
									break;
								case 2:
									mapProjectionInfoPtr->gridCoordinate.projectionCode = kAlaskaConformalCode;
									break;
								case 3:
									mapProjectionInfoPtr->gridCoordinate.projectionCode = kSpaceObliqueMercatorCode;
									break;
								case 7:
									mapProjectionInfoPtr->gridCoordinate.projectionCode = kMercatorCode;
									break;
								case 8:
									mapProjectionInfoPtr->gridCoordinate.projectionCode = kLambertConformalConicCode;
									break;
								case 10:
									mapProjectionInfoPtr->gridCoordinate.projectionCode = kLambertAzimuthalEqualAreaCode;
									break;
								case 11:
									mapProjectionInfoPtr->gridCoordinate.projectionCode = kAlbersConicalEqualAreaCode;
									break;
								case 12:
									mapProjectionInfoPtr->gridCoordinate.projectionCode = kAzimuthalEquidistantCode;
									break;
								case 13:
									mapProjectionInfoPtr->gridCoordinate.projectionCode = kEquidistantConicCode;
									break;
								case 14:
									mapProjectionInfoPtr->gridCoordinate.projectionCode = kStereographicCode;
									break;
								case 15:
									mapProjectionInfoPtr->gridCoordinate.projectionCode = kPolarStereographicCode;
									break;
								case 17:
									mapProjectionInfoPtr->gridCoordinate.projectionCode = kEquirectangularCode;
									break;
								case 19:
									mapProjectionInfoPtr->gridCoordinate.projectionCode = kGnomonicCode;
									break;
								case 20:
									mapProjectionInfoPtr->gridCoordinate.projectionCode = kMillerCylindricalCode;
									break;
								case 21:
									mapProjectionInfoPtr->gridCoordinate.projectionCode = kOrthographicCode;
									
											// This projection is always based on a sphere spheroid.
									mapProjectionInfoPtr->geodetic.spheroidCode = kSphereEllipsoidCode;
									mapProjectionInfoPtr->geodetic.datumCode = kSphereDatumCode;
									break;
								case 22:
									mapProjectionInfoPtr->gridCoordinate.projectionCode = kPolyconicCode;
									break;
								case 23:
									mapProjectionInfoPtr->gridCoordinate.projectionCode = kRobinsonCode;
									break;
								case 24:
									mapProjectionInfoPtr->gridCoordinate.projectionCode = kSinusoidalCode;
									break;
								case 25:
									mapProjectionInfoPtr->gridCoordinate.projectionCode = kVanderGrintenICode;
									break;
								case 26:
									mapProjectionInfoPtr->gridCoordinate.projectionCode = kNewZealandMapGridCode;
									break;
								case 27:
									mapProjectionInfoPtr->gridCoordinate.projectionCode = kTransvMercatorSouthOrientedCode;
									break;
								case 28:
									mapProjectionInfoPtr->gridCoordinate.projectionCode = kCylindricalEqualAreaCode;
									break;
								
								}	// end "switch (geoKeyRecordPtr[3])"
							break;
							
						case 3076:		// ProjLinearUnitsGeoKey 
							switch (geoKeyRecordPtr[3])
								{
								case 9001:
									mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kMetersCode;
									break;
									
								case 9002:
									mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kFeetCode;
									break;
									
								case 9003:
									mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kUSSurveyFeetCode;
									break;
								
								}	// end "switch (geoKeyRecordPtr[3])"
							break;
								
						case 3078:		// ProjStdParallel1GeoKey
							if (geoDoubleParamsPtr->tag == geoKeyRecordPtr[1])
								mapProjectionInfoPtr->gridCoordinate.standardParallel1 =
																	geoDoubleParameters[geoKeyRecordPtr[3]];
							break;
								
						case 3079:		// ProjStdParallel2GeoKey
							if (geoDoubleParamsPtr->tag == geoKeyRecordPtr[1])
								mapProjectionInfoPtr->gridCoordinate.standardParallel2 =
																	geoDoubleParameters[geoKeyRecordPtr[3]];
							break;
								
						case 3080:		// ProjNatOriginLongGeoKey
							if (geoDoubleParamsPtr->tag == geoKeyRecordPtr[1])
								mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian = 
																	geoDoubleParameters[geoKeyRecordPtr[3]];
							break;
								
						case 3081:		// ProjNatOriginLatGeoKey
							if (geoDoubleParamsPtr->tag == geoKeyRecordPtr[1])
								mapProjectionInfoPtr->gridCoordinate.latitudeOrigin =
																	geoDoubleParameters[geoKeyRecordPtr[3]];
							break;
								
						case 3082:		// ProjFalseEastingGeoKey
							if (geoDoubleParamsPtr->tag == geoKeyRecordPtr[1])
								mapProjectionInfoPtr->gridCoordinate.falseEasting =
																	geoDoubleParameters[geoKeyRecordPtr[3]];
							break;
								
						case 3083:		// ProjFalseNorthingGeoKey
							if (geoDoubleParamsPtr->tag == geoKeyRecordPtr[1])
								mapProjectionInfoPtr->gridCoordinate.falseNorthing =
																	geoDoubleParameters[geoKeyRecordPtr[3]];
							break;
								
						case 3084:		// ProjFalseOriginLongGeoKey
							if (geoDoubleParamsPtr->tag == geoKeyRecordPtr[1])
								mapProjectionInfoPtr->gridCoordinate.falseOriginLongitude =
																	geoDoubleParameters[geoKeyRecordPtr[3]];
							break;
								
						case 3085:		// ProjFalseOriginLatGeoKey
							if (geoDoubleParamsPtr->tag == geoKeyRecordPtr[1])
								mapProjectionInfoPtr->gridCoordinate.falseOriginLatitude =
																	geoDoubleParameters[geoKeyRecordPtr[3]];
							break;
								
						case 3086:		// ProjFalseEastingLatGeoKey
							if (geoDoubleParamsPtr->tag == geoKeyRecordPtr[1])
								mapProjectionInfoPtr->gridCoordinate.falseOriginEasting =
																	geoDoubleParameters[geoKeyRecordPtr[3]];
							break;
								
						case 3087:		// ProjFalseNorthingLatGeoKey
							if (geoDoubleParamsPtr->tag == geoKeyRecordPtr[1])
								mapProjectionInfoPtr->gridCoordinate.falseOriginNorthing =
																	geoDoubleParameters[geoKeyRecordPtr[3]];
							break;
								
						case 3088:		// ProjCenterLongGeoKey
							if (geoDoubleParamsPtr->tag == geoKeyRecordPtr[1])
								mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian =
																	geoDoubleParameters[geoKeyRecordPtr[3]];
							break;
								
						case 3089:		// ProjCenterLatGeoKey
							if (geoDoubleParamsPtr->tag == geoKeyRecordPtr[1])
								mapProjectionInfoPtr->gridCoordinate.latitudeOrigin =
																	geoDoubleParameters[geoKeyRecordPtr[3]];
							break;
								
						case 3092:		// ProjScaleAtNatOriginGeoKey
							if (geoDoubleParamsPtr->tag == geoKeyRecordPtr[1])
								mapProjectionInfoPtr->gridCoordinate.scaleFactorOfCentralMeridian =
																	geoDoubleParameters[geoKeyRecordPtr[3]];
							break;
								
						case 3093:		// ProjScaleAtCenterGeoKey
							if (geoDoubleParamsPtr->tag == geoKeyRecordPtr[1])
								mapProjectionInfoPtr->gridCoordinate.scaleFactorOfCentralMeridian =
																	geoDoubleParameters[geoKeyRecordPtr[3]];
							break;
								
						case 3094:		// ProjAzimuthAngleGeoKey
							if (geoDoubleParamsPtr->tag == geoKeyRecordPtr[1])
								mapProjectionInfoPtr->gridCoordinate.projAzimuthAngle =
																	geoDoubleParameters[geoKeyRecordPtr[3]];
							break;
								
						case 3095:		// ProjStraightVertPoleLongGeoKey
							if (geoDoubleParamsPtr->tag == geoKeyRecordPtr[1])
								mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian =
																	geoDoubleParameters[geoKeyRecordPtr[3]];
							break;
						
						}	// end "switch (geoKeyRecordPtr[0])"
										
					}	// end "for (recordIndex=0; recordIndex<count; recordIndex++)"
				
				}	// end "if (errCode == noErr)"
					
			if (errCode == noErr)
				errCode = MSetMarker (fileStreamPtr, 
												fsFromStart, 
												savedPosOff,
												kErrorMessages);
			
			if (mapProjectionInfoPtr->gridCoordinate.projectionCode == kNotDefinedCode &&
						mapProjectionInfoPtr->gridCoordinate.referenceSystemCode >= kGaussKrugerRSCode)
				mapProjectionInfoPtr->gridCoordinate.projectionCode = 
									GetProjectionCodeFromReferenceSystemCode (
													mapProjectionInfoPtr->gridCoordinate.referenceSystemCode,
													mapProjectionInfoPtr->gridCoordinate.gridZone);
												
			CheckAndUnlockHandle (mapProjectionHandle);
												
			}	// end "if (mapProjectionHandle != NULL)"
			
		}	// end "if (imageFileDirectoryPtr->type == kTIFFShort && ..."
		
	return (errCode);
	
}	// end "GetGeoKeyParameters"
#endif		// use_multispec_tiffcode



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetSpecifiedTIFFKeyDirectory
//
//	Software purpose:	The purpose of this routine is to get the specified TIFF
//							Key directory.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:			ListDescriptionInformation in SOther.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/18/2012
//	Revised By:			Larry L. Biehl			Date: 05/14/2012

Boolean GetSpecifiedTIFFKeyDirectory (
				FileInfoPtr 						fileInfoPtr,
				UInt32								tiffKeyNumber,
				TIFFImageFileDirectory*			tiffFileDirectoryPtr)

{
	char									headerRecordPtr[512];
	
	CMFileStream*						fileStreamPtr;
										
	SInt64								ifdOffset;
										
	UInt32								count,
											ifdOffset32Bit;
	
	SInt16								errCode,
											item,
											numberEntries;
											
	Boolean								foundFlag = FALSE,
											savedSwapBytesFlag;
	
								
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
	
			// Set the global swap bytes flag to reflect the data file being read.													
		
	savedSwapBytesFlag = gSwapBytesFlag;
	gSwapBytesFlag = fileInfoPtr->swapHeaderBytesFlag;
	
			// Make certain that we are at the beginning of the file.
	
	ifdOffset = 4;
	errCode = MSetMarker (fileStreamPtr, fsFromStart, ifdOffset, kErrorMessages);
	
			// Read bytes 4-7 bytes of the data.
	
	count = 4;
	if (errCode == noErr)
		errCode = MReadData (fileStreamPtr, &count, headerRecordPtr, kNoErrorMessages); 
				
	if (errCode == noErr)
		{
				// Get first Image File Directory.											
			
		BlockMoveData (headerRecordPtr, (char*)&ifdOffset32Bit, 4);
		ifdOffset = (UInt32)GetLongIntValue ((char*)&ifdOffset32Bit);

				// Initialize some tags that will be saved.
				
		tiffFileDirectoryPtr->tag = 0;	
				
		errCode = MSetMarker (fileStreamPtr, 
										fsFromStart, 
										ifdOffset,
										kErrorMessages);
			
				// Read in the first image file directory.							
		
		if (errCode == noErr)
			{
			count = 2;
			errCode = MReadData (fileStreamPtr, 
											&count, 
											&numberEntries,
											kErrorMessages);
				
			}	// end "if (errCode == noErr)" 
			
		if (errCode == noErr)
			{
			numberEntries = GetShortIntValue ((char*)&numberEntries);
			
			for (item=1; item<=numberEntries; item++)
				{
				errCode = GetTiffEntry (fileStreamPtr,
													headerRecordPtr,
													tiffFileDirectoryPtr);
				
				if (tiffFileDirectoryPtr->tag >= tiffKeyNumber)
					{
					if (tiffFileDirectoryPtr->tag == tiffKeyNumber)
						foundFlag = TRUE;
						
					break;
					
					}	// end "if (imageFileDirectoryPtr->tag >= tiffKeyNumber)"
					
				if (tiffFileDirectoryPtr->tag == 0 || errCode != noErr)
					break;
					
				}	// end "for (item=1, item<=numberEntries, item++)" 
																					
			}	// end "if (errCode == noErr)"
					
		}	// end "if (errCode == noErr)"	
		
			// Reset the global swap byte flag
				
	gSwapBytesFlag = savedSwapBytesFlag;
		
	return (foundFlag);
	
}	// end "GetSpecifiedTIFFKeyDirectory"
  

//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetSwapBytesFlagForTiffFile
//
//	Software purpose:	The purpose of this routine is to read the Geo Key Parameters.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/07/2012
//	Revised By:			Larry L. Biehl			Date: 02/07/2012

Boolean GetSwapBytesFlagForTiffFile (
				char*									headerRecordPtr)

{	
	unsigned char						charKeyCode[4];
	
	SInt16								stringCompare;
	
	Boolean								swapBytesFlag = FALSE;
	
	
	if (headerRecordPtr != NULL)
		{	
		charKeyCode[0] = 0x4d;
		charKeyCode[1] = 0x4d;
		charKeyCode[2] = 0x00;
		charKeyCode[3] = 0x2a;		                                                          
		stringCompare = (SInt16)strncmp ((char*)headerRecordPtr, (CharPtr)charKeyCode, 4);
		
		if (stringCompare == 0)
			swapBytesFlag = !gBigEndianFlag;
			
		else	// stringCompare != 0
			{              
			charKeyCode[0] = 0x49;
			charKeyCode[1] = 0x49;
			charKeyCode[2] = 0x2a;
			charKeyCode[3] = 0x00;             
			stringCompare = (SInt16)strncmp ((char*)headerRecordPtr, (CharPtr)charKeyCode, 4);
										
			if (stringCompare == 0)
				swapBytesFlag = gBigEndianFlag;
				
			}	// end "if (stringCompare != 0)" 
			
		}	// end "if (headerRecordPtr != NULL)"
		
	return (swapBytesFlag);
	
}	// end "GetSwapBytesFlagForTiffFile"
							


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetTiffEntry
//
//	Software purpose:	This routine reads a TIFF Image File Directory
//							and interprets the parameters.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/16/1994
//	Revised By:			Larry L. Biehl			Date: 07/07/2005

SInt16 GetTiffEntry (
				CMFileStream* 				fileStreamPtr,
				char*							inputBufferPtr,
				TIFF_IFD_Ptr				imageFileDirectoryPtr)

{
	UInt32						count,
									tempLongInt,
									value;
	
	SInt16						errCode,
									tag,
									tempInt,
									type;
	
			
	count = 12;
	errCode = MReadData (fileStreamPtr, &count, inputBufferPtr, kErrorMessages);
	
	if (errCode == noErr)
		{
				// Get the tag number.												
				
		BlockMoveData ((char*)&inputBufferPtr[0], (char*)&tempInt, 2);
		
      tag = GetShortIntValue ((char*)&tempInt);
			
				// Get the type code.												
				
		BlockMoveData ((char*)&inputBufferPtr[2], (char*)&tempInt, 2);
		
      type = GetShortIntValue ((char*)&tempInt);
			
				// Get the number of values of the indicated type.			
				
		BlockMoveData ((char*)&inputBufferPtr[4], (char*)&tempLongInt, 4);
		
		count = (UInt32)GetLongIntValue ((char*)&tempLongInt);
			
				// Get the value or the offset.									
		
		if (type == kTIFFShort && count == 1)
			{
			BlockMoveData ((char*)&inputBufferPtr[8], (char*)&tempInt, 2);
			
      	value = (UInt32)((UInt16)GetShortIntValue ((char*)&tempInt));
				
			}	// end "if (type == kTIFFShort && count == 1)" 
			
		else if (type == kTIFFLong || kTIFFRational || count > 1)
			{
			BlockMoveData ((char*)&inputBufferPtr[8], (char*)&tempLongInt, 4);
			
			value = (UInt32)GetLongIntValue ((char*)&tempLongInt);
				
			}	// end "if (type == kTIFFLong || kTIFFRational || count > 1)" 
			
		else	// something else
			value = 0;
			
		imageFileDirectoryPtr->tag = (UInt16)tag;
		imageFileDirectoryPtr->type = type;
		imageFileDirectoryPtr->count = count;
		imageFileDirectoryPtr->value = value;
			
		}	// end "if (errCode == noErr)" 
		
	else	// errCode != noErr 
		imageFileDirectoryPtr->tag = 0;
			
	return (errCode);
	
}	// end "GetTiffEntry"    
  


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetTIFFASCIIParameters
//
//	Software purpose:	The purpose of this routine is to read the text string for the
//							input tag.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/04/2000
//	Revised By:			Larry L. Biehl			Date: 12/22/2003

SInt16 GetTIFFASCIIParameters (
				CMFileStream*						fileStreamPtr,
				TIFF_IFD_Ptr						imageFileDirectoryPtr,
				char*									stringPtr,
				SInt32								numberCharacters)

{	
	SInt64								savedPosOff;
											
	UInt32								count;
	
	SInt16								errCode = -1;
	
	
	if (imageFileDirectoryPtr->type == kTIFFASCII && 
						numberCharacters <= (SInt32)imageFileDirectoryPtr->count)
		{		
		errCode = MGetMarker (fileStreamPtr, 
										&savedPosOff,
										kErrorMessages);
		
		if (errCode == noErr)
			errCode = MSetMarker (fileStreamPtr, 
											fsFromStart, 
											imageFileDirectoryPtr->value,
											kErrorMessages);
											
		count = (UInt32)numberCharacters;			
		if (errCode == noErr)
			errCode = MReadData (fileStreamPtr, 
											&count, 
											stringPtr,
											kErrorMessages);
		
				// Make sure the string is terminated.
																
		if (errCode == noErr)
			stringPtr[count] = 0;
											
				
		if (errCode == noErr)
			errCode = MSetMarker (fileStreamPtr, 
											fsFromStart, 
											savedPosOff,
											kErrorMessages);
			
		}	// end "if (imageFileDirectoryPtr->type == kTIFFASCII && ..."
		
	return (errCode);
	
}	// end "GetTIFFASCIIParameters"
  


#if use_multispec_tiffcode
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetTIFFDoubleParameters
//
//	Software purpose:	The purpose of this routine is to set the pixel size parameters
//							using the information in the GEOTIFF tag.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/01/2000
//	Revised By:			Larry L. Biehl			Date: 12/22/2003

SInt16 GetTIFFDoubleParameters (
				CMFileStream*						fileStreamPtr,
				TIFF_IFD_Ptr						imageFileDirectoryPtr,
				double*								doubleParameterPtr,
				SInt32								numberParameters)

{	
	UInt8									tempDouble[8];
	
	SInt64								savedPosOff;
	
	SInt32								index;
											
	UInt32								count;
	
	SInt16								errCode = -1;
	
	
	if (imageFileDirectoryPtr->type == kTIFFDouble && 
										numberParameters <= (SInt32)imageFileDirectoryPtr->count)
		{		
		errCode = MGetMarker (fileStreamPtr, 
										&savedPosOff,
										kErrorMessages);
		
		if (errCode == noErr)
			errCode = MSetMarker (fileStreamPtr, 
											fsFromStart, 
											imageFileDirectoryPtr->value,
											kErrorMessages);
											
		for (index=0; index<numberParameters; index++)
			{
			count = 8;			
			if (errCode == noErr)
				errCode = MReadData (fileStreamPtr, 
												&count, 
												tempDouble,
												kErrorMessages);
					
			if (errCode != noErr)
				break;
												
			doubleParameterPtr[index] = GetDoubleValue (tempDouble);
			
			}	// end "for (index=0; index<numberParameters; index++)"
				
		if (errCode == noErr)
			errCode = MSetMarker (fileStreamPtr, 
											fsFromStart, 
											savedPosOff,
											kErrorMessages);
			
		}	// end "if (imageFileDirectoryPtr->type == kTIFFDouble && count <= ..."
		
	return (errCode);
	
}	// end "GetTIFFDoubleParameters"
#endif		// use_multispec_tiffcode
  


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetTIFFImageDescription
//
//	Software purpose:	The purpose of this routine is to get the text for the "Image
//							Description Tag (270) in the TIFF/GeoTIFF file.
//
//	Parameters in:		None
//
//	Parameters out:	Text for the TIFF Image Description tag (270). No text is returned
//							if there is no 270 tag.
//
//	Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 06/28/2018
//	Revised By:			Larry L. Biehl			Date: 06/28/2018

void GetTIFFImageDescription (
				FileInfoPtr							fileInfoPtr,
				char*									imageDescriptionStringPtr,
				UInt32								stringLengthLimit)

{
	CMFileStream*						fileStreamPtr;
	TIFFImageFileDirectory			tiffFileDirectory;
	
	
	imageDescriptionStringPtr[0] = 0;
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
	if (GetSpecifiedTIFFKeyDirectory (fileInfoPtr,
													270,
													&tiffFileDirectory))
		{
		stringLengthLimit = (SInt16)MIN (tiffFileDirectory.count, stringLengthLimit);
		GetTIFFASCIIParameters (fileStreamPtr,
										&tiffFileDirectory,
										imageDescriptionStringPtr,
										stringLengthLimit);
		
		}	// end "if (GetSpecifiedTIFFKeyDirectory (fileInfoPtr, ..."
	
}	// end "GetTIFFImageDescription"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetTIFFNumberHeaderBytes
//
//	Software purpose:	The purpose of this routine is to get the number of header
//							bytes in the TIFF/GeoTIFF file. 0 will be returned if the number
//							of header bytes cannot be determined or if the data in the file
//							is compressed, tiled or striped. The data needs to be organized
//							such that one can calculate where a specific pixel is just based
//							on the number of header bytes and the number of lines, columns
//							and bytes per pixel.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:			ListDescriptionInformation in SOther.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/16/2013
//	Revised By:			Larry L. Biehl			Date: 04/16/2013

UInt32 GetTIFFNumberHeaderBytes (
				FileInfoPtr 						fileInfoPtr)

{
	TIFFImageFileDirectory			tiffFileDirectory;
	
	UInt32								numberHeaderBytes = 0;
	
								
	if (GetSpecifiedTIFFKeyDirectory (fileInfoPtr, 273, &tiffFileDirectory))
		{
		if (tiffFileDirectory.count == 1)
			numberHeaderBytes = tiffFileDirectory.value;

		}	// end "if (GetSpecifiedTIFFKeyDirectory (fileInfoPtr, 273, ..."
		
	return (numberHeaderBytes);
	
}	// end "GetTIFFNumberHeaderBytes"


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListGeoTiffTextDescriptionParameters
//
//	Software purpose:	The purpose of this routine is to list the information for the
//							ascii Geo Key Parameters.
//
//	Parameters in:		
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			ListTiffTextDescriptionParameters in SGeoTIFF.cpp
//
//	Coded By:			Larry L. Biehl			Date: 07/12/2011
//	Revised By:			Larry L. Biehl			Date: 07/12/2011

Boolean ListGeoTiffTextDescriptionParameters (
				FileInfoPtr							fileInfoPtr,
				TIFF_IFD_Ptr						imageFileDirectoryPtr,
				TIFF_IFD_Ptr						geoAsciiStringPtr,
				Boolean*								addBlankLineFlagPtr)

{	
	char									savedChar,
											textString[1024];
	
	SInt64								savedPosOff;
											
	CMFileStream*						fileStreamPtr;
	UInt16*								geoKeyRecordPtr;
				
	UInt32								count,
											i,
											index,
											recordIndex;
	
	UInt16								geoTagInformation[100];
	
	SInt16								errCode,
											stringLength,
											textStringLength;
	
	Boolean								continueFlag = TRUE,
											stringListedFlag;
	
	
	if (imageFileDirectoryPtr->type == kTIFFShort && imageFileDirectoryPtr->count > 0)
		{																	
				// Get ascii information in case it will be needed later.
					
		GetGeoASCIIStringFromGeoTIFF (fileInfoPtr,
													geoAsciiStringPtr,
													textString,
													1023);
																								
		fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
			
		errCode = MGetMarker (fileStreamPtr, 
											&savedPosOff,
											kErrorMessages);
			
		if (errCode == noErr)
			errCode = MSetMarker (fileStreamPtr, 
												fsFromStart, 
												imageFileDirectoryPtr->value,
												kErrorMessages);
												
		count = MIN (imageFileDirectoryPtr->count, 100);			
		count *= 2;
		if (errCode == noErr)
			errCode = MReadData (fileStreamPtr, 
												&count, 
												(char*)&geoTagInformation,
												kErrorMessages); 
												
		if (errCode == noErr)
			{
					// Convert to number of 2 byte integers.
						
			count /= 2;
			
					//  Convert to number of geo key records.
					
			count /= 4;
			
			index = 0;		
			for (recordIndex=0; recordIndex<count; recordIndex++)
				{
				geoKeyRecordPtr = &geoTagInformation[index];
				for (i=0; i<4; i++, index++)
							// Swap the bytes if needed.
					geoTagInformation[index] = 
									(UInt16)GetShortIntValue ((char*)&geoTagInformation[index]);
			
					// The first geo key record is the version and number of geo keys.
					// We will skip it.
					
				if (recordIndex == 0)
					geoKeyRecordPtr[0] = 0;
					
				stringListedFlag = FALSE;
									
				switch (geoKeyRecordPtr[0])
					{
					case 1026:	// GTCitationGeoKey
						textStringLength = MIN (geoKeyRecordPtr[2], 1023);
						stringLength = sprintf ((char*)gTextString,
														"    GeoTIFF GTCitationGeoKey: ");						
						continueFlag = ListString ((char*)gTextString,  
															stringLength,
															gOutputTextH);
								
								// Save the character for Windows version, since a terminator
								// character is put into this location.

						savedChar = textString[geoKeyRecordPtr[3]+textStringLength];
						
						continueFlag = ListString (
												&textString[geoKeyRecordPtr[3]], 
												textStringLength, 
												gOutputTextH);

						textString[geoKeyRecordPtr[3]+textStringLength] = savedChar;
						
						stringListedFlag = TRUE;
						break;
													
					case 2049:	// GeogCitationGeoKey
						textStringLength = MIN (geoKeyRecordPtr[2], 1023);
						stringLength = sprintf ((char*)gTextString,
														"    GeoTIFF GeogCitationGeoKey: ");						
						continueFlag = ListString ((char*)gTextString,  
															stringLength,
															gOutputTextH);
								
								// Save the character for Windows version, since a terminator
								// character is put into this location.

						savedChar = textString[geoKeyRecordPtr[3]+textStringLength];
															
						continueFlag = ListString (
												&textString[geoKeyRecordPtr[3]], 
												textStringLength, 
												gOutputTextH);

						textString[geoKeyRecordPtr[3]+textStringLength] = savedChar;
						
						stringListedFlag = TRUE;
						break;
													
					case 3073:	// PCSCitationGeoKey
						textStringLength = MIN (geoKeyRecordPtr[2], 1023);
						stringLength = sprintf ((char*)gTextString,
															"    GeoTIFF PCSCitationGeoKey: ");						
						continueFlag = ListString ((char*)gTextString,  
															stringLength,
															gOutputTextH);
															
						continueFlag = ListString (
												&textString[geoKeyRecordPtr[3]], 
												textStringLength, 
												gOutputTextH);
						
						stringListedFlag = TRUE;
						break;
												
					}	// end "switch (geoKeyRecordPtr[0])"
					
				if (stringListedFlag && continueFlag)
					{
					continueFlag = ListString (
											(char*)gEndOfLine, 
											gNumberOfEndOfLineCharacters, 
											gOutputTextH);		
											
					*addBlankLineFlagPtr = TRUE;
					
					}	// end "if (stringListedFlag && continueFlag)"
											
				if (!continueFlag || geoKeyRecordPtr[0] > 3073)
					break;
									
				}	// end "for (recordIndex=0; recordIndex<count; recordIndex++)"
			
			}	// end "if (errCode == noErr)"
				
		if (errCode == noErr)
			errCode = MSetMarker (fileStreamPtr, 
											fsFromStart, 
											savedPosOff,
											kErrorMessages);
			
		}	// end "if (imageFileDirectoryPtr->type == kTIFFShort && ..."
		
	return (continueFlag);
		
}	// end "ListGeoTiffTextDescriptionParameters"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListTiffTextDescriptionParameters
//
//	Software purpose:	This procedure lists the text information for those tags
//							which might contain them. This include tags 270, 1026, 2049 and
//							3073.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:			ListDescriptionInformation in SOther.cpp
//
//	Coded By:			Larry L. Biehl			Date: 07/12/2011
//	Revised By:			Larry L. Biehl			Date: 08/30/2011

Boolean ListTiffTextDescriptionParameters (
				FileInfoPtr 						fileInfoPtr)

{
	char									headerRecordPtr[512],
											textString[1024];
	
	TIFFImageFileDirectory			geoAsciiParamsTag,
											geoKeyDirectoryTag,
											imageFileDirectory;
	
	CMFileStream*						fileStreamPtr;
										
	UInt32								ifdOffset;
										
	UInt32								count;
	
	SInt16								errCode,
											item,
											numberEntries,
											stringLength,
											textStringLength;
											
	Boolean								addBlankLineFlag = FALSE,
											continueFlag = TRUE,
											stringListedFlag;
	
								
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
	
			// Set the global swap bytes flag to reflect the data file being read.													
			
	gSwapBytesFlag = fileInfoPtr->swapHeaderBytesFlag;
	
			// Make certain that we are at the beginning of the file.
	
	ifdOffset = 4;
	errCode = MSetMarker (fileStreamPtr, fsFromStart, ifdOffset, kErrorMessages);
	
			// Read bytes 4-7 bytes of the data.
	
	count = 4;
	if (errCode == noErr)
		errCode = MReadData (fileStreamPtr, &count, headerRecordPtr, kNoErrorMessages); 
				
	if (errCode == noErr)
		{
				// Get first Image File Directory.											
			
		BlockMoveData (headerRecordPtr, (char*)&ifdOffset, 4);
		ifdOffset = (UInt32)GetLongIntValue ((char*)&ifdOffset);

				// Initialize some tags that will be saved.
				
		geoAsciiParamsTag.tag = 0;
		geoKeyDirectoryTag.tag = 0;	
				
		errCode = MSetMarker (fileStreamPtr, 
										fsFromStart, 
										(SInt64)ifdOffset,
										kErrorMessages);
			
				// Read in the first image file directory.							
		
		if (errCode == noErr)
			{
			count = 2;
			errCode = MReadData (fileStreamPtr, 
											&count, 
											&numberEntries,
											kErrorMessages);
				
			}	// end "if (errCode == noErr)" 
			
		if (errCode == noErr)
			{
			numberEntries = GetShortIntValue ((char*)&numberEntries);
			
			for (item=1; item<=numberEntries; item++)
				{
				errCode = GetTiffEntry (fileStreamPtr,
													headerRecordPtr,
													&imageFileDirectory);
					
				stringListedFlag = FALSE;
				switch (imageFileDirectory.tag)
					{								
					case 270:	// Image Description
						textStringLength = (SInt16)MIN (imageFileDirectory.count, 1023);
						GetTIFFASCIIParameters (fileStreamPtr,
														&imageFileDirectory,
														textString,
														textStringLength);
														
						stringLength = sprintf ((char*)gTextString,
														"    Image Description: ");						
						continueFlag = ListString ((char*)gTextString,  
															stringLength,
															gOutputTextH);
						
						continueFlag = ListString (textString,
																textStringLength,
																gOutputTextH);
						
						stringListedFlag = TRUE;
						break;
													
					case 33432:	// Copyright
						textStringLength = (SInt16)MIN (imageFileDirectory.count, 1023);
						GetTIFFASCIIParameters (fileStreamPtr,
														&imageFileDirectory,
														textString,
														textStringLength);
														
						stringLength = sprintf ((char*)gTextString,
														"    Copyright: ");						
						continueFlag = ListString ((char*)gTextString,  
															stringLength,
															gOutputTextH);
						
						continueFlag = ListString (textString,
																textStringLength,
																gOutputTextH);
						
						stringListedFlag = TRUE;
						break;
													
					case 34735:	// Geo Key Directory
						geoKeyDirectoryTag = imageFileDirectory;
						break;
						
					case 34737:	// Geo ASCII Parameters
						geoAsciiParamsTag = imageFileDirectory;
						break;
						
					default:
						errCode = noErr;
						break;
						
					}	// end "switch (imageFileDirectory.tag)" 
					
				if (stringListedFlag && continueFlag)
					{
					continueFlag = ListString ((char*)gEndOfLine,
														gNumberOfEndOfLineCharacters,
														gOutputTextH);		
					
					addBlankLineFlag = TRUE;
					
					}	// end "if (stringListedFlag && continueFlag)"
					
				if (imageFileDirectory.tag == 0 || errCode != noErr || !continueFlag)
					break;
					
				}	// end "for (item=1, item<=numberEntries, item++)" 
				
			if (geoKeyDirectoryTag.tag == 34735 && errCode == noErr)
				continueFlag = ListGeoTiffTextDescriptionParameters (fileInfoPtr,
																						&geoKeyDirectoryTag,
																						&geoAsciiParamsTag,
																						&addBlankLineFlag);
																					
			}	// end "if (errCode == noErr)"
					
		if (continueFlag && addBlankLineFlag)
			continueFlag = ListString ((char*)gEndOfLine,
													gNumberOfEndOfLineCharacters,
													gOutputTextH);	
		
		}	// end "if (errCode == noErr)"	
		
			// Reset the global swap byte flag
				
	gSwapBytesFlag = FALSE;
		
	return (continueFlag);
	
}	// end "ListTiffTextDescriptionParameters"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 LoadHierarchalFileStructure
//
//	Software purpose:	This loads the hierarchal file structure based on the input
//							parameters.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:			ReadGDALHeaderInformation in SGDALInterface.cpp
//							CheckRowsPerStrip in SGeoTIFF.cpp
//							
//	Coded By:			Larry L. Biehl			Date: 01/26/2007
//	Revised By:			Larry L. Biehl			Date: 02/26/2018

SInt16 LoadHierarchalFileStructure (
				FileInfoPtr 						fileInfoPtr,
				UInt32								numberBlocks,
				UInt32*								blockStartsPtr,
				UInt32*								blockByteCountsPtr,
				UInt32								numberLinesPerStrip,
				UInt32								bytesPerStrip,
				UInt32								lastBytesPerStrip,
				UInt32								bytesPerStripForAllChannels)

{
	BlockFormatPtr						blockFormatPtr;
	HierarchalFileFormatPtr			hfaPtr;
	Ptr									ptr;
	
	UInt32								blocksPerChannel,
											index,
											lastChannelBlock,
											minimumBlockStart;
											
	SInt16								returnCode;
	
	Boolean								blockStartsOKFlag;
	
	
	returnCode = -8;
	blockStartsOKFlag = TRUE;

			// Get the storage for the heirarchal information.
			
	fileInfoPtr->hfaHandle = InitializeHierarchalFileStructure (NULL,
																					fileInfoPtr,
																					numberBlocks);
	
			// Get pointer to the hfa structure.
				
	hfaPtr = (HierarchalFileFormatPtr)GetHandlePointer (fileInfoPtr->hfaHandle);
	
	if (hfaPtr != NULL)
		{
			
		fileInfoPtr->blockedFlag = TRUE;
		fileInfoPtr->nonContiguousStripsFlag = TRUE;
		
		blocksPerChannel = (fileInfoPtr->numberLines + numberLinesPerStrip - 1)/
																					numberLinesPerStrip;
																					
		lastChannelBlock = blocksPerChannel;
															
		for (index=0; index<fileInfoPtr->numberChannels; index++)
			{				
			if (fileInfoPtr->bandInterleave != kBSQ)
				hfaPtr[index].layerOffsetBytes = blockStartsPtr[index];
																							
			hfaPtr[index].blockHeight = numberLinesPerStrip;
			hfaPtr[index].blocksPerChannel = blocksPerChannel;
			
					// Block width needs to represent just number of columns in the block.
			hfaPtr[index].blockWidth = fileInfoPtr->numberColumns;
			
			hfaPtr[index].numberBlockWidths = 1;
			
			hfaPtr[index].blockSize = bytesPerStrip;
																	
			if (fileInfoPtr->bandInterleave == kBSQ)
				hfaPtr[index].blockOffset = bytesPerStrip;
			
			else	// fileInfoPtr->bandInterleave != kBSQ
				hfaPtr[index].blockOffset = bytesPerStripForAllChannels;
			
			}	// end "for (index=0; index<fileInfoPtr->numberChannels; index++)"
		
		if (fileInfoPtr->gdalDataSetH != NULL)
			{
					// The GDAL interface routines will force the data to be continuous if not
					// already.
					
			fileInfoPtr->nonContiguousStripsFlag = FALSE;
			minimumBlockStart = 0;
			
			}	// end "if (fileInfoPtr->gdalDataSetH != NULL)"
						
		else if (blockStartsPtr != NULL && blockByteCountsPtr != NULL)
			{				
			ptr = (Ptr)hfaPtr;
			blockFormatPtr = (BlockFormatPtr)&ptr[fileInfoPtr->blockFormatIndex];
		
			minimumBlockStart = blockStartsPtr[0];
													
			for (index=0; index<numberBlocks; index++)
				{				
				minimumBlockStart = MIN (minimumBlockStart, blockStartsPtr[index]);
													
				blockFormatPtr[index].blockOffsetBytes = blockStartsPtr[index];
				blockFormatPtr[index].blockSize = blockByteCountsPtr[index];
					
				}	// end "for (index=0; index<numberBlocks; index++)"
				
			}	// end "else fileInfoPtr->gdalDataSetH == NULL && blockStartsPtr != NULL)"
			
		else	// blockStartsPtr == NULL || blockByteCountsPtr == NULL
			{
			blockStartsOKFlag = FALSE;
			minimumBlockStart = 0;
			
			}	// end "else blockStartsPtr == NULL || blockByteCountsPtr == NULL"
			
				// Set the number of header bytes to the minimum block start. This
				// will allow for proper calculation of the minimum file size based
				// on the image file description parameters.
				// Note that GDAL interface routines will set numberHeaderBytes if needed.
					
		fileInfoPtr->numberHeaderBytes = minimumBlockStart;
		
		CheckAndUnlockHandle (fileInfoPtr->hfaHandle);
		fileInfoPtr->hfaPtr = NULL;
		fileInfoPtr->blockFormatPtr = NULL;
		
		if (blockStartsOKFlag)
			returnCode = noErr;
															
		}	// end "if (hfaPtr != NULL)"
		
	return (returnCode);
	
}	// end "LoadHierarchalFileStructure"


#if use_multispec_tiffcode
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 ReadTIFFHeader
//
//	Software purpose:	This routine reads the file specifications from
//							a TIFF formatted file.
//							returnCode errors:
//								= -1: The number of bits for each TIFF channel was not the same
//								= -2: TIFF images is in a compressed format.
//								= -3: MultiSpec cannot open this type of TIFF file.
//								= -4:	(Used to be error for TIFF images with discontiguous strips. Not used now)
//								= -5: The number of colors in the TIFF color table was not 256
//								= -6: MultiSpec cannot handle 1-bit TIFF images.
//								= -7: MultiSpec cannot handle tiled TIFF images.
//								= -8: There was an inconsistency in reading the TIFF image.
//								= -9: (Use to be error for 32-bit images. Not used now.)
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/15/1994
//	Revised By:			Larry L. Biehl			Date: 04/29/2016

SInt16 ReadTIFFHeader (
				FileInfoPtr 						fileInfoPtr, 
				char*									headerRecordPtr,
				SInt16								formatOnlyCode)

{
	SInt64								savedPosOff;

	TIFFImageFileDirectory			geoAsciiParamsTag,
											geoDoubleParamsTag,
											geoKeyDirectoryTag,
											imageFileDirectory;
	
	CMFileStream*						fileStreamPtr;
	
	unsigned char						charKeyCode[4];
	
	ldiv_t 								ratio;
										
	SInt32								bytesToRead,
											index,
											numberControlPoints,
											numberPostLineBytes,
											numberStripByteCounts;
										
	UInt32								count,
											ifdOffset,
											numberBlockWidths,
											numberStripOffsets,
											numberTiles,
											rowsPerStrip,
											stripByteCountVector,
											stripOffsetVector,
											tempOffset,
											tiffTileByteCount,
											tileByteCount,
											tileLength,
											tileOffsetVector,
											tileWidth;
	
	SInt16								errCode,
											fileType,
											item,
											numberEntries,
											numberBits,
											returnCode,
											sampleFormat,
											savedNumberBits,
											savedSampleFormat,
											stringCompare,
											stripByteCountType,
											stripOffsetType;
											
	UInt16								rasterTypeGeoKey;
	
	Boolean								loadMapProjectionInfoFlag,
											checkRowsPerStripFlag,
											modelTransformationTag,
											notDoneFlag,
											pixelScaleSetFlag;
										
		
			// Initialize local variables.													
	
	fileType = 0;		
	returnCode = 1;
	numberPostLineBytes = 0;
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
	
	numberTiles = 0;
	numberStripOffsets = 0;
	tileByteCount = 0;
	tiffTileByteCount = 0;
	rowsPerStrip = UInt32_MAX;
	
	numberStripByteCounts = 0;
		
	if (fileInfoPtr != NULL && fileStreamPtr != NULL)		
		{
		charKeyCode[0] = 0x4d;
		charKeyCode[1] = 0x4d;
		charKeyCode[2] = 0x00;
		charKeyCode[3] = 0x2a;		                                                          
		stringCompare = (SInt16)strncmp ((char*)headerRecordPtr, (CharPtr)charKeyCode, 4);
		
		if (stringCompare != 0)
			{              
			charKeyCode[0] = 0x49;
			charKeyCode[1] = 0x49;
			charKeyCode[2] = 0x2a;
			charKeyCode[3] = 0x00;             
			stringCompare = (SInt16)strncmp ((char*)headerRecordPtr, (CharPtr)charKeyCode, 4);
										
			if (stringCompare == 0)
				{
				fileInfoPtr->swapBytesFlag = gBigEndianFlag;
				fileInfoPtr->swapHeaderBytesFlag = gBigEndianFlag;
				
				}	// end "if (stringCompare == 0)"
				
			}	// end "if (stringCompare != 0)"
			
		else	// stringCompare == 0  
			{
			fileInfoPtr->swapBytesFlag = !gBigEndianFlag;
			fileInfoPtr->swapHeaderBytesFlag = !gBigEndianFlag;
			
			}	// end "else stringCompare == 0"
										
		if (stringCompare == 0)
			fileType = kTIFFType;
				
		}	// end "if (fileInfoPtr != NULL && fileStreamPtr != NULL)"
		
	if (fileType != 0)
		{
		if (formatOnlyCode != kLoadHeader)
			{
			fileInfoPtr->format = fileType;
			
					// For now we will not check whether the file is thematic
					// or multispectral
					
			if (formatOnlyCode == kMultiFiles)
				fileInfoPtr->thematicType = FALSE;
			
			else if (formatOnlyCode == kThematicFiles)
				fileInfoPtr->thematicType = TRUE;
				
																			return (noErr);
																								
			}	// end "if (formatOnlyCode != kLoadHeader)"
			
		returnCode = noErr;
		
		numberControlPoints = 0;
		
				// Signify that file is of "TIFF" type. 									
				
		fileInfoPtr->format = kTIFFType;
		fileInfoPtr->ancillaryInfoformat = kTIFFType;
		
				// Make the default band interleave format BIS.
				
		fileInfoPtr->bandInterleave = kBIS;
	
				// Get the needed parameters.													
				
		gSwapBytesFlag = fileInfoPtr->swapBytesFlag;
		
				// If map projection information is already loaded, do not load it
				// again.  Was done with gdal routines which are more complete.
			
		loadMapProjectionInfoFlag = TRUE;
		if (fileInfoPtr->mapProjectionHandle != NULL)
			loadMapProjectionInfoFlag = FALSE;
				
				// Set default as RasterPixelIsArea. The geo tie point is for the upper
				// left of the pixel.
				
		rasterTypeGeoKey = 1;
				
				// Get first Image File Directory.											
				
		BlockMoveData ((char*)&headerRecordPtr[4], (char*)&ifdOffset, 4);
		ifdOffset = (UInt32)GetLongIntValue ((char*)&ifdOffset);
			
		checkRowsPerStripFlag = FALSE;
		modelTransformationTag = FALSE;
		pixelScaleSetFlag = FALSE;
		notDoneFlag = TRUE;
		while (notDoneFlag)
			{
					// Initialize some tags that will be saved.
					
			geoAsciiParamsTag.tag = 0;
			geoDoubleParamsTag.tag = 0;
			geoKeyDirectoryTag.tag = 0;	
					
			errCode = MSetMarker (fileStreamPtr, 
											fsFromStart, 
											ifdOffset,
											kErrorMessages);
				
					// Read in the first image file directory.							
			
			if (errCode == noErr)
				{
				count = 2;
				errCode = MReadData (fileStreamPtr, 
												&count, 
												&numberEntries,
												kErrorMessages);
					
				}	// end "if (errCode == noErr)" 
				
			if (errCode == noErr)
				{
      		numberEntries = GetShortIntValue ((char*)&numberEntries);
				
				for (item=1; item<=numberEntries; item++)
					{
					errCode = GetTiffEntry (fileStreamPtr,
													headerRecordPtr,
													&imageFileDirectory);
						
					switch (imageFileDirectory.tag)
						{
						case 256:	// ImageWidth 
							fileInfoPtr->numberColumns = imageFileDirectory.value;
							break;
							
						case 257:	// ImageLength 
							fileInfoPtr->numberLines = imageFileDirectory.value;
							break;
							
						case 258:	// BitsPerSample 
							if (imageFileDirectory.type != kTIFFShort)
								{
								returnCode = -1;
								break;
								
								}	// end "imageFileDirectory.type != kTIFFShort" 

							if (imageFileDirectory.count == 1)
								{
								savedNumberBits = (SInt16)imageFileDirectory.value;
								numberBits = savedNumberBits;

								}	// end "if (imageFileDirectory.count == 1)"

							else if (imageFileDirectory.count == 2)
								{
								savedNumberBits = (SInt16)(imageFileDirectory.value>>16);
								numberBits = (SInt16)(imageFileDirectory.value & 0x0000ffff);

								}	// end "else if (imageFileDirectory.count == 2)"
								
							else	// imageFileDirectory.count > 2 
								{
										// Make certain that the number of bits is		
										// the same for all 'channels'.						
										
								errCode = MGetMarker (fileStreamPtr, 
																&savedPosOff,
																kErrorMessages);
								
								if (errCode == noErr)
									errCode = MSetMarker (fileStreamPtr, 
																	fsFromStart, 
																	imageFileDirectory.value,
																	kErrorMessages);
								
								for (index=1; index<=(SInt32)imageFileDirectory.count; index++)
									{
									count = 2;
									if (errCode == noErr)
										errCode = MReadData (fileStreamPtr, 
																		&count, 
																		headerRecordPtr,
																		kErrorMessages);
																
									BlockMoveData (headerRecordPtr, (char*)&numberBits, 2);
													
      							numberBits = GetShortIntValue ((char*)&numberBits);
											
									if (index == 1)
										savedNumberBits = numberBits;
										
									if (errCode != noErr)
										break;
										
									if (savedNumberBits != numberBits)
										break;
										
									}	// end "for (index=1, ..." 
										
								if (errCode == noErr)
									errCode = MSetMarker (fileStreamPtr, 
																	fsFromStart, 
																	savedPosOff,
																	kErrorMessages);
								
								}	// end "else imageFileDirectory.count > 2" 
										
							if (savedNumberBits != numberBits)
								returnCode = -1;
										
							if (savedNumberBits < 8)
								returnCode = -6;
							
									// The rest of the checks will occur at the end so
									// that we know if the data is integer or floating point.
							break;
								
						case 259:	// Compression 
							if (imageFileDirectory.value != 1)
								returnCode = -2;
							break;
							
						case 262:	// PhotoInterp 
							if (imageFileDirectory.value == 0 || 
															imageFileDirectory.value == 1)
								fileInfoPtr->thematicType = FALSE;
								
							else if (imageFileDirectory.value == 2)
								fileInfoPtr->thematicType = FALSE;
								
							else if (imageFileDirectory.value == 3)
								{
								fileInfoPtr->thematicType = TRUE;
								fileInfoPtr->numberClasses = 256;
								
								}	// end "imageFileDirectory.value == 3" 
								
							else if (imageFileDirectory.value == 8)
								fileInfoPtr->thematicType = FALSE;
									
							else	// imageFileDirectory.value >= 4 
								returnCode = -3;
							break;
								
						case 270:	// Image Description
									// Used when image description is listed
							break;
							
						case 273:	// StripOffsets 
							if (imageFileDirectory.count == 1)
								fileInfoPtr->numberHeaderBytes = imageFileDirectory.value;
																	
							else	// imageFileDirectory.count != 1 
								{
								numberStripOffsets = imageFileDirectory.count;
								stripOffsetVector = imageFileDirectory.value;
								stripOffsetType = imageFileDirectory.type;
								
								checkRowsPerStripFlag = TRUE;
								
								}	// end "else imageFileDirectory.count != 1"
							break;
								
						case 274:	// Orientation (Not used)
							break;
							
						case 277:	// SamplesPerPixel 
							fileInfoPtr->numberChannels = (UInt16)imageFileDirectory.value;
							break;
							
						case 278:	// RowsPerStrip 
							rowsPerStrip = imageFileDirectory.value;
							if (imageFileDirectory.value < fileInfoPtr->numberLines)
								checkRowsPerStripFlag = TRUE;
							break;
							
						case 279:	// StripByteCounts
							numberStripByteCounts = (SInt32)imageFileDirectory.count;
							stripByteCountVector = imageFileDirectory.value;
							stripByteCountType = imageFileDirectory.type;
							break;
							
						case 282:	// XResolution (Not used) 
							break;
							
						case 283:	// YResolution (Not used) 
							break;
							
						case 284:	// PlanarConfiguration 
							if (imageFileDirectory.value == 1)
								fileInfoPtr->bandInterleave = kBIS;
								
							else if (imageFileDirectory.value == 2)
								fileInfoPtr->bandInterleave = kBSQ;
								
							else	// imageFileDirectory.value != 1 or 2 
								returnCode = -3;
							break;
							
						case 296:	// Resolution Unit (Not used) 
							break;
							
						case 306:	// Date Time (Not used) 
							break;
							
						case 320:	// ColorMap 
							if (imageFileDirectory.count == 768)
								{
								fileInfoPtr->colorTableOffset = imageFileDirectory.value;
								
								if (imageFileDirectory.type == kTIFFShort)
									{
									fileInfoPtr->colorTableValueBytes = 2;
									fileInfoPtr->swapColorTableBytes = gSwapBytesFlag;
									
									}	// end "if (imageFileDirectory.type ..." 
								
								}	// end "if (imageFileDirectory.count == 768)" 
																	
							else	// imageFileDirectory.count != 768 
								returnCode = -5;
							break;
							
						case 322:	// Tile width
							tileWidth = imageFileDirectory.value;				
							//returnCode = -7;
							break;
							
						case 323:	// Tile length	
							tileLength = imageFileDirectory.value;			
							//returnCode = -7;
							break;
							
						case 324:	// Tile offsets
						
									// Tiles will work as long as they are contiguous. To
									// allow non-contigous tiles to work. Another parameter
									// will have to be added to the hfa structure to save
									// the tile line start offsets

							errCode = MGetMarker (fileStreamPtr, 
															&savedPosOff,
															kErrorMessages);
							
							if (errCode == noErr)
								errCode = MSetMarker (fileStreamPtr, 
																fsFromStart, 
																imageFileDirectory.value,
																kErrorMessages);
														
							numberTiles = imageFileDirectory.count;
							bytesToRead = 4;
							
							tileOffsetVector = 0;
							tileByteCount = tileWidth * tileLength;
							for (index=1; index<=(SInt32)numberTiles; index++)
								{
								count = (UInt32)bytesToRead;
								tempOffset = 0;
								
								if (errCode == noErr)
									errCode = MReadData (fileStreamPtr, 
																	&count, 
																	&tempOffset,
																	kErrorMessages);
													
								if (errCode == noErr)
									tempOffset = (UInt32)GetLongIntValue ((char*)&tempOffset);
									
								if (errCode != noErr)
									break;										

								if (index == 1)
									{
									tileOffsetVector = tempOffset;
									fileInfoPtr->numberHeaderBytes = tempOffset;
									
									}	// end "if (index == 1)"
									
								else if (index == 2)
									{
									tileByteCount = tempOffset - tileOffsetVector;
									tileOffsetVector = tempOffset;
									
									}	// end "if (index == 1)"
									
								else // index > 2
									{
									tileOffsetVector += tileByteCount;
									if (tileOffsetVector != tempOffset)
										{
										tileOffsetVector = tempOffset;
										returnCode = -7;
										break;
										
										}	// end "if (tileOffsetVector != tempOffset)"
									
									}	// end "else index > 2"
									
								}	// end "for (index=1; index<=numberTiles; index++)"
									
							if (errCode == noErr)
								errCode = MSetMarker (fileStreamPtr, 
																fsFromStart, 
																savedPosOff,
																kErrorMessages);
																	
							//returnCode = -7;
							break;
							
						case 325:	// Tile byte counts
							tiffTileByteCount = imageFileDirectory.value;	
							//returnCode = -7;
							break;
							
						case 338:	// Extra Samples (not used)
							break;
							
						case 339:	// Sample Format
							if (imageFileDirectory.type != kTIFFShort)
								{
								returnCode = -1;
								break;
								
								}	// end "imageFileDirectory.type != kTIFFShort" 

							if (imageFileDirectory.count == 1)
								{
								savedSampleFormat = (SInt16)imageFileDirectory.value;
								sampleFormat = savedSampleFormat;

								}	// end "if (imageFileDirectory.count == 1)"

							else if (imageFileDirectory.count == 2)
								{
								savedSampleFormat = (SInt16)(imageFileDirectory.value>>16);
								sampleFormat = (SInt16)(imageFileDirectory.value & 0x0000ffff);

								}	// end "else if (imageFileDirectory.count == 2)"
								
							else	// imageFileDirectory.count > 2 
								{
										// Make certain that the format is the same for all of
										// the channels.						
										
								errCode = MGetMarker (fileStreamPtr, 
																&savedPosOff,
																kErrorMessages);
								
								if (errCode == noErr)
									errCode = MSetMarker (fileStreamPtr, 
																	fsFromStart, 
																	imageFileDirectory.value,
																	kErrorMessages);
								
								for (index=1; index<=(SInt32)imageFileDirectory.count; index++)
									{
									count = 2;
									
									if (errCode == noErr)
										errCode = MReadData (fileStreamPtr, 
																		&count, 
																		headerRecordPtr,
																		kErrorMessages);
																
									BlockMoveData (headerRecordPtr, 
														(char*)&sampleFormat, 
														2);
													
      							sampleFormat = GetShortIntValue ((char*)&sampleFormat);
											
									if (index == 1)
										savedSampleFormat = sampleFormat;
										
									if (errCode != noErr)
										break;

									if (savedSampleFormat != sampleFormat)
										break;
										
									}	// end "for (index=1, ..." 
										
								if (errCode == noErr)
									errCode = MSetMarker (fileStreamPtr, 
																	fsFromStart, 
																	savedPosOff,
																	kErrorMessages);
								
								}	// end "else imageFileDirectory.count > 1" 
										
							if (savedSampleFormat != sampleFormat)
								returnCode = -1;
										
							if (savedSampleFormat == 3)	//IEEE floating point data
								fileInfoPtr->dataTypeCode = kRealType;
										
							if (returnCode == 0 && errCode == noErr)
								{			
								if (savedSampleFormat == 2 || savedSampleFormat == 3)
									fileInfoPtr->signedDataFlag = TRUE;
								
								}	// end "if (returnCode == 0 && ..." 
							break;
							
						case 33432:	// Copyright
									// Used when image description is listed
							break;
							
						case 33550:	// Pixel Scale
							
							if (loadMapProjectionInfoFlag)
								{
								errCode = SetPixelScaleParametersFromGeoTIFF (
																				fileInfoPtr,
																				&imageFileDirectory);
								pixelScaleSetFlag = TRUE;
								
								}	// end "if (loadMapProjectionInfoFlag)"
							break;
							
						case 33922:	// Geo Tie Points

									// Note that tie points need to be reloaded. The upper-left point
									// may be adjusted to represent the center of the pixel later.
									// Do not want this done twice.
							errCode = SetGeoTiePointsFromGeoTIFF (fileInfoPtr,
																				&imageFileDirectory,
																				&numberControlPoints);
							break;
							
						case 34264:	// Model Transformation

									// Note that model transformation parameters need to be reloaded. The 
									// upper-left point may be adjusted to represent the center of the 
									// pixel later. Do not want this adjustment done twice.

							errCode = SetModelTransformationParametersFromGeoTIFF (
																				fileInfoPtr,
																				&imageFileDirectory);
							modelTransformationTag = TRUE;
							break;
							
						case 34735:	// Geo Key Directory
							if (loadMapProjectionInfoFlag)
								geoKeyDirectoryTag = imageFileDirectory;
							break;
							
						case 34736:	// Geo double parameters.
							if (loadMapProjectionInfoFlag)
								geoDoubleParamsTag = imageFileDirectory;
							break;
							
						case 34737:	// Geo ASCII Parameters
							if (loadMapProjectionInfoFlag)
								geoAsciiParamsTag = imageFileDirectory;
							break;
							
						default:
							errCode = noErr;
							break;
							
						}	// end "switch (imageFileDirectory.tag)" 
						
					if (imageFileDirectory.tag == 0 || errCode != noErr)
						break;
						
					if (returnCode < 0)
						break;
						
					}	// end "for (item=1, item<=numberEntries, item++)" 
					
				if (geoKeyDirectoryTag.tag == 34735 && errCode == noErr && returnCode >= 0)
					errCode = GetGeoKeyParameters (fileInfoPtr,
																&geoKeyDirectoryTag,
																&geoDoubleParamsTag,
																&geoAsciiParamsTag,
																&rasterTypeGeoKey);
																
				if (geoAsciiParamsTag.tag == 34737 && errCode == noErr && returnCode >= 0)
					errCode = SetGeoProjectionFromGeoTIFF (fileInfoPtr,
																		&geoAsciiParamsTag);
				
						// This is the rest of the checks for the number of bits.
							
				if (returnCode == 0 && errCode == noErr)
					{					
					fileInfoPtr->numberBytes = 1;
					fileInfoPtr->numberBits = (UInt16)savedNumberBits;
					if (savedNumberBits > 8 && savedNumberBits <= 16)
						fileInfoPtr->numberBytes = 2;
					else if (savedNumberBits > 16 && savedNumberBits <=32)
						fileInfoPtr->numberBytes = 4;
					else if (savedNumberBits > 32 && savedNumberBits <=64)
						fileInfoPtr->numberBytes = 8;
						
							// Force the number of classes to be 0, so that the user to
							// enter the number or have it computed.
						
					if (fileInfoPtr->thematicType && fileInfoPtr->numberBytes == 2)
						{
						fileInfoPtr->numberClasses = 0;
						fileInfoPtr->numberBins = (UInt32)ldexp ((double)1, 16);
						
						}	// end "if (fileInfoPtr->thematicType && fileInfoPtr->numberBytes == 2)"
					
					}	// end "if (returnCode == 0 && ..." 
					
				if (returnCode == noErr && errCode == noErr && checkRowsPerStripFlag)
					returnCode = CheckRowsPerStrip (fileInfoPtr,
																fileStreamPtr,
																numberStripOffsets,
																stripOffsetType,
																stripOffsetVector,
																rowsPerStrip,
																numberStripByteCounts,
																stripByteCountType,
																stripByteCountVector);
					
						// Read the offset to the next Image File Directory	
			
				if (returnCode == noErr && errCode == noErr)
					{
					count = 4;
					errCode = MReadData (fileStreamPtr, 
													&count, 
													&ifdOffset,
													kErrorMessages);
					
					if (errCode == noErr)
						ifdOffset = (UInt32)GetLongIntValue ((char*)&ifdOffset);
						
					}	// end "if (returnCode >= 0 && errCode == noErr)" 
					
						// For now we will ignore other image data in the file.	
					
				notDoneFlag = FALSE;
					
				}	// end "if (errCode == noErr)"
				
			if (errCode != noErr)
				notDoneFlag = FALSE;
					
			}	// end "while (notDoneFlag)" 
				
		if (returnCode < 0)
			{
					// Display an alert if the needed parameters were not 		
					// found in the file.
					
			index = IDS_Alert26 + abs (returnCode);
			if (returnCode == -7)
				index = IDS_Alert90;	
			else if (returnCode == -8)
				index = IDS_Alert120;
			else if (returnCode == -9)
				index = IDS_Alert121;																	
			
			DisplayAlert (kErrorAlertID, 
								kStopAlert, 
								kAlertStrID, 
								(SInt16)index,
								0, 
								NULL);
								
			errCode = eofErr;
			
			}	// end "if (returnCode < 0)" 
		
		if (returnCode == noErr && errCode != noErr)
			returnCode = 1;
			
		if (returnCode == noErr)
			{
					// This was tried out for tiled TIFF images. It has not
					// been completed yet. For now tiled TIFF images are not
					// allowed.
					
			if (numberTiles > 0)
				{
						// Get the storage for the heirarchal information.
						
				fileInfoPtr->hfaHandle = InitializeHierarchalFileStructure (
																			NULL,
																			fileInfoPtr,
																			fileInfoPtr->numberChannels);
																			
				if (fileInfoPtr->hfaHandle == NULL)
					returnCode = 1;
					
				else	// fileInfoPtr->hfaHandle != NULL
					{
							// Get pointer to the hfa structure.
							
					fileInfoPtr->hfaPtr = (HierarchalFileFormatPtr)GetHandlePointer (
																			fileInfoPtr->hfaHandle,
																			kLock);
						
					fileInfoPtr->blockedFlag = TRUE;
															
					fileInfoPtr->hfaPtr->layerOffsetBytes = 
															fileInfoPtr->numberHeaderBytes;
					
					if (fileInfoPtr->bandInterleave != kBIS)
						{											
						fileInfoPtr->bandInterleave = kBNonSQBlocked;
						fileInfoPtr->numberHeaderBytes = 0;
						
						}	// end "if (fileInfoPtr->bandInterleave != kBIS)"
																			
					fileInfoPtr->hfaPtr->blockWidth = tileWidth;
					fileInfoPtr->hfaPtr->blockHeight = tileLength;
					
					fileInfoPtr->hfaPtr->blockSize = tileByteCount;
															
					fileInfoPtr->hfaPtr->blockOffset = tileByteCount;
			
							// Get the number of bytes to skip to go from the start of one
							// line to the start of the next line, taking the tiles into
							// account.
						
					ratio = ldiv ((SInt32)fileInfoPtr->numberColumns, (SInt32)tileWidth);
					numberBlockWidths = (UInt32)ratio.quot;
					if (ratio.rem > 0)
						numberBlockWidths++;
					
					fileInfoPtr->hfaPtr->numberBlockWidths = numberBlockWidths;
					
					}	// end "else fileInfoPtr->hfaHandle != NULL"
				
				}	// end "if (numberTiles > 0)"
				
			CheckAndUnlockHandle (fileInfoPtr->hfaHandle);
			fileInfoPtr->hfaPtr = NULL;
				
			}	// end "if (returnCode == noErr)"
			
				// Get polynomial model to relate line/column to map location if
				// needed and can.
			
		if (numberControlPoints > 0 && 
					GetReferenceSystemCode (fileInfoPtr->mapProjectionHandle) <= 0)
			{
			GetPolynomialModel (fileInfoPtr);
			
					// Update map values for the upper left pixel.  The current values
					// may not represent the map values for pixel (1,1).
					
			UpdateUpperLeftMapValues (fileInfoPtr);
			
			}	// end "if (numberControlPoints > 0 && ..."
				
		if (returnCode == noErr)
			{
			if (fileInfoPtr->numberBytes == 1)
				fileInfoPtr->swapBytesFlag = FALSE;
				
			//fileInfoPtr->numberBins = (UInt32)ldexp ((double)1, fileInfoPtr->numberBits);
				
			if (fileInfoPtr->thematicType)
				{
				fileInfoPtr->bandInterleave = kBIL;
				fileInfoPtr->collapseClassSelection = kCollapseClass;
				
				}	// end "fileInfoPtr->thematicType"
				
			else	// !fileInfoPtr->thematicType
				{
						// If the number of channels is one, then indicate that the band
						// interleave format is BIL. (It doesn't make any difference for
						// one channel data except BIS formatted files are not allowed to
						// be linked. Change the band interleave format to BIL will allow
						// this file to be linked.
						
				if (fileInfoPtr->numberChannels == 1 && 
							fileInfoPtr->numberPostChannelBytes == 0 &&
								!fileInfoPtr->blockedFlag)
					fileInfoPtr->bandInterleave = kBIL;
				
				}	// end "else !fileInfoPtr->thematicType"
			
			if (gGetFileImageType == 0)
				{
						// If this is not a thematic type file but the number of
						// channels is one and there is a default arcview color file
						// available, then assume that the user would like for this
						// to be treated as a thematic image file.
						
				if (!fileInfoPtr->thematicType && 
							fileInfoPtr->numberChannels == 1 &&
									fileInfoPtr->numberBytes == 1)
					{
					if (CheckIfSpecifiedFileExists (fileStreamPtr, (char*)"\0.clr\0"))
						fileInfoPtr->thematicType = TRUE;
					
					}	// end "if (!fileInfoPtr->thematicType && ..."
					
				if (fileInfoPtr->thematicType)
					gGetFileImageType = kThematicImageType;
					
				else	// !fileInfoPtr->thematicType 
					gGetFileImageType = kMultispectralImageType;
					
				}	// end "if (gGetFileImageType == 0)" 
				
					// Check if this is a GeoTIFF file.
					
			if (fileInfoPtr->mapProjectionHandle != NULL)
				{
				fileInfoPtr->format = kGeoTIFFType;
				fileInfoPtr->ancillaryInfoformat = kGeoTIFFType;
				
						// Set geo tie points to reflect useage by MultiSpec.
						// This may not be needed when the tie points come from the
						// transformation tag.  This needs to be verified yet.
						// 8/29/2006
				
				if (!modelTransformationTag)			
					SetGeoTiePointsForRasterPixelType (fileInfoPtr, rasterTypeGeoKey);
				
				}	// end "if (fileInfoPtr->mapProjectionHandle != NULL)"
				
			}	// end "if (returnCode == noErr)" 												
				
		gSwapBytesFlag = FALSE;
		
		}	// end "if (fileInfoPtr != NULL && ..." 
		
	return (returnCode);
	
}	// end "ReadTIFFHeader" 
#endif		// use_multispec_tiffcode


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 SetProjectionInformationFromString
//
//	Software purpose:	The purpose of this routine is to set reference system and
//							projection parameters from the text in the input text string if 
//							the information is not already available in the map projection 
//							structure.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			ReadGDALProjectionInformation in SGDALInterface.cpp
//							SetProjectionInformationFromString2 in SGeoTIFF.cpp
//							SetGeoProjectionFromGeoTIFF in SGeoTIFF.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/17/2012
//	Revised By:			Larry L. Biehl			Date: 02/27/2018

void SetProjectionInformationFromString (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				char*									inputStringPtr)

{
	UInt8									direction,
											zone[256];
	
	UCharPtr								stringPtr;
	
	int									gridZone;
	
	SInt32								fipsNumber;
	
	SInt16								returnCode;
	
	Boolean								setStatePlaneTypeFlag = FALSE;
	
	
	if (mapProjectionInfoPtr != NULL && inputStringPtr != NULL)
		{
		if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode <= kUserDefinedRSCode)
			{	
			if (strstr (inputStringPtr, "UTM") != NULL || 
							strstr (inputStringPtr, "Universal Transverse Mercator") != NULL)
				{
						// Note than an attempt to reset the following to a specific UTM reference
						// system will be done later.
						
				mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = kUTMRSCode;
				
				mapProjectionInfoPtr->gridCoordinate.projectionCode = kTransverseMercatorCode;
				mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kMetersCode;
				
						// Get the zone. Can be labeled as Zone or Zone Number
						
				direction = ' ';
				
				stringPtr = (UCharPtr)strstr ((char*)inputStringPtr, "Zone Number");
				if (stringPtr != NULL)
					returnCode = (SInt16)sscanf ((char*)inputStringPtr, "Zone Number %s", zone);
				
				else if (stringPtr == NULL)
					{
					stringPtr = (UCharPtr)strstr ((char*)inputStringPtr, "Zone");
					
					if (stringPtr != NULL)
						returnCode = (SInt16)sscanf ((char*)stringPtr, "Zone %s", zone);
						
					}	// end "esle if (stringPtr == NULL)"
					
				if (stringPtr != NULL)
					{
					if (returnCode == 1)
						{
						CtoPstring (zone, zone);
							
								// Now get the zone number.
							
						returnCode = (SInt16)sscanf ((char*)&zone[1], 
													"%d",
													&gridZone);
													
						if (returnCode == 1)
							{
									// Find whether it is North or South Zone if not identified
									// already.
													
							sprintf ((char*)gTextString, "%dN", gridZone);
							if (strstr ((char*)zone, (char*)gTextString) != NULL)
								direction = 'N';
							
							if (direction == ' ')
								{
								sprintf ((char*)gTextString, "%d N", gridZone);
								if (strstr ((char*)zone, (char*)gTextString) != NULL)
									direction = 'N';
									
								}	// end "if (direction == ' ')"
							
							if (direction == ' ')
								{
								sprintf ((char*)gTextString, "%dS", gridZone);
								if (strstr ((char*)zone, (char*)gTextString) != NULL)
									direction = 'S';
									
								}	// end "if (direction == ' ')"
							
							if (direction == ' ')
								{
								sprintf ((char*)gTextString, "%d S", gridZone);
								if (strstr ((char*)zone, (char*)gTextString) != NULL)
									direction = 'S';
									
								}	// end "if (direction == ' ')"
							
							if (direction == ' ')
								{
										// Determine North/South from the upper left column origin.
										
								if (mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 <= 0)
									direction = 'S';
									
								else	// ...->planarCoordinate.yMapCoordinate11 > 0
									direction = 'N';
									
								}	// end "if (direction == ' ')"
								
							if (direction == 'S')
								gridZone = -abs (gridZone);
								
							mapProjectionInfoPtr->gridCoordinate.gridZone = (SInt16)gridZone;
							
							}	// end "if (returnCode == 1)"
						
						}	// end "if (returnCode == 1)"

					}	// end "if (stringPtr != NULL)"
				
				}	// if (strstr (inputStringPtr, "UTM") != NULL)
				
			else if (strstr (inputStringPtr, "State Plane") != NULL || 
							strstr (inputStringPtr, "StatePlane") != NULL ||
								strstr (inputStringPtr, "Indiana East") != NULL ||
									strstr (inputStringPtr, "Indiana West") != NULL)
				{
				if (mapProjectionInfoPtr->geodetic.datumCode == kNAD27Code)
					mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = kStatePlaneNAD27RSCode;
					
				else if (mapProjectionInfoPtr->geodetic.datumCode == kNAD83Code)
					mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = kStatePlaneNAD83RSCode;
					
				else	// Set state plane type after trying to read the datum
					setStatePlaneTypeFlag = TRUE;
					
						// Try to get the FIPS number if included.
				
				fipsNumber = 0;		
				if ((stringPtr = (UCharPtr)strstr ((char*)inputStringPtr, "FIPS")) != NULL)
					sscanf ((char*)stringPtr, "FIPS_%ld", &fipsNumber);
					
				if (fipsNumber == 0)
					{
							// Try to get information from the name
					
					if (strstr (inputStringPtr, "Indiana East") != NULL)
						fipsNumber = 1301;
					
					else if (strstr (inputStringPtr, "Indiana West") != NULL)
						fipsNumber = 1302;
					
					}	// end "if (fipsNumber == 0)"
					
				if (fipsNumber > 0 && fipsNumber < 32767)
					mapProjectionInfoPtr->gridCoordinate.gridZone = (SInt16)fipsNumber;
				
				}	// else if (strstr (inputStringPtr, "State Plane") != NULL)
				
			}	// end ""
			
		if (mapProjectionInfoPtr->gridCoordinate.projectionCode == kNotDefinedCode)
			{
			if (strstr (inputStringPtr, "LCC") != NULL)
				{
				mapProjectionInfoPtr->gridCoordinate.projectionCode = kLambertConformalConicCode;
				mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kMetersCode;
				
				}	// else if (strstr (inputStringPtr, "LCC") != NULL)
				
			else if (strstr (inputStringPtr, "LON/LAT") != NULL)
				{
				if (strstr (inputStringPtr, "Degrees") != NULL)
					mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kDecimalDegreesCode;
				
				}	// else if (strstr (string, "LON/LAT") != NULL)
				
			else if (strstr (inputStringPtr, "Geographic") != NULL)
				{
				if (mapProjectionInfoPtr->gridCoordinate.projectionCode == kNotDefinedCode)
					mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = kGeographicRSCode;
				
				if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode == kGeographicRSCode)
					mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kDecimalDegreesCode;
				
				}	// else if (strstr (inputStringPtr, "Geographic") != NULL)
				
			else if (strstr (inputStringPtr, "Krovak") != NULL)
				{
				mapProjectionInfoPtr->gridCoordinate.projectionCode = kKrovakCode;
				mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kMetersCode;
				mapProjectionInfoPtr->geodetic.spheroidCode = kBesselEllipsoidCode;
				
				SetKrovakParameters (mapProjectionInfoPtr);
				
				}	// else if (strstr (inputStringPtr, "Krovak") != NULL)
				
			else if (strstr (inputStringPtr, "LAMBERT_AZ") != NULL)
				{
				mapProjectionInfoPtr->gridCoordinate.projectionCode = kLambertAzimuthalEqualAreaCode;
				mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kMetersCode;
				mapProjectionInfoPtr->geodetic.spheroidCode = kSphereEllipsoidCode;
				
				}	// else if (strstr (inputStringPtr, "Krovak") != NULL)
				
			}	// end "if (...->gridCoordinate.projectionCode == kNotDefinedCode)"
			
		if (mapProjectionInfoPtr->geodetic.datumCode == kNoDatumDefinedCode)
			{
					// Get the horizontal datum
					
			if (strstr (inputStringPtr, "WGS84") != NULL || 
						strstr (inputStringPtr, "WGS 84") != NULL || 
								strstr (inputStringPtr, "WGS-84") != NULL)
				{
				if (strstr (inputStringPtr, "using WGS84") == NULL)
					{
					mapProjectionInfoPtr->geodetic.datumCode = kWGS84Code;
					mapProjectionInfoPtr->geodetic.spheroidCode = kWGS84EllipsoidCode;
					
					}	// end "if (strstr (inputStringPtr, "using WGS84") == NULL)"
				
				}	// end "else if (strstr (inputStringPtr, "WGS84") != NULL || ..."
				
			else if (strstr (inputStringPtr, "NAD27") != NULL || 
								strstr (inputStringPtr, "NAD 27") != NULL || 
										strstr (inputStringPtr, "NAD-27") != NULL)
				{
				mapProjectionInfoPtr->geodetic.datumCode = kNAD27Code;
				mapProjectionInfoPtr->geodetic.spheroidCode = kClarke1866EllipsoidCode;
				
				}	// end "else if (strstr (inputStringPtr, "NAD27") != NULL || ..."
				
			else if (strstr (inputStringPtr, "NAD83") != NULL || 
								strstr (inputStringPtr, "NAD 83") != NULL || 
										strstr (inputStringPtr, "NAD-83") != NULL || 
												strstr (inputStringPtr, "NAD_1983") != NULL)
				{
				mapProjectionInfoPtr->geodetic.datumCode = kNAD83Code;
				mapProjectionInfoPtr->geodetic.spheroidCode = kGRS80EllipsoidCode;
				
				}	// end "else if (strstr (inputStringPtr, "NAD83") != NULL || ..."
			
			}	// end "if (...->geodetic.datumCode == kNoDatumDefinedCode)"
			
		if (mapProjectionInfoPtr->geodetic.spheroidCode == 0)
			{
					// Get the ellipsoid
				
			if (strstr (inputStringPtr, "Clarke, 1866") != NULL)
				mapProjectionInfoPtr->geodetic.spheroidCode = 
																	kClarke1866EllipsoidCode;
				
			else if (strstr (inputStringPtr, "Clarke, 1880") != NULL)
				mapProjectionInfoPtr->geodetic.spheroidCode = 
																	kClarke1880EllipsoidCode;
				
			else if (strstr (inputStringPtr, "GRS_1980") != NULL)
				mapProjectionInfoPtr->geodetic.spheroidCode = 
																	kGRS80EllipsoidCode;
																	
			}	// end "if (...->geodetic.spheroidCode == 0)"
			
		if (mapProjectionInfoPtr->planarCoordinate.mapUnitsCode == kUnknownCode)
			{				
			if (strstr (inputStringPtr, "Feet") != NULL)
				mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kUSSurveyFeetCode;
			
			else if (strstr (inputStringPtr, "Meter") != NULL)
				mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kMetersCode;
			
			}	// end "if (...->planarCoordinate.mapUnitsCode == kUnknownCode)"
			
		if (setStatePlaneTypeFlag)
			{
			if (mapProjectionInfoPtr->geodetic.datumCode == kNAD27Code)
				mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = kStatePlaneNAD27RSCode;
				
			else // default to NAD83 Set
				mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = kStatePlaneNAD83RSCode;
				
			}	// end "if (setStatePlaneTypeFlag)"
			
		if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode == kStatePlaneNAD27RSCode &&
											mapProjectionInfoPtr->geodetic.datumCode == kNAD83Code)
			mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = kStatePlaneNAD83RSCode;
			
				// This is for a special case relative to the 2011-2013 Indiana Ortho Data 
				// to try to get the EPSG value if everything makes sense.
				
		if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode == kStatePlaneNAD83RSCode &&
				mapProjectionInfoPtr->planarCoordinate.mapUnitsCode == kUSSurveyFeetCode)
			{
			if (mapProjectionInfoPtr->gridCoordinate.gridZone == 1301)
				mapProjectionInfoPtr->projectedCSTypeGeoKey = 2965;
				
			else if (mapProjectionInfoPtr->gridCoordinate.gridZone == 1302)
				mapProjectionInfoPtr->projectedCSTypeGeoKey = 2966;
				
			if (mapProjectionInfoPtr->projectedCSTypeGeoKey != 0)
				mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = kByEPSGCodeCode;
			
			}	// end "if (...->gridCoordinate.referenceSystemCode == kStatePlaneNAD83RSCode && ..."
			
		}	// end "if (mapProjectionInfoPtr != NULL && inputStringPtr != NULL)"
	
}	// end "SetProjectionInformationFromString"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetProjectionInformationFromString2
//
//	Software purpose:	The purpose of this routine is to set projection parameters from
//							the text geo ascii string parameter.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/18/2012
//	Revised By:			Larry L. Biehl			Date: 02/18/2012

void SetProjectionInformationFromString2 (
				FileInfoPtr							fileInfoPtr,
				MapProjectionInfoPtr				mapProjectionInfoPtr)

{
										
	char									stringList[1024];
		
												
	GetGeoASCIIStringFromGeoTIFF (fileInfoPtr,
												(char*)stringList,
												1024);
	
	if (stringList[0] != 0)
		SetProjectionInformationFromString (mapProjectionInfoPtr, (char*)stringList);
	
}	// end "GetProjectionInformationFromString2"
  

#if use_multispec_tiffcode
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 SetGeoProjectionFromGeoTIFF
//
//	Software purpose:	The purpose of this routine is to set projection parameters from
//							the text in the geotiff text string if the information was not
//							available in the other geotiff tags.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/04/2000
//	Revised By:			Larry L. Biehl			Date: 04/14/2004

SInt16 SetGeoProjectionFromGeoTIFF (
				FileInfoPtr							fileInfoPtr,
				TIFF_IFD_Ptr						imageFileDirectoryPtr)

{
	char									stringList[1024];
	
	CMFileStream* 						fileStreamPtr;
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	Handle								mapProjectionHandle;
	
	SInt16								errCode = noErr;
	
	
	if (imageFileDirectoryPtr->count > 0 && 
					imageFileDirectoryPtr->type == kTIFFASCII &&
								imageFileDirectoryPtr->count < 1024)
		{
				// ASCII format is expected.
										
		mapProjectionHandle = GetFileMapProjectionHandle (fileInfoPtr);
		
		if (mapProjectionHandle != NULL)
			{ 
			mapProjectionInfoPtr = (MapProjectionInfoPtr)
												GetHandlePointer (mapProjectionHandle,
																			kLock);
							
			fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
			
			errCode = GetTIFFASCIIParameters (fileStreamPtr,
															imageFileDirectoryPtr,
															(char*)stringList,
															(SInt32)imageFileDirectoryPtr->count);
															
			SetProjectionInformationFromString (mapProjectionInfoPtr,
															(char*)stringList);
															
			CheckAndUnlockHandle (mapProjectionHandle); 
			
			}	// end "if (mapProjectionHandle != NULL)"
			
		}	// end "if (imageFileDirectory.count == 3 && ..."
		
	return (errCode);
	
}	// end "SetGeoProjectionFromGeoTIFF"
  


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetGeoTiePointsForRasterPixelType
//
//	Software purpose:	The purpose of this routine is to set the geo tie points to
//							to reflect the type of pixel in the image file. If the pixel
//							type is area, then the geo tie points in the header is for the 
//							upper left of the pixel. If the pixel type is point, then the 
//							geo tie points in the header is for the center of the pixel.
//							MultiSpec assumes the upper left coordinate point is for the
//							center of the upper left pixel.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/23/2002
//	Revised By:			Larry L. Biehl			Date: 02/24/2006

void SetGeoTiePointsForRasterPixelType (
				FileInfoPtr							fileInfoPtr,
				UInt16								rasterTypeGeoKey)

{
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	Handle								mapProjectionHandle;
	
	
	if (rasterTypeGeoKey == 1)
		{
				// If the upper left coordinate information exists, then adjust so the
				// the points reflects the center of the pixel not the upper left of the
				// pixel.
										
		mapProjectionHandle = GetFileMapProjectionHandle (fileInfoPtr);
		
		if (mapProjectionHandle != NULL)
			{ 								
			mapProjectionInfoPtr = (MapProjectionInfoPtr)
												GetHandlePointer (mapProjectionHandle);
	                                                                         
			if (mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 != 0 ||
					 mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 != 0)
				{		 
				mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 +=
							mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize/2;
				
				mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 -=
							mapProjectionInfoPtr->planarCoordinate.verticalPixelSize/2;
							
				mapProjectionInfoPtr->planarCoordinate.xMapOrientationOrigin =
										mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11;
				mapProjectionInfoPtr->planarCoordinate.yMapOrientationOrigin =
										mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11;
							
				}	// end "if (...->planarCoordinate.xMapCoordinate11 ..."
			
			}	// end "if (mapProjectionHandle != NULL)"
			
		}	// end "if (rasterTypeGeoKey == 1)"
	
}	// end "SetGeoTiePointsForRasterPixelType"
  


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 SetGeoTiePointsFromGeoTIFF
//
//	Software purpose:	The purpose of this routine is to set the pixel size parameters
//							using the information in the GEOTIFF tag.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/04/2000
//	Revised By:			Larry L. Biehl			Date: 03/11/2007

SInt16 SetGeoTiePointsFromGeoTIFF (
				FileInfoPtr							fileInfoPtr,
				TIFF_IFD_Ptr						imageFileDirectoryPtr,
				SInt32*								numberControlPointsPtr)

{
	double*								geoTiePointsPtr;
	
	CMFileStream*						fileStreamPtr;
	ControlPointsPtr					controlPointsPtr;
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	Handle								mapProjectionHandle;
	
	UInt32								count,
											geoIndex,
											index,
											numberControlPoints;
	
	SInt16								errCode = noErr;
	
	Boolean								checkLoadControlPointsFlag,
											loadControlPointsFlag;
	
		
				// At least 6 parameters and double format is expected.
	
	if (imageFileDirectoryPtr->count >= 6 && imageFileDirectoryPtr->type == kTIFFDouble)
		{
				// For now allow for count of at least 72 (9 control points) if loading
				// control points into an image.  This is defined when 
				// checkLoadControlPointsFlag is set to TRUE.
				
		checkLoadControlPointsFlag = FALSE;
		
		if (checkLoadControlPointsFlag) 
			count = MAX (imageFileDirectoryPtr->count, 72);
		
		else	// !checkLoadControlPointsFlag
			count = imageFileDirectoryPtr->count;
		
		geoTiePointsPtr = NULL;
										
		mapProjectionHandle = GetFileMapProjectionHandle (fileInfoPtr);
			
		if (mapProjectionHandle != NULL)
			geoTiePointsPtr = (double*)MNewPointer (count * sizeof (double));
		
		if (geoTiePointsPtr != NULL)
			{ 								
			mapProjectionInfoPtr = (MapProjectionInfoPtr)
												GetHandlePointer (mapProjectionHandle,
																			kLock);
		
					// The following lines are currently for loading control points into
					// the Corona data.
					
			loadControlPointsFlag = FALSE;
			count = imageFileDirectoryPtr->count;
			if (checkLoadControlPointsFlag &&
						mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize == 1 && 
								count == 6)
				{
				count = 72;
				loadControlPointsFlag = TRUE;
				
				}	// end "if (checkLoadControlPointsFlag && ..."
			
			fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
			
			errCode = GetTIFFDoubleParameters (
									fileStreamPtr, 
									imageFileDirectoryPtr, 
									geoTiePointsPtr, 
									imageFileDirectoryPtr->count);
									
			if (errCode == noErr)
				{	
				if (loadControlPointsFlag)
					{
					geoTiePointsPtr[0] = 1147;
					geoTiePointsPtr[1] = 933;
					geoTiePointsPtr[3] = -86.60633;
					geoTiePointsPtr[4] = 39.53560;
					
					geoTiePointsPtr[6] = 100026;
					geoTiePointsPtr[7] = 1237;
					geoTiePointsPtr[9] = -83.49399;
					geoTiePointsPtr[10] = 40.12384;
					
					geoTiePointsPtr[12] = 101020;
					geoTiePointsPtr[13] = 7489;
					geoTiePointsPtr[15] = -83.36302;
					geoTiePointsPtr[16] = 39.96659;
					
					geoTiePointsPtr[18] = 1215;
					geoTiePointsPtr[19] = 8068;
					geoTiePointsPtr[21] = -86.58610;
					geoTiePointsPtr[22] = 39.33462;
					
					geoTiePointsPtr[24] = 1410;
					geoTiePointsPtr[25] = 4410;
					geoTiePointsPtr[27] = -86.58870;
					geoTiePointsPtr[28] = 39.44090;
					
					geoTiePointsPtr[30] = 49168;
					geoTiePointsPtr[31] = 1259;
					geoTiePointsPtr[33] = -85.09008;
					geoTiePointsPtr[34] = 39.81425;
					
					geoTiePointsPtr[36] = 49632;
					geoTiePointsPtr[37] = 3597;
					geoTiePointsPtr[39] = -85.06191;
					geoTiePointsPtr[40] = 39.76594;
					
					geoTiePointsPtr[42] = 100456;
					geoTiePointsPtr[43] = 3651;
					geoTiePointsPtr[45] = -83.44250;
					geoTiePointsPtr[46] = 40.06459;
					
					geoTiePointsPtr[48] = 50782;
					geoTiePointsPtr[49] = 7154;
					geoTiePointsPtr[51] = -85.00614;
					geoTiePointsPtr[52] = 39.69308;
					
					geoTiePointsPtr[54] = 22518;
					geoTiePointsPtr[55] = 4371;
					geoTiePointsPtr[57] = -85.83667;
					geoTiePointsPtr[58] = 39.59409;
					
					geoTiePointsPtr[60] = 22947;
					geoTiePointsPtr[61] = 1014;
					geoTiePointsPtr[63] = -85.83877;
					geoTiePointsPtr[64] = 39.68333;
					
					geoTiePointsPtr[66] = 22510;
					geoTiePointsPtr[67] = 7467;
					geoTiePointsPtr[69] = -85.82248;
					geoTiePointsPtr[70] = 39.51889;
						
					}	// end "if (loadControlPointsFlag)"
				                                 
				mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 = geoTiePointsPtr[3];
				mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 = geoTiePointsPtr[4]; 
                                                                      
				mapProjectionInfoPtr->planarCoordinate.xMapOrientationOrigin = 
																							geoTiePointsPtr[3];
				mapProjectionInfoPtr->planarCoordinate.yMapOrientationOrigin = 
																							geoTiePointsPtr[4];
				
						// If there are 3 or more control points, save them in case they will
						// be need to create a polynomial model to relate line/column to 
						//	map location.
						
				numberControlPoints = count/6;
				if (numberControlPoints >= 3)
					{					
							// Get memory for the control points.
							
					fileInfoPtr->controlPointsHandle = MNewHandle (sizeof (ControlPoints) + 
									numberControlPoints * (4*sizeof (double) + sizeof (SInt16)));
									
					controlPointsPtr = (ControlPointsPtr)GetHandlePointer (
																		fileInfoPtr->controlPointsHandle);
					if (controlPointsPtr != NULL)
						controlPointsPtr->count = numberControlPoints;
									
					controlPointsPtr = GetControlPointVectorPointers (
																		fileInfoPtr->controlPointsHandle,
																		kLock);
																		
					if (controlPointsPtr != NULL)
						{			
						geoIndex = 0;
						for (index=0; index<numberControlPoints; index++)
							{
							controlPointsPtr->easting1Ptr[index] = geoTiePointsPtr[geoIndex];
							controlPointsPtr->northing1Ptr[index] = geoTiePointsPtr[geoIndex+1];
							
							controlPointsPtr->easting2Ptr[index] = geoTiePointsPtr[geoIndex+3];
							controlPointsPtr->northing2Ptr[index] = geoTiePointsPtr[geoIndex+4];
							
							controlPointsPtr->statusPtr[index] = 1;
							
							geoIndex += 6;
							
							}	// end "for (index=0; index<numberControlPoints; index++)"
							
						*numberControlPointsPtr = numberControlPoints;
						
						}	// end "if (controlPointsPtr->easting1Ptr != NULL)"
						
					CloseControlPointVectorPointers (fileInfoPtr->controlPointsHandle);
					
					}	// end "if (numberControlPoints >= 3)"
			
				}	// end "if (errcode == noErr)"
				
			CheckAndDisposePtr (geoTiePointsPtr);
				
			CheckAndUnlockHandle (mapProjectionHandle); 
			
			}	// end "if (if (geoTiePointsPtr != NULL)"
			
		}	// end "if (imageFileDirectoryPtr->count >= 6 && ..."
		
	return (errCode);
	
}	// end "SetGeoTiePointsFromGeoTIFF"
  


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 SetModelTransformationParametersFromGeoTIFF
//
//	Software purpose:	The purpose of this routine is to set the model transformation
//							parameters from this tag.  The parameters that will be set
//							are the pixel size and the image orientation.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 08/29/2006
//	Revised By:			Larry L. Biehl			Date: 08/29/2006

SInt16 SetModelTransformationParametersFromGeoTIFF (
				FileInfoPtr							fileInfoPtr,
				TIFF_IFD_Ptr						imageFileDirectoryPtr)

{
	double								modelTransformation[16],
											orientationAngle;
	
	CMFileStream*						fileStreamPtr;
	
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	Handle								mapProjectionHandle;
	
	SInt32								count;
	
	SInt16								errCode = noErr;
	
	
	if (imageFileDirectoryPtr->count >= 16 && imageFileDirectoryPtr->type == kTIFFDouble)
		{
				// At least 2 parameters and double format is expected.
										
		mapProjectionHandle = GetFileMapProjectionHandle (fileInfoPtr);
		
		if (mapProjectionHandle != NULL)
			{ 								
			mapProjectionInfoPtr = (MapProjectionInfoPtr)
												GetHandlePointer (mapProjectionHandle,
																			kLock);
			
			fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
			
			count = MIN (16, (SInt32)imageFileDirectoryPtr->count);
											
			errCode = GetTIFFDoubleParameters (
									fileStreamPtr, 
									imageFileDirectoryPtr, 
									modelTransformation, 
									count);
									
					// Get the orientation angle
					
			orientationAngle = atan2(-modelTransformation[1],modelTransformation[0]);
	                                                                         
			mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize = 
														modelTransformation[0]/cos (orientationAngle);
				
			mapProjectionInfoPtr->planarCoordinate.verticalPixelSize = 
														-modelTransformation[5]/cos (orientationAngle);
														
			mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle = orientationAngle;
	                                                                         
			mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 = modelTransformation[3];
			mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 = modelTransformation[7]; 
                                                                      
			mapProjectionInfoPtr->planarCoordinate.xMapOrientationOrigin = 
																						modelTransformation[3];
			mapProjectionInfoPtr->planarCoordinate.yMapOrientationOrigin = 
																						modelTransformation[7];
	
			CheckAndUnlockHandle (mapProjectionHandle); 
			
			}	// end "if (mapProjectionHandle != NULL)"
			
		}	// end "if (imageFileDirectory.count == 3 && ..."
		
	return (errCode);
	
}	// end "SetModelTransformationParametersFromGeoTIFF"
  


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 SetPixelScaleParametersFromGeoTIFF
//
//	Software purpose:	The purpose of this routine is to set the pixel size parameters
//							using the information in the GEOTIFF tag.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/01/2000
//	Revised By:			Larry L. Biehl			Date: 12/01/2000

SInt16 SetPixelScaleParametersFromGeoTIFF (
				FileInfoPtr							fileInfoPtr,
				TIFF_IFD_Ptr						imageFileDirectoryPtr)

{
	double								scaleParameters[3];
	
	CMFileStream*						fileStreamPtr;
	
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	Handle								mapProjectionHandle;
	
	SInt32								count;
	
	SInt16								errCode = noErr;
	
	
	if (imageFileDirectoryPtr->count >= 2 && imageFileDirectoryPtr->type == kTIFFDouble)
		{
				// At least 2 parameters and double format is expected.
										
		mapProjectionHandle = GetFileMapProjectionHandle (fileInfoPtr);
		
		if (mapProjectionHandle != NULL)
			{ 								
			mapProjectionInfoPtr = (MapProjectionInfoPtr)
												GetHandlePointer (mapProjectionHandle, kLock);
			
			fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
			
			count = MIN (3, (SInt32)imageFileDirectoryPtr->count);
											
			errCode = GetTIFFDoubleParameters (fileStreamPtr,
															imageFileDirectoryPtr,
															scaleParameters, 
															count);
				
			mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize = 
																					scaleParameters[0];
				
			mapProjectionInfoPtr->planarCoordinate.verticalPixelSize = 
																					scaleParameters[1];
	
			CheckAndUnlockHandle (mapProjectionHandle); 
			
			}	// end "if (mapProjectionHandle != NULL)"
			
		}	// end "if (imageFileDirectory.count == 3 && ..."
		
	return (errCode);
	
}	// end "SetPixelScaleParametersFromGeoTIFF"
#endif		// use_multispec_tiffcode
  


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateUpperLeftMapValues
//
//	Software purpose:	The purpose of this routine is to set the pixel size parameters
//							using the information in the GEOTIFF tag.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/08/2007
//	Revised By:			Larry L. Biehl			Date: 03/08/2007

void UpdateUpperLeftMapValues (
				FileInfoPtr							fileInfoPtr)

{		
	ControlPointsPtr					controlPointsPtr;
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
									
	controlPointsPtr = GetControlPointVectorPointers (fileInfoPtr->controlPointsHandle,
																		kLock);
														
	if (controlPointsPtr != NULL)
		{			
				// Check if the first control point is for (1,1)
				
		if (controlPointsPtr->easting1Ptr[0] != 1 ||
					controlPointsPtr->northing1Ptr[0] != 1)
			{
			mapProjectionInfoPtr = (MapProjectionInfoPtr)
												GetHandlePointer (fileInfoPtr->mapProjectionHandle,
																			kLock);
																			
			if (mapProjectionInfoPtr != NULL)
				{ 		
				GetCoefficientsVectorPointers (mapProjectionInfoPtr);
				
				TransformCoordinatePoint (
								1,
								1,
								&mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11,
								&mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11,
								mapProjectionInfoPtr->planarCoordinate.easting1CoefficientsPtr,
								mapProjectionInfoPtr->planarCoordinate.northing1CoefficientsPtr,
								mapProjectionInfoPtr->planarCoordinate.polynomialOrder);
								
				CloseCoefficientsVectorPointers (mapProjectionInfoPtr);
				
				}	// end "if (mapProjectionInfoPtr != NULL)"
				
			}	// end "if (controlPointsPtr->easting1Ptr[0] != 1 || ..."
			
		}	// end "if (controlPointsPtr != NULL)"
						
	CloseControlPointVectorPointers (fileInfoPtr->controlPointsHandle);
	
}	// end "UpdateUpperLeftMapValues"
