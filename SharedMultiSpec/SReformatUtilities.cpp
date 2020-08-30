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
//	File:						SReformatUtilities.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			08/29/2020
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	 
//
//
/* Template for debugging
	int numberChars = sprintf ((char*)gTextString3,
											" SReform2::xxx (entered routine. %s", 
											gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);	
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
	
#if defined multispec_mac || defined multispec_mac_swift
	#define IDC_ChannelNumber		4
	#define IDC_Description			8
	#define IDC_Value					10
#endif	// defined multispec_mac || defined multispec_mac_swift
                             
#if defined multispec_win
	#include "SImageWindow_class.h"
	#include "WAppendFileDialog.h"
	#include "WChangeChannelDescriptionDialog.h"
	#include "WReformatChangeHeaderDialog.h"
#endif	// defined multispec_win 
 
#if defined multispec_wx
	#include "SImageWindow_class.h"
   #include "xChangeChannelDescriptionDialog.h"
#endif	// defined multispec_wx



			// Prototype descriptions for routines in this file that are only
			// called by routines in this file.
															
SInt16	 AppendFile (
				FileInfoPtr 						fileInfoPtr, 
				SInt16								numberTypes, 
				OSType*								fileTypesPtr, 
				SInt64								numberBytes, 
				Boolean								modifyFlag,
				Boolean*								writeChannelDescriptionsFlagPtr,
				SInt64*								modifyAppendBytesSkipPtr);
							
SInt16 	AppendFileDialog (
				FileInfoPtr							appendFileInfoPtr, 
				FileInfoPtr							newFileInfoPtr, 
				Boolean								modifyFlag,
				SInt64*								bytesToSkipPtr);
							
void 		AppendFileDialogOK (
				FileInfoPtr							appendFileInfoPtr,
				FileInfoPtr							newFileInfoPtr,
				SInt16								lineColumnCode,
				Boolean								lineFlag,
				Boolean								modifyFlag,
				SInt32								newAfterLineChannel,
				SInt64*								bytesToSkipPtr);

void 		ChangeErdasHeader (void);
										
Boolean	ChangeErdasHeaderDialog (
				FileInfoPtr							fileInfoPtr, 
				SInt16*								requestedFormatPtr);
							
void		GetOutputFileName (
				FileInfoPtr							inputFileInfoPtr,
				FileStringPtr						outFileNamePtr);

Boolean 	InsertNewErdasHeader (
				FileInfoPtr							fileInfoPtr, 
				SInt16								requestedFormat);
	
void		LoadCurrentHeaderParametersInDialog (
				DialogPtr							dialogPtr, 
				FileInfoPtr							fileInfoPtr);

void 		LoadDescriptionIntoDItem (
				DialogPtr 							dialogPtr, 
				SInt16	 							itemNumber, 
				ChannelDescriptionPtr 			channelDescriptionPtr);

void 		ModifyChannelDescriptions (
				SInt16								requestedProcedure);

void		ModifyChannelDescriptionsViaDiskFile (void);

SInt16	ModifyChannelDescriptionsViaKeyboard (
				FileInfoPtr 						fileInfoPtr, 
				ChannelDescriptionPtr 			channelDescriptionPtr, 
				float* 								channelValuesPtr);
												
SInt16	WriteChannelDescriptions (
				FileInfoPtr							fileInfoPtr, 
				SInt16*								channelPtr,
				UInt32								numberChannels,
				Boolean								appendToFileFlag);
												
SInt16	WriteChannelValues (
				FileInfoPtr							fileInfoPtr, 
				SInt16*								channelPtr,
				UInt32								numberChannels,
				Boolean								appendToFileFlag);


                       
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 AppendFile
//
//	Software purpose:	The purpose of this routine is to put up the
//							standard get file dialog box to allow the user to
//							select the file that is to be appended.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	Error code for file operations.
//
// Called By:			GetReformatOutputFile in SReformatUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/06/1991
//	Revised By:			Larry L. Biehl			Date: 08/29/2020

SInt16 AppendFile (
				FileInfoPtr 						fileInfoPtr, 
				SInt16								numberTypes, 
				OSType*								fileTypesPtr, 
				SInt64								numberBytes, 
				Boolean								modifyFlag,
				Boolean*								writeChannelDescriptionsFlagPtr,
				SInt64*								modifyAppendBytesSkipPtr)

{   
	SInt64								bytesNeeded,
											bytesToSkip,
											freeBytes,
											logicalEndOfFile;
	
	Str31									blankString;
	            
	char									*fileNamePtr;
	
	ChannelDescriptionPtr			appendToDescriptionPtr,
											descriptionPtr;
									
	CMFileStream						*appendToFileStreamPtr,
											*fileStreamPtr;  
	
	FileInfoPtr							appendToFileInfoPtr;
	
	float									*appendToChannelValuesPtr,
											*channelValuesPtr;
									
	Handle								appendToFileInfoHandle;
	
	UInt32								appendToFileNumberChannels,
											index,
											inputNumberChannels; 
	
	SInt16								errCode,
											returnCode,
											stringIndex,
											vRefNum;  
	                                          
	Boolean								continueFlag,
											fileInfoLoadedFlag,
											fileSelected;
	
		
			// Make certain that there is enough memory for the SFGetFile			
			// routine to run in.																
			
	if (!CheckIfMemoryAvailable (15000))
																								return (1);
	
			// Get handle for append file information.
						
	appendToFileInfoHandle = InitializeFileInfoStructure (NULL, kNotPointer);
	if (appendToFileInfoHandle == NULL)
																								return (1);
	
			// Initialize local variables.
	
	continueFlag = TRUE;
	fileSelected = FALSE;
	fileInfoLoadedFlag = FALSE;
	bytesToSkip = 0;
	errCode = noErr;
	
	stringIndex = IDS_FileIO51;
	if (modifyFlag)
		stringIndex = IDS_FileIO52;
	
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
	fileNamePtr = (char*)GetFileNamePPointerFromFileInfo (fileInfoPtr);
	
	do	
		{
				// Indicate that the image type is to be determined from the 		
				// header part of the file.													
		
		gGetFileImageType = 0;
						
		errCode = GetFile (fileStreamPtr,
									numberTypes,
									(UInt32*)fileTypesPtr,
									NULL, 
									NULL,
									NULL,
									stringIndex);
									
		continueFlag = (errCode == noErr) & FileExists (fileStreamPtr);
			
		if (continueFlag)
			{
			vRefNum = GetVolumeReferenceNumber (fileStreamPtr);
			
			if (errCode == noErr)
				{
						// Get the space left on the volume.							
						
				errCode = GetVolumeFreeSpace (fileStreamPtr, vRefNum, &freeBytes);
				
				if (errCode == noErr)
					{
							// Everything is okay to here. Copy the file IO stream 
							// information to the structure that will be used for testing
							// whether the selected file is okay.
							
					appendToFileInfoPtr =  (FileInfoPtr)GetHandlePointer (
																	appendToFileInfoHandle, kLock);
					
					appendToFileStreamPtr = GetFileStreamPointer (appendToFileInfoPtr);
					
					CopyFileStream (appendToFileStreamPtr, fileStreamPtr); 
					
			    			// Determine the file format and load the header 			
			    			// information.														
			    				
			    	fileInfoLoadedFlag = LoadImageInformation (
													NULL, appendToFileInfoHandle, FALSE, NULL, 0);
			    				
					if (fileInfoLoadedFlag)
								// Make sure that the file is write enabled.
						errCode = SetFileWriteEnabled (appendToFileStreamPtr);
				
					if (fileInfoLoadedFlag && errCode == noErr)
						errCode = GetSizeOfFile (appendToFileStreamPtr, &logicalEndOfFile);
						
					if (errCode != noErr)
						fileInfoLoadedFlag = FALSE;
			    						
			    	}	// end "if (errCode == noErr)" 
			    						
				if (fileInfoLoadedFlag)
					{	
					appendToFileNumberChannels = appendToFileInfoPtr->numberChannels;
					inputNumberChannels = fileInfoPtr->numberChannels;
					
							// Check if there is enough space on the disk.  If 		
							// there is not enough space, even when considering 		
							// whether an old file is to be replaced then allow the 	
							// user to select the file/disk again.							
							
				   returnCode = AppendFileDialog (appendToFileInfoPtr, 
				   											fileInfoPtr, 
				   											modifyFlag, 
				   											&bytesToSkip);
				   
					if (returnCode >= 0)
						{	
						if (numberBytes + 100 >  
												(freeBytes + (logicalEndOfFile - bytesToSkip)))
							{
							bytesNeeded = numberBytes + 100 - 
												(freeBytes + (logicalEndOfFile - bytesToSkip));
							DiskFullAlert (bytesNeeded, (Str255*)fileNamePtr);

							}	// end "if (numberBytes + 100 > ..."
						
						else	// bytesNeeded <= 0 
							{
							fileSelected = TRUE;
							continueFlag = FALSE;
							
							}	// end "else bytesNeeded <= 0" 
							
						}	// end "if (returnCode >= 0)" 
				
					else	// returnCode < 0) 
						{
						if (returnCode == -2)
							returnCode = DisplayAlert (
										kErrorAlertID, 3, kAlertStrID, IDS_Alert17, 0, NULL);
							
						}	// end "else returnCode < 0)" 
						
					}	// end "if (fileInfoLoadedFlag)" 
					
				if (!fileSelected)
					{
					CloseImageFile (appendToFileInfoHandle);
					fileInfoLoadedFlag = FALSE;
					bytesToSkip = 0; 
					
							// Indicate that the file is closed in the file stream
							// structure which contains the original copy of
							// the append file stream structure.
							 
					IndicateFileClosed (fileStreamPtr);
					
					}	// end "if (!fileSelected)" 
					
				}	// end "if (errCode == noErr)" 
				
			else	// errCode != noErr 
				{
						// Cannot write to this volume.  Send message to the user and	
						// allow the user to try again.											
						
				IOCheck (errCode, fileStreamPtr);
				errCode = noErr;
					
				}	// end "else errCode != noErr)" 
				
			}	// end "if (continueFlag)" 
		
		}		while (continueFlag && errCode == noErr);
				
	if (fileSelected)
		{
		bytesToSkip = (SInt64)MIN (bytesToSkip, logicalEndOfFile);
                                                                       
		errCode = MSetMarker (appendToFileStreamPtr, 
										fsFromStart, 
										bytesToSkip,
										kErrorMessages);
	
		if (!modifyFlag && errCode == noErr)
			errCode = MSetSizeOfFile (appendToFileStreamPtr,
												bytesToSkip,
												kErrorMessages);
												
		*modifyAppendBytesSkipPtr = bytesToSkip;
		
		}	// end "if (fileSelected)" 
		
	IOCheck (errCode, fileNamePtr);
	
	if (errCode == noErr && !fileSelected)
		errCode = 1;
		
	if (errCode == noErr)
		{		
				// Now save the needed information that is now in the appendToFile 
				// information structure so that the data will be written correctly
				// to this file which from now on will be described by the file info 
				// structure.
				
		CopyFileStream (fileStreamPtr, appendToFileStreamPtr);

		fileInfoPtr->format = appendToFileInfoPtr->format;
		fileInfoPtr->ancillaryInfoformat = appendToFileInfoPtr->ancillaryInfoformat;
		fileInfoPtr->numberHeaderBytes = appendToFileInfoPtr->numberHeaderBytes;
		fileInfoPtr->bandInterleave = appendToFileInfoPtr->bandInterleave;
		fileInfoPtr->startLine = appendToFileInfoPtr->startLine;
		fileInfoPtr->startColumn = appendToFileInfoPtr->startColumn;
		
		fileInfoPtr->hdfHandle = appendToFileInfoPtr->hdfHandle;
		appendToFileInfoPtr->hdfHandle = NULL;
		
		fileInfoPtr->hfaHandle = appendToFileInfoPtr->hfaHandle;
		appendToFileInfoPtr->hfaHandle = NULL;
		
		fileInfoPtr->channelToHdfDataSetHandle = 
												appendToFileInfoPtr->channelToHdfDataSetHandle;
		appendToFileInfoPtr->channelToHdfDataSetHandle = NULL;
		
		fileInfoPtr->mapProjectionHandle = appendToFileInfoPtr->mapProjectionHandle;
		appendToFileInfoPtr->mapProjectionHandle = NULL;
		
		GetHierarchalFilePointers (NULL, fileInfoPtr);
		
				// If new channels are being added to the file and descriptions exist
				// for either the appended file or the added file, then we need to 
				// get a new description handle for fileInfoPtr that can save the
				// descriptions for the appended file.
				// This is not to be done for thematic image files or a multispectral 
				// image file that is just being modified.
		
		if (!fileInfoPtr->thematicType &&
				!modifyFlag &&
					(*writeChannelDescriptionsFlagPtr || 
												gImageWindowInfoPtr->descriptionCode == 0) && 		
						fileInfoPtr->numberChannels > inputNumberChannels &&
							(appendToFileInfoPtr->channelDescriptionH != NULL ||
													gImageWindowInfoPtr->descriptionCode != 0))
			{
			fileInfoPtr->channelDescriptionH = NULL;
			fileInfoPtr->channelValuesHandle = NULL;
			
			appendToFileNumberChannels = 
											fileInfoPtr->numberChannels - inputNumberChannels;
					
			fileInfoPtr->channelDescriptionH =
					MNewHandle (appendToFileNumberChannels * sizeof (ChannelDescription));
			
			if (fileInfoPtr->channelDescriptionH != NULL)		
				{
						// Make sure that the processor knows that channel descriptions
						// are to be written to the end of the file.
						
				*writeChannelDescriptionsFlagPtr = TRUE;
				fileInfoPtr->descriptionsFlag = TRUE;
				
						// Get a blank string.																
						
				MGetString (blankString, kFileIOStrID, IDS_BlankString16, 31);
	
				descriptionPtr = (ChannelDescriptionPtr)GetHandlePointer (
																	fileInfoPtr->channelDescriptionH);
																
				if (appendToFileInfoPtr->channelDescriptionH != NULL)
					{
					appendToDescriptionPtr = (ChannelDescriptionPtr)GetHandlePointer (
														appendToFileInfoPtr->channelDescriptionH);
																
					numberBytes = appendToFileNumberChannels * sizeof (ChannelDescription);
					BlockMoveData (appendToDescriptionPtr, 
											descriptionPtr, 
											(UInt32)numberBytes);
					
					descriptionPtr += appendToFileNumberChannels;
					
					}	// end "if (appendToFileInfoPtr->channelDescriptionH != NULL)"
					
				else	// appendToFileInfoPtr->channelDescriptionH == NULL
					{
					for (index=0; index<appendToFileNumberChannels; index++)
						{
						BlockMoveData ((char*)&blankString[1], descriptionPtr, kChannelDescriptionLength);
						descriptionPtr++; 
						
						}	// end "for (index=0; index<appendToFileNumberChannels; ..."
					
					}	// end "if (appendToFileInfoPtr->channelDescriptionH != NULL)"
					
						// Now Get the new handle for the channel values
				
				fileInfoPtr->channelValuesHandle =
									MNewHandle (appendToFileNumberChannels * sizeof (float));
				
				}	// end "if (fileInfoPtr->channelDescriptionH != NULL)"
									
			if (fileInfoPtr->channelValuesHandle != NULL)
				{
				channelValuesPtr = (float*)GetHandlePointer (
																	fileInfoPtr->channelValuesHandle);
																
				if (appendToFileInfoPtr->channelValuesHandle != NULL)
					{
					appendToChannelValuesPtr = (float*)GetHandlePointer (
														appendToFileInfoPtr->channelValuesHandle);
																
					numberBytes = appendToFileNumberChannels * sizeof (float);
					BlockMoveData (appendToChannelValuesPtr, 
										channelValuesPtr, 
										(UInt32)numberBytes);
					
					channelValuesPtr += appendToFileNumberChannels;
					
					}	// end "if (appendFileInfoPtr->channelValuesHandle != NULL)"
					
				else	// appendFileInfoPtr->channelValuesHandle == NULL
					{
					for (index=0; index<appendToFileNumberChannels; index++)
						{
						*channelValuesPtr = -1.;
						channelValuesPtr++; 
						
						}	// end "for (index=0; index<appendToFileNumberChannels; ..."
					
					}	// end "if (appendToFileInfoPtr->channelValuesHandle != NULL)"
				
				}	// end "if (fileInfoPtr->channelValuesHandle != NULL)"
				
			}	// end "if (!fileInfoPtr->thematicType && !modifyFlag && ..."
			
		else if (!fileInfoPtr->thematicType && 
						!modifyFlag &&
							fileInfoPtr->numberChannels == inputNumberChannels &&
								gImageWindowInfoPtr->descriptionCode != 0)
					// Set the flag indicating whether channel descriptions exist that
					// can be copied to the new file.
			fileInfoPtr->descriptionsFlag = TRUE;
		                                           
				// Indicate that the file is closed in the appendFileStream structure
				// so that it will not be closed when the appendFileInfoHandle is
				// disposed of.
				
		IndicateFileClosed (appendToFileStreamPtr);
		
		}	// end "if (errCode == noErr)" 
	    	
	DisposeFileInfoHandle (appendToFileInfoHandle);
	
	return (errCode);

}	// end 'AppendFile'     


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt32 AppendFileDialog
//
//	Software purpose:	The purpose of this routine is to handle the modal
//							dialog for specifying the position to start writing
//							the new data in the append file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 12/24/1991
//	Revised By:			Larry L. Biehl			Date: 09/01/2017	

SInt16 AppendFileDialog (
				FileInfoPtr					appendFileInfoPtr, 
				FileInfoPtr					newFileInfoPtr, 
				Boolean						modifyFlag,
				SInt64*						bytesToSkipPtr)

{
	SInt32  								newAfterLineChannel;
	
	SInt16								effectiveAppendFileBandInterleave,
											effectiveNewFileBandInterleave,
											returnCode;
	                
	Boolean								appendFileBandSequentialFlag,
											lineFlag;
	
	
	if (appendFileInfoPtr == NULL || newFileInfoPtr == NULL)
																							return (-1);
	
	*bytesToSkipPtr = 0;
	returnCode = 0;
	
			// Get the effective band interleave to be used.
			
	effectiveAppendFileBandInterleave = appendFileInfoPtr->bandInterleave;
	effectiveNewFileBandInterleave = newFileInfoPtr->bandInterleave;
	/*
			// Need to figure out way for this to be an option
			
	if (appendFileInfoPtr->numberChannels == 1 && newFileInfoPtr->numberChannels == 1)
		{
		effectiveAppendFileBandInterleave = kBIL;
		effectiveNewFileBandInterleave = kBIL;
		
		}	// end "if (appendFileInfoPtr->numberChannels == 1 && ..."
	*/	
	appendFileBandSequentialFlag = 
				(effectiveAppendFileBandInterleave == kBSQ ||
								effectiveAppendFileBandInterleave == kBNonSQ);
	/*							
	appendFileBandSequentialFlag = appendFileBandSequentialFlag ||
				(appendFileInfoPtr->bandInterleave == kBIL &&
													appendFileInfoPtr->numberChannels == 1);
	*/																		
			// Make certain that the append file and the new file parameters	
			// are consistant.																
			
	if (appendFileInfoPtr->thematicType != newFileInfoPtr->thematicType &&
													appendFileInfoPtr->thematicType == TRUE)
																							return (-2);
	
	if (effectiveAppendFileBandInterleave != effectiveNewFileBandInterleave)
		{
		if (!appendFileBandSequentialFlag || newFileInfoPtr->numberChannels > 1)
																							return (-2);
		
		}	// end "if (appendFileInfoPtr->bandInterleave != ..." 
																			
	if (appendFileInfoPtr->numberColumns != newFileInfoPtr->numberColumns)
																							return (-2);
																			
	if (appendFileInfoPtr->numberBytes != newFileInfoPtr->numberBytes)
																							return (-2);
		
	if (appendFileInfoPtr->thematicType || !appendFileBandSequentialFlag)
		{
		if (appendFileInfoPtr->numberChannels != newFileInfoPtr->numberChannels)
																							return (-2);
		lineFlag = TRUE;
		newAfterLineChannel = appendFileInfoPtr->numberLines;
		
		}	// end "if (appendFileInfoPtr->thematicType || ..." 
		
	else	// !appendFileInfoPtr->thematicType && ... 
		{
		if (appendFileInfoPtr->numberLines != newFileInfoPtr->numberLines)
																			return (-2);
		lineFlag = FALSE;
		newAfterLineChannel = appendFileInfoPtr->numberChannels;
		
		}	// end "else !appendFileInfoPtr->thematicType && ..."
		
