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
//	File:						SDisplayThematic.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			04/16/2020
//
//	Language:				C
//
//	System:					Linux, Macintosh and Windows Operating Systems
//
//	Brief description:	The procedures in this file control the display of
// 							thematic type image data in the image windows.
//
//	Diagram of MultiSpec routine calls for the procedures in the file.
//
//		DisplayThematicImage
//			LoadThematicDisplaySpecs
//				GetSelectedAreaInfo (in SSelectionUtility.cpp)
//
//			DisplayThematicDialog
//				LoadDItemValue	(MDialogUtilities.c)
//				ShowDialogWindow (ShowDialogWindow and SStubs.cpp)
//				CheckSomeEvents (MacOS routine, xUtilities.cpp, SStubs.cpp for Windows)
//				HiliteOKButton (in MDialogUtilities.c)
//				NumberDefault (in SUtilities.cpp)
//				GetDLogControl (in SUtilities.cpp)
//				ChangeDLogCheckBox (in SUtilities.cpp)
//				GetDItemValue (in SUtilities.cpp)
//				
//			HistogramThematicVector
//				UnlockAndDispose (in MMultiSpec.c)
//
//			CreatePalette		(in SPalette.cpp)
//
//			DisplayColorImage	(in SDisplay.cpp)
//
//			UpdateImageZoomControls	(in MControls.c)
//			UpdateImageWControls	(in MControls.c)
//			DrawScrollBars	(in MWindow.c)
//
//   Following is template for debugging
//   int numberChars2 = sprintf ((char*)gTextString2,
//                       " SDisThem::DisplayThematicImage (displaySpecsPtr): %d, %s",
//                        displaySpecsPtr,
//                        gEndOfLine);
//   ListString ((char*)gTextString2, numberChars2, gOutputTextH);
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
	#include "SDisplay_class.h"
	#include "SPalette_class.h"

#if defined multispec_wx
	#include "xDisplayThematicDialog.h"
	#include "xImageFrame.h"
	#include "xImageView.h"  
	#include "xMultiSpec.h"
#endif	// defined multispec_wx 
	
#if defined multispec_mac || defined multispec_mac_swift
	#define	IDC_Magnification				18
	#define	IDC_BackgroundColor			25
	#define	IDC_BackgroundColorChip		26
	#define	IDC_UseThresholdingFile		28
	#define	IDC_NumberClasses				30
	#define	IDC_NumberGroups				32
	#define	IDC_VectorOverlays			33
#endif	// defined multispec_mac || defined multispec_mac_swift
                             
#if defined multispec_win
	#include	"WImageView.h"
	#include	"WDisplayThematicDialog.h" 
	#include "WImageDoc.h" 
	#include "WImageFrame.h"
#endif	// defined multispec_win 
	 


SInt16					gAllSubsetClassGroupSelection;
								


			// Prototypes for routines in this file that are only called by		
			// other routines in this file.	  													
			
Boolean ClassGroupDialog (
				UInt32*								numberOutputItemsPtr,
				SInt16*								listPtr, 
				SInt16								minimumNumberItems,
				SInt16								numberInputItems, 
				SInt16								classGroupCode);      

Boolean DisplayThematicDialog (
				DisplaySpecsPtr					displaySpecsPtr);

PascalVoid DrawDisplayAllSubsetClassesGroupsPopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber);

PascalVoid DrawDisplayClassesGroupsPopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber);

Boolean HistogramThematicVector (
				DisplaySpecsPtr					displaySpecsPtr,
				Boolean								firstTimeFlag);

void LoadClassGroupDisplayInfo (
				DisplaySpecsPtr					displaySpecsPtr,
				FileInfoPtr							fileInfoPtr, 
				SInt16*								localClassGroupsPtr);

DisplaySpecsPtr LoadThematicDisplaySpecs (void);
								
void RemoveListCells (
				ListHandle							listHandle);

void UpdateLegendWidth (void);



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ClassGroupDialog
//
//	Software purpose:	The purpose of this routine is to display a dialog
//							with a list of the available classes that the user
//							can select from. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			ClassifyDialog   in SClassifyDialogs.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/15/1989
//	Revised By:			Larry L. Biehl			Date: 04/05/1996	

Boolean ClassGroupDialog (
				UInt32*								numberOutputItemsPtr, 
				SInt16*								listPtr, 
				SInt16								minimumNumberItems,
				SInt16								numberInputItems, 
				SInt16								classGroupCode)

{
	Cell									cell;
	
	DialogPtr							dialogPtr;
	
	SInt16								item,
											itemNumber,
											index,
											maxIndex;

	Boolean								returnFlag;
	
	
			// Check input variables.															
			
	if (classGroupCode < 1 || classGroupCode > 2)
																				return (FALSE);
		
			// Get the modal dialog for the reformat specification					
				
	dialogPtr = LoadRequestedDialog (kChannelSpecificationID, kCopyScrap, 1, 2);
	if (dialogPtr == NULL)													return (FALSE);
		
			// Get a handle to a one column list.											
			
	gDialogListHandle = GetOneColumnDialogListHandle (dialogPtr, 5);
	
			//	Load address of routine that will load the class or group list.	
			
	SetDialogItemDrawRoutine (dialogPtr, 5, gCreateOneColumnList1Ptr);
	
			//	Item 6: Load title of class list dialog.									
			//	Item 7: Load title for class list.											
	
	if (classGroupCode == kClassDisplay)
		{
		SetWTitle (GetDialogWindow (dialogPtr), "\pSelect Spectral Classes");
		LoadDItemString (dialogPtr, 7, (Str255*)"\pClass List");
		
		}	// end "if (classGroupCode == kClassDisplay)" 
		
	if (classGroupCode == kGroupDisplay)
		{
		SetWTitle (GetDialogWindow (dialogPtr), "\pSelect Information Groups");
		LoadDItemString (dialogPtr, 7, (Str255*)"\pGroup List");
		
		}	// end "if (classGroupCode == kGroupDisplay)" 
	
			// Initialize the class range text edit items.								
			
	LoadDItemValue (dialogPtr, 9, (SInt32)1);
	LoadDItemValue (dialogPtr, 11, (SInt32)numberInputItems);
	LoadDItemValue (dialogPtr, 13, (SInt32)1);
	SetDLogControlHilite (dialogPtr, 14, 0);
		
			// Center the dialog and then show it.											
			
	ShowDialogWindow (
					dialogPtr, kChannelSpecificationID, kDoNotSetUpDFilterTable);
	
			// Make certain that the graph port is set to the channels dialog		
			// window.																				
			
	SetPortDialogPort (dialogPtr);
	
			// Continue if there is a valid dialog list handle.						
	
	returnFlag = FALSE;	
	if (gDialogListHandle != NULL)
		{
				// Load the class names into the list.										
				
		LSetDrawingMode (FALSE, gDialogListHandle);
		
		if (classGroupCode == kClassDisplay)
			LoadThematicClassNamesIntoList (gDialogListHandle);
		
		if (classGroupCode == kGroupDisplay)
			LoadThematicGroupNamesIntoList (gDialogListHandle);
		
				// Hilite the currently selected items.									
				
		cell.h = 0;
		index = 0;
		maxIndex = *numberOutputItemsPtr - 1;
		itemNumber = 1;
		for (item=0; item<numberInputItems; item++)
			{
			if (listPtr[index] == itemNumber)
				{
				cell.v = item;
				LSetSelect (TRUE, cell, gDialogListHandle);
				
				if (index < maxIndex)
					index++;
					
				}	// end "if (index < *numberOutputFeatures && ...)" 
				
			itemNumber++;
				
			}	// end "for (item=0; item<numberInputItems; item++)" 
			
				// Set the number of selected items.										
				
		LoadDItemValue (dialogPtr, 16, (SInt32)index+1);
			
				// Turn list drawing mode back on.											
	  				
		LSetDrawingMode (TRUE, gDialogListHandle);
		
		LUpdate (GetPortVisibleRegion (
							GetDialogPort (dialogPtr), gTempRegion1), gDialogListHandle);
		SetEmptyRgn (gTempRegion1);
		
				// Handle modal dialog selections for one column class list.		
			
		returnFlag = ModalOneColListDialog (dialogPtr, 
														numberInputItems,
														numberOutputItemsPtr, 
														listPtr, 
														minimumNumberItems);
		
		if (gDialogListHandle != NULL)  
			LDispose (gDialogListHandle);
		gDialogListHandle = NULL;
		
		}	// end "if (gDialogListHandle != NULL)" 
		
	CloseRequestedDialog (dialogPtr, kDoNotSetUpDFilterTable);
	
	return (returnFlag);
	
}	// end "ClassGroupDialog" 
#endif	// defined multispec_mac



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DisplayColorThematicImage
//
//	Software purpose:	The purpose of this routine is to display the
//							thematic classes in color in a window on the screen.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None				
// 
// Called By:			DisplayColorImage
//
//	Coded By:			Larry L. Biehl			Date: 12/19/1989
//	Revised By:			Larry L. Biehl			Date: 04/15/2019

void DisplayColorThematicImage (
				DisplaySpecsPtr					displaySpecsPtr, 
				FileInfoPtr							fileInfoPtr, 
				HPtr									offScreenBufferPtr,
				UInt32								pixRowBytes, 
				PixMapHandle						savedPortPixMapH, 
				PixMapHandle						offScreenPixMapH, 
				LongRect*							rectPtr,
				LCToWindowUnitsVariables* 		lcToWindowUnitsVariablesPtr)
	
