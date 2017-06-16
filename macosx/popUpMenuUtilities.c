//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2017)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						popUpMenuUtilities.c
//
//	Authors:					Larry L. Biehl, Ravi Budruk
//
//	Revision number:		3.0
//
//	Revision date:			03/15/2017
//
//	Language:				C
//
//	System:					Macintosh Operating System
//
//	Brief description:	The purpose of the routines in this file is to
//								provide popup menu functions in MultiSpec.
//
//	Functions in file:	SInt16	 			ChannelsPopUpMenu
//								pascal void 		DrawBandInterleavePopUp
//								pascal void 		DrawBitsPerDataValuePopUp
//								pascal void 		DrawBytesPerDataValuePopUp
//								pascal void 		DrawChannelsPopUp
//								pascal void 		DrawDialogClassPopUp
//								void 					DrawDropDownTriangle
//								pascal void 		DrawImageFilePopUp 
//								void 					DrawPopUpMenuItem
//								pascal void 		DrawSymbolsPopUp
//								pascal void 		DrawWeightsPopUp
//								SInt16	 			GetPopUpMenuBitsPerDataValue
//								SInt16	 			StandardPopUpMenu

//#include	"SMulSpec.h"
//#include "SExtGlob.h" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				SInt16 ChannelsPopUpMenu
//
//	Software purpose:			The purpose of this routine is to handle a 
//									mousedown in the channels pop up menu location.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/02/1989
//	Revised By:			Larry L. Biehl			Date: 12/07/1990

SInt16 ChannelsPopUpMenu (
				DialogPtr							dialogPtr, 
				SInt16								dialogItem,
				SInt16								allType, 
				SInt16								currentChannelSelection, 
				SInt16								listType)

{
	MenuHandle							popUpMenu;
	
	SInt16								itemHit,
											popUpMenuID;
	
	
	if (allType == 1)
		{
		popUpMenu = gPopUpAllSubsetMenu;
		popUpMenuID = kPopUpAllSubsetMenuID;
		
		}		// end "if (allType == 1)" 
		
	else if (allType == 2)
		{
		popUpMenu = gPopUpAllAvailableSubsetMenu;
		popUpMenuID = kPopUpAllAvailableSubsetMenuID;
		
		}		// end "else if (allType == 2)" 
		
	if (listType == kSelectedItemsListOnly)
		{
		if (currentChannelSelection == kSubsetMenuItem)
	 		DisableMenuItem (popUpMenu, 1);
	 		
	 	if (currentChannelSelection == kAllMenuItem)
	 		DisableMenuItem (popUpMenu, 2);
		
		}		// end "if (listType == kSelectedItemsListOnly)" 
		
	itemHit = StandardPopUpMenu ( dialogPtr, 
												dialogItem-1,
												dialogItem, 
												popUpMenu, 
												currentChannelSelection, 
												popUpMenuID);
 	
 			// Make certain that both menu items are enabled.							
 			
	EnableMenuItem (popUpMenu, 1);
	EnableMenuItem (popUpMenu, 2);
	
			// Return the selection that was made.											
	
	return (itemHit);
			
}		// end "ChannelsPopUpMenu" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void DrawBandInterleavePopUp
//
//	Software purpose:	The purpose of this routine is to draw the band
//							interleave pop up menu.
//
//	Parameters in:		dialog pointer, dialog item number.
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			ModalFileSpecification in fileIO.c
//
//	Coded By:			Larry L. Biehl			Date: 11/01/1990
//	Revised By:			Larry L. Biehl			Date: 11/01/1990	

pascal void DrawBandInterleavePopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{

			// Use the generic pop up drawing routine.									
			
	DrawPopUpMenuItem (dialogPtr, itemNumber, 
						gPopUpBandInterleaveMenu, gBandInterleaveSelection, TRUE);
	
}		// end "DrawBandInterleavePopUp"  


/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void DrawDataValueTypePopUp
//
//	Software purpose:	The purpose of this routine is to draw the data value type 
//							pop up menu.
//
//	Parameters in:		dialog pointer, dialog item number.
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			ModalFileSpecification in fileIO.c
//
//	Coded By:			Larry L. Biehl			Date: 11/24/2003
//	Revised By:			Larry L. Biehl			Date: 01/11/2006	

