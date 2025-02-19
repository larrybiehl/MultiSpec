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
//	File:						SFileIO.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			02/18/2025
//
//	Language:				C
//
//	System:					Linux, Macintosh and Windows Operating Systems
//
//	Brief description:	The purpose of the routines in this file is to
//								provide utility type functions in MultiSpec.
//
/* Template for debugging
		int numberChars = snprintf ((char*)gTextString3,
												256,
												" SFileIO::GetFile (entered routine. %s", 
												gEndOfLine);
		ListString ((char*)gTextString3, numberChars, gOutputTextH);	
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include "SFileStream_class.h"
      
#if defined multispec_wx
	#include "wx/wx.h"
	#include "xMultiSpec.h"
	#include "xOpenFileDialog.h"
	#include "wx/filedlg.h"
#endif   // defined multispec_wx

#if defined multispec_mac
	#define kOpenPrefKey						1234
	#define kPutPrefKey						4321
	#define IDS_FileIO192					192
	#define IDS_Alert147						147
	#define IDS_FileIOStr202				202
							
	typedef struct NavUserData
		{
		FSRef									parentFSRef;
		FileInfoPtr							fileInfoPtr;
		UserItemUPP							drawLinkOptionsPopUpPtr;
		Handle								fileInfoHandle;
		NavMenuItemSpecArrayHandle		navMenuItemSpecHandle;
		SInt32								filterSelection;
		SInt32								initialMenuSelection;
		SInt32								stringIndex;
		
		}	NavUserData, *NavUserDataPtr;
#endif	// defined multispec_mac

#if defined multispec_mac_swift
	#define kOpenPrefKey						1
	#define IDS_FileIO192					192
	#define IDS_Alert147						147
	#define IDS_FileIOStr202				202
#endif	// defined multispec_mac_swift
  
#if defined multispec_win
	//#include <afxpriv.h>
	#include "SWindowInfo_class.h"
	#include "WOpenFileDialog.h"  
	#include <direct.h>
	                                                                   
	#include <afxdlgs.h>

	extern Boolean MGetString (
				TBYTE*								outTextPtr, 
				UInt16								stringListID, 
				UInt16								stringID);		

	Boolean GetSpecifiedStringNumber (
				SInt16								strListID, 
				SInt16								index,
				TBYTE*								textStringPtr, 
				Boolean								continueFlag);
#endif	// defined multispec_win


#if include_gdal_capability
	// oul: added definition of SIZE_UNSIGNED_LONG and SIZEOF_VOIDP
	// which are not defined in cpl_config.h
	
	#if defined multispec_wx
		#if defined NetBeansProject
			// The size of a 'unsigned long', as computed by sizeof.
			#define SIZEOF_UNSIGNED_LONG 8
			// The size of a 'void p', as computed by sizeof.
			#define SIZEOF_VOIDP 8
		#endif
	#endif
	
	// oul: changed file path of gdal.h
	#ifndef multispec_wx
		#include "gdal.h"
	#endif
	
	#if defined multispec_wx
		#ifndef NetBeansProject
			//#include "gdal/gdal.h"
			#include "gdal.h"
		#else
			#include "gdal.h"
		#endif
	#endif
#endif	// include_gdal_capability

extern Handle 			gCustomNavOpenList;
extern Handle 			gCustomNavPut;	

							

		// Prototype descriptions for routines in this file that are only		
		// called by routines in this file.

void		AdjustSignedData (
				FileInfoPtr							fileInfoPtr,
				HUCharPtr							fileIOBufferPtr,
				UInt32								numberSamples);

SInt16 	CheckIfSTAFile (
				CMFileStream*		 				fileStreamPtr);

SInt16 	CheckIfThematicSupportFile (
				CMFileStream*		 				fileStreamPtr);

void 		CloseUpHeirarchalFileIOParameters (
				LayerInfoPtr						layerInfoPtr,
				FileInfoPtr							fileInfoPtr, 
				UInt16*								channelListPtr,
				UInt16								numberChannels);
									
Boolean	CreateTRLSupportFile (
				CMFileStream*						trailerStreamPtr, 
				UInt16								numberSupportFileClasses, 
				RGBCharColorPtr					classColorTablePtr,
				char*									classNameTablePtr,
				SInt16								trailerCode,
				Boolean								writeClassNamesFlag);
							
SInt16	GetGDALLineOfData (
				FileInfoPtr							fileInfoPtr,	
				HUCharPtr							ioBufferPtr,
				UInt32								lineNumber,
				UInt32								columnStart,
				UInt32								columnEnd,
				UInt32								numberChannels,
				int*									gdalChannelListPtr,
				UInt32*								numberSamplesReadPtr,
				Boolean								oneReadFlag);
								
Boolean	GetFileDlgDetermineLinkVisibility ();

SInt16	GetFileTypeAndCreator (
				CMFileStream*						fileStreamPtr);
							
SInt16 	GetThematicSupportFileToCreate (
				FileInfoPtr							gisFileInfoPtr,
				CMFileStream*						trailerStreamPtr,
				UInt32								supportFileType,
				Boolean								promptFlag);
                       
void 		LoadErdasTRLClassColorBuffer (
				FileInfoPtr							gisFileInfoPtr,
				ColorSpec*							colorSpecPtr,
				SInt16*								classPtr,
				UInt32								numberListClasses,
				UInt16*								paletteIndexPtr,
				UInt32								numberTRLClasses,
				RGBCharColorPtr					classColorPtr,
				SInt16								collapseClassCode,
				SInt16								classNameCode);
                       
void	 	LoadErdasTRLClassNameBufferFromDescriptions (
				FileInfoPtr							gisFileInfoPtr,
				UInt32								inputNumberClasses,
				UCharPtr								ioTempBufferPtr,
				SInt16								collapseClassCode);	

SInt16	LoadGroupInformationBuffers (
				FileInfoPtr							fileInfoPtr,
				UInt16								numberSupportFileClasses,
				CTabHandle							groupCTableHandle,
				SInt16*								classToGroupPtr,
				UInt16*								groupToPalettePtr,
				RGBCharColorPtr					groupColorTablePtr,
				Boolean*								listAllGroupInfoPtr,
				UInt16*								classSymbolPtr,
				UInt16								numberClassSymbols,
				Boolean								asciiSymbolsFlag);

Boolean	LoadProbabilityGroupInformation (
				FileInfoPtr							fileInfoPtr,  
				RGBCharColorPtr					classColorTablePtr,
				CTabHandle*							cTableHandlePtr);
							
Boolean	NavServicesCheckForIgnoredFiles (
				FileStringPtr						filePathPtr);
			
#if defined multispec_mac				
	pascal Boolean NavServicesFilterProc (
					AEDesc* 								theItem, 
					void* 								info, 
					NavCallBackUserData 				callBackUD, 
					NavFilterModes 					filterMode);
								
	pascal void NavServicesGetEventProc (
					NavEventCallbackMessage 		callBackSelector, 
					NavCBRecPtr 						callBackParms, 
					NavCallBackUserData 				callBackUD);

	SInt16 	NavServicesGetFile (
					CMFileStream* 						fileStreamPtr, 
					SInt16								numberTypes, 
					OSType*								fileTypesPtr, 
					LocalAppFile*						localAppFilePtr,
					FSRef*								fileAsFSRefPtr,
					UInt32*								itemCountPtr, 
					SInt16								stringIndex);
	/*							
	pascal Boolean NavServicesPreviewProc (
					NavCBRecPtr							callBackParms,
					void*									callBackUD);
	*/							
	pascal void NavServicesPutEventProc (
					NavEventCallbackMessage 		callBackSelector, 
					NavCBRecPtr 						callBackParms, 
					NavCallBackUserData 				callBackUD);
								
	SInt16 	NavServicesPutFile (
					CMFileStream*		 				fileStreamPtr, 
					SInt64								numberBytes, 
					SInt16								stringIndex, 
					SInt32								creator);
#endif	// defined multispec_mac 
													
void 		PackBISData (
				LayerInfoPtr						layerInfoPtr,
				UInt32								columnInterval,
				UInt32								numberOutputBufferChannels,
				UInt32								numberBufferChannels, 
				UInt32								numberChannels,
				UInt16*								channelListPtr,
				UInt32								numberSamples, 
				UInt32								dataConversionCode, 
				HUCharPtr 							ioCharBufferPtr,
				HUCharPtr 							dataCharBufferPtr,
				Boolean								toBISFormatFlag);
							
UInt32 	PackMaskData (
				HUInt16Ptr							maskBufferPtr,
				UInt32								maskColumnStart,
				UInt32								numberChannels,
				UInt32								numberInputSamples,
				UInt32								columnInterval,
				HUCharPtr							dataBufferPtr,
				UInt32								numberOutBytes,
				SInt32								maskValueRequest);
							
void		PackNonBISData (
				UInt32								columnOffset,
				UInt32								columnInterval,
				UInt32								numberColumnsPerChannel,  
				UInt32								fileNumberChannels,
				UInt32								numberChannels,
				HUInt16Ptr							channelListPtr, 
				UInt32								numberSamples,
				UInt32								dataConversionCode, 
				HUCharPtr							ioCharBufferPtr, 
				HUCharPtr							dataCharBufferPtr, 
				Boolean								toBISFormat); 

void		PackGAIAData (
				SInt32								columnStart, 
				SInt32								columnEnd, 
				HSInt16Ptr							inputBufferPtr);

void 		PackBlockedData (
				UInt32								line,
				UInt32								columnStart, 
				UInt32								columnEnd,
				UInt32								channelStartIndex,
				FileInfoPtr							fileInfoPtr,
				HierarchalFileFormatPtr			hfaPtr,
				HUCharPtr							outputBufferPtr);

SInt16 	SetUpDataConversionCode (
				WindowInfoPtr						windowInfoPtr,
				LayerInfoPtr						layerInfoPtr,
				FileInfoPtr							fileInfoPtr,
				UInt16								numberListChannels,
				HUInt16Ptr							channelListPtr,	
				UInt16								forceOutByteCode);

void 		SetUpHeirarchalFileIOParameters (
				LayerInfoPtr						layerInfoPtr,
				FileInfoPtr							fileInfoPtr,
				UInt16								numberChannels, 
				UInt16*								channelListPtr,
				UInt32								columnStart, 
				UInt32								columnEnd);
	
	
													
// === Static Member Variable ===


#if defined multispec_mac
	static UInt16				sCustomControlCount = 0;
	static Boolean				sInitialLinkSelectedFilesFlag = FALSE;
#endif


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void AdjustSignedData
//
//	Software purpose:	The purpose of this routine is to adjust the signed one-byte
//							or two-byte data so that it is in a range of 0 to max one-byte
//							or two-byte unsigned number.
//		
//	Parameters in:		File stream pointer.
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 12/17/2010
//	Revised By:			Larry L. Biehl			Date: 12/17/2010

void AdjustSignedData (
				FileInfoPtr							fileInfoPtr,
				HUCharPtr							fileIOBufferPtr,
				UInt32								numberSamples)

{
	if (fileInfoPtr->numberBytes == 1)
		{
		HPtr					byte1Ptr;
		
		UInt32				sampleCount;
		char					signedBitMask1;
		char					signedValueMask1;
		
		signedBitMask1 = (char)fileInfoPtr->signedBitMask;
		signedValueMask1 = (char)fileInfoPtr->signedValueMask;
		byte1Ptr = (HPtr)fileIOBufferPtr;
		
		for (sampleCount=0; sampleCount<numberSamples; sampleCount++)
			{
			*byte1Ptr = (*byte1Ptr ^ signedBitMask1);
			*byte1Ptr = (*byte1Ptr & signedValueMask1);
			byte1Ptr++;
			
			}	// end "for (sampleCount=0;  ..." 
		
		}	// end "if (fileInfoPtr->numberBytes == 1)" 
	
	else if (fileInfoPtr->numberBytes == 2) 
		{
		HSInt16Ptr				twoBytePtr;
		
		UInt32					sampleCount;
		SInt16					signedBitMask2;
		SInt16					signedValueMask2;
		
		signedBitMask2 = fileInfoPtr->signedBitMask;
		signedValueMask2 = fileInfoPtr->signedValueMask;
		twoBytePtr = (HSInt16Ptr)fileIOBufferPtr;
		
		for (sampleCount=0; sampleCount<numberSamples; sampleCount++)
			{
			*twoBytePtr = (*twoBytePtr ^ signedBitMask2);
			*twoBytePtr = (*twoBytePtr & signedValueMask2);
			twoBytePtr++;
			
			}	// end "for (sampleCount=0; ..." 
		
		}	// end "else if (fileInfoPtr->numberBytes == 2)"  
	
}	// end "AdjustSignedData"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CheckIfDirectoryIsWriteable
//
//	Software purpose:	The purpose of this routine is to determine if the specified
//							directory is writeable. If not, the directory in the file
//							path in the input file stream structure is changed to the
//							working directory.
//							This routine is currently only used for the wxWidgets interface
//							version, specifically needed for MultiSpec Online on mygeohub.
//							Other notes:
// 						Use wxSetWorkingDirectory (wxString) or wxFileDialog::SetDirectory
// 						to do this. Use wxGetCwd or wxFileDialog::GetDirectory to determine
// 						if the default one is the standard directory.

//
//	Parameters in:
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/09/2019
//	Revised By:			Larry L. Biehl			Date: 04/11/2020

void CheckIfDirectoryIsWriteable (
				CMFileStream*						fileStreamPtr)

{
#if defined multispec_wx && !defined NetBeansProject
	wxFileName 							fileName;

	FileStringPtr						fileNamePtr,
											filePathPtr;
	
	
	filePathPtr = (FileStringPtr)fileStreamPtr->GetFilePathPPtr ();
	wxString wxFilePathName (&filePathPtr[2], wxConvUTF8);
	fileName.Assign (wxFilePathName);

	if (!fileName.IsDirWritable ())
		{
				// Need to change the directory to the working output directory
		
		fileNamePtr = (FileStringPtr)fileStreamPtr->GetFileNameCPtr ();
		
		wxString workingDirectory = wxGetCwd ();
		workingDirectory.Append ("/");
		workingDirectory.Append (fileNamePtr);
		wxCharBuffer charWorkingDirectory = workingDirectory.ToAscii ();
		
				// Close the current file and reset the path name for new .sta file

		fileStreamPtr->SetFilePathFromCharString (
										(StringPtr)charWorkingDirectory.data (),
										TRUE);  // force current file to be closed
		
		UpdateFileNameInformation (fileStreamPtr, NULL);
		
				// Now set the gOutputDirectory string
		
		gOutputDirectory = wxGetCwd ();
		
		}	// end "if (!fileName.IsDirWritable ())"
	
	else	// fileName.IsDirWritable
		{
		/*
		SInt16 numberChars9 = snprintf ((char*)gTextString3,
												256,
												" Directory is writable: %s",
												gEndOfLine);
		ListString ((char*)gTextString3, numberChars9, gOutputTextH);
		*/
		}	// end "else fileName.IsDirWritable"
#endif	// defined multispec_wx && !defined NetBeansProject
	
}	// end "CheckIfDirectoryIsWriteable"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CheckIfSpecifiedFileExists
//
//	Software purpose:	The purpose of this routine is to determine if the file exists
//							that has a name that is the result of the removing any suffix
//							and adding the input suffix.
//		
//	Parameters in:		File stream pointer.
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 05/13/2011
//	Revised By:			Larry L. Biehl			Date: 03/14/2017

Boolean CheckIfSpecifiedFileExists (
				CMFileStream* 						fileStreamPtr, 
				char*									suffixToCheckPtr)

{
	CMFileStream						checkFileStream;
	
	FileStringPtr						checkFileNamePtr;
											
	CMFileStream						*checkFileStreamPtr;
	
	SInt16								errCode;
	
	Boolean								fileExistsFlag = FALSE;
	
			
	if (fileStreamPtr != NULL && suffixToCheckPtr != NULL)
		{	
				// Assumption is made that the file to be checkd is in the same
				// folder as the input file.
				
		errCode = noErr;
								
		checkFileStreamPtr = &checkFileStream;		
		InitializeFileStream (checkFileStreamPtr, fileStreamPtr);
		
		checkFileNamePtr = 
					(FileStringPtr)GetFilePathPPointerFromFileStream (checkFileStreamPtr);
		
				// Remove any suffix that exists and then add requested suffix to 
				// the file name to see if that file exists.
		
		RemoveCharsNoCase ((char*)"\0.???\0", checkFileNamePtr);
		ConcatFilenameSuffix ((UInt8*)checkFileNamePtr, (UInt8*)suffixToCheckPtr);
		errCode = OpenFileReadOnly (checkFileStreamPtr, 
												kResolveAliasChains, 
												kLockFile, 
												kVerifyFileStream);
												
		if (errCode == noErr)
			fileExistsFlag = TRUE;			
			
		CloseFile (checkFileStreamPtr);
												
		}	// end "if (fileStreamPtr != NULL && suffixToCheckPtr != NULL)"
		
	return (fileExistsFlag);
	
}	// end "CheckIfSpecifiedFileExists"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 CheckIfSTAFile
//
//	Software purpose:	The purpose of this routine is to determine if the input file
//							is an ERDAS image statistics file.
//		
//	Parameters in:		File stream pointer.
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 10/04/2001
//	Revised By:			Larry L. Biehl			Date: 02/23/2017

SInt16 CheckIfSTAFile (
				CMFileStream*		 				fileStreamPtr)

{
	Str15									tempString;
	
	char									*erdasStatHeader = (char*)"TRAIL";
	FileStringPtr						filePathPtr;		
	
	UInt32								count;
	
	SInt16								errCode,
											returnCode = 0;
	
					
	count = 5;
	errCode = MReadData (fileStreamPtr, 
								&count, 
								&tempString,
								kNoErrorMessages);
				
	if (errCode == noErr)
		{  
		if (strncmp ((char*)&tempString, erdasStatHeader, 5) == 0)
			{	
			filePathPtr = 
						(FileStringPtr)GetFilePathPPointerFromFileStream (fileStreamPtr);
			
			if (CompareSuffixNoCase ((char*)"\0.sta", filePathPtr, NULL))
																								return (1);
				
			}	// end "if (strncmp ((char*)&tempString, erdasStatHeader, 5) == 0)"
						
		}	// end "if (errCode == noErr)"
		
	return (returnCode);
			
}	// end "CheckIfSTAFile"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 CheckIfThematicSupportFile
//
//	Software purpose:	The purpose of this routine is to determine if the input file
//							is an ERDAS class information file.
//		
//	Parameters in:		File stream pointer.
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 10/04/2001
//	Revised By:			Larry L. Biehl			Date: 02/23/2017

SInt16 CheckIfThematicSupportFile (
				CMFileStream*		 				fileStreamPtr)

{
	Str15									tempString;
	
	char									*erdasStatHeader = (char*)"TRAIL";
	FileStringPtr						filePathPtr;		
	
	UInt32								count;
	
	SInt16								errCode,
											returnCode = 0;
	
				
	filePathPtr = (FileStringPtr)GetFilePathPPointerFromFileStream (fileStreamPtr);
	
	if (CompareSuffixNoCase ((char*)"\0.clr", filePathPtr, NULL))
																								return (1);
	
	else	// !CompareSuffixNoCase ((char*)"\0.clr", ...
		{
		count = 5;
		errCode = MReadData (fileStreamPtr, 
									&count, 
									&tempString,
									kNoErrorMessages);
					
		if (errCode == noErr)
			{  
			if (strncmp ((char*)&tempString, erdasStatHeader, 5) == 0)
				{
				filePathPtr = 
						(FileStringPtr)GetFilePathPPointerFromFileStream (fileStreamPtr);
				
				if (CompareSuffixNoCase ((char*)"\0.trl", filePathPtr, NULL))
																								return (1);
											
				}	// end "if (strncmp ((char*)&tempString, erdasStatHeader, 5) == 0)"
				
			}	// end "if (errCode == noErr)"
			
		}	// end "else !CompareSuffixNoCase ((char*)"\0.clr", ..."
		
	return (returnCode);
			
}	// end "CheckIfThematicSupportFile"



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
//	Coded By:			Larry L. Biehl			Date: 03/23/1988
//	Revised By:			Larry L. Biehl			Date: 02/27/2018

void CloseFile (
				FileInfoPtr							fileInfoPtr)

{
				// Check input variables.														
				
	if (fileInfoPtr != NULL)
		{
		CMFileStream* fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
			
		CloseFile (fileStreamPtr);
		
		}	// end "if (fileInfoPtr != NULL)"
	
}	// end "CloseFile" 



void CloseFile (
				CMFileStream*						fileStreamPtr)

{
				// Check input variables.														
				
	if (fileStreamPtr != NULL)
		{
		#if defined multispec_mac
			SInt16				errCode = 1;
			
		
			if (fileStreamPtr->refNum != 0)
				{
				if (gHasHFSPlusAPIs)
					errCode = FSCloseFork (fileStreamPtr->refNum);
				
				else	// !gHasHFSPlusAPIs
					{
					if (fileStreamPtr->fileName[0] != 0)
						errCode = FSClose (fileStreamPtr->refNum);
					
					}	// end "else !gHasHFSPlusAPIs"
					      			
				if (errCode == noErr)
					{
							// If the file is not found (the user must have moved		
							// the file) do not worry about the error.					
							
					if (errCode == fnfErr)
						errCode = noErr;
						
					}	// end "if (errCode == noErr)"
				
				if (errCode == noErr)
					errCode = FlushVol (NIL, fileStreamPtr->vRefNum);
					
				if (errCode != wPrErr && errCode != afpAccessDenied)
					IOCheck (errCode, fileStreamPtr);
			  		
				fileStreamPtr->refNum = 0;
											
				gNumberOfOpenFiles--;
				
				}	// end "if (fileStreamPtr->refNum != 0)"
		#endif	// defined multispec_mac
			
      #if defined multispec_win || defined multispec_wx
			fileStreamPtr->MCloseFile ();
		#endif	// defined multispec_win || defined multispec_wx
			
		}	// end "if (fileStreamPtr != NULL)" 
	
}	// end "CloseFile" 



void CloseFile (
				Handle								fileStreamHandle)

{
	CMFileStream*						fileStreamPtr;
	
	SignedByte							handleStatus;
	
	
				// Check input variables.														
				
	fileStreamPtr = (CMFileStream*)GetHandleStatusAndPointer (fileStreamHandle,
																					&handleStatus);
																	
	CloseFile (fileStreamPtr); 
	
	MHSetState (fileStreamHandle, handleStatus);
	
}	// end "CloseFile" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 CloseUpFileIOInstructions
//
//	Software purpose:	This routine closes up parameters in the file IO instruction
//							structure.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/22/1998
//	Revised By:			Larry L. Biehl			Date: 08/27/2010

void CloseUpFileIOInstructions (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				AreaDescription* 					areaDescriptionPtr)

{
	if (fileIOInstructionsPtr != NULL)
		{
				// Unlock the mask buffer handle if needed.
		
		if (fileIOInstructionsPtr->maskBufferPtr != NULL && areaDescriptionPtr != NULL)
			{
			CheckAndUnlockHandle (areaDescriptionPtr->maskInfoPtr->maskHandle);
			fileIOInstructionsPtr->maskBufferPtr = NULL;
			
			}	// end "if (fileIOInstructionsPtr->maskBufferPtr != NULL && ..."
			
				// Release memory allocated to fileInfoPtr->gdalChannelListPtr.
				
		fileIOInstructionsPtr->gdalChannelListPtr = 
				(int*)CheckAndDisposePtr ((Ptr)fileIOInstructionsPtr->gdalChannelListPtr);
		
				// Force flag for Threaded IO to be FALSE.
				
		gUseThreadedIOFlag = FALSE;	

		EndFileIOThread (fileIOInstructionsPtr);

		}	// end "if (fileIOInstructionsPtr != NULL)"
		 	 
}	// end "CloseUpFileIOInstructions"   



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 CloseUpGeneralFileIOInstructions
//
//	Software purpose:	This routine closes the general parameters in the file IO
//							instruction structure.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 10/14/1999
//	Revised By:			Larry L. Biehl			Date: 05/05/2022

void CloseUpGeneralFileIOInstructions (
				FileIOInstructionsPtr			fileIOInstructionsPtr)
							
{	
	if (fileIOInstructionsPtr != NULL)
		{
		CloseUpHeirarchalFileIOParameters (
												fileIOInstructionsPtr->layerInfoPtr,
												fileIOInstructionsPtr->fileInfoPtr, 
												fileIOInstructionsPtr->channelListPtr,
												(UInt16)fileIOInstructionsPtr->numberChannels);
					
		UnlockHierarchalFileHandles (fileIOInstructionsPtr->windowInfoPtr, 
												fileIOInstructionsPtr->fileInfoPtr);
					
		UnlockHDF_FileHandles (fileIOInstructionsPtr->windowInfoPtr, 
										fileIOInstructionsPtr->fileInfoPtr);
		
		fileIOInstructionsPtr->windowInfoPtr = NULL;
		fileIOInstructionsPtr->fileInfoPtr = NULL;
		fileIOInstructionsPtr->layerInfoPtr = NULL;
		fileIOInstructionsPtr->channelListPtr = NULL;
		fileIOInstructionsPtr->inputBufferPtrs[0] = NULL;
		fileIOInstructionsPtr->inputBufferPtrs[1] = NULL;
									
		fileIOInstructionsPtr->outputBufferPtrs[0] = NULL;
		fileIOInstructionsPtr->outputBufferPtrs[1] = NULL;
		
		fileIOInstructionsPtr->tiledBufferPtrs[0] = NULL;
		fileIOInstructionsPtr->tiledBufferPtrs[1] = NULL;
		
		fileIOInstructionsPtr->bufferOffset = 0;
		
		fileIOInstructionsPtr->numberChannels = 0;
		
		fileIOInstructionsPtr->forceByteCode = kDoNotForceBytes;
		fileIOInstructionsPtr->forceOutputFormatCode = kDoNotForceFormat;
		fileIOInstructionsPtr->packDataFlag = FALSE;
		
		}	// end "if (fileIOInstructionsPtr != NULL)"
	
}	// end "CloseUpGeneralFileIOInstructions" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CloseUpHeirarchalFileIOParameters
//
//	Software purpose:	This routine closes the buffer pointers in the 
//							HierarchalFileFormat structures in the input fileInfo structure.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 10/20/1999
//	Revised By:			Larry L. Biehl			Date: 10/26/1999	

void CloseUpHeirarchalFileIOParameters (
				LayerInfoPtr						layerInfoPtr,
				FileInfoPtr							fileInfoPtr, 
				UInt16*								channelListPtr,
				UInt16								numberChannels)

{
	FileInfoPtr							localFileInfoPtr;
	
	HierarchalFileFormatPtr			hfaPtr,
											localHfaPtr;
	
	SInt32								fileInfoIndex,
											nextFileInfoIndex;
	
	UInt32								channelNumber,
											fileChannelNumber,
											index;
											
	Boolean								bandInterleaveByBlockFlag;
											
	
	if (fileInfoPtr != NULL)
		{
		fileChannelNumber = 0;
		fileInfoIndex = -1;
		
		for (index=1; index<=numberChannels; index++)
			{	
			channelNumber = index;								
			if (channelListPtr != NULL)
				channelNumber = channelListPtr[index-1] + 1;
			
			nextFileInfoIndex = 0;
			fileChannelNumber++;
			if (layerInfoPtr != NULL)
				{
				nextFileInfoIndex = layerInfoPtr[channelNumber].fileInfoIndex;
				fileChannelNumber = layerInfoPtr[channelNumber].fileChannelNumber;
				
				}	// end "if (layerInfoPtr != NULL)"
				
			if (fileInfoIndex != nextFileInfoIndex)
				{
				fileInfoIndex = nextFileInfoIndex;
				localFileInfoPtr = &fileInfoPtr[fileInfoIndex];
					
						// Get pointer to the hfa structure.
						
				hfaPtr = (HierarchalFileFormatPtr)GetHandlePointer (
																		localFileInfoPtr->hfaHandle);
														
				bandInterleaveByBlockFlag = FALSE;
				
				}	// end "if (fileInfoIndex != nextFileInfoIndex)"
			
			if (hfaPtr != NULL)
				{
				if (localFileInfoPtr->bandInterleave == kBIBlock)
					{
					fileChannelNumber = 1;
					bandInterleaveByBlockFlag = TRUE;
					
					}	// end "if (localFileInfoPtr->bandInterleave == kBIBlock)"
			
				localHfaPtr = &hfaPtr[fileChannelNumber-1];
				
				localHfaPtr->tiledBufferPtr = NULL;	
				
				localHfaPtr->firstLineRead = 0;
				localHfaPtr->lastLineRead = 0;	
				localHfaPtr->firstColumnStartByte = 0;
											
				localHfaPtr->firstColumnRead = 0;
				localHfaPtr->lastColumnRead = 0;
				localHfaPtr->numberBlocksRead = 0;	
				
				}	// end "if (hfaPtr != NULL)"
				
			}	// end "for (index=1; index<=numberChannels; index++)" 
			
		}	// end "if (fileInfoPtr != NULL)"
	
}	// end "CloseUpHeirarchalFileIOParameters" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CloseWindowImageFiles
//
//	Software purpose:	This routine closes the open image files associated
//							with the input window information handle and disposes
//							of the description handle and file information handle
//							associated with each of the image files.
//
//	Parameters in:		Handle to window information
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			DeactivateProject in SProject.cpp
//							ChangeProjectBaseImage in SProject.cpp
//
//	Coded By:			Larry L. Biehl			Date: 06/21/1990
//	Revised By:			Larry L. Biehl			Date: 06/04/1996

void CloseWindowImageFiles (
				Handle								windowInfoHandle)

{
	Handle								fileInfoHandle;
	SignedByte							handleStatus;
	WindowInfoPtr						windowInfoPtr;

		 
	windowInfoPtr = (WindowInfoPtr)GetHandleStatusAndPointer (
																	windowInfoHandle, &handleStatus);

	if (windowInfoPtr != NULL)
		{												
		fileInfoHandle = windowInfoPtr->fileInfoHandle;
		
		DisposeFileInfoHandle (fileInfoHandle);
		
		windowInfoPtr->fileInfoHandle = NULL;
		windowInfoPtr->numberImageFiles = 0;
			
		}	// end "if (windowInfoPtr != NULL)"
		
	MHSetState (windowInfoHandle, handleStatus); 
			
}	// end "CloseWindowImageFiles" 
                    


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt32 ConvertATRealtoInt
//
//	Software purpose:	This routine converts an IBM PC-AT real number
//							to a Motorola integer number.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:			CheckImageHeader
//
//	Coded By:			Larry L. Biehl			Date: 08/23/1988
//	Revised By:			Larry L. Biehl			Date: 04/17/1995

SInt32 ConvertATRealtoInt (
				CharPtr								ATrealPtr)

{
	SInt32								intValue;
	
	
	intValue = (SInt32)(ConvertATRealtoReal (ATrealPtr) + 0.5);
	
	return (intValue);

}	// end "ConvertATRealtoInt" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double ConvertATRealtoReal
//
//	Software purpose:	This routine converts an IBM PC-AT real number
//							to a Motorola real number (ThinkC double).
//
//							04/17/2000: Allowed for negative values
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:			ReadERDAS_staFile
//
//	Coded By:			Larry L. Biehl			Date: 11/02/1988
//	Revised By:			Larry L. Biehl			Date: 04/17/2000

double ConvertATRealtoReal (
				CharPtr								ATrealPtr)

{
	double								MOTrealNumber,
											onePlusFraction;
	
	SInt16								actualExponent;
	
	SInt32								exponent,
											fraction,
											sign,
											temp,
											workNumber;
	
	
	if (gSwapBytesFlag)
		{		
		memcpy ((Ptr)&workNumber, (Ptr)ATrealPtr, 4);
		
				// Set Motorola real number to -1.  If the number is not changed		
				// then a -1 indicates an error.  This assumes that there will be		
				// no TRUE -1's for this application.											
				
		MOTrealNumber = -1;
		
		sign = (workNumber & 0x00000080);

		exponent	= 0;
		exponent = (workNumber & 0x0000007f) << 1;
		temp = workNumber & 0x00008000;
		exponent = exponent | (temp>>15);
		
		if (exponent > 0 && exponent < 255)
			actualExponent = (SInt16)(exponent - 127);
			
		else	// exponent == 0 || exponent == 255
			actualExponent = (SInt16)exponent;
	
				// Make certain that sign does not get shifted right.						
				
		fraction = 0;
		temp = workNumber & 0xff000000;
		fraction = fraction | ((temp>>24) & 0x000000ff);
	
		temp = workNumber & 0x00ff0000;
		fraction = fraction | (temp>>8);
	
		temp = workNumber & 0x00007f00;
		fraction = fraction | (temp<<8);
	
		if (exponent == 0 && fraction == 0)  
			MOTrealNumber = 0;
		
		else	// exponent != 0 || fraction != 0  
			{
			onePlusFraction = 1. + (double)fraction/8388608.;
			MOTrealNumber = ldexp (onePlusFraction, actualExponent);
			
			if (sign != 0)
				MOTrealNumber = - MOTrealNumber;

			}	// end "else exponent != 0 || fraction != 0" 
				
		return (MOTrealNumber);
		
		}	// end "if (gSwapBytesFlag)"
		
	else	// !gSwapBytesFlag
		{
		float		floatValue; 
		
		
		memcpy ((Ptr)&floatValue, (Ptr)ATrealPtr, 4);
		return (floatValue);
		
		}	// end "else !gSwapBytesFlag"

}	// end "ConvertATRealtoReal"



//-----------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double ConvertIBM370RealtoReal
//
//	Software purpose:	This routine converts an IBM 370 real number
//							to a Motorola real number (ThinkC double).
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:			ReadERDAS_staFile		
//
//	Coded By:			Larry L. Biehl			Date: 02/18/1993
//	Revised By:			Larry L. Biehl			Date: 12/05/2011

double ConvertIBM370RealtoReal (
				SInt32*								IBM370realPtr)

{
	double								doubleFraction,
											MOTrealNumber;
	
	SInt32								exponent,
											fraction,
											workNumber;

	UInt32								sign;
	
	SInt16								actualExponent;
	
			
			// Swap the bytes if needed.
			
	if (gSwapBytesFlag)
		Swap4Bytes ((HUInt32Ptr)IBM370realPtr, 1);
	
			// Set Motorola real number to -1.  If the number is not changed		
			// then a -1 indicates an error.  This assumes that there will be		
			// no TRUE -1's for this application.											
			
	MOTrealNumber = -1;
	
	workNumber = *IBM370realPtr;
	
	sign = (workNumber & 0x80000000);
	if (sign == 0)
		{
		exponent	= 0;
		exponent = (workNumber & 0x7f000000) >> 24;
		actualExponent = (SInt16)(exponent - 64);
		
				// Take into account that the base used is actually 16 not 2.			
				
		actualExponent *= 4;
	
				// Make certain that sign does not get shifted right.						
				
		fraction = 0;
		fraction = workNumber & 0x00ffffff;
	
		if (exponent == 0 && fraction == 0)  
			MOTrealNumber = 0;
		
		else	// exponent != 0 || fraction != 0
			{
			doubleFraction = (double)fraction/16777216.;
			MOTrealNumber = ldexp (doubleFraction, actualExponent);

			}	// else exponent != 0 || fraction != 0
			
		}	// end "if (sign == 0)" 
	
	return (MOTrealNumber);

}	// end "ConvertIBM370RealtoReal" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt32 ConvertRealAT
//
//	Software purpose:	This routine converts an real number in Motorola
//							real (double format) to  PC-AT real number format.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None	
//
// Called By:			LoadErdasHeader in SFileIO.cpp
//							CreateSTASupportFile in SHistogram.cpp						
//										
//	Global Data:
//
//	Coded By:			Larry L. Biehl			Date: 08/29/1988
//	Revised By:			Larry L. Biehl			Date: 01/19/2001

SInt32 ConvertRealAT (
				double								realValue)

{
	double								mantissa,
											doubleFraction;
	
	int									actualExponent;
	
	SInt32								exponent,
											fraction,
											temp,
											workNumber;
											
	Boolean								negativeFlag;
	
	
	if (gSwapBytesFlag)
		{
				// Determine whether real value is negative or not.
		
		negativeFlag = FALSE;
		if (realValue < 0)
			negativeFlag = TRUE;
		
				// Get mantissa and exponent for base 2 number								
		
		realValue = fabs (realValue);		
		mantissa = frexp (realValue, &actualExponent);
		
				// Convert mantissa from range of 0.5 - 1.0 to 1.0 - 2.0.  				
				// Don't forget to adjust the exponent accordingly							
				//	Also work with only the fractional part of the mantissa.	
				// 	Move to before conversion of exponent on 11/28/2000 by Larry Biehl.
				//		Ran into problem with a number like 0.6 with it the other way.			
		
		if (mantissa > 0)
			{		
			mantissa *= 2;
			mantissa = mantissa - 1;
			actualExponent--;
		
					// Convert exponent to biased exponent between 0 and 255.				
					// If the actual exponent is 0, the biased exponent is also 0.			
					
			if (actualExponent >= -126 && actualExponent <= 127)
				exponent = actualExponent + 127;
				
			else if (actualExponent < -126)	// actualExponent < -126
				exponent = 0;
				
			else	// actualExponent > 127
				exponent = 255;
			
			}	// end "if (mantissa > 0)"
			
		else	// mantissa == 0
			exponent = 0;
		
				// Get the fraction of the mantissa to 23 bits
				// Added code to make sure 0.5 used for rounding was not larger than
				// 8388608 which caused problems. Number when read back was half of
				// what it should be (3/24/2006 Larry Biehl).								
		
		doubleFraction = mantissa * 8388608.;
		if (doubleFraction + 0.5 < 8388608)
			doubleFraction += 0.5;
			
		fraction = (SInt32)doubleFraction;
		
				// Load in the sign flag															
				
		workNumber = 0;
		if (negativeFlag)  
			workNumber = workNumber | 0x00000080;
		
				// Load the exponent in																
				
		temp = (exponent & 0x000000fe) >> 1;
		workNumber = workNumber | temp;
		
		temp = (exponent & 0x00000001) << 15;
		workNumber = workNumber | temp;
		
				// Load the fraction in																
				
		temp = (fraction & 0x000000ff) << 24;
		workNumber = workNumber | temp;
		
		temp = (fraction & 0x0000ff00) << 8;
		workNumber = workNumber | temp;
		
		temp = (fraction & 0x007f0000) >> 8;
		workNumber = workNumber | temp;
		
		}	// end "gSwapBytesFlag"
		
	else	// !gSwapBytesFlag
		{
		float floatValue = (float)realValue;
		memcpy ((Ptr)&workNumber, (Ptr)&floatValue, 4);
		
		}	// end "else !gSwapBytesFlag"
	
			// Return the converted number.  It is now in PC-AT floating point	
			// format (Real)																		
			
	return (workNumber);

}	// end "ConvertRealAT"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CopyFileStream
//
//	Software purpose:	The purpose of this routine is to intialize a new file stream
//							structure using the information from another input file
//							stream structure. The new file stream structure is left
//							with no file being opened even if the input file stream
//							represents a file that is opened.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 05/30/1997
//	Revised By:			Larry L. Biehl			Date: 04/09/2020

void CopyFileStream (
				CMFileStream*						newFileStreamPtr,
				CMFileStream*						oldFileStreamPtr)

{  		
	if (newFileStreamPtr != NULL && oldFileStreamPtr != NULL)
		{   
				// Now copy the information needed so that the new file stream 
				// represents an opened file in the same state as that in the
				// input file stream.
				                      
		#if defined multispec_mac                                                         		
			newFileStreamPtr->vRefNum = oldFileStreamPtr->vRefNum;
			newFileStreamPtr->parID = oldFileStreamPtr->parID;
			CopyPToP (newFileStreamPtr->fileName, oldFileStreamPtr->fileName);
			newFileStreamPtr->creator = oldFileStreamPtr->creator;
			newFileStreamPtr->type = oldFileStreamPtr->type;    
			newFileStreamPtr->fSSpecFlag = oldFileStreamPtr->fSSpecFlag;
			newFileStreamPtr->refNum = oldFileStreamPtr->refNum;
			
			newFileStreamPtr->fsRef = oldFileStreamPtr->fsRef;
			newFileStreamPtr->parentFSRef = oldFileStreamPtr->parentFSRef;
			newFileStreamPtr->uniFileName = oldFileStreamPtr->uniFileName;
			//CopyPToP (newFileStreamPtr->wideCharFileName,
			//										oldFileStreamPtr->wideCharFileName);
			newFileStreamPtr->pathLength = oldFileStreamPtr->pathLength;
			newFileStreamPtr->fSRefFlag = oldFileStreamPtr->fSRefFlag;
		#endif	// defined multispec_mac
		
      #if defined multispec_win
         TBYTE* oldFilePathPtr = 
						(TBYTE*)oldFileStreamPtr->GetFilePathPPtr (kReturnUnicode);
			newFileStreamPtr->SetFilePath (oldFilePathPtr,
														FALSE);
			newFileStreamPtr->mCreator = oldFileStreamPtr->mCreator;
			newFileStreamPtr->mFileType = oldFileStreamPtr->mFileType; 
         newFileStreamPtr->m_hFile = oldFileStreamPtr->m_hFile;
			newFileStreamPtr->mWidePathLength = oldFileStreamPtr->mWidePathLength;
			newFileStreamPtr->mUTF8PathLength = oldFileStreamPtr->mUTF8PathLength;
		#endif	// defined multispec_win
		
      #if defined multispec_wx
         WideFileStringPtr	oldFilePathPtr = 
					(WideFileStringPtr)oldFileStreamPtr->GetFilePathPPtr (kReturnUnicode);
			newFileStreamPtr->SetFilePath (oldFilePathPtr,
														FALSE);
			newFileStreamPtr->mCreator = oldFileStreamPtr->mCreator;
			newFileStreamPtr->mFileType = oldFileStreamPtr->mFileType; 
			newFileStreamPtr->mWidePathLength = oldFileStreamPtr->mWidePathLength;
			newFileStreamPtr->mUTF8PathLength = oldFileStreamPtr->mUTF8PathLength;
 		#endif	// defined multispec_wx 
		
		}	// end "if (newFileStreamPtr != NULL && ..."

}	// end "CopyFileStream" 

 
		
Boolean CopyFileStream (
				FileInfoPtr							inputFileInfoPtr,
				Handle*								outputFileStreamHandlePtr,
				CMFileStream**						outputFileStreamPtrPtr) 

{  
	CMFileStream						*inputFileStreamPtr,
											*newFileStreamPtr;
					
	Boolean								returnFlag = FALSE;
	
	
	inputFileStreamPtr = GetFileStreamPointer (inputFileInfoPtr);
			
	if (inputFileStreamPtr != NULL)
		{   
				// Now copy the information needed so that the new file stream 
				// represents an opened file in the same state as that in the
				// input file stream.
				                      
		#if defined multispec_mac                                                         		
			if (outputFileStreamHandlePtr != NULL)
				{
				if (*outputFileStreamHandlePtr == NULL)
					*outputFileStreamHandlePtr = MNewHandle (sizeof (CMFileStream));
					
				if (*outputFileStreamHandlePtr != NULL)
					{
					newFileStreamPtr = (CMFileStream*)GetHandlePointer (
															*outputFileStreamHandlePtr,
															kLock);
															
					newFileStreamPtr->vRefNum = inputFileStreamPtr->vRefNum;
					newFileStreamPtr->parID = inputFileStreamPtr->parID;
					CopyPToP (newFileStreamPtr->fileName, 
										inputFileStreamPtr->fileName);
					newFileStreamPtr->creator = inputFileStreamPtr->creator;
					newFileStreamPtr->type = inputFileStreamPtr->type;    
					newFileStreamPtr->fSSpecFlag = inputFileStreamPtr->fSSpecFlag;
					newFileStreamPtr->refNum = inputFileStreamPtr->refNum;
			
					newFileStreamPtr->fsRef = inputFileStreamPtr->fsRef;
					newFileStreamPtr->parentFSRef = inputFileStreamPtr->parentFSRef;
					newFileStreamPtr->uniFileName = inputFileStreamPtr->uniFileName;
					//CopyPToP (newFileStreamPtr->wideCharFileName,
					//								inputFileStreamPtr->wideCharFileName);
					newFileStreamPtr->pathLength = inputFileStreamPtr->pathLength;
					newFileStreamPtr->fSRefFlag = inputFileStreamPtr->fSRefFlag;
					
							// Make sure that the refNum in the input file stream does
							// does not now refer to an open file. We do not want
							// two structures to refer to the same open file.
							
					inputFileStreamPtr->refNum = 0;
					
					CheckAndUnlockHandle (*outputFileStreamHandlePtr);
					
					if (outputFileStreamPtrPtr != NULL)
						*outputFileStreamPtrPtr = NULL;
		         
		         returnFlag = TRUE;
					
					}	// end "if (*outputFileStreamHandlePtr != NULL)"
				
				}	// end "if (outputFileStreamHandlePtr != NULL)"
		#endif	// defined multispec_mac
		
		#if defined multispec_wx
			if (outputFileStreamPtrPtr != NULL)
				{
				if (*outputFileStreamPtrPtr == NULL)
					*outputFileStreamPtrPtr = new CMFileStream;
					
				if (*outputFileStreamPtrPtr != NULL)
					{
					newFileStreamPtr = *outputFileStreamPtrPtr;
					
		         WideFileStringPtr	oldFilePathPtr = (WideFileStringPtr)
											inputFileStreamPtr->GetFilePathPPtr (kReturnUnicode);
					newFileStreamPtr->SetFilePath (oldFilePathPtr,
																FALSE);
					newFileStreamPtr->mCreator = inputFileStreamPtr->mCreator;
					newFileStreamPtr->mFileType = inputFileStreamPtr->mFileType; 
					newFileStreamPtr->mWidePathLength =
															inputFileStreamPtr->mWidePathLength;
 		         returnFlag = TRUE;
		         
		      	}	// end "if (*outputFileStreamPtrPtr != NULL)"
         
         	}	// end "if (outputFileStreamPtrPtr != NULL)"
		#endif	// defined multispec_wx 
		
		#if defined multispec_win
			if (outputFileStreamPtrPtr != NULL)
				{
				if (*outputFileStreamPtrPtr == NULL)
					*outputFileStreamPtrPtr = new CMFileStream;
					
				if (*outputFileStreamPtrPtr != NULL)
					{
					newFileStreamPtr = *outputFileStreamPtrPtr;
					
		         TBYTE* oldFilePathPtr = 
								(TBYTE*)inputFileStreamPtr->GetFilePathPPtr (kReturnUnicode);              
					newFileStreamPtr->SetFilePath (oldFilePathPtr,
																FALSE);
					newFileStreamPtr->mCreator = inputFileStreamPtr->mCreator;
					newFileStreamPtr->mFileType = inputFileStreamPtr->mFileType; 
 		         //newFileStreamPtr->m_hFile = inputFileStreamPtr->m_hFile;
							// Do not allow the two classes to have the same file reference
					newFileStreamPtr->m_hFile = CFile::hFileNull;
					newFileStreamPtr->mWidePathLength =
															inputFileStreamPtr->mWidePathLength;
					newFileStreamPtr->mUTF8PathLength = 
															inputFileStreamPtr->mUTF8PathLength;
 		         returnFlag = TRUE;
		         
		      	}	// end "if (*outputFileStreamPtrPtr != NULL)"
         
         	}	// end "if (outputFileStreamPtrPtr != NULL)"	
		#endif	// defined multispec_win 
		
		}	// end "if (inputFileStreamPtr != NULL)"
		
	return (returnFlag);

}	// end "CopyFileStream"


/*
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CopyWideStringToUnicodeStringInFileStream
//
//	Software purpose:	The purpose of this routine is to copy the wide character
//							string in the FileStream stucture to the unicode character
//							string in the FileStream.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	Error code for file operations.
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/10/2017
//	Revised By:			Larry L. Biehl			Date: 03/10/2017

void CopyWideStringToUnicodeStringInFileStream (
				CMFileStream* 						fileStreamPtr)

{
	#if defined multispec_mac
				// Copy wide character file name string to unichar name string.

		UniChar*								fromPtr;
		int									count,
												indexIn = 0;

		int									numberCharacters;
		
		
		numberCharacters = fileStreamPtr->wideCharFileName[0];
		
		fromPtr = (UniChar*)&fileStreamPtr->wideCharFileName[1];
		for (count=0; count<numberCharacters; count++)
			{
			fileStreamPtr->uniFileName.unicode[indexIn++] = *fromPtr;
			fromPtr += 2;
			
			}	// end "for (count=0; count<numberCharacters; count++)"

		fileStreamPtr->uniFileName.length = numberCharacters;
	#endif

}	// end "CopyWideStringToUnicodeStringInFileStream"
*/


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 CreateNewFile
//
//	Software purpose:	The purpose of this routine is to create a new file
//							on the specified volume and open the file.  If the
//							file already exists, clear the old file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	Error code for file operations.
//
// Called By:			GetThematicSupportFileToCreate in SFileIO.cpp
//							PutFile in SFileIO.cpp
//							WriteArcViewWorldFile in SSaveWrite.cpp
//							WriteArcViewHeaderFile in SSaveWrite.cpp
//							FS_make_cor_mean_std in SStatisticsAlgorithms.cpp
//							CreateResultsDiskFile in SUtility.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/23/1990
//	Revised By:			Larry L. Biehl			Date: 03/16/2015

SInt16 CreateNewFile (
				CMFileStream* 						fileStreamPtr, 
				SInt16								vRefNum, 
				SInt32								creator,
				SInt16								messageCode,
				Boolean								replaceFlag)

{
	SInt16								errCode = noErr;
	
	
	#if defined multispec_mac
		FileInfo*							finderFileInfoPtr;
		FSCatalogInfo						catalogInformation;
	
		SInt32								parID;
	
		if (fileStreamPtr->fSSpecFlag)
			parID = fileStreamPtr->parID;
	
		else	// !fileStreamPtr->fSSpecFlag
			parID = 0;
	
				// Create the file on the selected volume
				
		if (fileStreamPtr->fSRefFlag)
			{					
			fileStreamPtr->pathLength = 0;
	
	
			finderFileInfoPtr = (FileInfo*)&catalogInformation.finderInfo;
			finderFileInfoPtr->fileType = fileStreamPtr->type;
			finderFileInfoPtr->fileCreator = creator;
			finderFileInfoPtr->finderFlags = 0;
			finderFileInfoPtr->location.h = 0;
			finderFileInfoPtr->location.v = 0;
			finderFileInfoPtr->reservedField = 0;
			/*
			catalogInformation.volume = vRefNum;
		
			errCode = FSFindFolder (vRefNum,
											OSType folderType,
											kDontCreateFolder,
											&fileStreamPtr->parentFSRef);
			
					// Make sure that both UTF8 and unicode strings exist
			
			UpdateFileNameInformation (fileStreamPtr, NULL);
					
					// Copy wide character file name string to unichar name string.
					
			CopyWideStringToUnicodeStringInFileStream (fileStreamPtr);
			*/
			errCode = FSCreateFileUnicode (&fileStreamPtr->parentFSRef, 
													fileStreamPtr->uniFileName.length,
													fileStreamPtr->uniFileName.unicode,
													kFSCatInfoFinderInfo, 
													&catalogInformation, 
													&fileStreamPtr->fsRef, 
													NULL);
			
			if (errCode == dupFNErr)
				{
				errCode = FSMakeFSRefUnicode (&fileStreamPtr->parentFSRef,  
															fileStreamPtr->uniFileName.length,
															fileStreamPtr->uniFileName.unicode,
															kTextEncodingUnicodeDefault, 
															&fileStreamPtr->fsRef);
															
				if (errCode == noErr)
					errCode = dupFNErr;
															
				}	// end "if (errCode == dupFNErr)"
			
			}	// end "if (fileStreamPtr->fSRefFlag)"
		
		else	// !fileStreamPtr->fSRefFlag		
			errCode = HCreate (vRefNum, 
	                           parID,
										fileStreamPtr->fileName, 
										(OSType)creator,
										(OSType)fileStreamPtr->type);

									
		if (errCode != dupFNErr && messageCode == kErrorMessages)
			IOCheck (errCode, (CharPtr)&fileStreamPtr->fileName);
			
		if (errCode != noErr && errCode != dupFNErr)	
																						return (errCode);
																				
		if (!replaceFlag && errCode == dupFNErr)	
																						return (errCode);
		
		errCode = noErr;
			
		if (fileStreamPtr->fSRefFlag)
			errCode = SetFileSizeToZero (fileStreamPtr);
			
		else	// !fileStreamPtr->fSRefFlag
			{					
			if (!fileStreamPtr->fSSpecFlag && !fileStreamPtr->fSRefFlag)
				{											
				errCode = FSMakeFSSpec (vRefNum,
												0,
												fileStreamPtr->fileName,
												&gFileFSSpec);
					
				if (errCode != noErr)	
					{
					if (messageCode == kErrorMessages)
						IOCheck (errCode, (CharPtr)&fileStreamPtr->fileName);
																						return (errCode);
																					
					}	// end "if (errCode != noErr)"
				
				vRefNum = gFileFSSpec.vRefNum;
				parID = gFileFSSpec.parID;
												
				}	// end "if (!fileStreamPtr->fSSpecFlag && ..."
			
					// Open the file ready for write mode at the beginning.					
														
			fileStreamPtr->vRefNum = vRefNum;
			fileStreamPtr->parID = parID;	
			fileStreamPtr->fSSpecFlag = TRUE;						
			errCode = SetFileSizeToZero (fileStreamPtr);
				
					// Make certain that the creator and file type are correct.				
					
			if (errCode == noErr)
				{	
				errCode = HGetFInfo (
										vRefNum, parID, fileStreamPtr->fileName, &gFinderInfo);
				
				if (errCode == noErr && (gFinderInfo.fdCreator != creator || 
													gFinderInfo.fdType != fileStreamPtr->type))
					{
					gFinderInfo.fdCreator = creator;
					gFinderInfo.fdType = fileStreamPtr->type;
				
					errCode = HSetFInfo (
										vRefNum, parID, fileStreamPtr->fileName, &gFinderInfo);
					
					}	// end "if (errCode == noErr)" 
				
				if (messageCode == kErrorMessages)	
					IOCheck (errCode, (CharPtr)&fileStreamPtr->fileName);
				
				}	// end "if (errCode == noErr)"
			
			}	// end "else !fileStreamPtr->fSRefFlag"
	#endif	// defined multispec_mac
	
	#if defined multispec_win || defined multispec_wx
		UpdateFileNameInformation (fileStreamPtr, NULL);
		errCode = fileStreamPtr->MCreateNewFile (replaceFlag);
	#endif	// defined multispec_win || defined multispec_wx
	
	return (errCode);

}	// end "CreateNewFile"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CreateThematicSupportFile
//
//	Software purpose:	This routine creates a thematic support file which will be 
//							written out as either an ERDAS .trl file or an ArcView style
//							.clr file. If the classNameCode is 'kFromDescriptionCode', then 
//							this is called from the Reformat-Change Image File Format
//							processor. The class names and colors come from those
//							currently being used in the active image window.
//
//							AreasToThematicFileControl (paletteID)
//							CovertMultispectralToThematic (paletteHandle)
//							CreateTrailerFiles (paletteID)
//							ChangeFormatToBILorBISorBSQ (paletteHandle)
//							WriteThematicClassesAs (colorSpecPtr)
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			AreasToThematicFileControl in SFieldsToThematicFile.cpp
//							CovertMultispectralToThematic in SFieldsToThematicFile.cpp
//							ENVI_ASCII_ROI_ToThematicFileControl in SFieldsToThematicFile.cpp
//							CreateTrailerFiles in SClassify.cpp
//							CreateClusterMaskFile in SCluster.cpp
//							MosaicImagesSideBySide in Mosaic.c
//							RectifyImage in SRectification.cpp
//							ReprojectImage in SRectification.cpp
//							ChangeFormatToBILorBISorBSQ in SReformatChangeImageFileFormat.cpp
//							ShapeToThematicFileControl in SShapeToThematic.cpp
//							WriteThematicClassesAs in SSaveWrite.cpp
//							
//	Coded By:			Larry L. Biehl			Date: 02/21/1990
//	Revised By:			Larry L. Biehl			Date: 04/15/2020

Boolean CreateThematicSupportFile (
				FileInfoPtr							gisFileInfoPtr, 
				ShapeInfoPtr						shapeInfoPtr,
				UInt16								numberClasses, 
				SInt16*								classPtr,
				UInt16								numberListClasses,  
				ColorSpec*							inputColorSpecPtr,
				CMPaletteInfo						paletteHandle,
				UInt16*								paletteIndexPtr,
				SInt16								paletteType,
				UInt16								numberPaletteEntriesToRead, 
				SInt16								classNameCode, 
				SInt16								trailerCode,
				SInt16								thematicListType,
				SInt16								collapseClassCode,
				UInt32								supportFileType)

{
	CMFileStream						trailerStream;
											
	Boolean								*listAllGroupInfoPtr = NULL;
	EchoClassifierVarPtr				echoClassifierVarPtr;
	CMFileStream*						trailerStreamPtr;
	ColorSpec*							colorSpecPtr;
	
	RGBCharColorPtr					classColorTablePtr = NULL,
											groupColorTablePtr = NULL;	
	
	char									*classNameTablePtr = NULL,
											*groupNameTablePtr = NULL,
											*tempClassNameTablePtr;
											
	SInt16								*classToGroupPtr = NULL;
											
	UInt16								*classSymbolPtr,
											*groupToPalettePtr;
											
	CTabHandle							groupCTableHandle = NULL;
	Handle								displaySpecsH;
										
	SInt32								classNumber;
	
	UInt32								count,
											index;
	
	SInt16								errCode,
											nameLength,
											numberProjectClasses,
											stringLength,
											statusStringNumber;
										
	UInt16								classIndex,
											numberGroups,
											numberSupportFileClasses;
										
	Boolean								continueFlag,
											disposeGroupCTableHandleFlag,
											promptFlag,
											writeClassNamesFlag;
	
	
	if (gisFileInfoPtr == NULL)
																							return (FALSE);
	
			// Do not create a trailer file if there are more than the class		
			// limit.																				
			
	if (numberClasses > gClassListLimit)
																							return (TRUE);
	
			// Get buffer to load the class names into. Allow for minimum
			// of 256 classes. Allow for background classes being created for some
			// situations.
	
	count = MAX (256, numberClasses+1);				
	classNameTablePtr = (char*)MNewPointer (count*32);

			// Get buffer to load the class colors into. Allow for minimum
			// of 256 classes. Allow for background classes being created for some
			// situations.
			
	if (classNameTablePtr != NULL)
		classColorTablePtr = (RGBCharColorPtr)MNewPointer (count*sizeof (RGBCharColor));

			// Get buffer to load the class group colors into. Allow for minimum
			// of 256 classes. Allow for background classes being created for some
			// situations.
	
	if (classColorTablePtr != NULL)
		groupColorTablePtr = (RGBCharColorPtr)MNewPointer (count*sizeof (RGBCharColor));
		
	if (groupColorTablePtr != NULL)
		listAllGroupInfoPtr = (Boolean*)MNewPointer (count);
		
	if (listAllGroupInfoPtr == NULL)
		{
		CheckAndDisposePtr ((Ptr)classNameTablePtr);
		CheckAndDisposePtr ((Ptr)classColorTablePtr);
		CheckAndDisposePtr ((Ptr)groupColorTablePtr);
																							return (FALSE);
																				
		}	// end "if (listAllGroupInfoPtr == NULL)"
	
	numberProjectClasses = 0;
	if (gProjectInfoPtr != NULL)
		numberProjectClasses = gProjectInfoPtr->numberStatisticsClasses;
		
			// Note that the number of colors in the color table is limited to 256
			// colors. If there are more than 256 classes, then the colors for them
			// will repeated.
		
	colorSpecPtr = inputColorSpecPtr;
	continueFlag = GetClassColorTable (gisFileInfoPtr, 
													numberClasses, 
													classPtr,
													numberListClasses,
													&numberSupportFileClasses,  
													&colorSpecPtr,
													paletteHandle,
													paletteIndexPtr,
													paletteType,
													numberPaletteEntriesToRead, 
													classNameCode,
													thematicListType,
													collapseClassCode);
	
	if (continueFlag)
		{
				// Get the pointer to the file stream block and initialize it.			
			
		trailerStreamPtr = &trailerStream;
		InitializeFileStream (trailerStreamPtr);
		
		promptFlag = FALSE;
		if (inputColorSpecPtr != NULL)
			promptFlag = TRUE;
		
		errCode = GetThematicSupportFileToCreate (gisFileInfoPtr, 
																trailerStreamPtr, 
																supportFileType,
																promptFlag);
		
		continueFlag = FileExists (trailerStreamPtr);
		
		}	// end "if (continueFlag)"
	
			// Continue if memory not full													
			
	if (continueFlag && errCode == noErr)
		{
				// Put writing trailer file message in status dialog.	

		statusStringNumber = 0;
		if (gStatusIDNumber == kUpdateStatsInfoID)
			statusStringNumber = IDC_Status11;
		else if (gStatusIDNumber == kShortStatusInfoID)
			statusStringNumber = IDC_ShortStatusText;

		if (statusStringNumber != 0)
			{
			UInt16	stringIndex = IDS_Support_trl;
			if (supportFileType == kICLRFileType)
				stringIndex = IDS_Support_clr;
				
			MGetString (gTextString, kClassifyStrID, stringIndex);
			
			LoadDItemString (gStatusDialogPtr, statusStringNumber, (Str255*)gTextString);
		
			CheckSomeEvents (updateMask);

			}	// end "if (statusStringNumber != 0)"
	
				// Now load the class color table to be used for the support file.
				
		LoadErdasTRLClassColorBuffer (gisFileInfoPtr,
												colorSpecPtr,
												classPtr,
												numberListClasses,
												paletteIndexPtr, 
												numberPaletteEntriesToRead,
												classColorTablePtr,
												collapseClassCode,
												classNameCode);
					
				// Load the class names into the class name table.								
		
		tempClassNameTablePtr = classNameTablePtr;	
		if (trailerCode & kClassNames)
			{
			nameLength = 0;
			if (classNameCode == 0 ||
					classNameCode == kClassifyFileCode ||
						classNameCode == kProbFormatCode ||
								classNameCode == kFromShapeToThematicCode ||
										classNameCode == kClusterMaskCode)
				{
				BlockMoveData ("background", tempClassNameTablePtr, 10);
				nameLength = 10;
				
				}	// end "if (classNameCode == 0 || classNameCode == ..."
				
			else if (classNameCode & (kEchoFieldsCode + kEchoClassesCode))
				{
				BlockMoveData ("threshold(", tempClassNameTablePtr, 10);

				echoClassifierVarPtr = (EchoClassifierVarPtr)GetHandlePointer (
															gClassifySpecsPtr->echoClassifierVarH,
															kLock);
					
				if (echoClassifierVarPtr->fixed_homogeneityThreshold_option)
					snprintf (&tempClassNameTablePtr[10], 256, "L");
					
				else	// !...fixed_homogeneityThreshold_option 
					snprintf (&tempClassNameTablePtr[10], 256, "P");
				
				if (classNameCode & kEchoFieldsCode)
					{
					stringLength = sprintf (&tempClassNameTablePtr[11], 
													"%.1f,A%.1f)", 
													echoClassifierVarPtr->homogeneityThreshold,
													echoClassifierVarPtr->annexationThreshold);
					nameLength = 11 + stringLength;
						
					}	// end "if (classNameCode & kEchoFieldsCode)"
						
				else	// !(classNameCode & kEchoFieldsCode) 
					{
					stringLength = sprintf ((char*)&tempClassNameTablePtr[11], 
													"%.1f)~", 
													echoClassifierVarPtr->homogeneityThreshold);
					nameLength = 11 + stringLength;
						
					}	// end "else !(classNameCode & kEchoFieldsCode)"
				
				CheckAndUnlockHandle (gClassifySpecsPtr->echoClassifierVarH);
				
				}	// end "else if (... & (kEchoFieldsCode + kEchoClassesCode))" 
								
			tempClassNameTablePtr[nameLength] = 0;
			tempClassNameTablePtr += 32;
			numberSupportFileClasses++;
			          
			if (classPtr != NULL)
				{
				classIndex = 0;
				for (classNumber=1; classNumber<=numberProjectClasses; classNumber++)
					{
					nameLength = LoadClassName ((SInt16)classNumber, 
															(SInt16)numberListClasses, 
															classPtr, 
															&classIndex, 
															(char*)tempClassNameTablePtr,
															0);
													
					tempClassNameTablePtr[nameLength] = 0;
					tempClassNameTablePtr += 32;
				
					}	// end "for (classNumber=1; classNumber<=..."
					
				}	// end "if (classPtr != NULL)" 
				
			else	// classPtr == NULL
				{
				if (classNameCode == kProbFormatCode)
					{
					if (numberClasses == gNumberProbabilityClasses)
						{
								// Likelihood probabilities.
								
						double rejectionPercent = 0;
						double rejectionStep = 1e-13;
						
						for (index=1; index<=gNumberProbabilityClasses; index++)
							{
							if (rejectionPercent < 0.1)
								nameLength = sprintf (tempClassNameTablePtr, 
																"%.0e - %.0e", 
																rejectionPercent, 
																rejectionPercent+rejectionStep);
										
							else	// rejectionPercent >= 0.1 
								nameLength = sprintf (tempClassNameTablePtr, 
																"%3.1f-%3.1f", 
																rejectionPercent, 
																rejectionPercent+rejectionStep);
										
							rejectionPercent += rejectionStep;
										
							if (index < 13)
								rejectionStep *= 10;
								
							else	// index >= 14 
								{
								if (index == 13)
									rejectionStep = .1;
									
								if (index == 22)
									rejectionStep = 1.;
									
								}	// end "else index >= 13" 
							
							tempClassNameTablePtr[nameLength] = 0;
							tempClassNameTablePtr += 32;
							
							}	// end "for (index=1; index<=..."
							
						}	// end "if (numberClasses == gNumberProbabilityClasses)"
						
					else if (numberClasses == 102)
						{
								// CEM values
						
						double CEMValue = 0;
						double CEMStep = 0.02;
						
						for (index=1; index<=numberClasses; index++)
							{
							if (index == 1)
								nameLength = sprintf (tempClassNameTablePtr, 
																"<0.00");
										
							else if (index <= 101) 
								{
								nameLength = sprintf (tempClassNameTablePtr, 
																"%3.2f-%3.2f", 
																CEMValue, 
																CEMValue+CEMStep);
								CEMValue += CEMStep;
								
								}	// end "else if (index <= 102)"
								
							else	// index > 101
								nameLength = sprintf (tempClassNameTablePtr, 
																">2.00");
								
							tempClassNameTablePtr[nameLength] = 0;	
							tempClassNameTablePtr += 32;
							
							}	// end "for (index=1; index<=..."
						
						}	// end "if (numberClasses == 102)"
						
					else if (numberClasses == 91)
						{
								// Correlation values
						
						UInt32 correlationValue = 90;
						UInt32 correlationStep = 1;
						
						for (index=1; index<=numberClasses; index++)
							{
							if (index == 1)
								nameLength = snprintf (tempClassNameTablePtr,
																5,
																">=90");
										
							else	// index > 1 
								{
								nameLength = sprintf (tempClassNameTablePtr,
																(char*)"%2d - <%2d",
																(int)(correlationValue-correlationStep),
																(int)correlationValue);
								correlationValue -= correlationStep;
								
								}	// end "else index > 1"
								
							tempClassNameTablePtr[nameLength] = 0;
							tempClassNameTablePtr += 32;
							
							}	// end "for (index=1; index<=..."
						
						}	// end "else if (numberClasses == 91)"
					
					else	// handle for general case; kNN for now.
						{
								// k nearest neighbor
						
						for (index=1; index<=numberClasses; index++)
							{
							nameLength = sprintf (tempClassNameTablePtr,
															"%d NNs", index);
							
							tempClassNameTablePtr[nameLength] = 0;
							tempClassNameTablePtr += 32;
							
							}	// end "for (index=1; index<=..."
						
						}	// end "else handle for general case; kNN for now"
					
					}	// end "if (classNameCode == kProbFormatCode)" 
					
				else if (classNameCode == kEchoFieldsCode)
					{
					for (index=1; index<=numberClasses; index++)
						{
						nameLength = sprintf ((char*)tempClassNameTablePtr, 
														"field %d", 
														(int)index);
						tempClassNameTablePtr[nameLength] = 0;
						tempClassNameTablePtr += 32;
					
						}	// end "for (index=1; index<=numberClasses; index++)" 
					
					}	// end "else if (classNameCode == kEchoFieldsCode)"

				else if ((classNameCode == kFromDescriptionCode) ||
																(classNameCode == kClusterMaskCode))
					{
					if (classNameCode == kFromDescriptionCode)
						{
								// Go back to location for class 0
									
						tempClassNameTablePtr -= 32;
						numberSupportFileClasses--;
						
						}	// end "if (classNameCode == kFromDescriptionCode)"
						
					LoadErdasTRLClassNameBufferFromDescriptions (
																	gisFileInfoPtr,
																	numberClasses,
																	(UCharPtr)tempClassNameTablePtr,
																	collapseClassCode);
					
					}	// end "else if ((classNameCode == kFromDescriptionCode) || ..." 

				else if (classNameCode == kFromShapeToThematicCode)
					{
							// The background class was already included in the count of the
							// number of classes by the shape to thematic processor.
							
					LoadShapeToThematicClassNames (shapeInfoPtr,
																tempClassNameTablePtr,
																numberClasses);
					/*
					for (index=1; index<=numberClasses; index++)
						{
						nameLength = sprintf ((char*)tempClassNameTablePtr, 
														"Class %ld", 
														index);
						tempClassNameTablePtr[nameLength] = 0;
						tempClassNameTablePtr += 32;
					
						}	// end "for (index=1; index<=numberClasses; index++)"
					*/					
					if (supportFileType == kICLRFileType)
						numberSupportFileClasses = numberClasses + 1;
					
					}	// end "else if (classNameCode == kFromShapeToThematicCode" 
					
				}	// end "else classPtr = NULL" 
				
			}	// end "if (trailerCode & kClassNames)"
			
		writeClassNamesFlag = (classNameCode != 0 || classPtr != NULL);
		
		if (supportFileType == kITRLFileType)	
			continueFlag = CreateTRLSupportFile (trailerStreamPtr, 
																numberSupportFileClasses,
																classColorTablePtr,
																classNameTablePtr,
																trailerCode,
																writeClassNamesFlag);
		
		else	// supportFileType == kICLRFileType
			{
			classSymbolPtr = NULL;
			if (gProcessorCode != kClusterProcessor && 
					gProcessorCode != kClassifyProcessor &&
						gProcessorCode != kSaveOutputAsProcessor)
				{
						// Get pointer to class symbol table.										
						// Remember to take into account that classSymbolPtr 					
						// points to an unsigned short int vector. Str15 is used instead 
						// of Str31.																			
								
				classSymbolPtr = (UInt16*)GetHandlePointer (
														gisFileInfoPtr->classDescriptionH, kLock);
				
				if (classSymbolPtr != NULL)
					classSymbolPtr = &classSymbolPtr[gisFileInfoPtr->numberClasses * 
																							sizeof (Str15)];
					
				}	// end "if (gProcessorCode != kClusterProcessor && ..."
				
			disposeGroupCTableHandleFlag = FALSE;
			if (gProcessorCode == kClassifyProcessor && 
					classNameCode == kProbFormatCode &&
						numberClasses == gNumberProbabilityClasses)
				{
						// Fill the group structure information so that it is ready to be
						// written to the clr file in a later step.
						// The group information is for the probability map from the 
						// classify processor for the maximum likelihood, fisher linear 
						// discriminant and echo classifier.

				continueFlag = LoadProbabilityGroupInformation (gisFileInfoPtr,  
																				classColorTablePtr,
																				&groupCTableHandle);
														
				disposeGroupCTableHandleFlag = TRUE;
				
				}	// end "if (gProcessorCode == kClassifyProcessor && ..."
				
			else	// gProcessorCode != kClassifyProcessor || ...
				{
						// Get the group color table handle if available
																				
				displaySpecsH = GetActiveDisplaySpecsHandle ();
				if (gisFileInfoPtr->numberGroups > 0 && displaySpecsH != NULL)
					{
					DisplaySpecsPtr displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
																							displaySpecsH);
																			
					groupCTableHandle = displaySpecsPtr->savedGroupCTableHandle;
					
					}	// end "if (gisFileInfoPtr->numberGroups > 0 && ..."
				
				}	// end "else gProcessorCode != kClassifyProcessor || ..."
				
					// Get group information if it exists for the clr support file type.
			
			numberGroups = 0;
			if (gisFileInfoPtr->numberGroups > 0)
				{
				groupNameTablePtr = (char*)GetHandlePointer (
																		gisFileInfoPtr->groupNameHandle,
																		kLock);
				groupToPalettePtr = (UInt16*)GetGroupToPalettePointer (gisFileInfoPtr);
															
				classToGroupPtr = GetClassToGroupPointer (gisFileInfoPtr);
				
				numberGroups = LoadGroupInformationBuffers (
																gisFileInfoPtr,
																numberSupportFileClasses,
																groupCTableHandle,
																classToGroupPtr,
																groupToPalettePtr,
																groupColorTablePtr,
																listAllGroupInfoPtr,
																classSymbolPtr,
																(UInt16)gisFileInfoPtr->numberClasses,
																gisFileInfoPtr->asciiSymbols);
				
				}	// end "if (gisFileInfoPtr->numberGroups > 0"	
				
			if (disposeGroupCTableHandleFlag)
				UnlockAndDispose ((Handle)groupCTableHandle);
				
			continueFlag = CreateCLRSupportFile (trailerStreamPtr, 
																numberSupportFileClasses,
																classColorTablePtr,
																classNameTablePtr,
																numberGroups,
																classToGroupPtr,
																groupColorTablePtr,
																groupNameTablePtr,
																listAllGroupInfoPtr,
																classSymbolPtr,
																(UInt16)gisFileInfoPtr->numberClasses,
																gisFileInfoPtr->asciiSymbols);
														
			gUpdateFileMenuItemsFlag = TRUE;
			gisFileInfoPtr->groupChangedFlag = FALSE;	
				
			}	// end "else supportFileType == kICLRFileType"
														
				// Make sure the group information is unlocked.
				
		CheckAndUnlockHandle (gisFileInfoPtr->classDescriptionH);
		CheckAndUnlockHandle (gisFileInfoPtr->groupNameHandle);
		CheckAndUnlockHandle (gisFileInfoPtr->groupTablesHandle);
	  		
		  		// Close the image statistics file											
		  				
		CloseFile (trailerStreamPtr);
		
				// If this is a new trailer file being written to disk from the 
				// File - Save Class Info As menu item, copy the file name to the 
				// gis image file information so that it will now be the default
				// class info.
				
	   if (gProcessorCode == kSaveProcessor)
	   	{
			SignedByte							handleStatus;
			
			FileStringPtr						currentTrailerFilePathPtr,
													newTrailerFilePathPtr;
	
	
			Handle windowInfoHandle = GetActiveImageWindowInfoHandle ();
	
			CMFileStream* outputFileStreamPtr = GetSupportFileStreamPointer (
																						windowInfoHandle,
																						&handleStatus);
			
			currentTrailerFilePathPtr = 
					(FileStringPtr)GetFilePathPPointerFromFileStream (outputFileStreamPtr);
			newTrailerFilePathPtr = 
					(FileStringPtr)GetFilePathPPointerFromFileStream (trailerStreamPtr);
		
			CopyPToP (currentTrailerFilePathPtr, newTrailerFilePathPtr);
			
			UnlockSupportFileStream (windowInfoHandle, handleStatus);
	   	
	   	}	// end "if (gProcessorCode == kSaveProcessor)"
		
		if (statusStringNumber != 0)
			LoadDItemString (gStatusDialogPtr, statusStringNumber, (Str255*)"\0");
			
		}	// end "if (continueFlag && errCode == noErr) 
	
			// Unlock dispose of handles to temporary buffer.							
				
	CheckAndDisposePtr ((Ptr)classNameTablePtr);
	CheckAndDisposePtr ((Ptr)classColorTablePtr);
	CheckAndDisposePtr ((Ptr)groupColorTablePtr);
	CheckAndDisposePtr ((Ptr)listAllGroupInfoPtr);
	
	if (inputColorSpecPtr == NULL)
		CheckAndDisposePtr ((Ptr)colorSpecPtr);
	
	return (continueFlag && errCode == noErr);

}	// end "CreateThematicSupportFile" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean LoadProbabilityGroupInformation
//
//	Software purpose:	This routine fills the group information for probability images
//							created by the classification processor. This routine is used
//							only for the creation of the associated clr files.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			CreateThematicSupportFile in SFileIO.cpp
//							
//	Coded By:			Larry L. Biehl			Date: 12/16/2014
//	Revised By:			Larry L. Biehl			Date: 12/17/2014	
// Revised By:       Wei-Kang Hsu         Date: 09/22/2015

Boolean LoadProbabilityGroupInformation (
				FileInfoPtr							fileInfoPtr,  
				RGBCharColorPtr					classColorTablePtr,
				CTabHandle*							cTableHandlePtr)

{
	RGBColor								theColor;
	
	ColorSpec*							colorSpecPtr;
	CTabPtr								cTablePtr;
										
	char									*groupNamePtr;
	
	SInt16								*classToGroupPtr,
											*groupToPalettePtr;
	
	Handle								groupNameHandle = NULL,
											groupTablesHandle = NULL;
											
	SInt32								colorTableIndex,
											lastColorTableIndex;
	
	UInt32								bytesNeeded,
											classIndex,
											groupIndex,
											nameLength,
											numberGroups;
										
	Boolean								changedFlag,
											continueFlag,
											groupNameChangedFlag,
											groupTableChangedFlag;
	
			
	*cTableHandlePtr = NULL;
	numberGroups = 16;
	
			// First make certain that we have memory allocated for 			
			// a class to grouping table. Allow for the same number of groups
			// as classes.
			
	continueFlag = GetGroupStructureMemory (
													MAX (fileInfoPtr->numberClasses, numberGroups),
													&groupTablesHandle,
													&groupNameHandle,
													&groupToPalettePtr,
													&groupNamePtr,
													&groupTableChangedFlag,
													&groupNameChangedFlag);
				
	if (continueFlag)
		{
		groupIndex = 0;
		groupToPalettePtr[groupIndex] = 0;
		classToGroupPtr = &groupToPalettePtr[256];
		classToGroupPtr[0] = (SInt16)groupIndex;
		
		groupIndex++;
		groupToPalettePtr[groupIndex] = 1;
		for (classIndex=1; classIndex<=22; classIndex++)
			classToGroupPtr[classIndex] = (SInt16)groupIndex;
		
		for (classIndex=23; classIndex<=26; classIndex++)
			{
			groupIndex++;
			groupToPalettePtr[groupIndex] = (SInt16)classIndex;
			classToGroupPtr[classIndex] = (SInt16)groupIndex;
			
			}	// end "for (classIndex=23; classIndex<=26; classIndex++)"
		
		groupIndex++;
		groupToPalettePtr[groupIndex] = 27;
		for (classIndex=27; classIndex<=31; classIndex++)
			classToGroupPtr[classIndex] = (SInt16)groupIndex;
		
		groupIndex++;
		groupToPalettePtr[groupIndex] = 32;
		for (classIndex=32; classIndex<=41; classIndex++)
			classToGroupPtr[classIndex] = (SInt16)groupIndex;
		
		groupIndex++;
		groupToPalettePtr[groupIndex] = 42;
		for (classIndex=42; classIndex<=51; classIndex++)
			classToGroupPtr[classIndex] = (SInt16)groupIndex;
		
		groupIndex++;
		groupToPalettePtr[groupIndex] = 52;
		for (classIndex=52; classIndex<=61; classIndex++)
			classToGroupPtr[classIndex] = (SInt16)groupIndex;
		
		groupIndex++;
		groupToPalettePtr[groupIndex] = 62;
		for (classIndex=62; classIndex<=71; classIndex++)
			classToGroupPtr[classIndex] = (SInt16)groupIndex;
		
		groupIndex++;
		groupToPalettePtr[groupIndex] = 72;
		for (classIndex=72; classIndex<=81; classIndex++)
			classToGroupPtr[classIndex] = (SInt16)groupIndex;
		
		groupIndex++;
		groupToPalettePtr[groupIndex] = 82;
		for (classIndex=82; classIndex<=91; classIndex++)
			classToGroupPtr[classIndex] = (SInt16)groupIndex;
		
		groupIndex++;
		groupToPalettePtr[groupIndex] = 92;
		for (classIndex=92; classIndex<=101; classIndex++)
			classToGroupPtr[classIndex] = (SInt16)groupIndex;
		
		groupIndex++;
		groupToPalettePtr[groupIndex] = 102;
		for (classIndex=102; classIndex<=111; classIndex++)
			classToGroupPtr[classIndex] = (SInt16)groupIndex;
		
		groupIndex++;
		groupToPalettePtr[groupIndex] = 112;
		for (classIndex=112; classIndex<=121; classIndex++)
			classToGroupPtr[classIndex] = (SInt16)groupIndex;
			
				// Load the group names.
				
		nameLength = sprintf (&groupNamePtr[1], "background");
		groupNamePtr[0] = (UInt8)nameLength;
		groupNamePtr += 32;
				
		nameLength = sprintf (&groupNamePtr[1], "0-1");
		groupNamePtr[0] = (UInt8)nameLength;
		groupNamePtr += 32;
				
		nameLength = sprintf (&groupNamePtr[1], "1-2");
		groupNamePtr[0] = (UInt8)nameLength;
		groupNamePtr += 32;
				
		nameLength = sprintf (&groupNamePtr[1], "2-3");
		groupNamePtr[0] = (UInt8)nameLength;
		groupNamePtr += 32;
				
		nameLength = sprintf (&groupNamePtr[1], "3-4");
		groupNamePtr[0] = (UInt8)nameLength;
		groupNamePtr += 32;
				
		nameLength = sprintf (&groupNamePtr[1], "4-5");
		groupNamePtr[0] = (UInt8)nameLength;
		groupNamePtr += 32;
				
		nameLength = sprintf (&groupNamePtr[1], "5-10");
		groupNamePtr[0] = (UInt8)nameLength;
		groupNamePtr += 32;
				
		nameLength = sprintf (&groupNamePtr[1], "10-20");
		groupNamePtr[0] = (UInt8)nameLength;
		groupNamePtr += 32;
				
		nameLength = sprintf (&groupNamePtr[1], "20-30");
		groupNamePtr[0] = (UInt8)nameLength;
		groupNamePtr += 32;
				
		nameLength = sprintf (&groupNamePtr[1], "30-40");
		groupNamePtr[0] = (UInt8)nameLength;
		groupNamePtr += 32;
				
		nameLength = sprintf (&groupNamePtr[1], "40-50");
		groupNamePtr[0] = (UInt8)nameLength;
		groupNamePtr += 32;
				
		nameLength = sprintf (&groupNamePtr[1], "50-60");
		groupNamePtr[0] = (UInt8)nameLength;
		groupNamePtr += 32;
				
		nameLength = sprintf (&groupNamePtr[1], "60-70");
		groupNamePtr[0] = (UInt8)nameLength;
		groupNamePtr += 32;
				
		nameLength = sprintf (&groupNamePtr[1], "70-80");
		groupNamePtr[0] = (UInt8)nameLength;
		groupNamePtr += 32;
				
		nameLength = sprintf (&groupNamePtr[1], "80-90");
		groupNamePtr[0] = (UInt8)nameLength;
		groupNamePtr += 32;
				
		nameLength = sprintf (&groupNamePtr[1], "90-100");
		groupNamePtr[0] = (UInt8)nameLength;
		groupNamePtr += 32;
			
				// Get memory for the color table.										
				
		bytesNeeded = fileInfoPtr->numberClasses *
														sizeof (ColorSpec) + sizeof (ColorTable);
		cTablePtr = (CTabPtr)CheckHandleSize ((Handle*)cTableHandlePtr,
															&continueFlag, 
															&changedFlag,  
															bytesNeeded);
									
		continueFlag = (cTablePtr != NULL);
		
		}	// end "if (continueFlag)"
		
	if (continueFlag)
		{                               
		colorSpecPtr = (ColorSpec*)&cTablePtr->ctTable;
		
		cTablePtr->ctSeed = GetCTSeed ();
		cTablePtr->ctFlags = 0;
		cTablePtr->ctSize = (SInt16)(numberGroups - 1);
		
				// Get the colors
		
		lastColorTableIndex = -1;
		for (classIndex=0; classIndex<fileInfoPtr->numberClasses; classIndex++)
			{
			groupIndex = classToGroupPtr[classIndex];
			colorTableIndex = groupToPalettePtr[groupIndex];
		
			if (colorTableIndex != lastColorTableIndex)
				{
				#ifndef multispec_wx
					theColor.red = classColorTablePtr[colorTableIndex].red << 8;
					theColor.green = classColorTablePtr[colorTableIndex].green << 8;
					theColor.blue = classColorTablePtr[colorTableIndex].blue << 8;
				#else
					theColor.red = classColorTablePtr[colorTableIndex].red;
					theColor.green = classColorTablePtr[colorTableIndex].green;
					theColor.blue = classColorTablePtr[colorTableIndex].blue;       
				#endif
				
				lastColorTableIndex = colorTableIndex;
				
				}	// end "if (colorTableIndex != lastColorTableIndex)"
		
			colorSpecPtr[classIndex].rgb = theColor;
			
			}	// end "for (classIndex=0; classIndex<fileInfoPtr->numberClasses; ..."
					
		fileInfoPtr->numberGroups = (UInt16)numberGroups;
		
		UnlockAndDispose (fileInfoPtr->groupNameHandle);
		fileInfoPtr->groupNameHandle = groupNameHandle;
		
		UnlockAndDispose (fileInfoPtr->groupTablesHandle);
		fileInfoPtr->groupTablesHandle = groupTablesHandle;
		
		}	// end "if (continueFlag)"
	
	else	// !continueFlag
		{
		UnlockAndDispose (groupNameHandle);
		UnlockAndDispose (groupTablesHandle);
		
		}	// end "if (!continueFlag)" 
			
	CheckAndUnlockHandle (fileInfoPtr->groupNameHandle);
	CheckAndUnlockHandle (fileInfoPtr->groupTablesHandle);
	
	return (continueFlag);
	
}	// end "LoadProbabilityGroupInformation"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CreateTRLSupportFile
//
//	Software purpose:	This routine creates a trl file. If the 
//							classNameCode is 'kFromDescriptionCode', then this
//							is called from the Reformat-Change Image File Format
//							processor. The class names and colors come from those
//							currently being used in the active image window.
//
//							AreasToThematicFileControl (paletteID)
//							CovertMultispectralToThematic (paletteHandle)
//							CreateTrailerFiles (paletteID)
//							ChangeFormatToBILorBISorBSQ (paletteHandle)
//							WriteThematicClassesAs (colorSpecPtr)
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			CreateThematicSupportFile in SFileIO.cpp
//							
//	Coded By:			Larry L. Biehl			Date: 02/21/1990
//	Revised By:			Larry L. Biehl			Date: 04/29/2013	

Boolean CreateTRLSupportFile (
				CMFileStream*						trailerStreamPtr, 
				UInt16								numberSupportFileClasses, 
				RGBCharColorPtr					classColorTablePtr,
//				UInt32								colorTableOffset,
				char*									classNameTablePtr,
				SInt16								trailerCode,
				Boolean								writeClassNamesFlag)

{
	HFileIOBufferPtr					ioBufferPtr;
	
	char									*erdasHeader = (char*)"TRAIL74";
										
	UCharPtr								ioTempBufferPtr;
	
	UInt32								count,
											index,
											numberBytes;
	
	SInt16								errCode;
										
	Boolean								continueFlag;
	
	
	if (numberSupportFileClasses > gClassListLimit)
																							return (TRUE);

			// Get buffer to load trailer file information into.  Try to get 		
			// buffer to hold one complete set of the 3 types of information -	
			// color scheme, class histogram, and class name. Note that even
			// though we are limited to colors for only 256 classes, we can
			// have class names for up to 'gClassListLimit' classes.			
			
	numberBytes = (((SInt32)numberSupportFileClasses+4)/4) * 128;
	numberBytes = MAX ((9*128), numberBytes);
	ioBufferPtr = (HFileIOBufferPtr)MNewPointer (numberBytes);
	continueFlag = (ioBufferPtr != NULL);
			
			// Continue if memory not full													
			
	if (continueFlag)
		{
		ioTempBufferPtr = (UCharPtr)ioBufferPtr;
	
					// Load in 'TRAILER' or 'TRAIL74'										
			
		BlockMoveData ((Ptr)erdasHeader, (Ptr)ioTempBufferPtr, 7);
	
				// Set bytes 8 to 896 to zeros.												
	
		ioTempBufferPtr = &ioTempBufferPtr[7];
		for (index=7; index<896; index++)
			{
			*ioTempBufferPtr = 0;
			ioTempBufferPtr++;
			
			}	// end "for (index=7; index<896; index++)" 
					
		ioTempBufferPtr = (unsigned char*)ioBufferPtr;
		ioTempBufferPtr = &ioTempBufferPtr[128];
											
				// Now load the colors into the trailer file buffer. 
				// Order is green, red, blue.
		
		for (index=0; index<numberSupportFileClasses; index++)
			{
			ioTempBufferPtr[0] = classColorTablePtr[index].green;
			ioTempBufferPtr[256] = classColorTablePtr[index].red;
			ioTempBufferPtr[512] = classColorTablePtr[index].blue;
			ioTempBufferPtr++;
			
			}	// end "for (index=0; index<numberSupportFileClasses; index++)"
		
				// Write buffer of data	to statistics file.								
					
		count = 7*128;
		errCode = MWriteData (trailerStreamPtr, &count, ioBufferPtr, kErrorMessages);
		
				// Load 0's for class histograms.											
				
		if (errCode == noErr && (trailerCode & kHistogram))
			{
					// Set bytes 1 to 1152 to zeros.											
		
			ioTempBufferPtr = (unsigned char*)ioBufferPtr;
			for (index=1; index<=1152; index++)
				{
				*ioTempBufferPtr = 0;
				ioTempBufferPtr++;
				
				}	// end "for (index=1; index<=numberBytes; index++)" 
			
					// Write histogram buffer to the trailer file.								
						
			count = 1152;
			errCode = MWriteData (trailerStreamPtr, &count, ioBufferPtr, kErrorMessages);
			
			}	// end "if (errCode == noErr && ...)" 
		
				// Load class names into the buffer if there is no error.								
			
		if (errCode == noErr && (trailerCode & kClassNames))
			{
			ioTempBufferPtr = (unsigned char*)ioBufferPtr;
			numberBytes = (((SInt32)numberSupportFileClasses+4)/4) * 128;
			for (index=0; index<numberBytes; index++)
				{
				*ioTempBufferPtr = 0;
				ioTempBufferPtr++;
				
				}	// end "for (index=0; index<numberBytes; index++)" 
			
			ioTempBufferPtr = (unsigned char*)ioBufferPtr;		
			for (index=0; index<numberSupportFileClasses; index++)
				{
				sprintf ((char*)ioTempBufferPtr, 
								"%s~", 
								classNameTablePtr);
								
				classNameTablePtr += 32;
				ioTempBufferPtr += 32;
				
				}	// end "for (index=0; index<numberSupportFileClasses; index++)" 
				
					// Write class names.														
					
			ioTempBufferPtr = (unsigned char*)ioBufferPtr;
				
					// Write buffer of data	to trailer file.								
			
			if (writeClassNamesFlag)
				{
				count = numberBytes;
				errCode = MWriteData (
										trailerStreamPtr, &count, ioBufferPtr, kErrorMessages);
				
				}	// end "if (writeClassNamesFlag" 
			
			}	// end "if (errCode == noErr && ...)" 
			
		}	// end "if (continueFlag)"
		  			
			// Unlock dispose of handles to temporary buffer.							
				
	CheckAndDisposePtr ((Ptr)ioBufferPtr);
	
	return (continueFlag && errCode == noErr);

}	// end "CreateTRLSupportFile"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		DetermineIfRequestedChannelsFromSingleFile
//
//	Software purpose:	This routine determines if the requested channels all come
//							from the a single image file.
//
//	Parameters in:
//
//	Parameters out:
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/03/2022
//	Revised By:			Larry L. Biehl			Date: 05/03/2022

Boolean DetermineIfRequestedChannelsFromSingleFile (
				WindowInfoPtr						windowInfoPtr,
				LayerInfoPtr						layerInfoPtr,
				FileInfoPtr							fileInfoPtr,
				UInt16								numberListChannels,
				UInt16*								channelListPtr)

{
	FileInfoPtr						localFileInfoPtr;
	
	UInt16							channel,
										fileInfoIndex;
	
	Boolean							fromSameFileFlag = FALSE;
	
	
			// If there is more than one image file for the window "file", then
			// determine if the channels all come from the same image file.
			// If the selected do not then return BILSpecialFlag as FALSE. It gets
			// more complicated to determine whether BILSpecialFlag can be used.
			
	if (windowInfoPtr != NULL && windowInfoPtr->numberImageFiles > 1)
		{
		if (channelListPtr != NULL)
			{
			if (layerInfoPtr != NULL)
				{
						// Determine if the selected channels all come from the same
						// image file.
						
				fromSameFileFlag = TRUE;
				fileInfoIndex = layerInfoPtr[channelListPtr[0]+1].fileInfoIndex;
				
				for (channel=0; channel<numberListChannels; channel++)
					{
					if (fileInfoIndex !=
									layerInfoPtr[channelListPtr[channel]+1].fileInfoIndex)
						fromSameFileFlag = FALSE;
						
					}	// end "for (channel=0; channel<..."
					
				if (!fromSameFileFlag)
					channel = numberListChannels;
																						
				localFileInfoPtr = &fileInfoPtr[fileInfoIndex];
				
				}	// end "if (layerInfoPtr != NULL)"
																					
			}	// end "if (channelListPtr != NULL)"
			
		}	// end "if (if (windowInfoPtr != NULL && ...->numberImageFiles > 1)"
		
	return (fromSameFileFlag);

}	// end "CreateTRLSupportFile"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		DisposeCMFileStream
//
//	Software purpose:	This routine disposes of the input CMFileStream pointer. This
//							differs between the Mac and Windows OSes.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/12/2010
//	Revised By:			Larry L. Biehl			Date: 08/14/2010

CMFileStream* DisposeCMFileStream (
				CMFileStream*						fileStreamPtr)

{
	if (fileStreamPtr != NULL)
		{
   	CloseFile (fileStreamPtr);
   	
		#if defined multispec_mac                         
			fileStreamPtr = (CMFileStream*)CheckAndDisposePtr ((Ptr)fileStreamPtr);
		#endif	// defined multispec_mac
   	
      #if defined multispec_win || defined multispec_wx                        
			delete fileStreamPtr;
			fileStreamPtr = NULL;
		#endif	// defined multispec_win || defined multispec_wx 
		
		}	// end "if (fileStreamPtr != NULL)"
		
	return (fileStreamPtr);
			
}	// end "DisposeCMFileStream"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		DiskFullAlert
//
//	Software purpose:	This routine puts up an alert warning the user that
//							there is not enough space on the selected volume
//							to write the file.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/07/1988
//	Revised By:			Larry L. Biehl			Date: 08/13/2010

void DiskFullAlert (
				SInt64								bytesNeeded, 
				Str255*								fileNamePtr)

{
	Str255								bytesNeededString;
	
	char* 								charFileNamePtr;
	
	SInt16								length,
											numberCharacters;
											
											
	charFileNamePtr = (char*)fileNamePtr;
	
			// Add commas to number of bytes needed.
			
	numberCharacters = sprintf ((char*)&bytesNeededString, 
											"%lld",  
											bytesNeeded);

	numberCharacters = InsertCommasInNumberString ((char*)&bytesNeededString, 
																	numberCharacters, 
																	-1,
																	numberCharacters);
										
	MGetString (gTextString, kFileIOStrID, IDS_FileIO01);
	/*									
	SInt16 length = snprintf ((char*)&gTextString2[1],
										255,
										(char*)&gTextString[1],
										&charFileNamePtr[1],
										bytesNeeded);
	*/									
	length = snprintf ((char*)&gTextString2[1],
								255,
								(char*)&gTextString[1],
								&charFileNamePtr[1],
								(char*)&bytesNeededString[0]);

	gTextString2[0] = (SInt8)length;

	DisplayAlert (kErrorAlertID, 
						kStopAlert, 
						0, 
						0, 
						0, 
						(UCharPtr)gTextString2);

}	// end "DiskFullAlert"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean FileExists
//
//	Software purpose:	This routine returns TRUE if a file currently exists
//							for the input file stream and FALSE otherwise. It
//							checks whether a file name is available.
//
//	Parameters in:		The file stream pointer			
//
//	Parameters out:	None			
//
// Value Returned:	TRUE if file name has been determined
//							FALSE if file name is not currently available
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/24/1995
//	Revised By:			Larry L. Biehl			Date: 02/08/2008

Boolean FileExists (
				CMFileStream*						fileStreamPtr)

{
	if (fileStreamPtr != NULL)
		{
		#if defined multispec_mac
			return (fileStreamPtr->fileName[0] > 0); 
		#endif	// defined multispec_mac
		
      #if defined multispec_win || defined multispec_wx
			return (fileStreamPtr->mFileType != -1);
		#endif	// defined multispec_win || defined multispec_wx
		
		}	// end "if (fileStreamPtr != NULL)"
		
	return (FALSE);

}	// end "FileExists"    



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean FileOpen
//
//	Software purpose:	This routine returns TRUE if a file is currently open
//							for the input file stream and FALSE otherwise.
//
//	Parameters in:		The file stream pointer			
//
//	Parameters out:	None			
//
// Value Returned:	TRUE if file is already open
//							FALSE if file is not open
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/16/1995
//	Revised By:			Larry L. Biehl			Date: 07/05/2017

Boolean FileOpen (
				CMFileStream*						fileStreamPtr)

{
	if (fileStreamPtr != NULL)
		{
		#if defined multispec_mac
			return (fileStreamPtr->refNum != 0); 
		#endif	// defined multispec_mac
		
		#if defined multispec_wx || defined multispec_win
			return (fileStreamPtr->FileOpen ());
		#endif	// defined multispec_win
		
		}	// end "if (fileStreamPtr != NULL)"
		
	return (FALSE);

}	// end "FileOpen"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetBILSpecial
//
//	Software purpose:	The purpose of this routine is to set
//							the BILSpecial flag.  This flag, when TRUE,
//							indicates for the routine 'GetLineOfData' that
//							all requested channels can be read with one
//							disk IO read.
//
//	Parameters in:				
//
//	Parameters out:			
//
//	Value Returned: 
//
// Called By:			BiPlotFieldData in SBiPlotData.cpp
//							MosaicImagesSideBySide in SMosaic.cpp
//							RectifyImage in SRectifyImage.cpp
//							ClassifyPerPointArea in SClassify.cpp
//							ClusterClassification in SCluster.cpp
//							GetNextClusterArea in SCluster.cpp
//							DisplayImagesSideBySide in SDisplayMultispectral.cpp
//							EchoClassifier in SClassifyEcho.cpp
//							ComputeHistogram in SHistogram.cpp
//							ListFieldData in SListData.cpp
//							GetAreaStats in SMatrixUtilities.cpp
//							GetFieldDataValues in SProjectMatrixUtilities.cpp
//							ChangeFormatToBILorBISorBSQ in SReformatChangeImageFileFormat.cpp
//							UpdateProjectMaskStats in SProjectComputeStatistics.cpp
//							HistogramFieldStats in SProjectHistogramStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/29/1989
//	Revised By:			Larry L. Biehl			Date: 05/14/2022

Boolean GetBILSpecial (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				UInt16*		 						channelListPtr, 
				UInt16								numberListChannels,
				UInt32								columnStart, 
				UInt32								columnEnd, 
				UInt32								columnInterval,
				UInt16								specialBILCode)

{  
	FileInfoPtr 						fileInfoPtr,
											localFileInfoPtr;
											
	LayerInfoPtr						layerInfoPtr;
	WindowInfoPtr						windowInfoPtr;
	                                
	UInt32								channel,
											fileInfoIndex,
											numberChannelsToRead;
	
	Boolean								BILSpecialFlag; 
	
	
			// Check input request.
			
	if (specialBILCode == kSetSpecialBILFlagFalse)
																							return (FALSE);
			
	if (specialBILCode == kSetSpecialBILFlagTrue)
																							return (TRUE);
																				
			// Initialize local variables.
			
	windowInfoPtr = fileIOInstructionsPtr->windowInfoPtr;
	layerInfoPtr = fileIOInstructionsPtr->layerInfoPtr;
	fileInfoPtr = fileIOInstructionsPtr->fileInfoPtr;
	
	localFileInfoPtr = fileInfoPtr;
	numberChannelsToRead = 0;
	
			// If there is more than one image file for the window "file", then	
			// determine if the channels all come from the same image file.		
			// If the selected do not then return BILSpecialFlag as FALSE. It gets	
			// more complicated to determine whether BILSpecialFlag can be used.		
			
	if (windowInfoPtr != NULL && windowInfoPtr->numberImageFiles > 1)
		{
		if (channelListPtr == NULL)
																							return (FALSE);
																				
		else	// channelListPtr != NULL 
			{
			if (layerInfoPtr == NULL)
																							return (FALSE);
																							
			if (numberListChannels < windowInfoPtr->totalNumberChannels)
				{
						// Determine if the selected channels all come from the same
						// image file.
						
				BILSpecialFlag = TRUE;
				fileInfoIndex = layerInfoPtr[channelListPtr[0]+1].fileInfoIndex;
				
				for (channel=0; channel<numberListChannels; channel++)
					{
					if (fileInfoIndex !=
									layerInfoPtr[channelListPtr[channel]+1].fileInfoIndex)
						BILSpecialFlag = FALSE;
						
					}	// end "for (channel=0; channel<..."
					
				if (!BILSpecialFlag)
																								return (FALSE);
																						
				localFileInfoPtr = &fileInfoPtr[fileInfoIndex];
				
				}	// end "if (numberListChannels < windowInfoPtr->totalNumberChannels)"
				
					// If all channels in the linked file are being used then it may be
					// possible to do a single read for the BIL formatted files.
					// Do further checks.
																						
			}	// end "else channelListPtr != NULL" 
			
		}	// end "if (if (windowInfoPtr != NULL && ...->numberImageFiles > 1)"
	
			// Do not set the special flag if the number of channels is only 1.
			
	if (localFileInfoPtr->numberChannels == 1) 
																							return (FALSE);
																				
			// If BIL formatted data and all channels and all columns are to be	
			// read in, then set flag to read all bytes at one time.					
	
	BILSpecialFlag = FALSE;
	if (localFileInfoPtr->bandInterleave == kBIL && 
			localFileInfoPtr->numberChannels >= numberListChannels &&
				localFileInfoPtr->numberBits >= 8)
		{
		if (channelListPtr == NULL)
			{
			BILSpecialFlag = TRUE;
			numberChannelsToRead = numberListChannels;
			
			}	// end "if (channelListPtr == NULL)"
			
		else	// channelListPtr != NULL 
			{
			BILSpecialFlag = 
				DetermineIfChannelsInOrder (channelListPtr, (UInt32)numberListChannels);
			
			if (BILSpecialFlag)
				{	
				numberChannelsToRead = 
								channelListPtr[numberListChannels-1] - channelListPtr[0] + 1;
		
						// Do not read all channels if the number being used is less than
						// about 3 percent of the total being read.
					
				if (numberChannelsToRead > 30 && 
													numberListChannels < numberChannelsToRead/30)
					BILSpecialFlag = FALSE;
					
				}	// end "if (BILSpecialFlag)"
							
			}	// end "else channelListPtr != NULL"
		
		}	// end "if (localFileInfoPtr->bandInterleave == kBIL && ..."
	
			// If reads are being handled by gdal library then all channels will be read
			// with one call to the routines. Do not do this for now since gdal packs the
			// data if not all columns are being read.
			
	//if (fileInfoPtr->gdalDataSetH != NULL)
	//	BILSpecialFlag = TRUE;
	
	if (BILSpecialFlag && (fileInfoPtr->numberPreChannelBytes != 0 ||
														fileInfoPtr->numberPostChannelBytes != 0))
		BILSpecialFlag = FALSE;
	
	if (BILSpecialFlag)
		fileIOInstructionsPtr->bilSpecialNumberChannels = (UInt16)numberChannelsToRead;
		
	return (BILSpecialFlag);
	      
}	// end "GetBILSpecial" 


/*
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetClassNameFromDescription
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
// Called By:			CreateThematicSupportFile in SFileIO.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/17/2011
//	Revised By:			Larry L. Biehl			Date: 05/20/2011 
                       
void GetClassNameFromDescription (
				UInt32								classNumber,
				UInt16*								classSymbolPtr,
				UInt16*								classSymbolIndexPtr,
				StringPtr							classNamePtr,
				SInt16*								classNameLengthPtr,
				StringPtr							outputClassNameBufferPtr,
				SInt16								collapseClassCode)

{  
	UInt32								classNameIndex,
											count;
											
											
	if (collapseClassCode == kCollapseClass)
		{
				// Class names will be in the order that they are in the description
				// structure.
				
		classNameIndex = classNumber * 32;
		classNamePtr = &classNamePtr[classNameIndex];
		
		count = MIN (classNamePtr[0], 31);
		BlockMoveData (&classNamePtr[1], outputClassNameBufferPtr, count);
		
		}	// end "if (collapseClassCode == kCollapseClass)"
		
	else	// "collapseClassCode != kCollapseClass"
		{
				// Class names from the description structure will be used for those
				// classes included in the class symbol vector. The missing classes
				// will be supplied with the default name "class n" where n represents
				// the class number.
				
		if (classSymbolPtr[*classSymbolIndexPtr] == classNumber)
			{
			classNameIndex = *classSymbolIndexPtr * 32;
			classNamePtr = &classNamePtr[classNameIndex];
		
			count = MIN (classNamePtr[0], 31);
			BlockMoveData (&classNamePtr[1], outputClassNameBufferPtr, count);
			
			(*classSymbolIndexPtr)++;
			
			}	// end "if (classSymbolPtr[*classSymbolIndexPtr] == classNumber)"
		
		else	// classSymbolPtr[*classSymbolIndexPtr] != classNumber
			{
			count = sprintf ((char*)outputClassNameBufferPtr, "Class %ld", classNumber);
			count = MIN (count, 31);
			
			}	// end "if (classSymbolPtr[*classSymbolIndexPtr] == classNumber)"
	
		}	// end "else collapseClassCode != kCollapseClass"
		
	outputClassNameBufferPtr [count] = '~';
	*classNameLengthPtr = count + 1;
		
}	// end "GetClassNameFromDescription"
*/



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetCopyOfPFileNameFromFileStream
//
//	Software purpose:	The purpose of this routine is to copy the actual file 
//							name not including the path to the specified location. 
//							The file name will output in pascal format.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 09/01/2017
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

void GetCopyOfPFileNameFromFileStream (
				CMFileStream*						fileStreamPtr,
				UInt8*								outputPtr)

{  
	GetCopyOfPFileNameFromFileStream (
											fileStreamPtr, outputPtr, kDefaultFileStringCode);
	
}	// end "GetCopyOfPFileNameFromFileStream"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetCopyOfPFileNameFromFileStream
//
//	Software purpose:	The purpose of this routine is to copy the actual file 
//							name not including the path to the specified location. 
//							The file name will output with a string length in the first
//							element of the string.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 12/11/1995
//	Revised By:			Larry L. Biehl			Date: 03/31/2023

void GetCopyOfPFileNameFromFileStream (
				CMFileStream*						fileStreamPtr,
				UInt8*								outputPtr,
				SInt16								charWidthCode)

{  
	void*									fileNamePtr;
	UInt16								nameLength = 0;
	
	
	if (outputPtr != NULL)
		{
		if (fileStreamPtr != NULL)
			{
			if (charWidthCode == kReturnUTF8)
				{
				UInt8* utf8FileNamePtr = fileStreamPtr->mUTF8FileName;
				StringCopy ((void*)outputPtr, (void*)&utf8FileNamePtr[2], 2, kReturnUTF8);
				
				nameLength = (int)strlen ((char*)&outputPtr[2]);
				
				}	// end "if (charWidthCode == kReturnUTF8)"
				
			else	// charWidthCode != kReturnUTF8
				{
				fileNamePtr =
							GetFileNameCPointerFromFileStream (fileStreamPtr, charWidthCode);
				StringCopy ((void*)outputPtr, (void*)fileNamePtr, 1, charWidthCode);

				nameLength = StringLength (fileNamePtr, (charWidthCode==kReturnASCII));
				
				}	// end "else charWidthCode != kReturnUTF8"
			
			SetPascalStringLengthCharacter ((void*)outputPtr, nameLength, charWidthCode);

			}	// end "if (fileInfoPtr != NULL)"
			
		}	// end "if (outputPtr != NULL)" 
	
}	// end "GetCopyOfPFileNameFromFileStream"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetCopyOfPFileNameFromFileInfo
//
//	Software purpose:	The purpose of this routine is to copy the actual file 
//							name not including the path to the specified location. 
//							The file name will output in pascal format.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 09/01/2017
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

void GetCopyOfPFileNameFromFileInfo (
				FileInfoPtr							fileInfoPtr,
				UInt8*								outputPtr)

{  
	GetCopyOfPFileNameFromFileInfo (fileInfoPtr, outputPtr, kDefaultFileStringCode);
	
}	// end "GetCopyOfPFileNameFromFileInfo"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetCopyOfPFileNameFromFileInfo
//
//	Software purpose:	The purpose of this routine is to copy the actual file 
//							name not including the path to the specified location. 
//							The file name will output in pascal format.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 12/12/1995
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

void GetCopyOfPFileNameFromFileInfo (
				FileInfoPtr							fileInfoPtr,
				UInt8*								outputPtr,
				SInt16								returnCode)

{  
	if (outputPtr != NULL)
		{
		if (fileInfoPtr != NULL)
			{              
			CMFileStream* fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
			
			GetCopyOfPFileNameFromFileStream (fileStreamPtr, outputPtr, returnCode);
			
			}	// end "if (fileInfoPtr != NULL)"
			
		else	// fileInfoPtr == NULL
			outputPtr[0] = 0;
			
		}	// end "if (outputPtr != NULL)" 
	
}	// end "GetCopyOfPFileNameFromFileInfo"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetDataConversionCode
//
//	Software purpose:	The purpose of this routine is to get the data
//							conversion code based on the number of bytes,
//							input sign flag and output sign desired, and where
//							bytes width on output is to be forced.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 01/11/2006
//	Revised By:			Larry L. Biehl			Date: 02/27/2022

UInt32 GetDataConversionCode (
				UInt32								inputDataTypeCode,
				UInt32								inputNumberBytes,
				Boolean								inputSignedFlag,
				UInt32								forceOutByteCode,
				Boolean								outputSignedFlag,
				Boolean*								changePossibleFlagPtr)

{  
	UInt32								dataConversionCode;
	
	UInt32								inputByteCode = 0,
											inputSignCode,
											outputByteCode = 0,
											outputSignCode;
											
	Boolean								changePossibleFlag;
											
							
	changePossibleFlag = FALSE;				
	outputSignCode = 0;				
												
	if (inputNumberBytes == 1)
		inputByteCode = 0x00000080;
	
	else if (inputNumberBytes == 2)
		inputByteCode = 0x00000100;
	
	else if (inputNumberBytes == 4)
		inputByteCode = 0x00000200;
	
	else if (inputNumberBytes == 8)
		inputByteCode = 0x00000400;
	
	if (inputDataTypeCode == kIntegerType)
		{
		if (inputSignedFlag)
			inputSignCode = 0x00000001;
		
		else	// !inputSignedFlag
			inputSignCode = 0x00000000;
			
		}	// end "if (inputDataTypeCode == kIntegerType)"
		
	else	// inputDataTypeCode == kRealType
		inputSignCode = 0x00000003;
	
	if (forceOutByteCode == kDoNotForceBytes)
		{
		outputByteCode = inputByteCode;
		outputSignCode = inputSignCode;
		
		}	// end "if (forceOutByteCode == kDoNotForceBytes)"
		
	else if (forceOutByteCode == kForce1Byte)
		outputByteCode = 0x00000080;
		
	else if (forceOutByteCode == kForce2Bytes)
		outputByteCode = 0x00000100;
		
	else if (forceOutByteCode == kForce4Bytes)
		outputByteCode = 0x00000200;
		
	else if (forceOutByteCode == kForceReal4Bytes)
		{
		outputByteCode = 0x00000200;
		outputSignCode = 0x00000003;
		
		}	// end "else if (forceOutByteCode == kForceReal4Bytes)"
		
	else if (forceOutByteCode == kForceReal8Bytes)
		{
		outputByteCode = 0x00000400;
		outputSignCode = 0x00000003;
		
		}	// end "else if (forceOutByteCode == kForceReal8Bytes)"
		
	if (outputSignCode == 0 && outputSignedFlag)
		outputSignCode = 0x00000001;
				
			// Determine if values could be changed during the conversion.
			
	if (outputByteCode < inputByteCode)
		changePossibleFlag = TRUE;
		
	if (inputByteCode == outputByteCode && inputSignCode != outputSignCode)
		changePossibleFlag = TRUE;
		
	if (changePossibleFlag && changePossibleFlagPtr == NULL &&
				inputNumberBytes != 8 && forceOutByteCode != kForceReal4Bytes)
		{
				// This call is for conversion in the ReadLine routine.  We will not
				// do down conversion here.  Force conversion for reals to be to 64-bit 
				// real and for integers to be to 32-bit integer and like sign. 
				// (except for conversion from 64-bit real to 32-bit real; will
				// reevaluate this case. I believe in stat-enhancement & feature
				// extraction.
				// All other down conversions should only be from the reformat change
				// image file format processor
		
		//outputByteCode = inputByteCode;
		//outputSignCode = inputSignCode;

		if (inputByteCode <= 0x00000200 && inputSignCode <= 0x00000001)
			{
			outputByteCode = 0x00000200;
			outputSignCode = inputSignCode;
			
			}	// end "if (inputByteCode <= 0x00000200 && inputSignCode <= 0x00000001)"
			
		else	// inputByteCode > 0x00000200 || inputSignCode > 0x00000001
			{
			outputByteCode = 0x00000400;
			outputSignCode = 0x00000003;
			
			}	// end "else inputByteCode > 0x00000200 || inputSignCode > 0x00000001"
		
		}	// end "if (changePossibleFlagPtr != NULL)"
			
	if (changePossibleFlagPtr != NULL)
		{
				// This is called from Reformat Processor which handles down conversion.
				
		*changePossibleFlagPtr = changePossibleFlag;
		
		if (changePossibleFlag)
			{
					// Force data conversion code to represent what the "input" format
					// will be when down converting.

			if (inputByteCode <= 0x00000200 && inputSignCode <= 0x00000001)
				inputByteCode = 0x00000200;
				
			else	// inputByteCode > 0x00000200 || inputSignCode > 0x00000001
				inputByteCode = 0x00000400;
				
			}	// end "if (changePossibleFlag)"
		
		}	// end "if (changePossibleFlagPtr != NULL)"
	
	if (outputByteCode == inputByteCode && outputSignCode == inputSignCode)
		{
		dataConversionCode = inputByteCode<<16;
		dataConversionCode += outputByteCode;
		
		}	// end "if (outputByteCode == inputByteCode && ..."
	
	else	// outputByteCode != inputByteCode || ...
		{
		inputByteCode += inputSignCode;
		outputByteCode += outputSignCode;
		dataConversionCode = inputByteCode<<16; 
		dataConversionCode += outputByteCode;
																	
		}	// end "else outputByteCode != inputByteCode || ..."
		
	return (dataConversionCode);
	
}	// end "GetDataConversionCode"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetFile
//
//	Software purpose:	The purpose of this routine is to put up the
//							standard get file dialog box to allow the user to
//							select the file to read from.
//		
//	Parameters in:		Address to MFileInfo structure for selected file.
//							Number of parameters in type list.
//							Address of type list.
//
//	Parameters out:	MFileInfo Structure parameters for selected file.
//
// Value Returned:	Error code for file operations.
//
// Called By:			GetSTASupportFile in SHistogram.cpp
//							OpenImageFile in SOpenFileDialog.cpp
//							GetProjectFile in SProject.cpp
//							UserLocateProjectBaseImage in SProject.cpp
//							LoadTransformationFile in SSaveWrite.c
//
//	Coded By:			Larry L. Biehl			Date: 05/28/1988
//	Revised By:			Larry L. Biehl			Date: 02/14/2025

SInt16 GetFile (
				CMFileStream* 						fileStreamPtr,
				DialogPtr							dialogPtr,
				SInt16								numberTypes,
				OSType*								fileTypesPtr, 
				LocalAppFile*						localAppFilePtr,
				FSRef*								fileAsFSRefPtr,
				UInt32*								itemCountPtr,
				int*									returnKeyCodePtr,
				SInt16								stringIndex)

{
	SInt16								errCode = noErr;
	

	#if defined multispec_mac
		if (gNavServicesExistsFlag)
			{
			errCode = NavServicesGetFile (fileStreamPtr, 
														numberTypes, 
														fileTypesPtr, 
														localAppFilePtr,
														fileAsFSRefPtr,
														itemCountPtr, 
														stringIndex);

			if (errCode == noErr && gMultiSpecWorkflowInfo.workFlowCode > 0)
						// Save the name of this file in case needed for work flow 
						// processing.
				GetCopyOfPFileNameFromFileStream (fileStreamPtr,
											gMultiSpecWorkflowInfo.lastFileReadName);

																						return (errCode);
					
			}	// end "if (gNavServicesExistsFlag)"
	#endif	// defined multispec_mac

	#if defined multispec_wx || defined multispec_win
		#if defined multispec_win
			USES_CONVERSION;

					// Initialize variables.
				
			gGetFileStatus = 3;

					// Tried to change the length of the maximum path to 1024. It had no
					// affect. Windows 10 allows it but looks like system settings need
					// to be changed or an application manifest. Not sure about backwards
					// capability though.
			//#define WinMaxPath 260	// _MAX_PATH which is equal to 260
			//The length of FSRef is _MAX_PATH+4 to allow for length at the beginning
			// and a c terminator plus 1 extra. It also makes it a multiple of 8 bytes.
		
			TBYTE	filterString[512];

			SInt16 filterStringIndex = IDS_FilterString;
			if (stringIndex == IDS_SelectProject)
				filterStringIndex = IDS_ProjectFilterString;
				
			else if (stringIndex == IDS_FileIO62)
				filterStringIndex = IDS_TransformFilterString;
				
			else if (stringIndex == IDS_FileIO76)
				filterStringIndex = IDS_TRLFilterString;
				
			else if (stringIndex == IDS_FileIO95)
				filterStringIndex = IDS_ClassificationFilterString;
				
			else if (stringIndex == IDS_FileIO96)
				filterStringIndex = IDS_ClassificationFilterString;
				
			else if (stringIndex == IDS_SelectNewBaseImage) 
				filterStringIndex = IDS_MultispectralFilterString;
				
			else if (stringIndex == IDS_FileIO105) 
				filterStringIndex = IDS_ClassificationFilterString;
				
			else if (stringIndex == IDS_SelectImageStatistics) 
				filterStringIndex = IDS_StatFilterString;
				
			else if (stringIndex == IDS_FileIO146) 
				filterStringIndex = IDS_TextFileFilterString;
			
			if (!GetSpecifiedStringNumber (
					kFileIOStrID, filterStringIndex, filterString, TRUE, 510))
																								return (1);

			gFileFilterIndex = 1;
			if (filterStringIndex == IDS_FilterString)
				gFileFilterIndex = gImageFileFilterIndex;
		 
			CMOpenFileDialog		dialog (NULL, (TBYTE*)&filterString[1]);
			#if defined _UNICODE
				TBYTE					string[_MAX_PATH+4];
			#endif
			CString 					filePathName;
			FSRef*					localFileAsFSRefPtr;
			TBYTE						pathName[_MAX_PATH+4];
			SInt16					pathLength;

			if (itemCountPtr != NULL)
				*itemCountPtr = 1;
					
			if (localAppFilePtr != NULL)
				{
				localFileAsFSRefPtr = (FSRef*)localAppFilePtr;
				
				}	// end "if (localAppFilePtr != NULL)"
				
			else	// localAppFilePtr == NULL
				{
				if (gMultipleImageFileCode)
					{
					gGetFileStatus = 2;
					
					}	// end "if (gMultipleImageFileCode)"
					
				else	// !gMultipleImageFileCode	
					{
					gGetFileStatus = -1;
					if (stringIndex == IDS_SelectImage)
						gGetFileStatus = 0; 
					
					}	// end "else !gMultipleImageFileCode"
					
				if (!dialog.DoDialog (stringIndex))
					{
					SetFileDoesNotExist (fileStreamPtr, kDoNotKeepUTF8CharName);
																								return (0);
																							
					}	// end "if (!dialog.DoDialog ())"
				
				if (fileAsFSRefPtr != NULL)
					{
					POSITION fileNamesPosition = dialog.GetStartPosition ();
					int iCtr = 0;
					while (fileNamesPosition != NULL)
						{
						filePathName = dialog.GetNextPathName (fileNamesPosition);
						localAppFilePtr = (TBYTE*)filePathName.GetBuffer (_MAX_PATH+4);
						pathLength = filePathName.GetLength ();

						if (pathLength < _MAX_PATH)
							{
									// Get copy of the path name so that it can be modified.
									// Convert to Pascal string so that it is in the same format
									// as the file name in the Macintosh version.
									// CtoPstring adds a C string terminator at the end.		
						
							wcscpy ((wchar_t*)&fileAsFSRefPtr[iCtr].hidden[1], localAppFilePtr);  
							fileAsFSRefPtr[iCtr].hidden[0] = pathLength;

							filePathName.ReleaseBuffer ();
							iCtr++;
						
							if (iCtr >= gMaximumNumberOfLinkedFiles)
								{
								DisplayAlert (
									kErrorAlertID, kStopAlert, kAlertStrID, IDS_Alert147, 0, NULL);
								break;

								}	// end "if (iCtr >= gMaximumNumberOfLinkedFiles)"

							}	// end "if (pathLength < _MAX_PATH)"

						else
							{
							DisplayAlert (
								kErrorAlertID, kStopAlert, kAlertStrID, IDS_Alert151, 0, NULL);
							break;

							}	// end "pathLength >= _MAX_PATH"
						
						}	// end "while (fileNamesPosition != NULL)"
					
					if (itemCountPtr != NULL)
						*itemCountPtr = iCtr;
					/*		
							// There are multiple files and linking was not requested. Verify that the multiple 
							// files are not to be linked. 
							// Decided this is not needed since it is automatic to link multiple files together
							// for WindowsOS version.
					
					if (iCtr > 1 && gMultipleImageFileCode == 0)
						{
						int itemHit = AfxMessageBox ((LPCTSTR)A2T("Multiple files selected. Link them together?"), MB_YESNO, 0);
						
						if (itemHit == IDYES)
							gMultipleImageFileCode = 2;

						}	// end "if (iCtr > 1 && gMultipleImageFileCode == 1)"
					*/
					if (iCtr > 1 && gMultipleImageFileCode == 2)
						gMultipleImageFileCode = 2;

					localFileAsFSRefPtr = &fileAsFSRefPtr[0];
					
					}	// end "if (fileAsFSRefPtr != NULL)"

				else	// fileAsFSRefPtr == NULL
					{
							// Only one file is being selected.
						
					filePathName = dialog.GetPathName ();
					localAppFilePtr = (TBYTE*)filePathName.GetBuffer (_MAX_PATH+4);
					pathLength = filePathName.GetLength ();
					wcscpy (string, localAppFilePtr);  
					CtoPstring (string, pathName);

					filePathName.ReleaseBuffer ();
					localFileAsFSRefPtr = (FSRef*)pathName;

					}	// end "else fileAsFSRefPtr == NULL"

				}	// end "else localAppFilePtr == NULL"
				
					// Get copy of the path name so that it can be modified.
					// Convert to Pascal string so that it is in the same format
					// as the file name in the Macintosh version.
					// Make certain that there is a C string terminator at the end.
		
			fileStreamPtr->SetFilePath ((WideFileStringPtr)localFileAsFSRefPtr,
													TRUE);
		
			errCode = CMFileStream::GetFileType (
										(TBYTE*)localFileAsFSRefPtr, &fileStreamPtr->mFileType);

					// Force the pascal file name to be updated in case user is
					// expecting to use it.

			fileStreamPtr->GetFileNamePPtr ();		
		#endif	// defined multispec_win

		#if defined multispec_wx
			long style;
		
				// Initialize variables.															
				
			gGetFileStatus = 3;
		
			SInt16 filterStringIndex = IDS_FilterString;
			if (stringIndex == IDS_SelectProject)
				filterStringIndex = IDS_ProjectFilterString;
				
			else if (stringIndex == IDS_FileIO62)
				filterStringIndex = IDS_TransformFilterString;
				
			else if (stringIndex == IDS_FileIO76)
				filterStringIndex = IDS_TRLFilterString;
				
			else if (stringIndex == IDS_FileIO95)
				filterStringIndex = IDS_ClassificationFilterString;
				
			else if (stringIndex == IDS_FileIO96)
				filterStringIndex = IDS_ClassificationFilterString;
				
			else if (stringIndex == IDS_SelectNewBaseImage) 
				filterStringIndex = IDS_MultispectralFilterString;
				
			else if (stringIndex == IDS_FileIO105) 
				filterStringIndex = IDS_ClassificationFilterString;
				
			else if (stringIndex == IDS_SelectImageStatistics) 
				filterStringIndex = IDS_StatFilterString;
				
			else if (stringIndex == IDS_FileIO146)
				{
				filterStringIndex = IDS_TextFileFilterString;
				stringIndex = IDS_SelectENVI;
				}
			
			if (!GetSpecifiedStringNumber (
					kFileIOStrID, filterStringIndex, gTextString, TRUE))
																								return (1);

			gFileFilterIndex = 1;
			if (filterStringIndex == IDS_FilterString)
				gFileFilterIndex = gImageFileFilterIndex;
	
		 			// Have tried several ways to center open file dialog on MultiSpec
		 			// Online window. If one uses NULL, the dialog will be centered
		 			// but will be behind the text and image windows. If one uses
		 			// the text window frame then the dialog box will be on top but
		 			// positioned over the text window. Not optimum but better that
		 			// the other options.
			
			#if defined multispec_wxlin || defined multispec_wxmac
				wxFrame* frame = gOutputViewCPtr->m_frame;
			#endif
			#if defined multispec_wxwin
				wxFrame* frame = GetMainFrame ();
				
						// The following is to force the get file dialog box to be on top
						// of the calling dialog box. Need for Windows OS only.
				if (dialogPtr != NULL)
					frame = (wxFrame*)dialogPtr;
			#endif

			CMOpenFileDialog *filedlgobj = new CMOpenFileDialog (frame);
	
			wxString 					filePathName;
			wxWCharBuffer				wideCharBuffer;
			FSRef*						localFileAsFSRefPtr;
			wchar_t						pathName[_MAX_PATH];
			size_t						stringLength;
			int							filePathIndex;
			
			if (fileAsFSRefPtr != NULL)
				style = wxFD_MULTIPLE | wxFD_OPEN | wxFD_FILE_MUST_EXIST;	
			else
				style = wxFD_OPEN;
				
			//style |= wxSTAY_ON_TOP;	// adding wxSTAY_ON_TOP does not work
					
			if (itemCountPtr != NULL)
				*itemCountPtr = 1;
					
			if (localAppFilePtr != NULL)
				{
				localFileAsFSRefPtr = (FSRef*)localAppFilePtr;
				
				}	// end "if (localAppFilePtr != NULL)"
				
			else	// localAppFilePtr == NULL
				{
				if (gMultipleImageFileCode)
					{
					gGetFileStatus = 2;
					
					}	// end "if (gMultipleImageFileCode)"
					
				else	// !gMultipleImageFileCode	
					{
					gGetFileStatus = -1;
					if (stringIndex == IDS_SelectImage)
						gGetFileStatus = 0; 
					
					}	// end "else !gMultipleImageFileCode"
					
				if (!filedlgobj->DoDialog (returnKeyCodePtr, stringIndex, style))
					{
					SetFileDoesNotExist (fileStreamPtr, kDoNotKeepUTF8CharName);
																								return (0);
																							
					}	// end "if (!dialog.DoDialog ())"

				localAppFilePtr = 
								(LocalAppFile*)MNewPointer ((_MAX_PATH)*sizeof (wchar_t));
				if (fileAsFSRefPtr != NULL)
					{
					//POSITION fileNamesPosition = dialog.GetStartPosition ();
					wxArrayString patharray;
					patharray = filedlgobj->GetPaths ();
					int iCtr = 0;

					for (int i = 0; i < patharray.GetCount (); i++)
						{
						filePathName = patharray.Item (i);
						stringLength = MIN (filePathName.length (), _MAX_PATH-3);
						wideCharBuffer = filePathName.wc_str ();
						wcsncpy (localAppFilePtr, wideCharBuffer.data (), stringLength);
						localAppFilePtr[stringLength] = 0;
						
								// Get copy of the path name so that it can be modified.
								// Convert to Pascal string so that it is in the same format
								// as the file name in the Macintosh version.
								// CtoPstring adds a C string terminator at the end.

						CtoPstring ((wchar_t*)localAppFilePtr,
										(wchar_t*)&fileAsFSRefPtr[iCtr],
										(int)stringLength);
						//filePathName.ReleaseBuffer ();
						iCtr++;
						
						if (iCtr >= gMaximumNumberOfLinkedFiles)
							{
							DisplayAlert (
								kErrorAlertID, kStopAlert, kAlertStrID, IDS_Alert147, 0, NULL);
							break;

							}	// end "if (iCtr >= gMaximumNumberOfLinkedFiles)"
						
						}	// end "for (int i = 0; i < patharray.GetCount (); i++)"
					
					if (itemCountPtr != NULL)
						*itemCountPtr = iCtr;

					if (iCtr > 1 && gMultipleImageFileCode == 2)
						gMultipleImageFileCode = 2;

					localFileAsFSRefPtr = &fileAsFSRefPtr[0];
					
					}	// end "if (fileAsFSRefPtr != NULL)"

				else	// fileAsFSRefPtr == NULL
					{
							// Only one file is being selected.
					
					filePathName = filedlgobj->GetPath ();
					stringLength = MIN (filePathName.length (), _MAX_PATH-3);
					wideCharBuffer = filePathName.wc_str ();
					
					wcsncpy (localAppFilePtr, wideCharBuffer.data (), stringLength);
					localAppFilePtr[stringLength] = 0;
						
					CtoPstring (localAppFilePtr, pathName, stringLength);
					localFileAsFSRefPtr = (FSRef*)pathName;

					}	// end "else fileAsFSRefPtr == NULL"
					
						// Save the directory part of filePathName so that it will become 
						// the default directory for the next GetFile operation. Work 
						// backwards on file path until we find the first '/' which we 
						// assume is the last portion of the directory name.
						// Remember to allow for 0 based index.
						
				filePathIndex = filePathName.length () - 1;		
				//#if defined multispec_wxlin || defined multispec_wxmac
					while (filePathIndex > 0 && filePathName[filePathIndex] != '/' &&
																		filePathName[filePathIndex] != '\\')
						filePathIndex--;
				//#endif
				//#if defined multispec_wxwin
				//	while (filePathIndex > 0 && filePathName[filePathIndex] != '\\')
				//		filePathIndex--;
				//#endif

				if (filePathIndex > 0 && (filePathName[filePathIndex] == '/' ||
																		filePathName[filePathIndex] == '\\'))
					filePathIndex--;
					
				filePathName.Truncate (filePathIndex+1);
				if (filePathIndex > 0)
					gDefaultDataDirectory = filePathName;	
										
				localAppFilePtr = 
									(LocalAppFile*)CheckAndDisposePtr ((Ptr)localAppFilePtr);

				}	// end "else localAppFilePtr == NULL"
				
					// Get copy of the path name so that it can be modified.
					// Convert to Pascal string so that it is in the same format
					// as the file name in the Macintosh version.
					// Make certain that there is a C string terminator at the end.

			fileStreamPtr->SetFilePath ((WideFileStringPtr)localFileAsFSRefPtr,
													TRUE);

			errCode = CMFileStream::GetFileType (
						(WideFileStringPtr)localFileAsFSRefPtr, &fileStreamPtr->mFileType);

					// Force the pascal file name to be updated in case user is
					// expecting to use it.

			fileStreamPtr->GetFileNamePPtr ();
		#endif	// defined multispec_wx
	
		if (errCode == noErr)
			errCode = OpenFileReadOnly (fileStreamPtr, 
													kResolveAliasChains,
													kLockFile,
													kDoNotVerifyFileStream);
						
		IOCheck (errCode, fileStreamPtr);
		
		if (errCode != noErr)	
			SetReferenceNumber (fileStreamPtr, 0);
	#endif	// defined multispec_wx || defined multispec_win
	
	#if defined multispec_mac
		if (errCode == noErr && gMultiSpecWorkflowInfo.workFlowCode > 0)
				// Save the name of this file in case needed for work flow processing.
			GetCopyOfPFileNameFromFileStream (fileStreamPtr,
									gMultiSpecWorkflowInfo.lastFileReadName);
	#endif	// end "defined multispec_mac"
	
	return (errCode);

}	// end "GetFile"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt32 GetFileCreator
//
//	Software purpose:	The purpose of this routine is to return the file
//							creator represented by the input file stream pointer.
//
//	Parameters in:		File Stream Pointer
//
//	Parameters out:	None
//
//	Value Returned:	file creator
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 10/25/1995
//	Revised By:			Larry L. Biehl			Date: 10/25/1995	

SInt32 GetFileCreator (
				CMFileStream*						fileStreamPtr)

{  
	SInt32								fileCreator = -1;
	
	
	if (fileStreamPtr != NULL)
		{                               
		#if defined multispec_mac                       
			fileCreator = fileStreamPtr->creator;
		#endif	// defined multispec_mac 
	              
      #if defined multispec_win | defined multispec_wx                           
			fileCreator = fileStreamPtr->mCreator;
		#endif	// defined multispec_win
		
		}	// end "if (fileStreamPtr != NULL)" 

	return (fileCreator);
	
}	// end "GetFileCreator" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetFileDlgDetermineLinkVisibility
//
//	Software purpose:	The purpose of this routine is to determine whether the link
//							file checkbox should be visible.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	TRUE: if link file checkbox can be visible
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 12/13/2012
//	Revised By:			Larry L. Biehl			Date: 04/25/2022	

Boolean GetFileDlgDetermineLinkVisibility ()

{  
	WindowInfoPtr						windowInfoPtr;
	
	
	windowInfoPtr = (WindowInfoPtr)GetHandlePointer (gActiveImageWindowInfoH);
	
	if	(windowInfoPtr == NULL ||
			windowInfoPtr->windowType != kImageWindowType ||
				windowInfoPtr->projectBaseImageFlag) // ||
					//windowInfoPtr->bandInterleave == kBIS)
		return (FALSE);
			
	else
		return (TRUE);
	
}	// end "GetFileDlgDetermineLinkVisibility" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetFileNameFromFSRef
//
//	Software purpose:	The purpose of this routine is to get the file name from the
//							input FSRef. The name will be returned with the string length
//							in the first two bytes and a c terminator at the end of the
//							string.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
//	Coded By:			Larry L. Biehl			Date: 02/04/2013
//	Revised By:			Larry L. Biehl			Date: 05/03/2023

SInt16 GetFileNameFromFSRef (
				FSRef*								fileAsFSRefPtr,
				StringPtr							fileNamePtr)

{  	
	SInt16								errCode;


	#if defined multispec_mac
		CFStringRef							cfStringRef;
												
		HFSUniStr255						uniFileName;
		
		
		fileNamePtr[0] = 0;
		fileNamePtr[1] = 0;
		
		errCode = FSGetCatalogInfo (fileAsFSRefPtr,
												kFSCatInfoNone,
												NULL,
												&uniFileName,
												NULL,
												NULL);

		if (errCode == noErr)
			{
			cfStringRef = CFStringCreateWithCharacters (kCFAllocatorDefault,
																		uniFileName.unicode,
																		uniFileName.length);
			
			CFStringGetCString (cfStringRef,
										(char*)&fileNamePtr[1],
										(CFIndex)255,
										kCFStringEncodingUTF8);
			fileNamePtr[0] = strlen ((char*)&fileNamePtr[1]);
												
			//CFAllocatorDeallocate (kCFAllocatorDefault, (void*)cfStringRef);
			CFRelease (cfStringRef);
										
			}	// end "if (errCode == noErr)" 
	#endif	// defined multispec_mac

	#if defined multispec_win || defined multispec_wx
		UInt8									uft8String[_MAX_PATH];
		FileStringPtr						localFileNamePtr;
		WideFileStringPtr					inputFileStringPtr;
		
		SInt16								nameLength,
												pathLength;
		
		
				// Need to convert input unicode path name to multibyte string.
		
		inputFileStringPtr = fileAsFSRefPtr[0].hidden;
		int size = (int)inputFileStringPtr[0];

		ConvertUnicodeStringToMultibyteString (inputFileStringPtr,
															uft8String,
															size,
															&pathLength);
															
				// Make sure c terminator is at the end of the string.
			
		uft8String[pathLength] = 0;
	
		if (pathLength > 0)
			{              
			localFileNamePtr = &uft8String[pathLength-1];
			
			nameLength = 0;
			#if defined multispec_win
				while ((nameLength < pathLength) &&	(*localFileNamePtr != '\\'))
			#endif	// defined multispec_win

			#if defined multispec_wx
				while ((nameLength < pathLength) &&	(*localFileNamePtr != '/') && 
																	(*localFileNamePtr != '\\'))
			#endif	// defined multispec_wx
				{
				localFileNamePtr--;
				nameLength++;
				
				}	// end "while ((nameLength < pathLength) && ..."
				
			if (nameLength > 0)
				localFileNamePtr++;

			errCode = noErr;
				
			}	// end "if (pathLength > 0)"	  
			
		else	// pathLength == 0
			{
			localFileNamePtr = uft8String;
			nameLength = 0;
			
			errCode = 1;
			
			}	// else pathLength == 0
		
				// Now copy the file name to the output file string location which
				// includes the string count in the first 2 bytes.

		//CtoPstring (localFileNamePtr, fileNamePtr);
		size_t bytesToMove = strlen ((CharPtr)localFileNamePtr);
		bytesToMove = MIN (bytesToMove, 253);
		memmove (&fileNamePtr[2], localFileNamePtr, bytesToMove);
		SetFileStringLength (fileNamePtr, bytesToMove);
	#endif	// defined multispec_win || defined multispec_wx
		
	return (errCode);
	
}	// end "GetFileNameFromFSRef"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void* GetFileNameCPointerFromFileHandle
//
//	Software purpose:	The purpose of this routine is to return a C string pointer to the
//							file name from the input file information handle. 
//							This is a helper routine for an overload where returnCode
//							defaults to 'kDefaultFileStringCode'.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 008/31/2017
//	Revised By:			Larry L. Biehl			Date: 08/31/2017

void* GetFileNameCPointerFromFileHandle (
				Handle								fileInfoHandle)

{  
	return (GetFileNameCPointerFromFileHandle (fileInfoHandle, kDefaultFileStringCode));
	
}	// end "GetFileNameCPointerFromFileHandle"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void* GetFileNameCPointerFromFileHandle
//
//	Software purpose:	The purpose of this routine is to return a C string pointer to the
//							file name from the input file information handle. The string
//							format is controlled by the return code which could be ascii or
//							unicode (wide char).
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 01/25/2013
//	Revised By:			Larry L. Biehl			Date: 03/01/2017	

void* GetFileNameCPointerFromFileHandle (
				Handle								fileInfoHandle,
				SInt16								returnCode)

{  
	FileInfoPtr							fileInfoPtr;
	
	
	fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
	
	return (GetFileNameCPointerFromFileInfo (fileInfoPtr, returnCode));
	
}	// end "GetFileNameCPointerFromFileHandle"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void* GetFileNameCPointerFromFileInfo
//
//	Software purpose:	The purpose of this routine is to return a C string pointer to the
//							file name from the input file information handle. 
//							This is a helper routine for the overload where returnCode
//							defaults to 'kDefaultFileStringCode'.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 08/31/2017
//	Revised By:			Larry L. Biehl			Date: 08/31/2017

void* GetFileNameCPointerFromFileInfo (
				FileInfoPtr							fileInfoPtr)

{
	return (GetFileNameCPointerFromFileInfo (fileInfoPtr, kDefaultFileStringCode));
	
}	// end "GetFileNameCPointerFromFileInfo"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void* GetFileNameCPointerFromFileInfo
//
//	Software purpose:	The purpose of this routine is to return a C string pointer to the
//							file name from the input file information structure. The string
//							format is controlled by the return code which could be ascii or
//							unicode (wide char).
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 09/11/1995
//	Revised By:			Larry L. Biehl			Date: 01/03/2017

void* GetFileNameCPointerFromFileInfo (
				FileInfoPtr							fileInfoPtr,
				SInt16								returnCode)

{  
	void*									fileNamePtr = NULL;
	
	
	if (fileInfoPtr != NULL)
		{            
		CMFileStream* fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
		
		fileNamePtr = GetFileNameCPointerFromFileStream (fileStreamPtr, returnCode);
		
		}	// end "if (fileInfoPtr != NULL)" 

	return (fileNamePtr);
	
}	// end "GetFileNameCPointerFromFileInfo"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetFileNameCPointerFromFileStream
//
//	Software purpose:	The purpose of this routine is to return a C string pointer to the
//							file name from the input file stream structure or class. 
//							This is a helper routine for the overload where returnCode
//							defaults to 'kDefaultFileStringCode'.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 09/01/2017
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

void* GetFileNameCPointerFromFileStream (
				CMFileStream*						fileStreamPtr)

{  
	return (GetFileNameCPointerFromFileStream (fileStreamPtr, kDefaultFileStringCode));
	
}	// end "GetFileNameCPointerFromFileStream" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetFileNameCPointerFromFileStream
//
//	Software purpose:	The purpose of this routine is to return a C string pointer to the
//							file name from the input file stream structure or class. The string
//							format is controlled by the return code which could be ascii or
//							unicode (wide char).
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 11/01/1995
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

void* GetFileNameCPointerFromFileStream (
				CMFileStream*						fileStreamPtr,
				SInt16								returnCode)

{  
	void*									fileNamePtr = NULL;
	
	
	if (fileStreamPtr != NULL)
		{                                  
		#if defined multispec_mac  
			if (returnCode == kReturnASCII)
				fileNamePtr = &fileStreamPtr->fileName[1];
			else
				fileNamePtr = (FileStringPtr)fileStreamPtr->uniFileName.unicode;
		#endif	// defined multispec_mac 
	      
		#if defined multispec_win || defined multispec_wx
			fileNamePtr = fileStreamPtr->GetFileNameCPtr (returnCode);
		#endif	// defined multispec_win || defined multispec_wx
		}	// end "if (fileStreamPtr != NULL)" 

	return (fileNamePtr);
	
}	// end "GetFileNameCPointerFromFileStream" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetFileNamePPointerFromFileHandle
//
//	Software purpose:	The purpose of this routine is to return a Pascal string pointer 
//							to the file name from the input file handle. 
//							This is a helper routine for the overload where returnCode
//							defaults to 'kDefaultFileStringCode'.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 09/01/2017
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

void* GetFileNamePPointerFromFileHandle (
				Handle								fileInfoHandle,
				SignedByte*							handleStatusPointer)

{  
	return (GetFileNamePPointerFromFileHandle (fileInfoHandle, 
																handleStatusPointer,
																kDefaultFileStringCode));
	
}	// end "GetFileNamePPointerFromFileHandle" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetFileNamePPointerFromFileHandle
//
//	Software purpose:	The purpose of this routine is to return a Pascal string pointer 
//							to the file name from the input file handle. The string
//							format is controlled by the return code which could be ascii or
//							unicode (wide char).
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 09/29/1995
//	Revised By:			Larry L. Biehl			Date: 03/09/2017

void* GetFileNamePPointerFromFileHandle (
				Handle								fileInfoHandle,
				SignedByte*							handleStatusPointer,
				SInt16								returnCode)

{  
	void*									fileNamePtr = NULL;
	FileInfoPtr							fileInfoPtr;
	
				
	*handleStatusPointer = -1;
	
	fileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (fileInfoHandle,
																			handleStatusPointer);
																	
	fileNamePtr = GetFileNamePPointerFromFileInfo (fileInfoPtr, returnCode);

	return (fileNamePtr);
	
}	// end "GetFileNamePPointerFromFileHandle" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetFileNamePPointerFromFileInfo
//
//	Software purpose:	The purpose of this routine is to return a Pascal string pointer 
//							to the file name from the input file structure. The string
//							format is controlled by the return code which could be ascii or
//							unicode (wide char).
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 09/01/2017
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

void* GetFileNamePPointerFromFileInfo (
				FileInfoPtr							fileInfoPtr)

{  
	return (GetFileNamePPointerFromFileInfo (fileInfoPtr, kDefaultFileStringCode));
	
}	// end "GetFileNamePPointerFromFileInfo" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetFileNamePPointerFromFileInfo
//
//	Software purpose:	The purpose of this routine is to return a Pascal string pointer 
//							to the file name from the input file structure. The string
//							format is controlled by the return code which could be ascii or
//							unicode (wide char).
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 09/29/1995
//	Revised By:			Larry L. Biehl			Date: 03/09/2017

void* GetFileNamePPointerFromFileInfo (
				FileInfoPtr							fileInfoPtr,
				SInt16								returnCode)

{  
	void*								fileNamePtr = NULL;
	
	
	if (fileInfoPtr != NULL)
		{              
		CMFileStream* fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
		
		fileNamePtr = GetFileNamePPointerFromFileStream (fileStreamPtr, returnCode);
		                                   		
		}	// end "if (fileInfoPtr != NULL)" 

	return (fileNamePtr);
	
}	// end "GetFileNamePPointerFromFileInfo" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetFileNamePPointerFromFileStream
//
//	Software purpose:	The purpose of this routine is to return a Pascal string pointer 
//							to the file name from the input file stream structure or class. 
//							The string format is controlled by the return code which could 
//							be ascii or unicode (wide char).
//							This routine is a helper for an overload where the returnCode
//							defaults to 'kDefaultFileStringCode'.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 08/31/2017
//	Revised By:			Larry L. Biehl			Date: 08/31/2017

void* GetFileNamePPointerFromFileStream  (
				CMFileStream*						fileStreamPtr)

{  
	return (GetFileNamePPointerFromFileStream (fileStreamPtr, kDefaultFileStringCode));
	
}	// end "GetFileNamePPointerFromFileStream"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetFileNamePPointerFromFileStream
//
//	Software purpose:	The purpose of this routine is to return a Pascal string pointer 
//							to the file name from the input file stream structure or class. 
//							The string format is controlled by the return code which could 
//							be ascii or unicode (wide char).
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 10/24/1995
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

void* GetFileNamePPointerFromFileStream  (
				CMFileStream*						fileStreamPtr,
				SInt16								returnCode)

{  
	void*									fileNamePtr = NULL;
	
	
	if (fileStreamPtr != NULL)
		{                         
		#if defined multispec_mac
			if (returnCode == kReturnASCII)                
	   		fileNamePtr = &fileStreamPtr->fileName[0];
			else
				//fileNamePtr = fileStreamPtr->uniFileName.unicode;
				fileNamePtr = fileStreamPtr->uniPascalFileName;
				if (fileStreamPtr->uniPascalFileName[0] == 0)
					{
							// Copy c version of uniFileName to this local
					
					strncpy ((char*)&fileStreamPtr->uniPascalFileName[1],
									(char*)fileStreamPtr->uniFileName.unicode,
									2*fileStreamPtr->uniFileName.length);
					
					}	// end "if (fileStreamPtr->uniPascalFileName[0] == 0)"
		#endif	// defined multispec_mac
		
		#if defined multispec_win || defined multispec_wx
			fileNamePtr = fileStreamPtr->GetFileNamePPtr (returnCode);
		#endif	// defined multispec_win || defined multispec_wx
		
		}	// end "if (fileInfoPtr != NULL)" 

	return (fileNamePtr);
	
}	// end "GetFileNamePPointerFromFileStream" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetFilePathFromFSRef
//
//	Software purpose:	The purpose of this routine is to get the file path from the
//							input FSRef. The path will be returned as a pascal name with
//							c terminator at the end.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 07/27/2018
//	Revised By:			Larry L. Biehl			Date: 04/12/2020

SInt16 GetFilePathFromFSRef (
				FSRef*								fileAsFSRefPtr,
				StringPtr							filePathPtr)

{  	
	SInt16								errCode;


	#if defined multispec_mac
		UInt8									uft8String[256];
		//CFStringRef							cfStringRef;
												
		//HFSUniStr255						uniFileName;
		
		
		filePathPtr[0] = 0;
		filePathPtr[1] = 0;
	
		errCode = FSRefMakePath (fileAsFSRefPtr,
											(UInt8*)uft8String,
											255);
		/*
		errCode = FSGetCatalogInfo (fileAsFSRefPtr,
												kFSCatInfoNone,
												NULL,
												&uniFileName,
												NULL,
												NULL);
	
		if (errCode == noErr)
			{
			cfStringRef = CFStringCreateWithCharacters (kCFAllocatorDefault,
																		uniFileName.unicode,
																		uniFileName.length);
			
			CFStringGetCString (cfStringRef,
										(char*)&filePathPtr[1],
										(CFIndex)255,
										kCFStringEncodingUTF8);
			filePathPtr[0] = strlen ((char*)&filePathPtr[1]);
												
			//CFAllocatorDeallocate (kCFAllocatorDefault, (void*)cfStringRef);
			CFRelease (cfStringRef);
										
			}	// end "if (errCode == noErr)"
		*/
	
				// Now copy the path name to the output file string location.

		CtoPstring (uft8String, filePathPtr);
	#endif	// defined multispec_mac

	#if defined multispec_win	|| defined multispec_wx
		UInt8									uft8String[_MAX_PATH];
		WideFileStringPtr					inputFileStringPtr;
		
		SInt16								pathLength;
		
		
				// Need to convert input unicode path name to multibyte string.
		
		inputFileStringPtr = fileAsFSRefPtr[0].hidden;
		int size = (int)inputFileStringPtr[0];

		ConvertUnicodeStringToMultibyteString (inputFileStringPtr,
															uft8String,
															size,
															&pathLength);
																												
				// Make sure c terminator is at the end of the string.
			
		uft8String[pathLength] = 0;

		if (pathLength > 0)
			errCode = noErr;
	
		else	// pathLength == 0
			errCode = 1;
	
				// Now copy the path name to the output file string location.

		//CtoPstring (uft8String, filePathPtr);
		size_t bytesToMove = strlen ((CharPtr)uft8String);
		memmove (&filePathPtr[2], uft8String, bytesToMove);
		SetFileStringLength (filePathPtr, bytesToMove);
	#endif	// defined multispec_win
		
	return (errCode);
	
}	// end "GetFilePathFromFSRef"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		int GetFilePathLengthFromFileStream
//
//	Software purpose:	The purpose of this routine is to get the length of the full
//							file path name for the specified file coding.
//							This is a helper routine for the overload where returnCode
//							defaults to kDefaultFileStringCode
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/09/2020
//	Revised By:			Larry L. Biehl			Date: 04/09/2020

int GetFilePathLengthFromFileStream (
				CMFileStream* 						fileStreamPtr)

{
	return (GetFilePathLengthFromFileStream (fileStreamPtr, kDefaultFileStringCode));
	
}	// end "GetFilePathLengthFromFileStream"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetFilePathLengthFromFileStream
//
//	Software purpose:	The purpose of this routine is to get a pointer
//							to the full file path represented by the input file
//							stream pointer. The output pointer will represent a
//							pascal pointer.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/09/2020
//	Revised By:			Larry L. Biehl			Date: 04/09/2020

int GetFilePathLengthFromFileStream (
				CMFileStream* 						fileStreamPtr,
				SInt16								returnCode)

{
	int									filePathLength = 0;
	
	
	if (fileStreamPtr != NULL)
		{
		#if defined multispec_wx
			filePathLength = fileStreamPtr->GetPathFileLength (returnCode);
		#endif	// defined multispec_wx

		#if defined multispec_mac
			if (returnCode == kReturnASCII)
				filePathLength = (void*)fileStreamPtr->fileName;
			else
				filePathLength = (void*)fileStreamPtr->uniFileName.unicode;
		#endif	// defined multispec_mac
	              
		#if defined multispec_win
			filePathLength = fileStreamPtr->GetPathFileLength (returnCode);
		#endif	// defined multispec_win
		
		}	// end "if (fileStreamPtr != NULL)"

	return (filePathLength);
	
}	// end "GetFilePathLengthFromFileStream"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetFilePathPPointerFromFileInfo
//
//	Software purpose:	The purpose of this routine is to return a Pascal string pointer 
//							to the file name from the input file structure.
//							This is a helper routine for the overload where returnCode
//							defaults to kDefaultFileStringCode
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 08/31/2017
//	Revised By:			Larry L. Biehl			Date: 08/31/2017

void* GetFilePathPPointerFromFileInfo (
				FileInfoPtr							fileInfoPtr)

{  
	return (GetFilePathPPointerFromFileInfo (fileInfoPtr, kDefaultFileStringCode));
	
}	// end "GetFilePathPPointerFromFileInfo"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetFilePathPPointerFromFileInfo
//
//	Software purpose:	The purpose of this routine is to get a pointer
//							to the full file path represented by the input file
//							information pointer. The output pointer will represent a
//							pascal pointer.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 01/16/1996
//	Revised By:			Larry L. Biehl			Date: 03/13/2017

void* GetFilePathPPointerFromFileInfo (
				FileInfoPtr							fileInfoPtr,
				SInt16								returnCode)

{  
	void*								fileNamePtr = NULL;
	
	
	if (fileInfoPtr != NULL)
		{              
		CMFileStream* fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
		
		fileNamePtr = GetFilePathPPointerFromFileStream (fileStreamPtr,
																			returnCode);
		
		}	// end "if (fileInfoPtr != NULL)" 

	return (fileNamePtr);
	
}	// end "GetFilePathPPointerFromFileInfo"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetFilePathPPointerFromFileStream
//
//	Software purpose:	The purpose of this routine is to get a pointer
//							to the full file path represented by the input file
//							stream pointer. The output pointer will represent a
//							pascal pointer.
//							This is a helper routine for the overload where returnCode
//							defaults to kDefaultFileStringCode
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 08/31/2017
//	Revised By:			Larry L. Biehl			Date: 04/09/2020

void* GetFilePathPPointerFromFileStream (
				CMFileStream* 						fileStreamPtr)

{  
	return (GetFilePathPPointerFromFileStream (fileStreamPtr,
																kDefaultFileStringCode));
	
}	// end "GetFilePathPPointerFromFileStream"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetFilePathPPointerFromFileStream
//
//	Software purpose:	The purpose of this routine is to get a pointer
//							to the full file path represented by the input file
//							stream pointer. The output pointer will represent a
//							pascal pointer.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 10/05/1995
//	Revised By:			Larry L. Biehl			Date: 04/09/2020

void* GetFilePathPPointerFromFileStream (
				CMFileStream* 						fileStreamPtr,
				SInt16								returnCode)

{                      
	void*									filePathPtr = NULL;
	
	if (fileStreamPtr != NULL)
		{
		#if defined multispec_wx
			filePathPtr = fileStreamPtr->GetFilePathPPtr (returnCode);
		#endif	// defined multispec_wx

		#if defined multispec_mac
			if (returnCode == kReturnASCII)
				filePathPtr = (void*)fileStreamPtr->fileName;
			else
				filePathPtr = (void*)fileStreamPtr->uniFileName.unicode;
		#endif	// defined multispec_mac
	              
		#if defined multispec_win
			filePathPtr = fileStreamPtr->GetFilePathPPtr (returnCode);
		#endif	// defined multispec_win
				
		}	// end "if (fileStreamPtr != NULL)" 

	return (filePathPtr);
	
}	// end "GetFilePathPPointerFromFileStream"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt32 GetFileType
//
//	Software purpose:	The purpose of this routine is to return the file
//							type represented by the input file stream pointer.
//
//	Parameters in:		file stream pointer
//
//	Parameters out:	None
//
//	Value Returned:	the file type
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 10/25/1995
//	Revised By:			Larry L. Biehl			Date: 10/25/1995	

SInt32 GetFileType (
				CMFileStream*						fileStreamPtr)

{  
	SInt32								fileType = -1;
	
	
	if (fileStreamPtr != NULL)
		{                               
		#if defined multispec_mac                       
			fileType = fileStreamPtr->type;
		#endif	// defined multispec_mac 
	
      #if defined multispec_win || defined multispec_wx
			fileType = fileStreamPtr->mFileType;
		#endif	// defined multispec_win || defined multispec_wx
		
		}	// end "if (fileStreamPtr != NULL)" 

	return (fileType);
	
}	// end "GetFileType"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt32 GetFileTypeAndCreator
//
//	Software purpose:	The purpose of this routine is to obtain the file type and
//							the creator for the input file stream.
//
//	Parameters in:		file stream pointer
//
//	Parameters out:	None
//
//	Value Returned:	the file type
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 12/07/2012
//	Revised By:			Larry L. Biehl			Date: 12/07/2012	

SInt16 GetFileTypeAndCreator (
				CMFileStream*						fileStreamPtr)

{  
	SInt16								errCode = noErr;
	
	#if defined multispec_mac
		FSCatalogInfo						fsCatalogInfo;


				// Get the file type and creator					
		
		if (fileStreamPtr->fSSpecFlag)
			{
			errCode = FSpGetFInfo ((FSSpec*)fileStreamPtr, &gFinderInfo);

			if (errCode == noErr)
				{
				fileStreamPtr->creator = gFinderInfo.fdCreator;
				fileStreamPtr->type = gFinderInfo.fdType;

				}	// end "if (errCode == noErr)"
			
			}	// end "if (fileStreamPtr->fSSpecFlag)"

		else	// !fileStreamPtr->fSSpecFlag
			{
			errCode = FSGetCatalogInfo (&fileStreamPtr->fsRef,
													kFSCatInfoFinderInfo,
													&fsCatalogInfo,
													NULL,
													NULL,
													NULL);													

			if (errCode == noErr)
				{
				fileStreamPtr->creator = 
											((FileInfo*)fsCatalogInfo.finderInfo)->fileCreator;
				fileStreamPtr->type = ((FileInfo*)fsCatalogInfo.finderInfo)->fileType;

				}	// end "if (errCode == noErr)"
			
			}	// end else	!fileStreamPtr->fSSpecFlag
	#endif	// defined multispec_mac
	
	#if defined multispec_win		
	#endif	// defined multispec_mac

	return (errCode);
	
}	// end "GetFileTypeAndCreator"	




//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt64 GetFilePositionOffset
//
//	Software purpose:	The purpose of this routine is to determine the
//							offset in the file for the input line, the number
//							of samples to read and the number of bytes to read.
//
//	Parameters in:				
//
//	Parameters out:	
//
//	Value Returned:	 
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 05/29/1996
//	Revised By:			Larry L. Biehl			Date: 07/13/2009

SInt64 GetFilePositionOffset (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				FileInfoPtr 						fileInfoPtr,
				UInt32								lineNumber, 
				SInt16								channelNumber, 
				UInt32								columnStart, 
				UInt32								columnEnd,
				UInt32*								numberSamplesPtr,
				UInt32*								countPtr,
				Boolean*								endHalfBytePtr)

{
	SInt64								posOff = 0;
	
	BlockFormatPtr						blockFormatPtr;
	HierarchalFileFormatPtr			hfaPtr;
	
	UInt32 								blockIndex,
											blockLineStart,
			 								halfBytes,
											segmentedEndOffset,
											segmentedStartOffset;
			
	Boolean								BILSpecialFlag,
											startHalfByte;
	
	
			// Initialize local variables.
			
	BILSpecialFlag = fileIOInstructionsPtr->oneReadFlag;
				
	if (BILSpecialFlag)
		{
		columnStart = 1;
		columnEnd = fileInfoPtr->numberColumns;
		
		}	// end "if (BILSpecialFlag)"
	
			// Get the number of samples to read and the initial number of
			// bytes to read.
	
	*numberSamplesPtr = columnEnd - columnStart + 1;
	*countPtr =  *numberSamplesPtr * fileInfoPtr->numberBytes;
	*endHalfBytePtr = FALSE;
					 							
			// Get the relative position to start reading with respect to			
			// the start of the file.															
			
	switch (fileInfoPtr->bandInterleave)
		{
		case kBIL:
			posOff = (SInt64)fileInfoPtr->numberHeaderBytes +
			
									 	(SInt64)(lineNumber-1) *
									 			fileInfoPtr->bytesPer1lineAllChans +
									 				
										fileInfoPtr->numberPreLineBytes + 
									 			channelNumber *
													fileInfoPtr->bytesPer1line1chan +
											
										fileInfoPtr->numberPreChannelBytes + 
									 			(columnStart-1) * fileInfoPtr->numberBytes;
			
					// If 4-bit data and the number of columns in the image data	
					// is odd, then modify posOff to allow for 1/2 bytes.				
					
			if (fileInfoPtr->numberBits == 4)
				{
				halfBytes = (lineNumber-1) * 
						fileInfoPtr->numberChannels * fileInfoPtr->numberColumns +
										channelNumber * fileInfoPtr->numberColumns +	
																						columnStart - 1;
						
				startHalfByte = (Boolean)(halfBytes & 0x00000001);
				
				posOff -= (halfBytes + 1)/2;
				
						// Adjust the number of bytes to read in and determine		
						// end half byte flag.													
				
				*endHalfBytePtr = 0;
				*countPtr = (*countPtr + 1)/2;
				
				if (startHalfByte)
					*endHalfBytePtr = !(*numberSamplesPtr & 0x00000001);
									
				else	// !startHalfByte 
					*endHalfBytePtr = (Boolean)(*numberSamplesPtr & 0x00000001);
									
				if (startHalfByte && *endHalfBytePtr)
					(*countPtr)++;
				
				}	// end "if (fileInfoPtr->numberBits == 4)" 
	
					// Handle case when file band interleave format is BIL and all	
					// channels and columns are to be read in at one time.			
					
			if (BILSpecialFlag)
				{
				*numberSamplesPtr *= fileIOInstructionsPtr->bilSpecialNumberChannels;
				*countPtr *= fileIOInstructionsPtr->bilSpecialNumberChannels;
				
				}	// end "if (BILSpecialFlag)" 
			break;
		
		case kBSQ:
			if (fileInfoPtr->nonContiguousStripsFlag)
				{
				hfaPtr = &fileInfoPtr->hfaPtr[channelNumber];
				
				blockIndex = channelNumber * hfaPtr->blocksPerChannel + 
																(lineNumber-1)/hfaPtr->blockHeight;
				blockFormatPtr = &fileInfoPtr->blockFormatPtr[blockIndex];
				
				posOff = (SInt64)blockFormatPtr->blockOffsetBytes;
				
				*countPtr = hfaPtr->numberBlocksRead * blockFormatPtr->blockSize;
					
				}	// end "if (fileInfoPtr->nonContiguousStripsFlag)"
			
			else	// !fileInfoPtr->nonContiguousStripsFlag
				posOff = fileInfoPtr->numberHeaderBytes +
								 				
								 	 (SInt64)channelNumber *
												fileInfoPtr->bytesPer1chanAllLines +
		
				 					fileInfoPtr->numberPreChannelBytes +
								 	  (lineNumber-1) *
								 			fileInfoPtr->bytesPer1line1chan +
								 		
								 		(columnStart-1) *
				 							fileInfoPtr->numberBytes +
				 									fileInfoPtr->numberPreLineBytes;
			break;
			
		case kBIS:
					// Note: For BIS type files all channels for the requested 		
					// columns will be read into the buffer.								
					
			posOff = fileInfoPtr->numberHeaderBytes +
			
									 (SInt64)(lineNumber-1) *
									 		fileInfoPtr->bytesPer1lineAllChans +
											
									fileInfoPtr->numberPreLineBytes + 
									 		(columnStart-1) * fileInfoPtr->numberChannels *
									 			fileInfoPtr->numberBytes;
									 			
			*countPtr *= fileInfoPtr->numberChannels;
			*numberSamplesPtr *= fileInfoPtr->numberChannels;
			
			if (fileInfoPtr->blockedFlag)
				{
				hfaPtr = fileInfoPtr->hfaPtr;
				
				blockLineStart = (lineNumber-1)/hfaPtr->blockHeight;
				posOff = hfaPtr->firstColumnStartByte + hfaPtr->blockOffset * 
											blockLineStart * hfaPtr->numberBlockWidths;
											
				*countPtr = hfaPtr->numberBlocksRead * hfaPtr->blockOffset;
				
				}	// end "if (fileInfoPtr->blockFlag)"
			break;
		
		case kBNonSQ:
			posOff = (SInt64)fileInfoPtr->hfaPtr[channelNumber].layerOffsetBytes +
				
						 					fileInfoPtr->numberPreChannelBytes +
										 	  (lineNumber-1) *
										 			fileInfoPtr->bytesPer1line1chan +
										 		
										 		(columnStart-1) *
						 							fileInfoPtr->numberBytes +
						 									fileInfoPtr->numberPreLineBytes;
			break;
		
		case kBNonSQBlocked:
		case kBIBlock:
			if (fileInfoPtr->hfaPtr != NULL)
				{
				hfaPtr = &fileInfoPtr->hfaPtr[channelNumber];
			
				blockLineStart = (lineNumber-1)/hfaPtr->blockHeight;
			
				posOff = (SInt64)hfaPtr->firstColumnStartByte + 
										(SInt64)hfaPtr->blockOffset * 
													blockLineStart * hfaPtr->numberBlockWidths;
			
				*countPtr = hfaPtr->numberBlocksRead * hfaPtr->blockOffset;
				
				}	// end "if (fileInfoPtr->hfaPtr != NULL)"
			break;
			
		}	// end "switch (fileInfoPtr->bandInterleave)"
		
	if (fileInfoPtr->format == kGAIAType)
		{
				// Allow for 'segmented bytes'.												
		
		segmentedStartOffset = GetNumberGAIALineSegments (columnStart) * 2;
		segmentedEndOffset = GetNumberGAIALineSegments (columnEnd) * 2;
		
		posOff += segmentedStartOffset;
		*countPtr += (segmentedEndOffset - segmentedStartOffset);
		
		}	// end "if (fileInfoPtr->format == kGAIAType)"

	return (posOff); 
	
}	// end "GetFilePositionOffset"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetFSSpecFlag
//
//	Software purpose:	The purpose of this routine is to return the input file 
//							directory id.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 10/31/2002
//	Revised By:			Larry L. Biehl			Date: 10/31/2002	

Boolean GetFSSpecFlag (
				FileInfoPtr							fileInfoPtr)

{  
	Boolean								fSSpecFlag = FALSE;
	
	
	CMFileStream* fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
	
	
	if (fileStreamPtr != NULL)
		{              
		#if defined multispec_mac                       
			fSSpecFlag = fileStreamPtr->fSSpecFlag;
		#endif	// defined multispec_mac 
	              
      #if defined multispec_win | defined multispec_wx
	 
		#endif	// defined multispec_win  | defined multispec_wx
		
		}	// end "if (fileInfoPtr != NULL)" 
	

	return (fSSpecFlag);
	
}	// end "GetFSSpecFlag"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetFSSpecFlag
//
//	Software purpose:	The purpose of this routine is to return the input file 
//							volume reference number.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 10/31/2002
//	Revised By:			Larry L. Biehl			Date: 10/31/2002	

Boolean GetFSSpecFlag (
				Handle								fileInfoHandle)

{  
	Boolean								fSSpecFlag = FALSE;
	
	
	FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
													
	fSSpecFlag = GetFSSpecFlag (fileInfoPtr);

	return (fSSpecFlag);
	
}	// end "GetFSSpecFlag"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetLine
//
//	Software purpose:	The purpose of this routine is to read the
//							requested line and channel of image data into a
//							buffer.
//
//	Parameters in:				
//
//	Parameters out:	Number of samples loaded into buffer
//
//	Value Returned:	Error code for file operations. 
//
// Called By:			GetLineOfData in SFileIO.cpp
//							GetClassesFromHistogram in SOpenImage.cpp
//							DisplayColorThematicImage in SDisplayThematic.cpp
//							CreateClusterMaskFile in SCluster.cpp
//							ConvertImagePixelsToClassNumbers in SFieldsToThematicFile.cpp
//							ConvertShapeToClassNumber in SShapeToThematic.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/23/1988
//	Revised By:			Larry L. Biehl			Date: 06/06/2012

SInt16 GetLine (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				FileInfoPtr							fileInfoPtr,
				UInt32								lineNumber, 
				SInt16								channelNumber,  
				UInt32								columnStart, 
				UInt32								columnEnd,
				UInt32*								numberSamplesPtr, 
				HUCharPtr							fileIOBufferPtr)

{
	SInt64								posOff;
	
	CMFileStream*						fileStreamPtr;
	HierarchalFileFormatPtr			hfaPtr = NULL;
	HUCharPtr							readBufferPtr; 
	
	UInt32								channelStartIndex,
											count,
											readLineNumber,
											segmentedEndOffset,
											segmentedStartOffset;
	
	SInt16								errCode = noErr;
	
	#if include_hdf_capability
		SInt16								hdfErrCode;
	#endif		// include_hdf_capability
			
	Boolean								endHalfByte,	
											readLineFlag = TRUE;
	
	  	
  			// Get total number of samples to be read from disk.						
  	
  	if (columnEnd < columnStart || lineNumber == 0 || columnEnd == 0)		
																							return (0);
																					
	if (fileInfoPtr->format == kGAIAType)
																							return (-1);
	
			// Initialize local variables.
				
	readBufferPtr = fileIOBufferPtr;
																					
	if (fileInfoPtr->blockedFlag)
		{
		if (fileInfoPtr->hfaPtr == NULL)
																							return (-1);
		
		channelStartIndex = 0;
		if (fileInfoPtr->bandInterleave == kBNonSQBlocked ||
				(fileInfoPtr->bandInterleave == kBSQ && 
														fileInfoPtr->nonContiguousStripsFlag))
			{
			hfaPtr = &fileInfoPtr->hfaPtr[channelNumber];
			channelStartIndex = 0;
			
			}	// end "if (fileInfoPtr->bandInterleave == kBNonSQBlocked)"
			
		else	// fileInfoPtr->bandInterleave == kBIBlock || 
					//				fileInfoPtr->bandInterleave == kBIS
			{
			hfaPtr = fileInfoPtr->hfaPtr;
			
			if (fileInfoPtr->bandInterleave == kBIBlock)
				channelStartIndex = channelNumber;
			
					// Set channel number to start reading at the first channel
					
			channelNumber = 0;
			
			}	// end "else fileInfoPtr->bandInterleave == kBIBlock"
		
				// Check if line needs to be read from disk.
				
		if (lineNumber >= hfaPtr->firstLineRead && lineNumber <= hfaPtr->lastLineRead)
			readLineFlag = FALSE;
			
		if (readLineFlag)
			{
					// Set up correct buffer to read the data into. It will be
					// a buffer to hold several lines.
					
			readBufferPtr = (HUCharPtr)hfaPtr->tiledBufferPtr;
			
			}	// end "if (readLineFlag)"
			
		else	// !readLineFlag
			{
			*numberSamplesPtr = columnEnd - columnStart + 1;
			if (fileInfoPtr->bandInterleave == kBIS)
				*numberSamplesPtr *= fileInfoPtr->numberChannels;
			
			}	// end "else !readLineFlag"
		
		}	// end "if (fileInfoPtr->blockedFlag)"
			
	if (readBufferPtr == NULL)
																							return (-1);
																					
	readLineNumber = lineNumber;		
	if (fileInfoPtr->treatLinesAsBottomToTopFlag)
		readLineNumber = fileInfoPtr->numberLines - lineNumber + 1;
													
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
	if (readLineFlag)
		{				
		#if include_gdal_capability
			if (fileInfoPtr->gdalDataSetH != NULL)
				{
				int channelNumber4 = channelNumber + 1;
				errCode = GetGDALLineOfData (fileInfoPtr,	
														readBufferPtr,
														readLineNumber,
														columnStart,
														columnEnd,
														1,
														&channelNumber4,
														numberSamplesPtr,
														FALSE);
				
				}	// end "if (fileInfoPtr->gdalDataSetH != NULL)"
				
			else if (!fileInfoPtr->callGetHDFLineFlag)
		#endif	// include_gdal_capability

		#if !include_gdal_capability
			if (!fileInfoPtr->callGetHDFLineFlag)
		#endif	// !include_gdal_capability
			{
			if (gUseThreadedIOFlag)
				{
				errCode = GetLineFileIOThread (fileIOInstructionsPtr,
															fileInfoPtr, 
															readLineNumber, 
															channelNumber, 
															columnStart, 
															columnEnd,
															numberSamplesPtr, 
															&fileIOBufferPtr);
															 
				count = fileIOInstructionsPtr->count;
				
				}	// end "if (gUseThreadedIOFlag)"
				
			else	// !gUseThreadedIOFlag
				{
						// Get the relative position to start reading with respect to			
						// the start of the file.					
						
				posOff = GetFilePositionOffset (fileIOInstructionsPtr, 
															fileInfoPtr, 
															readLineNumber, 
															channelNumber, 
															columnStart, 
															columnEnd,
															numberSamplesPtr,
															&count,
															&endHalfByte);
															
				errCode = MSetMarker (
										fileStreamPtr, fsFromStart, posOff, kNoErrorMessages);

				if (errCode == noErr)
					errCode = MReadData (fileStreamPtr, 
														&count, 
														readBufferPtr, 
														kNoErrorMessages);
														
				if (errCode == noErr && fileInfoPtr->blockedFlag)
					{
					hfaPtr->firstLineRead = 
								((readLineNumber-1)/hfaPtr->blockHeight) * hfaPtr->blockHeight + 1;
					hfaPtr->lastLineRead = hfaPtr->firstLineRead + hfaPtr->blockHeight - 1;
					
					//hfaPtr->nextReadStartByte += 
					//								hfaPtr->blockSize * hfaPtr->numberBlockWidths;
					
					}	// end "if (errCode == noErr && fileInfoPtr->blockedFlag)"
					
				}	// end "else !gUseThreadedIOFlag" 
				
			}	// end "else if (!fileInfoPtr->callGetHDFLineFlag)"
		
		#if include_hdf_capability
			else	// fileInfoPtr->callGetHDFLineFlag
				{
						// For now this will only happen with HDF or CDF formatted files.
						
				hdfErrCode = GetHDF4Line (fileInfoPtr,
													fileInfoPtr->numberChannels,
													readLineNumber, 
													channelNumber, 
													columnStart, 
													columnEnd, 
													numberSamplesPtr,
													readBufferPtr);
													
				if (hdfErrCode != 0)
					errCode = -1;			
				
				}	// end "else fileInfoPtr->callGetHDFLineFlag"
		#endif		// include_hdf_capability
			
		}	// end "if (readLineFlag)"												
	
	if (errCode == noErr)
		{
		if (fileInfoPtr->blockedFlag)
			{
			PackBlockedData (readLineNumber,
									columnStart, 
									columnEnd,
									channelStartIndex,
									fileInfoPtr,
									hfaPtr,
									fileIOBufferPtr);
			
					// count is just used for 4-bit data from here on.
					
			if (fileInfoPtr->numberBits == 4)
				{						
				count = (columnEnd - columnStart + 2)/2;
				
				if (!(columnStart & 0x00000001) && (columnEnd & 0x00000001))
					count++;
				
				endHalfByte = FALSE;
				if (columnEnd & 0x00000001)
					endHalfByte = TRUE;
				
				}	// end "if (fileInfoPtr->numberBits == 4)"
			
			}	// end "if (fileInfoPtr->blockedFlag)"
			
		if (fileInfoPtr->format == kGAIAType)
			{
					// Remove the 'segmented bytes'.											
			
			segmentedStartOffset = GetNumberGAIALineSegments (columnStart) * 2;
			segmentedEndOffset = GetNumberGAIALineSegments (columnEnd) * 2;												
			
			PackGAIAData (columnStart, columnEnd, (HSInt16Ptr)fileIOBufferPtr);
			count -= (segmentedEndOffset - segmentedStartOffset);
			
			}	// end "if (fileInfoPtr->format == kGAIAType && ...)" 
				
				// Swap the bytes if needed. 
				
		if (fileInfoPtr->swapBytesFlag && fileInfoPtr->numberBytes >= 2)
			SwapBytes (fileInfoPtr->numberBytes,
							fileIOBufferPtr,
							*numberSamplesPtr);
							
		if (gConvertSignedDataFlag && 
					fileInfoPtr->signedDataFlag &&
							fileInfoPtr->gdalDataSetH == NULL)
			AdjustSignedData (fileInfoPtr, fileIOBufferPtr, *numberSamplesPtr);

				// If 4 bit data, convert to 8 bit data.									
				
		if (fileInfoPtr->numberBits == 4)
			{
			HPtr						byte1Ptr;
			HPtr						byte2Ptr;
			
			UInt32					samplesLeft;
	
					// If number of samples is odd, convert the odd one and then	
					// loop through the samples two at a time.							
					// Note the lower sample number is in the lower 4 bits.			
			
			samplesLeft = *numberSamplesPtr;
			byte1Ptr = (HPtr)&fileIOBufferPtr[samplesLeft-1];
			byte2Ptr = (HPtr)&fileIOBufferPtr[count-1];
			if (endHalfByte)
				{
				*byte1Ptr = *byte2Ptr & 0x0f;
						
				samplesLeft--;
				byte1Ptr--;
				byte2Ptr--;
						
				}	// end "if (halfByteFlag)" 
				
			while (samplesLeft >= 2)
				{
						// Move high 4 bits.														
						
				*byte1Ptr = (*byte2Ptr & 0xf0)>>4;
				
				samplesLeft--;
				byte1Ptr--;
				
						// Move low 4 bits.														
				
				*byte1Ptr = *byte2Ptr & 0x0f;
				
				samplesLeft--;
				byte1Ptr--;
				byte2Ptr--;
				
				}	// end "while (samplesLeft >= 0)" 
				
			if (samplesLeft)
				*byte1Ptr = (*byte2Ptr & 0xf0)>>4;
			
			}	// end "if (fileInfoPtr->numberBits..." 
			
		}	// end "if (errCode == noErr)" 
		
	IOCheck (errCode, fileStreamPtr);
	return (errCode);
	
}	// end "GetLine"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetLineOfData
//
//	Software purpose:	The purpose of this routine is to read
//							all data for a requested line and requested
//							channels of image data into a buffer that has been
//							provided.  If the channel list pointer is NULL, then
//							load the data in for all channels.
//
//	Parameters in:				
//
//	Parameters out:	Number of samples loaded into buffer
//
//	Value Returned:	Error code for file operations. 
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 12/15/1988
//	Revised By:			Larry L. Biehl			Date: 05/06/2022

SInt16 GetLineOfData (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				UInt32								lineNumber, 
				UInt32								columnStart, 
				UInt32								columnEnd, 
				UInt32								columnInterval,
				HUCharPtr							fileIOBufferPtr, 
				HUCharPtr							dataBufferPtr)

{                                                
	FileInfoPtr							fileInfoPtr,
											localFileInfoPtr;
	
	HUCharPtr							ioBufferPtr;
											
	LayerInfoPtr						layerInfoPtr;
	
	UInt16								*channelListPtr,
											*splitChannelListPtr;
	
	WindowInfoPtr						windowInfoPtr; 
	
	UInt32								bytesChannelOffset,
											columnOffset,
											fileNumberChannels,
											index,
											numberChannels,
											numberColumnsPerChannel,
											numberSamples,
											numberSamplesRead;
	
	SInt16								channel,
											errCode,
											fileInfoIndex,
											numberBytes;
											
	UInt16								channelListIndex,
											forceOutByteCode;
											
	Boolean								callPackLineOfDataFlag,
											differentBuffersFlag,
											oneReadFlag,
											packDataFlag,
											toBISFormatFlag,
											useMultipleChannelGDALFlag;

	
	if (lineNumber <= 0  || fileIOBufferPtr == NULL)
																							return (-1);
		
			// Initialize some local variables.												
		
	numberSamples = columnEnd - columnStart + 1;
	
	useMultipleChannelGDALFlag = FALSE;
	channelListPtr = fileIOInstructionsPtr->channelListPtr;
	numberChannels = fileIOInstructionsPtr->numberChannels;
	windowInfoPtr = fileIOInstructionsPtr->windowInfoPtr;
	layerInfoPtr = fileIOInstructionsPtr->layerInfoPtr;
	fileInfoPtr = fileIOInstructionsPtr->fileInfoPtr;
	packDataFlag = fileIOInstructionsPtr->packDataFlag; 
	toBISFormatFlag = (fileIOInstructionsPtr->forceOutputFormatCode == kBIS);
	forceOutByteCode = fileIOInstructionsPtr->forceByteCode;
	oneReadFlag = fileIOInstructionsPtr->oneReadFlag;
	callPackLineOfDataFlag = fileIOInstructionsPtr->callPackLineOfDataFlag;
	differentBuffersFlag = fileIOInstructionsPtr->differentBuffersFlag;
	
	if (gProcessorCode != kDisplayProcessor &&
											numberChannels == windowInfoPtr->totalNumberChannels)
			// Note: cannot do this if Display Processer; channels may be repeated
		channelListPtr = NULL;
	
	if (oneReadFlag)
		{
		columnOffset = columnStart - 1;
		numberColumnsPerChannel = fileInfoPtr->numberColumns;
		
		}	// end "if (oneReadFlag)"
		
	else	// !oneReadFlag
		{
		columnOffset = 0;
		numberColumnsPerChannel = numberSamples;
		
		}	// end "else !oneReadFlag"
																						
	if (fileIOInstructionsPtr->maskBufferPtr != NULL)
		{
		if (!DetermineIfMaskDataInLine (fileIOInstructionsPtr->maskBufferPtr,
													fileIOInstructionsPtr->maskColumnStart,
													numberSamples,
													columnInterval,
													fileIOInstructionsPtr->maskValueRequest))
																							return (2);
																						
		}	// end "if (fileIOInstructionsPtr->maskBufferPtr != NULL)"
												
			// If the data buffer is NULL, then this implies that the data
			// output buffer can be the same as that used for the file IO
			// buffer.
			
	if (dataBufferPtr == NULL)
		dataBufferPtr = fileIOBufferPtr;
		
			// Initialize rest of local variables.													
		
	bytesChannelOffset = 0;
	localFileInfoPtr = NULL;
	
			// Determine if the line of data will need to be packed.	
			// One last item to be checked. The rest have been checked in 
			// 'SetUpFileIOInstructions'.
			
	if (packDataFlag && oneReadFlag && numberColumnsPerChannel > numberSamples)
		callPackLineOfDataFlag = TRUE;

			// Check for error case when data is to be packed but the input and	
			// output buffers are the same.													
			
	if ((fileIOBufferPtr == dataBufferPtr) && differentBuffersFlag)
																							return (-1);
		
			// Get the buffer to read the data into.										
			// Check for special case when data is to be packed but no effort is	
			// needed to do it other than read data into the data buffer.				
			
	ioBufferPtr = fileIOBufferPtr;
	if (packDataFlag && !callPackLineOfDataFlag)
		ioBufferPtr = dataBufferPtr;
		
			// Get the number of bytes in the data to be returned.					
			
	numberBytes = windowInfoPtr->localMaxNumberBytes;
	if (forceOutByteCode == kForce2Bytes)
		numberBytes = 2;
		
	else if (forceOutByteCode == kForce4Bytes || forceOutByteCode == kForceReal4Bytes)
		numberBytes = 4;
		
	else if (forceOutByteCode == kForceReal8Bytes)
		numberBytes = 8;
		
	fileNumberChannels = 0;
	fileInfoIndex = -1;
	channel = 0;
	channelListIndex = fileIOInstructionsPtr->channelListIndexStart;
	
			// If gdal will be used to read the data, check if all channels can be read
			// with one gdal call. If the last channel in the list is larger than the 
			// number of channels in the data set then this implies a linked file which
			// cannot be read with one call. Also if this is an hdf5 file with grouped
			// data sets, then all channels cannot be read with one call.
			// Also do not use multiple channel call if the lines are to be flipped from
			// top to bottom. Must force to go through GetLine.
			
	if (fileInfoPtr->gdalDataSetH != NULL)
		{
		if (fileIOInstructionsPtr->gdalChannelListPtr[
				fileIOInstructionsPtr->numberGdalChannels-1] <=
												fileInfoPtr->numberChannels && 
													fileInfoPtr->hdfHandle == NULL &&
														!fileInfoPtr->treatLinesAsBottomToTopFlag)
			useMultipleChannelGDALFlag = TRUE;		
		
		}	// end "if (fileInfoPtr->gdalDataSetH != NULL)"
	
			// Loop through the channels to be read in.									
			
	for (index=0; index<numberChannels; index++)
		{														
		if (channelListPtr != NULL)
			channel = channelListPtr[channelListIndex];
		
		channel++;
		if (fileInfoIndex != (SInt16)layerInfoPtr[channel].fileInfoIndex &&
				callPackLineOfDataFlag &&
					localFileInfoPtr != NULL)
			{
			if (localFileInfoPtr->bandInterleave != kBIS)
				{
				toBISFormatFlag = (fileIOInstructionsPtr->forceOutputFormatCode == kBIS);
				
						// Pack the data in the line leaving the unused columns out
						// and/or convert to BIS format if either has been
						// requested.  Also repack if 1 byte data is to be
						// converted to 2 byte data. Update the data buffer pointer.
						// Note that for this call of PackNonBISData, the "channelListPtr"
						// parameter (the 4) one will never be used
					
				PackNonBISData (columnOffset,
										columnInterval,
										numberColumnsPerChannel,
										(UInt32)fileNumberChannels,
										(UInt32)numberChannels,
										NULL,		// "channelListPtr"
										numberSamples,
										localFileInfoPtr->dataConversionCode,
										fileIOBufferPtr,
										dataBufferPtr,
										toBISFormatFlag);
										
				}	// end "if (localFileInfoPtr->bandInterleave != kBIS)"
												
					// Do channel and sample packing for BIS formats here.
			
			else	// localFileInfoPtr->bandInterleave == kBIS)
				{
				splitChannelListPtr = NULL;
				if (channelListPtr != NULL)
					splitChannelListPtr = (HUInt16Ptr)&channelListPtr[localFileInfoPtr->splitChannelsStart];

				toBISFormatFlag = (fileIOInstructionsPtr->forceOutputFormatCode == kBIS);
				if (fileIOInstructionsPtr->forceOutputFormatCode == 0)
					toBISFormatFlag = TRUE;
					
				PackBISData (layerInfoPtr,
									columnInterval,
									numberChannels,
									localFileInfoPtr->numberChannels,
									fileNumberChannels,
									splitChannelListPtr,
									numberSamples,
									localFileInfoPtr->dataConversionCode,
									fileIOBufferPtr,
									dataBufferPtr,
									toBISFormatFlag);
										
				}	// end "else localFileInfoPtr->bandInterleave==kBIS"
										
			if (toBISFormatFlag)
				dataBufferPtr =
						&dataBufferPtr[(SInt32)numberBytes * fileNumberChannels];
			
			else	// !toBISFormatFlag
				dataBufferPtr = &dataBufferPtr[(SInt32)numberBytes *
					fileNumberChannels * ((numberSamples + columnInterval - 1)/
																					columnInterval)];
						
			ioBufferPtr = (UCharPtr)fileIOBufferPtr;
									
			fileNumberChannels = 0;
			
			}	// end "if (fileInfoIndex != ..." 
			
		fileInfoIndex = layerInfoPtr[channel].fileInfoIndex;
		localFileInfoPtr = &fileInfoPtr[fileInfoIndex];
		
		#if include_gdal_capability
			if (localFileInfoPtr->gdalDataSetH != NULL && useMultipleChannelGDALFlag)
				{
				errCode = GetGDALLineOfData (localFileInfoPtr,	
														ioBufferPtr,
														lineNumber,
														columnStart,
														columnEnd,
														fileIOInstructionsPtr->numberGdalChannels,
														fileIOInstructionsPtr->gdalChannelListPtr,
														&numberSamplesRead,
														oneReadFlag);
				
				}	// end "if (localFileInfoPtr->gdalDataSetH != NULL && ..."
				
			else	// localFileInfoPtr->gdalDataSetH == NULL || ...
				{
		#endif	// include_gdal_capability
				errCode = GetLine (fileIOInstructionsPtr,
											localFileInfoPtr,
											lineNumber,
											layerInfoPtr[channel].fileChannelNumber-1,
											columnStart,
											columnEnd,
											&numberSamplesRead,
											ioBufferPtr);
			
		#if include_gdal_capability							
				}	// end "else fileInfoPtr->gdalDataSetH == NULL || ..."
		#endif	// include_gdal_capability
			
				// Exit routine if errCode is not noErr.								
				
		if (errCode != noErr)
																						return (errCode);
																			
		if (numberSamplesRead <= 0)
																						return (-1);
								
		bytesChannelOffset = numberSamplesRead*localFileInfoPtr->numberBytes;
		
				// Update buffer pointer to read in the next channel					
					
      ioBufferPtr = &ioBufferPtr[bytesChannelOffset];
      
      fileNumberChannels++;
      
      		// If the image file band interleave is by sample, indicate that	
      		// all channels have been read.  All channels for a line are read	
      		// by GetLine if the band interleave is BIS.	 Also if the read		
      		// was a special case of BIL format done with dgal routines then all the 
				// data for all channels was read at one time.
      		
      if (localFileInfoPtr->bandInterleave == kBIS ||
					oneReadFlag ||
					useMultipleChannelGDALFlag)
			{
					// For this case one may or may not have linked files
			
			if (windowInfoPtr->numberImageFiles == 1 || localFileInfoPtr->bandInterleave != kBIS)
				fileNumberChannels = index = numberChannels;
				
			else	// windowInfoPtr->numberImageFiles > 1 && ...->bandInterleave == kBIS
				{
				fileNumberChannels += localFileInfoPtr->numberSplitChannels - 1;
					
				index += localFileInfoPtr->numberSplitChannels - 1;		// 0 based
				channel = index + 1; 												// 1 based
				
				}	// end "else windowInfoPtr->numberImageFiles > 1"
      	
      	}	// end "if (localFileInfoPtr->bandInterleave == kBIS || ..."
      	
		channelListIndex++;
      			
      }	// end "for (index=0; index<numberChannels..."
	
			// Repack the line of data	if needed.											
	
	if (callPackLineOfDataFlag)
		{
		splitChannelListPtr = NULL;
		if (channelListPtr != NULL)
			{
			if (numberChannels == 1)
						// This implies reading one channel at at time such as coming from Change Image File
						// Format.
				splitChannelListPtr = channelListPtr;
			
			else	// numberChannels > 1
				splitChannelListPtr = (HUInt16Ptr)&channelListPtr[localFileInfoPtr->splitChannelsStart];
				
			}	// end "if (channelListPtr != NULL)"
			
		if (localFileInfoPtr->bandInterleave != kBIS)
			{
			if (!oneReadFlag || numberChannels ==
											fileIOInstructionsPtr->bilSpecialNumberChannels)
				splitChannelListPtr = NULL;
			
					// Pack the data in the line leaving the unused columns out
					// and/or convert to BIS format if either has been
					// requested.  Also repack if 1 byte data is to be
					// converted to 2 byte data or 1 or 2 byte data is to be converted
					// to 4 byte data. Update the data buffer pointer.
				
			PackNonBISData (columnOffset,
									columnInterval,
									numberColumnsPerChannel,
									(UInt32)fileNumberChannels,
									(UInt32)numberChannels,
									splitChannelListPtr,
									numberSamples,
									localFileInfoPtr->dataConversionCode,
									fileIOBufferPtr,
									dataBufferPtr,
									toBISFormatFlag);
				
			}	// end "if (localFileInfoPtr->bandInterleave != kBIS)"
			
				// Do channel and sample packing for BIS formats here.					
		
		else	// localFileInfoPtr->bandInterleave == kBIS
			{
			toBISFormatFlag = (fileIOInstructionsPtr->forceOutputFormatCode == kBIS);
			if (fileIOInstructionsPtr->forceOutputFormatCode == 0)
				toBISFormatFlag = TRUE;
				
			PackBISData (layerInfoPtr,
								columnInterval,
								numberChannels,
								localFileInfoPtr->numberChannels,
								fileNumberChannels,
								splitChannelListPtr,
								numberSamples,
								localFileInfoPtr->dataConversionCode, 
								fileIOBufferPtr,
								dataBufferPtr,
								toBISFormatFlag);
								
			}	// end "else localFileInfoPtr->bandInterleave == kBIS"
									
		}	// end "if (callPackLineOfDataFlag)"
				
			// If mask buffer is being used then get just those data values requested
			// by the mask. Note that this is currently only set up for BIS output
			// data.
							
	if (fileIOInstructionsPtr->maskBufferPtr != NULL)
		fileIOInstructionsPtr->numberOutputBufferSamples = PackMaskData (
														fileIOInstructionsPtr->maskBufferPtr,
														fileIOInstructionsPtr->maskColumnStart,
														numberChannels,
														numberSamples,
														columnInterval,
														dataBufferPtr,
														numberBytes,
														fileIOInstructionsPtr->maskValueRequest);
    
    return (noErr);
	      
}	// end "GetLineOfData" 



#if include_gdal_capability
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt32 GetGDALLineOfData
//
//	Software purpose:	The purpose of this is to get a line(s) of data using the GDAL
//							interface routines. This will also handle data that are tiled. 
//							If the data are tiled (i.e. groups of lines are read), the 
//							requested line and channels of data will be moved to the output
//							io buffer. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None	
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 12/16/2009
//	Revised By:			Larry L. Biehl			Date: 11/30/2018

SInt16 GetGDALLineOfData (
				FileInfoPtr							fileInfoPtr,	
				HUCharPtr							ioBufferPtr,
				UInt32								lineNumber,
				UInt32								columnStart,
				UInt32								columnEnd,
				UInt32								numberChannels,
				int*									gdalChannelListPtr,
				UInt32*								numberSamplesReadPtr,
				Boolean								oneReadFlag)

{
	HdfDataSets*						hdfDataSetsPtr = NULL;
	HierarchalFileFormatPtr			hfaPtr = NULL;
	
	HUCharPtr							ioBlockBufferPtr,
											readBufferPtr;
											
	int*									rasterIOChannelListPtr;
	
	GDALDatasetH						hDS;
	
	int									groupedChannelNumber;
	
	UInt32								bytesChannelOffset,
											channelNumber,
											dataSetsIndex,
											gdalColumnStart,
											gdalColumnEnd,
											gdalLineStart,
											gdalNumberLines,
											channelIndex,
											numberSamplesRead;
											
	SInt16								errCode;
											
	Boolean								readLineFlag;
	
	
			// Initialize local variables.
				
	readBufferPtr = ioBufferPtr;
	readLineFlag = TRUE;
	errCode = noErr;
	*numberSamplesReadPtr = 0;
	
	gdalLineStart = lineNumber - 1;
	gdalNumberLines = 1;
	
	hDS = fileInfoPtr->gdalDataSetH;
	
	rasterIOChannelListPtr = gdalChannelListPtr;
	 
	if ((fileInfoPtr->format == kHDF5Type ||
				fileInfoPtr->format == kNETCDF2Type ||
						fileInfoPtr->format == kHDF4Type2 ||
								fileInfoPtr->format == kNITFType) &&
										fileInfoPtr->hdfHandle)
		{
			 // Set up for case where subdatasets are being read from hdf5 formatted
			 // files.
			
		hdfDataSetsPtr = (HdfDataSets*)GetHandlePointer (fileInfoPtr->hdfHandle);
															
		if (hdfDataSetsPtr == NULL || fileInfoPtr->channelToHdfDataSetPtr == NULL)
			{
			*numberSamplesReadPtr = 0;
																								return (1);
		
			}	// end "if (hdfDataSetsPtr == NULL || ..."
		
		channelNumber = gdalChannelListPtr[0];
		
				// Note that channelToHdfDataSetPtr is 0 based; channels for gdal are
				// 1 based.
				
		dataSetsIndex = fileInfoPtr->channelToHdfDataSetPtr[channelNumber-1];
		
		hDS = (GDALDatasetH)hdfDataSetsPtr[dataSetsIndex].sdid;
		
				// If this is part of a grouped set of data sets then each data set
				// has only 1 channel.
					
		if (hdfDataSetsPtr[dataSetsIndex].groupedNumber > 0)
			{
			numberChannels = 1;
			groupedChannelNumber = 1;
			rasterIOChannelListPtr = &groupedChannelNumber;
			
			}	// end "if (hdfDataSetsPtr[dataSetsIndex].groupedNumber > 0)"
			
		}	// end "if ((fileInfoPtr->format == kHDF5Type || ...) && ...)"
																					
	if (fileInfoPtr->blockedFlag)
		{
		if (fileInfoPtr->hfaPtr == NULL)
																								return (-1);
																							
		channelIndex = 0;
		if (gdalChannelListPtr != NULL)
			channelIndex = gdalChannelListPtr[0] - 1;
			
		hfaPtr = &fileInfoPtr->hfaPtr[channelIndex];
		
				// Check if line needs to be read from disk.
				
		if (lineNumber >= hfaPtr->firstLineRead && lineNumber <= hfaPtr->lastLineRead)
			readLineFlag = FALSE;
			
		if (readLineFlag)
			{
					// Set up correct buffer to read the data into. It will be
					// a buffer to hold several lines.
					
			readBufferPtr = (HUCharPtr)hfaPtr->tiledBufferPtr;	
			
			hfaPtr->firstColumnRead = columnStart;			
			hfaPtr->lastColumnRead = columnEnd;
			
			hfaPtr->firstLineRead = 
						((lineNumber-1)/hfaPtr->blockHeight) * hfaPtr->blockHeight + 1;			
			hfaPtr->lastLineRead = hfaPtr->firstLineRead + hfaPtr->blockHeight - 1;			
			hfaPtr->lastLineRead = MIN (hfaPtr->lastLineRead, fileInfoPtr->numberLines);
			
					// We will let gdal routines load the data starting from the
					// requested lineNumber.
					
			hfaPtr->firstLineRead = lineNumber;
			
			gdalNumberLines = hfaPtr->lastLineRead - hfaPtr->firstLineRead + 1;
			
			}	// end "if (readLineFlag)"
		
		}	// end "if (fileInfoPtr->blockedFlag)"
			
	if (readBufferPtr != NULL)
		{	
		if (readLineFlag)
			{
			gdalColumnStart = columnStart - 1;
			gdalColumnEnd = columnEnd - 1;
			if (oneReadFlag)
				{
				gdalColumnStart = 0;
				gdalColumnEnd = fileInfoPtr->numberColumns - 1;
				
				}	// end "if (oneReadFlag)"
					
			numberSamplesRead = gdalColumnEnd - gdalColumnStart + 1;
					
			errCode = GDALDatasetRasterIO (hDS,
														GF_Read,
														gdalColumnStart, 
														gdalLineStart, 
														numberSamplesRead, 
														gdalNumberLines,
														readBufferPtr, 
														numberSamplesRead, 
														gdalNumberLines,
														(GDALDataType)fileInfoPtr->gdalDataTypeCode,		
														numberChannels, 
														rasterIOChannelListPtr,
														0, 
														0, 
														0);	
			
			if (errCode == noErr && fileInfoPtr->blockedFlag)
				{
				hfaPtr->blockSizeRead =
							gdalNumberLines * numberSamplesRead * fileInfoPtr->numberBytes;
			
						// We need to update the block width parameter to represent the
						// actual width of the lock that was read into memory.
						
				hfaPtr->blockWidthRead = numberSamplesRead;
				
				}	// end "if (errCode == noErr && fileInfoPtr->blockedFlag)"
			
			if (errCode != noErr)
				HandleGDALErrorMessage (errCode);
				
			}	// end "if (readLineFlag)"
			
		if (errCode == noErr)
			{
					// Get number of samples read for one channel.
			
			numberSamplesRead = columnEnd - columnStart + 1;
			
			if (fileInfoPtr->blockedFlag)
				{
				bytesChannelOffset = numberSamplesRead * fileInfoPtr->numberBytes;				
				ioBlockBufferPtr = ioBufferPtr;
				
				for (channelIndex=0; channelIndex<numberChannels; channelIndex++)
					{
					PackBlockedData (lineNumber,
											columnStart, 
											columnEnd,
											channelIndex,
											fileInfoPtr,
											hfaPtr,
											ioBlockBufferPtr);
					
							// Update buffer pointer for start of the next channel					
								
					ioBlockBufferPtr = &ioBlockBufferPtr[bytesChannelOffset];
											
					}	// end "for (channelIndex=0; channelIndex<numberChannels; ..."
				
				}	// end "if (fileInfoPtr->blockedFlag)"
			
					// Get number of samples read for all channels.
			
			numberSamplesRead *= numberChannels;
			
			if (gConvertSignedDataFlag && fileInfoPtr->signedDataFlag)
				AdjustSignedData (fileInfoPtr, ioBufferPtr, numberSamplesRead);
			
			}	// end "if (errCode == noErr)"
			
		}	// end "if (readBufferPtr != NULL)"
		
	else	// readBufferPtr == NULL
		errCode = -1;
														
	if (errCode == noErr)
		*numberSamplesReadPtr = numberSamplesRead;
		
	return (errCode);
	      
}	// end "GetGDALLineOfData" 
#endif	// include_gdal_capability



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt32 GetNumberGAIALineSegments
//
//	Software purpose:	The purpose of this is to compute the number of
//							'segments' in a line of data.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None	
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 02/03/1994
//	Revised By:			Larry L. Biehl			Date: 05/03/1995

SInt32 GetNumberGAIALineSegments (
				SInt32								numberColumns)

{
	//return (numberColumns/1022);
	
	return ((numberColumns + 5)/1021);

}	// end "GetNumberGAIALineSegments" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt32 GetParID
//
//	Software purpose:	The purpose of this routine is to return the input file 
//							directory id.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 12/17/2001
//	Revised By:			Larry L. Biehl			Date: 12/17/2001	

SInt32 GetParID (
				FileInfoPtr							fileInfoPtr)

{  
	SInt32								dirID = 0;
	
	
	CMFileStream* fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
	
	
	if (fileStreamPtr != NULL)
		{              
		#if defined multispec_mac                                              
			dirID = fileStreamPtr->parID;	
		#endif	// defined multispec_mac
		
		}	// end "if (fileInfoPtr != NULL)" 

	return (dirID);
	
}	// end "GetParID"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetParID
//
//	Software purpose:	The purpose of this routine is to return the input file 
//							volume reference number.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 10/31/2002
//	Revised By:			Larry L. Biehl			Date: 10/31/2002	

SInt32 GetParID (
				Handle								fileInfoHandle)

{  
	SInt32								dirID = 0;
	
	
	FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
													
	dirID = GetParID (fileInfoPtr);

	return (dirID);
	
}	// end "GetParID"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetReferenceNumber
//
//	Software purpose:	The purpose of this routine is to return the input file 
//							reference number.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 10/25/1995
//	Revised By:			Larry L. Biehl			Date: 10/25/1995	

SInt16 GetReferenceNumber (
				CMFileStream*						fileStreamPtr)

{  
	SInt16								refNum = 0;
	
	
	if (fileStreamPtr != NULL)
		{              
		#if defined multispec_mac                                                
	   	refNum = fileStreamPtr->refNum;  	
		#endif	// defined multispec_mac
		
		}	// end "if (fileStreamPtr != NULL)" 

	return (refNum);
	
}	// end "GetReferenceNumber" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt32 GetShortIntValue
//
//	Software purpose:	The purpose of this routine is to obtain the shot int
//							value from the input.  The bytes will be swapped
//							if needed depending upon the global setting.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/17/1995
//	Revised By:			Larry L. Biehl			Date: 04/17/1995	

SInt16 GetShortIntValue (
				CharPtr								shortIntPtr)

{
	SInt16								tempInt;
	

	if (gSwapBytesFlag)
		{
		memcpy ((Ptr)&tempInt, (Ptr)shortIntPtr, 2);
		return (((tempInt & 0xff00) >> 8) | ((tempInt & 0x00ff) << 8));
		
		}	// end "if (gSwapBytesFlag)"
		
	else	// !gSwapBytesFlag
		return (*((short int*)shortIntPtr));

}	// end "GetShortIntValue"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetSizeOfFile
//
//	Software purpose:	The purpose of this routine is to return the size
//							of the specified file.
//
//	Parameters in:		
//
//	Parameters out:	None.
//
//	Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/11/1995
//	Revised By:			Larry L. Biehl			Date: 12/19/2003 
                       
SInt16 GetSizeOfFile (
				FileInfoPtr							fileInfoPtr,
				SInt64*								sizeBytesPtr)

{ 						
	CMFileStream* fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
		
	return (GetSizeOfFile (fileStreamPtr, sizeBytesPtr));
		      
}	// end "GetSizeOfFile"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetSizeOfFile
//
//	Software purpose:	The purpose of this routine is to return the size
//							of the specified file.
//
//	Parameters in:		
//
//	Parameters out:	None.
//
//	Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/11/1995
//	Revised By:			Larry L. Biehl			Date: 03/12/2008 
                       
SInt16 GetSizeOfFile (
				CMFileStream*						fileStreamPtr,
				SInt64*								sizeBytesPtr)

{  
	SInt64								forkSize;

	UInt32								fileSize;
	
	SInt16								errCode = noErr,
											errCode2 = noErr;
											//forkRefNum;
											
	Boolean								closeFileFlag = FALSE;
	
	
			// Initialize local variables.													
			
	fileSize = 0;
	forkSize = 0;

			// Determine if file needs to be opened.
			
	if (!FileOpen (fileStreamPtr))
		{		
	   errCode = OpenFileReadOnly (fileStreamPtr,
											 kDontResolveAliasChains,
											 kNoLockFile,
											 kDoNotVerifyFileStream);
			
	   closeFileFlag = TRUE;
	   
	   }	// end "if (!FileOpen (fileStreamPtr))" 
	
	if (errCode == noErr)
		{      
		#if defined multispec_mac
					// Note that the maximum file size that GetEOF will handle
					// is 2,147,483,648 bytes.
					// Although on 12/16/03, it appears the max returned is 2,113,506,817
					// for a file that is over 6 gigabytes.
					// FSGetForkSize will allow files larger than 2 gigabytes. May not be
					// present in some OS's though.
			
	   	if (gHasHFSPlusAPIs) 
				errCode2 = FSGetForkSize (fileStreamPtr->refNum, &forkSize); 
			
			else	// !gHasHFSPlusAPIs
				{
				errCode2 = GetEOF (fileStreamPtr->refNum, (SInt32*)&fileSize);
				forkSize = fileSize;
				
				}	// end "else !gHasHFSPlusAPIs"  
			
			if (errCode2 == noErr)
				fileStreamPtr->fileSize = forkSize;
		#endif	// defined multispec_mac 
	
      #if defined multispec_win || defined multispec_wx
			errCode2 = fileStreamPtr->MGetSizeOfFile (&forkSize); 
			
			if (errCode == noErr)
				fileStreamPtr->mFileSize = (SInt32)forkSize;
		#endif	// defined multispec_win || defined multispec_wx
			
		IOCheck (errCode2, fileStreamPtr);
		
		if (closeFileFlag)
			CloseFile (fileStreamPtr);
		
		}	// end "if (errCode == noErr)" 
	
	if (errCode != fnfErr)
		IOCheck (errCode, fileStreamPtr);
		
	if (errCode == noErr)
		errCode = errCode2;
		
	if (sizeBytesPtr != NULL)
		*sizeBytesPtr = forkSize;
							 
	return (errCode);
		      
}	// end "GetSizeOfFile"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt64 GetSizeOfImage
//
//	Software purpose:	The purpose of this routine is to compute the size
//							(in bytes) of the specified image with no channel
//							descriptions or class to group descriptions at the end.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:			ReadThematicGroups in SDisplayThematic.cpp
//							ReadMultiSpecChannelDescriptionsAndValues in SOpenImage.cpp
//							ModifyChannelDescriptions in SReformat.cpp
//							WriteChannelDescriptionsAndValues in
//																SReformatChangeImageFileFormat.cpp
//							WriteThematicGroups in SSaveWrite.cpp
//
//	Coded By:			Larry L. Biehl			Date: 09/15/1992
//	Revised By:			Larry L. Biehl			Date: 05/25/1993

SInt64 GetSizeOfImage (
				FileInfoPtr							fileInfoPtr)

{
	SInt64								sizeOfImage;
	
	
	sizeOfImage = 0;
	
	if (fileInfoPtr != NULL)
		{
				// Get the size of the image without any channel descriptions.		
		
		sizeOfImage = 
				(SInt64)fileInfoPtr->bytesPer1lineAllChans * fileInfoPtr->numberLines;
					
		if (fileInfoPtr->numberBits == 4)
			sizeOfImage = (sizeOfImage +1)/2;
					
		sizeOfImage += 
					fileInfoPtr->numberHeaderBytes + fileInfoPtr->numberTrailerBytes;
					
		}	// end "if (fileInfoPtr != NULL)" 
					
	return (sizeOfImage);

}	// end "GetSizeOfImage"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetThematicSupportFileToCreate
//
//	Software purpose:	This routine gets a clr or trl file ready for writing information
//							to.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	File IO error code
//
// Called By:			CreateThematicSupportFile in SFileIO.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/11/1996
//	Revised By:			Larry L. Biehl			Date: 04/12/2020

SInt16 GetThematicSupportFileToCreate (
				FileInfoPtr							gisFileInfoPtr,
				CMFileStream*						supportFileStreamPtr,
				UInt32								supportFileType,
				Boolean								promptFlag)

{
	FileStringPtr						gisFilePathPtr,
											supportFilePathPtr;
										
	CMFileStream*						gisFileStreamPtr;
	
	UInt32								numberBytes;
	
	SInt16								errCode,
											promptStringNumber;
	
	
	gisFileStreamPtr = GetFileStreamPointer (gisFileInfoPtr);
					
			// Get the pointer to the file stream block and initialize it.	

	gisFilePathPtr = (FileStringPtr)GetFilePathPPointerFromFileInfo (gisFileInfoPtr);
	supportFilePathPtr =
				(FileStringPtr)GetFilePathPPointerFromFileStream (supportFileStreamPtr);
			
			// Get the output file name and volume to write file to.	Make the 
			// default name the same as the .gis file name with .trl at the	
			// end.  First make sure that we have a pointer cursor. Also 		
			// Check if there is enough space on the selected volume.			
		
	//CopyPToP (supportFilePathPtr, gisFilePathPtr);
	CopyFileStringToFileString (gisFilePathPtr, supportFilePathPtr, _MAX_PATH);
	RemoveSuffix (supportFilePathPtr);
		
	if (supportFileType == kITRLFileType)
		ConcatFilenameSuffix (supportFilePathPtr, (StringPtr)"\0.trl\0");
		
	else	// supportFileType == kICLRFileType
		ConcatFilenameSuffix (supportFilePathPtr, (StringPtr)"\0.clr\0");

	SetType (supportFileStreamPtr, supportFileType);
	SInt16 vRefNum = GetVolumeReferenceNumber (gisFileStreamPtr);
		
			// Get the clr or trl file. If it is a new file then create it.
	
	if (promptFlag)
		{
		promptStringNumber = IDS_FileIO75;
			
		if (supportFileType == kITRLFileType)
			{
			numberBytes = 7*128;
			numberBytes += 9*128;
			numberBytes += ((gisFileInfoPtr->numberClasses+4)/4) * 128;
			
			}	// end "if (supportFileType == kITRLFileType)"
		
		else	// supportFileType == kICLRFileType
			{
					// Will allow 256 characters per line. This should be twice what is
					// needed.
			
			numberBytes = (gisFileInfoPtr->numberClasses+1) * 256;
			
			if (gisFileInfoPtr->numberGroups > 0)
				promptStringNumber = IDS_FileIO192;
			
			}	// end "else supportFileType == kICLRFileType"
		
		numberBytes += 1000;		// Spare

				// For Mac version: Use the folder that that the gis file is 
				// in to be the default folder.
				// For Windows version, the path name gives the default parent
				// folder for the tif image to be saved to.
				
		#if defined multispec_mac 
			supportFileStreamPtr->parentFSRef = gisFileStreamPtr->parentFSRef;
		#endif	// defined multispec_mac

				// Now get wide character and unicode names.

		SetFileDoesNotExist (supportFileStreamPtr, kKeepUTF8CharName);
		UpdateFileNameInformation (supportFileStreamPtr, NULL);
		
		errCode = PutFile (
						supportFileStreamPtr, numberBytes, promptStringNumber, gCreator);
				
		}	// end "if (promptFlag)"
				
	else	// !promptFlag 
		{
		//SetReferenceNumber (supportFileStreamPtr, 0);
		IndicateFileClosed (supportFileStreamPtr);
		SetVolumeReference (gisFileStreamPtr, supportFileStreamPtr);

				// Now get wide character and unicode names.

		SetFileDoesNotExist (supportFileStreamPtr, kKeepUTF8CharName);
		UpdateFileNameInformation (supportFileStreamPtr, NULL);
    	
		errCode = CreateNewFile (supportFileStreamPtr, 
											vRefNum, 
											gCreator, 
											kErrorMessages,
											kReplaceFlag);
		
		}	// end "else !promptFlag"
								
				// Set position to start writing data at the start of the file.
		
	if ((errCode == noErr) && FileExists (supportFileStreamPtr))
		errCode = MSetMarker (supportFileStreamPtr, fsFromStart, 0, kErrorMessages);
		
	return (errCode);

}	// end "GetThematicSupportFileToCreate" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				SInt16 GetVolumeFreeSpace
//
//	Software purpose:			The purpose of this routine is to obtain the number
//									of bytes that are free for the specified volume.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/06/1989
//	Revised By:			Larry L. Biehl			Date: 04/12/2020

SInt16 GetVolumeFreeSpace (
				CMFileStream*						fileStreamPtr,
				SInt16								vRefNum, 
				SInt64*								freeBytesPtr)

{
	SInt16								errCode;
	 
	
			// Get pointer to the file name
	
	#if defined multispec_mac
		//SInt16								tmpVRefNum;
		FSVolumeInfo						volumeInformation;
		
		HParmBlkPtr							hParmBlkPtr;
		//XVolumeParamPtr					xVolumeParamPtr;
		
		//SInt16								drvNum;
		
				
				// Initialize local variables.													
				
		*freeBytesPtr = 0;
		errCode = 1;

		char* fileNamePtr =
						(char*)GetFileNamePPointerFromFileStream (fileStreamPtr);
		
		if (gHasHFSPlusAPIs)
			{
			/*
			xVolumeParamPtr = (XVolumeParamPtr)MNewPointer (sizeof (XVolumeParam));
			if (xVolumeParamPtr != NULL)
				{
				xVolumeParamPtr->ioCompletion = NULL;
				xVolumeParamPtr->ioNamePtr = NULL;
				xVolumeParamPtr->ioVRefNum = vRefNum;
				xVolumeParamPtr->ioVolIndex = 0;
				errCode = PBXGetVolInfoSync (xVolumeParamPtr);
				
				if (errCode == noErr)
					*freeBytesPtr = (SInt64)xVolumeParamPtr->ioVFreeBytes;
					
				DisposePtr ((Ptr)xVolumeParamPtr);
				
				}	// end "if (xVolumeParamPtr != NULL)"
			*/
			errCode = FSGetVolumeInfo ((FSVolumeRefNum)vRefNum, 
												0, 
												NULL, 
												kFSVolInfoSizes, 
												&volumeInformation, 
												NULL, 
												NULL);
			
			if (errCode == noErr)
				*freeBytesPtr = volumeInformation.freeBytes;
									
			IOCheck (errCode, (CharPtr)fileNamePtr);
				
			}	// end "if (gHasHFSPlusAPIs)"
		
		else	// !gHasHFSPlusAPIs
			{
					// Consider XParamBlockRec for greater than 2 gigabytes.
					//		and PBXGetVInfoSync
							
			hParmBlkPtr = (HParmBlkPtr)MNewPointer (sizeof (HParamBlockRec));
			if (hParmBlkPtr != NULL)
				{																
				hParmBlkPtr->volumeParam.ioCompletion = NULL;
				hParmBlkPtr->volumeParam.ioNamePtr = NULL;
				hParmBlkPtr->volumeParam.ioVRefNum = vRefNum;
				hParmBlkPtr->volumeParam.ioVolIndex = 0;
				errCode = PBHGetVInfoSync (hParmBlkPtr);
				
				if (errCode == noErr)
					{  
					//drvNum = hParmBlkPtr->volumeParam.ioVDrvInfo;
					
					*freeBytesPtr = (SInt64)hParmBlkPtr->volumeParam.ioVFrBlk * 
														hParmBlkPtr->volumeParam.ioVAlBlkSiz;
					
					}	// end "if (errCode == noErr)"
					
				DisposePtr ((Ptr)hParmBlkPtr);
				/*
				if (errCode == noErr)
					errCode = GetVInfo (drvNum,
												gTextString,
												&tmpVRefNum,
												(SInt32*)freeBytesPtr);
				*/
				IOCheck (errCode, (CharPtr)fileNamePtr);
				
				}	// end "if (hParmBlkPtr != NULL)"
					
			}	// end "else !gHasHFSPlusAPIs"	
	#endif	// defined multispec_mac
	
	#if defined multispec_win		
		*freeBytesPtr = 0;
		errCode = -1;    

				// fill disk free information
				
		struct _diskfree_t diskfree;                                      
		if (_getdiskfree (_getdrive (), &diskfree) == 0)
			{
			*freeBytesPtr = (SInt64)diskfree.avail_clusters *
													diskfree.sectors_per_cluster *
																		diskfree.bytes_per_sector;
			errCode = noErr;
			
			}	// end "if (..."
	#endif	// defined multispec_win
	
   #if defined multispec_wx
		FileStringPtr filePathPtr =
					(FileStringPtr)GetFilePathPPointerFromFileStream (fileStreamPtr);
	
				// Only want the path. Temporarily place an end of string character at
				// end of the path and replace when done checking the volume space.
	
		int pathLength = fileStreamPtr->mUTF8PathLength;
		UInt8 savedCharacter = filePathPtr[pathLength+1];
		filePathPtr[pathLength+1] = 0;
		/*
		int numberChars = snprintf ((char*)gTextString3,
											256,
											" SFileIO::GetVolumeFreeSpace (filePathPtr): %s%s",
											&filePathPtr[1],
											gEndOfLine);
		ListString ((char*)gTextString3, numberChars, gOutputTextH);
		*/
		wxLongLong wxFreeBytes;
   	const wxString pathString = wxString (&filePathPtr[2]);
		if (wxGetDiskSpace (pathString, NULL, &wxFreeBytes))
			{
			*freeBytesPtr = wxFreeBytes.GetValue ();
			errCode = noErr;
			
			}	// end "if (wxGetDiskSpace (pathString,..."
	
		else	// !wxGetDiskSpace (pathString, ...
			{
					// Assume 2 GB space is available for now.
			*freeBytesPtr = 0;
			errCode = -1;
			
			}	// end "else !wxGetDiskSpace (pathString, ..."
		/*
		int numberChars2 = snprintf ((char*)gTextString3,
												256,
												" SFileIO::GetVolumeFreeSpace (freeBytes): %lld%s",
												*freeBytesPtr,
												gEndOfLine);
		ListString ((char*)gTextString3, numberChars2, gOutputTextH);
		*/
		filePathPtr[pathLength+1] = savedCharacter;
   #endif
	
	return (errCode);
		
}	// end "GetVolumeFreeSpace"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetVolumeReferenceNumber
//
//	Software purpose:	The purpose of this routine is to return the input file 
//							volume reference number.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 10/25/1995
//	Revised By:			Larry L. Biehl			Date: 10/25/1995	

SInt16 GetVolumeReferenceNumber (
				CMFileStream*						fileStreamPtr)

{  
	SInt16								vRefNum = 0;
	
	
	if (fileStreamPtr != NULL)
		{              
		#if defined multispec_mac
	   	vRefNum = fileStreamPtr->vRefNum;
		#endif	// defined multispec_mac
		
		}	// end "if (fileInfoPtr != NULL)" 

	return (vRefNum);
	
}	// end "GetVolumeReferenceNumber" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetVolumeReferenceNumber
//
//	Software purpose:	The purpose of this routine is to return the input file 
//							volume reference number.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 09/13/1995
//	Revised By:			Larry L. Biehl			Date: 10/25/1995	

SInt16 GetVolumeReferenceNumber (
				FileInfoPtr							fileInfoPtr)

{  
	SInt16								vRefNum = 0;
	
	
	CMFileStream* fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
	
	vRefNum = GetVolumeReferenceNumber (fileStreamPtr);

	return (vRefNum);
	
}	// end "GetVolumeReferenceNumber" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetVolumeReferenceNumber
//
//	Software purpose:	The purpose of this routine is to return the input file 
//							volume reference number.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 10/24/1995
//	Revised By:			Larry L. Biehl			Date: 10/25/1995	

SInt16 GetVolumeReferenceNumber (
				Handle								fileInfoHandle)

{  
	SInt16								vRefNum = 0;
	
	
	FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
													
	vRefNum = GetVolumeReferenceNumber (fileInfoPtr);

	return (vRefNum);
	
}	// end "GetVolumeReferenceNumber"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void IndicateFileClosed
//
//	Software purpose:	The purpose of this routine is to indicate in the input
//							file stream structure that the file is closed and
//							should not be accessed any more.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 05/30/1997
//	Revised By:			Larry L. Biehl			Date: 06/02/1997
//
//      TODO: For Linux. Seems that we can avoid using this function by using
//						wxFile::IsOpen () in CFileStr.cpp. However, confirm later that
//						this not needed for Linux
//
void IndicateFileClosed (
				CMFileStream*						fileStreamPtr)

{  		
	if (fileStreamPtr != NULL)
		{	 
		#if defined multispec_mac                                                                       
			fileStreamPtr->refNum = 0;
		#endif	// defined multispec_mac
		
		#if defined multispec_win	 
			fileStreamPtr->m_hFile = CFile::hFileNull;
			//fileStreamPtr->Abort ();
		#endif	// defined multispec_win
		
		}	// end "if (fileStreamPtr != NULL && ..."

}	// end "IndicateFileClosed"    



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void InitializeFileIOInstructions
//
//	Software purpose:	The purpose of this routine is to set up a FileIO thread.
//
//	Parameters in:				
//
//	Parameters out:	
//
//	Value Returned:	 
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 05/31/1996
//	Revised By:			Larry L. Biehl			Date: 05/05/2022

void InitializeFileIOInstructions (
				FileIOInstructionsPtr			fileIOInstructionsPtr)

{
			// Load File IO Instructions structure.
			
	fileIOInstructionsPtr->asyncIOThread = 0;
	fileIOInstructionsPtr->wakeUpThread = 0;
	fileIOInstructionsPtr->theAppRef = 0;
	
	fileIOInstructionsPtr->count = 0;
	fileIOInstructionsPtr->posOff = 0;
	
	fileIOInstructionsPtr->bufferUsedForIO = 0;
	fileIOInstructionsPtr->bufferUsedForApp = 0;
	fileIOInstructionsPtr->errCode = noErr;

	fileIOInstructionsPtr->bufferReadyFlag = FALSE;
	fileIOInstructionsPtr->doneFlag = FALSE;
	fileIOInstructionsPtr->fileIOThreadStartedFlag = FALSE;
	fileIOInstructionsPtr->forceOutputFormatCode = kDoNotForceFormat;
	
}	// end "InitializeFileIOInstructions"


#if defined multispec_mac || defined multispec_wx
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Handle InitializeFileStream
//
//	Software purpose:	The purpose of this routine is to get memory if needed for
//							a File Stream Structure and initialize it.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 10/24/1995
//	Revised By:			Larry L. Biehl			Date: 04/10/2020
                   
Handle InitializeFileStream (
				Handle								fileStreamHandle)

{ 
	CMFileStream*						fileStreamPtr = NULL;
	SignedByte							handleStatus;
								
	
	if (fileStreamHandle == NULL)     
		fileStreamHandle = MNewHandle (sizeof (CMFileStream));
	
	if (fileStreamHandle != NULL)
		fileStreamPtr = (CMFileStream*)
								GetHandleStatusAndPointer (fileStreamHandle, &handleStatus);
	
				// Initialize the some variables and handles in the structure.
		
	if (fileStreamPtr != NULL)
		{		
		InitializeFileStream (fileStreamPtr);	
		
		MHSetState (fileStreamHandle, handleStatus);
	
		}	// end "fileStreamPtr != NULL" 
		
	return (fileStreamHandle);

}	// end "InitializeFileStream"
#endif	// defined multispec_mac || defined multispec_wx
      
                      
		           	
CMFileStream* InitializeFileStream (
				CMFileStream*						fileStreamPtr)

{     		         
	#if defined multispec_mac
		if (fileStreamPtr == NULL) 
			fileStreamPtr = (CMFileStream*)MNewPointer (sizeof (CMFileStream));
			
		if (fileStreamPtr != NULL)
			{      
					// Initialize the  structure.			
				
			fileStreamPtr->vRefNum = 0;
			fileStreamPtr->parID = 0;
			fileStreamPtr->fileName[0] = 0;
					// For test
			//memset (fileStreamPtr->fileName, 0, 256);
			fileStreamPtr->creator = -1;
			fileStreamPtr->type = 'TEXT';
			fileStreamPtr->refNum = 0;
			fileStreamPtr->fSSpecFlag = FALSE;
			fileStreamPtr->fileSize = 0;
			
			fileStreamPtr->uniFileName.length = 0;
			fileStreamPtr->uniPascalFileName[0] = 0;
			fileStreamPtr->pathLength = 0;
			fileStreamPtr->fSRefFlag = TRUE;		// This is now always true for carbon
															// based app.
			
			}	// end "if (fileStreamPtr != NULL)"
	#endif	// defined multispec_mac
	
	#if defined multispec_win | defined multispec_wx
		if (fileStreamPtr == NULL) 
			fileStreamPtr = new CMFileStream;
			
		else	// fileStreamPtr != NULL
			{
			fileStreamPtr->MCloseFile (); 
			fileStreamPtr->mWideFilePathName[0] = 0;
			fileStreamPtr->mCreator = -1;
			fileStreamPtr->mFileType = -1;
			fileStreamPtr->mFileSize = 0;
					
			}	// end "if (fileStreamPtr != NULL)"
	#endif	// defined multispec_win
		
	return (fileStreamPtr);        

}	// end "InitializeFileStream" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Handle InitializeFileStream
//
//	Software purpose:	The purpose of this routine is to intialize a new file stream
//							structure using the information from another input file
//							stream structure. The new file stream structure is left
//							with no file being opened even if the input file stream
//							represents a file that is opened.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 10/25/1995
//	Revised By:			Larry L. Biehl			Date: 07/03/2017

void InitializeFileStream (
				CMFileStream*						newFileStreamPtr,
				CMFileStream*						oldFileStreamPtr)

{
	if (newFileStreamPtr != NULL && oldFileStreamPtr != NULL)
		{
		#if defined multispec_mac
					// variables from the 'old' file stream to the 'new' file stream.			
				
			newFileStreamPtr->vRefNum = oldFileStreamPtr->vRefNum;
			newFileStreamPtr->parID = oldFileStreamPtr->parID;
			CopyPToP (newFileStreamPtr->fileName, oldFileStreamPtr->fileName);
			newFileStreamPtr->creator = oldFileStreamPtr->creator;
			newFileStreamPtr->type = oldFileStreamPtr->type;
			newFileStreamPtr->refNum = 0;	// oldFileStreamPtr->refNum;
			newFileStreamPtr->fSSpecFlag = oldFileStreamPtr->fSSpecFlag;
			
			newFileStreamPtr->fsRef = oldFileStreamPtr->fsRef;
			newFileStreamPtr->parentFSRef = oldFileStreamPtr->parentFSRef;
			newFileStreamPtr->uniFileName = oldFileStreamPtr->uniFileName;
			newFileStreamPtr->uniPascalFileName[0] = 0;
			newFileStreamPtr->pathLength = oldFileStreamPtr->pathLength;
			newFileStreamPtr->fSRefFlag = oldFileStreamPtr->fSRefFlag;
		#endif	// defined multispec_mac
		
      #if defined multispec_wx
         WideFileStringPtr	oldFilePathPtr = 
					(WideFileStringPtr)oldFileStreamPtr->GetFilePathPPtr (kReturnUnicode);
			newFileStreamPtr->SetFilePath (oldFilePathPtr, TRUE);
			newFileStreamPtr->mCreator = oldFileStreamPtr->mCreator;
			newFileStreamPtr->mFileType = oldFileStreamPtr->mFileType;
		#endif	// defined multispec_wx
		
      #if defined multispec_win
         TBYTE* oldFilePathPtr =
								(TBYTE*)oldFileStreamPtr->GetFilePathPPtr (kReturnUnicode);
			newFileStreamPtr->SetFilePath (oldFilePathPtr, TRUE);
			newFileStreamPtr->mCreator = oldFileStreamPtr->mCreator;
			newFileStreamPtr->mFileType = oldFileStreamPtr->mFileType;
		#endif	// defined multispec_win
		
		}	// end "if (newFileStreamPtr != NULL && ..."

}	// end "InitializeFileStream"                                      



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void IOCheck
//
//	Software purpose:	The purpose of this routine is to check for IO 
//							Errors and post a message if one has occured.
//		
//	Parameters in:		File operation error code.
//							Address of file name.
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 04/25/1988
//	Revised By:			Larry L. Biehl			Date: 12/10/2019

void IOCheck (
				SInt16								errCode,
				char*									fileNamePtr)
                
{		
	if (errCode != noErr)
		{                   
		#if defined multispec_mac
			SInt16							returnCode,
												stringIndex;
			
			                     
					// Get error alert string.														
					
			GetIndString (gTextString, kFileIOStrID, kFileIOStr25);
			
					// Append the file name to the string.										
			
			ConcatPStrings (gTextString, (StringPtr)fileNamePtr, 255);
			ConcatPStrings (gTextString, (StringPtr)"\0'.  \0", 255);
			
					// Convert the error code to a string.										
					
			NumToString ((SInt32)errCode, gTextString2);
			ConcatPStrings (gTextString, gTextString2, 255);
			
					// Get the error description string.										
			
			stringIndex = 0;
			if (errCode == bdNamErr)			
				stringIndex = kFileIOStr15;
			else if (errCode == dirFulErr)	
				stringIndex = kFileIOStr16;
			else if (errCode == dskFulErr)	
				stringIndex = kFileIOStr17;
			else if (errCode == dupFNErr)		
				stringIndex = kFileIOStr18;
			else if (errCode == eofErr)		
				stringIndex = kFileIOStr19;
			else if (errCode == fLckdErr)		
				stringIndex = kFileIOStr20;
			else if (errCode == fnOpnErr)		
				stringIndex = kFileIOStr21;
			else if (errCode == tmfoErr)		
				stringIndex = kFileIOStr22;
			else if (errCode == wPrErr)		
				stringIndex = kFileIOStr23;
			else if (errCode == opWrErr)		
				stringIndex = kFileIOStr24;
			else if (errCode == nsvErr)		
				stringIndex = kFileIOStr26;
			else if (errCode == wrPermErr)	
				stringIndex = kFileIOStr32;
			else if (errCode == fnfErr)		
				stringIndex = kFileIOStr33;
			else if (errCode == ioErr)		
				stringIndex = IDS_FileIO91;
			else if (errCode == notEnoughMemoryErr)		
				stringIndex = IDS_FileIO64;
			else if (errCode == afpAccessDenied)		
				stringIndex = kFileIOStr32;
			else if (errCode == afpTooManyFilesOpen)		
				stringIndex = IDS_FileIOStr202;
			
			if (stringIndex != 0)
				{
				ConcatPStrings (gTextString, (StringPtr)"\0: \0", 255);
				
				GetIndString (gTextString3, kFileIOStrID, stringIndex);
				ConcatPStrings (gTextString, gTextString3, 255);
				
				}	// end "if (stringIndex != 0)"
				
			else	// stringIndex == 0
				gTextString3[0] = 0;
			
			::InitCursor ();
			returnCode = DisplayAlert (kErrorAlertID, 3, 0, 0, 0, gTextString);
		#endif	// defined multispec_mac

		#if defined multispec_win || defined multispec_wx

			unsigned char					textString[_MAX_PATH];

			SInt16							returnCode,
												stringIndex,
												stringLength,
												subStringLength;

					// Get error alert string.

			MGetString (textString, kFileIOStrID, IDS_FileIO_Error);

					// Append the file name to the string.

			subStringLength = CopyFileStringToCString ((FileStringPtr)fileNamePtr,
				&textString[textString[0] + 1],
				(int)_MAX_PATH - textString[0] - 10);
			stringLength = textString[0] + subStringLength;

			subStringLength = strlen("'.  (");
			subStringLength = MIN (subStringLength, _MAX_PATH - stringLength - subStringLength);
			memcpy (&textString[stringLength + 1], (StringPtr)"'.  (", subStringLength);
			stringLength += subStringLength;

					// Convert the error code to a string.

			NumToString ((SInt32)errCode, (UCharPtr)gTextString2);
			subStringLength = MIN (gTextString2[0], _MAX_PATH - stringLength - gTextString2[0]);
			memcpy (&textString[stringLength + 1], &gTextString2[1], subStringLength);
			stringLength += gTextString2[0];

					// Get the error description string.

			stringIndex = 0;
			if (errCode == 2)
				stringIndex = IDS_FileIO_NoSuchFileOrDirectory;
			else if (errCode == 13)
				stringIndex = IDS_FileIO_PermissionDenied;
			else if (errCode == eofErr)
				stringIndex = IDS_EndOfFileError;

			if (stringIndex != 0)
				{
				subStringLength = strlen ("): ");
				subStringLength = MIN (subStringLength, _MAX_PATH - stringLength - subStringLength);
				memcpy (&textString[stringLength + 1], (StringPtr)"): ", subStringLength);
				stringLength += subStringLength;

				MGetString (gTextString3, kFileIOStrID, stringIndex);
				subStringLength = MIN (gTextString3[0], _MAX_PATH - stringLength - gTextString3[0]);
				memcpy (&textString[stringLength + 1], &gTextString3[1], gTextString3[0]);
				stringLength += gTextString3[0];

				}	// end "if (stringIndex != 0)"

			else	// stringIndex == 0
				gTextString3[0] = 0;

			textString[stringLength + 1] = 0;
		#endif	// defined multispec_win || defined multispec_wx

		#if defined multispec_win
			TBYTE 				wideTempString[32];

			USES_CONVERSION;

			if (errCode == eofErr)
				{
				errCode = CFileException::endOfFile;

				}	// end "if (errCode == eofErr)"
			
			switch (errCode)
				{        
				case CFileException::genericException:
					MessageBox (
							NULL, (LPCTSTR)_T("Unspecified Error Occurred"), NULL, MB_OK);
					
					#ifdef _DEBUG  
		   			afxDump << "Unspecified error occurred " << errCode << "\n";
		   		#endif
		   		break;           
		   		    
				case CFileException::fileNotFound:
					MessageBox (
							NULL, (LPCTSTR)_T("File Could Not be Found"), NULL, MB_OK);
					
					#ifdef _DEBUG  
		   			afxDump << "File could not be found " << errCode << "\n";
		   		#endif
		   		break;           
		   		
				case CFileException::badPath:
					MessageBox (NULL, (LPCTSTR)_T("Path is Invalid"), NULL, MB_OK);
					
					#ifdef _DEBUG   
		   			afxDump << "Path is invalid " << errCode << "\n";
		   		#endif
		   		break;     
		   		
				case CFileException::tooManyOpenFiles:
					MessageBox (NULL, (LPCTSTR)_T("Too Many Open Files"), NULL, MB_OK);
					
					#ifdef _DEBUG  
		   			afxDump << "Too Many Open Files " << errCode << "\n";
		   		#endif
		   		break;
		   		
				case CFileException::accessDenied:
					MessageBox (NULL, (LPCTSTR)_T("File Access Denied"), NULL, MB_OK);
					
					#ifdef _DEBUG 
		   			afxDump << "File access denied " << errCode << "\n";
		   		#endif
		   		break;
		   		
				case CFileException::invalidFile:
					MessageBox (NULL, (LPCTSTR)_T("Invalid File Handle"), NULL, MB_OK);
					
					#ifdef _DEBUG 
		   			afxDump << "Invalid File Handle " << errCode << "\n";
		   		#endif
		   		break;
		   		
				case CFileException::badSeek:
					MessageBox (
							NULL, (LPCTSTR)_T("Error Setting File Pointer"), NULL, MB_OK);
					
					#ifdef _DEBUG 
		   			afxDump << "Error setting file pointer " << errCode << "\n";
		   		#endif
		   		break;   
		   		
				case CFileException::hardIO:
					MessageBox (NULL, (LPCTSTR)_T("Hardware error"), NULL, MB_OK);
					
					#ifdef _DEBUG 
		   			afxDump << "Hardware error " << errCode << "\n";
		   		#endif
		   		break;   
		   		
				case CFileException::sharingViolation:
					MessageBox (NULL, (LPCTSTR)_T("Sharing Violation"), NULL, MB_OK);
					
					#ifdef _DEBUG 
		   			afxDump << "Sharing violation " << errCode << "\n";
		   		#endif
		   		break;     
		   		
				case CFileException::lockViolation:
					MessageBox (NULL,
									(LPCTSTR)_T("Attempt to lock region already locked"),
									NULL,
									MB_OK);
					
					#ifdef _DEBUG   
		   			afxDump << "Attempt to lock region already locked " << errCode << "\n";
		   		#endif
		   		break;           
		   		
				case CFileException::diskFull:
					MessageBox (NULL, (LPCTSTR)_T("Disk is Full"), NULL, MB_OK);
					
					#ifdef _DEBUG  
		   			afxDump << "Disk is full " << errCode << "\n";
		   		#endif
		   		break;    
		   		
				case CFileException::endOfFile:
					//MessageBox (NULL, (LPCTSTR)_T("End of File Reached"), NULL, MB_OK);
					MessageBox (NULL, (LPCTSTR)A2T((LPCSTR)&textString[1]), NULL, MB_OK);
					
					#ifdef _DEBUG  
		   			afxDump << "End of File Reached " << errCode << "\n";
		   		#endif
		   		break;     
		   		
				default: 
					swprintf (
							wideTempString, 32, _T("File Exception Error %hd. "), errCode);
					MessageBox (NULL, wideTempString, NULL, MB_OK);
					
					#ifdef _DEBUG  
		   			afxDump << "File Exception Error " << errCode << "\n";
		   		#endif
		   		break;
		   		       
		   	}	// end "switch (errCode)"                
		#endif	// defined multispec_win 

		#if defined multispec_wx			
		/*	
					// Get error alert string.
			
			MGetString (textString, kFileIOStrID, IDS_FileIO_Error);
			
					// Append the file name to the string.
			
			subStringLength = CopyFileStringToCString ((FileStringPtr)fileNamePtr,
																		&textString[textString[0]+1],
																		(int)_MAX_PATH-textString[0]-10);
			stringLength = textString[0] + subStringLength;
			
			subStringLength = strlen("'.  (");
			subStringLength = MIN (subStringLength, _MAX_PATH-stringLength-subStringLength);
			memcpy (&textString[stringLength+1], (StringPtr)"'.  (", subStringLength);
			stringLength += subStringLength;
			
					// Convert the error code to a string.
			
			NumToString ((SInt32)errCode, gTextString2);
			subStringLength = MIN (gTextString2[0], _MAX_PATH-stringLength-gTextString2[0]);
			memcpy (&textString[stringLength+1], &gTextString2[1], subStringLength);
			stringLength += gTextString2[0];
			
					// Get the error description string.
			
			stringIndex = 0;
			if (errCode == 2)
				stringIndex = IDS_FileIO_NoSuchFileOrDirectory;
			else if (errCode == 13)
				stringIndex = IDS_FileIO_PermissionDenied;
			else if (errCode == eofErr)
				stringIndex = IDS_EndOfFileError;
			
			if (stringIndex != 0)
				{
				subStringLength = strlen ("): ");
				subStringLength = MIN (subStringLength, _MAX_PATH-stringLength-subStringLength);
				memcpy (&textString[stringLength+1], (StringPtr)"): ", subStringLength);
				stringLength += subStringLength;
				
				MGetString (gTextString3, kFileIOStrID, stringIndex);
				subStringLength = MIN (gTextString3[0], _MAX_PATH-stringLength-gTextString3[0]);
				memcpy (&textString[stringLength+1], &gTextString3[1], gTextString3[0]);
				stringLength += gTextString3[0];
				
				}	// end "if (stringIndex != 0)"
			
			else	// stringIndex == 0
				gTextString3[0] = 0;
				
			textString[stringLength+1] = 0;
			*/
			MInitCursor ();
			returnCode = DisplayAlert (kErrorAlertID, 3, 0, 0, 0, textString);
			/*
			int numberChars = snprintf ((char*)gTextString3,
													256,
													" File error: %d for '%s'. %s", 
													errCode,
													fileNamePtr,
													gEndOfLine);
			ListString ((char*)gTextString3, numberChars, gOutputTextH);
			*/
		#endif
	   	
	  	}	// end "if (errCode != noErr)" 
	   
}	// end "IOCheck"     



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void IOCheck
//
//	Software purpose:	The purpose of this routine is to check for IO 
//							Errors and post a message if one has occured.
//		
//	Parameters in:		File operation error code.
//							Address of file name.
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 04/25/1988
//	Revised By:			Larry L. Biehl			Date: 02/14/2025

void IOCheck (
				SInt16								errCode,
				CMFileStream*						fileStreamPtr)
                
{
	if (errCode != noErr)
		{
		if (fileStreamPtr != NULL)
			{
			#if defined multispec_mac
				IOCheck (errCode, (CharPtr)&fileStreamPtr->fileName);
			#endif	// defined multispec_mac

			#if defined multispec_wx || defined multispec_win
				FileStringPtr filePathPtr = (FileStringPtr)fileStreamPtr->GetFilePathPPtr ();
				if (filePathPtr != NULL)
					IOCheck (errCode, (CharPtr)filePathPtr);
				else	// filePathPtr == NULL
					IOCheck (errCode, (CharPtr)NULL);
			#endif	// defined multispec_wx

			}	// end "if fileStreamPtr != NULL)"
					
		else	// fileStreamPtr == NULL
			IOCheck (errCode, (CharPtr)NULL);
		
		}	// end "if (errCode != noErr)"
		
}	// end "IOCheck"   



//------------------------------------------------------------------------------------
//
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean LoadErdasHeader
//
//	Software purpose:	This routine loads the ERDAS header 
//							record based on the input file information  parameters
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			WriteErdasHeader in SSaveWrite.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/21/1991
//	Revised By:			Larry L. Biehl			Date: 02/17/2012

Boolean	LoadErdasHeader (
				FileInfoPtr							fileInfoPtr, 
				UInt8*								headerRecordPtr, 
				SInt16								ERDASVersion)

{
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	char									*erdasHeader = (char*)"HEADER",
											*erdasHeader74 = (char*)"HEAD74";
											
	float									tempFloat;
	
	SInt32								tempLongInt;
	
	SInt16								iPack;
	
	UInt16								nBands,
											nClasses,
											tempInt;
	
			
			// Set up swap bytes flag depending upon the system architecture.
			
	gSwapBytesFlag = gBigEndianFlag;
		
			// Load in 'HEADER'																	
	
	if (ERDASVersion == kErdas73Type)	
		BlockMoveData (erdasHeader, (Ptr)headerRecordPtr, 6);
		
	else	// ERDASVersion == kErdas74Type 
		BlockMoveData (erdasHeader74, (Ptr)headerRecordPtr, 6);
			
			// Write pack type of the data -> 4, 8, or 16 bit data					
				
	iPack = 0;
	
			//	Currently do not write out 4 bit packed data files.
	//if (fileInfoPtr->numberBits == 4)
	//	iPack = 1;
		
	if (fileInfoPtr->numberBits > 8 || fileInfoPtr->numberBytes > 1) 
		iPack = 2;                               
		
	//if (fileInfoPtr->numberBytes == 4)
	//	iPack = 4;
		                 
	tempInt = GetShortIntValue ((char*)&iPack);
	BlockMoveData ((Ptr)&tempInt, (Ptr)&headerRecordPtr[6], 2);

			// Write number of channels in the image data								

	nBands = fileInfoPtr->numberChannels;                                         
	tempInt = GetShortIntValue ((char*)&nBands);
	BlockMoveData ((Ptr)&tempInt, (Ptr)&headerRecordPtr[8], 2);
	
			// Write code indicating whether the data is signed.
			
	iPack = 0;
	if (fileInfoPtr->signedDataFlag)
		iPack = 1;                
	tempInt = GetShortIntValue ((char*)&iPack);
	BlockMoveData ((Ptr)&tempInt, (Ptr)&headerRecordPtr[10], 2);

			// Write number of columns in the image data									

	tempLongInt = fileInfoPtr->numberColumns;
	if (ERDASVersion == kErdas73Type)
		tempLongInt = ConvertRealAT ((double)fileInfoPtr->numberColumns);
		
	else	// ERDASVersion == kErdas74Type 
		tempLongInt = GetLongIntValue ((char*)&tempLongInt); 
	
	BlockMoveData ((Ptr)&tempLongInt, (Ptr)&headerRecordPtr[16], 4);

			// Write number of lines in the image data									

	tempLongInt = fileInfoPtr->numberLines;
	if (ERDASVersion == kErdas73Type)
		tempLongInt = ConvertRealAT ((double)fileInfoPtr->numberLines);
		
	else	// ERDASVersion == kErdas74Type 
		tempLongInt = GetLongIntValue ((char*)&tempLongInt);
	
	BlockMoveData ((Ptr)&tempLongInt, (Ptr)&headerRecordPtr[20], 4);

			// Write start column of the image data										

	tempLongInt = fileInfoPtr->startColumn;
	if (ERDASVersion == kErdas73Type)
		tempLongInt = ConvertRealAT ((double)fileInfoPtr->startColumn);
		
	else	// ERDASVersion == kErdas74Type 
		tempLongInt = GetLongIntValue ((char*)&tempLongInt);
	
	BlockMoveData ((Ptr)&tempLongInt, (Ptr)&headerRecordPtr[24], 4);

			// Write start line of the image data											

	tempLongInt = fileInfoPtr->startLine;
	if (ERDASVersion == kErdas73Type)
		tempLongInt = ConvertRealAT ((double)fileInfoPtr->startLine);
		
	else	// ERDASVersion == kErdas74Type 
		tempLongInt = GetLongIntValue ((char*)&tempLongInt);
	
	BlockMoveData ((Ptr)&tempLongInt, (Ptr)&headerRecordPtr[28], 4);

			// Write number of classes in the image data.								

	nClasses = (UInt16)fileInfoPtr->numberClasses;                                                   
	tempInt = GetShortIntValue ((char*)&nClasses);
	BlockMoveData ((Ptr)&tempInt, (Ptr)&headerRecordPtr[90], 2);
	
	if (fileInfoPtr->mapProjectionHandle != NULL)
		{                                            
		mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																	fileInfoPtr->mapProjectionHandle,
																	kLock);
		
				// Allow for MultiSpec Geographic Code which is -1.
				
		iPack = ConvertMultiSpecProjectionCodeToERDASCode (
									mapProjectionInfoPtr->gridCoordinate.referenceSystemCode,
									mapProjectionInfoPtr->gridCoordinate.projectionCode);  
			
		if (iPack < 0)
			iPack = 0;                            
		tempInt = GetShortIntValue ((char*)&iPack);
		BlockMoveData ((Ptr)&tempInt, (Ptr)&headerRecordPtr[88], 2);
		
		iPack = mapProjectionInfoPtr->planarCoordinate.areaUnitsCode;                                                                                  
		tempInt = GetShortIntValue ((char*)&iPack);
		BlockMoveData ((Ptr)&tempInt, (Ptr)&headerRecordPtr[106], 2);
	
		tempLongInt = ConvertRealAT (mapProjectionInfoPtr->planarCoordinate.pixelArea);
		BlockMoveData ((char*)&tempLongInt, &headerRecordPtr[108], 4);
	
		tempLongInt = ConvertRealAT (
									mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11);
		BlockMoveData ((char*)&tempLongInt, &headerRecordPtr[112], 4);
	
		tempLongInt = ConvertRealAT (
									mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11);
		BlockMoveData ((char*)&tempLongInt, &headerRecordPtr[116], 4);
	
		tempFloat = (float)mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize;
		tempLongInt = ConvertRealAT (tempFloat);
		BlockMoveData ((char*)&tempLongInt, &headerRecordPtr[120], 4);
	
		tempFloat = (float)mapProjectionInfoPtr->planarCoordinate.verticalPixelSize;
		tempLongInt = ConvertRealAT (tempFloat);
		BlockMoveData ((char*)&tempLongInt, &headerRecordPtr[124], 4);
					
		CheckAndUnlockHandle (fileInfoPtr->mapProjectionHandle);
		
		}	// end "if (fileInfoPtr->mapProjectionHandle != NULL)" 
	
	return (TRUE);
	
}	// end "LoadErdasHeader" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadErdasTRLClassNameBufferFromDescriptions
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
// Called By:			CreateThematicSupportFile in SFileIO.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/14/2004
//	Revised By:			Larry L. Biehl			Date: 05/17/2018
                       
void LoadErdasTRLClassNameBufferFromDescriptions (
				FileInfoPtr							gisFileInfoPtr,
				UInt32								inputNumberClasses,
				UCharPtr								ioTempBufferPtr,
				SInt16								collapseClassCode)

{  
	UCharPtr								classNamePtr;
	UInt16*								classSymbolPtr;
	
	UInt32								classNumber,
											classSymbolIndex,
											count,
											outputNumberClasses;
	
	
	classNamePtr = (UCharPtr)GetHandlePointer (gisFileInfoPtr->classDescriptionH,
																kLock);
	
	if (classNamePtr != NULL)
		{
		if (collapseClassCode == kCollapseClass)
			{
					// Class names will be in the order that they are in the description
					// structure.
					
			outputNumberClasses = inputNumberClasses;
			for (classNumber=1; classNumber<=outputNumberClasses; classNumber++)
				{
				count = MIN (classNamePtr[0], 31);
				BlockMoveData (&classNamePtr[1], ioTempBufferPtr, count);
				ioTempBufferPtr[count] = 0;
			
				ioTempBufferPtr += 32;
				classNamePtr += 32;
			
				}	// end "for (classNumber=1; classNumber<=outputNumberClasses; ..." 
				
			}	// end "if (collapseClassCode == kCollapseClass)"
			
		else	// "collapseClassCode != kCollapseClass"
			{
					// Class names from the description structure will be used for those
					// classes included in the class symbol vector. The missing classes
					// will be supplied with the default name "class n" where n represents
					// the class number.
					
			outputNumberClasses = gisFileInfoPtr->maxClassNumberValue;
			
					// Get a pointer to storage for the class symbols.					
					
			classSymbolPtr = 
					(UInt16*)&classNamePtr [gisFileInfoPtr->numberClasses*sizeof (Str31)];
					
			classSymbolIndex = 0;
			for (classNumber=0; classNumber<=outputNumberClasses; classNumber++)
				{
						// Note that if the classSymbol vector contains ascii characters
						// as those used in a MultiSpec classification, then do not
						// try to match with a class number.
						
				if (gisFileInfoPtr->asciiSymbols || 
												classSymbolPtr[classSymbolIndex] == classNumber)
					{
					count = MIN (classNamePtr[0], 31);
					BlockMoveData (&classNamePtr[1], ioTempBufferPtr, count);
					
					classSymbolIndex++;
					classNamePtr += 32;
					
					}	// end "if (gisFileInfoPtr->asciiSymbols || ..."
				
				else	// classSymbolPtr[classSymbolIndex] != classNumber
					{
					count = snprintf ((char*)ioTempBufferPtr, 256, "Class %d", (unsigned int)classNumber);
					count = MIN (count, 31);
					
					}	// end "if (classSymbolPtr[classSymbolIndex] == classNumber)"
			
				ioTempBufferPtr[count] = 0;
				ioTempBufferPtr += 32;
			
				}	// end "for (classNumber=0; classNumber<=outputNumberClasses; ..." 
				
			}	// end "else collapseClassCode != kCollapseClass"
		
		CheckAndUnlockHandle (gisFileInfoPtr->classDescriptionH);
			
		}	// end "if (classNamePtr != NULL)"
	
	else	// classNamePtr == NULL
		{
				// Make sure the class name structure is initialized.
			
		outputNumberClasses = inputNumberClasses;
		for (classNumber=1; classNumber<=outputNumberClasses; classNumber++)
			{
			ioTempBufferPtr[0] = 0;
			ioTempBufferPtr += 32;
		
			}	// end "for (classNumber=1; classNumber<=outputNumberClasses; ..."
		
		}	// end "else classNamePtr == NULL"
		
}	// end "LoadErdasTRLClassNameBufferFromDescriptions"


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadErdasTRLClassColorBuffer
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
//	Coded By:			Larry L. Biehl			Date: 05/14/2004
//	Revised By:			Larry L. Biehl			Date: 04/15/2020
                       
void LoadErdasTRLClassColorBuffer (
				FileInfoPtr							gisFileInfoPtr,
				ColorSpec*							colorSpecPtr,
				SInt16*								classPtr,
				UInt32								numberListClasses,
				UInt16*								paletteIndexPtr,
				UInt32								numberColorSpecEntries,
				RGBCharColorPtr					classColorTablePtr,
				SInt16								collapseClassCode,
				SInt16								classNameCode)

{  
	UCharPtr								classNamePtr;
	UInt16*								classSymbolPtr;
	
	UInt32								numberClasses,
											classIndex,
											classNumber,
											index,
											paletteIndex,
											tableIndex;
											
	Boolean								backgroundFlag,
											incrementPaletteIndexFlag;
				
	
	classNamePtr = NULL;
	classSymbolPtr = NULL;
	numberClasses = gisFileInfoPtr->numberClasses;
	
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
			// The color for this class is white. Generally, this class is not
			// counted in 'gisFileInfoPtr->numberClasses'.
			// Note that if this is for the several cases listed, the background class
			// is included in the number of classes. Therefore the number of total classes
			// for which to copy information into the classColorTable is reduced by 1.

	if (classNameCode < kFromDescriptionCode ||
														classNameCode == kFromShapeToThematicCode)
		{
		classColorTablePtr->red = 255;
		classColorTablePtr->green = 255;
		classColorTablePtr->blue = 255;
		
		classColorTablePtr++;
		
		if (classNameCode == kClassifyFileCode ||
				classNameCode == kProbFormatCode ||
					classNameCode == kEchoFieldsCode ||
						classNameCode == kProbFormatCode ||
							classNameCode == kClusterMaskCode ||
								classNameCode == kFromShapeToThematicCode)
			numberClasses--;
		
		}	// end "if (classNameCode < kFromDescriptionCode || ..."
	
	paletteIndex = 0;
	classIndex = 0;	
	for (classNumber=1; classNumber<=numberClasses; classNumber++)
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
					// Note for files defined by ascii symbols such as MultiSpec
					// classification files, assume that all classes are being used
					// in order.
					
			if (gisFileInfoPtr->asciiSymbols ||
											classNumber == (UInt32)classSymbolPtr[classIndex]+1)
				classIndex++;
			
			else	// classNumber != classSymbolPtr[classIndex]+1 
				{
				backgroundFlag = TRUE;
				incrementPaletteIndexFlag = FALSE;
				
				}	// end "else classNumber != classSymbolPtr[classIndex]+1"
			
			}	// end "else if (classSymbolPtr != NULL)"
		
		if (backgroundFlag)
			classColorTablePtr->red =
									classColorTablePtr->green =
														classColorTablePtr->blue = 255;
		
		else	// !backgroundFlag 
			{
			if (paletteIndexPtr != NULL)
				index = paletteIndexPtr[paletteIndex];
				
			else	// newPaletteIndexPtr == NULL
				index = paletteIndex;
			
			if (index < numberColorSpecEntries)
				tableIndex = index;
				
			else	// index >= numberColorSpecEntries
						// This will cause the first component in the table to be skipped
						// in case it is white for background class.
				tableIndex = (index % numberColorSpecEntries) + 1;
			
			#ifndef multispec_wx
				classColorTablePtr->red =
												(UInt8)(colorSpecPtr[tableIndex].rgb.red >> 8);
				classColorTablePtr->green =
												(UInt8)(colorSpecPtr[tableIndex].rgb.green >> 8);
				classColorTablePtr->blue =
												(UInt8)(colorSpecPtr[tableIndex].rgb.blue >> 8);
			#endif
			
			#if defined multispec_wx
				classColorTablePtr->red = (UInt8)colorSpecPtr[tableIndex].rgb.red;
				classColorTablePtr->green = (UInt8)colorSpecPtr[tableIndex].rgb.green;
				classColorTablePtr->blue = (UInt8)colorSpecPtr[tableIndex].rgb.blue;
			#endif
								
			}	// end "else !backgroundFlag"
			
		classColorTablePtr++;
		
		if (incrementPaletteIndexFlag)
			paletteIndex++;
		
		}	// end "for (classNumber=1; classNumber<=..." 
		
	if (collapseClassCode != kCollapseClass)
		CheckAndUnlockHandle (gisFileInfoPtr->classDescriptionH);
		
}	// end "LoadErdasTRLClassColorBuffer"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 LoadGroupInformationBuffers
//
//	Software purpose:	This routine loads the group information buffers to prepare
//							for writing this information to an ArcView clr file.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			CreateThematicSupportFile in SFileIO.cpp
//							
//	Coded By:			Larry L. Biehl			Date: 05/31/2011
//	Revised By:			Larry L. Biehl			Date: 07/22/2015	

SInt16 LoadGroupInformationBuffers (
				FileInfoPtr							fileInfoPtr,
				UInt16								numberClasses,
				CTabHandle							groupCTableHandle,
				SInt16*								classToGroupPtr,
				UInt16*								groupToPalettePtr,
				RGBCharColorPtr					groupColorTablePtr,
				Boolean*								listAllGroupInfoPtr,
				UInt16*								classSymbolPtr,
				UInt16								numberClassSymbols,
				Boolean								asciiSymbolsFlag)
														
{
	ColorSpec*							colorSpecPtr = NULL;
	CTabPtr								cTablePtr;
	
	UInt32								classIndex,
											classNumber;
	
	SInt16								numberGroups = 0;
	
	UInt16					//			classSymbolIndex,
											groupNumber;
			
			
	if (asciiSymbolsFlag && classSymbolPtr == NULL)
																							return (FALSE);
																				
	if (fileInfoPtr->numberGroups > 0 && groupCTableHandle != NULL)
		{																
		cTablePtr = (CTabPtr)GetHandlePointer (
											(Handle)groupCTableHandle, 
											kLock);
		colorSpecPtr = (ColorSpec*)&cTablePtr->ctTable;
		
		}	// end "if (fileInfoPtr->numberGroups > 0 && displaySpecsH != NULL)"
		
	if (colorSpecPtr != NULL)
		{
		if (asciiSymbolsFlag)
			numberClasses = numberClassSymbols;
		
		//classSymbolIndex = 0;
		for (classIndex=0; classIndex<numberClasses; classIndex++)
			{
			listAllGroupInfoPtr[classIndex] = FALSE;
			
			classNumber = classIndex;
			if (asciiSymbolsFlag)
				classNumber = classSymbolPtr[classIndex];
			
					// Note that if the thematic file is a MultiSpec classification file (has
					// ascii symbols) then the class symbol vector contains those symbols and
					// does not reflect whether the classNumber is being used or not. Assume 
					// for this case that it is being used.
			/*
			if (classNumber == classSymbolPtr[classSymbolIndex] ||
											fileInfoPtr->format == kMultiSpecClassificationType)
			if (classSymbolPtr == NULL ||
						asciiSymbolsFlag ||
								classNumber == classSymbolPtr[classSymbolIndex])
				{
				groupNumber = classToGroupPtr[classSymbolIndex];
			*/
			groupNumber = classToGroupPtr[classIndex];
				//if (groupToPalettePtr[groupNumber] == classSymbolIndex)
			if (groupToPalettePtr[groupNumber] == classIndex)
				{
				listAllGroupInfoPtr[classIndex] = TRUE;
				
						// Order is green, red, blue to be consistant with that in the ERDAS
						// trailer file.
				
				#ifndef multispec_wx
					groupColorTablePtr[groupNumber].red =
														(colorSpecPtr[classIndex].rgb.red >> 8);
					groupColorTablePtr[groupNumber].green =
														(colorSpecPtr[classIndex].rgb.green >> 8);
					groupColorTablePtr[groupNumber].blue =
														(colorSpecPtr[classIndex].rgb.blue >> 8);
				#else
					groupColorTablePtr[groupNumber].red =
														colorSpecPtr[classIndex].rgb.red;
					groupColorTablePtr[groupNumber].green =
														colorSpecPtr[classIndex].rgb.green;
					groupColorTablePtr[groupNumber].blue =
														colorSpecPtr[classIndex].rgb.blue;
				#endif
					
				}	// end "if (groupToPalettePtr[groupNumber] == classIndex)"
				
				//if (classSymbolIndex < numberClassSymbols-1)
				//	classSymbolIndex++;
					
				//}	// end "if (classNumber == classSymbolPtr[classSymbolIndex] || ...)"
			
			}	// end "for (classIndex=0; classIndex<..."
		
		numberGroups = fileInfoPtr->numberGroups;
		
		CheckAndUnlockHandle ((Handle)groupCTableHandle);
		
		}	// end "if (colorSpecPtr != NULL)"
	
	return (numberGroups);

}	// end "LoadGroupInformationBuffers"  


/*
This routine is not used any more.
//------------------------------------------------------------------------------------
//
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean LoadNewErdasHeader
//
//	Software purpose:	This routine initializes a new Erdas header
//							record and then calls the routine to fill in
//							the parameters.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			FS_gen_make_stat_image_same_scale in SStatisticsImageAlgorithms.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/21/1991
//	Revised By:			Larry L. Biehl			Date: 11/21/1991

Boolean	LoadNewErdasHeader (
				FileInfoPtr							fileInfoPtr, 
				UInt8*								headerRecordPtr, 
				SInt16								ERDASVersion)

{
	SInt32*								localHeaderRecordPtr;
	SInt16								n;
	Boolean								continueFlag;
	
	
	localHeaderRecordPtr = (SInt32*)headerRecordPtr;
	
			// Initialize the header array to all zeros.									
					
	for (n=1; n<=32; n++)
		{
		*localHeaderRecordPtr = 0;
		localHeaderRecordPtr++;
		
		}	// end "for (n=1; n<=32; n++)" 
	
			// Fill the rest of the header and write it to the file.					
			
	continueFlag = LoadErdasHeader (fileInfoPtr, headerRecordPtr, ERDASVersion);
			
	return (continueFlag);
		
}	// end "LoadNewErdasHeader"	
*/


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 MDeleteFile
//
//	Software purpose:	The purpose of this routine is to delete the specified file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	Error code for file operations.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 08/15/2010
//	Revised By:			Larry L. Biehl			Date: 08/16/2010

SInt16 MDeleteFile (
				CMFileStream* 						fileStreamPtr,
				SInt16								messageCode)

{
	SInt16								errCode = noErr;
	
	if (fileStreamPtr != NULL)
		{
   	CloseFile (fileStreamPtr);
   	
		#if defined multispec_mac
			SInt32								parID;
		
			if (fileStreamPtr->fSSpecFlag)
				parID = fileStreamPtr->parID;
		
			else	// !fileStreamPtr->fSSpecFlag
				parID = 0;
		
					// Create the file on the selected volume
					
			if (fileStreamPtr->fSRefFlag)
				errCode = FSDeleteObject (&fileStreamPtr->fsRef);			
			
			else	// !fileStreamPtr->fSRefFlag		
				errCode = HDelete (fileStreamPtr->vRefNum, 
		                           parID,
											fileStreamPtr->fileName);
								
			if (messageCode == kErrorMessages)
				IOCheck (errCode, (CharPtr)&fileStreamPtr->fileName);	
		#endif	// defined multispec_mac 
		
      #if defined multispec_win | defined multispec_wx
			errCode = fileStreamPtr->MDeleteFile (messageCode);
		#endif	// defined multispec_win 
		
		}	// end "if (fileStreamPtr != NULL)"
	
	return (errCode);

}	// end "MDeleteFile"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void MGetMarker
//
//	Software purpose:	The purpose of this routine is to get the current location
//							of the file marker in the current file.
//
//	Parameters in:		
//
//	Parameters out:	None.
//
//	Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/07/1995
//	Revised By:			Larry L. Biehl			Date: 01/07/2004 
                       
SInt16 MGetMarker (
				CMFileStream*						fileStreamPtr,
				SInt64*								outOffsetPtr,
				SInt16								messageCode)

{  
	SInt16								errCode;
	
	                  
	#if defined multispec_mac   
		SInt32								outOffset32; 
	  
		if (gHasHFSPlusAPIs)     
			errCode = FSGetForkPosition (fileStreamPtr->refNum, outOffsetPtr);
			
		else	// !gSupportsHFSPlusVols 
			{                                         
			errCode = GetFPos (fileStreamPtr->refNum, &outOffset32);	
			
			if (errCode == noErr)
				*outOffsetPtr = outOffset32;
			
			}	// end "else !gSupportsHFSPlusVols"
		
		if (messageCode == kErrorMessages)		
			IOCheck (errCode, fileStreamPtr);
	#endif	// defined multispec_mac
              
   #if defined multispec_win || defined multispec_wx
		errCode = fileStreamPtr->MGetMarker (outOffsetPtr, messageCode);
	#endif	// defined multispec_win || defined multispec_wx
							 
	return (errCode);
		      
}	// end "MGetMarker"     



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void MReadData
//
//	Software purpose:	The purpose of this routine is to read the data from
//							the specified file to the user supplied buffer. The
//							file is assumed to already be opened and positioned
//							to the correct location.
//
//	Parameters in:		
//
//	Parameters out:	None.
//
//	Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/07/1995
//	Revised By:			Larry L. Biehl			Date: 02/08/2008 
                       
SInt16 MReadData (
				CMFileStream*						fileStreamCPtr,
				UInt32*								numberBytesPtr,
				void* 								inBufferPtr,
				SInt16								messageCode)

{  
	SInt16								errCode;
	
	                  
	#if defined multispec_mac    
		if (gHasHFSPlusAPIs)
			errCode = FSReadFork (fileStreamCPtr->refNum, 
											fsFromMark, 
											0, 
											*numberBytesPtr, 
											inBufferPtr, 
											numberBytesPtr);
		
		else	// !gHasHFSPlusAPIs
			errCode = FSRead (
							fileStreamCPtr->refNum, (SInt32*)numberBytesPtr, inBufferPtr);
	
		if (errCode != noErr && messageCode == kErrorMessages)		
			IOCheck (errCode, (CharPtr)&fileStreamCPtr->fileName);                       
	#endif	// defined multispec_mac 
              
   #if defined multispec_win | defined multispec_wx
		errCode = fileStreamCPtr->MReadData (inBufferPtr, numberBytesPtr, messageCode); 
	#endif	// defined multispec_win 
							 
	return (errCode);
		      
}	// end "MReadData"            



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 MSetMarker
//
//	Software purpose:	The purpose of this routine is to set the file marker
//							in the current file.
//
//	Parameters in:		Location to set the marker from the beginning.
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 09/08/1995 
//	Revised By:			Larry L. Biehl			Date: 07/07/2005  

SInt16 MSetMarker (
				CMFileStream*						fileStreamCPtr,
				SInt16								fromCode,
				SInt64								inOffset,
				SInt16								messageCode)
                    
{       
	SInt16								errCode = noErr;
	
	
	#if defined multispec_mac
	   if (gHasHFSPlusAPIs)     
			errCode = FSSetForkPosition (fileStreamCPtr->refNum, fromCode, inOffset);
			
		else	// !gHasHFSPlusAPIs                                             
			errCode = SetFPos (fileStreamCPtr->refNum, fromCode, (SInt32)inOffset);
		
		if (messageCode == kErrorMessages) 		
			IOCheck (errCode, (CharPtr)&fileStreamCPtr->fileName);
	#endif	// defined multispec_mac

   #if defined multispec_win || defined multispec_wx
		errCode = fileStreamCPtr->MSetMarker (inOffset, fromCode, messageCode);
	#endif	// defined multispec_win || defined multispec_wx
	
	return (errCode);
  
}	// end "MSetMarker" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void MSetSizeOfFile
//
//	Software purpose:	The purpose of this routine is to set the size
//							of the specified file.
//
//	Parameters in:		
//
//	Parameters out:	None.
//
//	Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/11/1995
//	Revised By:			Larry L. Biehl			Date: 07/08/2005 
                       
SInt16 MSetSizeOfFile (
				CMFileStream*						fileStreamPtr,
				SInt64								countBytes,
				SInt16								messageCode)

{  
	SInt16								errCode;
	
	                  
	#if defined multispec_mac
   	if (gHasHFSPlusAPIs) 
			errCode = FSSetForkSize (fileStreamPtr->refNum, fsFromStart, countBytes); 
		
		else	// !gHasHFSPlusAPIs
			errCode = ::SetEOF (fileStreamPtr->refNum, (SInt32)countBytes);
	#endif	// defined multispec_mac

	#if defined multispec_win || defined multispec_wx
		errCode = fileStreamPtr->MSetSizeOfFile (countBytes);
	#endif	// defined multispec_win || defined multispec_wx
		
	if (messageCode == kErrorMessages)		
		IOCheck (errCode, fileStreamPtr);                       
							 
	return (errCode);
		      
}	// end "MSetSizeOfFile"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 MWriteData
//
//	Software purpose:	The purpose of this routine is to write the date in the
//							input buffer to the specified disk file that has already
//							been opened.
//
//	Parameters in:		
//
//	Parameters out:	None.
//
//	Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/06/1995
//	Revised By:			Larry L. Biehl			Date: 07/08/2005 

SInt16 MWriteData (
				CMFileStream*						fileStreamPtr,
				UInt32*								numberBytesPtr,
				void*									inBufferPtr,
				SInt16								messageCode)

{  
	SInt16								errCode;
	
	                  
	#if defined multispec_mac
   	if (gHasHFSPlusAPIs) 
			errCode = FSWriteFork (fileStreamPtr->refNum,
											fsAtMark,
											0, 
											*numberBytesPtr, 
											inBufferPtr,
											numberBytesPtr);
   	
   	else	// !gHasHFSPlusAPIs
			errCode = FSWrite (
								fileStreamPtr->refNum, (SInt32*)numberBytesPtr, inBufferPtr);
		
		if (messageCode == kErrorMessages)		
			IOCheck (errCode, fileStreamPtr);
	#endif	// defined multispec_mac

	#if defined multispec_win || defined multispec_wx
		errCode = fileStreamPtr->MWriteData (inBufferPtr, numberBytesPtr, messageCode);
	#endif	// defined multispec_win
							 
	return (errCode);
		      
}	// end "MWriteData" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 NavServicesCheckForIgnoredFiles
//
//	Software purpose:	The purpose of this routine is to check for files to be ignored
//							by the filter function based on the suffix. These will be files
//							like .doc, docx, xls, etc. Save time trying to read the image 
//							format.
//
//	Parameters in:		
//
//	Parameters out:	None.
//
//	Value Returned:	TRUE file is one to be ignored. 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/18/2013
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

Boolean NavServicesCheckForIgnoredFiles (
				FileStringPtr						fileNamePtr)

{ 
	Boolean								returnFlag = FALSE;


	if (CompareSuffixNoCase ((char*)"\0.doc", fileNamePtr, NULL))
		returnFlag = TRUE;
		
	else if (CompareSuffixNoCase ((char*)"\0.docx", fileNamePtr, NULL))
		returnFlag = TRUE;
		
	else if (CompareSuffixNoCase ((char*)"\0.xls", fileNamePtr, NULL))
		returnFlag = TRUE;
		
	else if (CompareSuffixNoCase ((char*)"\0.xlsx", fileNamePtr, NULL))
		returnFlag = TRUE;
		
	else if (CompareSuffixNoCase ((char*)"\0.ppt", fileNamePtr, NULL))
		returnFlag = TRUE;
		
	else if (CompareSuffixNoCase ((char*)"\0.pptx", fileNamePtr, NULL))
		returnFlag = TRUE;
		
	else if (CompareSuffixNoCase ((char*)"\0.pps", fileNamePtr, NULL))
		returnFlag = TRUE;
		
	else if (CompareSuffixNoCase ((char*)"\0.zip", fileNamePtr, NULL))
		returnFlag = TRUE;
		
	else if (CompareSuffixNoCase ((char*)"\0.pdf", fileNamePtr, NULL))
		returnFlag = TRUE;
		
	else if (CompareSuffixNoCase ((char*)"\0.app", fileNamePtr, NULL))
		returnFlag = TRUE;
		
	else if (CompareSuffixNoCase ((char*)"\0.xml", fileNamePtr, NULL))
		returnFlag = TRUE;
		
	else if (CompareSuffixNoCase ((char*)"\0.wmv", fileNamePtr, NULL))
		returnFlag = TRUE;
		
	else if (CompareSuffixNoCase ((char*)"\0.rtf", fileNamePtr, NULL))
		returnFlag = TRUE;
		
	return (returnFlag);
		      
}	// end "NavServicesCheckForIgnoredFiles" 



#if defined multispec_mac 
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal Boolean NavServicesFilterProc
//
//	Software purpose:	The purpose of this routine is to filter the list of files
//							in the current folder being shown in the NavServices Get
//							file routine. Those files which are determined that MultiSpec
//							can not read directly are unhighlighted.
//		
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:	None
//
// Called By:			
//
//	Coded By:			From Apple Inc. examples
//	Revised By:			Larry L. Biehl			Date: 03/25/2017

pascal Boolean NavServicesFilterProc (
				AEDesc* 								theItem, 
				void* 								info, 
				NavCallBackUserData 				callBackUD, 
				NavFilterModes 					filterMode)
				
{
	//#pragma unused (callBackUD, filterMode)

	CMFileStream*						fileStreamPtr;
	FileInfoPtr							fileInfoPtr;
	FSSpec*								fsSpecPtr;
	NavFileOrFolderInfo* 			theInfo;
	NavUserData*						navUserDataPtr;
	FileStringPtr						filePathPtr;
	
	OSErr									errCode;
	SInt16								callCode,
											formatOnlyCode;
											
	Boolean								display = TRUE;
	
	
	navUserDataPtr = (NavUserData*)callBackUD;		
	theInfo = (NavFileOrFolderInfo*)info;
	
	if (theItem->descriptorType == typeFSS || theItem->descriptorType == typeFSRef)
		{
		if (!theInfo->isFolder && theInfo->visible)
			{									
			if (navUserDataPtr->filterSelection >= 101)
				{
				fileInfoPtr = navUserDataPtr->fileInfoPtr;
				
				if (fileInfoPtr != NULL)
					{
					fileInfoPtr->format = 0;
					fileInfoPtr->ancillaryInfoformat = 0;
					fileInfoPtr->thematicType = FALSE;
					
					fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
					fileStreamPtr->refNum = 0;
						
					if (theItem->descriptorType == typeFSS)
						{
						fsSpecPtr = (FSSpec*)*theItem->dataHandle;
						
						fileStreamPtr->vRefNum = fsSpecPtr->vRefNum;
						fileStreamPtr->parID = fsSpecPtr->parID;
						CopyPToP (fileStreamPtr->fileName, fsSpecPtr->name);
						fileStreamPtr->fSSpecFlag = TRUE;
						fileStreamPtr->fSRefFlag = FALSE;
							
						}	// end "if (theItem->descriptorType == typeFSS)"
						
					else	// theItem->descriptorType == typeFSRef
						{
						CFStringRef							cfStringRef;
						
						fileStreamPtr->fsRef = *((FSRef*)*theItem->dataHandle);
						
								// Get the file name.
														
						errCode = FSGetCatalogInfo (&fileStreamPtr->fsRef,
																kFSCatInfoVolume,
																NULL,
																&fileStreamPtr->uniFileName,
																NULL,
																NULL);
																
						if (errCode == noErr)
							{
							cfStringRef = CFStringCreateWithCharacters (
																kCFAllocatorDefault,
																fileStreamPtr->uniFileName.unicode,
																fileStreamPtr->uniFileName.length);
							
							CFStringGetCString (cfStringRef,
														(char*)&fileStreamPtr->fileName[1],
														(CFIndex)255,
														kCFStringEncodingUTF8);
							fileStreamPtr->fileName[0] =
													strlen ((char*)&fileStreamPtr->fileName[1]);
							/*
									// Get wide character file name. Not used now.
							fileStreamPtr->wideCharFileName[0] = 
																	fileStreamPtr->uniFileName.length;
							CFStringGetCString (cfStringRef,
														(char*)&fileStreamPtr->wideCharFileName[1],
														(CFIndex)255,
														kCFStringEncodingUTF32);
							fileStreamPtr->wideCharFileName[
														fileStreamPtr->wideCharFileName[0]+1] = 0;
							*/
									// This indicates that the full path is not part of the
									// file name.
							
							fileStreamPtr->pathLength = 0;
																
							//CFAllocatorDeallocate (kCFAllocatorDefault, (void*)cfStringRef);
							CFRelease (cfStringRef);
							
							}	// end "if (errCode == noErr)"
							
						fileStreamPtr->vRefNum = 0;
						fileStreamPtr->parID = 0;
						fileStreamPtr->fSSpecFlag = FALSE;
						fileStreamPtr->fSRefFlag = TRUE;
						
						}	// end "else theItem->descriptorType == typeFSRef"
						
					if (NavServicesCheckForIgnoredFiles (fileStreamPtr->fileName))
						errCode = 1;
				
					if (errCode == noErr)
						errCode = OpenFileReadOnly (fileStreamPtr, 
																kResolveAliasChains,
																kNoLockFile,
																kDoNotVerifyFileStream);
					
					formatOnlyCode = 0;
					callCode = 0;
					if (errCode == noErr)
						{		
						switch (navUserDataPtr->filterSelection)
							{
							case kAllReadableFiles:
							case kAllReadableImageFiles:
							case kMultiFiles:
							case kThematicFiles:
							case kShapeFiles:
								if (CheckImageHeader (
														NULL,
														navUserDataPtr->fileInfoHandle, 
														NULL, 
														navUserDataPtr->filterSelection,
														0) == 0)
									display = FALSE;
								break;
								
							case kSTAFiles:
								if (CheckIfSTAFile (fileStreamPtr) == 0)
									display = FALSE;
								break;
								
							case kProjectFiles:
								if (CheckIfProjectFile (fileStreamPtr, kProjectFiles) == -1)
									display = FALSE;
								break;
								
							case kTRLFiles:
								if (CheckIfThematicSupportFile (fileStreamPtr) == 0)
									display = FALSE;
								break;
								
							case kTransformFiles:
								//if (CheckIfTransformationFile (fileStreamPtr) == 0)
								if (theInfo->fileAndFolder.fileInfo.finderInfo.fdType !=
																									'TEXT' &&
										theInfo->fileAndFolder.fileInfo.finderInfo.fdType !=
																									'TRAN')
									display = FALSE;
								break;
								
							case kThematicGroupInfoFiles:
								filePathPtr = (FileStringPtr)
											GetFilePathPPointerFromFileStream (fileStreamPtr);
								if (!CompareSuffixNoCase ((char*)"\0.clr", filePathPtr, NULL))
									{
									if (CheckImageHeader (NULL,
														navUserDataPtr->fileInfoHandle, 
														NULL, 
														navUserDataPtr->filterSelection,
														0) == 0)
										display = FALSE;
									
									}	// end "if (!CompareSuffixNoCase ((char*)"\0.clr", ..."
									
								break;
							/*
							case kAllTextFiles:
								display = TRUE;
								break;
							*/
							}	// end "switch (navUserDataPtr->filterSelection)"
						
						CloseFile (fileStreamPtr);
							
						}	// end "if (errCode == noErr)"
						
					else 	// errCode != noErr
						display = FALSE;
					
					}	// end "if (fileInfoPtr != NULL)"
														
				}	// end "if (navUserDataPtr->filterSelection >= 101)"
		
			}	// end "if (!theInfo->isFolder && theInfo->visible)"
			
		}	// end "if (theItem->descriptorType == typeFSS || ...)"
		
	return display;
	
}	// end "NavServicesFilterProc"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void NavServicesGetEventProc
//
//	Software purpose:	The purpose of this routine is to respond to event prompts from
//							the NavServices GetFile routine.
//		
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:	None
//
// Called By:			
//
//	Coded By:			From Apple Inc. examples
//	Revised By:			Larry L. Biehl			Date: 03/25/2014

pascal void NavServicesGetEventProc (
				NavEventCallbackMessage 		callBackSelector, 
				NavCBRecPtr 						callBackParms, 
				NavCallBackUserData 				callBackUD)
				
{
	//FSRef								fsRefItem;
	Point									mousePt;
	Rect									theBox;

	AEDesc*								docListPtr;
	
	NavMenuItemSpec					*menuItemSpecPtr,
											*navMenuItemSpecPtr;
											
	NavUserData*						navUserDataPtr;
											
	DialogPtr							dialogPtr;
	//WindowPtr							window;
	
	//ControlHandle					theControl;
	//AEKeyword							keyword;
	//DescType							returnedType;

	SInt32								initialSelection,
											itemCount;
	
	OSErr 								errCode = noErr;
	
	SInt16								firstCustomControl,
											itemHit;
											
	//SignedByte						handleStatus;
	
	
	switch (callBackSelector)
		{
		case kNavCBEvent:
			{
			switch (callBackParms->eventData.eventDataParms.event->what)
				{
				case nullEvent:
					break;
					
				case updateEvt:
					if (callBackParms->window != (WindowPtr)callBackParms->
														eventData.eventDataParms.event->message)
  						DoUpdateEvent (callBackParms->eventData.eventDataParms.event);
  					/*
  					else	// if (callBackParms->window ==
  						{
  						errCode = NavCustomControl (callBackParms->context,
													kNavCtlGetFirstControlID,
													&firstCustomControl);
													
						countDITL = CountDITL (callBackParms->window);
												
						GetDialogItem (callBackParms->window, 
												firstCustomControl+1, 
												&theType, 
												(Handle*)&theControl, 
												&theBox);
						
						//if (theType < kStaticTextDialogItem)
						//	ShowControl (theControl);
						
						//ClipRect (GetWindowPortBounds (callBackParms->window, &gTempRect));
						//DrawControls (callBackParms->window);
  						
  						}	// end "else if (callBackParms->window =="
					*/
					break;
					
	      	case mouseDown:
					dialogPtr = GetDialogFromWindow (callBackParms->window);
					mousePt = callBackParms->eventData.eventDataParms.event->where;
					GlobalToLocal (&mousePt);
		  			
		  			itemHit = FindDialogItem (dialogPtr, mousePt);
		  			
					if (itemHit > 0)
		  				{
						errCode = NavCustomControl (callBackParms->context,
																kNavCtlGetFirstControlID,
																&firstCustomControl);
						
						if (errCode == noErr)
							{										
							switch (itemHit-firstCustomControl)
								{
								case 0:
									break;
									
								case 1:
									SetDLogControl (dialogPtr, firstCustomControl+2, 1);
									SetDLogControl (dialogPtr, firstCustomControl+3, 0);
									SetDLogControl (dialogPtr, firstCustomControl+4, 0);
									gGetFileImageType = 0;
									break;
			
								case 2:								
									SetDLogControl (dialogPtr, firstCustomControl+2, 0);
									SetDLogControl (dialogPtr, firstCustomControl+3, 1);
									SetDLogControl (dialogPtr, firstCustomControl+4, 0);
									gGetFileImageType = kMultispectralImageType;
									break;
									
								case 3:								
									SetDLogControl (dialogPtr, firstCustomControl+2, 0);
									SetDLogControl (dialogPtr, firstCustomControl+3, 0);
									SetDLogControl (dialogPtr, firstCustomControl+4, 1);
									gGetFileImageType = kThematicImageType;
									break;
								/*	
								case 4:
								
									if (gMultipleImageFileCode == 2)
										NavSetImageWindowFileFlagToFalse (dialogPtr, 
																					firstCustomControl+1);
										
									else	// gMultipleImageFileCode != 2 
										{
										SetDLogControl (dialogPtr, firstCustomControl+5, 1);
										gMultipleImageFileCode = 2;
									
										HideDialogItem (dialogPtr, firstCustomControl+2);
										HideDialogItem (dialogPtr, firstCustomControl+4);
										SetDLogControl (dialogPtr, firstCustomControl+2, 0);
										SetDLogControl (dialogPtr, firstCustomControl+3, 1);
										SetDLogControl (dialogPtr, firstCustomControl+4, 0);
										
										if (gActiveImageWindowInfoH != NULL && 
												MGetString (gTextString, 
																kFileIOStrID, 
																IDS_SelectFileToLink))
											{
											snprintf ((char*)&gTextString[13],
														256-13,
														"%3d", 
														((WindowInfoPtr)*gActiveImageWindowInfoH)->
																						numberImageFiles+1);
											gTextString[16] = ' ';
											
											SetWTitle (callBackParms->window, 
																(UCharPtr)gTextString);
											
											}	// end "if gActiveImageWindowInfoH && ..." 
											
										}	// end "else !gMultipleImageFileCode" 
									break;
								*/									
								case 4:
									itemHit = GetDItemValue (dialogPtr, firstCustomControl+5);
									/*
									itemHit = StandardPopUpMenu (dialogPtr, 
																			0, 
																			firstCustomControl+5, 
																			gPopUpLinkOptionsMenu, 
																			gLinkOptionsSelection, 
																			kPopUpLinkOptionsMenuID);
									*/								
									if (itemHit > 0)
										{
										gLinkOptionsSelection = itemHit;
										
										if (gLinkOptionsSelection == 1)
											NavSetImageWindowFileFlagToFalse (
																					dialogPtr,
																					firstCustomControl+1);
																					
										else if (gLinkOptionsSelection >= 2)
											{
											HideDialogItem (dialogPtr, firstCustomControl+2);
											HideDialogItem (dialogPtr, firstCustomControl+4);
											SetDLogControl (dialogPtr, firstCustomControl+2, 0);
											SetDLogControl (dialogPtr, firstCustomControl+3, 1);
											SetDLogControl (dialogPtr, firstCustomControl+4, 0);
												
											if (gMultipleImageFileCode < 2)
												{
												if (gActiveImageWindowInfoH != NULL &&
															gLinkOptionsSelection == 2 &&
																	MGetString (gTextString,
																					kFileIOStrID,
																					IDS_SelectFileToLink))
													{
													snprintf (
														(char*)&gTextString[13],
														256-13,
														"%3d",
														((WindowInfoPtr)*gActiveImageWindowInfoH)->
																						numberImageFiles+1);
													gTextString[16] = ' ';
													
													SetWTitle (callBackParms->window, 
																	(UCharPtr)gTextString);
													
													}	// end "if gActiveImageWindowInfoH && ..." 
													
												}	// end "if (gMultipleImageFileCode < 2)"
												
											gMultipleImageFileCode = gLinkOptionsSelection; 
											sInitialLinkSelectedFilesFlag = TRUE; 
											
											}	// end "if (gLinkOptionsSelection >= 2)"
																								
										}	// end "if (itemHit > 0)" 
									break;
								
								}	// end "switch (itemHit-firstCustomControl)"
								
							}	// end "if (errCode == noErr)"
						
						}	// end "if (mouseLoc == inContent)"
					break;

				case activateEvt:
					break;

				default:
					break;
					
				}	// end "switch (callBackParms->eventData.eventDataParms.event->what)"
			break;
				
			}	// end "case kNavCBEvent"
			
		case kNavCBCustomize:
			{
			if (gProcessorCode == kOpenImageFileProcessor)
				{
				if (callBackParms->customRect.bottom == 0)
					{
					callBackParms->customRect.right = 
											callBackParms->customRect.left + 400;
					callBackParms->customRect.bottom = 
											callBackParms->customRect.top + 120;
					
					sCustomControlCount = 0;
					
					}	// end "if (callBackParms->customRect.bottom == 0)"
					
				else	// callBackParms->customRect.bottom > 0
					{
					if (sCustomControlCount == 0)
						{
						sCustomControlCount++;
						
						}	// end "if (sCustomControlCount == 0)"
					
					}	// end "if (callBackParms->customRect.bottom == 0)"
				
				}	// end "if (gProcessorCode == kOpenImageFileProcessor)"
			break;
				
			}	// end "case kNavCBCustomize"
			
		case kNavCBStart:
			{
			navUserDataPtr = (NavUserData*)callBackUD;
			if (gProcessorCode == kOpenImageFileProcessor)
				{
						// Open Image or Shape File
						// 	Additional options need to be added.
						
				errCode = NavCustomControl (callBackParms->context,
													kNavCtlAddControlList,
													gCustomNavOpenList);
			
				if (errCode == noErr)										
					errCode = NavCustomControl (callBackParms->context,
														kNavCtlGetFirstControlID,
														&firstCustomControl);
											
				if (errCode == noErr)
					{
					dialogPtr = GetDialogFromWindow (callBackParms->window);
					/*
					navUserDataPtr->drawLinkOptionsPopUpPtr = 
															NewUserItemUPP (DrawLinkOptionsPopUp);
					SetDialogItemDrawRoutine (dialogPtr,
														firstCustomControl+6,
														&theBox,
														navUserDataPtr->drawLinkOptionsPopUpPtr);
					*/
					SetDialogItemDrawRoutine (dialogPtr, 
														&theBox,
														0, 
														firstCustomControl+5,
														(UserItemProcPtr)&DrawLinkOptionsPopUp,
														&navUserDataPtr->drawLinkOptionsPopUpPtr,
														firstCustomControl+5,
														gPopUpLinkOptionsMenu,
														gLinkOptionsSelection);
														
					if	(gActiveImageWindowInfoH == NULL)
						DisableMenuItem (gPopUpLinkOptionsMenu, 2);
					else
						EnableMenuItem (gPopUpLinkOptionsMenu, 2);
					
					if (gGetFileStatus == 0)
						{
						gGetFileImageType = 0;
			
						NavSetImageWindowFileFlagToFalse (dialogPtr, 
																	firstCustomControl+1);
			
						if (!GetFileDlgDetermineLinkVisibility ())
							{
							DisableMenuItem (gPopUpLinkOptionsMenu, 2);
							HideDialogItem (dialogPtr, firstCustomControl+5);
							
							}	// end "if (!GetFileDlgDetermineLinkVisibility ())"
								
						}	// end "if (gGetFileStatus == 0)"
		
					if (gMultipleImageFileCode == 2)
						{
						SetDLogControl (dialogPtr, firstCustomControl+2, 0);
						SetDLogControl (dialogPtr, firstCustomControl+3, 1);
						SetDLogControl (dialogPtr, firstCustomControl+4, 0);
						
						HideDialogItem (dialogPtr, firstCustomControl+2);
						HideDialogItem (dialogPtr, firstCustomControl+4);
						
						ShowDialogItem (dialogPtr, firstCustomControl+5);
						
						gLinkOptionsSelection = 2;
						if (gGetFileStatus == 2)
							SetDLogControlHilite (dialogPtr, firstCustomControl+5, 255);
										
						if (MGetString (gTextString, kFileIOStrID, IDS_SelectFileToLink))
							{
							snprintf ((char*)&gTextString[13],
										256-13,
										"%3d", 
										((WindowInfoPtr)*gActiveImageWindowInfoH)->
																					numberImageFiles+1);
							gTextString[16] = ' ';
							
							SetWTitle (callBackParms->window, (UCharPtr)gTextString);
							
							}	// end "if (MGetIndString (..." 

						}	// end "if (gMultipleImageFileCode == 2)"
						
					//else	// gMultipleImageFileCode != 2
					//	HideDialogItem (dialogPtr, firstCustomControl+5);
					
					}	// end "if (errCode == noErr)"
					
				if (gGetFileStatus != 2)
					sInitialLinkSelectedFilesFlag = FALSE;
													
				}	// end "if (gProcessorCode == kOpenImageFileProcessor)"
				
			NavMenuItemSpec		localMenuItemSpec;
			
			initialSelection = navUserDataPtr->initialMenuSelection;
												
			navMenuItemSpecPtr = (NavMenuItemSpec*)GetHandlePointer (
												(Handle)navUserDataPtr->navMenuItemSpecHandle);
			
			if (navMenuItemSpecPtr != NULL)
				localMenuItemSpec = navMenuItemSpecPtr[initialSelection];
			
			localMenuItemSpec.menuType = initialSelection;
		
			NavCustomControl (callBackParms->context, 
										kNavCtlSelectCustomType, 
										//(void*)&navMenuItemSpecPtr[initialSelection]);
										(void*)&localMenuItemSpec);

			//MHSetState ((Handle)navUserDataPtr->navMenuItemSpecHandle, handleStatus);
			break;
				
			}	// end "case kNavCBStart"
			
		case kNavCBSelectEntry:
			{
			if (gProcessorCode == kOpenImageFileProcessor)
				{
				itemCount = 0;
				docListPtr = 
						(AEDescList*)callBackParms->eventData.eventDataParms.param;
				if (AECountItems (docListPtr, &itemCount) == noErr)
					{
					gOpenImageSelectionListCount = itemCount;
					if (NavCustomControl (callBackParms->context,
															kNavCtlGetFirstControlID,
															&firstCustomControl) == noErr)
						{
						dialogPtr = GetDialogFromWindow (callBackParms->window);
						if (itemCount > 1)
							{
							ShowDialogItem (dialogPtr, firstCustomControl+5);
							EnableMenuItem (gPopUpLinkOptionsMenu, 3);
							//ShowDialogItem (dialogPtr, firstCustomControl+6);

							if (!sInitialLinkSelectedFilesFlag)
								{
								gLinkOptionsSelection = 3;
								gMultipleImageFileCode = gLinkOptionsSelection; 
								sInitialLinkSelectedFilesFlag = TRUE;
								
								}	// end "if (!sInitialLinkSelectedFilesFlag)"
								
							if (gLinkOptionsSelection >= 2)
								{
								SetDLogControl (dialogPtr, firstCustomControl+2, 0);
								SetDLogControl (dialogPtr, firstCustomControl+3, 1);
								SetDLogControl (dialogPtr, firstCustomControl+4, 0);
								HideDialogItem (dialogPtr, firstCustomControl+2);
								HideDialogItem (dialogPtr, firstCustomControl+4);
								SetDItemValue (
										dialogPtr, firstCustomControl+5, gLinkOptionsSelection);
																
								}	// end "if (gLinkOptionsSelection >= 2)"
							
							}	// end "if (itemCount > 1)"
							
						else	// itemCount <= 1
							{
									// At this point we do not know if the itemCount is really 
									// 1 or 0. I do not know why this is the case. If the OK 
									// button is not high lighted, the the count is 0.
									// Unfortunately this does not work since the OK control 
									// gets unhighlighted after this message is sent.
							/*
							ControlRef	theControl;
							GetDialogItemAsControl (dialogPtr, 1, &theControl);
							if (GetControlHilite (theControl) == 255)
								itemCount = 0;

							if (AEGetNthPtr (docListPtr,
													1, 
													typeFSRef, 
													&keyword, 
													&returnedType, 
													&fsRefItem, 
													sizeof (FSRef),
													NULL) != noErr)
								itemCount = 0;
							*/
									// Make sure all of the image type options are available
									// and set them to the most recent setting if the link
									// option is none.
						
							if (!sInitialLinkSelectedFilesFlag || gLinkOptionsSelection == 3)
								gLinkOptionsSelection = 1;
							
							if (gLinkOptionsSelection < 2)
								{
								ShowDialogItem (dialogPtr, firstCustomControl+2);
								ShowDialogItem (dialogPtr, firstCustomControl+4);
							
								SetDLogControl (dialogPtr,
														firstCustomControl+2,
														(gGetFileImageType==0));
								SetDLogControl (dialogPtr,
														firstCustomControl+3,
														(gGetFileImageType==kMultispectralImageType));
								SetDLogControl (dialogPtr,
														firstCustomControl+4,
														(gGetFileImageType==kThematicImageType));
								
								}	// end "if (gLinkOptionsSelection < 2)"
								
							DisableMenuItem (gPopUpLinkOptionsMenu, 3);
							//HideDialogItem (dialogPtr, firstCustomControl+5);
							if (!GetFileDlgDetermineLinkVisibility () || itemCount == 0)
								HideDialogItem (dialogPtr, firstCustomControl+5);
								
							else	// GetFileDlgDetermineLinkVisibility () && itemCount == 1
								ShowDialogItem (dialogPtr, firstCustomControl+5);
								
							SetDItemValue (
									dialogPtr, firstCustomControl+5, gLinkOptionsSelection);
								
							//sInitialLinkSelectedFilesFlag = FALSE;
							
							}	// end "else itemCount <= 1"
							
						}	// end "if (NavCustomControl (callBackParms->context, ..."
					
					}	// end "if (AECountItems (docListPtr, &itemCount) == noErr)"
					
				}	// end "if (gProcessorCode == kOpenImageFileProcessor)"
			break;
			
			}	// end "case kNavCBSelectEntry"
			
		case kNavCBPopupMenuSelect:
			{
			if (callBackParms != NULL)
				{						
				navUserDataPtr = (NavUserData*)callBackUD;
												
				menuItemSpecPtr = 
						(NavMenuItemSpec*)callBackParms->eventData.eventDataParms.param;
				
				navMenuItemSpecPtr = (NavMenuItemSpec*)GetHandlePointer (
											(Handle)navUserDataPtr->navMenuItemSpecHandle);
											
				if (menuItemSpecPtr->menuCreator)	// (= 'extn')
					navUserDataPtr->filterSelection = 
							navMenuItemSpecPtr[menuItemSpecPtr->menuType].menuType;
				
				else	// !menuItemSpecPtr->menuCreator  (= \0\0\0\0)
							// All Documents
					navUserDataPtr->filterSelection = 0;
					
				}	// end "if (callBackParms != NULL)"
			break;
				
			}	// end "case kNavCBPopupMenuSelect"
			
		case kNavCBAdjustPreview:
			{		
			if (callBackParms != NULL)
				{						
				Boolean previewShowing;
			
				errCode = NavCustomControl (callBackParms->context, 
														kNavCtlIsPreviewShowing, 
														&previewShowing);
														
				if  (errCode == noErr && previewShowing)
					previewShowing = FALSE;
													
				}	// end "if (callBackParms != NULL)"		
			break;
				
			}	// end "case kNavCBAdjustPreview"
			
		}	// end "switch (callBackSelector)"
		
}	// end "NavServicesGetEventProc" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 NavServicesGetFile
//
//	Software purpose:	The purpose of this routine is to put up the
//							navigation services get file dialog box to allow the user to
//							select the file to read from.
//		
//	Parameters in:		Address to MFileInfo structure for selected file.
//							Number of parameters in type list.
//							Address of type list.
//
//	Parameters out:	MFileInfo Structure parameters for selected file.
//
// Value Returned:	Error code for file operations.
//
// Called By:			GetSTASupportFile in SHistogram.cpp
//							OpenImageFile in SOpenFileDialog.cpp
//							GetProjectFile in SProject.cpp
//							UserLocateProjectBaseImage in SProject.cpp
//							LoadTransformationFile in SSaveWrite.cpp
//
//	Coded By:			Larry L. Biehl			Date: 08/24/2001
//	Revised By:			Larry L. Biehl			Date: 05/03/2020

SInt16 NavServicesGetFile (
				CMFileStream* 						fileStreamPtr, 
				SInt16								numberTypes, 
				OSType*								fileTypesPtr, 
				LocalAppFile*						localAppFilePtr,
				FSRef*								fileAsFSRefPtr,
				UInt32*								itemCountPtr,
				SInt16								stringIndex)

{
	CFStringRef							cfStringRef;
	FSCatalogInfo						fsCatalogInfo;
	FSRef 								fileAsFSRef;
	NavDialogCreationOptions 		dialogCreationOptions;
	NavDialogRef 						dialog;
	CFStringRef							menuItemString[5];
	FSRef*								localFileAsFSRefPtr;

	NavReplyRecord						theReply;
	//NavDialogOptions					dialogOptions;
	
	FileInfoPtr							fileInfoPtr = NULL;
	CMFileStream*						getFile_FileStreamPtr;
	NavEventUPP							eventUPP;
	NavMenuItemSpec*					navMenuItemSpecPtr = NULL;
	NavObjectFilterUPP				filterUPP;
	NavPreviewUPP						previewUPP = NULL;
	
	Handle								fileInfoHandle = NULL;
	NavMenuItemSpecArrayHandle		navMenuItemSpecHandle = NULL;
	
	NavUserData							navUserData;
					
	//AEKeyword							keyword;
	//DescType								returnedType;
	//Size									actSize;
	
	SInt32								countIndex,
											itemCount;
	
	SInt16								errCode = noErr,
											filterCode,
											numberPopupMenuItem;
											
	Boolean								fileSelectedFlag;
	
		
			// Initialize variables.															
			
	gGetFileStatus = 3;
	fileSelectedFlag = FALSE;
	
			// Make certain that MultiSpec is not in the background.					
			
	PauseIfInBackground ();
		
			// Check if this routine called from MultiSpec startup, in which		
			// case the file to open has already been selected.						
			
	if (localAppFilePtr != NULL)
		{
		fileStreamPtr->vRefNum = localAppFilePtr->vRefNum;
		fileStreamPtr->parID = localAppFilePtr->parID;
		CopyPToP (fileStreamPtr->fileName, localAppFilePtr->fNamePtr);
		fileStreamPtr->fSSpecFlag = localAppFilePtr->fSSpecFlag;
		
		fileStreamPtr->fsRef = localAppFilePtr->fsRef;
		fileStreamPtr->fSRefFlag = localAppFilePtr->fSRefFlag;
		
		if (itemCountPtr != NULL)
			*itemCountPtr = 1;
		
		}	// end "if (localAppFilePtr != NULL)" 
	
			// Else file has not been selected.												
			// Put the file dialog box up to select the input file.					
	
	else	// localAppFilePtr == NULL 
		{	
				// Get the requested prompt string.											
				
		if (!GetSpecifiedStringNumber (kFileIOStrID, stringIndex, gTextString, TRUE))
																					return (memFullErr);
		
		if (gMultipleImageFileCode == 2)
			{
			snprintf ((char*)&gTextString[13],
						256-13, 
						"%3d",
						((WindowInfoPtr)*gActiveImageWindowInfoH)->numberImageFiles+1);
			gTextString[16] = ' ';
			gGetFileStatus = 2;
			
			}	// end "if (gMultipleImageFileCode == 2)" 
			
		else	// !gMultipleImageFileCode 
			{
			gGetFileStatus = -1;
			if (stringIndex == IDS_SelectImage)
				gGetFileStatus = 0;
				
			}	// end "else !gMultipleImageFileCode" 
			
		theReply.validRecord = FALSE;
			
				// Only call standard io get file if there is a minimum amount of	
				// memory available.																
				
		if (CheckIfMemoryAvailable (15000))
			{
					// Get the resource number for the filter list.
					
			filterCode = 0;
			numberPopupMenuItem = 0;
			navUserData.stringIndex = 0;
			navUserData.initialMenuSelection = 0;
			switch (stringIndex)
				{
				case IDS_SelectImage:
					navUserData.stringIndex = IDS_SelectImage;
					filterCode = kMultiThematicShapeFiles;	// multispectral, thematic, 
																		// or shape files
					numberPopupMenuItem = 5;
					navUserData.initialMenuSelection = gImageFileFilterIndex;
					if (gActiveImageWindow == NULL)
						{
						filterCode =  kMultiThematicFiles;	// multispectral, thematic
						numberPopupMenuItem = 4;
						
						if (navUserData.initialMenuSelection == 4)
							navUserData.initialMenuSelection = 0;
						
						}	// end "if (gActiveImageWindow == NULL)"
					break;
					
				case IDS_SelectImageStatistics:	// image statistics (STA)
					filterCode = kSTAFiles;
					numberPopupMenuItem = 1;
					navUserData.initialMenuSelection = 0;
					break;
					
				case IDS_SelectProject:		// project
					filterCode = kProjectFiles;
					numberPopupMenuItem = 1;
					navUserData.initialMenuSelection = 0;
					break;
				
				case IDS_SelectFileToLink:		// multispectral
				case IDS_SelectNewBaseImage:
				case IDS_WhereIsProjectImage:
					filterCode = kMultiFiles;
					numberPopupMenuItem = 1;
					navUserData.initialMenuSelection = 0;
					break;
				
				case IDS_FileIO62:	// transformation
					filterCode = kTransformFiles;
					numberPopupMenuItem = 1;
					navUserData.initialMenuSelection = 0;
					break;
					
				case IDS_FileIO65:	// probability
					filterCode = kProbablilityFiles;
					numberPopupMenuItem = 1;
					navUserData.initialMenuSelection = 0;
					break;
					
				case IDS_FileIO76:	// thematic class info (TRL)
					filterCode = kTRLFiles;
					numberPopupMenuItem = 1;
					navUserData.initialMenuSelection = 0;
					break;
					
				case IDS_FileIO77:	// thematic group info
					filterCode = kThematicGroupInfoFiles;
					numberPopupMenuItem = 1;
					navUserData.initialMenuSelection = 0;
					break;
					
				case IDS_FileIO51:	// multispectral, thematic
				case IDS_FileIO52:
				case IDS_FileIO95:
				case IDS_FileIO96:
				case IDS_FileIO103:
				case IDS_FileIO104:
				case IDS_FileIO105:
					filterCode = kMultiThematicFiles;
					numberPopupMenuItem = 4;
					navUserData.initialMenuSelection = 0;
					break;
					
				case IDS_FileIO146:	// all
					filterCode = 0;		// kAllTextFiles;
					numberPopupMenuItem = 0;
					navUserData.initialMenuSelection = 0;
					break;
				
				}	// end "switch (stringIndex)"
				
			if (numberPopupMenuItem > 0)
				{
						// A file information structure will be needed.

				fileInfoHandle = MNewHandle (sizeof (MFileInfo));
				fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle,
																		kLock);
				
				if (fileInfoPtr != NULL)
					{														
					navMenuItemSpecHandle = (NavMenuItemSpecHandle)MNewHandle (
													numberPopupMenuItem*sizeof (NavMenuItemSpec));
												
					navMenuItemSpecPtr = (NavMenuItemSpec*)GetHandlePointer (
																		(Handle)navMenuItemSpecHandle,
																		kLock);
					
					getFile_FileStreamPtr = GetFileStreamPointer (fileInfoPtr);													
					InitializeFileStream (getFile_FileStreamPtr);
					
					}	// end "if (fileInfoPtr != NULL)"
													
				}	// end "if (numberPopupMenuItem > 0)"
			
					// Finish setting up navigation user data structure.
					
			navUserData.fileInfoPtr	= fileInfoPtr;
			navUserData.fileInfoHandle	= fileInfoHandle;
			navUserData.navMenuItemSpecHandle = navMenuItemSpecHandle;	
			navUserData.filterSelection = 0;
			
			if (navMenuItemSpecPtr != NULL)
				{
				switch (filterCode)
					{
					case kMultiThematicShapeFiles:					
						navMenuItemSpecPtr[0].version = kNavMenuItemSpecVersion;	
						navMenuItemSpecPtr[0].menuCreator = gCreator;
						navMenuItemSpecPtr[0].menuType = kAllReadableFiles;
						CopyPToP (navMenuItemSpecPtr[0].menuItemName,
										(UCharPtr)"\0All Readable Files\0");
					
						navMenuItemSpecPtr[1].version = kNavMenuItemSpecVersion;	
						navMenuItemSpecPtr[1].menuCreator = gCreator;
						navMenuItemSpecPtr[1].menuType = 0;
						CopyPToP (navMenuItemSpecPtr[1].menuItemName,
										(UCharPtr)"\0-\0");
					
						navMenuItemSpecPtr[2].version = kNavMenuItemSpecVersion;	
						navMenuItemSpecPtr[2].menuCreator = gCreator;
						navMenuItemSpecPtr[2].menuType = kMultiFiles;
						CopyPToP (navMenuItemSpecPtr[2].menuItemName,
										(UCharPtr)"\0Multispectral Image Files\0");
																			
						navMenuItemSpecPtr[3].version = kNavMenuItemSpecVersion;	
						navMenuItemSpecPtr[3].menuCreator = gCreator;
						navMenuItemSpecPtr[3].menuType = kThematicFiles;
						CopyPToP (navMenuItemSpecPtr[3].menuItemName,
										(UCharPtr)"\0Thematic Image Files\0");
										
						navMenuItemSpecPtr[4].version = kNavMenuItemSpecVersion;	
						navMenuItemSpecPtr[4].menuCreator = gCreator;
						navMenuItemSpecPtr[4].menuType = kShapeFiles;
						CopyPToP (navMenuItemSpecPtr[4].menuItemName, 
										(UCharPtr)"\0Shape Files\0");
	      				
						navUserData.filterSelection = kAllReadableFiles;
						break;
					
					case kSTAFiles:																				
						navMenuItemSpecPtr[0].version = kNavMenuItemSpecVersion;	
						navMenuItemSpecPtr[0].menuCreator = gCreator;
						navMenuItemSpecPtr[0].menuType = kSTAFiles;
						CopyPToP (navMenuItemSpecPtr[0].menuItemName, 
										(UCharPtr)"\0Image Statistics Files\0");
						navUserData.filterSelection = kSTAFiles;
						break;
						
					case kProjectFiles:				
						navMenuItemSpecPtr[0].version = kNavMenuItemSpecVersion;	
						navMenuItemSpecPtr[0].menuCreator = gCreator;
						navMenuItemSpecPtr[0].menuType = kProjectFiles;
						CopyPToP (navMenuItemSpecPtr[0].menuItemName, 
										(UCharPtr)"\0Project Files\0");
						navUserData.filterSelection = kProjectFiles;
						break;
						
					case kTRLFiles:				
						navMenuItemSpecPtr[0].version = kNavMenuItemSpecVersion;	
						navMenuItemSpecPtr[0].menuCreator = gCreator;
						navMenuItemSpecPtr[0].menuType = kTRLFiles;
						CopyPToP (navMenuItemSpecPtr[0].menuItemName, 
										(UCharPtr)"\0Thematic Class Info Files\0");
						navUserData.filterSelection = kTRLFiles;
						break;
					
					case kTransformFiles:																				
						navMenuItemSpecPtr[0].version = kNavMenuItemSpecVersion;	
						navMenuItemSpecPtr[0].menuCreator = gCreator;
						navMenuItemSpecPtr[0].menuType = kTransformFiles;
						CopyPToP (navMenuItemSpecPtr[0].menuItemName, 
										(UCharPtr)"\0Transformation Matrix Files\0");
						navUserData.filterSelection = kTransformFiles;
						break;
						
					case kThematicFiles:
					case kProbablilityFiles:				
						navMenuItemSpecPtr[0].version = kNavMenuItemSpecVersion;	
						navMenuItemSpecPtr[0].menuCreator = gCreator;
						navMenuItemSpecPtr[0].menuType = kThematicFiles;
						CopyPToP (navMenuItemSpecPtr[0].menuItemName, 
										(UCharPtr)"\0Thematic Image Files\0");
						navUserData.filterSelection = kThematicFiles;
						break;
						
					case kMultiThematicFiles:					
						navMenuItemSpecPtr[0].version = kNavMenuItemSpecVersion;	
						navMenuItemSpecPtr[0].menuCreator = 0;
						navMenuItemSpecPtr[0].menuType = kAllReadableImageFiles;
						CopyPToP (navMenuItemSpecPtr[0].menuItemName, 
										(UCharPtr)"\0All Readable Files\0");
					
						navMenuItemSpecPtr[1].version = kNavMenuItemSpecVersion;	
						navMenuItemSpecPtr[1].menuCreator = 1;
						navMenuItemSpecPtr[1].menuType = 0;
						CopyPToP (navMenuItemSpecPtr[1].menuItemName, 
										(UCharPtr)"\0---\0");
					
						navMenuItemSpecPtr[2].version = kNavMenuItemSpecVersion;	
						navMenuItemSpecPtr[2].menuCreator = 2;
						navMenuItemSpecPtr[2].menuType = kMultiFiles;
						CopyPToP (navMenuItemSpecPtr[2].menuItemName, 
										(UCharPtr)"\0Multispectral Image Files\0");
																			
						navMenuItemSpecPtr[3].version = kNavMenuItemSpecVersion;	
						navMenuItemSpecPtr[3].menuCreator = 3;
						navMenuItemSpecPtr[3].menuType = kThematicFiles;
						CopyPToP (navMenuItemSpecPtr[3].menuItemName, 
										(UCharPtr)"\0Thematic Image Files\0");
						navUserData.filterSelection = kAllReadableImageFiles;
						break;
						
					case kMultiFiles:					
						navMenuItemSpecPtr[0].version = kNavMenuItemSpecVersion;	
						navMenuItemSpecPtr[0].menuCreator = gCreator;
						navMenuItemSpecPtr[0].menuType = kMultiFiles;
						CopyPToP (navMenuItemSpecPtr[0].menuItemName, 
										(UCharPtr)"\0Multispectral Image Files\0");
						navUserData.filterSelection = kMultiFiles;
						break;
					/*
							// Note that even though this says all text files, the 
							// implimentation will be for all files. Don't know best way 
							// to limit filter to just text files.
					case kAllTextFiles:		
						navMenuItemSpecPtr[0].version = kNavMenuItemSpecVersion;	
						navMenuItemSpecPtr[0].menuCreator = 0;
						navMenuItemSpecPtr[0].menuType = kMultiFiles;
						CopyPToP ((char*)navMenuItemSpecPtr[0].menuItemName, 
										"\0All Files\0");
						navUserData.filterSelection = kAllTextFiles;
						break;
					*/
					}	// end "switch (filterCode)"
				
						// If the numberType is -1, then user has option key down and want 
						// all filetypes, available to select.
							
				//if (numberTypes == -1)
				//	navUserData.filterSelection = 0;
						
				}	// end "if (navMenuItemSpecPtr != NULL)"
						
			//CheckAndUnlockHandle ((Handle)navMenuItemSpecHandle);
				
			eventUPP = NewNavEventUPP (NavServicesGetEventProc);
			filterUPP = NewNavObjectFilterUPP (NavServicesFilterProc);
			//previewUPP = NewNavPreviewUPP (NavServicesPreviewProc);

					// Get the standard set of default dialog options
					
			errCode = NavGetDefaultDialogCreationOptions (&dialogCreationOptions);
			
			if (errCode == noErr)
				{
				dialogCreationOptions.modality = kWindowModalityAppModal;
				
				dialogCreationOptions.clientName = CFStringCreateWithPascalString (
																		NULL,
																		(UCharPtr)"\0MultiSpec", 
																		kCFStringEncodingMacRoman);
				
				dialogCreationOptions.windowTitle = CFStringCreateWithPascalString (
																		NULL,
																		(UCharPtr)gTextString, 
																		kCFStringEncodingMacRoman);
			
				dialogCreationOptions.preferenceKey = kOpenPrefKey;
			
						// We will do the translation ourselves later
					
				dialogCreationOptions.optionFlags |= kNavDontAutoTranslate;
				
						// Clear the preview option
						
				dialogCreationOptions.optionFlags ^= kNavAllowPreviews;
				//dialogCreationOptions.optionFlags &= ~kNavAllowPreviews;
				
						// Include 'All Files' in popup
					
				dialogCreationOptions.optionFlags |= kNavAllFilesInPopup;
				
						// Force all readable file types in popup to be selected when
						// opened.
				
				dialogCreationOptions.optionFlags |= kNavSelectAllReadableItem;
																		
						// Determine whether the user can select multiple files.
						// If the type of request is limited to just one file, then use
						// use the local FSRef structure. This is signified by the input
						// fileAsFSRefPtr being NULL.
						
				dialogCreationOptions.optionFlags ^= kNavAllowMultipleFiles;
				localFileAsFSRefPtr = &fileAsFSRef;
				if (fileAsFSRefPtr != NULL && 
						(filterCode == kMultiThematicFiles || 
								filterCode == kMultiThematicShapeFiles ||
										stringIndex == IDS_SelectFileToLink))
					{
					dialogCreationOptions.optionFlags |= kNavAllowMultipleFiles;
					localFileAsFSRefPtr = fileAsFSRefPtr;
					
					}	// end "if (fileAsFSRefPtr != NULL && ..."
				
						// The default selection is the browser selection.
					
				dialogCreationOptions.optionFlags |= kNavSelectDefaultLocation;
				
						// Fill the array with the strings for the popup type menu.
				
				if (numberPopupMenuItem > 0)		
					menuItemString[0] = CFStringCreateWithPascalString (
												NULL,
												(UCharPtr)navMenuItemSpecPtr[0].menuItemName, 
												kCFStringEncodingMacRoman);
				
				if (numberPopupMenuItem > 1)
					menuItemString[1] = CFStringCreateWithPascalString (
												NULL,
												(UCharPtr)navMenuItemSpecPtr[1].menuItemName, 
												kCFStringEncodingMacRoman);
				if (numberPopupMenuItem > 2)
					menuItemString[2] = CFStringCreateWithPascalString (
												NULL,
												(UCharPtr)navMenuItemSpecPtr[2].menuItemName, 
												kCFStringEncodingMacRoman);
				
				if (numberPopupMenuItem > 3)
					menuItemString[3] = CFStringCreateWithPascalString (
												NULL,
												(UCharPtr)navMenuItemSpecPtr[3].menuItemName, 
												kCFStringEncodingMacRoman);
				
				if (numberPopupMenuItem > 4)
					menuItemString[4] = CFStringCreateWithPascalString (
												NULL,
												(UCharPtr)navMenuItemSpecPtr[4].menuItemName, 
												kCFStringEncodingMacRoman);
				
				dialogCreationOptions.popupExtension = CFArrayCreate (
												kCFAllocatorDefault,
												(const void**)menuItemString, 
												numberPopupMenuItem, 
												NULL);
				
				}	// end "if (errCode == noErr)"
			
			if (errCode == noErr)
				errCode = NavCreateGetFileDialog (&dialogCreationOptions,
																NULL,
																eventUPP,
																previewUPP,		// Custom previews
																filterUPP,		// filterUPP,
																&navUserData,  // can be NULL
																&dialog);

					// Show the dialog box

			if (errCode == noErr)
				errCode = NavDialogRun (dialog);
			
					// Release the CFStrings.
					
			CFRelease (dialogCreationOptions.clientName);
			CFRelease (dialogCreationOptions.windowTitle);
			
			if (numberPopupMenuItem > 0)	
				CFRelease (menuItemString[0]);
			
			if (numberPopupMenuItem > 1)	
				CFRelease (menuItemString[1]);
			
			if (numberPopupMenuItem > 2)	
				CFRelease (menuItemString[2]);
			
			if (numberPopupMenuItem > 3)	
				CFRelease (menuItemString[3]);
			
			if (numberPopupMenuItem > 4)	
				CFRelease (menuItemString[4]);
				
			if (dialogCreationOptions.popupExtension != NULL)
				CFRelease (dialogCreationOptions.popupExtension);

					// Get the reply
					
			if (errCode == noErr)
				errCode = NavDialogGetReply (dialog, &theReply);
			
			if (theReply.validRecord && errCode == noErr)
				{
						// Count the items in the list.												
						
				errCode = AECountItems (&(theReply.selection), &itemCount);
				
				if (errCode == noErr && itemCount > 0)
					{						
					errCode = AEGetNthPtr (&(theReply.selection), 
													1, 
													typeFSRef, 
													NULL, 
													NULL, 
													localFileAsFSRefPtr, 
													sizeof (FSRef),
													NULL);
						
					if (errCode == noErr)
						{
						fileStreamPtr->fSRefFlag = TRUE;
						fileStreamPtr->fsRef = localFileAsFSRefPtr[0];
						
						errCode = FSGetCatalogInfo (&fileStreamPtr->fsRef,
																kFSCatInfoVolume,
																&fsCatalogInfo,
																&fileStreamPtr->uniFileName,
																NULL,
																&fileStreamPtr->parentFSRef);
		
						if (errCode == noErr)
							{
							fileSelectedFlag = TRUE;
						
							fileStreamPtr->vRefNum = fsCatalogInfo.volume;
																
							cfStringRef = CFStringCreateWithCharacters (
																kCFAllocatorDefault,
																fileStreamPtr->uniFileName.unicode,
																fileStreamPtr->uniFileName.length);
																
							CFStringGetCString (cfStringRef,
													(char*)&fileStreamPtr->fileName[1],
													(CFIndex)255,
													kCFStringEncodingUTF8);
							fileStreamPtr->fileName[0] =
												strlen ((char*)&fileStreamPtr->fileName[1]);
							
									// This indicates that the full path is not part of the
									// file name.
							
							fileStreamPtr->pathLength = 0;
																
							//CFAllocatorDeallocate (kCFAllocatorDefault,
							//									(void*)cfStringRef);
							CFRelease (cfStringRef);
							
							}	// end "if (errCode == noErr)"
						
						}	// end "if (errCode == noErr)"
						
					if (errCode == noErr && itemCount > 1)
						{
								// Get the rest of the FSRef's. They will get handled
								// later when the images are linked with the first one.
								// Note that the count is currently limited to
								// gMaximumNumberOfLinkedFiles (500).
						
						if (itemCount > gMaximumNumberOfLinkedFiles)
							DisplayAlert (kErrorAlertID,
												kStopAlert,
												kAlertStrID,
												IDS_Alert147,
												0,
												NULL);
							
						itemCount = MIN (itemCount, gMaximumNumberOfLinkedFiles);
						for (countIndex=2; countIndex<=itemCount; countIndex++)
							{
							errCode = AEGetNthPtr (&(theReply.selection), 
															countIndex, 
															typeFSRef, 
															NULL, 
															NULL, 
															&fileAsFSRefPtr[countIndex-1], 
															sizeof (FSRef),
															NULL);
															
							if (errCode != noErr)
								{
								itemCount = countIndex - 1;
								break;
								
								}	// end "if (errCode != noErr)"
															
							}	// end "for (countIndex=2; countIndex<=itemCount; ..."
							
						//if (itemCount > 1)
						//	gMultipleImageFileCode = 3;
						
						}	// end "if (itemCount > 1)"
					
					if (itemCountPtr != NULL)
						*itemCountPtr = itemCount;
						
					}	// end "if (errCode == noErr && itemCount > 0)"
					
				}	// end "if (theReply.validRecord && errCode == noErr)"

			NavDisposeReply (&theReply);
									
			DisposeNavEventUPP (eventUPP);
			DisposeNavObjectFilterUPP (filterUPP);
			//DisposeNavPreviewUPP (previewUPP);
			
					// Save the popup menu selection that the user used.
				
			if (fileSelectedFlag && navUserData.stringIndex == IDS_SelectImage)
				{
				if (navUserData.filterSelection == kAllReadableFiles || 
								navUserData.filterSelection == kAllReadableImageFiles)
					gImageFileFilterIndex = 0;
					
				else if (navUserData.filterSelection == kMultiFiles)
					gImageFileFilterIndex = 2;
					
				else if (navUserData.filterSelection == kThematicFiles)
					gImageFileFilterIndex = 3;
					
				else if (navUserData.filterSelection == kShapeFiles)
					gImageFileFilterIndex = 4;
				
				}	// end "if (fileSelectedFlag && ...)"
			
			navMenuItemSpecHandle =
				(NavMenuItemSpecHandle)UnlockAndDispose ((Handle)navMenuItemSpecHandle);
			fileInfoHandle = UnlockAndDispose (fileInfoHandle);
				
					// Make certain that all previous mouse events have been			
					// removed. This is done in case the user double clicked			
					// to select the file.														
									
	    	FlushEvents (mUpMask,0);
			
			}	// end "if (CheckIfMemoryAvailable (15000))" 
		
				// If file is to be added to the active image window, then the		
				// file type has to be forced to multispectral type.					
				
		if (gMultipleImageFileCode != 0)
			gGetFileImageType = kMultispectralImageType;
		
		CheckSomeEvents (activMask+updateMask);
	
		if (!fileSelectedFlag)
			{
			fileStreamPtr->fileName[0] = 0;
																								return (0);
																					
			}	// end "if (!fileSelectedFlag)"

				// Also make the filename a useable c string

		fileStreamPtr->fileName[fileStreamPtr->fileName[0]+1] = 0x00;
		
		}	// end "else localAppFilePtr == NULL" 
		
			// Get the parentFSRef if this selection was from the LocalAppFile pointer.
			// In other words from a user double clicking on the file rather than using
			// the File->Open Image menu command

	if (!fileStreamPtr->fSSpecFlag && localAppFilePtr != NULL)
		errCode = FSGetCatalogInfo (&fileStreamPtr->fsRef,
												kFSCatInfoVolume,
												&fsCatalogInfo,
												&fileStreamPtr->uniFileName,
												NULL,
												&fileStreamPtr->parentFSRef);

			// Get the file type and creator	
	
	if (errCode == noErr)
		errCode = GetFileTypeAndCreator (fileStreamPtr);			
	/*
	if (fileStreamPtr->fSSpecFlag)
		{
		errCode = FSpGetFInfo ((FSSpec*)fileStreamPtr, &gFinderInfo);

		if (errCode == noErr)
			{
			fileStreamPtr->creator = gFinderInfo.fdCreator;
			fileStreamPtr->type = gFinderInfo.fdType;

			}	// end "if (errCode == noErr)"
		
		}	// end "if (fileStreamPtr->fSSpecFlag)"

	else	// !fileStreamPtr->fSSpecFlag
		{
		if (localAppFilePtr != NULL)
			errCode = FSGetCatalogInfo (&fileStreamPtr->fsRef,
													kFSCatInfoVolume,
													&fsCatalogInfo,
													&fileStreamPtr->uniFileName,
													NULL,
													&fileStreamPtr->parentFSRef);
		
		if (errCode == noErr)
			errCode = FSGetCatalogInfo (&fileStreamPtr->fsRef,
													kFSCatInfoFinderInfo,
													&fsCatalogInfo,
													NULL,
													NULL,
													NULL);													

		if (errCode == noErr)
			{
			fileStreamPtr->creator = ((FileInfo*)fsCatalogInfo.finderInfo)->fileCreator;
			fileStreamPtr->type = ((FileInfo*)fsCatalogInfo.finderInfo)->fileType;

			}	// end "if (errCode == noErr)"
		
		}	// end else	!fileStreamPtr->fSSpecFlag
	*/
	if (errCode == noErr)
		errCode = OpenFileReadOnly (fileStreamPtr, 
												kResolveAliasChains,
												kLockFile,
												kDoNotVerifyFileStream);
					
	IOCheck (errCode, fileStreamPtr);
		
			// To allow this will also involve checking for this refnum in other	
			// windows when closing a file, because it is the same path as that	
			// obtained for the first open.													
				
	//if (errCode == opWrErr)
	//	errCode = noErr;
	
	if (errCode != noErr)	
		SetReferenceNumber (fileStreamPtr, 0);	
		
	return (errCode);

}	// end "NavServicesGetFile"


/*
#if defined multispec_mac 
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal Boolean NavServicesPreviewProc
//
//	Software purpose:	The purpose of this routine is to respond to handle preview
//							requests. Decided not to use for now because of performance
//							issues with large image files.
//		
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:	None
//
// Called By:			
//
//	Coded By:			From Apple Inc. examples
//	Revised By:			Larry L. Biehl			Date: 11/17/2009

pascal Boolean NavServicesPreviewProc (
				NavCBRecPtr							callBackParms,
				void*									callBackUD)				
{

	
	return (FALSE);
	
}	// end "NavServicesPreviewProc"
#endif	// defined multispec_mac 
*/

  
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void NavServicesPutEventProc
//
//	Software purpose:	The purpose of this routine is to respond to event prompts from
//							the NavServices Put File routine.
//		
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:	None
//
// Called By:			
//
//	Coded By:			From Apple Inc. examples
//	Revised By:			Larry L. Biehl			Date: 01/04/2013

pascal void NavServicesPutEventProc (
				NavEventCallbackMessage 		callBackSelector, 
				NavCBRecPtr 						callBackParms, 
				NavCallBackUserData 				callBackUD)
				
{
	Point									mousePt;
	
	DialogPtr							dialogPtr;
	NavMenuItemSpec*					menuItemSpecPtr;
	NavUserData*						navUserDataPtr;
	
	OSErr 								errCode = noErr;
	
	SInt16								firstCustomControl,
											itemHit;
											
	Boolean								changeCheckBoxFlag;
	
	
	switch (callBackSelector)
		{
		case kNavCBEvent:
			{
			switch (callBackParms->eventData.eventDataParms.event->what)
				{
				case nullEvent:
					break;
					
				case updateEvt:
					if (callBackParms->window != (WindowPtr)callBackParms->
															eventData.eventDataParms.event->message)
  						DoUpdateEvent (callBackParms->eventData.eventDataParms.event);
					break;
					
	      	case mouseDown:

					dialogPtr = GetDialogFromWindow (callBackParms->window);
					mousePt = callBackParms->eventData.eventDataParms.event->where;
					GlobalToLocal (&mousePt);
		  			
		  			itemHit = FindDialogItem (dialogPtr, mousePt);
		  			
					if (itemHit > 0)
		  				{
						errCode = NavCustomControl (callBackParms->context,
																kNavCtlGetFirstControlID,
																&firstCustomControl);
						
						if (errCode == noErr)
							{			
							changeCheckBoxFlag = FALSE;
								
							switch (itemHit-firstCustomControl)
								{
								case 0:
									break;
									
								case 1:
									if (!gProjectInfoPtr->keepClassStatsOnlyFlag)
										{
										gProjectInfoPtr->saveFieldSumsSquaresFlag = !
												gProjectInfoPtr->saveFieldSumsSquaresFlag;
										changeCheckBoxFlag = TRUE;
										
										}	// end "if (!...->keepClassStatsOnlyFlag)"
									break;
			
								case 2:
									if (!gProjectInfoPtr->keepClassStatsOnlyFlag)
										{
										gProjectInfoPtr->saveFieldMeansCovFlag = !
												gProjectInfoPtr->saveFieldMeansCovFlag;
										changeCheckBoxFlag = TRUE;
										
										}	// end "if (!...->keepClassStatsOnlyFlag)"
									break;
									
								case 3:
									if (gProjectInfoPtr->classChanStatsPtr != NULL)
										{
										gProjectInfoPtr->saveClassSumsSquaresFlag = !
												gProjectInfoPtr->saveClassSumsSquaresFlag;
										changeCheckBoxFlag = TRUE;
										
										}	// end "if (...->classChanStatsPtr != NULL)"
									break;
									
								case 4:
									if (gProjectInfoPtr->classChanStatsPtr != NULL)
										{
										gProjectInfoPtr->saveClassMeansCovFlag = !
												gProjectInfoPtr->saveClassMeansCovFlag;
										changeCheckBoxFlag = TRUE;
										
										}	// end "if (...->classChanStatsPtr != NULL)"
									break;
								
								}	// end "switch (countDITL-firstCustomControl)"
	
							if (changeCheckBoxFlag)
								ChangeDLogCheckBox (dialogPtr, itemHit+1);
								
							}	// end "if (errCode == noErr)"
						
						}	// end "if (mouseLoc == inContent)"
					break;

				case activateEvt:
					break;

				default:
					break;
					
				}	// end "switch (callBackParms->eventData.eventDataParms.event->what)"
			break;
				
			}	// end "case kNavCBEvent"
			
		case kNavCBCustomize:
			{
			if (gProcessorCode == kSaveProjectAsProcessor)
				{
				if (callBackParms->customRect.bottom == 0)
					{
					callBackParms->customRect.right = 
											callBackParms->customRect.left + 400;
					callBackParms->customRect.bottom = 
											callBackParms->customRect.top + 110;
					
					sCustomControlCount = 0;
					
					}	// end "if (callBackParms->customRect.bottom == 0)"
					
				else	// callBackParms->customRect.bottom > 0
					{
					if (sCustomControlCount == 0)
						{
						sCustomControlCount++;
						
						}	// end "if (sCustomControlCount == 0)"
					
					}	// end "if (callBackParms->customRect.bottom == 0)"
				
				}	// end "if (gProcessorCode == kSaveProjectAsProcessor)"
			break;
				
			}	// end "case kNavCBCustomize"
			
		case kNavCBStart:
			{
			if (gProcessorCode == kSaveProjectAsProcessor)
				{
						// Save Project File
						// 	Additional options need to be added.
						
				errCode = NavCustomControl (callBackParms->context,
													kNavCtlAddControlList,
													gCustomNavPut);
			
				if (errCode == noErr)										
					errCode = NavCustomControl (callBackParms->context,
														kNavCtlGetFirstControlID,
														&firstCustomControl);
														
				if (errCode == noErr)
					{
					dialogPtr = GetDialogFromWindow (callBackParms->window);
			
					if (gGetFileStatus == 0)
						{
						if (gProjectInfoPtr->keepClassStatsOnlyFlag)
							{
							SetDLogControlHilite (dialogPtr, firstCustomControl+2, 255);
							SetDLogControlHilite (dialogPtr, firstCustomControl+3, 255);
							
							gProjectInfoPtr->saveFieldSumsSquaresFlag = FALSE;
							gProjectInfoPtr->saveFieldMeansCovFlag = FALSE;
							
							}	// end "if (gProjectInfoPtr->keepClassStatsOnlyFlag)" 
						
						if (gProjectInfoPtr->classChanStatsPtr == NULL)
							{
							SetDLogControlHilite (dialogPtr, firstCustomControl+4, 255);
							SetDLogControlHilite (dialogPtr, firstCustomControl+5, 255);
							
							gProjectInfoPtr->saveClassSumsSquaresFlag = FALSE;
							gProjectInfoPtr->saveClassMeansCovFlag = FALSE;
							
							}	// end "if (gProjectInfoPtr->classChanStatsPtr == ...)" 
							
						SetDLogControl (dialogPtr, 
												firstCustomControl+2, 
												gProjectInfoPtr->saveFieldSumsSquaresFlag);
												
						SetDLogControl (dialogPtr, 
												firstCustomControl+3, 
												gProjectInfoPtr->saveFieldMeansCovFlag);
												
						SetDLogControl (dialogPtr, 
												firstCustomControl+4, 
												gProjectInfoPtr->saveClassSumsSquaresFlag);
												
						SetDLogControl (dialogPtr, 
												firstCustomControl+5, 
												gProjectInfoPtr->saveClassMeansCovFlag);
						
						gGetFileStatus = 2;
						
						}	// end "if (gGetFileStatus == 0)" 
					
					if (gGetFileStatus <= 0)	
						gGetFileStatus = 1;
					
					}	// end "if (errCode == noErr)"
				
				}	// end "if (gProcessorCode == kSaveProjectAsProcessor)"
			
					// Set the default folder to put the file in.
					// Get the parent folder for the active window.
			
			AEDesc theLocation = {typeNull, NULL};
			FSSpec theFSSpec;
			navUserDataPtr = (NavUserData*)callBackUD;
							
			errCode = FSGetCatalogInfo (&navUserDataPtr->parentFSRef, 
													kFSCatInfoNone, 
													NULL, 
													NULL, 
													&theFSSpec,
													NULL);
			
			if (errCode == noErr)										
				errCode = AECreateDesc (typeFSS, 
													&theFSSpec, 
													sizeof (FSSpec), 
													&theLocation);
											
			if (errCode == noErr)
				errCode = NavCustomControl (callBackParms->context,
														kNavCtlSetLocation, 
														(void*)&theLocation);		
														
			if (errCode == noErr)
				errCode = noErr;
				
			else	// errCode != noErr
				errCode = -2;
			/*		
			OSStatus theStatus = noErr;
			AEDesc theLocation = {typeNull, NULL);
			FSSpec theFSSpec;
			theStatus = FSMakeFSSpec (...<your construction here>..., &theFSSpec);
			OSErr FSMakeFSRefUnicode (const FSRef *parentRef, 
												UniCharCount nameLength, 
												const UniChar *name, 
												TextEncoding textEncodingHint, 
												FSRef *newRef);
			if (theStatus != noErr) {... your error handling here ...}
			theStatus = AECreateDesc (typeFSRef, 
												&theFSSpec, 
												sizeof (FSRef), 
												&theLocation));
			if (theStatus != noErr) {... your error handling here ...}
			theStatus = NavCustomControl (callBackParms->context,
                        kNavCtlSetLocation, (void*)&theLocation);
			if (theStatus != noErr) {... your error handling here ...}
			
			So FindFolder is the best choice here. In fact we can use a "modern" 
				variant that returns a FSRef which we can convert down to an FSSpec:

			FSRef theUsersFolderRef;
			OSStatus result;

			result = FSFindFolder kOnAppropriateDisk,
											kUsersFolderType,
											FALSE,
											&theUsersFolderRef);

			Then you can convert that to an FSSpec:

			FSSpec theUsersFolderSpec;

			FSGetCatalogInfo (theUsersFolderRef,
										0,
										NULL,
										NULL,
										&theUsersFolderSpec,
										NULL);
			*/				
			AEDisposeDesc (&theLocation);
			break;
				
			}	// end "case kNavCBStart"
			
		case kNavCBPopupMenuSelect:
			{
			if (callBackParms != NULL)
				menuItemSpecPtr = 
						(NavMenuItemSpec*)callBackParms->eventData.eventDataParms.param;
			break;
				
			}	// end "case kNavCBPopupMenuSelect"
			
		case kNavCBAccept:
					// This code will have to be changed for nav services.
					// Don't know how yet.
					
			if (gGetFileStatus == 2)
				{
				if (!ClusterStatisticsVerification ())
					itemHit = 0;
					
				}	// end "if (gGetFileStatus == 1)" 
			break;
			
		}	// end "switch (callBackSelector)"
		
}	// end "NavServicesPutEventProc" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 NavServicesPutFile
//
//	Software purpose:	The purpose of this routine is to put up the
//							standard put file dialog box to allow the user to
//							select the file to write to.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	Error code for file operations.
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/30/2001
//	Revised By:			Larry L. Biehl			Date: 12/06/2019

SInt16 NavServicesPutFile (
				CMFileStream*		 				fileStreamPtr, 
				SInt64								numberBytes, 
				SInt16								stringIndex, 
				SInt32								creator)

{
	NavReplyRecord						theReply;
	//NavDialogOptions					dialogOptions;
	
	SInt64								bytesNeeded,
											freeBytes,
											logicalEndOfFile;
											
	CMFileStream						tempFileStream;
	
	NavEventUPP							eventUPP;
					
	//AEKeyword							keyword;
	//DescType								returnedType;
	//Size									actSize;
		
	//SInt32								itemCount;
	
	SInt16								errCode = noErr;
	Boolean								fileSelected;
		
		
			// Make certain that MultiSpec is not in the background.					
			
	PauseIfInBackground ();
	
	fileSelected = FALSE;
	
	if (!CheckIfMemoryAvailable (6000))
																								return (1);
	do	
		{
		gGetFileStatus = -1;
		if (stringIndex == IDS_SaveProjectFile)
			gGetFileStatus = 0;
	
		ReleaseSpareMemoryForWarningMessage ();
		
		GetIndString (gTextString3, kFileIOStrID, stringIndex);
		if (gTextString3[gTextString3[0]] == ':')
			gTextString3[0]--;
															
		InitializeFileStream (&tempFileStream);
			
		eventUPP = NewNavEventUPP (NavServicesPutEventProc);
		
		AEDesc								aeDesc;
		FSCatalogInfo						fsCatalogInfo;
		NavDialogCreationOptions 		dialogCreationOptions;
		NavDialogRef 						dialog;	
		NavUserData							navUserData;

				// Get the standard set of default dialog options
				
		if (errCode == noErr)
			errCode = NavGetDefaultDialogCreationOptions (&dialogCreationOptions);
		
		dialogCreationOptions.clientName = CFStringCreateWithPascalString (
																		NULL, 
																		(UCharPtr)"\0MultiSpec", 
																		kCFStringEncodingMacRoman);
															
		dialogCreationOptions.windowTitle = CFStringCreateWithPascalString (
																		NULL, 
																		(UCharPtr)gTextString3, 
																		kCFStringEncodingMacRoman);
		/*
		dialogCreationOptions.saveFileName = CFStringCreateWithPascalString (
																		NULL, 
																		fileStreamPtr->fileName, 
																		kCFStringEncodingUTF8);
		*/	
												
		dialogCreationOptions.saveFileName = CFStringCreateWithBytes (
															kCFAllocatorDefault, 
															(UInt8*)&fileStreamPtr->fileName[1],
															fileStreamPtr->fileName[0],
															kCFStringEncodingUTF8,
															false);
		/*
		dialogCreationOptions.saveFileName = CFStringCreateWithCharacters (
															kCFAllocatorDefault,
															fileStreamPtr->uniFileName.unicode,
															fileStreamPtr->uniFileName.length);
		*/
		dialogCreationOptions.preferenceKey = kPutPrefKey;
		
		dialogCreationOptions.modality = kWindowModalityAppModal;
	
				// We will do the translation ourselves later
				
		dialogCreationOptions.optionFlags |= kNavDontAutoTranslate;
		dialogCreationOptions.optionFlags |= kNavNoTypePopup;
		
				// Check if confirm alert is to be shown.
				
		if (!gConfirmReplacementAlertFlag)
			dialogCreationOptions.optionFlags |= kNavDontConfirmReplacement;
		
				// Load client information.
				
		navUserData.parentFSRef = fileStreamPtr->parentFSRef;
	
		if (errCode == noErr)
			errCode = NavCreatePutFileDialog (&dialogCreationOptions,
															fileStreamPtr->type,
															creator,
															eventUPP,
															&navUserData,
															&dialog);

				// Show the dialog box

		if (errCode == noErr)
			errCode = NavDialogRun (dialog);
		
		if (dialogCreationOptions.clientName != NULL)
			CFRelease (dialogCreationOptions.clientName);
		
		if (dialogCreationOptions.windowTitle != NULL)
			CFRelease (dialogCreationOptions.windowTitle);
		
		if (dialogCreationOptions.saveFileName != NULL)
			CFRelease (dialogCreationOptions.saveFileName);
		
				// Get the reply
				
		if (errCode == noErr)
			{
			errCode = NavDialogGetReply (dialog, &theReply);
		
			if (errCode == noErr && theReply.validRecord)
				{
				errCode = AECoerceDesc (&theReply.selection, typeFSRef, &aeDesc);
				
				if (errCode == noErr)
					{
					errCode = AEGetDescData (&aeDesc, 
														&tempFileStream.parentFSRef, 
														sizeof (FSRef));
					
					AEDisposeDesc (&aeDesc);
					
					}	// end "if (errCode == noErr)"
														
				if (errCode == noErr)
					{
					tempFileStream.fSRefFlag = TRUE;
						
					tempFileStream.uniFileName.length = 
													CFStringGetLength (theReply.saveFileName);
					CFStringGetCharacters (
										theReply.saveFileName, 
										CFRangeMake (0, tempFileStream.uniFileName.length), 
										tempFileStream.uniFileName.unicode);
														
							// Get the UTF8 version of the file name
					/*		
					CFStringGetCString (theReply.saveFileName,
												(char*)&tempFileStream.fileName[1],
												(CFIndex)255,
												kCFStringEncodingUTF8);
					tempFileStream.fileName[0] = 
												strlen ((char*)&tempFileStream.fileName[1]);
					*/								   
					if (theReply.replacing)
						errCode = FSMakeFSRefUnicode (
															&tempFileStream.parentFSRef, 
															tempFileStream.uniFileName.length, 
															tempFileStream.uniFileName.unicode, 
															kTextEncodingUnicodeDefault, 
															&tempFileStream.fsRef);
											
					if (errCode == noErr)	
						errCode = FSGetCatalogInfo (&tempFileStream.parentFSRef,
																kFSCatInfoVolume,
																&fsCatalogInfo,
																NULL,
																NULL,
																NULL);
																
					if (errCode == noErr)
						{
						fileSelected = TRUE;
						
						tempFileStream.vRefNum = fsCatalogInfo.volume;
						
						CFStringGetCString (theReply.saveFileName,
													(char*)&tempFileStream.fileName[1],
													(CFIndex)255,
													kCFStringEncodingUTF8);  
						tempFileStream.fileName[0] = 
												strlen ((char*)&tempFileStream.fileName[1]);
							
						}	// end "if (errCode == noErr)"
					
					}	// end "if (errCode == noErr)"
				
				}	// end "if (theReply.validRecord && errCode == noErr)"
	
			NavDisposeReply (&theReply);
			
			}	// end "if (errCode == noErr)"

		if (eventUPP != NULL)
			DisposeNavEventUPP (eventUPP);
		
				// Get the spare memory back again.											
			
		SetHandleSize (gSpareWarnMemoryH, (Size)gSpareWarnSize);
		
		CheckSomeEvents (activMask+updateMask);
															
		if (errCode != noErr)												
																						return (errCode);
	
		if (!fileSelected)
			{
			fileStreamPtr->fileName[0] = 0;
																								return (0);
			
			}	// end "if (!fileSelected)" 
			
				// Check if enough space on the volume.									
				
		errCode = GetVolumeFreeSpace (tempFileStream,
												tempFileStream.vRefNum, 
												&freeBytes);
															
		if (errCode != noErr)												
																						return (errCode);
		
				// Use tempFileStream structure as temporary storage for file 		
				// information.			
				
    	tempFileStream.refNum = 0;
    	tempFileStream.type = fileStreamPtr->type;
		
				// If the project file is being saved, the number of bytes for		
				// the file needs to be calculated here since the user can make	
				// changes in the PutFile dialog that affects the size of the		
				// save project file.															
		
		if (stringIndex == IDS_SaveProjectFile)
			numberBytes = (SInt64)GetSizeOfProjectFile ();
		
				// Check if there is enough space on the disk.  If there is not	
				// enough space, even when considering whether an old file is to	
				// be replaced then allow the user to select the file/disk again.	
		
		bytesNeeded = numberBytes + 100 - freeBytes;
		if (bytesNeeded > 0)
			{
					// Get the number of bytes in the old file if one is being		
					// replaced.																	
			
			errCode = GetSizeOfFile (&tempFileStream, &logicalEndOfFile);
			if (errCode != noErr && errCode != fnfErr)
																						return (errCode);
				
			bytesNeeded = bytesNeeded - logicalEndOfFile;
			
			if (bytesNeeded > 0)
				{
				DiskFullAlert (bytesNeeded, (Str255*)tempFileStream.fileName);
				fileSelected = FALSE;
				
				}	// end "if (bytesNeeded > 0)"
			
			}	// end "if (bytesNeeded > 0)" 
			
				// Make certain that we can write to the file.							
				
		if (fileSelected)
			{
			if (tempFileStream.fSRefFlag)
				{
				/*
						// this is going to take some more research to implement.
				FSSetCatalogInfo (&fileStreamPtr->fsRef, 
											kFSCatInfoPermissions, 
											const FSCatalogInfo *catalogInfo)
				*/
				}	// end "tempFileStream.fSRefFlag"
				
			else	// !tempFileStream.fSRefFlag
				{
				errCode = FSMakeFSSpec (tempFileStream.vRefNum,
												tempFileStream.parID,
												tempFileStream.fileName,
												&gFileFSSpec);
					
				if (errCode == fnfErr ||  
						errCode == afpAccessDenied)
					errCode = noErr;
					
				if (errCode == noErr)	
					{
					tempFileStream.vRefNum = gFileFSSpec.vRefNum;
					tempFileStream.parID = gFileFSSpec.parID;
	    			tempFileStream.fSSpecFlag = TRUE;
				
					errCode = HRstFLock (tempFileStream.vRefNum, 
												tempFileStream.parID,
												tempFileStream.fileName);
					
					if (errCode == fnfErr ||  
							errCode == afpAccessDenied)
						errCode = noErr;
																					
					}	// end "if (errCode == noErr)"
				
				}	// end "else !tempFileStream.fSRefFlag"
						
			if (errCode != noErr)
				{
				IOCheck (errCode, (CharPtr)&tempFileStream.fileName);
				fileSelected = FALSE;
				
				}	// end "if (errCode != noErr)" 
				
			}	// end "if (fileSelected)" 
		
		}	while (!fileSelected);
		
			// Create the file on the selected volume										
			
	errCode = CreateNewFile (&tempFileStream, 
										tempFileStream.vRefNum, 
										creator, 
										kErrorMessages, 
										kReplaceFlag);
				
	if (errCode == noErr)
		{
				// Now copy the file parameters to the supplied structure.		
				
    	fileStreamPtr->vRefNum = tempFileStream.vRefNum;
    	fileStreamPtr->parID = tempFileStream.parID;
		CopyPToP (fileStreamPtr->fileName, tempFileStream.fileName);
    	fileStreamPtr->type = tempFileStream.type;
    	fileStreamPtr->refNum = tempFileStream.refNum;
    	fileStreamPtr->fSSpecFlag = tempFileStream.fSSpecFlag;    	
    	
    	fileStreamPtr->fsRef = tempFileStream.fsRef;
    	fileStreamPtr->parentFSRef = tempFileStream.parentFSRef;
    	fileStreamPtr->uniFileName = tempFileStream.uniFileName;
    	fileStreamPtr->fSRefFlag = tempFileStream.fSRefFlag;
		
				// Save the basic information for the last file in case it is 
				// needed later for special work flow (batch) versions of MultiSpec
				
		gMultiSpecWorkflowInfo.lastFileSavedInfo.vRefNum = tempFileStream.vRefNum;
		gMultiSpecWorkflowInfo.lastFileSavedInfo.parID = tempFileStream.parID;
		CopyPToP (gMultiSpecWorkflowInfo.lastFileSavedInfo.fileName, 
						tempFileStream.fileName);
		gMultiSpecWorkflowInfo.lastFileSavedInfo.type = tempFileStream.type;
		gMultiSpecWorkflowInfo.lastFileSavedInfo.refNum = 0;
		gMultiSpecWorkflowInfo.lastFileSavedInfo.fSSpecFlag = tempFileStream.fSSpecFlag;
		gMultiSpecWorkflowInfo.lastFileSavedInfo.fNamePtr = 
											gMultiSpecWorkflowInfo.lastFileSavedInfo.fileName;
		
		gMultiSpecWorkflowInfo.lastFileSavedInfo.fsRef = tempFileStream.fsRef;
		gMultiSpecWorkflowInfo.lastFileSavedInfo.fSRefFlag = tempFileStream.fSRefFlag;
		
		}	// end "if (errCode == noErr)"
		
	else	// errCode != noErr
		gMultiSpecWorkflowInfo.lastFileSavedInfo.fileName[0] = 0;
		
	return (errCode);

}	// end "NavServicesPutFile"  
#endif	// defined multispec_mac 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		OpenFileReadOnly
//
//	Software purpose:	This routine opens the given file with read only permission.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			GetDefaultERDASsupportFile in SFileIO.cpp
//							GetFile in SFileIO.cpp
//							ReadOneBytePalette in SPalette.cpp
//							GetSpecifiedImageFile in SProject.cpp	
//							LoadProjectFileAndLayerInformation in SProject.cpp
//							OpenProjectImageWindow in SProject.cpp
//							InsertNewErdasHeader in SReformatUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/19/1991
//	Revised By:			Larry L. Biehl			Date: 09/05/2017

SInt16 OpenFileReadOnly (
				CMFileStream*						fileStreamPtr, 
				Boolean								resolveAliasFlag,
				Boolean								lockFileFlag,
				Boolean								verifyFileStreamFlag)

{
	SInt16								errCode = noErr;
	
	
	#if defined multispec_mac
		HFSUniStr255 						dataForkName;
	
		if (fileStreamPtr->refNum == 0)
			{						
			Boolean	wasAliasedFlag = FALSE;
			
			if (gNumberOfOpenFiles >= gNumberOfOpenFilesLimit)
				errCode = afpTooManyFilesOpen;
			
			if (errCode == noErr && verifyFileStreamFlag)
				{
						// Force the unicode name to be regenerated to match the
						// UTF8 char file name.
		
				SetFileDoesNotExist (fileStreamPtr, kKeepUTF8CharName);
				errCode = VerifyFileStreamForOpen (fileStreamPtr, NULL);
				
				}	// end "if (errCode == noErr && verifyFileStreamFlag)"
			
					// First resolve any aliases if requested.									
					
			if (errCode == noErr && resolveAliasFlag)
				errCode = ResolveAnyAliases (fileStreamPtr, &wasAliasedFlag);
				
					// Set to read only permission and reopen		
					
			lockFileFlag = FALSE;	
				
			if (errCode == noErr && lockFileFlag)
				{
				if (fileStreamPtr->fSSpecFlag)
					errCode = FSpSetFLock ((FSSpec*)fileStreamPtr);
				
				else if (fileStreamPtr->fSRefFlag)
					/*
							// Do nothing here for now.  Don't understand it.
							// Should not get here anyway.
					FSSetCatalogInfo (&fileStreamPtr->fsRef, 
												kFSCatInfoPermissions, 
												const FSCatalogInfo *catalogInfo)
					*/
							// The following line was just put here for a debugging
							// stop to see if one would get here.
					fileStreamPtr->fSRefFlag = TRUE;
									
				if (errCode == wPrErr || 
										errCode == fLckdErr || 
											errCode == afpAccessDenied ||
												errCode == permErr)
					errCode = noErr;
					
				else	// errCode != wPrErr && errCode != fLckdErr && ... 
					if (errCode == noErr)
						errCode = FlushVol (NIL, fileStreamPtr->vRefNum);
						
				}	// end "if (errCode == noErr && lockFileFlag)" 
					
			if (errCode == noErr)
				{
				if (fileStreamPtr->fSSpecFlag)
					errCode = FSpOpenDF ((FSSpec*)fileStreamPtr, 
													fsRdPerm, 
													&fileStreamPtr->refNum);
													
				else if (fileStreamPtr->fSRefFlag)
					{
					errCode = FSGetDataForkName (&dataForkName);
					
					if (errCode == noErr)
						errCode = FSOpenFork (&fileStreamPtr->fsRef, 
														dataForkName.length, 
														dataForkName.unicode, 
														fsRdPerm, 
														&fileStreamPtr->refNum);
					
					}	// end "else if (fileStreamPtr->fSRefFlag)"
																	
				else	// !fileInfoPtr->fSSpecFlag || !fileStreamPtr->fSRefFlag
					errCode = HOpen (fileStreamPtr->vRefNum, 
											0, 
											fileStreamPtr->fileName, 
											fsRdPerm, 
											&fileStreamPtr->refNum);
																	
				}	// end "if (errCode == noErr)" 
											
			if (errCode == noErr)
				gNumberOfOpenFiles++;
				
			}	// end "if (fileStreamPtr->refNum == 0)"
	#endif	// defined multispec_mac
	
	#if defined multispec_wx
		if (verifyFileStreamFlag)
				// Force the unicode name to be regenerated to match the
				// UTF8 char file name.
			SetFileDoesNotExist (fileStreamPtr, kKeepUTF8CharName);
		UpdateFileNameInformation (fileStreamPtr, NULL);
		errCode = fileStreamPtr->MOpenFile ((UInt16)wxFile::read, 
														(UInt16)kNoErrorMessages);
	#endif
	
	#if defined multispec_win
		if (verifyFileStreamFlag)
				// Force the unicode name to be regenerated to match the
				// UTF8 char file name.
			SetFileDoesNotExist (fileStreamPtr, kKeepUTF8CharName);
		UpdateFileNameInformation (fileStreamPtr, NULL);
		errCode = fileStreamPtr->MOpenFile ((UInt16)kRead,
														(UInt16)kNoErrorMessages);
	#endif	// defined multispec_win

	return (errCode);
	
}	// end "OpenFileReadOnly"           



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void PackBISData
//
//	Software purpose:	The purpose of this routine is to pack the line
//							of data for BIS formated input files if the column
//							interval is more than 1 and/or the number of
//							reqested channels is less than the number of 
//							available channels. The output will be either BIS formatted
//							line of data our BSQ/BIL formatted line of data
//
//	Parameters in:				
//
//	Parameters out:	
//
// Value Returned:
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 09/18/1989
//	Revised By:			Larry L. Biehl			Date: 05/09/2022

void PackBISData (
				LayerInfoPtr						layerInfoPtr,
				UInt32								columnInterval,
				UInt32								numberOutputBufferChannels,
				UInt32								numberBufferChannels, 
				UInt32								numberChannels,
				UInt16*								channelListPtr,
				UInt32								numberSamples, 
				UInt32								dataConversionCode, 
				HUCharPtr 							ioCharBufferPtr,
				HUCharPtr 							dataCharBufferPtr,
				Boolean								toBISFormatFlag)

{
	SInt32								skipSamples;
	
	UInt32								channel,
											index,
											sample,
											toBufferSkip;
			
	
	skipSamples = columnInterval * numberBufferChannels;
	
	if (toBISFormatFlag)
		toBufferSkip = 1;
		 
	else // !toBISFormatFlag
		{
				// The number of channels for the channelList for the loop is the
				// minimum of the numberOutputBufferChannels and numberChannels.
				// This is to handle case when the reformat processor is handling one
				// channel at a time.
		numberChannels = MIN (numberChannels, numberOutputBufferChannels);
			
		toBufferSkip = (numberSamples-1)/columnInterval + 1;
		
		}	// end "else !toBISFormatFlag"
	
			// Note that the brackets for each of the cases was included to provide
			// scope for the variables within each case.
			
	switch (dataConversionCode)
		{
		case k8BitTo8Bit: 
			{
			HUCharPtr	 			fromCharPtr;
			HUCharPtr 				toCharPtr;
			HUCharPtr 				savedToCharPtr;
			
			toCharPtr = dataCharBufferPtr;
			fromCharPtr = ioCharBufferPtr;
			
			savedToCharPtr = toCharPtr;
			
			for (sample=0;  sample<numberSamples; sample+=columnInterval)
				{
				index = 0;
				channel = 0;
				toCharPtr = savedToCharPtr;
				while (index < numberChannels)
					{		
					if (channelListPtr)
						//channel = channelListPtr[index];
						channel = layerInfoPtr[channelListPtr[index]+1].fileChannelNumber-1;
						
					*toCharPtr = fromCharPtr[channel];
					toCharPtr += toBufferSkip;
					index++;
					channel++;
					
					}	// end "while (index < numberChannels)" 
					
				fromCharPtr += skipSamples;
						
				if (toBISFormatFlag)
					savedToCharPtr += numberOutputBufferChannels;
					
				else	// !toBISFormatFlag
					savedToCharPtr++;
				
				}	// end "for (sample=0; sample<numberSamples; ..." 
				
			}
			break;
			
		case k8BitIntSignedTo16BitIntSigned: 
			{
			HSInt8Ptr 				fromCharPtr;
			HSInt16Ptr 				toIntPtr;
			HSInt16Ptr 				savedToIntPtr;
			
			toIntPtr = (HSInt16Ptr)dataCharBufferPtr;
			fromCharPtr = (HSInt8Ptr)ioCharBufferPtr;
			
			savedToIntPtr = toIntPtr;
			
			for (sample=0;  sample<numberSamples; sample+=columnInterval)
				{
				index = 0;
				channel = 0;
				toIntPtr = savedToIntPtr;
				while (index < numberChannels)
					{
					if (channelListPtr)
						//channel = channelListPtr[index];
						channel = layerInfoPtr[channelListPtr[index]+1].fileChannelNumber-1;
						
					*toIntPtr = fromCharPtr[channel];
					toIntPtr += toBufferSkip;
					index++;
					channel++;
					
					}	// end "while (index < numberChannels)" 
					
				fromCharPtr += skipSamples;
						
				if (toBISFormatFlag)
					savedToIntPtr += numberOutputBufferChannels;
					
				else	// !toBISFormatFlag
					savedToIntPtr++;
				
				}	// end "for (sample=0; sample<numberSamples; ..." 
				
			}
			break;
			
		case k8BitIntUnsignedTo16BitIntUnsigned: 
		case k8BitIntUnsignedTo16BitIntSigned:
			{
			HUInt8Ptr 				fromCharPtr;
			HUInt16Ptr 				toInt16Ptr;
			HUInt16Ptr 				savedToInt16Ptr;
			
			toInt16Ptr = (HUInt16Ptr)dataCharBufferPtr;
			fromCharPtr = ioCharBufferPtr;
			
			savedToInt16Ptr = toInt16Ptr;
			
			for (sample=0;  sample<numberSamples; sample+=columnInterval)
				{
				index = 0;
				channel = 0;
				toInt16Ptr = savedToInt16Ptr;
				while (index < numberChannels)
					{
					if (channelListPtr)
						//channel = channelListPtr[index];
						channel = layerInfoPtr[channelListPtr[index]+1].fileChannelNumber-1;
						
					*toInt16Ptr = fromCharPtr[channel];
					toInt16Ptr += toBufferSkip;
					index++;
					channel++;
					
					}	// end "while (index < numberChannels)" 
					
				fromCharPtr += skipSamples;
						
				if (toBISFormatFlag)
					savedToInt16Ptr += numberOutputBufferChannels;
					
				else	// !toBISFormatFlag
					savedToInt16Ptr++;
				
				}	// end "for (sample=0; sample<numberSamples; ..." 
				
			} 
			break;
			
		case k8BitIntSignedTo32BitIntSigned: 
			{
			HSInt8Ptr 				fromCharPtr;
			HSInt32Ptr 				toSInt32Ptr;
			HSInt32Ptr 				savedToSInt32Ptr;
			
			toSInt32Ptr = (HSInt32Ptr)dataCharBufferPtr;
			fromCharPtr = (HSInt8Ptr)ioCharBufferPtr;
			
			savedToSInt32Ptr = toSInt32Ptr;
			
			for (sample=0;  sample<numberSamples; sample+=columnInterval)
				{
				index = 0;
				channel = 0;
				toSInt32Ptr = savedToSInt32Ptr;
				while (index < numberChannels)
					{
					if (channelListPtr)
						//channel = channelListPtr[index];
						channel = layerInfoPtr[channelListPtr[index]+1].fileChannelNumber-1;
						
					*toSInt32Ptr = fromCharPtr[channel];
					toSInt32Ptr += toBufferSkip;
					index++;
					channel++;
					
					}	// end "while (index < numberChannels)" 
					
				fromCharPtr += skipSamples;
						
				if (toBISFormatFlag)
					savedToSInt32Ptr += numberOutputBufferChannels;
					
				else	// !toBISFormatFlag
					savedToSInt32Ptr++;
				
				}	// end "for (sample=0; sample<numberSamples; ..."
				
			}
			break;
							
		case k8BitIntUnsignedTo32BitIntUnsigned: 
		case k8BitIntUnsignedTo32BitIntSigned:
			{
			HUCharPtr 				fromCharPtr;
			HSInt32Ptr 				toInt32Ptr;
			HSInt32Ptr 				savedToInt32Ptr;
			
			fromCharPtr = ioCharBufferPtr;
			toInt32Ptr = (HSInt32Ptr)dataCharBufferPtr;
			
			savedToInt32Ptr = toInt32Ptr;
			
			for (sample=0;  sample<numberSamples; sample+=columnInterval)
				{
				index = 0;
				channel = 0;
				toInt32Ptr = savedToInt32Ptr;
				while (index < numberChannels)
					{
					if (channelListPtr)
						//channel = channelListPtr[index];
						channel = layerInfoPtr[channelListPtr[index]+1].fileChannelNumber-1;
						
					*toInt32Ptr = fromCharPtr[channel];
					toInt32Ptr += toBufferSkip;
					index++;
					channel++;
					
					}	// end "while (index < numberChannels)" 
					
				fromCharPtr += skipSamples;
						
				if (toBISFormatFlag)
					savedToInt32Ptr += numberOutputBufferChannels;
					
				else	// !toBISFormatFlag
					savedToInt32Ptr++;
				
				}	// end "for (sample=0; sample<numberSamples; ..."
				
			}
			break;
							
		case k8BitIntSignedTo32BitReal: 
			{
			HSInt8Ptr 				fromCharPtr;
			HFloatPtr 				toFloatPtr;
			HFloatPtr 				savedToFloatPtr;
			
			fromCharPtr = (HSInt8Ptr)ioCharBufferPtr;
			toFloatPtr = (HFloatPtr)dataCharBufferPtr;
			
			savedToFloatPtr = toFloatPtr;
			
			for (sample=0;  sample<numberSamples; sample+=columnInterval)
				{
				index = 0;
				channel = 0;
				toFloatPtr = savedToFloatPtr;
				while (index < numberChannels)
					{
					if (channelListPtr)
						//channel = channelListPtr[index];
						channel = layerInfoPtr[channelListPtr[index]+1].fileChannelNumber-1;
						
					*toFloatPtr = fromCharPtr[channel];
					toFloatPtr += toBufferSkip;
					index++;
					channel++;
					
					}	// end "for (index=0; index<numberColumns;..." 
					
				fromCharPtr += skipSamples;
						
				if (toBISFormatFlag)
					savedToFloatPtr += numberOutputBufferChannels;
					
				else	// !toBISFormatFlag
					savedToFloatPtr++;
				
				}	// end "for (sample=0; sample<numberSamples; ..."
				
			}
			break;
							
		case k8BitIntUnsignedTo32BitReal: 
			{
			HUInt8Ptr 				fromCharPtr;
			HFloatPtr 				toFloatPtr;
			HFloatPtr 				savedToFloatPtr;
			
			fromCharPtr = (HUInt8Ptr)ioCharBufferPtr;
			toFloatPtr = (HFloatPtr)dataCharBufferPtr;
			
			savedToFloatPtr = toFloatPtr;
			
			for (sample=0;  sample<numberSamples; sample+=columnInterval)
				{
				index = 0;
				channel = 0;
				toFloatPtr = savedToFloatPtr;
				while (index < numberChannels)
					{
					if (channelListPtr)
						//channel = channelListPtr[index];
						channel = layerInfoPtr[channelListPtr[index]+1].fileChannelNumber-1;
						
					*toFloatPtr = fromCharPtr[channel];
					toFloatPtr += toBufferSkip;
					index++;
					channel++;
					
					}	// end "while (index < numberChannels)" 
					
				fromCharPtr += skipSamples;
						
				if (toBISFormatFlag)
					savedToFloatPtr += numberOutputBufferChannels;
					
				else	// !toBISFormatFlag
					savedToFloatPtr++;
				
				}	// end "for (sample=0; sample<numberSamples; ..."
				
			}
			break;
							
		case k8BitIntSignedTo64BitReal: 
			{
			HSInt8Ptr 				fromCharPtr;
			HDoublePtr 				toDoublePtr;
			HDoublePtr 				savedToDoublePtr;
			
			toDoublePtr = (HDoublePtr)dataCharBufferPtr;
			fromCharPtr = (HSInt8Ptr)ioCharBufferPtr;
			
			savedToDoublePtr = toDoublePtr;
			
			for (sample=0;  sample<numberSamples; sample+=columnInterval)
				{
				index = 0;
				channel = 0;
				toDoublePtr = savedToDoublePtr;
				while (index < numberChannels)
					{
					if (channelListPtr)
						//channel = channelListPtr[index];
						channel = layerInfoPtr[channelListPtr[index]+1].fileChannelNumber-1;
						
					*toDoublePtr = fromCharPtr[channel];
					toDoublePtr += toBufferSkip;
					index++;
					channel++;
					
					}	// end "for (index=0; index<numberColumns;..." 
					
				fromCharPtr += skipSamples;
						
				if (toBISFormatFlag)
					savedToDoublePtr += numberOutputBufferChannels;
					
				else	// !toBISFormatFlag
					savedToDoublePtr++;
				
				}	// end "for (sample=0; sample<numberSamples; ..."
				
			}
			break;
							
		case k8BitIntUnsignedTo64BitReal: 
			{
			HUInt8Ptr 				fromCharPtr;
			HDoublePtr 				toDoublePtr;
			HDoublePtr 				savedToDoublePtr;
			
			fromCharPtr = (HUInt8Ptr)ioCharBufferPtr;
			toDoublePtr = (HDoublePtr)dataCharBufferPtr;
			
			savedToDoublePtr = toDoublePtr;
			
			for (sample=0;  sample<numberSamples; sample+=columnInterval)
				{
				index = 0;
				channel = 0;
				toDoublePtr = savedToDoublePtr;
				while (index < numberChannels)
					{
					if (channelListPtr)
						//channel = channelListPtr[index];
						channel = layerInfoPtr[channelListPtr[index]+1].fileChannelNumber-1;
						
					*toDoublePtr = fromCharPtr[channel];
					toDoublePtr += toBufferSkip;
					index++;
					channel++;
					
					}	// end "while (index < numberChannels)" 
					
				fromCharPtr += skipSamples;
						
				if (toBISFormatFlag)
					savedToDoublePtr += numberOutputBufferChannels;
					
				else	// !toBISFormatFlag
					savedToDoublePtr++;
				
				}	// end "for (sample=0; sample<numberSamples; ..."
				
			}
			break;
							
		case k16BitTo16Bit: 
			{
			HSInt16Ptr 				fromInt16Ptr;
			HSInt16Ptr 				toInt16Ptr;
			HSInt16Ptr 				savedToInt16Ptr;
			
			
			toInt16Ptr = (HSInt16Ptr)dataCharBufferPtr;
			fromInt16Ptr = (HSInt16Ptr)ioCharBufferPtr;
			
			savedToInt16Ptr = toInt16Ptr;
			/*
			if (toBISFormatFlag)
				{
				for (sample=0; sample<numberSamples; sample+=columnInterval)
					{
					index = 0;
					channel = 0;
					toIntPtr = savedToIntPtr;
					while (index < numberChannels)
						{
						if (channelListPtr)
							channel = channelListPtr[index];
								
						*toIntPtr = fromIntPtr[channel];
						toIntPtr++;
						index++;
						channel++;
						
						}	// end "while (index < numberBufferChannels)"
							
					savedToIntPtr += numberOutputBufferChannels;
						
					fromIntPtr += skipSamples;
					
					}	// end "for (sample=0; sample<numberSamples; ..."
				
				}	// end "if (toBISFormatFlag)"
				
			else	// !toBISFormatFlag
				{
				HSInt16Ptr 				savedFromIntPtr;
				
				savedFromIntPtr = fromIntPtr;
				skipSamples = columnInterval * numberBufferChannels;
				index = 0;
				channel = 0;
				while (index < numberChannels)
					{
					if (channelListPtr)
						channel = channelListPtr[index];
							
					fromIntPtr = savedFromIntPtr + channel;
					for (sample=0; sample<numberSamples; sample+=columnInterval)
						{
						*toIntPtr = *fromIntPtr;
						fromIntPtr += skipSamples;
						toIntPtr++;
						
						}	// end "for (sample=0; sample<numberSamples; ..."
						
					index++;
					channel++;
					
					}	// end "while (index < numberBufferChannels)"
				
				}	// end "else !toBISFormatFlag"
			*/
			for (sample=0; sample<numberSamples; sample+=columnInterval)
				{
				index = 0;
				channel = 0;
				toInt16Ptr = savedToInt16Ptr;
				while (index < numberChannels)
					{
					if (channelListPtr)
						//channel = channelListPtr[index];
						channel = layerInfoPtr[channelListPtr[index]+1].fileChannelNumber-1;
							
					*toInt16Ptr = fromInt16Ptr[channel];
					toInt16Ptr += toBufferSkip;
					index++;
					channel++;
					
					}	// end "while (index < numberBufferChannels)"
					
				fromInt16Ptr += skipSamples;
						
				if (toBISFormatFlag)
					savedToInt16Ptr += numberOutputBufferChannels;
					
				else	// !toBISFormatFlag
					savedToInt16Ptr++;
				
				}	// end "for (sample=0; sample<numberSamples; ..."
				
			}
			break;
							
		case k16BitIntSignedTo32BitIntSigned: 
			{
			HSInt16Ptr 				fromSInt16Ptr;
			HSInt32Ptr 				toSInt32Ptr;
			HSInt32Ptr 				savedToSInt32Ptr;
			
			fromSInt16Ptr = (HSInt16Ptr)ioCharBufferPtr;
			toSInt32Ptr = (HSInt32Ptr)dataCharBufferPtr;
			
			savedToSInt32Ptr = toSInt32Ptr;
			
			for (sample=0;  sample<numberSamples; sample+=columnInterval)
				{
				index = 0;
				channel = 0;
				toSInt32Ptr = savedToSInt32Ptr;
				while (index < numberChannels)
					{
					if (channelListPtr)
						//channel = channelListPtr[index];
						channel = layerInfoPtr[channelListPtr[index]+1].fileChannelNumber-1;
						
					*toSInt32Ptr = fromSInt16Ptr[channel];
					toSInt32Ptr += toBufferSkip;
					index++;
					channel++;
					
					}	// end "while (index < numberChannels)" 
					
				fromSInt16Ptr += skipSamples;
						
				if (toBISFormatFlag)
					savedToSInt32Ptr += numberOutputBufferChannels;
					
				else	// !toBISFormatFlag
					savedToSInt32Ptr++;
				
				}	// end "for (sample=0; sample<numberSamples; ..."
				
			} 
			break;
							
		case k16BitIntUnsignedTo32BitIntUnsigned:
		case k16BitIntUnsignedTo32BitIntSigned: 
			{
			HUInt16Ptr 				fromUInt16Ptr;
			HUInt32Ptr 				toInt32Ptr;
			HUInt32Ptr 				savedToInt32Ptr;
			
			fromUInt16Ptr = (HUInt16Ptr)ioCharBufferPtr;
			toInt32Ptr = (HUInt32Ptr)dataCharBufferPtr;
			
			savedToInt32Ptr = toInt32Ptr;
			
			for (sample=0;  sample<numberSamples; sample+=columnInterval)
				{
				index = 0;
				channel = 0;
				toInt32Ptr = savedToInt32Ptr;
				while (index < numberChannels)
					{
					if (channelListPtr)
						//channel = channelListPtr[index];
						channel = layerInfoPtr[channelListPtr[index]+1].fileChannelNumber-1;
						
					*toInt32Ptr = fromUInt16Ptr[channel];
					toInt32Ptr += toBufferSkip;
					index++;
					channel++;
					
					}	// end "while (index < numberChannels)" 
					
				fromUInt16Ptr += skipSamples;
						
				if (toBISFormatFlag)
					savedToInt32Ptr += numberOutputBufferChannels;
					
				else	// !toBISFormatFlag
					savedToInt32Ptr++;
				
				}	// end "for (sample=0;  sample<numberSamples; ..."
				
			}   
			break;
							
		case k16BitIntSignedTo32BitReal: 
			{
			HSInt16Ptr 				fromSInt16Ptr;
			HFloatPtr 				toFloatPtr;
			HFloatPtr 				savedToFloatPtr;
			
			fromSInt16Ptr = (HSInt16Ptr)ioCharBufferPtr;
			toFloatPtr = (HFloatPtr)dataCharBufferPtr;
			
			savedToFloatPtr = toFloatPtr;
			
			for (sample=0;  sample<numberSamples; sample+=columnInterval)
				{
				index = 0;
				channel = 0;
				toFloatPtr = savedToFloatPtr;
				while (index < numberChannels)
					{
					if (channelListPtr)
						//channel = channelListPtr[index];
						channel = layerInfoPtr[channelListPtr[index]+1].fileChannelNumber-1;
						
					*toFloatPtr = fromSInt16Ptr[channel];
					toFloatPtr += toBufferSkip;
					index++;
					channel++;
					
					}	// end "while (index < numberChannels)" 
					
				fromSInt16Ptr += skipSamples;
						
				if (toBISFormatFlag)
					savedToFloatPtr += numberOutputBufferChannels;
					
				else	// !toBISFormatFlag
					savedToFloatPtr++;
				
				}	// end "for (sample=0;  sample<numberSamples; ..."
				
			}
			break;
							
		case k16BitIntUnsignedTo32BitReal: 
			{
			HUInt16Ptr 				fromCharPtr;
			HFloatPtr 				toFloatPtr;
			HFloatPtr 				savedToFloatPtr;
			
			toFloatPtr = (HFloatPtr)dataCharBufferPtr;
			fromCharPtr = (HUInt16Ptr)ioCharBufferPtr;
			
			savedToFloatPtr = toFloatPtr;
			
			for (sample=0;  sample<numberSamples; sample+=columnInterval)
				{
				index = 0;
				channel = 0;
				toFloatPtr = savedToFloatPtr;
				while (index < numberChannels)
					{
					if (channelListPtr)
						//channel = channelListPtr[index];
						channel = layerInfoPtr[channelListPtr[index]+1].fileChannelNumber-1;
						
					*toFloatPtr = fromCharPtr[channel];
					toFloatPtr += toBufferSkip;
					index++;
					channel++;
					
					}	// end "while (index < numberChannels)" 
					
				fromCharPtr += skipSamples;
						
				if (toBISFormatFlag)
					savedToFloatPtr += numberOutputBufferChannels;
					
				else	// !toBISFormatFlag
					savedToFloatPtr++;
				
				}	// end "for (sample=0; sample<numberSamples; ..."
				
			} 
			break;
							
		case k16BitIntSignedTo64BitReal: 
			{
			HSInt16Ptr 				fromSInt16Ptr;
			HDoublePtr 				toDoublePtr;
			HDoublePtr 				savedToDoublePtr;
			
			toDoublePtr = (HDoublePtr)dataCharBufferPtr;
			fromSInt16Ptr = (HSInt16Ptr)ioCharBufferPtr;
			
			savedToDoublePtr = toDoublePtr;
			
			for (sample=0;  sample<numberSamples; sample+=columnInterval)
				{
				index = 0;
				channel = 0;
				toDoublePtr = savedToDoublePtr;
				while (index < numberChannels)
					{
					if (channelListPtr)
						//channel = channelListPtr[index];
						channel = layerInfoPtr[channelListPtr[index]+1].fileChannelNumber-1;
						
					*toDoublePtr = fromSInt16Ptr[channel];
					toDoublePtr += toBufferSkip;
					index++;
					channel++;
					
					}	// end "while (index < numberChannels)" 
					
				fromSInt16Ptr += skipSamples;
						
				if (toBISFormatFlag)
					savedToDoublePtr += numberOutputBufferChannels;
					
				else	// !toBISFormatFlag
					savedToDoublePtr++;
				
				}	// end "for (sample=0; sample<numberSamples; ..."
				
			}
			break;
							
		case k16BitIntUnsignedTo64BitReal: 
			{
			HUInt16Ptr 				fromUInt16Ptr;
			HDoublePtr 				toDoublePtr;
			HDoublePtr 				savedToDoublePtr;
			
			toDoublePtr = (HDoublePtr)dataCharBufferPtr;
			fromUInt16Ptr = (HUInt16Ptr)ioCharBufferPtr;
			
			savedToDoublePtr = toDoublePtr;
			
			for (sample=0;  sample<numberSamples; sample+=columnInterval)
				{
				index = 0;
				channel = 0;
				toDoublePtr = savedToDoublePtr;
				while (index < numberChannels)
					{
					if (channelListPtr)
						//channel = channelListPtr[index];
						channel = layerInfoPtr[channelListPtr[index]+1].fileChannelNumber-1;
						
					*toDoublePtr = fromUInt16Ptr[channel];
					toDoublePtr += toBufferSkip;
					index++;
					channel++;
					
					}	// end "while (index < numberChannels)" 
					
				fromUInt16Ptr += skipSamples;
						
				if (toBISFormatFlag)
					savedToDoublePtr += numberOutputBufferChannels;
					
				else	// !toBISFormatFlag
					savedToDoublePtr++;
				
				}	// end "for (sample=0; sample<numberSamples; ..."
				
			} 
			break;
							
		case k32BitTo32Bit: 
			{
			HSInt32Ptr 				fromInt32Ptr;
			HSInt32Ptr 				toInt32Ptr;
			HSInt32Ptr 				savedToInt32Ptr;
			
			fromInt32Ptr = (HSInt32Ptr)ioCharBufferPtr;
			toInt32Ptr = (HSInt32Ptr)dataCharBufferPtr;
			
			savedToInt32Ptr = toInt32Ptr;
			/*
			if (toBISFormatFlag)
				{
				HSInt32Ptr 				savedToIntPtr;
				
				savedToIntPtr = toIntPtr;
				for (sample=0; sample<numberSamples; sample+=columnInterval)
					{
					index = 0;
					channel = 0;
					toIntPtr = savedToIntPtr;
					while (index < numberChannels)
						{
						if (channelListPtr)
							{
							//channel = channelListPtr[index];
							channel = layerInfoPtr[channelListPtr[index]+1].fileChannelNumber-1;
							
							}	// end "if (channelListPtr)"
								
						*toIntPtr = fromIntPtr[channel];
						toIntPtr++;
						index++;
						channel++;
						
						}	// end "while (index < numberBufferChannels)"
							
					savedToIntPtr += numberOutputBufferChannels;
						
					fromIntPtr += skipSamples;
					
					}	// end "for (sample=0; sample<numberSamples; ..."
				
				}	// end "if (toBISFormatFlag)"
			
			else	// !toBISFormatFlag
				{
				HSInt32Ptr 				savedFromIntPtr;
				
				savedFromIntPtr = fromIntPtr;
				index = 0;
				channel = 0;
				while (index < numberChannels)
					{
					if (channelListPtr)
						//channel = channelListPtr[index];
						channel = layerInfoPtr[channelListPtr[index]+1].fileChannelNumber-1;
							
					fromIntPtr = savedFromIntPtr + channel;
					for (sample=0; sample<numberSamples; sample+=columnInterval)
						{
						*toIntPtr = *fromIntPtr;
						fromIntPtr += skipSamples;
						toIntPtr++;
						
						}	// end "for (sample=0; sample<numberSamples; ..."
						
					index++;
					channel++;
					
					}	// end "while (index < fileNumberChannels)"
				
				}	// end "else !toBISFormatFlag"
			*/
			for (sample=0;  sample<numberSamples; sample+=columnInterval)
				{
				index = 0;
				channel = 0;
				toInt32Ptr = savedToInt32Ptr;
				while (index < numberChannels)
					{
					if (channelListPtr)
						//channel = channelListPtr[index];
						channel = layerInfoPtr[channelListPtr[index]+1].fileChannelNumber-1;
						
					*toInt32Ptr = fromInt32Ptr[channel];
					toInt32Ptr += toBufferSkip;
					index++;
					channel++;
					
					}	// end "while (index < numberChannels)"
					
				fromInt32Ptr += skipSamples;
						
				if (toBISFormatFlag)
					savedToInt32Ptr += numberOutputBufferChannels;
					
				else	// !toBISFormatFlag
					savedToInt32Ptr++;
				
				}	// end "for (sample=0; sample<numberSamples; ..."
			}
			break;
							
		case k32BitIntSignedTo32BitReal:
			{
			HSInt32Ptr 				fromSInt32Ptr;
			HFloatPtr 				toFloatPtr;
			HFloatPtr 				savedToFloatPtr;
			
			fromSInt32Ptr = (HSInt32Ptr)ioCharBufferPtr;
			toFloatPtr = (HFloatPtr)dataCharBufferPtr;
			
			savedToFloatPtr = toFloatPtr;
			
			for (sample=0;  sample<numberSamples; sample+=columnInterval)
				{
				index = 0;
				channel = 0;
				toFloatPtr = savedToFloatPtr;
				while (index < numberChannels)
					{
					if (channelListPtr)
						//channel = channelListPtr[index];
						channel = layerInfoPtr[channelListPtr[index]+1].fileChannelNumber-1;
						
					*toFloatPtr = (float)fromSInt32Ptr[channel];
					toFloatPtr += toBufferSkip;
					index++;
					channel++;
					
					}	// end "while (index < numberChannels)" 
					
				fromSInt32Ptr += skipSamples;
						
				if (toBISFormatFlag)
					savedToFloatPtr += numberOutputBufferChannels;
					
				else	// !toBISFormatFlag
					savedToFloatPtr++;
				
				}	// end "for (sample=0; sample<numberSamples; ..."
				
			}
			break;
							
		case k32BitIntUnsignedTo32BitReal: 
			{
			HUInt32Ptr 				fromUInt32Ptr;
			HFloatPtr 				toFloatPtr;
			HFloatPtr 				savedToFloatPtr;
			
			fromUInt32Ptr = (HUInt32Ptr)ioCharBufferPtr;
			toFloatPtr = (HFloatPtr)dataCharBufferPtr;
			
			savedToFloatPtr = toFloatPtr;
			
			for (sample=0;  sample<numberSamples; sample+=columnInterval)
				{
				index = 0;
				channel = 0;
				toFloatPtr = savedToFloatPtr;
				while (index < numberChannels)
					{
					if (channelListPtr)
						//channel = channelListPtr[index];
						channel = layerInfoPtr[channelListPtr[index]+1].fileChannelNumber-1;
						
					*toFloatPtr = (float)fromUInt32Ptr[channel];
					toFloatPtr += toBufferSkip;
					index++;
					channel++;
					
					}	// end "while (index < numberChannels)" 
					
				fromUInt32Ptr += skipSamples;
						
				if (toBISFormatFlag)
					savedToFloatPtr += numberOutputBufferChannels;
					
				else	// !toBISFormatFlag
					savedToFloatPtr++;
				
				}	// end "for (sample=0; sample<numberSamples; ..."
				
			} 
			break;
							
		case k32BitIntSignedTo64BitReal: 
			{
			HSInt32Ptr 				fromSInt32Ptr;
			HDoublePtr 				toDoublePtr;
			HDoublePtr 				savedToDoublePtr;
			
			toDoublePtr = (HDoublePtr)dataCharBufferPtr;
			fromSInt32Ptr = (HSInt32Ptr)ioCharBufferPtr;
			
			savedToDoublePtr = toDoublePtr;
			
			for (sample=0;  sample<numberSamples; sample+=columnInterval)
				{
				index = 0;
				channel = 0;
				toDoublePtr = savedToDoublePtr;
				while (index < numberChannels)
					{
					if (channelListPtr)
						//channel = channelListPtr[index];
						channel = layerInfoPtr[channelListPtr[index]+1].fileChannelNumber-1;
						
					*toDoublePtr = fromSInt32Ptr[channel];
					toDoublePtr += toBufferSkip;
					index++;
					channel++;
					
					}	// end "while (index < numberChannels)"
					
				fromSInt32Ptr += skipSamples;
						
				if (toBISFormatFlag)
					savedToDoublePtr += numberOutputBufferChannels;
					
				else	// !toBISFormatFlag
					savedToDoublePtr++;
				
				}	// end "for (sample=0; sample<numberSamples; ..."
				
			}
			break;
							
		case k32BitIntUnsignedTo64BitReal: 
			{
			HUInt32Ptr 				fromUInt32Ptr;
			HDoublePtr 				toDoublePtr;
			HDoublePtr 				savedToDoublePtr;
			
			fromUInt32Ptr = (HUInt32Ptr)ioCharBufferPtr;
			toDoublePtr = (HDoublePtr)dataCharBufferPtr;
			
			savedToDoublePtr = toDoublePtr;
			
			for (sample=0;  sample<numberSamples; sample+=columnInterval)
				{
				index = 0;
				channel = 0;
				toDoublePtr = savedToDoublePtr;
				while (index < numberChannels)
					{
					if (channelListPtr)
						//channel = channelListPtr[index];
						channel = layerInfoPtr[channelListPtr[index]+1].fileChannelNumber-1;
						
					*toDoublePtr = fromUInt32Ptr[channel];
					toDoublePtr += toBufferSkip;
					index++;
					channel++;
					
					}	// end "while (index < numberChannels)" 
					
				fromUInt32Ptr += skipSamples;
						
				if (toBISFormatFlag)
					savedToDoublePtr += numberOutputBufferChannels;
					
				else	// !toBISFormatFlag
					savedToDoublePtr++;
					
				}	// end "for (sample=0; sample<numberSamples; ..."
				
			}
			break;
			
		case k32BitRealTo64BitReal: 
			{
			HFloatPtr 				fromFloatPtr;
			HDoublePtr 				toDoublePtr;
			HDoublePtr 				savedToDoublePtr;
			
			fromFloatPtr = (HFloatPtr)ioCharBufferPtr;
			toDoublePtr = (HDoublePtr)dataCharBufferPtr;
			
			savedToDoublePtr = toDoublePtr;
			
			for (sample=0;  sample<numberSamples; sample+=columnInterval)
				{
				index = 0;
				channel = 0;
				toDoublePtr = savedToDoublePtr;
				while (index < numberChannels)
					{
					if (channelListPtr)
						//channel = channelListPtr[index];
						channel = layerInfoPtr[channelListPtr[index]+1].fileChannelNumber-1;
						
					*toDoublePtr = fromFloatPtr[channel];
					toDoublePtr += toBufferSkip;
					index++;
					channel++;
				
					}	// end "while (index < numberChannels)" 
					
				fromFloatPtr += skipSamples;
						
				if (toBISFormatFlag)
					savedToDoublePtr += numberOutputBufferChannels;
					
				else	// !toBISFormatFlag
					savedToDoublePtr++;
				
				}	// end "for (sample=0; sample<numberSamples; ..."
				
			}
			break;
							
		case k64BitTo64Bit: 
			{
			DoublePtr 				fromDoublePtr;
			DoublePtr 				toDoublePtr;
			DoublePtr 				savedToDoublePtr;
			
			fromDoublePtr = (DoublePtr)ioCharBufferPtr;
			toDoublePtr = (DoublePtr)dataCharBufferPtr;
			
			savedToDoublePtr = toDoublePtr;
			
			for (sample=0;  sample<numberSamples; sample+=columnInterval)
				{
				index = 0;
				channel = 0;
				toDoublePtr = savedToDoublePtr;
				while (index < numberChannels)
					{
					if (channelListPtr)
						//channel = channelListPtr[index];
						channel = layerInfoPtr[channelListPtr[index]+1].fileChannelNumber-1;
						
					*toDoublePtr = fromDoublePtr[channel];
					toDoublePtr += toBufferSkip;
					index++;
					channel++;
					
					}	// end "while (index < numberChannels)" 
					
				fromDoublePtr += skipSamples;
						
				if (toBISFormatFlag)
					savedToDoublePtr += numberOutputBufferChannels;
					
				else	// !toBISFormatFlag
					savedToDoublePtr++;
					
				}	// end "for (sample=0; sample<numberSamples; ..."
				
			}
			break;
		
		case k64BitRealTo32BitReal:
			{
			HDoublePtr 				fromDoublePtr;
			HFloatPtr 				toFloatPtr;
			HFloatPtr 				savedToFloatPtr;
			
			fromDoublePtr = (HDoublePtr)ioCharBufferPtr;
			toFloatPtr = (HFloatPtr)dataCharBufferPtr;
			
			savedToFloatPtr = toFloatPtr;
			
			for (sample=0;  sample<numberSamples; sample+=columnInterval)
				{
				index = 0;
				channel = 0;
				toFloatPtr = savedToFloatPtr;
				while (index < numberChannels)
					{
					if (channelListPtr)
						//channel = channelListPtr[index];
						channel = layerInfoPtr[channelListPtr[index]+1].fileChannelNumber-1;
						
					*toFloatPtr = (float)fromDoublePtr[channel];
					toFloatPtr += toBufferSkip;
					index++;
					channel++;
					
					}	// end "while (index < numberChannels)" 
					
				fromDoublePtr += skipSamples;
						
				if (toBISFormatFlag)
					savedToFloatPtr += numberOutputBufferChannels;
					
				else	// !toBISFormatFlag
					savedToFloatPtr++;
				
				}	// end "for (sample=0; sample<numberSamples; ..."
				
			}
			break;
			
		default:
			break;
		
		}	// end "switch (dataConversionCode)"
		
}	// end "PackBISData"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void PackBlockedData
//
//	Software purpose:	The purpose of this routine is to pack the data for those
//							image files that are stored in a tiled or blocked format.
//							
//							Note that the 'bytesPerColumn' variable represents the number
//								of bytes per column of data. If the band interleave format
//								is BIS then this represents the number of bytes for all
//								channels in that one column.
//
//	Parameters in:				
//
//	Parameters out:			
//
//	Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/13/1999
//	Revised By:			Larry L. Biehl			Date: 12/17/2009

void PackBlockedData (
				UInt32								line,
				UInt32								columnStart, 
				UInt32								columnEnd,
				UInt32								channelStartIndex,
				FileInfoPtr							fileInfoPtr,
				HierarchalFileFormatPtr			hfaPtr,
				HUCharPtr							outputBufferPtr)

{
	HUCharPtr							inputBufferPtr;
	
	UInt32								bytesPerColumn,
											block,
											blockStartByte,
											bytesToMove,
											firstByteToMove,
											firstColumnRead,
											numberBlocksRead;
	
	
	inputBufferPtr = hfaPtr->tiledBufferPtr;
	
	bytesPerColumn = fileInfoPtr->numberBytes;
	if (fileInfoPtr->bandInterleave == kBIS)
		bytesPerColumn *= fileInfoPtr->numberChannels;
	
	blockStartByte = 
			(line - hfaPtr->firstLineRead) * hfaPtr->blockWidthRead * bytesPerColumn;
	
	if (fileInfoPtr->numberBits == 4)
		blockStartByte /= 2;
	
			// For the BIS band interleave case, channelStartIndex will always be 0.
				
	blockStartByte += channelStartIndex * hfaPtr->blockSizeRead;
	
			// This section is needed for the case when the first column being
			// read changes such as it does in the cluster processors.
			
	if (columnStart - hfaPtr->firstColumnRead >= hfaPtr->blockWidthRead)
		{
		numberBlocksRead = hfaPtr->numberBlocksRead - 1;
		firstColumnRead = hfaPtr->firstColumnRead + hfaPtr->blockWidthRead;
		
		}	// end "if (columnStart - hfaPtr->firstColumnRead >= ...->blockWidthRead)"
		
	else	// columnStart - hfaPtr->firstColumnRead < hfaPtr->blockWidthRead
		{
		numberBlocksRead = hfaPtr->numberBlocksRead;
		firstColumnRead = hfaPtr->firstColumnRead;
		
		}	// end "if (columnStart - hfaPtr->firstColumnRead >= ...->blockWidthRead)"
	
	for (block=1; block<=numberBlocksRead; block++)
		{
		firstByteToMove = blockStartByte;
			
		if (block == 1)
			{
			if (fileInfoPtr->numberBits != 4)
				{
				firstByteToMove += (columnStart - firstColumnRead) * bytesPerColumn;
			
				if (numberBlocksRead == 1)
					bytesToMove = columnEnd - columnStart + 1;
			
				else	// numberBlocksRead > 1
					bytesToMove = firstColumnRead + hfaPtr->blockWidthRead - columnStart;
								
				}	// end "if (fileInfoPtr->numberBits != 4)"
								
			else	// fileInfoPtr->numberBits == 4
				{
				firstByteToMove += (columnStart - firstColumnRead) * bytesPerColumn / 2;
			
				if (numberBlocksRead == 1)
					{
					bytesToMove = columnEnd - columnStart + 1;
				
					if (!(columnStart & 0x00000001) && (columnEnd & 0x00000001))
						bytesToMove++;
					
					}	// end "if (numberBlocksRead == 1)"
			
				else	// numberBlocksRead > 1
					bytesToMove = firstColumnRead + hfaPtr->blockWidthRead - columnStart;
				
				}	// end "else fileInfoPtr->numberBits == 4"
			
			}	// end "if (block == 1)"
			
		else if (block != numberBlocksRead)
			{
			bytesToMove = hfaPtr->blockWidthRead;
			
			}	// end "else if (block != numberBlocksRead)" 
			
		else	// block == numberBlocksRead
			{
			bytesToMove = columnEnd - (hfaPtr->lastColumnRead - hfaPtr->blockWidthRead);
			
			}	// end "else if (block != numberBlocksRead)" 
			
		bytesToMove *= bytesPerColumn;
			
		if (fileInfoPtr->numberBits == 4)
			{
			bytesToMove = (bytesToMove+1)/2;
			
			}	// end "else fileInfoPtr->numberBits == 4"
			
		BlockMoveData (&inputBufferPtr[firstByteToMove], 
								outputBufferPtr, 
								(SInt32)bytesToMove);
								
		outputBufferPtr += bytesToMove;
			
		blockStartByte += hfaPtr->blockOffset;
		
		}	// end "for (block=1; block<=numberBlocks; block++)"
		
}	// end "PackBlockedData"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void PackGAIAData
//
//	Software purpose:	The purpose of this routine is to pack the data from
//							a line of GAIA data.  This includes removing the 
//							segmented bytes that will be at columns 1016, 2038,
//							3060, 4082, etc
//
//	Parameters in:				
//
//	Parameters out:			
//
//	Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/03/1994
//	Revised By:			Larry L. Biehl			Date: 05/03/1995

void PackGAIAData (
				SInt32								columnStart, 
				SInt32								columnEnd, 
				HSInt16Ptr							inputBufferPtr)

{
	HSInt16Ptr							outputBufferPtr;
	
	SInt32								lastColumnMoved,
											samplesToMove;
	
	
	outputBufferPtr = inputBufferPtr;
	
			// Determine the bytes to skip which will not have to be moved.	
	
	lastColumnMoved = 1015;
	lastColumnMoved = MIN (lastColumnMoved, columnEnd);
	
	samplesToMove = lastColumnMoved - columnStart + 1;
	
	while (lastColumnMoved < columnEnd)
		{
		inputBufferPtr += samplesToMove + 1;
		outputBufferPtr += samplesToMove;
	
		columnStart += samplesToMove;
		lastColumnMoved += 1021;
		lastColumnMoved = MIN (lastColumnMoved, columnEnd);
		
		samplesToMove = lastColumnMoved - columnStart + 1;
		
		BlockMoveData (inputBufferPtr, outputBufferPtr, samplesToMove*2);
		
		}	// end "while (lastColumnMoved < columnEnd)" 
		
}	// end "PackGAIAData"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		UInt32 PackMaskData
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
//	Coded By:			Larry L. Biehl			Date: 10/28/1998
//	Revised By:			Larry L. Biehl			Date: 12/14/2005

UInt32 PackMaskData (
				HUInt16Ptr							maskBufferPtr,
				UInt32								maskColumnStart,
				UInt32								numberChannels,
				UInt32								numberInputSamples,
				UInt32								columnInterval,
				HUCharPtr							dataBufferPtr,
				UInt32								numberOutBytes,
				SInt32								maskValueRequest)

{ 
	HUCharPtr							inDataBufferPtr,
											outDataBufferPtr;
											
	UInt32								numberBytesToMove,
											numberOutputSamples,
											sample;
	
	
			// Get the relative position to start reading in the mask buffer with 
			// respect to the start of the file.					
			
	maskBufferPtr += maskColumnStart;
	
	inDataBufferPtr = dataBufferPtr;
	outDataBufferPtr = dataBufferPtr;
	numberBytesToMove = numberOutBytes * numberChannels;
	numberOutputSamples = 0;
	
	if (maskValueRequest == 0)
		{										
		for (sample=0; sample<numberInputSamples; sample+=columnInterval)
			{
			if (*maskBufferPtr != 0)
				{
				if (inDataBufferPtr != outDataBufferPtr)
					BlockMoveData (inDataBufferPtr, outDataBufferPtr, numberBytesToMove);
					
				outDataBufferPtr += numberBytesToMove;
				numberOutputSamples++;
				
				}	// end "if (maskBufferPtr[maskIndex] != 0)"
				
			inDataBufferPtr += numberBytesToMove;
				
			maskBufferPtr += columnInterval;
				
			}	// end "for (sample=0; sample<count; sample++)"
			
		}	// end "if (maskValueRequest == 0)"
		
	else	// maskValueRequest != 0
		{										
		for (sample=0; sample<numberInputSamples; sample+=columnInterval)
			{
			if ((SInt32)*maskBufferPtr == maskValueRequest)
				{
				if (inDataBufferPtr != outDataBufferPtr)
					BlockMoveData (inDataBufferPtr, outDataBufferPtr, numberBytesToMove);
					
				outDataBufferPtr += numberBytesToMove;
				numberOutputSamples++;
				
				}	// end "if (maskBufferPtr[maskIndex] == maskValueRequest)"
				
			inDataBufferPtr += numberBytesToMove;
				
			maskBufferPtr += columnInterval;
				
			}	// end "for (sample=0; sample<count; sample++)"
			
		}	// end "else maskValueRequest != 0"
		
	return (numberOutputSamples);
	      
}	// end "PackMaskData"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void PackNonBISData
//
//	Software purpose:	The purpose of this routine is to pack the line
//							of data for nonBIS formated files if the column
//							interval is more than 1 and/or convert the line
//							of data to BIS formatted style.
//
//	Parameters in:				
//
//	Parameters out:	Number of samples loaded into buffer
//
//	Value Returned:	Error code for file operations. 
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 09/18/1989
//	Revised By:			Larry L. Biehl			Date: 04/12/2006

void PackNonBISData (
				UInt32								columnOffset,
				UInt32								columnInterval,
				UInt32								numberColumnsPerChannel, 
				UInt32								numberBufferChannels,
				UInt32								numberChannels,
				HUInt16Ptr							channelListPtr, 
				UInt32								numberSamples,
				UInt32								dataConversionCode,
				HUCharPtr							ioCharBufferPtr, 
				HUCharPtr							dataCharBufferPtr, 
				Boolean								toBISFormat)

{
	UInt32								channel,
											currentChannel,
											dataPtrIncrement,
											sample;
	
	
			// Get increment to use for buffer depending on if data is to be 		
			// converted to BIS format.														
			
	dataPtrIncrement = 1;
	if (toBISFormat)
		dataPtrIncrement = numberChannels;
		
			// Set up parameters for skipping some input channels if needed.
		
	currentChannel = 0;	
	if (channelListPtr != NULL)
		currentChannel = channelListPtr[0];
	
	switch (dataConversionCode)
		{
		case k8BitTo8Bit: 
			{
			HUCharPtr			dataCharPtr;
			HUCharPtr			ioCharPtr;
			
			dataCharPtr = dataCharBufferPtr;
			ioCharPtr = ioCharBufferPtr;
			ioCharPtr = &ioCharPtr[columnOffset];
			
					// Loop through by number of channels. Allow for case when some of
					// the input channels need to be skipped.	
				
			for (channel=0; channel<numberBufferChannels; channel++)
				{
				if (channelListPtr != NULL)
					{
					while (channelListPtr[channel] != currentChannel)
						{
						ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
						currentChannel++;
						
						}	// end "while (channelListPtr[channel] != currentChannel)"
						
					}	// end "if (channelListPtr != NULL)"
				
				if (toBISFormat)
					dataCharPtr = &dataCharBufferPtr[channel];
					
				for (sample=0;  sample<numberSamples; sample+=columnInterval)
					{
					*dataCharPtr = ioCharPtr[sample];
					dataCharPtr += dataPtrIncrement;
					
					}	// end "for (sample=0;  sample<numberChan..."
					
						// Update the input buffer pointer to point to beginning of	
						// data for next channel.												
						
				ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
				currentChannel++;
				
				}	// end "for (channel=0; channel<...)" 
				
			}
			break;
			
		case k8BitIntSignedTo16BitIntSigned: 
			{
			HSInt16Ptr			dataSInt16Ptr,
									savedDataSInt16Ptr;
			HSInt8Ptr			ioCharPtr;
		
			dataSInt16Ptr = (HSInt16Ptr)dataCharBufferPtr;
			savedDataSInt16Ptr = dataSInt16Ptr;
			
			ioCharPtr = (HSInt8Ptr)ioCharBufferPtr;
			ioCharPtr = &ioCharPtr[columnOffset];
			
					// Loop through by number of channels. Allow for case when some of
					// the input channels need to be skipped.				
				
			for (channel=0; channel<numberBufferChannels; channel++)
				{
				if (channelListPtr != NULL)
					{
					while (channelListPtr[channel] != currentChannel)
						{
						ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
						currentChannel++;
						
						}	// end "while (channelListPtr[channel] != currentChannel)"
						
					}	// end "if (channelListPtr != NULL)"
					
				if (toBISFormat)
					//dataSInt16Ptr = (HSInt16Ptr)&dataCharBufferPtr[channel*2];
					dataSInt16Ptr = savedDataSInt16Ptr + channel;
			
				for (sample=0;  sample<numberSamples; sample+=columnInterval)
					{
					*dataSInt16Ptr = ioCharPtr[sample];
					dataSInt16Ptr += dataPtrIncrement;
					
					}	// end "for (sample=0;  sample<numberChan..." 
					
						// Update the input buffer pointer to point to beginning of	
						// data for next channel.												
						
				ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
				currentChannel++;
				
				}	// end "for (channel=0; channel<...)" 
			/*
			if (swapBytesFlag)
				{
				numberSamples = (numberSamples + columnInterval)/columnInterval;
				numberSamples *= numberBufferChannels;
				
				Swap2Bytes ((HUInt16Ptr)dataCharBufferPtr, numberSamples);
				
				}	// end "if (swapBytesFlag)" 
			*/				
			}
			break;
			
		case k8BitIntUnsignedTo16BitIntUnsigned: 
			{
			HUInt16Ptr			dataIntPtr;
			HUCharPtr			ioCharPtr;
		
			dataIntPtr = (HUInt16Ptr)dataCharBufferPtr;
			ioCharPtr = ioCharBufferPtr;
			ioCharPtr = &ioCharPtr[columnOffset];
			
					// Loop through by number of channels. Allow for case when some of
					// the input channels need to be skipped.				
				
			for (channel=0; channel<numberBufferChannels; channel++)
				{
				if (channelListPtr != NULL)
					{
					while (channelListPtr[channel] != currentChannel)
						{
						ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
						currentChannel++;
						
						}	// end "while (channelListPtr[channel] != currentChannel)"
						
					}	// end "if (channelListPtr != NULL)"
					
				if (toBISFormat)
					dataIntPtr = (HUInt16Ptr)&dataCharBufferPtr[channel*2];
			
				for (sample=0;  sample<numberSamples; sample+=columnInterval)
					{
					*dataIntPtr = ioCharPtr[sample];
					dataIntPtr += dataPtrIncrement;
					
					}	// end "for (sample=0;  sample<numberChan..." 
					
						// Update the input buffer pointer to point to beginning of	
						// data for next channel.												
						
				ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
				currentChannel++;
				
				}	// end "for (channel=0; channel<...)" 
			/*
			if (swapBytesFlag)
				{
				numberSamples = (numberSamples + columnInterval)/columnInterval;
				numberSamples *= numberBufferChannels;
				
				Swap2Bytes ((HUInt16Ptr)dataCharBufferPtr, numberSamples);
				
				}	// end "if (swapBytesFlag)" 
			*/					
			}
			break;
			
		case k8BitIntSignedTo32BitIntSigned: 
			{
			HSInt32Ptr			dataSInt32Ptr;
			HSInt8Ptr			ioCharPtr;
		
			dataSInt32Ptr = (HSInt32Ptr)dataCharBufferPtr;
			ioCharPtr = (HSInt8Ptr)ioCharBufferPtr;
			ioCharPtr = &ioCharPtr[columnOffset];
			
					// Loop through by number of channels.									
				
			for (channel=0; channel<numberBufferChannels; channel++)
				{
				if (channelListPtr != NULL)
					{
					while (channelListPtr[channel] != currentChannel)
						{
						ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
						currentChannel++;
						
						}	// end "while (channelListPtr[channel] != currentChannel)"
						
					}	// end "if (channelListPtr != NULL)"
				
				if (toBISFormat)
					dataSInt32Ptr = (HSInt32Ptr)&dataCharBufferPtr[channel*4];
			
				for (sample=0;  sample<numberSamples; sample+=columnInterval)
					{
					*dataSInt32Ptr = ioCharPtr[sample];
					dataSInt32Ptr += dataPtrIncrement;
					
					}	// end "for (sample=0;  sample<numberChan..." 
					
						// Update the input buffer pointer to point to beginning of	
						// data for next channel.												
						
				ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
				currentChannel++;
				
				}	// end "for (channel=0; channel<...)"
				
			}
			break;
			
		case k8BitIntUnsignedTo32BitIntUnsigned: 
			{
			HUInt32Ptr			dataSInt32Ptr;
			HUInt8Ptr			ioCharPtr;
		
			dataSInt32Ptr = (HUInt32Ptr)dataCharBufferPtr;
			ioCharPtr = ioCharBufferPtr;
			ioCharPtr = &ioCharPtr[columnOffset];
			
					// Loop through by number of channels.									
				
			for (channel=0; channel<numberBufferChannels; channel++)
				{
				if (channelListPtr != NULL)
					{
					while (channelListPtr[channel] != currentChannel)
						{
						ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
						currentChannel++;
						
						}	// end "while (channelListPtr[channel] != currentChannel)"
						
					}	// end "if (channelListPtr != NULL)"
				
				if (toBISFormat)
					dataSInt32Ptr = (HUInt32Ptr)&dataCharBufferPtr[channel*4];
			
				for (sample=0;  sample<numberSamples; sample+=columnInterval)
					{
					*dataSInt32Ptr = ioCharPtr[sample];
					dataSInt32Ptr += dataPtrIncrement;
					
					}	// end "for (sample=0;  sample<numberChan..." 
					
						// Update the input buffer pointer to point to beginning of	
						// data for next channel.												
						
				ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
				currentChannel++;
				
				}	// end "for (channel=0; channel<...)"
					
			}
			break;
			
		case k8BitIntSignedTo32BitReal:
			{
			HFloatPtr			dataOutPtr;
			HSInt8Ptr			ioCharPtr;
		
			dataOutPtr = (HFloatPtr)dataCharBufferPtr;
			ioCharPtr = (HSInt8Ptr)ioCharBufferPtr;
			ioCharPtr = &ioCharPtr[columnOffset];
			
					// Loop through by number of channels.									
				
			for (channel=0; channel<numberBufferChannels; channel++)
				{
				if (channelListPtr != NULL)
					{
					while (channelListPtr[channel] != currentChannel)
						{
						ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
						currentChannel++;
						
						}	// end "while (channelListPtr[channel] != currentChannel)"
						
					}	// end "if (channelListPtr != NULL)"
				
				if (toBISFormat)
					dataOutPtr = (HFloatPtr)&dataCharBufferPtr[channel*4];
			
				for (sample=0;  sample<numberSamples; sample+=columnInterval)
					{
					*dataOutPtr = ioCharPtr[sample];
					dataOutPtr += dataPtrIncrement;
					
					}	// end "for (sample=0;  sample<numberChan..." 
					
						// Update the input buffer pointer to point to beginning of	
						// data for next channel.												
						
				ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
				currentChannel++;
				
				}	// end "for (channel=0; channel<...)"
				
			}
			break;
			
		case k8BitIntUnsignedTo32BitReal: 
			{
			HFloatPtr			dataOutPtr;
			HUInt8Ptr			ioCharPtr;
		
			dataOutPtr = (HFloatPtr)dataCharBufferPtr;
			ioCharPtr = ioCharBufferPtr;
			ioCharPtr = &ioCharPtr[columnOffset];
			
					// Loop through by number of channels.									
				
			for (channel=0; channel<numberBufferChannels; channel++)
				{
				if (channelListPtr != NULL)
					{
					while (channelListPtr[channel] != currentChannel)
						{
						ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
						currentChannel++;
						
						}	// end "while (channelListPtr[channel] != currentChannel)"
						
					}	// end "if (channelListPtr != NULL)"
				
				if (toBISFormat)
					dataOutPtr = (HFloatPtr)&dataCharBufferPtr[channel*4];
			
				for (sample=0;  sample<numberSamples; sample+=columnInterval)
					{
					*dataOutPtr = ioCharPtr[sample];
					dataOutPtr += dataPtrIncrement;
					
					}	// end "for (sample=0;  sample<numberChan..." 
					
						// Update the input buffer pointer to point to beginning of	
						// data for next channel.												
						
				ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
				currentChannel++;
				
				}	// end "for (channel=0; channel<...)"
			
			}
			break;
			
		case k8BitIntSignedTo64BitReal: 
			{
			HDoublePtr			dataOutPtr;
			HSInt8Ptr			ioCharPtr;
		
			dataOutPtr = (HDoublePtr)dataCharBufferPtr;
			ioCharPtr = (HSInt8Ptr)ioCharBufferPtr;
			ioCharPtr = &ioCharPtr[columnOffset];
			
					// Loop through by number of channels.									
				
			for (channel=0; channel<numberBufferChannels; channel++)
				{
				if (channelListPtr != NULL)
					{
					while (channelListPtr[channel] != currentChannel)
						{
						ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
						currentChannel++;
						
						}	// end "while (channelListPtr[channel] != currentChannel)"
						
					}	// end "if (channelListPtr != NULL)"
				
				if (toBISFormat)
					dataOutPtr = (HDoublePtr)&dataCharBufferPtr[channel*8];
			
				for (sample=0;  sample<numberSamples; sample+=columnInterval)
					{
					*dataOutPtr = ioCharPtr[sample];
					dataOutPtr += dataPtrIncrement;
					
					}	// end "for (sample=0;  sample<numberChan..." 
					
						// Update the input buffer pointer to point to beginning of	
						// data for next channel.												
						
				ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
				currentChannel++;
				
				}	// end "for (channel=0; channel<...)"
				
			}
			break;
			
		case k8BitIntUnsignedTo64BitReal: 
			{
			HDoublePtr			dataOutPtr;
			HUInt8Ptr			ioCharPtr;
		
			dataOutPtr = (HDoublePtr)dataCharBufferPtr;
			ioCharPtr = ioCharBufferPtr;
			ioCharPtr = &ioCharPtr[columnOffset];
			
					// Loop through by number of channels.									
				
			for (channel=0; channel<numberBufferChannels; channel++)
				{
				if (channelListPtr != NULL)
					{
					while (channelListPtr[channel] != currentChannel)
						{
						ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
						currentChannel++;
						
						}	// end "while (channelListPtr[channel] != currentChannel)"
						
					}	// end "if (channelListPtr != NULL)"
				
				if (toBISFormat)
					dataOutPtr = (HDoublePtr)&dataCharBufferPtr[channel*8];
			
				for (sample=0;  sample<numberSamples; sample+=columnInterval)
					{
					*dataOutPtr = ioCharPtr[sample];
					dataOutPtr += dataPtrIncrement;
					
					}	// end "for (sample=0;  sample<numberChan..." 
					
						// Update the input buffer pointer to point to beginning of	
						// data for next channel.												
						
				ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
				currentChannel++;
				
				}	// end "for (channel=0; channel<...)"
			
			}
			break;
			
		case k16BitTo16Bit: 
			{
			HSInt16Ptr					dataSInt16Ptr,
											ioSInt16Ptr,
											savedDataSInt16Ptr;
		
			dataSInt16Ptr = (HSInt16Ptr)dataCharBufferPtr;
			savedDataSInt16Ptr = dataSInt16Ptr;
			
			ioSInt16Ptr = (HSInt16Ptr)ioCharBufferPtr;
			ioSInt16Ptr = &ioSInt16Ptr[columnOffset];
			
					// Loop through by number of channels.										
				
			for (channel=0; channel<numberBufferChannels; channel++)
				{
				if (channelListPtr != NULL)
					{
					while (channelListPtr[channel] != currentChannel)
						{
						ioSInt16Ptr = &ioSInt16Ptr[numberColumnsPerChannel];
						currentChannel++;
						
						}	// end "while (channelListPtr[channel] != currentChannel)"
						
					}	// end "if (channelListPtr != NULL)"
					
				if (toBISFormat)
					//dataIntPtr = (HSInt16Ptr)&dataCharBufferPtr[channel*2];
					dataSInt16Ptr = savedDataSInt16Ptr + channel;
					
				for (sample=0;  sample<numberSamples; sample+=columnInterval)
					{
					*dataSInt16Ptr = ioSInt16Ptr[sample];
					dataSInt16Ptr += dataPtrIncrement;
					
					}	// end "for (sample=0;  sample<numberChan..." 
					
						// Update the input buffer pointer to point to beginning of		
						// data for next channel.													
				
				ioSInt16Ptr = &ioSInt16Ptr[numberColumnsPerChannel];
				currentChannel++;
				
				}	// end "for (channel=0; channel<numberBufferChannels; channel++)" 
				
			}
			break;
			
		case k16BitIntSignedTo32BitIntSigned:
			{
			HSInt32Ptr					dataSInt32Ptr;
			HSInt16Ptr					ioIntPtr;
		
			dataSInt32Ptr = (HSInt32Ptr)dataCharBufferPtr;
			ioIntPtr = (HSInt16Ptr)ioCharBufferPtr;
			ioIntPtr = &ioIntPtr[columnOffset];
			
					// Loop through by number of channels.										
				
			for (channel=0; channel<numberBufferChannels; channel++)
				{
				if (channelListPtr != NULL)
					{
					while (channelListPtr[channel] != currentChannel)
						{
						ioIntPtr = &ioIntPtr[numberColumnsPerChannel];
						currentChannel++;
						
						}	// end "while (channelListPtr[channel] != currentChannel)"
						
					}	// end "if (channelListPtr != NULL)"
				
				if (toBISFormat)
					dataSInt32Ptr = (HSInt32Ptr)&dataCharBufferPtr[channel*4];
					
				for (sample=0;  sample<numberSamples; sample+=columnInterval)
					{
					*dataSInt32Ptr = ioIntPtr[sample];
					dataSInt32Ptr += dataPtrIncrement;
					
					}	// end "for (sample=0;  sample<numberChan..." 
					
						// Update the input buffer pointer to point to beginning of		
						// data for next channel.													
				
				ioIntPtr = &ioIntPtr[numberColumnsPerChannel];
				currentChannel++;
				
				}	// end "for (channel=0; channel<numberBufferChannels; channel++)"
				
			}
			break;
			
		case k16BitIntUnsignedTo32BitIntUnsigned: 
			{
			HUInt32Ptr					dataUInt32Ptr;
			HUInt16Ptr					ioUIntPtr;
			HUInt32Ptr					savedDataUInt32Ptr;
		
			dataUInt32Ptr = (HUInt32Ptr)dataCharBufferPtr;
			savedDataUInt32Ptr = dataUInt32Ptr;
			ioUIntPtr = (HUInt16Ptr)ioCharBufferPtr;
			ioUIntPtr = &ioUIntPtr[columnOffset];
			
					// Loop through by number of channels.										
				
			for (channel=0; channel<numberBufferChannels; channel++)
				{
				if (channelListPtr != NULL)
					{
					while (channelListPtr[channel] != currentChannel)
						{
						ioUIntPtr = &ioUIntPtr[numberColumnsPerChannel];
						currentChannel++;
						
						}	// end "while (channelListPtr[channel] != currentChannel)"
						
					}	// end "if (channelListPtr != NULL)"
				
				if (toBISFormat)
					//dataSInt32Ptr = (HSInt32Ptr)&dataCharBufferPtr[channel*4];
					dataUInt32Ptr = savedDataUInt32Ptr + channel;
					
				for (sample=0;  sample<numberSamples; sample+=columnInterval)
					{
					*dataUInt32Ptr = ioUIntPtr[sample];
					dataUInt32Ptr += dataPtrIncrement;
					
					}	// end "for (sample=0;  sample<numberChan..." 
					
						// Update the input buffer pointer to point to beginning of		
						// data for next channel.													
				
				ioUIntPtr = &ioUIntPtr[numberColumnsPerChannel];
				currentChannel++;
				
				}	// end "for (channel=0; channel<numberBufferChannels; channel++)"
				
			}
			break;
			
		case k16BitIntSignedTo32BitReal:
			{
			HFloatPtr			dataOutPtr;
			HSInt16Ptr			ioCharPtr;
		
			dataOutPtr = (HFloatPtr)dataCharBufferPtr;
			ioCharPtr = (HSInt16Ptr)ioCharBufferPtr;
			ioCharPtr = &ioCharPtr[columnOffset];
			
					// Loop through by number of channels.									
				
			for (channel=0; channel<numberBufferChannels; channel++)
				{
				if (channelListPtr != NULL)
					{
					while (channelListPtr[channel] != currentChannel)
						{
						ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
						currentChannel++;
						
						}	// end "while (channelListPtr[channel] != currentChannel)"
						
					}	// end "if (channelListPtr != NULL)"
				
				if (toBISFormat)
					dataOutPtr = (HFloatPtr)&dataCharBufferPtr[channel*4];
			
				for (sample=0;  sample<numberSamples; sample+=columnInterval)
					{
					*dataOutPtr = ioCharPtr[sample];
					dataOutPtr += dataPtrIncrement;
					
					}	// end "for (sample=0;  sample<numberChan..." 
					
						// Update the input buffer pointer to point to beginning of	
						// data for next channel.												
						
				ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
				currentChannel++;
				
				}	// end "for (channel=0; channel<...)"
				
			}
			break;
			
		case k16BitIntUnsignedTo32BitReal: 
			{
			HFloatPtr			dataOutPtr;
			HUInt16Ptr			ioCharPtr;
		
			dataOutPtr = (HFloatPtr)dataCharBufferPtr;
			ioCharPtr = (HUInt16Ptr)ioCharBufferPtr;
			ioCharPtr = &ioCharPtr[columnOffset];
			
					// Loop through by number of channels.									
				
			for (channel=0; channel<numberBufferChannels; channel++)
				{
				if (channelListPtr != NULL)
					{
					while (channelListPtr[channel] != currentChannel)
						{
						ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
						currentChannel++;
						
						}	// end "while (channelListPtr[channel] != currentChannel)"
						
					}	// end "if (channelListPtr != NULL)"
				
				if (toBISFormat)
					dataOutPtr = (HFloatPtr)&dataCharBufferPtr[channel*4];
			
				for (sample=0;  sample<numberSamples; sample+=columnInterval)
					{
					*dataOutPtr = ioCharPtr[sample];
					dataOutPtr += dataPtrIncrement;
					
					}	// end "for (sample=0;  sample<numberChan..." 
					
						// Update the input buffer pointer to point to beginning of	
						// data for next channel.												
						
				ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
				currentChannel++;
				
				}	// end "for (channel=0; channel<...)"
			
			}
			break;
			
		case k16BitIntSignedTo64BitReal: 
			{
			HDoublePtr			dataOutPtr;
			HSInt16Ptr			ioCharPtr;
		
			dataOutPtr = (HDoublePtr)dataCharBufferPtr;
			ioCharPtr = (HSInt16Ptr)ioCharBufferPtr;
			ioCharPtr = &ioCharPtr[columnOffset];
			
					// Loop through by number of channels.									
				
			for (channel=0; channel<numberBufferChannels; channel++)
				{
				if (channelListPtr != NULL)
					{
					while (channelListPtr[channel] != currentChannel)
						{
						ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
						currentChannel++;
						
						}	// end "while (channelListPtr[channel] != currentChannel)"
						
					}	// end "if (channelListPtr != NULL)"
				
				if (toBISFormat)
					dataOutPtr = (HDoublePtr)&dataCharBufferPtr[channel*8];
			
				for (sample=0;  sample<numberSamples; sample+=columnInterval)
					{
					*dataOutPtr = ioCharPtr[sample];
					dataOutPtr += dataPtrIncrement;
					
					}	// end "for (sample=0;  sample<numberChan..." 
					
						// Update the input buffer pointer to point to beginning of	
						// data for next channel.												
						
				ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
				currentChannel++;
				
				}	// end "for (channel=0; channel<...)"
				
			}
			break;
			
		case k16BitIntUnsignedTo64BitReal: 
			{
			HDoublePtr			dataOutPtr;
			HUInt16Ptr			ioCharPtr;
		
			dataOutPtr = (HDoublePtr)dataCharBufferPtr;
			ioCharPtr = (HUInt16Ptr)ioCharBufferPtr;
			ioCharPtr = &ioCharPtr[columnOffset];
			
					// Loop through by number of channels.									
				
			for (channel=0; channel<numberBufferChannels; channel++)
				{
				if (channelListPtr != NULL)
					{
					while (channelListPtr[channel] != currentChannel)
						{
						ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
						currentChannel++;
						
						}	// end "while (channelListPtr[channel] != currentChannel)"
						
					}	// end "if (channelListPtr != NULL)"
				
				if (toBISFormat)
					dataOutPtr = (HDoublePtr)&dataCharBufferPtr[channel*8];
			
				for (sample=0;  sample<numberSamples; sample+=columnInterval)
					{
					*dataOutPtr = ioCharPtr[sample];
					dataOutPtr += dataPtrIncrement;
					
					}	// end "for (sample=0;  sample<numberChan..." 
					
						// Update the input buffer pointer to point to beginning of	
						// data for next channel.												
						
				ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
				currentChannel++;
				
				}	// end "for (channel=0; channel<...)"
			
			} 
			break;
			
		case k32BitTo32Bit: 
			{
			HSInt32Ptr					dataSInt32Ptr;
			HSInt32Ptr					ioIntPtr;
			HSInt32Ptr					savedDataSInt32Ptr;
		
			dataSInt32Ptr = (HSInt32Ptr)dataCharBufferPtr;
			savedDataSInt32Ptr = dataSInt32Ptr;
			ioIntPtr = (HSInt32Ptr)ioCharBufferPtr;
			ioIntPtr = &ioIntPtr[columnOffset];
			
					// Loop through by number of channels.										
				
			for (channel=0; channel<numberBufferChannels; channel++)
				{
				if (channelListPtr != NULL)
					{
					while (channelListPtr[channel] != currentChannel)
						{
						ioIntPtr = &ioIntPtr[numberColumnsPerChannel];
						currentChannel++;
						
						}	// end "while (channelListPtr[channel] != currentChannel)"
						
					}	// end "if (channelListPtr != NULL)"
				
				if (toBISFormat)
					//dataSInt32Ptr = (HSInt32Ptr)&dataCharBufferPtr[channel*4];
					dataSInt32Ptr = savedDataSInt32Ptr + channel;
					
				for (sample=0;  sample<numberSamples; sample+=columnInterval)
					{
					*dataSInt32Ptr = ioIntPtr[sample];
					dataSInt32Ptr += dataPtrIncrement;
					
					}	// end "for (sample=0;  sample<numberChan..." 
					
						// Update the input buffer pointer to point to beginning of		
						// data for next channel.													
				
				ioIntPtr = &ioIntPtr[numberColumnsPerChannel];
				currentChannel++;
				
				}	// end "for (channel=0; channel<numberBufferChannels; channel++)"
				
			}
			break;
			
		case k32BitIntSignedTo32BitReal:
			{
			HFloatPtr			dataOutPtr;
			HSInt32Ptr			ioCharPtr;
		
			dataOutPtr = (HFloatPtr)dataCharBufferPtr;
			ioCharPtr = (HSInt32Ptr)ioCharBufferPtr;
			ioCharPtr = &ioCharPtr[columnOffset];
			
					// Loop through by number of channels.									
				
			for (channel=0; channel<numberBufferChannels; channel++)
				{
				if (channelListPtr != NULL)
					{
					while (channelListPtr[channel] != currentChannel)
						{
						ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
						currentChannel++;
						
						}	// end "while (channelListPtr[channel] != currentChannel)"
						
					}	// end "if (channelListPtr != NULL)"
				
				if (toBISFormat)
					dataOutPtr = (HFloatPtr)&dataCharBufferPtr[channel*4];
			
				for (sample=0;  sample<numberSamples; sample+=columnInterval)
					{
					*dataOutPtr = (float)ioCharPtr[sample];
					dataOutPtr += dataPtrIncrement;
					
					}	// end "for (sample=0;  sample<numberChan..." 
					
						// Update the input buffer pointer to point to beginning of	
						// data for next channel.												
						
				ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
				currentChannel++;
				
				}	// end "for (channel=0; channel<...)"
				
			}
			break;
			
		case k32BitIntUnsignedTo32BitReal: 
			{
			HFloatPtr			dataOutPtr;
			HUInt32Ptr			ioCharPtr;
		
			dataOutPtr = (HFloatPtr)dataCharBufferPtr;
			ioCharPtr = (HUInt32Ptr)ioCharBufferPtr;
			ioCharPtr = &ioCharPtr[columnOffset];
			
					// Loop through by number of channels.									
				
			for (channel=0; channel<numberBufferChannels; channel++)
				{
				if (channelListPtr != NULL)
					{
					while (channelListPtr[channel] != currentChannel)
						{
						ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
						currentChannel++;
						
						}	// end "while (channelListPtr[channel] != currentChannel)"
						
					}	// end "if (channelListPtr != NULL)"
				
				if (toBISFormat)
					dataOutPtr = (HFloatPtr)&dataCharBufferPtr[channel*4];
			
				for (sample=0;  sample<numberSamples; sample+=columnInterval)
					{
					*dataOutPtr = (float)ioCharPtr[sample];
					dataOutPtr += dataPtrIncrement;
					
					}	// end "for (sample=0;  sample<numberChan..." 
					
						// Update the input buffer pointer to point to beginning of	
						// data for next channel.												
						
				ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
				currentChannel++;
				
				}	// end "for (channel=0; channel<...)"
			
			}
			break;
			
		case k32BitIntSignedTo64BitReal: 
			{
			HDoublePtr			dataOutPtr;
			HSInt32Ptr			ioCharPtr;
		
			dataOutPtr = (HDoublePtr)dataCharBufferPtr;
			ioCharPtr = (HSInt32Ptr)ioCharBufferPtr;
			ioCharPtr = &ioCharPtr[columnOffset];
			
					// Loop through by number of channels.									
				
			for (channel=0; channel<numberBufferChannels; channel++)
				{
				if (channelListPtr != NULL)
					{
					while (channelListPtr[channel] != currentChannel)
						{
						ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
						currentChannel++;
						
						}	// end "while (channelListPtr[channel] != currentChannel)"
						
					}	// end "if (channelListPtr != NULL)"
				
				if (toBISFormat)
					dataOutPtr = (HDoublePtr)&dataCharBufferPtr[channel*8];
			
				for (sample=0;  sample<numberSamples; sample+=columnInterval)
					{
					*dataOutPtr = ioCharPtr[sample];
					dataOutPtr += dataPtrIncrement;
					
					}	// end "for (sample=0;  sample<numberChan..." 
					
						// Update the input buffer pointer to point to beginning of	
						// data for next channel.												
						
				ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
				currentChannel++;
				
				}	// end "for (channel=0; channel<...)"
				
			}
			break;
			
		case k32BitIntUnsignedTo64BitReal: 
			{
			HDoublePtr			dataOutPtr;
			HUInt32Ptr			ioCharPtr;
		
			dataOutPtr = (HDoublePtr)dataCharBufferPtr;
			ioCharPtr = (HUInt32Ptr)ioCharBufferPtr;
			ioCharPtr = &ioCharPtr[columnOffset];
			
					// Loop through by number of channels.									
				
			for (channel=0; channel<numberBufferChannels; channel++)
				{
				if (channelListPtr != NULL)
					{
					while (channelListPtr[channel] != currentChannel)
						{
						ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
						currentChannel++;
						
						}	// end "while (channelListPtr[channel] != currentChannel)"
						
					}	// end "if (channelListPtr != NULL)"
				
				if (toBISFormat)
					dataOutPtr = (HDoublePtr)&dataCharBufferPtr[channel*8];
			
				for (sample=0;  sample<numberSamples; sample+=columnInterval)
					{
					*dataOutPtr = ioCharPtr[sample];
					dataOutPtr += dataPtrIncrement;
					
					}	// end "for (sample=0;  sample<numberChan..." 
					
						// Update the input buffer pointer to point to beginning of	
						// data for next channel.												
						
				ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
				currentChannel++;
				
				}	// end "for (channel=0; channel<...)"
			
			}
			break;
			
		case k32BitRealTo64BitReal: 
			{
			HDoublePtr			dataDoubleOutPtr;
			HFloatPtr			ioFloatCharPtr;
		
			dataDoubleOutPtr = (HDoublePtr)dataCharBufferPtr;
			ioFloatCharPtr = (HFloatPtr)ioCharBufferPtr;
			ioFloatCharPtr = &ioFloatCharPtr[columnOffset];
			
					// Loop through by number of channels.									
				
			for (channel=0; channel<numberBufferChannels; channel++)
				{
				if (channelListPtr != NULL)
					{
					while (channelListPtr[channel] != currentChannel)
						{
						ioFloatCharPtr = &ioFloatCharPtr[numberColumnsPerChannel];
						currentChannel++;
						
						}	// end "while (channelListPtr[channel] != currentChannel)"
						
					}	// end "if (channelListPtr != NULL)"
				
				if (toBISFormat)
					dataDoubleOutPtr = (HDoublePtr)&dataCharBufferPtr[channel*8];
			
				for (sample=0;  sample<numberSamples; sample+=columnInterval)
					{
					*dataDoubleOutPtr = (double)ioFloatCharPtr[sample];
					dataDoubleOutPtr += dataPtrIncrement;
					
					}	// end "for (sample=0;  sample<numberChan..." 
					
						// Update the input buffer pointer to point to beginning of	
						// data for next channel.												
						
				ioFloatCharPtr = &ioFloatCharPtr[numberColumnsPerChannel];
				currentChannel++;
				
				}	// end "for (channel=0; channel<...)"
			
			}
			break;
			
		case k64BitTo64Bit: 
			{
			DoublePtr					dataDoublePtr;
			DoublePtr					ioDoublePtr;
		
			dataDoublePtr = (DoublePtr)dataCharBufferPtr;
			ioDoublePtr = (DoublePtr)ioCharBufferPtr;
			ioDoublePtr = &ioDoublePtr[columnOffset];
			
					// Loop through by number of channels.										
				
			for (channel=0; channel<numberBufferChannels; channel++)
				{
				if (channelListPtr != NULL)
					{
					while (channelListPtr[channel] != currentChannel)
						{
						ioDoublePtr = &ioDoublePtr[numberColumnsPerChannel];
						currentChannel++;
						
						}	// end "while (channelListPtr[channel] != currentChannel)"
						
					}	// end "if (channelListPtr != NULL)"
				
				if (toBISFormat)
					dataDoublePtr = (DoublePtr)&dataCharBufferPtr[channel*8];
					
				for (sample=0;  sample<numberSamples; sample+=columnInterval)
					{
					*dataDoublePtr = ioDoublePtr[sample];
					dataDoublePtr += dataPtrIncrement;
					
					}	// end "for (sample=0;  sample<numberChan..." 
					
						// Update the input buffer pointer to point to beginning of		
						// data for next channel.													
				
				ioDoublePtr = &ioDoublePtr[numberColumnsPerChannel];
				currentChannel++;
				
				}	// end "for (channel=0; channel<numberBufferChannels; channel++)"
					
			} 
			break;
		
		}	// end "switch (dataConversionCode)"

}	// end "PackNonBISData"  
 


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 PrepareToReadTextFile
//
//	Software purpose:	The purpose of this routine is to set up structure and parameters
//							to read a text file one line at a time.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	Error code for io operation.
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/10/2011
//	Revised By:			Larry L. Biehl			Date: 03/15/2017	

SInt16 PrepareToReadTextFile (
				HParamBlockRec*					paramBlockPtr,
				CMFileStream*		 				fileStreamPtr,
				UCharPtr								bufferPtr,
				UInt32								bufferLength,
				SInt16*								endOfLineCodePtr,
				UInt32*								numberEndOfLineBytesPtr)

{
	OSErr									errCode;
	
	
			// Read a section of the transformation file. 			
	
	#if defined multispec_mac
		paramBlockPtr->ioParam.ioPosOffset = 0;
		paramBlockPtr->ioParam.ioActCount = 0;
		paramBlockPtr->ioParam.ioRefNum = fileStreamPtr->refNum;
		paramBlockPtr->ioParam.ioCompletion = NULL;
		paramBlockPtr->ioParam.ioBuffer = (CharPtr)bufferPtr;
		paramBlockPtr->ioParam.ioReqCount = bufferLength - 1;	// Allow for \0.
		
				// Find the end of line character that is used in this file.
		
		paramBlockPtr->ioParam.ioPosMode = FindEndOfLineCode ((ParmBlkPtr)paramBlockPtr,
																				fileStreamPtr,
																				&errCode,
																				endOfLineCodePtr,
																				numberEndOfLineBytesPtr); 
	#endif	// defined multispec_mac
	
	#if defined multispec_win || defined multispec_wx
		paramBlockPtr->ioPosOffset = 0;
		paramBlockPtr->ioActCount = 0;
		paramBlockPtr->fileStreamPtr = fileStreamPtr;
		paramBlockPtr->ioBuffer = bufferPtr;
		paramBlockPtr->ioReqCount = bufferLength - 1;	// Allow for \0.  
		
				// Find the end of line character that is used in this file.
		
		paramBlockPtr->ioPosMode = FindEndOfLineCode ((ParmBlkPtr)paramBlockPtr,
																		fileStreamPtr,
																		&errCode,
																		endOfLineCodePtr,
																		numberEndOfLineBytesPtr);         
	#endif	// defined multispec_win || defined multispec_wx 
		
	return (errCode);

}	// end "PrepareToReadTextFile"
 


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 PutFile
//
//	Software purpose:	The purpose of this routine is to put up the
//							standard put file dialog box to allow the user to
//							select the file to write to.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	Error code for file operations.
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: ??/??/1988
//	Revised By:			Larry L. Biehl			Date: 02/18/2025

SInt16 PutFile (
				CMFileStream*		 				fileStreamPtr, 
				SInt64								numberBytes, 
				SInt16								stringIndex, 
				SInt32								creator)

{
	SInt16								errCode = noErr;
	

	#if defined multispec_mac 		
		if (gNavServicesExistsFlag)
			{
			errCode = NavServicesPutFile (fileStreamPtr, 
														numberBytes, 
														stringIndex, 
														creator);
	
																						return (errCode);
					
			}	// end "if (gNavServicesExistsFlag)"
	#endif	// defined multispec_mac
	
	#if defined multispec_win 
		TBYTE								filterString[512];
		WideFileStringPtr				wideFileNamePtr;
		SInt16							filterStringIndex = IDS_FilterString;
		Boolean							fileSelected;
		
		
		USES_CONVERSION;

		if (stringIndex == IDS_SaveProjectFile)
			filterStringIndex = IDS_ProjectFilterString;
		else if (stringIndex == IDS_FileIO61)
			filterStringIndex = IDS_TransformFilterString;
		else if (stringIndex == IDS_FileIO69)
			filterStringIndex = IDS_ImageFilterString;
		else if (stringIndex == IDS_FileIO75)
			filterStringIndex = IDS_ThematicFilterString;
		else if (stringIndex == IDS_SaveImageStats)
			filterStringIndex = IDS_StatFilterString;
		else if (stringIndex == IDS_SaveClassificationAs)
			filterStringIndex = IDS_ClassificationFilterString;
		else if (stringIndex == IDS_SaveProbabilityAs)
			filterStringIndex = IDS_ClassificationFilterString; 
			
		GetSpecifiedStringNumber (
								kFileIOStrID, filterStringIndex, filterString, TRUE, 510);
		CFileDialog  temp (FALSE,													// bOpenFileDialog
									NULL,													// lpszDefExt
									NULL,													// lpszFileName
									OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,	// dwFlags
									//(LPCTSTR)A2T((char*)&filterString[1]),	// lpszFilter
									&filterString[1],									// lpszFilter
									NULL,													// pParentWnd
									0,														// dwSize
									FALSE);												// bVistaStyle
		temp.m_ofn.nFilterIndex = 1; 
	
		fileSelected = FALSE;
				
		wideFileNamePtr =(LPTSTR)
					(WideFileStringPtr)fileStreamPtr->GetFileNameCPtr (kReturnUnicode);
			
		do	
			{
			gGetFileStatus = -1;
			if (stringIndex == IDS_SaveProjectFile)
				gGetFileStatus = 0;
				    
			CString newName = wideFileNamePtr;
			
			StringHandle		stringHandle = NULL;
			CharPtr				stringPtr = NULL;

			temp.m_ofn.lpstrFile = (LPTSTR)wideFileNamePtr;
			if (stringIndex == IDS_SaveProjectFile)
				{
				temp.m_ofn.lpstrDefExt = _T("prj");
				GetSpecifiedString (IDS_SaveProjectFile, &stringHandle, &stringPtr);
				temp.m_ofn.lpstrTitle = (LPTSTR)A2T(&stringPtr[1]);
				
				}	// end "if (stringIndex == IDS_SaveProjectFile)"
				
			else if (stringIndex == IDS_FileIO69)
				{
				temp.m_ofn.lpstrDefExt = _T("tif");
				GetSpecifiedString (IDS_FileIO69, &stringHandle, &stringPtr);
				temp.m_ofn.lpstrTitle = (LPTSTR)A2T(&stringPtr[1]);
				
				}	// end "if (stringIndex == IDS_FileIO69)"
				
			else if (stringIndex == IDS_FileIO61)
				{
				temp.m_ofn.lpstrDefExt = _T("tra");
				GetSpecifiedString (IDS_FileIO61, &stringHandle, &stringPtr);
				temp.m_ofn.lpstrTitle = (LPTSTR)A2T(&stringPtr[1]);
				
				}	// end "if (stringIndex == IDS_FileIO61)"
				
			else if (stringIndex == IDS_FileIO75)
				{
				temp.m_ofn.lpstrDefExt = _T("tra");
				GetSpecifiedString (IDS_FileIO75, &stringHandle, &stringPtr);
				temp.m_ofn.lpstrTitle = (LPTSTR)A2T(&stringPtr[1]);
				
				}	// end "if (stringIndex == IDS_FileIO75)"
				
			else if (stringIndex == IDS_SaveImageStats)
				{
				temp.m_ofn.lpstrDefExt = _T("sta");
				GetSpecifiedString (IDS_SaveImageStats, &stringHandle, &stringPtr);
				temp.m_ofn.lpstrTitle = (LPTSTR)A2T(&stringPtr[1]);
				
				}	// end "if (stringIndex == IDS_SaveImageStats)"
			
			else if (stringIndex == IDS_SaveClassificationAs)
				{
				temp.m_ofn.lpstrDefExt = _T("gis");
				GetSpecifiedString (IDS_SaveClassificationAs, &stringHandle, &stringPtr);
				temp.m_ofn.lpstrTitle = (LPTSTR)A2T(&stringPtr[1]);
				
				}	// end "if (stringIndex == IDS_SaveClassificationAs)"
				
			else if (stringIndex == IDS_SaveProbabilityAs)
				{
				temp.m_ofn.lpstrDefExt = _T("gis");
				GetSpecifiedString (IDS_SaveProbabilityAs, &stringHandle, &stringPtr);
				temp.m_ofn.lpstrTitle = (LPTSTR)A2T(&stringPtr[1]);
				
				}	// end "if (stringIndex == IDS_SaveProbabilityAs)"
				
			if (temp.DoModal () == IDCANCEL)			
																							return (-1);

			wideFileNamePtr = (WideFileStringPtr)temp.m_ofn.lpstrFile;
			
					// Get the path length.

			size_t stringLength = wcslen ((wchar_t*)wideFileNamePtr);

			if (stringLength == 1 || stringLength > gFileNameLengthLimit)
				DisplayAlert (kErrorAlertID, 
									kStopAlert, 
									kAlertStrID, 
									IDS_Alert92,
									0, 
									NULL);
				
			else	// stringLength <= gFileNameLengthLimit
				{
				TBYTE pathName[_MAX_PATH];              	   
				CtoPstring (wideFileNamePtr, pathName);
				pathName[pathName[0]+1] = 0;
				newName.ReleaseBuffer ();
		   		
		   	wideFileNamePtr = pathName;
				fileSelected = TRUE;
				fileStreamPtr->SetFilePath (wideFileNamePtr, TRUE);
				
				}	// end "else stringLength <= gFileNameLengthLimit"
						
			} while (!fileSelected);
			
				// Create the file on the selected volume										
				
		errCode = fileStreamPtr->MCreateNewFile (kReplaceFlag);
	#endif	// defined multispec_win
	
	#if defined multispec_wx
		WideFileStringPtr				wideFileNamePtr;
		Boolean							fileSelected;
	
	
		GetSpecifiedStringNumber (
								kFileIOStrID, stringIndex, (UCharPtr)gTextString, TRUE);
		wxString promptString ((char*)&gTextString[1], wxConvUTF8);
		
		wxFrame* frame = GetActiveFrame ();
		wxFileDialog temp (frame,
									//wxT("Save file as"), 
									promptString,
									wxEmptyString,
									wxEmptyString, 
									wxEmptyString,
									//wdcard,
									wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
			
				// Set default directory if needed.
		
		if (gOutputDirectory != wxEmptyString)
			temp.SetDirectory (gOutputDirectory);

		fileSelected = FALSE;
		wideFileNamePtr =
					(WideFileStringPtr)fileStreamPtr->GetFileNameCPtr (kReturnUnicode);

		do 
			{
			gGetFileStatus = -1;
			if (stringIndex == IDS_SaveProjectFile)
				gGetFileStatus = 0;

			StringHandle stringHandle = NULL;
			CharPtr stringPtr = NULL;
			temp.SetFilename (wideFileNamePtr);
			if (stringIndex == IDS_SaveProjectFile) 
				{
				GetSpecifiedString (IDS_SaveProjectFile, &stringHandle, &stringPtr);
				wxString title (&stringPtr[1], wxConvUTF8);
				temp.SetTitle (title);
				
				}	// end "if (stringIndex == IDS_SaveProjectFile)"

			else if (stringIndex == IDS_FileIO69) 
				{
				GetSpecifiedString (IDS_FileIO69, &stringHandle, &stringPtr);
				wxString title (&stringPtr[1], wxConvUTF8);
				temp.SetTitle (title);
				
				}	// end "if (stringIndex == IDS_FileIO69)"

			else if (stringIndex == IDS_FileIO61) 
				{
				GetSpecifiedString (IDS_FileIO61, &stringHandle, &stringPtr);
				wxString title (&stringPtr[1], wxConvUTF8);
				temp.SetTitle (title);
				
				}	// end "if (stringIndex == IDS_FileIO61)"
			  
			else if (stringIndex == IDS_FileIO75) 
				{
				GetSpecifiedString (IDS_FileIO75, &stringHandle, &stringPtr);
				wxString title (&stringPtr[1], wxConvUTF8);
				temp.SetTitle (title);
				
				}	// end "if (stringIndex == IDS_FileIO75)"

			else if (stringIndex == IDS_SaveImageStats) 
				{
				GetSpecifiedString (IDS_SaveImageStats, &stringHandle, &stringPtr);
				wxString title (&stringPtr[1], wxConvUTF8);
				temp.SetTitle (title);
				
				}	// end "if (stringIndex == IDS_SaveImageStats)"
				
			else if (stringIndex == IDS_SaveClassificationAs) 
				{
				GetSpecifiedString (IDS_SaveClassificationAs, &stringHandle, &stringPtr);
				wxString title (&stringPtr[1], wxConvUTF8);
				temp.SetTitle (title);
				
				}	// end "if (stringIndex == IDS_SaveClassificationAs)"
				
			else if (stringIndex == IDS_SaveProbabilityAs) 
				{
				GetSpecifiedString (IDS_SaveProbabilityAs, &stringHandle, &stringPtr);
				wxString title (&stringPtr[1], wxConvUTF8);
				temp.SetTitle (title);
				
				}	// end "if (stringIndex == IDS_SaveProbabilityAs)"
				
			else if (stringIndex == IDS_SaveImageStatisticsAs) 
				{
				GetSpecifiedString (IDS_SaveImageStatisticsAs, &stringHandle, &stringPtr);
				wxString title (&stringPtr[1], wxConvUTF8);
				temp.SetTitle (title);
				
				}	// end "if (stringIndex == IDS_SaveImageStatisticsAs)"
				
			int returnError = temp.ShowModal ();
			if (returnError == wxID_OK)
				{
				wcsncpy (wideFileNamePtr,
							temp.GetPath (),
							(int)(temp.GetPath ()).Length ());
				
						//Get file name length and add zero at the end
						
				int pathlength = (int)(temp.GetPath ()).Length ();
				wideFileNamePtr[pathlength] = 0;
				
						// Get the path length.

				if (pathlength > gFilePathNameLengthLimit)
					DisplayAlert (kErrorAlertID,
										kStopAlert,
										kAlertStrID,
										IDS_Alert92,
										0,
										NULL);

				else	// pathlength <= gFilePathNameLengthLimit
					{
					wchar_t pathName[_MAX_PATH];
					CtoPstring (wideFileNamePtr, pathName, _MAX_PATH-2);
					pathName[pathName[0]+1] = 0;

					wideFileNamePtr = pathName;

					fileSelected = TRUE;

					fileStreamPtr->SetFilePath (wideFileNamePtr, TRUE);

					}	// end "else stringLength <= gFilePathNameLengthLimit"
				
				}	// end "if (returnError == wxID_OK)"
			
			else
				errCode = -1;

			}	while (!fileSelected && errCode == noErr);
	
				// This is only set when needed to change output directory to a
				// writeable one. See CheckIfDirectoryIsWriteable routine.
	
		ResetOutputDirectory ();

				// Create the file on the selected volume

		if (errCode == noErr)
			errCode = fileStreamPtr->MCreateNewFile (kReplaceFlag);
	
	#endif	// defined multispec_wx

	return (errCode);

}	// end "PutFile"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ResetOutputDirectory
//
//	Software purpose:	The purpose of this routine is to set the gOutputDirectory
//							global wxString to empty. This routine is currently on used
//							in the wxWidgets versions of MultiSpec.
//							The CheckIfDirectoryIsWriteable routine will set this variable
//							if needed.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/12/2019
//	Revised By:			Larry L. Biehl			Date: 12/12/2019

void ResetOutputDirectory (void)

{
#if defined multispec_wx
	gOutputDirectory = wxEmptyString;
#endif

}	// end "ResetOutputDirectory"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ResolveAnyAliases
//
//	Software purpose:	The purpose of this routine is to resolve the input
//							file aliases if the input file is an alias.
//
//	Parameters in:		Pointer to AppFile structure which contains the
//											volume reference number and file name.
//							Index in the list
//
//	Parameters out:				
//
// Value Returned:	None	
// 
// Called By:			GetImageNameFromSupportFile in SFileIO.cpp
//							OpenFileReadOnly in SFileIO.cpp
//
//	Coded By:			Larry L. Biehl			Date: 09/08/1993
//	Revised By:			Larry L. Biehl			Date: 02/27/2018

SInt16 ResolveAnyAliases (
				CMFileStream*						fileStreamPtr, 
				Boolean*								wasAliasedFlagPtr)

{							
	SInt16								errCode = noErr;
	
	#if defined multispec_mac 
		Boolean								targetIsFolderFlag = FALSE;
		
		
		errCode = noErr;
		*wasAliasedFlagPtr = FALSE;
				
		if (gAliasMgrPresentFlag)
			{
			if (fileStreamPtr->fSRefFlag)
				{
				errCode = FSResolveAliasFile (&fileStreamPtr->fsRef,
													   	TRUE,
															&targetIsFolderFlag,
													   	wasAliasedFlagPtr);
				
				}	// end "if (fileStreamPtr->fSRefFlag)"
				
			else	// !fileStreamPtr->fSRefFlag
				{
						// Load a FSSpec record.													
						
				if (fileStreamPtr->fSSpecFlag)
					{
					gFileFSSpec.vRefNum = fileStreamPtr->vRefNum;
					gFileFSSpec.parID = fileStreamPtr->parID;
					CopyPToP (gFileFSSpec.name, fileStreamPtr->fileName);
					
					}	// end "if (fileInfoPtr->fSSpecFlag)" 
													
				else	// !fileInfoPtr->fSSpecFlag 
					errCode = FSMakeFSSpec (fileStreamPtr->vRefNum,
													0,
													fileStreamPtr->fileName,
													&gFileFSSpec);
					
				if (errCode == noErr)
					errCode = ResolveAliasFile (&gFileFSSpec,
															TRUE,
															&targetIsFolderFlag,
															wasAliasedFlagPtr);
				
				}	// end "else !fileStreamPtr->fSRefFlag"
			
			if (!targetIsFolderFlag && *wasAliasedFlagPtr && 
														(errCode == noErr || errCode == fnfErr))
				{						
				fileStreamPtr->vRefNum = gFileFSSpec.vRefNum;
				fileStreamPtr->parID = gFileFSSpec.parID;
				CopyPToP (fileStreamPtr->fileName, gFileFSSpec.name);
												
				fileStreamPtr->fSSpecFlag = TRUE;
				
				}	// end "if (!targetIsFolderFlag && wasAliasedFlag && ...)" 
			
			}	// end "if (gAliasMgrPresentFlag)"
	#endif	// defined multispec_mac 

   #if defined multispec_win || defined multispec_wx
		*wasAliasedFlagPtr = FALSE;
	#endif	// defined multispec_win || defined multispec_wx
		
	return (errCode);
			
}	// end "ResolveAnyAliases" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetCFileName
//
//	Software purpose:	The purpose of this routine is to set the file name in the
//							input file information structure to the new input c name.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 11/28/2001
//	Revised By:			Larry L. Biehl			Date: 04/12/2020

void SetCFileName (
				FileInfoPtr							fileInfoPtr,
				FileStringPtr						inputFileNameCPtr)

{  
	CMFileStream*						fileStreamPtr;
	
	
	if (fileInfoPtr != NULL && inputFileNameCPtr != NULL)
		{
		fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
		
		SetCFileName (fileStreamPtr, inputFileNameCPtr);
		
		}	// end "if (fileInfoPtr != NULL && fileNameCPtr != NULL)"
	
}	// end "SetCFileName" 



void SetCFileName (
				CMFileStream*						fileStreamPtr,
				FileStringPtr						inputFileNameCPtr)

{  
	FileStringPtr						fileNameCPtr,
											filePathPPtr;
	
	SInt32								characterOverRun,
											newNameLength,
											oldNameLength,
											oldPathNameLength;
	
	
	if (fileStreamPtr != NULL && inputFileNameCPtr != NULL)
		{
				// First make sure the current file is closed before changing
				// the name.
		
		CloseFile (fileStreamPtr);
		
				// Get the location of the start of the file name. We are assuming 
				// that the new file name is in the same folder as the previous file
				// name.
		
		fileNameCPtr = (FileStringPtr)GetFileNameCPointerFromFileStream (fileStreamPtr);
		filePathPPtr = (FileStringPtr)GetFilePathPPointerFromFileStream (fileStreamPtr);
		
		oldNameLength = (SInt32)strlen ((char*)fileNameCPtr);
		newNameLength = (SInt32)strlen ((char*)inputFileNameCPtr);
		oldPathNameLength = GetFileStringLength (filePathPPtr);
		
		
		characterOverRun = 
			oldPathNameLength - oldNameLength + newNameLength - gFilePathNameLengthLimit;
			
		if (characterOverRun > 0)
			newNameLength -= characterOverRun;
			
		newNameLength = MAX (newNameLength, 0);
		
		strncpy ((char*)fileNameCPtr, (char*)inputFileNameCPtr, newNameLength);

				// Make sure that there is a c terminator.
				
		fileNameCPtr[newNameLength] = 0;
		
				// Now update the character count.
				
		SetFileStringLength (filePathPPtr,
									strlen ((char*)&filePathPPtr[2]));

		}	// end "if (fileStreamPtr != NULL && fileNameCPtr != NULL)"
	
}	// end "SetCFileName"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetFileDoesNotExist
//
//	Software purpose:	This routine sets the information in the input file stream
//							structure (class) to indicate that a file has not been
//							defined yet.
//
//	Parameters in:		The file stream pointer			
//
//	Parameters out:	None			
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/24/1995
//	Revised By:			Larry L. Biehl			Date: 04/09/2020

void SetFileDoesNotExist (
				CMFileStream*						fileStreamPtr,
				UInt16								keepNameTypeCode)

{
			// Initialize for pascal and c strings.
			
	if (fileStreamPtr != NULL)
		{	
		#if defined multispec_mac
			if (keepNameTypeCode != kKeepUTF8CharName)
				{
				fileStreamPtr->fileName[0] = 0; 
				fileStreamPtr->fileName[1] = 0;
				}

			fileStreamPtr->uniFileName.length = 0; 
			fileStreamPtr->uniPascalFileName[0] = 0;
		#endif	// defined multispec_mac
		
      #if defined multispec_win || defined multispec_wx
			fileStreamPtr->mFileType = -1;
			if (keepNameTypeCode != kKeepUTF8CharName)
				{
				fileStreamPtr->mUTF8FilePathName[0] = 0;
				fileStreamPtr->mUTF8FilePathName[1] = 0;
				fileStreamPtr->mUTF8FilePathName[2] = 0;
				fileStreamPtr->mUTF8PathLength = 0;
				}

			fileStreamPtr->mWideFilePathName[0] = 0;
			fileStreamPtr->mWideFilePathName[1] = 0;
			fileStreamPtr->mWidePathLength = 0;

			fileStreamPtr->mUTF8FileName[0] = 0;
			fileStreamPtr->mUTF8FileName[1] = 0;
			fileStreamPtr->mUTF8FileName[2] = 0;
		#endif	// defined multispec_win
		
		}	// end "if (fileStreamPtr != NULL)"

}	// end "SetFileDoesNotExist"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetFileReadOnly
//
//	Software purpose:	This routine sets the given file to read only permission.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			ChangeErdasHeader in SReformatUtilities.cpp
//							ModifyChannelDescriptions in SReformatUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 08/29/1988
//	Revised By:			Larry L. Biehl			Date: 03/12/2008

void SetFileReadOnly (
				CMFileStream*						fileStreamPtr)

{
	SInt16								errCode;
	
	
			// Close the file																		
			// Set to read only permission and reopen										
		
	CloseFile (fileStreamPtr);
	
	errCode = OpenFileReadOnly (fileStreamPtr, 
											kDontResolveAliasChains, 
											kLockFile,
											kDoNotVerifyFileStream);	
														
	IOCheck (errCode, fileStreamPtr);
	
}	// end "SetFileReadOnly"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 SetFileSizeToZero
//
//	Software purpose:	The purpose of this routine is to set the size of the
//							file to '0'.  In other words make the file empty.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:			CreateNewFile in SFileIO.cpp
//							GetProjectFileName in SProject.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/26/1989
//	Revised By:			Larry L. Biehl			Date: 04/09/1996

SInt16 SetFileSizeToZero (
				CMFileStream*						fileStreamPtr)

{
	SInt16							errCode,
										errCode2;
										
	
	errCode = SetFileWriteEnabled (fileStreamPtr); 
		
	if (errCode == noErr) 
		errCode2 = MSetSizeOfFile (fileStreamPtr, 0, kErrorMessages);
		
	if (errCode == noErr)
		errCode = errCode2;
			
	if (errCode != noErr)
				// There is a file error.  Close the file and return error code.	
		CloseFile (fileStreamPtr); 
		
	return (errCode);
				
}	// end "SetFileSizeToZero" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SetFileWriteEnabled
//
//	Software purpose:	This routine sets the given file to write enabled permission.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			WriteErdasHeader in SSaveWrite.cpp
//							InsertNewErdasHeader in SReformatUtilities.cpp
//							ModifyChannelDescriptions in SReformatUtilities.cpp
//							SetFileSizeToZero in SFileIO.cpp
//
//	Coded By:			Larry L. Biehl			Date: 09/10/1992
//	Revised By:			Larry L. Biehl			Date: 02/09/2008

SInt16 SetFileWriteEnabled (
				CMFileStream* 						fileStreamPtr)

{
	SInt16								errCode;
	
	
			// First close the file
			
	CloseFile (fileStreamPtr);
		
	#if defined multispec_mac
		HFSUniStr255 						dataForkName;
																		
				// Now set to write permission and reopen	
		
		if (fileStreamPtr->fSSpecFlag)
			errCode = FSpOpenDF ((FSSpec*)fileStreamPtr, 
											fsWrPerm, 
											&fileStreamPtr->refNum);
													
		else if (fileStreamPtr->fSRefFlag)
			{
			errCode = FSGetDataForkName (&dataForkName);
			
			if (errCode == noErr)
				errCode = FSOpenFork (&fileStreamPtr->fsRef, 
												dataForkName.length, 
												dataForkName.unicode, 
												fsWrPerm, 
												&fileStreamPtr->refNum);
			
			}	// end "else if (fileStreamPtr->fSRefFlag)"
		
		else	// fileStreamPtr->fSSpecFlag 									
			errCode = HOpen (fileStreamPtr->vRefNum, 
									0, 
									fileStreamPtr->fileName, 
									fsWrPerm, 
									&fileStreamPtr->refNum);
																			
		IOCheck (errCode, (CharPtr)&fileStreamPtr->fileName);
	#endif	// defined multispec_mac
	
	#if defined multispec_win
				// Set to write permission and reopen
		
		errCode = fileStreamPtr->MOpenFile (
												(SInt16)(CFile::modeReadWrite|CFile::typeBinary),
												(SInt16)kErrorMessages);
	#endif	// defined multispec_win
	
	#if defined multispec_wx
				// Set to write permission and reopen

		errCode = fileStreamPtr->MOpenFile (
										(SInt16) (wxFile::read_write),(SInt16) kErrorMessages);
   #endif	// defined multispec_wx
										
	if (errCode == noErr)
		gNumberOfOpenFiles++;
	
	return (errCode);
	
}	// end "SetFileWriteEnabled"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetFSSpecFlag
//
//	Software purpose:	The purpose of this routine is to set the input file 
//							reference number.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 10/31/2002
//	Revised By:			Larry L. Biehl			Date: 10/30/2008

void SetFSSpecFlag (
				CMFileStream*						fileStreamPtr,
				Boolean								fSSpecFlag)

{  
	if (fileStreamPtr != NULL)
		{              
		#if defined multispec_mac                       
	   	fileStreamPtr->fSSpecFlag = fSSpecFlag;
			
					// If carbon version of code being created then fSSpecFlag is always
					// false and fSRefFlag is always true.
			fileStreamPtr->fSSpecFlag = FALSE;
			fileStreamPtr->fSRefFlag = TRUE;
		#endif	// defined multispec_mac 
	
		}	// end "if (fileStreamPtr != NULL)" 
	
}	// end "SetFSSpecFlag" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetParID
//
//	Software purpose:	The purpose of this routine is to set the input file 
//							reference number.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 10/31/2002
//	Revised By:			Larry L. Biehl			Date: 10/31/2002

void SetParID (
				CMFileStream*						fileStreamPtr,
				SInt32								parID)

{  
	if (fileStreamPtr != NULL)
		{              
		#if defined multispec_mac                       
	   	fileStreamPtr->parID = parID;
		#endif	// defined multispec_mac 
		
		}	// end "if (fileStreamPtr != NULL)" 
	
}	// end "SetParID" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetReferenceNumber
//
//	Software purpose:	The purpose of this routine is to set the input file 
//							reference number.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 10/25/1995
//	Revised By:			Larry L. Biehl			Date: 10/25/1995	

void SetReferenceNumber (
				CMFileStream*						fileStreamPtr,
				SInt16								refNum)

{  
	if (fileStreamPtr != NULL)
		{              
		#if defined multispec_mac                       
	   	fileStreamPtr->refNum = refNum;
		#endif	// defined multispec_mac 
		
		}	// end "if (fileStreamPtr != NULL)" 
	
}	// end "SetReferenceNumber" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetType
//
//	Software purpose:	The purpose of this routine is to set the input file 
//							type.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 10/26/1995
//	Revised By:			Larry L. Biehl			Date: 10/26/1995	

void SetType (
				CMFileStream*						fileStreamPtr,
				SInt32								type)

{  
	if (fileStreamPtr != NULL)
		{              
		#if defined multispec_mac                       
	   	fileStreamPtr->type = type;
		#endif	// defined multispec_mac 
	
		#if defined multispec_win || defined multispec_wx
	   	fileStreamPtr->mFileType = type;
		#endif	// defined multispec_win || defined multispec_wx
		
		}	// end "if (fileStreamPtr != NULL)" 
	
}	// end "SetType"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 SetUpFileIOInstructions
//
//	Software purpose:	This routine sets up the parameters in the file IO
//							instruction structure.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			BiPlotFieldData in SBiPlotData.cpp
//							ClassifyPerPointArea in SClassify.cpp
//							ClusterClassification in SCluster.cpp
//							CreateClusterMaskFile in SCluster.cpp
//							GetClusterAreaStatistics in SCluster.cpp
//							ISODATAClusterPass in SClusterIsodata.cpp
//							OnePassClusterAreas in SClusterSinglePass.cpp
//							ListFieldData in SListData.cpp
//							GetAreaResults in SListResults.cpp
//							GetAreaStats in SMatrixUtilities.cpp
//							GetFieldDataValues in SProjectMatrixUtilities.cpp
//							HistogramFieldStats in SProjectHistogramStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/21/1998
//	Revised By:			Larry L. Biehl			Date: 10/26/1999

SInt16 SetUpFileIOInstructions (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				AreaDescription* 					areaDescriptionPtr,
				UInt16								numberListChannels,
				HUInt16Ptr							channelListPtr,
				UInt16								specialBILCode)

{
	SInt32								errCode;
	

	errCode = SetUpFileIOInstructions (fileIOInstructionsPtr,
													areaDescriptionPtr,
													areaDescriptionPtr->lineStart,
													areaDescriptionPtr->lineEnd,
													areaDescriptionPtr->lineInterval,
													areaDescriptionPtr->columnStart,
													areaDescriptionPtr->columnEnd,
													areaDescriptionPtr->columnInterval,
													numberListChannels,
													channelListPtr,
													specialBILCode);
													
	return ((SInt16)errCode);
	
}	// end "SetUpFileIOInstructions"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 SetUpFileIOInstructions
//
//	Software purpose:	This routine sets up the parameters in the file IO
//							instruction structure.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			DisplayCImage in SDisplayMultispectral.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/13/1999
//	Revised By:			Larry L. Biehl			Date: 05/15/2024

SInt16 SetUpFileIOInstructions (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				AreaDescription* 					areaDescriptionPtr,
				UInt32								lineStart,
				UInt32								lineEnd,
				UInt32								lineInterval,
				UInt32								columnStart,
				UInt32								columnEnd,
				UInt32								columnInterval,
				UInt16								numberListChannels,
				HUInt16Ptr							channelListPtr,
				UInt16								specialBILCode)

{
	FileInfoPtr							fileInfoPtr;
	WindowInfoPtr						windowInfoPtr;
	
	SInt32								errCode = noErr;
	
	UInt32								gdalIndex,
											index;
	
	Boolean								bilFormatFlag,
											BILSpecialFlag,
											bisFormatFlag,
											bsqFormatFlag,
											callPackLineOfDataFlag,
											differentBuffersFlag,
											useMultipleChannelGDALFlag;
	
	
	if (fileIOInstructionsPtr == NULL)
																						return (noErr);
																					
	fileInfoPtr = fileIOInstructionsPtr->fileInfoPtr;
																								
			// Load Some of the File IO Instructions structure.
			
	fileIOInstructionsPtr->columnEnd = columnEnd;
	fileIOInstructionsPtr->columnInterval = columnInterval;
	fileIOInstructionsPtr->columnStart = columnStart;
	
	fileIOInstructionsPtr->lineEnd = lineEnd;
	fileIOInstructionsPtr->lineInterval = lineInterval;
	fileIOInstructionsPtr->lineStart = lineStart;
	
	fileIOInstructionsPtr->numberOutputBufferSamples = 
		(fileIOInstructionsPtr->columnEnd - fileIOInstructionsPtr->columnStart + 
			fileIOInstructionsPtr->columnInterval)/fileIOInstructionsPtr->columnInterval;
	
	fileIOInstructionsPtr->channelListPtr = channelListPtr;
	
	//if (channelListPtr != NULL && fileInfoPtr->gdalDataSetH != NULL)
	if (fileInfoPtr->gdalDataSetH != NULL)
		{
					// Need to create a channel list in format required by gdal routines.
		
		fileIOInstructionsPtr->gdalChannelListPtr = 
										(int*)MNewPointer (numberListChannels * sizeof (int));
		if (fileIOInstructionsPtr->gdalChannelListPtr != NULL)
			{
			if (channelListPtr != NULL)
				{
						// Note that the channel list for gdal routines needs to be in 
						// numerical order and not contain replicated channels. The list 
						// should already be in numerical order but duplicates may exist 
						// from the display processor. However, do not remove duplicates 
						// if the channels being used are from linked image files.
									
				useMultipleChannelGDALFlag = FALSE;
				if (channelListPtr[numberListChannels-1] < fileInfoPtr->numberChannels && 
																		fileInfoPtr->hdfHandle == NULL)
					useMultipleChannelGDALFlag = TRUE;
				
				fileIOInstructionsPtr->gdalChannelListPtr[0] = channelListPtr[0] + 1;
				gdalIndex = 1;
				for (index=1; index<numberListChannels; index++)
					{
					fileIOInstructionsPtr->gdalChannelListPtr[gdalIndex] = 
																			channelListPtr[index] + 1;
					if (!useMultipleChannelGDALFlag || 
								fileIOInstructionsPtr->gdalChannelListPtr[gdalIndex] !=
										fileIOInstructionsPtr->gdalChannelListPtr[gdalIndex-1])
						gdalIndex++;
					
					}	// end "for (index=1; index<numberListChannels; index++)"
					
				}	// end "if (channelListPtr != NULL)"
				
			else	// channelListPtr == NULL
				{
						// Create a list for the number of channels requested.
			
				for (index=0; index<numberListChannels; index++)
					fileIOInstructionsPtr->gdalChannelListPtr[index] = index + 1;
				
				gdalIndex = numberListChannels;
				
				}	// end "else channelListPtr == NULL"

			fileIOInstructionsPtr->numberGdalChannels = gdalIndex;
				
			}	// end "if (fileIOInstructionsPtr->gdalChannelListPtr != NULL)"
			
		else	// fileIOInstructionsPtr->gdalChannelListPtr == NULL
			errCode = 1;
				
		}	// end "if (channelListPtr != NULL && fileInfoPtr->gdalDataSetH != NULL)"
		
	else	// channelListPtr == NULL || ...
		fileIOInstructionsPtr->gdalChannelListPtr = NULL;
				
	fileIOInstructionsPtr->numberChannels = numberListChannels;
						
			// Set flag to indicate whether entire line of data for all 		
			// channels can be read in at one time, rather than separate 		
			// reads for each channel.														
		
	fileIOInstructionsPtr->oneReadFlag = GetBILSpecial (fileIOInstructionsPtr, 
																			channelListPtr, 
																			numberListChannels, 
																			columnStart,
																			columnEnd,
																			1,
																			specialBILCode);
	
			// Determine if the line of data will need to be packed.					
			
	callPackLineOfDataFlag = FALSE;
	differentBuffersFlag = FALSE;
	windowInfoPtr = fileIOInstructionsPtr->windowInfoPtr;
	BILSpecialFlag = fileIOInstructionsPtr->oneReadFlag;
	
	if (fileIOInstructionsPtr->forceByteCode == kForce2Bytes)
		{
		if (windowInfoPtr->localMaxNumberBytes == 1 || 
															windowInfoPtr->localBytesDifferFlag)
			{
			callPackLineOfDataFlag = TRUE;
			differentBuffersFlag = TRUE;
			
			}	// end "if (windowInfoPtr->localMaxNumberBytes == 1 || ...)" 
			
		}	// end "if (fileIOInstructionsPtr->forceByteCode == kForce2Bytes)" 
	
	else if (fileIOInstructionsPtr->forceByteCode == kForce4Bytes)
		{
		if (windowInfoPtr->localMaxNumberBytes <= 2 ||
															windowInfoPtr->localBytesDifferFlag)
			{
			callPackLineOfDataFlag = TRUE;
			differentBuffersFlag = TRUE;
			
			}	// end "if (windowInfoPtr->localMaxNumberBytes <= 2 || ...)" 
			
		}	// end "else if (fileIOInstructionsPtr->forceByteCode == kForce4Bytes)" 
	
	else if (fileIOInstructionsPtr->forceByteCode == kForceReal4Bytes)
		{
		if (windowInfoPtr->localMaxNumberBytes <= 2 || 
									windowInfoPtr->dataTypeCode == kIntegerType ||
															windowInfoPtr->localBytesDifferFlag)
			{
			callPackLineOfDataFlag = TRUE;
			differentBuffersFlag = TRUE;
			
			}	// end "if (windowInfoPtr->localMaxNumberBytes <= 2 || ...)" 
			
		}	// end "else if (fileIOInstructionsPtr->forceByteCode == kForceFloat8Bytes)"
	
	else if (fileIOInstructionsPtr->forceByteCode == kForceReal8Bytes)
		{
		if (windowInfoPtr->localMaxNumberBytes <= 4 || 
															windowInfoPtr->localBytesDifferFlag)
			{
			callPackLineOfDataFlag = TRUE;
			differentBuffersFlag = TRUE;
			
			}	// end "if (windowInfoPtr->localMaxNumberBytes <= 4 || ...)" 
			
		}	// end "else if (fileIOInstructionsPtr->forceByteCode == kForceFloat8Bytes)"
		
			// Determine if the file format(s) in the file (or linked file) ...
			// BIL, BSQ, and/or BIS formatted files.
			
	GetFileFormatsInFile (windowInfoPtr,
									fileInfoPtr,
									&bilFormatFlag,
									&bsqFormatFlag,
									&bisFormatFlag);
																
	//if (fileInfoPtr->bandInterleave != kBIS)
	if (bilFormatFlag || bsqFormatFlag)
		{
		if (fileIOInstructionsPtr->packDataFlag)
			{
			if (fileIOInstructionsPtr->columnInterval > 1)
				callPackLineOfDataFlag = TRUE;
			
			else if (BILSpecialFlag && fileIOInstructionsPtr->numberChannels < 
												fileIOInstructionsPtr->bilSpecialNumberChannels)
					callPackLineOfDataFlag = TRUE;
				
			}	// end "if (fileIOInstructionsPtr->packDataFlag)"
			
		if (fileIOInstructionsPtr->forceOutputFormatCode == kBIS)
			{
			callPackLineOfDataFlag = TRUE;
			differentBuffersFlag = TRUE;
			
			}	// end "if (fileIOInstructionsPtr->forceOutputFormatCode == kBIS)"
			
		}	// end "if (fileInfoPtr->bandInterleave != kBIS)" 				
	
	//else	// fileInfoPtr->bandInterleave == kBIS
	if (bisFormatFlag)
		{
		if (fileIOInstructionsPtr->packDataFlag && 
				(fileIOInstructionsPtr->numberChannels<fileInfoPtr->numberChannels || 
							fileIOInstructionsPtr->columnInterval > 1 ||
								fileIOInstructionsPtr->windowInfoPtr->numberImageFiles > 1 ||
									fileIOInstructionsPtr->forceOutputFormatCode == kBIL ||
										(fileInfoPtr->gdalDataSetH == 0 &&
											fileIOInstructionsPtr->forceOutputFormatCode != kBIS)))
			{
			callPackLineOfDataFlag = TRUE;
			
			if (fileIOInstructionsPtr->forceOutputFormatCode != kBIS)
				differentBuffersFlag = TRUE;
			
			}	// end "if (fileIOInstructionsPtr->packDataFlag && ..."
			
		}	// end "else fileInfoPtr->bandInterleave == kBIS" 
		
	fileIOInstructionsPtr->callPackLineOfDataFlag = callPackLineOfDataFlag;
	fileIOInstructionsPtr->differentBuffersFlag = differentBuffersFlag;
	
	if (areaDescriptionPtr != NULL && areaDescriptionPtr->maskInfoPtr != NULL)
		{
		fileIOInstructionsPtr->maskBufferPtr = 
								(HUInt16Ptr)GetHandlePointer (
													areaDescriptionPtr->maskInfoPtr->maskHandle,
													kLock);
								
		fileIOInstructionsPtr->maskBufferPtr += 
					(areaDescriptionPtr->maskLineStart - 1) * 
										(areaDescriptionPtr->maskInfoPtr->numberColumns + 1);
											
		fileIOInstructionsPtr->maskColumnStart = areaDescriptionPtr->maskColumnStart;
											
		fileIOInstructionsPtr->maskValueRequest = areaDescriptionPtr->maskValueRequest;
		
				// Get the number of columns in the mask buffer to skip to go from
				// one line to the next. Allow for a line interval other than 1.
				
		fileIOInstructionsPtr->numberMaskColumnsPerLine = 
										areaDescriptionPtr->maskInfoPtr->numberColumns + 1;
				
		fileIOInstructionsPtr->numberMaskColumnsPerLine *= 
																areaDescriptionPtr->lineInterval;
		
		}	// end "if (areaDescriptionPtr != NULL && ..."
		
			// Set up heirarchal tile parameters, if needed.
		
	SetUpHeirarchalFileIOParameters (fileIOInstructionsPtr->layerInfoPtr,
												fileIOInstructionsPtr->fileInfoPtr,
												(UInt16)fileIOInstructionsPtr->numberChannels,
												(UInt16*)fileIOInstructionsPtr->channelListPtr,
												columnStart,
												columnEnd);
		
			// Set flag indicating whether Threaded IO can be used.
			
	gUseThreadedIOFlag = gHasThreadManager && fileIOInstructionsPtr->oneReadFlag;

	if (errCode == noErr)
		errCode = SetupFileIOThread (fileIOInstructionsPtr);
	
	return ((SInt16)errCode);
	
}	// end "SetUpFileIOInstructions"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 SetUpGeneralFileIOInstructions
//
//	Software purpose:	This routine sets up the parameters in the file IO
//							instruction structure.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 10/14/1999
//	Revised By:			Larry L. Biehl			Date: 05/06/2022

void SetUpGeneralFileIOInstructions (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				WindowInfoPtr						windowInfoPtr,
				LayerInfoPtr						layerInfoPtr,
				FileInfoPtr							fileInfoPtr,
				UInt16								numberListChannels,
				HUInt16Ptr							channelListPtr,
				HUCharPtr							inputBufferPtr,
				HUCharPtr							outputBufferPtr,
				HUCharPtr							tiledBufferPtr,
				UInt32								bufferOffset,
				Boolean								packDataFlag,
				UInt16								forceOutputFormatCode,	// forceBISFormatFlag,
				UInt16								forceOutByteCode,
				FileIOInstructionsPtr*			outputFileIOInstructionsPtrPtr)
							
{
	SInt16								returnCode;
	
	
	fileIOInstructionsPtr->windowInfoPtr = windowInfoPtr;
	fileIOInstructionsPtr->fileInfoPtr = fileInfoPtr;
	fileIOInstructionsPtr->layerInfoPtr = layerInfoPtr;
	
	fileIOInstructionsPtr->channelListPtr = channelListPtr;
	fileIOInstructionsPtr->numberChannels = numberListChannels;
	fileIOInstructionsPtr->channelListIndexStart = 0;
	fileIOInstructionsPtr->channelStart = 0;
	fileIOInstructionsPtr->channelEnd = 0;
	
	fileIOInstructionsPtr->inputBufferPtrs[0] = inputBufferPtr;
	fileIOInstructionsPtr->outputBufferPtrs[0] = outputBufferPtr;
	
	if (inputBufferPtr != NULL)
		fileIOInstructionsPtr->inputBufferPtrs[1] = &inputBufferPtr[bufferOffset];
	
	if (outputBufferPtr != NULL)
		fileIOInstructionsPtr->outputBufferPtrs[1] = &outputBufferPtr[bufferOffset];
	
	fileIOInstructionsPtr->tiledBufferPtrs[0] = tiledBufferPtr;
	
	fileIOInstructionsPtr->bufferOffset = bufferOffset;
	
	fileIOInstructionsPtr->forceByteCode = forceOutByteCode;
	fileIOInstructionsPtr->forceOutputFormatCode = forceOutputFormatCode;
	//fileIOInstructionsPtr->forceBISFormatFlag = forceBISFormatFlag;
	fileIOInstructionsPtr->packDataFlag = packDataFlag;
	
	fileIOInstructionsPtr->callPackLineOfDataFlag = FALSE;
	fileIOInstructionsPtr->differentBuffersFlag = FALSE;
	
	returnCode = SetUpDataConversionCode (windowInfoPtr,
														layerInfoPtr,
														fileInfoPtr,
														numberListChannels,
														channelListPtr,	
														forceOutByteCode);
	
	if (tiledBufferPtr != NULL)
		GetSetTiledIOBufferBytes (layerInfoPtr,
											fileInfoPtr,  
											(UInt16*)channelListPtr,
											(UInt16)numberListChannels,
											tiledBufferPtr);	

	GetHierarchalFilePointers (windowInfoPtr, fileInfoPtr);
	
	GetHDF_FilePointers (windowInfoPtr, fileInfoPtr);
	
	*outputFileIOInstructionsPtrPtr = fileIOInstructionsPtr;
	
}	// end "SetUpGeneralFileIOInstructions"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 SetUpDataConversionCode
//
//	Software purpose:	This routine sets up the parameters in the file IO
//							instruction structure.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			SetUpGeneralFileIOInstructions
//
//	Coded By:			Larry L. Biehl			Date: 12/16/2005
//	Revised By:			Larry L. Biehl			Date: 02/28/2022

SInt16 SetUpDataConversionCode (
				WindowInfoPtr						windowInfoPtr,
				LayerInfoPtr						layerInfoPtr,
				FileInfoPtr							fileInfoPtr,
				UInt16								numberListChannels,
				HUInt16Ptr							channelListPtr,	
				UInt16								forceOutByteCode)
							
{  
	FileInfoPtr							localFileInfoPtr;
	
	SInt32								fileInfoIndex,
											layerFileInfoIndex;
	
	UInt32								channel,
											index;
											
	SInt16								returnCode = 0;
	
	Boolean								signedDataFlag;
	
		
	fileInfoIndex = -1;
	channel = 0;
	for (index=1; index<=numberListChannels; index++)
		{										
		if (channelListPtr != NULL)
			channel = channelListPtr[index-1];
			
		channel++;
		
		layerFileInfoIndex = 0;
		if (layerInfoPtr != NULL)
			layerFileInfoIndex = layerInfoPtr[channel].fileInfoIndex;
			
		if (fileInfoIndex != layerFileInfoIndex)
			{
			fileInfoIndex = layerFileInfoIndex;
			localFileInfoPtr = &fileInfoPtr[fileInfoIndex];
			
			signedDataFlag = localFileInfoPtr->signedDataFlag;
			if (localFileInfoPtr->numberBytes <= 2 && gConvertSignedDataFlag)
				signedDataFlag = FALSE;
				
			localFileInfoPtr->dataConversionCode = GetDataConversionCode (
																	localFileInfoPtr->dataTypeCode,
																	localFileInfoPtr->numberBytes,
																	signedDataFlag,
																	forceOutByteCode,
																	signedDataFlag,
																	NULL);
			
			if (localFileInfoPtr->dataConversionCode == 0)
				returnCode = -1;
						
			}	// end "if (fileInfoIndex != ..." 
			
		}	// end "for (index=1; index<=numberChannels; index++)" 
		
	return (returnCode);
	
}	// end "SetUpDataConversionCode"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetUpHeirarchalFileIOParameters
//
//	Software purpose:	The purpose of this routine is to set the parameter in the 
//							heirarchal file io structure that indicates which block are to
//							be read from the disk file that will include the input area.
//
//	Parameters in:				
//
//	Parameters out:			
//
//	Value Returned:
//
// Called By:			SetUpFileIOInstructions in SFileIO.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/14/1999
//	Revised By:			Larry L. Biehl			Date: 12/17/2009

void SetUpHeirarchalFileIOParameters (
				LayerInfoPtr						layerInfoPtr,
				FileInfoPtr							fileInfoPtr,
				UInt16								numberChannels,
				UInt16*								channelListPtr,
				UInt32								columnStart, 
				UInt32								columnEnd) 

{
	ldiv_t 								ratio;
	
	FileInfoPtr							localFileInfoPtr;
	
	HierarchalFileFormatPtr			hfaPtr,
											localHfaPtr;
	
	SInt32								fileInfoIndex,
											nextFileInfoIndex;
	
	UInt32								blockColumnEnd,
											blockColumnStart,
											channelNumber,
											fileChannelNumber,
											index;
											
	Boolean								bandInterleaveByBlockFlag;		// Currently not used.
											
	
	if (fileInfoPtr != NULL)
		{
		fileChannelNumber = 0;
		fileInfoIndex = -1;
		
		for (index=1; index<=numberChannels; index++)
			{
			channelNumber = index;								
			if (channelListPtr != NULL)
				channelNumber = channelListPtr[index-1] + 1;
			
			nextFileInfoIndex = 0;
			fileChannelNumber++;
			if (layerInfoPtr != NULL)
				{
				nextFileInfoIndex = layerInfoPtr[channelNumber].fileInfoIndex;
				fileChannelNumber = layerInfoPtr[channelNumber].fileChannelNumber;
				
				}	// end "if (layerInfoPtr != NULL)"
				
			if (fileInfoIndex != nextFileInfoIndex)
				{
				fileInfoIndex = nextFileInfoIndex;
				localFileInfoPtr = &fileInfoPtr[fileInfoIndex];
					
						// Get pointer to the hfa structure.
						
				hfaPtr = (HierarchalFileFormatPtr)GetHandlePointer (
																	localFileInfoPtr->hfaHandle);
														
				bandInterleaveByBlockFlag = FALSE;
				
				}	// end "if (if (fileInfoIndex != layerInfoPtr...)"
			
			if (hfaPtr != NULL)
				{
				if (localFileInfoPtr->bandInterleave == kBIBlock ||
													localFileInfoPtr->bandInterleave == kBIS)
					{
					fileChannelNumber = 1;
					bandInterleaveByBlockFlag = TRUE;
					
					}	// end "if (localFileInfoPtr->bandInterleave == kBIBlock || ..."
			
				localHfaPtr = &hfaPtr[fileChannelNumber-1];
				
				if (localHfaPtr->blockWidth > 0)
					{
					blockColumnStart = (columnStart-1)/localHfaPtr->blockWidth;
				
					localHfaPtr->firstLineRead = 0;
					localHfaPtr->lastLineRead = 0;	
					localHfaPtr->firstColumnStartByte =  
											(UInt32)(localHfaPtr->layerOffsetBytes + 
													localHfaPtr->blockOffset * blockColumnStart);
					
					ratio = ldiv (columnEnd, localHfaPtr->blockWidth);
					blockColumnEnd = ratio.quot;
					if (ratio.rem > 0)
						blockColumnEnd++;
												
					localHfaPtr->firstColumnRead = 
												localHfaPtr->blockWidth * blockColumnStart + 1;
					localHfaPtr->lastColumnRead = 
												localHfaPtr->blockWidth * blockColumnEnd;
					localHfaPtr->numberBlocksRead = blockColumnEnd - blockColumnStart;
					localHfaPtr->blockSizeRead = localHfaPtr->blockSize;
					localHfaPtr->blockWidthRead = localHfaPtr->blockWidth;
					
					if (localFileInfoPtr->gdalDataSetH != NULL)
						localHfaPtr->blockWidthRead = columnEnd - columnStart + 1;
					
					}	// end "if (localHfaPtr->blockWidth > 0)"
				
				}	// end "if (hfaPtr != NULL)"
				
			}	// end "for (index=0; index<numberChannels; index++)"
			
		}	// end "if (fileInfoPtr != NULL)"
	
}	// end "SetUpHeirarchalFileIOParameters"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 SetVolume
//
//	Software purpose:	This routine sets the default volume.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			GetProjectFileName in SProject.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/25/1995
//	Revised By:			Larry L. Biehl			Date: 12/19/1997

SInt16 SetVolume (
				CMFileStream* 						fileStreamPtr,
				SInt16								messageCode)

{
	SInt16								errCode = noErr;
	
	
	if (fileStreamPtr != NULL)
		{	
		#if defined multispec_mac
			if (fileStreamPtr->fSSpecFlag)
				errCode = HSetVol (NULL, fileStreamPtr->vRefNum, fileStreamPtr->parID);  
					
			if (messageCode == kErrorMessages)		
				IOCheck (errCode, (CharPtr)NULL);            
		#endif	// defined multispec_mac
		}	// end "if (fileStreamPtr != NULL)"
	
	return (errCode);
	
}	// end "SetVolume"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetVolumeReference
//
//	Software purpose:	The purpose of this routine is to set the output file 
//							volume reference values to those in the input file
//							stream.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 12/19/1997
//	Revised By:			Larry L. Biehl			Date: 03/02/2009	

void SetVolumeReference (
				CMFileStream*						inputFileStreamPtr,
				CMFileStream*						outputFileStreamPtr)

{  
	if (inputFileStreamPtr != NULL && outputFileStreamPtr != NULL)
		{              
		#if defined multispec_mac                       
		      		// Copying all below will handle for cases of a FSSpec record
		      		// and a non-FSSpec record (pre-FSSpec) and FSRef record
						// (post FSSpec).
		      		
		   	outputFileStreamPtr->vRefNum = inputFileStreamPtr->vRefNum;
		   	outputFileStreamPtr->parID = inputFileStreamPtr->parID;
		   	outputFileStreamPtr->fSSpecFlag = inputFileStreamPtr->fSSpecFlag;
				
				outputFileStreamPtr->fSRefFlag = inputFileStreamPtr->fSRefFlag;
				outputFileStreamPtr->parentFSRef = inputFileStreamPtr->parentFSRef;
		#endif	// defined multispec_mac
		
		}	// end "if (inputFileStreamPtr != NULL && outputFileStreamPtr != NULL)" 
	
}	// end "SetVolumeReference"    



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SwapBytes
//
//	Software purpose:	The purpose of this routine is to swap the bytes
//							in the input buffer.
//
//	Parameters in:		Number of bytes per data value
//							Pointer to buffer of data
//							Number of samples in the buffer
//
//	Parameters out:	None
//
//	Value Returned:  
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/14/2006
//	Revised By:			Larry L. Biehl			Date: 04/14/2006

void SwapBytes (
				SInt16								numberBytes,
				HUInt8Ptr							fileIOBufferPtr,
				UInt32								numberSamples)
								
{	
	if (numberBytes == 2)
		Swap2Bytes ((HUInt16Ptr)fileIOBufferPtr,
						numberSamples);
						
	else if (numberBytes == 4)
		Swap4Bytes ((HUInt32Ptr)fileIOBufferPtr,
						numberSamples);
						
	else if (numberBytes == 8)
		Swap8Bytes ((DoublePtr)fileIOBufferPtr,
						numberSamples);  
	
}	// end "SwapBytes"    



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void Swap2Bytes
//
//	Software purpose:	The purpose of this routine is to swap the bytes
//							in the input buffer.
//
//	Parameters in:		Pointer to buffer of two-byte data
//							Number of samples in the buffer
//
//	Parameters out:	None
//
//	Value Returned:  
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/20/1994
//	Revised By:			Larry L. Biehl			Date: 04/28/1995

void Swap2Bytes (
				HUInt16Ptr							twoBytePtr,
				UInt32								numberSamples)
								
{	
	UInt32								sampleCount;
	
		
	for (sampleCount=0; sampleCount<numberSamples; sampleCount++)
		{
		*twoBytePtr = ((*twoBytePtr & 0xff00) >> 8) | ((*twoBytePtr & 0x00ff) << 8);
		twoBytePtr++;
				
		}	// end "for (sampleCount=0;  ..." 
	
}	// end "Swap2Bytes"    



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void Swap4Bytes
//
//	Software purpose:	The purpose of this routine is to swap the bytes
//							in the input buffer.
//
//	Parameters in:		Pointer to buffer of four-byte data
//							Number of samples in the buffer
//
//	Parameters out:	None
//
//	Value Returned:  
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 06/18/2003
//	Revised By:			Larry L. Biehl			Date: 06/18/2003

void Swap4Bytes (
				HUInt32Ptr							fourBytePtr,
				UInt32								numberSamples)
								
{	
	UInt32								sampleCount;
	
		
	for (sampleCount=0; sampleCount<numberSamples; sampleCount++)
		{
		*fourBytePtr = ((*fourBytePtr & 0xff000000) >> 24) | 
								((*fourBytePtr & 0x00ff0000) >>  8) | 
									((*fourBytePtr & 0x0000ff00) <<  8) | 
										((*fourBytePtr & 0x000000ff) << 24);
		fourBytePtr++;
				
		}	// end "for (sampleCount=0;  ..." 
	
}	// end "Swap4Bytes"    



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void Swap8Bytes
//
//	Software purpose:	The purpose of this routine is to swap the bytes
//							in the input buffer.
//
//	Parameters in:		Pointer to buffer of eight-byte data
//							Number of samples in the buffer
//
//	Parameters out:	None
//
//	Value Returned:  
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 06/18/2003
//	Revised By:			Larry L. Biehl			Date: 07/16/2003

void Swap8Bytes (
				DoublePtr							eightBytePtr,
				UInt32								numberSamples)
								
{	
	HUInt32Ptr							fourBytePtr;
				
	UInt32								sampleCount,
											temp4Byte;
	
		
	fourBytePtr = (HUInt32Ptr)eightBytePtr;
	for (sampleCount=0; sampleCount<numberSamples; sampleCount++)
		{
		temp4Byte = ((*fourBytePtr & 0xff000000) >> 24) | 
								((*fourBytePtr & 0x00ff0000) >>  8) | 
									((*fourBytePtr & 0x0000ff00) <<  8) | 
										((*fourBytePtr & 0x000000ff) << 24);
		
		fourBytePtr[0] = ((fourBytePtr[1] & 0xff000000) >> 24) | 
									((fourBytePtr[1] & 0x00ff0000) >>  8) | 
										((fourBytePtr[1] & 0x0000ff00) <<  8) | 
											((fourBytePtr[1] & 0x000000ff) << 24);;
		
		fourBytePtr[1] = temp4Byte;
								
		fourBytePtr += 2;
				
		}	// end "for (sampleCount=0;  ..." 
	
}	// end "Swap8Bytes"    



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 UpdateFileNameInformation
//
//	Software purpose:	The purpose of this routine is to make sure the file stream
//							structure is ready to be used to open the file.  It is used
//							after input a new file name that is to be opened. It will
//							verify that both the unicode and multibyte path strings are
//							are available.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 03/13/2017
//	Revised By:			Larry L. Biehl			Date: 07/05/2017

SInt16 UpdateFileNameInformation (
				CMFileStream*						fileStreamPtr,
				FileStringPtr						fileNamePtr)

{  
	SInt16								errCode = noErr;
	
	
	#if defined multispec_mac
		CFStringRef			cfStringRef;
		
		
		if (fileStreamPtr->uniFileName.length == 0 && fileStreamPtr->fileName[0] > 0)
			{
					// Need to create the unicode formatted name.
					
			cfStringRef = CFStringCreateWithBytes (
												kCFAllocatorDefault,
												(UInt8*)&fileStreamPtr->fileName[1],
												fileStreamPtr->fileName[0],
												kCFStringEncodingUTF8,
												false);								
												
			if (cfStringRef != NULL)
				{
				CFIndex usedBufferLength;
				
				fileStreamPtr->uniFileName.length = CFStringGetLength (cfStringRef);
				CFStringGetBytes (cfStringRef, 
										CFRangeMake (0, fileStreamPtr->fileName[0]),
										kCFStringEncodingUnicode, 
										'?', 
										false, 
										(UInt8*)fileStreamPtr->uniFileName.unicode, 
										512, 
										&usedBufferLength);
										
				//CFAllocatorDeallocate (kCFAllocatorDefault, (void*)cfStringRef);
				CFRelease (cfStringRef);
										
				}	// end "if (cfStringRef != NULL)"
			
			else
				errCode = 2;
			
					// Make sure pathLength is 0
			
			fileStreamPtr->pathLength = 0;
			
			}	// end "if (fileStreamPtr->uniFileName.length == 0 && ..."
	
		else if (fileStreamPtr->uniFileName.length == 0 && fileStreamPtr->fileName[0] == 0)
			errCode = 3;
		/*
		else	// fileStreamPtr->wideCharFileName[0] > 0 || fileStreamPtr->fileName[0] == 0
			{
			if (fileStreamPtr->uniFileName.length == 0)
						// Copy wide character file name string to unichar name string.
				CopyWideStringToUnicodeStringInFileStream (fileStreamPtr);
			
					// Now get the UTF8 formatted file name.
																
			cfStringRef = CFStringCreateWithCharacters (
												kCFAllocatorDefault,
												fileStreamPtr->uniFileName.unicode,
												fileStreamPtr->uniFileName.length);
												
			if (cfStringRef != NULL)
				{
				CFStringGetCString (cfStringRef,
										(char*)&fileStreamPtr->fileName[1],
										(CFIndex)255,
										kCFStringEncodingUTF8);
				fileStreamPtr->fileName[0] = strlen ((char*)&fileStreamPtr->fileName[1]);

				//CFAllocatorDeallocate (kCFAllocatorDefault, (void*)cfStringRef);
				CFRelease (cfStringRef);
													
				}	// end "if (cfStringRef != NULL)"
			
			else
				errCode = 2;
				
			}	// end "else fileStreamPtr->wideCharFileName[0] > 0 || ..."
		*/
	#endif	// defined multispec_mac
			  
	#if defined multispec_wx || defined multispec_win
				// for Windows and Linux versions, now copy the proposed file name to the
				// file path string. 

		fileStreamPtr->SetFileName (fileNamePtr);
	#endif	// defined multispec_wx || defined multispec_win

	return (errCode);
						
}	// end "UpdateFileNameInformation"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void VerifyFileStreamForOpen
//
//	Software purpose:	The purpose of this routine is to make sure the file stream
//							structure is ready to be used to open the file.  It is used
//							after input a new file name that is to be opened.  It is specifically
//							used for fsRef type reads.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 02/27/2008
//	Revised By:			Larry L. Biehl			Date: 07/06/2017

SInt16 VerifyFileStreamForOpen (
				CMFileStream*						fileStreamPtr,
				FileStringPtr						fileNamePtr)

{  
	SInt16								errCode = noErr;
	
	#if defined multispec_mac
		errCode = UpdateFileNameInformation (fileStreamPtr, NULL);
	
		if (fileStreamPtr->fSRefFlag)
			{
			if (gHasHFSPlusAPIs)
				{

				if (errCode == noErr)
					{
					errCode = FSMakeFSRefUnicode (&fileStreamPtr->parentFSRef,  
																fileStreamPtr->uniFileName.length,
																fileStreamPtr->uniFileName.unicode,
																kTextEncodingUnicodeDefault, 
																&fileStreamPtr->fsRef);
													
					}	// end "if (errCode == noErr)"
					
				if (errCode == noErr)
					errCode = FSGetCatalogInfo (&fileStreamPtr->fsRef,
															kFSCatInfoNone,
															NULL,
															&fileStreamPtr->uniFileName,
															NULL,
															NULL);
															
				}	// end "if (gHasHFSPlusAPIs)"
			
			else	// !gHasHFSPlusAPIs
				errCode = 1;
				
			}	// end "if (fileStreamPtr->fSRefFlag)"
	#endif	// defined multispec_mac

	#if defined multispec_wx || defined multispec_win
				// Create UTF8 character formatted file path name string

		UpdateFileNameInformation (fileStreamPtr, fileNamePtr);
	#endif	// defined multispec_wx || defined multispec_win
   	
   return (errCode);
	
}	// end "VerifyFileStreamForOpen" 