#if defined multispec_mac  
	Rect									theBox;
	
	DialogPtr							dialogPtr;
	
	Handle								theHandle;
	
	SInt32								maximumValue,
											theNum;
	
	SInt16								itemHit,
											theType;
	                
	Boolean								modalDone; 
	
																						
	dialogPtr = LoadRequestedDialog (kAppendFileDialogID, kCopyScrap, 1, 2);
	
	returnCode = -1;
	if (dialogPtr != NULL)
		{
				// Item 3: Append or modify.													
				
		if (modifyFlag)
			LoadDItemString (dialogPtr, 3, (Str255*)"\pModify ");
			
		else	// !modifyFlag 
			LoadDItemString (dialogPtr, 3, (Str255*)"\pAppend to ");
			
				// Item 5: File name to be appended to or modified.					
		
		StringPtr appendFileNamePtr =
							(StringPtr)GetFileNamePPointerFromFileInfo (appendFileInfoPtr);
		ParamText ((UCharPtr)appendFileNamePtr,"\p", "\p", "\p");
		
				// Item 6: After lines or channels.											
				
		GetDialogItem (dialogPtr, 6, &theType, &theHandle, &theBox);
		SetControlValue ((ControlHandle)theHandle, 0);
		
		if (lineFlag)
			SetControlTitle ((ControlHandle)theHandle, "\pLine:");
			
		else	// !lineFlag 
			SetControlTitle ((ControlHandle)theHandle, "\pChannel:");
		
				// Item 7: line or channel number.											
				
		LoadDItemValue (dialogPtr, 7, (SInt32)newAfterLineChannel);
		HideDialogItem (dialogPtr, 7);
		
				// Item 8: After last line or channel.										
				
		GetDialogItem (dialogPtr, 8, &theType, &theHandle, &theBox);
		SetControlValue ((ControlHandle)theHandle, 1);
		
		if (lineFlag)
			SetControlTitle ((ControlHandle)theHandle, "\pLast line");
			
		else	// !lineFlag 
			SetControlTitle ((ControlHandle)theHandle, "\pLast Channel");
		
				// Center the dialog and then show it.										
				
		ShowDialogWindow (dialogPtr, kAppendFileDialogID, kSetUpDFilterTable);
					
		maximumValue = newAfterLineChannel;  
		modalDone = FALSE;
		itemHit = 0;
		do 
			{
	  	 	ModalDialog (gProcessorDialogFilterPtr, &itemHit);
			if (itemHit > 2)
				{
				GetDialogItem (dialogPtr, itemHit, &theType, &theHandle, &theBox);
				if (theType == 16)
					{
					GetDialogItemText (theHandle, gTextString);	
					StringToNum (gTextString, &theNum);
				
					}	// end "if (theType == 16)" 
				
				switch (itemHit)
					{
					case 6:	// Line or channel to start after. 
						SetControlValue ((ControlHandle)theHandle, 1);
						SetDLogControl (dialogPtr, 8, 0);
						ShowDialogItem (dialogPtr, 7);
						SelectDialogItemText (dialogPtr, 7, 0, INT16_MAX);
						break;
						
					case 7:	// Line or channel to start after. 
						
						CheckStartValue (theNum, 
												&newAfterLineChannel,
												0,
												maximumValue, 
												dialogPtr, 
												itemHit);
						break;
						
					case 8:	// Last line or channel. 
						SetControlValue ((ControlHandle)theHandle, 1);
						SetDLogControl (dialogPtr, 6, 0);
						HideDialogItem (dialogPtr, 7);
						break;
						
					}	// end "switch (itemHit)" 
				
				}	// end "if (itemHit > 2)" 
			
			else if (itemHit > 0) 	// and itemHit <= 2 
				{	
				modalDone = TRUE;
				returnCode = 0;	
				
				if	(itemHit == 1)		// User selected OK for information 
					{
					SInt16 lineColumnCode = 0; 
					if (GetDLogControl (dialogPtr, 8))
						lineColumnCode = 1;
					
					AppendFileDialogOK (appendFileInfoPtr,
												newFileInfoPtr,
												lineColumnCode,
												lineFlag,
												modifyFlag,
												newAfterLineChannel,
												bytesToSkipPtr);
										
					}	// end "if (itemHit == 1)" 
				
				if	(itemHit == 2)      // User selected Cancel for information 
					returnCode = -1;
				
				}	// end "else if (itemHit > 0) and itemHit <= 2" 
				
			} while (!modalDone);
		
		CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
		
		}	// end "if (dialogPtr != NULL)"  
#endif	// defined multispec_mac  

	#if defined multispec_win 
		CMAppendFileDialog*		dialogPtr = NULL; 
		
		TRY
			{ 
			dialogPtr = new CMAppendFileDialog (); 
			                                                        
			returnCode = dialogPtr->DoDialog (appendFileInfoPtr, 
															newFileInfoPtr, 
															modifyFlag,
															bytesToSkipPtr,
															lineFlag,
															newAfterLineChannel); 
		                      
			delete dialogPtr;
			}
			
		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			returnCode = -1;
			}
		END_CATCH_ALL 
	#endif	// defined multispec_win  
	
	return (returnCode);
	
}	// end "AppendFileDialog"  	


		
void AppendFileDialogOK (
				FileInfoPtr							appendFileInfoPtr,
				FileInfoPtr							newFileInfoPtr,
				SInt16								lineColumnCode,
				Boolean								lineFlag,
				Boolean								modifyFlag,
				SInt32								newAfterLineChannel,
				SInt64*								bytesToSkipPtr)
				
{	
	
	if (lineColumnCode == 1)
		{
		if (lineFlag)
			newAfterLineChannel = appendFileInfoPtr->numberLines;
							
		else	// !lineFlag 
			newAfterLineChannel = appendFileInfoPtr->numberChannels;
							
		}	// end "if (lineColumnCode == 1)" 
	    			
			// Get the number of bytes to skip in the file to be appended to.														
						
	if (lineFlag)
		{
		*bytesToSkipPtr = appendFileInfoPtr->numberHeaderBytes +
						appendFileInfoPtr->bytesPer1lineAllChans * newAfterLineChannel;
						
		if (modifyFlag)			 
			newFileInfoPtr->numberLines = appendFileInfoPtr->numberLines;
						
		else	// !modifyFlag 			 
			newFileInfoPtr->numberLines += newAfterLineChannel;
									 
		}	// end "if (lineFlag)" 
						
	else	// !lineFlag 
		{
		if (appendFileInfoPtr->bandInterleave == kBIL ||
					appendFileInfoPtr->bandInterleave == kBSQ ||
							appendFileInfoPtr->bandInterleave == kBIS)
			*bytesToSkipPtr = (SInt64)appendFileInfoPtr->numberHeaderBytes +
						appendFileInfoPtr->bytesPer1chanAllLines * newAfterLineChannel;
					 
		else if (appendFileInfoPtr->bandInterleave == kBNonSQ)
			{
			GetHierarchalFilePointers (NULL, appendFileInfoPtr);
				
			*bytesToSkipPtr = 
							appendFileInfoPtr->hfaPtr[newAfterLineChannel].layerOffsetBytes;
			
			UnlockHierarchalFileHandles (NULL, appendFileInfoPtr);
			
			}	// end "else if (fileInfoPtr->bandInterleave == kBNonSQ)"
						
		if (modifyFlag)			 
			newFileInfoPtr->numberChannels = appendFileInfoPtr->numberChannels;
						
		else	// !modifyFlag 			 
			newFileInfoPtr->numberChannels += (SInt16)newAfterLineChannel;
									 
		}	// end "else !lineFlag" 
	
}	// end "AppendFileDialogOK"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		ChangeErdasHeader
//
//	Software purpose:	The purpose of this routine is to obtain the 
//							new header information and reformat that information
//							to ERDAS format and rewrite the header file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			SReformatControl in SReformatChangeImageFileFormat.cpp
//
//	Coded By:			Larry L. Biehl			Date: 08/28/1988
//	Revised By:			Larry L. Biehl			Date: 11/20/1991

void ChangeErdasHeader (void)

{  
	CMFileStream*						fileStreamPtr;
	unsigned char						*headerRecordPtr;
	                                  
	Handle								headerRecordH;
	
	UInt32								count,
											posOff;
	
	SInt16								errCode,
											requestedFormat;
	                                                  
	
	if (ChangeErdasHeaderDialog (gImageFileInfoPtr, &requestedFormat))
		{
		fileStreamPtr = GetFileStreamPointer (gImageFileInfoPtr);
		
				// Change header according the the processFlag							
				// processFlag == 1:  Do Write New Erdas Header Request				
				// processFlag == 2:  Do Change Erdas Header Request					
		
		if (gImageFileInfoPtr->format == 0)
			{
			MSetCursor (kWait);
			if (InsertNewErdasHeader (gImageFileInfoPtr, requestedFormat))
				gImageFileInfoPtr->format = requestedFormat;
				
			SetFileReadOnly (fileStreamPtr);
			
			MInitCursor ();
			
			}	// end "if (gImageFileInfoPtr->format == 0)" 
		
		else	// gImageFileInfoPtr->format != 0 
			{
					// Get memory for the header record.									
					
			headerRecordH = (Handle)MNewHandle ((SInt32)128);
			if (headerRecordH == NULL)										
																									return;

			headerRecordPtr =  (unsigned char*)GetHandlePointer (headerRecordH, kLock);
			
			posOff = 0;                                                 
			errCode = MSetMarker (fileStreamPtr, 
											fsFromStart, 
											posOff,
											kErrorMessages);
			if (errCode == noErr)											
				{
				count = 128;
				errCode = MReadData (fileStreamPtr, 
											&count, 
											headerRecordPtr,
											kErrorMessages);
				
				}	// end "if (errCode == noErr)" 
				
			if (errCode == noErr)											
				{	
				if (WriteErdasHeader (
										gImageFileInfoPtr, headerRecordPtr, requestedFormat))
					gImageFileInfoPtr->format = requestedFormat;
					
				SetFileReadOnly (fileStreamPtr);
				
				}	// end "if (errCode == noErr)" 

					// Dispose of the header record buffer.							
					
			UnlockAndDispose (headerRecordH);
			
			}	// end "else gImageFileInfoPtr->format != 0" 
					
		}	// end "if (ChangeErdasHeaderDialog (gImageFileInfoPtr, ..." 

}	// end "ChangeErdasHeader"


                      
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ChangeErdasHeaderDialog
//
//	Software purpose:	The purpose of this routine is to handle the modal 
//							dialog for validating the information that is to be 
//							written in the ERDAS formatted header record of the 
//							image file. 
//
//	Parameters in:			
//
//	Parameters out:	None
//
// Value Returned: 		
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/28/1988
//	Revised By:			Larry L. Biehl			Date: 10/22/2018

