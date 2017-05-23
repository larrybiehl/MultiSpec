//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SMenus.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision number:		3.0
//
//	Revision date:			03/25/2017
//
//	Language:				C
//
//	System:					Macintosh Operating System
//
//	Brief description:	 
//
//	Functions in file:	Boolean 				CharactersSelected
//								
//
//	Include files:			"MultiSpecHeaders"
//								"SMulSpec.h"
//
/*
// Template for debugging.
int numberChars = sprintf ((char*)&gTextString3,
							" SMenus: (gauge_range, gStatusGraphicsRight): %d, %f%s",
							gauge_range,
							gStatusGraphicsRight,
							gEndOfLine);
ListString ((char*)&gTextString3, numberChars, gOutputTextH);
 */
//-----------------------------------------------------------------------------

#include "SMulSpec.h"
	
#if defined multispec_mac  

	class  CCmdUI;

//	#include 	"WASTE.h"
	
#endif	// defined multispec_mac   
                             
#if defined multispec_win
	#include "WFormDlg.h" 
#endif	// defined multispec_win   
                             
#if defined multispec_lin
	#include <wx/menu.h>    
   #include "ldialog.h"
#endif	// defined multispec_lin 
	 
#include	"SExtGlob.h" 

	 
#if defined multispec_win || defined multispec_lin
	#pragma pack(4)
#endif	// defined multispec_win || defined multispec_lin	 
	
typedef struct ArcViewRecordHeader
	{
	UInt32						recordNumber;
	UInt32						recordLength;
	UInt32						shapeType;
	
	} ArcViewRecordHeader, *ArcViewRecordHeaderPtr;
	 
#if defined multispec_win || defined multispec_lin
	#pragma pack()
#endif	// defined multispec_win || defined multispec_lin
										
										

			// Prototypes for file routines that are only called from other 		
			// routines in this file.					

Boolean 						CharactersSelected (void);

class CMGraphView;

//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CharactersSelected
//
//	Software purpose:	The purpose of this routine is to determine if any
//							characters are selected in the output text window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	True if characters have been selected in the
//							output text window.  False if no selection
//							has been made.
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/25/1991
//	Revised By:			Larry L. Biehl			Date: 12/18/2008	

Boolean CharactersSelected (void)

{  
	Boolean		returnFlag = FALSE; 
	
	  
	#if defined multispec_mac 		  
				// Determine if text is selected.  If so change the menu items			
				// for 'Save Output Text' and 'Print Text' to indicate that				
				// Selected text will be used.													
		 
		#if use_mlte_for_text_window
			returnFlag = !TXNIsSelectionEmpty (gOutputTextH); 
		#endif
		#if !use_mlte_for_text_window
			SInt32			positionEnd,
								positionStart;
			WEGetSelection (&positionStart, &positionEnd, gOutputTextH);
			returnFlag = (positionStart < positionEnd);
		#endif	
		
	#endif	// defined multispec_mac 
	
	return (returnFlag);
	
}		// end "CharactersSelected"



//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ClearMenuItems
//
//	Software purpose:	The purpose of this routine is to delete the menu items for
//							the input menu begin at the requested menu position.
//
//	Parameters in:		Menu Handle (Mac) or CMenu pointer (Windows)
//							Last menu item to be kept (first start at 1)
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			OverlayControlEvent in controls.c
//							SetUpVectorPopUpMenu in SGraUtil.cpp
//							SetUpClearOverlaysSubMenu in SMenus.cpp
//							SetUpImageOverlayPopUpMenu in SMenus.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/09/2001
//	Revised By:			Larry L. Biehl			Date: 03/25/2015	

void ClearMenuItems (
				MenuHandle							menuHandle,
				SInt32								lastMenuItemToKeep)

{ 
	#if defined multispec_lin 
		wxMenu*								wxMenuPtr;
	#endif	// defined multispec_win
	
	#if defined multispec_win 
		CMenu*								cMenuPtr;
	#endif	// defined multispec_win 
		
		
	SInt32								numberMenuItems;
	
	
	#if defined multispec_mac 
		numberMenuItems = CountMenuItems	(menuHandle);						
		while (numberMenuItems > lastMenuItemToKeep)
			{			
			DeleteMenuItem (menuHandle, numberMenuItems);
			numberMenuItems--;
			
			}		// end "while (numberMenuItems > lastMenuItemToKeep)"
	#endif	// defined multispec_mac 
		
	#if defined multispec_lin 
		wxMenuPtr = (wxMenu*)menuHandle;
		
				// Allow for linux case where the menu position is 0 based,
				// not 1 based.
				
		lastMenuItemToKeep--;
				
		numberMenuItems = wxMenuPtr->GetMenuItemCount() - 1;
		
				// Get the menu item for the last menu in the list.
				
		wxMenuItem*		wx_menuItem;
		int				menuItemID;	
		while (numberMenuItems > lastMenuItemToKeep)	
			{		
			wx_menuItem = wxMenuPtr->FindItemByPosition (numberMenuItems);
			if (wx_menuItem != NULL)
				{
				int menuItemID = wx_menuItem->GetId();
				wxMenuPtr->Delete (menuItemID);
				
				}		// end "if (wx_menuItem != NULL)"
			numberMenuItems--;
			
			}		// end "while (menuItem > lastMenuItemToKeep)"
	#endif	// defined multispec_lin 
		
	#if defined multispec_win 
		cMenuPtr = (CMenu*)menuHandle;
		
				// Allow for windows case where the menu position is 0 based,
				// not 1 based.
				
		lastMenuItemToKeep--;
				
		numberMenuItems = cMenuPtr->GetMenuItemCount() - 1;						
		while (numberMenuItems > lastMenuItemToKeep)	
			{		
			cMenuPtr->DeleteMenu (numberMenuItems, MF_BYPOSITION);
			numberMenuItems--;
			
			}		// end "while (menuItem > lastMenuItemToKeep)"
	#endif	// defined multispec_win
	
}		// end "ClearMenuItems"		



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ClearOverlay
//
//	Software purpose:	This routine closes the overlay file as requested from the
//							selected clear overlay submenu item.
//
//	Parameters in:		menu item index
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/10/2003
//	Revised By:			Larry L. Biehl			Date: 07/13/2009

void ClearOverlay (
				UInt32			 					menuItemIndex)

{
	if (menuItemIndex <= gNumberImageOverlayFiles)
				// Clear the selected image overlay.
		ClearImageOverlay (menuItemIndex);
		
	else		// menuItemIndex > gNumberImageOverlayFiles
		ClearVectorOverlay (menuItemIndex-gNumberImageOverlayFiles);
	
}		// end "ClearOverlay"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CloseAllOverlayFiles
//
//	Software purpose:	This routine closes all of the overlay files and releases the
//							memory associated with them.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/10/2003
//	Revised By:			Larry L. Biehl			Date: 01/10/2003

void CloseAllOverlayFiles (void)

{	
	CloseAllImageOverlayFiles ();
	
	CloseAllVectorOverlayFiles ();
	
}		// end "CloseAllOverlayFiles"		



//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean DetermineIfProjectPossible
//
//	Software purpose:	The purpose of this routine is to determine if a project
//							can be created for the input image window.
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
//	Revised By:			Larry L. Biehl			Date: 10/19/2006

Boolean DetermineIfProjectPossible (
				Handle								imageWindowInfoH)

{	
	WindowInfoPtr						windowInfoPtr;
											
	
	windowInfoPtr = (WindowInfoPtr)GetHandlePointer (imageWindowInfoH,
																		kNoLock,
																		kNoMoveHi);	
										
	if (windowInfoPtr != NULL && 
			windowInfoPtr->imageType == kMultispectralImageType &&
				windowInfoPtr->numberImageFiles == 1)
																			return (TRUE);
																			
	return (FALSE);
	
}		// end "DetermineIfProjectPossible" 



//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoEditSelectAllImage
//
//	Software purpose:	The purpose of the following routines are to update the
//							menu items for the File menu. This is done here for cross
//							platform compatibility.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	Indication whether the menu item should be enabled.
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/13/1996
//	Revised By:			Larry L. Biehl			Date: 06/22/1999

void DoEditSelectAllImage (
				WindowPtr							windowPtr)

{	
	DisplaySpecsPtr					displaySpecsPtr;
	SelectionInfoPtr					selectionInfoPtr;
	LongRect*							lineColumnRectPtr;
	
	Handle								displaySpecsHandle,
											selectionInfoHandle,
											windowInfoHandle;
											
	
	windowInfoHandle = GetWindowInfoHandle (windowPtr);
	displaySpecsHandle = GetDisplaySpecsHandle (windowInfoHandle);
	selectionInfoHandle = GetSelectionInfoHandle (windowInfoHandle);
	
	if (displaySpecsHandle != NULL && selectionInfoHandle != NULL)
		{
				// Invalidate the current selection rectangle if it 		
				// exists.																
	
		ClearSelectionArea (windowPtr);
	
				// Get pointer to display specification for window.																
	
		displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer(
																displaySpecsHandle,
																kNoLock,
																kNoMoveHi);
		
				// Get pointer to structure which contains the selection	
				// rectangle information																								
	
		selectionInfoPtr = (SelectionInfoPtr)GetHandlePointer(
																selectionInfoHandle,
																kNoLock,
																kNoMoveHi);		
		
		selectionInfoPtr->offScreenRectangle.top = 0;
		selectionInfoPtr->offScreenRectangle.left = 0;
		selectionInfoPtr->offScreenRectangle.bottom = 
								(SInt16)displaySpecsPtr->imageDimensions[kVertical];
		selectionInfoPtr->offScreenRectangle.right =
								(SInt16)displaySpecsPtr->imageDimensions[kHorizontal];
																							
		selectionInfoPtr->lineColumnRectangle.left =
										displaySpecsPtr->displayedColumnStart;
		selectionInfoPtr->lineColumnRectangle.top =
											displaySpecsPtr->displayedLineStart;	
		selectionInfoPtr->lineColumnRectangle.right =
										displaySpecsPtr->displayedColumnEnd;	
		selectionInfoPtr->lineColumnRectangle.bottom =
											displaySpecsPtr->displayedLineEnd;
		
		lineColumnRectPtr = &selectionInfoPtr->lineColumnRectangle;									
		selectionInfoPtr->numberPixels = 
				(SInt64)(lineColumnRectPtr->right - lineColumnRectPtr->left + 1) * 
									(lineColumnRectPtr->bottom - lineColumnRectPtr->top + 1);
					
				// Set the selection area flag and set the selection 		
				// all flag.															
		
		selectionInfoPtr->typeFlag = kRectangleType;	
		
		UpdateSelectionCoordinates (windowInfoHandle);					
		
			// Outline selection rectangle.										
	
		OutlineSelectionArea (windowPtr);
		
				// Make sure that the coordinate view controls are correct if
				// displayed.
				
 		UpdateCoordinateViewControls (windowPtr);
			
				// Update the modal dialog with static text items			
			
		ShowSelection (windowInfoHandle);
		
				// Add selection to the statistics window if this is a project selection
				// window.
	 			
 		if (windowPtr == gProjectSelectionWindow && 
 												gProjectInfoPtr->selectionType == kRectangleType)
 			{
 			if (gProjectInfoPtr->statsWindowMode == kSelectFieldMode)	
				LoadRectangleInStatList (&selectionInfoPtr->lineColumnRectangle);
			
			gProjectInfoPtr->lastPointType = kRectangleType;

					// Hilite the "Add to List" control since a field is selected		
			
			if (gProjectWindow != NULL)		
				MHiliteControl (gProjectWindow, gProjectInfoPtr->addToControlH, 0);
 			
 			}		// end "if (gActiveImageWindow == gProjectSelectionWindow && ..."
		
		}		// end "if (displaySpecsHandle != NULL && ..."
					
}		// end "DoEditSelectAllImage"	