{
	LongRect								longSourceRect;
	
	FileIOInstructionsPtr			fileIOInstructionsPtr;
	
	HUCharPtr							bufferPtrEnd,
											ioBufferPtr,
											ioBuffer1Ptr,
											offScreenLinePtr,
											offScreenPtr,
											tiledBufferPtr;
	
	HUInt16Ptr							ioBuffer2Ptr;
	unsigned char*						symbolToPalettePtr;
	
	SInt32								displayBottomMax;
	
	UInt32								columnInterval,
											line,
											lineCount,
											lineEnd,
											lineInterval,
											longNumberSamples,
											numberSamples,
											numberTileBytes;
	
	SInt16								errCode,
											offscreenProcedure;
											
	Boolean								updateIOBufferUpdateFlag;
	
	
			// Initialize local variables.													
		
	lineEnd = displaySpecsPtr->lineEnd;
	lineInterval = displaySpecsPtr->lineInterval;
	columnInterval = displaySpecsPtr->columnInterval;
	numberSamples = displaySpecsPtr->columnEnd - 
															displaySpecsPtr->columnStart + 1;	
	ioBufferPtr = NULL;
	tiledBufferPtr = NULL;
	
			// Get display bottom maximum of image window								
	
	longSourceRect = *rectPtr;	
	displayBottomMax = longSourceRect.bottom;
		
			// Determine the appropriate routine to use to draw into the			
			// the offscreen map.																
			
	offscreenProcedure = 0;
	if (fileInfoPtr->numberBytes == 1)
		{
		//#if defined multispec_wx
		//	pixRowBytes = pixRowBytes/3;
		//#endif	// end "defined multispec_wx"

		if (!fileInfoPtr->asciiSymbols)
			{
			if (columnInterval > 1)
				{
				if (displaySpecsPtr->paletteOffset == 1)
					offscreenProcedure = 1;
					
				else if (displaySpecsPtr->paletteOffset == 0)
					offscreenProcedure = 5;
					
				}	// end "if (columnInterval > 1 || ...)" 
		
			else	// columnInterval == 1
				{
				if (displaySpecsPtr->paletteOffset == 1)
					offscreenProcedure = 2;
					
				else if (displaySpecsPtr->paletteOffset == 0)
					offscreenProcedure = 6;
					
				}	// end "else columnInterval == 1" 
				
			}	// end "if (!fileInfoPtr->asciiSymbols)" 
			
		else	// fileInfoPtr->asciiSymbols 
			offscreenProcedure = 4;
			
		}	// end "if (fileInfoPtr->numberBytes == 1)" 
		
	else if (fileInfoPtr->numberBytes == 2)
		{
		if (!fileInfoPtr->asciiSymbols)
			{
			if (displaySpecsPtr->paletteOffset == 1)
				offscreenProcedure = 3;
					
			else if (displaySpecsPtr->paletteOffset == 0)
				offscreenProcedure = 8;
				
			}	// end "if (!fileInfoPtr->asciiSymbols)" 
			
		else	// fileInfoPtr->asciiSymbols 
			offscreenProcedure = 7;
			
		offscreenProcedure = 7;
		
			
		}	// end "else if (fileInfoPtr->numberBytes == 2)" 
		
				// Make sure the symbol to palette pointer is up to date.
																							
	if (displaySpecsPtr->symbolToPaletteEntryH != NULL)
		{
		if (!HistogramThematicVector (displaySpecsPtr, FALSE))
			offscreenProcedure = 0;
			
		}	// end "if (displaySpecsPtr->symbolToPaletteEntryH != NULL)"
		
	if (offscreenProcedure == 0)
																							return;
		
				// Get pointer to symbol-to-palette vector if needed.					
	
	symbolToPalettePtr = NULL;
	if (fileInfoPtr->asciiSymbols || fileInfoPtr->numberBytes == 2)
		{	
		if (displaySpecsPtr->symbolToPaletteEntryH == NULL)
																							return;	
																							
		symbolToPalettePtr = (unsigned char*)GetHandlePointer (
										displaySpecsPtr->symbolToPaletteEntryH,
										kLock);
						
		}	// end "else fileInfoPtr->asciiSymbols || ..." 

			// Get handle to block of memory to use as file IO buffer for the		
			// thematic data.	Only get a buffer if the column interval is more	
			// than one or the number of bytes per data value is more than 1.		
	
	if (offscreenProcedure != 2 && offscreenProcedure != 6)
		{
		ioBufferPtr = (HUCharPtr)MNewPointer (
								(SInt32)numberSamples * fileInfoPtr->numberBytes);
	
				// If handle is NULL, exit the routine.									
			
		if (ioBufferPtr == NULL)
																							return;
																						
		updateIOBufferUpdateFlag = FALSE;
		
		ioBuffer2Ptr = (HUInt16Ptr)ioBufferPtr;
		
		}	// end "if (offscreenProcedure != 2 && offscreenProcedure != 6)" 
		
	else	// offscreenProcedure == 2 || offscreenProcedure == 6 
		updateIOBufferUpdateFlag = TRUE;     
		
	numberTileBytes = GetSetTiledIOBufferBytes (gImageLayerInfoPtr,
																	gImageFileInfoPtr, 
																	NULL, 
																	1,
																	NULL);
																	
	if (numberTileBytes > 0)
		{
		tiledBufferPtr = (HUCharPtr)MNewPointer (numberTileBytes);
		
			// Set up the general parameters in the File IO Instructions structure.

		if (tiledBufferPtr == NULL)
			{
			if (!updateIOBufferUpdateFlag)
				CheckAndDisposePtr (ioBufferPtr);
																								return;
				
			}	// end "if (tiledBufferPtr == NULL)"
														
		}	// end "if (numberTileBytes > 0)" 
		 
	SetUpGeneralFileIOInstructions (&gFileIOInstructions[0],
												gImageWindowInfoPtr,	
												gImageLayerInfoPtr,
												gImageFileInfoPtr,
												1,
												NULL,
												ioBufferPtr,
												ioBufferPtr,
												tiledBufferPtr,
												0,
												kDoNotPackData,
												kDoNotForceBISFormat,	
												kDoNotForceBytes,
												&fileIOInstructionsPtr);  
				
			// Draw the image in the image window. Note that Windows DIB
			// images are loaded with first line at the end of the bitmap 
			// to last line at the beginning of the bitmap.
					                             
	#if defined multispec_mac || defined multispec_wx                                   
		offScreenLinePtr = (HUCharPtr)offScreenBufferPtr;
	#endif	// defined multispec_mac || defined multispec_wx
				
	#if defined multispec_win 
		UInt32 numberLines = 
					(lineEnd - displaySpecsPtr->lineStart + lineInterval)/lineInterval;                  
		offScreenLinePtr = (HUCharPtr)(offScreenBufferPtr + (numberLines-1)*pixRowBytes);
	#endif	// defined multispec_win  
	
	if (updateIOBufferUpdateFlag)
		ioBufferPtr = offScreenLinePtr; 
	
			// Draw the thematic image in the image window.								
			                                           
	bufferPtrEnd = &ioBufferPtr[numberSamples * fileInfoPtr->numberBytes];
	
	SetUpFileIOInstructions (fileIOInstructionsPtr,
										NULL,
										displaySpecsPtr->lineStart,
										lineEnd,
										lineInterval,
										displaySpecsPtr->columnStart,
										displaySpecsPtr->columnEnd,
										columnInterval,
										1,
										NULL,
										kSetSpecialBILFlagFalse);
					
			// Intialize the nextTime variable to indicate when the next check	
			// should occur for a command-.													
			
	gNextTime = TickCount () + kDisplayTimeOffset;
					
	lineCount = 0;
	
	for (line=displaySpecsPtr->lineStart; line<=lineEnd; line+=lineInterval)
		{
		errCode = GetLine (fileIOInstructionsPtr,
									fileInfoPtr,
									line,
									0,
									(UInt32)displaySpecsPtr->columnStart,
									(UInt32)displaySpecsPtr->columnEnd,
									&longNumberSamples,
									(HUCharPtr)ioBufferPtr);

		if (errCode != noErr) 
			break;
		
		else	// errCode == noErr 
			{
			offScreenPtr = offScreenLinePtr;
	
					// Draw the line of data; switch to the appropriate section.	
			
			switch (offscreenProcedure)
				{	
				case 1:
							// Case for:															
							//		column interval > 1											
							// 	palette offset is 1											
							// The data was copied into a buffer; the requested 		
							// data will be copied to the offscreen map.					
							// Add one to data values to allow for white being the 	
							// first item in the palette.										
					
					for (ioBuffer1Ptr=ioBufferPtr; 
							ioBuffer1Ptr<bufferPtrEnd;
							ioBuffer1Ptr+=columnInterval)
						{
						*offScreenPtr = *ioBuffer1Ptr;
						offScreenPtr++;
	
						}	// end "for (ioBuffer1Ptr=(char*)ioBufferPtr; ..."
					break;
					
				case 2:
							// Case for:															
							//		column interval == 1											
							// 	palette offset is 1											
							// The data was already copied directly into 				
							// offscreen map.  Add one to data values to allow for 	
							// white being the first item in the palette.
							
						bufferPtrEnd = (HUCharPtr)&offScreenPtr[numberSamples];
						for (; offScreenPtr<bufferPtrEnd; offScreenPtr++)
							(*offScreenPtr)++;
					break;
				/*					
				case 3:
							// This loop will draw the image lines for two byte data.
							// The palette offset is 1.										
							// Note! Any values larger than 255 will wrap around. 	
						
					for (sample=0; sample<numberSamples; sample+=columnInterval)
						{
						*offScreenPtr = (UInt8)ioBuffer2Ptr[sample];
						(*offScreenPtr)++;
						offScreenPtr++;
							
						}	// end "for (sample=0;..." 
					break;
				*/					
				case 4:
							// This loop will draw the image lines for one byte data	
							// in which ascii symbols represent the classes.										

					bufferPtrEnd = ioBufferPtr + numberSamples;
					for (ioBuffer1Ptr=ioBufferPtr; 
							ioBuffer1Ptr<bufferPtrEnd;
							ioBuffer1Ptr+=columnInterval)
						{
						*offScreenPtr = symbolToPalettePtr[*ioBuffer1Ptr];
					
						offScreenPtr++;
	
						}	// end "for (ioBuffer1Ptr=(char*)ioBufferPtr;..." 
					break;
					
				case 5:
							// Case for:															
							//		column interval > 1											
							// 	palette offset is 0											
							// The data was copied into a buffer; the requested 		
							// data will be copied to the offscreen map.											

						for (ioBuffer1Ptr=ioBufferPtr; 
								ioBuffer1Ptr<bufferPtrEnd;
								ioBuffer1Ptr+=columnInterval)
							{
							*offScreenPtr = *ioBuffer1Ptr;
						
							offScreenPtr++;
		
							}	// end "for (ioBuffer1Ptr=..." 
					break;
					
				case 6:
							// Case for:															
							//		column interval == 1											
							// 	palette offset is 0											
							// The data was already copied directly into 				
							// offscreen map.  													
					break;
					
				case 7:
							// This loop will draw the image lines for two byte data	
							// in which ascii symbols represent the classes.			
							// Note! Any classes larger than 255 will wrap around. 	
							
					for (ioBuffer2Ptr=(HUInt16Ptr)ioBufferPtr; 
							ioBuffer2Ptr<(HUInt16Ptr)bufferPtrEnd;
							ioBuffer2Ptr+=columnInterval)
						{
						*offScreenPtr = symbolToPalettePtr[*ioBuffer2Ptr];
					
						offScreenPtr++;
	
						}	// end "for (ioBuffer1Ptr=(char*)ioBufferPtr;..." 
					break;

				}	// end "switch (offscreenProcedure)"
			
					// Check if user wants to exit drawing
			
			lineCount++;
			if (TickCount () >= gNextTime)
				{
				#if defined multispec_wx
					displaySpecsPtr->updateEndLine = lineCount;
				#endif
				
				longSourceRect.bottom = lineCount;
				if (!CheckSomeDisplayEvents (gImageWindowInfoPtr,
														displaySpecsPtr,
														lcToWindowUnitsVariablesPtr,
														savedPortPixMapH,
														offScreenPixMapH,
														&longSourceRect,
														displayBottomMax))
					break;
				
				#if defined multispec_wx
					displaySpecsPtr->updateStartLine = lineCount;
				#endif
				/*
				#if defined multispec_wxlin
							// Get the bitmap raw data pointer again. It may have changed.

					offScreenLinePtr = (unsigned char*)gImageWindowInfoPtr->imageBaseAddressH;
					offScreenLinePtr += (lineCount-1) * pixRowBytes;
				#endif
				*/
				}	// end "if (TickCount () >= gNextTime)"
			
					// Update the offscreen pointer for the next line and update	
					// the pointer for the IO buffer if needed.
					                             
			#if defined multispec_mac || defined multispec_wx
				offScreenLinePtr += pixRowBytes; 
			#endif	// defined multispec_mac || defined multispec_wx
					
			#if defined multispec_win 
				offScreenLinePtr -= pixRowBytes; 
			#endif	// defined multispec_win  							
					                           
			if (updateIOBufferUpdateFlag)
				ioBufferPtr = offScreenLinePtr;

			}	// end "if (errCode == noErr)" 
			
		}	// end "for (line=displaySpecsPtr->lineStart; ..." 
	
	CloseUpFileIOInstructions (fileIOInstructionsPtr, NULL);	
		
			// Set up return for inSourceRect to indicate if last few lines need	
			// to be drawn																			
	
	rectPtr->top = longSourceRect.top;
	
	if (longSourceRect.bottom == -1)
		rectPtr->bottom = -1;
	else	// longSourceRect.bottom != -1 
		rectPtr->bottom = lineCount;
	
	#if defined multispec_wx
		displaySpecsPtr->updateEndLine = lineCount;
	#endif

	CloseUpGeneralFileIOInstructions (fileIOInstructionsPtr);
	
	CheckAndDisposePtr (tiledBufferPtr);
	
	if (!updateIOBufferUpdateFlag)
		CheckAndDisposePtr (ioBufferPtr);
	
	CheckAndUnlockHandle (displaySpecsPtr->symbolToPaletteEntryH);
	
}	// end "DisplayColorThematicImage"  
												


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean DisplayThematicDialog
//
//	Software purpose:	The purpose of this routine is to handle the modal
//							dialog for confirming the Thematic image display 
//							specification.  The modal dialog template is in the 
//							resource file.  The default specification comes from 
//							the Thematic image file specification.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/19/1989
//	Revised By:			Larry L. Biehl			Date: 10/22/2018

Boolean DisplayThematicDialog (
				DisplaySpecsPtr					displaySpecsPtr)

