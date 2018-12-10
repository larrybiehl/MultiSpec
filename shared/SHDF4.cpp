//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SHDF.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			10/17/2018
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	This file contains routines which are used to access
//								various disk files.
//
//	Functions in file:	
//
//	Diagram of MultiSpec routine calls for the routines in the file.
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
/*	Template for debugging for MultiSpec Online on mygeohub.
	int numberChars = sprintf ((char*)gTextString3,
												" SHDF4: (filePathPtr hDS): %s %ld%s", 
												filePathPtr,
												hDS,
												gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);	
*/		
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
	
#if defined multispec_mac
	#define	kFileTypeStrID				175
	#define	IDS_FileType01				1		
#endif	// defined multispec_mac    

#if defined multispec_lin
	#include "SMultiSpec.h"
	#define TRUE true
	#define FALSE false
#endif

#if defined multispec_win
	#include "SMultiSpec.h"
#endif	// defined multispec_win

//#include "SExtGlob.h"

#if include_gdal_capability
	// oul: added definition of SIZE_UNSIGNED_LONG and SIZEOF_VOIDP
	// which are not defined in cpl_config.h

	#if defined multispec_lin
		#if defined NetBeansProject
			/* The size of a 'unsigned long', as computed by sizeof. */
			#define SIZEOF_UNSIGNED_LONG 8
			/* The size of a 'void p', as computed by sizeof. */
			#define SIZEOF_VOIDP 8
		#endif
	#endif
	
	#include "gdal.h"
	#include "cpl_error.h"
#endif	// include_gdal_capability

#if include_hdf_capability
	extern void				SetHDF4HasHFSPlusAPISetting (
									Boolean								hasHFSPlusAPIsFlag);
#endif	// 

Boolean						CheckIfHDF5FormattedFile (
									FileInfoPtr 						fileInfoPtr);


#if include_gdal_capability
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CheckIfHDF5FormattedFile
//
//	Software purpose:	This routine uses GDAL to determine if the hdf formatted file
//							is actually in hdf5 format and not hdf4 format.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/22/2010
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

Boolean CheckIfHDF5FormattedFile (
				FileInfoPtr 						fileInfoPtr)

{
	UInt8									filePathString[512];
	UCharPtr								fileFormatDescriptionPtr;
	FileStringPtr						filePathPtr;
	
	GDALDatasetH						hDS;
	GDALDriverH							dataSetDriver;
	
	SInt16								returnCode;
	
	Boolean								returnFlag = FALSE;


	returnCode = GetHDFFilePathCPointer (fileInfoPtr, 
													 filePathString, 
													 510,
													 &filePathPtr,
													 kHDF4_Library);	
	if (returnCode == noErr)	
		{
		GDALAllRegister ();

		hDS = GDALOpen ((char*)filePathPtr, GA_ReadOnly);
		if (hDS != NULL)
			{		
			dataSetDriver = GDALGetDatasetDriver (hDS);
			fileFormatDescriptionPtr = 
								(UCharPtr)GDALGetDescription ((GDALMajorObjectH)dataSetDriver);
		
		
			if (CompareStringsNoCase (fileFormatDescriptionPtr, (UCharPtr)"HDF5\0") == 0)
				returnFlag = TRUE;
				
			GDALClose (hDS);
				
			}	// end "if (hDS != NULL)"
		
		}	// end "if (returnCode == noErr)"
	
	return (returnFlag);
	
}	// end "CheckIfHDF5FormattedFile"
#endif	// include_gdal_capability


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void AppendGroupIndicater
//
//	Software purpose:	This routine appends the group identifier to the data set
//							identifier name making sure that the total length of the string
//							with pascal length and c-string terminator is not more than 80
//							characters.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/18/2006
//	Revised By:			Larry L. Biehl			Date: 06/18/2006

void AppendGroupIndicater (
				char*									namePtr,
				UInt32								groupNumber)
				
{
	SInt16								numChars;
	
	UInt16								charIndex;
	
	
			// Append indicater that data set is part of a group.
			
	charIndex = namePtr[0] + 1;
	charIndex = MIN (charIndex, 75);
	
			// Group number is limited to 2 digits.  Should never come close to this.
			
	groupNumber = MIN (groupNumber, 99);
	numChars = sprintf (&namePtr[charIndex], "_G%ld", groupNumber);
	namePtr[0] += (UInt8)numChars;
	charIndex = namePtr[0];
	namePtr[charIndex+1] = 0;
	
}	// end "AppendGroupIndicater"

		