Boolean ChangeErdasHeaderDialog (
				FileInfoPtr							fileInfoPtr, 
				SInt16*								requestedFormatPtr)

{                             
	Boolean								returnFlag = FALSE;
	
#if defined multispec_mac
	Rect									theBox;
	
	DialogPtr							dialogPtr;
	
	UserItemUPP							drawHeaderOptionsPopUpPtr;
	
	Handle								okHandle;
	
	SInt16								itemHit,
											itemHit2,
											stringNumber,
											theType;
	
	Boolean								modalDone;
									
																				
			// Get the modal dialog for the image file format description			
				
	dialogPtr = LoadRequestedDialog (kErdasHeaderSpecID, kDoNotCopyScrap, 1, 2);
	if (dialogPtr == NULL)														
																							return (2);
		
			// Intialize local user item proc pointers.									

	drawHeaderOptionsPopUpPtr = NewUserItemUPP (DrawHeaderOptionsPopUp);

			//	Update the specification dialog with the information given			
			// in the format dialog																
	
			// Save handle for the OK button for use later.								
			
	GetDialogItem (dialogPtr, 1,  &theType, &okHandle, &theBox);
			
			// Header format popup.																
				
	SetDialogItemDrawRoutine (dialogPtr, 5, drawHeaderOptionsPopUpPtr);
	gHeaderOptionsSelection = kERDAS74MenuItem;

	InitializeHeaderPopupMenu (dialogPtr, gPopUpHeaderOptionsMenu);
	
   DisableMenuItem (gPopUpHeaderOptionsMenu, kNoneMenuItem);
	DisableMenuItem (gPopUpHeaderOptionsMenu, kArcViewMenuItem);
	DisableMenuItem (gPopUpHeaderOptionsMenu, kGAIAMenuItem);
	DisableMenuItem (gPopUpHeaderOptionsMenu, kMatlabMenuItem);
	DisableMenuItem (gPopUpHeaderOptionsMenu, kTIFFGeoTIFFMenuItem);

			// Load the correct string for number of channels or
			// number of classes.													
	
	stringNumber = 5;
	if (fileInfoPtr->thematicType)
		stringNumber = 2;
	LoadDItemStringNumber (kDialogStrID, 
									stringNumber, 
									dialogPtr, 
									11, 
									(Str255*)gTextString);
	
	LoadCurrentHeaderParametersInDialog (dialogPtr, fileInfoPtr);
		
			// Center the dialog and then show it.											
			
	ShowDialogWindow (dialogPtr, kErdasHeaderSpecID, kSetUpDFilterTable);
	
	modalDone = FALSE;
	itemHit = 0;
	do 
		{
		ModalDialog (gProcessorDialogFilterPtr, &itemHit);
		
		if (itemHit > 2)
			{
			switch (itemHit)
				{	
				case 5:	// Header format 
					
					itemHit2 = StandardPopUpMenu (dialogPtr, 
															4, 
															5, 
															gPopUpHeaderOptionsMenu, 
															gHeaderOptionsSelection,
															kPopUpHeaderOptionsMenuID);
													
					if (itemHit2 != 0)
						gHeaderOptionsSelection = itemHit2;
	
							// Make certain that the correct label is drawn in the	
							// header options pop up box.	
				
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
					
				case 19:
					FileSpecificationDialog (
									gActiveImageFileInfoH, gActiveImageWindowInfoH, NULL);
	
					LoadCurrentHeaderParametersInDialog (dialogPtr, fileInfoPtr);
					
					if (fileInfoPtr->bandInterleave == kBIL)
						HiliteControl ((ControlHandle)okHandle, 0);
						
					else	// fileInfoPtr->bandInterleave != kBIL 
						HiliteControl ((ControlHandle)okHandle, 255);
					break;
					
				case 20:
					if (CoordinateDialog ())
						gUpdateWindowsMenuItemsFlag = TRUE;
	
					break;
					
				}	// end "switch (itemHit)" 
			
			}	// end "if (itemHit > 2)" 
			
		else if (itemHit > 0) 	// and itemHit <= 2 
			{
			modalDone = TRUE;
			
			if (itemHit == 1)		// OK 
				{
						// Header format.														
				
				*requestedFormatPtr = 0;	
					
				if (gHeaderOptionsSelection == kERDAS74MenuItem)
					*requestedFormatPtr = kErdas74Type;
					
				returnFlag = TRUE;
				
				}	// end "if (itemHit == 1)" 
				
			else	// itemHit == 2   Cancel 
				returnFlag = FALSE;
			
			}	// end "else if (itemHit > 0) and itemHit <= 2" 
				
		} while (!modalDone);
		
	DisposeUserItemUPP (drawHeaderOptionsPopUpPtr);
		
	CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
	
#endif	// !defined multispec_mac

	#if defined multispec_win 
		CMChangeHeaderDlg*		dialogPtr = NULL; 
		
		TRY
			{ 
			dialogPtr = new CMChangeHeaderDlg (); 
			                                                                     
			returnFlag = dialogPtr->DoDialog (requestedFormatPtr); 
			
			delete dialogPtr;
			}
			
		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			returnFlag = FALSE;
			}
		END_CATCH_ALL 
	#endif	// defined multispec_win
	
	return (returnFlag);    

}	// end "ChangeErdasHeaderDialog"



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawHeaderOptionsPopUp
//
//	Software purpose:	The purpose of this routine is to draw the header options
//							prompt and selection for header options 
//							pop up menu.  This routine will also draw a drop
//							shadow box around the pop up selection.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/19/1991
//	Revised By:			Larry L. Biehl			Date: 11/19/1991	

pascal void DrawHeaderOptionsPopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.									
			
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpHeaderOptionsMenu, 
								gHeaderOptionsSelection, 
								TRUE);
	
}	// end "DrawHeaderOptionsPopUp" 
#endif	// defined multispec_mac



#if defined multispec_mac 
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawOutputFilePopUp
//
//	Software purpose:	The purpose of this routine is to draw the output file
//							prompt and selection for the output file 
//							pop up menu.  This routine will also draw a drop
//							shadow box around the pop up selection.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/07/1992
//	Revised By:			Larry L. Biehl			Date: 01/07/1992	

pascal void DrawOutputFilePopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.									
			
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpOutputFileMenu, 
								gOutputFileSelection, 
								TRUE);
	
}	// end "DrawOutputFilePopUp"  
#endif	// defined multispec_mac



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetOutputBufferParameters
//
//	Software purpose:	The purpose of this routine is to determine parameters needed
//							to load data data properly into the output buffer.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/18/2006
//	Revised By:			Larry L. Biehl			Date: 11/03/2006

void GetOutputBufferParameters (
				FileInfoPtr							outFileInfoPtr, 
				ReformatOptionsPtr				reformatOptionsPtr,
				UInt32*								outChannelByteIncrementPtr,
				UInt32*								outLineByteIncrementPtr,
				UInt32*								outNumberBytesPerLineAndChannelPtr)

{
	UInt32								countOutBytes,
											numberOutputChannels,
											numberOutputColumns,
											preLineBytes;

	
	countOutBytes = reformatOptionsPtr->countOutBytes;
	numberOutputChannels = reformatOptionsPtr->numberChannels;
	numberOutputColumns = outFileInfoPtr->numberColumns;
	preLineBytes = outFileInfoPtr->numberPreLineBytes;

	*outChannelByteIncrementPtr = (countOutBytes - preLineBytes)/numberOutputChannels;
	
	if (outFileInfoPtr->bandInterleave == kBIS)
		*outChannelByteIncrementPtr /= numberOutputColumns;
	
	else if (outFileInfoPtr->bandInterleave == kBSQ ||
														outFileInfoPtr->bandInterleave == kBNonSQ)
		*outChannelByteIncrementPtr = 
								reformatOptionsPtr->ioOutBufferBytes/numberOutputChannels;
		
			// Get number of offset bytes in the output buffer for start of
			// each line.
			
	*outLineByteIncrementPtr = countOutBytes;
	if (outFileInfoPtr->bandInterleave == kBSQ ||
													outFileInfoPtr->bandInterleave == kBNonSQ)
		*outLineByteIncrementPtr /= outFileInfoPtr->numberChannels;
	
			// Get the number of bytes per line (all columns) and one band 
			// of data.
	
	*outNumberBytesPerLineAndChannelPtr = 
											(countOutBytes-preLineBytes)/numberOutputChannels;
	if (outFileInfoPtr->bandInterleave == kBIS)
		*outNumberBytesPerLineAndChannelPtr /= numberOutputColumns;
	
}	// end "GetOutputBufferParameters"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetOutputFileName
//
//	Software purpose:	This routine provides a default name for the output image file
//							being created by the Reformat->Change Image File Format processor.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/24/2013
//	Revised By:			Larry L. Biehl			Date: 08/28/2020

void GetOutputFileName (
				FileInfoPtr							inputFileInfoPtr,
				FileStringPtr						outFileNamePtr)

{
	FileInfoPtr							localFileInfoPtr;
	FileStringPtr						inFileNamePtr;
	
	SInt16								originalStringLength,
											stringLength;
	
				
	if (gProcessorCode == kRefFieldsToThematicFileProcessor)
		inFileNamePtr = 
					(FileStringPtr)GetFileNamePPointerFromProjectInfo (gProjectInfoPtr);
			
	else	// gProcessorCode != kRefFieldsToThematicFileProcessor
		inFileNamePtr = 
					(FileStringPtr)GetFileNamePPointerFromFileInfo (inputFileInfoPtr);
				
	if (inFileNamePtr != NULL && outFileNamePtr != NULL)
		{
				// Get the pointer to the input file name.
		
		//CopyPToP (outFileNamePtr, inFileNamePtr);
		CopyFileStringToFileString (inFileNamePtr, outFileNamePtr, _MAX_FILE);
				
		if (gProcessorCode != kRefFieldsToThematicFileProcessor)
			{
			if (gImageWindowInfoPtr != NULL && gImageWindowInfoPtr->numberImageFiles > 1)
				{			
						// Try to use the file name base which is common to all of the 
						// linked files.
						
				//stringLength = outFileNamePtr[0];
				stringLength = GetFileStringLength (outFileNamePtr);
				
				localFileInfoPtr = &inputFileInfoPtr[1];
				inFileNamePtr = 
						(FileStringPtr)GetFileNamePPointerFromFileInfo (localFileInfoPtr);
				
						// May need to modify this to only use the files being reformatted.
						// Check the next file in the sequence. 
						
				while (stringLength > 1) 
					{
					if (strncmp ((char*)&outFileNamePtr[2],
										(char*)&inFileNamePtr[2],
										stringLength)) 
						stringLength--;
						
					else	// !strncmp which means the strings are the same.
						break;
						
					}	// end "while (stringLength > 1)"
				
						// Check the last file in the sequence.
				
				originalStringLength = stringLength;
				if (gImageWindowInfoPtr->numberImageFiles > 2)
					{
					localFileInfoPtr = 
								&inputFileInfoPtr[gImageWindowInfoPtr->numberImageFiles-1];
					inFileNamePtr = 
						(FileStringPtr)GetFileNamePPointerFromFileInfo (localFileInfoPtr);
					
					while (stringLength > 1) 
						{
						if (strncmp ((char*)&outFileNamePtr[2],
											(char*)&inFileNamePtr[2],
											stringLength)) 
							stringLength--;
							
						else	// !strncmp which means the strings are the same.
							break;
							
						}	// end "while (stringLength > 1)"
						
					}	// end "if (gImageWindowInfoPtr->numberImageFiles > 2)"
				
				if (inputFileInfoPtr->instrumentCode == kLandsatTM ||
						inputFileInfoPtr->instrumentCode == kLandsatTM7 ||
							inputFileInfoPtr->instrumentCode == kLandsatMSS ||
								inputFileInfoPtr->instrumentCode == kLandsatLC8_OLI_TIRS ||
									inputFileInfoPtr->instrumentCode == kLandsatLC8_OLI ||
										inputFileInfoPtr->instrumentCode == kLandsatLC8_TIRS ||
						inputFileInfoPtr->instrumentCode == kSentinel2A_MSI ||
							inputFileInfoPtr->instrumentCode == kSentinel2B_MSI)
					{
					if (stringLength == originalStringLength)
						{
								// This could just mean the band suffixes are like B02 and B08.
								// Need to do some further checking.
						
						if (outFileNamePtr[stringLength] == 'B' &&
									outFileNamePtr[stringLength-1] == '_' &&
											stringLength > 2) 
							stringLength -= 1;
							
						}	// else !strncmp which means the strings are the same
						
					if (outFileNamePtr[stringLength+1] == 'B' &&
									outFileNamePtr[stringLength] == '_' &&
											stringLength > 2)
						stringLength -= 2;
					
					}	// end "if (inputFileInfoPtr->instrumentCode == kLandsatTM || ..."
					
				if (outFileNamePtr[stringLength+1] == '_' && stringLength > 1)
					stringLength--;
					
				//outFileNamePtr[0] = (UInt8)stringLength;
				//outFileNamePtr[stringLength+1] = 0;
				SetFileStringLength (outFileNamePtr, stringLength);
					
				if ((inputFileInfoPtr->instrumentCode == kSentinel2A_MSI ||
							inputFileInfoPtr->instrumentCode == kSentinel2B_MSI) &&
														gImageWindowInfoPtr->numberImageFiles > 2)
					{
					double		horizontalPixelSize,
									verticalPixelSize;
									
					GetPixelSize (inputFileInfoPtr->mapProjectionHandle, 
										&horizontalPixelSize, 
										&verticalPixelSize);
											
					if (horizontalPixelSize == 10 && verticalPixelSize == 10)
						ConcatFilenameSuffix (outFileNamePtr, (UCharPtr)"\0_10m");
						
					else if (horizontalPixelSize == 20 && verticalPixelSize == 20)
						ConcatFilenameSuffix (outFileNamePtr, (UCharPtr)"\0_20m");
						
					else if (horizontalPixelSize == 60 && verticalPixelSize == 60)
						ConcatFilenameSuffix (outFileNamePtr, (UCharPtr)"\0_60m");
					
					}	// end "if (inputFileInfoPtr->instrumentCode == kSentinel2_MSI ..."
				
				}	// end "if (gImageWindowInfoPtr != NULL && ...)"
				
			else	// gImageWindowInfoPtr == NULL || ...->numberImageFiles == 1
				RemoveSuffix (outFileNamePtr);
							
			}	// end "if (imageFilePathPtr != NULL && ..."
			
		else if (inputFileInfoPtr == NULL && 
						outFileNamePtr != NULL &&
							gProcessorCode == kENVIROItoThematicProcessor)
			//CopyPToP (outFileNamePtr, (UCharPtr)"\0roi_image");
			ConcatFilenameSuffix (outFileNamePtr, (UCharPtr)"\0roi_image");
			
		}	// end "if (gProcessorCode != kRefFieldsToThematicFileProcessor)"

}	// end "GetOutputFileName"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetReformatOutputFile
//
//	Software purpose:	The purpose of this routine is to set up a disk file 
//							to be used for writing a reformatted output image to.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/30/1990
//	Revised By:			Larry L. Biehl			Date: 08/29/2020

