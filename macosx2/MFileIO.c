//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2018)
//						(c) Purdue Research Foundation
//								All rights reserved.
//
//	File:						fileIO.c
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/21/2017
//
//	Language:				C
//
//	System:					Macintosh Operating System
//
//	Brief description:	This file contains routines which are used to access
//								various disk files.
//
//	Functions in file:	pascal short int 	GetFileHook
//								short int 			GetImageFile
//								void 					GetImageNameFromSupportFile
//								void					NavSetImageWindowFileFlagToFalse
//								pascal short int 	PutFileHook
//								void 					SetImageWindowFileFlagToFalse
//
//	Diagram of MultiSpec routine calls for the routines in the file.
//
//		.
//		.
//		etc.
//
//		GetDefaultSupportFile
//			CopyPToP (in multiSpecUtilities.c)
//			RemoveCharsNoCase (in multiSpecUtilities.c)
//			AppendPascaltoPascal (in multiSpecUtilities.c)
//			IOCheck (in fileIO.c)
//			CloseFile (in fileIO.c)
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//------------------------------------------------------------------------------------

//#include	"SMultiSpec.h"
//#include	"SExtGlob.h"


		// Prototypes for routines used just in this file.								
		
void 		GetImageNameFromSupportFile (
				LocalAppFilePtr					localAppFilePtr);

void		SetImageWindowFileFlagToFalse (
				DialogPtr							dialogPtr);


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal SInt16 GetFileHook
//
//	Software purpose:	The purpose of this routine is to handle the 
//							processing of the special items that have been added
//							to the standard GetFile routine.
//		
//	Parameters in:		Pointer to dialog window
//							Item number that was hit
//
//	Parameters out:	None
//
// Value Returned:	Item number that was input.
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/10/1990
//	Revised By:			Larry L. Biehl			Date: 09/08/2006

pascal SInt16 GetFileHook (
				SInt16								itemNumber, 
				DialogPtr							dialogPtr)