//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetControlPopUpMenuText
//
//	Software purpose:	The purpose of this routine is to get the text for for
//							the specified control popup menu item.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/29/2000
//	Revised By:			Larry L. Biehl			Date: 11/29/2000

void GetControlPopUpMenuText (
				ControlHandle						popupControlHandle,
				SInt16								popupMenuIndex,
				char*									stringPtr)

{	
#if defined multispec_mac  
	MenuHandle							menuHandle;
	
			
	menuHandle = GetPopUpMenuHandle (popupControlHandle);
	
	if (menuHandle != NULL)
		{	
		GetMenuItemText (menuHandle, popupMenuIndex, (UCharPtr)stringPtr);
		stringPtr[stringPtr[0]+1] = 0;
		
		}		// end "if (menuHandle != NULL)"
		
	else		// menuHandle == NULL
		{
		stringPtr[0] = 0;
		stringPtr[1] = 0;
		
		}		// end "else menuHandle == NULL"		
#endif	// defined multispec_mac   
                             
#if defined multispec_win
	 
#endif	// defined multispec_win 
	
}		// end "GetControlPopUpMenuText" 



//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetPopUpMenuBitsPerDataValue
//
//	Software purpose:	The purpose of this routine is to determine the requested
//							bits per data value from the menu selection.  It is
//							done in this manner so that the bits per data value
//							menu can be changed.
//
//	Parameters in:		The number of bits to compare menu item with.
//
//	Parameters out:	The menu item representing eight bits per data value.
//
// Value Returned:	The menu item which compares to the number of input bits. 
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/20/1992
//	Revised By:			Larry L. Biehl			Date: 11/02/1999	

SInt16 GetPopUpMenuBitsPerDataValue (
				SInt16								bitsPerDataValueSelection)

{
	SInt32								bitsPerDataValue;
	
	
	#if defined multispec_mac				
		GetMenuItemText (gPopUpBitsPerDataValueMenu, 
								bitsPerDataValueSelection, 
								gMenuItemString);
								
		StringToNum(gMenuItemString, &bitsPerDataValue);
	#endif 	// defined multispec_mac
	
	#if defined multispec_win
				// 'Hardwire for now. It would be nice to go through the resource
				// list so that the code would not have to be updated when changes
				// are made.
				             
		bitsPerDataValue = 0;              
		
		if (bitsPerDataValueSelection == 0)
			bitsPerDataValue = 4;
			
		else if (bitsPerDataValueSelection == 1) 
			bitsPerDataValue = 8; 
			
		else if (bitsPerDataValueSelection == 2) 
			bitsPerDataValue = 10;
			
		else if (bitsPerDataValueSelection == 3) 
			bitsPerDataValue = 12;
			
		else if (bitsPerDataValueSelection == 4) 
			bitsPerDataValue = 13;
			
		else if (bitsPerDataValueSelection == 5) 
			bitsPerDataValue = 14;
			
		else if (bitsPerDataValueSelection == 6) 
			bitsPerDataValue = 15;
			
		else if (bitsPerDataValueSelection == 7) 
			bitsPerDataValue = 16;
			
		if (bitsPerDataValue == 0)
			bitsPerDataValue = 8;
	#endif 	// defined multispec_mac
	
	return(  (SInt16)bitsPerDataValue );
	
}		// end "GetPopUpMenuBitsPerDataValue" 



//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetRectangularSelectionFlag
//
//	Software purpose:	The purpose of this routine is to return whether 
//							operations with the rectangular selection can be made.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/09/1998
//	Revised By:			Larry L. Biehl			Date: 10/09/1998

Boolean GetRectangularSelectionFlag (void)

{	
	
	Boolean		rectangularSelectionFlag = FALSE;
	
	
	if ((gActiveImageWindow != gProjectSelectionWindow) ||
				(gProjectInfoPtr != NULL &&
						gActiveImageWindow == gProjectSelectionWindow &&
								gProjectInfoPtr->selectionType == kRectangleType))
		rectangularSelectionFlag = TRUE;
		
	return (rectangularSelectionFlag);   
					
}		// end "GetRectangularSelectionFlag"	 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void MSetMenuItemText
//
//	Software purpose:	The purpose of this routine is to set the text in the specified
//							menu with the input text using a CFString to handle cases
//							when the input string could be in UTF8 format.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 03/14/2017
//	Revised By:			Larry L. Biehl			Date: 03/25/2017

void MSetMenuItemText (
				MenuRef								theMenu,
				MenuItemIndex						menuIndex,
				UInt8*								itemStringPascalPtr,
				UInt16								stringCharCode)
    
{
#	if defined multispec_mac
		CFStringRef				cfStringRef;
	
		cfStringRef = CFStringCreateWithBytes (kCFAllocatorDefault,
																&itemStringPascalPtr[1],
																itemStringPascalPtr[0],
																kCFStringEncodingUTF8,
																false);
																
		if (cfStringRef != NULL)
			{
			SetMenuItemTextWithCFString (theMenu,
													menuIndex,
													cfStringRef);
					// Using CFallocatorDeallocate causes system to crash later.
					// Not sure when one should use it and not use it.
			//CFAllocatorDeallocate (kCFAllocatorDefault, (void*)cfStringRef);
			CFRelease (cfStringRef);
			
			}		// end "if (cfStringRef != NULL)"
#	else
		SetMenuItemText (theMenu, menuIndex, itemStringPascalPtr);
#	endif
				
}		// end "MSetMenuItemText"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean SetTIFF_GeoTIFF_MenuItemString
//
//	Software purpose:	The purpose of this routine is to set the text in the specific 
//							menu location to be TIFF or GeoTIFF based on whether the map
//							projection information is available.
//
//	Parameters in:		window information structure pointer
//							menu resoure identifier
//							menu item
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 04/19/2013
//	Revised By:			Larry L. Biehl			Date: 03/02/2017	

Boolean SetTIFF_GeoTIFF_MenuItemString (
				WindowInfoPtr						windowInfoPtr,
				DialogPtr							dialogPtr,
				#if defined multispec_mac  
					MenuHandle							menuResource,
				#endif	// defined multispec_mac
				#if defined multispec_win  || defined multispec_lin
					UInt32								diskFileComboID,
				#endif	// defined multispec_win || defined multispec_lin 
				SInt16								menuItem)

{
	UInt8									tiffMenuNameString[16];
	
	Boolean								mapInfoExistsFlag;


#	if defined multispec_win
		USES_CONVERSION;
#	endif
	
	#if defined multispec_win  
		CComboBox* 							comboBoxPtr;
   #elif defined multispec_lin
      wxComboBox* comboBoxPtr;
	#endif	// defined multispec_win  
	
	
			// Set text indicating whether the output file format could be GeoTIFF
			// or TIFF
	
	mapInfoExistsFlag = FindIfMapInformationExists (windowInfoPtr);
		 
	#if defined multispec_win  
		comboBoxPtr = (CComboBox*)dialogPtr->GetDlgItem(diskFileComboID);
		comboBoxPtr->DeleteString (menuItem-1);
   #elif defined multispec_lin     
      if(diskFileComboID != NULL){              
            comboBoxPtr = (wxComboBox *)(dialogPtr->FindWindow(diskFileComboID));            
            comboBoxPtr->Delete(menuItem-1);         
      }
	#endif	// defined multispec_win  
	
	if (mapInfoExistsFlag)
		CtoPstring ((UCharPtr)"GeoTIFF", tiffMenuNameString);
		
	else		// !mapInfoExistsFlag
		CtoPstring ((UCharPtr)"TIFF", tiffMenuNameString);

	#if defined multispec_mac 
		SetMenuItemText (menuResource, 
								menuItem, 
								(UCharPtr)tiffMenuNameString);
	#endif	// defined multispec_mac 
	 
	#if defined multispec_win  
		comboBoxPtr->InsertString (menuItem-1, (LPCTSTR)A2T((LPCSTR)&tiffMenuNameString[1]));
		comboBoxPtr->SetItemData (menuItem-1, menuItem);
   #elif defined multispec_lin      
      if(diskFileComboID != NULL){
         comboBoxPtr->Insert(&tiffMenuNameString[1], menuItem-1);      
         comboBoxPtr->SetClientData (menuItem-1, (void*)menuItem);
      }
	#endif	// defined multispec_win 
	
	return (mapInfoExistsFlag);

}		// end "SetTIFF_GeoTIFF_MenuItemString"  


                   
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetUpClearOverlaysSubMenu
//
//	Software purpose:	The purpose of this routine is to set up the image overlay popup
//							menu with the overlay file names stored in the overlay structure.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:			SetUpClearOverlaysSubMenu in SMenus.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/28/2001
//	Revised By:			Larry L. Biehl			Date: 03/23/2017

void SetUpClearOverlaysSubMenu (
				MenuHandle							clearOverlySubMenuHandle)

