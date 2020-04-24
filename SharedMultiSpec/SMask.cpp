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
//	File:						SMask.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			04/16/2020
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	The purpose of the routines in this file is to provide utility 
//								mask functions in MultiSpec.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"   

#if defined multispec_wx
	#include "SFileStream_class.h"
	#include "SImageWindow_class.h"
#endif
  
#if defined multispec_win
	#include "SImageWindow_class.h"
#endif	// defined multispec_win



			// Prototype descriptions for routines in this file that are only		
			// called by routines in this file.

SInt16 CheckCurrentMaskFields (
				SInt16								maskSetCode,
				MaskInfoPtr							maskInfoPtr,
				HUInt32Ptr							maskValueCountVector,
				HUInt16Ptr							maskValueToFieldPtr,
				UInt32								maxMaskValue);

Boolean CheckMaskFileInfo (
				Handle								fileInfoHandle,
				SInt16*								errCodePtr);

Boolean LoadNewMaskFields (
				SInt16								maskSetCode,
				MaskInfoPtr							maskInfoPtr,
				HUInt32Ptr							maskValueCountVector,
				HUInt16Ptr							maskValueToFieldPtr,
				UInt32								maxMaskValue,
				Handle								maskWindowInfoHandle);

Boolean DetermineIfClassesSameAsProjectClassNames (
				HUCharPtr							classNamePtr,
				UInt32								numberClasses);

Boolean UserLocateProjectMaskImage (
				Handle								fileInfoHandle,
				SInt16								promptStringNumber);

	

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 CheckCurrentMaskFields
//
//	Software purpose:	The purpose of this routine is to check the mask information
//							that is loaded into the current fields. If the mask information
//							in the file is different then the field is set to be not 
//							up-to-date.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None
//
// Called By:			LoadMask in SMask.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/08/1999
//	Revised By:			Larry L. Biehl			Date: 01/08/1999

SInt16 CheckCurrentMaskFields (
				SInt16								maskSetCode,
				MaskInfoPtr							maskInfoPtr,
				HUInt32Ptr							maskValueCountVector,
				HUInt16Ptr							maskValueToFieldPtr,
				UInt32								maxMaskValue)

{ 
	HPClassNamesPtr					classNamesPtr;
	HPFieldIdentifiersPtr			fieldIdentPtr;
	
	SInt32								fieldNumber;
	
	UInt32								classNumber,
											classStorage,
											numberClasses;
	
	SInt16								fieldType,
											returnCode;
											
	UInt16								maskValue;
											
	Boolean								maskStatsChangedFlag;
											
	
			// Initialize local variables.
	
	returnCode = 0;		
	maskStatsChangedFlag = FALSE;
	
	fieldType = 0;
	if (maskSetCode == kTrainingMaskSet)
		fieldType = kTrainingType;
		
	else if (maskSetCode == kTestMaskSet)
		fieldType = kTestingType;
		
	if (fieldType == 0)
																							return (FALSE);											
			
	numberClasses = gProjectInfoPtr->numberStatisticsClasses;
	
			// Continue only if number of classes is one or more.						
	
	if (numberClasses > 0)
		{
				// Find all mask fields and check if the number of samples in
				// the input mask agrees with what is currently in the project
				// file. If not indicate that the statistics, if any, are not
				// up-to-date. Also fill in the mask to field vector.								
		
		classNamesPtr = gProjectInfoPtr->classNamesPtr;
		
		for (classNumber=0; classNumber<numberClasses; classNumber++)
			{					
					// Get the class storage number.													
								
			classStorage = gProjectInfoPtr->storageClass[classNumber];
			fieldNumber = classNamesPtr[classStorage].firstFieldNumber;
			
			while (fieldNumber != -1)
				{
				fieldIdentPtr = &gProjectInfoPtr->fieldIdentPtr[fieldNumber];
			
						// Make certain that field is training field.						
						
				if ((fieldIdentPtr->fieldType == fieldType) &&
														(fieldIdentPtr->pointType == kMaskType))
					{
					maskValue = fieldIdentPtr->maskValue;
					
					if (maskValue <= maxMaskValue)
						{
						if (fieldIdentPtr->numberPixels !=
															(SInt64)maskValueCountVector[maskValue])
							{
							fieldIdentPtr->numberPixels = maskValueCountVector[maskValue];
							
							if (fieldType == kTrainingType)
								{
								fieldIdentPtr->statsUpToDate = FALSE;
								maskStatsChangedFlag = TRUE;
								
								}	// end "if (fieldType == kTrainingType)"
							
							}	// end "if (fieldIdentPtr->numberPixels != ..."
						
						maskValueToFieldPtr[maskValue] = (UInt16)fieldNumber;
						
						}	// end "if (maskValue <= maxMaskValue)"
						
					else	// maskValue > maxMaskValue
						{
						returnCode += 1;
						break;
						
						}	// end "else maskValue > maxMaskValue"
									
					}	// end "if (fieldIdentPtrr->field..." 
					
				fieldNumber = fieldIdentPtr->nextField;
				
				}	// end "while (fieldNumber != -1)" 
				
			if (returnCode != 0)
				break;
			
			}	// end "for (classNumber=0; ... 
			
		}	// end "if (numberClasses > 0)"
		
	if (maskStatsChangedFlag && returnCode == 0)
		{
		returnCode = 2;
		
		}	// end "if (maskStatsChangedFlag && returnCode == 0)"
		
	return (returnCode);
	
}	// end "CheckCurrentMaskFields"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		CheckMaskFileInfo
//
//	Software purpose:	The purpose of this routine is to close and unlock
//							the file that is described by fileInfo.
//		
//	Parameters in:		Address to MFileInfo structure for file to be closed.
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 01/08/1999
//	Revised By:			Larry L. Biehl			Date: 04/28/2017

Boolean CheckMaskFileInfo (
				Handle								fileInfoHandle,
				SInt16*								errCodePtr)

{
	SInt64								fileSize;
	CMFileStream*						fileStreamPtr;
	FileInfoPtr							fileInfoPtr;
	
	Boolean								fileInfoLoadedFlag,
											returnFlag,
											sizeOKFlag;
											
	
			// Initialize local variables.
			
	returnFlag = FALSE;
	
			// Determine the file format and load the header 				
			// information.															
			
	fileInfoLoadedFlag = LoadImageInformation (NULL, fileInfoHandle, FALSE, NULL, 0);

			// If the file information was loaded, add to the active 	
			// image window "file" or create the image window for the 	
			// file.																		
			
   if (fileInfoLoadedFlag)
   	{
		fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
											
				// Make certain that the file is of the correct size.	
				
		sizeOKFlag = FALSE;	
		
		fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
		*errCodePtr = GetSizeOfFile (fileStreamPtr, &fileSize);
		
				// Do not compare with expected size of the file since the file
				// may be compressed.
		//if (*errCodePtr == noErr && fileSize >= GetSizeOfImage (fileInfoPtr))
		if (*errCodePtr == noErr)
			sizeOKFlag = TRUE;
				
				// Beep to alert user that the size of the file does not match
				// the image parameters.			
		
		if (!sizeOKFlag) 
			{ 
			SysBeep (10);
			CloseFile (fileStreamPtr);
		
			}	// end "if (!sizeOKFlag)"
			
		else	// sizeOKFlag
			returnFlag = TRUE;
			
		}	// end "if (fileInfoLoadedFlag)"
		
	return (returnFlag);
	
}	// end "CheckMaskFileInfo" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		CloseFile
//
//	Software purpose:	The purpose of this routine is to close and unlock
//							the file that is described by fileInfo.
//		
//	Parameters in:		Address to MFileInfo structure for file to be closed.
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 12/14/1998
//	Revised By:			Larry L. Biehl			Date: 12/14/1998

void CloseFile (
				MaskInfoPtr							maskInfoPtr)

{
	#if defined multispec_mac 	
		CloseFile (maskInfoPtr->fileStreamHandle);
	#endif	// defined multispec_mac 

              
#if defined multispec_win || defined multispec_wx	
		CloseFile (maskInfoPtr->fileStreamPtr);
	#endif	// defined multispec_win || lin
	
}	// end "CloseFile" 


