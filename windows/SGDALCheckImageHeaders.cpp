//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2009)
//							� Purdue Research Foundation
//									All rights reserved.
//
//					The following was put here for easy access to a template
//  				DebugStr ("\pSGDALCheckImageHeaders.cpp");
//
//	File:						pSGDALCheckImageHeaders.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision number:		3.0
//
//	Revision date:			12/11/2009
//
//	Language:				C
//
//	System:					Macintosh and Windows Operating Systems	
//
//	Brief description:	This file contains routines to access png type files.
//
//	Functions in file:	ReadJPEGHeader
//								ReadJPEG2000Header
//								ReadPNGHeader								
//
//	Diagram of MultiSpec routine calls for the routines in the file.
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
//------------------------------------------------------------------------------------

#include "SMulSpec.h"
	
#if defined multispec_mac
		
#endif	// defined multispec_mac    

#if defined multispec_win
   	 
#endif	// defined multispec_win

#include "SExtGlob.h"

#if include_gdal_capability

// SInt16						ReadBMPHeader (
// 									FileInfoPtr 						fileInfoPtr, 
// 									char*									headerRecordPtr,
// 									SInt16								formatOnlyCode);
									
SInt16						ReadPNGHeader (
									FileInfoPtr 						fileInfoPtr, 
									char*									headerRecordPtr,
									SInt16								formatOnlyCode);

extern SInt16				ReadGDALHeaderInformation (
									FileInfoPtr 						fileInfoPtr,
									SInt16								formatOnlyCode);


/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2008)
//								� Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 ReadBMPHeader
//
//	Software purpose:	This routine verifies that the input file buffer is from a 
//							bmp formatted file and then reads the file specifications 
//							for the image.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/11/2009
//	Revised By:			Larry L. Biehl			Date: 12/11/2009

SInt16 ReadBMPHeader (
				FileInfoPtr 						fileInfoPtr, 
				char*									headerRecordPtr,
				SInt16								formatOnlyCode)

{	
	SInt16								returnCode = 1;
	
	unsigned char						charKeyCode[2];
		
	CMFileStream*						fileStreamPtr;		
	SInt32								bmpReturn = 2;
		
	SInt16								errCode,
											fileType = 0;
	
			
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);					
		
	if (fileInfoPtr != NULL && fileStreamPtr != NULL)		
		{
				// Check if JPEG formatted image file.
				
		charKeyCode[0] = 'B';
		charKeyCode[1] = 'M';
		
		if (strncmp ((char*)headerRecordPtr, (CharPtr)charKeyCode, 2) == 0)
			fileType = kBMPType;
		
		}		// end "if (fileInfoPtr != NULL && fileStreamPtr != NULL)"
		
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
																								
			}		// end "if (formatOnlyCode != kLoadHeader)"
		
		returnCode = 0; 
		
				// Make sure that the default volume is correct.
				
		errCode = SetVolume (fileStreamPtr, kErrorMessages);
		
		bmpReturn = ReadGDALHeaderInformation (fileInfoPtr, kLoadHeader);

		if (bmpReturn != noErr)
			{
					// Display an alert indicating that MultiSpec could not read or had
					// problems reading this HDF file.
					
			DisplayAlert (kErrorAlertID, 
								kStopAlert, 
								kAlertStrID, 
								(SInt16)(IDS_Alert95 + bmpReturn - 1),
								0, 
								NULL);
								
			returnCode = 1;
			
			}		// end "if (bmpReturn != noErr)"
			
		if (returnCode == noErr)
			{
			fileInfoPtr->format = fileType;
//			fileInfoPtr->dataCompressionCode = kSkipHuffmanCompression;
			
			if (gGetFileImageType == 0)
				{
				if (fileInfoPtr->thematicType)
					gGetFileImageType = kThematicImageType;
					
				else		// !fileInfoPtr->thematicType 
					gGetFileImageType = kMultispectralImageType;
					
				}		// end "if (gGetFileImageType == 0)"
				
			}		// end "if (returnCode > 0)" 
		
		}		// end "if (fileTpye != 0)"  
		
	return (returnCode);
	
}		// end "ReadBMPHeader" 
*/

/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2008)
//								� Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 ReadJPEGHeader
//
//	Software purpose:	This routine verifies that the input file buffer is from a 
//							JPEG formatted file and then reads the file specifications 
//							for the image.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/11/2009
//	Revised By:			Larry L. Biehl			Date: 12/11/2009

SInt16 ReadJPEGHeader (
				FileInfoPtr 						fileInfoPtr, 
				char*									headerRecordPtr,
				SInt16								formatOnlyCode)