{  
	Handle*								imageOverlayHandlePtr;
	Handle*								shapeHandlePtr;
	ImageOverlayInfoPtr				imageOverlayInfoPtr;
	ShapeInfoPtr						shapeInfoPtr;
	FileStringPtr						fileNamePPointer;			
			
	SInt32								menuItem;

	UInt32								index;
	
	SInt16								count;
											
	#if defined multispec_lin		
		wxMenu*								clearOverlayMenuPtr;	
	#endif	// defined multispec_mac 
											
	#if defined multispec_win		
		CMenu*								clearOverlayMenuPtr;		
		TBYTE*								unicodeStringPtr;
	#endif	// defined multispec_mac 
		
											
				// Now delete the current menu items after the separator (item 2).

	ClearMenuItems (clearOverlySubMenuHandle, 3);
				
	#if defined multispec_mac
		DisableMenuItem (clearOverlySubMenuHandle, 1);
		menuItem = 3;
	#endif	// defined multispec_mac 
	
	#if defined multispec_lin
		clearOverlayMenuPtr = (wxMenu*)clearOverlySubMenuHandle;
		menuItem = ID_CLEAROVERLAYMENUITEMSTART;
		clearOverlayMenuPtr->Enable (ID_EDIT_CLEAR_ALL_IMAGE_OVERLAYS, false);
	#endif	// defined multispec_lin 
	
	#if defined multispec_win
		clearOverlayMenuPtr = (CMenu*)clearOverlySubMenuHandle;
		menuItem = ID_CLEAROVERLAYMENUITEMSTART;
		clearOverlayMenuPtr->EnableMenuItem (ID_CLEAROVERLAYMENUITEMSTART, 
															MF_BYCOMMAND+MF_GRAYED);
	#endif	// defined multispec_win 
	
	if (gImageOverlayHandleListLength > 0)
		{
		#if defined multispec_mac
			EnableMenuItem (clearOverlySubMenuHandle, 1);
		#endif	// defined multispec_mac 
				
		#if defined multispec_lin
			clearOverlayMenuPtr->Enable (ID_EDIT_CLEAR_ALL_IMAGE_OVERLAYS, true);
		#endif	// defined multispec_lin 
				
		#if defined multispec_win
			clearOverlayMenuPtr->EnableMenuItem (ID_CLEAROVERLAYMENUITEMSTART, 
															MF_BYCOMMAND+MF_ENABLED);
		#endif	// defined multispec_win 

		
				// Add the image overlay labels to the menu
					
		imageOverlayHandlePtr = (Handle*)GetHandlePointer(
													gImageOverlayHandle, kLock, kNoMoveHi);
		
		if (imageOverlayHandlePtr != NULL)
			{		
			for (index=0; index<gImageOverlayHandleListLength; index++)
				{	
				imageOverlayInfoPtr = (ImageOverlayInfoPtr)GetHandlePointer (
																imageOverlayHandlePtr[index],
																kLock,
																kNoMoveHi);
			
				if (imageOverlayInfoPtr != NULL)
					{
					#if defined multispec_mac
						AppendMenu (clearOverlySubMenuHandle, 
											(UCharPtr)imageOverlayInfoPtr->overlayName);
						menuItem++;
						MSetMenuItemText (clearOverlySubMenuHandle,
												menuItem, 
												(UCharPtr)imageOverlayInfoPtr->overlayName,
												kUTF8CharString);
					#endif	// defined multispec_mac 
						
					#if defined multispec_lin
						clearOverlayMenuPtr->Append (
									menuItem, 
									wxString::FromAscii((char*)&imageOverlayInfoPtr->overlayName[1]),
									wxEmptyString,
									wxITEM_NORMAL);
						menuItem++;
					#endif	// defined multispec_lin 
						
					#if defined multispec_win
						unicodeStringPtr = 
								ConvertMultibyteStringToUnicodeString (&imageOverlayInfoPtr->overlayName[1]);
						clearOverlayMenuPtr->AppendMenu (
									MF_STRING, 
									menuItem, 
									(LPCTSTR)&unicodeStringPtr[1]);
						menuItem++;
					#endif	// defined multispec_win 
					
					CheckAndUnlockHandle (imageOverlayHandlePtr[index]);
					
					}		// end "if (imageOverlayInfoPtr != NULL)"
												
				}		// end "for (index=0; index<gImageOverlayHandleListLength; index++)"
				
			}		// end "if (imageOverlayHandlePtr != NULL)"
			
		CheckAndUnlockHandle (gImageOverlayHandle);
			
		}		// end "if (gImageOverlayHandleListLength > 0)"
	
	#if defined multispec_mac
		DisableMenuItem (clearOverlySubMenuHandle, 2);
	#endif	// defined multispec_mac 
	
	#if defined multispec_lin
		clearOverlayMenuPtr->Enable (ID_EDIT_CLEAR_ALL_IMAGE_OVERLAYS+1, false);
	#endif	// defined multispec_lin 
	
	#if defined multispec_win
		clearOverlayMenuPtr->EnableMenuItem (ID_CLEAROVERLAYMENUITEMSTART+1, 
															MF_BYCOMMAND+MF_GRAYED);
	#endif	// defined multispec_win 

	if (gNumberShapeFiles > 0)
		{
		#if defined multispec_mac
			EnableMenuItem (clearOverlySubMenuHandle, 2);
		#endif	// defined multispec_mac 
	
		#if defined multispec_lin
			clearOverlayMenuPtr->Enable (ID_EDIT_CLEAR_ALL_IMAGE_OVERLAYS+1, true);
		#endif	// defined multispec_lin 
	
		#if defined multispec_win
			clearOverlayMenuPtr->EnableMenuItem (ID_CLEAROVERLAYMENUITEMSTART+1, 
																MF_BYCOMMAND+MF_ENABLED);
		#endif	// defined multispec_win 
		
				// Add the vector overlay labels to the menu
					
		shapeHandlePtr = (Handle*)GetHandlePointer(
													gShapeFilesHandle, kLock, kNoMoveHi);

		if (shapeHandlePtr != NULL) 
			{
			for (index=0; index<gShapeHandleListLength; index++)
				{
				shapeInfoPtr = (ShapeInfoPtr)GetHandlePointer (
																shapeHandlePtr[index],
																kLock,
																kNoMoveHi);
																
				if (shapeInfoPtr != NULL)
					{										
					fileNamePPointer = (FileStringPtr)GetFileNamePPointer (shapeInfoPtr);
					CopyPToP (gTextString, fileNamePPointer);
			
					if (shapeInfoPtr->conversionCode == 2)
						{
						if (shapeInfoPtr->unitsCodeForConversion == kMetersCode)
							ConcatPStrings ((UCharPtr)gTextString, (StringPtr)"\0_ltom", 255);
							
						else		// ...->unitsCodeForConversion != kMetersCode. Assume feet
							ConcatPStrings ((UCharPtr)gTextString, (StringPtr)"\0_ltof", 255);
						
						}		// end "if (shapeInfoPtr->conversionCode == 2)"
						
					if (shapeInfoPtr->versionNumberLoaded > 1 && gTextString[0]< 250)
						{
						count = sprintf((char*)&gTextString[gTextString[0]+1], 
												"_%d",
												shapeInfoPtr->versionNumberLoaded);
						gTextString[0]+= count;
												
						}		// end "if (shapeInfoPtr->versionNumberLoaded > 1 && ..."
									
					#if defined multispec_mac
						AppendMenu (clearOverlySubMenuHandle, (UCharPtr)gTextString);
						menuItem++;
						MSetMenuItemText (clearOverlySubMenuHandle,
												menuItem, 
												(UCharPtr)gTextString,
												kUTF8CharString);
					#endif	// defined multispec_mac 
						
					#if defined multispec_lin
						wxString overlayitem((char*) &gTextString[1], wxConvUTF8);
						clearOverlayMenuPtr->Append (menuItem, overlayitem);
						menuItem++;
					#endif	// defined multispec_lin 	
						
					#if defined multispec_win
						unicodeStringPtr = 
								ConvertMultibyteStringToUnicodeString (&gTextString[1]);
						clearOverlayMenuPtr->AppendMenu (MF_STRING, 
																	menuItem, 
																	(LPCTSTR)&unicodeStringPtr[1]);
						menuItem++;
					#endif	// defined multispec_win 	
					
					CheckAndUnlockHandle (shapeHandlePtr[index]);
					
					}		// end "if (shapeInfoPtr != NULL)"
												
				}		// end "for (index=0; index<gShapeHandleListLength; index++)"
					    
			}		// end "if (shapeHandlePtr != NULL)"
			
		CheckAndUnlockHandle (gShapeFilesHandle);
		
		}		// end "if (gNumberShapeFiles > 0)"
			
}		// end Routine "SetUpClearOverlaysSubMenu" 


                   
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetUpImageOverlayPopUpMenu
//
//	Software purpose:	The purpose of this routine is to set up the image overlay popup
//							menu that is used in dialog boxes to allow the user to select
//							the location for the new image overlay that is being created.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/17/2003
//	Revised By:			Larry L. Biehl			Date: 03/22/2017

void SetUpImageOverlayPopUpMenu (
				MenuHandle							popUpMenuHandle,
				Handle								windowInfoHandle,
				SInt16								currentSelection)

{  
	ImageOverlayInfoPtr				imageOverlayInfoPtr;
	ImageOverlaySpecs*				overlayListPtr;
	WindowInfoPtr						windowInfoPtr;
	
	UInt32								listIndex,
											numberOverlays;
											
	SInt16								imageOverlayIndex;
		
	#if defined multispec_win
		CComboBox* 							imageOverlayComboBoxPtr;
	#endif	// defined multispec_win 
		
	
	windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle,
																		kNoLock,
																		kNoMoveHi);
																		
	if (windowInfoPtr != NULL)
		{															
		#if defined multispec_mac
					// Now delete the current menu items after the separator (item 1).
			ClearMenuItems (popUpMenuHandle, 1);
					// Make sure that there is no check for the first menu item. It
					// will be reset at the end if needed.
			CheckMenuItem (popUpMenuHandle, 1, FALSE);
		#endif	// defined multispec_mac 
		
		#if defined multispec_win
					// Get a pointer to the overlay combo box.
			imageOverlayComboBoxPtr = (CComboBox*)popUpMenuHandle; 
		#endif	// defined multispec_win 
		
		numberOverlays = windowInfoPtr->numberImageOverlays;
		overlayListPtr = windowInfoPtr->imageOverlayList;
		
		for (listIndex=0; listIndex<numberOverlays; listIndex++)
			{
			imageOverlayIndex = abs(overlayListPtr[listIndex].index) - 1;
			
			imageOverlayInfoPtr = GetImageOverlayInfoPtr ((SInt16)imageOverlayIndex, 
																			kNoLock,
																			NULL);
			
			if (imageOverlayInfoPtr != NULL)
				{
				gTextString[0] = sprintf ((char*)&gTextString[1], 
											"Replace %s", 
											(char*)&imageOverlayInfoPtr->overlayName[1]);
			
				#if defined multispec_mac
					AppendMenu (popUpMenuHandle, (UCharPtr)gTextString);
				#endif	// defined multispec_mac 
				
				#if defined multispec_win                     
					imageOverlayComboBoxPtr->AddString((LPCTSTR)&gTextString[1]);
				#endif	// defined multispec_mac 
				
				}		// end "if (imageOverlayInfoPtr != NULL)"
											
			}		// end "for (listIndex=0; listIndex<numberOverlays; listIndex++)"
		
				// Indicate the current selection.
					
		#if defined multispec_mac
			CheckMenuItem (popUpMenuHandle, currentSelection, TRUE);
		#endif	// defined multispec_mac 
			
		#if defined multispec_win
		#endif	// defined multispec_mac 
		
		}		// end "if (windowInfoPtr != NULL)"
			
}		// end Routine "SetUpImageOverlayPopUpMenu"    


                   
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetUpWindowOverlayPopUpMenu
//
//	Software purpose:	The purpose of this routine is to set up the overlay popup
//							menu with the overlay file names stored in both the image
//							overlay and vector overlay structures that is used in the image
//							windows.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/21/2001
//	Revised By:			Larry L. Biehl			Date: 03/21/2017

void SetUpWindowOverlayPopUpMenu (
				MenuHandle							popUpMenuHandle,
				Boolean								optionKeyFlag)

