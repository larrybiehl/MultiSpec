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
//	File:						SNDFFormat.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/13/2019
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	This file contains routines which are used to access
//								the information in ndf formatted mage files.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include "SFileStream_class.h"
	
#if defined multispec_mac || defined multispec_mac_swift
	#define	IDS_FileIO161						161
	#define	IDS_FileIO162						162
	#define	IDS_FileIO163						163
	#define	IDS_FileIO164						164
	#define	IDS_FileIO165						165
	#define	IDS_FileIO166						166
	#define	IDS_FileIO167						167
	#define	IDS_FileIO168						168
	#define	IDS_FileIO169						169
	#define	IDS_FileIO170						170
	#define	IDS_FileIO171						171
	#define	IDS_FileIO172						172
	#define	IDS_FileIO173						173
	#define	IDS_FileIO174						174
	#define	IDS_FileIO194						194
	#define	IDS_FileIO195						195	
#endif	// defined multispec_mac || defined multispec_mac_swift

#if defined multispec_win
#endif	// defined multispec_win

#if defined multispec_wx
#endif   // defined multispec_wx

#define	kNdfType								29
#define	kPackedDegrees						1
#define	kDegrees								1



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 LinkNDFFiles
//
//	Software purpose:	The purpose of this routine is to link the image files in
//							the input NDF image data set.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/07/2007
//	Revised By:			Larry L. Biehl			Date: 02/27/2018

SInt16 LinkNDFFiles (
				Handle								windowInfoHandle)

{	
	CMFileStream						*fileStreamPtr,
											*inputFileStreamPtr;
	
	FileInfoPtr							fileInfoPtr,
											inputFileInfoPtr = NULL;
	
	Handle								fileInfoHandle,
											inputFileInfoHandle;
	
	UInt32								fileNumber,
											numberImageFiles,
											requestedFileNumber;
	
	SInt16								returnCode,
											version;
	
	Boolean								continueFlag = FALSE,
											doneFlag,
											tryBand6_7_SwapFlag;
	
	SignedByte							handleStatus;
	
	
	returnCode = 1;
	fileInfoHandle = MNewHandle (sizeof (MFileInfo));
												
	if (fileInfoHandle != NULL)
		{
		numberImageFiles = GetNumberImageFiles (windowInfoHandle);
		tryBand6_7_SwapFlag = TRUE;
		fileNumber = 1;
		doneFlag = FALSE;
		
		while (!doneFlag)
			{
			continueFlag = FALSE;
			fileNumber++;
		
					// Initialize the variables and handles in the structure.		
				
			InitializeFileInfoStructure (fileInfoHandle, kNotPointer);
			
			fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle, kLock);
			
					// Load the file name for the first channel
					// First get the file stream pointer for the structure to be used
					// to get the next file in the list.
					
			fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
					
					// Now get the pointer to the input file information block that
					// contains the first image file name for this set. Remember that
					// the fileInfoHandle may get changed by the call to 
					// "AddToImageWindowFile" so we need to get it each time through
					// this loop.
				
			inputFileInfoHandle = GetFileInfoHandle (windowInfoHandle);
			inputFileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (
																inputFileInfoHandle, &handleStatus);
												
			inputFileInfoPtr = &inputFileInfoPtr [numberImageFiles-1];
			inputFileStreamPtr = GetFileStreamPointer (inputFileInfoPtr);
			
					// We will try to swap bands 6 and 7 if Landsat TM to put in
					// wavelength order.
					 
			if (inputFileInfoPtr->instrumentCode != kLandsatTM)
				tryBand6_7_SwapFlag = FALSE;
			
					// Now copy the file name and other information.
															
			InitializeFileStream (fileStreamPtr, inputFileStreamPtr);
		
			MHSetState (inputFileInfoHandle, handleStatus);
			
					// Get the specified file number.  Read Landsat TM data in wavelength
					// order not the file number order.
			
			requestedFileNumber = fileNumber;
			if (tryBand6_7_SwapFlag)
				{
				if (fileNumber == 6)
					requestedFileNumber = 7;
				
				else if (fileNumber == 7)
					requestedFileNumber = 6;
				
				}	// end "if (tryBand6_7_SwapFlag)"
													
    				// Load the header information for the specified file													
    		
    		version = 2;
			if (ReadNDFHeader (fileInfoPtr, 
										NULL, 
										&version, 
										requestedFileNumber, 
										kLoadHeader) == noErr)
				{
						// Check if parameters make sense
					
				if (CheckFileInfoParameters (fileInfoPtr) == 1)
					{
							// Set some additional parameters.
							
					IntermediateFileUpdate (fileInfoPtr);
					
			    	if (!fileInfoPtr->thematicType)
				    			// Set flag in file information structure indicating				
				    			// whether wavelength description information is available.	
				    	ReadChannelDescriptionsAndValues (fileInfoHandle);
			
							// Add this file to the current window.
						
					continueFlag = AddToImageWindowFile (windowInfoHandle, fileInfoHandle);
					
					}	// end "if (CheckFileInfoParameters (fileInfoPtr) == 1)"
					
				else	// CheckFileInfoParameters (fileInfoPtr) != 1
					continueFlag = FALSE;
					
				}	// end "if (ReadNDFHeader (fileInfoPtr, NULL, ...) == noErr)"
	      	
	      else	// ReadNDFHeader (fileInfoPtr, ... != noErr)
	      	{
				continueFlag = TRUE;
				doneFlag = TRUE;
				
						// Check if this is for Landsat TM data and the requested file was
						// for 7 for a fileNumber of 6.  If so then try to get file number 6
						// to see if it is there.
						
				if (tryBand6_7_SwapFlag)
					{
					if (requestedFileNumber == 7 && fileNumber == 6)
						{
						fileNumber = 5;	// This will get changed to 6 after start of
												// while loop
						doneFlag = FALSE;	
						tryBand6_7_SwapFlag = FALSE;
						
						}	// end "if (requestedFileNumber == 7 && fileNumber == 6)"
					
					}	// end "if (tryBand6_7_SwapFlag)"
				
				}	// end "else ReadNDFHeader (fileInfoPtr, ... != noErr)"
 	    		
      	if (!continueFlag)
				doneFlag = TRUE;
				
			}	// end "while (!doneFlag)" 
			
		}	// end "if (fileInfoHandle != NULL)"
			
	DisposeFileInfoHandle (fileInfoHandle);
	
	if (continueFlag)
		returnCode = noErr;

	return (returnCode);
			
}	// end "LinkNDFFiles"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ReadNDFHeader
//
//	Software purpose:	This routine reads the file specifications from a NDF
//							header record.
//
//	Parameters in:		file information pointer
//							pointer to header record information
//
//	Parameters out:	None
//
//	Value Returned:	0 - if NDF header record parameters make sense
//							1 - if NDF header record paramters do not make sense.
//
// Called By:			CheckImageHeader in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/05/2007
//	Revised By:			Larry L. Biehl			Date: 01/17/2019