pascal void DrawDataValueTypePopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{

			// Use the generic pop up drawing routine.									
			
	DrawPopUpMenuItem (dialogPtr, itemNumber, 
				gPopUpDataValueTypeMenu, gDataValueTypeSelection, TRUE);
	
}		// end "DrawDataValueTypePopUp" 
*/

/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void DrawBitsPerDataValuePopUp
//
//	Software purpose:	The purpose of this routine is to draw the bits per
//							data value pop up menu.
//
//	Parameters in:		dialog pointer, dialog item number.
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			ModalFileSpecification in fileIO.c
//							ChangeImageFormatDialog in reformat.c
//
//	Coded By:			Larry L. Biehl			Date: 11/01/1990
//	Revised By:			Larry L. Biehl			Date: 11/01/1990	

pascal void DrawBitsPerDataValuePopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{

			// Use the generic pop up drawing routine.									
			
	DrawPopUpMenuItem (dialogPtr, itemNumber, 
				gPopUpBitsPerDataValueMenu, gBitsPerDataValueSelection, TRUE);
	
}		// end "DrawBitsPerDataValuePopUp" 
*/


//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void DrawDataValueTypePopUp
//
//	Software purpose:	The purpose of this routine is to draw the bytes per
//							data value pop up menu.
//
//	Parameters in:		dialog pointer, dialog item number.
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			ModalFileSpecification in fileIO.c
//
//	Coded By:			Larry L. Biehl			Date: 11/02/1990
//	Revised By:			Larry L. Biehl			Date: 01/11/2006	

pascal void DrawDataValueTypePopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{

			// Use the generic pop up drawing routine.									
			
	DrawPopUpMenuItem (dialogPtr, itemNumber, 
				gPopUpDataValueTypeMenu, gDataValueTypeSelection, TRUE);
	
}		// end "DrawDataValueTypePopUp" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void DrawChannelsPopUp
//
//	Software purpose:	The purpose of this routine is to draw the channel
//							prompt and channel set selection for channel
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
//	Coded By:			Larry L. Biehl			Date: 10/02/1989
//	Revised By:			Larry L. Biehl			Date: 10/30/1990	

pascal void DrawChannelsPopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.									
			
	DrawPopUpMenuItem (dialogPtr, itemNumber, 
										gPopUpAllSubsetMenu, gChannelSelection, TRUE);
	
}		// end "DrawChannelsPopUp" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void DrawDialogClassPopUp
//
//	Software purpose:	The purpose of this routine is to draw the class
//							prompt and class set selection for class
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
//	Coded By:			Larry L. Biehl			Date: 10/16/1989
//	Revised By:			Larry L. Biehl			Date: 10/30/1989	

pascal void DrawDialogClassPopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{	
			// Use the generic pop up drawing routine.									
			
	DrawPopUpMenuItem (dialogPtr, itemNumber, 
											gPopUpAllSubsetMenu, gClassSelection, TRUE);
	
}		// end "DrawDialogClassPopUp" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean DrawDropDownTriangle
//
//	Software purpose:	The purpose of this routine is to draw the drop-down
//							triangle that indicates that this shaded rectangle 
//							is a drop-down (i.e. popup) menu.  This code was take
//							from a network news article from Greg Anderson of Apple
//							on October 8, 1990.  He gave permission for anyone to
//							use.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Greg Anderson			Date: ?
//	Revised By:			Larry L. Biehl			Date: 10/30/1990	

void DrawDropDownTriangle (
				Rect*									theRectPtr)

{
	PenNormal();
	if(gTempRegion1)
		{
		OpenRgn();
		MoveTo( theRectPtr->right - 5, theRectPtr->top + 6);
		LineTo( theRectPtr->right - 17, theRectPtr->top + 6);
		LineTo( theRectPtr->right - 11, theRectPtr->top + 13);
		LineTo( theRectPtr->right - 5, theRectPtr->top + 6);
		CloseRgn (gTempRegion1);
		InvertRgn (gTempRegion1);
		SetEmptyRgn (gTempRegion1);
		
		}		// end "if(gTempRegion1)" 
	
}		// end "DrawDropDownTriangle" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean DrawDropDownTriangle2
//
//	Software purpose:	The purpose of this routine is to draw the drop-down
//							triangle that indicates that this shaded rectangle 
//							is a drop-down (i.e. popup) menu.  This code was take
//							from a network news article from Greg Anderson of Apple
//							on October 8, 1990.  He gave permission for anyone to
//							use. This draws a smaller triangle than DrawDropDownTriangle
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Greg Anderson			Date: ?
//	Revised By:			Larry L. Biehl			Date: 12/14/2000	

