//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2019)
//							 (c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SSaveWrite.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			01/07/2019
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	The routines in this file handle writing and reading
//								Transformation, kml, thematic support (.sti * .clr)
//								files and writing GeoTIFF files.
//
//	Functions in file:	Boolean 				GetGroupInfoFile
//								SInt16 				GetNextLine
//								Boolean 				GetThematicSupportFile
//								void 					LoadThematicClasses 
//								void 					LoadThematicGroups
//								void 					LoadThematicInfo
//								void					LoadTransformationFile
//								Boolean 				ReadOffsetGainFile
//								Boolean 				ReadTransformationFile
//								void					SaveImageWindowAs
//								Boolean				SaveTextOutputAs  
//								char* 				SkipToNextCarriageReturn
//								Boolean 				WriteErdasHeader
//								Boolean				WriteNewErdasHeader
//								Boolean				WriteNewImageHeader
//								Boolean 				WriteTextOutputFile
//								void 					WriteThematicClassesAs
//								void 					WriteThematicGroups
//								void 					WriteThematicInfo
//								short int 			WriteTIFFColorMap
//								short int 			WriteTIFFImageData
//								Boolean				WriteTIFFImageFile
//								void					WriteTransformationFile
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
/* Template for debugging
		int numberChars = sprintf ((char*)gTextString3,
												" SSaveWrt::xxx (entered routine. %s", 
												gEndOfLine);
		ListString ((char*)gTextString3, numberChars, gOutputTextH);	
*/
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#if defined multispec_lin
	#include "CFileStream.h"
	#include "LImageDoc.h"
	#include "LImageView.h"
	#include "wx/wx.h"
#endif	// defined multispec_lin
	
#if defined multispec_mac || defined multispec_mac_swift
	//#include 	"WASTE.h"
	#define	IDS_KML1				1
	#define	IDS_KML2				2
	#define	IDS_KML3				3
	#define	IDS_KML4				4
	#define	IDS_KML5				5
	#define	IDS_KML6				6
	#define	IDS_KML7				7
	#define	IDS_KML8				8
	#define	IDS_KML9				9
	#define	IDS_KML10			10
	#define	IDS_KML11			11
	
	#define	IDS_Alert138		138
	#define	IDS_Alert139		139
	#define	IDS_Alert140		140
	
	#define	IDS_FileIO193		193	
#endif	// defined multispec_mac || defined multispec_mac_swift

#if defined multispec_win
	#include "CFileStream.h"
	#include "CPalette.h"	 
#endif	// defined multispec_win 

//#include "SExtGlob.h"

#define	kKMLStrID								174

#define	kMacNewLinePosMode					0x0d81
#define	kIBMNewLinePosMode					0x0a81

#define	kWriteGeoInformation					0
#define	kGetNumberEntriesOnly				1

#define	kDoNotIncludeUTM						0
#define	kIncludeUTM								1


extern UInt16 GetPaletteEntry (
				UInt16								paletteIndex);
										
extern Boolean WriteKMLFile (
				FileInfoPtr							fileInfoPtr, 
				CMFileStream*						fileStreamPtr,
				UInt32								columnInterval,
				UInt32								lineInterval,
				UInt32								startMapColumn,
				UInt32								startMapLine,
				UInt32								endMapColumn,
				UInt32								endMapLine);

               
			// Prototypes for routines in this file that are only called by		
			// other routines in this file.	
							
Boolean 	GetGroupInfoFile (
				Handle								inputFileInfoHandle,
				FileInfoPtr							outputFileInfoPtr);

UInt32	GetNumberStripsToUse (
				SInt16								planarConfiguration,
				UInt32								numberColumns,
				UInt32								numberLines,
				UInt32								numberChannels,
				UInt32								numberBytes,
				UInt32*								numberStripsPtr,
				UInt32*								numberStripsPerChannelsPtr,
				UInt32*								numberBytesPerStripPtr,
				UInt32*								numberBytesInLastStripPtr);
							
Boolean 	GetThematicSupportFile (
				FileInfoPtr							fileInfoPtr);

UInt32 	LoadGeoDoubleValue (
				char*									geoTiffInfoBufferPtr,
				UInt32								index,
				double								inputValue);

UInt32 	LoadGeoKeyDirectory (
				FileInfoPtr							fileInfoPtr,
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				CharPtr								bufferPtr,
				CharPtr								doubleParametersBufferPtr,
				UInt32*								doubleParameterBytePtr,
				CharPtr								asciiParametersBufferPtr,
				UInt32*								numberASCIIParameterBytesPtr);

void 		LoadGeoKeyEntry (
				UInt16*								geoKeyRecordPtr,
				UInt16								entry0,
				UInt16								entry1,
				UInt16								entry2,
				UInt16								entry3);

UInt32 	LoadGeoModelTiePoints (
				FileInfoPtr 						fileInfoPtr,
				UInt32								numberControlPoints,
				char*									geoTiffInfoBufferPtr,
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				UInt32								index,
				UInt32								startMapLeft,
				UInt32								startMapTop);

UInt32	LoadGeoModelTransformationParameters (
				FileInfoPtr 						fileInfoPtr,
				char*									geoTiffInfoBufferPtr,
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				UInt32								index);

UInt16	LoadInstrumentNameToTIFFAsciiTag (
				FileInfoPtr 						fileInfoPtr,
				SInt16								tiffSourceCode,
				char*									stringPtr);
							
void 		LoadThematicClasses (
				FileInfoPtr							fileInfoPtr);

void 		LoadTiffEntry (
				TIFF_IFD_Ptr						imageFileDirectoryPtr,
				UInt16								tag,
				SInt16								type,
				UInt32								count,
				UInt32								value);
							
void 		LoadThematicGroups (
				FileInfoPtr							fileInfoPtr);

void 		LoadTIFFColorBuffer (
				FileInfoPtr							gisFileInfoPtr,
				ColorSpec*							colorSpecPtr,
				SInt16*								classPtr,
				UInt32								numberListClasses,
				UInt16*								paletteIndexPtr,
				SInt16								paletteOffset, 
				UInt32								numberTRLClasses,
				UInt16*								blueVectorPtr,
				UInt16*								greenVectorPtr,
				UInt16*								redVectorPtr,
				SInt16								collapseClassCode,
				SInt16								classNameCode);
							
Boolean 	ReadOffsetGainFile (
				CMFileStream*						fileStreamPtr,
				SInt16*								versionNumberPtr,
				Boolean*								fileFoundFlagPtr,
				SInt16*								returnCodePtr);
							
Boolean 	ReadTransformationFile (
				CMFileStream*						fileStreamPtr,
				SInt16*								versionNumberPtr,
				Boolean*								fileFoundFlagPtr,
				SInt16*								errorCodePtr);

SInt16	ReadTransformationFileSetSwapInfo (
				UInt32								fileSize,
				unsigned char						createdByCodeByte,
				SInt16*								numberChannelsPtr,
				SInt16*								numberFeaturesPtr,
				SInt16*								versionNumberPtr,
				Boolean*								readBandGroupingFlagPtr,
				SInt16*								createdByCodePtr,
				UInt32*								computedFileSize1Ptr,
				UInt32*								computedFileSize2Ptr,
				UInt32*								computedFileSize3Ptr,
				Boolean*								swapFlagPtr);

void		ReadTransformationGetFileSizes (
				UInt32								fileSize,
				unsigned char						createdByCodeByte,
				SInt16								numberChannels,
				SInt16								numberFeatures,
				SInt16*								versionNumberPtr,
				Boolean*								readBandGroupingFlagPtr,
				SInt16*								createdByCodePtr,
				UInt32*								computedFileSize1Ptr,
				UInt32*								computedFileSize2Ptr,
				UInt32*								computedFileSize3Ptr);

Boolean 	WriteArcViewWorldFile (
				FileInfoPtr 						fileInfoPtr);

SInt16 	WriteArcViewMapInformation (
				FileInfoPtr 						fileInfoPtr,
				CMFileStream*						headerStreamPtr,
				Boolean*								planarCoordinateInfoFlagPtr);

Boolean 	WriteArcViewHeaderFile (
				FileInfoPtr 						fileInfoPtr);

SInt16 	WriteGeoTIFFInformation (
				FileInfoPtr 						fileInfoPtr,
				CMFileStream*						fileStreamPtr,
				UInt32								directoryStart,
				UInt32								directoryDataStart,
				Boolean								countOnlyFlag,
				UInt32*								numberGeoEntriesPtr,
				UInt32*								numberDataBytesPtr,
				UInt32								displayedColumnInterval,
				UInt32								displayedLineInterval,
				UInt32								startMapLeft,
				UInt32								startMapTop);
								
SInt16	 WriteTIFFColorMap (
				CMFileStream* 						fileStreamPtr,
				CMPaletteInfo						paletteObject,
				UInt16								paletteOffset);

SInt16 	WriteTIFFColorMap2 (
				FileInfoPtr							fileInfoPtr,
				CMFileStream* 						fileStreamPtr,
				SInt16								tiffSourceCode,
				CMPaletteInfo						paletteObject,
				UInt16								paletteOffset,
				SInt16								thematicListType,
				SInt16								paletteSelection);
								
SInt16	 WriteTIFFImageData (
				CMFileStream* 						fileStreamPtr,
				Rect*									areaRectanglePtr,
				UInt16								paletteOffset);

Boolean 	WriteTextOutputFile (
				SInt16     							refNum, 
				Str255*								fileNamePtr,
				#if use_mlte_for_text_window
					TXNObject							wsTextH,
				#endif 
				#if !use_mlte_for_text_window
					WEReference 						wsTextH,
				#endif 
				SInt32   							beginText, 
				SInt32   							endText);



//------------------------------------------------------------------------------------
//								 	Copyright (1992-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 FindEndOfLineCode
//
//	Software purpose:	The purpose of this routine is to find the end of line 
//							character that is being used by the input file. The combinations
//							are:
//								Mac:    0x0d
//								IBM PC: 0x0d0a
//								Unix:   0x0a
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	The new input string pointer.
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/15/1999
//	Revised By:			Larry L. Biehl			Date: 03/16/2017

SInt16 FindEndOfLineCode (
				ParmBlkPtr							paramBlockPtr,
				CMFileStream*						fileStreamPtr,
				SInt16*								errCodePtr,
				SInt16*								endOfLineCodePtr,
				UInt32*								numberEndOfLineBytesPtr)

{
	UCharPtr								bufferPtr,
											newLinePtr;
	
	UInt32								count;
	 
	SInt16								endOfLineCharCode = kMacNewLinePosMode,
											errCode = noErr;
	
	
	if (endOfLineCodePtr != NULL)
		*endOfLineCodePtr = 0;
		
	if (numberEndOfLineBytesPtr != NULL)
		*numberEndOfLineBytesPtr = 1;
	
	#if defined multispec_mac	
		count = paramBlockPtr->ioParam.ioReqCount;
		bufferPtr = (UCharPtr)paramBlockPtr->ioParam.ioBuffer;
	#endif	// defined multispec_mac
		
   #if defined multispec_win | defined multispec_lin	
		count = paramBlockPtr->ioReqCount;
		bufferPtr = paramBlockPtr->ioBuffer;	
	#endif	// defined multispec_win | defined multispec_lin
	/*	
	#if defined multispec_mac
		IOParamPtr							ioParamPtr;
		SInt32								savedPosOffset;
		SInt16								savedPosMode;
	
	
		bufferPtr = paramBlockPtr->ioParam.ioBuffer;
		
				// Save the position offset in case the file needs to be reread with
				// a different end of line character.
		
		savedPosOffset = ioParamPtr->ioPosOffset;
		savedPosMode =  ioParamPtr->ioPosMode;
		
				// Check if end of line character is "0x0d"
	
		ioParamPtr->ioPosMode = kMacNewLinePosMode; 
		errCode = PBReadSync ((ParmBlkPtr)paramBlockPtr);
		
		if (errCode == noErr && ioParamPtr->ioBuffer[ioParamPtr->ioActCount-1] != 0x0d)
			{
					// Check if end of line character is "0x0d0a"
			
			ioParamPtr->ioPosOffset = savedPosOffset;
			ioParamPtr->ioPosMode = kIBMNewLinePosMode; 
			errCode = PBReadSync ((ParmBlkPtr)paramBlockPtr);
		
			if (errCode == noErr && 
									ioParamPtr->ioBuffer[ioParamPtr->ioActCount-1] != 0x0a)
					// Force end of line character to be "0d" and see what happens.
					// Don't know what the end of line character is.
					
				ioParamPtr->ioPosMode = kMacNewLinePosMode; 
			
			}	// end "if (errCode == noErr && ..."
			
		ioParamPtr->ioPosOffset = savedPosOffset;
		
		endOfLineCode = ioParamPtr->ioPosMode;
	#endif	// defined multispec_mac
	*/
	errCode = MSetMarker (fileStreamPtr, 
									fsFromStart, 
									0,
									kNoErrorMessages);
          
	if (errCode == noErr)
		{                       
				// Read in a buffer of data.
				
		errCode = MReadData (fileStreamPtr, 
									&count, 
									bufferPtr,
									kNoErrorMessages);
				
		if (errCode == eofErr)
			errCode = noErr;		
									
		}	// errCode == noErr
          
	if (errCode == noErr && count > 2)
		{
				// Allow for first two characters to be a carriage return and
				// line feed and skip them. This will insure that we do not back up
				// before bufferPtr actually starts later on.
				
		bufferPtr += 2;                        
		newLinePtr = SkipToNextCarriageReturn (bufferPtr);
		
		if (newLinePtr != NULL)
			{
					// This should be just after the end of line characters.
					// Back up to see what the end of line characters are.
					
			newLinePtr--;
			
			if (*newLinePtr == kCarriageReturn && endOfLineCodePtr != NULL)
				*endOfLineCodePtr = kMacNewLinePosMode;
			
			else if (*newLinePtr == kNewLineCharacter)
				{
						// Now backup one more and see if it is a carriage return. If
						// it is not then this may be a file transferred via unix.
						// Use the new line character as the end of line character in
						// the unix case return 0 as the "end of line code".
				
				newLinePtr--;		
				if (*newLinePtr == kCarriageReturn)
					{
					if (endOfLineCodePtr != NULL)
						*endOfLineCodePtr = kIBMNewLinePosMode;
						
					if (numberEndOfLineBytesPtr != NULL)
						*numberEndOfLineBytesPtr = 2;
					
					}	// end "if (*newLinePtr == kCarriageReturn)"
				
				else	// *newLinePtr != kCarriageReturn
					endOfLineCharCode = kIBMNewLinePosMode;
				
				}	// end else if (*newLinePtr == kNewLineCharacter)
					
			}	// end "if (newLinePtr != NULL)"
									
		}	// errCode == noErr
	
	*errCodePtr = errCode;
	
	return (endOfLineCharCode);
		
}	// end "FindEndOfLineCode" 


                   
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetGroupInfoFile
//
//	Software purpose:	The purpose of this routine is to allow the user to select
//							the image file that contains the group information that
//							is to be used.
//
//	Parameters in:		.
//
//	Parameters out:	None
//
//	Value Returned:	TRUE if file was selected
//							FALSE if not file was selected.			
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 11/13/1996
//	Revised By:			Larry L. Biehl			Date: 02/07/2018

Boolean GetGroupInfoFile (
				Handle								inputFileInfoHandle,
				FileInfoPtr							outputFileInfoPtr)

{								
	CMFileStream*						fileStreamPtr;
	FileInfoPtr							inputFileInfoPtr = NULL;
	FileStringPtr						fileNamePtr;
	
	SInt16								errCode,
											numberFileTypes,
											promptString;
	
	Boolean								continueFlag,
											fileFlag,
											fileInfoLoadedFlag,
											doneFlag;
	
		
	doneFlag = FALSE;
	
	if (inputFileInfoHandle != NULL)
		{
				// If the option key was down when the 'Open Image' menu item		
				// was selected then include all files in the file list.  			
				// Otherwise just show the selected file types.							
				
		numberFileTypes = gNumberFileTypes; 
		#if defined multispec_mac    
			if (gEventRecord.modifiers & optionKey)
				numberFileTypes = -1;
		#endif	// defined multispec_mac 
				
		while (!doneFlag)
			{
					// Initialize local variables.											
					
			promptString = IDS_FileIO77;
			fileInfoLoadedFlag = FALSE;
			continueFlag = FALSE;
		
					// Initialize the variables and handles in the structure.		
				
			InitializeFileInfoStructure (inputFileInfoHandle, kNotPointer);
			
			inputFileInfoPtr = (FileInfoPtr)GetHandlePointer (
													inputFileInfoHandle,
													kLock);
			
			fileStreamPtr = GetFileStreamPointer (inputFileInfoPtr);
		
					// Indicate that the image type is to be determined from the 	
					// header part of the file.												
				
			gGetFileImageType = kThematicImageType;
		
			errCode = GetFile (fileStreamPtr, 
										numberFileTypes, 
										gListTypes, 
										NULL, 
										NULL,
										NULL,
										promptString);
						
    		fileFlag = ((errCode == noErr) & FileExists (fileStreamPtr));
				
			CheckAndUnlockHandle (inputFileInfoHandle);
	    	
	    	if (fileFlag)
	    		{
						// Determine whether an image file which may have group info at the 
						// end of the file or a .clr file.
		
				fileNamePtr = 
						(FileStringPtr)GetFilePathPPointerFromFileStream (fileStreamPtr);
						
				if (CompareSuffixNoCase ((char*)"\0.clr\0", fileNamePtr, NULL))
					{
					SetType (fileStreamPtr, kICLRFileType);
					inputFileInfoPtr->ancillaryInfoformat = kArcViewDefaultSupportType;
					doneFlag = TRUE;
					
					}	// end "if (CompareSuffixNoCase ("\0.clr\0", suffixStringPtr, NULL)"
						
				else	// must be an image file with group info at end.
					{
							// Determine the file format and load the header 				
							// information.															
							
					fileInfoLoadedFlag = LoadImageInformation (
														NULL, inputFileInfoHandle, FALSE, NULL, 0);
			
							// If the file information was loaded, verify that the number of
							// classes in this file is the same as for the currently
							// active image window.																		
							
					if (fileInfoLoadedFlag)
						{
						if (inputFileInfoPtr->numberClasses == 
																	outputFileInfoPtr->numberClasses)
							doneFlag = TRUE;
							
						else	// inputFileInfoPtr->numberClasses != ...
							DisplayAlert (kErrorAlertID, 
												kCautionAlert, 
												kAlertStrID, 
												IDS_Alert128,
												0, 
												NULL);
							
						}	// end "if (fileInfoLoadedFlag)" 
						
					}	// end "else must be an image file with group info at end."
				
	      	}	// end "if (fileFlag)"
	      	
	      else	// !fileFlag
	      	{
	      	inputFileInfoPtr = NULL;
				doneFlag = TRUE;
	      	
	      	}	// end "else !fileFlag"
				
			}	// end "while (!doneFlag)"
      										
      }	// end "if (fileInfoHandle != NULL)"
      
	return (inputFileInfoPtr != NULL);
	
}	// end "GetGroupInfoFile"  



//------------------------------------------------------------------------------------
//								 		Copyright (1992-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		char* GetNextLine
//
//	Software purpose:	This routine finds the start of the next line in the project 
//							file. If there is no next line in the buffer, then a buffer is 
//							read from the file. 
//							Note that for the Mac a new line is read each time.
//							For Linux and Windows, we have to check for end of line
//							characters.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	The new input string pointer.
//
// Called By:			ConvertROIPixelsToClassNumbers in SFieldsT.cpp
//							ReadCovarianceInformation in SProjectFileIO.cpp
//							ReadModifiedStats in SProjectFileIO.cpp
//							ReadProjectFile in SProjectFileIO.cpp
//							ReadOffsetGainFile in SSaveWrite.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/15/1995
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

SInt16 GetNextLine (
				ParmBlkPtr							paramBlockPtr,
				UCharPtr*							inputStringPtrPtr)

{
	SInt16								errCode = noErr;
	
	
	#if defined multispec_mac	
		IOParamPtr							ioParamPtr;
	
	
		ioParamPtr = &paramBlockPtr->ioParam;
	
		errCode = PBReadSync ((ParmBlkPtr)paramBlockPtr);
			
		if (errCode == noErr && 
					paramBlockPtr->ioParam.ioActCount == 1 && 
												ioParamPtr->ioBuffer[0] == kCarriageReturn)
			{
					// This is special case when files are copied from Windows to
					// Unix to Mac (and vice versa) and some of the transfers are
					// done as binary and some as text. One can end up with a double
					// carriage return. We need to read another line.
			
			errCode = PBReadSync ((ParmBlkPtr)paramBlockPtr);
			
			}	// end "if (...->ioParam.ioActCount == 1 && ..."
		
				// Allow for last line not having an end of line character.
				
		if (errCode == eofErr && paramBlockPtr->ioParam.ioActCount > 0)
			errCode = noErr;
							
		*inputStringPtrPtr = (UCharPtr)ioParamPtr->ioBuffer;
				
				// Make sure that the beginning of 'newLinePtr' is not a line 
				// feed character.
				
		if (ioParamPtr->ioBuffer[0] == kNewLineCharacter)
			(*inputStringPtrPtr)++;
		
		ioParamPtr->ioBuffer[paramBlockPtr->ioParam.ioActCount] = kNullTerminator;	
	#endif	// defined multispec_mac
	
   #if defined multispec_win | defined multispec_lin  	   
	   UCharPtr 				bufferPtr,
	   							lastLinePtr,
	   							newLinePtr,
	   							nextLinePtr;
	   					
	   UInt32					count,
	   							numberCharsToMove;
	   					
	   
		newLinePtr = lastLinePtr = *inputStringPtrPtr;
		numberCharsToMove = 0;
		
		if (lastLinePtr != NULL && 
				(paramBlockPtr->ioPosMode == kNewLinePosMode ||
								paramBlockPtr->ioPosMode == kIBMNewLinePosMode))
			{                   
					// Find the start of the next line.
								
			newLinePtr = SkipToNextCarriageReturn (lastLinePtr);
			
			if (newLinePtr != NULL && *newLinePtr == kCarriageReturn)
				{
						// This is special case when files are copied from Windows to
						// Unix to Mac (and vice versa) and some of the transfers are
						// done as binary and some as text. One can end up with a double
						// carriage return. We need to read another line.
				
				newLinePtr++;
				
				}	// end "if (newLinePtr != NULL && *newLinePtr == ..."
			
					// Verify that the current buffer still includes a complete
					// line that has not been used.
					
			nextLinePtr = SkipToNextCarriageReturn (newLinePtr);
			
			if (nextLinePtr == NULL)
				{      
						// Move unused character to beginning of the buffer
						// and ready to read next set of characters from the file.
				
				if (newLinePtr != NULL)
					{
					numberCharsToMove = (UInt32)(paramBlockPtr->ioActCount - 
												(newLinePtr - paramBlockPtr->ioBuffer));	
					memmove (paramBlockPtr->ioBuffer, newLinePtr, (size_t)numberCharsToMove);
					
					}	// end "if (newLinePtr != NULL)"
					
				else	// newLinePtr == NULL
					numberCharsToMove = 0;
				
				bufferPtr = paramBlockPtr->ioBuffer + numberCharsToMove;

				if (paramBlockPtr->ioReqCount > numberCharsToMove)
					count = paramBlockPtr->ioReqCount - numberCharsToMove;
				else
					count = 0;
				
						// Indicate that the file needs to be read.
						
				newLinePtr = NULL;  
				
				}	// end "if (nextLinePtr == NULL)" 
			
			}	// end "if (lastLinePtr != NULL && ..."
			
		else	// *inputStringPtr == NULL || ...
			{
			bufferPtr = paramBlockPtr->ioBuffer;
			count = paramBlockPtr->ioReqCount;
			                                        
			if (paramBlockPtr->ioPosMode != kNewLinePosMode &&
								paramBlockPtr->ioPosMode != kIBMNewLinePosMode)
				newLinePtr = NULL;
			
			}	// end "else *inputStringPtr == NULL"
	
		if (newLinePtr == NULL)
			{                                        
			if (paramBlockPtr->ioPosOffset == 0 ||
										(paramBlockPtr->ioPosMode != kNewLinePosMode &&
								(paramBlockPtr->ioPosMode == kIBMNewLinePosMode ||
								paramBlockPtr->ioPosMode == fsFromStart)))
				{           
				errCode = MSetMarker (paramBlockPtr->fileStreamPtr, 
												fsFromStart, 
												paramBlockPtr->ioPosOffset,
												kNoErrorMessages);
				
				}	// end "	if (paramBlockPtr->ioPosOffset == 0 || ..."
			          
			if (errCode == noErr)
				{                       
						// Read in the next buffer of data.
				
				if (count > 0)
					errCode = MReadData (paramBlockPtr->fileStreamPtr, 
												&count, 
												bufferPtr,      
												kNoErrorMessages);
											
				paramBlockPtr->ioActCount = numberCharsToMove + count;
				paramBlockPtr->ioPosOffset += count;
				 
				paramBlockPtr->ioBuffer[paramBlockPtr->ioActCount] = kNullTerminator;
				
				}	// end "if (errCode == noErr)" 
				
			if (errCode == eofErr && paramBlockPtr->ioActCount > 0)
				errCode = noErr; 
				
			if (errCode == noErr)
				{ 
				newLinePtr = paramBlockPtr->ioBuffer;
				
						// Make sure that the beginning of 'newLinePtr' is not a carriage
						// return or line feed character. This may occur if no characters
						// were moved from the last buffer.
						
				if (numberCharsToMove == 0)
					{		
					if (*newLinePtr == kCarriageReturn)
						newLinePtr++;      
							
					if (*newLinePtr == kNewLineCharacter)
						newLinePtr++;
						
					}	// end "if (numberCharsToMove == 0)"
				
				}	// end "if (errCode == noErr)"
					
			}	// end "if (newLinePtr == NULL)"
			
		*inputStringPtrPtr = newLinePtr;	
	#endif	// defined multispec_win | defined multispec_lin
	
	return (errCode);
		
}	// end "GetNextLine"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//							  (c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetNumberStripsToUse
//
//	Software purpose:	The purpose of this routine is to determine the number of strips
//							to use to write out the tiff/geotiff data. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			
//							
//
//	Coded By:			Larry L. Biehl			Date: 05/21/2012
//	Revised By:			Larry L. Biehl			Date: 07/16/2012

UInt32 GetNumberStripsToUse (
				SInt16								planarConfiguration,
				UInt32								numberColumns,
				UInt32								numberLines,
				UInt32								numberChannels,
				UInt32								numberBytes,
				UInt32*								numberStripsPtr,
				UInt32*								numberStripsPerChannelPtr,
				UInt32*								numberBytesPerStripPtr,
				UInt32*								numberBytesInLastStripPtr)

{
	SInt64								numberBytesPerChannel,
											totalNumberBytes;
	
	UInt32								numberBytesInLastStrip,
											numberBytesLimitPerStrip = 4194304,		// 2^^22
											numberBytesPerStrip,
											numberLinesPerStrip,
											numberStrips = 1,
											numberStripsPerChannel = 1;
	
	
	numberBytesPerChannel = (SInt64)numberColumns * numberLines * numberBytes;
	totalNumberBytes = numberBytesPerChannel * numberChannels;
	numberLinesPerStrip = numberLines;
	
	if (totalNumberBytes <= UInt32_MAX)
		{
		numberBytesPerStrip = (UInt32)totalNumberBytes;
		numberBytesInLastStrip = numberBytesPerStrip;
		
		if (planarConfiguration == 2)
			{
			numberStrips = numberChannels;
			numberBytesPerStrip = (UInt32)numberBytesPerChannel;
			numberBytesInLastStrip = numberBytesPerStrip;
			
			}	// end "if (planarConfiguration == 2)"
																
		if (numberBytesPerStrip > numberBytesLimitPerStrip)
			{
					// Get the number of lines per strip. The way the code is set up, the
					// maximum number of lines per strip will be 128
					
			numberLinesPerStrip = 128;
			numberBytesPerStrip = numberColumns * numberLinesPerStrip * numberBytes;		
			if (planarConfiguration == 1)
				numberBytesPerStrip *= numberChannels;
			
			while (numberBytesPerStrip > numberBytesLimitPerStrip)
				{
				if (numberLinesPerStrip > 1)
					numberLinesPerStrip /= 2;
					
				numberBytesPerStrip = numberColumns * numberLinesPerStrip * numberBytes;
																
				if (planarConfiguration == 1)
					numberBytesPerStrip *= numberChannels;
					
				if (numberLinesPerStrip == 1)
					break;
				
				}	// end "while (numberBytesPerStrip > numberBytesLimitPerStrip)"
				
					// Now get the number of strips needed.
					
			numberStrips = 
					(UInt32)floor (((double)numberLines + numberLinesPerStrip - 1)/numberLinesPerStrip);
			
			if (planarConfiguration == 2 && numberChannels > 1)
				{
						// Need to determine on a channel basis
						
				numberStripsPerChannel = numberStrips;
				
				//numberStripsPerChannel = numberBytesPerChannel/numberBytesPerStrip;
				//if (numberStripsPerChannel * numberBytesPerStrip < numberBytesPerChannel)
				//	numberStripsPerChannel++;
					
				numberBytesInLastStrip = (UInt32)(numberBytesPerChannel - 
											(numberStripsPerChannel - 1) * numberBytesPerStrip);
					
				numberStrips = numberStripsPerChannel * numberChannels;
				
				}	// end "if (numberChannels == 1)"
				
			else	// bandInterleaveCode != kBSQ || numberChannels == 1
				{
				//numberStrips = totalNumberBytes/numberBytesPerStrip;
				//if (numberStrips * numberBytesPerStrip < totalNumberBytes)
				//	numberStrips++;	
					
				numberBytesInLastStrip = (UInt32)(totalNumberBytes - 
														(numberStrips - 1) * numberBytesPerStrip);
												
				numberStripsPerChannel = numberStrips;
				 
				}	// end "else planarConfiguration != 2 || numberChannels == 1"
			
			}	// end "if (totalNumberBytes > numberBytesLimitPerStrip)"
			
		*numberStripsPtr = numberStrips;
		*numberStripsPerChannelPtr = numberStripsPerChannel;
		*numberBytesPerStripPtr = numberBytesPerStrip;
		*numberBytesInLastStripPtr = numberBytesInLastStrip;
		
		}	// end "if (totalNumberBytes <= UInt32_MAX)"
		
	else	// totalNumberBytes > UInt32_MAX
		{
			// This file will have to be handle by BIGTIFF format.
			// For now this will be set up as 1 (very large strip). MultiSpec will be 
			// able to read it because of the way the read function is set up but few
			// other applications will be able to read it.
		
		*numberStripsPtr = 1;
		*numberStripsPerChannelPtr = 1;
		*numberBytesPerStripPtr = 0;
		*numberBytesInLastStripPtr = 0;
			
		}	// end "else totalNumberBytes > UInt32_MAX"

	return (numberLinesPerStrip);

}	// end "GetNumberStripsToUse"

	

//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetThematicSupportFile
//
//	Software purpose:	This routine allows the user to select the the image support
//							file for the active thematic image window that contains the class 
//							names and colors. This can either be an ERDAS GIS .trl file or an 
//							ArcView .clr file.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/11/1996
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

Boolean GetThematicSupportFile (
				FileInfoPtr							fileInfoPtr)

{
	SInt64								fileSize;
	CMFileStream*						supportFileStreamPtr; 
	char*									erdasStatHeader = (char*)"TRAIL";
	FileStringPtr						supportFileNamePtr;
	
	UInt32								count,
											minimumNumberBytes;
	
	SInt16								errCode,
											numberFileTypes,
											stringCompare;
	
	OSType								fileType[2];
	
	Boolean								continueFlag;
	
	SignedByte							handleStatus;
	
	
	continueFlag = TRUE;
	
			// Verify that a handle to the window information structure for
			// the active image window exists.				
	                                                      
	if (GetActiveImageWindowInfoHandle () == NULL)
																							return (FALSE);
																				
			// Get the minimum number of bytes for an ERDAS GIS trailer file.
			
	minimumNumberBytes = 7*128;
	minimumNumberBytes += 9*128;
	
	if (fileInfoPtr->numberClasses < 256)
		minimumNumberBytes += ((fileInfoPtr->numberClasses+4)/4) * 128;
		
	else	// fileInfoPtr->numberClasses >= 256
		minimumNumberBytes += (256/4) * 128;
			
	supportFileStreamPtr = GetActiveSupportFileStreamPointer (&handleStatus);
					
	if (supportFileStreamPtr != NULL)
		{        
				// Get the file name and volume to read the thematic support file 		
				// from.	First make sure that we have a pointer cursor.				

		MInitCursor ();
		
		fileType[0] = kITRLFileType;
		fileType[1] = kICLRFileType;
		numberFileTypes = 2;  
		#if defined multispec_mac    
			if (gEventRecord.modifiers & optionKey)
				numberFileTypes = -1; 
		#endif	// defined multispec_mac    
			
		stringCompare = -1;
		
		do
			{
			errCode = GetFile (supportFileStreamPtr, 
										numberFileTypes, 
										fileType, 
										NULL, 
										NULL,
										NULL, 
										IDS_FileIO76);
										
			continueFlag = (errCode == noErr) & FileExists (supportFileStreamPtr);
		
					// If file is set up to read image statistics from, check if	
					// file is of correct format.  Check for 'TRAIL' in first 5		
					// bytes																			
				
			if (continueFlag)
				{
						// Determine whether a .trl file or a .clr file.
		
				supportFileNamePtr = 
					(FileStringPtr)GetFilePathPPointerFromFileStream (supportFileStreamPtr);
						
				if (CompareSuffixNoCase ((char*)"\0.clr\0", supportFileNamePtr, NULL))
					{
					stringCompare = 0;
					SetType (supportFileStreamPtr, kICLRFileType);
					
					}	// end "if (CompareSuffixNoCase ("\0.clr\0", suffixStringPtr, NULL)"
						
				else	// must be erdas support file
					{
					count = 7;
					errCode = MReadData (supportFileStreamPtr, 
												&count, 
												gTextString,
												kErrorMessages);                           
					continueFlag = (errCode == noErr);
					if (continueFlag)
						stringCompare = strncmp ((char*)gTextString, erdasStatHeader, 5);
						
					if (stringCompare == 0)
						{
								// Make certain that the file is of the correct size.		
						
						errCode = GetSizeOfFile (supportFileStreamPtr, &fileSize);
						continueFlag = (errCode == noErr);
						if (continueFlag)
							{
							if ((numberFileTypes == 1 && fileSize < minimumNumberBytes) ||
									(numberFileTypes == -1 && fileSize < minimumNumberBytes))
								stringCompare = -2;
								
							}	// end "if (continueFlag)"
						
						}	// end "if (stringCompare == 0)" 
						
					if (stringCompare == 0)
						SetType (supportFileStreamPtr, kITRLFileType);
						
					else	// stringCompare != 0 
						{
								// Beep to alert user that file is not an image 				
								// statistics file that matches the image parameters.			

						SysBeep (10);               
						CloseFile (supportFileStreamPtr);
					
						if (stringCompare == -2)
							DisplayAlert (kErrorAlertID, 
												kCautionAlert, 
												kAlertStrID, 
												IDS_Alert111,
												0, 
												NULL);
						
						}	// end "else stringCompare != 0"
						
					}	// end "else must be erdas support file"
					
				}	// end "if (continueFlag)" 
			
			}		while (continueFlag & (stringCompare != 0));
		
		UnlockActiveSupportFileStream (handleStatus);

		}	// end "if (supportFileStreamPtr != NULL)" 

	else		
		continueFlag = FALSE;
  	
	return (continueFlag);
	
}	// end "GetThematicSupportFile" 
							


//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32 LoadGeoDoubleValue
//
//	Software purpose:	This routine loads the input double value into the geoTIFF
//							double value list.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/27/2007
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

UInt32 LoadGeoDoubleValue (
				char*									geoTiffInfoBufferPtr,
				UInt32								index,
				double								inputValue)

{
				// Swap the bytes if needed.
				
	inputValue = GetDoubleValue ((UCharPtr)&inputValue);
	BlockMoveData (&inputValue, &geoTiffInfoBufferPtr[index], 8);
	index += 8;
		
	return (index);
	
}	// end "LoadGeoDoubleValue" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32 LoadGeoKeyDirectory
//
//	Software purpose:	This routine writes the geo key directory information to the 
//							TIFF image file.
//
//	Parameters in:		file information pointer
//							pointer to header record information
//
//	Parameters out:	None
//
//	Value Returned:	
//
// Called By:			WriteTIFFImageFile in SSaveWrt.cpp
//
//	Coded By:			Larry L. Biehl			Date: 09/11/2002
//	Revised By:			Larry L. Biehl			Date: 08/11/2013

UInt32 LoadGeoKeyDirectory (
				FileInfoPtr							fileInfoPtr,
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				CharPtr								bufferPtr,
				CharPtr								doubleParametersBufferPtr,
				UInt32*								doubleParameterBytePtr,
				CharPtr								asciiParametersBufferPtr,
				UInt32*								numberASCIIParameterBytesPtr)