/*
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		UInt32 ConvertFieldNumberToMaskValue
//
//	Software purpose:	The purpose of this routine is to convert the input field
//							number to the mask value that represents that field number
//							using the mask value to field vector which is stored in the
//							input mask information structure.
//								This routine is not used now.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:	
// 
// Called By:			GetFieldBoundary in SProjectUtilities.cpp
//							IsProjectData in SStatisticsEnhancement.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/21/1998
//	Revised By:			Larry L. Biehl			Date: 12/21/1998	

UInt32 ConvertFieldNumberToMaskValue (
				MaskInfoPtr							maskInfoPtr,
				SInt16								fieldNumber)

{
	HUInt16Ptr							maskValueToFieldPtr;
	
	UInt32								index,
											maxMaskValue;
	
	
	maxMaskValue = maskInfoPtr->maxMaskValue;
																					
	maskValueToFieldPtr = (HUInt16Ptr)GetHandlePointer (
														maskInfoPtr->maskValueToFieldHandle);
	
	for (index=1; index<=maxMaskValue; index++)
		{	
		if (fieldNumber == maskValueToFieldPtr[index])
																							return (index);
																							
		}	// end "for (index=1; index<=maxMaskValue; index++)"
		
	return (0);
	
}	// end "ConvertFieldNumberToMaskValue" 
*/

	

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CloseMaskStructure
//
//	Software purpose:	The purpose of this routine is to initialize the input mask
//							structure.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/07/1999
//	Revised By:			Larry L. Biehl			Date: 01/07/1999

void CloseMaskStructure (
				MaskInfoPtr							maskInfoPtr)

{
	if (maskInfoPtr != NULL)
		{				
		CloseFile (maskInfoPtr->fileStreamHandle);
		UnlockAndDispose (maskInfoPtr->fileStreamHandle); 
		
		#if defined multispec_win || defined multispec_wx
			if (maskInfoPtr->fileStreamPtr != NULL)
				delete maskInfoPtr->fileStreamPtr;				
		#endif	// defined multispec_win || lin
			
		UnlockAndDispose (maskInfoPtr->maskHandle);
		UnlockAndDispose (maskInfoPtr->maskValueToFieldHandle); 
									
		InitializeMaskStructure (maskInfoPtr);
		
		}	// end "if (maskInfoPtr != NULL)"
	
}	// end "CloseMaskStructure" 


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean DetermineIfMaskDataInLine
//
//	Software purpose:	The purpose of this routine is to reduce the input line of data
//							to just that requested by the mask information.
//
//	Parameters in:				
//
//	Parameters out:	Number of mask samples included in the buffer
//
//	Value Returned:	Error code for file operations. 
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 12/21/1998
//	Revised By:			Larry L. Biehl			Date: 12/21/1998

Boolean DetermineIfMaskDataInLine (
				UInt16*								maskBufferPtr,
				UInt32								maskColumnStart,
				UInt32								numberSamples,
				UInt32								columnInterval,
				UInt32								maskValueRequest)

{ 
	UInt32								sample;
	
	
			// Return if no mask data in line. Don't have to check every pixel.
			
	if (*maskBufferPtr == 0)
																							return (FALSE);
																							
			// At least one mask value exists in this line. Determine if any
			// of the requested mask values exist.
	
	maskBufferPtr += maskColumnStart;
	
	if (maskValueRequest == 0)
		{
				// Any mask value other than 0 will be used.
														
		for (sample=0; sample<numberSamples; sample+=columnInterval)
			{
			if (*maskBufferPtr != 0)
																							return (TRUE);
				
			maskBufferPtr += columnInterval;
				
			}	// end "for (sample=0; sample<numberSamples; sample+=columnInterval)"
			
		}	// end "if (maskValueRequest == 0)"
		
	else	// maskValueRequest != 0
		{
				// Only a specific mask value will be used.
														
		for (sample=0; sample<numberSamples; sample+=columnInterval)
			{
			if (*maskBufferPtr == maskValueRequest)
																							return (TRUE);
				
			maskBufferPtr += columnInterval;
				
			}	// end "for (sample=0; sample<numberSamples; sample+=columnInterval)"
			
		}	// end "else maskValueRequest != 0"
		
	return (FALSE);
	      
}	// end "DetermineIfMaskDataInLine"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean DetermineIfClassesSameAsProjectClassNames
//
//	Software purpose:	This routine allows the user to locate the project
//							base image file.
//
//	Parameters in:		classNamePtr - pointer to the thematic image class names.
//							numberClasses - number of thematic image classes.
//
//	Parameters out:	None.		
//
// Value Returned:	FALSE if none of the thematic class names are the same as any
//								of the project class names.
//							TRUE if at least one of the thematic class names is the same
//								as one of the project class names.
//
// Called By:			LoadNewMaskFields in SMask.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/15/2000
//	Revised By:			Larry L. Biehl			Date: 02/15/2000

Boolean DetermineIfClassesSameAsProjectClassNames (
				HUCharPtr							classNamePtr, 
				UInt32								numberClasses)

{ 
	UInt32								index;

	Boolean								nameIsSameFlag = FALSE;
	

	for (index=0; index<numberClasses; index++)
		{
		if (CheckForDuplicateClassName (-1, (Str255*)classNamePtr) > 0)
			{
			nameIsSameFlag = TRUE;
			break;
			
			}	// end "if (CheckForDuplicateClassName (..."
			
		classNamePtr += 32;
		
		}	// end "for (index=0; index<numberClasses; index++)"
		
	return (nameIsSameFlag);

}	// end "DetermineIfClassesSameAsProjectClassNames"

	

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		UInt32 GetFirstMaskLine
//
//	Software purpose:	The purpose of this routine is to find the first line that
//							contains the specified mask value.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None
//
// Called By:			GetFieldBoundary in SProjectUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/28/1998
//	Revised By:			Larry L. Biehl			Date: 04/28/2017

UInt32 GetFirstMaskLine (
				MaskInfoPtr							maskInfoPtr,
				UInt16								maskValueRequest,
				UInt32								maskLineStart,
				UInt32								maskColumnStart,
				UInt32								lineStart,
				UInt32								lineEnd,
				UInt32								lineInterval,
				UInt32								columnStart,
				UInt32								columnEnd,
				UInt32								columnInterval)

{
	HUInt16Ptr							maskBufferPtr;
	
	UInt32								column,
											line,
											numberMaskColumns,
											maskExistsInLine;
	
	Boolean								continueFlag;
	
	
	if (maskInfoPtr == NULL)
																							return (0);
																							
	if (lineInterval <= 0 || columnInterval <= 0)
																							return (0);
	
			// Initialize local variables.
			
	maskExistsInLine = 0;
				
	maskBufferPtr = (HUInt16Ptr)GetHandlePointer (maskInfoPtr->maskHandle);
			
	continueFlag = (maskBufferPtr != NULL);
		
	if (continueFlag)
		{
		numberMaskColumns = maskInfoPtr->numberColumns + 1;
		
				// Position mask buffer to start at same relative location that lineStart
				// represents. 'lineStart' represents the line start in the image file.
		
		maskBufferPtr += (maskLineStart - 1) * numberMaskColumns;
	
				// Change the columnStart and columnEnd parameters to represent locations
				// within the mask buffer not the image file.
				
		UInt32 numberColumns = columnEnd - columnStart + 1;
		columnStart = maskColumnStart;
		columnEnd = columnStart + numberColumns - 1;
		
				// Now make sure that the number of mask columns represents the line
				// interval that is being used.
				
		numberMaskColumns *= lineInterval;
									
		for (line=lineStart; line<=lineEnd; line+=lineInterval)
			{
			for (column=columnStart; 
						column<=columnEnd; 
								column+=columnInterval)
				{
				if (maskBufferPtr[column] == maskValueRequest)
					{
					maskExistsInLine = line;
					break;
					
					}	// end "if (maskBufferPtr[column] == maskValueRequest)"
					
				}	// end "for (column=localColumnStart; column<=columnEnd; ..."
					
			maskBufferPtr += numberMaskColumns;
			
			if (maskExistsInLine > 0)
				break;
				
			}	// end "for (line=lineStart; line<=lineEnd; line+=localLineInterval)"

		}	// end "if (continueFlag)"
		
	CheckAndUnlockHandle (maskInfoPtr->maskHandle);
		
	return (maskExistsInLine);
	
}	// end "GetFirstMaskLine"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetMaskArea
//
//	Software purpose:	The purpose of this routine is to compare the requested image
//							input area to the available mask area and return the
//							areas of each that overlap. The image and mask start lines
//							and columns are taken into account as are the image line and
//							column intervals.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:	
// 
// Called By:			LoadMask in SMask.cpp
//							GetFieldBoundary in SProjectUtilities.cpp
//							UpdateProjectMaskStats in SProjectComputeStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/13/1998
//	Revised By:			Larry L. Biehl			Date: 01/15/1999	