void DrawDropDownTriangle2 (
				Rect*									theRectPtr)

{
	PenNormal();
	if(gTempRegion1)
		{
		OpenRgn();
		MoveTo( theRectPtr->right - 3, theRectPtr->top + 6);
		LineTo( theRectPtr->right - 13, theRectPtr->top + 6);
		LineTo( theRectPtr->right - 8, theRectPtr->top + 12);
		LineTo( theRectPtr->right - 3, theRectPtr->top + 6);
		CloseRgn (gTempRegion1);
		InvertRgn (gTempRegion1);
		SetEmptyRgn (gTempRegion1);
		
		}		// end "if(gTempRegion1)" 
	
}		// end "DrawDropDownTriangle2" 
										


//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void DrawImageFilePopUp
//
//	Software purpose:	The purpose of this routine is to draw the image file
//							names prompt and selection for image file name 
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
//	Coded By:			Larry L. Biehl			Date: 01/07/1993
//	Revised By:			Larry L. Biehl			Date: 01/07/1993	

pascal void DrawImageFilePopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.									
			
	DrawPopUpMenuItem (dialogPtr, itemNumber,
						gPopUpTemporaryMenu, gFileNamesSelection, TRUE);
	
}		// end "DrawImageFilePopUp" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void DrawImageOverlayPopUp
//
//	Software purpose:	The purpose of this routine is to draw the popup menu for
//							selecting the overlay location to be used to create a new image
//							overlay. This routine will also draw a drop shadow box around 
//							the pop up selection.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 01/17/2003
//	Revised By:			Larry L. Biehl			Date: 01/17/2003	

pascal void DrawImageOverlayPopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{	
			// Use the generic pop up drawing routine.									
			
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpImageOverlayMenu, 
								gSelectImageOverlaySelection, 
								TRUE);
	
}		// end "DrawImageOverlayPopUp" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void DrawLinkOptionsPopUp
//
//	Software purpose:	The purpose of this routine is to draw the link options popup	
//							menu in the Get File dialog box.
//
//	Parameters in:		dialog pointer, dialog item number.
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			ModalFileSpecification in fileIO.c
//
//	Coded By:			Larry L. Biehl			Date: 12/13/2012
//	Revised By:			Larry L. Biehl			Date: 12/13/2012	

pascal void DrawLinkOptionsPopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{

			// Use the generic pop up drawing routine.									
			
	DrawPopUpMenuItem (dialogPtr, itemNumber, 
				gPopUpLinkOptionsMenu, gLinkOptionsSelection, TRUE);
	
}		// end "DrawLinkOptionsPopUp" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DrawPopUpMenuItem
//
//	Software purpose:	The purpose of this routine is to draw the requested
//							pop up selection.  This routine will also 
//							draw a drop shadow box around the pop up selection if
//							the current selection is greater than 0.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/28/1990
//	Revised By:			Larry L. Biehl			Date: 03/25/2017

void DrawPopUpMenuItem (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber, 
				MenuHandle							popUpMenuHandle, 
				SInt16								menuItem, 
				Boolean								drawStringFlag)

