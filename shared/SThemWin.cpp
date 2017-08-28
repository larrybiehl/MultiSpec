//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
// File:						SThemWin.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision number:		3.1
//
//	Revision date:			07/28/2017
//
//	Language:				C
//
//	System:					Macintosh & Windows Operating Systems
//
//	Brief description:	This file contains routines to hand thematic type windows.
//
//	Functions in file:	void 						AddCellsToLegendList
//								void 						ChangeClassGroupPalette
//								void 						ChangeClassPalette
//								void 						ChangeGroupPalette
//								Boolean 					DoBlinkCursor1
//								void 						DoThematicWColorsUpdate
//								void 						EditClassGroupPalette
//								Boolean					EditGroupClassDialog
//								void						EditGroups
//								Boolean 					GetGroupStructureMemory
//								void 						LoadThematicLegendList
//								void	 					UpdateUserDefinedGroupColorTable
//								void 						UpdateThematicLegendControls
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
/*  Following is template used for testing/debugging
	int numberChars = sprintf ((char*)&gTextString3,
										" SThemWin: (test): %d%s",
										test,
										gEndOfLine);
	ListString ((char*)&gTextString3, numberChars, gOutputTextH);
 */

#include	"SMulSpec.h" 

#if defined multispec_lin
	#include "LEditClassGroupDialog.h"
	#include "LImageDoc.h"
	#include "LImageFrame.h" 
	#include "LImageView.h" 
	#include "LLegendList.h" 
	#include "LLegendView.h" 
	#include "LMainFrame.h" 
	#include "wx/colordlg.h"
#endif	// defined multispec_lin

#if defined multispec_win
	#include "CImagVew.h" 
	#include "WCsGpDlg.h"
	#include "WImagDoc.h"
	#include "WImagFrm.h" 
	#include "WLegendList.h" 
	#include "WMainFrm.h" 
	#include "WMClrDlg.h"
	#include "WTLegend.h" 
	
	#include <commdlg.h>    // common dialog APIs

	extern void 			AnimateEntry (
									CMPaletteInfo			paletteObject,
									UInt16					startIndex,    
									RGBColor*				newRGBColorPtr);
	
	extern Boolean 		StillDown (void);  

	extern UInt16			gDisplayBitsPerPixel;
#endif	// defined multispec_win

#include "SExtGlob.h"

extern void				CreateDefaultGroupTable (
								FileInfoPtr							fileInfoPtr,
								DisplaySpecsPtr					displaySpecsPtr);	


			// Prototypes for routines in this file that are only called by		
			// other routines in this file.														
			
void 						ChangeClassPalette (
								CMPaletteInfo 						paletteHandle,
								CTabHandle 							cTabHandle,
								UInt16	 							paletteIndex, 
								RGBColor*							newRGBColorPtr,
								Boolean 								changeColorTableFlag,
								SInt16								code);

void 						ChangeGroupPalette (
								CMPaletteInfo 						paletteHandle,
								CTabHandle 							cTableHandle, 
								UInt16	 							groupNumber, 
								UInt16								paletteIndex, 
								RGBColor* 							newRGBColorPtr,
								Boolean 								changeColorTableFlag,
								SInt16								code);
								
UInt16					GetPaletteEntry (
								UInt16								paletteIndex);

void	 					UpdateUserDefinedGroupColorTable (
								SInt16								groupNumber,
								SInt16								paletteIndex,
								Boolean								newGroupFlag);



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void AddCellsToLegendList
//
//	Software purpose:	The purpose of this routine is to add the cells to the legend
//							list.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
//	Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/28/1989
//	Revised By:			Larry L. Biehl			Date: 12/24/1996	

void AddCellsToLegendList (
				ListHandle							legendListHandle,
				SInt16								classGroupCode,
				SInt16*								classToGroupPtr,
				UInt16								imageNumberClasses,
				UInt16								imageNumberGroups)

{
	Cell									cell;
	
	UInt32								listLimit;
	
	SInt16								count;
	
	UInt16								cellValue,
											classIndex,
											group,
											numberClasses,
											numberGroups,
											numberInList,
											row;
	
		
			// Now load the legend list table.
			
	listLimit = gClassListLimit;
	if (classGroupCode == kClassDisplay)
		{
				// For spectral class display.												
	
		numberClasses = imageNumberClasses;
		numberGroups = 1; 
		numberInList = numberClasses;
		
		}		// end "if (classGroupCode != kClassDisplay)" 
	
	else if (classGroupCode == kGroupDisplay)
		{
				// For information group display.											
	
		numberClasses = 0;
		numberGroups = imageNumberGroups; 
		numberInList = numberGroups;
		
		}		// end "else classGroupCode == kClassDisplay"
		
	else		// classGroupCode == kGroupClassDisplay
		{
		numberClasses = imageNumberClasses;
		numberGroups = imageNumberGroups; 
		numberInList = numberGroups + numberClasses;
		listLimit = 2 * gClassListLimit;
		
		}		// end "else classGroupCode == kGroupClassDisplay"
		
			// If the number of classes/groups is greater than class list limit then do not	
			// create a legend.																	
			
	if (numberInList > listLimit)
																							return;
	
			// Load the class or group indices into the list.										
				
	LSetDrawingMode (FALSE, legendListHandle);
		
			// Get the current list type.
			                      
	#if defined multispec_mac 
		SInt16 listType = (((ListPtr)*legendListHandle)->refCon & 0x00000006)/2; 
	#endif	// defined multispec_mac 

	#if defined multispec_win || defined multispec_lin
		SInt16 listType = (SInt16)((CMLegendList*)legendListHandle)->m_listType;
	#endif	// defined multispec_win || ...
	
			// Delete any rows that are presently in the list.	
			
	#if defined multispec_mac 
		UInt16 numberInOldList = 
									((ListPtr)*legendListHandle)->dataBounds.bottom; 
	#endif	// defined multispec_mac 

	#if defined multispec_win 
		UInt16 numberInOldList = legendListHandle->GetCount(); 
	#endif	// defined multispec_win 
	
   #if defined multispec_lin 
		UInt16 numberInOldList = legendListHandle->GetItemCount();
   #endif
	
	count = (SInt16)numberInOldList;
	
	if (count > (SInt16)numberInList)
		{
				// Now delete rows to get the number required.
				
		count -= numberInList;
		LDelRow (count, numberInOldList-count, legendListHandle);
		
		}		// end "if (count > numberInList)"
	
	else if (count < (SInt16)numberInList)
		{
				// Now add rows to get the number required.	
				
		count = numberInList - count;
		row = LAddRow (count, numberInOldList, legendListHandle);
		
		}		// end "else if (count < numberInList)"
		
	if (listType != classGroupCode)
		{
				// The new type is different, scroll the list to the top.
		#if defined multispec_mac 
			count = (*legendListHandle)->visible.top;
			LScroll (0, -count, legendListHandle);   
		#endif	// defined multispec_mac 
	
		#if defined multispec_win 
			legendListHandle->SetTopIndex(0); 
		#endif	// defined multispec_win 

		#if defined multispec_lin
				// Do nothing for now
			legendListHandle->Scroll(0, 0);
		#endif
		
		}		// end "if (listType != classGroupCode)"
		
	else		// listType == classGroupCode
		{
				// The new type is the same, make sure that last cell is at bottom of
				// available list window.
				                                
		#if defined multispec_mac 
			count = (*legendListHandle)->visible.bottom - 
														(*legendListHandle)->dataBounds.bottom;
															
			if (count > 0)
				LScroll (0, -count, legendListHandle);  
		#endif	// defined multispec_mac 
	
		#if defined multispec_win                   
		#endif	// defined multispec_win 
		
		}		// end "if (listType != classGroupCode)"
	
	cell.h = 0;
	cell.v = 0;		 
	for (group=0; group<numberGroups; group++)
		{
		if (classGroupCode != kClassDisplay)
			{	
					// Add the index for the group information to the list.
					
			cellValue = group | 0x8000;                  
			#if defined multispec_mac 
				LSetCell ((char*)&cellValue, 
							2, 
							cell, 
							legendListHandle); 
			#endif	// defined multispec_mac 
		
			#if defined multispec_win
				UInt32 longCellValue = (UInt32)cellValue;                                                                  
				legendListHandle->DeleteString (cell.v);
				legendListHandle->InsertString (cell.v, (LPCTSTR)longCellValue);                  
			#endif	// defined multispec_win 

			#if defined multispec_lin
				legendListHandle->SetItemData(cell.v, cellValue);
			#endif
		
			cell.v++;
			
			}		// end "if (classGroupCode != kClassDisplay)"
		
				// Add class index to the list if a class display or
				// the class index for this group to the list if a group-class
				// display.						
		
		if (gMemoryError == noErr && classGroupCode != kGroupDisplay)
			for (classIndex=0; classIndex<numberClasses; classIndex++)
				{			
				if (classGroupCode == kClassDisplay ||
									classToGroupPtr[classIndex] == (SInt16)group)
					{
							// Add the index for the class information to the list.
							
					cellValue = classIndex;                  
					#if defined multispec_mac 
						LSetCell ((char*)&cellValue, 
									2, 
									cell, 
									legendListHandle);
					#endif	// defined multispec_mac 
				
					#if defined multispec_win         
						UInt32 longCellValue = (UInt32)cellValue;                                    
						legendListHandle->DeleteString (cell.v);
						legendListHandle->InsertString (cell.v, (LPCTSTR)longCellValue);                  
					#endif	// defined multispec_win 

					#if defined multispec_lin
						legendListHandle->SetItemData(cell.v, cellValue);
					#endif
				
					cell.v++;
			
					if (gMemoryError != noErr)
						break;
						
					}		// end "if (classToGroupPtr[class] == group)" 
					
				}		// end "for (class=0; class<numberClasses; class++)" 
		
		if (gMemoryError != noErr)
			break;
			
		}		// end "for (group=0; ... 
			
			// Reset the memory error parameter.											
			
	gMemoryError = noErr;
	
			// Turn list drawing mode back on.												
  				
	LSetDrawingMode (TRUE, legendListHandle);
	                   
	UpdateThematicLegendControls (gActiveImageWindow);
	
}		// end "AddCellsToList" 


                   
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ChangeClassGroupPalette
//
//	Software purpose:	The purpose of this routine is to change the palette
//							entries for the requested class or group.  If the call type
//							is '1' then the color table is being changed.  If the
//							call type is '2' then the palette index is only being
//							animated to the background color.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			DoBlinkCursor1 in SThemWin.cpp
//							EditClassGroupPalette in SThemWin.cpp
//							ThematicImageWBlink in thematicWindow.c
//
//	Coded By:			Larry L. Biehl			Date: 01/23/1991
//	Revised By:			Larry L. Biehl			Date: 04/26/2016

void ChangeClassGroupPalette (
				CMPaletteInfo						paletteHandle, 
				SInt16								groupNumber, 
				SInt16								paletteIndex, 
				RGBColor*							newRGBColorPtr, 
				SInt16								callType,
				SInt16								code)

{  
	SignedByte							handleStatus;
	
	DisplaySpecsPtr					displaySpecsPtr;
	
	Handle								displaySpecsHandle;
	
						
	displaySpecsHandle = GetActiveDisplaySpecsHandle();              
			                                        
	displaySpecsPtr = (DisplaySpecsPtr)GetHandleStatusAndPointer(
											displaySpecsHandle, &handleStatus, kNoMoveHi);
	
	if (groupNumber < 0)
		{
////		if (paletteIndex > 0 && paletteIndex < 255)
		if (paletteIndex >= 0 && paletteIndex <= 255)
			{
			CTabHandle		colorTableHandle;
			if (groupNumber == -1)
				colorTableHandle = displaySpecsPtr->savedClassCTableHandle;
				
			else		// groupNumber != -1
				colorTableHandle = displaySpecsPtr->savedGroupCTableHandle;
			
			ChangeClassPalette (paletteHandle, 
										colorTableHandle, 
										paletteIndex, 
										newRGBColorPtr,
										(callType == 1),
										code);
						
			}		// end "if (paletteIndex >= 0 && paletteIndex <= 255)"
						
		if (callType == 1)
			displaySpecsPtr->thematicClassPaletteType = kUserDefinedColors;
		
		}		// end "if (groupNumber < 0)" 
		
	else		// groupNumber >= 0 
		{
		ChangeGroupPalette (paletteHandle, 
									displaySpecsPtr->savedGroupCTableHandle,
									(UInt16)groupNumber, 
									paletteIndex, 
									newRGBColorPtr,
									(callType == 1),
									code);
		
		if (callType == 1)
			displaySpecsPtr->thematicGroupPaletteType = kUserDefinedColors;
			
		}		// end "else groupNumber >= 0" 
		
	MHSetState (displaySpecsHandle, handleStatus); 

}		// end "ChangeClassGroupPalette" 


                   
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ChangeClassPalette
//
//	Software purpose:	The purpose of this routine is to change the requested
//							class(es) to the specified color.  If the option key 
//							was down then all classes with a value less than or
//							equal to that number are also changed.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			ChangeClassGroupPalette  in multiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 09/14/1992
//	Revised By:			Larry L. Biehl			Date: 04/30/2016

void ChangeClassPalette (
				CMPaletteInfo						paletteHandle, 
				CTabHandle							cTableHandle, 
				UInt16								paletteIndex, 
				RGBColor*							newRGBColorPtr, 
				Boolean								changeColorTableFlag,
				SInt16								code)