{
	Boolean								returnFlag;
	
	#if defined multispec_mac
		DialogSelectArea					dialogSelectArea;
		
		Rect									displayBox,
												paletteBox,
												theBox,
												theColorBox;
											
		double								magnification,
												saveMagnification;
		
		RGBColor								newRGB;
		
		DialogPtr							dialogPtr;
		FileInfoPtr							fileInfoPtr;

		SInt16								*localClassGroupsPtr;
										
		UserItemUPP							drawBackgroundColorBoxPtr,
												drawDisplayAllSubsetClassesGroupsPopUpPtr,
												drawDisplayClassesGroupsPopUpPtr,
												drawPalettePopUpPtr;
		
		WindowInfoPtr						windowInfoPtr;
											
		Handle								localClassGroupsHandle,
												okHandle,
												theHandle;
		
		SInt32								interval,
												numberClasses,
												numberGroups,
												theNum;
		
		UInt32								localDisplayClasses,
												localDisplayGroups;
		
		SInt16								itemHit,
												localAllSubsetClass,
												localAllSubsetGroup,
												theType;
											
		Boolean								includeVectorOverlaysFlag,
												localPaletteUpToDateFlag,
												modalDone,
												paletteChangedFlag,
												showLegendFlag,
												tempFlag,
												thresholdFileFlag;
		
		
				// Initialize local variables.	 												
		
		localClassGroupsHandle = NULL;
			
				// Handle to file information should already be locked.  Get pointer 
				// to file information.																
		
		fileInfoPtr = gImageFileInfoPtr;
		windowInfoPtr = gImageWindowInfoPtr;
			
				// Get the modal dialog for the image display specification				
		
		dialogPtr = LoadRequestedDialog (
											kThematicDisplaySpecificationID, kCopyScrap, 1, 2);
		if (dialogPtr == NULL)													
																						return (FALSE);
			
				// Intialize local user item proc pointers.	
				
		drawBackgroundColorBoxPtr = NewUserItemUPP (DrawColorBoxInDialogWindow);
		drawDisplayAllSubsetClassesGroupsPopUpPtr = 
										NewUserItemUPP (DrawDisplayAllSubsetClassesGroupsPopUp);
		drawDisplayClassesGroupsPopUpPtr =
												NewUserItemUPP (DrawDisplayClassesGroupsPopUp);
		drawPalettePopUpPtr = NewUserItemUPP (DrawPalettePopUp);
		
				// Get pointer to local class to group list .
				
		localClassGroupsHandle = MNewHandle (
						MAX (fileInfoPtr->numberClasses, fileInfoPtr->numberGroups)  *
																					2 * sizeof (SInt16));
		if (localClassGroupsHandle == NULL)
			{
			DisposeDialog (dialogPtr);
																						return (FALSE);
			
			}	// end "if (localClassGroupsHandle == NULL)"
		
		localClassGroupsPtr = (SInt16*)GetHandlePointer (
													localClassGroupsHandle, kLock);
													
		DisplayThematicDialogInitialize (dialogPtr,
														displaySpecsPtr,
														windowInfoPtr,
														fileInfoPtr,
														&dialogSelectArea,
														&saveMagnification,
														&gClassGroupSelection,
														&paletteChangedFlag,
														&localPaletteUpToDateFlag,
														localClassGroupsPtr,
														&localDisplayClasses,
														&localDisplayGroups,
														&localAllSubsetClass,
														&localAllSubsetGroup,
														&gClassSelection,
														&gCurrentSelectedColor,
														&showLegendFlag,
														&thresholdFileFlag,
														&numberClasses,
														&numberGroups,
														&includeVectorOverlaysFlag);
												
				// Save handle for the OK button for use later.								
				
		GetDialogItem (dialogPtr, 1,  &theType, &okHandle, &theBox);											

				// To entire image icon.															
				//	Selected display area															
				
		LoadLineColumnItems (&dialogSelectArea, 
										dialogPtr, 
										kInitializeLineColumnValues, 
										kIntervalEditBoxesExist,
										1);
		
		SetDialogItemDrawRoutine (
									dialogPtr, 20, &paletteBox, drawPalettePopUpPtr);
									
		SetDialogItemDrawRoutine (dialogPtr, 22, drawDisplayClassesGroupsPopUpPtr);			
		
		SetDialogItemDrawRoutine (
			dialogPtr, 24, &displayBox, drawDisplayAllSubsetClassesGroupsPopUpPtr);
		
		SetDialogItemDrawRoutine (dialogPtr, 26, &theColorBox, drawBackgroundColorBoxPtr);				
		
		SetDLogControl (dialogPtr, 27, showLegendFlag);				
		
		SetDLogControl (dialogPtr, 28, thresholdFileFlag);
	
				// Center the dialog and then show it.
				
		ShowDialogWindow (
						dialogPtr, kThematicDisplaySpecificationID, kSetUpDFilterTable);
			
		gDialogItemDescriptorPtr[18] = kDItemReal;
		
				// Set default text selection to first edit text item						
				
		SelectDialogItemText (dialogPtr, 10, 0, INT16_MAX);
		
		modalDone = FALSE;
		itemHit = 0;
		do 
			{
			ModalDialog (gProcessorDialogFilterPtr, &itemHit);
			if (itemHit > 2)
				{
						// If itemHit was a number item, check for bad values.			
						// If itemHit was a radio button make appropriate control		
						// settings to indicate to the user the present selection.		
						// Get the handle to the itemHit.										
						// For number value items, get the string and convert it to		
						// a number.																	

				GetDialogItem (dialogPtr, itemHit, &theType, &theHandle, &theBox);
				if (theType == 16)
					{
					GetDialogItemText (theHandle, gTextString);	
					StringToNum (gTextString, &theNum);
					
					}	// end "if (theType == 16)" 
					
				switch (itemHit)
					{
					case 4:				// Entire area to selected area switch.
					case 35:				// Entire area to selected area switch. (Appearance Manager)			
					case 10:				//	 Display start line of image  
					case 11:				//	 Display end line of image  
					case 12:				//	 Display line interval of image  
					case 13:				//	 Display start column of image  
					case 14:				//	 Display end column of image  
					case 15:				//	 Display column interval of image  
						DialogLineColumnHits (&dialogSelectArea,
														dialogPtr, 
														itemHit,
														theHandle,
														theNum);
						break;
						
					case 18:				// Magnification	
						magnification = GetDItemRealValue (dialogPtr, 18);
						if (magnification > gMaxMagnification)
							RealNumberErrorAlert (saveMagnification, dialogPtr, 18, 3);
						if (magnification > gMinMagnification && 
																magnification <= gMaxMagnification)
							saveMagnification = magnification;
						break;
						
					case 20:			// Palette Popup menu. 			
						SetUpPalettePopUpMenu (dialogPtr,
														gPopUpPaletteMenu,
														fileInfoPtr->format,
														fileInfoPtr->ancillaryInfoformat,
														fileInfoPtr->numberClasses,
														fileInfoPtr->classDescriptionH, 
														fileInfoPtr->colorTableOffset, 
														displaySpecsPtr, 
														abs (gClassGroupSelection),
														gPaletteSelection);
		
						itemHit = StandardPopUpMenu (dialogPtr, 
																19, 
																20, 
																gPopUpPaletteMenu, 
																gPaletteSelection, 
																kPopUpPaletteMenuID);
														
						if (itemHit == kFalseColors)
							{
							HiliteControl ((ControlHandle)okHandle, 255);
							
							if (FalseColorPaletteDialog ())
								localPaletteUpToDateFlag = FALSE;
								
							else	// !FalseColorPaletteDialog ()
								itemHit = 0;
								
							HiliteControl ((ControlHandle)okHandle, 0);
							
							}	// end "if (itemHit == kFalseColors)"
							
						if (itemHit != 0)
							{
									// Check if palette was changed.								
									
							if (gPaletteSelection != itemHit)
								paletteChangedFlag = TRUE;
								
							gPaletteSelection = itemHit;
							
							}	// end "if (itemHit != 0)" 
		
								// Make certain that the correct label is drawn in the	
								// palette pop up box.												
						
						InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
						break;
						
					case 22:			// Display spectral classes/information groups 		
										// Popup menu. 												
						
						if (gClassGroupSelection < 0)
							break;
							
						itemHit = StandardPopUpMenu (dialogPtr, 
																21, 
																22, 
																gPopUpClassGroupMenu, 
																gClassGroupSelection, 
																kPopUpClassGroupMenuID);
														
						if (itemHit != 0)
							{
									// Check if there is a change.								
									
							if (gClassGroupSelection != itemHit)
								{
									// Change class/group list display if needed to 		
									// reflect a change in class or group display.			
									
								if (itemHit == kClassDisplay)
									gClassSelection = localAllSubsetClass;
									
								else	// itemHit != kClassDisplay 
									gClassSelection = localAllSubsetGroup;
									
								InvalWindowRect (GetDialogWindow (dialogPtr), &displayBox);
								
										// Change palette menu if needed to indicate			
										// change in class or group display.					
										
								if (!paletteChangedFlag)
									{
									if (itemHit == kClassDisplay)
										gPaletteSelection = 
													displaySpecsPtr->thematicClassPaletteType;
									
									else	// itemHit == kGroupDisplay 
										gPaletteSelection = 
													displaySpecsPtr->thematicGroupPaletteType;
													
									InvalWindowRect (GetDialogWindow (dialogPtr), &paletteBox);
									
									}	// end "if (!paletteChangedFlag)" 
						
								}	// end "if (gClassGroupSelection != itemHit)" 
								
							gClassGroupSelection = itemHit;
							
							}	// end "if (itemHit != 0)" 
		
								// Make certain that the correct label is drawn in the	
								// channel pop up box.												
						
						InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
						break;
						
					case 24:		// Display subset of classes or groups. 				
						if (gClassSelection < 0)
							break;
							
						itemHit = StandardPopUpMenu (dialogPtr, 
																23, 
																24, 
																gPopUpAllSubsetMenu, 
																gClassSelection, 
																kPopUpAllSubsetMenuID);
						if (itemHit == 2)
							{
									// Subset of classes to be used.								
									
							HiliteControl ((ControlHandle)okHandle, 255);
							
							if (gClassGroupSelection == kClassDisplay)
								{	
								tempFlag = ClassGroupDialog (
													&localDisplayClasses,
													localClassGroupsPtr,
													1,
													fileInfoPtr->numberClasses, 
													kClassDisplay);
									
								if (localDisplayClasses == fileInfoPtr->numberClasses)
									itemHit = 1;
									
								}	// end "if (gClassGroupSelection == kClassDisplay)" 
							
							else	// gClassGroupSelection != kClassDisplay 
								{	
								tempFlag = ClassGroupDialog (
												&localDisplayGroups,
												&localClassGroupsPtr[fileInfoPtr->numberClasses],
												1,
												fileInfoPtr->numberGroups, 
												kGroupDisplay);
									
								if (localDisplayGroups == fileInfoPtr->numberGroups)
									itemHit = 1;
									
								}	// end "else gClassGroupSelection != kClassDisplay" 
								
							if (tempFlag)
								localPaletteUpToDateFlag = FALSE;
									
							HiliteControl ((ControlHandle)okHandle, 0);
								
							}	// end "if (itemHit == 2)" 
						
						if (itemHit != 0)
							{
							gClassSelection = itemHit;
							
							if (gClassGroupSelection == kClassDisplay)
								localAllSubsetClass = itemHit;
								
							else	// gClassGroupSelection != kClassDisplay 
								localAllSubsetGroup = itemHit;
							
							}	// end "if (itemHit != 0)" 
		
								// Make certain that the correct label is drawn in the	
								// class pop up box.													
						
						InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
						break;
						
					case 25:
						if (SelectColor (3, 
													&gCurrentSelectedColor, 
													&newRGB))
							{
							gCurrentSelectedColor = newRGB;
							InvalWindowRect (GetDialogWindow (dialogPtr), &theColorBox);
								
							}	// end "if (SelectColor (3, ..." 
							
						break;
						
					case 27:			// Check box for 'Display class legend'.	
					case 28:			// Check box for 'Use thresholding file'.	
						ChangeDLogCheckBox ((ControlHandle)theHandle);
						break;
							
					}	// end "switch (itemHit)" 
					
				}	// end "if (itemHit > 2)" 
				
			else	// itemHit <= 2 
				{
						// If item hit is 1, check if display line-column values make 	
						// sense.  If they don't, sound an alert and make item hit 		
						// equal to 0 to allow user to make changes.							
						
				if (itemHit == 1)
					itemHit = CheckLineColValues (
											&dialogSelectArea,
											displaySpecsPtr->lineStart,
											displaySpecsPtr->columnStart,
											dialogPtr);

				if (itemHit == 1)
					{
					interval = GetDItemValue (dialogPtr, 12);
					itemHit = CheckNumberDisplayLines (
										dialogSelectArea.lineStart, 
										dialogSelectArea.lineEnd, 
										interval,
										dialogPtr, 
										10);
											
					}	// end "if (itemHit == 1)" 
									
				if (itemHit == 1)
					{
					interval = GetDItemValue (dialogPtr, 15);											
					itemHit = CheckNumberDisplayColumns (
											(UInt32)dialogSelectArea.columnStart, 
											(UInt32)dialogSelectArea.columnEnd, 
											(UInt32)interval,
											displaySpecsPtr->pixelSize,
											2,
											1, 
											dialogPtr, 
											13);
					
					}	// end "if (itemHit == 1)" 							
					
				if	(itemHit == 1)      // User selected OK for information 
					{
					modalDone = TRUE;    
														
							// Get the Display legend flag.											
				
					Boolean showLegendFlag = GetDLogControl (dialogPtr, 27);
					 
					DisplayThematicDialogOK (
									saveMagnification,
									displaySpecsPtr,
									&dialogSelectArea,
									localClassGroupsPtr,
									localClassGroupsHandle,
									localDisplayClasses,
									localDisplayGroups,
									localAllSubsetClass,
									localAllSubsetGroup,
									localPaletteUpToDateFlag,
									showLegendFlag,
									GetDLogControl (dialogPtr, IDC_VectorOverlays));
									
					returnFlag = TRUE;
													
					}	// end "if (itemHit == 1)" 
					
				if	(itemHit == 2)      // User selected Cancel for information 
					{
					modalDone = TRUE;		
					returnFlag = FALSE;
					displaySpecsPtr->classToGroupChangeFlag = FALSE;
					
					}	// end "if	(itemHit == 2)" 
					
				}	// end "else itemHit <= 2" 
					
			} while (!modalDone);
			
		HUnlock (displaySpecsPtr->displayClassGroupsHandle);
			
		UnlockAndDispose (localClassGroupsHandle);
			
		DisposeUserItemUPP (drawBackgroundColorBoxPtr);
		DisposeUserItemUPP (drawDisplayAllSubsetClassesGroupsPopUpPtr);
		DisposeUserItemUPP (drawDisplayClassesGroupsPopUpPtr);
		DisposeUserItemUPP (drawPalettePopUpPtr);
			
		CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
	#endif	// defined multispec_mac

	#if defined multispec_win   
		CMDisplayThematicDlg*		dialogPtr = NULL;
		
		TRY
			{ 
			dialogPtr = new CMDisplayThematicDlg ();
		
			CMDisplay* displayCPtr = gActiveImageViewCPtr->m_displayMultiCPtr;
			displayCPtr->SetDisplaySpecsPtr (displaySpecsPtr);
			
			returnFlag = dialogPtr->DoDialog (displaySpecsPtr);
		                       
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
		CMDisplayThematicDlg* dialogPtr = NULL;
		dialogPtr = new CMDisplayThematicDlg (NULL);

		CMDisplay* displayCPtr = gActiveImageViewCPtr->m_displayMultiCPtr;
		displayCPtr->SetDisplaySpecsPtr (displaySpecsPtr);

		returnFlag = dialogPtr->DoDialog (displaySpecsPtr);

		delete dialogPtr;
	#endif // defined multispec_wx   
	
	return (returnFlag);
	
}	// end "DisplayThematicDialog"  

									

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DisplayThematicDialogInitialize
//
//	Software purpose:	The purpose of this routine is to initialize variables that
//							will be used for the thematic display specification dialog box.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			OnInitDialog in xDisplayThematicDialog.cpp
//							DisplayThematicDialog
//
//	Coded By:			Larry L. Biehl			Date: 12/12/2006
//	Revised By:			Larry L. Biehl			Date: 04/27/2018

void DisplayThematicDialogInitialize (
				DialogPtr							dialogPtr,
				DisplaySpecsPtr					displaySpecsPtr,
				WindowInfoPtr						windowInfoPtr,
				FileInfoPtr							fileInfoPtr,
				DialogSelectArea*					dialogSelectAreaPtr,
				double*								magnificationPtr,
				SInt16*								classGroupSelectionPtr,
				Boolean*								paletteChangedFlagPtr,
				Boolean*								localPaletteUpToDateFlagPtr,
				SInt16*								localClassGroupsPtr,
				UInt32*								localDisplayClassesPtr,
				UInt32*								localDisplayGroupsPtr,
				SInt16*								localAllSubsetClassPtr,
				SInt16*								localAllSubsetGroupPtr,
				SInt16*								classSelectionPtr,
				RGBColor*							currentSelectedColorPtr,
				Boolean*								showLegendFlagPtr,
				Boolean*								thresholdFileFlagPtr,
				SInt32*								numberClassesPtr,
				SInt32*								numberGroupsPtr,
				Boolean*								includeVectorOverlaysFlagPtr)

{  
	SInt16								entireIconItem;
	
	
			// Initialize selected area structure.		

	entireIconItem = 4;
	if (gAppearanceManagerFlag)
		entireIconItem = 35;
		
	InitializeDialogSelectArea (dialogSelectAreaPtr,
											windowInfoPtr,
											gActiveImageWindow,
											displaySpecsPtr->columnStart,
											displaySpecsPtr->columnEnd,
											displaySpecsPtr->columnInterval,
											displaySpecsPtr->lineStart,
											displaySpecsPtr->lineEnd,
											displaySpecsPtr->lineInterval,
											10,
											entireIconItem,
											kDontAdjustToBaseImage);									

			// To entire image icon.															
			//	Selected display area															

	#if defined multispec_mac 
		if (gAppearanceManagerFlag)
			HideDialogItem (dialogPtr, 4);
		else	// !gAppearanceManagerFlag
			HideDialogItem (dialogPtr, 35);
	#endif	// defined multispec_mac  
			
	LoadLineColumnItems (dialogSelectAreaPtr, 
									dialogPtr, 
									kInitializeLineColumnValues, 
									kIntervalEditBoxesExist,
									1);	
	
			// Magnification of image from bit/pix map to window.			
		
	*magnificationPtr = displaySpecsPtr->magnification;
	LoadDItemRealValue (dialogPtr, IDC_Magnification, *magnificationPtr, 3);
				
			// Display classes or information groups popup menu.		
			
	*classGroupSelectionPtr = abs (displaySpecsPtr->classGroupCode);
	if (*classGroupSelectionPtr == kGroupClassDisplay)
		*classGroupSelectionPtr = kGroupDisplay;
	if (displaySpecsPtr->numberLevels > gClassListLimit) 
		*classGroupSelectionPtr = *classGroupSelectionPtr;  									
					
	#if defined multispec_mac   
		if (fileInfoPtr->numberGroups == 0)
			DisableMenuItem (gPopUpClassGroupMenu, 2);
			
		else	// fileInfoPtr->numberGroups > 0
			EnableMenuItem (gPopUpClassGroupMenu, 2);
	#endif	// defined multispec_mac

	#if defined multispec_win   
		if (fileInfoPtr->numberGroups == 0)		
			((CComboBox*)dialogPtr->GetDlgItem (IDC_DisplayCombo))->DeleteString (1);
	#endif	// defined multispec_win

	#if defined multispec_wx
		if (fileInfoPtr->numberGroups == 0)
			{
			wxChoice* dispcombo = ((wxChoice*)dialogPtr->FindWindow (IDC_DisplayCombo));
			if (dispcombo->GetCount () > 1)
				dispcombo->Delete (1);
			}
	#endif
	
			// Palette to use
	
	if (abs (*classGroupSelectionPtr) == kClassDisplay)
		gPaletteSelection = displaySpecsPtr->thematicClassPaletteType;
	
	else	// *classGroupSelectionPtr == kGroupDisplay 
		gPaletteSelection = displaySpecsPtr->thematicGroupPaletteType;
		
	*paletteChangedFlagPtr = FALSE;
	
	*localPaletteUpToDateFlagPtr = TRUE;
	
			// Set up the palette popup menu.											
	
	gPaletteSelection = SetUpPalettePopUpMenu (dialogPtr,
																gPopUpPaletteMenu,
																fileInfoPtr->format,
																fileInfoPtr->ancillaryInfoformat,
																fileInfoPtr->numberClasses,
																fileInfoPtr->classDescriptionH, 
																fileInfoPtr->colorTableOffset,
																displaySpecsPtr, 
																abs (*classGroupSelectionPtr),
																gPaletteSelection);
	
			// Display all or subset of classes/groups.	
	
	LoadLocalClassGroupDisplayInfo (displaySpecsPtr, 
												fileInfoPtr, 
												localClassGroupsPtr);
	
	*localDisplayClassesPtr = displaySpecsPtr->numberDisplayClasses;
	*localDisplayGroupsPtr = displaySpecsPtr->numberDisplayGroups;
	*localAllSubsetClassPtr = displaySpecsPtr->allSubsetClassSet;
	*localAllSubsetGroupPtr = displaySpecsPtr->allSubsetGroupSet;
	
	if (abs (*classGroupSelectionPtr) == kClassDisplay)
		*classSelectionPtr = *localAllSubsetClassPtr;
	
	else	// *classGroupSelectionPtr == kGroupDisplay 
		*classSelectionPtr = *localAllSubsetGroupPtr;
	 
	if (displaySpecsPtr->numberLevels > gClassListLimit)
		{
		#if defined multispec_mac   
			*classSelectionPtr = -*classSelectionPtr;
		#endif	// defined multispec_mac
		
		#if defined multispec_win   
			((CComboBox*)dialogPtr->GetDlgItem (IDC_ClassesGroupsCombo))->DeleteString (1);
			*classSelectionPtr = kAllMenuItem;
		#endif	// defined multispec_win

		#if defined multispec_wx   
			#if defined multispec_wxlin
				wxComboBox* dispcombo = ((wxComboBox*)dialogPtr->
																FindWindow (IDC_ClassesGroupsCombo));
			#endif
			#if defined multispec_wxmac
				wxChoice* dispcombo = ((wxChoice*)dialogPtr->
																FindWindow (IDC_ClassesGroupsCombo));
			#endif
	
			if (dispcombo->GetCount () > 1)
				dispcombo->Delete (1);
			*classSelectionPtr = kAllMenuItem;
		#endif	// defined multispec_wx
		
		}	// end "if (displaySpecsPtr->numberLevels > gClassListLimit)"
	
			// Background color.														
	
	displaySpecsPtr->editedBackgroundColor = displaySpecsPtr->backgroundColor;
	*currentSelectedColorPtr = displaySpecsPtr->backgroundColor;
							
	if (fileInfoPtr->numberClasses > 254)
		SetDLogControlHilite (dialogPtr, IDC_BackgroundColor, 255);
	
	#if defined multispec_win   
		MHideDialogItem (dialogPtr, IDC_BackgroundColorChip);
	#endif	// defined multispec_win
	
			// Display class legend.												
	
	*showLegendFlagPtr = windowInfoPtr->showLegend;
	
			// Use thresholding file.												
	
	*thresholdFileFlagPtr = displaySpecsPtr->thresholdFileFlag;
	HideDialogItem (dialogPtr, IDC_UseThresholdingFile);
	
			// Show number of classes.										
	
	*numberClassesPtr = displaySpecsPtr->numberLevels;
	LoadDItemValue (dialogPtr, IDC_NumberClasses, (SInt32)displaySpecsPtr->numberLevels);
	
			// Show number of groups.										
	
	*numberGroupsPtr = fileInfoPtr->numberGroups;
	LoadDItemValue (dialogPtr, IDC_NumberGroups, (SInt32)fileInfoPtr->numberGroups);
	
			// Only show option to overlay vectors if no image currently exists
			// for the window and shape files do exist.
			
	SetVectorOverlays (windowInfoPtr,
								dialogPtr,
								IDC_VectorOverlays,
								includeVectorOverlaysFlagPtr);
	
}	// end "DisplayThematicDialogInitialize"

									

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DisplayThematicImageOK
//
//	Software purpose:	The purpose of this routine is to set up the display
//							structure parameters that deal with displaying thematic
//							images based on the selections that the
//							user made in the dialog box.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			DoDialog in xDisplayThematicDialog.cpp
//							DisplayThematicDialog
//
//	Coded By:			Larry L. Biehl			Date: 03/29/1996
//	Revised By:			Larry L. Biehl			Date: 01/09/2020

void DisplayThematicDialogOK (
				double								magnification,
				DisplaySpecsPtr					displaySpecsPtr,
				DialogSelectArea*					dialogSelectAreaPtr,
				SInt16*								localClassGroupsPtr,
				Handle								localClassGroupsHandle,
				UInt32								localDisplayClasses,
				UInt32								localDisplayGroups,
				SInt16								localAllSubsetClass,
				SInt16								localAllSubsetGroup,
				Boolean								paletteUpToDateFlag,
				Boolean								showLegendFlag,
				Boolean								includeVectorOverlaysFlag)

{  
	//Boolean								redrawScrollBarFlag;
	
	
	gClassGroupSelection = abs (gClassGroupSelection);
				
			// Selected display area.												
				
	displaySpecsPtr->lineStart = dialogSelectAreaPtr->lineStart;
	displaySpecsPtr->lineEnd = dialogSelectAreaPtr->lineEnd;
	displaySpecsPtr->lineInterval = dialogSelectAreaPtr->lineInterval;
	displaySpecsPtr->columnStart = dialogSelectAreaPtr->columnStart;
	displaySpecsPtr->columnEnd = dialogSelectAreaPtr->columnEnd;
	displaySpecsPtr->columnInterval = dialogSelectAreaPtr->columnInterval;
	
			// Magnification to use										
						
	displaySpecsPtr->magnification = magnification;
	#if defined multispec_wx
		gActiveImageViewCPtr->m_Scale = magnification;
				
		UInt32 maxMagnification = 99;
	
		#if defined multispec_wxlin
					// Check max magnification. GTK has problems if too large. Checks
					// indicate that when number of pixels in a direction times the
					// magnification is larger than 26,000 or so, gtk errors occur.
			SInt32 numberHorizontalPixels = (displaySpecsPtr->lineEnd -
					displaySpecsPtr->lineStart + displaySpecsPtr->lineInterval) /
									displaySpecsPtr->lineInterval;
		
			SInt32 numberVerticalPixels = (displaySpecsPtr->columnEnd -
					displaySpecsPtr->columnStart + displaySpecsPtr->columnInterval) /
									displaySpecsPtr->columnInterval;
									
			maxMagnification =
							MIN (maxMagnification, 26000/numberHorizontalPixels);
									
			maxMagnification =
							MIN (maxMagnification, 26000/numberVerticalPixels);
		#endif	// defined multispec_wxlin
							
		displaySpecsPtr->maxMagnification = maxMagnification;		
	#endif // defined multispec_wx
				
			// Palette to be used.	
			
	SetPaletteSpecification (displaySpecsPtr,
										gClassGroupSelection,
										gPaletteSelection,
										paletteUpToDateFlag);
								
			// Group spectral classes.									
						
	if (displaySpecsPtr->classGroupCode != gClassGroupSelection)
		displaySpecsPtr->paletteUpToDateFlag = FALSE;
	displaySpecsPtr->classGroupCode = gClassGroupSelection;
				
				// Check if reading from disk file needs to be forced.	
							
	if (gClassGroupSelection == kClassDisplay && 
							displaySpecsPtr->savedClassCTableHandle == NULL)
		displaySpecsPtr->readPaletteFromDiskFlag = TRUE;
	if (gClassGroupSelection != kClassDisplay && 
							displaySpecsPtr->savedGroupCTableHandle == NULL)
		displaySpecsPtr->readPaletteFromDiskFlag = TRUE;
				
			// Display all/subset of classes/groups.				
					
			// 	Make sure all classes selected if requested.					
				
	if (localAllSubsetClass == 1)
		LoadClassGroupVector (&localDisplayClasses,
										localClassGroupsHandle,
										gImageFileInfoPtr->numberClasses,
										gImageFileInfoPtr->numberGroups,
										-kClassDisplay);
					
			// 	Make sure all groups selected if requested.					
				
	if (localAllSubsetGroup == 1)
		LoadClassGroupVector (&localDisplayGroups,
										localClassGroupsHandle,
										gImageFileInfoPtr->numberClasses,
										gImageFileInfoPtr->numberGroups,
										-kGroupDisplay);
						
			// Get the new list of classes and groups to be displayed	
			// and force the palette to be updated if there was a 		
			// change.																	
						
	if (displaySpecsPtr->allSubsetClassSet != localAllSubsetClass)
		displaySpecsPtr->paletteUpToDateFlag = FALSE;
					
	if (displaySpecsPtr->allSubsetGroupSet != localAllSubsetGroup)
		displaySpecsPtr->paletteUpToDateFlag = FALSE;
								
	displaySpecsPtr->allSubsetClassSet = localAllSubsetClass;
	displaySpecsPtr->allSubsetGroupSet = localAllSubsetGroup;
				
	displaySpecsPtr->numberDisplayClasses = localDisplayClasses;
	displaySpecsPtr->numberDisplayGroups = localDisplayGroups;
				
	LoadClassGroupDisplayInfo (
					displaySpecsPtr, gImageFileInfoPtr, localClassGroupsPtr);
					
			// Background color.	    										
				
	if (displaySpecsPtr->backgroundColor.red != gCurrentSelectedColor.red || 
			displaySpecsPtr->backgroundColor.green != gCurrentSelectedColor.green ||
			 displaySpecsPtr->backgroundColor.blue != gCurrentSelectedColor.blue)
		displaySpecsPtr->paletteUpToDateFlag = FALSE;
	displaySpecsPtr->backgroundColor = gCurrentSelectedColor;
													
			// Display legend.											
			
	gImageWindowInfoPtr->showLegend = showLegendFlag;

	displaySpecsPtr->includeVectorOverlaysFlag = includeVectorOverlaysFlag;
					
			// Make certain that the palette will be updated if it		
			// needs to be read from the disk.									
						
	if (displaySpecsPtr->readPaletteFromDiskFlag)
		displaySpecsPtr->paletteUpToDateFlag = FALSE;
					
	if (!paletteUpToDateFlag)
		displaySpecsPtr->paletteUpToDateFlag = FALSE; 
	
}	// end "DisplayThematicDialogOK" 

									

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DisplayThematicImage
//
//	Software purpose:	The purpose of this routine is to control drawing
//							Thematic images in the window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			DisplayImage in SDisplay.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/18/1988
//	Revised By:			Larry L. Biehl			Date: 04/16/2020

Boolean DisplayThematicImage (void)

{
	double								previousMagnification;
	
	time_t								startTime;
	
	DisplaySpecsPtr					displaySpecsPtr;
	GrafPtr								savedPort;
	
	SInt16								previousClassGroupCode,
											returnCode,
											savedNumberGroups;
	
	Boolean								continueFlag = TRUE,
											displayChangedFlag,
											imageDisplayedFlag = FALSE,
											paletteChangedFlag;
		    		
	
			// If spare memory had to be used to load code resources, then exit	
			// routine.																				
			
	if (gMemoryTypeNeeded < 0)
																		return (imageDisplayedFlag);
																							
			// Code resources loaded okay, so set flag back for non-Code			
			// resources.																			
			
	gMemoryTypeNeeded = 0;
	
			// Make certain that there are not too many classes to be displayed.	
			
	if (gImageFileInfoPtr->numberClasses >= kMaxNumberDisplayClasses)
		{															
				// Display an alert.																
		
		returnCode = DisplayAlert (
						kErrorAlertID, kCautionAlert, kAlertStrID, IDS_Alert16, 0, NULL);
																		return (imageDisplayedFlag);
																							
		}	// end "if (gFileInfoPtr->numberClasses >= kMaxNumberDisplayClasses)" 
	
			// Load in the default display specifications.								
			  				
   displaySpecsPtr = LoadThematicDisplaySpecs ();

	if (displaySpecsPtr == NULL)
																		return (imageDisplayedFlag);
	
			// Initialize the flag indicating whether the data needs to be read	
			// from the disk.  The case may occur where only the palette needs	
			// to be changed.																		
			
	displayChangedFlag = FALSE;																				
	if (gImageWindowInfoPtr->offScreenMapHandle == NULL)
		displayChangedFlag = TRUE;	
	
			// Save the current class group code and magnification.											
			
	previousClassGroupCode = displaySpecsPtr->classGroupCode;
	previousMagnification = displaySpecsPtr->magnification;
   
   if (gCallProcessorDialogFlag)
      continueFlag = DisplayThematicDialog (displaySpecsPtr);
	
	else	// !gCallProcessorDialogFlag
		{
				// Update the combo list in the legend.
						
		gClassGroupSelection = abs (displaySpecsPtr->classGroupCode);
		if (gClassGroupSelection == kGroupClassDisplay)
			gClassGroupSelection = kGroupDisplay;

		CMLegendView* legendViewCPtr = gActiveImageViewCPtr->GetImageLegendViewCPtr ();
		legendViewCPtr->UpdateClassGroupComboList (gClassGroupSelection);
		
		}	// end "else !gCallProcessorDialogFlag"
		
	if (continueFlag)
		{
		MSetCursor (kWait);
         
      UpdateLegendWidth ();
			
				// Set up the vector to convert image data values to display		
				// level values.																	
					
		if (HistogramThematicVector (displaySpecsPtr, TRUE))
			{
			GetPort (&savedPort);
			SetPortWindowPort (gActiveImageWindow);
			
			paletteChangedFlag = !displaySpecsPtr->paletteUpToDateFlag;
			
					// Create the palette.														
			
			savedNumberGroups = gImageFileInfoPtr->numberGroups;
			if (CreatePalette (gImageWindowInfoPtr->windowInfoHandle, 
										displaySpecsPtr, 
										kThematicWindowType))
				{
				if (!displaySpecsPtr->imageWindowIsUpToDateFlag)
					displayChangedFlag = TRUE;
				
				#if defined multispec_wx
							// Be sure scale is set for ImageView. This is put here
							// in case the dialog is not called.
					gActiveImageViewCPtr->m_Scale = displaySpecsPtr->magnification;
				#endif	// defined multispec_wx
					
						// Draw the image as requested. 										
						// Remove the selection rectangle if it exists and the		
						// area to be displayed is now different.							
				
				if (displaySpecsPtr->displayedLineStart !=
														displaySpecsPtr->lineStart ||
						displaySpecsPtr->displayedLineEnd !=
														displaySpecsPtr->lineEnd ||
						displaySpecsPtr->displayedLineInterval !=
														displaySpecsPtr->lineInterval ||
						displaySpecsPtr->displayedColumnStart !=
														displaySpecsPtr->columnStart ||
						displaySpecsPtr->displayedColumnEnd !=
														displaySpecsPtr->columnEnd ||
						displaySpecsPtr->displayedColumnInterval !=
														displaySpecsPtr->columnInterval)
					{
					displayChangedFlag = TRUE;
					
					ClearSelectionArea (gActiveImageWindow);
													
							// Save displayed lines and columns.			 				
							
					displaySpecsPtr->displayedLineStart = displaySpecsPtr->lineStart;
					displaySpecsPtr->displayedLineEnd = displaySpecsPtr->lineEnd;
					displaySpecsPtr->displayedLineInterval =
																displaySpecsPtr->lineInterval;
																
					displaySpecsPtr->displayedColumnStart =
																	displaySpecsPtr->columnStart;
					displaySpecsPtr->displayedColumnEnd = displaySpecsPtr->columnEnd;
					displaySpecsPtr->displayedColumnInterval =
															displaySpecsPtr->columnInterval;
					
					}	// end "if (displaySpecsPtr->displayedLineStart !=..." 
															
				if (gImageWindowInfoPtr->grafPortType == kCGrafType)
					{
					if (displayChangedFlag)
						{
								// Initialize some global variables pertaining to output.	
						
						gOutputTextOKFlag = TRUE;
						gOutputForce1Code = 0x0001;
		
								// Force text selection to start from end of present 		
								// text.																	
							
						ForceTextToEnd ();
						
						continueFlag = ListHeaderInfo (NULL, 
																	0, 
																	&gOutputForce1Code, 
																	kNoStatisticsUsed, 
																	continueFlag);
						
						if (savedNumberGroups == 0 && gImageFileInfoPtr->numberGroups > 0)
							{
									// List message in Output Text Window that a default group table was
									// created.
							
							int numChars = sprintf ((char*)gTextString3,
											"    A default group table was created.%s",
											gEndOfLine);
							ListString ((char*)gTextString3, numChars, gOutputTextH);
						
							}	// end "if (savedNumberGroups == 0 && ..."
			
								// Get start time.													
					
						startTime = time (NULL);
															
						DisplayColorImage (displaySpecsPtr, NULL);
						imageDisplayedFlag = TRUE;
						displaySpecsPtr->imageWindowIsUpToDateFlag = TRUE;
					
								// Get the end time and print the time elapsed in the 	
								// output window.														
									
						continueFlag = ListCPUTimeInformation (NULL, continueFlag, startTime);
			
								// Scroll output window to the selection and adjust the 	
								// scroll bar.															
						
						UpdateOutputWScrolls (gOutputWindow, 1, kDisplayMessage);
						
						}	// end "if (displayChangedFlag)"
						
					else	// !displayChangedFlag
						{
						if (previousMagnification != displaySpecsPtr->magnification)
							{
							Boolean eraseFlag = 
									(displaySpecsPtr->magnification < previousMagnification);
							InvalidateWindow (gActiveImageWindow, kImageFrameArea, eraseFlag);
							
							}	// end "if (previousMagnification != ..."
						                 
						if (paletteChangedFlag)
							ActivateImageWindowPalette (displaySpecsPtr->paletteObject);
							
						}	// end "else !displayChangedFlag"
						
					}	// end "...->grafPortType == kCGrafType" 
				
				UpdateImageZoomControls (
							gActiveImageWindow, displaySpecsPtr->magnification, TRUE);
							
				UpdateCoordinateViewControls (gActiveImageWindow);
													
						// Load the legend list.												
				
				gPaletteOffset = displaySpecsPtr->paletteOffset;
				gClassPaletteEntries = displaySpecsPtr->numPaletteEntriesUsed;
				
				if (gImageWindowInfoPtr->imageBaseAddressH ||
													gImageWindowInfoPtr->offscreenGWorld)
					{
					if (displayChangedFlag || previousClassGroupCode != 
									displaySpecsPtr->classGroupCode ||
																paletteChangedFlag)								
						LoadThematicLegendList (
										gImageWindowInfoPtr->legendListHandle, FALSE);
						
					}	// end "if (...imageBaseAddressH || ..."
													
				}	// end "if (CreateThematicPalette (..." 
			
			UpdateActiveImageWScrolls (displaySpecsPtr->magnification);
		
			SetPort (savedPort);
			
			}	// end "if (HistogramThematicVector (displaySpecsPtr...)"
		
				// Dispose of the symbolToPalette table if needed.
						
		displaySpecsPtr->symbolToPaletteEntryH = 
						UnlockAndDispose (displaySpecsPtr->symbolToPaletteEntryH);
     			
     			// Indicate that the entire image can be selected if it is the			
     			// active window.	

		gUpdateFileMenuItemsFlag = TRUE;
		gUpdateEditMenuItemsFlag = TRUE;
			
		}	// end 'if (continueFlag)'
	
	MInitCursor ();
			
			// Unlock handle to display specifications.
	
	Handle displaySpecsHandle = GetActiveDisplaySpecsHandle ();
	CheckAndUnlockHandle (displaySpecsHandle);
	
	return (imageDisplayedFlag);
	
}	// end "DisplayThematicImage" 


/*
#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawBackgroundColorBox
//
//	Software purpose:	The purpose of this routine is to draw a rounded
//							rectangle filled with the background color.  It is
//							used by the Thematic Dialog routine.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			DisplayThematicDialog in SDisplayThematic.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/23/1991
//	Revised By:			Larry L. Biehl			Date: 03/18/1996

pascal void DrawBackgroundColorBox (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{
	PenState								penState;
	Rect									backgroundRect;
	RGBColor								savedForeGroundColor;
	
	Handle								theHandle;
	
	SInt16								theType;
	
		
			// Save the current pen state.													
		
	GetPenState (&penState);
	if (gColorQDflag)
		GetForeColor (&savedForeGroundColor);

			// Get the area to draw the background color in.							
			
	GetDialogItem (dialogPtr, itemNumber, &theType, &theHandle, &backgroundRect);
	
			// Set the penmode and color.														
	
	PenMode (patCopy);
	
	theHandle = gImageWindowInfoPtr->displaySpecsHandle;
	if (gColorQDflag)
		RGBForeColor (&((DisplaySpecsPtr)*theHandle)->editedBackgroundColor);
		
	else	// !gColorQDflag
		ForeColor (whiteColor);
	
			// Paint the color.																	
			
	PaintRoundRect (&backgroundRect, 6, 4);
	
			// Reset the foreground color.													
	
	if (gColorQDflag)		
		RGBForeColor (&savedForeGroundColor);
		
	else	// !gColorQDflag
		ForeColor (blackColor);
	
			// Now outline the box.																
	
	FrameRoundRect (&backgroundRect, 6, 4);		
			
			// Reset the pen state back to the original settings.						

	SetPenState (&penState);
	
}	// end "DrawBackgroundColorBox"  
#endif	// defined multispec_mac
*/


#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawDisplayAllSubsetClassesGroupsPopUp
//
//	Software purpose:	The purpose of this routine is to draw the display
//							all classes/groups prompt and 
//							the current selection for the pop up menu.  
//							This routine will also draw a drop
//							shadow box around the pop up selection.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			DisplayThematicDialog in SDisplayThematic.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/26/1990
//	Revised By:			Larry L. Biehl			Date: 11/26/1990	

pascal void DrawDisplayAllSubsetClassesGroupsPopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.									
			
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpAllSubsetMenu, 
								gClassSelection, 
								TRUE);
	
}	// end "DrawDisplayAllSubsetClassesGroupsPopUp"  
#endif	// defined multispec_mac



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawDisplayClassesGroupsPopUp
//
//	Software purpose:	The purpose of this routine is to draw the display
//							spectral classes or information groups prompt and 
//							the current selection for the pop up menu.  
//							This routine will also draw a drop
//							shadow box around the pop up selection.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			DisplayThematicDialog in SDisplayThematic.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/26/1990
//	Revised By:			Larry L. Biehl			Date: 11/26/1990	

pascal void DrawDisplayClassesGroupsPopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.									
			
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpClassGroupMenu, 
								gClassGroupSelection, 
								TRUE);
	
}	// end routine "DrawDisplayClassesGroupsPopUp"  
#endif	// defined multispec_mac



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean HistogramThematicVector
//
//	Software purpose:	The purpose of this routine is to load the vector
//							which maps the class number values to palette color
//							indeces.
//
//	Parameters in:		Window pointer to the grafport of the image window.
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/19/1989
//	Revised By:			Larry L. Biehl			Date: 05/01/2013	

Boolean HistogramThematicVector (
				DisplaySpecsPtr					displaySpecsPtr,
				Boolean								firstTimeFlag)

{
	FileInfoPtr							fileInfoPtr;
	HUInt16Ptr							classSymbolPtr;
	UCharPtr								symbolToPalettePtr;
	
	UInt32								index;
	
	SInt16								grafPortType;
	
	UInt16								numberPaletteEntriesUsed,
											paletteEntries,
											paletteIndex,
											paletteOffset;
											
	Boolean								firstTimeStartOverFlag,
											returnFlag;
	
	
			// Initialize local variables.													
			
	returnFlag = FALSE;
																			
			// Get the window port type.														
	
	grafPortType = gImageWindowInfoPtr->grafPortType;
		
			// Handle to file information should already be locked.  Get pointer 
			// to file information.																
	
	fileInfoPtr = gImageFileInfoPtr;
	
			// Check if need to go any farther.												
			
	if (grafPortType == kCGrafType && fileInfoPtr->numberBytes == 1 &&
																!fileInfoPtr->asciiSymbols)
																					return (TRUE);
	
			// Get symbol to palette entry table.		
	
	if (displaySpecsPtr->symbolToPaletteEntryH == NULL)
		displaySpecsPtr->symbolToPaletteEntryH = MNewHandleClear (
					(SInt32)fileInfoPtr->numberBins * sizeof (unsigned char));
		
	if (displaySpecsPtr->symbolToPaletteEntryH != NULL)
		{
				// If this is the first call for a color type image, then exit. 
				// The table will be filled later from a call from
				// 'DisplayColorThematicImage'.
				
		if (grafPortType == kCGrafType && firstTimeFlag)
																					return (TRUE);
		
				// Get pointer to symbol-to-palette vector.								
				
		symbolToPalettePtr = (unsigned char*)GetHandlePointer (
												displaySpecsPtr->symbolToPaletteEntryH,
												kLock);
	
		paletteIndex = 0;
		
				// The maximum number of palette entries is 256. If the number
				// of classes is more than this then the assigned colors will
				// start over again at 1. This allows for situations where the
				// first (0) entry is white for background.														
		
		numberPaletteEntriesUsed = displaySpecsPtr->numPaletteEntriesUsed;
		paletteOffset = displaySpecsPtr->paletteOffset;	
		paletteEntries = MIN (numberPaletteEntriesUsed, 256);
		firstTimeStartOverFlag = TRUE;
		
		if (fileInfoPtr->asciiSymbols)
			{
					// Get pointer to class symbol table.								
					// Remember to take into account that classSymbolPtr 			
					// points to a short int vector.	Str15 is used instead of	
					// Str31.																	
			
			classSymbolPtr = (HUInt16Ptr)GetHandlePointer (
														fileInfoPtr->classDescriptionH,
														kLock);
			classSymbolPtr = 
					&classSymbolPtr[fileInfoPtr->numberClasses*sizeof (Str15)];
						
			}	// end "if (fileInfoPtr->asciiSymbols)"
		
		if (grafPortType == kCGrafType)
			{
			if (fileInfoPtr->asciiSymbols && fileInfoPtr->classDescriptionH != NULL)
				{
						// Set up vector for the class symbols.							
				
				for (index=0; index<fileInfoPtr->numberClasses; index++)
					{		
					if (paletteIndex >= paletteEntries)
						{
						if (firstTimeStartOverFlag)
							{
							paletteEntries--;
							firstTimeStartOverFlag = FALSE;
							
							}	// end "if (!firstTimeStartOverFlag)"
							
						//paletteIndex = paletteIndex % paletteEntries;
						paletteIndex = 1;
						
						}	// end "if (paletteIndex >= numberPaletteEntriesUsed)"
		
					symbolToPalettePtr[classSymbolPtr[index]] = (UInt8)paletteIndex;
					paletteIndex++;
					
					}	// end "for (index=0; index<fileInfoPtr->..." 
				
				returnFlag = TRUE;
			
				}	// end "if (fileInfoPtr->asciiSymbols && ...)"
				
			else if (fileInfoPtr->numberBytes == 2)
				{
						// Set up vector for 2 byte data. 	
				
				for (index=0; index<fileInfoPtr->numberClasses; index++)
					{	
					if (paletteIndex >= paletteEntries)
						{
						if (firstTimeStartOverFlag)
							{
							paletteEntries--;
							firstTimeStartOverFlag = FALSE;
							
							}	// end "if (!firstTimeStartOverFlag)"
							
						//paletteIndex = (UInt16)index % paletteEntries;
						paletteIndex = 1;
						
						}	// end "if (paletteIndex >= numberPaletteEntriesUsed)"
					
					symbolToPalettePtr[index] = (UInt8)paletteIndex;
					paletteIndex++;
					
					}	// end "for (index=0; index<fileInfoPtr->..." 
				
				returnFlag = TRUE;
				
				}	// end "else if (fileInfoPtr->numberBytes == 2)"
				
			}	// end "if (grafPortType == kCGrafType)"
			
		else	// grafPortType != kCGrafType
			{
			/*		// Set up vector for converting class numbers to patterns.
		
					// We will assume that the number of palette entries that will	
					// be used is 256 - 3. This will be updated in CreatePPalette.
					// Note that between this routine and CreatePPalette, the
					// symbolToPalette table will not be set up correctly for 
					// 2-byte data. This code is not used enough to fix at this
					// time.										
					
			numberPaletteEntriesUsed = 256 - 3;	
				
			for (index=0; index<fileInfoPtr->numberClasses; index++)
				{
				if (paletteIndex >= numberPaletteEntriesUsed)
					paletteIndex = paletteIndex % numberPaletteEntriesUsed;
				
				if (fileInfoPtr->asciiSymbols)
					symbolToPalettePtr[classSymbolPtr[index]] = 
																		(UInt8)(paletteIndex + 1);
				
				else	//	!fileInfoPtr->asciiSymbols
					symbolToPalettePtr[index] = (UInt8)(paletteIndex + 1);
					
				paletteIndex++;
				
				}	// end "for (index=0; index<..." 
			*/			
			returnFlag = FALSE;
				
			}	// end "else grafPortType != kCGrafType" 
			
		CheckAndUnlockHandle (fileInfoPtr->classDescriptionH);
		CheckAndUnlockHandle (displaySpecsPtr->symbolToPaletteEntryH);
		
		}	// end "if (displaySpecsPtr->symbolToPaletteEntryH != NULL)" 
	
	return (returnFlag);

}	// end "HistogramThematicVector" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadClassGroupDisplayInfo
//
//	Software purpose:	The purpose of this routine is to load the
//							class/group display list stored in the display structure
//							from the local class/group display list used in	the
// 						ThematicDialog routine.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			ThematicDialog   in SDisplayThematic.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/25/1991
//	Revised By:			Larry L. Biehl			Date: 01/25/1991	

