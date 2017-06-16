//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2017)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						dialogUtilities.c
//
//	Authors:					Larry L. Biehl, Ravi Budruk
//
//	Revision number:		3.5
//
//	Revision date:			03/25/2017
//
//	Language:				C
//
//	System:					Macintosh Operating System
//
//	Brief description:	The purpose of the routines in this file is to
//								provide utility type functions in MultiSpec.
//
//	Functions in file:	void						ChangeDLogCheckBox
//								Boolean 					ChannelsDialog
//								void 						CheckEndLCValue
//								Boolean 					CheckKeyDownDialogFilter
//								SInt16 					CheckLineColValues
//								void 						CheckStartValue
//								DialogPtr 				CloseRequestedDialog
//								pascal void 			CreateOneColumnList1
//								pascal void 			CreateOneColumnList2
//								SInt16 	 				DialogKeyFilter
//								void 						DialogLineColumnHits
//								pascal Boolean 		DialogListFilter
//								pascal void 			DrawGraphicsBox
//								void 						FlashDLogItem
//								double 					GetDItemRealValue
//								int 						GetDLogControl
//								void						GetDlogOrigin
//								void						HideDialogItems
//								pascal void 			HiliteOKButton
//								DialogPtr 				LoadRequestedDialog
//								Boolean 					ModalOneColListDialog
//								void 						NumberDefault
//								pascal Boolean 		ProcessorDialogFilter
//								void 						RealNumberErrorAlert
//								void 						SetDialogItemDrawRoutine
//								pascal void 			SetEntireImageIcon
//								void						ShowDialogItems
//								void						ShowDialogWindow
//								void						ShowHideDialogItems
//								void 						ShowStatusDialogItemSet
//								void						ShowStatusDialogWindow
//								void 						UpdateDialogOneColumnList
//								void 						UpdateLineColumnValues
//								short int 				UpdateOneColListSelections
//
                
//#include "SMulSpec.h"  

#if defined multispec_win
                        
	#include "CImagWin.h"
	#include "WStatDlg.h"
	 
#endif	// defined multispec_win

//#include "SExtGlob.h"


			// Prototypes for routines in this file that are only called by		
			// other routines in this file.												

void 						UpdateDialogOneColumnList (
								DialogPtr							dialogPtr, 
								SInt16								itemNumber,
								ListHandle							listHandle);
										
SInt16 					UpdateOneColListSelections (
								UInt16								numberInputVecItems, 
								SInt16								listStart, 
								SInt16								listEnd, 
								SInt16								listInterval);


//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ChangeDLogCheckBox
//
//	Software purpose:	The purpose of this routine is to change the 
//							setting of the check box control in a dialog window 
//							to the opposite of what it's present setting is.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			StatisticsDialog   in statistics.c
//
//	Coded By:			Larry L. Biehl			Date: 01/29/1989
//	Revised By:			Larry L. Biehl			Date: 11/03/2005

void ChangeDLogCheckBox (
				DialogPtr							dialogPtr,
				SInt16								itemNumber)

{
	Rect									theBox;
	ControlHandle						theHandle;
	SInt16								theType;
	
	
	GetDialogItem (dialogPtr, itemNumber, &theType, (Handle*)&theHandle, &theBox);
	
	ChangeDLogCheckBox (theHandle);
/*
	if (theHandle != NULL)
		{
		if ( GetControlValue (theHandle) )
			SetControlValue (theHandle, 0);
									
		else
			SetControlValue (theHandle, 1);
			
		}		// end "if (theHandle != NULL)" 
*/												
}		// end "ChangeDLogCheckBox" 



void ChangeDLogCheckBox (
				ControlHandle						theHandle)

{
	if (theHandle != NULL)
		{
		if ( GetControlValue (theHandle) )
			SetControlValue (theHandle, 0);
									
		else
			SetControlValue (theHandle, 1);
			
		}		// end "if (theHandle != NULL)" 
												
}		// end "ChangeDLogCheckBox" 


                       
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ChannelsDialog
//
//	Software purpose:	The purpose of this routine is to display a dialog
//							with a list of the available channels that the user
//							can select from.
//							listType=1:  user can select channels in the list.
//							listType=2:  a list of channel descriptions is shown.
//								No selections can be made and no channels are
//								hilighted.
//							listType=3:  a list of the selected channels is shown.
//								No selections can be made.  The selected channels
//								are hilighted.
//							listType=4:  user can select Principal Component
//								channels in the list.  There will be no channel
//								descriptions.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			ClassifyDialog   in classify.c
//
//	Coded By:			Larry L. Biehl			Date: 01/13/1989
//	Revised By:			Larry L. Biehl			Date: 01/21/2006

Boolean ChannelsDialog (
				SInt16*								numberOutputFeaturesPtr, 
				SInt16* 								selectedFeaturePtr, 
				LayerInfoPtr						layerInfoPtr,
				FileInfoPtr							fileInfoPtr, 
				SInt16								listType, 
				Boolean								useTransformFlag, 
				SInt16* 								availableFeaturePtr, 
				SInt16								numberInputChannels,
				SInt16								currentSelection)

{                 
	DialogPtr							dialogPtr;
	
	SInt32								numberSelectedChannels;
	
	SInt16								channel,
											itemHit;

	Boolean								modalDone,
											returnFlag = FALSE;

	
	if (useTransformFlag)
		{
		numberInputChannels = gTransformationMatrix.numberFeatures;
		HLock (gTransformationMatrix.eigenFeatureHandle);
		availableFeaturePtr = NULL;
		fileInfoPtr = NULL;
		
		}		// end "if (useTransformFlag)"

	if (listType != kItemsListOnly && listType != kSelectedItemsListOnly)
		LoadSubsetList (numberOutputFeaturesPtr, 
								selectedFeaturePtr,
								availableFeaturePtr, 
								numberInputChannels,
								&currentSelection);
	
			// Get the modal dialog for the reformat specification					
				
	dialogPtr = LoadRequestedDialog (kChannelSpecificationID, kCopyScrap, 1, 2);
	if (dialogPtr == NULL)													
																						return(FALSE);
		
			// Get a handle to a one column list.											
			
	gDialogListHandle = GetOneColumnDialogListHandle (dialogPtr, 5);
	
			//	Create the channel list.														
			
	SetDialogItemDrawRoutine (dialogPtr, 5, gCreateOneColumnList1Ptr);
	
			// Initialize the channel range text edit items.							
			
	LoadDItemValue (dialogPtr, 9, (SInt32)1);
	LoadDItemValue (dialogPtr, 11, (SInt32)numberInputChannels);
	LoadDItemValue (dialogPtr, 13, (SInt32)1);
	SetDLogControlHilite (dialogPtr, 14, 0);
	
			// If this is just a display list, hide all but OK button and list.	
			// Put "Channel Descriptions" or "Selected Channels" as title of 		
			// the list.																			
			
	if (listType == kItemsListOnly || listType == kSelectedItemsListOnly)
		{
		HideDialogItems (dialogPtr, 2, 4);
		HideDialogItems (dialogPtr, 7, 14);
		
		if (listType == kItemsListOnly)
			SetWTitle ( GetDialogWindow(dialogPtr), "\pChannel Descriptions" );
			
		if (listType == kSelectedItemsListOnly)
			SetWTitle ( GetDialogWindow(dialogPtr), "\pSelected Channels" );
		
		}		// end "if (listType == kItemsListOnly || ...)" 
		
	if (listType == kItemsListOnly)
		HideDialogItems (dialogPtr, 15, 16);
			
	if (useTransformFlag)
		{
		SetWTitle ( GetDialogWindow(dialogPtr), "\pSelect Transformation Features" );
		LoadDItemString (dialogPtr, 7, (Str255*)"\pFeature List");
		
		}		// end "if (useTransformFlag)" 
			
			// Show the dialog.																	
			
	ShowDialogWindow (dialogPtr, kChannelSpecificationID, kSetUpDFilterTable);
	
			// Continue if there is a valid dialog list handle.						
			
	returnFlag = FALSE;
	if (gDialogListHandle != NULL)
		{
		numberSelectedChannels = AddChannelsToDialogList (
														gDialogListHandle,
														numberOutputFeaturesPtr, 
														selectedFeaturePtr, 
														layerInfoPtr,
														fileInfoPtr,
														listType, 
														availableFeaturePtr, 
														numberInputChannels,
														currentSelection);
														
		LUpdate ( GetPortVisibleRegion (
							GetDialogPort (dialogPtr), gTempRegion1), gDialogListHandle );
		SetEmptyRgn (gTempRegion1);
		
	
				// Set the number of selected items.									
				
		LoadDItemValue (dialogPtr, 16, (SInt32)numberSelectedChannels);
			
				// Set default text selection to first edit text item					
				
		SelectDialogItemText (dialogPtr, 9, 0, INT16_MAX);
		
				// Handle modal dialog selections for one column channel list and	
				// type == 1 for user selections.											
			
		if ( (listType == kSelectItemsList || listType == kSelectPCList) && 
																			gMemoryError == noErr)
			{
			UInt32 numberFeatures = (UInt32)*numberOutputFeaturesPtr;
			returnFlag = ModalOneColListDialog (dialogPtr, 
															numberInputChannels,
															&numberFeatures, 
															selectedFeaturePtr, 
															1);
															
			*numberOutputFeaturesPtr = (SInt16)numberFeatures;
												
					// Convert selectedFeature vector to point to the channel 		
					// numbers that are in available feature vector.					
			
			if (returnFlag)
				{	
				for (channel=0; channel<*numberOutputFeaturesPtr; channel++)
					selectedFeaturePtr[channel]--;

				SaveSubsetList (*numberOutputFeaturesPtr, 
										selectedFeaturePtr,
										availableFeaturePtr, 
										numberInputChannels);
					
				}		// end "if (returnFlag)"
				
			}		// end "if (listType == kSelectChannelsList || ...)" 
		
				// Handle modal dialog selections for one column channel list and	
				// type == 2 for presentation of channel descriptions.  No 			
				// selections will be made.													
															
		if ( (listType == kItemsListOnly || 
							listType == kSelectedItemsListOnly) && 
																		gMemoryError == noErr)	
			{
			modalDone = FALSE;
			itemHit = 0;
			do 
				{
		   	ModalDialog (gProcessorDialogFilterPtr, &itemHit);
		   
				if	(itemHit == 1)      // User selected OK for information 
					modalDone = TRUE;
					
				} while (!modalDone);
				
			returnFlag = TRUE;
				
			}		// end "if (listType >= 2 && ...)" 
			
				// Make certain that global memory error is set back to 'noErr'.	
				
		gMemoryError = noErr;
		
				// Dispose of the dialog list handle.										
				
		if (gDialogListHandle != NULL)  
			LDispose(gDialogListHandle);
		gDialogListHandle = NULL;
		
		}		// end "if (gDialogListHandle != NULL)" 
		
	CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
		
	CheckAndUnlockHandle (gTransformationMatrix.eigenFeatureHandle);
	
	return (returnFlag);
	
}		// end "ChannelsDialog"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CheckEndLCValue
//
//	Software purpose:	The purpose of this routine is to check the end
//							line or column value to see if it makes sense.
//
//	Parameters in:					
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/07/1989
//	Revised By:			Larry L. Biehl			Date: 01/30/1998	

void CheckEndLCValue (
				SInt32								newValue, 
				SInt32*								endLineCol, 
				SInt32								maxLineCol,
				DialogPtr							dialogPtr, 
				Handle								theHandle, 
				SInt16								itemHit)

{
	if (newValue == 0)
		NumberErrorAlert ( (SInt32)*endLineCol, dialogPtr, itemHit);
		
	else
		if (newValue > maxLineCol)
			{
		 	*endLineCol = maxLineCol;
			NumberDefault (theHandle, (SInt32)maxLineCol, &gTextString);
		 	
		 	}		// end "if (newValue > maxLineCol)" 
		 				
		else		// newValue <= maxLineCol 
		 	*endLineCol = newValue;
						 		
}		// end "CheckEndLCValue"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CheckKeyDownDialogFilter
//
//	Software purpose:	The purpose of this routine is to handle the 
//							processing for key down dialog control hits in a dialog
//							that may have integer or real, positive or negative
//							values.  A structure is accessed which describes
//							the characteristics of items in the calling dialog
//		
//	Parameters in:		Pointer to dialog window
//							Address of event record
//
//	Parameters out:	Item hit
//
// Value Returned:	TRUE if routine handled item hit.
//							FALSE if routine did not handle item hit.
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/04/1994
//	Revised By:			Larry L. Biehl			Date: 03/26/2006