{
	char									asciiString[1024];
	
	UInt16*								geoKeyDirectoryFieldPtr;
	
	double								geogSemiMajorAxisGeoKey,
											geogSemiMinorAxisGeoKey,
											projAzimuthAngleGeoKey,
											projCenterLatGeoKey,
											projCenterLongGeoKey,
											projFalseEastingGeoKey,
											projFalseNorthingGeoKey,
											projFalseOriginEastingGeoKey,
											projFalseOriginLatGeoKey,
											projFalseOriginLongGeoKey,
											projFalseOriginNorthingGeoKey,
											projNatOriginLatGeoKey,
											projNatOriginLongGeoKey,
											projScaleAtNatOriginGeoKey,
											projStdParallel1GeoKey,
											projStdParallel2GeoKey,
											projStraightVertPoleLongGeoKey,
											tempDoubleValue;
	
	UInt32								doubleParameterBuffIndex,
											index,
											numberASCIIParameterBuffIndex;
											
	SInt16								gridZone;
	
	UInt16								count,
											datumCode,
											doubleParameterIndex,
											geogAngularUnitsGeoKey,
											geogEllipsoidGeoKey,
											geogGeodeticDatumGeoKey,
											geographicTypeGeoKey,
											geoKeyDirectoryField[4],
											geogLinearUnitsGeoKey,
											geogPrimeMeridianGeoKey,
											modelTypeGeoKey,
											projCoordTransGeoKey,
											projectedCSTypeGeoKey,
											projectionGeoKey,
											projLinearUnitsGeoKey,
											referenceSystemCode;
											
	Boolean								geogCitationGeoKeyFlag;
	
	
			// Initialize flag indicating whether the GeogCitationGeoKey will be 
			// required.
			
	geogCitationGeoKeyFlag = FALSE;
	
	referenceSystemCode = mapProjectionInfoPtr->gridCoordinate.referenceSystemCode;
	datumCode = mapProjectionInfoPtr->geodetic.datumCode;
	gridZone = mapProjectionInfoPtr->gridCoordinate.gridZone;
	
			// Get model type.
			
	modelTypeGeoKey = 1;		// Projection Coordinate System.
	if (referenceSystemCode == kGeographicRSCode)
		modelTypeGeoKey = 2;
	
			// Find if a ProjectedCSTypeGeoKey can be determined.
			// ProjectedCSTypeGeoKey: 3072
			
	projectedCSTypeGeoKey = 0;
	if (referenceSystemCode == kGGRS87RSCode)
		projectedCSTypeGeoKey = 2100;
		
	else if (referenceSystemCode == kUTM_WGS72RSCode && gridZone != 0)
		{
		projectedCSTypeGeoKey = 
							32200 + abs (gridZone);
	
		if (gridZone < 0)
			projectedCSTypeGeoKey += 100;
				
		}	// end  "if (referenceSystemCode == kUTM_WGS72RSCode && ..."
		
	else if (referenceSystemCode == kUTM_WGS84RSCode && gridZone != 0)
		{
		projectedCSTypeGeoKey = 
							32600 + abs (gridZone);
	
		if (gridZone < 0)
			projectedCSTypeGeoKey += 100;
			
		}	// end  "else if (referenceSystemCode == kUTM_WGS84RSCode && ..."
		
	else if (referenceSystemCode == kUTM_NAD27RSCode && 
											gridZone >= 3 && gridZone <= 22)
		projectedCSTypeGeoKey = 26700 + gridZone;
			
		
	else if (referenceSystemCode == kUTM_NAD83RSCode &&
						gridZone >= 3 && gridZone <= 23)
		projectedCSTypeGeoKey = 26900 + gridZone;
			
	else if (referenceSystemCode == kGDA94RSCode &&
						gridZone >= -58 && gridZone <= -48)
		projectedCSTypeGeoKey = 28300 + abs (gridZone);
		
	else if ((referenceSystemCode == kStatePlaneNAD27RSCode || 
							referenceSystemCode == kStatePlaneNAD83RSCode) &&
												gridZone != 0)
		{
		#if include_gdal_capability
					// Get the pcs code			
			projectedCSTypeGeoKey = (UInt16)GetStatePlanePCSCode (datumCode, gridZone);	
		#endif	// include_gdal_capability
		
				// Handle 3 state plane codes for older version with no gdal support.
				
		if (projectedCSTypeGeoKey == 0 && datumCode == kNAD27Code)
			{
			switch (gridZone)
				{
				case 1301:
					projectedCSTypeGeoKey = 26773;
					break;
					
				case 1302:
					projectedCSTypeGeoKey = 26774;
					break;
					
				case 3104:
					projectedCSTypeGeoKey = 32018;
					break;
				
				}	// end "switch (gridZone)"
		
			}	// end "if (projectedCSTypeGeoKey == 0 && datumCode == kNAD27Code)"
		
		else if (projectedCSTypeGeoKey == 0 && datumCode == kNAD83Code)
			{
			switch (gridZone)
				{
				case 1301:
					projectedCSTypeGeoKey = 26973;
					break;
					
				case 1302:
					projectedCSTypeGeoKey = 26974;
					break;
					
				case 3104:
					projectedCSTypeGeoKey = 32118;
					break;
				
				}	// end "switch (gridZone)"
				
			}	// end "else if (projectedCSTypeGeoKey == 0 && datumCode == kNAD83Code)"
			
		}	// end "else if (referenceSystemCode == kStatePlaneNAD27RSCode && ...)"
		
	else if (referenceSystemCode == kGaussKrugerRSCode && gridZone > 0)
		{
				// Or should it be 28460.  I do not know what the difference is.
				
		projectedCSTypeGeoKey = 28400 + gridZone;
		
		}	// end "if (referenceSystemCode == kGaussKrugerRSCode && ..."
		
	else if (referenceSystemCode == kUTM_SAD69RSCode && gridZone != 0)
		{
		projectedCSTypeGeoKey = 29160 + abs (gridZone);
	
		if (gridZone < 0)
			projectedCSTypeGeoKey += 60;
		
		}	// end "else if (referenceSystemCode == kUTM_SAD69RSCode && ..."

	if (projectedCSTypeGeoKey == 0)
		projectedCSTypeGeoKey = mapProjectionInfoPtr->projectedCSTypeGeoKey;
	
	if (projectedCSTypeGeoKey == 0 && 
				mapProjectionInfoPtr->gridCoordinate.projectionCode != 0 &&
						mapProjectionInfoPtr->geodetic.datumCode != 0 &&
								mapProjectionInfoPtr->geodetic.spheroidCode != 0 &&
										mapProjectionInfoPtr->geodetic.semiMajorAxis > 0 &&
												mapProjectionInfoPtr->geodetic.semiMinorAxis > 0)
		projectedCSTypeGeoKey = 32767;
		
			// If Projected CS Type Geo Key not found, then find what is known.
			// GeographicTypeGeoKey: Key ID = 2048
	
	geographicTypeGeoKey = 0;
	if ((projectedCSTypeGeoKey == 0 || projectedCSTypeGeoKey == 32767) && 
												mapProjectionInfoPtr->geodetic.datumCode != 0)
		{	
		if (mapProjectionInfoPtr->geodetic.datumCode == kSphereDatumCode)
			{
			geographicTypeGeoKey = 32767;		// User-defined
			//geographicTypeGeoKey = 4035;		// GCSE_Sphere
			geogCitationGeoKeyFlag = TRUE;
			
			}	// end "if (...->geodetic.datumCode == kSphereDatumCode)"
								
		else if (mapProjectionInfoPtr->geodetic.datumCode == kGGRS87DatumCode)
			geographicTypeGeoKey = 4121;
		else if (mapProjectionInfoPtr->geodetic.datumCode == kNAD83_CSRSCode)
			geographicTypeGeoKey = 4140;
		else if (mapProjectionInfoPtr->geodetic.datumCode == kBeijing1954Code)
			geographicTypeGeoKey = 4214;
		else if (mapProjectionInfoPtr->geodetic.datumCode == kCampoCode)
			geographicTypeGeoKey = 4221;
		else if (mapProjectionInfoPtr->geodetic.datumCode == kNAD27Code)
			geographicTypeGeoKey = 4267;	
		else if (mapProjectionInfoPtr->geodetic.datumCode == kNAD83Code)
			geographicTypeGeoKey = 4269;		
		else if (mapProjectionInfoPtr->geodetic.datumCode == 
																	kNewZealandGeodeticDatum1949Code)
			geographicTypeGeoKey = 4272;
		else if (mapProjectionInfoPtr->geodetic.datumCode == kNGO1948_OslowCode)
			geographicTypeGeoKey = 4273;
		else if (mapProjectionInfoPtr->geodetic.datumCode == kDatum_OSGB_1936Code)
			geographicTypeGeoKey = 4277;
		else if (mapProjectionInfoPtr->geodetic.datumCode == 
																	kGeocentricDatumAustralia1994Code)
			geographicTypeGeoKey = 4283;
		else if (mapProjectionInfoPtr->geodetic.datumCode == kPulkovo1942DatumCode)
			geographicTypeGeoKey = 4284;
		else if (mapProjectionInfoPtr->geodetic.datumCode == kPulkovo1995DatumCode)
			geographicTypeGeoKey = 4200;
		else if (mapProjectionInfoPtr->geodetic.datumCode == kSAD69DatumCode)
			geographicTypeGeoKey = 4291;
		else if (mapProjectionInfoPtr->geodetic.datumCode == kWGS72DatumCode)
			geographicTypeGeoKey = 4322;
		else if (mapProjectionInfoPtr->geodetic.datumCode == kWGS84Code)
			geographicTypeGeoKey = 4326;	
		else
			geographicTypeGeoKey = 32767;
			
		}	// end "if ((projectedCSTypeGeoKey == 0 || ..."
		
			// If GeographicTypeGeoKey is user defined, then define the Geographic 
			// Geodetic Datum
			// GeogGeodeticDatumGeoKey: Key ID = 2050
			
	geogGeodeticDatumGeoKey = 0;
	if (mapProjectionInfoPtr->geodetic.datumCode == kSphereDatumCode ||
			((projectedCSTypeGeoKey == 0 || projectedCSTypeGeoKey == 32767) &&
				(geographicTypeGeoKey == 0 || geographicTypeGeoKey == 32767) &&
						mapProjectionInfoPtr->geodetic.datumCode != 0))
		{	
		if (mapProjectionInfoPtr->geodetic.datumCode == kSphereDatumCode)
			{
			geogGeodeticDatumGeoKey = 32767;		// User-defined
			
					// ArcGIS does not recognize the following. Imagine does.
			//geogGeodeticDatumGeoKey = 6035;		// DatumE_Sphere. 
			//geogCitationGeoKeyFlag = TRUE;
			
			}	// end "if (...->geodetic.datumCode == kSphereDatumCode)"
			
		else if (mapProjectionInfoPtr->geodetic.datumCode == kGGRS87DatumCode)
			geogGeodeticDatumGeoKey = 6121;
		else if (mapProjectionInfoPtr->geodetic.datumCode == kNAD83_CSRSCode)
			geogGeodeticDatumGeoKey = 6140;
		else if (mapProjectionInfoPtr->geodetic.datumCode == kBeijing1954Code)
			geogGeodeticDatumGeoKey = 6214;
		else if (mapProjectionInfoPtr->geodetic.datumCode == kCampoCode)
			geogGeodeticDatumGeoKey = 6221;
		else if (mapProjectionInfoPtr->geodetic.datumCode == kNAD27Code)
			geogGeodeticDatumGeoKey = 6267;	
		else if (mapProjectionInfoPtr->geodetic.datumCode == kNAD83Code)
			geogGeodeticDatumGeoKey = 6269;	
		else if (mapProjectionInfoPtr->geodetic.datumCode == 
																	kNewZealandGeodeticDatum1949Code)
			geogGeodeticDatumGeoKey = 6272;
		else if (mapProjectionInfoPtr->geodetic.datumCode == kNGO1948_OslowCode)
			geogGeodeticDatumGeoKey = 6273;
		else if (mapProjectionInfoPtr->geodetic.datumCode == kDatum_OSGB_1936Code)
			geogGeodeticDatumGeoKey = 6277;
		else if (mapProjectionInfoPtr->geodetic.datumCode == 
																	kGeocentricDatumAustralia1994Code)
			geogGeodeticDatumGeoKey = 6283;
		else if (mapProjectionInfoPtr->geodetic.datumCode == kPulkovo1942DatumCode)
			geogGeodeticDatumGeoKey = 6284;
		else if (mapProjectionInfoPtr->geodetic.datumCode == kPulkovo1995DatumCode)
			geogGeodeticDatumGeoKey = 6200;
		else if (mapProjectionInfoPtr->geodetic.datumCode == kSAD69DatumCode)
			geogGeodeticDatumGeoKey = 6291;
		else if (mapProjectionInfoPtr->geodetic.datumCode == kWGS72DatumCode)
			geogGeodeticDatumGeoKey = 6322;
		else if (mapProjectionInfoPtr->geodetic.datumCode == kWGS84Code)
			geogGeodeticDatumGeoKey = 6326;
			
		}	// end "if ((projectedCSTypeGeoKey == 0 || ..."
		
			// Define the Prime Meridian. Greenwich is the default.
			// I am including this now to see if Imagine will handle some geotiff better
			// 3/9/2007.
			// GeogPrimeMeridianGeoKey: Key ID = 2051
			
	geogPrimeMeridianGeoKey = 8901;
	if (projectedCSTypeGeoKey > 0 && projectedCSTypeGeoKey < 32767)
		geogPrimeMeridianGeoKey = 0;
		
			// If Projected CS Type Geo Key not found, then find what is known.
			// GeogEllipsoidGeoKey: 2056
	
	geogEllipsoidGeoKey = 0;
	if ((projectedCSTypeGeoKey == 0 || projectedCSTypeGeoKey == 32767) && 
			(geographicTypeGeoKey == 0 || geographicTypeGeoKey == 32767) && 
											mapProjectionInfoPtr->geodetic.spheroidCode != 0)
		{	
		if (mapProjectionInfoPtr->geodetic.spheroidCode == kAiryEllipsoidCode)
			geogEllipsoidGeoKey = 7001;	
		else if (mapProjectionInfoPtr->geodetic.spheroidCode == 
																				kAustralianEllipsoidCode)
			geogEllipsoidGeoKey = 7003;	
		else if (mapProjectionInfoPtr->geodetic.spheroidCode == kBesselEllipsoidCode)
			geogEllipsoidGeoKey = 7004;
		else if (mapProjectionInfoPtr->geodetic.spheroidCode == 
																				kClarke1866EllipsoidCode)
			geogEllipsoidGeoKey = 7008;
		else if (mapProjectionInfoPtr->geodetic.spheroidCode == 
																				kClarke1880EllipsoidCode)
			geogEllipsoidGeoKey = 7034;
		else if (mapProjectionInfoPtr->geodetic.spheroidCode == kEverestEllipsoidCode)
			geogEllipsoidGeoKey = 7015;
		else if (mapProjectionInfoPtr->geodetic.spheroidCode == kGRS80EllipsoidCode)
			geogEllipsoidGeoKey = 7019;
		else if (mapProjectionInfoPtr->geodetic.spheroidCode == 
																		kInternational1909EllipsoidCode)
			geogEllipsoidGeoKey = 7022;
		else if (mapProjectionInfoPtr->geodetic.spheroidCode == 
																				kKrassovskyEllipsoidCode)
			geogEllipsoidGeoKey = 7024;
		else if (mapProjectionInfoPtr->geodetic.spheroidCode == kSphereEllipsoidCode)
			{
			//if (mapProjectionInfoPtr->geodetic.semiMajorAxis > 0)
			//	geogEllipsoidGeoKey = 32767;
						
			//else	// mapProjectionInfoPtr->geodetic.semiMajorAxis <= 0
			geogEllipsoidGeoKey = 7035;
			
			}	// end "else if (...->geodetic.spheroidCode == kSphereEllipsoidCode)"
			
		else if (mapProjectionInfoPtr->geodetic.spheroidCode == kWGS72EllipsoidCode)
			geogEllipsoidGeoKey = 7043;
		else if (mapProjectionInfoPtr->geodetic.spheroidCode == kWGS84EllipsoidCode)
			geogEllipsoidGeoKey = 7030;
		else if (mapProjectionInfoPtr->geodetic.spheroidCode == 
																		kGRS1967ModifiedEllipsoidCode)
			geogEllipsoidGeoKey = 7050;
		else	// Check if user define with major and minor axes
			{
			if (mapProjectionInfoPtr->geodetic.semiMajorAxis > 0 &&
						mapProjectionInfoPtr->geodetic.semiMinorAxis > 0)
				geogEllipsoidGeoKey = 32767;
				
			}	// end "else check if user define with major and minor axes"
			
		}	// end "if ((projectedCSTypeGeoKey == 0 || ..."
	
			// Specify the major axis if this is a user defined ellipsoid
			// GeogSemiMajorAxisGeoKey: 2057
				
	geogSemiMajorAxisGeoKey = 0;
	if (geogEllipsoidGeoKey == 32767 || geogEllipsoidGeoKey == 7035)
		geogSemiMajorAxisGeoKey = mapProjectionInfoPtr->geodetic.semiMajorAxis;
	
			// Specify the minor axis if this is a user defined ellipsoid
			// GeogSemiMinorAxisGeoKey: 2058
		
	geogSemiMinorAxisGeoKey = 0;
	if (geogEllipsoidGeoKey == 32767 || geogEllipsoidGeoKey == 7035)
		geogSemiMinorAxisGeoKey = mapProjectionInfoPtr->geodetic.semiMinorAxis;
	
			// If ProjectedCSTypeGeoKey is user defined then define the ProjectionGeoKey
			// ProjectionGeoKey: 3074
			
	projectionGeoKey = 0;
	if (projectedCSTypeGeoKey == 32767 && 
									mapProjectionInfoPtr->gridCoordinate.projectionCode != 0)
		projectionGeoKey = 32767;
		
			// If Projected CS Type Geo Key not found, then check for Projected 
			// Coordinate Transformation Geo Key.
			// ProjCoordTransGeoKey: 3075
	
	projCoordTransGeoKey = 0;
	if (projectedCSTypeGeoKey == 0 || projectedCSTypeGeoKey == 32767)
		{	 
		switch (mapProjectionInfoPtr->gridCoordinate.projectionCode)
			{
			case kTransverseMercatorCode:
				projCoordTransGeoKey = 1;
				break;
			case kSpaceObliqueMercatorCode:
				projCoordTransGeoKey = 3;
				break;
			case kMercatorCode:
				projCoordTransGeoKey = 7;
				break;
			case kLambertConformalConicCode:
				projCoordTransGeoKey = 8;
				break;
			case kLambertAzimuthalEqualAreaCode:
				projCoordTransGeoKey = 10;
				break;
			case kAlbersConicalEqualAreaCode:
				projCoordTransGeoKey = 11;
				break;
			case kAzimuthalEquidistantCode:
				projCoordTransGeoKey = 12;
				break;
			case kEquidistantConicCode:
				projCoordTransGeoKey = 13;
				break;
			case kStereographicCode:
				projCoordTransGeoKey = 14;
				break;
			case kPolarStereographicCode:
				projCoordTransGeoKey = 15;
				break;
			case kEquirectangularCode:
				projCoordTransGeoKey = 17;
				break;
			case kGnomonicCode:
				projCoordTransGeoKey = 19;
				break;
			case kMillerCylindricalCode:
				projCoordTransGeoKey = 20;
				break;
			case kOrthographicCode:
				projCoordTransGeoKey = 21;
				break;
			case kPolyconicCode:
				projCoordTransGeoKey = 22;
				break;
			case kSinusoidalCode:
				projCoordTransGeoKey = 24;
				break;
			case kVanderGrintenICode:
				projCoordTransGeoKey = 25;
				break;
			case kNewZealandMapGridCode:
				projCoordTransGeoKey = 26;
				break;
			case kTransvMercatorSouthOrientedCode:
				projCoordTransGeoKey = 27;
				break;
			case kCylindricalEqualAreaCode:
				projCoordTransGeoKey = 28;
				break;
			case kKrovakCode:
						// This seems to be special case. I have more to sort out on this 
						// "projection".
				if (projectedCSTypeGeoKey == 0)
					projectedCSTypeGeoKey = 2065;
				break;
			
			}	// end "switch (mapProjectionInfoPtr->gridCoordinate.code)"
		
		}	// end "if (projectedCSTypeGeoKey == 0 || ..."

			// Get the linear units.
			// GeogLinearUnitsGeoKey: 2052
			// ProjLinearUnitsGeoKey: 3076
	
	projLinearUnitsGeoKey = 0;
	geogLinearUnitsGeoKey = 0;		
	if (mapProjectionInfoPtr->planarCoordinate.mapUnitsCode == kMetersCode)
		geogLinearUnitsGeoKey = 9001;	
	else if (mapProjectionInfoPtr->planarCoordinate.mapUnitsCode == kFeetCode)
		geogLinearUnitsGeoKey = 9002;	
	else if (mapProjectionInfoPtr->planarCoordinate.mapUnitsCode == kUSSurveyFeetCode)
		geogLinearUnitsGeoKey = 9003;
	/*
	if (projectedCSTypeGeoKey > 0 && projectedCSTypeGeoKey < 32767)
		{
		projLinearUnitsGeoKey = geogLinearUnitsGeoKey;
		geogLinearUnitsGeoKey = 0;
		
		}	// end "if (projectedCSTypeGeoKey > 0 && projectedCSTypeGeoKey < 32767)"
	*/	
	projLinearUnitsGeoKey = geogLinearUnitsGeoKey;
	
			// Get angular units.
			// GeogAngularUnitsGeoKey: 2054
			
	geogAngularUnitsGeoKey = 9102;
	if (projectedCSTypeGeoKey > 0 && projectedCSTypeGeoKey < 32767)
		geogAngularUnitsGeoKey = 0;
	
			// Get parameters for the projection
			
	GetProjectionParameters (mapProjectionInfoPtr,
										kDoNotIncludeUTM,
										&projStdParallel1GeoKey,
										&projStdParallel2GeoKey,
										&projNatOriginLongGeoKey,
										&projNatOriginLatGeoKey,
										&projFalseEastingGeoKey,
										&projFalseNorthingGeoKey,
										&projFalseOriginLongGeoKey,
										&projFalseOriginLatGeoKey,
										&projFalseOriginEastingGeoKey,
										&projFalseOriginNorthingGeoKey,
										&projCenterLongGeoKey,
										&projCenterLatGeoKey,
										&projScaleAtNatOriginGeoKey,
										&projAzimuthAngleGeoKey,
										&projStraightVertPoleLongGeoKey);
			
	index = 0;
	doubleParameterIndex = 0;
	doubleParameterBuffIndex = 0;
	numberASCIIParameterBuffIndex = 0;
		
			// Version
				
	if (bufferPtr != NULL)
		{	
		LoadGeoKeyEntry (geoKeyDirectoryField,
									1,
									1,
									0,
									3);

		BlockMoveData (&geoKeyDirectoryField[0], 
								&bufferPtr[index], 
								8);
				
		}	// end "if (bufferPtr != NULL)"
		
	index += 8;
		
				// GTModelTypeGeoKey
				
	if (bufferPtr != NULL)
		{	
		LoadGeoKeyEntry (geoKeyDirectoryField,
								1024,
								0,
								1,
								modelTypeGeoKey);

		BlockMoveData (&geoKeyDirectoryField[0], 
							&bufferPtr[index], 
							8);
				
		}	// end "if (bufferPtr != NULL)"
		
	index += 8;
		
				// GTRasterTypeGeoKey
				
	if (bufferPtr != NULL)
		{		
		LoadGeoKeyEntry (geoKeyDirectoryField,
								1025,
								0,
								1,
								1);

		BlockMoveData (&geoKeyDirectoryField[0], 
							&bufferPtr[index], 
							8);
				
		}	// end "if (bufferPtr != NULL)"
		
	index += 8;
			
			// GTCitationGeoKey
			// Get the date and time to add.
	
	count = 0;
	/*
	time_t currentTime = time (NULL);
	struct tm *currentDate = localtime (&currentTime);
	strftime ((char*)gTextString3, 254, "%m-%d-%Y  %X", currentDate);
		
	count = sprintf (string, 
								"Generated by %s on %s|", 
								&gApplicationIdentifierString,
								gTextString3);
								
	count += LoadInstrumentNameToTIFFAsciiTag (fileInfoPtr, &string[count]);
	*/
	/*		// The following was for debugging a problem with gdal in April 2012. Will 
			// take out when problem is completely resolved.
	count = sprintf (string, 
							"IMAGINE GeoTIFF Support.\nERDAS Desktop 2011 Version 11.0.3 "
								"11.0.0.896.\nProjection Name = State Plane.\nUnits = "
								"us_survey_feet.\nGeoTIFF Units = us_survey_feet|");
							"IMAGINE GeoTIFF Support.\nERDAS Desktop 2011 Version 11.0.3 "
							"11.0.0.896.\nProjection Name = State Plane.\nUnits = "
							"us_survey_feet.|");
	*/
	if (count > 0)
		{
		if (bufferPtr != NULL && asciiParametersBufferPtr != NULL)
			{	
			LoadGeoKeyEntry (geoKeyDirectoryField,
										1026,
										34737,
										count,
										(UInt16)numberASCIIParameterBuffIndex);

			BlockMoveData (&geoKeyDirectoryField[0], 
									&bufferPtr[index], 
									8);
											
			BlockMoveData (asciiString,
									&asciiParametersBufferPtr[numberASCIIParameterBuffIndex], 
									count);
			
			}	// end "if (bufferPtr != NULL && ..."
	
		index += 8;
		numberASCIIParameterBuffIndex += count;
	
		}	// end "if (count > 0)"

	if (geographicTypeGeoKey != 0)
		{
				// GeographicTypeGeoKey
				
		if (bufferPtr != NULL)
			{	
			LoadGeoKeyEntry (geoKeyDirectoryField,
									2048,
									0,
									1,
									geographicTypeGeoKey);
										
			BlockMoveData (&geoKeyDirectoryField[0], 
								&bufferPtr[index], 
								8);
			
			}	// end "if (bufferPtr != NULL)"
		
		index += 8;
		
		}	// end "if (geographicTypeGeoKey != 0)"

	if (geogCitationGeoKeyFlag)
		{
				// GeoCitationGeoKey
		
		count = 0;
		if (mapProjectionInfoPtr->geodetic.spheroidCode == kSphereEllipsoidCode)
			count = sprintf (asciiString, "Spherical Earth|");
		
				// Add null value at end of asciiString.
				
		asciiString[count] = 0;
		count++;
							
		if (bufferPtr != NULL && asciiParametersBufferPtr != NULL)
			{	
			LoadGeoKeyEntry (geoKeyDirectoryField,
									2049,
									34737,
									count,
									(UInt16)numberASCIIParameterBuffIndex);
										
			BlockMoveData (&geoKeyDirectoryField[0], 
								&bufferPtr[index], 
								8);
										
			BlockMoveData (asciiString,
								&asciiParametersBufferPtr[numberASCIIParameterBuffIndex], 
								count);
			
			}	// end "if (bufferPtr != NULL)"
			
		index += 8;
		numberASCIIParameterBuffIndex += count;
		
		}	// end "if (geogCitationGeoKeyFlag)"
			
	if (geogGeodeticDatumGeoKey != 0)
		{
				// GeogGeodeticDatumGeoKey
				
		if (bufferPtr != NULL)
			{	
			LoadGeoKeyEntry (geoKeyDirectoryField,
									2050,
									0,
									1,
									geogGeodeticDatumGeoKey);

			BlockMoveData (&geoKeyDirectoryField[0], 
								&bufferPtr[index], 
								8);
			
			}	// end "if (bufferPtr != NULL)"
		
		index += 8;
		
		}	// end "if (geogGeodeticDatumGeoKey != 0)"
		
	if (geogPrimeMeridianGeoKey != 0)
		{
				// GeogPrimeMeridianGeoKey
				
		if (bufferPtr != NULL)
			{	
			LoadGeoKeyEntry (geoKeyDirectoryField,
									2051,
									0,
									1,
									geogPrimeMeridianGeoKey);

			BlockMoveData (&geoKeyDirectoryField[0], 
								&bufferPtr[index], 
								8);
			
			}	// end "if (bufferPtr != NULL)"
		
		index += 8;
		
		}	// end "if (geogPrimeMeridianGeoKey != 0)"
		
	if (geogLinearUnitsGeoKey != 0)
		{
				// GeogLinearUnitsGeoKey
				
		if (bufferPtr != NULL)
			{	
			LoadGeoKeyEntry (geoKeyDirectoryField,
									2052,
									0,
									1,
									geogLinearUnitsGeoKey);

			BlockMoveData (&geoKeyDirectoryField[0], 
								&bufferPtr[index], 
								8);
			
			}	// end "if (bufferPtr != NULL)"
		
		index += 8;
		
		}	// end "if (geogLinearUnitsGeoKey != 0)"
		
	if (geogAngularUnitsGeoKey != 0)
		{
				// GeogAngularUnitsGeoKey
				
		if (bufferPtr != NULL)
			{	
			LoadGeoKeyEntry (geoKeyDirectoryField,
									2054,
									0,
									1,
									geogAngularUnitsGeoKey);

			BlockMoveData (&geoKeyDirectoryField[0], 
								&bufferPtr[index], 
								8);
			
			}	// end "if (bufferPtr != NULL)"
		
		index += 8;
		
		}	// end "if (geogAngularUnitsGeoKey != 0)"
		
	if (geogEllipsoidGeoKey != 0)
		{
				// geogEllipsoidGeoKey
				
		if (bufferPtr != NULL)
			{	
			LoadGeoKeyEntry (geoKeyDirectoryField,
									2056,
									0,
									1,
									geogEllipsoidGeoKey);

			BlockMoveData (&geoKeyDirectoryField[0], 
								&bufferPtr[index], 
								8);
			
			}	// end "if (bufferPtr != NULL)"
		
		index += 8;
		
		}	// end "if (geogEllipsoidGeoKey != 0)"
		
	if (geogSemiMajorAxisGeoKey > 0)
		{
				// geogSemiMajorAxisGeoKey
				
		if (bufferPtr != NULL && doubleParametersBufferPtr != NULL)
			{	
			LoadGeoKeyEntry (geoKeyDirectoryField,
									2057,
									34736,
									1,
									(UInt16)doubleParameterIndex);

			BlockMoveData (&geoKeyDirectoryField[0], 
								&bufferPtr[index], 
								8);

			tempDoubleValue = GetDoubleValue ((UCharPtr)&geogSemiMajorAxisGeoKey);
			BlockMoveData (&tempDoubleValue, 
								&doubleParametersBufferPtr[doubleParameterBuffIndex], 
								8);
			
			}	// end "if (bufferPtr != NULL && ..."
		
		index += 8;
		doubleParameterIndex++;
		doubleParameterBuffIndex += 8;
		
		}	// end "if (geogSemiMajorAxisGeoKey != 0)"
		
	if (geogSemiMinorAxisGeoKey > 0)
		{
				// geogSemiMajorAxisGeoKey
				
		if (bufferPtr != NULL && doubleParametersBufferPtr != NULL)
			{	
			LoadGeoKeyEntry (geoKeyDirectoryField,
									2058,
									34736,
									1,
									doubleParameterIndex);

			BlockMoveData (&geoKeyDirectoryField[0], 
								&bufferPtr[index], 
								8);

			tempDoubleValue = GetDoubleValue ((UCharPtr)&geogSemiMinorAxisGeoKey);
			BlockMoveData (&tempDoubleValue, 
								&doubleParametersBufferPtr[doubleParameterBuffIndex], 
								8);
			
			}	// end "if (bufferPtr != NULL && ..."
		
		index += 8;
		doubleParameterIndex++;
		doubleParameterBuffIndex += 8;
		
		}	// end "if (geogSemiMinorAxisGeoKey != 0)"
		
	if (projectedCSTypeGeoKey != 0)
		{
				// ProjectedCSTypeGeoKey
				
		if (bufferPtr != NULL)
			{	
			LoadGeoKeyEntry (geoKeyDirectoryField,
									3072,
									0,
									1,
									projectedCSTypeGeoKey);

			BlockMoveData (&geoKeyDirectoryField[0], 
								&bufferPtr[index], 
								8);
			
			}	// end "if (bufferPtr != NULL)"
			
		index += 8;
		
		}	// end "if (projectedCSTypeGeoKey != 0)"
		
	if (projectionGeoKey != 0)
		{
				// ProjectionGeoKey
				
		if (bufferPtr != NULL)
			{	
			LoadGeoKeyEntry (geoKeyDirectoryField,
									3074,
									0,
									1,
									projectionGeoKey);

			BlockMoveData (&geoKeyDirectoryField[0], 
								&bufferPtr[index], 
								8);
			
			}	// end "if (bufferPtr != NULL)"
			
		index += 8;
		
		}	// end "if (projectionGeoKey != 0)"
		
	if (projCoordTransGeoKey != 0)
		{
				// ProjectedCSTypeGeoKey
				
		if (bufferPtr != NULL)
			{	
			LoadGeoKeyEntry (geoKeyDirectoryField,
									3075,
									0,
									1,
									projCoordTransGeoKey);

			BlockMoveData (&geoKeyDirectoryField[0], 
								&bufferPtr[index], 
								8);
			
			}	// end "if (bufferPtr != NULL)"
			
		index += 8;
		
		}	// end "if (projectedCSTypeGeoKey != 0)"
		
	if (projLinearUnitsGeoKey != 0)
		{
				// GeogLinearUnitsGeoKey
				
		if (bufferPtr != NULL)
			{	
			LoadGeoKeyEntry (geoKeyDirectoryField,
									3076,
									0,
									1,
									projLinearUnitsGeoKey);

			BlockMoveData (&geoKeyDirectoryField[0], 
								&bufferPtr[index], 
								8);
			
			}	// end "if (bufferPtr != NULL)"
		
		index += 8;
		
		}	// end "if (projLinearUnitsGeoKey != 0)"
		
	if (projectedCSTypeGeoKey <= 0 || projectedCSTypeGeoKey >= 32767)
		{
		if (projStdParallel1GeoKey != -9999)
			{
					// ProjStdParallel1GeoKey
					
			if (bufferPtr != NULL && doubleParametersBufferPtr != NULL)
				{	
				LoadGeoKeyEntry (geoKeyDirectoryField,
										3078,
										34736,
										1,
										doubleParameterIndex);

				BlockMoveData (&geoKeyDirectoryField[0], 
									&bufferPtr[index], 
									8);

				tempDoubleValue = GetDoubleValue ((UCharPtr)&projStdParallel1GeoKey);
				BlockMoveData (&tempDoubleValue, 
									&doubleParametersBufferPtr[doubleParameterBuffIndex], 
									8);
				
				}	// end "if (bufferPtr != NULL && ..."
				
			index += 8;
			doubleParameterIndex++;
			doubleParameterBuffIndex += 8;
			
			}	// end "if (projStdParallel1GeoKey != -9999)"
			
		if (projStdParallel2GeoKey != -9999)
			{
					// ProjStdParallel2GeoKey
					
			if (bufferPtr != NULL && doubleParametersBufferPtr != NULL)
				{	
				LoadGeoKeyEntry (geoKeyDirectoryField,
										3079,
										34736,
										1,
										doubleParameterIndex);

				BlockMoveData (&geoKeyDirectoryField[0], 
									&bufferPtr[index], 
									8);

				tempDoubleValue = GetDoubleValue ((UCharPtr)&projStdParallel2GeoKey);
				BlockMoveData (&tempDoubleValue, 
									&doubleParametersBufferPtr[doubleParameterBuffIndex], 
									8);
				
				}	// end "if (bufferPtr != NULL && ..."
				
			index += 8;
			doubleParameterIndex++;
			doubleParameterBuffIndex += 8;
			
			}	// end "if (projStdParallel2GeoKey != -9999)"
			
		if (projNatOriginLongGeoKey != -9999)
			{
					// ProjNatOriginLongGeoKey
					
			if (bufferPtr != NULL && doubleParametersBufferPtr != NULL)
				{	
				LoadGeoKeyEntry (geoKeyDirectoryField,
										3080,
										34736,
										1,
										doubleParameterIndex);

				BlockMoveData (&geoKeyDirectoryField[0], 
									&bufferPtr[index], 
									8);

				tempDoubleValue = GetDoubleValue ((UCharPtr)&projNatOriginLongGeoKey);
				BlockMoveData (&tempDoubleValue, 
									&doubleParametersBufferPtr[doubleParameterBuffIndex], 
									8);
				
				}	// end "if (bufferPtr != NULL && ..."
				
			index += 8;
			doubleParameterIndex++;
			doubleParameterBuffIndex += 8;
			
			}	// end "if (projNatOriginLongGeoKey != -9999)"
			
		if (projNatOriginLatGeoKey != -9999)
			{
					// ProjNatOriginLatGeoKey
					
			if (bufferPtr != NULL && doubleParametersBufferPtr != NULL)
				{	
				LoadGeoKeyEntry (geoKeyDirectoryField,
										3081,
										34736,
										1,
										doubleParameterIndex);

				BlockMoveData (&geoKeyDirectoryField[0], 
									&bufferPtr[index], 
									8);

				tempDoubleValue = GetDoubleValue ((UCharPtr)&projNatOriginLatGeoKey);
				BlockMoveData (&tempDoubleValue, 
									&doubleParametersBufferPtr[doubleParameterBuffIndex], 
									8);
				
				}	// end "if (bufferPtr != NULL && ..."
				
			index += 8;
			doubleParameterIndex++;
			doubleParameterBuffIndex += 8;
			
			}	// end "if (projNatOriginLatGeoKey != -9999)"
			
		if (projFalseEastingGeoKey != -9999)
			{
					// ProjFalseEastingGeoKey
					
			if (bufferPtr != NULL && doubleParametersBufferPtr != NULL)
				{	
				LoadGeoKeyEntry (geoKeyDirectoryField,
										3082,
										34736,
										1,
										doubleParameterIndex);

				BlockMoveData (&geoKeyDirectoryField[0], 
									&bufferPtr[index], 
									8);

				tempDoubleValue = GetDoubleValue ((UCharPtr)&projFalseEastingGeoKey);
				BlockMoveData (&tempDoubleValue, 
									&doubleParametersBufferPtr[doubleParameterBuffIndex], 
									8);
				
				}	// end "if (bufferPtr != NULL && ..."
				
			index += 8;
			doubleParameterIndex++;
			doubleParameterBuffIndex += 8;
			
			}	// end "if (projFalseEastingGeoKey != -9999)"
			
		if (projFalseNorthingGeoKey != -9999)
			{
					// ProjFalseNorthingGeoKey
					
			if (bufferPtr != NULL && doubleParametersBufferPtr != NULL)
				{	
				LoadGeoKeyEntry (geoKeyDirectoryField,
										3083,
										34736,
										1,
										doubleParameterIndex);

				BlockMoveData (&geoKeyDirectoryField[0], 
									&bufferPtr[index], 
									8);

				tempDoubleValue = GetDoubleValue ((UCharPtr)&projFalseNorthingGeoKey);
				BlockMoveData (&tempDoubleValue, 
									&doubleParametersBufferPtr[doubleParameterBuffIndex], 
									8);
				
				}	// end "if (bufferPtr != NULL && ..."
				
			index += 8;
			doubleParameterIndex++;
			doubleParameterBuffIndex += 8;
			
			}	// end "if (projFalseNorthingGeoKey != -9999)"
			
		if (projFalseOriginLongGeoKey != -9999)
			{
					// ProjFalseOriginLongGeoKey
					
			if (bufferPtr != NULL && doubleParametersBufferPtr != NULL)
				{	
				LoadGeoKeyEntry (geoKeyDirectoryField,
										3084,
										34736,
										1,
										doubleParameterIndex);

				BlockMoveData (&geoKeyDirectoryField[0], 
									&bufferPtr[index], 
									8);

				tempDoubleValue = GetDoubleValue ((UCharPtr)&projFalseOriginLongGeoKey);
				BlockMoveData (&tempDoubleValue, 
									&doubleParametersBufferPtr[doubleParameterBuffIndex], 
									8);
				
				}	// end "if (bufferPtr != NULL && ..."
				
			index += 8;
			doubleParameterIndex++;
			doubleParameterBuffIndex += 8;
			
			}	// end "if (projFalseOriginLongGeoKey != -9999)"
			
		if (projFalseOriginLatGeoKey != -9999)
			{
					// ProjFalseOriginLatGeoKey
					
			if (bufferPtr != NULL && doubleParametersBufferPtr != NULL)
				{	
				LoadGeoKeyEntry (geoKeyDirectoryField,
										3085,
										34736,
										1,
										doubleParameterIndex);

				BlockMoveData (&geoKeyDirectoryField[0], 
									&bufferPtr[index], 
									8);

				tempDoubleValue = GetDoubleValue ((UCharPtr)&projFalseOriginLatGeoKey);
				BlockMoveData (&tempDoubleValue, 
									&doubleParametersBufferPtr[doubleParameterBuffIndex], 
									8);
				
				}	// end "if (bufferPtr != NULL && ..."
				
			index += 8;
			doubleParameterIndex++;
			doubleParameterBuffIndex += 8;
			
			}	// end "if (projFalseOriginLatGeoKey != -9999)"
			
		if (projFalseOriginEastingGeoKey != -9999)
			{
					// ProjFalseOriginEastingGeoKey
					
			if (bufferPtr != NULL && doubleParametersBufferPtr != NULL)
				{	
				LoadGeoKeyEntry (geoKeyDirectoryField,
										3086,
										34736,
										1,
										doubleParameterIndex);

				BlockMoveData (&geoKeyDirectoryField[0], 
									&bufferPtr[index], 
									8);

				tempDoubleValue = 
									GetDoubleValue ((UCharPtr)&projFalseOriginEastingGeoKey);
				BlockMoveData (&tempDoubleValue, 
									&doubleParametersBufferPtr[doubleParameterBuffIndex], 
									8);
				
				}	// end "if (bufferPtr != NULL && ..."
				
			index += 8;
			doubleParameterIndex++;
			doubleParameterBuffIndex += 8;
			
			}	// end "if (projFalseOriginEastingGeoKey != -9999)"
			
		if (projFalseOriginNorthingGeoKey != -9999)
			{
					// ProjFalseOriginNorthingGeoKey
					
			if (bufferPtr != NULL && doubleParametersBufferPtr != NULL)
				{	
				LoadGeoKeyEntry (geoKeyDirectoryField,
										3087,
										34736,
										1,
										doubleParameterIndex);

				BlockMoveData (&geoKeyDirectoryField[0], 
									&bufferPtr[index], 
									8);

				tempDoubleValue = 
									GetDoubleValue ((UCharPtr)&projFalseOriginNorthingGeoKey);
				BlockMoveData (&tempDoubleValue, 
									&doubleParametersBufferPtr[doubleParameterBuffIndex], 
									8);
				
				}	// end "if (bufferPtr != NULL && ..."
				
			index += 8;
			doubleParameterIndex++;
			doubleParameterBuffIndex += 8;
			
			}	// end "if (projFalseOriginNorthingGeoKey != -9999)"
			
		if (projCenterLongGeoKey != -9999)
			{
					// ProjCenterLongGeoKey
					
			if (bufferPtr != NULL && doubleParametersBufferPtr != NULL)
				{	
				LoadGeoKeyEntry (geoKeyDirectoryField,
										3088,
										34736,
										1,
										doubleParameterIndex);

				BlockMoveData (&geoKeyDirectoryField[0], 
									&bufferPtr[index], 
									8);

				tempDoubleValue = GetDoubleValue ((UCharPtr)&projCenterLongGeoKey);
				BlockMoveData (&tempDoubleValue, 
									&doubleParametersBufferPtr[doubleParameterBuffIndex], 
									8);
				
				}	// end "if (bufferPtr != NULL && ..."
				
			index += 8;
			doubleParameterIndex++;
			doubleParameterBuffIndex += 8;
			
			}	// end "if (projCenterLongGeoKey != -9999)"
			
		if (projCenterLatGeoKey != -9999)
			{
					// ProjCenterLatGeoKey
					
			if (bufferPtr != NULL && doubleParametersBufferPtr != NULL)
				{	
				LoadGeoKeyEntry (geoKeyDirectoryField,
										3089,
										34736,
										1,
										doubleParameterIndex);

				BlockMoveData (&geoKeyDirectoryField[0], 
									&bufferPtr[index], 
									8);

				tempDoubleValue = GetDoubleValue ((UCharPtr)&projCenterLatGeoKey);
				BlockMoveData (&tempDoubleValue, 
									&doubleParametersBufferPtr[doubleParameterBuffIndex], 
									8);
				
				}	// end "if (bufferPtr != NULL && ..."
				
			index += 8;
			doubleParameterIndex++;
			doubleParameterBuffIndex += 8;
			
			}	// end "if (projCenterLatGeoKey != -9999)"
			
		if (projScaleAtNatOriginGeoKey != -9999)
			{
					// ProjCenterLatGeoKey
					
			if (bufferPtr != NULL && doubleParametersBufferPtr != NULL)
				{	
				LoadGeoKeyEntry (geoKeyDirectoryField,
										3092,
										34736,
										1,
										doubleParameterIndex);

				BlockMoveData (&geoKeyDirectoryField[0], 
									&bufferPtr[index], 
									8);

				tempDoubleValue = GetDoubleValue ((UCharPtr)&projScaleAtNatOriginGeoKey);
				BlockMoveData (&tempDoubleValue, 
									&doubleParametersBufferPtr[doubleParameterBuffIndex], 
									8);
				
				}	// end "if (bufferPtr != NULL && ..."
				
			index += 8;
			doubleParameterIndex++;
			doubleParameterBuffIndex += 8;
			
			}	// end "if (projScaleAtNatOriginGeoKey != -9999)"
			
		if (projAzimuthAngleGeoKey != -9999)
			{
					// ProjCenterLatGeoKey
					
			if (bufferPtr != NULL && doubleParametersBufferPtr != NULL)
				{	
				LoadGeoKeyEntry (geoKeyDirectoryField,
										3094,
										34736,
										1,
										doubleParameterIndex);

				BlockMoveData (&geoKeyDirectoryField[0], 
									&bufferPtr[index], 
									8);

				tempDoubleValue = GetDoubleValue ((UCharPtr)&projAzimuthAngleGeoKey);
				BlockMoveData (&tempDoubleValue, 
									&doubleParametersBufferPtr[doubleParameterBuffIndex], 
									8);
				
				}	// end "if (bufferPtr != NULL && ..."
				
			index += 8;
			doubleParameterIndex++;
			doubleParameterBuffIndex += 8;
			
			}	// end "if (projAzimuthAngleGeoKey != -9999)"
			
		if (projStraightVertPoleLongGeoKey != -9999)
			{
					// ProjStraightVertPoleLongGeoKey
					
			if (bufferPtr != NULL && doubleParametersBufferPtr != NULL)
				{	
				LoadGeoKeyEntry (geoKeyDirectoryField,
										3095,
										34736,
										1,
										doubleParameterIndex);

				BlockMoveData (&geoKeyDirectoryField[0], 
									&bufferPtr[index], 
									8);

				tempDoubleValue = 
									GetDoubleValue ((UCharPtr)&projStraightVertPoleLongGeoKey);
				BlockMoveData (&tempDoubleValue, 
									&doubleParametersBufferPtr[doubleParameterBuffIndex], 
									8);
				
				}	// end "if (bufferPtr != NULL && ..."
				
			index += 8;
			doubleParameterIndex++;
			doubleParameterBuffIndex += 8;
			
			}	// end "if (projAzimuthAngleGeoKey != -9999)"
			
		}	// end "if (projectedCSTypeGeoKey <= 0 || projectedCSTypeGeoKey >= 32767)"
			
			// load the correct number of geo keys.
			
	if (bufferPtr != NULL)
		{
				// This is the number of geo key records less the first info one.
				
		geoKeyDirectoryFieldPtr = (UInt16*)bufferPtr;
		count = (UInt16)(index/8-1);
		geoKeyDirectoryFieldPtr[3] = (UInt16)GetShortIntValue ((char*)&count);
		
		}	// end "if (bufferPtr != NULL)"
	
	if (doubleParameterBytePtr != NULL)
		*doubleParameterBytePtr = doubleParameterBuffIndex;
	
	if (numberASCIIParameterBytesPtr != NULL)
		*numberASCIIParameterBytesPtr = numberASCIIParameterBuffIndex;
		
	return (index);
	
}	// end "LoadGeoKeyDirectory" 
							