{  
	FileStringPtr						fileNamePPointer;
	
	Handle*								imageOverlayHandlePtr;
	Handle*								shapeHandlePtr;
	ImageOverlayInfoPtr				imageOverlayInfoPtr;
	ImageOverlaySpecsPtr				imageOverlayListPtr;
	OverlaySpecsPtr					overlayListPtr;
	WindowInfoPtr						windowInfoPtr;
	ShapeInfoPtr						shapeInfoPtr;
	
	SInt32								menuItem;
	 
	UInt32								index,
											numberOverlays;
	
	SInt16								count,
											imageFileIndex,
											shapeFileIndex;
	
	Boolean								checkFlag;
											
	SignedByte							windowHandleStatus;
		
	#if defined multispec_win
		CMenu*								clearOverlayPopupMenuPtr;
		TBYTE*								unicodeStringPtr;
		UINT									nFlags;
	#endif	// defined multispec_win 

	#if defined multispec_lin
		wxMenu*								clearOverlayPopupMenuPtr;
	#endif	// defined multispec_lin
	
		
	windowInfoPtr = (WindowInfoPtr)GetHandleStatusAndPointer (
							gActiveImageWindowInfoH, &windowHandleStatus, kNoMoveHi);

	if (windowInfoPtr != NULL && (windowInfoPtr->numberImageOverlays + 
														windowInfoPtr->numberOverlays) > 0)
		{
		#if defined multispec_mac
			CheckMenuItem (popUpMenuHandle, 1, FALSE);
			CheckMenuItem (popUpMenuHandle, 2, FALSE);
			
			if (optionKeyFlag)
				{
				DisableMenuItem (popUpMenuHandle, 1);
				DisableMenuItem (popUpMenuHandle, 2);
				
				}		// end "if (optionKeyFlag)"
			
			else		// !optionKeyFlag
				{
				EnableMenuItem (popUpMenuHandle, 1);
				EnableMenuItem (popUpMenuHandle, 2);
				
				}		// end "else !optionKeyFlag"
		
			menuItem = 4;				
		#endif	// defined multispec_mac 
		
		#if defined multispec_win
			clearOverlayPopupMenuPtr = (CMenu*)popUpMenuHandle;
			
			if (optionKeyFlag)
				{
				clearOverlayPopupMenuPtr->EnableMenuItem (
														ID_SHOWOVERLAYMENUITEMSTART, MF_GRAYED);
				clearOverlayPopupMenuPtr->EnableMenuItem (
														ID_SHOWOVERLAYMENUITEMSTART+1, MF_GRAYED);
				
				}		// end "if (optionKeyFlag)"
			
			else		// !optionKeyFlag
				{
				clearOverlayPopupMenuPtr->EnableMenuItem (
														ID_SHOWOVERLAYMENUITEMSTART, MF_ENABLED);
				clearOverlayPopupMenuPtr->EnableMenuItem (
														ID_SHOWOVERLAYMENUITEMSTART+1, MF_ENABLED);
				
				}		// end "else !optionKeyFlag"

			menuItem = ID_SHOWOVERLAYMENUITEMSTART + 3;
		#endif	// defined multispec_win 

		#if defined multispec_lin
			clearOverlayPopupMenuPtr = (wxMenu*)popUpMenuHandle;

			if (optionKeyFlag) {
				clearOverlayPopupMenuPtr->Enable(ID_SHOWOVERLAYMENUITEMSTART, false);
				clearOverlayPopupMenuPtr->Enable(ID_SHOWOVERLAYMENUITEMSTART + 1, false);

				}	// end "if (optionKeyFlag)"

			else // !optionKeyFlag
				{
				clearOverlayPopupMenuPtr->Enable(ID_SHOWOVERLAYMENUITEMSTART, true);
				clearOverlayPopupMenuPtr->Enable(ID_SHOWOVERLAYMENUITEMSTART + 1, true);

				} // end "else !optionKeyFlag"

			menuItem = ID_SHOWOVERLAYMENUITEMSTART + 3;
		#endif	// defined multispec_lin
				
			// Now get a pointer to the vector overlay list and the number of overlays.
								
		numberOverlays = windowInfoPtr->numberImageOverlays;
		imageOverlayListPtr = windowInfoPtr->imageOverlayList;
	
		imageOverlayHandlePtr = (Handle*)GetHandlePointer(
													gImageOverlayHandle, kLock, kNoMoveHi);
													
		if (imageOverlayHandlePtr == NULL)
			numberOverlays = 0;
	
		for (index=0; index<numberOverlays; index++)
			{
			imageFileIndex = imageOverlayListPtr[index].index;
			
			checkFlag = (imageFileIndex > 0);
			
			imageFileIndex = abs(imageFileIndex) - 1;
		
			imageOverlayInfoPtr = (ImageOverlayInfoPtr)GetHandlePointer (
															imageOverlayHandlePtr[imageFileIndex],
															kNoLock,
															kNoMoveHi);
			
			if (imageOverlayInfoPtr != NULL)
				CopyPToP (gTextString, imageOverlayInfoPtr->overlayName);
				
			else		// imageOverlayInfoPtr == NULL
				gTextString[0] = sprintf ((char*)&gTextString[1],
													(char*)"No Image Overlay %ld", 
													imageFileIndex);
				
			if (optionKeyFlag)
				ConcatPStrings ((UCharPtr)gTextString, (StringPtr)"\0...", 255);
			
			#if defined multispec_mac
				AppendMenu (popUpMenuHandle, (UCharPtr)gTextString);
				MSetMenuItemText (popUpMenuHandle,
										menuItem,
										(UCharPtr)gTextString,
										kUTF8CharString);
				CheckMenuItem (popUpMenuHandle, menuItem, checkFlag);
			#endif	// defined multispec_mac 
				
			#if defined multispec_win
				if (checkFlag)
					nFlags = MF_STRING+MF_CHECKED;
				else		// !checkFlag
					nFlags = MF_STRING+MF_UNCHECKED;

				unicodeStringPtr = ConvertMultibyteStringToUnicodeString (&gTextString[1]);

				clearOverlayPopupMenuPtr->AppendMenu (
														nFlags, 
														menuItem, 
														(LPCTSTR)&unicodeStringPtr[1]);
			#endif	// defined multispec_win 

			#if defined multispec_lin
				wxString overlayitem((char*) &gTextString[1], wxConvUTF8);
				clearOverlayPopupMenuPtr->AppendCheckItem(menuItem, overlayitem);
				if (checkFlag)
					clearOverlayPopupMenuPtr->Check(menuItem, true);
				else // !checkFlag
					clearOverlayPopupMenuPtr->Check(menuItem, false);

				//         clearOverlayPopupMenuPtr->AppendMenu(
				//                 nFlags,
				//                 menuItem,
				//                 (char*) &gTextString[1]);
			#endif	// defined multispec_lin
			
			menuItem++;
											
			}		// end "for (index=0; index<numberOverlays; ..."
			
		CheckAndUnlockHandle (gImageOverlayHandle);
				
			// Now get a pointer to the vector overlay list and the number of 
			// overlays.
								
		numberOverlays = windowInfoPtr->numberOverlays;
		overlayListPtr = windowInfoPtr->overlayList;
	
		shapeHandlePtr = (Handle*)GetHandlePointer(
													gShapeFilesHandle, kLock, kNoMoveHi);
													
		if (shapeHandlePtr == NULL)
			numberOverlays = 0;
	
		for (index=0; index<numberOverlays; index++)
			{
			shapeFileIndex = overlayListPtr[index].index;
			
			checkFlag = (shapeFileIndex > 0);
			
			shapeFileIndex = abs(shapeFileIndex) - 1;
		
			shapeInfoPtr = (ShapeInfoPtr)GetHandlePointer (
															shapeHandlePtr[shapeFileIndex],
															kLock,
															kNoMoveHi);
															
			if (shapeInfoPtr != NULL)
				{
				fileNamePPointer = (FileStringPtr)GetFileNamePPointer (shapeInfoPtr);
				CopyPToP (gTextString, fileNamePPointer);
				
				if (shapeInfoPtr->conversionCode == 2)
					{
					if (shapeInfoPtr->unitsCodeForConversion == kMetersCode)
						ConcatPStrings ((UCharPtr)gTextString, (StringPtr)"\0_ltom", 255);
						
					else		// ...->unitsCodeForConversion != kMetersCode. Assume feet
						ConcatPStrings ((UCharPtr)gTextString, (StringPtr)"\0_ltof", 255);
					
					}		// end "if (shapeInfoPtr->conversionCode == 2)"
							
				if (shapeInfoPtr->versionNumberLoaded > 1 && gTextString[0]< 250)
					{
					count = sprintf((char*)&gTextString[gTextString[0]+1], 
											"_%d",
											shapeInfoPtr->versionNumberLoaded);
					gTextString[0] += count;
									
					}		// end "if (shapeInfoPtr->versionNumberLoaded > 1 && ..."
				
				if (optionKeyFlag)
					ConcatPStrings ( (UCharPtr)gTextString, (StringPtr)"\0...", 255);
				
				#if defined multispec_mac
					AppendMenu (popUpMenuHandle, (UCharPtr)gTextString);
					MSetMenuItemText (popUpMenuHandle,
											menuItem,
											(UCharPtr)gTextString,
											kUTF8CharString);
					CheckMenuItem (popUpMenuHandle, menuItem, checkFlag);
				#endif	// defined multispec_mac 
					
				#if defined multispec_win
					if (checkFlag)
						nFlags = MF_STRING+MF_CHECKED;
					else		// !checkFlag
						nFlags = MF_STRING+MF_UNCHECKED;

					unicodeStringPtr = 
										ConvertMultibyteStringToUnicodeString (&gTextString[1]);

					clearOverlayPopupMenuPtr->AppendMenu (
															nFlags, 
															menuItem, 
															(LPCTSTR)&unicodeStringPtr[1]);
//					menuItem++;
				#endif	// defined multispec_win 

				#if defined multispec_lin
					wxString overlayitemb((char*) &gTextString[1], wxConvUTF8);
					clearOverlayPopupMenuPtr->AppendCheckItem(menuItem, overlayitemb);
					if (checkFlag)
						clearOverlayPopupMenuPtr->Check(menuItem, true);
					else // !checkFlag
						clearOverlayPopupMenuPtr->Check(menuItem, false);

					//         clearOverlayPopupMenuPtr->AppendMenu(
					//                 nFlags,
					//                 menuItem,
					//                 (char*) &gTextString[1]);
				#endif	// defined multispec_lin
				
				CheckAndUnlockHandle (shapeHandlePtr[shapeFileIndex]);
				
				menuItem++;
				
				}		// end "if (shapeInfoPtr != NULL)"
											
			}		// end "for (index=0; index<numberOverlays; ..."
			
		CheckAndUnlockHandle (gShapeFilesHandle);
													
		}		// end "if (windowInfoPtr != NULL && ..."
	
	MHSetState (gActiveWindowInfoH, windowHandleStatus);
			
}		// end Routine "SetUpWindowOverlayPopUpMenu"    



//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		"several"
//
//	Software purpose:	The purpose of the following routines are to update the
//							menu items for the File menu. This is done here for cross
//							platform compatibility.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	Indication whether the menu item should be enabled.
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/13/1996
//	Revised By:			Larry L. Biehl			Date: 05/13/1998

void UpdateEditGraphicsCopy (
				CCmdUI*								pCmdUI)