{                  
	ColorSpec*							colorSpecPtr;
	
	UInt16								endIndex,
											index,
											startIndex;
	
		
	if (newRGBColorPtr == NULL || changeColorTableFlag)
		{
		if (cTableHandle == NULL)
																							return;  
		
		CTabPtr colorTablePtr = (CTabPtr)GetHandlePointer (
											(Handle)cTableHandle, kLock, kNoMoveHi); 
		colorSpecPtr = colorTablePtr->ctTable;
		
		}		// end "if (newRGBColorPtr == NULL || changeColorTableFlag)" 
   
	#if defined multispec_lin
			// Get the original palette back before making changes
		if(cTableHandle != NULL)
			{
			CTabPtr colorTablePtr = (CTabPtr)GetHandlePointer (
												(Handle)cTableHandle, kLock, kNoMoveHi); 
			colorSpecPtr = colorTablePtr->ctTable;
			UInt16 numberClasses = (UInt16)GetActiveNumberClasses ();
			endIndex = MIN(numberClasses, (UInt16)256);
			for (index=0; index<endIndex; index++)
				MSetEntryColor (paletteHandle, 
										index, 
										&colorSpecPtr[index].rgb);
		}
	#endif
		 
	#if defined multispec_mac
//		if ((gEventRecord.modifiers & cmdKey) && !changeColorTableFlag)
		if ((gEventRecord.modifiers & controlKey) && !changeColorTableFlag)
	#endif	// defined multispec_mac 
	
	#if defined multispec_win                        
		if (GetKeyState(VK_CONTROL) < 0 && !changeColorTableFlag)
	#endif	// defined multispec_win 
	
	#if defined multispec_lin
		/*
		int numberChars = sprintf ((char*)&gTextString3,
													" SThemWin:ChangeClassPalette (WXK_CONTROL): %d%s",
													wxGetKeyState(WXK_CONTROL),
													gEndOfLine);
		ListString ((char*)&gTextString3, numberChars, gOutputTextH);
		CheckSomeEvents (updateMask);
		*/
		//if (wxGetKeyState(WXK_CONTROL) && !changeColorTableFlag)
		if (code == 2 && !changeColorTableFlag)
   #endif	// defined multispec_lin 
		{
				// This turns all classes except the selected class to the
				// background color.

		UInt16 numberClasses = (UInt16)GetActiveNumberClasses ();
		endIndex = MIN(numberClasses, (UInt16)256); 
		
		if (gBlinkProcedure == kBlink2)
			{
			if (newRGBColorPtr != NULL)
				{
				for (index=0; index<endIndex; index++)
					if (index != paletteIndex)
						{
						#if defined multispec_mac
							AnimateEntry (gActiveImageWindow, 
												index, 
												newRGBColorPtr);
						#endif	// defined multispec_mac 
						
						#if defined multispec_win          
							AnimateEntry (paletteHandle,
												index, 
												newRGBColorPtr); 
						#endif	// defined multispec_win
						}
											
					}		// end "if (newRGBColorPtr != NULL)" 
					
			else		// newRGBColorPtr == NULL
				{
				for (index=0; index<endIndex; index++)
					if (index != paletteIndex)
						{ 
						#if defined multispec_mac
							AnimateEntry (gActiveImageWindow, 
												index, 
												&colorSpecPtr[index].rgb); 
						#endif	// defined multispec_mac 
						
						#if defined multispec_win          
							AnimateEntry (paletteHandle,
												index,
												&colorSpecPtr[index].rgb); 
						#endif	// defined multispec_win
						}
											
					}		// end "else newRGBColorPtr == NULL"
				
				}		// end "if (gBlinkProcedure == kBlink2)" 
			
		else		// gBlinkProcedure == kBlink1 
			{
			if (newRGBColorPtr != NULL)
				{
				for (index=0; index<endIndex; index++)
					if (index != paletteIndex)
						MSetEntryColor (paletteHandle, 
											index, 
											newRGBColorPtr);
											
				}		// end "if (newRGBColorPtr)" 
					
			else		// newRGBColorPtr == NULL
				{
				for (index=0; index<endIndex; index++)
					if (index != paletteIndex)
						MSetEntryColor (paletteHandle, 
											index, 
											&colorSpecPtr[index].rgb);
											
				}		// end "else newRGBColorPtr == NULL"  
				
			}		// end "else gBlinkProcedure == kBlink1 && ..." 
		
		}		// end "if ((gEventRecord.modifiers & commandKey) && ..." 
		
	else		// !(gEventRecord.modifiers & commandKey) || ...
		{
		startIndex = paletteIndex; 
		#ifndef multispec_lin		
			#if defined multispec_mac                                                 
				if ((gEventRecord.modifiers & optionKey) && !changeColorTableFlag)
			#endif	// defined multispec_mac 
			
			#if defined multispec_win                                  
				if (GetKeyState (VK_RBUTTON) < 0)
			#endif	// defined multispec_win
               startIndex = 1;
		#endif 
		  
		#if defined multispec_lin
			//if (wxGetMouseState().RightIsDown())
			//if (wxGetKeyState(WXK_ALT))
			if (code == 4)
				startIndex = 0;
		#endif
										
		if (gBlinkProcedure == kBlink2 && !changeColorTableFlag)
			{
			if (newRGBColorPtr != NULL)
				for (index=startIndex; index<=paletteIndex; index++)
					{
					#if defined multispec_mac
						AnimateEntry (gActiveImageWindow, index, newRGBColorPtr);
					#endif	// defined multispec_mac 
						
					#if defined multispec_win          
						AnimateEntry (paletteHandle,
											index, 
											newRGBColorPtr);
					#endif	// defined multispec_win 
					
					}		// end "for (index=startIndex; index<=paletteIndex; index++)"
					
			else		// newRGBColorPtr == NULL
				for (index=startIndex; index<=paletteIndex; index++)
					{
					#if defined multispec_mac
						AnimateEntry (gActiveImageWindow, index, &colorSpecPtr[index].rgb); 
					#endif	// defined multispec_mac 
						
					#if defined multispec_win          
						AnimateEntry (paletteHandle,
											index,
											&colorSpecPtr[index].rgb);                        
					#endif	// defined multispec_win  
					}		// end "for (index=startIndex; index<=paletteIndex; index++)"
				
			}		// end "if (gBlinkProcedure == kBlink2)" 
			
		else		// gBlinkProcedure == kBlink1 || changeColorTableFlag
			{
			if (newRGBColorPtr != NULL)
				{
				for (index=startIndex; index<=paletteIndex; index++)
					MSetEntryColor (paletteHandle, index, newRGBColorPtr);
													
				}		// end "if (newRGBColorPtr != NULL)"
					
			else		// newRGBColorPtr == NULL
				for (index=startIndex; index<=paletteIndex; index++)
					MSetEntryColor (paletteHandle, index, &colorSpecPtr[index].rgb);
						
			if (changeColorTableFlag)
				{
				colorSpecPtr[paletteIndex].value     = paletteIndex;
				colorSpecPtr[paletteIndex].rgb		 = *newRGBColorPtr;
										
				}		// end "if (changeColorTableFlag)" 
				
			}		// end "else gBlinkProcedure == kBlink1 && ..." 
			
		}		// end "else !(gEventRecord.modifiers & ..."
		
	CheckAndUnlockHandle ((Handle)cTableHandle);
		
}		// end "ChangeClassPalette" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ChangeGroupPalette
//
//	Software purpose:	The purpose of this routine is to change the requested
//							group(s) to the specified color.  If the option key 
//							was down then all groups with a value less than or
//							equal to that number are also changed.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			ChangeClassGroupPalette  in multiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 09/14/1992
//	Revised By:			Larry L. Biehl			Date: 04/27/2016

void ChangeGroupPalette (
				CMPaletteInfo						paletteHandle, 
				CTabHandle							cTableHandle, 
				UInt16								groupNumber, 
				UInt16								inputPaletteIndex, 
				RGBColor*							newRGBColorPtr, 
				Boolean								changeColorTableFlag,
				SInt16								code)

{                  
	ColorSpec*							colorSpecPtr;
	
	UInt16								*classToGroupPtr,
											*groupToPalettePtr;
										
	UInt16								endIndex,
											index,
											numberClasses,
											paletteIndex,
											startGroup,
											startIndex;
	
	
	if (newRGBColorPtr == NULL || changeColorTableFlag)
		{
		if (cTableHandle == NULL)
																						return;  
		
		CTabPtr colorTablePtr = (CTabPtr)GetHandlePointer (
											(Handle)cTableHandle, kLock, kNoMoveHi); 
		colorSpecPtr = colorTablePtr->ctTable;
		
		}		// end "if (newRGBColorPtr == NULL || changeColorTableFlag)"
		 
   #if defined multispec_lin
				// Get the original palette back before making changes
		if (cTableHandle != NULL)
			{
			CTabPtr colorTablePtr = (CTabPtr)GetHandlePointer (
												(Handle)cTableHandle, kLock, kNoMoveHi); 
			colorSpecPtr = colorTablePtr->ctTable;
			UInt16 numberClasses = (UInt16)GetActiveNumberClasses ();
			endIndex = MIN(numberClasses, (UInt16)256);
			for (index=0; index<endIndex; index++)
				{
				paletteIndex = index + gPaletteOffset;
					MSetEntryColor (paletteHandle, 
										paletteIndex, 
										&colorSpecPtr[paletteIndex].rgb);
				}
			}
	#endif
		
//	numberGroups = ((FileInfoPtr)*gActiveImageFileInfoH)->numberGroups;
//	groupTablesHandle = ((FileInfoPtr)*gActiveImageFileInfoH)->groupTablesHandle;
	classToGroupPtr = (UInt16*)GetClassToGroupPointer (gActiveImageFileInfoH);
	groupToPalettePtr = (UInt16*)GetGroupToPalettePointer (gActiveImageFileInfoH); 
		
	if (classToGroupPtr != NULL && groupToPalettePtr != NULL)
		{                                                                    
		numberClasses = (UInt16)GetActiveNumberClasses ();
		
		startIndex = 0;
		//endIndex = MIN(numberClasses-1, 256); 
		endIndex = numberClasses-1;   

		#if defined multispec_mac
			if ((gEventRecord.modifiers & controlKey) && !changeColorTableFlag)
		#endif	// defined multispec_mac 
		
		#if defined multispec_win                        
			if (GetKeyState(VK_CONTROL) < 0 && !changeColorTableFlag)
		#endif	// defined multispec_win 
      #if defined multispec_lin                        
			//if (wxGetKeyState(WXK_CONTROL) && !changeColorTableFlag)
			if (code == 2 && !changeColorTableFlag)
      #endif
			{
			if (gBlinkProcedure == kBlink2)
				{
				if (newRGBColorPtr)
					{
					for (index=startIndex; index<=endIndex; index++)
						{
						paletteIndex = GetPaletteEntry (index);
						//if (classToGroupPtr[index] != groupNumber)
						if (paletteIndex != inputPaletteIndex)
							{
							//paletteIndex = GetPaletteEntry (index);
							#if defined multispec_mac
								AnimateEntry (gActiveImageWindow, 
													//index + gPaletteOffset,
													paletteIndex, 
													newRGBColorPtr); 
							#endif	// defined multispec_mac 
								
							#if defined multispec_win          
								AnimateEntry (paletteHandle,
													//index + gPaletteOffset,
													paletteIndex, 
													newRGBColorPtr); 
							#endif	// defined multispec_win 
							}
							
						}		// end "for (index=startIndex; index<=endIndex; index++)"
												
					}		// end "if (newRGBColorPtr)" 
						
				else		// !newRGBColorPtr 
					{
					for (index=startIndex; index<=endIndex; index++)
						{
						paletteIndex = GetPaletteEntry (index);
						//if (classToGroupPtr[index] != groupNumber)
						if (paletteIndex != inputPaletteIndex)
							{
							paletteIndex = GetPaletteEntry (index);
							#if defined multispec_mac
								AnimateEntry (	gActiveImageWindow, 
													paletteIndex, 
													&colorSpecPtr[paletteIndex].rgb); 
							#endif	// defined multispec_mac 
								
							#if defined multispec_win          
								AnimateEntry (paletteHandle,
													paletteIndex, 
													&colorSpecPtr[paletteIndex].rgb); 
							#endif	// defined multispec_win 
												
							}		// end "if (classToGroupPtr[index] != ..." 
							
						}		// end "for (index=startIndex; index<=endIndex; index++)"
												
					}		// end "else !newRGBColorPtr"
					
				}		// end "if (gBlinkProcedure == kBlink2 && ...)" 
				
			else		// gBlinkProcedure == kBlink1 
				{
				if (newRGBColorPtr)
					{
					for (index=startIndex; index<=endIndex; index++)
						{
						paletteIndex = GetPaletteEntry (index);
						//if (classToGroupPtr[index] != groupNumber)
						if (paletteIndex != inputPaletteIndex)
							{
							paletteIndex = GetPaletteEntry (index);
							MSetEntryColor (paletteHandle, 
												paletteIndex, 
												newRGBColorPtr);
							}
							
						}		// end "for (index=startIndex; index<=endIndex; index++)"
												
					}		// end "if (newRGBColorPtr)" 
						
				else		// !newRGBColorPtr 
					{
					for (index=startIndex; index<=endIndex; index++)
						{
						paletteIndex = GetPaletteEntry (index);
						//if (classToGroupPtr[index] != groupNumber)
						if (paletteIndex != inputPaletteIndex)
							{
							paletteIndex = GetPaletteEntry (index);
							MSetEntryColor (paletteHandle, 
												paletteIndex, 
												&colorSpecPtr[paletteIndex].rgb);
												
							}		// end "if (classToGroupPtr[index] != ..." 
							
						}		// end "for (index=startIndex; index<=endIndex; index++)"
												
					}		// end "else !newRGBColorPtr" 
					
				}		// end "else gBlinkProcedure == kBlink1" 
		
			}		// end "if ((gEventRecord.modifiers & commandKey) && ..." 
			
		else		// !(gEventRecord.modifiers & commandKey) || ...
			{
			startGroup = groupNumber;     
			#if defined multispec_mac                                                 
				if ((gEventRecord.modifiers & optionKey) && !changeColorTableFlag)
			#endif	// defined multispec_mac 
			#if defined multispec_win                                  
				if (GetKeyState (VK_RBUTTON) < 0)
			#endif	// defined multispec_win   
         #if defined multispec_lin
            //if(wxGetMouseState().RightIsDown())
				//if (wxGetKeyState(WXK_ALT))
				if (code == 4)
         #endif
					startGroup = 0;
				
			if (gBlinkProcedure == kBlink2 && !changeColorTableFlag)
				{
				if (newRGBColorPtr)
					{
					for (index=startIndex; index<=endIndex; index++)
						if (classToGroupPtr[index] >= startGroup && 
														classToGroupPtr[index] <= groupNumber)
							{ 
							paletteIndex = GetPaletteEntry (index);
							//paletteIndex = groupToPalettePtr [classToGroupPtr[index]];
							#if defined multispec_mac
								AnimateEntry (gActiveImageWindow, 
													paletteIndex, 
													newRGBColorPtr);  
							#endif	// defined multispec_mac 
								
							#if defined multispec_win          
								AnimateEntry (paletteHandle,
													index + gPaletteOffset, 
													newRGBColorPtr); 
							#endif	// defined multispec_win 
                  }
												
					}		// end "if (newRGBColorPtr)" 
					
				else		// !newRGBColorPtr 
					{
					for (index=startIndex; index<=endIndex; index++)
						if (classToGroupPtr[index] >= startGroup && 
														classToGroupPtr[index] <= groupNumber)
							{
							paletteIndex = GetPaletteEntry (index);
							//paletteIndex = groupToPalettePtr [classToGroupPtr[index]];
							#if defined multispec_mac
								AnimateEntry (	gActiveImageWindow, 
													paletteIndex, 
													&colorSpecPtr[paletteIndex].rgb);  
							#endif	// defined multispec_mac 
								
							#if defined multispec_win          
								AnimateEntry (paletteHandle,
													paletteIndex, 
													&colorSpecPtr[paletteIndex].rgb); 
							#endif	// defined multispec_win 
												
							}		// end "if (classToGroupPtr[index] >= ..." 
												
					}		// end "else !newRGBColorPtr"
					
				}		// end "if (gBlinkProcedure == kBlink2 && ...)" 
					
			else		// gBlinkProcedure == kBlink1 || changeColorTableFlag 
				{
				if (newRGBColorPtr)
					{
					for (index=startIndex; index<=endIndex; index++)
						{
						if (classToGroupPtr[index] >= startGroup && 
														classToGroupPtr[index] <= groupNumber)
							{
							paletteIndex = GetPaletteEntry (index);
							//paletteIndex = groupToPalettePtr [classToGroupPtr[index]];
							MSetEntryColor (paletteHandle, 
												paletteIndex, 
												newRGBColorPtr);
					
							if (changeColorTableFlag)
								{
								colorSpecPtr[paletteIndex].value     = paletteIndex;
								colorSpecPtr[paletteIndex].rgb   	= *newRGBColorPtr;
								
								}		// end "if (changeColorTableFlag)" 

							}		// end "if (changeColorTableFlag)"

						}		// end "for (index=startIndex; index<=endIndex; index++)"
												
					}		// end "if (newRGBColorPtr)" 
							
				else		// !newRGBColorPtr 
					{
					for (index=startIndex; index<=endIndex; index++)
						{
						if (classToGroupPtr[index] >= startGroup && 
														classToGroupPtr[index] <= groupNumber)
							{
							paletteIndex = GetPaletteEntry (index);
							MSetEntryColor (paletteHandle, 
												paletteIndex, 
												&colorSpecPtr[paletteIndex].rgb);
												
							}		// end "if (classToGroupPtr[index] >= ..." 

						}		// end "for (index=startIndex; index<=endIndex; index++)"
												
					}		// end "else !newRGBColorPtr" 
						
				}		// end "else gBlinkProcedure == kBlink1 || ..." 
				
			}		// end "else !(gEventRecord.modifiers & commandKey) || ..." 
		
		UnlockGroupTablesHandle (gActiveImageFileInfoH);
				
		}		// end "if (classToGroupPtr != NULL)"
		
	if (!newRGBColorPtr || changeColorTableFlag)
		CheckAndUnlockHandle ((Handle)cTableHandle); 
		
}		// end "ChangeGroupPalette"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CheckForDuplicateName
//
//	Software purpose:	The purpose of this routine is to check if the input
//							class or group name is a duplicate.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/07/1997
//	Revised By:			Larry L. Biehl			Date: 03/16/2017	