//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CreateFullDataSetIdentifierName
//
//	Software purpose:	This routine creates the data set identifier name based on the
//							file name and the data set name.  The total length of this 
//							string cannot be more than 73 characters to allow for data set
//							prefix number (* or _), group indicator (_Gxx), pascal string length
//							and c-string terminator. Use '_' as space between file name and
//							data set name.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	
//
// Called By:			GetImageWindowName in SStrings.cpp
//
//	Coded By:			Larry L. Biehl			Date: 07/28/2006
//	Revised By:			Larry L. Biehl			Date: 03/14/2017

void CreateFullDataSetIdentifierName (
				FileInfoPtr 						fileInfoPtr, 
				SInt16								hdfDataSetIndex,
				StringPtr							fullDataSetNamePtr,
				SInt16								nameLengthLimit)
				
{
	UInt8									fileName[256];
	Str255								dataSetName;
	
	SInt16								dataSetNameLength,
											fileNameLength,
											numberFileNameCharsToCopy;
	
	
	fullDataSetNamePtr[0] = 0;
	
	if (fileInfoPtr != NULL)
		{
				// Get file name.
					
		GetHdfHeaderFileName (fileInfoPtr, fileName);
		RemoveSuffix (fileName);
		
				// Get the data set name.
				
		GetHdfDataSetName (fileInfoPtr, 
									hdfDataSetIndex,
									dataSetName,
									NULL);
						
		fileNameLength = fileName[0];
		dataSetNameLength = dataSetName[0];
			
		numberFileNameCharsToCopy = fileNameLength;
		if (fileNameLength + dataSetNameLength + 1 > nameLengthLimit)
			{
			numberFileNameCharsToCopy = nameLengthLimit - 1 - dataSetNameLength;
			numberFileNameCharsToCopy = MAX (0, numberFileNameCharsToCopy);
				
			}	// end "if (fileNameLength + dataSetNameLength + 1 > ..."
				
				// Insert the file name that is to be used.
				
		strncpy ((char*)&fullDataSetNamePtr[1], 
						(char*)&fileName[1], 
						numberFileNameCharsToCopy);
			
				// Add the data set name.
					
		sprintf ((char*)&fullDataSetNamePtr[1+numberFileNameCharsToCopy], 
						"_%s", 
						&dataSetName[1]);
						
		fullDataSetNamePtr[0] = numberFileNameCharsToCopy + 1 + dataSetNameLength;
						
		}	// end "if (fileInfoPtr != NULL)"
					
}	// end "CreateFullDataSetIdentifierName"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean DetermineIfHDFProjectPossible
//
//	Software purpose:	The purpose of this routine is to determine that if the
//							input image file is in HDF format, can it be used as the
//							base image for a project.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/20/2001
//	Revised By:			Larry L. Biehl			Date: 12/20/2001

Boolean DetermineIfHDFProjectPossible (
				Handle								imageWindowInfoH)

{	
	FileInfoPtr							fileInfoPtr;
	
	Handle								fileInfoHandle;
	

	fileInfoHandle = GetFileInfoHandle (imageWindowInfoH);
	fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
				
	if (fileInfoPtr != NULL)
		{
		if (fileInfoPtr->format == kHDF4Type || fileInfoPtr->format == kNETCDFType)
			{
			if (fileInfoPtr->hdfDataSetSelection == 1)
																						return (TRUE);
																				
			else	// fileInfoPtr->hdfDataSetSelection > 1
																						return (TRUE);
																				
			}	// end "if (...->format == kHDF4Type || ...->format == kNETCDFType)"
				
				// This is not an HDF or netCDF type of image file. Assume for now that it 
				// can be used as the base image for a project.				
																				
		return (TRUE);
		
		}	// end "if (fileInfoPtr != NULL)"
		
	return (FALSE);
	
}	// end "DetermineIfHDFProjectPossible" 
				


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 DisplayHDFAlerts
//
//	Software purpose:	The purpose of this routine is to display any alerts
//							after reading an HDF data set.
//
//	Parameters in:		
//
//	Parameters out:	None
//
//	Value Returned:				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 11/29/2001
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

SInt16 DisplayHDFAlerts (
				SInt16								alertCode,
				FileInfoPtr							fileInfoPtr)
 