{	
	#if defined multispec_mac 
		SetMenuItemText (gMultiSpecMenus[kEditM], 
		 							kCopy, 
		 							"\pCopy Graph");
	#endif	// defined multispec_mac
	
	#if defined multispec_win 
		pCmdUI->SetText ((LPCTSTR)_T("Copy Graph\tCtrl+C"));
	#endif	// defined multispec_mac 
				
}		// end "UpdateEditGraphicsCopy"		




Boolean UpdateEditImageCopy (
				CCmdUI*								pCmdUI,
				WindowInfoPtr						windowInfoPtr)

{	
	Rect									tempRect;
	
	Boolean 								returnFlag = FALSE,
											selectionRectangleExistsFlag;
	
			
			// Determine if portion of image is selected.  If so, change the 
			// menu items for 'Save Image' to indicate that				
			// the selected portion of the image will be saved.			
													
	selectionRectangleExistsFlag = GetSelectedOffscreenRectangle (
													windowInfoPtr, 
													&tempRect, 
													TRUE,
													FALSE);									
	
	#if defined multispec_mac 
		if ( selectionRectangleExistsFlag )
			SetMenuItemText (gMultiSpecMenus[kEditM], 
		 							kCopy, 
		 							"\pCopy Image Selection");
	
		else		// !selectionRectangleExistsFlag 
			SetMenuItemText (gMultiSpecMenus[kEditM], 
		 							kCopy, 
		 							"\pCopy Image");
			
				// If there is no image in the image window, then disable the	
				// the save as menu item.						
				          
		if (gActiveOffscreenMapExists)
			returnFlag = TRUE; 
	#endif	// defined multispec_mac
	
	#if defined multispec_win
		if ( selectionRectangleExistsFlag )
			pCmdUI->SetText ((LPCTSTR)_T("Copy Image Selection\tCtrl+C"));
			
		else		// !selectionRectangleExistsFlag 
			pCmdUI->SetText ((LPCTSTR)_T("Copy Image\tCtrl+C"));
			
				// If there is no image in the image window, then disable the	
				// the copy menu item.	
		
		if (windowInfoPtr != NULL && windowInfoPtr->imageBaseAddressH != NULL) 
			returnFlag = TRUE;
	#endif	// defined multispec_win
		
	#if defined multispec_lin
		if ( selectionRectangleExistsFlag )
			pCmdUI->SetText (wxT("Copy Image Selection\tCtrl+C"));
		
		else		// !selectionRectangleExistsFlag
			pCmdUI->SetText (wxT("Copy Image\tCtrl+C"));
		
				// If there is no image in the image window, then disable the
				// the copy menu item.
		
		if (windowInfoPtr != NULL && windowInfoPtr->imageBaseAddressH != NULL)
			returnFlag = TRUE;
	#endif	// defined multispec_lin
	
	return (returnFlag);
				
}		// end "UpdateEditImageCopy"	



void UpdateEditImageSelectAll (
				CCmdUI*								pCmdUI)

{	
	#if defined multispec_mac 	
		SetMenuItemText (gMultiSpecMenus[kEditM], 
		 							kSelectAll, 
		 							"\pSelect All Image");			
	#endif	// defined multispec_mac
	
	#if defined multispec_win 	
		pCmdUI->SetText ((LPCTSTR)_T("Select All Image\tCtrl+A"));			
	#endif	// defined multispec_win 
	
	#if defined multispec_lin 	
		pCmdUI->SetText (wxT("Select All Image\tCtrl+A"));			
	#endif	// defined multispec_lin 
				
}		// end "UpdateEditImageSelectAll"



void UpdateEditClearSelection (
				CCmdUI*								pCmdUI,
				SInt16								selectionTypeCode)

{	
	Boolean								polygonTypeFlag = FALSE;
	
	
   if (selectionTypeCode == kPolygonType)
   	polygonTypeFlag = TRUE;
	 								
	#if defined multispec_mac 
		if ( !polygonTypeFlag )
			SetMenuItemText (gMultiSpecMenus[kEditM], 
		 							kClearSelectionRect, 
		 							"\pClear Selection Rectangle");
		 							
		else		// polygonTypeFlag
			SetMenuItemText (gMultiSpecMenus[kEditM], 
		 							kClearSelectionRect, 
		 							"\pClear Selection Polygon");
	#endif	// defined multispec_mac
	
	#if defined multispec_win 
		if ( !polygonTypeFlag )
			pCmdUI->SetText ((LPCTSTR)_T("Clear Selection Rectangle\tDelete"));
		 							
		else		// polygonTypeFlag
			pCmdUI->SetText ((LPCTSTR)_T("Clear Selection Polygon\tDelete"));
	#endif	// defined multispec_win 
				
	#if defined multispec_lin
		if (!polygonTypeFlag)
		  pCmdUI->SetText(wxT("Clear Selection Rectangle\tDelete"));

		else // polygonTypeFlag
		  pCmdUI->SetText(wxT("Clear Selection Polygon\tDelete"));
	#endif

} // end "UpdateEditClearSelection"


void UpdateEditTextClear (
				CCmdUI*								pCmdUI)

{	
	#if defined multispec_mac 
		SetMenuItemText (gMultiSpecMenus[kEditM], 
		 							kClear, 
		 							"\pClear Selected Text");
	#endif	// defined multispec_mac
	
	#if defined multispec_win 
		pCmdUI->SetText ((LPCTSTR)_T("Clear Selected Text\tDelete"));
	#endif	// defined multispec_win
	
	#if defined multispec_lin 
		pCmdUI->SetText (wxT("Clear Selected Text\tDelete"));
	#endif	// defined multispec_lin
				
}		// end "UpdateEditTextClear"



void UpdateEditTextCopy (
				CCmdUI*								pCmdUI)

{	
	#if defined multispec_mac 
		SetMenuItemText (gMultiSpecMenus[kEditM], 
		 							kCopy, 
		 							"\pCopy Selected Text");
	#endif	// defined multispec_mac
	
	#if defined multispec_win 
		pCmdUI->SetText ((LPCTSTR)_T("Copy Selected Text\tCtrl+C"));
	#endif	// defined multispec_win
	
	#if defined multispec_lin 
		pCmdUI->SetText (wxT("Copy Selected Text\tCtrl+C"));
	#endif	// defined multispec_lin 
				
}		// end "UpdateEditTextCopy"



void UpdateEditTextCut (
				CCmdUI*								pCmdUI)

{	
	#if defined multispec_mac 
		SetMenuItemText (gMultiSpecMenus[kEditM], 
		 							kCut, 
		 							"\pCut Selected Text");
	#endif	// defined multispec_mac
	
	#if defined multispec_win
		pCmdUI->SetText ((LPCTSTR)_T("Cut Selected Text\tCtrl+X"));
	#endif	// defined multispec_win 
	
	#if defined multispec_lin
		pCmdUI->SetText (wxT("Cut Selected Text\tCtrl+X"));
	#endif	// defined multispec_lin 
				
}		// end "UpdateEditTextCut"



void UpdateEditTextPaste (
				CCmdUI*								pCmdUI)

{	
	#if defined multispec_mac 
		SetMenuItemText (gMultiSpecMenus[kEditM], 
		 							kPaste, 
		 							"\pPaste Text");
	#endif	// defined multispec_mac
	
	#if defined multispec_win
		pCmdUI->SetText ((LPCTSTR)_T("Paste Text\tCtrl+V"));
	#endif	// defined multispec_win 
	
	#if defined multispec_lin
		pCmdUI->SetText (wxT("Paste Text\tCtrl+V"));
	#endif	// defined multispec_lin 
				
}		// end "UpdateEditTextPaste"



void UpdateEditTextSelectAll (
				CCmdUI*								pCmdUI)

{	
	#if defined multispec_mac 
		SetMenuItemText (gMultiSpecMenus[kEditM], 
		 							kSelectAll, 
		 							"\pSelect All Text");
	#endif	// defined multispec_mac
	
	#if defined multispec_win
		pCmdUI->SetText ((LPCTSTR)_T("Select All Text\tCtrl+A"));
	#endif	// defined multispec_win
	
	#if defined multispec_lin 	
		pCmdUI->SetText (wxT("Select All Text\tCtrl+A"));			
	#endif	// defined multispec_lin 
				
}		// end "UpdateEditTextSelectAll"



void UpdateFileGraphClose (
				CCmdUI*								pCmdUI)

{	
	#if defined multispec_mac 
		SetMenuItemText (gMultiSpecMenus[kFileM], 
	 							kCloseImage, 
	 							"\pClose Graph Window");
	#endif	// defined multispec_mac
	
	#if defined multispec_win 
		pCmdUI->SetText ((LPCTSTR)_T("&Close Graph Window\tCtrl+W"));
	#endif	// defined multispec_mac 
	
	#if defined multispec_lin 	
		pCmdUI->SetText (wxT("Close Graph Window\tCtrl+W"));			
	#endif	// defined multispec_lin 
				
}		// end "UpdateFileGraphClose"	



void UpdateFileImageClose (
				CCmdUI*								pCmdUI)

{	
	#if defined multispec_mac 
		SetMenuItemText (gMultiSpecMenus[kFileM], 
	 							kCloseImage, 
	 							"\pClose Image Window");
	#endif	// defined multispec_mac
	
	#if defined multispec_win
		pCmdUI->SetText ((LPCTSTR)_T("&Close Image Window\tCtrl+W"));
	#endif	// defined multispec_win
	
	#if defined multispec_lin
		pCmdUI->SetText (wxT("&Close Image Window\tCtrl+W"));
	#endif	// defined multispec_lin 
				
}		// end "UpdateFileImageClose"	



void UpdateFilePaletteClose (
				CCmdUI*								pCmdUI)

{	
	#if defined multispec_mac 
		SetMenuItemText (gMultiSpecMenus[kFileM], 
	 							kCloseImage, 
	 							"\pClose Palette Window");
	#endif	// defined multispec_mac
	
	#if defined multispec_win 
		pCmdUI->SetText ((LPCTSTR)_T("&Close Palette Window\tCtrl+W"));
	#endif	// defined multispec_mac 
				
}		// end "UpdateFilePaletteClose"	



void UpdateFileProjectClose (
				CCmdUI*								pCmdUI)

{	
	#if defined multispec_mac 
		SetMenuItemText (gMultiSpecMenus[kFileM], 
	 							kCloseImage, 
	 							"\pClose Project");
	#endif	// defined multispec_mac
	
	#if defined multispec_win 
		pCmdUI->SetText ((LPCTSTR)_T("&Close Project\tCtrl+W"));
	#endif	// defined multispec_win
	
	#if defined multispec_lin
		pCmdUI->SetText (wxT("&Close Project\tCtrl+W"));
	#endif	// defined multispec_lin 
				
}		// end "UpdateFileProjectClose"	



void UpdateFileTextClose (
				CCmdUI*								pCmdUI)

