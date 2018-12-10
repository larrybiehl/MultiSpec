//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SOpenFileDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/30/2018
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	This file contains routines for dialog boxes which are used to 
//								access and/or interpret image files stored on disk.
//
//	Functions in file:	pascal void			DrawCollapseClassOptionPopUp
//								Boolean 				ModalFileSpecification
//								void 					OpenImageFile
//								SInt16 				VerifyFileInfoDialogValues
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
/*	Template for debugging
		int numberChars = sprintf ((char*)gTextString3,
													" SOpenFileDialog: (): %s",
													gEndOfLine);
		ListString ((char*)gTextString3, numberChars, gOutputTextH);	
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#if defined multispec_lin
	#include "LMultiSpec.h"
	#include "LFileFormatDialog.h"
	
	#define	IDOK									1
	#define	IDC_FillDataValueExists			29
	#define	IDC_FillDataValue					30
#endif	

#if defined multispec_mac || defined multispec_mac_swift
	#define	IDOK									1
	#define	IDC_NumberLines					5
	#define	IDC_NumberColumns					7
	#define	IDC_STATIC8							8
	#define	IDC_STATIC9							9
	#define	IDC_NumberChannels				10
	#define	IDC_StartLineNumber				12
	#define	IDC_StartColumnNumber			14
	#define	IDC_HeaderPrompt					15
	#define	IDC_HeaderBytes					16
	#define	IDC_TrailerPrompt					17
	#define	IDC_TrailerBytes					18
	#define	IDC_PreLinePrompt					19
	#define	IDC_PreLineBytes					20
	#define	IDC_PostLinePrompt				21
	#define	IDC_PostLineBytes					22
	#define	IDC_PreChannelPrompt				23
	#define	IDC_PreChannelBytes				24
	#define	IDC_PostChannelPrompt			25
	#define	IDC_PostChannelBytes				26
	#define	IDC_STATIC27						27
	#define	IDC_BandInterleavePopUp			28
	#define	IDC_FillDataValueExists			29
	#define	IDC_FillDataValue					30
	#define	IDC_SwapBytes						31
	#define	IDC_DataValueTypePopUp			33
	#define	IDC_DetermineNumClasses 		34
	#define	IDC_ComputeNumClasses			35
	#define	IDC_SignedData						38
	#define	IDC_BlockHeightPrompt			39
	#define	IDC_BlockHeight					40
	#define	IDC_BlockWidthPrompt				41
	#define	IDC_BlockWidth						42
	#define	IDC_HDFDataSetPrompt				36
	#define	IDC_HDFDataSet						43 
	#define	IDC_HDFDataSetHelp				46 
	#define	IDC_CollapseClassPrompt			44
	#define	IDC_CollapseClass					45
	#define	IDC_LinesBottomToTop				47

	#define	 IDS_Alert135						135
	#define	 IDS_Alert148						148

	extern void DoWorkFlow (void);
#endif	// defined multispec_mac || defined multispec_mac_swift

#if defined multispec_win
	#include "CFileStream.h"
	#include "WImageView.h" 
	#include "WMultiSpec.h" 
	#include "WFileFormatDialog.h"
#endif	// defined multispec_win 

//#include		"SExtGlob.h"

#define	kFirstFileStreamNotLoaded		0
#define	kFirstFileStreamLoaded			1

extern void FileSpecificationDialogInitialize (
				DialogPtr							dialogPtr,
				Handle								fileInfoHandle,
				Handle								windowInfoHandle,
				FileInfoPtr*						fileInfoPtrPtr,
				WindowInfoPtr*						windowInfoPtrPtr,
				UInt32*								numberLinesPtr,
				UInt32*								numberColumnsPtr,
				UInt32*								numberChannelsPtr,
				SInt32*								startLinePtr,
				SInt32*								startColumnPtr,
				UInt32*								numberHeaderBytesPtr,
				UInt32*								numberTrailerBytesPtr,
				UInt32*								numberPreLineBytesPtr,
				UInt32*								numberPostLineBytesPtr,
				UInt32*								numberPreChannelBytesPtr,
				UInt32*								numberPostChannelBytesPtr,
				UInt32*								blockHeightPtr,
				UInt32*								blockWidthPtr,
				SInt16*								bandInterleaveSelectionPtr,
				SInt16*								dataValueTypeSelectionPtr,
				SInt16*								eightBitsPerDataSelectionPtr,
				Boolean*								swapBytesFlagPtr,
				Boolean*								signedDataFlagPtr,
				Boolean*								linesBottomToTopFlagPtr,
				Boolean*								fillDataValueExistsFlagPtr,
				double*								fillDataValuePtr,
				SInt16*								hdfDataSetPtr,
				SInt16*								collapseClassesSelectionPtr,
				Boolean*								callGetHDFLineFlagPtr);

extern SInt16 FileSpecificationDialogSetHDFValues (
				DialogPtr							dialogPtr,
				FileInfoPtr							fileInfoPtr,
				SInt16								hdfDataSetSelection,
				Handle								okHandle,
				Boolean								shiftKeyDownFlag,
				Handle*								mapInformationHandlePtr,
				Handle*								hfaHandlePtr,
				Handle*								channelToHdfDataSetHandlePtr,
				SInt16*								bandInterleaveSelectionPtr,
				SInt16*								dataValueTypeSelectionPtr,
				UInt16*								dataCompressionCodePtr,
				SInt16*								gdalDataTypeCodePtr,
				Boolean*								callGetHDFLineFlagPtr);

extern SInt16 FileSpecificationDialogGetNumberBytes (
				SInt16								dataTypeSelection);

extern Boolean FileSpecificationDialogOK (
				DialogPtr							dialogPtr,
				FileInfoPtr							fileInfoPtr,
				Handle								fileInfoHandle,
				Handle*								newMapInformationHandlePtr,
				Handle*								newHfaHandlePtr,
				Handle*								newChannelToHdfDataSetHandlePtr,
				WindowInfoPtr						windowInfoPtr,
				Handle								windowInfoHandle,
				UInt32								numberLines,
				UInt32								numberColumns,
				UInt32								numberChannels,
				SInt32								startLine,
				SInt32								startColumn,
				UInt32								numberHeaderBytes,
				UInt32								numberTrailerBytes,
				UInt32								numberPreLineBytes,
				UInt32								numberPostLineBytes,
				UInt32								numberPreChannelBytes,
				UInt32								numberPostChannelBytes,
				UInt32								blockHeight,
				UInt32								blockWidth,
				Boolean								forceGroupTableUpdateFlag,
				SInt16								bandInterleaveSelection,
				SInt16								dataValueTypeSelection,
				Boolean								swapBytesFlag,
				Boolean								signedDataFlag,
				Boolean								linesBottomToTopFlag,
				Boolean								computeNumberClassesFlag,
				Boolean								fillDataValueExistsFlag,
				double								fillDataValue,
				SInt16								hdfDataSetSelection,
				UInt32								hdfDataSetIndex,
				SInt16								collapseClassesSelection,
				UInt16								dataCompressionCode,
				SInt16								gdalDataTypeCode,
				Boolean								callGetHDFLineFlag);

extern void FileSpecificationDialogSetInterleaveItems (
				DialogPtr							dialogPtr,
				SInt16								bandInterleaveSelection,
				Boolean								blockedFlag);

extern SInt16 VerifyFileInfoDialogValues (
				DialogPtr							dialogPtr,
				SInt32								numberLines,
				SInt32								numberColumns,
				SInt32								numberChannels_Classes,
				SInt32								startLine,
				SInt32								startColumn,
				SInt32								numberHeaderBytes,
				SInt32								numberTrailerBytes,
				SInt32								numberPreLineBytes,
				SInt32								numberPostLineBytes,
				SInt32								numberPreChannelBytes,
				SInt32								numberPostChannelBytes,
				SInt16								dataTypeSelection,
				Boolean								thematicType,
				SInt32*								minLimitPtr,
				SInt32*								maxLimitPtr);



// Prototypes for routines in this file that are only called by		
// other routines in this file.	

Boolean AddSelectedFilesToWindow (
				Handle								windowInfoHandle,
				Handle								fileInfoHandle,
				UInt32								itemCount,
				FSRef*								fileAsFSRefPtr,
				SInt16								instrumentCode,
				Boolean								firstFileStreamLoadedFlag);

Boolean CheckForLandsatAnalysisReadyFileList (
				UInt32								itemCount,
				FSRef*								fileAsFSRefPtr,
				FSRef*								savedFsRefPtr,
				SInt16*								fileVectorPtr,
				char*									bandStringPtr,
				SInt16*								instrumentCodePtr);

Boolean CheckForLandsatETMFileList (
				UInt32								itemCount,
				FSRef*								fileAsFSRefPtr,
				FSRef*								savedFsRefPtr,
				SInt16*								fileVectorPtr,
				char*									bandStringPtr,
				SInt16*								instrumentCodePtr);

Boolean CheckForInstrumentFileList (
				UInt32								itemCount,
				FSRef*								fileAsFSRefPtr,
				SInt16*								instrumentCodePtr);

Boolean CheckForLandsatMSSFileList (
				UInt32								itemCount,
				FSRef*								fileAsFSRefPtr,
				FSRef*								savedFsRefPtr,
				SInt16*								fileVectorPtr,
				char*									bandStringPtr,
				SInt16*								instrumentCodePtr);

Boolean CheckForLandsatSurfaceReflectanceFileList (
				UInt32								itemCount,
				FSRef*								fileAsFSRefPtr,
				FSRef*								savedFsRefPtr,
				SInt16*								fileVectorPtr,
				char*									bandStringPtr,
				SInt16*								instrumentCodePtr);

Boolean CheckForLandsatTMFileList (
				UInt32								itemCount,
				FSRef*								fileAsFSRefPtr,
				FSRef*								savedFileAsFSRefPtr,
				SInt16*								fileVectorPtr,
				char*									bandStringPtr,
				SInt16*								instrumentCodePtr);

Boolean CheckForLandsat8FileList (
				UInt32								itemCount,
				FSRef*								fileAsFSRefPtr,
				FSRef*								savedFileAsFSRefPtr,
				SInt16*								fileVectorPtr,
				char*									bandStringPtr,
				SInt16*								instrumentCodePtr);
				
Boolean CheckForSentinel2FileList (
				UInt32								itemCount,
				FSRef*								fileAsFSRefPtr,
				FSRef*								savedFsRefPtr,
				SInt16*								fileVectorPtr,
				char*									bandStringPtr,
				SInt16*								instrumentCodePtr);

PascalVoid DrawCollapseClassOptionPopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber);

PascalVoid DrawHDFDataSetPopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber);

SInt16 LinkSelectedFilesToNewWindow (
				Handle								fileInfoHandle,
				UInt32								itemCount,
				FSRef*								fileAsFSRefPtr);

void ListFileIgnoredMessage (
				Handle								fileInfoHandle,
				SInt16								messageCode);

Boolean OpenSeparateImageWindows (
				Handle								fileInfoHandle,
				UInt32								itemCount,
				FSRef*								fileAsFSRefPtr,
				Boolean								fileInfoLoadedFlag);

SInt16 OpenSpecifiedFile (
				Handle								fileInfoHandle,
				FSRef									fileAsFSRef);

void ReorderLandsat8FileList (
				SInt16*								fileVectorPtr,
				SInt16*								instrumentCodePtr);

void SetHDFDataSetFileInformation (
				DialogPtr							dialogPtr,
				FileInfoPtr							fileInfoPtr,
				SInt16								hdfDataSetSelection,
				UInt32								dataSetIndex);

void SetUpHDFDataSetPopupMenu (
				DialogPtr							dialogPtr,
				FileInfoPtr							fileInfoPtr);

Boolean VerifyImageFileSizeAgainstBaseImage (
				Handle								windowInfoHandle,
				Handle								fileInfoHandle);



SInt16 gHdfDataSetSelection = 1;

SInt16 gCollapseClassSelection = 1;



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean AddSelectedFilesToWindow
//
//	Software purpose:	The purpose of this routine is to link the image files in
//							the input image set.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 11/12/1999
//	Revised By:			Larry L. Biehl			Date: 09/05/2017	

Boolean AddSelectedFilesToWindow (
				Handle								windowInfoHandle,
				Handle								fileInfoHandle,
				UInt32								itemCount,
				FSRef*								fileAsFSRefPtr,
				SInt16								instrumentCode,
				Boolean								firstFileStreamLoadedFlag)
{
	WindowInfoPtr						windowInfoPtr;

	UInt32								fileNumber,
											formatVersionCode,
											numberImageFiles;

	SInt16								errCode,
											fileFormat,
											linkOffsetIndex;

	SInt16								index,
											length;

	Boolean								continueFlag,
											doneFlag,
											fileInfoLoadedFlag;


	errCode = noErr;
	continueFlag = FALSE;

	if (windowInfoHandle != NULL && fileInfoHandle != NULL)
		{
		numberImageFiles = GetNumberImageFiles (windowInfoHandle);
		fileNumber = 0;
		doneFlag = FALSE;

		linkOffsetIndex = 0;
		if (!firstFileStreamLoadedFlag)
					// This implies that the first link has been loaded in the window
					// structure. Allow for it for later use with hdf like formatted files.
			linkOffsetIndex = 1;

		if (itemCount <= 0)
			doneFlag = continueFlag = TRUE;

		gNextTime = TickCount ();

		while (!doneFlag)
			{
			continueFlag = FALSE;

			if (fileNumber > 0 || !firstFileStreamLoadedFlag)
				{
						// The file information structure for the first one in the list
						// was already set up in OpenImageFile.

				fileInfoLoadedFlag = FALSE;

						// Initialize the variables and handles in the structure.		

				InitializeFileInfoStructure (fileInfoHandle, kNotPointer);

						// Set the instrument code in case it was set.

				SetFileInstrumentCode (fileInfoHandle, instrumentCode);

				errCode = OpenSpecifiedFile (fileInfoHandle, fileAsFSRefPtr[fileNumber]);

            }	// end "if (fileNumber > 0 || !firstFileStreamLoadedFlag)"

					// Determine the file format and load the header information.		
					// Note that the file information structure for the first selected
					// file has already been completed.													

			if (errCode == noErr)
				fileInfoLoadedFlag = LoadImageInformation (windowInfoHandle,
																			fileInfoHandle,
																			FALSE,
																			&formatVersionCode,
																			linkOffsetIndex);

			if (fileInfoLoadedFlag)
				{
				fileFormat = GetFileFormat (fileInfoHandle);

                // If this not a shape file, verify that the number of lines and
                // columns in the image to be added is the same as that for the
                // window being added to.

				if (fileFormat != kArcViewShapeType)
					{
					fileInfoLoadedFlag = VerifyImageFileSizeAgainstBaseImage (
																	windowInfoHandle, fileInfoHandle);

					if (!fileInfoLoadedFlag)
							// List message in output text window.			
						ListFileIgnoredMessage (fileInfoHandle, 2);

					}	// end "if (fileFormat != kArcViewShapeType)"

				}	// end "if (fileInfoLoadedFlag)"

			if (fileInfoLoadedFlag && fileFormat != kArcViewShapeType)
				{
						// Add to the active image window as long as the band interleave
						// is not BIS

				if (GetBandInterleave (fileInfoHandle) != kBIS)
					{
					continueFlag = AddToImageWindowFile (windowInfoHandle, fileInfoHandle);

					if (formatVersionCode == 1)
						{
								// This is a special case single file selection which implies
                        // multiple files to be linked.

						if (LinkFiles (fileFormat, windowInfoHandle) == noErr)
							continueFlag = TRUE;

						}	// end "if (formatVersionCode == 1)"

					linkOffsetIndex++;

					}	// end "if (GetBandInterleave (fileInfoHandle) != kBIS)"

				else	// ...bandInterleave == kBIS 
							// Can not logically link BIS interleaved files. 	
					DisplayAlert (kErrorAlertID, kCautionAlert, kAlertStrID, 15, 0, NULL);

            }	// end "if (fileInfoLoadedFlag && fileFormat != kArcViewShapeType)"

			else	// !fileInfoLoadedFlag || fileFormat == kArcViewShapeType
            {
						// Indicate that processing of the file list can continue. This
						// particular file in the list is being ignored.

				continueFlag = TRUE;

            }	// end "else !fileInfoLoadedFlag || fileFormat == kArcViewShapeType"

			fileNumber++;
			if (fileNumber >= itemCount || !continueFlag)
				doneFlag = TRUE;

			if (gAlertReturnCode == 3)
						// There is a problem reading this set of files. This could be because of a
						// file limit in the hdf4 library. User has requested to stop linking the files.
				doneFlag = TRUE;

						// Cause the cursor to spin.

			if (TickCount () >= gNextTime)
				CheckSomeEvents (0);

			}	// end "while (!doneFlag)"

				// Update the title for the window to indicate that the		
				// window represents more than 1 file layer.		

		windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);

		GetImageWindowTitle (windowInfoHandle, gTextString);
		length = gTextString[0];
		gTextString[gTextString[0]+1] = 0;
		index = 1;
		if (windowInfoPtr->numberImageFiles > 2)
			{
					// Determine the length of the current layer					
					// identification.													

			NumToString ((UInt32)(windowInfoPtr->numberImageFiles - 1), gTextString2);
			index += 2 + gTextString2[0];
			length -= index - 1;

			}	// end "if (windowInfoPtr->numberImageFiles > 2)" 

		length = sprintf ((char*)&gTextString2[1],
								 "L%hd-%s",
								 windowInfoPtr->numberImageFiles,
								 &gTextString[index]);

		//NumToString ((SInt32)(windowInfoPtr->numberImageFiles-1), gTextString);
		//length += 2 + gTextString[0];
		gTextString2[0] = (UInt8)length;

		SetImageWindowTitle (windowInfoHandle, gTextString2);

		#if defined multispec_mac
					// Update the image window name in the Window menu.				

			MSetMenuItemText (gMultiSpecMenus[kWindowM],
									 windowInfoPtr->windowMenuItem,
									 gTextString2,
									 kASCIICharString);
		#endif	// defined multispec_mac

		}	// end "if (windowInfoHandle != NULL && fileInfoHandle != NULL)"

	gAlertReturnCode = 0;

	return (continueFlag);

}	// end "AddSelectedFilesToWindow"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CheckForLandsatAnalysisReadyFileList
//
//	Software purpose:	The purpose of this routine is to check for a Landsat Analysis
//							Ready data and set the proper instrument value.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	True: first item in the FSRef file has been change.
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 11/07/2017
//	Revised By:			Larry L. Biehl			Date: 11/07/2017

Boolean CheckForLandsatAnalysisReadyFileList (
				UInt32								itemCount,
				FSRef*								fileAsFSRefPtr,
				FSRef*								savedFsRefPtr,
				SInt16*								fileVectorPtr,
				char*									bandStringPtr,
				SInt16*								instrumentCodePtr)
{
	UInt8									fileName[256],
											savedFileName[256];
											
	UInt32								extraFileCount,
											fileIndex;

	SInt16								bandNameStart,
											bandNumber,
											errCode,
											instrumentCode,
											tReturnCode;

	Boolean								continueFlag,
											firstFileFoundFlag,
											srbSetFlag;


			//	Check if this is a Landsat surface reflectance data set
			// This will be bands labeled as '...sr_band1...

	instrumentCode = 0;
	extraFileCount = 0;
	firstFileFoundFlag = FALSE;
	srbSetFlag = FALSE;
	for (fileIndex=0; fileIndex<itemCount; fileIndex++)
		{
		continueFlag = FALSE;
		errCode = GetFileNameFromFSRef (&fileAsFSRefPtr[fileIndex], (StringPtr)fileName);

		if (errCode == noErr)
			{
			RemoveSuffix ((FileStringPtr)fileName);

					// Save the first file name to compare to the rest in the list.

			if (!firstFileFoundFlag)
				{
				CopyPToP (savedFileName, fileName);
				savedFileName[0] -= 5;
				if (CompareStringsNoCase ((UCharPtr)"LT04", &savedFileName[1], 4) == 0 ||
						CompareStringsNoCase ((UCharPtr)"LT05", &savedFileName[1], 4) == 0)
					instrumentCode = kLandsatTM;
					
				else if (CompareStringsNoCase ((UCharPtr)"LT07", &savedFileName[1], 4) == 0)
					instrumentCode = kLandsatTM7;
					
				else if (CompareStringsNoCase ((UCharPtr)"LC08", &savedFileName[1], 4) == 0)
					instrumentCode = kLandsatLC8_OLI;
					
				if (instrumentCode == 0)
					break;

				bandNameStart = savedFileName[0] + 1;
				firstFileFoundFlag = TRUE;

				}	// end "if (!firstFileFoundFlag)"

					// For Landsat analysis ready data, the band name is like _TABn for
					// top of the atmosphere reflectance and _SRBn for surface reflectance
					// where n can be 1 through 7 or 9.
					// Fill vector with the order of the n's

			tReturnCode = sscanf ((char*)&fileName[bandNameStart],
											"_TAB%hd",
											&bandNumber);
											
			if (tReturnCode == 0)
				{
						// Not TAB set; check for SRB set
				tReturnCode = sscanf ((char*)&fileName[bandNameStart],
												"_SRB%hd",
												&bandNumber);
												
				if (tReturnCode == 1)								
					srbSetFlag = TRUE;
											
				}	// end "if (tReturnCode == 0)"

			if (tReturnCode == 0)
				{
						// Check if this might be just an extra file that was selected. If 
						// so just ignore the file.

				if (itemCount > 6 + extraFileCount)
					continueFlag = TRUE;

						// If this is the file used to set the initial file name, then the
						// first file name will need to be reset.

				if (fileIndex == extraFileCount)
					firstFileFoundFlag = FALSE;

				extraFileCount++;

            }	// end "if (tReturnCode == 0)"

			if (tReturnCode == 1 &&
					!CompareStringsNoCase (
									&savedFileName[1], &fileName[1], savedFileName[0]) &&
						bandNumber >= 1 &&
							bandNumber <= 9)
				{
						// Verify that there are no duplicate bands

				if (fileVectorPtr[bandNumber - 1] == -1)
					{
					fileVectorPtr[bandNumber - 1] = (SInt16)fileIndex;
					continueFlag = TRUE;

					}	// end "if (fileVectorPtr[bandNumber-1] == -1)"

            }	// end "if (tReturnCode == 1 && ..."

        }	// end "if (errCode == noErr)"

		if (!continueFlag)
			break;

		}	// end "for (fileNumber=1; fileNumber<=itemCount; fileNumber++)"

	if (continueFlag)
		{
				// This is a Landsat surface reflectance set of files.

		*instrumentCodePtr = instrumentCode;

				// The bands for surface reflectance are in order of wavelength. If
				// Landsat 4 or 5 data, then move band 7 file to the slot 6 since
				// slot 6 will be empty.
		
		if (instrumentCode == kLandsatTM && fileVectorPtr[5] == -1)
			{
			fileVectorPtr[5] = fileVectorPtr[6];
			fileVectorPtr[6] = -1;
			
			}	// end "if (instrumentCode == kLandsatTM && fileVectorPtr[5] == 0)"
		
		else if (instrumentCode == kLandsatLC8_OLI)
			ReorderLandsat8FileList (fileVectorPtr,instrumentCodePtr);

		}	// end "if (continueFlag)"

	return (continueFlag);

}	// end "CheckForLandsatAnalysisReadyFileList"




//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CheckForLandsatETMFileList
//
//	Software purpose:	The purpose of this routine is to check for a Landsat 7 ETM+ 
//							file list and adjust the list if needed so that the bands are
//							in wavelength order.
//							Check if Landsat 7
//							This can be just the reflective bands (1,2,3,4,5,7)or
//							Or 7 or 8 channels including 1 or 2 band 6's (thermal).
//							Band 8 if included will be ignored.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	True: first item in the FSRef file has been change.
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/15/2013
//	Revised By:			Larry L. Biehl			Date: 03/16/2017	

Boolean CheckForLandsatETMFileList (
				UInt32								itemCount,
				FSRef*								fileAsFSRefPtr,
				FSRef*								savedFsRefPtr,
				SInt16*								fileVectorPtr,
				char*									bandStringPtr,
				SInt16*								instrumentCodePtr)
{
	UInt8									fileName[256],
											savedFileName[256];

	UInt32								fileIndex;

	SInt16								bandNameStart,
											bandNumber,
											characterCount,
											errCode,
											fileVectorIndex,
											savedIndex,
											tReturnCode;

	Boolean								continueFlag;


	for (fileIndex = 0; fileIndex < itemCount; fileIndex++)
		{
		continueFlag = FALSE;
		errCode = GetFileNameFromFSRef (&fileAsFSRefPtr[fileIndex], (StringPtr)fileName);

		if (errCode == noErr)
			{
			RemoveSuffix ((FileStringPtr)fileName);

					// Save the first file name to compare to the rest in the list.

			if (fileIndex == 0)
				{
				CopyPToP (savedFileName, fileName);
				savedFileName[0] -= 3;
				if (CompareStringsNoCase ((UCharPtr)"LE7", &savedFileName[1], 3))
					break;

						// Now need to verify that is first file is not one of the thermal
						// channels named "_B6_VCID_1" or _B6_VCID_2" at the end.

				if (StrStrNoCase ((char*)&fileName[1], "_B6_VCID_") != NULL)
					{
							// Remove the _VCID_n suffix from the name but be sure the
							// number of characters is not less than 0

					characterCount = MIN (7, savedFileName[0]);
					savedFileName[0] -= characterCount;

					}	// end "if (StrStrNoCase (&fileName[1], "_B6_VCID_") != NULL)"

				bandNameStart = savedFileName[0] + 1;

            }	// end "if (fileIndex == 0)"

					// For Landsat 7, the band name is like _Bn where n can be 1 through 8.
					// There can be 2 band 6's
					// Fill vector with the order of the n's

			tReturnCode = sscanf ((char*)&fileName[bandNameStart],
											"_B%hd",
											&bandNumber);

			if (tReturnCode == 1 &&
					!CompareStringsNoCase (
										&savedFileName[1], &fileName[1], savedFileName[0]) &&
						bandNumber >= 1 &&
							bandNumber <= 8)
				{
                // If thermal channel, determine whether VICD_1 or 2.
                // 6_VICD_2 will be stored at index 7

				fileVectorIndex = bandNumber - 1;
				if (bandNumber == 6 && !CompareStringsNoCase (
											(UCharPtr)"_B6_VCID_2", &fileName[bandNameStart], 10))
					fileVectorIndex = 7;

				if (bandNumber == 8)
					fileVectorIndex = 8;

						// Verify that there are no duplicate bands

				if (fileVectorPtr[fileVectorIndex] == -1)
					{
					fileVectorPtr[fileVectorIndex] = (SInt16)fileIndex;
					continueFlag = TRUE;

					}	// end "if (fileVectorPtr[fileVectorIndex] == -1)"

				if ((bandNumber == 6 || bandNumber == 8)&& itemCount == 6)
                    // This implies that the thermal channel is part of a set of data
                    // with only 6 channels. A reflective channel must have been left
						  // out.
					continueFlag = FALSE;

            }	// end "if (tReturnCode == 1 && ..."

			}	// end "if (errCode == noErr)"

		if (!continueFlag)
			break;

		}	// end "for (fileIndex=0; fileIndex<itemCount; fileIndex++)"

	if (continueFlag)
		{
				// This is a Landsat 7 list of files.

		*instrumentCodePtr = kLandsatTM7;

				// Now put the bands in order.
				// First reverse the order for the indices for bands 6 and 7.

		savedIndex = fileVectorPtr[6];
		fileVectorPtr[6] = fileVectorPtr[5];
		fileVectorPtr[5] = savedIndex;
		/*
		for (fileIndex=0; fileIndex<itemCount; fileIndex++)
			{
			if (fileVectorPtr[fileIndex] != fileIndex)
				fileAsFSRefPtr[fileIndex] = savedFsRefPtr[fileVectorPtr[fileIndex]];
			
			}	// end "for (fileIndex=1; fileIndex<=itemCount; fileIndex++)"
		*/
		}	// end "if (continueFlag)"

	return (continueFlag);

}	// end "CheckForLandsatETMFileList"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CheckForInstrumentFileList
//
//	Software purpose:	The purpose of this routine is to check for a Landsat 5 or 7 
//							file list and adjust the list if needed so that the bands are
//							in wavelength order.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	True: first item in the FSRef file has been change.
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/15/2013
//	Revised By:			Larry L. Biehl			Date: 08/22/2017