{	
	unsigned char						formatName[256];
	
	char*									fileNamePtr;
	
	SInt16								format,
											returnCode = 0,
											stringLength;
											
	Boolean								continueFlag;
	
		
	if (alertCode != noErr)
		{								
				// Display an alert indicating that MultiSpec could not read this 
				// HDF file.
				
		if (alertCode == 8)
			{
					// Date set file not found in the same folder as the header.
					
			fileNamePtr = (char*)GetFileNameCPointerFromFileInfo (fileInfoPtr);
					
			if (LoadSpecifiedStringNumberStringP (kAlertStrID, 
																IDS_Alert95 + alertCode - 1, 
																(char*)gTextString,
																(char*)gTextString2,
																TRUE,
																(char*)&fileNamePtr[1]))
				DisplayAlert (kErrorAlertID, kStopAlert, 0, 0, 0, gTextString);
			
			stringLength = gTextString[0];
			sprintf ((char*)&gTextString[stringLength+1],
							"%s%s",
							gEndOfLine,
							gEndOfLine);
									
			OutputString (NULL, 
								(char*)&gTextString[1], 
								0, 
								gOutputForce1Code, 
								TRUE,
								kUTF8CharString);
												
			}	// end "if (alertCode == 8)"
												
		else	// alertCode != 8								
			{
			format = fileInfoPtr->format;		
			format += IDS_FileType01;

			continueFlag = GetSpecifiedStringNumber (
											kFileTypeStrID, format, formatName, TRUE);
						
			continueFlag = LoadSpecifiedStringNumberStringP (
											kAlertStrID, 
											(SInt16)(IDS_Alert95 + alertCode - 1), 
											(char*)gTextString,
											(char*)gTextString2,
											continueFlag,
											(char*)&formatName[1]);
				
			if (continueFlag)
				DisplayAlert (kErrorAlertID, 
									kStopAlert, 
									0, 
									0,
									0, 
									gTextString);
											
//			DisplayAlert (kErrorAlertID, 
//								kStopAlert, 
//								kAlertStrID, 
//								IDS_Alert95 + alertCode - 1,
//								0, 
//								NULL);
								
			}	// end "else alertCode != 8"
							
		returnCode = 1;
		
		}	// end "if (alertCode != noErr)"
		
	return (returnCode);
	
}	// end "DisplayHDFAlerts" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt16 GetHdfDataSetInstrumentChannelNumber
//
//	Software purpose:	This routine returns the specified hdf data set channel number.
//							It will take into account whether this is part of a group of 
//							data sets. This is currently only used for ASTER image files.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/28/2005
//	Revised By:			Larry L. Biehl			Date: 09/11/2018

UInt16 GetHdfDataSetInstrumentChannelNumber (
				FileInfoPtr 						fileInfoPtr,
				UInt16								fileChannelNumber)