{	
	#if defined multispec_mac 
		SetMenuItemText (gMultiSpecMenus[kFileM], 
	 							kCloseImage, 
	 							"\pClose Window");
	#endif	// defined multispec_mac
	
	#if defined multispec_win 
		pCmdUI->SetText ((LPCTSTR)_T("&Close Window\tCtrl+W"));
	#endif	// defined multispec_win 
	
	#if defined multispec_lin 
		pCmdUI->SetText (wxT("&Close Window\tCtrl+W"));
	#endif	// defined multispec_lin 
				
}		// end "UpdateFileTextClose"	



Boolean UpdateFileDefaultPrint (
				CCmdUI*								pCmdUI)

{	
	Boolean 								returnFlag = FALSE;											
		
	#if defined multispec_mac 
		SetMenuItemText (gMultiSpecMenus[kFileM], 
	 							kPrintOutput, 
	 							"\pPrint...");
	#endif	// defined multispec_mac
	
	#if defined multispec_win 
		pCmdUI->SetText ((LPCTSTR)_T("&Print...\tCtrl+P"));
	#endif	// defined multispec_mac 
		
	return (returnFlag);
				
}		// end "UpdateFileDefaultPrint"



Boolean UpdateFileGraphPrint (
				CCmdUI*								pCmdUI)

{	
	#if defined multispec_mac 
		SetMenuItemText (gMultiSpecMenus[kFileM], 
	 							kPrintOutput, 
	 							"\pPrint Graph Window...");
	#endif	// defined multispec_mac
	
	#if defined multispec_win 
		pCmdUI->SetText ((LPCTSTR)_T("&Print Graph Window...\tCtrl+P"));
	#endif	// defined multispec_win 
	
	#if defined multispec_lin 
		pCmdUI->SetText (wxT("Print Graph Window...\tCtrl+P"));
	#endif	// defined multispec_lin
		
	return (TRUE);
				
}		// end "UpdateFileGraphPrint"	



Boolean UpdateFileDefaultSaveAs (
				CCmdUI*								pCmdUI)

{	
	Boolean 								returnFlag = FALSE;
													
	
	#if defined multispec_mac 
		SetMenuItemText (gMultiSpecMenus[kFileM], 
	 							kSaveOutputAs, 
	 							"\pSave As...");
	#endif	// defined multispec_mac
	
	#if defined multispec_win 
		pCmdUI->SetText ((LPCTSTR)_T("Save &As...\tCtrl+S"));
	#endif	// defined multispec_win 
	
	#if defined multispec_lin 
		pCmdUI->SetText (wxT("Save &As...\tCtrl+S"));
	#endif	// defined multispec_lin 
		
	return (returnFlag);
				
}		// end "UpdateFileDefaultSaveAs"	



Boolean UpdateFileOpenThematicInfo (
				CCmdUI*								pCmdUI)

{	
			// Open Thematic Class/Group Info Menu item.
			                                                       
	Handle windowInfoHandle = GetActiveWindowInfoHandle ();
	SInt16 windowType = GetWindowType (windowInfoHandle);
	SInt16 classGroupCode = GetDisplayClassGroupCode (windowInfoHandle);
 		                                                                 
	if (classGroupCode >= kGroupDisplay)
		{
		#if defined multispec_mac 
			SetMenuItemText (gMultiSpecMenus[kFileM], 
		 							kOpenThematicClassGroup, 
		 							"\pOpen Thematic Group Info...");
		#endif	// defined multispec_mac
		
		#if defined multispec_win 
			pCmdUI->SetText ((LPCTSTR)_T("Open &Thematic Group Info..."));
		#endif	// defined multispec_win
		
		#if defined multispec_lin 
			pCmdUI->SetText (wxT("Open Thematic Group Info..."));
		#endif	// defined multispec_lin
		}
	else		// classGroupCode == kClassDisplay || classGroupCode == 0 
				// (!kThematicWindowType)
		{
		#if defined multispec_mac 
			SetMenuItemText (gMultiSpecMenus[kFileM], 
		 							kOpenThematicClassGroup, 
		 							"\pOpen Thematic Class Info...");
		#endif	// defined multispec_mac
		
		#if defined multispec_win 
			pCmdUI->SetText ((LPCTSTR)_T("Open &Thematic Class Info..."));
		#endif	// defined multispec_win
		
		#if defined multispec_lin 
			pCmdUI->SetText (wxT("Open Thematic Class Info..."));
		#endif	// defined multispec_lin
		}
 		
	return (windowType == kThematicWindowType);
				
}		// end "UpdateFileOpenThematicInfo"  



Boolean UpdateFileTransform (
				CCmdUI*								pCmdUI)

{
	Boolean								returnFlag;
	
									                                     
			// Load or Save transformation matrix menu item.
 		                                                                
	if (gTransformationMatrix.numberChannels <= 0)
		{
		#if defined multispec_mac 
			SetMenuItemText (gMultiSpecMenus[kFileM], 
		 							kSaveTransformMenuItem, 
		 							"\pLoad Transformation Matrix...");
		#endif	// defined multispec_mac
		
		#if defined multispec_win 
			pCmdUI->SetText ((LPCTSTR)_T("&Load Transformation Matrix..."));
		#endif	// defined multispec_win
		
		#if defined multispec_lin
			pCmdUI->SetText (wxT("Load Transformation Matrix..."));
		#endif	// defined multispec_lin
		}
	else 		// gTransformationMatrix.numberChannels > 0
		{
		#if defined multispec_mac 
			SetMenuItemText (gMultiSpecMenus[kFileM], 
		 							kSaveTransformMenuItem, 
		 							"\pSave Transformation Matrix...");
		#endif	// defined multispec_mac
		
		#if defined multispec_win 
			pCmdUI->SetText ((LPCTSTR)_T("&Save Transformation Matrix..."));
		#endif	// defined multispec_win 
		
		#if defined multispec_lin 
			pCmdUI->SetText (wxT("Save Transformation Matrix..."));
		#endif	// defined multispec_lin  
		}
		
	returnFlag = TRUE;
	if (!gFullVersionFlag || gTransformationMatrix.createdByCode >= 16)
		returnFlag = FALSE;
 		
 	return (returnFlag);	
				
}		// end "UpdateFileTransform"



Boolean UpdateFileSave (
				CCmdUI*								pCmdUI)

{	
	Boolean 								returnFlag = FALSE;
	FileInfoPtr							fileInfoPtr;
	Handle								fileInfoHandle;
	
	                                                             
	Handle windowInfoHandle = GetActiveWindowInfoHandle (); 
	SInt16 classGroupCode = GetDisplayClassGroupCode (windowInfoHandle);
		
			// Save Thematic Class/Group Info Menu item.
 		                                                                
	if (classGroupCode >= kGroupDisplay)
   {
		#if defined multispec_mac 
			SetMenuItemText (gMultiSpecMenus[kFileM], 
		 							kSave, 
		 							"\pSave Thematic Group Info");
		#endif	// defined multispec_mac
		
		#if defined multispec_win 
			pCmdUI->SetText ((LPCTSTR)_T("&Save Thematic Group Info"));
		#endif	// defined multispec_win
		
		#if defined multispec_lin
			pCmdUI->SetText("&Save Thematic Group Info");
		#endif
	}
	else if (classGroupCode == kClassDisplay)
		{
		#if defined multispec_mac 
			SetMenuItemText (gMultiSpecMenus[kFileM], 
		 							kSave, 
		 							"\pSave Thematic Class Info As...");
		#endif	// defined multispec_mac
		
		#if defined multispec_win
			pCmdUI->SetText ((LPCTSTR)_T("&Save Thematic Class Info As..."));
		#endif	// defined multispec_win
					
		#if defined multispec_lin
			pCmdUI->SetText (wxT("&Save Thematic Class Info As..."));
		#endif	// defined multispec_lin
		}
		
	else		// classGroupCode == 0, i.e. != kThematicWindowType
		{
		#if defined multispec_mac 
			SetMenuItemText (gMultiSpecMenus[kFileM], 
		 							kSave, 
		 							"\pSave");
		#endif	// defined multispec_mac
		
		#if defined multispec_win
			pCmdUI->SetText ((LPCTSTR)_T("&Save"));
		#endif	// defined multispec_win
					
		#if defined multispec_lin
			pCmdUI->SetText (wxT("&Save"));
		#endif	// defined multispec_lin
   }
 	
 	fileInfoHandle = GetActiveImageFileInfoHandle ();
 	fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle, kNoLock, kNoMoveHi);	
	if ( (classGroupCode == kClassDisplay) ||
			(classGroupCode >= kGroupDisplay && fileInfoPtr != NULL && 
						fileInfoPtr->groupChangedFlag && fileInfoPtr->bandInterleave <= kBIS) )
 		returnFlag = TRUE;
 		
 	return (returnFlag);	
				
}		// end "UpdateFileSave"



Boolean UpdateFileOutputTextSaveAs (
				CCmdUI*								pCmdUI)

{	
	Boolean 								returnFlag = FALSE;
	
	
	Boolean charactersSelectedFlag = CharactersSelected ();
	
			
			// Determine if text is selected.  If so change the menu items			
			// for 'Save Output Text' and 'Print Text' to indicate that				
			// Selected text will be used.													
	
	if (charactersSelectedFlag)
		{
		#if defined multispec_mac 
			SetMenuItemText (gMultiSpecMenus[kFileM], 
		 							kSaveOutputAs, 
		 							"\pSave Selected Text As...");
		#endif	// defined multispec_mac
		
		#if defined multispec_win
			pCmdUI->SetText ((LPCTSTR)_T("Save Selected Text &As...\tCtrl+S"));
		#endif	// defined multispec_win
					
		#if defined multispec_lin
			pCmdUI->SetText (wxT("Save Selected Text &As...\tCtrl+S"));
		#endif	// defined multispec_lin
		}
	else		// !charactersSelectedFlag 
		{
		#if defined multispec_mac 
			SetMenuItemText (gMultiSpecMenus[kFileM], 
		 							kSaveOutputAs, 
		 							"\pSave Text Output As...");
		#endif	// defined multispec_mac
		
		#if defined multispec_win
			pCmdUI->SetText ((LPCTSTR)_T("Save Text Output &As...\tCtrl+S"));
		#endif	// defined multispec_win
					
		#if defined multispec_lin
			pCmdUI->SetText (wxT("Save Text Output &As...\tCtrl+S"));
		#endif	// defined multispec_lin
		}
			
				// If there is no text in the output window, then disable the	
				// the print and save text output menu items.						
				                   
	#if defined multispec_mac 
		#if use_mlte_for_text_window
			if (TXNDataSize (gOutputTextH) > 2) 
		#endif
		#if !use_mlte_for_text_window
			if (WEGetTextLength (gOutputTextH) > 2) 
		#endif
			returnFlag = TRUE;
	#endif	// defined multispec_mac
		
	#if defined multispec_win || defined multispec_lin                             
		returnFlag = TRUE;  
	#endif	// defined multispec_win || defined multispec_lin
		
	return (returnFlag);
				
}		// end "UpdateFileOutputTextSaveAs"	



Boolean UpdateFileOutputTextPrint (
				CCmdUI*								pCmdUI)

