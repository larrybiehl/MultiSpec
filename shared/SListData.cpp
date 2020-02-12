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
//	File:						SListData.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/25/2019
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	This file contains functions that allow the user to
//								list the data values for selected channels, classes
//								and fields and or selected area to the output 
//								text window or a disk file.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"    

#if defined multispec_wx
   #include "xGraphDoc.h"
	#include "xImageView.h" 
	#include "xListDataDialog.h" 
	#include "xMultiSpec.h"
   #include "wx/evtloop.h"
#endif

#if defined multispec_mac || defined multispec_mac_swift
	#include	"MGraphView.h"
	
	#define	IDS_ListData11		11
	#define	IDS_ListData12		12
	#define	IDS_ListData13		13
	#define	IDS_ListData14		14
#endif	// defined multispec_mac || defined multispec_mac_swift  
  
#if defined multispec_win 
	#include	"WGraphView.h"
	
	#include "WMultiSpec.h" 
	#include "WGraphDoc.h"
	#include "WListDataDialog.h"
#endif	// defined multispec_win 

ListDataSpecsPtr		gListDataSpecsPtr;

SInt16					gListDataFormatCode = 0;
	
	
	
			// Prototypes for routines in this file that are only called by		
			// other routines in this file.													
			
Boolean	CheckListDataTextWindowSpaceNeeded (
				ListDataSpecsPtr 					listDataSpecsPtr);

UInt32	DetermineBytesForListDataText (
				ListDataSpecsPtr 					listDataSpecsPtr,
				Boolean 								textWindowFlag);

PascalVoid	DrawListDataFormatPopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber);

SInt16	GetMaximumDataValueStringLength (
				WindowInfoPtr						windowInfoPtr,
				FileInfoPtr							fileInfoPtr,
				Boolean								includeMaxColumnValueFlag,
				SInt16								numberFFormatDecimalDigits,
				SInt16*								numberEFormatDecimalDigitsPtr);

SInt16	GetStringLengthForNumber (
				UInt32								maximumNumber);
											
SInt16	ListClassData (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				SInt16								dataValueFieldSize,
				SInt16								numberEDecimalDigits,
				SInt16								numberFDecimalDigits, 
				SInt16								classNumber);

Boolean	ListDataDialog (
				FileInfoPtr							fileInfoPtr);

Boolean	ListDataTitleLine (
				SInt16								dataValueFieldSize);

Boolean	ListDataTitleLine2 (
				SInt16								dataValueFieldSize,
				SInt16								classNumberCode, 
				SInt16								fieldNumber);

Boolean	ListDataTitleLine3 (
				SInt16								pointType,
				SInt16								dataValueFieldSize, 
				UInt32								columnStart,
				UInt32								columnEnd,
				FileIOInstructionsPtr			fileIOInstructionsPtr);

SInt16	ListFieldData (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				SInt16								dataValueFieldSize,
				SInt16								numberEDecimalDigits,
				SInt16								numberFDecimalDigits, 
				SInt16								classNumberCode, 
				SInt16								fieldNumber);

Boolean	LoadListDataSpecs (
				Handle								windowInfoHandle);

Boolean	ListProjectData (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				SInt16								dataValueFieldSize,
				SInt16								numberEDecimalDigits,
				SInt16								numberFDecimalDigits);



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CheckListDataTextWindowSpaceNeeded
//
//	Software purpose:	The purpose of this routine is to check if there is
// 						enough memory to list the histogram values in the
//							text window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 11/17/1993
//	Revised By:			Larry L. Biehl			Date: 07/26/2005	

Boolean CheckListDataTextWindowSpaceNeeded (
				ListDataSpecsPtr					listDataSpecsPtr)

{
	Boolean								returnFlag;
	UInt32								numberBytesNeeded;
	
	
	if (!(gOutputCode & 0x0001))
																						return (TRUE);
	
			// Initialize local variables.													
		
	numberBytesNeeded = DetermineBytesForListDataText (listDataSpecsPtr, TRUE);
	
			// Allow space for status dialog box.
			
	if (numberBytesNeeded < UInt32_MAX-gMinMemoryForDrawingDialog)
		numberBytesNeeded += gMinMemoryForDrawingDialog;
	else	// floatNumberBytesNeeded >= UInt32_MAX-gMinMemoryForDrawingDialog
		numberBytesNeeded = UInt32_MAX;									
	
	returnFlag = CheckTextWindowSpaceNeeded (numberBytesNeeded);
		
	return (returnFlag);

}	// end "CheckListDataTextWindowSpaceNeeded" 


/*
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CreateChannelWavelengthOrderSubset ()
//
//	Software purpose:	The purpose of this routine is to load the List 
//							Data Specification structure with the default set of
//							parameters.
//
//	Parameters in:		.
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			ListDataControl ()
//
//	Coded By:			Larry L. Biehl			Date: 06/29/2018
//	Revised By:			Larry L. Biehl			Date: 07/02/2018

void CreateChannelWavelengthOrderSubset (
				WindowInfoPtr						windowInfoPtr,
				FileInfoPtr							fileInfoPtr,
				SInt16*								channelListPtr,
				SInt32								channelListLength)
			
{
	float*								channelValuePtr;
	
	SInt16*								listDataChannelOrderPtr;
	
	UInt16*								channelWavelengthOrderPtr;
	
	Handle								channelValuesHandle = NULL;
	
	SInt32								missingIndex;
	
	UInt32								index,
											loops,
											nextIndex;
	
	
	if (fileInfoPtr != NULL)
		channelValuesHandle = fileInfoPtr->channelValuesHandle;

	if (channelValuesHandle != NULL)
		{
		channelValuePtr = (float*)GetHandlePointer (channelValuesHandle, kLock);
		
		channelWavelengthOrderPtr =
									(UInt16*)&channelValuePtr[2*fileInfoPtr->numberChannels];
		
				// Initialize the List Data channel order vector
		
		listDataChannelOrderPtr =
								&channelListPtr[windowInfoPtr->totalNumberChannels];
		
		if (channelListLength == fileInfoPtr->numberChannels)
			{
					// Just make copy of the image channel wavelength order
			
			for (index=0; index<fileInfoPtr->numberChannels; index++)
				listDataChannelOrderPtr[index] = channelWavelengthOrderPtr[index];
			
			}	// end "if (channelListLength == fileInfoPtr->numberChannels)"
		
		else	// create a channel wavelength order subset
			{
					// First fill the vector with -1s.
			
			for (index=0; index<fileInfoPtr->numberChannels; index++)
				listDataChannelOrderPtr[index] = -1;
			
					// Now fill with the input channel subset
			
			for (index=0; index<channelListLength; index++)
				listDataChannelOrderPtr[channelListPtr[index]] =
												channelWavelengthOrderPtr[channelListPtr[index]];
			
					// Now change listDataChannelOrder to reflect order for just
					// the channels being used. Reduce any channel index values higher
					// that channels not being used.
			
			for (loops=0; loops<fileInfoPtr->numberChannels-channelListLength; loops++)
				{
						// Find -1 in the list
				
				missingIndex = -1;
				for (index=0; index<fileInfoPtr->numberChannels; index++)
					{
					if (listDataChannelOrderPtr[index] == -1)
						{
						missingIndex = channelWavelengthOrderPtr[index];
						listDataChannelOrderPtr[index] = -2;
						break;
						
						}	// end "if (listDataChannelOrderPtr[index] == -1)"
						
					}	// end "for (index=0; index<fileInfoPtr->numberChannels; index++)"
				
						// Now reduce channel index values greater than the missing index
						// by 1.
				
				if (missingIndex >= 0)
					{
					for (index=0; index<fileInfoPtr->numberChannels; index++)
						{
						if (listDataChannelOrderPtr[index] > missingIndex)
							listDataChannelOrderPtr[index]--;
						
						}	// end "for (index=0; index<fileInfoPtr->numberChannels; index++)"
					
					}	// end "if (missingIndex >= 0)"
				
				}	// end "for (loops=0; loops<fileInfoPtr->numberChannels-..."
			
					// Now remove any -1s in the listDataChannelOrder list and move
					// actually channel indices up in the list to replace them
			
			nextIndex = 0;
			for (index=0; index<channelListLength; index++)
				{
				nextIndex++;
				while (listDataChannelOrderPtr[nextIndex] == -1 &&
															nextIndex < fileInfoPtr->numberChannels)
					nextIndex++;
				
				if (listDataChannelOrderPtr[index] < 0)
					listDataChannelOrderPtr[index] = listDataChannelOrderPtr[nextIndex];
						
				}	// end "for (index=0; index<fileInfoPtr->numberChannels; index++)"
			
			}	// end "else create a channel wavelength order subset"
		
		}	// end "if (channelValuesHandle != NULL)"

}	// end "CreateChannelWavelengthOrderSubset"
*/


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		UInt32 DetermineBytesForListDataText
//
//	Software purpose:	The purpose of this routine is determine the approximate
//							number of bytes in the ListData text output.
//
//	Parameters in:		Pointer to the list data instruction structure.
//
//	Parameters out:	None
//
// Value Returned:	Number of bytes needed for text output.
//
//	Calls To:
//
//	Coded By:			Larry L. Biehl			Date: 11/17/1993	
//	Revised By:			Larry L. Biehl			Date: 07/19/2006

UInt32 DetermineBytesForListDataText (
				ListDataSpecsPtr					listDataSpecsPtr, 
				Boolean								textWindowFlag)

{
	double								floatBytesNeeded;
	
	UInt32								bytesNeeded,
											estimatedNumberWidth,
											numberColumns,
											numberControlCharacters,
											numberLines,
											numberTextLines;


	floatBytesNeeded = 0;
	bytesNeeded = 0;
	numberTextLines = 0;
	numberControlCharacters = 0;
												
	estimatedNumberWidth = 4;
	if (gImageWindowInfoPtr->numberBytes == 2)		
		estimatedNumberWidth = 6;
	else if (gImageWindowInfoPtr->numberBytes > 2)		
		estimatedNumberWidth = 12;
		
				// Note that the number of control characters is also needed		
				// if the number of bytes needed for text in the output window		
				// is being computed.  The number of control characters is the		
				// same as the number of data values.										
				// The software used for the text output window requires 6 bytes	
				// of overhead for each control character.								
	
	if (listDataSpecsPtr->listClassesDataFlag)
		{
				// Code needs to be added here to determine the actual number		
				// of data values to be listed.												
				// Now I am assuming 7 lines per class.									
				
		floatBytesNeeded += listDataSpecsPtr->numberClasses * 50;
														
		if (listDataSpecsPtr->includeLineColumnValuesFlag)
			floatBytesNeeded += 7 * 2;
														
		if (listDataSpecsPtr->includeClassFieldFlag)
			floatBytesNeeded += 7 * 2;
		
		numberTextLines +=  listDataSpecsPtr->numberClasses * 7;
		
		}	// end "if (listDataSpecsPtr->listClassesDataFlag)" 
	
	if (listDataSpecsPtr->listSelectedAreaDataFlag)
		{
				// Get the number of data values to be listed.							
				
		numberLines = (listDataSpecsPtr->lineEnd -
									listDataSpecsPtr->lineStart +
											listDataSpecsPtr->lineInterval)/
													listDataSpecsPtr->lineInterval;
			
		numberColumns = (listDataSpecsPtr->columnEnd -
									listDataSpecsPtr->columnStart +
											listDataSpecsPtr->columnInterval)/
													listDataSpecsPtr->columnInterval;
		
		floatBytesNeeded += (double)numberLines * numberColumns * 
														listDataSpecsPtr->numberChannels;
														
		if (listDataSpecsPtr->includeLineColumnValuesFlag)
			floatBytesNeeded += numberLines * numberColumns * 2;
		
		numberTextLines +=  numberLines * numberColumns;
		
		}	// end "if (listDataSpecsPtr->listSelectedAreaDataFlag)" 
			
			// Now convert the number of data values to number of characters.		
		
	numberControlCharacters = bytesNeeded;
	floatBytesNeeded *= estimatedNumberWidth;
	
			// Add in the number of bytes for the List Data header.					
			
	floatBytesNeeded += 100 + listDataSpecsPtr->numberChannels * 50;
	
	if (textWindowFlag)
		{
				// Now add in the overhead for the text output window.				
		
		//bytesNeeded += numberTextLines * sizeof (LineRec);

				// For now assume 10 for WASTE;
				
		floatBytesNeeded += numberTextLines * 10;
		
		floatBytesNeeded += numberControlCharacters * 6;
		
		}	// end "if (textWindowFlag)" 
			
	if (floatBytesNeeded < UInt32_MAX)
		bytesNeeded = (UInt32)floatBytesNeeded + gMinMemoryForDrawingDialog;
	else	// floatBytesNeeded >= UInt32_MAX
		bytesNeeded = UInt32_MAX;
		
	return (bytesNeeded);
			
}	// end "DetermineBytesForListDataText" 



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawListDataFormatPopUp
//
//	Software purpose:	The purpose of this routine is to draw the 
//							disk file selection for Classify To Disk
//							pop up menu.  This routine will also draw a drop
//							shadow box around the pop up selection.
//
//	Parameters in:					
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/07/1998
//	Revised By:			Larry L. Biehl			Date: 10/07/1998	

pascal void DrawListDataFormatPopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.									
	
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpListDataFormatMenu, 
								gListDataFormatCode, 
								TRUE);
	
}	// end "DrawListDataFormatPopUp" 
#endif	// defined multispec_mac



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetMaximumDataValueStringLength
//
//	Software purpose:	The purpose of this routine is to determine the maximum
//							possible string length for the image data values based
//							on the data value specifications.  Information is returned for
//							use with f formatted numbers and if the data are real values,
//							information is also return for E formatted numbers.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/07/1998
//	Revised By:			Larry L. Biehl			Date: 02/25/2012	

SInt16 GetMaximumDataValueStringLength (
				WindowInfoPtr						windowInfoPtr,
				FileInfoPtr							fileInfoPtr,
				Boolean								includeMaxColumnValueFlag,
				SInt16								numberFFormatDecimalDigits,
				SInt16*								numberEFormatDecimalDigitsPtr)