Boolean GetReformatOutputFile (
				FileInfoPtr							outFileInfoPtr, 
				ReformatOptionsPtr				reformatOptionsPtr)

{
	CMFileStream*						outFileStreamPtr;
	UCharPtr								savedSuffixPtr,
											tempStringPtr;
	
	FileStringPtr						inFileNamePtr,
											outFileNamePtr;
	
	SInt64								modifyAppendBytesSkip,
											numberBytes;
	
	SInt32								creator,
											type;
							
	SInt16								errCode,
											stringLength;
	
	Boolean								returnFlag;


			// Check input variables.															
			
	if (outFileInfoPtr == NULL)
																						return (FALSE);
																				
	outFileStreamPtr = GetFileStreamPointer (outFileInfoPtr);
																							
			// Get number of bytes to be written to output file						
			
	numberBytes = (SInt64)outFileInfoPtr->bytesPer1line1chan *
					outFileInfoPtr->numberLines * outFileInfoPtr->numberChannels +
																outFileInfoPtr->numberHeaderBytes; 
							
	if (outFileInfoPtr->format == kMatlabType)					
		numberBytes *= sizeof (SDouble)/outFileInfoPtr->numberBytes;
	
			// If channel descriptions are to be written to disk, add				
			// the number of bytes for them.													
			
	if (reformatOptionsPtr->channelDescriptions)
		numberBytes += (20 + outFileInfoPtr->numberChannels*kChannelDescriptionLength);
		
			// Get the output file name and volume to write file to.					
			// Check if enough space on volume.												
			// Load default file name.
   
	outFileNamePtr = 
					(FileStringPtr)GetFileNamePPointerFromFileStream (outFileStreamPtr);
	
	GetOutputFileName (gImageFileInfoPtr, outFileNamePtr);
	
	if (gProcessorCode == kRefFieldsToThematicFileProcessor)
		{
		RemoveSuffix (outFileNamePtr);
		if (reformatOptionsPtr->fieldType & kTrainingType) 
			ConcatFilenameSuffix (outFileNamePtr, (StringPtr)"\0_training\0");
		if (reformatOptionsPtr->fieldType & kTestingType) 
			ConcatFilenameSuffix (outFileNamePtr, (StringPtr)"\0_test\0");
		
		}	// end "if (gProcessorCode == kRefFieldsToThematicFileProcessor)"

	type = kBINAFileType;
	creator = gCreator;
	
	if (outFileInfoPtr->format == kGAIAType)
		{
		type = kGAIAFileType;
		creator = kGAIACreator;
		
		}	// end "if (outFileInfoPtr->format == kGAIAType)" 
	
	else if (outFileInfoPtr->format == kMatlabType)
		{
		type = kMatlabFileType;
		creator = kMatlabCreator; 
		RemoveSuffix (outFileNamePtr);           
		ConcatFilenameSuffix (outFileNamePtr, (StringPtr)"\0.mat\0");         
		
		}	// end "if (outFileInfoPtr->format == kMatlabType)"  
	
	else if (outFileInfoPtr->format == kArcViewType)
		{ 
		RemoveSuffix (outFileNamePtr);
		if (outFileInfoPtr->bandInterleave == kBIL)  
			ConcatFilenameSuffix (outFileNamePtr, (StringPtr)"\0.bil\0");         
				
		else if (outFileInfoPtr->bandInterleave == kBSQ)
			ConcatFilenameSuffix (outFileNamePtr, (StringPtr)"\0.bsq\0");
			
		else if (outFileInfoPtr->bandInterleave == kBIS)
			ConcatFilenameSuffix (outFileNamePtr, (StringPtr)"\0.bip\0");
			
		else 		// outFileNamePtr->bandInterleave != kBIL, kBSQ, kBIS
			ConcatFilenameSuffix (outFileNamePtr, (StringPtr)"\0.???\0"); 
		
		}	// end "if (outFileInfoPtr->format == kArcViewType)"  
	
	else if (outFileInfoPtr->format == kTIFFType || 
										outFileInfoPtr->format == kGeoTIFFType)
		{
		RemoveSuffix (outFileNamePtr);
		ConcatFilenameSuffix (outFileNamePtr, (StringPtr)"\0.tif\0");
		
		}	// end "else if (outFileInfoPtr->format == kTIFFType || ..."
	
	else if (outFileInfoPtr->format == kErdas74Type)
		{
		RemoveSuffix (outFileNamePtr);
		
		if (outFileInfoPtr->thematicType)
			ConcatFilenameSuffix (outFileNamePtr, (StringPtr)"\0.gis\0");
			
		else	// !outFileInfoPtr->thematicType
			ConcatFilenameSuffix (outFileNamePtr, (StringPtr)"\0.lan\0");
		
		}	// end "else if (outFileInfoPtr->format == kErdas74Type)"
		
			// Check if this name is the same as that for the input file. If so,
			// add a modifier to it.
			
	inFileNamePtr = (FileStringPtr)GetFileNamePPointerFromFileInfo (gImageFileInfoPtr);
	
	if (inFileNamePtr != NULL)
		{
		stringLength = GetFileStringLength (outFileNamePtr);
		if (!StringCompare (inFileNamePtr, outFileNamePtr) && stringLength < 251)
			{
					// Add modifer to the string.
					
			gTextString[0] = 0;
			//stringLength = outFileNamePtr[0];
         savedSuffixPtr = NULL;
			if (outFileNamePtr[stringLength-2] == _T('.'))
				{
						// Save the suffix to be used later.
				
				savedSuffixPtr = gTextString;
				strncpy ((char*)&gTextString[1], 
								(char*)&outFileNamePtr[stringLength-2],
								4);
				savedSuffixPtr[0] = 4;
				
				}	// end "if (outFileNamePtr[stringLength-3] == ".")"
				
			if (gMultiSpecWorkflowInfo.workFlowCode == 0)
				{
				strncpy ((char*)&outFileNamePtr[stringLength-2], "_new", 4);
				
				}	// end "if (gMultiSpecWorkflowInfo.workFlowCode == 0)"
				
			else	// gMultiSpecWorkflowInfo.workFlowCode != 0
				{
				tempStringPtr = (UCharPtr)strcpy (
													(char*)&outFileNamePtr[2],
													(char*)&gMultiSpecWorkflowInfo.defaultName[1]);
				stringLength = (SInt16)strlen ((char*)tempStringPtr);
				
				}	// end "else gMultiSpecWorkflowInfo.workFlowCode != 0"
			
			if (savedSuffixPtr != NULL && savedSuffixPtr[0] == 4)
				{
						// Add the suffix back.
				
				strncpy ((char*)&outFileNamePtr[stringLength+2],
							(char*)&gTextString[1],
							4);
	
            }	// end "if (savedSuffixPtr != NULL && gTextString[0] == 4)"
				
			stringLength += 4;
			//outFileNamePtr[0] = (UInt8)stringLength;
			//outFileNamePtr[stringLength+1] = 0;
			SetFileStringLength (outFileNamePtr, stringLength);
			
			}	// end "if (!strcmp (inFileNamePtr, outFileNamePtr) && ...)"
			
		}	// end "if (inFileNamePtr != NULL)"

			// for Linux & Windows versions, copy the proposed file name to the
			// file path string. 
			  
	#if defined multispec_wx || defined multispec_win                                               
		outFileStreamPtr->SetFileName (outFileNamePtr);
	#endif	// defined multispec_wx || defined multispec_win

	SetType (outFileStreamPtr, type); 
	
	if (reformatOptionsPtr->outputFileCode == kNewFileMenuItem)
		{
		if (gMultiSpecWorkflowInfo.workFlowCode == 0)									
			errCode = PutFile (outFileStreamPtr, 
										numberBytes, 
										IDS_SaveNewImageFileAs, 
										creator);
				
		else	// gMultiSpecWorkflowInfo.workFlowCode != 0
			{
					// Create the file on the selected volume	without display dialog box	
				
			CMFileStream* inputFileStreamPtr = GetFileStreamPointer (gImageFileInfoPtr);
			SetVolumeReference (inputFileStreamPtr, outFileStreamPtr);
			errCode = SetVolume (outFileStreamPtr, kErrorMessages);
			
			if (errCode == noErr)
				errCode = CreateNewFile (outFileStreamPtr, 
													GetVolumeReferenceNumber (outFileStreamPtr), 
													gCreator,
													kErrorMessages,
													kDoNotReplaceFlag);
												
			}	// end "else gMultiSpecWorkflowInfo.workFlowCode != 0"
												
		outFileNamePtr = 
					(FileStringPtr)GetFilePathPPointerFromFileStream (outFileStreamPtr);
									
				// If the file was created okay and the output interleave is to be BSQ,
				// set the end of the file to the size of the file. This is done because
				// the data will not be written to the file in sequential order.
				
		if (errCode == noErr && 
					outFileInfoPtr->bandInterleave == kBSQ &&
							outFileNamePtr[0] != 0)
			{
			errCode = MSetSizeOfFile (outFileStreamPtr,
												numberBytes,
												kErrorMessages);
													
					// Make sure that the write marker is set for the start of the file.
			
			if (errCode == noErr)	
				errCode = MSetMarker (outFileStreamPtr, 
												fsFromStart,
												0,
												kErrorMessages);	
			
			}	// end "if (errCode == noErr && ...->bandInterleave == kBSQ && ..."				
		
				// Set the flag indicating whether channel descriptions exist that
				// can be copied to the new file.
											
		if (!outFileInfoPtr->thematicType && gImageWindowInfoPtr->descriptionCode != 0)
			outFileInfoPtr->descriptionsFlag = TRUE;
									
		}	// end "if (reformatOptionsPtr->outputFileCode == kNewFileMenuItem)"
		
	else	// reformatOptionsPtr->outputFileCode != kNewFileMenuItem 
		{
		errCode = AppendFile (
								outFileInfoPtr, 
								gNumberFileTypes, 
								gListTypes, 
								numberBytes, 
								reformatOptionsPtr->outputFileCode == kModifyPartMenuItem,
								&reformatOptionsPtr->channelDescriptions,
								&modifyAppendBytesSkip);
		
		reformatOptionsPtr->headerFormat = outFileInfoPtr->format; 
		reformatOptionsPtr->modifyAppendBytesSkip = modifyAppendBytesSkip;
	
				// Update some intermediate values used to read data in and to 
				// compute the size of the image.
																
		IntermediateFileUpdate (outFileInfoPtr); 

		}	// end "else reformatOptionsPtr->outputFileCode != kNewFileMenuItem"
	
			// Now check if a file name was entered.

	outFileNamePtr = 
					(FileStringPtr)GetFilePathPPointerFromFileStream (outFileStreamPtr);
										
	returnFlag = (errCode != noErr) || (GetFileStringLength (outFileNamePtr) == 0);
	
	return (returnFlag);
				 										
}	// end "GetReformatOutputFile" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		InsertNewErdasHeader
//
//	Software purpose:	This routine creates a new 128 byte Erdas formatted
//							and inserts it at the front of an image file.  The
//							image file must be in BIL format.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			ChangeErdasHeader	
//
//	Coded By:			Larry L. Biehl			Date: 08/30/1988
//	Revised By:			Larry L. Biehl			Date: 12/19/2003

Boolean InsertNewErdasHeader (
				FileInfoPtr							fileInfoPtr, 
				SInt16								requestedFormat)

{
	SInt64								logEOF,
											newLogEOF,
											readPosOff,
											writePosOff;
	
	CMFileStream*						fileStreamPtr;
	HFileIOBufferPtr					ioBufferPtr;
	
	SInt32								lineBytes,
											numberBytes,
											trailerBytes;
	
	UInt32								count,
											line;
	
	SInt16								errCode;
	
	Boolean								continueFlag;
		

			// Initialize local variables.													
			
	continueFlag = FALSE;
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
	
			// Get the logical EOF of the file and											
	                                              
	errCode = GetSizeOfFile (fileStreamPtr, &logEOF);
	if (errCode == noErr)
		{
				// Close the file and then reopen set for write permission.			
				// Note, closing the file, sets it back to write permission if		
				// volume is not locked															
		
		errCode = SetFileWriteEnabled (fileStreamPtr);
		
		}	// end "if (errCode == noErr)" 
		
	if (errCode == noErr)
		{
				// move the logical EOF 128 bytes to give room for the header.  	
				// Check if disk is full														
				
		newLogEOF = logEOF + 128;                         
		errCode = MSetSizeOfFile (fileStreamPtr, newLogEOF, kNoErrorMessages);
																					
		if (errCode == dskFulErr)
			{
			CloseFile (fileInfoPtr);
			
			errCode = OpenFileReadOnly (fileStreamPtr, 
													kDontResolveAliasChains, 
													kLockFile, 
													kDoNotVerifyFileStream);
																					
			}	// end "if (errCode == dskFulErr)" 
		
		}	// end "if (errCode == noErr)" 
		
	if (errCode == noErr)
		{
				// Determine the number of bytes in one line (all channels) of 	
				// data.																				
		
		lineBytes = (SInt32)fileInfoPtr->numberColumns * 
						  								(SInt32)fileInfoPtr->numberChannels;
		if (fileInfoPtr->numberBytes == 2)  
			lineBytes *=2;
		
				// Determine number of trailer bytes										
				
		trailerBytes = (SInt32)(logEOF - (gImageWindowInfoPtr->maxNumberColumns *
								gImageWindowInfoPtr->maxNumberLines *
									(SInt32)gImageWindowInfoPtr->totalNumberChannels *
										(SInt32)fileInfoPtr->numberBytes));
		
				// Set up buffer to be used to move the data in the file				
				// Make the buffer equivalent to one line (all channels) of data	
				// and at least 'trailerBytes' long and at least 128 bytes (to 	
				// handle the header																
		
		numberBytes = lineBytes;
		
		numberBytes = MAX (numberBytes, trailerBytes);
		numberBytes = MAX (numberBytes, 128);
		ioBufferPtr = (HFileIOBufferPtr)MNewPointer (numberBytes);
		continueFlag = (ioBufferPtr != NULL);												
		
		}	// end "if (errCode == noErr)" 
		
			// Display any disk io error at this point except for disk full.  It has 
			// already been displayed.													
																		
	if (errCode != dskFulErr)
		{			
		IOCheck (errCode, fileStreamPtr);
		errCode = noErr;
		
		}	// end "if (errCode != dskFulErr)" 
		
	if (continueFlag)
		{
		readPosOff = logEOF;
		writePosOff = newLogEOF;
								  
				// Move trailer bytes if any													
		
		if (trailerBytes > 0)
			{
					// Read trailer bytes														
					
			readPosOff = logEOF - trailerBytes;    
			errCode = MSetMarker (fileStreamPtr, 
											fsFromStart, 
											readPosOff,
											kErrorMessages);						
			count = trailerBytes;
			if (errCode == noErr)
				errCode = MReadData (fileStreamPtr, 
												&count,
												ioBufferPtr,
												kErrorMessages);
			
					// Write trailer bytes														
					
			writePosOff = newLogEOF - trailerBytes;
			if (errCode == noErr)						           
				errCode = MSetMarker (fileStreamPtr, 
												fsFromStart, 
												writePosOff,
												kErrorMessages);
			count = trailerBytes;
			if (errCode == noErr)
				errCode = MWriteData (fileStreamPtr, 
												&count, 
												ioBufferPtr,
												kErrorMessages);
			
			}	// end "if (trailerBytes > 0)" 
		
		continueFlag = (errCode == noErr);
		
		}	// end "if (continueFlag)" 
		
			// Move image data a line (all channels) at a time							
	
	if (continueFlag)
		{
		for (line=1; line<=gImageWindowInfoPtr->maxNumberLines; line++)
			{
					// Read line of image data													
					
			readPosOff = readPosOff - lineBytes; 
			errCode = MSetMarker (fileStreamPtr, 
											fsFromStart, 
											readPosOff,
											kErrorMessages);
			count = lineBytes;
			if (errCode == noErr)
				errCode = MReadData (fileStreamPtr, 
											&count, 
											ioBufferPtr,
											kErrorMessages);
		
				// Write line of data															
				
			writePosOff = writePosOff - lineBytes;
			if (errCode == noErr)					
				errCode = MSetMarker (fileStreamPtr, 
												fsFromStart, 
												writePosOff,
												kErrorMessages);	
			count = lineBytes;
			if (errCode == noErr)
				errCode = MWriteData (fileStreamPtr, 
												&count, 
												ioBufferPtr,
												kErrorMessages);
				
			if (errCode != noErr)														break;
			
			}	// end "for (line=1; .... 
			
		continueFlag = (errCode == noErr);
		
		}	// end "if (continueFlag)"
		
			// Initialize the header array to all zeros and								
			// Fill the rest of the header and write it to the file.					
	
	if (continueFlag)	
		continueFlag = WriteNewErdasHeader (
						fileInfoPtr, (SInt16*)ioBufferPtr, requestedFormat);
	
			// Update the number of header bytes for the image since it has		
			// changed.																				
			
	fileInfoPtr->numberHeaderBytes = 128;	
		
			// Write channel discriptions at the end of the file if needed.		
	
	if (continueFlag && !fileInfoPtr->thematicType)		
		WriteChannelDescriptionsAndValues (fileInfoPtr, 
														NULL, 
														fileInfoPtr->numberChannels,
														FALSE);
		
			// dispose of the buffer handle.													
			
	ioBufferPtr = (HFileIOBufferPtr)CheckAndDisposePtr ((Ptr)ioBufferPtr);
	
	return (continueFlag);	

}	// end "InsertNewErdasHeader" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				Boolean ListReformatResultsInformation
//
//	Software purpose:	The purpose of this routine is to list the results
//							of the reformat operation.
//
//	Parameters in:		Pointer to reformat instruction structure.
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 05/14/1992
//	Revised By:			Larry L. Biehl			Date: 05/06/2020