//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void LoadGeoKeyEntry
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
//	Coded By:			Larry L. Biehl			Date: 11/08/2005
//	Revised By:			Larry L. Biehl			Date: 11/08/2005

void LoadGeoKeyEntry (
				UInt16*								geoKeyRecordPtr,
				UInt16								entry0,
				UInt16								entry1,
				UInt16								entry2,
				UInt16								entry3)

{
			// Swap the bytes if needed.
			
	geoKeyRecordPtr[0] = (UInt16)GetShortIntValue ((char*)&entry0);
	geoKeyRecordPtr[1] = (UInt16)GetShortIntValue ((char*)&entry1);
	geoKeyRecordPtr[2] = (UInt16)GetShortIntValue ((char*)&entry2);
	geoKeyRecordPtr[3] = (UInt16)GetShortIntValue ((char*)&entry3);
	
}	// end "LoadGeoKeyEntry" 
							


//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32 LoadGeoModelTiePoints
//
//	Software purpose:	This routine loads the GeoTIFF model tie point (tag 33922).
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/27/2007
//	Revised By:			Larry L. Biehl			Date: 03/31/2007

UInt32 LoadGeoModelTiePoints (
				FileInfoPtr 						fileInfoPtr,
				UInt32								numberControlPoints,
				char*									geoTiffInfoBufferPtr,
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				UInt32								index,
				UInt32								startMapLeft,
				UInt32								startMapTop)

{
	double								pixelStart,
											spacerValue;
											
	ControlPointsPtr					controlPointsPtr;
	
	UInt32								count;
	
	
	spacerValue = 0;
	
			// Make the starting line and column 0 based.
			
	startMapLeft -= 1;
	startMapTop -= 1;
	
	if (numberControlPoints == 1)
		{
		index = LoadGeoDoubleValue (geoTiffInfoBufferPtr,
												index,
												spacerValue);
												
		index = LoadGeoDoubleValue (geoTiffInfoBufferPtr,
												index,
												spacerValue);
												
		index = LoadGeoDoubleValue (geoTiffInfoBufferPtr,
												index,
												spacerValue);
		
		pixelStart = mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 +
							mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize *
																						startMapLeft - 
								mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize/2;
		index = LoadGeoDoubleValue (geoTiffInfoBufferPtr,
												index,
												pixelStart);
		
		pixelStart = mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 - 
							mapProjectionInfoPtr->planarCoordinate.verticalPixelSize *
																						startMapTop +
									mapProjectionInfoPtr->planarCoordinate.verticalPixelSize/2;
		index = LoadGeoDoubleValue (geoTiffInfoBufferPtr,
												index,
												pixelStart);
												
		index = LoadGeoDoubleValue (geoTiffInfoBufferPtr,
												index,
												spacerValue);
			
		}	// end "if (numberControlPoints == 1)"
		
	else if (numberControlPoints > 1)
		{
		controlPointsPtr = GetControlPointVectorPointers (
																	fileInfoPtr->controlPointsHandle,
																	kNoLock);
															
		if (controlPointsPtr == NULL)
			numberControlPoints = 0;
		
		//startMapLeft = fileInfoPtr->startColumn - 1;
		//startMapTop = fileInfoPtr->startLine - 1;
		
		for (count = 0; count<numberControlPoints; count++)
			{
			index = LoadGeoDoubleValue (
											geoTiffInfoBufferPtr,
											index,
											controlPointsPtr->easting1Ptr[count]-startMapLeft);
													
			index = LoadGeoDoubleValue (
											geoTiffInfoBufferPtr,
											index,
											controlPointsPtr->northing1Ptr[count]-startMapTop);
													
			index = LoadGeoDoubleValue (geoTiffInfoBufferPtr,
													index,
													spacerValue);
													
			index = LoadGeoDoubleValue (geoTiffInfoBufferPtr,
													index,
													controlPointsPtr->easting2Ptr[count]);
													
			index = LoadGeoDoubleValue (geoTiffInfoBufferPtr,
													index,
													controlPointsPtr->northing2Ptr[count]);
													
			index = LoadGeoDoubleValue (geoTiffInfoBufferPtr,
													index,
													spacerValue);
			
			}	// end "for (count = 0; count<numberControlPoints; count++)"
			
		CloseControlPointVectorPointers (fileInfoPtr->controlPointsHandle);
		
		}	// end "else if (numberControlPoints > 1)"
		
	return (index);
	
}	// end "LoadGeoModelTiePoints" 
  


//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt16 LoadInstrumentNameToTIFFAsciiTag
//
//	Software purpose:	The purpose of this routine is to add the instrument name to
//							the tiff ascii string tag.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			WriteTIFFImageFile in SSaveWrt.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/10/2013
//	Revised By:			Larry L. Biehl			Date: 07/02/2018

UInt16 LoadInstrumentNameToTIFFAsciiTag (
				FileInfoPtr 						fileInfoPtr,
				SInt16								tiffSourceCode,
				char*									stringPtr)

{
	ReformatOptionsPtr				reformatOptionsPtr;
	
	SInt16								stringIndex,
											stringLength = 0;
											
	Boolean								continueFlag = TRUE;
	
	
	if (fileInfoPtr->instrumentCode > 0)
		{			
		stringLength = sprintf ((char*)stringPtr, " Instrument Name: ");
		
		stringIndex = IDS_InstrumentName01+fileInfoPtr->instrumentCode-1;
		
		continueFlag = GetSpecifiedStringNumber (
											kInstrumentNameStrID, 
											IDS_InstrumentName01+fileInfoPtr->instrumentCode-1, 
											gTextString2, 
											continueFlag);
		
		if (continueFlag)
			{
			if (tiffSourceCode == kFromReformat)
				{				
				reformatOptionsPtr = (ReformatOptionsPtr)GetHandlePointer (
												gNonProjProcessorSpecs.reformatOptionsSpecsH);
														
				if (!reformatOptionsPtr->channelDescriptions)
					stringLength += 
							sprintf ((char*)&stringPtr[stringLength],"Transformed ");
										
				}	// end "if (tiffSourceCode == kFromReformat)"
				
			stringLength += 
					sprintf ((char*)&stringPtr[stringLength],"%s |", &gTextString2[1]);
			
					// For PeruSat data add info for band order if it is known that the
					// bands are not in wavelength order.
			
			if (fileInfoPtr->channelsInWavelengthOrderCode == kNotInOrder)
				stringLength +=
							sprintf ((char*)&stringPtr[stringLength]," B2 B1 B0 B3 |");
			
			}	// end "if (continueFlag)"
									
		}	// end "if (fileInfoPtr->instrumentCode > 0)" 

	return (stringLength);
	
}	// end "LoadInstrumentNameToTIFFAsciiTag"
  


//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 LoadGeoModelTransformationParameters
//
//	Software purpose:	The purpose of this routine is to load the model transformation
//							parameters from this tag.  This tag will handle those cases which
//							have image rotation. I.E. north is not at the top.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/20/2012
//	Revised By:			Larry L. Biehl			Date: 02/28/2012

UInt32 LoadGeoModelTransformationParameters (
				FileInfoPtr 						fileInfoPtr,
				char*									geoTiffInfoBufferPtr,
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				UInt32								index)

{
	double								cosTransformationAngle,
											sinTransformationAngle,
											value,
											zeroValue;
											
	
			// The transformation angle is the negative of the map orientation angle.
			
	cosTransformationAngle = 
						cos (-mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle);
			
	sinTransformationAngle = 
						sin (-mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle);
	
	zeroValue = 0;
	
			// Model Transformation 0
			
	value = mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize * 
																					cosTransformationAngle;
	index = LoadGeoDoubleValue (geoTiffInfoBufferPtr,
											index,
											value);
	
			// Model Transformation 1
									
	value = mapProjectionInfoPtr->planarCoordinate.verticalPixelSize * 
																					sinTransformationAngle;
	index = LoadGeoDoubleValue (geoTiffInfoBufferPtr,
											index,
											value);
									
			// Model Transformation 2
			
	index = LoadGeoDoubleValue (geoTiffInfoBufferPtr,
											index,
											zeroValue);
									
			// Model Transformation 3
			
	value = mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11;
	index = LoadGeoDoubleValue (geoTiffInfoBufferPtr,
											index,
											value);
									
			// Model Transformation 4
			
	value = mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize * 
																					sinTransformationAngle;
	index = LoadGeoDoubleValue (geoTiffInfoBufferPtr,
											index,
											value);
									
			// Model Transformation 5
			
	value = -mapProjectionInfoPtr->planarCoordinate.verticalPixelSize * 
																					cosTransformationAngle;
	index = LoadGeoDoubleValue (geoTiffInfoBufferPtr,
											index,
											value);
									
			// Model Transformation 6
			
	index = LoadGeoDoubleValue (geoTiffInfoBufferPtr,
											index,
											zeroValue);
									
			// Model Transformation 7
			
	value = mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11;
	index = LoadGeoDoubleValue (geoTiffInfoBufferPtr,
											index,
											value);
									
			// Model Transformation 8
			
	index = LoadGeoDoubleValue (geoTiffInfoBufferPtr,
											index,
											zeroValue);
									
			// Model Transformation 9
			
	index = LoadGeoDoubleValue (geoTiffInfoBufferPtr,
											index,
											zeroValue);
									
			// Model Transformation 10
			
	index = LoadGeoDoubleValue (geoTiffInfoBufferPtr,
											index,
											zeroValue);
									
			// Model Transformation 11
			
	index = LoadGeoDoubleValue (geoTiffInfoBufferPtr,
											index,
											zeroValue);
									
			// Model Transformation 12
			
	index = LoadGeoDoubleValue (geoTiffInfoBufferPtr,
											index,
											zeroValue);
									
			// Model Transformation 13
			
	index = LoadGeoDoubleValue (geoTiffInfoBufferPtr,
											index,
											zeroValue);
									
			// Model Transformation 14
			
	index = LoadGeoDoubleValue (geoTiffInfoBufferPtr,
											index,
											zeroValue);
									
			// Model Transformation 15
	
	value = 1;
	index = LoadGeoDoubleValue (geoTiffInfoBufferPtr,
											index,
											value);
		
	return (index);
	
}	// end "LoadGeoModelTransformationParameters"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//							  (c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void LoadThematicClasses
//
//	Software purpose:	The purpose of this routine is to load the thematic classes info 
//							from the requested trailer file. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			LoadThematicInfo in SSAveWrt.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/11/1996
//	Revised By:			Larry L. Biehl			Date: 03/16/2017

void LoadThematicClasses (
				FileInfoPtr							fileInfoPtr)

{  
	CMFileStream*						supportFileStreamPtr;
	
	Handle								windowInfoHandle;
	SInt32								fileType;
	
	Boolean								classNamesExistFlag,
											supportFileSelectedFlag;
											
	SignedByte							handleStatus;
	
	
	supportFileSelectedFlag = GetThematicSupportFile (fileInfoPtr);
	
	if (supportFileSelectedFlag)
		{
		windowInfoHandle = GetActiveImageWindowInfoHandle ();
								
				// Read the class names into memory.	
				
		UCharPtr classNamePtr = (UCharPtr)GetHandlePointer (
													fileInfoPtr->classDescriptionH, kLock);
											
		supportFileStreamPtr = GetSupportFileStreamPointer (windowInfoHandle,
																				&handleStatus);
											
		fileType = GetFileType (supportFileStreamPtr);
		
		classNamesExistFlag = FALSE;
		if (fileType == kICLRFileType)
			classNamesExistFlag = ReadArcViewClassNames (
										fileInfoPtr, windowInfoHandle, classNamePtr);				
		
		else if (fileType == kITRLFileType)
			classNamesExistFlag = ReadERDASClassNames (
										fileInfoPtr, windowInfoHandle, classNamePtr);
		
		if (classNamesExistFlag)
			{
			ForceTextToEnd (); 
				
					// Get pointer to the file name and list a message in the
					// output text window that class information was read in okay.
			
			FileStringPtr fileNamePtr = 
					(FileStringPtr)GetFileNameCPointerFromFileInfo (fileInfoPtr);
			FileStringPtr infileNamePtr = 
					(FileStringPtr)GetFileNameCPointerFromFileStream (supportFileStreamPtr);
							
			ListSpecifiedStringNumber (kFileIOStrID,
													IDS_FileIO99,
													NULL, 
													gOutputForce1Code, 
													(char*)fileNamePtr, 
													(char*)infileNamePtr, 
													TRUE,
													kUTF8CharString);
			
			UnlockSupportFileStream (windowInfoHandle, handleStatus);
						
			}	// end "if (classNamesExistFlag)"
				
		CheckAndUnlockHandle (fileInfoPtr->classDescriptionH);
						
		Handle displaySpecsH = GetDisplaySpecsHandle (windowInfoHandle);
		DisplaySpecsPtr displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
																						displaySpecsH);
		
		if (displaySpecsPtr != NULL)
			{
			displaySpecsPtr->filePaletteFlag = TRUE;								
			displaySpecsPtr->thematicClassPaletteType = kImageDefaultColorTable;
			displaySpecsPtr->paletteUpToDateFlag = FALSE;
			displaySpecsPtr->readPaletteFromDiskFlag = TRUE;
			SInt16 classGroupCode = displaySpecsPtr->classGroupCode;
			fileInfoPtr->colorTableValueBytes = 1;
			fileInfoPtr->swapColorTableBytes = FALSE;
			UpdateActiveImageLegend (classGroupCode, kCallCreatePalette);
			
			gUpdateFileMenuItemsFlag = TRUE;
			fileInfoPtr->classChangedFlag = TRUE;
			
			}	// end "if (displaySpecsPtr != NULL)"
		
		}	// end "if (supportFileSelectedFlag)"
	
}	// end "LoadThematicClasses"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//							  (c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void LoadThematicGroups
//
//	Software purpose:	The purpose of this routine is to load the thematic
//							groups info from the requested image file. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			LoadThematicInfo in SSAveWrt.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/13/1996
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

void LoadThematicGroups (
				FileInfoPtr							outputFileInfoPtr)

{
	FileInfoPtr							inputFileInfoPtr;
	Handle								inputFileInfoHandle;
	
	Boolean								continueFlag;
	
	
			// Get a new handle to a block of memory to be used for					
			// file information for the image with the group information.								
	
	inputFileInfoHandle = MNewHandle (sizeof (MFileInfo));
	
			// Allow the user to select the file with the desired group 
			// information.
			
	continueFlag = GetGroupInfoFile (inputFileInfoHandle, outputFileInfoPtr);
	
	if (continueFlag)
		{					
		inputFileInfoPtr = (FileInfoPtr)GetHandlePointer (inputFileInfoHandle, kLock);
													
				// Load in any group information if it exists.	
						
		Handle displaySpecsH = GetActiveDisplaySpecsHandle ();
		DisplaySpecsPtr displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
																					displaySpecsH, kLock);						
				
		if (ReadThematicGroups (
						inputFileInfoPtr, outputFileInfoPtr, displaySpecsPtr))
			{
			displaySpecsPtr->paletteUpToDateFlag = FALSE;
			SInt16 classGroupCode = displaySpecsPtr->classGroupCode;
			CheckAndUnlockHandle (displaySpecsH);
			
			UpdateActiveImageLegend (classGroupCode, kCallCreatePalette);
			
			gUpdateFileMenuItemsFlag = TRUE;
			
			if (outputFileInfoPtr->ancillaryInfoformat == kArcViewDefaultSupportType)
				outputFileInfoPtr->groupChangedFlag = FALSE;	
		
			ForceTextToEnd (); 
				
					// Get pointer to the file file name and list a message in the
					// output text window that group information was read in okay.
			
			FileStringPtr outfileNamePtr = 
						(FileStringPtr)GetFileNameCPointerFromFileInfo (outputFileInfoPtr);
			FileStringPtr infileNamePtr = 
						(FileStringPtr)GetFileNameCPointerFromFileInfo (inputFileInfoPtr);
							
			ListSpecifiedStringNumber (kFileIOStrID,
												IDS_FileIO98,
												NULL, 
												gOutputForce1Code, 
												(char*)outfileNamePtr,
												(char*)infileNamePtr,
												TRUE,
												kUTF8CharString);
			
			}	// end "if (ReadThematicGroups (..."
		
		}	// end "if (continueFlag)"
		
	DisposeFileInfoHandle (inputFileInfoHandle);
	
}	// end "LoadThematicGroups"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//							  (c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void LoadThematicInfo
//
//	Software purpose:	The purpose of this routine is to load the thematic
//							classes or group info from the requested trailer file. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			Menus in menus.c
//							
//
//	Coded By:			Larry L. Biehl			Date: 11/11/1996
//	Revised By:			Larry L. Biehl			Date: 03/11/1999

void LoadThematicInfo (void)

{
	FileInfoPtr							fileInfoPtr = NULL;
	
	Handle								fileInfoHandle,
											windowInfoHandle;
	
	
	windowInfoHandle = GetActiveImageWindowInfoHandle ();
	if (windowInfoHandle == NULL)
																									return;
																						
	gProcessorCode = kOpenThematicInfo;
																					
			// Get the file pointer for the active image window.
	
	fileInfoHandle = GetFileInfoHandle (windowInfoHandle);
	fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle, kLock);
	
	if (fileInfoPtr != NULL)
		{	
		SInt16 classGroupCode = GetDisplayClassGroupCode (windowInfoHandle);
	
		if (classGroupCode == kClassDisplay)
			LoadThematicClasses (fileInfoPtr);
		
		else	// classGroupCode >= kGroupDisplay
			LoadThematicGroups (fileInfoPtr);
			
				// Scroll output window to the selection and adjust the scroll bar
				// in case text was written to the text window.															
		
		UpdateOutputWScrolls (gOutputWindow, 1, kDisplayNoMessage);
			
		}	// end "if (fileInfoPtr != NULL)"
	
	CheckAndUnlockHandle (fileInfoHandle);	
				
	gProcessorCode = 0;

}	// end "LoadThematicInfo"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void LoadTIFFColorBuffer
//
//	Software purpose:	The purpose of this routine is to load the input trl buffer
//							with the class names being used for the input image window file.
//
//	Parameters in:		
//
//	Parameters out:	None.
//
//	Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/22/2010
//	Revised By:			Larry L. Biehl			Date: 03/28/2017 

void LoadTIFFColorBuffer (
				FileInfoPtr							gisFileInfoPtr,
				ColorSpec*							colorSpecPtr,
				SInt16*								classPtr,
				UInt32								numberListClasses,
				UInt16*								paletteIndexPtr,
				SInt16								paletteOffset, 
				UInt32								numberTRLClasses,
				UInt16*								blueVectorPtr,
				UInt16*								greenVectorPtr,
				UInt16*								redVectorPtr,
				SInt16								collapseClassCode,
				SInt16								classNameCode)

{  
	UCharPtr								classNamePtr;
	UInt16*								classSymbolPtr;
	
	UInt32								classIndex,
											classNumber,
											index,
											maxClass,
											paletteIndex,
											vectorIndex;
	
	Boolean								backgroundFlag,
											incrementPaletteIndexFlag;
	
	
	classNamePtr = NULL;
	classSymbolPtr = NULL;
	vectorIndex = 0;
	
	if (collapseClassCode != kCollapseClass)
		{
		classNamePtr = (UCharPtr)GetHandlePointer (gisFileInfoPtr->classDescriptionH,
																 kLock);
		
		if (classNamePtr == NULL)
			collapseClassCode = kCollapseClass;
		
		else	// classNamePtr != NULL
			classSymbolPtr = 
				(UInt16*)&classNamePtr [gisFileInfoPtr->numberClasses*sizeof (Str31)];
		
		}	// end "if (collapseClassCode != kCollapseClass)"
	
		// If the trailer file is being created for a classification
		// image, then allow for class 0, which is a background class.
		// The color for this class is white. This class is not
		// counted in 'numberTRLClasses'.	
	
	if (classNameCode < kFromDescriptionCode)
		{
		blueVectorPtr[0] = 65535;
		greenVectorPtr[0] = 65535;
		redVectorPtr[0] = 65535;
		vectorIndex++;
		
		}	// end "if (classNameCode < kFromDescriptionCode)"
	
	paletteIndex = 0;
	classIndex = 0;	
	maxClass = MIN (255, numberTRLClasses);
	for (classNumber=1; classNumber<=maxClass; classNumber++)
		{
				// This section checks if the class is included in the list.
				// If there is no list, then we assume that the class is
				// included. If the class list is available and a given
				// class is not included, the white (background) color is used
				// for its color. This occurs when the user selects a subset
				// of the classes for a classification.
		
		backgroundFlag = FALSE;
		incrementPaletteIndexFlag = TRUE;
		if (classPtr != NULL)
			{
			if (classNumber == (UInt32)classPtr[classIndex])
				{
				if (classIndex < numberListClasses - 1)
					classIndex++;
				
				}	// end "if (classNumber == classPtr[index])" 
			
			else	// classNumber != classPtr[index] 
				backgroundFlag = TRUE;
			
			}	// end "if (classPtr != NULL)" 
		
		else if (classSymbolPtr != NULL)
			{
			if (classNumber == (UInt32)classSymbolPtr[classIndex]+1)
				classIndex++;
			
			else	// classNumber != classSymbolPtr[classIndex]+1 
				{
				backgroundFlag = TRUE;
				incrementPaletteIndexFlag = FALSE;
				
				}	// end "else classNumber != classSymbolPtr[classIndex]+1"
			
			}	// end "else if (classSymbolPtr != NULL)"
		
		if (backgroundFlag)
			blueVectorPtr[vectorIndex] = 
						greenVectorPtr[vectorIndex] = 
									redVectorPtr[vectorIndex] = 65535;
		
		else	// !backgroundFlag 
			{
			if (colorSpecPtr == NULL)
				{
				greenVectorPtr[vectorIndex] = (UInt16)vectorIndex * 256;
				redVectorPtr[vectorIndex] = greenVectorPtr[vectorIndex];
				blueVectorPtr[vectorIndex] = greenVectorPtr[vectorIndex];
				
				}	// end "if (colorSpecPtr == NULL)"
				
			else	// colorSpecPtr != NULL
				{
				if (paletteIndexPtr != NULL)
					index = paletteIndexPtr[paletteIndex];
				
				else	// newPaletteIndexPtr == NULL
					index = paletteIndex + paletteOffset;
				
				greenVectorPtr[vectorIndex] = colorSpecPtr[index].rgb.green;
				redVectorPtr[vectorIndex] = colorSpecPtr[index].rgb.red;
				blueVectorPtr[vectorIndex] = colorSpecPtr[index].rgb.blue;
				
				}	// end "else colorSpecPtr != NULL"
			
			}	// end "else !backgroundFlag"
		
		vectorIndex++;
		
		if (incrementPaletteIndexFlag)
			paletteIndex++;
		
		}	// end "for (classNumber=1; classNumber<=..." 
	
	if (collapseClassCode != kCollapseClass)
		CheckAndUnlockHandle (gisFileInfoPtr->classDescriptionH);
	
}	// end "LoadTIFFColorBuffer"
							


//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void LoadTiffEntry
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
//	Coded By:			Larry L. Biehl			Date: 11/07/2005
//	Revised By:			Larry L. Biehl			Date: 11/08/2005

void LoadTiffEntry (
				TIFF_IFD_Ptr						imageFileDirectoryPtr,
				UInt16								tag,
				SInt16								type,
				UInt32								count,
				UInt32								value)

{
	UInt32								inputCount;
	
	SInt16								inputType;
									
	UInt16								shortValue;
	
			
			// Set the tag number.												
			
   tag = GetShortIntValue ((char*)&tag);
		
			// Set the type code.	
	
	inputType = type;
   type = GetShortIntValue ((char*)&type);
		
			// Set the number of values of the indicated type.	
	
	inputCount = count;
	count = (UInt32)GetLongIntValue ((char*)&count);
		
			// Get the value or the offset.									
	
	if (inputType == kTIFFShort && inputCount == 1)
		{
		shortValue = (UInt16)value;
   	shortValue = (UInt16)((UInt16)GetShortIntValue ((char*)&shortValue));
   	
		value = 0;
		BlockMoveData ((char*)&shortValue, (char*)&value, 2);
			
		}	// end "if (inputType == kTIFFShort && inputCount == 1)" 
		
	else if (inputType == kTIFFLong || kTIFFRational || inputCount > 1)
		value = (UInt32)GetLongIntValue ((char*)&value);
		
	else	// something else
		value = 0;
		
	imageFileDirectoryPtr->tag = (UInt16)tag;
	imageFileDirectoryPtr->type = type;
	imageFileDirectoryPtr->count = count;
	imageFileDirectoryPtr->value = value;
	
}	// end "LoadTiffEntry"    

                   
                   
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void LoadTransformationFile
//
//	Software purpose:	The purpose of this routine is to load the transformation 
//							from the selected disk file to the transformation structure.
//							Currenty the transformation can be a matrix such as an
//							eigenvector/eigenvalue or an offset-gain file.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:			Menus in menus.c
//
//	Coded By:			Larry L. Biehl			Date: 07/12/1993
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

void LoadTransformationFile (void)

{
	CMFileStream						fileStreamBlock; 
	
	FileStringPtr						filePathPtr;
											
	CMFileStream*						fileStreamPtr;
	
	OSType								fileTypePtr[2];
	
	UInt32								count;
	
	SInt16								numberFileTypes,
											tReturnCode,
											versionNumber;
	
	OSErr 								errCode;
	
	Boolean								continueFlag,
											errorMessageFlag,
											fileFoundFlag;
											
																					
	continueFlag = TRUE;
	fileFoundFlag = FALSE;
	errorMessageFlag = FALSE;
	
			// Get a new handle to a block of memory to be used for					
			// file information for the new image window and	
			// initialize the variables and handles in the structure.			
	
	fileStreamPtr = (CMFileStream*)&fileStreamBlock;	     
	InitializeFileStream (fileStreamPtr);
	filePathPtr = (FileStringPtr)GetFilePathPPointerFromFileStream (fileStreamPtr);

			// Get the file name and volume to read transformation file 		
			// from.	First make sure that we have a pointer cursor.				

	SetType (fileStreamPtr, kTRANFileType);
	
	fileTypePtr[0] = kTEXTFileType;
	fileTypePtr[1] = kTRANFileType;
	
	do
		{
		numberFileTypes = 2;
		
		#if defined multispec_mac    
			if (gEventRecord.modifiers & optionKey)
				numberFileTypes = -1; 
		#endif	// defined multispec_mac  
			
		errCode = GetFile (fileStreamPtr, 
									numberFileTypes, 
									fileTypePtr, 
									NULL, 
									NULL,
									NULL, 
									IDS_FileIO62);
		continueFlag = (errCode == noErr) & (filePathPtr[0] != 0);
			
		if (continueFlag)
			{
			MSetCursor (kWait);
			
					// Read the image file name.											
			
			count = 256;		
			errCode = MReadData (fileStreamPtr, 
										&count, 
										&gTransformationMatrix.imageFileName,
										kNoErrorMessages);
				
			if (errCode == eofErr)
				errCode = noErr;		
				
			if (errCode != noErr)
				errorMessageFlag = TRUE;		
										
			else	// errCode == noErr || errCode == eofErr
				{						
						// Determine if this is an offsets/gains file.							
						
				GetFileHeaderValue (kFileIOStrID, 
											IDS_FileIO92, 	// Offsets-Gains
											(char*)&gTransformationMatrix.imageFileName, 
											0,
											kDoNotSkipEqual, 
											&tReturnCode);
													
				if (tReturnCode == 0)
					continueFlag = ReadOffsetGainFile (fileStreamPtr,  
																	&versionNumber,
																	&fileFoundFlag,
																	&errCode);
					
				else	// tReturnCode != 0, i.e. transformation file.
					{
					if (errCode == noErr)
						continueFlag = ReadTransformationFile (fileStreamPtr, 
																			&versionNumber,
																			&fileFoundFlag,
																			&errCode);
																		
					else	// errCode != noErr
						errorMessageFlag = TRUE;
						
					}	// end "else tReturnCode != 0, i.e. transformation file."
				
				}	// end "if (errCode == noErr)"
			
			if (errorMessageFlag)
				IOCheck (errCode, fileStreamPtr); 
				
			MInitCursor ();
				
			if (!fileFoundFlag)
				{
					// Beep to alert user that file is not a transformation file	
					// that can be read.		
																					
				SysBeep (10);
		
				if (errCode >= 1001 && errCode <= 1003)
					{
							// Alert user about problems determined with the selected
							// transformation file.									
						
					UInt16 stringID = IDS_Alert138;
					if (errCode == 1002)
						stringID = IDS_Alert139;
					else if (errCode == 1003)
						stringID = IDS_Alert140;
									
					if (LoadSpecifiedStringNumberLongP (kAlertStrID, 
																	stringID, 
																	(char*)gTextString,
																	(char*)gTextString2,
																	TRUE,
																	versionNumber,
																	0))
						errCode = DisplayAlert (
												kErrorAlertID, kStopAlert, 0, 0, 0, gTextString);
						
					}	// end "if (errCode >= 1001 & errCode <= 1003)"
				
				}	// end "if (!fileFoundFlag)"
				
			if (errCode != noErr)
				fileFoundFlag = FALSE;
			
			}	// end "if (continueFlag)" 
	
				// Close the transformation file.											
			
		CloseFile (fileStreamPtr);
		
		}		while (!fileFoundFlag && continueFlag);
	
	if (fileFoundFlag)
		{
		gUpdateFileMenuItemsFlag = TRUE;
		gUpdateEditMenuItemsFlag = TRUE;
		gUpdateProcessorMenuItemsFlag = TRUE;
		
				// Force text selection to start from end of present text.			
						
		ForceTextToEnd (); 
	
				// List the dashed line separator string to indicate start loading
				// transformation output description.
				
		ListSpecifiedStringNumber (kSharedStrID, 
												IDS_Shared5, 
												(unsigned char*)gTextString, 
												NULL, 
												gOutputForce1Code, 
												continueFlag);
		
		char* fileNamePtr = (char*)GetFileNamePPointerFromFileStream (fileStreamPtr);
		continueFlag = ListSpecifiedStringNumber (
												kFileIOStrID,
												IDS_FileIO102, 
												NULL, 
												gOutputForce1Code,
												fileNamePtr,
												continueFlag,
												kUTF8CharString);
		
				// Scroll output window to the selection and adjust the 				
				// scroll bar.																		
		
		UpdateOutputWScrolls (gOutputWindow, 1, kDisplayMessage);
 		
 		}	// end "if (fileFoundFlag)" 
	
	else	// !fileFoundFlag 
		ClearTransformationMatrix (FALSE);
	
}	// end "LoadTransformationFile"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ReadOffsetGainFile
//
//	Software purpose:	The purpose of this routine is to read the offset-gain
//							transformation file and load the parameters into the 
//							transformation structure.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/08/1997
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