SInt16 ReadNDFHeader (
				FileInfoPtr 						fileInfoPtr,
				char*									headerRecordPtr, 
				SInt16*								versionPtr, 
				UInt32								fileNumber,
				SInt16								formatOnlyCode)

{
	CMFileStream						headerFileStream;

	Str63									infoStrings[4];
	
	double								lowerRightX,
											lowerRightY,
											realValue,
											realValues[15],
											upperLeftX,
											upperLeftY;
	
	CharPtr								headerSuffixName1 = (char*)"\0.H1\0",
											headerSuffixName2 = (char*)"\0.H2\0",
											headerSuffixName3 = (char*)"\0.H3\0",
											//headerSuffixName4 = (char*)"\0.DH\0",
											endOfLineStringPtr,
											ioBufferPtr,
											//ioRecordBufferPtr,
											strPtr;
											
	CMFileStream						*fileStreamPtr,
											*headerFileStreamPtr;
											
	MapProjectionInfoPtr				mapProjectionInfoPtr = NULL;
	
	FileStringPtr						headerFileNameCPtr,
											headerFilePathPPtr,
											imageFileNameCPtr,
											imageFilePathPPtr;
	
	StringPtr							headerSuffixNamePtr[3];
	
	SInt32								value;
	
	UInt32								count,
											string,
											headerSet;
	
	SInt16								errCode,
											fileType,
											instrumentCode,
											numberBands,
											returnCode = 1,
											stringNumber,
											tReturnCode;
											
	UInt16								numSuffixChars,
											pathLength;
											
	Boolean								demFlag,
											foundFlag;
	
	SignedByte							handleStatus;
	
	char									direction = ' ';
	
		
	errCode = noErr;
	fileType = 0;
	returnCode = 1;
	demFlag = FALSE;
		
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
		
	if (fileStreamPtr != NULL)		
		{
		if (fileNumber == 1 && headerRecordPtr != NULL)
			{
					//  Check if image file is a NDF header file.
						
			if (MGetString (gTextString3, kFileIOStrID, IDS_FileIO161))	// NDF_REVISION
				{
				if (strncmp ((char*)headerRecordPtr, (char*)&gTextString3[1], 6) == 0)
					{
					fileType = kNdfType;
					*versionPtr = 1;
				
							// Determine if this is a DEM header file. 
							// Find "DATA_SET_TYPE=" in the buffer.	
																
					strPtr = headerRecordPtr;
					tReturnCode = GetFileHeaderString (kFileIOStrID, 
																	IDS_FileIO173, // DATA_SET_TYPE=
																	&strPtr, 
																	-3, 
																	kDoNotSkipEqual,
																	(char*)gTextString,
																	0,
																	50,
																	kNoSubstringAllowed);	
									
					if (tReturnCode == 0)
						{
								// Get the instrument
						
						if (strstr ((char*)gTextString, "NLAPS_DEM") != NULL)
							demFlag = TRUE;
							
						}	// end "if (tReturnCode == 0)"
						
					}	// end "if (strncmp ((char*)headerRecordPtr, ..."
					
				}	// end "if (MGetString (gTextString3, ..."
			
			if (fileType == 0)
				{
						// Not NDF header; check if image file associated with ndf header
						
				imageFilePathPPtr =
						(FileStringPtr)GetFilePathPPointerFromFileStream (fileStreamPtr);
		
				if (CompareSuffixNoCase ((char*)"\0.I?",
													imageFilePathPPtr,
													&numSuffixChars))
					{
					fileType = kNdfType;
					*versionPtr = 2;
					
					}	// end "if (CompareSuffixNoCase ((char*)"\0.I?", ..."
		
				else if (CompareSuffixNoCase ((char*)"\0.DD",
														imageFilePathPPtr,
														&numSuffixChars))
					{
					fileType = kNdfType;
					*versionPtr = 2;
					demFlag = TRUE;
					
					}	// end "else if (CompareSuffixNoCase ((char*)"\0.DD", ..."
					
				}	// end "if (fileType == 0)"
				
			}	// end "if (fileNumber == 1 && ...)"
			
		else if (fileNumber > 1)
			fileType = kNdfType;
			
		}	// end "if (fileStreamPtr != NULL)"
		
	if (fileType != 0)
		{
		if (formatOnlyCode != kLoadHeader)
			{
			fileInfoPtr->format = fileType;
			fileInfoPtr->thematicType = FALSE;
			//*versionPtr = 0;
																						return (noErr);
																								
			}	// end "if (formatOnlyCode != kLoadHeader)"
			
				// Get a buffer to read in the header record into.  For now use
				// 5000 bytes.
		
		ioBufferPtr = (char*)MNewPointer (5000);
		if (ioBufferPtr == NULL)
																						return (1);

		if (*versionPtr == 1)
			{
					// The NDF header file was selected; read the metadata information
					// in the file.
			
			errCode = MSetMarker (fileStreamPtr, fsFromStart, 0, kErrorMessages);
		
					// Read header.
			
			count = 4999;
			if (errCode == noErr)
				errCode = MReadData (fileStreamPtr, &count, ioBufferPtr, kNoErrorMessages);
			
			if (errCode == eofErr)
				errCode = noErr;
			
			if (errCode == noErr)
				{	
						// Make sure that there is a c string terminator at the end of
						// the data that was read to force searches to end there now.
						// Also put a c string terminator at the end of that which was read
						// in.
				
				ioBufferPtr[4999] = 0;	
				ioBufferPtr[count] = 0;
				
						// Save the path length
				
				pathLength = 0;
				#if defined multispec_wx || defined multispec_win
					pathLength = fileStreamPtr->GetFileUTF8PathLength ();
				#endif
				
						// The header file has been read.  Close the header file.
					
				imageFilePathPPtr = (FileStringPtr)GetFilePathPPointerFromFileStream (
																							fileStreamPtr);
				CloseFile (fileStreamPtr);
				
				if (demFlag)
					{
					RemoveCharsNoCase ((char*)"\0.DH\0", imageFilePathPPtr);
					ConcatFilenameSuffix (imageFilePathPPtr, (UCharPtr)"\0.DD\0");
					
					}	// end "demFlag"
				
				else	// !demFlag
					{
							// Get the name of the first band file in the list.
							// Find "BAND1_FILENAME=" in the buffer.
						
					strPtr = ioBufferPtr;
					/*
					#if defined multispec_win
					
							// Convert wide string to multibye string.
				
						wcstombs ((char*)&gTextString[1], &imageFilePathPPtr[1], 254);
						gTextString[0] = (unsigned char)imageFilePathPPtr[0];
						tReturnCode = GetFileHeaderString (kFileIOStrID,
																	IDS_FileIO195, // BAND1_FILENAME
																	&strPtr, 
																	-3, 
																	kSkipEqual,
																	(char*)gTextString,
																	0,
																	64,
																	kNoSubstringAllowed);
					#else
					*/
					tReturnCode = GetFileHeaderString (kFileIOStrID,
																	IDS_FileIO195, // BAND1_FILENAME
																	&strPtr, 
																	-3, 
																	kSkipEqual,
																	(char*)imageFilePathPPtr,
																	pathLength,
																	254,
																	kNoSubstringAllowed);
					//#endif
						
					}	// end "else !demFlag"
									
				if (tReturnCode == 0)
					{
							// Locate the image file.
			
					errCode = OpenFileReadOnly (fileStreamPtr, 
															kResolveAliasChains, 
															kLockFile, 
															kVerifyFileStream);
					
					}	// end "if (tReturnCode == 0)"
					
				else	// tReturnCode != 0
					errCode = tReturnCode;
					
				}	// end "if (errCode == noErr)"
				
			}	// end "if (*versionPtr == 1)" 

		else	// *versionPtr == 2
			{
					// The NDF image file was selected; get the header file for
					// this image. It can have suffixes of H1, H2, H3 or HD.
			
			headerFileStreamPtr = &headerFileStream;		
			InitializeFileStream (headerFileStreamPtr, fileStreamPtr);
					
			headerFileNameCPtr = (FileStringPtr)GetFileNameCPointerFromFileStream (
																					headerFileStreamPtr);
			
			imageFileNameCPtr = (FileStringPtr)GetFileNameCPointerFromFileStream (
																					fileStreamPtr);
			headerFilePathPPtr = (FileStringPtr)GetFilePathPPointerFromFileStream (
																					headerFileStreamPtr);
			
			if (demFlag)
				RemoveCharsNoCase ((char*)"\0.DD\0", headerFilePathPPtr);
			
			else	// !demFlag
				RemoveCharsNoCase ((char*)"\0.I?\0", headerFilePathPPtr);
			
			headerSuffixNamePtr[0] = (StringPtr)headerSuffixName1;
			headerSuffixNamePtr[1] = (StringPtr)headerSuffixName2;
			headerSuffixNamePtr[2] = (StringPtr)headerSuffixName3;
			
			foundFlag = FALSE;
			headerSet = 0;
			while (!foundFlag && headerSet<3 && errCode == noErr)
				{
				if (demFlag)
					ConcatFilenameSuffix (headerFilePathPPtr, (UCharPtr)"\0.DH\0");
			
				else	// !demFlag
					{
					RemoveCharsNoCase ((char*)"\0.H?\0", headerFilePathPPtr);
					ConcatFilenameSuffix (headerFilePathPPtr,
													headerSuffixNamePtr[headerSet]);
					
					}	// end "else !demFlag"
					
				errCode = OpenFileReadOnly (headerFileStreamPtr, 
														kResolveAliasChains, 
														kLockFile, 
														kVerifyFileStream);
					
				if (errCode == noErr)
					{
					count = 4999;
					errCode = MReadData (headerFileStreamPtr,
												&count, 
												ioBufferPtr, 
												kNoErrorMessages);
			
					if (errCode == eofErr)
						errCode = noErr;
												
							// Make sure that there is a c string terminator at the end of
							// to force searches to end there now.
							// Also put a c string terminator at the end of that which was
							// read in.
					
					if (errCode == noErr)
						{
						ioBufferPtr[4999] = 0;	
						ioBufferPtr[count] = 0;
						
						}	// end "if (errCode == noErr)"
		
					CloseFile (headerFileStreamPtr);
		
					}	// end "if (errCode == noErr)"
					
				if (errCode == noErr)
					{	
					if (demFlag)
						foundFlag = TRUE;
					
					else	// !demFlag
						{
								// Now find the image file name in this administrative record.
								// Remember that 'imageFileNameCPtr' is a c string.
								
						strPtr = strstr (ioBufferPtr, (char*)&imageFileNameCPtr[1]);

						if (strPtr != NULL)
							foundFlag = TRUE;
							
						}	// end "else !demFlag"
		
					}	// end "if (errCode == noErr)"
														
				if (errCode == fnfErr)
					errCode = noErr;
						
				headerSet++;
					
				}	// end "while (!foundFlag && headerSet<3 && errCode == noErr)"
				
			if (!foundFlag)
				errCode = -1;
				
			if (foundFlag && fileNumber > 1)
				{
						// Save the path length
				
				pathLength = 0;
				#if defined multispec_wx || defined multispec_win
					pathLength = fileStreamPtr->GetFileUTF8PathLength ();
				#endif
				
						// Get the name of the specified file in the list.
						// Find "FILENAME=" in the buffer.	
						
				imageFilePathPPtr = (FileStringPtr)GetFilePathPPointerFromFileStream (
																							fileStreamPtr);
				//strPtr = ioBufferPtr;
				
				stringNumber = IDS_FileIO195 + (SInt16)fileNumber - 1;
				
				tReturnCode = GetFileHeaderString (kFileIOStrID, 
																stringNumber, 	// BANDn_FILENAME
																&strPtr, 
																-3, 
																kSkipEqual,
																(char*)imageFilePathPPtr,
																pathLength,
																254,
																kNoSubstringAllowed);
																
				if (tReturnCode != noErr)
					errCode = tReturnCode;
								
				else	// tReturnCode == noErr
							// Locate the image file.			
					errCode = OpenFileReadOnly (fileStreamPtr, 
															kResolveAliasChains, 
															kLockFile, 
															kVerifyFileStream);
									
				}	// end "if (foundFlag && fileNumber > 1)"
				
			}	// end "else *versionPtr == 2"
				
		if (errCode == noErr)
			{
					// Find "PIXELS_PER_LINE=" in the buffer.								
				
			value = GetFileHeaderValue (kFileIOStrID, 
													IDS_FileIO162, 	// PIXELS_PER_LINE=
													ioBufferPtr, 
													1,
													kDoNotSkipEqual, 
													&tReturnCode);
								
			if (tReturnCode == 0)
				fileInfoPtr->numberColumns = value;
				
			else	// tReturnCode != 0
				errCode = tReturnCode;
			
			}	// end "if (errCode == noErr)" 
		
		if (errCode == noErr)
			{
					// Find "LINES_PER_DATA_FILE=" in the buffer.								
				
			value = GetFileHeaderValue (kFileIOStrID, 
													IDS_FileIO163, 	// LINES_PER_DATA_FILE=
													ioBufferPtr, 
													1,
													kDoNotSkipEqual, 
													&tReturnCode);
								
			if (tReturnCode == 0)
				fileInfoPtr->numberLines = value;
				
			else	// tReturnCode != 0
				errCode = tReturnCode;
			
			}	// end "if (errCode == noErr)"  
		
		if (errCode == noErr)
			{
					// Find "PIXEL_FORMAT=" in the buffer.	
					
			strPtr = ioBufferPtr;
			tReturnCode = GetFileHeaderString (kFileIOStrID, 
															IDS_FileIO164, 	// PIXEL_FORMAT=
															&strPtr, 
															-3, 
															kDoNotSkipEqual,
															(char*)gTextString,
															0,
															50,
															kNoSubstringAllowed);	
							
			if (tReturnCode == 0)
				{
						// Get the number of bytes per pixel
				
				if (strstr ((char*)gTextString, "2BYTEINT") != NULL)
					fileInfoPtr->numberBytes = 2;
				
				else if (strstr ((char*)gTextString, "4BYTEINT") != NULL)
					fileInfoPtr->numberBytes = 4;
				
				else if (strstr ((char*)gTextString, "BYTE") != NULL)
					fileInfoPtr->numberBytes = 1;
				
				else if (strstr ((char*)gTextString, "REAL") != NULL)
					fileInfoPtr->numberBytes = 4;
				
				else if (strstr ((char*)gTextString, "DOUBLE") != NULL)
					fileInfoPtr->numberBytes = 8;
				
				fileInfoPtr->numberBits = 8 * fileInfoPtr->numberBytes;
					
				}	// end "if (tReturnCode == 0)"
				
			else	// tReturnCode != 0
				errCode = tReturnCode;
			
			}	// end "if (errCode == noErr)"  
		
		if (errCode == noErr && fileInfoPtr->numberBytes >= 2)
			{
					// Find "PIXEL_ORDER=" in the buffer.	
					
			strPtr = ioBufferPtr;
			tReturnCode = GetFileHeaderString (kFileIOStrID, 
															IDS_FileIO174, 	// PIXEL_ORDER=
															&strPtr, 
															-3, 
															kDoNotSkipEqual,
															(char*)gTextString,
															0,
															50,
															kNoSubstringAllowed);	
							
			if (tReturnCode == 0)
				{
						// Get the number of bytes per pixel
				
				if (strstr ((char*)gTextString, "BYTE_INVERTED") != NULL)
					fileInfoPtr->swapBytesFlag = gBigEndianFlag; 
				
				else	// !BYTE_INVERTED
					fileInfoPtr->swapBytesFlag = !gBigEndianFlag;
					
				}	// end "if (tReturnCode == 0)"
				
			else	// tReturnCode != 0
				errCode = tReturnCode;
			
			}	// end "if (errCode == noErr && fileInfoPtr->numberBytes >= 2)"  
		
		if (errCode == noErr)
			{
					// Find "NUMBER_OF_BANDS_IN_VOLUME=" in the buffer.								
			
			strPtr = ioBufferPtr;	
			numberBands = (SInt16)GetFileHeaderValue (
													kFileIOStrID, 
													IDS_FileIO165, // NUMBER_OF_BANDS_IN_VOLUME=
													ioBufferPtr, 
													1, 
													kDoNotSkipEqual, 
													&tReturnCode);		
								
			if (tReturnCode == 0)
				fileInfoPtr->numberChannels = 1;
				
			else	// tReturnCode != 0
				errCode = tReturnCode;
			
			}	// end "if (errCode == noErr)" 
		
		if (errCode == noErr)
			{
					// Find "DATA_SET_TYPE=" in the buffer.	
														
			strPtr = ioBufferPtr;
			tReturnCode = GetFileHeaderString (kFileIOStrID, 
															IDS_FileIO173, 	// DATA_SET_TYPE=
															&strPtr, 
															-3, 
															kDoNotSkipEqual,
															(char*)gTextString,
															0,
															50,
															kNoSubstringAllowed);	
							
			if (tReturnCode == 0)
				{
						// Get the instrument
				
				if (strstr ((char*)gTextString, "EDC_ETM+") != NULL)
					instrumentCode = kLandsatTM7;
				
				else if (strstr ((char*)gTextString, "EDC_MSS") != NULL)
					{
					instrumentCode = kLandsatMSS;
					if (numberBands == 5)
						instrumentCode = kLandsatMSS5;
					
					}	// end "else if (strstr ((char*)gTextString, ..."
				
				else if (strstr ((char*)gTextString, "EDC_TM") != NULL)
					instrumentCode = kLandsatTM;
					
				}	// end "if (tReturnCode == 0)"
					
			else	// tReturnCode != 0
				errCode = tReturnCode;
				
			}	// end "if (errCode == noErr)" 
		
		if (errCode == noErr)
			{
			fileInfoPtr->mapProjectionHandle = GetMapProjectionHandle ();
			
			if (fileInfoPtr->mapProjectionHandle != NULL)
				{
				mapProjectionInfoPtr = (MapProjectionInfoPtr)
								GetHandleStatusAndPointer (fileInfoPtr->mapProjectionHandle,
																	&handleStatus);
					
				}	// end "if (fileInfoPtr->mapProjectionHandle != NULL)"
				
			}	// end "if (errCode == noErr)"
								
		if (mapProjectionInfoPtr != NULL)
			{
					// Note that errors in this section finding map projection parameters will not be
					// considered something to stop accepting this header.
					
					// Find "USGS_PROJECTIOIN_NUMBER=" in the buffer.	
					
			value = GetFileHeaderValue (kFileIOStrID, 
													IDS_FileIO166, 	// USGS_PROJECTIOIN_NUMBER=
													ioBufferPtr, 
													1, 
													kDoNotSkipEqual, 
													&tReturnCode);
							
			if (tReturnCode == 0)
				{
						// Get the projection code
				
				mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = 
							GetMapProjectionCodeFromGCTPNumber (
										(SInt16)value,
										FALSE,
										&mapProjectionInfoPtr->gridCoordinate.projectionCode);
				
				if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode ==
																					kStatePlaneNAD27RSCode)
					mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kUSSurveyFeetCode;
								
				else	// mapProjectionInfoPtr->gridCoordinate.referenceSystemCode != ...
					mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kMetersCode;
					
				}	// end "if (tReturnCode == 0)"
				
					// Find "PIXEL_SPACING=" in the buffer.		
					
			realValue = GetFileHeaderRealValue (kFileIOStrID, 
															IDS_FileIO167, 	// PIXEL_SPACING=
															ioBufferPtr, 
															1,
															kDoNotSkipEqual, 
															&tReturnCode);
								
			if (tReturnCode == 0)
				{
				mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize = realValue;
				mapProjectionInfoPtr->planarCoordinate.verticalPixelSize = realValue;
								
				}	// end "if (tReturnCode == 0)"
				
					// Find "DATUM =" in the buffer.		
										
			strPtr = ioBufferPtr;
			tReturnCode = GetDatumInfo (&strPtr, 
													&mapProjectionInfoPtr->geodetic.datumCode,
													IDS_FileIO194,
													kSkipEqual);	
			
			if (tReturnCode == noErr)
				{
				if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode ==
																			kStatePlaneNAD27RSCode &&
										mapProjectionInfoPtr->geodetic.datumCode == kNAD83Code)
					mapProjectionInfoPtr->gridCoordinate.referenceSystemCode =
																					kStatePlaneNAD83RSCode;
				SetEllipsoidFromDatum (mapProjectionInfoPtr);
				
				}	// end "if (tReturnCode == noErr)"
		
					// Find "USGS_MAP_ZONE=" in the buffer.								
				
			value = GetFileHeaderValue (kFileIOStrID, 
													IDS_FileIO171, 	// USGS_MAP_ZONE=
													ioBufferPtr, 
													1,
													kDoNotSkipEqual, 
													&tReturnCode);
								
			if (tReturnCode == 0)
				mapProjectionInfoPtr->gridCoordinate.gridZone = (SInt16)value;
				
					// Find "USGS_PROJECTION_PARAMETERS=" in the buffer.			
									
			strPtr = ioBufferPtr;					
			tReturnCode = GetFileHeaderString (kFileIOStrID, 
															IDS_FileIO172, // USGS_PROJECTION_PARAMETERS=
															&strPtr, 
															-1, 
															kDoNotSkipEqual,
															(char*)gTextString,
															0,
															50,
															kNoSubstringAllowed);		
								
			if (tReturnCode == 0)
				{
						// The projection info line was found.  Now read the 15 strings that 
						// should be in this line that are separated by commas.
						
				returnCode = sscanf (
							strPtr,
							"%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf;",
							&realValues[0], 
							&realValues[1], 
							&realValues[2], 
							&realValues[3], 
							&realValues[4], 
							&realValues[5], 
							&realValues[6], 
							&realValues[7], 
							&realValues[8], 
							&realValues[9], 
							&realValues[10], 
							&realValues[11], 
							&realValues[12], 
							&realValues[13], 
							&realValues[14]);
										
				if (returnCode >= 8)
					SetProjectionParameters (
								mapProjectionInfoPtr, realValues, kPackedDegrees, kDegrees);
				
				}	// end "if (tReturnCode == 0)"
				
					// Find "UPPER_LEFT_CORNER=" in the buffer.			
										
			strPtr = ioBufferPtr;				
			tReturnCode = GetFileHeaderString (kFileIOStrID, 
															IDS_FileIO168, 	// UPPER_LEFT_CORNER=
															&strPtr, 
															-1, 
															kDoNotSkipEqual,
															(char*)gTextString,
															0,
															50,
															kNoSubstringAllowed);		
								
			
			upperLeftX = 0;
			upperLeftY = 0;
			if (tReturnCode == 0)
				{
				endOfLineStringPtr = (char*)strstr (strPtr, ";");
					
				for (string=0; string<4; string++)
					{
					strPtr = GetStringToComma (strPtr, 
															endOfLineStringPtr,
															(char*)infoStrings[string],
															64);
					
					if (strPtr == NULL)
						break;
					
					}	// end "for (string=0; string<4; string++)"
								
						// Determine whether this image is from the northern
						// or southern hemisphere. It will be used for UTM zone
						// defination if that is the projection.
						
				if (strstr ((char*)&infoStrings[1], "N") != NULL)
					direction = 'N';
					
				else if (strstr ((char*)&infoStrings[1], "S") != NULL)
					direction = 'S';
				
						// Now get the values 3rd and 4th values in the line.
				
				tReturnCode = sscanf ((char*)&infoStrings[2], "%lf", &realValue);
				if (tReturnCode == 1)
					upperLeftX = realValue;
					
				tReturnCode = sscanf ((char*)&infoStrings[3], "%lf", &realValue);
				if (tReturnCode == 1)
					upperLeftY = realValue;
				
				}	// end "if (tReturnCode == 0)"
				
					// Find "LOWER_RIGHT_CORNER=" in the buffer.			
														
			tReturnCode = GetFileHeaderString (kFileIOStrID, 
															IDS_FileIO169, 	// LOWER_RIGHT_CORNER=
															&strPtr, 
															-1, 
															kDoNotSkipEqual,
															(char*)gTextString,
															0,
															50,
															kNoSubstringAllowed);		
								
			lowerRightX = 0;
			lowerRightY = 0;
			if (tReturnCode == 0)
				{
						// Read the 4 strings
						
				endOfLineStringPtr = (char*)strstr (strPtr, ";");
					
				for (string=0; string<4; string++)
					{
					strPtr = GetStringToComma (strPtr, 
															endOfLineStringPtr,
															(char*)infoStrings[string],
															64);
					
					if (strPtr == NULL)
						break;
					
					}	// end "for (string=0; string<4; string++)"
												
						// Now get the values 3rd and 4th values in the line.
				
				tReturnCode = sscanf ((char*)&infoStrings[2], "%lf", &realValue);
				if (tReturnCode == 1)
					lowerRightX = realValue;
					
				tReturnCode = sscanf ((char*)&infoStrings[3], "%lf", &realValue);
				if (tReturnCode == 1)
					lowerRightY = realValue;
				
				}	// end "if (tReturnCode == 0)"
				
					// Now set the actual sampled pixel width and the upper left
					// pixel center coordinates.
					
			if (lowerRightX < upperLeftX)
				mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize = 
						-fabs (mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize);

			if (lowerRightY > upperLeftY)
				mapProjectionInfoPtr->planarCoordinate.verticalPixelSize = 
						-fabs (mapProjectionInfoPtr->planarCoordinate.verticalPixelSize);
				
			mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 = upperLeftX;
			mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 = upperLeftY;
									
			mapProjectionInfoPtr->planarCoordinate.xMapOrientationOrigin =           
									mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11;
			mapProjectionInfoPtr->planarCoordinate.yMapOrientationOrigin =           
									mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11;
				
					// Load additional projection information if needed.
			
			if (fileNumber > 1)		
				LoadSpheroidInformation (fileInfoPtr->mapProjectionHandle);
		
			MHSetState (fileInfoPtr->mapProjectionHandle, handleStatus);
				
			}	// end "if (mapProjectionInfoPtr != NULL)"
		
		if (errCode == noErr)
			{
			returnCode = noErr;
			
					// Determine the image type if it has not been determined yet.
					
			if (gGetFileImageType == 0)
				{
				gGetFileImageType = kMultispectralImageType;
				fileInfoPtr->numberClasses = 0;
				
				}	// end "if (gGetFileImageType == 0)"
				
			fileInfoPtr->format = kNdfType;
			fileInfoPtr->ancillaryInfoformat = kNdfType;
			fileInfoPtr->instrumentCode = instrumentCode;
			
			fileInfoPtr->thematicType = FALSE;
			if (gGetFileImageType == kThematicImageType)
				fileInfoPtr->thematicType = TRUE;
				
			}	// end "if (returnCode == noErr)"
		
		CheckAndDisposePtr (ioBufferPtr);
		
		}	// end "if (fileInfoPtr != NULL)"
		
	return (returnCode);
	
}	// end "ReadNDFHeader"