UInt16 CheckForDuplicateName (
				SInt16						newEditCode,
				Handle						nameHandle,
				Handle						fileInfoHandle,
				UCharPtr						textStringPtr,
				SInt16*						classGroupIndexPtr,
				Boolean*						noChangeFlagPtr)

{						
	Boolean			stringSameFlag;
	
	SInt16			numChars;
	
	UInt16			index,
						numberClassesGroups;
						
				
			// Check if proposed class or group name is a duplicate.	 If it  		
			// is, allow user to change it again.
					
	FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (
												fileInfoHandle, kNoLock, kNoMoveHi);
	
	if (newEditCode == kEditClass)
		numberClassesGroups = (UInt16)fileInfoPtr->numberClasses;
	
	else		// newEditCode != kEditClass
		{
		numberClassesGroups = fileInfoPtr->numberGroups;
					
		if (newEditCode == kNewGroup)
			*classGroupIndexPtr = numberClassesGroups;
			
		}		// end "else newEditCode != kEditClass"
		
	char* namePtr = (char*)GetHandlePointer (nameHandle, kNoLock, kNoMoveHi);
	stringSameFlag = FALSE;
	*noChangeFlagPtr = FALSE;
	for (index=0; index<numberClassesGroups; index++)
		{
		if (namePtr[0] == textStringPtr[0])
			{
			numChars = textStringPtr[0];
			if (strncmp(  
					(char*)&textStringPtr[1], (char*)&namePtr[1], numChars) == 0)
				{
				if (index != (UInt16)*classGroupIndexPtr)
					stringSameFlag = TRUE;
					
				else		// index == classGroupIndex
					*noChangeFlagPtr = TRUE;
				
				}		// end "if (strncmp(..." 
						
			}		// end "if (namePtr[0] != textStringPtr[0])"
			
		namePtr += 32;
			
		if (stringSameFlag)
			break;
	
		}		// end "for (index=0; ...
		
	if (stringSameFlag)
		numberClassesGroups = 0;
		
	return (numberClassesGroups); 
	
}		// end "CheckForDuplicateName"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CreateDefaultGroupTable
//
//	Software purpose:	The purpose of this routine is to create a default group table
//							based on the input color table if there are similar colors in
//							a row.
//
//	Parameters in:		Window pointer to the grafport of the image window.
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
//	Called By:			DisplayThematicImage in displayThematic.c
//							UpdateActiveImageLegend in SPalette.c
//							PrintImageWindow in print.c
//
//	Coded By:			Larry L. Biehl			Date: 04/23/2011
//	Revised By:			Larry L. Biehl			Date: 06/22/2015	

void CreateDefaultGroupTable (
				FileInfoPtr							fileInfoPtr,
				DisplaySpecsPtr					displaySpecsPtr)

{ 
	char*									groupNamePtr;
	
	ColorSpec							*classColorSpecPtr;
											
	CTabPtr								cTablePtr;
	
	SInt16								*classToGroupPtr,
											*groupToPalettePtr;
	
	UInt32								bytesNeeded,
											groupCount,
											index,
											numberClasses;
											
	SInt16								returnCode,
											stringLength;
	
	Boolean								changedFlag,
											continueFlag,
											groupNameChangedFlag,
											groupTableChangedFlag,
											newGroupFlag,
											sameColorFlag = FALSE;
							
	
	if (fileInfoPtr == NULL || displaySpecsPtr == NULL)
																									return;
																									
			// A default group table will be created only if the number of colors 
			// in the color table is the same as the number of classes.
			
	numberClasses = fileInfoPtr->numberClasses;
	cTablePtr = (CTabPtr)GetHandlePointer (
					(Handle)displaySpecsPtr->savedClassCTableHandle, kNoLock, kNoMoveHi);
			
	if (cTablePtr == NULL || numberClasses != cTablePtr->ctSize+1)
																									return;
																											
			// Get the class color table
			
	cTablePtr = (CTabPtr)GetHandlePointer (
					(Handle)displaySpecsPtr->savedClassCTableHandle, kLock, kNoMoveHi);
	classColorSpecPtr = cTablePtr->ctTable;
		
			// Determine if there are identical colors in a row in the color table. 
			// This will be used as the key to create a default group table.
	
	//numberClassesLessOne = numberClasses - 1;	
	for (index=1; index<numberClasses; index++)
		{
		if (classColorSpecPtr[index].rgb.red == classColorSpecPtr[index-1].rgb.red &&
				classColorSpecPtr[index].rgb.green == classColorSpecPtr[index-1].rgb.green &&
					classColorSpecPtr[index].rgb.blue == classColorSpecPtr[index-1].rgb.blue)
			{
			sameColorFlag = TRUE;
			break;
			
			}		// end "if (colorSpecPtr[index].rgb.red == ..."
			
		}		// end "for (index=0; index<numberClassesLessOne; index++)"
		
	if (sameColorFlag)
		{
		continueFlag = GetGroupStructureMemory (fileInfoPtr,
																&groupToPalettePtr,
																&groupNamePtr,
																&groupTableChangedFlag,
																&groupNameChangedFlag);
		
		if(continueFlag)
			{
					// Create default group list based on the colors in the color table.
							
			groupCount = 0;
			classToGroupPtr = GetClassToGroupPointer (fileInfoPtr);
						
			newGroupFlag = TRUE;			
			for (index=0; index<numberClasses; index++)
				{
				if (index > 0)
					{
					if (classColorSpecPtr[index].rgb.red != classColorSpecPtr[index-1].rgb.red ||
							classColorSpecPtr[index].rgb.green != classColorSpecPtr[index-1].rgb.green ||
								classColorSpecPtr[index].rgb.blue != classColorSpecPtr[index-1].rgb.blue)
						{
						newGroupFlag = TRUE;
						
						}		// end "colorSpecPtr[index].rgb.red != colorSpecPtr[index-1].rgb.red || ..."
						
					}		// end "if (index > 0)"
					
				if (newGroupFlag)
					{	
					groupToPalettePtr[groupCount] = (SInt16)index;
					groupCount++;
					
							// Load default group name.
							
					stringLength = sprintf (&groupNamePtr[1],
														"Group %d", 
														groupCount);
					groupNamePtr[0] = (UInt8)stringLength;
					groupNamePtr += 32;
					
					newGroupFlag = FALSE;
					
					}		// end "if (newGroupFlag)"
					
				classToGroupPtr[index] = (SInt16)groupCount - 1;
				
				}		// end "for (index=0; index<colorTableCount; index++)"
																					
			fileInfoPtr->numberGroups = (UInt16)groupCount;
			fileInfoPtr->groupChangedFlag = TRUE;
			
			#if defined multispec_lin
					// Make sure that wxWidgets knows the document has changed.
				gActiveImageViewCPtr->GetDocument()->Modify(TRUE);
			#endif // defined multispec_lin
			
					// Get the memory for and then load the group color table. It will
					// be identical to the class color table to start with.										
			
			bytesNeeded = MGetHandleSize ((Handle)displaySpecsPtr->savedClassCTableHandle);	
				
			cTablePtr = (CTabPtr)CheckHandleSize ( 
										(Handle*)&displaySpecsPtr->savedGroupCTableHandle,
										&continueFlag, 
										&changedFlag,  
										bytesNeeded);
										
			returnCode = CopyHandleToHandle ((Handle)displaySpecsPtr->savedClassCTableHandle,
														(Handle)displaySpecsPtr->savedGroupCTableHandle);
			
			if (returnCode == noErr)
				{
				cTablePtr = (CTabPtr)GetHandlePointer (
					(Handle)displaySpecsPtr->savedGroupCTableHandle, kNoLock, kNoMoveHi);

				if (cTablePtr != NULL)
					{
					cTablePtr->ctSeed = GetCTSeed ();
					cTablePtr->ctFlags = 0;
					
					}		// end "if (cTablePtr != NULL)"
				
								// Test
				
//				RGBColor	theColor;				
//				classColorSpecPtr = cTablePtr->ctTable;
//				theColor = classColorSpecPtr[255].rgb;
				
				}		// end "if (if (returnCode == noErr))"
				
			}		// end "if (continueFlag)"
			
		CheckAndUnlockHandle (fileInfoPtr->groupNameHandle);
		CheckAndUnlockHandle (fileInfoPtr->groupTablesHandle);
		
		}		// end "if (sameColorFlag)"
	
	CheckAndUnlockHandle ((Handle)displaySpecsPtr->savedClassCTableHandle);
	
}		// end "CreateDefaultGroupTable"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoBlinkCursor1
//
//	Software purpose:	The purpose of this routine is to handle blinking the
//							colors from the legend palette..
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			LegendCClickLoop  in thematicWindow.c
//
//	Coded By:			Larry L. Biehl			Date: 01/16/1997
//	Revised By:			Larry L. Biehl			Date: 05/01/2016

Boolean DoBlinkCursor1 (
				ListHandle							legendListHandle,
				Point									lCell,
				SInt16								listType,
				SInt16								code)

{ 
	RGBColor								oldRGB;
	SignedByte							handleStatus;
	
	DisplaySpecsPtr					displaySpecsPtr;
	
	Handle								displaySpecsHandle;
											
	CMPaletteInfo						paletteHandle;
	SInt16								cellLine,
											paletteIndex;
											
	#if defined multispec_lin
		bool                          ctrlflag,
												altflag,
												pctrlflag,
												paltflag,
												changeflag;
	#endif	// defined multispec_lin
									
									
			// Set up for changes in color.												
			
	paletteHandle = GetActivePaletteHandle ();
	if (paletteHandle == NULL)
																			return (FALSE);

			// Get current color for class or group.
	
	if (lCell.v < 0)
																			return (TRUE);
		
	displaySpecsHandle = GetActiveDisplaySpecsHandle ();
	displaySpecsPtr = (DisplaySpecsPtr)GetHandleStatusAndPointer(
													displaySpecsHandle,
													&handleStatus,
													kNoMoveHi);
													
	gPaletteOffset = displaySpecsPtr->paletteOffset;
	gClassPaletteEntries = displaySpecsPtr->numPaletteEntriesUsed;
	
	if (listType == kClassDisplay)		// Class list.	
		{
		paletteIndex = lCell.v;
		
				// lCell.v will now indicate that this is a class list 			
				// not a group list.															
				
		cellLine = -1;
		
		}		// end "if (listType == kClassDisplay)" 
			
	else		// listType != kClassDisplay, i.e. Group or Group-Class list.
		{
		cellLine = lCell.v;
		SInt16 classGroupCode = 1;
																
		if (listType == kGroupClassDisplay)
			{            
			#if defined multispec_mac 
				DataHandle cellData = ((ListPtr)*legendListHandle)->cells;
				BlockMoveData (&((*cellData)[lCell.v*2]), &cellLine, 2);
			#endif	// defined multispec_mac 
			
			#if defined multispec_win || defined multispec_lin
				cellLine = (SInt16)legendListHandle->GetItemData(lCell.v); 
			#endif	// defined multispec_win 
		
			classGroupCode = (cellLine & 0x8000); 
			cellLine &= 0x7fff;
			
			}		// end "if (listType == kGroupClassDisplay)"
		
		if (classGroupCode == 0)
			{
					// This is a class cell
					
			paletteIndex = cellLine;
			cellLine = -2;
			
			}		// end "if (classGroupCode == 0)"
			
		else		// classGroupCode != 0
			{				   
			#if defined multispec_mac
				Handle fileInfoHandle = (*legendListHandle)->userHandle;
			#endif	// defined multispec_mac 
			
			#if defined multispec_win || defined multispec_lin
				Handle fileInfoHandle = 
								((CMLegendList*)legendListHandle)->m_imageFileInfoHandle;  
			#endif	// defined multispec_win 
											  
			FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (
													fileInfoHandle, kNoLock, kNoMoveHi);
			SInt16* classGroupToPalettePtr = (SInt16*)GetHandlePointer(
										fileInfoPtr->groupTablesHandle, kNoLock, kNoMoveHi); 
			paletteIndex = classGroupToPalettePtr[cellLine];
			
			}		// end "else classGroupCode != 0"
		
		}		// end "else listType != kClassDisplay"
		
	paletteIndex = GetPaletteEntry (paletteIndex);	
	MGetEntryColor (paletteHandle, paletteIndex, &oldRGB);

	#if defined multispec_win
			// Set blinking procedure to use for Windows OS.

		gBlinkProcedure = kBlink2;
		if (gDisplayBitsPerPixel > 8)
			gBlinkProcedure = kBlink1;
	#endif	// define multispec_win
	
	#if defined multispec_lin
		gBlinkProcedure = kBlink1;
		//pctrlflag = wxGetKeyState(WXK_CONTROL);
		//prclickflag = wxGetMouseState().RightIsDown();
		//paltflag = wxGetKeyState(WXK_ALT);
	#endif
	
	ChangeClassGroupPalette (paletteHandle, 
										cellLine, 
										paletteIndex, 
										&displaySpecsPtr->backgroundColor,
										2,
										code);
		
	#if defined multispec_mac 
		if (gBlinkProcedure == kBlink1)
			DoThematicWColorsUpdate (); 
	#endif	// defined multispec_win
	
	#if defined multispec_lin
		//((CMLegendList*) legendListHandle)->m_paletteObject = (wxPalette*)paletteHandle;
	#endif
								
	#if defined multispec_win || defined multispec_lin
		DoThematicWColorsUpdate ();
	#endif	// defined multispec_win    
	
	MSetCursor (kBlinkShutCursor);
	#ifndef multispec_lin
		while (StillDown ())
			{ }
	#endif

	#if defined multispec_lin
		//if (code == 1)
			while (wxGetKeyState(WXK_SHIFT))
				{
					//ctrlflag = wxGetKeyState(WXK_CONTROL);
				//rclickflag = wxGetMouseState().RightIsDown();
				//altflag = wxGetKeyState(WXK_ALT);
				/*
				int numberChars = sprintf ((char*)&gTextString3,
													" SThemWin:DoblinkCursor1 (ctrlflag, altflag, pctrlflag, paltflag): %d, %d, %d, %d%s",
													ctrlflag,
													altflag,
													pctrlflag,
													paltflag,
													gEndOfLine);
				ListString ((char*)&gTextString3, numberChars, gOutputTextH);
				CheckSomeEvents (updateMask);
				*/
				/*
				if (ctrlflag != pctrlflag || altflag != paltflag)
					{
					pctrlflag = ctrlflag;
					paltflag = altflag;
					
					ChangeClassGroupPalette (paletteHandle, 
												cellLine, 
												paletteIndex, 
												&displaySpecsPtr->backgroundColor,
												2,
												code);
					//((CMLegendList*) legendListHandle)->m_paletteObject = (wxPalette*)paletteHandle;
					DoThematicWColorsUpdate ();
				
					}		// end "if (ctrlflag != pctrlflag || altflag != paltflag)"
				*/
				}		// end "while (wxGetKeyState(WXK_SHIFT))"
		/*
		else if (code == 2)
			while (wxGetKeyState(WXK_CONTROL))
				{
				}
	
		else if (code == 4)
			while (wxGetKeyState(WXK_ALT))
				{
				}
		*/
	#endif
											  
	ChangeClassGroupPalette (paletteHandle,
										cellLine, 
										paletteIndex,  
										NULL, 
										2,
										code);
	
	#if defined multispec_lin
		//((CMLegendList*) legendListHandle)->m_paletteObject = (wxPalette*)paletteHandle;
		//((CMLegendList*) legendListHandle)->m_paletteObject = GetActivePaletteHandle ();
	#endif
	
	#if defined multispec_mac 
		if (gBlinkProcedure == kBlink1)
			DoThematicWColorsUpdate (); 
	#endif	// defined multispec_win 
								
	#if defined multispec_win || defined multispec_lin 
		DoThematicWColorsUpdate ();
	#endif	// defined multispec_win    
	
	#ifndef multispec_lin
		MSetCursor (kBlinkOpenCursor1);
	#endif
	
	MHSetState (displaySpecsHandle, handleStatus);
		
	return (FALSE);
		
}		// end "DoBlinkCursor1"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoThematicWColorsUpdate
//
//	Software purpose:	This routine handles the update of colors in an
//							active Thematic image window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			LegendCClickLoop in thematicWindow.c
//							ThematicImageWBlink in thematicWindow.c
//
//	Coded By:			Larry L. Biehl			Date: 01/28/1991
//	Revised By:			Larry L. Biehl			Date: 07/24/1998			