{	
	UInt16*								channelToHdfDataSetPtr;
	HdfDataSets*						hdfDataSetsPtr;
	
	SInt16								hdfDataSetIndex,
											instrumentChannelNumber;
	
	
	hdfDataSetsPtr = (HdfDataSets*)GetHandlePointer (fileInfoPtr->hdfHandle);
	
	instrumentChannelNumber = 0;
	
			// Note that the number of bytes check is to ignore those ASTER data
			// sets which are not 2-byte data as a quick & dirty way to not assign
			// wavelengths to channels which are probably something else.  There
			// has to be a better way to check for this.
			
	if (hdfDataSetsPtr != NULL && 
						(fileInfoPtr->numberBytes == 1 || fileInfoPtr->numberBytes == 2))
		{
		channelToHdfDataSetPtr = (UInt16*)GetHandlePointer (
															fileInfoPtr->channelToHdfDataSetHandle);
		
		//hdfDataSetIndex = fileInfoPtr->hdfDataSetSelection;
		hdfDataSetIndex = channelToHdfDataSetPtr[fileChannelNumber-1];
		
		instrumentChannelNumber = hdfDataSetsPtr[hdfDataSetIndex].instrumentChannelNumber;
		/*
				// Now determine if this is part of a group of hdf data sets.
						
		if (hdfDataSetsPtr[hdfDataSetIndex].groupedNumber > 0)
			{
			if (fileInfoPtr->format == kHDF4Type)
				{
				
				hdfDataSetIndex = hdfDataSetsPtr[hdfDataSetIndex].dataSet + 1;
				
						// Beginning in 9/2018, allowance is made for the channels for
						// ASTER to not be in order in HDF4 formatted files.
			
				instrumentChannelNumber = hdfDataSetsPtr[
								hdfDataSetIndex+fileChannelNumber-1].instrumentChannelNumber;
				
				}	// end "if (fileInfoPtr->format != kHDF4Type)"
			
			else	// fileInfoPtr->format != kHDF4Type
				{
				hdfDataSetIndex = fileInfoPtr->numberHdfDataSets +
													hdfDataSetsPtr[hdfDataSetIndex].groupedNumber;
				instrumentChannelNumber =
						hdfDataSetsPtr[hdfDataSetIndex].instrumentChannelNumber +
																					fileChannelNumber - 1;
				
				}	// end "if (fileInfoPtr->format != kHDF4Type)"
			
			}	// end "if (hdfDataSetsPtr[hdfDataSetIndex].groupedNumber > 0)"
		*/
		}	// end "if (hdfDataSetsPtr != NULL && fileInfoPtr->numberBytes == 2)"
		
	return (instrumentChannelNumber);
	
}	// end "GetHdfDataSetInstrumentChannelNumber" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetHdfDataSetName
//
//	Software purpose:	This routine returns the specified hdf data set name for
//							the input file information structure.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:			HistogramDialogInitialize in SHistgrm.cpp
//							GetDefaultSupportFileName in SOpnImag.cpp
//							ListDescriptionInformation in SOther.cpp
//							GetImageWindowName in SStrings.cpp
//							ListProcessorTitleLine in SStrings.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/30/2001
//	Revised By:			Larry L. Biehl			Date: 03/11/2005

void GetHdfDataSetName (
				FileInfoPtr 						fileInfoPtr, 
				SInt16								hdfDataSetIndex,
				StringPtr							dataSetNamePtr,
				UInt16*								dataSetPtr)

{	
	HdfDataSets*						hdfDataSetsPtr;
	
	SInt16								dataSet,
											numChars;
	
	
	hdfDataSetsPtr = (HdfDataSets*)GetHandlePointer (fileInfoPtr->hdfHandle);
	
	dataSetNamePtr[0] = 0;
	dataSet = 0;
	
	if (hdfDataSetsPtr != NULL)
		{
		dataSet = hdfDataSetsPtr[hdfDataSetIndex].dataSet;
		
				// Now determine if this is part of a group of hdf data sets.
						
		if (hdfDataSetsPtr[hdfDataSetIndex].groupedNumber > 0)
			hdfDataSetIndex = fileInfoPtr->numberHdfDataSets +
													hdfDataSetsPtr[hdfDataSetIndex].groupedNumber;
			
				// Do not include the leading character. Copy the c terminator.
		
		numChars = hdfDataSetsPtr[hdfDataSetIndex].name[0];
		
		if (numChars > 0)
			{
			memcpy ((char*)&dataSetNamePtr[1], 
						(char*)&hdfDataSetsPtr[hdfDataSetIndex].name[2],
						numChars);
	
			dataSetNamePtr[0]	= numChars - 1;
			
			if (dataSetPtr != NULL)
				*dataSetPtr = dataSet;
			
			}	// end "if (numChars > 0)"		
		
		}	// end "if (hdfDataSetsPtr != NULL)"
	
}	// end "GetHdfDataSetName" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetHDFFilePathCPointer
//
//	Software purpose:	This routine returns a pointer to the file path used by the 
//							hdf routines.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 10/08/2008
//	Revised By:			Larry L. Biehl			Date: 09/18/2017

SInt16 GetHDFFilePathCPointer (
				FileInfoPtr		 					fileInfoPtr,
				FileStringPtr						filePathStringPtr,
				SInt16								fileStringLength,
				FileStringPtr*						filePathCPtrPtr,
				SInt16								libraryCode)
				