{
			// Declare local variables and structures										
	
	Rect									popUpBox,
											dialogWindowRect;
											
	CFStringRef							cfStringRef;
											
	Handle								theHandle;
	
	OSStatus								osStatus;
	
	SInt16								bottomSpace = 5,
											leftSpace = 12,
											localMenuItem,
											stringWidth,
											theType;
											
	CGContextRef						context;

	
			// Make certain that the correct graph port is set up.					
			
	SetPortDialogPort (dialogPtr);
	
			// Get the rectangle for the pop up box.										
			
	GetDialogItem (dialogPtr, itemNumber, &theType, &theHandle, &popUpBox);
	
			// Clear the pop up box.
			// Allow a little extra space at the top for the top line															
			
	popUpBox.top -= 2;
	EraseRect (&popUpBox);
	popUpBox.top += 2;
	
	if (drawStringFlag)
		{
		gMenuItemString[0] = 0;
		cfStringRef = NULL;
		osStatus = noErr;
		
		if (popUpMenuHandle != NULL)
			{
					// Make certain that we have a correct menu item number.
			
			localMenuItem = abs(menuItem);
			if (menuItem < 0 && popUpMenuHandle != gPopUpBitsPerDataValueMenu && 
											popUpMenuHandle != gPopUpClusterMaskDiskFileMenu && 
											popUpMenuHandle != gPopUpBitsOfColorMenu &&
											popUpMenuHandle != gPopUpDisplayTypeMenu &&
											popUpMenuHandle != gPopUpDataValueTypeMenu &&
											popUpMenuHandle != gPopUpMapUnitsMenu &&
											popUpMenuHandle != gPopUpProjectionMenu &&
											popUpMenuHandle != gPopUpHorizontalDatumMenu &&
											popUpMenuHandle != gPopUpEllipsoidMenu)
				localMenuItem = 1;
			
					// Draw the current selection.
					// Will try to draw text using HIThemeDrawTextBox so that unicode characters
					// can be drawn. If not possible, will use DrawString.
			
			osStatus = CopyMenuItemTextAsCFString (
										  popUpMenuHandle,
										  localMenuItem,
										  &cfStringRef);
			
			if (osStatus == noErr && cfStringRef != NULL)
				{
				CFStringGetCString (cfStringRef,
											(char*)&gMenuItemString[1],
											(CFIndex)255,
											kCFStringEncodingUTF8);
				gMenuItemString[0] = strlen ((char*)&gMenuItemString[1]);
				
				}		// end "if (cfStringRef != NULL)"
			
			else
				GetMenuItemText (popUpMenuHandle, localMenuItem, gMenuItemString);
			
			}	// end "if (popUpMenuHandle != NULL)"
		
		else // popUpMenuHandle == NULL
			{
					// Specific text is to be drawn
					// If menuItem is 1, then draw text is for the project changes popup menu in
					// the statistics dialog window.
			
			if (menuItem == 1)
				{
				cfStringRef = CFSTR("Project Changes");
				}
			
			}	// end "else popUpMenuHandle == NULL"
			
		if (gMenuItemString[0] == 0 && cfStringRef == NULL)
																											return;
		
		context = NULL;
		GetWindowPortBounds (GetDialogWindow(dialogPtr), &dialogWindowRect);
		HIRect			theRect = {popUpBox.left+4, 
										 dialogWindowRect.bottom-popUpBox.bottom+1, 
										 popUpBox.right-popUpBox.left-25,		// Allow for down arrow
										 popUpBox.bottom-popUpBox.top};
										 
		HIThemeTextInfo textInfo = {0, 
												kThemeStateActive, 
												kThemeMenuItemFont,		// or kThemeSystemFont
												kHIThemeTextHorizontalFlushLeft, 
												kHIThemeTextVerticalFlushCenter,
												//kHIThemeTextTruncationNone,
												//kHIThemeTextTruncationEnd, 
												kHIThemeTextTruncationMiddle,
												0,
												false};
												
		if (osStatus == noErr)
			osStatus = CreateCGContextForPort (GetWindowPort (GetDialogWindow(dialogPtr)), &context);
		
		if (osStatus == noErr)
			osStatus = HIThemeDrawTextBox (cfStringRef, 
														&theRect,
														&textInfo,
														context,
														kHIThemeOrientationInverted);
													
		if (osStatus != noErr)
			{											
			MoveTo (popUpBox.left+leftSpace, popUpBox.bottom-bottomSpace);
		
					// Get the string width and make sure it is within the space allowed.
					// Allow for drop-down triangle.
		
			stringWidth = popUpBox.right - popUpBox.left;
				
			if (stringWidth > 35)
				stringWidth -= 19;
			
			CheckStringLength (stringWidth, 
									(Str255*)gMenuItemString, 
									(Str255*)gTextString3);
									
			DrawString (gTextString3);
			
			}	// end "if (osStatus != noErr)"
								
		if (menuItem > 0)
			{
					// PopUp menu exits, therefore....										
					// Draw the pop up box around currently selected pop up option	
					// and the drop down triangle.											
		
			popUpBox.top -= 2;
			DrawDropShadowBox (popUpBox);
			DrawDropDownTriangle (&popUpBox);
			
			}		// end "if (menuItem > 0)" 
			
		if (context != NULL)
			{				
			gCGContextFlushPtr (context);
			gCGContextReleasePtr (context);
			
			}		// end "if (context != NULL)"
		
		}		// end "if (drawStringFlag)" 
	
}		// end "DrawPopUpMenuItem" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void DrawSymbolsPopUp
//
//	Software purpose:	The purpose of this routine is to draw the symbols
//							prompt and symbol set selection for class symbols
//							pop up menu.  This routine will also draw a drop
//							shadow box around the pop up selection.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	 Value Returned:	None		
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 07/26/1990
//	Revised By:			Larry L. Biehl			Date: 10/30/1990	