void DoThematicWColorsUpdate (void)

{
	#if defined multispec_mac 
		Rect						clipRect;
		
		ListHandle				legendListHandle;
		PaletteHandle			paletteHandle;
		
				
				// Initialize local variables.													
				
		legendListHandle = ((WindowInfoPtr)*gActiveImageWindowInfoH)->legendListHandle;
		paletteHandle = GetPalette (gActiveImageWindow);
		
				// Make sure that the latest palette is written to the window 			
				// palette.																				
		
		ActivateImageWindowPalette (paletteHandle);
		
		BeginUpdate (gActiveImageWindow);
		GetWindowClipRectangle (gActiveImageWindow, kImageFrameArea, &clipRect);
		CopyOffScreenImage (gActiveImageWindow, NIL, &clipRect, kDestinationCopy, kNotFromImageUpdate);
		if (((WindowInfoPtr)*gActiveImageWindowInfoH)->showLegend)
			{
			clipRect.left = 0;
			clipRect.right = ((WindowInfoPtr)*gActiveImageWindowInfoH)->legendWidth;
			ClipRect (&clipRect);
			LUpdate (GetPortVisibleRegion (
					GetWindowPort (gActiveImageWindow), gTempRegion1), legendListHandle);
			SetEmptyRgn (gTempRegion1);
	
			#if TARGET_API_MAC_CARBON	
				if (QDIsPortBuffered (GetWindowPort(gActiveImageWindow)))
					QDFlushPortBuffer (GetWindowPort(gActiveImageWindow), NULL);
			#endif	// TARGET_API_MAC_CARBON
			
			}		// end "((WindowInfoPtr)*gActiveImageWindowInfoH)->..." 
			
		EndUpdate (gActiveImageWindow);
		
				// Set the clip area back to just the image area and legend list.		
				
		GetWindowClipRectangle (gActiveImageWindow, kImageFrameArea, &clipRect);
		clipRect.left = 0;
		ClipRect (&clipRect); 
	#endif	// defined multispec_mac  
		
		
	#if defined multispec_win 
		CMainFrame* pAppFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
		pAppFrame->SendMessage(WM_QUERYNEWPALETTE, NULL, NULL);
					                                      
		CMImageDoc* imageDocCPtr = gActiveImageViewCPtr->GetDocument();
		CMImageFrame* imageFrameCPtr = imageDocCPtr->GetImageFrameCPtr();
		imageFrameCPtr->UpdateWindow();  
	#endif	// defined multispec_win 

#if defined multispec_lin
      CMImageCanvas* canvasptr = gActiveImageViewCPtr->m_Canvas;
      canvasptr->Refresh();
      canvasptr->Update();
      CMImageFrame* frameptr = gActiveImageViewCPtr->m_frame;
      (frameptr->GetLegendViewCPtr())->Update();
      ((frameptr->GetLegendViewCPtr())->GetLegendListCPtr())->DrawLegendList();
      ((frameptr->GetLegendViewCPtr())->GetLegendListCPtr())->Update();
#endif

	
}		// end "DoThematicWColorsUpdate" 


 
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void EditClassGroupPalette
//
//	Software purpose:	The purpose of this routine is to get the rectangle
//							that encloses the thematic legend popup box.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 11/06/1996
//	Revised By:			Larry L. Biehl			Date: 04/26/2016	

void EditClassGroupPalette (
				ListHandle							legendListHandle,
				Handle								displaySpecsH,
				SInt16								cellLine,
				SInt16								listType)

{
	Cell									lCell;
	CMPaletteInfo						paletteHandle = NULL;
	
	RGBColor								newRGB,
											oldRGB;
	
	DisplaySpecsPtr					displaySpecsPtr;
	SInt16*								displayClassGroupsPtr;
	
	Handle								displayClassGroupsHandle;
	
	SInt16								displayClassGroupCode,
											paletteIndex;
									
									
			// Set up for changes in color only if color quickdraw is present.
		
	paletteHandle = GetActivePaletteHandle();
	
	if (paletteHandle == NULL)
																					return;

			// Get current color for class or group.
			
	#if defined multispec_mac 
		if (cellLine < 0 || cellLine > (*legendListHandle)->dataBounds.bottom)
	#endif
	
	#if defined multispec_win
		if (cellLine < 0 || cellLine > (legendListHandle->GetCount()))
	#endif
         
	#if defined multispec_lin
      if (cellLine < 0 || cellLine > (legendListHandle->GetItemCount()))
	#endif

																									return;

			// Get information to check if this class is being displayed.

	displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer(displaySpecsH,
																			kNoLock,
																			kNoMoveHi);
	
	displayClassGroupsHandle = displaySpecsPtr->displayClassGroupsHandle;
	displayClassGroupsPtr = (SInt16*)GetHandlePointer (displayClassGroupsHandle,
																			kNoLock,
																			kNoMoveHi);	
																																				
	if (displayClassGroupsPtr == NULL)
																									return;
																				
	displayClassGroupCode = displaySpecsPtr->classGroupCode;
	gClassPaletteEntries = displaySpecsPtr->numPaletteEntriesUsed;
	gPaletteOffset = displaySpecsPtr->paletteOffset;
	
	if (listType == kClassDisplay)		// Class list.	
		{
		paletteIndex = cellLine;
		
		if (!(displayClassGroupsPtr[cellLine] & 0x0001))
																									return;
		
				// Set cellLine so that it indicates that this is a class	
				// list and not a group list.											
																							
		cellLine = -1;
		
		}		// end "if (listType == kClassDisplay)" 
			
	else		// listType != kClassDisplay 		// Group list. 
		{
		if (listType == kGroupClassDisplay)
			{
			lCell.h = 0;
			lCell.v = cellLine;
			#if defined multispec_mac 
				DataHandle cellData = ((ListPtr)*legendListHandle)->cells;
				BlockMoveData (&((*cellData)[lCell.v*2]), &cellLine, 2);
			#endif	// defined multispec_mac 
				
			#if defined multispec_win 
				cellLine = (SInt16)legendListHandle->GetItemData(lCell.v); 
			#endif	// defined multispec_win 

			#if defined multispec_lin 
				cellLine = (SInt16)legendListHandle->GetItemData(lCell.v);
			#endif	// defined multispec_win 

			SInt16 classGroupCode = (cellLine & 0x8000);
					
					// If this is a class in a group-class list then
					// exit. No color chip will be modified in this case.
					
			if (classGroupCode == 0)
																					return;
			cellLine &= 0x7fff;
			
			}		// end "if (listType == kGroupClassDisplay)"
			
		#if defined multispec_mac
			Handle fileInfoHandle = (*legendListHandle)->userHandle;
		#endif	// defined multispec_mac 
				
		#if defined multispec_win || defined multispec_lin
			Handle fileInfoHandle = 
						((CMLegendList*)legendListHandle)->m_imageFileInfoHandle;  
		#endif	// defined multispec_win

		FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (
												fileInfoHandle, kNoLock, kNoMoveHi);
		SInt16* classGroupToPalettePtr = (SInt16*)GetHandlePointer(
								fileInfoPtr->groupTablesHandle, kNoLock, kNoMoveHi); 
		paletteIndex = classGroupToPalettePtr[cellLine];

		if (!(displayClassGroupsPtr[paletteIndex] & 0x0100))
																					return;
		
		}		// end "else userHandle != NULL" 
						
	paletteIndex = GetPaletteEntry (paletteIndex);
	/*
	if (paletteIndex >= gClassPaletteEntries)
		{
		UInt16 paletteEntries = MIN(gClassPaletteEntries, 256);
		paletteIndex -= paletteEntries;
		paletteIndex = paletteIndex % (paletteEntries-1) + 1;
		
		}		// end "if (paletteIndex >= gClassPaletteEntries)"
	*/		
	GetEntryColor (paletteHandle, paletteIndex, &oldRGB);
	
	if (SelectColor (4, &oldRGB, &newRGB))
		{
		ChangeClassGroupPalette (paletteHandle, 
											cellLine, 
											paletteIndex, 
											&newRGB, 
											1,
											0);
				
		FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer(
																gActiveImageFileInfoH,
																kNoLock,
																kNoMoveHi);
	
		if (displayClassGroupCode == kClassDisplay)
			fileInfoPtr->classChangedFlag = TRUE;
	
		else		// displayClassGroupCode != kClassDisplay
			fileInfoPtr->groupChangedFlag = TRUE;	
			
		#if defined multispec_lin
				// Make sure that wxWidgets knows the document has changed.
			gActiveImageViewCPtr->GetDocument()->Modify(TRUE);
		#endif // defined multispec_lin
								
		#if defined multispec_mac										
			gUpdateFileMenuItemsFlag = TRUE; 
//			UpdatePaletteWindow (TRUE, FALSE);
			ActivateImageWindowPalette (paletteHandle);
		#endif

		#if defined multispec_win || defined multispec_lin
			displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer(displaySpecsH,
																					kNoLock,
																					kNoMoveHi);
			displaySpecsPtr->paletteUpToDateFlag = FALSE;
			UpdateActiveImageLegend (listType, kCallCreatePalette);
		#endif
		
		}		// end "if (SelectColor (4, &oldRGB, &newRGB))" 
	
}		// end "EditClassGroupPalette"

 
/*
#if defined multispec_win  
UINT ColorDialogHookProcedure (
				HWND									dialogPtr,
				UINT									uiMessage,
				WPARAM								wParam, 
				LPARAM								lParam)

{	
	if (uiMessage == WM_INITDIALOG)
		{                            
		PositionDialogWindow ((CDialog*)dialogPtr);
		
		return (TRUE);
		
		}		// end "if (uiMessage == WM_INITDIALOG)"
		
	else		//
		return (FALSE);
	
}		// end "ColorDialogHookProcedure" 
#endif
*/


//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean EditGroupClassDialog
//
//	Software purpose:	The purpose of this routine is to present a dialog
//							to allow the user to enter or edit the group name.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			ModalGroupsDialog   in displayThematic.c
//
//	Coded By:			Larry L. Biehl			Date: 01/16/1991
//	Revised By:			Larry L. Biehl			Date: 12/16/2016

Boolean EditGroupClassDialog (
				ListHandle							legendListHandle,
				SInt16								selectedCell,
				SInt16								newEditCode, 
				SInt16								classGroupIndex)