{	
	FileStringPtr							filePathPPtr;
	
   
	filePathPPtr = (FileStringPtr)GetFilePathPPointerFromFileInfo (fileInfoPtr);
	*filePathCPtrPtr = &filePathPPtr[1];
	
	#if defined multispec_mac
		CMFileStream*						fileStreamPtr;
		
		SInt16								errCode,
												hdfAlertCode;

		#if include_hdf_capability
			SetHDF4HasHFSPlusAPISetting (gHasHFSPlusAPIs);
		#endif	// 
		fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
		
		if (fileStreamPtr->fSRefFlag)
			{
					// Make the path include the entire full name.
					
			errCode = FSRefMakePath (&fileStreamPtr->fsRef,
												(UInt8*)filePathStringPtr,
												fileStringLength);
			
			if (errCode == noErr)
				*filePathCPtrPtr = filePathStringPtr;
				
			else	// errCode == noErr
				{	
				if (errCode == -2110)
					hdfAlertCode = 4;
					
				else	// errCode != -2110 || 0
					hdfAlertCode = 5;
				
																				return hdfAlertCode;
				
				}	// end "else errCode == noErr"
			
			}	// end "if (fileStreamPtr->fSRefFlag)"
	#endif	// defined multispec_mac	

	#if defined multispec_lin || defined multispec_win
		if (libraryCode == kGDAL_Library)
			{
			FileStringPtr								filePathCPtr;
			
			filePathCPtr = (FileStringPtr)GetFilePathPPointerFromFileInfo (fileInfoPtr);

					// Need to return a c pointer.
			
			if (filePathCPtr != NULL)
				filePathCPtr = &filePathCPtr[1];

			*filePathCPtrPtr = filePathCPtr;

			}	// end "if (libraryCode == kGDAL_Library)"
	#endif		// defined multispec_lin || defined multispec_win

return (0);
    
}	// end "GetHDFFilePathCPointer"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetHdfHeaderFileName
//
//	Software purpose:	This routine returns the specified hdf data set name for
//							the input file information structure.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/02/2001
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

void GetHdfHeaderFileName (
				FileInfoPtr 						fileInfoPtr, 
				UCharPtr								headerFileNamePtr)

{	
	HdfDataSets*						hdfDataSetsPtr;
	
	SInt16								numChars;
	
	
			// Make empty string for both c and pascal string.
			
	headerFileNamePtr[0] = 0;
	headerFileNamePtr[1] = 0;	
	
	hdfDataSetsPtr = (HdfDataSets*)GetHandlePointer (fileInfoPtr->hdfHandle);
									
	if (hdfDataSetsPtr != NULL)
		{
				// Do not include the leading character. Copy the c terminator.
		
		numChars = hdfDataSetsPtr[0].name[0];
		
		if (numChars > 0)
			CopyPToP (headerFileNamePtr, hdfDataSetsPtr[0].name);
		
		}	// end "if (hdfDataSetsPtr != NULL)"
	
}	// end "GetHdfHeaderFileName" 


#if include_hdf_capability
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 ReadHDFHeader
//
//	Software purpose:	This routine reads the file specifications from
//							a HDF formatted file.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/25/1995
//	Revised By:			Larry L. Biehl			Date: 11/29/2018

SInt16 ReadHDFHeader (
				FileInfoPtr 						fileInfoPtr, 
				char*									headerRecordPtr,
				SInt16								formatOnlyCode)