{
	if (gGetFileStatus <= 0 || gMultipleImageFileCode)
		{
				// Item 16: Set routine to draw outline on "OK" box.				
			
		SetDialogItemDrawRoutine (dialogPtr, 16, gHiliteOKButtonPtr);
		
		}	// end "if (gGetFileStatus <= 0 || gMultipleImageFileCode)" 
		
	if (gGetFileStatus == -1)
		{
		HideDialogItems (dialogPtr, 12, 15);
		HideDialogItem (dialogPtr, 17);
		
		}	// end "if (gImageType == -1)" 
			
	if (gGetFileStatus == 0)
		{
		gGetFileImageType = 0;
		
		SetImageWindowFileFlagToFalse (dialogPtr);
		
		if	(gActiveImageWindowInfoH == NULL ||
			((WindowInfoPtr)*gActiveImageWindowInfoH)->windowType != kImageWindowType ||
				((WindowInfoPtr)*gActiveImageWindowInfoH)->projectBaseImageFlag ||
					((WindowInfoPtr)*gActiveImageWindowInfoH)->bandInterleave == kBIS)
			HideDialogItem (dialogPtr, 17);
		
		}	// end "if (gGetFileStatus == 0)" 
		
	if (gMultipleImageFileCode == 2)
		{
		SetDLogControl (dialogPtr, 13, 0);
		SetDLogControl (dialogPtr, 14, 1);
		SetDLogControl (dialogPtr, 15, 0);
		
		HideDialogItem (dialogPtr, 13);
		HideDialogItem (dialogPtr, 15);
		
		SetDLogControl (dialogPtr, 17, 1);
		if (gGetFileStatus == 2)
			SetDLogControlHilite (dialogPtr, 17, 255);

		}	// end "if (gMultipleImageFileCode == 2)" 
	
	if (gGetFileStatus == 1)		
		gGetFileStatus = 3;
	
	if (gGetFileStatus <= 0)	
		gGetFileStatus = 1;
	
	switch (itemNumber)
		{
		case 13:
			SetDLogControl (dialogPtr, 13, 1);
			SetDLogControl (dialogPtr, 14, 0);
			SetDLogControl (dialogPtr, 15, 0);
			gGetFileImageType = 0;
			break;
			
		case 14:
			SetDLogControl (dialogPtr, 13, 0);
			SetDLogControl (dialogPtr, 14, 1);
			SetDLogControl (dialogPtr, 15, 0);
			gGetFileImageType = kMultispectralImageType;
			break;
			
		case 15:
			SetDLogControl (dialogPtr, 13, 0);
			SetDLogControl (dialogPtr, 14, 0);
			SetDLogControl (dialogPtr, 15, 1);
			gGetFileImageType = kThematicImageType;
			break;
			
		case 17:
			if (gMultipleImageFileCode == 2)
				SetImageWindowFileFlagToFalse (dialogPtr);
				
			else	// gMultipleImageFileCode != 2 
				{
				SetDLogControl (dialogPtr, 17, 1);
				gMultipleImageFileCode = 2;
			
				HideDialogItem (dialogPtr, 13);
				HideDialogItem (dialogPtr, 15);
				SetDLogControl (dialogPtr, 13, 0);
				SetDLogControl (dialogPtr, 14, 1);
				SetDLogControl (dialogPtr, 15, 0);
				
				if (MGetString (gTextString, kFileIOStrID, IDS_SelectFileToLink))
					{
					sprintf ((char*)&gTextString[13], 
						"%3d", 
						((WindowInfoPtr)*gActiveImageWindowInfoH)->numberImageFiles+1);
					gTextString[16] = ' ';
		
					LoadDItemString (dialogPtr, 11, &gTextString);
					
					}	// end "if (MGetString (..." 
				
				}	// end "else !gMultipleImageFileCode"
			break;
			
		}	// end "switch (itemNumber)" 
		
	return (itemNumber);
	
}	// end "GetFileHook" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetImageFile
//
//	Software purpose:	The purpose of this routine is to put up the
//							standard get file dialog box to allow the user to
//							select the file to read from.
//		
//	Parameters in:				
//
//	Parameters out:			
//
// Value Returned:	Error code for file operations.
//
// Called By:			Menus in menus.c
//
//	Coded By:			Larry L. Biehl			Date: 09/03/1993
//	Revised By:			Larry L. Biehl			Date: 09/19/2017

void GetImageFile (void)

{
	SInt32								projectVersionCode;
	
	SInt16								returnError;
	
	
	if (gAppleEventsFlag)
		{
		returnError = SendAEOpenOrPrintDocument (NULL, kAEOpenDocuments);
		
		}	// end "if (gAppleEventsFlag)" 
	 
	 else	// !gAppleEventsFlag 
	 	{    	
		projectVersionCode = OpenImageFile (NULL, FALSE, 0);
 	      		
 	   if (projectVersionCode > 0)
			DoOpenProjectFile (projectVersionCode);
						
		}	// end "else !gAppleEventsFlag" 
	
	return;

}	// end "GetImageFile" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetImageNameFromSupportFile
//
//	Software purpose:	The purpose of this routine is to convert the name of 
//							the input support file the associated image file.  
//							This routine works behind the scenes. If the file is 
//							not found then no change is made in the input structure.
//
//	Parameters in:		Pointer to AppFile structure which contains the
//								volume reference number parent ID and file name.
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			main in multiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 09/08/1993
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

void GetImageNameFromSupportFile (
				LocalAppFilePtr					localAppFilePtr)

