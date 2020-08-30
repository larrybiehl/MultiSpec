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
//	File:						SDialogUtilities.cpp
//
//	Authors:					Larry L. Biehl, Ravi Budruk
//
//	Revision date:			08/25/2020
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	The purpose of the routines in this file is to
//								provide utility type functions in MultiSpec.
/*
	Template for debugging
		int numberChars = sprintf ((char*)gTextString3,
													" SDialogUtilities: (): %s",
													gEndOfLine);
		ListString ((char*)gTextString3, numberChars, gOutputTextH);	
*/
//------------------------------------------------------------------------------------
                
#include "SMultiSpec.h"  

#if defined multispec_wx
	#include "wx/wx.h"
	#include "wx/msgdlg.h"
	#include "wx/string.h"
	#include "wx/colour.h"
	#include "xMultiSpec.h"
	#include "xStatusDialog.h"
#endif

#if defined multispec_mac  
	#define kContinueStopAlertID		1169
#endif	// defined multispec_mac

#if defined multispec_win     
	#include "SImageWindow_class.h"
	#include "WStatusDialog.h"
#endif	// defined multispec_win

//GetDialogItem (dialog, itemIndex, &itemType, &itemHandle, &itemRect);
//itemType |= itemDisable;
//SetDialogItem (dialog, itemIndex, itemType, itemHandle, &itemRect);
// kStaticTextDialogItem         = 8,
//  kEditTextDialogItem           = 16,
//  kIconDialogItem               = 32,
//  kPictureDialogItem            = 64,
//  kUserDialogItem               = 0,
//  kHelpDialogItem               = 1,
//  kItemDisableBit               = 128



/*                       
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ActivateDialogItem
//
//	Software purpose:	The purpose of this routine is to activate the specified
//							dialog item.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/19/2012
//	Revised By:			Larry L. Biehl			Date: 03/26/2012

void ActivateDialogItem (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{   
	if (dialogPtr != NULL && itemNumber > 0)
		{   
		#if defined multispec_mac 
		
			Handle					theHandle;
			Rect						theBox; 
			SInt16					theType; 
		
			GetDialogItem (dialogPtr, itemNumber, &theType, &theHandle, &theBox);
			
			if (theHandle != NULL && (theType & kItemDisableBit))			
				{
				theType -= kItemDisableBit;			
				SetDialogItem (dialogPtr, itemNumber, theType, theHandle, &theBox);
				InsetRect (&theBox, -2, -2);
				InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
				
				}	// end "if (theHandle != NULL && theType & kItemDisableBit)"
			
		#endif	// defined multispec_mac

		#if defined multispec_win
			dialogPtr->GetDlgItem (itemNumber)->SendMessage (EM_SETREADONLY,0,0);
		#endif // defined multispec_win 
		
		}	// end "if (dialogPtr != NULL && itemNumber > 0)" 
	
}	// end "ActivateDialogItem" 
*/


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt32 AddChannelsToDialogList
//
//	Software purpose:	The purpose of this routine is to add the channels and
//							descriptions to the input list.
//
//	Parameters in:					
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			OnInitDialog in xChannelsDialog.cpp
//							OnInitDialog in xFalseColorDialog.cpp
//							FalseColorPaletteDialog in SPalette.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/10/1996
//	Revised By:			Larry L. Biehl			Date: 08/25/2020

SInt32 AddChannelsToDialogList (
			#ifdef multispec_wx
				wxListBox*							listHandle,
			#endif
			#ifndef multispec_wx
				ListHandle							listHandle,
			#endif
				SInt16*								numberOutputFeatures, 
				SInt16* 								selectedFeaturePtr, 
				LayerInfoPtr						layerInfoPtr,
				FileInfoPtr							fileInfoPtr,
				SInt16								listType, 
				SInt16* 								availableFeaturePtr, 
				SInt32								numberInputChannels,
				SInt16								currentSelection)

{
	Cell									cell;
	ChannelDescriptionPtr			channelDescriptionPtr;
	
	char									*spacer = (char*)": ";
	
	FileInfoPtr							localFileInfoPtr;
	
	SInt32								estimatedLengthListDescription;

	SInt16								channel,
											channelNum,
											fileChanNum,
											fileInfoIndex,
											index,
											row,
											startString;
									
									
			// Initialize local variables.													
			
	channelDescriptionPtr = NULL;
	fileInfoIndex = -1;
	localFileInfoPtr = NULL;
   index = 0;
	
			// Get estimate of the length of the description in the list.			
			// Use 30 for the description list - 4 for channel number and 26
			// for the channel description.													
		
	estimatedLengthListDescription = (SInt32)numberInputChannels * 30;
									
	if (listHandle != NULL)
		{
		#if defined multispec_mac
					// If list type is 2 or 3, change the select specification.			
			
			if (listType == kItemsListOnly || listType == kSelectedItemsListOnly)
				(*listHandle)->selFlags = lOnlyOne;
		#endif	// defined multispec_mac
		
				// Put the channels into the list.											
				
		LSetDrawingMode (FALSE, listHandle);
		
		if (numberInputChannels > 0)
			{
			row = LAddRow ((SInt16)numberInputChannels, 0, listHandle);
			 
			index = 0;
			cell.h = 0;
			for (channel=0; channel<numberInputChannels; channel++)
				{																
						// Add channel number and description to the channel list.	
							
				cell.v = channel;
				
						// Get channel number from available feature list if it 		
						// exists.  Otherwise assume all channels.						
						
				if (availableFeaturePtr != NULL)
					channelNum = availableFeaturePtr[channel];
					
				else	// availableFeaturePtr == NULL
					channelNum = channel;
				
				channelNum++;		
				NumToString ((UInt32)channelNum, gTextString);
				
						// If description for channel exists, add the description	
						// the cell.																
						// First get a pointer to the channel descriptions if 		
						// they exist.																
				
				if (fileInfoPtr != NULL && layerInfoPtr != NULL &&
							fileInfoIndex != (SInt16)layerInfoPtr[channelNum].fileInfoIndex)
					{
					if (fileInfoIndex != -1)	
						CheckAndUnlockHandle (localFileInfoPtr->channelDescriptionH);
						
					fileInfoIndex = layerInfoPtr[channelNum].fileInfoIndex;
					localFileInfoPtr = &fileInfoPtr[fileInfoIndex];
		
					channelDescriptionPtr = NULL;
					if (localFileInfoPtr->channelDescriptionH != NULL && 
											estimatedLengthListDescription < SInt16_MAX)
						{
						channelDescriptionPtr = (ChannelDescriptionPtr)GetHandlePointer (
													localFileInfoPtr->channelDescriptionH, 
													kLock);
						
						}	// end "if (fileInfoPtr != NULL && fileInfoPtr->...)" 
						
					}	// end "if (fileInfoPtr != NULL && ..." 
						
				if (channelDescriptionPtr != NULL)
					{
					startString = gTextString[0] + 1;
					BlockMoveData (spacer, &gTextString[startString], 2);
																
					startString += 2;
					fileChanNum = layerInfoPtr[channelNum].fileChannelNumber;
					BlockMoveData (&channelDescriptionPtr[fileChanNum-1],
																&gTextString[startString], 24);
					gTextString[0] += 26;
					gTextString[gTextString[0]+1] = 0;
					
					}	// end "if (channelDescriptionPtr != NULL)" 
					
						// Load the string into the cell.									
						
				LSetCell ((char*)&gTextString[1],
								(short int)gTextString[0],
								cell,
								listHandle);
			
						// Make certain that we didn't have a memory with the last	
						// setting of the list for the cell.								
						
				if (gMemoryError != noErr)
					break;
				
						// Select the cell if the channel is in the selected 			
						// channel list and the list type is 1, 3 or 4.					
				
				Boolean tempFlag = FALSE;
				if (listType == kSelectItemsList || 
											listType == kSelectedItemsListOnly ||
													listType == kSelectPCList)
					{
					if (currentSelection == kAllMenuItem || 
							(index<*numberOutputFeatures &&
													selectedFeaturePtr[index] == (SInt16)channel))
						tempFlag = TRUE;
						
					}	// end "if (listType == kSelectItemsList || ..."
					
				LSetSelect (tempFlag, cell, listHandle);
						
				if (tempFlag)
					index++; 
				
				}	// end "for (channel=0; ... 
				
			}	// end "if (numberInputChannels > 0)" 
	
				// Scroll to the hilited list item.												
		                  		
		LAutoScroll (listHandle);
			
				// Turn list drawing mode back on.											
	  				
		LSetDrawingMode (TRUE, listHandle);
	
		if (localFileInfoPtr != NULL)
			CheckAndUnlockHandle (localFileInfoPtr->channelDescriptionH);
		
		}	// end "if (listHandle != NULL)"
		
	return (index);

}	// end "AddChannelsToDialogList"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 CheckMaxValue
//
//	Software purpose:	The purpose of this routine is to check the input dialog item
//							value and verify that it is not 0 and not more than the maximum
//							value allowed.
//
//	Parameters in:					
//
//	Parameters out:	None
//
// Value Returned:	FALSE indicating the no more changes need to be made.
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: ??/??/????
//	Revised By:			Larry L. Biehl			Date: 02/27/2013

SInt16 CheckMaxValue (
				DialogPtr							dialogPtr,
				SInt16								itemNumber,
				SInt32								minValue,
				SInt32								maxValue,
				Boolean								showAlertMessageFlag)
										
{
	SInt16								itemHit = 1;

	SInt32								theNum;
											


	theNum = GetDItemValue (dialogPtr, itemNumber);
	
	if (theNum < (SInt32)minValue || theNum > (SInt32)maxValue)
		{
		if (showAlertMessageFlag)
			{
			SetDLogControlHilite (dialogPtr, 1, 255);
			
			if (LoadSpecifiedStringNumberLongP (kAlertStrID,
															IDS_Alert91, 
															(char*)gTextString,
															(char*)gTextString2,
															TRUE,
															(SInt32)minValue,
															(SInt32)maxValue))
				{
				DisplayAlert (kErrorAlertID, kStopAlert, 0, 0, 0, gTextString);
					
				#if defined multispec_mac
							// Force the main dialog box to be redrawn.
						
					InvalWindowRect (GetDialogWindow (dialogPtr), 
											GetPortBounds (GetDialogPort (dialogPtr), &gTempRect));
				#endif	// defined multispec_mac

				}	// end "if (LoadSpecifiedStringNumberLongP (..."
				
			SetDLogControlHilite (dialogPtr, 1, 0);
				
			SelectDialogItemText (dialogPtr, itemNumber, 0, SHRT_MAX);
								
			}	// end "if (showAlertMessageFlag)"
		
		else	// !showAlertMessageFlag	
			NumberErrorAlert (theNum, dialogPtr, itemNumber);
						
		itemHit = 0;
		
		}	// end "if (theNum < minValue || theNum > maxValue)"
		
	return (itemHit);
					
}	// end "CheckMaxValue" 


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 CheckDialogRealValue
//
//	Software purpose:	The purpose of this routine is to check the input dialog item
//							real value and verify that it is fits with the range allowed.
//                   Currently this routine is only being used in the linux version.
//
//	Parameters in:					
//
//	Parameters out:	None
//
// Value Returned:	FALSE indicating the no more changes need to be made.
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/22/2015
//	Revised By:			Larry L. Biehl			Date: 01/20/2016

SInt16 CheckDialogRealValue (
				DialogPtr							dialogPtr,
				SInt16								itemNumber,
				double								minValue,
				double								maxValue,
				SInt16								numberDecimalPlaces,
				Boolean								minValueAllowedFlag,
				Boolean								maxValueAllowedFlag,
				Boolean								showAlertMessageFlag)
										
{
	double								theRealValue = 0;
	
	SInt16								numberChars,
											returnCode = 0;
	
	Boolean								errorFlag = FALSE;
											

	#ifdef multispec_mac
		theRealValue = GetDItemRealValue (dialogPtr, itemNumber);
	#endif	// defined multispec_mac

	#if defined multispec_wx
		//wxTextCtrl* textctrl = (wxTextCtrl*)dialogPtr->FindWindow (itemNumber);
      wxString stringValue;
      wxTextCtrl* textctrl = wxDynamicCast (
												dialogPtr->FindWindow (itemNumber), wxTextCtrl);
      if (textctrl != NULL)
			{
         stringValue = textctrl->GetValue ();
			
			}	// end "if (textctrl != NULL)"
	
		else	// textctrl == NULL
			{
         wxComboBox* combobox = wxDynamicCast (
												dialogPtr->FindWindow (itemNumber), wxComboBox);
         stringValue = combobox->GetValue ();
			
			}	// end "else textctrl == NULL"
      
      theRealValue = wxAtof (stringValue);
	#endif   // defined multispec_wx
	
	if (minValueAllowedFlag)
		errorFlag = (theRealValue < minValue);
	else
		errorFlag = (theRealValue <= minValue);
		
	if (!errorFlag)
		{
		if (maxValueAllowedFlag)
			errorFlag = (theRealValue > maxValue);
		else
			errorFlag = (theRealValue >= maxValue);
			
		}	// end "if (!errorFlag)"

	if (errorFlag)
		{
		if (showAlertMessageFlag)
			{
			SetDLogControlHilite (dialogPtr, 1, 255);
			
			if (minValueAllowedFlag && maxValueAllowedFlag)
				numberChars = sprintf ((char*)&gTextString[1],
													"Enter value >= %.*f and <= %.*f.",
													numberDecimalPlaces, 
													minValue,
													numberDecimalPlaces,
													maxValue);	
			
			else if (minValueAllowedFlag && !maxValueAllowedFlag)
				numberChars = sprintf ((char*)&gTextString[1],
													"Enter value >= %.*f and < %.*f.",
													numberDecimalPlaces, 
													minValue,
													numberDecimalPlaces,
													maxValue);
			
			else if (!minValueAllowedFlag && maxValueAllowedFlag)
				numberChars = sprintf ((char*)&gTextString[1],
													"Enter value > %.*f and <= %.*f.",
													numberDecimalPlaces, 
													minValue,
													numberDecimalPlaces,
													maxValue);
			
			else 
				numberChars = sprintf ((char*)&gTextString[1],
													"Enter value > %.*f and < %.*f.",
													numberDecimalPlaces, 
													minValue,
													numberDecimalPlaces,
													maxValue);
													
			gTextString[0] = (UInt8)numberChars;
													
			DisplayAlert (kErrorAlertID, kStopAlert, 0, 0, 0, gTextString);
				
			#if defined multispec_mac
						// Force the main dialog box to be redrawn.
					
				InvalWindowRect (GetDialogWindow (dialogPtr), 
										GetPortBounds (GetDialogPort (dialogPtr), &gTempRect));
			#endif	// defined multispec_mac

			SetDLogControlHilite (dialogPtr, 1, 0);
				
			SelectDialogItemText (dialogPtr, itemNumber, 0, SHRT_MAX);
								
			}	// end "if (showAlertMessageFlag)"
		
		//else	// !showAlertMessageFlag	
		//	NumberErrorAlert (theNum, dialogPtr, itemNumber);
			
		returnCode = itemNumber;
						
		}	// end "if (errorFlag)"
		
	return (returnCode);
					
}	// end "CheckDialogRealValue" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CheckFeatureTransformationDialog
//
//	Software purpose:	The purpose of this routine is to update the
//							feature transformation dialog items.
//
//	Parameters in:					
//
//	Parameters out:	None
//
// Value Returned:	FALSE indicating the no more changes need to be made.
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/30/1993
//	Revised By:			Larry L. Biehl			Date: 06/24/2016

Boolean CheckFeatureTransformationDialog (
				DialogPtr							dialogPtr, 
				Boolean								featureTransformAllowedFlag,
				SInt16								featureTransformItem, 
				SInt16								channelFeatureItem, 
				Boolean*								featureTransformationFlagPtr)