Boolean ReadOffsetGainFile (
				CMFileStream*						fileStreamPtr,
				SInt16*								versionNumberPtr,
				Boolean*								fileFoundFlagPtr,
				SInt16*								errorCodePtr)

{  
	UInt8									bufferPtr[500];
	    
	HParamBlockRec						paramBlock;
	
	double								gain,
											offset,
											scaleFactor;
	
	HDoublePtr 							gainPtr,
											offsetPtr;
											
	UCharPtr								inputStringPtr;
	
	SInt32								versionNumber;
	
	UInt32								count,
											numberChannels,
											numberEndOfLineBytes;
	
	OSErr 								errCode;
	
	SInt16								endOfLineCode,
											tReturnCode;
	
	Boolean								continueFlag,
											fileFoundFlag;
											
																					
	continueFlag = TRUE;
	fileFoundFlag = FALSE;
	versionNumber = 0;
	numberChannels = 0;
		
				// Read a section of the transformation file. 			
				
	#if defined multispec_mac
		paramBlock.ioParam.ioPosOffset = 0;
		paramBlock.ioParam.ioRefNum = fileStreamPtr->refNum;
		paramBlock.ioParam.ioCompletion = NULL;
		paramBlock.ioParam.ioBuffer = (char*)bufferPtr;
		paramBlock.ioParam.ioReqCount = 500 - 1;	// Allow for \0.
		  
		
			// Find the end of line character that is used in this file.
			
		paramBlock.ioParam.ioPosMode = FindEndOfLineCode (
											(ParmBlkPtr)&paramBlock,
											fileStreamPtr,
											&errCode,
											&endOfLineCode,
											&numberEndOfLineBytes); 
	#endif	// defined multispec_mac
	
	#if defined multispec_win
		paramBlock.ioPosOffset = 0;
		paramBlock.fileStreamPtr = fileStreamPtr;
		paramBlock.ioBuffer = bufferPtr;
		paramBlock.ioReqCount = 500 - 1;	// Allow for \0.  
		
				// Find the end of line character that is used in this file.
			
		paramBlock.ioPosMode = FindEndOfLineCode (
											(ParmBlkPtr)&paramBlock,
											fileStreamPtr,
											&errCode,
											&endOfLineCode,
											&numberEndOfLineBytes);         
	#endif	// defined multispec_win                                                 
				
	inputStringPtr = NULL;							

			// Reread the first line and then skip it.
			
	if (errCode == noErr)
		errCode = GetNextLine ((ParmBlkPtr)&paramBlock, &inputStringPtr);
	
			// Now get the current=new or new=current line.
	
	if (errCode == noErr)
		errCode = GetNextLine ((ParmBlkPtr)&paramBlock, &inputStringPtr);
	
	if (errCode == noErr)
		{					
				// Read whether the offsets and gains are for:
				//		Current image file = new image file requires the input parameters to
				//			be "inverted".
				//		New image file = current image file									
			
		GetFileHeaderValue (kFileIOStrID, 
									IDS_FileIO93,	// New=Current
									(char*)inputStringPtr, 
									0,
									kDoNotSkipEqual, 
									&tReturnCode);
										
		if (tReturnCode == 0)
			versionNumber = 16;
										
		else	// tReturnCode != 0)
			{
			GetFileHeaderValue (kFileIOStrID, 
										IDS_FileIO94, 	// Current=New
										(char*)inputStringPtr, 
										0,
										kDoNotSkipEqual, 
										&tReturnCode);
										
			if (tReturnCode == 0)
				versionNumber = 17;
										
			}	// end "else tReturnCode != 0)"
										
		}	// end "if (errCode == noErr)"
	
			// Now get the number of offsets-gains line.
	
	if (errCode == noErr)
		{				
		errCode = GetNextLine ((ParmBlkPtr)&paramBlock, &inputStringPtr);
					
		if (errCode == noErr)
			{	
					// Get the number of offsets-gains
					
			tReturnCode = sscanf ((char*)inputStringPtr, 
											"%ld\r",
											&numberChannels);
											
			if (tReturnCode != 1)
				numberChannels = 0;
											
			}	// end "if (errCode == noErr)"
										
		}	// end "if (errCode == noErr)"
	
			// Now get the scale factor. This is used to change the scale of the input
			// offset-gains, e.g. from 0-1 to 0 -100.
	
	if (errCode == noErr)
		{			
		errCode = GetNextLine ((ParmBlkPtr)&paramBlock, &inputStringPtr);
					
		if (errCode == noErr)
			{	
					// Get the number of offsets-gains
					
			tReturnCode = sscanf ((char*)inputStringPtr, 
											"%lf\r",
											&scaleFactor);
											
			if (tReturnCode != 1)
				scaleFactor = 1;
											
			}	// end "if (errCode == noErr)"
										
		}	// end "if (errCode == noErr)"
		
	if (errCode == noErr)
		{		
				// Determine if parameters make sense so far.				
		
		fileFoundFlag = TRUE;
				
		if (versionNumber == 0)
			fileFoundFlag = FALSE;
			
		if (numberChannels < 1 || numberChannels > 8192)
			fileFoundFlag = FALSE;
		
		}	// end "if (errCode == noErr)"
		
	if (fileFoundFlag)
		{			
		gTransformationMatrix.createdByCode = (SInt16)versionNumber;
		gTransformationMatrix.numberChannels = (SInt16)numberChannels;
		gTransformationMatrix.numberFeatures = (SInt16)numberChannels; 
	
				// Set up the rest of the memory.
			
				// Get memory for the gain matrix. Allow for a diagonal matrix.
					
		count = (UInt32)gTransformationMatrix.numberFeatures * 
										gTransformationMatrix.numberChannels * sizeof (double);
										
		gTransformationMatrix.eigenVectorHandle = MNewHandle (count);
		continueFlag = (gTransformationMatrix.eigenVectorHandle != NULL);
		
				// Get memory for the offsets vector.			
		
		if (continueFlag)
			{
			count = gTransformationMatrix.numberFeatures * 2 * sizeof (double);
			gTransformationMatrix.eigenValueHandle = MNewHandle (count);
			continueFlag = (gTransformationMatrix.eigenValueHandle != NULL);
			
			}	// end "if (continueFlag)"
		
				// Get memory for the channels vector.										
				
		if (continueFlag)
			{
			count = gTransformationMatrix.numberChannels * sizeof (SInt16);
			gTransformationMatrix.eigenFeatureHandle = MNewHandle (count);
			continueFlag = (gTransformationMatrix.eigenFeatureHandle != NULL);
			
			if (continueFlag)
				{
						// Initialize the channels vector.				
		                                                 
				SInt16* channelsPtr = (SInt16*)GetHandlePointer (
														gTransformationMatrix.eigenFeatureHandle,
														kLock);
													
				for (count=0; count<numberChannels; count++)
					channelsPtr[count] = (SInt16)count;
			
				}	// end "if (continueFlag)"
			
			}	// end "if (continueFlag)"
		
		}	// end "if (errCode == noErr && ...)"
		
	if (continueFlag)
		{
				// Get pointer for the offset values.
				
		offsetPtr = (HDoublePtr)GetHandlePointer (
														gTransformationMatrix.eigenValueHandle,
														kLock);
														
				// Intialize the offset vector.
				
		ZeroMatrix (offsetPtr, numberChannels, 2, kNotSquareMatrix);
										
		offsetPtr = &offsetPtr[gTransformationMatrix.numberChannels];
		
				// Get pointer for the gains. The gains will be read into a diagonal
				// matrix.                 
				
		gainPtr = (HDoublePtr)GetHandlePointer (
														gTransformationMatrix.eigenVectorHandle,
														kLock);
														
				// Intialize the gain matrix.
				
		ZeroMatrix (gainPtr, numberChannels, numberChannels, kSquareInputMatrix);
		
				// Now loop to get the offset and gain for each channel.
		
		for (count=0; count<numberChannels; count++)
			{
					// Now get the number of offsets-gains line.
						
			errCode = GetNextLine ((ParmBlkPtr)&paramBlock, &inputStringPtr);
				
			if (errCode == noErr)
				{
						// Get the offset and gain for the 'count' channel.
					
				tReturnCode = sscanf ((char*)inputStringPtr, 
												"%lf%lf\r",
												&offset,
												&gain);
											
				if (tReturnCode == 2)
					{	
					if (versionNumber == 16)
						{														
						*offsetPtr = offset * scaleFactor;
						*gainPtr = gain * scaleFactor;
						
						}	// end "if (versionNumber == 16)"
					
					else	// versionNumber == 17
						{							
						*offsetPtr = -offset*scaleFactor/gain;
						*gainPtr = scaleFactor/gain;
						
						}	// end "else versionNumber == 17"
						
					offsetPtr++;
					gainPtr += numberChannels + 1;
					
					}	// end "if (tReturnCode == 2)"
				
				else	// tReturnCode != 2	
					errCode = 1;
				
				}	// end "if (errCode == noErr)"
				
			if (errCode != noErr)
				break;
			
			}	// end "for (count=0; count<numberChannels; count++)"	
		
		CheckAndUnlockHandle (gTransformationMatrix.eigenVectorHandle);
		CheckAndUnlockHandle (gTransformationMatrix.eigenValueHandle);
		CheckAndUnlockHandle (gTransformationMatrix.eigenFeatureHandle);
		
		}	// end "if (*errorCodePtr == noErr && ...)" 
	
	*fileFoundFlagPtr = fileFoundFlag;	
	*errorCodePtr = errCode;  
	*versionNumberPtr = (SInt16)versionNumber;
		
	return (continueFlag);
	
}	// end "ReadOffsetGainFile"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ReadTransformationFile
//
//	Software purpose:	The purpose of this routine is to read the eigenvector/eigenvalue
//							transformation file and load the parameters into the 
//							transformation structure.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/08/1997
//	Revised By:			Larry L. Biehl			Date: 09/01/2011

Boolean ReadTransformationFile (
				CMFileStream*						fileStreamPtr,
				SInt16*								versionNumberPtr,
				Boolean*								fileFoundFlagPtr,
				SInt16*								errorCodePtr)

{   
	SInt64								fileSize;
	
	char									*bufferPtr = NULL,
											*charPtr;
											
	double*								doublePtr;
	
	HPtr									hugeCharPtr;
	
	SInt32								eigenvalueValueSize,
											eigenvectorValueSize;
	
	UInt32								computedFileSize1,
											computedFileSize2,
											computedFileSize3,
											count,
											numberValues;
	
	OSErr 								errCode;
	
	SInt16								swapCode,
											versionNumber;
	
	Boolean								changeVersionCreatedByBytesFlag,
											continueFlag,
											fileFoundFlag,
											numberFeaturesReadFlag,
											readBandGroupingFlag,
											swapFlag;
											
	unsigned char						createdByCodeByte,
											endiannessAndVersionByte,
											savedByte;
											
																					
	continueFlag = TRUE;
	fileFoundFlag = FALSE;
	numberFeaturesReadFlag = FALSE;
	readBandGroupingFlag = FALSE;
	swapFlag = FALSE;
	
			// Get the size of the file to help determine whether bytes
			// need to be swapped.
			
	errCode = GetSizeOfFile (fileStreamPtr, &fileSize);
				
	if (errCode == noErr)
		{						
				// Read the number of channels.									
		
		count = sizeof (SInt16);		
		errCode = MReadData (fileStreamPtr, 
									&count, 
									&gTransformationMatrix.numberChannels,
									kErrorMessages);
								
		}	// end "if (errCode == noErr)"
							
	if (errCode == noErr)
		{		
				// Read the endianness and version number of the file.								
				// 0 is the first version, 1 is the second version, etc.	
		
		count = 1;		
		errCode = MReadData (fileStreamPtr, 
									&count, 
									&endiannessAndVersionByte,
									kErrorMessages);
						
		versionNumber = (0x0f & endiannessAndVersionByte);	
			
		swapCode = (0xf0 & endiannessAndVersionByte) >> 4;		
		if ((swapCode == 1 && !gBigEndianFlag) ||
					(swapCode == 2 && gBigEndianFlag))
			swapFlag = TRUE;
		
		}	// end "if (errCode == noErr)"  
							
	if (errCode == noErr)
		{		
				// Read the code for how matrix was generated.	
		
		count = 1;		
		errCode = MReadData (fileStreamPtr, 
									&count, 
									&createdByCodeByte,
									kErrorMessages);
						
		gTransformationMatrix.createdByCode = createdByCodeByte;
													
		if (gTransformationMatrix.createdByCode == 21)
			{
			readBandGroupingFlag = TRUE;
			gTransformationMatrix.createdByCode = 2;
			
			}	// end "if (gTransformationMatrix.createdByCode == 21)"
													
		}	// end "if (errCode == noErr)"  
		
	if (errCode == noErr)
		{
				// This section of code tries to catch changes that need to be made
				// to handle reading version 1 and 2 of this file. The version number 
				// and created by code may be swapped. This does not catch all cases.
				// For example it will not catch byte swapping if version is 1 and
				// created-by-code is 1 or version is 2 and created-by-code is 2.
				// Those will be caught by comparing the file size.
		
		changeVersionCreatedByBytesFlag = FALSE;		
		if (versionNumber == 1 && createdByCodeByte == 2 && swapCode == 0)
					// This condition implies that the version byte and createdByCode
					// code were swapped when written to the disk using an OSX version older
					// than August 2011 on Mac Intel computers or by the Windows version.
					// Version 1 did not have tranformation info for feature extraction.
			changeVersionCreatedByBytesFlag = TRUE;
			
		else if ((versionNumber <= 2 || versionNumber == 5) && swapCode != 0)
					// This condition implies that the version byte and createdByCode
					// code were swapped when written to the disk using an OSX version older
					// than August 2011 on Mac Intel computers. This condition will not
					// catch all cases of this. The check against file sizes will catch the
					// rest.
			changeVersionCreatedByBytesFlag = TRUE;
			
		if (changeVersionCreatedByBytesFlag)
			{
			savedByte = endiannessAndVersionByte;
			endiannessAndVersionByte = createdByCodeByte;
			createdByCodeByte = savedByte;
			
					// Reset the version number and endianness
					
			versionNumber = (0x0f & endiannessAndVersionByte);	
				
			swapCode = (0xf0 & endiannessAndVersionByte) >> 4;	
			swapFlag = FALSE;	
			if ((swapCode == 1 && !gBigEndianFlag) ||
						(swapCode == 2 && gBigEndianFlag))
				swapFlag = TRUE;
				
					// Reset the create by code information
							
			gTransformationMatrix.createdByCode = createdByCodeByte;
			if (gTransformationMatrix.createdByCode == 21)
				{
				readBandGroupingFlag = TRUE;
				gTransformationMatrix.createdByCode = 2;
				
				}	// end "if (gTransformationMatrix.createdByCode == 21)"
			
			}	// end "if (changeVersionCreatedByBytesFlag)"
			
		}	// end "if (errCode == noErr)"
		
	if (swapCode > 0 && versionNumber > 3)
			// This is a newer version of the transformation file than this version
			// of MultiSpec can read.
		errCode = 1001;		

	if (errCode == noErr && versionNumber >= 2)
		{
				// Read the number of features.									
		
		count = sizeof (SInt16);		
		errCode = MReadData (fileStreamPtr, 
								&count, 
								&gTransformationMatrix.numberFeatures,
								kErrorMessages);
		numberFeaturesReadFlag = TRUE;
		
		}	// end "if (errCode == noErr && versionNumber >= 2)"
		
	if (versionNumber < 2)
		gTransformationMatrix.numberFeatures = gTransformationMatrix.numberChannels;
	
	eigenvalueValueSize = eigenvectorValueSize = sizeof (double);
	if (errCode == noErr)
		{
		if (swapCode == 0)
			{
					// The swapFlag is not defined yet because this in an older version of
					// the transformation file. Determine how large the file should be 
					// based on the number of channels and number of features. If the 
					// computed file size is not equal to the size that the file actually is
					// then there is a good chance that the bytes need to be swapped.
					// There are three possible sizes for the file:
					//		- all doubles are k68881Bytes (12) bytes (Pre OSX versions)
					//		- eigenvalues are 8 bytes and eigenvectors are 8 bytes stored
					//			as 12 bytes that are left justified. (OSX versions 7/28/2011 &
					//			before.
					//		- all doubles are 8 bytes (Windows and OSX after 7/28/2011 version)
			
			errCode = ReadTransformationFileSetSwapInfo (
																(UInt32)fileSize,
																createdByCodeByte,
																&gTransformationMatrix.numberChannels,
																&gTransformationMatrix.numberFeatures,
																&versionNumber,
																&readBandGroupingFlag,
																&gTransformationMatrix.createdByCode,
																&computedFileSize1,
																&computedFileSize2,
																&computedFileSize3,
																&swapFlag);
															
			if (errCode == noErr && versionNumber == 2 && !numberFeaturesReadFlag)
				{
						// Read the number of features.									
				
				count = sizeof (SInt16);		
				errCode = MReadData (fileStreamPtr, 
											&count, 
											&gTransformationMatrix.numberFeatures,
											kErrorMessages);
										
				if (swapFlag)
					Swap2Bytes ((UInt16*)&gTransformationMatrix.numberFeatures, 1);
				
				}	// end "if (errCode == noErr && versionNumber >= 2)"

					// Now set the sizes for the expected double values stored on the disk.
					
			if (computedFileSize1 == fileSize)
				{
				eigenvalueValueSize = k68881Bytes;
				eigenvectorValueSize = k68881Bytes;
				
				}	// end "if (computedFileSize1 == fileSize)"
				
			else if (computedFileSize2 == fileSize)
				eigenvectorValueSize = k68881Bytes;	
						
			}	// end "if (swapCode == 0)"
			
		else	// swapCode != 0
			{
					// This will be for version 3 or later.
			if (swapFlag)
				{
				Swap2Bytes ((UInt16*)&gTransformationMatrix.numberChannels, 1);
				Swap2Bytes ((UInt16*)&gTransformationMatrix.numberFeatures, 1);
				
				}	// end "if (swapFlag)"
			
			}	// end "else swapCode != 0"

		}	// end "if (errCode == noErr)"
		
	if (errCode == noErr)
		{
				// Determine if parameters make sense so far.				
		
		fileFoundFlag = TRUE;		
		if (gTransformationMatrix.numberChannels < 1 ||
								gTransformationMatrix.numberChannels > 8192)
			fileFoundFlag = FALSE;	
			
		else if (gTransformationMatrix.numberFeatures < 1 ||
								gTransformationMatrix.numberFeatures > 8192)
			fileFoundFlag = FALSE;
				
		else if (gTransformationMatrix.createdByCode < 0 ||
									gTransformationMatrix.createdByCode > 2)
			fileFoundFlag = FALSE;
			
		if (!fileFoundFlag)
			errCode = 1002;
			
		#if defined multispec_win
			if (fileFoundFlag && eigenvalueValueSize == k68881Bytes)
				{
						// Cannot handle this transformation file in Windows version
				fileFoundFlag = FALSE;
				errCode = 1003;
				
				}	// end "if (fileFoundFlag && eigenvalueValueSize == k68881Bytes)"
		#endif	// defined multispec_win 
		
		}	// end "if (errCode == noErr)"
		
	if (errCode == noErr && fileFoundFlag)
		{
				// Set up the rest of the memory.								
				
		count = gTransformationMatrix.numberChannels * sizeof (SInt16);
		gTransformationMatrix.eigenFeatureHandle = MNewHandle (count);
		continueFlag = (gTransformationMatrix.eigenFeatureHandle != NULL);
			
				// Get memory for the band grouping vector if needed.
		
		if (continueFlag && readBandGroupingFlag)
			{			
			count = gTransformationMatrix.numberFeatures * sizeof (UInt16);
			gTransformationMatrix.preProcBandGroupingHandle = MNewHandle (count);
			continueFlag = 
							(gTransformationMatrix.preProcBandGroupingHandle != NULL);
						
			}	// end "if (continueFlag && readBandGroupingFlag)"
			
				// Allow to read in the values in 68881 format which		
				// takes 12 bytes.  The size of the handle will be			
				// reduced at the end of the conversion if needed.			
		
		count = gTransformationMatrix.numberFeatures * 2 * eigenvalueValueSize;
		if (continueFlag)
			gTransformationMatrix.eigenValueHandle = MNewHandle (count);
		continueFlag = (gTransformationMatrix.eigenValueHandle != NULL);
			
				// Get memory for the eigenvector matrix.
					
		count = (UInt32)gTransformationMatrix.numberFeatures * 
									gTransformationMatrix.numberChannels * sizeof (double);
		if (continueFlag)
			gTransformationMatrix.eigenVectorHandle = MNewHandle (count);
		continueFlag = (gTransformationMatrix.eigenVectorHandle != NULL);
						
				// Get temp buffer to read 'numberFeatures' of the
				// eigenvector matrix  at a time. Allow to read in the 
				// values in 68881 format which takes 12 bytes.
				
		count = (UInt32)gTransformationMatrix.numberFeatures * eigenvectorValueSize;
		if (continueFlag)
			bufferPtr = MNewPointer (count);
		continueFlag = (bufferPtr != NULL);
		
		}	// end "if (errCode == noErr && ...)" 

	if (errCode == noErr && continueFlag && fileFoundFlag)
		{
				// Read the list of features that were used.					
		                                                 
		hugeCharPtr = (HPtr)GetHandlePointer (
										gTransformationMatrix.eigenFeatureHandle,
										kLock);
											
		count = gTransformationMatrix.numberChannels * sizeof (SInt16);	
		errCode = MReadData (fileStreamPtr, 
								&count, 
								hugeCharPtr,
								kErrorMessages);
								
		if (swapFlag)
			Swap2Bytes ((HUInt16Ptr)hugeCharPtr, gTransformationMatrix.numberChannels);
			
		CheckAndUnlockHandle (gTransformationMatrix.eigenFeatureHandle);
		
		}	// end "if (errCode == noErr && ...)" 

	if (errCode == noErr && continueFlag && fileFoundFlag && readBandGroupingFlag)
		{
				// Read the list of features that were used.					
		                                                 
		hugeCharPtr = (HPtr)GetHandlePointer (
										gTransformationMatrix.preProcBandGroupingHandle,
										kLock);
										
		count = gTransformationMatrix.numberFeatures * sizeof (SInt16);	
		errCode = MReadData (fileStreamPtr, 
								&count, 
								hugeCharPtr,
								kErrorMessages); 
								
		if (swapFlag)
			Swap2Bytes ((HUInt16Ptr)hugeCharPtr, gTransformationMatrix.numberFeatures);
								
		CheckAndUnlockHandle (gTransformationMatrix.preProcBandGroupingHandle);
		
		}	// end "if (errCode == noErr && ...)" 

	if (errCode == noErr && continueFlag && fileFoundFlag)
		{
				// Read the eigenvalues and principal component means.
				
		hugeCharPtr = (HPtr)GetHandlePointer (
										gTransformationMatrix.eigenValueHandle,
										kLock);
										
		numberValues = gTransformationMatrix.numberFeatures;
		if (versionNumber > 0)
			numberValues *= 2;
			
		count = numberValues * eigenvalueValueSize; 
		
		errCode = MReadData (fileStreamPtr, 
								&count, 
								hugeCharPtr,
								kErrorMessages);
                      
		#if defined multispec_mac
			#ifndef _MC68881_ 
				if (eigenvalueValueSize == k68881Bytes)
					{
					doublePtr = (double*)hugeCharPtr;
					charPtr = (char*)hugeCharPtr;
					
					for (count=0; count<numberValues; count++)
						{
						x96tononx96 ((extended96*)charPtr, doublePtr);
						
						doublePtr++;
						charPtr += k68881Bytes;
								
						}	// end "for (count=0; ..." 
					
							// Now changed the size of the handle back to allowing only
							// 8 bytes per double value.
											
					CheckAndUnlockHandle (gTransformationMatrix.eigenValueHandle);	
					count = 2 * gTransformationMatrix.numberFeatures * sizeof (double);
					SetHandleSize (gTransformationMatrix.eigenValueHandle, (Size)count);
					MHLock (gTransformationMatrix.eigenValueHandle);
					
							// For this case the eigenvalues, principal component means and
							// eigenvectors do not need to be swaped.
							
					swapFlag = FALSE;
					
					}	// end "if (eigenvalueValueSize == k68881Bytes)"
				
			#endif	// not defined _MC68881_ 
		#endif	// defined multispec_mac

		if (swapFlag)
			Swap8Bytes ((DoublePtr)hugeCharPtr, numberValues);
		
		if (versionNumber == 0)
			{
					// Set the principal component means to 0.  They 		
					// were not saved in version 0 of this file.
				
			doublePtr = (double*)GetHandlePointer (
												gTransformationMatrix.eigenValueHandle);
			doublePtr = &doublePtr[gTransformationMatrix.numberFeatures];
			for (count=0; count<numberValues; count++)
				{
				*doublePtr = 0.;
				doublePtr++;
				
				}	// end "for (count=0; count<..." 
			
			}	// end "if (versionNumber == 0)" 

		CheckAndUnlockHandle (gTransformationMatrix.eigenValueHandle);
		
		}	// end "if (errCode == noErr && ...)" 
		
	if (errCode == noErr && continueFlag && fileFoundFlag)
		{
		double		doubleValue;
		
		UInt32		i,
						j;
						
						
				// Read the eigenvectors.                 
				
		HDoublePtr eigenVectorPtr = (HDoublePtr)GetHandlePointer (
										gTransformationMatrix.eigenVectorHandle,
										kLock);
				                 
		count = gTransformationMatrix.numberFeatures * eigenvectorValueSize; 
		
		for (i=0; i<(UInt32)gTransformationMatrix.numberChannels; i++)
			{	
			errCode = MReadData (fileStreamPtr, 
								&count, 
								bufferPtr,
								kErrorMessages);
								
			if (errCode != noErr)
				break;
			
			charPtr = bufferPtr;
			for (j=0; j<(UInt32)gTransformationMatrix.numberFeatures; j++)
				{  
				#if defined multispec_mac
					#ifdef _MC68881_						
						doublePtr = (double*)charPtr;
						doubleValue = *doublePtr;
							
					#else	// !_MC68881_ 
						if (eigenvalueValueSize == k68881Bytes)
							{
									// This condition indicates that the eigenvectors
									// are stored as old style 80 bit doubles.
									
							doubleValue = 0;
							x96tononx96 ((extended96*)charPtr, &doubleValue);

							}	// end "if (eigenvalueValueSize == k68881Bytes)"
													
						else	// eigenvalueValueSize == 8
									// This condition indicates that the eigenvectors
									// are stored as 64-bit doubles in either 8 bytes or
									// 12 bytes which is handled by "eigenvectorValueSize"
									// below.
									 
							{
							doublePtr = (double*)charPtr;
							doubleValue = *doublePtr;
								
							}	// end "else eigenvectorValueSize == 8"
					#endif	// not _MC68881_ 
				#endif	// defined multispec_mac  
					
				#if defined multispec_win 
					doublePtr = (double*)charPtr;
					doubleValue =  *doublePtr;
				#endif	// defined multispec_win				
								
				if (swapFlag)
					Swap8Bytes (&doubleValue, 1);
				
				eigenVectorPtr[i+j*gTransformationMatrix.numberChannels] = doubleValue;
								
				charPtr += eigenvectorValueSize;
						
				}	// end "for (j=0; j<...numberFeatures; j++)" 
				
			}	// end "for (i=0; i<...numberChannels; i++)"
		
		CheckAndUnlockHandle (gTransformationMatrix.eigenVectorHandle);
		
		}	// end "if (errCode == noErr && ...)"
		
	CheckAndDisposePtr (bufferPtr);
	
	*fileFoundFlagPtr = fileFoundFlag;	
	*errorCodePtr = errCode; 
	*versionNumberPtr = versionNumber;
		
	return (continueFlag);
	
}	// end "ReadTransformationFile"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ReadTransformationFileSetSwapInfo
//
//	Software purpose:	The purpose of this routine is to determine whether the 
//							transformation matrix data being read from disk needs to be 
//							swapped or not. This is somewhat complex since version 2 and 
//							earlier did not include a code indicating what the endianness
//							of the data were. The basis for swapping or not swapping is
//							based on comparing the actual size of the file with the computed
//							size of the file for 3 possible scenarios.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/09/2011
//	Revised By:			Larry L. Biehl			Date: 09/01/2011

SInt16 ReadTransformationFileSetSwapInfo (
				UInt32								fileSize,
				unsigned char						createdByCodeByte,
				SInt16*								numberChannelsPtr,
				SInt16*								numberFeaturesPtr,
				SInt16*								versionNumberPtr,
				Boolean*								readBandGroupingFlagPtr,
				SInt16*								createdByCodePtr,
				UInt32*								computedFileSize1Ptr,
				UInt32*								computedFileSize2Ptr,
				UInt32*								computedFileSize3Ptr,
				Boolean*								swapFlagPtr)

{
	SInt16								errCode = noErr,
											numberChannels,
											numberFeatures,
											versionNumber;
	
	Boolean								readBandGroupingFlag;
											
	
			// Initialize local variables.
			
	numberChannels = *numberChannelsPtr;
	numberFeatures = *numberFeaturesPtr;
	versionNumber = *versionNumberPtr;
	readBandGroupingFlag = *readBandGroupingFlagPtr;
				
			// Get the file sizes for the 3 possible cases.
			
	ReadTransformationGetFileSizes (fileSize,
												createdByCodeByte,
												(SInt16)numberChannels,
												(SInt16)numberFeatures,
												&versionNumber,
												&readBandGroupingFlag,
												createdByCodePtr,
												computedFileSize1Ptr,
												computedFileSize2Ptr,
												computedFileSize3Ptr);
																									
	if (*computedFileSize1Ptr != fileSize && 
			*computedFileSize2Ptr != fileSize &&
				*computedFileSize3Ptr != fileSize)
		{			
				// This implies that the bytes need to be swapped. Swap the bytes
				// and continue. 
				
		*swapFlagPtr = TRUE;
		Swap2Bytes ((UInt16*)&numberChannels, 1);
		Swap2Bytes ((UInt16*)&numberFeatures, 1);
		
				// Now get the sizes again.
			
		ReadTransformationGetFileSizes (fileSize,
													createdByCodeByte,
													(SInt16)numberChannels,
													(SInt16)numberFeatures,
													&versionNumber,
													&readBandGroupingFlag,
													createdByCodePtr,
													computedFileSize1Ptr,
													computedFileSize2Ptr,
													computedFileSize3Ptr);
																																				
		if (*computedFileSize1Ptr > fileSize && 
				*computedFileSize2Ptr > fileSize &&
					*computedFileSize3Ptr > fileSize)
			errCode = 1002;
													
		}	// end "if (computedFileSize1 != fileSize && ..."
		
	*numberChannelsPtr = (SInt16)numberChannels;
	*numberFeaturesPtr = (SInt16)numberFeatures;
	*versionNumberPtr = versionNumber;
	*readBandGroupingFlagPtr = readBandGroupingFlag;
	
	return (errCode);
	
}	// end "ReadTransformationFileSetSwapInfo"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ReadTransformationGetFileSizes
//
//	Software purpose:	The purpose of this routine is to determine the 3 possible
//							transformation matrix file sizes.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/11/2011
//	Revised By:			Larry L. Biehl			Date: 08/11/2011

void ReadTransformationGetFileSizes (
				UInt32								fileSize,
				unsigned char						createdByCodeByte,
				SInt16								numberChannels,
				SInt16								numberFeatures,
				SInt16*								versionNumberPtr,
				Boolean*								readBandGroupingFlagPtr,
				SInt16*								createdByCodePtr,
				UInt32*								computedFileSize1Ptr,
				UInt32*								computedFileSize2Ptr,
				UInt32*								computedFileSize3Ptr)

{
	UInt32								baseComputedFileSize,
											computedFileSize1,
											computedFileSize2,
											computedFileSize3,
											numberStoredValues;
											
	SInt16								versionNumber;
	
	Boolean								readBandGroupingFlag;
											
	
			// Initialize local variables.
			
	versionNumber = *versionNumberPtr;
	readBandGroupingFlag = *readBandGroupingFlagPtr;
				
			// Get the file sizes for the 3 possible cases.
			
	baseComputedFileSize = 256 + 2 * sizeof (SInt16) + numberChannels * sizeof (SInt16);
														
	if (versionNumber == 2)
		baseComputedFileSize += sizeof (SInt16);
	
	if (readBandGroupingFlag) 
		baseComputedFileSize += numberFeatures * sizeof (SInt16);
		
	numberStoredValues = 2 * numberFeatures + numberFeatures * numberChannels;
		
	computedFileSize1 = baseComputedFileSize + numberStoredValues * k68881Bytes;
		
	computedFileSize2 = baseComputedFileSize + 
											2 * numberFeatures * sizeof (double) +
													numberFeatures * numberChannels * k68881Bytes;
		
	computedFileSize3 = baseComputedFileSize + numberStoredValues * sizeof (double);
														
	if (computedFileSize2 == fileSize - 2)
		{
				// This implies that this transformation file was created by an OSX version
				// before August 2011 running on an Intel computer. The Version number and 
				// byte code are swapped.
				
		versionNumber = createdByCodeByte;
		
		*createdByCodePtr = *versionNumberPtr;
		readBandGroupingFlag = FALSE;
		if (*createdByCodePtr == 21)
			{
			readBandGroupingFlag = TRUE;
			*createdByCodePtr = 2;
			
			}	// end "if (gTransformationMatrix.createdByCode == 21)"
			
		computedFileSize2 += 2;
		
		}	// end "if (computedFileSize2 == fileSize - 2)"
		
	*versionNumberPtr = versionNumber;
	*readBandGroupingFlagPtr = readBandGroupingFlag;
	*computedFileSize1Ptr = computedFileSize1;
	*computedFileSize2Ptr = computedFileSize2;
	*computedFileSize3Ptr = computedFileSize3;
	
}	// end "ReadTransformationGetFileSizes"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean SaveImageWindowAs
//
//	Software purpose:	The purpose of this routine is to save the selected data
//							in the active image window to a disk file.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/21/1994
//	Revised By:			Larry L. Biehl			Date: 09/05/2017

void SaveImageWindowAs (void)