{
	CMFileStream						*aliasedFileStreamPtr,
											*fileStreamPtr;
											
	SInt16								count,
											index,
											returnCode;
	
	Boolean								checkAliasedFlag,
											continueFlag,
											foundFlag;
	
	
	aliasedFileStreamPtr = NULL;
	fileStreamPtr = NULL;
	returnCode = -1;
	
			// Get some temporary memory to be used to stored the file			
			// information in for the input file and any target of an			
			// aliased file if the input file is an aliased file.					
			
	fileStreamPtr = InitializeFileStream ((CMFileStream*)NULL);
	continueFlag = (fileStreamPtr != NULL);
	
	if (continueFlag)
		{
		aliasedFileStreamPtr = InitializeFileStream ((CMFileStream*)NULL);
		continueFlag = (aliasedFileStreamPtr != NULL);
		
		}	// end "if (continueFlag)" 
		
	if (continueFlag)
		{
				// Initialize the new file stream structures.
				
		fileStreamPtr->vRefNum = aliasedFileStreamPtr->vRefNum = 
																			localAppFilePtr->vRefNum;
		fileStreamPtr->parID = aliasedFileStreamPtr->parID = 
																			localAppFilePtr->parID;
		CopyPToP (fileStreamPtr->fileName, localAppFilePtr->fNamePtr);
		CopyPToP (aliasedFileStreamPtr->fileName, localAppFilePtr->fNamePtr);
		fileStreamPtr->fSSpecFlag = aliasedFileStreamPtr->fSSpecFlag = 
																		localAppFilePtr->fSSpecFlag;
																
		ResolveAnyAliases (aliasedFileStreamPtr, &checkAliasedFlag);
		
		}	// end "if (continueFlag)" 
	
	if (continueFlag)
		{
		foundFlag = FALSE;
		index = 0;
		count = 7;
		
		while (index<count && !foundFlag)
			{
			if (index == 0)
				{
				if (localAppFilePtr->type == 'ISTA')
					{
							// Remove the standard suffix for the input file type.
						
					RemoveCharsNoCase ((char*)"\p.STA", fileStreamPtr->fileName);
					
					}	// end "if (	localAppFilePtr->type == 'ISTA')" 
					
				else if (localAppFilePtr->type == 'ITRL')
					{
							// Remove the standard suffix for the input file type.
						
					RemoveCharsNoCase ((char*)"\p.trl", fileStreamPtr->fileName);
					
					}	// end "if (	localAppFilePtr->type == 'ITRL')" 
					
				else if (localAppFilePtr->type == 'TRAN')
					{
							// Remove the standard suffix for the input file type.
						
					RemoveCharsNoCase ((char*)"\p.TransformMatrix",
												fileStreamPtr->fileName);
					
					}	// end "if (fileType == 'TRAN')"
						
				}	// end "if (index == 0)" 
				
			if (fileStreamPtr->fSSpecFlag)
				returnCode = FSpGetFInfo ((FSSpec*)fileStreamPtr, &gFinderInfo);
				
			else	// !fileInfoPtr->fSSpecFlag 
				returnCode = HGetFInfo (
						fileStreamPtr->vRefNum, 0, fileStreamPtr->fileName, &gFinderInfo);
				
			foundFlag = (returnCode == noErr);
		
			if (foundFlag)
				{
				localAppFilePtr->vRefNum = fileStreamPtr->vRefNum;
				localAppFilePtr->parID = fileStreamPtr->parID;
				CopyPToP (localAppFilePtr->fNamePtr, fileStreamPtr->fileName);
				localAppFilePtr->fSSpecFlag = fileStreamPtr->fSSpecFlag;
				localAppFilePtr->type = gFinderInfo.fdType;
				
				}	// end "if (foundFlag)" 
				
			else	// !foundFlag 
				{
				index++;
				
						// Try a different suffix at the end of the file name.	
				
				if (localAppFilePtr->type == 'ISTA' || localAppFilePtr->type == 'TRAN')
					{
					if (index == 1)		
						ConcatFilenameSuffix (fileStreamPtr->fileName, (UCharPtr)"\p.lan");
					
					else if (index == 2)
						{		
						RemoveCharsNoCase ((char*)"\p.LAN", fileStreamPtr->fileName);
						ConcatFilenameSuffix (fileStreamPtr->fileName, (UCharPtr)"\p.Lan");
						
						}	// end "if (index == 1)" 
					
					else if (index == 3)
						{		
						RemoveCharsNoCase ((char*)"\p.Lan", fileStreamPtr->fileName);
						ConcatFilenameSuffix (fileStreamPtr->fileName, (UCharPtr)"\p.LAN");
						
						}	// end "if (index == 3)" 
					
					else if (index == 4)
						{		
						RemoveCharsNoCase ((char*)"\p.lan", fileStreamPtr->fileName);
						ConcatFilenameSuffix (fileStreamPtr->fileName, (UCharPtr)"\p.dat");
						
						}	// end "if (index == 4)" 
					
					else if (index == 5)
						{		
						RemoveCharsNoCase ((char*)"\p.dat", fileStreamPtr->fileName);
						ConcatFilenameSuffix (fileStreamPtr->fileName, (UCharPtr)"\p.DAT");
						
						}	// end "if (index == 5)" 
					
					else if (index == 6)
						{		
						RemoveCharsNoCase ((char*)"\p.DAT", fileStreamPtr->fileName);
						ConcatFilenameSuffix (fileStreamPtr->fileName, (UCharPtr)"\p.tif");
						
						}	// end "if (index == 6)" 
						
					}	// end "if (localAppFilePtr->type == 'ISTA')" 
				
				if (localAppFilePtr->type == 'ITRL')
					{
					if (index == 1)		
						ConcatFilenameSuffix (fileStreamPtr->fileName, (UCharPtr)"\p.gis");
					
					else if (index == 2)
						{		
						RemoveCharsNoCase ((char*)"\p.GIS", fileStreamPtr->fileName);
						ConcatFilenameSuffix (fileStreamPtr->fileName, (UCharPtr)"\p.Gis");
						
						}	// end "if (index == 1)" 
					
					else if (index == 3)
						{		
						RemoveCharsNoCase ((char*)"\p.Gis", fileStreamPtr->fileName);
						ConcatFilenameSuffix (fileStreamPtr->fileName, (UCharPtr)"\p.GIS");
						
						}	// end "if (index == 3)" 
						
					}	// end "if (localAppFilePtr->type == 'ITRL')" 
					
				if (index >= count && checkAliasedFlag)
					{
					index = 0;
					DisposePtr ((Ptr)fileStreamPtr);
					fileStreamPtr = aliasedFileStreamPtr;
					aliasedFileStreamPtr = NULL;
					checkAliasedFlag = FALSE;
					
					}	// end "if (index >= count && checkAliasedFlag)" 
				
				}	// end "if (!foundFlag)" 
			
			}	// end "while (index<count && !foundFlag)" 
			
		}	// end "if (continueFlag)" 
		
	fileStreamPtr = (CMFileStream*)CheckAndDisposePtr ((Ptr)fileStreamPtr);
	aliasedFileStreamPtr = (CMFileStream*)CheckAndDisposePtr (
																			(Ptr)aliasedFileStreamPtr);
		
	return;
			
}	// end "GetImageNameFromSupportFile"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 OpenMultiSpecDocument
//
//	Software purpose:	The purpose of this routine is to open the requested
//							input document.  The document can be an image file or
//							a project file.
//
//	Parameters in:		Pointer to AppFile structure which contains the
//								volume reference number and file name.
//							Index in the list
//
//	Parameters out:	=-2 if no image was loaded.
//							=0 if document was openned okay.
//							=1 if document was a project file.  It will be openned last.
//
// Value Returned:	None
//
// Called By:			DoOpenOrPrintDocument in appleEvents.c
//
//	Coded By:			Larry L. Biehl			Date: 09/01/1993
//	Revised By:			Larry L. Biehl			Date: 09/19/2017	