Boolean ListReformatResultsInformation (
				ReformatOptionsPtr				reformatOptionsPtr, 
				FileInfoPtr							outFileInfoPtr)

{  
	char									degreesStrings[6],
											formatName[32],
											numberLowCountString[64],
											numberHighCountString[64];
											
	FileStringPtr						fileNamePtr;
	
	Handle								mosaicFileInfoHandle;
	
	int 									numChars;
	
	SInt16								index,
											numberChars;
											
	Boolean								continueFlag;
	

	#if defined multispec_win
		USES_CONVERSION;
	#endif              
	
	continueFlag = TRUE;
	
	if (reformatOptionsPtr->transformDataCode != kNoTransform)
		{
		if (reformatOptionsPtr->transformDataCode == kAdjustChannel)	
			{
			continueFlag = ListSpecifiedStringNumber (
											kReformatStrID, 
											IDS_Reform7,	// 7, 
											(unsigned char*)gTextString, 
											NULL,  
											gOutputForce1Code, 
											continueFlag);
										
			sprintf ((char*)gTextString,
						"    New value = %f + %f*oldValue/%f%s",
						reformatOptionsPtr->adjustOffset,
						reformatOptionsPtr->adjustFactor,
						reformatOptionsPtr->adjustDivisor,
						gEndOfLine);
			continueFlag = OutputString ((CMFileStream*)NULL,
													(char*)gTextString, 
													0, 
													gOutputForce1Code,
													continueFlag);
												
			}	// end "if (...->transformDataCode == kAdjustChannel)" 
			
		else if (reformatOptionsPtr->transformDataCode == kAdjustChannelsByChannel)	
			{
			continueFlag = ListSpecifiedStringNumber (kReformatStrID, 
																	IDS_Reform31,
																	(unsigned char*)gTextString, 
																	NULL,  
																	gOutputForce1Code, 
																	continueFlag);
										
			sprintf ((char*)gTextString,
						"    New value = oldValue + %f * channel_%d%s",
						reformatOptionsPtr->transformAdjustSelectedChannelsFactor,
						reformatOptionsPtr->transformAdjustSelectedChannel,
						gEndOfLine);
			continueFlag = OutputString ((CMFileStream*)NULL,
													(char*)gTextString, 
													0, 
													gOutputForce1Code,
													continueFlag);
												
			}	// end "else if (...->transformDataCode == kAdjustChannelsByChannel)" 
			
		else if (reformatOptionsPtr->transformDataCode == kTransformChannels)	
			{
			continueFlag = ListSpecifiedStringNumber (kReformatStrID, 
																	IDS_Reform8,	// 8, 
																	(unsigned char*)gTextString, 
																	NULL,  
																	gOutputForce1Code, 
																	continueFlag);
																	
			if (reformatOptionsPtr->algebraicTransformOption == kAlgebraicTransformGeneral)
				{
				numChars = sprintf ((char*)gTextString, "     = %f + (%s)/(%s) * %f%s",
											reformatOptionsPtr->transformOffset,
											&reformatOptionsPtr->numeratorString[1],
											&reformatOptionsPtr->denominatorString[1],
											reformatOptionsPtr->transformFactor,
											gEndOfLine);
				continueFlag = OutputString ((CMFileStream*)NULL,
														(char*)gTextString,
														numChars,
														gOutputForce1Code,
														continueFlag);
			
				}	// end "if (...->algebraicTransformOption == kAlgebraicTransformGeneral)"
																		
			else	// ...->algebraicTransformOption >= kAlgebraicTransformThermal_K)
				{
				degreesStrings[0] = 'K';
				degreesStrings[1] = 0;
				degreesStrings[2] = 'C';
				degreesStrings[3] = 0;
				degreesStrings[4] = 'F';
				degreesStrings[5] = 0;
				int index = (reformatOptionsPtr->algebraicTransformOption - 1) * 2;
				
				continueFlag = ListSpecifiedStringNumber (kReformatStrID,
																		IDS_Reform34,
																		NULL,
																		gOutputForce1Code,
																		&degreesStrings[index],
																		continueFlag,
																		kUTF8CharString);
																		
				numChars = sprintf ((char*)gTextString, "      Radiance = %f + (%s)/(%s) * %f%s",
											reformatOptionsPtr->transformOffset,
											&reformatOptionsPtr->numeratorString[1],
											&reformatOptionsPtr->denominatorString[1],
											reformatOptionsPtr->transformFactor,
											gEndOfLine);
				continueFlag = OutputString ((CMFileStream*)NULL,
														(char*)gTextString,
														numChars,
														gOutputForce1Code,
														continueFlag);
																		
				numChars = sprintf ((char*)gTextString, "      Radiant Temperature = %f / ln(%f/Radiance + 1)%s",
											reformatOptionsPtr->algebraicTransformK2Value,
											reformatOptionsPtr->algebraicTransformK1Value,
											gEndOfLine);
				continueFlag = OutputString ((CMFileStream*)NULL,
														(char*)gTextString,
														numChars,
														gOutputForce1Code,
														continueFlag);
				
				}	// end "if (...->algebraicTransformOption >= kAlgebraicTransformThermal_K)"
												
			}	// end "else if (...->transformDataCode == kTransformChannels)" 
			
		else if (reformatOptionsPtr->transformDataCode == kCreatePCImage)	
			{
			if (reformatOptionsPtr->eigenSource == 1)
				index = IDS_Reform9;		// PrincipalComponentImage;
			
			else	// reformatOptionsPtr->eigenSource == 2 
				index = IDS_Reform10;	// FeatureExtractionImage
				
			continueFlag = ListSpecifiedStringNumber (kReformatStrID, 
																	index, 
																	(unsigned char*)gTextString, 
																	NULL,  
																	gOutputForce1Code, 
																	continueFlag);
											
			continueFlag = ListSpecifiedStringNumber (kReformatStrID, 
																	IDS_Reform11,	// 11, 
																	NULL,  
																	gOutputForce1Code, 
																	reformatOptionsPtr->pcScaleFactor, 
																	continueFlag);
			
					// List output data format used.
			/*													
			gDataValueTypeSelection = FileSpecificationDialogSetDataType (
																outFileInfoPtr->dataTypeCode,
																outFileInfoPtr->numberBits,
																outFileInfoPtr->signedDataFlag);
			
			GetMenuItemText (gPopUpDataValueTypeMenu, 
									gDataValueTypeSelection, 
									gTextString2);
											
			continueFlag = ListSpecifiedStringNumber (kReformatStrID, 
																	IDS_Reform12,		// 12, 
																	NULL,  
																	gOutputForce1Code, 
																	gTextString2, 
																	continueFlag);
		*/												
			}	// end "else if (...->transformDataCode == kCreatePCImage)" 
			
		else if (reformatOptionsPtr->transformDataCode == kFunctionOfChannels)	
			{
			continueFlag = ListSpecifiedStringNumber (kReformatStrID, 
																	IDS_Reform30,
																	(unsigned char*)gTextString, 
																	NULL,  
																	gOutputForce1Code, 
																	continueFlag);
			
			#if defined multispec_mac
				GetMenuItemText (gPopUpFunctionMenu, 
										reformatOptionsPtr->functionCode, 
										gTextString2);
				gTextString2[gTextString2[0]+1] = 0;
			#endif	// defined multispec_mac    
			                            
         #if defined multispec_win | defined multispec_wx
				MGetString (gTextString2,
									0,
									IDS_ChannelFunction01+reformatOptionsPtr->functionCode-1);
			#endif	// defined multispec_win
											
			sprintf ((char*)gTextString,
						"    New value = %s * %f%s",
						&gTextString2[1],
						reformatOptionsPtr->functionFactor,
						gEndOfLine);
			continueFlag = OutputString ((CMFileStream*)NULL,
												(char*)gTextString, 
												0, 
												gOutputForce1Code,
												continueFlag);
												
			if (reformatOptionsPtr->functionCode == kFunctionKthSmallestElement)
				{
				sprintf ((char*)gTextString,
							"      kth smallest value: %d%s",
							(int)reformatOptionsPtr->kthSmallestValue,
							gEndOfLine);
				continueFlag = OutputString ((CMFileStream*)NULL,
													(char*)gTextString, 
													0, 
													gOutputForce1Code,
													continueFlag);
				
				}	// and "if (...->functionCode == kFunctionKthSmallestElement)"
												
			}	// end "else if (...->transformDataCode == kFunctionOfChannels)"
											
		}	// end "if (...->transformDataCode != kNoTransform)" 
		
	if (continueFlag && gProcessorCode != kENVIROItoThematicProcessor)
		continueFlag = ListLineColumnIntervalString (
														NULL,
														gOutputForce1Code,
														(UInt32)reformatOptionsPtr->lineStart, 
														(UInt32)reformatOptionsPtr->lineEnd, 
														(UInt32)reformatOptionsPtr->lineInterval,
														(UInt32)reformatOptionsPtr->columnStart, 
														(UInt32)reformatOptionsPtr->columnEnd, 
														(UInt32)reformatOptionsPtr->columnInterval, 
														continueFlag);
	
	if (continueFlag && gProcessorCode == kRefMosaicImagesProcessor)
		{
				// List information for the 2nd file that was used for mosaicing
				
		mosaicFileInfoHandle = GetFileInfoHandle (
										reformatOptionsPtr->rightBottomMosaicWindowInfoHandle);
	
		fileNamePtr =
				(FileStringPtr)GetFileNameCPointerFromFileHandle (mosaicFileInfoHandle);
		if (reformatOptionsPtr->mosaicDirectionCode == kMosaicLeftRight)
			sprintf ((char*)gTextString,
								"%s    Right input image file name: '%s'%s",
								gEndOfLine,
								fileNamePtr,
								gEndOfLine);
		else	// reformatOptionsPtr->mosaicDirectionCode == kMosaicTopBottom
			sprintf ((char*)gTextString,
								"%s    Bottom input image file name: '%s'%s",
								gEndOfLine,
								fileNamePtr,
								gEndOfLine);
								
		continueFlag = OutputString ((CMFileStream*)NULL,
											(char*)gTextString, 
											0, 
											gOutputForce1Code,
											continueFlag,
											kUTF8CharString);
		
		continueFlag = ListLineColumnIntervalString (NULL, 
																	gOutputForce1Code,
																	reformatOptionsPtr->startLine, 
																	reformatOptionsPtr->stopLine, 
																	1,
																	reformatOptionsPtr->startColumn, 
																	reformatOptionsPtr->stopColumn, 
																	1, 
																	continueFlag);
		
		}	// end "if (continueFlag && gProcessorCode == kRefMosaicImagesProcessor)"
		
			// Insert a blank line
		
	continueFlag = OutputString (NULL, 
										(char*)gEndOfLine, 
										0, 
										gOutputForce1Code,
										continueFlag); 
				
				// List the channels used.											
	
	if (continueFlag && 
			!outFileInfoPtr->thematicType &&
				reformatOptionsPtr->transformDataCode != kCreatePCImage)
		continueFlag = ListChannelsUsed (
				gImageFileInfoPtr,
				NULL,
				reformatOptionsPtr->channelPtr,
				reformatOptionsPtr->numberChannels, 
				NULL,
				&gOutputForce1Code,
				kNoTransformation);
										
			// List "  Output Information:"
			
	continueFlag = ListSpecifiedStringNumber (kSharedStrID, 
																IDS_Shared8, 
																(unsigned char*)gTextString, 
																NULL, 
																gOutputForce1Code, 
																continueFlag);
	
	fileNamePtr = (FileStringPtr)GetFileNameCPointerFromFileInfo (outFileInfoPtr);
	sprintf ((char*)gTextString,
						"    New output image file name: '%s'%s",
						fileNamePtr,
						gEndOfLine);
						
	continueFlag = OutputString ((CMFileStream*)NULL,
											(char*)gTextString, 
											0, 
											gOutputForce1Code,
											continueFlag,
											kUTF8CharString);
				
						// List the features to create											
			
	if (continueFlag && 
				reformatOptionsPtr->transformDataCode == kCreatePCImage)
		continueFlag = ListChannelsUsed (
											gImageFileInfoPtr,
											reformatOptionsPtr->pcChannelPtr,
											reformatOptionsPtr->channelPtr,
											reformatOptionsPtr->numberPCChannels, 
											NULL,
											&gOutputForce1Code,
											TRUE);
											
			// List the basic file structure information for the new output file.
			// Note that if the new file is TIFF/GeoTIFF, the start line and column
			// in the new file will be 1 since there are no tags in the header to
			// record the values.
			
	if (outFileInfoPtr->format == kTIFFType || outFileInfoPtr->format == kGeoTIFFType)
		{
		outFileInfoPtr->startLine = 1;
		outFileInfoPtr->startColumn = 1;
		
		}	// end "if (...->format == kTIFFType || ...->format == kGeoTIFFType)"
											
	continueFlag = ListDescriptionInformationForFile (outFileInfoPtr, formatName, 32);
																	
	if (continueFlag && gProcessorCode == kRefChangeFileFormatProcessor)
		{
				// List message if file converted to wavelength order
		
		if (reformatOptionsPtr->outputInWavelengthOrderFlag)
			{
			continueFlag = ListSpecifiedStringNumber (kReformatStrID, 
																		IDS_Reform33,
																		(unsigned char*)gTextString, 
																		NULL, 
																		gOutputForce1Code, 
																		continueFlag);
			
			}	// end "if (reformatOptionsPtr->outputInWavelengthOrderFlag)"
		
		}	// end "if (continueFlag && gProcessorCode == kRefChangeFileFormatProcessor)"
			
	if (reformatOptionsPtr->transformDataCode != kNoTransform ||
										reformatOptionsPtr->checkForSaturationFlag)
		{	
		numberChars = sprintf (
								numberLowCountString,
								"%u",
								(unsigned int)reformatOptionsPtr->lowSaturationCount);
																
		numberChars = InsertCommasInNumberString (
									numberLowCountString, 
									numberChars, 
									-1,
									numberChars);
									
		numberChars = sprintf (
								numberHighCountString,
								"%u",
								(unsigned int)reformatOptionsPtr->highSaturationCount);
																
		numberChars = InsertCommasInNumberString (
									numberHighCountString, 
									numberChars, 
									-1,
									numberChars);
									
		sprintf ((char*)gTextString,
					"%s"
					"    %g is lowest calculated value%s"
					"    %g is highest calculated value%s"
					"    %s data values saturated at low end: %g%s"
					"    %s data values saturated at high end: %g%s",
					gEndOfLine,
					reformatOptionsPtr->minimumValue, gEndOfLine,
					reformatOptionsPtr->maximumValue, gEndOfLine,
					numberLowCountString,
					outFileInfoPtr->minUsableDataValue, gEndOfLine,
					numberHighCountString,
					outFileInfoPtr->maxUsableDataValue, gEndOfLine);
					
		continueFlag = OutputString ((CMFileStream*)NULL,
											(char*)gTextString, 
											0, 
											gOutputForce1Code, 
											continueFlag);
											
		}	// end "if (...->transformDataCode != kNoTransform || ..." 
			
	if (reformatOptionsPtr->transformDataCode == kCreatePCImage && 
			(reformatOptionsPtr->lowSaturationCount > 0 ||
											reformatOptionsPtr->highSaturationCount > 0))	
		{
		double			maxValue,
							newScaleFactor;
			
		maxValue = MAX (fabs (reformatOptionsPtr->minimumValue -
										reformatOptionsPtr->transformOffset), 
								fabs (reformatOptionsPtr->maximumValue -
										reformatOptionsPtr->transformOffset));
										
		newScaleFactor = (outFileInfoPtr->maxUsableDataValue - 
															outFileInfoPtr->minUsableDataValue)/2;
													
		newScaleFactor = (newScaleFactor/maxValue) * reformatOptionsPtr->pcScaleFactor;
		
		if (newScaleFactor >= 1)												
			newScaleFactor = (double)((SInt32)newScaleFactor);
			
		else	// newScaleFactor < 1
			{												
			newScaleFactor = (double)((SInt32)(newScaleFactor*10));
			newScaleFactor /= 10;
			
			}	// end "else newScaleFactor < 1"
													
		continueFlag = ListSpecifiedStringNumber (kReformatStrID, 
																IDS_Reform28,
																NULL, 
																gOutputForce1Code, 
																newScaleFactor, 
																continueFlag);
			
		}	// end "if (...->transformDataCode == kCreatePCImage && ..."
		
	return (continueFlag);
	
}	// end "ListReformatResultsInformation" 



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadCurrentHeaderParametersInDialog
//
//	Software purpose:	This loads the current header parameters in
//							the change header dialog box.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			ChangeErdasHeaderDialog		
//
//	Coded By:			Larry L. Biehl			Date: 11/20/1991
//	Revised By:			Larry L. Biehl			Date: 11/20/1991