Boolean CheckForInstrumentFileList (
				UInt32								itemCount,
				FSRef*								fileAsFSRefPtr,
				SInt16*								instrumentCodePtr)
{
	char									bandString[5];

	FSRef									savedFsRef[13];

	UInt32								fileIndex,
											fileNumber;

	SInt16								fileVector[13],
											fileVectorIndex;

	Boolean								continueFlag,
											doneFlag,
											returnFlag;


	*instrumentCodePtr = 0;
	returnFlag = FALSE;

	if (itemCount >= 2 && itemCount <= 13)
		{
		continueFlag = FALSE;
		doneFlag = FALSE;
		fileNumber = 0;
		bandString[0] = '_';
		bandString[1] = 'B';
		bandString[3] = 0;
		bandString[4] = 0;

		for (fileIndex = 0; fileIndex < 13; fileIndex++)
			fileVector[fileIndex] = -1;

		for (fileIndex = 0; fileIndex < itemCount; fileIndex++)
			savedFsRef[fileIndex] = fileAsFSRefPtr[fileIndex];

		if (itemCount >= 2 && itemCount <= 4)
			{
			continueFlag = CheckForLandsatMSSFileList (itemCount,
																	  fileAsFSRefPtr,
																	  savedFsRef,
																	  fileVector,
																	  bandString,
																	  instrumentCodePtr);

			}	// end "if (itemCount == 4)"

		if (*instrumentCodePtr == 0 && itemCount >= 2 && itemCount <= 8)
			{
			continueFlag = CheckForLandsatTMFileList (itemCount,
																	  fileAsFSRefPtr,
																	  savedFsRef,
																	  fileVector,
																	  bandString,
																	  instrumentCodePtr);

			}	// end "if (*instrumentCodePtr == 0 && itemCount >= 2 && itemCount <= 8)"

		if (*instrumentCodePtr == 0 && itemCount >= 2 && itemCount <= 9)
			{
			continueFlag = CheckForLandsatETMFileList (itemCount,
																	  fileAsFSRefPtr,
																	  savedFsRef,
																	  fileVector,
																	  bandString,
																	  instrumentCodePtr);

			}	// end "if (*instrumentCodePtr == 0 && itemCount >= 6 && itemCount <= 9)"

		if (*instrumentCodePtr == 0 && itemCount >= 2 && itemCount <= 12)
			{
			continueFlag = CheckForLandsat8FileList (itemCount,
																  fileAsFSRefPtr,
																  savedFsRef,
																  fileVector,
																  bandString,
																  instrumentCodePtr);

			}	// end "if (*instrumentCodePtr == 0 && itemCount >= 2 && itemCount <= 12)"


		if (*instrumentCodePtr == 0 && itemCount >= 6 && itemCount <= 7)
			{
			continueFlag = CheckForLandsatSurfaceReflectanceFileList (
																	itemCount,
																  fileAsFSRefPtr,
																  savedFsRef,
																  fileVector,
																  bandString,
																  instrumentCodePtr);

			}	// end "if (*instrumentCodePtr == 0 && itemCount >= 6 && itemCount <= 7)"


		if (*instrumentCodePtr == 0 && itemCount >= 6 && itemCount <= 8)
			{
			continueFlag = CheckForLandsatAnalysisReadyFileList (
																	itemCount,
																  fileAsFSRefPtr,
																  savedFsRef,
																  fileVector,
																  bandString,
																  instrumentCodePtr);

			}	// end "if (*instrumentCodePtr == 0 && itemCount >= 6 && itemCount <= 7)"

		if (*instrumentCodePtr == 0 && itemCount >= 3 && itemCount <= 13)
			{
			continueFlag = CheckForSentinel2FileList (itemCount,
																	  fileAsFSRefPtr,
																	  savedFsRef,
																	  fileVector,
																	  bandString,
																	  instrumentCodePtr);

			}	// end "if (*instrumentCodePtr == 0 && itemCount >= 3 && itemCount <= 13)"

		if (continueFlag)
			{
					// Now make sure the fileVector list is continuous with a list
					// of indices to channels. Some channels may have been left out.

			fileIndex = 0;
			for (fileVectorIndex = 0; fileVectorIndex < 13; fileVectorIndex++)
				{
				if (fileVector[fileIndex] == -1 && fileVector[fileVectorIndex] != -1)
					{
					fileVector[fileIndex] = fileVector[fileVectorIndex];
					fileVector[fileVectorIndex] = -1;
					
					}	// end "if (fileVector[fileIndex] == -1 && ..."

				if (fileVector[fileIndex] != -1)
					fileIndex++;

				if (fileIndex >= itemCount)
					break;

            }	// end "for (fileVectorIndex=0; fileVectorIndex<11; fileVectorIndex++)"

			for (fileIndex = 0; fileIndex < itemCount; fileIndex++)
				{
				if (fileVector[fileIndex] != fileIndex)
					fileAsFSRefPtr[fileIndex] = savedFsRef[fileVector[fileIndex]];

            }	// end "for (fileIndex=1; fileIndex<=itemCount; fileIndex++)"

			if (fileVector[0] != 0)
				returnFlag = TRUE;

			}	// end "if (continueFlag)"

		}	// end "if (itemCount>=2 && itemCount <= 13)"

    return (returnFlag);

}	// end "CheckForInstrumentFileList"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CheckForLandsatMSSFileList
//
//	Software purpose:	The purpose of this routine is to check for a Landsat MSS 
//							file list and adjust the list if needed so that the bands are
//							in wavelength order.
//							More than one naming convention has been found ... M1 at the start
//							with the bands named as 40, 50, 60 & 70
//							and LM at the start with the bands named as 4, 5, 6 & 7.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	True: first item in the FSRef file has been change.
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/15/2013
//	Revised By:			Larry L. Biehl			Date: 03/16/2017	

Boolean CheckForLandsatMSSFileList (
				UInt32								itemCount,
				FSRef*								fileAsFSRefPtr,
				FSRef*								savedFsRefPtr,
				SInt16*								fileVectorPtr,
				char*									bandStringPtr,
				SInt16*								instrumentCodePtr)
{
	UInt8									fileName[256],
											savedFileName[256];

	UInt32								fileIndex;

	SInt16								bandNameStart,
											bandNumber,
											errCode,
											numberBandDigits,
											tReturnCode;

	Boolean								continueFlag;


	for (fileIndex=0; fileIndex<itemCount; fileIndex++)
		{
		continueFlag = FALSE;
		errCode = GetFileNameFromFSRef (&fileAsFSRefPtr[fileIndex], (StringPtr)fileName);

		if (errCode == noErr)
			{
			RemoveSuffix ((FileStringPtr)fileName);

					// Save the first file name to compare to the rest in the list.

			if (fileIndex == 0)
				{
				CopyPToP (savedFileName, fileName);
				if (CompareStringsNoCase ((UCharPtr)"M1", &savedFileName[1], 2) == 0)
					numberBandDigits = 2;

				else if (CompareStringsNoCase ((UCharPtr)"LM", &savedFileName[1], 2) == 0)
					numberBandDigits = 1;

				else	// correct prefix was not found.
					break;

				savedFileName[0] -= (numberBandDigits + 2);

				bandNameStart = savedFileName[0] + 1;

            }	// end "if (fileIndex == 0)"

					// For Landsat MSS, the band name is like _Bn0 where n can be 4 through 7.
					// Fill vector with the order of the n's

			tReturnCode = sscanf ((char*)&fileName[bandNameStart],
											"_B%hd",
											&bandNumber);

			if (numberBandDigits == 2)
				bandNumber /= 10;

			if (tReturnCode == 1 &&
					!CompareStringsNoCase (
									&savedFileName[1], &fileName[1], savedFileName[0]) &&
						bandNumber >= 4 &&
							bandNumber <= 7)
				{
						// Verify that there are no duplicate bands

				bandNumber -= 4;
				if (fileVectorPtr[bandNumber] == -1)
					{
					fileVectorPtr[bandNumber] = (SInt16)fileIndex;
					continueFlag = TRUE;

					}	// end "if (fileVectorPtr[bandNumber] == -1)"

				}	// end "if (tReturnCode == 1 && ..."

			}	// end "if (errCode == noErr)"

		if (!continueFlag)
			break;

		}	// end "for (fileNumber=1; fileNumber<=itemCount; fileNumber++)"

	if (continueFlag)
			// This is a Landsat MSS list of files.
		*instrumentCodePtr = kLandsatMSS;

	return (continueFlag);

}	// end "CheckForLandsatMSSFileList"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CheckForLandsatSurfaceReflectanceFileList
//
//	Software purpose:	The purpose of this routine is to check for a Landsat Surface
//							Reflectance file list and set the proper instrument code. The
//							bands should be in the correct wavelength order.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	True: first item in the FSRef file has been change.
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 11/01/2017
//	Revised By:			Larry L. Biehl			Date: 11/02/2017

Boolean CheckForLandsatSurfaceReflectanceFileList (
				UInt32								itemCount,
				FSRef*								fileAsFSRefPtr,
				FSRef*								savedFsRefPtr,
				SInt16*								fileVectorPtr,
				char*									bandStringPtr,
				SInt16*								instrumentCodePtr)
{
	UInt8									fileName[256],
											savedFileName[256];
											
	UInt32								extraFileCount,
											fileIndex;

	SInt16								bandNameStart,
											bandNumber,
											errCode,
											instrumentCode,
											tReturnCode;

	Boolean								continueFlag,
											firstFileFoundFlag;


			//	Check if this is a Landsat surface reflectance data set
			// This will be bands labeled as '...sr_band1...

	instrumentCode = 0;
	extraFileCount = 0;
	firstFileFoundFlag = FALSE;
	for (fileIndex=0; fileIndex<itemCount; fileIndex++)
		{
		continueFlag = FALSE;
		errCode = GetFileNameFromFSRef (&fileAsFSRefPtr[fileIndex], (StringPtr)fileName);

		if (errCode == noErr)
			{
			RemoveSuffix ((FileStringPtr)fileName);

					// Save the first file name to compare to the rest in the list.

			if (!firstFileFoundFlag)
				{
				CopyPToP (savedFileName, fileName);
				savedFileName[0] -= 9;
				if (CompareStringsNoCase ((UCharPtr)"LT04", &savedFileName[1], 4) == 0 ||
						CompareStringsNoCase ((UCharPtr)"LT05", &savedFileName[1], 4) == 0)
					instrumentCode = kLandsatTM;
					
				else if (CompareStringsNoCase ((UCharPtr)"LT07", &savedFileName[1], 4) == 0)
					instrumentCode = kLandsatTM7;
					
				else if (CompareStringsNoCase ((UCharPtr)"LC08", &savedFileName[1], 4) == 0)
					instrumentCode = kLandsatLC8_OLI;		
				
				if (instrumentCode == 0)
					break;

				bandNameStart = savedFileName[0] + 1;
				firstFileFoundFlag = TRUE;

				}	// end "if (!firstFileFoundFlag)"

					// For Landsat surface reflectance, the band name is like _sr_bandn 
					// where n can be 1 through 7.
					// Fill vector with the order of the n's

			tReturnCode = sscanf ((char*)&fileName[bandNameStart],
											"_sr_band%hd",
											&bandNumber);

			if (tReturnCode == 0)
				{
						// Check if this might be just an extra file that was selected. If 
						// so just ignore the file.

				if (itemCount > 6 + extraFileCount)
					continueFlag = TRUE;

						// If this is the file used to set the initial file name, then the
						// first file name will need to be reset.

				if (fileIndex == extraFileCount)
					firstFileFoundFlag = FALSE;

				extraFileCount++;

            }	// end "if (tReturnCode == 0)"

			if (tReturnCode == 1 &&
					!CompareStringsNoCase (
									&savedFileName[1], &fileName[1], savedFileName[0]) &&
						bandNumber >= 1 &&
							bandNumber <= 7)
				{
						// Verify that there are no duplicate bands

				if (fileVectorPtr[bandNumber - 1] == -1)
					{
					fileVectorPtr[bandNumber - 1] = (SInt16)fileIndex;
					continueFlag = TRUE;

					}	// end "if (fileVectorPtr[bandNumber-1] == -1)"

            }	// end "if (tReturnCode == 1 && ..."

        }	// end "if (errCode == noErr)"
				
		if (!continueFlag)
			break;

		}	// end "for (fileNumber=1; fileNumber<=itemCount; fileNumber++)"

	if (continueFlag)
		{
				// This is a Landsat surface reflectance set of files.

		*instrumentCodePtr = instrumentCode;

				// The bands for surface reflectance are in order of wavelength. If
				// Landsat 4 or 5 data, then move band 7 file to the slot 6 since
				// slot 6 will be empty.

		if (fileVectorPtr[5] == -1)
			{
			fileVectorPtr[5] = fileVectorPtr[6];
			fileVectorPtr[6] = -1;
			
			}	// end "if (fileVectorPtr[5] == 0)"

		}	// end "if (continueFlag)"

	return (continueFlag);

}	// end "CheckForLandsatSurfaceReflectanceFileList"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CheckForLandsatTMFileList
//
//	Software purpose:	The purpose of this routine is to check for a Landsat 4 or 5 TM 
//							file list and adjust the list if needed so that the bands are
//							in wavelength order.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	True: first item in the FSRef file has been change.
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/15/2013
//	Revised By:			Larry L. Biehl			Date: 10/06/2017

Boolean CheckForLandsatTMFileList (
				UInt32								itemCount,
				FSRef*								fileAsFSRefPtr,
				FSRef*								savedFsRefPtr,
				SInt16*								fileVectorPtr,
				char*									bandStringPtr,
				SInt16*								instrumentCodePtr)
{
	UInt8									fileName[256],
											savedFileName[256];
											
	UInt32								extraFileCount,
											fileIndex;

	SInt16								bandNameStart,
											bandNumber,
											errCode,
											savedIndex,
											tReturnCode;

	Boolean								continueFlag,
											firstFileFoundFlag;


			//	Check if Landsat 5
			// This can be just the reflective bands (1,2,3,4,5,7)or
			// Or all 7 channels including band 6 (thermal)

	extraFileCount = 0;
	firstFileFoundFlag = FALSE;
	for (fileIndex=0; fileIndex<itemCount; fileIndex++)
		{
		continueFlag = FALSE;
		errCode = GetFileNameFromFSRef (&fileAsFSRefPtr[fileIndex], (StringPtr)fileName);

		if (errCode == noErr)
			{
			RemoveSuffix ((FileStringPtr)fileName);

					// Save the first file name to compare to the rest in the list.
					// The LT04 and LT05 are for the newer version of the name.

			if (!firstFileFoundFlag)
				{
				CopyPToP (savedFileName, fileName);
				savedFileName[0] -= 3;
				if (CompareStringsNoCase ((UCharPtr)"LT4", &savedFileName[1], 3) &&
						CompareStringsNoCase ((UCharPtr)"LT5", &savedFileName[1], 3) &&
							CompareStringsNoCase ((UCharPtr)"LT04", &savedFileName[1], 4) &&
								CompareStringsNoCase ((UCharPtr)"LT05", &savedFileName[1], 4))
					break;

				bandNameStart = savedFileName[0] + 1;
				firstFileFoundFlag = TRUE;

				}	// end "if (!firstFileFoundFlag)"

					// For Landsat 5, the band name is like _Bn where n can be 1 through 7.
					// Fill vector with the order of the n's

			tReturnCode = sscanf ((char*)&fileName[bandNameStart],
											"_B%hd",
											&bandNumber);

			if (tReturnCode == 0)
				{
						// Check if this might be just an extra file that was selected. If so
						// just ignore the file.

				if (itemCount > 6 + extraFileCount)
					continueFlag = TRUE;

						// If this is the file used to set the initial file name, then the
						// first file name will need to be reset.

				if (fileIndex == extraFileCount)
					firstFileFoundFlag = FALSE;

				extraFileCount++;

            }	// end "if (tReturnCode == 0)"

			if (tReturnCode == 1 &&
					!CompareStringsNoCase (
									&savedFileName[1], &fileName[1], savedFileName[0]) &&
						bandNumber >= 1 &&
							bandNumber <= 7)
				{
						// Verify that there are no duplicate bands

				if (fileVectorPtr[bandNumber - 1] == -1)
					{
					fileVectorPtr[bandNumber - 1] = (SInt16)fileIndex;
					continueFlag = TRUE;

					}	// end "if (fileVectorPtr[bandNumber-1] == -1)"

				if (bandNumber == 6 && itemCount == 6)
                    // This implies that the thermal channel is part of a set of data
                    // with only 6 channels. A reflective channel must have been left
						  // out.
					continueFlag = FALSE;

            }	// end "if (tReturnCode == 1 && ..."

        }	// end "if (errCode == noErr)"

		if (!continueFlag)
			break;

		}	// end "for (fileNumber=1; fileNumber<=itemCount; fileNumber++)"

	if (continueFlag)
		{
				// This is a Landsat 5 list of files.

		*instrumentCodePtr = kLandsatTM;

				// Now put the bands in order.
				// First reverse the order for the indices for bands 6 and 7.

		savedIndex = fileVectorPtr[6];
		fileVectorPtr[6] = fileVectorPtr[5];
		fileVectorPtr[5] = savedIndex;
		/*
		for (fileIndex=0; fileIndex<itemCount; fileIndex++)
			{
			if (fileVectorPtr[fileIndex] != fileIndex)
				fileAsFSRefPtr[fileIndex] = savedFsRefPtr[fileVectorPtr[fileIndex]];

			}	// end "for (fileIndex=1; fileIndex<=itemCount; fileIndex++)"
		*/
		}	// end "if (continueFlag)"

	return (continueFlag);

}	// end "CheckForLandsatTMFileList"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CheckForLandsat8FileList
//
//	Software purpose:	The purpose of this routine is to check for a Landsat 8 
//							file list and adjust the list if needed so that the bands are
//							in wavelength order.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	True: first item in the FSRef file has been change.
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/01/2013
//	Revised By:			Larry L. Biehl			Date: 08/23/2017	