SInt16 OpenMultiSpecDocument (
				LocalAppFilePtr					localAppFilePtr)

{
	SInt16			returnCode;
	
	
	returnCode = -1;
	
	
	if (localAppFilePtr->type == 'ISTA' || 
			localAppFilePtr->type == 'ITRL' || 
				localAppFilePtr->type == 'TRAN')
		GetImageNameFromSupportFile (localAppFilePtr);
	
	if (localAppFilePtr->type != 'ISTA' && 
			localAppFilePtr->type != 'ITRL' && 
				localAppFilePtr->type != 'TRAN')
		{
		gProcessorCode = kOpenImageFileProcessor;
		returnCode = OpenImageFile (localAppFilePtr, FALSE, 0);
		gProcessorCode = 0;
		if (returnCode == -1)
			returnCode = 0;
			
		}	// end "if (localAppFilePtr->type != 'ISTA' && ..." 
		
	return (returnCode);
			
}	// end "OpenMultiSpecDocument"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal SInt16 PutFileHook
//
//	Software purpose:	The purpose of this routine is to handle the 
//							processing of the special items that have been added
//							to the standard PutFile routine.
//		
//	Parameters in:		Pointer to dialog window
//							Item number that was hit
//
//	Parameters out:	None
//
// Value Returned:	Item number that was input.
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/27/1992
//	Revised By:			Larry L. Biehl			Date: 02/14/1994