{	
	SInt16								stringNumber;
	
		
	if (featureTransformAllowedFlag)
		SetDLogControlHilite (dialogPtr, featureTransformItem, 0);
		
	else	// !featureTransformAllowedFlag 
		{
		*featureTransformationFlagPtr = FALSE;
		SetDLogControlHilite (dialogPtr, featureTransformItem, 255);
	
		}	// end "else !featureTransformAllowedFlag" 
		
	SetDLogControl (dialogPtr, featureTransformItem, *featureTransformationFlagPtr);
	
	stringNumber = IDS_Dialog17;
	if (*featureTransformationFlagPtr)
		stringNumber = IDS_Dialog16;
		
	LoadDItemStringNumber (kDialogStrID, 
										stringNumber, 
										dialogPtr, 
										channelFeatureItem, 
										(Str255*)gTextString);
										
	return (FALSE);
		
}	// end "CheckFeatureTransformationDialog" 


                       
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CloseStatusDialog
//
//	Software purpose:	The purpose of this routine is to close the status
//							dialog box and update the menu bar.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	 		
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 03/15/1993
//	Revised By:			Larry L. Biehl			Date: 07/09/1998	
void CloseStatusDialog (
				Boolean								showMenuBarFlag)

{		
	if (gStatusDialogPtr != NULL)
		{           
		#if defined multispec_mac
			CloseRequestedDialog (gStatusDialogPtr, kDoNotSetUpDFilterTable);
			
			if (showMenuBarFlag)
				ShowMultiSpecMenuBar ();
		#endif	// defined multispec_mac 
		
		#if defined multispec_win  
			gStatusDialogPtr->ShowWindow (SW_HIDE);
			gStatusDialogPtr->DestroyWindow ();
			//delete gStatusDialogPtr;	  
		#endif	// defined multispec_win 

      #if defined multispec_wx
         gStatusDialogPtr->Show (false);
         gStatusDialogPtr->Destroy ();
      #endif	// defined multispec_wx
		
		}	// end "if (gStatusDialogPtr != NULL)"
		
	gStatusDialogPtr = NULL;
	gStatusProgressControlHandle = NULL;
	gStatusIDNumber = 0;
	gStatusTitleItemNumber = 0; 
	
}	// end "CloseStatusDialog" 


#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 CreateUnicodeStaticTextControl
//
//	Software purpose:	The purpose of this routine is to load the requested
//							alert from the resource file and center it on the
//							monitor that the mouse is in.  The alert is then displayed.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	1: Okay, Yes, Continue
//							2: Cancel, No
//							3: Depends on alert
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/25/1990
//	Revised By:			Larry L. Biehl			Date: 03/23/2017

SInt16 CreateUnicodeStaticTextControl (
				DialogPtr							dialogPtr,
				UCharPtr								stringPtr,
				UInt16								stringLength,
				UInt16								dialogItem,
				ControlRef*							controlRefPtr)
				
				
{
	Rect									theBox;
	
	CFStringRef							cfStringRef;
	ControlRef							controlRef;
	Handle								theHandle;
	OSErr									osErr = 1;
	
	SInt16								returnCode = 0,
											theType;
	
	
	GetDialogItem (dialogPtr, dialogItem, &theType, &theHandle, &theBox);
	
	cfStringRef = CFStringCreateWithBytes (kCFAllocatorDefault,
															stringPtr,
															stringLength,
															kCFStringEncodingUTF8,
															false);
															
	if (cfStringRef != NULL)
		osErr = CreateStaticTextControl (GetDialogWindow (dialogPtr), 
														&theBox, 
														cfStringRef, 
														NULL, 
														&controlRef);
		
	if (osErr == 0)
		HideDialogItem (dialogPtr, dialogItem);

	else
		//LoadDItemString (dialogPtr, dialogItem, (Str255*)&dialogString);
		returnCode = 1;
	
	if (controlRefPtr != NULL)
		{
		if (returnCode == noErr)
			*controlRefPtr = controlRef;
		else
			*controlRefPtr = NULL;
		
		}	// end "if (controlRefPtr != NULL)"
		
	return (returnCode);
		
}	// end "CreateUnicodeStaticTextControl"
#endif
 
/*
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DeactivateDialogItem
//
//	Software purpose:	The purpose of this routine is to deactivate the specified 
//							dialog item.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/19/2012
//	Revised By:			Larry L. Biehl			Date: 03/26/2012

void DeactivateDialogItem (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{   
	if (dialogPtr != NULL && itemNumber > 0)
		{   
		#if defined multispec_mac 
		
			Handle					theHandle;
			Rect						theBox; 
			SInt16					theType; 
		
			GetDialogItem (dialogPtr, itemNumber, &theType, &theHandle, &theBox);
			
			if (theHandle != NULL)
				{
				theType |= kItemDisableBit;
				SetDialogItem (dialogPtr, itemNumber, theType, theHandle, &theBox);
				InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
				
				}	// end "if (theHandle != NULL)"
			
		#endif	// defined multispec_mac

		#if defined multispec_win
			dialogPtr->GetDlgItem (itemNumber)->SendMessage (EM_SETREADONLY,1,0);
		#endif // defined multispec_win 
		
		}	// end "if (dialogPtr != NULL && itemNumber > 0)" 
	
}	// end "DeactivateDialogItem"
*/	



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 DisplayAlert
//
//	Software purpose:	The purpose of this routine is to load the requested
//							alert from the resource file and center it on the
//							monitor that the mouse is in.  The alert is then displayed.
//							This routine provides an overload for DisplayAlert.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	1: Okay, Yes, Continue
//							2: Cancel, No
//							3: Depends on alert
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/04/2017
//	Revised By:			Larry L. Biehl			Date: 03/12/2019

SInt16 DisplayAlert (
				SInt16								alertResourceId, 
				SInt16								alertType, 
				SInt16								stringID, 
				SInt16								stringNumber1,
				SInt16								stringNumber2,
				UCharPtr								stringPtr)

{
	return (DisplayAlert (alertResourceId,
									alertType,
									stringID,
									stringNumber1,
									stringNumber2,
									stringPtr,
									NULL,
									kASCIICharString));

}	// end "DisplayAlert"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 DisplayAlert
//
//	Software purpose:	The purpose of this routine is to load the requested
//							alert from the resource file and center it on the
//							monitor that the mouse is in.  The alert is then displayed.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	1: Okay, Yes, Continue
//							2: Cancel, No
//							3: Depends on alert
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/25/1990
//	Revised By:			Larry L. Biehl			Date: 03/12/2019

SInt16 DisplayAlert (
				SInt16								alertResourceId, 
				SInt16								alertType, 
				SInt16								stringID, 
				SInt16								stringNumber1,
				SInt16								stringNumber2,
				UCharPtr								stringPtr,
				DialogPtr							parentPtr,
				UInt16								stringCharCode)

{
	SInt16								itemHit = 0;
	
	#if defined multispec_mac
		BitMap								bitMap;
		Rect									displayRect;
		
		Str255								alertTextString,
												explanationString;
		
		GDHandle								currentDevice;
		Handle								alertHandle;
		Point									mouseLocation;
		
		Rect*									alertRect;
		
		UCharPtr								string1Ptr,
												string2Ptr;
		
		SInt16								standardAlertType;
		
		Boolean								callStandardAlertFlag;
	
	
				// Initialize local variables.														
				
		itemHit = -1;
		
				// Make sure that MultiSpec is in the foreground before continuing.		
				
		PauseIfInBackground ();
		
				// Release spare memory, if available, to make certain that 				
				// warning message can be displayed.												
		
		ReleaseSpareMemoryForWarningMessage ();
			
				// Get text information for the alert if needed.
				
		string1Ptr = NULL;
		string2Ptr = NULL;							
		
		if (stringPtr == NULL && stringID > 0)
			{
			GetIndString (alertTextString, stringID, stringNumber1);
			if (alertTextString[0] != 0)
				string1Ptr = &alertTextString[0];
			
			if (stringNumber2 > 0)
				{	
				GetIndString (explanationString, stringID, stringNumber2);
				if (explanationString[0] != 0)
					string2Ptr = &explanationString[0];
					
				}	// end "if (stringNumber2 > 0)"
			
			}	// end "if (stringPtr == NULL && stringID > 0)"
			
		else	// stringPtr != NULL ||& stringID == 0
			string1Ptr = stringPtr;
		
		callStandardAlertFlag = FALSE;	
		if (gAppearanceManagerFlag)
			{
			callStandardAlertFlag = TRUE;	
			if (alertResourceId == kMemoryAlertID ||
					alertResourceId == kMemoryWarningAlertID ||
					alertResourceId == kCoprocessorAlertID ||
					alertResourceId == kIndirectCoprocessorAlertID ||
					alertResourceId == kToLargeToCopyAlertID ||
					alertResourceId == kOperatingSystemAlertID ||
					alertResourceId == kCoreGraphicsAlertID)
				callStandardAlertFlag = FALSE;	
			
			}	// end "if (gAppearanceManagerFlag)"
			
		if (callStandardAlertFlag)
			{
			/*
			AlertStdAlertParamRec		alertStdAlertParamRec;
			
			alertStdAlertParamRec.movable = TRUE;
			alertStdAlertParamRec.helpButton = FALSE;
			alertStdAlertParamRec.filterProc = gProcessorDialogFilterPtr;
			alertStdAlertParamRec.defaultText = (UCharPtr)kAlertDefaultOKText;
			alertStdAlertParamRec.cancelText = (UCharPtr)kAlertDefaultCancelText;
			alertStdAlertParamRec.otherText = NULL;
			alertStdAlertParamRec.defaultButton = kAlertStdAlertOKButton;
			alertStdAlertParamRec.cancelButton = kAlertStdAlertCancelButton;
			alertStdAlertParamRec.position = kWindowAlertPositionParentWindowScreen;
			*/
			AlertStdCFStringAlertParamRec		stdCFStringAlertParamRec;

			stdCFStringAlertParamRec.version = kStdCFStringAlertVersionTwo;
			stdCFStringAlertParamRec.movable = TRUE;
			stdCFStringAlertParamRec.helpButton = FALSE;
			stdCFStringAlertParamRec.defaultText = NULL;
			stdCFStringAlertParamRec.cancelText = NULL;
			stdCFStringAlertParamRec.otherText = NULL;
			stdCFStringAlertParamRec.defaultButton = kAlertStdAlertOKButton;
			stdCFStringAlertParamRec.cancelButton = kAlertStdAlertCancelButton;
			stdCFStringAlertParamRec.position = kWindowAlertPositionParentWindowScreen;
			stdCFStringAlertParamRec.flags = 0;
			stdCFStringAlertParamRec.icon = NULL;

			standardAlertType = kAlertPlainAlert;
			if (alertType == kAlert)
				standardAlertType = kAlertPlainAlert;
			else if (alertType == kNoteAlert)
				standardAlertType = kAlertNoteAlert;
			else if (alertType == kCautionAlert)
				standardAlertType = kAlertCautionAlert;
			else if (alertType == kStopAlert)
				standardAlertType = kAlertStopAlert;
				
			switch (alertResourceId)
				{
				case kErrorAlertID:
				case kErrorAlert2ID:
				case kOKNoSoundAlertID:
				case kTextWindowLowMemoryAlertID:
					//alertStdAlertParamRec.cancelText = NULL;
					//alertStdAlertParamRec.cancelButton = 0;
					stdCFStringAlertParamRec.cancelText = NULL;
					stdCFStringAlertParamRec.cancelButton = 0;
					break;
					
				case kOpenImageWAlertID:
					//alertStdAlertParamRec.defaultText = "\pOpen";
					//alertStdAlertParamRec.otherText = "\pDon't Open";
					stdCFStringAlertParamRec.defaultText = CFSTR("Open");
					stdCFStringAlertParamRec.otherText = CFSTR("Don't Open");
					break;
					
				case kContinueCancelAlertID:
					//alertStdAlertParamRec.defaultText = "\pContinue";
					stdCFStringAlertParamRec.defaultText = CFSTR("Continue");
					break;
					
				case kSaveAlertID:
					//alertStdAlertParamRec.defaultText = "\pSave";
					//alertStdAlertParamRec.otherText = "\pDon't Save";
					stdCFStringAlertParamRec.cancelText = CFSTR("Cancel");
					stdCFStringAlertParamRec.defaultText = CFSTR("Save");
					stdCFStringAlertParamRec.otherText = CFSTR("Don't Save");
					break;
					
				case kOKCancelAlertID:
							// The default.
					break;
					
				case kYesCancelAlertID:
					//alertStdAlertParamRec.defaultText = "\pYes";
					stdCFStringAlertParamRec.defaultText = CFSTR("Yes");
					break;
					
				case kUpdateCancelAlertID:
					//alertStdAlertParamRec.defaultText = "\pUpdate";
					stdCFStringAlertParamRec.defaultText = CFSTR("Update");
					break;
					
				case kFinishDontCancelAlertID:
					//alertStdAlertParamRec.defaultText = "\pFinish";
					//alertStdAlertParamRec.otherText = "\pDon't Finish";
					stdCFStringAlertParamRec.defaultText = CFSTR("Finish");
					stdCFStringAlertParamRec.otherText = CFSTR("Don't Finish");
					break;
					
				case kRedoDontCancelAlertID:
					//alertStdAlertParamRec.defaultText = "\pRedo";
					//alertStdAlertParamRec.otherText = "\pDon't Redo";
					stdCFStringAlertParamRec.defaultText = CFSTR("Redo");
					stdCFStringAlertParamRec.otherText = CFSTR("Don't Redo");
					break;
					
				case kChangeDontAlertID:
					//alertStdAlertParamRec.defaultText = "\pChange";
					//alertStdAlertParamRec.cancelText = NULL;
					//alertStdAlertParamRec.otherText = "\pDon't Change";
					//alertStdAlertParamRec.cancelButton = 0;
					stdCFStringAlertParamRec.defaultText = CFSTR("Change");
					stdCFStringAlertParamRec.cancelText = NULL;
					stdCFStringAlertParamRec.otherText = CFSTR("Don't Change");
					stdCFStringAlertParamRec.cancelButton = 0;
					break;
					
				case kSaveDontSaveAlertID:
					//alertStdAlertParamRec.defaultText = "\pSave";
					//alertStdAlertParamRec.cancelText = NULL;
					//alertStdAlertParamRec.otherText = "\pDon't Save";
					//alertStdAlertParamRec.cancelButton = 0;
					stdCFStringAlertParamRec.defaultText = CFSTR("Save");
					stdCFStringAlertParamRec.cancelText = NULL;
					stdCFStringAlertParamRec.otherText = CFSTR("Don't Save");
					stdCFStringAlertParamRec.cancelButton = 0;
					break;
					
				case kYesNoAlertID:
					//alertStdAlertParamRec.defaultText = "\pYes";
					//alertStdAlertParamRec.cancelText = "\pNo";
					stdCFStringAlertParamRec.defaultText = CFSTR("Yes");
					stdCFStringAlertParamRec.cancelText = CFSTR("No");
					break;
					
				case kContinueStopAlertID:
					//alertStdAlertParamRec.defaultText = "\pContinue";
					//alertStdAlertParamRec.cancelText = "\pStop";
					stdCFStringAlertParamRec.defaultText = CFSTR("Continue");
					stdCFStringAlertParamRec.cancelText = CFSTR("Stop");
					break;
				
				}	// end "switch (alertResourceId)"
			/*
			StandardAlert (standardAlertType,
								string1Ptr,
								string2Ptr,
								&alertStdAlertParamRec,
								&itemHit);
			*/
			CFStringRef error = NULL;
			CFStringRef explanation = NULL;
			if (string1Ptr != NULL)
				error = CFStringCreateWithBytes (
													kCFAllocatorDefault,
													&string1Ptr[1],
													string1Ptr[0],
													kCFStringEncodingUTF8,
													false);
			if (string2Ptr != NULL)
				explanation = CFStringCreateWithBytes (
															kCFAllocatorDefault,
															&string2Ptr[1],
															string2Ptr[0],
															kCFStringEncodingUTF8,
															false);
				

			DialogRef		outAlert;
			OSStatus osErr = CreateStandardAlert (
										standardAlertType,
										error,
										explanation,
										&stdCFStringAlertParamRec,
										&outAlert);
			
			if (osErr == noErr)
				RunStandardAlert (outAlert,
										NULL,
										&itemHit);
			
			}	// end "if (callStandardAlertFlag)"
			
		else	// !callStandardAlertFlag
			{
					// Load the requested alert resource.												
					
			alertHandle = GetResource ('ALRT', alertResourceId);
			
			if (alertHandle != NULL)
				{
						// Get the display rectangle for the device that the mouse cursor		
						// is in.  A check will be made for multiple device of color 			
						// quickdraw is available.															
						
				GetQDGlobalsScreenBits (&bitMap);
				displayRect = bitMap.bounds;
				
				currentDevice = NULL;
				if (gColorQDflag)
					{
					GetMouse (&mouseLocation);
					LocalToGlobal (&mouseLocation);
					currentDevice = GetDeviceList ();
				
					while (currentDevice)
						{
						gTempRect = (*currentDevice)->gdRect;
						if (TestDeviceAttribute (currentDevice, screenDevice) &&
								TestDeviceAttribute (currentDevice, screenActive) &&
								PtInRect (mouseLocation, &gTempRect))
							displayRect = (*currentDevice)->gdRect;
						
						currentDevice = GetNextDevice (currentDevice);
					
						}	// end "while (currentDevice)" 
						
					}	// end "if (gColorQDflag)" 
					
				displayRect.top += GetMBarHeight ();
			
						// Calculate upper left corner that will leave box centered.		
				
				alertRect = (Rect*)*alertHandle;
				OffsetRect (alertRect, 
								(displayRect.right + displayRect.left)/2 - 
												(alertRect->right + alertRect->left)/2,
								(displayRect.top + displayRect.bottom)/2 -
												(alertRect->top + alertRect->bottom)/2);
								
						// Adjust so that 1/5 of space is above and 4/5 of space is 		
						// below.																			
												
				OffsetRect (alertRect, 0, -(alertRect->top - displayRect.top) * .6);
					
				if (stringPtr != NULL)
					ParamText (string1Ptr, "\p", "\p", "\p");
				
				if (alertType == kAlert)
					itemHit = Alert (alertResourceId, gDialogListFilterPtr);
				
				else if (alertType == kNoteAlert)
					itemHit = NoteAlert (alertResourceId, gDialogListFilterPtr);
				
				else if (alertType == kCautionAlert)
					itemHit = CautionAlert (alertResourceId, gDialogListFilterPtr);
				
				else if (alertType == kStopAlert)
					itemHit = StopAlert (alertResourceId, gDialogListFilterPtr);
				
				ParamText ("\p", "\p", "\p", "\p");
				
				}	// end "if (alertHandle != NULL)" 
					
			}	// end "else !callStandardAlertFlag"
			
				// Get the spare memory back again.													
				
		SetHandleSize (gSpareWarnMemoryH, (Size)gSpareWarnSize);
	#endif	// defined multispec_mac

	#if defined multispec_win 
		USES_CONVERSION;

		if (alertResourceId == 0)
			alertResourceId = MB_OK;
		
	   if (stringNumber1 > 0)                               
			itemHit = AfxMessageBox (stringNumber1, alertResourceId, 0);
			
		else if (stringPtr != NULL) 
			{
			if (stringCharCode == kASCIICharString)
				itemHit = AfxMessageBox ((LPCTSTR)A2T((LPCSTR)&stringPtr[1]), alertResourceId, 0);
			
			else
				{
				TBYTE* unicodeStringPtr;
				unicodeStringPtr = ConvertMultibyteStringToUnicodeString (&stringPtr[1]);
				itemHit = AfxMessageBox (&unicodeStringPtr[1], alertResourceId, 0);
				
				}
			
			}
			              
		switch (itemHit)
			{
			case IDOK: 
				itemHit = 1;
				break;
				
			case IDYES:
				itemHit = 1;
				break; 
				
			case IDNO:
				itemHit = 3;
				break;  
				
			case IDCANCEL:
				itemHit = 2;
				break;
					
			}	// end "switch (itemHit)"	
	#endif // defined multispec_win

	#if defined multispec_wx
		unsigned char	textptr[256];
		wxString			message;
		long				style;
		
		style = alertResourceId;

		if (alertResourceId == 0)
			style = wxOK;
			  
		style |= wxSTAY_ON_TOP;
			
		if (stringNumber1 > 0) 
			{
					// First get a pointer to the string
				
			if (MGetString (textptr, 0, stringNumber1))
				{
						// Now convert in wxstring
				
				unsigned char* textmsg = &textptr[1];
				message << textmsg;

				}	// end "if (MGetString (textptr, 0, stringNumber1))"
				
			else	// !MGetString (textptr, 0, stringNumber1)
				{
				message = wxString::Format (
										wxT("%d: %s"), stringNumber1, "Invalid String table ID");
				
				}	// end "else !MGetString (textptr, 0, stringNumber1)"

			}	// end "if (stringNumber1 > 0)"
			
		else if (stringPtr != NULL) 
			{
			message << &stringPtr[1];
			
			}	// end "else if (stringPtr != NULL) "
	
				// Note that I tried using the positioning parameter for wxMessageDialg but
				// find that it is not used.
	
		wxWindow* parentWindowPtr = parentPtr;
		if (parentPtr == NULL)
			parentWindowPtr = GetMainFrame ();
		wxMessageDialog dialog (parentWindowPtr, message, wxT("MultiSpec"), style);
	
		int answer = dialog.ShowModal ();
		switch (answer) 
			{
			case wxID_OK:
				itemHit = 1;
				break;

			case wxID_YES:
				itemHit = 1;
				break;

			case wxID_NO:
				itemHit = 3;
				break;

			case wxID_CANCEL:
				itemHit = 2;
				break;

			}	// end "switch (itemHit)"
	#endif	// defined multispec_wx

	return (itemHit);

} 	// end "DisplayAlert"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DupClassFieldNameAlert
//
//	Software purpose:	The purpose of this routine is to display an alert
//							to inform the user that the requested class or field
//							name is a duplicate.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			EditGroupClassDialog in SThematicWindow.cpp
//							EditClassFieldDialog in SEditStatistics.cpp
//							NewClassFieldDialog in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/20/1989
//	Revised By:			Larry L. Biehl			Date: 03/16/2017	