Boolean CheckKeyDownDialogFilter (
				DialogPtr							theDialog, 
				EventRecord*						theEventPtr, 
				SInt16*								itemHitPtr)

{
	Rect									theBox;
								
	Handle								theHandle;
	TEHandle								textH;
	
	SInt16								decimalLocation,
											decimalMaximum,
											digitMaximum,
											i,
											selEnd,
											selStart,
											stringLength,
											theType,
											theValue;
	
	Boolean								minusFlag,
											minusItemFlag,
											numberFlag,
											periodFlag,
											realItemFlag,
											sendAlertFlag,
											stringItemFlag;
								
			
   theValue = (theEventPtr->message & charCodeMask);
   
   		// Check for default cases.														
   
	theType = DialogKeyFilter (theDialog, theEventPtr, itemHitPtr, theValue);
	
	if (theType == 1)
																			return (TRUE);
	
	if (theType == 0)
																			return (FALSE);

			// Get the item number of the edit field										
			
//	*itemHitPtr = ((DialogPeek)theDialog)->editField + 1;
	*itemHitPtr = GetDialogKeyboardFocusItem (theDialog);
		
	GetDialogItem (theDialog, *itemHitPtr, &theType, &theHandle, &theBox);
	textH = GetDialogTextEditHandle (theDialog);	
	
			// Check for invalid numbers for edittext fields if the					
			// type is 16																			
				
	if (theType == 16)
		{
		sendAlertFlag = FALSE;
		
				// Check if character is a number, period or minus.					
		
		numberFlag = (theValue >= 0x30 && theValue <= 0x39);
		periodFlag = (theValue == 0x2E);
		minusFlag = (theValue == 0x2D);
		
		realItemFlag = kDItemReal & gDialogItemDescriptorPtr[*itemHitPtr];
		minusItemFlag = kDItemMinus & gDialogItemDescriptorPtr[*itemHitPtr];
		stringItemFlag = kDItemString & gDialogItemDescriptorPtr[*itemHitPtr];
		
		if (stringItemFlag)
			{
			Boolean stringItemType1Flag = 
							kDItemSType1 & gDialogItemDescriptorPtr[*itemHitPtr];
		
			if (stringItemType1Flag)
				{
						// Make sure only a period, plus, ??, ??, minus and numerical
						// digit is used.
						
				if ( !periodFlag && (theValue != 0x43) && (theValue != 0x63) && 
								(theValue != 0x2B) && !minusFlag && !numberFlag && (theValue != '*'))
					sendAlertFlag = TRUE;
			
				}		// end "if (stringItemType1Flag)" 
							
			Boolean stringItemType2Flag = 
							kDItemSType2 & gDialogItemDescriptorPtr[*itemHitPtr];
		
			if (stringItemType2Flag && !sendAlertFlag)
				{
						// Make sure that a blank is not placed at the beginning
						// of the string.
						
				if (theValue == ' ' && ((TEPtr)*textH)->selStart == 0)
					sendAlertFlag = TRUE;
			
				}		// end "if (stringItemType2Flag)" 
				
			if (!sendAlertFlag)
																				return (FALSE);
				
			}		// end "if (stringItemFlag)" 
		
		if (!stringItemFlag)
			{
					// Check for character error in numerical fields.					
					
			if ( !periodFlag && !numberFlag && !minusFlag)
				sendAlertFlag = TRUE;
			
			else		// periodFlag || numberFlag || minusFlag 
				{
				if ( minusFlag && !minusItemFlag )
					sendAlertFlag = TRUE;
				
				else		// !minusFlag || minusItemFlag 
					{	
					if ( periodFlag && !realItemFlag )
						sendAlertFlag = TRUE;
						
					}		// end "else !minusFlag || minusItemFlag" 
					
				}		// end "else periodFlag || numberFlag || minusFlag" 
				
			}		// end "else !stringItemFlag" 
			
				// Continue check for real number field if okay to here.				
			
		if ( !sendAlertFlag && !stringItemFlag)
			{
			if (numberFlag && !realItemFlag && !minusItemFlag)
																				return (FALSE);
			
					// Determine where the selection point is in the string.	
				
			selStart = ((TEPtr)*textH)->selStart + 1;
			selEnd = ((TEPtr)*textH)->selEnd + 1;
			
					// Get the string.															
					
			GetDialogItemText (theHandle, gTextString);
			
					// Determine if minus sign is the first character.					
					
			if (minusFlag && minusItemFlag )
				{
						// Only allow minus at the beginning of the string.			
					
				if (selStart > 1 && minusFlag)
					sendAlertFlag = TRUE;
					
				}		// end "if (minusFlag && minusItemFlag" 
			
					// Check for proper location of decimal if real number field.	
				
			if ( realItemFlag && !sendAlertFlag )
				{
						// Initialize variables.												
					
				stringLength = gTextString[0];
				decimalLocation = 256;
				decimalMaximum = 20;
				digitMaximum = 21;
			
						// Find if entire string is not selected.							
					
				if ( selEnd-selStart != stringLength )
					{
							// Get decimal point location.									
				
					i = 1;
					while (i<=stringLength && decimalLocation == 256)
						{
						if ( (i<selStart || i>=selEnd) && gTextString[i] == 0x2E)
							decimalLocation = i;
						
						i++;
						
						}		// end "while (i<=stringLength && ..." 
					
							// Limit the number of decimal places to 						
							// decimalMaximum.													
						
					if ( decimalLocation != 256 && !periodFlag)
						if (selStart > decimalLocation+decimalMaximum)
							sendAlertFlag = TRUE;
					
							// Do not allow a 2nd decimal point.							
						
					if ( decimalLocation != 256 && periodFlag)
						sendAlertFlag = TRUE;
				
					else		// decimalLocation == 256 || !periodFlag 
						{
								// Do not allow more than maximum total digits.			
					
						if ( (stringLength - selEnd + selStart) >= digitMaximum )
							sendAlertFlag = TRUE;
						
						}		// else decimalLocation == 256 || !periodFlag 
						
					}		// end "if ( selEnd-selStart != stringLength )" 
			
				}		// end "if ( realItemFlag && !sendAlertFlag )" 
			
			}		// end "!sendAlertFlag && !string2ItemFlag" 
			
		if (sendAlertFlag)
			{
			Alert (kErrorAlert2ID, NIL);
			return(TRUE);
			
			}		// end "if (sendAlertFlag)" 
			
		}		// end "if (theType == 16)" 
		
	return (FALSE);
	
}		// end "CheckKeyDownDialogFilter" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CheckStartValue
//
//	Software purpose:	The purpose of this routine is to check the start
//							input value to see if it makes sense relative to the
//							input min and max values. If it does not an error condition
//							is made.
//
//	Parameters in:					
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/01/1999
//	Revised By:			Larry L. Biehl			Date: 02/01/1999	

SInt32 CheckIntervalValue (
				SInt32								newValue, 
				SInt32								currentInterval, 
				SInt32								minimumValue,  
				SInt32								maximumValue, 
				DialogPtr							dialogPtr, 
				SInt16								itemHit,
				Handle								theHandle)
				
{ 
	SInt32 newInterval = currentInterval;
		
	if (newValue < minimumValue)
		NumberErrorAlert ( 
		 			currentInterval, 
					dialogPtr, 
					itemHit);
	
	else if (newValue > maximumValue)
		NumberDefault (
					theHandle, 
		 			currentInterval, 
		 			&gTextString);
		 			
	else		// newValue >= minimumValue && newValue <= maximumValue 
		newInterval = newValue;
				 
	return (newInterval);
	
}		// end "CheckIntervalValue" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 CheckLineColValues
//
//	Software purpose:	The purpose of this routine is to check the
//							line or column values after the user has selected
//							OK in a dialog box to see if they make sense.  If
//							the values do make sense, a value of '1' is returned.
//							If the values do not make sense, an alert is sounded,
//							the last changed value is hilited, and a '0' is 
//							returned.
//							NOTE:  The order of the dialog items for the start
//							line, end line, line interval, start column, end
//							column, and column interval is expected to be as
//							listed.
//
//	Parameters in:					
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			BiPlotDataDialog in biPlotData.c
//							DisplayMultispectralDialog in displayMultispectral.c
//							LoadAreasToThematicDialog in fieldsToThematicFile.c
//							HistogramDialog in histogram.c
//							MosaicImagesSideBySideDialog in mosaic.cpp
//							RectifyImageDialog in rectification.c
//							CEMClassifyDialog in SClassfy.cpp
//							ClassifyDialog in SClassfy.cpp
//							ClusterDialog in SCluster.cpp
//							ISODATAClusterDialog in SClustID.cpp
//							OnePassClusterDialog in SClustSP.cpp
//							DisplayThematicDialog in SDisThem.cpp
//							ListDataDialog in SLstData.cpp
//							ListResultsDialog in SLstRslt.cpp
//							PrincipalComponentDialog in SPrinCom.cpp
//							ChangeImageFormatDialog in SReform1.cpp
//							StatisticsEnhanceDialog in statisticsEnhancement.c
//							StatisticsImageDialog in statisticsImage.c
//
//	Coded By:			Larry L. Biehl			Date: 03/07/1989
//	Revised By:			Larry L. Biehl			Date: 11/10/1999

SInt16 CheckLineColValues (
				DialogSelectAreaPtr				newSelectAreaPtr,
				SInt32								oldStartLine,
				SInt32								oldStartCol,
				DialogPtr							dialogPtr)

{
	SInt16								startItem;
	

	startItem = newSelectAreaPtr->baseItem;
	
			
			// Check if start start line is greater than end line						
			
	if (newSelectAreaPtr->lineStart > newSelectAreaPtr->lineEnd)
		{
		if (newSelectAreaPtr->lineStart != oldStartLine)
			SelectDialogItemText (dialogPtr, startItem, 0, SHRT_MAX);
									
		else		// newSelectAreaPtr->lineStart == oldStartLine
			SelectDialogItemText (dialogPtr, startItem+1, 0, SHRT_MAX); 
									
		DisplayAlert (kErrorAlertID, 
							kStopAlert, 
							kAlertStrID, 
							IDS_Alert68,
							0,
							NULL);
					
				// Force the main dialog box to be redrawn.
				
		InvalWindowRect (GetDialogWindow(dialogPtr), 
					GetPortBounds(GetDialogPort(dialogPtr), &gTempRect));
							
																						return (0);
		
		}		// end "if (new...->lineStart > new...->lineEnd)" 
		
	if (newSelectAreaPtr->columnStart > newSelectAreaPtr->columnEnd)
		{
		if (newSelectAreaPtr->columnStart != oldStartCol)
			SelectDialogItemText (dialogPtr, startItem+3, 0, SHRT_MAX);
			
		else		// newSelectAreaPtr->columnStart == oldStartCol 
			SelectDialogItemText (dialogPtr, startItem+4, 0, SHRT_MAX);
									
		DisplayAlert (kErrorAlertID, 
							kStopAlert, 
							kAlertStrID, 
							IDS_Alert69,
							0,
							NULL);
					
				// Force the main dialog box to be redrawn.
				
		InvalWindowRect (GetDialogWindow(dialogPtr), 
					GetPortBounds(GetDialogPort(dialogPtr), &gTempRect));
							
																						return (0);
		
		}		// end "if (new...->columnStart > new...->columnEnd)" 
	
	return (1);
	
}		// end "CheckLineColValues"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 CheckMaxRealValue
//
//	Software purpose:	The purpose of this routine is to check the input dialog item
//							real value and verify that it is not 0 and not more than the maximum
//							value allowed.
//
//	Parameters in:					
//
//	Parameters out:	None
//
// Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/22/2005
//	Revised By:			Larry L. Biehl			Date: 12/22/2005

