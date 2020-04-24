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
//	File:						SClassifyEcho.cpp
//
//	Authors:					Byeungwoo Jeon
//								Larry L. Biehl
//
//	Revision date:			04/14/2020
//
//	Language:				C
//
//	System:					Linux, Macintosh and Windows Operating Systems
//
//	Brief description:	This file contains functions that do echo classification
//								processes.
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
//------------------------------------------------------------------------------------

#include	"SMultiSpec.h" 
  
#if defined multispec_wx  
	#include <stdio.h>
	#include <iostream>
	#include	"SFileStream_class.h"
#endif	// defined multispec_wx

#if defined multispec_win  
	#include	"SFileStream_class.h"
#endif	// defined multispec_win

#include	"SEcho.h"


		//	pointers to use for obtaining information about ECHO classifier		
		// These are defined in SClassifyEchoControl.cpp 											
		
extern common_classifier_informationPtr		gCommon_InfoVariablePtr;


		// Prototypes of functions defined and used in this file	

SInt64  GetMemoryNeededForEchoInfoParameters (
				EchoClassifierVar*					echoInfoPtr);

Boolean SetUseTempDiskFileFlag (
				EchoClassifierVarPtr					echoInfoPtr);
				
SInt16 Write_Classified_Fields_File (
				HUCharPtr 								ioBuffer1Ptr,
				common_classifier_information* 	common_info,
				EchoClassifierVar* 					echo_info);
				
SInt16 Write_Homogeneous_Fields_File (
				HUCharPtr 								ioBuffer1Ptr,
				common_classifier_information* 	common_info,
				EchoClassifierVar* 					echo_info);

					

//------------------------------------------------------------------------------------
//                   Copyright 1989-2020 Purdue Research Foundation
//
//	Function name:		SInt16 EchoClassifier
//
//	Software purpose:	Entry point of "ECHO" core.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	The new input string pointer.
//
//	Calls to the functions in other files:
//							setup_echo_image_selection	in SAux_echo.cpp
//
// Called By:			ClassifyArea in SClassify.cpp
//
// Globals:				gCommon_InfoVariablePtr
//							gEchoClassifierVariablePtr"
//
//	Coded By:			Byeungwoo Jeon			Date: 01/01/1989
// Revised By:			Byeungwoo Jeon			Date: 09/28/1991
//	Revised By:			Larry L. Biehl			Date: 08/13/2010

SInt16 EchoClassifier (
				SInt16								classPointer,
				AreaDescriptionPtr				areaDescriptionPtr,
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				LCToWindowUnitsVariables* 		lcToWindowUnitsVariablesPtr,
				ClassifierVarPtr					clsfyVariablePtr,
				SInt64*								countVectorPtr)