{	
	Boolean 								returnFlag = FALSE;
	
	
	Boolean charactersSelectedFlag = CharactersSelected ();											
		
	if (charactersSelectedFlag)
   {
		#if defined multispec_mac 
			SetMenuItemText (gMultiSpecMenus[kFileM], 
		 							kPrintOutput, 
//		 							"\pPrint Selected Text...");
		 							"\pPrint Text...");
		#endif	// defined multispec_mac
		
		#if defined multispec_win 
			pCmdUI->SetText ((LPCTSTR)_T("&Print Selected Text...\tCtrl+P"));
		#endif	// defined multispec_mac 
	}	
	else		// !charactersSelectedFlag 
   {
		#if defined multispec_mac 
			SetMenuItemText (gMultiSpecMenus[kFileM], 
		 							kPrintOutput, 
		 							"\pPrint Text...");
		#endif	// defined multispec_mac
		
		#if defined multispec_win 
			pCmdUI->SetText ((LPCTSTR)_T("&Print Text...\tCtrl+P"));
		#endif	// defined multispec_mac  
   }
			
				// Check if there is text in the output window.						
				                   
	#if defined multispec_mac 
		#if use_mlte_for_text_window
			if (TXNDataSize (gOutputTextH) > 2) 
		#endif
		#if !use_mlte_for_text_window
			if (WEGetTextLength (gOutputTextH) > 2)
		#endif
				returnFlag = TRUE;
	#endif	// defined multispec_mac
		
	#if defined multispec_win                             
		returnFlag = TRUE;  
	#endif	// defined multispec_win
	
	return (returnFlag);
				
}		// end "UpdateFileOutputTextPrint"	



Boolean UpdateFileImageSaveAs (
				CCmdUI*								pCmdUI,
				WindowInfoPtr						windowInfoPtr)

{	
	Rect									tempRect;
	
	Boolean 								returnFlag = FALSE,
											mapInfoExistsFlag,
											selectionRectangleExistsFlag;
	

			// Determine if portion of image is selected.  If so, change the 
			// menu items for 'Save Image' to indicate that				
			// the selected portion of the image will be saved.			
													
	selectionRectangleExistsFlag = GetSelectedOffscreenRectangle (
													windowInfoPtr, 
													&tempRect, 
													TRUE,
													FALSE);	
													
	mapInfoExistsFlag = FindIfMapInformationExists (windowInfoPtr);								
	
	if ( selectionRectangleExistsFlag )
   {
		#if defined multispec_mac 
			if (mapInfoExistsFlag)
				SetMenuItemText (gMultiSpecMenus[kFileM], 
		 							kSaveOutputAs, 
		 							"\pSave Image Selection To GeoTIFF As...");
		 	else	//! mapInfoExistsFlag
				SetMenuItemText (gMultiSpecMenus[kFileM], 
		 							kSaveOutputAs, 
		 							"\pSave Image Selection To TIFF As...");
		#endif	// defined multispec_mac
		
		#if defined multispec_win
			if (mapInfoExistsFlag)
				pCmdUI->SetText ((LPCTSTR)_T("Save Image Selection To GeoTIFF &As...\tCtrl+S"));
		 	else	//! mapInfoExistsFlag
				pCmdUI->SetText ((LPCTSTR)_T("Save Image Selection To TIFF &As...\tCtrl+S"));
		#endif	// defined multispec_win
				
		#if defined multispec_lin
			if (mapInfoExistsFlag)
				pCmdUI->SetText (wxT("Save Image Selection To GeoTIFF &As...\tCtrl+S"));
			else	//! mapInfoExistsFlag
				pCmdUI->SetText (wxT("Save Image Selection To TIFF &As...\tCtrl+S"));
		#endif	// defined multispec_lin
	}
	else		// !selectionRectangleExistsFlag
   {
		#if defined multispec_mac 
			if (mapInfoExistsFlag)
				SetMenuItemText (gMultiSpecMenus[kFileM], 
		 								kSaveOutputAs, 
		 								"\pSave Image To GeoTIFF As...");
		 	else	//! mapInfoExistsFlag
				SetMenuItemText (gMultiSpecMenus[kFileM], 
		 								kSaveOutputAs, 
		 								"\pSave Image To TIFF As...");
		#endif	// defined multispec_mac
		
		#if defined multispec_win
			if (mapInfoExistsFlag)
				pCmdUI->SetText ((LPCTSTR)_T("Save Image To GeoTIFF &As...\tCtrl+S"));
		 	else	//! mapInfoExistsFlag
				pCmdUI->SetText ((LPCTSTR)_T("Save Image To TIFF &As...\tCtrl+S"));
		#endif	// defined multispec_win
				
		#if defined multispec_lin
				if (mapInfoExistsFlag)
					pCmdUI->SetText (wxT("Save Image To GeoTIFF &As...\tCtrl+S"));
				else	//! mapInfoExistsFlag
					pCmdUI->SetText (wxT("Save Image To TIFF &As...\tCtrl+S"));
		#endif	// defined multispec_lin
   }
	
				// If there is no image in the image window, then disable the
				// the save as menu item.						
				           
	#if defined multispec_mac 
		if ( gActiveOffscreenMapExists )
			returnFlag = TRUE; 
	#endif	// defined multispec_mac
		
	#if defined multispec_win || defined multispec_lin 
		if (windowInfoPtr != NULL && windowInfoPtr->imageBaseAddressH != NULL) 
			returnFlag = TRUE;
	#endif	// defined multispec_win || defined multispec_lin 
		
	return (returnFlag);
				
}		// end "UpdateFileImageSaveAs"	



Boolean UpdateFileImagePrint (
				CCmdUI*								pCmdUI,
				WindowInfoPtr						windowInfoPtr)

{	
	Rect									tempRect;
	
	Boolean 								returnFlag = FALSE,
											selectionRectangleExistsFlag;
	
			
			// Determine if portion of image is selected.  If so, change the 
			// menu items for 'Save Image' to indicate that				
			// the selected portion of the image will be saved.				
													
	selectionRectangleExistsFlag = GetSelectedOffscreenRectangle (
													windowInfoPtr, 
													&tempRect, 
													TRUE,
													FALSE);												
		
	if (selectionRectangleExistsFlag)
		{
		#if defined multispec_mac 
			SetMenuItemText (gMultiSpecMenus[kFileM], 
		 							kPrintOutput, 
		 							"\pPrint Image Selection...");
		#endif	// defined multispec_mac
		
		#if defined multispec_win 
			pCmdUI->SetText ((LPCTSTR)_T("&Print Image Selection...\tCtrl+P"));
		#endif	// defined multispec_win 
		
		#if defined multispec_lin
			pCmdUI->SetText (wxT("&Print Image Selection...\tCtrl+P"));
		#endif	// defined multispec_liin 
		}	
	else		// !selectionRectangleExistsFlag 
		{
		#if defined multispec_mac 
			SetMenuItemText (gMultiSpecMenus[kFileM], 
		 							kPrintOutput, 
		 							"\pPrint Image...");
		#endif	// defined multispec_mac
		
		#if defined multispec_win 
			pCmdUI->SetText ((LPCTSTR)_T("&Print Image...\tCtrl+P"));
		#endif	// defined multispec_win  
		
		#if defined multispec_lin
			pCmdUI->SetText (wxT("&Print Image...\tCtrl+P"));
		#endif	// defined multispec_lin   
		}	// end "else !selectionRectangleExistsFlag"
			
				// If there is no image in the image window, then disable the	
				// the save as menu item.						
				           
	#if defined multispec_mac 
		if ( gActiveOffscreenMapExists )
			returnFlag = TRUE; 
	#endif	// defined multispec_mac
		
	#if defined multispec_win
		if (windowInfoPtr != NULL && windowInfoPtr->imageBaseAddressH != NULL) 
			returnFlag = TRUE;
	#endif	// defined multispec_win
		
	#if defined multispec_lin 
		if (windowInfoPtr != NULL && windowInfoPtr->imageBaseAddressH != NULL) 
			returnFlag = TRUE;
	#endif	// defined multispec_win 
		
	return (returnFlag);
				
}		// end "UpdateFileImagePrint"	



Boolean UpdateEditImageMapParameters (void)

{	
	Boolean 								returnFlag = FALSE;
	Handle								windowInfoHandle;
	SInt16								windowType;
	
	                                                             
	windowInfoHandle = GetActiveImageWindowInfoHandle (); 
	windowType = GetWindowType (windowInfoHandle);
		
			// Save Thematic Class/Group Info Menu item.
 		                                                                
	if (windowType == kImageWindowType || windowType == kThematicWindowType)
		returnFlag = TRUE;
	
 	return (returnFlag);	
				
}		// end "UpdateEditImageMapParameters"



Boolean UpdateEditClearOverlays (
				CCmdUI*								pCmdUI)

{	
	if (gNumberShapeFiles + gNumberImageOverlayFiles > 0)
		{
		#if defined multispec_mac 
			SetUpClearOverlaysSubMenu (gMultiSpecMenus[kOverlaysM]);
		
			return (TRUE);
		#endif	// defined multispec_mac
			
		#if defined multispec_lin
			if (pCmdUI != NULL)
				SetUpClearOverlaysSubMenu (pCmdUI);
		
			if (gNumberImageOverlayFiles > 0)
				return (TRUE);
			else		// gNumberImageOverlayFiles == 0
				return (FALSE);
		#endif	// defined multispec_lin
			
		#if defined multispec_win
			if (pCmdUI->m_pMenu != NULL && pCmdUI->m_pSubMenu == NULL)
				SetUpClearOverlaysSubMenu (pCmdUI->m_pMenu);
		
			if (gNumberImageOverlayFiles > 0)
				return (TRUE);
			else		// gNumberImageOverlayFiles == 0
				return (FALSE);
		#endif	// defined multispec_win
		
		}		// end "if (gNumberShapeFiles + gNumberImageOverlayFiles > 0)"
		
	else		// gNumberShapeFiles == 0 && gNumberImageOverlayFiles == 0
		{	
		#if defined multispec_lin
			if (pCmdUI != NULL)
				SetUpClearOverlaysSubMenu (pCmdUI);
		#endif	// defined multispec_lin
		
		#if defined multispec_win
			if (pCmdUI->m_pMenu != NULL && pCmdUI->m_pSubMenu == NULL)
				SetUpClearOverlaysSubMenu (pCmdUI->m_pMenu);
		#endif	// defined multispec_win

		return (FALSE);
		
		}		// end "else gNumberShapeFiles == 0 && ..."
				
}		// end "UpdateEditClearOverlays"	




Boolean UpdateReformatChangeHeader (
				WindowInfoPtr						windowInfoPtr,
				FileInfoPtr							fileInfoPtr,
				CCmdUI*								pCmdUI)