SInt16 CheckMaxRealValue (
				DialogPtr							dialogPtr,
				SInt16								itemNumber,
				double								maxValue,
				Boolean								zeroAllowedFlag,
				Boolean								showAlertMessageFlag)
										
{
	double								minValue,
											theNum;
											
	SInt16								itemHit = 1;
	Boolean								errorFlag = FALSE;


	theNum = GetDItemRealValue (dialogPtr, itemNumber);
	
	if (theNum > maxValue)
		errorFlag = TRUE;
		
	else if (!zeroAllowedFlag && theNum == 0)
		errorFlag = TRUE;
		
	if (errorFlag)
		{
		if (showAlertMessageFlag)
			{
			minValue = 1;
			if (zeroAllowedFlag)
				minValue = 0;
				
			SetDLogControlHilite (dialogPtr, 1, 255);
												
			if (LoadSpecifiedStringNumberLongP ( 
												kAlertStrID, 
												IDS_Alert91, 
												(char*)gTextString,
												(char*)gTextString2,
												TRUE,
												minValue,
												maxValue ) )
				{
				DisplayAlert (kErrorAlertID, kStopAlert, 0, 0, 0, gTextString);
					
						// Force the main dialog box to be redrawn.
						
				InvalWindowRect (GetDialogWindow(dialogPtr), 
							GetPortBounds(GetDialogPort(dialogPtr), &gTempRect));
							
				}		// end "if (LoadSpecifiedStringNumberLongP (..."
				
			SetDLogControlHilite (dialogPtr, 1, 0);
				
			SelectDialogItemText (dialogPtr, itemNumber, 0, SHRT_MAX);
								
			}		// end "if (showAlertMessageFlag)"
		
		else		// !showAlertMessageFlag	
			NumberErrorAlert (theNum, dialogPtr, itemNumber);
						
		itemHit = 0;
		
		}		// end "if (errorFlag)"
		
	return (itemHit);
					
}		// end "CheckMaxRealValue" 



SInt16 CheckRealValue (
				DialogPtr							dialogPtr,
				SInt16								itemNumber,
				double								minValue, 
				SInt16								decimalPlaces)
										
{
	double								theValue;
	SInt16								itemHit = 1;


	theValue = GetDItemRealValue (dialogPtr, itemNumber);
	if (theValue <= minValue)
		{
		RealNumberErrorAlert (theValue, dialogPtr, itemNumber, decimalPlaces);
		itemHit = 0;
		
		}		// end "if (theValue <= minValue)"
		
	return (itemHit);
					
}		// end "CheckRealValue" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CheckStartValue
//
//	Software purpose:	The purpose of this routine is to check the start
//							input value to see if it makes sense relative to the
//							input min and max values. If it does not an error condition
//							is made.
//
//	Parameters in:					
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/07/1989
//	Revised By:			Larry L. Biehl			Date: 01/30/1998	

void CheckStartValue (
				SInt32								newValue, 
				SInt32*								startValuePtr, 
				SInt32								minimumValue,  
				SInt32								maximumValue, 
				DialogPtr							dialogPtr, 
				SInt16								itemHit)
				
{
			
	if (newValue < minimumValue || newValue > maximumValue)
		NumberErrorAlert ( *startValuePtr, 
									dialogPtr,
									itemHit );
												
	else		// newValue >= minimumValue && newValue <= maximumValue 
		*startValuePtr = newValue;

}		// end "CheckStartValue"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CloseRequestedDialog
//
//	Software purpose:	The purpose of this routine is to close the input
//							dialog, copy any TE scrap to the desk scrap and
//							do any activate events that are in the event queue.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	DialogPtr 		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/09/1993
//	Revised By:			Larry L. Biehl			Date: 01/21/2006

void CloseRequestedDialog ( 	
				DialogPtr							dialogPtr,
				Boolean								setUpFilterTableFlag)

{
	HideWindow (GetDialogWindow(dialogPtr));
	DisposeDialog (dialogPtr);
	
	CheckSomeEvents (activMask);
	
	if (setUpFilterTableFlag)
		{
				// Indicate that the last dialog list table is to be used.		
		
		gDialogItemTableSet--;
	
		if (gDialogItemTableSet >= 0 && gDialogItemTableSet < 3)
			gDialogItemDescriptorPtr = &gDialogItemListTable[
									gDialogItemTableSet * kDialogItemTableLength];
										
		else		// gDialogItemTableSet < 0 || ... 
			gDialogItemDescriptorPtr = NULL;
			
		}		// end "if (setUpFilterTableFlag)" 
	
}		// end CloseRequestedDialog 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void CreateOneColumnList1
//
//	Software purpose:	The purpose of this routine is to create a one 
//							column list for list1 in a dialog window.  The routine will
//							be used to create a list for loading in a column
//							of channel numbers/descriptions and class names.
//
//	Functional description:	The steps in this function follow.
//
//			1) Get the location and area of the list.
//			2) Outline the area that the list will fall into.
//			3) Get a handle to the list information.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  None
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 01/13/1989
//	Revised By:			Larry L. Biehl			Date: 06/28/2003