pascal SInt16 PutFileHook (
				SInt16								itemNumber, 
				DialogPtr							dialogPtr)

{
	Rect									theBox;
	
	Handle								theHandle;
	
	SInt16								theType;
	
	Boolean								changeCheckBoxFlag;
	
	
	if (gGetFileStatus == -1)
		{
		HideDialogItems (dialogPtr, 9, 13);
		
		}	// end "if (gImageType == -1)" 
			
	if (gGetFileStatus == 0)
		{
		if (gProjectInfoPtr->keepClassStatsOnlyFlag)
			{
			SetDLogControlHilite (dialogPtr, 10, 255);
			SetDLogControlHilite (dialogPtr, 11, 255);
			
			gProjectInfoPtr->saveFieldSumsSquaresFlag = FALSE;
			gProjectInfoPtr->saveFieldMeansCovFlag = FALSE;
			
			}	// end "if (gProjectInfoPtr->keepClassStatsOnlyFlag)" 
		
		if (gProjectInfoPtr->classChanStatsPtr == NULL)
			{
			SetDLogControlHilite (dialogPtr, 12, 255);
			SetDLogControlHilite (dialogPtr, 13, 255);
			
			gProjectInfoPtr->saveClassSumsSquaresFlag = FALSE;
			gProjectInfoPtr->saveClassMeansCovFlag = FALSE;
			
			}	// end "if (gProjectInfoPtr->classChanStatsPtr == ...)" 
			
		SetDLogControl (dialogPtr, 10, gProjectInfoPtr->saveFieldSumsSquaresFlag);
		SetDLogControl (dialogPtr, 11, gProjectInfoPtr->saveFieldMeansCovFlag);
		SetDLogControl (dialogPtr, 12, gProjectInfoPtr->saveClassSumsSquaresFlag);
		SetDLogControl (dialogPtr, 13, gProjectInfoPtr->saveClassMeansCovFlag);
		
		gGetFileStatus = 2;
		
		}	// end "if (gGetFileStatus == 0)" 
	
	if (gGetFileStatus <= 0)	
		gGetFileStatus = 1;
	
	changeCheckBoxFlag = FALSE;
	
	switch (itemNumber)
		{
		case 10:
			gProjectInfoPtr->saveFieldSumsSquaresFlag = !
														gProjectInfoPtr->saveFieldSumsSquaresFlag;
			changeCheckBoxFlag = TRUE;
			break;
			
		case 11:
			gProjectInfoPtr->saveFieldMeansCovFlag = !
														gProjectInfoPtr->saveFieldMeansCovFlag;
			changeCheckBoxFlag = TRUE;
			break;
			
		case 12:
			gProjectInfoPtr->saveClassSumsSquaresFlag = !
														gProjectInfoPtr->saveClassSumsSquaresFlag;
			changeCheckBoxFlag = TRUE;
			break;
			
		case 13:
			gProjectInfoPtr->saveClassMeansCovFlag = !
														gProjectInfoPtr->saveClassMeansCovFlag;
			changeCheckBoxFlag = TRUE;
			break;
			
		}	// end "switch (itemNumber)" 
	
	if (changeCheckBoxFlag)
		{
		GetDialogItem (dialogPtr, itemNumber, &theType, &theHandle, &theBox);
		ChangeDLogCheckBox ((ControlHandle)theHandle);
		
		}	// end "if (changeCheckBoxFlag)" 
		
	if (gGetFileStatus == 2 && itemNumber == 1)
		{
		if (!ClusterStatisticsVerification ())
			itemNumber = 0;
			
		}	// end "if (gGetFileStatus == 1 && itemNumber == 1)" 
		
	return (itemNumber);
	
}	// end "PutFileHook"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void NavSetImageWindowFileFlagToFalse
//
//	Software purpose:	The purpose of this routine is to set the items in the
//							file dialog to the proper settings when the
//							ImageWindowFileFlag is to be set to FALSE.
//		
//	Parameters in:		Pointer to dialog window
//
//	Parameters out:	None
//
// Value Returned:	None.
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/21/1991
//	Revised By:			Larry L. Biehl			Date: 12/13/2012