{
	HUCharPtr							ioBuffer1Ptr,
											probabilityBufferPtr;
   
   CMFileStream						*clProbabilityFileStreamPtr,
											*resultsFileStreamPtr;
   							
   pixel_information					*pixel_info;
   statistics							*class_stat;
   
   SInt32								num_row,
											num_col;
   
   SInt16								cell_width,
											error_code;
	
   Boolean								continueFlag;
   							

			// Set up local Variables  

	continueFlag			= TRUE;
   class_stat  			= gCommon_InfoVariablePtr->class_stat;
   pixel_info				= &(gCommon_InfoVariablePtr->pixel_info);
   cell_width				= gEchoClassifierVariablePtr->cell_width;
   							  
	ioBuffer1Ptr = 		(HUCharPtr)gOutputBufferPtr;
	probabilityBufferPtr = NULL;
	
	if (gClassifySpecsPtr->createThresholdTableFlag)
		probabilityBufferPtr = 
								&ioBuffer1Ptr[gClassifySpecsPtr->probabilityBufferOffset];
   
			// Initialize variables related to the Image Selection
			// and Set up echo variables related to the Image Selection
			// by calling "setup_echo_image_selection ()"

   pixel_info->nrow0 = areaDescriptionPtr->lineStart;
   pixel_info->nrow1 = areaDescriptionPtr->lineEnd;
   pixel_info->ncol0 = areaDescriptionPtr->columnStart;
   pixel_info->ncol1 = areaDescriptionPtr->columnEnd; 
   
   num_row = pixel_info->num_row = areaDescriptionPtr->numberLines;
   num_col = pixel_info->num_col = (UInt32)areaDescriptionPtr->numSamplesPerChan; 
	
	pixel_info->ipixels = (SInt64)num_row * num_col;
      
   setup_echo_image_selection (gCommon_InfoVariablePtr,
   										gEchoClassifierVariablePtr);
	
			// Determine if a temporary disk file needs to be used because of
			// the size of the selected area.
	
	continueFlag = SetUseTempDiskFileFlag (gEchoClassifierVariablePtr);
   										
			// Assign memories to "epix->epixel" and "epix->fldlik" and other
			// vectors to be used during classification 
 			// by calling malloc_epix ()

	if (continueFlag)
		continueFlag = malloc_epix (gEchoClassifierVariablePtr);
	
   if (!continueFlag)
		{
   	free_epix (gEchoClassifierVariablePtr);
      																				return (1);
      
      }	// end "if (!continueFlag)
   
			// PHASE 1 : Find out Homogeneous Cells and Compute Cell Lik.
		
			// Load some of the File IO Instructions structure that pertain
			// to the specific area being classified.
			
	SetUpFileIOInstructions (fileIOInstructionsPtr,
										areaDescriptionPtr,
										(UInt16)fileIOInstructionsPtr->numberChannels,
										fileIOInstructionsPtr->channelListPtr,
										kDetermineSpecialBILFlag);
										
	fileIOInstructionsPtr->nextLineToRead = areaDescriptionPtr->lineStart;
	fileIOInstructionsPtr->numberLinesAtOnce = cell_width;
	fileIOInstructionsPtr->numberColumns =
												(SInt32)areaDescriptionPtr->numSamplesPerChan;
	
	ShowStatusDialogItemSet (kStatusLine);
	LoadDItemValue (
				gStatusDialogPtr, IDC_Status20, (SInt32)areaDescriptionPtr->numberLines);

	LoadDItemStringNumber (kClassifyStrID,
									IDS_Classify42,	// "\pCreating Homogeneous Fields:"
									gStatusDialogPtr,
									IDC_Status21,
									(Str255*)gTextString);
									
	if (gStatusDialogPtr != NULL)
		DrawDialog (gStatusDialogPtr);
					
	error_code = phase1 (fileIOInstructionsPtr,
								class_stat,
								gEchoClassifierVariablePtr,
								gClassifySpecsPtr->thresholdTablePtr,
								probabilityBufferPtr,
								areaDescriptionPtr);
								
	HideStatusDialogItemSet (kStatusMinutes);
   
   		// Free the memory for the likelihood values.  They are not
   		// needed any more.
   		
   gEchoClassifierVariablePtr->fldlikPtr = (HFldLikPtr)CheckAndDisposePtr (
   											(Ptr)gEchoClassifierVariablePtr->fldlikPtr);
   								
   gEchoClassifierVariablePtr->work1 = CheckAndDisposePtr (
   											gEchoClassifierVariablePtr->work1);
   											
   gEchoClassifierVariablePtr->work2 = CheckAndDisposePtr (
   											gEchoClassifierVariablePtr->work2);
   											
   gEchoClassifierVariablePtr->cellClassPtr = CheckAndDisposePtr (
   									gEchoClassifierVariablePtr->cellClassPtr);
   									
   gEchoClassifierVariablePtr->fieldLikeIndicesPtr = 
   			(HFldLikPtr*)CheckAndDisposePtr (
   				(Ptr)gEchoClassifierVariablePtr->fieldLikeIndicesPtr);
   				
   gEchoClassifierVariablePtr->fieldLikeFlagsPtr = CheckAndDisposePtr (
   										gEchoClassifierVariablePtr->fieldLikeFlagsPtr);

			//	Save Homogeneous Fields and Classified Homogeneous Fields
	   
	if (error_code == 0 && areaDescriptionPtr->classNumber == 0 &&
													(gOutputCode & kEchoFieldsCode))
		{
		LoadDItemValue (gStatusDialogPtr, 
								IDC_Status20, 
								(SInt32)areaDescriptionPtr->numberLines);
		LoadDItemValue (gStatusDialogPtr, IDC_Status22, (SInt32)0);
					
		error_code = Write_Homogeneous_Fields_File (ioBuffer1Ptr,
																	gCommon_InfoVariablePtr,
																	gEchoClassifierVariablePtr);
      
      if (error_code == noErr)
      	{
			LoadDItemValue (gStatusDialogPtr, IDC_Status22, (SInt32)0);
			error_code = Write_Classified_Fields_File (ioBuffer1Ptr,
																		gCommon_InfoVariablePtr,
																		gEchoClassifierVariablePtr);
											
			}	// end "if (error_code == noErr)" 
	 													
	 	}	// end "if (error_code == 0 && ...)"

			// Write results to requested files and output window.

	if (gStatusDialogPtr)
		{ 
		LoadDItemStringNumber (kClassifyStrID,
										IDS_Classify43,	// "\pWriting Classification Results"
										gStatusDialogPtr,
										IDC_Status21,
										(Str255*)gTextString);
		LoadDItemString (gStatusDialogPtr, IDC_Status22,(Str255*)"\0");
		LoadDItemValue (gStatusDialogPtr, IDC_Status20, (SInt32)num_row);
		DrawDialog (gStatusDialogPtr);
		
		}	// end "if (gStatusDialogPtr)"
   
   if (error_code == 0) 
   	{			
		resultsFileStreamPtr = 			GetResultsFileStreamPtr (0);
		clProbabilityFileStreamPtr = 	GetResultsFileStreamPtr (1);
		error_code = PostEchoClassifier (classPointer,
													areaDescriptionPtr, 
													resultsFileStreamPtr,
													clProbabilityFileStreamPtr, 
													ioBuffer1Ptr, 
													gEchoClassifierVariablePtr,
													clsfyVariablePtr, 
													countVectorPtr,
													lcToWindowUnitsVariablesPtr);
		
   	}	// "if (error_code == 0)..."
 
			// Free memory assigned to "echo_info_ptr->fldlikPtr"
			// and "echo_info_ptr->epixel"							
   
   free_epix (gEchoClassifierVariablePtr);
   
   return (error_code);
   
}	// End "EchoClassifier"



//------------------------------------------------------------------------------------
//                   Copyright 1989-2020 Purdue Research Foundation
//
//	Function name:		UInt32 EstimateNumberOfHomogeneousFields
//
//	Software purpose:	Make an estimate of the number of homogenous fields to allow
//							for. Make it on the high side. Not sure though what a good 
//							number is though.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	Maximum number of homogeneous fields to allow for.
//
// Called By:			EchoClassifier in SClassifyEcho.cpp
//
//	Coded By:			Larry L. Biehl			Date: 08/17/2010
//	Revised By:			Larry L. Biehl			Date: 08/17/2010

UInt32 EstimateNumberOfHomogeneousFields (
				EchoClassifierVar*				echoInfoPtr)