void DupClassFieldNameAlert (
				SInt16								classFieldFlag, 
				UCharPtr								duplicateNamePtr)

{
	SInt16								returnCode,
											stringID;
	
	
	if (classFieldFlag == 1)
		stringID = IDS_Alert43;
		
	if (classFieldFlag == 2)
		stringID = IDS_Alert44;
		
	if (classFieldFlag == 3)
		stringID = IDS_Alert45;
		
			// Make sure the c-string terminator is at the end of the string.
			
	duplicateNamePtr[duplicateNamePtr[0]+1] = 0; 
									
	if (LoadSpecifiedStringNumberStringP (
										kAlertStrID, 
										stringID, 
										(char*)gTextString2,
										(char*)gTextString3,
										TRUE,
										(char*)&duplicateNamePtr[1]))
		returnCode = DisplayAlert (kErrorAlertID, 3, 0, 0, 0, gTextString2);
	
}	// end "DupClassFieldNameAlert" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetDialogLocalVectors
//
//	Software purpose:	The purpose of this routine is to get the memory for the 
//							requested input pointers which will then be used as local
//							vectors in various dialog boxes.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			EditGroupClassDialog in SThematicWindow.cpp
//							EditClassFieldDialog in SEditStatistics.cpp
//							NewClassFieldDialog in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/01/1998
//	Revised By:			Larry L. Biehl			Date: 10/13/2015	

Boolean GetDialogLocalVectors (
				UInt16**								channelsPtrPtr,
				UInt16**								transformFeaturesPtrPtr,
				UInt16**								classPtrPtr,
				UInt16**								classAreaPtrPtr,
				float**								classWeightsPtrPtr,
				unsigned char**					symbolsPtrPtr,
				UInt16**								channelCombinationsPtrPtr,
				SInt16**								classPairWeightsListPtrPtr)		
	
{  
	SInt32								bytesNeeded,
											numberClasses,
											totalNumberChannels;
	
	Boolean								continueFlag = TRUE;
	
		
	if (channelsPtrPtr != NULL || transformFeaturesPtrPtr != NULL)
		{                       
				// Get the total number of channels possible.
		
		totalNumberChannels = -1;		                         
		if (gImageWindowInfoPtr != NULL)
			totalNumberChannels = (SInt32)gImageWindowInfoPtr->totalNumberChannels;
			
		else if (gProjectInfoPtr != NULL)
			{
					// Must be based just on the project structure 
					
			WindowInfoPtr projectWindowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																gProjectInfoPtr->windowInfoHandle);
			
			if (projectWindowInfoPtr != NULL)						
				totalNumberChannels = (SInt32)projectWindowInfoPtr->totalNumberChannels; 
			
			}	// end "else if (gProjectInfoPtr != NULL)" 
			
		if (totalNumberChannels == -1)
			continueFlag = FALSE;
		
		}	// end "if (channelsPtrPtr != NULL || ..."
					
	if (continueFlag && channelsPtrPtr != NULL)
		{                                                   
		
				// Get pointer to memory for temporary storage of channel list. 
			                                                                       
		*channelsPtrPtr = (UInt16*)MNewPointer (
													(UInt32)totalNumberChannels * sizeof (UInt16));
				                           
	                                                       
		continueFlag = (*channelsPtrPtr != NULL);
		
		}	// end "if (continueFlag && channelsPtrPtr != NULL)"
						
	if (continueFlag && transformFeaturesPtrPtr != NULL)
		{                          
				// Get pointer to memory for temporary storage of channel list. 
			                                                                       
		*transformFeaturesPtrPtr = (UInt16*)MNewPointer (
													(UInt32)totalNumberChannels * sizeof (UInt16)); 
	                                                       
		continueFlag = (*transformFeaturesPtrPtr != NULL);
		
		}	// end "if (continueFlag && transformFeaturesPtrPtr != NULL)"
	
			// If there are no statistics classes at this time, assume that there is
			// one class so that the number of bytes for the handle is not 0.
			
	if (continueFlag && classPtrPtr != NULL)
		{
				// Get memory for the local class list vector.	
		
		if (gProjectInfoPtr->numberStatisticsClasses	> 0)
			{
			*classPtrPtr = (UInt16*)MNewPointer (
						(UInt32)gProjectInfoPtr->numberStatisticsClasses * sizeof (UInt16));
			continueFlag = (*classPtrPtr != NULL);
			
			}	// end "if (gProjectInfoPtr->numberStatisticsClasses	> 0)"
			
		else	// gProjectInfoPtr->numberStatisticsClasses == 0
			*classPtrPtr = NULL;
		
		}	// end "if (continueFlag && ..." 
	
	if (continueFlag && classAreaPtrPtr != NULL)
		{
				// Get memory for the local class area list vector.
		
		if (gProjectInfoPtr->numberStatisticsClasses	> 0)
			{
			*classAreaPtrPtr = (UInt16*)MNewPointer (
						(UInt32)gProjectInfoPtr->numberStatisticsClasses * sizeof (UInt16));
			continueFlag = (*classAreaPtrPtr != NULL);
			
			}	// end "if (gProjectInfoPtr->numberStatisticsClasses	> 0)"
			
		else	// gProjectInfoPtr->numberStatisticsClasses == 0
			*classAreaPtrPtr = NULL;
		
		}	// end "if (continueFlag && ..." 
	
	if (continueFlag && classWeightsPtrPtr != NULL)
		{
				// Get memory for the local class weights vector.	
				
		*classWeightsPtrPtr = GetTempClassWeightsPtr (); 
	                                                       
		continueFlag = (*classWeightsPtrPtr != NULL);
		
		}	// end "if (continueFlag && ..." 
	
	if (continueFlag && symbolsPtrPtr != NULL)
		{
				// Get memory for the local symbols vector.
				
		numberClasses = gProjectInfoPtr->numberStatisticsClasses;
		if (gProcessorCode == kClusterProcessor)
			numberClasses = kMaxNumberStatClasses;
		*symbolsPtrPtr = (unsigned char*)MNewPointer (
																(numberClasses+1) * sizeof (char));
	                                                       
		continueFlag = (*symbolsPtrPtr != NULL);
		
		}	// end "if (continueFlag && ..." 
						
	if (continueFlag && channelCombinationsPtrPtr != NULL)
		{                          
				// Get pointer to memory for temporary storage of channel list. 
			                                                                       
		*channelCombinationsPtrPtr = (UInt16*)MNewPointer (
				(UInt32)gProjectInfoPtr->numberStatisticsChannels * sizeof (UInt16)); 
	                                                       
		continueFlag = (*channelCombinationsPtrPtr != NULL);
		
		}	// end "if (continueFlag && channelCombinationsPtrPtr != NULL)"
	
	if (continueFlag && classPairWeightsListPtrPtr != NULL)
		{
				// Get memory for the local class pair weights list vector.
				
		bytesNeeded = ::MGetHandleSize (gProjectInfoPtr->classPairWeightsListHandle);

		if (bytesNeeded > 0)
			{		
			*classPairWeightsListPtrPtr = (SInt16*)MNewPointer (bytesNeeded);

			continueFlag = (*classPairWeightsListPtrPtr != NULL);
			
			}	// end "if (bytesNeeded > 0)"
		
		}	// end "if (continueFlag && ..." 
		
	return (continueFlag);

}	// end "GetDialogLocalVectors"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetDItemValue
//
//	Software purpose:	The purpose of this routine is to get the string for the input
//							dialog window and item and convert the string to a number.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/07/1988
//	Revised By:			Larry L. Biehl			Date: 03/26/2012

SInt32 GetDItemValue (
				DialogPtr							dialogPtr,
				SInt16								itemNumber)

{			
	#if defined multispec_mac
		ControlHandle						controlHandle;
		Handle								theHandle;
		Rect									theBox;
		SInt32								theNum;
		
		OSErr									errorCode;
		SInt16								theType;
		

		GetDialogItem (dialogPtr, itemNumber, &theType, &theHandle, &theBox);
		
		theNum = 0;
		if (theType == kEditTextDialogItem || theType == kStaticTextDialogItem)
			{
			GetDialogItemText (theHandle, gTextString);	
			StringToNum (gTextString, &theNum);
			
			}	// end "if (theType == kEditTextDialogItem || ..."
			
		else if (theType & kControlDialogItem)
			{
			errorCode = ::GetDialogItemAsControl (dialogPtr, itemNumber, &controlHandle);
		
			if (errorCode == noErr)
				theNum = GetControlValue (controlHandle);
			
			}	// end "else if (theType & kControlDialogItem)"
		
		return (theNum);
	#endif	// defined multispec_mac

	#if defined multispec_mac_swift
		return (0);
	#endif	// defined multispec_mac_swift

	#if defined multispec_win
		return (dialogPtr->GetDialogItemValue (itemNumber));
	#endif	// defined multispec_win 

	#if defined multispec_wx
      wxTextCtrl* textctrl = (wxTextCtrl*)dialogPtr->FindWindow (itemNumber);
      wxString strval;
      strval = textctrl->GetValue ();
      return (wxAtoi (strval));
	#endif   // defined multispec_wx
																	
}	// end "GetDItemValue"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetMultiSpecDialogItemText
//
//	Software purpose:	The purpose of this routine is to determine the number of 
//							selected pixels identified by the input DialogSelectionArea
//							structure.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/25/2012
//	Revised By:			Larry L. Biehl			Date: 03/25/2012

void GetMultiSpecDialogItemText (
				DialogPtr							dialogPtr,
				SInt16								itemNumber,
				unsigned char*						textStringPtr)

{						
	textStringPtr[0] = 0;
	
	#if defined multispec_mac
		Rect									theBox;
		
		Handle								theHandle;
		
		SInt16								theType;
		
		
		GetDialogItem (dialogPtr, itemNumber, &theType, &theHandle, &theBox);
		if (theHandle != NULL && 
					(theType == kEditTextDialogItem || theType == kStaticTextDialogItem))
			{
			GetDialogItemText (theHandle, textStringPtr);
			
					// Add c string terminator.
					
			textStringPtr[textStringPtr[0]+1] = 0;
			
			}	// end "if (theHandle != NULL)"
	#endif	// defined multispec_mac  
			               																							
}	// end "GetMultiSpecDialogItemText"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt64 GetNumberOfSelectedPixels
//
//	Software purpose:	The purpose of this routine is to determine the number of 
//							selected pixels identified by the input DialogSelectionArea
//							structure.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 08/04/2010
//	Revised By:			Larry L. Biehl			Date: 08/13/2010