void LoadClassGroupDisplayInfo (
				DisplaySpecsPtr					displaySpecsPtr, 
				FileInfoPtr							fileInfoPtr, 
				SInt16*								localClassGroupsPtr)

{
	SInt16								*displayClassToGroupPtr,
											*displayClassGroupsPtr;
										
	SInt32								index,
											localIndex,
											maxIndex,
											numberClasses;
	
	SInt16								group,
											itemNumber,
											numberGroups;


			// Initialize local variables.													
			
	displayClassGroupsPtr = (short int*)GetHandlePointer (
														displaySpecsPtr->displayClassGroupsHandle);
	numberClasses = fileInfoPtr->numberClasses;
	
			// Load class display list.														
	
	maxIndex = displaySpecsPtr->numberDisplayClasses - 1;
	
	itemNumber = 1;
	localIndex = 0;	
	for (index=0; index<numberClasses; index++)
		{
		if (localClassGroupsPtr[localIndex] == itemNumber)
			{
			displayClassGroupsPtr[index] = displayClassGroupsPtr[index] | 0x0001;
			
			if (localIndex < maxIndex)
				localIndex++;
			
			}	// end "for (index=0; index<numberClasses; index++)" 
			
		else	// localClassGroupsPtr[localIndex] != itemNumber 
			displayClassGroupsPtr[index] = displayClassGroupsPtr[index] & 0x0100;
			
		itemNumber++;
			
		}	// end "for (index=0; index<numberClasses; index++)" 
	
			// Load group display list.														
	
	localClassGroupsPtr = &localClassGroupsPtr[numberClasses];
	numberGroups = fileInfoPtr->numberGroups;
	displayClassToGroupPtr = GetClassToGroupPointer (fileInfoPtr);
	maxIndex = displaySpecsPtr->numberDisplayGroups - 1;
	
	itemNumber = 1;
	localIndex = 0;	
	for (group=0; group<numberGroups; group++)
		{
		if (localClassGroupsPtr[localIndex] == itemNumber)
			{
			for (index=0; index<numberClasses; index++)
				if (displayClassToGroupPtr[index] == group)
					displayClassGroupsPtr[index] |= 0x0100;
			
			if (localIndex < maxIndex)
				localIndex++;
					
			}	// end "if (localClassGroupsPtr[localIndex] == itemNumber)" 
				
		else	// localClassGroupsPtr[localIndex] != itemNumber 
			{
			for (index=0; index<numberClasses; index++)
				if (displayClassToGroupPtr[index] == group)
					displayClassGroupsPtr[index] &= 0x0001;
				
			}	// end "else localClassGroupsPtr[localIndex] != itemNumber" 
			
		itemNumber++;
			
		}	// end "for (group=0; group<numberGroups; group++)" 
	
}	// end routine "LoadClassGroupDisplayInfo" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				void LoadClassGroupVector
//
//	Software purpose:			The purpose of this routine is to load the class/group
//									vector with a list of all class or group numbers
//									available.
//		
//	Parameters in:				
//
//	Parameters out:			
//
// Value Returned:		
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/21/1991
//	Revised By:			Larry L. Biehl			Date: 11/21/1996	