{    
	CMFileStream						fileStreamBlock;  
	
	Rect									offScreenRectangle;
	      
	CMFileStream						*fileStreamPtr,
											*imageFileStreamPtr;
											
	DisplaySpecsPtr					displaySpecsPtr;
	UCharPtr								filePathPtr;
	
	Handle								displaySpecsHandle,
											windowInfoHandle; 
	
	SInt32								numberBytes; 
	
	UInt32								endMapColumn,
											endMapLine,
											startMapColumn,
											startMapLine;
											
	SInt16			 					errCode,
											handleStatus;
	
	Boolean								continueFlag; 
	
	
			// Get pointers to active image window information structures.			
			
	if (SetUpActiveImageInformationGlobals (
											&windowInfoHandle, &handleStatus, kDoNotUseProject))
		{				
				// Get pointer to file information structure and initialize.		
			
		fileStreamPtr = &fileStreamBlock;                        
		InitializeFileStream (fileStreamPtr);
			
					// Make the default volume and folder the same as that for the 
					// active image file.											
		
		imageFileStreamPtr = GetFileStreamPointer (gImageFileInfoPtr);
		SetVolumeReference (imageFileStreamPtr, fileStreamPtr);
				
				// Get pointer to the display information structure for the			
				// active image window.							
		
		displaySpecsHandle = GetDisplaySpecsHandle (gImageWindowInfoPtr);				
		displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (displaySpecsHandle,
																				kLock);
					
				// Get the portion of the image to be saved to disk.					

		GetSelectedOffscreenRectangle (gImageWindowInfoPtr, 
													&offScreenRectangle, 
													TRUE,
													TRUE);
		
				// Estimate number of bytes that will be saved to the output disk	
				// file.																				
				
		numberBytes = (offScreenRectangle.right - offScreenRectangle.left) *
											(offScreenRectangle.bottom - offScreenRectangle.top);
						
		if (displaySpecsPtr->pixelSize > 8)
			numberBytes *= 3;
			
		numberBytes += 2000;
	
				// Set up default name.															
				// Get the output file name and volume to write file to.	Make the 
				// default name the same as the active image window name.
				// If a workflow process is underway, make the default name the
				// that which is specified in the work flow structure if available.		 	
				// Check if there is enough space on the selected volume.				
				
		filePathPtr = (FileStringPtr)GetFilePathPPointerFromFileStream (fileStreamPtr);
				
		if (gMultiSpecWorkflowInfo.workFlowCode > 0 &&
					gMultiSpecWorkflowInfo.defaultName[0] > 0)
			CopyPToP (filePathPtr, gMultiSpecWorkflowInfo.defaultName);
			
		else	// gMultiSpecWorkflowInfo.workFlowCode == 0 || ...
			{
			GetImageWindowName (displaySpecsPtr, 
										gImageFileInfoPtr,
										gTextString,
										TRUE);          
			CopyPToP (filePathPtr, gTextString);
			
			}	// end "else gMultiSpecWorkflowInfo.workFlowCode == 0 || ..."
			
		//RemoveSuffix ((FileStringPtr)filePathPtr, kASCIICharString);
		RemoveSuffix ((FileStringPtr)filePathPtr);
		                                                    
		#if defined multispec_mac               
			ConcatFilenameSuffix ((FileStringPtr)filePathPtr, (StringPtr)"\0.tif\0");
		#endif	// defined multispec_mac	               
		                                                    
		#if defined multispec_win || defined multispec_lin         
			ConcatFilenameSuffix ((FileStringPtr)filePathPtr, (StringPtr)"\0.tif\0");
		#endif	// defined multispec_win || defined multispec_lin
		
				// Now get wide character and unicode names.
		
		SetFileDoesNotExist (fileStreamPtr, kKeepUTF8CharName);
		UpdateFileNameInformation (fileStreamPtr, NULL);
	                                
		SetType (fileStreamPtr, kTIFFFileType);
		errCode = PutFile (fileStreamPtr, numberBytes, IDS_FileIO69, gCreator);
		continueFlag = (errCode == noErr) && FileExists (fileStreamPtr);

				// Write the file.																
		
		if (continueFlag)
			{
			MSetCursor (kWait);
				
			startMapColumn = displaySpecsPtr->displayedColumnStart + 
						offScreenRectangle.left * displaySpecsPtr->displayedColumnInterval;
				
			startMapLine = displaySpecsPtr->displayedLineStart + 
						offScreenRectangle.top * displaySpecsPtr->displayedLineInterval;
			
			continueFlag = WriteTIFFImageFile (gImageFileInfoPtr,
															fileStreamPtr,
															displaySpecsPtr, 
															&offScreenRectangle,
															kFromImageWindow,
															kClassDisplay,
															kNoPaletteColorsDefined,
															NULL,
															startMapColumn,
															startMapLine);
																							
			if (continueFlag)
				{
				char* fileNamePtr = 
									(char*)GetFileNameCPointerFromFileStream (fileStreamPtr);
				GetActiveImageWindowTitle (gTextString2);
				
				sprintf ((char*)gTextString,
							"'%s' image window saved as TIFF file -'%s'.%s%s",
							&gTextString2[1],
							fileNamePtr,
							gEndOfLine,
							gEndOfLine);
							
				gOutputForce1Code = 0x0001;
				continueFlag = OutputString (NULL, 
														(char*)gTextString, 
														0, 
														gOutputForce1Code, 
														continueFlag,
														kUTF8CharString);
		
						// Scroll output window to the selection and adjust the 	
						// scroll bar.															
				
				UpdateOutputWScrolls (gOutputWindow, 1, kDisplayNoMessage);
				
				}	// end "if (continueFlag)"
															
			if (continueFlag && DetermineIfInverseLatLongPossible (windowInfoHandle))
				{
				/*
				GetCopyOfPFileNameFromFileStream (fileStreamPtr, savedImageFileName);
				
						// Close the tiff file.	
						
				vRefNum = GetVolumeReferenceNumber (fileStreamPtr);
				CloseFile (fileStreamPtr);
				
						// Get the default kml file name.
		
				RemoveSuffix (filePathPtr);
				ConcatFilenameSuffix (filePathPtr, (StringPtr)"\0.kml\0");
	
						// Force the uniFileName to be recreated to match the kml file
						// name.
						
				fileStreamPtr->uniFileName.length = 0;
				
						// Now open the kml file.
						
				SetType (fileStreamPtr, kTEXTFileType);
    	
				errCode = CreateNewFile (fileStreamPtr, 
													vRefNum, 
													-1, 
													kErrorMessages,
													kReplaceFlag);
								
							// Set position to start writing data at the start of the file.
					
				if ((errCode == noErr) && FileExists (fileStreamPtr))
					errCode = MSetMarker (fileStreamPtr, fsFromStart, 0, kErrorMessages);
				*/				
				endMapColumn = displaySpecsPtr->displayedColumnStart + 
						offScreenRectangle.right * displaySpecsPtr->displayedColumnInterval;
				
				endMapLine = displaySpecsPtr->displayedLineStart + 
						offScreenRectangle.bottom * displaySpecsPtr->displayedLineInterval;
		
				if (errCode == noErr)
					WriteKMLFile (gImageFileInfoPtr, 
										fileStreamPtr,
										displaySpecsPtr->displayedColumnInterval,
										displaySpecsPtr->displayedLineInterval,
										startMapColumn,
										startMapLine,
										endMapColumn,
										endMapLine);
				
				}	// end "if (continueFlag && ..."
												
			}	// end "if (continueFlag)" 
			
		CheckAndUnlockHandle (displaySpecsHandle);

				// Close the file and flush the volume, to make sure disk is 		
				// correct.																			
	
		CloseFile (fileStreamPtr);
		
		MInitCursor ();
		
		}	// end "SetUpActiveImageInformationGlobals (..." 
		
	UnlockActiveImageInformationGlobals (handleStatus);
	
}	// end "SaveImageWindowAs" 



#if defined multispec_mac
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean SaveTextOutputAs
//
//	Software purpose:	The purpose of this routine is to save the text in the text 
//							output window to a disk file to be named by the user.
//
//							NOTE.  The outline for this routine came from THINK's CAPPS 
//							editor construction kit.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			?							Date: ??/??/????
//	Revised By:			Larry L. Biehl			Date: 02/07/2018

Boolean SaveTextOutputAs (void)

{
									
	CMFileStream*						fileStreamPtr;
	
	Boolean								abortedFlag = FALSE;
	Handle								windowInfoHandle;
	
	#if use_mlte_for_text_window
		TXNOffset							beginSelect = 0,
												endSelect = 0;
	#endif
	#if !use_mlte_for_text_window		
		SInt32								beginSelect = 0,
												endSelect = 0;		 
	#endif

	SInt32								textLength;
	
	SInt16	 							errCode;
									
	SignedByte							handleStatus;

	
			// Get a handle to a block of memory to be used for file information	
			// for the text window output file. Lock the memory until the 			
			// information has been loaded in. Then get the pointer to the file 	
			// information block.	
	
	windowInfoHandle = (Handle)GetWRefCon (gOutputWindow);
	
	fileStreamPtr = GetFileStreamPointer (windowInfoHandle, &handleStatus);
					
	if (fileStreamPtr != NULL)
		{
				// Get the number of characters of text to be written to disk.		
				// If nothing is selected then save all the text to disk.	
					
		#if use_mlte_for_text_window				
			TXNGetSelection (gOutputTextH, &beginSelect, &endSelect); 
		#endif
		
		#if !use_mlte_for_text_window				
			WEGetSelection (&beginSelect, &endSelect, gOutputTextH); 
		#endif
		
		textLength = endSelect - beginSelect;
		
		if (textLength == 0)
			{
			beginSelect = 0;
			#if use_mlte_for_text_window
				textLength = TXNDataSize (gOutputTextH); 
						// Note that characters are stored as unicode (2 bytes/character)
				endSelect = textLength/2;
			#endif
			
			#if !use_mlte_for_text_window
				textLength = WEGetTextLength (gOutputTextH); 
				textLength -= 2;
				endSelect = textLength;
			#endif			
			}	// end "if (textLength == 0)"

			// Set up default name.																
			// Get the output file name and volume to write file to.	Make the 	
			// default name the same as the output window name minus ".Project"	
			// if it is there and add ".txt" at the end.  Check if there 	
			// is enough space on the selected volume.									
	
		GetWTitle (gOutputWindow, gTextString);
		CopyPToP (fileStreamPtr->fileName, gTextString);
		//RemoveCharsNoCase ((char*)"\0.Project\0", fileStreamPtr->fileName);
		//fileStreamPtr->uniFileName.length = 0;
		//UpdateFileNameInformation (fileStreamPtr, NULL);
		ConcatFilenameSuffix (fileStreamPtr->fileName, (UCharPtr)"\p.txt");
		fileStreamPtr->uniFileName.length = 0;
		UpdateFileNameInformation (fileStreamPtr, NULL);
	
		fileStreamPtr->type = 'TEXT';
		errCode = PutFile (fileStreamPtr, textLength, IDS_SaveTextOutputAs, -1);
		abortedFlag = (errCode != noErr) | (fileStreamPtr->fileName[0] == 0);

				// Write the file.																
		
		HLock ((Handle)gOutputTextH);
		if (!abortedFlag)
			{
			MSetCursor (kWait);
			
			abortedFlag = !WriteTextOutputFile (fileStreamPtr->refNum, 
															(Str255*)&fileStreamPtr->fileName,
															gOutputTextH, 
															beginSelect, 
															endSelect);
												
			}	// end "if (!abortedFlag)" 
		
		HUnlock ((Handle)gOutputTextH);

				// Close the file and flush the volume, to make sure disk is 		
				// correct.																			
	
		CloseFile (fileStreamPtr);
		
		MInitCursor ();
										
		}	// end "if (fileStreamPtr != NULL)" 
										
			// Unlock the file info handle.

	UnlockFileStream (windowInfoHandle, handleStatus);
	
	if (!abortedFlag)
		SetOutputWindowChangedFlag (FALSE);
	
	return (!abortedFlag);
	
}	// end "SaveTextOutputAs" 
#endif	// defined multispec_mac  



//------------------------------------------------------------------------------------
//								 	Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		char* SkipToNextCarriageReturn
//
//	Software purpose:	This routine finds the next carriage return in the input string 
//							and returns the location just after the carriage return.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	The new input string pointer.
//
// Called By:			GetNextLine in SSaveWrt.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/05/1992
//	Revised By:			Larry L. Biehl			Date: 03/16/2017

UCharPtr SkipToNextCarriageReturn (
				UCharPtr 							inputStringPtr)

{
	if (inputStringPtr != NULL)
		{                                                                             
		while (*inputStringPtr != kCarriageReturn && 
						*inputStringPtr != kNullTerminator &&
						 *inputStringPtr != kNewLineCharacter)
			inputStringPtr++;                                                         
		
		if (*inputStringPtr != kNullTerminator)
			{    
			inputStringPtr++;
				
			if (*inputStringPtr == kNewLineCharacter) 
				inputStringPtr++; 
			
			}	// end "if (*inputStringPtr != kNullTerminator)"
			
		else	// *inputStringPtr == kNullTerminator
			inputStringPtr = NULL;
			
		}	// end "if (inputStringPtr != NULL)" 
	
	return (inputStringPtr);
		
}	// end "SkipToNextCarriageReturn" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean WriteArcViewWorldFile
//
//	Software purpose:	This routine writes an ARCView header BLW file.
//
//	Parameters in:		file information pointer
//							pointer to header record information
//
//	Parameters out:	None
//
//	Value Returned:	true - if ArcView header file written without problem
//							false - if error writing ArcView header file.
//
// Called By:			WriteNewImageHeader in SReform2.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/13/2000
//	Revised By:			Larry L. Biehl			Date: 04/09/2018

Boolean WriteArcViewWorldFile (
				FileInfoPtr 						fileInfoPtr)

{
	char									buffer[300];
	
	double								zeroValue = 0.;
	
	CMFileStream						blwFileStream;
	
	FileStringPtr						blwFileNamePtr,
											imageFileNamePtr;
											
	CMFileStream						*fileStreamPtr,
											*blwStreamPtr;
											
	MapProjectionInfoPtr				mapProjectionInfoPtr;
											
	UInt32								count;
	
	SInt16								errCode = noErr;
	
	Boolean								planarCoordinateInfoFlag = FALSE;
	
			
	if (fileInfoPtr != NULL)
		{
				// Determine if any planar coordinate information exists to write
				// to a blw file. First, get a pointer to the map projection information 
				// if it exists.		
	
		mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																	fileInfoPtr->mapProjectionHandle,
																	kLock);
		
		if (mapProjectionInfoPtr != NULL)
			{																
			if (mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize != 0 &&
					mapProjectionInfoPtr->planarCoordinate.verticalPixelSize != 0)
				planarCoordinateInfoFlag = TRUE;
				
			}	// end "if (mapProjectionInfoPtr != NULL)"
			
		if (planarCoordinateInfoFlag)
			{
			errCode = noErr;
			fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
					
					// Get the pointer to the header stream block and initialize it.			
				
			blwStreamPtr = &blwFileStream;
			InitializeFileStream (blwStreamPtr);
				
					// Get pointer to the file name and close the input file. If it is
					// a header file, the information has already been read. If it is
					// an image file then we don't need it for now and need to look for
					// the ArcView header file.
				
			imageFileNamePtr = 
							(FileStringPtr)GetFilePathPPointerFromFileStream (fileStreamPtr);
			blwFileNamePtr = 
							(FileStringPtr)GetFilePathPPointerFromFileStream (blwStreamPtr);
				
					// Get the output file name and volume to write file to.	Make the 
					// default name the same as the image file name with .hdr at the	
					// end. Also check if there is enough space on the selected volume.			
				
			CopyPToP (blwFileNamePtr, imageFileNamePtr);
			RemoveSuffix (blwFileNamePtr);
							
			ConcatFilenameSuffix (blwFileNamePtr, (StringPtr)"\0.blw\0");			
			
			#if defined multispec_mac
						// Force the uniFileName to be recreated to match the support file
						// name.
			
				blwStreamPtr->uniFileName.length = 0;
			#endif	// defined multispec_mac
			
			SetType (blwStreamPtr, kTEXTFileType);
				
					// Create the header file in the same volume as that for the image file.
					
			SetVolumeReference (fileStreamPtr, blwStreamPtr);
			SInt16 vRefNum = GetVolumeReferenceNumber (fileStreamPtr);		
			
			errCode = CreateNewFile (blwStreamPtr, 
												vRefNum, 
												'ttxt', 
												kErrorMessages, 
												kReplaceFlag);
			
			if (errCode == noErr)
				{	
						// Now start writing the needed parameters.
					
				count = sprintf (
							buffer, 
							"%35.14f %s%35.14f %s %35.14f %s %35.14f %s %35.14f %s%35.14f%s", 
							mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize,
							gEndOfLine, 
							zeroValue,
							gEndOfLine, 
							zeroValue,
							gEndOfLine, 
							mapProjectionInfoPtr->planarCoordinate.verticalPixelSize,
							gEndOfLine, 
							mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11,
							gEndOfLine, 
							mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11,
							gEndOfLine);
					
				errCode = MWriteData (blwStreamPtr, &count, &buffer, kErrorMessages);
				
				}	// end "if (errCode == noErr)" 
			  		
			  		// Close the blw file											
			  				
			CloseFile (blwStreamPtr);
			
			CheckAndUnlockHandle (fileInfoPtr->mapProjectionHandle);
			
			}	// end "if (planarCoordinateInfoFlag)"
		
		}	// end "if (fileInfoPtr != NULL)"
		
	return (errCode == noErr);
	
}	// end "WriteArcViewWorldFile"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean WriteArcViewHeaderFile
//
//	Software purpose:	This routine writes an ARCView header file.
//
//	Parameters in:		file information pointer
//							pointer to header record information
//
//	Parameters out:	None
//
//	Value Returned:	true - if ArcView header file written without problem
//							false - if error writing ArcView header file.
//
// Called By:			WriteNewImageHeader in SReform2.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/07/2000
//	Revised By:			Larry L. Biehl			Date: 02/07/2018

Boolean WriteArcViewHeaderFile (
				FileInfoPtr 						fileInfoPtr)

{
	CMFileStream						headerFileStream;
	
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	FileStringPtr						headerFileNamePtr,
											imageFileNamePtr;
											
	CMFileStream						*fileStreamPtr,
											*headerStreamPtr;
											
	UInt32								count,
											numberBits;
	
	SInt16								errCode;
											
	Boolean								planarCoordinateInfoFlag = FALSE,
											UTMFlag;
	
	char									byteOrderCharacter;
	
			
   errCode = noErr;
	if (fileInfoPtr != NULL)
		{
		fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
					
				// Get the pointer to the header stream block and initialize it.
				// Then copy info from 'fileInfoPtr' so that the file is written
				// in the same folder.			
			
		headerStreamPtr = &headerFileStream;
		CopyFileStream (headerStreamPtr, fileStreamPtr);
			
				// Get pointer to the file name and close the input file. If it is
				// a header file, the information has already been read. If it is
				// an image file then we don't need it for now and need to look for
				// the ArcView header file.
			
		imageFileNamePtr = 
						(FileStringPtr)GetFilePathPPointerFromFileStream (fileStreamPtr);
		headerFileNamePtr = 
						(FileStringPtr)GetFilePathPPointerFromFileStream (headerStreamPtr);
			
				// Get the output file name and volume to write file to.	Make the 
				// default name the same as the image file name with .hdr at the	
				// end. Also check if there is enough space on the selected volume.			
			
		RemoveSuffix (headerFileNamePtr);
					
		ConcatFilenameSuffix (headerFileNamePtr, (StringPtr)"\0.hdr\0");	

		SetType (headerStreamPtr, kTEXTFileType);
		SInt16 vRefNum = GetVolumeReferenceNumber (fileStreamPtr);
		
				// Make sure that the settings for the header file indicate that it is 
				// closed to start with. Otherwise we may end up closing the file 
				// for 'fileInfoPtr'.
				
		IndicateFileClosed (headerStreamPtr);

				// Now get wide character and unicode names.

		SetFileDoesNotExist (headerStreamPtr, kKeepUTF8CharName);
		UpdateFileNameInformation (headerStreamPtr, NULL);
			
				// Create the header file in the same volume as that for the image file.		
		
		errCode = CreateNewFile (headerStreamPtr, 
											vRefNum, 
											'ttxt', 
											kErrorMessages, 
											kReplaceFlag);
			
				// Now start writing the needed parameters.
		
		if (errCode == noErr)
			{	
					// Write the comment portion of the header.	
				
			count = sprintf ((char*)gTextString, 
										";%s; ArcView Image Information%s;%s",
										gEndOfLine,
										gEndOfLine,
										gEndOfLine);
				
			errCode = MWriteData (headerStreamPtr, &count, gTextString, kErrorMessages);
			
			}	// end "if (errCode == noErr)"
		
		if (errCode == noErr)
			{
					// Write "NCOLS" to the output header file.
				
			count = sprintf ((char*)gTextString, 
									"NCOLS       %ld%s", 
									fileInfoPtr->numberColumns,
									gEndOfLine);
				
			errCode = MWriteData (headerStreamPtr, &count, gTextString, kErrorMessages);
			
			}	// end "if (errCode == noErr)" 
		
		if (errCode == noErr)
			{
					// Write "NROWS" to the output header file.	
				
			count = sprintf ((char*)gTextString, 
									"NROWS       %ld%s", 
									fileInfoPtr->numberLines,
									gEndOfLine);
				
			errCode = MWriteData (headerStreamPtr, &count, gTextString, kErrorMessages);
			
			}	// end "if (errCode == noErr)"  
		
		if (errCode == noErr)
			{	
					// Write "NBANDS" to the output header file.	
				
			count = sprintf ((char*)gTextString, 
									"NBANDS      %d%s", 
									fileInfoPtr->numberChannels,
									gEndOfLine);
				
			errCode = MWriteData (headerStreamPtr, &count, gTextString, kErrorMessages);
			
			}	// end "if (errCode == noErr)"
		
		if (errCode == noErr)
			{	
					// Write "NBITS" to the output header file.
					
			numberBits = fileInfoPtr->numberBits;
			if (numberBits > 8 && numberBits <= 16)
				numberBits = 16;
			else if (numberBits > 16 && numberBits <= 32)
				numberBits = 32;
				
			count = sprintf ((char*)gTextString,
									"NBITS       %d%s", 
									(unsigned int)numberBits,
									gEndOfLine);
				
			errCode = MWriteData (headerStreamPtr, &count, gTextString, kErrorMessages);
			
			}	// end "if (errCode == noErr)" 
		
		if (errCode == noErr)
			{
					// Write "LAYOUT" to the output header file.		
		      
			if (fileInfoPtr->bandInterleave == kBIL)
				count = sprintf ((char*)gTextString, "LAYOUT      BIL%s", gEndOfLine);
					
			else if (fileInfoPtr->bandInterleave == kBSQ)
				count = sprintf ((char*)gTextString, "LAYOUT      BSQ%s", gEndOfLine);
				
			else if (fileInfoPtr->bandInterleave == kBIS)
				count = sprintf ((char*)gTextString, "LAYOUT      BIP%s", gEndOfLine);
				
			else 		// fileInfoPtr->bandInterleave != kBIL, kBSQ, kBIS
				count = sprintf ((char*)gTextString, "LAYOUT      ???%s", gEndOfLine);
				
			errCode = MWriteData (headerStreamPtr, &count, gTextString, kErrorMessages);
			
			}	// end "if (errCode == noErr)"     
		
		if (errCode == noErr && fileInfoPtr->numberBits > 8)
			{	
					// Write "BYTEORDER" to the output header file.
					
			if (gBigEndianFlag)
				{		
				if (fileInfoPtr->swapBytesFlag)
					byteOrderCharacter = 'L';
					
				else	// !fileInfoPtr->swapBytesFlag
					byteOrderCharacter = 'M';
					
				}	// end "if (gBigEndianFlag)"	
			
			else	// !gBigEndianFlag
				{		
				if (fileInfoPtr->swapBytesFlag)
					byteOrderCharacter = 'M';
					
				else	// !fileInfoPtr->swapBytesFlag
					byteOrderCharacter = 'L';
					
				}	// end "else !gBigEndianFlag"
				
			count = sprintf ((char*)gTextString, 
									"BYTEORDER   %c%s", 
									byteOrderCharacter,
									gEndOfLine);
				
			errCode = MWriteData (headerStreamPtr, &count, gTextString, kErrorMessages);
			
			}	// end "if (errCode == noErr && ..."   
		
		if (errCode == noErr)
			{
					// Write "SKIPBYTES" to the output header file.	
				
			count = sprintf ((char*)gTextString, 
									"SKIPBYTES   %ld%s", 
									fileInfoPtr->numberHeaderBytes,
									gEndOfLine);
				
			errCode = MWriteData (headerStreamPtr, &count, gTextString, kErrorMessages);
			
			}	// end "if (errCode == noErr)"     
		
		if (errCode == noErr)
			{
					// Write "BANDROWBYTES" to the output header file if needed.
				
			if (fileInfoPtr->bandInterleave == kBIL && 
						fileInfoPtr->numberPostChannelBytes > 0)
				{
				UInt32 numberBandRowBytes = fileInfoPtr->numberPostChannelBytes +
										fileInfoPtr->numberColumns * fileInfoPtr->numberBytes;
				count = sprintf ((char*)gTextString, 
										"BANDROWBYTES   %ld%s",
										numberBandRowBytes,
										gEndOfLine);
				
				errCode = MWriteData (headerStreamPtr, &count, gTextString, kErrorMessages);
			
				}	// end "if (fileInfoPtr->signedDataFlag)"
				
			}	// end "if (errCode == noErr)"  
			
		if (errCode == noErr)
			errCode =  WriteArcViewMapInformation (fileInfoPtr, 
																headerStreamPtr, 
																&planarCoordinateInfoFlag);
		
		if (errCode == noErr && 
						(planarCoordinateInfoFlag || fileInfoPtr->signedDataFlag))
			{	
					// Now write some other documentation information.	These are not
					// ArcView parameters but they will be useful for documentation and
					// for MultiSpec.
				
			count = sprintf ((char*)gTextString, 
									"; Other Information%s",
									gEndOfLine);
				
			errCode = MWriteData (headerStreamPtr, &count, gTextString, kErrorMessages);
			
			}	// end "if (errCode == noErr)"  
		
		if (errCode == noErr && fileInfoPtr->signedDataFlag)
			{
				// Write "DATAVALUETYPE" to the output header file.
				
			count = sprintf ((char*)gTextString, 
									"DATAVALUETYPE   SIGNED%s",
									gEndOfLine);
			
			errCode = MWriteData (headerStreamPtr, &count, gTextString, kErrorMessages);
				
			}	// end "if (errCode == noErr)" 
		
		if (errCode == noErr)
			{
					// This is for a parameter that gdal uses. It handles what was added 
					// to MultiSpec in the "DATAVALUETYPE" parameter above.
					// Write "PIXELTYPE" to the output header file.
					
			count = sprintf ((char*)gTextString, "PIXELTYPE   ");
			
			if (fileInfoPtr->dataTypeCode == kRealType)
				count += sprintf ((char*)&gTextString[count], 
										"FLOAT");
										
			else if (fileInfoPtr->signedDataFlag)
				count += sprintf ((char*)&gTextString[count], 
										"SIGNEDINT");
										
			else
				count += sprintf ((char*)&gTextString[count], 
										"UNSIGNEDINT");
										
			count += sprintf ((char*)&gTextString[count], "%s", gEndOfLine);
			
			errCode = MWriteData (headerStreamPtr, &count, gTextString, kErrorMessages);
				
			}	// end "if (errCode == noErr)" 
			
				// Determine if any planar coordinate information exists to write
				// other documentation information to the ArcView header file		
	
		mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																	fileInfoPtr->mapProjectionHandle,
																	kLock);
		
		if (mapProjectionInfoPtr == NULL)
			planarCoordinateInfoFlag = FALSE;
			
		if (planarCoordinateInfoFlag)
			{ 
			SInt16 mapUnitsCode = mapProjectionInfoPtr->planarCoordinate.mapUnitsCode;
			if (errCode == noErr && 
						mapUnitsCode >= kDecimalDegreesCode &&
								mapUnitsCode <= kInchesCode)
				{
						// Write "MAP_UNITS" to the output header file.	
						
				count = sprintf ((char*)gTextString, "MAP_UNITS   ");
						
				if (mapUnitsCode == kMetersCode)
					count += sprintf ((char*)&gTextString[count], "meters");
						
				else if (mapUnitsCode == kUSSurveyFeetCode)
					count += sprintf ((char*)&gTextString[count], "us survey feet");
						
				else if (mapUnitsCode == kFeetCode)
					count += sprintf ((char*)&gTextString[count], "feet");
			      
				else if (mapUnitsCode == kDecimalDegreesCode)
					count += sprintf ((char*)&gTextString[count], "decimal degrees");
						
				else if (mapUnitsCode == kKilometersCode)
					count += sprintf ((char*)&gTextString[count], "kilometers");
						
				else if (mapUnitsCode == kCentimetersCode)
					count += sprintf ((char*)&gTextString[count], "centimeters");
						
				else if (mapUnitsCode == kMillimetersCode)
					count += sprintf ((char*)&gTextString[count], "millimeters");
						
				else if (mapUnitsCode == kMicrometersCode)
					count += sprintf ((char*)&gTextString[count], "micrometers");
						
				else if (mapUnitsCode == kMilesCode)
					count += sprintf ((char*)&gTextString[count], "miles");
						
				else if (mapUnitsCode == kYardsCode)
					count += sprintf ((char*)&gTextString[count], "yards");
						
				else if (mapUnitsCode == kInchesCode)
					count += sprintf ((char*)&gTextString[count], "inches");
					
				count += sprintf ((char*)&gTextString[count], "%s", gEndOfLine);
				
				errCode = MWriteData (headerStreamPtr, 
												&count, 
												gTextString, 
												kErrorMessages);
					
				}	// end "if (errCode == noErr && ..."  
				
					// Write DATUM to output header file.
			
			SInt16 datumCode = mapProjectionInfoPtr->geodetic.datumCode;
			if (errCode == noErr && 
						datumCode >= kNAD27Code &&
								datumCode <= kWGS84Code)
				{
						// Write "MAP_UNITS" to the output header file.	
						
				count = sprintf ((char*)gTextString, "DATUM   ");
						
				if (datumCode == kNAD27Code)
					count += sprintf ((char*)&gTextString[count], "NAD27");
						
				else if (datumCode == kNAD83Code)
					count += sprintf ((char*)&gTextString[count], "NAD83");
			      
				else if (datumCode == kWGS84Code)
					count += sprintf ((char*)&gTextString[count], "WGS84");
					
				count += sprintf ((char*)&gTextString[count], "%s", gEndOfLine);
				
				errCode = MWriteData (headerStreamPtr, 
												&count, 
												gTextString, 
												kErrorMessages);
					
				}	// end "if (errCode == noErr && ..." 
				
					// Write PROJECTION_NAME to output header file. 
			
			SInt16 projectionCode = mapProjectionInfoPtr->gridCoordinate.projectionCode;
			SInt16 referenceSystemCode = 
									mapProjectionInfoPtr->gridCoordinate.referenceSystemCode;
							
			UTMFlag = (referenceSystemCode >= kUTM_NAD27RSCode &&
									referenceSystemCode <= kUTMRSCode);
									
			if (errCode == noErr && 
						(UTMFlag || referenceSystemCode == kStatePlaneNAD27RSCode ||
								referenceSystemCode == kStatePlaneNAD83RSCode || 
												projectionCode == kAlbersConicalEqualAreaCode))
				{
				count = sprintf ((char*)gTextString, "PROJECTION_NAME   ");
						
				if (UTMFlag)
					count += sprintf ((char*)&gTextString[count], "UTM");
						
				else if (referenceSystemCode == kStatePlaneNAD27RSCode || 
													referenceSystemCode == kStatePlaneNAD83RSCode)
					count += sprintf ((char*)&gTextString[count], "SPCS");
			      
				else if (projectionCode == kAlbersConicalEqualAreaCode)
					count += sprintf ((char*)&gTextString[count], "ACEA");
					
				count += sprintf ((char*)&gTextString[count], "%s", gEndOfLine);
				
				errCode = MWriteData (headerStreamPtr, 
												&count, 
												gTextString, 
												kErrorMessages);
					
				}	// end "if (errCode == noErr && ..." 
				
					// Write "PROJECTION_ZONE" to the output header file.	
						
			if (errCode == noErr && UTMFlag &&
								mapProjectionInfoPtr->gridCoordinate.gridZone != 0)
				{
				count = sprintf ((char*)gTextString, 
										"PROJECTION_ZONE   %d%s",
										abs (mapProjectionInfoPtr->gridCoordinate.gridZone),
										gEndOfLine);
				
				errCode = MWriteData (headerStreamPtr, 
												&count, 
												gTextString, 
												kErrorMessages);
											
				}	// end "if (errCode == noErr && ..."
			
			CheckAndUnlockHandle (fileInfoPtr->mapProjectionHandle);
	 
	 		}	// end "if (planarCoordinateInfoFlag)"
		  		
		  		// Close the image statistics file											
		  				
		CloseFile (headerStreamPtr);
		
		}	// end "if (fileInfoPtr != NULL)"
		
	return (errCode == noErr);
	
}	// end "WriteArcViewHeaderFile"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean WriteArcViewHeaderFiles
//
//	Software purpose:	This routine writes an ARCView header file.
//
//	Parameters in:		file information pointer
//							pointer to header record information
//
//	Parameters out:	None
//
//	Value Returned:	true - if ArcView header file written without problem
//							false - if error writing ArcView header file.
//
// Called By:			WriteArcViewSupportFiles in SSaveWrt.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/13/2000
//	Revised By:			Larry L. Biehl			Date: 06/13/2002

Boolean WriteArcViewSupportFiles (
				FileInfoPtr 						fileInfoPtr)

{
	Boolean								continueFlag = FALSE;
	
			
	if (fileInfoPtr != NULL)
		{
		continueFlag = WriteArcViewHeaderFile (fileInfoPtr);
		
				// The world file does not add any more information than what is already
				// in the header file. Do not create for now.
		//if (continueFlag)
		//	continueFlag = WriteArcViewWorldFile (fileInfoPtr);
		
		}	// end "WriteArcViewHeaderFile"
		
	return (continueFlag);
		
}	// end "WriteArcViewSupportFiles"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 WriteArcViewMapInformation
//
//	Software purpose:	This routine writes the map information in the ARCView header 
//							file.
//
//	Parameters in:		file information pointer
//							pointer to header record information
//
//	Parameters out:	None
//
//	Value Returned:	true - if ArcView header file written without problem
//							false - if error writing ArcView header file.
//
// Called By:			WriteArcViewHeaderFile in SSaveWrt.cpp
//
//	Coded By:			Larry L. Biehl			Date: 06/13/2002
//	Revised By:			Larry L. Biehl			Date: 01/25/2013

SInt16 WriteArcViewMapInformation (
				FileInfoPtr 						fileInfoPtr,
				CMFileStream*						headerStreamPtr,
				Boolean*								planarCoordinateInfoFlagPtr)

{
	MapProjectionInfoPtr				mapProjectionInfoPtr;
											
	UInt32								count;
	
	SInt16								errCode = noErr;
	
	Boolean								planarCoordinateInfoFlag = FALSE;
	
	
	if (fileInfoPtr != NULL)
		{
				// Determine if any planar coordinate information exists to write
				// to a blw file. First, get a pointer to the map projection information 
				// if it exists.		
	
		mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																	fileInfoPtr->mapProjectionHandle,
																	kLock);
		
		if (mapProjectionInfoPtr != NULL)
			{																
			if (mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize != 0 &&
					mapProjectionInfoPtr->planarCoordinate.verticalPixelSize != 0)
				planarCoordinateInfoFlag = TRUE;
				
			}	// end "if (mapProjectionInfoPtr != NULL)"
			
		if (planarCoordinateInfoFlag)
			{  
					// Write "ULXMAP" to the output header file.	
				
			count = sprintf ((char*)gTextString, 
									"ULXMAP   %35.7f %s", 
									mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11,
									gEndOfLine);
				
			errCode = MWriteData (headerStreamPtr, &count, gTextString, kErrorMessages);
			
			if (errCode == noErr)
				{
						// Write "ULYMAP" to the output header file.	
					
				count = sprintf ((char*)gTextString, 
										"ULYMAP   %35.7f %s", 
										mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11,
										gEndOfLine);
					
				errCode = MWriteData (headerStreamPtr, 
												&count, 
												gTextString, 
												kErrorMessages);
			
				}	// end "if (errCode == noErr)"  
			
			if (errCode == noErr)
				{
						// Write "XDIM" to the output header file.	
					
				count = sprintf (
						(char*)gTextString, 
						"XDIM   %35.11f %s", 
						fabs (mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize),
						gEndOfLine);
					
				errCode = MWriteData (headerStreamPtr, 
												&count, 
												gTextString, 
												kErrorMessages);
			
				}	// end "if (errCode == noErr)"  
			
			if (errCode == noErr)
				{
						// Write "YDIM" to the output header file.	
					
				count = sprintf (
							(char*)gTextString, 
							"YDIM   %35.11f %s", 
							fabs (mapProjectionInfoPtr->planarCoordinate.verticalPixelSize),
							gEndOfLine);
					
				errCode = MWriteData (headerStreamPtr, 
												&count, 
												gTextString,
												kErrorMessages);
			
				}	// end "if (errCode == noErr)" 
			
			CheckAndUnlockHandle (fileInfoPtr->mapProjectionHandle);
			
			}	// end "if (planarCoordinateInfoFlag)"
		
		}	// end "if (fileInfoPtr != NULL)"
		
	*planarCoordinateInfoFlagPtr = planarCoordinateInfoFlag;
		
	return (errCode);
	
}	// end "WriteArcViewMapInformation"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//							  (c) Purdue Research Foundation
//
//	Function name:		Boolean WriteErdasHeader
//
//	Software purpose:	This routine writes an ERDAS formatted header 
//							record to an image file
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			WriteNewErdasHeader in fileIO.c
//							ChangeErdasHeader in reformat.c
//
//	Coded By:			Larry L. Biehl			Date: 08/29/1988
//	Revised By:			Larry L. Biehl			Date: 12/08/2014

Boolean	WriteErdasHeader (
				FileInfoPtr							fileInfoPtr, 
				UInt8*								headerRecordPtr, 
				SInt16								ERDASVersion)

{
	CMFileStream*						fileStreamPtr;
	
	SInt32								posOff;
	
	UInt32								count;
	
	SInt16								errCode;
	
		
	if (!LoadErdasHeader (fileInfoPtr, headerRecordPtr, ERDASVersion))
																							return (FALSE);
																					
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
			
			// Close the file and set file back for write permission					 
			//	Then reopen to prepare for changing the file								
			
	errCode = SetFileWriteEnabled (fileStreamPtr);
	if (errCode != noErr)															return (FALSE);
	
			// Position the marker in the image file to prepare for writing		
			// the header																			
		
	posOff = 0;    
	errCode = MSetMarker (fileStreamPtr, fsFromStart, posOff, kErrorMessages);
	if (errCode != noErr)															return (FALSE);
	
			// Write the updated record to the image file								
	
	count = 128;
	errCode = MWriteData (fileStreamPtr, &count, headerRecordPtr, kErrorMessages);
	if (errCode != noErr)															return (FALSE);
	
	fileInfoPtr->numberHeaderBytes = 128;
	
	return (TRUE);
	
}	// end "WriteErdasHeader" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 WriteGeoTIFFInformation
//
//	Software purpose:	This routine writes the geo information to the TIFF image file
//							if any exists.
//
//	Parameters in:		file information pointer
//							pointer to header record information
//
//	Parameters out:	None
//
//	Value Returned:	
//
// Called By:			WriteTIFFImageFile in SSaveWrt.cpp
//
//	Coded By:			Larry L. Biehl			Date: 09/04/2002
//	Revised By:			Larry L. Biehl			Date: 08/06/2014