pascal void DrawSymbolsPopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.									
			
	DrawPopUpMenuItem (dialogPtr, itemNumber, 
												gPopUpSymbolsMenu, gSymbolSelection, TRUE);
	
}		// end "DrawSymbolsPopUp" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void DrawWeightsPopUp
//
//	Software purpose:	The purpose of this routine is to draw the weights
//							prompt and weight set selection for class weights
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
//	Coded By:			Larry L. Biehl			Date: 10/16/1989
//	Revised By:			Larry L. Biehl			Date: 10/30/1990	

pascal void DrawWeightsPopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{	
			// Use the generic pop up drawing routine.									
			
	DrawPopUpMenuItem (dialogPtr, itemNumber, 
											gPopUpWeightsMenu, gWeightsSelection, TRUE);
	
}		// end "DrawWeightsPopUp" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 StandardPopUpMenu
//
//	Software purpose:	The purpose of this routine is to handle the 'standard'
//							popup menu action when a mouse down occurs in the 
//							popup rectangle in a dialog box.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/30/1990
//	Revised By:			Larry L. Biehl			Date: 12/13/2012

SInt16 StandardPopUpMenu (
				DialogPtr							dialogPtr,
				SInt16								promptDialogItem,
				SInt16								popupDialogItem, 
				MenuHandle							popUpMenu, 
				SInt16								currentSelection, 
				SInt16								popUpMenuID)

{
	Rect									popUpBox,
											promptBox;
	
	Handle								theHandle;
	Point									popUpLoc;
	
	SInt16								menuData,
											theType;


	SetPortDialogPort (dialogPtr);
	
			// Get location of pop up box and convert it to global.					
	
	GetDialogItem (dialogPtr, popupDialogItem, &theType, &theHandle, &popUpBox);
	popUpLoc.v = popUpBox.top + 1;
	popUpLoc.h = popUpBox.left;
	LocalToGlobal (&popUpLoc);
	
			// Invert the prompt box.															
	
	if (promptDialogItem > 0)
		{
		GetDialogItem (dialogPtr, promptDialogItem, &theType, &theHandle, &promptBox);
		InvertRect (&promptBox);
		
		}		// end "if (promptDialogItem > 0)"
	
			// Insert the pop up menu and then let user make a selection.			
			
	InsertMenu (popUpMenu, -1);
	
			// Put check by current selection.												
			
	CheckMenuItem (popUpMenu, currentSelection, TRUE);
		
	EraseRect (&popUpBox);
	menuData = PopUpMenuSelect (
							popUpMenu, popUpLoc.v, popUpLoc.h, currentSelection);
		
			// Invert the prompt box again to make normal and remove the			
			// the channel pop up menu.														
			
	InvertRect (&promptBox);
	
			// Remove the check by the "old" selection.									
			
	CheckMenuItem (popUpMenu, currentSelection, FALSE);
	DeleteMenu (popUpMenuID);
	
			// Invalidate the area within the pop up box.								
			
	InvalWindowRect ( GetDialogWindow(dialogPtr), &popUpBox );
	
			// Return the selection that was made.											
	
	return ( LoWord (menuData) );
			
}		// end "StandardPopUpMenu" 