{	
	Boolean							OKFlag;
	
	Handle							fileInfoHandle,
										nameHandle = NULL;
	
	
			// Get the correct handle to the list names to be used.
			
	fileInfoHandle = GetActiveImageFileInfoHandle ();
	FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (
												fileInfoHandle, kNoLock, kNoMoveHi);
	
	if (newEditCode == kEditClass)
		nameHandle = fileInfoPtr->classDescriptionH;
	
	else		// newEditCode != kEditClass
		nameHandle = fileInfoPtr->groupNameHandle;
	
#if defined multispec_mac  
	Str255							textString;	
	
	Handle							okHandle,
										theHandle;
									
	Boolean							modalDone,
										noChangeFlag;
	
	Rect								theBox;
	
	char*								namePtr;
	
	SInt16							itemHit,
										maxStringLength = kMaxClassGroupNameLength,
										theType;
	
	DialogPtr						dialogPtr;
	

			// Get the modal dialog for the channel description specification.	
			
	dialogPtr = LoadRequestedDialog (kGroupNameDialogID, kCopyScrap, 1, 2);
	if (dialogPtr == NULL)													
																					return(FALSE);
	
			// Save handle for the OK button for use later.								
			
	GetDialogItem (dialogPtr, 1,  &theType, &okHandle, &theBox);
							
			// item = 5: Default class or group name.													
			
	GetDialogItem (dialogPtr, 5, &theType, &theHandle, &theBox);
		
	HLock (nameHandle);
	namePtr = (char*)*nameHandle;
	namePtr = &namePtr[classGroupIndex*32];
			
	if (newEditCode == kNewGroup)
		{
		SetWTitle (GetDialogWindow(dialogPtr), "\pEnter New Thematic Group Name");
			
		namePtr = (char*)GetHandlePointer(
									((FileInfoPtr)*gActiveImageFileInfoH)->classDescriptionH,
									kLock,
									kNoMoveHi);
		namePtr = &namePtr[classGroupIndex*32];
		SetDialogItemText (theHandle, (ConstStr255Param)namePtr);
		
		CheckAndUnlockHandle (((FileInfoPtr)*gActiveImageFileInfoH)->classDescriptionH);
		
		}		// end "if (newEditCode == kNewGroup)" 
		
	if (newEditCode == kEditGroup)
		{
		SetWTitle (GetDialogWindow(dialogPtr), "\pEdit Thematic Group Name");
		SetDialogItemText (theHandle, (ConstStr255Param)namePtr);
		
		}		// end "if (newEditCode == kEditGroup)" 
		
	if (newEditCode == kEditClass)
		{
		SetWTitle (GetDialogWindow(dialogPtr), "\pEdit Thematic Class Name");
		SetDialogItemText (theHandle, (ConstStr255Param)namePtr);
		
		}		// end "if (newEditCode == kEditClass)" 
	
			// Select the default name.												
			
	SelectDialogItemText (dialogPtr, 5, 0, INT16_MAX);
		
			// Center the dialog and then show it.											
			
	ShowDialogWindow (dialogPtr, kGroupNameDialogID, kSetUpDFilterTable);
	
	gDialogItemDescriptorPtr[5] = kDItemString + kDItemSType2;
	
	modalDone = FALSE;
	itemHit = 0;
	do 
		{
	   ModalDialog (gProcessorDialogFilterPtr, &itemHit);
		if (itemHit > 2)
			{
					// Get the handle to the itemHit.										
					
			GetDialogItem (dialogPtr, itemHit, &theType, &theHandle, &theBox);
			
			switch (itemHit)
				{	
				case 5:				// Class or Group Name 
							// Check for the string being too long.						
							
					GetDialogItemText (theHandle, textString);
					if (textString[0] > maxStringLength)
						{
						textString[0] = (char)maxStringLength;
						SetDialogItemText (theHandle, (ConstStr255Param)&textString);
						SelectDialogItemText (dialogPtr, 5, 0, INT16_MAX);
						SysBeep(2);
						
						}		// end "if (textString[0] > maxStringLength)"
					break;
						
				}		// end "switch (itemHit)" 
				
			}		// end "if (itemHit > 2)" 
		else
			if	(itemHit == 1)      // User selected OK for information 
				{
				UInt16	numberClassesGroups;
				
				modalDone = TRUE;
				OKFlag = TRUE;
				
						// Item 5; Class or Group Name													

				GetDialogItem (dialogPtr, 5,  &theType, &theHandle, &theBox);
				GetDialogItemText (theHandle, textString);
				
				numberClassesGroups = CheckForDuplicateName (newEditCode,
														nameHandle,
														gActiveImageFileInfoH,
														textString,
														&classGroupIndex,
														&noChangeFlag);
														
				if (numberClassesGroups == 0)		
					{
					HiliteControl ((ControlHandle)okHandle, 255);
					
					SInt16 classGroupCode = 1; 
					if (newEditCode != kEditClass)
						classGroupCode = 3;
						
					DupClassFieldNameAlert (classGroupCode, textString);
					HiliteControl ((ControlHandle)okHandle, 0);
					SelectDialogItemText (dialogPtr, 5, 0, INT16_MAX);
					modalDone = FALSE;
					OKFlag = FALSE;
					
					}		// end "if (stringSameFlag)" 
				
						// If name is not duplicate, change name in group list.		
				
				if (modalDone)
					{
					EditGroupClassDialogOK (legendListHandle,
														newEditCode,
														nameHandle,
														gActiveImageFileInfoH,
														(char*)&textString,
														selectedCell,
														numberClassesGroups,
														classGroupIndex,
														noChangeFlag);
														
					}		// end "if (modalDone)" 
												
				}		// end "if (itemHit == 1)" 
				
			if	(itemHit == 2)      // User selected Cancel for information 
				{
				modalDone = TRUE;
				OKFlag = FALSE;
				
				}		// end "if	(itemHit == 2)" 
				
		} while (!modalDone);
		
	CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
#endif	// defined multispec_mac


	#if defined multispec_win   
		CMEditClassGroupDlg*		dialogPtr = NULL;
		
		TRY
			{ 
			dialogPtr = new CMEditClassGroupDlg(); 
			
			OKFlag = dialogPtr->DoDialog (
											(CMLegendList*)legendListHandle,
											selectedCell,
											newEditCode, 
											classGroupIndex,
											fileInfoHandle,
											nameHandle); 
		                       
			delete dialogPtr;
			}
			
		CATCH_ALL(e)
			{
			MemoryMessage(0, kObjectMessage);
			OKFlag = FALSE;
			}
		END_CATCH_ALL  
	#endif	// defined multispec_win  

	#if defined multispec_lin   
		CMEditClassGroupDlg* dialogPtr = NULL;


		dialogPtr = new CMEditClassGroupDlg((wxWindow *) gActiveImageViewCPtr->m_frame);

		OKFlag = dialogPtr->DoDialog(
				  (CMLegendList*) legendListHandle,
				  selectedCell,
				  newEditCode,
				  classGroupIndex,
				  fileInfoHandle,
				  nameHandle);

		delete dialogPtr;
	#endif	// defined multispec_lin 
		
	CheckAndUnlockHandle (nameHandle);
	
	return (OKFlag);
	
}		// end "EditGroupClassDialog"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void EditGroupClassDialogOK
//
//	Software purpose:	The purpose of this routine is to check if the input
//							class or group name is a duplicate.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/07/1997
//	Revised By:			Larry L. Biehl			Date: 07/26/2017

void EditGroupClassDialogOK (
				ListHandle							legendListHandle,
				SInt16								newEditCode,
				Handle								nameHandle,
				Handle								fileInfoHandle,
				char*									textStringPtr,
				SInt16								selectedCell,
				UInt16								numberClassesGroups,
				SInt16								classGroupIndex,
				Boolean								noChangeFlag)

{	                          
	char*									namePtr;
	FileInfoPtr 						fileInfoPtr;
	
	SInt16								paletteIndex;
	
		
			// Create a new group if new group name.				
			           
	if (newEditCode == kNewGroup)
		{                   
		Cell				cell;          
		
		SInt16			oldGroup,
							stringLength;
							
		UInt16			classValue = 0;
		
		
				// Get class to group and group to palette pointers.
				
		SInt16* classToGroupPtr = GetClassToGroupPointer (fileInfoHandle);
																
		SInt16* groupToPalettePtr = GetGroupToPalettePointer (fileInfoHandle);
																
		stringLength = 2;

		cell.h = 0;
		cell.v = selectedCell;
            
#		if defined multispec_lin
			classValue = legendListHandle->GetItemData (gSelectedCell);
#		endif 
		
#		if defined multispec_mac
			LGetCell (&classValue, &stringLength, cell, legendListHandle);
#		endif

#		if defined multispec_win  
			((CMLegendList*)legendListHandle)->GetTextValue (gSelectedCell, &classValue);
#		endif

				// Add new group name pointer info.
				
		//SInt16 cellValue = (numberClassesGroups-1) | 0x8000;
		//groupToPalettePtr[numberClassesGroups-1] = classValue;
		
				// Now update the group to palette pointer for the
				// group that the class for the new group came out of.
				// Remove the old group if there are now no classes
				// that belong to it.
				
		oldGroup = classToGroupPtr[classValue];	
		
				// Set the new group for the selected class. This
				// may be beyond the limit of the number of classes.
				// It will be reduced in the call to
				// 'UpdateGroupTables'
				
		classToGroupPtr[classValue] = numberClassesGroups;
		
		UpdateGroupTables (oldGroup, classToGroupPtr, groupToPalettePtr);
		
				// Add new group name pointer info.
				
		fileInfoPtr = (FileInfoPtr)GetHandlePointer (
												fileInfoHandle, kNoLock, kNoMoveHi);
							
		fileInfoPtr->numberGroups++;
		numberClassesGroups = fileInfoPtr->numberGroups;
		groupToPalettePtr[numberClassesGroups-1] = classValue;
		
				// Now add the new cells to the legend
									
		UInt16	numberClasses = (UInt16)fileInfoPtr->numberClasses;

		AddCellsToLegendList (legendListHandle,
										kGroupClassDisplay,
										classToGroupPtr,
										numberClasses,
										numberClassesGroups);
					
		paletteIndex = groupToPalettePtr[numberClassesGroups-1];
										
		UnlockGroupTablesHandle (fileInfoHandle);

				// Update the group palette and color table.
				
		UpdateUserDefinedGroupColorTable (numberClassesGroups-1, paletteIndex, TRUE);
		
		UpdateActiveImageLegend (kGroupClassDisplay, kDoNotCallCreatePalette);
		
		noChangeFlag = FALSE;
		
		classGroupIndex = numberClassesGroups - 1;
		
		}	// end "if (newEditCode == kNewGroup)" 
		
			// Save new group name.					
			
	namePtr = (char*)GetHandlePointer (nameHandle, kNoLock, kNoMoveHi);
	namePtr = &namePtr[classGroupIndex*32];
	BlockMoveData (textStringPtr, namePtr, textStringPtr[0]+1);
		
	if (newEditCode != kNewGroup)
		{                        		
#		if defined multispec_mac   
			Cell				cell; 
			Rect				cellRect;
			GrafPtr			savedPort;
				
			cell.h = 0;
			cell.v = selectedCell;
			LRect (&cellRect, cell, legendListHandle);
			GetPort (&savedPort);
			SetPortWindowPort (gActiveImageWindow);
			InvalWindowRect (gActiveImageWindow, &cellRect);
			SetPort (savedPort);
#		endif	// defined multispec_mac 
		 
#		if defined multispec_win 
			RECT		cellRect;
			legendListHandle->GetItemRect(selectedCell, &cellRect);
			legendListHandle->InvalidateRect(&cellRect, TRUE);
#		endif	// defined multispec_win
		
		}	// end "if (newEditCode != kNewGroup)"	
				
	if (!noChangeFlag)
		{		
		fileInfoPtr = (FileInfoPtr)GetHandlePointer (
												fileInfoHandle, kNoLock, kNoMoveHi);
												
		if (newEditCode == kEditClass)
			fileInfoPtr->classChangedFlag = TRUE;
		
		else		// newEditCode != kNewClass
			fileInfoPtr->groupChangedFlag = TRUE;
			
#		if defined multispec_lin
				// Make sure that wxWidgets knows the document has changed.
			gActiveImageViewCPtr->GetDocument()->Modify(TRUE);
#		endif // defined multispec_lin
																		
		gUpdateFileMenuItemsFlag = TRUE; 
		
		}	// end "if (!noChangeFlag)"
	
}	// end "EditGroupClassDialogOK"



//#if defined multispec_mac 
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void EditGroups
//
//	Software purpose:	The purpose of this routine is to allow the user to edit
//							the class grouping.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 11/06/1996
//	Revised By:			Larry L. Biehl			Date: 03/02/2017
#ifndef multispec_lin
	void EditGroups(
				ListHandle							legendListHandle)
#endif
#ifdef multispec_lin
	void EditGroups(
				ListHandle							legendListHandle, 
				wxMouseEvent&						event)