SInt16 WriteGeoTIFFInformation (
				FileInfoPtr 						fileInfoPtr,
				CMFileStream*						fileStreamPtr,
				UInt32								directoryStart,
				UInt32								directoryDataStart,
				Boolean								countOnlyFlag,
				UInt32*								numberGeoEntriesPtr,
				UInt32*								numberDataBytesPtr,
				UInt32								columnInterval,
				UInt32								lineInterval,
				UInt32								startMapColumn,
				UInt32								startMapLine)

{
	TIFFImageFileDirectory			imageFileDirectory;
	
	char									geoTiffInfoBuffer[1000];
	
	double								horizontalPixelSize,
											spacerValue,
											verticalPixelSize;
	
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	UInt32								count,
											index,
											numberControlPoints,
											numberDataBytes,
											numberGeoASCIIParameterBytes,
											numberGeoDoubleParameterBytes,
											numberGeoKeyDirectoryBytes,
											tiffValue;
	
	SInt16								errCode = noErr,
											numberGDALNoDataValueDigits;
	
	Boolean								d33550Flag = FALSE,
											d33922Flag = FALSE,
											d34264Flag = FALSE,
											d34735Flag = FALSE,
											d34736Flag = FALSE,
											d34737Flag = FALSE,
											d42113Flag = FALSE;
	
	
	*numberGeoEntriesPtr = 0;
	numberDataBytes = 0;
	numberGeoASCIIParameterBytes = 0;
	if (fileInfoPtr != NULL)
		{
				// Determine if any planar coordinate information exists to write
				// to a blw file. First, get a pointer to the map projection information 
				// if it exists.		
	
		mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																	fileInfoPtr->mapProjectionHandle,
																	kLock);
		
		if (mapProjectionInfoPtr != NULL)
			{	
			index = 0;	
			spacerValue = 0;
															
			if (mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize != 0 &&
					mapProjectionInfoPtr->planarCoordinate.verticalPixelSize != 0)
				{
				if (mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle == 0)
					{
					if (!countOnlyFlag)
						{
								// 33550: Pixel Scale		
					
						LoadTiffEntry (&imageFileDirectory,
											33550,
											(SInt16)kTIFFDouble,
											3,
											directoryDataStart);	
											
						BlockMoveData (&imageFileDirectory, &geoTiffInfoBuffer[index], 12);
						index += 12;
						
						d33550Flag = TRUE;
						
						}	// end "if (!countOnlyFlag)"
						
					(*numberGeoEntriesPtr)++;
					numberDataBytes += 24;
					
					}	// end "if (...->planarCoordinate.mapOrientationAngle == 0)"
			
						// 33922: Geo Tie Points	
						
				numberControlPoints = GetNumberControlPoints (fileInfoPtr);
															
				if (mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle == 0 ||
							numberControlPoints > 0)
					{
					if (numberControlPoints == 0)
						numberControlPoints = 1;
					
					if (!countOnlyFlag)
						{	
						LoadTiffEntry (&imageFileDirectory,
											33922,
											(SInt16)kTIFFDouble,
											numberControlPoints * 6,
											directoryDataStart + numberDataBytes);	
						BlockMoveData (&imageFileDirectory, &geoTiffInfoBuffer[index], 12);
						index += 12;
						
						d33922Flag = TRUE;
						
						}	// end "if (!countOnlyFlag)"
						
					(*numberGeoEntriesPtr)++;
					numberDataBytes += numberControlPoints * 48;
					
					}	// end "if (...->planarCoordinate.mapOrientationAngle == 0) || ...
															
				else	// ...->planarCoordinate.mapOrientationAngle != 0 && ...
					{
					if (!countOnlyFlag)
						{	
						LoadTiffEntry (&imageFileDirectory,
											34264,
											(SInt16)kTIFFDouble,
											16,
											directoryDataStart + numberDataBytes);	
						BlockMoveData (&imageFileDirectory, &geoTiffInfoBuffer[index], 12);
						index += 12;
						
						d34264Flag = TRUE;
						
						}	// end "if (!countOnlyFlag)"
						
					(*numberGeoEntriesPtr)++;
					numberDataBytes += 16 * 8;
					
					}	// end "else ...->planarCoordinate.mapOrientationAngle != 0 && ..."
				
				}	// end "if ((...->planarCoordinate.horizontalPixelSize != 0 && ..."
															
			if (mapProjectionInfoPtr->gridCoordinate.projectionCode > 0 || 
																				*numberGeoEntriesPtr > 0)
				{
						// 34735: Geo Key Directory
				
				numberGeoKeyDirectoryBytes = LoadGeoKeyDirectory (
																		fileInfoPtr,
																		mapProjectionInfoPtr,
																		NULL,
																		NULL,
																		&numberGeoDoubleParameterBytes,
																		NULL,
																		&numberGeoASCIIParameterBytes);
				
				if (numberGeoKeyDirectoryBytes > 0)
					{			
					if (!countOnlyFlag)
						{
						LoadTiffEntry (&imageFileDirectory,
											34735,
											(SInt16)kTIFFShort,
											numberGeoKeyDirectoryBytes/2,
											directoryDataStart + numberDataBytes);	
						BlockMoveData (&imageFileDirectory, &geoTiffInfoBuffer[index], 12);
						index += 12;
						
						d34735Flag = TRUE;
						
						}	// end "if (!countOnlyFlag)"
					
					(*numberGeoEntriesPtr)++;
					numberDataBytes += numberGeoKeyDirectoryBytes;
						
					}	// end "if (numberGeoKeyDirectoryBytes > 0)"
																			
						// 34736: Geo Double Parameters Directory
				
				if (numberGeoDoubleParameterBytes > 0)
					{			
					if (!countOnlyFlag)
						{
						LoadTiffEntry (&imageFileDirectory,
											34736,
											(SInt16)kTIFFDouble,
											numberGeoDoubleParameterBytes/8,
											directoryDataStart + numberDataBytes);	
						BlockMoveData (&imageFileDirectory, &geoTiffInfoBuffer[index], 12);
						index += 12;
						
						d34736Flag = TRUE;
						
						}	// end "if (!countOnlyFlag)"
					
					(*numberGeoEntriesPtr)++;
					numberDataBytes += numberGeoDoubleParameterBytes;
						
					}	// end "if (numberGeoDoubleParametersBytes > 0)"
				
				}	// end "if (mapProjectionInfoPtr->gridCoordinate.code != 0 || ..."
			
					// 34737: Geo ASCII Parameters		
				
			if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode != 0)
				{	
				if (!countOnlyFlag)
					{
					LoadTiffEntry (&imageFileDirectory,
										34737,
										(SInt16)kTIFFASCII,
										numberGeoASCIIParameterBytes,
										directoryDataStart + numberDataBytes);	
					BlockMoveData (&imageFileDirectory, &geoTiffInfoBuffer[index], 12);
					index += 12;
					
					d34737Flag = TRUE;
					
					}	// end "if (!countOnlyFlag)"
						
				(*numberGeoEntriesPtr)++;
				numberDataBytes += numberGeoASCIIParameterBytes;
				
				}	// end "if (...->gridCoordinate.referenceSystemCode != 0)"
			
					// 42113: GDAL NODATA Tag		
				
			if (fileInfoPtr->noDataValueFlag)
				{	
				numberGDALNoDataValueDigits = sprintf ((char*)gTextString,
																		"%g", 
																		fileInfoPtr->noDataValue);
																						
				if (!countOnlyFlag)
					{
					if (numberGDALNoDataValueDigits > 4)
						tiffValue = directoryDataStart + numberDataBytes;
					else	// numberGDALNoDataValueDigits <= 4
						{
								// Load the string directly into tiffValue.
								
						BlockMoveData (
								(char*)gTextString, &tiffValue, numberGDALNoDataValueDigits);
						
						}	// end "else numberGDALNoDataValueDigits <= 4"

					LoadTiffEntry (&imageFileDirectory,
										42113,
										(SInt16)kTIFFASCII,
										numberGDALNoDataValueDigits,
										tiffValue);	
					BlockMoveData (&imageFileDirectory, &geoTiffInfoBuffer[index], 12);
					index += 12;
					
					d42113Flag = TRUE;
					
					}	// end "if (!countOnlyFlag)"
						
				(*numberGeoEntriesPtr)++;
				if (numberGDALNoDataValueDigits > 4)
					numberDataBytes += numberGDALNoDataValueDigits;
				
				}	// end "if (fileInfoPtr->noDataValueFlag)"
												
		
					// Write the file geo file directory information to the file.
					
			if (!countOnlyFlag)
				{  
				errCode = MSetMarker (fileStreamPtr, 
												fsFromStart, 
												directoryStart, 
												kErrorMessages);
	
				count = index;
				if (errCode == noErr)
					errCode = MWriteData (fileStreamPtr, 
													&count, 
													geoTiffInfoBuffer, 
													kErrorMessages);
				
						// Now write the data information that goes with the directory tags.
				
				index = 0;	
						
				if (d33550Flag)
					{
					horizontalPixelSize = columnInterval * 
								mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize;
					index = LoadGeoDoubleValue (geoTiffInfoBuffer,
															index,
															horizontalPixelSize);
					
					verticalPixelSize = lineInterval * 
								mapProjectionInfoPtr->planarCoordinate.verticalPixelSize;
					index = LoadGeoDoubleValue (geoTiffInfoBuffer,
															index,
															verticalPixelSize);
					
					index = LoadGeoDoubleValue (geoTiffInfoBuffer,
															index,
															spacerValue);
					
					}	// end "if (d33550Flag)"
						
				if (d33922Flag)
					{
					index = LoadGeoModelTiePoints (fileInfoPtr,
																numberControlPoints,
																geoTiffInfoBuffer,
																mapProjectionInfoPtr,
																index,
																startMapColumn,
																startMapLine);
																
					}	// end "if (d33922Flag)"
						
				if (d34264Flag)
					{
					index = LoadGeoModelTransformationParameters (
																fileInfoPtr,
																geoTiffInfoBuffer,
																mapProjectionInfoPtr,
																index);
																
					}	// end "if (d34264Flag)"
						
				if (d34735Flag)
					{
					LoadGeoKeyDirectory (
										fileInfoPtr,
										mapProjectionInfoPtr,
										&geoTiffInfoBuffer[index],
										&geoTiffInfoBuffer[index+numberGeoKeyDirectoryBytes],
										NULL,
										&geoTiffInfoBuffer[index+numberGeoKeyDirectoryBytes+
																		numberGeoDoubleParameterBytes],
										NULL);
					index += numberGeoKeyDirectoryBytes +  
												numberGeoDoubleParameterBytes + 
																		numberGeoASCIIParameterBytes;
					
					}	// end "if (d34735Flag)"
						
				//if (d34737Flag)
				//	sprintf ((char*)&geoTiffInfoBuffer[index], 
				//				"UTM Zone 16N NAD27\|Clarke, 1866 by Default|");
						
				if (d42113Flag)
					BlockMoveData ((char*)gTextString, 
										(char*)&geoTiffInfoBuffer[index], 
										numberGDALNoDataValueDigits);
						
				if (errCode == noErr)
					errCode = MSetMarker (fileStreamPtr, 
													fsFromStart, 
													directoryDataStart, 
													kErrorMessages);
	
				count = numberDataBytes;
				if (errCode == noErr)
					errCode = MWriteData (fileStreamPtr, 
													&count,
													geoTiffInfoBuffer, 
													kErrorMessages);
				
				}	// end "if (!countOnlyFlag)"
					
			}	// end "if (mapProjectionInfoPtr != NULL)"
		
		}	// end "if (fileInfoPtr != NULL)"
		
	*numberDataBytesPtr = numberDataBytes;
		
	return (errCode);
	
}	// end "WriteGeoTIFFInformation"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//							  (c) Purdue Research Foundation
//
//	Function name:		Boolean WriteKMLFile
//
//	Software purpose:	This routine writes an ERDAS formatted header 
//							record to an image file
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			WriteNewErdasHeader in SFileIO.cpp
//							ChangeErdasHeader in SReformat.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/13/2007
//	Revised By:			Larry L. Biehl			Date: 12/07/2017

Boolean	WriteKMLFile (
				FileInfoPtr							fileInfoPtr, 
				CMFileStream*						fileStreamPtr,
				UInt32								columnInterval,
				UInt32								lineInterval,
				UInt32								startMapColumn,
				UInt32								startMapLine,
				UInt32								endMapColumn,
				UInt32								endMapLine)

{
	FileStringName255					savedImageFileName;
	         
	DoublePoint							centerCoordinate,
											coordinate;
											
	LongPoint							lineColumn;
	
	double								bottomMapEdge,
											eastEdgeLongitude,
											heading,
											horizontalPixelSize,
											leftMapEdge,
											maxMapSize,
											northEdgeLatitude,
											range,
											rightMapEdge,
											southEdgeLatitude,
											tilt,
											topMapEdge,
											verticalPixelSize,
											westEdgeLongitude;
	
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	FileStringPtr						filePathPtr;
	
	SInt16								errCode,
											vRefNum;
	
	Boolean								continueFlag;
	
	
	continueFlag = TRUE;	
	errCode = noErr;
	
			// Get some parameters needed for the kml file.
			
	mapProjectionInfoPtr = (MapProjectionInfoPtr)
											GetHandlePointer (fileInfoPtr->mapProjectionHandle,
																	kLock);		
															
			// Get the latitude and longitude of the center of the image.
															
	lineColumn.h = (startMapColumn + endMapColumn)/2;
	lineColumn.v = (startMapLine + endMapLine)/2;

	ConvertLCPointToMapPoint (mapProjectionInfoPtr, 
										&lineColumn, 
										&centerCoordinate);
										
	continueFlag = ConvertMapPointToLatLongPoint (
										mapProjectionInfoPtr, 
										&centerCoordinate);
										
			// Get the boundary latitude & longitude of the image.
			// 	First get the effective pixel size
			
	horizontalPixelSize = columnInterval * 
								mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize;
					
	verticalPixelSize = lineInterval * 
								mapProjectionInfoPtr->planarCoordinate.verticalPixelSize;
								
			// Convert the effective pixel size to meters if needed.
			
	ConvertFeetToMeters (mapProjectionInfoPtr->planarCoordinate.mapUnitsCode, 
										&horizontalPixelSize, 
										&verticalPixelSize);

			// Assuming one is to use the top, right,bottom and left edge latitudes.								
			// North edge
	
	lineColumn.h = (startMapColumn + endMapColumn)/2;
	lineColumn.v = startMapLine;

	ConvertLCPointToMapPoint (mapProjectionInfoPtr, 
										&lineColumn, 
										&coordinate);
										
	coordinate.v += verticalPixelSize/2;
	topMapEdge = coordinate.v;
	
	if (continueFlag)
		continueFlag = ConvertMapPointToLatLongPoint (mapProjectionInfoPtr, 
																		&coordinate);
										
	northEdgeLatitude = coordinate.v;
	
			// East edge
	
	lineColumn.h = endMapColumn;
	lineColumn.v = (startMapLine + endMapLine)/2;

	ConvertLCPointToMapPoint (mapProjectionInfoPtr, 
										&lineColumn, 
										&coordinate);
										
	coordinate.h += horizontalPixelSize/2;
	rightMapEdge = coordinate.h;
			
	if (continueFlag)
		continueFlag = ConvertMapPointToLatLongPoint (mapProjectionInfoPtr,
																		&coordinate);
										
	eastEdgeLongitude = coordinate.h;
	
			// South edge
	
	lineColumn.h = (startMapColumn + endMapColumn)/2;
	lineColumn.v = endMapLine;

	ConvertLCPointToMapPoint (mapProjectionInfoPtr, 
										&lineColumn, 
										&coordinate);
										
	coordinate.v -= verticalPixelSize/2;
	bottomMapEdge = coordinate.v;
			
	if (continueFlag)
		continueFlag = ConvertMapPointToLatLongPoint (mapProjectionInfoPtr,
																		&coordinate);
										
	southEdgeLatitude = coordinate.v;
	
			// West edge
	
	lineColumn.h = startMapColumn;
	lineColumn.v = (startMapLine + endMapLine)/2;

	ConvertLCPointToMapPoint (mapProjectionInfoPtr, 
										&lineColumn, 
										&coordinate);
										
	coordinate.h -= horizontalPixelSize/2;
	leftMapEdge = coordinate.h;
		
	if (continueFlag)
		continueFlag = ConvertMapPointToLatLongPoint (mapProjectionInfoPtr, 
																		&coordinate);
										
	westEdgeLongitude = coordinate.h;		
	/*			
			// Assuming one is to use the top-left and bottom-right corner lat-longs.	
			//	Northwest edge
	
	lineColumn.h = startMapColumn;
	lineColumn.v = startMapLine;

	ConvertLCPointToMapPoint (mapProjectionInfoPtr, 
										&lineColumn, 
										&coordinate);
										
	coordinate.v += verticalPixelSize/2;
	coordinate.h -= horizontalPixelSize/2;
	
	leftMapEdge = coordinate.h;
	topMapEdge = coordinate.v;
										
	continueFlag = ConvertMapPointToLatLongPoint (
										mapProjectionInfoPtr,
										&coordinate);
										
	westEdgeLongitude = coordinate.h;
	northEdgeLatitude = coordinate.v;
	
			// 	Bottom-right corner
	
	lineColumn.h = endMapColumn;
	lineColumn.v = endMapLine;

	ConvertLCPointToMapPoint (mapProjectionInfoPtr, 
										&lineColumn, 
										&coordinate);
										
	coordinate.h += horizontalPixelSize/2;
	coordinate.v -= verticalPixelSize/2;
	
	rightMapEdge = coordinate.h;
	bottomMapEdge = coordinate.v;
										
	continueFlag = ConvertMapPointToLatLongPoint (
										mapProjectionInfoPtr,
										&coordinate);
										
	eastEdgeLongitude = coordinate.h;
	southEdgeLatitude = coordinate.v;	
	*/

	if (continueFlag)
		{
				// All is okay. Create the KML file.
				
		GetCopyOfPFileNameFromFileStream (fileStreamPtr, savedImageFileName);
		
				// Close the tiff file.	
				
		vRefNum = GetVolumeReferenceNumber (fileStreamPtr);
		CloseFile (fileStreamPtr);
		
				// Get the default kml file name.

		filePathPtr = (FileStringPtr)GetFilePathPPointerFromFileStream (fileStreamPtr);
		RemoveSuffix (filePathPtr);
		ConcatFilenameSuffix (filePathPtr, (StringPtr)"\0.kml\0");

				// Now get wide character and unicode names.

		SetFileDoesNotExist (fileStreamPtr, kKeepUTF8CharName);
		UpdateFileNameInformation (fileStreamPtr, NULL);	
		
				// Now open the kml file.
				
		SetType (fileStreamPtr, kTEXTFileType);

		errCode = CreateNewFile (fileStreamPtr, 
											vRefNum, 
											-1, 
											kErrorMessages,
											kReplaceFlag);
						
					// Set position to start writing data at the start of the file.
			
		continueFlag = (errCode == noErr) && FileExists (fileStreamPtr);
		
		if (continueFlag)
			errCode = MSetMarker (fileStreamPtr, fsFromStart, 0, kErrorMessages);

		if (errCode != noErr)
			continueFlag = FALSE;
			
		if (continueFlag)
			{
					// Write xml version line
					
			continueFlag = WriteSpecifiedStringNumber (fileStreamPtr, 
																		kKMLStrID, 
																		IDS_KML1, 
																		(char*)gTextString2,
																		continueFlag);
			
					// Write kml version line
					
			continueFlag = WriteSpecifiedStringNumber (fileStreamPtr, 
																		kKMLStrID, 
																		IDS_KML2, 
																		(char*)gTextString2,
																		continueFlag);
			
					// Write Document line
					
			continueFlag = WriteSpecifiedStringNumber (fileStreamPtr, 
																		kKMLStrID, 
																		IDS_KML3, 
																		(char*)gTextString2,
																		continueFlag);
			
					// Write LookAt line
					
			range = 2000000;
			
			maxMapSize = MIN (fabs (rightMapEdge-leftMapEdge), 
									fabs (topMapEdge-bottomMapEdge));
			//if (mapProjectionInfoPtr->planarCoordinate.mapUnitsCode == kMetersCode)
				range = 8 * maxMapSize;
			
			//else if (mapProjectionInfoPtr->planarCoordinate.mapUnitsCode == kFeetCode)
			//	range = 24 * maxMapSize;
			
			range = MIN (2000000, range);
			
			tilt = 0;
			heading = 0;
			continueFlag = WriteSpecifiedStringNumber (fileStreamPtr, 
																		kKMLStrID, 
																		IDS_KML4, 
																		(char*)gTextString,
																		(char*)gTextString2,
																		centerCoordinate.h,
																		centerCoordinate.v,
																		range,
																		tilt,
																		heading,
																		continueFlag);
			
					// Write GroundOverlay line
					
			continueFlag = WriteSpecifiedStringNumber (fileStreamPtr, 
																		kKMLStrID, 
																		IDS_KML5, 
																		(char*)gTextString2,
																		continueFlag);
			
					// Write name line
					
			continueFlag = WriteSpecifiedStringNumber (fileStreamPtr, 
																		kKMLStrID, 
																		IDS_KML6, 
																		(char*)gTextString,
																		(char*)gTextString2,
																		(char*)&savedImageFileName[1],
																		continueFlag);
			
					// Write Icon line
					
			continueFlag = WriteSpecifiedStringNumber (fileStreamPtr, 
																		kKMLStrID, 
																		IDS_KML7, 
																		(char*)gTextString,
																		(char*)gTextString2,
																		(char*)&savedImageFileName[1],
																		continueFlag);
			
					// Write LatLonBox line
					
			continueFlag = WriteSpecifiedStringNumber (fileStreamPtr, 
																		kKMLStrID, 
																		IDS_KML8, 
																		(char*)gTextString,
																		(char*)gTextString2,
																		northEdgeLatitude,
																		eastEdgeLongitude,
																		southEdgeLatitude,
																		westEdgeLongitude,
																		0,
																		continueFlag);
			
					// Write /GroundOverlay line
					
			continueFlag = WriteSpecifiedStringNumber (fileStreamPtr, 
																		kKMLStrID, 
																		IDS_KML9, 
																		(char*)gTextString2,
																		continueFlag);
			
					// Write /Document line
					
			continueFlag = WriteSpecifiedStringNumber (fileStreamPtr, 
																		kKMLStrID, 
																		IDS_KML10, 
																		(char*)gTextString2,
																		continueFlag);
			
					// Write /kml line
					
			continueFlag = WriteSpecifiedStringNumber (fileStreamPtr, 
																		kKMLStrID, 
																		IDS_KML11, 
																		(char*)gTextString2,
																		continueFlag);
																	
			}	// end "if (continueFlag)"
																
		}	// end "if (continueFlag)"
	
	return (continueFlag);
	
}	// end "WriteKMLFile" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//							  (c) Purdue Research Foundation
//
//	Function name:		Boolean WriteNewErdasHeader
//
//	Software purpose:	This routine initializes a new Erdas header record and then 
//							calls the routine to fill in the parameters and write the header 
//							record to the file.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			CreateBackgroundImageFile in fieldsToThematicFile.c
//							CreateResultsDiskFile in multiSpecUtilities.c
//							InsertNewErdasHeader in reformat.c
//
//	Coded By:			Larry L. Biehl			Date: 08/29/1988
//	Revised By:			Larry L. Biehl			Date: 11/16/1991

Boolean	WriteNewErdasHeader (
				FileInfoPtr							fileInfoPtr, 
				SInt16*								headerRecordPtr, 
				SInt16								ERDASVersion)

{
	SInt16								n;
	Boolean								continueFlag;
	
	
			// Initialize the header array to all zeros.									
					
	for (n=0; n<=63; n++)
		headerRecordPtr[n] = 0;
	
			// Fill the rest of the header and write it to the file.					
			
	continueFlag = WriteErdasHeader (
										fileInfoPtr, (UInt8*)headerRecordPtr, ERDASVersion);
			
	return (continueFlag);
		
}	// end "WriteNewErdasHeader" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean WriteNewImageHeader
//
//	Software purpose:	This routine writes a new image header based on the
//							requested format.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:			CovertMultispectralToThematic in fieldsToThematic.c
//							MosaicImagesSideBySide in mosaic.c
//							RectifyImage in rectification.c
//							ChangeFormatToBILorBIS in reformat.c
//							ChangeFormatToBSQ in reformat.c	
//
//	Coded By:			Larry L. Biehl			Date: 10/08/1992
//	Revised By:			Larry L. Biehl			Date: 08/20/2010

Boolean WriteNewImageHeader (
				FileInfoPtr 						inFileInfoPtr, 
				FileInfoPtr 						outFileInfoPtr, 
				HPtr									headerRecordPtr,
				DisplaySpecsPtr					displaySpecsPtr,
				SInt16								tiffSourceCode,
				SInt16								outputFileCode, 
				SInt32								lineInterval, 
				SInt32								columnInterval,
				SInt16								thematicListType,
				SInt16								paletteSelection)

{
	SInt64								writePosOff64;
	SInt32								writePosOff;
	
	SInt16								errCode,
	numberChannels,
	numberClasses;
	
	Boolean								continueFlag;
	
	
	continueFlag = TRUE;
	if (outFileInfoPtr->format)
		{
		CMFileStream* outFileStreamPtr = GetFileStreamPointer (outFileInfoPtr);
		
		errCode = noErr;
		
				// Set writePosOff64 to indicate the location to begin writing		
					// the image data to.															
		
		writePosOff64 = -1;
		if (outputFileCode != kNewFileMenuItem)
			errCode = MGetMarker (outFileStreamPtr, 
										 &writePosOff64,
										 kErrorMessages);
		
		if (errCode == noErr)
			{
			switch (outFileInfoPtr->format)
				{
				case kArcViewType:
					continueFlag = WriteArcViewSupportFiles (outFileInfoPtr);
					break;
					
					case kErdas74Type:
					continueFlag = WriteNewErdasHeader (outFileInfoPtr, 
																	(SInt16*)headerRecordPtr, 
																	outFileInfoPtr->format);
					break;
					
				case kVICARType:
					break;
					
				case kGAIAType:		
					gSwapBytesFlag = gBigEndianFlag;
					
					if (gProcessorCode != kMultispecToThematicProcessor &&
						 inFileInfoPtr != NULL &&
						 (inFileInfoPtr->format == kGAIAType ||
						  inFileInfoPtr->format == kGAIA2Type))
						{	
							continueFlag = CopyToNewGAIAHeader (inFileInfoPtr, 
																			outFileInfoPtr, 
																			headerRecordPtr, 
																			lineInterval, 
																			columnInterval);
							
						}	// end "if (... != kMultispecToThematicProcessor ..." 
					
					else	// ...!= kMultispecToThematicProcessor || ...
						{
						InitializeAreaDescription (&gAreaDescription, 
															1, 
															outFileInfoPtr->numberLines, 
															1, 
															outFileInfoPtr->numberColumns, 
															1, 
															1,
															1,
															1,
															0);
						
						numberChannels = 0;
						numberClasses = 0;
						if (gProjectInfoPtr)
							{
							numberChannels = gProjectInfoPtr->numberStatisticsChannels;
							numberClasses = gProjectInfoPtr->numberStatisticsClasses;
							
							}	// end "if (gProjectInfoPtr)"
						
						continueFlag = WriteGAIAHeader (outFileInfoPtr, 
																  NULL, 
																  numberChannels, 
																  NULL,
																  numberClasses,
																  NULL);
						
						}	// end "else ...!= kMultispecToThematicProcessor..."
					
					gSwapBytesFlag = FALSE; 
					break;
					
				case kMatlabType:
							// Michael Gansler 
					WriteMatlabHeader (outFileInfoPtr);
					break;
					
				case kTIFFType:
				case kGeoTIFFType:
							//					writePosOff = (SInt32)writePosOff64;
					writePosOff = 0;
							// Note that when one appends or modifies TIFF/GeoTIFF files,
							// there is the real possibility of messing up the original
							// file if the header was not original written by MultiSpec.  
							// It is pretty easy to use tags in different ways between 
							// applications.
					
					continueFlag = WriteTIFFImageFile (outFileInfoPtr,
																  outFileStreamPtr, 
																  displaySpecsPtr,
																  NULL,
																  tiffSourceCode,	// kFromReformat,
																  thematicListType,
																  paletteSelection,
																  &writePosOff,
																  1,
																  1);
					
					if (continueFlag)
						{												
						outFileInfoPtr->numberHeaderBytes = writePosOff;
						if (outputFileCode == kNewFileMenuItem)
							writePosOff64 = writePosOff;
							
						}	// end "if (continueFlag)"
					break;
					
				}	// end "switch (outFileInfoPtr->format)" 
			
			if (continueFlag && writePosOff64 > 0)
				errCode = MSetMarker (outFileStreamPtr, 
											 fsFromStart, 
											 writePosOff64,
											 kErrorMessages);
			
			}	// end "if (errCode == noErr)" 
		
		if (errCode != noErr)
			continueFlag = FALSE;
		
		}	// end "if (outFileInfoPtr->format)" 
	
	return (continueFlag);
	
}	// end "WriteNewImageHeader" 



#if defined multispec_mac
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean WriteTextOutputFile
//
//	Software purpose:	The purpose of this routine is to write the text
//							in the text output window to a disk file.
//
//							NOTE.  The outline for this routine came from
//							THINK's CAPPS editor construction kit.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			?							Date: ??/??/??
//	Revised By:			Larry L. Biehl			Date: 12/29/2008

Boolean WriteTextOutputFile (
				SInt16     							refNum, 
				Str255*								fileNamePtr,
				#if use_mlte_for_text_window
					TXNObject							textObject,
				#endif
				#if !use_mlte_for_text_window
					WEReference							textObject,
				#endif
				SInt32   							beginText, 
				SInt32   							endText)

{
	char*									textPtr;
	
	Handle								hText = NULL;
	
	SInt32								count,
											index;
	
	OSErr 								IOResult;
	

			// Try to make the file an empty one. 											
	
	IOResult = SetEOF (refNum, 0L);		
	if (IOResult != noErr)
		{
		IOCheck (IOResult, (CharPtr)fileNamePtr);
		return (FALSE);
		
		}	// end "if (IOResult != noErr)" 
		
			// Get Handle to the actual text
			
	#if !TARGET_RT_MAC_MACHO	
		hText = WEGetText (textObject);
		index = beginText;
	#else	// TARGET_RT_MAC_MACHO
		#if use_mlte_for_text_window
			IOResult = TXNGetDataEncoded (
										textObject, beginText, endText, &hText, kTXNTextData);
		#endif	
		
		#if !use_mlte_for_text_window	
			hText = MNewHandle (2*(endText-beginText+1));
			if (hText != NULL)  
				WEStreamRange (beginText,
									endText,
									kTypeText,
									weStreamDestinationKindMask,	// ??
									hText,
									textObject);
		#endif	// !use_mlte_for_text_window
		
		index = 0;
	#endif	// TARGET_RT_MAC_MACHO
	
	textPtr = (char*)GetHandlePointer (hText, kLock);
	
	if (IOResult == noErr && textPtr != NULL)
		{			
				// Write out the text.	
				
		count = endText - beginText;
		IOResult = FSWrite (refNum, &count, &textPtr[index]);
		
		}	// end "if (textPtr != NULL)" 
		
	#if !TARGET_RT_MAC_MACHO
		CheckAndUnlockHandle (hText);
	#else
		UnlockAndDispose (hText);	
	#endif
			
	if (IOResult != noErr)
		{
		IOCheck (IOResult, (CharPtr)fileNamePtr);
		return (FALSE);
		
		}	// end "if (IOResult != noErr)" 

	return (TRUE);
	
}	// end "WriteTextOutputFile" 
#endif	// defined multispec_mac


                   
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//							  (c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void WriteThematicClassesAs
//
//	Software purpose:	The purpose of this routine is to write the thematic
//							classes and color palette to the requested trailer file. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/08/1996
//	Revised By:			Larry L. Biehl			Date: 02/07/2018

void WriteThematicClassesAs (
				Handle								windowInfoHandle,
				FileInfoPtr							fileInfoPtr)

{                  
	ColorSpec*							colorSpecPtr = NULL;
	
	CTabHandle							cTableHandle;
	CTabPtr								colorTablePtr;
	Handle								displaySpecsH;
	UInt32								supportFileType;
	SInt16								stringIndex;
	
	Boolean								continueFlag;
	SignedByte							handleStatus;
	
	
	cTableHandle = NULL;
			
	displaySpecsH = GetDisplaySpecsHandle (windowInfoHandle);
	DisplaySpecsPtr displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
																							displaySpecsH);
	cTableHandle = displaySpecsPtr->savedClassCTableHandle;
	gPaletteOffset = displaySpecsPtr->paletteOffset;
	
	if (cTableHandle != NULL)
		{
		colorTablePtr = (CTabPtr)GetHandlePointer ((Handle)cTableHandle, kLock);
		colorSpecPtr = colorTablePtr->ctTable;
		
		}	// end "if (cTableHandle != NULL)"
		
	if (fileInfoPtr->format == kErdas74Type)
		supportFileType = kITRLFileType;
		
	else	// fileInfoPtr->format != kErdas74Type
		supportFileType = kICLRFileType;
		
	continueFlag = CreateThematicSupportFile (fileInfoPtr, 
															NULL,
															(UInt16)fileInfoPtr->numberClasses, 
															NULL,
															0,  
															colorSpecPtr,
															NULL,
															NULL, 
															0,
															(UInt16)fileInfoPtr->numberClasses, 
															kFromDescriptionCode, 
															kPaletteHistogramClassNames,
															kClassDisplay,
															kDoNotCollapseClass,
															supportFileType);
												
	CheckAndUnlockHandle ((Handle)cTableHandle);
		
	if (continueFlag)
		{
		gUpdateFileMenuItemsFlag = TRUE;
		
		fileInfoPtr->classChangedFlag = FALSE;		
		stringIndex = IDS_FileIO101;
		
		if (supportFileType == kICLRFileType && fileInfoPtr->numberGroups > 0)
			{
			fileInfoPtr->groupChangedFlag = FALSE;
			stringIndex = IDS_FileIO193;
			
			}	// end "if (supportFileType == kICLRFileType && ...)"
			
		if (!fileInfoPtr->classChangedFlag && !fileInfoPtr->groupChangedFlag)
			{
			#if defined multispec_lin
						// Make sure that wxWidgets knows the document has not changed
						// since the last save.
				gActiveImageViewCPtr->GetDocument ()->Modify (FALSE);
			#endif
			}	// end "if (!fileInfoPtr->classChangedFlag && ..."


		ForceTextToEnd (); 
			
				// Get pointer to the file file name and list a message in the
				// output text window that class information was saved okay.
		
		FileStringPtr fileNamePtr = 
								(FileStringPtr)GetFileNameCPointerFromFileInfo (fileInfoPtr);

		CMFileStream* outputFileStreamPtr = GetSupportFileStreamPointer (
																						windowInfoHandle,
																						&handleStatus);
		char* outfileNamePtr = 
							(char*)GetFileNameCPointerFromFileStream (outputFileStreamPtr);
								
		ListSpecifiedStringNumber (kFileIOStrID, 
											stringIndex,
											NULL, 
											gOutputForce1Code, 
											(char*)fileNamePtr,
											(char*)outfileNamePtr, 
											TRUE,
											kUTF8CharString);
						
		UnlockSupportFileStream (windowInfoHandle, handleStatus);
		
		}	// end "if (continueFlag)"

}	// end "WriteThematicClassesAs" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//							  (c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void WriteThematicGroups
//
//	Software purpose:	The purpose of this routine is to write the thematic
//							groups to the end of an image file. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			WriteThematicInfo	in SSaveWrt.cpp
//							SaveIfWindowChanged in SWindow.cpp
//
//	Coded By:			Larry L. Biehl			Date: 09/09/1992
//	Revised By:			Larry L. Biehl			Date: 01/07/2019

void WriteThematicGroups (
				Handle								windowInfoHandle,
				FileInfoPtr							fileInfoPtr)