SInt64 GetNumberOfSelectedPixels (
				DialogSelectAreaPtr				dialogSelectAreaPtr)

{										
	SInt64								numberColumns,
											numberLines;
	
	
	numberLines = (SInt64)(dialogSelectAreaPtr->lineEnd - 
										dialogSelectAreaPtr->lineStart + 
													dialogSelectAreaPtr->lineInterval)/
																dialogSelectAreaPtr->lineInterval;
	
	numberColumns = (SInt64)(dialogSelectAreaPtr->columnEnd - 
										dialogSelectAreaPtr->columnStart + 
													dialogSelectAreaPtr->columnInterval)/
																dialogSelectAreaPtr->columnInterval;
	
	return (numberColumns * numberLines);
	
}	// end "GetNumberOfSelectedPixels" 


                       
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				DialogPtr GetStatusDialog
//
//	Software purpose:	The purpose of this routine is to get the status
//							dialog box and initialize the hidden and shown
//							items.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	DialogPtr to status dialog box 
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/27/1992
//	Revised By:			Larry L. Biehl			Date: 01/30/2019

DialogPtr GetStatusDialog (
				SInt16								statusInfoID, 
				Boolean								showWindowFlag)

{
	DialogPtr 							statusDialogPtr = NULL;
	
	
	if (gStatusDialogPtr != NULL && gFromToolParameterFileFlag)
		{
				// This implies that the histogram function is being called within a tool
				// parameter process and the status dialog box is being displayed for
				// that. It is not needed now. Close the status dialog box and get a
				// new one.
		
		CloseStatusDialog (FALSE);
		
		}
	                      
		// If the status dialog already exists return.								
		
	if (gStatusDialogPtr != NULL)
																				return (gStatusDialogPtr); 
														
	gStatusTitleItemNumber = 0;
	gStatusIDNumber = 0;
															
	#if defined multispec_mac
		Handle								theHandle;
		SInt16								theType;
					
			// Get pointer to status dialog box.											
		
		statusDialogPtr = LoadRequestedDialog (statusInfoID, kDoNotCopyScrap, 1, 2);
		
		if (statusDialogPtr != NULL) 
			{
			if (statusInfoID == kUpdateStatsInfoID)
				{
				gStatusIDNumber = kUpdateStatsInfoID;
				gStatusTitleItemNumber = 11;
				
				HideDialogItems (statusDialogPtr, 1, 22);
					
				ShowDialogItem (statusDialogPtr, 11);
					
				}	// end "if (statusInfoID == kUpdateStatsInfoID)" 
				
			if (statusInfoID == kGraphicStatusDialogID)
				{
				gStatusIDNumber = kGraphicStatusDialogID;
				gStatusTitleItemNumber = 2;
	
				if (gAppearanceManagerFlag)
					{
					GetDialogItem (statusDialogPtr, 7, &theType, &theHandle, &gTempRect);
	
							// Make the progress indicator
									
					gStatusProgressControlHandle = NewControl (
																	GetDialogWindow (statusDialogPtr), 
																	&gTempRect, 
																	"\p", 
																	TRUE,
																	0, 
																	0, 
																	100, 
																	kControlProgressBarProc,
																	0);
																	
					gStatusGraphicsBox.left = 0;		
					gStatusGraphicsBox.right = 100;
																																							
					}	// end "if (gAppearanceManagerFlag)"
				
				if (gStatusProgressControlHandle == NULL)
					{
							// Set routine to draw the graphics box.							
							
					SetDialogItemDrawRoutine (
							statusDialogPtr, 7, &gStatusGraphicsBox, gDrawGraphicsBoxPtr);
					
					}	// end "if (gStatusProgressControlHandle == NULL)"
						
				gStatusGraphicsRight = gStatusGraphicsBox.left;
				
				HideDialogItems (statusDialogPtr, 3, 7);
					
				}	// end "if (statusInfoID == kGraphicStatusDialogID)" 
				
			if (statusInfoID == kShortStatusInfoID)
				{
				gStatusIDNumber = kShortStatusInfoID;
				gStatusTitleItemNumber = IDC_ShortStatusText;
				
				}	// end "if (statusInfoID == kShortStatusInfoID)"
				
					//	Clear title string.													
	
			LoadDItemString (statusDialogPtr, gStatusTitleItemNumber, (Str255*)"\p");
		
			if (showWindowFlag)
				{
				ShowDialogWindow (
							statusDialogPtr, statusInfoID, kDoNotSetUpDFilterTable);
	     	
				CheckSomeEvents (updateMask+activMask);
				
				}	// end "if (showWindowFlag)" 
			
			HideMultiSpecMenuBar ();
			
			}	// end "if (statusDialogPtr)"
	#endif	// defined multispec_mac  
			               						
	#if defined multispec_win                                           
		TRY
			{  
			CShortStatusDlg* statusDialogPtr2 = new CShortStatusDlg (statusInfoID);
	
			if (statusDialogPtr2 != NULL)
				{
				if (statusDialogPtr2->m_initializedFlag)
					statusDialogPtr = (DialogPtr)statusDialogPtr2;
	
				}	// end "if (statusDialogPtr2 != NULL)" 
				
			}
			
		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			}
		END_CATCH_ALL   
		
		if (statusDialogPtr != NULL) 
			{
			gStatusTitleItemNumber = 0;
			
			if (statusInfoID == kUpdateStatsInfoID)
				{
				gStatusIDNumber = kUpdateStatsInfoID;
				gStatusTitleItemNumber = IDC_Status11;
				                                           
				MHideDialogItem (statusDialogPtr, IDCANCEL);
				MHideDialogItem (statusDialogPtr, IDC_Status2);
				MHideDialogItem (statusDialogPtr, IDC_Status3);
				MHideDialogItem (statusDialogPtr, IDC_Status4);
				MHideDialogItem (statusDialogPtr, IDC_Status5);
				MHideDialogItem (statusDialogPtr, IDC_Status6);
				MHideDialogItem (statusDialogPtr, IDC_Status7);
				MHideDialogItem (statusDialogPtr, IDC_Status8);
				MHideDialogItem (statusDialogPtr, IDC_Status9);
				MHideDialogItem (statusDialogPtr, IDC_Status10);
				MHideDialogItem (statusDialogPtr, IDC_Status12);
				MHideDialogItem (statusDialogPtr, IDC_Status13);
				MHideDialogItem (statusDialogPtr, IDC_Status14);
				MHideDialogItem (statusDialogPtr, IDC_Status15);
				MHideDialogItem (statusDialogPtr, IDC_Status16);
				MHideDialogItem (statusDialogPtr, IDC_Status17);
				MHideDialogItem (statusDialogPtr, IDC_Status18);
				MHideDialogItem (statusDialogPtr, IDC_Status19);
				MHideDialogItem (statusDialogPtr, IDC_Status20);
				MHideDialogItem (statusDialogPtr, IDC_Status21);
				MHideDialogItem (statusDialogPtr, IDC_Status22);
				
				MHideDialogItem (statusDialogPtr, IDC_EscapeMessage); 
					
				}	// end "if (statusInfoID == kUpdateStatsInfoID)" 
				
			if (statusInfoID == kGraphicStatusDialogID)
				{
				gStatusIDNumber = kGraphicStatusDialogID;
				gStatusTitleItemNumber = IDC_Status2;
					
						// Set routine to draw the graphics box.							
							
				//SetDialogItemDrawRoutine (
				//			statusDialogPtr, 7, &gStatusGraphicsBox, gDrawGraphicsBoxPtr);
						
				gStatusGraphicsRight = gStatusGraphicsBox.left;
				
				MHideDialogItem (statusDialogPtr, IDC_Status3);
				MHideDialogItem (statusDialogPtr, IDC_Status4);
				MHideDialogItem (statusDialogPtr, IDC_Status5);
				MHideDialogItem (statusDialogPtr, IDC_Status6);
				MHideDialogItem (statusDialogPtr, IDC_Status7);
						
				}	// end "if (statusInfoID == kGraphicStatusDialogID)"
				
					// Hide the cancel button until it has been implemented. 
				
			MHideDialogItem (statusDialogPtr, IDCANCEL); 
					
			if (statusInfoID == kShortStatusInfoID)
				{
				gStatusIDNumber = kShortStatusInfoID;
				gStatusTitleItemNumber = IDC_ShortStatusText; 
				
				}	// end "if (statusInfoID == kShortStatusInfoID)"  
				
					//	Clear title string.													
	
			LoadDItemString (statusDialogPtr, gStatusTitleItemNumber, (Str255*)"\0");
				
			}	// end "if (statusDialogPtr != NULL)"	
	#endif	// defined multispec_win
		
	#if defined multispec_wx
		CShortStatusDlg* statusDialogPtr2 = new CShortStatusDlg (
																		statusInfoID,
																		NULL);
 
		if (statusDialogPtr2 != NULL)
			{
			if (statusDialogPtr2->m_initializedFlag)
				statusDialogPtr = (DialogPtr)statusDialogPtr2;

			}	// end "if (statusDialogPtr2 != NULL)"

		if (statusDialogPtr != NULL)
			{
			gStatusTitleItemNumber = 0;

			if (statusInfoID == kUpdateStatsInfoID)
				{
				gStatusIDNumber = kUpdateStatsInfoID;
				gStatusTitleItemNumber = IDC_Status11;

				MHideDialogItem (statusDialogPtr, IDC_CancelOperation);
				MHideDialogItem (statusDialogPtr, IDC_Status2);
				MHideDialogItem (statusDialogPtr, IDC_Status3);
				MHideDialogItem (statusDialogPtr, IDC_Status4);
				MHideDialogItem (statusDialogPtr, IDC_Status5);
				MHideDialogItem (statusDialogPtr, IDC_Status6);
				MHideDialogItem (statusDialogPtr, IDC_Status7);
				MHideDialogItem (statusDialogPtr, IDC_Status8);
				MHideDialogItem (statusDialogPtr, IDC_Status9);
				MHideDialogItem (statusDialogPtr, IDC_Status10);
				MHideDialogItem (statusDialogPtr, IDC_Status12);
				MHideDialogItem (statusDialogPtr, IDC_Status13);
				MHideDialogItem (statusDialogPtr, IDC_Status14);
				MHideDialogItem (statusDialogPtr, IDC_Status15);
				MHideDialogItem (statusDialogPtr, IDC_Status16);
				MHideDialogItem (statusDialogPtr, IDC_Status17);
				MHideDialogItem (statusDialogPtr, IDC_Status18);
				MHideDialogItem (statusDialogPtr, IDC_Status19);
				MHideDialogItem (statusDialogPtr, IDC_Status20);
				MHideDialogItem (statusDialogPtr, IDC_Status21);
				MHideDialogItem (statusDialogPtr, IDC_Status22);

					
				}	// end "if (statusInfoID == kUpdateStatsInfoID)"

			if (statusInfoID == kGraphicStatusDialogID)
				{
				gStatusIDNumber = kGraphicStatusDialogID;
				gStatusTitleItemNumber = IDC_Status2;

						// Set routine to draw the graphics box.

				//SetDialogItemDrawRoutine (
				//				statusDialogPtr, 7, &gStatusGraphicsBox, gDrawGraphicsBoxPtr);

				gStatusGraphicsRight = gStatusGraphicsBox.left;

				MHideDialogItem (statusDialogPtr, IDC_Status3);
				MHideDialogItem (statusDialogPtr, IDC_Status4);
				MHideDialogItem (statusDialogPtr, IDC_Status5);
				MHideDialogItem (statusDialogPtr, IDC_Status6);
				MHideDialogItem (statusDialogPtr, IDC_Status7);

				}	// end "if (statusInfoID == kGraphicStatusDialogID)"

					// Hide the cancel button until it has been implemented.

			//MHideDialogItem (statusDialogPtr, wxID_CANCEL);

			if (statusInfoID == kShortStatusInfoID)
				{
				gStatusIDNumber = kShortStatusInfoID;
				gStatusTitleItemNumber = IDC_ShortStatusText;

				}	// end "if (statusInfoID == kShortStatusInfoID)"

					//	Clear title string.

			LoadDItemString (statusDialogPtr, gStatusTitleItemNumber, (Str255*) "\0");

					// Show the dialog
				
			if (showWindowFlag)
				{
				statusDialogPtr->Show (true);
				statusDialogPtr->Update ();
				statusDialogPtr->SetFocus ();
				//statusDialogPtr->Fit ();
				//(wxTheApp->GetTopWindow ())->SetFocus ();
				CheckSomeEvents (osMask+updateMask);
				
				}	// end "if (showWindowFlag)"
				
			//statusDialogPtr->SetCursor (wxCursor (wxCURSOR_WAIT));
			
		 }	// end "if (statusDialogPtr != NULL)"
	#endif

	gOperationCanceledFlag = FALSE;
		
	return (statusDialogPtr);
	
}	// end "GetStatusDialog"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void HideStatusDialogItemSet
//
//	Software purpose:	The purpose of this routine is to hide
//							the specified status dialog box item set.
//
//	Parameters in:		Status Dialog item set
//
//	Parameters out:	None
//
// Value Returned:	None 			
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 03/27/1992
//	Revised By:			Larry L. Biehl			Date: 01/30/2019

void HideStatusDialogItemSet (
				SInt16								setNumber)
				
{
	if (gStatusDialogPtr == NULL)
																									return;
	
	#if defined multispec_mac 
		SInt16								endItem,
												startItem;
	
		switch (setNumber)
			{
			case kStatusTitle1:	// Title1 
				startItem = 11;
				endItem = 11;
				break;
				
			case kStatusTitle2:	// Title2 
				startItem = 21;
				endItem = 22;
				break;
				
			case kStatusClassA:	// ClassA 
			case kStatusClassB:	// ClassB 
				startItem = 2;
				endItem = 5;
				
				if (setNumber == kStatusClassB)
					break;
				HideDialogItem (gStatusDialogPtr, 6);
				HideDialogItem (gStatusDialogPtr, 12);
				break;
				
			case kStatusField:	// Field 
				startItem = 7;
				endItem = 10;
				break;
				
			case kStatusLine:	// Line 
				startItem = 17;
				endItem = 20;
				break;
				
			case kStatusCluster:	// Number Clusters 
				startItem = 15;
				endItem = 16;
				break;
				
			case kStatusMinutes:	// Minutes Until Completion 
				startItem = 13;
				endItem = 14;
				break;
				
			case kStatusCommand:	// Command-. 
				startItem = 1;
				endItem = 1;
				break;
				
			}	// end "switch (setNumber)" 
			
		HideDialogItems (gStatusDialogPtr, startItem, endItem);
	#endif	// defined multispec_mac
	
	#if defined multispec_win 
		switch (setNumber)
			{
			case kStatusTitle1:	// Title1 
	   		gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status11);
				break;
				
			case kStatusTitle2:	// Title2 
	   		gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status21);
	   		gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status22);
				break;
				
			case kStatusClassA:	// ClassA 
			case kStatusClassB:	// ClassB 
	   		gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status2);
	   		gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status3);
	   		gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status4);
	   		gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status5);
				if (setNumber == kStatusClassB)
					break;
					
	   		gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status6);
	   		gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status12);
				break;
				
			case kStatusField:	// Field 
	   		gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status7);
	   		gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status8);
	   		gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status9);
	   		gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status10);
				break;
				
			case kStatusLine:	// Line 
	   		gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status17);
	   		gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status18);
	   		gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status19);
	   		gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status20);
				break;
				
			case kStatusCluster:	// Number Clusters 
	   		gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status15);
	   		gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status16);
				break;
				
			case kStatusMinutes:	// Minutes Until Completion 
	   		gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status13);
	   		gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status14);
				break;
				
			case kStatusCommand:	// Command-. 
	   		gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDCANCEL);
				break;
				
			}	// end "switch (setNumber)" 
	#endif	// defined multispec_win 
	
	#if defined multispec_wx
		switch (setNumber)
			{
			case kStatusTitle1: // Title1
				gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status11);
            break;

			case kStatusTitle2: // Title2
            gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status21);
            gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status22);
            break;

			case kStatusClassA: // ClassA
			case kStatusClassB: // ClassB
            gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status2);
            gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status3);
            gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status4);
            gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status5);
            if (setNumber == kStatusClassB)
                break;

            gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status6);
            gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status12);
            break;

			case kStatusField: // Field
            gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status7);
            gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status8);
            gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status9);
            gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status10);
            break;

			case kStatusLine: // Line
            gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status17);
            gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status18);
            gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status19);
            gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status20);
            break;

			case kStatusCluster: // Number Clusters
            gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status15);
            gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status16);
            break;

			case kStatusMinutes: // Minutes Until Completion
            gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status13);
            gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_Status14);
            break;

			case kStatusCommand: // Command-.
            gStatusDialogPtr->MHideDialogItem (gStatusDialogPtr, IDC_CancelOperation);
            break;

			}	// end "switch (setNumber)"
	#endif	// defined multispec_wx
	
}	// end "HideStatusDialogItemSet"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void InitializeDialogFeatureParameters
//
//	Software purpose:	The purpose of this routine is to initialize the feature/
//							transformed feature parameters.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			StatisticsDialog   in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/04/1998
//	Revised By:			Larry L. Biehl			Date: 01/28/2003