{	
	Boolean 								returnFlag = TRUE;
	
														
	#if defined multispec_mac 
		if (fileInfoPtr != NULL && fileInfoPtr->format != 0)
			SetMenuItemText (gMultiSpecMenus[kReformatM], 
		 							kReformatChangeHeaderMenuItem, 
		 							"\pChange Header...");
	
		else		// fileInfoPtr == NULL || fileInfoPtr->format == 0
			SetMenuItemText (gMultiSpecMenus[kReformatM], 
		 							kReformatChangeHeaderMenuItem, 
		 							"\pInsert Header...");
	#endif	// defined multispec_mac
	
	#if defined multispec_win 
		if (fileInfoPtr != NULL && fileInfoPtr->format != 0)
			pCmdUI->SetText ((LPCTSTR)_T("Change Header..."));
			
		else		// fileInfoPtr == NULL || fileInfoPtr->format == 0 
			pCmdUI->SetText ((LPCTSTR)_T("Insert Header..."));
	#endif	// defined multispec_win
	
	if (!gFullVersionFlag ||
			fileInfoPtr == NULL ||
			windowInfoPtr == NULL ||
			windowInfoPtr->numberImageFiles > 1 ||
			fileInfoPtr->bandInterleave != kBIL )
		returnFlag = FALSE;
		
			// Currently one can only add and modify headers for image files 		
			// that have no header or are of ERDAS type.									
			
	else if (fileInfoPtr->format != kErdas73Type &&
					fileInfoPtr->format != kErdas74Type &&
					fileInfoPtr->format != 0)
		returnFlag = FALSE;
		
	return (returnFlag);
				
}		// end "UpdateReformatChangeHeader"	



Boolean UpdateReformatConvertShape (
				WindowInfoPtr						windowInfoPtr,
				FileInfoPtr							fileInfoPtr,
				CCmdUI*								pCmdUI)

{	
	ArcViewRecordHeaderPtr			arcViewRecordHeaderPtr;
	Handle*								shapeHandlePtr;
	Ptr									vectorDataPtr = NULL;
	ShapeInfoPtr						shapeInfoPtr = NULL;
	
	SInt32								shapeFileIndex;
	
	Boolean 								returnFlag = TRUE;
	
														
	if (windowInfoPtr == NULL || windowInfoPtr->numberOverlays == 0)
		returnFlag = FALSE;
		
	else 		// windowInfoPtr != NULL && ...
		{
		shapeFileIndex = abs(windowInfoPtr->overlayList[0].index) - 1;
		
		shapeHandlePtr = (Handle*)GetHandlePointer(
														gShapeFilesHandle, kNoLock, kNoMoveHi);
		
		if (shapeHandlePtr != NULL)
			shapeInfoPtr = (ShapeInfoPtr)GetHandlePointer (
														shapeHandlePtr[shapeFileIndex],
														kNoLock,
														kNoMoveHi);	
								
		if (shapeInfoPtr != NULL)						
			vectorDataPtr = (Ptr)GetHandlePointer (shapeInfoPtr->vectorDataHandle,
														kNoLock,
														kNoMoveHi);
									
		if (vectorDataPtr != NULL)
			{						
			arcViewRecordHeaderPtr = (ArcViewRecordHeaderPtr)vectorDataPtr;
			
			if (arcViewRecordHeaderPtr->shapeType != 1 && 
						arcViewRecordHeaderPtr->shapeType != 3 && 
								arcViewRecordHeaderPtr->shapeType != 5)
				returnFlag = FALSE;
				
			}		// end "if (vectorDataPtr != NULL)"
		
		else		// vectorDataPtr == NULL
			returnFlag = FALSE;
		
		}		// end "else windowInfoPtr != NULL && ..."
		
	return (returnFlag);
				
}		// end "UpdateReformatConvertShape"		



Boolean UpdateReformatModifyChannel (
				WindowInfoPtr						windowInfoPtr,
				FileInfoPtr							fileInfoPtr,
				CCmdUI*								pCmdUI)

{	
	Boolean 								returnFlag = TRUE,
											versionOKFlag = FALSE;  
			
																									
	#if defined multispec_mac 
		if (fileInfoPtr != NULL && fileInfoPtr->descriptionsFlag)
			SetMenuItemText (gMultiSpecMenus[kReformatM], 
		 							kReformatModifyChannelMenuItem, 
		 							"\pModify Channel Descriptions...");
	
		else		// fileInfoPtr == NULL || !fileInfoPtr->descriptionsFlag
			SetMenuItemText (gMultiSpecMenus[kReformatM], 
		 							kReformatModifyChannelMenuItem, 
		 							"\pAdd Channel Descriptions...");
	#endif	// defined multispec_mac
	
	#if defined multispec_win 
		if (fileInfoPtr != NULL && fileInfoPtr->descriptionsFlag)
			pCmdUI->SetText ((LPCTSTR)_T("Modify Channel Descriptions..."));
			
		else		// fileInfoPtr == NULL || !fileInfoPtr->descriptionsFlag
			pCmdUI->SetText ((LPCTSTR)_T("Add Channel Descriptions..."));
	#endif	// defined multispec_win 

   #if defined multispec_lin
		if (fileInfoPtr != NULL && fileInfoPtr->descriptionsFlag)
			pCmdUI->SetText (_T("Modify Channel Descriptions..."));
			
		else		// fileInfoPtr == NULL || !fileInfoPtr->descriptionsFlag
			pCmdUI->SetText (_T("Add Channel Descriptions..."));
	#endif	// defined multispec_lin 
	
			// Make sure that the version is okay to allow channel descriptions
			// to be added.	
		
	if (fileInfoPtr != NULL && (fileInfoPtr->format == kNoneType ||
											fileInfoPtr->format == kErdas73Type ||
											fileInfoPtr->format == kErdas74Type ||
											fileInfoPtr->format == kFastL7AType ||
											fileInfoPtr->format == kENVIType ||
											fileInfoPtr->format == kArcViewType ) )
		versionOKFlag = TRUE;
									
			// Do not allow this option for thematic images.							
			// Also wavelength descriptions will not be allow for a window image	
			// with more than one image disk file.	  
	
	if (!versionOKFlag || 
				windowInfoPtr == NULL ||
				windowInfoPtr->numberImageFiles > 1 ||
					fileInfoPtr->thematicType ||
						fileInfoPtr->bandInterleave > kBIS ||
							!gFullVersionFlag)
		returnFlag = FALSE;
		
	return (returnFlag);
				
}		// end "UpdateReformatModifyChannel"	



Boolean UpdateReformatMosaicImages (
				CCmdUI*								pCmdUI)

{	
	SInt16								listLength;
	Boolean								returnFlag = FALSE;

	gProcessorCode = kRefMosaicImagesProcessor;
	GetImageList (NULL, gActiveImageWindowInfoH, 0, FALSE, &listLength);
	gProcessorCode = 0;
				
	if (listLength > 0)
		returnFlag = TRUE;	
									
	return (returnFlag);
				
}		// end "UpdateReformatMosaicImages"	



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean UpdateReformatRecodeThematicImage
//
//	Software purpose:	This routine checks if the image file represented by the active
//							image window can be recode.
//
//	Parameters in:		menu item index
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/16/2013
//	Revised By:			Larry L. Biehl			Date: 01/08/2015

Boolean UpdateReformatRecodeThematicImage (
				FileInfoPtr							fileInfoPtr)

{	
	Boolean 								headerFromGDALOKFlag,
											returnFlag = FALSE;
	
														
	if (fileInfoPtr != NULL)
		{
		headerFromGDALOKFlag = FALSE;
		if (fileInfoPtr->gdalDataSetH == NULL ||
				((fileInfoPtr->format == kTIFFType ||
								fileInfoPtr->format == kGeoTIFFType) &&
										fileInfoPtr->numberHeaderBytes > 0))
			headerFromGDALOKFlag = TRUE;

		if (gFullVersionFlag &&
				fileInfoPtr != NULL && 
					fileInfoPtr->thematicType &&
						fileInfoPtr->dataCompressionCode == kNoCompression && 
							fileInfoPtr->bandInterleave <= kBNonSQMenuItem &&
								fileInfoPtr->numberBytes >= 1 &&
									fileInfoPtr->numberBytes <= 2 &&
										headerFromGDALOKFlag)
			returnFlag = TRUE;
								
		}		// end "if (fileInfoPtr != NULL)"
		
	return (returnFlag);
				
}		// end "UpdateReformatRecodeThematicImage"



Boolean UpdateWindowCoordinateView (
				CCmdUI*								pCmdUI)

{	
	Boolean 								returnFlag;
	Handle								windowInfoHandle;
	SInt16								windowType;
	
	                                                             
	windowInfoHandle = GetActiveWindowInfoHandle (); 
	windowType = GetWindowType (windowInfoHandle);
	
//	if (windowType == kStatisticsWindowType)
//		{
//		windowInfoHandle = GetActiveImageWindowInfoHandle ();
//		windowType = GetWindowType (windowInfoHandle);
//		
//		}		// end "if (windowType == kStatisticsWindowType)"
		
	if (windowType == kImageWindowType || windowType == kThematicWindowType)
		{
		returnFlag = TRUE;
		
		if (GetCoordinateHeight(windowInfoHandle) > 0)
			{
			#if defined multispec_mac 
				SetMenuItemText (gMultiSpecMenus[kWindowM], 
										kShowHideCoordinateView, 
										"\pHide Coordinate View");
			#endif	// defined multispec_mac
			
			#if defined multispec_win 
				pCmdUI->SetText ((LPCTSTR)_T("&Hide Coordinate View"));
			#endif	// defined multispec_win 
			
			#if defined multispec_lin 
				pCmdUI->SetText (wxT("&Hide Coordinate View"));
			#endif	// defined multispec_lin 
			}							
		else		// coordinateHeight == 0
			{
			#if defined multispec_mac 
				SetMenuItemText (gMultiSpecMenus[kWindowM], 
										kShowHideCoordinateView, 
										"\pShow Coordinate View");
			#endif	// defined multispec_mac
			
			#if defined multispec_win 
				pCmdUI->SetText ((LPCTSTR)_T("&Show Coordinate View"));
			#endif	// defined multispec_win
			
			#if defined multispec_lin 
				pCmdUI->SetText (wxT("&Show Coordinate View"));
			#endif	// defined multispec_lin 
			}
		
		}		// end "if (windowInfoPtr->windowType == kImageWindowType || ..."
		
	else		// windowType != kImageWindowType && ...
		{
		returnFlag = FALSE;
		
		#if defined multispec_mac 
			SetMenuItemText (gMultiSpecMenus[kWindowM], 
									kShowHideCoordinateView, 
									"\pShow Coordinate View");
		#endif	// defined multispec_mac
		
		#if defined multispec_win 
			pCmdUI->SetText ((LPCTSTR)_T("&Show Coordinate View"));
		#endif	// defined multispec_win
		
		#if defined multispec_lin 
			pCmdUI->SetText (wxT("&Show Coordinate View"));
		#endif	// defined multispec_lin 
		
		}		// end "else windowType != kImageWindowType && ..."
	
 	return (returnFlag);	
				
}		// end "UpdateWindowCoordinateView"



Boolean UpdateWindowSelectionGraph (void)

{	
	Boolean 								returnFlag = FALSE;
	
	                                                 
	if (gActiveImageWindow != NULL)
		{	
		if (gNumberOfGWindows < kMaxNumberGWindows)
			returnFlag = TRUE;
			
		}		// end "if (gActiveImageWindow != NULL)"
	
 	return (returnFlag);	
				
}		// end "UpdateWindowSelectionGraph"