{
	double								maxDataValue;
	
	SInt32								columnValueFieldSize,
											dataValueEFormatFieldSize,
											dataValueFFormatFieldSize,
											dataValueFieldSize,
											numberEFormatDecimalDigits;
											
	
			// Initialize variables. E Format will not be needed for integer
			// based values.
			
	dataValueEFormatFieldSize = -1;
	numberEFormatDecimalDigits = -1;
	
			// Find the data value for the longest string in the data. 
	
	maxDataValue = fabs (fileInfoPtr->maxDataValue);
	maxDataValue = MAX (maxDataValue, fabs (fileInfoPtr->minDataValue));
	
	if (maxDataValue > kMaxValueToListWith_f)
		{
		dataValueEFormatFieldSize = (SInt32)log10 (maxDataValue) + 1;
		numberEFormatDecimalDigits = dataValueEFormatFieldSize - 1;
		numberEFormatDecimalDigits = MIN (7, numberEFormatDecimalDigits);
		
		dataValueEFormatFieldSize = numberEFormatDecimalDigits + 6;
      
		#if defined multispec_win || defined multispec_wx
				// For windows allow for E+xxx or E-xxx instead of
				// E+xx or E-xx as it is for Macintosh
			dataValueEFormatFieldSize++;
		#endif	// defined multispec_win || defined multispec_wx
		
		maxDataValue = kMaxValueToListWith_f;
		
		}	// end "if (maxDataValue > kMaxValueToListWith_f"
	
	dataValueFFormatFieldSize = 1;
	if (maxDataValue != 0)
		dataValueFFormatFieldSize = (SInt32)(log10 (maxDataValue) + 1);
	
	dataValueFFormatFieldSize = MAX (dataValueFFormatFieldSize, 3);
	dataValueFFormatFieldSize = MIN (dataValueFFormatFieldSize, 10);
	
	if (numberFFormatDecimalDigits > 0)
		dataValueFFormatFieldSize += numberFFormatDecimalDigits + 1;
	
	dataValueFFormatFieldSize = MIN (dataValueFFormatFieldSize, 15);
	
	if (dataValueEFormatFieldSize <= 0)
		{
		dataValueEFormatFieldSize = dataValueFFormatFieldSize;
      
		#if defined multispec_win || defined multispec_wx
				// For windows allow for E+xxx or E-xxx instead of
				// E+xx or E-xx as it is for Macintosh
			dataValueEFormatFieldSize++;
		#endif	// defined multispec_win || defined multispec_wx

		numberEFormatDecimalDigits = numberFFormatDecimalDigits-4;

		numberEFormatDecimalDigits = MAX (0, numberEFormatDecimalDigits);
			
		}	// end "if (dataValueEFormatFieldSize <= 0)"
		
	dataValueFieldSize = MAX (dataValueFFormatFieldSize, dataValueEFormatFieldSize);
	
	*numberEFormatDecimalDigitsPtr = (SInt16)numberEFormatDecimalDigits;
	
	if (windowInfoPtr->signedDataFlag)
		dataValueFieldSize++;
		
	if (includeMaxColumnValueFlag)
		{
		columnValueFieldSize = 
							GetStringLengthForNumber (windowInfoPtr->maxNumberColumns);
			
		dataValueFieldSize = MAX (dataValueFieldSize, columnValueFieldSize);
		
		}	// end "if (includeMaxColumnValueFlag)"
		
	return ((SInt16)dataValueFieldSize);
			
}	// end "GetMaximumDataValueStringLength" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetStringLengthForNumber
//
//	Software purpose:	The purpose of this routine is to determine the
//							string length for the input value.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/08/1998
//	Revised By:			Larry L. Biehl			Date: 10/08/1998	

SInt16 GetStringLengthForNumber (
				UInt32								maximumNumber)

{
	SInt16								numberStringLength;
											
											
			// Get maximum possible string length for the line number.
			
	NumToString (maximumNumber, gTextString);
	numberStringLength = MAX (gTextString[0], 3);
		
	return (numberStringLength);
			
}	// end "GetStringLengthForNumber" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ListClassData
//
//	Software purpose:	The purpose of this routine is to list the class
//							data for the requested channels to the output text
//							window or disk file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/24/1990
//	Revised By:			Larry L. Biehl			Date: 12/20/2007	

SInt16 ListClassData (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				SInt16								dataValueFieldSize, 
				SInt16								numberEDecimalDigits,
				SInt16								numberFDecimalDigits, 
				SInt16								classNumber)

{
	HPClassNamesPtr					classNamesPtr;
	HPFieldIdentifiersPtr			fieldIdentPtr;
	
	SInt16								classStorage,
											fieldCount,
											fieldNumber,
											numberChannels,
											numberOfFields;
	
	
			// Initialize local variables.													
			
	numberChannels = gListDataSpecsPtr->numberChannels;
	classNamesPtr = gProjectInfoPtr->classNamesPtr;
	
			// Get the class storage number.													
						
	classStorage = gProjectInfoPtr->storageClass[classNumber];
	
			// Continue only if the number of fields in the class is one or more.
	
	numberOfFields = 0;
	if (gListDataSpecsPtr->fieldTypeCode & kTrainingType)
		numberOfFields += classNamesPtr[classStorage].numberOfTrainFields;
	if (gListDataSpecsPtr->fieldTypeCode & kTestingType)
		numberOfFields += classNamesPtr[classStorage].numberOfTestFields;
		
	if (numberOfFields > 0)
		{
				// Set up status dialog. Class name, and number of						
				// training fields for the class.											
				
		LoadDItemString (gStatusDialogPtr, 
								IDC_Status6, 
								(Str255*)&classNamesPtr[classStorage].name);
		LoadDItemValue (gStatusDialogPtr, IDC_Status10, numberOfFields);
			
		fieldNumber = classNamesPtr[classStorage].firstFieldNumber;
		
		fieldCount = 1;
		while (fieldNumber != -1)
			{
			fieldIdentPtr = &gProjectInfoPtr->fieldIdentPtr[fieldNumber];
	
					// Make certain that field is training field.						
					
			if (fieldIdentPtr->fieldType & gListDataSpecsPtr->fieldTypeCode)
				{
						// Set up field count in status dialog.							
				
				LoadDItemValue (gStatusDialogPtr, IDC_Status8, (SInt32)fieldCount);
					
						// List the requested data for the field.							
					
				if (ListFieldData (fileIOInstructionsPtr, 
												mapProjectionInfoPtr,
												dataValueFieldSize,
												numberEDecimalDigits,
												numberFDecimalDigits,
												classNumber+1, 
												fieldNumber) == 0)
																							return (0);
				
				fieldCount++;
								
				}	// end "if (fieldIdentPtr[fieldNumber].field..." 
				
			fieldNumber = fieldIdentPtr->nextField;
			
			}	// end "while (fieldNumber != -1)" 
			
		}	// end "if (classNamesPtr[classStorage].number ..." 
	
			// Indicate that routine completed normally.									
			
	return (1);
		
}	// end "ListClassData" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ListDataControl ()
//
//	Software purpose:	The purpose of this routine is to handle the
//							"List Data" event.  The defaults will be set
//							according to the statistics window mode and selection.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/24/1990
//	Revised By:			Larry L. Biehl			Date: 06/29/2018

void ListDataControl (void)