#endif
{
				// Allow editing groups only if the group-class legend is
				// being displayed.
				
	#if defined multispec_win
		RECT									theBox;
	#endif

	#if defined multispec_mac
		Rect									theBox;
	#endif

	Cell									lastVisibleCell,
											lCell;
					
	SInt16 								*classToGroupPtr,
											*groupToPalettePtr; 
					
	Boolean								continueFlag;
								
	SInt16								classNumber,
											groupNumber,
											newSelectedCell,
											oldGroup,
											paletteIndex,
											selectedCell,
											stringLength;
											
	UInt16								cellValue,	
					                  classMoved;
											
					
	lCell.h = 0;
	lCell.v = 0;

				// Find if class needs to be moved.						
	if (LGetSelect (TRUE, &lCell, legendListHandle))
		{
		selectedCell = lCell.v;
      
		#if defined multispec_lin
				int pflags;
				selectedCell = legendListHandle->HitTest(event.GetPosition(), pflags);
		#endif

		#if defined multispec_win || defined multispec_lin
			gVerticalCellOffset = selectedCell - gSelectedCell;
		#endif 

		if (gVerticalCellOffset != 0)
			{
			LSetDrawingMode (FALSE, legendListHandle);
			
					// Get the last visible cell.  Save the 			
					// rectangle for it in case this cell needs		
					// to be erased.											
			
			lastVisibleCell.h = -1;

			#if defined multispec_win
				lastVisibleCell.v = legendListHandle->GetCount() - 1;

				legendListHandle->GetItemRect(gSelectedCell, &theBox); 
			#endif

			#if defined multispec_mac
				lastVisibleCell.v = (*legendListHandle)->dataBounds.bottom - 1;

				if (lastVisibleCell.v < (*legendListHandle)->visible.bottom)
					{
					lastVisibleCell.h = 0;
					LRect (&theBox, lastVisibleCell, legendListHandle);
				
					}		// end "if (lastVisibleCell.v < ..." 
			#endif

					// Get the value of the cell that is being moved.
					
			stringLength = 2;
         
			#if defined multispec_lin
				//LGetCell(&classMoved, &stringLength, lCell, legendListHandle);
				classMoved = legendListHandle->GetItemData(gSelectedCell);
			#endif
			
			#if defined multispec_mac
				LGetCell (&classMoved, &stringLength, lCell, legendListHandle);
			#endif

			#if defined multispec_win                                       
				((CMLegendList*)legendListHandle)->GetTextValue (gSelectedCell, &classMoved);
				//stringLength = legendListHandle->GetText (gSelectedCell, (LPTSTR)&gTextString);
				//BlockMoveData(&gTextString, &longCellValue, 4);
				//classMoved = (UInt16)longCellValue;
			#endif

					// Get the new location and the value for the cell just
					// before the location that the selected cell is being moved
					// to.	
					
			stringLength = 2;

			#if defined multispec_win || defined multispec_lin
				newSelectedCell = gSelectedCell + gVerticalCellOffset;
			#endif
			
			#if defined multispec_mac
				newSelectedCell = selectedCell + gVerticalCellOffset;
			#endif
			
					// If the new selected cell is above the first cell in the list,
					// then assume that the user is just putting the pointer out of
					// range and does not really want to move a cell.
				
			continueFlag = (newSelectedCell >= 0);
			
			if (continueFlag)
				{
						// Get the new group number for the class that is being moved.
								
				lCell.v = newSelectedCell;
				
				#if defined multispec_mac
					LGetCell (&cellValue, &stringLength, lCell, legendListHandle);
				#endif  
            
				#if defined multispec_lin
					cellValue = legendListHandle->GetItemData (newSelectedCell);
				#endif 
	
				#if defined multispec_win              
					((CMLegendList*)legendListHandle)->GetTextValue (newSelectedCell, &cellValue);
					//stringLength = legendListHandle->GetText (newSelectedCell, (LPTSTR)&gTextString);
					//BlockMoveData(&gTextString, &longCellValue, 4);
					//cellValue = (UInt16)longCellValue;
				#endif
									
				classToGroupPtr = GetClassToGroupPointer (gActiveImageFileInfoH);
				groupToPalettePtr = GetGroupToPalettePointer (gActiveImageFileInfoH);
					
				oldGroup = classToGroupPtr[classMoved];
						
				if (cellValue & 0x8000)
							// This is a group cell. Get the group number.
					groupNumber = (cellValue & 0x7fff);
					
				else		// !(cellValue & 0x8000)
					{
							// This is a class cell. Get the group number from the class to
							// group table.
							
					classNumber = (cellValue & 0x7fff);
					groupNumber = classToGroupPtr[classNumber];
							
					}		// end "else !(cellValue & 0x8000)"
			
						// If the cell is being moved is moved to the same group that it
						// already belongs to,then no change needs to be made.
						
				continueFlag = (oldGroup != groupNumber);
					
				}		// end "if (continueFlag)"
				
			if (continueFlag)
				{
						// Now add a new row starting at the next row. The old row there
						// will be pushed down.
						
				newSelectedCell++;
				lCell.v = LAddRow (1, newSelectedCell, legendListHandle);
				
				#if defined multispec_win
					if (newSelectedCell < gSelectedCell)
						gSelectedCell++;
				#endif
				
				#if defined multispec_mac
					if (newSelectedCell < selectedCell)
						selectedCell++;
				#endif
				
						// Put selected cell class info into the new cell.														
						 
				lCell.v = newSelectedCell;
				#ifndef multispec_lin
					LSetCell ((char*) &classMoved, stringLength, lCell, legendListHandle);
				#endif
				#ifdef multispec_lin
					LSetCell (selectedCell, newSelectedCell, legendListHandle);
				#endif
					
						// Select this new cell.								
						
				LSetSelect (TRUE, lCell, legendListHandle); 
				
						// Remove the previously selected cell.			
						
				#if defined multispec_win || defined multispec_lin
					LDelRow (1, gSelectedCell, legendListHandle);
				#endif
				
				#if defined multispec_mac
					LDelRow (1, selectedCell, legendListHandle);
				#endif
				
				classToGroupPtr[classMoved] = groupNumber;
				
				if (UpdateGroupTables (oldGroup, classToGroupPtr, groupToPalettePtr))
					{
					if (groupNumber > oldGroup)
						groupNumber--;
					
					}		// end "if (UpdateGroupTables (oldGroup, ..."
					
				paletteIndex = groupToPalettePtr[groupNumber];
							
				UnlockGroupTablesHandle (gActiveImageFileInfoH);

						// Update the group palette and color table.
						
				UpdateUserDefinedGroupColorTable (groupNumber, paletteIndex, FALSE);
				
				UpdateActiveImageLegend (kGroupClassDisplay, kDoNotCallCreatePalette); 
				
				#if defined multispec_mac
					InvalWindowRect (gActiveImageWindow, &(*legendListHandle)->rView);
				#endif
					
			}		// end "if (continueFlag)"
				
			else		// !continueFlag
				UnlockGroupTablesHandle (gActiveImageFileInfoH);
			
			LSetDrawingMode (TRUE, legendListHandle);
			
			}		// end "if (gVerticalCellOffset != 0)" 
		
		}		// end "if (LGetSelect (TRUE, ..." 
	
}		// end "EditGroups"
//#endif	// defined multispec_mac 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetGroupStructureMemory
//
//	Software purpose:	The purpose of this routine is to obtain the memory 
//							for the group structures if needed.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/10/1996
//	Revised By:			Larry L. Biehl			Date: 07/01/2011	

Boolean GetGroupStructureMemory (
				UInt32								numberClasses,
				Handle*								groupTablesHandlePtr,
				Handle*								groupNameHandlePtr,
				SInt16**								groupToPalettePtrPtr,
				CharPtr*								groupNamePtrPtr,
				Boolean*								groupChangedFlagPtr,
				Boolean*								groupNameChangedFlagPtr)

{
	UInt32								bytesNeeded,
											localNumberClasses;
	
	Boolean								continueFlag;
	
	
	*groupToPalettePtrPtr = NULL;
	*groupNamePtrPtr = NULL;
	
			// First make certain that we have memory allocated for 			
			// a class to grouping table. Allow for minimum of 256 classes and
			// groups.
			
	localNumberClasses = MAX (256, numberClasses);
		
	bytesNeeded = sizeof(SInt16) * 2 * localNumberClasses;
	*groupToPalettePtrPtr = (SInt16*)CheckHandleSize ( 
								groupTablesHandlePtr, 
								&continueFlag,
								groupChangedFlagPtr, 
								bytesNeeded);
		
			// Now make certain that there is enough memory for the group 	
			// names. Allow for the same number of groups as classes.																		
	
	bytesNeeded = (SInt32)32 * numberClasses;
	if (continueFlag)
		*groupNamePtrPtr = (CharPtr)CheckHandleSize ( 
								groupNameHandlePtr, 
								&continueFlag,
								groupNameChangedFlagPtr,  
								bytesNeeded);
								
	return (continueFlag);
	
}		// end "GetGroupStructureMemory"



Boolean GetGroupStructureMemory (
				FileInfoPtr							fileInfoPtr,
				SInt16**								groupTablePtrPtr,
				CharPtr*								groupNamePtrPtr,
				Boolean*								groupChangedFlagPtr,
				Boolean*								groupNameChangedFlagPtr)

{
			// Get memory for group structure for the input file information
			// structure.		
			
	Boolean continueFlag = GetGroupStructureMemory (
									MAX(fileInfoPtr->numberClasses, fileInfoPtr->numberGroups),
									&fileInfoPtr->groupTablesHandle,
									&fileInfoPtr->groupNameHandle,
									groupTablePtrPtr,
									groupNamePtrPtr,
									groupChangedFlagPtr,
									groupNameChangedFlagPtr);
											
	return (continueFlag);
	
}		// end "GetGroupStructureMemory"       



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetPaletteEntry
//
//	Software purpose:	The purpose of this routine is to load the class names
//							or group names into the the legend list for Thematic 
//							image type windows.
//
//	Parameters in:		Window pointer to the grafport of the image window.
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
//	Called By:			DisplayThematicImage in displayThematic.c
//							UpdateActiveImageLegend in SPalette.c
//							PrintImageWindow in print.c
//
//	Coded By:			Larry L. Biehl			Date: 04/27/2016
//	Revised By:			Larry L. Biehl			Date: 04/30/2016

UInt16 GetPaletteEntry (
				UInt16								paletteIndex)

{    
	UInt16							paletteEntries;


	#if defined multispec_mac
		if (paletteIndex >= gClassPaletteEntries)
			{
			paletteEntries = MIN(gClassPaletteEntries, 256);
			paletteIndex -= paletteEntries;
			paletteIndex = paletteIndex % (paletteEntries-1) + 1;
			
			}		// end "if (paletteIndex >= gClassPaletteEntries)" 
	#endif

	#if defined multispec_win || defined multispec_lin
		if (paletteIndex >= gClassPaletteEntries)
			{                        
			paletteEntries = (UInt16)MIN(gClassPaletteEntries, 254);
			paletteIndex = (paletteIndex - 1) % paletteEntries + 1;
				
			}		// end "if (paletteIndex >= gClassPaletteEntries)" 
	#endif
		
	return paletteIndex;
	
}		// end "GetPaletteEntry"       



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void LoadThematicLegendList
//
//	Software purpose:	The purpose of this routine is to load the class names
//							or group names into the the legend list for Thematic 
//							image type windows.
//
//	Parameters in:		Window pointer to the grafport of the image window.
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
//	Called By:			DisplayThematicImage in displayThematic.c
//							UpdateActiveImageLegend in SPalette.c
//							PrintImageWindow in print.c
//
//	Coded By:			Larry L. Biehl			Date: 12/28/1989
//	Revised By:			Larry L. Biehl			Date: 06/22/2015	

void LoadThematicLegendList (
				ListHandle							legendListHandle, 
				Boolean								printWindowFlag)