Boolean CheckForLandsat8FileList (
				UInt32								itemCount,
				FSRef*								fileAsFSRefPtr,
				FSRef*								savedFsRefPtr,
				SInt16*								fileVectorPtr,
				char*									bandStringPtr,
				SInt16*								instrumentCodePtr)
{
	UCharPtr								startBandIdentiferPtr;
	UInt8									fileName[256],
											savedFileName[256];

   UInt32								fileIndex,
											fileVectorIndex;

   SInt16								bandNameStart,
											bandNumber,
											errCode,
											tReturnCode;

	Boolean								continueFlag;


			// Check if Landsat 8
			// This can be just the reflective bands (1,2,3,4,5,6,7,9)or
			// 9 or 10 channels including 1 or 2 of the thermal channels 10 and 11.
			// Band 8 if included will be ignored.

   for (fileIndex=0; fileIndex<itemCount; fileIndex++)
		{
		continueFlag = FALSE;
		errCode = GetFileNameFromFSRef (&fileAsFSRefPtr[fileIndex], fileName);

		if (errCode == noErr)
			{
         RemoveSuffix ((FileStringPtr)fileName);

					// Save the first file name to compare to the rest in the list.

         if (fileIndex == 0)
				{
            CopyPToP (savedFileName, fileName);

						// Remove _Bn or _Bnn from the saved file name.

				startBandIdentiferPtr = (UCharPtr)strstr ((char*)savedFileName, "_B");

            bandNameStart = (SInt16)(startBandIdentiferPtr - savedFileName - 1);
            savedFileName[0] = (UInt8)bandNameStart;

						// There are 4 possible prefixes for Landsat 8

            if (CompareStringsNoCase ((UCharPtr)"LC8", &savedFileName[1], 3) != 0 &&
						CompareStringsNoCase ((UCharPtr)"LO8", &savedFileName[1], 3) != 0 &&
						CompareStringsNoCase ((UCharPtr)"LT8", &savedFileName[1], 3) != 0 &&
						CompareStringsNoCase ((UCharPtr)"LC08", &savedFileName[1], 4) != 0)
					break;

            bandNameStart++;

            }	// end "if (fileIndex == 0)"

					// For Landsat 8, the band name is like _Bn where n can be 1 through 11.
					// Fill vector with the order of the n's
			
			tReturnCode = sscanf ((char*)&fileName[bandNameStart],
										  "_B%hd",
										  &bandNumber);

			if (tReturnCode == 0)
				{
						// Check if this is the BQA band.

				if (CompareStringsNoCase (
											&fileName[bandNameStart], (UCharPtr)"_BQA", 4) == 0)
					continueFlag = TRUE;

            }	// end "if (tReturnCode == 0)"

			if (tReturnCode == 1 &&
					!CompareStringsNoCase (
										&savedFileName[1], &fileName[1], savedFileName[0]) &&
						bandNumber >= 1 &&
							bandNumber <= 11)
				{
				fileVectorIndex = bandNumber - 1;

						// Verify that there are no duplicate bands

				if (fileVectorPtr[fileVectorIndex] == -1)
					{
					fileVectorPtr[fileVectorIndex] = (SInt16)fileIndex;
					continueFlag = TRUE;

					}	// end "if (fileVector[fileVectorIndex] == -1)"

            }	// end "if (tReturnCode == 1 && ..."

			}	// end "if (errCode == noErr)"

		if (!continueFlag)
			break;

		}	// end "for (fileIndex=0; fileIndex<itemCount; fileIndex++)"

	if (continueFlag)
		{
				// This is a Landsat 8 list of files.
				// Now put the bands in order.
				// Put band 9 in wavelenght order with the rest and leave band 8 out.
				
		ReorderLandsat8FileList (fileVectorPtr,instrumentCodePtr);
		/*
		savedIndex8 = fileVectorPtr[7];
		savedIndex = fileVectorPtr[8];
		fileVectorPtr[7] = fileVectorPtr[6];
		fileVectorPtr[6] = fileVectorPtr[5];
		fileVectorPtr[5] = savedIndex;
		fileVectorPtr[8] = fileVectorPtr[9];
		fileVectorPtr[9] = fileVectorPtr[10];
		fileVectorPtr[10] = savedIndex8;

		*instrumentCodePtr = kLandsatLC8_OLI_TIRS;

		if (fileVectorPtr[8] == -1 && fileVectorPtr[9] == -1)
			*instrumentCodePtr = kLandsatLC8_OLI;

		else if (fileVectorPtr[0] == -1 && fileVectorPtr[1] == -1 &&
					fileVectorPtr[2] == -1 && fileVectorPtr[3] == -1 &&
					fileVectorPtr[4] == -1 && fileVectorPtr[5] == -1 &&
					fileVectorPtr[6] == -1 && fileVectorPtr[7] == -1)
			*instrumentCodePtr = kLandsatLC8_TIRS;
		*/
		}	// end "if (continueFlag)"

	return (continueFlag);

}	// end "CheckForLandsat8FileList"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CheckForSentinel2FileList
//
//	Software purpose:	The purpose of this routine is to check for a Sentinel 2A or 2B
//							file list. The full file path is used to determine whether
//							S2A_ or S2B_ exists anywhere in the file path. The identifier
//							is not always in the file name.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	True: first item in the FSRef file has been change.
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 08/22/2017
//	Revised By:			Larry L. Biehl			Date: 07/27/2018

Boolean CheckForSentinel2FileList (
				UInt32								itemCount,
				FSRef*								fileAsFSRefPtr,
				FSRef*								savedFsRefPtr,
				SInt16*								fileVectorPtr,
				char*									bandStringPtr,
				SInt16*								instrumentCodePtr)
{
	UCharPtr								startBandIdentiferPtr;
	UInt8									fileName[256],
											pathName[256],
											savedFileName[256];

   UInt32								fileIndex,
											fileVectorIndex;

   SInt16								bandNameStart,
											bandNumber,
											errCode,
											instrumentCode = 0,
											tReturnCode;

	Boolean								band8AFlag = FALSE,
											continueFlag;


			// Check if Sentinel 2 MSI data
			// This can be 10 meter data (2,3,4,8)or
			// 20 meter data (5,6,7,8a,11,12)or
			// 60 meter data (1,9,10)

   for (fileIndex=0; fileIndex<itemCount; fileIndex++)
		{
		continueFlag = FALSE;
		errCode = GetFilePathFromFSRef (&fileAsFSRefPtr[fileIndex], pathName);
		
		if (errCode == noErr)
			errCode = GetFileNameFromFSRef (&fileAsFSRefPtr[fileIndex], fileName);

		if (errCode == noErr)
			{
         RemoveSuffix ((FileStringPtr)fileName);

					// Save the first file name to compare to the rest in the list.

         if (fileIndex == 0)
				{
            CopyPToP (savedFileName, fileName);

						// Remove _Bn or _Bnn from the saved file name.

				startBandIdentiferPtr = (UCharPtr)strstr ((char*)savedFileName, "_B");

            bandNameStart = (SInt16)(startBandIdentiferPtr - savedFileName - 1);
            savedFileName[0] = (UInt8)bandNameStart;

						// There are 2 possible prefixes for Sentinel 2
				
				if (StrStrNoCase ((char*)&pathName[1], "S2A_"))
					instrumentCode = kSentinel2A_MSI;

            else if (StrStrNoCase ((char*)&pathName[1], "S2B_"))
					instrumentCode = kSentinel2B_MSI;
				
				else	// not Sentinel description
					break;
				
            bandNameStart++;

            }	// end "if (fileIndex == 0)"

					// For Sentinel 2, the band name is like _Bn where n can be 1 through 12.
					// Fill vector with the order of the n's
					// Note that the 8A band will be band 9, then 9 will be 10, etc.
			
			tReturnCode = sscanf ((char*)&fileName[bandNameStart],
										  "_B%hd",
										  &bandNumber);

			if (tReturnCode == 0 || (fileVectorPtr[7] != -1 && !band8AFlag))
				{
						// Band 8 has already been found or reading band 8A caused a problem
						// Check if this is the B8A band.
				
				if (CompareStringsNoCase (
											&fileName[bandNameStart], (UCharPtr)"_B8A", 4) == 0)
					{
					band8AFlag = TRUE;
					tReturnCode = 1;
					bandNumber = 8;
					continueFlag = TRUE;
					
					}	// end "if (CompareStringsNoCase (&fileName[bandNameStart], ..."

            }	// end "if (tReturnCode == 0)"

			if (tReturnCode == 1 &&
					!CompareStringsNoCase (
										&savedFileName[1], &fileName[1], savedFileName[0]) &&
						bandNumber >= 1 &&
							bandNumber <= 12)
				{
				if (bandNumber >= 8 && band8AFlag)
					bandNumber++;
					
				fileVectorIndex = bandNumber - 1;

						// Verify that there are no duplicate bands

				if (fileVectorPtr[fileVectorIndex] == -1)
					{
					fileVectorPtr[fileVectorIndex] = (SInt16)fileIndex;
					continueFlag = TRUE;

					}	// end "if (fileVector[fileVectorIndex] == -1)"

            }	// end "if (tReturnCode == 1 && ..."

			}	// end "if (errCode == noErr)"

		if (!continueFlag)
			break;

		}	// end "for (fileIndex=0; fileIndex<itemCount; fileIndex++)"

	if (continueFlag)
		{
				// This is a Sentinel list of files.
				// Now 

		*instrumentCodePtr = instrumentCode;

		}	// end "if (continueFlag)"

	return (continueFlag);

}	// end "CheckForSentinel2FileList"



#if defined multispec_mac
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void DrawCollapseClassOptionPopUp
//
//	Software purpose:	The purpose of this routine is to draw the collapse class
//							option selection for the file specification dialog.  This 
//							routine will also draw a drop shadow box around the pop up 
//							selection.
//
//	Parameters in:					
//
//	Parameters out:	None
//
//	Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/08/2004
//	Revised By:			Larry L. Biehl			Date: 05/08/2004	

pascal void DrawCollapseClassOptionPopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber)
{
		// Use the generic pop up drawing routine.

	DrawPopUpMenuItem (dialogPtr,
								itemNumber,
								gPopUpCollapseClassMenu,
								gCollapseClassSelection,
								TRUE);

}	// end "DrawCollapseClassOptionPopUp"
#endif	// defined multispec_mac 


#if defined multispec_mac
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void DrawHDFDataSetPopUp
//
//	Software purpose:	The purpose of this routine is to draw the hdf data set selection
//							popup for the file specification dialog.  This 
//							routine will also draw a drop shadow box around the pop up 
//							selection.
//
//	Parameters in:					
//
//	Parameters out:	None
//
//	Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/28/2001
//	Revised By:			Larry L. Biehl			Date: 11/28/2001	

pascal void DrawHDFDataSetPopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber)
{
		// Use the generic pop up drawing routine.

	DrawPopUpMenuItem (dialogPtr,
								itemNumber,
								gPopUpTemporaryMenu,
								gHdfDataSetSelection,
								TRUE);

}	// end "DrawHDFDataSetPopUp"
#endif	// defined multispec_mac


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean FileSpecificationDialog
//
//	Software purpose:	The purpose of this routine is to handle the modal 
//							dialog for confirming the image file description
//							for either multspectral type files and Thematic
//							(classification)type files. The modal dialog 
//							template is in the resource file. The default 
//							description assumes an ERDAS format.
//
//	Parameters in:		Pointer to window grafport.
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:			Menus in menus.cpp
//							ModalFileFormat in openImageRoutines.c
//							ChangeErdasHeaderDialog in reformat.c
//
//	Coded By:			Larry L. Biehl			Date: 04/12/1988
//	Revised By:			Larry L. Biehl			Date: 11/30/2018