void LoadCurrentHeaderParametersInDialog (
				DialogPtr							dialogPtr, 
				FileInfoPtr							fileInfoPtr)

{                     
	SInt16								numberBits;
		
		
			// Number of image lines															
				
	LoadDItemValue (dialogPtr, 8, (SInt32)gImageWindowInfoPtr->maxNumberLines);
		
			// Number of image columns															
				
	LoadDItemValue (dialogPtr, 10, (SInt32)gImageWindowInfoPtr->maxNumberColumns);
		
			// Number of image channels														
		
	if (!fileInfoPtr->thematicType)
		LoadDItemValue (
				dialogPtr, 12, (SInt32)gImageWindowInfoPtr->totalNumberChannels);
		
			// Number of image classes.														
				
	else	// fileInfoPtr->thematicType 
		LoadDItemValue (dialogPtr, 12, (SInt32)fileInfoPtr->numberClasses);
		
			//	Set one byte data value radio button										
				
	numberBits = 0;
	if (fileInfoPtr->numberBits <= 16)  
		numberBits = 16;
	if (fileInfoPtr->numberBits <= 8)  
		numberBits = 8;
	if (fileInfoPtr->numberBits <= 4) 
		numberBits = 4; 
	LoadDItemValue (dialogPtr, 14, (SInt32)numberBits);
		
			// Start line of image																
				
	LoadDItemValue (dialogPtr, 16, (SInt32)fileInfoPtr->startLine);
		
			// Start column of image															
				
	LoadDItemValue (dialogPtr, 18, (SInt32)fileInfoPtr->startColumn);
	
}	// end "LoadCurrentHeaderParametersInDialog" 
#endif	// defined multispec_mac   



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadDescriptionIntoDItem
//
//	Software purpose:	The purpose of this routine is to load the description
//							for the input channel into the specified dialog 
//							item.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			ModifyChannelDescriptionsViaKeyboard   in SReformatUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 09/16/1992
//	Revised By:			Larry L. Biehl			Date: 08/29/2020

void LoadDescriptionIntoDItem (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber, 
				ChannelDescriptionPtr			channelDescriptionPtr)

{
	Str255								descriptionString;
	SInt16								index;
	
					
	if (dialogPtr == NULL)
																								return;
	
	BlockMoveData (channelDescriptionPtr,
						&descriptionString[1],
						kChannelDescriptionLength);
			
	for (index=kChannelDescriptionLength; index> 0; index--)
		if (descriptionString[index] != ' ') 
			break;
			
	descriptionString[0] = (char)index;
	descriptionString[descriptionString[0]+1] = 0;
	
	LoadDItemString (dialogPtr, itemNumber, &descriptionString);
	
}	// end "LoadDescriptionIntoDItem" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ModifyChannelDescriptions
//
//	Software purpose:	The purpose of this routine is to allow the user
//							to add or modify the descriptions for the 
//							channels to a multispectral image file.  The modification
//							will be done via keyboard or disk file depending upon
//							the users wishes.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			ReformatControl in SReformatChangeImageFileFormat.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/09/1989
//	Revised By:			Larry L. Biehl			Date: 08/29/2020

void ModifyChannelDescriptions (
				SInt16								requestedProcedure)
				
{
	SInt64								bytesToAdd,
											freeBytes,
											sizeImageDiskFile,
											sizeImageFile;
	
	CMFileStream*						fileStreamPtr;
	ChannelDescriptionPtr			channelDescriptionPtr;
	char*									fileNamePtr;
	char									*blankPtr;
	float									*channelValuesPtr;
	
	SInt16								alertReturnCode,
											errCode,
											returnCode,
											vRefNum;

	UInt16								channel;
	
	Boolean								continueFlag,
											returnFlag;
												
	
			// Get pointer to file information												
	
	fileStreamPtr = GetFileStreamPointer (gImageFileInfoPtr);
	
			// Get the size of the image without any wavelength descriptions.		
			
	sizeImageFile = GetSizeOfImage (gImageFileInfoPtr);
			
			// Check if there is enough space free on the volume for any 			
			// descriptions that may be added.												
			// First get size of image disk file.											
			                              
	errCode = GetSizeOfFile (fileStreamPtr, &sizeImageDiskFile);
	
	bytesToAdd = sizeImageFile + 40 + 
						gImageFileInfoPtr->numberChannels*(kChannelDescriptionLength+10) -
																			sizeImageDiskFile;
																				
			// Skip check if no bytes will be added.										
			
	if (bytesToAdd > 0)
		{
				// Get space free on the volume.												
				                      
		fileNamePtr = (char*)GetFileNamePPointerFromFileStream (fileStreamPtr);
		vRefNum = GetVolumeReferenceNumber (fileStreamPtr);
		errCode = GetVolumeFreeSpace (fileStreamPtr, vRefNum, &freeBytes);
								
		if (bytesToAdd + 50 > freeBytes)
			{
			DiskFullAlert (bytesToAdd + 50 - freeBytes, (Str255*)fileNamePtr);
			errCode = -1;
			
			}	// end "if (bytesNeeded > 0)" 
			
		}	// end "if (bytesToAdd > 0)" 
			
			// Close the file and set file back for write permission.  Then		 
			// reopen to prepare for adding/changing the channel descriptions.	
	
	if (errCode == noErr)
		errCode = SetFileWriteEnabled (fileStreamPtr);
	
			// Position the marker in the image file to end of image data, just	
			// before the postion for the channel descriptions.						
	
	if (errCode == noErr)
		{                                                               
		errCode = MSetMarker (fileStreamPtr, 
										fsFromStart, 
										sizeImageFile-1,
										kErrorMessages);
		
		}	// end "if (errCode == noErr)" 
	
	if (errCode == noErr)
		{
				// Get pointer to memory for channel descriptions.						
				
		channelDescriptionPtr = (ChannelDescriptionPtr)CheckHandleSize (
						&gImageFileInfoPtr->channelDescriptionH, 
						&continueFlag,
						&returnFlag, 
						gImageFileInfoPtr->numberChannels * sizeof (ChannelDescription));

		if (continueFlag)
			{
					// Get a blank string.														
					
			MGetString (gTextString3, kFileIOStrID, IDS_BlankString16);
			blankPtr = (char*)&gTextString3;
			blankPtr++;
						
					// If no channel descriptions exist now, load in blanks.			
					
			if (!gImageFileInfoPtr->descriptionsFlag)
				for (channel=0; channel<gImageFileInfoPtr->numberChannels; channel++)
					BlockMoveData (blankPtr,
										&channelDescriptionPtr[channel],
										kChannelDescriptionLength);
			
			}	// end "if (continueFlag)" 
			
		else	// !CheckHandleSize 
			errCode = -1;
			
		}	// end "if (errCode == noErr)" 
			
	if (errCode == noErr)
		{
				// Get pointer to memory for channel values.								
			
		channelValuesPtr = (float*)CheckHandleSize (
								&gImageFileInfoPtr->channelValuesHandle,
								&continueFlag,
								&returnFlag, 
								gImageFileInfoPtr->numberChannels * sizeof (float));

		if (continueFlag)
			{
			if (returnFlag)
				for (channel=0; channel<gImageFileInfoPtr->numberChannels; channel++)
					channelValuesPtr[channel] = (float)(channel + 1);
				
			}	// end "if (continueFlag)" 
			
		else	// !continueFlag 
			errCode = -1;
			
		}	// end "if (errCode == noErr)" 
	
	if (errCode == noErr)
		{
		returnCode = ModifyChannelDescriptionsViaKeyboard (
														gImageFileInfoPtr, 
														channelDescriptionPtr, 
														channelValuesPtr);
			
		if (returnCode == 1)
			{
         
			gImageFileInfoPtr->descriptionsFlag = TRUE;
			
			if (gImageWindowInfoPtr->descriptionCode == 0)
				gImageWindowInfoPtr->descriptionCode = kDescriptionExists;
				
			WriteChannelDescriptionsAndValues (gImageFileInfoPtr,
															NULL, 
															gImageFileInfoPtr->numberChannels,
															FALSE);
			
			}	// end "if (returnCode == 1)" 
				
		else if (returnCode == 2)			// Remove all channel descriptions.
			{ 
			alertReturnCode = DisplayAlert (kYesCancelAlertID, 
														kCautionAlert,
														kAlertStrID,
														IDS_Alert84,
														0,
														NULL);
													
			if (alertReturnCode == 1)
				{
				gImageFileInfoPtr->descriptionsFlag = FALSE;
			
						// Set the EOF to the end of the image data.	

				errCode = MSetSizeOfFile (fileStreamPtr, 
													sizeImageFile,
													kErrorMessages);					
						                                                
				}	// end "if (alertReturnCode == 1)"
				
			else	// alertReturnCode != 1
				returnCode = 0;
				
			}	// end "else if (returnCode == 2)"	
			
		if (returnCode != 1)
			{
			if (!gImageFileInfoPtr->descriptionsFlag)
				{
				gImageFileInfoPtr->channelDescriptionH = 
							UnlockAndDispose (gImageFileInfoPtr->channelDescriptionH);
				
				gImageFileInfoPtr->channelValuesHandle = 
							UnlockAndDispose (gImageFileInfoPtr->channelValuesHandle);
				
				}	// end "if (!gImageFileInfoPtr->descriptionsFlag)" 
			
			}	// end "if (returnCode != 1) " 
			
		}	// end "if (errCode == noErr)" 
											
			// Make certain that image file is now read only.							
			
	SetFileReadOnly (fileStreamPtr);
	
	CheckAndUnlockHandle (gImageFileInfoPtr->channelDescriptionH);
	CheckAndUnlockHandle (gImageFileInfoPtr->channelValuesHandle);

}	// end "ModifyChannelDescriptions"


                      
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ModifyChannelDescriptionsViaKeyboard
//
//	Software purpose:	The purpose of this routine is to allow the user
//							to add or modify the descriptions and values for the 
//							channels via the keyboard.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			ModifyChannelDescriptions in SReformatUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/09/1989
//	Revised By:			Larry L. Biehl			Date: 01/11/2017

SInt16 ModifyChannelDescriptionsViaKeyboard (
				FileInfoPtr							fileInfoPtr, 
				ChannelDescriptionPtr			channelDescriptionPtr, 
				float*								channelValuesPtr)