{	
	unsigned char						formatName[256];
	
	SInt16								format,
											returnCode = 1;
	
	Boolean								dataSetSpecifiedFlag = FALSE;
	
	unsigned char						charKeyCode[4];
	
	CMFileStream*						fileStreamPtr;
	
	SInt32								hdfReturn;
	
	SInt16								errCode,
											fileType = 0;
										
	Boolean								continueFlag;

	
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);					
		
	if (fileInfoPtr != NULL && fileStreamPtr != NULL)		
		{
				// Check if HDF formatted image file.
				
		charKeyCode[0] = 0x0e;
		charKeyCode[1] = 0x03;
		charKeyCode[2] = 0x13;
		charKeyCode[3] = 0x01;
		
		if (strncmp ((char*)headerRecordPtr, (CharPtr)charKeyCode, 4) == 0)
			fileType = kHDF4Type;
		
		//#if !defined multispec_wxmac
			if (fileType == 0)
				{
						// Check if netCDF formatted image file.
				
				charKeyCode[0] = 'C';
				charKeyCode[1] = 'D';
				charKeyCode[2] = 'F';
				charKeyCode[3] = 0x01;
				
				if (strncmp ((char*)headerRecordPtr, (CharPtr)charKeyCode, 4) == 0)
					fileType = kNETCDFType;
				
				}	// end "if (fileType == 0)"
		//#endif	// !multispec_wxmac
		/*
				// The library being used does not handle cdf2 format.
				
		if (fileType == 0)
			{
					// Check if netCDF formatted image file.
					
			charKeyCode[0] = 'C';
			charKeyCode[1] = 'D';
			charKeyCode[2] = 'F';
			charKeyCode[3] = 0x02;
			
			if (strncmp ((char*)headerRecordPtr, (CharPtr)charKeyCode, 4) == 0)
				fileType = kNETCDFType;
				
			}	// end "if (fileType == 0)"
		*/							
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
		
		returnCode = 0;
		hdfReturn = noErr; 
		
				// Make sure that the default volume is correct.
				
		errCode = SetVolume (fileStreamPtr, kErrorMessages);
		
		if (fileInfoPtr->hdfDataSetSelection > 0)
			dataSetSpecifiedFlag = TRUE;

		hdfReturn = LoadHDF4Information (fileInfoPtr, fileType, dataSetSpecifiedFlag);
			
		if (hdfReturn != noErr)
			{
					// Display an alert indicating that MultiSpec could not read or had
					// problems reading this HDF or netCDF file.
			
			format = fileType;		
			format += IDS_FileType01;										
		
			continueFlag = GetSpecifiedStringNumber (
											kFileTypeStrID, format, formatName, TRUE);
					
			continueFlag = LoadSpecifiedStringNumberStringP (
												kAlertStrID, 
												(SInt16)(IDS_Alert95 + hdfReturn - 1), 
												(char*)gTextString,
												(char*)gTextString2,
												continueFlag,
												(char*)&formatName[1]);
									
			if (continueFlag)
				{
				if (gLinkOptionsSelection == 1)
					DisplayAlert (kErrorAlertID, 
										kStopAlert, 
										0, 
										0,
										0, 
										gTextString);
										
				else	// gLinkOptionsSelection > 1
					{
					gAlertReturnCode = DisplayAlert (kContinueCancelAlertID, 
																kStopAlert, 
																0, 
																0,
																0, 
																gTextString);
					
					if (gAlertReturnCode == 2)
								// This signifies to stop immediately.
						gAlertReturnCode = 3;
										
					}	// end "else gLinkOptionsSelection > 1"
					
				}	// end "if (continueFlag)"
			
					// If no data sets were found then the hdf file may be hdf5 format.
					// Set format to not known yet to force try for hdf5 format.
			
			if (hdfReturn == 1)
				fileInfoPtr->format = 0;
			
			returnCode = 1;
			
			}	// end "if (hdfReturn != noErr)"
			
		if (returnCode == noErr)
			{
			fileInfoPtr->format = fileType;
			
			if (gGetFileImageType == 0)
				{
				if (fileInfoPtr->thematicType)
					gGetFileImageType = kThematicImageType;
					
				else	// !fileInfoPtr->thematicType 
					gGetFileImageType = kMultispectralImageType;
					
				}	// end "if (gGetFileImageType == 0)"
				
			}	// end "if (returnCode > 0)" 
		
		}	// end "if (fileTpye != 0)" 

	return (returnCode);
	
}	// end "ReadHDFHeader" 
#endif		// include_hdf_capability



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 SetUpHDF_FileInformation
//
//	Software purpose:	This sets up the vector that is used to relate image channels
//							to the hdf scientific data set access values. This is always
//							set up even though it may not be used. It is used when the
//							hdf4 library is used to read the data ... compressed data, netcdf
//							data and some ASTER data when the bands are not in order.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	none.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 10/25/2007
//	Revised By:			Larry L. Biehl			Date: 10/17/2018

SInt16 SetUpHDF_FileInformation (
				FileInfoPtr							fileInfoPtr,
				HdfDataSets*						hdfDataSetsPtr,
				SInt32								hdfDataSetSelection,
				Boolean								useGroupedDataSetsFlag)
				