{
	double								maxNumberFields,
											percent;
	
	UInt32								max_field_number;
	
	
   maxNumberFields = (double)echoInfoPtr->nrw_fldlik * echoInfoPtr->ncl_fldlik;
   
   		// If number of potential homogeneous fields is less that 2,000,000 then 
   		// just use this value which is absolute maximum.
   		
   if (maxNumberFields > 2000000 && maxNumberFields < 50000000)
   	{
   	percent = .20 - ((maxNumberFields-2000000)/498000000) * .10;
   	maxNumberFields *= percent;
   	
   	}	// end "if (maxNumberFields > 2000000 && maxNumberFields < 50000000)"
   	
   else if (maxNumberFields >= 50000000)
   	maxNumberFields *= .1;
   	
   max_field_number = (UInt32)MIN (maxNumberFields, 500000000);
   
   return (max_field_number);
   
}	// End "EstimateNumberOfHomogeneousFields"



//------------------------------------------------------------------------------------
//                   Copyright 1989-2020 Purdue Research Foundation
//
//	Function name:		SInt64 GetMemoryNeededForEchoInfoParameters
//
//	Software purpose:	Determine the amount of memory needed for the EchoInfo structure
//							for all parameters except for epix_buffer.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	Estimate of total bytes needed
//
// Called By:			EchoClassifier in SClassifyEcho.cpp
//
//	Coded By:			Larry L. Biehl			Date: 08/16/2010
//	Revised By:			Larry L. Biehl			Date: 08/17/2010

SInt64 GetMemoryNeededForEchoInfoParameters (
				EchoClassifierVar*				echoInfoPtr)

{
	SInt64								numberBytes;
  	
	SInt32								ncls;
								
	UInt32								current_max_field_number,
											numberVectorElements;
   
  	UInt16								nband;
   							

   ncls    = echoInfoPtr->ncls;
   nband   = echoInfoPtr->nband;
   
   current_max_field_number = EstimateNumberOfHomogeneousFields (echoInfoPtr);
   		
			// Memory Allocation to FLDLIK												

	numberBytes = 0;
	if (echoInfoPtr->ncl_fldlik > 0)
		numberBytes = (SInt64)3 * echoInfoPtr->ncl_fldlik * ncls * sizeof (FldLik);

			// Memory Allocation to field likelihood table index vector								
 
	numberBytes += (current_max_field_number+1) * sizeof (UInt32);
	
			// Memory Allocation to field class number vector							
	
	numberBytes += (current_max_field_number+1) * sizeof (SInt16);

			// Memory Allocation to field_number_table								

	numberBytes += (current_max_field_number+1) * sizeof (UInt32);

			// Memory Allocation to work1 vector 										

   numberBytes += MAX (nband, ncls) * sizeof (double);
   	  	
			// Memory Allocation to work2 vector 										

   numberVectorElements = (UInt32)nband * (nband+1)/2;
   numberVectorElements = MAX ((UInt32)(4*ncls), numberVectorElements);
	numberBytes += numberVectorElements * sizeof (double);

			// Memory Allocation to cellClassPtr 										

	if (echoInfoPtr->ncl_fldlik > 0)
     numberBytes += echoInfoPtr->ncl_fldlik * sizeof (SInt16);

			// Memory Allocation to fieldLikeIndicesPtr 								

	if (echoInfoPtr->ncl_fldlik > 0)
      numberBytes += echoInfoPtr->ncl_fldlik * 2 * sizeof (FldLik*);

			// Memory Allocation to fieldLikeFlagsPtr									

	if (echoInfoPtr->ncl_fldlik > 0)
      numberBytes += echoInfoPtr->ncl_fldlik * 2 * sizeof (char);
  
   return (numberBytes);

}	// End "GetMemoryNeededForEchoInfoParameters"



//------------------------------------------------------------------------------------
//                   Copyright 1989-2020 Purdue Research Foundation
//
//	Function name:		SInt16 SaveProbabilityInformation
//
//	Software purpose:	Save Cell Likelihood Values in a file pointed by fp.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	error_code
//
//	Calls to the functions in other files:
//							setup_echo_image_selection ()	in SAux_echo.cpp
//
// Called By:			EchoClassifier in SClassifyEcho.cpp
//
// Globals:				gEchoClassifierVariable
//							gCommon_InfoVariablePtr
//
//	Coded By:			Byeungwoo Jeon			Date: 01/01/1989
//	Revised By:			Larry L. Biehl			Date: 08/13/2010

SInt16 SaveProbabilityInformation (
				SInt32								numberLines, 
				double*								thresholdTablePtr, 
				statistics*							class_stat, 
				HUCharPtr							probabilityBufferPtr, 
				HFldLikPtr							cellLikePtr, 
				HSInt32Ptr							epix_ptr, 
				HSInt16Ptr							fieldClassNumberPtr, 
				HUInt32Ptr							field_number_table,
				AreaDescriptionPtr				areaDescriptionPtr)
				