void InitializeDialogFeatureParameters (
				Boolean								featureTransformationFlag,
				UInt16								localActiveNumberFeatures, 
				UInt16								numberFeatures, 
				UInt16								numberTransformFeatures,
				UInt16*								localFeaturesPtr,
				UInt16*								localTransformFeaturesPtr,
				UInt16*								localNumberFeaturesPtr,
				UInt16*								localNumberTransformFeaturesPtr,
				UInt16**								localActiveFeaturesPtrPtr)

{          
			// Set feature/transform feature parameters
						
	if (featureTransformationFlag)
		{
		*localNumberFeaturesPtr = numberFeatures;
		
		*localNumberTransformFeaturesPtr = localActiveNumberFeatures;
		
		*localActiveFeaturesPtrPtr = localTransformFeaturesPtr;
			
		}	// end "if (featureTransformationFlag)"
								
	else	// !featureTransformationFlag
		{
		*localNumberFeaturesPtr = localActiveNumberFeatures;
		
		if (localNumberTransformFeaturesPtr != NULL)
			*localNumberTransformFeaturesPtr = numberTransformFeatures;
		
		*localActiveFeaturesPtrPtr = localFeaturesPtr;
			
		}	// end "else !featureTransformationFlag" 

}	// end "InitializeDialogFeatureParameters" 		



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void InitializeDialogSelectArea
//
//	Software purpose:	The purpose of this routine is to initialize the input dialog
//							select area struction with the input parameters.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			ClassifyDialogInitialize  in SClassifyDialogs.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/19/1999
//	Revised By:			Larry L. Biehl			Date: 03/19/1999

void InitializeDialogSelectArea (
				DialogSelectAreaPtr				dialogSelectAreaPtr,
				WindowInfoPtr						windowInfoPtr,
				WindowPtr							windowPtr,
				UInt32								columnStart,
				UInt32								columnEnd,
				UInt32								columnInterval,
				UInt32								lineStart,
				UInt32								lineEnd,
				UInt32								lineInterval,
				SInt16								baseItem,
				SInt16								entireIconItem,
				Boolean								adjustToBaseImageFlag)
	
{										
	dialogSelectAreaPtr->imageWindowInfoPtr = windowInfoPtr;
	dialogSelectAreaPtr->windowPtr = windowPtr;
	dialogSelectAreaPtr->columnStart = columnStart;
	dialogSelectAreaPtr->columnEnd = columnEnd;
	dialogSelectAreaPtr->columnInterval = columnInterval;
	dialogSelectAreaPtr->lineStart = lineStart;
	dialogSelectAreaPtr->lineEnd = lineEnd;
	dialogSelectAreaPtr->lineInterval = lineInterval;
	dialogSelectAreaPtr->baseItem = baseItem;	
	dialogSelectAreaPtr->entireIconItem = entireIconItem;
	dialogSelectAreaPtr->adjustToBaseImageFlag = adjustToBaseImageFlag;
	dialogSelectAreaPtr->drawRoutineLoadedFlag = FALSE;

}	// end "InitializeDialogSelectArea" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void InvalDialogItemRect
//
//	Software purpose:	The purpose of this routine is to invalidate the area in the
//							input dialog represented by the input dialog item. In other
//							words this will force an update for this portion of the dialog
//							window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/20/2012
//	Revised By:			Larry L. Biehl			Date: 03/20/2012