{	
	UInt16*								channelToHdfDataSetPtr;
	
	UInt32								channelIndex,
											dataSetIndex,
											savedChannelIndex;
	
	SInt16								groupedNumber,
											returnCode = noErr;
	
	
//	if (fileInfoPtr->callGetHDFLineFlag)
//		{
		if (hdfDataSetsPtr == NULL)
																								return (1);
			
				// Get vectors for the sdids.
				// Get new handle if for channel to hdf data set structure if needed
	
		fileInfoPtr->channelToHdfDataSetHandle = MNewHandle (
												fileInfoPtr->numberChannels * sizeof (UInt16));
		if (fileInfoPtr->channelToHdfDataSetHandle == NULL)
																								return (1);
		
		channelToHdfDataSetPtr = (UInt16*)GetHandlePointer (
															fileInfoPtr->channelToHdfDataSetHandle);
		
				// Get the group number for the data set.
		
		groupedNumber = 0;
		if (useGroupedDataSetsFlag)
			groupedNumber = hdfDataSetsPtr[hdfDataSetSelection].groupedNumber;
		
		if (groupedNumber > 0)
			{
			if (fileInfoPtr->instrumentCode == kASTER &&
						fileInfoPtr->dataCompressionCode == kNoCompression &&
								fileInfoPtr->format == kHDF4Type)
				fileInfoPtr->callGetHDFLineFlag = FALSE;
				
				// Now find the data sets for each of the channels in the group.
				// Note that 'hdfDataSetSelection' may not point to the first channel
				// in the group so go through the entire list of data sets.
			
			channelIndex = 0;
			for (dataSetIndex=1; 
						dataSetIndex<=fileInfoPtr->numberHdfDataSets; 
								dataSetIndex++) 
				{
				if (groupedNumber == hdfDataSetsPtr[dataSetIndex].groupedNumber)
					{
							// Verify that we have a valid sdid for the data set.
						
					if (hdfDataSetsPtr[dataSetIndex].sdid != NULL)
						{
						savedChannelIndex = channelIndex;
						if (fileInfoPtr->instrumentCode == kASTER &&
								groupedNumber == 1 &&
									fileInfoPtr->numberChannels == 3)
							{
									// Need to handle case for L1T data in which the VNIR
									// channels may be out of order
							
							if (strcmp ((char*)&hdfDataSetsPtr[dataSetIndex].name[2],
											"ImageData1_G1") == 0)
								channelIndex = 0;
							
							else if (strcmp ((char*)&hdfDataSetsPtr[dataSetIndex].name[2],
											"ImageData2_G1") == 0)
								channelIndex = 1;
							
							else if (strcmp ((char*)&hdfDataSetsPtr[dataSetIndex].name[2],
											"ImageData3N_G1") == 0)
								channelIndex = 2;
							
							if (channelIndex != savedChannelIndex)
								fileInfoPtr->callGetHDFLineFlag = TRUE;
							
							}	// end "if (fileInfoPtr->instrumentCode == kASTER)
						
						channelToHdfDataSetPtr[channelIndex] = (UInt16)dataSetIndex;
						
						channelIndex = savedChannelIndex;
						
						}	// end "if (hdfDataSetsPtr[dataSetIndex].sdid != NULL)"
					
					else	// hdfDataSetsPtr[dataSetIndex].sdid <= 0
						{
						returnCode = 1;
						break;
							
						}	// end "else hdfDataSetsPtr[dataSetIndex].sdid <= 0"
						
					channelIndex++;
					
					}	// end "if (groupedNumber == hdfDataSetsPtr[dataSetIndex]...."
					
				if (groupedNumber < hdfDataSetsPtr[dataSetIndex].groupedNumber)
							// No reason to continue checking; 
							// last data set for group was found.
					break;
				
				}	// end "for (dataSetIndex=1; dataSetIndex<=...->numberHdfDataSets; ..."
				
			}	// end "if (groupedNumber > 0)"
			
		else	// groupedNumber = 0
			{
			dataSetIndex = hdfDataSetSelection;
			if (hdfDataSetsPtr[dataSetIndex].sdid != NULL)
				{
						// All channels belong to the same data set (and therefore sdid)
						
				for (channelIndex=0; 
							channelIndex<fileInfoPtr->numberChannels; 
									channelIndex++) 
					channelToHdfDataSetPtr[channelIndex] = (UInt16)dataSetIndex;
				
				}	// end "if (hdfDataSetsPtr[dataSetIndex].sdid != NULL))"
				
			else	// hdfDataSetsPtr[dataSetIndex].sdid == NULL)
				returnCode = 7;
			
			}	// end "else groupedNumber = 0"
			
//		}	// end "if (fileInfoPtr->callGetHDFLineFlag)"
		
	return (returnCode);
    
}	// end "SetUpHDF_FileInformation"