void NavSetImageWindowFileFlagToFalse (
				DialogPtr							dialogPtr,
				SInt16								customStart)

{
	ShowDialogItem (dialogPtr, customStart+1);
	ShowDialogItem (dialogPtr, customStart+3);
	
	SetDLogControl (dialogPtr, customStart+2, 0);
				
	if (gGetFileImageType == 0)
		SetDLogControl (dialogPtr, customStart+1, 1);
	
	if (gGetFileImageType == kMultispectralImageType)
		SetDLogControl (dialogPtr, customStart+2, 1);
		
	if (gGetFileImageType == kThematicImageType)
		SetDLogControl (dialogPtr, customStart+3, 1);
		
	gMultipleImageFileCode = 0;
	//SetDLogControl (dialogPtr, customStart+4, 0);
				
	if (MGetString (gTextString, kFileIOStrID, IDS_SelectImage))
		SetWTitle (GetDialogWindow (dialogPtr), (UCharPtr)&gTextString);
			
}	// end "NavSetImageWindowFileFlagToFalse" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetImageWindowFileFlagToFalse
//
//	Software purpose:	The purpose of this routine is to set the items in the
//							file dialog to the proper settings when the
//							ImageWindowFileFlag is to be set to FALSE.
//		
//	Parameters in:		Pointer to dialog window
//
//	Parameters out:	None
//
// Value Returned:	None.
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/21/1991
//	Revised By:			Larry L. Biehl			Date: 09/08/2006

void SetImageWindowFileFlagToFalse (
				DialogPtr							dialogPtr)

{
	ShowDialogItem (dialogPtr, 13);
	ShowDialogItem (dialogPtr, 15);
	
	SetDLogControl (dialogPtr, 14, 0);
				
	if (gGetFileImageType == 0)
		SetDLogControl (dialogPtr, 13, 1);
	
	if (gGetFileImageType == kMultispectralImageType)
		SetDLogControl (dialogPtr, 14, 1);
		
	if (gGetFileImageType == kThematicImageType)
		SetDLogControl (dialogPtr, 15, 1);
		
	gMultipleImageFileCode = 0;
	SetDLogControl (dialogPtr, 17, 0);
				
	if (MGetString (gTextString, kFileIOStrID, IDS_SelectImage))
		LoadDItemString (dialogPtr, 11, &gTextString);
			
}	// end "SetImageWindowFileFlagToFalse" 