Boolean FileSpecificationDialog (
				Handle								fileInfoHandle,
				Handle								windowInfoHandle,
				Boolean*								parameterChangeFlagPtr)
{
	Handle								newChannelToHdfDataSetHandle,
											newHfaHandle,
											newMapProjectionHandle;

	Boolean								returnFlag;

	SignedByte							fileInfoStatus,
											windowInfoStatus;


	fileInfoStatus = MHGetState (fileInfoHandle);
	windowInfoStatus = MHGetState (windowInfoHandle);

	newChannelToHdfDataSetHandle = NULL;
	newMapProjectionHandle = NULL;
	newHfaHandle = NULL;

#if defined multispec_mac
	CGEventSourceStateID				eventSource;

	Rect									bitsBox,
											bytesBox,
											interleaveBox,
											theBox;

	double								fillDataValue,
											sizeDifference;

	StringPtr							fileNamePtr;
	DialogPtr							dialogPtr;
	FileInfoPtr							fileInfoPtr;
	WindowInfoPtr						windowInfoPtr;
	//unsigned char*						charPtr;

	UserItemUPP							drawCollapseClassOptionPopUpPtr = NULL;
	UserItemUPP							drawDataValueTypePopUpPtr = NULL;
	UserItemUPP							drawHDFDataSetPopUpPtr = NULL;

	Handle								okHandle,
											theHandle;

	UInt32								blockHeight,
											blockWidth,
											numberHeaderBytes,
											numberChannelsClasses,
											numberColumns,
											numberLines,
											numberPreChannelBytes,
											numberPreLineBytes,
											numberPostChannelBytes,
											numberPostLineBytes,
											numberTrailerBytes;

	SInt32								maxLimit,
											minLimit,
											startColumn,
											startLine,
											theNum;

	SInt16								eightBitsPerDataSelection,
											gdalDataTypeCode,
											itemHit,
											itemHit2,
											numberBytes,
											returnCode,
											savedHdfDataSetSelection,
											theType;

	UInt16								dataCompressionCode;

	Boolean								callGetHDFLineFlag,
											changedFlag,
											checkOKFlag,
											fillDataValueFlag,
											forceGroupTableUpdateFlag,
											linesBottomToTopFlag,
											modalDone,
											okHiliteFlag,
											parameterChangeFlag,
											//parametersOKFlag,
											shiftKeyDownFlag,
											signedDataFlag,
											swapBytesFlag;


			// Initialize local variables.

	forceGroupTableUpdateFlag = FALSE;
	changedFlag = FALSE;

	if (parameterChangeFlagPtr != NULL)
		*parameterChangeFlagPtr = FALSE;

			// Get the modal dialog for the image file format description

	dialogPtr = LoadRequestedDialog (kFormatDescriptionID, kCopyScrap, 1, 2);
	if (dialogPtr == NULL)
																							return (FALSE);

			// Intialize local user item proc pointers.

	drawHDFDataSetPopUpPtr = NewUserItemUPP (DrawHDFDataSetPopUp);
	drawDataValueTypePopUpPtr = NewUserItemUPP (DrawDataValueTypePopUp);
	drawCollapseClassOptionPopUpPtr = NewUserItemUPP (DrawCollapseClassOptionPopUp);

			//	Set the draw routines.

	SetDialogItemDrawRoutine (
								dialogPtr, 28, &interleaveBox, gDrawBandInterleavePopUpPtr);
	SetDialogItemDrawRoutine (dialogPtr, 33, &bitsBox, drawDataValueTypePopUpPtr);
	SetDialogItemDrawRoutine (dialogPtr, 43, &theBox, drawHDFDataSetPopUpPtr);
	SetDialogItemDrawRoutine (dialogPtr, 45, &theBox, drawCollapseClassOptionPopUpPtr);

			// Update the specification dialog with the information given
			// in the format dialog

	FileSpecificationDialogInitialize (dialogPtr,
													fileInfoHandle,
													windowInfoHandle,
													&fileInfoPtr,
													&windowInfoPtr,
													&numberLines,
													&numberColumns,
													&numberChannelsClasses,
													&startLine,
													&startColumn,
													&numberHeaderBytes,
													&numberTrailerBytes,
													&numberPreLineBytes,
													&numberPostLineBytes,
													&numberPreChannelBytes,
													&numberPostChannelBytes,
													&blockHeight,
													&blockWidth,
													&gBandInterleaveSelection,
													&gDataValueTypeSelection,
													&eightBitsPerDataSelection,
													&swapBytesFlag,
													&signedDataFlag,
													&linesBottomToTopFlag,
													&fillDataValueFlag,
													&fillDataValue,
													&gHdfDataSetSelection,
													&gCollapseClassSelection,
													&callGetHDFLineFlag);

	fileNamePtr = (FileStringPtr)GetFileNamePPointerFromFileInfo (fileInfoPtr);

			// Save handle for the OK button for use later.

	GetDialogItem (dialogPtr, 1, &theType, &okHandle, &theBox);

			// Number of image lines.

	LoadDItemValue (dialogPtr, 5, numberLines);

			// Number of image columns

	LoadDItemValue (dialogPtr, 7, numberColumns);

			// Number of image channels, multispectral format.
			// Number of classes, Thematic format.											

	if (!fileInfoPtr->thematicType)
		LoadDItemValue (dialogPtr, 10, numberChannelsClasses);

	else	// fileInfoPtr->thematicType
		LoadDItemValue (dialogPtr, 10, numberChannelsClasses);

			// Start line of image

	LoadDItemValue (dialogPtr, 12, startLine);

			// Start column of image

	LoadDItemValue (dialogPtr, 14, startColumn);

			// Number of file header bytes

	LoadDItemValue (dialogPtr, 16, numberHeaderBytes);

			// Number of file trailer bytes

	LoadDItemValue (dialogPtr, 18, numberTrailerBytes);

			// Number of preline calibration bytes

	LoadDItemValue (dialogPtr, 20, numberPreLineBytes);

			// Number of postline calibration bytes.

	LoadDItemValue (dialogPtr, 22, numberPostLineBytes);

			// Number of preline calibration bytes

	LoadDItemValue (dialogPtr, 24, numberPreChannelBytes);

			// Number of postline calibration bytes.

	LoadDItemValue (dialogPtr, 26, numberPostChannelBytes);

			// Make certain that the band interleave for BSQ is enabled,
			// Make sure that the BNonSQ, BNonSQBlocked and BIBlock options
			//	are disabled. They can be displayed to indicate if those
			// interleave options are used in the current image file, but
			// one cannot set that parameter for now.

	EnableMenuItem (gPopUpBandInterleaveMenu, kBSQ);
	DisableMenuItem (gPopUpBandInterleaveMenu, kBNonSQ);
	DisableMenuItem (gPopUpBandInterleaveMenu, kBNonSQBlocked);
	DisableMenuItem (gPopUpBandInterleaveMenu, kBIBlock);

			// Set swap bytes check box.

	SetDLogControl (dialogPtr, 31, (SInt16)swapBytesFlag);

	EnableMenuItem (gPopUpDataValueTypeMenu, k4BitUnsignedIntegerMenuItem);
	//EnableMenuItem (gPopUpDataValueTypeMenu, 6);
	//DisableMenuItem (gPopUpDataValueTypeMenu, 9);

			// Set the image file name.														

	if (CreateUnicodeStaticTextControl (dialogPtr, 
													&fileNamePtr[1], 
													fileNamePtr[0], 
													37, 
													NULL) != noErr)
	LoadDItemString (dialogPtr, 37, (Str255*)fileNamePtr);

			// Set signed data check box.

	SetDLogControl (dialogPtr, 38, (SInt16)signedDataFlag);

			// Block height in lines
			// Block width in columns											

	LoadDItemValue (dialogPtr, 40, blockHeight);
	LoadDItemValue (dialogPtr, 42, blockWidth);

			// Treat lines as bottom to top flag.

	SetDLogControl (dialogPtr, 47, (SInt16)linesBottomToTopFlag);

			// Set fill data value flag and data value.

	SetDLogControl (dialogPtr, 29, (SInt16)fillDataValueFlag);
	LoadDItemRealValue (dialogPtr, 30, fillDataValue, 0);

			// Center the dialog and then show it.

	ShowDialogWindow (dialogPtr, kFormatDescriptionID, kSetUpDFilterTable);

	gDialogItemDescriptorPtr[30] = kDItemMinus + kDItemReal;

			// Do not allow specifications to be changed if the active
			// window represents the base image for an open project.					

	okHiliteFlag = TRUE;
	if (GetControlHilite ((ControlHandle)okHandle) == 255)
		okHiliteFlag = FALSE;

	if (gProcessorCode == kChangeImageDescriptionProcessor &&
            windowInfoPtr->projectWindowFlag)
		{
		HiliteControl ((ControlHandle)okHandle, 255);
		checkOKFlag = TRUE;
		okHiliteFlag = FALSE;
		itemHit = DisplayAlert (kErrorAlertID, kCautionAlert, kAlertStrID, 25, 0, NULL);

				// Force the main dialog box to be redrawn.

		InvalWindowRect (GetDialogWindow (dialogPtr),
									GetPortBounds (GetDialogPort (dialogPtr), &gTempRect));

		}	// end "if (gProcessorCode == kChangeImageDescriptionProcessor && ..."

	if ((gProcessorCode == kOpenImageFileProcessor ||
			gProcessorCode == kRefChangeFileFormatProcessor) &&
			(fileInfoPtr->format == kHDF4Type || fileInfoPtr->format == kNETCDFType ||
				fileInfoPtr->format == kHDF5Type || fileInfoPtr->format == kNETCDF2Type ||
				fileInfoPtr->format == kHDF4Type2 || fileInfoPtr->format == kNITFType) &&
					fileInfoPtr->numberHdfDataSets > 1 &&
								!gHDFDataSetSelectionAlertDisplayedFlag)
		{
		HiliteControl ((ControlHandle)okHandle, 255);
		DisplayAlert (kErrorAlertID,
							 kNoteAlert,
							 kAlertStrID,
							 IDS_Alert103,
							 IDS_Alert104,
							 NULL);
		HiliteControl ((ControlHandle)okHandle, 0);

				// Force the main dialog box to be redrawn.

		InvalWindowRect (GetDialogWindow (dialogPtr),
								GetPortBounds (GetDialogPort (dialogPtr), &gTempRect));

		gHDFDataSetSelectionAlertDisplayedFlag = TRUE;

		}	// end "if (gProcessorCode == kOpenImageFileProcessor && fileInfoPtr->...)"

			// Force any alert messages concerning the default hdf data set to be shown.

	if ((gProcessorCode == kOpenImageFileProcessor ||
			gProcessorCode == kRefChangeFileFormatProcessor) &&
            (fileInfoPtr->format == kHDF4Type || fileInfoPtr->format == kNETCDFType ||
				fileInfoPtr->format == kHDF5Type || fileInfoPtr->format == kNETCDF2Type ||
				fileInfoPtr->format == kHDF4Type2 || fileInfoPtr->format == kNITFType))
		{
		returnCode = FileSpecificationDialogSetHDFValues (dialogPtr,
																		 fileInfoPtr,
																		 gHdfDataSetSelection,
																		 okHandle,
																		 FALSE,
																		 &newMapProjectionHandle,
																		 &newHfaHandle,
																		 &newChannelToHdfDataSetHandle,
																		 &gBandInterleaveSelection,
																		 &gDataValueTypeSelection,
																		 &dataCompressionCode,
																		 &gdalDataTypeCode,
																		 &callGetHDFLineFlag);

		if (returnCode != noErr)
					// Force the main dialog box to be redrawn.
			InvalWindowRect (GetDialogWindow (dialogPtr),
									GetPortBounds (GetDialogPort (dialogPtr), &gTempRect));

		}	// end "if (gProcessorCode == kOpenImageFileProcessor && (...format == ..."
	/*
	itemHit = VerifyFileInfoDialogValues (dialogPtr,
															  numberLines,
															  numberColumns,
															  numberChannelsClasses,
															  startLine,
															  startColumn,
															  numberHeaderBytes,
															  numberTrailerBytes,
															  numberPreLineBytes,
															  numberPostLineBytes,
															  numberPreChannelBytes,
															  numberPostChannelBytes,
															  gDataValueTypeSelection, 
															  fileInfoPtr->thematicType);
														
	parametersOKFlag = (itemHit == 0);
	if (!parametersOKFlag)
		{
		HiliteControl ((ControlHandle)okHandle, 255);
		okHiliteFlag = FALSE;
		checkOKFlag = TRUE;

		if (itemHit == IDC_DataValueTypePopup)
				 itemHit = 5;

		}	// end "if (!parametersOKFlag)"

	else	// parametersOKFlag 
		itemHit = 5;
	*/
	if (!okHiliteFlag)
		checkOKFlag = TRUE;

			// Set default text selection to first edit text item

	SelectDialogItemText (dialogPtr, 5, 0, SHRT_MAX);

	modalDone = FALSE;
	itemHit = 0;
	do {
		if (checkOKFlag && !okHiliteFlag)
			{
			HiliteControl ((ControlHandle)okHandle, 255);
			checkOKFlag = FALSE;

			}	// end "if (checkOKFlag && !okHiliteFlag)"

		ModalDialog (gProcessorDialogFilterPtr, &itemHit);

		if (itemHit > 2)
			{
					//	 If itemHit was a number item, check for bad values.
					// If itemHit was a radio button make appropriate control		
					// settings to indicate to the user the present selection.		
					// Get the handle to the itemHit. For number value items, get	
					//	the string and convert it to a number.								

			GetDialogItem (dialogPtr, itemHit, &theType, &theHandle, &theBox);
			if (theType == 16)
				{
				GetDialogItemText (theHandle, gTextString);
				StringToNum (gTextString, &theNum);

            }	// end "if (theType == 16)"

			switch (itemHit)
				{
				case 5: //	 Number of image lines
					numberLines = theNum;
					if (theNum != fileInfoPtr->numberLines)
						changedFlag = TRUE;
					break;

				case 7: //	 Number of image columns  
					numberColumns = theNum;
					if (theNum != fileInfoPtr->numberColumns)
						changedFlag = TRUE;
					break;

				case 10: //	 Number of image channels or number of classes 		
					numberChannelsClasses = theNum;
					break;

				case 12: //	 Start line of image  
					startLine = theNum;
					break;

				case 14: //	 Start column of image  
					startColumn = theNum;
					break;

				case 16: //	 Number of file header bytes.  
					numberHeaderBytes = theNum;
					if (theNum != fileInfoPtr->numberHeaderBytes)
						changedFlag = TRUE;
					break;

				case 18: //	 Number of file trailer bytes.  
					numberTrailerBytes = theNum;
					break;

				case 20: //	 Number of preline calibration bytes.  
					numberPreLineBytes = theNum;
					if (theNum != fileInfoPtr->numberPreLineBytes)
						changedFlag = TRUE;
					break;

				case 22: //	 Number of postline calibration bytes.  
					numberPostLineBytes = theNum;
					if (theNum != fileInfoPtr->numberPostLineBytes)
						changedFlag = TRUE;
					break;

				case 24: //	 Number of prechannel calibration bytes.  
					numberPreChannelBytes = theNum;
					if (theNum != fileInfoPtr->numberPreChannelBytes)
						changedFlag = TRUE;
					break;

				case 26: //	 Number of postchannel calibration bytes.  
					numberPostChannelBytes = theNum;
					if (theNum != fileInfoPtr->numberPostChannelBytes)
						changedFlag = TRUE;
					break;

				case 28: // Band interleave format 
					itemHit2 = StandardPopUpMenu (dialogPtr,
															 27,
															 28,
															 gPopUpBandInterleaveMenu,
															 gBandInterleaveSelection,
															 kPopUpBandInterleaveMenuID);

					if (itemHit2 > 0)
						{
						gBandInterleaveSelection = itemHit2;
						FileSpecificationDialogSetInterleaveItems (
																			dialogPtr,
																			gBandInterleaveSelection,
																			fileInfoPtr->blockedFlag);

						}	// end "if (itemHit2 > 0)"
					break;

				case 29: // No Data (or Fill)Value flag 
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					fillDataValueFlag = !fillDataValueFlag;
					ShowHideDialogItem (dialogPtr, 30, fillDataValueFlag);
					break;

				case 31: // Swap bytes 
				case 35: // Compute number of classes flag 
				case 47: // Treat lines as bottom to top 
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					break;

				case 33: // data value type
					itemHit2 = StandardPopUpMenu (dialogPtr,
															 32,
															 33,
															 gPopUpDataValueTypeMenu,
															 gDataValueTypeSelection,
															 kPopUpDataValueTypeMenuID);
					if (itemHit2 > 0)
						{
						gDataValueTypeSelection = itemHit2;

						if (gDataValueTypeSelection > 3)
							SetDLogControlHilite (dialogPtr, 31, 0);

						else	// gDataValueTypeSelection <= 3
							{
							SetDLogControl (dialogPtr, 31, 0);
							SetDLogControlHilite (dialogPtr, 31, 255);

							}	// end "else gDataValueTypeSelection <= 3"

						}	// end "if (itemHit2 > 0)"
					break;

				case 34: // Determine number of classes from histogram 
					if (GetClassesFromHistogram (fileInfoPtr, gCollapseClassSelection))
						{
						LoadDItemValue (dialogPtr, 10, (SInt32)fileInfoPtr->numberClasses);
						forceGroupTableUpdateFlag = TRUE;

						}	// end "if (GetClassesFromHistogram (fileInfoPtr ..."

					if (fileInfoPtr->classesComputedFlag)
						SetDLogControlHilite (dialogPtr, 34, 255);
					break;

				case 43: // HDF Data Set
					itemHit2 = StandardPopUpMenu (dialogPtr,
															 36,
															 43,
															 gPopUpTemporaryMenu,
															 gHdfDataSetSelection,
															 kColorMenuID);

							// Determine if shift key is down. The user uses this to
							// specify that data sets are not to be grouped.

					eventSource = kCGEventSourceStateCombinedSessionState;
					shiftKeyDownFlag = (CGEventSourceKeyState (eventSource, 0x38));

					if (itemHit2 > 0 &&
							 (gHdfDataSetSelection != itemHit2 || shiftKeyDownFlag))
						{
						savedHdfDataSetSelection = gHdfDataSetSelection;
						gHdfDataSetSelection = itemHit2;

						InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);

								// Note that the info in the following handles may have
								// changed since it can vary by data set selection.

						newMapProjectionHandle =
								  DisposeMapProjectionHandle (newMapProjectionHandle);
						newHfaHandle = UnlockAndDispose (newHfaHandle);
						newChannelToHdfDataSetHandle =
								  UnlockAndDispose (newChannelToHdfDataSetHandle);

						returnCode = FileSpecificationDialogSetHDFValues (
																		  dialogPtr,
																		  fileInfoPtr,
																		  itemHit2,
																		  okHandle,
																		  shiftKeyDownFlag,
																		  &newMapProjectionHandle,
																		  &newHfaHandle,
																		  &newChannelToHdfDataSetHandle,
																		  &gBandInterleaveSelection,
																		  &gDataValueTypeSelection,
																		  &dataCompressionCode,
																		  &gdalDataTypeCode,
																		  &callGetHDFLineFlag);

						if (returnCode == noErr || returnCode == 20 || returnCode == 2)
							{
							gHdfDataSetSelection = itemHit2;
							InvalWindowRect (GetDialogWindow (dialogPtr), &interleaveBox);
							InvalWindowRect (GetDialogWindow (dialogPtr), &bytesBox);
							InvalWindowRect (GetDialogWindow (dialogPtr), &bitsBox);

							SelectDialogItemText (dialogPtr, 5, 0, SHRT_MAX);

							}	// end "if (returnCode == noErr || ..."

						else	// returnCode != noErr && returnCode != 20
							gHdfDataSetSelection = savedHdfDataSetSelection;

						FileSpecificationDialogSetInterleaveItems (dialogPtr,
																					gBandInterleaveSelection,
																					fileInfoPtr->blockedFlag);

						if (returnCode != noErr)
									// Force the main dialog box to be redrawn.
							InvalWindowRect (
											GetDialogWindow (dialogPtr),
											GetPortBounds (GetDialogPort (dialogPtr), &gTempRect));

						InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);

						}	// end "if (itemHit2 > 0)"
					break;

				case 45: // collapse class option
					itemHit2 = StandardPopUpMenu (dialogPtr,
															 44,
															 45,
															 gPopUpCollapseClassMenu,
															 gCollapseClassSelection,
															 kPopUpCollapseClassMenuID);
					if (itemHit2 > 0)
						{
						if (gCollapseClassSelection != itemHit2)
							{
							SetDLogControlHilite (dialogPtr, 34, 0);

							}	// end "if (itemHit2 > 0)"

						gCollapseClassSelection = itemHit2;

						}	// end "if (itemHit2 > 0)"
					break;

				case 46: // help message for hdf data set
					HiliteControl ((ControlHandle)okHandle, 255);
					DisplayAlert (kErrorAlertID,
										 kNoteAlert,
										 kAlertStrID,
										 IDS_Alert103,
										 IDS_Alert104,
										 NULL);
					HiliteControl ((ControlHandle)okHandle, 0);

							// Force the main dialog box to be redrawn.

					InvalWindowRect (GetDialogWindow (dialogPtr),
											GetPortBounds (GetDialogPort (dialogPtr), &gTempRect));
					break;

            }	// end "switch (itemHit)" 

			if (changedFlag)
				SetDLogControlHilite (dialogPtr, 34, 255);

			/*
			if (!parametersOKFlag)
				{
				itemHit = VerifyFileInfoDialogValues (dialogPtr,
																 numberLines,
																 numberColumns,
																 numberChannelsClasses,
																 startLine,
																 startColumn,
																 numberHeaderBytes,
																 numberTrailerBytes,
																 numberPreLineBytes,
																 numberPostLineBytes,
																 numberPreChannelBytes,
																 numberPostChannelBytes,
																 gDataValueTypeSelection, 
																 fileInfoPtr->thematicType);
				parametersOKFlag = (itemHit == 0);
				if (parametersOKFlag)
					SetDLogControlHilite (dialogPtr, 1, 0);

				else	// !parametersOKFlag
					NumberErrorAlert (GetDItemValue (dialogPtr, itemHit),
											dialogPtr,
											itemHit);

				}	// end "if (!parametersOKFlag)"
			*/
			}	// end "if (itemHit > 2)"

		else if (itemHit == 1)// User selected OK for information
			{
					// Make sure that we have the latest values for number lines, etc.
					// They could have been set in the hdf specification selection.

			numberLines = GetDItemValue (dialogPtr, 5);
			numberColumns = GetDItemValue (dialogPtr, 7);
			numberChannelsClasses = GetDItemValue (dialogPtr, 10);
			numberHeaderBytes = GetDItemValue (dialogPtr, 16);
			numberTrailerBytes = GetDItemValue (dialogPtr, 18);
			numberPreLineBytes = GetDItemValue (dialogPtr, 20);
			numberPostLineBytes = GetDItemValue (dialogPtr, 22);
			numberPreChannelBytes = GetDItemValue (dialogPtr, 24);
			numberPostChannelBytes = GetDItemValue (dialogPtr, 26);
			fillDataValue = GetDItemRealValue (dialogPtr, 30);

			itemHit = VerifyFileInfoDialogValues (dialogPtr,
															  numberLines,
															  numberColumns,
															  numberChannelsClasses,
															  startLine,
															  startColumn,
															  numberHeaderBytes,
															  numberTrailerBytes,
															  numberPreLineBytes,
															  numberPostLineBytes,
															  numberPreChannelBytes,
															  numberPostChannelBytes,
															  gDataValueTypeSelection,
															  fileInfoPtr->thematicType,
															  &minLimit,
															  &maxLimit);

			if (itemHit > 1)
				{
				HiliteControl ((ControlHandle)okHandle, 255);

				if (itemHit == IDC_DataValueTypePopUp)
					{
					returnCode = DisplayAlert (kErrorAlertID,
														 kStopAlert,
														 kAlertStrID,
														 IDS_Alert135,
														 0,
														 NULL);

					}	// end "if (itemHit == IDC_DataValueTypePopUp)"

				else	// itemHit != IDC_DataValueTypePopup
					{
					NumberErrorAlert (GetDItemValue (dialogPtr, itemHit),
											dialogPtr,
											itemHit);

					if (LoadSpecifiedStringNumberLongP (kAlertStrID,
																	IDS_Alert91,
																	(char*)gTextString,
																	(char*)gTextString2,
																	TRUE,
																	minLimit,
																	maxLimit))
						DisplayAlert (kErrorAlertID,
												kStopAlert,
												0,
												0,
												0,
												gTextString);

					}	// end "else itemHit != IDC_DataValueTypePopUp"

				if (okHiliteFlag)
					HiliteControl ((ControlHandle)okHandle, 0);

						// Force the main dialog box to be redrawn.

				InvalWindowRect (GetDialogWindow (dialogPtr),
										GetPortBounds (GetDialogPort (dialogPtr), &gTempRect));

            }	// end "if (itemHit > 1)"

			}	// end "if (itemHit == 1)"

		if (itemHit == 1)
			{
					// Check file parameters against the size of the file.
					// Display an alert message if the parameters specify a		
					// file that is larger than the file actually is.	

					// Do not check HDF files with more than one data set
					// for now. A more complex check will need to be made to
					// get the name of external files.			

			sizeDifference = 0;
			if ((fileInfoPtr->format != kHDF4Type && fileInfoPtr->format != kNETCDFType &&
						fileInfoPtr->format != kHDF5Type &&
							fileInfoPtr->format != kNETCDF2Type &&
								fileInfoPtr->format != kHDF4Type2 &&
									fileInfoPtr->format != kNITFType) ||
										fileInfoPtr->numberHdfDataSets <= 1)
				{
				theNum = 1;
				if (!fileInfoPtr->thematicType)
					theNum = numberChannelsClasses;

				numberBytes =
							FileSpecificationDialogGetNumberBytes (gDataValueTypeSelection);

				sizeDifference = CompareFileInfoAndFileSize (fileInfoPtr,
																			numberLines,
																			numberColumns,
																			theNum,
																			numberBytes,
																			numberPreLineBytes,
																			numberPostLineBytes,
																			numberPreChannelBytes,
																			numberPostChannelBytes,
																			numberHeaderBytes,
																			numberTrailerBytes,
																			gBandInterleaveSelection,
																			(gDataValueTypeSelection == 1));

            }	// end "if ((...format != kHDF4Type && != kNETCDFType)|| ...."

			if (sizeDifference > 0 && SizeOfImageFileCanBeCalculated (fileInfoPtr))
				{
						// Display an alert.

				GetIndString (gTextString, kAlertStrID, 1);
				ConcatPStrings (gTextString, fileNamePtr, 255);

				GetIndString (gTextString2, kAlertStrID, 2);
				ConcatPStrings (gTextString, gTextString2, 255);

				sprintf ((char*)gTextString2, "%.0lf", sizeDifference);
				CopyCStringToPascal ((char*)gTextString2, gTextString2);
				ConcatPStrings (gTextString, gTextString2, 255);

				GetIndString (gTextString3, kAlertStrID, 3);
				ConcatPStrings (gTextString, gTextString3, 255);

				HiliteControl ((ControlHandle)okHandle, 255);
				returnCode = DisplayAlert (kRedoDontCancelAlertID,
													kCautionAlert,
													0,
													0,
													0,
													&gTextString[0]);

				if (okHiliteFlag)
					HiliteControl ((ControlHandle)okHandle, 0);

						// Force the main dialog box to be redrawn.

				InvalWindowRect (GetDialogWindow (dialogPtr),
										GetPortBounds (GetDialogPort (dialogPtr), &gTempRect));

				if (returnCode == 1)
					itemHit = 3;
				else if (returnCode == 2)
					itemHit = 2;
				else if (returnCode == 3)
					itemHit = 1;

				}	// end "if (sizeDifference > 0 && ...)"

			}	// end "if (itemHit == 1)"

		if (itemHit == 1)
			{
			modalDone = TRUE;
			returnFlag = TRUE;

			parameterChangeFlag = FileSpecificationDialogOK (
															dialogPtr,
															fileInfoPtr,
															fileInfoHandle,
															&newMapProjectionHandle,
															&newHfaHandle,
															&newChannelToHdfDataSetHandle,
															windowInfoPtr,
															windowInfoHandle,
															numberLines,
															numberColumns,
															numberChannelsClasses,
															startLine,
															startColumn,
															numberHeaderBytes,
															numberTrailerBytes,
															numberPreLineBytes,
															numberPostLineBytes,
															numberPreChannelBytes,
															numberPostChannelBytes,
															(UInt32)GetDItemValue (dialogPtr, 40),
															(UInt32)GetDItemValue (dialogPtr, 42),
															forceGroupTableUpdateFlag,
															gBandInterleaveSelection,
															gDataValueTypeSelection,
															(Boolean)GetDLogControl (dialogPtr, 31),
															(Boolean)GetDLogControl (dialogPtr, 38),
															(Boolean)GetDLogControl (dialogPtr, 47),
															(Boolean)GetDLogControl (dialogPtr, 35),
															(Boolean)GetDLogControl (dialogPtr, 29),
															fillDataValue,
															gHdfDataSetSelection,
															0,
															gCollapseClassSelection,
															dataCompressionCode,
															gdalDataTypeCode,
															callGetHDFLineFlag);

			if (parameterChangeFlagPtr != NULL)
				*parameterChangeFlagPtr = parameterChangeFlag;

			}	// end "if (itemHit == 1)"

		if (itemHit == 2)// User selected Cancel for information
			{
			modalDone = TRUE;
			returnFlag = FALSE;

			}	// end "if	(itemHit == 2)"

		} while (!modalDone);

			// Now delete the hdf data set menu items.

	SInt16 numberMenuItems = CountMenuItems (gPopUpTemporaryMenu);

	DeleteMenuItems (gPopUpTemporaryMenu, 1, numberMenuItems);

	DisposeUserItemUPP (drawCollapseClassOptionPopUpPtr);
	DisposeUserItemUPP (drawHDFDataSetPopUpPtr);
	DisposeUserItemUPP (drawDataValueTypePopUpPtr);

	CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
#endif	// defined multispec_mac					

	#if defined multispec_win
		CMFileFormatSpecsDlg* dialogPtr = NULL;

		TRY
			{
			dialogPtr = new CMFileFormatSpecsDlg ();

			returnFlag = dialogPtr->DoDialog (fileInfoHandle,
															windowInfoHandle,
															&newMapProjectionHandle,
															&newHfaHandle,
															&newChannelToHdfDataSetHandle,
															parameterChangeFlagPtr);

			  if (dialogPtr != NULL)
					delete dialogPtr;
			}

		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			}
		 
		END_CATCH_ALL
	#endif   //	defined multispec_win

	#if defined multispec_lin
		CMFileFormatSpecsDlg* dialogPtr = NULL;
		try
			{
					// Try to get the current top window.
					
			//wxFrame* parentFrame = GetActiveFrame ();
			//dialogPtr = new CMFileFormatSpecsDlg ((wxWindow *)gActiveImageViewCPtr->m_frame);
			//dialogPtr = new CMFileFormatSpecsDlg (parentFrame);
			//dialogPtr = new CMFileFormatSpecsDlg ((wxWindow*)GetMainFrame ());
			dialogPtr = new CMFileFormatSpecsDlg (NULL);
		 
			returnFlag = dialogPtr->DoDialog (fileInfoHandle,
															windowInfoHandle,
															&newMapProjectionHandle,
															&newHfaHandle,
															&newChannelToHdfDataSetHandle,
															parameterChangeFlagPtr);

			if (dialogPtr != NULL)
				delete dialogPtr;
			}
		catch (int e)
			{
			MemoryMessage (0, kObjectMessage);
			}
	#endif
			// Unlock the file and window information handles if needed.

	MHSetState (fileInfoHandle, fileInfoStatus);
	MHSetState (windowInfoHandle, windowInfoStatus);

	UnlockAndDispose (newMapProjectionHandle);
	UnlockAndDispose (newHfaHandle);
	UnlockAndDispose (newChannelToHdfDataSetHandle);

	return (returnFlag);

}	// end "FileSpecificationDialog"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void FileSpecificationDialogInitialize
//
//	Software purpose:	The purpose of this routine is to hide and show the items in
//							the file specification dialog box that depend on the band
//							interleave setting.
//
//	Parameters in:		
//
//	Parameters out:	
//
//	Value Returned:	
//
// Called By:			FileSpecificationDialog in openImageRoutines.c
//
//	Coded By:			Larry L. Biehl			Date: 11/01/1999
//	Revised By:			Larry L. Biehl			Date: 07/29/2018