{   
	time_t								startTime;
	
	CMFileStream*						resultsFileStreamPtr;
	CMGraphView*						graphViewCPtr = NULL;
	FileIOInstructionsPtr			fileIOInstructionsPtr;
	GraphPtr								graphRecordPtr;
	MapProjectionInfoPtr 			mapProjectionInfoPtr;
	
	Handle								activeWindowInfoHandle,  
											graphRecHandle,
											mapProjectionHandle;
	
	UInt32								numberBytes,
											numberValues; 
	                                      
	SInt16								activeImageGlobalHandleStatus,
											dataValueFieldSize,
											numberEDecimalDigits,
											savedThresholdSize;
	
	Boolean								continueFlag,
											continueGraphFlag,
											showStatusDialogFlag;
	  
	SignedByte							handleStatus;
	
		    		
			// If spare memory had to be used to load code resources, then exit	
			// routine.																				
			
	if (gMemoryTypeNeeded < 0)
																							return;
																							
			// Code resources loaded okay, so set flag back for non-Code			
			// resources.																			
			
	gMemoryTypeNeeded = 0;
 			
 			// Initialize variables.															
 			
 	gStatusDialogPtr = NULL;
 	gListDataSpecsPtr = NULL;
	graphRecHandle	= NULL;
	fileIOInstructionsPtr = NULL;
	mapProjectionHandle = NULL; 
	mapProjectionInfoPtr = NULL;
	numberBytes = 0;
	
			// Reset global variables for list data.										
			
	savedThresholdSize = gThresholdSize;
	gThresholdSize = 0;
 	
 			// Get image window globals.
 	
	continueFlag = SetUpActiveImageInformationGlobals (
																&activeWindowInfoHandle,
																&activeImageGlobalHandleStatus, 
																kUseProject);
	
			// Set up the list data specification structure.							
			
	if (continueFlag && LoadListDataSpecs (activeWindowInfoHandle))
		{
		if (gImageWindowInfoPtr == NULL || gImageFileInfoPtr == NULL)
																							return;
		
		continueFlag = ListDataDialog (gImageFileInfoPtr);
			
		if (continueFlag)
			{
			startTime = time (NULL);
	
			InitializeAreaDescription (&gAreaDescription);
			
					// Initialize some global variables pertaining to output.		
					
			gOutputTextOKFlag = TRUE;
			gOutputCode = gListDataSpecsPtr->outputStorageType;
			gOutputForce1Code = (gOutputCode | 0x0001);
			
			continueFlag = TRUE;
			UInt16* channelsPtr = (UInt16*)GetHandlePointer (
															gListDataSpecsPtr->channelsHandle);
			
			SInt16* classPtr = (SInt16*)GetHandlePointer (
																gListDataSpecsPtr->classHandle);
				 						
			continueFlag = GetIOBufferPointers (
										&gFileIOInstructions[0],
										gImageWindowInfoPtr,
										gImageLayerInfoPtr,
										gImageFileInfoPtr,
										&gInputBufferPtr, 
										&gOutputBufferPtr,
										1,
				 						gImageWindowInfoPtr->maxNumberColumns,
				 						gListDataSpecsPtr->columnInterval,
				 						gListDataSpecsPtr->numberChannels,
										channelsPtr,
										kPackData,
										kForceBISFormat,
										kForceReal8Bytes,
										kDoNotAllowForThreadedIO,
										&fileIOInstructionsPtr);
	
					// Get pointer to buffer to use to store data values to be listed.  
					// Make certain that there is a minimum of 64 bytes.							
			
			if (continueFlag)
				{
						// Get the maximum length required for the data values.
						
				dataValueFieldSize = GetMaximumDataValueStringLength (
									gImageWindowInfoPtr,
									gImageFileInfoPtr, 
									gListDataSpecsPtr->outputFormatCode == kChannelColumnList,
									gListDataSpecsPtr->numberFDecimalDigits,
									&numberEDecimalDigits);
						
				if (gListDataSpecsPtr->outputFormatCode == kChannelColumnList)
					{
					numberValues = (gListDataSpecsPtr->columnEnd - 
						gListDataSpecsPtr->columnStart + gListDataSpecsPtr->columnInterval)/
													gListDataSpecsPtr->columnInterval;
													
					if (gListDataSpecsPtr->listClassesDataFlag)
								// Without going through all fields to find the maximum number
								// of columns, we will assume the entire image.
						numberValues = (gImageFileInfoPtr->numberColumns - 1 + 
												gListDataSpecsPtr->columnInterval)/
																		gListDataSpecsPtr->columnInterval;
					
					}	// end "if (lineFlag)" 
				
				else	// gListDataSpecsPtr->outputFormatCode == kChannelLineList
					numberValues = (UInt32)gListDataSpecsPtr->numberChannels;
				
						// Now get the maximum number of bytes these values can represent.
						// Allow for the tabs.
							
				numberBytes = numberValues * (dataValueFieldSize+1);
				
						// Now allow for the worst case for inclusion of the line and column
						// values and the class and field codes for the line format case.
						// 8 allows for the longest currently allowed line and column of
						// 200,000 plus a tab character.
							
				numberBytes += 4 * 8;
				
						// Also allow for inclusion of the latitude and longitude for the
						// line format case. 15 allows for current format for listing the
						// latitude and longitude.
				
				numberBytes += 2 * 15;
				
						// The minimum of 64 bytes more than covers the case for inclusion
						// of 'Thematic image data values' for a thematic image window.
						
				numberBytes = MAX (numberBytes, 64);
				gCharBufferPtr1 = (HPtr)MNewPointer (numberBytes);
			
						// Check if out of memory.												
						
				if (gCharBufferPtr1 == NULL)										
					continueFlag = FALSE;
				
				}	// end "if (continueFlag)" 
			
			if (continueFlag)
				continueFlag = CheckListDataTextWindowSpaceNeeded (gListDataSpecsPtr);
			
					// If list data results are to go the a disk file, 				
					// open the	disk file.	Trick 'CreateResultsDiskFiles' to 		
					// estimate the number of bytes on 'pseudo' lines and 			
					// columns.  Assume 128K bytes needed for now.						
			
			if (continueFlag && gListDataSpecsPtr->outputStorageType & 0x0002)
				{
				InitializeAreaDescription (
										&gAreaDescription, 
										1, 
										DetermineBytesForListDataText (gListDataSpecsPtr, FALSE), 
										1, 
										8, 
										1, 
										1,
										1,
										1,
										0);
					
				continueFlag = CreateResultsDiskFiles (
													gListDataSpecsPtr->outputStorageType, 0, 0);
										
				}	// end "if (continueFlag && ..." 
		
 			resultsFileStreamPtr = GetResultsFileStreamPtr (0);
 		
					// Continue list data if everything is okay.							
			
			if (continueFlag)
				{
						// Force text selection to start from end of present text.	
						
				ForceTextToEnd ();
								
				}	// end "if (continueFlag)" 
					
					// List the processor name, date, time and project info.			

			continueFlag = ListHeaderInfo (
											resultsFileStreamPtr,
											kLImageInfo + 2*gListDataSpecsPtr->projectFlag, 
											&gOutputForce1Code, 
											kNoStatisticsUsed, 
											continueFlag);
		
					// Insert a blank line after the table.
			
			continueFlag = OutputString (resultsFileStreamPtr, 
													gEndOfLine, 
													gNumberOfEndOfLineCharacters, 
													gOutputForce1Code, 
													continueFlag);
				
					// List the channels used.													
			
			if (continueFlag && !gImageFileInfoPtr->thematicType)
				continueFlag = ListChannelsUsed (gImageFileInfoPtr,
																NULL,
																(SInt16*)channelsPtr,
																gListDataSpecsPtr->numberChannels, 
																resultsFileStreamPtr, 
																&gOutputForce1Code,
																kNoTransformation);
			
					// List the classes used.													
											
			if (continueFlag && gListDataSpecsPtr->listClassesDataFlag)
				{
				gAreaDescription.lineInterval = gListDataSpecsPtr->lineInterval;
				gAreaDescription.columnInterval = gListDataSpecsPtr->columnInterval;
	
				continueFlag =  ListClassesUsed (NULL,
															gImageFileInfoPtr,
															classPtr, 
															kListNoSymbols,
															kListNoWeights, 
															gListDataSpecsPtr->numberClasses, 
															resultsFileStreamPtr, 
															&gOutputForce1Code,
															FALSE);
		
						// List the fields/areas used.										
							
				if (continueFlag)							
					continueFlag = ListProjectFieldsUsed (gImageFileInfoPtr, 
																		classPtr,
																		gListDataSpecsPtr->numberClasses, 
																		NULL, 
																		gListDataSpecsPtr->fieldTypeCode, 
																		resultsFileStreamPtr, 
																		&gOutputForce1Code,
																		FALSE);
				
				}	// end "if (... && ...->listClassesDataFlag)" 
										
					// List "  Output Information:".
					
			continueFlag = ListSpecifiedStringNumber (kSharedStrID, 
																		IDS_Shared8, 
																		(unsigned char*)gTextString, 
																		resultsFileStreamPtr, 
																		gOutputForce1Code, 
																		continueFlag);
	
					// List the title line.														
			
			if (continueFlag)
				continueFlag = ListDataTitleLine (dataValueFieldSize);
			
			if (gListDataSpecsPtr->includeLatLongValuesFlag)
				{
						// Get the pointer to the map projection information if needed.
					
				mapProjectionHandle = GetFileMapProjectionHandle2 (activeWindowInfoHandle);
		
				mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																		mapProjectionHandle, kLock);
															
				if (mapProjectionInfoPtr == NULL)
					gListDataSpecsPtr->includeLatLongValuesFlag = FALSE;
					
				}	// end "if (gListDataSpecsPtr->includeLatLongValuesFlag)"
					
					// Get dialog box to display list data status.	
					
			showStatusDialogFlag = TRUE;
			//	if (gListDataSpecsPtr->graphDataFlag)
			//		showStatusDialogFlag = FALSE;
			
			gStatusDialogPtr = NULL;
			if (continueFlag && !gListDataSpecsPtr->graphDataFlag)
				gStatusDialogPtr = GetStatusDialog (
														kUpdateStatsInfoID, showStatusDialogFlag);
				
			ShowStatusDialogItemSet (kStatusClassA);
			ShowStatusDialogItemSet (kStatusField);
			ShowStatusDialogItemSet (kStatusLine);
			ShowStatusDialogItemSet (kStatusCommand);
	
					// Get the graph window if needed.										
			
			if (continueFlag && gListDataSpecsPtr->graphDataFlag)
				{
				#if defined multispec_wx
					wxDocument*  graph_doc =
											((CMultiSpecApp*)wxTheApp)->ActivateListDataView ();
				
               graphViewCPtr = ((CMGraphDoc*)graph_doc)->GetGraphViewCPtr ();
               
							// Assign window ID for list data window
				
               ((wxWindow*)graphViewCPtr)->SetId (GR_LISTDATA);
				#endif	// defined multispec_wx

				#if defined multispec_mac
					graphViewCPtr = CreateGraphWindow ();
				#endif	// defined multispec_mac
			
				#if defined multispec_win                
							// open a graph window 
				
					CMultiDocTemplate* graphDocTemplatePtr = 
										((CMultiSpecApp*)AfxGetApp ())->GetGraphDocTemplate ();
					CMGraphDoc* graphDocCPtr = 
									(CMGraphDoc*)graphDocTemplatePtr->OpenDocumentFile (NULL);
					graphViewCPtr = graphDocCPtr->GetGraphViewCPtr ();
				#endif	// defined multispec_win
			
				continueGraphFlag = (graphViewCPtr != NULL);
	
				if (continueGraphFlag)
					continueGraphFlag = graphViewCPtr->FinishGraphRecordSetUp (
														(SInt16*)channelsPtr,
														(SInt32)gListDataSpecsPtr->numberChannels,
														gImageFileInfoPtr->numberChannels,
														1,
														1,
														kIntegerType,
														gImageFileInfoPtr->dataTypeCode);
			
				graphRecordPtr = GetGraphRecordPtr (graphViewCPtr,
																	&handleStatus,
																	&graphRecHandle);

				if (continueGraphFlag && graphRecordPtr != NULL)
					{
					gGraphRecordPtr = graphRecordPtr;
					SetXAxisDescriptionInfo (gGraphRecordPtr,
														gImageWindowInfoPtr,
														gImageFileInfoPtr);
					GetGraphLabels (gGraphRecordPtr);
														
					#if defined multispec_wx	
						gActiveImageWindow->m_frame->Update ();
					#endif	// defined multispec_wx
					
					gGraphRecordPtr->imageWindow = gActiveImageWindow;
						
					DisplayAlert (kErrorAlertID,
										kNoteAlert, 
										kAlertStrID, 
										IDS_Alert109,
										0,
										NULL);
					
					}	// end "if (continueGraphFlag && ..."
				
				else	// !continueGraphFlag || ...
					{
							// Graphing not possible. Turn flag off and list a message.
					
					gListDataSpecsPtr->graphDataFlag = FALSE;
					
					continueFlag = ListSpecifiedStringNumber (kListDataStrID,
																			IDS_ListData15,
																			(unsigned char*)gTextString, 
																			resultsFileStreamPtr, 
																			gOutputCode,
																			TRUE);
					
					if (graphRecordPtr != NULL)
						CloseGraphicsWindow (graphRecordPtr->window);
					
					}	// end "else !continueGraphFlag || ..."
				
				}	// end "if (... && gListDataSpecsPtr->graphDataFlag)" 
				
					// Change cursor to watch cursor until done with process.	
				
			MSetCursor (kWait);
				
			if (continueFlag && gListDataSpecsPtr->listClassesDataFlag)
				{
						// "\pListing Data for Project Classes.");
				LoadDItemStringNumber (kListDataStrID, 
												IDS_ListData1,
												gStatusDialogPtr, 
												IDC_Status11, 
												(Str255*)gTextString);
				
				continueFlag = ListProjectData (fileIOInstructionsPtr, 
															mapProjectionInfoPtr,
															dataValueFieldSize, 
															numberEDecimalDigits,
															gListDataSpecsPtr->numberFDecimalDigits);
				
				}	// end "if (... && gListDataSpecsPtr->listClassesDataFlag" 
				
			if (continueFlag && gListDataSpecsPtr->listSelectedAreaDataFlag)
				{
						// "\pListing Data for Selected Area.");
				LoadDItemStringNumber (kListDataStrID, 
												IDS_ListData2,
												gStatusDialogPtr, 
												IDC_Status11, 
												(Str255*)gTextString);
				HideStatusDialogItemSet (kStatusClassA);
				HideStatusDialogItemSet (kStatusField);
			
				continueFlag = ListFieldData (fileIOInstructionsPtr, 
															mapProjectionInfoPtr,
															dataValueFieldSize, 
															numberEDecimalDigits,
															gListDataSpecsPtr->numberFDecimalDigits, 
															0, 
															-1);
				
				}	// end "... && gListDataSpecsPtr->listSelectedAreaDataFlag" 
				
			if (gListDataSpecsPtr->graphDataFlag && gGraphRecordPtr != NULL)
				{	
				if (gGraphRecordPtr->window != NULL)
					{
					gListDataCode = 2;
      			CloseGraphicsWindow (gGraphRecordPtr->window);
					
					}	// end "if (gGraphRecordPtr->window != NULL)"
      					
      		}	// end "if (gListDataSpecsPtr->graphDataFlag && ..."
			
			gGraphRecordPtr = NULL;
      		
      	CheckAndUnlockHandle (mapProjectionHandle);
				
					// Dispose of the IO buffer.		
										
			DisposeIOBufferPointers (fileIOInstructionsPtr,
												&gInputBufferPtr, 
												&gOutputBufferPtr);
			
					// Dispose of the buffer used to read a line of information 	
					// into.																			
					
			gCharBufferPtr1 = CheckAndDisposePtr (gCharBufferPtr1);
			
					// List a cancel message in the text output window.
					
			if (!continueFlag)	
				continueFlag = ListSpecifiedStringNumber (kListDataStrID,
																		IDS_ListData10, 
																		(unsigned char*)gTextString, 
																		resultsFileStreamPtr, 
																		gOutputCode,
																		TRUE);
			
					// List the CPU time taken for listing the data.		
				
			continueFlag = ListCPUTimeInformation (resultsFileStreamPtr,
																continueFlag, 
																startTime);
			
					// Make certain that the maximum number of characters in a 		
					// line of text takes into account the tab characters.			
		
			gMaxCharsInLine = MAX ((UInt32)gMaxCharsInLine, numberBytes);
	
					// Scroll output window to the selection and adjust the 			
					// scroll bar.																	
			
			UpdateOutputWScrolls (gOutputWindow, 1, kDisplayMessage);
				
					// Dispose of status dialog box.											
			
			CloseStatusDialog (TRUE);
					
					// Close the  results file if needed and								
					// release the memory assigned to it.									
					
			CloseResultsFiles ();
			
					// Change cursor back to pointer.										
						
			MInitCursor ();
			
			//if (graphRecHandle && handleStatus >= 0)
			//	CheckAndUnlockHandle (graphRecHandle);
			
			}	// end "if (continueFlag)" 
			
		}	// end "if (LoadListDataSpecs ())" 
			
				// Unlock the structure handles.	
				
	UnlockImageInformationHandles (activeImageGlobalHandleStatus,
												activeWindowInfoHandle);
	
	if (gListDataSpecsPtr != NULL)
		{		
		CheckAndUnlockHandle (gListDataSpecsPtr->featureHandle);
		CheckAndUnlockHandle (gListDataSpecsPtr->channelsHandle);
		CheckAndUnlockHandle (gListDataSpecsPtr->classHandle);
		CheckAndUnlockHandle (gListDataSpecsPtr->fieldHandle);      
		
		}	// end "if (gListDataSpecsPtr != NULL)" 
		
			// Unlock memory for the list data specifications.							
			
	CheckAndUnlockHandle (gNonProjProcessorSpecs.listDataSpecsH);
	
	gListDataSpecsPtr = NULL;
	
	gImageWindowInfoPtr = NULL;
	gImageFileInfoPtr = NULL;	
	gImageLayerInfoPtr = NULL;	
	
	gThresholdSize = savedThresholdSize;

}	// end "ListDataControl" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ListDataDialog
//
//	Software purpose:	The purpose of this routine is to obtain the
//							user specifications for the listing the image file data.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	True if user selects OK in dialog box
//							False if user selects Cancel in dialog box
//
// Called By:			ListDataControl
//
//	Coded By:			Larry L. Biehl			Date: 05/24/1990
//	Revised By:			Larry L. Biehl			Date: 07/05/2018
	
Boolean ListDataDialog (
				FileInfoPtr							fileInfoPtr)
	