{                      
	SInt16								returnCode = 0; 
		
		
	if (fileInfoPtr == NULL || channelDescriptionPtr == NULL)
																					return (returnCode);
	
	#if defined multispec_mac
		Rect										theBox;
		Str255									descriptionString;
		
		char*										blankPtr;
		DialogPtr								dialogPtr;
		
		float										realValue;

		Handle									theHandle;
		
		SInt16									channelIndex,
													itemHit,
													lastSelectedTextItem,
													theType;
		
		Boolean									changeFlag,
													modalDone;
		
																				
				// Get modal dialog for adding/modifying the channel descriptions.	
					
		dialogPtr = LoadRequestedDialog (kChannelDescriptionID, kCopyScrap, 1, 2);
		if (dialogPtr == NULL)															
																					return (returnCode);	
																					
				// Get a blank string.														
			
		MGetString (gTextString3, kFileIOStrID, IDS_BlankString16);
		blankPtr = (char*)&gTextString3;
	
				// Set routine to draw outline on "OK" box.									
				
		SetDialogItemDrawRoutine (dialogPtr, 11, gHiliteOKButtonPtr);	
		
		SetDLogControl (dialogPtr, 3, 1);	
		
		SetDLogControl (dialogPtr, 12, 0);
		if (!fileInfoPtr->descriptionsFlag)
			SetDLogControlHilite (dialogPtr, 12, 255);
		
				// Item 4: Write channel number.													
		
		channelIndex = 0;
		LoadDItemValue (dialogPtr, 4, (SInt32)(channelIndex+1));
			
				// Item 8: Present channel description for first channel.				
				// Remove trailing blanks.															
		
		LoadDescriptionIntoDItem (dialogPtr, 8, channelDescriptionPtr);
		
		LoadDItemRealValue (dialogPtr, 10, *channelValuesPtr, 3);
			
				// Center the dialog and then show it.											
				
		ShowDialogWindow (dialogPtr, kChannelDescriptionID, kSetUpDFilterTable);
			
		gDialogItemDescriptorPtr[8] = kDItemString;
		gDialogItemDescriptorPtr[10] = kDItemReal + kDItemMinus;
		
				// Select the default item.
			
		lastSelectedTextItem = 8;
		SelectDialogItemText (dialogPtr, lastSelectedTextItem, 0, INT16_MAX);
				
		changeFlag = FALSE;
		modalDone = FALSE;
		itemHit = 0;
		do 
			{
	   	ModalDialog (gProcessorDialogFilterPtr, &itemHit);
	   	
	   	if (itemHit == 1 || itemHit == 5 || itemHit == 6)
	   		{
	   				// Get the new description
	   				
				BlockMoveData (&blankPtr[1],
									&descriptionString[1],
									kChannelDescriptionLength);
				GetDialogItem (dialogPtr, 
										IDC_Description,  
										&theType, 
										&theHandle, 
										&theBox);
				GetDialogItemText (theHandle, descriptionString);
				
						// Get the new value
				
				realValue = GetDItemRealValue (dialogPtr, IDC_Value);
	
				changeFlag = ModifyChannelDescriptionsUpdate (dialogPtr, 
																				channelDescriptionPtr, 
																				channelValuesPtr,
																				channelIndex,
																				(char*)&descriptionString,
																				realValue,
																				changeFlag);
																				
	   		}	// end "if (itemHit == 1 || ..." 
	   		
			if (itemHit > 2)
				{
						// If itemHit was a radio button make appropriate control		
						// settings to indicate to the user the present 					
						// selection. Get the handle to the itemHit.							
					
				GetDialogItem (dialogPtr, itemHit, &theType, &theHandle, &theBox);
			
				switch (itemHit)
					{
					case 3:	// Add or edit the descriptions.	
						
						SetDLogControl (dialogPtr, 3, 1);
						SetDLogControl (dialogPtr, 12, 0);
						
						ShowDialogItem (dialogPtr, 4);
						ShowDialogItem (dialogPtr, 5);
						ShowDialogItem (dialogPtr, 6);
						ShowDialogItem (dialogPtr, 7);
						ShowDialogItem (dialogPtr, 8);
						ShowDialogItem (dialogPtr, 9);
						ShowDialogItem (dialogPtr, 10);										
						break;	
								
					case 5:	// Go to previous channel.  Wrap around if at end. 		
					
						channelIndex--;
						if (channelIndex < 0)
							channelIndex = fileInfoPtr->numberChannels - 1;
							
						ModifyChannelDescriptionsChangeChannel (dialogPtr, 
																				channelDescriptionPtr, 
																				channelValuesPtr,
																				channelIndex,
																				lastSelectedTextItem);
						break; 
					
					case 6:	// Go to next channel.  Wrap around if at end. 				
					
						channelIndex++;
						if (channelIndex >= fileInfoPtr->numberChannels)
							channelIndex = 0;
							
						ModifyChannelDescriptionsChangeChannel (dialogPtr, 
																				channelDescriptionPtr, 
																				channelValuesPtr,
																				channelIndex,
																				lastSelectedTextItem);
																				
						break;
						
					case 8:				// Channel description 
					
						GetDialogItemText (theHandle, gTextString);
						if (gTextString[0] > 16)
							{
							gTextString[0] = (char)16;
							SetDialogItemText (theHandle, (ConstStr255Param)gTextString);
							SelectDialogItemText (dialogPtr, itemHit, 0, INT16_MAX);
							SysBeep (2);
							
							}	// end "if (gTextString[0] > 16)" 
							
						lastSelectedTextItem = itemHit;
						
						break;
						
					case 10:				// Channel value 
					
						lastSelectedTextItem = itemHit;
						break;
						
					case 12:				// Delete channel descriptions
						
						SetDLogControl (dialogPtr, 3, 0);
						SetDLogControl (dialogPtr, 12, 1);
						
						HideDialogItem (dialogPtr, 4);
						HideDialogItem (dialogPtr, 5);
						HideDialogItem (dialogPtr, 6);
						HideDialogItem (dialogPtr, 7);
						HideDialogItem (dialogPtr, 8);
						HideDialogItem (dialogPtr, 9);
						HideDialogItem (dialogPtr, 10);
						break;
						
					}	// end "switch (itemHit)" 
				
				}	// end "if (itemHit > 2)" 
					
			else if (itemHit > 0) 	// and itemHit <= 2 
				{
				modalDone = TRUE;
				returnCode = 0;
				
				if (GetDLogControl (dialogPtr, 3) && changeFlag)
					returnCode = 1;
					
				else if (GetDLogControl (dialogPtr, 12))
					returnCode = 2;
				
				if	(itemHit == 2)	  // User selected Cancel.  
					returnCode = 0;
						
				break;
				
				}	// end "else if (itemHit > 0) and itemHit <= 2" 
				
			} while (!modalDone);
			
		CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
	#endif	// defined multispec_mac   
	
	#if defined multispec_win                        
		CMChangeChannelDescriptionDlg*		dialogPtr = NULL;
		
		TRY
			{ 
			dialogPtr = new CMChangeChannelDescriptionDlg (); 
			                                                                  
			returnCode = dialogPtr->DoDialog (fileInfoPtr, 
															channelDescriptionPtr, 
															channelValuesPtr); 
			
			delete dialogPtr;
			}
			
		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			}
		END_CATCH_ALL 
	#endif	// defined multispec_win   

   #if defined multispec_wx
      CMChangeChannelDescriptionDlg*		dialogPtr = NULL;
      
      dialogPtr = new CMChangeChannelDescriptionDlg (); 
			                                                                  
      returnCode = dialogPtr->DoDialog (fileInfoPtr, 
															channelDescriptionPtr, 
															channelValuesPtr); 
			
		delete dialogPtr;
   #endif
   
	return (returnCode);

}	// end "ModifyChannelDescriptionsViaKeyboard" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ModifyChannelDescriptionsUpdate
//
//	Software purpose:	The purpose of this routine is to update parameters
//							in the file structure used for the output file based
//							on the user selections made in the dialog box.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/29/1999
//	Revised By:			Larry L. Biehl			Date: 08/29/2020

Boolean ModifyChannelDescriptionsUpdate (
				DialogPtr							dialogPtr, 
				ChannelDescriptionPtr			channelDescriptionPtr, 
				float*								channelValuesPtr,
				SInt16								channelIndex,
				char*									stringPtr,
				float									realValue,
				Boolean								changeFlag)

{										
			// Copy any new info for the description or values back to the	
			// appropriate structures.		
	
	if (stringPtr[0] == 0 || strncmp (&stringPtr[1],
			(char*)&channelDescriptionPtr[channelIndex], kChannelDescriptionLength) != 0)
		{
				// First copy in a blank string to handle case where new string is
				// shorter than the previous one.
				
		BlockMoveData ("                        ",
								&channelDescriptionPtr[channelIndex], 
								kChannelDescriptionLength);
		BlockMoveData (&stringPtr[1], 
								&channelDescriptionPtr[channelIndex], 
								stringPtr[0]);
		changeFlag = TRUE;
		
		}	// end "if (strncmp (&gTextString[1], ..." 
	
	if (channelValuesPtr[channelIndex] != realValue)
		{
		channelValuesPtr[channelIndex] = realValue;
		changeFlag = TRUE;
		
		}	// end "if (channelValuesPtr[channelIndex] != realValue)" 
		
	return (changeFlag);

}	// end "ModifyChannelDescriptionsUpdate"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ModifyChannelDescriptionsChangeChannel
//
//	Software purpose:	The purpose of this routine is to update parameters
//							in the file structure used for the output file based
//							on the user selections made in the dialog box.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/29/1999
//	Revised By:			Larry L. Biehl			Date: 12/29/1999

void ModifyChannelDescriptionsChangeChannel (
				DialogPtr							dialogPtr, 
				ChannelDescriptionPtr			channelDescriptionPtr, 
				float*								channelValuesPtr,
				SInt16								channelIndex,
				SInt16								lastSelectedTextItem)

{		
	::LoadDItemValue (dialogPtr, IDC_ChannelNumber, (SInt32)(channelIndex+1));
	
	LoadDescriptionIntoDItem (
					dialogPtr, IDC_Description, &channelDescriptionPtr[channelIndex]);

	LoadDItemRealValue (
					dialogPtr, IDC_Value, channelValuesPtr[channelIndex], 3);
					
	SelectDialogItemText (dialogPtr, lastSelectedTextItem, 0, SHRT_MAX);

}	// end "ModifyChannelDescriptionsChangeChannel"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void UpdateOutputFileStructure
//
//	Software purpose:	The purpose of this routine is to update parameters
//							in the file structure used for the output file based
//							on the user selections made in the dialog box.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			MosaicImagesSideBySideControl in SMosaic.cpp
//							RectifyImageControl in SRectifyImage.cpp
//							AreasToThematicFileControl in SFieldsToThematicFile.cpp
//							CovertMultispectralToThematicControl in SFieldsToThematicFile.cpp
//							ENVI_ASCII_ROI_ToThematicFileControl in SFieldsToThematicFile.cpp
//							ChangeImageFileFormat in SReformatChangeImageFileFormat.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/29/1990
//	Revised By:			Larry L. Biehl			Date: 07/10/2018

void UpdateOutputFileStructure (
				FileInfoPtr							outFileInfoPtr,
				ReformatOptionsPtr				reformatOptionsPtr,
				SInt32								startLine, 
				SInt32								startColumn,
				Handle								mapProjectionHandle)

{
			// Update number of channels for the output image file.				
													
	outFileInfoPtr->format = reformatOptionsPtr->headerFormat;
	outFileInfoPtr->ancillaryInfoformat = reformatOptionsPtr->headerFormat;
			
	outFileInfoPtr->numberChannels =	reformatOptionsPtr->numberChannels;
	if (reformatOptionsPtr->transformDataCode == kTransformChannels ||
					reformatOptionsPtr->transformDataCode == kFunctionOfChannels)
		outFileInfoPtr->numberChannels =	1;
		
	if (reformatOptionsPtr->transformDataCode == kCreatePCImage)
		outFileInfoPtr->numberChannels =	reformatOptionsPtr->numberPCChannels;

			// Update number of lines and columns for the output image file.	
	
	outFileInfoPtr->numberLines = 
		(reformatOptionsPtr->lineEnd - reformatOptionsPtr->lineStart +
				reformatOptionsPtr->lineInterval)/
									reformatOptionsPtr->lineInterval;
									
	outFileInfoPtr->numberColumns =
		(reformatOptionsPtr->columnEnd - reformatOptionsPtr->columnStart +
				reformatOptionsPtr->columnInterval)/
									reformatOptionsPtr->columnInterval;
									
			// Take into account the number of columns in the right image if	
			// mosaicking.																		
			
	if (gProcessorCode == kRefMosaicImagesProcessor)
		{
		if (reformatOptionsPtr->mosaicDirectionCode == kMosaicLeftRight)
			outFileInfoPtr->numberColumns +=
				reformatOptionsPtr->stopColumn - reformatOptionsPtr->startColumn + 1;
				
		else	// reformatOptionsPtr->mosaicDirectionCode == kMosaicTopBottom)
			outFileInfoPtr->numberLines +=
				reformatOptionsPtr->stopLine - reformatOptionsPtr->startLine + 1;
				
		}	// end "if (gProcessorCode == kRefMosaicImagesProcessor)"
									
	if (outFileInfoPtr->format == kGAIAType)
		{
		outFileInfoPtr->numberPreLineBytes = 14;
		outFileInfoPtr->numberPostLineBytes = 
					GetNumberGAIALineSegments (outFileInfoPtr->numberColumns) * 2;
		
		}	// end "if (outFileInfoPtr->format == kGAIAType)" 
	
	if (reformatOptionsPtr->outputFileCode == kNewFileMenuItem)
		{								
		outFileInfoPtr->startLine = 
								reformatOptionsPtr->lineStart + startLine - 1;
		outFileInfoPtr->startColumn = 
								reformatOptionsPtr->columnStart + startColumn - 1;
								
		}	// end "if (reformatOptionsPtr->outputFileCode == kNewFileMenuItem)" 
	
	outFileInfoPtr->numberHeaderBytes = 0;
	if (outFileInfoPtr->format == kErdas74Type)
		outFileInfoPtr->numberHeaderBytes = 128;
		
			// Get whether the output data is to be signed.							
			
	outFileInfoPtr->signedDataFlag = reformatOptionsPtr->signedOutputDataFlag;
		
			// Update the number of bits.													
											
	if (outFileInfoPtr->numberBytes == 1 && outFileInfoPtr->numberBits > 8)
		outFileInfoPtr->numberBits = 8;
	
			// Update some intermediate values used to read data in and to compute the
			// size of the image.
															
	IntermediateFileUpdate (outFileInfoPtr); 
	
			// Make sure instrument code will still make sense.
			// Will allow output number of channels to be fewer than the original file
			// number of channels for the instrument code. This will allow the source
			// instrument to be documented. The wavelength bands written at the end
			// will indicate which bands are in the file. (7/9/2018). Will see if
			// this causes problems down the road.
			
	if (outFileInfoPtr->thematicType)
		outFileInfoPtr->instrumentCode = 0;
	
			// Set channel wavelength order code
	
	if (outFileInfoPtr->instrumentCode == kPeruSat &&
			gImageWindowInfoPtr->channelsInWavelengthOrderCode == kNotInOrder &&
				!reformatOptionsPtr->outputInWavelengthOrderFlag)
		outFileInfoPtr->channelsInWavelengthOrderCode = kNotInOrder;
	
			// Now copy map project information to the output file info structure
			// the information exists for the input file and a new file is being
			// created.	
		
	if (mapProjectionHandle != NULL && 
								reformatOptionsPtr->outputFileCode == kNewFileMenuItem)
		{
		UpdateMapProjectionStructure (mapProjectionHandle,
													outFileInfoPtr,
													startLine, 
													startColumn,
													reformatOptionsPtr->lineInterval,
													reformatOptionsPtr->columnInterval,
													reformatOptionsPtr->rightToLeft,
													reformatOptionsPtr->bottomToTop);
													
				// Change the file format from TIFF to GeoTIFF if map projection
				// information exists.
				
		if (FindIfMapInformationExists (mapProjectionHandle) && 
															outFileInfoPtr->format == kTIFFType)
			outFileInfoPtr->format = kGeoTIFFType;			
																
		}	// end "if (mapProjectionHandle != NULL && ..."
				 										
}	// end "UpdateOutputFileStructure" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 WriteChannelDescriptions
//
//	Software purpose:	The purpose of this routine is to write the channel
//							descriptions to the end of an image file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			WriteChannelDescriptionsAndValues
//
//	Coded By:			Larry L. Biehl			Date: 02/10/1989
//	Revised By:			Larry L. Biehl			Date: 08/29/2020