void LoadClassGroupVector (
				UInt32*								numberClassesGroupsPtr, 
				Handle								classGroupHandle, 
				UInt32								numberClasses, 
				UInt32								numberGroups, 
				SInt16								classGroupCode)

{
	SInt16*								classGroupPtr;

	UInt32								index;
	SInt16								setting;


			// Initialize local variables.													
			
	classGroupPtr = (SInt16*)GetHandlePointer (classGroupHandle);
	
	if (classGroupCode > 0)
		{
				// Load class/group display vector in display structure format.	
				
		if (classGroupCode == kClassDisplay)
			{
			setting = 0x0001;
			*numberClassesGroupsPtr = numberClasses;
			
			}	// end "if (classGroupCode == kClassDisplay)" 
			
		else	// classGroupCode != kClassDisplay 
			{
			setting = 0x0100;
			*numberClassesGroupsPtr = numberGroups;
			
			}	// end "else classGroupCode != kClassDisplay" 
		
		for (index=0; index<numberClasses; index++)
			classGroupPtr[index] = classGroupPtr[index] | setting;
			
		}	// end "if (classGroupCode > 0)" 
		
	else	// classGroupCode <= 0 
		{
				// Load class/group display vector in thematic dialog format.	
				
		if (classGroupCode == -kClassDisplay)
			{
			*numberClassesGroupsPtr = (unsigned short int)numberClasses;
			for (index=0; index<numberClasses; index++)
				classGroupPtr[index] = (UInt16)(index + 1);
			
			}	// end "if (classGroupCode == kClassDisplay)" 
			
		else	// classGroupCode != -kClassDisplay 
			{
			*numberClassesGroupsPtr = numberGroups;
			classGroupPtr = &classGroupPtr[numberClasses];
			for (index=0; index<numberGroups; index++)
				classGroupPtr[index] = (UInt16)(index + 1);
			
			}	// end "else classGroupCode != kClassDisplay" 
			
		}	// end "else classGroupCode <= 0" 
					
}	// end "LoadClassGroupVector" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadLocalClassGroupDisplayInfo
//
//	Software purpose:	The purpose of this routine is to load the local
//							class/group display list for the ThematicDialog
//							routine.  The display specification structure class
//							group display list is converted to a different
//							format in the local list
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			ThematicDialog   in SDisplayThematic.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/25/1991
//	Revised By:			Larry L. Biehl			Date: 07/01/1997	