{     
	SInt64								sizeImageFile;
	             
	CMFileStream*						fileStreamPtr;
	ColorSpec*							colorSpecPtr;
	CTabHandle							cTableHandle;
	CTabPtr								colorTablePtr;
	
	char									*groupNamePtr,
											*stringPtr;
										
	SInt16								*classToGroupPtr,
											*groupToPalettePtr;
											
	Handle								displaySpecsH;
											
	SInt32								numberBytes;
	
	UInt32								classIndex,
											count,
											group;
	
	SInt16								errCode;
	
	Boolean								continueFlag;
	
	
	continueFlag = TRUE;
	cTableHandle = NULL;
		
	if (fileInfoPtr->groupTablesHandle == NULL && 
										fileInfoPtr->groupNameHandle == NULL)
																									return;
	
			// Do not allow group information to be written to image files
			// with bandinterleave formats of KBNonSQ, kBNonSQBlock and kBIBlock
			// since we do not know where the end of the file is with the parameters.
			// This includes the Imagine formatted files.
									
	if (fileInfoPtr->bandInterleave > kBIS)
																									return;
																								
			// Do not allow group information to be written to image files
			// that are in Imagine, TIFF, GeoTIFF, HDF and netCDF formats, since we do 
			// not know for sure where the end of the file is based on the parameters 
			// available. Also do not allow to be written if this is a gdal handled file.
									
	if (fileInfoPtr->gdalDataSetH != NULL ||
				fileInfoPtr->format == kTIFFType || 
						fileInfoPtr->format == kImagineType ||
								fileInfoPtr->format == kHDF4Type ||
										fileInfoPtr->format == kNETCDFType ||
												fileInfoPtr->format == kGeoTIFFType ||
														fileInfoPtr->format == kHDF4Type2)
		{
		DisplayAlert (kErrorAlertID, 
							kStopAlert, 
							kAlertStrID, 
							IDS_Alert126,
							0, 
							NULL);
																									return;
																								
		}	// end "if (fileInfoPtr->format == kTIFFType || ..."
		
			// Get temporary space for creating the group description to		
			// be appended to the file.													

	numberBytes = 5 * fileInfoPtr->numberClasses;
	numberBytes = MAX (numberBytes, 100);
	gCharBufferPtr1 = (char*)MNewPointer (numberBytes);
	continueFlag = (gCharBufferPtr1 != NULL);
		
	if (continueFlag)
		{
		groupNamePtr = (char*)GetHandlePointer (fileInfoPtr->groupNameHandle);
																				
				// List has been set up before; load the last list in.				
				
		classToGroupPtr = GetClassToGroupPointer (fileInfoPtr);
		
		fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
		
				// Get the size of the image without any group descriptions.		
		
		sizeImageFile = GetSizeOfImage (fileInfoPtr);
		
		errCode = SetFileWriteEnabled (fileStreamPtr);
	
				// Position the marker in the image file to end of image data, 	
				// just before the postion for the group descriptions.				
	
		if (errCode == noErr)
			{
			errCode = MSetMarker (
								fileStreamPtr, fsFromStart, sizeImageFile, kErrorMessages);
			
			}	// end "if (errCode == noErr)" 
			
		continueFlag = (errCode == noErr);
		
		}	// end "if (continueFlag)" 
	
	if (continueFlag)
		{
				// Write group description identifier to the image file.				

		MGetString (gTextString, kFileIOStrID, IDS_GroupDescriptions);
	
		count = gTextString[0];
		errCode = MWriteData (
							fileStreamPtr, &count, (char*)&gTextString[1], kErrorMessages);
			
		continueFlag = (errCode == noErr);
		
		}	// end "if (continueFlag)" 
			
	if (continueFlag)
		{
					// Loop through the groups and write the group names and 		
					// colors to the end of the file.										
						
		displaySpecsH = GetDisplaySpecsHandle (windowInfoHandle);
		DisplaySpecsPtr displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
																						displaySpecsH);
		cTableHandle = displaySpecsPtr->savedGroupCTableHandle;
		gPaletteOffset = displaySpecsPtr->paletteOffset;
		if (cTableHandle != NULL)
			{                             
			colorTablePtr = (CTabPtr)GetHandlePointer (
													(Handle)cTableHandle, kLock);
			colorSpecPtr = colorTablePtr->ctTable;
			
			}	// end "if (cTableHandle != NULL)" 
			
		else	// !cTableHandle 
			continueFlag = FALSE;
			
		}	// end "if (continueFlag)" 
		
	if (continueFlag)
		{
				// Write group description summary to the image file.					

		sprintf (gCharBufferPtr1,
					" 1 1 9 Grouping1 %4d %1d", 
					fileInfoPtr->numberGroups,
					gPaletteOffset);
		count = 23;
		
		errCode = MWriteData (fileStreamPtr, &count, gCharBufferPtr1, kErrorMessages);
			
		continueFlag = (errCode == noErr);
		
		}	// end "if (continueFlag)" 
			
	if (continueFlag)
		{
				// Write the class to group table.											
				
		stringPtr = gCharBufferPtr1;
		for (classIndex=0; classIndex<fileInfoPtr->numberClasses; classIndex++)
			{
					// Fill temporary string with class to group table.				
	
			sprintf (stringPtr,
						" %4d", 
						classToGroupPtr[classIndex] + 1);
			stringPtr += 5;
			
			}	// end "for (classIndex=0; ..." 
		
		count = fileInfoPtr->numberClasses * 5;
		errCode = MWriteData (fileStreamPtr, &count, gCharBufferPtr1, kErrorMessages);
		
		continueFlag = (errCode == noErr);
			
		}	// end "if (continueFlag)" 
			
	if (continueFlag)
		{
					// Loop through the groups and write the group names and 		
					// colors to the end of the file.										
					
		groupToPalettePtr = GetGroupToPalettePointer (fileInfoPtr);
		for (group=0; 
				group<fileInfoPtr->numberGroups;
				group++)
			{
			if (continueFlag)
				{
						// Fill temporary string with length of group name.			
		
				sprintf (gCharBufferPtr1,
							" %4d ", 
							groupNamePtr[0]);
				stringPtr = &gCharBufferPtr1[6];
				
						// Fill temporary string with group name.							
				
				BlockMoveData (&groupNamePtr[1], stringPtr, groupNamePtr[0]);
				stringPtr += groupNamePtr[0];
				
						// Fill temporary string with group color.						
				
				#if defined multispec_mac || defined multispec_win
					sprintf (
							stringPtr,
							" %4x %4x %4x ",
							colorSpecPtr[groupToPalettePtr[group]+gPaletteOffset].rgb.red,
							colorSpecPtr[groupToPalettePtr[group]+gPaletteOffset].rgb.green,
							colorSpecPtr[groupToPalettePtr[group]+gPaletteOffset].rgb.blue);
				#endif
				
				#if defined multispec_lin
							// Need to allow for just 8-bit color definition in ColorSpec vector
							// for wxWidgets version. The output format is expecting 16-bit
							// color with useful color informtation in the upper 8 bits.
					sprintf (
							stringPtr,
							" %4x %4x %4x ",
							colorSpecPtr[groupToPalettePtr[group]+gPaletteOffset].rgb.red << 8,
							colorSpecPtr[groupToPalettePtr[group]+gPaletteOffset].rgb.green << 8,
							colorSpecPtr[groupToPalettePtr[group]+gPaletteOffset].rgb.blue << 8);
				#endif
							
				count = 6 + groupNamePtr[0] + 15;
				if (group == (UInt32)(fileInfoPtr->numberGroups - 1))
					count++;
					
				errCode = MWriteData (fileStreamPtr, 
												&count, 
												gCharBufferPtr1, 
												kErrorMessages);
		
				continueFlag = (errCode == noErr);
				
				}	// end "if (continueFlag)" 
				
			if (!continueFlag)
				break;
		
			groupNamePtr += 32;
				
			}	// end "for (group=0; ..." 
			
		CheckAndUnlockHandle ((Handle)cTableHandle);
			
		}	// end "if (continueFlag)" 
		
	if (continueFlag)
		{
		gUpdateFileMenuItemsFlag = TRUE;
		fileInfoPtr->groupChangedFlag = FALSE;	
		
		ForceTextToEnd (); 
			
				// Get pointer to the file file name and list a message in the
				// output text window that group information was saved okay.
		
		char* fileNamePtr = (char*)GetFileNameCPointerFromFileStream (fileStreamPtr);
						
		ListSpecifiedStringNumber (kFileIOStrID,
											IDS_FileIO100,
											NULL, 
											gOutputForce1Code, 
											fileNamePtr,
											fileNamePtr,
											TRUE,
											kUTF8CharString);
			
		#if defined multispec_lin
					// Make sure that wxWidgets knows the document has not changed
					// since the last save.
			gActiveImageViewCPtr->GetDocument ()->Modify (FALSE);
		#endif
		
		}	// end "if (continueFlag)" 
	
	if (fileInfoPtr != NULL)
		{		
		CheckAndUnlockHandle (fileInfoPtr->groupNameHandle);
		CheckAndUnlockHandle (fileInfoPtr->groupTablesHandle);
										
				// Make certain that image file is now read only.					
			
		SetFileReadOnly (fileStreamPtr);
		
		}	// end "if (fileInfoPtr != NULL)" 
	
	gCharBufferPtr1 = CheckAndDisposePtr (gCharBufferPtr1);

}	// end "WriteThematicGroups" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//							  (c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void WriteThematicInfo
//
//	Software purpose:	The purpose of this routine is to write the thematic classes or 
//							group info to the requested trailer file. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			menus in menus.c
//							SaveIfWindowChanged in SWindow.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/11/1996
//	Revised By:			Larry L. Biehl			Date: 07/27/2011

void WriteThematicInfo (
				Handle								windowInfoHandle,
				SInt16								classGroupCode)

{	
	FileInfoPtr							fileInfoPtr = NULL;
	
	Handle								fileInfoHandle;

	SignedByte							handleStatus;
	
	
	if (windowInfoHandle == NULL)
		windowInfoHandle = GetActiveImageWindowInfoHandle ();
	
	if (windowInfoHandle == NULL || gCharBufferPtr1 != NULL)
																									return;
																					
			// Get the file pointer for the active image window.
	
	fileInfoHandle = GetFileInfoHandle (windowInfoHandle);
	fileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (fileInfoHandle, 
																			&handleStatus);
	
	if (fileInfoPtr != NULL)
		{	
		if (classGroupCode == 0)
			classGroupCode = GetDisplayClassGroupCode (windowInfoHandle);
		
		if (classGroupCode == kClassDisplay ||
					(fileInfoPtr->ancillaryInfoformat != kErdas74Type &&
										fileInfoPtr->ancillaryInfoformat != kErdas73Type))
			WriteThematicClassesAs (windowInfoHandle, fileInfoPtr);
			
		else	// classGroupCode >= kGroupDisplay && ... != kArcViewDefaultSupportType
			WriteThematicGroups (windowInfoHandle, fileInfoPtr);

		MHSetState (fileInfoHandle, handleStatus);
			
				// Scroll output window to the selection and adjust the scroll bar
				// in case text was written to the text window.															
		
		UpdateOutputWScrolls (gOutputWindow, 1, kDisplayNoMessage);
			
		}	// end "if (fileInfoPtr != NULL)"
	
	CheckAndUnlockHandle (fileInfoHandle);

}	// end "WriteThematicInfo" 


/*                   
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 WriteTIFFColorMap2
//
//	Software purpose:	This purpose of this routine is to save the input color
//							map to a TIFF formatted disk file.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/21/1994
//	Revised By:			Larry L. Biehl			Date: 04/26/2004

SInt16 WriteTIFFColorMap2 (
				FileInfoPtr							outFileInfoPtr, 
				CMFileStream* 						fileStreamPtr,
				UInt16								numberClasses,
				CMPaletteInfo						paletteObject,
				UInt16								paletteOffset)

{                                       
	ColorSpec*							colorSpecPtr;
	
	UInt32								colorIndex,
											count;
	
	SInt16								errCode = -1;
										
	UInt16								blueVectorPtr[256],
											greenVectorPtr[256],
											redVectorPtr[256]; 
	
	UInt16								classNumber,
											index, 
											paletteCode;
											
	Boolean								continueFlag; 
		                                  
	
	if (outFileInfoPtr != NULL)
		{
				// Get table for palette information. Allow enough for the
				// number of classes, white, black, and background. The
				// white, black and background will probably not be need for
				// the trailer file, but the routines being called will
				// fix those colors.
					
		colorSpecPtr = (ColorSpec*)MNewPointer (
												(SInt32)(numberClasses+3)*sizeof (ColorSpec));
		continueFlag = (colorSpecPtr != NULL);
			
				// Get the requested color scheme for the classes.
		
		if (continueFlag)
			continueFlag = LoadColorSpecTable (NULL, 
											NULL,
											NULL,
											colorSpecPtr,
											paletteObject,
											paletteOffset,
											MIN (numberClasses+3, 256),
											numberClasses,
											kPaletteHandle,
											numberClasses,
											kClassDisplay,
											&paletteCode);
		
				// Load the red, green and blue colors.			
		
		//paletteIndex = 0;
		colorIndex = 0;
		index = 0;			
		for (classNumber=1; classNumber<=numberClasses; classNumber++)
			{
			//if (paletteIndexPtr != NULL)
			//	index = paletteIndexPtr[paletteIndex];
							
			//else	// newPaletteIndexPtr == NULL
			//	index = paletteIndex + paletteOffset;
					
			redVectorPtr[colorIndex] = colorSpecPtr[index].rgb.red; 
			greenVectorPtr[colorIndex] = colorSpecPtr[index].rgb.green;
			blueVectorPtr[colorIndex] = colorSpecPtr[index].rgb.blue;
			colorIndex++;
			//paletteIndex++;
			index++;
			                            
			}	// end "for (index=0; index<numberEntries; index++)"  
			
				// Finish fill with black if needed.
				
		for (index=colorIndex; index<256; index++)
			{
			redVectorPtr[index] = 0;
			greenVectorPtr[index] = 0;
			blueVectorPtr[index] = 0;
			
			}	// end "for (index=colorIndex; index<256; index++)"
	
		CheckAndDisposePtr ((Ptr)colorSpecPtr);
			
		count = 512;
		errCode = 
				MWriteData (fileStreamPtr, &count, redVectorPtr, kErrorMessages);
			
		count = 512;
		if (errCode == noErr)
			errCode = 
				MWriteData (fileStreamPtr, &count, greenVectorPtr, kErrorMessages);
			
		count = 512;
		if (errCode == noErr)
			errCode = 
				MWriteData (fileStreamPtr, &count, blueVectorPtr, kErrorMessages);
		
		}	// end "if (outFileInfoPtr != NULL)" 
		
	return (errCode);
	
}	// end "WriteTIFFColorMap2" 
*/

                   
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 WriteTIFFColorMap2
//
//	Software purpose:	The purpose of this routine is to save the input color map to a 
//							TIFF formatted disk file.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/21/1994
//	Revised By:			Larry L. Biehl			Date: 04/27/2016
// 
SInt16 WriteTIFFColorMap2 (
				FileInfoPtr							fileInfoPtr,
				CMFileStream* 						fileStreamPtr,
				SInt16								tiffSourceCode,
				CMPaletteInfo						paletteObject,
				UInt16								paletteOffset,
				SInt16								thematicListType,
				SInt16								paletteSelection)

{                                       
	RGBColor								theColor;
	
	ColorSpec*							colorSpecPtr = NULL;
	UInt16*								groupToPalettePtr; 
	
	UInt32								colorIndex,
											count,
											index,
											paletteIndex,
											numberClasses;
	
	SInt16								errCode = -1;
										
	UInt16								blueVectorPtr[256],
											greenVectorPtr[256],
											redVectorPtr[256],
											numberTRLClasses; 
	
	UInt16								numberEntries; 
	
	Boolean								continueFlag;
		                                  
	
	if (fileStreamPtr != NULL)
		{
		continueFlag = TRUE;
		
		if (tiffSourceCode == kFromClassification)
			{
			numberClasses = fileInfoPtr->numberClasses;
			if (numberClasses == 0)
				numberClasses = gProjectInfoPtr->numberStatisticsClasses;
			
			continueFlag = GetClassColorTable (fileInfoPtr, 
														  (UInt16)numberClasses, 
														  NULL,
														  (UInt16)numberClasses,
														  &numberTRLClasses,
														  &colorSpecPtr,
														  NULL,
														  NULL,
														  paletteSelection,
														  (UInt16)numberClasses, 
														  0,
														  thematicListType,
														  kDoNotCollapseClass);
			
					// Now load the colors into the trailer file.
			
			if (continueFlag)
				LoadTIFFColorBuffer (fileInfoPtr,
											colorSpecPtr,
											NULL,
											numberClasses,
											NULL,
											paletteOffset, 
											numberTRLClasses,
											blueVectorPtr,
											greenVectorPtr,
											redVectorPtr,
											kDoNotCollapseClass,
											0);
			
			CheckAndDisposePtr ((Ptr)colorSpecPtr);
			
			}	// end "if (colorTableOriginationCode == kResources)"
		
		else if (tiffSourceCode != kFromClassification)
			{
			colorIndex = 0;
				
			if (paletteObject != NULL)
				{			
						// Initialize some more local variables.									
																 
				numberEntries = MGetNumberPaletteEntries (paletteObject);
				if (paletteOffset == USHRT_MAX)
					numberEntries++;
					
				numberEntries = MIN (256, numberEntries);
				
						// Load the red, green and blue colors.	
						
				if (thematicListType == kClassDisplay)
					{
					if (paletteOffset == USHRT_MAX ||
										// Following will catch when a background class is 
										// added during the reformatting step.
								(tiffSourceCode == kFromReformat && 
										fileInfoPtr->numberClasses == numberEntries+1))
						{
								// The first color is to be white.
								
						redVectorPtr[colorIndex] = 65535; 
						greenVectorPtr[colorIndex] = 65535;
						blueVectorPtr[colorIndex] = 65535;
						colorIndex++;
						paletteOffset = 0;
						
						}	// end "if (paletteOffset == USHRT_MAX)"		
							
					for (index=paletteOffset; index<numberEntries; index++)
						{
						MGetEntryColor (paletteObject, (UInt16)index, &theColor);
						
						redVectorPtr[colorIndex] = theColor.red; 
						greenVectorPtr[colorIndex] = theColor.green;
						blueVectorPtr[colorIndex] = theColor.blue;
						colorIndex++;
															 
						}	// end "for (index=0; index<numberEntries; index++)"  
						
					}	// end "if (thematicListType == kClassDisplay)"
					
				else	// thematicListType == kGroupDisplay
					{
							// Update the inforation to create the correct palette for the 
							// groups.
					
					groupToPalettePtr = (UInt16*)GetGroupToPalettePointer (fileInfoPtr);
				
							// The output file is being converted to appear like a group
							// display in the image window.
							
					if (paletteOffset == USHRT_MAX)
						{
								// The first color is to be white.
								
						redVectorPtr[colorIndex] = 65535; 
						greenVectorPtr[colorIndex] = 65535;
						blueVectorPtr[colorIndex] = 65535;
						colorIndex++;
						paletteOffset = 0;
						
						}	// end "if (paletteOffset == USHRT_MAX)"		
							
					gClassPaletteEntries = numberEntries;
					for (index=0; index<fileInfoPtr->numberClasses; index++)
						{
						paletteIndex = groupToPalettePtr[index] + paletteOffset;
						paletteIndex = GetPaletteEntry ((UInt16)paletteIndex);
						MGetEntryColor (paletteObject, (UInt16)paletteIndex, &theColor);
						
						redVectorPtr[colorIndex] = theColor.red; 
						greenVectorPtr[colorIndex] = theColor.green;
						blueVectorPtr[colorIndex] = theColor.blue;
						colorIndex++;
						
						if (colorIndex >= 256)
							break;
															 
						}	// end "for (index=0; index<fileInfoPtr->numberClasses; index++)"
						
					UnlockGroupTablesHandle (fileInfoPtr);   
						
					}	// end "if (thematicListType == kClassDisplay)"
					
				}	// end "if (paletteObject != NULL)"
					
					// Finish fill with black if needed.
					
			for (index=colorIndex; index<256; index++)
				{
				redVectorPtr[index] = 0;
				greenVectorPtr[index] = 0;
				blueVectorPtr[index] = 0;
				
				}	// end "for (index=colorIndex; index<256; index++)"
			
			}	// end "else if (tiffSourceCode != kFromClassification)"		
		
		if (continueFlag)
			{
			count = 512;
			errCode = MWriteData (fileStreamPtr, &count, redVectorPtr, kErrorMessages);
				
			count = 512;
			if (errCode == noErr)
				errCode = 
						MWriteData (fileStreamPtr, &count, greenVectorPtr, kErrorMessages);
				
			count = 512;
			if (errCode == noErr)
				errCode =  
						MWriteData (fileStreamPtr, &count, blueVectorPtr, kErrorMessages);
			
			}	// end "if (continueFlag)"
		
		}	// end "if (fileStreamPtr != NULL)" 
		
	return (errCode);
	
}	// end "WriteTIFFColorMap2" 


/*                   
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 WriteTIFFColorMap
//
//	Software purpose:	The purpose of this routine is to save the input color
//							map to a TIFF formatted disk file.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/21/1994
//	Revised By:			Larry L. Biehl			Date: 07/22/2010
// TODO: For Linux
SInt16 WriteTIFFColorMap (
				CMFileStream* 						fileStreamPtr,
				SInt16								originationCode,
				CMPaletteInfo						paletteObject,
				UInt16								paletteOffset)

{                                       
#ifndef multispec_lin
	RGBColor								theColor;
	
	UInt32								colorIndex,
											count;
	
	SInt16								errCode = -1;
										
	UInt16								blueVectorPtr[256],
											greenVectorPtr[256],
											redVectorPtr[256]; 
	
	UInt16								index, 
											numberEntries; 
	
	Boolean								continueFlag;
	
	
	if (fileStreamPtr != NULL)
		{
		continueFlag = TRUE;
			
		if (originationCode == kResources)
			{
			continueFlag = GetClassColorTable (gisFileInfoPtr, 
														  numberClasses, 
														  classPtr,
														  numberListClasses,  
														  &colorSpecPtr,
														  paletteHandle,
														  paletteIndexPtr,
														  paletteType,
														  numberPaletteEntriesToRead, 
														  paletteOffset, 
														  classNameCode,
														  thematicListType,
														  collapseClassCode);
			
			}	// end "if (originationCode == kResources)"
	
		else if (originationCode == kPaletteObject)
			{
			if (paletteObject != NULL)
				{
						// Initialize some more local variables.									
																 
				numberEntries = MGetNumberPaletteEntries (paletteObject);
				if (paletteOffset == USHRT_MAX)
					numberEntries++;
					
				numberEntries = MIN (256, numberEntries);
				
						// Load the red, green and blue colors.	
						
				colorIndex = 0;
				if (paletteOffset == USHRT_MAX)
					{
							// The first color is to be white.
							
					redVectorPtr[colorIndex] = 255; 
					greenVectorPtr[colorIndex] = 255;
					blueVectorPtr[colorIndex] = 255;
					colorIndex++;
					paletteOffset = 0;
					
					}	// end "if (paletteOffset == USHRT_MAX)"		
						
				for (index=paletteOffset; index<numberEntries; index++)
					{
					MGetEntryColor (paletteObject, index, &theColor);
					
					redVectorPtr[colorIndex] = theColor.red; 
					greenVectorPtr[colorIndex] = theColor.green;
					blueVectorPtr[colorIndex] = theColor.blue;
					colorIndex++;
														 
					}	// end "for (index=0; index<numberEntries; index++)"  
					
						// Finish fill with black if needed.
						
				for (index=numberEntries-paletteOffset; index<256; index++)
					{
					redVectorPtr[index] = 0;
					greenVectorPtr[index] = 0;
					blueVectorPtr[index] = 0;
					
					}	// end "for (index=numberEntries; index<256; index++)"
					
				}	// end "if (paletteObject != NULL)"
					
			}	// end "else if (originationCode == kPaletteObject)"
		
		if (continueFlag)
			{
			count = 512;
			errCode = 
					MWriteData (fileStreamPtr, &count, redVectorPtr, kErrorMessages);
				
			count = 512;
			if (errCode == noErr)
				errCode = 
					MWriteData (fileStreamPtr, &count, greenVectorPtr, kErrorMessages);
				
			count = 512;
			if (errCode == noErr)
				errCode = 
					MWriteData (fileStreamPtr, &count, blueVectorPtr, kErrorMessages); 
				
			}	// end "if (continueFlag)"
			
		}	// end "if (fileStreamPtr != NULL)"
		
	return (errCode);
	
#else	//  defined multispec_lin
    return -1;
#endif	// else defined multispec_lin

}	// end "WriteTIFFColorMap" 
*/

                   
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				SInt16 WriteTIFFImageData
//
//	Software purpose:			This purpose of this routine is to write the selected
//									image data to the TIFF formatted disk file.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:			WriteTIFFImageFile
//
//	Coded By:			Larry L. Biehl			Date: 12/21/1994
//	Revised By:			Larry L. Biehl			Date: 12/02/2016

SInt16 WriteTIFFImageData (
				CMFileStream* 						fileStreamPtr,
				Rect*									areaRectanglePtr,
				UInt16								paletteOffset)

{     	
	HUCharPtr	 						offScreenBufferPtr,
											savedBufferPtr,
				 							tempBufferPtr,
											tOffScreenBufferPtr;
		                                           
	                   
	Handle								//imageBaseAddressH,
											offScreenMapH;
	
	SInt32								rowBytes;
	
	UInt32								column,         
											count,
											line,
											numberBytes,
											numberBytesPerLine,
											offset;
	
	SInt16								errCode = noErr,
											pixelSize;


			// Initialize local variables.													
			            
	savedBufferPtr = NULL;
	
	offScreenMapH = gImageWindowInfoPtr->offScreenMapHandle; 
			
	#if defined multispec_mac		
		if (gOSXCoreGraphicsFlag)
			{
			rowBytes = gImageWindowInfoPtr->cgInfo.contextRowBytes;
			
			}	// end "if (gOSXCoreGraphicsFlag)"
			
		else	// !gOSXCoreGraphicsFlag
			{							
			rowBytes = MGetPixRowBytes ((PixMapHandle)offScreenMapH);
			
			}	// end "else !gOSXCoreGraphicsFlag"
		 
		pixelSize = ((PixMap*)*offScreenMapH)->pixelSize;
	#endif	// defined multispec_mac 
	
	#if defined multispec_win 
	   BITMAPINFOHEADER* bitMapInfoHeadPtr = (BITMAPINFOHEADER*)GetHandlePointer (
																							offScreenMapH);
	   												                              
	   pixelSize = bitMapInfoHeadPtr->biBitCount;
	   rowBytes = GetNumberPixRowBytes (bitMapInfoHeadPtr->biWidth, pixelSize);
					
		UInt32 firstRowToCopy = 
						(UInt32)bitMapInfoHeadPtr->biHeight - areaRectanglePtr->top;	 
	#endif	// defined multispec_win  

	#if defined multispec_lin
		UInt32		linuxIncrement = 1;
		
				// In linux the offscreenbuffer always has 3 bytes for multispectral 
				// image windows and 1 byte for thematic image windows.
				
		if (!gImageFileInfoPtr->thematicType)
			linuxIncrement = 3;

		DisplaySpecsPtr displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
										gActiveImageViewCPtr->GetDisplaySpecsHandle (), kLock);
		pixelSize = displaySpecsPtr->pixelSize;
		int numberColumns = ((displaySpecsPtr->columnEnd - 
												displaySpecsPtr->columnStart) / 
																displaySpecsPtr->columnInterval) + 1;
		
		rowBytes = numberColumns*linuxIncrement;
	#endif
	
	//imageBaseAddressH = gImageWindowInfoPtr->imageBaseAddressH;
	
	numberBytesPerLine = areaRectanglePtr->right - areaRectanglePtr->left;
	numberBytes = 1;
	
	if (pixelSize == 32)
		{
				// Macintosh format
				
		numberBytesPerLine *= 3;
		numberBytes = 4;
		
		}	// end "if (pixelSize == 32)" 
	
	else if (pixelSize == 24)
		{
				// Windows format
				
		numberBytesPerLine *= 3;
		numberBytes = 3;
		
		}	// end "if (pixelSize == 24)" 
	
	else if (pixelSize == 16)
		{
		numberBytesPerLine *= 3;
		numberBytes = 2;
		
		}	// end "if (pixelSize == 16)" 
		
	else if (pixelSize != 8)
																							return (-1);		
																																										
			// If this is not a palette image, then get a buffer to copy			
			// the offscreen data into.														
	#ifndef multispec_lin		
		if (numberBytes > 1 || paletteOffset >= 1)
			{
			savedBufferPtr = (HUCharPtr)MNewPointer (numberBytesPerLine);
			if (savedBufferPtr == NULL)
																							return (-2);
		
			}	// end "if (numberBytes > 1 || paletteOffset >= 1)" 
	#endif	// #ifndef multispec_lin
   
	#if defined multispec_lin
		savedBufferPtr = (HUCharPtr) MNewPointer (numberBytesPerLine);
		if (savedBufferPtr == NULL)
																							return (-2);
	#endif					
														
			// Get pointer to the offscreen buffer.
			
	#if defined multispec_mac																			
		if (gOSXCoreGraphicsFlag)
			offScreenBufferPtr = (HUCharPtr)GetHandlePointer (
														gImageWindowInfoPtr->imageBaseAddressH);
							
		else	// !gOSXCoreGraphicsFlag
			offScreenBufferPtr = 
					(HUCharPtr)GetPixBaseAddr ((PixMapHandle)offScreenMapH);
					
				// Get pointer to first line to be saved.											
				
		offScreenBufferPtr += (rowBytes * areaRectanglePtr->top);  
	#endif	// defined multispec_mac 
	
	#if defined multispec_win                                            
		offScreenBufferPtr = 
					(HUCharPtr)GetHandlePointer (
												gImageWindowInfoPtr->imageBaseAddressH, kLock);  
		
				// Get pointer to first line to be saved.
				// Take into account that the image is stored in reverse order.											
				
		offScreenBufferPtr += (rowBytes * (firstRowToCopy - 1));
		rowBytes = -rowBytes; 
	#endif	// defined multispec_win 

	#if defined multispec_lin
		offScreenBufferPtr = (HUCharPtr) GetHandlePointer (
														gImageWindowInfoPtr->imageBaseAddressH);

				// Get pointer to first line to be saved.											

		offScreenBufferPtr += (rowBytes * areaRectanglePtr->top);
	#endif
		
	count = numberBytesPerLine;	
															
	#ifdef multispec_lin	
		offset = areaRectanglePtr->left * linuxIncrement;
	#endif	
													
	#ifndef multispec_lin		
		offset = areaRectanglePtr->left * numberBytes;
	#endif
						
	if (numberBytes == 1 && paletteOffset == 0)
		{
		for (line=(UInt32)areaRectanglePtr->top; 
				line<(UInt32)areaRectanglePtr->bottom; 
					line++)
			{
			#if defined multispec_lin
				tempBufferPtr = savedBufferPtr;
				tOffScreenBufferPtr = &offScreenBufferPtr[offset];
			
				for (column = (UInt32) areaRectanglePtr->left;
						  column < (UInt32) areaRectanglePtr->right;
						  column++) 
					{
					*tempBufferPtr = *tOffScreenBufferPtr;		
					tOffScreenBufferPtr += linuxIncrement;
					tempBufferPtr++;

					}
					
				errCode = MWriteData (fileStreamPtr,
						  &count,
						  savedBufferPtr,
						  kErrorMessages);
			#endif
			
			#ifndef multispec_lin
				errCode = MWriteData (fileStreamPtr, 
												&count, 
												&offScreenBufferPtr[offset],
												kErrorMessages);
			#endif
			
			if (errCode != noErr)
				break;
				
			offScreenBufferPtr += rowBytes;
			
			}	// end "for (line=areaRectanglePtr->top; ..." 
			
		}	// end "if (numberBytes == 1 && paletteOffset == 0)"
		
	else if (numberBytes == 1 && paletteOffset == 1)
		{
		for (line=(UInt32)areaRectanglePtr->top; 
				line<(UInt32)areaRectanglePtr->bottom; 
					line++)
			{                              
			tempBufferPtr = savedBufferPtr;
			tOffScreenBufferPtr = &offScreenBufferPtr[offset];
			
			for (column=(UInt32)areaRectanglePtr->left; 
					column<(UInt32)areaRectanglePtr->right;
						column++)
				{                     
				*tempBufferPtr = *tOffScreenBufferPtr - 1;	
				tempBufferPtr++;
				tOffScreenBufferPtr++;
				
				}	// end "for (column=areaRectanglePtr->left; ..." 
				
			errCode = MWriteData (fileStreamPtr, 
											&count, 
											savedBufferPtr,
											kErrorMessages);
			
			if (errCode != noErr)
				break;
				
			offScreenBufferPtr += rowBytes;
			
			}	// end "for (line=areaRectanglePtr->top; ..." 
		
		}	// end "else if (numberBytes == 1 && paletteOffset == 1)"
		
	else if (numberBytes == 2)
		{
		HUInt16Ptr 			t2OffScreenBufferPtr;
												
		for (line=(UInt32)areaRectanglePtr->top; 
					line<(UInt32)areaRectanglePtr->bottom; 
						line++)
			{
			tempBufferPtr = savedBufferPtr;
			t2OffScreenBufferPtr = (HUInt16Ptr)&offScreenBufferPtr[offset];
			
			for (column=(UInt32)areaRectanglePtr->left; 
					column<(UInt32)areaRectanglePtr->right;
						column++)
				{
				*tempBufferPtr++ = (UInt8)((*t2OffScreenBufferPtr & 0x7c00) >> 7);
				*tempBufferPtr++ = (UInt8)((*t2OffScreenBufferPtr & 0x03d0) >> 2);
				*tempBufferPtr++ = (UInt8)((*t2OffScreenBufferPtr & 0x001f) << 3);
				
				t2OffScreenBufferPtr++;
				
				}	// end "for (column=areaRectanglePtr->left; ..." 
				
			errCode = MWriteData (fileStreamPtr, 
											&count, 
											savedBufferPtr,
											kErrorMessages);
			
			if (errCode != noErr)
				break;
				
			offScreenBufferPtr += rowBytes;
			
			}	// end "for (line=areaRectanglePtr->top; ..." 
			
		}	// end "else if (numberBytes == 2)"   
			
	else if (numberBytes == 3) 
		{       
		for (line=(UInt32)areaRectanglePtr->top; 
				line<(UInt32)areaRectanglePtr->bottom; 
					line++)
			{                              
			tempBufferPtr = savedBufferPtr;
			tOffScreenBufferPtr = &offScreenBufferPtr[offset];
			
			for (column=(UInt32)areaRectanglePtr->left; 
					column<(UInt32)areaRectanglePtr->right;
						column++)
				{        
				#ifndef multispec_lin
					tempBufferPtr[2] = *tOffScreenBufferPtr++;
					tempBufferPtr[1] = *tOffScreenBufferPtr++;
					tempBufferPtr[0] = *tOffScreenBufferPtr++;
				#endif
				
				#if defined multispec_lin
					tempBufferPtr[0] = *tOffScreenBufferPtr++;
					tempBufferPtr[1] = *tOffScreenBufferPtr++;
					tempBufferPtr[2] = *tOffScreenBufferPtr++;
				#endif
				
				tempBufferPtr += 3;
				
				}	// end "for (column=areaRectanglePtr->left; ..." 
				
			errCode = MWriteData (fileStreamPtr, 
											&count, 
											savedBufferPtr,
											kErrorMessages);
			
			if (errCode != noErr)
				break;
				
			offScreenBufferPtr += rowBytes;
			
			}	// end "for (line=areaRectanglePtr->top; ..." 
			
		}	// end "else numberBytes == 4" 
			
	else 	// numberBytes == 4 
		{
		for (line=(UInt32)areaRectanglePtr->top; 
				line<(UInt32)areaRectanglePtr->bottom; 
					line++)
			{
			tempBufferPtr = savedBufferPtr;
			tOffScreenBufferPtr = &offScreenBufferPtr[offset];
			
			for (column=(UInt32)areaRectanglePtr->left; 
					column<(UInt32)areaRectanglePtr->right;
						column++)
				{
				tOffScreenBufferPtr++;
				*tempBufferPtr++ = *tOffScreenBufferPtr++;
				*tempBufferPtr++ = *tOffScreenBufferPtr++;
				*tempBufferPtr++ = *tOffScreenBufferPtr++;
				
				}	// end "for (column=areaRectanglePtr->left; ..." 
				
			errCode = MWriteData (fileStreamPtr, 
											&count, 
											savedBufferPtr,
											kErrorMessages);
			
			if (errCode != noErr)
				break;
				
			offScreenBufferPtr += rowBytes;
			
			}	// end "for (line=areaRectanglePtr->top; ..." 
			
		}	// end "else numberBytes == 4" 
		
			// Unlock Handle to offscreen storage.	
	/*
	#if defined multispec_mac									
		if (!gOSXCoreGraphicsFlag)
			{
			if (gImageWindowInfoPtr->offscreenMapSize < gMaxMoveableBlockSize)
				UnlockPixels ((PixMapHandle)offScreenMapH); 
						
			}	// end "if (!gOSXCoreGraphicsFlag)"
	#endif	// defined multispec_mac 
	*/
	#if defined multispec_win || defined multispec_lin 
		CheckAndUnlockHandle (gImageWindowInfoPtr->imageBaseAddressH);  
	#endif	// defined multispec_win || defined multispec_lin 
			
	CheckAndDisposePtr (savedBufferPtr);
				
	return (errCode);
	
}	// end "WriteTIFFImageData" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean WriteTIFFImageFile
//
//	Software purpose:	This routine writes the input image window as a TIFF
//							formatted disk file.
//
//	Parameters in:		...
//							startMapColumn & startMapLine represent the upper left line and 
//							 column value relative to the upperleft map coordinates values
//							 in the input map projection structure.  If greater than (1,1), 
//							 the upper left map values need to be adjusted.
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:			CreateResultsDiskFile in SUtility.cpp
//							SaveImageWindowAs in SSaveWrt.cpp
//							WriteNewImageHeader in SReform2.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/20/1994
//	Revised By:			Larry L. Biehl			Date: 01/27/2017

Boolean WriteTIFFImageFile (
				FileInfoPtr							fileInfoPtr,
				CMFileStream* 						fileStreamPtr, 
				DisplaySpecsPtr					displaySpecsPtr,
				Rect*									areaRectanglePtr,
				SInt16								tiffSourceCode,
				SInt16								thematicListType,
				SInt16								paletteSelection,
				SInt32*								imageDataStartPtr,
				UInt32								startMapColumn,
				UInt32								startMapLine)