void FileSpecificationDialogInitialize (
            DialogPtr                     dialogPtr,
            Handle                        fileInfoHandle,
            Handle                        windowInfoHandle,
            FileInfoPtr*                  fileInfoPtrPtr,
            WindowInfoPtr*                windowInfoPtrPtr,
            UInt32*                       numberLinesPtr,
            UInt32*                       numberColumnsPtr,
            UInt32*                       numberChannelsPtr,
            SInt32*                       startLinePtr,
            SInt32*								startColumnPtr,
            UInt32*                       numberHeaderBytesPtr,
            UInt32*                       numberTrailerBytesPtr,
            UInt32*                       numberPreLineBytesPtr,
            UInt32*                       numberPostLineBytesPtr,
            UInt32*                       numberPreChannelBytesPtr,
            UInt32*                       numberPostChannelBytesPtr,
            UInt32*								blockHeightPtr,
            UInt32*								blockWidthPtr,
            SInt16*								bandInterleaveSelectionPtr,
            SInt16*								dataValueTypeSelectionPtr,
            SInt16*								eightBitsPerDataSelectionPtr,
            Boolean*								swapBytesFlagPtr,
            Boolean*								signedDataFlagPtr,
            Boolean*								linesBottomToTopFlagPtr,
            Boolean*								fillDataValueExistsFlagPtr,
            double*								fillDataValuePtr,
            SInt16*								hdfDataSetPtr,
            SInt16*								collapseClassesSelectionPtr,
            Boolean*                      callGetHDFLineFlagPtr)
{
   FileInfoPtr                   fileInfoPtr;
   HdfDataSets*                  hdfDataSetsPtr;
   WindowInfoPtr                 windowInfoPtr;

   UInt32                        blockOffset,
                                 blockSize;

   SInt16								firstHdfDataSetSelection;

   Boolean								dataSetPromptFlag = FALSE;


         // Lock the memory, in case it isn't already locked, until the information
         // has been loaded in. Then get the pointer to the file information block.				

   *fileInfoPtrPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle, kLock);

   *windowInfoPtrPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle, kLock);

   fileInfoPtr = *fileInfoPtrPtr;
   windowInfoPtr = *windowInfoPtrPtr;

         // Do not allow specifications to be changed if the active
         // window represents linked images.

   if (windowInfoPtr != NULL &&
         gProcessorCode == kChangeImageDescriptionProcessor &&
         windowInfoPtr->numberImageFiles > 1)
     SetDLogControlHilite (dialogPtr, IDOK, 255);

         // Do not allow specifications to be changed if the active
         // window represents the base image for an open project.

   if (windowInfoPtr != NULL &&
         gProcessorCode == kChangeImageDescriptionProcessor &&
				windowInfoPtr->projectWindowFlag)
		SetDLogControlHilite (dialogPtr, IDOK, 255);

         // Number of image lines.

	*numberLinesPtr = fileInfoPtr->numberLines;

         // Number of image columns

   *numberColumnsPtr = fileInfoPtr->numberColumns;

         // Number of image channels, multispectral format.
         // Number of classes, Thematic format.

   if (!fileInfoPtr->thematicType)
		*numberChannelsPtr = fileInfoPtr->numberChannels;

   else	// fileInfoPtr->thematicType 
		*numberChannelsPtr = fileInfoPtr->numberClasses;

         // Start line of image

   *startLinePtr = fileInfoPtr->startLine;

         // Start column of image

   *startColumnPtr = fileInfoPtr->startColumn;

         // Number of file header bytes

   *numberHeaderBytesPtr = fileInfoPtr->numberHeaderBytes;

         // Number of file trailer bytes

   *numberTrailerBytesPtr = fileInfoPtr->numberTrailerBytes;

         // Number of preline calibration bytes

   *numberPreLineBytesPtr = fileInfoPtr->numberPreLineBytes;

         // Number of postline calibration bytes.

   *numberPostLineBytesPtr = fileInfoPtr->numberPostLineBytes;

         // Number of preline calibration bytes

   *numberPreChannelBytesPtr = fileInfoPtr->numberPreChannelBytes;

         // Number of postline calibration bytes.

   *numberPostChannelBytesPtr = fileInfoPtr->numberPostChannelBytes;

         // Band interleave format

   *bandInterleaveSelectionPtr = fileInfoPtr->bandInterleave;
   FileSpecificationDialogSetInterleaveItems (dialogPtr,
																*bandInterleaveSelectionPtr,
                                                fileInfoPtr->blockedFlag);

         // Number of bytes of data

   //HideDialogItem (dialogPtr, IDC_FillDataValueExists);
   //HideDialogItem (dialogPtr, IDC_FillDataValue);
   HideDialogItem (dialogPtr, IDC_SignedData);

         // Set swap bytes check box.

   *swapBytesFlagPtr = FALSE;
   if (fileInfoPtr->numberBytes >= 2)
      *swapBytesFlagPtr = fileInfoPtr->swapBytesFlag;

   else	// fileInfoPtr->numberBytes != 2 
      SetDLogControlHilite (dialogPtr, IDC_SwapBytes, 255);

         //	Get the data value type selection.

   *eightBitsPerDataSelectionPtr = 3;

   *dataValueTypeSelectionPtr = FileSpecificationDialogSetDataType (
																			fileInfoPtr->dataTypeCode,
																			fileInfoPtr->numberBits,
																			fileInfoPtr->signedDataFlag);

         // Set Items for multispectral format only.

   if (!fileInfoPtr->thematicType)
      {
      HideDialogItem (dialogPtr, IDC_STATIC9);
      HideDialogItem (dialogPtr, IDC_DetermineNumClasses);
      HideDialogItem (dialogPtr, IDC_ComputeNumClasses);

      }	// end "if (!fileInfoPtr->thematicType)"

        // Set & Hide items for Thematic type file.								

   else	// fileInfoPtr->thematicType
      {
        // Get the requested prompt string.											

		#if defined multispec_mac
         GetIndString (gTextString, kDialogStrID, kDialogStr1);
         SetWTitle (GetDialogWindow (dialogPtr), gTextString);
		#endif 	// defined multispec_mac

      HideDialogItem (dialogPtr, IDC_STATIC8);
      HideDialogItem (dialogPtr, IDC_PreChannelPrompt);
      HideDialogItem (dialogPtr, IDC_PreChannelBytes);
      HideDialogItem (dialogPtr, IDC_PostChannelPrompt);
      HideDialogItem (dialogPtr, IDC_PostChannelBytes);
      HideDialogItem (dialogPtr, IDC_STATIC27);
      HideDialogItem (dialogPtr, IDC_BandInterleavePopUp);
      HideDialogItem (dialogPtr, IDC_SignedData);

            // If 'bytesPer1lineAllChans' is 0, then the intermediate
            // parameters have not been set up yet.  Computing the number		
            // of classes cannot be done until after the dialog box is 			
            // closed.																			

      if (fileInfoPtr->bytesPer1lineAllChans == 0 ||
                fileInfoPtr->numberClasses > kMaxNumberClasses)
         {
         HideDialogItem (dialogPtr, IDC_DetermineNumClasses);
         if (fileInfoPtr->numberClasses > kMaxNumberClasses)
                SetDLogControlHilite (dialogPtr, IDC_ComputeNumClasses, 255);

         }    // end "if (fileInfoPtr->bytesPer1line1chan == 0 || ...)"

      else	//fileInfoPtr->bytesPer1line1chan != 0 && ...
         HideDialogItem (dialogPtr, IDC_ComputeNumClasses);

      if (fileInfoPtr->classesComputedFlag)
         SetDLogControlHilite (dialogPtr, IDC_DetermineNumClasses, 255);

      }	// end "else fileInfoPtr->thematicType"

         // Block height and width. Get the current settings.

   GetBlockInformation (fileInfoPtr,
                           blockHeightPtr,
                           blockWidthPtr,
                           &blockSize,
                           &blockOffset);

   *linesBottomToTopFlagPtr = fileInfoPtr->treatLinesAsBottomToTopFlag;

   *fillDataValueExistsFlagPtr = fileInfoPtr->noDataValueFlag;
   *fillDataValuePtr = fileInfoPtr->noDataValue;
   if (!fileInfoPtr->noDataValueFlag)
      HideDialogItem (dialogPtr, IDC_FillDataValue);

         // HDF Data Set popup menu

   dataSetPromptFlag = (fileInfoPtr->hdfHandle != NULL);
   //		fileInfoPtr->format == kHDF4Type ||
   //				fileInfoPtr->format == kNETCDFType || fileInfoPtr->format == kHDF5Type);
   if (!(dataSetPromptFlag && gProcessorCode == kOpenImageFileProcessor) &&
			!(dataSetPromptFlag && gProcessorCode == kRefChangeFileFormatProcessor) &&
			!(dataSetPromptFlag && gProcessorCode == kChangeImageDescriptionProcessor) &&
			!(dataSetPromptFlag && gProcessorCode == kOpenProjectImageProcessor))
      {
      HideDialogItem (dialogPtr, IDC_HDFDataSetPrompt);
      HideDialogItem (dialogPtr, IDC_HDFDataSet);
      HideDialogItem (dialogPtr, IDC_HDFDataSetHelp);

      }	// end "if (fileInfoPtr->format != kHDF4Type || ..."

   *hdfDataSetPtr = fileInfoPtr->hdfDataSetSelection;
    
         // If hdf files are being linked then see if one can use the same hdf data set
         // as was selected for the first image file.

   if (gProcessorCode == kOpenImageFileProcessor &&
			(fileInfoPtr->format == kHDF4Type || fileInfoPtr->format == kNETCDFType ||
				fileInfoPtr->format == kHDF5Type || fileInfoPtr->format == kNETCDF2Type ||
				fileInfoPtr->format == kHDF4Type2 || fileInfoPtr->format == kNITFType) &&
						fileInfoPtr->numberHdfDataSets > 1 &&
								gLastHDFNumberDataSets > 0 &&
									fileInfoPtr->numberHdfDataSets >= gLastHDFDataSetSelection)
                              //fileInfoPtr->numberHdfDataSets == gLastHDFNumberDataSets)
                              //gMultipleImageFileCode == 2)
      {
      //imageFileInfoHandle = GetFileInfoHandle (gActiveImageWindowInfoH);
      //firstHdfDataSetSelection = GetHdfDataSetSelection (imageFileInfoHandle);
      firstHdfDataSetSelection = gLastHDFDataSetSelection;

      if (fileInfoPtr->numberHdfDataSets > firstHdfDataSetSelection)
         {
         hdfDataSetsPtr = (HdfDataSets*)GetHandlePointer (fileInfoPtr->hdfHandle,
																				kLock);

         if (hdfDataSetsPtr[firstHdfDataSetSelection].name[1] == '*')
            *hdfDataSetPtr = firstHdfDataSetSelection;
            
         }	// end "if (fileInfoPtr->numberHdfDataSets > firstHdfDataSetSelection)"

      }	// end "if (gProcessorCode == kOpenImageFileProcessor && ..."

   SetUpHDFDataSetPopupMenu (dialogPtr, fileInfoPtr);

         // Set option for collapsing classes

   *collapseClassesSelectionPtr = fileInfoPtr->collapseClassSelection;

         // Set current option for whether to call the HDF4 get line routine.

   *callGetHDFLineFlagPtr = fileInfoPtr->callGetHDFLineFlag;

   if (!fileInfoPtr->thematicType)
      {
      HideDialogItem (dialogPtr, IDC_CollapseClassPrompt);
      HideDialogItem (dialogPtr, IDC_CollapseClass);

      }	// end "if (!fileInfoPtr->thematicType)"

}	// end "FileSpecificationDialogInitialize"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean FileSpecificationDialogOK
//
//	Software purpose:	The purpose of this routine is to hide and show the items in
//							the file specification dialog box that depend on the band
//							interleave setting.
//
//	Parameters in:		
//
//	Parameters out:	
//
//	Value Returned:	
//
// Called By:			FileSpecificationDialog in SOpenFileDialog.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/27/1999
//	Revised By:			Larry L. Biehl			Date: 09/30/2018

Boolean FileSpecificationDialogOK (
				DialogPtr							dialogPtr,
				FileInfoPtr							fileInfoPtr,
				Handle								fileInfoHandle,
				Handle*								newMapProjectionHandlePtr,
				Handle*								newHfaHandlePtr,
				Handle*								newChannelToHdfDataSetHandlePtr,
				WindowInfoPtr						windowInfoPtr,
				Handle								windowInfoHandle,
				UInt32								numberLines,
				UInt32								numberColumns,
				UInt32								numberChannels,
				SInt32 								startLine,
				SInt32 								startColumn,
				UInt32 								numberHeaderBytes,
				UInt32 								numberTrailerBytes,
				UInt32 								numberPreLineBytes,
				UInt32 								numberPostLineBytes,
				UInt32 								numberPreChannelBytes,
				UInt32 								numberPostChannelBytes,
				UInt32 								blockHeight,
				UInt32 								blockWidth,
				Boolean 								forceGroupTableUpdateFlag,
				SInt16 								bandInterleaveSelection,
				SInt16 								dataValueTypeSelection,
				Boolean 								swapBytesFlag,
				Boolean 								signedDataFlag,
				Boolean 								linesBottomToTopFlag,
				Boolean 								computeNumberClassesFlag,
				Boolean 								fillDataValueExistsFlag,
				double 								fillDataValue,
				SInt16 								hdfDataSetSelection,
				UInt32 								hdfDataSetIndex,
				SInt16 								collapseClassesSelection,
				UInt16 								dataCompressionCode,
				SInt16 								gdalDataTypeCode,
				Boolean 								callGetHDFLineFlag)
{
	SInt16								coordinateViewUnites,
											dataTypeCode,
											lineColumnUnitsMenuItem,
											numberBits,
											offset;

	SInt32								bytesPerDataValue;

	UInt32								currentBlockHeight,
											currentBlockOffset,
											currentBlockSize,
											currentBlockWidth;

	Boolean								callUpdateLayerInfoStructureFlag,
											changedFlag,
											latLongPossibleFlag,
											reloadChannelDescriptionsFlag;


   changedFlag = FALSE;
	reloadChannelDescriptionsFlag = FALSE;
	callUpdateLayerInfoStructureFlag = FALSE;

			// Number of image lines

	if (fileInfoPtr->numberLines != numberLines)
		changedFlag = TRUE;
	fileInfoPtr->numberLines = numberLines;

			// Number of image columns

	if (fileInfoPtr->numberColumns != numberColumns)
		changedFlag = TRUE;
	fileInfoPtr->numberColumns = numberColumns;

			// Number of image channels or Number of classes.

	if (!fileInfoPtr->thematicType)
		{
		if (fileInfoPtr->numberChannels != numberChannels)
			{
			changedFlag = TRUE;
			fileInfoPtr->channelDescriptionH =
					  UnlockAndDispose (fileInfoPtr->channelDescriptionH);
			fileInfoPtr->channelValuesHandle =
					  UnlockAndDispose (fileInfoPtr->channelValuesHandle);

			fileInfoPtr->descriptionsFlag = FALSE;
			fileInfoPtr->descriptionCode = 0;
			fileInfoPtr->numberChannels = (UInt16)numberChannels;
			reloadChannelDescriptionsFlag = TRUE;

			}	// end "if (fileInfoPtr->numberChannels != numberChannels)"

		}	// end "if (!fileInfoPtr->thematicType)"

	else	// fileInfoPtr->thematicType
		{
				// Check number of classes.  If the number is different,
				// then reload the class names.
				// Note that maxClassNumberValue will be updated in 	
				// LoadClassNameDescriptions if needed.

		if (fileInfoPtr->numberClasses != numberChannels ||
                fileInfoPtr->collapseClassSelection != collapseClassesSelection)
			{
			if (fileInfoPtr->numberClasses != numberChannels &&
																		collapseClassesSelection == 2)
				fileInfoPtr->classesComputedFlag = FALSE;

			fileInfoPtr->classDescriptionH =
											UnlockAndDispose (fileInfoPtr->classDescriptionH);
			fileInfoPtr->descriptionsFlag = FALSE;
			fileInfoPtr->numberClasses = numberChannels;
			fileInfoPtr->collapseClassSelection = collapseClassesSelection;

			//if (fileInfoPtr->format != kMultiSpecClassificationType &&
			//					fileInfoPtr->format != kTIFFType &&
			//						fileInfoPtr->format != kGeoTIFFType)
			fileInfoPtr->asciiSymbols = FALSE;

			if (fileInfoPtr->numberClasses > 0)
				fileInfoPtr->maxClassNumberValue = fileInfoPtr->numberClasses - 1;

			LoadClassNameDescriptions (windowInfoHandle);

			forceGroupTableUpdateFlag = TRUE;

			}	// end "if (fileInfoPtr->numberClasses != numberChannels ||"

		fileInfoPtr->numberChannels = 1;

		if (forceGroupTableUpdateFlag)
			changedFlag = TRUE;

		}	// end "else fileInfoPtr->thematicType"

			// Start line of image

	fileInfoPtr->startLine = startLine;

			// Start column of image

	fileInfoPtr->startColumn = startColumn;

			// Force band interleave selection not to be changed
			// between kBNonSq, kBNonSqBlock, and kBIBlock for now.

	if (fileInfoPtr->bandInterleave >= kBNonSQ &&
            fileInfoPtr->bandInterleave <= kBIBlock &&
            bandInterleaveSelection >= kBNonSQ)
		bandInterleaveSelection = fileInfoPtr->bandInterleave;

			// Number of file header bytes

	if (bandInterleaveSelection == kBIL ||
            bandInterleaveSelection == kBSQ ||
            bandInterleaveSelection == kBIS)
		{
		if (fileInfoPtr->numberHeaderBytes != numberHeaderBytes)
			{
			changedFlag = TRUE;
			reloadChannelDescriptionsFlag = TRUE;

			}	// end "if (fileInfoPtr->numberHeaderBytes != numberHeaderBytes)"

		fileInfoPtr->numberHeaderBytes = numberHeaderBytes;
		if (changedFlag && fileInfoPtr->numberHeaderBytes == 0)
			{
			if (fileInfoPtr->format != kHDF4Type &&
                    fileInfoPtr->format != kNETCDFType &&
                    fileInfoPtr->format != kHDF5Type &&
                    fileInfoPtr->format != kNETCDF2Type &&
						  fileInfoPtr->format != kHDF4Type2 &&
						  fileInfoPtr->format != kNITFType &&
                    fileInfoPtr->gdalDataSetH != NULL)
				fileInfoPtr->format = 0;

			}	// end "if (changedFlag && fileInfoPtr->numberHeaderBytes == 0)"

		}	// end "if (bandInterleaveSelection == kBIL || ...

			// Number of file trailer bytes

	if (bandInterleaveSelection == kBIL ||
            bandInterleaveSelection == kBSQ ||
            bandInterleaveSelection == kBIS)
		{
		if (fileInfoPtr->numberTrailerBytes != numberTrailerBytes)
			changedFlag = TRUE;
		fileInfoPtr->numberTrailerBytes = numberTrailerBytes;

		}	// end "if (bandInterleaveSelection == kBIL || ..."

	if (bandInterleaveSelection == kBIL ||
            bandInterleaveSelection == kBSQ ||
            bandInterleaveSelection == kBIS ||
            bandInterleaveSelection == kBNonSQ)
		{
				// Number of preline bytes

		if (fileInfoPtr->numberPreLineBytes != numberPreLineBytes)
			{
			changedFlag = TRUE;
			reloadChannelDescriptionsFlag = TRUE;

			}	// end "if (...->numberPreLineBytes != numberPreLineBytes)"

		fileInfoPtr->numberPreLineBytes = numberPreLineBytes;

				// Number of postline bytes

		if (fileInfoPtr->numberPostLineBytes != numberPostLineBytes)
			{
			changedFlag = TRUE;
			reloadChannelDescriptionsFlag = TRUE;

			}	// end "if (...->numberPostLineBytes != numberPostLineBytes)"

		fileInfoPtr->numberPostLineBytes = numberPostLineBytes;

		}	// end "if (bandInterleaveSelection == kBIL || ..."

	if (bandInterleaveSelection == kBIL ||
            bandInterleaveSelection == kBSQ ||
						bandInterleaveSelection == kBNonSQ)
		{
				// Number of prechannel calibration bytes

		if (fileInfoPtr->numberPreChannelBytes != numberPreChannelBytes)
			{
			changedFlag = TRUE;
			reloadChannelDescriptionsFlag = TRUE;

			}	// end "if (...->numberPreChannelBytes != numberPreChannelBytes)"

		fileInfoPtr->numberPreChannelBytes = numberPreChannelBytes;

				// Number of postchannel calibration bytes

		if (fileInfoPtr->numberPostChannelBytes != numberPostChannelBytes)
			{
			changedFlag = TRUE;
			reloadChannelDescriptionsFlag = TRUE;

			}	// end "if (...->numberPostChannelBytes != numberPostChannelBytes)"

		fileInfoPtr->numberPostChannelBytes = numberPostChannelBytes;

		}	// end "if (bandInterleaveSelection == kBIL || ..."

	if (bandInterleaveSelection == kBNonSQBlocked ||
																bandInterleaveSelection == kBIBlock)
		{
		GetBlockInformation (fileInfoPtr,
									 &currentBlockHeight,
									 &currentBlockWidth,
									 &currentBlockSize,
									 &currentBlockOffset);

				// Number of block height lines

		if (currentBlockHeight != blockHeight)
			changedFlag = TRUE;

				// Number of block height columns

		if (currentBlockWidth != blockWidth)
			changedFlag = TRUE;

		}	// end "if (bandInterleaveSelection == kBNonSQBlocked || ..."

			// Image file format (BIL, BSQ, BIS, kBNonSQ, kBNonSQBlocked,
			// kBIBlock)

	if (fileInfoPtr->bandInterleave != bandInterleaveSelection)
		changedFlag = TRUE;
	fileInfoPtr->bandInterleave = bandInterleaveSelection;

			// Swap bytes (yes or no)

	if (fileInfoPtr->swapBytesFlag != swapBytesFlag)
		{
		changedFlag = TRUE;
		fileInfoPtr->swapBytesFlag = swapBytesFlag;

		}	// end "if (fileInfoPtr->swapBytesFlag != ..."

	dataTypeCode = kIntegerType;
	switch (dataValueTypeSelection)
		{
		case k4BitUnsignedIntegerMenuItem:
			numberBits = 4;
			bytesPerDataValue = 1;
			signedDataFlag = FALSE;
			break;

		case k1ByteSignedIntegerMenuItem:
			numberBits = 8;
			bytesPerDataValue = 1;
			signedDataFlag = TRUE;
			break;

		case k1ByteUnsignedIntegerMenuItem:
			numberBits = 8;
			bytesPerDataValue = 1;
			signedDataFlag = FALSE;
			break;

		case k2ByteSignedIntegerMenuItem:
			numberBits = 16;
			bytesPerDataValue = 2;
			signedDataFlag = TRUE;
			break;

		case k2ByteUnsignedIntegerMenuItem:
			numberBits = 16;
			bytesPerDataValue = 2;
			signedDataFlag = FALSE;
			break;

		case k4ByteSignedIntegerMenuItem:
			numberBits = 32;
			bytesPerDataValue = 4;
			signedDataFlag = TRUE;
			break;

		case k4ByteUnsignedIntegerMenuItem:
			numberBits = 32;
			bytesPerDataValue = 4;
			signedDataFlag = FALSE;
			break;

		case k4ByteRealMenuItem:
			numberBits = 32;
			bytesPerDataValue = 4;
			signedDataFlag = TRUE;
			dataTypeCode = kRealType;
			break;

		case k8ByteRealMenuItem:
			numberBits = 64;
			bytesPerDataValue = 8;
			signedDataFlag = TRUE;
			dataTypeCode = kRealType;
			break;

		}	// end "switch (bitsPerDataValueSelection)"

	if (fileInfoPtr->numberBits != numberBits)
		changedFlag = TRUE;
	fileInfoPtr->numberBits = numberBits;

			// Number of bytes per value.

	if (fileInfoPtr->numberBytes != bytesPerDataValue)
		{
		changedFlag = TRUE;
		reloadChannelDescriptionsFlag = TRUE;

		}	// end "if (...->numberBytes != bytesPerDataValue)"
	
	fileInfoPtr->numberBytes = (UInt16)bytesPerDataValue;

			// Integer or real data type.

	if (fileInfoPtr->dataTypeCode != dataTypeCode)
		{
		changedFlag = TRUE;
		reloadChannelDescriptionsFlag = TRUE;

		}	// end "if (...->dataTypeCode != dataTypeCode)"
	
	fileInfoPtr->dataTypeCode = dataTypeCode;
	/*
			// Get the number of bins.	  Now done in IntermediateFileUpdate
			
	fileInfoPtr->numberBins = 0;
				
	if (fileInfoPtr->numberBits  > 0 && fileInfoPtr->numberBits <= 16)
		fileInfoPtr->numberBins = (UInt32)ldexp ((double)1, fileInfoPtr->numberBits);
	else if (fileInfoPtr->numberBits > 16)
		fileInfoPtr->numberBins = 2048;
	*/
	//	if (numberBits > 0 && numberBits <= 32)
	//		fileInfoPtr->numberBins =
	//							(UInt32)ldexp ((double)1, numberBits);

			// Swap bytes (yes or no)

	if (fileInfoPtr->signedDataFlag != signedDataFlag)
		{
		changedFlag = TRUE;
		fileInfoPtr->signedDataFlag = signedDataFlag;

		}	// end "if (fileInfoPtr->swapBytesFlag != ..."

	#if defined multispec_mac
		//offset = 4;
		offset = 0;
		lineColumnUnitsMenuItem = kLineColumnUnitsMenuItem;
	#endif 	// defined multispec_mac

	#if defined multispec_win || defined multispec_lin
		offset = 0;
		lineColumnUnitsMenuItem = kLineColumnUnitsMenuItem - 1;
	#endif 	// defined multispec_win

	if (fileInfoPtr->format == kHDF4Type || fileInfoPtr->format == kNETCDFType ||
			fileInfoPtr->format == kHDF5Type || fileInfoPtr->format == kNETCDF2Type ||
				fileInfoPtr->format == kHDF4Type2 || fileInfoPtr->format == kNITFType)
		{
		if (// (gProcessorCode == kOpenImageFileProcessor)||
                (fileInfoPtr->hdfDataSetSelection != hdfDataSetSelection - offset))
			{
					// This indicates that a different hdf data set is being
					// used. Force a new set of statistics to be found.

			changedFlag = TRUE;
			callUpdateLayerInfoStructureFlag = TRUE;

			SetHDFDataSetFileInformation (dialogPtr,
													  fileInfoPtr,
													  hdfDataSetSelection,
													  hdfDataSetIndex);

			if (windowInfoPtr != NULL)
				{
				SignedByte supportHandleStatus;

				CMFileStream* supportFileStreamPtr = GetSupportFileStreamPointer (
															windowInfoHandle, &supportHandleStatus);
				CloseFile (supportFileStreamPtr);
				SetFileDoesNotExist (supportFileStreamPtr, kDoNotKeepUTF8CharName);
				UnlockSupportFileStream (windowInfoHandle, supportHandleStatus);

						// Force a new name for the image window

				GetImageWindowName (NULL,
											fileInfoPtr,
											gTextString,
											FALSE);

				#if defined multispec_mac
					SetWTitle (windowInfoPtr->windowPtr, (ConstStr255Param)gTextString);

							// Update the image window name in the Window menu.				

					SetMenuItemText (gMultiSpecMenus[kWindowM],
												windowInfoPtr->windowMenuItem,
												gTextString);
				#endif	// defined multispec_mac    

				#if defined multispec_win
					if (gActiveImageViewCPtr != NULL)
						MSetWindowTitle ((CDocument*)gActiveImageViewCPtr->GetDocument (),
													gTextString);
				#endif	// defined multispec_win	

				#if defined multispec_lin
					if (gActiveImageViewCPtr != NULL)
						MSetWindowTitle (gActiveImageViewCPtr,&gTextString[0]);
				#endif

						// Also remove any vector and image overlays. They will need to be
						// reloaded.

				windowInfoPtr->numberImageOverlays = 0;
				windowInfoPtr->numberOverlays = 0;

				}	// end "if (windowInfoPtr != NULL)"

					// The map information may have changed with this new data set.
					// Load the new map information.

			UnlockAndDispose (fileInfoPtr->mapProjectionHandle);
			fileInfoPtr->mapProjectionHandle = *newMapProjectionHandlePtr;
			*newMapProjectionHandlePtr = NULL;

					// The map information change may affect the setting in the coordinate
					// view. If current setting is for decimal degrees or map units but
					// there is no map information, force units to be line-column.

			latLongPossibleFlag = DetermineIfLatLongPossible (windowInfoHandle);
			coordinateViewUnites = GetCoordinateViewUnits (windowInfoHandle);

			if (!latLongPossibleFlag &&
										coordinateViewUnites >= kDecimalLatLongUnitsMenuItem)
				{		
				SetCoordinateViewUnits (windowInfoHandle, kLineColumnUnitsMenuItem);
				SetControlValue (windowInfoPtr->coordinateUnitsControl,
										lineColumnUnitsMenuItem);
				SetCoordinateViewAreaUnits (windowInfoHandle, kNumberPixelsUnitsMenuItem);
				SetCoordinateViewLocationParameters (windowInfoHandle);

				}	// end "if (!latLongPossibleFlag && coordinateViewUnites >= ..."

					// Any heirarchal information may have changed.

			UnlockAndDispose (fileInfoPtr->hfaHandle);
			fileInfoPtr->hfaHandle = *newHfaHandlePtr;
			*newHfaHandlePtr = NULL;

					// The channel to data set information may have changed.  This is
					// used for hdf files with compressed data.
			
			if (fileInfoPtr->channelToHdfDataSetHandle !=
																*newChannelToHdfDataSetHandlePtr)
				UnlockAndDispose (fileInfoPtr->channelToHdfDataSetHandle);
			fileInfoPtr->channelToHdfDataSetHandle = *newChannelToHdfDataSetHandlePtr;
			*newChannelToHdfDataSetHandlePtr = NULL;
			
					// The channel description information may have changed.
					
			fileInfoPtr->descriptionsFlag = FALSE;
			fileInfoPtr->descriptionCode = 0;

			fileInfoPtr->dataCompressionCode = dataCompressionCode;
			fileInfoPtr->gdalDataTypeCode = gdalDataTypeCode;

			}	// end "if (fileInfoPtr->hdfDataSetSelection != ..."

		else if (fileInfoPtr->hfaHandle == NULL && *newHfaHandlePtr != NULL)
			{
					// This is the first time through and the first data set is selected.
					// Make sure that the hfaHandle is saved.

			fileInfoPtr->hfaHandle = *newHfaHandlePtr;
			*newHfaHandlePtr = NULL;

			}	// end "else if (fileInfoPtr->hfaHandle == NULL && ..."

		fileInfoPtr->callGetHDFLineFlag = callGetHDFLineFlag;

		}	// end "if ((fileInfoPtr->format == kHDF4Type || ...->format == kNETCDFType)"

	if (fileInfoPtr->treatLinesAsBottomToTopFlag != linesBottomToTopFlag)
		{
		changedFlag = TRUE;
		fileInfoPtr->treatLinesAsBottomToTopFlag = linesBottomToTopFlag;

		}	// end "if (...->treatLinesAsBottomToTopFlag != linesBottomToTopFlag)"

	if (fileInfoPtr->noDataValueFlag != fillDataValueExistsFlag ||
            fileInfoPtr->noDataValue != fillDataValue)
		{
		//changedFlag = TRUE;  // Do not need to force a redraw of the image window
		fileInfoPtr->noDataValueFlag = fillDataValueExistsFlag;
		fileInfoPtr->noDataValue = fillDataValue;

		}	// end "if (fileInfoPtr->noDataValueFlag != fillDataValueExistsFlag || ..."

	if (windowInfoPtr != NULL)
		{
		#if defined multispec_mac || defined multispec_mac_swift
			ClearSelectionArea ((WindowPtr)windowInfoPtr->windowPtr);
		#else
			ClearSelectionArea (gActiveImageViewCPtr);
		#endif
		
		}	// end "if (windowInfoPtr != NULL)"
       
	IntermediateFileUpdate (fileInfoPtr);

	if (reloadChannelDescriptionsFlag)
		{
		ReadChannelDescriptionsAndValues (fileInfoHandle);

		if (windowInfoHandle != NULL)
			{
			windowInfoPtr->descriptionCode = 0;
			if (fileInfoPtr->descriptionsFlag)
				windowInfoPtr->descriptionCode |= fileInfoPtr->descriptionCode;

			}	// end "if (windowInfoHandle != NULL)"

		}	// end "if (reloadChannelDescriptionsFlag)"

			// Now update the window information structure if this
			// routine was called from the Change Image Description menu
			// item or the Reformat Change Header.	

	if (windowInfoPtr != NULL &&
            (gProcessorCode == kChangeImageDescriptionProcessor ||
            gProcessorCode == kRefChangeHeaderProcessor ||
            callUpdateLayerInfoStructureFlag))
		{
		windowInfoPtr->numberImageFiles = 0;
		windowInfoPtr->totalNumberChannels = 0;
		UpdateLayerInfoStructure (windowInfoPtr, fileInfoHandle);

		}	// end "if (windowInfoPtr != NULL && ..." 

			// Check to see if histogram and display specification  	
			// handles exists.  If so, dispose of the handles and 	
			// set them to NULL. This will force computation of 		
			// histogram and the information data to display level  	
			// vector since they are now invalid.						

	if (changedFlag && windowInfoHandle != NULL)
		{
		DisposeOfImageWindowSupportMemory (windowInfoPtr);
		#if defined multispec_mac || defined multispec_mac_swift
			InvalidateWindow ((WindowPtr)windowInfoPtr->windowPtr, kFrameArea, FALSE);
		#else
			InvalidateWindow (gActiveImageViewCPtr, kFrameArea, FALSE);
		#endif

		}	// end "if (changedFlag && windowInfoHandle != NULL)"

	if (changedFlag)
		{
		fileInfoPtr->classesComputedFlag = FALSE;

				// The parameters have changed. Cannot deal with changing
				// tile parameters for now, so set the blocked to false
				// to indicate that the data are not in a tiled.
				// I am commenting the statement below out on 11/13/2002
				// so that it does not cause problems later. If blockedFlag
				// needs to be set to false here, then the block height, and
				// band interleave settings need to be changed.

		//fileInfoPtr->blockedFlag = FALSE;

		if (windowInfoHandle != NULL)
			windowInfoPtr->fileInfoVersion++;

		}	// end "if (changedFlag)"

	if (fileInfoPtr->thematicType && computeNumberClassesFlag)
		{
		//printf ("ready to call GetClassesFromHistogram\n");
		if (GetClassesFromHistogram (fileInfoPtr, fileInfoPtr->collapseClassSelection))
			{
			Handle displaySpecsHandle = GetDisplaySpecsHandle (windowInfoHandle);

			DisplaySpecsPtr displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
																						displaySpecsHandle);

			if (displaySpecsPtr != NULL)
				displaySpecsPtr->firstTime = TRUE;

			forceGroupTableUpdateFlag = TRUE;

			}	// end "if (GetClassesFromHistogram (fileInfoPtr))"

		}	// end "if (fileInfoPtr->thematicType && computeNumberClassesFlag)"

	if (fileInfoPtr->thematicType && forceGroupTableUpdateFlag)
		{
				// The number of classes was determined by histogramming the
				// image. A new class symbol table may have been set up. The
				// current group table may not be valid, force a new one to
				// be created. Note that the new set of class names has
				// been read or created.

		fileInfoPtr->groupTablesHandle =
											UnlockAndDispose (fileInfoPtr->groupTablesHandle);
		fileInfoPtr->groupNameHandle = UnlockAndDispose (fileInfoPtr->groupNameHandle);
		fileInfoPtr->numberGroups = 0;

		}	// end "if (forceGroupTableUpdateFlag)"

	return (changedFlag);

}	// end "FileSpecificationDialogOK"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 FileSpecificationDialogSetHDFValues
//
//	Software purpose:	The purpose of this routine is to hide and show the items in
//							the file specification dialog box that depend on the band
//							interleave setting.
//
//	Parameters in:		
//
//	Parameters out:	
//
//	Value Returned:	
//
// Called By:			FileSpecificationDialog in openImageRoutines.c
//
//	Coded By:			Larry L. Biehl			Date: 11/29/2001
//	Revised By:			Larry L. Biehl			Date: 03/17/2014