{
	Point							point;
	RgnHandle					rgnHandle;
	                  
	double						dValue;
	
	UInt32						numberClasses;
	
	CMFileStream*				clProbabilityFileStreamPtr;
	SInt16*						thresholdProbabilityPtr;
	
   SInt16						cellWidth,
   								errorCode,
   								maxClass,     
   								thresholdCode;
   
	SInt32						column,
									line,
   								numberColumns;
									
	UInt32						count;
									
	HUCharPtr	 				savedProbabilityBufferPtr;
	
	SInt16						ix,
									index;
									
	Boolean						polygonFieldFlag;


	if (probabilityBufferPtr == NULL)
																							return (0);
		
		// Set up local variables 															

   errorCode			= 0;
   numberClasses		= gEchoClassifierVariablePtr->ncls;
	numberColumns		= gEchoClassifierVariablePtr->num_col;
   cellWidth 			= gEchoClassifierVariablePtr->cell_width;
   
	polygonFieldFlag 	= areaDescriptionPtr->polygonFieldFlag;
	rgnHandle 			= areaDescriptionPtr->rgnHandle;
	point.v 				= (SInt16)areaDescriptionPtr->line;
	point.h 				= (SInt16)areaDescriptionPtr->columnStart;
		
	if (polygonFieldFlag && rgnHandle == NULL)
																							return (0);
   
   thresholdCode = 0;
	if (gClassifySpecsPtr->thresholdFlag)
		thresholdCode = gClassifySpecsPtr->probabilityThresholdCode;
	
	thresholdProbabilityPtr = gClassifySpecsPtr->thresholdProbabilityPtr;
	
	clProbabilityFileStreamPtr = NULL;
	if (areaDescriptionPtr->classNumber == 0)
		clProbabilityFileStreamPtr = GetResultsFileStreamPtr (1);
	
	savedProbabilityBufferPtr = probabilityBufferPtr;

	if (thresholdTablePtr != NULL) 
		{
				// Load the probability information for the homogeous cells.		
				
   	for (column=0; column<numberColumns; column+=cellWidth) 
   		{
      	if (*epix_ptr > 0)
      		{
      		maxClass = 
      			fieldClassNumberPtr[field_number_table [*epix_ptr]] - 1;

	   				// Cell likehood value is  classConstant - r**2/2 				
	   				// But threshold is for the value r**2/2							
	   			
				dValue = class_stat[maxClass].classConstant - cellLikePtr[maxClass];
					
				index = GetThresholdClass (dValue, 
														thresholdTablePtr);
	
				for (ix=0; ix<cellWidth; ix++)
					{
					count = 0;
					for (line=0; line<numberLines; line++)
						{
						if (!polygonFieldFlag || PtInRgn (point, rgnHandle))
							{
							probabilityBufferPtr[count] = (UInt8)index;
											
	 						gTempDoubleVariable1 += thresholdProbabilityPtr[index];
											
							if (*probabilityBufferPtr <= thresholdCode)
								epix_ptr[count] |= 0x40000000;
								
							count += numberColumns;
      					point.v++;
						
							}	// end "if (!polygonFieldFlag || PtInRgn (..." 
						
						}	// end "for (line=0; line<numberLines; line++)" 
						
					probabilityBufferPtr++;
					epix_ptr++;
					point.h++;
					point.v -= (SInt16)numberLines;
						
					}	// end "for (ix=0; ix<cellWidth; ix++)" 
				
				}	// end "if (*epix_ptr > 0)" 
			
			else	// *epix_ptr <= 0 
				{
				probabilityBufferPtr += cellWidth;
   			epix_ptr += cellWidth;
				point.h += cellWidth;
   			
   			}	// end "else *epix_ptr <= 0" 
				
   		cellLikePtr += numberClasses;
   			   		   
   		}	// "for (column=0; column<=...."	
   	
		probabilityBufferPtr = savedProbabilityBufferPtr;
				
		}	//	end "if (thresholdTablePtr)" 

			// Save probability information to disk file.								
	
	if (clProbabilityFileStreamPtr != NULL)
		{	
		count = numberLines * numberColumns;
		errorCode = MWriteData (clProbabilityFileStreamPtr, 
										&count, 
										probabilityBufferPtr,
										kErrorMessages);
		
		}	// end "if (clProbabilityFileStreamPtr != NULL)" 
  
  	return (errorCode);
	
}	// end "SaveProbabilityInformation" 
												


//------------------------------------------------------------------------------------
//                   Copyright 1989-2020 Purdue Research Foundation
//
//	Function name:		Boolean SetUseTempDiskFileFlag
//
//	Software purpose:	Determine if a temporary disk file needs to be used because of
//							the size of the selected area.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 08/12/2010
//	Revised By:			Larry L. Biehl			Date: 04/14/2020

Boolean SetUseTempDiskFileFlag (
				EchoClassifierVarPtr				echoInfoPtr)