{	
	SInt16								returnCode = 1;
	
	unsigned char						charKeyCode[3];
		
	CMFileStream*						fileStreamPtr;		
	SInt32								jpegReturn = 2;
		
	SInt16								errCode,
											fileType = 0;
	
			
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);					
		
	if (fileInfoPtr != NULL && fileStreamPtr != NULL)		
		{
				// Check if JPEG formatted image file.
				
		charKeyCode[0] = 0xff;
		charKeyCode[1] = 0xd8;
		charKeyCode[2] = 0xff;
		
		if (strncmp ((char*)headerRecordPtr, (CharPtr)charKeyCode, 3) == 0)
			fileType = kJPEGType;
		
		}		// end "if (fileInfoPtr != NULL && fileStreamPtr != NULL)"
		
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
																								
			}		// end "if (formatOnlyCode != kLoadHeader)"
		
		returnCode = 0; 
		
				// Make sure that the default volume is correct.
				
		errCode = SetVolume (fileStreamPtr, kErrorMessages);
		
		jpegReturn = ReadGDALHeaderInformation (fileInfoPtr, kLoadHeader);

		if (jpegReturn != noErr)
			{
					// Display an alert indicating that MultiSpec could not read or had
					// problems reading this jpeg file.
					
			DisplayAlert (kErrorAlertID, 
								kStopAlert, 
								kAlertStrID, 
								(SInt16)(IDS_Alert95 + jpegReturn - 1),
								0, 
								NULL);
								
			returnCode = 1;
			
			}		// end "if (jpegReturn != noErr)"
			
		if (returnCode == noErr)
			{
			fileInfoPtr->format = fileType;
			fileInfoPtr->dataCompressionCode = kSkipHuffmanCompression;
			
			if (gGetFileImageType == 0)
				{
				if (fileInfoPtr->thematicType)
					gGetFileImageType = kThematicImageType;
					
				else		// !fileInfoPtr->thematicType 
					gGetFileImageType = kMultispectralImageType;
					
				}		// end "if (gGetFileImageType == 0)"
				
			}		// end "if (returnCode > 0)" 
		
		}		// end "if (fileTpye != 0)"  
		
	return (returnCode);
	
}		// end "ReadJPEGHeader" 
*/

/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2008)
//								� Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 ReadJPEG2000Header
//
//	Software purpose:	This routine verifies that the input file buffer is from a 
//							JPEG2000 formatted file and then reads the file specifications 
//							for the image.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/06/2009
//	Revised By:			Larry L. Biehl			Date: 12/11/2009

SInt16 ReadJPEG2000Header (
				FileInfoPtr 						fileInfoPtr, 
				char*									headerRecordPtr,
				SInt16								formatOnlyCode)

{	
	SInt16								returnCode = 1;
	
	unsigned char						charKeyCode[12];
		
	CMFileStream*						fileStreamPtr;		
	SInt32								jpeg2000Return = 2;
		
	SInt16								errCode,
											fileType = 0;
	
			
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);					
		
	if (fileInfoPtr != NULL && fileStreamPtr != NULL)		
		{
				// Check if JPEG2000 formatted image file.
				
		charKeyCode[0] = 0x00;
		charKeyCode[1] = 0x00;
		charKeyCode[2] = 0x00;
		charKeyCode[3] = 0x0C;
		charKeyCode[4] = 0x6A;
		charKeyCode[5] = 0x50;
		charKeyCode[6] = 0x20;
		charKeyCode[7] = 0x20;
		charKeyCode[8] = 0x0D;
		charKeyCode[9] = 0x0A;
		charKeyCode[10] = 0x87;
		charKeyCode[11] = 0x0A;
			
		if (strncmp ((char*)&headerRecordPtr[3], (CharPtr)&charKeyCode[3], 9) == 0)
			fileType = kJPEG2000Type;
		
		if (headerRecordPtr[0] != charKeyCode[0] || 
				headerRecordPtr[1] != charKeyCode[1] ||
				headerRecordPtr[2] != charKeyCode[2])			
			fileType = 0;
		
		}		// end "if (fileInfoPtr != NULL && fileStreamPtr != NULL)"
		
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
																								
			}		// end "if (formatOnlyCode != kLoadHeader)"
		
		returnCode = 0; 
		
				// Make sure that the default volume is correct.
				
		errCode = SetVolume (fileStreamPtr, kErrorMessages);
		
		jpeg2000Return = ReadGDALHeaderInformation (fileInfoPtr, kLoadHeader);

		if (jpeg2000Return != noErr)
			{
					// Display an alert indicating that MultiSpec could not read or had
					// problems reading this jpeg2000 file.
					
			DisplayAlert (kErrorAlertID, 
								kStopAlert, 
								kAlertStrID, 
								(SInt16)(IDS_Alert95 + jpeg2000Return - 1),
								0, 
								NULL);
								
			returnCode = 1;
			
			}		// end "if (hdfReturn != noErr)"
			
		if (returnCode == noErr)
			{
			fileInfoPtr->format = fileType;
			fileInfoPtr->dataCompressionCode = kJPEG2000Compression;
			
			if (gGetFileImageType == 0)
				{
				if (fileInfoPtr->thematicType)
					gGetFileImageType = kThematicImageType;
					
				else		// !fileInfoPtr->thematicType 
					gGetFileImageType = kMultispectralImageType;
					
				}		// end "if (gGetFileImageType == 0)"
				
			}		// end "if (returnCode > 0)" 
		
		}		// end "if (fileTpye != 0)"  
		
	return (returnCode);
	
}		// end "ReadJPEG2000Header" 
*/