SInt16 FileSpecificationDialogSetHDFValues (
				DialogPtr							dialogPtr,
				FileInfoPtr							fileInfoPtr,
				SInt16								hdfDataSetSelection,
				Handle								okHandle,
				Boolean								shiftKeyDownFlag,
				Handle*								mapInformationHandlePtr,
				Handle*								hfaHandlePtr,
				Handle*								channelToHdfDataSetHandlePtr,
				SInt16*								bandInterleaveSelectionPtr,
				SInt16*								dataValueTypeSelectionPtr,
				UInt16*								dataCompressionCodePtr,
				SInt16*								gdalDataTypeCodePtr,
				Boolean*								callGetHDFLineFlagPtr)
{
	MFileInfo hdfFileInfo;

	CMFileStream*						hdfFileStreamPtr;
	FileInfoPtr							hdfFileInfoPtr;
	HdfDataSets*						hdfDataSetsPtr;

	UInt32								dataSetIndex;

	SInt16								returnCode;

	Boolean								useGroupedDataSetsFlag;

	#if defined multispec_win
		CComboBox*							comboBoxPtr;
	#endif	// defined multispec_win
	#if defined multispec_lin
		wxComboBox*							comboBoxPtr;
	#endif


	#if defined multispec_win
		USES_CONVERSION;
	#endif

	returnCode = 1;

	InitializeFileInfoStructure ((Handle)&hdfFileInfo, kPointer);
	hdfFileInfoPtr = &hdfFileInfo;

		 // Load the handle to the map information structure in the file information
		 // structure in case in has already been set up.  The same is true for the
		 // instrument code. 

	hdfFileInfoPtr->mapProjectionHandle = *mapInformationHandlePtr;
	hdfFileInfoPtr->instrumentCode = fileInfoPtr->instrumentCode;
	hdfFileInfoPtr->channelToHdfDataSetHandle = *channelToHdfDataSetHandlePtr;
	hdfFileInfoPtr->format = fileInfoPtr->format;

		 // Now update the new file stream pointer so that it reflects the
		 // same file and directory as that in the current file information
		 // structure. However, indicate that the file is closed.

	hdfFileStreamPtr = GetFileStreamPointer (&hdfFileInfo);
	CopyFileStream (hdfFileStreamPtr, GetFileStreamPointer (fileInfoPtr));
	IndicateFileClosed (hdfFileStreamPtr);

	hdfDataSetsPtr = (HdfDataSets*)GetHandlePointer (fileInfoPtr->hdfHandle);

	if (hdfDataSetsPtr != NULL)
		{
		hdfFileInfo.thematicType = FALSE;
		hdfFileInfo.numberHdfDataSets = fileInfoPtr->numberHdfDataSets;

		#if defined multispec_mac
			GetMenuItemRefCon (gPopUpTemporaryMenu, hdfDataSetSelection, &dataSetIndex);
		#endif	// defined multispec_mac

		#if defined multispec_win
			comboBoxPtr = (CComboBox*)dialogPtr->GetDlgItem (IDC_HDFDataSet);
			dataSetIndex = (UInt32)comboBoxPtr->GetItemData (hdfDataSetSelection);
		#endif	// defined multispec_win

		#if defined multispec_lin
			comboBoxPtr = (wxComboBox*)dialogPtr->FindWindow (IDC_HDFDataSet);
			SInt64 mclientdata = (SInt64)((int*)comboBoxPtr->
																GetClientData (hdfDataSetSelection));
			dataSetIndex = (UInt32)mclientdata;
		#endif

				// Set the name for the hdf header file.

		SetCFileName (&hdfFileInfo, (FileStringPtr)&hdfDataSetsPtr[0].name[1]);

				// Check if the shift key is down. If so then the selected hdf data sets
				// will not be grouped with others.

		useGroupedDataSetsFlag = !shiftKeyDownFlag;
		returnCode = LoadSelectedDataSetInformation (fileInfoPtr,
																	 &hdfFileInfo,
																	 dataSetIndex,
																	 useGroupedDataSetsFlag);

		if (returnCode == noErr || returnCode == 20 || returnCode == 2)
			{
					// Number of image lines.

			LoadDItemValue (dialogPtr, IDC_NumberLines, hdfFileInfo.numberLines);

					// Number of image columns

			LoadDItemValue (dialogPtr, IDC_NumberColumns, hdfFileInfo.numberColumns);

					// Number of image channels, multispectral format.
					// Number of classes, Thematic format.

			if (!fileInfoPtr->thematicType)
				LoadDItemValue (dialogPtr, IDC_NumberChannels, hdfFileInfo.numberChannels);

			else	// fileInfoPtr->thematicType 
				LoadDItemValue (dialogPtr, IDC_NumberChannels, hdfFileInfo.numberChannels);

					// Start line of image

			LoadDItemValue (dialogPtr, IDC_StartLineNumber, hdfFileInfo.startLine);

					// Start column of image

			LoadDItemValue (dialogPtr, IDC_StartColumnNumber, hdfFileInfo.startColumn);

					// Number of file header bytes

			LoadDItemValue (dialogPtr, IDC_HeaderBytes, hdfFileInfo.numberHeaderBytes);

					// Number of file trailer bytes

			LoadDItemValue (dialogPtr, IDC_TrailerBytes, hdfFileInfo.numberTrailerBytes);

					// Number of preline calibration bytes

			LoadDItemValue (dialogPtr, IDC_PreLineBytes, hdfFileInfo.numberPreLineBytes);

					// Number of postline calibration bytes.

			LoadDItemValue (dialogPtr, IDC_PostLineBytes, hdfFileInfo.numberPostLineBytes);

					// Number of preline calibration bytes

			LoadDItemValue (
						dialogPtr, IDC_PreChannelBytes, hdfFileInfo.numberPreChannelBytes);

					// Number of postline calibration bytes.

			LoadDItemValue (
						dialogPtr, IDC_PostChannelBytes, hdfFileInfo.numberPostChannelBytes);

					// Set band interleave popup.

			*bandInterleaveSelectionPtr = hdfFileInfo.bandInterleave;

					// Set swap bytes check box.

			SetDLogControl (dialogPtr, IDC_SwapBytes, (SInt16)hdfFileInfo.swapBytesFlag);

					// Set signed data check box.

			SetDLogControl (dialogPtr, IDC_SignedData, (SInt16)hdfFileInfo.signedDataFlag);

					// Set data type.

			*dataValueTypeSelectionPtr = FileSpecificationDialogSetDataType (
																		  hdfFileInfo.dataTypeCode,
																		  hdfFileInfo.numberBits,
																		  hdfFileInfo.signedDataFlag);

					// Set swap bytes check box.

			if (hdfFileInfo.numberBytes >= 2)
				SetDLogControlHilite (dialogPtr, IDC_SwapBytes, 0);

			else	// hdfFileInfo.numberBytes < 2 
				SetDLogControlHilite (dialogPtr, IDC_SwapBytes, 255);

					// Treat lines as bottom to top flag.

			SetDLogControl (dialogPtr,
									IDC_LinesBottomToTop,
									(SInt16)hdfFileInfo.treatLinesAsBottomToTopFlag);

					// Set No Data (or Fill Data)information.

			SetDLogControl (dialogPtr,
									IDC_FillDataValueExists,
									(SInt16)hdfFileInfo.noDataValueFlag);
			LoadDItemRealValue (dialogPtr, IDC_FillDataValue, hdfFileInfo.noDataValue, 0);
			ShowHideDialogItem (dialogPtr,
										IDC_FillDataValue,
										(Boolean)hdfFileInfo.noDataValueFlag);

			*callGetHDFLineFlagPtr = hdfFileInfo.callGetHDFLineFlag;

			}	// end "if (returnCode == noErr || ..."

		if (returnCode != noErr)
			{
			HiliteControl ((ControlHandle)okHandle, 255);
			DisplayHDFAlerts (returnCode, &hdfFileInfo);
			HiliteControl ((ControlHandle)okHandle, 0);

			}	// end "if (returnCode != noErr)"

		}	// end "if (hdfDataSetsPtr != NULL)"

	CloseFile (hdfFileStreamPtr);

	#if defined multispec_win || defined multispec_lin
		if (hdfFileStreamPtr != NULL)
			delete hdfFileStreamPtr;
	#endif	// defined multispec_win

			// Save some variables in case they are needed for the new data set
			// selection.

	*dataCompressionCodePtr = hdfFileInfo.dataCompressionCode;
	*gdalDataTypeCodePtr = hdfFileInfo.gdalDataTypeCode;

			// Save some structures in case the information needs to
			// be used for the new data set selection.

	*mapInformationHandlePtr = hdfFileInfoPtr->mapProjectionHandle;
	*hfaHandlePtr = hdfFileInfoPtr->hfaHandle;
	*channelToHdfDataSetHandlePtr = hdfFileInfoPtr->channelToHdfDataSetHandle;

	return (returnCode);

}	// end "FileSpecificationDialogSetHDFValues"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void FileSpecificationDialogSetInterleaveItems
//
//	Software purpose:	The purpose of this routine is to hide and show the items in
//							the file specification dialog box that depend on the band
//							interleave setting.
//
//	Parameters in:		
//
//	Parameters out:	
//
//	Value Returned:	
//
// Called By:			FileSpecificationDialog in openImageRoutines.c
//
//	Coded By:			Larry L. Biehl			Date: 10/27/1999
//	Revised By:			Larry L. Biehl			Date: 01/31/2003

void FileSpecificationDialogSetInterleaveItems (
				DialogPtr							dialogPtr,
				SInt16								bandInterleaveSelection,
				Boolean								blockedFlag)
{
	switch (bandInterleaveSelection)
		{
		case kBIL:
		case kBSQ:
			ShowDialogItem (dialogPtr, IDC_HeaderPrompt);
			ShowDialogItem (dialogPtr, IDC_HeaderBytes);
			ShowDialogItem (dialogPtr, IDC_TrailerPrompt);
			ShowDialogItem (dialogPtr, IDC_TrailerBytes);
			ShowDialogItem (dialogPtr, IDC_PreLinePrompt);
			ShowDialogItem (dialogPtr, IDC_PreLineBytes);
			ShowDialogItem (dialogPtr, IDC_PostLinePrompt);
			ShowDialogItem (dialogPtr, IDC_PostLineBytes);
			ShowDialogItem (dialogPtr, IDC_PreChannelPrompt);
			ShowDialogItem (dialogPtr, IDC_PreChannelBytes);
			ShowDialogItem (dialogPtr, IDC_PostChannelPrompt);
			ShowDialogItem (dialogPtr, IDC_PostChannelBytes);
			HideDialogItem (dialogPtr, IDC_BlockHeightPrompt);
			HideDialogItem (dialogPtr, IDC_BlockHeight);
			HideDialogItem (dialogPtr, IDC_BlockWidthPrompt);
			HideDialogItem (dialogPtr, IDC_BlockWidth);
			break;

		case kBIS:
			ShowDialogItem (dialogPtr, IDC_HeaderPrompt);
			ShowDialogItem (dialogPtr, IDC_HeaderBytes);

			if (blockedFlag)
				{
				HideDialogItem (dialogPtr, IDC_TrailerPrompt);
				HideDialogItem (dialogPtr, IDC_TrailerBytes);
				HideDialogItem (dialogPtr, IDC_PreLinePrompt);
				HideDialogItem (dialogPtr, IDC_PreLineBytes);
				HideDialogItem (dialogPtr, IDC_PostLinePrompt);
				HideDialogItem (dialogPtr, IDC_PostLineBytes);
				HideDialogItem (dialogPtr, IDC_PreChannelPrompt);
				HideDialogItem (dialogPtr, IDC_PreChannelBytes);
				HideDialogItem (dialogPtr, IDC_PostChannelPrompt);
				HideDialogItem (dialogPtr, IDC_PostChannelBytes);
				ShowDialogItem (dialogPtr, IDC_BlockHeightPrompt);
				ShowDialogItem (dialogPtr, IDC_BlockHeight);
				ShowDialogItem (dialogPtr, IDC_BlockWidthPrompt);
				ShowDialogItem (dialogPtr, IDC_BlockWidth);

            }	// end "if (blockedFlag)"

			else	// !blockedFlag
            {
				ShowDialogItem (dialogPtr, IDC_TrailerPrompt);
				ShowDialogItem (dialogPtr, IDC_TrailerBytes);
				ShowDialogItem (dialogPtr, IDC_PreLinePrompt);
				ShowDialogItem (dialogPtr, IDC_PreLineBytes);
				ShowDialogItem (dialogPtr, IDC_PostLinePrompt);
				ShowDialogItem (dialogPtr, IDC_PostLineBytes);
				HideDialogItem (dialogPtr, IDC_PreChannelPrompt);
				HideDialogItem (dialogPtr, IDC_PreChannelBytes);
				HideDialogItem (dialogPtr, IDC_PostChannelPrompt);
				HideDialogItem (dialogPtr, IDC_PostChannelBytes);
				HideDialogItem (dialogPtr, IDC_BlockHeightPrompt);
				HideDialogItem (dialogPtr, IDC_BlockHeight);
				HideDialogItem (dialogPtr, IDC_BlockWidthPrompt);
				HideDialogItem (dialogPtr, IDC_BlockWidth);

            }	// else !blockedFlag
			break;

		case kBNonSQ:
			HideDialogItem (dialogPtr, IDC_HeaderPrompt);
			HideDialogItem (dialogPtr, IDC_HeaderBytes);
			HideDialogItem (dialogPtr, IDC_TrailerPrompt);
			HideDialogItem (dialogPtr, IDC_TrailerBytes);
			ShowDialogItem (dialogPtr, IDC_PreLinePrompt);
			ShowDialogItem (dialogPtr, IDC_PreLineBytes);
			ShowDialogItem (dialogPtr, IDC_PostLinePrompt);
			ShowDialogItem (dialogPtr, IDC_PostLineBytes);
			ShowDialogItem (dialogPtr, IDC_PreChannelPrompt);
			ShowDialogItem (dialogPtr, IDC_PreChannelBytes);
			ShowDialogItem (dialogPtr, IDC_PostChannelPrompt);
			ShowDialogItem (dialogPtr, IDC_PostChannelBytes);
			HideDialogItem (dialogPtr, IDC_BlockHeightPrompt);
			HideDialogItem (dialogPtr, IDC_BlockHeight);
			HideDialogItem (dialogPtr, IDC_BlockWidthPrompt);
			HideDialogItem (dialogPtr, IDC_BlockWidth);
			break;

		case kBNonSQBlocked:
			HideDialogItem (dialogPtr, IDC_HeaderPrompt);
			HideDialogItem (dialogPtr, IDC_HeaderBytes);
			HideDialogItem (dialogPtr, IDC_TrailerPrompt);
			HideDialogItem (dialogPtr, IDC_TrailerBytes);
			HideDialogItem (dialogPtr, IDC_PreLinePrompt);
			HideDialogItem (dialogPtr, IDC_PreLineBytes);
			HideDialogItem (dialogPtr, IDC_PostLinePrompt);
			HideDialogItem (dialogPtr, IDC_PostLineBytes);
			HideDialogItem (dialogPtr, IDC_PreChannelPrompt);
			HideDialogItem (dialogPtr, IDC_PreChannelBytes);
			HideDialogItem (dialogPtr, IDC_PostChannelPrompt);
			HideDialogItem (dialogPtr, IDC_PostChannelBytes);
			ShowDialogItem (dialogPtr, IDC_BlockHeightPrompt);
			ShowDialogItem (dialogPtr, IDC_BlockHeight);
			ShowDialogItem (dialogPtr, IDC_BlockWidthPrompt);
			ShowDialogItem (dialogPtr, IDC_BlockWidth);
			break;

		case kBIBlock:
			ShowDialogItem (dialogPtr, IDC_HeaderPrompt);
			ShowDialogItem (dialogPtr, IDC_HeaderBytes);
			HideDialogItem (dialogPtr, IDC_TrailerPrompt);
			HideDialogItem (dialogPtr, IDC_TrailerBytes);
			HideDialogItem (dialogPtr, IDC_PreLinePrompt);
			HideDialogItem (dialogPtr, IDC_PreLineBytes);
			HideDialogItem (dialogPtr, IDC_PostLinePrompt);
			HideDialogItem (dialogPtr, IDC_PostLineBytes);
			HideDialogItem (dialogPtr, IDC_PreChannelPrompt);
			HideDialogItem (dialogPtr, IDC_PreChannelBytes);
			HideDialogItem (dialogPtr, IDC_PostChannelPrompt);
			HideDialogItem (dialogPtr, IDC_PostChannelBytes);
			ShowDialogItem (dialogPtr, IDC_BlockHeightPrompt);
			ShowDialogItem (dialogPtr, IDC_BlockHeight);
			ShowDialogItem (dialogPtr, IDC_BlockWidthPrompt);
			ShowDialogItem (dialogPtr, IDC_BlockWidth);
			break;

		}	// end "switch (bandInterleaveSelection)"

}	// end "FileSpecificationDialogSetInterleaveItems"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void FileSpecificationDialogGetNumberBytes
//
//	Software purpose:	The purpose of this routine is to determine the number of bytes
//							based on the data type popup menu selection.
//
//	Parameters in:		
//
//	Parameters out:	
//
//	Value Returned:	
//
// Called By:			FileSpecificationDialog in openImageRoutines.c
//
//	Coded By:			Larry L. Biehl			Date: 12/12/2003
//	Revised By:			Larry L. Biehl			Date: 04/07/2006