{
	CMFileStream*						tempFileStreamPtr;
	FileStringPtr						tempFileNamePtr;
	
	SInt64								bytesLeftForPixels,
											bytesNeeded,
											freeBytes,
											longestContBlock,
											memoryNeededForRest;
	
	SInt32								parID;
	
	UInt32 								current_max_field_number;
	
	SInt16								errCode,
											volRefNum;

	Boolean								continueFlag = TRUE,
											fSSpecFlag;
	
	
			// Make an estimate of the number of pixels that can be stored in memory
			// to determine if MultiSpec needs to use a temporary disk file to run
			// the classification.
			
	freeBytes = MGetFreeMemory (&longestContBlock);
	memoryNeededForRest = GetMemoryNeededForEchoInfoParameters (echoInfoPtr);
	bytesLeftForPixels = freeBytes - memoryNeededForRest;
	
			// Allow for safety buffer of 2000000 bytes.
			
	bytesLeftForPixels -= 2000000;
	
	if (echoInfoPtr->ipixels > bytesLeftForPixels/(SInt64)sizeof (SInt32))
		{
		tempFileStreamPtr = InitializeFileStream ((CMFileStream*)NULL);
		continueFlag = (tempFileStreamPtr != NULL);
		
		if (continueFlag)
			{
			echoInfoPtr->tempFileStreamPtr = tempFileStreamPtr;

					// Put the temp file in the same directory/folder as the input image
					// file.
					
			volRefNum = GetVolumeReferenceNumber (gImageFileInfoPtr);
			parID = GetParID (gImageFileInfoPtr);
			fSSpecFlag = GetFSSpecFlag (gImageFileInfoPtr);
			
			SetType (tempFileStreamPtr, 'BINA');		
			SetFSSpecFlag (tempFileStreamPtr, fSSpecFlag);
			SetParID (tempFileStreamPtr, parID);
			
			#if defined multispec_mac
						// Set parent FSRef
			
				CMFileStream*						fileStreamPtr;
			
				fileStreamPtr = GetFileStreamPointer (gImageFileInfoPtr);
				tempFileStreamPtr->parentFSRef = fileStreamPtr->parentFSRef;	
			#endif	// defined multispec_mac
			
			#if defined multispec_win || defined multispec_wx
				FileStringPtr filePathPointer =
					(FileStringPtr)GetFilePathPPointerFromFileInfo (gImageFileInfoPtr);
			
				FileStringPtr tempFilePathPtr =
					(FileStringPtr)GetFilePathPPointerFromFileStream (tempFileStreamPtr);
					
				//CopyPToP (tempFilePathPtr,  filePathPointer);
				CopyFileStringToFileString(filePathPointer, tempFilePathPtr, _MAX_FILE);

						// Now get wide character and unicode names.

				SetFileDoesNotExist (tempFileStreamPtr, kKeepUTF8CharName);
				UpdateFileNameInformation (tempFileStreamPtr, NULL);
			
						// This will force the path info in tempFileSreamPtr to be set.
			
				//tempFileStreamPtr->GetFileUTF8PathLength ();
			#endif	// defined multispec_win || defined multispec_wx
			
			tempFileNamePtr =
					(FileStringPtr)GetFileNamePPointerFromFileStream (tempFileStreamPtr);
			//CopyPToP (tempFileNamePtr,  (UCharPtr)"\0tempEcho0000.tmp");
			int length = GetFileStringLength (tempFileNamePtr);
			memcpy ((char*)&tempFileNamePtr[length+1],
						(char*)"tempEcho0000.tmp",
						16);
			SetFileStringLength (tempFileNamePtr, length+16);
			
			#if defined multispec_win || defined multispec_wx
						// Now copy the proposed file name to the file path string.
			
				tempFileStreamPtr->SetFileName (tempFileNamePtr);
			
						// GetVolumeFreeSpace requires the full path name
			
				//tempFileNamePtr = tempFilePathPtr;
			#endif	// defined multispec_win || defined multispec_wx
			
					// The following will verify that the direcctory specified is
					// writable and if not change it to the working directory. Only
					// done for wxWidgets interface versions.
			
			CheckIfDirectoryIsWriteable (tempFileStreamPtr);
			
					// Check if enough space on the volume.									
			
			errCode = GetVolumeFreeSpace (tempFileStreamPtr,
													volRefNum, 
													&freeBytes);
			
					// Allow for space for temporary file, classification, probability
					// image, echo class image and echo field image. Also allow for 1
					// gigabyte minimum space to be left.
			
			bytesNeeded = 0;
			if (GetResultsFileStreamPtr (0) != NULL)
						// Classified file
				bytesNeeded += sizeof (SInt8);
			
			if (GetResultsFileStreamPtr (1) != NULL)
						// Probability file
				bytesNeeded += sizeof (SInt8);
			
			if (GetResultsFileStreamPtr (2) != NULL)
				{
						// Homogeneous fields file

				current_max_field_number = 
									EstimateNumberOfHomogeneousFields (echoInfoPtr);

				if (current_max_field_number < 256)
					bytesNeeded += sizeof (UInt8);
				
				else if (current_max_field_number < UInt16_MAX)
					bytesNeeded += sizeof (UInt16);
				
				else	// current_max_field_number > UInt16_MAX
					bytesNeeded += sizeof (UInt32);

				}	// end "if (GetResultsFileStreamPtr (2) != NULL)"
			
			if (GetResultsFileStreamPtr (3) != NULL)
						// Classified fields file
				bytesNeeded += sizeof (SInt8);
			
					// Temporary file

			bytesNeeded += sizeof (SInt32);
			
			bytesNeeded *= echoInfoPtr->ipixels;
			
					// Allow for 1 GB space to be left.
			
			bytesNeeded += 1000000000;
			
			bytesNeeded -= freeBytes;
			
			if (errCode == noErr && bytesNeeded > 0)
				{
				DiskFullAlert (bytesNeeded, (Str255*)tempFileNamePtr);
				errCode = 1;
				
				}	// end "if (errCode == noErr && bytesNeeded > 0)"
		
			if (errCode == noErr)
				errCode = CreateNewFile (echoInfoPtr->tempFileStreamPtr, 
												 volRefNum, 
												 gCreator, 
												 kErrorMessages, 
												 kReplaceFlag);
	
					// Now check if a file name was entered.

			tempFileNamePtr =
					(FileStringPtr)GetFilePathPPointerFromFileStream (tempFileStreamPtr);
			continueFlag =
						(errCode == noErr) && (GetFileStringLength (tempFileNamePtr) > 0);
			
			if (continueFlag)
				{
				echoInfoPtr->useTempDiskFileFlag = TRUE;
				echoInfoPtr->tempDiskFileWasUsedFlag = TRUE;
				
				}	// end "if (continueFlag)"
		
					// Make sure the output directory variable is set to empty.
			
			ResetOutputDirectory ();
			
			}	// end "if (continueFlag)"
		
		}	// end "if (echoInfoPtr->ipixels > bytesLeftForPixels...)"
																			
	return (continueFlag);
   
}	// End "SetUseTempDiskFileFlag"