pascal void CreateOneColumnList1 (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{
	UpdateDialogOneColumnList (dialogPtr, itemNumber, gDialogListHandle);

}		// end "CreateOneColumnList1" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void CreateOneColumnList2
//
//	Software purpose:	The purpose of this routine is to create a one 
//							column list for list1 in a dialog window.  The routine will
//							be used to create a list for loading in a column
//							of channel numbers/descriptions and class names.
//
//	Functional description:	The steps in this function follow.
//
//			1) Get the location and area of the list.
//			2) Outline the area that the list will fall into.
//			3) Get a handle to the list information.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  None
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 01/13/1989
//	Revised By:			Larry L. Biehl			Date: 06/28/26

pascal void CreateOneColumnList2 (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{
	UpdateDialogOneColumnList (dialogPtr, itemNumber, gDialogListHandle2);

}		// end "CreateOneColumnList2" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 DialogKeyFilter
//
//	Software purpose:	The purpose of this routine is to handle the default
//							key down cases for dialogs.  If the default cases do
//							not exist, then a flag is set (return = -1) so that the
//							calling routine can handle the case.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/08/1990
//	Revised By:			Larry L. Biehl			Date: 04/08/1994

SInt16 DialogKeyFilter (
				DialogPtr							dialogPtr, 
				EventRecord*						theEvent, 
				SInt16*								itemHitPtr, 
				SInt16								theValue)

{
	Rect									theBox;
	
	Handle								theHandle;
	
	SInt16								defaultItem,
											returnCode,
											theType;
	
	
			// Check for carriage return or enter for 'OK' exit.						

   if (theValue == 0x0d || theValue == 0x03)
   	{
   			// Make certain that the OK control is hilited.							
   	
   	defaultItem = GetDialogDefaultItem (dialogPtr);
		GetDialogItem (dialogPtr, defaultItem, &theType, &theHandle, &theBox);
		if ( GetControlHilite((ControlRef)theHandle) == 255 )
			*itemHitPtr = -1;
   			
   	else		// GetControlHilite(theHandle) != 255 
   		*itemHitPtr = 1;
   		
   	if (*itemHitPtr == 1)
   		FlashDLogItem (dialogPtr, defaultItem);
   		
   																				return (1);
   	
   	}		// end "if (theValue == 0x0d || theValue == 0x03)" 
	
			// Check for esc for 'Cancel' exit.												

   if (theValue == 0x1B)
   	{
	   *itemHitPtr = 2;
	   FlashDLogItem (dialogPtr, 2);
	   																			return (1);
   	
   	}		// end "if (theValue == 0x1B)" 
   
			// Allow system to handle backspace and tab keys.							

	if (theValue == 0x08 || theValue == 0x09)
																					return (0);
   	
   		// Handle 'command key'.															
   		
   if ( theEvent->modifiers & cmdKey )
   	{
   	returnCode = 0;
   	switch (theValue)
   		{
   		case '.': 		// command-. is equivalent to cancel.						
   			
		   	*itemHitPtr = 2;
		   	FlashDLogItem (dialogPtr, 2);
   			returnCode = 1;
   			break;
	   	
   		case 'x': 		// command-x is equivalent to cut.							
   		case 'X':
   			
				DialogCut ( dialogPtr );
   			returnCode = 1;
   			break;
	   	
   		case 'c': 		// command-c is equivalent to copy.							
   		case 'C':
   			
				DialogCopy ( dialogPtr );
   			returnCode = 1;
   			break;
	   	
   		case 'v': 		// command-v is equivalent to paste.						
   		case 'V':
   			
				DialogPaste ( dialogPtr );
   			returnCode = 1;
   			break;
	   	
   		case 'a': 		// command-a as equivalent to select all.					
   		case 'A':
   			
				SelectDialogItemText (dialogPtr, *itemHitPtr, 0, INT16_MAX);
   			returnCode = 1;
   			break;
	   	
	   	}		// end "switch (theValue)" 
	   	
	   		// All other commmand-key combinations do nothing.						
	   
	   if (returnCode == 0)	
	  		theEvent->what = nullEvent;
	  		
	  			// Handle the case where the item number is a -1 and editing of	
	  			// a text field takes place.  This occurs when the Edit command	
	  			// keys are used.																	
	  			
	  	if (returnCode == 1 && *itemHitPtr == -1)
			*itemHitPtr = GetDialogKeyboardFocusItem (dialogPtr);
	  		
	   																return (returnCode);
	   
	   }		// end "if ( theEvent->modifiers & cmdKey )" 
	   	
   		// Option-key combinations do nothing.											
   		// Leave this in to get special characters.									
   		
//	if ( theEvent->modifiers & optionKey )
//   	{
//  		theEvent->what = nullEvent;
//   																				return (0);
//   																			
//   	}		// end "if ( theEvent->modifiers & optionKey )" 
   
			// Ignore non-printing characters.												

	if (theValue < ' ')
   	{
  		theEvent->what = nullEvent;
   																				return (0);
   																			
   	}		// end "if (theValue < ' ')" 
																				
			// Let the calling routine handle the key down event.						
			
	return (-1);
				
}		// end "DialogKeyFilter"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DialogLineColumnHits
//
//	Software purpose:	The purpose of this routine is to handle the 
//							processing for dialog hits in line or column items.
//		
//	Parameters in:		Pointer do dialog window
//							Dialog item that was selected
//							Handle to the selected dialog item
//							Box for the selected dialog item
//							The current value in the selected dialog item
//
//	Parameters out:	None
//
// Value Returned:	None.
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 12/13/1994
//	Revised By:			Larry L. Biehl			Date: 03/19/2014

void DialogLineColumnHits (
				DialogSelectAreaPtr				dialogSelectAreaPtr,
				DialogPtr							dialogPtr, 
				SInt16								itemHit,
				Handle								theHandle,
				SInt32								theNum,
				Boolean								intervalEditBoxesExistFlag)

{
	WindowInfoPtr						imageWindowInfoPtr;
	
	SInt16								parameter;


	imageWindowInfoPtr = dialogSelectAreaPtr->imageWindowInfoPtr;
	parameter = itemHit - dialogSelectAreaPtr->baseItem + 1;
	
	if (!intervalEditBoxesExistFlag && parameter >= 3)
		parameter++;
	
	switch (parameter)
		{
		case 1:				//	 Start line of image.  
			
			CheckStartValue ( 
							theNum, 
							(SInt32*)&dialogSelectAreaPtr->lineStart,
							1,
							imageWindowInfoPtr->maxNumberLines, 
							dialogPtr, 
							itemHit);
			break;

		case 2:				//	 End line of image  
		
			CheckEndLCValue ( 
							theNum, 
							(SInt32*)&dialogSelectAreaPtr->lineEnd,
							imageWindowInfoPtr->maxNumberLines, 
							dialogPtr, 
							theHandle, 
							itemHit);
			break;

		case 3:				//	 Line interval of image  

			dialogSelectAreaPtr->lineInterval = CheckIntervalValue (
																	theNum, 
																	dialogSelectAreaPtr->lineInterval, 
																	1,  
																	imageWindowInfoPtr->maxNumberLines, 
																	dialogPtr, 
																	itemHit,
																	theHandle);
			break;

		case 4:				//	 Start column of image  
			
			CheckStartValue ( 
							theNum, 
							(SInt32*)&dialogSelectAreaPtr->columnStart,
							1,
							imageWindowInfoPtr->maxNumberColumns, 
							dialogPtr, 
							itemHit);
			break;

		case 5:				//	 End column of image  
		
			CheckEndLCValue ( 
							theNum, 
							(SInt32*)&dialogSelectAreaPtr->columnEnd,
							imageWindowInfoPtr->maxNumberColumns, 
							dialogPtr, 
							theHandle, 
							itemHit);
			break;

		case 6:				//	 Column interval of image  

			dialogSelectAreaPtr->columnInterval = CheckIntervalValue (
																	theNum, 
																	dialogSelectAreaPtr->columnInterval, 
																	1,  
																	imageWindowInfoPtr->maxNumberColumns, 
																	dialogPtr, 
																	itemHit,
																	theHandle); 
																	
			break;
			
		default:		// Entire area to selected area switch.			
			UpdateLineColumnValues (
							imageWindowInfoPtr,
							dialogSelectAreaPtr->windowPtr, 
							dialogPtr, 
							&dialogSelectAreaPtr->toSelectionBox, 
							dialogSelectAreaPtr->baseItem, 
							(SInt32*)&dialogSelectAreaPtr->lineStart, 
							(SInt32*)&dialogSelectAreaPtr->lineEnd, 
							(SInt32*)&dialogSelectAreaPtr->columnStart, 
							(SInt32*)&dialogSelectAreaPtr->columnEnd,
							dialogSelectAreaPtr->adjustToBaseImageFlag);

			UpdateEntireIconButton (dialogSelectAreaPtr,
												dialogPtr);
			break;
			
		}		// end "switch (parameter)" 
			
	if (dialogSelectAreaPtr->entireIconItem != 0 && 
											parameter >= 1 && parameter <= 6)
		{
		if (gEntireImageFlag != CheckIfEntireImage ( 
							dialogSelectAreaPtr->imageWindowInfoPtr, 
							dialogSelectAreaPtr->lineStart, 
							dialogSelectAreaPtr->lineEnd, 
							dialogSelectAreaPtr->columnStart, 
							dialogSelectAreaPtr->columnEnd))
			{
			gEntireImageFlag = !gEntireImageFlag;
			
			InvalWindowRect (GetDialogWindow(dialogPtr), 
									&dialogSelectAreaPtr->toSelectionBox);

			UpdateEntireIconButton (dialogSelectAreaPtr,
												dialogPtr);
								
			}		// end "if (gEntireImageFlag != ..." 
								
		}		// end "if ( dialogSelectAreaPtr->entireIconItem != 0 && ..."
				
}		// end "DialogLineColumnHits" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal Boolean DialogListFilter
//
//	Software purpose:	The purpose of this routine is to handle the 
//							processing for dialog hits in lists.  The only
//							item handled by this filter is a hit which 
//							occurred within a list.
//		
//	Parameters in:		Pointer do dialog window
//							Address of event record
//
//	Parameters out:	Item hit
//
// Value Returned:	TRUE if routine handled item hit.
//							FALSE if routine did not handle item hit.
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/15/1989
//	Revised By:			Larry L. Biehl			Date: 06/26/1996

pascal Boolean DialogListFilter (
				DialogPtr							theDialogPtr, 
				EventRecord*						theEventPtr, 
				SInt16*								itemHitPtr)

{
	Rect									rect;
	
	Handle								theHandle;
	
	SInt16								theType,
											theValue;
	
	Boolean								returnFlag;
	
	
	returnFlag = FALSE;
		
	switch (theEventPtr->what)
		{
		case keyDown:
			
		   theValue = (theEventPtr->message & charCodeMask);
		   
		   		// Check for default cases.												
		   
			theType = DialogKeyFilter (theDialogPtr, theEventPtr, itemHitPtr, theValue);
			
			if (theType == 1)
																					return (TRUE);
			
			if (theType == 0)
																					return (FALSE);

					// Get the item number of the edit field								
					
//			*itemHitPtr = ((DialogPeek)theDialogPtr)->editField + 1;
			*itemHitPtr = GetDialogKeyboardFocusItem (theDialogPtr);
				
			GetDialogItem (theDialogPtr, *itemHitPtr, &theType, &theHandle, &rect);
			
					// Check for invalid numbers for edittext fields if the			
					// type is 16																	
						
			if (theType == 16)
				{
				if ( theValue < 0x30 || theValue > 0x39 )
					{
					Alert (kErrorAlert2ID, NIL);
					return(TRUE);
					
					}		// end "if ( theValue < 0x30 || theValue > 0x39 )" 
					
				}		// end "if (theType == 16)" 
				
			break;
			
		}		// end "switch (theEventPtr->what)" 
	
			// Indicate that the default filter should be used.						
			
	return (returnFlag);
	
}		// end "DialogListFilter"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void DrawColorBoxInDialogWindow
//
//	Software purpose:	The purpose of this routine is to draw a rounded
//							rectangle filled with the current color.  It is
//							used by the Thematic and Overlay Parameter dialog routines.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			DisplayThematicDialog in displayThematic.c
//
//	Coded By:			Larry L. Biehl			Date: 05/24/2001
//	Revised By:			Larry L. Biehl			Date: 05/24/2001

pascal void DrawColorBoxInDialogWindow (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{
	PenState								penState;
	Rect									overlayRect;
	RGBColor								savedForeGroundColor;
	
	Handle								theHandle;
	
	SInt16								theType;
	
		
			// Save the current pen state.													
		
	GetPenState (&penState);
	if (gColorQDflag)
		GetForeColor (&savedForeGroundColor);

			// Get the area to draw the background color in.							
			
	GetDialogItem (dialogPtr, itemNumber, &theType, &theHandle, &overlayRect);
	
			// Set the penmode and color.														
	
	PenMode (patCopy);
	
	if (gColorQDflag)
		RGBForeColor (&gCurrentSelectedColor);
		
	else		// !gColorQDflag
		ForeColor (whiteColor);
	
			// Paint the color.																	
			
	PaintRoundRect (&overlayRect, 6, 4);
	
			// Reset the foreground color.													
	
	if (gColorQDflag)		
		RGBForeColor (&savedForeGroundColor);
		
	else		// !gColorQDflag
		ForeColor (blackColor);
	
			// Now outline the box.																
	
	FrameRoundRect (&overlayRect, 6, 4);		
			
			// Reset the pen state back to the original settings.						

	SetPenState (&penState);
	
}		// end "DrawColorBoxInDialogWindow"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void DrawGraphicsBox
//
//	Software purpose:	The purpose of this routine is to draw the 
//							graphics box in the graphics status dialog window.
//
//	Parameters in:					
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 11/08/1993
//	Revised By:			Larry L. Biehl			Date: 12/22/1995	

pascal void DrawGraphicsBox (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{	
	Handle								theHandle;
	Rect									tempRect;
	Pattern								pattern;
	
	SInt16								theType;
	
	
	GetDialogItem (dialogPtr, itemNumber, &theType, &theHandle, &tempRect);
	
			// Draw background

	FillRect (&tempRect, GetQDGlobalsGray (&pattern));
	FrameRect (&tempRect);
			
//	RGBColor color;
//	color.red = 0x4000;
//	color.green = 0x4000;
//	color.blue = 0x4000;
//	RGBForeColor ( &color );
//	PaintRect( &tempRect );

			// Fill used bar
	
	tempRect.right = gStatusGraphicsRight;
	FillRect (&tempRect, GetQDGlobalsBlack(&pattern));
			
//	color.red = 0xCCCC;
//	color.green = 0xCCCC;
//	color.blue = 0xFFFF;
//	RGBForeColor ( &color );
//	PaintRect( &tempRect );
	
}		// end "DrawGraphicsBox"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void FlashDLogItem
//
//	Software purpose:	The purpose of this routine is to momentarily hilite
//							a dialog button.  This routine came from a network news
//							article on October 7, 1990 by Greg L. Anderson from
//							Apple Computer Inc.  Permission was given for anyone
//							to use this code.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:			DialogKeyFilter in dialogUtilities.c
//							CheckSomeEvents in multiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 11/08/1990
//	Revised By:			Larry L. Biehl			Date: 06/23/1993

void FlashDLogItem (
				DialogPtr							dialogPtr, 
				SInt16								dialogItem)

{
	Rect									theRect;
	Handle								theHandle;
	SInt32								ticky;
	SInt16								theType;


	if (dialogPtr != NULL)
		{
		GetDialogItem (dialogPtr, dialogItem, &theType, &theHandle, &theRect);
		HiliteControl ( (ControlHandle)theHandle, 1);
		Draw1Control ( (ControlHandle)theHandle );
		
		ticky = TickCount() + 10;
		while (ticky > TickCount() ) 
			{ }
			
		HiliteControl ( (ControlHandle)theHandle, 0);
		Draw1Control ( (ControlHandle)theHandle );
		
		}		// end "if (dialogPtr != NULL)" 
			
}		// end "FlashDLogItem"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		double GetDItemRealValue
//
//	Software purpose:	The purpose of this routine is to get the string
//							for the input dialog window and item and convert the
//							string to a real number. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			DisplayThematicDialog   in display.c
//
//	Coded By:			Larry L. Biehl			Date: 12/19/1989
//	Revised By:			Larry L. Biehl			Date: 06/06/1997

double GetDItemRealValue (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{
	double				realValue;
	
	Rect					theBox;
	
	Handle				theHandle;
	
	SInt16				theType;
	

	if (dialogPtr != NULL)
		{
		GetDialogItem (dialogPtr, itemNumber, &theType, &theHandle, &theBox);
		GetDialogItemText (theHandle, gTextString);
		
		gTextString[ gTextString[0]+1 ] = kNullTerminator;
//		realValue = atof ((char*)&gTextString[1]);
		realValue = strtod ((char*)&gTextString[1], NULL);
		
		return (realValue);
		
		}		// end "if (dialogPtr != NULL)" 
		
	return (0);
	
}		// end "GetDItemRealValue"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		short int GetDLogControl
//
//	Software purpose:	The purpose of this routine is to get the
//							value of the control in the dialog window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			StatisticsDialog   in statistics.c
//
//	Coded By:			Larry L. Biehl			Date: 01/30/1989
//	Revised By:			Larry L. Biehl			Date: 02/19/1991

SInt16 GetDLogControl (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{
	Handle								theHandle;
	Rect									theBox;
	SInt16								theType;
	
	
	GetDialogItem (dialogPtr, itemNumber,  &theType, &theHandle, &theBox);
	return ( GetControlValue ((ControlHandle)theHandle) );
												
}		// end "GetDLogControl"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetDlogOrigin
//
//	Software purpose:	The purpose of this routine is to Calculate where 
//							to put a dialog box so that it's centered from left
//							to right.  This is screen-independant.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:		
//
//	Coded By:			?							Date: ??/??/??
//	Revised By:			Larry L. Biehl			Date: 07/24/2001

void GetDlogOrigin (
				SInt16	     						dlogID, 
				Point*								where)

{
	BitMap								bitMap;
	Rect									tempRect;
	Point									mousePt;
	Rect* 								dialogRectPtr;
	DialogTHndl 						dlogHandle;
	GDHandle								curDevice;
	

			// Get a handle to the dialog.													
			
	dlogHandle = (DialogTHndl) GetResource ('DLOG', dlogID);

	if (dlogHandle == NULL)
		SetPt (where, 85, 85);
		
	else		// dlogHandle != NULL 
		{
				// Get bounding rectangle of the screen that the mouse is 			
				// currently in.																	
		
		GetQDGlobalsScreenBits (&bitMap);
		tempRect = bitMap.bounds;
		
		if (gColorQDflag)
			{
			curDevice = GetDeviceList();
			GetMouse (&mousePt);
			LocalToGlobal (&mousePt);
		
			while (curDevice)
				{
				if ( 	TestDeviceAttribute(curDevice, screenDevice) &&
						TestDeviceAttribute(curDevice, screenActive) &&
						PtInRect(mousePt, &(*curDevice)->gdRect) )
					tempRect = (*curDevice)->gdRect;
					
				curDevice = GetNextDevice (curDevice);
				
				}		// end "while (curDevice)" 
				
			}		// end "if (gColorQDflag)" 
			
		tempRect.top += GetMBarHeight();
		
				// Get pointer to its bounding rectangle.									
				
		dialogRectPtr = &((**dlogHandle).boundsRect);
	
				// Calculate upper left corner that will leave box centered.		
		
		where->h = 	dialogRectPtr->left + 
						(tempRect.right + tempRect.left)/2 -
						(dialogRectPtr->right + dialogRectPtr->left)/2;
		
		where->v = 	dialogRectPtr->top + 
						(tempRect.top + tempRect.bottom)/2 -
						(dialogRectPtr->top + dialogRectPtr->bottom)/2;
						
				// Adjust so that 1/5 of space is above and 4/5 of space is 		
				// below.																			
		
		where->v -= (where->v - tempRect.top) * 6/10;
		
		}		// end "else dlogHandle != NULL" 
	
}		// end "GetDlogOrigin" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		ListHandle GetOneColumnDialogListHandle
//
//	Software purpose:	The purpose of this routine is to get a handle for
//							a one column list.  The routine will
//							be used to create a list for loading in a column
//							of channel numbers/descriptions and class names.
//
//	Functional description:	The steps in this function follow.
//
//			1) Get the location and area of the list.
//			2) Outline the area that the list will fall into.
//			3) Get a handle to the list information.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 02/20/1989
//	Revised By:			Larry L. Biehl			Date: 07/28/2001

ListHandle GetOneColumnDialogListHandle (
				DialogRef							theDialog, 
				SInt16								itemNumber)

{
	Point									cellSize = {0,0};
	Rect									dataBounds,
											rView;
									
	Handle								theHandle;
	ListHandle							listHandle;
	
	SInt16								cellHeight,
											newListHeight,
											theType;
	
	
			// Create a one column list for the dialog window.							

			// Get the display rectangle for the item to place the list into.		
		
	GetDialogItem (theDialog, itemNumber, &theType, &theHandle, &rView);
	
	rView.right -= 15;
	SetRect (&dataBounds, 0, 0, 1, 0);
	
	listHandle = LNew (&rView, 
								&dataBounds, 
								cellSize, 
								0, 
								GetDialogWindow(theDialog), 
								TRUE, 
								FALSE, 
								FALSE, 
								TRUE);
	
	if (listHandle != NULL)
		{
				// Make certain that the list view is just tall enough for an		
				// integer number of lines.													
				
	   cellHeight = ((ListPtr)*listHandle)->cellSize.v;
	   newListHeight = ((rView.bottom-rView.top+1)/cellHeight) * cellHeight;
	   LSize (rView.right-rView.left+1, newListHeight, listHandle);
	   
	   		// Now make the dialog item for the list the same size.
	   
	   rView.bottom = rView.top + newListHeight;
		rView.right += 15;	
		SetDialogItem (theDialog, itemNumber, theType, theHandle, &rView);
		
				// Make default settings for selection flag.
				
		(*listHandle)->selFlags = lNoRect + lNoExtend + lUseSense;
	   
	   }		// end "if (listHandle != NULL)"
	
	return (listHandle);

}		// end "GetOneColumnDialogListHandle"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void HideDialogItems
//
//	Software purpose:	The purpose of this routine is to hide the requested
//							continuous list of dialog items.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/22/1993
//	Revised By:			Larry L. Biehl			Date: 04/22/1993

void HideDialogItems (
				DialogPtr							dialogPtr, 
				SInt16				     			startItem, 
				SInt16				     			lastItem)

{
	SInt16								index;
	
	
	if (dialogPtr != NULL)
		{
		for (index=startItem; index<=lastItem; index++)
			HideDialogItem (dialogPtr, index);
		
		}		// end "if (dialogPtr != NULL)" 
	
}		// end "HideDialogItems" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void HiliteOKButton
//
//	Software purpose:	The purpose of this routine is to hilite the OK
//							button in dialog windows.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/12/1988
//	Revised By:			Larry L. Biehl			Date: 10/12/1989	

pascal void HiliteOKButton (
				DialogPtr							dialogPtr, 
				SInt16								item)

{
			// Declare local variables															
			
	GrafPtr								savedPort;
	Handle								theHandle;
	Rect									theBox;
	SInt16								theType;
	

			// Hilite the OK button as the default button								
			
	GetDialogItem (dialogPtr, 1, &theType, &theHandle, &theBox);
	GetPort (&savedPort);
//	SetPort (dialogPtr);
	SetPortDialogPort (dialogPtr);
	PenSize (3,3);
	InsetRect (&theBox, -4, -4);
	FrameRoundRect (&theBox, 16, 16);
	PenNormal();
	SetPort (savedPort);

}		// end "HiliteOKButton"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void InvalidateDialogItemRect
//
//	Software purpose:	The purpose of this routine is to invalidate the rectangle
//							the input dialog item.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/02/1999
//	Revised By:			Larry L. Biehl			Date: 12/02/1999	

void InvalidateDialogItemRect (
				DialogPtr							dialogPtr, 
				SInt16								item)

{			
	Rect									theBox;
	Handle								theHandle;
	SInt16								theType;
						
			
	GetDialogItem (dialogPtr, item, &theType, &theHandle, &theBox);
//	InvalRect (&theBox);
	InvalWindowRect (GetDialogWindow(dialogPtr), &theBox);

}		// end "InvalidateDialogItemRect"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		DialogPtr LoadRequestedDialog
//
//	Software purpose:	The purpose of this routine is to load the requested
//							dialog and make certain that there is enough memory
//							left to display the dialog.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	DialogPtr 		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/10/1991
//	Revised By:			Larry L. Biehl			Date: 09/21/2001

DialogPtr LoadRequestedDialog (
				SInt16								specificationID,
				Boolean								copyScrapFlag,
				SInt16								defaultItemNumber,
				SInt16								cancelItemNumber)
				
{
	DialogPtr							dialogPtr;
	
	Handle								checkHandle;
	
	SInt32								resourceSize;

	
			// First verify that memory is available for the modal filter if		
			// requested.																			
		
	resourceSize = 0;
	dialogPtr = NULL;
	SetResLoad (FALSE);
	checkHandle = Get1Resource ('DLOG', specificationID);
	if (checkHandle != NULL)
		{
		resourceSize = GetMaxResourceSize (checkHandle);
		ReleaseResource (checkHandle);
		
		}		// end "if (checkHandle!= NULL)" 
		
	checkHandle = Get1Resource ('DITL', specificationID);
	if (checkHandle != NULL)
		{
		resourceSize += GetMaxResourceSize (checkHandle);
		ReleaseResource (checkHandle);
		
		}		// end "if (checkHandle!= NULL)" 
		
	SetResLoad (TRUE);
		
			// Make sure there is a minimum amount of memory available for			
			// displaying the dialog.															
			
	checkHandle = MNewHandle (resourceSize + gMinMemoryForDrawingDialog);
	if (checkHandle != NULL)
		{
		DisposeHandle (checkHandle);
			
		dialogPtr = GetNewDialog (specificationID, NIL, (WindowPtr)-1);
		
		if (dialogPtr != NULL)
			{
					// Make sure there is a minimum amount of memory available for	
					// displaying the dialog.													
				
			checkHandle = MNewHandle (gMinMemoryForDrawingDialog);
			if (checkHandle == NULL)
				{
				DisposeDialog (dialogPtr);
				dialogPtr = NULL;
			
				}		// end "if (checkHandle == NULL)" 
			
			else		// checkHandle != NULL 
				{
				DisposeHandle (checkHandle);
				
				if (defaultItemNumber > 0)
					SetDialogDefaultItem (dialogPtr, defaultItemNumber);
				
				if (cancelItemNumber > 0)
					SetDialogCancelItem (dialogPtr, cancelItemNumber);
				
						// There are no desk accesseries in Carbon implementation.
				
				if (copyScrapFlag)
					{
					SetDialogTracksCursor (dialogPtr, TRUE);
					
					#if !TARGET_API_MAC_CARBON	
								// Load the desk scrap to the TextEdit scrap in case the	
								// use wants to copy values into text edit fields.			
						
						UpdateTextEditScrap ();
					#endif	// !TARGET_API_MAC_CARBON
							
					}		// end "if (copyScrapFlag)" 
				
				}		// end "else checkHandle != NULL" 
				
			}		// end "if (dialogPtr != NULL)" 
		
		}		// end "if (checkHandle == NULL)" 
		
	return (dialogPtr);
	
}		// end LoadRequestedDialog 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ModalOneColListDialog
//
//	Software purpose:	The purpose of this routine is to handle the user
//							selections in the one column list dialog that is used
//							to allow the user the select channels or classes. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			ClassDialog   in projectUtilities.c
// 						ClassGroupDialog   in displayThematic.c
// 						ChannelsDialog   in multiSpecUtilities.c
//
//	Coded By:			Larry L. Biehl			Date: 01/16/1989
//	Revised By:			Larry L. Biehl			Date: 01/30/1998

Boolean ModalOneColListDialog (
				DialogPtr							dialogPtr, 
				SInt16								numberInputVecItems,
				UInt32*								numberOutputVecItemsPtr, 
				SInt16*								vectorPtr, 
				SInt16								minimumItemsRequired)

{						
	Rect									theBox;
	
	Cell									cell;
	
	Handle								okHandle,
											theHandle;
	
	SInt32								theNum;
	
	SInt16								index,
											item,
											itemHit,
											listEnd,
											listInterval,
											listStart,
											numberSelections,
											theType;
								
	Boolean								modalDone,
											returnFlag,
											updateSelectionsFlag;


			// Initialize local variables.													
			
	returnFlag = FALSE;
	listStart = 1;
	listEnd = numberInputVecItems;
	listInterval = 1;
	updateSelectionsFlag = FALSE;
	
			// Continue if dialogPtr and gDialogListHandle are not NULL.			
			
	if (dialogPtr != NULL && gDialogListHandle != NULL)
		{
				// Get the "handle" for the OK button for use later.					
				
		GetDialogItem (dialogPtr, 1, &theType, &okHandle, &theBox);
		
		SelectDialogItemText (dialogPtr, 9, 0, INT16_MAX);
		
		modalDone = FALSE;
		itemHit = 0;
		do 
			{
		   ModalDialog (gProcessorDialogFilterPtr, &itemHit);
			if (itemHit > 2)
				{
						// For number value items, get the string and convert it to	
						// a number.																
	
				GetDialogItem (dialogPtr, itemHit, &theType, &theHandle, &theBox);
				if (theType == 16)
					{
					GetDialogItemText (theHandle, gTextString);	
					StringToNum ( gTextString, &theNum);
					
					}		// end "if (theType == 16)" 
				
				switch (itemHit)
					{
					case 3:		// Select all items in the list. 
						LSetDrawingMode (FALSE, gDialogListHandle);
						cell.h = 0;
						for ( item=0; item<numberInputVecItems; item++)
							{
							cell.v = item;
							LSetSelect (TRUE, cell, gDialogListHandle);
							
							}		// end "for ( item=0; item<..." 
							
						numberSelections = numberInputVecItems;
							
						LSetDrawingMode (TRUE, gDialogListHandle);
						LUpdate ( GetPortVisibleRegion (GetDialogPort (dialogPtr), gTempRegion1), gDialogListHandle );
						SetEmptyRgn (gTempRegion1);
						HiliteControl ((ControlHandle)okHandle, 0);
						break;
					
					case 4:		// Deselect all items in the list. 
						LSetDrawingMode (FALSE, gDialogListHandle);
						cell.h = 0;
						for ( item=0; item<numberInputVecItems; item++)
							{
							cell.v = item;
							LSetSelect (FALSE, cell, gDialogListHandle);
							
							}		// end "for ( item=0; item<..." 
							
						numberSelections = 0;
							
						LSetDrawingMode (TRUE, gDialogListHandle);
						LUpdate ( GetPortVisibleRegion (GetDialogPort (dialogPtr), gTempRegion1), gDialogListHandle );
						SetEmptyRgn (gTempRegion1);
						HiliteControl ((ControlHandle)okHandle, 255);
						break;
						
					case 5:				// Modify item list 
						cell.h = 0;
						cell.v = 0;
						numberSelections = 0;
							
								// Get the count of the number of selections.			
								
						while ( LGetSelect (TRUE, &cell, gDialogListHandle) )
							{
							cell.v++;
							numberSelections++;
							
							}		// end "while ( !LGetSelect..." 
						break;
						
					case 9:		// List start 
						if (theNum == 0 || theNum > numberInputVecItems)
							NumberErrorAlert ( (SInt32)listStart, dialogPtr, itemHit);
							 		
						else		// theNum > 0 && theNum <= numberInputVecItems 
							{
							if (listStart != theNum)
								{
								updateSelectionsFlag = TRUE;
								listStart = theNum;
								
								}		// end "if (listStart != theNum)" 
							
							}		// end "else theNum > 0 && ..." 
						break;
						
					case 11:		// List end 
						if (theNum == 0 || theNum > numberInputVecItems)
							NumberErrorAlert ( 
							 		(SInt32)listEnd, dialogPtr, itemHit);
							 		
						else		// theNum > 0 && theNum <= numberInputVecItems 
							{
							if (listEnd != theNum)
								{
								updateSelectionsFlag = TRUE;
								listEnd = theNum;
								
								}		// end "if (listEnd != theNum)" 
							
							}		// end "else theNum > 0 && ..." 
						break;
						
					case 13:		// List interval 
						if (theNum == 0 || theNum > numberInputVecItems)
							NumberErrorAlert ( 
							 	(SInt32)listInterval, dialogPtr, itemHit);
							 		
						else		// theNum > 0 && theNum <= numberInputVecItems 
							{
							if (listInterval != theNum)
								{
								updateSelectionsFlag = TRUE;
								listInterval = theNum;
								
								}		// end "if (listInterval != theNum)" 
									
							}		// end "else theNum > 0 && ..." 
						break;
						
					case 14:		// Enter new list selections. 
						updateSelectionsFlag = TRUE;
						break;
							
					}		// end "switch (itemHit)" 
					
				if (itemHit >= 9 && itemHit <= 13)
					{
							// Check if first channel is greater than last channel. 
							
					if (listStart > listEnd)
						SetDLogControlHilite (dialogPtr, 14, 255);
						
					else		// listStart <= listEnd 
						SetDLogControlHilite (dialogPtr, 14, 0);
						 				
					}		// end "if (itemHit >= 9 && itemHit <= 13)" 
					
				if (updateSelectionsFlag)
					{
					numberSelections = UpdateOneColListSelections (
							numberInputVecItems, listStart, listEnd, listInterval);
					LUpdate ( GetPortVisibleRegion (GetDialogPort (dialogPtr), gTempRegion1), gDialogListHandle );
					SetEmptyRgn (gTempRegion1);
							
					}		// end "if (updateSelectionsFlag)" 
				
				if ( 	(itemHit >= 3 && itemHit <= 5 ) || updateSelectionsFlag)
					{
							// Hilite OK button depending on number of selections.	
							
					if (numberSelections >=minimumItemsRequired)
						HiliteControl ((ControlHandle)okHandle, 0);
						
					else		// numberSelections < minimumItemsRequired 
						HiliteControl ((ControlHandle)okHandle, 255);
				
							// Set the number of selected items.							
							
					LoadDItemValue (dialogPtr, 16, (SInt32)numberSelections);
					
					updateSelectionsFlag = FALSE;
					
					}		// end "if ( (itemHit >= 3 && itemHit <= 5) || ..." 
					
				}		// end "if (itemHit > 2)" 
				
			else
				if	(itemHit == 1)      // User selected OK for information 
					{
					modalDone = TRUE;
					returnFlag = TRUE;
						
							// Item 5; Get items to use.										
					
					index = 0;	
					cell.h = 0;
					
					for ( item=0; item<numberInputVecItems; item++)
						{
						cell.v = item;
						if ( LGetSelect (FALSE, &cell, gDialogListHandle) )
							{
							vectorPtr[index] = item + 1;
							index++;
							
							}		// end "if ( LGetSelect (FALSE, &cell, ..." 
							
						}		// end "for ( channel=0; channel<..." 
	
					*numberOutputVecItemsPtr = index;
													
					}		// end "if (itemHit == 1)" 
					
				if	(itemHit == 2)      // User selected Cancel for information 
					{
					modalDone = TRUE;
					returnFlag = FALSE;
					
					}		// end "if	(itemHit == 2)" 
					
			} while (!modalDone);
			
		}		// end "if (dialogPtr != NULL && gDialogListHandle != NULL)" 
		
	return (returnFlag);
			
}		// end "ModalOneColListDialog"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void NumberDefault
//
//	Software purpose:	The purpose of this routine is to set the value
//							of the given field in a dialog box back to a default
//							value.  This routine is usually called after an
//							invalid value has been entered in the field.
//
//	Parameters in:		Handle to the item in the dialog box.
//							Address of a string
//							Default value to be converted to a string
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 04/20/1988
//	Revised By:			Larry L. Biehl			Date: 09/07/1989	

void NumberDefault (
				Handle								theHandle, 
				SInt32								theLongValue, 
				Str255*								stringPtr)

{
	NumToString (theLongValue, *stringPtr);
	SetDialogItemText (theHandle, (ConstStr255Param)*stringPtr);
	
}		// end "NumberDefault" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal Boolean ProcessorDialogFilter
//
//	Software purpose:	The purpose of this routine is to handle the 
//							processing for dialog control hits in a dialog
//							that may have integer or real, positive or negative
//							values.  A structure is accessed which describes
//							the characteristics of items in the calling dialog.
//		
//	Parameters in:		Pointer do dialog window
//							Address of event record
//
//	Parameters out:	Item hit
//
// Value Returned:	TRUE if routine handled item hit.
//							FALSE if routine did not handle item hit.
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/04/1994
//	Revised By:			Larry L. Biehl			Date: 11/06/2009

pascal Boolean ProcessorDialogFilter (
				DialogPtr							theDialogPtr, 
				EventRecord*						theEventPtr, 
				SInt16*								itemHitPtr)

{
	Rect									rect;
	
	Point									mousePt;
	
	ModalFilterUPP						standardProc;
	WindowPtr							window;
	
//	Handle								theHandle;
	
	SInt32								menuData;

	SInt16								errorReturn,
											mouseLoc,
//											theType,
											what;
											
//	UInt16								controlHilite;

	Boolean								doubleClick,
											dialogEventFlag,
											returnFlag;
//											setHiliteFlag;
	
	
	returnFlag = FALSE;
	
	what = theEventPtr->what;
	dialogEventFlag = IsDialogEvent (theEventPtr);
		
	if (what == updateEvt && !dialogEventFlag)
//	if (what == updateEvt && (WindowPtr)theEventPtr->message != GetDialogWindow(theDialogPtr))
		{
		DoUpdateEvent (theEventPtr);
		*itemHitPtr = 0;
		returnFlag = TRUE;
		
		}		// end "if (what == updateEvt && !dialogEventFlag)" 
		
	else if (what == activateEvt && !dialogEventFlag)
		{
		DoActivateEvent ( (WindowPtr)theEventPtr->message,
									(theEventPtr->modifiers & activeFlag) );
		*itemHitPtr = 0;
		returnFlag = TRUE;
		
		}		// end "if (what == activateEvt && !dialogEventFlag)" 
		
	else if (gMultiSpecWorkflowInfo.workFlowCode == 1 || 
					gMultiSpecWorkflowInfo.workFlowCode == 2)
		{
		if (gProcessorCode == kOpenImageFileProcessor)
					// For now this will always be assumed to be okay. Cancel
					// would be to not display the image window.
			*itemHitPtr = 1;
		
		else		// gProcessorCode != kOpenImageFileProcessor
			*itemHitPtr = gMultiSpecWorkflowInfo.workFlowCode;
		
		returnFlag = TRUE;
		
		}		// end "if ((gMultiSpecWorkFlowCode == 1 || gMultiSpecWorkFlowCode == 2) && ..."
									
	else		// call standard dialog filter
		{
		errorReturn = GetStdFilterProc (&standardProc);
		if (errorReturn == noErr)
			{
/*			setHiliteFlag = FALSE;
			if (what == activateEvt || what == updateEvt)
				{
						// Get the hilite of the OK button. Make sure that that hilite is not
						// change upon exit from standardProc.
						
				GetDialogItem (theDialogPtr, 1,  &theType, &theHandle, &rect);
				if ( theHandle != NULL && (theType & ctrlItem) )
					{
					controlHilite = GetControlHilite ((ControlRef)theHandle);
					
					if (controlHilite == 255)
						setHiliteFlag = TRUE;
						
					}		// end "if ( theHandle != NULL && (theType & ctrlItem) )"
				
				}		// end "if (what == activateEvt || what == updateEvt)"
*/				
			returnFlag = InvokeModalFilterUPP (theDialogPtr, theEventPtr, itemHitPtr, standardProc);
			
//			if (setHiliteFlag)
//				HiliteControl ((ControlRef)theHandle, controlHilite);
			
			}		// end "if (errorReturn == noErr)"
			
		}		// end "else call standard dialog filter"
		
	if (!returnFlag && what != nullEvent)
//	if (theEventPtr->what != nullEvent)
		{
//		what = theEventPtr->what;
				
		if (dialogEventFlag)
			{
//			errorReturn = GetStdFilterProc (&standardProc);
//			if (errorReturn == noErr)
//				returnFlag = CallModalFilterProc (standardProc, theDialogPtr, theEventPtr, itemHitPtr);
				
					// These events have already been handled so not do anything.
					
			switch (what)
				{
	  			case updateEvt:
				case activateEvt:
					what = nullEvent;
					
							// Make sure that itemHit is set to "no item".
							
					*itemHitPtr = -2;
					returnFlag = FALSE;
	      		break;
	      		
	      	}		// end "switch (what)"
					
			}		// end "if (dialogEventFlag)" 
		
		switch (what)
			{
	 		case mouseDown:
	
	  			mouseLoc = FindWindow (theEventPtr->where, &window);
	  
	  			switch (mouseLoc)
	  				{
	    			case inMenuBar:
	    			
	    				menuData = MenuSelect (theEventPtr->where);
	      			break;
				
	    			case inSysWindow:
	    			
	    				#if !TARGET_API_MAC_CARBON
	      				SystemClick (theEventPtr, window);
	    				#endif	// !TARGET_API_MAC_CARBON
	      			break;
	      
	    			case inDrag:
	    	
	    				if (window == GetDialogWindow(theDialogPtr))
	    					{
	    							// Move the moveable model dialog window
	    							// (movableDBoxProc)
	    							
	    					DragMultiSpecWindow(window, theEventPtr->where);
	
							returnFlag = TRUE; 
	      					
	      				}		// end "if (window && ...)" 
	      				
	      			else		// window == NULL || (window != ... 
	 						FlushEvents (mUpMask,mDownMask);
	 						
	      			break;
	      			
	      		case inContent:
						if (gDialogListHandle != NULL)
							{
									// Get mouse location and convert it from global to local	
									// coordinates.															
									
							mousePt = theEventPtr->where;
							GlobalToLocal (&mousePt);
							rect = ((ListPtr)*gDialogListHandle)->rView;
							rect.right += kSBarWidth;
//							TextFont (gWindowTextFont); 				// monaco 
							if ( PtInRect ( mousePt, &rect ) )
								doubleClick = LClick (mousePt, 
																theEventPtr->modifiers, 
																gDialogListHandle);
																						
							else if (gDialogListHandle2 != NULL)
								{
								rect = ((ListPtr)*gDialogListHandle2)->rView;
								rect.right += kSBarWidth;
								if ( PtInRect ( mousePt, &rect ) )
									doubleClick = LClick (mousePt, 
																	theEventPtr->modifiers, 
																	gDialogListHandle2);
				
								}		// end "else if (gDialogListHandle2 != NULL)" 
																						
							}		// end "if (gDialogListHandle != NULL)" 
							
						else if (gProcessorCode == kAboutMultiSpecProcessor)	// gDialogListHandle == NULL
							{			
									// About box is displayed.
									
							if (!gFullVersionSettingFlag)
								{									
								*itemHitPtr = 1;
								returnFlag = TRUE;
								
								}		// end "if (!gFullVersionSettingFlag)"
							
							}		// end "else if (gProcessorCode == kAboutMultiSpecProcessor)"
							
						else
							{	
	  						gEventRecord.modifiers = theEventPtr->modifiers;	
	  			
		 							// Flush the mouse ups so that we are ready to 	
		 							// look for the next mouse down.  Otherwise		
		 							// no more mousedowns are seen.					
		 					
		 					FlushEvents (mUpMask,mDownMask);
		 					}
	      			break;
	      			
	      		default:
	 					FlushEvents (mUpMask,mDownMask);
						break;
	      			
	      		}		// end "switch (mouseLoc)"
	      		
	 			break;
	
			case keyDown:
				
				if (gProcessorCode != kAboutMultiSpecProcessor)
					returnFlag = 
						CheckKeyDownDialogFilter (theDialogPtr, theEventPtr, itemHitPtr);
						
				else		// gProcessorCode == kAboutMultiSpecProcessor
					{
							// About box is displayed.
							
					*itemHitPtr = 1;
					returnFlag = TRUE;
					
					}		// end "else if (gProcessorCode == kAboutMultiSpecProcessor)"
				break;
				
	    	case osEvt:
				*itemHitPtr = -2;
				returnFlag = FALSE;
				break;
/*			  
	  		case updateEvt:
	  		
				if ((WindowPtr)theEventPtr->message != GetDialogWindow(theDialogPtr))
					{
	  				DoUpdateEvent (theEventPtr);
					returnFlag = TRUE;
					
							// Make sure that itemHit is set to "no item".
							
					*itemHitPtr = 0;
	
					}		// end "if ((WindowPtr)theEventPtr->message != theDialogPtr)"
	      	break;
	    
			case activateEvt:
	
				if ((WindowPtr)theEventPtr->message != GetDialogWindow(theDialogPtr))
					{
					DoActivateEvent ( (WindowPtr)theEventPtr->message,
												(theEventPtr->modifiers & activeFlag) );
					returnFlag = TRUE;
					
							// Make sure that itemHit is set to "no item".
							
					*itemHitPtr = 0;
	 
					}		// end "if ((WindowPtr)theEventPtr->message != theDialogPtr)"
				break;
*/				
			}		// end "switch (what)" 
			
		}		// end "if (theEventPtr->what != nullEvent)"
		
	return (returnFlag);
	
}		// end "ProcessorDialogFilter"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void RealNumberErrorAlert
//
//	Software purpose:	The purpose of this routine is to notify the user 
//							that an illegal real number was entered into a dialog 
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
//	Coded By:			Larry L. Biehl			Date: 12/20/1989
//	Revised By:			Larry L. Biehl			Date: 04/21/2006

void RealNumberErrorAlert (
				double								theRealValue,
				DialogPtr							dialogPtr, 
				SInt16								itemHit, 
				SInt16								decimalPlaces)

{
	RealNumberErrorAlert (theRealValue,
									dialogPtr, 
									itemHit, 
									decimalPlaces, 
									decimalPlaces);
				
/*	Alert (kErrorAlert2ID, NIL);
	
	if (decimalPlaces >= 0)
		LoadDItemRealValue (dialogPtr, itemHit, theRealValue, decimalPlaces);
						
	SelectDialogItemText (dialogPtr, itemHit, 0, INT16_MAX);
*/
} 		// end "RealNumberErrorAlert"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void RealNumberErrorAlert
//
//	Software purpose:	The purpose of this routine is to notify the user 
//							that an illegal real number was entered into a dialog 
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
//	Coded By:			Larry L. Biehl			Date: 04/21/2006
//	Revised By:			Larry L. Biehl			Date: 04/21/2006

void RealNumberErrorAlert (
				double								theRealValue,
				DialogPtr							dialogPtr, 
				SInt16								itemHit, 
				SInt16								numberFDecimalDigits, 
				SInt16								numberEDecimalDigits)

{
	Alert (kErrorAlert2ID, NIL);
	
	if (numberFDecimalDigits >= 0)
		LoadDItemRealValue (dialogPtr, 
									itemHit, 
									theRealValue, 
									numberFDecimalDigits,
									numberEDecimalDigits);
						
	SelectDialogItemText (dialogPtr, itemHit, 0, INT16_MAX);

} 		// end "RealNumberErrorAlert"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetDialogItemDrawRoutine
//
//	Software purpose:	The purpose of this routine is to set the input draw
//							routine for the specified dialog and item number.
//
//	Parameters in:		The dialog pointer.
//							The dialog item number.
//							Address to the draw routine.
//
//	Parameters out:	Rectangle for the input item number.
//
// Value Returned:	None.			
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 07/22/1999
//	Revised By:			Larry L. Biehl			Date: 07/22/1999	

void SetDialogItemDrawRoutine (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber, 
				UserItemUPP							userItemProcPtr)

{
	Rect								theBox;
	

	SetDialogItemDrawRoutine (dialogPtr, itemNumber, &theBox, userItemProcPtr);
	
}		// end "SetDialogItemDrawRoutine" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetDialogItemDrawRoutine
//
//	Software purpose:	The purpose of this routine is to set the input draw
//							routine for the specified dialog and item number.
//
//	Parameters in:		The dialog pointer.
//							The dialog item number.
//							Address to the draw routine.
//
//	Parameters out:	Rectangle for the input item number.
//
// Value Returned:	None.			
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 05/20/1992
//	Revised By:			Larry L. Biehl			Date: 03/25/2017	

void SetDialogItemDrawRoutine (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber,
				Rect*									rectPtr, 
				UserItemUPP							userItemProcPtr)

{
	Handle							theHandle;
	SInt16							theType;
	

	GetDialogItem (dialogPtr, itemNumber, &theType, &theHandle, rectPtr);
	
	if (theType == kUserDialogItem)
		SetDialogItem (dialogPtr, itemNumber, theType, (Handle)userItemProcPtr, rectPtr);
	
			// Add a little more space to the top of the box to allow for the top
			// line of the box to be drawn. The rectPtr box is used to invalidate the
			// region around the box.
			
	rectPtr->top -= 2;		
	
}		// end "SetDialogItemDrawRoutine" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2012)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetDialogItemDrawRoutine
//
//	Software purpose:	The purpose of this routine is to set the input draw
//							routine for the specified dialog and item number.
//
//	Parameters in:		The dialog pointer.
//							The dialog item number.
//							Address to the draw routine.
//
//	Parameters out:	Rectangle for the input item number.
//
// Value Returned:	None.			
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 09/09/2001
//	Revised By:			Larry L. Biehl			Date: 09/28/2001	

void SetDialogItemDrawRoutine (
				DialogPtr							dialogPtr, 
				Rect*									rectPtr,
				SInt16								promptItemNumber, 
				SInt16								itemNumber,
				UserItemProcPtr					drawRoutinePtr,
				UserItemUPP*						userItemProcPtrPtr,
				SInt16								appearanceItemNumber,
				MenuHandle							inputPopupMenuHandle,
				SInt16								popupMenuSetting)

{
	ControlHandle						theControl;
	Handle								theHandle;
	MenuHandle							popupMenuHandle;
	OSErr									errorCode;
	SInt16								theType;
	Size									size;
	
		
	if (gAppearanceManagerFlag)
		{
		if (drawRoutinePtr != NULL)
			*userItemProcPtrPtr = NULL;
		
		errorCode = ::GetDialogItemAsControl (dialogPtr, appearanceItemNumber, &theControl);
		
		if (errorCode == noErr)
			{
					// The following is done to be sure that the popup menus that were loaded
					// at the beginning are used. When all dialogs are converted to take
					// advantage of the Appearance manager, setting preloaded popup menu handles
					// will not have to be done.
					
			errorCode = GetControlData (
									theControl, 
									0, 
									kControlPopupButtonMenuHandleTag, 
									sizeof (MenuHandle),
									(Ptr)&popupMenuHandle,
									&size);

			if (popupMenuHandle != inputPopupMenuHandle)
				{
				errorCode = SetControlData (
									theControl, 
									0, 
									kControlPopupButtonMenuHandleTag, 
									sizeof (MenuHandle),
									(Ptr)&inputPopupMenuHandle);
				
						// Dispose of the menu if it is not the one that we are keeping so
						// that we do not have memory leaks.
											
				if (errorCode == noErr && popupMenuHandle != NULL)
					{
					if (!gOSXFlag)
						DisposeMenu (popupMenuHandle);
						
//					#if TARGET_API_MAC_CARBON	
//						else		// gOSXFlag
//							ReleaseMenu (popupMenuHandle);
//					#endif	// TARGET_API_MAC_CARBON
						
					}		// end "if (errorCode == noErr && popupMenuHandle != NULL)"
					
				}		// end "if (popupMenuHandle != inputPopupMenuHandle)"
									
			SetControlValue (theControl, popupMenuSetting);
			popupMenuSetting = GetControlValue (theControl);
                           
         }		// end "if (errorCode == noErr)"
      
      if (promptItemNumber != 0)   
			HideDialogItem (dialogPtr, promptItemNumber);
			
//		if (appearanceItemNumber != itemNumber)
//			HideDialogItem (dialogPtr, itemNumber);
		
		}		// end "if (gAppearanceManagerFlag)"
			
	else		// !gAppearanceManagerFlag
		{
		if (drawRoutinePtr != NULL)
			*userItemProcPtrPtr = NewUserItemUPP (drawRoutinePtr);
			
		GetDialogItem (dialogPtr, itemNumber, &theType, &theHandle, rectPtr);
		
		if (*userItemProcPtrPtr != NULL)
			SetDialogItem (dialogPtr, 
									itemNumber, 
									theType, 
									(Handle)*userItemProcPtrPtr, 
									rectPtr);
		
//		if (appearanceItemNumber != itemNumber)
//			HideDialogItem (dialogPtr, appearanceItemNumber);
		
		}		// end "else !gAppearanceManagerFlag"
	
}		// end "SetDialogItemDrawRoutine" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetDItemValue
//
//	Software purpose:	The purpose of this routine is to get the string
//							for the input dialog window and item and convert the
//							string to a number.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			ClassifyDialog   in classify.c
//
//	Coded By:			Larry L. Biehl			Date: 01/28/2001
//	Revised By:			Larry L. Biehl			Date: 01/28/2001

void SetDItemValue (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber,
				SInt32								controlValue)

{
	ControlHandle						controlHandle;
	Handle								theHandle;
	Rect									theBox;
	
	OSErr									errorCode;
	SInt16								theType;
	

	GetDialogItem (dialogPtr, itemNumber, &theType, &theHandle, &theBox);
	
	if (theType & kControlDialogItem)
		{
		errorCode = ::GetDialogItemAsControl (dialogPtr, itemNumber, &controlHandle);
	
		if (errorCode == noErr)
			SetControlValue (controlHandle, controlValue);
		
		}		// end "if (theType & kControlDialogItem)"
	
}		// end "SetDItemValue"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetEntireImageIcon
//
//	Software purpose:	The purpose of this routine is to set the 
//							"Entire image button icon" in dialogs.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl					Date:	10/16/1989
//	Revised By:			Larry L. Biehl					Date: 10/17/1989

pascal void SetEntireImageIcon (
				DialogPtr									dialogPtr, 
				SInt16										itemNumber)

{
	Rect									theBox;
	
	Handle								iconHandle,
											theHandle;
											
	SInt16								theType;										
	
	
	if (dialogPtr != NULL && itemNumber)
		{
		GetDialogItem (dialogPtr, itemNumber, &theType, &theHandle, &theBox);
		
		if (!gEntireImageFlag || !gSelectionRectangleFlag)
			iconHandle = GetIcon (151);
			
		else		// entireImageFlag && gSelectionRectangleFlag 
			iconHandle = GetIcon (152);
		
		PlotIcon (&theBox, iconHandle);
		
		if (gEntireImageFlag && !gSelectionRectangleFlag)
			{
			RGBColor 		savedForeColor;
			
			GetForeColor (&savedForeColor);
		
			RGBForeColor (&gInactiveForeGroundColor);
			
			PlotIcon (&theBox, iconHandle);
		
			PenNormal ();
		
			RGBForeColor (&savedForeColor);
				
			}		// end "if (gEntireImageFlag && !gSelectionRectangleFlag)" 
			
		}		// end "if (dialogPtr != NULL && itemNumber)" 
		
}		// end "SetEntireImageIcon" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ShowDialogItems
//
//	Software purpose:	The purpose of this routine is to show the requested
//							continuous list of dialog items.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/22/1993
//	Revised By:			Larry L. Biehl			Date: 04/22/1993

void ShowDialogItems (
				DialogPtr							dialogPtr, 
				SInt16				     			startItem, 
				SInt16				     			lastItem)

{
	SInt16								index;
	
	
	if (dialogPtr != NULL)
		{
		for (index=startItem; index<=lastItem; index++)
			ShowDialogItem (dialogPtr, index);
		
		}		// end "if (dialogPtr != NULL)" 
	
}		// end "ShowDialogItems"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ShowDialogWindow
//
//	Software purpose:	The purpose of this routine is to center the dialog
//							horizontally and then show the dialog window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/02/1989
//	Revised By:			Larry L. Biehl			Date: 01/21/2001

void ShowDialogWindow (	
				DialogPtr							dialogPtr, 
				SInt16				     			dialogID, 
				Boolean								setUpFilterTableFlag)

{
	Point									where;
	
	SInt16								index;
	
	
	if (dialogPtr != NULL)
		{
		GetDlogOrigin (dialogID, &where);
		MoveWindow (GetDialogWindow(dialogPtr), where.h, where.v, FALSE);
		ShowWindow ( GetDialogWindow(dialogPtr) );
		SetPortDialogPort (dialogPtr);	
	
		}		// end "if (dialogPtr != NULL)" 
	
	if (setUpFilterTableFlag)
		{
				// Initialize dialog item list table for the filter procedure.	
	
		gDialogItemTableSet++;
	
		if (gDialogItemTableSet >= 0 && gDialogItemTableSet < 3)
			{
			gDialogItemDescriptorPtr = &gDialogItemListTable[
										gDialogItemTableSet * kDialogItemTableLength ];
									
			for (index=0; index<kDialogItemTableLength; index++)
				gDialogItemDescriptorPtr[index] = 0;
				
			}		// end "if (gDialogItemTableSet >= 0 && ..." 
			
		else		// gDialogItemTableSet < 0 || gDialogItemTableSet >= 3 
			gDialogItemDescriptorPtr = gDialogItemListTable;
			
		}		// end "if (setUpFilterTableFlag)" 
	
}		// end "ShowDialogWindow"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ShowHideDialogItems
//
//	Software purpose:	The purpose of this routine is to show
//							or hide the specifed dialog items dependant upon
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
//	Coded By:			Larry L. Biehl			Date: 08/29/1997
//	Revised By:			Larry L. Biehl			Date: 08/29/1997	

void ShowHideDialogItems (
				DialogPtr							dialogPtr, 
				SInt16				     			startItem, 
				SInt16				     			lastItem, 
				Boolean								showFlag)
				
{
	if (showFlag)	
		ShowDialogItems (dialogPtr, startItem, lastItem);
		
	else		// !showFlag 		
		HideDialogItems (dialogPtr, startItem, lastItem);
	
}		// end "ShowHideDialogItems"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ShowStatusDialogWindow
//
//	Software purpose:	The purpose of this routine is to center the dialog
//							horizontally and then show the dialog window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/15/1993
//	Revised By:			Larry L. Biehl			Date: 12/07/1999

void ShowStatusDialogWindow (
				SInt16		     					statusDialogID)

{
	if (gStatusDialogPtr != NULL)
		{
		ShowDialogWindow (gStatusDialogPtr, statusDialogID, kDoNotSetUpDFilterTable);
		DrawDialog (gStatusDialogPtr);
		CheckSomeEvents (activMask);
		
		}		// end "if (gStatusDialogPtr != NULL)" 
	
}		// end "ShowStatusDialogWindow"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateDialogOneColumnList
//
//	Software purpose:	The purpose of this routine is to update a one 
//							column list in a dialog window.  The routine will
//							be used to create a list for loading in a column
//							of channel numbers/descriptions and class names.
//
//	Functional description:	The steps in this function follow.
//
//			1) Get the location and area of the list.
//			2) Outline the area that the list will fall into.
//			3) Get a handle to the list information.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  None
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 01/13/1989
//	Revised By:			Larry L. Biehl			Date: 06/28/2003

void UpdateDialogOneColumnList (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber,
				ListHandle							listHandle)

{
	Handle								theHandle;
	
	Rect									rView;
	
	SInt16								theType;
	

	if (dialogPtr != NULL)
		{																
				// Get the display rectangle for the item to place the list into.		
				
		GetDialogItem (dialogPtr, itemNumber, &theType, &theHandle, &rView);
		
				// Outline the list area.															
				
		PenMode (patCopy);
		PenSize (1,1);
		InsetRect (&rView, -1, -1);
		FrameRect (&rView);
		
				// Draw the list.
				
		if (listHandle != NULL)
			{
			LUpdate ( GetPortVisibleRegion (GetDialogPort (dialogPtr), gTempRegion1), 
														gDialogListHandle );
			SetEmptyRgn (gTempRegion1);
			
			}		// end "if (listHandle != NULL)"
			
		}		// end "if (dialogPtr != NULL)"

}		// end "UpdateDialogOneColumnList" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateEntireIconButton
//
//	Software purpose:	The purpose of this routine is to update the
//							requested line column values based on the entire
//							image flag.
//
//	Parameters in:		displaySpecsPtr
//							fileInfoPtr
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			DialogLineColumnHits in dialogUtilities.c
//
//	Coded By:	Larry L. Biehl					Date:	10/26/2001
//	Revised By:	Larry L. Biehl					Date: 10/26/2001

void UpdateEntireIconButton (
				DialogSelectAreaPtr				dialogSelectAreaPtr,
				DialogPtr							dialogPtr)

{	
	Rect							theRect;
	Handle						theHandle;
	SInt16						theType;
	
			
	GetDialogItem (dialogPtr, 
							dialogSelectAreaPtr->entireIconItem, 
							&theType, 
							&theHandle, 
							&theRect);
	
	if (theType != kUserDialogItem)
		{
		ControlButtonContentInfo 		contentInfo;
		OSErr									osErr;
		ControlRef							theControl;
		
		SInt16								hilite,
												resID;
		

		if (!gEntireImageFlag || !gSelectionRectangleFlag)
			resID = 304;
			
		else		// entireImageFlag && gSelectionRectangleFlag 
			resID = 305;
				
		osErr = ::GetDialogItemAsControl (dialogPtr, 
														dialogSelectAreaPtr->entireIconItem, 
														&theControl);
		
		contentInfo.contentType = kControlIconResourceIDTag;
		
		osErr = GetBevelButtonContentInfo (theControl,
														&contentInfo);
		
		contentInfo.u.resID = resID;
		osErr = SetBevelButtonContentInfo (theControl,
														&contentInfo);
						
		hilite = 0;									
		if (gEntireImageFlag && !gSelectionRectangleFlag)
			hilite = 255;
			
		HiliteControl (theControl, hilite);
				
		}		// end "else theType == 'control item'"

}		// end "UpdateEntireIconButton" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateLineColumnValues
//
//	Software purpose:	The purpose of this routine is to update the
//							requested line column values based on the entire
//							image flag.
//
//	Parameters in:		displaySpecsPtr
//							fileInfoPtr
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			DialogLineColumnHits in dialogUtilities.c
//
//	Coded By:	Larry L. Biehl					Date:	10/12/1989
//	Revised By:	Larry L. Biehl					Date: 03/19/1999

void UpdateLineColumnValues (
				WindowInfoPtr						windowInfoPtr,
				WindowPtr							windowPtr, 
				DialogPtr							dialogPtr, 
				Rect*									theBoxPtr,
				SInt16								itemStart, 
				SInt32*								lineStart, 
				SInt32*								lineEnd, 
				SInt32*								columnStart, 
				SInt32*								columnEnd,
				Boolean								adjustToBaseImageFlag)

{	
	if (!gEntireImageFlag || gSelectionRectangleFlag )
		{
		if (ButtonDownEvent (theBoxPtr, TRUE))
			{
			if (!gEntireImageFlag)
				{
						// Change to entire image.													
						
				*lineStart = 1;
				*columnStart = 1;
				*lineEnd = windowInfoPtr->maxNumberLines;
				*columnEnd = windowInfoPtr->maxNumberColumns;
				
				gEntireImageFlag = TRUE;
				
				}		// end "if (!entireImageFlag)" 
				
			else		// gEntireImageFlag 
				{
						// Change to selected rectangle.											
						
				if (GetSelectionRectangle (
									windowPtr, 
									&gTempLongRect, 
									kDontClearSelectionArea, 
									kUseThreshold, 
									adjustToBaseImageFlag) )
					{			
					*lineStart = gTempLongRect.top;
					*lineEnd = gTempLongRect.bottom;
					*columnStart = gTempLongRect.left;
					*columnEnd = gTempLongRect.right;
				
					gEntireImageFlag = FALSE;
					
					}		// end "if (GetSelectionRectangle (..." 
				
				}		// end "else entireImageFlag" 
			
					// Make changes in the dialog items for the line and columns.		
					
			LoadDItemValue (dialogPtr, itemStart, (SInt32)*lineStart);
			LoadDItemValue (dialogPtr, itemStart+1, (SInt32)*lineEnd);
			LoadDItemValue (dialogPtr, itemStart+3, (SInt32)*columnStart);
			LoadDItemValue (dialogPtr, itemStart+4, (SInt32)*columnEnd);
			
					// Set default text selection to first edit text item					
					
			SelectDialogItemText (dialogPtr, itemStart, 0, INT16_MAX);
			
			InvalWindowRect (GetDialogWindow(dialogPtr), theBoxPtr);
			
			}		// end "if (ButtonDownEvent (theBoxPtr))" 
		
		}		// end "if (dialogPtr && itemNumber && (!gEntireImageFlag || ..." 
		
}		// end "UpdateLineColumnValues"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 UpdateOneColListSelections
//
//	Software purpose:	The purpose of this routine is to update the selections
//							in the one column list based on the start, end
//							and interval values.
//
//	Parameters in:		dialog list handle
//									
//
//	Parameters out:	None
//
// Value Returned:	the number of selections
//
// Called By:			ClassDialog   in projectUtilities.c
// 						ClassGroupDialog   in displayThematic.c
// 						ChannelsDialog   in multiSpecUtilities.c
//
//	Coded By:			Larry L. Biehl			Date: 05/16/1991
//	Revised By:			Larry L. Biehl			Date: 06/11/1998

SInt16 UpdateOneColListSelections (
				UInt16								numberInputVecItems, 
				SInt16								listStart, 
				SInt16								listEnd, 
				SInt16								listInterval)

{
	Cell									cell;
	SInt16								item,
											nextTrueSelection,
											numberSelections;
	
	
	LSetDrawingMode (FALSE, gDialogListHandle);
	cell.h = 0;
	cell.v = 0;
	
	for (item=1; item<listStart; item++)
		{
		LSetSelect (FALSE, cell, gDialogListHandle);
		cell.v++;
		
		}		// end "for ( item=0; item<..." 
	
	nextTrueSelection = listStart;
	for (item=listStart; item<=listEnd; item++)
		{
		if (item == nextTrueSelection)
			{
			LSetSelect (TRUE, cell, gDialogListHandle);
			nextTrueSelection += listInterval;
			
			}		// end "if (item == nextTrueSelection)" 
			
		else		// item == nextTrueSelection 
			LSetSelect (FALSE, cell, gDialogListHandle);
			
		cell.v++;
		
		}		// end "for ( item=0; item<..." 
	
	for (item=listEnd+1; item<=numberInputVecItems; item++)
		{
		LSetSelect (FALSE, cell, gDialogListHandle);
		cell.v++;
		
		}		// end "for ( item=0; item<..." 
			
	numberSelections = 
					(listEnd - listStart + listInterval)/listInterval;
	
			// Scroll to the hilited list item.												
	                  		
	LAutoScroll (gDialogListHandle);
		
	LSetDrawingMode (TRUE, gDialogListHandle);
	
	return (numberSelections);
						
}		// end "UpdateOneColListSelections"