Boolean GetMaskArea (
				MaskInfoPtr							maskInfoPtr,
				SInt16								maskSetCode,
				ProjectInfoPtr						projectInfoPtr,
				WindowInfoPtr						windowInfoPtr,
				UInt32								imageStartLine,
				UInt32								imageStartColumn,
				UInt32								imageLineInterval, 
				UInt32								imageColumnInterval, 
				UInt32*								imageLineStartPtr, 
				UInt32*								imageLineEndPtr, 
				UInt32*								imageColumnStartPtr, 
				UInt32*								imageColumnEndPtr, 
				UInt32*								maskLineStartPtr, 
				UInt32*								maskColumnStartPtr)

{
	ldiv_t								offset;
	
	SInt32								imageLineStart, 
											imageLineEnd, 
											imageColumnStart, 
											imageColumnEnd, 
											initialImageColumnStart,
											initialImageLineStart,
											maskColumnEnd,
											maskColumnStart,
											maskLineEnd,
											maskLineStart;
											
	
	if (maskInfoPtr == NULL)			
		maskInfoPtr = GetMaskInfoPointer (maskSetCode, projectInfoPtr, windowInfoPtr);
		
	if (maskInfoPtr == NULL)
																							return (FALSE);
																							
	if (imageLineInterval == 0 || imageColumnInterval == 0)
																							return (FALSE);
	
			// Adjust the mask lines to reflect the same area as that relative to
			// the image area in case the mask and image areas start at different
			// locations.
			
	maskLineStart = 1 +  maskInfoPtr->startLine - imageStartLine;
	maskLineEnd = maskInfoPtr->numberLines + 
												maskInfoPtr->startLine - imageStartLine;
			
	maskColumnStart = 1 + maskInfoPtr->startColumn - imageStartColumn;
	maskColumnEnd = maskInfoPtr->numberColumns + 
												maskInfoPtr->startColumn - imageStartColumn;
	
			// Save the requested start line and column.
			
	initialImageLineStart = (SInt32)*imageLineStartPtr;
	initialImageColumnStart = (SInt32)*imageColumnStartPtr;
	
	imageLineStart = (SInt32)*imageLineStartPtr;
	imageLineEnd = (SInt32)*imageLineEndPtr;
	imageColumnStart = (SInt32)*imageColumnStartPtr;
	imageColumnEnd = (SInt32)*imageColumnEndPtr;
	
			// Adjust the selected image area to represent the same area as the
			// mask area that is available.

	imageLineStart = MAX (imageLineStart, maskLineStart);
	
	if (imageLineEnd > 0)
		imageLineEnd = MIN (imageLineEnd, maskLineEnd);
		
	else	// *imageLineEndPtr = 0
		imageLineEnd = maskLineEnd;
	
	imageColumnStart = MAX (imageColumnStart, maskColumnStart);
	
	if (imageColumnEnd > 0)
		imageColumnEnd = MIN (imageColumnEnd, maskColumnEnd);
		
	else	// *imageLineEndPtr = 0
		imageColumnEnd = maskColumnEnd;
	
			// Take into account the interval requests.
			
	offset = ldiv (imageLineStart-initialImageLineStart, imageLineInterval);
	imageLineStart += offset.rem;
			
	offset = ldiv (imageColumnStart-initialImageColumnStart, imageColumnInterval);
	imageColumnStart += offset.rem;
			
			// Adjust the mask area to represent the image area to be used.

	maskLineStart = MAX (imageLineStart, maskLineStart);
	maskLineEnd = MIN (imageLineEnd, maskLineEnd);
	
	maskColumnStart = MAX (imageColumnStart, maskColumnStart);
	maskColumnEnd = MIN (imageColumnEnd, maskColumnEnd);
	
	if (maskLineStart > maskLineEnd || maskColumnStart > maskColumnEnd)
																						return (FALSE);
		
	else	// maskLineStart <= maskLineEnd && ... 
		{
		*imageLineStartPtr = (UInt32)imageLineStart;
		*imageLineEndPtr = (UInt32)imageLineEnd;
		*imageColumnStartPtr = (UInt32)imageColumnStart;
		*imageColumnEndPtr = (UInt32)imageColumnEnd;
			
				// Return the mask line and column start values relative to the
				// mask area.
				
		*maskLineStartPtr = 
					(UInt32)maskLineStart - maskInfoPtr->startLine + imageStartLine;
		*maskColumnStartPtr = 
					(UInt32)maskColumnStart - maskInfoPtr->startColumn + imageStartColumn;
	
		return (TRUE);
		
		}	// end "else maskLineStart <= maskLineEnd && ..."

}	// end "GetMaskArea"

	

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Handle GetMaskFile
//
//	Software purpose:	This routine allows the user to select the image file
//							that contains the specified mask.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/08/1998
//	Revised By:			Larry L. Biehl			Date: 04/28/2017

Handle GetMaskFile (
				Handle								inputFileInfoHandle,
				SInt16								promptString)

{
	CMFileStream*						fileStreamPtr;
	FileInfoPtr							fileInfoPtr;
	
	Handle								fileInfoHandle;
	
	SInt16								errCode,
											numberFileTypes;
	
	Boolean								continueFlag,
											doneFlag,
											fileInfoLoaded;
	
	
			// Initialize local variables.
			
	fileInfoHandle = NULL;
	fileStreamPtr = NULL;
	continueFlag = TRUE;
	doneFlag = FALSE;
	
			// Get a handle to a block of memory to be used for file information 
			// for mask image file.								
	
	if (continueFlag)
		fileInfoHandle = MNewHandle (sizeof (MFileInfo));
		
	if (fileInfoHandle != NULL)
		{
				// If the option key was down when the 'Open Image' menu item		
				// was selected then include all files in the file list.  			
				// Otherwise just show the selected file types.	
				
		numberFileTypes = gNumberFileTypes;
		
		#if defined multispec_mac
			if (gEventRecord.modifiers & optionKey)
				numberFileTypes = -1;
		#endif	// defined multispec_mac
		
		do
			{
					// Initialize local variables.
		
			fileInfoLoaded = FALSE;
			continueFlag = FALSE;
		
					// Initialize the variables and handles in the structure.		
				
			InitializeFileInfoStructure (fileInfoHandle, kNotPointer);
			
			fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
			
			fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
		
					// Indicate that the image type is to be determined from the 	
					// header part of the file.												
				
			gGetFileImageType = 0;
			
			errCode = GetFile (fileStreamPtr, 
										numberFileTypes, 
										gListTypes, 
										NULL,
										NULL,
										NULL, 
										promptString);
										
			continueFlag = ((errCode == noErr) & FileExists (fileStreamPtr));
				
			if (continueFlag)
				{
						// Check if header and file size make sense.
						
				doneFlag = CheckMaskFileInfo (fileInfoHandle, &errCode);
				continueFlag = (errCode == noErr);
				
				}	// end "if (continueFlag)" 
			
			}		while (continueFlag & !doneFlag);

		}	// end "if (fileInfoHandle != NULL)"

	else		
		continueFlag = FALSE;
		
	if (continueFlag)
		DisposeFileInfoHandle (inputFileInfoHandle);
 	    		
	else	// !continueFlag
		{
		DisposeFileInfoHandle (fileInfoHandle);
		fileInfoHandle = inputFileInfoHandle;
		
		}	// end "else !continueFlag"
  	
	return (fileInfoHandle);
	
}	// end "GetMaskFile"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		FileStringPtr GetMaskFileNamePPointer
//
//	Software purpose:	The purpose of this routine is to obtain the pascal file name
//							pointer for the input mask information structure.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:  	pointer to the support file stream pointer.
//							NULL if not available.
//
// Called By:			StatisticsDialog in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date:	12/08/1998
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

FileStringPtr GetMaskFileNamePPointer (
				MaskInfo*							maskInfoPtr,
				SignedByte*							handleStatusPtr)