void InvalDialogItemRect (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{   
	if (dialogPtr != NULL && itemNumber > 0)
		{   
		#if defined multispec_mac 
			Handle					theHandle;
			Rect						theBox; 
			SInt16					theType; 
		
			GetDialogItem (dialogPtr, itemNumber, &theType, &theHandle, &theBox);
			
			if (theHandle != NULL)
				InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
		#endif	// defined multispec_mac

		#if defined multispec_win
		
		#endif // defined multispec_win 
		
		}	// end "if (dialogPtr != NULL && itemNumber > 0)" 
	
}	// end "InvalDialogItemRect"		



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadDialogLocalVectors
//
//	Software purpose:	The purpose of this routine is to load the input dialog local
//							vectors with the saved input vector information.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			ClassifyDialog   in SClassifyDialogs.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/13/1998
//	Revised By:			Larry L. Biehl			Date: 04/01/2000

void LoadDialogLocalVectors (
				UInt16*								localChannelsPtr,
				UInt16*								localTransformFeaturesPtr,
				Handle								featureHandle,
				UInt16								numberChannels,
				UInt16*								localClassPtr,
				Handle								classHandle,
				UInt32								numberClasses,
				UInt16*								localClassAreaPtr,
				Handle								classAreaHandle,
				UInt32								numberClassAreas,
				unsigned char*						localSymbolPtr,
				Handle								symbolsHandle,
				UInt32								numberSymbols,
				UInt16*								localChannelCombinationsPtr,
				Handle								channelCombinationsHandle,
				UInt32								numberChannelGroupCombinations,
				SInt16**								localClassPairWeightsListPtrPtr)
	
{
	UInt16* 								channelsPtr;
	
	UInt32								index;
	
	
	channelsPtr = (UInt16*)GetHandlePointer (featureHandle);
						
	if (localChannelsPtr != NULL && featureHandle != NULL)
		{  						
		for (index=0; index<numberChannels; index++)
			localChannelsPtr[index] = channelsPtr[index];
		
		}	// end "if (localChannelsPtr != NULL && featureHandle != NULL)"
	
						
	if (localTransformFeaturesPtr != NULL && featureHandle != NULL)
		{             
		for (index=0; index<numberChannels; index++)
			localTransformFeaturesPtr[index] = channelsPtr[index];
		
		}	// end "if (localTransformFeaturesPtr != NULL && ..."
	
	if (localClassPtr != NULL && classHandle != NULL)
		{
		UInt16* classPtr = (UInt16*)GetHandlePointer (classHandle);
									
		for (index=0; index<numberClasses; index++)
			localClassPtr[index] = classPtr[index];
		
		}	// end "if (localClassPtr != NULL && classHandle != NULL)"  
	
	if (localClassAreaPtr != NULL && classAreaHandle != NULL)
		{
		UInt16* classAreaPtr = (UInt16*)GetHandlePointer (classAreaHandle);
									
		for (index=0; index<numberClassAreas; index++)
			localClassAreaPtr[index] = classAreaPtr[index];
		
		}	// end "if (localClassAreaPtr != NULL && classAreaHandle != NULL)" 
	
	if (localSymbolPtr != NULL && symbolsHandle != NULL)
		{
		char* symbolsPtr = (char*)GetHandlePointer (symbolsHandle);
				
		BlockMoveData (symbolsPtr, localSymbolPtr, numberSymbols+1);
		
		}	// end "if (localSymbolPtr != NULL && symbolsHandle != NULL)"
	
	if (localChannelCombinationsPtr != NULL && channelCombinationsHandle != NULL)
		{
		SInt16* channelCombinationsPtr = (SInt16*)GetHandlePointer (
																			channelCombinationsHandle);
													
		for (index=0; index<numberChannelGroupCombinations; index++)
			localChannelCombinationsPtr[index] = channelCombinationsPtr[index];	
		
		}	// end "if (localChannelCombinationsPtr != NULL && ..."
		
	if (localClassPairWeightsListPtrPtr != NULL)
		{					
				// Load copy of weights list information into the pointer. The
				// memory for the pointer has already been allocated.
				
		CopyHandleToPointer (gProjectInfoPtr->classPairWeightsListHandle,
										(Ptr*)localClassPairWeightsListPtrPtr);
		
		}	// end "if (localClassPairWeightsListPtrPtr != NULL)"
																	
}	// end "LoadDialogLocalVectors"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadDItemRealValue
//
//	Software purpose:	The purpose of this routine is to convert the
//							input real value to a string with the requested 
//							number of decimal places and load it into the
//							requested dialog item number.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			ClassifyDialog   in SClassifyDialogs.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/19/1989
//	Revised By:			Larry L. Biehl			Date: 04/21/2006

void LoadDItemRealValue (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber, 
				double								realValue,
				SInt16								decimalPlaces)

{
	LoadDItemRealValue (dialogPtr, 
								itemNumber, 
								realValue,
								decimalPlaces,
								decimalPlaces);
	
}	// end "LoadDItemRealValue" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadDItemRealValue
//
//	Software purpose:	The purpose of this routine is to convert the
//							input real value to a string with the requested 
//							number of decimal places and load it into the
//							requested dialog item number.
//
//	Parameters in:		pointer to the dialog structure (or class)
//							dialog item to be modified
//							the value to be entered
//							the number of decimal digits if in f format
//								note that if this is negative, g format will be used
//							the number of decimal digits if in e format
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			ClassifyDialog   in SClassifyDialogs.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/19/1989
//	Revised By:			Larry L. Biehl			Date: 03/16/2012

void LoadDItemRealValue (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber, 
				double								realValue,
				SInt16								numberFDecimalDigits,
				SInt16								numberEDecimalDigits)

{
	SInt16								decimalLocation,
											index,
											numberChars;
											
	Boolean								eFormatFlag = FALSE;
	
	
	if (dialogPtr != NULL && itemNumber > 0)
		{
		numberChars = LoadRealValueString ((char*)&gTextString[1],
														realValue,
														0,
														numberFDecimalDigits,
														numberEDecimalDigits,
														(char*)"",
														(char*)"");
		
				// Make sure that the string length is not more than 255.		
				
		numberChars = MIN (numberChars, 255);
				
		//sprintf ((char*)&gTextString[1], "%.*f", decimalPlaces, realValue);

				// Determine if this is an E format string.																
				
		index = numberChars;
		for (; index>0; index--)
			{
			if (gTextString[index] == 'E')
				{
				eFormatFlag = TRUE;
				break;
				
				}	// end "for (; index>0; index--)"
				
			}	// end "for (; index>0; index--)"
		
		if (!eFormatFlag)	
			{
					// Determine whether a decimal point exists.  If so remove
					// trailing zeros.				
			
			decimalLocation = -1;
			for (index=1; index<numberChars; index++)
				{
				if (gTextString[index] == '.')
					{
					decimalLocation = index;
					break;
					
					}	// end "if (gTextString[index] == '.')"
					
				}	// end "for (index=1; index<numberChars; index++)" 
			
			if (decimalLocation > 0)
				{
						// Remove any trailing 0's except the first one after a			
						// decimal point.																
						
				index = numberChars;
				decimalLocation++;
				for (; index>decimalLocation; index--)
					{
					if (gTextString[index] != '0')
						break;
						
					}	// end "for (; index>0; index--)" 
					
				numberChars = MAX (index, 2);
				
				}	// end "if (decimalFlag)" 
				
			}	// end "if (!eFormatFlag)"
		
				// Make it a pascal string.
						
		gTextString[0] = (UInt8)numberChars;
			
		#if defined multispec_mac 
			Rect							theBox;
			Handle						theHandle;
			SInt16						theType;
		
			GetDialogItem (dialogPtr, itemNumber, &theType, &theHandle, &theBox);
			SetDialogItemText (theHandle, gTextString);
		#endif	// defined multispec_mac

		#if defined multispec_win
		   dialogPtr->LoadDItemString ((UInt16)itemNumber, (Str255*)gTextString);
		#endif // defined multispec_win 
		
      #if defined multispec_wx
        dialogPtr->LoadDItemString ((UInt16) itemNumber, (Str255*)gTextString);
        dialogPtr->Fit ();
      #endif // defined multispec_wx

		}	// end "if (dialogPtr != NULL && itemNumber > 0)" 
	
}	// end "LoadDItemRealValue" 


                                           
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadDItemString
//
//	Software purpose:	The purpose of this routine is to load the
//							specified string into the requested dialog item
//							number.
//							This is for a overloaded routine for LoadDItemString
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			ClassifyDialog   in SClassifyDialogs.cpp
//
//	Coded By:			Larry L. Biehl			Date: 09/04/2017
//	Revised By:			Larry L. Biehl			Date: 04/13/2020
                         
void LoadDItemString (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber, 
				Str255* 								theStringPtr)

{
	LoadDItemString (dialogPtr,
							itemNumber,
							theStringPtr,
							FALSE);

}	// end "LoadDItemString"


                                           
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadDItemString
//
//	Software purpose:	The purpose of this routine is to load the
//							specified string into the requested dialog item
//							number.
//							The routine handles the choice for the input being a wide
//							character string or a single character string.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			several routines
//
//	Coded By:			Larry L. Biehl			Date: 01/06/1989
//	Revised By:			Larry L. Biehl			Date: 04/13/2020
                         
void LoadDItemString (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber, 
				Str255* 								theStringPtr,
				Boolean								unicodeInputStringFlag)

{          
	if (dialogPtr != NULL && itemNumber > 0)
		{  
		#if defined multispec_mac
			Handle								theHandle;
			Rect									theBox;
				
			SInt16								theType;
				
			
			GetDialogItem (dialogPtr, itemNumber, &theType, &theHandle, &theBox);
			
			if (unicodeInputStringFlag)
				{
				OSErr									osErr;
				ControlRef							theControl;
				CFStringRef							cfStringRef;
				Boolean								validControlFlag;
				UCharPtr		stringPtr = (UCharPtr)theStringPtr;
				
				cfStringRef = CFStringCreateWithBytes (kCFAllocatorDefault,
																	&stringPtr[1],
																	stringPtr[0],
																	kCFStringEncodingUTF8,
																	false);
																
				if (cfStringRef != NULL)
					{
					if (theHandle != NULL)
						{
						SetDialogItem (dialogPtr,
											itemNumber,
											kControlDialogItem,
											theHandle,
											&theBox);
						osErr = ::GetDialogItemAsControl (dialogPtr,
																		itemNumber,
																		&theControl);
						//SetDialogItem (dialogPtr, itemNumber, theType, theHandle, &theBox);
						
                  validControlFlag = FALSE;
						if (osErr == noErr)
							validControlFlag = IsValidControlHandle (theControl);
							/*
							osErr = SetControlData (theControl,
																kControlEntireControl,
																kControlStaticTextCFStringTag,
																CFStringGetLength (cfStringRef),
																cfStringRef);

							osErr = SetControlData (theControl,
															kControlEntireControl,
															kControlStaticTextTextTag,
															CFStringGetLength (cfStringRef),
															CFStringGetCStringPtr (
																			cfStringRef,
																			kCFStringEncodingMacRoman));
							*/
                  if (validControlFlag)
                     osErr = SetControlTitleWithCFString (theControl, cfStringRef);
						
						SetDialogItem (dialogPtr, itemNumber, theType, theHandle, &theBox);
						
						}	// end "if (theHandle != NULL)"
					
							// Using CFallocatorDeallocate causes system to crash later.
							// Not sure when one should use it and not use it.

					CFRelease (cfStringRef);
					
					}	// end "if (cfStringRef != NULL)"

				}	// end "if (unicodeInputStringFlag)"

			else	// !unicodeInputStringFlag
				{
				if (theType & 0x0018)
					SetDialogItemText (theHandle, (ConstStr255Param)theStringPtr);
				
				}	// end "else !unicodeInputStringFlag
		#endif	// defined multispec_mac
		
		#if defined multispec_win
			TRY
				{ 
				dialogPtr->LoadDItemString ((UInt16)itemNumber, 
														theStringPtr,
														unicodeInputStringFlag);
				}
					
			CATCH_ALL (e)
				{
				Boolean tryFailedFlag = TRUE;
				}
			END_CATCH_ALL         
		#endif	// defined multispec_win
		
		#if defined multispec_wx
					// No error handling for now
         dialogPtr->LoadDItemString ((UInt16)itemNumber, theStringPtr);
		#endif	// defined multispec_wx
		
		}	// end "if (dialogPtr != NULL && itemNumber > 0)" 
						  
}	// end "LoadDItemString"

 
                         
void LoadDItemString (
				DialogPtr							dialogPtr,
				SInt16								itemNumber,
				CharPtr								theStringPtr)

{                   
	if (dialogPtr != NULL && itemNumber > 0)
		{  
		#if defined multispec_mac
			LoadDItemString (dialogPtr, itemNumber, (Str255*)theStringPtr);
		#endif	// defined multispec_mac
	
		#if defined multispec_win || defined multispec_wx
		   dialogPtr->LoadDItemString ((UInt16)itemNumber, theStringPtr); 
		#endif	// defined multispec_win || defined multispec_wx
					
		}	// end "if (dialogPtr != NULL && itemNumber > 0)" 
						  
}	// end "LoadDItemString"
	                                

                     
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadDItemStringNumber
//
//	Software purpose:	The purpose of this routine is to load the
//							specified string number from the resource file
//							into the requested dialog item number.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			ClassifyDialog   in SClassifyDialogs.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/28/1993
//	Revised By:			Larry L. Biehl			Date: 05/01/2015

void LoadDItemStringNumber (
				SInt16								strListID, 
				SInt16								stringNumber, 
				DialogPtr							dialogPtr, 
				SInt16								itemNumber, 
				Str255* 								stringPtr)

{
	if (dialogPtr != NULL)
		{
		if (MGetString ((UCharPtr)stringPtr, strListID, stringNumber))
			{
			LoadDItemString (dialogPtr, itemNumber, stringPtr);
		
			#ifdef multispec_wx
				dialogPtr->Fit ();
			#endif
			
			}	// end "if (MGetString ((UCharPtr)stringPtr, strListID, stringNumber))"
			
		}	// end "if (dialogPtr != NULL)"
        
}	// end "LoadDItemStringNumber" 


                       
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadDItemValue
//
//	Software purpose:	The purpose of this routine is to convert the
//							input value to a string and load it into the
//							requested dialog item number.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			ClassifyDialog   in SClassifyDialogs.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/07/1988
//	Revised By:			Larry L. Biehl			Date: 01/20/2016

void LoadDItemValue (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber, 
				SInt64								theNum)

{   
	if (dialogPtr != NULL && itemNumber > 0)
		{   
		#if defined multispec_wx
	   	dialogPtr->LoadDItemValue (itemNumber, (SInt32)theNum);
			dialogPtr->Fit ();
		#endif // defined multispec_wx
		
		#if defined multispec_mac
			Handle					theHandle;
			Rect						theBox; 
			SInt16					theType; 
		
			GetDialogItem (dialogPtr, itemNumber, &theType, &theHandle, &theBox);
			NumToString (theNum, gTextString);
			SetDialogItemText (theHandle, gTextString);
		#endif	// defined multispec_mac

		#if defined multispec_win
	   	dialogPtr->LoadDItemValue (itemNumber, (SInt32)theNum);
		#endif // defined multispec_win

		}	// end "if (dialogPtr != NULL && itemNumber > 0)"
	
}	// end "LoadDItemValue" 


                       
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadDItemValueWithCommas
//
//	Software purpose:	The purpose of this routine is to convert the
//							input value to a string and load it into the
//							requested dialog item number.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			ClassifyDialog   in SClassifyDialogs.cpp
//
//	Coded By:			Larry L. Biehl			Date: 08/27/2010
//	Revised By:			Larry L. Biehl			Date: 01/20/2016

void LoadDItemValueWithCommas (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber, 
				SInt64								theNum)

{  
	Str255								pixelCountString;
	
	SInt16								numberChars;


	if (dialogPtr != NULL && itemNumber > 0)
		{   
		numberChars = CreateNumberWithCommasInString ((char*)&pixelCountString[1], 
																		theNum);
		#if defined multispec_mac 
			Handle					theHandle;
			Rect						theBox; 
			SInt16					theType; 
		
			GetDialogItem (dialogPtr, itemNumber, &theType, &theHandle, &theBox);
			
			pixelCountString[0] = numberChars;
			SetDialogItemText (theHandle, pixelCountString);
		#endif	// defined multispec_mac

      #if defined multispec_win
	   	dialogPtr->LoadDItemString (itemNumber, (char*)&pixelCountString[1]);
		#endif // defined multispec_win

      #if defined multispec_wx
	   	dialogPtr->LoadDItemString (itemNumber, (char*)&pixelCountString[1]);
			dialogPtr->Fit ();
		#endif // defined multispec_wx
		
		}	// end "if (dialogPtr != NULL && itemNumber > 0)" 
	
}	// end "LoadDItemValueWithCommas" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadLineColumnItems
//
//	Software purpose:	The purpose of this routine is to handle the 
//							processing for dialog hits in line or column items.
//		
//	Parameters in:		Pointer to dialog selection area structure
//							Pointer to the dialog box
//							Flag indicating whether the line, column, intervals are to be set
//								in the input dialog box
//							Flag indicating whether the interval items in the set exist
//							Value indicating which line/column/interval are to be initialize. 
//								This is only used for windows and linux.
//
//	Parameters out:	None
//
// Value Returned:	None.
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 12/13/1994
//	Revised By:			Larry L. Biehl			Date: 07/14/2015

void LoadLineColumnItems (
				DialogSelectAreaPtr				dialogSelectAreaPtr,
				DialogPtr							dialogPtr,
				Boolean								initializeLineColumnValuesFlag,
				Boolean								intervalEditBoxesExistFlag,
				SInt16								lineColumnSetNumber)

{
   
	#if defined multispec_mac 
		UInt16						index;
		
		
				// To entire image icon.															
		
		if (dialogSelectAreaPtr->entireIconItem != 0)
			{
			if (!dialogSelectAreaPtr->drawRoutineLoadedFlag)
				{
				SetDialogItemDrawRoutine (dialogPtr,
													dialogSelectAreaPtr->entireIconItem, 
													&dialogSelectAreaPtr->toSelectionBox, 
													gSetEntireImageIconPtr);
				
				Boolean	scaleFlag = TRUE;
				SInt16	theType;
				Handle	theHandle;
				Rect		theRect;
				
				GetDialogItem (dialogPtr, 
									dialogSelectAreaPtr->entireIconItem,
									&theType, 
									&theHandle, 
									&theRect);
				
				if (theType != kUserDialogItem)
					{
					OSErr									osErr;
					ControlRef							theControl;
							
					osErr = ::GetDialogItemAsControl (dialogPtr, 
																	dialogSelectAreaPtr->entireIconItem, 
																	&theControl);
				
					osErr = SetControlData (theControl,
													0, 
													kControlBevelButtonScaleIconTag, 
													sizeof (Boolean),
													(Ptr)&scaleFlag);
					
					}	// end "if (theType != kUserDialogItem)"
							
				dialogSelectAreaPtr->drawRoutineLoadedFlag = TRUE;
							
				}	// end "if (!dialogSelectAreaPtr->drawRoutineLoadedFlag)"
							
			gEntireImageFlag = CheckIfEntireImage (
															dialogSelectAreaPtr->imageWindowInfoPtr,
															dialogSelectAreaPtr->lineStart, 
															dialogSelectAreaPtr->lineEnd, 
															dialogSelectAreaPtr->columnStart, 
															dialogSelectAreaPtr->columnEnd);	

			UpdateEntireIconButton (dialogSelectAreaPtr, dialogPtr);
						
			}	// end "if (dialogSelectAreaPtr->entireIconItem != 0)" 
							
				//	Display start line; 																
		
		LoadDItemValue (dialogPtr, 
								dialogSelectAreaPtr->baseItem, 
								dialogSelectAreaPtr->lineStart);
		
				// Display end line of image														
		
		LoadDItemValue (dialogPtr, 
								dialogSelectAreaPtr->baseItem+1, 
								dialogSelectAreaPtr->lineEnd);
		
				// Display line interval of image	
		
		index = 2;
		if (intervalEditBoxesExistFlag)
			{
			LoadDItemValue (dialogPtr, 
									dialogSelectAreaPtr->baseItem+index,
									dialogSelectAreaPtr->lineInterval);
			index++;
								
			}	// end "if (intervalEditBoxesExistFlag)"
		
				// Display start column of image													
		
		LoadDItemValue (dialogPtr, 
								dialogSelectAreaPtr->baseItem+index, 
								dialogSelectAreaPtr->columnStart);
		index++;
		
				// Display end column of image													
		
		LoadDItemValue (dialogPtr, 
								dialogSelectAreaPtr->baseItem+index, 
								dialogSelectAreaPtr->columnEnd);
		index++;
		
				// Display column interval of image												
		
		if (intervalEditBoxesExistFlag)
			LoadDItemValue (dialogPtr, 
									dialogSelectAreaPtr->baseItem+index, 
									dialogSelectAreaPtr->columnInterval);
	#endif	// defined multispec_mac  	
			
	#if defined multispec_win || defined multispec_wx
		if (dialogSelectAreaPtr->imageWindowInfoPtr != NULL)
			dialogPtr->SetMaxNumberLinesAndColumns (
									dialogSelectAreaPtr->imageWindowInfoPtr->maxNumberLines,
									dialogSelectAreaPtr->imageWindowInfoPtr->maxNumberColumns);
							
				// To entire image icon.															
		
		if (dialogSelectAreaPtr->entireIconItem != 0)
			dialogPtr->SetEntireImageButtons (dialogPtr,
															dialogSelectAreaPtr->lineStart,
															dialogSelectAreaPtr->lineEnd,
															dialogSelectAreaPtr->columnStart,
															dialogSelectAreaPtr->columnEnd);
															
		if (initializeLineColumnValuesFlag)
			{   
			if (lineColumnSetNumber == 1)
				{
						//	Enter start, end and interval for lines. 																
				
				dialogPtr->LoadDItemValue (IDC_LineStart, dialogSelectAreaPtr->lineStart);
				dialogPtr->LoadDItemValue (IDC_LineEnd, dialogSelectAreaPtr->lineEnd);
             
				if (intervalEditBoxesExistFlag)
					dialogPtr->LoadDItemValue (IDC_LineInterval,
														dialogSelectAreaPtr->lineInterval);
				
						// Enter start, end and interval for columns.													
				
				dialogPtr->LoadDItemValue (IDC_ColumnStart,
													dialogSelectAreaPtr->columnStart);
				dialogPtr->LoadDItemValue (IDC_ColumnEnd, dialogSelectAreaPtr->columnEnd);
            
				if (intervalEditBoxesExistFlag)
					dialogPtr->LoadDItemValue (IDC_ColumnInterval,
														dialogSelectAreaPtr->columnInterval);
				
				}	// end "if (lineColumnSetNumber == 1)"

			else if (lineColumnSetNumber == 2)
				{
						//	Enter start, end and interval for lines.
				
				dialogPtr->LoadDItemValue (IDC_LineStart2, dialogSelectAreaPtr->lineStart);
				dialogPtr->LoadDItemValue (IDC_LineEnd2, dialogSelectAreaPtr->lineEnd);
				
						// So far no intervals for set 2
				
				//if (intervalEditBoxesExistFlag)
				//	dialogPtr->LoadDItemValue (IDC_LineInterval2,
				//										dialogSelectAreaPtr->lineInterval);
				
						// Enter start, end and interval for columns.
				
				dialogPtr->LoadDItemValue (IDC_ColumnStart2,
													dialogSelectAreaPtr->columnStart);
				dialogPtr->LoadDItemValue (IDC_ColumnEnd2, dialogSelectAreaPtr->columnEnd);
				
						// So far no intervals for set 2
				
				//if (intervalEditBoxesExistFlag)
				//	dialogPtr->LoadDItemValue (IDC_ColumnInterval2,
				//										dialogSelectAreaPtr->columnInterval);
				
				}	// end "if (lineColumnSetNumber == 2)"

			else if (lineColumnSetNumber == 3)
				{
						//	Enter start, end and interval for lines.
				
				dialogPtr->LoadDItemValue (IDC_LineStart3, dialogSelectAreaPtr->lineStart);
				dialogPtr->LoadDItemValue (IDC_LineEnd3, dialogSelectAreaPtr->lineEnd);
				
						// So far no intervals for set 3
				
				//if (intervalEditBoxesExistFlag)
				//	dialogPtr->LoadDItemValue (IDC_LineInterval3,
				//										dialogSelectAreaPtr->lineInterval);
				
						// Enter start, end and interval for columns.
				
				dialogPtr->LoadDItemValue (IDC_ColumnStart3,
													dialogSelectAreaPtr->columnStart);
				dialogPtr->LoadDItemValue (IDC_ColumnEnd3, dialogSelectAreaPtr->columnEnd);

						// So far no intervals for set 3
				
				//if (intervalEditBoxesExistFlag)
				//	dialogPtr->LoadDItemValue (IDC_ColumnInterval3,
				//										dialogSelectAreaPtr->columnInterval);
				
				}	// end "if (lineColumnSetNumber == 3)"
				
			}	// end "if (initializeLineColumnValuesFlag)"
	#endif	// defined multispec_win || defined multispec_wx
				
}	// end "LoadLineColumnItems" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadProcessorVectorsFromDialogLocalVectors
//
//	Software purpose:	The purpose of this routine is to load the input processor
//							parameter values with the dialog local vectors.
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned: 	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 05/01/1998
//	Revised By:			Larry L. Biehl			Date: 08/15/2012

void LoadProcessorVectorsFromDialogLocalVectors (
				SInt16								channelSelection,
				Boolean								featureTransformationFlag,
				UInt16								numberAllChannels,
				UInt16								localNumberFeatures,
				UInt16*								localFeaturesPtr,
				SInt16*								channelSetPtr,
				UInt16*								numberFeaturesPtr,
				Handle								featureHandle,
				UInt16*								numberChannelsPtr,
				Handle								channelsHandle,
				SInt16								classSelection,
				UInt32								localNumberClasses,
				UInt16*								localClassPtr,
				SInt16*								classSetPtr,
				UInt32*								numberClassesPtr,
				Handle								classHandle,
				SInt16								symbolSelection,
				unsigned char*						localSymbolPtr,
				SInt16*								symbolSetPtr,
				Handle								symbolsHandle,
				SInt16								interClassWeightsSelection,
				SInt16*								localClassPairWeightsListPtr,
				SInt16*								classPairWeightSetPtr)
	
{
	UInt32								index,
											numberClasses;
											
	Boolean								projectFlag;
	
	
			// Channels		
	
	if (featureHandle != NULL)
		{
		SInt16* featurePtr = (SInt16*)GetHandlePointer (featureHandle);
													
		*channelSetPtr = channelSelection;
		if (channelSelection == kAllMenuItem)		// All channels or features 
			LoadFeatureVector (featureTransformationFlag,
										(SInt16*)numberFeaturesPtr,
										featurePtr,
										numberAllChannels);
										//gProjectInfoPtr->numberStatisticsChannels);
		
		else	// channelSelection == kSubsetMenuItem 
			{				    		
			*numberFeaturesPtr = localNumberFeatures; 
			for (index=0; index<localNumberFeatures; index++)
				featurePtr[index] = localFeaturesPtr[index];
													
			}	// end "else channelSelection == kSubsetMenuItem"															
		
		if (channelsHandle != NULL)
			{
			SInt16* channelPtr = (SInt16*)GetHandlePointer (channelsHandle); 
													
			projectFlag = kProject;
			if (gProcessorCode == kClusterProcessor)
						// For clustering the cluster channels vector will be used not
						// any current project channels vector.
				projectFlag = kNoProject;
														
			LoadChannelsVector (projectFlag,
										featureTransformationFlag,
										*numberFeaturesPtr, 
										featurePtr, 
										(SInt16*)numberChannelsPtr,
										channelPtr);
									
			}	// end "if (channelsHandle != NULL)"
								
		}	// end "if (featureHandle != NULL)"

			// Classes																	

	if (classHandle != NULL)
		{
		*classSetPtr = classSelection;
	   
		SInt16* classPtr = (SInt16*)GetHandlePointer (classHandle);
													
		if (classSelection == kAllMenuItem)		// All classes 
			LoadTrainClassVector (numberClassesPtr,
											classSetPtr, 
											classPtr);
												
		else	// classSelection == kSubsetMenuItem
			{                                              
			*numberClassesPtr = localNumberClasses;
			for (index=0; index<localNumberClasses; index++)
				classPtr[index] = localClassPtr[index]; 
				
			}	// end "else classSelection == kSubsetMenuItem"
			
		}	// end "if (classHandle != NULL)"
								
			// Class symbols.	
	
	if (symbolsHandle != NULL)
		{		
		*symbolSetPtr = symbolSelection;
			
		numberClasses = gProjectInfoPtr->numberStatisticsClasses;
		if (gProcessorCode == kClusterProcessor)
			numberClasses = kMaxNumberStatClasses;
				
		unsigned char* symbolsPtr = (UCharPtr)GetHandlePointer (symbolsHandle);													
		
		if (symbolSelection == kDefaultMenuItem)		
			LoadClassSymbolVector (symbolsPtr, 
											IDS_Symbol1,
											(SInt16)numberClasses);
												
		else	// symbolSelection != kDefaultMenuItem
					// Note that the index for symbols starts at 1.
					
			BlockMoveData (localSymbolPtr, 
								symbolsPtr,
								numberClasses+1);
			
		}	// end "if (symbolsHandle != NULL)"
													
			// Class Pair Weights.	
			
	if (classPairWeightSetPtr != NULL)
		*classPairWeightSetPtr = interClassWeightsSelection; 
			
	if (localClassPairWeightsListPtr != NULL && 
													interClassWeightsSelection == kUnequalWeights)
		{
		CopyPointerToHandle ((Ptr)localClassPairWeightsListPtr,
										&gProjectInfoPtr->classPairWeightsListHandle);

				// Verify that the handle is locked.
				
		MHLock (gProjectInfoPtr->classPairWeightsListHandle);
										
		}	// end "if (localClassPairWeightsListPtr != NULL && ..."
																	
}	// end "LoadProcessorVectorsFromDialogLocalVectors"


                       
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadSubsetList
//
//	Software purpose:	The purpose of this routine is to load the input vector with
//							the last saved subset list if it exists and the last selection
//							is for all items in the list.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 05/14/1998
//	Revised By:			Larry L. Biehl			Date: 10/06/1998

void LoadSubsetList (
				SInt16*								numberOutputFeaturesPtr, 
				SInt16* 								selectedFeaturePtr,
				SInt16* 								availableFeaturePtr, 
				SInt16								numberInputChannels,
				SInt16*								currentSelectionPtr)

{  
	ChannelSubsetsPtr					channelSubsetsPtr;
	SInt32*								channelSubsetsHeaderPtr;
	UInt16*								savedFeaturePtr;
	
	SInt32								firstSet;
	
	UInt32								availIndex,
											index,
											numberSavedFeatures,
											outputIndex;
											
	UInt16								selectedFeature;
	
		
	if (*currentSelectionPtr == kAllMenuItem && gChannelSubsetsH != NULL)
		{
		channelSubsetsHeaderPtr = (SInt32*)GetHandlePointer (gChannelSubsetsH,
																				kLock);
																				
		firstSet = channelSubsetsHeaderPtr[1];
		
		channelSubsetsPtr	= (ChannelSubsetsPtr)&channelSubsetsHeaderPtr[firstSet];
		
				// Now copy the channels in the last saved feature subset to
				// the current selected feature vector.
				
		numberSavedFeatures = channelSubsetsPtr->numberChannels;
		savedFeaturePtr = channelSubsetsPtr->channelList;
		
		availIndex = 0;
		outputIndex = 0;
		for (index=0; index<numberSavedFeatures; index++)
			{
			if (availableFeaturePtr == NULL)
				{
				selectedFeature = savedFeaturePtr[index];
				availIndex = selectedFeature;
				
				if (availIndex >= (UInt32)numberInputChannels)
					selectedFeature = 0;
				
				}	// end "if (availableFeaturePtr == NULL)"
				
			else	// availableFeaturePtr != NULL
				{
				while (availIndex < (UInt32)(numberInputChannels-1) &&
						(UInt16)availableFeaturePtr[availIndex] < savedFeaturePtr[index])
					availIndex++;
					
				selectedFeature = availableFeaturePtr[availIndex];
					
				}	// end "else availableFeaturePtr != NULL"
					
			if (savedFeaturePtr[index] == selectedFeature)
				{
				selectedFeaturePtr[outputIndex] = (UInt16)availIndex;
				outputIndex++;
				
				}	// end "if (savedFeaturePtr[index] == selectedFeature)"
			
			}	// end "for (index=0; index<numberSavedFeatures; index++)"
			
		*numberOutputFeaturesPtr = (SInt16)outputIndex;	
			
		CheckAndUnlockHandle (gChannelSubsetsH);
		
		*currentSelectionPtr = kSubsetMenuItem;												
		
		}	// end "if (currentSelection == kAllMenuItem && ..."
																	
}	// end "LoadSubsetList"  


                       
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void MHideDialogItem
//
//	Software purpose:	The purpose of this routine is to hide the specified
//							dialog item number.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			ClassifyDialog   in SClassifyDialogs.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/14/1995
//	Revised By:			Larry L. Biehl			Date: 11/14/1995

void MHideDialogItem (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{   
	if (dialogPtr != NULL && itemNumber > 0)
		{   
		#if defined multispec_mac 
			HideDialogItem (dialogPtr, itemNumber);
		#endif	// defined multispec_mac

      #if defined multispec_win || defined multispec_wx
	   	dialogPtr->MHideDialogItem (dialogPtr, itemNumber);
		#endif // defined multispec_win || defined multispec_wx
		
		}	// end "if (dialogPtr != NULL && itemNumber > 0)" 
	
}	// end "MHideDialogItem" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void NumberErrorAlert
//
//	Software purpose:	The purpose of this routine is to notify the user 
//							that an illegal number was entered into a dialog 
//							text field.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 04/14/1988
//	Revised By:			Larry L. Biehl			Date: 01/30/1998

void NumberErrorAlert (
				SInt32								theLongValue,
				DialogPtr							dialogPtr, 
				SInt16								itemHit)

{
	#if defined multispec_mac
		Alert (kErrorAlert2ID, NIL);
		LoadDItemValue (dialogPtr, itemHit, theLongValue);
		SelectDialogItemText (dialogPtr, itemHit, 0, SHRT_MAX);
		//SysBeep (1);
	#endif	// defined multispec_mac

} 	// end "NumberErrorAlert"


                                                
#if defined multispec_win
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void PositionDialogWindow
//
//	Software purpose:	The purpose of this routine is to set the hilite
//							of the control in the dialog window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 06/29/1995
//	Revised By:			Larry L. Biehl			Date: 12/01/1995

void PositionDialogWindow (
				CDialog*								dialogPtr)

{  
	Rect									dialogRect,
											screenRect; 

	SInt16								newLeft,
											newTop;
				

			// Get rectangle for dialog box
		
	dialogPtr->GetWindowRect ((tagRECT*)&dialogRect);
	
			// Get rectangle for screen
			
	GetScreenRect (&screenRect);
	
			// Calculate upper left corner that will leave the box centered.
			
	newLeft = dialogRect.left + (screenRect.right + screenRect.left)/2 -
															(dialogRect.right + dialogRect.left)/2;
			
	newTop = dialogRect.top + (screenRect.top + screenRect.bottom)/2 -
															(dialogRect.top + dialogRect.bottom)/2;
					
			// Adjust so that 1/5 of space is above and 4/5's of space is below
			
	newTop -= (newTop - screenRect.top) * 6/10;
	
	Boolean returnFlag = dialogPtr->SetWindowPos (NULL,
																	newLeft, 
																	newTop, 
																	0, 
																	0, 
																	SWP_NOSIZE | SWP_NOZORDER); 

}	// end "PositionDialogWindow"
#endif	// defined multispec_win


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ReleaseDialogLocalVectors
//
//	Software purpose:	The purpose of this routine is to release the input vectors
//							that were set up for dialog boxes.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/10/1998
//	Revised By:			Larry L. Biehl			Date: 12/28/1999 

void ReleaseDialogLocalVectors (
				UInt16*								channelsPtr,
				UInt16*								transformFeaturesPtr,
				UInt16*								classPtr,
				UInt16*								classAreaPtr,
				float*								classWeightsPtr,
				unsigned char*						symbolsPtr,
				UInt16*								channelCombinationsPtr,
				SInt16*								classPairWeightsListPtr)
	
{ 
	CheckAndDisposePtr (channelsPtr);  
	CheckAndDisposePtr (transformFeaturesPtr);
	CheckAndDisposePtr (classPtr);
	CheckAndDisposePtr (classAreaPtr); 
	CheckAndDisposePtr (classWeightsPtr); 
	CheckAndDisposePtr (symbolsPtr); 
	CheckAndDisposePtr (channelCombinationsPtr);
	CheckAndDisposePtr (classPairWeightsListPtr);

}	// end "ReleaseDialogLocalVectors" 


                       
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SaveSubsetList
//
//	Software purpose:	The purpose of this routine is to save the input subset vector 
//							to the storage for the last subset vector in case it is needed
//							later. The items in the list are saved as actual channel (or
//							feature number starting from 1.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 05/14/1998
//	Revised By:			Larry L. Biehl			Date: 06/10/1998

void SaveSubsetList (
				SInt16								numberSelectedFeatures, 
				SInt16* 								selectedFeaturePtr,
				SInt16* 								availableFeaturePtr, 
				SInt16								numberInputChannels)

{  
	ChannelSubsetsPtr					channelSubsetsPtr;
	SInt32*								channelSubsetsHeaderPtr;
	UInt16*								savedFeaturePtr;
	
	UInt32								bytesNeeded,
											index;
											
	UInt16								selectedFeature;
	
	Boolean								changedFlag,
											returnFlag;
	
	
			// Only save the set if it is actually a subset.
			
	if (numberSelectedFeatures < numberInputChannels)
		{
				// First verify that we have enough memory for the list.
				
		bytesNeeded = 2 * sizeof (SInt32) + sizeof (ChannelSubsets) + 
															numberSelectedFeatures * sizeof (UInt16);
														
		channelSubsetsHeaderPtr = (SInt32*)CheckHandleSize (&gChannelSubsetsH,
																				&returnFlag,  
																				&changedFlag, 
																				bytesNeeded);
		
		if (returnFlag)
			{
			channelSubsetsHeaderPtr[0] = 1;
			channelSubsetsHeaderPtr[1] = 3;
			
			channelSubsetsPtr	= (ChannelSubsetsPtr)&channelSubsetsHeaderPtr[3];
			
					// Now save the channel subset.
					
			channelSubsetsPtr->numberChannels = numberSelectedFeatures;
			
			savedFeaturePtr = channelSubsetsPtr->channelList;
			
			for (index=0; index<(UInt32)numberSelectedFeatures; index++)
				{
				if (availableFeaturePtr == NULL)
					selectedFeature = selectedFeaturePtr[index];
					
				else	// availableFeaturePtr != NULL
					selectedFeature = availableFeaturePtr[selectedFeaturePtr[index]];
						
				savedFeaturePtr[index] = selectedFeature;
				
				}	// end "for (index=0; index<numberSavedFeatures; index++)"
			
			CheckAndUnlockHandle (gChannelSubsetsH);
			
			}	// end "if (returnFlag)"												
		
		}	// end "if (numberSelectedFeatures < numberInputChannels)"
																	
}	// end "SaveSubsetList"  


                       
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetDialogItemToEditText
//
//	Software purpose:	The purpose of this routine is to set the specified dialog
//							item as an edit text item.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/18/2012
//	Revised By:			Larry L. Biehl			Date: 03/28/2012
// TODO: For Linux
void SetDialogItemToEditText (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{   
	if (dialogPtr != NULL && itemNumber > 0)
		{   
		#if defined multispec_wx
			wxTextCtrl* text = (wxTextCtrl*) (dialogPtr->FindWindow (itemNumber));
			text->Enable (true);
		#endif // defined multispec_wx

		#if defined multispec_mac
			Handle					theHandle;
			Rect						theBox; 
			SInt16					theType; 
		
			GetDialogItem (dialogPtr, itemNumber, &theType, &theHandle, &theBox);
			
			if (theHandle != NULL && theType != kEditTextDialogItem)
				{
				theType = kEditTextDialogItem;
				SetDialogItem (dialogPtr, itemNumber, theType, theHandle, &theBox);
				InsetRect (&theBox, -5, -5);
				InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
				
				}	// end "if (theHandle != NULL && ..."
		#endif	// defined multispec_mac

		#if defined multispec_win
			dialogPtr->GetDlgItem (itemNumber)->SendMessage (EM_SETREADONLY,0,0);
		#endif // defined multispec_win 
		
		}	// end "if (dialogPtr != NULL && itemNumber > 0)"
	
}	// end "SetDialogItemToEditText" 

                       
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetDialogItemToStaticText
//
//	Software purpose:	The purpose of this routine is to set the specified dialog item
//							as a static text item.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/18/2012
//	Revised By:			Larry L. Biehl			Date: 07/05/2017

void SetDialogItemToStaticText (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{   
	if (dialogPtr != NULL && itemNumber > 0)
		{   
		#if defined multispec_wx
			wxTextCtrl* text = (wxTextCtrl*)(dialogPtr->FindWindow (itemNumber));
			//text->SetEditable (false);
			//text->Disable ();
			text->Enable (false);
		#endif
		
		#if defined multispec_mac 
			Handle					theHandle;
			Rect						theBox; 
			SInt16					theType; 
		
			GetDialogItem (dialogPtr, itemNumber, &theType, &theHandle, &theBox);
			
			if (theHandle != NULL && theType != kStaticTextDialogItem + kItemDisableBit)
				{
				theType = kStaticTextDialogItem + kItemDisableBit;			
				SetDialogItem (dialogPtr, itemNumber, theType, theHandle, &theBox);
				InsetRect (&theBox, -5, -5);
				InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
				
				}	// end "if (theHandle != NULL && ..."
		#endif	// defined multispec_mac

		#if defined multispec_win
			dialogPtr->GetDlgItem (itemNumber)->SendMessage (EM_SETREADONLY,1,0);
		#endif // defined multispec_win 
		}	// end "if (dialogPtr != NULL && itemNumber > 0)" 
	
}	// end "SetDialogItemToStaticText"



//-----------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetDLogControl
//
//	Software purpose:	The purpose of this routine is to set the control
//							value of the control in the dialog window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			StatisticsDialog   in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/30/1989
//	Revised By:			Larry L. Biehl			Date: 03/01/1996

void SetDLogControl (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber, 
				SInt16								setting)

{
   #if defined multispec_wx
		wxCheckBox* buttonPtr = wxDynamicCast (
											dialogPtr->FindWindow (itemNumber), wxCheckBox);
	
		if (buttonPtr != NULL)
			buttonPtr->SetValue (setting);
      
		else 
			{
			wxRadioBox* buttonPtr = wxDynamicCast (
												dialogPtr->FindWindow (itemNumber), wxRadioBox);
			if (buttonPtr != NULL)
				{
				buttonPtr->SetSelection (setting);
            return;
				}
				            
         wxRadioButton* buttonPtr2 = wxDynamicCast (
											dialogPtr->FindWindow (itemNumber), wxRadioButton);
         if (buttonPtr2 != NULL)
				{
				buttonPtr2->SetValue (setting);
            return;
				}
			}
   #endif	// defined multispec_wx

	#if defined multispec_mac
		Handle							theHandle;
		Rect								theBox;
		
		SInt16							theType;
		
		
		if (dialogPtr != NULL)
			{
			GetDialogItem (dialogPtr, itemNumber,  &theType, &theHandle, &theBox);
			SetControlValue ((ControlHandle)theHandle, setting);
			
			}	// end "if (dialogPtr != NULL)" 
	#endif // defined multispec_mac
			
	#if defined multispec_win 
		CButton* buttonPtr = (CButton*)(dialogPtr->GetDlgItem (itemNumber));
		
		if (buttonPtr != NULL)
			buttonPtr->SetCheck (setting);
		
		//CheckDlgButton (itemNumber, setting);
	#endif	// defined multispec_win 
												
}	// end "SetDLogControl"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetDLogControlHilite
//
//	Software purpose:	The purpose of this routine is to set the hilite
//							of the control in the dialog window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			Many dialogs
//
//	Coded By:			Larry L. Biehl			Date: 01/30/1989
//	Revised By:			Larry L. Biehl			Date: 07/22/2015

void SetDLogControlHilite (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber, 
				SInt16								setting)

{		
   #if defined multispec_wx
		if (itemNumber == 1)
			itemNumber = wxID_OK;
		wxWindow* dialogItemWindowPtr = dialogPtr->FindWindow (itemNumber);

		if (dialogItemWindowPtr != NULL)
			{
			if (setting == 0)
				dialogItemWindowPtr->Enable (TRUE);

			else if (setting == 255)
				dialogItemWindowPtr->Enable (FALSE);

			}	// end "if (dialogItemWindowPtr != NULL)"
   #endif	// defined multispec_win

	#if defined multispec_mac
		Handle							theHandle;
		Rect								theBox;
		
		short int						theType;
	 
	
		GetDialogItem (dialogPtr, itemNumber,  &theType, &theHandle, &theBox);
		if (theType & ctrlItem)
			HiliteControl ((ControlHandle)theHandle, setting);
	#endif // defined multispec_mac
	
	#if defined multispec_win   
	   CWnd* dialogItemWindowPtr = dialogPtr->GetDlgItem (itemNumber);
	   
	   if (dialogItemWindowPtr != NULL)
	   	{ 
	   	if (setting == 0)
				dialogItemWindowPtr->EnableWindow (TRUE);
		
			else if (setting == 255)
				dialogItemWindowPtr->EnableWindow (FALSE);
				
			}	// end "if (dialogItemWindowPtr != NULL)"
	#endif	// defined multispec_win 	
			
}	// end "SetDLogControlHilite"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetDLogControlTitle
//
//	Software purpose:	The purpose of this routine is to set the text in the dialog
//							control item.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			Many dialogs
//
//	Coded By:			Larry L. Biehl			Date: 09/04/1997
//	Revised By:			Larry L. Biehl			Date: 03/20/2017

void SetDLogControlTitle (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber, 
				CharPtr 								theStringPtr,
				UInt16								inputStringFormatCode)

{		
	#if defined multispec_wx
	   if (dialogPtr != NULL)
			{
			wxCheckBox* checkbox = (wxCheckBox*)dialogPtr->FindWindow (itemNumber);
         wxString mystring (&theStringPtr[1], wxConvUTF8);
         checkbox->SetLabel (mystring);
			}       
	#endif
												
	#if defined multispec_mac
		Handle							theHandle;
		Rect								theBox;
		
		SInt16							theType;
	 
	
		GetDialogItem (dialogPtr, itemNumber,  &theType, &theHandle, &theBox);
		if (theType & ctrlItem)
			SetControlTitle ((ControlHandle)theHandle, (UCharPtr)theStringPtr);
	#endif // defined multispec_mac
	
	#if defined multispec_win  
		USES_CONVERSION;
		   
	   if (dialogPtr != NULL)
			{
			if (inputStringFormatCode == kASCIICharString)
				dialogPtr->SetDlgItemText (itemNumber, (LPCTSTR)A2T(&theStringPtr[1]));
			else if (inputStringFormatCode == kUTF8CharString)
				dialogPtr->SetDlgItemText (itemNumber, (LPCTSTR)A2T(&theStringPtr[1]));
			else	// inputStringFormatCode != kUnicodeCharString
				{
				wchar_t*	theUnicodeStringPtr = (wchar_t*)theStringPtr;
				dialogPtr->SetDlgItemText (itemNumber, &theUnicodeStringPtr[1]);

				}	// end "else inputStringFormatCode != kUnicodeCharString"

			}	// end "if (dialogPtr != NULL)"

	#endif	// defined multispec_win 			

}	// end "SetDLogControlTitle"
	                                

                     
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetDLogControlTitleNumber
//
//	Software purpose:	The purpose of this routine is to load the specified string 
//							number from the resource file into the requested dialog item 
//							number.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/21/1998
//	Revised By:			Larry L. Biehl			Date: 09/05/2017

void SetDLogControlTitleNumber (
				SInt16								strListID, 
				SInt16								stringNumber, 
				DialogPtr							dialogPtr, 
				SInt16								itemNumber, 
				Str255* 								stringPtr)

{
	if (dialogPtr != NULL)
		{
		if (MGetString ((UCharPtr)stringPtr, strListID, stringNumber))
			SetDLogControlTitle (dialogPtr, 
										itemNumber, 
										(CharPtr)stringPtr, 
										kASCIICharString);
			
		}	// end "if (dialogPtr != NULL)" 
	
}	// end "SetDLogControlTitleNumber" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ShowHideDialogItem
//
//	Software purpose:	The purpose of this routine is to show
//							or hide the specifed dialog item dependant upon
//							the input flag.
//
//	Parameters in:		Status Dialog item set
//
//	Parameters out:	None
//
// Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/24/1993
//	Revised By:			Larry L. Biehl			Date: 04/08/1998	

void ShowHideDialogItem (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber, 
				Boolean								showFlag)
				
{
   #if defined multispec_wx
		wxWindow* dialogItemWindowPtr = dialogPtr->FindWindow (itemNumber);

		if (dialogItemWindowPtr != NULL) 
			{
			if (showFlag)
				dialogItemWindowPtr->Show (true);

			else	// !showFlag
				dialogItemWindowPtr->Show (false);

			}	// end "if (dialogItemWindowPtr != NULL)"
   #endif	// defined multispec_wx

	#if defined multispec_mac
		if (showFlag)	
			ShowDialogItem (dialogPtr, itemNumber);
			
		else	// !showFlag 		
			HideDialogItem (dialogPtr, itemNumber);
	#endif // defined multispec_mac
	
	#if defined multispec_win   
		CWnd* dialogItemWindowPtr = dialogPtr->GetDlgItem (itemNumber);
		
		if (dialogItemWindowPtr != NULL)
			{
			if (showFlag)	
				dialogItemWindowPtr->ShowWindow (SW_SHOW);
				
			else	// !showFlag 		
				dialogItemWindowPtr->ShowWindow (SW_HIDE);
				
			}	// end "if (dialogItemWindowPtr != NULL)"
	#endif	// defined multispec_win 
		
}	// end "ShowHideDialogItem"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ShowStatusDialogItemSet
//
//	Software purpose:	The purpose of this routine is to show
//							the specified status dialog box item set.
//
//	Parameters in:		Status Dialog item set
//
//	Parameters out:	None
//
// Value Returned:	None 			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/27/1992
//	Revised By:			Larry L. Biehl			Date: 01/30/2019

void ShowStatusDialogItemSet (
				SInt16								setNumber)
				
{
	if (gStatusDialogPtr == NULL)
																									return;
																							
	#if defined multispec_mac
		Handle							theHandle;
		Rect								theBox;	
		
		SInt16							endItem,
											startItem,
											theType;
		
		
		switch (setNumber)
			{
			case kStatusTitle1:	// Title1 
				startItem = 11;
				endItem = 11;
				break;
				
			case kStatusTitle2:	// Title2 
				startItem = 21;
				endItem = 22;
				break;
				
			case kStatusTitle3:	// Title3 
				GetDialogItem (gStatusDialogPtr, 2, &theType, &theHandle, &theBox);
				theBox.right = theBox.left + 85;
				SetDialogItem (gStatusDialogPtr, 2, theType, theHandle, &theBox);
				
				GetDialogItem (gStatusDialogPtr, 3, &theType, &theHandle, &theBox);
				theBox.left += 47;
				theBox.right = theBox.left + 84;
				SetDialogItem (gStatusDialogPtr, 3, theType, theHandle, &theBox);
				
				startItem = 2;
				endItem = 3;
				break;
				
			case kStatusClassA:	// ClassA 
			case kStatusClassB:	// ClassB 
				startItem = 2;
				endItem = 5;
				
				if (setNumber == kStatusClassB)
					break;
				ShowDialogItem (gStatusDialogPtr, 6);
				ShowDialogItem (gStatusDialogPtr, 12);
				break;
				
			case kStatusField:	// Field 
				startItem = 7;
				endItem = 10;
				break;
				
			case kStatusLine:	// Line 
				startItem = 17;
				endItem = 20;
				break;
				
			case kStatusCluster:	// Number Clusters 
				startItem = 15;
				endItem = 16;
				break;
				
			case kStatusMinutes:	// Minutes Until Completion 
				startItem = 13;
				endItem = 14;
				break;
				
			case kStatusCommand:	// Command-. 
				startItem = 1;
				endItem = 1;
				break;
				
			}	// end "switch (setNumber)" 
			
		ShowDialogItems (gStatusDialogPtr, startItem, endItem);
	#endif	// defined multispec_mac
	
   #if defined multispec_win || defined multispec_wx
		switch (setNumber)
			{
			case kStatusTitle1:	// Title1 
	   		gStatusDialogPtr->MShowDialogItem (gStatusDialogPtr, IDC_Status11);
				break;
				
			case kStatusTitle2:	// Title2 
	   		gStatusDialogPtr->MShowDialogItem (gStatusDialogPtr, IDC_Status21);
	   		gStatusDialogPtr->MShowDialogItem (gStatusDialogPtr, IDC_Status22);
				break;
				
			case kStatusTitle3:	// Title3 
	   		gStatusDialogPtr->MShowDialogItem (gStatusDialogPtr, IDC_Status2);
	   		gStatusDialogPtr->MShowDialogItem (gStatusDialogPtr, IDC_Status3);
				break;
				
			case kStatusClassA:	// ClassA
			case kStatusClassB:	// ClassB  
	   		gStatusDialogPtr->MShowDialogItem (gStatusDialogPtr, IDC_Status2);
	   		gStatusDialogPtr->MShowDialogItem (gStatusDialogPtr, IDC_Status3);
	   		gStatusDialogPtr->MShowDialogItem (gStatusDialogPtr, IDC_Status4);
	   		gStatusDialogPtr->MShowDialogItem (gStatusDialogPtr, IDC_Status5);
				if (setNumber == kStatusClassB)
					break;
					
	   		gStatusDialogPtr->MShowDialogItem (gStatusDialogPtr, IDC_Status6);
	   		gStatusDialogPtr->MShowDialogItem (gStatusDialogPtr, IDC_Status12);
				break;
				
			case kStatusField:	// Field 
	   		gStatusDialogPtr->MShowDialogItem (gStatusDialogPtr, IDC_Status7);
	   		gStatusDialogPtr->MShowDialogItem (gStatusDialogPtr, IDC_Status8);
	   		gStatusDialogPtr->MShowDialogItem (gStatusDialogPtr, IDC_Status9);
	   		gStatusDialogPtr->MShowDialogItem (gStatusDialogPtr, IDC_Status10);
				break;
				
			case kStatusLine:	// Line 
	   		gStatusDialogPtr->MShowDialogItem (gStatusDialogPtr, IDC_Status17);
	   		gStatusDialogPtr->MShowDialogItem (gStatusDialogPtr, IDC_Status18);
	   		gStatusDialogPtr->MShowDialogItem (gStatusDialogPtr, IDC_Status19);
	   		gStatusDialogPtr->MShowDialogItem (gStatusDialogPtr, IDC_Status20);
				break;
				
			case kStatusCluster:	// Number Clusters 
	   		gStatusDialogPtr->MShowDialogItem (gStatusDialogPtr, IDC_Status15);
	   		gStatusDialogPtr->MShowDialogItem (gStatusDialogPtr, IDC_Status16);
				break;
				
			case kStatusMinutes:	// Minutes Until Completion 
	   		gStatusDialogPtr->MShowDialogItem (gStatusDialogPtr, IDC_Status13);
	   		gStatusDialogPtr->MShowDialogItem (gStatusDialogPtr, IDC_Status14);
				break;
				
			case kStatusCommand:	// Command-.
				#if defined multispec_win
					gStatusDialogPtr->MShowDialogItem (gStatusDialogPtr, IDCANCEL);
				#endif
				#if defined multispec_wx
	   			gStatusDialogPtr->MShowDialogItem (gStatusDialogPtr, IDC_CancelOperation);
				#endif
				break;               
				
			}	// end "switch (setNumber)"
			
		#if defined multispec_wx
			gStatusDialogPtr->Fit ();
		#endif
		
		#if defined multispec_win
			gStatusDialogPtr->UpdateWindow ();
		#endif
	#endif	// defined multispec_win || defined multispec_wx
	
}	// end "ShowStatusDialogItemSet" 



//-----------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 UpdateDialogFeatureParameters
//
//	Software purpose:	The purpose of this routine is to upate the feature/
//							transformed feature parameters in a dialog so that they
//							are consistant with the feature transformation flag. This
//							routine is called after the transformation flag has been
//							changed.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 05/04/1998
//	Revised By:			Larry L. Biehl			Date: 05/05/1998

SInt16 UpdateDialogFeatureParameters (
				Boolean								featureTransformationFlag, 
				UInt16*								localActiveNumberFeaturesPtr,
				UInt16**								localActiveFeaturePtrPtr, 
				SInt16								localNumberFeatures,
				UInt16*								localFeaturesPtr,
				UInt16								maxNumberFeatures, 
				SInt16								localNumberTransformedFeatures,
				UInt16*								localTransformFeaturesPtr,
				UInt16								maxNumberTransformFeatures)

{  
	SInt16								channelSelection;
	
	
	if (featureTransformationFlag)
		{
		*localActiveNumberFeaturesPtr = localNumberTransformedFeatures;
		*localActiveFeaturePtrPtr = localTransformFeaturesPtr;
		
		maxNumberFeatures = maxNumberTransformFeatures;
		
		}	// end "if (featureTransformationFlag)"
								
	else	// !featureTransformationFlag
		{
		*localActiveNumberFeaturesPtr = localNumberFeatures;
		*localActiveFeaturePtrPtr = localFeaturesPtr; 
			
		}	// end "else !featureTransformationFlag" 
			
	channelSelection = kAllMenuItem;
	if (*localActiveNumberFeaturesPtr < maxNumberFeatures)
		channelSelection = kSubsetMenuItem; 
		
	return (channelSelection);

}	// end "UpdateDialogFeatureParameters" 



//-----------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void UpdateDialogNumberFeatureParameters
//
//	Software purpose:	The purpose of this routine is to upate the number of
//							feature/transformed feature parameters in a dialog.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 05/04/1998
//	Revised By:			Larry L. Biehl			Date: 05/04/1998

SInt16 UpdateDialogNumberFeatureParameters (
				Boolean								featureTransformationFlag, 
				UInt16*								localActiveNumberFeaturesPtr,
				UInt16*								localNumberFeaturesPtr,
				UInt16								maxNumberFeatures, 
				UInt16*								localNumberTransformFeaturesPtr,
				UInt16								maxNumberTransformFeatures,
				SInt16								channelSelection)

{  
			// Update the number of features for the all feature selection in case
			// it was selected using the popup menu.				

	if (channelSelection == kAllMenuItem)
		{
		if (featureTransformationFlag)
			*localActiveNumberFeaturesPtr = maxNumberTransformFeatures;
								
		else	// !featureTransformationFlag
			*localActiveNumberFeaturesPtr = maxNumberFeatures;
								
		}	// end "if (channelSelection == kAllMenuItem)"
	
			// Now save the number of features or transform features in case needed
			// for later use.
							
	if (featureTransformationFlag)
		{
		*localNumberTransformFeaturesPtr = *localActiveNumberFeaturesPtr;
		
		maxNumberFeatures = maxNumberTransformFeatures;
			
		}	// end "if (featureTransformationFlag)"
								
	else	// !featureTransformationFlag
		*localNumberFeaturesPtr = *localActiveNumberFeaturesPtr;
			
	channelSelection = kAllMenuItem;
	if (*localActiveNumberFeaturesPtr < maxNumberFeatures)
		channelSelection = kSubsetMenuItem; 
		
	return (channelSelection);

}	// end "UpdateDialogNumberFeatureParameters" 