SInt16 WriteChannelDescriptions (
				FileInfoPtr							fileInfoPtr, 
				SInt16*								channelPtr,
				UInt32								numberChannels,
				Boolean								appendToFileFlag)

{
	ChannelDescriptionPtr			channelDescriptionPtr;
	CMFileStream*						fileStreamPtr;
	FileInfoPtr							localFileInfoPtr;
	char									*blankPtr,
											*descriptionPtr;
	
	UInt32								count,
											numberChannelsWritten;
	
	SInt16								channel,
											errCode,
											fileChannel; 
												
	UInt16								channelIndex,
											fileIndex,
											fileInfoIndex;
	
   
	
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
	numberChannelsWritten = 0;
	
			// Write channel description identifier to the image file.				

	MGetString (gTextString, kFileIOStrID, IDS_ChannelDescription2);
    
	count = gTextString[0];
	errCode = MWriteData (fileStreamPtr, 
									&count, 
									(char*)&gTextString[1],
									kErrorMessages);
			
			// Get a blank string. We will ignore any error in getting the string.
			// We will just use what is in the first 24 characters of gTextString3.
			
	MGetString (gTextString3, kFileIOStrID, IDS_BlankString16);
	blankPtr = (char*)&gTextString3[1];
	
	if (errCode == noErr && appendToFileFlag && 
														fileInfoPtr->channelDescriptionH != NULL)
		{
				// First add any channel descriptions which may already exist
				// for the appended-to file.

		count = MGetHandleSize (fileInfoPtr->channelDescriptionH);

		
		channelDescriptionPtr = (ChannelDescriptionPtr)GetHandlePointer (
																	fileInfoPtr->channelDescriptionH, 
																	kLock);
											
		errCode = MWriteData (fileStreamPtr, 
									&count, 
									channelDescriptionPtr,
									kErrorMessages);
									
		CheckAndUnlockHandle (fileInfoPtr->channelDescriptionH);
		
		numberChannelsWritten = count/sizeof (ChannelDescription);
      
			
		}	// end "if (errCode == noErr && fileInfoPtr->channelDescriptionH != NULL)"
	
	if (errCode == noErr && numberChannelsWritten < fileInfoPtr->numberChannels)
		{
				// This part allows the channel descriptions for linked-files
				// to be merged into the new or appended output file.
					
		channelIndex = 0;
		channel = 1;
		fileIndex = -1;
		
		while (errCode == noErr && channelIndex < numberChannels)
			{
			if (channelPtr != NULL)
				channel = channelPtr[channelIndex] + 1;
		
			fileInfoIndex = gImageLayerInfoPtr[channel].fileInfoIndex;
			
			if (fileIndex != fileInfoIndex)
				{
				fileIndex = fileInfoIndex;
				localFileInfoPtr = &gImageFileInfoPtr[fileInfoIndex];
			
				channelDescriptionPtr = (ChannelDescriptionPtr)GetHandlePointer (
															localFileInfoPtr->channelDescriptionH);
				
				}	// end "if (fileIndex != fileInfoIndex)"
				
			if (channelDescriptionPtr != NULL)
				{
				fileChannel = gImageLayerInfoPtr[channel].fileChannelNumber;
				descriptionPtr = (char*)&channelDescriptionPtr[fileChannel-1];
										
				}	// end "if (channelDescriptionPtr != NULL)"
					
			else	// channelDescriptionPtr == NULL
				descriptionPtr = blankPtr;
				
			count = kChannelDescriptionLength;
			errCode = MWriteData (fileStreamPtr,
											&count,
											descriptionPtr,
											kErrorMessages);
			
			channelIndex++;
			channel++;
		
			}	// end "while (errCode == noErr && ..."
		
		}	// end "if (errCode == noErr && ..."
		
	return (errCode);

}	// end "WriteChannelDescriptions" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void WriteChannelDescriptionsAndValues
//
//	Software purpose:	The purpose of this routine is to write the channel
//							descriptions to the end of an image file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			MosaicImagesSideBySide in SMosaic.cpp
//							RectifyImage in SRectifyImage.cpp
//							ChangeFormatToBILorBISorBSQ in SReformatChangeImageFileFormat.cpp
//							InsertNewErdasHeader in SReformatChangeImageFileFormat.cpp
//							ModifyChannelDescriptions in SReformatUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/10/1989
//	Revised By:			Larry L. Biehl			Date: 12/22/2003

void WriteChannelDescriptionsAndValues (
				FileInfoPtr							fileInfoPtr, 
				SInt16*								channelPtr,
				UInt32								numberChannels,
				Boolean								appendToFileFlag)

{
	SInt64								sizeImageFile;
	SInt16								errCode;
	
	
			// Verify some global variables that are needed.
			
	if (gImageWindowInfoPtr == NULL ||
				gImageFileInfoPtr == NULL ||
					gImageLayerInfoPtr == NULL)
																										return;
			
			// First determine if any channel descriptions are to be written for
			// the new output image file.																
			
	if (fileInfoPtr->descriptionsFlag)
		{
				// Get the size of the image without any wavelength descriptions.	
			
		sizeImageFile = GetSizeOfImage (fileInfoPtr);
	   
				// Position the marker in the image file to end of image data, 	
				// just before the postion for the channel descriptions.				
	                                      
		CMFileStream* fileStreamPtr = GetFileStreamPointer (fileInfoPtr);                 
		errCode = MSetMarker (fileStreamPtr, 
										fsFromStart, 
										sizeImageFile,
										kErrorMessages);
		
		if (errCode == noErr)
			errCode = WriteChannelDescriptions (fileInfoPtr, 
																channelPtr, 
																numberChannels,
																appendToFileFlag);
		
		if (errCode == noErr)
			errCode = WriteChannelValues (fileInfoPtr,
													channelPtr,
													numberChannels,
													appendToFileFlag);
					
		}	// end "if (fileInfoPtr->descriptionsFlag)" 

}	// end "WriteChannelDescriptionsAndValues" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 WriteChannelValues
//
//	Software purpose:	The purpose of this routine is to write the channel
//							values to the end of an image file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			WriteChannelDescriptionsAndValues
//
//	Coded By:			Larry L. Biehl			Date: 09/16/1992
//	Revised By:			Larry L. Biehl			Date: 04/04/2019

SInt16 WriteChannelValues (
				FileInfoPtr							fileInfoPtr, 
				SInt16*								channelPtr,
				UInt32								numberChannels,
				Boolean								appendToFileFlag)

{
	SInt64								positionOffset;
	
	CMFileStream* 						fileStreamPtr;
	FileInfoPtr							localFileInfoPtr;
	float*								channelValuePtr;
	
	float									noValue;
	
	UInt32								channel,
											count,
											numberChannelsWritten;
	
	SInt16								errCode, 
											fileChannel; 
												
	UInt16								channelIndex,
											fileIndex,
											fileInfoIndex;
	
	
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
	numberChannelsWritten = 0;
	noValue = -1.;
		
			// Get the current location of the file pointer.							
			// This will be used if writing of the channel values has to be		
			// cancelled for the case when there are no channel values.				
		
	errCode = MGetMarker (fileStreamPtr, 
									&positionOffset,
									kErrorMessages);
	
			// Write channel value identifier to the image file.						

	MGetString (gTextString, kFileIOStrID, IDS_ChannelValues);

	if (errCode == noErr)
		{
		count = gTextString[0];
		errCode = MWriteData (fileStreamPtr, 
										&count, 
										(char*)&gTextString[1],
										kErrorMessages);
		
		}	// end "if (errCode == noErr)" 
		
	if (errCode == noErr && 
					appendToFileFlag && 
									fileInfoPtr->channelValuesHandle != NULL)
		{
				// First add any channel descriptions which may already exist
				// for the appended-to file.

		count = MGetHandleSize (fileInfoPtr->channelValuesHandle);
		numberChannelsWritten = count/sizeof (float);
		
		channelValuePtr = (float*)GetHandlePointer (fileInfoPtr->channelValuesHandle, 
																	kLock);											
				
		for (channel=0; 
				channel<numberChannelsWritten;
				channel++)
			{								
			sprintf ((char*)gTextString, 
							" %f",
							channelValuePtr[channel]);
				
					// The length is forced to be 9
			//count = (UInt32)strlen ((char*)gTextString);
			count = 9;
				
			errCode = MWriteData (fileStreamPtr, 
											&count, 
											(char*)gTextString,
											kErrorMessages);
										
			}	// end "for (channel=0; ..."
									
		CheckAndUnlockHandle (fileInfoPtr->channelValuesHandle);
			
		}	// end "if (errCode == noErr && fileInfoPtr->channelDescriptionH != NULL)"
			
	if (errCode == noErr && numberChannelsWritten < fileInfoPtr->numberChannels)
		{
				// This part allows the channel values for linked-files
				// to be merged into the new or appended output file.
					
		channelIndex = 0;
		channel = 1;
		fileIndex = -1;
		
		while (errCode == noErr && channelIndex < numberChannels)
			{
			if (channelPtr != NULL)
				channel = channelPtr[channelIndex] + 1;
		
			fileInfoIndex = gImageLayerInfoPtr[channel].fileInfoIndex;
			
			if (fileIndex != fileInfoIndex)
				{
				fileIndex = fileInfoIndex;
				localFileInfoPtr = &gImageFileInfoPtr[fileInfoIndex];
			
				channelValuePtr = (float*)GetHandlePointer (
															localFileInfoPtr->channelValuesHandle);
				
				}	// end "if (fileIndex != fileInfoIndex)"
			
			if (channelValuePtr != NULL)
				{
				fileChannel = gImageLayerInfoPtr[channel].fileChannelNumber;
				sprintf ((char*)gTextString, 
								" %f",
								channelValuePtr[fileChannel-1]);
								
				}	// end "if (channelValuePtr != NULL)"
				
			else if (numberChannelsWritten > 0)
				{
				sprintf ((char*)gTextString, 
								" %f",
								noValue);
				
				}	// end "else if (numberChannelsWritten > 0)"
					
			else	// channelValuePtr == NULL && numberChannelsWritten == 0
				{
						// If channel values do not exist for any channel, then		
						// do not write any channel values to the output file.	
					                                                   
				errCode = MSetSizeOfFile (fileStreamPtr, 
													positionOffset,
													kErrorMessages);
				
				errCode = -1;
				break;
				
				}	// end "else channelValuePtr == NULL && numberChannelsWritten == 0"
			
					// The length is forced to be 9
			
			//count = (UInt32)strlen ((char*)gTextString);
			count = 9;
			
			errCode = MWriteData (fileStreamPtr,
											&count, 
											(char*)gTextString,
											kErrorMessages);
			
			channelIndex++;
			channel++;
		
			}	// end "while (errCode == noErr && ..."

		}	// end "if (errCode == noErr && ..."
		
	return (errCode);

}	// end "WriteChannelValues"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 WriteOutputDataToFile
//
//	Software purpose:	The purpose of this routine is to write the data in the input
//							buffer to the output file at the location specified by the 
//							output file information structure.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/13/2005
//	Revised By:			Larry L. Biehl			Date: 02/26/2013

SInt16 WriteOutputDataToFile (
				FileInfoPtr							outFileInfoPtr,
				CMFileStream*						outFileStreamPtr,
				unsigned char						*ioOutBufferPtr, 
				SInt16*								channelsPtr,
				UInt32								numberOutChannels,
				UInt32								lastOutputWrittenLine,
				UInt32								numberBytesPerLineAndChannel,
				UInt32								numberOutputLines,
				UInt32								outBSQOffsetIncrement,
				UInt32								totalIOOutBytes,
				ReformatOptionsPtr				reformatOptionsPtr,
				UInt32								numberOutsideLoops)

{
	SInt64								fileBSQOffsetIncrement,
											numberHeaderBytes,
											positionOffset;
	
	UInt32								channelCount,
											count,
											numberBytesPerChannel,
											outputBufferOffset;
	
	SInt16								errCode = noErr;
	
		
	outputBufferOffset = 0;
	
	if (outFileInfoPtr->bandInterleave == kBSQ ||
												outFileInfoPtr->bandInterleave == kBNonSQ)
		{
				// Set position for the first channel to be written.
				
		numberHeaderBytes = outFileInfoPtr->numberHeaderBytes;
		if (reformatOptionsPtr->outputFileCode != kNewFileMenuItem)
			numberHeaderBytes = reformatOptionsPtr->modifyAppendBytesSkip;
			
		positionOffset = (SInt64)numberHeaderBytes + 
								(SInt64)lastOutputWrittenLine * numberBytesPerLineAndChannel;
									
		numberBytesPerChannel = totalIOOutBytes/numberOutChannels;
		fileBSQOffsetIncrement =
								(SInt64)numberOutputLines * numberBytesPerLineAndChannel;
					
		for (channelCount=1; channelCount<=numberOutChannels; channelCount++)
			{		
			if (outFileInfoPtr->swapBytesFlag && outFileInfoPtr->numberBytes >= 2)
				SwapBytes (outFileInfoPtr->numberBytes,
								&ioOutBufferPtr[outputBufferOffset],
								numberBytesPerChannel/outFileInfoPtr->numberBytes);
								
			if (outFileInfoPtr->bandInterleave == kBNonSQ)
				{
				numberHeaderBytes = 
					outFileInfoPtr->hfaPtr[channelsPtr[channelCount-1]].layerOffsetBytes;
				if (reformatOptionsPtr->outputFileCode != kNewFileMenuItem)
					numberHeaderBytes = reformatOptionsPtr->modifyAppendBytesSkip;
		
				positionOffset = numberHeaderBytes + 
										lastOutputWrittenLine * numberBytesPerLineAndChannel;
				
				}	// end "if (outFileInfoPtr->bandInterleave == kBNonSQ)"
			
			if (numberOutsideLoops == 1)
						// This implies that the data are being handled by line then by
						// channel as apposed to channel and then be line.
				errCode = MSetMarker (outFileStreamPtr, 
												fsFromStart, 
												positionOffset,
												kErrorMessages);
											
			count = numberBytesPerChannel;	
			
			if (errCode == noErr)		
				errCode = MWriteData (outFileStreamPtr, 
												&count, 
												&ioOutBufferPtr[outputBufferOffset],
												kErrorMessages);
											
			if (errCode != noErr)
				break;
			
			positionOffset += fileBSQOffsetIncrement;
			outputBufferOffset += outBSQOffsetIncrement;
											
			}	// end "for (channelCount=1; ..."
			
		}	// end "if outFileInfoPtr->bandInterleave == kBSQ || ..."
	
	else	// outFileInfoPtr->bandInterleave != kBSQ
		{
		if (outFileInfoPtr->swapBytesFlag && outFileInfoPtr->numberBytes >= 2)
			SwapBytes (outFileInfoPtr->numberBytes,
							&ioOutBufferPtr[outputBufferOffset],
							totalIOOutBytes/outFileInfoPtr->numberBytes);
								
		count = totalIOOutBytes;
		errCode = MWriteData (outFileStreamPtr, 
									&count, 
									&ioOutBufferPtr[outputBufferOffset],
									kErrorMessages);
			
		}	// end "else outFileInfoPtr->bandInterleave != kBSQ"
		
	return (errCode);
	
}	// end "WriteOutputDataToFile"             