{	
	CMFileStream*						maskFileStreamPtr;
	
	
	maskFileStreamPtr = GetMaskFileStreamPointer (maskInfoPtr, handleStatusPtr);
		
	return ((FileStringPtr)GetFileNamePPointerFromFileStream (maskFileStreamPtr));
												
}	// end "GetMaskFileNamePPointer"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		CMFileStream* GetMaskFileStreamPointer
//
//	Software purpose:	The purpose of this routine is to obtain the file stream
//							pointer for the input mask file.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:  	pointer to the support file stream pointer.
//							NULL if not available.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	01/07/1999
//	Revised By:			Larry L. Biehl			Date: 01/07/1999

CMFileStream* GetMaskFileStreamPointer (
				MaskInfoPtr							maskInfoPtr,
				SignedByte*							handleStatusPtr)

{	
	CMFileStream*						maskFileStreamPtr = NULL;
	
												
	*handleStatusPtr = -1;
												
	if (maskInfoPtr != NULL)
		{	            
		#if defined multispec_mac
			maskFileStreamPtr = (CMFileStream*)GetHandleStatusAndPointer (
								maskInfoPtr->fileStreamHandle, 
								handleStatusPtr);	
		#endif	// defined multispec_mac
	              
		#if defined multispec_win || defined multispec_wx
			maskFileStreamPtr = maskInfoPtr->fileStreamPtr;	 
		#endif	// defined multispec_win || lin
														
		}	// end "if (maskInfoPtr != NULL)"                    
		
	return (maskFileStreamPtr);
												
}	// end "GetMaskFileStreamPointer"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		MaskInfoPtr GetMaskInfoPointer
//
//	Software purpose:	The purpose of this routine is to return a pointer to the mask
//							structure for the input mask code and window information 
//							structure.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:	
// 
// Called By:			GetMaskArea in SMask.cpp
//							SetUpMaskAreaDescriptionParameters in SMask.cpp
//							GetFieldBoundary in SProjectUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/30/1998
//	Revised By:			Larry L. Biehl			Date: 11/30/1998	

MaskInfoPtr GetMaskInfoPointer (
				SInt16								maskSetCode,
				ProjectInfoPtr						projectInfoPtr,
				WindowInfoPtr						windowInfoPtr)

{
	MaskInfoPtr							maskInfoPtr;
											
											
			// Intialize local variables.
			
	maskInfoPtr = NULL;
		
			// Get the mask set to use.
			
	switch (maskSetCode)
		{
		case kTrainingMaskSet:
			if (projectInfoPtr != NULL)
				maskInfoPtr = &projectInfoPtr->trainingMask;
   		break;
   		
		case kTestMaskSet:
			if (projectInfoPtr != NULL)
				maskInfoPtr = &projectInfoPtr->testMask;
   		break;
   		
		case kMaskSet:
			if (windowInfoPtr != NULL)
				maskInfoPtr = &windowInfoPtr->mask;
   		break;
		
		}	// end "switch (maskSetCode)"
		
	return (maskInfoPtr);
	
}	// end "GetMaskInfoPointer"

	

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		UInt32 GetNumberPixelsInMaskArea
//
//	Software purpose:	The purpose of this routine is to compute the number of 
//							'maskValueRequest'pixels in the mask area. 
//							Note that the first column skip is not used for mask areas as
//							it is for rectangular and polygon areas; it
//							just does not seem that this capability will add much.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None
//
// Called By:			GetTotalNumberOfPixels in SUtility.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/22/1998
//	Revised By:			Larry L. Biehl			Date: 04/28/2017

SInt64 GetNumberPixelsInMaskArea (
				MaskInfoPtr							maskInfoPtr,
				UInt16								maskValueRequest,
				UInt32								maskLineStart,
				UInt32								maskColumnStart,
				SInt32								lineStart,
				SInt32								lineEnd,
				SInt32								lineInterval,
				SInt32								columnStart,
				SInt32								columnEnd,
				SInt32								columnInterval,
				SInt16								algorithmCode)

{
	SInt64								pixelCount;
	
	HUInt16Ptr							maskBufferPtr;
	
	UInt32								column,
											firstColumnSkip,
											line,
											localColumnInterval,
											localColumnStart,
											localLineInterval,
											numberMaskColumns;
	
	Boolean								continueFlag,
											singlePassClusterFlag;
	
	
	if (maskInfoPtr == NULL)
																							return (0);
	
			// Initialize local variables.
			
	pixelCount = 0;
				
	maskBufferPtr = (HUInt16Ptr)GetHandlePointer (maskInfoPtr->maskHandle);
			
	continueFlag = (maskBufferPtr != NULL);
		
	if (continueFlag)
		{
		numberMaskColumns = maskInfoPtr->numberColumns + 1;
		
				// Position mask buffer to start at same relative location that lineStart
				// represents. 'lineStart' represents the line start in the image file.
		
		maskBufferPtr += (maskLineStart - 1) * numberMaskColumns;
		
				// Set the number of mask columns to represent the index to skip when 
				// going from one line to the next allowing for lines to be skipped.
		
		if (!(algorithmCode & kSPClusterCase))		
			numberMaskColumns *= lineInterval;
	
				// Change the columnStart and columnEnd parameters to represent locations
				// within the mask buffer not the image file.
				
		UInt32 numberColumns = columnEnd - columnStart + 1;
		columnStart = maskColumnStart;
		columnEnd = columnStart + numberColumns - 1;
		
		localColumnInterval = columnInterval;
		localLineInterval = lineInterval;
		singlePassClusterFlag = FALSE;
		if (algorithmCode & kSPClusterCase)
			{
					// Adjust for one-pass cluster.
					
			localLineInterval = 1;
			localColumnInterval = 1;
			
			singlePassClusterFlag = TRUE;
			
			}	// end "if (algorithmCode & kSPClusterCase)"
			
		firstColumnSkip = 0;
									
		for (line=lineStart; line<=(UInt32)lineEnd; line+=localLineInterval)
			{
			localColumnStart = columnStart + firstColumnSkip;
				
			for (column=localColumnStart;
						column<=(UInt32)columnEnd;
								column+=localColumnInterval)
				{
				if (maskBufferPtr[column] == maskValueRequest)
					pixelCount++;
					
				}	// end "for (column=localColumnStart; column<=columnEnd; ..."
  				
			maskBufferPtr += numberMaskColumns;
				
			if (singlePassClusterFlag && pixelCount > 0)
				{
					// The first line with requested data has been found. Be sure that
					// the column interval is set for the 'rest of the lines'.
					
				localLineInterval = lineInterval; 
				localColumnInterval = columnInterval;	
				numberMaskColumns *= lineInterval;
				
				singlePassClusterFlag = FALSE;
				
				}	// end "if (singlePassClusterFlag && pixelCount > 0)"
				
			}	// end "for (line=lineStart; line<=lineEnd; line+=localLineInterval)"

		}	// end "if (continueFlag)"
		
	CheckAndUnlockHandle (maskInfoPtr->maskHandle);
		
	return (pixelCount);
	
}	// end "GetNumberPixelsInMaskArea"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetSpecifiedMaskFile
//
//	Software purpose:	The purpose of this routine is to locate a file with the input 
//							file name in the same folder as the project file is in. If the 
//							file cannot be located then a dialog box is presented to the 
//							user to let him/her locate the file to use for the mask.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:			ReadProjectFile in SProjectFileIO.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/08/1999
//	Revised By:			Larry L. Biehl			Date: 04/16/2020

Boolean GetSpecifiedMaskFile (
				SInt16								maskSetCode,
				UCharPtr 							maskFileNamePtr,
				MaskInfoPtr							maskInfoPtr, 
				Boolean								userPrompt)