{                  
	double								end,
											endBinValue,
											interval,
											logInterval,
											logWholeDigits,
											start,
											startBinValue,
											wholeDigits;
								
	HUCharPtr							dataDisplayPtr;
	DisplaySpecsPtr 					displaySpecsPtr;
	HistogramSummaryPtr				histogramSummaryPtr;
	
	FileInfoPtr 						fileInfoPtr;
	
	Handle								activeImageFileInfoHandle,
											activeImageWindowInfoHandle,
											displaySpecsHandle,
											histogramSummaryHandle;
	
	SInt16*								classToGroupPtr = NULL;
	UInt16*								classSymbolPtr;
	
	SInt32								dataValueIndexOffset,
											handleSize,
											sizeRequired;
	
	UInt32								channelIndex,
											dataValueIndex,
											index,
											maxBin,
											maxIndex,
											minIndex,
											numberClasses;
	
	SignedByte							handleStatus;
	
	SInt16								classGroupCode,
											imageType,
											length,
											width;
											
	UInt16								dataTypeCode,
											eDecimalPlaces,
											fDecimalPlaces;
											
	Boolean								continueFlag = TRUE,
											groupNameChangedFlag,
											groupTableChangedFlag,
											includeLessThanSymbolFlag,
											singleValueFlag,
											singleValue2Flag;
//											invertValuesFlag;
	
	
	activeImageWindowInfoHandle = GetActiveImageWindowInfoHandle ();
	activeImageFileInfoHandle = GetActiveImageFileInfoHandle ();
					
	if (legendListHandle == NULL || activeImageFileInfoHandle == NULL)	
																							return;
		
	fileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (
								activeImageFileInfoHandle, &handleStatus, kNoMoveHi);
		
			// Get a handle to the class or group names and the number of names	
			// to deal with.																		
	
	displaySpecsHandle = GetActiveDisplaySpecsHandle ();           
	displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer(
													displaySpecsHandle, kNoLock, kNoMoveHi);   
													
	if (displaySpecsPtr == NULL)
		{
		MHSetState (activeImageFileInfoHandle, handleStatus);
																							return;
																							
		}
		
	classGroupCode = displaySpecsPtr->classGroupCode;
	numberClasses = fileInfoPtr->numberClasses;
	
	includeLessThanSymbolFlag = TRUE;
	singleValueFlag = FALSE;
	singleValue2Flag = FALSE;
	imageType = GetImageType (activeImageWindowInfoHandle);
	if (imageType == kMultispectralImageType && !fileInfoPtr->classNamesLoadedFlag)
		{
				// Get the information that describes how the data values are being
				// assigned to the class number.
				
		channelIndex = displaySpecsPtr->channelNumber - 1;
	
		dataDisplayPtr = (HUCharPtr)GetHandlePointer (
											gToDisplayLevels.vectorHandle, kLock, kNoMoveHi);
											
		histogramSummaryHandle = GetHistogramSummaryHandle (activeImageWindowInfoHandle);
	
		histogramSummaryPtr = (HistogramSummaryPtr)GetHandlePointer(
											histogramSummaryHandle, kNoLock, kNoMoveHi);
											
		maxBin = histogramSummaryPtr[channelIndex].numberBins - 1;
																						
				// Load default class names for thematic type one-channel display.	
		
		handleSize = MGetHandleSize (fileInfoPtr->classDescriptionH);
		sizeRequired = fileInfoPtr->numberClasses * (sizeof(Str31) + sizeof(SInt16));
		
		if (handleSize < sizeRequired)
			{
			fileInfoPtr->classDescriptionH = 
												UnlockAndDispose (fileInfoPtr->classDescriptionH);	
			fileInfoPtr->classDescriptionH = MNewHandle (sizeRequired);
				
			}		// end "if (handleSize < sizeRequired)"
				
		char* classNamePtr = (char*)GetHandlePointer(
							fileInfoPtr->classDescriptionH, kLock, kNoMoveHi);	
							
		if (dataDisplayPtr != NULL && classNamePtr != NULL)
			{
			classSymbolPtr = (UInt16*)&classNamePtr[ 
													fileInfoPtr->numberClasses*sizeof(Str31)];
											
			start = displaySpecsPtr->thematicTypeMinMaxValues[0];
			start *= displaySpecsPtr->thematicValueFactor;
			
			end = displaySpecsPtr->thematicTypeMinMaxValues[1];
			end *= displaySpecsPtr->thematicValueFactor;
			
			minIndex = displaySpecsPtr->thematicTypeMinMaxIndices[0];
			maxIndex = displaySpecsPtr->thematicTypeMinMaxIndices[1];
			
//			minIndex = GetBinIndexForDataValue (
//						displaySpecsPtr->thematicTypeMinMaxValues[0]+fileInfoPtr->signedValueOffset,
//						&histogramSummaryPtr[channelIndex]);					
//			maxIndex = GetBinIndexForDataValue (
//						displaySpecsPtr->thematicTypeMinMaxValues[1]+fileInfoPtr->signedValueOffset,
//						&histogramSummaryPtr[channelIndex]);
							
			dataValueIndex = minIndex;
			
					// The first time through the loop the offset will be 0.  After that the
					// offset will be -1 so as to use the last data value for which the palette
					// index is the same as the start palette value.
					
			dataValueIndexOffset = 0;
			
					// Get the width of the number to be used in the legend list
					
//			start = GetDataValueForBinIndex (minIndex,
//														&histogramSummaryPtr[channelIndex],
//														fileInfoPtr->signedValueOffset);
//			start *= displaySpecsPtr->thematicValueFactor;
					
//			end = GetDataValueForBinIndex (maxIndex,
//														&histogramSummaryPtr[channelIndex],
//														fileInfoPtr->signedValueOffset);
//			end *= displaySpecsPtr->thematicValueFactor;
			
			wholeDigits = fabs(start);
			logWholeDigits = 1;
			if (wholeDigits >= 1)
				logWholeDigits = log10(wholeDigits) + 1;
			
			width = (SInt16)logWholeDigits;
			
			wholeDigits = fabs(end);
			logWholeDigits = 1;
			if (wholeDigits >= 1)
				logWholeDigits = log10(wholeDigits) + 1;
			
			width = MAX(width, (SInt16)logWholeDigits);
			if (start < 0)
				width++;
				
					// Get the smallest interval between classes
					
			if (displaySpecsPtr->enhancementCode == kLinearStretch)
				{						
				interval = (end - start);
				if (numberClasses >= 2)
					interval /= (numberClasses-2);
					
				if (fileInfoPtr->dataTypeCode == kIntegerType)
					{
					if (numberClasses-2 == end - start + 1)
						{
						interval = 1;
						singleValueFlag = TRUE;
						
						}		// end "if (numberClasses-2 == end - start + 1)"
						
					else if (numberClasses-2 == end - start)
						{
						interval = 1;
						singleValue2Flag = TRUE;
						
						}		// end "if (numberClasses-2 == end - start)"
						
					}		// end "if (fileInfoPtr->dataTypeCode == kIntegerType)"
				
				}		// end "if (displaySpecsPtr->enhancementCode == kLinearStretch)"
				
			else		// ...->enhancementCode != kLinearStretch
				{
						// Use bin index value of 1 which represents the minNonSatValue.
						// The data value per histogram bin vector is linear starting
						// at this value.  It may not be starting from bin 0 which 
						// represenents the minValue.
						 
				startBinValue = GetDataValueForBinIndex (
											1,
											&histogramSummaryPtr[channelIndex],
											fileInfoPtr->signedValueOffset);
											
				interval = GetDataValueForBinIndex (
											1 + displaySpecsPtr->smallestIndexInterval,
											&histogramSummaryPtr[channelIndex],
											fileInfoPtr->signedValueOffset);
											
				interval -= startBinValue;
				interval *= displaySpecsPtr->thematicValueFactor;
				
				}		// end "else ...->enhancementCode != kLinearStretch"
				
			if (interval > 10)
				includeLessThanSymbolFlag = FALSE;
			
					// Get the default number of decimal places to use.
					// The result of above will be 1 more than what is needed.
			
			dataTypeCode = kRealType;
			if (interval == 1)
				dataTypeCode = kIntegerType;
			GetNumberDecimalDigits (dataTypeCode,
												start,
												end,
												interval,
												&eDecimalPlaces,
												&fDecimalPlaces);
							
			if (fDecimalPlaces > 0 && fDecimalPlaces < 5)
				{
				if (interval < 1)
					{
					logInterval = floor (log10 (interval));
					if (fDecimalPlaces > fabs(logInterval))					
						fDecimalPlaces--;
					
					}		// end "if (interval < 1)"
					
				else		// interval >= 1	
					{
					if (fDecimalPlaces > 1 || interval > 10)
						fDecimalPlaces--;
						
					}		// end "else interval >= 1"
				
				}		// end "if (fDecimalPlaces > 0 && fDecimalPlaces < 5)"
			
			if (fDecimalPlaces > 0)											
				width += fDecimalPlaces + 1;
			
			startBinValue = start;
			for (index=1; index<=numberClasses; index++)
				{
				if (index > 1)
					{
					if (displaySpecsPtr->enhancementCode != kLinearStretch)
						{
						while (dataDisplayPtr[dataValueIndex] == index-1 &&
									dataValueIndex >= minIndex && 
											dataValueIndex <= maxIndex &&
													index < numberClasses)
							dataValueIndex++;
						
						}		// end "if (...->enhancementCode != kLinearStretch)"
						
					}		// end "if (index > 1)"
				
				if (index == 1)
					{
					if (numberClasses >= 2)
						length = sprintf (&classNamePtr[1], 
													"< %*.*lf",
													width,
													fDecimalPlaces,
													startBinValue);
													
					else		// numberClasses == 1
						length = sprintf (&classNamePtr[1], 
													"all values");
												
					endBinValue = startBinValue;
						
					}		// end "if (index == 1)"
												
				else if (index != numberClasses)
					{
					if (index < numberClasses-1)
						{
						if (displaySpecsPtr->enhancementCode == kLinearStretch)
							endBinValue = startBinValue + interval;
							
						else		// displaySpecsPtr->enhancementCode != kLinearStretch
							{
							endBinValue = GetDataValueForBinIndex (dataValueIndex+dataValueIndexOffset,
																					&histogramSummaryPtr[channelIndex],
																					fileInfoPtr->signedValueOffset);
							endBinValue *= displaySpecsPtr->thematicValueFactor;
								
							}		// end "else ...->enhancementCode != kLinearStretch"
							
						}		// end "if (index < numberClasses-1)"
						
					else		// index == numberClasses-1
						endBinValue = end;
				
					if (singleValueFlag || singleValue2Flag)
						length = sprintf (&classNamePtr[1], 
													"%*.*lf",
													width,
													fDecimalPlaces, 
													startBinValue);
												
					else		// !singleValueFlag
						{
						if (includeLessThanSymbolFlag)
							length = sprintf (&classNamePtr[1], 
														"%*.*lf - < %*.*lf",
														width,
														fDecimalPlaces, 
														startBinValue,
														width,
														fDecimalPlaces, 
														endBinValue);
														
						else		// !includeLessThanSymbolFlag
							length = sprintf (&classNamePtr[1], 
														"%*.*lf - %*.*lf",
														width,
														fDecimalPlaces, 
														startBinValue,
														width,
														fDecimalPlaces, 
														endBinValue);
														
						}		// end "else !singleValueFlag"
					
					}		// end "else if (index != numberClasses)"
					
				else		// index == numberClasses
					{
					if (singleValueFlag)
						length = sprintf (&classNamePtr[1], 
													"> %*.*lf",
													width,
													fDecimalPlaces, 
													endBinValue);
	
					else		// !singleValueFlag
						length = sprintf (&classNamePtr[1], 
													">= %*.*lf",
													width,
													fDecimalPlaces, 
													endBinValue);
													
					}		// end "else index == numberClasses"
												
				classNamePtr[0] = MIN(length,31);
				
				startBinValue = endBinValue;
							
				dataValueIndexOffset = -1;
				
				classNamePtr += 32;
				
				}		// end "for (index=1; index<numberClasses; index++)"
				
					// Now also need to be sure there is a correct class symbol vector.
					
			for (index=0; index<numberClasses; index++)
				classSymbolPtr[index] = (UInt16)index;
				
			fileInfoPtr->classNamesLoadedFlag = TRUE;
			
			}		// end "if (dataDisplayPtr != NULL && classNamePtr != NULL)"
					
		}		// end "if (imageType == kMultispectralImageType)"
	
	if (classGroupCode == kGroupClassDisplay)
		{
				// Verify that we have the group table information.
				
		SInt16*					groupTablePtr;
		char*						groupNamePtr;
		
				// First make certain that we have memory allocated for 					
				// a class to grouping table.
				
		continueFlag = GetGroupStructureMemory (
												fileInfoPtr,
												&groupTablePtr,
												&groupNamePtr,
												&groupTableChangedFlag,
												&groupNameChangedFlag);	
		
		if (continueFlag)
			{										
			classToGroupPtr = GetClassToGroupPointer (fileInfoPtr);
			
//			if (groupTableChangedFlag || groupNameChangedFlag)
			if (fileInfoPtr->numberGroups == 0)
				{							
						// Load default list.														
						
				for (index=0; index<numberClasses; index++)
					{
					groupTablePtr[index] = (SInt16)index;	
					classToGroupPtr[index] = (SInt16)index;
					
					}		// end "for (index=0; index<numberClasses; index++)"
																						
						// Load default group names.	
						
				char* classNamePtr = (char*)GetHandlePointer(
									fileInfoPtr->classDescriptionH, kLock, kNoMoveHi);		
				
				UInt32 bytesNeeded = (SInt32)32 * numberClasses;
				BlockMoveData (classNamePtr, groupNamePtr, bytesNeeded);
				CheckAndUnlockHandle (fileInfoPtr->classDescriptionH);
					
				fileInfoPtr->numberGroups = (UInt16)numberClasses;
				
				fileInfoPtr->groupChangedFlag = TRUE; 
				displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer(
													displaySpecsHandle, kNoLock, kNoMoveHi);
				displaySpecsPtr->classToGroupChangeFlag = TRUE;
			
				#if defined multispec_lin
						// Make sure that wxWidgets knows the document has changed.
					gActiveImageViewCPtr->GetDocument()->Modify(TRUE);
				#endif // defined multispec_lin
				
				}		// end "if (fileInfoPtr->numberGroups == 0)"
				
			}		// end "if (continueFlag)"
			
		CheckAndUnlockHandle (fileInfoPtr->groupNameHandle);
		
		}		// end "if (classGroupCode == kGroupClassDisplay)"
		
			// Now load the legend list table.
		
	if (continueFlag)
		AddCellsToLegendList (legendListHandle,
									classGroupCode,
									classToGroupPtr,
									(UInt16)fileInfoPtr->numberClasses,
									(UInt16)fileInfoPtr->numberGroups);
									
	CheckAndUnlockHandle (fileInfoPtr->groupTablesHandle);
		
	MHSetState (activeImageFileInfoHandle, handleStatus);
		                  
	#if defined multispec_mac   
		if (!printWindowFlag && ((WindowInfoPtr)*gActiveImageWindowInfoH)->showLegend)
			{ 
					// Invalidate the legend to force the list to be drawn.						
		
			GetWindowPortBounds (gActiveImageWindow, &gTempRect);
			gViewRect.left = 0;
			gViewRect.top = 0;
			gViewRect.bottom = gTempRect.bottom - kSBarWidth;
			gViewRect.right = gActiveLegendWidth;
			InvalWindowRect (gActiveImageWindow, &gViewRect);
			
			}		// end "if (!printWindowFlag && ((WindowInfoPtr)*gActiveImageWindowInfoH)->..."
				
				// Set handle that will be used to read group or class names from
				// and the class to group and group to class tables. 
			
		(*legendListHandle)->userHandle = activeImageFileInfoHandle;
		
		(*legendListHandle)->refCon &= 0x00000001;
		(*legendListHandle)->refCon += 2*classGroupCode;
	#endif	// defined multispec_mac 

	#if defined multispec_win 
		displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer(
													displaySpecsHandle, kNoLock, kNoMoveHi);
		((CMLegendList*)legendListHandle)->m_paletteObject = 
											(CPalette*)displaySpecsPtr->paletteObject;
		((CMLegendList*)legendListHandle)->m_backgroundPaletteObject = 
											(CPalette*)displaySpecsPtr->backgroundPaletteObject;
		((CMLegendList*)legendListHandle)->m_imageFileInfoHandle = 
																	activeImageFileInfoHandle;
		((CMLegendList*)legendListHandle)->m_listType = classGroupCode; 
		((CMLegendList*)legendListHandle)->m_paletteOffset = 
																	displaySpecsPtr->paletteOffset;           
		((CMLegendList*)legendListHandle)->m_classPaletteEntries = 
																	displaySpecsPtr->numPaletteEntriesUsed;
		gClassPaletteEntries = (SInt16)((CMLegendList*)legendListHandle)->m_classPaletteEntries;
	#endif	// defined multispec_win 

	#if defined multispec_lin 
		displaySpecsPtr = (DisplaySpecsPtr) GetHandlePointer(
				  displaySpecsHandle, kNoLock, kNoMoveHi);
		((CMLegendList*) legendListHandle)->m_paletteObject = (wxPalette*) displaySpecsPtr->paletteObject;
		((CMLegendList*) legendListHandle)->m_backgroundPaletteObject = (wxPalette*) displaySpecsPtr->backgroundPaletteObject;
		((CMLegendList*) legendListHandle)->m_imageFileInfoHandle = activeImageFileInfoHandle;
		((CMLegendList*) legendListHandle)->m_listType = classGroupCode;
		((CMLegendList*) legendListHandle)->m_paletteOffset = displaySpecsPtr->paletteOffset;
		((CMLegendList*) legendListHandle)->m_classPaletteEntries = displaySpecsPtr->numPaletteEntriesUsed;
		((CMLegendList*) legendListHandle)->listready = true;
	#endif	// defined multispec_lin 

}		// end "LoadThematicLegendList" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean SelectColor
//
//	Software purpose:	The purpose of this routine is to allow the user to select a
//							new color.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			EditClassGroupPalette in SThemWin.cpp
//
//	Coded By:			Junaida Halan			Date: 07/10/1998
//	Revised By:			Larry L. Biehl			Date: 07/09/2015	

Boolean SelectColor (
				SInt16								alertMessageNumber,
				RGBColor*							oldRGBPtr,
				RGBColor*							newRGBPtr)

{	
	Boolean								colorChangedFlag = FALSE;
	

		
			// Make certain that there is enough memory left for loading	
			// the color picker.															
		
	if (CheckMemoryForColorPicker (&gTextString, alertMessageNumber))
		{
				// Handle a change in color.
															
		#if defined multispec_mac											
			Point									wherePoint;

			wherePoint.h = 0;
			wherePoint.v = 0;
			if (gAppleEventsFlag)
				{
				wherePoint.h = -1;
				wherePoint.v = -1;
				
				}		// end "if (gAppleEventsFlag)" 

			colorChangedFlag = GetColor (wherePoint, 
								 	 				  gTextString, 
													  oldRGBPtr, 
													  newRGBPtr);
		#endif

		#if defined multispec_win                 
			SInt16			returnCode;
			
			TRY
				{                      
            CMColorDialog colorDialog(0,0,NULL);
            
				colorDialog.m_cc.lStructSize = sizeof (CHOOSECOLOR);
				colorDialog.m_cc.Flags = CC_FULLOPEN | CC_RGBINIT | CC_ENABLEHOOK; 
				colorDialog.m_cc.rgbResult = RGB((BYTE)(oldRGBPtr->red>>8),
													(BYTE)(oldRGBPtr->green>>8),
													(BYTE)(oldRGBPtr->blue>>8)); 
                                       
				returnCode = colorDialog.DoModal();

				if (returnCode == IDOK)
					{
					newRGBPtr->red = (UInt16)(GetRValue(colorDialog.m_cc.rgbResult)<<8);
					newRGBPtr->green = (UInt16)(GetGValue(colorDialog.m_cc.rgbResult)<<8);
					newRGBPtr->blue = (UInt16)(GetBValue(colorDialog.m_cc.rgbResult)<<8);
				
					colorChangedFlag = TRUE;
				
					}		// end "if (returnCode == IDOK)" 
				
				}
			
			CATCH_ALL(e)
				{
//				MemoryMesage(0, kObjectMessage);
				}
			END_CATCH_ALL
		#endif
      
		#if defined multispec_lin
			wxColourData cdata;
			cdata.SetChooseFull(true);
			//wxColour defcolor((BYTE)(oldRGBPtr->red >> 8), (BYTE)(oldRGBPtr->green >> 8), (BYTE)(oldRGBPtr->blue >> 8));
         wxColour defcolor((BYTE)(oldRGBPtr->red), (BYTE)(oldRGBPtr->green), (BYTE)(oldRGBPtr->blue ));
			cdata.SetColour(defcolor);
			wxColourDialog colorDialog((wxWindow *) gActiveImageViewCPtr->m_frame, &cdata);
			if(colorDialog.ShowModal() == wxID_OK)
				{
				wxColourData retData = colorDialog.GetColourData();
				wxColour retcolor = retData.GetColour();
				//newRGBPtr->red = (UInt16)(retcolor.Red()<<8);
				//newRGBPtr->green = (UInt16)(retcolor.Green()<<8);
				//newRGBPtr->blue = (UInt16)(retcolor.Blue()<<8);
						// Note that for linux version, the colors are being kept as 8-bit values.
				newRGBPtr->red = (UInt16)retcolor.Red();
				newRGBPtr->green = (UInt16)retcolor.Green();
				newRGBPtr->blue = (UInt16)retcolor.Blue();
				colorChangedFlag = TRUE;
				}
		#endif

		}		// end "if (CheckMemoryForColorPicker (&gTextString, 4))"

	return (colorChangedFlag);

}		// end "SelectColor" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateUserDefinedGroupColorTable
//
//	Software purpose:	The purpose of this routine is to remove empty items from the
//							group color table.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			EditGroupClassDialogOK in SThemWin.cpp
//							EditGroups in SThemWin.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/12/1999
//	Revised By:			Larry L. Biehl			Date: 06/08/2011	