SInt16 FileSpecificationDialogGetNumberBytes (
				SInt16								dataTypeSelection)
{
	SInt16								numberBytes;


	switch (dataTypeSelection)
		{
		case k4BitUnsignedIntegerMenuItem:
		case k1ByteSignedIntegerMenuItem:
		case k1ByteUnsignedIntegerMenuItem:
		default:
			numberBytes = 1;
			break;

		case k2ByteSignedIntegerMenuItem:
		case k2ByteUnsignedIntegerMenuItem:
			numberBytes = 2;
			break;

		case k4ByteSignedIntegerMenuItem:
		case k4ByteUnsignedIntegerMenuItem:
		case k4ByteRealMenuItem:
			numberBytes = 4;
			break;

		case k8ByteRealMenuItem:
			numberBytes = 8;
			break;

		}	// end "switch (dataTypeSelection)"

	return (numberBytes);

}	// end "FileSpecificationDialogGetNumberBytes"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void FileSpecificationDialogSetDataType
//
//	Software purpose:	The purpose of this routine is to determine the setting
//							for the data type popup menu.
//
//	Parameters in:		
//
//	Parameters out:	
//
//	Value Returned:	
//
// Called By:			FileSpecificationDialog in openImageRoutines.c
//
//	Coded By:			Larry L. Biehl			Date: 07/18/2003
//	Revised By:			Larry L. Biehl			Date: 04/26/2013

SInt16 FileSpecificationDialogSetDataType (
				SInt16								dataTypeCode,
				SInt16								numberBits,
				Boolean								signedDataFlag)
{
	SInt16								dataTypeSelection;


	dataTypeSelection = 0;
	if (numberBits == 4)
		dataTypeSelection = k4BitUnsignedIntegerMenuItem;

	if (numberBits == 8)
		{
		if (signedDataFlag)
			dataTypeSelection = k1ByteSignedIntegerMenuItem;

		else	// !signedDataFlag
			dataTypeSelection = k1ByteUnsignedIntegerMenuItem;

		}	// end "if (numberBits == 8)"

	else if (numberBits > 8 && numberBits <= 16)
		{
		if (signedDataFlag)
			dataTypeSelection = k2ByteSignedIntegerMenuItem;

		else	// !signedDataFlag
			dataTypeSelection = k2ByteUnsignedIntegerMenuItem;

		}	// end "else if (numberBits == 16)"

	else if (numberBits == 32)
		{
		if (dataTypeCode == kIntegerType)
			{
			if (signedDataFlag)
				dataTypeSelection = k4ByteSignedIntegerMenuItem;

			else	// !signedDataFlag
				dataTypeSelection = k4ByteUnsignedIntegerMenuItem;

			}	// end "if (dataTypeCode == kIntegerType)"

		else	// dataTypeCode == kRealType
			dataTypeSelection = k4ByteRealMenuItem;

		}	// end "else if (numberBits == 32)"

	else if (numberBits == 64)
		dataTypeSelection = k8ByteRealMenuItem;

	else	// if not equal to any, assume the number of bits is 8-bit unsigned.
		dataTypeSelection = k1ByteUnsignedIntegerMenuItem;

	return (dataTypeSelection);

}	// end "FileSpecificationDialogSetDataType"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 LinkFiles
//
//	Software purpose:	The purpose of this routine is to link the image files in
//							the input image set.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	
// 
// Called By:			AddSelectedFilesToWindow in SOpenDlg.cpp
//							OpenSeparateImageWindows in SOpenDlg.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/07/2007
//	Revised By:			Larry L. Biehl			Date: 01/15/2013	

SInt16 LinkFiles (
				SInt16								fileFormat,
				Handle								windowInfoHandle)
{
	SInt16								returnCode = 1;


	if (fileFormat == kFastL7AType)
		returnCode = LinkFastL7AFiles (windowInfoHandle);

	else if (fileFormat == kNdfType)
		returnCode = LinkNDFFiles (windowInfoHandle);

	return (returnCode);

}	// end "LinkFiles"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 LinkSelectedFilesToNewWindow
//
//	Software purpose:	The purpose of this routine is to link the image files in
//							the input file list to a new image window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/15/2013
//	Revised By:			Larry L. Biehl			Date: 03/09/2013	

SInt16 LinkSelectedFilesToNewWindow (
				Handle								fileInfoHandle,
				UInt32								itemCount,
				FSRef*								fileAsFSRefPtr)
{
	Handle								windowInfoHandle;

	UInt32								fileNumber,
											formatVersionCode;

	SInt16								errCode,
											fileFormat,
											instrumentCode;

	Boolean								continueFlag,
											doneFlag,
											fileInfoLoadedFlag,
											setUpFileInfoStructureFlag;


	errCode = noErr;
	continueFlag = FALSE;
	instrumentCode = 0;
	windowInfoHandle = NULL;
	
	if (fileInfoHandle != NULL)
		{
		setUpFileInfoStructureFlag = CheckForInstrumentFileList (itemCount,
																				fileAsFSRefPtr,
																				&instrumentCode);

		fileNumber = 0;
		doneFlag = FALSE;

		while (!doneFlag)
			{
			continueFlag = FALSE;

			if (setUpFileInfoStructureFlag)
				{
						// The file information structure for the first one in the list
						// was already set up in OpenImageFile.

				fileInfoLoadedFlag = FALSE;

						// Initialize the variables and handles in the structure.		

				InitializeFileInfoStructure (fileInfoHandle, kNotPointer);

				errCode = OpenSpecifiedFile (fileInfoHandle, fileAsFSRefPtr[fileNumber]);

				}	// end "if (setUpFileInfoStructureFlag)"

					// Set the instrument code in case it was set.

			SetFileInstrumentCode (fileInfoHandle, instrumentCode);

					// Determine the file format and load the header information.													

			if (errCode == noErr)
				fileInfoLoadedFlag = LoadImageInformation (windowInfoHandle,
																			fileInfoHandle,
																			FALSE,
																			&formatVersionCode,
																			0);

			fileFormat = GetFileFormat (fileInfoHandle);
			if (fileInfoLoadedFlag && fileFormat != kArcViewShapeType)
				{
						// The first file for the window has been found. Create the structures
						// for the new window.

				#if defined multispec_mac
					windowInfoHandle = GetWindowInfoStructures (fileInfoHandle,
																				gGetFileImageType,
																				gGetFileImageType);
				#endif	// defined multispec_mac

				#if defined multispec_win
					windowInfoHandle = ((CMultiSpecApp*)AfxGetApp ())->SetUpNewImageDocument (
																				fileInfoHandle,
																				gGetFileImageType,
																				gGetFileImageType);
				#endif	// defined multispec_win

				#if defined multispec_lin
					windowInfoHandle = ((CMultiSpecApp*)wxTheApp)->SetUpNewImageDocument (
																				fileInfoHandle,
																				gGetFileImageType,
																				gGetFileImageType);
				#endif	// defined multispec_lin

						// The file information structure is now associated with the
						// new window information structure. Get a new structure to use to
						// for the files to be added to this window.

				fileInfoHandle = MNewHandle (sizeof (MFileInfo));

						// Now add the rest of the files to the window.

				//MSetCursor (kWait);
				gPresentCursor = kSpin;
				fileNumber++;
				continueFlag = AddSelectedFilesToWindow (windowInfoHandle,
																		fileInfoHandle,
																		itemCount - fileNumber,
																		&fileAsFSRefPtr[fileNumber],
																		instrumentCode,
																		kFirstFileStreamNotLoaded);
				MInitCursor ();

				fileInfoHandle = UnlockAndDispose (fileInfoHandle);

				if (continueFlag)
					{
					if (gGetFileImageType == kMultispectralImageType)
						continueFlag = SetUpImageWindow (windowInfoHandle);

					else if (gGetFileImageType == kThematicImageType)
						continueFlag = SetUpThematicImageWindow (windowInfoHandle);

					//gMultiSpecWorkflowInfo.workFlowCode = 2;

					}	// end "if (continueFlag)"

				if (!continueFlag)
					{
					Handle layerInfoHandle = GetLayerInfoHandle (windowInfoHandle);
					UnlockAndDispose (layerInfoHandle);

					windowInfoHandle = UnlockAndDispose (windowInfoHandle);

					}	// end "if (!continueFlag)"

				doneFlag = TRUE;

				}	// end "if (fileInfoLoadedFlag && fileFormat != kArcViewShapeType)"

			else	// !fileInfoLoadedFlag || fileFormat == kArcViewShapeType
				{
						// Indicate that processing of the file list can continue. This
						// particular file in the list is being ignored.

				continueFlag = TRUE;

						// List message in output text window.

				if (fileFormat != kArcViewShapeType)
					ListFileIgnoredMessage (fileInfoHandle, 1);

				}	// end "else !fileInfoLoadedFlag || fileFormat == kArcViewShapeType"

			fileNumber++;
			setUpFileInfoStructureFlag = TRUE;
			if (fileNumber >= itemCount || !continueFlag)
				doneFlag = TRUE;

			}	// end "while (!doneFlag)" 

		}	// end "if (fileInfoHandle != NULL)"

			// The handles within the file information structure are now being used in
			// another file info structure for the window. We do not want to clear them out.

	CheckAndUnlockHandle (fileInfoHandle);

	return (continueFlag);

}	// end "LinkSelectedFilesToNewWindow"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ListFileIgnoredMessage
//
//	Software purpose:	The purpose of this routine is to list a message in the output
//							text window indicating that the file is being ignored.
//
//	Parameters in:		
//
//	Parameters out:	None
//
//	Value Returned:				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/25/2013
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

void ListFileIgnoredMessage (
				Handle								fileInfoHandle,
				SInt16								messageCode)
{
	char*									fileNamePtr;


	fileNamePtr = (char*)GetFileNameCPointerFromFileHandle (fileInfoHandle);

	if (fileNamePtr != NULL)
		{
		if (messageCode == 1)
			sprintf ((char*)gTextString,
						 "    The file '%s' could not be read; it will be ignored.%s",
						 fileNamePtr,
						 gEndOfLine);

		else if (messageCode == 2)
			sprintf ((char*)gTextString,
						 "    The file '%s' has a different number of lines or columns than"
						 " first image in the list; it will be ignored.%s",
						 fileNamePtr,
						 gEndOfLine);

		ListString ((char*)gTextString,
							(UInt32)strlen ((char*)gTextString),
							gOutputTextH,
							kUTF8CharString);

		}	// end "if (fileNamePtr != NULL)"

}	// end "ListFileIgnoredMessage"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean LoadSelectedDataSetInformation
//
//	Software purpose:	The purpose of this routine is to load the selected data set
//							information.
//
//	Parameters in:		
//
//	Parameters out:	None
//
//	Value Returned:				
// 
// Called By:			LoadImageInformation in SOpnImag.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/12/2012
//	Revised By:			Larry L. Biehl			Date: 07/29/2018

SInt16 LoadSelectedDataSetInformation (
				FileInfoPtr							inputFileInfoPtr,
				FileInfoPtr							outputFileInfoPtr,
				UInt32								dataSetIndex,
				Boolean								useGroupedDataSetsFlag)
{
	HdfDataSets*						hdfDataSetsPtr;
	SInt16								returnCode;


	returnCode = noErr;

	hdfDataSetsPtr = (HdfDataSets*)GetHandlePointer (inputFileInfoPtr->hdfHandle);

	#if include_hdf_capability
		if (inputFileInfoPtr->format == kHDF4Type ||
															inputFileInfoPtr->format == kNETCDFType)
			returnCode = LoadHDF4DataSetInformation (inputFileInfoPtr->hdf4FileID,
																	hdfDataSetsPtr,
																	outputFileInfoPtr,
																	inputFileInfoPtr->format,
																	(SInt32)dataSetIndex,
																	useGroupedDataSetsFlag);
	#endif		// include_hdf_capability

	#if include_gdal_capability
		#if include_hdf5_capability
			if (inputFileInfoPtr->format == kHDF4Type2 ||
						inputFileInfoPtr->format == kHDF5Type ||
								inputFileInfoPtr->format == kNETCDF2Type  ||
										inputFileInfoPtr->format == kNITFType)
				{
				outputFileInfoPtr->gdalDataSetH = inputFileInfoPtr->gdalDataSetH;
				returnCode = LoadHDF5HeaderInformation (hdfDataSetsPtr,
																		outputFileInfoPtr,
																		dataSetIndex,
																		useGroupedDataSetsFlag);
				}
		#endif		// include_hdf5_capability
	#endif		// include_gdal_capability && ...

	return (returnCode);

}	// end "LoadSelectedDataSetInformation"


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt32 OpenImageFile
//
//	Software purpose:	The purpose of this routine is to open an image file
//							either multispectral type or Thematic (classification)
//							type.  It is called from the 'File' menu.
//
//	Parameters in:		Pointer to structure giving information about the image
//								to use as the selected image file. If this pointer is
//								NULL, then present a dialog box to the use to allow
//								he/she to select the image file.
//
//	Parameters out:	None
//
//	Value Returned:	0 if selected file is not a project file
//							value > 0 indicating the project file version if the
//								selected file is not a project file.			
// 
// Called By:			GetImageFile in fileIO.c
//							OpenMultiSpecDocument in fileIO.c
//
//	Coded By:			Larry L. Biehl			Date: 12/18/1989
//	Revised By:			Larry L. Biehl			Date: 02/07/2018

SInt32 OpenImageFile (
				LocalAppFile*						localAppFilePtr,
				Boolean								fromOpenProjectImageWindowFlag,
				UInt16								hdfDataSetSelection)
{
	CMFileStream*						fileStreamPtr;

	FileInfoPtr							fileInfoPtr;

	FSRef									fileAsFSRef[500];

	Handle								fileInfoHandle,
											windowInfoHandle;

	SInt32								projectFileCode = 0;

	UInt32								itemCount;

	SInt16								errCode,
											fileFormat,
											itemHit,
											numberFileTypes = -1,
											promptString;

	Boolean								//callLinkFileFlag,
											continueFlag,
											disposeFileInfoHandleFlag,
											fileFlag,
											fileInfoLoadedFlag,
											doneFlag;


	doneFlag = FALSE;
	disposeFileInfoHandleFlag = FALSE;
	gMultipleImageFileCode = 0;

			// Make sure that we are not at the limit of the number of image
			// windows.

	if (gNumberOfIWindows >= kMaxNumberIWindows)
		{
		DisplayAlert (kErrorAlertID,
                kNoteAlert,
                kAlertStrID,
                IDS_Alert47,
                0,
                NULL);

		return (0);

		}	// end "if (gNumberOfIWindows >= kMaxNumberIWindows)"

			// Make certain that the cursor has been set for the pointer.  It		
			// is possible for the cursor to be something else if this open 		
			// command is being called directly from the finder.							

	MInitCursor ();

			// Get a new handle to a block of memory to be used for					
			// file information for the new image window.								

	fileInfoHandle = MNewHandle (sizeof (MFileInfo));
	if (fileInfoHandle != NULL)
		{
		#if defined multispec_mac
				// If the option key was down when the 'Open Image' menu item		
				// was selected then include all files in the file list.  			
				// Otherwise just show the selected file types.							

        numberFileTypes = gNumberFileTypes;
        if (gEventRecord.modifiers & optionKey)
            numberFileTypes = -1;
		#endif	// defined multispec_mac

		while (!doneFlag)
			{
					// Initialize local variables.											

			promptString = IDS_SelectImage;
			if (gMultipleImageFileCode)
				promptString = IDS_SelectFileToLink;

			fileInfoLoadedFlag = FALSE;
			continueFlag = TRUE;
			projectFileCode = 0;

					// Initialize the variables and handles in the structure.		

			InitializeFileInfoStructure (fileInfoHandle, kNotPointer);

			fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle, kLock);

			fileStreamPtr = GetFileStreamPointer (fileInfoPtr);

					// Indicate that the image type is to be determined from the 	
					// header part of the file.												

			gGetFileImageType = 0;

			errCode = GetFile (fileStreamPtr,
									  numberFileTypes,
									  gListTypes,
									  localAppFilePtr,
									  fileAsFSRef,
									  &itemCount,
									  promptString);

			gOpenImageSelectionListCount = itemCount;
			//gMultipleImageFileCode = 3;

			fileFlag = ((errCode == noErr)& FileExists (fileStreamPtr));

					// If appFilePtr is not NULL then this is a call from the		
					// initialization routine.  This current file may be a project	
					// file.  Check for project file case and return if it is.		

			if ((gGetFileStatus == 1 || localAppFilePtr)&& fileFlag)
				{					
				projectFileCode = CheckIfProjectFile (fileStreamPtr, kLoadHeader);

				if (projectFileCode > 0)
					{
							// Convert to a value that fits in a short integer.

					projectFileCode = 1;

					#if defined multispec_mac
						if (gGetFileStatus == 1)
							{
							gLocalAppFile.vRefNum = fileStreamPtr->vRefNum;
							gLocalAppFile.parID = fileStreamPtr->parID;
							CopyPToP (gAppFileName, fileStreamPtr->fileName);
							gLocalAppFile.type = fileStreamPtr->type;
							gLocalAppFile.refNum = 0;
							gLocalAppFile.fSSpecFlag = fileStreamPtr->fSSpecFlag;
							gLocalAppFile.fNamePtr = gAppFileName;

							}	// end "if (gGetFileStatus <= 1)" 
					#endif	// defined multispec_mac

					fileFlag = FALSE;
					CloseFile (fileStreamPtr);

					}	// end "if (projectFileCode > 0)"

				}	// end "if (appFilePtr != NULL && fileFlag)"
			
					// If this is from a call when a project file is being opened and the
					// the project window is being opened, make sure the correct data set
					// selection has been set in the file information structure.
					// This is needed for Linux and Windows
			
			if (fromOpenProjectImageWindowFlag)
				fileInfoPtr->hdfDataSetSelection	= hdfDataSetSelection;
			
			CheckAndUnlockHandle (fileInfoHandle);
				
			//gMultipleImageFileCode = 3;
			fileFormat = 0;
			if (fileFlag)
				{
				if (gMultipleImageFileCode == 0)
					{
							// Request is to start a new image window for each selected file.

							// Make sure that we are not at the limit of the number of image
							// windows.

					if (gNumberOfIWindows + itemCount > kMaxNumberIWindows)
						{
								// The number of image windows is more than the limit. Display
								// an alert for the user. The number in the list will be
								// adjusted to fit within the limit.

						itemHit = DisplayAlert (kContinueCancelAlertID,
														  kNoteAlert,
														  kAlertStrID,
														  IDS_Alert148,
														  0,
														  NULL);

						itemCount = kMaxNumberIWindows - gNumberOfIWindows;
						if (itemCount <= 0 || itemHit == 2)
							continueFlag = FALSE;

						}	// end "if (gNumberOfIWindows >= kMaxNumberIWindows)"
				  
							// Put up a message box indicating that the file is being read if
							// this file is from a file handler situation. Some iRods files
							// take a long time to setup, read and display.
					
					#if defined multispec_wxlin
						if (gFromToolParameterFileFlag)
							{
							//CheckSomeEvents (osMask+updateMask);
							gStatusDialogPtr = GetStatusDialog (kShortStatusInfoID, FALSE);
							if (gStatusDialogPtr != NULL)
								{
								ShowHideDialogItem (gStatusDialogPtr, IDC_ShortStatusText, TRUE);
								ShowHideDialogItem (gStatusDialogPtr, IDC_ShortStatusValue, TRUE);
								MGetString (gTextString, kAlertStrID, IDS_Alert62);
								LoadDItemString (
									gStatusDialogPtr, IDC_ShortStatusText, (Str255*)gTextString);
								LoadDItemValue (
												gStatusDialogPtr, IDC_ShortStatusValue, (SInt32)1);
								ShowStatusDialogWindow (kShortStatusInfoID);
								
								}	// end "if (gStatusDialogPtr != NULL)"
							
							MSetCursor (kWait);
							CheckSomeEvents (osMask+updateMask);
							
							}	// end "if (gFromToolParameterFileFlag)"
					#endif
					
					if (continueFlag)
						continueFlag = OpenSeparateImageWindows (fileInfoHandle,
																				 itemCount,
																				 fileAsFSRef,
																				 fileInfoLoadedFlag);
					
					if (gStatusDialogPtr != NULL)
						{
						CloseStatusDialog (TRUE);
						MInitCursor ();
						//gFromToolParameterFileFlag = FALSE;
						
						}	// end "if (gStatusDialogPtr != NULL)"
						
					doneFlag = TRUE;
					
					}	// end "if (gMultipleImageFileCode == 0)"

				else if (gMultipleImageFileCode == 2)
					{
							// Linking has been requested to the active image window.

					windowInfoHandle = gActiveImageWindowInfoH;
					continueFlag = AddSelectedFilesToWindow (windowInfoHandle,
																			 fileInfoHandle,
																			 itemCount,
																			 fileAsFSRef,
																			 0,
																			 kFirstFileStreamLoaded);

					disposeFileInfoHandleFlag = TRUE;

					}	// end "if (gMultipleImageFileCode == 2)" 

				else if (gMultipleImageFileCode == 3)
					{
							// Multiple files will be linked together. 

					continueFlag = LinkSelectedFilesToNewWindow (fileInfoHandle,
																				 itemCount,
																				 fileAsFSRef);

					doneFlag = TRUE;

					}	// end "else if (gMultipleImageFileCode == 3)"

				else	// gMultipleImageFileCode != 0, 2 or 3
				  continueFlag = FALSE;

				}	// end "if (fileFlag)"

			else	// !fileFlag
				continueFlag = FALSE;

			if (!continueFlag)//|| fileFormat == kArcViewShapeType)
				{
				DisposeFileInfoHandle (fileInfoHandle);
				fileInfoHandle = NULL;
				doneFlag = TRUE;

				}	// end "if (!continueFlag || fileFormat == kArcViewShapeType)" 

			}	// end "while (!doneFlag)"  

		if (disposeFileInfoHandleFlag)
			UnlockAndDispose (fileInfoHandle);

		gUpdateProjectMenuItemsFlag = TRUE;
		gUpdateProcessorMenuItemsFlag = TRUE;

		}	// end "if (fileInfoHandle != NULL)"

	gMultipleImageFileCode = 0;
	gLinkOptionsSelection = 1;		
	 
	return ((SInt16)projectFileCode);

}	// end "OpenImageFile"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean OpenSeparateImageWindows
//
//	Software purpose:	The purpose of this routine is to open each of the image files in
//							the input selected file set in separate image windows.							
//							This routine will handle cases where:
//								- There is just one file selected
//								- There is just one file selected but it is inherently 
//											multiple files
//								- There are multiple files selected.
//							Shape files in a list with multiple selections will be ignored.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/08/2012
//	Revised By:			Larry L. Biehl			Date: 07/05/2017