{  			
	char									textString[1024];
				                                
	TIFFImageFileDirectory			imageFileDirectory;
	
	unsigned char						charKeyCode[4];
										
	CMPaletteInfo						paletteObject;
	//UInt16*							imageFileDirectoryShortValuePtr;
	UCharPtr								bufferPtr;
	
	SInt32								index,
											tempLongInt;
	/*										
	#if defined multispec_mac
		SInt32								TIFFMagicNumber1 = 0x4d4d002a;
		SInt32								TIFFMagicNumber2 = 0x4949002a;
	#endif	// defined multispec_mac            
										
	#if defined multispec_win
		SInt32								TIFFMagicNumber1 = 0x002a4949;
		SInt32								TIFFMagicNumber2 = 0x002a4d4d;
	#endif	// defined multispec_win
	*/	
	UInt32								bytesInLastStrip,
											bytesPerStrip,
											columnInterval,
											colorTableOffset,
											count,
											geoDataOffset,
											i,
											imageDataOffset,
											j,
											lineInterval,
											bytesPerChannel,
											numberColumns,
											numberGeoDataBytes,
											numberGeoEntries,
											numberLines,
											numberStrips,
											numberStripSets,
											numberStripsPerChannel,
											numberTIFFChannels,
											otherValueOffset,
											otherValueOffsetStart,
											rowsPerStrip,
											tempULongInt,
											tempULongInt2,
											textStringBufferLength,
											textStringLength,
											textStringStart;
	
	SInt16								errCode = noErr,
											numberBits,
											numberBytes,
											planarConfiguration,
											photoInterpret,
											sampleFormatCode,
											tempShortInt;
											
	UInt16								numberEntries,
											paletteOffset;
											
	Boolean								saved_gSwapBytesFlag;
		                                 
		
	if (fileStreamPtr != NULL)
		{
		saved_gSwapBytesFlag = gSwapBytesFlag;
		//imageFileDirectoryShortValuePtr = (UInt16*)&imageFileDirectory.value;
		
		if (tiffSourceCode == kFromImageWindow)
			{
			numberLines = areaRectanglePtr->bottom - areaRectanglePtr->top;
			numberColumns = areaRectanglePtr->right - areaRectanglePtr->left;
						
			columnInterval = displaySpecsPtr->displayedColumnInterval;
			lineInterval = displaySpecsPtr->displayedLineInterval;
			
			paletteObject = displaySpecsPtr->paletteObject;
			paletteOffset = displaySpecsPtr->paletteOffset;
		
					// Get the code for the color space of the image.						
					
			photoInterpret = 3;
			if (displaySpecsPtr->pixelSize >= 16)
				photoInterpret = 2;
				
					// Get the number of TIFF channels for the image.						
			
			numberTIFFChannels = 1;
			if (photoInterpret == 2)
				numberTIFFChannels = 3;
				
			numberBits = 8;
			numberBytes = 1;
			
			gSwapBytesFlag = FALSE;
				
			planarConfiguration = 1;
			
			sampleFormatCode = 1;
				
			}	// end "if (tiffSourceCode == kFromImageWindow)"
			
		else	// tiffSourceCode != kFromImageWindow
			{
			numberLines = fileInfoPtr->numberLines;
			numberColumns = fileInfoPtr->numberColumns;
						
			columnInterval = 1;
			lineInterval = 1;
			
			paletteObject = NULL;
			paletteOffset = 0;
			
			if (displaySpecsPtr != NULL)
				{
				paletteObject = displaySpecsPtr->paletteObject;
				paletteOffset = displaySpecsPtr->paletteOffset;
				
				}	// end "if (displaySpecsPtr != NULL)"
				
			else 		// displaySpecsPtr == NULL
				{
				paletteObject = GetPaletteHandle ();
 				paletteOffset = GetPaletteOffset ();
				
				}	// end "else displaySpecsPtr == NULL"
			
			if (tiffSourceCode == kFromClassification)
				{
				paletteObject = NULL;
				paletteOffset = 0;
				
				}	// end "if (tiffSourceCode == kFromClassification)"
				
					// Get the number of TIFF channels for the image.						
			
			numberTIFFChannels = fileInfoPtr->numberChannels;
			numberBits = fileInfoPtr->numberBits;
			numberBytes = fileInfoPtr->numberBytes;
			
			gSwapBytesFlag = fileInfoPtr->swapBytesFlag;
				
			planarConfiguration = 2;
			if (fileInfoPtr->bandInterleave == kBIS)
				planarConfiguration = 1;
		
					// Get the code for the color space of the image.						
					
			photoInterpret = 3;
			if (!fileInfoPtr->thematicType)
				{
				photoInterpret = 2;		// RGB
				
				if (numberTIFFChannels < 3)
					photoInterpret = 1;		// Black is Zero
				
				}	// end "if (!fileInfoPtr->thematicType)"
			
			sampleFormatCode = 1;
			if (fileInfoPtr->signedDataFlag)
				sampleFormatCode = 2;
			
			if (fileInfoPtr->dataTypeCode == kRealType)
				sampleFormatCode = 3;
			
			}	// end "else tiffSourceCode != kFromImageWindw"
	
				// Set some needed parameters.	
			
		bytesPerChannel = numberLines * numberColumns * numberBytes;
		
		rowsPerStrip = GetNumberStripsToUse (planarConfiguration,
															numberColumns,
															numberLines,
															numberTIFFChannels,
															numberBytes,
															&numberStrips,
															&numberStripsPerChannel,
															&bytesPerStrip,
															&bytesInLastStrip);
			
		numberStripSets = 1;
		if (planarConfiguration == 2)
			numberStripSets = numberTIFFChannels;
		
				// Get the number directory entries.										
				
		numberEntries = 16;
		if (photoInterpret == 3)
			numberEntries += 1;
		if (planarConfiguration == 1 && numberTIFFChannels > 3)
			numberEntries += 1;
			
				// Get entries for Geo information.
				
		WriteGeoTIFFInformation (fileInfoPtr, 
											fileStreamPtr, 
											0,
											0,
											kGetNumberEntriesOnly,
											&numberGeoEntries,
											&numberGeoDataBytes,
											columnInterval,
											lineInterval,
											startMapColumn,
											startMapLine);
											
		numberEntries += (UInt16)numberGeoEntries;
			
				// Get the offset where other data start. The last 4 bytes are the 4 
				// bytes of 0 at the end of the image file directory.									
				
		otherValueOffsetStart = 10 + numberEntries * 12 + 4;
		otherValueOffset = otherValueOffsetStart;
			
				// Get the offset where the image data starts.
					
		imageDataOffset = otherValueOffset;
				
				// for "258: BitsPerSample"
		if (photoInterpret <= 2 && numberTIFFChannels > 2)
			imageDataOffset += 2 * numberTIFFChannels;
			
				// for "270: Image Description"				
				// Get the date and time to add.

		time_t currentTime = time (NULL);
		struct tm *currentDate = localtime (&currentTime);
		strftime ((char*)gTextString3, 254, "%m-%d-%Y  %X", currentDate);
				
		textStringLength = sprintf (textString, 
												"Generated by %s on %s |", 
												(char*)gApplicationIdentifierString,
												(char*)gTextString3);
								
		textStringLength += LoadInstrumentNameToTIFFAsciiTag (
								fileInfoPtr, tiffSourceCode, &textString[textStringLength]);
		textString[textStringLength] = 0;
		textStringLength++;
		textStringStart = imageDataOffset;
		
				// Force buffer for string to be multiple of 4
		textStringBufferLength = ((textStringLength+3)/4) * 4;
		
		imageDataOffset += textStringBufferLength;
			
				// for "273: StripOffsets"
				// for "279: StripByteCounts"														
		/*
		if (numberStrips == 1)
			{
			if (planarConfiguration == 2 && numberTIFFChannels > 1)
				imageDataOffset += 2 * 4 * numberTIFFChannels;
				
			}	// end "if (numberStrips == 1)"
			
		else	// numberStrips > 1
			{
		*/
		if (numberStrips > 1)
			imageDataOffset += 2 * 4 * numberStrips;
				
				// for "282: XResolution & 283: YResolution"
		imageDataOffset += 16;
				
				// for "338: Extra Samples"
		if (planarConfiguration == 1 && numberTIFFChannels > 5)
			imageDataOffset += 2 * (numberTIFFChannels-3);
				
				// for "339: Sample Format"
		if (numberTIFFChannels > 2)
			imageDataOffset += 2 * numberTIFFChannels;
		
				// for "320: ColorMap"
		colorTableOffset = imageDataOffset;
		if (photoInterpret == 3)
			imageDataOffset += 2 * 768;
			
		geoDataOffset = imageDataOffset;					
				
		imageDataOffset += numberGeoDataBytes;
		
				// Get buffer to load tag information in.
				
		bufferPtr = (UCharPtr)MNewPointer (colorTableOffset+100);
		if (bufferPtr == NULL)
																						return (FALSE);
			
				// Load the TIFF identifier string.		
		
		if (gSwapBytesFlag == !gBigEndianFlag)
			{
			charKeyCode[0] = 0x4d;
			charKeyCode[1] = 0x4d;
			charKeyCode[2] = 0x00;
			charKeyCode[3] = 0x2a;	
			
			}	// end "if (gSwapBytesFlag == !gBigEndianFlag)"	                                                
			
		else	// gSwapBytesFlag == gBigEndianFlag
			{              
			charKeyCode[0] = 0x49;
			charKeyCode[1] = 0x49;
			charKeyCode[2] = 0x2a;
			charKeyCode[3] = 0x00;             
				
			}	// end "else gSwapBytesFlag == gBigEndianFlag"									
				
		BlockMoveData ((char*)charKeyCode, bufferPtr, 4);
		index = 4;
				
				// Load the offset of the first Image File Directory.					
			
		tempLongInt = 0x00000008;
		tempLongInt = GetLongIntValue ((char*)&tempLongInt);
		BlockMoveData ((char*)&tempLongInt, &bufferPtr[index], 4);
		index += 4;
				
      tempShortInt = GetShortIntValue ((char*)&numberEntries);
		BlockMoveData ((char*)&tempShortInt, &bufferPtr[index], 2);
		index += 2;
		
				// 254: Sub File Type.	
				
		LoadTiffEntry (&imageFileDirectory,
							254,
							kTIFFLong,
							1,
							0);															
		
		BlockMoveData (&imageFileDirectory, &bufferPtr[index], 12);
		index += 12;
				
				// 256: Image Width.	
				
		LoadTiffEntry (&imageFileDirectory,
							256,
							kTIFFLong,
							1,
							numberColumns);																		
		
		BlockMoveData (&imageFileDirectory, &bufferPtr[index], 12);
		index += 12;
		
				// 257: Image Length.		
				
		LoadTiffEntry (&imageFileDirectory,
							257,
							kTIFFLong,
							1,
							numberLines);														
		
		BlockMoveData (&imageFileDirectory, &bufferPtr[index], 12);
		index += 12;
		
				// 258: BitsPerSample.												
		
		imageFileDirectory.value = 0;
		
		if (photoInterpret <= 2)
			{
			if (numberTIFFChannels == 1)
				imageFileDirectory.value = numberBits;

			else if (numberTIFFChannels == 2)
				imageFileDirectory.value = (numberBits<<16) + numberBits;
				
			else	// numberTIFFChannels > 2
				{
				imageFileDirectory.value = otherValueOffset;
				
      		tempShortInt = GetShortIntValue ((char*)&numberBits);
				for (i=0; i<numberTIFFChannels; i++)
					{
					BlockMoveData ((char*)&tempShortInt, &bufferPtr[otherValueOffset], 2);
					otherValueOffset += 2;
				
					}	// end "for (i=0; i<numberTIFFChannels; i++)" 
					
				}	// end "else numberTIFFChannels > 2"
			
			}	// end "if (photoInterpret <= 2)" 
		
		else	// photoInterpret > 2 
			imageFileDirectory.value = numberBits;	
				
		LoadTiffEntry (&imageFileDirectory,
							258,
							kTIFFShort,
							numberTIFFChannels,
							imageFileDirectory.value);				
			
		BlockMoveData (&imageFileDirectory, &bufferPtr[index], 12);
		index += 12;
		
				// 259: Compression.																
						
		LoadTiffEntry (&imageFileDirectory,
							259,
							kTIFFShort,
							1,
							1);				 
		BlockMoveData (&imageFileDirectory, &bufferPtr[index], 12);
		index += 12;
		
				// 262: PhotoInterp.																
		
		LoadTiffEntry (&imageFileDirectory,
							262,
							kTIFFShort,
							1,
							photoInterpret);				
		BlockMoveData (&imageFileDirectory, &bufferPtr[index], 12);
		index += 12;
		
				// 270: Image Description
							
		LoadTiffEntry (&imageFileDirectory,
							270,
							kTIFFASCII,
							textStringLength,
							textStringStart);				
		BlockMoveData (&imageFileDirectory, &bufferPtr[index], 12);
		index += 12;
		
		BlockMoveData (textString, &bufferPtr[otherValueOffset], textStringBufferLength);
		otherValueOffset += textStringBufferLength;
		
				// 273: StripOffsets.															
		
		if (numberStrips == 1)
			{
			/*
			if (planarConfiguration == 2 && numberTIFFChannels > 1)
				{
				imageFileDirectory.count = numberTIFFChannels;
				imageFileDirectory.value = otherValueOffset;
					
				tempULongInt = imageDataOffset;
				for (i=0; i<numberTIFFChannels; i++)
					{
					tempULongInt2 = GetLongIntValue ((char*)&tempULongInt);
					BlockMoveData ((char*)&tempULongInt2, &bufferPtr[otherValueOffset], 4);
					otherValueOffset += 4;
					tempULongInt += bytesPerChannel;
				
					}	// end "for (i=0; i<numberTIFFChannels; i++)"
				
				}	// end "if (planarConfiguration == 2 && ..." 
			
			else	// if (planarConfiguration == 1 || numberTIFFChannels == 1) 
				{ 
			*/
				imageFileDirectory.count = 1;
				imageFileDirectory.value = imageDataOffset;
				
			//	}	// end "else planarConfiguration == 1 || ..."
				
			}	// end "if (numberStrips == 1)"
			
		else	// numberStrips > 1
			{
			imageFileDirectory.count = numberStrips;
			imageFileDirectory.value = otherValueOffset;
				
			tempULongInt = imageDataOffset;
			for (i=1; i<=numberStripSets; i++)
				{
				for (j=1; j<=numberStripsPerChannel; j++)
					{
					tempULongInt2 = GetLongIntValue ((char*)&tempULongInt);
					BlockMoveData ((char*)&tempULongInt2, &bufferPtr[otherValueOffset], 4);
					otherValueOffset += 4;
					
					if (j < numberStripsPerChannel)
						tempULongInt += bytesPerStrip;
						
					else	// j == numberStripsPerChannel
						tempULongInt += bytesInLastStrip;
				
					}	// end "for (j=1; j<=numberOfStripsPerChannel; j++)"
					
				}	// end "for (i=1; i<=numberStripSets; i++)"
			
			}	// end "else numberStrips > 1" 
				
		LoadTiffEntry (&imageFileDirectory,
							273,
							kTIFFLong,
							imageFileDirectory.count,
							imageFileDirectory.value);				
		BlockMoveData (&imageFileDirectory, &bufferPtr[index], 12);
		index += 12;
		
				// 277: SamplesPerPixel.														
		
		LoadTiffEntry (&imageFileDirectory,
							277,
							kTIFFShort,
							1,
							numberTIFFChannels);				
		
		BlockMoveData (&imageFileDirectory, &bufferPtr[index], 12);
		index += 12;
		
				// 278: RowsPerStrip.															
		
		//rowsPerStrip = numberLines;
		//if (rowsPerStrip > SInt32_MAX/2)
		//	rowsPerStrip = 1;
		LoadTiffEntry (&imageFileDirectory,
							278,
							kTIFFLong,
							1,
							rowsPerStrip);			
		BlockMoveData (&imageFileDirectory, &bufferPtr[index], 12);
		index += 12;
		
				// 279: StripByteCounts.																
		
		if (numberStrips == 1)
			{	
			/*												
			if (planarConfiguration == 2 && numberTIFFChannels > 1)
				{
				imageFileDirectory.count = numberTIFFChannels;
				imageFileDirectory.value = otherValueOffset;
				
				tempULongInt = GetLongIntValue ((char*)&bytesPerChannel);
				for (i=0; i<numberTIFFChannels; i++)
					{
					BlockMoveData ((char*)&tempULongInt, &bufferPtr[otherValueOffset], 4);
					otherValueOffset += 4;
				
					}	// end "for (i=0; i<numberTIFFChannels; i++)"
				
				}	// end "if (planarConfiguration == 2 && ..." 
			
			else	// if (planarConfiguration == 1 || numberTIFFChannels == 1) 
				{ 
			*/
				imageFileDirectory.count = 1;
				//imageFileDirectory.value = bytesPerChannel;
				imageFileDirectory.value = bytesPerStrip;
				
			//	if (planarConfiguration == 1)
			//		imageFileDirectory.value = bytesPerChannel * numberTIFFChannels;
							
			//	}	// end "else planarConfiguration == 1 || ..."	
				
			}	// end "if (numberStrips == 1)"
			
		else	// numberStrips > 1
			{
			imageFileDirectory.count = numberStrips;
			imageFileDirectory.value = otherValueOffset;
				
			for (i=1; i<=numberStripSets; i++)
				{
				tempULongInt = GetLongIntValue ((char*)&bytesPerStrip);
				for (j=1; j<=numberStripsPerChannel; j++)
					{
					if (j == numberStripsPerChannel)
						tempULongInt = GetLongIntValue ((char*)&bytesInLastStrip);
						
					BlockMoveData ((char*)&tempULongInt, &bufferPtr[otherValueOffset], 4);
					otherValueOffset += 4;
				
					}	// end "for (j=1; j<=numberOfStripsPerChannel; j++)"
					
				}	// end "for (i=1; i<=numberStripSets; i++)"
			
			}	// end "else numberStrips > 1" 
				
		LoadTiffEntry (&imageFileDirectory,
							279,
							kTIFFLong,
							imageFileDirectory.count,
							imageFileDirectory.value);				
		BlockMoveData (&imageFileDirectory, &bufferPtr[index], 12);
		index += 12;
		
				// 282: XResolution.																
		
		LoadTiffEntry (&imageFileDirectory,
							282,
							kTIFFRational,
							1,
							otherValueOffset);				
		BlockMoveData (&imageFileDirectory, &bufferPtr[index], 12);
		index += 12;
		
		tempLongInt = 0x00000001;
      tempLongInt = GetLongIntValue ((char*)&tempLongInt);
		BlockMoveData ((char*)&tempLongInt, &bufferPtr[otherValueOffset], 4);
		otherValueOffset += 4;
		BlockMoveData ((char*)&tempLongInt, &bufferPtr[otherValueOffset], 4);
		otherValueOffset += 4;
		
				// 283: YResolution.																	
		
		LoadTiffEntry (&imageFileDirectory,
							283,
							kTIFFRational,
							1,
							otherValueOffset);				
		BlockMoveData (&imageFileDirectory, &bufferPtr[index], 12);
		index += 12;
		
		BlockMoveData ((char*)&tempLongInt, &bufferPtr[otherValueOffset], 4);
		otherValueOffset += 4;
		BlockMoveData ((char*)&tempLongInt, &bufferPtr[otherValueOffset], 4);
		otherValueOffset += 4;
		
				// 284: PlanarConfiguration.														
		
		LoadTiffEntry (&imageFileDirectory,
							284,
							kTIFFShort,
							1,
							planarConfiguration);				
		BlockMoveData (&imageFileDirectory, &bufferPtr[index], 12);
		index += 12;
		
				// 296: Resolution Unit.															
		
		LoadTiffEntry (&imageFileDirectory,
							296,
							kTIFFShort,
							1,
							1);				
		BlockMoveData (&imageFileDirectory, &bufferPtr[index], 12);
		index += 12;
		
				// 320: ColorMap.	
		
		if (photoInterpret == 3)
			{	
			LoadTiffEntry (&imageFileDirectory,
								320,
								kTIFFShort,
								768,
								colorTableOffset);	
			BlockMoveData (&imageFileDirectory, &bufferPtr[index], 12);
			index += 12;
			
			}	// end "if (photoInterpret == 3)"
			
				// 338: Extra Samples.	
		
		if (planarConfiguration == 1 && numberTIFFChannels > 3)
			{	
			imageFileDirectory.value = 0;
			
			if (numberTIFFChannels > 5)
				{
				imageFileDirectory.value = otherValueOffset;
				tempShortInt = 0;
				tempShortInt = GetShortIntValue ((char*)&tempShortInt);
			
				for (i=3; i<numberTIFFChannels; i++)
					{
					BlockMoveData ((char*)&tempShortInt, &bufferPtr[otherValueOffset], 2);
					otherValueOffset += 2;
					
					}	// end "for (i=0; i<numberTIFFChannels; i++)" 
				
				}	// end "if (numberTIFFChannels > 5)"
				
			LoadTiffEntry (&imageFileDirectory,
								338,
								kTIFFShort,
								numberTIFFChannels - 3,
								imageFileDirectory.value);	
			BlockMoveData (&imageFileDirectory, &bufferPtr[index], 12);
			index += 12;
			
			}	// end "if (planarConfiguration == 1 && numberTIFFChannels > 3)"
			
				// 339: Sample Format.															
		
		imageFileDirectory.value = 0;
		
		if (numberTIFFChannels == 1)
			imageFileDirectory.value = sampleFormatCode;

		else if (numberTIFFChannels == 2)
			imageFileDirectory.value = (sampleFormatCode<<16) + sampleFormatCode;
		
		else	// numberTIFFChannels > 2 
			{
			imageFileDirectory.value = otherValueOffset;
      	tempShortInt = GetShortIntValue ((char*)&sampleFormatCode);
			
			for (i=0; i<numberTIFFChannels; i++)
				{
				BlockMoveData ((char*)&tempShortInt, &bufferPtr[otherValueOffset], 2);
				otherValueOffset += 2;
				
				}	// end "for (i=0; i<numberTIFFChannels; i++)" 
			
			}	// end "else numberTIFFChannels > 1" 
							
		LoadTiffEntry (&imageFileDirectory,
							339,
							kTIFFShort,
							numberTIFFChannels,
							imageFileDirectory.value);	
		BlockMoveData (&imageFileDirectory, &bufferPtr[index], 12);
		index += 12;
				
				// Force the rest of this buffer to be 0's. Any GeoTIFF information will 
				// be loaded later. The last 4 bytes of 0 indicate the end of the first 
				// Image File Directory.					
		
		for (i=index; i<otherValueOffsetStart; i++)
			bufferPtr[i] = 0;
	
				// Position the marker in the image file to prepare for writing		
				// the header																			
			  
		errCode = MSetMarker (fileStreamPtr, fsFromStart, 0, kErrorMessages);
		if (errCode == noErr)
			{
					// Write the TIFF file directories and associated data except for color map.	
			
			count = otherValueOffset;
			errCode = MWriteData (fileStreamPtr, &count, bufferPtr, kErrorMessages);
				
			}	// end "if (errCode == noErr)"
		
		bufferPtr = CheckAndDisposePtr (bufferPtr);
		
				// Write the TIFF Color Map if needed.											
				
		if (errCode == noErr && photoInterpret == 3)
			errCode = WriteTIFFColorMap2 (fileInfoPtr, 
													fileStreamPtr,
													tiffSourceCode,
													paletteObject,
													paletteOffset,
													thematicListType,
													paletteSelection);
													
				// Write the Geo information if available. It will be written over the
				// 0's that were entered above.
				
		if (errCode == noErr)
			errCode = WriteGeoTIFFInformation (fileInfoPtr, 
															fileStreamPtr, 
															index,
															geoDataOffset,
															kWriteGeoInformation,
															&numberGeoEntries,
															&numberGeoDataBytes,
															columnInterval,
															lineInterval,
															startMapColumn,
															startMapLine);
															
		gSwapBytesFlag = saved_gSwapBytesFlag;
			
				// Write the image data.															
				
		if (errCode == noErr && tiffSourceCode == kFromImageWindow)
			errCode = WriteTIFFImageData (fileStreamPtr, 
													areaRectanglePtr,
													paletteOffset);
													
		if (errCode == noErr && (fileInfoPtr->thematicType || 
				(displaySpecsPtr != NULL &&
							displaySpecsPtr->displayType == k1_ChannelThematicDisplayType)))
			{                        
					// Get copy of fileInfoPtr structure and change the file name so that
					// it matches that in the fileStreamPtr structure/class. The temp
					// MFileInfo structure will then be used by 'CreateThematicSupportFile'
					
			MFileInfo			thematicFileInfo;
			CMFileStream*		thematicFileStreamPtr;
			
			memcpy (&thematicFileInfo, fileInfoPtr, sizeof (MFileInfo));
			
					// ASCII symbol vector conversion is not needed for this. 
			thematicFileInfo.asciiSymbols = FALSE;
			
			#if defined multispec_lin || defined multispec_win
				thematicFileInfo.fileStreamCPtr = new CMFileStream;
			#endif	// defined multispec_lin || defined multispec_win
			
			thematicFileStreamPtr = GetFileStreamPointer (&thematicFileInfo);
			CopyFileStream (thematicFileStreamPtr, fileStreamPtr);
			
			CreateThematicSupportFile (&thematicFileInfo,
												NULL, 
												(UInt16)fileInfoPtr->numberClasses, 
												NULL,
												0,
												NULL, 
												GetPaletteHandle (),
												NULL,		// newPaletteIndexPtr, 
												GetPaletteType (),
												(UInt16)fileInfoPtr->numberClasses, 
												kFromDescriptionCode, 
												kPaletteHistogramClassNames,
												kClassDisplay,
												kCollapseClass,
												kICLRFileType);
											
			#if defined multispec_lin || defined multispec_win
				delete thematicFileInfo.fileStreamCPtr;
			#endif	// defined multispec_lin || defined multispec_win
											
			}	// end "if (continueFlag && fileInfoPtr->thematicType || ..." 
													
		if (imageDataStartPtr != NULL)
			*imageDataStartPtr = imageDataOffset;
		
		}	// end "if (fileStreamPtr != NULL)"
		
	return (errCode == noErr);
	
}	// end "WriteTIFFImageFile"


                   
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void WriteTransformationFile
//
//	Software purpose:	The purpose of this routine is to write the transformation
//							matrix structure to a disk file.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:			Menus in menus.c
//
//	Coded By:			Larry L. Biehl			Date: 07/12/1993
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

void WriteTransformationFile (void)

{
	CMFileStream						fileStreamBlock;
	CMFileStream*						fileStreamPtr;
	HPtr		 							ptr;
	FileStringPtr						filePathPtr;
	    
	#if defined multispec_mac || defined multispec_mac_swift
		#ifdef _MC68881_
			double							extendedValue;
		#endif
		
		#ifndef _MC68881_
			#if TARGET_RT_MAC_MACHO	
				double							extendedValue;
			#else	// !TARGET_RT_MAC_MACHO
				extended96						extendedValue;
				double*							doublePtr;
				UInt32							index;
			#endif	// TARGET_RT_MAC_MACHO, else...
		#endif                                      
	#endif  
	  
	#if defined multispec_win  
		double								extendedValue;                                   
	#endif  
	
   #if defined multispec_lin  
		double extendedValue;
   #endif 
	
	SInt32								fileLength;
	
	UInt32								count,
											i,
											j;
	
	OSErr 								errCode;
	
	Boolean								continueFlag;
	
	
	continueFlag = FALSE;
	
			// Get a new handle to a block of memory to be used for					
			// file information for the new image window.								
	
	fileStreamPtr = &fileStreamBlock;	                     
	InitializeFileStream (fileStreamPtr);
	filePathPtr = (FileStringPtr)GetFilePathPPointerFromFileStream (fileStreamPtr);
		
			// Set up default name.																

	CopyPToP (filePathPtr, gTransformationMatrix.imageFileName);
	RemoveSuffix (filePathPtr);

	#if defined multispec_mac
		ConcatFilenameSuffix (filePathPtr, (StringPtr)"\0.TransformMatrix\0");
	#endif	// defined multispec_win
	
	#if defined multispec_win
		ConcatFilenameSuffix (filePathPtr, (StringPtr)"\0.tra\0");
	#endif	// defined multispec_win

			// Now get wide character and unicode names.

	SetFileDoesNotExist (fileStreamPtr, kKeepUTF8CharName);
	UpdateFileNameInformation (fileStreamPtr, NULL);
                                
	SetType (fileStreamPtr, kTRANFileType);
	fileLength = (SInt32)256 + 3*sizeof (SInt16) + 
					gTransformationMatrix.numberChannels * sizeof (SInt16) +
						gTransformationMatrix.numberChannels * 2 * sizeof (double) +
								(SInt32)gTransformationMatrix.numberChannels * 
									gTransformationMatrix.numberChannels * sizeof (double);
							
	errCode = PutFile (fileStreamPtr, fileLength, IDS_FileIO61, gCreator);
	continueFlag = (errCode == noErr) && (filePathPtr[0] != 0);
		
	if (!continueFlag)
		errCode = -1;
		
	if (errCode == noErr)
				// Make the file an empty one. 								
		errCode = MSetSizeOfFile (fileStreamPtr, 0L, kErrorMessages);
		
	if (errCode == noErr)
		{
		MSetCursor (kWait);
				
				// Write the image file name.		
		/*
		sprintf ((char*)gTextString,
					"%s",
					gTransformationMatrix.imageFileName,
					gEndOfLine);
		*/
		count = 256;	
		/*
		errCode = MWriteData (fileStreamPtr,
										&count,
										gTextString,
										kErrorMessages);
		*/
		errCode = MWriteData (fileStreamPtr,
										&count,
										gTransformationMatrix.imageFileName,
										kErrorMessages);
		
		}	// end "if (errCode == noErr)" 
	
	if (errCode == noErr)
		{
				// Write the number of channels.												
		
		count = sizeof (SInt16);		
		errCode = MWriteData (fileStreamPtr, 
										&count, 
										&gTransformationMatrix.numberChannels,
										kErrorMessages);
		
		}	// end "if (errCode == noErr)" 
	
	if (errCode == noErr)
		{
				// Write the version number of the file in the low order 4 bits
				// and the Endianness of the data in the file in the upper 4 bits.
				//
				// Endianness: (in highest 4 bits)
				//		0 = Unknown
				//		1 = Big Endian
				//		2 = Little Endian
				//
				// Version: (in lowest 4 bits)
				//		0 = did not include the principal component means. They
				//			will be assumed to be 0 when this version is read back in.
				//		1 = allows for the principal component means to be saved
				//			to the file. 
				//		2 = allows for a transformation matrix of numberFeatures 
				//			by numberChannels, i.e. not restricted to numberChannels X
				//			numberChannels.	It also allows for the band grouping vector
				//			to be saved if this transformation was created by the
				//			the preprocessor part of feature extraction.
				//		3 = allows for code of endianness (8/2011)
	
		Byte endiannessAndVersionByte = 0x20 + 0x03;
		if (gBigEndianFlag)
			endiannessAndVersionByte = 0x10 + 0x03;
			
		count = 1;		
		errCode = MWriteData (fileStreamPtr, 
										&count, 
										&endiannessAndVersionByte,
										kErrorMessages);
		
		}	// end "if (errCode == noErr)"
	
	if (errCode == noErr)
		{
				// Write the code for how matrix was generated 		
				//		1 = principal components
				//		2 = feature extraction
	
		Byte createdByCode = (Byte)gTransformationMatrix.createdByCode;
		if (gTransformationMatrix.preProcBandGroupingHandle != NULL)
			createdByCode = 21;
		
		count = 1;	
		errCode = MWriteData (fileStreamPtr, 
										&count, 
										&createdByCode,
										kErrorMessages);
		
		}	// end "if (errCode == noErr)"  
	
	if (errCode == noErr)
		{
				// Write the number of features.												
		
		count = sizeof (SInt16);		
		errCode = MWriteData (fileStreamPtr, 
										&count, 
										&gTransformationMatrix.numberFeatures,
										kErrorMessages);
		
		}	// end "if (errCode == noErr)" 
	
	if (errCode == noErr)
		{
				// Write the list of channels that were used.							
		                                                   
		ptr = (HPtr)GetHandlePointer (gTransformationMatrix.eigenFeatureHandle,
												kLock);
		count = gTransformationMatrix.numberChannels * sizeof (SInt16);		
		errCode = MWriteData (fileStreamPtr, 
										&count, 
										ptr,
										kErrorMessages);
		CheckAndUnlockHandle (gTransformationMatrix.eigenFeatureHandle);
		
		}	// end "if (errCode == noErr)" 
	
	if (errCode == noErr && gTransformationMatrix.preProcBandGroupingHandle != NULL)
		{
				// Write the bandgrouping list if this transformation was
				// generated by the preprocessor part of feature extraction.			
		                                                   
		ptr = (HPtr)GetHandlePointer (gTransformationMatrix.preProcBandGroupingHandle,
												kLock);
		count = gTransformationMatrix.numberFeatures * sizeof (SInt16);		
		errCode = MWriteData (fileStreamPtr, 
									&count, 
									ptr,
									kErrorMessages);
		CheckAndUnlockHandle (gTransformationMatrix.preProcBandGroupingHandle);
		
		}	// end "if (errCode == noErr && ...)"
		
	if (errCode == noErr)
		{
				// Write the eigenvalues and principal component means.				
		                                                   
		ptr = (HPtr)GetHandlePointer (gTransformationMatrix.eigenValueHandle, kLock);
		    
		#if defined multispec_mac
			#ifdef _MC68881_ 
				count = 2 * gTransformationMatrix.numberFeatures * sizeof (double);		
				errCode = MWriteData (fileStreamPtr, 
											&count, 
											ptr,
											kErrorMessages);
			#else	// not _MC68881_ 
				#if TARGET_RT_MAC_MACHO	
					count = 2 * gTransformationMatrix.numberFeatures * sizeof (double);		
					errCode = MWriteData (fileStreamPtr, 
												&count, 
												ptr,
												kErrorMessages);
				#else	// !TARGET_RT_MAC_MACHO	
					fileLength = 2 * gTransformationMatrix.numberFeatures;
					doublePtr = (double*)ptr;
					count = k68881Bytes;
					
					for (index=0; index<fileLength; index++)
						{
						nonx96tox96 (doublePtr, &extendedValue);
						
						errCode = MWriteData (fileStreamPtr, 
												&count, 
												&extendedValue,
												kErrorMessages);
							
						if (errCode != noErr)
							break;
								
						doublePtr++;
								
						}	// end "for (index=0; ..." 
				#endif	// TARGET_RT_MAC_MACHO, else...
			#endif	// _MC68881_ 
		#endif	// defined multispec_mac 
		    
		#if defined multispec_win 
			count = 2 * gTransformationMatrix.numberFeatures * sizeof (double);		
			errCode = MWriteData (fileStreamPtr, 
										&count, 
										ptr,
										kErrorMessages); 
		#endif	// defined multispec_win
					
		CheckAndUnlockHandle (gTransformationMatrix.eigenValueHandle);
		
		}	// end "if (errCode == noErr)" 
		
	if (errCode == noErr)
		{
		HDoublePtr		eigenVectorPtr,
							localEigenVectorPtr;
		
				// Write the eigenvectors.             
		eigenVectorPtr = (HDoublePtr)GetHandlePointer (
															gTransformationMatrix.eigenVectorHandle,
															kLock);
			                      
		count = sizeof (double);
		#if defined multispec_mac
			#ifdef _MC68881_ 	
				count = k68881Bytes;				
			#else 		// not _MC68881_ 
				#ifndef TARGET_RT_MAC_MACHO
					count = k68881Bytes;					
				#endif	// !TARGET_RT_MAC_MACHO, else...				
			#endif	// _MC68881_  
		#endif	// defined multispec_mac 
				    
				// The eigenvector matrix needs to be transposed when written out
				// to agree with the format that was initially used.
													
		for (i=0; i<(UInt32)gTransformationMatrix.numberChannels; i++)
			{
			localEigenVectorPtr = eigenVectorPtr + i;
			
			for (j=0; j<(UInt32)gTransformationMatrix.numberFeatures; j++)
				{           
				#if defined multispec_mac || defined multispec_mac_swift
					#ifdef _MC68881_
						extendedValue = *localEigenVectorPtr;						
					#else 		// not _MC68881_ 
						#if TARGET_RT_MAC_MACHO	
							extendedValue = *localEigenVectorPtr;			
						#else	// !TARGET_RT_MAC_MACHO	
							nonx96tox96 (localEigenVectorPtr, &extendedValue);							
						#endif	// TARGET_RT_MAC_MACHO, else...
					#endif	// _MC68881_  
				#endif	// defined multispec_mac 
				    
				#if defined multispec_win  				                   
					extendedValue = *localEigenVectorPtr;				
				#endif	// defined multispec_win
							
				errCode = MWriteData (fileStreamPtr, 
												&count,
												&extendedValue,
												kErrorMessages);
							
				if (errCode != noErr)
					break;
				
				localEigenVectorPtr += gTransformationMatrix.numberChannels;
					
				}	// end "for (j=0; j<...numberFeatures; j++)"
							
			if (errCode != noErr)
				break;
				
			}	// end "for (i=0; i<...numberChannels; i++)"
		
		CheckAndUnlockHandle (gTransformationMatrix.eigenVectorHandle);
		
		}	// end "if (errCode == noErr)" 
	
	if (errCode == noErr)
		{	
				// Force text selection to start from end of present text.			
						
		ForceTextToEnd (); 
		
		FileStringPtr fileNamePtr = 
						(FileStringPtr)GetFileNameCPointerFromFileStream (fileStreamPtr);
			
		continueFlag = ListSpecifiedStringNumber (kFileIOStrID,
																IDS_FileIO68, 
																NULL, 
																gOutputForce1Code,
																(char*)fileNamePtr,
																continueFlag,
																kUTF8CharString);
			
				// Scroll output window to the selection and adjust the scroll bar.																		
		
		UpdateOutputWScrolls (gOutputWindow, 1, kDisplayMessage);
						
		}	// end "if (errCode == noErr)" 

			// Close the file and flush the volume, to make sure disk is 			
			// correct.																				

	CloseFile (fileStreamPtr);
				
	MInitCursor ();
	
}	// end "WriteTransformationFile"