void UpdateUserDefinedGroupColorTable (
				SInt16								groupNumber,
				SInt16								paletteIndex,
				Boolean								newGroupFlag)

{	
	RGBColor								groupRGB;
	
	ColorSpec*							colorSpecPtr;
	
	CTabHandle							classColorTableHandle,
											groupColorTableHandle;
											
	CMPaletteInfo						paletteHandle;
	
	SInt16								thematicPaletteType;
		
		
			// Check if this routine needs to be used. It is only needed for
			// the user defined palette case.
			// It may be best to always call this routine after a change in 
			// the palette rather than re-reading palette from file or resource.
																						
	Handle displaySpecsH = GetActiveDisplaySpecsHandle ();
	DisplaySpecsPtr displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
												displaySpecsH, kNoLock, kNoMoveHi);
	thematicPaletteType = displaySpecsPtr->thematicGroupPaletteType;
	
	if (thematicPaletteType != kUserDefinedColors)
																									return;
																								
			// Get the class and group color tables.
																						
	classColorTableHandle = displaySpecsPtr->savedClassCTableHandle;		
	groupColorTableHandle = displaySpecsPtr->savedGroupCTableHandle;
			
			
			// Get the current palette for the image window.
		
	paletteHandle = GetActivePaletteHandle();
	if (paletteHandle == NULL)
																									return;
	
			// Now get the index into the palette that is the color for the
			// input group.
						
	paletteIndex = GetPaletteEntry (paletteIndex);
	/*
	if (paletteIndex >= gClassPaletteEntries)
		{
		UInt16 paletteEntries = MIN(gClassPaletteEntries, 256);
		paletteIndex -= paletteEntries;
		paletteIndex = paletteIndex % (paletteEntries-1) + 1;
		
		}		// end "if (paletteIndex >= gClassPaletteEntries)"
	*/		
			// Get the color for the modified group and make sure that this color is used 
			// for all of the classes in the group.
	
	if (newGroupFlag)
		{
		CTabPtr colorTablePtr = (CTabPtr)GetHandlePointer (
										(Handle)classColorTableHandle, kNoLock, kNoMoveHi); 
		colorSpecPtr = colorTablePtr->ctTable;
		groupRGB = colorSpecPtr[paletteIndex].rgb;
		
		}		// end "if (newGroupFlag)"
		
	else		// !newGroupFlag
		GetEntryColor (paletteHandle, paletteIndex, &groupRGB); 
		
	ChangeGroupPalette (paletteHandle, 
								groupColorTableHandle,
								(UInt16)groupNumber, 
								paletteIndex, 
								&groupRGB,
								TRUE,
								0);
								
//	#if defined multispec_mac						
//		UpdatePaletteWindow (TRUE, FALSE);
//	#endif

//	#if defined multispec_win
//		displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer(displaySpecsH,
//																				kNoLock,
//																				kNoMoveHi);
//		displaySpecsPtr->paletteUpToDateFlag = FALSE;
//		UpdateActiveImageLegend (listType, kDoNotCallCreatePalette);
//	#endif

	ActivateImageWindowPalette (paletteHandle);
	
	return;

}		// end "UpdateUserDefinedGroupColorTable" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateGroupTables
//
//	Software purpose:	The purpose of this routine is to remove empty groups from
//							the group tables.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			EditGroupClassDialogOK in SThemWin.cpp
//							EditGroups in SThemWin.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/06/1996
//	Revised By:			Larry L. Biehl			Date: 06/22/2015	

Boolean UpdateGroupTables (
				SInt16								oldGroup,
				SInt16*								classToGroupPtr,
				SInt16*								groupToPalettePtr)

{	
	DisplaySpecsPtr					displaySpecsPtr;
	
	FileInfoPtr							fileInfoPtr;
	
	Handle								activeImageFileInfoHandle,
											displaySpecsH;
		
	UInt32								numberClasses,
											numberGroups;
											
	Boolean								groupRemovedFlag;
						
	
	activeImageFileInfoHandle = GetActiveImageFileInfoHandle ();
	groupRemovedFlag = FALSE;
	
			// Now update the group to palette pointer for the
			// group that the class for the new group came out of.
			// If there is no class that belongs to it, indicated by
			// the -1 still being in the groupToPalettePtr vector at that
			// position, then remove the group.
	                         
	fileInfoPtr = (FileInfoPtr)GetHandlePointer (
									activeImageFileInfoHandle, kNoLock, kNoMoveHi);
	
	numberClasses = fileInfoPtr->numberClasses;
	numberGroups = fileInfoPtr->numberGroups;

	groupRemovedFlag = UpdateGroupTables (oldGroup,
														classToGroupPtr,
														groupToPalettePtr,
														(SInt16)numberClasses,
														&numberGroups,
														fileInfoPtr->groupNameHandle);
/*	
	groupToPalettePtr[oldGroup] = -1;
	for (classIndex=0; classIndex<numberClasses; classIndex++)
		{
		if (classToGroupPtr[classIndex] == oldGroup)
			{
			groupToPalettePtr[oldGroup] = (UInt16)classIndex;
			break;
			
			}		// end "if (classToGroupPtr[classIndex] == ..." 
		
		}		// end "for (classIndex=0; classIndex<..."
		
	if (groupToPalettePtr[oldGroup] == -1)
		{
		groupRemovedFlag = TRUE;
	
				// Update group numbers in class to group vector taking
				// into account the group that is being deleted.
				
		for (classIndex=0; classIndex<numberClasses; classIndex++)
			{
			if (classToGroupPtr[classIndex] >= oldGroup)
				classToGroupPtr[classIndex]--;
			
			}		// end "for (classIndex=0; classIndex<..."
		
				// Now update the group to palette vector and the group
				// names taking into account the group that is being deleted.

		nameHandle = fileInfoPtr->groupNameHandle;
		namePtr = (char*)GetHandlePointer (nameHandle, kNoLock, kNoMoveHi);
		namePtr = &namePtr[oldGroup*32];
		oldNamePtr = &namePtr[32];

		numberGroups--;
		for (groupIndex=oldGroup; groupIndex<numberGroups; groupIndex++)
			{
			groupToPalettePtr[groupIndex] = groupToPalettePtr[groupIndex+1];
			
			BlockMoveData (oldNamePtr, namePtr, oldNamePtr[0]+1); 
			
			oldNamePtr += 32;
			namePtr += 32;
			
			}		// end "for (groupIndex=oldGroup; groupIndex<..."
							
		}		// end "if (groupToPalettePtr[oldGroup] == -1)"
*/																
	fileInfoPtr->numberGroups = (UInt16)numberGroups;
	fileInfoPtr->groupChangedFlag = TRUE;
			
	#if defined multispec_lin
			// Make sure that wxWidgets knows the document has changed.
		gActiveImageViewCPtr->GetDocument()->Modify(TRUE);
	#endif // defined multispec_lin
						
			// Update some display specification structures and update
			// the palette.
	
	displaySpecsH = GetActiveDisplaySpecsHandle ();
	displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
												displaySpecsH, kNoLock, kNoMoveHi);
	displaySpecsPtr->paletteUpToDateFlag = FALSE;
	displaySpecsPtr->classToGroupChangeFlag = TRUE;
	
	return (groupRemovedFlag);

}		// end "UpdateGroupTables" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateGroupTables
//
//	Software purpose:	The purpose of this routine is to remove empty groups from
//							the group tables.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			UpdateGroupTables
//
//	Coded By:			Larry L. Biehl			Date: 05/11/2011
//	Revised By:			Larry L. Biehl			Date: 06/03/2011	

Boolean UpdateGroupTables (
				SInt16								oldGroup,
				SInt16*								classToGroupPtr,
				SInt16*								groupToPalettePtr,
				SInt16								numberClasses,
				UInt32*								numberGroupsPtr,
				Handle								groupNameHandle)

{		
	char									*namePtr,
											*oldNamePtr;
		
	UInt32								classIndex,
											groupIndex;
											
	Boolean					//			removeGroupFlag = TRUE,
											groupRemovedFlag = FALSE;
	
	
	if (numberClasses <= 0 || *numberGroupsPtr == 0)
																			return (groupRemovedFlag);
	
	groupToPalettePtr[oldGroup] = -1;
	for (classIndex=0; classIndex<(UInt32)numberClasses; classIndex++)
		{
		if (classToGroupPtr[classIndex] == oldGroup)
			{
			groupToPalettePtr[oldGroup] = (SInt16)classIndex;
			break;
			
			}		// end "if (classToGroupPtr[classIndex] == ..." 
		
		}		// end "for (classIndex=0; classIndex<..."
		
	if (groupToPalettePtr[oldGroup] == -1)
		{
		groupRemovedFlag = TRUE;
	
				// Update group numbers in class to group vector taking
				// into account the group that is being deleted.
				
		for (classIndex=0; classIndex<(UInt32)numberClasses; classIndex++)
			{
			if (classToGroupPtr[classIndex] >= oldGroup)
				classToGroupPtr[classIndex]--;
			
			}		// end "for (classIndex=0; classIndex<..."
		
				// Now update the group to palette vector and the group
				// names taking into account the group that is being deleted.

		namePtr = (char*)GetHandlePointer (groupNameHandle, kNoLock, kNoMoveHi);
		namePtr = &namePtr[oldGroup*32];
		oldNamePtr = &namePtr[32];

		(*numberGroupsPtr)--;
		for (groupIndex=oldGroup; groupIndex<*numberGroupsPtr; groupIndex++)
			{
			groupToPalettePtr[groupIndex] = groupToPalettePtr[groupIndex+1];
			
			BlockMoveData (oldNamePtr, namePtr, oldNamePtr[0]+1); 
			
			oldNamePtr += 32;
			namePtr += 32;
			
			}		// end "for (groupIndex=oldGroup; groupIndex<..."
							
		}		// end "if (groupToPalettePtr[oldGroup] == -1)"
		
	return (groupRemovedFlag);

}		// end "UpdateGroupTables" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateThematicLegendControls
//
//	Software purpose:	This routine controls updating the thematic image legend
//							controls.  The routine is called after any change is made 
//							in the size or content of the thematic legend.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None	
// 
// Called By:			ImageWControlEvent in controls.c
//							UpdateScrolls in controls.c
//							AddCellsToList in thematicWindow.c
//							ThematicLegendWMouseDn in thematicWindow.c
//
//	Coded By:			Larry L. Biehl			Date: 11/07/1996
//	Revised By:			Larry L. Biehl			Date: 11/07/1996		

void UpdateThematicLegendControls (
				WindowPtr							theWindow)

{	                  
#if defined multispec_mac 
	ControlHandle						controlHandle;
	Handle								displaySpecsHandle;
	ListHandle							legendListHandle;
	
	
	if (gActiveLegendWidth == 0)
																					return;
																						
	legendListHandle = 
				((WindowInfoPtr)*gActiveImageWindowInfoH)->legendListHandle;
				
	displaySpecsHandle = 
				((WindowInfoPtr)*gActiveImageWindowInfoH)->displaySpecsHandle;
																							
			// Save the current clip region.													
			
	GetClip (gTempRegion1);				
			
			// Set clip region of the window to include the scroll bars so			
			// that the controls can be changed.											
				
	ClipRect (GetWindowPortBounds(theWindow, &gTempRect));
	
			// Update thematic legend scroll controls		
				
			//kLegendScrollUpControl
	
	controlHandle = ((WindowInfoPtr)*gActiveImageWindowInfoH)->legendScrollUpControl;
	if ((*legendListHandle)->visible.top > 
									(*legendListHandle)->dataBounds.top)
		HiliteControl (controlHandle, 0);
		
	else		// ...->visible.bottom >= ...->dataBounds.bottom
		HiliteControl (controlHandle, 255);
				
			// LegendScrollDownControl
	
	controlHandle = ((WindowInfoPtr)*gActiveImageWindowInfoH)->legendScrollDownControl;
	if ((*legendListHandle)->visible.bottom < 
									(*legendListHandle)->dataBounds.bottom)
		HiliteControl (controlHandle, 0);
		
	else		// ...->visible.bottom >= ...->dataBounds.bottom
		HiliteControl (controlHandle, 255);
				
			// LegendPaletteControl
			
	controlHandle = ((WindowInfoPtr)*gActiveImageWindowInfoH)->legendPaletteControl;
	if (displaySpecsHandle != NULL &&
				((DisplaySpecsPtr)*displaySpecsHandle)->imageDimensions[kVertical] > 0)
		HiliteControl (controlHandle, 0);
	
	else		// ...->imageDimensions[] == 0
		HiliteControl (controlHandle, 255);
																							
			// Reset the clip region back to that upon entry to this routine.		
			
	SetClip (gTempRegion1);
	SetEmptyRgn (gTempRegion1);
#endif	// defined multispec_mac 
		                  
#if defined multispec_win   
	if (theWindow != NULL)
		{
		CMImageDoc* imageDocCPtr = theWindow->GetDocument();
		CMImageFrame* imageFrameCPtr = imageDocCPtr->GetImageFrameCPtr();
		CMLegendView* legendViewCPtr = imageFrameCPtr->GetLegendViewCPtr();
	
		legendViewCPtr->UpdateThematicLegendControls();

		}		// end "if (theWindow != NULL)"
#endif	// defined multispec_win 	       	       
   
#if defined multispec_lin   
   if (theWindow != NULL) 
		{
      CMImageDoc* imageDocCPtr = theWindow->GetDocument();
      CMImageFrame* imageFrameCPtr = imageDocCPtr->GetImageFrameCPtr();
      CMLegendView* legendViewCPtr = imageFrameCPtr->GetLegendViewCPtr();

      legendViewCPtr->UpdateThematicLegendControls();

		} // end "if (theWindow != NULL)"
#endif	// defined multispec_lin          
	
}		// end "UpdateThematicLegendControls" 