{
	FileStringPtr						fileNamePtr,
											filePathPtr;
											
	CMFileStream*						fileStreamPtr;
	FileInfoPtr							fileInfoPtr;
	
	Handle								maskFileInfoHandle;
											
	SInt16								errCode,
											promptStringNumber,
											returnCode;
	
	Boolean								fileFoundFlag;
	

			// Initialize local variables														
			
	fileFoundFlag = FALSE;
	
			// Get a handle to a block of memory to be used for file information 
			// for mask image file.								
	
	maskFileInfoHandle = InitializeFileInfoStructure (NULL, kNotPointer);	
		
				
	fileInfoPtr = (FileInfoPtr)GetHandlePointer (maskFileInfoHandle);
	
	if (fileInfoPtr != NULL)
		{		
				// For now assume that the mask image file is in the same
				// directory as the project file. Therefore copy filestream
				// variables from the project filestream structure to the
				// mask file stream structure.
			
		fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
		
		CMFileStream* projectFileStreamPtr = 
											GetFileStreamPointer (gProjectInfoPtr);
	
				// Initialize variables in the structure.	
			
		InitializeFileStream (fileStreamPtr, projectFileStreamPtr);		

		fileNamePtr = (FileStringPtr)GetFileNameCPointerFromFileStream (fileStreamPtr);
		memcpy (fileNamePtr, (CharPtr)&maskFileNamePtr[1], maskFileNamePtr[0]+1);
		
		filePathPtr = (FileStringPtr)GetFilePathPPointerFromFileStream (fileStreamPtr);
		int length = strlen ((char*)&filePathPtr[2]);
		SetFileStringLength (filePathPtr, length);
		
				// Check if image file is in the same volume as the project			
				// file. If it is, open a path to the file and return the			
				// handle to the file information.											
				
		errCode = OpenFileReadOnly (fileStreamPtr, 
												kResolveAliasChains, 
												kLockFile, 
												kVerifyFileStream);
																			
		if (errCode == noErr)
			{
					// Check if header and file size make sense.
					
			fileFoundFlag = CheckMaskFileInfo (maskFileInfoHandle, &errCode);
				
			if (!fileFoundFlag)
				CloseFile (fileInfoPtr);
				
			}	// end "if (errCode == noErr)" 
			
				// If file not found at this point, allow user to locate the		
				// image file.																		
		
		if (!fileFoundFlag && userPrompt)
			{
			promptStringNumber = IDS_FileIO103;
			if (maskSetCode == kTestMaskSet)
				promptStringNumber = IDS_FileIO104;
			
			fileFoundFlag = UserLocateProjectMaskImage (maskFileInfoHandle, 
																		promptStringNumber);
			
			}	// end "if (!fileFoundFlag && userPrompt)"
		
		}	// end "if (fileNamePtr && ...)" 
	
	if (fileFoundFlag)
		{	
		returnCode = LoadMask (maskSetCode,
										maskFileInfoHandle,
										maskInfoPtr,
										maskInfoPtr->fileLayer,
										kCheckCurrentFields);
								
		if (returnCode == 1)
			fileFoundFlag = FALSE;
				
		}	// end "if (fileFoundFlag)" 
		
	else	// !fileFoundFlag
		{
					// Display an alert indicating that the mask file was not found
					// and therefore not loaded in.	
					
		promptStringNumber = IDS_Alert55;
		if (maskSetCode == kTestMaskSet)
			promptStringNumber = IDS_Alert56;							
			
		returnCode = DisplayAlert (
				kErrorAlertID, kCautionAlert, kAlertStrID, promptStringNumber, 0, NULL);
							
		}	// end "else !fileFoundFlag"
		
			// Dispose of temporary handle for mask file structures if needed.
			
	DisposeFileInfoHandle (maskFileInfoHandle);
		
	return (fileFoundFlag);

}	// end "GetSpecifiedMaskFile" 

	

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void InitializeMaskStructure
//
//	Software purpose:	The purpose of this routine is to initialize the input mask
//							structure.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/30/1998
//	Revised By:			Larry L. Biehl			Date: 01/11/1999

void InitializeMaskStructure (
				MaskInfoPtr							maskInfoPtr)

{
	if (maskInfoPtr != NULL)
		{
		maskInfoPtr->fileStreamPtr = NULL;
		maskInfoPtr->maskPointer = NULL;
		maskInfoPtr->maskValueToFieldPtr = NULL;
		
		maskInfoPtr->fileStreamHandle = NULL;
		maskInfoPtr->maskHandle = NULL;
		maskInfoPtr->maskValueToFieldHandle = NULL;
		
		maskInfoPtr->fileLayer = 0;
		maskInfoPtr->maxMaskValue = 0;
		maskInfoPtr->numberColumns = 0;
		maskInfoPtr->numberLines = 0;
		maskInfoPtr->numberLayers = 0;
		maskInfoPtr->startColumn = 0;
		maskInfoPtr->startLine = 0;
		
		}	// end "if (maskInfoPtr != NULL)"
	
}	// end "InitializeMaskStructure"

	

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 LoadMask
//
//	Software purpose:	This routine loads the specified mask input memory and sets up
//							other mask information structures (vectors).
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	0: Indicates that Mask was not loaded in
//							1: Indicates that Mask was loaded in
//
// Called By:			GetSpecifiedMaskFile in SMask.cpp
//							StatisticsDialogMaskCheck in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/11/1998
//	Revised By:			Larry L. Biehl			Date: 04/28/2017

SInt16 LoadMask (
				SInt16								maskSetCode,
				Handle								maskFileInfoHandle,
				MaskInfoPtr							maskInfoPtr,
				UInt32								maskFileLayer,
				SInt16								loadTypeCode)