{
	Boolean								returnFlag;

#if defined multispec_mac
	DialogSelectArea					dialogSelectArea;
								
	Rect									theBox;
	
	DialogPtr							dialogPtr = NULL;
	
	SInt16*								localFeaturePtr = NULL;
	
	UInt16*								localClassListPtr = NULL;

	UserItemUPP							drawListDataFormatPopUpPtr;
	
	Handle								okHandle,
											theHandle;
	
	SInt32								theNum;
	
	UInt32								localNumberClasses;
	
	SInt16								entireIconItem,
											index,	
											itemHit,
											selectAreaCode,
											selectItem,
											theType;
											
	UInt16								localNumberChannels,
											numberFDecimalDigits;
	
	Boolean								continueFlag,
											latLongPossibleFlag,
											localGraphDataFlag,
											modalDone;

	
			// Get pointer to memory for temporary storage of channel list.		
	
	localFeaturePtr = (SInt16*)MNewPointer (
								gImageWindowInfoPtr->totalNumberChannels * sizeof (SInt16));
	continueFlag = (localFeaturePtr != NULL);
	
	if (continueFlag && gListDataSpecsPtr->projectFlag)
		{							
				// Get memory for the local class list vector.	
				
		localClassListPtr = (UInt16*)MNewPointer (
					(UInt32)gProjectInfoPtr->numberStatisticsClasses * sizeof (UInt16));
					
		continueFlag = (localClassListPtr != NULL);
					
		}	// end "if (localFeaturePtr != NULL && ..."
	
			// Get the modal dialog for the list data specification.					
	
	if (continueFlag)			
		dialogPtr = LoadRequestedDialog (kListDataSpecificationID, kCopyScrap, 1, 2);
		
	if (dialogPtr == NULL)
		{			
		CheckAndDisposePtr ((Ptr)localFeaturePtr);	
		CheckAndDisposePtr ((Ptr)localClassListPtr);									
																							return (FALSE);
																				
		}	// end "if (dialogPtr == NULL)"
				
			// Initialize selected area structure.		
			
	entireIconItem = 11;
	if (gAppearanceManagerFlag)
		entireIconItem = 37;
	
	InitializeDialogSelectArea (&dialogSelectArea,
											gImageWindowInfoPtr,
											gActiveImageWindow,
											gListDataSpecsPtr->columnStart,
											gListDataSpecsPtr->columnEnd,
											gListDataSpecsPtr->columnInterval,
											gListDataSpecsPtr->lineStart,
											gListDataSpecsPtr->lineEnd,
											gListDataSpecsPtr->lineInterval,
											13,
											entireIconItem,
											kDontAdjustToBaseImage);
											
			// Intialize local user item proc pointers.	
			
	drawListDataFormatPopUpPtr = NewUserItemUPP (DrawListDataFormatPopUp);
	
			// Set Procedure pointers for those dialog items that need them.
				
	if (gListDataSpecsPtr->projectFlag)
		SetDialogItemDrawRoutine (dialogPtr, 5, gDrawDialogClassPopUpPtr); 
		
	SetDialogItemDrawRoutine (dialogPtr, 20, gDrawChannelsPopUpPtr);
	
			// Set routine to draw outline on "OK" box.									
			
	SetDialogItemDrawRoutine (dialogPtr, 26, gHiliteOKButtonPtr);
	
			// Set routine to draw list data format popup.									
			
	SetDialogItemDrawRoutine (dialogPtr, 33, drawListDataFormatPopUpPtr);
	
			// Save handle for the OK button for use later.								
			
	GetDialogItem (dialogPtr, 1,  &theType, &okHandle, &theBox);
		
			// Update the modal dialog with the default settings						
			// Set check box for "Classes".													
			// items = 4 & 5: Classes to use.												
				
	SetDLogControl (dialogPtr, 4, gListDataSpecsPtr->listClassesDataFlag);
				
	if (!gListDataSpecsPtr->projectFlag)
		HideDialogItem (dialogPtr, 4);
	
	gClassSelection = gListDataSpecsPtr->classSet;
	localNumberClasses = gListDataSpecsPtr->numberClasses; 
	
	if (gListDataSpecsPtr->projectFlag)
		{
		UInt16* classPtr = (UInt16*)GetHandlePointer (gListDataSpecsPtr->classHandle);
									 
		for (index=0; index<localNumberClasses; index++)
			localClassListPtr[index] = classPtr[index]; 
			
		}	// end "if (gListDataSpecsPtr->projectFlag)" 	
		
	selectAreaCode = 0;
	if (gListDataSpecsPtr->listClassesDataFlag)
		{
		selectAreaCode += 1;
		ShowDialogItem (dialogPtr, 5);
		HideDialogItems (dialogPtr, 6, 7);
		ShowDialogItem (dialogPtr, 22);
		ShowDialogItems (dialogPtr, 29, 30);
		
		}	// end "if (gListDataSpecsPtr->listClassesDataFlag)" 
		
	else	// !gListDataSpecsPtr->listClassesDataFlag 
		{
		HideDialogItems (dialogPtr, 5, 7);
		HideDialogItem (dialogPtr, 22);
		HideDialogItems (dialogPtr, 29, 30);
		
		}	// end "else !gListDataSpecsPtr->listClassesDataFlag" 

			// To entire image icon.															
			//	Reformat area																		
			
	LoadLineColumnItems (&dialogSelectArea, 
									dialogPtr, 
									kInitializeLineColumnValues, 
									kIntervalEditBoxesExist,
									1);

			// Set check box for area.															

	if (gAppearanceManagerFlag)
		HideDialogItem (dialogPtr, 11);
	else	// !gAppearanceManagerFlag
		HideDialogItem (dialogPtr, 37);
	
	if (gListDataSpecsPtr->listSelectedAreaDataFlag)
		{
		SetDLogControl (dialogPtr, 8, 1);
		ShowDialogItem (dialogPtr, 9);
		ShowDialogItem (dialogPtr, entireIconItem);
		ShowDialogItem (dialogPtr, 13);
		ShowDialogItem (dialogPtr, 14);
		ShowDialogItem (dialogPtr, 16);
		ShowDialogItem (dialogPtr, 17);
		
		selectItem = 13;
		selectAreaCode += 2;
		
		}	// end "if (gListDataSpecsPtr->listSelectedAreaDataFlag)" 
					
	else	// !gListDataSpecsPtr->listSelectedAreaDataFlag 
		{
		SetDLogControl (dialogPtr, 8, 0);
		HideDialogItem (dialogPtr, 9);
		HideDialogItem (dialogPtr, entireIconItem);
		HideDialogItem (dialogPtr, 13);
		HideDialogItem (dialogPtr, 14);
		HideDialogItem (dialogPtr, 16);
		HideDialogItem (dialogPtr, 17);
		
		selectItem = 15;
		
		}	// end "else !gListDataSpecsPtr->listSelectedAreaDataFlag" 
	
			//	Get the rectangles for the channel prompt and popup boxes.			
			//	Make all channels the default		
	
	localNumberChannels = gListDataSpecsPtr->numberChannels;
	SInt16* featurePtr = (SInt16*)GetHandlePointer (
															gListDataSpecsPtr->featureHandle);
						
	for (index=0; index<localNumberChannels; index++)
		localFeaturePtr[index] = featurePtr[index]; 											
	
	gChannelSelection = gListDataSpecsPtr->channelSet;
	
	if (fileInfoPtr->thematicType)
		HideDialogItems (dialogPtr, 19, 20);
		
	else	// !fileInfoPtr->thematicType 
		ShowDialogItems (dialogPtr, 19, 20);
	
			// Set check box for "Inlude line and column values".		
			
	SetDLogControl (dialogPtr, 21, gListDataSpecsPtr->includeLineColumnValuesFlag);
	
			// Set check box for "Include latitude and longitude values".
			
	if (DetermineIfLatLongPossible (gListDataSpecsPtr->windowInfoHandle))
		{
		latLongPossibleFlag = TRUE;
		ShowDialogItem (dialogPtr, 34);
		SetDLogControl (dialogPtr, 34, gListDataSpecsPtr->includeLatLongValuesFlag);
		
		}	// end "if (DetermineIfLatLongPossible (...->windowInfoHandle))"
		
	else	// !DetermineIfLatLongPossible (...->windowInfoHandle)
		{
		latLongPossibleFlag = FALSE;
		HideDialogItem (dialogPtr, 34);
		SetDLogControl (dialogPtr, 34, 0);
		
		}	// end "else !DetermineIfLatLongPossible (...->windowInfoHandle))"
	
			// Set check box for "Inlude class and field codes".			
			
	SetDLogControl (dialogPtr, 22, gListDataSpecsPtr->includeClassFieldFlag);
	
			// Set check box for "text output window".						
			
	SetDLogControl (dialogPtr, 24, (gListDataSpecsPtr->outputStorageType & 0x0001));
	
			// Set check box for "disk file".									
	
	if (gListDataSpecsPtr->outputStorageType & 0x0002)
		SetDLogControl (dialogPtr, 25, 1);
	else	// !(gListDataSpecsPtr->outputStorageType & 0x0002)
		SetDLogControl (dialogPtr, 25, 0);
	
			// Set control for including training fields.								
			
	SetDLogControl (dialogPtr, 29, gListDataSpecsPtr->fieldTypeCode & kTrainingType);
	
			// Set control for including test fields.										
			
	SetDLogControl (dialogPtr, 30, gListDataSpecsPtr->fieldTypeCode & kTestingType);
	if (gProjectInfoPtr && gProjectInfoPtr->numberStatTestFields <= 0)
		{                
		SetDLogControl (dialogPtr, 30, 0);
		SetDLogControlHilite (dialogPtr, 30, 255);
		
		}	// end "if (gProjectInfoPtr && ..."
	
			// Set check box for "Graph data values".						
	
	localGraphDataFlag = gListDataSpecsPtr->graphDataFlag;
	if (gNumberOfGWindows < kMaxNumberGWindows &&
															gChannelSelection == kAllMenuItem)
		SetDLogControl (dialogPtr, 31, gListDataSpecsPtr->graphDataFlag);
		
	else	// gNumberOfGWindows >= kMaxNumberGWindows || ...
		{
		SetDLogControl (dialogPtr, 31, 0);
		SetDLogControlHilite (dialogPtr, 31, 255);
		
		}	// end "else gNumberOfGWindows >= kMaxNumberGWindows"
		
	numberFDecimalDigits = gListDataSpecsPtr->numberFDecimalDigits;
	if (gImageFileInfoPtr->dataTypeCode == kIntegerType)
		{
		HideDialogItem (dialogPtr, 35);
		HideDialogItem (dialogPtr, 36);
				
		}	// end "if (gImageFileInfoPtr->dataTypeCode == kIntegerType)"
		
	else	// gImageFileInfoPtr->dataTypeCode == kRealType
		{
		ShowDialogItem (dialogPtr, 35);
		ShowDialogItem (dialogPtr, 36);
		LoadDItemValue (dialogPtr, 36, numberFDecimalDigits);
		
		}	// end "else gImageFileInfoPtr->dataTypeCode == kRealType"
		
			// List data format.
			
	gListDataFormatCode = gListDataSpecsPtr->outputFormatCode;
		
			// Center the dialog and then show it.											
			
	ShowDialogWindow (dialogPtr, kListDataSpecificationID, kSetUpDFilterTable);
	
			// Set default text selection to first edit text item						
			
	SelectDialogItemText (dialogPtr, selectItem, 0, INT16_MAX);
	modalDone = FALSE;
	itemHit = 0;
	do
		{
		ModalDialog (gProcessorDialogFilterPtr, &itemHit);
	
				// If itemHit was a number item, check for bad values.  If			
				// itemHit was a radio button make appropriate control settings	
				// to indicate to the user the present selection.  Get the handle	
				// to the itemHit.  For number value items, get the string and		
				// convert it to a number.														

		GetDialogItem (dialogPtr, itemHit, &theType, &theHandle, &theBox);
		if (theType == 16)
			{
			GetDialogItemText (theHandle, gTextString);	
			StringToNum (gTextString, &theNum);
			
			}	// end "if (theType == 16)" 

		if (itemHit > 2)
			{
			switch (itemHit)
				{
				case 4:		// check button for "Classes" 
					if (GetControlValue ((ControlHandle)theHandle))
						{
						SetControlValue ((ControlHandle)theHandle, 0);
						HideDialogItems (dialogPtr, 5, 7);
						HideDialogItem (dialogPtr, 22);
						HideDialogItems (dialogPtr, 29, 30);
						
						selectAreaCode -= 1;
						
						}	// end "if (GetControlValue ((ControlHandle)..." 
						
					else	// !GetControlValue ((ControlHandle)theHandle) 
						{
						SetControlValue ((ControlHandle)theHandle, 1);
						ShowDialogItem (dialogPtr, 5);
						HideDialogItem (dialogPtr, 6);
						HideDialogItem (dialogPtr, 7);
						ShowDialogItem (dialogPtr, 22);
						ShowDialogItem (dialogPtr, 29);
						ShowDialogItem (dialogPtr, 30);
						
						selectAreaCode += 1;
						
						}	// end "!GetControlValue ((ControlHandle)theHandle)" 
					break;
					
				case 5:		// Classes 
					itemHit = StandardPopUpMenu (dialogPtr, 
															4, 
															5, 
															gPopUpAllSubsetMenu, 
															gClassSelection, 
															kPopUpAllSubsetMenuID);
													
					if (itemHit == kSubsetMenuItem)
						{
								// Subset of classes to be used.								
								
						itemHit = ClassDialog (&localNumberClasses,
														(SInt16*)localClassListPtr,
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
					
				case 7:
					break;
					
				case 8:		// List data within selected area. 
					if (GetControlValue ((ControlHandle)theHandle))
						{
						SetControlValue ((ControlHandle)theHandle, 0);
						HideDialogItem (dialogPtr, 9);
						HideDialogItem (dialogPtr, entireIconItem);
						HideDialogItem (dialogPtr, 13);
						HideDialogItem (dialogPtr, 14);
						HideDialogItem (dialogPtr, 16);
						HideDialogItem (dialogPtr, 17);
						
						selectAreaCode -= 2;
						
						}	// end "if (GetControlValue ((ControlHandle)..." 
						
					else	// !GetControlValue ((ControlHandle)theHandle) 
						{
						SetControlValue ((ControlHandle)theHandle, 1);
						ShowDialogItem (dialogPtr, 9);
						ShowDialogItem (dialogPtr, entireIconItem);
						ShowDialogItem (dialogPtr, 13);
						ShowDialogItem (dialogPtr, 14);
						ShowDialogItem (dialogPtr, 16);
						ShowDialogItem (dialogPtr, 17);
						SelectDialogItemText (dialogPtr, 13, 0, INT16_MAX);
						
						selectAreaCode += 2;
						
						}	// end "!GetControlValue ((ControlHandle)theHandle)" 
					break;
					
				case 11:				// Entire area to selected area switch.	
				case 37:				// Entire area to selected area switch. (Appearance Manager)		
				case 13:				//	 lineStart  
				case 14:				//	 lineEnd  
				case 15:				//	 lineInterval  	
				case 16:				//	 columnStart  
				case 17:				//	 columnEnd  
				case 18:				//	 columnInterval  	
					DialogLineColumnHits (&dialogSelectArea,
													dialogPtr, 
													itemHit,
													theHandle,
													theNum);
					break;
					
				case 20:		// Channels 
					itemHit = StandardPopUpMenu (dialogPtr, 
															19, 
															20, 
															gPopUpAllSubsetMenu, 
															gChannelSelection, 
															kPopUpAllSubsetMenuID);
													
					if (itemHit == kSubsetMenuItem)
						{
								// Subset of channels to be used.							
								
						HiliteControl ((ControlHandle)okHandle, 255);
						ChannelsDialog ((SInt16*)&localNumberChannels,
												localFeaturePtr,
												gImageLayerInfoPtr,
												fileInfoPtr,
												1,
												kNoTransformation,
												NULL,
												gImageWindowInfoPtr->totalNumberChannels,
												gChannelSelection);
						HiliteControl ((ControlHandle)okHandle, 0);
												
						if (localNumberChannels == gImageWindowInfoPtr->totalNumberChannels)
							itemHit = kAllMenuItem;
							
						}	// end "if (itemHit == kSubsetMenuItem)" 
					
					if (itemHit != 0)
						gChannelSelection = itemHit;
	
					if (gNumberOfGWindows < kMaxNumberGWindows &&
																	gChannelSelection == kAllMenuItem)
						{
						SetDLogControl (dialogPtr, 31, localGraphDataFlag);
						SetDLogControlHilite (dialogPtr, 31, 0);
						
						}	// end "if (gChannelSelection == kAllMenuItem)"
					
					else	// gChannelSelection == kSubsetMenuItem || ...
						{
						SetDLogControl (dialogPtr, 31, 0);
						SetDLogControlHilite (dialogPtr, 31, 255);
						
						}	// end "else gChannelSelection == kSubsetMenuItem"
	
							// Make certain that the correct label is drawn in the	
							// channel pop up box.												
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
					
				case 21:		// check box for "include line and column values." 
				case 22:		// check box for "include class and field codes." 
				case 24:		// check box for "write results to output window." 
				case 25:		// check box for "write results to disk file." 
				case 29:		// check box for "training fields" 
				case 30:		// check box for "test fields"
				case 34:		// check box for "include latitude and longitude values." 
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					break;
					
				case 31:		// check box for "graphing data"
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					localGraphDataFlag = !localGraphDataFlag;
					break;
					
				case 33:		// List data format code 
					itemHit = StandardPopUpMenu (dialogPtr, 
															32, 
															33, 
															gPopUpListDataFormatMenu, 
															gListDataFormatCode, 
															kPopUpListDataFormatMenuID);
													
					if (itemHit != 0)
						{
						gListDataFormatCode = itemHit;
						
						if (gListDataFormatCode == 1 && latLongPossibleFlag)
							ShowDialogItem (dialogPtr, 34);
						
						else	// gListDataFormatCode == 2 || !latLongPossibleFlag
							HideDialogItem (dialogPtr, 34);
						
						}	// end "if (itemHit != 0)"
	
							// Make certain that the correct label is drawn in the	
							// list data format pop up box.													
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
					
				case 36:		// number decimal places to use for f formated real data.
					if (theNum < 0 || theNum > 9)
						NumberErrorAlert ((SInt16)numberFDecimalDigits, 
												dialogPtr, 
												itemHit);
												
					else
						numberFDecimalDigits = theNum;
					break;
						
				}	// end "switch (itemHit)"
			/*
			if (selectAreaCode == 0)
				HiliteControl ((ControlHandle)okHandle, 255);
				
			else	// selectAreaCode != 0 
				HiliteControl ((ControlHandle)okHandle, 0);
			*/
			}	// end "if (itemHit > 2)"
			
		else	// itemHit <= 2 
			{	
					// Check if area to be selected has been listed.
					
			if (itemHit == 1 && selectAreaCode == 0)
				{
				HiliteControl ((ControlHandle)okHandle, 255);
				DisplayAlert (kErrorAlertID, 
									kStopAlert, 
									kAlertStrID, 
									IDS_Alert116,
									0, 
									NULL);
				HiliteControl ((ControlHandle)okHandle, 0);
									
				itemHit = 0;
				//updateDialogWindowFlag = TRUE;
										
				}	// end "if (itemHit == 1 && selectAreaCode == 0)"
				
					// If item hit is 1, check if line-column values make 			
					// sense.  If they don't, sound an alert and make item hit 		
					// equal to 0 to allow user to make changes.							
					
			if (itemHit == 1 && GetDLogControl (dialogPtr, 8) == 1)
				itemHit = CheckLineColValues (&dialogSelectArea,
															gListDataSpecsPtr->lineStart,
															gListDataSpecsPtr->columnStart,
															dialogPtr);
				
			if (itemHit == 1)
				{
				modalDone = TRUE;    
				
				ListDataDialogOK (gListDataSpecsPtr,
										GetDLogControl (dialogPtr, 4),
										gClassSelection,
										localNumberClasses,
										localClassListPtr,
										GetDLogControl (dialogPtr, 8),
										&dialogSelectArea,
										gChannelSelection,
										localNumberChannels,
										(UInt16*)localFeaturePtr,
										GetDLogControl (dialogPtr, 21),
										GetDLogControl (dialogPtr, 34),
										GetDLogControl (dialogPtr, 22),
										GetDLogControl (dialogPtr, 24),
										GetDLogControl (dialogPtr, 25),
										GetDLogControl (dialogPtr, 29),
										GetDLogControl (dialogPtr, 30),
										GetDLogControl (dialogPtr, 31),
										numberFDecimalDigits,
										gListDataFormatCode);
				
				returnFlag = TRUE;

				}	// end if (itemHit == 1) 
			
			if (itemHit == 2)
				{
				modalDone = TRUE;
				returnFlag = FALSE;
				
				}	// end "if (itemHit == 2)" 
				
			}	// end "else itemHit <= 2" 
				
		}	while (!modalDone);
		
	DisposeUserItemUPP (drawListDataFormatPopUpPtr);
		
	CheckAndDisposePtr ((Ptr)localFeaturePtr);	
	CheckAndDisposePtr ((Ptr)localClassListPtr);	
		
	CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
#endif	// defined multispec_mac		
	
	#if defined multispec_win
		CMListDataDialog*		dialogPtr = NULL;
		
		TRY
			{ 
			dialogPtr = new CMListDataDialog ();
			
			returnFlag = dialogPtr->DoDialog (); 
		                       
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
		CMListDataDialog* dialogPtr = NULL;

		dialogPtr = new CMListDataDialog (NULL);

		returnFlag = dialogPtr->DoDialog ();

		delete dialogPtr;
	#endif
	
	return (returnFlag);

}	// end "ListDataDialog"  	


		
void ListDataDialogOK (
				ListDataSpecsPtr					listDataSpecsPtr,
				Boolean								classFlag,
				SInt16								classSelection,
				UInt32								localNumberClasses,
				UInt16*								localClassPtr,
				Boolean								areaFlag,
				DialogSelectArea*					dialogSelectAreaPtr,
				SInt16								channelSelection,
				UInt16								localNumberChannels,
				UInt16*								localChannelPtr,
				Boolean								includeLineColumnFlag,
				Boolean								includeLatLongFlag,
				Boolean								includeClassFieldFlag,
				Boolean								textWindowFlag,
				Boolean								diskFileFlag,
				Boolean								trainingFlag,
				Boolean								testFlag,
				Boolean								graphDataFlag,
				UInt16								numberFDecimalDigits,
				SInt16								listDataFormatCode)
				
{	
	UInt32								index;
	
				
			// List requested data for classes.						
				
	listDataSpecsPtr->listClassesDataFlag = classFlag; 
					
			// Classes														
				
	listDataSpecsPtr->classSet = classSelection; 
				                               
	SInt16* classPtr = (SInt16*)GetHandlePointer (listDataSpecsPtr->classHandle);
				
	if (listDataSpecsPtr->listClassesDataFlag)
		{
		if (listDataSpecsPtr->classSet == kAllMenuItem)
			LoadClassVector (&listDataSpecsPtr->numberClasses,
										(SInt16*)classPtr);
											
		else	// ...->classSet == kSubsetMenuItem
			{                                              
			listDataSpecsPtr->numberClasses = (UInt16)localNumberClasses;
			for (index=0; index<localNumberClasses; index++)
				classPtr[index] = localClassPtr[index]; 
			
			}	// end "else ...->classSet == kSubsetMenuItem" 
			
		}	// end "if (gListDataSpecsPtr->listClassesDataFlag)"
													
			// List requested data for selected area.				
				
	listDataSpecsPtr->listSelectedAreaDataFlag = areaFlag; 
																
			// Selected list data area.											
				
	listDataSpecsPtr->lineStart = dialogSelectAreaPtr->lineStart;
	listDataSpecsPtr->lineEnd = dialogSelectAreaPtr->lineEnd;
	listDataSpecsPtr->lineInterval = dialogSelectAreaPtr->lineInterval;
	listDataSpecsPtr->columnStart = dialogSelectAreaPtr->columnStart;
	listDataSpecsPtr->columnEnd = dialogSelectAreaPtr->columnEnd;
	listDataSpecsPtr->columnInterval = dialogSelectAreaPtr->columnInterval;
					
			// Channels

	listDataSpecsPtr->channelSet = channelSelection; 											
				                                                  
	SInt16* featurePtr = (SInt16*)GetHandlePointer (listDataSpecsPtr->featureHandle);
						
	if (listDataSpecsPtr->channelSet == kAllMenuItem)		// All channels 
		LoadFeatureVector (kNoTransformation,
									(SInt16*)&listDataSpecsPtr->numberChannels,
									featurePtr,
									gImageWindowInfoPtr->totalNumberChannels);
	
	else	// listDataSpecsPtr->channelSet == kSubsetMenuItem 
		{				    		
		listDataSpecsPtr->numberChannels = (UInt16)localNumberChannels; 
		for (index=0; index<localNumberChannels; index++)
			featurePtr[index] = localChannelPtr[index];
												
		}	// end "else listDataSpecsPtr->channelSet == kSubsetMenuItem" 
					
			// Load the list data channels vector to be used by GetLineOfData.	
									
	SInt16* channelsPtr = (SInt16*)GetHandlePointer (listDataSpecsPtr->channelsHandle);
															
	LoadChannelsVector (kNoProject,
								kNoTransformation,
								listDataSpecsPtr->numberChannels, 
								featurePtr, 
								(SInt16*)&listDataSpecsPtr->numberChannels,
								channelsPtr);
	
			// Include line and column values.						
				
	listDataSpecsPtr->includeLineColumnValuesFlag = includeLineColumnFlag; 
	
			// Include latitude and longitude values.
			
	listDataSpecsPtr->includeLatLongValuesFlag = includeLatLongFlag;
				
	listDataSpecsPtr->latLongUnitsCode = kDecimalLatLongUnitsMenuItem;
	if (GetCoordinateViewUnits (
							listDataSpecsPtr->windowInfoHandle) == kDMSLatLongUnitsMenuItem)
		listDataSpecsPtr->latLongUnitsCode = kDMSLatLongUnitsMenuItem;
													
			// Include class and field codes.						
				
	listDataSpecsPtr->includeClassFieldFlag = FALSE;
	if (listDataSpecsPtr->listClassesDataFlag)
		listDataSpecsPtr->includeClassFieldFlag = includeClassFieldFlag; 
													
			// Write output to output text window.					
						
	listDataSpecsPtr->outputStorageType = 0;							
	if (textWindowFlag)				
		listDataSpecsPtr->outputStorageType += 1;
													
			// Write output to disk file.								
				
	if (diskFileFlag)			
		listDataSpecsPtr->outputStorageType += 2;
													
			// Training and test field flags.				
					
	listDataSpecsPtr->fieldTypeCode = 0;
	if (trainingFlag)
		listDataSpecsPtr->fieldTypeCode += kTrainingType;
	if (testFlag)
		listDataSpecsPtr->fieldTypeCode += kTestingType;
													
			// Graph data flag.											
					
	listDataSpecsPtr->graphDataFlag = graphDataFlag;
	
			// If data will be graphed, create the channel wavelength or vector in case
			// it is needed.
			// This capability was not fully implemented on 7/2/2018. It does not
			// seem to be worth the effort at this time to be able to handle
			// out of order wavelengths for subsets of channels for graphing. One
			// will also need to be able to handle this for linked files if it is
			// implemented.
	/*
	if (graphDataFlag)
		CreateChannelWavelengthOrderSubset (gImageWindowInfoPtr,
														gImageFileInfoPtr,
														channelsPtr,
														listDataSpecsPtr->numberChannels);
	*/
			// List data format.
			
	listDataSpecsPtr->outputFormatCode = listDataFormatCode;
	
			// Number of decimal digits to use for real type data.
			
	listDataSpecsPtr->numberFDecimalDigits = numberFDecimalDigits;
				
}	// end "ListDataDialogOK" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ListDataTitleLine ()
//
//	Software purpose:	The purpose of this routine is to list the title
//							lines for the List Data table to the output
//							text window or disk file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date:  6/07/1990
//	Revised By:			Larry L. Biehl			Date: 12/12/2007	

Boolean ListDataTitleLine (
				SInt16								dataValueFieldSize)
				
{
	CMFileStream*						resultsFileStreamPtr;
	char*									tempCharBufferPtr;
	SInt16*								channelsPtr;
	
	SInt16								dataValueFieldIndexSkip,
											lineColumnValueFieldSize;
									
	UInt16								channelIndex,
											stringIndex;
	
	Boolean								continueFlag;
	
	
			// Check variables.																	
			
	if (gCharBufferPtr1 == NULL)
																				return (FALSE);

			// Initialize local variables.													
	
	continueFlag = TRUE;
	
	channelsPtr = (SInt16*)GetHandlePointer (gListDataSpecsPtr->channelsHandle);
										
	dataValueFieldIndexSkip = dataValueFieldSize + 1;
	resultsFileStreamPtr = GetResultsFileStreamPtr (0);
	lineColumnValueFieldSize = MAX (
						GetStringLengthForNumber (gImageWindowInfoPtr->maxNumberLines),
						GetStringLengthForNumber (gImageWindowInfoPtr->maxNumberColumns));
			
	lineColumnValueFieldSize = MAX (4, lineColumnValueFieldSize);
	
			// Label for the table.											
	
	stringIndex = IDS_ListData8;		// " Multispectral Image Data Values\r"
	if (gImageFileInfoPtr->thematicType)
		stringIndex = IDS_ListData3;		// "\r Thematic Image Data Values\r"
		
	continueFlag = ListSpecifiedStringNumber (kListDataStrID,
															(SInt16)stringIndex,
															(unsigned char*)gTextString, 
															resultsFileStreamPtr, 
															gOutputCode,
															continueFlag);
	
	if (gListDataSpecsPtr->outputFormatCode == kChannelLineList)
		{										
		tempCharBufferPtr = gCharBufferPtr1;
	
				// Skip a line before the first title line.									
				
		sprintf (tempCharBufferPtr, "%s    ", gEndOfLine);
		tempCharBufferPtr += gNumberOfEndOfLineCharacters + 4;
	
				// Include the title for the lines and columns if needed.				
		
		if (gListDataSpecsPtr->includeLineColumnValuesFlag)
			{
			stringIndex = IDS_ListData5;
			if (lineColumnValueFieldSize == 5)
				stringIndex = IDS_ListData6;
			else if (lineColumnValueFieldSize == 6)
				stringIndex = IDS_ListData7;
			
			continueFlag = MGetString (gTextString, kListDataStrID, stringIndex);
			if (continueFlag)
				sprintf (tempCharBufferPtr, 
							"%s", 
							&gTextString[1]);
			
			tempCharBufferPtr += gTextString[0];
			
			}	// end "if (gListDataSpecsPtr->includeLineColumnValuesFlag)" 
		
				// Include the title for the latitude and longitude values.				
		
		if (gListDataSpecsPtr->includeLatLongValuesFlag)
			{
			stringIndex = IDS_ListData11;
			if (gListDataSpecsPtr->latLongUnitsCode == kDMSLatLongUnitsMenuItem)
				stringIndex = IDS_ListData13;
			
			continueFlag = MGetString (gTextString, kListDataStrID, stringIndex);
			if (continueFlag)
				sprintf (tempCharBufferPtr, 
							"%s", 
							&gTextString[1]);
			
			tempCharBufferPtr += gTextString[0];
			
			}	// end "if (gListDataSpecsPtr->includeLatLongValuesFlag)"
			
				// Include the title for the class and field code if needed.			
		
		if (gListDataSpecsPtr->includeClassFieldFlag)
			{
			sprintf (tempCharBufferPtr, "Clas\tFiel\t");
			tempCharBufferPtr += 10;
								
			}	// end "if (gListDataSpecsPtr->includeClassFieldFlag)" 											
		
		if (gImageFileInfoPtr->thematicType)
			{
					// Include a carriage return for thematic images.								
				
			sprintf (tempCharBufferPtr, "%s", gEndOfLine);
			tempCharBufferPtr += gNumberOfEndOfLineCharacters;
							
			}	// end "if (gImageFileInfoPtr->thematicType)"										
		
		else	// !gImageFileInfoPtr->thematicType
			{
					// Include the title for the channels if multispectral type. A
					// carriage return is part of this string.
					// ' Channels\r'
				
			continueFlag = MGetString (gTextString, kListDataStrID, IDS_ListData4);
			if (continueFlag)
				sprintf (tempCharBufferPtr, 
							"%s", 
							&gTextString[1]);
							
			}	// end "else	// !gImageFileInfoPtr->thematicType"
			
				// List the line.																		
				
		continueFlag = OutputString (resultsFileStreamPtr, 
												gCharBufferPtr1, 
												0, 
												gOutputCode,
												continueFlag);
			
				// If everything is okay, list the 2nd title line.							
				
		if (continueFlag)
			{			
					// Insert four blanks at beginning of the line.
					
			tempCharBufferPtr = gCharBufferPtr1;
			sprintf (tempCharBufferPtr, "    ");
			tempCharBufferPtr += 4;
		
					// Include the title for the lines and columns if needed.			
			
			if (gListDataSpecsPtr->includeLineColumnValuesFlag)
				{
				sprintf (tempCharBufferPtr, "      ");
				tempCharBufferPtr += lineColumnValueFieldSize;
				
				sprintf (tempCharBufferPtr, "\t      ");
				tempCharBufferPtr += lineColumnValueFieldSize + 1;
				
				sprintf (tempCharBufferPtr, "\t");
				tempCharBufferPtr++;
				
				}	// end "if (gListDataSpecsPtr->includeLineColumnValuesFlag)" 
		
						// Include blanks for 2nd line for latitude and longitude values.				
				
				if (gListDataSpecsPtr->includeLatLongValuesFlag)
					{
					stringIndex = IDS_ListData12;
					if (gListDataSpecsPtr->latLongUnitsCode == kDMSLatLongUnitsMenuItem)
						stringIndex = IDS_ListData14;
			
					continueFlag = MGetString (gTextString, kListDataStrID, stringIndex);
					if (continueFlag)
						sprintf (tempCharBufferPtr, 
									"%s", 
									&gTextString[1]);
					
					tempCharBufferPtr += gTextString[0];
					
					}	// end "if (gListDataSpecsPtr->includeLatLongValuesFlag)"
			
					// Include the title for the class and field code if needed.		
			
			if (gListDataSpecsPtr->includeClassFieldFlag)
				{
				sprintf (tempCharBufferPtr, "Code\tCode\t");
				tempCharBufferPtr += 10;
									
				}	// end "if (gListDataSpecsPtr->includeClassFieldFlag)" 
			
					// Include the channel numbers.												
				
			if (!gImageFileInfoPtr->thematicType)
				{
				for (channelIndex=0; 
					channelIndex<gListDataSpecsPtr->numberChannels; 
					channelIndex++)
					{
					sprintf (tempCharBufferPtr, 
									"%*d\t",
									dataValueFieldSize, 
									(int)(channelsPtr[channelIndex]+1));
					tempCharBufferPtr += dataValueFieldIndexSkip;
			
					}	// end "for (channelNum=1; channelNum<=..." 
					
				}	// end "if (!gImageFileInfoPtr->thematicType)" 
						
					// Add the carriage return in place of the last tab if one exists.					
			
			if (tempCharBufferPtr != gCharBufferPtr1)
				tempCharBufferPtr--;
															
			sprintf (tempCharBufferPtr, "%s", gEndOfLine);
			
					// List the line.																	
					
			continueFlag = OutputString (resultsFileStreamPtr, 
													gCharBufferPtr1, 
													0, 
													gOutputCode, 
													continueFlag);
		
			}	// end "if (continueFlag)" 
													
		}	// end "if (gListDataSpecsPtr->outputFormatCode == kChannelLineList)"

	return (continueFlag);	
		
}	// end "ListDataTitleLine" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ListDataTitleLine2
//
//	Software purpose:	The purpose of this routine is to list the title
//							lines for the List Data table to the output
//							text window or disk file. This routine is for the title lines
//							for column type lists.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 10/07/1998
//	Revised By:			Larry L. Biehl			Date: 11/30/2000	

Boolean ListDataTitleLine2 (
				SInt16								dataValueFieldSize,
				SInt16								classNumberCode, 
				SInt16								fieldNumberCode)
				
{
	CMFileStream*						resultsFileStreamPtr;
	char*									tempCharBufferPtr;
	
	SInt16								lineFieldSize;
	
	Boolean								continueFlag;
	
	
			// Check variables.																	
			
	if (gCharBufferPtr1 == NULL)
																				return (FALSE);
	
	if (gListDataSpecsPtr->outputFormatCode != kChannelColumnList)
																				return (TRUE);

			// Initialize local variables.													
	
	continueFlag = TRUE;
	resultsFileStreamPtr = GetResultsFileStreamPtr (0);
			
	lineFieldSize = MAX (4,
							GetStringLengthForNumber (gImageWindowInfoPtr->maxNumberLines));	
	
			// Skip a line before the first title line.									
			
	sprintf (gCharBufferPtr1, "%s", gEndOfLine);
	tempCharBufferPtr = gCharBufferPtr1;
	tempCharBufferPtr += gNumberOfEndOfLineCharacters;
	
			// Include the title for the class and field code if needed.			
	
	if (gListDataSpecsPtr->includeClassFieldFlag)
		{
		sprintf (tempCharBufferPtr, 
						"    Class %4hd\tField %4hd%s", 
						classNumberCode,
						fieldNumberCode,
						gEndOfLine);
		tempCharBufferPtr += 25 + gNumberOfEndOfLineCharacters;
							
		}	// end "if (gListDataSpecsPtr->includeClassFieldFlag)" 
		
			// Include the title for the lines and columns if needed.
	
	if (gListDataSpecsPtr->includeLineColumnValuesFlag)
		{
				// Put the Line title in based on the length of the line numbers.
				
		continueFlag = MGetString (gTextString, kListDataStrID, 9);
		if (continueFlag)
			sprintf (tempCharBufferPtr, "    %s", &gTextString[1]);
		
		tempCharBufferPtr += lineFieldSize + 5;
		
				// Now put the rest of the title line in.
			
		if (gImageFileInfoPtr->thematicType || gListDataSpecsPtr->numberChannels == 1)
			{
			sprintf (tempCharBufferPtr, "Columns%s", gEndOfLine);
			tempCharBufferPtr += 7 + gNumberOfEndOfLineCharacters;
			
			}	// end "if (gImageFileInfoPtr->thematicType)"
			
		else	// !gImageFileInfoPtr->thematicType && ...->numberChannels > 1
			{
			sprintf (tempCharBufferPtr, "Chan\tColumns%s", gEndOfLine);
			tempCharBufferPtr += 12 + gNumberOfEndOfLineCharacters;
			
			}	// end "else !gImageFileInfoPtr->thematicType"
		
		}	// end "if (gListDataSpecsPtr->includeLineColumnValuesFlag)"
		
	else	// !gListDataSpecsPtr->includeLineColumnValuesFlag
		{
		if (!gImageFileInfoPtr->thematicType && gListDataSpecsPtr->numberChannels > 1)
			{
			sprintf (tempCharBufferPtr, "    Chan%s", gEndOfLine);
			tempCharBufferPtr += 8 + gNumberOfEndOfLineCharacters;
			
			}	// end "if (!gImageFileInfoPtr->thematicType && .."
		
		}	// end "if (gListDataSpecsPtr->includeLineColumnValuesFlag)"
		
			// List the line.																		
			
	continueFlag = OutputString (resultsFileStreamPtr, 
											gCharBufferPtr1, 
											0, 
											gOutputCode,
											continueFlag);

	return (continueFlag);	
		
}	// end "ListDataTitleLine2" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ListDataTitleLine3
//
//	Software purpose:	The purpose of this routine is to list the column title
//							lines for the List Data table to the output text window or 
//							disk file. This routine is for the title lines for column type 
//							lists.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:	
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 10/07/1998
//	Revised By:			Larry L. Biehl			Date: 10/26/1999	

Boolean ListDataTitleLine3 (
				SInt16								pointType,
				SInt16								dataValueFieldSize, 
				UInt32								columnStart,
				UInt32								columnEnd,
				FileIOInstructionsPtr			fileIOInstructionsPtr)
				
{
	CMFileStream*						resultsFileStreamPtr;
	char*									tempCharBufferPtr;
	HUInt16Ptr							maskBufferPtr;
	
	UInt32								column,
											columnInterval;
	
	SInt16								dataValueFieldIndexSkip,
											lineFieldSize;
											
	UInt16								maskRequestValue;
	
	Boolean								continueFlag;
	
	
			// Check variables.																	
			
	if (gCharBufferPtr1 == NULL)
																				return (FALSE);
	
	if (gListDataSpecsPtr->outputFormatCode != kChannelColumnList)
																				return (TRUE);

			// Initialize local variables.													
	
	continueFlag = TRUE;
	columnInterval = gListDataSpecsPtr->columnInterval;
	resultsFileStreamPtr = GetResultsFileStreamPtr (0);
	maskRequestValue = (UInt16)fileIOInstructionsPtr->maskValueRequest;
		
	dataValueFieldIndexSkip = dataValueFieldSize + 1;
			
	lineFieldSize = MAX (4,
							GetStringLengthForNumber (gImageWindowInfoPtr->maxNumberLines));
							
			// Include the title for the lines and columns if needed.			
	
	tempCharBufferPtr = gCharBufferPtr1;
	if (gListDataSpecsPtr->includeLineColumnValuesFlag)
		{	
		if (pointType == kMaskType)
			{
					// Skip a line before the first title line.									
					
			sprintf (tempCharBufferPtr, "%s", gEndOfLine);
			tempCharBufferPtr += gNumberOfEndOfLineCharacters;
			
			}	// end "if (pointType == kMaskType)"
							
				// Insert four blanks at beginning of the line.
				
		sprintf (tempCharBufferPtr, "    ");
		tempCharBufferPtr += 4;
	
				// Put the Line title in based on the length of the line numbers.
			
		sprintf (tempCharBufferPtr, "          ");
		tempCharBufferPtr += lineFieldSize;
			
		if (gImageFileInfoPtr->thematicType || gListDataSpecsPtr->numberChannels == 1)
			{
			sprintf (tempCharBufferPtr, "\t");
			tempCharBufferPtr++;
			
			}	// end "if (gImageFileInfoPtr->thematicType || ..."
			
		else	// !gImageFileInfoPtr->thematicType && ...->numberChannels > 1
			{
			sprintf (tempCharBufferPtr, "\t    \t");
			tempCharBufferPtr += 6;
			
			}	// end "else !gImageFileInfoPtr->thematicType && ..."
	
				// Include the column numbers.
				
		if (fileIOInstructionsPtr->maskBufferPtr != NULL)
			maskBufferPtr = &fileIOInstructionsPtr->maskBufferPtr[
													fileIOInstructionsPtr->maskColumnStart];
		
		for (column=columnStart; column<=columnEnd; column+=columnInterval)
			{
			if (pointType != kMaskType || 
						(pointType == kMaskType && *maskBufferPtr == maskRequestValue))
				{
				sprintf (tempCharBufferPtr, 
								"%*u\t",
								dataValueFieldSize, 
								(unsigned int)column);
				tempCharBufferPtr += dataValueFieldIndexSkip;
				
				}	// end "if (pointType != kMaskType || ..."
					
			if (pointType == kMaskType)
				maskBufferPtr += columnInterval;
	
			}	// end "for (column=columnStart; column<=columnEnd; ..."
					
				// Add the carriage return in place of the last tab.					
		
		tempCharBufferPtr--;											
		sprintf (tempCharBufferPtr, "%s", gEndOfLine);
		
				// List the line.																	
				
		continueFlag = OutputString (resultsFileStreamPtr, 
												gCharBufferPtr1, 
												0, 
												gOutputCode, 
												continueFlag);
		
		}	// end "if (gListDataSpecsPtr->includeLineColumnValuesFlag)" 

	return (continueFlag);	
		
}	// end "ListDataTitleLine3"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ListFieldData
//
//	Software purpose:	The purpose of this routine is to transfer the
//							field data for the requested channels to the output
//							text window or disk file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/06/1998
//	Revised By:			Larry L. Biehl			Date: 11/25/2019

SInt16 ListFieldData (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				SInt16								dataValueFieldSize, 
				SInt16								numberEDecimalDigits,
				SInt16								numberFDecimalDigits, 
				SInt16								classNumberCode, 
				SInt16								fieldNumber)

{
	DoublePoint							latLongPoint;								
	LongPoint							lineColumnPoint;
	Str31									blankString;
	
	CMFileStream*						resultsFileStreamPtr;
	HDoublePtr							buffer8BytePtr;
	
	HUCharPtr							inputBufferPtr,
											inputBufferPtr2;
											
	HUInt16Ptr							maskBufferPtr;
											
	UInt16*								localChannelsPtr;
	
	Point									point;
	RgnHandle							rgnHandle;
	
	SInt32								line,
											lineEnd,
											lineInterval,
											lineStart;
								
	UInt32								charIndex,
											column,
											columnEnd,
											columnInterval,
											columnStart,
											lineCount;
	
	SInt16								channelFieldSize,
											classFieldIndexSkip,
											classFieldSize,
											dataValueFieldIndexSkip,
											errCode,
											fieldNumberCode,
											index,
											latLongDecimalPlaces,
											latFieldSize,
											latLongFieldIndexSkip,
											lineColumnFieldSize,
											lineColumnFieldIndexSkip,
											lineFieldSize,
											localNumberChannels,
											longFieldSize,
											pointType;
											
	UInt16								maskRequestValue;
	
	Boolean								classFieldNumberFlag,
											continueFlag,
											includePixelFlag,
											latLongValueFlag,
											lineColumnValueFlag,
											lineFlag;
											
   
			// Make certain that input values make sense									
			
	if (fieldNumber < -1)
																						return (0);
	if (gProjectInfoPtr != NULL &&
							fieldNumber >= gProjectInfoPtr->numberStorageFields)
																					 	return (0);
	if (fileIOInstructionsPtr->fileInfoPtr == NULL)													
																						return (0);
																						
			// Exit if this is a cluster type field.										
	
	pointType = kRectangleType;		
	if (fieldNumber > -1 && gProjectInfoPtr != NULL)
		pointType = gProjectInfoPtr->fieldIdentPtr[fieldNumber].pointType;
		
	if (pointType == kClusterType)
																						return (1);
	
			// Initialize local variables.													
	
	continueFlag = 			TRUE;		
	inputBufferPtr = 			(HUCharPtr)gInputBufferPtr;
	inputBufferPtr2 = 		(HUCharPtr)gOutputBufferPtr;
	localNumberChannels = 	gListDataSpecsPtr->numberChannels;
	localChannelsPtr = 		(UInt16*)GetHandlePointer (
															gListDataSpecsPtr->channelsHandle);
	columnInterval = 			gListDataSpecsPtr->columnInterval;
	lineInterval = 			gListDataSpecsPtr->lineInterval;
	lineColumnValueFlag = 	gListDataSpecsPtr->includeLineColumnValuesFlag;
	latLongValueFlag =		gListDataSpecsPtr->includeLatLongValuesFlag;
	classFieldNumberFlag = 	gListDataSpecsPtr->includeClassFieldFlag;
	resultsFileStreamPtr = 	GetResultsFileStreamPtr (0);
	fieldNumberCode = 		fieldNumber + 1;
	
	lineFlag = (gListDataSpecsPtr->outputFormatCode == kChannelLineList);
	
			// Get a blank string the width of 'dataValueFieldSize' plus a tab to be
			// used for column lists when pixels are not a part of the polygonal field.
		
	sprintf ((char*)blankString, "..............");
	sprintf ((char*)&blankString[dataValueFieldSize], "\t");
	
			// Get the number string lengths for the lines, columns and channels.	
			
	lineFieldSize = MAX (4,
							GetStringLengthForNumber (gImageWindowInfoPtr->maxNumberLines));		
	
	lineColumnFieldSize = MAX (
						lineFieldSize,
						GetStringLengthForNumber (gImageWindowInfoPtr->maxNumberColumns));									
	
	lineColumnFieldIndexSkip = 2 * (lineColumnFieldSize + 1);
	
	channelFieldSize = MAX (4,
							GetStringLengthForNumber (gImageWindowInfoPtr->totalNumberChannels));
	
			// Get the number string lengths for the latitude and longitude values.
			
	latFieldSize = 10;
	longFieldSize = 11;
	
	if (gListDataSpecsPtr->latLongUnitsCode == kDMSLatLongUnitsMenuItem)
		{
		latFieldSize = 12;
		longFieldSize = 13;
		
		}	// end "if (...->latLongUnitsCode == kDMSLatLongUnitsMenuItem)"
		
	latLongFieldIndexSkip = latFieldSize + longFieldSize + 2;
	latLongDecimalPlaces = 6;
	
			// Get the number string lengths for the class and field codes. Fix
			// at 4 for now.	
	
	classFieldSize = 4;
	classFieldIndexSkip = 2 * (classFieldSize + 1);
		
	dataValueFieldIndexSkip = dataValueFieldSize + 1;
	
			// Determine if field is described by a polygon or a rectangle.  If	
			// the field is described by a polygon then create a region for the	
			// field.	
			
	gAreaDescription.lineInterval = lineInterval;
	gAreaDescription.columnInterval = columnInterval;																			
	
	if (fieldNumber >= 0)
		{
		GetFieldBoundary (gProjectInfoPtr, &gAreaDescription, fieldNumber);
		
		if (gAreaDescription.columnStart == 0)
																								return (1);
										
		rgnHandle = 	gAreaDescription.rgnHandle;
		lineStart = 	gAreaDescription.lineStart;
		lineEnd = 		gAreaDescription.lineEnd;
		columnStart = 	gAreaDescription.columnStart;
		columnEnd = 	gAreaDescription.columnEnd;
		
		}	// end "if (fieldNumber >= 0)" 
		
	else	// if (fieldNumber < 0)  // It is a selected area. 
		{
		rgnHandle = 	NULL;
		lineStart = 	gListDataSpecsPtr->lineStart;
		lineEnd = 		gListDataSpecsPtr->lineEnd;
		columnStart = 	gListDataSpecsPtr->columnStart;
		columnEnd = 	gListDataSpecsPtr->columnEnd;
		
		}	// end "else if (fieldNumber < 0)"
	
			// Load the total number of lines into the status dialog.				
				
	LoadDItemValue (gStatusDialogPtr, 
							IDC_Status20, 
							(lineEnd-lineStart+lineInterval)/lineInterval);
	lineCount = 0;
					
			// Intialize the nextTime variable to indicate when the next check	
			// should occur for a command-.													
			
	gNextTime = TickCount ();
	gNextStatusTime = TickCount ();
									
	continueFlag = ListDataTitleLine2 (dataValueFieldSize,
													classNumberCode, 
													fieldNumberCode);
				
	if (pointType != kMaskType)					
		continueFlag = ListDataTitleLine3 (pointType,
														dataValueFieldSize,
														columnStart, 
														columnEnd,
														fileIOInstructionsPtr);
														
			// Ready buffer for start of line. There will be 4 blanks.
			
	sprintf (gCharBufferPtr1, "    ");
	
			
			// Load some of the File IO Instructions structure that pertain
			// to the specific area being used.
			
	errCode = SetUpFileIOInstructions (fileIOInstructionsPtr,
													&gAreaDescription,
													lineStart,
													lineEnd,
													lineInterval,
													columnStart,
													columnEnd,
													columnInterval, 
													localNumberChannels,
													localChannelsPtr,
													kDetermineSpecialBILFlag);
													
	maskRequestValue = (UInt16)fileIOInstructionsPtr->maskValueRequest;

	#if defined multispec_wx
				// Disable all active windows except list data window in the wxwidgets
				// linux version.
		if (gListDataSpecsPtr->graphDataFlag)
			m_windowDisabler = new wxWindowDisabler (
											(wxWindow*)gGraphRecordPtr->graphViewCPtr->m_frame);
	#endif
	
			// Loop through the lines for the field.										

	for (line=lineStart; line<=lineEnd; line+=lineInterval)
		{
		if (!continueFlag)
			break;
					
				// Load the line count into the status dialog.							
		
		lineCount++;	
		if (TickCount () >= gNextStatusTime)
			{
			LoadDItemValue (gStatusDialogPtr, IDC_Status18, lineCount);
			gNextStatusTime = TickCount () + gNextStatusTimeOffset;
			
			}	// end "if (TickCount () >= gNextStatusTime)"
		
		point.v = (SInt16)line;
			
				// Get data for the requested channels for the line of image 		
				// data.  Return if there is a file IO error.							
			 
		errCode = GetLineOfData (fileIOInstructionsPtr,
											line, 
											columnStart,
											columnEnd,
											columnInterval,
											(unsigned char*)inputBufferPtr,
											(unsigned char*)inputBufferPtr2);
								
		if (errCode < noErr)
			{	
			CloseUpFileIOInstructions (fileIOInstructionsPtr, &gAreaDescription);
			CloseUpAreaDescription (&gAreaDescription);												
																								return (0);
			
			}	// end "if (errCode < noErr)"
			
		if (errCode != kSkipLine)
			{
			if (lineFlag)
				{
			   buffer8BytePtr = (HDoublePtr)inputBufferPtr2;
				
				if (fileIOInstructionsPtr->maskBufferPtr != NULL)
					maskBufferPtr = &fileIOInstructionsPtr->maskBufferPtr[
															fileIOInstructionsPtr->maskColumnStart];
			   
				for (column=columnStart; column<=columnEnd; column+=columnInterval)
					{								
							// For charIndex allow for initial four blanks.
							
					charIndex = 4;
					point.h = (SInt16)column;
				
					includePixelFlag = FALSE;
					if (pointType == kRectangleType)
						includePixelFlag = TRUE;
						
					else if (pointType == kPolygonType && PtInRgn (point, rgnHandle))
						includePixelFlag = TRUE;
					
					else if (pointType == kMaskType && *maskBufferPtr == maskRequestValue)
						includePixelFlag = TRUE;
						
					if (includePixelFlag)
						{
								// Put the line and column values in the string if needed.	
						
						if (lineColumnValueFlag)
							{
							sprintf (&gCharBufferPtr1[charIndex], 
											"%*d\t%*u\t",
											lineColumnFieldSize,
											(int)line,
											lineColumnFieldSize,
											(unsigned int)column);
							charIndex += lineColumnFieldIndexSkip;
							
							}	// end "if (lineColumnValueFlag)"
							
						if (latLongValueFlag)
							{
							lineColumnPoint.v = line;
							lineColumnPoint.h = column;
							
							ConvertLCPointToMapPoint (mapProjectionInfoPtr, 
																&lineColumnPoint, 
																&latLongPoint);
							
							if (!ConvertMapPointToLatLongPoint (
																mapProjectionInfoPtr,
																&latLongPoint))
								{
								latLongPoint.v = 0;
								latLongPoint.h = 0;
								
								}	// end "if (!ConvertMapPointToLatLongPoint (..."
								
							if (gListDataSpecsPtr->latLongUnitsCode ==
																				kDMSLatLongUnitsMenuItem)
								{
								LoadDMSLatLongStrings (latLongPoint.v,
																latLongPoint.h,
																(char*)gTextString,
																(char*)gTextString2);
																					
								sprintf (&gCharBufferPtr1[charIndex], 
												"%*s\t%*s\t",
												latFieldSize, 
												(char*)&gTextString[1],	// skip the first blank
												longFieldSize, 
												(char*)gTextString2);
								
								}	// end "if (...->latLongUnitsCode == ..."
								
							else	// ...->latLongUnitsCode == kDecimalLatLongUnitsMenuItem
								{
								sprintf (&gCharBufferPtr1[charIndex], 
											"%*.*f\t%*.*f\t",
											latFieldSize, latLongDecimalPlaces, latLongPoint.v,
											longFieldSize, latLongDecimalPlaces, latLongPoint.h);
									
								}	// end "else ...->latLongUnitsCode == ..."
											
							charIndex += latLongFieldIndexSkip;
							
							}	// end "if (latLongValueFlag)" 
						
								// Put the class and field number in the string if needed.	
						
						if (classFieldNumberFlag)
							{
							sprintf (&gCharBufferPtr1[charIndex], 
											"%*hd\t%*hd\t", 
											classFieldSize, classNumberCode, 
											classFieldSize, fieldNumberCode);
							charIndex += classFieldIndexSkip;
												
							}	// end "if (classFieldNumberFlag)" 
			   		
						for (index=0; index<localNumberChannels; index++)
							{
							LoadRealValueString (&gCharBufferPtr1[charIndex],
															*buffer8BytePtr,
															dataValueFieldSize,
															numberFDecimalDigits,
															numberEDecimalDigits,
															(char*)"",
															(char*)"\t");
																													
							buffer8BytePtr++;	
							
							charIndex += dataValueFieldIndexSkip;
								
							}	// end "for (index=0; index<localNumberChannels..."
							
								// Remove the last tab and add a carriage return. 
						
						sprintf (&gCharBufferPtr1[charIndex-1], "%s", gEndOfLine);
						charIndex += (gNumberOfEndOfLineCharacters - 1);
						continueFlag = OutputString (resultsFileStreamPtr, 
																gCharBufferPtr1, 
																charIndex, 
																gOutputCode, 
																continueFlag);
					
						}	// end "if (includePixelFlag)"
						
					else	// !includePixelFlag
						{
						if (pointType == kPolygonType) 	
							buffer8BytePtr += localNumberChannels;
							
						}	// end "else !includePixelFlag"
				
					if (!continueFlag)
						break;
						
					if (pointType == kMaskType)
						maskBufferPtr += columnInterval;
							
					} 		// end "for (column=columnStart; column<=..."
						
				}	// end "if (lineFlag)"
				
			else	// !lineFlag
				{
				if (pointType == kMaskType)					
					continueFlag = ListDataTitleLine3 (pointType,
																	dataValueFieldSize,
																	columnStart, 
																	columnEnd,
																	fileIOInstructionsPtr);
														
				for (index=0; index<localNumberChannels; index++)
					{
					charIndex = 0;
					
					if (index == 0)
						{
						if (!gImageFileInfoPtr->thematicType && localNumberChannels > 1)
							{
									// Put a blank line in before the next line of data.
									
							sprintf (gCharBufferPtr1, 
											"%s",
											gEndOfLine);
							charIndex = gNumberOfEndOfLineCharacters;
							
							}	// end "if (!gImageFileInfoPtr->thematicType && ..."
														
								// Insert 4 blanks.
								
						sprintf (&gCharBufferPtr1[charIndex], "    ");
						charIndex += 4;
				
								// Put the line number in the string if needed.
						
						if (lineColumnValueFlag)
							{
							sprintf (&gCharBufferPtr1[charIndex], 
											"%*d\t",
											lineFieldSize,
											(int)line);
							charIndex += lineFieldSize + 1;
											
							}	// end "if (lineColumnValueFlag)"
						
						}	// end "if (index == 0)"
						
					else	// index != 0
						{					
								// Insert 4 blanks.
								
						sprintf (&gCharBufferPtr1[charIndex], "    ");
						charIndex += 4;
						
								// Put a space in the string in place of the line number
								// if needed.
						
						if (lineColumnValueFlag)
							{
							sprintf (&gCharBufferPtr1[charIndex], "      ");
							charIndex += lineFieldSize;
							
							sprintf (&gCharBufferPtr1[charIndex], "\t");
							charIndex++;
							
							}	// end "if (lineColumnValueFlag)"
						
						}	// end "else index != 0"
					
					if (!gImageFileInfoPtr->thematicType && localNumberChannels > 1)
						{
						sprintf (&gCharBufferPtr1[charIndex], 
										"%*d\t",
										channelFieldSize, 	
										(int)(localChannelsPtr[index]+1));
										
						charIndex += channelFieldSize + 1;
						
						}	// end "if (!gImageFileInfoPtr->thematicType && ..."
							
				   buffer8BytePtr = (HDoublePtr)inputBufferPtr2;
				   buffer8BytePtr = &buffer8BytePtr[index];
					
					if (fileIOInstructionsPtr->maskBufferPtr != NULL)
						maskBufferPtr = &fileIOInstructionsPtr->maskBufferPtr[
															fileIOInstructionsPtr->maskColumnStart];
			   
					for (column=columnStart; column<=columnEnd; column+=columnInterval)
						{
						point.h = (SInt16)column;
					
						includePixelFlag = FALSE;
						if (pointType == kRectangleType)
							includePixelFlag = TRUE;
							
						else if (pointType == kPolygonType && PtInRgn (point, rgnHandle))
							includePixelFlag = TRUE;
						
						else if (pointType == kMaskType &&
																*maskBufferPtr == maskRequestValue)
							includePixelFlag = TRUE;
						
						if (includePixelFlag)
							{
							LoadRealValueString (&gCharBufferPtr1[charIndex],
															*buffer8BytePtr,
															dataValueFieldSize,
															numberFDecimalDigits,
															numberEDecimalDigits,
															(char*)"",
															(char*)"\t");
																										
							charIndex += dataValueFieldIndexSkip;
							
							}	// end "if (includePixelFlag)"
						
						else if (pointType != kMaskType)
							{
							sprintf (&gCharBufferPtr1[charIndex], "%s", (char*)blankString);
							charIndex += dataValueFieldIndexSkip;
							
							}	// end "else if (pointType != kMaskType)"
														
						buffer8BytePtr += localNumberChannels;
						
						if (pointType == kMaskType)
							maskBufferPtr += columnInterval;
								
						} 		// end "for (column=columnStart; column<=..."
						
							// Add the carriage return in place of the last tab if this	
							// is a column oriented list for each channel.	
						
					sprintf (&gCharBufferPtr1[charIndex-1], "%s", gEndOfLine);
					charIndex += (gNumberOfEndOfLineCharacters - 1);
					continueFlag = OutputString (resultsFileStreamPtr,
															gCharBufferPtr1,
															charIndex, 
															gOutputCode, 
															continueFlag);
					
					if (!continueFlag)
						break;
						
					}	// end "for (index=0; index<localNumberChannels..." 
							
				}	// end "else !lineFlag"

			if (gListDataSpecsPtr->graphDataFlag)
				{
				buffer8BytePtr = (HDoublePtr)inputBufferPtr2;
					
				if (fileIOInstructionsPtr->maskBufferPtr != NULL)
					maskBufferPtr = &fileIOInstructionsPtr->maskBufferPtr[
															fileIOInstructionsPtr->maskColumnStart];
			   
				for (column=columnStart; column<=columnEnd; column+=columnInterval)
					{
					point.h = (SInt16)column;
					
					includePixelFlag = FALSE;
					if (pointType == kRectangleType)
						includePixelFlag = TRUE;
						
					else if (pointType == kPolygonType && PtInRgn (point, rgnHandle))
						includePixelFlag = TRUE;
					
					else if (pointType == kMaskType && *maskBufferPtr == maskRequestValue)
						includePixelFlag = TRUE;
               
						if (includePixelFlag)
							{
							LoadGraphYVector (gGraphRecordPtr, 
														buffer8BytePtr, 
														(UInt32)localNumberChannels, 
														1);
														 
							gGraphRecordPtr->drawGraphCode = 1; 
						
							continueFlag = CreateGraph (gGraphRecordPtr->graphViewCPtr,
																	gGraphRecordPtr, 
																	line, 
																	point.h, 
																	0, 
																	0);
							
							MInitCursor ();
                    
							continueFlag = CheckSomeGraphWindowEvents (gGraphRecordPtr);
   
							MSetCursor (kWait);
						
							}	// end "if (includePixelFlag)" 
                  
					if (!continueFlag)
						break;
														
					buffer8BytePtr += localNumberChannels;
						
					if (pointType == kMaskType)
						maskBufferPtr += columnInterval;
							
					} 		// end "for (column=columnStart; column<=..." 
						
				}	// end "if (gListDataSpecsPtr->graphDataFlag)" 
					
			}	// end "if (errCode != kSkipLine)"
			
				// Exit routine if user has "command period" down							
			
		if (TickCount () >= gNextTime)
			{
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
				continueFlag = FALSE;
								
			}	// end "if (TickCount () >= gNextTime)"
			
		if (!continueFlag)
			break;
			
		if (pointType == kMaskType)
			fileIOInstructionsPtr->maskBufferPtr += 
												fileIOInstructionsPtr->numberMaskColumnsPerLine;
	      
		}	// end "for (line=lineStart; line<=lineEnd; line+=...)" 
		
	LoadDItemValue (gStatusDialogPtr, IDC_Status18, lineCount);
	
	#if defined multispec_wx
				// Re-enable all active windows and main menu in the wxwidgets
				// versions.
		if (m_windowDisabler != NULL)
			{
			delete m_windowDisabler;
			m_windowDisabler = NULL;
		
			}	// end "if (m_windowDisabler != NULL)"
	#endif

			// Close up any File IO Instructions structure that pertain to the 
			// specific area used for the list data.
			
	CloseUpFileIOInstructions (fileIOInstructionsPtr, &gAreaDescription);
	
	CloseUpAreaDescription (&gAreaDescription);
		
	return ((SInt16)continueFlag);
		
}	// end "ListFieldData" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ListProjectData
//
//	Software purpose:	The purpose of this routine is to list the
//							data for the requested channels, classes and fields
//							to the output text window or disk file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/24/1990
//	Revised By:			Larry L. Biehl			Date: 12/20/2007

Boolean ListProjectData (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				SInt16								dataValueFieldSize,
				SInt16								numberEDecimalDigits,
				SInt16								numberFDecimalDigits)

{
	SInt16*								classPtr;
	
	UInt32								classIndex,
											numberClasses;
	
	SInt16								classNumber;
	
	Boolean								returnFlag;
	
	
			// Initialize local variables.													
			
	numberClasses = gListDataSpecsPtr->numberClasses;
	returnFlag = TRUE;
	
			// Continue only if number of classes is one or more.						
	
	if (numberClasses > 0)
		{				
		classPtr = (SInt16*)GetHandlePointer (gListDataSpecsPtr->classHandle);
										
				// Get offsets to use for lines and columns in case needed for project 
				// fields.	
	
		SetAreaDescriptionOffsetVariables (&gAreaDescription,
														gImageWindowInfoPtr,
														gImageFileInfoPtr);
														
				// Set up status dialog.  Load in number of classes.					
				
		LoadDItemValue (gStatusDialogPtr, IDC_Status5, (SInt32)numberClasses);
						
		for (classIndex=0; classIndex<numberClasses; classIndex++)
			{
					// Set up status dialog.  Load in class count.						
				
			LoadDItemValue (gStatusDialogPtr, IDC_Status3, (SInt32)classIndex+1);
			
			classNumber = classPtr[classIndex] - 1;
			
					// Transfer data for the class.											
						
			if (ListClassData (fileIOInstructionsPtr, 
										mapProjectionInfoPtr,
										dataValueFieldSize, 
										numberEDecimalDigits, 
										numberFDecimalDigits,
										classNumber) == 0)
				{
				returnFlag = FALSE;
				break;
				
				}	// end "if (ListClassData (..." 
			
			}	// end "for (classIndex=0; ... 

		ClearAreaDescriptionOffsetVariables (&gAreaDescription);
			
		}	// end "if (numberClasses > 0)" 	
	
			// Indicate that routine completed normally.									
			
	return (returnFlag);
		
}	// end "ListProjectData" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean LoadListDataSpecs ()
//
//	Software purpose:	The purpose of this routine is to load the List 
//							Data Specification structure with the default set of
//							parameters.
//
//	Parameters in:		.
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			ListDataControl ()
//
//	Coded By:			Larry L. Biehl			Date: 05/24/1990
//	Revised By:			Larry L. Biehl			Date: 07/02/2018

Boolean LoadListDataSpecs (
				Handle								windowInfoHandle)
			
{
	SInt16								*channelsPtr,
											*classPtr,
											*featurePtr,
											*fieldPtr;
											
	SInt32								bytesNeeded;
		
	Boolean								changedFlag,
											projectFlag,
											returnFlag;
	
	
			// Assume routine will work okay.  If there isn't enough memory		
			// then the flag will be changed.												
			
	returnFlag = TRUE;
	projectFlag = (gProjectInfoPtr != NULL && 
											gProjectInfoPtr->numberStatisticsClasses > 0);
			
	if (!gImageWindowInfoPtr->projectWindowFlag)
		projectFlag = FALSE;
	
			// If a list data structure already exists, then check if it is		
			// for the current window.  If not release the old structure and		
			// initialize a new one.															
			
	if (gNonProjProcessorSpecs.listDataSpecsH != NULL)
		{
				// Lock the handle to the list data specifications and get			
				// a pointer to the structure.												
		
		gListDataSpecsPtr = (ListDataSpecsPtr)GetHandlePointer (
									gNonProjProcessorSpecs.listDataSpecsH, kLock, kMoveHi);
							
		if (gListDataSpecsPtr->windowInfoHandle != windowInfoHandle || 
			gImageWindowInfoPtr->fileInfoVersion != gListDataSpecsPtr->fileInfoVersion)
			{
			ReleaseListDataSpecsMemory (&gNonProjProcessorSpecs.listDataSpecsH);
			gListDataSpecsPtr = NULL;
			
			}	// end "if (gListDataSpecsPtr->windowInfoHandle != ..." 
			
		else	// gListDataSpecsPtr->windowInfoHandle == ... 
			{
			gListDataSpecsPtr->projectFlag = projectFlag;
			
			if (!projectFlag)
				{
				gListDataSpecsPtr->listClassesDataFlag = FALSE;
				gListDataSpecsPtr->listSelectedAreaDataFlag = TRUE;
				gListDataSpecsPtr->includeClassFieldFlag = FALSE;
				
				}	// end "if (!projectFlag)" 
				
			}	// end "else gListDataSpecsPtr->windowInfoHandle == ..." 
				
		}	// end "else gNonProjProcessorSpecs.listDataSpecsH != NULL" 
	
	if (gNonProjProcessorSpecs.listDataSpecsH == NULL)
		{
				// Set up handle to project file information								
			
		gNonProjProcessorSpecs.listDataSpecsH = 
													MNewHandle (sizeof (ListDataSpecs));
		if (gNonProjProcessorSpecs.listDataSpecsH != NULL)
			{
			gListDataSpecsPtr = (ListDataSpecsPtr)GetHandlePointer (
								gNonProjProcessorSpecs.listDataSpecsH, kLock, kMoveHi);
			
			gListDataSpecsPtr->includeLineColumnValuesFlag = FALSE;
			gListDataSpecsPtr->includeLatLongValuesFlag = FALSE;
			
					// Initialize the transfer data specification structure.			
			
			if (projectFlag && gProjectInfoPtr->numberStatisticsClasses > 0)
				{	
				gListDataSpecsPtr->listClassesDataFlag = TRUE;
				gListDataSpecsPtr->listSelectedAreaDataFlag = FALSE;
				gListDataSpecsPtr->includeClassFieldFlag = TRUE;
				
				}	// end "if (projectFlag && gProjectInfoPtr->..." 
			
			else	// !projectFlag || gProjectInfoPtr->...) 
				{	
				gListDataSpecsPtr->listClassesDataFlag = FALSE;
				gListDataSpecsPtr->listSelectedAreaDataFlag = TRUE;
				gListDataSpecsPtr->includeClassFieldFlag = FALSE;
				
				}	// end "else !projectFlag || gProjectInfoPtr->..." 
				
			gListDataSpecsPtr->projectFlag = projectFlag;
			gListDataSpecsPtr->graphDataFlag = FALSE;
			gListDataSpecsPtr->firstTime = TRUE;
			gListDataSpecsPtr->windowInfoHandle = windowInfoHandle;
			gListDataSpecsPtr->fileInfoVersion = gImageWindowInfoPtr->fileInfoVersion;
			gListDataSpecsPtr->classSet = kAllMenuItem;
			gListDataSpecsPtr->channelSet = kAllMenuItem;
			gListDataSpecsPtr->fieldSet = 0;
			gListDataSpecsPtr->lineStart = 1;
			gListDataSpecsPtr->lineEnd = gImageWindowInfoPtr->maxNumberLines;
			gListDataSpecsPtr->lineInterval = 1;
			gListDataSpecsPtr->columnStart = 1;
			gListDataSpecsPtr->columnEnd = gImageWindowInfoPtr->maxNumberColumns;
			gListDataSpecsPtr->columnInterval = 1;
			gListDataSpecsPtr->outputStorageType = 1;
			gListDataSpecsPtr->outputFormatCode = kChannelLineList;
			gListDataSpecsPtr->featureHandle = NULL;
			gListDataSpecsPtr->channelsHandle = NULL;
			gListDataSpecsPtr->classHandle = NULL;
			gListDataSpecsPtr->fieldHandle = NULL;
			gListDataSpecsPtr->fieldTypeCode = kTrainingType;
			if (gProjectInfoPtr && gProjectInfoPtr->numberStatTestFields > 0)
				gListDataSpecsPtr->fieldTypeCode = kTrainingType | kTestingType;
			
			gListDataSpecsPtr->latLongUnitsCode = kDecimalLatLongUnitsMenuItem;
			
			gListDataSpecsPtr->numberChannels = 
													gImageWindowInfoPtr->totalNumberChannels;
													
			gListDataSpecsPtr->numberFDecimalDigits = 
												gImageFileInfoPtr->maxNumberFDecimalDigits;
			
			if (projectFlag && gProjectInfoPtr != NULL)
				{
				gListDataSpecsPtr->numberClasses =
												gProjectInfoPtr->numberStatisticsClasses;
				gListDataSpecsPtr->numberFields = 
												gProjectInfoPtr->numberStatisticsFields;
												
				}	// end "if (projectFlag && gProjectInfoPtr != NULL)" 
				
			else	// !projectFlag || gProjectInfoPtr == NULL 
				{
				gListDataSpecsPtr->numberClasses = 0;
				gListDataSpecsPtr->numberFields = 0;
				
				}	// end "else !projectFlag || gProjectInfoPtr == NULL" 
			
			}	// end "if (gListDataSpecsH != NULL)" 
		
		else	// else gNonProjProcessorSpecs.listDataSpecsH == NULL 
			returnFlag = FALSE;
			
		}	// end "if (gNonProjProcessorSpecs.listDataSpecsH == NULL)" 
					
			// Check memory for classes vector.												
		
	if (returnFlag && gListDataSpecsPtr->projectFlag)
		{	
		bytesNeeded = (SInt32)gProjectInfoPtr->numberStatisticsClasses * sizeof (SInt16);
		classPtr = (SInt16*)CheckHandleSize (&gListDataSpecsPtr->classHandle,
															&returnFlag, 
															&changedFlag, 
															bytesNeeded);
		if (changedFlag)
			gListDataSpecsPtr->classSet = kAllMenuItem;
			
		}	// end "if (returnFlag)" 
			
			// If memory is not full, check memory for training fields				
			// vector.  For now we will allow enough space for all training 		
			// fields to belong to one class.  This should not be too wastefull.	

	if (returnFlag && gListDataSpecsPtr->projectFlag)
		{
		bytesNeeded = (SInt32)gProjectInfoPtr->numberStatisticsFields * sizeof (SInt16);
		fieldPtr = (SInt16*)CheckHandleSize (&gListDataSpecsPtr->fieldHandle,
															&returnFlag, 
															&changedFlag, 
															bytesNeeded);
		if (changedFlag)
			gListDataSpecsPtr->fieldSet = 0;
			
		}	// end "if (returnFlag && ...)" 
			
			// If memory is not full, check memory for feature vector pointer.

	if (returnFlag)
		{
		bytesNeeded = (SInt32)gImageWindowInfoPtr->totalNumberChannels * sizeof (SInt16);
		featurePtr = (SInt16*)CheckHandleSize (&gListDataSpecsPtr->featureHandle,
															&returnFlag,  
															&changedFlag, 
															bytesNeeded);
		
		if (changedFlag)
			gListDataSpecsPtr->channelSet = kAllMenuItem;
			
		}	// end "if (returnFlag)" 
			
			// If memory is not full, check memory for statistics						
			// channels vector.																	

	if (returnFlag)
		{
				// Allow for a channel wavelength order vector
				// Do not allow for wavelength order for now. It is not being implemented.
		
		channelsPtr = (SInt16*)CheckHandleSize (&gListDataSpecsPtr->channelsHandle,
																&returnFlag, 
																&changedFlag, 
																bytesNeeded);
		
		}	// end "if (returnFlag)" 
		
	if (returnFlag)
		{
				// Load selection rectangle information into list data		 		
				// specification information if the selection rectangle has 		
				// been set up																		

		GetSelectedAreaInfo (gActiveImageWindow,
									gImageWindowInfoPtr,
									&gListDataSpecsPtr->lineStart,
									&gListDataSpecsPtr->lineEnd,
									&gListDataSpecsPtr->columnStart,
									&gListDataSpecsPtr->columnEnd,
									kDontClearSelectionArea,
									kUseThreshold,
									kDontAdjustToBaseImage);
		
				// Make certain that feature and class vectors are						
				// filled in properly.															
		
		if (gListDataSpecsPtr->channelSet == kAllMenuItem)		
			LoadFeatureVector (kNoTransformation,
									(SInt16*)&gListDataSpecsPtr->numberChannels,
									featurePtr,
									gListDataSpecsPtr->numberChannels);
															
		LoadChannelsVector (kNoProject,
									kNoTransformation,
									gListDataSpecsPtr->numberChannels, 
									featurePtr, 
									(SInt16*)&gListDataSpecsPtr->numberChannels,
									channelsPtr);
		
		if (gListDataSpecsPtr->classSet == kAllMenuItem && 
															gListDataSpecsPtr->projectFlag)								
			LoadClassVector (&gListDataSpecsPtr->numberClasses,
									classPtr);
		
		if (gListDataSpecsPtr->fieldSet == 0 && gListDataSpecsPtr->projectFlag)									
			LoadFieldVector (&gListDataSpecsPtr->numberFields,
									fieldPtr,
									kTrainingType+kTestingType);
			
		}	// end "if (returnFlag)" 
	
			// If unable to set up information for list data							
			// specifications, release memory allocated to it (if any).				
			
	else	// !returnFlag 
		{
		ReleaseListDataSpecsMemory (&gNonProjProcessorSpecs.listDataSpecsH);
		gListDataSpecsPtr = NULL;
		
		}	// end "else !returnFlag" 
		
	return (returnFlag);

}	// end "LoadListDataSpecs"