//------------------------------------------------------------------------------------
//                   Copyright 1989-2020 Purdue Research Foundation
//
//	Function name:		SInt16 Write_Homogeneous_Fields_File
//
//	Software purpose:	Write homogeneous field numbers to disk file.
//							field_number : It allows 2 bytes (unsigned short int) for 
//							field Numbers. So the maximum number of fields is
//								2 ** 16 = 65546
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	error_code
//
//	Calls to the functions in other files:
//							setup_echo_image_selection ()	in SAux_echo.cpp
//
// Called By:			EchoClassifier in SClassifyEcho.cpp
//
//	Coded By:			Byeungwoo Jeon			Date: 01/01/1989
//	Revised By:			Byeungwoo Jeon			Date: 09/28/1991
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

SInt16 Write_Homogeneous_Fields_File (
				HUCharPtr								ioBuffer1Ptr, 
				common_classifier_information		*common_info, 
				EchoClassifierVar						*echo_info)

{  
	CMFileStream*						resultsFileStreamPtr;
	FileStringPtr						resultsFileNamePtr;
	HUInt32Ptr							fieldLikelihoodTableIndexPtr;
   HUInt32Ptr							field_number_table;	
  	HSInt32Ptr							epix_ptr;
  	HSInt32Ptr							li_ptr;
   HUCharPtr							c1_ptr;
   HUInt16Ptr							si_ptr1;
   FileInfoPtr							resultsFilePtr;
	pixel_information					*pixel_info;
   
   SInt32								ibuf,
   										lineCount,
											num_col,
											num_row,
											iy;
   										
   UInt32								count,
   										epixLineBufferCount,
   										ix,
   										max_field_number,
											numberBytes;
   							
   SInt16								background,
											byteCode,
											errCode,
   										tableIndex;
   										
 	Boolean								useTempDiskFileFlag;
  											   							
   
	resultsFilePtr = GetResultsFilePtr (2);
	
	if (resultsFilePtr == NULL || ioBuffer1Ptr == NULL)
																						return (1);
																				
	resultsFileStreamPtr = GetFileStreamPointer (resultsFilePtr);
	
	LoadDItemStringNumber (kClassifyStrID,
								 IDS_Classify44,		// "\pSaving Homogeneous Fields"
									gStatusDialogPtr,
									IDC_Status21,
									(Str255*)gTextString);
	
	LoadDItemValue (gStatusDialogPtr, 
							IDC_Status22, 
							(SInt32)echo_info->number_of_fields + 1);
   
   pixel_info = &(common_info->pixel_info);

			// Set up Local Variables 

   num_row 	= pixel_info->num_row;
   num_col 	= pixel_info->num_col;
   
   		// Allow for background class
   		
   resultsFilePtr->numberClasses = echo_info->number_of_fields + 1;
   
   		// Get byte code for the number of bytes to use to represent
   		// the output field numbers.
   	
   if (resultsFilePtr->numberClasses <= 256)	//
   	{
  		byteCode = 1;
  		numberBytes = (SInt32)num_col;
  		
  		}	// end "if (resultsFilePtr->numberClasses <= 256)"

	else if (resultsFilePtr->numberClasses <= UInt16_MAX)
		{
		byteCode = 2;
		numberBytes = (SInt32)num_col * 2;
		resultsFilePtr->numberBytes = 2;
		resultsFilePtr->numberBits	= 16;
		
		}	// end "else if (resultsFilePtr->numberClasses <= UInt16_Max)"
				
	else	// resultsFilePtr->numberClasses > UInt16_Max)
		{
		byteCode = 4;
		numberBytes = (SInt32)num_col * 4;
		resultsFilePtr->numberBytes = 4;
		resultsFilePtr->numberBits	= 32;
		
		if (resultsFilePtr->format != kTIFFType)
			{
			resultsFilePtr->numberChannels = 1;
			resultsFilePtr->format = kTIFFType;
			resultsFilePtr->thematicType = FALSE;
		
			resultsFileNamePtr =
							(FileStringPtr)GetFilePathPPointerFromFileInfo (resultsFilePtr);
			RemoveCharsNoCase ((char*)"\0.gis\0", resultsFileNamePtr);
			RemoveCharsNoCase ((char*)"\0.tif\0", resultsFileNamePtr);
			ConcatFilenameSuffix (resultsFileNamePtr, (StringPtr)"\0.tif\0");
		
			SetFileDoesNotExist (resultsFileStreamPtr, kKeepUTF8CharName);
			
			}	// end "if (resultsFilePtr->format != kTIFFType)"
		
		}	// end "else resultsFilePtr->numberClasses > UInt16_Max)"
		
	//}	// end "else ...numberClasses > 256"
   
   background = 0;
	
			// Now create the file. It was not created in CreateResultsDiskFile since
			// we did not know how many classes there were going to be.
	
	errCode = CreateNewFile (resultsFileStreamPtr, 
									 GetVolumeReferenceNumber (resultsFileStreamPtr), 
									 gCreator, 
									 kErrorMessages, 
									 kReplaceFlag);
	
	if (errCode != noErr)
																							return (1);
							
			// Now write the header.
   	
	if (!WriteNewImageHeader (NULL,
										resultsFilePtr, 
										(char*)gTextString,
										NULL,
										kFromClassification,
										kNewFileMenuItem, 
										1,
										1,
										kClassDisplay,
										kDefaultColors))
																							return (1);

			//   The field number of each homogeneous field is written to the disk
			//   file. The values of Inhomogeneous cells are set to 0.
			//
			//   Note: "non-homogeneous Cells" are counted as a separate class.
            
   epix_ptr = echo_info->epix_ibufPtr; 
	useTempDiskFileFlag = echo_info->useTempDiskFileFlag; 
   field_number_table = echo_info->field_number_table;
   fieldLikelihoodTableIndexPtr = echo_info->fieldLikelihoodTableIndexPtr;
	epixLineBufferCount = num_col * sizeof (SInt32);
	
			// Create table to convert field numbers to consecutive numbers.		
			// They may not be if fields were combined.									
			
   max_field_number = echo_info->current_max_field_number;
	tableIndex = 0; 
	for (ix=1; ix<=max_field_number ; ix++)
		{
		if (field_number_table[ix] == ix)
			fieldLikelihoodTableIndexPtr[ix] = ++tableIndex;
		
		}	// end "for (ix=0; ix< ; ix++)" 
	
	errCode = noErr;
	if (useTempDiskFileFlag)
		{
		errCode = MSetMarker (echo_info->tempFileStreamPtr, 
										fsFromStart, 
										0, 
										kErrorMessages);
		
		if (errCode != noErr)
																					return (errCode);
		
		}	// end "if (useTempDiskFileFlag)"
					
	lineCount = 0;    
   	
   for (ix=0; ix<(UInt32)num_row; ix++) 
   	{
				// Display line status information.											
				
		lineCount++;
		if (TickCount () >= gNextStatusTime)
			{
			LoadDItemValue (gStatusDialogPtr, IDC_Status18, (SInt32)lineCount);
			gNextStatusTime = TickCount () + gNextStatusTimeOffset;
			
			}	// end "if (TickCount () >= gNextStatusTime)" 
		
				// Exit routine if user has "command period" down						
		
 		if (TickCount () >= gNextTime)
			{
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
				{
																							return (1);
				
				}	// end "if (!CheckSomeEvents..." 
			
			}	// end "if (TickCount () >= gNextTime)"
			
		if (useTempDiskFileFlag)
			{
			count = epixLineBufferCount;
			epix_ptr = echo_info->epix_ibufPtr;
			errCode = MReadData (echo_info->tempFileStreamPtr, 
											&count, 
											epix_ptr,
											kErrorMessages);
			
			if (errCode != noErr)
																							return (1);
			
			}	// end "if (useTempDiskFileFlag)"
			
		switch (byteCode)
			{
   		case 1: 
	   		c1_ptr = ioBuffer1Ptr;
	   		
		   	for (iy=0; iy<num_col; iy++) 
		   		{
		   		if (*epix_ptr > 0)		// Homogeneous cell 
		   			{
						ibuf = *epix_ptr & 0xbfffffff;
	      			*c1_ptr =
							(UInt8)fieldLikelihoodTableIndexPtr[field_number_table[ibuf]];
	      								
	      			}	// end "if (*epix_ptr > ZERO)" 
	      			
	      		else	// *epix_ptr == ZERO 
	      			*c1_ptr = (UInt8)background;
	      
	      	   epix_ptr++;
	      	   c1_ptr++;
	      	   
	   	   	}	// end "for (iy=0;iy<num_col;iy++)"
	   	   break; 
     
   		case 2: 
	   		si_ptr1 	= (HUInt16Ptr)ioBuffer1Ptr;
	   		
		   	for (iy=0;iy < num_col;iy++) 
		   		{
		   		if (*epix_ptr > 0)		// Homogeneous cell 
		   			{
						ibuf = *epix_ptr & 0xbfffffff;
	      			*si_ptr1 =
							(UInt16)fieldLikelihoodTableIndexPtr[field_number_table[ibuf]];
	      			
	      			}	// end "if (*epix_ptr > ZERO)" 
	      			
	      		else	// *epix_ptr == ZERO 
	      			*si_ptr1 = background;
	      
	      	   epix_ptr++;
	      	   si_ptr1++;
	      	   
	   	   	}	// end "for (iy=0; iy<num_col; iy++)" 
	   	   break; 
     
   		case 4: 
	   		li_ptr 	= (HSInt32Ptr)ioBuffer1Ptr;
	   		
		   	for (iy=0; iy<num_col; iy++) 
		   		{
		   		if (*epix_ptr > 0)		// Homogeneous cell 
		   			{
						ibuf = *epix_ptr & 0xbfffffff;
	      			*li_ptr =
							(SInt32)fieldLikelihoodTableIndexPtr[field_number_table[ibuf]];
	      			
	      			}	// end "if (*epix_ptr > ZERO)" 
	      			
	      		else	// *epix_ptr == ZERO 
	      			*li_ptr = background;
	      
	      	   epix_ptr++;
	      	   li_ptr++;
	      	   
	   	   	}	// end "for (iy=0; iy<num_col; iy++)" 
	   	   break; 
	   	   
	  		}	// end "switch (byteCode)"
           	
     	errCode = MWriteData (resultsFileStreamPtr, 
     									&numberBytes, 
     									ioBuffer1Ptr,
     									kErrorMessages);	
		if (errCode != noErr)
																							return (1);
   	   	
		}	// end "for (ix=0; ix<num_row; ix++)" 
		
	LoadDItemValue (gStatusDialogPtr, IDC_Status18, (SInt32)lineCount);
		
   return (errCode);

}	// End of Write_Homogeneous_Fields_File 