{
	MaskInfo								maskInfo;  
	 
	FileInfoPtr							maskFileInfoPtr;
	FileIOInstructionsPtr			fileIOInstructionsPtr;
	HUCharPtr							inputbufferPtr;
	HUInt32Ptr							maskValueCountVector;
	
	HUInt16Ptr							maskPointer,
											maskValueToFieldPtr;
	
	LayerInfoPtr						maskLayerInfoPtr;
											
	WindowInfoPtr						projectWindowInfoPtr,
											maskWindowInfoPtr;
	
	Handle								maskHandle,
											maskValueToFieldHandle,
											maskWindowInfoHandle;
	
	UInt32								imageColumnEnd, 
											imageColumnStart, 
											imageLineEnd, 
											imageLineStart,
											j,
											line,
											maskColumnEnd,
											maskColumnStart,
											maskLineEnd,
											maskLineStart,
											maskNumberColumns,
											maskNumberLines,
											maxMaskValue;
	
	SInt16								errCode,
											fileImageType,
											maskInfoHandleStatus,
											returnCode;
											
	UInt16								channel;
	
	Boolean								continueFlag;
	
			
	maskFileInfoPtr = (FileInfoPtr)GetHandlePointer (maskFileInfoHandle);
											
	if (maskFileInfoPtr == NULL)
																						return (0);
	
			// Initialize local variables.
																							
	maskWindowInfoHandle = NULL;
	fileIOInstructionsPtr = NULL;
																					
			// Get a window and layer structures for the mask file.
	
	fileImageType = kMultispectralImageType;
	if (maskFileInfoPtr->thematicType)	
		fileImageType = kThematicImageType;
		
	maskWindowInfoHandle = GetWindowInfoStructures (maskFileInfoHandle, 
																	fileImageType,
																	fileImageType);
	
	if (maskWindowInfoHandle == NULL)
																						return (0);
																						
			// Initialize local variables.
	
	maskValueCountVector = NULL;
	maskHandle = NULL;
	maskValueToFieldHandle = NULL;
	continueFlag = TRUE;
	errCode = noErr;
	returnCode = 1;
	maxMaskValue = 0;

	GetInformationPointers (&maskInfoHandleStatus, 
										maskWindowInfoHandle,
										&maskWindowInfoPtr, 
										&maskLayerInfoPtr, 
										&maskFileInfoPtr);
		
	projectWindowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																gProjectInfoPtr->windowInfoHandle);
	
	imageLineStart = 1;
	imageLineEnd = projectWindowInfoPtr->maxNumberLines;
	imageColumnStart = 1;
	imageColumnEnd = projectWindowInfoPtr->maxNumberColumns;
	
			// Determine if the mask area intersects with the project image area.
			
	maskInfo.numberColumns = maskFileInfoPtr->numberColumns;
	maskInfo.numberLines = maskFileInfoPtr->numberLines;
	maskInfo.startColumn = maskFileInfoPtr->startColumn;
	maskInfo.startLine = maskFileInfoPtr->startLine;
	
	continueFlag = GetMaskArea (&maskInfo,
											0,
											NULL,
											NULL,
											gProjectInfoPtr->startLine,
											gProjectInfoPtr->startColumn, 
											1, 
											1, 
											&imageLineStart, 
											&imageLineEnd, 
											&imageColumnStart, 
											&imageColumnEnd, 
											&maskLineStart, 
											&maskColumnStart);
	
	if (continueFlag)
		{
		maskNumberColumns = imageColumnEnd - imageColumnStart + 1;
		maskNumberLines = imageLineEnd - imageLineStart + 1;
		
		maskColumnEnd = maskColumnStart + maskNumberColumns - 1;
		maskLineEnd = maskLineStart + maskNumberLines - 1;
		
				// Now make sure that we have the correct size of memory for the
				// mask.
				
		maskHandle = MNewHandle ((SInt64)maskNumberLines * 
													(maskNumberColumns+1) * sizeof (UInt16));
				
		maskPointer = (HUInt16Ptr)GetHandlePointer (maskHandle);
			
		continueFlag = (maskPointer != NULL);
		
		}	// end "if (continueFlag)"
	
	if (continueFlag)
		{
				// Note that the channel list for 'GetLineOfData' is 0 based.
				
		channel = (UInt16)maskFileLayer - 1;
		
				// Get buffers to read data from image file into.					
			 				
		continueFlag = GetIOBufferPointers (&gFileIOInstructions[0],
														maskWindowInfoPtr,
														maskLayerInfoPtr,
														maskFileInfoPtr,
														&gInputBufferPtr, 
														&gOutputBufferPtr,
														maskColumnStart,
														maskColumnEnd,
														1,
														1,
														&channel,
														kDoNotPackData,
														kDoNotForceBISFormat,
														kForce2Bytes,
														kDoNotAllowForThreadedIO,
														&fileIOInstructionsPtr);
		
		}	// end "if (continueFlag)"
		
	if (continueFlag)
		{
		inputbufferPtr = gInputBufferPtr;
			
				// Load some of the File IO Instructions structure that pertain
				// to the specific area being used.
				
		SetUpFileIOInstructions (fileIOInstructionsPtr,
											NULL,
											maskLineStart,
											maskLineEnd,
											1,
											maskColumnStart,
											maskColumnEnd,
											1,
											1,
											&channel,
											kSetSpecialBILFlagFalse);
						
				// Intialize the nextTime variable to indicate when the next check	
				// should occur for a command-.													
				
		gNextTime = TickCount ();
										
		for (line=maskLineStart; line<=maskLineEnd; line++)
			{
					// Handle case when input and output buffers are the same. No data
					// transfer needs to be done. Copy directly into the mask buffer.
			
			if (gInputBufferPtr == gOutputBufferPtr)		
				inputbufferPtr = (HUCharPtr)&maskPointer[1];
			
					// Read in a line of data.												
				 
			errCode = GetLineOfData (fileIOInstructionsPtr,
												line, 
												(UInt32)maskColumnStart,
												(UInt32)maskColumnEnd, 
												(UInt32)1,
												inputbufferPtr,
												(HUCharPtr)&maskPointer[1]);
									
			if (errCode != noErr)
				{
				continueFlag = FALSE;														
				break;
				
				}	// end "if (errCode != noErr)"
				 
			else	// errCode == noErr
				{
						// Check if there are any mask values in this line and get the
						// maximum mask value. The maximum mask value will be used to 
						// determine the length of the 'maskValueToField' vector.
						
				maskPointer[0] = 0;
							
				for (j=1; j<=maskNumberColumns; j++)
					{
					if (maskPointer[j] > 0)
						{
						maxMaskValue = MAX (maxMaskValue, maskPointer[j]);
						maskPointer[0] = 1;
						
						}	// end "if (maskPointer[j] > 0)"
						
					}	// end "for (j=0;..." 
					
				maskPointer += maskNumberColumns + 1;
				
						// Check if user wants to exit reading the mask.									
				
				if (TickCount () >= gNextTime)
					{
					if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
						{
						continueFlag = FALSE;
						break;
						
						}	// end "if (!CheckSomeEvents (..."
						
					}	// end "if (TickCount () >= gNextTime)" 
					
				}	// end "else errCode == noErr"
				
			}	// end "for (line=1; line<=fileInfoPtr->numberLines; line++)"

		}	// end "if (continueFlag)"
		
			// Close up any File IO Instructions structure that pertain to the 
			// specific area used.
			
	CloseUpFileIOInstructions (fileIOInstructionsPtr, NULL); 
				
			// Dispose of the IO buffer.		
								
	DisposeIOBufferPointers (fileIOInstructionsPtr,
										&gInputBufferPtr, 
										&gOutputBufferPtr);
	
	if (continueFlag)
		{
				// Get memory for mask value to class vector if needed..
				
		if (maskSetCode != kMaskSet)
			{
			maskValueToFieldHandle = 
								MNewHandleClear ((maxMaskValue+1) * sizeof (UInt16));
			
			continueFlag = (maskValueToFieldHandle != NULL);
			
			}	// end "if (maskSetCode != kMaskSet)"
			
		}	// end "if (continueFlag)"
	
	if (continueFlag)
		{
				// Get temporary memory to stored counts of mask values.
				
		if (maskSetCode != kMaskSet)
			{
			maskValueCountVector = (HUInt32Ptr)MNewPointerClear (
															(maxMaskValue+1) * sizeof (UInt32));
			
			continueFlag = (maskValueCountVector != NULL);
			
			}	// end "if (maskSetCode != kMaskSet)"
			
		}	// end "if (continueFlag)"
	
	if (continueFlag)
		{
				// Load the mask value to class vector if needed.
			
		if (maskSetCode != kMaskSet)
			{
			maskValueToFieldPtr = (HUInt16Ptr)GetHandlePointer (
																	maskValueToFieldHandle);
															
					// Set the pointer value back to the beginning.
															
			maskPointer = (HUInt16Ptr)GetHandlePointer (maskHandle);
															
					// First find which mask values are used.
										
			for (line=1; line<=maskNumberLines; line++)
				{
				if (maskPointer[0] > 0)
					{	
					for (j=1; j<=maskNumberColumns; j++)
						{
						if (maskPointer[j] > 0)
							maskValueCountVector[maskPointer[j]]++;
							
						}	// end "for (j=0;..." 
					
					}	// end "if (maskPointer[0] > 0)"
						
				maskPointer += maskNumberColumns + 1;
				
				}	// end "for (line=1; line<=maskNumberLines; line++)"
			
			if (loadTypeCode == kNewMaskFields)
				continueFlag = LoadNewMaskFields (maskSetCode,
																maskInfoPtr,
																maskValueCountVector,
																maskValueToFieldPtr,
																maxMaskValue,
																maskWindowInfoHandle);
															
			else if (loadTypeCode == kCheckCurrentFields)
				{
				returnCode = CheckCurrentMaskFields (maskSetCode,
																	maskInfoPtr,
																	maskValueCountVector,
																	maskValueToFieldPtr,
																	maxMaskValue);
				
				if (returnCode == 1)
					continueFlag = FALSE;
		
				if (returnCode == 2)
				
							// The current training mask is different than what the current
							// training fields indicate. Force the mask statistics to be 
							// recomputed.
		
					ClearProjectMaskStatistics ();
																		
				}	// end "else if (loadTypeCode == kCheckCurrentFields)"
				
			}	// end "if (maskSetCode != kMaskSet)"
			
		}	// end "if (continueFlag)"
	
			// Dispose of the window structures for the mask file.
	
	#if defined multispec_mac
		UnlockAndDispose (maskWindowInfoPtr->layerInfoHandle);
		UnlockAndDispose (maskWindowInfoHandle); 
	#endif	// defined multispec_mac
		
	#if defined multispec_win || defined multispec_wx
				// Make sure that the file information handle is not deleted yet.
				
		maskWindowInfoPtr->fileInfoHandle = NULL;
		CMImageWindow* imageWindowCPtr = GetWindowClassPointer (maskWindowInfoHandle);
		delete imageWindowCPtr;
	#endif	// defined multispec_win || lin
		
	if (continueFlag)
		{
				// Everything is okay. 
				// Dispose of the old mask handles.
				
		UnlockAndDispose (maskInfoPtr->maskHandle);
		UnlockAndDispose (maskInfoPtr->maskValueToFieldHandle);
		
				// Close the current mask file.
				
		CloseFile (maskInfoPtr);
	
				// Unlock the new mask handles.
				
		CheckAndUnlockHandle (maskHandle);
		CheckAndUnlockHandle (maskValueToFieldHandle);
		
				// Replace the previous mask information with the new mask 
				// information.
		
		CopyFileStream (maskFileInfoPtr,
								&maskInfoPtr->fileStreamHandle,
								&maskInfoPtr->fileStreamPtr);
		
		maskInfoPtr->maskPointer = NULL;
		maskInfoPtr->maskValueToFieldPtr = NULL;
		maskInfoPtr->maskHandle = maskHandle;
		maskInfoPtr->maskValueToFieldHandle = maskValueToFieldHandle;
		maskInfoPtr->fileLayer = maskFileLayer;
		maskInfoPtr->maxMaskValue = maxMaskValue;
		maskInfoPtr->numberColumns = maskNumberColumns;
		maskInfoPtr->numberLines = maskNumberLines;
		maskInfoPtr->numberLayers = maskFileInfoPtr->numberChannels;
		maskInfoPtr->startColumn = 
								maskFileInfoPtr->startColumn + maskColumnStart - 1;
		maskInfoPtr->startLine = 
								maskFileInfoPtr->startLine + maskLineStart - 1;
		
		}	// end "if (continueFlag)"
	
	else	// !continueFlag
		{
		UnlockAndDispose (maskHandle);
		UnlockAndDispose (maskValueToFieldHandle);
		
		}	// end "else !continueFlag"
		
	CheckAndDisposePtr (maskValueCountVector);
	
	CheckAndUnlockHandle (maskFileInfoHandle);
 	
 	if (loadTypeCode == kNewMaskFields)
 		{   		
		if (!continueFlag || errCode != noErr)
			returnCode = 0;
			
		}	// end "if (loadTypeCode == kNewMaskFields)"
  	
	return (returnCode);
	
}	// end "LoadMask"  

	

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean LoadMaskFileInfo
//
//	Software purpose:	The purpose of this routine is to load the mask file information
//							structure given the file stream (file name).
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	Handle to the mask file information structure
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/03/2017
//	Revised By:			Larry L. Biehl			Date: 05/03/2017