void LoadLocalClassGroupDisplayInfo (
				DisplaySpecsPtr					displaySpecsPtr, 
				FileInfoPtr							fileInfoPtr, 
				SInt16*								localClassGroupsPtr)

{
	SInt16								*displayGroupToClassPtr,
											*displayClassGroupsPtr;
	
	SInt32								classIndex,
											index,
											itemCount,
											numberClasses,
											numberGroups;
								
								
			// Initialize local variables.													
			
	displayClassGroupsPtr = (SInt16*)GetHandlePointer (
													displaySpecsPtr->displayClassGroupsHandle);
	numberClasses = fileInfoPtr->numberClasses;
	
			// Load local class display list.												
	
	itemCount = 0;	
	for (index=0; index<numberClasses; index++)
		{
		if (displayClassGroupsPtr[index] & 0x0001)
			{
			localClassGroupsPtr[itemCount] = (SInt16)(index + 1);
			itemCount++;
			
			}	// end "if (displayClassGroupsPtr[index] & 0x0001)" 
			
		}	// end "for (index=0; index<numberClasses; index++)" 
		
	displaySpecsPtr->numberDisplayClasses = itemCount;
	
			// Load local group display list.												
			// Exit if no groups exist.														
	
	if (fileInfoPtr->groupTablesHandle == NULL)
																						return;
																						
	itemCount = 0;
	localClassGroupsPtr = &localClassGroupsPtr[numberClasses];
	displayGroupToClassPtr = (short int*)GetHandlePointer (
																fileInfoPtr->groupTablesHandle);
	numberGroups = fileInfoPtr->numberGroups;
	
	for (index=0; index<numberGroups; index++)
		{
		classIndex = displayGroupToClassPtr[index];
		if (displayClassGroupsPtr[classIndex] & 0x0100)
			{
			localClassGroupsPtr[itemCount] = (SInt16)(index + 1);
			itemCount++;
			
			}	// end "if (displayClassGroupsPtr[classIndex] & 0x0100)" 
			
		}	// end "for (index=0; index<numberClasses; index++)" 
		
	displaySpecsPtr->numberDisplayGroups = itemCount;
	
}	// end routine "LoadLocalClassGroupDisplayInfo" 


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadThematicClassNamesIntoList
//
//	Software purpose:	The purpose of this routine is to load the thematic 
//							class names into a list specified by the input handle.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			ClassGroupDialog in SDisplayThematic.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/21/1991
//	Revised By:			Larry L. Biehl			Date: 09/18/2006	

#if defined multispec_mac || defined multispec_mac_swift || defined multispec_win
	void LoadThematicClassNamesIntoList (
				ListHandle							listHandle)
#endif
#if defined multispec_wx
	void LoadThematicClassNamesIntoList (
				wxListBox*							listHandle)
#endif

{
	Cell									cell;
	
	HCharPtr								classNamePtr;
	
	Handle								classNameHandle;
	UInt16								classIndex,
											numberClasses,
											row;


			// Initialize local variables.
			// Note that this routine is not called if the number of classes is
			// more than 32766.													
			
	numberClasses = (UInt16)gImageFileInfoPtr->numberClasses;
	classNameHandle = gImageFileInfoPtr->classDescriptionH;
		
			//	Continue if list handle is not null.
		
	if (listHandle != NULL && classNameHandle != NULL && numberClasses > 0)
		{	
				// Get a pointer to the class name information.							
						                    
		classNamePtr = (HCharPtr)GetHandlePointer (classNameHandle, kLock);
		
				// Delete any rows that are presently in the list.						
			
		#if defined multispec_mac || defined multispec_win
					// This is not needed for wxWidgets version
			RemoveListCells (listHandle);
		#endif
		
				// Now add the correct number of rows to the list.						
	
		row = LAddRow (numberClasses, 0, listHandle);
			
		cell.h = 0;
		cell.v = 0;
		
		for (classIndex=0; classIndex<numberClasses; classIndex++)
			{				
			LSetCell ((HCharPtr)&classNamePtr[1], classNamePtr[0], cell, listHandle);
		
			classNamePtr += 32;
				
			cell.v++;
	
			if (gMemoryError != noErr)
				break;
			
			}	// end "for (classIndex=0; ... 
		
		CheckAndUnlockHandle (classNameHandle);
			
		}	// end "if (listHandle != NULL && ...)" 
			
}	// end Routine "LoadThematicClassNamesIntoList"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		DisplaySpecsPtr LoadThematicDisplaySpecs
//
//	Software purpose:	The purpose of this routine is to load the Display 
//							Specification structure with the default set of
//							parameters for a new Thematic image file which 
//							has been opened.
//
//	Parameters in:		Window pointer to the grafport of the image window.
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/19/1989
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

DisplaySpecsPtr LoadThematicDisplaySpecs (void)