/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2008)
//								� Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 ReadPNGHeader
//
//	Software purpose:	This routine verifies that the input file buffer is from a 
//							png formatted file and then reads the file specifications 
//							for the image.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/11/2009
//	Revised By:			Larry L. Biehl			Date: 12/11/2009

SInt16 ReadPNGHeader (
				FileInfoPtr 						fileInfoPtr, 
				char*									headerRecordPtr,
				SInt16								formatOnlyCode)

{	
	SInt16								returnCode = 1;
	
	unsigned char						charKeyCode[8];
		
	CMFileStream*						fileStreamPtr;		
	SInt32								pngReturn = 2;
		
	SInt16								errCode,
											fileType = 0;
	
			
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);					
		
	if (fileInfoPtr != NULL && fileStreamPtr != NULL)		
		{
				// Check if PNG formatted image file.
				
		charKeyCode[0] = 137;
		charKeyCode[1] = 80;
		charKeyCode[2] = 78;
		charKeyCode[3] = 71;
		charKeyCode[4] = 13;
		charKeyCode[5] = 10;
		charKeyCode[6] = 26;
		charKeyCode[7] = 10;
		
		if (strncmp ((char*)headerRecordPtr, (CharPtr)charKeyCode, 8) == 0)
			fileType = kPNGType;
		
		}		// end "if (fileInfoPtr != NULL && fileStreamPtr != NULL)"
		
	if (fileType != 0)
		{
		if (formatOnlyCode != kLoadHeader)
			{
			fileInfoPtr->format = fileType;
			fileInfoPtr->dataCompressionCode = kPNGCompression;
			
					// For now we will not check whether the file is thematic
					// or multispectral
					
			if (formatOnlyCode == kMultiFiles)
				fileInfoPtr->thematicType = FALSE;
			
			else if (formatOnlyCode == kThematicFiles)
				fileInfoPtr->thematicType = TRUE;
				
																			return (noErr);
																								
			}		// end "if (formatOnlyCode != kLoadHeader)"
		
		returnCode = 0; 
		
				// Make sure that the default volume is correct.
				
		errCode = SetVolume (fileStreamPtr, kErrorMessages);
		
		pngReturn = ReadGDALHeaderInformation (fileInfoPtr, kLoadHeader);

		if (pngReturn != noErr)
			{
					// Display an alert indicating that MultiSpec could not read or had
					// problems reading this PNG file.
					
			DisplayAlert (kErrorAlertID, 
								kStopAlert, 
								kAlertStrID, 
								(SInt16)(IDS_Alert95 + pngReturn - 1),
								0, 
								NULL);
								
			returnCode = 1;
			
			}		// end "if (pngReturn != noErr)"
			
		if (returnCode == noErr)
			{
			fileInfoPtr->format = fileType;
			fileInfoPtr->dataCompressionCode = kPNGCompression;
			
			if (gGetFileImageType == 0)
				{
				if (fileInfoPtr->thematicType)
					gGetFileImageType = kThematicImageType;
					
				else		// !fileInfoPtr->thematicType 
					gGetFileImageType = kMultispectralImageType;
					
				}		// end "if (gGetFileImageType == 0)"
				
			}		// end "if (returnCode > 0)" 
		
		}		// end "if (fileTpye != 0)"  
		
	return (returnCode);
	
}		// end "ReadPngHeader" 
*/
#endif	// include_gdal_capability