Handle LoadMaskFileInfo (
				CMFileStream*						maskFileStreamPtr)

{ 
	CMFileStream*						fileStreamPtr;
	FileInfoPtr							fileInfoPtr;
	Handle								fileInfoHandle;
	
	SInt16								errCode;
	
	Boolean								fileFoundFlag = FALSE;
	
	
			// Get a handle to a block of memory to be used for file information 
			// for mask image file.								

	fileInfoHandle = InitializeFileInfoStructure (NULL, kNotPointer);	
	
	fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);

	if (fileInfoPtr != NULL)
		{								
				// Initialize variables in the structure.	
				
		fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
		InitializeFileStream (fileStreamPtr, maskFileStreamPtr);

				// Check if image file is in the same volume as the project			
				// file. If it is, open a path to the file and return the			
				// handle to the file information.											
				
		errCode = OpenFileReadOnly (fileStreamPtr, 
												kResolveAliasChains, 
												kLockFile, 
												kVerifyFileStream);
																			
		if (errCode == noErr)
			{
					// Check if header and file size make sense.
					
			fileFoundFlag = CheckMaskFileInfo (fileInfoHandle, &errCode);
				
			if (!fileFoundFlag)
				CloseFile (fileInfoPtr);
				
			}	// end "if (errCode == noErr)"
		
		if (!fileFoundFlag)
			DisposeFileInfoHandle (fileInfoHandle);
		
		}	// end "if (fileInfoPtr != NULL)"
		
	return (fileInfoHandle);
	
}	// end "LoadMask" 

	

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean LoadNewMaskFields
//
//	Software purpose:	The purpose of this routine is to load the mask information
//							into new fields that are being added to the project.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/11/1998
//	Revised By:			Larry L. Biehl			Date: 04/28/2017

Boolean LoadNewMaskFields (
				SInt16								maskSetCode,
				MaskInfoPtr							maskInfoPtr,
				HUInt32Ptr							maskValueCountVector,
				HUInt16Ptr							maskValueToFieldPtr,
				UInt32								maxMaskValue,
				Handle								maskWindowInfoHandle)

{ 
	HPFieldIdentifiersPtr			fieldIdentPtr;
	HUCharPtr							classNamePtr;
	UInt16*								classSymbolPtr;
	
	Handle								classNameHandle,
											maskFileInfoHandle;
	
	UInt32								j,
											numberClasses;
											
	SInt16								classNumber;
	
	UInt16								currentClass,
											currentField;
											
	Boolean								continueFlag,
											matchingClassFoundFlag,
											namesSameAsProjectNamesFlag;
											
	
			// Initialize local variables.
			
	continueFlag = TRUE;
	classNameHandle = NULL;
	
			// Remove any current mask fields.
			
	CutMaskFields (maskSetCode);
	
			// Get support file stream structure.		
			
	SetSupportFileStreamHandle (maskWindowInfoHandle);	
	
			// Get any available class names that may be with thematic formatted mask
			// files.
			
	LoadClassNameDescriptions (maskWindowInfoHandle);
	
			// Get a pointer to the class names.
	
	maskFileInfoHandle = GetFileInfoHandle (maskWindowInfoHandle);
	
	numberClasses = GetNumberClasses (maskFileInfoHandle);
	
	if (numberClasses > 0)
		classNameHandle = GetClassDescriptionHandle (maskFileInfoHandle);
	
	classNamePtr = (HUCharPtr)GetHandlePointer (classNameHandle);
	if (classNamePtr != NULL)
		{
				// Now skip the first class name if it is for the background class;
				// i.e. data value of 0.						
				
		classSymbolPtr = (UInt16*)&classNamePtr[numberClasses*sizeof (Str31)];
		if (*classSymbolPtr == 0)
			{
			classNamePtr += 32;
			numberClasses--;
			
			}	// end "if (*classSymbolPtr == 0)"
			
		}	// end "if (classNamePtr != NULL)"
	
			// Determine if any of the thematic class names are the same as any of
			// the project class names. If so then, the thematic class fields will
			// be assigned to the appropriate project class. Those that do not match
			// will be assigned to a new project class.
				
	namesSameAsProjectNamesFlag = DetermineIfClassesSameAsProjectClassNames (
																							classNamePtr,
																							numberClasses);
	
			// Set the current field and class values.
			
	currentField = gProjectInfoPtr->numberStorageFields;
	currentClass = 1;
														
	if (namesSameAsProjectNamesFlag)
		currentClass = gProjectInfoPtr->numberStatisticsClasses + 1;
		
												
			// Now load the mask to field vector in order of class number 
			// relative to the mask values used. Assume one field per
			// class.
			//
			// Also while doing this create the mask fields and assign them
			// to the classes. Create classes if one needs to.

	sprintf ((char*)&gTextString[1], "MaskClass");
	gTextString[0] = 9; 
	
	sprintf ((char*)&gTextString3[1], "MaskArea");
	gTextString3[0] = 8; 
			
	matchingClassFoundFlag = FALSE;
	for (j=1; j<=maxMaskValue; j++)
		{
		if (maskValueCountVector[j] > 0)
			{
			//if (currentClass > gProjectInfoPtr->numberStatisticsClasses + 1)
			//	{
			//	continueFlag = FALSE;
			//	break;
				
			//	}	// end "if (currentClass > numberClasses)"
				
			if (namesSameAsProjectNamesFlag)
				{
				classNumber = CheckForDuplicateClassName (-1, (Str255*)classNamePtr);
				
				if (classNumber > 0)
					matchingClassFoundFlag = TRUE;
				
				else	// classNumber <= 0
					{
							// This class name is different than all of the other project
							// class names. Assign this mask field to a new project class.
					
					classNumber = currentClass;
					matchingClassFoundFlag = FALSE;
					
					}	// end "else classNumber <= 0"
				
				}	// end "if (namesSameAsProjectNamesFlag)"
				
			else	// !namesSameAsProjectNamesFlag
				classNumber = currentClass;
				
			if (classNumber > gProjectInfoPtr->numberStatisticsClasses)
				{
						// Get name for new class.
						
				if (classNamePtr == NULL)	
					GetUniqueClassName (classNumber,
													(UCharPtr)gTextString,
													9,
													FALSE,
													(char*)NULL);
													
				else	// classNamePtr != NULL
					{
					BlockMoveData (classNamePtr, 
											(Ptr)gTextString, 
											classNamePtr[0]+1);
											
					}	// end "else classNamePtr != NULL"
									
				continueFlag = AddClassToProject (gTextString);
				
				}	// end "if (currentClass > gProjectInfoPtr->numberStatisticsClasses)"
				
			else	// currentClass <= gProjectInfoPtr->numberStatisticsClasses
				gProjectInfoPtr->currentClass = classNumber - 1;
			
			if (continueFlag)
				{
				if (classNamePtr == NULL)
					{
							// Make sure that we start with the base name.
							
					gTextString3[0] = 8; 
					
					}	// end "if (classNamePtr == NULL)"
					
				else	// classNamePtr != NULL
					{	
					BlockMoveData (classNamePtr, 
											(Ptr)gTextString3, 
											classNamePtr[0]+1);
											
					sprintf ((char*)&gTextString3[classNamePtr[0]+1], 
											"_M");
											
					gTextString3[0] += 2;
					
					gTextString3[0] = MIN (gTextString3[0], 30);
					
					}	// end "else classNamePtr != NULL"
											
				GetUniqueFieldName (j,
											(UCharPtr)gTextString3,
											gTextString3[0]);
								
				continueFlag = AddFieldToProject (classNumber-1, 
																kMaskType, 
																maskSetCode,
																gTextString3);
											
				}	// end "if (continueFlag)"
				
			if (continueFlag)
				{
				fieldIdentPtr = 
						&gProjectInfoPtr->fieldIdentPtr[gProjectInfoPtr->currentField];
				fieldIdentPtr->numberPixels = maskValueCountVector[j];
				
				if (maskSetCode == kTrainingMaskSet)
					{
					gProjectInfoPtr->classNamesPtr[gProjectInfoPtr->currentClass].
											numberTrainPixels += fieldIdentPtr->numberPixels;
      			//fieldIdentPtr->numberPixelsUsedForStats =
      			//												fieldIdentPtr->numberPixels;
      			//fieldIdentPtr->loadedIntoClassStats = TRUE;
					
					}	// end "if (maskSetCode == kTrainingMaskSet)"
																			
				fieldIdentPtr->maskValue = (UInt16)j;	
																				
				gProjectInfoPtr->moveMemoryFlag = TRUE;
											
				}	// end "if (continueFlag)"
		
			maskValueToFieldPtr[j] = currentField;
			
			if (!matchingClassFoundFlag)
				currentClass++;
			
			currentField++;
			
			if (classNamePtr != NULL)
				classNamePtr += 32;
			
			}	// end "if (maskValueCountVector[j] > 0)"
			
		else	// maskValueCountVector[j] == 0
			{
			if (classNamePtr != NULL && numberClasses >= maxMaskValue)
				classNamePtr += 32;
			
			}	// end "else maskValueCountVector[j] == 0"
			
		if (!continueFlag)
			break;
			
		}	// end "for (j=1; j<=maxMaskValue; j++)"
		
	CheckAndUnlockHandle (classNameHandle);
	
	UnlockAndDisposeSupportFileStream (maskWindowInfoHandle);
		
	return (continueFlag);
	
}	// end "LoadNewMaskFields"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetUpMaskAreaDescriptionParameters
//
//	Software purpose:	This routine sets up the mask area description parameters. It
//							is currently used in places where one checks if the input pixel
//							is a training pixe.
//
//	Parameters in:		
//
//							
//
//	Parameters out:			
//
// Value Returned:	FALSE if the project mask image file is not located.
//							TRUE if the project mask image file is located.
//
// Called By:			GetSpecifiedMaskFile in SMask.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/29/2001
//	Revised By:			Larry L. Biehl			Date: 04/28/2017