Boolean OpenSeparateImageWindows (
				Handle								fileInfoHandle,
				UInt32								itemCount,
				FSRef*								fileAsFSRefPtr,
				Boolean								fileInfoLoadedFlag)
{
	Handle								windowInfoHandle = NULL;

	UInt32								fileNumber,
											formatVersionCode;

	SInt16								errCode,
											fileFormat,
											savedGetFileImageType,
											savedProcessorCode;

	Boolean								continueFlag,
											doneFlag;


	errCode = noErr;
	continueFlag = TRUE;

	if (fileInfoHandle != NULL)
		{
		savedProcessorCode = gProcessorCode;
		savedGetFileImageType = gGetFileImageType;
		fileNumber = 0;
		doneFlag = FALSE;

		while (!doneFlag)
			{
			gProcessorCode = savedProcessorCode;
			gGetFileImageType = savedGetFileImageType;

			if (fileNumber > 0)
				{
						// The file information structure for the first one in the list
						// was already set up in OpenImageFile.

				fileInfoLoadedFlag = FALSE;

						// Initialize the variables and handles in the structure.	

				fileInfoHandle = MNewHandle (sizeof (MFileInfo));

				InitializeFileInfoStructure (fileInfoHandle, kNotPointer);

				errCode = OpenSpecifiedFile (fileInfoHandle,
														fileAsFSRefPtr[fileNumber]);

            }	// end "if (fileNumber > 0)"

					// Determine the file format and load the header information.		
					// Note that the file information structure for the first selected
					// file has already been completed.
			
			if (errCode == noErr)
				fileInfoLoadedFlag = LoadImageInformation (windowInfoHandle,
																		  fileInfoHandle,
																		  FALSE,
																		  &formatVersionCode,
																		  0);

			fileFormat = GetFileFormat (fileInfoHandle);
			if (fileInfoLoadedFlag && fileFormat != kArcViewShapeType)
				{
						// Vector types are not handled here.

				#if defined multispec_mac
                windowInfoHandle = GetWindowInfoStructures (fileInfoHandle,
																				gGetFileImageType,
																				gGetFileImageType);
				#endif	// defined multispec_mac

				#if defined multispec_win
                windowInfoHandle =
                        ((CMultiSpecApp*)AfxGetApp ())->SetUpNewImageDocument (
																						fileInfoHandle,
																						gGetFileImageType,
																						gGetFileImageType);
				#endif	// defined multispec_win

				#if defined multispec_lin
                windowInfoHandle = ((CMultiSpecApp*)wxTheApp)->SetUpNewImageDocument (
																						fileInfoHandle,
																						gGetFileImageType,
																						gGetFileImageType);
				#endif	// multispec_lin

				continueFlag = (windowInfoHandle != NULL);

				if (continueFlag && formatVersionCode == 1)
					{
					if (LinkFiles (fileFormat, windowInfoHandle) != noErr)
						continueFlag = FALSE;

					}	// end "if (continueFlag && formatVersionCode == 1)"

				if (continueFlag)
					{
							// The Workflow items here are for the U2U project to create
							// the threshold temperature files.

					//gMultiSpecWorkflowInfo.workFlowCode = 2;	

					if (gGetFileImageType == kMultispectralImageType)
						continueFlag = SetUpImageWindow (windowInfoHandle);

					else if (gGetFileImageType == kThematicImageType)
						continueFlag = SetUpThematicImageWindow (windowInfoHandle);

					//		DoWorkFlow ();
					//#endif	// defined multispec_mac	

					}	// end "if (continueFlag)" 

				if (!continueFlag)
					{
					Handle layerInfoHandle = GetLayerInfoHandle (windowInfoHandle);
					UnlockAndDispose (layerInfoHandle);

					windowInfoHandle = UnlockAndDispose (windowInfoHandle);

					}	// end "if (!continueFlag)"

				}	// end "if (fileInfoLoadedFlag && gGetFileImageType != kVectorImageType)" 

			if (!continueFlag || gGetFileImageType == kVectorImageType)
				{
				DisposeFileInfoHandle (fileInfoHandle);
				fileInfoHandle = NULL;

            }	// end "if (!continueFlag || gGetFileImageType == kVectorImageType)"

			else	// continueFlag && gGetFileImageType != kVectorImageType
                // Check clear memory taken up by the handle. The handles within the
                // structure are now being used in another file info structure for the
                // window. We do not want to clear them out.
				CheckAndUnlockHandle (fileInfoHandle);

			fileNumber++;
			if (fileNumber >= itemCount || !continueFlag)
				doneFlag = TRUE;

			}	// end "while (!doneFlag)"

		gGetFileImageType = 0;
		
				// For U2U project to create min temperature threshold data.
		//gMultiSpecWorkflowInfo.workFlowCode = 1;

		}	// end "if (fileInfoHandle != NULL)"

	return (TRUE);

}	// end "OpenSeparateImageWindows"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 OpenSpecifiedFile
//
//	Software purpose:	The purpose of this routine is to open the specified file in the
//							input file list. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			FileSpecificationDialogOK
//
//	Coded By:			Larry L. Biehl			Date: 01/15/2013
//	Revised By:			Larry L. Biehl			Date: 06/22/2017

SInt16 OpenSpecifiedFile (
				Handle								fileInfoHandle,
				FSRef									fileAsFSRef)
{
	#if defined multispec_mac
		CFStringRef							cfStringRef;
		FSCatalogInfo						fsCatalogInfo;
	#endif	// defined multispec_mac

	FileInfoPtr							fileInfoPtr;
	CMFileStream*						fileStreamPtr;

	SInt16 errCode = 1;


	fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle, kLock);

	if (fileInfoPtr != NULL)
		{
				// Load the file name for the input file
				// First get the file stream pointer for the structure to be used

		fileStreamPtr = GetFileStreamPointer (fileInfoPtr);

		#if defined multispec_lin
        FSRef* fileAsFSRefPtr = &fileAsFSRef;
        fileStreamPtr->SetFilePath ((wchar_t*)&fileAsFSRefPtr[0], TRUE);

        errCode = CMFileStream::GetFileType (
							(WideFileStringPtr)&fileAsFSRefPtr[0], &fileStreamPtr->mFileType);

					// Force the pascal file name to be updated in case user is
					// expecting to use it.

        fileStreamPtr->GetFileNamePPtr ();
		#endif	// defined multispec_lin

		#if defined multispec_mac
			fileStreamPtr->fSRefFlag = TRUE;
			fileStreamPtr->fsRef = fileAsFSRef;

			errCode = FSGetCatalogInfo (&fileStreamPtr->fsRef,
                kFSCatInfoVolume,
                &fsCatalogInfo,
                &fileStreamPtr->uniFileName,
                NULL,
                &fileStreamPtr->parentFSRef);

			if (errCode == noErr)
				{
            fileStreamPtr->vRefNum = fsCatalogInfo.volume;

            cfStringRef = CFStringCreateWithCharacters (
																  kCFAllocatorDefault,
																  fileStreamPtr->uniFileName.unicode,
																  fileStreamPtr->uniFileName.length);

            CFStringGetCString (cfStringRef,
											(char*)&fileStreamPtr->fileName[1],
											(CFIndex)255,
											kCFStringEncodingUTF8);
				fileStreamPtr->fileName[0] = strlen ((char*)&fileStreamPtr->fileName[1]);
				
						// This indicates that the full path is not part of the file name.
				
				fileStreamPtr->pathLength = 0;

            CFRelease (cfStringRef);

            errCode = GetFileTypeAndCreator (fileStreamPtr);

				}	// end "if (errCode == noErr)"
		#endif	// defined multispec_mac

		#if defined multispec_win
			FSRef* fileAsFSRefPtr = &fileAsFSRef;
			fileStreamPtr->SetFilePath ((TBYTE*)&fileAsFSRefPtr[0], TRUE);

			errCode = CMFileStream::GetFileType (
									(TBYTE*)&fileAsFSRefPtr[0], &fileStreamPtr->mFileType);

					// Force the pascal file name to be updated in case user is
					// expecting to use it.

			fileStreamPtr->GetFileNamePPtr ();
		#endif	// defined multispec_win
		
		if (errCode == noErr)
			errCode = OpenFileReadOnly (fileStreamPtr,
													kResolveAliasChains,
													kLockFile,
													kDoNotVerifyFileStream);

		}	// end "if (fileInfoPtr != NULL)"

	CheckAndUnlockHandle (fileInfoHandle);

	return (errCode);

}	// end "OpenSpecifiedFile"		



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ReorderLandsat8FileList
//
//	Software purpose:	The purpose of this routine is to reorder the Landsat 8 file
//							list so that the list is in wavelength order. Put band 9 in 
//							wavelength order with the rest and leave band 8 out if it
//							exists.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			FileSpecificationDialogOK
//
//	Coded By:			Larry L. Biehl			Date: 11/07/2017
//	Revised By:			Larry L. Biehl			Date: 11/07/2017

void ReorderLandsat8FileList (
				SInt16*								fileVectorPtr,
				SInt16*								instrumentCodePtr)
				
{
	SInt16								savedIndex,
											savedIndex8;
											
											
	savedIndex8 = fileVectorPtr[7];
	savedIndex = fileVectorPtr[8];
	fileVectorPtr[7] = fileVectorPtr[6];
	fileVectorPtr[6] = fileVectorPtr[5];
	fileVectorPtr[5] = savedIndex;
	fileVectorPtr[8] = fileVectorPtr[9];
	fileVectorPtr[9] = fileVectorPtr[10];
	fileVectorPtr[10] = savedIndex8;

	*instrumentCodePtr = kLandsatLC8_OLI_TIRS;

	if (fileVectorPtr[8] == -1 && fileVectorPtr[9] == -1)
		*instrumentCodePtr = kLandsatLC8_OLI;

	else if (fileVectorPtr[0] == -1 && fileVectorPtr[1] == -1 &&
				fileVectorPtr[2] == -1 && fileVectorPtr[3] == -1 &&
				fileVectorPtr[4] == -1 && fileVectorPtr[5] == -1 &&
				fileVectorPtr[6] == -1 && fileVectorPtr[7] == -1)
		*instrumentCodePtr = kLandsatLC8_TIRS;

}	// end "ReorderLandsat8FileList"	



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 SetHDFDataSetFileInformation
//
//	Software purpose:	The purpose of this routine is to set up the HDF data set
//							popup menu. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			FileSpecificationDialogOK
//
//	Coded By:			Larry L. Biehl			Date: 11/29/2001
//	Revised By:			Larry L. Biehl			Date: 04/30/2012

void SetHDFDataSetFileInformation (
				DialogPtr							dialogPtr,
				FileInfoPtr							fileInfoPtr,
				SInt16								hdfDataSetSelection,
				UInt32								dataSetIndex)
{
	HdfDataSets*						hdfDataSetsPtr;

	SInt32								offset;

	UInt32								dataSet;


			// Initialize local variables.	

	#if defined multispec_mac
		GetMenuItemRefCon (gPopUpTemporaryMenu, hdfDataSetSelection, &dataSetIndex);
		//offset = 4;
		offset = 0;
	#endif	// defined multispec_mac

	#if defined multispec_win || defined multispec_lin
		offset = 0;
	#endif	// defined multispec_win

	hdfDataSetsPtr = (HdfDataSets*)GetHandlePointer (fileInfoPtr->hdfHandle);

	if (hdfDataSetsPtr != NULL)
		{
		dataSet = hdfDataSetsPtr[dataSetIndex].dataSet;

		#if include_hdf_capability
					// This will check if this data set uses an extension. If so the current
					// file will be closed and the new one will be added to the file 
					// information structure. The file will then be opened.

			if (fileInfoPtr->format == kHDF4Type || fileInfoPtr->format == kNETCDFType)
				GetHDFDataSetSpecialInfo (fileInfoPtr, dataSetIndex);
		#endif		// include_hdf_capability

		fileInfoPtr->hdfDataSetSelection = (UInt16)(hdfDataSetSelection - offset);

				// Set global variable to help set default for users when making 
				// the same data set selection on similar hdf files.

		gLastHDFDataSetSelection = fileInfoPtr->hdfDataSetSelection;
		gLastHDFNumberDataSets = fileInfoPtr->numberHdfDataSets;

		}	// end "if (hdfDataSetsPtr != NULL)"
		
}	// end "SetHDFDataSetFileInformation" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 SetUpHDFDataSetPopupMenu
//
//	Software purpose:	The purpose of this routine is to set up the HDF data set
//							popup menu. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 11/28/2001
//	Revised By:			Larry L. Biehl			Date: 03/25/2017

void SetUpHDFDataSetPopupMenu (
				DialogPtr							dialogPtr,
				FileInfoPtr							fileInfoPtr)
{
	#if defined multispec_win
		CComboBox*							comboBoxPtr;
		SInt32								windowsMenuIndex;
	#endif	// defined multispec_win
		 
	#if defined multispec_lin
		wxComboBox*							comboBoxPtr;
		SInt32								windowsMenuIndex;
	#endif

	HdfDataSets*						hdfDataSetsPtr;

	SInt32								dataSetIndex,
											dataSetType,
											menuIndex,
											numberHdfDataSets;


	#if defined multispec
		USES_CONVERSION;
	#endif

		// Initialize local variables.													

	numberHdfDataSets = fileInfoPtr->numberHdfDataSets;

	hdfDataSetsPtr = (HdfDataSets*)GetHandlePointer (fileInfoPtr->hdfHandle, kLock);

	if (hdfDataSetsPtr != NULL)
		{
		#if defined multispec_win
			comboBoxPtr = (CComboBox*)dialogPtr->GetDlgItem (IDC_HDFDataSet);
			windowsMenuIndex = 0;
		#endif	// defined multispec_win
        
		#if defined multispec_lin
			comboBoxPtr = (wxComboBox*)dialogPtr->FindWindow (IDC_HDFDataSet);
			windowsMenuIndex = 0;
			((CMFileFormatSpecsDlg*)dialogPtr)->m_menuclientdata = calloc (
														numberHdfDataSets, sizeof (unsigned int));
			unsigned int* menuptr =
					(unsigned int*)((CMFileFormatSpecsDlg*)dialogPtr)->m_menuclientdata;
		#endif

		dataSetType = 0;
		menuIndex = 0;
        
		for (dataSetIndex = 1; dataSetIndex <= numberHdfDataSets; dataSetIndex++)
			{
			if (hdfDataSetsPtr[dataSetIndex].dataSetType != dataSetType)
				{
				if (dataSetType == 0)
					{
					if (hdfDataSetsPtr[dataSetIndex].dataSetType > 1)
						{
						dataSetType = 1;

                        // Indicate that there are no 8-bit images.

						#if defined multispec_mac
							//menuIndex++;
							//AppendMenu (gPopUpTemporaryMenu, "\pNo 8-bit images");
							//DisableMenuItem (gPopUpTemporaryMenu, menuIndex);

							//menuIndex++;
							//AppendMenu (gPopUpTemporaryMenu, "\p-");
						#endif	// defined multispec_mac

						#if defined multispec_win
							//dialogPtr->SetComboItemText (IDC_HDFDataSet,
							//											windowsMenuIndex,
							//											(char*)"\0No 8-bit images");
							//windowsMenuIndex++;
						#endif	// defined multispec_win

						}	// end "if (hdfDataSetsPtr[index].dataSetType != 1)"

					}	// end "if (dataSetType == 0)"

				if (dataSetType == 1 && hdfDataSetsPtr[dataSetIndex].dataSetType >= 2)
					{
					if (hdfDataSetsPtr[dataSetIndex].dataSetType > 2)
						{
                        // Indicate that there are not 8-bit images.

						#if defined multispec_mac
							//menuIndex++;
							//AppendMenu (gPopUpTemporaryMenu, "\pNo 24-bit images");
							//DisableMenuItem (gPopUpTemporaryMenu, menuIndex);
						#endif	// defined multispec_mac

						#if defined multispec_win  
							//dialogPtr->SetComboItemText (IDC_HDFDataSet,
							//											windowsMenuIndex,
							//											(char*)"\0No 24-bit images");
							//windowsMenuIndex++;
						#endif	// defined multispec_win

						}	// end "if (hdfDataSetsPtr[index].dataSetType > 2)"

					dataSetType = 2;

					}	// end "if (dataSetType == 1 && ..."

				if (dataSetType == 2 && hdfDataSetsPtr[dataSetIndex].dataSetType >= 3)
					{
                    // Add separator.

					#if defined multispec_mac
						//menuIndex++;
						//AppendMenu (gPopUpTemporaryMenu, "\p-");
					#endif	// defined multispec_mac

					#if defined multispec_win  
                    //dialogPtr->SetComboItemText (IDC_HDFDataSet,
                    //											windowsMenuIndex,
                    //											(char*)"\0-");
                    //windowsMenuIndex++;
					#endif	// defined multispec_win

					}	// end "if (dataSetType == 2 && ..."

				}	// end "if (hdfDataSetsPtr[index].dataSetType != dataSetType)"

			#if defined multispec_mac
				menuIndex++;
            AppendMenu (gPopUpTemporaryMenu, "\pNewFile");
            SetMenuItemText (gPopUpTemporaryMenu,
										menuIndex,
										hdfDataSetsPtr[dataSetIndex].name);
				/*
				CFStringRef cfStringRef;
				cfStringRef = CFStringCreateWithBytes (kCFAllocatorDefault,
																	&hdfDataSetsPtr[dataSetIndex].name[1],
																	hdfDataSetsPtr[dataSetIndex].name[0],
																	kCFStringEncodingUTF8,
																	false);
				//cfStringRef = CFStringCreateFromExternalRepresentation (
				//								kCFAllocatorDefault,
				//								(const __CFData*)hdfDataSetsPtr[dataSetIndex].name,
				//								kCFStringEncodingUTF8);
 				SetMenuItemTextWithCFString (gPopUpTemporaryMenu,
														menuIndex,
														cfStringRef);
																   
				//CFAllocatorDeallocate (kCFAllocatorDefault, (void*)cfStringRef);
				CFRelease (cfStringRef);
				*/
            SetMenuItemRefCon (gPopUpTemporaryMenu, menuIndex, dataSetIndex);
			#endif	// defined multispec_mac

			#if defined multispec_win
            dialogPtr->SetComboItemText (IDC_HDFDataSet,
														(SInt16)windowsMenuIndex,
														&hdfDataSetsPtr[dataSetIndex].name[1]);
            comboBoxPtr->SetItemData (windowsMenuIndex, dataSetIndex);
            windowsMenuIndex++;
			#endif	// defined multispec_win
            
			#if defined multispec_lin
            dialogPtr->SetComboItemText (IDC_HDFDataSet,
													  (SInt16)windowsMenuIndex,
													  (char*)&hdfDataSetsPtr[dataSetIndex].name[1]);
            *menuptr = (unsigned int)dataSetIndex;
            comboBoxPtr->SetClientData (windowsMenuIndex, (void*)dataSetIndex);
            windowsMenuIndex++;
            menuptr++;
			#endif

			dataSetType = hdfDataSetsPtr[dataSetIndex].dataSetType;

			}	// end "for (index=0; index<numberHdfDataSets; index++)"

		if (dataSetType == 1)
			{
					// Indicate that there are no 24-bit images.

			#if defined multispec_mac
				//AppendMenu (gPopUpTemporaryMenu, "\p-");
				//AppendMenu (gPopUpTemporaryMenu, "\pNo 24-bit images");
			#endif	// defined multispec_mac

			dataSetType = 2;

			}	// end "if (dataSetType == 1)"

		if (dataSetType == 2)
			{
					// Indicate that there are not scientific data sets.

			#if defined multispec_mac
				//AppendMenu (gPopUpTemporaryMenu, "\p-");
				AppendMenu (gPopUpTemporaryMenu, "\pNo scientific data sets");
			#endif	// defined multispec_mac

			dataSetType = 3;

			}	// end "if (dataSetType == 1"

		CheckAndUnlockHandle (fileInfoPtr->hdfHandle);

		}	// end "if (hdfDataSetsPtr != NULL)"

}	// end "SetUpHDFDataSetPopupMenu"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 VerifyFileInfoDialogValues
//
//	Software purpose:	The purpose of this routine is to check the file
//							parameter values.  If one is not within limits then
//							the number of that dialog item is returned.  If
//							all parameters are within limits then 0 is returned
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/12/1993
//	Revised By:			Larry L. Biehl			Date: 06/22/2006	

SInt16 VerifyFileInfoDialogValues (
				DialogPtr							dialogPtr,
				SInt32								numberLines,
				SInt32								numberColumns,
				SInt32								numberChannels_Classes,
				SInt32								startLine,
				SInt32								startColumn,
				SInt32								numberHeaderBytes,
				SInt32								numberTrailerBytes,
				SInt32								numberPreLineBytes,
				SInt32								numberPostLineBytes,
				SInt32								numberPreChannelBytes,
				SInt32								numberPostChannelBytes,
				SInt16								dataTypeSelection,
				Boolean								thematicType,
				SInt32*								minLimitPtr,
				SInt32*								maxLimitPtr)
{
	SInt16								returnValue;


	returnValue = 1;

	if (numberLines < 1 || numberLines > kMaxNumberLines)
		{
		returnValue = IDC_NumberLines;
		*minLimitPtr = 1;
		*maxLimitPtr = kMaxNumberLines;

		}	// end "else if (numberLines < 1 || ..."

	else if (numberColumns < 1 || numberColumns > kMaxNumberColumns)
		{
		returnValue = IDC_NumberColumns;
		*minLimitPtr = 1;
		*maxLimitPtr = kMaxNumberColumns;

		}	// end "else if (numberColumns < 1 || ..."

	else if (thematicType && (numberChannels_Classes < 1 ||
														numberChannels_Classes > kMaxNumberClasses))
		{
		returnValue = IDC_NumberChannels;
		*minLimitPtr = 1;
		*maxLimitPtr = kMaxNumberClasses;

		}	// end "else if thematicType && (..."

	else if (!thematicType && (numberChannels_Classes < 1 ||
														numberChannels_Classes > kMaxNumberChannels))
		{
		returnValue = IDC_NumberChannels;
		*minLimitPtr = 1;
		*maxLimitPtr = kMaxNumberChannels;

		}	// end "else if !thematicType && (..."

	else if (startLine < 1 || startLine > LONG_MAX)
		{
		returnValue = IDC_StartLineNumber;
		*minLimitPtr = 1;
		*maxLimitPtr = LONG_MAX;

		}	// end "else if (startLine < 1 || ..."

	else if (startColumn < 1 || startColumn > LONG_MAX)
		{
		returnValue = IDC_StartColumnNumber;
		*minLimitPtr = 1;
		*maxLimitPtr = LONG_MAX;

		}	// end "else if (startColumn < 1 || ..."

	else if (numberHeaderBytes > LONG_MAX)
		{
		returnValue = IDC_HeaderBytes;
		*minLimitPtr = 0;
		*maxLimitPtr = LONG_MAX;

		}	// end "else if (numberHeaderBytes > LONG_MAX)"

	else if (numberTrailerBytes > LONG_MAX)
		{
		returnValue = IDC_TrailerBytes;
		*minLimitPtr = 0;
		*maxLimitPtr = LONG_MAX;

		}	// end "else if (numberTrailerBytes > LONG_MAX)"

	else if (numberPreLineBytes > LONG_MAX)
		{
		  returnValue = IDC_PreLineBytes;
		  *minLimitPtr = 0;
		  *maxLimitPtr = LONG_MAX;

		}	// end "else if (numberPreLineBytes > LONG_MAX)"

	else if (numberPostLineBytes > LONG_MAX)
		{
		returnValue = IDC_PostLineBytes;
		*minLimitPtr = 0;
		*maxLimitPtr = LONG_MAX;

		}	// end "else if (numberPostLineBytes > LONG_MAX)"

	else if (numberPreChannelBytes > LONG_MAX)
		{
		returnValue = IDC_PreChannelBytes;
		*minLimitPtr = 0;
		*maxLimitPtr = LONG_MAX;

		}	// end "else if (numberPreChannelBytes > LONG_MAX)"

	else if (numberPostChannelBytes > LONG_MAX)
		{
		returnValue = IDC_PostChannelBytes;
		*minLimitPtr = 0;
		*maxLimitPtr = LONG_MAX;

		}	// end "else if (numberPostChannelBytes > LONG_MAX)"

	else if (thematicType && (dataTypeSelection < k4BitUnsignedIntegerMenuItem ||
											dataTypeSelection > k2ByteUnsignedIntegerMenuItem))
		returnValue = IDC_DataValueTypePopUp;

	return (returnValue);

}	// end "VerifyFileInfoDialogValues"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean VerifyImageFileSizeAgainstBaseImage
//
//	Software purpose:	The purpose of this routine is to compare the image
//							file information with the project file information.
//							True is returned if the two compare.  False is returned
//							if they do not compare.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 01/24/2013
//	Revised By:			Larry L. Biehl			Date: 03/17/2013	

Boolean VerifyImageFileSizeAgainstBaseImage (
				Handle								windowInfoHandle,
				Handle								fileInfoHandle)
{
	FileInfoPtr							fileInfoPtr;
	WindowInfoPtr						windowInfoPtr;

	Boolean								returnFlag;


	returnFlag = FALSE;
	if (fileInfoHandle != NULL && windowInfoHandle != NULL)
		{
		returnFlag = TRUE;

		windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);

		fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);

		if (fileInfoPtr->numberLines != windowInfoPtr->maxNumberLines ||
							fileInfoPtr->numberColumns != windowInfoPtr->maxNumberColumns)
			returnFlag = FALSE;

		}	// end "if (fileInfoHandle != NULL && ...)"

	return (returnFlag);

}	// end "VerifyImageFileSizeAgainstBaseImage"