{
	LongRect								longRect;
	
	//char*									strPtr;
	DisplaySpecsPtr					displaySpecsPtr;
	
	Handle								displaySpecsHandle;
	//SInt32								numberBytes;

	Boolean								groupInfoExistsFlag;
										
	
			// Setup and initialize the structure if need.
			
	displaySpecsHandle =  GetActiveDisplaySpecsHandle ();
		
	displaySpecsHandle = GetDisplaySpecsStructure (displaySpecsHandle, TRUE);
																					
	SetActiveDisplaySpecsHandle (displaySpecsHandle);
				
	if (displaySpecsHandle == NULL)
																					return (NULL);
		
			// Lock handle to display specification structure and get pointer to	
			// it.																					
		
	displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
													displaySpecsHandle, kLock, kMoveHi);		
	
			// Initialize the structure if needed.
	
	if (displaySpecsPtr->firstTime)
		{		
				// Set up the "first time" default parameters for thematic image.	
				
				// Get pointer to palette class with space for 256 entries. 
		
		#if defined multispec_win || defined multispec_wx
			displaySpecsPtr->paletteObject = new CMPalette;
			if (displaySpecsPtr->paletteObject == NULL)
																						return (NULL);
																						
			displaySpecsPtr->backgroundPaletteObject = new CMPalette;
			if (displaySpecsPtr->backgroundPaletteObject == NULL)
																						return (NULL);
		#endif	// defined multispec_win || defined multispec_wx													
		
		displaySpecsPtr->numberDisplayClasses = gImageFileInfoPtr->numberClasses;
		displaySpecsPtr->classGroupCode = kClassDisplay;
		
				// Get memory for class/group vector.										
		
		if (InitializeClassGroupsVector (gImageFileInfoPtr, displaySpecsPtr) != noErr)
																						return (NULL);

				// Force pixel size to be no larger than 8 bits.
				
		if (gOSXCoreGraphicsFlag)	
			//displaySpecsPtr->pixelSize = 32;
			displaySpecsPtr->pixelSize = 8;
			
		else	// !gOSXCoreGraphicsFlag
			displaySpecsPtr->pixelSize = MIN (8, displaySpecsPtr->pixelSize);
			
		displaySpecsPtr->displayType = k1_ChannelGrayLevelDisplayType;
			
		displaySpecsPtr->numberLevels = gImageFileInfoPtr->numberClasses;
		displaySpecsPtr->numberPaletteLevels = gImageFileInfoPtr->numberClasses;
		
		if (!CheckIfOffscreenImageExists (gImageWindowInfoPtr))
			SetDisplayImageWindowSizeVariables (displaySpecsPtr);
		
				// Determine whether a default file palette exists for the image.	
			
		displaySpecsPtr->filePaletteFlag = GetDefaultThematicFilePalette (
																gImageWindowInfoPtr,
																gImageFileInfoPtr);
		
				// Initial palette type setting.												
				// Default palette will be read from supporting file if the file	
				// exists.																			
								
		displaySpecsPtr->thematicClassPaletteType = kDefaultColors;
		displaySpecsPtr->thematicGroupPaletteType = kDefaultColors;
		
		if (displaySpecsPtr->filePaletteFlag)
			{			
			displaySpecsPtr->thematicClassPaletteType = kImageDefaultColorTable;
			displaySpecsPtr->thematicGroupPaletteType = kImageDefaultColorTable;
			
			}	// end "if (displaySpecsPtr->filePaletteFlag)" 
			
		else	// !displaySpecsPtr->filePaletteFlag 
			{
					// Determine if this is a statistics image file by whether		
					// .STI is in the file name.  If so then make the default		
					// palette the correlation palette.										
			
			FileStringPtr fileNamePtr =
						(FileStringPtr)GetFileNamePPointerFromFileInfo (gImageFileInfoPtr);
			if (CompareSuffixNoCase ((char*)"\0.sti", fileNamePtr, NULL))
				{
				displaySpecsPtr->thematicClassPaletteType = kCorrelationMatrixColors;
				displaySpecsPtr->thematicGroupPaletteType = kCorrelationMatrixColors;
				gImageWindowInfoPtr->showLegend = FALSE;
				
				}	// end "if (strPtr != NULL)" 
			
			}	// end "else !displaySpecsPtr->filePaletteFlag" 
			
				// Load in any group information if it exists.							
				
		groupInfoExistsFlag = ReadThematicGroups (
									gImageFileInfoPtr, gImageFileInfoPtr, displaySpecsPtr);
		
				// If there is a good chance that this is a probability map then default
				// to the group display.
				
		if (gImageFileInfoPtr->numberClasses == 122 && 
														gImageFileInfoPtr->numberGroups == 16)
			displaySpecsPtr->classGroupCode = kGroupDisplay;
			
				// Indicate that the display specification structure has been		
				// set up the first time.														
				
		displaySpecsPtr->firstTime = FALSE;
		
		}	// end "if (displaySpecsPtr->firstTime)" 
		
	else	// "!displaySpecsPtr->firstTime 
	
				// The display specifications have been set up at least once by	
				// the user for this image window.  Just check to see if the item 
				// make sense.																		
		{
		if (displaySpecsPtr->lineStart > gImageWindowInfoPtr->maxNumberLines)
			displaySpecsPtr->lineStart = 1;
		if (displaySpecsPtr->lineEnd > gImageWindowInfoPtr->maxNumberLines)
			displaySpecsPtr->lineEnd = gImageFileInfoPtr->numberLines;
		if (displaySpecsPtr->lineInterval > gImageWindowInfoPtr->maxNumberLines)
			displaySpecsPtr->lineInterval = 1;
						
		if (displaySpecsPtr->columnStart > gImageWindowInfoPtr->maxNumberColumns)
			displaySpecsPtr->columnStart = 1;
		if (displaySpecsPtr->columnEnd > gImageWindowInfoPtr->maxNumberColumns)
			displaySpecsPtr->columnEnd = gImageWindowInfoPtr->maxNumberColumns;
		if (displaySpecsPtr->columnInterval > gImageWindowInfoPtr->maxNumberColumns)
			displaySpecsPtr->columnInterval = 1;
		
		if ((UInt16)displaySpecsPtr->channelNumber >
														gImageWindowInfoPtr->totalNumberChannels)
			displaySpecsPtr->channelNumber = 1;
		
				// Do not load user selected rectangle information into display 			
				// specification information; just set up information so that the user
				// can use the entire/subset button to choose the selected area. We will 
				// assume that the user just made a selection
				// for zooming and otherwise and does not really want to display
				// a subset of the image.				

		GetSelectedAreaInfo (gActiveImageWindow,
									gImageWindowInfoPtr,
									(SInt32*)&longRect.top,
									(SInt32*)&longRect.bottom,
									(SInt32*)&longRect.left,
									(SInt32*)&longRect.right,
									kDontClearSelectionArea,
									kUseThreshold,
									kDontAdjustToBaseImage);
		
		}	// End "else !displaySpecsPtr->firstTime"
	
	return (displaySpecsPtr);
	
}	// end of "LoadThematicDisplaySpecs"


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadThematicGroupNamesIntoList
//
//	Software purpose:	The purpose of this routine is to load the thematic 
//							group names into a list specified by the input handle.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/21/1991
//	Revised By:			Larry L. Biehl			Date: 02/20/1991	

#if defined multispec_mac || defined multispec_mac_swift || defined multispec_win
	void LoadThematicGroupNamesIntoList (
				ListHandle							listHandle)
#endif
#if defined multispec_wx
	void LoadThematicGroupNamesIntoList (
				wxListBox*							listHandle)
#endif

{
	Cell									cell;
	
	DisplaySpecsPtr					displaySpecsPtr;
	char									*groupNamePtr;
	
	Handle								displaySpecsH,
											groupNameHandle;
											
	SInt16								group,
											numberGroups,
											row;

		
			// Get pointer to display specification.										
			// Handle has already been locked.												
	
	displaySpecsH = GetDisplaySpecsHandle (gImageWindowInfoPtr);
	if (displaySpecsH == NULL)
																								return;
	
	displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (displaySpecsH);
	
			// Initialize local variables.													
			
	numberGroups = gImageFileInfoPtr->numberGroups;
	groupNameHandle = gImageFileInfoPtr->groupNameHandle;
	
			//	Continue if list handle is not null.										
		
	if (listHandle != NULL && groupNameHandle != NULL && numberGroups > 0)
		{	
				// Get a pointer to the group name information.							
						             
		groupNamePtr = (char*)GetHandlePointer (groupNameHandle, kLock);
		
				// Delete any rows that are presently in the list.						
				
		#if defined multispec_mac || defined multispec_win
			RemoveListCells (listHandle);
		#endif
		
				// Now add the correct number of rows to the list.						
	
		row = LAddRow (numberGroups, 0, listHandle);
			
		cell.h = 0;
		cell.v = 0;
		
		for (group=0; group<numberGroups; group++)
			{				
			LSetCell (
				&(groupNamePtr[1]), (short int)(groupNamePtr[0]), cell, listHandle);
		
			groupNamePtr += 32;
				
			cell.v++;
	
			if (gMemoryError != noErr)
				break;
			
			}	// end "for (group=0; ... 
		
		CheckAndUnlockHandle (groupNameHandle);
			
		}	// end "if (listHandle != NULL && ...)" 
			
}	// end Routine "LoadThematicGroupNamesIntoList"  


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void RemoveListCells
//
//	Software purpose:	The purpose of this routine is to remove all cells
//							in the input list to prepare for a new list.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			OpenFieldStatistics in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/09/1996
//	Revised By:			Larry L. Biehl			Date: 04/09/1996	

void RemoveListCells (
				ListHandle							listHandle)

{  
	#if defined multispec_mac 
				// Declare local variables and structures
		
		SInt16							count;
		
		
				// Delete rows in the list															
					
		count = ((ListPtr)*listHandle)->dataBounds.bottom;
		LDelRow (count, 0, listHandle);
	#endif	// defined multispec_mac 
	
	#if defined multispec_win 
		listHandle->ResetContent ();
	#endif	// defined multispec_win 

}	// end "RemoveListCells"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void UpdateLegendWidth
//
//	Software purpose:	The purpose of this routine is to update the legend width to
//                   match the user request in the dialog box.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			in SDisplayThematic.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/09/2016
//	Revised By:			Larry L. Biehl			Date: 05/09/2016

void UpdateLegendWidth ()

{
   Boolean                       redrawScrollBarFlag = FALSE;
   
   
   if (gImageWindowInfoPtr->showLegend && gImageWindowInfoPtr->legendWidth == 0)
      {
      UpdateLegendParameters (GetActiveImageWindowInfoHandle (),
                              gDefaultLegendWidth);
      redrawScrollBarFlag = TRUE;
      
      }	// end "if (windowInfoPtr->showLegend && ..."

   if (!gImageWindowInfoPtr->showLegend && gImageWindowInfoPtr->legendWidth != 0)
      {
      gImageWindowInfoPtr->legendWidth = 0;
      redrawScrollBarFlag = TRUE;
      
      }	// end "if (!windowInfoPtr->showLegend && ..."

         // Redraw the scroll bars if needed.

   if (redrawScrollBarFlag)
      {
      #if defined multispec_mac
         Rect				theBox;

         SetPortWindowPort (gActiveImageWindow);
         DrawScrollBars (gActiveImageWindow);
      
               // Invalidate the area for the legend list so that it
               // will be drawn.
      
      
         GetWindowPortBounds (gActiveImageWindow, &theBox);
         theBox.bottom -= kSBarWidth;
         EraseRect (&theBox);

         InvalWindowRect (gActiveImageWindow, &theBox);
      #endif	// defined multispec_mac
      
      #if defined multispec_win
            CMImageDoc* imageDocCPtr =
											(CMImageDoc*)gActiveImageViewCPtr->GetDocument ();
            CMImageFrame* imageFrameCPtr = imageDocCPtr->GetImageFrameCPtr ();
            imageFrameCPtr->SetLegendWidth (gImageWindowInfoPtr->legendWidth);
      #endif	// defined multispec_win
      
      }	// end "if (redrawScrollBarFlag)"

}	// end "UpdateLegendWidth"