void SetUpMaskAreaDescriptionParameters (
				AreaDescriptionPtr				maskAreaDescriptionPtr, 
				AreaDescriptionPtr				areaDescriptionPtr)

{
	Boolean								definedAreaFlag;
			
			
	if (maskAreaDescriptionPtr != NULL)
		{
				// Initialize mask area description in case it is needed.
			
		InitializeAreaDescription (maskAreaDescriptionPtr, 
												areaDescriptionPtr->lineStart, 
												areaDescriptionPtr->lineEnd, 
												areaDescriptionPtr->columnStart, 
												areaDescriptionPtr->columnEnd, 
												1, 
												1,
												1,
												1,
												0);
												
				// If each pixel is to be checked to see whether it is a training sample,
				// then set up some mask variables if a training mask exists.
		
		if (gProjectInfoPtr->trainingMask.maskHandle != NULL)
			{
					// Determine if any of the requested lines and columns are within
					// the area represented by the mask.
			
			definedAreaFlag = GetMaskArea (
											NULL,
											kTrainingType,
											gProjectInfoPtr,
											NULL,
											gProjectInfoPtr->startLine,
											gProjectInfoPtr->startColumn,
											1, 
											1, 
											(UInt32*)&maskAreaDescriptionPtr->lineStart, 
											(UInt32*)&maskAreaDescriptionPtr->lineEnd, 
											(UInt32*)&maskAreaDescriptionPtr->columnStart, 
											(UInt32*)&maskAreaDescriptionPtr->columnEnd, 
											(UInt32*)&maskAreaDescriptionPtr->maskLineStart, 
											(UInt32*)&maskAreaDescriptionPtr->maskColumnStart);
												
					// Get the mask buffer and value-to-field vector if needed.
			
			if (definedAreaFlag)
				{
				maskAreaDescriptionPtr->maskInfoPtr = GetMaskInfoPointer (
																		kTrainingType,
																		gProjectInfoPtr,
																		NULL);	
																					
				maskAreaDescriptionPtr->maskInfoPtr->maskPointer = 
							(HUInt16Ptr)GetHandlePointer (
										maskAreaDescriptionPtr->maskInfoPtr->maskHandle);
				
						// Adjust buffer to start of mask area within the image area.
											
				maskAreaDescriptionPtr->maskInfoPtr->maskPointer += 
						(maskAreaDescriptionPtr->maskLineStart - 1) * 
									(maskAreaDescriptionPtr->maskInfoPtr->numberColumns + 1);
																	
				}	// end "if (definedAreaFlag)"
			
			}	// end "if (gProjectInfoPtr->trainingMask.maskHandle != NULL)"
			
		}	// end "if (maskAreaDescriptionPtr != NULL)"
	
}	// end "SetUpMaskAreaDescriptionParameters"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean UserLocateProjectMaskImage
//
//	Software purpose:	This routine allows the user to locate the project
//							base image file.
//
//	Parameters in:		fileInfoPtr - pointer to structure for file information.
//
//							promptStringNumber - resource string number of prompt.
//
//	Parameters out:	None.		
//
// Value Returned:	FALSE if the project mask image file is not located.
//							TRUE if the project mask image file is located.
//
// Called By:			GetSpecifiedMaskFile in SMask.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/23/1999
//	Revised By:			Larry L. Biehl			Date: 04/23/1999

Boolean UserLocateProjectMaskImage (
				Handle								fileInfoHandle, 
				SInt16								promptStringNumber)

{
	CMFileStream*						fileStreamPtr;
	
	FileInfoPtr							fileInfoPtr;
	
	SInt16								errCode,
											numberFileTypes;
	
	Boolean								continueFlag,
				 							fileFoundFlag;
	
	SignedByte							handleStatus;
	

			// Check input variables.															
			
	if (fileInfoHandle == NULL)
																					return (FALSE);
																						
			// Initialize local variables.													
			
	fileFoundFlag = FALSE;
	
			// Lock handle to file information and get pointer to it.				
			
	fileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (
															fileInfoHandle, &handleStatus);
																
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
	
			// If the option key was down when the 'Open Image' menu item			
			// was selected then include all files in the file list.  				
			// Otherwise just show the selected file types.								
			
	numberFileTypes = GetNumberFileTypes ();

			// Loop until an acceptable mask image file is located or the user	
			// cancels.																				
			
	do
		{	
		SetType (fileStreamPtr, kTEXTFileType);
		errCode = GetFile (fileStreamPtr,
									numberFileTypes, 
									gListTypes, 
									NULL,
									NULL,
									NULL, 
									promptStringNumber);
	   continueFlag = ((errCode == noErr) & FileExists (fileStreamPtr));
	   				
	   		// Continue if no IO error and user did not cancel.					
	   
	   if (continueFlag)
			fileFoundFlag = CheckMaskFileInfo (fileInfoHandle, &errCode);
			
		}		while (continueFlag && !fileFoundFlag);
				
			// Unlock the file information handle if needed.							
	
	MHSetState (fileInfoHandle, handleStatus);
		
	return (fileFoundFlag);

}	// end "UserLocateProjectMaskImage" 