//------------------------------------------------------------------------------------
//                   Copyright 1989-2020 Purdue Research Foundation
//
//	Function name:		SInt16 Write_Classified_Fields_File
//
//	Software purpose:	Write classified homogeous fields to disk file.
//								file_boundary
//									num_row = pixel_info.num_row;
//									num_col = pixel_info.num_col;
//								homog_fields : It allows 1 byte (unsigned char) for field 
//									class. Thus there can be maximum 2**8 = 256 
//									different classes. (Actually 255 since the 
//									background (which is non-homogeneous cells) is
//									counted as one class)
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	error_code
//
//	Calls to the functions in other files:
//							setup_echo_image_selection ()	in SAux_echo.cpp
//
// Called By:			EchoClassifier in SClassifyEcho.cpp
//
//	Coded By:			Byeungwoo Jeon			Date: 01/01/1989
//	Revised By:			Byeungwoo Jeon			Date: 09/28/1991
//	Revised By:			Larry L. Biehl			Date: 02/14/2014

SInt16 Write_Classified_Fields_File (
				HUCharPtr								ioBuffer1Ptr, 
				common_classifier_information		*common_info, 
				EchoClassifierVar						*echo_info)

{  
	CMFileStream*						resultsFileStreamPtr;
	FileInfoPtr							resultsFilePtr;
  	HSInt16Ptr							fieldClassNumberPtr;
  	HSInt32Ptr							epix_ptr;
   HUCharPtr							c1_ptr;
  	HUInt32Ptr							field_number_table;
	pixel_information					*pixel_info;
   
   SInt32								ibuf,
   										lineCount,
											num_col,
											num_row,
											ix,
											iy;
   										
   UInt32								count,
   										epixLineBufferCount,
   										numberBytes;
   							
   SInt16								background,
											*classPtr,
   										classNumber,
   										errCode;
   										
   Boolean								useTempDiskFileFlag;
   										
   										
	resultsFilePtr = GetResultsFilePtr (3);
	
	if (!resultsFilePtr || !ioBuffer1Ptr)
																						return (1);
																				
	resultsFileStreamPtr = GetFileStreamPointer (resultsFilePtr);
	
   pixel_info 	= &(common_info->pixel_info);

			// Set up Local Variables 

   num_row 			= pixel_info->num_row;
   num_col 			= pixel_info->num_col;
   background 		= 0;			// Non-homogeneous Cell	
   numberBytes 	= (SInt32)num_col;
   
	LoadDItemStringNumber (kClassifyStrID,
								 	IDS_Classify45,	// "Saving Classified Homogeneous Fields"
									gStatusDialogPtr,
									IDC_Status21,
									(Str255*)gTextString);
	LoadDItemValue (gStatusDialogPtr, 
							IDC_Status22, 
							(SInt32)resultsFilePtr->numberClasses);

			//	Make  Homogeneous Region  Image
			//	
			// 1. homog_flds : Show field classes of homogeneous cells.
			// The values of Inhomogeneous cells are initialized 
			// at read_field_boundary as "background" (0)

   epix_ptr 	= echo_info->epix_ibufPtr; 
	useTempDiskFileFlag = echo_info->useTempDiskFileFlag;
   fieldClassNumberPtr  = echo_info->fieldClassNumberPtr;
   field_number_table 	= echo_info->field_number_table; 
	epixLineBufferCount = num_col * sizeof (SInt32); 
	lineCount = 0;    
	
			// Get the class number pointer.  Subtract 1 from the pointer since	
			// class numbers start from 1 no zero.  This will save many				
			// subtractions.																		
			
   classPtr	= gClassifySpecsPtr->classPtr - 1;
					
	errCode = noErr;
	if (useTempDiskFileFlag)
		{
		errCode = MSetMarker (echo_info->tempFileStreamPtr, 
										fsFromStart, 
										0, 
										kErrorMessages);
		
		if (errCode != noErr)
																				return (errCode);
		
		}	// end "if (useTempDiskFileFlag)"
	    
   for (ix=0; ix<num_row; ix++) 
   	{
				// Display line status information.											
				
		lineCount++;
		if (TickCount () >= gNextStatusTime)
			{
			LoadDItemValue (gStatusDialogPtr, IDC_Status18, (SInt32)lineCount);
			gNextStatusTime = TickCount () + gNextStatusTimeOffset;
			
			}	// end "if (TickCount () >= gNextStatusTime)" 
		
				// Exit routine if user has "command period" down						
		
 		if (TickCount () >= gNextTime)
			{
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
				{
																					return (1);
				
				}	// end "if (!CheckSomeEvents..." 
			
			}	// end "if (TickCount () >= gNextTime)"
			
		if (useTempDiskFileFlag)
			{
			count = epixLineBufferCount;
			epix_ptr = echo_info->epix_ibufPtr;
			errCode = MReadData (echo_info->tempFileStreamPtr, 
											&count, 
											epix_ptr,
											kErrorMessages);
			
			if (errCode != noErr)
																					return (1);
			
			}	// end "if (useTempDiskFileFlag)"
			
   	c1_ptr = ioBuffer1Ptr;
   
		for (iy=0; iy<num_col; iy++) 
			{
			ibuf = *epix_ptr;
			if (ibuf > 0)		// Homogeneous cell 
				{
				ibuf &= 0xbfffffff;
      		classNumber = (UInt8)fieldClassNumberPtr[field_number_table[ibuf]];
      		*c1_ptr = (UInt8)classPtr[classNumber];
      		
      		}	// end "if (ibuf > ZERO)" 
      		
      	else	// ibuf <= ZERO 
      		*c1_ptr = (UChar)background;
      
      	epix_ptr++;
      	c1_ptr++;
      	   
   		}	// end "for (iy=0; iy<num_col; iy++)"
     	
     	errCode = MWriteData (resultsFileStreamPtr, 
     									&numberBytes, 
     									ioBuffer1Ptr,
     									kErrorMessages);	
     									
		if (errCode != noErr)
																					return (1);
   	
   	}	// end for "(ix=0; ix<num_row; ix++)" 
		
	LoadDItemValue (gStatusDialogPtr, IDC_Status18, (SInt32)lineCount);
   
   return (errCode);

}	// end "Write_Classified_Fields_File"

			
